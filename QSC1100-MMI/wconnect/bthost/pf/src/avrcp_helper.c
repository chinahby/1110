#ifdef FEATURE_BT_EXTPF_AV
/**
 * @file
 * @internal  
 * helper functions for the Audio/Video Remote Control Profile implementation
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVRCP

#include "oi_bt_assigned_nos.h"
#include "oi_bytestream.h"
#include "oi_debug.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_avrcp_helper.h"
#include "oi_avrcp_consts.h"
#include "oi_avctp.h"
#include "oi_init_flags.h"
#include "oi_std_utils.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

// AVRCP_COMMON common;


/********** Local functions */
static void Debug_PrintPacket(PACKETQ_ENTRY *packet);
static void Debug_PrintPacketQueue(void);

static void EnqueuePacket(PACKETQ_ENTRY *packet);
static PACKETQ_ENTRY *DequeuePacket(void);
void FlushPacketsByAddr(const OI_BD_ADDR *addr);
static OI_STATUS TryAvctpSend(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_UINT8 type, OI_UINT8 *data, OI_UINT16 dataLen);
static OI_STATUS AVRCPCOMMON_ValidateAndComputePacketSize(const OI_AVRCP_FRAME *frame, OI_UINT16 *size);

/********** Callback prototypes */

static void AvctpConnectInd(OI_BD_ADDR *addr);
static void AvctpDisconnectInd(OI_BD_ADDR *addr, OI_STATUS result);
static void AvctpRecvInd(OI_BD_ADDR *addr, OI_UINT8 transaction, OI_UINT8 type, OI_BOOL invalidPID, OI_BYTE *data, OI_UINT16 dataLen);
static void AvctpSendCfm(OI_BD_ADDR *addr, OI_UINT16 pid, OI_UINT8 transaction, OI_UINT8 type, OI_BYTE *data, OI_UINT16 dataLen, OI_STATUS result);
static void AvctpConnectCfm(OI_BD_ADDR *addr, OI_UINT16 inMTU, OI_UINT16 outMTU, OI_STATUS result);

/********** DEBUG */
#ifdef OI_DEBUG
#error code not present
#else

#define StateText(state) ""
#define AVRCP_Dump() ""
    
#endif
    
/********** Callback function implementation */

    static void AvctpConnectInd(OI_BD_ADDR *addr)
{
    OI_DBGTRACE(("OI_AVRCP_CONNECT_IND(addr=%:)", addr));    
    
    OI_AVRCP_This->cb->connectInd(addr);
}

static void AvctpDisconnectInd(OI_BD_ADDR *addr, OI_STATUS result)
{
    OI_STATUS avrcpStatus;

    OI_DBGTRACE(("OI_AVRCP_DISCONNECT_IND(addr=%:,result=%!)", addr, result));
    
    OI_AVRCPController_FlushTransactions(addr, result);
    FlushPacketsByAddr(addr);

    if (OI_AVRCP_This->controller) {
        avrcpStatus = OI_AVRCPCommon_ClearAddressTransactionTable(addr, OI_AVRCP_This->controller->transactionTable);
        if (!OI_SUCCESS(avrcpStatus)) {
            OI_DBGPRINT2(("Failed to clean up the controller transaction table for device %: status %!", addr, avrcpStatus));
        }
    }

    if (OI_AVRCP_This->target) {
        avrcpStatus = OI_AVRCPCommon_ClearAddressTransactionTable(addr, OI_AVRCP_This->target->transactionTable);
    if(!OI_SUCCESS(avrcpStatus)) {
            OI_DBGPRINT2(("Failed to clean up the target transaction table for device %: status %!", addr, avrcpStatus));
        }
    }

    if (OI_AVRCP_This->numConnections > 0) {
        OI_AVRCP_This->numConnections -= 1;
    }
    
    OI_AVRCP_This->cb->disconnectInd(addr, result);
}

static void AvctpRecvInd(OI_BD_ADDR *addr,
                         OI_UINT8 transaction,
                         OI_UINT8 type,
                         OI_BOOL invalidPID,
                         OI_UINT8 *data,
                         OI_UINT16 dataLen)
{
    OI_AVRCP_FRAME frame;
    OI_STATUS status;

    OI_DBGPRINT(("AvctpRecvInd: %s, %d", OI_BDAddrText(addr),transaction));

    if (invalidPID) {
        OI_DBGPRINT(("Received invalidPID response"));
        if (type == OI_AVCTP_TYPE_RSP) {
            OI_AVRCPController_CommandFailed(addr, transaction, OI_FAIL);
        } else {
            OI_LOG_ERROR(("Peer sent command with invalidPID flag set. Ignoring."));
        }
        return;
    }

    if (dataLen < AVRCP_MIN_PACKET_SIZE) {
        OI_DBGPRINT(("Packet is too small to be an AVRCP frame."));
        return;
    }

    status = AVRCPCOMMON_ExtractFrame(&frame, data, dataLen);

    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("Couldn't interpret AVRCP frame."));
        return;
    }

    switch (type) {
        case OI_AVCTP_TYPE_CMD:
            OI_DBGPRINT(("Target received command"));
            if ( OI_AVRCP_This->Target_AvctpRecvInd ){
                OI_AVRCP_This->Target_AvctpRecvInd(addr, transaction, &frame);
            }
            break;
        case OI_AVCTP_TYPE_RSP:
            OI_DBGPRINT(("Controller received reply"));
            if ( OI_AVRCP_This->Controller_AvctpRecvInd ){
                OI_AVRCP_This->Controller_AvctpRecvInd(addr, transaction, &frame);
            }
            break;
        default:
            OI_DBGPRINT(("Received invalid AVRCP packet type"));
    }
}

static void AvctpSendCfm(OI_BD_ADDR *addr,
                         OI_UINT16 pid,
                         OI_UINT8 transaction,
                         OI_UINT8 type,
                         OI_UINT8 *data,
                         OI_UINT16 dataLen,
                         OI_STATUS result)
{
    PACKETQ_ENTRY *entry;
    OI_STATUS status;

    OI_DBGPRINT(("AvctpSendCfm %!", result));
    if (!OI_SUCCESS(result) && type == OI_AVCTP_TYPE_CMD) {
        OI_AVRCPController_CommandFailed(addr, transaction, result);        
    }
    OI_FreeIf(&data);

    entry = DequeuePacket();

    if (entry) {
        OI_DBGPRINT2(("Sending to %: (%d)", &entry->addr, entry->transaction));
        OI_DBGPRINT2(("%@", entry->data, entry->len));

        status = OI_AVCTP_Send(&entry->addr,
                               OI_UUID_AV_RemoteControl,
                               entry->transaction,
                               entry->type,
                               entry->data,
                               entry->len);

        OI_DBGPRINT(("AVRCP_Send: %!", status));
        if (status == OI_STATUS_WRITE_IN_PROGRESS) {
            OI_LOG_ERROR(("Got WRITE_IN_PROGRESS from AVCTP on a sendcfm. Trying again later."));
            EnqueuePacket(entry);
            return;
        } else if (!OI_SUCCESS(status)) {
            if (type == OI_AVCTP_TYPE_CMD) {
                OI_AVRCPController_CommandFailed(&entry->addr, entry->transaction, status);
            }
            OI_Free(entry->data);
        }
        OI_Free(entry);
    }
}

static void AvctpConnectCfm(OI_BD_ADDR *addr,
                            OI_UINT16 inMTU,
                            OI_UINT16 outMTU,
                            OI_STATUS result)
{
    OI_STATUS avrcpStatus = OI_OK;
    
    OI_DBGTRACE(("AvctpConnectCfm(addr=%:,inMTU=%d,outMTU=%d,result=%!)",
                 addr, inMTU, outMTU, result));

    if (OI_SUCCESS(result)) {
        
        if (OI_AVRCP_This->controller) {
            avrcpStatus = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECTED, OI_AVRCP_This->controller->transactionTable);
            if (!OI_SUCCESS(avrcpStatus)) {
                OI_DBGPRINT2(("Failed to update the controller transaction table for device %: status %!", addr, avrcpStatus));
            }
        }
        if (OI_AVRCP_This->target) {
            avrcpStatus = OI_AVRCPCommon_UpdateAddressTransactionTable(addr, AVRCP_CONNECTED, OI_AVRCP_This->target->transactionTable);
            if (!OI_SUCCESS(avrcpStatus)) {
                OI_DBGPRINT2(("Failed to update the target transaction table for device %: status %!", addr, avrcpStatus));
            }
        }

        if(!OI_SUCCESS(avrcpStatus)) {
            result = OI_AVCTP_Disconnect(addr, OI_UUID_AV_RemoteControl);
            if (!OI_SUCCESS(result)) {
                OI_LOG_ERROR(("AVRCP error %!. Failed to disconnet AVCTP %!, still connected", result, avrcpStatus));
            }
            result = avrcpStatus;           
        }
    }

    if (!OI_SUCCESS(result)) {
        
        if (OI_AVRCP_This->controller) {
            avrcpStatus = OI_AVRCPCommon_ClearAddressTransactionTable(addr, OI_AVRCP_This->controller->transactionTable);
        if(!OI_SUCCESS(avrcpStatus)) {
                OI_DBGPRINT2(("Failed to clean up the controller transaction table for device %: status %!", addr, avrcpStatus));
            }
        }
        
        if (OI_AVRCP_This->target) {
            avrcpStatus = OI_AVRCPCommon_ClearAddressTransactionTable(addr, OI_AVRCP_This->target->transactionTable);
            if (!OI_SUCCESS(avrcpStatus)) {
                OI_DBGPRINT2(("Failed to clean up the target transaction table for device %: status %!", addr, avrcpStatus));
            }
        }
        
        if (OI_AVRCP_This->numConnections > 0 ) {
            OI_AVRCP_This->numConnections -= 1;
        }
    }

    OI_AVRCP_This->cb->connectCfm(addr, result);
}

/********** Private functions' implementations */

static OI_STATUS TryAvctpSend(OI_BD_ADDR *addr,
                              OI_UINT8 transaction,
                              OI_UINT8 type,
                              OI_UINT8 *data,
                              OI_UINT16 dataLen)
{
    OI_STATUS status;

    status = OI_AVCTP_Send(addr,
                           OI_UUID_AV_RemoteControl,
                           transaction,
                           type,
                           data,
                           dataLen);

    OI_DBGPRINT(("OI_AVCTP_Send: %!", status));
    if (status == OI_STATUS_WRITE_IN_PROGRESS) {
        PACKETQ_ENTRY *packet;

        packet = OI_Malloc(sizeof(PACKETQ_ENTRY));
        if (packet == NULL) {
            OI_DBGPRINT(("Insufficient memory to enqueue packet for later delivery"));
            return OI_STATUS_OUT_OF_MEMORY;
        }

        packet->addr = *addr;
        packet->transaction = transaction;
        packet->type = type;
        packet->data = data;
        packet->len = dataLen;

        EnqueuePacket(packet);

        status = OI_OK;
    }

    return status;
}

static void Debug_PrintPacket(PACKETQ_ENTRY *packet)
{
    OI_DBGPRINT2(("%s Tr=%d Type=%d", OI_BDAddrText(&packet->addr),
                  packet->transaction, packet->type));
    OI_DBGPRINT2(("<0x%08lx:0x%08lx>", packet, packet->data));
    OI_DBGPRINT2(("%@", packet->data, packet->len));
}

static void Debug_PrintPacketQueue(void)
{
    PACKETQ_ENTRY *packet;
    OI_DBGPRINT2(("Head: %08lx", OI_AVRCP_This->packetq.head));
    packet = OI_AVRCP_This->packetq.head;
    while (packet) {
        OI_DBGPRINT2(("%08lx: %s %2d %2x %08lx %3d <%08lx>", packet,
                      OI_BDAddrText(&packet->addr),
                      packet->transaction, packet->type,
                      packet->data, packet->len, packet->next));
        packet = packet->next;
    }
    OI_DBGPRINT2(("Tail: %08lx", OI_AVRCP_This->packetq.tail));
}

static void EnqueuePacket(PACKETQ_ENTRY *packet)
{
    OI_DBGPRINT2(("Enqueing packet for later transmission"));

    Debug_PrintPacket(packet);

    packet->next = NULL;
    if (OI_AVRCP_This->packetq.tail == NULL) {
        OI_ASSERT(OI_AVRCP_This->packetq.head == NULL);
        OI_AVRCP_This->packetq.head = packet;
        OI_AVRCP_This->packetq.tail = packet;
    } else {
        OI_AVRCP_This->packetq.tail->next = packet;
        OI_AVRCP_This->packetq.tail = packet;
    }
    Debug_PrintPacketQueue();
}

static PACKETQ_ENTRY *DequeuePacket(void)
{
    PACKETQ_ENTRY *packet;
    packet = OI_AVRCP_This->packetq.head;

    Debug_PrintPacketQueue();
    if (packet == NULL) {
        OI_DBGPRINT2(("No packets remaining to send"));
    } else {
        OI_DBGPRINT2(("Dequeing packet for immediate transmission"));
        Debug_PrintPacket(packet);
    }

    if (OI_AVRCP_This->packetq.head == NULL) {
        OI_ASSERT(OI_AVRCP_This->packetq.tail == NULL);
        return NULL;
    } else if (OI_AVRCP_This->packetq.head == OI_AVRCP_This->packetq.tail) {
        OI_AVRCP_This->packetq.head = NULL;
        OI_AVRCP_This->packetq.tail = NULL;
        return packet;
    } else {
        OI_AVRCP_This->packetq.head = OI_AVRCP_This->packetq.head->next;
        return packet;
    }
}

void OI_AVRCPCOMMON_FlushPacketByTransaction(OI_BD_ADDR *addr,
                                             OI_UINT8 trans)
{
    PACKETQ_ENTRY **p;

    OI_DBGPRINT2(("Flushing transaction %d for device %:\n", trans, addr));
    
    p = &OI_AVRCP_This->packetq.head;

    while (*p) {
        if ((trans == (*p)->transaction) && (SAME_BD_ADDR(addr, &(*p)->addr))) {
            PACKETQ_ENTRY *victim = *p;  /* Pointer to element to be deleted */

            *p = victim->next;      /* Cause cell that used to point to the "victim" cell to point to its successor instead */

            /* Update the tail if we're deleting the old tail */
            if (!victim->next) {
                OI_ASSERT((OI_AVRCP_This->packetq.tail == victim));
                OI_AVRCP_This->packetq.tail = *p;
            }

            OI_Free(victim->data);
            OI_Free(victim);
            break;
        } else {
            p = &(*p)->next;
        }
    }
    Debug_PrintPacketQueue();
}

void FlushPacketsByAddr(const OI_BD_ADDR *addr)
{
    PACKETQ_ENTRY **p;
    OI_DBGPRINT2(("Flushing packets for device %:\n", addr));
    p = &OI_AVRCP_This->packetq.head;
    while (*p) {
        if (SAME_BD_ADDR(addr, &(*p)->addr)) {
            PACKETQ_ENTRY *victim;
            victim = *p;
            *p = (*p)->next;
            OI_Free(victim->data);
            OI_Free(victim);
        } else {
            p = &(*p)->next;
        }
    }
    OI_AVRCP_This->packetq.tail = *p;
    Debug_PrintPacketQueue();
}

static void ChangeState(OI_BD_ADDR *addr,
                        OI_UINT8 *currentState,
                        OI_UINT8 newState)
{
    if (newState != *currentState) {
        OI_DBGPRINT(("Connection state (%:) %s -> %s\n", addr, StateText(*currentState), StateText(newState)));
        *currentState = newState;
    }
#ifdef OI_DEBUG    
#error code not present
#endif    
}

AVRCP_TRANS_TABLE * LookupTransactionTable(OI_BD_ADDR *addr,
                                           OI_BOOL isController)
{
    OI_INT j;
    AVRCP_TRANS_TABLE *table;

    OI_ASSERT(addr);
    
    if (isController) {
        OI_ASSERT(OI_AVRCP_This->controller);
        OI_ASSERT(OI_AVRCP_This->controller->transactionTable);
        table = OI_AVRCP_This->controller->transactionTable;
    } else {
        OI_ASSERT(OI_AVRCP_This->target);
        OI_ASSERT(OI_AVRCP_This->target->transactionTable);
        table = OI_AVRCP_This->target->transactionTable;        
    }
    
    for (j = 0; j < OI_AVRCP_This->maxConnections; j++) {    
        if (SAME_BD_ADDR(&table[j].addr,addr)) {
            return &table[j];
        }
    }

    return NULL;
}
    
OI_STATUS OI_AVRCPCommon_ClearAddressTransactionTable(OI_BD_ADDR *addr,
                                                      AVRCP_TRANS_TABLE *transactionTable)
{

    OI_INT i;
    
    OI_ASSERT(addr);

    if (NULL != transactionTable) {
            for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
                OI_DBGPRINT2(("Clear: Found address %:\n", addr));
            if (SAME_BD_ADDR(&(transactionTable[i].addr), addr)) {
                transactionTable[i].addr = OI_ZeroAddr;
                ChangeState(addr, &transactionTable[i].connectionState, AVRCP_NOT_CONNECTED);
                    return OI_OK;
                }
            }
            OI_DBGPRINT2(("Address entry does not exist!"));
            return OI_STATUS_NOT_FOUND;
        }
    return OI_OK;
}

OI_STATUS OI_AVRCPCommon_UpdateAddressTransactionTable(OI_BD_ADDR *addr,
                                                       OI_UINT8 state,
                                                       AVRCP_TRANS_TABLE *transactionTable)
{
    OI_INT i;

    OI_ASSERT(addr);
    
    if (NULL != transactionTable) {
        for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
            if (SAME_BD_ADDR(&(transactionTable[i].addr), addr)) {
                OI_DBGPRINT2(("Update transaction table: Found address %:\n", addr));
                switch (transactionTable[i].connectionState) {
                case AVRCP_CONNECTED:
                    if (state == AVRCP_DISCONNECT_PENDING) {
                        break;
                    } else {
                        return OI_STATUS_CONNECTION_EXISTS;
                    }
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */
                case AVRCP_CONNECT_PENDING:
                    if ((state == AVRCP_CONNECTED) || (state == AVRCP_NOT_CONNECTED)){
                        break;
                    } else {
                        return OI_STATUS_PENDING;
                    }
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */
                case AVRCP_DISCONNECT_PENDING:
                    if(state == AVRCP_NOT_CONNECTED) {
                        break;
                    } else {
                        return OI_STATUS_PENDING;
                    }
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */    
                case AVRCP_NOT_CONNECTED:
                    if(state == AVRCP_CONNECT_PENDING) {
                        break;
                    } else {
                        return OI_STATUS_NOT_CONNECTED;
                    }
                default:
                    return OI_STATUS_INVALID_STATE;                            
                }                    
                ChangeState (addr,  &transactionTable[i].connectionState, state);
                return OI_OK;
            }
        }
        for (i = 0; i < OI_AVRCP_This->maxConnections; i++) {
            if (SAME_BD_ADDR(&(transactionTable[i].addr), &OI_ZeroAddr)) {
                OI_DBGPRINT2(("Update transaction table %:\n", addr));
                OI_ASSERT (transactionTable[i].connectionState == AVRCP_NOT_CONNECTED);
                if (state == AVRCP_CONNECT_PENDING) {
                    transactionTable[i].addr = *addr;
                    ChangeState (addr,  &transactionTable[i].connectionState, state);
                    return OI_OK;
                }
            }
        }
        
        return OI_STATUS_NOT_FOUND;
    }

    return OI_OK;
}

/********** Public API */

OI_STATUS AVRCPCOMMON_Register(void)
{
    OI_STATUS status;

    OI_CHECK_INIT(AVRCP);

    status =  OI_AVCTP_Register(OI_UUID_AV_RemoteControl,
                                &AvctpConnectInd,
                                &AvctpDisconnectInd,
                                &AvctpRecvInd,
                                &AvctpConnectCfm,
                                &AvctpSendCfm);

    if (status == OI_STATUS_ALREADY_REGISTERED) {
        return OI_OK;
    }

    return status;
}

/* Must be called *after* setting the respective callback pointers to NULL */
OI_STATUS AVRCPCOMMON_Deregister(void)
{
    PACKETQ_ENTRY *entry;
    OI_CHECK_INIT(AVRCP);

    if ( (OI_AVRCP_This->targetCb == NULL) &&
         (OI_AVRCP_This->controllerCb == NULL) ){

        while ( (entry = DequeuePacket()) != NULL ){
            OI_FreeIf(&entry->data);
            OI_FreeIf(&entry);
        }

        return OI_AVCTP_Deregister(OI_UUID_AV_RemoteControl);
    }
    return OI_OK;
}

OI_STATUS AVRCPCOMMON_ValidateAndComputePacketSize(const OI_AVRCP_FRAME *frame, OI_UINT16 *size)
{
    OI_UINT16 tmpSize;
    tmpSize = AVRCP_MIN_PACKET_SIZE;

    OI_CHECK_INIT(AVRCP);

    if (frame->cmd > OI_AVRCP_CMD_RESPONSE_MAX) {
        OI_DBGPRINT(("frame->cmd out of range: 0x%02x", frame->cmd));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (frame->operandCount > 0 && frame->operand == NULL) {
        OI_DBGPRINT(("frame->operandCount > 0 but operand == NULL"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (frame->subunitType == OI_AVRCP_SUBUNIT_TYPE_EXTENDED) {
        return OI_STATUS_NOT_IMPLEMENTED;
    }

    if (frame->subunitID == OI_AVRCP_SUBUNIT_ID_EXTENDED) {
        return OI_STATUS_NOT_IMPLEMENTED;
    }

    tmpSize += frame->operandCount;    
    *size = tmpSize;
    return OI_OK;
}

OI_STATUS AVRCPCOMMON_ExtractFrame(OI_AVRCP_FRAME *frame, OI_BYTE *packet, OI_UINT packetLen)
{
    OI_BYTE_STREAM bs;

    OI_CHECK_INIT(AVRCP);

    OI_DBGPRINT2(("Received packet:"));
    OI_DBGPRINT2(("%@", packet, packetLen));

    if (packetLen < AVRCP_MIN_PACKET_SIZE) {
#ifndef OI_CODE
        return OI_STATUS_INVALID_PACKET;
#else
#error code not present
#endif /* OI_CODE */
    }

    ByteStream_Init(bs, packet, packetLen);
    ByteStream_Open(bs, BYTESTREAM_READ);

    ByteStream_GetUINT8_Checked(bs, frame->cmd);

    ByteStream_Extract(bs, frame->subunitID, 0, 3);
    ByteStream_Extract(bs, frame->subunitType, 3, 5);
    ByteStream_Next(bs);

    if ((frame->subunitType == OI_AVRCP_SUBUNIT_TYPE_EXTENDED) || (frame->subunitID == OI_AVRCP_SUBUNIT_ID_EXTENDED)) {
        goto BadPacket;
    }

    ByteStream_GetUINT8_Checked(bs, frame->opcode);

    if (ByteStream_Error(bs)) {
        goto BadPacket;
    }

    frame->operand = ByteStream_GetCurrentBytePointer(bs);
    frame->operandCount = ByteStream_NumReadBytesAvail(bs);
    ByteStream_Close(bs);

    return OI_OK;

BadPacket:
    ByteStream_Close(bs);
    return OI_STATUS_INVALID_PACKET;
}

OI_STATUS AVRCPCOMMON_Send(OI_BD_ADDR *addr,
                           OI_UINT8 transaction,
                           OI_AVRCP_FRAME *frame)
{
    OI_BYTE_STREAM bs;
    OI_BYTE *packetData;
    OI_UINT16 packetLen;
    OI_UINT8 avctpType;
    OI_STATUS status;

    OI_CHECK_INIT(AVRCP);

    ARGCHECK(frame != NULL);

    status = AVRCPCOMMON_ValidateAndComputePacketSize(frame, &packetLen);

    if (!OI_SUCCESS(status)) {
        return status;
    }

    packetData = OI_Calloc(packetLen);

    if (packetData == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Init(bs, packetData, packetLen);
    ByteStream_Open(bs, BYTESTREAM_WRITE);

    ByteStream_PutUINT8(bs, frame->cmd);
    ByteStream_Insert(bs, frame->subunitType, 3, 5);
    ByteStream_Insert(bs, frame->subunitID, 0, 3);
    ByteStream_Next(bs);
    ByteStream_PutUINT8(bs, frame->opcode);

    OI_ASSERT((OI_UINT)ByteStream_NumWriteBytesAllowed(bs) >= frame->operandCount);

    ByteStream_PutBytes(bs, frame->operand, frame->operandCount);

    ByteStream_Close(bs);

    avctpType = (OI_AVRCP_IS_COMMAND(frame->cmd) ? OI_AVCTP_TYPE_CMD : OI_AVCTP_TYPE_RSP);

    OI_DBGPRINT2(("Sending to %s <0x%08lx> trans %d", OI_BDAddrText(addr), packetData, transaction));
    OI_DBGPRINT2(("%@", packetData, packetLen));

    status = TryAvctpSend(addr,
                          transaction,
                          avctpType,
                          packetData,
                          packetLen);


    OI_DBGPRINT(("TryAvctpSend: %!", status));
    if (!OI_SUCCESS(status)) {
        OI_Free(packetData);
    }

    return status;
}


/********** SDP Records */

const OI_DATAELEM ConstAvrcpSupportedCategories = OI_ELEMENT_UINT16(0);


static const OI_DATAELEM L2CAP_Descriptor[] =  {
    OI_ELEMENT_UUID32(OI_UUID_L2CAP),
    OI_ELEMENT_UINT16(OI_PSM_AVCTP)
};

static const OI_DATAELEM AVCTP_Descriptor[] = {
    OI_ELEMENT_UUID32(OI_UUID_AVCTP),
    OI_ELEMENT_UINT16(0x0100)                  /* AVCTP Version 1.0 */
};

const OI_DATAELEM OI_ProtocolDescriptorList[2] = {
    OI_ELEMENT_SEQ(L2CAP_Descriptor),
    OI_ELEMENT_SEQ(AVCTP_Descriptor)
};


static OI_DATAELEM AvrcpProfileDescriptors[] = {
    OI_ELEMENT_UUID32(OI_UUID_AV_RemoteControl),
    OI_ELEMENT_UINT16(0x0100)                 /* AVRCP Version 1.0 */
};

const OI_DATAELEM OI_ProfileDescriptorList[1] = {
    OI_ELEMENT_SEQ(AvrcpProfileDescriptors)
};

void AvrcpProfileVersionInit(OI_BOOL metadata) {
  if (metadata) {
      OI_SET_UINT_ELEMENT(AvrcpProfileDescriptors[1], (OI_UINT16)0x0103);
  }
}
#endif /* FEATURE_BT_EXTPF_AV */

