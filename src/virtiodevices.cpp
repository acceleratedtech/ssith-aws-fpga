
#include <stdint.h>
#include <stdio.h>

extern "C" {
#include "virtio.h"
}

#include "virtiodevices.h"


VirtioDevices::VirtioDevices() {
  console = console_init(1);
  mem_map = phys_mem_map_init();
  virtio_bus = (VIRTIOBusDef *)mallocz(sizeof(*virtio_bus));
  virtio_bus->mem_map = mem_map;
  virtio_bus->addr = 0x40000000;

  virtio_console = virtio_console_init(virtio_bus, console);
  fprintf(stderr, "virtio_console=%08p\n", virtio_console);
}

VirtioDevices::~VirtioDevices() {
}

PhysMemoryRange *VirtioDevices::get_phys_mem_range(uint64_t paddr)
{
  return ::get_phys_mem_range(mem_map, paddr);
}
