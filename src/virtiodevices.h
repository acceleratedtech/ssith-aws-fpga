
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
  VIRTIODevice *virtio_entropy;
  IRQSignal *irq;
  int irq_num;
  const char *tun_ifname;

 public:
  VirtioDevices(int first_irq_num = 0, const char *tun_ifname = 0);
  ~VirtioDevices();
  PhysMemoryRange *get_phys_mem_range(uint64_t paddr);
  uint8_t *phys_mem_get_ram_ptr(uint64_t paddr, BOOL is_rw);
  void set_dram_buffer(uint8_t *buf);
  void process_io();
  int has_pending_actions();
  int perform_pending_actions();
  void wait_for_pending_actions(int timeout);
  void add_virtio_block_device(std::string filename);
  void add_virtio_console_device();
};

