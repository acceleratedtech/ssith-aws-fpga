
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
#include <portal.h>
#include "fpga.h"
#include "loadelf.h"
#include "util.h"

#define DEFAULT_DMA_ENABLED 0
#ifdef SIMULATION
#define DEFAULT_XDMA_ENABLED 0
#else
#define DEFAULT_XDMA_ENABLED 1
#endif

// we should read this from the dtb
#define BOOTROM_BASE  (0x70000000)
#define BOOTROM_LIMIT (0x70010000)
#define DEVICETREE_OFFSET (0x20)

const struct option long_options[] = {
    { "block", required_argument, 0, 'B' },
    { "bootrom", required_argument, 0, 'b' },
    { "cpuverbosity",   required_argument, 0, 'v' },
    { "dma",     optional_argument, 0, 'D' },
    { "dtb",     required_argument, 0, 'd' },
    { "elf",     required_argument, 0, 'e' },
    { "entry",   required_argument, 0, 'E' },
    { "help",    no_argument, 0, 'h' },
    { "htif-console",  optional_argument, 0, 'H' },
    { "sleep-seconds", required_argument, 0, 's' },
    { "tv",      no_argument,       0, 'T' },
    { "tun",      required_argument,       0, 't' },
    { "uart",          optional_argument, 0, 'U' },
    { "uart-console",  optional_argument, 0, 'U' },
    { "usemem",  no_argument,       0, 'M' },
    { "virtio-console", optional_argument, 0, 'C' },
    { "xdma",     optional_argument, 0, 'X' },
    { "debug-log", no_argument,     0, 'L' },
    { 0,         0,                 0, 0 }
};

void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [options] [elf ...]\n", name);
    for (const struct option *option = long_options; option->name != 0; option++) {
	if (option->has_arg == required_argument) {
	    fprintf(stderr, "        --%s arg\n", option->name);
	} else if (option->has_arg == optional_argument) {
	    fprintf(stderr, "        --%s [arg]\n", option->name);
	} else {
	    fprintf(stderr, "        --%s\n", option->name);
	}
    }
}

AWSP2 *fpga;

int main(int argc, char * const *argv)
{
    const char *bootrom_filename = 0;
    const char *dtb_filename = 0;
    std::vector<std::string> elf_files;
    int cpuverbosity = 0;
    uint32_t entry = 0;
    int sleep_seconds = 1;
    int usemem = 0;
    const char *tun_iface = 0;
    int tv = 0;
    int enable_virtio_console = 0;
    uint64_t htif_enabled = 0;
    uint64_t uart_enabled = 0;
    int dma_enabled = DEFAULT_DMA_ENABLED;
    int xdma_enabled = DEFAULT_XDMA_ENABLED;
    std::vector<std::string> block_files;
    int debug_log = 0;

    while (1) {
        int option_index = optind ? optind : 1;
        char c = getopt_long(argc, argv, "b:B:Cd:D:e:hH:LMp:s:TvU:X:",
                             long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 'b':
            bootrom_filename = optarg;
            break;
        case 'B':
            block_files.push_back(std::string(optarg));
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
        case 'D':
            if (optarg) {
                dma_enabled = strtoul(optarg, 0, 0);
            } else {
                dma_enabled = 1;
            }
	    //fprintf(stderr, "DMA %d\n", dma_enabled);
            break;
        case 'e':
            elf_files.push_back(std::string(optarg));
            break;
        case 'E':
            entry = strtoul(optarg, 0, 0);
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
        case 't':
            tun_iface = optarg;
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
	    //fprintf(stderr, "UART %d\n", uart_enabled);
            break;
        case 'X':
            if (optarg) {
                xdma_enabled = strtoul(optarg, 0, 0);
            } else {
                xdma_enabled = 1;
            }
	    //fprintf(stderr, "XDMA %d\n", xdma_enabled);
            break;
	case 'L':
	    debug_log = 1;
	    break;
        }
    }

    // Enable a UART by default if no other terminal I/O requested
    if (enable_virtio_console == 0 && htif_enabled == 0 && uart_enabled == 0) {
        uart_enabled = 1;
    }

    setEnableDebugLog(debug_log);

    while (optind < argc) {
        elf_files.push_back(argv[optind++]);
    }
    if (!bootrom_filename && !elf_files.size()) {
        usage(argv[0]);
        return -1;
    }

    // allocate a memory object for Rom
    size_t rom_alloc_sz = 1024*1024;
    uint8_t *romBuffer = (uint8_t *)malloc(rom_alloc_sz);
    debugLog("romBuffer=%lx\n", (long)romBuffer);

    Rom rom = { BOOTROM_BASE, BOOTROM_LIMIT, (uint64_t *)romBuffer };
    fpga = new AWSP2(IfcNames_AWSP2_ResponseH2S, rom, tun_iface);
#ifdef SIMULATION
    fpga->map_simulated_dram();
#else
    if (dma_enabled)
        fpga->map_pcis_dma();
    if (xdma_enabled)
        fpga->open_xdma();
#endif
    fpga->set_htif_enabled(htif_enabled);
    fpga->set_uart_enabled(uart_enabled);

    for (std::string block_file: block_files) {
        fpga->get_virtio_devices().add_virtio_block_device(block_file);
    }

    uint32_t *bootInstrs = (uint32_t *)romBuffer;
    bootInstrs[0] = 0x0000006f; // loop forever
    bootInstrs[1] = 0x0000006f; // loop forever

    // load the ROM code
    if (bootrom_filename)
        copyFile((char *)romBuffer, bootrom_filename, rom_alloc_sz);

    if (enable_virtio_console) {
        debugLog("Enabling virtio console\n");
        fpga->get_virtio_devices().add_virtio_console_device();
    }

    if (dtb_filename) {
        copyFile((char *)romBuffer + DEVICETREE_OFFSET, dtb_filename, rom_alloc_sz - 0x10);
    }

    // Unblock memory accesses in the SoC.
    // This has to be done before attempting to read/write memory through DM System Bus
    // or deadlock will ensue.
    fpga->memory_ready();

    debugLog("asserting haltreq\n");
    fpga->halt();
    fpga->capture_tv_info(0);

    debugLog("dmi state machine status %d\n", fpga->dmi_status());

    bool first_elf = true;
    for (std::string elf_file: elf_files) {
        uint64_t elf_entry = loadElf(fpga, elf_file.c_str(), 0x40000000, first_elf);
        if (first_elf) {
            first_elf = false;
            debugLog("elf_entry=%08lx\n", elf_entry);

            if (!entry)
                entry = elf_entry;
        }
    }

    // update the dpc
    debugLog("setting pc val %08x\n", entry);
    fpga->write_csr(0x7b1, entry);
    debugLog("reading pc val %08lx\n", fpga->read_csr(0x7b1));

    // for loading linux, set pointer to devicetree
    fpga->write_gpr(10, 0);
    fpga->write_gpr(11, BOOTROM_BASE + DEVICETREE_OFFSET);

    fpga->capture_tv_info(tv);
    // and resume
    fpga->get_virtio_devices().start();
    fpga->resume();

    while (1) {

        // event processing is in the other thread

        fpga->process_io();

        if (0) {
            fpga->halt();
            uint64_t dpc = fpga->read_csr(0x7b1);
            uint64_t ra = fpga->read_gpr(1);
            uint64_t stvec = fpga->read_csr(0x105);
            fprintf(stderr, "pc %08lx ra %08lx stvec %08lx irq %08x\n", dpc, ra, stvec, fpga->read_irq_status());
            if (0 && !tv && dpc >= 0x8200210a) {
                tv = 1;
                fpga->capture_tv_info(tv);
            }
            if (dpc == 0x1000 || dpc == 0x80003168) {
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
