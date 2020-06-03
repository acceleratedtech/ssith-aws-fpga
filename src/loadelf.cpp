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

__attribute__((aligned(4096)))
static uint8_t zeroes[4096];

uint64_t loadElf(AWSP2 *fpga, const char *elf_filename, size_t max_mem_size)
{
    // Verify the elf library version
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ERROR: loadElf: Failed to initialize the libelfg library!\n");
        exit(1);
    }

    int fd = open(elf_filename, O_RDONLY);
    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    uint64_t entry_vaddr = 0;

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
        entry_vaddr = ehdr64->e_entry;
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


    uint64_t htif_vaddr = 0;
    uint64_t tohost_vaddr = 0;
    uint64_t fromhost_vaddr = 0;
    // Grab the string section index
    size_t shstrndx = ehdr.e_shstrndx;
    Elf_Scn  *scn   = 0;
    while ((scn = elf_nextscn(e,scn)) != NULL) {
        // get the header information for this section
        GElf_Shdr shdr;
        gelf_getshdr(scn, &shdr);

        char *sec_name = elf_strptr(e, shstrndx, shdr.sh_name);
        debugLog("Section %-16s: ", sec_name);
        debugLog("addr %16lx to addr %16lx; size 0x%8lx (= %0ld) bytes\n",
                 shdr.sh_addr, shdr.sh_addr + shdr.sh_size, shdr.sh_size, shdr.sh_size);

        // If we find the symbol table, search for symbols of interest
        if (shdr.sh_type == SHT_SYMTAB) {

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

            continue;
        }

        Elf_Data *data = 0;
        data = elf_getdata (scn, data);

        if (strcmp(sec_name, ".htif") == 0) {
            htif_vaddr = shdr.sh_addr;
        } else if (strcmp(sec_name, ".tohost") == 0) {
            tohost_vaddr = shdr.sh_addr;
        } else if (strcmp(sec_name, ".fromhost") == 0) {
            fromhost_vaddr = shdr.sh_addr;
        }
    }

    Elf64_Phdr phdr;
    uint8_t *rawdata = (uint8_t *)elf_rawfile(e, NULL);
    uint64_t entry_paddr = entry_vaddr;
    uint64_t htif_paddr = htif_vaddr;
    uint64_t tohost_paddr = tohost_vaddr;
    uint64_t fromhost_paddr = fromhost_vaddr;
    for (int cnt = 0; cnt < ehdr.e_phnum; ++cnt) {
        gelf_getphdr(e, cnt, &phdr);
        debugLog("phdr %d type %x flags %x va %08lx pa %08lx\n", cnt, phdr.p_type, phdr.p_flags, phdr.p_vaddr, phdr.p_paddr);

        if (phdr.p_type != PT_LOAD) continue;

        fpga->write(phdr.p_paddr, rawdata + phdr.p_offset, phdr.p_filesz);
        size_t zero_paddr = phdr.p_paddr + phdr.p_filesz;
        size_t zero_len = phdr.p_memsz - phdr.p_filesz;
        while (zero_len > 0) {
            size_t block_len = std::min(zero_len, sizeof(zeroes));
            fpga->write(zero_paddr, zeroes, block_len);
            zero_paddr += block_len;
            zero_len -= block_len;
        }

        if (phdr.p_vaddr <= entry_vaddr && entry_vaddr < phdr.p_vaddr + phdr.p_memsz) {
            entry_paddr = phdr.p_paddr + (entry_vaddr - phdr.p_vaddr);
        }
        if (phdr.p_vaddr <= htif_vaddr && htif_vaddr < phdr.p_vaddr + phdr.p_memsz) {
            htif_paddr = phdr.p_paddr + (htif_vaddr - phdr.p_vaddr);
        }
        if (phdr.p_vaddr <= tohost_vaddr && tohost_vaddr < phdr.p_vaddr + phdr.p_memsz) {
            tohost_paddr = phdr.p_paddr + (tohost_vaddr - phdr.p_vaddr);
        }
        if (phdr.p_vaddr <= fromhost_vaddr && fromhost_vaddr < phdr.p_vaddr + phdr.p_memsz) {
            fromhost_paddr = phdr.p_paddr + (fromhost_vaddr - phdr.p_vaddr);
        }
    }

    elf_end(e);

    if (htif_paddr) {
        fpga->set_htif_base_addr(htif_paddr);
    }
    if (tohost_paddr) {
        fpga->set_tohost_addr(tohost_paddr);
    }
    if (fromhost_paddr) {
        fpga->set_fromhost_addr(fromhost_paddr);
    }

    return entry_paddr;
}
