#ifndef _OI_HID_ENDPOINT_H
#define _OI_HID_ENDPOINT_H

/**
  @file
  @internal
*/

/** \addtogroup HID_Internal */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

#include "oi_common.h"
#include "oi_l2cap.h"
#include "oi_l2cap_qos.h"
#include "oi_mbuf.h"
#include "oi_hid.h"
#include "oi_dispatch.h"
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */


#define HID_TRANSACTION_HEADER_LEN      sizeof(OI_BYTE)

#define HID_TRANSACTION_REPORT_ID_LEN   sizeof(OI_UINT8)

#define HID_TRANSACTION_BUFFER_SIZE_LEN sizeof(OI_UINT16)


#define HID_REQUEST_MAX_LEN (HID_TRANSACTION_HEADER_LEN + HID_TRANSACTION_REPORT_ID_LEN + HID_TRANSACTION_BUFFER_SIZE_LEN)

#define HID_TRANSACTION_HEADER(type, param) ((((type) & 0xf) << 4) | ((param) & 0xf))

#define HID_TRANSACTION_HEADER_GET_TYPE(header) (((header) & 0xf0) >> 4)

#define HID_TRANSACTION_HEADER_GET_PARAM(header) ((header) & 0xf)

#define MAX_HID_CONNECTIONS 2


typedef OI_UINT16 HID_CONNECTION_HANDLE;

typedef struct _HID_CHANNEL HID_CHANNEL;

typedef struct _HID_CONNECTION HID_CONNECTION;

typedef void (*HID_ENDPOINT_CONNECT_IND)(OI_BD_ADDR const *addr,
                                         HID_CONNECTION_HANDLE handle);

typedef void (*HID_ENDPOINT_CONNECT_CFM)(HID_CONNECTION_HANDLE handle,
                                         OI_STATUS status);

typedef void (*HID_ENDPOINT_DISCONNECT_IND)(HID_CONNECTION_HANDLE handle,
                                            OI_STATUS status);

typedef void (*HID_ENDPOINT_SEND_CFM)(HID_CHANNEL *chan,
                                      OI_STATUS status);


typedef void (*HID_RECV_IND)(HID_CONNECTION_HANDLE connHandle,
                             OI_BYTE transType,
                             OI_BYTE transParam,
                             OI_BYTE *data,
                             OI_UINT16 len,
                             OI_BOOL final);


typedef void (*HID_TRANSACTION_TIMEOUT_IND)(HID_CONNECTION *conn);

typedef struct {
    HID_ENDPOINT_CONNECT_IND connectInd;                /**< connectInd is required */
    HID_ENDPOINT_CONNECT_CFM connectCfm;                /**< connectCfm is required */
    HID_ENDPOINT_DISCONNECT_IND disconnectInd;          /**< disconnectInd is required */
    HID_RECV_IND controlRecvInd;                        /**< controlRecvInd is required */
    HID_RECV_IND interruptRecvInd;                      /**< interruptRecvInd is required */
    HID_TRANSACTION_TIMEOUT_IND transactionTimeoutInd;  /**< transactionTimeoutInd is optional */
} HID_ENDPOINT_CALLBACKS;


typedef enum {
    HID_IDLE,
    HID_CONNECTING,
    HID_CONNECTED,
    HID_ERROR,
    HID_DISCONNECTING,
    HID_CONNECT_INCOMPLETE
} HID_CONNECTION_STATE;

struct _HID_CHANNEL {
    OI_L2CAP_CID cid;
#ifndef OI_CODE
    OI_L2CAP_CONNECT_PARAMS *l2capParams;
#else
#error code not present
#endif
    OI_L2CAP_FLOWSPEC const *l2capQos;
    HID_CONNECTION_STATE state;
    OI_UINT16 inMTU;
    OI_UINT16 outMTU;
    OI_BYTE const *clientDataPtr;
    OI_UINT16 clientDataLen;
    /* Segmentation state information */
    HID_ENDPOINT_SEND_CFM sendCfm;
    OI_BOOL lastFragment;
    OI_BYTE transType ;
    OI_BYTE transParam ;
    OI_BYTE header;
    OI_MBUF *buf;
    /* Reassembly state information */
    OI_BOOL expectingMore;
};

struct _HID_CONNECTION {
    OI_BD_ADDR addr;        /* Set only by OI_HIDEndpoint_NewConnection */
    OI_BOOL initiator;      /* Set only by OI_HIDEndpoint_NewConnection */
    OI_UINT8 protocol;
    OI_UINT8 pendingProtocol;
    HID_CHANNEL control;
    HID_CHANNEL hid_interrupt;
    HID_CONNECTION_HANDLE handle;
    HID_CONNECTION_STATE state;
    DISPATCH_CB_HANDLE timeoutCB;
};

#define GET_HANDLE(conn) ((conn)->handle)

typedef struct {
    HID_CONNECTION                 *HID_Connections;
    HID_ENDPOINT_CALLBACKS const   *HID_EndpointCallbacks;
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */
    OI_CONNECT_POLICY               connectPolicy;
    OI_UINT32                       sdp_record;
    OI_UINT8                        HID_MaxConnections;
    OI_UINT8                        connectionCount;
} HID_THIS;


OI_STATUS OI_HIDEndpoint_Register(HID_ENDPOINT_CALLBACKS const *cb,
                                  OI_UINT maxConnections,
                                  HID_THIS **hidThis);

OI_STATUS OI_HIDEndpoint_Deregister(void);

OI_STATUS OI_HIDEndpoint_Connect(OI_BD_ADDR const *addr,
                                 OI_HID_CONNECTION_PARAMS const *params,
                                 HID_CONNECTION_HANDLE *handle);

OI_STATUS OI_HIDEndpoint_Accept(HID_CONNECTION_HANDLE handle,
                                OI_HID_CONNECTION_PARAMS const *params,
                                OI_BOOL allow);

OI_STATUS OI_HIDEndpoint_Disconnect(HID_CONNECTION_HANDLE handle);

OI_STATUS OI_HIDEndpoint_SendData(HID_ENDPOINT_SEND_CFM sendCfm,
                                  HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_BYTE const *data,
                                  OI_UINT16 len);

OI_STATUS OI_HIDEndpoint_SendMbuf(HID_ENDPOINT_SEND_CFM sendCfm,
                                  HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_MBUF *buf);

OI_STATUS OI_HIDEndpoint_SendSimple(HID_CONNECTION_HANDLE handle,
                                    OI_L2CAP_PSM channel,
                                    OI_BYTE transType,
                                    OI_BYTE transParam);

OI_STATUS OI_HIDEndpoint_SendByte(HID_CONNECTION_HANDLE handle,
                                  OI_L2CAP_PSM channel,
                                  OI_BYTE transType,
                                  OI_BYTE transParam,
                                  OI_BYTE byte);

OI_STATUS OI_HIDEndpoint_FatalError(HID_CONNECTION_HANDLE handle);

void OI_HIDEndpoint_FreeMbufCb(HID_CHANNEL *chan,
                               OI_STATUS status);


void OI_HIDEndpoint_SetTransactionTimeout(HID_CONNECTION_HANDLE handle);

void OI_HIDEndpoint_CancelTransactionTimeout(HID_CONNECTION_HANDLE handle);

HID_CONNECTION *OI_HIDEndpoint_GetConnectionByHandle(HID_CONNECTION_HANDLE handle);

HID_CONNECTION *OI_HIDEndpoint_GetConnectionByBDAddr(OI_BD_ADDR const *addr);

HID_CONNECTION *OI_HIDEndpoint_GetConnectionByCid(OI_L2CAP_CID cid);

HID_CHANNEL *OI_HIDEndpoint_GetChannel(HID_CONNECTION_HANDLE handle,
                                       OI_L2CAP_PSM psm);

#ifdef OI_DEBUG
#error code not present
#else

#define OI_HIDEndpoint_DumpChannel(x)

#endif

#endif /* _OI_HID_ENDPOINT_H */

/**@}*/
