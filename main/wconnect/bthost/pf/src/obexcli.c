/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/obexcli.c#3 $ 
  $DateTime: 2009/05/28 13:18:41 $
  $Author: ganeshs $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------ 
  2009-05-27   gs  Changed Abort to asynchronous command
  2009-02-21   gs  Updated disconnect API to allow cancelling connection even
                   if connection state is RFCOMM_CONNECTING.
  2009-02-19   gs  Updated connection state check in OI_OBEXCLI_Disconnect() 
  2008-11-19   NM  Fixed obex auth interop issue with PTS(TC_SEN_OA_BV_01_I) 
  2008-07-24   gs  Fixed Klocwork error.

*    #35       30 Jul 2007            RS
* Allow the continue packets to pass onto the application.
*
*    #34       20 Jun 2007            GS
* Fixed debug message in ConnectionError().
*
*    #31-33    01 Jun 2007            PG
* Fixed incorrect change in #29.
*
*    #30       23 May 2007            GS
* Updated DisconnectResponse() to supress bogus error message.
*
*    #29       21 May 2007            SSK
* Fix compiler warnings.
*
*    #28       21 Mar 2007            MH
* Fixed not calling challenge indication for client initated auth
* Modified to take care realm string type.
*
*    #27       28 Feb 2007            MH
* Full access info added to challenge call back 
* Free connection receive buffere after disconnect indication  callback.
*
*    #26       14 Dec 2006            RP
* Pass the correct reason to App in case of connection error.
*
*    #25       22 Nov 2006            PR
* Merged OI Code v3.0.14rev1revQ and added new code to handle Abort.
*
*    #24       17 Nov 2006            MH
* Fix to send challenge response for server initiated challenge 
*
*    #23       10 Nov 2006            MH
* Resetting client realm info table
*
*    #22       13 Oct 2006            GS
* Send connection id header in all Get requests instead of just the very
* first one.
*
*    #21       25 Sept 2006           MH
* Added response codes to map response code function.
*
*    #20       22 Sept 2006           GS
* Ignore OBEX packets in OBEX_CONNECTED state instead of disconnecting
* RFCOMM.
*
*    #19       19 Sept 2006           MH
* New indication to client APP to diffentiate authentication initiator.
*
*    #18       12 Aug 2006            BK
* Obex Client initiated Authentication Fixes. Initialization & cleanup.
*
*    #17       01 Aug 2006            BK
* Removed compiler warnings.
*
*    #16       28 Jul 2006            JH
*  Use separate header count and header array to store headers received in
*  disconnect response, instead of reusing the connect header variables.
*
*  Modified for client initiated obex authentication. 
*
*    #14-15    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #13       23 Jun 2006            JH
* Added support for optional headers in the Disconnect and SetPath commands, as
*  well as the Connect, Disconnect, and SetPath responses for JSR82.
* For SetPath command, allow folder name to be specified even if upLevel is set.
*
*    #12       25 May 2006            GS
* Updated GetResponse() logic to handle special case (for BPP GetEvent
* SOAP message) when End-of-Body header is present in Continue respose 
* message.
*
*    #11       16 Nov 2005            JH
* Fixed compiler warning.
*    
*    #10       16 Nov 2005            GS
* Temporary fix for making sure that appropriate events are generated (always)
* if the link goes down in the middle of OBEX transfer.
*    
*    #9        12 Sep 2005            ABu
* Added code to AbortOperation to ensure that an IrDA LMP connection is 
* disconnected in all cases when our OBEX client aborts an OBEX server.
*
*    #8        04 Jul 2005            JH
* Added code to allow an abort command to be sent if server does not respond
*  to a previous put/get command.
*
*    #7        28 Feb 2005            GS
* Increased OBEX response timeout from 20sec to 60sec in order to interop
* better with HP BPP printer.
*
*    #6        31 Jan 2005            JH
* Added patch to send the connection ID header when sending disconnect and abort
*  commands if target was supplied when connecting.
*
*    #5        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
*    #3        30 Aug 2004            JH
* Modified ConnectionError to prevent connection->CB.connect.confirm from
*  being called in the OBEX_DISCONNECTING state.
*
===========================================================================*/
/**
@file  
@internal
  
OBEX client
*/

/**********************************************************************************
  $Revision: #3 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_OBEX_CLI

#include "oi_dispatch.h"
#include "oi_bytestream.h"
#include "oi_osinterface.h"
#include "oi_memmgr.h"
#include "oi_argcheck.h"
#include "oi_assert.h"
#include "oi_debug.h"
#include "oi_rfcomm_internal.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

#include "oi_obexcommon.h"
#include "oi_obexauth.h"
#include "oi_obexcli.h"

#ifndef OI_CODE
/* This is included to get BT Messages */
#include "btmsg.h"
#endif

/*
 * Timeout for a response to an abort command
 */
#define OBEX_ABORT_TIMEOUT   OI_SECONDS(5)

/*
 * How long we will wait for a disconnect to complete
 */
#define OBEX_DISCONNECT_TIMEOUT   OI_SECONDS(3)

/*
 * Normal timeout for a response to a command.
 */
#ifdef OI_CODE
#error code not present
#else
#define OBEX_DEFAULT_RESPONSE_TIMEOUT   OI_SECONDS(60)
#endif
/*
 * Longer timeout to allow for password entry.
 */
#define OBEX_AUTH_TIMEOUT   OI_SECONDS(120)


#define CONNECT_REQ_LEN     16
#define DISCONNECT_REQ_LEN  3

#define TMP_BYTESTREAM_SIZE 8

/**
 * Connection states - the order matters.
 */

typedef enum {
    INVALID_CONNECTION  = 0,
    RFCOMM_CONNECTING   = 1,
    RFCOMM_CONNECTED    = 2,
    OBEX_DISCONNECTING  = 3,
    OBEX_CONNECTED      = 4,
    OBEX_ABORTING       = 5,
    OBEX_GETTING        = 6,
    OBEX_PUTTING        = 7,
    OBEX_SETTING_PATH   = 8
} CONNECTION_STATE;


typedef enum {
    NO_ABORT,
    ABORT_GET,   /**< Get callback will be called */
    ABORT_PUT,   /**< Put callback will be called */
    GET_ABORTED, /**< Abort callback will be called */
    PUT_ABORTED  /**< Abort callback will be called */
} ABORT_STATE;


struct _PUT {
    OI_OBEXCLI_PUT_CFM confirm;
};

struct _GET {
    OI_OBEXCLI_GET_RECV_DATA recvData;
};

struct _CONNECT {
    OI_OBEXCLI_CONNECT_CFM confirm;
};

struct _SETPATH {
    OI_OBEXCLI_SETPATH_CFM confirm;
};

struct _ABORT {
    OI_OBEXCLI_ABORT_CFM confirm;
};

/**
 * Connection state 
 */

typedef struct {

    
    OBEX_COMMON common;

    /**
     * Application specific context.
     */
    void *context;

    OI_INTERVAL responseTimeout;     /** current timeout value on this connection */
    DISPATCH_CB_HANDLE timeoutCB;    /** dispatch function handle for timeout function */

    OI_OBEXCLI_DISCONNECT_IND disconnectInd;

    /**
     * Callback function for obtaining a password from the application.
     */
    OI_OBEXCLI_AUTH_CHALLENGE_IND challengeCB;

#ifndef OI_CODE
    /**
     * Callback function for indicating response recd for challenge. 
     */
    OI_OBEXCLI_AUTH_RESPONSE_IND authResponseCB;
#endif

    /**
     * Union of callback information for each cmd
     */
    union {
        struct _PUT put;
        struct _GET get;
        struct _CONNECT connect;
        struct _SETPATH setpath;
        struct _ABORT abort;
    } CB;

    /**
     * Copy of the headers passed in to a connect request. 
     */
    OI_OBEX_HEADER connectHdrs[OI_OBEX_MAX_CONNECT_HDRS];
    OI_UINT8 connectHdrCount;

#ifndef OI_CODE
    /**
     * Copy of the headers returned in a disconnect response.
     */
    OI_OBEX_HEADER disconnectHdrs[OI_OBEX_MAX_CONNECT_HDRS];
    OI_UINT8 disconnectHdrCount;
#endif /* OI_CODE */

    /**
     * If a target was specified != OI_OBEX_INVALID_CONNECTION_ID
     */
    OI_UINT32 cid;

    /**
     * Indicates if the current operation is being aborted.
     */
    ABORT_STATE abort;

    /*
     * Indicates that we are in the process of dropping the RFCOMM connection
     */
    OI_BOOL droppingLink;

    CONNECTION_STATE state;

#ifndef OI_CODE 
    /* 
    Flag set to TRUE, if the client APP initiaes Authentication 
    else set to FALSE
    */
    OI_BOOL cliAuthInit;

    /* 
     * If this flag is TRUE it indicates that the current operation needs to be
     * aborted.
     */ 
    OI_BOOL abortCurOp;

    /* Temporarily holds the reason for RFCOMM Disconnection */
    OI_STATUS disconStatus;

    OI_BOOL cancelConnect;
#endif

} OBEXCLI_CONNECTION;


/**
 * Context for asynchronously reporting an error.
 */

typedef struct {
    OBEXCLI_CONNECTION *connection;
    OI_STATUS status;
} OBEXCLI_ERROR_CONTEXT;

#ifndef OI_CODE

#define OI_OBEX_AUTH_TABLE_FREE     0xFF
/**
 * Flag to indicate that the client data stuctures are initialized
 */
OI_AUTH_INFO cliRealmInfo[OI_MAX_NUM_APPS];

static OI_STATUS SearchCliAuthInfo(OI_BD_ADDR *bdAddr,
                                   OI_UINT8 channel,
                                   OI_UINT16 *authIndex);
#endif /* OI_CODE */

/**
 * A well behaved application will wait for the confirmation callback before
 * issuing another command, however we have to protect against badly behaved
 * applications.  A connection is busy if there is an command in progress. Since
 * we set a timeout on all commands, if the timeout is set the connection is
 * busy.
 */
#define BUSY(connection)  ((connection)->timeoutCB != 0)


/***************** Forward function definition *************************/

static void HandleResponsePkt(OBEXCLI_CONNECTION *connection);

/************************************************************************/


/*
 * Connection state is stored in the RFCOMM link context.
 */
#define LookupConnection(link)  ((OBEXCLI_CONNECTION*) OI_RFCOMM_GetLinkContext(link))


/*
 * Call appropriate callback functions when a connection is dropped then free
 * the connection state information.
 * 
 * For GETTING/PUTTING we check BUSY() to determine if the get/put callback
 * needs to be called.
 */
static void FreeConnection(OI_RFCOMM_LINK_HANDLE link,
                           OBEXCLI_CONNECTION *connection,
                           OI_STATUS status)
{
#ifdef OI_CODE
#error code not present
#else
    OI_OBEX_HEADER_LIST hdrs;
#endif
    CONNECTION_STATE state = connection->state;

    OI_DBGPRINT2(("FreeConnection [%d] %!", link, status));

    /*
     * Invalidate state in case upper layer calls back in on the callback
     * thread.
     */
    connection->state = INVALID_CONNECTION;
    /*
     * Clear connection timeout if one is set. Note that this would also clear
     * BUSY() which is why we save the busy state above.
     */
    if (connection->timeoutCB) {
        OI_Dispatch_CancelFunc(connection->timeoutCB); 
        connection->timeoutCB = 0;
    }

    if (connection->common.mbuf) {
        OI_DBGPRINT(("Mbuf was not freed prior to disconnect"));
        OI_MBUF_Free(connection->common.mbuf);
        connection->common.mbuf = NULL;
    }

    /*
     * Report disconnect to application if there is an operation in progress.
     */
    switch (state) {
        case RFCOMM_CONNECTING:
        case RFCOMM_CONNECTED:
            /*
             * If we didn't establish  the OBEX connection we need to confirm
             * that the connect attempt failed rather than indicating a
             * disconnect.
             */
#ifdef OI_CODE
#error code not present
#else
            connection->disconStatus = OI_OK;
            connection->CB.connect.confirm(connection->common.link, FALSE, NULL, status);
#endif
            /*
             * This will prevent us from indicating a disconnect.
             */
            connection->disconnectInd = NULL;
            break;
        case OBEX_GETTING:
            OI_DBGPRINT2(("FreeConnection getting"));
#ifdef OI_CODE
#error code not present
#else
            connection->CB.get.recvData(link, NULL, status);
#endif
            break;
        case OBEX_PUTTING:
            OI_DBGPRINT2(("FreeConnection putting"));
#ifdef OI_CODE
#error code not present
#else
            connection->CB.put.confirm(link, NULL, status);
#endif
            break;
      case OBEX_SETTING_PATH:
#ifdef OI_CODE
#error code not present
#else
            connection->CB.setpath.confirm(link, NULL, status);
#endif
            break;
        case OBEX_ABORTING:
            OI_DBGPRINT2(("FreeConnection while aborting"));
            switch (connection->abort) {
                case ABORT_GET:
                    connection->abort = NO_ABORT;
                    connection->CB.get.recvData(link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
                    break;
                case ABORT_PUT:
                    connection->abort = NO_ABORT;
                    connection->CB.put.confirm(link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
                    break;
                case GET_ABORTED:
                case PUT_ABORTED:
                    connection->abort = NO_ABORT;
                    if (connection->CB.abort.confirm) {
                        connection->CB.abort.confirm(connection->common.link);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    if (connection->disconnectInd != NULL) {
#ifdef OI_CODE
#error code not present
#else
        if ((state == OBEX_DISCONNECTING) && (connection->disconnectHdrCount > 0)) {
            hdrs.list = connection->disconnectHdrs;
            hdrs.count = connection->disconnectHdrCount;
            connection->disconnectInd(link, &hdrs);
        } 
        else {
            connection->disconnectInd(link, NULL);
        }        
        connection->disconnectHdrCount = 0;
#endif
    }

    OI_FreeIf(&connection->common.authentication);
    OI_FreeIf(&connection->common.rcvBuffer);

    OI_Free(connection);

#ifndef OI_CODE
    /* clear connection ptr in BT PF SIO table */
    (void) OI_RFCOMM_SetLinkContext(connection->common.link, NULL);
#endif 
    OI_INIT_FLAG_DECREMENT(OBEX_CLI);
}


/**
 * OI_RFCOMM_DISCONNECT_IND
 */

static void RfcommDisconnectInd(OI_RFCOMM_LINK_HANDLE link,
                                OI_STATUS status)
{
    OBEXCLI_CONNECTION *connection = LookupConnection(link);

    OI_DBGPRINT(("RfcommDisconnectInd [%d] %!", link, status));

#ifndef OI_CODE
    if((connection != NULL) && (connection->disconStatus != OI_OK))
    {
      status = connection->disconStatus;
    }    
#endif

    if (connection != NULL) {
        /*
         * If the RFCOMM connection was dropped by the server while we have an
         * operation in progress we need to report a generic error status.
         */
        if (OI_SUCCESS(status) && (connection->state >= OBEX_CONNECTED)) {
            status = OI_OBEX_NOT_CONNECTED;
        }
        FreeConnection(link, connection, status);
    }
}


/*
 * Handle a fatal error - drop the RFCOMM connection, cleanup, and try to let
 * the application know what happened.
 */

static void ConnectionError(OBEXCLI_CONNECTION *connection,
                            OI_STATUS status)
{
    OI_STATUS tmpStatus;
#ifdef OI_CODE
#error code not present
#else
    connection->disconStatus = status;
    BT_ERR("OBEX client error! status = 0x%x", status, 0, 0);
#endif
    connection->common.rcvState = OI_OBEX_RCV_BUF_EMPTY;
    connection->abort = NO_ABORT;

    /*
     * If there is an RFCOMM connection, disconnect.
     */
#ifdef OI_CODE
#error code not present
#else
    if ((connection->state >= RFCOMM_CONNECTED) || 
        (connection->state >= RFCOMM_CONNECTING && connection->cancelConnect == TRUE)) {
        connection->cancelConnect = FALSE;
#endif 
        connection->droppingLink = TRUE;
        tmpStatus = OI_RFCOMM_Disconnect(connection->common.link);
        if (OI_SUCCESS(tmpStatus)) {
            /*
             * Remaining cleanup will be be performed by RfcommDisconnectInd()
             * when the underlying RFCOMM connection is dropped.
             */
            return;
        }
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("RFCOMM disconnect failed "));
#endif
    }
    /*
     * Free resources allocated for the connection.
     */
    FreeConnection(connection->common.link, connection, status);
}


/*
 * Dispatch function for asynchronously reporting an aborted commmand.
 */

static void CommandError(DISPATCH_ARG *darg)
{
    OBEXCLI_ERROR_CONTEXT ctxt;

    ctxt = Dispatch_GetArg(darg, OBEXCLI_ERROR_CONTEXT);
    ConnectionError(ctxt.connection, ctxt.status);
}


static void ReportError(OBEXCLI_CONNECTION *connection,
                        OI_STATUS status)
{
    OBEXCLI_ERROR_CONTEXT ctxt;
    DISPATCH_ARG darg;

    ctxt.connection = connection;
    ctxt.status = status;
    Dispatch_SetArg(darg, ctxt);
    OI_Dispatch_RegisterFunc(CommandError, &darg, NULL);
}


/**
 * Timeout an OBEX operation.
 */

static void ResponseTimeout(DISPATCH_ARG *darg)
{
    OBEXCLI_CONNECTION *connection = Dispatch_GetArg(darg, OBEXCLI_CONNECTION*);
    ConnectionError(connection, OI_OBEX_CONNECTION_TIMEOUT);
}


/**
 * When we start to send an obex command we expect to receive a response within
 * some reasonable timeout period. 
 */

static void SetCommandTimeout(OBEXCLI_CONNECTION *connection,
                              OI_INTERVAL timeout)
{
    DISPATCH_ARG darg;

    if (OI_Dispatch_IsValidHandle(connection->timeoutCB)) {
        /*
         * Timeout has already been registered - just extend the timeout period.
         */
        OI_Dispatch_SetFuncTimeout(connection->timeoutCB, timeout);
    } else {
        /*
         * Register a new timeout.
         */
        Dispatch_SetArg(darg, connection);
        OI_Dispatch_RegisterTimedFunc(ResponseTimeout, &darg, timeout, &connection->timeoutCB);
    }
}


/**
 * Map an OBEX response to an OI_STATUS
 */

#ifdef OI_CODE
#error code not present
#else
OI_STATUS MapResponse(OI_UINT8 rspCode,
#endif
                             OI_STATUS status)
{
    switch (rspCode) {
        case OI_OBEX_FINAL(OI_OBEX_RSP_OK):
            status = OI_OK;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE):
            status = OI_OBEX_CONTINUE;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_BAD_REQUEST):
            status = OI_OBEX_BAD_REQUEST;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_FORBIDDEN):
            status = OI_OBEX_ACCESS_DENIED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_NOT_FOUND):
            status = OI_OBEX_NOT_FOUND;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_NOT_IMPLEMENTED):
            status = OI_OBEX_UNKNOWN_COMMAND;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_LENGTH_REQUIRED):
            status = OI_OBEX_LENGTH_REQUIRED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_SERVICE_UNAVAILABLE):
            status = OI_OBEX_SERVICE_UNAVAILABLE;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_NOT_ACCEPTABLE):
            status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_PRECONDITION_FAILED):
            status = OI_OBEX_PRECONDITION_FAILED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_DATABASE_FULL):
            status = OI_OBEX_DATABASE_FULL;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_DATABASE_LOCKED):
            status = OI_OBEX_DATABASE_LOCKED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_INTERNAL_SERVER_ERROR):
            status = OI_OBEX_INTERNAL_SERVER_ERROR;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_UNSUPPORTED_MEDIA_TYPE):
            status = OI_OBEX_UNSUPPORTED_MEDIA_TYPE;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_UNAUTHORIZED):
            status = OI_OBEX_UNAUTHORIZED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_PARTIAL_CONTENT):
            status = OI_OBEX_PARTIAL_CONTENT;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_METHOD_NOT_ALLOWED):
            status = OI_OBEX_METHOD_NOT_ALLOWED;
            break;
#ifndef OI_CODE
        case OI_OBEX_FINAL(OI_OBEX_RSP_NOT_MODIFIED):
            status = OI_OBEX_NOT_MODIFIED;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_GONE):
            status = OI_OBEX_GONE;
            break;
        case OI_OBEX_FINAL(OI_OBEX_RSP_REQUEST_TIME_OUT):
            status = OI_OBEX_REQ_TIMEOUT;
            break;
#endif
    }

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX : Map Response : ResponseCode=%x : Status = %x",rspCode,status,0);
#endif

    return status;
}


static void WriteCfm(OI_RFCOMM_LINK_HANDLE link,
                     OI_MBUF *mbuf,
                     OI_STATUS result)
{
    OBEXCLI_CONNECTION *connection = LookupConnection(link);

    if (connection == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Write confirmation for unknown link"));
#endif
        return;
    }

        connection->common.mbuf = NULL;
        OI_DBGPRINT2(("Write completed: freeing mbuf %#x", mbuf));
        OI_MBUF_Free(mbuf);

    if (OI_SUCCESS(result)) {
        /*
         * Check if there is a response waiting to be handled.
         */
        if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) {
            OI_DBGPRINT2(("OBEX client handling postponed response"));
            HandleResponsePkt(connection);
        }
    } else {
        ConnectionError(connection, result);
        }
    }


static OI_STATUS ClientSendPacket(OBEXCLI_CONNECTION *connection,
                                  OI_INTERVAL timeout)
{
    OI_STATUS status;

    OI_DBGPRINT2(("Sending mbuf %#x", connection->common.mbuf));

    status = OI_RFCOMM_WriteMBUF(WriteCfm, connection->common.link, connection->common.mbuf);
    if (OI_SUCCESS(status)) {
        SetCommandTimeout(connection, timeout);
    } else {
        OI_MBUF_Free(connection->common.mbuf);
        connection->common.mbuf = NULL;
    }
    return status;
}


static OI_STATUS ClientSendBodySegment(OBEXCLI_CONNECTION *connection)
{
    OI_STATUS status;

    OI_DBGPRINT(("ClientSendBodySegment"));

    status = OI_OBEXCOMMON_MarshalBodySegment(&connection->common);
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, connection->responseTimeout);
    }
    return status;
}


/*
 * Send an parameterless OBEX command.
 */
static OI_STATUS ClientSendCommand(OBEXCLI_CONNECTION *connection,
                                   OI_UINT8 opcode,
#ifndef OI_CODE
                                   OI_OBEX_HEADER_LIST const *optionalHdrs,
#endif
                                   OI_INTERVAL timeout)
{
    OI_STATUS status;
    OI_OBEX_HEADER headers[1];
    OI_UINT16 headerCount = 0;
    OI_BYTE_STREAM pkt;

    OI_ASSERT(OI_OBEX_IS_FINAL(opcode));

    OI_OBEXCOMMON_InitPacket(&connection->common, opcode, &pkt);
    /*
     * If a target was specified at connect time we must send the connection id
     * in the command
     */
    if (connection->cid != OI_OBEX_INVALID_CONNECTION_ID) {
        OI_OBEX_HEADER *hdr = &headers[headerCount++];
        hdr->id = OI_OBEX_HDR_CONNECTION_ID;
        hdr->val.connectionId = connection->cid;
    }
#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, optionalHdrs);
#endif
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, timeout);
    }
    return status;
}


/**
 * Handle response to an OBEX disconnect request.
 */

static void DisconnectResponse(OBEXCLI_CONNECTION *connection,
                               OI_UINT8 rspCode)
{
    OI_STATUS status;
#ifndef OI_CODE
    OI_OBEX_HEADER_LIST hdrs;
#endif

    status = MapResponse(rspCode, OI_OBEX_DISCONNECT_FAILED);
    if (!OI_SUCCESS(status)) {
        ConnectionError(connection, status);
    } else {
#ifndef OI_CODE
        /* 
         * Save optional headers 
         */
        status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
        if (OI_SUCCESS(status)) {
            /*
             * Reuse the connect header area to store these headers 
             */
            if (hdrs.count > OI_OBEX_MAX_CONNECT_HDRS)
            { 
                /*
                 * This should never happen. Checking just to be safe.
                 */ 
                hdrs.count = OI_OBEX_MAX_CONNECT_HDRS;
            }

            if ( hdrs.list ) {
                OI_MemCopy(connection->disconnectHdrs, hdrs.list, sizeof(OI_OBEX_HEADER) * hdrs.count);
                connection->disconnectHdrCount = hdrs.count;
                OI_FreeIf(&hdrs.list);
            }
        }
#endif
        /*
         * Terminate the underlying RFCOMM connection
         */
        connection->droppingLink = TRUE;
        status = OI_RFCOMM_Disconnect(connection->common.link);
        if (!OI_SUCCESS(status)) {
            ConnectionError(connection, OI_OBEX_DISCONNECT_FAILED);
        }
    }
}


static void SendAbort(OBEXCLI_CONNECTION *connection)
{
    OI_STATUS status;

    OI_DBGPRINT(("Sending abort command"));
#ifdef OI_CODE
#error code not present
#else
    status = ClientSendCommand(connection, OI_OBEX_CMD_ABORT, NULL, OBEX_ABORT_TIMEOUT);
#endif
    if (OI_SUCCESS(status)) {
        connection->state = OBEX_ABORTING;
    } else {
        /*
         * If we could not send the abort command something has gone badly
         * wrong so we need to attempt to disconnect.
         */
        OI_LOG_ERROR(("Could not send abort command"));
        ConnectionError(connection, OI_OBEX_ERROR);
    }
}


/**
 * Called by the application to put data. 
 */

OI_STATUS OI_OBEXCLI_Put(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST const *cmdHeaders,
                         OI_OBEXCLI_PUT_CFM putCfm,
                         OI_STATUS status)
{
    OI_OBEX_HEADER headers[1];
    OI_UINT16 headerCount = 0;
    OI_BYTE_STREAM pkt;
    OI_OBEX_HEADER *hdr;
    OBEXCLI_CONNECTION *connection;
    OI_UINT8 cmd = (status == OI_OBEX_CONTINUE) ? OI_OBEX_CMD_PUT : OI_OBEX_FINAL(OI_OBEX_CMD_PUT);

    OI_DBGTRACE(("OI_OBEXCLI_Put (connectionId = %d, <*cmdHeaders = %x>, <*putCfm = %x>, status = %!)\n",
                 connectionId, cmdHeaders, putCfm, status));

    ARGCHECK(putCfm != NULL);

    /*
     * Must be connected to an OBEX server.
     */
    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        return OI_OBEX_NOT_CONNECTED;
    }

#ifndef OI_CODE
    /* 
     * This command is called immediately after OI_OBEXCLI_AbortOperation 
     * is called to force an immediate abort of the put operation. In this case, 
     * we need to cancel the timeout first.
     */
    if ((connection->abort == ABORT_PUT) && (connection->timeoutCB)) {
      OI_Dispatch_CancelFunc(connection->timeoutCB); 
      connection->timeoutCB = 0;
    }
#endif /* OI_CODE */
    if (BUSY(connection)) {
#ifdef OI_CODE
#error code not present
#else
        return OI_OBEX_OPERATION_IN_PROGRESS;
#endif
    }
    /*
     * Only one operation at a time on each connection.
     */
    if ((connection->state != OBEX_CONNECTED) && (connection->state != OBEX_PUTTING)) {
        OI_DBGPRINT(("OBEX client PUT: another operation is in progress"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * Save callback function.
     */
    connection->CB.put.confirm = putCfm;
    /*
     * Check if we need to send an ABORT instead of a PUT
     * Note - this code path is only executed if the deprecated abort API was
     * called.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->abort == ABORT_PUT || connection->abortCurOp) {
        if(connection->abortCurOp)
        {
            connection->abort = ABORT_PUT;
        }
#endif
        SendAbort(connection);
        /*
         * We return OI_OK because the putCfm callback will be called when the
         * abort completes.
         */
        return OI_OK;
    }

    OI_DBGPRINT(("OBEX client issuing PUT request"));

    /*
     * The only way to terminate a PUT is to send an abort so if the application
     * called us with an error status we need to abort the put.
     */
    if (!OI_SUCCESS(status) && (status != OI_OBEX_CONTINUE)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX client terminating PUT operation with error"));
#endif
        connection->abort = ABORT_PUT;
        SendAbort(connection);
        return OI_OK;
    }

    if (!cmdHeaders) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    OI_DBGPRINT2(("OBEX client issuing %s PUT request", OI_SUCCESS(status) ? "final" : "continue"));

    OI_OBEXCOMMON_InitPacket(&connection->common, cmd, &pkt);

    /*
     * If a target was specified at connect time we must send the connection id
     * in the first PUT packet.
     */
    if (connection->state == OBEX_CONNECTED) {
        if (connection->cid != OI_OBEX_INVALID_CONNECTION_ID) {
            hdr = &headers[headerCount++];
            hdr->id = OI_OBEX_HDR_CONNECTION_ID;
            hdr->val.connectionId = connection->cid;
        }
        connection->state = OBEX_PUTTING;
    }
    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, cmdHeaders);
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, connection->responseTimeout);
    }
    if (!OI_SUCCESS(status)) {
        goto PutReqError;
    }

    return OI_OK;

PutReqError:
#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEXCLI_PutReq"));
#endif
    connection->state = OBEX_CONNECTED;

    return status;
}


/**
 * Request data from an OBEX server.
 */

OI_STATUS OI_OBEXCLI_Get(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                         OI_OBEX_HEADER_LIST const *cmdHeaders,
                         OI_OBEXCLI_GET_RECV_DATA getRecvData,
                         OI_BOOL final)
{
    OI_OBEX_HEADER headers[1];
    OI_UINT16 headerCount = 0;
    OI_UINT8 cmd = (final) ? OI_OBEX_FINAL(OI_OBEX_CMD_GET) : OI_OBEX_CMD_GET;
    OI_OBEX_HEADER *hdr;
    OI_BYTE_STREAM pkt;
    OI_STATUS status;
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_Get (connectionId = %d, <*cmdHeaders = %x>, <*getRecvData = %x>, final = %s)\n",
                 connectionId, cmdHeaders, getRecvData, final ? "TRUE" : "FALSE"));

    ARGCHECK(getRecvData != NULL);

    /*
     * Must be connected to an OBEX server.
     */
    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        return OI_OBEX_NOT_CONNECTED;

    }

#ifndef OI_CODE
    /* 
     * This command is called immediately after OI_OBEXCLI_AbortOperation 
     * is called to force an immediate abort of the put operation. In this case, 
     * we need to cancel the timeout first.
     */
    if ((connection->abort == ABORT_GET) && (connection->timeoutCB)) {
      OI_Dispatch_CancelFunc(connection->timeoutCB); 
      connection->timeoutCB = 0;
    }
#endif /* OI_CODE */

    if (BUSY(connection)) {
#ifdef OI_CODE
#error code not present
#else
            return OI_OBEX_OPERATION_IN_PROGRESS;
#endif
    }
    /*
     * Check we are in the correct state for a get request.
     */
    if ((connection->state != OBEX_CONNECTED) && (connection->state != OBEX_GETTING)) {
        OI_DBGPRINT(("OBEX client GET: another operation is in progress"));
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * Save the callback function.
     */
    connection->CB.get.recvData = getRecvData;
    /*
     * Check if we need to send an ABORT instead of a GET
     * Note - this code path is only executed if the deprecated abort API was
     * called.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->abort == ABORT_GET || connection->abortCurOp) {
        if(connection->abortCurOp)
        {
            connection->abort = ABORT_GET;
        }
#endif
        SendAbort(connection);
        /*
         * Return OI_OK because the getRecvData callback will be called when the
         * abort completes.
         */
        return OI_OK;
    }

    OI_DBGPRINT(("OBEX client issuing GET request"));

    OI_OBEXCOMMON_InitPacket(&connection->common, cmd, &pkt);
    /*
     * If a target was specified at connect time we must send the connection id
     * in the first get packet.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->state == OBEX_CONNECTED || connection->state == OBEX_GETTING) {
#endif
        if (connection->cid != OI_OBEX_INVALID_CONNECTION_ID) {
            hdr = &headers[headerCount++];
            hdr->id = OI_OBEX_HDR_CONNECTION_ID;
            hdr->val.connectionId = connection->cid;
        }
    connection->state = OBEX_GETTING;
    }

    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, cmdHeaders);
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, connection->responseTimeout);
    }
    if (!OI_SUCCESS(status)) {
        goto GetReqError;
    }

    return OI_OK;

GetReqError:
#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEXCLI_GetReq"));
#endif
    connection->state = OBEX_CONNECTED;

    return status;

}


/**
 * Handle response to an OBEX setpath command
 */

static void SetpathResponse(OBEXCLI_CONNECTION *connection,
                            OI_UINT8 rspCode)
{
    OI_STATUS status;

#ifndef OI_CODE    
    OI_OBEX_HEADER_LIST hdrs;
    OI_OBEX_HEADER_LIST *hdrsPtr = NULL;

    /* Process optional headers */
    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (OI_SUCCESS(status)) {
        hdrsPtr = &hdrs;
    }
#endif

    OI_DBGPRINT(("OBEX client response to SETPATH %d", rspCode));

    /*
     * For interop - some non-compliant implementations return a CREATED
     * response instead of an OK response if a new folder was created.
     */
    if (rspCode == OI_OBEX_FINAL(OI_OBEX_RSP_CREATED)) {
        rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_OK);
    }

    status = MapResponse(rspCode, OI_OBEX_ERROR);
    if (status == OI_OBEX_NOT_FOUND) {
        /*
         * Refine error message.
         */
        status = OI_OBEX_NO_SUCH_FOLDER;
    }

#ifndef OI_CODE
    /* If the abort has been issued form the app, then 
       it doesn't matter if the setpath is successfull or not 
       change the status to ABORTED and pass the status to APP */
    if(connection->abortCurOp)
    {
        connection->abortCurOp = 0;
        status = OI_OBEX_CLIENT_ABORTED_COMMAND;
    }
#endif
    connection->state = OBEX_CONNECTED;
    /*
     * Confirm to the application that the path has been set or report an error.
     */
#ifdef OI_CODE
#error code not present
#else
    connection->CB.setpath.confirm(connection->common.link, hdrsPtr, status);
    if (hdrsPtr != NULL) {
      OI_FreeIf(&hdrs.list);
    }
#endif
}

/**
 * Handle response to an OBEX put request.
 */

static void PutResponse(OBEXCLI_CONNECTION *connection,
                        OI_UINT8 rspCode)
{
    OI_OBEX_HEADER_LIST hdrs;
    OI_STATUS status;

    OI_DBGPRINT(("OBEX client response to PUT request %d", rspCode));

    /*
     * Map the OBEX response into a OI_STATUS code.
     */
    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (OI_SUCCESS(status)) {
        status = MapResponse(rspCode, OI_OBEX_PUT_RESPONSE_ERROR);
    }
    /*
     * No longer putting if the status is not CONTINUE.
     */
    if (status != OI_OBEX_CONTINUE) {
        connection->state = OBEX_CONNECTED;
        connection->abort = NO_ABORT;
#ifndef OI_CODE
        if(connection->abortCurOp)
        {
            connection->abortCurOp = 0;
        }
#endif
    }
    /*
     * Check if we have an abort pending.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->abort == ABORT_PUT || connection->abortCurOp) {
        if(connection->abortCurOp)
        {
          connection->abort = ABORT_PUT;
        }
#endif
        SendAbort(connection);
        return;
    }
    /*
     * Keep sending body segments if there are more to send.
     */
    if ((connection->state == OBEX_PUTTING) && OI_OBEX_IS_A_BODY_HEADER(connection->common.bodySegment.id)) {
        status = ClientSendBodySegment(connection);
        if (OI_SUCCESS(status)) {
            OI_FreeIf(&hdrs.list);
            return;
        }
    }
    connection->CB.put.confirm(connection->common.link, &hdrs, status);
    OI_FreeIf(&hdrs.list);
}


/**
 * Handle response to an OBEX get request.
 */

static void GetResponse(OBEXCLI_CONNECTION *connection,
                        OI_UINT8 rspCode)
{
    OI_OBEX_HEADER_LIST hdrs;
    OI_STATUS status;

    OI_DBGPRINT(("OBEX client response to GET request %d", rspCode));

    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (OI_SUCCESS(status)) {
        status = MapResponse(rspCode, OI_OBEX_GET_RESPONSE_ERROR);
    }
    /*
     * No longer getting if the status is not CONTINUE.
     */
    if (status != OI_OBEX_CONTINUE) {
        connection->state = OBEX_CONNECTED;
        connection->abort = NO_ABORT;
#ifndef OI_CODE
        if(connection->abortCurOp)
        {
            connection->abortCurOp = 0;
        }
#endif
    }
    /*
     * Check if we have an abort pending.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->abort == ABORT_GET || connection->abortCurOp) {
        if(connection->abortCurOp)
        {
          connection->abort = ABORT_GET;
        }
#endif
        OI_FreeIf(&hdrs.list);
        SendAbort(connection);
        return;
    }
    /*
     * Keep sending body segments if there are more to send.
     */
    if ((connection->state == OBEX_GETTING) && OI_OBEX_IS_A_BODY_HEADER(connection->common.bodySegment.id)) {
        status = ClientSendBodySegment(connection);
        if (OI_SUCCESS(status)) {
            OI_FreeIf(&hdrs.list);
            return;
        }
    }
    connection->CB.get.recvData(connection->common.link, &hdrs, status);
    OI_FreeIf(&hdrs.list);
}



/**
 * Handle response to an OBEX connect request.
 */

static void ConnectResponse(OBEXCLI_CONNECTION *connection,
                            OI_UINT8 rspCode)
{
    OI_OBEX_HEADER_LIST hdrs = { 0, 0 };
    OI_OBEX_HEADER *connectionId;
    OI_OBEX_HEADER *authChallenge;
    OI_OBEX_HEADER *authResponse;
    OI_BOOL notAuthorized = FALSE;
    OI_STATUS status;
    OI_UINT8 version = 0;
#ifndef OI_CODE
    OI_OBEX_HEADER_LIST optionalHdrs = { 0, NULL };
    OI_UINT8 i;
    OI_UINT8 skipHdr = 0;
    OI_BOOL fullAccess;
#endif

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Client connect Response", 0, 0, 0 );
#endif

    OI_DBGPRINT(("OBEX client connect response"));

    status = MapResponse(rspCode, OI_OBEX_CONNECT_FAILED);
    if (!OI_SUCCESS(status)) {
        /*
         * Server responds with NOT_AUTHORIZED if authentication is required.
         */
        if (status == OI_OBEX_UNAUTHORIZED) { 
            OI_DBGPRINT(("OBEX Client authentication requested"));
            notAuthorized = TRUE;
        } else {
            OI_DBGPRINT(("OBEX Client connection rejected %!", status));
            goto ConnectFailed;
        }
    }

    ByteStream_GetUINT8_Checked(connection->common.rcvPacket, version);
    /*
     * Due to a misreading on the specification some implementations (including
     * earlier versions of BM3) send the OBEX spec version number instead of the
     * OBEX protocol version number. For interoperability we will only check the
     * major version number.
     */
    if (OI_OBEX_MAJOR_VERSION_NUMBER(version) != OI_OBEX_MAJOR_VERSION) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX Client connect - version mismatch"));
#endif
        status = OI_OBEX_UNSUPPORTED_VERSION;
        goto ConnectFailed;
    }
    OI_DBGPRINT2(("OBEX Client connect - version %d.%d", OI_OBEX_MAJOR_VERSION_NUMBER(version), OI_OBEX_MINOR_VERSION_NUMBER(version)));

    /* We don't currently use flags -- Skipping*/
    /* ByteStream_GetUINT8_Checked(connection->common.rcvPacket, flags); */
    ByteStream_Skip(connection->common.rcvPacket, 1);

    
    ByteStream_GetUINT16_Checked(connection->common.rcvPacket, connection->common.maxSendPktLen, OI_OBEX_BO);

    if (ByteStream_Error(connection->common.rcvPacket)) {
        status = OI_OBEX_INCOMPLETE_PACKET;
        goto ConnectFailed;
    }

    OI_DBGPRINT2(("OBEX Client connect - requested packet len = %d", connection->common.maxSendPktLen));

    /*
     * The OBEX specification defines a minimum packet length. Reject
     * connections that do not conform to the specification.
     */
    if (connection->common.maxSendPktLen < OI_OBEX_MIN_PACKET_SIZE) {
        status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
        goto ConnectFailed;
    }

    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (!OI_SUCCESS(status)) {
        goto ConnectFailed;
    }

    /*
     * If the server returned an unauthorized response we cannot complete the
     * connection at this time. Instead we call back the application to request
     * a password.
     */
    if (notAuthorized) {
#ifndef OI_CODE
        BT_MSG_DEBUG("OBEX client : Server returns with Not Authorized",0,0,0);
    /*
     * For IOBEX APP, mutual authentication to be done  only if the client challenge server
     * and server challenge client.     
     */
        if(connection->authResponseCB != NULL) {
            if(connection->cliAuthInit == TRUE) {
                /* Client initiated authentication, Server respond with unauthorized, and sends challenge...
                * Mutual Authentication 
                */
                BT_MSG_DEBUG("OBEX client : Mutual Authentication...",0,0,0);
                connection->common.mutualAuth = TRUE ;
            }
        } else {
            if(connection->cliAuthInit == TRUE) {
                /* Client initiated authentication, Server respond with unauth-
                 * orized, and sends challenge... Mutual Authentication 
                */
                BT_MSG_DEBUG("OBEX client : Mutual Authentication, authResponseCB = NULL",0,0,0);
                connection->common.mutualAuth = TRUE ;
            }
        }
#endif /* OI_CODE */

        connection->common.authenticating = TRUE;

        authChallenge = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_CHALLENGE);
        /*
         * If the server returned an unauthorized response but did not return an
         * authentication challenge, something is broken with the server. 
         */
        if (authChallenge == NULL) {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
            goto ConnectFailed;
        }

#ifndef OI_CODE
        skipHdr++;
#endif
        OI_DBGPRINT(("OBEX client received authentication challenge"));
        if (connection->common.authentication == NULL) {
            /*
             * If the application is not allowing authentication we
             * have to reject the connnection.
             */
            status = OI_OBEX_NOT_IMPLEMENTED;
            goto ConnectFailed;
        }
        /*
         * Save the challenge digest and request a password from the
         * application.
         */
        status = OI_OBEXAUTH_SaveChallenge(&connection->common, authChallenge);
#ifndef OI_CODE
        if (status == OI_OBEX_BAD_PACKET) {
            BT_MSG_DEBUG("OBEX client : Bad packet", 0, 0, 0);
            goto ConnectFailed;
        }
        fullAccess = !(connection->common.readOnly);
        BT_MSG_DEBUG("OBEX client : Challenge call back...UIDReqd:%x FullAccess:%x",
                                connection->common.authentication->userIdRequired,
                                fullAccess,0);
#endif
        connection->challengeCB(connection->common.link, 
#ifndef OI_CODE
                                fullAccess,
#endif
                                connection->common.authentication->userIdRequired);
        /*
         * We will be resending the connection request so we are done with the
         * connection response.
         */
        OI_Free(hdrs.list);
        return;
    }
    /*
     * Get the connection id if there was one.
     */
    connectionId = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_CONNECTION_ID);
    if (connectionId != NULL) {
        connection->cid = connectionId->val.connectionId;
#ifndef OI_CODE
        skipHdr++;
#endif
    }
    

#ifndef OI_CODE
    /*
    Auth Response Indication to app for the Client initiated authentication.
    This is Required for non BT APP to differntiate Client initiated auth and
    Server initiated auth.
    */
    if((connection->cliAuthInit == TRUE) && (connection->common.mutualAuth != TRUE)){
        authResponse = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_RESPONSE);
        if (authResponse == NULL) {
            OI_LOG_ERROR(("BT OBEX: Nof found Auth Response header"));
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
            goto ConnectFailed;
        }
        OI_OBEXAUTH_SaveResponse(&connection->common, authResponse);
        /*
        Client initiated challenge, send auth challenge response iddication event to app.
        */
        if(connection->authResponseCB != NULL){
            /*
            Client initiated auth, call authResponseCB if app registers 
            auth response CB indication
            */
           BT_MSG_DEBUG( "BT OBEX: Self Initiated Auth challenge response CB Indication", 0, 0, 0 );
           connection->authResponseCB(connection->common.link);
        } else {
            /*
            Client initiated auth, call authChallengeCB if app don't register
            auth response CB indication
            */
            BT_MSG_DEBUG( "BT OBEX: Self Initiated Auth:Challenge CB Indication", 0, 0, 0 );
            connection->challengeCB(connection->common.link, 
                                    !connection->common.readOnly,
                                    connection->common.authentication->userIdRequired);
        }
        OI_Free(hdrs.list);
        return;
    }
    #endif
    /*
     * If this connection is being authenticated we should have got an
     * authentication response from the server.
     */
#ifdef OI_CODE
#error code not present
#else
    if((connection->common.mutualAuth == TRUE)) {
#endif
        authResponse = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_RESPONSE);
        if (authResponse == NULL) {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
            goto ConnectFailed;
        }
        OI_OBEXAUTH_SaveResponse(&connection->common, authResponse);
        status = OI_OBEXAUTH_Authenticate(&connection->common);
        if (!OI_SUCCESS(status)) {
            goto ConnectFailed;
        }
    }

#ifndef OI_CODE
    /*
     * Extract the remaining headers and send them to the application
     */
    if (hdrs.count > skipHdr)
    {
      optionalHdrs.list = OI_Malloc(sizeof(OI_OBEX_HEADER) * (hdrs.count - skipHdr));
      for (i = 0; i < hdrs.count; i++) {
          if ((hdrs.list[i].id != OI_OBEX_HDR_CONNECTION_ID) && 
              (hdrs.list[i].id != OI_OBEX_HDR_AUTHENTICATION_RESPONSE)) {              
              optionalHdrs.list[optionalHdrs.count++] = hdrs.list[i];
          }
      }
    }
#endif
    /*
     * Let the application know the connection has been established.
     */

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX:Conn CFM Ind CB : Link=%d :Readonly=%d",connection->common.link,connection->common.readOnly,0);
#else 
#error code not present
#endif
    connection->state = OBEX_CONNECTED;
#ifdef OI_CODE
#error code not present
#else
    if (optionalHdrs.count == 0) {
        connection->CB.connect.confirm(connection->common.link, connection->common.readOnly, NULL, OI_OK);
    }
    else {
        connection->CB.connect.confirm(connection->common.link, connection->common.readOnly, &optionalHdrs, OI_OK);
        OI_FreeIf(&optionalHdrs.list);
    }
#endif
    /*
     * All done with the received headers.
     */
    OI_FreeIf(&hdrs.list);
    return;

ConnectFailed:

    OI_FreeIf(&hdrs.list);
#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEX Client connection failed"));
    BT_MSG_HIGH("OBEX Client connection failed STATUS=%x",status,0,0);
#endif 
 
    /*
     * Terminate the RFCOMM connection
     */
    ConnectionError(connection, status);
}


static OI_STATUS SendConnectRequest(OBEXCLI_CONNECTION *connection,
                                    OI_INTERVAL timeout)
{
    OI_OBEX_HEADER_LIST cmdHdrs;
    OI_OBEX_HEADER headers[2];
    OI_UINT16 headerCount = 0;
    OI_BYTE_STREAM pkt;
    OI_STATUS status;

    OI_ASSERT(connection->state == RFCOMM_CONNECTED);

    OI_OBEXCOMMON_InitPacket(&connection->common, OI_OBEX_CMD_CONNECT, &pkt);
    /*
     * Write the command fields.
     */
    ByteStream_PutUINT8(pkt, OI_OBEX_VERSION_NUMBER);
    ByteStream_PutUINT8(pkt, OI_OBEX_CONNECT_FLAGS);
    ByteStream_PutUINT16(pkt, connection->common.maxRecvPktLen, OI_OBEX_BO);

#ifdef OI_CODE
#error code not present
#else
    BT_MSG_DEBUG("BT OBEX Client SendConnectRequest OBEX Ver=%2x: ConnFlags =%2x",
                    OI_OBEX_VERSION_NUMBER, OI_OBEX_CONNECT_FLAGS, 0);
#endif

    /*
     * The Bluetooth GOEP specification is very vague about OBEX authentication
     * it seems to imply that authentication is always mutual and is always
     * initiated by the server. So if we are authenticating it means we are
     * doing so in response to a challenge from the server.
     */
    if (connection->common.authenticating) {
#ifndef OI_CODE
        if(connection->common.mutualAuth){
#endif
            /*
             * Generate the client's challenge header.
             */
            OI_OBEXAUTH_ComposeChallenge(OI_CONFIG_TABLE_GET(OBEX_CLI)->privateKey, &connection->common, &headers[headerCount++]);
            /*
             * Respond to the server's challenge.
             */
            OI_OBEXAUTH_ComposeResponse(&connection->common, &headers[headerCount++]);
        }
#ifndef OI_CODE
        else if(connection->cliAuthInit){
            /*
             * Generate the client's challenge header.
             */
            OI_OBEXAUTH_ComposeChallenge(OI_CONFIG_TABLE_GET(OBEX_CLI)->privateKey, &connection->common, &headers[headerCount++]);
        }else {
             /*
             * Respond to the server's challenge.
             */
            OI_OBEXAUTH_ComposeResponse(&connection->common, &headers[headerCount++]);
        }
#endif
    }


    cmdHdrs.list = connection->connectHdrs;
    cmdHdrs.count = connection->connectHdrCount;

    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, &cmdHdrs);
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, timeout);
    }
    if (!OI_SUCCESS(status)) {
        goto ConnectReqError;
    }

    return OI_OK;

ConnectReqError:

    return status;
}

#ifndef OI_CODE
/*
Response to an indication for client initiated authentication.
*/
static OI_STATUS OBEXAuthResponse(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OBEXCLI_CONNECTION *connection;
    OI_OBEX_HEADER_LIST hdrs = { 0, NULL };
    OI_STATUS status ;
    OI_OBEX_HEADER_LIST optionalHdrs = { 0, NULL };
    OI_UINT8 i;
    OI_UINT8 skipHdr = 2; //Need to change it. TBD

    BT_MSG_DEBUG( "BT OBEX: Client initiated challenge...Auth Response", 0, 0, 0 );

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (!OI_SUCCESS(status)) {
        goto ConnectFailed;
    }

    /*
     * Extract the remaining headers and send them to the application
     */
    if (hdrs.count > skipHdr)
    {
      optionalHdrs.list = OI_Malloc(sizeof(OI_OBEX_HEADER) * (hdrs.count - skipHdr));
      for (i = 0; i < hdrs.count; i++) {
          if ((hdrs.list[i].id != OI_OBEX_HDR_CONNECTION_ID) && 
              (hdrs.list[i].id != OI_OBEX_HDR_AUTHENTICATION_RESPONSE)) {              
              optionalHdrs.list[optionalHdrs.count++] = hdrs.list[i];
          }
      }
    }
    status = OI_OBEXAUTH_Authenticate(&connection->common);
    if (!OI_SUCCESS(status)) {
            goto ConnectFailed;
     }
      /*
     * Let the application know the connection has been established.
     */

    BT_MSG_DEBUG("BT OBEX: OBEX client connection confirmed", 0, 0, 0 );
    OI_DBGPRINT(("OBEX client connection confirmed"));
    connection->state = OBEX_CONNECTED;
    if (optionalHdrs.count == 0) {
        connection->CB.connect.confirm(connection->common.link, connection->common.readOnly, NULL, OI_OK);
    }
    else {
        connection->CB.connect.confirm(connection->common.link, connection->common.readOnly, &optionalHdrs, OI_OK);
        OI_FreeIf(&optionalHdrs.list);
    }

    /*
     * All done with the received headers.
     */
    OI_FreeIf(&hdrs.list);

    return OI_OK;

ConnectFailed:

    OI_FreeIf(&hdrs.list);
    BT_MSG_HIGH("OBEX Client connection failed",0,0,0);
    OI_DBGPRINT(("OBEX Client connection failed"));
    /*
     * Terminate the RFCOMM connection
     */
    ConnectionError(connection, status);

    return status ;
}
#endif /*OI_CODE*/

/**
 * Response to an authentication indication.
 */

OI_STATUS OI_OBEXCLI_AuthenticationResponse(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                            const OI_BYTE *userId,
                                            OI_UINT8 userIdLen,
                                            const OI_CHAR *password)
{
    OI_STATUS status;
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_AuthenticationResponse (connectionId = %d, userId = \"%?s\", userIdLen = %d, password = \"%s\")\n",
                 connectionId, userId, userIdLen, userIdLen, password ? password : "<NULL>"));


#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Client Auth Response: ConnID=%d : UIDLen=%d",connectionId,userIdLen, 0 );
#endif
    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    /*
     * Make sure we are doing authentication.
     */
    if (!connection->common.authenticating) {
        return OI_OBEX_METHOD_NOT_ALLOWED;
    }

    if (password == NULL) {
        /*
         * Application is abandoning the connection attempt.
         */
        OI_DBGPRINT(("Application didn't specify password. aborting connection."));
        status = OI_OBEX_CLIENT_ABORTED_COMMAND; 
        goto AuthResponseError;
    }
    
    status = OI_OBEXAUTH_SaveAuthInfo(&connection->common, userId, userIdLen, password); 
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXAUTH_SaveAuthInfo"));
#endif
        goto AuthResponseError;
    }

#ifndef OI_CODE
    if((connection->cliAuthInit == TRUE) && (connection->common.mutualAuth != TRUE)){
        status = OBEXAuthResponse(connectionId);
        return status ;
    }
#endif /*OI_CODE*/

    /*
     * Re-issue the connection request with authentication information.
     */
    status = SendConnectRequest(connection, OBEX_AUTH_TIMEOUT);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("SendConnectRequest"));
#endif
        goto AuthResponseError;
    }

    return OI_OK;

AuthResponseError:

    OI_OBEXAUTH_Reset(&connection->common);
    ReportError(connection, OI_OBEX_CLIENT_ABORTED_COMMAND);

    return status;
}


static void HandleResponsePkt(OBEXCLI_CONNECTION *connection)
{
    OI_UINT8 rspCode;

    OI_ASSERT(connection->common.rcvState == OI_OBEX_RCV_COMPLETE);

    /*
     * Get the OBEX response code and skip over the packet length.
     */
    ByteStream_GetUINT8(connection->common.rcvPacket, rspCode);
    ByteStream_Skip(connection->common.rcvPacket, sizeof(OI_UINT16));

    /*
     * Cancel the timeout function.
     */
    OI_Dispatch_CancelFunc(connection->timeoutCB);
    connection->timeoutCB = 0;

    /*
     * Ignore any OBEX data we receive while we are dropping the RFCOMM link.
     */
    if (!connection->droppingLink) {

        switch (connection->state) {
            case RFCOMM_CONNECTED:
                ConnectResponse(connection, rspCode);
                break;
            case OBEX_DISCONNECTING:
                DisconnectResponse(connection, rspCode);
                break;
            case OBEX_PUTTING:
                PutResponse(connection, rspCode);
                break;
            case OBEX_GETTING:
                GetResponse(connection, rspCode);
                break;
            case OBEX_SETTING_PATH:
                SetpathResponse(connection, rspCode);
                break;
            case OBEX_ABORTING:
                /*
                 * This should be the response to the abort command.
                 */
                connection->state = OBEX_CONNECTED;
                /*
                 * Call the appropriate callback to let the application know
                 * that the last GET or PUT operation was aborted.
                 */
                switch (connection->abort) {
                    case ABORT_GET:
                        connection->abort = NO_ABORT;
#ifndef OI_CODE
                        connection->abortCurOp = FALSE;
#endif
                        connection->CB.get.recvData(connection->common.link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
                        break;
                    case ABORT_PUT:
                        connection->abort = NO_ABORT;
#ifndef OI_CODE
                        connection->abortCurOp = FALSE;
#endif
                        connection->CB.put.confirm(connection->common.link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
                        break;
                    case GET_ABORTED:
                    case PUT_ABORTED:
                        connection->abort = NO_ABORT;
                        if (connection->CB.abort.confirm) {
                            connection->CB.abort.confirm(connection->common.link);
                        }
                        break;
                    default:
                        break;
                }
                break;
#ifndef OI_CODE
            case OBEX_CONNECTED:
                /*
                 * We probably got here because remote end sent data
                 * even after Abort.
                 */ 
                OI_LOG_ERROR(("Ignoring unexpected response packet"));
                break;
#endif
            default:
                /*
                 * No data expected from server in this state.
                 */
                OI_LOG_ERROR(("Unexpected reponse packet"));
                ConnectionError(connection, OI_OBEX_ERROR);
        }
    }

    connection->common.rcvState = OI_OBEX_RCV_BUF_EMPTY;
}


/**
 * Data received from an OBEX server. 
 */

static void RfcommRecvDataInd(OI_RFCOMM_LINK_HANDLE link,
                              OI_BYTE *dataBuf,
                              OI_UINT16 dataLen)
{
    OBEXCLI_CONNECTION *connection = LookupConnection(link);
    OI_STATUS status;

    if (connection == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX received data for unknown connection"));
#endif
        return;
    }

    OI_DBGPRINT2(("OBEX client received data"));

    status = OI_OBEXCOMMON_ReassemblePacket(&connection->common, dataBuf, dataLen);
    if (!OI_SUCCESS(status)) {
        OI_DBGPRINT(("OBEX client receive error %!", status));
        ConnectionError(connection, status);
        return;
    }

    if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) {
        OI_DBGPRINT2(("OBEX client packet complete"));
        /*
         * If there is a packet being sent, we must defer processing this
         * response until we have confirmation that the send has completed.
         */
        if (connection->common.mbuf != NULL) {
            OI_DBGPRINT2(("OBEX server postponing response"));
        } else {
            HandleResponsePkt(connection);
        }
    }
}


/**
 * Confirmation that an RFCOMM link has been established to a remote OBEX
 * server. Now we set up the OBEX connection over this link.
 */

static void RfCommConnectCfm(OI_RFCOMM_LINK_HANDLE link,
                             OI_UINT16 frameSize,
                             OI_STATUS result)
{
    OBEXCLI_CONNECTION *connection = LookupConnection(link);

    OI_DBGPRINT2(("RfCommConnectCfm %d", link));

    /*
     * We expect to find a matching entry - with no match there is
     * no callback we can call so silently return.
     */
    if (connection == NULL) {
        OI_LOG_ERROR(("Error: null connection handle!"));
        return;
    }

    OI_ASSERT(connection->state == RFCOMM_CONNECTING);

    if (OI_SUCCESS(result)) {
        connection->common.frameSize = frameSize;
        connection->state = RFCOMM_CONNECTED;
        /*
         * Use the default timeout on a connect request rather than application
         * specified response timeout.
         */
        result = SendConnectRequest(connection, OBEX_DEFAULT_RESPONSE_TIMEOUT);
    }
    /*
     * If the RFCOMM connection failed, free the entry and propogate the error
     * status to the application via the connect confirm callback.
     */
    if (!OI_SUCCESS(result)) {
        ConnectionError(connection, result);
    }
}


/**
 * Establishes a connection to an OBEX server by creating an RFCOMM link and
 * then sending an OBEX connect request. The connect confirmation callback is
 * not called until an OBEX connect response is received from the remote RFCOMM
 * server.
 */

OI_STATUS OI_OBEXCLI_Connect(OI_BD_ADDR *addr,
                             OI_UINT8 channel,
                             OI_OBEX_HEADER_LIST const *cmdHeaders,
                             OI_OBEXCLI_CONNECT_CFM connectCfmCB,
                             OI_OBEXCLI_DISCONNECT_IND disconnectIndCB,
                             OI_OBEXCLI_AUTH_CHALLENGE_IND authChallengeIndCB,
#ifndef OI_CODE
                             OI_OBEXCLI_AUTH_RESPONSE_IND authResponseIndCB,
#endif
                             OI_OBEXCLI_CONNECTION_HANDLE *connectionId,
                             const OI_CONNECT_POLICY *policy)
                             
{
    static const OI_RFCOMM_LINK_INDICATIONS linkIndications = {
        RfcommDisconnectInd,
        RfcommRecvDataInd,
        NULL,
        NULL,
        NULL
    };
    OI_STATUS status;
    OBEXCLI_CONNECTION *connection = NULL;

#ifndef OI_CODE
    OI_UINT16 authDataIndex = 0;
#endif

    OI_DBGTRACE(("OI_OBEXCLI_Connect (<*addr = %x>, channel = %d, <*cmdHeaders = %x>, <*connectCfmCB = %x>, <*disconnectIndCB = %x>, <authChallengeIndCB = %x>, <*connectionId = %x>, <*policy = %x>)\n",
                 addr, channel, cmdHeaders, connectCfmCB, disconnectIndCB, authChallengeIndCB, connectionId, policy));
    OI_DBGPRINT2(("\taddr = %:\n", addr));



#ifdef OI_CODE
#error code not present
#endif
    ARGCHECK(connectCfmCB != NULL);
    ARGCHECK(disconnectIndCB != NULL);
    ARGCHECK(connectionId != NULL);

    OI_DBGPRINT2(("OBEX client connecting"));

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Client connecting", 0, 0, 0 );
#endif

    /*
     * Allocate a connection record
     */
    connection = OI_Calloc(sizeof(OBEXCLI_CONNECTION));
    if (connection == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }
    /*
     * Default timeout can be overriden by OI_OBEXCLI_SetResponseTimeout()
     */
    connection->responseTimeout = OBEX_DEFAULT_RESPONSE_TIMEOUT;
    /*
     * Until we negotiate maximum  packet size for sending on this connection
     * use the default minimum packet sizes.
     */
    connection->common.maxSendPktLen = OI_OBEX_MIN_PACKET_SIZE;
    /*
     * The maximum packet size we can receive is specified by a configuration
     * parameter.
     */
    connection->common.maxRecvPktLen = OI_CONFIG_TABLE_GET(OBEX_CLI)->maxPktLen;
    /*
     * Allocate a buffer for packet reassembly.
     */
    connection->common.rcvBuffer = OI_Malloc(connection->common.maxRecvPktLen);
    if (connection->common.rcvBuffer == NULL) {
        OI_Free(connection);
        return OI_STATUS_NO_RESOURCES;
    }

    /*
     * Global connection state
     */
    connection->state = RFCOMM_CONNECTING;
    connection->disconnectInd = disconnectIndCB;
#ifndef OI_CODE
    connection->disconnectHdrCount = 0;
    connection->disconStatus = OI_OK;
#endif
    connection->common.authenticating = FALSE;
    connection->cid = OI_OBEX_INVALID_CONNECTION_ID;

    /*
     * Command specific state
     */
    connection->CB.connect.confirm = connectCfmCB;

    /*
     * An application indicates that it supports authentication by passing an
     * authenticatio challenge callback function. If authentication is required
     * allocate memory for managing the authentication state information.
     */
    if (authChallengeIndCB != NULL) {
        connection->common.authentication = OI_Malloc(sizeof(OBEX_AUTHENTICATION));
        if (connection->common.authentication == NULL) {
            status = OI_STATUS_OUT_OF_MEMORY;
            goto ConnectErrorExit;
        }
        connection->challengeCB = authChallengeIndCB;
    }

#ifndef OI_CODE
    if(authResponseIndCB) {
        BT_MSG_DEBUG("BT OBEX: Client Auth Response CB registered", 0, 0, 0 );
    }
    connection->authResponseCB = authResponseIndCB;
#endif
    /*
     * Copy the connect command headers
     */
    if (cmdHeaders == NULL) {
        connection->connectHdrCount = 0;
    } else {
        OI_ASSERT(cmdHeaders->count <= OI_OBEX_MAX_CONNECT_HDRS);
        OI_MemCopy(connection->connectHdrs, cmdHeaders->list, sizeof(OI_OBEX_HEADER) * cmdHeaders->count);
        connection->connectHdrCount = cmdHeaders->count;
    }

#ifndef OI_CODE
    /* Check Client APP issued Authenticate command */
    status = SearchCliAuthInfo(addr, channel, &authDataIndex);

    if (OI_SUCCESS(status)){
       /*
        * Cleint APP issued Authenticate command
        */
       connection->cliAuthInit = TRUE;
       connection->common.authenticating = TRUE;
       /* 
        *Store values received from client authenticate command from 
        *the client application  
        */
       if(connection->common.authentication != NULL){
           if(cliRealmInfo[authDataIndex].uidRqd == TRUE){
               connection->common.authentication->userIdRequired = TRUE;
           }
           if(cliRealmInfo[authDataIndex].fullAccess != TRUE){
               connection->common.readOnly = TRUE ;
           }
           if(cliRealmInfo[authDataIndex].realmInfo.realmLen > 0){
               connection->common.authentication->realmStrType = 
                        cliRealmInfo[authDataIndex].realmInfo.realmStrType;
               OI_MemCopy(&connection->common.authentication->realmStr[0],
                          &cliRealmInfo[authDataIndex].realmInfo.realmStr[0],
                          cliRealmInfo[authDataIndex].realmInfo.realmLen);
               connection->common.authentication->realmLen = 
                   cliRealmInfo[authDataIndex].realmInfo.realmLen;
           }

           /*Re - Initialize client auth data is stored in connection database */
           OI_OBEXCLI_InitData(authDataIndex); 
       }
    }
#endif /* OI_CODE */
    /*
     * Use the requested packet length for the maximum frame size.
     */
    status = OI_RFCOMM_Connect(RfCommConnectCfm,
                               &linkIndications,
                               addr,
                               channel,
                               OI_CONFIG_TABLE_GET(OBEX_CLI)->maxPktLen, /* Try framesize that holds an entire packet. */
                               0,                         /* No buffering by RFCOMM */
                               &connection->common.link,
                               policy);

    if (!OI_SUCCESS(status)) {
        goto ConnectErrorExit;
    }

    /*
     * Associate the connection struct with the RFCOMM link.
     */
    status = OI_RFCOMM_SetLinkContext(connection->common.link, connection);
    if (!OI_SUCCESS(status)) {
        goto ConnectErrorExit;
    }

    /*
     * An OBEX server handle is simply the underlying RFCOMM connection handle.
     */
    *connectionId = connection->common.link;

    OI_INIT_FLAG_INCREMENT(OBEX_CLI);

    return OI_OK;

ConnectErrorExit:

    if (connection) {
        OI_FreeIf(&connection->common.rcvBuffer);
        OI_FreeIf(&connection->common.authentication);
        OI_Free(connection);
    }

    return status;
}


/**
 * Request to disconnect a link to an OBEX server. The disconnect will complete
 * asynchronously.
 */

#ifdef OI_CODE
#error code not present
#else
OI_STATUS OI_OBEXCLI_Disconnect(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                OI_OBEX_HEADER_LIST const *optionalHdrs)
#endif
{
    OI_STATUS status;
    OBEXCLI_CONNECTION *connection = LookupConnection(connectionId);

    OI_DBGTRACE(("OI_OBEXCLI_Disconnect (connectionId = %d)\n", connectionId));

    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        if ((connection != NULL) && (connection->state != INVALID_CONNECTION)) {
            /*
             * Some kind of limbo state - cleanup connection.
             */
#ifndef OI_CODE
            connection->cancelConnect = TRUE;
#endif
            ConnectionError(connection, OI_OBEX_CLIENT_ABORTED_COMMAND);
        }
        return OI_OK;
    }
    if (connection->state > OBEX_CONNECTED) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
#ifdef OI_CODE
#error code not present
#else
    status = ClientSendCommand(connection, OI_OBEX_CMD_DISCONNECT, optionalHdrs, OBEX_DISCONNECT_TIMEOUT);
#endif
    if (OI_SUCCESS(status)) {
        connection->state = OBEX_DISCONNECTING;
    } else {
        /*
         * We could not send the disconnect so the connection has probably gone
         * away already.
         */
        ConnectionError(connection, status);
    }
    return OI_OK;
}


OI_STATUS OI_OBEXCLI_SetResponseTimeout(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                        OI_INTERVAL timeout)
{
    OBEXCLI_CONNECTION *connection;

    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        OI_DBGPRINT(("OI_OBEXCLI_SetResponseTimeout: not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }
    ARGCHECK(timeout > 0);
    connection->responseTimeout = timeout;
    return OI_OK;
}


OI_STATUS OI_OBEXCLI_Abort(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                           OI_OBEXCLI_ABORT_CFM abortCfm)
{
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_Abort (connectionId = %d, <*abortCfm = %x>)\n",
                 connectionId, abortCfm));

    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        OI_DBGPRINT(("OI_OBEXCLI_Abort: not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }

    if ((connection->state != OBEX_GETTING) && (connection->state != OBEX_PUTTING)) {
#ifndef OI_CODE
        if(connection->state == OBEX_SETTING_PATH || 
           connection->state == OBEX_CONNECTED)
        {
            connection->abortCurOp = TRUE;
            return OI_STATUS_PENDING;
        }
#endif
        OI_DBGPRINT(("OI_OBEXCLI_Abort: no operation to abort"));
        return OI_STATUS_INVALID_STATE;
    }
        /*
         * We need to know if we are aborting a GET or a PUT so we can call the
         * appropriate callback function when we get a response to the abort
         * command.
         */
#ifdef OI_CODE
#error code not present
#else
else{
        OI_DBGPRINT(("%s abort pending ", (connection->state == OBEX_GETTING) ? "GET" : "PUT"));
        connection->abort = (connection->state == OBEX_GETTING) ? GET_ABORTED : PUT_ABORTED;
        connection->CB.abort.confirm = abortCfm;
        SendAbort(connection);
        return OI_OK;
    }
#endif

#ifndef OI_CODE
  #if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
  #endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */
#endif /* not OI_CODE */

}


OI_STATUS OI_OBEXCLI_SetPath(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                             OI_OBEX_UNICODE const *folder,
                             OI_BOOL dontCreate,
                             OI_BOOL upLevel,
#ifndef OI_CODE
                             OI_OBEX_HEADER_LIST const *cmdHdrs,
#endif
                             OI_OBEXCLI_SETPATH_CFM setpathCfm)
{
    OI_OBEX_HEADER headers[2];
    OI_UINT16 headerCount = 0;
    OI_OBEX_HEADER *hdr;
    OI_BYTE_STREAM pkt;
    OI_STATUS status;
    OI_UINT8 flags;
    const OI_UINT8 constants = 0;
    OBEXCLI_CONNECTION *connection;
#if defined(OI_DEBUG)
#error code not present
#endif

    OI_DBGTRACE(("OI_OBEXCLI_SetPath (connectionId = %d, folder = \"%?S\", dontCreate = %s, upLevel = %s, <*setpathCfm = %x>)\n",
                 connectionId, (folder && folder->str) ? folder->str : nullStr16,
                 (folder && folder->str) ? folder->len : OI_ARRAYSIZE(nullStr16) - 1,
                 dontCreate ? "TRUE" : "FALSE", upLevel ? "TRUE" : "FALSE", setpathCfm));

    ARGCHECK(setpathCfm != NULL);

    /*
     * Must be connected to an OBEX server.
     */
    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        return OI_OBEX_NOT_CONNECTED;
    }

    OI_DBGPRINT2(("OBEX client issuing SETPATH request"));

    if (BUSY(connection)) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }
    /*
     * Make sure there are no PUT or GET operations in progress.
     */
    if (connection->state != OBEX_CONNECTED) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

#ifdef OI_CODE
#error code not present
#endif

    /*
     * Save the callback function.
     */
    connection->CB.setpath.confirm = setpathCfm;
    /*
     * If a target was specified at connect time we must send the connection id.
     */
    if (connection->cid != OI_OBEX_INVALID_CONNECTION_ID) {
        hdr = &headers[headerCount++];
        hdr->id = OI_OBEX_HDR_CONNECTION_ID;
        hdr->val.connectionId = connection->cid;
    }

    if (!upLevel) {
        hdr = &headers[headerCount++];
        hdr->id = OI_OBEX_HDR_NAME;
        /*
         * An empty name header sets the path to the root folder.
         */
        if (folder == NULL) {
            hdr->val.name.str = NULL;
            hdr->val.name.len = 0;
        } else {
            hdr->val.name = *folder;
        }
    }

    OI_OBEXCOMMON_InitPacket(&connection->common, OI_OBEX_FINAL(OI_OBEX_CMD_SET_PATH), &pkt);
    flags = 0;

#ifndef OI_CODE
    BT_MSG_HIGH("Setpath Command Recd from app: DonotCreate=%x : UpLevel=%x", dontCreate, upLevel, 0);
#endif

    if (dontCreate) {
        flags |= OI_OBEX_SETPATH_NO_CREATE;
    }
    if (upLevel) {
        flags |= OI_OBEX_SETPATH_UP_LEVEL;
    }
    ByteStream_PutUINT8(pkt, flags);
    ByteStream_PutUINT8(pkt, constants);

    connection->state = OBEX_SETTING_PATH;

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, cmdHdrs);
#endif
    if (OI_SUCCESS(status)) {
        status = ClientSendPacket(connection, connection->responseTimeout);
    }
    if (!OI_SUCCESS(status)) {
        goto SetPathError;
    }

    return OI_OK;

SetPathError:

    connection->state = OBEX_CONNECTED;

    return status;
}


OI_UINT16 OI_OBEXCLI_OptimalBodyHeaderSize(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_OptimalBodyHeaderSize (connectionId = %d)\n", connectionId));

    /*
     * We don't know the packet length until we are connected.
     */
    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        return 0;
    } else {
        return connection->common.maxSendPktLen - OI_OBEX_BODY_PKT_OVERHEAD;
    }
}


OI_STATUS OI_OBEXCLI_SetConnectionContext(OI_OBEXCLI_CONNECTION_HANDLE connectionId,
                                          void *context)
{
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_OptimalBodyHeaderSize (connectionId = %d, <*context = %x>)\n",
                 connectionId, context));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_NOT_FOUND;
    } else {
        connection->context = context;
        return OI_OK;
    }

}


void* OI_OBEXCLI_GetConnectionContext(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OBEXCLI_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXCLI_GetConnectionContext (connectionId = %d)\n", connectionId));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return NULL;
    } else {
        return connection->context;
    }
}


/* In case if the user want's to disconnect the RFCOMM channel when an
 * abort is issued, this API can be used */
OI_STATUS OI_OBEXCLI_AbortObexOperation(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OBEXCLI_CONNECTION *connection = LookupConnection(connectionId);

    if ((connection == NULL) || (connection->state < OBEX_CONNECTED))
    {
        OI_DBGPRINT(("OI_OBEXCLI_Abort: not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }

    if ((connection->state != OBEX_GETTING) && (connection->state != OBEX_PUTTING))
    {
        OI_DBGPRINT(("OI_OBEXCLI_Abort: no operation to abort"));
        return OI_STATUS_INVALID_STATE;
    }
    else
    {
       ConnectionError(connection, OI_OBEX_CLIENT_ABORTED_COMMAND);
    }
    return OI_OK;
}

#ifdef OI_DEPRECATED
/*****************************************************************************
                              Deprecated
 *****************************************************************************/

/**
 * @deprecated
 */
OI_STATUS OI_OBEXCLI_Init(OI_CONFIG_OBEX_CLI const *Config)
{
    /*
     * Update the configuration table with caller's config
     */
    OI_CONFIG_TABLE_PUT(Config, OBEX_CLI);
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OBEXCLI_Reset(void)
{
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OBEXCLI_Terminate(void)
{
    return OI_OK;
}
#endif /* OI_DEPRECATED */

/**
 * @deprecated
 */
OI_STATUS OI_OBEXCLI_AbortOperation(OI_OBEXCLI_CONNECTION_HANDLE connectionId)
{
    OBEXCLI_CONNECTION *connection;

    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state < OBEX_CONNECTED)) {
        OI_DBGPRINT(("OI_OBEXCLI_AbortOperation: not connected"));
        return OI_OBEX_NOT_CONNECTED;
    }
    if ((connection->state != OBEX_GETTING) && (connection->state != OBEX_PUTTING)) {
        /*
         * No abortable operation in progress: silently ignore.
         */
        OI_DBGPRINT(("OI_OBEXCLI_AbortOperation: no operation to abort"));
    } else {
        /*
         * We need to know if we are aborting a GET or a PUT so we can call the
         * appropriate callback function when we get a response to the abort
         * command.
         */
        connection->abort = (connection->state == OBEX_GETTING) ? ABORT_GET : ABORT_PUT;
        OI_DBGPRINT(("%s abort pending ", (connection->state == OBEX_GETTING) ? "GET" : "PUT"));
    }

#ifndef OI_CODE
  #if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
  #endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */
#endif /* not OI_CODE */

    return OI_OK;
}

#ifndef OI_CODE 
/* This is to initialize the array */
void OI_OBEXCLI_InitData(OI_UINT16 index)
{
    cliRealmInfo[index].channel = OI_OBEX_AUTH_TABLE_FREE;

    return;
}

/* This is used by the applications to update the needed realm info
 * if the application wants the client to initiate the obex authentication.
 */
OI_STATUS OI_OBEXCLI_Authenticate(OI_BD_ADDR *pAddr,
                                OI_UINT8 channel,
                                OI_REALM_INFO *realmInfo,
                                OI_BOOL userId,
                                OI_BOOL accessControl)
{
    OI_UINT16 i;
    OI_STATUS status = OI_STATUS_NO_RESOURCES;

    BT_MSG_DEBUG( "BT OBEX: Client Authenticate Command : UIDReqd=%d : \
                  FullAccess=%d : Channel=%d", userId, accessControl, channel);
 
    for (i = 0; i < OI_MAX_NUM_APPS; i++ )
    {
        /* This check verifies if the location is free */
        if(cliRealmInfo[i].channel == OI_OBEX_AUTH_TABLE_FREE)
        {
            cliRealmInfo[i].channel = channel;
            cliRealmInfo[i].fullAccess = accessControl;            
            OI_MemCopy(&cliRealmInfo[i].addr.addr[0], pAddr, sizeof(OI_BD_ADDR));
            cliRealmInfo[i].uidRqd = userId;
            cliRealmInfo[i].realmInfo.realmLen = 0;
            if(realmInfo->realmLen > 0)
            {
                BT_MSG_DEBUG("OI_OBEXCliAuthenticate : realm_len=%d:realmStrType=%d",
                                      realmInfo->realmLen,realmInfo->realmStrType,0);
                if(realmInfo->realmLen <= OI_OBEX_MAX_REALM_LEN)
                {
                    cliRealmInfo[i].realmInfo.realmLen = realmInfo->realmLen;
                    cliRealmInfo[i].realmInfo.realmStrType = realmInfo->realmStrType;
                    OI_MemCopy(&cliRealmInfo[i].realmInfo.realmStr[0],
                       realmInfo->realmStr,
                       realmInfo->realmLen);                
                }
             }
             status = OI_OK;
             break;
            }
        }

    if(i >= OI_MAX_NUM_APPS)
    {
        status = OI_STATUS_NO_RESOURCES;
        BT_MSG_DEBUG(" BT OBEX Failed to get entry in client auth table for channel=%d",
                                                                        channel,0,0);
    }
    return status;
}

/*
This function given the BD Address and the channel number, gets the corresponding
Client authentication information.
*/

static OI_STATUS SearchCliAuthInfo(OI_BD_ADDR *bdAddr,
                                   OI_UINT8 channel,
                                   OI_UINT16 *authIndex)
{
    OI_UINT16 index = OI_MAX_NUM_APPS;

    BT_MSG_DEBUG( "BT OBEX: Search entry in client auth table for channel=%d",
                                                                channel,0,0);

    for (index = 0; index < OI_MAX_NUM_APPS; index++ )
    {
        if(cliRealmInfo[index].channel == channel)
        {
            if(OI_MemCmp(bdAddr, &cliRealmInfo[index].addr.addr[0], sizeof(OI_BD_ADDR)) == 0)
            {
                *authIndex=index ;
                return OI_OK;
            }
        }
    }

    return OI_FAIL;
}
#endif /* OI_CODE */
/*****************************************************************************/

