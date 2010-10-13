#ifdef FEATURE_BT_EXTPF_CTP
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/tcs_msg.c#1 $ 

$Log: $
*
*    #4        14 Mar 2006            RY
* Added proper featurization.
*
*    #3        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
===========================================================================*/

/**
 * @file  
 *
 * functions for sending TCS messages
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_TCS

#include "oi_tcs_spec.h"
#include "oi_tcs_msg.h"
#include "oi_tcs_cc.h"
#include "oi_tcs_coding.h"
#include "oi_tcs_helper.h"

#include "oi_bytestream.h"
#include "oi_assert.h"
#include "oi_debug.h"
#include "oi_text.h"
#include "oi_status.h"

static void enqueue(TCS_PACKET *packet) 
{
    TCS_PACKET *current;

    if (NULL == OI_Tcs_Data->queue) {
        OI_Tcs_Data->queue = packet;
        return;
    }

    current = OI_Tcs_Data->queue;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = packet;
}

static TCS_PACKET *dequeue(void)
{
    TCS_PACKET *current;

    current = OI_Tcs_Data->queue;
    if (OI_Tcs_Data->queue != NULL) {
        OI_Tcs_Data->queue = OI_Tcs_Data->queue->next;
    }
    return current;
}

static void writeCB(OI_L2CAP_CID cid,
                    OI_UINT8 *buf,
                    OI_UINT16 sendCount,
                    OI_STATUS result)
{
    TCS_PACKET *packet;
    OI_STATUS status = OI_OK;

    OI_Free(buf);

    do {
        packet = dequeue();
        if (packet != NULL) {
            status = OI_L2CAP_Write(writeCB, packet->cid, packet->buf, packet->bufLen);
            if (OI_SUCCESS(status)) {
                OI_Tcs_StartTimer(OI_Tcs_GetCallByCID(packet->cid), packet->timerType);
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("L2CAP_Write failed in write cb"));
#endif
                OI_Free(packet->buf);
            }
            OI_Free(packet);
        }
    } while ((packet != NULL) && !OI_SUCCESS(status));
}

/**
 * Compare two bearer capabilities to ensure that they are equal.
 *
 * @param a     First bearer to compare
 * @param b     Second bearer to compare
 */
static OI_BOOL CompareBearer(OI_TCS_BEARER *a, OI_TCS_BEARER *b)
{
    OI_BOOL result = FALSE;

    if (a && b) {
        if (a->type != b->type) {
            result = FALSE;
        } else if (OI_TCS_BEARER_LINK_TYPE_SCO == a->type) {
            result = (0 == OI_MemCmp(&a->info.sco, &b->info.sco, sizeof(OI_TCS_BEARER_SCO)));
        } else if (OI_TCS_BEARER_LINK_TYPE_ACL == a->type) {
            /*
             * Must compare flowspec structures in ACL link type.  If one is NULL, assume that
             * it is equivalent to all parameters being 0.
             */
            if (a->info.acl.flowspec && b->info.acl.flowspec) {
                result = (0 == OI_MemCmp(a->info.acl.flowspec, b->info.acl.flowspec, sizeof(OI_L2CAP_FLOWSPEC)));
            } else if (!a->info.acl.flowspec && !b->info.acl.flowspec) {
                result = TRUE;
            } else if (!a->info.acl.flowspec) {
                result = (b->info.acl.flowspec->serviceType == 0);
                result = result && (b->info.acl.flowspec->tokenRate == 0);
                result = result && (b->info.acl.flowspec->tokenBucketSize == 0);
                result = result && (b->info.acl.flowspec->peakBandwidth == 0); 
                result = result && (b->info.acl.flowspec->latency == 0);
                result = result && (b->info.acl.flowspec->delayVariation == 0);
            } else if (!b->info.acl.flowspec) {
                result = (a->info.acl.flowspec->serviceType == 0);
                result = result && (a->info.acl.flowspec->tokenRate == 0);
                result = result && (a->info.acl.flowspec->tokenBucketSize == 0);
                result = result && (a->info.acl.flowspec->peakBandwidth == 0); 
                result = result && (a->info.acl.flowspec->latency == 0);
                result = result && (a->info.acl.flowspec->delayVariation == 0);
            }
            if (result) {
                result = result && (a->info.acl.layer2 == b->info.acl.layer2);
                result = result && (a->info.acl.layer3 == b->info.acl.layer3);
            }
        } else {
            result = (a->type == b->type);
        }
    } else if (!a && !b) {
        result = TRUE;
    } else if (!a) {
        /*
         * A NULL bearer implies the default capability (SCO HV3 CVSD).
         */
        if ((OI_TCS_BEARER_LINK_TYPE_SCO == b->type) &&
            (OI_TCS_BEARER_PACKET_TYPE_HV3 == b->info.sco.packetType) &&
            (OI_TCS_BEARER_CODING_CVSD == b->info.sco.coding)) {
            result = TRUE;
        }
    } else if (!b) {
        if ((OI_TCS_BEARER_LINK_TYPE_SCO == a->type) &&
            (OI_TCS_BEARER_PACKET_TYPE_HV3 == a->info.sco.packetType) &&
            (OI_TCS_BEARER_CODING_CVSD == a->info.sco.coding)) {
            result = TRUE;
        }
    }
    return result;
}

/** 
 * Clear all the non-selected users in a multpoint call.
 *
 * @param selected  The selected call.
 */
OI_STATUS OI_Tcs_ClearAllNonSelected(TCS_CALL_INFO *selected)
{
    OI_STATUS result = OI_OK;
    TCS_CALL_INFO *call;
    OI_UINT i;

    OI_ASSERT(OI_Tcs_Data->role == OI_TCS_ROLE_GATEWAY);

    for (i = 0; i < OI_Tcs_Data->numCalls; i++) {
        call = &OI_Tcs_Data->calls[i];
        if (!IsCallFree(call) && (call != selected)) {
            /*
             * Always cancel the timer associated with this call.  If they responded
             * to us, then we must also send a RELEASE message, otherwise we just
             * reset the internal state to NULL.
             */
            OI_Tcs_CancelTimer(call);
            if (TCS_STATE_CALL_INITIATED == call->state) {
                OI_DBGPRINT2(("Clearing (all) non-selected user %:\n", &call->addr));
                SetCallState(call, TCS_STATE_NULL);
            } else if (TCS_STATE_OVERLAP_SENDING == call->state ||
                       TCS_STATE_OUTGOING_CALL_PROCEEDING == call->state ||
                       TCS_STATE_CALL_DELIVERED == call->state ||
                       TCS_STATE_CONNECT_RECEIVED == call->state ||
                       TCS_STATE_ACTIVE == call->state) {
                OI_DBGPRINT2(("Clearing (all) non-selected user %:\n", &call->addr));
                result = OI_Tcs_Send_Release(call, OI_TCS_CAUSE_NONSELECTED_USER_CLEARING, 0);
                if (OI_SUCCESS(result)) {
                    SetCallState(call, TCS_STATE_RELEASE_REQ);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("Send RELEASE failed"));
#endif
                }
            }
        }
    }
    return result;
}

/** 
 * Clear a non-selected user in a multpoint call.
 *
 * @param nonselected  The non-selected user.
 */
static OI_STATUS ClearNonSelected(TCS_CALL_INFO *nonselected)
{
    OI_STATUS result;

    OI_DBGPRINT2(("Clearing non-selected user %:\n", &nonselected->addr));
    OI_ASSERT(OI_Tcs_Data->role == OI_TCS_ROLE_GATEWAY);
    OI_Tcs_CancelTimer(nonselected);
    result = OI_Tcs_Send_Release(nonselected, OI_TCS_CAUSE_NONSELECTED_USER_CLEARING, 0);
    if (OI_SUCCESS(result)) {
        OI_Tcs_StartTimer(nonselected, TCS_T308);
        SetCallState(nonselected, TCS_STATE_RELEASE_REQ);
    }
    return result;
}

static void ElemsInit(TCS_ELEMS *elems) 
{
    OI_ASSERT(elems != NULL);

    elems->audioControl = OI_TCS_AUDIO_CONTROL_NONE;
    elems->bearer = NULL;
    elems->class = OI_TCS_CALL_CLASS_NONE;
    elems->calledNumber = NULL;
    elems->callingNumber = NULL;
    elems->cause = OI_TCS_CAUSE_NONE;
    elems->offset = TCS_CLOCK_OFFSET_INVALID;
    elems->config = NULL;
    elems->destCID = 0;
    elems->keypadFacility = (OI_CHAR)OI_TCS_KEYPAD_FACILITY_NONE;
    elems->progress = OI_TCS_PROGRESS_NONE;
    elems->handle = 0;
    elems->signal = OI_TCS_SIGNAL_NONE;
    elems->sendingComplete = FALSE;
    elems->companySpecific = NULL;
    elems->companySpecificSize = 0;
}

/** Send a packet as specified by the fields set in the TCS_ELEMS structure. */
static OI_STATUS OI_Tcs_Send_Generic(TCS_CALL_INFO *call,
                                     TCS_ELEMS *elems, 
                                     TCS_MSG type,
                                     OI_UINT timerType) 
{
    OI_BYTE_STREAM bs;
    OI_BYTE *buf;
    OI_STATUS result;
    TCS_PACKET *packet;
    OI_L2CAP_CID cid;
    OI_UINT maxMessageSize;

    if (!call) {
        maxMessageSize = TCS_MAX_MESSAGE_SIZE;
    } else {
        maxMessageSize = OI_MIN(TCS_MAX_MESSAGE_SIZE, call->outMTU);
    }
    buf = OI_Malloc(maxMessageSize);
    if (NULL == buf) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    ByteStream_Init(bs, buf, maxMessageSize);
    ByteStream_Open(bs, BYTESTREAM_WRITE);

    ByteStream_PutUINT8(bs, type);

    if (NULL == elems) {
        goto Done;
    }

    //Important! - the order of these elements is significant (and is not logical or well specified)

    result = OI_OK;
    if (elems->class != OI_TCS_CALL_CLASS_NONE) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_CALL_CLASS, (OI_UINT8)elems->class);
    }
    if (OI_SUCCESS(result) && (TRUE == elems->sendingComplete)) {
        result = OI_Tcs_EncodeSingle(&bs, TCS_ELEM_SENDING_COMPLETE);
    }
    if (OI_SUCCESS(result) && (elems->keypadFacility != OI_TCS_KEYPAD_FACILITY_NONE)) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_KEYPAD_FACILITY, elems->keypadFacility);
    }
    if (OI_SUCCESS(result) && ((TCS_CLOCK_OFFSET_VALID & elems->offset) != 0)) {
        TCS_CLOCK_OFFSET tempOffset = ~TCS_CLOCK_OFFSET_VALID & elems->offset; //Don't send the top bit
        result = OI_Tcs_EncodeVariable(&bs, TCS_ELEM_CLOCK_OFFSET, (OI_UINT8 *)&tempOffset, 2);
    }
    if (OI_SUCCESS(result) && (elems->config != NULL)) {
        result = OI_Tcs_EncodeConfig(&bs, elems->config);
    }
    if (OI_SUCCESS(result) && (elems->bearer != NULL)) {
        result = OI_Tcs_EncodeBearer(&bs, elems->bearer);
    }
    if (OI_SUCCESS(result) && (elems->signal != 0)) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_SIGNAL, (OI_UINT8)elems->signal);
    }
    if (OI_SUCCESS(result) && (elems->callingNumber != NULL)) {
        result = OI_Tcs_EncodeNumber(&bs, elems->callingNumber);
    }
    if (OI_SUCCESS(result) && (elems->calledNumber != NULL)) {
        result = OI_Tcs_EncodeNumber(&bs, elems->calledNumber);
    }
    if (OI_SUCCESS(result) && (elems->audioControl != OI_TCS_AUDIO_CONTROL_NONE)) {
        result = OI_Tcs_EncodeVariable(&bs, TCS_ELEM_AUDIO_CONTROL, (OI_UINT8 *)&elems->audioControl, 1);
    }
    if (OI_SUCCESS(result) && (elems->cause != 0)) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_CAUSE, (OI_UINT8)elems->cause);
    }
    if (OI_SUCCESS(result) && (elems->progress != 0)) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_PROGRESS_INDICATOR, (OI_UINT8)elems->progress);
    }
    if (OI_SUCCESS(result) && (elems->handle != 0)) {
        result = OI_Tcs_EncodeDouble(&bs, TCS_ELEM_SCO_HANDLE, (OI_UINT8)elems->handle);
    }
    if (OI_SUCCESS(result) && (elems->destCID != NULL)) {
        result = OI_Tcs_EncodeVariable(&bs, TCS_ELEM_DEST_CID, (OI_UINT8 *)&elems->destCID, 2);
    }
    if (OI_SUCCESS(result) && (elems->companySpecific != NULL)) {
        result = OI_Tcs_EncodeVariable(&bs, TCS_ELEM_COMPANY_SPECIFIC, elems->companySpecific, elems->companySpecificSize);
    }

    if (!OI_SUCCESS(result)) {
        OI_Free(buf);
        return result;
    }

Done:
    ByteStream_Close(bs);
    OI_Tcs_DumpMsg("Send ", call ? &call->addr : NULL, (OI_UINT8)type, elems);

    if (!call) {
        cid = OI_Tcs_Data->groupCID;
    } else {
        cid = call->cid;
    }
    result = OI_L2CAP_Write(writeCB, cid, ByteStream_GetDataPointer(bs), ByteStream_GetSize(bs));
    if (OI_SUCCESS(result)) {
        OI_Tcs_StartTimer(call, timerType);
    } else if (OI_STATUS_WRITE_IN_PROGRESS == result) {
        packet = OI_Malloc(sizeof(TCS_PACKET));
        if (!packet) {
            return OI_STATUS_OUT_OF_MEMORY;
        }

        packet->buf = ByteStream_GetDataPointer(bs);
        packet->bufLen = ByteStream_GetSize(bs);
        packet->next = NULL;
        packet->cid = cid;
        packet->timerType = timerType;
        enqueue(packet);
        result = OI_OK;
    }
    return result;
}

OI_STATUS OI_Tcs_Send_Alerting(TCS_CALL_INFO *call,
                               OI_TCS_BEARER *bearer,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID,
                               OI_UINT8 *companySpecific,
                               OI_UINT8 companySpecificSize) 
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.bearer = bearer;
    elems.progress = progress;
    elems.handle = handle;
    elems.destCID = destCID;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_ALERTING, TCS_TNULL);
}



OI_STATUS OI_Tcs_Send_CallProceeding(TCS_CALL_INFO *call,
                                     OI_TCS_BEARER *bearer,
                                     OI_TCS_PROGRESS progress,
                                     SCO_HANDLE handle,
                                     OI_L2CAP_CID destCID,
                                     OI_UINT8 *companySpecific,
                                     OI_UINT8 companySpecificSize)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.bearer = bearer;
    elems.progress = progress;
    elems.handle = handle;
    elems.destCID = destCID;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_CALL_PROCEEDING, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_Connect(TCS_CALL_INFO *call,
                              OI_TCS_BEARER *bearer,
                              SCO_HANDLE handle) 
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.bearer = bearer;
    elems.handle = handle;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_CONNECT, TCS_T313);
}

OI_STATUS OI_Tcs_Send_ConnectAck(TCS_CALL_INFO *call, 
                                 SCO_HANDLE handle,
                                 OI_L2CAP_CID destCID)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.handle = handle;
    elems.destCID = destCID;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_CONNECT_ACK, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_Disconnect(TCS_CALL_INFO *call,
                                 OI_TCS_CAUSE cause,
                                 OI_TCS_PROGRESS progress,
                                 SCO_HANDLE handle,
                                 OI_L2CAP_CID destCID)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.cause = cause;
    elems.progress = progress;
    elems.handle = handle;
    elems.destCID = destCID;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_DISCONNECT, TCS_T305);
}


OI_STATUS OI_Tcs_Send_Information(TCS_CALL_INFO *call,
                                  OI_BOOL sendingComplete,
                                  OI_CHAR keypadFacility,
                                  OI_TCS_NUMBER *callingNumber,
                                  OI_TCS_NUMBER *calledNumber,
                                  OI_TCS_AUDIO_CONTROL audioControl,
                                  OI_UINT8 *companySpecific,
                                  OI_UINT8 companySpecificSize,
                                  OI_UINT timerType)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.sendingComplete = sendingComplete;
    elems.keypadFacility = keypadFacility;
    elems.callingNumber = callingNumber;
    elems.calledNumber = calledNumber;
    elems.audioControl = audioControl;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_INFORMATION, timerType);
}

OI_STATUS OI_Tcs_Send_Progress(TCS_CALL_INFO *call,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.progress = progress;
    elems.handle = handle;
    elems.destCID = destCID;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_PROGRESS, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_Release(TCS_CALL_INFO *call,
                              OI_TCS_CAUSE cause,
                              SCO_HANDLE handle)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.cause = cause;
    elems.handle = handle;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_RELEASE, TCS_T308);
}

OI_STATUS OI_Tcs_Send_ReleaseComplete(TCS_CALL_INFO *call,
                                      OI_TCS_CAUSE cause,
                                      SCO_HANDLE handle)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.cause = cause;
    elems.handle = handle;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_RELEASE_COMPLETE, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_Setup(TCS_CALL_INFO *call,
                            OI_TCS_CALL_CLASS callClass,
                            OI_BOOL sendingComplete,
                            OI_TCS_BEARER *bearer,
                            OI_TCS_SIGNAL signal,
                            OI_TCS_NUMBER *callingNumber,
                            OI_TCS_NUMBER *calledNumber,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.class = callClass;
    elems.sendingComplete = sendingComplete;
    elems.bearer = bearer;
    elems.signal = signal;
    elems.callingNumber = callingNumber;
    elems.calledNumber = calledNumber;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_SETUP, TCS_T303);
}

OI_STATUS OI_Tcs_Send_SetupAck(TCS_CALL_INFO *call,
                               OI_TCS_BEARER *bearer,
                               OI_TCS_PROGRESS progress,
                               SCO_HANDLE handle,
                               OI_L2CAP_CID destCID,
                               OI_UINT8 *companySpecific,
                               OI_UINT8 companySpecificSize)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.bearer = bearer;
    elems.progress = progress;
    elems.handle = handle;
    elems.destCID = destCID;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_SETUP_ACK, TCS_T302);
}

OI_STATUS OI_Tcs_Send_StartDTMF(TCS_CALL_INFO *call,
                                OI_CHAR facility)
{
    TCS_ELEMS elems;

    if (facility & BIT7) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    ElemsInit(&elems);
    elems.keypadFacility = facility;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_START_DTMF, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_StartDTMFAck(TCS_CALL_INFO *call, 
                                   OI_CHAR facility)
{
    TCS_ELEMS elems;

    if (facility & BIT7) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    ElemsInit(&elems);
    elems.keypadFacility = facility;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_START_DTMF_ACK, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_StartDTMFReject(TCS_CALL_INFO *call, 
                                      OI_TCS_CAUSE cause)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.cause = cause;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_START_DTMF_REJECT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_StopDTMF(TCS_CALL_INFO *call) {

    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_STOP_DTMF, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_StopDTMFAck(TCS_CALL_INFO *call, 
                                  OI_CHAR facility)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.keypadFacility = facility;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_STOP_DTMF_ACK, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_AccessRightsRequest(TCS_CALL_INFO *call)
{
    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_ACCESS_RIGHTS_REQUEST, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_AccessRightsAccept(TCS_CALL_INFO *call)
{
    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_ACCESS_RIGHTS_ACCEPT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_AccessRightsReject(TCS_CALL_INFO *call)
{
    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_ACCESS_RIGHTS_REJECT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_InfoSuggest(TCS_CALL_INFO *call, TCS_CONFIG_DATA *config)
{    
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.config = config;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_INFO_SUGGEST, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_InfoAccept(TCS_CALL_INFO *call)
{
    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_INFO_ACCEPT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_ListenRequest(TCS_CALL_INFO *call, OI_TCS_NUMBER *calledNumber)
{
    TCS_ELEMS elems;

    //calledNumber is required
    if (NULL == calledNumber) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    ElemsInit(&elems);
    elems.calledNumber = calledNumber;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_LISTEN_REQUEST, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_ListenSuggest(TCS_CALL_INFO *call)
{
    return OI_Tcs_Send_Generic(call, NULL, TCS_MSG_LISTEN_SUGGEST, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_ListenAccept(TCS_CALL_INFO *call, TCS_CLOCK_OFFSET offset)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.offset = offset;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_LISTEN_ACCEPT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_ListenReject(TCS_CALL_INFO *call, OI_TCS_CAUSE cause)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.cause = cause;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_LISTEN_REJECT, TCS_TNULL);
}

OI_STATUS OI_Tcs_Send_CLInfo(TCS_CALL_INFO *call, 
                             OI_TCS_AUDIO_CONTROL audioControl,
                             OI_UINT8 *companySpecific,
                             OI_UINT8 companySpecificSize)
{
    TCS_ELEMS elems;

    ElemsInit(&elems);
    elems.audioControl = audioControl;
    elems.companySpecific = companySpecific;
    elems.companySpecificSize = companySpecificSize;

    return OI_Tcs_Send_Generic(call, &elems, TCS_MSG_CL_INFO, TCS_TNULL);
}



static void Recv_Alerting(TCS_CALL_INFO *call,
                          OI_TCS_BEARER *bearer,
                          OI_TCS_PROGRESS progress,
                          SCO_HANDLE handle,
                          OI_L2CAP_CID destCID,
                          OI_UINT8 *companySpecific,
                          OI_UINT8 companySpecificSize)
{
    /* 
     * In a multi-point call, there may already be an existing ACTIVE call.
     * In that case, we need to send a RELEASE message to indicate that we
     * are clearing the non selected call.
     */
    if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) && OI_Tcs_ActiveCallExists()) {
        ClearNonSelected(call);
        return;
    }

    if (TCS_STATE_CALL_INITIATED == call->state ||
        TCS_STATE_OVERLAP_SENDING == call->state ||
        TCS_STATE_OUTGOING_CALL_PROCEEDING == call->state) {

        OI_Tcs_CancelTimer(call); // T304, T303, T310
        OI_Tcs_CancelTimer(NULL);

        if (TCS_STATE_CALL_INITIATED == call->state && !CompareBearer(bearer, &call->bearer)) {
            SetCallState(call, TCS_STATE_CALL_DELIVERED);
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);
            return;
        }

        SetCallState(call, TCS_STATE_CALL_DELIVERED);
        OI_Tcs_StartTimer(call, TCS_T301);
        OI_Tcs_Data->callbacks.outgoingStatusCB(&call->addr, OI_TCS_CALL_STATUS_ALERTING, companySpecific, companySpecificSize);
    } else {
        OI_DBGPRINT2(("Unexpected ALERTING received!"));
    }    
}


static void Recv_CallProceeding(TCS_CALL_INFO *call,
                                OI_TCS_BEARER *bearer,
                                OI_TCS_PROGRESS progress,
                                SCO_HANDLE SCOHandle,
                                OI_L2CAP_CID destCID,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize)
{
    /* 
     * In a multi-point call, there may already be an existing ACTIVE call.
     * In that case, we need to send a RELEASE message to indicate that we
     * are clearing the non selected call.
     */
    if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) && OI_Tcs_ActiveCallExists()) {
        ClearNonSelected(call);
        return;
    }

    if (TCS_STATE_CALL_INITIATED == call->state ||
        TCS_STATE_OVERLAP_SENDING == call->state) {

        OI_Tcs_CancelTimer(call); // T303, T304
        OI_Tcs_CancelTimer(NULL);

        if (TCS_STATE_CALL_INITIATED == call->state && !CompareBearer(bearer, &call->bearer)) {
            SetCallState(call, TCS_STATE_OUTGOING_CALL_PROCEEDING);
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);        
            return;
        }

        SetCallState(call, TCS_STATE_OUTGOING_CALL_PROCEEDING);
        OI_Tcs_StartTimer(call, TCS_T310);
        OI_Tcs_Data->callbacks.outgoingStatusCB(&call->addr, OI_TCS_CALL_STATUS_PROCEEDING, companySpecific, companySpecificSize);
    } else {
        OI_DBGPRINT2(("Unexpected CALL PROCEEDING received!"));
    }
}

static void Recv_Connect(TCS_CALL_INFO *call,
                         OI_TCS_BEARER *bearer,
                         SCO_HANDLE handle)
{
    OI_STATUS result;

    /* 
     * In a multi-point call, there may already be an existing ACTIVE call.
     * In that case, we need to send a RELEASE message to indicate that we
     * are clearing the non selected call.
     */
    if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) && OI_Tcs_ActiveCallExists()) {
        ClearNonSelected(call);
        return;
    }

    if (TCS_STATE_CALL_INITIATED == call->state ||
        TCS_STATE_OVERLAP_SENDING == call->state ||
        TCS_STATE_OUTGOING_CALL_PROCEEDING == call->state ||
        TCS_STATE_CALL_DELIVERED == call->state) {

        OI_Tcs_CancelTimer(call); // T301, T303, T304, T310
        OI_Tcs_CancelTimer(NULL);

        if (TCS_STATE_CALL_INITIATED == call->state && !CompareBearer(bearer, &call->bearer)) {
            SetCallState(call, TCS_STATE_CONNECT_RECEIVED);
            OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);        
            return;
        }

        /*
         * We need to connect to the bearer channel (which should already be 
         * initiated by the other side) before sending the CONNECT ACK.
         */
        if ((OI_TCS_BEARER_LINK_TYPE_NONE == call->bearer.type) || (CH_ST_CONNECTED == call->bearerState)) {
            result = OI_Tcs_Send_ConnectAck(call, NULL, 0);
            if (OI_SUCCESS(result)) {
                SetCallState(call, TCS_STATE_ACTIVE);
                if (OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) {
                    OI_Tcs_ClearAllNonSelected(call);
                }
                OI_Tcs_Data->callbacks.establishCfm(&call->addr, OI_OK);
            } else {
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("Send CONNECT ACK after receiving CONNECT failed"));
#endif
            }
        } else {
            /*
             * In the following state, the bearer connect complete will send a 
             * CONNECT ACK and set the call to the ACTIVE state.
             */
            SetCallState(call, TCS_STATE_CONNECT_RECEIVED);
        }
    } else {
        OI_DBGPRINT2(("Unexpected CONNECT message!\n"));
    }
}

static void Recv_ConnectAck(TCS_CALL_INFO *call,
                            SCO_HANDLE handle,
                            OI_L2CAP_CID destCID)
{
    if (TCS_STATE_CONNECT_REQ == call->state) {
        if ((OI_TCS_BEARER_LINK_TYPE_NONE != call->bearer.type) && (CH_ST_CONNECTED != call->bearerState)) {
            OI_DBGPRINT(("Received CONNECT ACK, but bearer channel not connected\n"));
        }
        OI_Tcs_CancelTimer(call); // T313
        SetCallState(call, TCS_STATE_ACTIVE);
        OI_Tcs_Data->callbacks.establishCfm(&call->addr, OI_OK);
    } else {
        OI_DBGPRINT2(("Unexpected CONNECT ACK message!\n"));
    }
}

static void Recv_Disconnect(TCS_CALL_INFO *call,
                            OI_TCS_CAUSE cause,
                            OI_TCS_PROGRESS progress,
                            SCO_HANDLE handle,
                            OI_L2CAP_CID destCID) 
{
    OI_STATUS result;

    if (TCS_STATE_NULL == call->state) {
        OI_DBGPRINT2(("Unexpected DISCONNECT message received!\n"));
        return;
    }

    if (TCS_STATE_DISCONNECT_REQ == call->state) {
        OI_Tcs_CancelTimer(call); // T305
    } else if ((TCS_STATE_NULL != call->state) &&
        (TCS_STATE_CALL_PRESENT != call->state) &&
        (TCS_STATE_CALL_INITIATED != call->state) &&
        (TCS_STATE_DISCONNECT_REQ != call->state) && 
        (TCS_STATE_DISCONNECT_IND != call->state) &&
        (TCS_STATE_RELEASE_REQ != call->state)) {

        OI_Tcs_CancelTimer(call);
        SetCallState(call, TCS_STATE_DISCONNECT_IND);
    } else {
        OI_DBGPRINT2(("Received unexpected DISCONNECT message!"));
        return;
    }

    result = OI_Tcs_Send_Release(call, cause, handle);
    if (OI_SUCCESS(result)) {
        SetCallState(call, TCS_STATE_RELEASE_REQ);
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Send RELEASE after receiving DISCONNECT failed"));
#endif
    }
}

static void Recv_Information(TCS_CALL_INFO *call,
                             OI_BOOL sendingComplete,
                             OI_CHAR keypadFacility,
                             OI_TCS_NUMBER *callingNumber,
                             OI_TCS_NUMBER *calledNumber,
                             OI_TCS_AUDIO_CONTROL audioControl,
                             OI_UINT8 *companySpecific,
                             OI_UINT8 companySpecificSize)
{
    if ((TCS_STATE_NULL == call->state)
        || (TCS_STATE_CALL_PRESENT == call->state)
        || (TCS_STATE_CALL_INITIATED == call->state)
        || (TCS_STATE_DISCONNECT_REQ == call->state) 
        || (TCS_STATE_DISCONNECT_IND == call->state)
        || (TCS_STATE_RELEASE_REQ == call->state)) {

        OI_DBGPRINT2(("Unexpected INFORMATION message received!\n"));
    }

    if (TCS_STATE_OVERLAP_RECEIVING == call->state) {
        OI_Tcs_CancelTimer(call); // T302
        OI_Tcs_StartTimer(call, TCS_T302);
    }
    if (OI_Tcs_Data->callbacks.infoInd != NULL) {
        OI_Tcs_Data->callbacks.infoInd(&call->addr, sendingComplete, keypadFacility, callingNumber, calledNumber, audioControl,
            companySpecific, companySpecificSize);
    }
}

static void Recv_Progress(TCS_CALL_INFO *call,
                          OI_TCS_PROGRESS progress,
                          SCO_HANDLE handle,
                          OI_L2CAP_CID destCID)
{
}

static void Recv_Release(TCS_CALL_INFO *call,
                         OI_TCS_CAUSE cause,
                         SCO_HANDLE handle)
{
    if ((TCS_STATE_OVERLAP_RECEIVING == call->state) ||
        (TCS_STATE_INCOMING_CALL_PROCEEDING == call->state) ||
        (TCS_STATE_CALL_RECEIVED == call->state) ||
        (TCS_STATE_CONNECT_REQ == call->state) ||
        (TCS_STATE_DISCONNECT_REQ == call->state) ||
        (TCS_STATE_RELEASE_REQ == call->state)) {
        
        OI_Tcs_CancelTimer(call); // T305 or T308

        /*
         * Ensure that the RELEASE COMPLETE message is not sent before the bearer
         * has been disconnected.
         */
        if (TCS_STATE_RELEASE_REQ == call->state) { 
            SetCallState(call, TCS_STATE_RELEASE_COLLISION_RECEIVED);
        } else {
            SetCallState(call, TCS_STATE_RELEASE_RECEIVED);
        }
        call->cause = cause;
        if (CH_ST_DISCONNECTED == call->bearerState) {
            OI_TCS_ClearCall(&call->addr, call->cause);
        } else if (CH_ST_CONNECTED == call->bearerState || CH_ST_CONNECT_IN_PROGRESS == call->bearerState) {
            OI_Tcs_DisconnectBearer(call);
        } else {
            /* Disconnect already in progress, just need to wait for it to complete */
        }
    } else {
        OI_DBGPRINT2(("Unexpected RELEASE message received!\n"));
    }
}

static void Recv_ReleaseComplete(TCS_CALL_INFO *call,
                                 OI_TCS_CAUSE cause,
                                 SCO_HANDLE handle)
{
    if ((TCS_STATE_CALL_INITIATED == call->state) ||
        (TCS_STATE_CALL_PRESENT == call->state) ||
        (TCS_STATE_RELEASE_REQ == call->state)) {

        if (TCS_STATE_RELEASE_REQ != call->state) {
            OI_Tcs_CancelTimer(NULL);
        }

        /*
         * Only disconnect bearer here for CTP, leave control connection open.  
         * Intercom will disconnect both after calling the releaseInd below.
         */
        if (!call->intercom && (CH_ST_CONNECTED == call->bearerState || CH_ST_CONNECT_IN_PROGRESS == call->bearerState)) {
            OI_Tcs_DisconnectBearer(call);
        }

        OI_Tcs_CancelTimer(call);
        OI_Tcs_ReleaseCallInfo(call);
        SetCallState(call, TCS_STATE_NULL);
        OI_Tcs_Data->callbacks.releaseInd(&call->addr, cause);

        if (call->intercom) {
            OI_Tcs_InternalDisconnect(call, OI_OK);
        }
    } else {
        OI_DBGPRINT2(("Unexpected RELEASE COMPLETE message received, ignoring\n"));
    }    
}

OI_STATUS OI_Tcs_CopyNumber(OI_TCS_NUMBER *dest, 
                            OI_TCS_NUMBER *src)
{
    *dest = *src;
    if (src->digits) {
        dest->digits = OI_Malloc(src->numDigits);
        if (!dest->digits) {
            OI_LOG_ERROR(("Could not allocate memory to copy number\n"));
            return OI_STATUS_OUT_OF_MEMORY;
        }
        OI_MemCopy(dest->digits, src->digits, src->numDigits);
    }
    return OI_OK;
}

static void Recv_Setup(TCS_CALL_INFO *call,
                       OI_TCS_CALL_CLASS callClass,
                       OI_BOOL sendingComplete,
                       OI_TCS_BEARER *bearer,
                       OI_TCS_SIGNAL signal,
                       OI_TCS_NUMBER *callingNumber,
                       OI_TCS_NUMBER *calledNumber,
                       OI_UINT8 *companySpecific,
                       OI_UINT8 companySpecificSize)
{
    OI_STATUS result;
    OI_TCS_CONNECT_INFO info;

    OI_ASSERT(call);

    if ((TCS_STATE_NULL != call->state) && (TCS_STATE_CALL_PRESENT != call->state)) {
        OI_DBGPRINT(("Unexpected SETUP message received, ignoring %s\n", OI_Tcs_CallStateText(call->state)));
        return;
    }

    /*
     * Must check whether a call is in progress before setting state.  After
     * checking, set state so that the call is cleared correctly.
     */
    if (OI_Tcs_CallInProgress()) {
        OI_DBGPRINT2(("Call in progress, rejecting SETUP"));
        SetCallState(call, TCS_STATE_CALL_PRESENT);
        OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_USER_BUSY);
        return;
    }

    SetCallState(call, TCS_STATE_CALL_PRESENT);
    if (call->intercom && OI_Dispatch_IsValidHandle(call->intercomSetupTimer)) {
        OI_Dispatch_CancelFunc(call->intercomSetupTimer);
    }

    /*
     * Save setup info
     */
    info.callClass = callClass;
    info.sendingComplete = sendingComplete;
    info.bearer = bearer;
    info.signal = signal;
    info.callingNumber = callingNumber;
    info.calledNumber = calledNumber;
    info.companySpecific = companySpecific;
    info.companySpecificSize = companySpecificSize;
    result = OI_Tcs_SetupCallInfo(call, &info);
    if (!OI_SUCCESS(result)) {
        OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_TEMPORARY_FAILURE);
        return;
    }

    /* Check that mandatory call class element is present */
    if (OI_TCS_CALL_CLASS_NONE == callClass) {
        OI_DBGPRINT(("Missing call class in SETUP message, rejecting\n"));
        OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_MANDATORY_INFORMATION_ELEM_MISSING);
        return;
    }

    OI_Tcs_Data->callbacks.establishInd(&call->addr, &info);
}

static void Recv_SetupAck(TCS_CALL_INFO *call,
                          OI_TCS_BEARER *bearer,
                          OI_TCS_PROGRESS progress,
                          SCO_HANDLE handle,
                          OI_L2CAP_CID destCID,
                          OI_UINT8 *companySpecific,
                          OI_UINT8 companySpecificSize)
{
    /*  
     * In a multi-point call, there may already be an existing ACTIVE call.
     * In that case, we need to send a RELEASE message to indicate that we
     * are clearing the non selected call.
     */
    if ((OI_TCS_ROLE_GATEWAY == OI_Tcs_Data->role) && OI_Tcs_ActiveCallExists()) {
        ClearNonSelected(call);
        return;
    }

    if (TCS_STATE_CALL_INITIATED != call->state) {
        OI_DBGPRINT2(("Unexpected Setup Acknowledge!\n"));
        return;
    }

    OI_Tcs_CancelTimer(call); // T303
    OI_Tcs_CancelTimer(NULL);

    if (TCS_STATE_CALL_INITIATED == call->state && !CompareBearer(bearer, &call->bearer)) {
        SetCallState(call, TCS_STATE_OVERLAP_SENDING);
        OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE);        
        return;
    }

    SetCallState(call, TCS_STATE_OVERLAP_SENDING);
    OI_Tcs_StartTimer(call, TCS_T304);

    OI_Tcs_Data->callbacks.outgoingStatusCB(&call->addr, OI_TCS_CALL_STATUS_OVERLAP_RECEIVING, companySpecific, companySpecificSize);
}

static void Recv_StartDTMF(TCS_CALL_INFO *call,
                           OI_CHAR facility)
{    
    if (OI_Tcs_Data->callbacks.startDTMF) {
        OI_Tcs_Data->callbacks.startDTMF(&call->addr, facility);
    } else {
        OI_TCS_StartDTMFReject(&call->addr, OI_TCS_CAUSE_FACILITY_REJECTED);
    }
}

static void Recv_StartDTMFAck(TCS_CALL_INFO *call, 
                              OI_CHAR facility)
{
    if (OI_Tcs_Data->callbacks.startDTMFAck) {
        OI_Tcs_Data->callbacks.startDTMFAck(&call->addr, facility);
    }
}

static void Recv_StartDTMFReject(TCS_CALL_INFO *call,
                                 OI_TCS_CAUSE cause)
{
    if (OI_Tcs_Data->callbacks.startDTMFReject) {
        OI_Tcs_Data->callbacks.startDTMFReject(&call->addr, cause);
    }
}

static void Recv_StopDTMF(TCS_CALL_INFO *call) 
{
    if (OI_Tcs_Data->callbacks.stopDTMF) {
        OI_Tcs_Data->callbacks.stopDTMF(&call->addr);
    } else {
        /*
         * If the callback above is not defined, then we must have rejected
         * any Start DTMF requests, so this is an invalid message and safe
         * to ignore.
         */
    }
}

static void Recv_StopDTMFAck(TCS_CALL_INFO *call,
                             OI_CHAR facility) 
{
    if (OI_Tcs_Data->callbacks.stopDTMFAck) {
        OI_Tcs_Data->callbacks.stopDTMFAck(&call->addr, facility);
    }
}

static void Recv_AccessRightsRequest(TCS_CALL_INFO *call) 
{
}

static void Recv_AccessRightsAccept(TCS_CALL_INFO *call)
{
}

static void Recv_AccessRightsReject(TCS_CALL_INFO *call)
{
}

static void Recv_InfoSuggest(TCS_CALL_INFO *call,
                             TCS_CONFIG_DATA *config)
{
}

static void Recv_InfoAccept(TCS_CALL_INFO *call)
{
}

static void Recv_ListenRequest(TCS_CALL_INFO *call,
                               OI_TCS_NUMBER *calledNumber)
{
}

static void Recv_ListenSuggest(TCS_CALL_INFO *call)
{
}

static void Recv_ListenAccept(TCS_CALL_INFO *call,
                              TCS_CLOCK_OFFSET clockOffset)
{
}

static void Recv_ListenReject(TCS_CALL_INFO *call,
                              OI_TCS_CAUSE cause)
{
}

static void Recv_CLInfo(TCS_CALL_INFO *call,
                        OI_TCS_AUDIO_CONTROL audioControl,
                        OI_UINT8 *companySpecific,
                        OI_UINT8 companySpecificSize)
{
    if (OI_Tcs_Data->callbacks.clInfoInd) {
        OI_Tcs_Data->callbacks.clInfoInd(&call->addr, audioControl, companySpecific, companySpecificSize);
    }
}

/** 
 * Free the data associated with any structures in the TCS_ELEMS
 * structure, but not the structure itself. 
 */
static void FreeDataElems(TCS_ELEMS *elems) 
{
    OI_ASSERT(elems);

    if (elems->bearer != NULL) {
        if (elems->bearer->type == OI_TCS_BEARER_LINK_TYPE_ACL) {
            OI_FreeIf(&elems->bearer->info.acl.flowspec);
        }
        OI_FreeIf(&elems->bearer);
    }
    if (elems->calledNumber != NULL) {
        OI_FreeIf(&elems->calledNumber->digits);
        OI_FreeIf(&elems->calledNumber);
    }
    if (elems->callingNumber != NULL) {
        OI_FreeIf(&elems->callingNumber->digits);
        OI_FreeIf(&elems->callingNumber);
    }
    OI_FreeIf(&elems->config);    
}

/** 
 * Handle receipt of a message.
 *
 * @param call data structure for call
 * @param buf  The buffer of data.
 * @param bufLen  The length of the data buffer.
 */
void OI_TCS_Recv_Msg(TCS_CALL_INFO *call, 
                     OI_UINT8 *buf,
                     OI_UINT16 bufLen)
{
    OI_BYTE_STREAM bs;
    TCS_MSG msgType;
    TCS_ELEMS elems;
    OI_STATUS status;
    OI_UINT8 tmp = 0;

    ElemsInit(&elems);

    ByteStream_Init(bs, buf, bufLen);
    ByteStream_Open(bs, BYTESTREAM_READ);

    ByteStream_GetUINT8_Checked(bs, tmp);
    if (ByteStream_Error(bs)) {
        OI_LOG_ERROR(("Error decoding message\n"));
        goto Done;
    }
    msgType = (TCS_MSG)tmp;

    status = OI_Tcs_DecodeMsg(&bs, &elems);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Message decode failed for type"));
#endif
        goto Done;
    }
    OI_Tcs_DumpMsg("Received ", &call->addr, (OI_UINT8)msgType, &elems);

    switch (msgType) {
        /*
         * Call control
         */
        case TCS_MSG_ALERTING:
            Recv_Alerting(call, elems.bearer, elems.progress, elems.handle, elems.destCID, 
                elems.companySpecific, elems.companySpecificSize);
            break;
        case TCS_MSG_CALL_PROCEEDING:
            Recv_CallProceeding(call, elems.bearer, elems.progress, elems.handle, elems.destCID, 
                elems.companySpecific, elems.companySpecificSize);
            break;
        case TCS_MSG_CONNECT:
            Recv_Connect(call, elems.bearer, elems.handle);
            break;
        case TCS_MSG_CONNECT_ACK:
            Recv_ConnectAck(call, elems.handle, elems.destCID);
            break;
        case TCS_MSG_PROGRESS:
            Recv_Progress(call, elems.progress, elems.handle, elems.destCID);
            break;
        case TCS_MSG_SETUP:
            Recv_Setup(call, elems.class, elems.sendingComplete, elems.bearer, elems.signal, 
                elems.callingNumber, elems.calledNumber, elems.companySpecific, elems.companySpecificSize);
            break;
        case TCS_MSG_SETUP_ACK:
            Recv_SetupAck(call, elems.bearer, elems.progress, elems.handle, elems.destCID, 
                elems.companySpecific, elems.companySpecificSize);
            break;
        case TCS_MSG_DISCONNECT:
            Recv_Disconnect(call, elems.cause, elems.progress, elems.handle, elems.destCID);
            break;
        case TCS_MSG_RELEASE:
            Recv_Release(call, elems.cause, elems.handle);
            break;
        case TCS_MSG_RELEASE_COMPLETE:
            Recv_ReleaseComplete(call, elems.cause, elems.handle);
            break;
        case TCS_MSG_INFORMATION:
            Recv_Information(call, elems.sendingComplete, elems.keypadFacility, elems.callingNumber, 
                elems.calledNumber, elems.audioControl, elems.companySpecific, elems.companySpecificSize);
            break;

        /*
         * Supplementary services
         */
        case TCS_MSG_START_DTMF:
            Recv_StartDTMF(call, elems.keypadFacility);
            break;
        case TCS_MSG_START_DTMF_ACK:
            Recv_StartDTMFAck(call, elems.keypadFacility);
            break;
        case TCS_MSG_START_DTMF_REJECT:
            Recv_StartDTMFReject(call, elems.cause);
            break;
        case TCS_MSG_STOP_DTMF:
            Recv_StopDTMF(call);
            break;
        case TCS_MSG_STOP_DTMF_ACK:
            Recv_StopDTMFAck(call, elems.keypadFacility);
            break;

        /*
         * Group management
         */
        case TCS_MSG_INFO_SUGGEST:
            Recv_InfoSuggest(call, elems.config);
            break;
        case TCS_MSG_INFO_ACCEPT:
            Recv_InfoAccept(call);
            break;
        case TCS_MSG_LISTEN_REQUEST:
            Recv_ListenRequest(call, elems.calledNumber);
            break;
        case TCS_MSG_LISTEN_ACCEPT:
            Recv_ListenSuggest(call);
            break;
        case TCS_MSG_LISTEN_SUGGEST:
            Recv_ListenAccept(call, elems.offset);
            break;
        case TCS_MSG_LISTEN_REJECT:
            Recv_ListenReject(call, elems.cause);
            break;
        case TCS_MSG_ACCESS_RIGHTS_REQUEST:
            Recv_AccessRightsRequest(call);
            break;
        case TCS_MSG_ACCESS_RIGHTS_ACCEPT:
            Recv_AccessRightsAccept(call);
            break;
        case TCS_MSG_ACCESS_RIGHTS_REJECT:
            Recv_AccessRightsReject(call);
            break;

        /*
         * Connectionless
         */
        case TCS_MSG_CL_INFO:
            Recv_CLInfo(call, elems.audioControl, elems.companySpecific, elems.companySpecificSize);
            break;

      default:
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("invalid packet"));
#endif
            break;
    }

Done:
    FreeDataElems(&elems);
}
#endif /* FEATURE_BT_EXTPF_CTP */

