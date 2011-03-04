#ifdef FEATURE_BT_EXTPF_AV
/**
@file
@internal

This file provides internal interfaces for the BLUEmagic 3.0 implementation of
the Audio/Video Distribution Transport Protocol.
*/

/** \addtogroup AVS_Internal */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _AVDTP_PRIVATE_H
#define _AVDTP_PRIVATE_H

#include "oi_mbuf.h"
#include "oi_dispatch.h"

#define AVDTP_MAX_ENDPOINTS 62 //2^6 - 2 reserved

// minimum L2CAP MTU
#define AVDTP_MIN_MTU 335

// signal identifiers
#define AVDTP_DISCOVER          0x01
#define AVDTP_GET_CAPS          0x02
#define AVDTP_SET_CONFIG        0x03
#define AVDTP_GET_CONFIG        0x04
#define AVDTP_RECONFIGURE       0x05
#define AVDTP_OPEN              0x06
#define AVDTP_START             0x07
#define AVDTP_CLOSE             0x08
#define AVDTP_SUSPEND           0x09
#define AVDTP_ABORT             0x0A
#define AVDTP_SECURITY_CONTROL  0x0B

#define AVDTP_GENERAL_REJECT    0xFF /* not defined in the spec, but implied in the test spec */

/* header fields */
#define AVDTP_TRANSACTION_MASK  (BIT7 | BIT6 | BIT5 | BIT4)

// packet type
#define AVDTP_PKT_TYPE_SINGLE    0
#define AVDTP_PKT_TYPE_START     BIT2
#define AVDTP_PKT_TYPE_CONTINUE  BIT3
#define AVDTP_PKT_TYPE_END       (BIT3 | BIT2)
#define AVDTP_PKT_TYPE_MASK      (BIT3 | BIT2)

// message type
#define AVDTP_MSG_TYPE_COMMAND     0
#define AVDTP_MSG_TYPE_RSP_ACCEPT  BIT1
#define AVDTP_MSG_TYPE_RFD         BIT0 //reserved
#define AVDTP_MSG_TYPE_RSP_REJECT  (BIT1 | BIT0)
#define AVDTP_MSG_TYPE_MASK        (BIT1 | BIT0)

// states
enum {
    AVDTP_STATE_IDLE,       /**< State machine has been initialized. */
    AVDTP_STATE_CONFIGURED, /**< INT has successfully configured an ACP’s stream endpoint. */
    AVDTP_STATE_OPEN,       /**< INT and ACP have successfully opened a stream endpoint. */
    AVDTP_STATE_STREAMING,  /**< INT and ACP have successfully established a streaming session on a stream endpoint. */
    AVDTP_STATE_CLOSING,    /**< INT and ACP are closing a stream endpoint. */
    AVDTP_STATE_ABORTING    /**< INT or ACP has requested to abort the stream establishment. */
};

#define NOT_IN_USE 0
#define IN_USE     1

typedef struct _REMOTE_ENDPOINT {

    /** Link to next remote endpoint record */
    struct _REMOTE_ENDPOINT *nextEndpoint;

    /** the remote endpoint Bluetooth device address */
    OI_BD_ADDR addr;

    /** the channel identifier of the signalling channel */
    OI_L2CAP_CID signalCID;
    
    /** The stream endpoint identifier of the remote endpoint. */
    OI_UINT8 remoteSEID;

    /** the most recent transaction label number */
    OI_UINT8 transaction;
    
} AVDTP_REMOTE_ENDPOINT;

typedef struct {

    /** list of Remote Endpoints connected to this endpoint */
    AVDTP_REMOTE_ENDPOINT *remoteEndpoints;

    /** the capabilities of this endpoint */
    const OI_AVDTP_CAP *caps;

    /** the configured capabilities */
    OI_AVDTP_CAP *configCaps;
    OI_AVDTP_CAP *newConfigCaps; /** Temp storage until application accepts */

    /** the channel identifier of the media transport channel */
    OI_L2CAP_CID mediaCID;

    /* Local Service Endpoint ID */
    OI_UINT8 seid;

    OI_UINT8 mediaType;

    OI_UINT8 endpointType;

    /** indicates number of remote endpoints using this endpoint */
    OI_UINT8 inUse;

    /** the state of the endpoint */
    OI_UINT8 state;

    /** the number of Local Endpoint capabilities */
    OI_UINT8 numCaps;
    
    /** the number of configured capabilities */
    OI_UINT8 numConfigCaps;
    OI_UINT8 newNumCaps; /** Temp storage until application accepts */

} AVDTP_ENDPOINT;


/** data structure maintaining signalling channel information */
typedef struct _AVDTP_SIGNAL_INFO {

    /** Linked list of signal structure */
    struct _AVDTP_SIGNAL_INFO *next;

    /** the queue of mbufs to be sent */
    OI_MBUF *sendQ;

    /** the receive packet in progress */
    OI_BYTE *recvData;

    /** the Timeout Callback handle for Command in progress */
    DISPATCH_CB_HANDLE dispatchCB;

    /** the remote endpoint Bluetooth device address */
    OI_BD_ADDR addr;

    /** the outgoing MTU of the signalling channel */
    OI_UINT16 signalMTU;

    /** the number of bytes received so far */
    OI_UINT16 recvLen;

    /** the channel identifier of the signalling channel */
    OI_L2CAP_CID cid;

    /** the number of references to this signalling channel */
    OI_UINT8 refCount;

    /** the number of packet fragments left to send for the current packet */
    OI_UINT8 numSendPkts;

    /** the header byte used for continue and end packets */
    OI_UINT8 headerByte;

    /** the number of remaining packet fragments expected */
    OI_UINT8 numRecvPkts;
    
    /** the transaction number for the received packet */
    OI_UINT8 recvTransaction;

    /** the transaction number for last TXed Cmd packet */
    OI_UINT8 TxCmdTransaction;

    /** marker for Connection Completes seen, for cmd Timeout purposes */
    OI_UINT8 connCmplt;

} AVDTP_SIGNAL_INFO;


OI_STATUS AVDTP_GetAddrFromCID(OI_L2CAP_CID cid,
                               OI_BD_ADDR *addr);

OI_STATUS AVDTP_GetCIDFromAddr(OI_BD_ADDR *addr, 
                               OI_L2CAP_CID *cid);

AVDTP_SIGNAL_INFO *OI_AVDTP_GetSignalInfo(OI_L2CAP_CID cid);

void AVDTPRecv_Complete(OI_L2CAP_CID cid,
                        OI_BYTE     *data,
                        OI_UINT16    dataLen);

void AVDTP_CancelTimeout(OI_L2CAP_CID cid,
                         OI_UINT8     transaction);

OI_BOOL AVDTP_ConnectInProgress( void );

OI_UINT8 AVDTP_ConnectCompletes( void );

#endif /* _AVDTP_PRIVATE_H */

/**@}*/
#endif /* FEATURE_BT_EXTPF_AV */
