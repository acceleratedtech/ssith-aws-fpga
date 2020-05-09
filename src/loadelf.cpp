#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gelf.h>

#include "loadelf.h"
#include "util.h"

int debug_pcis_write = 0;
int debug_p2_memcpy = 0;

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
        if (((i + 1) % 1024) == 0)
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
  fprintf(stderr, "IMemory::write addr %x num_bytes %ld ... ", start_addr, num_bytes);
  fpga->ddr_write(start_addr, data, num_bytes);
  fprintf(stderr, "\n");
}


PCIS_DMA_Memory::PCIS_DMA_Memory(AWSP2 *fpga, uint32_t uncached_base_addr, uint32_t cached_base_addr)
    : fpga(fpga), uncached_base_addr(uncached_base_addr), cached_base_addr(cached_base_addr), helper_loaded(0)
{
    pcis_fd = 0;
    pcis_buffer = 0;
}

PCIS_DMA_Memory::~PCIS_DMA_Memory()
{
    if (pcis_buffer) {
        munmap(pcis_buffer, pcis_buffer_size);
        pcis_buffer = 0;
    }
    if (pcis_fd) {
        close(pcis_fd);
        pcis_fd = 0;
    }
}

uint32_t PCIS_DMA_Memory::read32(uint32_t addr)
{
    return fpga->read32(addr);
}

uint64_t PCIS_DMA_Memory::read64(uint32_t addr)
{
    return fpga->read64(addr);
}

void PCIS_DMA_Memory::write32(uint32_t addr, uint32_t data)
{
    fprintf(stderr, "write32 %08x %08x\n", addr, data);
    fpga->write32(addr, data);
}

void PCIS_DMA_Memory::write64(uint32_t addr, uint64_t data)
{
    fprintf(stderr, "write64 %08x %016lx\n", addr, data);
    fpga->write64(addr, data);
}

void PCIS_DMA_Memory::write(uint32_t start_addr, const uint32_t *data32, size_t num_bytes)
{
    if (!helper_loaded)
        load_helper();

    char *data = (char *)data32;
    uint32_t end_addr = start_addr + num_bytes;
    const uint32_t max_chunk_size = 4096;
    uint32_t first_chunk_offset = start_addr & 7;
    uint32_t aligned_start_addr = start_addr - first_chunk_offset;
    uint32_t num_chunks = (num_bytes + first_chunk_offset + (max_chunk_size - 1)) / max_chunk_size;
    uint32_t last_padded_chunk_size = first_chunk_offset + num_bytes - (num_chunks - 1) * max_chunk_size;

    for (uint32_t chunk_idx = 0; chunk_idx < num_chunks; ++chunk_idx) {
        uint32_t chunk_offset = (chunk_idx == 0) ? first_chunk_offset : 0;
        uint32_t chunk_addr = aligned_start_addr + chunk_offset + chunk_idx * max_chunk_size;
        size_t padded_chunk_size = (chunk_idx == num_chunks - 1) ? last_padded_chunk_size : max_chunk_size;
        size_t chunk_size = padded_chunk_size - chunk_offset;
        uint32_t data_offset = chunk_addr - start_addr;

        if (debug_pcis_write) fprintf(stderr, "PCIS_DMA_Memory::write chunk %08x chunk_size %ld / %ld\n", chunk_addr, chunk_size, padded_chunk_size);
        if (pcis_buffer) {
            if (debug_p2_memcpy) fprintf(stderr, "Copying to PCIS DMA buffer ... ");
            memset(pcis_buffer, 0, padded_chunk_size);
            memcpy(pcis_buffer + chunk_offset, data + data_offset, chunk_size);
            if (debug_p2_memcpy) fprintf(stderr, "done\n");
        } else {
            fpga->ddr_write(uncached_base_addr + chunk_offset, (const uint32_t *)&data[data_offset], chunk_size);
        }
        if (debug_p2_memcpy) fprintf(stderr, "PCIS_DMA_Memory::write calling p2_memcpy chunk_addr %08x uncached base + offset %08x chunk_size %ld done addr %08x\n", chunk_addr, uncached_base_addr + chunk_offset, chunk_size, uncached_base_addr + max_chunk_size);
        p2_memcpy(chunk_addr, uncached_base_addr + chunk_offset, chunk_size, uncached_base_addr + max_chunk_size);
        if (debug_p2_memcpy) fprintf(stderr, "done\n");

        if (0) {
            uint32_t last_read_addr = 0;
            uint64_t last_read_data = 0;
            for (size_t i = 0; i < chunk_size; i++) {
                uint32_t byte_addr = chunk_addr + i;
                uint32_t block_offset = byte_addr & 7;
                uint32_t block_addr = byte_addr - block_offset;
                if (block_addr != last_read_addr) {
                    last_read_data = fpga->read64(block_addr);
                    last_read_addr = block_addr;
                }
                uint8_t expected = data[data_offset + i];
                uint8_t actual = (last_read_data >> (block_offset * 8)) & 0xFF;
                if (expected != actual)
                    fprintf(stderr, "Mismatch at %08x expected %08x actual %08x (%ld/%ld)\n", byte_addr, expected, actual, i, chunk_size);
            }
        }

    }
}

#define RISCV_REG_A0 10
#define RISCV_REG_A1 11
#define RISCV_REG_A2 12
#define RISCV_REG_A3 13

// invoke the memcpy helper on the P2 processor to have it copy from the uncached bank to the cached bank
void PCIS_DMA_Memory::p2_memcpy(uint32_t cached_dest, uint32_t uncached_source, uint32_t count, uint32_t done_addr)
{
    fpga->write_gpr(RISCV_REG_A0, cached_dest);
    fpga->write_gpr(RISCV_REG_A1, uncached_source);
    fpga->write_gpr(RISCV_REG_A2, count);
    fpga->write_gpr(RISCV_REG_A3, done_addr);
    fpga->write32(done_addr, 0);
    // dpc
    fpga->write_csr(0x7b1, helper_entry_point);
    if (debug_p2_memcpy) fprintf(stderr, "Calling helper pc %08x\n", helper_entry_point);
    fpga->resume();
    while (1) {
        uint32_t done = fpga->read32(done_addr);
        if (done == count)
            break;
    }
    fpga->halt();
}

// riscv-unknown-linux-gnu-gcc -O -c src/helpers/memcpy.c
// riscv-unknown-linux-gnu-objcopy -O binary memcpy.o memcpy.bin
// hexdump -x memcpy.bin

static uint16_t riscv_memcpy[] = {
    0x07b3, 0x40b0, 0x8b9d, 0x7363, 0x00f6, 0x87b2, 0x0833, 0x00f5,
    0x0c63, 0x0105, 0x872e, 0x0705, 0x4883, 0xfff7, 0x0505, 0x0fa3,
    0xff15, 0x1ae3, 0xfea8, 0x95be, 0x0733, 0x40f6, 0x7793, 0xff87,
    0x97c2, 0x0563, 0x02f8, 0x88ae, 0x8542, 0xb303, 0x0008, 0x0521,
    0x08a1, 0x3c23, 0xfe65, 0x9ae3, 0xfea7, 0x87b3, 0x4107, 0x17e1,
    0x9be1, 0x8513, 0x0087, 0x07b3, 0x00a8, 0x95aa, 0x8b1d, 0x973e,
    0x0a63, 0x00f7, 0x0585, 0xc503, 0xfff5, 0x0785, 0x8fa3, 0xfea7,
    0x1ae3, 0xfef7, 0xe290, 0xa001
};

void PCIS_DMA_Memory::load_helper()
{
    uint8_t buffer[128];
    helper_entry_point = cached_base_addr + 0x10000000 - 128;
    fprintf(stderr, "loading memcpy helper at %08x ... ", helper_entry_point);
    memcpy((char *)buffer, (char *)riscv_memcpy, sizeof(riscv_memcpy));
    for (int i = 0; i < sizeof(buffer); i += 4) {
        fpga->write32(helper_entry_point + i, *(uint32_t *)(buffer + i));
    }
    fprintf(stderr, " done\n");
    helper_loaded = 1;

    fprintf(stderr, "mapping PCIS DMA memory ... ");
    pcis_fd = open("/dev/portal_dma_pcis", O_RDWR);
    if (pcis_fd < 0) {
        fprintf(stderr, "error: opening /dev/portal_dma_pcis %s\n", strerror(errno));
        return;
    }
    pcis_buffer_size = 65536;
    pcis_buffer = (uint8_t *)mmap(0, pcis_buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, pcis_fd, 0);
    fprintf(stderr, "done\n");
}

uint64_t loadElf(IMemory *mem, const char *elf_filename, size_t max_mem_size, AWSP2 *fpga)
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

    Elf64_Phdr phdr;
    uint64_t base_va = 0x80000000;
    uint64_t base_pa = 0x80000000;
    for (int cnt = 0; cnt < ehdr.e_phnum; ++cnt) {
        gelf_getphdr(e, cnt, &phdr);
        fprintf(stderr, "phdr %d type %x flags %x va %08lx pa %08lx\n", cnt, phdr.p_type, phdr.p_flags, phdr.p_vaddr, phdr.p_paddr);
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
            uint32_t section_base_addr = shdr.sh_addr - base_va + base_pa;
            size_t max_addr = (section_base_addr + data->d_size - 1);    // shdr.sh_size + 4;
            fprintf(stderr, "section_base_addr %08x max_addr %08lx\n", section_base_addr, max_addr);

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
                    mem->write(section_base_addr, (uint32_t *)data->d_buf, data->d_size);
                }
            }

            fprintf (stdout, "addr %16x to addr %16lx; size 0x%8lx (= %0ld) bytes\n",
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

            fprintf(stdout, "Parsed\n");
        }
        else {
            fprintf(stdout, "Ignored\n");
        }
    }

    elf_end(e);

    return entry;
}
