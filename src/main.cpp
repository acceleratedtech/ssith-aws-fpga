
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <semaphore.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include "GeneratedTypes.h"
#include "dmaManager.h"
#include <portal.h>
#include "fpga.h"
#include "loadelf.h"

using namespace std;

int copyFile(char *buffer, const char *filename, size_t buffer_size)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: failed to open file %s: %s\n", filename, strerror(errno));
        return -errno;
    }
    size_t bytes_copied = 0;
    do {
        char readbuf[4096];
        size_t readsize = sizeof(readbuf);
        if (readsize > (buffer_size - bytes_copied))
            readsize = (buffer_size - bytes_copied);
        int bytes_read = read(fd, readbuf, readsize);
        if (readsize <= 0) {
            fprintf(stderr, "Error: failed to read file %s at offset %ld: %s\n", filename, bytes_copied, strerror(errno));
            return -errno;
        }
        if (!bytes_read)
            break;
        memcpy(buffer + bytes_copied, readbuf, bytes_read);
        bytes_copied += bytes_read;
        fprintf(stderr, "Copied %d bytes %ld bytes total readsize %ld\n", bytes_read, bytes_copied, readsize);
    } while (bytes_copied < buffer_size);
    close(fd);
    fprintf(stderr, "Read %ld bytes from %s\n", bytes_copied, filename);
    return bytes_copied;
}

const struct option long_options[] = {
    { "bootrom", required_argument, 0, 'b' },
    { "cpuverbosity",   required_argument, 0, 'v' },
    { "block", required_argument, 0, 'B' },
    { "virtio-console", optional_argument, 0, 'C' },
    { "htif-console",  optional_argument, 0, 'H' },
    { "uart-console",  optional_argument, 0, 'U' },
    { "uart",          required_argument, 0, 'U' },
    { "dtb",     required_argument, 0, 'd' },
    { "elf",     required_argument, 0, 'e' },
    { "entry",   required_argument, 0, 'E' },
    { "flash",   required_argument, 0, 'f' },
    { "sleep-seconds", required_argument, 0, 's' },
    { "tv",      no_argument,       0, 'T' },
    { "usemem",  no_argument,       0, 'M' },
    { 0,         0,                 0, 0 }
};

void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [-b bootrombin] [--bootrom bootrombin] [--flash flashbin] [--elf elf] [ -e elf] [elf] [--cpuverbosity n] [--sleep-seconds n]\n", name);
}

AWSP2 *fpga;

int main(int argc, char * const *argv)
{
    const char *bootrom_filename = 0;
    const char *dtb_filename = 0;
    const char *elf_filename = 0;
    const char *flash_filename = 0;
    int cpuverbosity = 0;
    uint32_t entry = 0;
    int sleep_seconds = 1;
    int usemem = 0;
    int tv = 0;
    int enable_virtio_console = 0;
    uint64_t htif_enabled = 0;
    uint64_t uart_enabled = 0;
    std::vector<string> block_files;

    while (1) {
        int option_index = optind ? optind : 1;
        char c = getopt_long(argc, argv, "b:B:Cd:e:hH:Ms:TvU:",
                             long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'b':
            bootrom_filename = optarg;
            break;
        case 'B':
            block_files.push_back(string(optarg));
            break;
        case 'C':
            if (optarg) {
                enable_virtio_console = strtoul(optarg, 0, 0);
            } else {
                enable_virtio_console = 1;
            }
            break;
        case 'd':
            dtb_filename = optarg;
            break;
        case 'e':
            elf_filename = optarg;
            break;
        case 'E':
            entry = strtoul(optarg, 0, 0);
            break;
        case 'f':
            flash_filename = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return 2;
        case 'H':
            if (optarg) {
                htif_enabled = strtoul(optarg, 0, 0);
            } else {
                htif_enabled = 1;
            }
            break;
        case 'M':
            usemem = 1;
            break;
        case 's':
            sleep_seconds = strtoul(optarg, 0, 0);
            break;
        case 'T':
            tv = 1;
            break;
        case 'v':
            cpuverbosity = strtoul(optarg, 0, 0);
            break;
        case 'U':
            if (optarg) {
                uart_enabled = strtoul(optarg, 0, 0);
            } else {
                uart_enabled = 1;
            }
            break;
        }
    }

    if (optind < argc) {
        elf_filename = argv[optind];
    }
    if (!bootrom_filename && !elf_filename) {
        usage(argv[0]);
        return -1;
    }

    DmaManager *dma = platformInit();

    // allocate a shared memory object for Rom
    size_t rom_alloc_sz = 1024*1024;
    int romObject = portalAlloc(rom_alloc_sz, 0);
    uint8_t *romBuffer = (uint8_t *)portalMmap(romObject, rom_alloc_sz);
    fprintf(stderr, "romBuffer=%lx\n", (long)romBuffer);

    // allocate a shared memory object for Flash
    size_t flash_alloc_sz = 0x08000000;
    int flashObject = portalAlloc(flash_alloc_sz, 0);
    uint8_t *flashBuffer = (uint8_t *)portalMmap(flashObject, flash_alloc_sz);
    fprintf(stderr, "flashBuffer=%lx\n", (long)flashBuffer);

    size_t dram_alloc_sz = 1024*1024*1024;
    int dramObject = portalAlloc(dram_alloc_sz, 0);
    uint8_t *dramBuffer = (uint8_t *)portalMmap(dramObject, dram_alloc_sz);
    memset(dramBuffer, 0x0, flash_alloc_sz);
    fprintf(stderr, "dramBuffer=%lx\n", (long)dramBuffer);

    Rom rom = { 0x00001000, 0x00010000, (uint64_t *)romBuffer };
    fpga = new AWSP2(IfcNames_AWSP2_ResponseH2S, rom);
    fpga->set_dram_buffer(dramBuffer);
    fpga->set_uart_enabled(uart_enabled);

    for (string block_file: block_files) {
        fpga->get_virtio_devices().add_virtio_block_device(block_file);
    }

    if (enable_virtio_console) {
        fprintf(stderr, "Enabling virtio console\n");
        fpga->get_virtio_devices().add_virtio_console_device();
    }

    // load the ROM code into Flash
    if (bootrom_filename)
        copyFile((char *)romBuffer, bootrom_filename, rom_alloc_sz);

    uint32_t *bootInstrs = (uint32_t *)romBuffer;
    bootInstrs[0] = 0x0000006f; // loop forever
    bootInstrs[1] = 0x0000006f; // loop forever

    if (dtb_filename)
        copyFile((char *)romBuffer + 0x10, dtb_filename, rom_alloc_sz - 0x10);

    // where is this coming from?
    if (flash_filename)
        copyFile((char *)flashBuffer, flash_filename, flash_alloc_sz);

    if (1) {
        // register the DRAM memory object with the SoC (and program the MMU)
        int objId = dma->reference(dramObject);
        fprintf(stderr, "DRAM objId %d\n", objId);
        fpga->register_region(8, objId);
    }

    // Unblock memory accesses in the SoC.
    // This has to be done before attempting to read/write memory through DM System Bus
    // or deadlock will ensue.
    fpga->memory_ready();

    fprintf(stderr, "asserting haltreq\n");
    fpga->halt();
    fpga->capture_tv_info(0);

    fprintf(stderr, "dmi state machine status %d\n", fpga->dmi_status());

    // load the app code into DRAM
    P2Memory mem(dramBuffer);
    AWSP2_Memory fpgamem(fpga);
    if (usemem) {
        fprintf(stderr, "Using shared memory loader\n");
    }
    IMemory *memifc = usemem ? static_cast<IMemory *>(&mem) : static_cast<IMemory *>(&fpgamem);
    uint64_t elf_entry = loadElf(memifc, elf_filename, dram_alloc_sz, fpga);
    fprintf(stderr, "elf_entry=%08lx\n", elf_entry);

    if (!entry)
        entry = elf_entry;

    // update the dpc
    fprintf(stderr, "setting pc val %08x\n", entry);
    fpga->write_csr(0x7b1, entry);
    fprintf(stderr, "reading pc val %08lx\n", fpga->read_csr(0x7b1));

    // for loading linux, set pointer to devicetree
    fpga->write_gpr(10, 0);
    fpga->write_gpr(11, 0x00001010);

    fpga->capture_tv_info(tv);
    // and resume
    fpga->resume();

    while (1) {

        // event processing is in the other thread

        fpga->process_io();

        if (0) {
            fpga->halt();
            uint64_t dpc = fpga->read_csr(0x7b1);
            uint64_t ra = fpga->read_gpr(1);
            uint64_t stvec = fpga->read_csr(0x105);
            fprintf(stderr, "pc %08lx ra %08lx stvec %08lx\n", dpc, ra, stvec);
            if (0 && !tv && dpc >= 0x8200210a) {
                tv = 1;
                fpga->capture_tv_info(tv);
            }
            if (dpc == 0x1000 || dpc == 0x80003168 || dpc == 0xffffffe000000154ull) {
                for (int i = 0; i < 32; i++) {
                    fprintf(stderr, "reg %d val %08lx\n", i, fpga->read_gpr(i));
                }

                fprintf(stderr, "mepc   %08lx\n", fpga->read_csr(0x341));
                fprintf(stderr, "mcause %08lx\n", fpga->read_csr(0x342));
                fprintf(stderr, "mtval  %08lx\n", fpga->read_csr(0x343));

                break;
            }
            fpga->resume();
        }

        sleep(sleep_seconds);
    }
    return 0;
}
