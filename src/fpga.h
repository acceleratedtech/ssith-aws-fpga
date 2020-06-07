#pragma once

#include <string.h>
#include <queue>
#include <mutex>
#include <pthread.h>
#include <termios.h>

#include "virtiodevices.h"
#include "AWSP2_Request.h"
#include "AWSP2_Response.h"

#define DM_CONTROL_REG 0x10
#define DM_STATUS_REG 0x11
#define DM_HALTSUM1_REG 0x13
#define DM_COMMAND_REG 0x17
#define DM_SBCS_REG 0x38
#define DM_SBADDRESS0_REG 0x39
#define DM_SBADDRESS1_REG 0x3a
#define DM_SBADDRESS2_REG 0x3b
#define DM_SBADDRESS3_REG 0x37 // note out of order reg address
#define DM_SBDATA0_REG 0x3c
#define DM_SBDATA1_REG 0x3d
#define DM_SBDATA2_REG 0x3e
#define DM_SBDATA3_REG 0x3f

#define DM_CONTROL_HALTREQ (1 << 31)
#define DM_CONTROL_RESUMEREQ (1 << 30)

#define DM_COMMAND_ACCESS_REGISTER 0
#define DM_COMMAND_ACCESS_MEMORY 2

#define AAMVIRTUAL (1 << 23)
#define AAMSIZE_8 (3 << 20)
#define AAMPOSTINCREMENT (1 << 19)
#define WRITE (1 << 16)

#define SBCS_SBREADONDATA (1 << 15)
#define SBCS_SBAUTOINCREMENT (1 << 16)
#define SBCS_SBACCESS32 (2 << 17)
#define SBCS_SBREADONADDR (1 << 20)
#define SBCS_SBBUSY (1 << 21)
#define SBCS_SBBUSYERROR (1 << 22)

// ================================================================
// Encodings
// cf. "Trace Protocol Specification Version 2018-09-12, Darius Rad, Bluespec, Inc."

const uint8_t te_op_begin_group     = 1;
const uint8_t te_op_end_group       = 2;
const uint8_t te_op_incr_pc         = 3;
const uint8_t te_op_full_reg        = 4;
const uint8_t te_op_incr_reg        = 5;
const uint8_t te_op_incr_reg_OR     = 6;
const uint8_t te_op_addl_state      = 7;
const uint8_t te_op_mem_req         = 8;
const uint8_t te_op_mem_rsp         = 9;
const uint8_t te_op_hart_reset      = 10;
const uint8_t te_op_state_init      = 11;
const uint8_t te_op_16b_instr       = 16;
const uint8_t te_op_32b_instr       = 17;

const uint8_t te_mem_req_size_8     = 0;
const uint8_t te_mem_req_size_16    = 1;
const uint8_t te_mem_req_size_32    = 2;
const uint8_t te_mem_req_size_64    = 3;

const uint8_t te_mem_req_op_Load       = 0;
const uint8_t te_mem_req_op_Store      = 1;
const uint8_t te_mem_req_op_LR         = 2;
const uint8_t te_mem_req_op_SC         = 3;
const uint8_t te_mem_req_op_AMO_swap   = 4;
const uint8_t te_mem_req_op_AMO_add    = 5;
const uint8_t te_mem_req_op_AMO_xor    = 6;
const uint8_t te_mem_req_op_AMO_and    = 7;
const uint8_t te_mem_req_op_AMO_or     = 8;
const uint8_t te_mem_req_op_AMO_min    = 9;
const uint8_t te_mem_req_op_AMO_max    = 10;
const uint8_t te_mem_req_op_AMO_minu   = 11;
const uint8_t te_mem_req_op_AMO_maxu   = 12;
const uint8_t te_mem_req_op_ifetch     = 13;

const uint8_t te_mem_result_success    = 0;
const uint8_t te_mem_result_failure    = 1;

const uint8_t te_op_addl_state_priv     = 1;
const uint8_t te_op_addl_state_paddr    = 2;
const uint8_t te_op_addl_state_eaddr    = 3;
const uint8_t te_op_addl_state_data8    = 4;
const uint8_t te_op_addl_state_data16   = 5;
const uint8_t te_op_addl_state_data32   = 6;
const uint8_t te_op_addl_state_data64   = 7;
const uint8_t te_op_addl_state_mtime    = 8;
const uint8_t te_op_addl_state_pc_paddr = 9;
const uint8_t te_op_addl_state_pc       = 10;


struct Rom {
  uint64_t base;
  uint64_t limit;
  uint64_t *data;
};

struct AXI_Write_State {
  uint32_t awaddr;
  uint16_t wdata_count;
  uint16_t wid;
  AXI_Write_State(uint32_t awaddr, uint16_t wdata_count, uint16_t wid)
    : awaddr(awaddr), wdata_count(wdata_count), wid(wid) {}
};

class DmaManager;

class AWSP2_Response;
class AWSP2 {
    sem_t sem;
    AWSP2_Response *response;
    AWSP2_RequestProxy *request;
    DmaManager *dma;
    Rom rom;
    VirtioDevices virtio_devices;
    uint32_t rsp_data;
    uint32_t last_addr;
    int ctrla_seen;
    std::queue<AXI_Write_State> io_write_queue;
    int stop_capture;
    int display_tandem;
    bsvvector_Luint8_t_L64 pcis_rsp_data;
    uint64_t tohost_addr;
    uint64_t fromhost_addr;
    uint64_t htif_enabled;
    uint64_t uart_enabled;
    int pcis_dma_fd;
    uint8_t *dram_mapping;
    size_t dram_mapping_size;
    int xdma_c2h_fd;
    int xdma_h2c_fd;

    std::mutex client_mutex;
    std::mutex stdin_mutex;
    std::queue<uint8_t> stdin_queue;
    int stop_stdin_pipe[2];
    pthread_t stdin_thread;
    int virtio_stdio_pipe[2];
    static struct termios orig_stdin_termios;
    static struct termios orig_stdout_termios;

    friend class AWSP2_Response;
public:
    AWSP2(int id, const Rom &rom, const char *tun_iface);
    virtual ~AWSP2();

    void capture_tv_info(int c, int display = 1);
    void wait();

    uint32_t dmi_status();

    void map_simulated_dram();
    void map_pcis_dma();
    void unmap_pcis_dma();
    void open_xdma();
    void close_xdma();
    void ddr_read(uint32_t addr, bsvvector_Luint8_t_L64 data);
    void ddr_write(uint32_t addr, const bsvvector_Luint8_t_L64 data, uint64_t wstrb = 0xFFFFFFFFFFFFFFFFul);
    void ddr_write(uint32_t start_addr, const uint32_t *data, size_t num_bytes);

    void register_region(uint32_t region, uint32_t objid);
    void memory_ready();
    uint64_t read_csr(int i);
    void write_csr(int i, uint64_t val);
    uint64_t read_gpr(int i);
    void write_gpr(int i, uint64_t val);

    void write(uint32_t addr, uint8_t *data, size_t num_bytes);

    void halt(int timeout = 100);
    void resume(int timeout = 100);

    void irq_set_levels(uint32_t w1s);
    void irq_clear_levels(uint32_t w1s);
    int read_irq_status ();

    void set_fabric_verbosity(uint8_t verbosity);
    void set_dram_buffer(uint8_t *buf);

    void enqueue_stdin(char *buf, size_t num_chars);
    int dequeue_stdin(uint8_t *chp);

    VirtioDevices &get_virtio_devices() { return virtio_devices; }
    void start_io();
    void stop_io();
    void join_io();

    void set_htif_base_addr(uint64_t baseaddr);
    void set_tohost_addr(uint64_t addr);
    void set_fromhost_addr(uint64_t addr);
    void set_htif_enabled(bool enabled);
    void set_uart_enabled(bool enabled);

 private:
    void process_stdin();
    static void *process_stdin_thread(void *opaque);
    static void reset_termios();
    uint32_t dmi_read(uint32_t addr);
    void dmi_write(uint32_t addr, uint32_t data);
    void sbcs_wait();
    
};
