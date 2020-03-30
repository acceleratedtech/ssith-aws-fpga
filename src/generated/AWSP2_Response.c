#include "GeneratedTypes.h"

int AWSP2_Response_dmi_read_data ( struct PortalInternal *p, const uint32_t rsp_data )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_dmi_read_data, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_dmi_read_data, "AWSP2_Response_dmi_read_data")) return 1;
    p->transport->write(p, &temp_working_addr, rsp_data);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_dmi_read_data << 16) | 2, -1);
    return 0;
};

int AWSP2_Response_dmi_status_data ( struct PortalInternal *p, const uint16_t status )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_dmi_status_data, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_dmi_status_data, "AWSP2_Response_dmi_status_data")) return 1;
    p->transport->write(p, &temp_working_addr, status);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_dmi_status_data << 16) | 2, -1);
    return 0;
};

int AWSP2_Response_io_awaddr ( struct PortalInternal *p, const uint32_t awaddr, const uint16_t awlen, const uint16_t awid )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_io_awaddr, 3);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_io_awaddr, "AWSP2_Response_io_awaddr")) return 1;
    p->transport->write(p, &temp_working_addr, awaddr);
    p->transport->write(p, &temp_working_addr, awid|(((unsigned long)awlen)<<16));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_io_awaddr << 16) | 3, -1);
    return 0;
};

int AWSP2_Response_io_araddr ( struct PortalInternal *p, const uint32_t araddr, const uint16_t arlen, const uint16_t arid )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_io_araddr, 3);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_io_araddr, "AWSP2_Response_io_araddr")) return 1;
    p->transport->write(p, &temp_working_addr, araddr);
    p->transport->write(p, &temp_working_addr, arid|(((unsigned long)arlen)<<16));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_io_araddr << 16) | 3, -1);
    return 0;
};

int AWSP2_Response_io_wdata ( struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_io_wdata, 4);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_io_wdata, "AWSP2_Response_io_wdata")) return 1;
    p->transport->write(p, &temp_working_addr, (wdata>>56));
    p->transport->write(p, &temp_working_addr, (wdata>>24));
    p->transport->write(p, &temp_working_addr, wstrb|(((unsigned long)wdata)<<8));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_io_wdata << 16) | 4, -1);
    return 0;
};

int AWSP2_Response_tandem_packet ( struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Response_tandem_packet, 20);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Response_tandem_packet, "AWSP2_Response_tandem_packet")) return 1;
    p->transport->write(p, &temp_working_addr, num_bytes);
    p->transport->write(p, &temp_working_addr, bytes[3]|(((unsigned long)bytes[2])<<8)|(((unsigned long)bytes[1])<<16)|(((unsigned long)bytes[0])<<24));
    p->transport->write(p, &temp_working_addr, bytes[7]|(((unsigned long)bytes[6])<<8)|(((unsigned long)bytes[5])<<16)|(((unsigned long)bytes[4])<<24));
    p->transport->write(p, &temp_working_addr, bytes[11]|(((unsigned long)bytes[10])<<8)|(((unsigned long)bytes[9])<<16)|(((unsigned long)bytes[8])<<24));
    p->transport->write(p, &temp_working_addr, bytes[15]|(((unsigned long)bytes[14])<<8)|(((unsigned long)bytes[13])<<16)|(((unsigned long)bytes[12])<<24));
    p->transport->write(p, &temp_working_addr, bytes[19]|(((unsigned long)bytes[18])<<8)|(((unsigned long)bytes[17])<<16)|(((unsigned long)bytes[16])<<24));
    p->transport->write(p, &temp_working_addr, bytes[23]|(((unsigned long)bytes[22])<<8)|(((unsigned long)bytes[21])<<16)|(((unsigned long)bytes[20])<<24));
    p->transport->write(p, &temp_working_addr, bytes[27]|(((unsigned long)bytes[26])<<8)|(((unsigned long)bytes[25])<<16)|(((unsigned long)bytes[24])<<24));
    p->transport->write(p, &temp_working_addr, bytes[31]|(((unsigned long)bytes[30])<<8)|(((unsigned long)bytes[29])<<16)|(((unsigned long)bytes[28])<<24));
    p->transport->write(p, &temp_working_addr, bytes[35]|(((unsigned long)bytes[34])<<8)|(((unsigned long)bytes[33])<<16)|(((unsigned long)bytes[32])<<24));
    p->transport->write(p, &temp_working_addr, bytes[39]|(((unsigned long)bytes[38])<<8)|(((unsigned long)bytes[37])<<16)|(((unsigned long)bytes[36])<<24));
    p->transport->write(p, &temp_working_addr, bytes[43]|(((unsigned long)bytes[42])<<8)|(((unsigned long)bytes[41])<<16)|(((unsigned long)bytes[40])<<24));
    p->transport->write(p, &temp_working_addr, bytes[47]|(((unsigned long)bytes[46])<<8)|(((unsigned long)bytes[45])<<16)|(((unsigned long)bytes[44])<<24));
    p->transport->write(p, &temp_working_addr, bytes[51]|(((unsigned long)bytes[50])<<8)|(((unsigned long)bytes[49])<<16)|(((unsigned long)bytes[48])<<24));
    p->transport->write(p, &temp_working_addr, bytes[55]|(((unsigned long)bytes[54])<<8)|(((unsigned long)bytes[53])<<16)|(((unsigned long)bytes[52])<<24));
    p->transport->write(p, &temp_working_addr, bytes[59]|(((unsigned long)bytes[58])<<8)|(((unsigned long)bytes[57])<<16)|(((unsigned long)bytes[56])<<24));
    p->transport->write(p, &temp_working_addr, bytes[63]|(((unsigned long)bytes[62])<<8)|(((unsigned long)bytes[61])<<16)|(((unsigned long)bytes[60])<<24));
    p->transport->write(p, &temp_working_addr, bytes[67]|(((unsigned long)bytes[66])<<8)|(((unsigned long)bytes[65])<<16)|(((unsigned long)bytes[64])<<24));
    p->transport->write(p, &temp_working_addr, bytes[71]|(((unsigned long)bytes[70])<<8)|(((unsigned long)bytes[69])<<16)|(((unsigned long)bytes[68])<<24));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Response_tandem_packet << 16) | 20, -1);
    return 0;
};

AWSP2_ResponseCb AWSP2_ResponseProxyReq = {
    portal_disconnect,
    AWSP2_Response_dmi_read_data,
    AWSP2_Response_dmi_status_data,
    AWSP2_Response_io_awaddr,
    AWSP2_Response_io_araddr,
    AWSP2_Response_io_wdata,
    AWSP2_Response_tandem_packet,
};
AWSP2_ResponseCb *pAWSP2_ResponseProxyReq = &AWSP2_ResponseProxyReq;

const uint32_t AWSP2_Response_reqinfo = 0x60050;
const char * AWSP2_Response_methodSignatures()
{
    return "{\"tandem_packet\": [\"long\", \"long\"], \"io_wdata\": [\"long\", \"long\"], \"dmi_status_data\": [\"long\"], \"io_araddr\": [\"long\", \"long\", \"long\"], \"io_awaddr\": [\"long\", \"long\", \"long\"], \"dmi_read_data\": [\"long\"]}";
}

int AWSP2_Response_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd)
{
    static int runaway = 0;
    int   tmp __attribute__ ((unused));
    int tmpfd __attribute__ ((unused));
    AWSP2_ResponseData tempdata __attribute__ ((unused));
    memset(&tempdata, 0, sizeof(tempdata));
    volatile unsigned int* temp_working_addr = p->transport->mapchannelInd(p, channel);
    switch (channel) {
    case CHAN_NUM_AWSP2_Response_dmi_read_data: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.dmi_read_data.rsp_data = (uint32_t)(((tmp)&0xfffffffful));
        ((AWSP2_ResponseCb *)p->cb)->dmi_read_data(p, tempdata.dmi_read_data.rsp_data);
      } break;
    case CHAN_NUM_AWSP2_Response_dmi_status_data: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.dmi_status_data.status = (uint16_t)(((tmp)&0xfffful));
        ((AWSP2_ResponseCb *)p->cb)->dmi_status_data(p, tempdata.dmi_status_data.status);
      } break;
    case CHAN_NUM_AWSP2_Response_io_awaddr: {
        p->transport->recv(p, temp_working_addr, 2, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_awaddr.awaddr = (uint32_t)(((tmp)&0xfffffffful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_awaddr.awid = (uint16_t)(((tmp)&0xfffful));
        tempdata.io_awaddr.awlen = (uint16_t)(((tmp>>16)&0xfffful));
        ((AWSP2_ResponseCb *)p->cb)->io_awaddr(p, tempdata.io_awaddr.awaddr, tempdata.io_awaddr.awlen, tempdata.io_awaddr.awid);
      } break;
    case CHAN_NUM_AWSP2_Response_io_araddr: {
        p->transport->recv(p, temp_working_addr, 2, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_araddr.araddr = (uint32_t)(((tmp)&0xfffffffful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_araddr.arid = (uint16_t)(((tmp)&0xfffful));
        tempdata.io_araddr.arlen = (uint16_t)(((tmp>>16)&0xfffful));
        ((AWSP2_ResponseCb *)p->cb)->io_araddr(p, tempdata.io_araddr.araddr, tempdata.io_araddr.arlen, tempdata.io_araddr.arid);
      } break;
    case CHAN_NUM_AWSP2_Response_io_wdata: {
        p->transport->recv(p, temp_working_addr, 3, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_wdata.wdata = (uint64_t)(((uint64_t)(((tmp)&0xfful))<<56));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_wdata.wdata |= (uint64_t)(((uint64_t)(((tmp)&0xfffffffful))<<24));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_wdata.wstrb = (uint8_t)(((tmp)&0xfful));
        tempdata.io_wdata.wdata |= (uint64_t)(((tmp>>8)&0xfffffful));
        ((AWSP2_ResponseCb *)p->cb)->io_wdata(p, tempdata.io_wdata.wdata, tempdata.io_wdata.wstrb);
      } break;
    case CHAN_NUM_AWSP2_Response_tandem_packet: {
        p->transport->recv(p, temp_working_addr, 19, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.num_bytes = (uint32_t)(((tmp)&0xfffffffful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[3] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[2] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[1] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[0] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[7] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[6] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[5] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[4] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[11] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[10] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[9] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[8] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[15] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[14] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[13] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[12] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[19] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[18] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[17] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[16] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[23] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[22] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[21] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[20] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[27] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[26] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[25] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[24] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[31] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[30] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[29] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[28] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[35] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[34] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[33] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[32] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[39] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[38] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[37] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[36] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[43] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[42] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[41] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[40] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[47] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[46] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[45] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[44] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[51] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[50] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[49] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[48] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[55] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[54] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[53] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[52] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[59] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[58] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[57] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[56] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[63] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[62] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[61] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[60] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[67] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[66] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[65] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[64] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.tandem_packet.bytes[71] = (uint8_t)(((tmp)&0xfful));
        tempdata.tandem_packet.bytes[70] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.tandem_packet.bytes[69] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.tandem_packet.bytes[68] = (uint8_t)(((tmp>>24)&0xfful));
        ((AWSP2_ResponseCb *)p->cb)->tandem_packet(p, tempdata.tandem_packet.num_bytes, tempdata.tandem_packet.bytes);
      } break;
    default:
        PORTAL_PRINTF("AWSP2_Response_handleMessage: unknown channel 0x%x\n", channel);
        if (runaway++ > 10) {
            PORTAL_PRINTF("AWSP2_Response_handleMessage: too many bogus indications, exiting\n");
#ifndef __KERNEL__
            exit(-1);
#endif
        }
        return 0;
    }
    return 0;
}
