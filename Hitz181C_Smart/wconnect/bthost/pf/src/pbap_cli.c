/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E B O O K   A C C E S S   P R O F I L E   
                                C L I E N T

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile client 
  data and code.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_cli.c#1 $ 

$Log: $

*    #7        28 Feb 2007            MH
* Auth indication modified to support full access 
*
*    #6        23 Oct 2006            GS
* Send Name header in PullvCardListing even if it is empty. 
* Fixed issue in handling OBEX continue response without a body or 
* end-of-body header.  Updated debug messages. 
*
*    #5        19 Sept 2006           MH
* Changed to support OI client connect
*
*    #4        29 Aug 2006            GS
* Fixing klocwork error.
*
*    #3        27 Jun 2006            GS
* Updated to use revised OBEX APIs for Connect, Disconnect, SetPath
* and AcceptConnect.
*
*    #2        15 Jun 2006            GS
* Fixed problem in sending sort attribute in PullvCardListing
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/
#define __OI_MODULE__ OI_MODULE_PBAP_CLI

#include "oi_bt_assigned_nos.h"
#include "pbap_cli.h"
#include "pbap_spec.h"
#include "pbap_private.h"
#include "oi_obexcli.h"
#include "oi_assert.h"
#include "oi_argcheck.h"
#include "oi_securitydb.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

#include "oi_obexcommon.h"

typedef union {
    PBAPCLI_PULL_PHONE_BOOK_CFM    pullPhoneBook;  
    PBAPCLI_SET_PHONE_BOOK_CFM     setPhoneBook;
    PBAPCLI_PULL_VCARD_LISTING_CFM pullvCardListing;
    PBAPCLI_PULL_VCARD_ENTRY_CFM   pullvCardEntry;
} PBAP_CALLBACK_UNION;

/** Implementation */

/** This type is never instantiated. Rather, it exists to document the types and
 * sizes of the application parameters used by the PBAP request functions.
 * Compiler-added padding may cause the size of this data structure to be
 * slightly overestimated. This should be minimal.*/
union _PBAP_CLI_APP_PARAM_ELEMENTS {
  struct {
    UINT64    filter;
    OI_UINT8  format;
    OI_UINT16 listCount;
    OI_UINT16 startOffset;
  } pullPhoneBookParams;

  struct {
    OI_UINT8  order;
    OI_OBEX_BYTESEQ  value;  // UTF-8 
    OI_UINT8  attribute; 
    OI_UINT16 listCount;
    OI_UINT16 startOffset;
  } pullvCardListing;

  struct {
    UINT64    filter;
    OI_UINT8  format;
  } pullvCardEntry;
};

#define MAX_PARAM_ELEMENTS 5    /* Max number of app params */

#define MAX_PARAM_DATA_BYTES ((MAX_PARAM_ELEMENTS * OI_OBEX_APPLICATION_PARAMETER_PREFIX_LEN) + sizeof(union _PBAP_CLI_APP_PARAM_ELEMENTS))

/**
 * Client connection states
 */
typedef enum {
    PBAP_CLI_STATE_IDLE,
    PBAP_CLI_STATE_CONNECTING,
    PBAP_CLI_STATE_CONNECTED,
    PBAP_CLI_STATE_REQUEST_IN_PROGRESS,
    PBAP_CLI_STATE_ABORTING
} PBAP_CLI_STATE;


/**
 * Struct for PBAP client connection.
 */
typedef struct {
    OI_CONNECT_POLICY connectPolicy;

    PBAP_APP_PARAM_BOOKKEEPING param;
    OI_BYTE paramData[MAX_PARAM_DATA_BYTES];
    OI_OBEXCLI_CONNECTION_HANDLE handle;
    
    PBAP_CLI_STATE state;

    PBAPCLI_AUTH_IND authInd;
    PBAPCLI_CONNECT_CFM connectCfm;
    PBAPCLI_DISCONNECT_IND disconnectInd;
    
    union {
        struct {
            OI_OBEX_HEADER obexHeader[1];
            OI_OBEX_HEADER_LIST obexHeaderList;
            OI_BYTE uuidBytes[OI_BT_UUID128_SIZE];
        } connection;
        struct {
            PBAP_CMD currentFunction;
            PBAP_CALLBACK_UNION callback;
            PBAP_GENERIC_PARAMS params;
            void *paramData;
            PBAP_APP_PARAM_BOOKKEEPING param;
            PBAP_REQUEST_HEADERS reqHdrs;
        } operation;
    } data;
} PBAP_CLIENT_DATA;

static PBAP_CLIENT_DATA *this = NULL;

/** This is a helper macro to cast the saved callback function to the
 * appropriate type and invoke it with the required arguments.
 */
#define INVOKE_CALLBACK(_function, _args) (this->data.operation.callback._function) _args

#define PARAM_STRUCT(_function) ((PBAP_##_function##_PARAMS *)this->data.operation.params)


static OI_STATUS internalTerminate(void);

static void DispatchCallback(PBAP_CMD command, OI_STATUS status)
{
    switch (this->data.operation.currentFunction) {
        case PBAP_CMD_PULL_PHONE_BOOK:
            PARAM_STRUCT(PULL_PHONE_BOOK)->response.phoneBookSize = this->data.operation.reqHdrs.phoneBookSize;
            PARAM_STRUCT(PULL_PHONE_BOOK)->response.missedCalls = this->data.operation.reqHdrs.missedCalls;
            PARAM_STRUCT(PULL_PHONE_BOOK)->response.data = this->data.operation.reqHdrs.body;
            PARAM_STRUCT(PULL_PHONE_BOOK)->response.final = this->data.operation.reqHdrs.final;

            PARAM_STRUCT(PULL_PHONE_BOOK)->response.flags = this->data.operation.reqHdrs.flags;
            INVOKE_CALLBACK(pullPhoneBook, (this->handle, this->data.operation.params, status));
            break;

        case PBAP_CMD_SET_PHONE_BOOK:
            INVOKE_CALLBACK(setPhoneBook, (this->handle, status));
            break;

        case PBAP_CMD_PULL_VCARD_LISTING:
            PARAM_STRUCT(PULL_VCARD_LISTING)->response.phoneBookSize = this->data.operation.reqHdrs.phoneBookSize;
            PARAM_STRUCT(PULL_VCARD_LISTING)->response.missedCalls = this->data.operation.reqHdrs.missedCalls;
            PARAM_STRUCT(PULL_VCARD_LISTING)->response.data = this->data.operation.reqHdrs.body;
            PARAM_STRUCT(PULL_VCARD_LISTING)->response.final = this->data.operation.reqHdrs.final;

            PARAM_STRUCT(PULL_VCARD_LISTING)->response.flags = this->data.operation.reqHdrs.flags;
            INVOKE_CALLBACK(pullvCardListing, (this->handle, this->data.operation.params, status));
            break;

        case PBAP_CMD_PULL_VCARD_ENTRY:
            PARAM_STRUCT(PULL_VCARD_ENTRY)->response.data = this->data.operation.reqHdrs.body;
            PARAM_STRUCT(PULL_VCARD_ENTRY)->response.final = this->data.operation.reqHdrs.final;
            INVOKE_CALLBACK(pullvCardEntry, (this->handle, this->data.operation.params, status));
            break;
                
      default:
            BT_ERR("PBAP CLI: Invalid this->currentFunction: %d", 
                   this->data.operation.currentFunction, 0, 0);
    }
}
 
/*
 * 
 */
static void obexConnectCfm(OI_OBEXCLI_CONNECTION_HANDLE connectionId, 
                           OI_BOOL readOnly, 
#ifndef OI_CODE
                           OI_OBEX_HEADER_LIST *hdrs,
#endif
                           OI_STATUS status)
{
    if (connectionId != this->handle || this->state != PBAP_CLI_STATE_CONNECTING) {
        BT_ERR("PBAP CLI: Received unexpected OBEX connect cfm", 0, 0, 0);
    } else {
        BT_MSG_DEBUG("PBAP CLI: obexConnectCfm: %d", status, 0, 0);
        if (OI_SUCCESS(status)) {
            this->state = PBAP_CLI_STATE_CONNECTED;
        } else {
            this->state = PBAP_CLI_STATE_IDLE;
        }

        
        OI_ASSERT(this->connectCfm);
        this->connectCfm(this->handle, status);
    }
}

#ifdef OI_CODE
#error code not present
#else
static void obexDisconnectInd(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                              OI_OBEX_HEADER_LIST *hdrs)
#endif
{
    BT_MSG_DEBUG("PBAP CLI: Obex disconnected", 0, 0, 0);
    OI_ASSERT(this->disconnectInd != NULL);

    this->state = PBAP_CLI_STATE_IDLE;
    this->disconnectInd(this->handle);

    internalTerminate();
}

static void obexAuthChallengeInd(OI_OBEXCLI_CONNECTION_HANDLE connectionId, 
#ifndef OI_CODE
                                 OI_BOOL fullAccess,
#endif
                                 OI_BOOL userIdRequired)
{
    BT_MSG_DEBUG("PBAP CLI: Obex Auth Challenge Ind", 0, 0, 0);
    if (this->authInd == NULL) {
        BT_ERR("PBAP CLI: authInd is NULL!", 0, 0, 0);
        OI_OBEXCLI_AuthenticationResponse(connectionId, NULL, 0, NULL);
    } else {
        this->authInd(connectionId, userIdRequired);
    }
    
}

OI_STATUS PBAPCLI_AuthenticationResponse(PBAPCLI_HANDLE connectionId,
                                         const OI_BYTE *userId,
                                         OI_UINT8 userIdLen,
                                         const OI_CHAR *pin)
{
    if (OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return OI_OBEXCLI_AuthenticationResponse(this->handle, userId, userIdLen, pin);
    } else {
        return OI_OBEX_INVALID_OPERATION;
    }
}


static OI_STATUS internalTerminate(void)
{
    // can only terminate if we're already initialized
    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_STATUS_NOT_INITIALIZED) ;
    }

    // release our memory
    OI_FreeIf(&this) ;

    // set initialization flag to show that we're not initialized
    OI_INIT_FLAG_PUT_FLAG(FALSE, PBAP_CLI) ;
    
    return(OI_OK) ;
}

OI_STATUS PBAPCLI_Connect(OI_BD_ADDR *addr,
                          OI_UINT8 channel,
                          PBAPCLI_AUTH_IND authInd,
                          PBAPCLI_CONNECT_CFM connectCfm,
                          PBAPCLI_DISCONNECT_IND disconnectInd,
                          PBAPCLI_HANDLE *handle)
{
    static const OI_BYTE target[OI_OBEX_UUID_SIZE] = OBEX_PHONE_BOOK_ACCESS_UUID;
    OI_STATUS status;
    OI_CONNECT_POLICY  connectPolicy =  {
        OI_UUID_PhoneBookAccess_Server,    
        OI_SEC_REQUIRE_NOTHING,    
        FALSE,                    
        NULL,                     
        0                            
    } ;

    ARGCHECK(connectCfm != NULL);
    ARGCHECK(disconnectInd != NULL);
    ARGCHECK(handle != NULL);

    BT_MSG_DEBUG("PBAP CLI: Connecting", 0, 0, 0);
    
    /* make sure initialization only happens once - currently, the init flag is boolean */
    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        this = OI_Calloc(sizeof(*this));
        if (!this) {
            return OI_STATUS_OUT_OF_MEMORY;
        }
        this->state = PBAP_CLI_STATE_IDLE;
        OI_INIT_FLAG_PUT_FLAG(TRUE, PBAP_CLI) ;
    }

    if (this->state != PBAP_CLI_STATE_IDLE) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    
    this->authInd = authInd;
    this->connectCfm = connectCfm;
    this->disconnectInd = disconnectInd;

    this->data.connection.obexHeader[0].id = OI_OBEX_HDR_TARGET;
    this->data.connection.obexHeader[0].val.target.data = (OI_BYTE*)target;
    this->data.connection.obexHeader[0].val.target.len = OI_OBEX_UUID_SIZE;

    this->data.connection.obexHeaderList.count = 1;
    this->data.connection.obexHeaderList.list = this->data.connection.obexHeader;

    // get security policy.
    this->connectPolicy = connectPolicy;
    this->connectPolicy.securityPolicy = OI_SECURITYDB_GetPolicy(this->connectPolicy.serviceUuid32);

    status = OI_OBEXCLI_Connect(addr,
                                channel,
                                &this->data.connection.obexHeaderList,
                                obexConnectCfm,
                                obexDisconnectInd,
                                obexAuthChallengeInd,
#ifndef OI_CODE
                                NULL, /*Challenge Response Call back */
#endif
                                handle,
                                &this->connectPolicy);

    if (OI_SUCCESS(status)) {
        this->state = PBAP_CLI_STATE_CONNECTING;
        this->handle = *handle;
    } else {
        internalTerminate();
    }
    
    return status;
}

OI_STATUS PBAPCLI_Disconnect(PBAPCLI_HANDLE connection)
{
    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    BT_MSG_DEBUG("PBAP CLI: Disconnecting", 0, 0, 0);
#ifdef OI_CODE
#error code not present
#else
    return OI_OBEXCLI_Disconnect(this->handle, NULL);
#endif
}

OI_STATUS PBAPCLI_Abort(PBAPCLI_HANDLE connection)
{
    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    return OI_OBEXCLI_AbortOperation(this->handle);
}

/*
 * PullPhoneBookCfm.
 *
 */
static void PullPhoneBookCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                             OI_OBEX_HEADER_LIST *rspHeaders,
                             OI_STATUS status)
{
    OI_BOOL final;
    OI_STATUS stat;
    
    OI_ASSERT(this->data.operation.currentFunction != PBAP_CMD_NONE);
    BT_MSG_DEBUG("PBAP CLI: PullPhoneBookCfm status=%d", status, 0, 0);

    final = (status != OI_OBEX_CONTINUE);
    
    if (final) {
        this->state = PBAP_CLI_STATE_CONNECTED;
    }

    stat = PBAP_UnmarshallHeaderData(&this->data.operation.reqHdrs, rspHeaders);

    if (stat != OI_OK) {
      BT_ERR("PBAP CLI: UnmarshallHeaderData failed in PullPhoneBookCfm, \
             stat=%x", stat, 0, 0);
    }

    this->data.operation.reqHdrs.final = final;

    DispatchCallback(this->data.operation.currentFunction, status);
}

/*
 * SetPhoneBookCfm.
 *
 */
static void SetPhoneBookCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
#ifndef OI_CODE
                            OI_OBEX_HEADER_LIST *rspHeaders,
#endif
                            OI_STATUS status)
{
    OI_ASSERT(this->data.operation.currentFunction != PBAP_CMD_NONE);
    BT_MSG_DEBUG("PBAP CLI: SetPhoneBookCfm status=%d", status, 0, 0);

    /*
     * Map errors to return more specific error information.
     */
    switch(status) {
        case OI_OBEX_NOT_FOUND:
            status = OI_OBEX_NO_SUCH_FOLDER;
            break;
        case OI_OBEX_METHOD_NOT_ALLOWED:
            status = OI_OBEX_FOLDER_BROWSING_NOT_ALLOWED;
            break;
        default:
            /* no mapping */
            break;
    }

    this->state = PBAP_CLI_STATE_CONNECTED;

    DispatchCallback(this->data.operation.currentFunction, status);
}

/*
 * PullvCardListingCfm.
 *
 */
static void PullvCardListingCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                                OI_OBEX_HEADER_LIST *rspHeaders,
                                OI_STATUS status)
{
    OI_BOOL final;
    OI_STATUS stat;
    
    OI_ASSERT(this->data.operation.currentFunction != PBAP_CMD_NONE);
    BT_MSG_DEBUG("PBAP CLI: PullvCardListingCfm status=%d", status, 0, 0);

    final = (status != OI_OBEX_CONTINUE);
    
    if (final) {
        this->state = PBAP_CLI_STATE_CONNECTED;
    }
    
    stat = PBAP_UnmarshallHeaderData(&this->data.operation.reqHdrs, rspHeaders);

    if (stat != OI_OK) {
      BT_ERR("PBAP CLI: UnmarshallHeaderData failed in PullvCardListingCfm, \
             stat=%x", stat, 0, 0);
    }

    this->data.operation.reqHdrs.final = final;

    DispatchCallback(this->data.operation.currentFunction, status);
}

/*
 * PullvCardEntryCfm.
 *
 */
static void PullvCardEntryCfm(OI_OBEXCLI_CONNECTION_HANDLE handle,
                              OI_OBEX_HEADER_LIST *rspHeaders,
                              OI_STATUS status)
{
    OI_BOOL final;
    OI_STATUS stat;
    
    OI_ASSERT(this->data.operation.currentFunction != PBAP_CMD_NONE);
    BT_MSG_DEBUG("PBAP CLI: PullvCardEntryCfm status=%d", status, 0, 0);

    final = (status != OI_OBEX_CONTINUE);
    
    if (final) {
        this->state = PBAP_CLI_STATE_CONNECTED;
    }
    
    stat = PBAP_UnmarshallHeaderData(&this->data.operation.reqHdrs, rspHeaders);

    if (stat != OI_OK) {
        BT_ERR("PBAP CLI: UnmarshallHeaderData failed in PullvCardEntryCfm, \
               stat=%x", stat, 0, 0);
    }

    this->data.operation.reqHdrs.final = final;

    DispatchCallback(this->data.operation.currentFunction, status);
}


/*** Requests ***********************************************************/

/*
 * PullPhoneBook
 * OBEX GET
 *
 */
OI_STATUS PBAPCLI_PullPhoneBook(PBAPCLI_HANDLE connection,
                                PBAPCLI_PULL_PHONE_BOOK_CFM cb,
                                PBAP_PULL_PHONE_BOOK_PARAMS *params)
{
    OI_STATUS           status;
    OI_BOOL             first;
    OI_OBEX_HEADER      hdrs[4];
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    BT_MSG_DEBUG("PBAP CLI: Issuing GET request for PullPhoneBook", 0, 0, 0);
    
    if (this->state < PBAP_CLI_STATE_CONNECTED) {
        BT_ERR("PBAP CLI: Not connected! state = %d", this->state, 0, 0);
        return OI_OBEX_NOT_CONNECTED;
    } else if (this->state == PBAP_CLI_STATE_REQUEST_IN_PROGRESS && 
               this->data.operation.currentFunction != PBAP_CMD_PULL_PHONE_BOOK) {
        BT_ERR("PBAP CLI: Operation in progress! state = %d, op = %d",
               this->state, this->data.operation.currentFunction, 0);
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (this->state == PBAP_CLI_STATE_CONNECTED) {
        first = TRUE;
        this->state = PBAP_CLI_STATE_REQUEST_IN_PROGRESS;
    } else {
        first = FALSE;
    }

    /* Build App. Params */
    PBAP_InitAppParams(&this->param,
                       this->paramData,
                       MAX_PARAM_DATA_BYTES);

    if (params->request.flags & PBAP_PARAM_FILTER) {
        PBAP_AppendAppParamUINT64(&this->param,
                                  PBAP_OBEX_APP_PARAM_FILTER,
                                  params->request.filter);
    }

    if (params->request.flags & PBAP_PARAM_FORMAT) {
        PBAP_AppendAppParamUINT8(&this->param,
                                 PBAP_OBEX_APP_PARAM_FORMAT,
                                 params->request.format);
    }

    if (params->request.flags & PBAP_PARAM_MAX_LIST_COUNT) {
        PBAP_AppendAppParamUINT16(&this->param,
                                  PBAP_OBEX_APP_PARAM_MAX_LIST_COUNT,
                                  params->request.listCount);
    }

    if (params->request.flags & PBAP_PARAM_LIST_START_OFFSET) {
        PBAP_AppendAppParamUINT16(&this->param,
                                  PBAP_OBEX_APP_PARAM_LIST_START_OFFSET,
                                  params->request.startOffset);
    }

    PBAP_FinishAppParams(&this->param);

      
    /* Build OBEX packet */
    hdrList.list  = hdrs;
    hdrList.count = 0;

    if (first) {
        hdrList.list[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrList.list[hdrList.count].val.type = *(params->request.objType); 
        hdrList.count++;

        if (params->request.objName->len != 0) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_NAME;
            hdrList.list[hdrList.count].val.name = *(params->request.objName);
            hdrList.count++;
        }

        hdrList.list[hdrList.count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
        hdrList.list[hdrList.count].val.applicationParams = this->param.byteseq;
        hdrList.count++;
    }

    status = OI_OBEXCLI_Get(this->handle, 
                            &hdrList, 
                            PullPhoneBookCfm, 
                            TRUE);
    if (OI_SUCCESS(status)) {
          this->data.operation.currentFunction = PBAP_CMD_PULL_PHONE_BOOK;
          this->data.operation.callback.pullPhoneBook = cb;
          this->data.operation.params = params;
    } else {
          this->data.operation.currentFunction = PBAP_CMD_NONE;
          this->data.operation.params = NULL;
    }

    return status;
}

/*
 * SetPhoneBook
 * OBEX SETPATH
 *
 */
OI_STATUS PBAPCLI_SetPhoneBook(PBAPCLI_HANDLE connection,
                               PBAPCLI_SET_PHONE_BOOK_CFM cb,
                               OI_OBEX_UNICODE *folder,
                               PBAP_CLIENT_SETPATH_CONTROL setPathCtrl)
{
    OI_STATUS           status;
    OI_BOOL             uplevel = FALSE;

    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    BT_MSG_DEBUG("Issuing SETPATH request for SetPhoneBook", 0, 0, 0);
    
    if (this->state < PBAP_CLI_STATE_CONNECTED) {
        BT_ERR("PBAP CLI: Not connected! state = %d", this->state, 0, 0);
        return OI_OBEX_NOT_CONNECTED;
    } else if (this->state == PBAP_CLI_STATE_REQUEST_IN_PROGRESS && 
               this->data.operation.currentFunction != PBAP_CMD_SET_PHONE_BOOK) {
        BT_ERR("PBAP CLI: Operation in progress! state = %d, op = %d",
               this->state, this->data.operation.currentFunction, 0);
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (this->state == PBAP_CLI_STATE_CONNECTED) {
        this->state = PBAP_CLI_STATE_REQUEST_IN_PROGRESS;
    } 

    if (setPathCtrl == PBAP_CLIENT_SETPATH_TO_PARENT) {
        uplevel = TRUE;
    } else {
        uplevel = FALSE;
        if (setPathCtrl == PBAP_CLIENT_SETPATH_TO_ROOT) {
            /*
             * Folder cannot be specified when setting path to root.
             */
            if ((folder != NULL) && (folder->len > 0)) {
                return OI_STATUS_INVALID_PARAMETERS;
            }
        }
    }

    status = OI_OBEXCLI_SetPath(this->handle, 
                                folder, 
                                TRUE, 
                                uplevel, 
#ifndef OI_CODE
                                NULL,
#endif
                                SetPhoneBookCfm);
    if (OI_SUCCESS(status)) {
          this->data.operation.currentFunction = PBAP_CMD_SET_PHONE_BOOK;
          this->data.operation.callback.setPhoneBook = cb;
          this->data.operation.params = NULL;
    } else {
          this->data.operation.currentFunction = PBAP_CMD_NONE;
          this->data.operation.params = NULL;
    }

    return status;
}

/*
 * PullvCardListing
 * OBEX GET
 *
 */
OI_STATUS PBAPCLI_PullvCardListing(PBAPCLI_HANDLE connection,
                                   PBAPCLI_PULL_VCARD_LISTING_CFM cb,
                                   PBAP_PULL_VCARD_LISTING_PARAMS *params)
{
    OI_STATUS           status;
    OI_BOOL             first;
    OI_OBEX_HEADER      hdrs[4];
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    BT_MSG_DEBUG("Issuing GET request for PullvCardListing", 0, 0, 0);
    
    if (this->state < PBAP_CLI_STATE_CONNECTED) {
        BT_ERR("PBAP CLI: Not connected! state = %d", this->state, 0, 0);
        return OI_OBEX_NOT_CONNECTED;
    } else if (this->state == PBAP_CLI_STATE_REQUEST_IN_PROGRESS && 
               this->data.operation.currentFunction != PBAP_CMD_PULL_VCARD_LISTING) {
        BT_ERR("PBAP CLI: Operation in progress! state = %d, op = %d",
               this->state, this->data.operation.currentFunction, 0);
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (this->state == PBAP_CLI_STATE_CONNECTED) {
        first = TRUE;
        this->state = PBAP_CLI_STATE_REQUEST_IN_PROGRESS;
    } else {
        first = FALSE;
    }

    /* Build App. Params */
    PBAP_InitAppParams(&this->param,
                       this->paramData,
                       MAX_PARAM_DATA_BYTES);

    if (params->request.flags & PBAP_PARAM_ORDER) {
        PBAP_AppendAppParamUINT8(&this->param,
                                 PBAP_OBEX_APP_PARAM_ORDER,
                                 params->request.order); 
    }

    if (params->request.flags & PBAP_PARAM_VALUE) {
        PBAP_AppendAppParamUTF8(&this->param,
                                PBAP_OBEX_APP_PARAM_SEARCH_VALUE,
                                &params->request.searchValue);
    }

    if (params->request.flags & PBAP_PARAM_ATTRIBUTE) {
        PBAP_AppendAppParamUINT8(&this->param,
                                 PBAP_OBEX_APP_PARAM_SEARCH_ATTRIBUTE,
                                 params->request.searchAttrib);
    }

    if (params->request.flags & PBAP_PARAM_MAX_LIST_COUNT) {
        PBAP_AppendAppParamUINT16(&this->param,
                                  PBAP_OBEX_APP_PARAM_MAX_LIST_COUNT,
                                  params->request.listCount);
    }

    if (params->request.flags & PBAP_PARAM_LIST_START_OFFSET) {
        PBAP_AppendAppParamUINT16(&this->param,
                                  PBAP_OBEX_APP_PARAM_LIST_START_OFFSET,
                                  params->request.startOffset);
    }

    PBAP_FinishAppParams(&this->param);

      
    /* Build OBEX packet */
    hdrList.list  = hdrs;
    hdrList.count = 0;

    if (first) {
        hdrList.list[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrList.list[hdrList.count].val.type = *(params->request.objType); 
        hdrList.count++;

        hdrList.list[hdrList.count].id = OI_OBEX_HDR_NAME;
        if (params->request.objName->len != 0) {
            hdrList.list[hdrList.count].val.name = *(params->request.objName);
        }
        else {
            hdrList.list[hdrList.count].val.name.str = NULL;
            hdrList.list[hdrList.count].val.name.len = 0;
        }
        hdrList.count++;

        hdrList.list[hdrList.count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
        hdrList.list[hdrList.count].val.applicationParams = this->param.byteseq;
        hdrList.count++;
    }

    status = OI_OBEXCLI_Get(this->handle, 
                            &hdrList, 
                            PullvCardListingCfm, 
                            TRUE);
    if (OI_SUCCESS(status)) {
          this->data.operation.currentFunction = PBAP_CMD_PULL_VCARD_LISTING;
          this->data.operation.callback.pullvCardListing = cb;
          this->data.operation.params = params;
    } else {
          this->data.operation.currentFunction = PBAP_CMD_NONE;
          this->data.operation.params = NULL;
    }

    return status;
}

/*
 * PullvCardEntry
 * OBEX GET
 *
 */
OI_STATUS PBAPCLI_PullvCardEntry(PBAPCLI_HANDLE connection,
                                 PBAPCLI_PULL_VCARD_ENTRY_CFM cb,
                                 PBAP_PULL_VCARD_ENTRY_PARAMS *params)
{
    OI_STATUS           status;
    OI_BOOL             first;
    OI_OBEX_HEADER      hdrs[4];
    OI_OBEX_HEADER_LIST hdrList;

    if (!OI_INIT_FLAG_VALUE(PBAP_CLI)) {
        return(OI_OBEX_NOT_CONNECTED) ;
    }

    BT_MSG_DEBUG("Issuing GET request for PullvCardEntry", 0, 0, 0);
    
    if (this->state < PBAP_CLI_STATE_CONNECTED) {
        BT_ERR("PBAP CLI: Not connected! state = %d", this->state, 0, 0);
        return OI_OBEX_NOT_CONNECTED;
    } else if (this->state == PBAP_CLI_STATE_REQUEST_IN_PROGRESS && 
               this->data.operation.currentFunction != PBAP_CMD_PULL_VCARD_ENTRY) {
        BT_ERR("PBAP CLI: Operation in progress! state = %d, op = %d",
               this->state, this->data.operation.currentFunction, 0);
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    if (this->state == PBAP_CLI_STATE_CONNECTED) {
        first = TRUE;
        this->state = PBAP_CLI_STATE_REQUEST_IN_PROGRESS;
    } else {
        first = FALSE;
    }

    /* Build App. Params */
    PBAP_InitAppParams(&this->param,
                       this->paramData,
                       MAX_PARAM_DATA_BYTES);

    if (params->request.flags & PBAP_PARAM_FILTER) {
        PBAP_AppendAppParamUINT64(&this->param,
                                  PBAP_OBEX_APP_PARAM_FILTER,
                                  params->request.filter);
    }
    if (params->request.flags & PBAP_PARAM_FORMAT) {
        PBAP_AppendAppParamUINT8(&this->param,
                                 PBAP_OBEX_APP_PARAM_FORMAT,
                                 params->request.format);
    }

    PBAP_FinishAppParams(&this->param);

      
    /* Build OBEX packet */
    hdrList.list  = hdrs;
    hdrList.count = 0;

    if (first) {
        hdrList.list[hdrList.count].id = OI_OBEX_HDR_TYPE;
        hdrList.list[hdrList.count].val.type = *(params->request.objType); 
        hdrList.count++;

        if (params->request.objName->len != 0) {
            hdrList.list[hdrList.count].id = OI_OBEX_HDR_NAME;
            hdrList.list[hdrList.count].val.name = *(params->request.objName);
            hdrList.count++;
        }

        hdrList.list[hdrList.count].id = OI_OBEX_HDR_APPLICATION_PARAMS;
        hdrList.list[hdrList.count].val.applicationParams = this->param.byteseq;
        hdrList.count++;
    }

    status = OI_OBEXCLI_Get(this->handle, 
                            &hdrList, 
                            PullvCardEntryCfm, 
                            TRUE);
    if (OI_SUCCESS(status)) {
          this->data.operation.currentFunction = PBAP_CMD_PULL_VCARD_ENTRY;
          this->data.operation.callback.pullvCardEntry = cb;
          this->data.operation.params = params;
    } else {
          this->data.operation.currentFunction = PBAP_CMD_NONE;
          this->data.operation.params = NULL;
    }

    return status;
}

