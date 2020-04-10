#include "GeneratedTypes.h"
#ifndef _AWSP2_RESPONSE_H_
#define _AWSP2_RESPONSE_H_
#include "portal.h"

class AWSP2_ResponseProxy : public Portal {
    AWSP2_ResponseCb *cb;
public:
    AWSP2_ResponseProxy(int id, int tile = DEFAULT_TILE, AWSP2_ResponseCb *cbarg = &AWSP2_ResponseProxyReq, int bufsize = AWSP2_Response_reqinfo, PortalPoller *poller = 0) :
        Portal(id, tile, bufsize, NULL, NULL, this, poller), cb(cbarg) {};
    AWSP2_ResponseProxy(int id, PortalTransportFunctions *transport, void *param, AWSP2_ResponseCb *cbarg = &AWSP2_ResponseProxyReq, int bufsize = AWSP2_Response_reqinfo, PortalPoller *poller = 0) :
        Portal(id, DEFAULT_TILE, bufsize, NULL, NULL, transport, param, this, poller), cb(cbarg) {};
    AWSP2_ResponseProxy(int id, PortalPoller *poller) :
        Portal(id, DEFAULT_TILE, AWSP2_Response_reqinfo, NULL, NULL, NULL, NULL, this, poller), cb(&AWSP2_ResponseProxyReq) {};
    int dmi_read_data ( const uint32_t rsp_data ) { return cb->dmi_read_data (&pint, rsp_data); };
    int dmi_status_data ( const uint16_t status ) { return cb->dmi_status_data (&pint, status); };
    int ddr_data ( const bsvvector_Luint8_t_L64 data ) { return cb->ddr_data (&pint, data); };
    int io_awaddr ( const uint32_t awaddr, const uint16_t awlen, const uint16_t awid ) { return cb->io_awaddr (&pint, awaddr, awlen, awid); };
    int io_araddr ( const uint32_t araddr, const uint16_t arlen, const uint16_t arid ) { return cb->io_araddr (&pint, araddr, arlen, arid); };
    int io_wdata ( const uint64_t wdata, const uint8_t wstrb ) { return cb->io_wdata (&pint, wdata, wstrb); };
    int tandem_packet ( const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes ) { return cb->tandem_packet (&pint, num_bytes, bytes); };
};

extern AWSP2_ResponseCb AWSP2_Response_cbTable;
class AWSP2_ResponseWrapper : public Portal {
public:
    AWSP2_ResponseWrapper(int id, int tile = DEFAULT_TILE, PORTAL_INDFUNC cba = AWSP2_Response_handleMessage, int bufsize = AWSP2_Response_reqinfo, PortalPoller *poller = 0) :
           Portal(id, tile, bufsize, cba, (void *)&AWSP2_Response_cbTable, this, poller) {
    };
    AWSP2_ResponseWrapper(int id, PortalTransportFunctions *transport, void *param, PORTAL_INDFUNC cba = AWSP2_Response_handleMessage, int bufsize = AWSP2_Response_reqinfo, PortalPoller *poller=0):
           Portal(id, DEFAULT_TILE, bufsize, cba, (void *)&AWSP2_Response_cbTable, transport, param, this, poller) {
    };
    AWSP2_ResponseWrapper(int id, PortalPoller *poller) :
           Portal(id, DEFAULT_TILE, AWSP2_Response_reqinfo, AWSP2_Response_handleMessage, (void *)&AWSP2_Response_cbTable, this, poller) {
    };
    AWSP2_ResponseWrapper(int id, PortalTransportFunctions *transport, void *param, PortalPoller *poller):
           Portal(id, DEFAULT_TILE, AWSP2_Response_reqinfo, AWSP2_Response_handleMessage, (void *)&AWSP2_Response_cbTable, transport, param, this, poller) {
    };
    virtual void disconnect(void) {
        printf("AWSP2_ResponseWrapper.disconnect called %d\n", pint.client_fd_number);
    };
    virtual void dmi_read_data ( const uint32_t rsp_data ) = 0;
    virtual void dmi_status_data ( const uint16_t status ) = 0;
    virtual void ddr_data ( const bsvvector_Luint8_t_L64 data ) = 0;
    virtual void io_awaddr ( const uint32_t awaddr, const uint16_t awlen, const uint16_t awid ) = 0;
    virtual void io_araddr ( const uint32_t araddr, const uint16_t arlen, const uint16_t arid ) = 0;
    virtual void io_wdata ( const uint64_t wdata, const uint8_t wstrb ) = 0;
    virtual void tandem_packet ( const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes ) = 0;
};
#endif // _AWSP2_RESPONSE_H_
