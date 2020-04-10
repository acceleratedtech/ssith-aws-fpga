#include "GeneratedTypes.h"

int AWSP2_Request_set_debug_verbosity ( struct PortalInternal *p, const uint8_t verbosity )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_set_debug_verbosity, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_set_debug_verbosity, "AWSP2_Request_set_debug_verbosity")) return 1;
    p->transport->write(p, &temp_working_addr, verbosity);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_set_debug_verbosity << 16) | 2, -1);
    return 0;
};

int AWSP2_Request_set_fabric_verbosity ( struct PortalInternal *p, const uint8_t verbosity )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_set_fabric_verbosity, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_set_fabric_verbosity, "AWSP2_Request_set_fabric_verbosity")) return 1;
    p->transport->write(p, &temp_working_addr, verbosity);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_set_fabric_verbosity << 16) | 2, -1);
    return 0;
};

int AWSP2_Request_dmi_read ( struct PortalInternal *p, const uint8_t req_addr )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_dmi_read, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_dmi_read, "AWSP2_Request_dmi_read")) return 1;
    p->transport->write(p, &temp_working_addr, req_addr);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_dmi_read << 16) | 2, -1);
    return 0;
};

int AWSP2_Request_dmi_write ( struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_dmi_write, 3);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_dmi_write, "AWSP2_Request_dmi_write")) return 1;
    p->transport->write(p, &temp_working_addr, req_addr);
    p->transport->write(p, &temp_working_addr, req_data);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_dmi_write << 16) | 3, -1);
    return 0;
};

int AWSP2_Request_dmi_status ( struct PortalInternal *p )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_dmi_status, 1);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_dmi_status, "AWSP2_Request_dmi_status")) return 1;
    p->transport->write(p, &temp_working_addr, 0);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_dmi_status << 16) | 1, -1);
    return 0;
};

int AWSP2_Request_ddr_read ( struct PortalInternal *p, const uint32_t addr )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_ddr_read, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_ddr_read, "AWSP2_Request_ddr_read")) return 1;
    p->transport->write(p, &temp_working_addr, addr);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_ddr_read << 16) | 2, -1);
    return 0;
};

int AWSP2_Request_ddr_write ( struct PortalInternal *p, const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_ddr_write, 20);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_ddr_write, "AWSP2_Request_ddr_write")) return 1;
    p->transport->write(p, &temp_working_addr, addr);
    p->transport->write(p, &temp_working_addr, data[3]|(((unsigned long)data[2])<<8)|(((unsigned long)data[1])<<16)|(((unsigned long)data[0])<<24));
    p->transport->write(p, &temp_working_addr, data[7]|(((unsigned long)data[6])<<8)|(((unsigned long)data[5])<<16)|(((unsigned long)data[4])<<24));
    p->transport->write(p, &temp_working_addr, data[11]|(((unsigned long)data[10])<<8)|(((unsigned long)data[9])<<16)|(((unsigned long)data[8])<<24));
    p->transport->write(p, &temp_working_addr, data[15]|(((unsigned long)data[14])<<8)|(((unsigned long)data[13])<<16)|(((unsigned long)data[12])<<24));
    p->transport->write(p, &temp_working_addr, data[19]|(((unsigned long)data[18])<<8)|(((unsigned long)data[17])<<16)|(((unsigned long)data[16])<<24));
    p->transport->write(p, &temp_working_addr, data[23]|(((unsigned long)data[22])<<8)|(((unsigned long)data[21])<<16)|(((unsigned long)data[20])<<24));
    p->transport->write(p, &temp_working_addr, data[27]|(((unsigned long)data[26])<<8)|(((unsigned long)data[25])<<16)|(((unsigned long)data[24])<<24));
    p->transport->write(p, &temp_working_addr, data[31]|(((unsigned long)data[30])<<8)|(((unsigned long)data[29])<<16)|(((unsigned long)data[28])<<24));
    p->transport->write(p, &temp_working_addr, data[35]|(((unsigned long)data[34])<<8)|(((unsigned long)data[33])<<16)|(((unsigned long)data[32])<<24));
    p->transport->write(p, &temp_working_addr, data[39]|(((unsigned long)data[38])<<8)|(((unsigned long)data[37])<<16)|(((unsigned long)data[36])<<24));
    p->transport->write(p, &temp_working_addr, data[43]|(((unsigned long)data[42])<<8)|(((unsigned long)data[41])<<16)|(((unsigned long)data[40])<<24));
    p->transport->write(p, &temp_working_addr, data[47]|(((unsigned long)data[46])<<8)|(((unsigned long)data[45])<<16)|(((unsigned long)data[44])<<24));
    p->transport->write(p, &temp_working_addr, data[51]|(((unsigned long)data[50])<<8)|(((unsigned long)data[49])<<16)|(((unsigned long)data[48])<<24));
    p->transport->write(p, &temp_working_addr, data[55]|(((unsigned long)data[54])<<8)|(((unsigned long)data[53])<<16)|(((unsigned long)data[52])<<24));
    p->transport->write(p, &temp_working_addr, data[59]|(((unsigned long)data[58])<<8)|(((unsigned long)data[57])<<16)|(((unsigned long)data[56])<<24));
    p->transport->write(p, &temp_working_addr, data[63]|(((unsigned long)data[62])<<8)|(((unsigned long)data[61])<<16)|(((unsigned long)data[60])<<24));
    p->transport->write(p, &temp_working_addr, (byte_enable>>32));
    p->transport->write(p, &temp_working_addr, byte_enable);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_ddr_write << 16) | 20, -1);
    return 0;
};

int AWSP2_Request_register_region ( struct PortalInternal *p, const uint32_t region, const uint32_t objectId )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_register_region, 3);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_register_region, "AWSP2_Request_register_region")) return 1;
    p->transport->write(p, &temp_working_addr, region);
    p->transport->write(p, &temp_working_addr, objectId);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_register_region << 16) | 3, -1);
    return 0;
};

int AWSP2_Request_memory_ready ( struct PortalInternal *p )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_memory_ready, 1);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_memory_ready, "AWSP2_Request_memory_ready")) return 1;
    p->transport->write(p, &temp_working_addr, 0);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_memory_ready << 16) | 1, -1);
    return 0;
};

int AWSP2_Request_capture_tv_info ( struct PortalInternal *p, const int c )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_capture_tv_info, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_capture_tv_info, "AWSP2_Request_capture_tv_info")) return 1;
    p->transport->write(p, &temp_working_addr, (c & 1L));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_capture_tv_info << 16) | 2, -1);
    return 0;
};

int AWSP2_Request_set_watch_tohost ( struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_set_watch_tohost, 3);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_set_watch_tohost, "AWSP2_Request_set_watch_tohost")) return 1;
    p->transport->write(p, &temp_working_addr, (watch_tohost & 1L));
    p->transport->write(p, &temp_working_addr, tohost_addr);
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_set_watch_tohost << 16) | 3, -1);
    return 0;
};

int AWSP2_Request_io_rdata ( struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_io_rdata, 4);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_io_rdata, "AWSP2_Request_io_rdata")) return 1;
    p->transport->write(p, &temp_working_addr, (data>>39));
    p->transport->write(p, &temp_working_addr, (data>>7));
    p->transport->write(p, &temp_working_addr, (last & 1L)|(((unsigned long)rresp)<<1)|(((unsigned long)rid)<<9)|(((unsigned long)data)<<25));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_io_rdata << 16) | 4, -1);
    return 0;
};

int AWSP2_Request_io_bdone ( struct PortalInternal *p, const uint16_t bid, const uint8_t bresp )
{
    volatile unsigned int* temp_working_addr_start = p->transport->mapchannelReq(p, CHAN_NUM_AWSP2_Request_io_bdone, 2);
    volatile unsigned int* temp_working_addr = temp_working_addr_start;
    if (p->transport->busywait(p, CHAN_NUM_AWSP2_Request_io_bdone, "AWSP2_Request_io_bdone")) return 1;
    p->transport->write(p, &temp_working_addr, bresp|(((unsigned long)bid)<<8));
    p->transport->send(p, temp_working_addr_start, (CHAN_NUM_AWSP2_Request_io_bdone << 16) | 2, -1);
    return 0;
};

AWSP2_RequestCb AWSP2_RequestProxyReq = {
    portal_disconnect,
    AWSP2_Request_set_debug_verbosity,
    AWSP2_Request_set_fabric_verbosity,
    AWSP2_Request_dmi_read,
    AWSP2_Request_dmi_write,
    AWSP2_Request_dmi_status,
    AWSP2_Request_ddr_read,
    AWSP2_Request_ddr_write,
    AWSP2_Request_register_region,
    AWSP2_Request_memory_ready,
    AWSP2_Request_capture_tv_info,
    AWSP2_Request_set_watch_tohost,
    AWSP2_Request_io_rdata,
    AWSP2_Request_io_bdone,
};
AWSP2_RequestCb *pAWSP2_RequestProxyReq = &AWSP2_RequestProxyReq;

const uint32_t AWSP2_Request_reqinfo = 0xd0050;
const char * AWSP2_Request_methodSignatures()
{
    return "{\"register_region\": [\"long\", \"long\"], \"ddr_write\": [\"long\", \"long\", \"long\"], \"ddr_read\": [\"long\"], \"io_bdone\": [\"long\", \"long\"], \"set_fabric_verbosity\": [\"long\"], \"memory_ready\": [], \"io_rdata\": [\"long\", \"long\", \"long\", \"long\"], \"dmi_read\": [\"long\"], \"capture_tv_info\": [\"long\"], \"set_watch_tohost\": [\"long\", \"long\"], \"dmi_write\": [\"long\", \"long\"], \"dmi_status\": [], \"set_debug_verbosity\": [\"long\"]}";
}

int AWSP2_Request_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd)
{
    static int runaway = 0;
    int   tmp __attribute__ ((unused));
    int tmpfd __attribute__ ((unused));
    AWSP2_RequestData tempdata __attribute__ ((unused));
    memset(&tempdata, 0, sizeof(tempdata));
    volatile unsigned int* temp_working_addr = p->transport->mapchannelInd(p, channel);
    switch (channel) {
    case CHAN_NUM_AWSP2_Request_set_debug_verbosity: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.set_debug_verbosity.verbosity = (uint8_t)(((tmp)&0xful));
        ((AWSP2_RequestCb *)p->cb)->set_debug_verbosity(p, tempdata.set_debug_verbosity.verbosity);
      } break;
    case CHAN_NUM_AWSP2_Request_set_fabric_verbosity: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.set_fabric_verbosity.verbosity = (uint8_t)(((tmp)&0xful));
        ((AWSP2_RequestCb *)p->cb)->set_fabric_verbosity(p, tempdata.set_fabric_verbosity.verbosity);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_read: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.dmi_read.req_addr = (uint8_t)(((tmp)&0x7ful));
        ((AWSP2_RequestCb *)p->cb)->dmi_read(p, tempdata.dmi_read.req_addr);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_write: {
        p->transport->recv(p, temp_working_addr, 2, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.dmi_write.req_addr = (uint8_t)(((tmp)&0x7ful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.dmi_write.req_data = (uint32_t)(((tmp)&0xfffffffful));
        ((AWSP2_RequestCb *)p->cb)->dmi_write(p, tempdata.dmi_write.req_addr, tempdata.dmi_write.req_data);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_status: {
        p->transport->recv(p, temp_working_addr, 0, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        ((AWSP2_RequestCb *)p->cb)->dmi_status(p);
      } break;
    case CHAN_NUM_AWSP2_Request_ddr_read: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_read.addr = (uint32_t)(((tmp)&0xfffffffful));
        ((AWSP2_RequestCb *)p->cb)->ddr_read(p, tempdata.ddr_read.addr);
      } break;
    case CHAN_NUM_AWSP2_Request_ddr_write: {
        p->transport->recv(p, temp_working_addr, 19, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.addr = (uint32_t)(((tmp)&0xfffffffful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[3] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[2] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[1] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[0] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[7] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[6] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[5] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[4] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[11] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[10] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[9] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[8] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[15] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[14] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[13] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[12] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[19] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[18] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[17] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[16] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[23] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[22] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[21] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[20] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[27] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[26] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[25] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[24] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[31] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[30] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[29] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[28] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[35] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[34] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[33] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[32] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[39] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[38] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[37] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[36] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[43] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[42] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[41] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[40] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[47] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[46] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[45] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[44] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[51] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[50] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[49] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[48] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[55] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[54] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[53] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[52] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[59] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[58] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[57] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[56] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.data[63] = (uint8_t)(((tmp)&0xfful));
        tempdata.ddr_write.data[62] = (uint8_t)(((tmp>>8)&0xfful));
        tempdata.ddr_write.data[61] = (uint8_t)(((tmp>>16)&0xfful));
        tempdata.ddr_write.data[60] = (uint8_t)(((tmp>>24)&0xfful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.byte_enable = (uint64_t)(((uint64_t)(((tmp)&0xfffffffful))<<32));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.ddr_write.byte_enable |= (uint64_t)(((tmp)&0xfffffffful));
        ((AWSP2_RequestCb *)p->cb)->ddr_write(p, tempdata.ddr_write.addr, tempdata.ddr_write.data, tempdata.ddr_write.byte_enable);
      } break;
    case CHAN_NUM_AWSP2_Request_register_region: {
        p->transport->recv(p, temp_working_addr, 2, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.register_region.region = (uint32_t)(((tmp)&0xfffffffful));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.register_region.objectId = (uint32_t)(((tmp)&0xfffffffful));
        ((AWSP2_RequestCb *)p->cb)->register_region(p, tempdata.register_region.region, tempdata.register_region.objectId);
      } break;
    case CHAN_NUM_AWSP2_Request_memory_ready: {
        p->transport->recv(p, temp_working_addr, 0, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        ((AWSP2_RequestCb *)p->cb)->memory_ready(p);
      } break;
    case CHAN_NUM_AWSP2_Request_capture_tv_info: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.capture_tv_info.c = (int)(((tmp)&0x1ul));
        ((AWSP2_RequestCb *)p->cb)->capture_tv_info(p, tempdata.capture_tv_info.c);
      } break;
    case CHAN_NUM_AWSP2_Request_set_watch_tohost: {
        p->transport->recv(p, temp_working_addr, 2, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.set_watch_tohost.watch_tohost = (int)(((tmp)&0x1ul));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.set_watch_tohost.tohost_addr = (uint32_t)(((tmp)&0xfffffffful));
        ((AWSP2_RequestCb *)p->cb)->set_watch_tohost(p, tempdata.set_watch_tohost.watch_tohost, tempdata.set_watch_tohost.tohost_addr);
      } break;
    case CHAN_NUM_AWSP2_Request_io_rdata: {
        p->transport->recv(p, temp_working_addr, 3, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_rdata.data = (uint64_t)(((uint64_t)(((tmp)&0x1fffffful))<<39));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_rdata.data |= (uint64_t)(((uint64_t)(((tmp)&0xfffffffful))<<7));
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_rdata.last = (int)(((tmp)&0x1ul));
        tempdata.io_rdata.rresp = (uint8_t)(((tmp>>1)&0xfful));
        tempdata.io_rdata.rid = (uint16_t)(((tmp>>9)&0xfffful));
        tempdata.io_rdata.data |= (uint64_t)(((tmp>>25)&0x7ful));
        ((AWSP2_RequestCb *)p->cb)->io_rdata(p, tempdata.io_rdata.data, tempdata.io_rdata.rid, tempdata.io_rdata.rresp, tempdata.io_rdata.last);
      } break;
    case CHAN_NUM_AWSP2_Request_io_bdone: {
        p->transport->recv(p, temp_working_addr, 1, &tmpfd);
        tmp = p->transport->read(p, &temp_working_addr);
        tempdata.io_bdone.bresp = (uint8_t)(((tmp)&0xfful));
        tempdata.io_bdone.bid = (uint16_t)(((tmp>>8)&0xfffful));
        ((AWSP2_RequestCb *)p->cb)->io_bdone(p, tempdata.io_bdone.bid, tempdata.io_bdone.bresp);
      } break;
    default:
        PORTAL_PRINTF("AWSP2_Request_handleMessage: unknown channel 0x%x\n", channel);
        if (runaway++ > 10) {
            PORTAL_PRINTF("AWSP2_Request_handleMessage: too many bogus indications, exiting\n");
#ifndef __KERNEL__
            exit(-1);
#endif
        }
        return 0;
    }
    return 0;
}
