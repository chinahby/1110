#ifdef FEATURE_BT_EXTPF_AV

/**
@file
@internal

This file implements portions of the Audio/Video Distribution Protocol specification.
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_AVDTP

#include <stdlib.h>
#include <stdarg.h>
#include "oi_core_common.h"
#include "oi_securitydb.h"
#include "oi_avdtp.h"
#include "oi_avdtp_signal.h"
#include "oi_avdtp_private.h"
#include "oi_argcheck.h"
#include "oi_bytestream.h"
#include "oi_mbuf.h"
#include "oi_l2cap_mbuf.h"
#include "oi_l2cap_group.h"
#include "oi_testprint.h"
#include "oi_init_flags.h"
#include "oi_config_table.h"

#define AVDTP_MEDIA_HEADER_LEN 12

typedef struct {
    /** The endpoints. */
    AVDTP_ENDPOINT *endpoints;

    /** The application callbacks. */
    const OI_AVDTP_CALLBACKS *callbacks;

    AVDTP_SIGNAL_INFO *signalInfo;

    OI_CONNECT_POLICY connectPolicy;

    /** The parameters to use for transport channel establishment. */
    OI_L2CAP_CONNECT_PARAMS params;

    OI_L2CAP_CID mediaCID;

    OI_L2CAP_CID fecCID;

    OI_UINT8 maxEndpoints;

    /* Used to keep track of seid which is being queried. */
    OI_UINT8 SEID;

    /** The next SEID to be allocated. */
    OI_UINT8 nextSEID;

    OI_UINT8 transaction;

    /* Outstanding Connection request counters */
    OI_UINT8 connReq;

    OI_UINT8 connCmplt;
} AVDTP;

static AVDTP *avdtp = NULL;

/******************************************************************

  Prototypes

*****************************************************************/

static void ResetEndpoint(AVDTP_ENDPOINT *e);
static void FreeSignalInfo(OI_L2CAP_CID cid);

static void RecvDataInd(OI_L2CAP_CID cid,
                        OI_BYTE *data,
                        OI_UINT16 dataLen);

static void DisconnectCB(OI_L2CAP_CID cid,
                         OI_STATUS reason);

static AVDTP_SIGNAL_INFO *AllocSignalInfo(OI_L2CAP_CID cid,
                                          OI_BD_ADDR *addr);

static AVDTP_ENDPOINT *GetEndpointByTransportCID(OI_L2CAP_CID cid);
static AVDTP_ENDPOINT *GetEndpointBySignalCID(OI_L2CAP_CID cid);
static AVDTP_REMOTE_ENDPOINT *GetRemoteEndpointByBdAddr(OI_BD_ADDR *addr);
static AVDTP_ENDPOINT *GetStreamingEndpoint(void);

static void MediaDisconnectCB(OI_L2CAP_CID cid,
                              OI_STATUS reason);

static void MediaConnectCfmAcp(OI_L2CAP_CID cid,
                            OI_UINT16 inMTU,
                            OI_UINT16 outMTU,
                            OI_STATUS result);

static void MediaConnectCfmInt(OI_L2CAP_CID cid,
                            OI_UINT16 inMTU,
                            OI_UINT16 outMTU,
                            OI_STATUS result);

static void MediaRecvDataInd(OI_L2CAP_CID cid,
                             OI_BYTE *data,
                             OI_UINT16 dataLen);

static void MediaRecvBroadcastDataInd(OI_L2CAP_PSM psm,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen);
/******************************************************************

  Macros

*****************************************************************/

#ifdef OI_DEBUG
#error code not present
#else

#define StateString(state) ""

#endif  // OI_DEBUG

#define SetState(endpoint, s) OI_DBGPRINT2(("State transition SEID %d: %s -> %s", (endpoint)->seid, StateString((endpoint)->state), StateString(s))); (endpoint)->state = s;

#ifdef OI_DEBUG
#error code not present
#else

#define MsgTypeText(msgType) ""

#endif

/******************************************************************

  L2CAP Callbacks

*****************************************************************/

//Signal channel callbacks

static void ConnectCfmInt(OI_L2CAP_CID cid,
                          OI_UINT16 inMTU,
                          OI_UINT16 outMTU,
                          OI_STATUS result)
{
    OI_STATUS retVal;
    OI_BD_ADDR addr;
    AVDTP_SIGNAL_INFO *info;

    avdtp->connCmplt++;

    retVal = AVDTP_GetAddrFromCID(cid, &addr);
    if (OI_SUCCESS(retVal)) {
        if (OI_SUCCESS(result)) {
            info = OI_AVDTP_GetSignalInfo(cid);
            OI_ASSERT(info);
#ifndef OI_CODE
			if( NULL == info ) {
				return;
			}
#endif /* OI_CODE */
            info->signalMTU = outMTU;
        } else {
            FreeSignalInfo(cid);
        }
    } else {
        FreeSignalInfo(cid);
        OI_MemSet(&addr, 0x00, sizeof(OI_BD_ADDR));
        result = retVal;
    }
    avdtp->callbacks->connectCfm(&addr, result);
}

static void ConnectCfmAcp(OI_L2CAP_CID cid,
                          OI_UINT16 inMTU,
                          OI_UINT16 outMTU,
                          OI_STATUS result)
{
    OI_STATUS retVal;
    OI_BD_ADDR addr;
    AVDTP_SIGNAL_INFO *info;

    /* Increment both counters to indicate connection activity,
     * but not upset balance between Requests and Completes. */
    avdtp->connCmplt++;
    avdtp->connReq++;

    info = OI_AVDTP_GetSignalInfo(cid);
    OI_ASSERT(info);
#ifndef OI_CODE
	if( NULL == info ) {
		return;
    }
#endif /* OI_CODE */
    info->signalMTU = outMTU;

    OI_DBGPRINT(("ConnectCfmAcp: %!",result));
    retVal = AVDTP_GetAddrFromCID(cid, &addr);
    if (OI_SUCCESS(retVal)) {
        if (!OI_SUCCESS(result)) {
            FreeSignalInfo(cid);
        }
        avdtp->callbacks->connectCfm(&addr, result);
    } else {
        FreeSignalInfo(cid);
    }
}

/**
 * Handle incoming L2CAP connections.  This function must handle both
 * incoming signalling connections and incoming media transport
 * connections.  The only way to differentiate between the two is to
 * check the state of all endpoints with signalling connections to
 * this address.
 */
static void ConnectInd(OI_L2CAP_PSM psm,
                       OI_BD_ADDR *addr,
                       OI_L2CAP_CID cid)
{
    OI_L2CAP_CONNECT_PARAMS *params = NULL;
    OI_L2CAP_CID             signalCID;
    OI_STATUS                result;
    OI_UINT8                 i;


    /* Increment both counters to indicate connection activity,
     * but not upset balance between Requests and Completes. */
    avdtp->connCmplt++;
    avdtp->connReq++;

    if (avdtp->params.inMTU != 0){
        params = &avdtp->params;
    }

    result = AVDTP_GetCIDFromAddr(addr, &signalCID);
    if (!OI_SUCCESS(result)) { //No signalling channel exists
        if (!AllocSignalInfo(cid, addr)) {
            result = OI_L2CAP_Accept(ConnectCfmAcp,
                                     DisconnectCB,
                                     RecvDataInd,
                                     cid,
                                     params,
                                     FALSE,
                                     &avdtp->connectPolicy);
        } else {
            /* Do not accept connections automatically. Now application is
             * responsible using the OI_AVDTP_Accept() call.
             */
            avdtp->callbacks->connectInd(addr);
        }

        /* We have handled this Connection Indication Satisfactorily */
        goto connectIndSuccess;

    } else { //A signalling channel exists, assume this is media channel
        for (i = 0; i < avdtp->maxEndpoints; i++) {
            if ((avdtp->endpoints[i].seid != 0)
                && (avdtp->endpoints[i].remoteEndpoints != NULL)
                && (avdtp->endpoints[i].remoteEndpoints->signalCID == signalCID)) {


                if (avdtp->endpoints[i].mediaCID != 0){
                    OI_LOG_ERROR(("Invalid ConnectInd Received: (mediaCID != 0)"));
                    goto connectIndFail;
                }

                if (avdtp->endpoints[i].state != AVDTP_STATE_OPEN) {
                    OI_LOG_ERROR(("Invalid ConnectInd Received (state != AVDTP_STATE_OPEN)"));
                    goto connectIndFail;
                }

                avdtp->endpoints[i].mediaCID = cid;

                result = OI_L2CAP_Accept(MediaConnectCfmAcp,
                                         MediaDisconnectCB,
                                         MediaRecvDataInd,
                                         cid,
                                         params,
                                         TRUE,
                                         &avdtp->connectPolicy);
                if (!OI_SUCCESS(result)){
                    OI_LOG_ERROR(("OI_L2CAP_Accept failed! cid=%d error = %!", cid, result));
                } else {
                    /* We have handled this Connection Indication */
                    goto connectIndSuccess;
                }

                break;
            } else {
                OI_DBGPRINT2(("seid: %d, mediaCID: %d, state: %d, remoteEndpoint: %x",
                              avdtp->endpoints[i].seid,
                              avdtp->endpoints[i].mediaCID,
                              avdtp->endpoints[i].state,
                              avdtp->endpoints[i].remoteEndpoints));
            }
        }
        /* Signalling channel exists, but there is no signalling endpoint - this does
            not appear to be a real media connect attempt - reject the connect request */
        OI_LOG_ERROR(("Rejecting connect request %!", OI_AVDTP_CONNECTION_SEQ_ERROR));
        result = OI_L2CAP_Accept(ConnectCfmAcp,
                                 DisconnectCB,
                                 RecvDataInd,
                                 cid,
                                 params,
                                 FALSE,
                                 &avdtp->connectPolicy);
        if (!OI_SUCCESS(result)) {
            OI_LOG_ERROR(("OI_L2CAP_Accept (reject) failed %!", result));
        }
    }

connectIndFail:
    /* We have failed to Satisfactorily handle this connect indication */
    OI_LOG_ERROR(("Unknown Connect Indication Received. cid=%d psm=%d addr=%:", cid, psm, addr));

connectIndSuccess:
    return;
}

static void DisconnectCB(OI_L2CAP_CID cid,
                         OI_STATUS reason)
{
    OI_STATUS retVal;
    OI_BD_ADDR addr;
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote, **last;

    if (!OI_INIT_FLAG_VALUE(AVDTP)){
        return;
    }

    endpoint = GetEndpointBySignalCID(cid);
    if (endpoint) {
        last = &endpoint->remoteEndpoints;
        for( remote = endpoint->remoteEndpoints; remote != NULL; remote = remote->nextEndpoint ){
            if (remote->signalCID == cid){
                /* If we are a broadcasting, remove device from group */
                if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
                    OI_DBGPRINT2(("Removing Media Transports for %:", &remote->addr));
                    OI_L2CAP_GroupRemoveMember(avdtp->mediaCID, &remote->addr );
                    OI_L2CAP_GroupRemoveMember(avdtp->fecCID, &remote->addr );
                }
                *last = remote->nextEndpoint;
                OI_Free(remote);
                break; // Break out of for loop
            }
            last = &remote->nextEndpoint;
        }
        if( endpoint->remoteEndpoints == NULL ){
            ResetEndpoint(endpoint);
        }
    }

    retVal = AVDTP_GetAddrFromCID(cid, &addr);
    FreeSignalInfo(cid);
    if (OI_SUCCESS(retVal)) {
        avdtp->callbacks->disconnectCB(&addr, reason);
    }
}

/** Receive a packet from L2CAP.  This function handles reassembly of
    packet fragments.  If a complete packet is received (type SINGLE),
    it is immediately passed up for further processing. */
static void RecvDataInd(OI_L2CAP_CID cid,
                        OI_BYTE *data,
                        OI_UINT16 dataLen)
{
    AVDTP_SIGNAL_INFO *info = NULL;
    OI_UINT8 pktType;

    OI_ASSERT(data);

    OI_DBGPRINT2(("RecvDataInd dataLen=%d", dataLen));
    if (dataLen < 1) {
        OI_LOG_ERROR(("Invalid packet length!"));
        goto bad_rx;
    }

    pktType = (data[0] & AVDTP_PKT_TYPE_MASK);

    if (AVDTP_PKT_TYPE_SINGLE == pktType) { //Complete packet
        OI_DBGPRINT2(("Received single packet"));
        AVDTPRecv_Complete(cid, data, dataLen);
    } else { //Packet fragment
        OI_DBGPRINT2(("Received packet fragment"));
        info = OI_AVDTP_GetSignalInfo(cid);
        if (NULL == info){
            OI_LOG_ERROR(("Packet received on invalid CID"));
            goto bad_rx;
        }
        if (AVDTP_PKT_TYPE_START == pktType) {
            OI_DBGPRINT2(("Received START fragment"));
            if (dataLen < 3) {
                OI_LOG_ERROR(("Invalid packet length!"));
                goto bad_rx;
            }
            if (info->recvData != NULL) {
                OI_LOG_ERROR(("Unexpected START fragment! - discarding previous packet"));
                OI_Free(info->recvData);
            }
            info->numRecvPkts = data[1];
            info->recvData = OI_Malloc(info->numRecvPkts * (dataLen - 1));
            info->recvLen = 0;
            if (NULL == info->recvData) {
                OI_LOG_ERROR(("Could not allocate reassembly buffer!"));
                /* Send rejection. We can't handle data that big */
                AVDTP_GenericRsp(cid, data[2],
                                 (OI_UINT8)((data[0] & AVDTP_TRANSACTION_MASK) >> 4),
                                 OI_AVDTP_ERR_INSUFFICIENT_RESOURCE);
                goto bad_rx;
            }
            info->recvTransaction = data[0] & AVDTP_TRANSACTION_MASK;
            info->recvData[0] = data[0];
            info->recvData[1] = data[2]; //skip numPackets byte
            OI_MemCopy(info->recvData + 2, data + 3, dataLen - 3);
            info->recvLen += (dataLen - 1);
            info->numRecvPkts--;
        } else {
            if (NULL == info->recvData) {
                OI_LOG_ERROR(("Unexpected packet fragment!"));
                goto bad_rx;
            }
            if (dataLen < 1) {
                OI_LOG_ERROR(("Invalid packet length!"));
                goto bad_rx;
            } else { //Valid continue or end packet
                if ( (data[0] & AVDTP_TRANSACTION_MASK) != info->recvTransaction) {
                    OI_LOG_ERROR(("Invalid transaction number in fragment"));
                    goto bad_rx;
                }
                OI_MemCopy(info->recvData + info->recvLen, data + 1, dataLen - 1);
                info->recvLen += (dataLen - 1);
                info->numRecvPkts--;

                if (AVDTP_PKT_TYPE_END == pktType) {
                    OI_DBGPRINT2(("Received END fragment"));
                    if (info->numRecvPkts != 0) {
                        OI_LOG_ERROR(("Incorrect number of packet fragments - dropping packet!"));
                        goto bad_rx;
                    }
                    OI_DBGPRINT2(("Raw packet is: %@", info->recvData, info->recvLen));
                    AVDTPRecv_Complete(cid, info->recvData, info->recvLen);
                    OI_FreeIf(&info->recvData);
                } else {
                    OI_DBGPRINT2(("Received CONTINUE fragment"));
                }
            }
        }
    }
    return;

bad_rx:
    if ( NULL != info ){
        OI_FreeIf(&info->recvData);
    }
    return;
}

//Media transport channel callbacks  - P-to-P establishment only (single remote endpoint)

static void MediaConnectCfmInt(OI_L2CAP_CID cid,
                               OI_UINT16 inMTU,
                               OI_UINT16 outMTU,
                               OI_STATUS result)
{
    OI_UINT8 error;
    AVDTP_ENDPOINT *endpoint;

    OI_DBGPRINT2(("Transport channel connected (CID %d)", cid));

    endpoint = GetEndpointByTransportCID(cid);
    if (NULL == endpoint) {
        OI_LOG_ERROR(("Endpoint not found"));
        return;
    }

    if (OI_SUCCESS(result)) {
        error = OI_AVDTP_SUCCESS;
    } else {
        error = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
    }

    if ( endpoint->remoteEndpoints ){
        // avdtp->callbacks->openCfm(endpoint->remoteEndpoints->transaction, endpoint->seid, (OI_UINT16)(outMTU - AVDTP_MEDIA_HEADER_LEN), error);
        avdtp->callbacks->openCfm(endpoint->remoteEndpoints->transaction,
                                 endpoint->remoteEndpoints->signalCID,
                                 (OI_UINT16)(outMTU - AVDTP_MEDIA_HEADER_LEN),
                                 error);
    } else {
        OI_LOG_ERROR(("Remote endpoint not found"));
    }
}

static void MediaConnectCfmAcp(OI_L2CAP_CID cid,
                               OI_UINT16 inMTU,
                               OI_UINT16 outMTU,
                               OI_STATUS result)
{
    AVDTP_ENDPOINT *endpoint;

    OI_DBGPRINT2(("Transport channel connected (CID %d)", cid));

    endpoint = GetEndpointByTransportCID(cid);
    if (NULL == endpoint) {
        OI_LOG_ERROR(("Endpoint not found"));
        return;
    }

    avdtp->callbacks->openInd(endpoint->remoteEndpoints->signalCID, (OI_UINT16)(outMTU - AVDTP_MEDIA_HEADER_LEN));
}

static void MediaDisconnectCB(OI_L2CAP_CID cid,
                              OI_STATUS reason)
{
    AVDTP_ENDPOINT *endpoint;
    OI_UINT8 state;

    OI_DBGPRINT2(("Transport channel disconnected (CID %d)", cid));

    endpoint = GetEndpointByTransportCID(cid);
    if (endpoint && (endpoint->state != AVDTP_STATE_IDLE)) {
        OI_UINT16 handle = 0;
        OI_UINT8  transaction = 0;

        state = endpoint->state;
        endpoint->mediaCID = 0;

        if ( endpoint->remoteEndpoints ){
            handle = endpoint->remoteEndpoints->signalCID,
            transaction = endpoint->remoteEndpoints->transaction;
        }

        ResetEndpoint(endpoint);

        if (handle){
            if (AVDTP_STATE_CLOSING == state) {
                avdtp->callbacks->closeCfm(transaction, handle, OI_AVDTP_SUCCESS);
            }
            else if (AVDTP_STATE_ABORTING == state) {
                avdtp->callbacks->abortCfm(transaction, handle, OI_AVDTP_SUCCESS);
            } else {
                /* If media Transport drops for unknown reason, treat as an Abort so
                 * that the application stays in sync with endpoint state. The
                 * remote application is likely misbehaving, or an abort response
                 * was dropped.
                 */
                OI_LOG_ERROR(("Unexpected Transport channel disconnect: %d", handle));
                avdtp->callbacks->abortInd(0, handle);
            }
        }
    }
}


static void MediaRecvDataInd(OI_L2CAP_CID cid,
                             OI_BYTE *data,
                             OI_UINT16 dataLen)
{
    AVDTP_ENDPOINT *endpoint;
    OI_BYTE_STREAM bs;
    OI_UINT16 seqNum = 0;
    OI_UINT32 timestamp = 0;
    OI_UINT8 byte1 = 0;
    OI_UINT8 byte2 = 0;
    OI_UINT16 reliability = OI_AVDTP_RECV_NO_ERROR; //no errors
    OI_UINT16 csrcCount = 0;
    OI_UINT16 totalHeaderLen = 0;

    if (dataLen < AVDTP_MEDIA_HEADER_LEN) {
        OI_DBGPRINT(("Runt packet received!"));
    }

    if ( cid ){
        /* Recover endpoint for this CID */
        endpoint = GetEndpointByTransportCID(cid);
    } else {
        /* If CID is zero, this is a Broadcast data packet */
        /* Pass up on any streaming endpoint found */
        endpoint = GetStreamingEndpoint();
    }
    if (endpoint != NULL) {
        if (AVDTP_STATE_STREAMING == endpoint->state) {
            ByteStream_Init(bs, data, dataLen);
            ByteStream_Open(bs, BYTESTREAM_READ);
            ByteStream_GetUINT8_Checked(bs, byte1);
            ByteStream_GetUINT8_Checked(bs, byte2);
            ByteStream_GetUINT16_Checked(bs, seqNum, OI_BIG_ENDIAN_BYTE_ORDER);
            ByteStream_GetUINT32_Checked(bs, timestamp, OI_BIG_ENDIAN_BYTE_ORDER);
            totalHeaderLen = AVDTP_MEDIA_HEADER_LEN;

            csrcCount = byte1 & 0xf;
            ByteStream_Skip(bs, csrcCount);
            totalHeaderLen += csrcCount;

            if (ByteStream_Error(bs)) {
                OI_LOG_ERROR(("Error in received data frame"));
                reliability = OI_AVDTP_RECV_FRAME_LOST;
            }
        } else {
            OI_DBGPRINT2(("Invalid state to receive packet"));
            reliability = OI_AVDTP_RECV_FRAME_LOST;
        }

        if (endpoint->remoteEndpoints != NULL){
            if (OI_AVDTP_RECV_NO_ERROR == reliability) {
                avdtp->callbacks->recvDataInd(endpoint->remoteEndpoints->signalCID,
                                             seqNum,
                                             timestamp,
                                             data + totalHeaderLen,
                                             (OI_UINT16) (dataLen - totalHeaderLen),
                                             (OI_UINT8)(byte2 & 0x7F),
                                             (OI_UINT16)((byte2 & BIT7) >> 7),
                                             reliability);
            } else {
                avdtp->callbacks->recvDataInd(endpoint->remoteEndpoints->signalCID,
                                             0,
                                             0,
                                             NULL,
                                             (OI_UINT16)0,
                                             (OI_UINT8)0,
                                             (OI_UINT16)0,
                                             reliability); /*reliability - media frame/packet lost */
            }
        }
    }
}
static void MediaRecvBroadcastDataInd(OI_L2CAP_PSM psm,
                                      OI_BYTE *data,
                                      OI_UINT16 dataLen)
{
    AVDTP_ENDPOINT *endpoint;
    OI_UINT8 seqNum;
    OI_UINT8 offset;
    OI_UINT8 fractional;

    OI_DBGPRINT(("Broadcast packet received!"));

    OI_ASSERT( dataLen > 3 );
    seqNum = *data++;
    offset = *data++;
    fractional = *data++;
    dataLen -= 3;

    /* Don't assert if FEC packet */
    OI_ASSERT( (OI_PSM_AVDTP != psm) || (dataLen > offset) );
    OI_ASSERT( (OI_PSM_AVDTP != psm) || (dataLen > fractional) );

    endpoint = GetStreamingEndpoint();

    if( (endpoint != NULL) && (avdtp->callbacks->recvBroadcastDataInd != NULL) ){

        if( (endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO)  &&
            endpoint->remoteEndpoints ){
            avdtp->callbacks->recvBroadcastDataInd(endpoint->remoteEndpoints->signalCID,
                                                  seqNum,
                                                  offset,
                                                  fractional,
                                                  ( (psm == OI_PSM_AVDTP) ? FALSE : TRUE ),
                                                  data,
                                                  dataLen);
        }
    }
    // MediaRecvDataInd(0, data, dataLen);
}


static void MediaWriteMBUFCfm(OI_L2CAP_CID cid,
                              OI_MBUF *mbuf,
                              OI_STATUS result)
{
    AVDTP_ENDPOINT *endpoint;

    endpoint = GetEndpointByTransportCID(cid);
    if (endpoint != NULL) {
        if (endpoint->remoteEndpoints != NULL) {
#ifdef OI_CODE
#error code not present
#else /* OI_CODE */
            /* Return pointer to the dsm, and use the MBUF interface fcns! */
            avdtp->callbacks->sendCfm(endpoint->remoteEndpoints->signalCID,
                                     (OI_BYTE *)mbuf->dsm_ptr,
                                     OI_MBUF_NumBytes(mbuf),
                                     result);
#endif /* OI_CODE */
        }
    }
    OI_MBUF_Free(mbuf);
}


/******************************************************************

  Internal Functions

*****************************************************************/
void OI_AVDTP_FreeCaps(OI_AVDTP_CAP **caps,
                       OI_UINT8       numCaps)
{
    OI_UINT8 i;
    OI_AVDTP_CAP *cap;

    if (NULL != *caps){
        cap = *caps;

        for (i = 0; i < numCaps; i++) {
            switch(cap[i].type) {
                case OI_AVDTP_CAP_MEDIA_TRANSPORT:
                    /* No Variable Part */
                    break;

                case OI_AVDTP_CAP_CODEC:
                    /* Free variable part of Codec Cap */
                    OI_FreeIf(&cap[i].data.codecData.data);
                    break;

                default:
                    /* Free variable part of Generic Cap */
                    OI_FreeIf(&cap[i].data.genericData.data);
                    break;
            }
        }

        /* Free memory allocated to main array */
        OI_FreeIf(caps);
    }
}

OI_STATUS OI_AVDTP_CopyCaps(const OI_AVDTP_CAP  *caps,
                            OI_UINT8       numCaps,
                            OI_AVDTP_CAP **newCaps,
                            OI_UINT8      *newNumCaps)
{
    OI_UINT8 i;

    *newNumCaps = 0;
    *newCaps = OI_Malloc(numCaps * sizeof(OI_AVDTP_CAP));
    if (NULL == *newCaps){
        goto bail_copycaps;
    }

    for (i = 0; (i < numCaps); i++) {
        const OI_AVDTP_CAP *cap = &((caps)[i]);
        OI_AVDTP_CAP *newCap = &((*newCaps)[i]);

        *newNumCaps += 1;
        newCap->type = cap->type;

        switch(cap->type) {
            case OI_AVDTP_CAP_MEDIA_TRANSPORT:
                break;

            case OI_AVDTP_CAP_CODEC:
                newCap->data.codecData.dataLen = cap->data.codecData.dataLen;
                newCap->data.codecData.mediaType = cap->data.codecData.mediaType;
                newCap->data.codecData.codecType = cap->data.codecData.codecType;
                if (cap->data.codecData.dataLen) {
                    void *data = OI_Malloc(cap->data.codecData.dataLen);

                    newCap->data.codecData.data = data;
                    if (NULL != data){
                        OI_MemCopy(data, cap->data.codecData.data,
                                   cap->data.codecData.dataLen);
                    } else {
                        goto bail_copycaps;
                    }
                } else {
                    newCap->data.codecData.data = NULL;
                }

                break;

            default:
                newCap->data.genericData.dataLen = cap->data.genericData.dataLen;
                if (cap->data.genericData.dataLen) {
                    void *data = OI_Malloc(cap->data.genericData.dataLen);

                    newCap->data.genericData.data = data;
                    if (NULL != data){
                        OI_MemCopy(data, cap->data.genericData.data,
                                   cap->data.genericData.dataLen);
                    } else {
                        goto bail_copycaps;
                    }
                } else {
                    newCap->data.genericData.data = NULL;
                }
                break;
        }
    }

    return OI_OK;

bail_copycaps:
    OI_AVDTP_FreeCaps(newCaps,*newNumCaps);
    return OI_STATUS_OUT_OF_MEMORY;
}

static void ResetEndpoint(AVDTP_ENDPOINT *endpoint)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    void *ptr;

#ifndef OI_CODE
    if(endpoint == NULL)
    {
      OI_LOG_ERROR(("NULL pointer endpoint"));
      return ;
    }
#endif /* OI_CODE */
    endpoint->inUse = 0;
    remote = endpoint->remoteEndpoints;
    endpoint->remoteEndpoints = NULL;

    /* Only disconnect the media CID */
    while ( remote ){
        if (endpoint->mediaCID != 0) {
            if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
                OI_L2CAP_GroupRemoveMember(avdtp->mediaCID, &remote->addr );
                OI_L2CAP_GroupRemoveMember(avdtp->fecCID, &remote->addr );
            }
        }
        ptr = remote;
        remote = remote->nextEndpoint;
        OI_Free(ptr);
    }

    OI_AVDTP_FreeCaps(&endpoint->configCaps, endpoint->numConfigCaps);
    OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
#ifndef OI_CODE
    endpoint->numConfigCaps = 0;
    endpoint->newNumCaps = 0;
#endif /* OI_CODE */

    /* Do cleanup on Media connection */
    if (endpoint->mediaCID != 0) {
        if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
            OI_L2CAP_GroupClose(endpoint->mediaCID);
            OI_L2CAP_GroupClose(avdtp->fecCID);
        } else {
            OI_L2CAP_Disconnect(endpoint->mediaCID);
        }
        endpoint->mediaCID = 0;
    }
    OI_L2CAP_DisableCLT(OI_PSM_AVDTP);
    OI_L2CAP_DisableCLT(OI_PSM_AVDTP | 0x80);
    SetState(endpoint, AVDTP_STATE_IDLE);
}

static AVDTP_ENDPOINT *GetEndpoint(OI_UINT8 seid)
{
    OI_UINT8 i;

    if (0 == seid) return NULL;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if (avdtp->endpoints[i].seid == seid) {
            return &avdtp->endpoints[i];
        }
    }
    return NULL;
}

static AVDTP_REMOTE_ENDPOINT *GetRemoteEndpoint(OI_L2CAP_CID cid)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 i;

    if (0 == cid) return NULL;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ( avdtp->endpoints[i].seid != 0 ){
            for (remote = avdtp->endpoints[i].remoteEndpoints; remote != NULL; remote = remote->nextEndpoint){
                if ( remote->signalCID == cid ){
                    return remote;
                }
            }
        }
    }
    return NULL;
}

static AVDTP_ENDPOINT *GetEndpointByTransportCID(OI_L2CAP_CID cid)
{

    OI_INT8 i;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ((avdtp->endpoints[i].seid != 0) && (avdtp->endpoints[i].mediaCID == cid)) {
            return &avdtp->endpoints[i];
        }
    }
    return NULL;
}

static AVDTP_ENDPOINT *GetStreamingEndpoint(void)
{

    OI_INT8 i;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ((avdtp->endpoints[i].seid != 0) && (avdtp->endpoints[i].state == AVDTP_STATE_STREAMING)) {
            return &avdtp->endpoints[i];
        }
    }
    return NULL;
}

static AVDTP_ENDPOINT *GetEndpointBySignalCID(OI_L2CAP_CID cid)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_INT8 i;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if (avdtp->endpoints[i].seid != 0){
            for (remote = avdtp->endpoints[i].remoteEndpoints; NULL != remote; remote = remote->nextEndpoint){
                if (remote->signalCID == cid) {
                    return &avdtp->endpoints[i];
                }
            }

        }
    }
    return NULL;
}

static OI_UINT16 GetHandleByBdAddr(OI_BD_ADDR *addr)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_INT8 i;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ( avdtp->endpoints[i].seid != 0 ){
            for (remote = avdtp->endpoints[i].remoteEndpoints; NULL != remote; remote = remote->nextEndpoint){
                if ( 0 == OI_MemCmp(&remote->addr, addr, 6) ){
                    return remote->signalCID;
                }
            }
        }
    }
    return 0;
}


static AVDTP_REMOTE_ENDPOINT *GetRemoteEndpointByBdAddr(OI_BD_ADDR *addr)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_INT8 i;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ( avdtp->endpoints[i].seid != 0 ){
            for (remote = avdtp->endpoints[i].remoteEndpoints; NULL != remote; remote = remote->nextEndpoint){
                if ( 0 == OI_MemCmp(&remote->addr, addr, 6) ){
                    return remote;
                }
            }
        }
    }
    return NULL;
}

static AVDTP_ENDPOINT *AllocEndpoint(OI_UINT8 seid)
{
    OI_UINT8 i;
    AVDTP_ENDPOINT *endpoint;

    /* check for duplicate seids */
    if (GetEndpoint(seid)) return NULL;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        endpoint = &avdtp->endpoints[i];
        if (endpoint->seid == 0) {
            OI_ASSERT(endpoint->configCaps == NULL);  /* Probable memory leak if this assert fails  */
            OI_ASSERT(endpoint->newConfigCaps == NULL);  /* Probable memory leak if this assert fails */
            endpoint->seid = seid;
            endpoint->remoteEndpoints = NULL;
            endpoint->mediaCID = 0;
            endpoint->inUse = 0;
            endpoint->caps = NULL;
            endpoint->numCaps = 0;
            endpoint->numConfigCaps = 0;
            endpoint->newNumCaps = 0;

            return endpoint;
        }
    }
    return NULL;
}

static void FreeRemoteEndpoint(OI_L2CAP_CID cid)
{
    AVDTP_ENDPOINT *endpoint = GetEndpointBySignalCID(cid);
    AVDTP_REMOTE_ENDPOINT *target = GetRemoteEndpoint(cid);
    AVDTP_REMOTE_ENDPOINT *remote;

    if (NULL != target){
        if(NULL != endpoint){
            if (target == endpoint->remoteEndpoints){
                endpoint->remoteEndpoints = target->nextEndpoint;
            } else {
                for(remote = endpoint->remoteEndpoints; NULL != remote; remote = remote->nextEndpoint){
                    if (target == remote->nextEndpoint){
                        remote->nextEndpoint = target->nextEndpoint;
                        break;
                    }
                }
            }
        }
        OI_Free(target);
    }
}
static void FreeEndpoint(AVDTP_ENDPOINT *endpoint)
{
    OI_ASSERT(endpoint);

    ResetEndpoint(endpoint);
    endpoint->seid = 0;
}

OI_UINT8 AVDTP_ConnectCompletes( void )
{
    if (NULL == avdtp){
        return 0;
    } else {
        return avdtp->connCmplt;
    }
}

OI_BOOL AVDTP_ConnectInProgress( void )
{
    if (NULL == avdtp){
        return FALSE;
    } else {
        return ((avdtp->connReq == avdtp->connCmplt) ? FALSE : TRUE);
    }
}

OI_STATUS AVDTP_GetAddrFromCID(OI_L2CAP_CID cid,
                               OI_BD_ADDR *addr)
{
    AVDTP_SIGNAL_INFO *this_signal;

    OI_CHECK_INIT(AVDTP);

    this_signal = avdtp->signalInfo;
    while (NULL != this_signal){
        if (this_signal->cid == cid) {
            *addr = this_signal->addr;
            return OI_OK;
        } else {
            this_signal = this_signal->next;
        }
    }

    return OI_STATUS_NOT_FOUND;
}

OI_STATUS AVDTP_GetCIDFromAddr(OI_BD_ADDR *addr,
                               OI_L2CAP_CID *cid)
{
    AVDTP_SIGNAL_INFO *this_signal;

    OI_CHECK_INIT(AVDTP);

    this_signal = avdtp->signalInfo;
    while (NULL != this_signal){
        if (SAME_BD_ADDR(&this_signal->addr, addr)) {
            *cid = this_signal->cid;
            return OI_OK;
        } else {
            this_signal = this_signal->next;
        }
    }

    return OI_STATUS_NOT_FOUND;
}


static AVDTP_SIGNAL_INFO *AllocSignalInfo(OI_L2CAP_CID cid,
                                          OI_BD_ADDR *addr)
{
    AVDTP_SIGNAL_INFO *new_signal;

    new_signal = OI_Calloc(sizeof(AVDTP_SIGNAL_INFO));
    if (NULL != new_signal){
        new_signal->next = avdtp->signalInfo;
        avdtp->signalInfo = new_signal;
        new_signal->addr = *addr;
        new_signal->cid = cid;
    }

    return new_signal;
}

AVDTP_SIGNAL_INFO *OI_AVDTP_GetSignalInfo(OI_L2CAP_CID cid)
{
    AVDTP_SIGNAL_INFO *this_signal;

    OI_ASSERT(OI_INIT_FLAG_VALUE(AVDTP));
#ifndef OI_CODE
    if(NULL == avdtp)
    {
      OI_LOG_ERROR(("NULL AVDTP Object"));
      return NULL;
    }
#endif
    this_signal = avdtp->signalInfo;
    while (NULL != this_signal){
        if (cid == this_signal->cid) {
            return this_signal;
        } else {
            this_signal = this_signal->next;
        }
    }

    return NULL;
}

static void FreeSignalInfo(OI_L2CAP_CID cid)
{
    AVDTP_SIGNAL_INFO  *this_signal = avdtp->signalInfo;
    AVDTP_SIGNAL_INFO **last_signal = &avdtp->signalInfo;
    OI_MBUF            *mbuf;

    while (NULL != this_signal){
        if (cid == this_signal->cid) {
            OI_FreeIf(&this_signal->recvData);
            while( NULL != this_signal->sendQ ){
                mbuf = this_signal->sendQ;
                this_signal->sendQ = mbuf->next;
                OI_MBUF_Free(mbuf);
            }
            *last_signal = this_signal->next;
            OI_FreeIf(&this_signal);
        } else {
            last_signal = &this_signal->next;
            this_signal = this_signal->next;
        }
    }
}

static OI_UINT8 NewTransaction(void)
{
    avdtp->transaction++;
    if (avdtp->transaction >= BIT4)
        avdtp->transaction = 0;

    return avdtp->transaction;
}

#define CheckState(endpoint, s) ((endpoint)->state == (s))

#if 0 /* Defined but never used */
/**
 * Check the state of the specified list of endpoint handles.
 *
 * @param handles  The list of handles
 * @param numHandles  The number of handles in the list.
 * @param state  The state to test for
 *
 * @param return  The value of the first id that failed the test.  O if all are successful.
 */
static OI_UINT8 CheckStates(OI_UINT8 *handles,
                           OI_UINT8 numHandles,
                           OI_UINT8 state)
{
    OI_UINT8 i;
    AVDTP_ENDPOINT *endpoint;

    for (i = 0; i < numHandles; i++) {
        endpoint = GetEndpointBySignalCID(handles[i]);
        if (NULL == endpoint) return handles[i];
        if (!CheckState(endpoint, state)) return handles[i];
    }
    return 0;
}
#endif

/**
 * Set the state of the specified list of endpoint handles.
 *
 * @param handles     the list of handles
 * @param numHandles  the number of handles in the list
 * @param state   the state to set
 *
 * @return        the value of the first ID that failed the test; if all are successful
 */

static OI_UINT16 SetStates(OI_UINT16 *handles,
                          OI_UINT8 numHandles,
                          OI_UINT8 state)
{
    OI_UINT8 i;
    AVDTP_ENDPOINT *endpoint;

    for (i = 0; i < numHandles; i++) {
        endpoint = GetEndpointBySignalCID(handles[i]);
        if (NULL == endpoint) {
            return handles[i];
        }
        SetState(endpoint, state);
    }
    return 0;
}


static void MediaTransportRemoveCB(OI_BD_ADDR *addr)
{
    /* Does nothing/Never gets called */
}

static void MediaTransportMemberAddCfm( OI_L2CAP_CID cid,
                                        OI_BD_ADDR *addr,
                                        OI_STATUS result)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT16 handle;
    OI_UINT8 error;

    remote = GetRemoteEndpointByBdAddr(addr);
    if (NULL == remote) {
        OI_LOG_ERROR(("Remote endpoint not found"));
        return;
    }

    handle = GetHandleByBdAddr(addr);

    OI_DBGPRINT2(("Transport channel connected (CID %d)", remote->remoteSEID));

    if (OI_SUCCESS(result)) {
        error = OI_AVDTP_SUCCESS;
    } else {
        error = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
    }

    if ( avdtp->mediaCID == cid ){
        /* No MTU actually negotiated - Limit MTU to size of one DH5 packet */
        avdtp->callbacks->openCfm(remote->transaction, handle, 332, error);
    }
}


static OI_STATUS CreateMediaTransport(OI_UINT16 handle)
{
    OI_STATUS result;
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_L2CAP_CONNECT_PARAMS *params = NULL;

    endpoint = GetEndpointBySignalCID(handle);

    if ( endpoint == NULL ){
        return OI_STATUS_NOT_FOUND;
    }

    if ( endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO ){ // If Broadcast

        /* More than one endpoint, we are broadcasting */
        if (avdtp->mediaCID == 0){
            result = OI_L2CAP_GroupCreate( OI_PSM_AVDTP,
                                           &endpoint->mediaCID,
                                           MediaTransportRemoveCB );

            if ( !OI_SUCCESS(result) ){
                OI_LOG_ERROR(("failed media Group Create %!", result));
                return result;
            }

            result = OI_L2CAP_GroupCreate( OI_PSM_AVDTP | 0x80,
                                           &avdtp->fecCID,
                                           MediaTransportRemoveCB );

            if ( !OI_SUCCESS(result) ){
                OI_LOG_ERROR(("failed fec Group Create %!", result));
                return result;
            }
        }

        avdtp->mediaCID = endpoint->mediaCID;
        remote = GetRemoteEndpoint(handle);

        OI_DBGPRINT2(("Adding Media Transport for %:", &remote->addr));

        result = OI_L2CAP_GroupAddMember(MediaTransportMemberAddCfm,
                                         endpoint->mediaCID,
                                         &remote->addr);
        if ( !OI_SUCCESS(result) ){
            OI_LOG_ERROR(("failed mediaCID add %!", result));
            return result;
        }

        result = OI_L2CAP_GroupAddMember(MediaTransportMemberAddCfm,
                                         avdtp->fecCID,
                                         &remote->addr);
        if ( !OI_SUCCESS(result) ){
            OI_LOG_ERROR(("failed fecCID add %!", result));
        }
        return result;
    } else {

        /* Only one endpoint, establish point-to-point connection */
        if ( endpoint->remoteEndpoints != NULL ){
            if (avdtp->params.inMTU != 0){
                params = &avdtp->params;
            }

            return OI_L2CAP_Connect(MediaConnectCfmInt,
                                    MediaDisconnectCB,
                                    MediaRecvDataInd,
                                    OI_PSM_AVDTP,
                                    &endpoint->remoteEndpoints->addr,
                                    params,
                                    &endpoint->mediaCID,
                                    &avdtp->connectPolicy);
        } else {
            return OI_STATUS_NOT_FOUND;
        }

    }
}

static OI_STATUS internalInit( void )
{
    OI_CONFIG_AVDTP const *configParams;

    if (OI_INIT_FLAG_VALUE(AVDTP)){
        return OI_STATUS_ALREADY_REGISTERED;
    }

    configParams = OI_CONFIG_TABLE_GET(AVDTP);
    ARGCHECK(NULL != configParams);

    /* Allocate and Zero local variable storage */
    avdtp = OI_Calloc(sizeof(AVDTP));
    if (avdtp == NULL){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    /* Allocate space for local Endpoints */
    avdtp->endpoints = OI_Calloc(configParams->maxEndpoints * sizeof(AVDTP_ENDPOINT));
    if (avdtp->endpoints == NULL){
        OI_FreeIf(&avdtp);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    /* Determine Security Policy */
    avdtp->connectPolicy.serviceUuid32 = OI_UUID_AVDTP;
    avdtp->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(OI_UUID_AVDTP);;

    /* Initialize non-Zero variables */
    avdtp->maxEndpoints = configParams->maxEndpoints;
    avdtp->nextSEID = 1;

    OI_INIT_FLAG_PUT_FLAG(TRUE, AVDTP);
    return OI_OK;
}

static OI_STATUS internalTerminate( void )
{
    OI_CHECK_INIT(AVDTP);

    /* Free memory owned by AVDTP */
    OI_FreeIf(&avdtp->endpoints);
    OI_FreeIf(&avdtp);

    OI_INIT_FLAG_PUT_FLAG(FALSE, AVDTP);

    return OI_OK;
}

/******************************************************************

  External Functions

*****************************************************************/
#ifdef OI_DEPRECATED
/**
 * @deprecated Use oi_bt_profile_config_default.c.
 */
OI_STATUS OI_AVDTP_Init(const OI_CONFIG_AVDTP *configParams)
{
    OI_CONFIG_TABLE_PUT(configParams,AVDTP);
    return OI_OK;
}
#endif

OI_STATUS OI_AVDTP_Register(const OI_AVDTP_CALLBACKS *c,
                            const OI_L2CAP_CONNECT_PARAMS *params)
{
    OI_STATUS result = OI_OK;

    OI_ENSURE_INIT(AVDTP);

    ARGCHECK(c);
    ARGCHECK(c->connectInd);
    ARGCHECK(c->connectCfm);
    ARGCHECK(c->disconnectCB);
    ARGCHECK(c->discoverCfm);
    ARGCHECK(c->getCapsCfm);
    ARGCHECK(c->setConfigInd);
    ARGCHECK(c->setConfigCfm);
    ARGCHECK(c->getConfigCfm);
    ARGCHECK(c->openCfm);
    ARGCHECK(c->openInd);
    ARGCHECK(c->closeInd);
    ARGCHECK(c->closeCfm);
    ARGCHECK(c->startInd);
    ARGCHECK(c->startCfm);
    ARGCHECK(c->abortInd);
    ARGCHECK(c->abortCfm);
    ARGCHECK(c->recvDataInd);
    ARGCHECK(c->sendCfm);

    avdtp->callbacks = c;
    if (params) {
        if (params->inMTU == 0) {
            result = OI_STATUS_INVALID_PARAMETERS;
        } else {
            avdtp->params = *params;
        }
    }

    if (OI_SUCCESS(result)) {
#ifdef OI_CODE
#error code not present
#else
        result = OI_L2CAP_Register(OI_PSM_AVDTP, ConnectInd, params);
#endif
    }

    if (!OI_SUCCESS(result)) {
        internalTerminate();
    }

    return result;
}

OI_STATUS OI_AVDTP_Deregister( void )
{
    OI_STATUS result, term_result;
    OI_UINT8 i;

    OI_CHECK_INIT(AVDTP);

    /* Verify that all connections have been closed */
    if ( NULL != avdtp->signalInfo) {
        return OI_STATUS_STILL_CONNECTED;
    }

    /* Verify that all endpoints have been deregistered */
    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ( 0 != avdtp->endpoints[i].seid) {
            return OI_STATUS_STILL_REGISTERED;
        }
    }

    /* Make sure both these get done, but return either error if available */
    result = OI_L2CAP_Deregister(OI_PSM_AVDTP);
#ifdef OI_CODE
#error code not present
#endif
    if ( OI_SUCCESS(result) ){
#ifndef OI_CODE
        /* Terminate (i.e free the AVDTP object) only if the 
           De-register is successful */
        term_result = internalTerminate();
#endif
        return term_result;
    } else {
        return result;
    }
}

OI_STATUS OI_AVDTP_RegisterEndpoint(OI_UINT8 mediaType,
                                    OI_UINT8 endpointType,
                                    const OI_AVDTP_CAP *caps,
                                    OI_UINT8 numCaps,
                                    OI_UINT8 *seid)
{
    AVDTP_ENDPOINT *endpoint;
    OI_UINT16       count = 0;

    OI_CHECK_INIT(AVDTP);
    ARGCHECK(seid);
    ARGCHECK(NULL != caps);
    ARGCHECK(numCaps != 0);

    if ( (0 == avdtp->nextSEID) || (0x3F == avdtp->nextSEID) ){
        avdtp->nextSEID = 1; /* Don't use restricted values */
    }

    do {
        endpoint = AllocEndpoint(avdtp->nextSEID);
        if (endpoint != NULL) {
            endpoint->mediaType = mediaType;
            endpoint->endpointType = endpointType;
            endpoint->caps = caps;
            endpoint->numCaps = numCaps;
            endpoint->state = AVDTP_STATE_IDLE;
            endpoint->mediaCID = 0;
            *seid = avdtp->nextSEID;
            return OI_OK;
        }
        avdtp->nextSEID++;
        count++;
    } while (count < 255);
    return OI_STATUS_NO_RESOURCES;
}


/**
 * Deregister the specified stream endpoint.
 *
 * @param  seid  The identifier to deregister.
 */
OI_STATUS OI_AVDTP_DeregisterEndpoint(OI_UINT8 seid)
{
    AVDTP_ENDPOINT *endpoint;

    OI_CHECK_INIT(AVDTP);

    endpoint = GetEndpoint(seid);
    if (NULL == endpoint) {
        return OI_STATUS_NOT_FOUND;
    }

    /* Reseting endpoint will close Media Transport connection, if it exists */
    ResetEndpoint(endpoint);

    /* Frees all dynamic memory assosiated with endpoint, and marks as unused */
    FreeEndpoint(endpoint);
    return OI_OK;
}

#ifdef OI_DEPRECATED
/**
 * @deprecated - Just register OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO as the endpoint mediaType
 */
OI_STATUS OI_AVDTP_SetBroadcast(OI_UINT8                    seid,
                                OI_AVDTP_RECV_BROADCAST_IND RecvDataInd)
{
    AVDTP_ENDPOINT *endpoint;

    endpoint = GetEndpoint(seid);
    if (NULL == endpoint) {
        return OI_STATUS_NOT_FOUND;
    }

    endpoint->mediaType = OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO;

    return OI_OK;
}
#endif

OI_STATUS OI_AVDTP_TestConnect(OI_BD_ADDR *addr,
                               OI_L2CAP_CONNECT_PARAMS *params)
{
    OI_STATUS result;
    OI_L2CAP_CID cid;

    OI_CHECK_INIT(AVDTP);

    //Only allow one connection per address
    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (OI_SUCCESS(result)) return OI_STATUS_CONNECTION_EXISTS;

    OI_AVDTP_TestPrint(("AVDT_ConnectReq\n\tBD_ADDR=%s\n\tConfig_Requests:\n\t  InMTU=0x%.4x\n\t  OutFlow=0x%.4x\n\t  OutFlushTO=0x%.4x\n\t  LinkTO=0x%.4x\n",
                            OI_BDAddrText(addr),
                            params->inMTU,
                            0x0,
                            params->outFlushTO,
                            params->linkTO));

    result = OI_L2CAP_Connect(ConnectCfmInt,
                              DisconnectCB,
                              RecvDataInd,
                              OI_PSM_AVDTP,
                              addr,
                              params,
                              &cid,
                              &avdtp->connectPolicy);
    if (OI_SUCCESS(result)){
        avdtp->connReq++;
        AllocSignalInfo(cid, addr);
    }
    return result;
}

OI_STATUS OI_AVDTP_Connect(OI_BD_ADDR *addr)
{
    OI_L2CAP_CONNECT_PARAMS *params = NULL;
    OI_STATUS                result;
    OI_L2CAP_CID             cid;

    OI_CHECK_INIT(AVDTP);

    //Only allow one connection per address
    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (OI_SUCCESS(result)) {
        return OI_STATUS_CONNECTION_EXISTS;
    }

    OI_AVDTP_TestPrint(("AVDT_ConnectReq\n\tBD_ADDR=%s\n\tConfig_Requests:\n\t  InMTU=0x2a0\n\t  OutFlow=0x0\n\t  OutFlushTO=0xFFFF\n\t  LinkTO=0xFFFF\n",
                  OI_BDAddrText(addr)));

    if (avdtp->params.inMTU != 0){
        params = &avdtp->params;
    }

    result = OI_L2CAP_Connect(ConnectCfmInt,
                              DisconnectCB,
                              RecvDataInd,
                              OI_PSM_AVDTP,
                              addr,
                              params,
                              &cid,
                              &avdtp->connectPolicy);
    if (OI_SUCCESS(result)) {
        avdtp->connReq++;
        AllocSignalInfo(cid, addr);
    }
    return result;
}


OI_STATUS OI_AVDTP_Accept(OI_BD_ADDR *addr,
                          OI_BOOL accept)
{
    OI_L2CAP_CONNECT_PARAMS *params = NULL;
    OI_STATUS                result;
    OI_L2CAP_CID             cid;

    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (OI_SUCCESS(result)) {
        if (avdtp->params.inMTU != 0){
            params = &avdtp->params;
        }

        result = OI_L2CAP_Accept(ConnectCfmAcp,
                                 DisconnectCB,
                                 RecvDataInd,
                                 cid,
                                 params,
                                 accept,
                                 &avdtp->connectPolicy);
    } else {
        result = OI_STATUS_INVALID_STATE;
    }
    return result;
}

OI_STATUS OI_AVDTP_Disconnect(OI_BD_ADDR *addr)
{
    OI_STATUS result;
    OI_L2CAP_CID cid;

    OI_DBGPRINT(("Trace: OI_AVDTP_Disconnect: %:", addr));
    //Only allow one connection per address
    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (!OI_SUCCESS(result)) {
        return OI_STATUS_NOT_CONNECTED;
    }

    return OI_L2CAP_Disconnect(cid);
}

//Commands

OI_STATUS OI_AVDTP_Discover(OI_BD_ADDR *addr,
                            OI_UINT8 *transaction)
{
    OI_L2CAP_CID cid;
    OI_STATUS result;
    OI_UINT8 t;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Discover_Req\n\tBD_ADDR=%s\n", OI_BDAddrText(addr)));
    OI_AVDTP_TestPrint(("AVDT_Discover_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (OI_SUCCESS(result)) {
        result = AVDTPSignal_Discover(cid, t);
    } else {
        OI_AVDTP_TestPrint(("AVDT_Discover_Req\n\tTransaction=%d\n\tRSP=0x.4x\n", t, result));
    }

    return result;
}

OI_STATUS OI_AVDTP_GetCap(OI_BD_ADDR *addr,
                          OI_UINT8 seid,
                          OI_UINT8 *transaction)
{
    OI_L2CAP_CID cid;
    OI_STATUS result;
    OI_UINT8 t;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Get_Capabilities_Req\n\tBD_ADDR=%s\n\tACP SEID=%d\n", OI_BDAddrText(addr), seid));
    OI_AVDTP_TestPrint(("AVDT_Get_Capabilities_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (!OI_SUCCESS(result)) return result;

    avdtp->SEID = seid;
    return AVDTPSignal_GetCap(cid, t, seid);
}

OI_STATUS OI_AVDTP_GetConfig(OI_UINT16 handle,
                             OI_UINT8 *transaction)
{
    OI_UINT8 t;
    AVDTP_REMOTE_ENDPOINT *remote;

    OI_CHECK_INIT(AVDTP);

    remote = GetRemoteEndpoint(handle);
    if (!remote) {
        return OI_STATUS_NOT_FOUND;
    }

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Get_Configuration_Req\n\tStreamHandle=%d\n", handle));
    OI_AVDTP_TestPrint(("AVDT_Get_Configuration_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));
    remote->transaction = t;
    return AVDTPSignal_GetConfig(remote->signalCID, t, remote->remoteSEID);
}

OI_STATUS OI_AVDTP_SetConfig(OI_BD_ADDR *addr,
                             OI_UINT8 remoteSEID,
                             OI_UINT8 localSEID,
                             const OI_AVDTP_CAP *caps,
                             OI_UINT8 numCaps,
                             OI_UINT8 *transaction,
                             OI_UINT16 *handle)
{
    OI_L2CAP_CID cid;
    OI_STATUS result;
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 t;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    result = AVDTP_GetCIDFromAddr(addr, &cid);
    if (!OI_SUCCESS(result)) {
        return result;
    }

    *handle = cid;

    OI_AVDTP_TestPrint(("AVDT_Set_Configuration_Req\n\tBD_ADDR=%s\n\tACP SEID=%d\n\tINT SEID=%d\n",
                        OI_BDAddrText(addr),
                        remoteSEID,
                        localSEID));
    OI_AVDTP_TestPrintCaps(caps, numCaps);
    OI_AVDTP_TestPrint(("AVDT_Set_Configuration_Req\n\tTransaction=%d\n\tStream Handle=%d\n\tRSP=0x0000\n",
                        t,
                        *handle));

    endpoint = GetEndpoint(localSEID);
    if (NULL == endpoint) {
        return OI_STATUS_NOT_FOUND;
    }

    // if (endpoint->state != AVDTP_STATE_IDLE) {
        // return OI_STATUS_INVALID_STATE;
    // }

    remote = OI_Malloc(sizeof(AVDTP_REMOTE_ENDPOINT));
    if ( remote == NULL ){
        return OI_STATUS_OUT_OF_MEMORY;
    }

    /* link in new remote endpoint */
    remote->addr = *addr;
    remote->remoteSEID = remoteSEID;
    remote->signalCID = cid;
    remote->nextEndpoint = endpoint->remoteEndpoints;
    remote->transaction = t;
    endpoint->remoteEndpoints = remote;
    OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
    result = OI_AVDTP_CopyCaps(caps, numCaps, &endpoint->newConfigCaps, &endpoint->newNumCaps);

    if (OI_SUCCESS(result)){
        result = AVDTPSignal_SetConfig(cid,
                                       t,
                                       remoteSEID,
                                       localSEID,
                                       endpoint->newConfigCaps,
                                       numCaps);
    }

    if (!OI_SUCCESS(result)){
        OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, numCaps);
#ifndef OI_CODE
        endpoint->newNumCaps = 0;
#endif /* OI_CODE */
    }

    return result;
}


OI_STATUS OI_AVDTP_Open(OI_UINT16 handle,
                        OI_UINT8 *transaction)
{
    AVDTP_REMOTE_ENDPOINT *remote;
#ifndef OI_CODE
	AVDTP_ENDPOINT        *endpoint;
#endif /* OI_CODE */
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Open_Req\n\tStream Handle=%d\n", handle));
    OI_AVDTP_TestPrint(("AVDT_Open_Req\n\tTransaction=%d\n\tResponse=0x000\n", t));

    remote = GetRemoteEndpoint(handle);
    if (NULL == remote) {
        return OI_STATUS_NOT_FOUND;
    }
#ifndef OI_CODE
	endpoint = GetEndpointBySignalCID(handle);
	if ( NULL == endpoint ) {
        return OI_STATUS_NOT_FOUND;
    }

    if (endpoint->state != AVDTP_STATE_CONFIGURED) {
        // return OI_STATUS_NOT_CONFIGURED;
    }
#else  
#error code not present
#endif /* OI_CODE */
    result = AVDTPSignal_Open(remote->signalCID, t, remote->remoteSEID);
    if (OI_SUCCESS(result)) {
        remote->transaction = t;
    }
    return result;
}

OI_STATUS OI_AVDTP_Start(OI_UINT16 *handles,
                         OI_UINT8 num,
                         OI_UINT8 *transaction)
{
    AVDTP_ENDPOINT *endpoint = NULL;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 *remoteSEIDs;
    OI_L2CAP_CID cid = 0;
    OI_UINT8 i;
    OI_STATUS result;
    OI_UINT8 t;

    OI_CHECK_INIT(AVDTP);


    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Start_Req\n\tStream Handles="));
    OI_AVDTP_TestPrintHandles(handles, num);
    OI_AVDTP_TestPrint(("AVDT_Start_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    remoteSEIDs = OI_Malloc(num);
    if (NULL == remoteSEIDs) {
        result = OI_STATUS_OUT_OF_MEMORY;
        goto start_fail;
    }

    for (i = 0; i < num; i++) {
        endpoint = GetEndpointBySignalCID(handles[i]);
        remote = GetRemoteEndpoint(handles[i]);
        if ((NULL == endpoint) || (NULL == remote)) {
            result = OI_STATUS_NOT_FOUND;
            goto start_fail;
        }
        // if (endpoint->state != AVDTP_STATE_OPEN) {
            // return OI_STATUS_INVALID_STATE;
        // }

        OI_ASSERT(remote->remoteSEID != 0);
        remoteSEIDs[i] = remote->remoteSEID;
        remote->transaction = t;

        //Check that all SEIDs have the same signalling channel
        if (i == 0) {
            cid = remote->signalCID;
        } else {
            if (remote->signalCID != cid) {
                result = OI_STATUS_INVALID_PARAMETERS;
                goto start_fail;
            }
        }
    }

    result = AVDTPSignal_Start(cid, t, remoteSEIDs, num);
    if (OI_SUCCESS(result)) {
        if ( endpoint ) {
            if (endpoint->endpointType == OI_AVDTP_SNK) {
                SetState(endpoint, AVDTP_STATE_STREAMING);
            }
        }
    }

start_fail:
    OI_FreeIf(&remoteSEIDs);
    return result;
}

OI_STATUS OI_AVDTP_Close(OI_UINT16 handle,
                         OI_UINT8 *transaction)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Close_Req\n\tStream Handle=%d\n", handle));
    OI_AVDTP_TestPrint(("AVDT_Close_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (remote == NULL) ) return OI_STATUS_NOT_FOUND;

    if ( (endpoint->state != AVDTP_STATE_OPEN) && (endpoint->state != AVDTP_STATE_STREAMING)) {
        return OI_STATUS_INVALID_STATE;
    }

    result = AVDTPSignal_Close(remote->signalCID, t, remote->remoteSEID);
    if (OI_SUCCESS(result)) {
        remote->transaction = t;
    }
    return result;
}

OI_STATUS OI_AVDTP_Abort(OI_UINT16 handle,
                         OI_UINT8 *transaction)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Abort_Req\n\tStream Handle=%d\n", handle));
    OI_AVDTP_TestPrint(("AVDT_Abort_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (remote == NULL) ) return OI_STATUS_NOT_FOUND;

    if (endpoint->state == AVDTP_STATE_IDLE) {
        return OI_STATUS_NOT_CONFIGURED;
    }
    result = AVDTPSignal_Abort(remote->signalCID, t, remote->remoteSEID);
    if (OI_SUCCESS(result)) {
        remote->transaction = t;
    }
    return result;
}

OI_STATUS OI_AVDTP_Suspend(OI_UINT16 *handles,
                           OI_UINT8   numHandles,
                           OI_UINT8  *transaction)
{
    AVDTP_ENDPOINT *endpoint = NULL;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 *remoteSEIDs;
    OI_L2CAP_CID cid = 0;
    OI_UINT8 i;
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);
    ARGCHECK(avdtp->callbacks->suspendCfm);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Suspend_Req\n\tStream Handles="));
    OI_AVDTP_TestPrintHandles(handles, numHandles);
    OI_AVDTP_TestPrint(("AVDT_Suspend_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    remoteSEIDs = OI_Malloc(numHandles);
    if (NULL == remoteSEIDs){
        result = OI_STATUS_OUT_OF_MEMORY;
        goto suspend_fail;
    }

    for (i = 0; i < numHandles; i++) {
        endpoint = GetEndpointBySignalCID(handles[i]);
        remote = GetRemoteEndpoint(handles[i]);
        if ((NULL == endpoint) || (NULL == remote)) {
            result = OI_STATUS_NOT_FOUND;
            goto suspend_fail;
        }

        if (endpoint->state != AVDTP_STATE_STREAMING) return OI_STATUS_INVALID_STATE;

        remoteSEIDs[i] = remote->remoteSEID;
        remote->transaction = t;

        if (i == 0) {
            cid = remote->signalCID;
        }
        else if (remote->signalCID != cid){
            result = OI_STATUS_INVALID_PARAMETERS;
            goto suspend_fail;
        }
    }

    result = AVDTPSignal_Suspend(cid, t, remoteSEIDs, numHandles);

suspend_fail:
    OI_FreeIf(&remoteSEIDs);
    return result;
}


OI_STATUS OI_AVDTP_Reconfigure(OI_UINT16 handle,
                               OI_AVDTP_CAP *caps,
                               OI_UINT8 numCaps,
                               OI_UINT8 *transaction)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);
    ARGCHECK(avdtp->callbacks->reconfigureCfm);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_ReConfigure_Req\n\tStream Handle=%d\n\t", handle));
    OI_AVDTP_TestPrintCaps(caps, numCaps);
    OI_AVDTP_TestPrint(("AVDT_ReConfigure_Req\n\tTransaction=%d\n\tRSP=0x0000\n", t));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (remote == NULL) ) return OI_STATUS_NOT_FOUND;

    if (endpoint->state != AVDTP_STATE_OPEN) {
        return OI_STATUS_INVALID_STATE;
    }

    OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
    result = OI_AVDTP_CopyCaps(caps, numCaps, &endpoint->newConfigCaps, &endpoint->newNumCaps);

    if (OI_SUCCESS(result)) {
        result = AVDTPSignal_Reconfigure(remote->signalCID,
                                         t,
                                         remote->remoteSEID,
                                         endpoint->newConfigCaps,
                                         numCaps);
    }

    if (OI_SUCCESS(result)) {
        remote->transaction = t;
    } else {
        OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
#ifndef OI_CODE
        endpoint->newNumCaps = 0;
#endif /* OI_CODE */
    }
    return result;
}

//Responses

OI_STATUS OI_AVDTP_SetConfigRsp(OI_UINT16 handle,
                                OI_UINT8  transaction,
                                OI_UINT8  category,
                                OI_UINT8  error)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;

    OI_CHECK_INIT(AVDTP);

    OI_AVDTP_TestPrint(("AVDT_Set_Configuration_Rsp\n\tTransaction=%d\n\tStream Handle=%d\n\tError=0x%.2x\n\tFailing Params:\n",
                    transaction,
                    handle,
                    error));
    OI_AVDTP_TestPrint(("AVDT_Set_Configuration_Rsp\n\tRSP=0x0000\n"));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (NULL == remote) ){
        return OI_STATUS_NOT_FOUND;
    }

    if ( OI_AVDTP_SUCCESS == error ){
        /* We are accepting, so discard old caps in favor of just received caps */
        SetState(endpoint, AVDTP_STATE_CONFIGURED);
        OI_AVDTP_FreeCaps(&endpoint->configCaps, endpoint->numConfigCaps);
        endpoint->configCaps = endpoint->newConfigCaps;
        endpoint->numConfigCaps = endpoint->newNumCaps;
        endpoint->newConfigCaps = NULL;
        endpoint->newNumCaps = 0;
    } else {
        /* We are rejecting, so discard the just received caps */
        endpoint->inUse = FALSE;
        OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
        endpoint->newNumCaps = 0;
    }

    return AVDTPSignal_SetConfigRsp(remote->signalCID, transaction, category, error);
}

OI_STATUS OI_AVDTP_ReconfigureRsp(OI_UINT16 handle,
                                  OI_UINT8 transaction,
                                  OI_UINT8 category,
                                  OI_UINT8 error)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;

    OI_CHECK_INIT(AVDTP);

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (NULL == remote) ){
        return OI_STATUS_NOT_FOUND;
    }

    if (endpoint->state != AVDTP_STATE_OPEN) {
        return OI_STATUS_INVALID_STATE;
    }

    if ( OI_AVDTP_SUCCESS == error ){
        /* We are accepting, so discard old caps in favor of just received caps */
        OI_AVDTP_FreeCaps(&endpoint->configCaps, endpoint->numConfigCaps);
        endpoint->configCaps = endpoint->newConfigCaps;
        endpoint->numConfigCaps = endpoint->newNumCaps;
        endpoint->newConfigCaps = NULL;
        endpoint->newNumCaps = 0;
    } else {
        /* We are rejecting, so discard the just received caps */
        OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
        endpoint->newNumCaps = 0;
    }

    return AVDTPSignal_ReconfigureRsp(remote->signalCID, transaction, category, error);
}

OI_STATUS OI_AVDTP_SecurityControl(OI_UINT16 handle,
                                   OI_BYTE  *data,
                                   OI_UINT16 dataLen,
                                   OI_UINT8 *transaction)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 t;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);
    ARGCHECK(avdtp->callbacks->securityCfm);

    t = NewTransaction();
    if (transaction) {
        *transaction = t;
    }

    OI_AVDTP_TestPrint(("AVDT_Security_Control_Req\n\tStream Handle=%d\n\tData Length=%d\n\tData=%s\n",
                   handle,
                   dataLen,
                   OI_HexText(data, dataLen)));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (NULL == remote) ){
        return OI_STATUS_NOT_FOUND;
    }

    if ( (endpoint->state == AVDTP_STATE_IDLE)
         || (endpoint->state == AVDTP_STATE_CLOSING)
         || (endpoint->state == AVDTP_STATE_ABORTING)) {

        return OI_STATUS_INVALID_STATE;
    }

    result = AVDTPSignal_SecurityControl(remote->signalCID, t, remote->remoteSEID, data, dataLen);
    OI_AVDTP_TestPrint(("AVDT_Security_Control_Req\n\tTransaction=%d\n\tRSP=0x%.4x\n", t, result));
    if (OI_SUCCESS(result)) {
        remote->transaction = t;
    }
    return result;
}

OI_STATUS OI_AVDTP_SecurityControlRsp(OI_UINT16 handle,
                                      OI_UINT8  transaction,
                                      OI_BYTE  *data,
                                      OI_UINT16 dataLen,
                                      OI_UINT8  error)
{
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;

    OI_CHECK_INIT(AVDTP);

    OI_AVDTP_TestPrint(("AVDT_Security_Control_Rsp\n\tTransaction=%d\n\tStream Handle=%d\n\tData Length=%d\n\tData=%s\n",
                  transaction,
                  handle,
                  dataLen,
                  OI_HexText(data, dataLen)));

    endpoint = GetEndpointBySignalCID(handle);
    remote = GetRemoteEndpoint(handle);
    if( (NULL == endpoint) || (NULL == remote) ){
        return OI_STATUS_NOT_FOUND;
    }

    if ( (endpoint->state == AVDTP_STATE_IDLE)
         || (endpoint->state == AVDTP_STATE_CLOSING)
         || (endpoint->state == AVDTP_STATE_ABORTING)) {

        return OI_STATUS_INVALID_STATE;
    }

    return AVDTPSignal_SecurityControlRsp(remote->signalCID, transaction, data, dataLen, error);
}



OI_STATUS OI_AVDTP_BroadcastData(OI_UINT16 handle,
                                 OI_UINT8  seqNum,
                                 OI_UINT8  offset,
                                 OI_UINT8  fractional,
                                 OI_BOOL   fec,
                                 OI_BYTE * data,
                                 OI_UINT16 dataLen)
{
    //AVDTP_ENDPOINT *endpoint;
    OI_BYTE_STREAM *header;
    OI_MBUF *mbuf;
    OI_STATUS result;

    OI_CHECK_INIT(AVDTP);

    OI_AVDTP_TestPrint(("AVDT_Write_Req\n\tStream Handle=%d\n\tLength=%d\n\tOutBuffer=%s\n",
                        handle,
                        dataLen,
                        OI_HexText(data, dataLen)));

    //endpoint = GetEndpointBySignalCID(handle);
    //if (NULL == endpoint) return OI_STATUS_NOT_FOUND;

    // if (endpoint->state != AVDTP_STATE_STREAMING) {
        // return OI_STATUS_INVALID_STATE;
    // }
    if (0 == avdtp->mediaCID) return OI_STATUS_NOT_FOUND;

    //Create the header
    header = ByteStream_Alloc(AVDTP_MEDIA_HEADER_LEN);
    if (NULL == header) return OI_STATUS_OUT_OF_MEMORY;

    ByteStream_Open(*header, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*header, seqNum );
    ByteStream_PutUINT8(*header, offset );
    ByteStream_PutUINT8(*header, fractional );
    ByteStream_Close(*header);

    mbuf = OI_MBUF_Alloc(2); //hdr + data
    if (NULL == mbuf) {
        ByteStream_Free(header);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    //Add Data
    result = OI_MBUF_Prepend(mbuf, data, dataLen, MBUF_KEEP);
    OI_ASSERT(OI_SUCCESS(result));

    //Add header
    result = OI_MBUF_Prepend(mbuf,
                             ByteStream_GetDataPointer(*header),
                             ByteStream_GetSize(*header),
                             MBUF_FREE);

    OI_ASSERT(OI_SUCCESS(result));

    if ( fec ){
#ifndef OI_CODE
        result = OI_L2CAP_WriteMBUF(MediaWriteMBUFCfm, avdtp->fecCID, mbuf
                        , NULL);
#else
#error code not present
#endif
    } else {
#ifndef OI_CODE
        result = OI_L2CAP_WriteMBUF(MediaWriteMBUFCfm, avdtp->mediaCID, mbuf
                        , NULL);
#else
#error code not present
#endif
    }

    if (!OI_SUCCESS(result)) {
        OI_MBUF_Free(mbuf);
    }
    OI_Free(header); //we don't need the bytestream anymore, just its data
    return result;
}

OI_STATUS OI_AVDTP_VSendFragmented(OI_UINT16 handle,
                                  OI_UINT16 seqNum,
                                  OI_UINT32 timestamp,
                                  OI_UINT8  payloadType,
                                  OI_UINT16 marker,
                                   OI_INT    segments,
                                   va_list   argp)
{
    AVDTP_ENDPOINT *endpoint;
    OI_BYTE_STREAM *header;
    OI_MBUF *mbuf;
    OI_STATUS result;
    OI_BYTE        *dataPtr;
    OI_UINT16       dataLen;

    OI_CHECK_INIT(AVDTP);

    OI_AVDTP_TestPrint(("AVDT_Write_Req\n\tStream Handle=%d\n\tSeqNum=%d\n\tTimeInfo=%lu\n\tPayload Type=%d\n\tMarker=%d\n",
                   handle, seqNum, timestamp, payloadType, marker));

    endpoint = GetEndpointBySignalCID(handle);
    if (NULL == endpoint) {
        return OI_STATUS_NOT_FOUND;
    }

    if (endpoint->state != AVDTP_STATE_STREAMING) {
        return OI_STATUS_INVALID_STATE;
    }

    if ((segments < 0) || (segments >= 255)){
        return OI_STATUS_INVALID_PARAMETERS;
    }

    //Create the header
    header = ByteStream_Alloc(AVDTP_MEDIA_HEADER_LEN);
    if (NULL == header) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    ByteStream_Open(*header, BYTESTREAM_WRITE);
    ByteStream_PutUINT8(*header, (2 << 6));
    ByteStream_PutUINT8(*header, (marker ? BIT7 : 0) | (payloadType & 0x7F) );
    ByteStream_PutUINT16(*header, seqNum, OI_BIG_ENDIAN_BYTE_ORDER);
    ByteStream_PutUINT32(*header, timestamp, OI_BIG_ENDIAN_BYTE_ORDER);
    ByteStream_PutUINT32(*header, 0, OI_BIG_ENDIAN_BYTE_ORDER); //SSRC
    ByteStream_Close(*header);

    OI_ASSERT(segments >= 0);
    OI_ASSERT(segments < 254);

    mbuf = OI_MBUF_Alloc((OI_UINT8)(segments + 1));
    if (NULL == mbuf) {
        ByteStream_Free(header);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    //Add AVDTP header
    result = OI_MBUF_Append(mbuf,
                             ByteStream_GetDataPointer(*header),
                             ByteStream_GetSize(*header),
                             MBUF_FREE);

    while(OI_SUCCESS(result) && segments--){
        dataLen = (OI_UINT16) va_arg(argp, OI_INT);
        dataPtr = (OI_BYTE *) va_arg(argp, void *);
        if (NULL != dataPtr){
            result = OI_MBUF_Append(mbuf, dataPtr, dataLen, MBUF_KEEP);
        }
    }

    /* No more Segments */

    if (OI_SUCCESS(result)) {
    if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
        OI_DBGPRINT(("Broadcast packet Sent!"));
    } else {
        OI_DBGPRINT(("P-to-P packet Sent!"));
    }
#ifndef OI_CODE
    result = OI_L2CAP_WriteMBUF(MediaWriteMBUFCfm, endpoint->mediaCID, mbuf, 
                                NULL);
#else
#error code not present
#endif
    if (!OI_SUCCESS(result)) {
        OI_MBUF_Free(mbuf);
    }
    }
    OI_Free(header); //we don't need the bytestream anymore, just its data
    return result;
}

OI_STATUS OI_AVDTP_SendFragmented(OI_UINT16 handle,
                                  OI_UINT16 seqNum,
                                  OI_UINT32 timestamp,
                                  OI_UINT8  payloadType,
                                  OI_UINT16 marker,
                                  OI_INT    segments,
                                  ... )
{
    OI_STATUS result;
    va_list argp;

    va_start(argp, segments);

    result = OI_AVDTP_VSendFragmented(handle, seqNum, timestamp, payloadType, marker, segments, argp);

    va_end(argp);

    return result;
}


/******************************************************************

  Receive Functions

*****************************************************************/

//Macro to get an SEID (left-shifted 2) from a bytestream
#define GetSEID_Checked(bs, seid) ByteStream_GetUINT8_Checked(bs, (seid)); (seid) = ((seid) >> 2);

/**
 *
 * @param transaction  The transaction number to search for.
 * @param handles      [OUT] The array to hold the handles
 * @param numHandles   [OUT] The number of ids found.
 */
static void GetHandlesByTransaction(OI_UINT8  transaction,
                                    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS],
                                    OI_UINT8 *numHandles)
{
    AVDTP_REMOTE_ENDPOINT *remote;
    OI_UINT8 i;

    OI_ASSERT(OI_INIT_FLAG_VALUE(AVDTP));

    *numHandles = 0;

    for (i = 0; i < avdtp->maxEndpoints; i++) {
        if ( avdtp->endpoints[i].seid != 0 ){
            for( remote = avdtp->endpoints[i].remoteEndpoints; remote != NULL; remote = remote->nextEndpoint){
                if ( remote->transaction == transaction) {
                    handles[(*numHandles)++] = remote->signalCID;
                }
            }
        }
    }
}

static OI_UINT8 AVDTPRecv_Discover(OI_L2CAP_CID cid,
                                   OI_BYTE_STREAM *pBs,
                                   OI_UINT8 transaction,
                                   OI_UINT8 msgType)
{
    OI_BD_ADDR addr;
    OI_STATUS status;
    OI_UINT8 result = OI_AVDTP_SUCCESS;

    OI_DBGPRINT(("Received DISCOVER - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    status = AVDTP_GetAddrFromCID(cid, &addr);
    (void) status;     // satisfy compiler
    OI_ASSERT(OI_SUCCESS(status));

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        OI_AVDTP_DISCOVER_RSP_INFO *info = NULL;
        OI_UINT8 byte = 0;
        OI_UINT8 count;
        OI_INT16 len = ByteStream_NumReadBytesAvail(*pBs);
        OI_UINT8 i;

        //Determine number of info blocks
        count = len / 2;

        if( count ){
            OI_DBGPRINT2(("Allocating space for %d responses", count));
            info = OI_Malloc(count * sizeof(OI_AVDTP_DISCOVER_RSP_INFO));

            if (NULL == info) {
                OI_LOG_ERROR(("Error - out of memory"));
                result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
                count = 0;
            }
        }
        for (i = 0; i < count; i++) {
            ByteStream_GetUINT8_Checked(*pBs, byte);
            info[i].seid = byte >> 2;
            info[i].inUse = (byte & BIT1) >> 1;
            ByteStream_GetUINT8_Checked(*pBs, byte);
            info[i].mediaType = byte >> 4;
            info[i].endpointType = (byte & BIT3) ? TRUE : FALSE;
        }
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->discoverCfm(transaction, &addr, info, count, result);
        OI_FreeIf(&info);
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        ByteStream_GetUINT8_Checked(*pBs, result);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        }
        if (OI_AVDTP_SUCCESS == result) {
            result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->discoverCfm(transaction, &addr, NULL, 0, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        OI_UINT8 i;
        OI_UINT8 count = 0;
        OI_AVDTP_DISCOVER_RSP_INFO info[AVDTP_MAX_ENDPOINTS];

        for (i = 0; i < avdtp->maxEndpoints; i++) {
            if (avdtp->endpoints[i].seid != 0) {
                info[i].mediaType = avdtp->endpoints[i].mediaType;
                info[i].endpointType = avdtp->endpoints[i].endpointType;
                info[i].seid = avdtp->endpoints[i].seid;
                info[i].inUse = avdtp->endpoints[i].inUse;
                count++;
            }
        }

        OI_DBGPRINT2(("Returning %d endpoints", count));

        OI_AVDTP_TestPrint(("AVDT_DiscoverInd_CB\n\tTransaction=%d\n\tBD_ADDR=%s\n",
                        transaction,
                        OI_BDAddrText(&addr)));
        /* *Always* respond to commands */
        AVDTPSignal_DiscoverRsp(cid, transaction, info, count, result);
    }
    return result;
}


/**
 * This function parses the service capabilities.
 *
 * @param pBs     the bytestream to begin parsing
 * @param caps    This parameter holds the parsed capabilites.
 *                This memory must be freed when it is no longer in use.
 * @param numCaps the number of capabilities parsed
 *
 * @return        OI_AVDTP_SUCCESS if successful
 */
static OI_UINT8 ParseCaps(OI_BYTE_STREAM *pBs,
                          OI_AVDTP_CAP **caps,
                          OI_UINT8 *numCaps)
{
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    OI_UINT8 i;
    OI_UINT8 count = 0;
    OI_UINT8 parsed_caps = 0;
    OI_UINT16 pos;
    OI_UINT8 len = 0;

    *caps = NULL;
    *numCaps = 0;
    pos = ByteStream_GetPos(*pBs);

    while ( ByteStream_NumReadBytesAvail(*pBs) > 0) {
        ByteStream_Skip_Checked(*pBs, 1); /* skip type field */
        ByteStream_GetUINT8_Checked(*pBs, len);
        ByteStream_Skip_Checked(*pBs, len);
        count++;
    }
    if (ByteStream_Error(*pBs)) {
        result = OI_AVDTP_ERR_LENGTH;
        goto parse_fail;
    }

    if( count ){
        *caps = OI_Malloc(count * sizeof(OI_AVDTP_CAP));
        if (NULL == *caps) {
            OI_LOG_ERROR(("ParseCaps: Could not allocate memory for %d capabilities", count));
            result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
            goto parse_fail;
        }
    }
    OI_DBGPRINT(("numCaps = %d", count));
    ByteStream_SetPos(*pBs, pos); //Go back to where we started

    for (i = 0; ((i < count) && ( 0 == ByteStream_Error(*pBs))); i++) {
        OI_AVDTP_CAP *cap = &((*caps)[i]);
        OI_UINT8 len = 0;

        ByteStream_GetUINT8_Checked(*pBs, cap->type);
        ByteStream_GetUINT8_Checked(*pBs, len);
        OI_DBGPRINT2(("got capability %d", cap->type));

        switch(cap->type) {
            case OI_AVDTP_CAP_MEDIA_TRANSPORT:
                if (len != 0) {
                    OI_LOG_ERROR(("Invalid transport capability"));
                    result = OI_AVDTP_ERR_MEDIA_TRANSPORT_FORMAT;
                    goto parse_fail;
                }
                break;
            case OI_AVDTP_CAP_CODEC:
                ByteStream_GetUINT8_Checked(*pBs, cap->data.codecData.mediaType);
                cap->data.codecData.mediaType >>= 4;
                ByteStream_GetUINT8_Checked(*pBs, cap->data.codecData.codecType);

                cap->data.codecData.dataLen = len - AVDTP_CAP_CODEC_LEN;
                if (len < AVDTP_CAP_CODEC_LEN){
                    result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
                    goto parse_fail;
                } else if (cap->data.codecData.dataLen) {
                    cap->data.codecData.data = OI_Malloc(cap->data.codecData.dataLen);
                    if (NULL != cap->data.codecData.data){
                        ByteStream_GetBytes_Checked(*pBs,
                                                    cap->data.codecData.data,
                                                    cap->data.codecData.dataLen);
                    } else {
                        result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE; 
                        goto parse_fail;
                    }
                } else {
                    cap->data.codecData.data = NULL;
                }
                break;

            default:
                cap->data.genericData.dataLen = len;
                if (len) {
                    cap->data.genericData.data = OI_Malloc(len);
                    if (NULL != cap->data.genericData.data){
                        ByteStream_GetBytes_Checked(*pBs, cap->data.genericData.data, len);
                    } else {
                        result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE; 
                        goto parse_fail;
                    }
                } else {
                    cap->data.genericData.data = NULL;
                }
                break;
        }

        /* Count successfully parsed caps in case we need to abort */
        parsed_caps++;
    }

parse_fail:
    if (ByteStream_Error(*pBs)) {
        OI_LOG_ERROR(("ParseCaps: length error"));
        result = OI_AVDTP_ERR_LENGTH;
    }

    if (result != OI_AVDTP_SUCCESS) {
        OI_AVDTP_FreeCaps(caps, parsed_caps);
        return result;
    } else {
        *numCaps = count;
        return OI_AVDTP_SUCCESS;
    }
}


static OI_UINT8 AVDTPRecv_GetCaps(OI_L2CAP_CID    cid,
                                  OI_BYTE_STREAM *pBs,
                                  OI_UINT8        transaction,
                                  OI_UINT8        msgType)
{
    OI_AVDTP_CAP *caps;
    OI_UINT8 numCaps;
    OI_BD_ADDR addr;
    OI_STATUS status;   // debug variable
    OI_UINT8 result = OI_AVDTP_SUCCESS;

    OI_ASSERT(pBs);

    OI_DBGPRINT(("Received GET CAPS - %s", MsgTypeText(msgType)));

    status = AVDTP_GetAddrFromCID(cid, &addr);
    (void) status;      // satisfy compiler
    OI_ASSERT(OI_SUCCESS(status));

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        result = ParseCaps(pBs, &caps, &numCaps);
        /* *Always* pass a response up to app */
        avdtp->callbacks->getCapsCfm(transaction, &addr, avdtp->SEID, caps, numCaps, result);
        OI_AVDTP_FreeCaps(&caps, numCaps);
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {

        ByteStream_GetUINT8_Checked(*pBs, result);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        }
        if (OI_AVDTP_SUCCESS == result) {
            result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->getCapsCfm(transaction, &addr, avdtp->SEID, NULL, 0, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        AVDTP_ENDPOINT *endpoint = NULL;
        OI_UINT8 seid = 0;

        ByteStream_GetUINT8_Checked(*pBs, seid);
        seid = seid >> 2;

        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        } else {
            endpoint = GetEndpoint(seid);
            if (NULL == endpoint) {
                OI_LOG_ERROR(("Get caps received for unknown seid %d", seid));
                result = OI_AVDTP_ERR_ACP_SEID;
            }
        }

        OI_AVDTP_TestPrint(("AVDT_Capabilities_Rsp\n\tTransaction=%d\n\tBD_ADDR=%s\n\tSEID=%d\n\tErrorcode=0x%.2x\n",
                            transaction,
                            OI_BDAddrText(&addr),
                            seid,
                            result));
        OI_AVDTP_TestPrintCaps((OI_AVDTP_CAP *)endpoint->caps, endpoint->numCaps);
        OI_AVDTP_TestPrint(("AVDT_Capabilities_Rsp\n\tRSP=0x0000\n"));

        /* *Always* respond to commands */
        if (OI_AVDTP_SUCCESS == result) {
            AVDTPSignal_GetCapRsp(cid, transaction, (OI_AVDTP_CAP *)endpoint->caps, endpoint->numCaps, OI_AVDTP_SUCCESS);
        } else {
            AVDTPSignal_GetCapRsp(cid, transaction, NULL, 0, result);
        }
    }
    return result;
}

static OI_UINT8 AVDTPRecv_SetConfig(OI_L2CAP_CID    cid,
                                    OI_BYTE_STREAM *pBs,
                                    OI_UINT8        transaction,
                                    OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_AVDTP_CAP *caps;
    OI_UINT8 numCaps;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    OI_UINT8 acpSEID = 0;
    OI_UINT8 intSEID = 0;
    OI_STATUS retVal;
    OI_BD_ADDR addr;
    AVDTP_REMOTE_ENDPOINT *remote;
    AVDTP_ENDPOINT *endpoint;

    OI_DBGPRINT(("Received SET CONFIG - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    retVal = AVDTP_GetAddrFromCID(cid, &addr);
    OI_ASSERT(OI_SUCCESS(retVal));

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        endpoint = GetEndpointBySignalCID(cid);
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            SetStates(handles, numHandles, AVDTP_STATE_CONFIGURED);

            remote = GetRemoteEndpoint(cid);
            if (NULL == remote) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                remote->addr = addr;
                remote->signalCID = cid;
                if (NULL != endpoint){
                    OI_AVDTP_FreeCaps(&endpoint->configCaps, endpoint->numConfigCaps);
                    endpoint->configCaps = endpoint->newConfigCaps;
                    endpoint->numConfigCaps = endpoint->newNumCaps;
                    endpoint->newConfigCaps = NULL;
                    endpoint->newNumCaps = 0;
                }
            }
        }

        if (OI_AVDTP_SUCCESS != result){
            /* On reject/failure, clean up malloced remote endpoint */
            FreeRemoteEndpoint(cid);
            if (NULL != endpoint){
                OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->setConfigCfm(transaction, cid, 0, result);
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        OI_UINT8 category = 0;

        endpoint = GetEndpointBySignalCID(cid);
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, category);
            ByteStream_GetUINT8_Checked(*pBs, result);

            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
                category = 0;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
                category = 0;
            }
        }

        /* On reject/failure, clean up malloced remote endpoint, and caps */
        FreeRemoteEndpoint(cid);
        if (NULL != endpoint){
            OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
#ifndef OI_CODE
            endpoint->newNumCaps = 0;
#endif /* OI_CODE */
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->setConfigCfm(transaction, cid, category, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        GetSEID_Checked(*pBs, acpSEID);
        GetSEID_Checked(*pBs, intSEID);
        result = ParseCaps(pBs, &caps, &numCaps);

        if (OI_AVDTP_SUCCESS == result) {
            endpoint = GetEndpoint(acpSEID);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                if (endpoint->inUse) {
                    /* Only 1 remote endpoint for ACP side, even in Multicast env */
                    result = OI_AVDTP_ERR_SEP_IN_USE;
                } else {
                    if (NULL != endpoint->newConfigCaps) {
                        /* We have insufficient resources to handle multiple config commands */
                        result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
                    } else {
                        retVal = AVDTP_GetAddrFromCID(cid, &addr);
                        if (OI_SUCCESS(retVal)) {
                            /* gix - When do we Malloc this??? */
                            remote = OI_Malloc(sizeof(AVDTP_REMOTE_ENDPOINT));
                            if ( NULL != remote ){
                                remote->nextEndpoint = endpoint->remoteEndpoints;
                                remote->signalCID = cid;
                                remote->remoteSEID = intSEID;
                                remote->addr = addr;
                                endpoint->remoteEndpoints = remote;
                                endpoint->inUse++;

                                /* Save capabilities until Application accepts them */
                                endpoint->newConfigCaps = caps;
                                endpoint->newNumCaps = numCaps;
                            } else {
                                result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
                            }
                        } else {
                            result = OI_AVDTP_ERR_BAD_SERVICE;
                        }
                    }
                }
            }
        }

        /* We must either pass a successfully parsed CAPs list to the
         * application *or* send a rejection message to the remote
         * device ourselves
         */
        if (OI_AVDTP_SUCCESS == result) {
            avdtp->callbacks->setConfigInd(transaction, &addr, acpSEID, intSEID, cid, caps, numCaps);
        } else {
            OI_AVDTP_FreeCaps(&caps, numCaps);
            AVDTPSignal_SetConfigRsp(cid, transaction, 0, result);
        }
    }
    return result;
}


static OI_UINT8 AVDTPRecv_Reconfigure(OI_L2CAP_CID    cid,
                                      OI_BYTE_STREAM *pBs,
                                      OI_UINT8        transaction,
                                      OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_AVDTP_CAP *caps = NULL;
    OI_UINT8 numCaps = 0;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    OI_UINT8 acpSEID = 0;
    OI_STATUS retVal;
    OI_BD_ADDR addr;
    AVDTP_ENDPOINT *endpoint;
    AVDTP_REMOTE_ENDPOINT *remote;

    if (!OI_INIT_FLAG_VALUE(AVDTP)){
        return OI_AVDTP_ERR_STATE;
    }

    OI_DBGPRINT(("Received RECONFIGURE - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    retVal = AVDTP_GetAddrFromCID(cid, &addr);
    (void) retVal;  // satisfy compiler
    OI_ASSERT(OI_SUCCESS(retVal));

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        endpoint = GetEndpointBySignalCID(cid);
        if (0 == numHandles) {
            result =  OI_AVDTP_ERR_HEADER_FORMAT;
        } else {

            remote = GetRemoteEndpoint(handles[0]);
            if (NULL == remote) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                remote->addr = addr;
                remote->signalCID = cid;
                if (NULL != endpoint){
                    OI_AVDTP_FreeCaps(&endpoint->configCaps, endpoint->numConfigCaps);
                    endpoint->configCaps = endpoint->newConfigCaps;
                    endpoint->numConfigCaps = endpoint->newNumCaps;
                    endpoint->newConfigCaps = NULL;
                    endpoint->newNumCaps = 0;
                }
            }
        }

        if (OI_AVDTP_SUCCESS != result){
            if (NULL != endpoint){
                OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
#ifndef OI_CODE
                endpoint->newNumCaps = 0;
#endif /* OI_CODE */
            }
        }

        /* *Always* pass a response up to app */
        if (NULL != avdtp->callbacks->reconfigureCfm){
            avdtp->callbacks->reconfigureCfm(transaction, cid, 0, result);
        }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        OI_UINT8 category = 0;

        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, category);
            ByteStream_GetUINT8_Checked(*pBs, result);

            if (ByteStream_Error(*pBs)) {
                category = 0;
                result = OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                category = 0;
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        endpoint = GetEndpointBySignalCID(cid);
        if (NULL != endpoint){
            OI_AVDTP_FreeCaps(&endpoint->newConfigCaps, endpoint->newNumCaps);
#ifndef OI_CODE
            endpoint->newNumCaps = 0;
#endif /* OI_CODE */
        }

        /* *Always* pass a response up to app */
        if (NULL != avdtp->callbacks->reconfigureCfm){
            avdtp->callbacks->reconfigureCfm(transaction, cid, category, result);
        }
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        if (NULL == avdtp->callbacks->reconfigureInd){
            result = OI_AVDTP_ERR_UNSUPPORTED_COMMAND;
        } else {
        GetSEID_Checked(*pBs, acpSEID);
        result = ParseCaps(pBs, &caps, &numCaps);
        if (OI_AVDTP_SUCCESS == result) {
            endpoint = GetEndpoint(acpSEID);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_SEP_NOT_IN_USE;
            } else {
                if (endpoint->state != AVDTP_STATE_OPEN) {
                    result = OI_AVDTP_ERR_STATE;
                } else {
                    if (NULL != endpoint->newConfigCaps) {
                        /* We have insufficient resources to handle multiple config commands */
                        result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
                    } else {
                        /* Save new capabilities until Application accepts them */
                        endpoint->newConfigCaps = caps;
                        endpoint->newNumCaps = numCaps;
                    }
                }
            }
        }
        }

        /* We must either pass a successfully parsed CAPs list to the
         * application *or* send a rejection message to the remote
         * device ourselves
         */
        if (OI_AVDTP_SUCCESS == result) {
                avdtp->callbacks->reconfigureInd(transaction, cid, caps, numCaps);
        } else {
            /* We didn't save these caps, so discard them and send reject reply */
            OI_AVDTP_FreeCaps(&caps, numCaps);
            AVDTPSignal_ReconfigureRsp(cid, transaction, 0, result);
        }
    }
    return result;
}


static OI_UINT8 AVDTPRecv_GetConfig(OI_L2CAP_CID    cid,
                                    OI_BYTE_STREAM *pBs,
                                    OI_UINT8        transaction,
                                    OI_UINT8        msgType)
{
    OI_UINT8 acpSEID = 0;
    OI_AVDTP_CAP *caps = NULL;
    OI_UINT8 numHandles;
    OI_UINT8 numCaps = 0;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    AVDTP_ENDPOINT *endpoint;

    OI_DBGPRINT(("Received GET CONFIG - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            endpoint = GetEndpointBySignalCID(handles[0]);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                result = ParseCaps(pBs, &caps, &numCaps);
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->getConfigCfm(transaction, cid, caps, numCaps, result);
        OI_AVDTP_FreeCaps(&caps, numCaps);
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {

        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
        }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->getConfigCfm(transaction, cid, NULL, 0, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        GetSEID_Checked(*pBs, acpSEID);

        endpoint = GetEndpoint(acpSEID);
        if (NULL == endpoint) {
            result = OI_AVDTP_ERR_ACP_SEID;
        }

        OI_AVDTP_TestPrint(("AVDT_GetConfiguration_Rsp\n\tTransaction=%d\n\tStreamHandle=%d\n\tError=0x%.2x\n\tConfigParams=\n",
                            transaction,
                            cid,
                            result));

        /* *Always* respond to an GetConfig command */
        if (OI_AVDTP_SUCCESS == result) {
            OI_AVDTP_TestPrintCaps(endpoint->configCaps, endpoint->numConfigCaps);
            AVDTPSignal_GetConfigRsp(cid, transaction, endpoint->configCaps, endpoint->numConfigCaps, OI_AVDTP_SUCCESS);
        } else {
            AVDTPSignal_GetConfigRsp(cid, transaction, NULL, 0, result);
        }

        OI_AVDTP_TestPrint(("AVDT_GetConfiguration_Rsp\n\tRSP=0x0000\n"));
    }
    return result;
}

static OI_UINT8 AVDTPRecv_Open(OI_L2CAP_CID    cid,
                               OI_BYTE_STREAM *pBs,
                               OI_UINT8        transaction,
                               OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    AVDTP_ENDPOINT *endpoint = NULL;

    OI_DBGPRINT(("Received OPEN - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        OI_STATUS status;

        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            endpoint = GetEndpointBySignalCID(handles[0]);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID; //can't repond w/ error in Multicast
            } else {
                status = CreateMediaTransport(cid);
                if (OI_SUCCESS(status)) {
                    SetStates(handles, 1, AVDTP_STATE_OPEN);
                } else {
                    result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
                }
            }
        }

        /* *Only* pass up here if we are unsuccessful, because the real
         * notification to the application will occur when the Media
         * Transport has been successfully established (or not)
         */
        if( result != OI_AVDTP_SUCCESS ){
            avdtp->callbacks->openCfm(transaction, cid, 0, result);
        }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        ByteStream_GetUINT8_Checked(*pBs, result);
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            if (ByteStream_Error(*pBs)) {
                result =  OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->openCfm(transaction, cid, 0, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        OI_UINT8 seid = 0;

        GetSEID_Checked(*pBs, seid);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        } else {
            endpoint = GetEndpoint(seid);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                if (endpoint->state != AVDTP_STATE_CONFIGURED) {
                    result = OI_AVDTP_ERR_STATE;
                } else {
                    SetState(endpoint, AVDTP_STATE_OPEN);
                }
            }
        }

        OI_AVDTP_TestPrint(("AVDT_Open_Rsp\n\tTransaction=%d\n\tStreamHandle=%d\n\tError=0x%.2x\n",
                            transaction,
                            cid,
                            result));
        OI_AVDTP_TestPrint(("AVDT_Open_Rsp\n\tRSP=0x0000\n"));

        if ( endpoint ){
            if( endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO ){
                /* Only call OpenInd if we will be RXing a broadcast stream */
                OI_L2CAP_EnableCLT(OI_PSM_AVDTP, MediaRecvBroadcastDataInd);
                OI_L2CAP_EnableCLT(OI_PSM_AVDTP | 0x80, MediaRecvBroadcastDataInd);
                avdtp->callbacks->openInd(cid, 339 - AVDTP_MEDIA_HEADER_LEN);
            }
        }
        AVDTPSignal_OpenRsp(cid, transaction, result);
    }
    return result;
}

static OI_UINT8 AVDTPRecv_Start(OI_L2CAP_CID    cid,
                                OI_BYTE_STREAM *pBs,
                                OI_UINT8        transaction,
                                OI_UINT8        msgType)
{
    OI_UINT8 seid = 0;
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;

    OI_DBGPRINT(("Received START - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);
    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            SetStates(handles, numHandles, AVDTP_STATE_STREAMING);
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->startCfm(transaction, handles, numHandles, 0, result);
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        if (0 == numHandles) {
            result =  OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            GetSEID_Checked(*pBs, seid);
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result =  OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->startCfm(transaction, handles, numHandles, seid, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        AVDTP_ENDPOINT *endpoint;
        OI_UINT8 i;

        numHandles = ByteStream_NumReadBytesAvail(*pBs);
        if (numHandles > AVDTP_MAX_ENDPOINTS) {
            result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
        } else if (0 == numHandles) {
            result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
        } else {
            for (i = 0; i < numHandles; i++) {
                GetSEID_Checked(*pBs, seid);
                if (ByteStream_Error(*pBs)) {
                    result = OI_AVDTP_ERR_LENGTH;
                } else {
                    endpoint = GetEndpoint(seid);
                    if (NULL == endpoint) {
                        result = OI_AVDTP_ERR_ACP_SEID;
                    } else {
                        if (endpoint->state != AVDTP_STATE_OPEN) {
                            result = OI_AVDTP_ERR_STATE;
                        }
                    }
                }
                handles[i] = cid;
            }
        }

        AVDTPSignal_StartRsp(cid, transaction, 0, result);

        OI_AVDTP_TestPrint(("AVDT_Start_Ind\n\tTransaction=%d\n\tStreamHandles=", transaction));
        OI_AVDTP_TestPrintHandles(handles, numHandles);
        OI_AVDTP_TestPrint(("AVDT_Start_Rsp\n\tTransaction=%d\n\tErrorCode=0x%.2x\n\tStreamHandles=", transaction, result));
        OI_AVDTP_TestPrintHandles(handles, numHandles);
        OI_AVDTP_TestPrint(("AVDT_Start_Rsp\n\tRSP=0x0000\n"));

        if (OI_AVDTP_SUCCESS == result) {
            SetStates(handles, numHandles, AVDTP_STATE_STREAMING);
            avdtp->callbacks->startInd(transaction, handles, numHandles);
        }
    }
    return result;
}


static OI_UINT8 AVDTPRecv_Close(OI_L2CAP_CID    cid,
                                OI_BYTE_STREAM *pBs,
                                OI_UINT8        transaction,
                                OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    AVDTP_ENDPOINT *endpoint = NULL;

    OI_DBGPRINT(("Received CLOSE - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {

            SetStates(handles, 1, AVDTP_STATE_CLOSING);

            endpoint = GetEndpointBySignalCID(handles[0]);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                OI_STATUS status = OI_OK;

                OI_DBGPRINT2(("Disconnecting media transport"));
                if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
                    AVDTP_REMOTE_ENDPOINT *remote;

                    remote = GetRemoteEndpoint(cid);
                    if( remote != NULL ){
                        /* Remove device from Broadcast media transport */
                        status = OI_L2CAP_GroupRemoveMember(endpoint->mediaCID, &remote->addr );
                    } else {
                        status = OI_FAIL;
                    }

                } else {
                    /* Close Point-to-Point media transport */
                    status = OI_L2CAP_Disconnect(endpoint->mediaCID);
                }
                if (!OI_SUCCESS(status)) {
                    OI_LOG_ERROR(("Error disconnecting media transport - %!", result));
                    ResetEndpoint(endpoint);
                    result = OI_AVDTP_ERR_STATE;
                }
            }
        }

        /* *Only* respond here if we are unsuccessful or Multicast, because the real
         * notification to the application will occur when the Media
         * Transport has been successfully closed (or not)
         */
        if( result != OI_AVDTP_SUCCESS ){
            avdtp->callbacks->closeCfm(transaction, cid, result);
        } else if (endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO){
            ResetEndpoint(endpoint);
            avdtp->callbacks->closeCfm(transaction, cid, result);
        }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass a response up to app */
        avdtp->callbacks->closeCfm(transaction, cid, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        OI_UINT8 seid = 0;

        GetSEID_Checked(*pBs, seid);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        } else {
            endpoint = GetEndpoint(seid);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                if ( (endpoint->state != AVDTP_STATE_STREAMING) && (endpoint->state != AVDTP_STATE_OPEN) ) {
                    result = OI_AVDTP_ERR_STATE;
                } else {
                    if ( (endpoint->mediaCID == 0) ||
                         (endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO) ){
                        /* No Media Transport, Reset immediately */
                        ResetEndpoint(endpoint);
                    } else {
                        SetState(endpoint, AVDTP_STATE_CLOSING);
                    }
                }
            }
        }

        AVDTPSignal_CloseRsp(cid, transaction, result);

        OI_AVDTP_TestPrint(("AVDT_Close_Rsp\n\tTransaction=%d\n\tStreamHandle=%d\n\tErrorCode=0x%.2x\n",
                            transaction,
                            cid,
                            result));

        if (OI_AVDTP_SUCCESS == result) {
            avdtp->callbacks->closeInd(transaction, cid);
        }
    }
    return result;
}



static OI_UINT8 AVDTPRecv_Suspend(OI_L2CAP_CID    cid,
                                  OI_BYTE_STREAM *pBs,
                                  OI_UINT8        transaction,
                                  OI_UINT8        msgType)
{
    OI_UINT8 seid = 0;
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;

    OI_DBGPRINT(("Received SUSPEND - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            SetStates(handles, numHandles, AVDTP_STATE_OPEN);
        }

        /* *Always* pass the response up to app */
        if (NULL != avdtp->callbacks->suspendCfm){
            avdtp->callbacks->suspendCfm(transaction, handles, numHandles, 0, result);
        }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            GetSEID_Checked(*pBs, seid);
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass the response up to app */
        if (NULL != avdtp->callbacks->suspendCfm){
            avdtp->callbacks->suspendCfm(transaction, handles, numHandles, seid, result);
        }
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        AVDTP_ENDPOINT *endpoint;
        OI_UINT8 i;

        numHandles = ByteStream_NumReadBytesAvail(*pBs);
        if (NULL == avdtp->callbacks->suspendInd){
            result = OI_AVDTP_ERR_UNSUPPORTED_COMMAND;
        }
        else if (numHandles > AVDTP_MAX_ENDPOINTS) {
            result = OI_AVDTP_ERR_INSUFFICIENT_RESOURCE;
        }
        else if (0 == numHandles) {
            result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
        } else {
            for (i = 0; i < numHandles; i++) {
                GetSEID_Checked(*pBs, seid);
                handles[i] = cid;
                if (ByteStream_Error(*pBs)) {
                    result = OI_AVDTP_ERR_LENGTH;
                } else {
                    endpoint = GetEndpoint(seid);
                    if (NULL == endpoint) {
                        result = OI_AVDTP_ERR_ACP_SEID;
                    } else {
                        if (endpoint->state != AVDTP_STATE_STREAMING) {
                            result = OI_AVDTP_ERR_STATE;
                        }
                    }
                }
            }
        }

        OI_AVDTP_TestPrint(("AVDT_Suspend_Rsp\n\tTransaction=%d\n\tErrorCode=0x%.2x\n\tStreamHandles=", transaction, result));
        OI_AVDTP_TestPrintHandles(handles, numHandles);
        OI_AVDTP_TestPrint(("AVDT_Suspend_Rsp\n\tRSP=0x0000\n"));

        AVDTPSignal_SuspendRsp(cid, transaction, 0, result);

        if (OI_AVDTP_SUCCESS == result) {
            SetStates(handles, numHandles, AVDTP_STATE_OPEN);
            avdtp->callbacks->suspendInd(transaction, handles, numHandles);
        }
    }
    return result;
}


/** Abort processing is handled slightly differently than most Cmd-Rsp
 *  transactions. ABORT commands are not suppose to be rejected, but they also
 *  cannot be accepted for bad SEID values. Every effort needs to be taken to
 *  accept ABORT commands, but also to continue to operate in a known state if
 *  either the command is dropped in transit, or the Accept is dropped in
 *  transit.
 */
static OI_UINT8 AVDTPRecv_Abort(OI_L2CAP_CID    cid,
                                OI_BYTE_STREAM *pBs,
                                OI_UINT8        transaction,
                                OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;
    AVDTP_ENDPOINT *endpoint = NULL;
    AVDTP_REMOTE_ENDPOINT *remote;

    OI_DBGPRINT(("Received ABORT - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {

            SetStates(handles, numHandles, AVDTP_STATE_ABORTING);

            /* Find a valid Id, or die trying */
            while ( (NULL == endpoint) && numHandles ){
                endpoint = GetEndpointBySignalCID(handles[--numHandles]);
            }

            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                OI_STATUS status;

                OI_DBGPRINT2(("Disconnecting media transport"));
                if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){

                    remote = GetRemoteEndpoint(handles[0]);
                    if( remote != NULL){
                        /* Remove device from Broadcast media transport */
                        status = OI_L2CAP_GroupRemoveMember(endpoint->mediaCID, &remote->addr );
                    }

                    /* It is legal to abort an endpoint from any state, so
                     * pass up successful abort response, even if there was no
                     * media transport to tear down.
                     */
                    ResetEndpoint( GetEndpointBySignalCID(cid) );
                    avdtp->callbacks->abortCfm(transaction, cid, OI_AVDTP_SUCCESS);
                }
                else if (endpoint->mediaCID == 0) {
                    /* Nothing to disconnect if no MediaCID */
                    ResetEndpoint( GetEndpointBySignalCID(cid) );
                    avdtp->callbacks->abortCfm(transaction, cid, OI_AVDTP_SUCCESS);
                } else {
                    /* Close Point-to-Point media transport */
                    status = OI_L2CAP_Disconnect(endpoint->mediaCID);
                    if (!OI_SUCCESS(status)) {
                        OI_LOG_ERROR(("Error disconnecting media transport - %!", result));
                        endpoint->mediaCID = 0;
                        ResetEndpoint( GetEndpointBySignalCID(cid) );
                        avdtp->callbacks->abortCfm(transaction, cid, OI_AVDTP_SUCCESS);
                    }
                }
            }
        }

        /* *Only* respond here if we are unsuccessful, because the real
         * notification to the application will occur when the Media
         * Transport has been successfully (or not) closed.
         */
        if (result != OI_AVDTP_SUCCESS) {
            OI_LOG_ERROR(("Abort Accept RXed for unknown endpoint. transaction: %d", transaction));
            ResetEndpoint( GetEndpointBySignalCID(cid) );
            avdtp->callbacks->abortCfm(transaction, cid, OI_AVDTP_SUCCESS);

        }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {

        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }


        /* Only internaly generated ABORT Time-out messages should ever arrive here */
        if (OI_AVDTP_ERR_NO_RESPONSE != result) {
            OI_LOG_ERROR(("Remote Abort Reject RXed (illegal)"));
        }

        /* *Always* pass the response up to app */
        ResetEndpoint( GetEndpointBySignalCID(cid) );
        avdtp->callbacks->abortCfm(transaction, cid, result);
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        OI_UINT8 seid = 0;

        GetSEID_Checked(*pBs, seid);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        } else {
            endpoint = GetEndpoint(seid);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            }

            if (OI_AVDTP_SUCCESS == result) {
                /* The spec says that no reject responses shall be sent. */
                AVDTPSignal_AbortRsp(cid, transaction, OI_AVDTP_SUCCESS);

                OI_AVDTP_TestPrint(("AVDT_Abort_Rsp\n\tTransaction=%d\n\tErrorCode=0x%.2x\n\tStreamHandles=%d\n",
                                    transaction,
                                    result,
                                    cid));
                OI_AVDTP_TestPrint(("AVDT_Abort_Rsp\n\tTransaction=%d\n\tStreamHandle=%d\n\tErrorCode=0x%.2x\n",
                                    transaction,
                                    cid,
                                    result));

                if ( (endpoint->mediaCID == 0) ||
                     (endpoint->mediaType == OI_AVDTP_MEDIA_TYPE_MULTICAST_AUDIO) ){
                    /* No Media Transport, Reset immediately */
                    ResetEndpoint(endpoint);
                } else {
                    /* Wait for Media Transport to be torn down */
                    SetState(endpoint, AVDTP_STATE_ABORTING);
                }

#ifdef OI_CODE
#error code not present
#endif
            }
        }
    }
    return result;
}

static OI_UINT8 AVDTPRecv_SecurityControl(OI_L2CAP_CID    cid,
                                          OI_BYTE_STREAM *pBs,
                                          OI_UINT8        transaction,
                                          OI_UINT8        msgType)
{
    OI_UINT16 handles[AVDTP_MAX_ENDPOINTS];
    OI_UINT8 numHandles;
    OI_UINT8 result = OI_AVDTP_SUCCESS;

    OI_DBGPRINT(("Received SECURITY CONTROL - %s", MsgTypeText(msgType)));

    OI_ASSERT(pBs);

    GetHandlesByTransaction(transaction, handles, &numHandles);

    if (AVDTP_MSG_TYPE_RSP_ACCEPT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        }

        /* *Always* pass the response up to app */
        if (NULL != avdtp->callbacks->securityCfm){
            avdtp->callbacks->securityCfm(transaction, cid, ByteStream_GetCurrentBytePointer(*pBs),
                              (OI_UINT16)ByteStream_NumReadBytesAvail(*pBs), result);
    }
    }
    else if (AVDTP_MSG_TYPE_RSP_REJECT == msgType) {
        if (0 == numHandles) {
            result = OI_AVDTP_ERR_HEADER_FORMAT;
        } else {
            ByteStream_GetUINT8_Checked(*pBs, result);
            if (ByteStream_Error(*pBs)) {
                result = OI_AVDTP_ERR_LENGTH;
            }
            if (OI_AVDTP_SUCCESS == result) {
                result = OI_AVDTP_ERR_PAYLOAD_FORMAT;
            }
        }

        /* *Always* pass the response up to app */
        if (NULL != avdtp->callbacks->securityCfm){
            avdtp->callbacks->securityCfm(transaction, cid, NULL, 0, result);
        }
    }
    else if (AVDTP_MSG_TYPE_COMMAND == msgType) {
        OI_UINT8 seid = 0;
        AVDTP_ENDPOINT *endpoint;

        if (NULL == avdtp->callbacks->securityInd){
            result = OI_AVDTP_ERR_UNSUPPORTED_COMMAND;
        } else {
        GetSEID_Checked(*pBs, seid);
        if (ByteStream_Error(*pBs)) {
            result = OI_AVDTP_ERR_LENGTH;
        } else {
            endpoint = GetEndpoint(seid);
            if (NULL == endpoint) {
                result = OI_AVDTP_ERR_ACP_SEID;
            } else {
                if (endpoint->state == AVDTP_STATE_IDLE) {
                    result = OI_AVDTP_ERR_STATE;
                }
            }
        }
        }

        if (result != OI_AVDTP_SUCCESS) {
            /* Only respond to remote device if there was an error. If we have
             * successfully received a Security Control command, it is the
             * responsibility of the application to respond to the remote device.
             */
            OI_AVDTP_TestPrint(("AVDT_Security_Control_Rsp\n\tTransaction=%d\n\tStream Handle=%d\n\tData Length=0\n\tError=%d\n",
                                transaction,
                                cid,
                                result));

            AVDTPSignal_SecurityControlRsp(cid, transaction, NULL, 0, result);
        } else {
            /* Only pass successfully parsed Sec Control commands to application */
            avdtp->callbacks->securityInd(transaction, cid, ByteStream_GetCurrentBytePointer(*pBs),
                                         (OI_UINT16)ByteStream_NumReadBytesAvail(*pBs));
        }
    }
    return result;
}


typedef OI_UINT8 (*AVDTP_RECV_FUNC)(OI_L2CAP_CID    cid,
                                    OI_BYTE_STREAM *pBs,
                                    OI_UINT8        transaction,
                                    OI_UINT8        msgType);

void AVDTPRecv_Complete(OI_L2CAP_CID cid,
                        OI_BYTE     *data,
                        OI_UINT16    dataLen)
{
    OI_BYTE_STREAM bs;
    OI_BYTE header = 0;
    OI_BYTE identifier = 0;
    OI_BYTE msgType;
    OI_BYTE transaction = 0xFF;
    AVDTP_RECV_FUNC f;
    OI_UINT8 result;

    ByteStream_Init(bs, data, dataLen);
    ByteStream_Open(bs, BYTESTREAM_READ);
    ByteStream_GetUINT8_Checked(bs, header);

    if (!ByteStream_Error(bs)) {
        transaction = (header & AVDTP_TRANSACTION_MASK) >> 4;
        OI_DBGPRINT2(("transaction = %d", transaction));
    }

    ByteStream_GetUINT8_Checked(bs, identifier);

    if (ByteStream_Error(bs)) {
        /* Send a general reject error if complete header not received */
        OI_LOG_ERROR(("Error parsing AVDTP Signal header"));
        if (0xFF != transaction){
            /* Can't risk rejecting a transaction we mangled */
            AVDTP_GeneralReject(cid, transaction);
        }
        return;
    }

    msgType = header & AVDTP_MSG_TYPE_MASK;
    if (msgType == AVDTP_MSG_TYPE_RFD) {
        OI_LOG_ERROR(("Invalid message type! (RFD)"));
        return;
    }

    if (msgType != AVDTP_MSG_TYPE_COMMAND) {
        /* Response packet, note transaction ID */
        AVDTP_CancelTimeout(cid, transaction);
    }

    switch (identifier) {
        case AVDTP_DISCOVER:
            f = AVDTPRecv_Discover;
            break;
        case AVDTP_GET_CAPS:
            f = AVDTPRecv_GetCaps;
            break;
        case AVDTP_SET_CONFIG:
            f = AVDTPRecv_SetConfig;
            break;
        case AVDTP_GET_CONFIG:
            f = AVDTPRecv_GetConfig;
            break;
        case AVDTP_RECONFIGURE:
            f = AVDTPRecv_Reconfigure;
            break;
        case AVDTP_OPEN:
            f = AVDTPRecv_Open;
            break;
        case AVDTP_START:
            f = AVDTPRecv_Start;
            break;
        case AVDTP_CLOSE:
            f = AVDTPRecv_Close;
            break;
        case AVDTP_SUSPEND:
            f = AVDTPRecv_Suspend;
            break;
        case AVDTP_ABORT:
            f = AVDTPRecv_Abort;
            break;
        case AVDTP_SECURITY_CONTROL:
            f = AVDTPRecv_SecurityControl;
            break;
        case AVDTP_GENERAL_REJECT:
            if (AVDTP_MSG_TYPE_COMMAND == msgType) {
                OI_LOG_ERROR(("Unrecognized command %d!", identifier));
                AVDTP_GeneralReject(cid, transaction);
            } else {
                OI_DBGPRINT2(("Got general reject message"));
            }
            return;
        default:
            OI_LOG_ERROR(("Unrecognized command %d!", identifier));
            AVDTP_GeneralReject(cid, transaction);
            return;
    }

    result = f(cid, &bs, transaction, msgType);
    if ((result != OI_AVDTP_SUCCESS)) {
        OI_LOG_ERROR(("Error on receipt - %d", result));
    }
}


#ifdef OI_TEST
/*** Test functions ***/

#include "oi_avdtp_test.h"

static OI_CHAR *EventText(OI_UINT16 event)
{
    switch (event) {
        case AVDT_ConnectReq_Ind:
            return "AVDT_ConnectReq_Ind";
        case AVDT_ConnectReq_Cfm:
            return "AVDT_ConnectReq_Cfm";
        case AVDT_DisconnectReq_Ind:
            return "AVDT_DisconnectReq_Ind";
        case AVDT_DisconnectReq_Cfm:
            return "AVDT_DisconnectReq_Cfm";
        case AVDT_Discover_Ind:
            return "AVDT_Discover_Ind";
        case AVDT_Discover_Cfm:
            return "AVDT_Discover_Cfm";
        case AVDT_Get_Capabilities_Ind:
            return "AVDT_Get_Capabilities_Ind";
        case AVDT_Get_Capabilities_Cfm:
            return "AVDT_Get_Capabilities_Cfm";
        case AVDT_Set_Configuration_Ind:
            return "AVDT_Set_Configuration_Ind";
        case AVDT_Set_Configuration_Cfm:
            return "AVDT_Set_Configuration_Cfm";
        case AVDT_Get_Configuration_Ind:
            return "AVDT_Get_Configuration_Ind";
        case AVDT_Get_Configuration_Cfm:
            return "AVDT_Get_Configuration_Cfm";
        case AVDT_Open_Ind:
            return "AVDT_Open_Ind";
        case AVDT_Open_Cfm:
            return "AVDT_Open_Cfm";
        case AVDT_Close_Ind:
            return "AVDT_Close_Ind";
        case AVDT_Close_Cfm:
            return "AVDT_Close_Cfm";
        case AVDT_Start_Ind:
            return "AVDT_Start_Ind";
        case AVDT_Start_Cfm:
            return "AVDT_Start_Cfm";
        case AVDT_Suspend_Ind:
            return "AVDT_Suspend_Ind";
        case AVDT_Suspend_Cfm:
            return "AVDT_Suspend_Cfm";
        case AVDT_ReConfigure_Ind:
            return "AVDT_ReConfigure_Ind";
        case AVDT_ReConfigure_Cfm:
            return "AVDT_ReConfigure_Cfm";
        case AVDT_Security_Control_Ind:
            return "AVDT_Security_Control_Ind";
        case AVDT_Security_Control_Cfm:
            return "AVDT_Security_Control_Cfm";
        case AVDT_Abort_Ind:
            return "AVDT_Abort_Ind";
        case AVDT_Abort_Cfm:
            return "AVDT_Abort_Cfm";
        default:
            return "Unknown";
    }
}

OI_STATUS AVDT_Sig_Event_Registration(OI_UINT16 event)
{
    OI_AVDTP_TestPrint(("AVDT_Sig_Event_Registration\n\tEvent=0x%.4x\n\tCallback=%s\n",
                        event,
                        EventText(event)));
    OI_AVDTP_TestPrint(("AVDT_EventRegistration\n\tResult=0x0000\n"));
    return OI_OK;
}
#endif

#endif /* FEATURE_BT_EXTPF_AV */

