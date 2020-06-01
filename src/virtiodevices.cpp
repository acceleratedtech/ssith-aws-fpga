
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

extern "C" {
#include "virtio.h"
#include "iomem.h"
}

#include "fpga.h"
#include "virtiodevices.h"

static int debug = 0;

extern AWSP2 *fpga;


void awsp2_set_irq(void *opaque, int irq_num, int level)
{
    if (debug) fprintf(stderr, "%s: irq_num=%d level=%d\n", __FUNCTION__, irq_num, level);
    if (level)
	fpga->irq_set_levels(1 << irq_num);
    else
	fpga->irq_clear_levels(1 << irq_num);
}

static PhysMemoryRange *fpga_register_ram(PhysMemoryMap *s, uint64_t addr,
                                          uint64_t size, int devram_flags, uint8_t *phys_mem)
{
    PhysMemoryRange *pr;

    pr = register_ram_entry(s, addr, size, devram_flags);

    pr->phys_mem = phys_mem;
    if (!pr->phys_mem) {
        fprintf(stderr, "Could not allocate VM memory\n");
        exit(1);
    }
    return pr;
}


void VirtioDevices::set_dram_buffer(uint8_t *buf) {
    fpga_register_ram(mem_map, 0x80000000, 0x40000000, 0, buf);
    fpga_register_ram(mem_map, 0xC0000000, 0x40000000, 0, buf + 0x40000000);
}

void VirtioDevices::xdma_init(int c2h_fd, int h2c_fd) {
    virtio_xdma_init(c2h_fd, h2c_fd);
}

VirtioDevices::VirtioDevices(int first_irq_num, const char *tun_ifname)
  : tun_ifname(tun_ifname) {
    mem_map = phys_mem_map_init();
    irq = (IRQSignal *)mallocz(32 * sizeof(IRQSignal));
    irq_num = first_irq_num;
    virtio_bus = (VIRTIOBusDef *)mallocz(sizeof(*virtio_bus));
    virtio_bus->mem_map = mem_map;
    virtio_bus->addr = 0x40000000;

    for (int i = 0; i < 32; i++)
        irq_init(&irq[i], awsp2_set_irq, (void *)22, i);

    // set up a network device
    virtio_bus->irq = &irq[irq_num++];
    ethernet_device = tun_ifname ? tun_open(tun_ifname) : slirp_open();
    virtio_net = virtio_net_init(virtio_bus, ethernet_device);
    fprintf(stderr, "ethernet device %p virtio net device %p at addr %08lx\n", ethernet_device, virtio_net, virtio_bus->addr);

    // set up an entropy device
    virtio_bus->addr += 0x1000;
    virtio_bus->irq = &irq[irq_num++];
    virtio_entropy = virtio_entropy_init(virtio_bus);
    fprintf(stderr, "virtio entropy device %p at addr %08lx\n", virtio_entropy, virtio_bus->addr);
}

VirtioDevices::~VirtioDevices() {
}

void VirtioDevices::add_virtio_block_device(std::string filename)
{
    // set up a block device
    virtio_bus->addr += 0x1000;
    virtio_bus->irq = &irq[irq_num++];
    block_device = block_device_init(filename.c_str(), BF_MODE_RW);
    fprintf(stderr, "block device %s (%p)\n", filename.c_str(), block_device);
    virtio_block = virtio_block_init(virtio_bus, block_device);
    fprintf(stderr, "virtio block device %p at addr %08lx\n", virtio_block, virtio_bus->addr);

}

void VirtioDevices::add_virtio_console_device()
{
    console = console_init(1);
    virtio_bus->addr += 0x1000;
    virtio_bus->irq = &irq[irq_num++];
    virtio_console = virtio_console_init(virtio_bus, console);
}

PhysMemoryRange *VirtioDevices::get_phys_mem_range(uint64_t paddr)
{
    return ::get_phys_mem_range(mem_map, paddr);
}

uint8_t *VirtioDevices::phys_mem_get_ram_ptr(uint64_t paddr, BOOL is_rw)
{
    return ::phys_mem_get_ram_ptr(mem_map, paddr, is_rw);
}

void VirtioDevices::process_io()
{
    int stdin_fd;
    int fd_max = -1;
    fd_set rfds, wfds, efds;
    int delay = 10; // ms
    struct timeval tv;
    int stop_fd = stop_pipe[0];

    for (;;) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        FD_SET(stop_fd, &rfds);
        fd_max = stop_fd;

        if (virtio_console && virtio_console_can_write_data(virtio_console)) {
            //STDIODevice *s = console->opaque;
            stdin_fd = STDIN_FILENO; //s->stdin_fd;
            FD_SET(stdin_fd, &rfds);
            fd_max = std::max(stdin_fd, fd_max);

#if 0
            if (s->resize_pending) {
                int width, height;
                console_get_size(s, &width, &height);
                virtio_console_resize_event(virtio_console, width, height);
                s->resize_pending = FALSE;
            }
#endif
        }
        if (ethernet_device) {
            ethernet_device->select_fill(ethernet_device, &fd_max, &rfds, &wfds, &efds, &delay);
        }
        tv.tv_sec = delay / 1000;
        tv.tv_usec = (delay % 1000) * 1000;
        int ret = select(fd_max + 1, &rfds, &wfds, &efds, &tv);
        if (FD_ISSET(stop_fd, &rfds)) {
            close(stop_fd);
            return;
        }

        if (ethernet_device) {
            ethernet_device->select_poll(ethernet_device, &rfds, &wfds, &efds, ret);
        }

        if (virtio_console && FD_ISSET(stdin_fd, &rfds)) {
            uint8_t buf[128];
            int ret, len;
            len = virtio_console_get_write_len(virtio_console);
            len = min_int(len, sizeof(buf));
            ret = console->read_data(console->opaque, buf, len);
            if (ret > 0) {
                virtio_console_write_data(virtio_console, buf, ret);
            }
        }
    }
}

void *VirtioDevices::process_io_thread(void *opaque)
{
    ((VirtioDevices *)opaque)->process_io();
    return NULL;
}

void VirtioDevices::start()
{
    VIRTIODevice *ps[4];
    int n = 0;
#define ADD_DEVICE(s) if (s) ps[n++] = s
    ADD_DEVICE(virtio_net);
    ADD_DEVICE(virtio_entropy);
    ADD_DEVICE(virtio_block);
    ADD_DEVICE(virtio_console);
#undef ADD_DEVICE
    virtio_start_pending_notify_thread(n, ps);

    pipe(stop_pipe);
    fcntl(stop_pipe[1], F_SETFL, O_NONBLOCK);
    pthread_t thread;
    pthread_create(&thread, NULL, &process_io_thread, this);
    pthread_detach(thread);
}

void VirtioDevices::stop()
{
    virtio_stop_pending_notify_thread();
    char dummy = 'X';
    write(stop_pipe[1], &dummy, sizeof(dummy));
    close(stop_pipe[1]);
}
