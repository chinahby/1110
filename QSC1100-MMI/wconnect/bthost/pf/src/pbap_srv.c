/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                S E R V E R

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile server 
  data and code.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_srv.c#1 $ 

$Log: $

*    #13       13 Feb 2008            RS
* Handle the situation when the remote sends type header with
* no string.
*
*    #12       04 Dec 2007            MH
* Added support for updating BD Address with connect indication.
*
*    #11       27 Jun 2007            MH
* Fixed issue in PBAP server connect indication calling accept 
* connection twice.
*
*    #10       07 Jun 2007            GS
* Updated obexSetPathInd to correctly handle set path to parent when
* folder name is also present.
*
*    #9        21 May 2007            SSK
* Fix compiler warnings.
*
*    #8        28 Feb 2007            MH
* Modified disconnect indication to support optional headers
*
*    #7        13 Feb 2007            MH
* Modified Set path indication to support optional headers
*
*    #6        26 Oct 2006            MH
* Modified obex connect indication to support all headers
*
*    #5        23 Oct 2006            GS
* Updated OBEX error codes to confirm to spec.  Updated debug messages.
*
*    #4        29 July 2006           BK 
* Modified to initiate obex authentication from the client 
*
*    #3        27 Jun 2006            GS
* Updated to use revised OBEX APIs for Connect, Disconnect, SetPath
* and AcceptConnect.
*
*    #2        01 Jun 2006            GS
* Updated obexPutInd to return 'bad request' as response code.
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/
#define __OI_MODULE__ OI_MODULE_PBAP_SRV

#include "oi_assert.h"
#include "oi_argcheck.h"
#include "pbap_private.h"
#include "pbap_srv.h"
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
    OI_OBEX_BYTESEQ const *type;
    PBAP_CMD command; 
    OI_UINT16 params;
} PBAP_TYPEMAP;

static const PBAP_TYPEMAP GetCommandMap[] = {
    { &OI_PBAP_PullPhoneBookType, PBAP_CMD_PULL_PHONE_BOOK, (PBAP_PARAM_FILTER |
                                                             PBAP_PARAM_FORMAT |
                                                             PBAP_PARAM_MAX_LIST_COUNT |
                                                             PBAP_PARAM_LIST_START_OFFSET) },
    { &OI_PBAP_PullvCardListingType, PBAP_CMD_PULL_VCARD_LISTING, 
                                                            (PBAP_PARAM_ORDER |
                                                             PBAP_PARAM_VALUE |
                                                             PBAP_PARAM_ATTRIBUTE |
                                                             PBAP_PARAM_MAX_LIST_COUNT |
                                                             PBAP_PARAM_LIST_START_OFFSET) },
    { &OI_PBAP_PullvCardEntryType, PBAP_CMD_PULL_VCARD_ENTRY, 
                                                            (PBAP_PARAM_FILTER |
                                                             PBAP_PARAM_FORMAT) }
};


typedef enum {
    PBAP_SRV_STATE_UNUSED,
    PBAP_SRV_STATE_IDLE,
    PBAP_SRV_STATE_REGISTERED,
    PBAP_SRV_STATE_CONNECTED,
    PBAP_SRV_STATE_REQUEST
} PBAPSRV_STATE;


union PBAP_SRV_APP_PARAM_ELEMENTS {
    struct {
        OI_UINT16 phoneBookSize;
        OI_UINT8  newMissedCalls;
    } pullPhoneBookParams;

    struct {
        OI_UINT16 phoneBookSize;
        OI_UINT8  newMissedCalls;
    } pullvCardListingParams;
};

#define MAX_PARAM_ELEMENTS 5 

#define MAX_PARAM_DATA_BYTES ((MAX_PARAM_ELEMENTS * OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN) + sizeof(union PBAP_SRV_APP_PARAM_ELEMENTS))

#define MAX_RESPONSE_OBEX_HEADERS 5 

typedef struct {
    PBAPSRV_STATE state;
    OI_CONNECT_POLICY  connectPolicy;
    OI_UINT8 rfcommChannel;
    OI_OBEXSRV_CONNECTION_HANDLE connectionId;
    PBAP_REQUEST_STATE requestState;

    PBAPSRV_CALLBACKS cb;
    PBAP_APP_PARAM_BOOKKEEPING appParam;
    OI_BYTE appParamData[MAX_PARAM_DATA_BYTES];
    PBAP_GENERIC_PARAMS params;
    PBAP_TYPEMAP const *currentCommand;
    PBAP_REQUEST_HEADERS reqHdrs;
    OI_OBEX_HEADER obexHeader[MAX_RESPONSE_OBEX_HEADERS];
    OI_OBEX_HEADER_LIST obexHeaderList;

    OI_OBEX_UNICODE rootFolder;
    OI_UINT8 folderLevel;               /* 0 means at root folder */
    OI_UINT8 reqLevel; 

} PBAP_SERVER_DATA;

/* the following static is required to support the current PBAPSRV initialization api.
    It has to be static because the configuration table will contain a pointer to this config.
*/
static OI_CONFIG_PBAP_SRV deprecatedConfig ;

static PBAP_SERVER_DATA *this;


/*************************************************************************
 *
 *  Request identification, unmarshalling, and dispatching.
 *
 */

#define OBEX_BYTESEQ_EQUAL(s1, s2) (((s1).len == (s2).len) && (OI_MemCmp((s1).data, (s2).data, (s1).len) == 0))

#define ASSIGN_PARAM(function, param, header) \
    ((PBAP_##function##_PARAMS *)this->params)->request.param = this->reqHdrs.header
#define CLEAR_REQUEST_PARAMS(function) \
    OI_MemZero(&((PBAP_##function##_PARAMS *)this->params)->request, sizeof(PBAP_##function##_PARAMS))

/* Given a command, this function maps the unmarshalled OBEX headers to the
 * command-specific parameter structure, invokes the callback, and clears the
 * response parameters (since they aren't valid after the server's get/put
 * indication callback returns.)
 */

/**
 * Check that a file or folder name is a base name. That is, check that the name
 * does not include relative or absolute path. 
 *
 * Rejects names that contain "/", backslash, "..", or ":"
 */
#ifdef OI_CODE
#error code not present
#else
OI_BOOL ValidateFolderName(OI_OBEX_UNICODE *name)
#endif
{
    OI_INT i;

    /*
     * Reject . ..
     */
    if (name->str[0] == (OI_CHAR16) '.') {
        if ((name->len == 1) || (name->str[1] == 0)) {
            goto NotBase;
        }
        if (name->str[1] == ((OI_CHAR16) '.')) {
            goto NotBase;
        }
    }
    /*
     * Reject : / \  
     */
    for (i = 0; i < name->len; ++i) {
        OI_CHAR16 c = name->str[i];
        if ((c == (OI_CHAR16)(':')) || (c == (OI_CHAR16)('/')) || (c == (OI_CHAR16)('\\'))) {
            goto NotBase;
        }
    }
    return TRUE;

NotBase:

    BT_ERR("PBAP SRV: Invalid folder name", 0, 0, 0);
    return FALSE;
}

static OI_STATUS DispatchCallback(OI_STATUS status)
{
    OI_STATUS result = OI_STATUS_INVALID_STATE;

    OI_ASSERT(this != NULL);
    OI_ASSERT(this->state == PBAP_SRV_STATE_REQUEST);

    switch(this->currentCommand->command) {
        case PBAP_CMD_PULL_PHONE_BOOK:
            switch (this->requestState) {
                case PBAP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PULL_PHONE_BOOK, objName, name);
                    ASSIGN_PARAM(PULL_PHONE_BOOK, objType, type);
                    ASSIGN_PARAM(PULL_PHONE_BOOK, filter, filter);
                    ASSIGN_PARAM(PULL_PHONE_BOOK, format, format);
                    ASSIGN_PARAM(PULL_PHONE_BOOK, listCount, listCount);
                    ASSIGN_PARAM(PULL_PHONE_BOOK, startOffset, startOffset);

                    /* let the app know which app. params are present 
                      in the original request */
                    ASSIGN_PARAM(PULL_PHONE_BOOK, flags, flags);
                
                /* fall through */
                case PBAP_REQUEST_CONTINUE:
                case PBAP_REQUEST_CLEANUP:
                    result = this->cb.pullPhoneBook(this->connectionId, this->params, this->requestState, status);
            }

            CLEAR_REQUEST_PARAMS(PULL_PHONE_BOOK);
            break;

        case PBAP_CMD_PULL_VCARD_LISTING:
            switch (this->requestState) {
               case PBAP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PULL_VCARD_LISTING, objName, name);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, objType, type);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, order,  order);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, searchValue, value);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, searchAttrib, attribute);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, listCount, listCount);
                    ASSIGN_PARAM(PULL_VCARD_LISTING, startOffset, startOffset);

                    /* let the app know which app. params were present 
                      in the original request */
                    ASSIGN_PARAM(PULL_VCARD_LISTING, flags, flags);
                
                /* fall through */
                case PBAP_REQUEST_CONTINUE:
                case PBAP_REQUEST_CLEANUP:
                    result = this->cb.pullvCardListing(this->connectionId, this->params, this->requestState, status);
            }

            CLEAR_REQUEST_PARAMS(PULL_VCARD_LISTING);
            break;
            
        case PBAP_CMD_PULL_VCARD_ENTRY:
            switch (this->requestState) {
            case PBAP_REQUEST_INITIAL:
                    ASSIGN_PARAM(PULL_VCARD_ENTRY, objName, name);
                    ASSIGN_PARAM(PULL_VCARD_ENTRY, objType, type);
                    ASSIGN_PARAM(PULL_VCARD_ENTRY, filter, filter);
                    ASSIGN_PARAM(PULL_VCARD_ENTRY, format, format);

                    /* let the app know which app. params were present 
                      in the original request */
                    ASSIGN_PARAM(PULL_VCARD_ENTRY, flags, flags);
                
                /* fall through */
                case PBAP_REQUEST_CONTINUE:
                case PBAP_REQUEST_CLEANUP:
                    result = this->cb.pullvCardEntry(this->connectionId, this->params, this->requestState, status);
            }

            CLEAR_REQUEST_PARAMS(PULL_VCARD_ENTRY);
            break;

        default:
            BT_ERR("PBAP SRV: Invalid function - %d", 
                   this->currentCommand->command, 0, 0);
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
                            PBAP_TYPEMAP const *map,
                            OI_UINT mapLen)
{
    OI_BOOL first;
    OI_BOOL cleanup;
    OI_BOOL error;
    OI_UINT i;

    BT_MSG_DEBUG("PBAP SRV: requestInd - %x", status, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in requestInd: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    if (this->state < PBAP_SRV_STATE_CONNECTED) {
        OI_ASSERT(this->state >= PBAP_SRV_STATE_CONNECTED);
        BT_ERR("PBAP SRV: Invalid state - %d", this->state, 0, 0);
        return OI_STATUS_INVALID_STATE;
    }

    /* OI_OK and OI_OBEX_CONTINUE indicate success, other codes indicate failure */
    error = !(OI_SUCCESS(status) || (status == OI_OBEX_CONTINUE));

    /* This is to clarify tests based on whether this is the first request in a
     * sequence */
    first = (this->state == PBAP_SRV_STATE_CONNECTED);

    /* OBEXSRV signals that it's time to clean up any state associated with a
     * request by setting the header list to NULL. This can be because the
     * request completed successfully (including the transmission of response
     * data) or that the request was aborted by the user or otherwise terminated
     */
    cleanup = (cmdHeaders == NULL);

    /* Assert that  error => cleanup. The OBEXSRV api doesn't send header data
     * along with any error response */
    if (error){
        OI_ASSERT(cleanup);
    }

    if (cleanup) {
        this->requestState = PBAP_REQUEST_CLEANUP;
        if (this->currentCommand != NULL) {
            DispatchCallback(status);
        }
        this->state = PBAP_SRV_STATE_CONNECTED;
        if (this->params != NULL) {
            DEBUG_ONLY(OI_MemZero(this->params, sizeof(PBAP_PARAM_UNION)));
            OI_FreeIf(&this->params);
        }
        return status;
    }

    if (PBAP_UnmarshallHeaderData(&this->reqHdrs, cmdHeaders) != OI_OK) {
      return OI_OBEX_BAD_REQUEST;
    }

    /* The first request in a transaction must include a type header. If it
     * doesn't, or if the type header doesn't match a known command, an error is
     * returned to the client. Subsequent type headers shouldn't be present, but
     * are ignored if they are. */
    if (first) {
        OI_OBEX_BYTESEQ *type = this->reqHdrs.type;

#ifdef OI_CODE
#error code not present
#else
        if ( (type == NULL) || (type->data == NULL) ) {
#endif
            /* A type header must be present in the first request */
            BT_ERR("PBAP SRV: Type header or data missing in request", 0, 0, 0);
            return OI_OBEX_BAD_REQUEST;
        }

        this->currentCommand = NULL;
        for (i = 0; i < mapLen; ++i) {
            if (OBEX_BYTESEQ_EQUAL(*type, *map[i].type)) {
                this->currentCommand = &map[i];
                break;
            }
        }
        if (this->currentCommand == NULL) {
            /* The request wasn't found in the type map. Return an error. */
            BT_ERR("PBAP SRV: Unknown request type", 0, 0, 0);
            return OI_OBEX_BAD_REQUEST;
        }

        OI_ASSERT(this->params == NULL);  /* Catch a leak of this block */
        this->params = OI_Calloc(sizeof(PBAP_PARAM_UNION));

        if (this->params == NULL) {
            return OI_STATUS_NO_RESOURCES;
        }

        /* The request is recognized and implemented; we may continue. */
        this->state = PBAP_SRV_STATE_REQUEST;
        this->requestState = PBAP_REQUEST_INITIAL;
    } else {
        OI_ASSERT(this->state == PBAP_SRV_STATE_REQUEST);
        this->requestState = PBAP_REQUEST_CONTINUE;
    }

    status = DispatchCallback(status);
    if (!OI_SUCCESS(status)) {
        OI_FreeIf(&this->params);
        this->state = PBAP_SRV_STATE_CONNECTED;
    }

    return status;
}


/*************************************************************************
 *
 *  OBEX callbacks
 *
 */

static void AuthRequest(OI_OBEXSRV_CONNECTION_HANDLE connectionId);


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
    /* Reject Put requests */
    BT_ERR("OBEX SRV: OBEX PUT not allowed", 0, 0, 0);
    return OI_OBEX_BAD_REQUEST;
}

#ifdef OI_CODE
#error code not present
#else
static OI_STATUS obexSetPathInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                OI_OBEX_UNICODE *folder,
                                OI_BOOL upLevel,
                                OI_BOOL create,
                                OI_OBEX_HEADER_LIST *headers)
#endif
{

    OI_STATUS status;

    if ( create ) {
      BT_ERR("PBAP SRV: Operation not allowed, create=%d", create, 0, 0);
      return OI_OBEX_ACCESS_DENIED;
    }

    /*
     * NULL or empty folder name means set to root or parent folder
     */
    if ((folder == NULL) || (folder->str == NULL) || (folder->str[0] == 0)) {
        if (upLevel) {
            /*
             * Check we are not aleady at the root folder
             */
            if (this->folderLevel == 0) {
                BT_ERR("PBAP SRV: Already in root folder, folder=%d", 
                       this->folderLevel, 0, 0);
                return OI_OBEX_NOT_FOUND;
            }
            this->reqLevel = this->folderLevel - 1;
            return this->cb.setPhoneBook(this->connectionId, NULL, this->reqLevel);
        } else {
            this->reqLevel = 0;
            folder = &this->rootFolder;
        }
    } else {
        /*
         * Check we have a valid folder name
         */
        if (folder->len == 0) {
            BT_ERR("PBAP SRV: Folder len=0", 0, 0, 0);
            return OI_OBEX_BAD_REQUEST;
        }
        if (!ValidateFolderName(folder)) {
            BT_ERR("PBAP SRV: Invalid folder name", 0, 0, 0);
            return OI_OBEX_BAD_REQUEST;
        }
        if (upLevel) {
            if (this->folderLevel == 0) {
                BT_ERR("PBAP SRV: Already in root folder, folder=%d, len=%d", 
                       this->folderLevel, folder->len, 0);
                return OI_OBEX_NOT_FOUND;
            }
            this->reqLevel = this->folderLevel - 1;
            return this->cb.setPhoneBook(this->connectionId, NULL, this->reqLevel);
        }
        else {
            this->reqLevel = this->folderLevel + 1;
        }
    }

    status = this->cb.setPhoneBook(this->connectionId, folder, this->reqLevel);

    return status;
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
    OI_UINT8 channel;
    OI_BD_ADDR clientAddr;

    /* When the connection comes in, it may be with a connectionId that PBAP has never
     * seen before. We need to ask OBEX for the connection's associated rfcomm
     * channel in order to locate the correct server data */

    BT_MSG_DEBUG("PBAP SRV: Received Connect Ind", 0, 0, 0);

    status = OI_OBEXSRV_GetRfcommChannel(connectionId, &channel);
    if (!OI_SUCCESS(status)) {
        status = OI_OBEX_NOT_CONNECTED;
        goto error;
    }

    if (this->rfcommChannel != channel) {
        BT_ERR("PBAP SRV: No server registered on RFCOMM channel %d", 
               connectionId, 0, 0);
        status = OI_OBEX_NOT_CONNECTED;
        goto error;
    }

    /* At this point the server which should be associated with this channel has
     * been identified. We need to make sure it is free to accept requests. */

    if (this->state != PBAP_SRV_STATE_REGISTERED) {
        status = OI_STATUS_NO_RESOURCES;
        goto error;
    }

    /* The server isn't busy. If authentication has been requsted, pass the
     * request to the application */

    if (unauthorized) {
        BT_MSG_DEBUG("PBAP SRV: Obex requested credentials", 0, 0, 0);
        AuthRequest(connectionId);
        return;
    }

    /* Authentication, if any, has been processed. Now ask the application about
     * the request */
#ifdef OI_CODE
#error code not present
#else
    OI_OBEXSRV_GetClientAddr(connectionId, &clientAddr);
    status = this->cb.connectInd(connectionId, userId, userIdLen, &clientAddr);
#endif

    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, status);
#endif
    }
    return ;
    
error:
#ifdef OI_CODE
#error code not present
#else
    OI_OBEXSRV_AcceptConnect(connectionId, FALSE, NULL, status);
#endif
}

#ifdef OI_CODE
#error code not present
#else
static void obexDisconnectInd(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                            OI_OBEX_HEADER_LIST *headers)
#endif
{
    BT_MSG_DEBUG("PBAP SRV: Obex disconnect indication", 0, 0, 0);
    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in obexDisconnectInd: %d", 
               connectionId, 0, 0);
        return;
    }

    this->cb.disconnectInd(connectionId);
    OI_FreeIf(&this->params);
    this->state = PBAP_SRV_STATE_REGISTERED;
    this->connectionId = 0;
}

static void AuthRequest(OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    BT_MSG_DEBUG("PBAP SRV: Obex auth request", 0, 0, 0);

    if (this->cb.authInd) {
        this->connectionId = connectionId;
        this->cb.authInd(connectionId);
    } else {
        BT_MSG_DEBUG("PBAP SRV: doesn't support authentication. Refusing.", 0, 0, 0);
        OI_OBEXSRV_AuthenticationResponse(connectionId, NULL, FALSE);
    }
}

#ifndef OI_CODE
static OI_OBEXSRV_CB_LIST obexCallbacks = {
    obexConnectInd,
    obexDisconnectInd,
    obexGetInd,
    obexPutInd,
    obexSetPathInd,
    NULL
};
#else
#error code not present
#endif

/*************************************************************************
 *
 *  Initialization, authentication, and server registration
 *
 */

static void initData(void)
{
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_PhoneBookAccess_Server,  // OI_UINT32           serviceUuid32 ;
        OI_SEC_REQUIRE_NOTHING,          // securityPolicy is a place holder, get real policy at runtime
        FALSE,                           // OI_BOOL             mustBeMaster ;
        NULL,                            // OI_L2CAP_FLOWSPEC   *flowspec;
        0                                // OI_UINT8            powerSavingDisables ;
    } ;

    // get configuration
    (void)OI_CONFIG_TABLE_GET(PBAP_SRV) ;

    /** get security policy. */
    this->connectPolicy = connectPolicy;
    this->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(this->connectPolicy.serviceUuid32);
}

static OI_STATUS internalInit(void)
{

    /* make sure initialization only happens once - currently, the init flag is boolean */
    if (OI_INIT_FLAG_VALUE(PBAP_SRV)) {
        return(OI_STATUS_ALREADY_INITIALIZED) ;
    }

    // get configuration
    (void)OI_CONFIG_TABLE_GET(PBAP_SRV) ;

    this = OI_Calloc(sizeof(*this));
    if (NULL == this) {
        return (OI_STATUS_OUT_OF_MEMORY);
    }

    initData();

    // set out initialization flag TRUE - we're initialized
    OI_INIT_FLAG_PUT_FLAG(TRUE, PBAP_SRV) ;

    return OI_OK;
}

static OI_STATUS internalTerminate(void)
{
    // can only terminate if we're already initialized
    if (!OI_INIT_FLAG_VALUE(PBAP_SRV)) {
        return(OI_STATUS_NOT_INITIALIZED) ;
    }

    // release our memory
    OI_FreeIf(&this) ;

    // set initialization flag to show that we're not initialized
    OI_INIT_FLAG_PUT_FLAG(FALSE, PBAP_SRV) ;

    return(OI_OK) ;
}

OI_STATUS PBAPSRV_Init(OI_UINT maxServers)
{
    if (!OI_INIT_FLAG_VALUE(PBAP_SRV)) {
        deprecatedConfig.maxServers = 1;
        OI_CONFIG_TABLE_PUT(&deprecatedConfig, PBAP_SRV);
        return OI_OK;
    }
    return OI_STATUS_ALREADY_REGISTERED;
}

void PBAPSRV_AuthenticationResponse(PBAPSRV_HANDLE connectionId,
                                    OI_CHAR *pin)
{
    BT_MSG_DEBUG("PBAP SRV: AuthenticationResponse, pin=%s", pin, 0, 0);
    OI_OBEXSRV_AuthenticationResponse(connectionId, pin, FALSE);
}

OI_STATUS PBAPSRV_Register(const PBAPSRV_CALLBACKS *cb,
                           PBAPSRV_INFO *info,
                           const OI_SDP_STRINGS *strings,
                           OI_OBEXSRV_AUTHENTICATION authentication,
                           OI_UINT8 *channel)
{
    OI_STATUS status;
    static const OI_BYTE uuid[OI_OBEX_UUID_SIZE] = OBEX_PHONE_BOOK_ACCESS_UUID;
    OI_OBEX_BYTESEQ target = { OI_ARRAYSIZE(uuid), (OI_BYTE*) uuid };

    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_PhoneBookAccess_Server,  // service class UUID
        OI_SEC_REQUIRE_NOTHING,          // don't care
        FALSE,                           // don't care
        NULL,                            // don't care    
        0                                // don't care    
    } ;

    OI_ENSURE_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: Register", 0, 0, 0);

    ARGCHECK(cb != NULL);
    ARGCHECK(info != NULL);

    if (info->supportedRepositories == 0) {
        BT_ERR("PBAP SRV: Register wasn't called with any supported repositories",
               0, 0, 0);
        return OI_STATUS_INVALID_PARAMETERS;
    }

    OI_ASSERT(this->state == PBAP_SRV_STATE_UNUSED);

    this->cb = *cb;

    status = OI_OBEXSRV_RegisterServer(&target,
                                       &obexCallbacks,
                                       authentication,
                                       channel,
                                       &connectPolicy);

    if (OI_SUCCESS(status)) {
        this->state = PBAP_SRV_STATE_REGISTERED;
        this->rfcommChannel = *channel;
    } else {
        OI_OBEXSRV_DeregisterServer(*channel);
    }

    return status;
}

OI_STATUS PBAPSRV_DeregisterServer(OI_UINT8 channel)
{
    OI_STATUS status;

    OI_CHECK_INIT(PBAP_SRV);
    BT_MSG_DEBUG("PBAP SRV: Deregister", 0, 0, 0);

    status = OI_OBEXSRV_DeregisterServer(channel);
    if (!OI_SUCCESS(status)) {
        BT_ERR("PBAP SRV: Deregister failed: %d", status, 0, 0);
    }

    OI_MemZero(this, sizeof(*this));
    this->state = PBAP_SRV_STATE_UNUSED;

    internalTerminate();

    return OI_OK;
}


OI_STATUS PBAPSRV_AcceptConnection(PBAPSRV_HANDLE connectionId,
                                   OI_BOOL accept)
{
    OI_STATUS status = OI_OBEX_ACCESS_DENIED;

    if (!OI_INIT_FLAG_VALUE(PBAP_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    BT_MSG_DEBUG("PBAP SRV: AcceptConnection, accept=%d", accept, 0, 0);

    /*
     * If the connection is being rejected or was unauthorized reject the
     * connection.
     */
    if (!accept) {
        goto Reject;
    }

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXSRV_AcceptConnect(connectionId, TRUE, NULL, OI_OK);
#endif
    if (OI_SUCCESS(status)) {
        this->state = PBAP_SRV_STATE_CONNECTED;
        this->connectionId = connectionId;
    } else {
        this->connectionId = 0;
        this->state = PBAP_SRV_STATE_IDLE;
    }

    return status;

Reject:

#ifdef OI_CODE
#error code not present
#else
    OI_OBEXSRV_AcceptConnect(this->connectionId, FALSE, NULL, status);
#endif

    return status;
}

/*************************************************************************
 *
 *  Response-related functions
 *
 */

static OI_STATUS PBAP_GetResponse(PBAPSRV_HANDLE connectionId,
                                  PBAP_CMD function,
                                  const OI_UINT32 *length,
                                  const PBAP_APP_PARAM_BOOKKEEPING *appParam,
                                  const OI_OBEX_BYTESEQ *data,
                                  OI_BOOL final,
                                  OI_STATUS result)
{
    OI_UINT hdrCount = 0;

    OI_CHECK_INIT(PBAP_SRV);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in GetResponse: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    if (length != NULL) {
        this->obexHeader[hdrCount].id = OI_OBEX_HDR_LENGTH;
        this->obexHeader[hdrCount].val.length = *length;
        hdrCount++;
    }

    if (appParam != NULL) {
        this->obexHeader[hdrCount].id = OI_OBEX_HDR_APPLICATION_PARAMS;
        this->obexHeader[hdrCount].val.applicationParams = appParam->byteseq;
        hdrCount++;
    }

    if (data != NULL) {
        this->obexHeader[hdrCount].id = final ? OI_OBEX_HDR_END_OF_BODY : OI_OBEX_HDR_BODY;
        this->obexHeader[hdrCount].val.byteseq = *data;
        hdrCount++;
    }

    OI_ASSERT(hdrCount <= OI_ARRAYSIZE(this->obexHeader));

    this->obexHeaderList.list = this->obexHeader;
    this->obexHeaderList.count = hdrCount;

    /* Do clean up here.  GetInd cb will not be invoked again since we
       are sending error response */
    if ((result != OI_OK) && (result != OI_OBEX_CONTINUE)) {
        this->state = PBAP_SRV_STATE_CONNECTED;
        if (this->params != NULL) {
            DEBUG_ONLY(OI_MemZero(this->params, sizeof(PBAP_PARAM_UNION)));
            OI_FreeIf(&this->params);
        }
    }

    return OI_OBEXSRV_GetResponse(this->connectionId, &this->obexHeaderList, result);
}

OI_STATUS PBAPSRV_PullPhoneBookResponse(PBAPSRV_HANDLE connectionId,
                                        PBAP_PULL_PHONE_BOOK_PARAMS *params,
                                        OI_STATUS result)
{
    OI_CHECK_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: PullPhoneBookResponse, result=%x", result, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in PullPhoneBookResponse: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    PBAP_InitAppParams(&this->appParam,
                       this->appParamData,
                       MAX_PARAM_DATA_BYTES);

    if (params->response.flags & PBAP_PARAM_PHONE_BOOK_SIZE) {
        PBAP_AppendAppParamUINT16(&this->appParam,
                                  PBAP_OBEX_APP_PARAM_PHONE_BOOK_SIZE,
                                  params->response.phoneBookSize);
    }

    if (params->response.flags & PBAP_PARAM_NEW_MISSED_CALLS) {
        PBAP_AppendAppParamUINT8(&this->appParam,
                                 PBAP_OBEX_APP_PARAM_NEW_MISSED_CALLS,
                                 params->response.missedCalls);
    }

    PBAP_FinishAppParams(&this->appParam);

    return PBAP_GetResponse(connectionId,
                            PBAP_CMD_PULL_PHONE_BOOK,
                            NULL,    /* length */
                            &this->appParam,
                            params->response.data,
                            params->response.final,
                            result);
}

OI_STATUS PBAPSRV_SetPhoneBookResponse(PBAPSRV_HANDLE connectionId,
                                       OI_STATUS result)
{
    OI_CHECK_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: SetPhoneBookResponse, result=%x", result, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in SetPhoneBookResponse: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    if (OI_SUCCESS(result)) {
        this->folderLevel = this->reqLevel;
    }
#ifdef OI_CODE
#error code not present
#else
    return OI_OBEXSRV_ConfirmSetpath(connectionId, NULL, result);
#endif
}

OI_STATUS PBAPSRV_PullvCardListingResponse(PBAPSRV_HANDLE connectionId,
                                           PBAP_PULL_VCARD_LISTING_PARAMS *params,
                                           OI_STATUS result)
{
    OI_CHECK_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: PullvCardListingResponse, result=%x", result, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in PullvCardListingResponse: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    PBAP_InitAppParams(&this->appParam,
                       this->appParamData,
                       MAX_PARAM_DATA_BYTES);

    if (params->response.flags & PBAP_PARAM_PHONE_BOOK_SIZE) {
        PBAP_AppendAppParamUINT16(&this->appParam,
                                  PBAP_OBEX_APP_PARAM_PHONE_BOOK_SIZE,
                                  params->response.phoneBookSize);
    }

    if (params->response.flags & PBAP_PARAM_NEW_MISSED_CALLS) {
        PBAP_AppendAppParamUINT8(&this->appParam,
                                 PBAP_OBEX_APP_PARAM_NEW_MISSED_CALLS,
                                 params->response.missedCalls);
    }

    PBAP_FinishAppParams(&this->appParam);

    return PBAP_GetResponse(connectionId,
                            PBAP_CMD_PULL_VCARD_LISTING,
                            NULL,    /* length */
                            &this->appParam,
                            params->response.data,
                            params->response.final,
                            result);
}

OI_STATUS PBAPSRV_PullvCardEntryResponse(PBAPSRV_HANDLE connectionId,
                                         PBAP_PULL_VCARD_ENTRY_PARAMS *params,
                                         OI_STATUS result)
{
    OI_CHECK_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: PullvCardEntryResponse, result=%x", result, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown OBEX conn id in PullvCardEntryResponse: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_HANDLE;
    }

    return PBAP_GetResponse(connectionId,
                            PBAP_CMD_PULL_VCARD_ENTRY,
                            NULL,    /* length */
                            NULL,    /* app params */
                            params->response.data,
                            params->response.final,
                            result);
}

OI_STATUS PBAPSRV_ForceDisconnect(PBAPSRV_HANDLE connectionId)
{
    OI_CHECK_INIT(PBAP_SRV);

    BT_MSG_DEBUG("PBAP SRV: ForceDisconnect", 0, 0, 0);

    if (this->connectionId != connectionId) {
        BT_ERR("PBAP SRV: Unknown conn id in ForceDisconnect: %d", 
               connectionId, 0, 0);
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (this->state < PBAP_SRV_STATE_CONNECTED) {
        return OI_OBEX_NOT_CONNECTED;
    }

    return OI_OBEXSRV_ForceDisconnect(this->connectionId);
}


