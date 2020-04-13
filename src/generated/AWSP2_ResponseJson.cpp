#include "GeneratedTypes.h"
#ifdef PORTAL_JSON
#include "jsoncpp/json/json.h"

int AWSP2_ResponseJson_dmi_read_data ( struct PortalInternal *p, const uint32_t rsp_data )
{
    Json::Value request;
    request.append(Json::Value("dmi_read_data"));
    request.append((Json::UInt64)rsp_data);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_dmi_read_data);
    return 0;
};

int AWSP2_ResponseJson_dmi_status_data ( struct PortalInternal *p, const uint16_t status )
{
    Json::Value request;
    request.append(Json::Value("dmi_status_data"));
    request.append((Json::UInt64)status);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_dmi_status_data);
    return 0;
};

int AWSP2_ResponseJson_ddr_data ( struct PortalInternal *p, const bsvvector_Luint8_t_L64 data )
{
    Json::Value request;
    request.append(Json::Value("ddr_data"));
    Json::Value dataVector;
    dataVector.append((Json::UInt64)data[0]);
    dataVector.append((Json::UInt64)data[1]);
    dataVector.append((Json::UInt64)data[2]);
    dataVector.append((Json::UInt64)data[3]);
    dataVector.append((Json::UInt64)data[4]);
    dataVector.append((Json::UInt64)data[5]);
    dataVector.append((Json::UInt64)data[6]);
    dataVector.append((Json::UInt64)data[7]);
    dataVector.append((Json::UInt64)data[8]);
    dataVector.append((Json::UInt64)data[9]);
    dataVector.append((Json::UInt64)data[10]);
    dataVector.append((Json::UInt64)data[11]);
    dataVector.append((Json::UInt64)data[12]);
    dataVector.append((Json::UInt64)data[13]);
    dataVector.append((Json::UInt64)data[14]);
    dataVector.append((Json::UInt64)data[15]);
    dataVector.append((Json::UInt64)data[16]);
    dataVector.append((Json::UInt64)data[17]);
    dataVector.append((Json::UInt64)data[18]);
    dataVector.append((Json::UInt64)data[19]);
    dataVector.append((Json::UInt64)data[20]);
    dataVector.append((Json::UInt64)data[21]);
    dataVector.append((Json::UInt64)data[22]);
    dataVector.append((Json::UInt64)data[23]);
    dataVector.append((Json::UInt64)data[24]);
    dataVector.append((Json::UInt64)data[25]);
    dataVector.append((Json::UInt64)data[26]);
    dataVector.append((Json::UInt64)data[27]);
    dataVector.append((Json::UInt64)data[28]);
    dataVector.append((Json::UInt64)data[29]);
    dataVector.append((Json::UInt64)data[30]);
    dataVector.append((Json::UInt64)data[31]);
    dataVector.append((Json::UInt64)data[32]);
    dataVector.append((Json::UInt64)data[33]);
    dataVector.append((Json::UInt64)data[34]);
    dataVector.append((Json::UInt64)data[35]);
    dataVector.append((Json::UInt64)data[36]);
    dataVector.append((Json::UInt64)data[37]);
    dataVector.append((Json::UInt64)data[38]);
    dataVector.append((Json::UInt64)data[39]);
    dataVector.append((Json::UInt64)data[40]);
    dataVector.append((Json::UInt64)data[41]);
    dataVector.append((Json::UInt64)data[42]);
    dataVector.append((Json::UInt64)data[43]);
    dataVector.append((Json::UInt64)data[44]);
    dataVector.append((Json::UInt64)data[45]);
    dataVector.append((Json::UInt64)data[46]);
    dataVector.append((Json::UInt64)data[47]);
    dataVector.append((Json::UInt64)data[48]);
    dataVector.append((Json::UInt64)data[49]);
    dataVector.append((Json::UInt64)data[50]);
    dataVector.append((Json::UInt64)data[51]);
    dataVector.append((Json::UInt64)data[52]);
    dataVector.append((Json::UInt64)data[53]);
    dataVector.append((Json::UInt64)data[54]);
    dataVector.append((Json::UInt64)data[55]);
    dataVector.append((Json::UInt64)data[56]);
    dataVector.append((Json::UInt64)data[57]);
    dataVector.append((Json::UInt64)data[58]);
    dataVector.append((Json::UInt64)data[59]);
    dataVector.append((Json::UInt64)data[60]);
    dataVector.append((Json::UInt64)data[61]);
    dataVector.append((Json::UInt64)data[62]);
    dataVector.append((Json::UInt64)data[63]);
    request.append(dataVector);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_ddr_data);
    return 0;
};

int AWSP2_ResponseJson_io_awaddr ( struct PortalInternal *p, const uint32_t awaddr, const uint16_t awlen, const uint16_t awid )
{
    Json::Value request;
    request.append(Json::Value("io_awaddr"));
    request.append((Json::UInt64)awaddr);
    request.append((Json::UInt64)awlen);
    request.append((Json::UInt64)awid);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_io_awaddr);
    return 0;
};

int AWSP2_ResponseJson_io_araddr ( struct PortalInternal *p, const uint32_t araddr, const uint16_t arlen, const uint16_t arid )
{
    Json::Value request;
    request.append(Json::Value("io_araddr"));
    request.append((Json::UInt64)araddr);
    request.append((Json::UInt64)arlen);
    request.append((Json::UInt64)arid);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_io_araddr);
    return 0;
};

int AWSP2_ResponseJson_io_wdata ( struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb )
{
    Json::Value request;
    request.append(Json::Value("io_wdata"));
    request.append((Json::UInt64)wdata);
    request.append((Json::UInt64)wstrb);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_io_wdata);
    return 0;
};

int AWSP2_ResponseJson_irq_status ( struct PortalInternal *p, const uint32_t levels )
{
    Json::Value request;
    request.append(Json::Value("irq_status"));
    request.append((Json::UInt64)levels);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_irq_status);
    return 0;
};

int AWSP2_ResponseJson_tandem_packet ( struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes )
{
    Json::Value request;
    request.append(Json::Value("tandem_packet"));
    request.append((Json::UInt64)num_bytes);
    Json::Value bytesVector;
    bytesVector.append((Json::UInt64)bytes[0]);
    bytesVector.append((Json::UInt64)bytes[1]);
    bytesVector.append((Json::UInt64)bytes[2]);
    bytesVector.append((Json::UInt64)bytes[3]);
    bytesVector.append((Json::UInt64)bytes[4]);
    bytesVector.append((Json::UInt64)bytes[5]);
    bytesVector.append((Json::UInt64)bytes[6]);
    bytesVector.append((Json::UInt64)bytes[7]);
    bytesVector.append((Json::UInt64)bytes[8]);
    bytesVector.append((Json::UInt64)bytes[9]);
    bytesVector.append((Json::UInt64)bytes[10]);
    bytesVector.append((Json::UInt64)bytes[11]);
    bytesVector.append((Json::UInt64)bytes[12]);
    bytesVector.append((Json::UInt64)bytes[13]);
    bytesVector.append((Json::UInt64)bytes[14]);
    bytesVector.append((Json::UInt64)bytes[15]);
    bytesVector.append((Json::UInt64)bytes[16]);
    bytesVector.append((Json::UInt64)bytes[17]);
    bytesVector.append((Json::UInt64)bytes[18]);
    bytesVector.append((Json::UInt64)bytes[19]);
    bytesVector.append((Json::UInt64)bytes[20]);
    bytesVector.append((Json::UInt64)bytes[21]);
    bytesVector.append((Json::UInt64)bytes[22]);
    bytesVector.append((Json::UInt64)bytes[23]);
    bytesVector.append((Json::UInt64)bytes[24]);
    bytesVector.append((Json::UInt64)bytes[25]);
    bytesVector.append((Json::UInt64)bytes[26]);
    bytesVector.append((Json::UInt64)bytes[27]);
    bytesVector.append((Json::UInt64)bytes[28]);
    bytesVector.append((Json::UInt64)bytes[29]);
    bytesVector.append((Json::UInt64)bytes[30]);
    bytesVector.append((Json::UInt64)bytes[31]);
    bytesVector.append((Json::UInt64)bytes[32]);
    bytesVector.append((Json::UInt64)bytes[33]);
    bytesVector.append((Json::UInt64)bytes[34]);
    bytesVector.append((Json::UInt64)bytes[35]);
    bytesVector.append((Json::UInt64)bytes[36]);
    bytesVector.append((Json::UInt64)bytes[37]);
    bytesVector.append((Json::UInt64)bytes[38]);
    bytesVector.append((Json::UInt64)bytes[39]);
    bytesVector.append((Json::UInt64)bytes[40]);
    bytesVector.append((Json::UInt64)bytes[41]);
    bytesVector.append((Json::UInt64)bytes[42]);
    bytesVector.append((Json::UInt64)bytes[43]);
    bytesVector.append((Json::UInt64)bytes[44]);
    bytesVector.append((Json::UInt64)bytes[45]);
    bytesVector.append((Json::UInt64)bytes[46]);
    bytesVector.append((Json::UInt64)bytes[47]);
    bytesVector.append((Json::UInt64)bytes[48]);
    bytesVector.append((Json::UInt64)bytes[49]);
    bytesVector.append((Json::UInt64)bytes[50]);
    bytesVector.append((Json::UInt64)bytes[51]);
    bytesVector.append((Json::UInt64)bytes[52]);
    bytesVector.append((Json::UInt64)bytes[53]);
    bytesVector.append((Json::UInt64)bytes[54]);
    bytesVector.append((Json::UInt64)bytes[55]);
    bytesVector.append((Json::UInt64)bytes[56]);
    bytesVector.append((Json::UInt64)bytes[57]);
    bytesVector.append((Json::UInt64)bytes[58]);
    bytesVector.append((Json::UInt64)bytes[59]);
    bytesVector.append((Json::UInt64)bytes[60]);
    bytesVector.append((Json::UInt64)bytes[61]);
    bytesVector.append((Json::UInt64)bytes[62]);
    bytesVector.append((Json::UInt64)bytes[63]);
    bytesVector.append((Json::UInt64)bytes[64]);
    bytesVector.append((Json::UInt64)bytes[65]);
    bytesVector.append((Json::UInt64)bytes[66]);
    bytesVector.append((Json::UInt64)bytes[67]);
    bytesVector.append((Json::UInt64)bytes[68]);
    bytesVector.append((Json::UInt64)bytes[69]);
    bytesVector.append((Json::UInt64)bytes[70]);
    bytesVector.append((Json::UInt64)bytes[71]);
    request.append(bytesVector);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Response_tandem_packet);
    return 0;
};

AWSP2_ResponseCb AWSP2_ResponseJsonProxyReq = {
    portal_disconnect,
    AWSP2_ResponseJson_dmi_read_data,
    AWSP2_ResponseJson_dmi_status_data,
    AWSP2_ResponseJson_ddr_data,
    AWSP2_ResponseJson_io_awaddr,
    AWSP2_ResponseJson_io_araddr,
    AWSP2_ResponseJson_io_wdata,
    AWSP2_ResponseJson_irq_status,
    AWSP2_ResponseJson_tandem_packet,
};
AWSP2_ResponseCb *pAWSP2_ResponseJsonProxyReq = &AWSP2_ResponseJsonProxyReq;
const char * AWSP2_ResponseJson_methodSignatures()
{
    return "{\"tandem_packet\": [\"long\", \"long\"], \"io_wdata\": [\"long\", \"long\"], \"irq_status\": [\"long\"], \"ddr_data\": [\"long\"], \"dmi_status_data\": [\"long\"], \"io_araddr\": [\"long\", \"long\", \"long\"], \"io_awaddr\": [\"long\", \"long\", \"long\"], \"dmi_read_data\": [\"long\"]}";
}

int AWSP2_ResponseJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd)
{
    static int runaway = 0;
    int   tmp __attribute__ ((unused));
    int tmpfd __attribute__ ((unused));
    AWSP2_ResponseData tempdata __attribute__ ((unused));
    memset(&tempdata, 0, sizeof(tempdata));
    Json::Value msg = Json::Value(connectalJsonReceive(p));
    switch (channel) {
    case CHAN_NUM_AWSP2_Response_dmi_read_data: {
        ((AWSP2_ResponseCb *)p->cb)->dmi_read_data(p, tempdata.dmi_read_data.rsp_data);
      } break;
    case CHAN_NUM_AWSP2_Response_dmi_status_data: {
        ((AWSP2_ResponseCb *)p->cb)->dmi_status_data(p, tempdata.dmi_status_data.status);
      } break;
    case CHAN_NUM_AWSP2_Response_ddr_data: {
        ((AWSP2_ResponseCb *)p->cb)->ddr_data(p, tempdata.ddr_data.data);
      } break;
    case CHAN_NUM_AWSP2_Response_io_awaddr: {
        ((AWSP2_ResponseCb *)p->cb)->io_awaddr(p, tempdata.io_awaddr.awaddr, tempdata.io_awaddr.awlen, tempdata.io_awaddr.awid);
      } break;
    case CHAN_NUM_AWSP2_Response_io_araddr: {
        ((AWSP2_ResponseCb *)p->cb)->io_araddr(p, tempdata.io_araddr.araddr, tempdata.io_araddr.arlen, tempdata.io_araddr.arid);
      } break;
    case CHAN_NUM_AWSP2_Response_io_wdata: {
        ((AWSP2_ResponseCb *)p->cb)->io_wdata(p, tempdata.io_wdata.wdata, tempdata.io_wdata.wstrb);
      } break;
    case CHAN_NUM_AWSP2_Response_irq_status: {
        ((AWSP2_ResponseCb *)p->cb)->irq_status(p, tempdata.irq_status.levels);
      } break;
    case CHAN_NUM_AWSP2_Response_tandem_packet: {
        ((AWSP2_ResponseCb *)p->cb)->tandem_packet(p, tempdata.tandem_packet.num_bytes, tempdata.tandem_packet.bytes);
      } break;
    default:
        PORTAL_PRINTF("AWSP2_ResponseJson_handleMessage: unknown channel 0x%x\n", channel);
        if (runaway++ > 10) {
            PORTAL_PRINTF("AWSP2_ResponseJson_handleMessage: too many bogus indications, exiting\n");
#ifndef __KERNEL__
            exit(-1);
#endif
        }
        return 0;
    }
    return 0;
}
#endif /* PORTAL_JSON */
