#ifdef FEATURE_BT_EXTPF_AV
/**
@file
@internal

*/

/** \addtogroup AVRC_Internal */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef __AVRCP_COMMON_H
#define __AVRCP_COMMON_H

#include "oi_avrcp.h"
#include "oi_dispatch.h"
#include "oi_sdpdb.h"

#define AVRCP_MIN_PACKET_SIZE 3
#define AVRCP_MAX_TRANSACTIONS 16
#define AVRCP_MAX_CONNECTIONS 16

typedef struct _AVCTP_PACKET_ENTRY {
    OI_BD_ADDR addr;
    
    OI_UINT8 transaction;
    OI_UINT8 type;
    OI_UINT8 *data;
    OI_UINT16 len;
    struct _AVCTP_PACKET_ENTRY *next;
} PACKETQ_ENTRY;

enum {
    AVRCP_UNUSED = 0,
    AVRCP_SENDING,
    AVRCP_AWAITING_REPLY,
    AVRCP_RECEIVED_REPLY,
    AVRCP_TIMED_OUT
};

enum {
    AVRCP_NOT_CONNECTED             = 0,
    AVRCP_CONNECTED                 = 1,
    AVRCP_CONNECT_PENDING           = 2,
    AVRCP_DISCONNECT_PENDING        = 3
};

typedef struct {
    OI_AVRCP_FRAGMENT_DATA attrFragment;
    
    OI_UINT16 currAttrIndex;   /* Current attribute/element/value index */

    union {
        
        OI_UINT16 currAttrDataPos; /* 0xffff - means start from attribute/element header, i.e., id, charset, size...;
                                    * any other value means a current position in the data (text) string */
        OI_UINT16 capability;      /* capability indicator for capabilities request */
    } u;
    
    OI_UINT16 count;           /* Total count of attrs/elements/capabilities */
    OI_UINT8 PDU;              /* PDU of the pending response */

} AVRCP_FRAGMENT;

typedef struct{
    OI_UINT8           state;
    OI_UINT8  eventID;
    DISPATCH_CB_HANDLE timeoutFn;
} AVRCP_TRANS_TABLE_ENTRY;    

typedef struct{
    OI_BD_ADDR               addr;
    OI_UINT8                 connectionState;
    AVRCP_TRANS_TABLE_ENTRY *transaction;
    OI_UINT8                 nextTransaction;
    AVRCP_FRAGMENT          *fragment;
} AVRCP_TRANS_TABLE;    

typedef struct {
    OI_UINT32          sdpHandle;
    AVRCP_TRANS_TABLE *transactionTable;
    OI_BYTE companyID[3];
    OI_UINT16 tgResponseFrameSize;
} AVRCP_TARGET;

typedef struct {
    OI_UINT32          sdpHandle;
    AVRCP_TRANS_TABLE *transactionTable;
    OI_INTERVAL        tRCP;
    OI_INTERVAL        tMTC;
    OI_INTERVAL        tMTP;
} AVRCP_CONTROLLER;

typedef void (AVCTP_RECV_IND)(OI_BD_ADDR     *addr,
                              OI_UINT8        transaction,
                              OI_AVRCP_FRAME *frame);

typedef struct {
    const OI_AVRCP_CALLBACKS            *cb;
    const OI_AVRCP_CONTROLLER_CALLBACKS *controllerCb;
    const OI_AVRCP_TARGET_CALLBACKS     *targetCb;
    AVRCP_TARGET                        *target;
    AVRCP_CONTROLLER                    *controller;
    AVCTP_RECV_IND                      *Target_AvctpRecvInd;
    AVCTP_RECV_IND                      *Controller_AvctpRecvInd;
    struct {
        PACKETQ_ENTRY *head;
        PACKETQ_ENTRY *tail;
    } packetq;
    OI_UINT8 maxTransactions;
    OI_UINT8 maxConnections;
    OI_UINT8 numConnections;
    OI_BOOL  metadata;
} AVRCP_THIS;

/* Expose "OI_AVRCP_This" pointer to AVRCP profile only */
extern AVRCP_THIS * OI_AVRCP_This;

OI_STATUS AVRCPCOMMON_Register(void);
OI_STATUS AVRCPCOMMON_Deregister(void);

OI_STATUS AVRCPCOMMON_ExtractFrame(OI_AVRCP_FRAME *frame, OI_BYTE *packet, OI_UINT packetLen);

OI_STATUS AVRCPCOMMON_RequiredPacketLen(const OI_AVRCP_FRAME *frame, OI_UINT *size);
OI_STATUS AVRCPCOMMON_Send(OI_BD_ADDR *addr,
                           OI_UINT8 transaction,
                           OI_AVRCP_FRAME *frame);

AVRCP_TRANS_TABLE * LookupTransactionTable(OI_BD_ADDR *addr,
                                           OI_BOOL isController);

void OI_AVRCPCOMMON_FlushPacketByTransaction(OI_BD_ADDR *addr,
                                             OI_UINT8 transaction);

OI_STATUS OI_AVRCPCommon_UpdateAddressTransactionTable(OI_BD_ADDR *addr,
                                                       OI_UINT8 state,
                                                       AVRCP_TRANS_TABLE *transactionTable);
OI_STATUS OI_AVRCPCommon_ClearAddressTransactionTable(OI_BD_ADDR *addr,
                                                      AVRCP_TRANS_TABLE *transactionTable);

/* Controller / Target internal entry points */

void OI_AVRCPController_FlushTransactions(OI_BD_ADDR *addr, OI_STATUS reason);
void OI_AVRCPController_CommandFailed(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_STATUS reason);

/* SDP Records */
void AvrcpProfileVersionInit(OI_BOOL metadata);

extern const OI_DATAELEM OI_ProfileDescriptorList[1];
extern const OI_DATAELEM OI_ProtocolDescriptorList[2];
extern const OI_DATAELEM ConstAvrcpSupportedCategories;
extern OI_SDPDB_SERVICE_RECORD AvrcpControllerServiceRecord;
extern OI_SDPDB_SERVICE_RECORD AvrcpTargetServiceRecord;
#endif

/**@}*/
#endif /* FEATURE_BT_EXTPF_AV */
