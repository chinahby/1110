/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/obexsrv.c#2 $ 
  $DateTime: 2009/06/05 13:50:09 $
  $Author: ganeshs $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2008-07-28   gs  Added NULL ptr checks.  
  2008-07-24   gs  Fixed Klocwork error. 
*
*    #38       18 Oct  2007           MH
* Fixed server initiated authentication issues due to OI code merge
*
*    #37       29 May  2007           GS
* Fixed OBEX auth related issue introduced in previous check-in.
*
*    #36       04 Apr  2007           GS
* Updated WriteCfm() to disconnect RFCOMM if there is no OBEX connection.
*
*    #35       03 Apr  2007           MH
* Removed unused variables
*
*    #34       21 Mar  2007           MH
* Modified to take care realm string type and some code cleanup.
*
*    #33       13 Mar 2007            GS
* Added NULL pointer check in PutCommandError case.
*
*    #32       12 Mar  2007           MH
* Removing unused variables to remove compiler warnings
*
*    #31       28 Feb  2007           MH
* Added server disconnection to indicate to app and added server disconnect
* response command. 
*
*    #30       13 Feb  2007           MH
* Modified set path indication to support optional headers
*  
*    #29       14 Nov 2006            MH
* Modified to check UIDlen before storing UID. 
*
*    #28       20 Nov 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #26       17 Nov 2006            MH
* Modified to pass UserId to server app in case of server initiated auth. 
*
*    #25       26 Oct 2006            MH
* Modified srv confirm set path and srv connect indication call back to 
* support all headers
*
*    #24       25 Sept 2006           MH
* Added more response codes to map status function.
*
*    #23       19 Sept 2006           MH
* Server Authenticate command to initiate server authentication
*
*    #22       30 Aug 2006            JH
* Take connection ID header into account when figuring out optimal body header
*  size.
*
*    #21       16 Aug 2006            RP
* Added a check on the return value,from function OI_OBEXCOMMON_ParseHeaderList,
*  in function PutCommand.
*
*    #20       14 Aug 2006            BK 
* Changes for client initiation of OBEX authentication.
*
*    #19       01 Aug 2006            BK
* Removed compiler warnings.
*  
*    #18       29 Jul 2006            BK
* Modified for obex client initiated authentication.
*  
*    #16-17    10 Jul 2006            JH
* Removed extra linefeeds introduced in previous checkin.
*
*    #15       31 May 2006            JH
* Added support for optional headers in the AcceptConnect commands for JSR82.
* For SetPath command, allow folder name to be specified even if upLevel is set.
* Initialize targetInfo even when target is not specified when registering a 
*  server.
*
*    #14       22 Oct 2005            JH
* Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
*
*    #13       26 Sep 2005            ABu
* Added code to accept client connection requests using an all-zero target 
*  for IrDA interoperability with PocketPC OS.
*
*    #12       02 Sep 2005            RY
* Merged in Sirius changes for FEATURE_PCAA_SIRIUS for "+++" detection.
*
*    #11       07 Jul 2005            GS
* Added OI v3.0.13 patch for fixing problem in de-registering the server if
* Bluetooth link is lost during OBEX authentication.
*
*
*    #10       31 May 2005            RY
* Fixed compilation problem.
*
*    #9        31 May 2005            RY
* Merged in Sirius changes.
*
*    #8        24 Mar 2005            JH
* Fixed bug in OI_OBEXSRV_Get/PutResponse causing an empty header to be passed
*  to OI_OBEXCOMMON_MarshalPacket when target is not specified when connecting.
*
*    #7        14 Mar 2005            JH
* Added connection ID header to OBEX server responses.
* Added version number, flags and OBEX max pkt length fields to connect
*  error response.
*
*    #6        05 Mar 2005            JH
* Removed a call to OI_LOG_ERROR() inadvertently left behind after debugging.
*
*    #5        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/**
@file  
@internal
  
API for OBEX server
*/

/**********************************************************************************
  $Revision: #2 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_OBEX_SRV

#include "oi_rfcomm.h"
#include "oi_memmgr.h"
#include "oi_dispatch.h"
#include "oi_bytestream.h"
#include "oi_assert.h"
#include "oi_debug.h"
#include "oi_argcheck.h"
#include "oi_rfcomm_internal.h"

#include "oi_init_flags.h"
#include "oi_config_table.h"
#include "oi_bt_profile_config.h"

#include "oi_obexcommon.h"
#include "oi_obexauth.h"
#include "oi_obexsrv.h"

#ifndef OI_CODE
/* This is included to get BT Messages */
#include "btmsg.h"
#endif

/*
 * Number of failed authentication attempts allowed before the server drops the
 * RFCOMM connection.
 */

#define MAX_AUTHENTICATION_ATTEMPTS   3


typedef enum {
    SERVER_REGISTERED,  /**< The server has been registered with RFCOMM. */
    RFCOMM_CONNECTING,  /**< A client is establishng an RFCOMM link to the server. */
    RFCOMM_CONNECTED,   /**< A client has established an RFCOMM link to the server. */
    OBEX_CONNECTING,    /**< OBEX connection has been indicated to upper layer */
    CONNECT_COMPLETING, /**< Waiting for the connect to complete */
    OBEX_CONNECTED,     /**< A OBEX connection has been established over the RFCOMM link. */
    GET_IN_PROGRESS,
    GET_COMPLETING,
    PUT_IN_PROGRESS,
    PUT_COMPLETING,
    SETTING_PATH,
    OBEX_DISCONNECTING  /**< Server has received a disconnect command */
} CONNECTION_STATE;


typedef struct _OBEXSRV_TARGET_INFO {
    OI_OBEX_BYTESEQ target;                 /**< Target for this connection */
    #ifndef OI_CODE
    OI_OBEXSRV_CB_LIST *CBList;             /**< Callbacks for this connection*/
    #else
#error code not present
    #endif /* OI_CODE */
    OI_OBEXSRV_AUTHENTICATION authRequired; /**< Level of authentication required on this server */
    struct _OBEXSRV_TARGET_INFO *next;
} OBEXSRV_TARGET_INFO;


typedef struct {

    void *context;                          /**< Application specific context.  */

    OBEX_COMMON common;
    OI_BOOL clientSpecifiedTarget;          /**< Indicates if the client specified a target */
    OBEXSRV_TARGET_INFO targetInfo;         /**< List of all targets and associated callbacks */
    OBEXSRV_TARGET_INFO *currentTarget;     /**< Pointer to currently connected target */

    OI_UINT8 authTries;                     /**< How many authentication attempts */
    OI_UINT8 rfcommChannel;                 /**< RFCOMM channel for this client connection */
    OI_BOOL forcedDisconnect;               /**< Application has forced a disconnect */
    OI_BOOL unauthorized;                   /**< Connection in progress not authorized */
    OI_BOOL connectIndicated;               /**< Connection has been indicated to the upper-layer */
    CONNECTION_STATE state;

    OI_BD_ADDR clientBdAddr;                /**< BDADDR of connected client */
#ifndef OI_CODE
    OI_BOOL srvAuthInit;                     /**< Flag to Indicate Server initiated Authentication */
    OI_BOOL discInd ;                        /* Indicates disconnect indication to higher layers*/
#endif
} OBEXSRV_CONNECTION;


#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */

#ifndef OI_CODE
/*To store Realm Information*/
OI_AUTH_INFO srvRealmInfo[OI_MAX_NUM_APPS];
#endif /* OI_CODE */


/***************** Forward function definition *************************/

static void HandleRequestPkt(OBEXSRV_CONNECTION *connection);
#ifndef OI_CODE
static OI_STATUS ServerSendPacket(OBEXSRV_CONNECTION *connection);
#endif /* not OI_CODE */

/************************************************************************/

/*
 * Connection state is stored in the RFCOMM link context.
 */
#define LookupConnection(link)  ((OBEXSRV_CONNECTION*) OI_RFCOMM_GetLinkContext(link))


/*
 * Cleanup after a connection has terminated. This function is called during
 * normal and abnormal disconnects. It is possible that this function will be
 * called more than once for the same connection in certain connection failure
 * scenarios.
 */

static void ServerDisconnect(OBEXSRV_CONNECTION *connection,
                             OI_STATUS reason)
{
    CONNECTION_STATE state;

    OI_DBGTRACE(("ServerDisconnect(reason=%!)\n", reason));

    state = connection->state;
    connection->state = SERVER_REGISTERED;
    connection->authTries = 0;
    connection->forcedDisconnect = FALSE;
    connection->srvAuthInit = FALSE ;

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

    switch (state) {
        case PUT_IN_PROGRESS:
        case PUT_COMPLETING:
            connection->currentTarget->CBList->putInd(connection->common.link, NULL, OI_OBEX_NOT_CONNECTED);
            break;
        case GET_IN_PROGRESS:
        case GET_COMPLETING:
            connection->currentTarget->CBList->getInd(connection->common.link, NULL, OI_OBEX_NOT_CONNECTED);
            break;
        default:
            break;
    }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif
    OI_FreeIf(&connection->common.authentication);
    OI_FreeIf(&connection->common.rcvBuffer);
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif

    /*
     * If a connection has been indicated to the upper layer,
     * indicate that the connection has been terminated.
     */
    if (connection->connectIndicated) {
        connection->connectIndicated = FALSE;
#ifdef OI_CODE
#error code not present
#else
        connection->currentTarget->CBList->disconnectInd(connection->common.link, NULL);
#endif
    }

#ifndef OI_CODE
    if ( connection->common.rcvState != OI_OBEX_RCV_BUF_EMPTY ) {
        BT_ERR("OBEX server - unexp rcvState %d", 
               connection->common.rcvState, 0, 0);
        connection->common.rcvState = OI_OBEX_RCV_BUF_EMPTY;
    }
#endif

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif
}


/**
 *  OI_RFCOMM_DISCONNECT_IND
 */

static void RfcommDisconnectInd(OI_RFCOMM_LINK_HANDLE link,
                                OI_STATUS reason)
{
    OBEXSRV_CONNECTION *connection = LookupConnection(link);

    OI_DBGTRACE(("RfcommDisconnectInd %!", reason));

    if (connection != NULL) {
        OI_DBGPRINT2(("OBEX server RFCOMM disconnect on handle %x %!", link, reason));
        if (connection->forcedDisconnect) {
            reason = OI_OBEX_SERVER_FORCED_DISCONNECT;
        }
        ServerDisconnect(connection, reason);
    } else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX server RFCOMM disconnect on bogus link handle"));
#endif
    }
}


/**
 * Map OI status into an OBEX response.
 */

static OI_UINT8 MapStatus(OI_STATUS status)
{
    OI_UINT8 rspCode;

    switch (status) {
        case OI_OK:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_OK);
            break;
        case OI_OBEX_BAD_PACKET:
        case OI_OBEX_BAD_REQUEST:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_BAD_REQUEST);
            break;
        case OI_OBEX_OPERATION_IN_PROGRESS:
        case OI_OBEX_ACCESS_DENIED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_FORBIDDEN);
            break;
        case OI_OBEX_NOT_FOUND:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_NOT_FOUND);
            break;
        case OI_OBEX_UNKNOWN_COMMAND:
        case OI_OBEX_NOT_IMPLEMENTED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_NOT_IMPLEMENTED);
            break;
        case OI_OBEX_LENGTH_REQUIRED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_LENGTH_REQUIRED);
            break;
        case OI_OBEX_SERVICE_UNAVAILABLE:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_SERVICE_UNAVAILABLE);
            break;
        case OI_OBEX_VALUE_NOT_ACCEPTABLE:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_NOT_ACCEPTABLE);
            break;
        case OI_OBEX_REQUIRED_HEADER_NOT_FOUND:
        case OI_OBEX_UNSUPPORTED_VERSION:
        case OI_OBEX_INCOMPLETE_PACKET:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_BAD_REQUEST);
            break;
        case OI_OBEX_PRECONDITION_FAILED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_PRECONDITION_FAILED);
            break;
        case OI_OBEX_DATABASE_FULL:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_DATABASE_FULL);
            break;
        case OI_OBEX_DATABASE_LOCKED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_DATABASE_LOCKED);
            break;
        case OI_OBEX_UNSUPPORTED_MEDIA_TYPE:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_UNSUPPORTED_MEDIA_TYPE);
            break;
        case OI_OBEX_UNAUTHORIZED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_UNAUTHORIZED);
            break;
        case OI_OBEX_PARTIAL_CONTENT:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_PARTIAL_CONTENT);
            break;
        case OI_OBEX_METHOD_NOT_ALLOWED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_METHOD_NOT_ALLOWED);
            break;
#ifndef OI_CODE
        case OI_OBEX_NOT_MODIFIED:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_NOT_MODIFIED);
            break;
        case OI_OBEX_GONE:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_GONE);
            break;
        case OI_OBEX_REQ_TIMEOUT:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_REQUEST_TIME_OUT);
            break;
#endif
        default:
            rspCode = OI_OBEX_FINAL(OI_OBEX_RSP_INTERNAL_SERVER_ERROR);
            break;
    }
    return rspCode;
}


#ifndef OI_CODE
/*
 * Send an parameterless OBEX response.
 */
static OI_STATUS ServerSendResponse(OBEXSRV_CONNECTION *connection,
                                    OI_UINT8 response )
{
    OI_STATUS status;
    OI_OBEX_HEADER header;
    OI_BYTE_STREAM pkt;

    OI_OBEXCOMMON_InitPacket(&connection->common, response, &pkt);
    /*
     * Only sends connection ID header when target is specified when connecting.
     */
    if (connection->clientSpecifiedTarget) {
        header.id = OI_OBEX_HDR_CONNECTION_ID;
        header.val.connectionId = (OI_UINT32) connection;

        status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, &header, 1, NULL);
        if (OI_SUCCESS(status)) {
            status = ServerSendPacket(connection);
        }
    }
    else {
      status = OI_OBEXCOMMON_SendSimple(&connection->common, response);
    }

    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("ServerSendResponse send error %!", status));
    }
    return status;
}
#endif /* not OI_CODE */

/**
 * Map an error code into an OBEX response code and send the response code to
 * the client.
 */

static void ErrorResponse(OBEXSRV_CONNECTION *connection,
                          OI_STATUS status)
{
    OI_DBGPRINT(("OBEX Server sending error response %!\n", status));
#ifdef OI_CODE
#error code not present
#else
    ServerSendResponse(connection, MapStatus(status));
#endif /* OI_CODE */
}


static void WriteCfm(OI_RFCOMM_LINK_HANDLE link,
                     OI_MBUF *mbuf,
                     OI_STATUS result)
{
    OI_STATUS status;
    OBEXSRV_CONNECTION *connection = LookupConnection(link);

    if (connection == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("Write confirmation for bogus link"));
#endif
        return;
    }

        connection->common.mbuf = NULL;
        OI_DBGPRINT2(("Write completed: freeing mbuf %#x", mbuf));
        OI_MBUF_Free(mbuf);

    if (OI_SUCCESS(result)) {
        /*
         * Check if there is a request waiting to be handled.
         */
        if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) {
            OI_DBGPRINT2(("OBEX server handling postponed response"));
            HandleRequestPkt(connection);
        }
        /*
         * Check for operations that need to be completed.
         */
        switch (connection->state) {
            case CONNECT_COMPLETING:
                /*
                 * No longer need the authentication state info.
                 */
                OI_FreeIf(&connection->common.authentication);
                connection->state = OBEX_CONNECTED;
                break;
            case GET_COMPLETING:
                if (OI_OBEX_IS_A_BODY_HEADER(connection->common.bodySegment.id)) {
                    /*
                     * We are doing automatic body header segementation so we
                     * still we are not done until the entire body header has
                     * been sent.
                     */
                    break;
                }
                connection->state = OBEX_CONNECTED;
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
                connection->currentTarget->CBList->getInd(connection->common.link, NULL, OI_OK);
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

                break;
            case PUT_COMPLETING:
                connection->state = OBEX_CONNECTED;
                connection->currentTarget->CBList->putInd(connection->common.link, NULL, OI_OK);
                break;
#ifndef OI_CODE
            case RFCOMM_CONNECTED:
                if (connection->currentTarget != NULL) {
                    break;
                }
                BT_MSG_DEBUG("WriteCfm: Disconnecting RFCOMM.. state %x", 
                             connection->state, 0, 0);
                /* 
                 * Fall through deliberate.  Disconnect RFCOMM if connection 
                 * was rejected because matching Target was not found
                 */
#endif
            case OBEX_DISCONNECTING:
                /*
                 * Drop the obex connection.
                 */
                status = OI_RFCOMM_Disconnect(link);
                if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("OI_RFCOMM_Disconnect failed"));
#endif
                    ServerDisconnect(connection, status);
                }
                break;
            default:
                /* Nothing to do */
                break;
        }
    } else {
         OI_LOG_ERROR(("OBEX server RFCOMM write error %! - disconnecting", result));
        /*
         * Drop the RFCOMM connection.
         */
        status = OI_RFCOMM_Disconnect(link);
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("OI_RFCOMM_Disconnect failed"));
#endif
            ServerDisconnect(connection, status);
        }
    }
}


static OI_STATUS ServerSendPacket(OBEXSRV_CONNECTION *connection)
{
    OI_STATUS status;

    OI_DBGPRINT2(("Sending mbuf %#x", connection->common.mbuf));

    status = OI_RFCOMM_WriteMBUF(WriteCfm, connection->common.link, connection->common.mbuf);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_RFCOMM_WriteMBUF failed"));
#endif
        OI_MBUF_Free(connection->common.mbuf);
        connection->common.mbuf = NULL;
    }
    return status;
}


static OI_STATUS ServerSendBodySegment(OBEXSRV_CONNECTION *connection)
{
    OI_STATUS status;

    OI_DBGPRINT(("ServerSendBodySegment"));

    status = OI_OBEXCOMMON_MarshalBodySegment(&connection->common);
    if (OI_SUCCESS(status)) {
        status = ServerSendPacket(connection);
    }
    else {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXCOMMON_MarshalBodySegment failed"));
#endif
    }
    
    return status;
}

/*
 * Cleanup and callback the application to indicate that the current operation
 * has been terminated by an ABORT command from the client.
 */

static OI_STATUS AbortOperation(OBEXSRV_CONNECTION *connection)
{
    OI_DBGPRINT(("Server got abort command"));
    switch (connection->state) {
        case PUT_IN_PROGRESS:
        case PUT_COMPLETING:
            connection->state = OBEX_CONNECTED;
            connection->currentTarget->CBList->putInd(connection->common.link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
            break;
        case GET_IN_PROGRESS:
        case GET_COMPLETING:
            connection->state = OBEX_CONNECTED;
            connection->currentTarget->CBList->getInd(connection->common.link, NULL, OI_OBEX_CLIENT_ABORTED_COMMAND);
            break;
        default:
            break;
    }
    /*
     * Acknowledge receipt of ABORT command.
     */
#ifdef OI_CODE
#error code not present
#else
    return ServerSendResponse(connection, OI_OBEX_FINAL(OI_OBEX_RSP_OK));
#endif /* OI_CODE */
}


/*
 * Called by application to write headers to satisfy a GET request.
 */

OI_STATUS OI_OBEXSRV_GetResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                 OI_OBEX_HEADER_LIST *headers,
                                 OI_STATUS rspStatus)
{
    OI_UINT8 rsp;
    OI_STATUS status = OI_OK;
    OI_BYTE_STREAM pkt;
    OBEXSRV_CONNECTION* connection;
#ifndef OI_CODE
    OI_OBEX_HEADER hdr;
#endif /* not OI_CODE */

    OI_DBGTRACE(("OI_OBEXSRV_GetResponse (connectionId = %d, <*headers = %x>, rspStatus = %!)\n",
                 connectionId, headers, rspStatus));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        OI_DBGPRINT2(("GetResponse bad connectionId"));
        return OI_STATUS_INVALID_PARAMETERS;
    }
    /*
     * Only one operation at a time on each connection.
     */
    if (connection->state != GET_IN_PROGRESS) {
        OI_DBGPRINT2(("GetResponse when other operation in progress"));

        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    OI_DBGPRINT2(("GetResponse %!", rspStatus));

    if ((rspStatus != OI_OK) && (rspStatus != OI_OBEX_CONTINUE)) {
        goto GetResponseError;
    }

    if (rspStatus == OI_OBEX_CONTINUE) {
        rsp = OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE);
    } else {
        connection->state = GET_COMPLETING;
        rsp = OI_OBEX_FINAL(OI_OBEX_RSP_OK);
    }
    OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
#ifdef OI_CODE
#error code not present
#else
    /*
     * Only sends connection ID header when target is specified when connecting.
     */
    if (connection->clientSpecifiedTarget) {
        hdr.id = OI_OBEX_HDR_CONNECTION_ID;
        hdr.val.connectionId = (OI_UINT32) connection;
        status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, &hdr, 1, headers);
    } else {
        status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, NULL, 0, headers);
    }
#endif /* OI_CODE */

    if (OI_SUCCESS(status)) {
        status = ServerSendPacket(connection);
    }
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXSRV_GetResponse failed"));
#endif
        rspStatus = status;
        goto GetResponseError;
    }

    return OI_OK;

GetResponseError:

    /*
     * Report an error result to the client.
     */
    connection->state = OBEX_CONNECTED;
    ErrorResponse(connection, rspStatus);
    return status;
}


/*
 * Called by application to acknowledge a PUT request. Most servers will
 * respond with NULL headers, but the Sync profile requires Application
 * Parameter headers in some PUT responses.
 */

OI_STATUS OI_OBEXSRV_PutResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                 OI_OBEX_HEADER_LIST *headers,
                                 OI_STATUS rspStatus)
{
    OI_UINT8 rsp;
    OI_STATUS status = OI_OK;
    OI_BYTE_STREAM pkt;
    OBEXSRV_CONNECTION* connection;
#ifndef OI_CODE
    OI_OBEX_HEADER hdr;
#endif /* not OI_CODE */

    OI_DBGTRACE(("OI_OBEXSRV_PutResponse (connectionId = %d, <*headers = %x>, rspStatus = %!)\n",
                 connectionId, headers, rspStatus));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    /*
     * Only one operation at a time on each connection.
     */
    if (connection->state != PUT_IN_PROGRESS) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    OI_DBGPRINT2(("OI_OBEXSRV_PutResponse(%d, %8x, %!)", connectionId, headers, rspStatus));

    if ((rspStatus != OI_OK) && (rspStatus != OI_OBEX_CONTINUE) && (rspStatus != OI_OBEX_PARTIAL_CONTENT)) {
        goto PutResponseError;
    }

    if (rspStatus == OI_OBEX_CONTINUE) {
        rsp = OI_OBEX_FINAL(OI_OBEX_RSP_CONTINUE);
    } else {
        connection->state = PUT_COMPLETING;
        rsp = MapStatus(rspStatus);
    }

    if (headers) {
        OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
#ifdef OI_CODE
#error code not present
#else
        /*
         * Only sends connection ID header when target is specified when connecting.
         */
        if (connection->clientSpecifiedTarget) {
            hdr.id = OI_OBEX_HDR_CONNECTION_ID;
            hdr.val.connectionId = (OI_UINT32) connection;
            status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, &hdr, 1, headers);
        } else {
            status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, NULL, 0, headers);
        }
#endif /* OI_CODE */
        if (OI_SUCCESS(status)) {
            status = ServerSendPacket(connection);
        }
    } else {
        if (connection->state == PUT_COMPLETING) {
            connection->state = OBEX_CONNECTED;
        }
#ifdef OI_CODE
#error code not present
#else
        status = ServerSendResponse(connection, rsp);
#endif /* OI_CODE */
    }
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXSRV_PutResponse failed"));
#endif
        rspStatus = status;
        goto PutResponseError;
    }

    return OI_OK;

PutResponseError:

    connection->state = OBEX_CONNECTED;
    ErrorResponse(connection, rspStatus);
    return status;
}

#ifndef OI_CODE
OI_STATUS OBEXSRV_DisconnectResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                     OI_OBEX_HEADER_LIST *headers,
                                     OI_STATUS rspStatus)
{
    OI_UINT8 rsp;
    OI_STATUS status = OI_OK;
    OI_BYTE_STREAM pkt;
    OBEXSRV_CONNECTION* connection;

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    /*
     * Only one operation at a time on each connection.
     */
    if (connection->state != OBEX_DISCONNECTING) {
        return OI_OBEX_DISCONNECT_FAILED;
    }

    rsp = MapStatus(rspStatus);
  
    if (headers) {

        BT_MSG_DEBUG("OBEXSRV_DisconnectResponse... HdrCnt=%d : rspStatus= %x", 
                                                headers->count, rspStatus,0);

        OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
        status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, NULL, 0, headers);
        if (OI_SUCCESS(status)) {
            status = ServerSendPacket(connection);
        }
    } else {
        status = ServerSendResponse(connection, rsp);
    }
    if (!OI_SUCCESS(status)) {
        BT_MSG_HIGH("OBEXSRV_DisconnectResponse failed",0,0,0);
        rspStatus = status;
        goto DisconnectResponseError;
    }

    return OI_OK;

DisconnectResponseError:
    BT_MSG_DEBUG("DisconnectResponseError: status=%d",status,0,0);
    ErrorResponse(connection, rspStatus);
    return status;
}
#endif

/**
 * Bluetooth specification 5.6 - if a Target was used to establish the
 * connection a ConnectionId is required in all subesquent requests.
 */
static OI_STATUS VerifyConnectionId(OBEXSRV_CONNECTION *connection,
                                    OI_OBEX_HEADER_LIST *headers)
{
    OI_STATUS status = OI_OK;
    OI_OBEX_HEADER *conId;

    if (connection->clientSpecifiedTarget) {
        conId = OI_OBEXCOMMON_FindHeader(headers, OI_OBEX_HDR_CONNECTION_ID);
        if (conId == NULL) {
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
        } else {
            if (conId->val.connectionId != (OI_UINT32) connection) {
                status = OI_OBEX_SERVICE_UNAVAILABLE;
            }
        }
    }
    return status;
}


/**
 * 
 */

static OI_STATUS GetCommand(OBEXSRV_CONNECTION *connection,
                            OI_BOOL final)
{
    OI_OBEX_HEADER_LIST hdrs = { NULL, 0 };
    OI_BOOL newRequest;
    OI_STATUS status;

    /*
     * Is this the start of a new GET request?
     */
    if (connection->state == OBEX_CONNECTED) {
        OI_DBGPRINT2(("OBEX server received new get command"));
        newRequest = TRUE;
        connection->state = GET_IN_PROGRESS;
    } else {
        newRequest = FALSE;
        if ((connection->state != GET_IN_PROGRESS) && (connection->state != GET_COMPLETING)) {
            OI_DBGPRINT(("OBEX server received get command during other operation"));
            status = OI_OBEX_OPERATION_IN_PROGRESS;
            goto GetCommandError;
        }
    }

#ifndef OI_CODE
    if (connection->currentTarget == NULL || connection->currentTarget->CBList == NULL) {
        BT_ERR("GetCommand: encountered unexp NULL ptr!", 0, 0, 0);
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto GetCommandError;
    }
#endif 

    /*
     * Check that there is a server to handle the get request.
     */
    if (connection->currentTarget->CBList->getInd == NULL) {
        OI_DBGPRINT(("OBEX server received get without application to service it"));
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto GetCommandError;
    }

    /*
     * Parse the command into a list of headers. 
     */
    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXCOMMON_ParseHeaderList failed"));
#endif
        goto GetCommandError;
    }

    if (newRequest) {
        /*
         * Bluetooth specification 5.6 - either the Type header or the Name header
         * must be included in the GET request.
         */
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
        /*
         * Bluetooth specification 5.6 - either the Type header or the Name header
         * must be included in the GET request.
         */
        if (!OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_NAME) && !OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_TYPE)) { 
            status = OI_OBEX_REQUIRED_HEADER_NOT_FOUND;
            goto GetCommandError;
        }
        /*
         * Only the first command packet has a connection id.
         */
        status = VerifyConnectionId(connection, &hdrs);
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("VerifyConnectionId failed"));
#endif
            goto GetCommandError;
        }

#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
        /*
         * Make sure the body segmentation header is cleared.
         */
        connection->common.bodySegment.id = 0;
    } else {
        /*
         * If we are sending a segmented body header we will not be calling the
         * application for more data until we are done with this body header.
         */
        if (OI_OBEX_IS_A_BODY_HEADER(connection->common.bodySegment.id)) {
            status = ServerSendBodySegment(connection);
            if (!OI_SUCCESS(status)) {
                goto GetCommandError;
            }
            OI_FreeIf(&hdrs.list);
            return OI_OK;
        }
    }

    /*
     * Send event to application to request object data.
     */
    status = connection->currentTarget->CBList->getInd(connection->common.link,
                                                       &hdrs,
                                                       final ? OI_OBEX_CONTINUE : OI_OBEXSRV_INCOMPLETE_GET);
    if (!OI_SUCCESS(status)) {
        /*
         * Application reported an error - we need to return this error status
         * to the OBEX client.
         */
        connection->state = OBEX_CONNECTED;
        ErrorResponse(connection, status);
    }

    OI_FreeIf(&hdrs.list);
    return OI_OK;

GetCommandError:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEX get command error"));
#endif

    OI_FreeIf(&hdrs.list);

#ifdef OI_CODE
#error code not present
#else
    if (connection->currentTarget != NULL) {
        if (connection->currentTarget->CBList != NULL) {
            if (connection->currentTarget->CBList->getInd != NULL) {
                (void) connection->currentTarget->CBList->getInd(connection->common.link, NULL, status);
            }
        }
    }
#endif
    connection->state = OBEX_CONNECTED;
    ErrorResponse(connection, status);

    return status;
}

#ifndef OI_CODE
/**
 * Handle a request from the client to perform a disconnect operation.
 */

static OI_STATUS DisconnectCommand(OBEXSRV_CONNECTION *connection)
{
    OI_OBEX_HEADER_LIST headers;
    OI_STATUS status;

     BT_MSG_DEBUG("BT OBEX: Server Disconnect commnad",0,0,0);
    /*
     * Parse the command into a list of headers.
     */
    status = OI_OBEXCOMMON_ParseHeaderList(&headers, &connection->common.rcvPacket);
    BT_MSG_DEBUG("BT OBEX: Number of headers recd with disconnect request = %d",headers.count,0,0);

    if (!OI_SUCCESS(status))
    {
      goto DisconnectCommandError;
    }
   
    /*
     * Check that there is a server to handle the disconnect request.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->currentTarget == NULL || 
        connection->currentTarget->CBList == NULL || 
        connection->currentTarget->CBList->disconnectInd == NULL) {
#endif
        status = OI_OBEX_ERROR;
        goto DisconnectCommandError;
    }

    /*
     * Call server with the Disconnect request headers.
     */
    connection->currentTarget->CBList->disconnectInd(connection->common.link,
                                                                    &headers);
                                       
    OI_FreeIf(&headers.list);
    return OI_OK;

DisconnectCommandError:

    BT_MSG_HIGH("Disconnect command Error",0,0,0 );
    connection->state = OBEX_DISCONNECTING;
    OI_FreeIf(&headers.list);
    ErrorResponse(connection, status);
    return status;
}
#endif

/**
 * Handle a request from the client to perform a put operation.
 */

static OI_STATUS PutCommand(OBEXSRV_CONNECTION *connection,
                            OI_BOOL final)
{
    OI_OBEX_HEADER_LIST headers;
    OI_STATUS status;

    /*
     * Parse the command into a list of headers.
     */
    status = OI_OBEXCOMMON_ParseHeaderList(&headers, &connection->common.rcvPacket);

#ifndef OI_CODE
    if (!OI_SUCCESS(status)) {
       goto PutCommandError;
    }
#endif
    /*
     * Is this the start of a new PUT request?
     */
    if (connection->state == OBEX_CONNECTED) {
        /*
         * Only the first command packet has a connection id.
         */
#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
        status = VerifyConnectionId(connection, &headers);
        if (!OI_SUCCESS(status)) {
            goto PutCommandError;
        }
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
        connection->state = PUT_IN_PROGRESS;
    } else {
        if (connection->state != PUT_IN_PROGRESS) {
            status = OI_OBEX_OPERATION_IN_PROGRESS;
            goto PutCommandError;
        }
    }

    /*
     * Check that there is a server to handle the put request.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->currentTarget == NULL || 
        connection->currentTarget->CBList == NULL ||
        connection->currentTarget->CBList->putInd == NULL ) {
        BT_ERR("PutCommand: encountered unexp NULL ptr!", 0, 0, 0);
#endif
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto PutCommandError;
    }

    /*
     * Call server with the PUT request headers.
     */
    status = connection->currentTarget->CBList->putInd(connection->common.link,
                                        &headers,
                                        final ? OI_OK : OI_OBEX_CONTINUE);
    if (!OI_SUCCESS(status)) {
        /*
         * Application reported an error - we need to return this error status
         * to the OBEX client.
         */
        connection->state = OBEX_CONNECTED;
        ErrorResponse(connection, status);
    }

    OI_FreeIf(&headers.list);
    return OI_OK;

PutCommandError:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("PutCommand error"));
#endif

    OI_FreeIf(&headers.list);

#ifdef OI_CODE
#error code not present
#else
    if ( connection->currentTarget != NULL ) {
        if ( connection->currentTarget->CBList != NULL ) {
            if ( connection->currentTarget->CBList->putInd != NULL ) {
                connection->currentTarget->CBList->putInd(connection->common.link, NULL, status);
            }
        }
    }
#endif
    connection->state = OBEX_CONNECTED;
    ErrorResponse(connection, status);
    return status;
}


/**
 * 
 */

static OI_STATUS SetPathCommand(OBEXSRV_CONNECTION *connection)
{
    OI_OBEX_HEADER_LIST hdrs = { NULL, 0 };
    OI_OBEX_HEADER *hdr;
    OI_OBEX_UNICODE *name = NULL;
    OI_UINT8 flags = 0;
    OI_BOOL uplevel;
    OI_BOOL create;
    OI_STATUS status;

    OI_DBGPRINT(("OBEX Server - SetPathCommand"));

    if (connection->state != OBEX_CONNECTED) {
        status = OI_OBEX_OPERATION_IN_PROGRESS;
        goto SetPathCommandError;
    }

#ifndef OI_CODE
    if (connection->currentTarget == NULL || connection->currentTarget->CBList == NULL) {
        BT_ERR("SetPathCommand: encountered unexp NULL ptr!", 0, 0, 0);
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto SetPathCommandError;
    }
#endif 

    /*
     * Check that there is a server to handle the setpath request.
     */
    if (connection->currentTarget->CBList->setPathInd == NULL) {
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto SetPathCommandError;
    }


    ByteStream_GetUINT8_Checked(connection->common.rcvPacket, flags);

    /* We don't currently use constants -- Skipping*/
    /* ByteStream_GetUINT8_Checked(connection->common.rcvPacket, constants); */
    ByteStream_Skip_Checked(connection->common.rcvPacket, 1);

    if (ByteStream_Error(connection->common.rcvPacket)) {
        status = OI_OBEX_BAD_REQUEST;
        goto SetPathCommandError;
    }

    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (!OI_SUCCESS(status)) {
        goto SetPathCommandError;
    }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#else /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */
    status = VerifyConnectionId(connection, &hdrs);
    if (!OI_SUCCESS(status)) {
        goto SetPathCommandError;
    }
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

    BT_MSG_HIGH("Recd Setpath : Flags=%x",flags,0,0);

    uplevel = (flags & OI_OBEX_SETPATH_UP_LEVEL) != 0;
    create = (flags & OI_OBEX_SETPATH_NO_CREATE) == 0;

    hdr = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_NAME);
    if ((hdr != NULL) && (hdr->val.name.len != 0)) {
        name = &hdr->val.name;
    }

    /*
     * Reject illegal argument combinations.
     */

#ifdef OI_CODE
#error code not present
#else
    /* (name != NULL) && upLevel is the equivalent of cd ..\<name> and should
       be allowed */
    if ((name == NULL) && create)
#endif
    {
        status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
        goto SetPathCommandError;
    }
    connection->state = SETTING_PATH;

    /*
     * Call the application to set the path.
     */
#ifdef OI_CODE
#error code not present
#else
    status = connection->currentTarget->CBList->setPathInd(connection->common.link, name, uplevel, create, &hdrs);
#endif
    if (!OI_SUCCESS(status)) {
        goto SetPathCommandError;
    }

    OI_FreeIf(&hdrs.list);

    return OI_OK;

SetPathCommandError:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("SetPathCommand error"));
#endif

    if (connection->state == SETTING_PATH) {
        connection->state = OBEX_CONNECTED;
    }
    OI_FreeIf(&hdrs.list);
    ErrorResponse(connection, status);
    return status;
}



OI_STATUS OI_OBEXSRV_AcceptConnect(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                   OI_BOOL accept,
#ifndef OI_CODE
                                   OI_OBEX_HEADER_LIST const *cmdHdrs,
#endif
                                   OI_STATUS status)
{
    OBEXSRV_CONNECTION *connection;
    OI_OBEX_HEADER headers[3];
    OI_UINT16 headerCount = 0;
    OI_UINT8 rsp = OI_OBEX_FINAL(OI_OBEX_RSP_OK);
    OI_OBEX_HEADER *hdr;
    OI_BYTE_STREAM pkt;

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Server Accept Connect commnad Status:%d : Accept=%d", status,accept, 0 );
#endif

    OI_DBGTRACE(("OI_OBEXSRV_AcceptConnect (connectionId = %d, accept = %d, status = %!)\n",
                 connectionId, accept, status));

    if (!OI_INIT_FLAG_VALUE(OBEX_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state != OBEX_CONNECTING)) {
        return OI_OBEX_INVALID_OPERATION;
    }

    if ((accept && !OI_SUCCESS(status)) || (!accept && OI_SUCCESS(status))) {
        OI_LOG_ERROR(("Invalid combination of accept and status"));
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (!accept) {
        /*
         * RFCOMM connection is still alive.
         */
        connection->state = RFCOMM_CONNECTED;
        ErrorResponse(connection, status);
        return OI_OK;
    }

    if (connection->unauthorized) {
        /*
         * The OBEX connection has not been established
         */
        connection->state = RFCOMM_CONNECTED;
        /*
         * Reject the connection request and send an authentication challenge
         * header in the response.
         */
        rsp = OI_OBEX_FINAL(OI_OBEX_RSP_UNAUTHORIZED);

        OI_OBEXAUTH_ComposeChallenge(OI_CONFIG_TABLE_GET(OBEX_SRV)->privateKey, &connection->common, &headers[headerCount++]);
    } else {
        /*
         * The connection will be complete when the connect response packet has
         * been sent.
         */
        connection->state = CONNECT_COMPLETING;
        /*
         * If the connect request specified a target we must respond with a
         * ConnectionId header and a Who header.
         */
        if (connection->clientSpecifiedTarget) {
            hdr = &headers[headerCount++];
            hdr->id = OI_OBEX_HDR_CONNECTION_ID;
            hdr->val.connectionId = (OI_UINT32) connection;
            hdr = &headers[headerCount++];
            hdr->id = OI_OBEX_HDR_WHO;
            hdr->val.who.len = connection->currentTarget->target.len;
            hdr->val.who.data = connection->currentTarget->target.data;
        }

        /*
         * If authentication is required include an authentication challenge
         * in the connection response.
         */
#ifdef OI_CODE
#error code not present
#else
        if(connection->common.authenticating){
            if((connection->common.mutualAuth == TRUE) || (connection->srvAuthInit != TRUE)){
                OI_OBEXAUTH_ComposeResponse(&connection->common, &headers[headerCount++]);
            }
        }
#endif
    }

    /*
     * Initialize the packet header.
     */
    OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
    ByteStream_PutUINT8(pkt, OI_OBEX_VERSION_NUMBER);
    ByteStream_PutUINT8(pkt, 0); /* flags */
    ByteStream_PutUINT16(pkt, connection->common.maxRecvPktLen, OI_OBEX_BO);

#ifdef OI_CODE
#error code not present
#else
    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, headers, headerCount, cmdHdrs);
#endif
    if (OI_SUCCESS(status)) {
        status = ServerSendPacket(connection);
    }
    if (!OI_SUCCESS(status)) {
        goto ConnectRspError;
    }

    return OI_OK;

ConnectRspError:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OI_OBEXSRV_AcceptConnect error"));
#endif
    /*
     * The RFCOMM connection is still up, the OBEX connection is not.
     */
    connection->state = RFCOMM_CONNECTED;
    OI_FreeIf(&connection->common.authentication);
    ErrorResponse(connection, status);
    return status;
}


/*
 * Dispatch function for re-indicating a connection to the upper layer. This has
 * to be done via the dispatcher because a call into the stack cannot generate
 * a call out.
 */

static void ReIndicateConnect(DISPATCH_ARG *darg)
{
    OBEXSRV_CONNECTION *connection = Dispatch_GetArg(darg, OBEXSRV_CONNECTION*);

    OI_ASSERT(!connection->unauthorized);
#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Server Reindicating connection", 0, 0, 0 );
#endif
    OI_DBGPRINT(("Re-indicating connection"));

    connection->connectIndicated = TRUE;
#ifdef OI_CODE
#error code not present
#else
    connection->currentTarget->CBList->connectInd(connection->common.link, FALSE, NULL, 0, NULL);
#endif
}


/**
 * Response to a connect indication that indicated unauthorized.
 */

OI_STATUS OI_OBEXSRV_AuthenticationResponse(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                            const OI_CHAR *password,
                                            OI_BOOL readOnly)
{
    OI_STATUS status;
    DISPATCH_ARG darg;
    OBEXSRV_CONNECTION *connection;

#ifndef OI_CODE
    BT_MSG_DEBUG("BT OBEX: Server Auth Response : ConnID=%d : Readonly=%d", connectionId, readOnly, 0 );
#endif
    OI_DBGTRACE(("OI_OBEXSRV_AuthenticationResponse (connectionId = %d, password = \"%s\", readOnly = %s)",
                 connectionId, password ? password : "<null>", readOnly ? "TRUE" : "FALSE"));

    if (!OI_INIT_FLAG_VALUE(OBEX_SRV)) {
        return OI_STATUS_NOT_INITIALIZED;
    }

    /*
     * Check that we are connecting.
     */
    connection = LookupConnection(connectionId);
    if ((connection == NULL) || (connection->state != OBEX_CONNECTING)) {
        OI_DBGPRINT(("Authentication response not expected"));
        return OI_OBEX_INVALID_OPERATION;
    }

    if (!connection->unauthorized) {
        OI_DBGPRINT(("Connection did not required authentication"));
        return OI_OBEX_INVALID_OPERATION;
    }

    /*
     * Password must be a null terminated string.
     */
    if (password == NULL) {
        password = "\0";
    }

    /*
     * Readonly is only passed to the client if the connection is authenticated.
     */
    connection->common.readOnly = readOnly;

    status = OI_OBEXAUTH_SaveAuthInfo(&connection->common, NULL, 0, password);

    if (OI_SUCCESS(status)) {

#ifndef OI_CODE
        /*
        *Added to support CASE1, where client initiates the authentication and server does not initiate authentication and
        *in this case serverOBEX does not want to do authentication and just indicate to Server APP, which will then issue
        *Accept command.
        */
        if((connection->common.mutualAuth == FALSE) && (connection->srvAuthInit == FALSE)){
           BT_MSG_DEBUG("BT OBEX: Server Auth Response...Server auth response for client challenge", 0, 0, 0 );
           connection->unauthorized = FALSE;
           Dispatch_SetArg(darg, connection);
           OI_Dispatch_RegisterFunc(ReIndicateConnect, &darg, NULL);
           return status ;
        }
#endif
        /*
         * If authentication succeeds. re-indicate the connection to the
         * upper layer so that the connection can be accepted.
         */
        if (OI_OBEXAUTH_Authenticate(&connection->common) == OI_OK) {
            connection->unauthorized = FALSE;
            Dispatch_SetArg(darg, connection);
            OI_Dispatch_RegisterFunc(ReIndicateConnect, &darg, NULL);
        } else {
            if (connection->authTries < MAX_AUTHENTICATION_ATTEMPTS) {
            /*
             * Accept the connection to allow the authentication to proceed.
             */
#ifdef OI_CODE
#error code not present
#else
            status = OI_OBEXSRV_AcceptConnect(connectionId, TRUE, NULL, OI_OK);
#endif
            } else {
                OI_DBGPRINT(("Reached maximum number of authentication attempts"));
                status = OI_OBEX_ACCESS_DENIED;
            }
        }
    }

    /*
     * Reset the OBEX connection if we get an error.
     */
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXSRV_AuthenticationResponse error"));
#endif
        OI_OBEXAUTH_Reset(&connection->common);
        connection->state = RFCOMM_CONNECTED;
        ErrorResponse(connection, status);
        return OI_OK;
    } else {
        return status;
    }
}

/**
 * Given two (possibly null) byteseq pointers that represent OBEX targets,
 * determine if they match. Two byteseqs match if they both have the same data,
 * or if both are NULL or have a NULL data pointer
 */

static OI_BOOL MatchTarget(const OI_OBEX_BYTESEQ *t1,
                           const OI_OBEX_BYTESEQ *t2)
{
    if (t1 && t2 && (t1->len == t2->len)) {
        return (OI_MemCmp(t1->data, t2->data, t1->len) == 0);
    }

    return ((t1 == NULL || t1->len == 0) && (t2 == NULL || t2->len == 0));
}

/**
 * Given a server connection block and the target header from the client (if
 * present), determine if a matching connection target can be found. If so, set
 * the currentTarget field, otherwise return an error.
 */

static OI_STATUS FindTarget(OBEXSRV_CONNECTION *connection,
                            const OI_OBEX_BYTESEQ *clientTarget)
{
    OBEXSRV_TARGET_INFO *target;    /* Iterator for the list of targets */

    /*
     * We need to know if the client specified a target so we know whether to
     * use a connection id subsequent response packets.
     */
    connection->clientSpecifiedTarget = (clientTarget != NULL);

    for (target = &connection->targetInfo; target != NULL; target = target->next) {
        if (MatchTarget(clientTarget, &target->target)) {
            connection->currentTarget = target;
            return OI_OK;
        }
    }

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_IRDA) */
    return OI_OBEX_SERVICE_UNAVAILABLE;
}


#ifndef OI_CODE
/**
 * Send a failure response to a connect command.
 */
static void ConnectErrorResponse(OBEXSRV_CONNECTION *connection, OI_STATUS status)
{
    OI_UINT8 rsp;
    OI_BYTE_STREAM pkt;
  
    rsp = MapStatus(status);
    /*
     * A fail response still includes the version, flags, and packet size information.
     */
    OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
    ByteStream_PutUINT8(pkt, OI_OBEX_VERSION_NUMBER);
    ByteStream_PutUINT8(pkt, 0); /* flags */
    ByteStream_PutUINT16(pkt, connection->common.maxRecvPktLen, OI_OBEX_BO);
  
    status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, NULL, 0, NULL);
    if (OI_SUCCESS(status)) {
        status = ServerSendPacket(connection);
    }
    if (!OI_SUCCESS(status)){
      OI_LOG_ERROR(("ConnectErrorResponse send error %!", status));
    }
}
#endif /* not OI_CODE */


/**
 * Handle an OBEX connect request.
 */

static OI_STATUS ConnectCommand(OBEXSRV_CONNECTION *connection,
                                OI_UINT8 cmdCode)
{
    OI_BYTE *userId = NULL;
    OI_UINT8 userIdLen = 0;
    OI_OBEX_HEADER_LIST hdrs = { NULL, 0 };
    OI_OBEX_HEADER *target;
    OI_OBEX_HEADER *authChallenge = NULL;
    OI_OBEX_HEADER *authResponse = NULL;
    OI_STATUS status;
    OI_UINT8 version = 0;
    OI_UINT8 flags = 0;
    OI_BOOL havePassword;
    OI_BOOL haveUserId;
    OI_BOOL needUserId;
    
#ifndef OI_CODE
    OI_BOOL fullAccess = FALSE;
    OI_REALM_INFO realmInfo;
    BT_MSG_DEBUG("BT OBEX: Connect Request to Server from client", 0, 0, 0 );
#endif /* OI_CODE */

    /*
     * Reject a connect command if we are already connected.
     */
    if (connection->state != RFCOMM_CONNECTED) {
        status = OI_OBEX_SERVICE_UNAVAILABLE;
        goto ConnectError;
    }
#ifndef OI_CODE
    OI_MemSet(&realmInfo,0,sizeof(realmInfo));
#endif
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
        OI_LOG_ERROR(("OBEX Server connect - version mismatch"));
#endif
        status = OI_OBEX_UNSUPPORTED_VERSION;
        goto ConnectError;
    }
    OI_DBGPRINT2(("OBEX Server connect - version %d.%d", OI_OBEX_MAJOR_VERSION_NUMBER(version), OI_OBEX_MINOR_VERSION_NUMBER(version)));
    
    ByteStream_GetUINT8_Checked(connection->common.rcvPacket, flags);
    if (flags != 0) {
        status = OI_OBEX_COMMAND_ERROR;
        goto ConnectError;
    }
    ByteStream_GetUINT16_Checked(connection->common.rcvPacket, connection->common.maxSendPktLen, OI_OBEX_BO);
    if (ByteStream_Error(connection->common.rcvPacket)) {
        status = OI_OBEX_INCOMPLETE_PACKET;
        goto ConnectError;
    }

    /*
     * The OBEX specification defines a minimum packet length. Reject
     * connections that do not conform to the specification.
     */
    if (connection->common.maxSendPktLen < OI_OBEX_MIN_PACKET_SIZE) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX Server connect - packet size too small"));
#endif
        status = OI_OBEX_VALUE_NOT_ACCEPTABLE;
        goto ConnectError;
    }

    OI_DBGPRINT2(("OBEX Server connect - requested packet len = %d", connection->common.maxSendPktLen));
    status = OI_OBEXCOMMON_ParseHeaderList(&hdrs, &connection->common.rcvPacket);
    if (!OI_SUCCESS(status)) {
        goto ConnectError;
    }
    /* 
     * If the client has specified a target check that the server supports the
     * target. For the purposes of the comparison, NULL is a valid target and
     * is valid if the server supports the NULL target.
     */
    target = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_TARGET);
    status = FindTarget(connection, target ? &target->val.target : NULL);
    if (!OI_SUCCESS(status)) {
        OI_LOG_ERROR(("No matching server target was found"));
        goto ConnectError;
    }   

    /*
     * Authentication is required if the server requires authentication or if
     * the client included an authentication challenge in the connection
     * request.
     */
#ifdef OI_CODE
#error code not present
#else
    if (connection->currentTarget->authRequired || OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_CHALLENGE) ||
       (connection->srvAuthInit == TRUE) ) {
#endif
        connection->common.authenticating = TRUE;

        /*
         * Until we know better assume that the connection is not authorized.
         */
        connection->unauthorized = TRUE;
        /*
         * If this this is the first authentication attempt we need to
         * allocate memory to manage the authentication state information.
         */
        if (connection->common.authentication == NULL) {
            connection->common.authentication = OI_Calloc(sizeof(OBEX_AUTHENTICATION));
            if (connection->common.authentication == NULL) {
                OI_LOG_ERROR(("OBEX server could not allocate authentication state"));
                goto ConnectError;
            }
            if (connection->currentTarget->authRequired == OI_OBEXSRV_AUTH_USERID_AND_PASSWORD) {
                connection->common.authentication->userIdRequired = TRUE;
            }
        }
        /*
         * We count the number of times authentication has been attempted on to
         * establish this connection. Reject the connection request is there are
         * too many failed attempts.
         */
        if (connection->authTries >= MAX_AUTHENTICATION_ATTEMPTS) {
            OI_DBGPRINT(("Reached maximum number of authentication attempts"));
            OI_OBEXAUTH_Reset(&connection->common);
            status = OI_OBEX_ACCESS_DENIED;
            goto ConnectError;
        }
        /*
         * To do authentication we need both the challenge and response headers.
         */
        authChallenge = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_CHALLENGE);
        authResponse = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_RESPONSE);

#ifndef OI_CODE
        if(authResponse != NULL) {
            OI_OBEXAUTH_SaveResponse(&connection->common, authResponse);
            userIdLen = connection->common.authentication->userIdLen;
            if (userIdLen > 0) {
                BT_MSG_DEBUG("BT OBEX: Connection includes userId. userIdLen=%d", userIdLen, 0, 0 );
                userId = &connection->common.authentication->userId[0];
            }
        }

        /*
        For IOBEX APP, do mutual authentication only if the app issues to do so, for 
        BT APP do mutual authentication, if the client sends challenge or the server
        registers to do auth.
        */
        if(connection->targetInfo.CBList->authChallengeIndCB != NULL){
            if((connection->srvAuthInit == TRUE) && (authChallenge != NULL)){
                BT_MSG_DEBUG("BT OBEX: Mutual Authentication...IOBEX APP", 0, 0, 0 );
                connection->common.mutualAuth = TRUE ;
            }
        }else {
            BT_MSG_DEBUG("BT OBEX: Mutual Authentication...BT APP", 0, 0, 0 );
            connection->common.mutualAuth = TRUE ;
        }

        if (((authResponse == NULL) && (authChallenge != NULL)) &&
            (connection->common.mutualAuth != TRUE )) {

            BT_MSG_DEBUG("BT OBEX: Remote device sent challenge...", 0, 0, 0 );
            authChallenge = OI_OBEXCOMMON_FindHeader(&hdrs, OI_OBEX_HDR_AUTHENTICATION_CHALLENGE);

            if ( !authChallenge ) {
                goto ConnectError;
            }

            status = OI_OBEXAUTH_SaveChallenge(&connection->common, authChallenge);
            if( status != OI_OK ) {
                goto ConnectError;
            }

            userIdLen = connection->common.authentication->userIdLen;

            if (userIdLen > 0) {
                userId = &connection->common.authentication->userId[0];
                OI_DBGPRINT2(("Connection includes user id"));
            }
            connection->unauthorized = TRUE;

            realmInfo.realmLen = connection->common.authentication->realmLen ;
            realmInfo.realmStrType = connection->common.authentication->realmStrType;
                               OI_MemCopy(&(realmInfo.realmStr[0]),
                         &(connection->common.authentication->realmStr[0]),
                           connection->common.authentication->realmLen );

            /*
             * We are connecting.
             */
            connection->state = OBEX_CONNECTING;
            fullAccess = !(connection->common.readOnly);
            BT_MSG_DEBUG("BT OBEX: Authenticate Indication Call back to ServerAPP UIDLen=%d:FullAccess=%d:RealmLen=%d", 
                                                                            userIdLen, fullAccess, realmInfo.realmLen );
            connection->currentTarget->CBList->authChallengeIndCB( connection->common.link, fullAccess,
                                                               userId, connection->common.authentication->userIdRequired,
                                                               userIdLen, &realmInfo);
             /*
             * No longer need the received headers.
             */
            OI_FreeIf(&hdrs.list);

            return OI_OK; 
        }
#endif      

        if ((authResponse != NULL) && (authChallenge != NULL)) {

            OI_OBEXAUTH_SaveChallenge(&connection->common, authChallenge);
            OI_OBEXAUTH_SaveResponse(&connection->common, authResponse);

            userIdLen = connection->common.authentication->userIdLen;
            if (userIdLen > 0) {
                userId = &connection->common.authentication->userId[0];
                OI_DBGPRINT2(("Connection includes user id"));
            }
            /*
             * Does the server require a user id as well as a password?
             */
            if (connection->common.authentication->userIdRequired && (userId == NULL)) {
                /*
                 * Reject the connection attempt.
                 */
                OI_DBGPRINT(("Server requires a user id to authenticate"));
                OI_OBEXAUTH_Reset(&connection->common);
                status = OI_OBEX_ACCESS_DENIED;
                goto ConnectError;
            }
        }
    } else {
        /*
         * If the connection does not require authentication then it is
         * implicilty authorized.
         */
        connection->common.authenticating = FALSE;
        connection->unauthorized = FALSE;
        /*
        If the Server initiates authentication, first time connect request indication goes to non BT app, which will issue 
        authentication command, the next time connect indication indicates non BT app that the link is unauthorized.
        */
#ifndef OI_CODE
        if((connection->srvAuthInit == TRUE) && (connection->connectIndicated)){  
            connection->unauthorized = TRUE;
        }
#endif
    }
#ifdef OI_CODE
#error code not present
#endif
    /*
     * We are connecting.
     */
    connection->state = OBEX_CONNECTING;

    if (connection->unauthorized) {
        /*
         * Check if we have all the information we need to authenticate.
         */
        havePassword = (authResponse != NULL);
        haveUserId = (connection->common.authentication->userIdLen != 0);
        needUserId = (connection->currentTarget->authRequired == OI_OBEXSRV_AUTH_USERID_AND_PASSWORD);
        if (!havePassword || (needUserId && !haveUserId)) {
            /*
             * Continue the connection process to get all the authentication information.
             */
#ifdef OI_CODE
#error code not present
#else
            status = OI_OBEXSRV_AcceptConnect(connection->common.link, TRUE, NULL,OI_OK);

#endif
            if (OI_SUCCESS(status)) {
                return status;
            } else {
                OI_LOG_ERROR(("OBEX server could not accept connection"));
                goto ConnectError;
            }
        }
    }

    ++connection->authTries;

    /*
    * Let application know that a connect request has been received. If
    * unauthorized is TRUE the upper layer must call back with a password.
    */
#ifndef OI_CODE
     BT_MSG_DEBUG("BT OBEX: Conn Ind to Server App Link=%d UnAuthorized=%d", 
                   connection->common.link, connection->unauthorized, 0 );
#endif

     connection->connectIndicated = TRUE;
#ifdef OI_CODE
#error code not present
#else
     connection->currentTarget->CBList->connectInd(connection->common.link, 
                                                   connection->unauthorized, 
                                                   userId, userIdLen, &hdrs);
     /*
     * No longer need the received headers.
     */
    OI_FreeIf(&hdrs.list);
#endif
    return OI_OK;

ConnectError:

#ifdef OI_CODE
#error code not present
#else
    BT_MSG_DEBUG("Server Connect command Error....status=%x",status,0,0);

    
    /*
     * If we are already in a connection, do not free authentication structure.
     */
    if (connection->state == RFCOMM_CONNECTED) /* not yet in OBEX connection */
    { 
      OI_FreeIf(&connection->common.authentication);
    }
    OI_FreeIf(&hdrs.list);
    ConnectErrorResponse(connection, status);
#endif /* OI_CODE */
    return status;
}


/**
 * Returns the current target associated with an OI_OBEXSRV_CONNECTION_HANDLE
 */

OI_STATUS OI_OBEXSRV_GetTarget(OI_OBEXSRV_CONNECTION_HANDLE connectionHandle,
                               OI_OBEX_BYTESEQ *target)
{
    OBEXSRV_CONNECTION *connection = LookupConnection(connectionHandle);

    OI_DBGTRACE(("OI_OBEXSRV_GetTarget (connectionHandle = %d, <*target = %x>)\n",
                 connectionHandle, target));

    if (connection == NULL) {
        OI_DBGPRINT(("No connection for handle %x", connectionHandle));
        return OI_OBEX_NOT_CONNECTED;
    }
    *target = connection->currentTarget->target;
    return OI_OK;
}


/**
 * Returns the BD_ADDR associated with an OI_OBEXSRV_CONNECTION_HANDLE
 */

OI_STATUS OI_OBEXSRV_GetClientAddr(OI_OBEXSRV_CONNECTION_HANDLE connectionHandle,
                                   OI_BD_ADDR *pBdAddr)
{
    OBEXSRV_CONNECTION *connection = LookupConnection(connectionHandle);

    OI_DBGTRACE(("OI_OBEXSRV_GetClientAddr (connectionHandle = %d, <*pBdAddr = %x>)\n",
                 connectionHandle, pBdAddr));

    if (connection == NULL) {
        OI_DBGPRINT(("No connection for handle %x", connectionHandle));
        return OI_OBEX_NOT_CONNECTED;
    }
    *pBdAddr = connection->clientBdAddr;
    return OI_OK;
}

/**
 * Returns the RFCOMM channel number associated with an OI_OBEXSRV_CONNECTION_HANDLE
 */
OI_STATUS OI_OBEXSRV_GetRfcommChannel(OI_OBEXSRV_CONNECTION_HANDLE connectionHandle,
                                      OI_UINT8 *rfcommChannel)
{
    OBEXSRV_CONNECTION *connection = LookupConnection(connectionHandle);

    OI_DBGTRACE(("OI_OBEXSRV_GetRfcommChannel (connectionHandle, <*rfcommChannel = %x>)\n",
                 connectionHandle, rfcommChannel));

    if (connection == NULL) {
        OI_DBGPRINT(("No connection for handle %x", connectionHandle));
        return OI_OBEX_NOT_CONNECTED;
    }
    *rfcommChannel = connection->rfcommChannel;
    return OI_OK;
}


static void HandleRequestPkt(OBEXSRV_CONNECTION *connection)
{
    OI_STATUS status;
    OI_UINT8 cmdCode;

    /*
     * The client should not be sending any OBEX commands after issuing a
     * disconnect. If we get spurious commands we will force a disconnect of the
     * underlying RFCOMM connection and let the miscreant client deal with the
     * consequences.
     */
    if (connection->state == OBEX_DISCONNECTING) {
        OI_LOG_ERROR(("Received command after a disconnect"));
        status = OI_RFCOMM_Disconnect(connection->common.link);
        if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
            OI_LOG_ERROR(("OI_RFCOMM_Disconnect failed ")); 
#endif
            ServerDisconnect(connection, status);
        }
        return;
    }

    OI_ASSERT(connection->common.rcvState == OI_OBEX_RCV_COMPLETE);

    /*
     * Get the OBEX response code and skip over the packet length.
     */
    ByteStream_GetUINT8(connection->common.rcvPacket, cmdCode);
    ByteStream_Skip(connection->common.rcvPacket, sizeof(OI_UINT16));

    switch (OI_OBEX_FINAL(cmdCode)) {
        case OI_OBEX_FINAL(OI_OBEX_CMD_CONNECT):
            status = ConnectCommand(connection, cmdCode);
            break;
        case OI_OBEX_FINAL(OI_OBEX_CMD_DISCONNECT):
            OI_DBGPRINT(("OBEX server received disconnect request"));

#ifndef OI_CODE
            if(connection->discInd != TRUE)
            {
#endif
            connection->state = OBEX_DISCONNECTING;
            /*
             * Acknowledge the disconnect request. The disconnect will complete
             * when RFCOMM confirms the response has been written.
             */
#ifdef OI_CODE
#error code not present
#else
            status = ServerSendResponse(connection, OI_OBEX_FINAL(OI_OBEX_RSP_OK));
#endif /* OI_CODE */
            if (!OI_SUCCESS(status)) {
                /*
                 * Even if we are unable to respond to the disconnect request we
                 * need to act on it.
                 */
#ifdef OI_CODE
#error code not present
#else
                OI_LOG_ERROR(("OI_OBEXCOMMON_SendOk failed"));
#endif
                status = OI_RFCOMM_Disconnect(connection->common.link);
                if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
                    OI_LOG_ERROR(("OI_RFCOMM_Disconnect failed"));
#endif
                    ServerDisconnect(connection, status);
                }
            }
#ifndef OI_CODE
            } else {
                connection->state = OBEX_DISCONNECTING;
                status = DisconnectCommand(connection);
            }
#endif
            break;
        case OI_OBEX_FINAL(OI_OBEX_CMD_PUT):
            status = PutCommand(connection, OI_OBEX_IS_FINAL(cmdCode));
            break;
        case OI_OBEX_FINAL(OI_OBEX_CMD_GET):
            status = GetCommand(connection, OI_OBEX_IS_FINAL(cmdCode));
            break;
        case OI_OBEX_FINAL(OI_OBEX_CMD_SET_PATH):
            status = SetPathCommand(connection);
            break;
        case OI_OBEX_FINAL(OI_OBEX_CMD_ABORT):
            status = AbortOperation(connection);
            break;
        default:
            status = OI_OBEX_UNKNOWN_COMMAND;
    }

    connection->common.rcvState = OI_OBEX_RCV_BUF_EMPTY;

    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX request error"));
#endif
    }
}


/**
 * Data received from an OBEX client. 
 */

static void RfcommRecvDataInd(OI_RFCOMM_LINK_HANDLE link,
                              OI_BYTE *dataBuf,
                              OI_UINT16 dataLen)
{ 
    OBEXSRV_CONNECTION *connection = LookupConnection(link);
    OI_STATUS status;

    if (connection == NULL) {
        OI_LOG_ERROR(("OBEX received data for unknown connection"));
        return;
    }

    OI_DBGPRINT2(("OBEX server received data"));

    status = OI_OBEXCOMMON_ReassemblePacket(&connection->common, dataBuf, dataLen);
    if (!OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_OBEXCOMMON_ReassemblePacket failed"));
#endif
        return;
    }

    if (connection->common.rcvState == OI_OBEX_RCV_COMPLETE) {
        OI_DBGPRINT2(("OBEX server packet complete"));
        /*
         * If there is a packet being sent, we must defer processing this
         * request until we have confirmation that the send has completed.
         * This is unlikely but possible because write complete is reported
         * asynchronously.
         */
        if (connection->common.mbuf != NULL) {
            OI_DBGPRINT2(("OBEX server postponing request"));
        } else {
            HandleRequestPkt(connection);
        }
    }
}


static void RfcommConnectCfm(OI_RFCOMM_LINK_HANDLE link,
                             OI_UINT16 frameSize,
                             OI_STATUS result)
{   
    OBEXSRV_CONNECTION *connection = LookupConnection(link);

    OI_DBGTRACE(("RfcommConnectCfm %!", result));

    if (connection == NULL) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("RFCOMM connect confirm called for bogus link handle"));
#endif
        return;
    }

    if (OI_SUCCESS(result)) {
        OI_DBGPRINT(("OBEX server RFCOMM connection confirmed"));
        /*
         * Finalize the connection. Most of the intialization was performed in
         * RfcommConnectInd() when the connection was indicated.
         */
        connection->common.frameSize = frameSize;
        connection->state = RFCOMM_CONNECTED;
    } else {
        /*
         * Free any dynamically allocated memory and release the connection
         * entry.
         */
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OBEX server RFCOMM connection failed"));
#endif
        connection->state = SERVER_REGISTERED;
        OI_FreeIf(&connection->common.rcvBuffer);
    }
}



/**
 * Called by RFCOMM when a client attempt to establish an RFCOMM connection to a
 * registered OBEX server. We have to either accept or reject the link request.
 */

static void RfcommConnectInd(OI_BD_ADDR *addr,
                             OI_UINT8 channel,
                             OI_UINT16 frameSize,
                             OI_RFCOMM_LINK_HANDLE link)
{
    OBEXSRV_CONNECTION *service;
    OBEXSRV_CONNECTION *connection = NULL;
    OI_STATUS status;

    OI_DBGTRACE(("RfcommConnectInd %:", addr));

    /*
     * Reject the connection if we don't know this channel.
     */
    service = OI_RFCOMM_GetServerContext(channel);
    if (service == NULL) {
        OI_LOG_ERROR(("OBEX server - unexpected channel"));
        goto RejectConnection;
    }

    /*
     * Reject the connect attempt is the service is in use.
     *
     * Although in principle this is not disallowed by the RFCOMM spec, at this
     * time none of our OBEX server implementations allow multiple connections
     * to use the same RFCOMM channel. 
     */
    if (service->state != SERVER_REGISTERED) {
        goto RejectConnection;
    }

    /*
     * Got our connection.
     */
    connection = service;

    /*
     * Clean out stale state from the connection.
     */
    OI_MemZero(&connection->common, sizeof(OBEX_COMMON));

    /*
     * Save connecting device's bdaddr and the RFCOMM link handle.
     */
    connection->clientBdAddr = *addr;
    connection->common.link = link;
    /*
     * Until we negotiate maximum  packet size for sending on this connection
     * use the default minimum packet sizes.
     */
    connection->common.maxSendPktLen = OI_OBEX_MIN_PACKET_SIZE;
    /*
     * The maximum packet size we can receive is specified by a configuration
     * parameter.
     */
    connection->common.maxRecvPktLen = OI_CONFIG_TABLE_GET(OBEX_SRV)->maxPktLen;
    /*
     * Allocate a buffer for packet reassembly.
     */
    connection->common.rcvBuffer = OI_Malloc(connection->common.maxRecvPktLen);
    if (connection->common.rcvBuffer == NULL) {
        OI_LOG_ERROR(("OBEX server out of memory"));
        goto RejectConnection;
    }
    /*
     * Accept the link request.
     */
    connection->state = RFCOMM_CONNECTING;
    status = OI_RFCOMM_Accept(RfcommConnectCfm, link, TRUE);
    if (OI_SUCCESS(status)) {
        /*
         * Associate this OBEX connection with this connection.
         */
        status = OI_RFCOMM_SetLinkContext(link, connection);
        OI_ASSERT(OI_SUCCESS(status));
        return;
    }

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OBEX server error accepting link request"));
#endif

RejectConnection:

    if ((connection != NULL) && (connection->state == RFCOMM_CONNECTING)) {
        /*
         * Tried to accept but the accept failed.
         */
        connection->state = SERVER_REGISTERED;
    } else {
        /*
         * Reject the link request.
         */
        OI_DBGPRINT(("OBEX server rejecting link request from %:", addr));
        (void) OI_RFCOMM_Accept(NULL, link, FALSE);
    }
    /*
     * Free any dynamically allocated memory.
     */
    if (connection != NULL) {
        OI_FreeIf(&connection->common.rcvBuffer);
    }
}

/*
 * Called by application to acknowledge a setpath request. Most servers will
 * respond with NULL headers, but the IOBEX Application requires
 * Parameter headers in some set path responses.
 */

OI_STATUS OI_OBEXSRV_ConfirmSetpath(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
#ifndef OI_CODE
                                    OI_OBEX_HEADER_LIST *headers,
#endif
                                    OI_STATUS status)
{
#ifndef OI_CODE
    OI_BYTE_STREAM pkt;
    OI_UINT8 rsp;
#endif /* not OI_CODE */

    OBEXSRV_CONNECTION *connection = LookupConnection(connectionId);

    OI_DBGTRACE(("OI_OBEXSRV_ConfirmSetpath (connectionId = %d, status = %!)\n",
                 connectionId, status));

    if ((connection == NULL) || (connection->state != SETTING_PATH)) {
        return OI_OBEX_INVALID_OPERATION;
    }
    connection->state = OBEX_CONNECTED;

#ifndef OI_CODE
    if (headers) {
        rsp = MapStatus (status);
        BT_MSG_DEBUG("OBEX Srv Confirm setpath : rsp:%x",rsp,0,0);

        OI_OBEXCOMMON_InitPacket(&connection->common, rsp, &pkt);
            status = OI_OBEXCOMMON_MarshalPacket(&connection->common, &pkt, NULL, 0, headers);

        if (OI_SUCCESS(status)) {
            status = ServerSendPacket(connection);
        }
        if (!OI_SUCCESS(status)) {
            BT_MSG_DEBUG("OBEX Srv Confirm setpath : Error response Status :%x",
                                                                 status,0,0);
            ErrorResponse(connection, status);
            status = OI_OK;
        }
        return status;
    }
#endif
    if (OI_SUCCESS(status)) {
#ifdef OI_CODE
#error code not present
#else
        status = ServerSendResponse(connection, OI_OBEX_FINAL(OI_OBEX_RSP_OK));
#endif /* OI_CODE */
    } else {
        ErrorResponse(connection, status);
        status = OI_OK;
    }
    return status;
}


OI_UINT16 OI_OBEXSRV_OptimalBodyHeaderSize(OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    OBEXSRV_CONNECTION *connection = LookupConnection(connectionId);

    OI_DBGTRACE(("OI_OBEXSRV_OptimalBodyHeaderSize (connectionId = %d)\n", connectionId));

    /*
     * We don't know the packet length until we are connected.
     */
    if ((connection == NULL) || (connection->state < OBEX_CONNECTING)) {
        return 0;
    } else {
#ifdef OI_CODE
#error code not present
#else  
        /* connection ID header is now sent in get responses if target is 
           specified when connecting, need to take this into account */
        return( connection->common.maxSendPktLen - OI_OBEX_BODY_PKT_OVERHEAD -
                OI_OBEX_CONNECTION_ID_LEN );
#endif 
    }
}


/**
 * Registers the OBEX server to accept put and/or get requests from OBEX
 * clients.
 */
#ifndef OI_CODE
OI_STATUS OI_OBEXSRV_RegisterServer(const OI_OBEX_BYTESEQ *target,
                                    OI_OBEXSRV_CB_LIST *CBList,
                                    OI_OBEXSRV_AUTHENTICATION authentication,
                                    OI_UINT8 *channelNumber,
                                    const OI_CONNECT_POLICY *policy)
#else
#error code not present
#endif /* OI_CODE */
{
    static const OI_RFCOMM_LINK_INDICATIONS linkIndications = {
        RfcommDisconnectInd,
        RfcommRecvDataInd,
        NULL,
        NULL,
        NULL
    };
    OI_STATUS status;
    OBEXSRV_CONNECTION *connection = NULL;

    OI_DBGTRACE(("OI_OBEXSRV_RegisterServer (<*target = %x>, <*CBList = %x>, authentication = %s, <*channelNumber = %x>, <*policy = %x>)\n",
                 target, CBList, 
                 (authentication == OI_OBEXSRV_AUTH_NONE) ? "NONE" :
                 ((authentication == OI_OBEXSRV_AUTH_PASSWORD) ? "PASSWORD only" :
                  ((authentication == OI_OBEXSRV_AUTH_USERID_AND_PASSWORD) ? "USERID and PASSWORD" : "INVALID")),
                 channelNumber, policy));

    if ((CBList == NULL) || ((target != NULL) && (target->data == NULL))) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    OI_DBGPRINT2(("Target: %@", target ? target->data : NULL, target ? target->len : 0));

    connection = OI_Calloc(sizeof(OBEXSRV_CONNECTION));
    if (connection == NULL) {
        status = OI_STATUS_NO_RESOURCES;
        goto RegistrationFailed;
    }

    /*
     * Register the RFCOMM server. The optimal max RFCOMM frame size is the OBEX
     * packet size. Because OBEX is a command/response protocol wnd we assemble
     * complete packets before calling up the the application we do not need
     * RFCOMM to do any buffering for flow control.
     */
    status = OI_RFCOMM_RegisterServer(RfcommConnectInd,
                                      &linkIndications,
                                      OI_CONFIG_TABLE_GET(OBEX_SRV)->maxPktLen, /* optimal frame size. */
                                      0,                                        /* no buffering. */
                                      channelNumber,
                                      policy);

    if (!OI_SUCCESS(status)) {
        goto RegistrationFailed;
    }

    status = OI_RFCOMM_SetServerContext(*channelNumber, connection);
    OI_ASSERT(OI_SUCCESS(status));

    if (target != NULL) {
        connection->targetInfo.target.data = target->data;
        connection->targetInfo.target.len = target->len;
    }
#ifndef OI_CODE
    else {
      connection->targetInfo.target.data = NULL;
      connection->targetInfo.target.len = 0;
    }
#endif

    connection->targetInfo.CBList = CBList;

    #ifndef OI_CODE
    /* Initialization. If needed this will be filled later */
    connection->targetInfo.CBList->authChallengeIndCB = NULL;

    /*This indicates disconnect indication should be sent to higher layers.*/
    connection->discInd = policy->discInd ;
    BT_MSG_HIGH("OBEX SRV Reg. Policy Disconnect Indication=%x",connection->discInd,0,0);
    #endif /* OI_CODE */

    connection->targetInfo.authRequired = authentication;

    connection->state = SERVER_REGISTERED;
    connection->rfcommChannel = *channelNumber;
    connection->connectIndicated = FALSE;

    OI_INIT_FLAG_INCREMENT(OBEX_SRV);

    return OI_OK;

RegistrationFailed:

#ifdef OI_CODE
#error code not present
#else
    OI_LOG_ERROR(("OI_OBEXSRV_RegisterServer error"));
#endif

    if (connection) {
        OI_Free(connection);
    }
    return status;
}


/**
 * This function registers an additional target on an existing OBEX server.
 */
#ifndef OI_CODE
OI_STATUS OI_OBEXSRV_RegisterSecondaryTarget(const OI_OBEX_BYTESEQ *target,
                                             OI_OBEXSRV_CB_LIST *CBList,
                                             OI_OBEXSRV_AUTHENTICATION authentication,
                                             OI_UINT8 channelNumber)
#else
#error code not present
#endif /* OI_CODE */
{
    OBEXSRV_CONNECTION *connection;
    OBEXSRV_TARGET_INFO *targetInfo;

    OI_DBGTRACE(("OI_OBEXSRV_RegisterSecondaryTarget (<*target = %x>, <*CBList = %x>, authentication = %s, <*channelNumber = %x>)\n",
                 target, CBList, 
                 (authentication == OI_OBEXSRV_AUTH_NONE) ? "NONE" :
                 ((authentication == OI_OBEXSRV_AUTH_PASSWORD) ? "PASSWORD only" :
                  ((authentication == OI_OBEXSRV_AUTH_USERID_AND_PASSWORD) ? "USERID and PASSWORD" : "INVALID")),
                 channelNumber));

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return OI_STATUS_CHANNEL_NOT_FOUND;
    }

    OI_DBGPRINT2(("Target: %@", target ? target->data : NULL, target ? target->len : 0));

    if (FindTarget(connection, target) == OI_OK) {
        return OI_STATUS_ALREADY_REGISTERED;
    }
    targetInfo = OI_Calloc(sizeof(*targetInfo));
    if (targetInfo == NULL) {
        return OI_STATUS_NO_RESOURCES;
    }
    if (target != NULL) {
        targetInfo->target.data = target->data;
        targetInfo->target.len = target->len;
    }
    
    targetInfo->CBList = CBList;
    targetInfo->authRequired = authentication;
    targetInfo->next = connection->targetInfo.next;
    connection->targetInfo.next = targetInfo;
    
    return OI_OK;
}


/*
 * Forcibly sever the connection to an OBEX client.
 */
OI_STATUS OI_OBEXSRV_ForceDisconnect(OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    OBEXSRV_CONNECTION *connection;
    
    OI_DBGTRACE(("OI_OBEXSRV_ForceDisconnect (connectionId = %d)\n", connectionId));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
    if (connection->state <= SERVER_REGISTERED) {
        return OI_OBEX_NOT_CONNECTED;
    }
    /*
     * This will let us report the reason for the disconnect in any callbacks
     * that are pending.
     */
    connection->forcedDisconnect = TRUE;
    return OI_RFCOMM_Disconnect(connection->common.link);
}


/*
 * Deregisters the OBEX server so that it will no longer accept put and/or get
 * requests from OBEX clients. All targets for the service are deregistered.
 *
 * The server cannot be deregistered if there is a client connected. The
 * application must first force a disconnect and then deregister the server.
 */

OI_STATUS OI_OBEXSRV_DeregisterServer(OI_UINT8 channelNumber)
{
    OI_STATUS status;
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_DeregisterServer (channelNumber = %d)\n", channelNumber));

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return OI_OBEX_SERVICE_UNAVAILABLE;
    }
    if (connection->state != SERVER_REGISTERED) {
        return OI_OBEX_OPERATION_IN_PROGRESS;
    }

    status = OI_RFCOMM_DeregisterServer(channelNumber);
    if (!OI_SUCCESS(status)){
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_RFCOMM_DeregisterServer failed"));
#endif
    } else {
        OBEXSRV_TARGET_INFO *target = connection->targetInfo.next;
        /*
         * Free all secondary targets associated with this server
         */
        while (target != NULL) {
            OBEXSRV_TARGET_INFO *goner;
            goner = target;
            target = target->next;
            OI_Free(goner);
        }
        OI_Free(connection);
        OI_INIT_FLAG_DECREMENT(OBEX_SRV);
    }
    return status;
}


OI_BOOL OI_OBEXSRV_IsServerBusy(OI_UINT8 channelNumber)
{
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_IsServerBusy (channelNumber = %d)\n", channelNumber));

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return FALSE;
    }
    if (connection->state == SERVER_REGISTERED) {
        return FALSE;
    } else {
        return TRUE;
    }
}


void* OI_OBEXSRV_GetConnectionContext(OI_OBEXSRV_CONNECTION_HANDLE connectionId)
{
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_GetConnectionContext (connectionId = %d)\n", connectionId));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return NULL;
    } else {
        return connection->context;
    }
}

OI_STATUS OI_OBEXSRV_SetConnectionContext(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                          void *context)
{
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_SetConnectionContext (connectionId = %d, <*context = %x>)\n",
                 connectionId, context));

    connection = LookupConnection(connectionId);
    if (connection == NULL) {
        return OI_STATUS_NOT_FOUND;
    } else {
        connection->context = context;
        return OI_OK;
    }
}


OI_STATUS OI_OBEXSRV_SetServerContext(OI_UINT8 channelNumber,
                                      void *context)
{
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_SetServerContext (channelNumber = %d, <*context = %x>)\n",
                 channelNumber, context));

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return OI_STATUS_NOT_FOUND;
    } else {
        connection->context = context;
        return OI_OK;
    }

}


void* OI_OBEXSRV_GetServerContext(OI_UINT8 channelNumber)
{
    OBEXSRV_CONNECTION *connection;

    OI_DBGTRACE(("OI_OBEXSRV_GetServerContext (channelNumber = %d)\n", channelNumber));

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return NULL;
    } else {
        return connection->context;
    }
}

#ifdef OI_DEPRECATED
/**
 * @deprecated
 */
OI_STATUS OI_OBEXSRV_Init(OI_CONFIG_OBEX_SRV   const *Config)
{
    /*
     * update the configuration table with caller's config
     */
    OI_CONFIG_TABLE_PUT(Config, OBEX_SRV);
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OBEXSRV_Reset(void)
{
    return OI_OK;
}

/**
 * @deprecated
 */
OI_STATUS OI_OBEXSRV_Terminate(void)
{
    return OI_OK;
}
#endif /* OI_DEPRECATED */




#ifndef OI_CODE
OI_STATUS OI_OBEXSRV_RegisterServerAuthCb(OI_UINT8 channelNumber,
                                          OI_OBEXSRV_CB_LIST *CBlist)
{

    OBEXSRV_CONNECTION *connection;

    BT_MSG_DEBUG("BT OBEX: Authentication Indication Call back register. %d", channelNumber, 0, 0 );

    connection = OI_RFCOMM_GetServerContext(channelNumber);
    if (connection == NULL) {
        return OI_STATUS_NOT_FOUND;
    }
    connection->targetInfo.CBList = CBlist;
    return OI_OK;
}

/* 
This is used by the applications to tell the OBEX to initiate the Server authentication, 
JSR82 application can authenticate some clients and may not authenticate some clients, only
JSR82 App calls this API, OBEX Should initiates authentication.
*/

OI_STATUS OI_OBEXSRV_Authenticate(OI_OBEXSRV_CONNECTION_HANDLE connectionId,
                                    OI_BD_ADDR *bdAddr,
                                    OI_UINT8 channel,
                                    OI_REALM_INFO *realmInfo,
                                    OI_BOOL uidReqd,
                                    OI_BOOL fullAccess)
{
    OI_STATUS status = OI_OK;
    OBEXSRV_CONNECTION *connection;

    

    /*
     * Check that we are connecting.
     */
    connection = LookupConnection(connectionId);

    BT_MSG_DEBUG("BT OBEX: Server Authenticate command ConnID=%d: Channel=%d:State=%d", 
                                                connectionId, channel,connection->state);

    BT_MSG_DEBUG("BT OBEX: Server Authenticate command uidReqd:%d  FullAccess:%d", 
                                                uidReqd, fullAccess,0);

    if ((connection == NULL) || (connection->state != OBEX_CONNECTING)) {
        OI_LOG_ERROR(("Authentication response not expected"));
        return OI_OBEX_INVALID_OPERATION;
    }

    if ((connection->authTries == 0) || 
        (connection->common.authentication  == NULL)) {
        connection->common.authentication = OI_Calloc(sizeof(OBEX_AUTHENTICATION));
        if (connection->common.authentication == NULL) {
            OI_LOG_ERROR(("OBEX server could not allocate authentication state"));
            return OI_STATUS_NO_RESOURCES;
        }
    }
    /*
     * We count the number of times authentication has been attempted on to
     * establish this connection. Reject the connection request is there are
     * too many failed attempts.
     */
    ++connection->authTries;

    // Indicates Server Application requested for authentication
    connection->srvAuthInit = TRUE;

     if (connection->common.authenticating) {
         connection->common.mutualAuth = TRUE;
     }

    /* 
    Store values received from client authenticate command from 
    the client application  
    */
    if(connection->common.authentication != NULL){
        if(uidReqd == TRUE) {    
            connection->common.authentication->userIdRequired = TRUE;
         }
         if(fullAccess != TRUE){
             connection->common.readOnly = TRUE ;
         }
         if(realmInfo->realmLen > 0){
             connection->common.authentication->realmStrType = 
                                               realmInfo->realmStrType;
             OI_MemCopy(&connection->common.authentication->realmStr[0],
                        realmInfo->realmStr,
                        realmInfo->realmLen);
             connection->common.authentication->realmLen = realmInfo->realmLen;
         }
    }

    connection->unauthorized = TRUE ;
    status = OI_OBEXSRV_AcceptConnect(connectionId, TRUE, NULL, OI_OK);
    
    return status;
}
#endif /* OI_CODE */


/*****************************************************************************/


