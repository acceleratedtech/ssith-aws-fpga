
#include <stdint.h>
#include <stdio.h>

extern "C" {
#include "virtio.h"
}

#include "virtiodevices.h"

void awsp2_set_irq(void *opaque, int irq_num, int level)
{
    fprintf(stderr, "%s: irq_num=%d level=%d\n", __FUNCTION__, irq_num, level);
}

VirtioDevices::VirtioDevices() {
    console = console_init(1);
    mem_map = phys_mem_map_init();
    IRQSignal *irq = (IRQSignal *)mallocz(4 * sizeof(IRQSignal));
    virtio_bus = (VIRTIOBusDef *)mallocz(sizeof(*virtio_bus));
    virtio_bus->mem_map = mem_map;
    virtio_bus->addr = 0x40000000;
    virtio_bus->irq = irq;
    irq_init(irq, awsp2_set_irq, (void *)22, 0);

    virtio_console = virtio_console_init(virtio_bus, console);
    fprintf(stderr, "virtio_console=%p\n", virtio_console);
}

VirtioDevices::~VirtioDevices() {
}

PhysMemoryRange *VirtioDevices::get_phys_mem_range(uint64_t paddr)
{
    return ::get_phys_mem_range(mem_map, paddr);
}
