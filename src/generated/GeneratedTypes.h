#ifndef __GENERATED_TYPES__
#define __GENERATED_TYPES__
#include "portal.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum ChannelType { ChannelType_Read, ChannelType_Write,  } ChannelType;
typedef struct DmaDbgRec {
    uint32_t x : 32;
    uint32_t y : 32;
    uint32_t z : 32;
    uint32_t w : 32;
} DmaDbgRec;
typedef enum DmaErrorType { DmaErrorNone, DmaErrorSGLIdOutOfRange_r, DmaErrorSGLIdOutOfRange_w, DmaErrorMMUOutOfRange_r, DmaErrorMMUOutOfRange_w, DmaErrorOffsetOutOfRange, DmaErrorSGLIdInvalid, DmaErrorTileTagOutOfRange,  } DmaErrorType;
typedef uint32_t SpecialTypeForSendingFd;
typedef enum TileState { Idle, Stopped, Running,  } TileState;
typedef struct TileControl {
    uint8_t tile : 2;
    TileState state;
} TileControl;
typedef enum XsimIfcNames { XsimIfcNames_XsimMsgRequest, XsimIfcNames_XsimMsgIndication,  } XsimIfcNames;
typedef enum IfcNames { IfcNamesNone=0, PlatformIfcNames_MemServerRequestS2H=1, PlatformIfcNames_MMURequestS2H=2, PlatformIfcNames_MemServerIndicationH2S=3, PlatformIfcNames_MMUIndicationH2S=4, IfcNames_AWSP2_ResponseH2S=5, IfcNames_AWSP2_RequestS2H=6,  } IfcNames;


int MemServerRequest_addrTrans ( struct PortalInternal *p, const uint32_t sglId, const uint32_t offset );
int MemServerRequest_setTileState ( struct PortalInternal *p, const TileControl tc );
int MemServerRequest_stateDbg ( struct PortalInternal *p, const ChannelType rc );
int MemServerRequest_memoryTraffic ( struct PortalInternal *p, const ChannelType rc );
enum { CHAN_NUM_MemServerRequest_addrTrans,CHAN_NUM_MemServerRequest_setTileState,CHAN_NUM_MemServerRequest_stateDbg,CHAN_NUM_MemServerRequest_memoryTraffic};
extern const uint32_t MemServerRequest_reqinfo;

typedef struct {
    uint32_t sglId;
    uint32_t offset;
} MemServerRequest_addrTransData;
typedef struct {
    TileControl tc;
} MemServerRequest_setTileStateData;
typedef struct {
    ChannelType rc;
} MemServerRequest_stateDbgData;
typedef struct {
    ChannelType rc;
} MemServerRequest_memoryTrafficData;
typedef union {
    MemServerRequest_addrTransData addrTrans;
    MemServerRequest_setTileStateData setTileState;
    MemServerRequest_stateDbgData stateDbg;
    MemServerRequest_memoryTrafficData memoryTraffic;
} MemServerRequestData;
int MemServerRequest_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*addrTrans) (  struct PortalInternal *p, const uint32_t sglId, const uint32_t offset );
    int (*setTileState) (  struct PortalInternal *p, const TileControl tc );
    int (*stateDbg) (  struct PortalInternal *p, const ChannelType rc );
    int (*memoryTraffic) (  struct PortalInternal *p, const ChannelType rc );
} MemServerRequestCb;
extern MemServerRequestCb MemServerRequestProxyReq;

int MemServerRequestJson_addrTrans ( struct PortalInternal *p, const uint32_t sglId, const uint32_t offset );
int MemServerRequestJson_setTileState ( struct PortalInternal *p, const TileControl tc );
int MemServerRequestJson_stateDbg ( struct PortalInternal *p, const ChannelType rc );
int MemServerRequestJson_memoryTraffic ( struct PortalInternal *p, const ChannelType rc );
int MemServerRequestJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern MemServerRequestCb MemServerRequestJsonProxyReq;

int MMURequest_sglist ( struct PortalInternal *p, const uint32_t sglId, const uint32_t sglIndex, const uint64_t addr, const uint32_t len );
int MMURequest_region ( struct PortalInternal *p, const uint32_t sglId, const uint64_t barr12, const uint32_t index12, const uint64_t barr8, const uint32_t index8, const uint64_t barr4, const uint32_t index4, const uint64_t barr0, const uint32_t index0 );
int MMURequest_idRequest ( struct PortalInternal *p, const SpecialTypeForSendingFd fd );
int MMURequest_idReturn ( struct PortalInternal *p, const uint32_t sglId );
int MMURequest_setInterface ( struct PortalInternal *p, const uint32_t interfaceId, const uint32_t sglId );
enum { CHAN_NUM_MMURequest_sglist,CHAN_NUM_MMURequest_region,CHAN_NUM_MMURequest_idRequest,CHAN_NUM_MMURequest_idReturn,CHAN_NUM_MMURequest_setInterface};
extern const uint32_t MMURequest_reqinfo;

typedef struct {
    uint32_t sglId;
    uint32_t sglIndex;
    uint64_t addr;
    uint32_t len;
} MMURequest_sglistData;
typedef struct {
    uint32_t sglId;
    uint64_t barr12;
    uint32_t index12;
    uint64_t barr8;
    uint32_t index8;
    uint64_t barr4;
    uint32_t index4;
    uint64_t barr0;
    uint32_t index0;
} MMURequest_regionData;
typedef struct {
    SpecialTypeForSendingFd fd;
} MMURequest_idRequestData;
typedef struct {
    uint32_t sglId;
} MMURequest_idReturnData;
typedef struct {
    uint32_t interfaceId;
    uint32_t sglId;
} MMURequest_setInterfaceData;
typedef union {
    MMURequest_sglistData sglist;
    MMURequest_regionData region;
    MMURequest_idRequestData idRequest;
    MMURequest_idReturnData idReturn;
    MMURequest_setInterfaceData setInterface;
} MMURequestData;
int MMURequest_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*sglist) (  struct PortalInternal *p, const uint32_t sglId, const uint32_t sglIndex, const uint64_t addr, const uint32_t len );
    int (*region) (  struct PortalInternal *p, const uint32_t sglId, const uint64_t barr12, const uint32_t index12, const uint64_t barr8, const uint32_t index8, const uint64_t barr4, const uint32_t index4, const uint64_t barr0, const uint32_t index0 );
    int (*idRequest) (  struct PortalInternal *p, const SpecialTypeForSendingFd fd );
    int (*idReturn) (  struct PortalInternal *p, const uint32_t sglId );
    int (*setInterface) (  struct PortalInternal *p, const uint32_t interfaceId, const uint32_t sglId );
} MMURequestCb;
extern MMURequestCb MMURequestProxyReq;

int MMURequestJson_sglist ( struct PortalInternal *p, const uint32_t sglId, const uint32_t sglIndex, const uint64_t addr, const uint32_t len );
int MMURequestJson_region ( struct PortalInternal *p, const uint32_t sglId, const uint64_t barr12, const uint32_t index12, const uint64_t barr8, const uint32_t index8, const uint64_t barr4, const uint32_t index4, const uint64_t barr0, const uint32_t index0 );
int MMURequestJson_idRequest ( struct PortalInternal *p, const SpecialTypeForSendingFd fd );
int MMURequestJson_idReturn ( struct PortalInternal *p, const uint32_t sglId );
int MMURequestJson_setInterface ( struct PortalInternal *p, const uint32_t interfaceId, const uint32_t sglId );
int MMURequestJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern MMURequestCb MMURequestJsonProxyReq;

int MemServerIndication_addrResponse ( struct PortalInternal *p, const uint64_t physAddr );
int MemServerIndication_reportStateDbg ( struct PortalInternal *p, const DmaDbgRec rec );
int MemServerIndication_reportMemoryTraffic ( struct PortalInternal *p, const uint64_t words );
int MemServerIndication_error ( struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
enum { CHAN_NUM_MemServerIndication_addrResponse,CHAN_NUM_MemServerIndication_reportStateDbg,CHAN_NUM_MemServerIndication_reportMemoryTraffic,CHAN_NUM_MemServerIndication_error};
extern const uint32_t MemServerIndication_reqinfo;

typedef struct {
    uint64_t physAddr;
} MemServerIndication_addrResponseData;
typedef struct {
    DmaDbgRec rec;
} MemServerIndication_reportStateDbgData;
typedef struct {
    uint64_t words;
} MemServerIndication_reportMemoryTrafficData;
typedef struct {
    uint32_t code;
    uint32_t sglId;
    uint64_t offset;
    uint64_t extra;
} MemServerIndication_errorData;
typedef union {
    MemServerIndication_addrResponseData addrResponse;
    MemServerIndication_reportStateDbgData reportStateDbg;
    MemServerIndication_reportMemoryTrafficData reportMemoryTraffic;
    MemServerIndication_errorData error;
} MemServerIndicationData;
int MemServerIndication_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*addrResponse) (  struct PortalInternal *p, const uint64_t physAddr );
    int (*reportStateDbg) (  struct PortalInternal *p, const DmaDbgRec rec );
    int (*reportMemoryTraffic) (  struct PortalInternal *p, const uint64_t words );
    int (*error) (  struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
} MemServerIndicationCb;
extern MemServerIndicationCb MemServerIndicationProxyReq;

int MemServerIndicationJson_addrResponse ( struct PortalInternal *p, const uint64_t physAddr );
int MemServerIndicationJson_reportStateDbg ( struct PortalInternal *p, const DmaDbgRec rec );
int MemServerIndicationJson_reportMemoryTraffic ( struct PortalInternal *p, const uint64_t words );
int MemServerIndicationJson_error ( struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
int MemServerIndicationJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern MemServerIndicationCb MemServerIndicationJsonProxyReq;

int MMUIndication_idResponse ( struct PortalInternal *p, const uint32_t sglId );
int MMUIndication_configResp ( struct PortalInternal *p, const uint32_t sglId );
int MMUIndication_error ( struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
enum { CHAN_NUM_MMUIndication_idResponse,CHAN_NUM_MMUIndication_configResp,CHAN_NUM_MMUIndication_error};
extern const uint32_t MMUIndication_reqinfo;

typedef struct {
    uint32_t sglId;
} MMUIndication_idResponseData;
typedef struct {
    uint32_t sglId;
} MMUIndication_configRespData;
typedef struct {
    uint32_t code;
    uint32_t sglId;
    uint64_t offset;
    uint64_t extra;
} MMUIndication_errorData;
typedef union {
    MMUIndication_idResponseData idResponse;
    MMUIndication_configRespData configResp;
    MMUIndication_errorData error;
} MMUIndicationData;
int MMUIndication_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*idResponse) (  struct PortalInternal *p, const uint32_t sglId );
    int (*configResp) (  struct PortalInternal *p, const uint32_t sglId );
    int (*error) (  struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
} MMUIndicationCb;
extern MMUIndicationCb MMUIndicationProxyReq;

int MMUIndicationJson_idResponse ( struct PortalInternal *p, const uint32_t sglId );
int MMUIndicationJson_configResp ( struct PortalInternal *p, const uint32_t sglId );
int MMUIndicationJson_error ( struct PortalInternal *p, const uint32_t code, const uint32_t sglId, const uint64_t offset, const uint64_t extra );
int MMUIndicationJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern MMUIndicationCb MMUIndicationJsonProxyReq;

int XsimMsgRequest_msgSink ( struct PortalInternal *p, const uint32_t portal, const uint32_t data );
int XsimMsgRequest_msgSinkFd ( struct PortalInternal *p, const uint32_t portal, const SpecialTypeForSendingFd data );
enum { CHAN_NUM_XsimMsgRequest_msgSink,CHAN_NUM_XsimMsgRequest_msgSinkFd};
extern const uint32_t XsimMsgRequest_reqinfo;

typedef struct {
    uint32_t portal;
    uint32_t data;
} XsimMsgRequest_msgSinkData;
typedef struct {
    uint32_t portal;
    SpecialTypeForSendingFd data;
} XsimMsgRequest_msgSinkFdData;
typedef union {
    XsimMsgRequest_msgSinkData msgSink;
    XsimMsgRequest_msgSinkFdData msgSinkFd;
} XsimMsgRequestData;
int XsimMsgRequest_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*msgSink) (  struct PortalInternal *p, const uint32_t portal, const uint32_t data );
    int (*msgSinkFd) (  struct PortalInternal *p, const uint32_t portal, const SpecialTypeForSendingFd data );
} XsimMsgRequestCb;
extern XsimMsgRequestCb XsimMsgRequestProxyReq;

int XsimMsgRequestJson_msgSink ( struct PortalInternal *p, const uint32_t portal, const uint32_t data );
int XsimMsgRequestJson_msgSinkFd ( struct PortalInternal *p, const uint32_t portal, const SpecialTypeForSendingFd data );
int XsimMsgRequestJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern XsimMsgRequestCb XsimMsgRequestJsonProxyReq;

int XsimMsgIndication_msgSource ( struct PortalInternal *p, const uint32_t portal, const uint32_t data );
enum { CHAN_NUM_XsimMsgIndication_msgSource};
extern const uint32_t XsimMsgIndication_reqinfo;

typedef struct {
    uint32_t portal;
    uint32_t data;
} XsimMsgIndication_msgSourceData;
typedef union {
    XsimMsgIndication_msgSourceData msgSource;
} XsimMsgIndicationData;
int XsimMsgIndication_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*msgSource) (  struct PortalInternal *p, const uint32_t portal, const uint32_t data );
} XsimMsgIndicationCb;
extern XsimMsgIndicationCb XsimMsgIndicationProxyReq;

int XsimMsgIndicationJson_msgSource ( struct PortalInternal *p, const uint32_t portal, const uint32_t data );
int XsimMsgIndicationJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern XsimMsgIndicationCb XsimMsgIndicationJsonProxyReq;

typedef uint8_t bsvvector_Luint8_t_L64[64];
typedef uint8_t bsvvector_Luint8_t_L64[64];
int AWSP2_Request_set_debug_verbosity ( struct PortalInternal *p, const uint8_t verbosity );
int AWSP2_Request_set_fabric_verbosity ( struct PortalInternal *p, const uint8_t verbosity );
int AWSP2_Request_dmi_read ( struct PortalInternal *p, const uint8_t req_addr );
int AWSP2_Request_dmi_write ( struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data );
int AWSP2_Request_dmi_status ( struct PortalInternal *p );
int AWSP2_Request_ddr_read ( struct PortalInternal *p, const uint32_t addr );
int AWSP2_Request_ddr_write ( struct PortalInternal *p, const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable );
int AWSP2_Request_register_region ( struct PortalInternal *p, const uint32_t region, const uint32_t objectId );
int AWSP2_Request_memory_ready ( struct PortalInternal *p );
int AWSP2_Request_capture_tv_info ( struct PortalInternal *p, const int c );
int AWSP2_Request_set_watch_tohost ( struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr );
int AWSP2_Request_io_rdata ( struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last );
int AWSP2_Request_io_bdone ( struct PortalInternal *p, const uint16_t bid, const uint8_t bresp );
int AWSP2_Request_irq_set_levels ( struct PortalInternal *p, const uint32_t w1s );
int AWSP2_Request_irq_clear_levels ( struct PortalInternal *p, const uint32_t w1c );
int AWSP2_Request_read_irq_status ( struct PortalInternal *p );
int AWSP2_Request_uart_fromhost ( struct PortalInternal *p, const uint8_t ch );
enum { CHAN_NUM_AWSP2_Request_set_debug_verbosity,CHAN_NUM_AWSP2_Request_set_fabric_verbosity,CHAN_NUM_AWSP2_Request_dmi_read,CHAN_NUM_AWSP2_Request_dmi_write,CHAN_NUM_AWSP2_Request_dmi_status,CHAN_NUM_AWSP2_Request_ddr_read,CHAN_NUM_AWSP2_Request_ddr_write,CHAN_NUM_AWSP2_Request_register_region,CHAN_NUM_AWSP2_Request_memory_ready,CHAN_NUM_AWSP2_Request_capture_tv_info,CHAN_NUM_AWSP2_Request_set_watch_tohost,CHAN_NUM_AWSP2_Request_io_rdata,CHAN_NUM_AWSP2_Request_io_bdone,CHAN_NUM_AWSP2_Request_irq_set_levels,CHAN_NUM_AWSP2_Request_irq_clear_levels,CHAN_NUM_AWSP2_Request_read_irq_status,CHAN_NUM_AWSP2_Request_uart_fromhost};
extern const uint32_t AWSP2_Request_reqinfo;

typedef struct {
    uint8_t verbosity;
} AWSP2_Request_set_debug_verbosityData;
typedef struct {
    uint8_t verbosity;
} AWSP2_Request_set_fabric_verbosityData;
typedef struct {
    uint8_t req_addr;
} AWSP2_Request_dmi_readData;
typedef struct {
    uint8_t req_addr;
    uint32_t req_data;
} AWSP2_Request_dmi_writeData;
typedef struct {
        int padding;

} AWSP2_Request_dmi_statusData;
typedef struct {
    uint32_t addr;
} AWSP2_Request_ddr_readData;
typedef struct {
    uint32_t addr;
    bsvvector_Luint8_t_L64 data;
    uint64_t byte_enable;
} AWSP2_Request_ddr_writeData;
typedef struct {
    uint32_t region;
    uint32_t objectId;
} AWSP2_Request_register_regionData;
typedef struct {
        int padding;

} AWSP2_Request_memory_readyData;
typedef struct {
    int c;
} AWSP2_Request_capture_tv_infoData;
typedef struct {
    int watch_tohost;
    uint32_t tohost_addr;
} AWSP2_Request_set_watch_tohostData;
typedef struct {
    uint64_t data;
    uint16_t rid;
    uint8_t rresp;
    int last;
} AWSP2_Request_io_rdataData;
typedef struct {
    uint16_t bid;
    uint8_t bresp;
} AWSP2_Request_io_bdoneData;
typedef struct {
    uint32_t w1s;
} AWSP2_Request_irq_set_levelsData;
typedef struct {
    uint32_t w1c;
} AWSP2_Request_irq_clear_levelsData;
typedef struct {
        int padding;

} AWSP2_Request_read_irq_statusData;
typedef struct {
    uint8_t ch;
} AWSP2_Request_uart_fromhostData;
typedef union {
    AWSP2_Request_set_debug_verbosityData set_debug_verbosity;
    AWSP2_Request_set_fabric_verbosityData set_fabric_verbosity;
    AWSP2_Request_dmi_readData dmi_read;
    AWSP2_Request_dmi_writeData dmi_write;
    AWSP2_Request_dmi_statusData dmi_status;
    AWSP2_Request_ddr_readData ddr_read;
    AWSP2_Request_ddr_writeData ddr_write;
    AWSP2_Request_register_regionData register_region;
    AWSP2_Request_memory_readyData memory_ready;
    AWSP2_Request_capture_tv_infoData capture_tv_info;
    AWSP2_Request_set_watch_tohostData set_watch_tohost;
    AWSP2_Request_io_rdataData io_rdata;
    AWSP2_Request_io_bdoneData io_bdone;
    AWSP2_Request_irq_set_levelsData irq_set_levels;
    AWSP2_Request_irq_clear_levelsData irq_clear_levels;
    AWSP2_Request_read_irq_statusData read_irq_status;
    AWSP2_Request_uart_fromhostData uart_fromhost;
} AWSP2_RequestData;
int AWSP2_Request_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*set_debug_verbosity) (  struct PortalInternal *p, const uint8_t verbosity );
    int (*set_fabric_verbosity) (  struct PortalInternal *p, const uint8_t verbosity );
    int (*dmi_read) (  struct PortalInternal *p, const uint8_t req_addr );
    int (*dmi_write) (  struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data );
    int (*dmi_status) (  struct PortalInternal *p );
    int (*ddr_read) (  struct PortalInternal *p, const uint32_t addr );
    int (*ddr_write) (  struct PortalInternal *p, const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable );
    int (*register_region) (  struct PortalInternal *p, const uint32_t region, const uint32_t objectId );
    int (*memory_ready) (  struct PortalInternal *p );
    int (*capture_tv_info) (  struct PortalInternal *p, const int c );
    int (*set_watch_tohost) (  struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr );
    int (*io_rdata) (  struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last );
    int (*io_bdone) (  struct PortalInternal *p, const uint16_t bid, const uint8_t bresp );
    int (*irq_set_levels) (  struct PortalInternal *p, const uint32_t w1s );
    int (*irq_clear_levels) (  struct PortalInternal *p, const uint32_t w1c );
    int (*read_irq_status) (  struct PortalInternal *p );
    int (*uart_fromhost) (  struct PortalInternal *p, const uint8_t ch );
} AWSP2_RequestCb;
extern AWSP2_RequestCb AWSP2_RequestProxyReq;

int AWSP2_RequestJson_set_debug_verbosity ( struct PortalInternal *p, const uint8_t verbosity );
int AWSP2_RequestJson_set_fabric_verbosity ( struct PortalInternal *p, const uint8_t verbosity );
int AWSP2_RequestJson_dmi_read ( struct PortalInternal *p, const uint8_t req_addr );
int AWSP2_RequestJson_dmi_write ( struct PortalInternal *p, const uint8_t req_addr, const uint32_t req_data );
int AWSP2_RequestJson_dmi_status ( struct PortalInternal *p );
int AWSP2_RequestJson_ddr_read ( struct PortalInternal *p, const uint32_t addr );
int AWSP2_RequestJson_ddr_write ( struct PortalInternal *p, const uint32_t addr, const bsvvector_Luint8_t_L64 data, const uint64_t byte_enable );
int AWSP2_RequestJson_register_region ( struct PortalInternal *p, const uint32_t region, const uint32_t objectId );
int AWSP2_RequestJson_memory_ready ( struct PortalInternal *p );
int AWSP2_RequestJson_capture_tv_info ( struct PortalInternal *p, const int c );
int AWSP2_RequestJson_set_watch_tohost ( struct PortalInternal *p, const int watch_tohost, const uint32_t tohost_addr );
int AWSP2_RequestJson_io_rdata ( struct PortalInternal *p, const uint64_t data, const uint16_t rid, const uint8_t rresp, const int last );
int AWSP2_RequestJson_io_bdone ( struct PortalInternal *p, const uint16_t bid, const uint8_t bresp );
int AWSP2_RequestJson_irq_set_levels ( struct PortalInternal *p, const uint32_t w1s );
int AWSP2_RequestJson_irq_clear_levels ( struct PortalInternal *p, const uint32_t w1c );
int AWSP2_RequestJson_read_irq_status ( struct PortalInternal *p );
int AWSP2_RequestJson_uart_fromhost ( struct PortalInternal *p, const uint8_t ch );
int AWSP2_RequestJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern AWSP2_RequestCb AWSP2_RequestJsonProxyReq;

typedef uint8_t bsvvector_Luint8_t_L64[64];
typedef uint8_t bsvvector_Luint8_t_L72[72];
typedef uint8_t bsvvector_Luint8_t_L64[64];
typedef uint8_t bsvvector_Luint8_t_L72[72];
int AWSP2_Response_dmi_read_data ( struct PortalInternal *p, const uint32_t rsp_data );
int AWSP2_Response_dmi_status_data ( struct PortalInternal *p, const uint16_t status );
int AWSP2_Response_ddr_data ( struct PortalInternal *p, const bsvvector_Luint8_t_L64 data );
int AWSP2_Response_io_awaddr ( struct PortalInternal *p, const uint64_t awaddr, const uint16_t awlen, const uint16_t awid );
int AWSP2_Response_io_araddr ( struct PortalInternal *p, const uint64_t araddr, const uint16_t arlen, const uint16_t arid );
int AWSP2_Response_io_wdata ( struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb );
int AWSP2_Response_irq_status ( struct PortalInternal *p, const uint32_t levels );
int AWSP2_Response_tandem_packet ( struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes );
int AWSP2_Response_uart_tohost ( struct PortalInternal *p, const uint8_t ch );
enum { CHAN_NUM_AWSP2_Response_dmi_read_data,CHAN_NUM_AWSP2_Response_dmi_status_data,CHAN_NUM_AWSP2_Response_ddr_data,CHAN_NUM_AWSP2_Response_io_awaddr,CHAN_NUM_AWSP2_Response_io_araddr,CHAN_NUM_AWSP2_Response_io_wdata,CHAN_NUM_AWSP2_Response_irq_status,CHAN_NUM_AWSP2_Response_tandem_packet,CHAN_NUM_AWSP2_Response_uart_tohost};
extern const uint32_t AWSP2_Response_reqinfo;

typedef struct {
    uint32_t rsp_data;
} AWSP2_Response_dmi_read_dataData;
typedef struct {
    uint16_t status;
} AWSP2_Response_dmi_status_dataData;
typedef struct {
    bsvvector_Luint8_t_L64 data;
} AWSP2_Response_ddr_dataData;
typedef struct {
    uint64_t awaddr;
    uint16_t awlen;
    uint16_t awid;
} AWSP2_Response_io_awaddrData;
typedef struct {
    uint64_t araddr;
    uint16_t arlen;
    uint16_t arid;
} AWSP2_Response_io_araddrData;
typedef struct {
    uint64_t wdata;
    uint8_t wstrb;
} AWSP2_Response_io_wdataData;
typedef struct {
    uint32_t levels;
} AWSP2_Response_irq_statusData;
typedef struct {
    uint32_t num_bytes;
    bsvvector_Luint8_t_L72 bytes;
} AWSP2_Response_tandem_packetData;
typedef struct {
    uint8_t ch;
} AWSP2_Response_uart_tohostData;
typedef union {
    AWSP2_Response_dmi_read_dataData dmi_read_data;
    AWSP2_Response_dmi_status_dataData dmi_status_data;
    AWSP2_Response_ddr_dataData ddr_data;
    AWSP2_Response_io_awaddrData io_awaddr;
    AWSP2_Response_io_araddrData io_araddr;
    AWSP2_Response_io_wdataData io_wdata;
    AWSP2_Response_irq_statusData irq_status;
    AWSP2_Response_tandem_packetData tandem_packet;
    AWSP2_Response_uart_tohostData uart_tohost;
} AWSP2_ResponseData;
int AWSP2_Response_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
typedef struct {
    PORTAL_DISCONNECT disconnect;
    int (*dmi_read_data) (  struct PortalInternal *p, const uint32_t rsp_data );
    int (*dmi_status_data) (  struct PortalInternal *p, const uint16_t status );
    int (*ddr_data) (  struct PortalInternal *p, const bsvvector_Luint8_t_L64 data );
    int (*io_awaddr) (  struct PortalInternal *p, const uint64_t awaddr, const uint16_t awlen, const uint16_t awid );
    int (*io_araddr) (  struct PortalInternal *p, const uint64_t araddr, const uint16_t arlen, const uint16_t arid );
    int (*io_wdata) (  struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb );
    int (*irq_status) (  struct PortalInternal *p, const uint32_t levels );
    int (*tandem_packet) (  struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes );
    int (*uart_tohost) (  struct PortalInternal *p, const uint8_t ch );
} AWSP2_ResponseCb;
extern AWSP2_ResponseCb AWSP2_ResponseProxyReq;

int AWSP2_ResponseJson_dmi_read_data ( struct PortalInternal *p, const uint32_t rsp_data );
int AWSP2_ResponseJson_dmi_status_data ( struct PortalInternal *p, const uint16_t status );
int AWSP2_ResponseJson_ddr_data ( struct PortalInternal *p, const bsvvector_Luint8_t_L64 data );
int AWSP2_ResponseJson_io_awaddr ( struct PortalInternal *p, const uint64_t awaddr, const uint16_t awlen, const uint16_t awid );
int AWSP2_ResponseJson_io_araddr ( struct PortalInternal *p, const uint64_t araddr, const uint16_t arlen, const uint16_t arid );
int AWSP2_ResponseJson_io_wdata ( struct PortalInternal *p, const uint64_t wdata, const uint8_t wstrb );
int AWSP2_ResponseJson_irq_status ( struct PortalInternal *p, const uint32_t levels );
int AWSP2_ResponseJson_tandem_packet ( struct PortalInternal *p, const uint32_t num_bytes, const bsvvector_Luint8_t_L72 bytes );
int AWSP2_ResponseJson_uart_tohost ( struct PortalInternal *p, const uint8_t ch );
int AWSP2_ResponseJson_handleMessage(struct PortalInternal *p, unsigned int channel, int messageFd);
extern AWSP2_ResponseCb AWSP2_ResponseJsonProxyReq;
#ifdef __cplusplus
}
#endif
#endif //__GENERATED_TYPES__
