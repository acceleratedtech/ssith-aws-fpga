#pragma once

#include <string.h>
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


class AWSP2 : public AWSP2_ResponseWrapper {
    sem_t sem;
    AWSP2_RequestProxy *request;
    uint32_t rsp_data;
    uint32_t last_addr;
    uint32_t wdata_count;
    uint32_t wid;
    bsvvector_Luint8_t_L64 pcis_rsp_data;
public:
AWSP2(int id)
    : AWSP2_ResponseWrapper(id), last_addr(0), wdata_count(0), wid(0) {
        sem_init(&sem, 0, 0);
        request = new AWSP2_RequestProxy(IfcNames_AWSP2_RequestS2H);
    }
    virtual void dmi_read_data(uint32_t rsp_data) {
        //fprintf(stderr, "dmi_read_data data=%08x\n", rsp_data);
        this->rsp_data = rsp_data;
        sem_post(&sem);
    }
    virtual void dmi_status_data(uint16_t rsp_data) {
        this->rsp_data = rsp_data;
        sem_post(&sem);
    }
    void ddr_data ( const bsvvector_Luint8_t_L64 data ) {
        memcpy(&pcis_rsp_data, data, 64);
        sem_post(&sem);
    }
    void capture_tv_info(int c) {
        request->capture_tv_info(c);
    }
    virtual void tandem_packet(const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes) {
        uint8_t packet[72];
        for (int i = 0; i < 72; i++)
            packet[i] = bytes[71 - i];
        //fprintf(stderr, "[TV] %x %x %x %x\n", packet[0], packet[1], packet[2], packet[11]);
        if (packet[0] == te_op_begin_group && packet[1] == te_op_state_init && packet[2] == te_op_mem_req && packet[11] == 0x21) {
            uint64_t addr = 0;
            uint32_t val = 0;
            memcpy(&addr, &packet[3], sizeof(addr));
            memcpy(&val, &packet[12], sizeof(val));
            fprintf(stderr, "[TV] write mem [%08lx] <- %08x\n", addr, val);
        } else if (packet[0] == te_op_begin_group && packet[1] == te_op_addl_state && packet[2] == te_op_addl_state_pc && packet[11] == te_op_32b_instr) {
            uint64_t addr = 0;
            uint32_t instr = 0;
            memcpy(&addr, &packet[3], sizeof(addr));
            memcpy(&instr, &packet[12], sizeof(instr));
            fprintf(stderr, "[TV] next pc %08lx instr %08x\n", addr, instr);
        } else {
            
            fprintf(stderr, "[TV] %d packet", num_bytes);
            if (num_bytes < 72) {
                for (uint32_t i = 0; i < num_bytes; i++) {
                    fprintf(stderr, " %02x", packet[i] & 0xFF);
                }
            }
            fprintf(stderr, "\n");
        }
    }

    uint32_t dmi_status() {
        request->dmi_status();
        wait();
        return rsp_data;
    }
    uint32_t dmi_read(uint32_t addr) {
        //fprintf(stderr, "sw dmi_read %x\n", addr);
        request->dmi_read(addr);
        wait();
        return rsp_data;
    }

    void dmi_write(uint32_t addr, uint32_t data) {
        request->dmi_write(addr, data);
        wait();
    }

    void ddr_read(uint32_t addr, bsvvector_Luint8_t_L64 data) {
        request->ddr_read(addr);
        wait();
        if (data)
            memcpy(data, pcis_rsp_data, 64);
    }
    void ddr_write(uint32_t addr, const bsvvector_Luint8_t_L64 data, uint64_t wstrb = 0xFFFFFFFFFFFFFFFFul) {
        request->ddr_write(addr, data, wstrb);
        wait();
    }

    void register_region(uint32_t region, uint32_t objid) {
        request->register_region(region, objid);
    }

    void memory_ready() {
        request->memory_ready();
    }

    uint64_t read_csr(int i) {
        dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | i);
        uint64_t val = dmi_read(5);
        val <<=  32;
        val |= dmi_read(4);
        return val;
    }

    void write_csr(int i, uint64_t val) {
        dmi_write(5, (val >> 32) & 0xFFFFFFFF);
        dmi_write(4, (val >>  0) & 0xFFFFFFFF);
        dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | (1 << 16) | i);
    }

    uint64_t read_gpr(int i) {
        dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | 0x1000 | i);
        uint64_t val = dmi_read(5);
        val <<=  32;
        val |= dmi_read(4);
        return val;
    }

    void write_gpr(int i, uint64_t val) {
        dmi_write(5, (val >> 32) & 0xFFFFFFFF);
        dmi_write(4, (val >>  0) & 0xFFFFFFFF);
        dmi_write(DM_COMMAND_REG, DM_COMMAND_ACCESS_REGISTER | (3 << 20) | (1 << 17) | (1 << 16) | 0x1000 | i);
    }

    void sbcs_wait() {
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

    uint32_t read32(uint32_t addr) {
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

    uint64_t read64(uint32_t addr) {
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

    void write32(uint32_t addr, uint32_t val) {
        if (1 || last_addr != addr) {
            dmi_write(DM_SBCS_REG, SBCS_SBACCESS32);
            dmi_write(DM_SBADDRESS0_REG, addr);
        }
        sbcs_wait();
        dmi_write(DM_SBDATA0_REG, (val >>  0) & 0xFFFFFFFF);
        last_addr = addr + 4;
    }

    void write64(uint32_t addr, uint64_t val) {
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

    void halt(int timeout = 100) {
        dmi_write(DM_CONTROL_REG, DM_CONTROL_HALTREQ | dmi_read(DM_CONTROL_REG));
        for (int i = 0; i < 100; i++) {
            uint32_t status = dmi_read(DM_STATUS_REG);
            if (status & (1 << 9))
                break;
        }
        dmi_write(DM_CONTROL_REG, ~DM_CONTROL_HALTREQ & dmi_read(DM_CONTROL_REG));
    }

    void resume(int timeout = 100) {
        dmi_write(DM_CONTROL_REG, DM_CONTROL_RESUMEREQ | dmi_read(DM_CONTROL_REG));
        for (int i = 0; i < 100; i++) {
            uint32_t status = dmi_read(DM_STATUS_REG);
            if (status & (1 << 17))
                break;
        }
        dmi_write(DM_CONTROL_REG, ~DM_CONTROL_RESUMEREQ & dmi_read(DM_CONTROL_REG));
    }

    void irq_set_levels(uint32_t w1s) {
        request->irq_set_levels(w1s);
    }

    void irq_clear_levels(uint32_t w1c) {
        request->irq_clear_levels(w1c);
    }

    uint32_t read_irq_status() {
        request->read_irq_status();
        wait();
        return rsp_data;
    }

// handlers below here

    void io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid) {
        fprintf(stderr, "io_awaddr awaddr=%08x awlen=%d\n", awaddr, awlen);
        wdata_count = awlen / 8;
        wid = awid;
    }

    void io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid) {
        fprintf(stderr, "io_araddr araddr=%08x arlen=%d\n", araddr, arlen);
        uint64_t instrs[8];
        instrs[0] = (12 << 20); // ebreak
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            request->io_rdata(instrs[i], arid, 0, last);
        }
    }

    void io_wdata(uint64_t wdata, uint8_t wstrb) {
        fprintf(stderr, "io_wdata wdata=%lx wstrb=%x\n", wdata, wstrb);
        wdata_count -= 1;
        if (wdata_count <= 0) {
            fprintf(stderr, "-> io_bdone\n");
            request->io_bdone(wid, 0);
        }
    }

    void irq_status(uint32_t levels) {
        this->rsp_data = levels;
        sem_post(&sem);
    }

    void set_fabric_verbosity(uint8_t verbosity) {
        request->set_fabric_verbosity(verbosity);
    }

 private:

    void wait() {
        //fprintf(stderr, "fpga::wait\n");
        sem_wait(&sem);
    }

};
