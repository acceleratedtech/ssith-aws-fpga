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


struct Rom {
  uint64_t base;
  uint64_t limit;
  uint64_t *data;
};

class AWSP2 : public AWSP2_ResponseWrapper {
    sem_t sem;
    AWSP2_RequestProxy *request;
    Rom rom;
    uint32_t rsp_data;
    uint32_t last_addr;
    uint32_t awaddr;
    uint32_t wdata_count;
    uint32_t wid;
    int start_of_line;
    int stop_capture;
    int display_tandem;
    bsvvector_Luint8_t_L64 pcis_rsp_data;
public:
 AWSP2(int id, const Rom &rom)
    : AWSP2_ResponseWrapper(id), rom(rom), last_addr(0), wdata_count(0), wid(0), start_of_line(1) {
        sem_init(&sem, 0, 0);
        request = new AWSP2_RequestProxy(IfcNames_AWSP2_RequestS2H);
	stop_capture = 0;
	display_tandem = 1;
    }
    virtual void dmi_read_data(uint32_t rsp_data) {
        //fprintf(stderr, "dmi_read_data data=%08x\n", rsp_data);
        this->rsp_data = rsp_data;
        sem_post(&sem);
    }
    void ddr_data ( const bsvvector_Luint8_t_L64 data ) {
	memcpy(&pcis_rsp_data, data, 64);
	sem_post(&sem);
    }
    virtual void dmi_status_data(uint16_t rsp_data) {
        this->rsp_data = rsp_data;
        sem_post(&sem);
    }
    void capture_tv_info(int c, int display = 1) {
        request->capture_tv_info(c);
	display_tandem = display;
    }
    virtual void tandem_packet(const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes) {
	if (stop_capture)
	    return;

        uint8_t packet[72];
	int matched_pc = 0;
        for (int i = 0; i < 72; i++)
            packet[i] = bytes[71 - i];
        //fprintf(stderr, "[TV] %x %x %x %x\n", packet[0], packet[1], packet[2], packet[11]);
        if (packet[0] == te_op_begin_group && packet[1] == te_op_state_init && packet[2] == te_op_mem_req && packet[11] == 0x21) {
            uint64_t addr = 0;
            uint32_t val = 0;
            memcpy(&addr, &packet[3], sizeof(addr));
            memcpy(&val, &packet[12], sizeof(val));
            if (display_tandem) fprintf(stderr, "[TV] write mem [%08lx] <- %08x\n", addr, val);
        } else if (packet[0] == te_op_begin_group) {
            int offset = 1;
            if (display_tandem) fprintf(stderr, "[TV]");
            while (offset < num_bytes) {
                if (packet[offset] == te_op_addl_state) {
                    offset++;
                    if (packet[offset] == te_op_addl_state_priv) {
                        offset++;
                        if (display_tandem) fprintf(stderr, " priv %x", packet[offset]);
                        offset++;
                    } else if (packet[offset] == te_op_addl_state_pc) {
                        offset++;
                        uint64_t addr = 0;
                        memcpy(&addr, &packet[offset], sizeof(addr));
                        offset += sizeof(addr);
			if ((addr & 0xffffffffffffffc0ul) == 0xffffffe0006591c0ul)
			  matched_pc = 1;
                        if (display_tandem) fprintf(stderr, " next pc %08lx", addr);
			if (addr == 0x82000130) {
			    if (display_tandem) fprintf(stderr, " matched STOP pc %08lx, stopping capture", addr);
			    stop_capture = 1;
			}
                    } else if (packet[offset] == te_op_addl_state_data8) {
                        offset++;
                        uint8_t data = 0;
                        memcpy(&data, &packet[offset], sizeof(data));
                        offset += sizeof(data);
                        if (display_tandem) fprintf(stderr, " data8 %02x", data);
                    } else if (packet[offset] == te_op_addl_state_data16) {
                        offset++;
                        uint16_t data = 0;
                        memcpy(&data, &packet[offset], sizeof(data));
                        offset += sizeof(data);
                        if (display_tandem) fprintf(stderr, " data16 %04x", data);
                    } else if (packet[offset] == te_op_addl_state_data32) {
                        offset++;
                        uint32_t data = 0;
                        memcpy(&data, &packet[offset], sizeof(data));
                        offset += sizeof(data);
                        if (display_tandem) fprintf(stderr, " data32 %08x", data);
                    } else if (packet[offset] == te_op_addl_state_data64) {
                        offset++;
                        uint64_t data = 0;
                        memcpy(&data, &packet[offset], sizeof(data));
                        offset += sizeof(data);
                        if (display_tandem) fprintf(stderr, " data64 %08lx", data);
                    } else if (packet[offset] == te_op_addl_state_eaddr) {
                        offset++;
                        uint64_t data = 0;
                        memcpy(&data, &packet[offset], sizeof(data));
                        offset += sizeof(data);
                        if (display_tandem) fprintf(stderr, " eaddr %08lx", data);
                    } else {
                        break;
                    }
                } else if (packet[offset] == te_op_16b_instr) {
                    offset++;
                    uint16_t instr = 0;
                    memcpy(&instr, &packet[offset], sizeof(instr));
                    offset += sizeof(instr);
                    if (display_tandem) fprintf(stderr, " instr %04x", instr);
                } else if (packet[offset] == te_op_32b_instr) {
                    offset++;
                    uint32_t instr = 0;
                    memcpy(&instr, &packet[offset], sizeof(instr));
                    offset += sizeof(instr);
                    if (display_tandem) fprintf(stderr, " instr %08x", instr);
                } else if (packet[offset] == te_op_full_reg) {
                    offset++;
                    uint16_t regnum = 0;
                    uint64_t regval = 0;
                    memcpy(&regnum, &packet[offset], sizeof(regnum));
                    offset += sizeof(regnum);
                    memcpy(&regval, &packet[offset], sizeof(regval));
                    offset += sizeof(regval);

                    if (display_tandem) fprintf(stderr, " reg %x val %08lx", regnum, regval);
                } else if (packet[offset] == te_op_end_group) {
                    offset++;
                } else {
                    break;
                }
            }
            if (display_tandem) fprintf(stderr, "\n");

            if (offset < num_bytes) {
                if (display_tandem) fprintf(stderr, " len %d offset %d op %x\n", num_bytes, offset, packet[offset]);
                for (uint32_t i = 0; i < num_bytes; i++) {
                    if (display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
                }
                if (display_tandem) fprintf(stderr, "\n");
            }
        } else {
            
            if (display_tandem) fprintf(stderr, "[TV] %d packet", num_bytes);
            if (num_bytes < 72) {
                for (uint32_t i = 0; i < num_bytes; i++) {
                    if (display_tandem) fprintf(stderr, " %02x", packet[i] & 0xFF);
                }
            }
            if (display_tandem) fprintf(stderr, "\n");
        }
	if (matched_pc)
	  display_tandem = 1;
    }

    void wait() {
        //fprintf(stderr, "fpga::wait\n");
        sem_wait(&sem);
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

    void io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid) {
        this->awaddr = awaddr;
        if (awaddr == 0x60000000) {
            // UART 
	} else if (awaddr == 0x10001000 || awaddr == 0x50001000) {
	    // tohost
        } else if (awaddr == 0x10001008 || awaddr == 0x50001008) {
	    // fromhost
	} else {
            fprintf(stderr, "io_awaddr awaddr=%08x awlen=%d\n", awaddr, awlen);
        }
        wdata_count = awlen / 8;
        wid = awid;
    }

    void io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid) {
        if (rom.base <= araddr && araddr < rom.limit) {
            int offset = (araddr - rom.base) / 8;
            //fprintf(stderr, "rom offset %x data %08lx\n", (int)(araddr - rom.base), rom.data[offset]);
            for (int i = 0; i < arlen / 8; i++) {
                int last = i == ((arlen / 8) - 1);
                //fprintf(stderr, "io_rdata %08lx\n", rom.data[offset + i]);
                request->io_rdata(rom.data[offset + i], arid, 0, last);
            }
        } else {
            if (araddr != 0x10001000 && araddr != 0x10001008 && araddr != 0x50001000 && araddr != 0x50001008)
                fprintf(stderr, "io_araddr araddr=%08x arlen=%d\n", araddr, arlen);
            for (int i = 0; i < arlen / 8; i++) {
                int last = i == ((arlen / 8) - 1);
		// 0xbad0beef
                request->io_rdata(0, arid, 0, last);
            }       
        }
        
    }

    void console_putchar(uint64_t wdata) {
	if (start_of_line) {
	    printf("\nCONSOLE: ");
	    start_of_line = 0;
	}
	fputc(wdata, stdout);
	//fprintf(stderr, "CONSOLE: %c\n", (int)wdata);
	if (wdata == '\n') {
	    printf("\n");
	    fflush(stdout);
	    start_of_line = 1;
	}
    }

    void io_wdata(uint64_t wdata, uint8_t wstrb) {
        if (awaddr == 0x60000000) {
	    console_putchar(wdata);
	} else if (awaddr == 0x10001000 || awaddr == 0x50001000) {
	    // tohost
	    uint8_t dev = (wdata >> 56) & 0xFF;
	    uint8_t cmd = (wdata >> 48) & 0xFF;
	    uint64_t payload = wdata & 0x0000FFFFFFFFFFFFul;
	    if (dev == 1 && cmd == 1) {
		// putchar
		console_putchar(payload);
	    } else {
		fprintf(stderr, "\nHTIF: dev=%d cmd=%02x payload=%08lx\n", dev, cmd, payload);
	    }
        } else if (awaddr == 0x10001008) {
	    fprintf(stderr, "\nHTIF: awaddr %08x wdata=%08lx\n", awaddr, wdata);
        } else {
            fprintf(stderr, "io_wdata wdata=%lx wstrb=%x\n", wdata, wstrb);
        }
        wdata_count -= 1;
        if (wdata_count <= 0) {
            if (awaddr != 0x60000000
		&& awaddr != 0x10001008
		&& awaddr != 0x10001000
		&& awaddr != 0x50001008)
		fprintf(stderr, "-> io_bdone %08x\n", awaddr);
            request->io_bdone(wid, 0);
        }
    }

    void set_fabric_verbosity(uint8_t verbosity) {
        request->set_fabric_verbosity(verbosity);
    }

};
