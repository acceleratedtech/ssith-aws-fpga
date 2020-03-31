#include "GeneratedTypes.h"

#ifndef NO_CPP_PORTAL_CODE
extern const uint32_t ifcNamesNone = IfcNamesNone;
extern const uint32_t platformIfcNames_MemServerRequestS2H = PlatformIfcNames_MemServerRequestS2H;
extern const uint32_t platformIfcNames_MMURequestS2H = PlatformIfcNames_MMURequestS2H;
extern const uint32_t platformIfcNames_MemServerIndicationH2S = PlatformIfcNames_MemServerIndicationH2S;
extern const uint32_t platformIfcNames_MMUIndicationH2S = PlatformIfcNames_MMUIndicationH2S;
extern const uint32_t ifcNames_AWSP2_ResponseH2S = IfcNames_AWSP2_ResponseH2S;
extern const uint32_t ifcNames_AWSP2_RequestS2H = IfcNames_AWSP2_RequestS2H;

/************** Start of MemServerRequestWrapper CPP ***********/
#include "MemServerRequest.h"
int MemServerRequestdisconnect_cb (struct PortalInternal *p) {
    (static_cast<MemServerRequestWrapper *>(p->parent))->disconnect();
    return 0;
};
int MemServerRequestaddrTrans_cb (  struct PortalInternal *p, const uint32_t sglId, const uint32_t offset ) {
    (static_cast<MemServerRequestWrapper *>(p->parent))->addrTrans ( sglId, offset);
    return 0;
};
int MemServerRequestsetTileState_cb (  struct PortalInternal *p, const TileControl tc ) {
    (static_cast<MemServerRequestWrapper *>(p->parent))->setTileState ( tc);
    return 0;
};
int MemServerRequeststateDbg_cb (  struct PortalInternal *p, const ChannelType rc ) {
    (static_cast<MemServerRequestWrapper *>(p->parent))->stateDbg ( rc);
    return 0;
};
int MemServerRequestmemoryTraffic_cb (  struct PortalInternal *p, const ChannelType rc ) {
    (static_cast<MemServerRequestWrapper *>(p->parent))->memoryTraffic ( rc);
    return 0;
};
MemServerRequestCb MemServerRequest_cbTable = {
    MemServerRequestdisconnect_cb,
    MemServerRequestaddrTrans_cb,
    MemServerRequestsetTileState_cb,
    MemServerRequeststateDbg_cb,
    MemServerRequestmemoryTraffic_cb,
};

/************** Start of MMURequestWrapper CPP ***********/
#include "MMURequest.h"
int MMURequestdisconnect_cb (struct PortalInternal *p) {
    (static_cast<MMURequestWrapper *>(p->parent))->disconnect();
    return 0;
};
int MMURequestsglist_cb (  struct PortalInternal *p, const uint32_t sglId, const uint32_t sglIndex, const uint64_t addr, const uint32_t len ) {
    (static_cast<MMURequestWrapper *>(p->parent))->sglist ( sglId, sglIndex, addr, len);
    return 0;
};
int MMURequestregion_cb (  struct PortalInternal *p, const uint32_t sglId, const uint64_t barr12, const uint32_t index12, const uint64_t barr8, const uint32_t index8, const uint64_t barr4, const uint32_t index4, const uint64_t barr0, const uint32_t index0 ) {
    (static_cast<MMURequestWrapper *>(p->parent))->region ( sglId, barr12, index12, barr8, index8, barr4, index4, barr0, index0);
    return 0;
};
int MMURequestidRequest_cb (  struct PortalInternal *p, const SpecialTypeForSendingFd fd ) {
    (static_cast<MMURequestWrapper *>(p->parent))->idRequest ( fd);
    return 0;
};
int MMURequestidReturn_cb (  struct PortalInternal *p, const uint32_t sglId ) {
    (static_cast<MMURequestWrapper *>(p->parent))->idReturn ( sglId);
    return 0;
};
int MMURequestsetInterface_cb (  struct PortalInternal *p, const uint32_t interfaceId, const uint32_t sglId ) {
    (static_cast<MMURequestWrapper *>(p->parent))->setInterface ( interfaceId, sglId);
    return 0;
};
MMURequestCb MMURequest_cbTable = {
    MMURequestdisconnect_cb,
    MMURequestsglist_cb,
    MMURequestregion_cb,
    MMURequestidRequest_cb,
    MMURequestidReturn_cb,
    MMURequestsetInterface_cb,
};

/************** Start of MemServerIndicationWrapper CPP ***********/
#include "MemServerIndication.h"
int MemServerIndicationdisconnect_cb (struct PortalInternal *p) {
    (static_cast<MemServerIndicationWrapper *>(p->parent))->disconnect();
    return 0;
};
int MemServerIndicationaddrResponse_cb (  struct PortalInternal *p, const uint64_t physAddr ) {
    (static_cast<MemServerIndicationWrapper *>(p->parent))->addrResponse ( physAddr);
    return 0;
};
int MemServerIndicationreportStateDbg_cb (  struct PortalInternal *p, const DmaDbgRec rec ) {
    (static_cast<MemServerIndicationWrapper *>(p->parent))->reportStateDbg ( rec);
    return 0;
};
int MemServerIndicationreportMemoryTraffic_cb (  struct PortalInternal *p, const uint64_t words ) {
    (static_cast<MemServerIndicationWrapper *>(p->parent))->reportMemoryTraffic ( words);
    return 0;
};
int MemServerIndicationerror_cb (  struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra ) {
    (static_cast<MemServerIndicationWrapper *>(p->parent))->error ( code, sglId, offset, extra);
    return 0;
};
MemServerIndicationCb MemServerIndication_cbTable = {
    MemServerIndicationdisconnect_cb,
    MemServerIndicationaddrResponse_cb,
    MemServerIndicationreportStateDbg_cb,
    MemServerIndicationreportMemoryTraffic_cb,
    MemServerIndicationerror_cb,
};

/************** Start of MMUIndicationWrapper CPP ***********/
#include "MMUIndication.h"
int MMUIndicationdisconnect_cb (struct PortalInternal *p) {
    (static_cast<MMUIndicationWrapper *>(p->parent))->disconnect();
    return 0;
};
int MMUIndicationidResponse_cb (  struct PortalInternal *p, const uint32_t sglId ) {
    (static_cast<MMUIndicationWrapper *>(p->parent))->idResponse ( sglId);
    return 0;
};
int MMUIndicationconfigResp_cb (  struct PortalInternal *p, const uint32_t sglId ) {
    (static_cast<MMUIndicationWrapper *>(p->parent))->configResp ( sglId);
    return 0;
};
int MMUIndicationerror_cb (  struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra ) {
    (static_cast<MMUIndicationWrapper *>(p->parent))->error ( code, sglId, offset, extra);
    return 0;
};
MMUIndicationCb MMUIndication_cbTable = {
    MMUIndicationdisconnect_cb,
    MMUIndicationidResponse_cb,
    MMUIndicationconfigResp_cb,
    MMUIndicationerror_cb,
};

/************** Start of XsimMsgRequestWrapper CPP ***********/
#include "XsimMsgRequest.h"
int XsimMsgRequestdisconnect_cb (struct PortalInternal *p) {
    (static_cast<XsimMsgRequestWrapper *>(p->parent))->disconnect();
    return 0;
};
int XsimMsgRequestmsgSink_cb (  struct PortalInternal *p, const uint32_t portal, const uint32_t data ) {
    (static_cast<XsimMsgRequestWrapper *>(p->parent))->msgSink ( portal, data);
    return 0;
};
int XsimMsgRequestmsgSinkFd_cb (  struct PortalInternal *p, const uint32_t portal, const SpecialTypeForSendingFd data ) {
    (static_cast<XsimMsgRequestWrapper *>(p->parent))->msgSinkFd ( portal, data);
    return 0;
};
XsimMsgRequestCb XsimMsgRequest_cbTable = {
    XsimMsgRequestdisconnect_cb,
    XsimMsgRequestmsgSink_cb,
    XsimMsgRequestmsgSinkFd_cb,
};

/************** Start of XsimMsgIndicationWrapper CPP ***********/
#include "XsimMsgIndication.h"
int XsimMsgIndicationdisconnect_cb (struct PortalInternal *p) {
    (static_cast<XsimMsgIndicationWrapper *>(p->parent))->disconnect();
    return 0;
};
int XsimMsgIndicationmsgSource_cb (  struct PortalInternal *p, const uint32_t portal, const uint32_t data ) {
    (static_cast<XsimMsgIndicationWrapper *>(p->parent))->msgSource ( portal, data);
    return 0;
};
XsimMsgIndicationCb XsimMsgIndication_cbTable = {
    XsimMsgIndicationdisconnect_cb,
    XsimMsgIndicationmsgSource_cb,
};

/************** Start of AWSP2_RequestWrapper CPP ***********/
#include "AWSP2_Request.h"
int AWSP2_Requestdisconnect_cb (struct PortalInternal *p) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->disconnect();
    return 0;
};
int AWSP2_Requestset_debug_verbosity_cb (  struct PortalInternal *p, const uint8_t verbosity ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->set_debug_verbosity ( verbosity);
    return 0;
};
int AWSP2_Requestset_fabric_verbosity_cb (  struct PortalInternal *p, const uint8_t verbosity ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->set_fabric_verbosity ( verbosity);
    return 0;
};
int AWSP2_Requestdmi_read_cb (  struct PortalInternal *p, const uint8_t req_addr ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->dmi_read ( req_addr);
    return 0;
};
int AWSP2_Requestdmi_write_cb (  struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->dmi_write ( req_addr, req_data);
    return 0;
};
int AWSP2_Requestdmi_status_cb (  struct PortalInternal *p ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->dmi_status ( );
    return 0;
};
int AWSP2_Requestregister_region_cb (  struct PortalInternal *p, const uint32_t region, const uint32_t objectId ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->register_region ( region, objectId);
    return 0;
};
int AWSP2_Requestmemory_ready_cb (  struct PortalInternal *p ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->memory_ready ( );
    return 0;
};
int AWSP2_Requestcapture_tv_info_cb (  struct PortalInternal *p, const int c ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->capture_tv_info ( c);
    return 0;
};
int AWSP2_Requestset_watch_tohost_cb (  struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->set_watch_tohost ( watch_tohost, tohost_addr);
    return 0;
};
int AWSP2_Requestio_rdata_cb (  struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->io_rdata ( data, rid, rresp, last);
    return 0;
};
int AWSP2_Requestio_bdone_cb (  struct PortalInternal *p, const uint16_t bid, const uint8_t bresp ) {
    (static_cast<AWSP2_RequestWrapper *>(p->parent))->io_bdone ( bid, bresp);
    return 0;
};
AWSP2_RequestCb AWSP2_Request_cbTable = {
    AWSP2_Requestdisconnect_cb,
    AWSP2_Requestset_debug_verbosity_cb,
    AWSP2_Requestset_fabric_verbosity_cb,
    AWSP2_Requestdmi_read_cb,
    AWSP2_Requestdmi_write_cb,
    AWSP2_Requestdmi_status_cb,
    AWSP2_Requestregister_region_cb,
    AWSP2_Requestmemory_ready_cb,
    AWSP2_Requestcapture_tv_info_cb,
    AWSP2_Requestset_watch_tohost_cb,
    AWSP2_Requestio_rdata_cb,
    AWSP2_Requestio_bdone_cb,
};

/************** Start of AWSP2_ResponseWrapper CPP ***********/
#include "AWSP2_Response.h"
int AWSP2_Responsedisconnect_cb (struct PortalInternal *p) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->disconnect();
    return 0;
};
int AWSP2_Responsedmi_read_data_cb (  struct PortalInternal *p, const uint32_t rsp_data ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->dmi_read_data ( rsp_data);
    return 0;
};
int AWSP2_Responsedmi_status_data_cb (  struct PortalInternal *p, const uint16_t status ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->dmi_status_data ( status);
    return 0;
};
int AWSP2_Responseio_awaddr_cb (  struct PortalInternal *p, const uint32_t awaddr, const uint16_t awlen, const uint16_t awid ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->io_awaddr ( awaddr, awlen, awid);
    return 0;
};
int AWSP2_Responseio_araddr_cb (  struct PortalInternal *p, const uint32_t araddr, const uint16_t arlen, const uint16_t arid ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->io_araddr ( araddr, arlen, arid);
    return 0;
};
int AWSP2_Responseio_wdata_cb (  struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->io_wdata ( wdata, wstrb);
    return 0;
};
int AWSP2_Responsetandem_packet_cb (  struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes ) {
    (static_cast<AWSP2_ResponseWrapper *>(p->parent))->tandem_packet ( num_bytes, bytes);
    return 0;
};
AWSP2_ResponseCb AWSP2_Response_cbTable = {
    AWSP2_Responsedisconnect_cb,
    AWSP2_Responsedmi_read_data_cb,
    AWSP2_Responsedmi_status_data_cb,
    AWSP2_Responseio_awaddr_cb,
    AWSP2_Responseio_araddr_cb,
    AWSP2_Responseio_wdata_cb,
    AWSP2_Responsetandem_packet_cb,
};
#endif //NO_CPP_PORTAL_CODE
