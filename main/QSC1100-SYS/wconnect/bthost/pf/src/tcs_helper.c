#ifdef FEATURE_BT_EXTPF_CTP
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/tcs_helper.c#1 $ 

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
@file  
helper functions for TCS protocol
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_TCS

#include "oi_dispatch.h"
#include "oi_assert.h"

#include "oi_tcs_cc.h"
#include "oi_tcs_msg.h"
#include "oi_tcs_spec.h"
#include "oi_tcs_helper.h"

/** The table for associating a timer identifier (TCS_TXXX) with its
    value (TCS_TXXX_DURATION). TCS_MAX is the largest value in the
    enumeration for the identifiers.*/
static const OI_UINT timerTable[TCS_TIMER_ENUM_MAX] = { 
    TCS_TNULL_DURATION,
    TCS_T301_DURATION,
    TCS_T302_DURATION,
    TCS_T303_DURATION,
    TCS_T304_DURATION,
    TCS_T305_DURATION,
    TCS_T308_DURATION,
    TCS_T310_DURATION,
    TCS_T313_DURATION,
    TCS_T401_DURATION,
    TCS_T402_DURATION,
    TCS_T403_DURATION,
    TCS_T404_DURATION,
    TCS_T405_DURATION,
    TCS_T406_DURATION
};

#ifdef OI_DEBUG
#error code not present
#endif

/** Handle timer expirations. */
static void TimeoutHandler(DISPATCH_ARG *arg) 
{    
    OI_STATUS result;
    TCS_CALL_INFO *call;
    OI_UINT timerType;
    OI_UINT i;

    call = Dispatch_GetArg(arg, TCS_CALL_INFO *);
    timerType = call ? call->timerType : OI_Tcs_Data->timerType;
    OI_DBGPRINT2(("Timer %s expired", TimerTypeText(timerType)));

    switch (timerType) {
        case TCS_T302:
            if (call) {
                if (TCS_STATE_OVERLAP_RECEIVING == call->state) {
                    OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_INVALID_NUMBER_FORMAT);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("T302 expired for %:, but call in invalid state"));
#endif
                }
            } else {
                OI_LOG_ERROR(("T302 expired for multi-point call\n"));
            }
            break;

        case TCS_T303:
            if (call) {
                if (TCS_STATE_CALL_INITIATED == call->state) {
                    OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_RECOVERY_ON_TIMER_EXPIRY);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("T303 expired for %:, but call in invalid state"));
#endif
                }
            } else if (!call) {
                for (i = 0; i < OI_Tcs_Data->numCalls; ++i) {
                    call = &OI_Tcs_Data->calls[i];
                    if (!IsCallFree(call)) {
                        if (TCS_STATE_CALL_INITIATED == call->state) {
                            OI_Tcs_ReleaseCallInfo(call);
                            SetCallState(call, TCS_STATE_NULL);
                        } else {
#ifdef OI_CODE
#error code not present
#else
                            OI_LOG_ERROR(("T303 expired for %:, but call in invalid state"));
#endif
                        }
                    }
                }
            }
            break;

        case TCS_T301:
        case TCS_T304:
        case TCS_T310:
        case TCS_T313:
            if (call) {
                if ((TCS_T301 == timerType && TCS_STATE_CALL_DELIVERED == call->state) ||
                    (TCS_T304 == timerType && TCS_STATE_OVERLAP_SENDING == call->state) ||
                    (TCS_T310 == timerType && TCS_STATE_OUTGOING_CALL_PROCEEDING == call->state) ||
                    (TCS_T313 == timerType && TCS_STATE_CONNECT_REQ == call->state)) {
                    OI_TCS_ClearCall(&call->addr, OI_TCS_CAUSE_RECOVERY_ON_TIMER_EXPIRY);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("Timer expired for %:, but call in invalid state"));
#endif
                }
            } else {
                OI_LOG_ERROR(("Timer expired for multi-point call\n"));
            }
            break;

        case TCS_T305:
            if (call) {
                if (TCS_STATE_DISCONNECT_REQ == call->state) {
                    result = OI_Tcs_Send_Release(call, call->cause, 0);
                    if (OI_SUCCESS(result)) {
                        SetCallState(call, TCS_STATE_RELEASE_REQ);
                    } else {
#ifdef OI_CODE
#error code not present
#else
                        OI_LOG_ERROR(("Send RELEASE on T305 expiration failed"));
#endif
                    }
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("T305 expired for %:, but call in invalid state"));
#endif
                }
            } else {
                OI_LOG_ERROR(("T305 expired for multi-point call\n"));
            }
            break;

        case TCS_T308:
            if (call) {
                if (TCS_STATE_RELEASE_REQ == call->state) {
                    if (CH_ST_CONNECTED == call->bearerState) {
                        OI_Tcs_DisconnectBearer(call);
                    }
                    OI_Tcs_ReleaseCallInfo(call);
                    SetCallState(call, TCS_STATE_NULL);
                } else {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("T308 expired for %:, but call in invalid state"));
#endif
                }
            } else {
                OI_LOG_ERROR(("T308 expired for multi-point call\n"));
            }
            break;
    }
}

/** Start one of the TCS timers. */
OI_STATUS OI_Tcs_StartTimer(TCS_CALL_INFO *call, OI_UINT timerType) 
{
    OI_STATUS status;
    OI_INTERVAL timeout;
    DISPATCH_ARG arg;

    OI_ASSERT(timerType < TCS_TIMER_ENUM_MAX);
    if (TCS_TNULL == timerType) {
        return OI_OK;
    }

    /*
     * If there is unexpired timer, cancel it before setting the new one.
     */
    if (call && OI_Dispatch_IsValidHandle(call->timerHandle)) {
        status = OI_Dispatch_CancelFunc(call->timerHandle);
    } else if (!call && OI_Dispatch_IsValidHandle(OI_Tcs_Data->timerHandle)) {
        status = OI_Dispatch_CancelFunc(OI_Tcs_Data->timerHandle);
    }

    if (call) {
        call->timerType = timerType;
    } else {
        OI_Tcs_Data->timerType = timerType;
    }
    Dispatch_SetArg(arg, call);

    timeout = timerTable[timerType];

    OI_DBGPRINT2(("Starting timer %s", TimerTypeText(timerType)));
    if (call) {
        status = OI_Dispatch_RegisterTimedFunc(TimeoutHandler, &arg, timeout, &call->timerHandle);
    } else {
        status = OI_Dispatch_RegisterTimedFunc(TimeoutHandler, &arg, timeout, &OI_Tcs_Data->timerHandle);
    }
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("Timer register failed - %!\n", status));
    }
    return status;
}

/** Cancel the current timer. */
OI_STATUS OI_Tcs_CancelTimer(TCS_CALL_INFO *call) 
{
    OI_STATUS status;

    if (call && OI_Dispatch_IsValidHandle(call->timerHandle)) {
        status = OI_Dispatch_CancelFunc(call->timerHandle);
        OI_DBGPRINT2(("Cancel timer - %!", status));
    } else if (!call && OI_Dispatch_IsValidHandle(OI_Tcs_Data->timerHandle)) {
        status = OI_Dispatch_CancelFunc(OI_Tcs_Data->timerHandle);
        OI_DBGPRINT2(("Cancel timer - %!", status));
    } else {
        status = OI_OK;
    }
    return status;
}


/** Copy a bearer. */
void OI_Tcs_CopyBearer(OI_TCS_BEARER *dest, OI_TCS_BEARER *src)
{
    OI_ASSERT(dest != NULL);
    OI_ASSERT(src != NULL);

    dest->type = src->type;

    if (dest->type == OI_TCS_BEARER_LINK_TYPE_SCO) {
        dest->info.sco.packetType = src->info.sco.packetType;
        dest->info.sco.coding = src->info.sco.coding;
    } else {
        /*
         * L2CAP QoS is not supported yet
         *
        if (dest->info.acl.flowspec != NULL) {
            dest->info.acl.flowspec->serviceType = src->info.acl.flowspec->serviceType;
            dest->info.acl.flowspec->tokenRate = src->info.acl.flowspec->tokenRate;
            dest->info.acl.flowspec->tokenBucketSize = src->info.acl.flowspec->tokenBucketSize;
            dest->info.acl.flowspec->peakBandwidth = src->info.acl.flowspec->peakBandwidth;
            dest->info.acl.flowspec->latency = src->info.acl.flowspec->latency;
            dest->info.acl.flowspec->delayVariation = src->info.acl.flowspec->delayVariation;
         }
         */
        dest->info.acl.layer2 = src->info.acl.layer2;
        dest->info.acl.layer3 = src->info.acl.layer3;
    }
}
#endif /* FEATURE_BT_EXTPF_CTP */

