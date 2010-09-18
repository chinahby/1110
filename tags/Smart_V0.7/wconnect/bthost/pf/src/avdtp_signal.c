#ifdef FEATURE_BT_EXTPF_AV
/**
 * @file
 * @internal
 * This file implements portions of Audio/Video Distribution Protocol specification 
 * relating to the assembly, fragmentation, and queuing of AVDTP command and response 
 * messages.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVDTP

#include "oi_bytestream.h"
#include "oi_avdtp_signal.h"
#include "oi_avdtp_private.h"
#include "oi_l2cap.h"
#include "oi_mbuf.h"
#include "oi_l2cap_mbuf.h"
#include "oi_testprint.h"
#include "oi_dispatch.h"

/** Local Timeout Structure */

typedef struct{
    OI_L2CAP_CID cid;
    OI_UINT8     transaction;
    OI_UINT8     command;
}TO_STRUCT;


static OI_STATUS SendFragment(AVDTP_SIGNAL_INFO *info);
static OI_MBUF *DequeueSendPkt(AVDTP_SIGNAL_INFO *info);
static OI_STATUS StartSend(AVDTP_SIGNAL_INFO *info);
static void SetCommandTimeout(OI_L2CAP_CID cid, OI_UINT8 transaction, OI_UINT8 command);

/** When a write is confirmed, decrement the number of packets left to
  send. If there are no packets left to send, remove the first
  packet. If there is another packet in the queue, send that
  packet. */
static void MBUFWriteCfm(OI_L2CAP_CID cid,
                         OI_MBUF *mbuf,
                         OI_STATUS result)
{
    AVDTP_SIGNAL_INFO *info;
    OI_MBUF *sentMbuf;

    OI_MBUF_Free(mbuf);
    info = OI_AVDTP_GetSignalInfo(cid);
    if (!info) {
        OI_LOG_ERROR(("No signalling channel found for CID %d\n", cid));
        return;
    }

    OI_ASSERT(info->numSendPkts > 0);
#ifndef OI_CODE
    info->numSendPkts = 1;
#endif
    info->numSendPkts--;
    if (0 == info->numSendPkts) {
        sentMbuf = DequeueSendPkt(info);
        OI_MBUF_Free(sentMbuf);
        if (info->sendQ != NULL) {
            StartSend(info);
        }
    } else {
        SendFragment(info);
    }
}


/******************************************************************

  fragmentation

 *****************************************************************/

/** Start sending a packet to be fragmented. Calculate the number of
  packets to be sent and try to send the first packet. */
static OI_STATUS StartSend(AVDTP_SIGNAL_INFO *info)
{
    OI_UINT16 dataLen;
#ifndef OI_CODE
    OI_UINT16 mtu = AVDTP_MIN_MTU;
#endif /* OI_CODE */

    OI_ASSERT(info);
    OI_ASSERT(0 == info->numSendPkts);
    OI_ASSERT(info->sendQ);

    dataLen = OI_MBUF_NumBytes(info->sendQ);

#ifndef OI_CODE
    /* the mtu if set shall be considered */
    if (info->signalMTU != 0) {
      mtu = info->signalMTU;
    }
#endif /* OI_CODE */

    /** Calculate number of packets to be sent. dataLen already
      includes 2 header bytes, but we need to add 1 more to the first packet.
      Each packet other than the first has 1 extra header byte. */
#ifndef OI_CODE
    info->numSendPkts = (dataLen / (mtu - 1)) + 1;
    OI_DBGPRINT2(("signalMTU = %d\n", mtu));
#else
#error code not present
#endif /* OI_CODE */
    OI_DBGPRINT2(("dataLen = %d\n", dataLen));
    OI_DBGPRINT2(("Number of fragments to send = %d\n", info->numSendPkts));
    return SendFragment(info);
}

/**
 * Enqueue a packet to be sent on the specified signalling channel.
 *
 * @param info      pointer to the info structure on which to enqueue the packet
 * @param mbuf      the mbuf to enqueue
 */
static void EnqueueSendPkt(AVDTP_SIGNAL_INFO *info,
                           OI_MBUF *mbuf)
{
    OI_MBUF *current;

    OI_ASSERT(info);
    OI_ASSERT(mbuf);

    mbuf->next = NULL;
    if (NULL == info->sendQ) {
        info->sendQ = mbuf;
    } else {
        current = info->sendQ;
        while (current->next != NULL) current = current->next;
        current->next = mbuf;
    }
}


/** Remove (dequeue) the head element. */
static OI_MBUF *DequeueSendPkt(AVDTP_SIGNAL_INFO *info)
{
    OI_MBUF *current;

    OI_ASSERT(info);

    OI_DBGPRINT2(("DequeueSendPkt\n"));

    current = info->sendQ;
    if (current != NULL) {
        info->sendQ = current->next;
    }
    return current;
}


/** Send a fragment. */
static OI_STATUS SendFragment(AVDTP_SIGNAL_INFO *info)
{
    OI_MBUF *mbuf;
    OI_BYTE header[3]; // header is always <= 3 bytes
    OI_UINT16 headerLen;
    OI_STATUS result;
    OI_MBUF *src = info->sendQ;
#ifndef OI_CODE
    OI_UINT16 mtu = AVDTP_MIN_MTU;
#endif /* OI_CODE */

    OI_ASSERT(info);

    OI_DBGPRINT2(("SendFragment\n"));

    if (NULL == src) {
        OI_DBGPRINT2(("No packets to be sent\n"));
        return OI_OK;
    }
#ifndef OI_CODE
    /* Disable support for fragmenting signalling messages.
     * Only send one packet here */
    info->numSendPkts = 1;

    /* the mtu if set shall be considered */
    if (info->signalMTU != 0) {
      mtu = info->signalMTU;
    }
#endif /* OI_CODE */

    if (OI_MBUF_IsPristine(src)) { // first packet
        if (info->numSendPkts > 1) {
            OI_DBGPRINT2(("Sending START fragment\n"));
            OI_MBUF_PullBytes(&info->headerByte, src, 1); // Save the first byte for future use.
            header[0] = info->headerByte | AVDTP_PKT_TYPE_START; //modify packet type: single -> start
            header[1] = info->numSendPkts;
            OI_MBUF_PullBytes(header + 2, src, 1);
            headerLen = 3;
        }
        else {
            headerLen = 0;
        }
    }
    else {
        if (1 == info->numSendPkts) {
            OI_DBGPRINT2(("Sending END fragment\n"));
            header[0] = info->headerByte |= AVDTP_PKT_TYPE_END;
        }
        else {
            OI_DBGPRINT2(("Sending CONTINUE fragment\n"));
            header[0] = info->headerByte |= AVDTP_PKT_TYPE_CONTINUE;
        }
        headerLen = 1;
    }

    if (0 == headerLen) { // single packet
        mbuf = OI_MBUF_Alloc(1);
        if (!mbuf) return OI_STATUS_OUT_OF_MEMORY;
#ifndef OI_CODE
        OI_MBUF_PullMBuf(mbuf, info->sendQ, mtu);
#else
#error code not present
#endif /* OI_CODE */

    }
    else {
        mbuf = OI_MBUF_Alloc(2); // 2 = header + data
        if (!mbuf) return OI_STATUS_OUT_OF_MEMORY;
#ifndef OI_CODE
        OI_MBUF_PullMBuf(mbuf, info->sendQ, (OI_UINT16)(mtu - headerLen));
#else
#error code not present
#endif /* OI_CODE */
        OI_MBUF_Prepend(mbuf, header, headerLen, MBUF_COPY);
    }

    OI_DBGPRINT2(("Fragment length = %d\n", OI_MBUF_NumBytes(mbuf)));
    OI_ASSERT(OI_MBUF_NumBytes(mbuf) != 0);
#ifndef OI_CODE
    result = OI_L2CAP_WriteMBUF(MBUFWriteCfm, info->cid, mbuf, NULL);
#else
#error code not present
#endif
    if (!OI_SUCCESS(result)) {
        OI_LOG_ERROR(("L2CAP Write failed!\n"));
        OI_MBUF_Free(mbuf);
    }
    return result;
}

static void CommandTimeoutCB(DISPATCH_ARG *darg)
{
    AVDTP_SIGNAL_INFO *info;
    TO_STRUCT to_info;
    OI_UINT8 data[4];
    OI_UINT16 dataLen;

    to_info = Dispatch_GetArg(darg, TO_STRUCT);
    info = OI_AVDTP_GetSignalInfo(to_info.cid);
    if (NULL != info){
        if (to_info.transaction == info->TxCmdTransaction){
            if (AVDTP_ConnectInProgress() || (info->connCmplt != AVDTP_ConnectCompletes())){
                /* Connection activity took place while command was outstanding.
                 * Reset Timeout, to give it additional time to complete.
                 */
                SetCommandTimeout(to_info.cid, to_info.transaction, to_info.command);
            } else {
                /* TIME-OUT! */
                OI_LOG_ERROR(("Didn't get command response to transaction %d within timeout period", to_info.transaction));
                data[0] = (to_info.transaction << 4) | AVDTP_MSG_TYPE_RSP_REJECT;
                data[1] = to_info.command;
                switch( to_info.command ){
                    case AVDTP_SET_CONFIG:
                    case AVDTP_RECONFIGURE:
                    case AVDTP_START:
                    case AVDTP_SUSPEND:
                        data[2] = 0;
                        data[3] = OI_AVDTP_ERR_NO_RESPONSE;
                        dataLen = 4;
                        break;
                    default:
                        data[2] = OI_AVDTP_ERR_NO_RESPONSE;
                        dataLen = 3;
                        break;
                }
                AVDTPRecv_Complete(to_info.cid, data, dataLen);
            }
        }
    }
}

void AVDTP_CancelTimeout(OI_L2CAP_CID cid,
                         OI_UINT8 transaction)
{
    AVDTP_SIGNAL_INFO *info;

    info = OI_AVDTP_GetSignalInfo(cid);
    if( NULL != info){
        /* Timeout is only running for last issued command */
        if (info->TxCmdTransaction == transaction) {
            OI_Dispatch_CancelFunc(info->dispatchCB);
        }
    }
}

/*
 * Set a timeout to ensure we get a response from the GW device to our
 * command. Timeout is short, but renewable, to account for long delays
 * caused not by the non-response of the remote device, but by connection
 * activity.
 */
#ifdef OI_CODE
#error code not present
#else
#define COMMAND_TIMEOUT OI_SECONDS(3)
#endif /* OI_CODE */
static void SetCommandTimeout(OI_L2CAP_CID cid,
                              OI_UINT8 transaction,
                              OI_UINT8 command)
{
    DISPATCH_ARG       darg;
    AVDTP_SIGNAL_INFO *info;
    TO_STRUCT          to_info;

    to_info.cid = cid;
    to_info.transaction = transaction;
    to_info.command = command;

    info = OI_AVDTP_GetSignalInfo(cid);
    if( NULL != info){
        info->connCmplt = AVDTP_ConnectCompletes();
        Dispatch_SetArg(darg, to_info);
        OI_DBGPRINT2(("Setting command timeout"));
        info->TxCmdTransaction = transaction;
        OI_Dispatch_CancelFunc(info->dispatchCB);
        OI_Dispatch_RegisterTimedFunc(CommandTimeoutCB, &darg, COMMAND_TIMEOUT, &info->dispatchCB);
    }
}


/* SendCmd and SendRsp are identical, except that SendCmd's are timed
 * out on receiving the matching response.
 */
#define AVDTP_SendRsp(cid, data, dataLen) AVDTP_Send(cid, data, dataLen)

/** Send an AVDTP Command or Response on the specified signalling channel.
 *  @param cid     channel ID of signalling channel
 *  @param data    pointer to buffer holding the command to be sent
 *  @param dataLen length of the command
 */

static OI_STATUS AVDTP_Send(OI_L2CAP_CID cid,
                            OI_BYTE     *data,
                            OI_UINT16    dataLen)
{
    AVDTP_SIGNAL_INFO *info;
    OI_STATUS result = OI_OK;
    OI_MBUF *mbuf;

    OI_ASSERT(data);

    OI_DBGPRINT2(("AVDTP_Send\n"));

    info = OI_AVDTP_GetSignalInfo(cid);
    if (!info) {
        OI_LOG_ERROR(("Signalling info not found for CID %d\n", cid));
        return OI_STATUS_INVALID_CID;
    }
#ifdef OI_CODE
#error code not present
#else
    mbuf = OI_MBUF_Wrap(data, dataLen, MBUF_KEEP);
#endif 


    if (!mbuf) {
        OI_LOG_ERROR(("Out of memory!\n"));
        return OI_STATUS_OUT_OF_MEMORY;
    }
    EnqueueSendPkt(info, mbuf);
    if (info->sendQ == mbuf) { // This is the only packet.
        result = StartSend(info);
    }
    return result;
}

static OI_STATUS AVDTP_SendCmd(OI_L2CAP_CID cid,
                               OI_UINT8 transaction,
                               OI_UINT8 cmd,
                               OI_BYTE *data,
                               OI_UINT16 dataLen)
{
    OI_STATUS result;

    result = AVDTP_Send(cid, data, dataLen);

    /* Timeout all successfully sent commands requiring responses from the remote side */
    if( OI_SUCCESS(result) ){
        SetCommandTimeout(cid, transaction, cmd);
    }

    return result;
}

/******************************************************************

  message assembly

 *****************************************************************/

/** This function assembles and then sends a generic AVDTP request
  on the specified signalling channel.
  @param cid          channel identifier of signalling channel
  @param signalID     signal identifier for the request to be sent
  @param transaction  transaction label
  @param seid         stream endpoint identifier to which the request is to be sent
  */
OI_STATUS AVDTP_GenericReq(OI_L2CAP_CID cid,
                           OI_UINT8 signalID,
                           OI_UINT8 transaction,
                           OI_UINT8 seid)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT(signalID <= (BIT6 - 1));
    OI_ASSERT(seid <= (BIT6 - 1));

    pBs = ByteStream_Alloc(4 + AVDTP_CMD_HEADER_LEN);
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, signalID);
    ByteStream_PutUINT8(*pBs, (seid << 2));
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           signalID,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));
#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/** This function assembles and then sends a generic AVDTP response
  on the specified signalling channel.
  @param cid          channel identifier of signalling channel
  @param signalID     signal identifier for the request to be sent
  @param transaction  transaction label
  @param error        error code
  */
OI_STATUS AVDTP_GenericRsp(OI_L2CAP_CID cid,
                           OI_UINT8 signalID,
                           OI_UINT8 transaction,
                           OI_UINT8 error)
{
    OI_BYTE_STREAM *pBs;
    OI_STATUS result;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT(signalID <= (BIT6 - 1));

    pBs = ByteStream_Alloc(4 + AVDTP_CMD_HEADER_LEN); // Allocate enough to hold accept or reject.
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
    }
    ByteStream_PutUINT8(*pBs, signalID);
    if (error) {
        ByteStream_PutUINT8(*pBs, error);
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

OI_STATUS AVDTP_GeneralReject(OI_L2CAP_CID cid,
                              OI_UINT8 transaction)
{
    OI_BYTE_STREAM *pBs;
    OI_STATUS result;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc(2);
    if (NULL == pBs) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
    ByteStream_PutUINT8(*pBs, (OI_UINT8)0xFF); /* The spec says RFA, but the test spec says all 1's. */
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends AVDTP requests to multiple stream endpoints
 * on the specified signalling channel.
 * @param cid          channel identifier of signalling channel
 * @param signalID     signal identifier
 * @param transaction  transaction label
 * @param ids          pointer to an array of stream endpoint IDs
 * @param num          the number of stream endpoint IDs in the array
 */
OI_STATUS AVDTP_MultipleReq(OI_L2CAP_CID cid,
                            OI_UINT8 signalID,
                            OI_UINT8 transaction,
                            OI_UINT8 *ids,
                            OI_UINT8 num)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;
    OI_UINT8 i;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT(signalID <= (BIT6 - 1));

    pBs = ByteStream_Alloc(4 + AVDTP_CMD_HEADER_LEN);
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, signalID);
    for (i = 0; i < num; i++) {
        ByteStream_PutUINT8(*pBs, (ids[i] << 2));
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           signalID,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends an AVDTP response to a command sent
 * to multiple stream endpoints on the specified signalling channel.
 * @param cid          channel identifier of signalling channel
 * @param signalID     signal identifier
 * @param transaction  transaction label
 * @param seid         If an error occurred, this is the stream endpoint identifier
 *                     of the first endpoint to cause an error.
 * @param error        error code
 */
OI_STATUS AVDTP_MultipleRsp(OI_L2CAP_CID cid,
                            OI_UINT8 signalID,
                            OI_UINT8 transaction,
                            OI_UINT8 seid,
                            OI_UINT8 error)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT(signalID <= (BIT6 - 1));

    pBs = ByteStream_Alloc(4 + AVDTP_CMD_HEADER_LEN);
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
    }
    ByteStream_PutUINT8(*pBs, signalID);
    if (error) {
        ByteStream_PutUINT8(*pBs, (seid << 2));
        ByteStream_PutUINT8(*pBs, error);
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends an AVDTP_DISCOVER command to discover
 * all stream endpoints associated with a particular channel identifier.
 * @param cid          channel identifier of signalling channel
 * @param transaction  transaction label
 */
OI_STATUS AVDTPSignal_Discover(OI_L2CAP_CID cid,
                               OI_UINT8 transaction)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc(4 + AVDTP_CMD_HEADER_LEN);
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, AVDTP_DISCOVER);
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           AVDTP_DISCOVER,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends a response to an AVDTP_DISCOVER
 * message with a list of stream endpoints associated with a particular signalling
 * channel.
 *
 * @param cid          channel identifier of signalling channel
 * @param transaction  transaction label
 * @param array        pointer to an array of endpoint records
 * @param arrayLen     the number of records in the array
 * @param error        error code
 */
OI_STATUS AVDTPSignal_DiscoverRsp(OI_L2CAP_CID cid,
                                  OI_UINT8 transaction,
                                  OI_AVDTP_DISCOVER_RSP_INFO *array,
                                  OI_UINT16 arrayLen,
                                  OI_UINT8 error)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

#ifdef OI_TEST
    OI_BD_ADDR addr;
    result = AVDTP_GetAddrFromCID(cid, &addr);
    OI_AVDTP_TestPrint(("AVDT_Discover_Rsp\n\tTransaction=%d\n\tBD_ADDR=%s\n\tErrorcode=0x%.2x\n",
                        transaction,
                        OI_BDAddrText(&addr),
                        error));
    OI_AVDTP_TestPrintSeidData(array, arrayLen);
    OI_AVDTP_TestPrint(("AVDT_Discover_Rsp\n\tRSP=0x0000\n"));
#endif

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc((OI_UINT16)((arrayLen * sizeof(*array)) + AVDTP_CMD_HEADER_LEN)); //a little more than necessary
    if (NULL == pBs) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
    }
    ByteStream_PutUINT8(*pBs, AVDTP_DISCOVER);

    if (error) {
        ByteStream_PutUINT8(*pBs, error);
    }
    else {
        // Marshal the array.
        OI_UINT8 i;

        for (i = 0; i < arrayLen; i++) {
            OI_ASSERT( (array[i].inUse == IN_USE) || (array[i].inUse == NOT_IN_USE));
            OI_ASSERT( (array[i].endpointType == OI_AVDTP_SRC) || (array[i].endpointType == OI_AVDTP_SNK));
            ByteStream_PutUINT8(*pBs, (array[i].seid << 2) | (array[i].inUse << 1));
            ByteStream_PutUINT8(*pBs, (array[i].mediaType << 4) | (array[i].endpointType << 3));
        }
    }

    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/** Returns the size in bytes required to marshal */ 
static OI_UINT16 CapsSize( OI_AVDTP_CAP *caps,
                           OI_UINT8 numCaps)
{
    OI_UINT16 size = 0;
    OI_UINT i;

    for (i = 0; i < numCaps; i++) {
        OI_AVDTP_CAP *cap = &caps[i];

        switch (cap->type) {
            case OI_AVDTP_CAP_MEDIA_TRANSPORT:
                size += 2;
                break;
            case OI_AVDTP_CAP_CODEC:
                size += 4 + cap->data.codecData.dataLen;
                break;
            default:
                size += 2 + cap->data.genericData.dataLen;
                break;
        }
    }
    return size;
}

/** This function marshals capabilities information into a byte stream. */
/** This function must be the complement to ParseCaps() in avdtp.c */
static void MarshalCaps(OI_BYTE_STREAM *pBs,
                        OI_AVDTP_CAP *caps,
                        OI_UINT8 numCaps)
{
    OI_INT i;

    for (i = 0; i < numCaps; i++) {
        OI_AVDTP_CAP *cap = &caps[i];

        // type
        ByteStream_PutUINT8(*pBs, cap->type);

        // length and data
        switch (cap->type) {
            case OI_AVDTP_CAP_MEDIA_TRANSPORT:
                ByteStream_PutUINT8(*pBs, 0);
                break;
            case OI_AVDTP_CAP_CODEC:
                ByteStream_PutUINT8(*pBs, AVDTP_CAP_CODEC_LEN + cap->data.codecData.dataLen);
                ByteStream_PutUINT8(*pBs, (cap->data.codecData.mediaType << 4));
                ByteStream_PutUINT8(*pBs, cap->data.codecData.codecType);
                ByteStream_PutBytes(*pBs, cap->data.codecData.data, cap->data.codecData.dataLen);
                break;
            default:
                ByteStream_PutUINT8(*pBs, cap->data.genericData.dataLen);
                if (cap->data.genericData.dataLen) {
                    ByteStream_PutBytes(*pBs, cap->data.genericData.data, cap->data.genericData.dataLen);
                }
                break;
        }
    }
}

/** This function assembles and then sends a response containing capabilities
  information, in response to either an AVDTP_GET_CAPS or an AVDTP_GET_CONFIG command.
  @param cid          channel identifier of signalling channel
  @param command      either AVDTP_GET_CAPS or AVDTP_GET_CONFIG
  @param transaction  transaction label
  @param caps         pointer to a list of capabilities
  @param numCaps      number of capabilites in the list
  @param error        error code
  */
OI_STATUS AVDTPSignal_GenericCapsRsp(OI_L2CAP_CID cid,
                                     OI_UINT8 command,
                                     OI_UINT8 transaction,
                                     OI_AVDTP_CAP *caps,
                                     OI_UINT8 numCaps,
                                     OI_UINT8 error)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;
    OI_UINT16 capsSize = 0;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT((caps != NULL) || (numCaps == 0));
    OI_ASSERT((AVDTP_GET_CAPS == command) || (AVDTP_GET_CONFIG == command));

    if (caps) {
        capsSize = CapsSize(caps, numCaps);
    }

    pBs = ByteStream_Alloc((OI_UINT16)(capsSize + 3));
    if (NULL == pBs) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
    }
    ByteStream_PutUINT8(*pBs, command);

    if (error) {
        ByteStream_PutUINT8(*pBs, error);
    }
    else {
        MarshalCaps(pBs, caps, numCaps);
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends an AVDTP_SET_CONFIG command.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param remoteSEID    stream endpoint identifier of remote endpoint
 * @param localSEID     stream endpoint identifier of local endpoint
 * @param caps          pointer to a list of capabilities
 * @param numCaps       number of capabilities in the list
 */
OI_STATUS AVDTPSignal_SetConfig(OI_L2CAP_CID cid,
                                OI_UINT8 transaction,
                                OI_UINT8 remoteSEID,
                                OI_UINT8 localSEID,
                                OI_AVDTP_CAP *caps,
                                OI_UINT8 numCaps)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc((OI_UINT16)(CapsSize(caps, numCaps) + 4));
    if (NULL == pBs) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, AVDTP_SET_CONFIG);
    ByteStream_PutUINT8(*pBs, (remoteSEID << 2));
    ByteStream_PutUINT8(*pBs, (localSEID << 2));
    MarshalCaps(pBs, caps, numCaps);
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           AVDTP_SET_CONFIG,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends an AVDTP_AVDTP_RECONFIGURE command.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param seid          stream endpoint identifier
 * @param caps          pointer to a list of capabilities
 * @param numCaps       number of capabilities in the list
 */
OI_STATUS AVDTPSignal_Reconfigure(OI_L2CAP_CID cid,
                                  OI_UINT8 transaction,
                                  OI_UINT8 seid,
                                  OI_AVDTP_CAP *caps,
                                  OI_UINT8 numCaps)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc((OI_UINT16)(CapsSize(caps, numCaps) + 3));
    if (NULL == pBs) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, AVDTP_RECONFIGURE);
    ByteStream_PutUINT8(*pBs, (seid << 2));
    MarshalCaps(pBs, caps, numCaps);
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           AVDTP_RECONFIGURE,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends a generic configuration response,
 * in response to either an AVDTP_SET_CONFIG or an AVDTP_RECONFIGURE command.
 * @param cid          channel identifier of signalling channel.
 * @param command      the command type
 * @param transaction  transaction label
 * @param category     service category
 * @param error        error code
 */
OI_STATUS AVDTPSignal_ConfigRsp(OI_L2CAP_CID cid,
                                OI_UINT8 command,
                                OI_UINT8 transaction,
                                OI_UINT8 category,
                                OI_UINT8 error)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));
    OI_ASSERT((command == AVDTP_SET_CONFIG) || (command == AVDTP_RECONFIGURE));

    pBs = ByteStream_Alloc(4);
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (!error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
        ByteStream_PutUINT8(*pBs, command);
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
        ByteStream_PutUINT8(*pBs, command);
        ByteStream_PutUINT8(*pBs, category);
        ByteStream_PutUINT8(*pBs, error);
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends an AVDTP_SECURITY_CONTROL command on the
 * specified signalling channel.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param seid          stream endpoint identifier
 * @param data          pointer to buffer
 * @param dataLen       length of data in buffer
 */
OI_STATUS AVDTPSignal_SecurityControl(OI_L2CAP_CID cid,
                                      OI_UINT8 transaction,
                                      OI_UINT8 seid,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc((OI_UINT16)(3 + dataLen));
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_COMMAND));
    ByteStream_PutUINT8(*pBs, AVDTP_SECURITY_CONTROL);
    ByteStream_PutUINT8(*pBs, (seid << 2));
    ByteStream_PutBytes(*pBs, data, dataLen);
    ByteStream_Close(*pBs);

    result = AVDTP_SendCmd(cid,
                           transaction,
                           AVDTP_RECONFIGURE,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}

/**
 * This function assembles and then sends a response to an AVDTP_SECURITY_CONTROL command.
 * @param cid           channel identifier of signalling channel
 * @param transaction   transaction label
 * @param data          pointer to buffer
 * @param dataLen       length of data in buffer
 * @param error         error code
 */
OI_STATUS AVDTPSignal_SecurityControlRsp(OI_L2CAP_CID cid,
                                         OI_UINT8 transaction,
                                         OI_BYTE *data,
                                         OI_UINT16 dataLen,
                                         OI_UINT8 error)
{
    OI_STATUS result;
    OI_BYTE_STREAM *pBs;

    OI_ASSERT(transaction <= (BIT4 - 1));

    pBs = ByteStream_Alloc((OI_UINT16)(3 + dataLen));
    if (NULL == pBs) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*pBs, BYTESTREAM_WRITE);
    if (!error) {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_ACCEPT));
        ByteStream_PutUINT8(*pBs, AVDTP_SECURITY_CONTROL);
        ByteStream_PutBytes(*pBs, data, dataLen);
    }
    else {
        ByteStream_PutUINT8(*pBs, ((transaction << 4) | AVDTP_PKT_TYPE_SINGLE | AVDTP_MSG_TYPE_RSP_REJECT));
        ByteStream_PutUINT8(*pBs, AVDTP_SECURITY_CONTROL);
        ByteStream_PutUINT8(*pBs, error);
    }
    ByteStream_Close(*pBs);

    result = AVDTP_SendRsp(cid,
                           ByteStream_GetDataPointer(*pBs),
                           ByteStream_GetSize(*pBs));

#ifdef OI_CODE
#error code not present
#else
    ByteStream_Free(pBs);
    if (!OI_SUCCESS(result)) {
        return result;
    }
#endif

    return OI_OK;
}
#endif /* FEATURE_BT_EXTPF_AV */

