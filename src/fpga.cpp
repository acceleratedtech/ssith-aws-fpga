
#include "fpga.h"

AWSP2::AWSP2(int id, const Rom &rom) 
    : AWSP2_ResponseWrapper(id), rom(rom), last_addr(0), wdata_count(0), wid(0), start_of_line(1) {
    sem_init(&sem, 0, 0);
    request = new AWSP2_RequestProxy(IfcNames_AWSP2_RequestS2H);
    stop_capture = 0;
    display_tandem = 1;
}

AWSP2::~AWSP2() {
}

void AWSP2::tandem_packet(const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes) 
{
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

void AWSP2::io_awaddr(uint32_t awaddr, uint16_t awlen, uint16_t awid) {
    this->awaddr = awaddr;
    PhysMemoryRange *pr = virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
	uint32_t offset = awaddr - pr->addr;
	fprintf(stderr, "virtio awaddr %08x device addr %08lx offset %08x len %d\n", awaddr, pr->addr, offset, awlen);
    } else if (awaddr == 0x60000000) {
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

void AWSP2::io_araddr(uint32_t araddr, uint16_t arlen, uint16_t arid)
{
    PhysMemoryRange *pr = virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
	uint32_t offset = araddr - pr->addr;
	int size_log2 = 4;
	fprintf(stderr, "virtio araddr %08x device addr %08lx offset %08x len %d\n", awaddr, pr->addr, offset, arlen);
        for (int i = 0; i < arlen / 8; i++) {
            int last = i == ((arlen / 8) - 1);
            //fprintf(stderr, "io_rdata %08lx\n", rom.data[offset + i]);
	    uint32_t val = pr->read_func(pr->opaque, offset, size_log2);
            request->io_rdata(val, arid, 0, last);
	    offset += 4;
        }
    } else if (rom.base <= araddr && araddr < rom.limit) {
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

void AWSP2::io_wdata(uint64_t wdata, uint8_t wstrb) {
    PhysMemoryRange *pr = virtio_devices.get_phys_mem_range(awaddr);
    if (pr) {
	int size_log2 = 4;
	uint32_t offset = awaddr - pr->addr;
	pr->write_func(pr->opaque, offset, wdata, size_log2);
    } else if (awaddr == 0x60000000) {
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

void AWSP2::console_putchar(uint64_t wdata) {
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
