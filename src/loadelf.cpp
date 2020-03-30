#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gelf.h>
#include "loadelf.h"

void IMemory::read(uint32_t start_addr, uint32_t *data, size_t num_bytes)
{
    for (size_t i = 0; i < num_bytes; i += 4) {
	data[i / 4] = read32(start_addr + i);
    }
}

void IMemory::write(uint32_t start_addr, const uint32_t *data, size_t num_bytes)
{
    fprintf(stderr, "IMemory::write addr %x num_bytes %ld\n", start_addr, num_bytes);
    for (size_t i = 0; i < num_bytes; i += 4) {
	fprintf(stderr, ".");
	write32(start_addr + i, data[i / 4]);
    }
    fprintf(stderr, "\n");
}

P2Memory::P2Memory(uint8_t *buffer)
    : buffer(buffer) {
}

uint32_t P2Memory::read32(uint32_t addr)
{
    addr &= ~0xC0000000;
    uint32_t *intbuffer = (uint32_t *)buffer;
    return intbuffer[addr / 4];
}

void P2Memory::write32(uint32_t addr, uint32_t data)
{
    addr &= ~0xC0000000;
    uint32_t *intbuffer = (uint32_t *)buffer;
    intbuffer[addr / 4] = data;
}

uint64_t P2Memory::read64(uint32_t addr)
{
    addr &= ~0xC0000000;
    uint64_t *longbuffer = (uint64_t *)buffer;
    return longbuffer[addr / 8];
}

void P2Memory::write64(uint32_t addr, uint64_t data)
{
    addr &= ~0xC0000000;
    uint64_t *longbuffer = (uint64_t *)buffer;
    longbuffer[addr / 8] = data;
}

AWSP2_Memory::AWSP2_Memory(AWSP2 *fpga) : fpga(fpga) {
}
uint32_t AWSP2_Memory::read32(uint32_t addr)
{
    return fpga->read32(addr);
}

uint64_t AWSP2_Memory::read64(uint32_t addr)
{
    return fpga->read64(addr);
}

void AWSP2_Memory::write32(uint32_t addr, uint32_t data)
{
    fprintf(stderr, "write32 %08x %08x\n", addr, data);
    fpga->write32(addr, data);
}

void AWSP2_Memory::write64(uint32_t addr, uint64_t data)
{
    fprintf(stderr, "write64 %08x %016lx\n", addr, data);
    fpga->write64(addr, data);
}

void AWSP2_Memory::write(uint32_t start_addr, const uint32_t *data, size_t num_bytes)
{
    fprintf(stderr, "IMemory::write addr %x num_bytes %ld\n", start_addr, num_bytes);
    fpga->dmi_write(DM_SBCS_REG, SBCS_SBACCESS32 | SBCS_SBAUTOINCREMENT);
    fpga->sbcs_wait();
    fpga->dmi_write(DM_SBADDRESS0_REG, start_addr);
    fpga->sbcs_wait();
    for (size_t i = 0; i < num_bytes; i += 4) {
        fpga->sbcs_wait();
        fpga->dmi_write(DM_SBDATA0_REG, data[i / 4]);
    }
    fpga->sbcs_wait();
    fpga->dmi_write(DM_SBCS_REG, 0);
    fprintf(stderr, "\n");
}


uint64_t loadElf(IMemory *mem, const char *elf_filename, size_t max_mem_size, uint64_t *tohost_address)
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
        fprintf(stdout, "loadElf: %s is a 32-bit ELF file\n", elf_filename);
        //bitwidth = 32;
    }
    else if (gelf_getclass(e) == ELFCLASS64) {
        fprintf(stdout, "loadElf: %s is a 64-bit ELF file\n", elf_filename);
        //bitwidth = 64;
        Elf64_Ehdr *ehdr64 = elf64_getehdr(e);
        fprintf(stderr, "loadElf: entry point %08lx\n", ehdr64->e_entry);
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

    // Grab the string section index
    size_t shstrndx = ehdr.e_shstrndx;
    Elf_Scn  *scn   = 0;
    while ((scn = elf_nextscn(e,scn)) != NULL) {
        // get the header information for this section
        GElf_Shdr shdr;
        gelf_getshdr(scn, & shdr);

        char *sec_name = elf_strptr(e, shstrndx, shdr.sh_name);
        fprintf(stdout, "Section %-16s: ", sec_name);

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
            size_t max_addr = (shdr.sh_addr + data->d_size - 1) & ~0xC0000000ul;    // shdr.sh_size + 4;

            if (strcmp(sec_name, ".tohost") == 0 || strcmp(sec_name, ".htif") == 0) {
                if (tohost_address != 0) {
                    *tohost_address = shdr.sh_addr;
                }
            } else {
                if (max_addr >= max_mem_size) {
                    fprintf(stdout, "INTERNAL ERROR: max_addr (0x%0lx) > buffer size (0x%0lx)\n",
                            max_addr, max_mem_size);
                    fprintf(stdout, "    Please increase the #define in this program, recompile, and run again\n");
                    fprintf(stdout, "    Abandoning this run\n");
                    //exit(1);
                }

                if (shdr.sh_type != SHT_NOBITS) {
		    mem->write(shdr.sh_addr, (uint32_t *)data->d_buf, data->d_size);
                }
            }

            fprintf (stdout, "addr %16lx to addr %16lx; size 0x%8lx (= %0ld) bytes\n",
                     shdr.sh_addr, shdr.sh_addr + data->d_size, data->d_size, data->d_size);

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

        }
        else {
            fprintf(stdout, "Ignored\n");
        }
    }

    elf_end(e);

    return entry;
}
