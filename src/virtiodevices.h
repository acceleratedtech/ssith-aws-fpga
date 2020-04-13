
#pragma once

extern "C" {
#include "virtio.h"
#include "temu.h"
}

class VirtioDevices {
 private:
  CharacterDevice *console;
  PhysMemoryMap *mem_map;
  VIRTIOBusDef *virtio_bus;
  VIRTIODevice *virtio_console;

 public:
  VirtioDevices();
  ~VirtioDevices();
  PhysMemoryRange *get_phys_mem_range(uint64_t paddr);
  void set_dram_buffer(uint8_t *buf);
  void process_io();
};

