
#pragma once

#include <string>
#include <pthread.h>

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
  VIRTIODevice *virtio_console = 0;
  VIRTIODevice *virtio_block = 0;
  VIRTIODevice *virtio_net = 0;
  VIRTIODevice *virtio_entropy = 0;
  IRQSignal *irq;
  int irq_num;
  const char *tun_ifname;
  int stop_pipe[2];
  pthread_t io_thread;

  void process_io();
  static void *process_io_thread(void *opaque);

 public:
  VirtioDevices(int first_irq_num = 0, const char *tun_ifname = 0);
  ~VirtioDevices();
  PhysMemoryRange *get_phys_mem_range(uint64_t paddr);
  uint8_t *phys_mem_get_ram_ptr(uint64_t paddr, BOOL is_rw);
  void set_dram_buffer(uint8_t *buf);
  void xdma_init(int c2h_fd, int h2c_fd);
  void add_virtio_block_device(std::string filename);
  void add_virtio_console_device();
  void set_virtio_stdin_fd(int fd);
  bool has_virtio_console_device();
  void start();
  void stop();
  void join();
};

