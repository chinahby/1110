/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bpp_sender.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2008-06-11   pr  Moved cancelCfm from cb union to BPP_SENDER_CLI struct,
                  to prevent the other callbacks from being overwritten during
                  abort.

*    #20       28 Sep 2007            RS
* Made provisions to pass up the end of body message as well to the 
* layer above in the GetCfm function.
*
*    #19       30 Jul 2007            RS
* Removing the code contributing to the automatic OK respose
*
*    #18       28 Feb 2007            MH
* Modified obex disconnect indication to support optional headers
*
*    #17       10 Nov  2006           PR
*  Merged OI Code v3.0.14rev1revQ and added new code to handle Abort.
*
*    #16       26 Oct 2006            MH
* Modified obex srv connect indication to support all headers
*
*    #15       19 Sept 2006           MH
*  Changed to support OI client connect
*
*    #14       29 Aug 2006            GS
* Fixing klocwork error.
*
*    #13       29 Jul 2006            BK
* Modified for obex client initiated authentication.
*
*    #11-12    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #10       23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #9        19 Apr 2005            GS
* Modified calls to DECLARE_HEADER macro to account for End-of-Body header.
* Updated fix in #7 for GetEvent SOAP message.
*
*    #8        04 Apr 2005            JH
* Featurize change made in #7.
*
*    #7        01 Apr 2005            GS
* Updated GetCfm() to return OK status whenever End-of-Body header is present
* even if remote end sent Continue response (needed for proper handling of 
* GetEvent SOAP response in the App)
*
*    #6        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #5        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #4        18 May 2004            JH
* Modified connect command to use a dummy connection policy to pass in
*  destination server UUID down to sio_open
*
*    #2        15 Apr 2004            JH
* Modified server register function to use a dummy connection policy to
*  pass in service class UUID down to sio_open
*
===========================================================================*/
/**
@file
@internal  
Basic Printing Profile sender role
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_BPP_SENDER

#include "oi_bpp_sender.h"
#include "oi_bpp_private.h"
#include "oi_memmgr.h"
#include "oi_assert.h"
#include "oi_obexcommon.h"
#include "oi_argcheck.h"
#include "oi_init_flags.h"
#ifndef OI_CODE
#include "oi_bt_assigned_nos.h"
#endif

typedef struct {
    OI_OBEXCLI_CONNECTION_HANDLE handle;
    OI_BPP_SENDER_CONNECT_CFM connectCfm;
    OI_BPP_SENDER_DISCONNECT_IND disconnectInd;
    OI_BPP_SENDER_AUTH_CHALLENGE_IND authInd;
    OI_BYTE appParamData[OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN + sizeof(OI_BPP_JOB_ID)];

    enum {
        BPPCLI_STATE_IDLE,
        BPPCLI_STATE_CONNECTING,
        BPPCLI_STATE_CONNECTED,
        BPPCLI_STATE_SOAP,
        BPPCLI_STATE_SEND_FILE,
        BPPCLI_STATE_SEND_REFERENCE,
        BPPCLI_STATE_GET_RUI,
        BPPCLI_STATE_DISCONNECTING,
        BPPCLI_STATE_ABORTING
    } state;

    union {
        OI_BPP_SENDER_GET_CFM getCfm;
        OI_BPP_SENDER_SEND_FILE_CFM sendFile;
        OI_BPP_SENDER_SEND_REFERENCE_CFM sendReference;
#ifdef OI_CODE
#error code not present
#endif
    } cb;
#ifndef OI_CODE
    OI_BPP_SENDER_CANCEL_CFM cancelCfm;
#endif
} BPP_SENDER_CLI;

typedef struct {
    OI_OBEXSRV_CONNECTION_HANDLE handle;
    OI_UINT8 channel;

    OI_BYTE appParamData[OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN + sizeof(OI_INT32)];

    enum {
        BPPSRV_STATE_REGISTERED,
        BPPSRV_STATE_CONNECTED,
        BPPSRV_STATE_GETTING,
        BPPSRV_STATE_DISCONNECTING
    } state;
    const OI_BPP_SENDER_CALLBACKS *cb;
} BPP_SENDER_SRV;

typedef struct {
    BPP_SENDER_CLI job;
    BPP_SENDER_CLI status;
    BPP_SENDER_CLI rui;

    BPP_SENDER_SRV obj;
    BPP_SENDER_SRV ruiObj;

    SENDER_SDP_DATA *sdp;
} BPP_SENDER;


static BPP_SENDER *Sender;

static BPP_SENDER_CLI *LookupClient(OI_OBEXCLI_CONNECTION_HANDLE handle)
{
    BPP_SENDER_CLI *client;

    if (Sender == NULL) {
        client = NULL;
    } else if (handle == 0) {
        client = NULL;
    } else if (Sender->job.handle == handle) {
        client = &Sender->job;
    } else if (Sender->status.handle == handle) {
        client = &Sender->status;
    } else if (Sender->rui.handle == handle) {
        client = &Sender->rui;
    } else {
        client = NULL;
    }

    if (client) {
        OI_ASSERT(client->state != BPPCLI_STATE_IDLE);
    }

    return client;
}

static BPP_SENDER_SRV *LookupServer(OI_OBEXSRV_CONNECTION_HANDLE handle)
{
    if (Sender == NULL) {
        return NULL;
    } else if (handle == 0) {
        OI_LOG_ERROR(("LookupServer called 0 handle"));
        return NULL;
    } else if (Sender->obj.handle == handle) {
        return &Sender->obj;
    } else if (Sender->ruiObj.handle == handle) {
        return &Sender->ruiObj;
    } else {
        OI_STATUS status;
        OI_UINT8 channel;
        status = OI_OBEXSRV_GetRfcommChannel(handle, &channel);
        if (!OI_SUCCESS(status)) {
            return NULL;
        } else if (Sender->obj.channel == channel) {
            return &Sender->obj;
        } else if (Sender->ruiObj.channel == channel) {
            return &Sender->ruiObj;
        } else {
            return NULL;
        }
    }
}

static void ObexClientConnectCfm(OI_OBEXCLI_CONNECTION_HANDLE handle, 
                                 OI_BOOL readOnly, 
#ifndef OI_CODE
                                 OI_OBEX_HEADER_LIST *hdrs, /*ignored */
#endif
                                 OI_STATUS status)
{
    BPP_SENDER_CLI *client;

    OI_DBGPRINT(("ObexClientConnectCfm: %d", handle));

    client = LookupClient(handle);
    if (client == NULL || client->state != BPPCLI_STATE_CONNECTING) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected connectCfm on handle"));
#endif
        return;
    }
    
    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_CONNECTED;
    } else {
        client->state = BPPCLI_STATE_IDLE;
        client->handle = 0;
    }

    client->connectCfm(handle, status);
}

#ifdef OI_CODE
#error code not present
#else
static void ObexClientDisconnectInd(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                    OI_OBEX_HEADER_LIST *hdrs) /*ignored */
#endif
{
    BPP_SENDER_CLI *client;

    OI_DBGPRINT(("ObexClientDisconnectInd: %d", handle));

    client = LookupClient(handle);
    if (client == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected obex disconnectInd"));
#endif
        return;
    }
    client->state = BPPCLI_STATE_IDLE;
    client->handle = 0;
    client->disconnectInd(handle);
}

static void ObexClientAuthInd(OI_OBEXCLI_CONNECTION_HANDLE handle, 
#ifndef OI_CODE
                              OI_BOOL fullAccess,
#endif
                              OI_BOOL userIdRequired)
{
    BPP_SENDER_CLI *client;
    OI_DBGPRINT(("ObexClientAuthInd: %d", handle));
    
    client = LookupClient(handle);
    if (client == NULL || client->authInd == NULL) {
        if (client == NULL) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Unexpected authInd on handle"));
#endif
        }
        OI_OBEXCLI_AuthenticationResponse(handle, NULL, 0, NULL); /* Abort the connection attempt */ 
    } else {
        client->authInd(handle, userIdRequired);
    }
}

OI_STATUS OI_BPP_SENDER_Connect(OI_BD_ADDR *addr,
                                OI_UINT8 channel,
                                OI_BPP_TARGET target,
                                OI_BPP_SENDER_CONNECT_CFM connectCfm,
                                OI_BPP_SENDER_DISCONNECT_IND disconnectInd,
                                OI_BPP_SENDER_AUTH_CHALLENGE_IND authInd,
                                OI_OBEXCLI_CONNECTION_HANDLE *handle)

{
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 2);
#else
#error code not present
#endif /* not OI_CODE */

    BPP_SENDER_CLI *client;
    OI_OBEX_BYTESEQ const *targetSeq;
    OI_STATUS status;
#ifndef OI_CODE
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_DirectPrinting,     // OI_UINT32           serviceUuid32 ;
        OI_SEC_REQUIRE_NOTHING,     // securityPolicy is a place holder, get real policy at runtime
        FALSE,                      // OI_BOOL             mustBeMaster ;
        NULL,                       // OI_L2CAP_FLOWSPEC   *flowspec;    
        0                           // OI_UINT8            powerSavingDisables ;    
    } ;
#endif /* not OI_CODE */
    
    ARGCHECK(addr != NULL);
    ARGCHECK(handle != NULL);
    ARGCHECK(connectCfm != NULL);
    ARGCHECK(disconnectInd != NULL);

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }
    
    switch (target) {
        case BPP_TARGET_DPS:
            targetSeq = &OI_BPP_ObexTarget_DPS;
#ifndef OI_CODE            
            connectPolicy.serviceUuid32 = OI_UUID_DirectPrinting;
#endif
            client = &Sender->job;
            break;
        case BPP_TARGET_PBR:
            targetSeq = &OI_BPP_ObexTarget_PBR;
#ifndef OI_CODE            
            connectPolicy.serviceUuid32 = OI_UUID_ReferencePrinting;
#endif
            client = &Sender->job;
            break;
        case BPP_TARGET_STS:
            targetSeq = &OI_BPP_ObexTarget_STS;
#ifndef OI_CODE            
            connectPolicy.serviceUuid32 = OI_UUID_PrintingStatus;
#endif
            client = &Sender->status;
            break;
        case BPP_TARGET_RUI:
            targetSeq = &OI_BPP_ObexTarget_RUI;
#ifndef OI_CODE            
            connectPolicy.serviceUuid32 = OI_UUID_ReflectedUI;
#endif
            client = &Sender->rui;
            break;
        default:
            return OI_STATUS_INVALID_PARAMETERS;
    }

    if (client->state != BPPCLI_STATE_IDLE) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    INIT_HEADER(hdrList);

    ADD_HEADER(hdrList, OI_OBEX_HDR_TARGET, target, (OI_OBEX_BYTESEQ*)targetSeq);

#ifdef OI_CODE
#error code not present
#else
    /* connectPolicy is used to pass destination server UUID down to */
    /* OI_RFCOMM_Connect and then to sio_open */
    status = OI_OBEXCLI_Connect(addr,
                                channel,
                                &hdrList,
                                ObexClientConnectCfm,
                                ObexClientDisconnectInd,
                                ObexClientAuthInd,
                                NULL, /*Challenge Response Call back */
                                handle,
                                &connectPolicy );
#endif /* OI_CODE */

    if (OI_SUCCESS(status)) {
        client->handle = *handle;
        client->state = BPPCLI_STATE_CONNECTING;
        client->connectCfm = connectCfm;
        client->disconnectInd = disconnectInd;
        client->authInd = authInd;
    }
    return status;
}

OI_STATUS OI_BPP_SENDER_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE handle)
{
    BPP_SENDER_CLI *client;
    OI_STATUS status;
    client = LookupClient(handle);
    
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    } else if (client == NULL) {
        return OI_STATUS_INVALID_HANDLE;
    } else if (client->state != BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    OI_DBGPRINT2(("%x %x %x", (OI_UINT32)client, (OI_UINT32)&Sender->job, (OI_UINT32)&Sender->status));
    
    if ((client == &Sender->job) && (Sender->status.state != BPPCLI_STATE_IDLE)) {
        OI_LOG_ERROR(("The status channel must be disconneted before the job channel may be disconnected."));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCLI_Disconnect(handle, NULL);
#endif
    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_DISCONNECTING;
    }

    return status;
}


static void GetCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                    OI_OBEX_HEADER_LIST *headers,
                    OI_STATUS status)
{
    BPP_SENDER_CLI *client;
    OI_OBEX_BYTESEQ *body = NULL;
    OI_BPP_JOB_ID *jobIdPtr = NULL;
    OI_BPP_JOB_ID jobId = 0;
#ifndef OI_CODE
    OI_BOOL final = 0; /* End of body has not arrived */
#endif
    
    OI_DBGPRINT(("GetCfm: %d %!", handle, status));
    
    client = LookupClient(handle);
    if (client == NULL || (client->state != BPPCLI_STATE_GET_RUI && client->state != BPPCLI_STATE_SOAP)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected obex GetCfm"));
#endif
        return;
    }

    if (headers) {
        OI_OBEX_HEADER *hdr;
        hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_BODY);
        if (hdr == NULL) {
            hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_END_OF_BODY);
#ifndef OI_CODE
            if (hdr != NULL) {
                final = 1; /* End of body has arrived */
            }
#endif
        }
        if (hdr) {
            body = &hdr->val.body;
        }

        hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_APPLICATION_PARAMS);
        if (hdr) {
            OI_OBEX_APP_PARAM_LIST paramList;

            if (OI_SUCCESS(OI_OBEXCOMMON_ParseAppParamsHeader(&hdr->val.applicationParams, &paramList))) {
                OI_UINT j;
                for (j = 0; j < paramList.count; ++j) {
                    if ((paramList.list[j].tag == BPP_OBEX_APP_PARAM_TAG_JOBID) && (paramList.list[j].len == sizeof(OI_UINT32))) {
                        jobId = GetUINT32_BigEndian(paramList.list[j].data);
                        jobIdPtr = &jobId;
                        break;
                    }
                }
                OI_Free(paramList.list);
            }
        }
    }

    if (status != OI_OBEX_CONTINUE) {
        client->state = BPPCLI_STATE_CONNECTED;
    }
#ifdef OI_CODE
#error code not present
#else
    client->cb.getCfm(handle, body, jobIdPtr, status, final);
#endif
}


OI_STATUS OI_BPP_SENDER_SOAPRequest(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                    OI_BPP_SENDER_GET_CFM cb,
                                    OI_OBEX_BYTESEQ *body,
                                    OI_BOOL final)
{
    BPP_SENDER_CLI *client;
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 3);
#else
#error code not present
#endif /* not OI_CODE */
    OI_BOOL first;
    OI_STATUS status;

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    ARGCHECK(cb != NULL);

    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    } else if (client->state == BPPCLI_STATE_CONNECTED) {
        first = TRUE;
    } else if (client->state == BPPCLI_STATE_SOAP) {
        first = FALSE;
    } else {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    INIT_HEADER(hdrList);

    if (first) {
        ADD_HEADER(hdrList, OI_OBEX_HDR_TYPE, type, &OI_BPP_MimeType_SOAP);
    }

    ADD_BODY_HEADER(hdrList, final, body);

    client->cb.getCfm = cb;
    status = OI_OBEXCLI_Get(handle, &hdrList, GetCfm, final);
    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_SOAP;
    } else {
        client->state = BPPCLI_STATE_CONNECTED;
    }
    return status;
}   

static void SendFileCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                        OI_OBEX_HEADER_LIST *headers,
                        OI_STATUS status)
{
    BPP_SENDER_CLI *client;

    OI_DBGPRINT(("SendFileCfm: %d %!", handle, status));
    
    client = LookupClient(handle);
    if (client == NULL || client->state != BPPCLI_STATE_SEND_FILE) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected obex PutCfm"));
#endif
        return;
    }
    
    if (status != OI_OBEX_CONTINUE) {
        client->state = BPPCLI_STATE_CONNECTED;
    }
    client->cb.sendFile(handle, status);
}

OI_STATUS OI_BPP_SENDER_SendFile(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                 OI_BPP_SENDER_SEND_FILE_CFM cb,
                                 OI_OBEX_BYTESEQ *type,
                                 OI_OBEX_UNICODE *description,
                                 OI_OBEX_UNICODE *name,
                                 OI_BPP_JOB_ID *jobId,
                                 OI_OBEX_BYTESEQ *data,
                                 OI_BOOL final)
{
    OI_BOOL first;
    BPP_SENDER_CLI *client;
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 6);
#else
#error code not present
#endif /* not OI_CODE */
    OI_STATUS status;

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    ARGCHECK(cb != NULL);

    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    } else if (client->state == BPPCLI_STATE_CONNECTED) {
        first = TRUE;
    } else if (client->state == BPPCLI_STATE_SEND_FILE) {
        first = FALSE;
    } else {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    
    INIT_HEADER(hdrList);
    
    if (first) {
        if (type == NULL) {
            return OI_STATUS_INVALID_PARAMETERS;
        }

        ADD_HEADER(hdrList, OI_OBEX_HDR_TYPE, type, type);
        if (description != NULL) {
            ADD_HEADER(hdrList, OI_OBEX_HDR_DESCRIPTION, description, description);
        }
        if (name != NULL) {
            ADD_HEADER(hdrList, OI_OBEX_HDR_NAME, name, name);
        }

        if (jobId) {
            OI_BPP_AddJobIdHeader(&hdrList, *jobId, client->appParamData);
        }
    } else if ((type != NULL) || (description != NULL) ||  (name != NULL) || (jobId != NULL)) {
        return OI_STATUS_INVALID_PARAMETERS; 
    }

    ADD_BODY_HEADER(hdrList, final, data);

    client->cb.sendFile = cb;
    status = OI_OBEXCLI_Put(handle, &hdrList, SendFileCfm, final ? OI_OK : OI_OBEX_CONTINUE);

    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_SEND_FILE;
    } else {
        client->state = BPPCLI_STATE_CONNECTED;
    }
    return status;
}


static void SendReferenceCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                             OI_OBEX_HEADER_LIST *headers,
                             OI_STATUS status)
{
    BPP_SENDER_CLI *client;
    OI_OBEX_BYTESEQ *url = NULL;
    OI_OBEX_BYTESEQ *httpChallenge = NULL;

    OI_DBGPRINT(("SendReferenceCfm: %d %!", handle, status));

    client = LookupClient(handle);
    if (client == NULL || client->state != BPPCLI_STATE_SEND_REFERENCE) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected obex PutCfm"));
#endif
        return;
    }

    if (headers) {
        OI_OBEX_HEADER *hdr;
        hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_BODY);
        if (hdr == NULL) {
            hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_END_OF_BODY);
        }
        if (hdr != NULL) {
            url = &hdr->val.body;
        }

        hdr = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_HTTP);
        if (hdr != NULL) {
            httpChallenge = &hdr->val.http;
        }
    }

    if (status != OI_OBEX_CONTINUE) {
        client->state = BPPCLI_STATE_CONNECTED;
    }

    client->cb.sendReference(handle, url, httpChallenge, status);
}


OI_STATUS OI_BPP_SENDER_SendReference(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                      OI_BPP_SENDER_SEND_REFERENCE_CFM cb,
                                      OI_BPP_REF_TYPE type,
                                      OI_OBEX_BYTESEQ *httpHeaders,
                                      OI_BPP_JOB_ID *jobId,
                                      OI_OBEX_BYTESEQ *data,
                                      OI_BOOL final)
{
    OI_BOOL first;
    BPP_SENDER_CLI *client;
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 6);
#else
#error code not present
#endif /* not OI_CODE */
    OI_STATUS status;

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    ARGCHECK(cb != NULL);

    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    } else if (client->state == BPPCLI_STATE_CONNECTED) {
        first = TRUE;
    } else if (client->state == BPPCLI_STATE_SEND_REFERENCE) {
        first = FALSE;
    } else {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    INIT_HEADER(hdrList);
    
    if (first) {
        switch (type) {
            case OI_BPP_REF_SIMPLE:
                ADD_HEADER(hdrList, OI_OBEX_HDR_TYPE, type, &OI_BPP_MimeType_RefSimple);
                break;
            case OI_BPP_REF_XML:
                ADD_HEADER(hdrList, OI_OBEX_HDR_TYPE, type, &OI_BPP_MimeType_RefXML);
                break;
            case OI_BPP_REF_LIST:
                ADD_HEADER(hdrList, OI_OBEX_HDR_TYPE, type, &OI_BPP_MimeType_RefList);
                break;
            default:
                return OI_STATUS_INVALID_PARAMETERS;
        }

        if (httpHeaders != NULL) {
            ADD_HEADER(hdrList, OI_OBEX_HDR_HTTP, http, httpHeaders);
        }
        if (jobId != NULL) {
            OI_BPP_AddJobIdHeader(&hdrList, *jobId, client->appParamData);
        }
    } else if ((type != 0) || (httpHeaders != NULL) || (jobId != NULL)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    ADD_BODY_HEADER(hdrList, final, data);

    client->cb.sendReference = cb;
    status = OI_OBEXCLI_Put(handle, &hdrList, SendReferenceCfm, final ? OI_OK : OI_OBEX_CONTINUE);

    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_SEND_FILE;
    } else {
        client->state = BPPCLI_STATE_CONNECTED;
    }
    return status;
}


OI_STATUS OI_BPP_SENDER_GetRUI(OI_OBEXCLI_CONNECTION_HANDLE handle,
                               OI_BPP_SENDER_GET_CFM cb,
                               OI_OBEX_BYTESEQ *type,
                               OI_OBEX_UNICODE *name,
                               OI_OBEX_BYTESEQ *httpHeaders,
                               OI_OBEX_BYTESEQ *body,
                               OI_BOOL final)
{
    BPP_SENDER_CLI *client;
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 6);
#else
#error code not present
#endif /* not OI_CODE */
    OI_BOOL first;
    OI_STATUS status;

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    ARGCHECK(cb != NULL);

    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    } else if (client->state == BPPCLI_STATE_CONNECTED) {
        first = TRUE;
    } else if (client->state == BPPCLI_STATE_GET_RUI) {
        first = FALSE;
    } else {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    INIT_HEADER(hdrList);

    if (first) {
        if (type == NULL) {
            return OI_STATUS_INVALID_PARAMETERS;
        }
        
        if (name != NULL) {
            ADD_HEADER(hdrList, OI_OBEX_HDR_NAME, name, name);
        }

        if (httpHeaders != NULL) {
            ADD_HEADER(hdrList, OI_OBEX_HDR_HTTP, http, httpHeaders);
        }
    } else if ((type != NULL) || (name != NULL) || (httpHeaders != NULL)) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    
    ADD_BODY_HEADER(hdrList, final, body);

    client->cb.getCfm = cb;

    status = OI_OBEXCLI_Get(handle, &hdrList, GetCfm, final);
    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_GET_RUI;
    } else {
        client->state = BPPCLI_STATE_CONNECTED;
    }
    return status;
}


static void AbortCfm(OI_OBEXCLI_CONNECTION_HANDLE handle)
{
    BPP_SENDER_CLI *client;
    
    client = LookupClient(handle);

    if (!client) {
        OI_LOG_ERROR(("Failed to find BPP client data for handle: %d\n", handle));
        return;
    }
#ifdef OI_CODE
#error code not present
#else
    if (client->cancelCfm) {
        client->cancelCfm(handle);
    }
#endif
    if (client->state == BPPCLI_STATE_ABORTING) {
        client->state = BPPCLI_STATE_CONNECTED;
    }
}


OI_STATUS OI_BPP_SENDER_Cancel(OI_OBEXCLI_CONNECTION_HANDLE handle,
                               OI_BPP_SENDER_CANCEL_CFM cancelCfm)
{
    BPP_SENDER_CLI *client;
    OI_STATUS status;
    
    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }
    
    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    
#ifndef OI_CODE
    /* If the client is in CONNECTED state this means that none of the
       operation is in progress, so ignore an Abort in this state */
    if((client->state <= BPPCLI_STATE_CONNECTED)
       || (client->state == BPPCLI_STATE_DISCONNECTING))
    {
      return OI_OBEX_INVALID_OPERATION;
    }
    client->cancelCfm = cancelCfm;
#else
#error code not present
#endif
    status = OI_OBEXCLI_Abort(handle, AbortCfm);
    if (OI_SUCCESS(status)) {
        client->state = BPPCLI_STATE_ABORTING;
    }
    return status;
}

/*************************************************************************
 *
 * BPP Sender OBEX server callbacks and registration
 *
 */

#ifdef OI_CODE
#error code not present
#else
static void ObexServerConnectInd(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                 OI_BOOL unauthorized,
                                 OI_BYTE *userId,
                                 OI_UINT8 userIdLen,
                                 OI_OBEX_HEADER_LIST *cmd_headers_ptr)
#endif
{
    BPP_SENDER_SRV *server;
    OI_STATUS status;
    OI_STATUS clientStatus;
    OI_BD_ADDR clientAddr;
    
    server = LookupServer(handle);
    
    /* This is a logic error if it happens. LookupServer should only return a
     * server record if matches the handle used at registration, and
     * registration only occurs if server callbacks are specified. */
    OI_ASSERT(server->cb != NULL);
    
    
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("unexpected obex connectInd"));
#endif
        clientStatus = OI_OBEX_INTERNAL_SERVER_ERROR;
    } else if (unauthorized) {
        /* The referenced objects servers do not use authentication */
        clientStatus = OI_OBEX_ACCESS_DENIED;
    } else if (server->state >= BPPSRV_STATE_CONNECTED) {
        /* Only one connection allowed per server */
        clientStatus = OI_STATUS_NO_RESOURCES;
    } else {
        OI_ASSERT(server->handle == 0);

        clientStatus = OI_OBEXSRV_GetClientAddr(handle, &clientAddr);
        if (OI_SUCCESS(clientStatus)) {
            OI_ASSERT(server->cb != NULL);
            clientStatus = server->cb->connectInd(handle, &clientAddr);
        }
    }
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_AcceptConnect(handle, OI_SUCCESS(clientStatus), NULL, clientStatus);
#endif
    if (OI_SUCCESS(clientStatus) && OI_SUCCESS(status)) {
        server->handle = handle;
        server->state = BPPSRV_STATE_CONNECTED;
    }
}

#ifdef OI_CODE
#error code not present
#else
static void ObexServerDisconnectInd(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                         OI_OBEX_HEADER_LIST *headers)
#endif
{
    BPP_SENDER_SRV *server;
    
    server = LookupServer(handle);
    if (server == NULL || server->state == BPPSRV_STATE_REGISTERED || server->cb == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unexpected obex disconnectInd"));
#endif
        return;
    }

    server->state = BPPSRV_STATE_REGISTERED;
    server->handle = 0;
    server->cb->disconnectInd(handle);
}

static OI_STATUS ObexServerGetInd(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                  OI_OBEX_HEADER_LIST *cmdHeaders,
                                  OI_STATUS rcvStatus)  
{
    BPP_SENDER_SRV *server;
    OI_BOOL first;
    OI_STATUS status = OI_STATUS_INVALID_STATE;
    
    server = LookupServer(handle);
    if (server == NULL || server->cb == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("getInd on handle not matching any known server"));
#endif
        return OI_OBEX_SERVICE_UNAVAILABLE;
    } else if (server->state == BPPSRV_STATE_CONNECTED) {
        first = TRUE;
    } else if (server->state == BPPSRV_STATE_GETTING) {
        first = FALSE;
    } else {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (rcvStatus == OI_OBEXSRV_INCOMPLETE_GET) {
        /* Multi-part gets are not currently supported */
        return OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
    }
    
    if (!cmdHeaders) {
        /* This signals the cleanup callback */
        server->state = BPPSRV_STATE_CONNECTED;
        (void)server->cb->getObj(handle, NULL, 0, 0, FALSE, rcvStatus);
        return OI_OK;
    }
    
    OI_ASSERT(rcvStatus == OI_OBEX_CONTINUE);
    
    if (first) {
        /* Extract the request parameters */
        OI_OBEX_HEADER *hdr;
        OI_OBEX_UNICODE *name = NULL;
        OI_UINT i;
        OI_UINT32 offset = 0;
        OI_BOOL haveOffset = FALSE;
        OI_INT32 count = 0;
        OI_BOOL haveCount = FALSE;
        OI_BOOL getFileSize = FALSE;
        OI_OBEX_APP_PARAM_LIST paramList;
        
        hdr = OI_OBEXCOMMON_FindHeader(cmdHeaders, OI_OBEX_HDR_NAME);
        if (hdr == NULL) {
            return OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        }
        name = &hdr->val.name;
        
        hdr = OI_OBEXCOMMON_FindHeader(cmdHeaders, OI_OBEX_HDR_APPLICATION_PARAMS);
        if (hdr == NULL) {
            return OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        }
        status = OI_OBEXCOMMON_ParseAppParamsHeader(&hdr->val.applicationParams, &paramList);
        if (!OI_SUCCESS(status)) {
            return OI_OBEX_BAD_PACKET;
        }
        
        status = OI_OK;
        for (i = 0; i < paramList.count; i++) {
            switch (paramList.list[i].tag) {
                case BPP_OBEX_APP_PARAM_TAG_OFFSET:
                    if (paramList.list[i].len == 4) {
                        offset = GetUINT32_BigEndian(paramList.list[i].data);
                        haveOffset = TRUE;
                    }
                    break;
                case BPP_OBEX_APP_PARAM_TAG_COUNT:
                    if (paramList.list[i].len == 4) {
                        count = (OI_INT32)GetUINT32_BigEndian(paramList.list[i].data);
                        haveCount = TRUE;
                    }
                    break;
                case BPP_OBEX_APP_PARAM_TAG_FILESIZE:
                    getFileSize = TRUE;
                    break;
            }
        }
        if (haveCount && haveOffset) {
            status = server->cb->getObj(handle, name, offset, count, getFileSize, OI_OBEX_CONTINUE);
        } else {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        }
        OI_FreeIf(&paramList.list);
    } else {
        status = server->cb->getObj(handle, NULL, 0, 0, FALSE, OI_OBEX_CONTINUE);
    }

    if (OI_SUCCESS(status)) {
        server->state = BPPSRV_STATE_GETTING;
    }
    return status;
}

OI_STATUS OI_BPP_SENDER_GetReferencedObjectsResponse(OI_OBEXSRV_CONNECTION_HANDLE handle,
                                                     OI_OBEX_BYTESEQ *data,
                                                     OI_INT32 *fileSize,
                                                     OI_STATUS status)
{
    BPP_SENDER_SRV *server;
#ifndef OI_CODE
    DECLARE_HEADER(hdrList, 3);
#else
#error code not present
#endif /* not OI_CODE */

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    server = LookupServer(handle);

    if (server == NULL || server->state < BPPSRV_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    
    INIT_HEADER(hdrList);

    if (status == OI_OK || status == OI_OBEX_CONTINUE) {

        if (fileSize) {
            OI_OBEX_BYTESEQ appParam;
            server->appParamData[0] = BPP_OBEX_APP_PARAM_TAG_FILESIZE;
            server->appParamData[1] = sizeof(*fileSize);
            SetUINT32_BigEndian(&server->appParamData[2], (OI_UINT32)*fileSize);
            appParam.data = server->appParamData;
            appParam.len = sizeof(server->appParamData);
            ADD_HEADER(hdrList, OI_OBEX_HDR_APPLICATION_PARAMS, applicationParams, &appParam);
        }

        if (data != NULL) {
            ADD_HEADER(hdrList, (status == OI_OBEX_CONTINUE) ? OI_OBEX_HDR_BODY : OI_OBEX_HDR_END_OF_BODY,
                       body, data);
        }
    }
    return OI_OBEXSRV_GetResponse(handle, &hdrList, status);
}

OI_STATUS OI_BPP_SENDER_Register(const OI_SDP_STRINGS *strings,
                                 const OI_BPP_SENDER_CALLBACKS *objectCallbacks,
                                 OI_UINT8 *objectChannelNumber,
                                 const OI_BPP_SENDER_CALLBACKS *ruiRefCallbacks,
                                 OI_UINT8 *ruiObjectChannelNumber)

{
    OI_STATUS status;
    #ifndef OI_CODE
    static OI_OBEXSRV_CB_LIST obexSrvCb = {
        ObexServerConnectInd,
        ObexServerDisconnectInd,
        ObexServerGetInd,
        NULL,
        NULL,
        NULL
    };
    #else
#error code not present
#endif /* OI_CODE */
#ifndef OI_CODE
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_DirectPrintingReferenceObjectsService, // service class UUID
        OI_SEC_REQUIRE_NOTHING,                        // don't care
        FALSE,                                         // don't care
        NULL,                                          // don't care    
        0                                              // don't care    
    } ;
#endif

    /* check our boolean initialization flag */
    if (OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_ALREADY_REGISTERED;
    }

    if (NULL != objectCallbacks) {
        ARGCHECK(objectCallbacks->connectInd);
        ARGCHECK(objectCallbacks->disconnectInd);
        ARGCHECK(objectCallbacks->getObj);
    }
    if (NULL != ruiRefCallbacks) {
        ARGCHECK(ruiRefCallbacks->connectInd);
        ARGCHECK(ruiRefCallbacks->disconnectInd);
        ARGCHECK(ruiRefCallbacks->getObj);
    }

    Sender = OI_Calloc(sizeof(*Sender));
    if (Sender == NULL) {
        return OI_STATUS_OUT_OF_MEMORY;
    }

    Sender->obj.cb = objectCallbacks;
    Sender->ruiObj.cb = ruiRefCallbacks;

    /* If the caller has specified handlers for the referenced objects server,
     * register it */
    if (Sender->obj.cb) {
#ifdef OI_CODE
#error code not present
#else
        status = OI_OBEXSRV_RegisterServer(&OI_BPP_ObexTarget_OBJ,
                                           &obexSrvCb,
                                           OI_OBEXSRV_AUTH_NONE,
                                           &Sender->obj.channel,
                                           &connectPolicy);
#endif                                                 
       if (!OI_SUCCESS(status)) {
           goto cleanup;
       }
    } else {
        Sender->obj.channel = 0;
    }
#ifndef OI_CODE
    if (ruiRefCallbacks != NULL)
    {
#endif    
        /* If the caller has specified handlers for the RUI referenced objects server,
         * register it */
        if (Sender->ruiObj.channel) {
            status = OI_OBEXSRV_RegisterServer(&OI_BPP_ObexTarget_OBJ,
                                               &obexSrvCb,
                                               OI_OBEXSRV_AUTH_NONE,
                                               &Sender->ruiObj.channel,
                                               NULL);

            if (!OI_SUCCESS(status)) {
                goto cleanup;
            }
        } else {
            Sender->ruiObj.channel = 0;
        }
    
        /* If the caller has specified any sort of referenced objects server, then
         * a service record is necessary */
        if (Sender->ruiObj.channel || Sender->obj.channel) {
            status = OI_BPP_RegisterSenderServiceRecord(strings,
                                                    &Sender->sdp,
                                                    Sender->obj.channel,
                                                    Sender->ruiObj.channel);

            if (!OI_SUCCESS(status)) {
                goto cleanup;
            }
        }
#ifndef OI_CODE
    }
#endif

    if (objectChannelNumber) {
        *objectChannelNumber = Sender->obj.channel;
    }

    if (ruiObjectChannelNumber) {
        *ruiObjectChannelNumber = Sender->ruiObj.channel;
    }
    
    /* we're initialized */
    OI_INIT_FLAG_PUT_FLAG(TRUE, BPP_SENDER) ;

    return OI_OK;
    
cleanup:

    if (Sender->obj.channel != 0) {
        (void)OI_OBEXSRV_DeregisterServer(Sender->obj.channel);
    }
    if (Sender->ruiObj.channel != 0) {
        (void)OI_OBEXSRV_DeregisterServer(Sender->ruiObj.channel);
    }

    OI_FreeIf(&Sender);
    return status;
}

OI_STATUS OI_BPP_SENDER_Deregister(void)
{
    OI_BOOL serverBusy;
    OI_BOOL clientBusy;
    OI_STATUS status;

    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }

    serverBusy = FALSE;
    if ((Sender->obj.channel != 0) && OI_OBEXSRV_IsServerBusy(Sender->obj.channel)) {
        serverBusy = TRUE;
    }
    if ((Sender->ruiObj.channel != 0) && OI_OBEXSRV_IsServerBusy(Sender->ruiObj.channel)) {
        serverBusy = TRUE;
    }

    clientBusy = (Sender->job.state != BPPCLI_STATE_IDLE)
        || (Sender->status.state != BPPCLI_STATE_IDLE)
        || (Sender->rui.state != BPPCLI_STATE_IDLE);
    
    if (serverBusy || clientBusy) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (Sender->obj.channel) {
        status = OI_OBEXSRV_DeregisterServer(Sender->obj.channel);
        if (status == OI_OBEX_OPERATION_IN_PROGRESS)
            OI_ASSERT(status != OI_OBEX_OPERATION_IN_PROGRESS); /* This should be ensured by OI_OBEXSRV_IsServerBusy */
    }

    if (Sender->ruiObj.channel) {
        status = OI_OBEXSRV_DeregisterServer(Sender->ruiObj.channel);
        if (status == OI_OBEX_OPERATION_IN_PROGRESS)
            OI_ASSERT(status != OI_OBEX_OPERATION_IN_PROGRESS);
    }
    
    OI_BPP_DeregisterSenderServiceRecord(Sender->sdp);

    OI_FreeIf(&Sender);

    /* we're no longer initialized */
    OI_INIT_FLAG_PUT_FLAG(FALSE, BPP_SENDER) ;

    return OI_OK;
}


OI_STATUS OI_BPP_SENDER_GetServiceRecord(OI_UINT32 *handle)
{
    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(BPP_SENDER);

    *handle = Sender->sdp->sdpHandle;
    return OI_OK;
}


OI_STATUS OI_BPP_SENDER_ForceDisconnect(OI_OBEXSRV_CONNECTION_HANDLE handle)
{
    OI_STATUS status = OI_FAIL;  /* initialize status to suppress compiler warning */
    BPP_SENDER_SRV *server;

    OI_CHECK_INIT(BPP_SENDER);

    server = LookupServer(handle);

    if (server) {
        switch (server->state) {
        case BPPSRV_STATE_REGISTERED:
            status = OI_OBEX_NOT_CONNECTED;
            break;

        case BPPSRV_STATE_CONNECTED:
        case BPPSRV_STATE_GETTING:
        case BPPSRV_STATE_DISCONNECTING:
            status = OI_OBEXSRV_ForceDisconnect(handle);
        }
    } else {
        OI_LOG_ERROR(("Unkown BPP PRINTER connection handle: %d", handle));
        status = OI_STATUS_INVALID_PARAMETERS;
    }

    return status;
}



#ifdef OI_DEPRECATED
/**
 * @deprecated
 */
OI_STATUS OI_BPP_SENDER_Abort(OI_OBEXCLI_CONNECTION_HANDLE handle)
{
    BPP_SENDER_CLI *client;
    
    /* check our boolean initialization flag */
    if (!OI_INIT_FLAG_VALUE(BPP_SENDER)) {
        return OI_STATUS_NOT_REGISTERED;
    }
    
    client = LookupClient(handle);
    if (client == NULL || client->state < BPPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    
    return OI_OBEXCLI_AbortOperation(client->handle);
}
#endif

