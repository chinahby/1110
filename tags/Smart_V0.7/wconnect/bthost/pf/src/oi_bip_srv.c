
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_srv.c#1 $ 

$Log: $
*
*   #21         13 Feb  2008          RS
* Handle the situation when the remote sends type header with
* no string.
*
*   #20         27 Nov  2007          RS
* Advanced image printing code path to send the end_of_flag set 
* in the last chunk and not send length hdr if the length is zero.
*
*   #19         13 Jul  2007          GS
* Initialize server->server_type during registration.
*
*   #18         13 Apr  2007          GS
* Added code to validate server->params in DispatchCallback()
*
*   #17         28 Feb  2007          MH
* Modified disconnect indication to support optional headers
*
*    #16       10 Nov 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #15       03 Nov 2006            GS
* Save connection id info during OBEX auth so that disconnect indication 
* will be generated if auth fails after max tries.
*
*    #14       26 Oct 2006            MH
* Modified obex connect indication to support all headers
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
*    #9        18 Jul 2005            GS
* Added support for BIP server force disconnect.
*
*    #8        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #7        19 Oct 2004            JH
* Modified OI_BIPSRV_GetImagesListResponse to add the number of returned handles 
*  parameter in the application parameter field
*
*    #6        31 Aug 2004            JH
* Removed #ifdef OI_CODE around the bloack of code containing call to 
*  OI_BIPSRV_CreateObjectsServerSDPRecord so that server->rfcommChannel is
*  properly updated.
*
*    #5        23 Jul 2004            JH
* Moved to version 3.0.12 revision 1
*
*    #4        06 Jun 2004            JH
* Bug fixes
*
*    #2        15 Apr 2004            JH
* Modified sender register function to use a dummy connection policy to
*  pass in service class UUID down to sio_open
* Modified sender register API to return server channel number and make
*  RUI server registration input optional

===========================================================================*/
/**
  @file
  @internal

  Basic Imaging Profile server
  */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_BIP_SRV

#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_bip_private.h"
#include "oi_bip_srv.h"
#include "oi_bip_sdp.h"
#include "oi_debug.h"
#include "oi_memmgr.h"
#include "oi_securitydb.h"
#include "oi_obexsrv.h"
#include "oi_obexcommon.h"
#include "oi_bt_assigned_nos.h"
#include "oi_securitydb.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

/** An array of these structs encapsulates the information necessary to
 * demultiplex a command request and unmarshall the associated parameters. */
typedef struct {
    OI_OBEX_BYTESEQ const *type;    /**< The OBEX Type header associated with a request (such as "x-bt/img-properties") */
    BIP_CMD command;                /**< The constant used by this profile to refer to a command */
    OI_UINT16 params;         /**< A bitmask describing which fields are necessary in the first packet of a request */
} BIP_TYPEMAP;

static const BIP_TYPEMAP GetCommandMap[] = {
    { &OI_BIP_CapabilitiesType, BIP_CMD_GET_CAPABILITIES, (BIP_PARAM_NONE) },
    { &OI_BIP_ImageListType, BIP_CMD_GET_IMAGES_LIST, (BIP_PARAM_NB_RETURNED_HANDLES |
                                                       BIP_PARAM_LIST_START_OFFSET |
                                                       BIP_PARAM_LATEST_CAPTURED_IMAGES |
                                                       BIP_PARAM_DESCRIPTOR) },
    { &OI_BIP_ImageType, BIP_CMD_GET_IMAGE, (BIP_PARAM_HANDLE | BIP_PARAM_DESCRIPTOR) },
    { &OI_BIP_ThumbnailType, BIP_CMD_GET_LINKED_THUMBNAIL, (BIP_PARAM_HANDLE) },
    { &OI_BIP_AttachmentType, BIP_CMD_GET_LINKED_ATTACHMENT, (BIP_PARAM_HANDLE | BIP_PARAM_NAME) },
    { &OI_BIP_PartialType, BIP_CMD_GET_PARTIAL_IMAGE, (BIP_PARAM_NAME |
                                                       BIP_PARAM_PARTIAL_FILE_LENGTH |
                                                       BIP_PARAM_PARTIAL_FILE_START_OFFSET) },
    { &OI_BIP_MonitoringType, BIP_CMD_GET_MONITORING_IMAGE, (BIP_PARAM_STORE_FLAG) },
    { &OI_BIP_StatusType, BIP_CMD_GET_STATUS, (BIP_PARAM_NONE) },
    { &OI_BIP_PropertiesType, BIP_CMD_GET_IMAGE_PROPERTIES, (BIP_PARAM_HANDLE) }
};

static const BIP_TYPEMAP PutCommandMap[] = {
    /* PutImage and DeleteImage share a typemap entry */
    { &OI_BIP_ImageType, BIP_CMD_PUT_OR_DELETE_IMAGE },

    { &OI_BIP_ImageType, BIP_CMD_PUT_IMAGE },
    { &OI_BIP_ImageType, BIP_CMD_DELETE_IMAGE},

    { &OI_BIP_ThumbnailType, BIP_CMD_PUT_LINKED_THUMBNAIL },
    { &OI_BIP_AttachmentType, BIP_CMD_PUT_LINKED_ATTACHMENT },
    { &OI_BIP_DisplayType, BIP_CMD_REMOTE_DISPLAY },
    { &OI_BIP_PrintType, BIP_CMD_START_PRINT },
    { &OI_BIP_ArchiveType, BIP_CMD_START_ARCHIVE }
};

/* Important: These offsets must match the offsets of the command map above */
#define PUT_COMMAND_MAP_PUT_IMAGE_OFFSET 1
#define PUT_COMMAND_MAP_DELETE_IMAGE_OFFSET 2


/*************************************************************************
 *
 *  Server bookkeeping and lookup
 *
 */


typedef enum {
    BIPSRV_STATE_UNUSED,
    BIPSRV_STATE_REGISTERED,
    BIPSRV_STATE_CONNECTED,
    BIPSRV_STATE_REQUEST
} BIPSRV_STATE;

/** This type is never instantiated. Rather, it exists to document the types and
 * sizes of the application parameters used by the BIP response functions.
 * Compiler-added padding may cause the size of this data structure to be
 * slightly overestimated. This should be minimal.
 *
 * This union mirrors the scheme used in oi_bip_cli.c. It's not as necessary
 * here, but the BIP spec anticipates latter augmentations of the standard. This
 * way it will be easier to correctly add additional responses with different
 * application parameters. */

union _OI_BIP_SRV_APP_PARAM_ELEMENTS {
    struct {
        OI_UINT32 totalFileSize;
        OI_UINT8 endFlag;
    } partialImageParams;
};

#define MAX_PARAM_ELEMENTS 2    /* No more than 2 application parameters are used */

#define MAX_PARAM_DATA_BYTES ((MAX_PARAM_ELEMENTS * OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN) + sizeof(union _OI_BIP_SRV_APP_PARAM_ELEMENTS))

#define MAX_RESPONSE_OBEX_HEADERS 5 /* The maximum number of OBEX headers possibly added by either BIP_GetResponse or BIP_PutResponse */

typedef enum { SERVER_IMAGE_RESPONDER, SERVER_REFERENCED_OBJECTS, SERVER_ARCHIVED_OBJECTS } SERVER_TYPE;

typedef struct {
    void *context;
    BIPSRV_SERVER_TYPE type;
    BIPSRV_STATE state;
    OI_BIP_REQUEST_STATE requestState;
    OI_UINT8 rfcommChannel;
    OI_OBEXSRV_CONNECTION_HANDLE connectionId;

    SERVER_TYPE server_type;
    union {
    OI_BIPSRV_IMAGING_RESPONDER_CALLBACKS imgResp;
    OI_BIPSRV_REFERENCED_OBJECTS_CALLBACKS refObj;
    OI_BIPSRV_ARCHIVED_OBJECTS_CALLBACKS archObj;
    } cb;

    OI_BIP_APP_PARAM_BOOKKEEPING appParam;
    OI_BYTE appParamData[MAX_PARAM_DATA_BYTES];
    OI_BIP_GENERIC_PARAMS params;
    BIP_TYPEMAP const *currentCommand;
    BIP_REQUEST_HEADERS reqHdrs;
    OI_OBEX_HEADER obexHeader[MAX_RESPONSE_OBEX_HEADERS];
    OI_OBEX_HEADER_LIST obexHeaderList;

    OI_BIP_SDP_INFO *sdpInfo;
} BIP_SERVER_DATA;

typedef struct {
    OI_CONNECT_POLICY  connectPolicy;
    BIP_SERVER_DATA *servers;
    OI_UINT serverCount;
    OI_UINT serversUsed;
} OI_BIPSRV_DATA;

static OI_BIPSRV_DATA *BIPSRV_This;

/**
 * Helper macro to get the Bip Server pointer from the OBEX connection handle
*/
#define BipServer(c) ((BIP_SERVER_DATA*)OI_OBEXSRV_GetConnectionContext((OI_OBEXSRV_CONNECTION_HANDLE)(c)))


/*************************************************************************
 *  Forward references
 */
static OI_STATUS internalTerminate(void);


/*************************************************************************/

/* Looks for an unused server instance record */
static BIP_SERVER_DATA *AllocateServer(void)
{
    OI_UINT i;

    if( OI_INIT_FLAG_VALUE(BIP_SRV) ){
        for (i = 0; i < BIPSRV_This->serverCount; i++) {
            if (BIPSRV_This->servers[i].state == BIPSRV_STATE_UNUSED) {
                BIPSRV_This->serversUsed++;
                return &BIPSRV_This->servers[i];
            }
        }
    }

    return NULL;
}

/* Removes a server instance record */
static void DeallocateServer(BIP_SERVER_DATA *server)
{
    OI_ASSERT(OI_INIT_FLAG_VALUE(BIP_SRV));
    
    OI_MemZero(server, sizeof(*server));
    server->state = BIPSRV_STATE_UNUSED;
    BIPSRV_This->serversUsed--;

    if (BIPSRV_This->serversUsed == 0) {
        internalTerminate();
    }
}

static BIP_SERVER_DATA *LookupServerByRfcommChannel(OI_UINT8 rfcommChannel)
{
    OI_UINT i;

    if( OI_INIT_FLAG_VALUE(BIP_SRV) ){
        for (i = 0; i < BIPSRV_This->serverCount; i++) {
            if (BIPSRV_This->servers[i].rfcommChannel == rfcommChannel) {
                return &BIPSRV_This->servers[i];
            }
        }
    }

    return NULL;
}

static BIP_SERVER_DATA *LookupServerByConnectionId(OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    OI_UINT i;

    if( OI_INIT_FLAG_VALUE(BIP_SRV) ){
        for (i = 0; i < BIPSRV_This->serverCount; i++) {
            if (BIPSRV_This->servers[i].connectionId == connectionId) {
                return &BIPSRV_This->servers[i];
            }
        }
    }

    return NULL;
}

/*************************************************************************
 *
 *  Request identification, unmarshalling, and dispatching.
 *
 */

#define OBEX_BYTESEQ_EQUAL(s1, s2) (((s1).len == (s2).len) && (OI_MemCmp((s1).data, (s2).data, (s1).len) == 0))

static OI_BOOL IsImplemented(const BIP_SERVER_DATA *server, BIP_CMD command)
{
    switch (server->server_type) {
    case SERVER_IMAGE_RESPONDER:
    switch (command) {
        case BIP_CMD_GET_CAPABILITIES:              return (server->cb.imgResp.getCapabilities != NULL);
        case BIP_CMD_PUT_IMAGE:                     return (server->cb.imgResp.putImage != NULL);
        case BIP_CMD_PUT_LINKED_ATTACHMENT:         return (server->cb.imgResp.putLinkedAttachment != NULL);
        case BIP_CMD_PUT_LINKED_THUMBNAIL:          return (server->cb.imgResp.putLinkedThumbnail != NULL);
        case BIP_CMD_REMOTE_DISPLAY:                return (server->cb.imgResp.remoteDisplay != NULL);
        case BIP_CMD_GET_IMAGES_LIST:               return (server->cb.imgResp.getImagesList != NULL);
        case BIP_CMD_GET_IMAGE_PROPERTIES:          return (server->cb.imgResp.getImageProperties != NULL);
        case BIP_CMD_GET_IMAGE:                     return (server->cb.imgResp.getImage != NULL);
        case BIP_CMD_GET_LINKED_THUMBNAIL:          return (server->cb.imgResp.getLinkedThumbnail != NULL);
        case BIP_CMD_GET_LINKED_ATTACHMENT:         return (server->cb.imgResp.getLinkedAttachment != NULL);
        case BIP_CMD_DELETE_IMAGE:                  return (server->cb.imgResp.deleteImage != NULL);
        case BIP_CMD_START_PRINT:                   return (server->cb.imgResp.startPrint != NULL);
        case BIP_CMD_START_ARCHIVE:                 return (server->cb.imgResp.startArchive != NULL);
        case BIP_CMD_GET_MONITORING_IMAGE:          return (server->cb.imgResp.getMonitoringImage != NULL);
        case BIP_CMD_GET_STATUS:                    return (server->cb.imgResp.getStatus != NULL);
                default:                                    break;
        }

    case SERVER_REFERENCED_OBJECTS:
        switch (command) {
        case BIP_CMD_GET_CAPABILITIES:              return (server->cb.refObj.getCapabilities != NULL);
        case BIP_CMD_GET_PARTIAL_IMAGE:             return (server->cb.refObj.getPartialImage != NULL);
                default:                                    break;
        }

    case SERVER_ARCHIVED_OBJECTS:
    switch (command) {
        case BIP_CMD_GET_CAPABILITIES:              return (server->cb.archObj.getCapabilities != NULL);
        case BIP_CMD_GET_IMAGES_LIST:               return (server->cb.archObj.getImagesList != NULL);
        case BIP_CMD_GET_IMAGE_PROPERTIES:          return (server->cb.archObj.getImageProperties != NULL);
        case BIP_CMD_GET_IMAGE:                     return (server->cb.archObj.getImage != NULL);
        case BIP_CMD_GET_LINKED_THUMBNAIL:          return (server->cb.archObj.getLinkedThumbnail != NULL);
        case BIP_CMD_GET_LINKED_ATTACHMENT:         return (server->cb.archObj.getLinkedAttachment != NULL);
        case BIP_CMD_DELETE_IMAGE:                  return (server->cb.archObj.deleteImage != NULL);
                default:                                    break;
            }
        default:
        break;
    }
            return FALSE;
    }

#define ASSIGN_PARAM(function, param, header) \
    ((OI_BIP_##function##_PARAMS *)server->params)->request.param = server->reqHdrs.header
#define CLEAR_REQUEST_PARAMS(function) \
    OI_MemZero(&((OI_BIP_##function##_PARAMS *)server->params)->request, sizeof(OI_BIP_##function##_PARAMS))

/* Given a command, this function maps the unmarshalled OBEX headers to the
 * command-specific parameter structure, invokes the callback, and clears the
 * response parameters (since they aren't valid after the server's get/put
 * indication callback returns.)
 */

static OI_STATUS DispatchCallback(const BIP_SERVER_DATA *server,
                                  OI_STATUS status)
{
    OI_STATUS result = OI_STATUS_INVALID_STATE;

    OI_ASSERT(server != NULL);
    OI_ASSERT(server->state == BIPSRV_STATE_REQUEST);

    OI_DBGPRINT2(("DispatchCallback(%!)", status));

#ifndef OI_CODE
    /* 
    RequestIndication shall allocate server->params memory, First time or coninue state
    If server->params is NULL means some state is wrong. Validate server->params before 
    using it.
    */
    if (server->params == NULL) {
      BT_ERR ("OI BIP SRV: server->params is NULL!", 0, 0, 0);
      return OI_OBEX_INTERNAL_SERVER_ERROR;
    }
#endif

    switch(server->currentCommand->command) {
        case BIP_CMD_GET_CAPABILITIES:
            OI_DBGPRINT2(("BIP_CMD_GET_CAPABILITIES"));
            /* no request parameters */
            switch(server->server_type) {
            case SERVER_IMAGE_RESPONDER:
                OI_ASSERT(server->cb.imgResp.getCapabilities != NULL);
                result = server->cb.imgResp.getCapabilities(server->connectionId, server->params, server->requestState, status);
                break;

            case SERVER_REFERENCED_OBJECTS:
                OI_ASSERT(server->cb.refObj.getCapabilities != NULL);
                result = server->cb.refObj.getCapabilities(server->connectionId, server->params, server->requestState, status);
                break;

            case SERVER_ARCHIVED_OBJECTS:
                OI_ASSERT(server->cb.archObj.getCapabilities != NULL);
                result = server->cb.archObj.getCapabilities(server->connectionId, server->params, server->requestState, status);
                break;
            }



            break;

        case BIP_CMD_GET_IMAGES_LIST:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGES_LIST"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_IMAGES_LIST, handleCount, nbReturnedHandles);
                    ASSIGN_PARAM(GET_IMAGES_LIST, handleOffset, listStartOffset);
                    ASSIGN_PARAM(GET_IMAGES_LIST, latest, latestCapturedImages);
                    ASSIGN_PARAM(GET_IMAGES_LIST, handlesDescriptor, descriptor);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getImagesList != NULL);
                        result = server->cb.imgResp.getImagesList(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_IMAGES_LIST command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.getImagesList != NULL);
                        result = server->cb.archObj.getImagesList(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_IMAGES_LIST);
            break;

        case BIP_CMD_GET_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_IMAGE, imageHandle, handle);
                    ASSIGN_PARAM(GET_IMAGE, imageDescriptor, descriptor);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getImage != NULL);
                        result = server->cb.imgResp.getImage(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.getImage != NULL);
                        result = server->cb.archObj.getImage(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_IMAGE);
            break;

        case BIP_CMD_GET_LINKED_THUMBNAIL:
            OI_DBGPRINT2(("BIP_CMD_GET_LINKED_THUMBNAIL"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_LINKED_THUMBNAIL, imageHandle, handle);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getLinkedThumbnail != NULL);
                        result = server->cb.imgResp.getLinkedThumbnail(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_LINKED_THUMBNAIL command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.getLinkedThumbnail != NULL);
                        result = server->cb.archObj.getLinkedThumbnail(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_LINKED_THUMBNAIL);
            break;

        case BIP_CMD_GET_LINKED_ATTACHMENT:
            OI_DBGPRINT2(("BIP_CMD_GET_LINKED_ATTACHMENT"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_LINKED_ATTACHMENT, imageHandle, handle);
                    ASSIGN_PARAM(GET_LINKED_ATTACHMENT, attachmentName, name);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getLinkedAttachment != NULL);
                        result = server->cb.imgResp.getLinkedAttachment(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support LINKED_ATTACHMENT command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.getLinkedAttachment);
                        result = server->cb.archObj.getLinkedAttachment(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_LINKED_ATTACHMENT);
            break;

        case BIP_CMD_GET_PARTIAL_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_PARTIAL_IMAGE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_PARTIAL_IMAGE, imageName, name);
                    ASSIGN_PARAM(GET_PARTIAL_IMAGE, partialLength, partialFileLength);
                    ASSIGN_PARAM(GET_PARTIAL_IMAGE, partialOffset, partialFileStartOffset);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_LOG_ERROR(("IMAGE_REPONDER server type does not support GET_PARTIAL_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_ASSERT(server->cb.refObj.getPartialImage);
                        result = server->cb.refObj.getPartialImage(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support GET_PARTIAL_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_PARTIAL_IMAGE);
            break;

        case BIP_CMD_GET_MONITORING_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_GET_MONITORING_IMAGE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_MONITORING_IMAGE, storeImage, storeFlag);
                    /* fall through */
                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getMonitoringImage);
                        result = server->cb.imgResp.getMonitoringImage(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_MONITORING_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support GET_MONITORING_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_MONITORING_IMAGE);
            break;

        case BIP_CMD_GET_STATUS:
            OI_DBGPRINT2(("BIP_CMD_GET_STATUS"));
            /* no parameters */
            switch(server->server_type) {
            case SERVER_IMAGE_RESPONDER:
                OI_ASSERT(server->cb.imgResp.getStatus);
                result = server->cb.imgResp.getStatus(server->connectionId, status);
                break;
                
            case SERVER_REFERENCED_OBJECTS:
                OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_STATUS command.\n"));
                result = OI_OBEX_NOT_IMPLEMENTED;
                break;
                
            case SERVER_ARCHIVED_OBJECTS:
                OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support GET_STATUS command.\n"));
                result = OI_OBEX_NOT_IMPLEMENTED;
                break;
            }
            break;

        case BIP_CMD_GET_IMAGE_PROPERTIES:
            OI_DBGPRINT2(("BIP_CMD_GET_IMAGE_PROPERTIES"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(GET_IMAGE_PROPERTIES, imageHandle, handle);
                    /* fall through */
                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.getImageProperties);
                        result = server->cb.imgResp.getImageProperties(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support GET_IMAGE_PROPERTIES command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.getImageProperties);
                        result = server->cb.archObj.getImageProperties(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(GET_IMAGE_PROPERTIES);
            break;

        case BIP_CMD_PUT_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_PUT_IMAGE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PUT_IMAGE, imageName, name);
                    ASSIGN_PARAM(PUT_IMAGE, imageDescriptor, descriptor);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                    ASSIGN_PARAM(PUT_IMAGE, data, body);
                    ASSIGN_PARAM(PUT_IMAGE, final, final);
                    /* fall through */

                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.putImage);
                        result = server->cb.imgResp.putImage(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support PUT_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support PUT_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(PUT_IMAGE);
            break;

        case BIP_CMD_DELETE_IMAGE:
            OI_DBGPRINT2(("BIP_CMD_DELETE_IMAGE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(DELETE_IMAGE, imageHandle, handle);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.deleteImage);
                        result = server->cb.imgResp.deleteImage(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support DELETE_IMAGE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_ASSERT(server->cb.archObj.deleteImage);
                        result = server->cb.archObj.deleteImage(server->connectionId, server->params, server->requestState, status);
                        break;
                    }
            }
            break;

        case BIP_CMD_PUT_LINKED_THUMBNAIL:
            OI_DBGPRINT2(("BIP_CMD_PUT_LINKED_THUMBNAIL"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PUT_LINKED_THUMBNAIL, imageHandle, handle);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                    ASSIGN_PARAM(PUT_LINKED_THUMBNAIL, data, body);
                    ASSIGN_PARAM(PUT_LINKED_THUMBNAIL, final, final);
                    /* fall through */

                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.putLinkedThumbnail);
                        result = server->cb.imgResp.putLinkedThumbnail(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support PUT_LINKED_THUMBNAIL command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support PUT_LINKED_THUMBNAIL command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(PUT_LINKED_THUMBNAIL);
            break;

        case BIP_CMD_PUT_LINKED_ATTACHMENT:
            OI_DBGPRINT2(("BIP_CMD_PUT_LINKED_ATTACHMENT"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PUT_LINKED_ATTACHMENT, imageHandle, handle);
                    ASSIGN_PARAM(PUT_LINKED_ATTACHMENT, attachmentDescriptor, descriptor);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                    ASSIGN_PARAM(PUT_LINKED_ATTACHMENT, data, body);
                    ASSIGN_PARAM(PUT_LINKED_ATTACHMENT, final, final);
                    /* fall through */

                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.putLinkedAttachment);
                        result = server->cb.imgResp.putLinkedAttachment(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support PUT_LINKED_ATTACHEMENT command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support PUT_LINKED_ATTACHEMENT command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(PUT_LINKED_ATTACHMENT);
            break;


        case BIP_CMD_REMOTE_DISPLAY:
            OI_DBGPRINT2(("BIP_CMD_REMOTE_DISPLAY"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(REMOTE_DISPLAY, imageHandle, handle);
                    ASSIGN_PARAM(REMOTE_DISPLAY, displayCommand, remoteDisplay);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.remoteDisplay);
                        result = server->cb.imgResp.remoteDisplay(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support REMOTE_DISPLAY command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support REMOTE_DISPLAY command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(REMOTE_DISPLAY);
            break;

        case BIP_CMD_START_PRINT:
            OI_DBGPRINT2(("BIP_CMD_START_PRINT"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(START_PRINT, secondaryServiceId, serviceId);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                    ASSIGN_PARAM(START_PRINT, data, body);
                    ASSIGN_PARAM(START_PRINT, final, final);
                    /* fall through */

                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.startPrint);
                        result = server->cb.imgResp.startPrint(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support START_PRINT command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support START_PRINT command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(START_PRINT);
            break;

        case BIP_CMD_START_ARCHIVE:
            OI_DBGPRINT2(("BIP_CMD_START_ARCHIVE"));
            switch (server->requestState) {
                case OI_BIP_REQUEST_INITIAL:
                    ASSIGN_PARAM(START_ARCHIVE, secondaryServiceId, serviceId);
                    /* fall through */

                case OI_BIP_REQUEST_CONTINUE:
                case OI_BIP_REQUEST_CLEANUP:
                    switch(server->server_type) {
                    case SERVER_IMAGE_RESPONDER:
                        OI_ASSERT(server->cb.imgResp.startArchive);
                        result = server->cb.imgResp.startArchive(server->connectionId, server->params, server->requestState, status);
                        break;
                        
                    case SERVER_REFERENCED_OBJECTS:
                        OI_LOG_ERROR(("REFERENCED_OBJECTS server type does not support START_ARCHIVE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                        
                    case SERVER_ARCHIVED_OBJECTS:
                        OI_LOG_ERROR(("ARCHIVED_OBJECTS server type does not support START_ARCHIVE command.\n"));
                        result = OI_OBEX_NOT_IMPLEMENTED;
                        break;
                    }
            }

            CLEAR_REQUEST_PARAMS(START_ARCHIVE);
            break;

        default:
            result = OI_OBEX_NOT_IMPLEMENTED;
    }
    if (result == OI_OBEX_CONTINUE) {
        result = OI_OK;
    }
    return result;
}

static OI_STATUS requestInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *cmdHeaders,
                            OI_STATUS status,
                            BIP_TYPEMAP const *map,
                            OI_UINT mapLen)
{
    BIP_SERVER_DATA *server;
    OI_BOOL haveAllParams;
    OI_BOOL first;
    OI_BOOL isFinalPut;
    OI_BOOL cleanup;
    OI_BOOL error;
    OI_UINT i;

    OI_DBGPRINT(("obexReqInd: %!",status));

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle"));
#endif
        return OI_STATUS_INVALID_HANDLE;
    }

    if (server->state < BIPSRV_STATE_CONNECTED) {
        OI_ASSERT(server->state >= BIPSRV_STATE_CONNECTED);
        return OI_STATUS_INVALID_STATE;
    }

    /* OI_OK and OI_OBEX_CONTINUE indicate success, other codes indicate failure */
    error = !(OI_SUCCESS(status) || (status == OI_OBEX_CONTINUE));

    /* This is to clarify tests based on whether this is the first request in a
     * sequence */
    first = (server->state == BIPSRV_STATE_CONNECTED);

    /* OBEXSRV signals that it's time to clean up any state associated with a
     * request by setting the header list to NULL. This can be because the
     * request completed successfully (including the transmission of response
     * data) or that the request was aborted by the user or otherwise terminated
     */
    cleanup = (cmdHeaders == NULL);

    /* PutImage requires special handling to see if it's really a delete. If
     * this is a put-type request and also a final packet, make a note of that
     * fact to help correctly dispatch.
     */
    isFinalPut = (map == PutCommandMap) && (status != OI_OBEX_CONTINUE);

    /* Assert that  error => cleanup. The OBEXSRV api doesn't send header data
     * along with any error response */
    if (error){
        OI_ASSERT(cleanup);
    }

    OI_DBGPRINT2(("%s%s%s", error ? "error " : "", first ? "first " : "", cleanup ? "cleanup " : ""));


    if (cleanup) {
        OI_DBGPRINT2(("Cleaning up after request"));
        server->requestState = OI_BIP_REQUEST_CLEANUP;
        if (server->currentCommand != NULL) {
            /* temporarily set server state so we can clean up */
            server->state = BIPSRV_STATE_REQUEST;
            DispatchCallback(server, status);
        }
        server->state = BIPSRV_STATE_CONNECTED;
        if (server->params != NULL) {
            DEBUG_ONLY(OI_MemZero(server->params, sizeof(BIP_PARAM_UNION)));
            OI_FreeIf(&server->params);
        }
        return status;
    }

    OI_BIP_UnmarshallHeaderData(&server->reqHdrs, cmdHeaders);

    /* The first request in a transaction must include a type header. If it
     * doesn't, or if the type header doesn't match a known command, an error is
     * returned to the client. Subsequent type headers shouldn't be present, but
     * are ignored if they are. */
    if (first) {
        OI_OBEX_BYTESEQ *type = server->reqHdrs.type;
#ifdef OI_CODE
#error code not present
#else
        if ( (type == NULL) || (type->data == NULL) ) {
#endif
            /* A type header must be present in the first request */
            OI_LOG_ERROR(("Type header or data missing in request"));
            return OI_OBEX_BAD_REQUEST;
        }

        OI_DBGPRINT2(("ReqType: %s", type->data));
        server->currentCommand = NULL;
        for (i = 0; i < mapLen; ++i) {
            if (OBEX_BYTESEQ_EQUAL(*type, *map[i].type)) {
                server->currentCommand = &map[i];
                break;
            }
        }
        if (server->currentCommand == NULL) {
            /* The request wasn't found in the type map. Return an error. */
            OI_LOG_ERROR(("Unknown request type"));
            return OI_OBEX_BAD_REQUEST;
        }

        /* Special handling for PutImage and DeleteImage. If this block is being
         * executed, we know this is the first request of the sequence. If it's
         * also a final PUT, -and- there is no body header (as opposed to a body
         * header with 0 length) then this is a delete command. Otherwise, it's
         * a put. Once we know, update the currentCommand field with the real
         * command map entry. */
        if (server->currentCommand->command == BIP_CMD_PUT_OR_DELETE_IMAGE) {
            OI_DBGPRINT2(("Ambiguous Put/Delete. isFinalPut: %d. Body is %s", isFinalPut,
                server->reqHdrs.body ? "not NULL" : "NULL"));
            if (isFinalPut && server->reqHdrs.body == NULL) {
                OI_DBGPRINT2(("Resolved as Delete"));
                /* This is really a DeleteImage request */
                server->currentCommand = &PutCommandMap[PUT_COMMAND_MAP_DELETE_IMAGE_OFFSET];

                /* Make sure that the correct map entry is being selected */
                OI_ASSERT(server->currentCommand->command == BIP_CMD_DELETE_IMAGE);
            } else {
                OI_DBGPRINT2(("Resolved as Put"));
                /* This is really a PutImage request */
                server->currentCommand = &PutCommandMap[PUT_COMMAND_MAP_PUT_IMAGE_OFFSET];

                /* Make sure that the correct map entry is being selected */
                OI_ASSERT(server->currentCommand->command == BIP_CMD_PUT_IMAGE);
            }
        }

        if (!IsImplemented(server, server->currentCommand->command)) {
            /* The request was recognized but isn't implemented by this server.
             * Return an error. */
            OI_LOG_ERROR(("Unimplemented command requested"));
            return OI_OBEX_UNKNOWN_COMMAND;
        }
        
        /* Ensure all required parameters are present */
        haveAllParams = ((server->currentCommand->params & server->reqHdrs.flags) == server->currentCommand->params);
        if (!haveAllParams) {
            OI_LOG_ERROR(("Incomplete request"));
            OI_DBGPRINT2(("Header mask: %d", server->reqHdrs.flags));
            OI_DBGPRINT2(("Needed mask: %d", server->currentCommand->params));
            return OI_OBEX_PRECONDITION_FAILED;
        }

        OI_ASSERT(server->params == NULL);  /* Catch a leak of this block */
        server->params = OI_Calloc(sizeof(BIP_PARAM_UNION));

        if (server->params == NULL) {
            return OI_STATUS_NO_RESOURCES;
        }


        /* The request is recognized and implemented; we may continue. */
        server->state = BIPSRV_STATE_REQUEST;
        server->requestState = OI_BIP_REQUEST_INITIAL;
    } else {
        OI_ASSERT(server->state == BIPSRV_STATE_REQUEST);
        server->requestState = OI_BIP_REQUEST_CONTINUE;
    }

    status = DispatchCallback(server, status);
    if (!OI_SUCCESS(status)) {
        OI_FreeIf(&server->params);
        server->state = BIPSRV_STATE_CONNECTED;
    }

    return status;
}


/*************************************************************************
 *
 *  OBEX callbacks
 *
 */

static void AuthRequest(BIP_SERVER_DATA *server, OI_OBEXSRV_CONNECTION_HANDLE connectionId);



static OI_STATUS obexGetInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *cmdHeaders,
                            OI_STATUS status)
{
    return requestInd(connectionId, cmdHeaders, status, GetCommandMap, OI_ARRAYSIZE(GetCommandMap));
}

static OI_STATUS obexPutInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                            OI_OBEX_HEADER_LIST *cmdHeaders,
                            OI_STATUS status)
{

    return requestInd(connectionId, cmdHeaders, status, PutCommandMap, OI_ARRAYSIZE(PutCommandMap));
}

#ifdef OI_CODE
#error code not present
#else
static void obexConnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                OI_BOOL unauthorized,
                                OI_BYTE *userId,
                                OI_UINT8 userIdLen,
                                OI_OBEX_HEADER_LIST *cmd_headers_ptr)
#endif
{
    OI_STATUS status;
    BIP_SERVER_DATA *server;
    OI_UINT8 channel;

    /* When the connection comes in, it may be with a connectionId that BIP has never
     * seen before. We need to ask OBEX for the connection's associated rfcomm
     * channel in order to locate the correct server data */

    status = OI_OBEXSRV_GetRfcommChannel(connectionId, &channel);
    if (!OI_SUCCESS(status)) {
        status = OI_OBEX_NOT_CONNECTED;
        goto error;
    }

    server = LookupServerByRfcommChannel(channel);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("No server registered on RFCOMM channel"));
#endif
        status = OI_OBEX_NOT_CONNECTED;
        goto error;
    }

    /* At this point the server which should be associated with this channel has
     * been identified. We need to make sure it is free to accept requests. */

    if (server->state != BIPSRV_STATE_REGISTERED) {
        status = OI_STATUS_NO_RESOURCES;
        goto error;
    }

    /*
     * Associate the "server" pointer with the OBEX client handle
     */
    OI_OBEXSRV_SetConnectionContext(connectionId, (void*)server);

    /* The server isn't busy. If authentication has been requsted, pass the
     * request to the application */

    if (unauthorized) {
        OI_DBGPRINT(("Obex requested credentials"));
        AuthRequest(server, connectionId);
#ifndef OI_CODE
        server->connectionId = connectionId;
#endif
    } else {
    /* Authentication, if any, has been processed. Now ask the application about
     * the request */
        switch(server->server_type) {
        case SERVER_IMAGE_RESPONDER:
            server->cb.imgResp.connectInd(connectionId);
            break;

        case SERVER_REFERENCED_OBJECTS:
            server->cb.refObj.connectInd(connectionId);
            break;

        case SERVER_ARCHIVED_OBJECTS:
            server->cb.archObj.connectInd(connectionId);
            break;
        }
    }
    return;

error:
#ifdef OI_CODE
#error code not present
#else
    OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, status);
#endif
}


OI_STATUS OI_BIPSRV_AcceptConnection(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                     OI_BOOL accept)
{
    OI_STATUS status;
    BIP_SERVER_DATA *server = NULL;
    OI_UINT8 channel;

    status = OI_OBEXSRV_GetRfcommChannel(connectionId, &channel);
        if (OI_SUCCESS(status)) {
        server = LookupServerByRfcommChannel(channel);
        }
    if (!server) {
        OI_LOG_ERROR(("No server registered on RFCOMM channel %d", connectionId));
        return OI_OBEX_NOT_CONNECTED;
    }
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_AcceptConnect(connectionId, accept, NULL, accept? OI_OK : OI_FAIL);
#endif
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("OI_OBEXSRV_AcceptConnect failed %!", status));
    }
    if (accept && OI_SUCCESS(status)) {
        server->state = BIPSRV_STATE_CONNECTED;
        server->connectionId = connectionId;
    }
    return status;
}

#ifdef OI_CODE
#error code not present
#else
static void obexDisconnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *headers)
#endif
{
    BIP_SERVER_DATA *server;
    OI_DBGPRINT(("Obex disconnect indication"));
    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle. Ignoring disconnectInd"));
#endif
        return;
    }

    switch(server->server_type) {
    case SERVER_IMAGE_RESPONDER:
        server->cb.imgResp.disconnectInd(connectionId);
        break;
        
    case SERVER_REFERENCED_OBJECTS:
        server->cb.refObj.disconnectInd(connectionId);
        break;
        
    case SERVER_ARCHIVED_OBJECTS:
        server->cb.archObj.disconnectInd(connectionId);
        break;
    }
    OI_FreeIf(&server->params);
    server->state = BIPSRV_STATE_REGISTERED;
    server->connectionId = 0;
}

static void AuthRequest(BIP_SERVER_DATA *server, OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    OI_DBGPRINT(("Obex credentials requested"));

    switch(server->server_type) {
    case SERVER_IMAGE_RESPONDER:
        if (server->cb.imgResp.authInd) {
            server->cb.imgResp.authInd(connectionId);
        } else {
            OI_DBGPRINT(("BIP server doesn't support authentication. Refusing."));
            OI_OBEXSRV_AuthenticationResponse(connectionId, NULL, FALSE);
        }
        break;
        
    case SERVER_REFERENCED_OBJECTS:
        if (server->cb.refObj.authInd) {
            server->cb.refObj.authInd(connectionId);
        } else {
              OI_DBGPRINT(("BIP server doesn't support authentication. Refusing."));
              OI_OBEXSRV_AuthenticationResponse(connectionId, NULL, FALSE);
        }
        break;
        
    case SERVER_ARCHIVED_OBJECTS:
        if (server->cb.archObj.authInd) {
            server->cb.archObj.authInd(connectionId);
        } else {
            OI_DBGPRINT(("BIP server doesn't support authentication. Refusing."));
            OI_OBEXSRV_AuthenticationResponse(connectionId, NULL, FALSE);
        }
        break;
    }
}

/*************************************************************************
 *
 *  Initialization, authentication, and server registration
 *
 */

static void initData(void)
{
    OI_CONFIG_BIP_SRV *pConfig;
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_Imaging,            /* OI_UINT32           serviceUuid32 ; */
        OI_SEC_REQUIRE_NOTHING,     /* securityPolicy is a place holder, get real policy at runtime */
        FALSE,                      /* OI_BOOL             mustBeMaster ; */
        NULL,                       /* OI_L2CAP_FLOWSPEC   *flowspec; */
        0                           /* OI_UINT8            powerSavingDisables ; */
    } ;

    /* get configuration */
    pConfig = OI_CONFIG_TABLE_GET(BIP_SRV) ;

    BIPSRV_This->serverCount = pConfig->maxServers;

    /** get security policy. */
    BIPSRV_This->connectPolicy = connectPolicy;
    BIPSRV_This->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(BIPSRV_This->connectPolicy.serviceUuid32);
}

static OI_STATUS internalInit(void)
{
    OI_CONFIG_BIP_SRV *pConfig;

    /* make sure initialization only happens once - currently, the init flag is boolean */
    if (OI_INIT_FLAG_VALUE(BIP_SRV)) {
        return(OI_STATUS_ALREADY_INITIALIZED) ;
    }

    /* get configuration */
    pConfig = OI_CONFIG_TABLE_GET(BIP_SRV) ;

    BIPSRV_This = OI_Calloc(sizeof(*BIPSRV_This));
    if (NULL == BIPSRV_This) {
        return (OI_STATUS_OUT_OF_MEMORY);
    }

    BIPSRV_This->servers = OI_Calloc(pConfig->maxServers * sizeof(BIP_SERVER_DATA));
    if (BIPSRV_This->servers == NULL) {
        OI_FreeIf(&BIPSRV_This);
        return OI_STATUS_OUT_OF_MEMORY;
    }

    initData();

    /* set out initialization flag TRUE - we're initialized */
    OI_INIT_FLAG_PUT_FLAG(TRUE, BIP_SRV) ;

    return OI_OK;
}

static OI_STATUS internalTerminate(void)
{
    /* can only terminate if we're already initialized */
    if (!OI_INIT_FLAG_VALUE(BIP_SRV)) {
        return(OI_STATUS_NOT_INITIALIZED) ;
    }

    /* release our memory */
    OI_FreeIf(&BIPSRV_This->servers);
    OI_FreeIf(&BIPSRV_This) ;

    /* set initialization flag to show that we're not initialized */
    OI_INIT_FLAG_PUT_FLAG(FALSE, BIP_SRV) ;

    return(OI_OK) ;
}


void OI_BIPSRV_AuthenticationResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                      OI_CHAR *pin)
{
    OI_OBEXSRV_AuthenticationResponse(connectionId, pin, FALSE);
}
#ifndef OI_CODE
static OI_OBEXSRV_CB_LIST obexCallbacks = {
    obexConnectInd,
    obexDisconnectInd,
    obexGetInd,
    obexPutInd,
    NULL,
    NULL   /* Set Path operation not supported */
};
#else
#error code not present
#endif /* OI_CODE */

OI_STATUS OI_BIPSRV_RegisterImagingResponder(const OI_BIPSRV_IMAGING_RESPONDER_CALLBACKS *cb,
                                             OI_BIPSRV_IMAGING_RESPONDER_INFO *info,
                                             const OI_SDP_STRINGS *strings,
                                             OI_BOOL authentication,
                                             OI_UINT8 *channel)
{
    BIP_SERVER_DATA *server;
    OI_STATUS status;
    OI_OBEXSRV_AUTHENTICATION obexAuth;
    OI_UINT i;

    /* The following table maps the supported feature bits of the sdp
     * information to the corresponding obex target */
    static const struct {
        OI_UINT16 featureMask;
        OI_OBEX_BYTESEQ const *target;
    } featureTargetMap[] = {
        { OI_BIP_SDP_FEATURE_IMAGE_PUSH | OI_BIP_SDP_FEATURE_IMAGE_PUSH_STORE | OI_BIP_SDP_FEATURE_IMAGE_PUSH_PRINT | OI_BIP_SDP_FEATURE_IMAGE_PUSH_DISPLAY,
            &OI_BIP_ImagePushObexTargetUUID },
            { OI_BIP_SDP_FEATURE_IMAGE_PULL, &OI_BIP_ImagePullObexTargetUUID },
            { OI_BIP_SDP_FEATURE_ADVANCED_IMAGE_PRINTING, &OI_BIP_AdvancedImagePrintingObexTargetUUID },
            { OI_BIP_SDP_FEATURE_AUTOMATIC_ARCHIVE, &OI_BIP_AutomaticArchiveObexTargetUUID },
            { OI_BIP_SDP_FEATURE_REMOTE_CAMERA, &OI_BIP_RemoteCameraObexTargetUUID },
            { OI_BIP_SDP_FEATURE_REMOTE_DISPLAY, &OI_BIP_RemoteDisplayObexTargetUUID }
    };
#ifndef OI_CODE
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_ImagingResponder,   // service class UUID
        OI_SEC_REQUIRE_NOTHING,     // don't care
        FALSE,                      // don't care
        NULL,                       // don't care    
        0                           // don't care    
    } ;
#endif

    OI_ENSURE_INIT(BIP_SRV);

    ARGCHECK(cb != NULL);
    ARGCHECK(info != NULL);

    if (info->supportedFeatures == 0) {
        OI_LOG_ERROR(("OI_BIPSRV_RegisterImagingResponder wasn't called with any supported features"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    server = AllocateServer();
    if (server == NULL) {
        OI_LOG_ERROR(("Couldn't register the imaging responder server"));
        return OI_STATUS_NO_RESOURCES;
    }

    OI_ASSERT(server->state == BIPSRV_STATE_UNUSED);

    server->cb.imgResp = *cb;

    obexAuth = authentication ? OI_OBEXSRV_AUTH_PASSWORD : OI_OBEXSRV_AUTH_NONE;

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_RegisterServer(NULL /* target */,
                                       &obexCallbacks,
                                       obexAuth,
                                       channel,
                                       &connectPolicy);
#endif /* OI_CODE */

    if (OI_SUCCESS(status)) {
        server->rfcommChannel = *channel;
        for (i = 0; i < OI_ARRAYSIZE(featureTargetMap); i++) {
            if (info->supportedFeatures & featureTargetMap[i].featureMask) {
                status = OI_OBEXSRV_RegisterSecondaryTarget(featureTargetMap[i].target,
                                                            &obexCallbacks,
                                                            obexAuth,
                                                            *channel);
                if (!OI_SUCCESS(status)) {
                    break;
                }
            }
        }

        if (OI_SUCCESS(status)) {
        status = OI_BIPSRV_RegisterImagingResponderServiceRecord(*channel,
                                                           info,
                                                           &server->sdpInfo,
                                                           strings);
        }
        if (OI_SUCCESS(status)) {
            server->state = BIPSRV_STATE_REGISTERED;
#ifndef OI_CODE
            server->server_type = SERVER_IMAGE_RESPONDER;
#endif
        } else {
            /* failure somewhere, deregister server will cleanup as needed */
            OI_BIPSRV_DeregisterServer(*channel);
        }
    }
    else {
        DeallocateServer(server);
    }
    return status;
}

OI_STATUS OI_BIPSRV_RegisterReferencedObjects(const OI_BIPSRV_REFERENCED_OBJECTS_CALLBACKS *cb,
                                              OI_BIPSRV_OBJECTS_SERVER_INFO *info,
                                              const OI_SDP_STRINGS *strings,
                                              OI_BOOL authentication,
                                              OI_UINT8 *channel)
{
    OI_STATUS status;
    BIP_SERVER_DATA *server;
#ifndef OI_CODE
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_ImagingReferencedObjects, // service class UUID
        OI_SEC_REQUIRE_NOTHING,           // don't care
        FALSE,                            // don't care
        NULL,                             // don't care    
        0                                 // don't care    
    } ;
#endif

    OI_ENSURE_INIT(BIP_SRV);

    server = AllocateServer();
    if (server == NULL) {
        OI_LOG_ERROR(("Couldn't register the referenced objects server"));
        return OI_STATUS_NO_RESOURCES;
    }

    OI_ASSERT(server->state == BIPSRV_STATE_UNUSED);

    OI_MemZero(&server->cb, sizeof(server->cb));

    server->cb.refObj = *cb;

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_RegisterServer(&OI_BIP_ReferencedObjectsObexTargetUUID,
                                       &obexCallbacks,
                                       authentication ? OI_OBEXSRV_AUTH_PASSWORD : OI_OBEXSRV_AUTH_NONE,
                                       channel,
                                       &connectPolicy);
#endif /* OI_CODE */

    if (OI_SUCCESS(status)) {
        server->rfcommChannel = *channel;
        status = OI_BIPSRV_RegisterObjectsServerServiceRecord(*channel,
                                                        OI_UUID_ImagingReferencedObjects,
                                                        info,
                                                        &server->sdpInfo,
                                                        strings);
        if (OI_SUCCESS(status)) {
            server->state = BIPSRV_STATE_REGISTERED;
#ifndef OI_CODE
            server->server_type = SERVER_REFERENCED_OBJECTS;
#endif
        } else {
            OI_BIPSRV_DeregisterServer(*channel);
        }
    }
    else {
        DeallocateServer(server);
    }

    return status;
}

OI_STATUS OI_BIPSRV_RegisterArchivedObjects(const OI_BIPSRV_ARCHIVED_OBJECTS_CALLBACKS *cb,
                                            OI_BIPSRV_OBJECTS_SERVER_INFO *info,
                                            const OI_SDP_STRINGS *strings,
                                            OI_BOOL authentication,
                                            OI_UINT8 *channel)
{
    OI_STATUS status;
    BIP_SERVER_DATA *server;
#ifndef OI_CODE
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_ImagingAutomaticArchive, // service class UUID
        OI_SEC_REQUIRE_NOTHING,           // don't care
        FALSE,                            // don't care
        NULL,                             // don't care    
        0                                 // don't care    
    } ;
#endif                      

    OI_ENSURE_INIT(BIP_SRV);

    server = AllocateServer();
    if (server == NULL) {
        OI_LOG_ERROR(("Couldn't register the referenced objects server"));
        return OI_STATUS_NO_RESOURCES;
    }

    OI_ASSERT(server->state == BIPSRV_STATE_UNUSED);

    OI_MemZero(&server->cb, sizeof(server->cb));

    server->cb.archObj = *cb;

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_RegisterServer(&OI_BIP_ArchivedObjectsObexTargetUUID,
                                       &obexCallbacks,
                                       authentication ? OI_OBEXSRV_AUTH_PASSWORD : OI_OBEXSRV_AUTH_NONE,
                                       channel,
                                       &connectPolicy);
#endif /* OI_CODE */

    /* SDP registration is done by BT SIO */
    if (OI_SUCCESS(status)) {
        server->rfcommChannel = *channel;
        status = OI_BIPSRV_RegisterObjectsServerServiceRecord(*channel,
                                                        OI_UUID_ImagingAutomaticArchive,
                                                        info,
                                                        &server->sdpInfo,
                                                        strings);
        if (OI_SUCCESS(status)) {
            server->state = BIPSRV_STATE_REGISTERED;
#ifndef OI_CODE
            server->server_type = SERVER_ARCHIVED_OBJECTS;
#endif
        } else {
            OI_BIPSRV_DeregisterServer(*channel);
        }
    }
    else {
        DeallocateServer(server);        
    }

    return status;
}

OI_STATUS OI_BIPSRV_DeregisterServer(OI_UINT8 channel)
{
    OI_STATUS status;
    BIP_SERVER_DATA *server;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByRfcommChannel(channel);

    if (server == NULL) {
        return OI_STATUS_CHANNEL_NOT_FOUND;
    }

    status = OI_BIPSRV_DeregisterServiceRecord(&server->sdpInfo);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_BIPSRV_DestroySDPRecord"));
#endif
    }

    status = OI_OBEXSRV_DeregisterServer(channel);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXSRV_DeregisterServer"));
#endif
    }

    DeallocateServer(server);

    return OI_OK;
}

/*************************************************************************
 *
 *  Response-related functions
 *
 */

static OI_STATUS BIP_GetResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                 BIP_CMD function,
                                 const OI_OBEX_UNICODE *handle,
                                 const OI_OBEX_BYTESEQ  *descriptor,
                                 const OI_UINT32 *length,
                                 const OI_BIP_APP_PARAM_BOOKKEEPING *appParam,
                                 const OI_OBEX_BYTESEQ *data,
                                 OI_BOOL final,
                                 OI_STATUS result)
{
    BIP_SERVER_DATA *server;
    OI_UINT hdrCount = 0;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle"));
#endif
        return OI_STATUS_INVALID_HANDLE;
    }

    /*
     * If the caller reported an error status the GET operation is now complete
     * so we clean up and pass the error on to obex.
     */
    if ((result != OI_OBEX_CONTINUE) && !OI_SUCCESS(result)) {
        server->state = BIPSRV_STATE_CONNECTED;
        if (server->params != NULL) {
            DEBUG_ONLY(OI_MemZero(server->params, sizeof(BIP_PARAM_UNION)));
            OI_FreeIf(&server->params);
        }
        return OI_OBEXSRV_GetResponse(server->connectionId, NULL, result);
    }

    /* Extra headers should only be present for the first get response */
    if (server->requestState == OI_BIP_REQUEST_INITIAL) {
        if (handle != NULL) {
            server->obexHeader[hdrCount].id = OI_BIP_OBEX_HDR_IMG_HANDLE;
            server->obexHeader[hdrCount].val.unicode = *handle;
            hdrCount++;
        }

        if (descriptor != NULL) {
            server->obexHeader[hdrCount].id = OI_BIP_OBEX_HDR_IMG_DESCRIPTOR;
            server->obexHeader[hdrCount].val.byteseq = *descriptor;
            hdrCount++;
        }

        if (length != NULL) {
            server->obexHeader[hdrCount].id = OI_OBEX_HDR_LENGTH;
            server->obexHeader[hdrCount].val.length = *length;
            hdrCount++;
        }

        if (appParam != NULL) {
            server->obexHeader[hdrCount].id = OI_OBEX_HDR_APPLICATION_PARAMS;
            server->obexHeader[hdrCount].val.applicationParams = appParam->byteseq;
            hdrCount++;
        }
    } else {
        if (final) {
            /* 
             * informing the remote about the end of file information through 
             * the endflag 
             */
            if (appParam != NULL) {
                server->obexHeader[hdrCount].id = OI_OBEX_HDR_APPLICATION_PARAMS;
                server->obexHeader[hdrCount].val.applicationParams = appParam->byteseq;
                hdrCount++;
            }
        }
    }

    if (data != NULL) {
        server->obexHeader[hdrCount].id = final ? OI_OBEX_HDR_END_OF_BODY : OI_OBEX_HDR_BODY;
        server->obexHeader[hdrCount].val.byteseq = *data;
        hdrCount++;
    }

    OI_ASSERT(hdrCount <= OI_ARRAYSIZE(server->obexHeader));

    server->obexHeaderList.list = server->obexHeader;
    server->obexHeaderList.count = hdrCount;

    return OI_OBEXSRV_GetResponse(server->connectionId, &server->obexHeaderList, result);
}

static OI_STATUS BIP_PutResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                 BIP_CMD function,
                                 const OI_OBEX_UNICODE *handle,
                                 OI_STATUS result)
{
    BIP_SERVER_DATA *server;
    OI_UINT hdrCount = 0;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle"));
#endif
        return OI_STATUS_INVALID_HANDLE;
    }

    /*
     * If the caller reported an error status the PUT operation is now complete
     * so we clean up and pass the error on to obex.
     */
    if ((result != OI_OBEX_CONTINUE) && (result != OI_OBEX_PARTIAL_CONTENT) && !OI_SUCCESS(result)) {
        server->state = BIPSRV_STATE_CONNECTED;
        if (server->params != NULL) {
            DEBUG_ONLY(OI_MemZero(server->params, sizeof(BIP_PARAM_UNION)));
            OI_FreeIf(&server->params);
        }
        return OI_OBEXSRV_PutResponse(server->connectionId, NULL, result);
    }

    if (handle != NULL) {
        server->obexHeader[hdrCount].id = OI_BIP_OBEX_HDR_IMG_HANDLE;
        server->obexHeader[hdrCount].val.unicode = *handle;
        hdrCount++;
    }

    server->obexHeaderList.list = server->obexHeader;
    server->obexHeaderList.count = hdrCount;

    return OI_OBEXSRV_PutResponse(server->connectionId, &server->obexHeaderList, result);
}



OI_STATUS OI_BIPSRV_GetCapabilitiesResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                            OI_BIP_GET_CAPABILITIES_PARAMS *params,
                                            OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_CAPABILITIES,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}

OI_STATUS OI_BIPSRV_PutImageResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                     OI_BIP_PUT_IMAGE_PARAMS *params,
                                     OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_PUT_IMAGE,
                           params ? params->response.imageHandle : NULL,
                           result);
}

OI_STATUS OI_BIPSRV_PutLinkedThumbnailResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                               OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS *params,
                                               OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_PUT_LINKED_THUMBNAIL,
                           NULL,
                           result);
}


OI_STATUS OI_BIPSRV_PutLinkedAttachmentResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                                OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS *params,
                                                OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_PUT_LINKED_ATTACHMENT,
                           NULL,
                           result);
}


OI_STATUS OI_BIPSRV_RemoteDisplayResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                          OI_BIP_REMOTE_DISPLAY_PARAMS *params,
                                          OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_REMOTE_DISPLAY,
                           params ? params->response.imageHandle : NULL,
                           result);
}


OI_STATUS OI_BIPSRV_GetImagesListResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                          OI_BIP_GET_IMAGES_LIST_PARAMS *params,
                                          OI_STATUS result)
{
    BIP_SERVER_DATA *server;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle"));
#endif
        return OI_STATUS_INVALID_HANDLE;
    }

    OI_BIP_InitAppParams(&server->appParam,
                         server->appParamData,
                         MAX_PARAM_DATA_BYTES);

    OI_BIP_AppendAppParamUINT16(&server->appParam,
                               OI_BIP_OBEX_APP_PARAM_NB_RETURNED_HANDLES,
                               params->response.nbReturnedHandles);
    
    OI_BIP_FinishAppParams(&server->appParam);

    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_IMAGES_LIST,
                           NULL,    /* handle */
                           params->response.imageHandlesDescriptor,
                           NULL,    /* length */
                           &server->appParam,
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_GetImagePropertiesResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                               OI_BIP_GET_IMAGE_PROPERTIES_PARAMS *params,
                                               OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_IMAGE_PROPERTIES,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}

OI_STATUS OI_BIPSRV_GetImageResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                     OI_BIP_GET_IMAGE_PARAMS *params,
                                     OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_IMAGE,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           &params->response.imageFileSize,
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_GetLinkedThumbnailResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                               OI_BIP_GET_LINKED_THUMBNAIL_PARAMS *params,
                                               OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_LINKED_THUMBNAIL,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_GetLinkedAttachmentResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                                OI_BIP_GET_LINKED_ATTACHMENT_PARAMS *params,
                                                OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_LINKED_ATTACHMENT,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_DeleteImageResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                        OI_BIP_DELETE_IMAGE_PARAMS *params,
                                        OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_DELETE_IMAGE,
                           NULL,    /* handle */
                           result);
}


OI_STATUS OI_BIPSRV_StartPrintResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                       OI_BIP_START_PRINT_PARAMS *params,
                                       OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_START_PRINT,
                           NULL,    /* handle */
                           result);
}


OI_STATUS OI_BIPSRV_GetPartialImageResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                            OI_BIP_GET_PARTIAL_IMAGE_PARAMS *params,
                                            OI_STATUS result)
{
    BIP_SERVER_DATA *server;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Unkown OBEX connection handle"));
#endif
        return OI_STATUS_INVALID_HANDLE;
    }

    OI_BIP_InitAppParams(&server->appParam,
                         server->appParamData,
                         MAX_PARAM_DATA_BYTES);

    OI_BIP_AppendAppParamUINT32(&server->appParam,
                                OI_BIP_OBEX_APP_PARAM_TOTAL_FILE_SIZE,
                                params->response.totalFileSize);

    OI_BIP_AppendAppParamUINT8(&server->appParam,
                               OI_BIP_OBEX_APP_PARAM_END_FLAG,
                               (OI_UINT8)params->response.endFlag);

    OI_BIP_FinishAppParams(&server->appParam);

    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_PARTIAL_IMAGE,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           &params->response.partialLength,
                           &server->appParam,
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_StartArchiveResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                         OI_BIP_START_ARCHIVE_PARAMS *params,
                                         OI_STATUS result)
{
    return BIP_PutResponse(connectionId,
                           BIP_CMD_START_ARCHIVE,
                           NULL,    /* handle */
                           result);
}


OI_STATUS OI_BIPSRV_GetMonitoringImageResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                               OI_BIP_GET_MONITORING_IMAGE_PARAMS *params,
                                               OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_MONITORING_IMAGE,
                           params->response.imageHandle,
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           params->response.data,
                           params->response.final,
                           result);
}


OI_STATUS OI_BIPSRV_GetStatusResponse(OI_BIPSRV_CONNECTION_HANDLE connectionId,
                                      OI_STATUS result)
{
    return BIP_GetResponse(connectionId,
                           BIP_CMD_GET_STATUS,
                           NULL,    /* handle */
                           NULL,    /* descriptor */
                           NULL,    /* length */
                           NULL,    /* app params */
                           NULL,    /* data */
                           TRUE,    /* final */
                           result);
}



OI_STATUS OI_BIPSRV_SetContext(OI_BIPSRV_CONNECTION_HANDLE connection,
                               void *context)
{
    BIP_SERVER_DATA *server = BipServer(connection);
    if (server == NULL) {
        return OI_STATUS_NOT_FOUND;
    }
    server->context = context;
    return OI_OK;
}


void* OI_BIPSRV_GetContext(OI_BIPSRV_CONNECTION_HANDLE connection)
{
    BIP_SERVER_DATA *server = BipServer(connection);
    if (server == NULL) {
        return NULL;
    } else {
        return server->context;
    }
}

OI_STATUS OI_BIPSRV_ForceDisconnect(OI_BIPSRV_CONNECTION_HANDLE connectionId)
{
    BIP_SERVER_DATA *server;

    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByConnectionId(connectionId);
    if (server == NULL) {
#ifdef OI_CODE 
#error code not present
#else
        OI_LOG_ERROR(("Unknown BIPSRV connection handle"));
#endif
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (server->state < BIPSRV_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }

    return OI_OBEXSRV_ForceDisconnect(server->connectionId);
}


#ifdef OI_DEPRECATED
/**
 * @deprecated
 */
OI_STATUS OI_BIPSRV_Init(OI_UINT maxServers)
{
    /* The following static is required to support this deprecated BIPSRV
     * initialization api.  It has to be static because the configuration
     * table will contain a pointer to this config.
     */
    static OI_CONFIG_BIP_SRV deprecatedConfig;

    if (!OI_INIT_FLAG_VALUE(BIP_SRV)) {
        deprecatedConfig.maxServers = maxServers ;
        OI_CONFIG_TABLE_PUT(&deprecatedConfig, BIP_SRV);
        return OI_OK;
    }
    return OI_STATUS_ALREADY_REGISTERED;
}
#endif

OI_STATUS OI_BIPSRV_GetServiceRecord(OI_UINT8 channel, OI_UINT32 *handle)
{
    BIP_SERVER_DATA *server;

    if (!handle) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    OI_CHECK_INIT(BIP_SRV);

    server = LookupServerByRfcommChannel(channel);
    if (server == NULL) {
        OI_LOG_ERROR(("Unknown BIPSRV channel: %d", channel));
        return OI_STATUS_CHANNEL_NOT_FOUND;
    }
#ifdef OI_CODE
#error code not present
#else
    if ( server->sdpInfo != NULL ) {
       *handle = server->sdpInfo->serviceRecordHandle;
    }
#endif
    return OI_OK;
}

#ifndef OI_CODE
OI_STATUS OI_BIPSRV_RegisterImagingResponderServiceRecord(OI_UINT8 channel,
                                                    OI_BIPSRV_IMAGING_RESPONDER_INFO *serviceInfo,
                                                    OI_BIP_SDP_INFO **sdpInfo,
                                                    const OI_SDP_STRINGS *strings)
{
    return OI_OK;
}


OI_STATUS OI_BIPSRV_RegisterObjectsServerServiceRecord(OI_UINT8 channel,
                                                 OI_UINT16 uuid,
                                                 OI_BIPSRV_OBJECTS_SERVER_INFO *serviceInfo,
                                                 OI_BIP_SDP_INFO **sdpInfo,
                                                 const OI_SDP_STRINGS *strings)
{
   return OI_OK;
}


OI_STATUS OI_BIPSRV_DeregisterServiceRecord(OI_BIP_SDP_INFO **sdpInfo)
{
   return OI_OK;
}
#endif

