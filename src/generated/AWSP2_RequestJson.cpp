#include "GeneratedTypes.h"
#ifdef PORTAL_JSON
#include "jsoncpp/json/json.h"

int AWSP2_RequestJson_set_debug_verbosity ( struct PortalInternal *p, const uint8_t verbosity )
{
    Json::Value request;
    request.append(Json::Value("set_debug_verbosity"));
    request.append((Json::UInt64)verbosity);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_set_debug_verbosity);
    return 0;
};

int AWSP2_RequestJson_set_fabric_verbosity ( struct PortalInternal *p, const uint8_t verbosity )
{
    Json::Value request;
    request.append(Json::Value("set_fabric_verbosity"));
    request.append((Json::UInt64)verbosity);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_set_fabric_verbosity);
    return 0;
};

int AWSP2_RequestJson_dmi_read ( struct PortalInternal *p, const uint8_t req_addr )
{
    Json::Value request;
    request.append(Json::Value("dmi_read"));
    request.append((Json::UInt64)req_addr);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_dmi_read);
    return 0;
};

int AWSP2_RequestJson_dmi_write ( struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data )
{
    Json::Value request;
    request.append(Json::Value("dmi_write"));
    request.append((Json::UInt64)req_addr);
    request.append((Json::UInt64)req_data);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_dmi_write);
    return 0;
};

int AWSP2_RequestJson_dmi_status ( struct PortalInternal *p )
{
    Json::Value request;
    request.append(Json::Value("dmi_status"));
    

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_dmi_status);
    return 0;
};

int AWSP2_RequestJson_ddr_read ( struct PortalInternal *p, const uint32_t addr )
{
    Json::Value request;
    request.append(Json::Value("ddr_read"));
    request.append((Json::UInt64)addr);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_ddr_read);
    return 0;
};

int AWSP2_RequestJson_ddr_write ( struct PortalInternal *p, const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable )
{
    Json::Value request;
    request.append(Json::Value("ddr_write"));
    request.append((Json::UInt64)addr);
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
    request.append((Json::UInt64)byte_enable);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_ddr_write);
    return 0;
};

int AWSP2_RequestJson_register_region ( struct PortalInternal *p, const uint32_t region, const uint32_t objectId )
{
    Json::Value request;
    request.append(Json::Value("register_region"));
    request.append((Json::UInt64)region);
    request.append((Json::UInt64)objectId);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_register_region);
    return 0;
};

int AWSP2_RequestJson_memory_ready ( struct PortalInternal *p )
{
    Json::Value request;
    request.append(Json::Value("memory_ready"));
    

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_memory_ready);
    return 0;
};

int AWSP2_RequestJson_capture_tv_info ( struct PortalInternal *p, const int c )
{
    Json::Value request;
    request.append(Json::Value("capture_tv_info"));
    request.append((Json::UInt64)c);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_capture_tv_info);
    return 0;
};

int AWSP2_RequestJson_set_watch_tohost ( struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr )
{
    Json::Value request;
    request.append(Json::Value("set_watch_tohost"));
    request.append((Json::UInt64)watch_tohost);
    request.append((Json::UInt64)tohost_addr);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_set_watch_tohost);
    return 0;
};

int AWSP2_RequestJson_io_rdata ( struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last )
{
    Json::Value request;
    request.append(Json::Value("io_rdata"));
    request.append((Json::UInt64)data);
    request.append((Json::UInt64)rid);
    request.append((Json::UInt64)rresp);
    request.append((Json::UInt64)last);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_io_rdata);
    return 0;
};

int AWSP2_RequestJson_io_bdone ( struct PortalInternal *p, const uint16_t bid, const uint8_t bresp )
{
    Json::Value request;
    request.append(Json::Value("io_bdone"));
    request.append((Json::UInt64)bid);
    request.append((Json::UInt64)bresp);

    std::string requestjson = Json::FastWriter().write(request);;
    connectalJsonSend(p, requestjson.c_str(), (int)CHAN_NUM_AWSP2_Request_io_bdone);
    return 0;
};

AWSP2_RequestCb AWSP2_RequestJsonProxyReq = {
    portal_disconnect,
    AWSP2_RequestJson_set_debug_verbosity,
    AWSP2_RequestJson_set_fabric_verbosity,
    AWSP2_RequestJson_dmi_read,
    AWSP2_RequestJson_dmi_write,
    AWSP2_RequestJson_dmi_status,
    AWSP2_RequestJson_ddr_read,
    AWSP2_RequestJson_ddr_write,
    AWSP2_RequestJson_register_region,
    AWSP2_RequestJson_memory_ready,
    AWSP2_RequestJson_capture_tv_info,
    AWSP2_RequestJson_set_watch_tohost,
    AWSP2_RequestJson_io_rdata,
    AWSP2_RequestJson_io_bdone,
};
AWSP2_RequestCb *pAWSP2_RequestJsonProxyReq = &AWSP2_RequestJsonProxyReq;
const char * AWSP2_RequestJson_methodSignatures()
{
    return "{\"register_region\": [\"long\", \"long\"], \"ddr_write\": [\"long\", \"long\", \"long\"], \"ddr_read\": [\"long\"], \"io_bdone\": [\"long\", \"long\"], \"set_fabric_verbosity\": [\"long\"], \"memory_ready\": [], \"io_rdata\": [\"long\", \"long\", \"long\", \"long\"], \"dmi_read\": [\"long\"], \"capture_tv_info\": [\"long\"], \"set_watch_tohost\": [\"long\", \"long\"], \"dmi_write\": [\"long\", \"long\"], \"dmi_status\": [], \"set_debug_verbosity\": [\"long\"]}";
}

int AWSP2_RequestJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd)
{
    static int runaway = 0;
    int   tmp __attribute__ ((unused));
    int tmpfd __attribute__ ((unused));
    AWSP2_RequestData tempdata __attribute__ ((unused));
    memset(&tempdata, 0, sizeof(tempdata));
    Json::Value msg = Json::Value(connectalJsonReceive(p));
    switch (channel) {
    case CHAN_NUM_AWSP2_Request_set_debug_verbosity: {
        ((AWSP2_RequestCb *)p->cb)->set_debug_verbosity(p, tempdata.set_debug_verbosity.verbosity);
      } break;
    case CHAN_NUM_AWSP2_Request_set_fabric_verbosity: {
        ((AWSP2_RequestCb *)p->cb)->set_fabric_verbosity(p, tempdata.set_fabric_verbosity.verbosity);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_read: {
        ((AWSP2_RequestCb *)p->cb)->dmi_read(p, tempdata.dmi_read.req_addr);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_write: {
        ((AWSP2_RequestCb *)p->cb)->dmi_write(p, tempdata.dmi_write.req_addr, tempdata.dmi_write.req_data);
      } break;
    case CHAN_NUM_AWSP2_Request_dmi_status: {
        ((AWSP2_RequestCb *)p->cb)->dmi_status(p);
      } break;
    case CHAN_NUM_AWSP2_Request_ddr_read: {
        ((AWSP2_RequestCb *)p->cb)->ddr_read(p, tempdata.ddr_read.addr);
      } break;
    case CHAN_NUM_AWSP2_Request_ddr_write: {
        ((AWSP2_RequestCb *)p->cb)->ddr_write(p, tempdata.ddr_write.addr, tempdata.ddr_write.data, tempdata.ddr_write.byte_enable);
      } break;
    case CHAN_NUM_AWSP2_Request_register_region: {
        ((AWSP2_RequestCb *)p->cb)->register_region(p, tempdata.register_region.region, tempdata.register_region.objectId);
      } break;
    case CHAN_NUM_AWSP2_Request_memory_ready: {
        ((AWSP2_RequestCb *)p->cb)->memory_ready(p);
      } break;
    case CHAN_NUM_AWSP2_Request_capture_tv_info: {
        ((AWSP2_RequestCb *)p->cb)->capture_tv_info(p, tempdata.capture_tv_info.c);
      } break;
    case CHAN_NUM_AWSP2_Request_set_watch_tohost: {
        ((AWSP2_RequestCb *)p->cb)->set_watch_tohost(p, tempdata.set_watch_tohost.watch_tohost, tempdata.set_watch_tohost.tohost_addr);
      } break;
    case CHAN_NUM_AWSP2_Request_io_rdata: {
        ((AWSP2_RequestCb *)p->cb)->io_rdata(p, tempdata.io_rdata.data, tempdata.io_rdata.rid, tempdata.io_rdata.rresp, tempdata.io_rdata.last);
      } break;
    case CHAN_NUM_AWSP2_Request_io_bdone: {
        ((AWSP2_RequestCb *)p->cb)->io_bdone(p, tempdata.io_bdone.bid, tempdata.io_bdone.bresp);
      } break;
    default:
        PORTAL_PRINTF("AWSP2_RequestJson_handleMessage: unknown channel 0x%x\n", channel);
        if (runaway++ > 10) {
            PORTAL_PRINTF("AWSP2_RequestJson_handleMessage: too many bogus indications, exiting\n");
#ifndef __KERNEL__
            exit(-1);
#endif
        }
        return 0;
    }
    return 0;
}
#endif /* PORTAL_JSON */
