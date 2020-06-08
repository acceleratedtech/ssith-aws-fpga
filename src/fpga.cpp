
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fpga.h"
#include "dmaManager.h"

#define TOHOST_OFFSET 0
#define FROMHOST_OFFSET 8
#define FIRST_VIRTIO_IRQ 1

static int debug_virtio = 0;
static int debug_stray_io = 0;

class AWSP2_Response : public AWSP2_ResponseWrapper {
private:
    AWSP2 *fpga;
public:
    AWSP2_Response(int id, AWSP2 *fpga) : AWSP2_ResponseWrapper(id), fpga(fpga) {
    }
    virtual void dmi_read_data(uint32_t rsp_data) {
        //fprintf(stderr, "dmi_read_data data=%08x\r\n", rsp_data);
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
        if (fpga->display_tandem) fprintf(stderr, "[TV] write mem [%08lx] <- %08x\r\n", addr, val);
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
        if (fpga->display_tandem) fprintf(stderr, "\r\n");

        if (offset < num_bytes) {
            if (fpga->display_tandem) fprintf(stderr, " len %d offset %d op %x\r\n", num_bytes, offset, packet[offset]);
            for (uint32_t i = 0; i < num_bytes; i++) {
                if (fpga->display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
            }
            if (fpga->display_tandem) fprintf(stderr, "\r\n");
        }
    } else {

        if (fpga->display_tandem) fprintf(stderr, "[TV] %d packet", num_bytes);
        if (num_bytes < 72) {
            for (uint32_t i = 0; i < num_bytes; i++) {
                if (fpga->display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
            }
        }
        if (fpga->display_tandem) fprintf(stderr, "\r\n");
    }
    if (matched_pc)
        fpga->display_tandem = 1;
}

void AWSP2_Response::io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid) {
    fpga->io_write_queue.emplace(awaddr, awlen / 8, awid);
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
        uint32_t offset = awaddr - pr->addr;
        if (debug_virtio) fprintf(stderr, "virtio awaddr %08x device addr %08lx offset %08x len %d\r\n", awaddr, pr->addr, offset, awlen);
    } else if (awaddr == fpga->tohost_addr) {
        // tohost
    } else if (awaddr == fpga->fromhost_addr) {
        // fromhost
    } else {
        if (debug_stray_io) fprintf(stderr, "io_awaddr awaddr=%08x awlen=%d\r\n", awaddr, awlen);
        //fprintf(stderr, "htif_base_addr=%08x\r\n", fpga->htif_base_addr);
    }
}

void AWSP2_Response::io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid)
{
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(araddr);
    if (pr) {
        uint32_t offset = araddr - pr->addr;
        int size_log2 = 2;
        if (debug_virtio) fprintf(stderr, "virtio araddr %08x device addr %08lx offset %08x len %d\r\n", araddr, pr->addr, offset, arlen);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            uint64_t val = pr->read_func(pr->opaque, offset, size_log2);
            if ((offset % 8) == 4)
              val = (val << 32);
            fpga->request->io_rdata(val, arid, 0, last);
            if (debug_virtio)
                fprintf(stderr, "virtio araddr %0x device addr %08lx offset %08x len %d val %08lx last %d\r\n",
                        araddr + i * 4, pr->addr, offset, arlen, val, last);
            offset += 4;
        }
    } else if (fpga->rom.base <= araddr && araddr < fpga->rom.limit) {
        int offset = (araddr - fpga->rom.base) / 8;
        //fprintf(stderr, "rom offset %x data %08lx\r\n", (int)(araddr - fpga->rom.base), fpga->rom.data[offset]);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            //fprintf(stderr, "io_rdata %08lx\r\n", fpga->rom.data[offset + i]);
            fpga->request->io_rdata(fpga->rom.data[offset + i], arid, 0, last);
        }
    } else if (araddr == fpga->fromhost_addr) {
        uint8_t ch = 0;
        if (arlen != 0)
          fprintf(stderr, "ERROR: fromhost araddr %08x arlen %d\r\n", araddr, arlen);
        if (fpga->htif_enabled && fpga->dequeue_stdin(&ch)) {
            uint64_t cmd = (1ul << 56) | (0ul << 48) | ch;
            fpga->request->io_rdata(cmd, arid, 0, 1);
        } else {
            fpga->request->io_rdata(0, arid, 0, 1);
        }
    } else if (araddr == fpga->sifive_test_addr) {
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            fpga->request->io_rdata(0, arid, 0, last);
        }
    } else {
        if (araddr != 0x10001000 && araddr != 0x10001008 && araddr != 0x50001000 && araddr != 0x50001008)
            if (debug_stray_io) fprintf(stderr, "io_araddr araddr=%08x arlen=%d\r\n", araddr, arlen);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            fpga->request->io_rdata(0, arid, 0, last);
        }
    }

}

void AWSP2_Response::io_wdata(uint64_t wdata, uint8_t wstrb) {
    AXI_Write_State &io_write = fpga->io_write_queue.front();
    uint32_t awaddr = io_write.awaddr;
    PhysMemoryRange *pr = fpga->virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
        int size_log2 = 2;
        uint32_t offset = awaddr - pr->addr;
        if (awaddr & 4) {
            wdata = (wdata >> 32) & 0xFFFFFFFF;;
        }
        if (debug_virtio) fprintf(stderr, "virtio awaddr %08x offset %x wdata %08lx wstrb %x\r\n", awaddr, offset, wdata, wstrb);
        pr->write_func(pr->opaque, offset, wdata, size_log2);
    } else if (awaddr == fpga->tohost_addr) {
        // tohost
        uint8_t dev = (wdata >> 56) & 0xFF;
        uint8_t cmd = (wdata >> 48) & 0xFF;
        uint64_t payload = wdata & 0x0000FFFFFFFFFFFFul;
        if (dev == 1 && cmd == 1) {
            console_putchar(payload);
        } else if (dev == 0 && cmd == 0) {
            int code;
            if (payload == 1) {
                code = 0;
                fprintf(stderr, "PASS\r\n");
            } else {
                code = payload >> 1;
                fprintf(stderr, "FAIL: error %u\r\n", code);
            }
            fpga->stop_io(code);
        } else {
            fprintf(stderr, "\r\nHTIF: dev=%d cmd=%02x payload=%08lx\r\n", dev, cmd, payload);
        }
    } else if (awaddr == fpga->fromhost_addr) {
        //fprintf(stderr, "\r\nHTIF: awaddr %08x wdata=%08lx\r\n", awaddr, wdata);
    } else if (awaddr == fpga->sifive_test_addr) {
        // Similar to HTIF, but the address is in the device tree so an
        // unmodified BBL can use it. It gets used for shutdown so we make it
        // silent.
        int status = wdata & 0xFFFF;
        if (status == 0x3333) {
            // FAIL
            int code = (wdata >> 16) & 0xFFFF;
            fpga->stop_io(code);
        } else if (status == 0x5555) {
            // PASS
            fpga->stop_io(0);
        } else {
            fprintf(stderr, "\r\nSiFive Test Finisher: status=%04x\r\n", status);
        }
    } else {
        if (debug_stray_io) fprintf(stderr, "    io_wdata wdata=%lx wstrb=%x\r\n", wdata, wstrb);
    }
    io_write.wdata_count -= 1;
    if (io_write.wdata_count == 0) {
        fpga->request->io_bdone(io_write.wid, 0);
        //if (debug_stray_io) fprintf(stderr, "    io_bdone awaddr=%08x\r\n", awaddr);
        fpga->io_write_queue.pop();
    }
}


void AWSP2_Response::uart_tohost(uint8_t ch) {
    console_putchar(ch);
    //fprintf(stdout, "uart{%x}\r\n", ch); fflush(stdout);
}

void AWSP2_Response::console_putchar(uint64_t wdata) {
    fputc(wdata, stdout);
    fflush(stdout);
}

AWSP2::AWSP2(int id, const Rom &rom, const char *tun_iface)
    : response(0), rom(rom), last_addr(0), ctrla_seen(0), sifive_test_addr(0x50000000),
      htif_enabled(0), uart_enabled(0), virtio_devices(FIRST_VIRTIO_IRQ, tun_iface),
      pcis_dma_fd(-1), dram_mapping(0), xdma_c2h_fd(-1), xdma_h2c_fd(-1)
{
    sem_init(&sem, 0, 0);
    response = new AWSP2_Response(id, this);
    request = new AWSP2_RequestProxy(IfcNames_AWSP2_RequestS2H);
    dma = platformInit();
    stop_capture = 0;
    display_tandem = 1;
    set_htif_base_addr(0x10001000);
}

AWSP2::~AWSP2() {
}

void AWSP2::capture_tv_info(int c, int display) {
    request->capture_tv_info(c);
    display_tandem = display;
}

void AWSP2::wait() {
    //fprintf(stderr, "fpga::wait\r\n");
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
    //fprintf(stderr, "sw dmi_read %x\r\n", addr);
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
            fprintf(stderr, "ERROR: sbcs=%x\r\n", sbcs);
        }
        if (sbcs & (SBCS_SBBUSY)) {
            fprintf(stderr, "BUSY sbcs=%x %d\r\n", sbcs, count);
        }
    } while (sbcs & (SBCS_SBBUSY));
}

void AWSP2::map_simulated_dram()
{
    size_t dram_alloc_sz = 2 * 1024 * 1024 * 1024ul;
    int dramObject = portalAlloc(dram_alloc_sz, 0);
    uint8_t *dramBuffer = (uint8_t *)portalMmap(dramObject, dram_alloc_sz);
    memset(dramBuffer, 0x0, dram_alloc_sz);
    fprintf(stderr, "dramBuffer=%lx\r\n", (long)dramBuffer);
    int objId = dma->reference(dramObject);
    fprintf(stderr, "DRAM objId %d\r\n", objId);
    register_region(8, objId);

    dram_mapping = dramBuffer;
    dram_mapping_size = dram_alloc_sz;
    set_dram_buffer(dram_mapping);
}

void AWSP2::map_pcis_dma()
{
    size_t dram_size = 2 * 1024 * 1024 * 1024ul;
    off_t dram_offset = 2 * 1024 * 1024 * 1024ul;
    pcis_dma_fd = open("/dev/portal_dma_pcis", O_RDWR);
    if (pcis_dma_fd < 0) {
        fprintf(stderr, "error: opening /dev/portal_dma_pcis %s\r\n", strerror(errno));
        abort();
    }
    // The portal_dma_pcis device driver does not respect the mmap offset, so
    // we map starting from 0 and manually offset the pointer when passing to
    // set_dram_buffer.
    dram_mapping = (uint8_t *)mmap(0, dram_offset + dram_size, PROT_READ|PROT_WRITE, MAP_SHARED, pcis_dma_fd, 0);
    dram_mapping_size = dram_offset + dram_size;
    fprintf(stderr, "PCIS DMA DRAM mapping %08lx size 0x%lx fd %d\r\n", (long)dram_mapping, (long)dram_size, pcis_dma_fd);
    if (dram_mapping == MAP_FAILED) {
        fprintf(stderr, "mmap PCIS DMA failed %s\r\n", strerror(errno));
        dram_mapping = NULL;
        abort();
    }
    set_dram_buffer(dram_mapping + dram_offset);
}

void AWSP2::unmap_pcis_dma()
{
    if (dram_mapping)
        munmap(dram_mapping, dram_mapping_size);
    if (pcis_dma_fd >= 0)
        close(pcis_dma_fd);
    dram_mapping = 0;
    pcis_dma_fd = -1;
}

void AWSP2::open_xdma()
{
    xdma_c2h_fd = open("/dev/xdma0_c2h_0", O_RDONLY);
    if (xdma_c2h_fd < 0) {
        fprintf(stderr, "ERROR: Failed to open /dev/xdma0_c2h_0: %s\r\n", strerror(errno));
        abort();
    }
    xdma_h2c_fd = open("/dev/xdma0_h2c_0", O_WRONLY);
    if (xdma_h2c_fd < 0) {
        fprintf(stderr, "ERROR: Failed to open /dev/xdma0_h2c_0: %s\r\n", strerror(errno));
        abort();
    }
    virtio_devices.xdma_init(xdma_c2h_fd, xdma_h2c_fd);
}

void AWSP2::close_xdma()
{
    if (xdma_c2h_fd >= 0)
        close(xdma_c2h_fd);
    if (xdma_h2c_fd >= 0)
        close(xdma_h2c_fd);
}

void AWSP2::write(uint32_t addr, uint8_t *data, size_t size) {
    if (xdma_h2c_fd >= 0) {
        int bytes_written = pwrite(xdma_h2c_fd, data, size, addr);
        if (bytes_written < 0) {
            fprintf(stderr, "pwrite %d addr %08x size %ld failed: %s\r\n",
                    xdma_h2c_fd, addr, size, strerror(errno));
            abort();
        }
    } else {
        uint8_t *ram_ptr = virtio_devices.phys_mem_get_ram_ptr(addr, TRUE);
        memcpy(ram_ptr, data, size);
    }
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

void AWSP2::enqueue_stdin(char *buf, size_t num_chars)
{
    size_t j = 0;
    for (size_t i = 0; i < num_chars; ++i) {
        if (ctrla_seen) {
            ctrla_seen = 0;
            switch (buf[i]) {
                case 'x':
                    stop_io(0);
                    fprintf(stderr, "\r\nTerminated\r\n");
                    return;
                case 'h':
                    fprintf(stderr, "\r\n");
                    fprintf(stderr, "C-a h   print this help\r\n");
                    fprintf(stderr, "C-a x   exit\r\n");
                    fprintf(stderr, "C-a C-a send C-a\r\n");
                    continue;
                // C-a itself, and any unrecognised characters, are just passed
                // through as if C-a wasn't pressed.
                case 1:
                default:
                    break;
            }
        } else if (buf[i] == 1) {
            ctrla_seen = 1;
            continue;
        }

        buf[j++] = buf[i];
    }
    num_chars = j;

    if (virtio_devices.has_virtio_console_device()) {
        while (num_chars > 0) {
            ssize_t sent = ::write(virtio_stdio_pipe[1], buf, num_chars);
            if (sent < 0) {
                if (errno == EINTR) continue;
                abort();
            }
            buf += sent;
            num_chars -= sent;
        }
    } else {
        std::lock_guard<std::mutex> lock(stdin_mutex);
        for (int i = 0; i < num_chars; i++) {
            if (uart_enabled) {
                request->uart_fromhost(buf[i]);
            } else {
                stdin_queue.push(buf[i]);
            }
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

void AWSP2::process_stdin()
{
    int stdin_fd = STDIN_FILENO;
    int fd_max = -1;
    fd_set rfds, wfds, efds;
    int stop_fd = stop_stdin_pipe[0];

    for (;;) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);
        FD_SET(stdin_fd, &rfds);
        FD_SET(stop_fd, &rfds);
        fd_max = std::max(stdin_fd, stop_fd);

        int ret = select(fd_max + 1, &rfds, &wfds, &efds, NULL);
        if (FD_ISSET(stop_fd, &rfds)) {
            break;
        }
        if (FD_ISSET(stdin_fd, &rfds)) {
            // Read from stdin and enqueue for HTIF/UART get char
            char buf[128];
            memset(buf, 0, sizeof(buf));
            ssize_t ret = read(stdin_fd, buf, sizeof(buf));
            if (ret > 0) {
                enqueue_stdin(buf, ret);
            }
        }
    }

    close(stop_fd);
    if (virtio_devices.has_virtio_console_device()) {
        close(virtio_stdio_pipe[1]);
    }
}

void *AWSP2::process_stdin_thread(void *opaque)
{
    ((AWSP2 *)opaque)->process_stdin();
    return NULL;
}

void AWSP2::start_io()
{
    struct termios stdin_termios;
    struct termios stdout_termios;

    tcgetattr(STDIN_FILENO, &stdin_termios);
    tcgetattr(STDOUT_FILENO, &stdout_termios);
    orig_stdin_termios = stdin_termios;
    orig_stdout_termios = stdout_termios;
    atexit(&reset_termios);

    cfmakeraw(&stdin_termios);
    cfmakeraw(&stdout_termios);
    stdin_termios.c_cc[VMIN] = 1;
    stdout_termios.c_cc[VMIN] = 1;
    stdin_termios.c_cc[VTIME] = 0;
    stdout_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_termios);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &stdout_termios);

    pipe(stop_stdin_pipe);
    fcntl(stop_stdin_pipe[1], F_SETFL, O_NONBLOCK);
    pthread_create(&stdin_thread, NULL, &process_stdin_thread, this);
    pthread_setname_np(stdin_thread, "Console input");

    if (virtio_devices.has_virtio_console_device()) {
        pipe(virtio_stdio_pipe);
        virtio_devices.set_virtio_stdin_fd(virtio_stdio_pipe[0]);
    }

    virtio_devices.start();
}

void AWSP2::stop_io(int code)
{
    exit_code = code;

    char dummy = 'X';
    ::write(stop_stdin_pipe[1], &dummy, sizeof(dummy));
    close(stop_stdin_pipe[1]);

    virtio_devices.stop();
}

int AWSP2::join_io()
{
    pthread_join(stdin_thread, NULL);

    virtio_devices.join();

    return exit_code;
}

struct termios AWSP2::orig_stdin_termios;
struct termios AWSP2::orig_stdout_termios;

void AWSP2::reset_termios()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_stdin_termios);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &orig_stdout_termios);
}

void AWSP2::set_htif_base_addr(uint64_t baseaddr)
{
    tohost_addr = baseaddr + TOHOST_OFFSET;
    fromhost_addr = baseaddr + FROMHOST_OFFSET;
}

void AWSP2::set_tohost_addr(uint64_t addr)
{
    tohost_addr = addr;
}

void AWSP2::set_fromhost_addr(uint64_t addr)
{
    fromhost_addr = addr;
}

void AWSP2::set_htif_enabled(bool enabled)
{
    htif_enabled = enabled;
}

void AWSP2::set_uart_enabled(bool enabled)
{
    uart_enabled = enabled;
}
