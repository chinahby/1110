
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_cli.c#1 $ 
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-07-24   gs  Fixed Klocwork error. 

*    #20        19 Feb 2008            RS
* The pending get request's callback gets cleared at every continue 
* response and so at unbond the notification shall not happen to the app.
* Still every time a get request is made the callback has to be registered.
*
*    #19        28 Jun 2007            GS
* Updated BipRequestCfm() to treat response as "final" if status
* is anything other than CONTINUE.
*
*    #18        14 May 2007            GS
* Updated OI_BIP_CLI_Disconnect() to not modify client state before
* command is sent out.
*
*    #17        10 Mar 2007            MH
* Fixed setting dispatch callback pointer to NULL as it was causing 
* subsequent put operations to fail.
*
*    #16        01 Mar 2007            MH
* Fixed issues from previous check-in.
*
*    #15        28 Feb 2007            MH
* Auth indication modified to support full access 
*
*    #14       08 Dec 2006            RP
* In "DispatchCallback" function, the callback pointer is made NULL only if the
* current operation is completed.
*   
*    #13       18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #12       19 Sept 2006           MH
*  Changed to support OI client connect
*
*    #10-11    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #9        23 Jun 2006            JH
* Modified to work with updated OBEX code which supports JSR82.
*
*    #8        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #7        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #6        19 Oct 2004            JH
* Added number of returned handles field to the list of parameters returned in 
*  the response to BIP_CMD_GET_IMAGES_LIST command.
*
*    #5        23 Jul 2004            JH
* Updated to version 3.0.12 revision 1 baseline
*
*    #4        06 Jun 2004            JH
* Modified OI_BIPCLI_Connect to use the proper UUID for connection.
* Bug fixes.
*
===========================================================================*/
/**
  @file
  @internal

  Basic Imaging Profile
  */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_BIP_CLI


#include "oi_bt_assigned_nos.h"
#include "oi_bip_cli.h"
#include "oi_bip_spec.h"
#include "oi_bip_private.h"
#include "oi_obexcli.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_securitydb.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

#include "oi_obexcommon.h"


typedef void (*ANY_FUNC)(OI_BIPCLI_HANDLE connection);


typedef union {
    OI_BIPCLI_GET_CAPABILITIES_CFM getCapabilities;
    OI_BIPCLI_PUT_IMAGE_CFM putImage;
    OI_BIPCLI_PUT_LINKED_THUMBNAIL_CFM putLinkedThumbnail;
    OI_BIPCLI_PUT_LINKED_ATTACHMENT_CFM putLinkedAttachment;
    OI_BIPCLI_REMOTE_DISPLAY_CFM remoteDisplay;
    OI_BIPCLI_GET_IMAGES_LIST_CFM getImagesList;
    OI_BIPCLI_GET_IMAGE_PROPERTIES_CFM getImageProperties;
    OI_BIPCLI_GET_IMAGE_CFM getImage;
    OI_BIPCLI_GET_LINKED_THUMBNAIL_CFM getLinkedThumbnail;
    OI_BIPCLI_GET_LINKED_ATTACHMENT_CFM getLinkedAttachment;
    OI_BIPCLI_DELETE_IMAGE_CFM deleteImage;
    OI_BIPCLI_START_PRINT_CFM startPrint;
    OI_BIPCLI_GET_PARTIAL_IMAGE_CFM getPartialImage;
    OI_BIPCLI_START_ARCHIVE_CFM startArchive;
    OI_BIPCLI_GET_MONITORING_IMAGE_CFM getMonitoringImage;
    OI_BIPCLI_GET_STATUS_CFM getStatus;
    ANY_FUNC func;
} BIP_CALLBACK_UNION;


static OI_STATUS BIP_GetRequest(OI_BIPCLI_HANDLE connection,
                                BIP_CMD currentFunction,
                                BIP_CALLBACK_UNION callback,
                                OI_BIP_GENERIC_PARAMS params,
                                const OI_OBEX_BYTESEQ *type,
                                const OI_OBEX_UNICODE *name,
                                const OI_OBEX_BYTESEQ *descriptor,
                                const OI_OBEX_UNICODE *handle,
                                const OI_BIP_APP_PARAM_BOOKKEEPING *appParam);

static OI_STATUS BIP_PutRequest(OI_BIPCLI_HANDLE connection,
                                BIP_CMD currentFunction,
                                BIP_CALLBACK_UNION callback,
                                OI_BIP_GENERIC_PARAMS params,
                                const OI_OBEX_BYTESEQ *type,
                                const OI_OBEX_UNICODE *name,
                                const OI_OBEX_BYTESEQ *descriptor,
                                const OI_OBEX_UNICODE *handle,
                                const OI_BIP_APP_PARAM_BOOKKEEPING *appParam,
                                const OI_OBEX_BYTESEQ *data,
                                OI_BOOL final);
/** Implementation */

/** This type is never instantiated. Rather, it exists to document the types and
 * sizes of the application parameters used by the BIP request functions.
 * Compiler-added padding may cause the size of this data structure to be
 * slightly overestimated. This should be minimal.*/
union _OI_BIP_CLI_APP_PARAM_ELEMENTS {
    struct {
        OI_UINT8 remoteDisplay;
    } remoteDisplayParams;

    struct {
        OI_UINT16 nbReturnedHandles;
        OI_UINT16 listStartOffset;
        OI_UINT8 latestCapturedImages;
    } getImagesListParams;

    struct {
        OI_UUID128 serviceId;
    } startPrintParams;

    struct {
        OI_UINT32 partialFileLength;
        OI_UINT32 partialFileStartOffset;
    } getPartialImageParams;

    struct {
        OI_UINT128 serviceId;
    } startArchiveParams;

    struct {
        OI_UINT8 storeFlag;
    } getMonitoringImageParams;

};

#define MAX_PARAM_ELEMENTS 3    /* No BIP function uses more than 3 application parameters */

#define MAX_PARAM_DATA_BYTES ((MAX_PARAM_ELEMENTS * OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN) + sizeof(union _OI_BIP_CLI_APP_PARAM_ELEMENTS))

typedef enum {
    BIPCLI_STATE_NOT_CONNECTED,
    BIPCLI_STATE_CONNECTING,
    BIPCLI_STATE_CONNECTED,
    BIPCLI_STATE_REQUEST_IN_PROGRESS,
    BIPCLI_STATE_ABORTING
} BIPCLI_STATE;

typedef struct {
    void *context;      /**< Application context set by OI_BIPCLI_SetContext */
    OI_CONNECT_POLICY connectPolicy;

    OI_BIP_APP_PARAM_BOOKKEEPING param;
    OI_BYTE paramData[MAX_PARAM_DATA_BYTES];
    
    BIPCLI_STATE state;
    
    OI_BIPCLI_AUTH_IND authInd;
    OI_BIPCLI_CONNECT_CFM connectCfm;
    OI_BIPCLI_DISCONNECT_IND disconnectInd;
    OI_BIPCLI_CANCEL_CFM cancelCfm;

    union {
        struct {
            OI_OBEX_HEADER obexHeader[1];
            OI_OBEX_HEADER_LIST obexHeaderList;
            OI_BYTE uuidBytes[OI_BT_UUID128_SIZE];
        } connection;
        struct {
            BIP_CMD currentFunction;
            BIP_CALLBACK_UNION callback;
            OI_BIP_GENERIC_PARAMS params;
            void *paramData;
            OI_BIP_APP_PARAM_BOOKKEEPING param;
            BIP_REQUEST_HEADERS reqHdrs;
        } operation;
    } data;
} BIP_CLIENT_DATA;



/**
 * Helper macro to get the Bip Client pointer from the OBEX connection handle
 */
#define BipClient(c) ((BIP_CLIENT_DATA*)OI_OBEXCLI_GetConnectionContext((OI_OBEXCLI_CONNECTION_HANDLE)(c)))



 
/** This is a helper macro to cast the saved callback function to the
 * appropriate type and invoke it with the required arguments. As an example:
 * INVOKE_CALLBACK(getCapabilities, (client->data.operation.savedData, result))
 */
#define INVOKE_CALLBACK(_function, _args) cb._function _args

#define PARAM_STRUCT(_function) ((OI_BIP_##_function##_PARAMS *)client->data.operation.params)

static OI_STATUS internalTerminate(BIP_CLIENT_DATA *client);


static void DispatchCallback(BIP_CLIENT_DATA *client,
                             OI_BIPCLI_HANDLE connection,
                             BIP_CMD command,
                             OI_STATUS status)
{
    BIP_CALLBACK_UNION cb = client->data.operation.callback;

#ifndef OI_CODE
    /* Droping the notification is ok for the GET Status requests as well  
     * because for every get request we want to issue to check the  
     * status on the secondary channel we would register a fresh callback.
     */
    if ( client->data.operation.callback.func ) {
        client->data.operation.callback.func = NULL;
    } else {
        /* No function registered to be notified. */
        OI_LOG_ERROR(("Event could not be notified as no cb was available:"));
        return;
    }
#else
#error code not present
#endif

    OI_DBGPRINT(("DispatchCallback(%d, %!)", command, status));

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: BIP Dispath callback Command:%x Status:%x", command, status, 0 );
#endif

    switch (client->data.operation.currentFunction) {
        case BIP_CMD_GET_STATUS:
            OI_DBGPRINT2(("BIP_CMD_GET_STATUS"));
            INVOKE_CALLBACK(getStatus, (connection, status));
            break;
            
        case BIP_CMD_START_ARCHIVE:
            OI_DBGPRINT2(("BIP_CMD_START_ARCHIVE"));
            INVOKE_CALLBACK(startArchive, (connection, status));
            break;
            
        case BIP_CMD_DELETE_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_DELETE_IMAGE"));
            INVOKE_CALLBACK(deleteImage, (connection, status));
            break;
            
        case BIP_CMD_GET_CAPABILITIES:
            OI_DBGPRINT2(("BIP_CMD_GET_CAPABILITIES"));
            PARAM_STRUCT(GET_CAPABILITIES)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_CAPABILITIES)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getCapabilities, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_PUT_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_PUT_IMAGE"));
            PARAM_STRUCT(PUT_IMAGE)->response.imageHandle = client->data.operation.reqHdrs.handle;
            INVOKE_CALLBACK(putImage, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_PUT_LINKED_THUMBNAIL:
            OI_DBGPRINT2(("BIP_CMD_PUT_LINKED_THUMBNAIL"));
            INVOKE_CALLBACK(putLinkedThumbnail, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_PUT_LINKED_ATTACHMENT:
            OI_DBGPRINT2(("BIP_CMD_PUT_LINKED_ATTACHMENT"));
            INVOKE_CALLBACK(putLinkedAttachment, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_REMOTE_DISPLAY:
            OI_DBGPRINT2(("BIP_CMD_REMOTE_DISPLAY"));
            PARAM_STRUCT(REMOTE_DISPLAY)->response.imageHandle = client->data.operation.reqHdrs.handle;
            INVOKE_CALLBACK(remoteDisplay, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_GET_IMAGES_LIST:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGES_LIST"));
            PARAM_STRUCT(GET_IMAGES_LIST)->response.nbReturnedHandles = client->data.operation.reqHdrs.nbReturnedHandles;
            PARAM_STRUCT(GET_IMAGES_LIST)->response.imageHandlesDescriptor = client->data.operation.reqHdrs.descriptor;
            PARAM_STRUCT(GET_IMAGES_LIST)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_IMAGES_LIST)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getImagesList, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_GET_IMAGE_PROPERTIES:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGE_PROPERTIES"));
            PARAM_STRUCT(GET_IMAGE_PROPERTIES)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_IMAGE_PROPERTIES)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getImageProperties, (connection, client->data.operation.params, status));
            break;

        case BIP_CMD_GET_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGE"));
            PARAM_STRUCT(GET_IMAGE)->response.imageFileSize = client->data.operation.reqHdrs.length;
            PARAM_STRUCT(GET_IMAGE)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_IMAGE)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getImage, (connection, client->data.operation.params, status));
            break;

        case BIP_CMD_GET_LINKED_THUMBNAIL:
            OI_DBGPRINT2(("BIP_CMD_GET_LINKED_THUMBNAIL"));
            PARAM_STRUCT(GET_LINKED_THUMBNAIL)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_LINKED_THUMBNAIL)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getLinkedThumbnail, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_GET_LINKED_ATTACHMENT:
            OI_DBGPRINT2(("BIP_CMD_GET_LINKED_ATTACHMENT"));
            PARAM_STRUCT(GET_LINKED_ATTACHMENT)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_LINKED_ATTACHMENT)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getLinkedAttachment, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_START_PRINT:
            OI_DBGPRINT2(("BIP_CMD_START_PRINT"));
            INVOKE_CALLBACK(startPrint, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_GET_PARTIAL_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_PARTIAL_IMAGE"));
            PARAM_STRUCT(GET_PARTIAL_IMAGE)->response.partialLength = client->data.operation.reqHdrs.length;
            PARAM_STRUCT(GET_PARTIAL_IMAGE)->response.totalFileSize = client->data.operation.reqHdrs.totalFileSize;
            PARAM_STRUCT(GET_PARTIAL_IMAGE)->response.endFlag = client->data.operation.reqHdrs.endFlag;
            PARAM_STRUCT(GET_PARTIAL_IMAGE)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_PARTIAL_IMAGE)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getPartialImage, (connection, client->data.operation.params, status));
            break;
            
        case BIP_CMD_GET_MONITORING_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_MONITORING_IMAGE"));
            PARAM_STRUCT(GET_MONITORING_IMAGE)->response.imageHandle = client->data.operation.reqHdrs.handle;
            PARAM_STRUCT(GET_MONITORING_IMAGE)->response.data = client->data.operation.reqHdrs.body;
            PARAM_STRUCT(GET_MONITORING_IMAGE)->response.final = client->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(getMonitoringImage, (connection, client->data.operation.params, status));
            break;

      default:
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("Invalid client->currentFunction in BipRequestCfm:"));
#endif
    }
}


/*
 * BIP requests can be divided into five categories:
 * 
 * Simple: RemoteDisplay, DeleteImage, StartArchive, GetStatus
 *      The Client sends a single-packet request and receives a single packet
 *      reply, with or without headers on either side.
 *      
 * Upload with response parameters: PutImage
 *      The Client sends a (possibly) multi-packet request. The first packet
 *      contains parameters and body data. Subsequent packets contain more body
 *      data if necessary. After the Client signals its final packet, the server
 *      responds with a single packet, containing a reply parameter.
 * 
 * Upload without response params: PutLinkedThumbnail, PutLinkedAttachment, StartPrint
 *      The Client sends a (possibly) multi-packet request. The first packet
 *      contains parameters and body data. Subsequent packets contain more body
 *      data if necessary. The server responds only with CONTINUE, OK, or ERROR
 *      codes.
 * 
 * Download with response params: GetImagesList, GetImage, GetPartialImage,
 *                                GetMonitoringImage
 *      The Client sends a single-packet request. This packet contains
 *      request parameters. The server sends a multi-packet response with the
 *      first packet containing reply parameters and body data.
 *
 * Download without response params: GetCapabilities, GetImageProperties,
 *                                   GetLinkedThumbnail, GetLinkedAttachment
 *      The Client sends a single-packet request, possibly containing request
 *      parameters. The server sends a multi-packet response consisting of body
 *      data only.
 * 
 */

static void BipRequestCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                          OI_OBEX_HEADER_LIST *rspHeaders,
                          OI_STATUS status)
{
    BIP_CLIENT_DATA *client = BipClient(handle);
    OI_BOOL final;

#ifndef OI_CODE
    if (!client) {
        BT_ERR("BipRequestCfm: client ptr is NULL! handle=%x", 
               handle, 0, 0);
        return;
    }
#endif
    
    OI_ASSERT(client->data.operation.currentFunction != BIP_CMD_NONE);
    OI_DBGPRINT(("BipRequestCfm: %!", status));

    final = (status != OI_OBEX_CONTINUE);
    
    if (final) {
        OI_DBGPRINT(("Request complete."));
        OI_DBGPRINT2(("client->state = BIP_STATE_CONNECTED"));
        client->state = BIPCLI_STATE_CONNECTED;
    }
    
    OI_BIP_UnmarshallHeaderData(&client->data.operation.reqHdrs, rspHeaders);
#ifndef OI_CODE
    /* Over-ride "final" setting in UnmarshallHeaderData(). Response is
       "final" if status is not CONTINUE.  
       There are some dubious implementations which send no end-of-body header 
       but with SUCCESS response.  We need to prevent ourselves from getting 
       stuck in an infinite loop of sending GET requests repeatedly.
     */
    client->data.operation.reqHdrs.final = final;
#endif

    DispatchCallback(client, handle, client->data.operation.currentFunction, status);
}

static OI_STATUS BIP_GetRequest(OI_BIPCLI_HANDLE connection,
                                BIP_CMD function,
                                BIP_CALLBACK_UNION callback,
                                OI_BIP_GENERIC_PARAMS params,
                                const OI_OBEX_BYTESEQ *type,
                                const OI_OBEX_UNICODE *name,
                                const OI_OBEX_BYTESEQ *descriptor,
                                const OI_OBEX_UNICODE *handle,
                                const OI_BIP_APP_PARAM_BOOKKEEPING *appParam)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    OI_STATUS status;
    OI_BOOL first;
    OI_OBEX_HEADER hdrs[5];
    OI_OBEX_HEADER_LIST hdrList;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    ARGCHECK(type != NULL);

    OI_DBGPRINT(("Issuing GET request for %s", OI_BIP_CommandString(function)));
    OI_DBGPRINT2(("client->state = %d", client->state));
    
    if (client->state < BIPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->data.operation.callback.func != NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("BIP GET request while request outstanding"));
#endif
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    if (client->state == BIPCLI_STATE_REQUEST_IN_PROGRESS && function != client->data.operation.currentFunction) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (client->state == BIPCLI_STATE_CONNECTED) {
        first = TRUE;
        client->state = BIPCLI_STATE_REQUEST_IN_PROGRESS;
        OI_DBGPRINT2(("client->state = BIPCLI_STATE_REQUEST_IN_PROGRESS"));
    } else {
        first = FALSE;
    }
    
    hdrList.list = hdrs;
    hdrList.count = 0;

    if (first) {
        hdrList.list[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrList.list[hdrList.count].val.type = *type;
        hdrList.count++;

        if (handle != NULL) {
            hdrList.list[hdrList.count].id = OI_BIP_OBEX_HDR_IMG_HANDLE;
            hdrList.list[hdrList.count].val.unicode = *handle;
            hdrList.count++;
        }

        if (name != NULL) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_NAME;
            hdrList.list[hdrList.count].val.name = *name;
            hdrList.count++;
        }

        if (appParam != NULL) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
            hdrList.list[hdrList.count].val.applicationParams = appParam->byteseq;
            hdrList.count++;
        }

        if (descriptor != NULL) {
            hdrList.list[hdrList.count].id = OI_BIP_OBEX_HDR_IMG_DESCRIPTOR;
            hdrList.list[hdrList.count].val.byteseq = *descriptor;
            hdrList.count++;
        }
    }

    status = OI_OBEXCLI_Get(connection, &hdrList, BipRequestCfm, TRUE);
    if (OI_SUCCESS(status)) {
        client->data.operation.currentFunction = function;
        client->data.operation.callback = callback;
        client->data.operation.params = params;
    } else {
        client->data.operation.currentFunction = BIP_CMD_NONE;
        client->data.operation.params = NULL;
    }

    return status;
}

static OI_STATUS BIP_PutRequest(OI_BIPCLI_HANDLE connection,
                                BIP_CMD function,
                                BIP_CALLBACK_UNION callback,
                                OI_BIP_GENERIC_PARAMS params,
                                const OI_OBEX_BYTESEQ *type,
                                const OI_OBEX_UNICODE *name,
                                const OI_OBEX_BYTESEQ *descriptor,
                                const OI_OBEX_UNICODE *handle,
                                const OI_BIP_APP_PARAM_BOOKKEEPING *appParam,
                                const OI_OBEX_BYTESEQ *data,
                                OI_BOOL final)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    OI_STATUS status;
    OI_BOOL first;
    OI_OBEX_HEADER hdrs[6];
    OI_OBEX_HEADER_LIST hdrList;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    OI_DBGPRINT(("Issuing PUT request for %s", OI_BIP_CommandString(function)));
    OI_DBGPRINT2(("client->state = %d", client->state));

    ARGCHECK(type != NULL);
    
    if (client->state < BIPCLI_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    if (client->data.operation.callback.func != NULL) {
#ifdef OI_CODE
#error code not present
#else
       OI_LOG_ERROR(("BIP PUT request while request outstanding"));
#endif
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    if (client->state == BIPCLI_STATE_REQUEST_IN_PROGRESS && function != client->data.operation.currentFunction) {
        OI_LOG_ERROR(("BIP PUT request while other BIP operation already in progress"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (client->state == BIPCLI_STATE_CONNECTED) {
        first = TRUE;
    } else {
        first = FALSE;
    }
    

    hdrList.list = hdrs;
    hdrList.count = 0;

    if (first) {
        hdrList.list[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrList.list[hdrList.count].val.type = *type;
        hdrList.count++;

        if (handle != NULL) {
            hdrList.list[hdrList.count].id = OI_BIP_OBEX_HDR_IMG_HANDLE;
            hdrList.list[hdrList.count].val.unicode = *handle;
            hdrList.count++;
        }

        if (name != NULL) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_NAME;
            hdrList.list[hdrList.count].val.name = *name;
            hdrList.count++;
        }

        if (appParam != NULL) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
            hdrList.list[hdrList.count].val.applicationParams = appParam->byteseq;
            hdrList.count++;
        }

        if (descriptor != NULL) {
            hdrList.list[hdrList.count].id = OI_BIP_OBEX_HDR_IMG_DESCRIPTOR;
            hdrList.list[hdrList.count].val.byteseq = *descriptor;
            hdrList.count++;
        }
    }

    if (data != NULL) {
        hdrList.list[hdrList.count].id = final ? OI_OBEX_HDR_END_OF_BODY : OI_OBEX_HDR_BODY;
        hdrList.list[hdrList.count].val.body = *data;
        hdrList.count++;
    }

    status = OI_OBEXCLI_Put(connection, &hdrList, BipRequestCfm, final ? OI_OK : OI_OBEX_CONTINUE);
    if (OI_SUCCESS(status)) {
        client->data.operation.currentFunction = function;
        client->data.operation.callback = callback;
        client->data.operation.params = params;
        client->state = BIPCLI_STATE_REQUEST_IN_PROGRESS;
        OI_DBGPRINT2(("client->state = BIPCLI_STATE_REQUEST_IN_PROGRESS"));
    } else {
        client->data.operation.currentFunction = BIP_CMD_NONE;
        client->data.operation.params = NULL;
        client->state = BIPCLI_STATE_CONNECTED;
        OI_DBGPRINT2(("client->state = BIPCLI_STATE_CONNECTED"));
    }

    return status;
}

static void obexConnectCfm(OI_OBEXCLI_CONNECTION_HANDLE connection,
                           OI_BOOL readOnly,
#ifndef OI_CODE
                           OI_OBEX_HEADER_LIST *hdrs, /* ignored */
#endif
                           OI_STATUS status)
{
    BIP_CLIENT_DATA *client = BipClient(connection);

    if (client == NULL || client->state != BIPCLI_STATE_CONNECTING) {
        OI_DBGPRINT(("Received unexpected OBEX connect confirmation"));
    } else {
        OI_DBGPRINT(("obexConnectCfm: %!", status));
        if (OI_SUCCESS(status)) {
            OI_DBGPRINT2(("client->state = BIPCLI_STATE_CONNECTED"));
            client->state = BIPCLI_STATE_CONNECTED;
        } else {
            client->state = BIPCLI_STATE_NOT_CONNECTED;
        }
        OI_ASSERT(client->connectCfm);
        /*
         * Clean out operation state and confirm the connection
         */
        OI_MemZero(&client->data.operation, sizeof(client->data.operation));
        client->connectCfm((OI_BIPCLI_HANDLE)connection, status);
        /*
         * Cleanup if the connect failed.
         */
        if (!OI_SUCCESS(status)) {
            internalTerminate(client);
        }
    }
}

#ifdef OI_CODE
#error code not present
#else
static void obexDisconnectInd(OI_OBEXCLI_CONNECTION_HANDLE connection,
                              OI_OBEX_HEADER_LIST *hdrs) /* ignored */
#endif
{
    BIP_CLIENT_DATA *client = BipClient(connection);

#ifndef OI_CODE
    if (!client) {
        BT_ERR("obexDisconnectInd: client ptr is NULL!", 0, 0, 0);
        return;
    }
#endif

    OI_DBGPRINT(("Obex disconnected"));
    OI_ASSERT(client != NULL);
    OI_ASSERT(client->disconnectInd != NULL);

    OI_DBGPRINT2(("client->state = BIPCLI_STATE_NOT_CONNECTED"));
    client->state = BIPCLI_STATE_NOT_CONNECTED;
    client->disconnectInd(connection);
    internalTerminate(client);
}

static void obexAuthChallengeInd(OI_OBEXCLI_CONNECTION_HANDLE connection, 
#ifndef OI_CODE
                                 OI_BOOL fullAccess,
#endif
                                 OI_BOOL userIdRequired)
{
    BIP_CLIENT_DATA *client = BipClient(connection);

    OI_DBGPRINT(("Obex credentials requested"));
    if (userIdRequired || client->authInd == NULL) {
        OI_OBEXCLI_AuthenticationResponse(connection, NULL, 0, NULL);
    } else {
        client->authInd(connection);
    }
    
}

void OI_BIPCLI_AuthenticationResponse(OI_BIPCLI_HANDLE connection,
                                      const OI_CHAR *pin)
{
    OI_OBEXCLI_AuthenticationResponse(connection, NULL, 0, pin);
}


static OI_STATUS internalTerminate(BIP_CLIENT_DATA *client)
{
    /*
     * Release our memory
     */
    OI_Free(client);
    OI_INIT_FLAG_DECREMENT(BIP_CLI);
    return(OI_OK) ;
}

OI_STATUS OI_BIPCLI_Connect(OI_BD_ADDR *addr,
                            OI_UINT8 channel,
                            OI_OBEX_BYTESEQ const *target,
                            OI_BIPCLI_AUTH_IND authInd,
                            OI_BIPCLI_CONNECT_CFM connectCfm,
                            OI_BIPCLI_DISCONNECT_IND disconnectInd,
                            OI_BIPCLI_HANDLE *handle)
{
    BIP_CLIENT_DATA *client;
    OI_STATUS status;
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_Imaging,            // OI_UINT32           serviceUuid32 ;
        OI_SEC_REQUIRE_NOTHING,     // securityPolicy is a place holder, get real policy at runtime
        FALSE,                      // OI_BOOL             mustBeMaster ;
        NULL,                       // OI_L2CAP_FLOWSPEC   *flowspec;    
        0                           // OI_UINT8            powerSavingDisables ;    
    } ;
#ifndef OI_CODE                                                                  
    OI_BYTE refObjObexTargetUuid[16] = OI_BIP_REFERENCED_OBJECTS_OBEX_TARGET_UUID;
    OI_BYTE archObjObexTargetUuid[16] = OI_BIP_ARCHIVED_OBJECTS_OBEX_TARGET_UUID;
#endif /* OI_CODE */
    
    ARGCHECK(connectCfm != NULL);
    ARGCHECK(disconnectInd != NULL);
    ARGCHECK(target != NULL);
    ARGCHECK(handle != NULL);

    OI_DBGPRINT(("Connecting"));

#ifndef OI_CODE                            
    if ( memcmp( target->data, refObjObexTargetUuid, 16 ) == 0 )
    {
      connectPolicy.serviceUuid32 = OI_UUID_ImagingReferencedObjects;
    }
    else if ( memcmp( target->data, archObjObexTargetUuid, 16 ) == 0 )
    {
      connectPolicy.serviceUuid32 = OI_UUID_ImagingAutomaticArchive;
    }
    else
    {
      connectPolicy.serviceUuid32 = OI_UUID_ImagingResponder;
    }
#endif /* OI_CODE */
    
    client = OI_Calloc(sizeof(BIP_CLIENT_DATA));
    if (!client) {
        return OI_STATUS_OUT_OF_MEMORY;
    }
    client->state = BIPCLI_STATE_NOT_CONNECTED;
    client->authInd = authInd;
    client->connectCfm = connectCfm;
    client->disconnectInd = disconnectInd;

    client->data.connection.obexHeader[0].id = OI_OBEX_HDR_TARGET;
    client->data.connection.obexHeader[0].val.target = *target;
    
    client->data.connection.obexHeaderList.count = 1;
    client->data.connection.obexHeaderList.list = client->data.connection.obexHeader;

    // get security policy.
    client->connectPolicy = connectPolicy;
    client->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(client->connectPolicy.serviceUuid32);

    OI_INIT_FLAG_INCREMENT(BIP_CLI);

    status = OI_OBEXCLI_Connect(addr,
                                channel,
                                &client->data.connection.obexHeaderList,
                                obexConnectCfm,
                                obexDisconnectInd,
                                obexAuthChallengeInd,
#ifndef OI_CODE
                                NULL, /*Challenge Response Call back */
#endif
                                handle,
                                &client->connectPolicy);

    if (OI_SUCCESS(status)) {
        OI_DBGPRINT2(("client->state = BIPCLI_STATE_CONNECTING"));
        client->state = BIPCLI_STATE_CONNECTING;
        /*
         * Associate the "client" pointer with the OBEX client handle
         */
        OI_OBEXCLI_SetConnectionContext(*handle, (void*)client);
    } else {
        internalTerminate(client);
    }
    
    return status;
}

OI_STATUS OI_BIPCLI_Disconnect(OI_BIPCLI_HANDLE connection)
{
   BIP_CLIENT_DATA *client = BipClient(connection);

    if (!client || (client->state == BIPCLI_STATE_NOT_CONNECTED)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
#ifdef OI_CODE
#error code not present
#else
    return OI_OBEXCLI_Disconnect(connection, NULL);
#endif
}



/*** Requests ***********************************************************/

/*
 * GetCapabilities
 * OBEX GET
 * Type: x-bt/img-capabilities
 *
 */
OI_STATUS OI_BIPCLI_GetCapabilities(OI_BIPCLI_HANDLE connection,
                                    OI_BIPCLI_GET_CAPABILITIES_CFM cb,
                                    OI_BIP_GET_CAPABILITIES_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    ARGCHECK(params != NULL);
    cbu.getCapabilities = cb;
    
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_CAPABILITIES,
                          cbu,
                          params,
                          &OI_BIP_CapabilitiesType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          NULL /* image handle */,
                          NULL /* OBEX App Params */);
}

/*
 * PutImage
 * OBEX PUT
 * Type: x-bt/img-img
 * Name: imageName
 * Img-Descriptor:  imageDescriptor
 * Body/EndOfBody:  imageData
 * 
 */
OI_STATUS OI_BIPCLI_PutImage(OI_BIPCLI_HANDLE connection,
                             OI_BIPCLI_PUT_IMAGE_CFM cb,
                             OI_BIP_PUT_IMAGE_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
    ARGCHECK(params != NULL);
    if (client->state == BIPCLI_STATE_CONNECTED) {
        ARGCHECK(params->request.imageName != NULL);
        ARGCHECK(params->request.imageDescriptor != NULL);
    }
    cbu.putImage = cb;

    return BIP_PutRequest(connection,
                          BIP_CMD_PUT_IMAGE,
                          cbu,
                          params,
                          &OI_BIP_ImageType,
                          params->request.imageName,
                          params->request.imageDescriptor,
                          NULL /* image handle */,
                          NULL /* OBEX App Params */,
                          params->request.data,
                          params->request.final);

}


/*
 * PutLinkedThumbnail
 * OBEX PUT
 * Type: x-bt/img-thm
 * Img-Handle: <imageHandle>
 * Body/EndOfBody: <imageData>
 */
OI_STATUS OI_BIPCLI_PutLinkedThumbnail(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_PUT_LINKED_THUMBNAIL_CFM cb,
                                       OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
    ARGCHECK(params != NULL);
    if (client->state == BIPCLI_STATE_CONNECTED) {
        ARGCHECK(params->request.imageHandle != NULL);
    }
    cbu.putLinkedThumbnail = cb;
    return BIP_PutRequest(connection,
                          BIP_CMD_PUT_LINKED_THUMBNAIL,
                          cbu,
                          params,
                          &OI_BIP_ThumbnailType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          NULL /* App Params */,
                          params->request.data,
                          params->request.final);
}

/*
 * PutLinkedAttachment
 * OBEX PUT
 * Type: x-bt/img-attachment
 * Img-Handle: <imageHandle>
 * Img-Description: <attachmentDescriptor>
 * Body/EndOfBody: <attachmentData>
 */
OI_STATUS OI_BIPCLI_PutLinkedAttachment(OI_BIPCLI_HANDLE connection,
                                        OI_BIPCLI_PUT_LINKED_ATTACHMENT_CFM cb,
                                        OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.putLinkedAttachment = cb;
    return BIP_PutRequest(connection,
                          BIP_CMD_PUT_LINKED_ATTACHMENT,
                          cbu,
                          params,
                          &OI_BIP_AttachmentType,
                          NULL /* name */,
                          params->request.attachmentDescriptor,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */,
                          params->request.data,
                          params->request.final);
}

/*
 * RemoteDisplay
 * OBEX PUT
 * Type: x-bt/img-display
 * Img-Handle: <imageHandle>
 * Params:
 *     RemoteDisplay<UINT8> remoteDisplayCommand
 */
OI_STATUS OI_BIPCLI_RemoteDisplay(OI_BIPCLI_HANDLE connection,
                                  OI_BIPCLI_REMOTE_DISPLAY_CFM cb,
                                  OI_BIP_REMOTE_DISPLAY_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
    OI_BIP_InitAppParams(&client->param,
                         client->paramData,
                         MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUINT8(&client->param,
                               OI_BIP_OBEX_APP_PARAM_REMOTE_DISPLAY,
                               params->request.displayCommand);
    OI_BIP_FinishAppParams(&client->param);
    cbu.remoteDisplay = cb;
    return BIP_PutRequest(connection,
                          BIP_CMD_REMOTE_DISPLAY,
                          cbu,
                          params,
                          &OI_BIP_DisplayType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          &client->param,
                          NULL /* data */,
                          TRUE);
}

/*
 * GetImagesList
 * OBEX GET
 * Type: x-bt/img-listing
 * Params:
 *     NbReturnedHandles<UINT16_BE> handleCount
 *     ListStartOffset<UINT16_BE>   handleOffset
 *     LatestCapturedImages<UINT8>  latest
 * Img-Description: <handlesDescriptor>
 */
OI_STATUS OI_BIPCLI_GetImagesList(OI_BIPCLI_HANDLE connection,
                                  OI_BIPCLI_GET_IMAGES_LIST_CFM cb,
                                  OI_BIP_GET_IMAGES_LIST_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;
    
    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
    OI_BIP_InitAppParams(&client->param,
                         client->paramData,
                         MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUINT16(&client->param,
                                OI_BIP_OBEX_APP_PARAM_NB_RETURNED_HANDLES,
                                params->request.handleCount);
    OI_BIP_AppendAppParamUINT16(&client->param,
                                OI_BIP_OBEX_APP_PARAM_LIST_START_OFFSET,
                                params->request.handleOffset);
    OI_BIP_AppendAppParamUINT8(&client->param,
                               OI_BIP_OBEX_APP_PARAM_LATEST_CAPTURED_IMAGES,
                               params->request.latest);
    OI_BIP_FinishAppParams(&client->param);
    cbu.getImagesList = cb;
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_IMAGES_LIST,
                          cbu,
                          params,
                          &OI_BIP_ImageListType,
                          NULL /* name */,
                          params->request.handlesDescriptor,
                          NULL /* image handle */,
                          &client->param);

}

/*
 * GetImageProperties
 * OBEX GET
 * Type: x-bt/img-properties
 * Img-Handle: <imageHandle>
 */
OI_STATUS OI_BIPCLI_GetImageProperties(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_IMAGE_PROPERTIES_CFM cb,
                                       OI_BIP_GET_IMAGE_PROPERTIES_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.getImageProperties = cb;
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_IMAGE_PROPERTIES,
                          cbu,
                          params,
                          &OI_BIP_PropertiesType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */);
}

/*
 * GetImage
 * OBEX GET
 * Type: x-bt/img-img
 * Img-Handle:         <imageHandle>
 * Img-Description:    <imageDescriptor>
 */

OI_STATUS OI_BIPCLI_GetImage(OI_BIPCLI_HANDLE connection,
                             OI_BIPCLI_GET_IMAGE_CFM cb,
                             OI_BIP_GET_IMAGE_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.getImage = cb;
    
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_IMAGE,
                          cbu,
                          params,
                          &OI_BIP_ImageType,
                          NULL /* name */,
                          params->request.imageDescriptor,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */);
}

/*
 * GetLinkedThumbnail
 * OBEX GET
 * Type: x-bt/img-thm
 * Img-Handle:         <image handle>
 */
OI_STATUS OI_BIPCLI_GetLinkedThumbnail(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_LINKED_THUMBNAIL_CFM cb,
                                       OI_BIP_GET_LINKED_THUMBNAIL_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.getLinkedThumbnail = cb;
    
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_LINKED_THUMBNAIL,
                          cbu,
                          params,
                          &OI_BIP_ThumbnailType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */);
}


/*
 * GetLinkedAttachment
 * OBEX GET
 * Type:   x-bt/img-attachment
 * Img-Handle:         <image handle>
 * Name:               <attachment name>
 */
OI_STATUS OI_BIPCLI_GetLinkedAttachment(OI_BIPCLI_HANDLE connection,
                                        OI_BIPCLI_GET_LINKED_ATTACHMENT_CFM cb,
                                        OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.getLinkedAttachment = cb;
    
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_LINKED_ATTACHMENT,
                          cbu,
                          params,
                          &OI_BIP_AttachmentType,
                          params->request.attachmentName,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */);
}

/*
 * DeleteImage
 * OBEX PUT
 * Type:   x-bt/img-img
 * Img-Handle:     <image handle>
 */
OI_STATUS OI_BIPCLI_DeleteImage(OI_BIPCLI_HANDLE connection,
                                OI_BIPCLI_DELETE_IMAGE_CFM cb,
                                OI_BIP_DELETE_IMAGE_PARAMS *params)
{
    BIP_CALLBACK_UNION cbu;
    cbu.deleteImage = cb;
    
    return BIP_PutRequest(connection,
                          BIP_CMD_DELETE_IMAGE,
                          cbu,
                          params,
                          &OI_BIP_ImageType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          params->request.imageHandle,
                          NULL /* OBEX App Params */,
                          NULL /* data */,
                          TRUE);
}

/*
 * StartPrint
 * OBEX PUT
 * Type:   x-bt/img-print
 * Params:
 *     ServiceID<UUID128>
 * Body/EndOfBody
 */
OI_STATUS OI_BIPCLI_StartPrint(OI_BIPCLI_HANDLE connection,
                               OI_BIPCLI_START_PRINT_CFM cb,
                               OI_BIP_START_PRINT_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;
    cbu.startPrint = cb;

    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    OI_BIP_InitAppParams(&client->param, client->paramData, MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUUID128(&client->param, OI_BIP_OBEX_APP_PARAM_SERVICE_ID, &params->request.secondaryServiceId);
    OI_BIP_FinishAppParams(&client->param);

    return BIP_PutRequest(connection,
                          BIP_CMD_START_PRINT,
                          cbu,
                          params,
                          &OI_BIP_PrintType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          NULL /* handle */,
                          &client->param,
                          params->request.data,
                          params->request.final);
}

/*
 * GetPartialImage
 * OBEX GET
 * Type: x-bt/img-partial
 * Name:   <image file name>
 * Params:
 *     PartialFileLength<UINT32_BE>
 *     PartialFileStartOffset<UINT32_BE>
 */
OI_STATUS OI_BIPCLI_GetPartialImage(OI_BIPCLI_HANDLE connection,
                                    OI_BIPCLI_GET_PARTIAL_IMAGE_CFM cb,
                                    OI_BIP_GET_PARTIAL_IMAGE_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;
    cbu.getPartialImage = cb;
    
    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    OI_BIP_InitAppParams(&client->param,
                         client->paramData,
                         MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUINT32(&client->param,
                                OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_LENGTH,
                                params->request.partialLength);
    OI_BIP_AppendAppParamUINT32(&client->param,
                                OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_START_OFFSET,
                                params->request.partialOffset);
    OI_BIP_FinishAppParams(&client->param);

    return BIP_GetRequest(connection,
                          BIP_CMD_GET_PARTIAL_IMAGE,
                          cbu,
                          params,
                          &OI_BIP_PartialType,
                          params->request.imageName, /* name */
                          NULL, /* descriptor */
                          NULL, /* handle */
                          &client->param);
}

/*
 * StartArchve
 * OBEX PUT
 * TYpe: x-bt/img-archive
 * Params:
 *     ServiceID<UUID128>
 */
OI_STATUS OI_BIPCLI_StartArchive(OI_BIPCLI_HANDLE connection,
                                 OI_BIPCLI_START_ARCHIVE_CFM cb,
                                 OI_BIP_START_ARCHIVE_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;
    cbu.startArchive = cb;
    
    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    OI_BIP_InitAppParams(&client->param, client->paramData, MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUUID128(&client->param, OI_BIP_OBEX_APP_PARAM_SERVICE_ID, &params->request.secondaryServiceId);
    OI_BIP_FinishAppParams(&client->param);

    return BIP_PutRequest(connection,
                          BIP_CMD_START_ARCHIVE,
                          cbu,
                          params,
                          &OI_BIP_ArchiveType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          NULL /* handle */,
                          &client->param,
                          NULL /* data */,
                          TRUE);
}

/*
 * GetStatus
 * OBEX GET
 * Type: x-bt/img-status
 */
OI_STATUS OI_BIPCLI_GetStatus(OI_BIPCLI_HANDLE connection,
                              OI_BIPCLI_GET_STATUS_CFM cb)
{
    BIP_CALLBACK_UNION cbu;
    cbu.getStatus = cb;
    
    return BIP_GetRequest(connection,
                          BIP_CMD_GET_STATUS,
                          cbu,
                          NULL,
                          &OI_BIP_StatusType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          NULL /* handle */,
                          NULL /* OBEX App Params */);
}

/*
 * GetMonitoringImage
 * OBEX GET
 * Type: x-bt/img-monitoring
 * Params:
 *     StoreFlag<UINT8>
 */
OI_STATUS OI_BIPCLI_GetMonitoringImage(OI_BIPCLI_HANDLE connection,
                                       OI_BIPCLI_GET_MONITORING_IMAGE_CFM cb,
                                       OI_BIP_GET_MONITORING_IMAGE_PARAMS *params)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    BIP_CALLBACK_UNION cbu;
    cbu.getMonitoringImage = cb;
    
    if (!client) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    OI_BIP_InitAppParams(&client->param, client->paramData, MAX_PARAM_DATA_BYTES);
    OI_BIP_AppendAppParamUINT8(&client->param,
                               OI_BIP_OBEX_APP_PARAM_STORE_FLAG,
                               (OI_UINT8)(params->request.storeImage ? 1 : 0));
    OI_BIP_FinishAppParams(&client->param);

    return BIP_GetRequest(connection,
                          BIP_CMD_GET_MONITORING_IMAGE,
                          cbu,
                          params,
                          &OI_BIP_MonitoringType,
                          NULL /* name */,
                          NULL /* descriptor */,
                          NULL /* handle */,
                          &client->param);
}



static void AbortCfm(OI_OBEXCLI_CONNECTION_HANDLE connection)
{
    BIP_CLIENT_DATA *client = BipClient((OI_BIPCLI_HANDLE)connection);

#ifndef OI_CODE
    if (!client) {
        BT_ERR("AbortCfm: client ptr is NULL!", 0, 0, 0);
        return;
    }
#endif

    if (client->cancelCfm) {
        client->cancelCfm((OI_BIPCLI_HANDLE)connection);
    }
    if (client->state == BIPCLI_STATE_ABORTING) {
        client->state = BIPCLI_STATE_CONNECTED;
    }
}


OI_STATUS OI_BIPCLI_Cancel(OI_BIPCLI_HANDLE connection,
                           OI_BIPCLI_CANCEL_CFM cancelCfm)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    OI_STATUS status;

    if (!client || (client->state < BIPCLI_STATE_CONNECTED)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
#ifndef OI_CODE
    /* If the client is in CONNECTED state this means that none of the
       operation is in progress, so ignore an Abort in this state */
    if(client->state <= BIPCLI_STATE_CONNECTED)
        return OI_OBEX_INVALID_OPERATION;
#endif

    client->cancelCfm = cancelCfm;
    status = OI_OBEXCLI_Abort((OI_OBEXCLI_CONNECTION_HANDLE)connection, AbortCfm);
    if (OI_SUCCESS(status)) {
        client->state = BIPCLI_STATE_ABORTING;
    }
    return status;
}


OI_STATUS OI_BIPCLI_SetContext(OI_BIPCLI_HANDLE connection,
                               void *context)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    if (client == NULL) {
        return OI_STATUS_NOT_FOUND;
    }
    client->context = context;
    return OI_OK;
}


void* OI_BIPCLI_GetContext(OI_BIPCLI_HANDLE connection)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    if (client == NULL) {
        return NULL;
    } else {
        return client->context;
    }
}

#ifdef OI_DEPRECATED
/*
 * Use OI_BIPLCI_Cancel instead.
 */
OI_STATUS OI_BIPCLI_Abort(OI_BIPCLI_HANDLE connection)
{
    BIP_CLIENT_DATA *client = BipClient(connection);
    if (!client || (client->state < BIPCLI_STATE_CONNECTED)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }
    return OI_OBEXCLI_AbortOperation(connection);
}
#endif
