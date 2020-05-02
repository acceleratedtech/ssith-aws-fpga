
#include "fpga.h"

#define TOHOST_OFFSET 8
#define FROMHOST_OFFSET 0

static int debug_virtio = 0;
static int debug_stray_io = 0;

class AWSP2_Response : public AWSP2_ResponseWrapper {
private:
    AWSP2 *fpga;
public:
    AWSP2_Response(int id, AWSP2 *fpga) : AWSP2_ResponseWrapper(id), fpga(fpga) {
    }
    virtual void dmi_read_data(uint32_t rsp_data) {
        //fprintf(stderr, "dmi_read_data data=%08x\n", rsp_data);
        fpga->rsp_data = rsp_data;
        sem_post(&fpga->sem);
    }
    void ddr_data ( const bsvvector_Luint8_t_L64 data ) {
        memcpy(&fpga->pcis_rsp_data, data, 64);
        sem_post(&fpga->sem);
    }
    virtual void dmi_status_data(uint16_t rsp_data) {
        fpga->rsp_data = rsp_data;
        sem_post(&fpga->sem);
    }
    virtual void irq_status ( const uint32_t levels );
    virtual void tandem_packet(const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes);
    void io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid);
    void io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid);
    void console_putchar(uint64_t wdata);
    virtual void io_wdata(uint64_t wdata, uint8_t wstrb);
    virtual void uart_tohost(uint8_t ch);
};

void AWSP2_Response::irq_status ( const uint32_t levels )
{
    fpga->rsp_data = levels;
    sem_post(&fpga->sem);
}


void AWSP2_Response::tandem_packet(const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes)
{
    if (fpga->stop_capture)
        return;

    uint8_t packet[72];
    int matched_pc = 0;
    for (int i = 0; i < 72; i++)
        packet[i] = bytes[71 - i];
    if (packet[0] == te_op_begin_group && packet[1] == te_op_state_init && packet[2] == te_op_mem_req && packet[11] == 0x21) {
        uint64_t addr = 0;
        uint32_t val = 0;
        memcpy(&addr, &packet[3], sizeof(addr));
        memcpy(&val, &packet[12], sizeof(val));
        if (fpga->display_tandem) fprintf(stderr, "[TV] write mem [%08lx] <- %08x\n", addr, val);
    } else if (packet[0] == te_op_begin_group) {
        int offset = 1;
        if (fpga->display_tandem) fprintf(stderr, "[TV]");
        while (offset < num_bytes) {
            if (packet[offset] == te_op_addl_state) {
                offset++;
                if (packet[offset] == te_op_addl_state_priv) {
                    offset++;
                    if (fpga->display_tandem) fprintf(stderr, " priv %x", packet[offset]);
                    offset++;
                } else if (packet[offset] == te_op_addl_state_pc) {
                    offset++;
                    uint64_t addr = 0;
                    memcpy(&addr, &packet[offset], sizeof(addr));
                    offset += sizeof(addr);
                    if ((addr & 0xffffffffffffffc0ul) == 0xffffffe0006591c0ul)
                        matched_pc = 1;
                    if (fpga->display_tandem) fprintf(stderr, " next pc %08lx", addr);
                    if (addr == 0x82000130) {
                        if (fpga->display_tandem) fprintf(stderr, " matched STOP pc %08lx, stopping capture", addr);
                        fpga->stop_capture = 1;
                    }
                } else if (packet[offset] == te_op_addl_state_data8) {
                    offset++;
                    uint8_t data = 0;
                    memcpy(&data, &packet[offset], sizeof(data));
                    offset += sizeof(data);
                    if (fpga->display_tandem) fprintf(stderr, " data8 %02x", data);
                } else if (packet[offset] == te_op_addl_state_data16) {
                    offset++;
                    uint16_t data = 0;
                    memcpy(&data, &packet[offset], sizeof(data));
                    offset += sizeof(data);
                    if (fpga->display_tandem) fprintf(stderr, " data16 %04x", data);
                } else if (packet[offset] == te_op_addl_state_data32) {
                    offset++;
                    uint32_t data = 0;
                    memcpy(&data, &packet[offset], sizeof(data));
                    offset += sizeof(data);
                    if (fpga->display_tandem) fprintf(stderr, " data32 %08x", data);
                } else if (packet[offset] == te_op_addl_state_data64) {
                    offset++;
                    uint64_t data = 0;
                    memcpy(&data, &packet[offset], sizeof(data));
                    offset += sizeof(data);
                    if (fpga->display_tandem) fprintf(stderr, " data64 %08lx", data);
                } else if (packet[offset] == te_op_addl_state_eaddr) {
                    offset++;
                    uint64_t data = 0;
                    memcpy(&data, &packet[offset], sizeof(data));
                    offset += sizeof(data);
                    if (fpga->display_tandem) fprintf(stderr, " eaddr %08lx", data);
                } else {
                    break;
                }
            } else if (packet[offset] == te_op_16b_instr) {
                offset++;
                uint16_t instr = 0;
                memcpy(&instr, &packet[offset], sizeof(instr));
                offset += sizeof(instr);
                if (fpga->display_tandem) fprintf(stderr, " instr %04x", instr);
            } else if (packet[offset] == te_op_32b_instr) {
                offset++;
                uint32_t instr = 0;
                memcpy(&instr, &packet[offset], sizeof(instr));
                offset += sizeof(instr);
                if (fpga->display_tandem) fprintf(stderr, " instr %08x", instr);
            } else if (packet[offset] == te_op_full_reg) {
                offset++;
                uint16_t regnum = 0;
                uint64_t regval = 0;
                memcpy(&regnum, &packet[offset], sizeof(regnum));
                offset += sizeof(regnum);
                memcpy(&regval, &packet[offset], sizeof(regval));
                offset += sizeof(regval);

                if (fpga->display_tandem) fprintf(stderr, " reg %x val %08lx", regnum, regval);
            } else if (packet[offset] == te_op_end_group) {
                offset++;
            } else {
                break;
            }
        }
        if (fpga->display_tandem) fprintf(stderr, "\n");

        if (offset < num_bytes) {
            if (fpga->display_tandem) fprintf(stderr, " len %d offset %d op %x\n", num_bytes, offset, packet[offset]);
            for (uint32_t i = 0; i < num_bytes; i++) {
                if (fpga->display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
            }
            if (fpga->display_tandem) fprintf(stderr, "\n");
        }
    } else {

        if (fpga->display_tandem) fprintf(stderr, "[TV] %d packet", num_bytes);
        if (num_bytes < 72) {
            for (uint32_t i = 0; i < num_bytes; i++) {
                if (fpga->display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
            }
        }
        if (fpga->display_tandem) fprintf(stderr, "\n");
    }
    if (matched_pc)
        fpga->display_tandem = 1;
}

void AWSP2_Response::io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid) {
    fpga->awaddr = awaddr;
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
        uint32_t offset = awaddr - pr->addr;
        if (debug_virtio) fprintf(stderr, "virtio awaddr %08x device addr %08lx offset %08x len %d\n", awaddr, pr->addr, offset, awlen);
    } else if (awaddr == 0x60000000) {
        // UART
    } else if (awaddr == (fpga->htif_base_addr + TOHOST_OFFSET)) {
        // tohost
    } else if (awaddr == (fpga->htif_base_addr + FROMHOST_OFFSET)) {
        // fromhost
    } else {
        fprintf(stderr, "io_awaddr awaddr=%08x awlen=%d\n", awaddr, awlen);
        //fprintf(stderr, "htif_base_addr=%08x\n", fpga->htif_base_addr);
    }
    fpga->wdata_count = awlen / 8;
    fpga->wid = awid;
}

void AWSP2_Response::io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid)
{
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(araddr);
    if (pr) {
        uint32_t offset = araddr - pr->addr;
        int size_log2 = 2;
        if (debug_virtio) fprintf(stderr, "virtio araddr %08x device addr %08lx offset %08x len %d\n", araddr, pr->addr, offset, arlen);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            uint64_t val = pr->read_func(pr->opaque, offset, size_log2);
            if ((offset % 8) == 4)
              val = (val << 32);
            fpga->request->io_rdata(val, arid, 0, last);
            if (debug_virtio)
                fprintf(stderr, "virtio araddr %0x device addr %08lx offset %08x len %d val %08lx last %d\n",
                        araddr + offset, pr->addr, offset, arlen, val, last);
            offset += 4;
        }
    } else if (fpga->rom.base <= araddr && araddr < fpga->rom.limit) {
        int offset = (araddr - fpga->rom.base) / 8;
        //fprintf(stderr, "rom offset %x data %08lx\n", (int)(araddr - rom.base), rom.data[offset]);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            //fprintf(stderr, "io_rdata %08lx\n", rom.data[offset + i]);
            fpga->request->io_rdata(fpga->rom.data[offset + i], arid, 0, last);
        }
    } else if (araddr == (fpga->htif_base_addr + FROMHOST_OFFSET)) {
        uint8_t ch = 0;
        if (fpga->dequeue_stdin(&ch) && !fpga->uart_enabled) {
            uint64_t cmd = (1ul << 56) | (0ul << 48) | ch;
            fpga->request->io_rdata(cmd, arid, 0, 1);
        } else {
            fpga->request->io_rdata(0, arid, 0, 1);
        }
    } else {
        if (araddr != 0x10001000 && araddr != 0x10001008 && araddr != 0x50001000 && araddr != 0x50001008)
            if (debug_stray_io) fprintf(stderr, "io_araddr araddr=%08x arlen=%d\n", araddr, arlen);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            // 0xbad0beef
            fpga->request->io_rdata(0, arid, 0, last);
        }
    }

}

void AWSP2_Response::io_wdata(uint64_t wdata, uint8_t wstrb) {
    uint32_t awaddr = fpga->awaddr;
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
        int size_log2 = 2;
        uint32_t offset = awaddr - pr->addr;
        if (awaddr & 4) {
            wdata = (wdata >> 32) & 0xFFFFFFFF;;
        }
        if (debug_virtio) fprintf(stderr, "virtio awaddr %08x offset %x wdata %08lx wstrb %x\n", awaddr, offset, wdata, wstrb);
        pr->write_func(pr->opaque, offset, wdata, size_log2);
    } else if (awaddr == 0x60000000) {
        console_putchar(wdata);
    } else if (awaddr == (fpga->htif_base_addr + TOHOST_OFFSET)) {
        // tohost
        uint8_t dev = (wdata >> 56) & 0xFF;
        uint8_t cmd = (wdata >> 48) & 0xFF;
        uint64_t payload = wdata & 0x0000FFFFFFFFFFFFul;
        if (dev == 1 && cmd == 1) {
            // putchar
            console_putchar(payload);
        } else if (dev == 0 && cmd == 0) {
	  if (payload == 1) {
            fprintf(stderr, "PASS\n");
	  } else {
            fprintf(stderr, "FAIL: error %lu\n", (payload >> 1));
	  }
	  //fpga->halt();
        } else {
            fprintf(stderr, "\nHTIF: dev=%d cmd=%02x payload=%08lx\n", dev, cmd, payload);
        }
    } else if (awaddr == (fpga->htif_base_addr + FROMHOST_OFFSET)) {
        //fprintf(stderr, "\nHTIF: awaddr %08x wdata=%08lx\n", awaddr, wdata);
    } else {
        if (debug_stray_io) fprintf(stderr, "io_wdata wdata=%lx wstrb=%x\n", wdata, wstrb);
    }
    fpga->wdata_count -= 1;
    if (fpga->wdata_count <= 0) {
        fpga->request->io_bdone(fpga->wid, 0);
    }
}


void AWSP2_Response::uart_tohost(uint8_t ch) {
  console_putchar(ch);
}

void AWSP2_Response::console_putchar(uint64_t wdata) {
    if (fpga->start_of_line) {
        printf("\nCONSOLE: ");
        fpga->start_of_line = 0;
    }
    fputc(wdata, stdout);
    fflush(stdout);
    if (wdata == '\n') {
        fflush(stdout);
        fpga->start_of_line = 1;
    }
}

AWSP2::AWSP2(int id, const Rom &rom)
  : response(0), rom(rom), last_addr(0), wdata_count(0), wid(0), start_of_line(1), uart_enabled(0)
{
    sem_init(&sem, 0, 0);
    response = new AWSP2_Response(id, this);
    request = new AWSP2_RequestProxy(IfcNames_AWSP2_RequestS2H);
    stop_capture = 0;
    display_tandem = 1;
    htif_base_addr = 0x10001000;
}

AWSP2::~AWSP2() {
}

void AWSP2::capture_tv_info(int c, int display) {
    request->capture_tv_info(c);
    display_tandem = display;
}

void AWSP2::wait() {
    //fprintf(stderr, "fpga::wait\n");
    sem_wait(&sem);
}

uint32_t AWSP2::dmi_status() {
    std::lock_guard<std::mutex> lock(client_mutex);
    request->dmi_status();
    wait();
    return rsp_data;
}

// private API, should only be called with the lock held
uint32_t AWSP2::dmi_read(uint32_t addr) {
    //fprintf(stderr, "sw dmi_read %x\n", addr);
    request->dmi_read(addr);
    wait();
    return rsp_data;
}

// private API, should only be called with the lock held
void AWSP2::dmi_write(uint32_t addr, uint32_t data) {
    request->dmi_write(addr, data);
    wait();
}

void AWSP2::ddr_read(uint32_t addr, bsvvector_Luint8_t_L64 data) {
    std::lock_guard<std::mutex> lock(client_mutex);
    request->ddr_read(addr);
    wait();
    if (data)
        memcpy(data, pcis_rsp_data, 64);
}

void AWSP2::ddr_write(uint32_t addr, const bsvvector_Luint8_t_L64 data, uint64_t wstrb) {
    std::lock_guard<std::mutex> lock(client_mutex);
    request->ddr_write(addr, data, wstrb);
    wait();
}

void AWSP2::ddr_write(uint32_t start_addr, const uint32_t *data, size_t num_bytes)
{
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(DM_SBCS_REG, SBCS_SBACCESS32 | SBCS_SBAUTOINCREMENT);
    sbcs_wait();
    dmi_write(DM_SBADDRESS0_REG, start_addr);
    sbcs_wait();
    for (size_t i = 0; i < num_bytes; i += 4) {
        sbcs_wait();
        dmi_write(DM_SBDATA0_REG, data[i / 4]);
    }
    sbcs_wait();
    dmi_write(DM_SBCS_REG, 0);
}


void AWSP2::register_region(uint32_t region, uint32_t objid) {
    std::lock_guard<std::mutex> lock(client_mutex);
    request->register_region(region, objid);
}

void AWSP2::memory_ready() {
    std::lock_guard<std::mutex> lock(client_mutex);
    request->memory_ready();
}

uint64_t AWSP2::read_csr(int i) {
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | i);
    uint64_t val = dmi_read(5);
    val <<=  32;
    val |= dmi_read(4);
    return val;
}

void AWSP2::write_csr(int i, uint64_t val) {
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(5, (val >> 32) & 0xFFFFFFFF);
    dmi_write(4, (val >>  0) & 0xFFFFFFFF);
    dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | (1 << 16) | i);
}

uint64_t AWSP2::read_gpr(int i) {
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | 0x1000 | i);
    uint64_t val = dmi_read(5);
    val <<=  32;
    val |= dmi_read(4);
    return val;
}

void AWSP2::write_gpr(int i, uint64_t val) {
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(5, (val >> 32) & 0xFFFFFFFF);
    dmi_write(4, (val >>  0) & 0xFFFFFFFF);
    dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | (1 << 16) | 0x1000 | i);
}

// private API, should only be called with the lock held
void AWSP2::sbcs_wait() {
    uint32_t sbcs = 0;
    int count = 0;
    do {
        sbcs = dmi_read(DM_SBCS_REG);
        count++;
        if (sbcs & (SBCS_SBBUSYERROR)) {
            fprintf(stderr, "ERROR: sbcs=%x\n", sbcs);
        }
        if (sbcs & (SBCS_SBBUSY)) {
            fprintf(stderr, "BUSY sbcs=%x %d\n", sbcs, count);
        }
    } while (sbcs & (SBCS_SBBUSY));
}

uint32_t AWSP2::read32(uint32_t addr) {
    std::lock_guard<std::mutex> lock(client_mutex);

    if (1 || last_addr != addr) {
        dmi_write(DM_SBCS_REG, SBCS_SBACCESS32 | SBCS_SBREADONADDR | SBCS_SBREADONDATA | SBCS_SBAUTOINCREMENT | SBCS_SBBUSYERROR);
        sbcs_wait();
        dmi_write(DM_SBADDRESS0_REG, addr);
    }
    sbcs_wait();
    dmi_write(DM_SBCS_REG, 0);
    sbcs_wait();
    uint64_t lo = dmi_read(DM_SBDATA0_REG);
    last_addr = addr + 4;
    return lo;
}

uint64_t AWSP2::read64(uint32_t addr) {
    std::lock_guard<std::mutex> lock(client_mutex);

    if (1 || last_addr != addr) {
        dmi_write(DM_SBCS_REG, SBCS_SBACCESS32 | SBCS_SBREADONADDR | SBCS_SBREADONDATA | SBCS_SBAUTOINCREMENT | SBCS_SBBUSYERROR);
        sbcs_wait();
        dmi_write(DM_SBADDRESS0_REG, addr);
    }
    sbcs_wait();
    uint64_t lo = dmi_read(DM_SBDATA0_REG);
    sbcs_wait();
    dmi_write(DM_SBCS_REG, 0);
    sbcs_wait();
    uint64_t hi = dmi_read(DM_SBDATA0_REG);
    last_addr = addr + 8;
    return (hi << 32) | lo;
}

void AWSP2::write32(uint32_t addr, uint32_t val) {
    std::lock_guard<std::mutex> lock(client_mutex);

    if (1 || last_addr != addr) {
        dmi_write(DM_SBCS_REG, SBCS_SBACCESS32);
        dmi_write(DM_SBADDRESS0_REG, addr);
    }
    sbcs_wait();
    dmi_write(DM_SBDATA0_REG, (val >>  0) & 0xFFFFFFFF);
    last_addr = addr + 4;
}

void AWSP2::write64(uint32_t addr, uint64_t val) {
    std::lock_guard<std::mutex> lock(client_mutex);

    if (1 || last_addr != addr) {
        dmi_write(DM_SBCS_REG, SBCS_SBACCESS32 | SBCS_SBAUTOINCREMENT);
        dmi_write(DM_SBADDRESS0_REG, addr);
    }
    sbcs_wait();
    dmi_write(DM_SBDATA0_REG, (val >>  0) & 0xFFFFFFFF);
    sbcs_wait();
    dmi_write(DM_SBDATA0_REG, (val >>  32) & 0xFFFFFFFF);
    last_addr = addr + 8;
    sbcs_wait();
    dmi_write(DM_SBCS_REG, 0);
}

void AWSP2::halt(int timeout) {
    std::lock_guard<std::mutex> lock(client_mutex);
    dmi_write(DM_CONTROL_REG, DM_CONTROL_HALTREQ | dmi_read(DM_CONTROL_REG));
    for (int i = 0; i < 100; i++) {
        uint32_t status = dmi_read(DM_STATUS_REG);
        if (status & (1 << 9))
            break;
    }
    dmi_write(DM_CONTROL_REG, ~DM_CONTROL_HALTREQ & dmi_read(DM_CONTROL_REG));
}

void AWSP2::resume(int timeout) {
    std::lock_guard<std::mutex> lock(client_mutex);

    dmi_write(DM_CONTROL_REG, DM_CONTROL_RESUMEREQ | dmi_read(DM_CONTROL_REG));
    for (int i = 0; i < 100; i++) {
        uint32_t status = dmi_read(DM_STATUS_REG);
        if (status & (1 << 17))
            break;
    }
    dmi_write(DM_CONTROL_REG, ~DM_CONTROL_RESUMEREQ & dmi_read(DM_CONTROL_REG));
}

void AWSP2::irq_set_levels(uint32_t w1s)
{
    std::lock_guard<std::mutex> lock(client_mutex);

    request->irq_set_levels(w1s);
}

void AWSP2::irq_clear_levels(uint32_t w1c)
{
    std::lock_guard<std::mutex> lock(client_mutex);

    request->irq_clear_levels(w1c);
}

int AWSP2::read_irq_status ()
{
    std::lock_guard<std::mutex> lock(client_mutex);

    request->read_irq_status();
    wait();
    return rsp_data;
}


void AWSP2::set_fabric_verbosity(uint8_t verbosity) {
    std::lock_guard<std::mutex> lock(client_mutex);

    request->set_fabric_verbosity(verbosity);
}

void AWSP2::set_dram_buffer(uint8_t *buf) {
    virtio_devices.set_dram_buffer(buf);
}

void AWSP2::enqueue_stdin(const char *buf, int num_chars)
{
    std::lock_guard<std::mutex> lock(stdin_mutex);
    for (int i = 0; i < num_chars; i++) {
	if (uart_enabled) {
	    request->uart_fromhost(buf[i]);
	} else {
	    stdin_queue.push(buf[i]);
	}
    }
}

int AWSP2::dequeue_stdin(uint8_t *chp)
{
    std::lock_guard<std::mutex> lock(stdin_mutex);
    if (stdin_queue.size()) {
        *chp = stdin_queue.front();
        stdin_queue.pop();
        return 1;
    } else {
        return 0;
    };
}

void AWSP2::process_io()
{
    int stdin_fd = 0;
    int fd_max = -1;
    fd_set rfds,  wfds, efds;
    int delay = 10; // ms
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    FD_SET(stdin_fd, &rfds);
    fd_max = stdin_fd;

    tv.tv_sec = delay / 1000;
    tv.tv_usec = (delay % 1000) * 1000;
    int ret = select(fd_max + 1, &rfds, &wfds, &efds, &tv);
    if (FD_ISSET(stdin_fd, &rfds)) {
        // Read from stdin and enqueue for HTIF get char
        char buf[128];
        memset(buf, 0, sizeof(buf));
        int ret = read(0, buf, sizeof(buf));
        if (ret > 0) {
            enqueue_stdin(buf, ret);
        }
    }
    virtio_devices.process_io();
    if (virtio_devices.has_pending_actions()) {
        halt();

        virtio_devices.perform_pending_actions();

        resume();
    }
}

void AWSP2::set_htif_base_addr(uint64_t baseaddr)
{
    htif_base_addr = baseaddr;
}

void AWSP2::set_uart_enabled(bool enabled)
{
    uart_enabled = enabled;
}
