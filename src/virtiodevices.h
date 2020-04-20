
#pragma once

#include <string>

extern "C" {
#include "virtio.h"
#include "temu.h"
}

class VirtioDevices {
 private:
  BlockDevice *block_device;
  CharacterDevice *console;
  EthernetDevice *ethernet_device;
  PhysMemoryMap *mem_map;
  VIRTIOBusDef *virtio_bus;
  VIRTIODevice *virtio_console;
  VIRTIODevice *virtio_block;
  VIRTIODevice *virtio_net;
  IRQSignal *irq;
  int irq_num;

 public:
  VirtioDevices();
  ~VirtioDevices();
  PhysMemoryRange *get_phys_mem_range(uint64_t paddr);
  void set_dram_buffer(uint8_t *buf);
  void process_io();
  int has_pending_actions();
  int perform_pending_actions();
  void add_virtio_block_device(std::string filename);
};

