#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gelf.h>

#include "fpga.h"
#include "loadelf.h"
#include "util.h"

int debug_pcis_write = 0;

uint64_t loadElf(AWSP2 *fpga, const char *elf_filename, size_t max_mem_size)
{
    // Verify the elf library version
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ERROR: loadElf: Failed to initialize the libelfg library!\n");
        exit(1);
    }

    int fd = open(elf_filename, O_RDONLY);
    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    uint64_t entry = 0;

    // Verify that the file is an ELF file
    if (elf_kind(e) != ELF_K_ELF) {
        elf_end(e);
        fprintf(stderr, "ERROR: loadElf: specified file '%s' is not an ELF file!\n", elf_filename);
        exit(1);
    }

    // Get the ELF header
    GElf_Ehdr ehdr;
    if (gelf_getehdr(e, & ehdr) == NULL) {
        elf_end(e);
        fprintf(stderr, "ERROR: loadElf: get_getehdr() failed: %s\n", elf_errmsg(-1));
        exit(1);
    }

    // Is this a 32b or 64 ELF?
    if (gelf_getclass(e) == ELFCLASS32) {
        debugLog("loadElf: %s is a 32-bit ELF file\n", elf_filename);
        //bitwidth = 32;
    }
    else if (gelf_getclass(e) == ELFCLASS64) {
        debugLog("loadElf: %s is a 64-bit ELF file\n", elf_filename);
        //bitwidth = 64;
        Elf64_Ehdr *ehdr64 = elf64_getehdr(e);
        debugLog("loadElf: entry point %08lx\n", ehdr64->e_entry);
        entry = ehdr64->e_entry;
    }

    // Verify we are dealing with a RISC-V ELF
    if (ehdr.e_machine != 243) { // EM_RISCV is not defined, but this returns 243 when used with a valid elf file.
        elf_end(e);
        fprintf(stderr, "ERROR: loadElf: %s is not a RISC-V ELF file\n", elf_filename);
        exit(1);
    }
    // Verify we are dealing with a little endian ELF
    if (ehdr.e_ident[EI_DATA] != ELFDATA2LSB) {
        elf_end(e);
        fprintf(stderr,
                 "ERROR: loadElf: %s is a big-endian 64-bit RISC-V executable which is not supported\n",
                 elf_filename);
        exit(1);
    }

    Elf64_Phdr phdr;
    uint64_t base_va = 0x80000000;
    uint64_t base_pa = 0x80000000;
    for (int cnt = 0; cnt < ehdr.e_phnum; ++cnt) {
        gelf_getphdr(e, cnt, &phdr);
        debugLog("phdr %d type %x flags %x va %08lx pa %08lx\n", cnt, phdr.p_type, phdr.p_flags, phdr.p_vaddr, phdr.p_paddr);
        // phdr 0 type 1 flags 7 va ffffffc000000000 pa c0200000
        // phdr 1 type 2 flags 6 va ffffffc001b2c038 pa c1d2c038
        // phdr 2 type 6474e551 flags 6 va 00000000 pa 00000000
        // phdr 3 type 4 flags 4 va ffffffc001aa5098 pa c1ca5098

        if (phdr.p_type == PT_LOAD) {
            base_va = phdr.p_vaddr;
            base_pa = phdr.p_paddr;
        }
    }

    // Grab the string section index
    size_t shstrndx = ehdr.e_shstrndx;
    Elf_Scn  *scn   = 0;
    while ((scn = elf_nextscn(e,scn)) != NULL) {
        // get the header information for this section
        GElf_Shdr shdr;
        gelf_getshdr(scn, & shdr);

        char *sec_name = elf_strptr(e, shstrndx, shdr.sh_name);
        debugLog("Section %-16s: ", sec_name);

        // If we find a code/data section, load it into the model
        if (   ((shdr.sh_type == SHT_PROGBITS)
                || (shdr.sh_type == SHT_NOBITS)
                || (shdr.sh_type == SHT_INIT_ARRAY)
                || (shdr.sh_type == SHT_FINI_ARRAY))
            && ((shdr.sh_flags & SHF_WRITE)
                || (shdr.sh_flags & SHF_ALLOC)
                || (shdr.sh_flags & SHF_EXECINSTR))) {

            Elf_Data *data = 0;
            data = elf_getdata (scn, data);

            // n_initialized += data->d_size;
            uint32_t section_base_addr = shdr.sh_addr - base_va + base_pa;
            size_t max_addr = (section_base_addr + data->d_size - 1);    // shdr.sh_size + 4;

            if (strcmp(sec_name, ".htif") == 0) {
                fpga->set_htif_base_addr(section_base_addr);
            } else if (strcmp(sec_name, ".tohost") == 0) {
                fpga->set_tohost_addr(section_base_addr);
            } else if (strcmp(sec_name, ".fromhost") == 0) {
                fpga->set_fromhost_addr(section_base_addr);
            } else {
                if (data->d_size > max_mem_size) {
                    fprintf(stdout, "INTERNAL ERROR: section size (0x%0lx) > buffer size (0x%0lx)\n",
                            data->d_size, max_mem_size);
                    fprintf(stdout, "    Please increase the #define in this program, recompile, and run again\n");
                    fprintf(stdout, "    Abandoning this run\n");
                    exit(1);
                }

                if (shdr.sh_type != SHT_NOBITS) {
                    fpga->write(section_base_addr, (uint8_t *)data->d_buf, data->d_size);
                }
            }

            debugLog("addr %16x to addr %16lx; size 0x%8lx (= %0ld) bytes\n",
                     section_base_addr, section_base_addr + data->d_size, data->d_size, data->d_size);

        }

        // If we find the symbol table, search for symbols of interest
        else if (shdr.sh_type == SHT_SYMTAB) {

            // Get the section data
            Elf_Data *data = 0;
            data = elf_getdata(scn, data);

            // Get the number of symbols in this section
            int symbols = shdr.sh_size / shdr.sh_entsize;

            // search for the uart_default symbols we need to potentially modify.
            GElf_Sym sym;
            for (int i = 0; i < symbols; ++i) {
                gelf_getsym(data, i, &sym);

                // get the name of the symbol
                //char *name = elf_strptr(e, shdr.sh_link, sym.st_name);

            }

            debugLog("Parsed\n");
        }
        else {
            debugLog("Ignored\n");
        }
    }

    elf_end(e);

    return entry;
}
