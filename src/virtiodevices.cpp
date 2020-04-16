
#include <stdint.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {
#include "virtio.h"
#include "iomem.h"
}

#include "fpga.h"
#include "virtiodevices.h"

static int debug = 0;

static uint8_t *dramBuffer = 0;
extern AWSP2 *fpga;


void awsp2_set_irq(void *opaque, int irq_num, int level)
{
    //fprintf(stderr, "%s: irq_num=%d level=%d\n", __FUNCTION__, irq_num, level);
    if (level)
	fpga->irq_set_levels(0xFFFFFFFF);
    else
	fpga->irq_clear_levels(0xFFFFFFFF);
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
    dramBuffer = buf;
    fpga_register_ram(mem_map, 0x80000000, 0x08000000, 0, buf);
    fpga_register_ram(mem_map, 0xC0000000, 0x08000000, 0, buf);
}


VirtioDevices::VirtioDevices() {
    console = console_init(1);
    mem_map = phys_mem_map_init();
    IRQSignal *irq = (IRQSignal *)mallocz(4 * sizeof(IRQSignal));
    virtio_bus = (VIRTIOBusDef *)mallocz(sizeof(*virtio_bus));
    virtio_bus->mem_map = mem_map;
    virtio_bus->addr = 0x40000000;
    virtio_bus->irq = irq;
    irq_init(virtio_bus->irq, awsp2_set_irq, (void *)22, 0);

    virtio_console = virtio_console_init(virtio_bus, console);

#if 0
    // set up a block device
    virtio_bus->addr += 0x1000;
    virtio_bus->irq += 1;
    irq_init(virtio_bus->irq, awsp2_set_irq, (void *)22, 0);
    block_device = block_device_init("block.bin", BF_MODE_RW);
    fprintf(stderr, "block device %p\n", block_device);
    virtio_block = virtio_block_init(virtio_bus, block_device);
    fprintf(stderr, "virtio block device %p at addr %08lx\n", virtio_block, virtio_bus->addr);
#endif

    // set up a network device
    virtio_bus->addr += 0x1000;
    virtio_bus->irq += 1;
    irq_init(virtio_bus->irq, awsp2_set_irq, (void *)22, 0);
    ethernet_device = slirp_open();
    virtio_net = virtio_net_init(virtio_bus, ethernet_device);
    fprintf(stderr, "ethernet device %p virtio net device %p at addr %08lx\n", ethernet_device, virtio_net, virtio_bus->addr);

}

VirtioDevices::~VirtioDevices() {
}

PhysMemoryRange *VirtioDevices::get_phys_mem_range(uint64_t paddr)
{
    return ::get_phys_mem_range(mem_map, paddr);
}

void VirtioDevices::process_io()
{
    int stdin_fd = 0;
    int fd_max;
    fd_set rfds;

    FD_ZERO(&rfds);

    if (virtio_console && virtio_console_can_write_data(virtio_console)) {
        //STDIODevice *s = console->opaque;
        stdin_fd = 0; //s->stdin_fd;
        FD_SET(stdin_fd, &rfds);
        fd_max = stdin_fd;

#if 0
        if (s->resize_pending) {
            int width, height;
            console_get_size(s, &width, &height);
            virtio_console_resize_event(virtio_console, width, height);
            s->resize_pending = FALSE;
        }
#endif
    }
#if 0
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
#endif
}

int VirtioDevices::has_pending_actions()
{
    return virtio_has_pending_actions(virtio_net);
}

int VirtioDevices::perform_pending_actions()
{
    virtio_perform_pending_actions(virtio_net);
}

uint16_t virtio_read16(VIRTIODevice *s, virtio_phys_addr_t addr)
{
    uint8_t *ptr;
    if (addr & 1)
        return 0; /* unaligned access are not supported */

    uint32_t val32 = fpga->read32(addr & ~3);
    if (addr & 2) {
      return (val32 >> 16) & 0x0000FFFF;
    } else {
      return (val32 >> 0) & 0x0000FFFF;
    }
}

uint32_t virtio_read32(VIRTIODevice *s, virtio_phys_addr_t addr)
{
    uint8_t *ptr;
    if (addr & 1)
        return 0; /* unaligned access are not supported */

    uint32_t val32 = fpga->read32(addr & ~3);
    return val32;
}

void virtio_write16(VIRTIODevice *s, virtio_phys_addr_t addr, uint16_t val)
{
    uint8_t *ptr;
    if (addr & 1)
        return; /* unaligned access are not supported */
    uint32_t val32 = fpga->read32(addr & ~3);
    if (addr & 2) {
      val32 = (val32 & 0x0000FFFF) | (val << 16);
    } else {
      val32 = (val32 & 0xFFFF0000) | (val << 0);
    }
    fpga->write32((addr & ~3), val32);
}

void virtio_write32(VIRTIODevice *s, virtio_phys_addr_t addr, uint32_t val)
{
    uint8_t *ptr;
    if (addr & 3)
        return; /* unaligned access are not supported */
    fpga->write32(addr, val);
}

int virtio_memcpy_from_ram(VIRTIODevice *s, uint8_t *buf, virtio_phys_addr_t addr, int count)
{
    uint8_t *ptr;
    int l;

    virtio_phys_addr_t last_addr = 0;
    uint32_t val32 = 0;
    if (debug) fprintf(stderr, "%s: addr %08lx count %d\n", __FUNCTION__, addr, count);
    if (((addr & 3) == 0) && ((count & 3) == 0)) {
	while (count) {
	    val32 = fpga->read32(addr);
	    memcpy(buf, &val32, 4);
	    addr  += 4;
	    count -= 4;
	    buf   += 4;
	}
    } else {
	while (count) {
	    virtio_phys_addr_t base_addr = addr & ~3;
	    if (last_addr != base_addr) {
		val32 = fpga->read32(base_addr);
		last_addr = base_addr;
	    }
	    uint8_t val = val32 >> ((addr & 3) * 8);
	    if (debug) fprintf(stderr, "addr %08lx offset %d val32 %08x val %02x\n", addr, (int)(addr & 3), val32, val);
	    *buf = val;
	    addr++;
	    count--;
	    buf++;
	}
    }
    return count;
}

int virtio_memcpy_to_ram(VIRTIODevice *s, virtio_phys_addr_t addr, const uint8_t *buf, int count)
{
    uint8_t *ptr;
    int l;

    virtio_phys_addr_t last_addr = 0;
    uint32_t val32 = 0;
    if (debug) fprintf(stderr, "%s: addr %08lx count %d\n", __FUNCTION__, addr, count);
    if (((addr & 3) == 0) && ((count & 3) == 0)) {
	while (count) {
	    memcpy(&val32, buf, 4);
	    fpga->write32(addr, val32);
	    addr  += 4;
	    count -= 4;
	    buf   += 4;
	}
    } else {
	while (count) {
	    virtio_phys_addr_t base_addr = addr & ~3;
	    if (last_addr != base_addr) {
		if (last_addr) {
		    if (debug) fprintf(stderr, "write32 %08lx %08x\n", last_addr, val32);
		    fpga->write32(last_addr, val32);
		}
		val32 = fpga->read32(base_addr);
		last_addr = base_addr;
	    }
	    uint8_t bitpos = ((addr & 3) * 8);
	    uint32_t mask = 0xFFFF << bitpos;
	    uint8_t val = *buf;
	    uint32_t newval32 = (val32 & ~mask) | (val << bitpos);
	    if (debug)
		fprintf(stderr, "addr %08lx offset %d val32 %08x val %02x newval32 %08x\n",
			addr, (int)(addr & 3), val32, val, newval32);
	    val32 = newval32;
	    addr++;
	    count--;
	    buf++;
	}
	if (debug)
	    fprintf(stderr, "write32 %08lx %08x\n", last_addr, val32);
	fpga->write32(last_addr, val32);
    }

    return 0;
}
