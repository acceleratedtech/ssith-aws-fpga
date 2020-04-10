#include "GeneratedTypes.h"
#ifndef _AWSP2_REQUEST_H_
#define _AWSP2_REQUEST_H_
#include "portal.h"

class AWSP2_RequestProxy : public Portal {
    AWSP2_RequestCb *cb;
public:
    AWSP2_RequestProxy(int id, int tile = DEFAULT_TILE, AWSP2_RequestCb *cbarg = &AWSP2_RequestProxyReq, int bufsize = AWSP2_Request_reqinfo, PortalPoller *poller = 0) :
        Portal(id, tile, bufsize, NULL, NULL, this, poller), cb(cbarg) {};
    AWSP2_RequestProxy(int id, PortalTransportFunctions *transport, void *param, AWSP2_RequestCb *cbarg = &AWSP2_RequestProxyReq, int bufsize = AWSP2_Request_reqinfo, PortalPoller *poller = 0) :
        Portal(id, DEFAULT_TILE, bufsize, NULL, NULL, transport, param, this, poller), cb(cbarg) {};
    AWSP2_RequestProxy(int id, PortalPoller *poller) :
        Portal(id, DEFAULT_TILE, AWSP2_Request_reqinfo, NULL, NULL, NULL, NULL, this, poller), cb(&AWSP2_RequestProxyReq) {};
    int set_debug_verbosity ( const uint8_t verbosity ) { return cb->set_debug_verbosity (&pint, verbosity); };
    int set_fabric_verbosity ( const uint8_t verbosity ) { return cb->set_fabric_verbosity (&pint, verbosity); };
    int dmi_read ( const uint8_t req_addr ) { return cb->dmi_read (&pint, req_addr); };
    int dmi_write ( const uint8_t req_addr, const uint32_t req_data ) { return cb->dmi_write (&pint, req_addr, req_data); };
    int dmi_status (  ) { return cb->dmi_status (&pint); };
    int ddr_read ( const uint32_t addr ) { return cb->ddr_read (&pint, addr); };
    int ddr_write ( const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable ) { return cb->ddr_write (&pint, addr, data, byte_enable); };
    int register_region ( const uint32_t region, const uint32_t objectId ) { return cb->register_region (&pint, region, objectId); };
    int memory_ready (  ) { return cb->memory_ready (&pint); };
    int capture_tv_info ( const int c ) { return cb->capture_tv_info (&pint, c); };
    int set_watch_tohost ( const int watch_tohost, const uint32_t tohost_addr ) { return cb->set_watch_tohost (&pint, watch_tohost, tohost_addr); };
    int io_rdata ( const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last ) { return cb->io_rdata (&pint, data, rid, rresp, last); };
    int io_bdone ( const uint16_t bid, const uint8_t bresp ) { return cb->io_bdone (&pint, bid, bresp); };
};

extern AWSP2_RequestCb AWSP2_Request_cbTable;
class AWSP2_RequestWrapper : public Portal {
public:
    AWSP2_RequestWrapper(int id, int tile = DEFAULT_TILE, PORTAL_INDFUNC cba = AWSP2_Request_handleMessage, int bufsize = AWSP2_Request_reqinfo, PortalPoller *poller = 0) :
           Portal(id, tile, bufsize, cba, (void *)&AWSP2_Request_cbTable, this, poller) {
    };
    AWSP2_RequestWrapper(int id, PortalTransportFunctions *transport, void *param, PORTAL_INDFUNC cba = AWSP2_Request_handleMessage, int bufsize = AWSP2_Request_reqinfo, PortalPoller *poller=0):
           Portal(id, DEFAULT_TILE, bufsize, cba, (void *)&AWSP2_Request_cbTable, transport, param, this, poller) {
    };
    AWSP2_RequestWrapper(int id, PortalPoller *poller) :
           Portal(id, DEFAULT_TILE, AWSP2_Request_reqinfo, AWSP2_Request_handleMessage, (void *)&AWSP2_Request_cbTable, this, poller) {
    };
    AWSP2_RequestWrapper(int id, PortalTransportFunctions *transport, void *param, PortalPoller *poller):
           Portal(id, DEFAULT_TILE, AWSP2_Request_reqinfo, AWSP2_Request_handleMessage, (void *)&AWSP2_Request_cbTable, transport, param, this, poller) {
    };
    virtual void disconnect(void) {
        printf("AWSP2_RequestWrapper.disconnect called %d\n", pint.client_fd_number);
    };
    virtual void set_debug_verbosity ( const uint8_t verbosity ) = 0;
    virtual void set_fabric_verbosity ( const uint8_t verbosity ) = 0;
    virtual void dmi_read ( const uint8_t req_addr ) = 0;
    virtual void dmi_write ( const uint8_t req_addr, const uint32_t req_data ) = 0;
    virtual void dmi_status (  ) = 0;
    virtual void ddr_read ( const uint32_t addr ) = 0;
    virtual void ddr_write ( const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable ) = 0;
    virtual void register_region ( const uint32_t region, const uint32_t objectId ) = 0;
    virtual void memory_ready (  ) = 0;
    virtual void capture_tv_info ( const int c ) = 0;
    virtual void set_watch_tohost ( const int watch_tohost, const uint32_t tohost_addr ) = 0;
    virtual void io_rdata ( const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last ) = 0;
    virtual void io_bdone ( const uint16_t bid, const uint8_t bresp ) = 0;
};
#endif // _AWSP2_REQUEST_H_
