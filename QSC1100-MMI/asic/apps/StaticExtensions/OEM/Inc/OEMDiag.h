#ifndef OEMDIAG_H
#define OEMDIAG_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

This file is covered by the licensing terms for CDMA 
ASIC licensees.  Under terms of those licensing agreements, 
this file MUST NOT be transmitted in whole or in part 
to any party who has not been licensed for use of the
QUALCOMM CDMA technologies. 

FILE:  OEMDiag.h

SERVICES:  AEE OEM Diag Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
QC's Diagnostics software.


        Copyright (©) 2004, 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMDiag.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/07   jas     Removing unused include file
12/04/04   sada    Rerouted APP_GetActiveDlgID to Field Debug APP to support BREW 3.1
10/25/04   sada    Adding Get_DiagVersion function
10/14/04   sada    Rerouted APP_SEND_EVENT to Field Debug APP to support BREW 3.1
===========================================================================*/

#include "AEEDiag.h"

/************************************************************************
 *
 * Constant definitions
 *
 ***********************************************************************/
#define DIAG_MAJOR_VERSION 2
#define DIAG_MINOR_VERSION 1

/************************************************************************
 *
 * Common IDiag type definitions
 *
 ***********************************************************************/

/* OEMDiagResponseHdrData contains all the data that needs to be
 * stored for the diagnostic response header
 *  SubsysId is the subsystem ID from the diagnostic request
 *  SubsysCmdCode is the subsystem command code from the diagnostic request
 *  DelayedRspId is the "delayed response ID" value that is used for the
 *     diagnostic responses
 *  NextRspCount is the "response count" value that will be sent in the
 *     header of the next diagnostic response
 */
typedef struct
{
    diagpkt_subsys_id_type              SubsysId;
    diagpkt_subsys_cmd_code_type        SubsysCmdCode;
    diagpkt_subsys_delayed_rsp_id_type  DelayedRspId;
    diagpkt_subsys_rsp_cnt              NextRspCount;
} OEMDiag_ResponseHdrData;


/* OEMDiag_RequestData is a structure used to store data associated with
 * a specific instance of a diagnostic request.
 *  TransactionId is an ID assigned to a specific request.  The app
 *     obtains the transaction ID via a function call and uses it to
 *     access request payload and to send delayed responses.
 *  AppId is the app class ID sent in the event to the app
 *  EventId is the event ID sent in the event to the app
 *  wp is the word parameter sent in the event to the app
 *  dwp is the double word parameter sent in the event to the app
 *  HdrData stores the header data needed for constructing diagnostic responses
 *  PayloadSize is the size of the stored request payload data, if any
 *  Payload is a pointer to the stored request payload data, if any
 *  TimestampMS is a millisecond-resolution timestamp indicating when
 *     the diagnostic request was received.
 */
typedef struct
{
    uint16                   TransactionId;
    AEECLSID                 AppId;
    AEEEvent                 EventId;
    uint16                   wp;
    uint32                   dwp;
    OEMDiag_ResponseHdrData *HdrData;
    uint16                   PayloadSize;
    uint8                   *Payload;
    uint32                   TimestampMS;
} OEMDiag_RequestData;

/* We store request data in a doubly-linked list to facilitate
 * searches and insertion/deletion.  Note that all the data is at
 * the head of the node.  Consequently, the node can be cast as
 * the data type for users, so that they cannot see the pointer members.
 * The link functions, however, can cast the pointer as a node
 * pointer to access the pointer members.
 */
typedef struct OEMDiag_RequestNode
{
    OEMDiag_RequestData             sReqData;
    struct OEMDiag_RequestNode     *pPrev;
    struct OEMDiag_RequestNode     *pNext;
} OEMDiag_RequestNode;

struct IDiag
{

    AEEVTBL(IDiag)      *pvt;            // Vector table of IDIAG methods
    IShell              *m_pIShell;      // Pointer to the IShell object
    uint32               m_uRefs;        // Reference count
    OEMDiag_RequestNode *m_pReqData;     // Data for outstanding diag requests
    rex_crit_sect_type   m_CritSect;     // Critical section for access to
                                         //   data structures

};

/* DiagRegEntry is the registration structure for an app.  This structure
 * is stored for every registered app and used in the implementation
 * of some of the diags.
 *  AppId is the class ID of the registering app
 *  AppContext is a pointer to the application's context
 *  AppVersion is the registered application's version
 *  NumRegEvents is the number of events the app has registered
 *  RegEvents is list of registered events, with their attributes
 */
typedef struct
{
    AEECLSID        AppId;
    PACONTEXT       AppContext;
    AECHAR          AppVersion[MAX_APP_VERSION_SIZE];
    uint16          NumRegEvents;
    IDiagEvtReg    *RegEvents;
} DiagRegEntry;

/* DiagRegNode is the node structure used to implement a linked list
 * of app registration records.
 */
typedef struct DiagRegNode
{
    DiagRegEntry         Data;
    struct DiagRegNode  *Next;
} DiagRegNode;

/* DiagRegTable is the type for the registration table.  It
 * is simply an unsorted linked list of registration nodes.
 */
typedef DiagRegNode *DiagRegTable;

/* OEMDIAG_CommandsType defines all of the subsystem command
 * codes used for application subsystem diagnostics
 */
typedef enum
{
  OEMDIAG_GET_ID = 0,
  OEMDIAG_START = 1,
  OEMDIAG_STOP = 2,
  OEMDIAG_SUSPEND = 3,
  OEMDIAG_RESUME = 4,
  OEMDIAG_SEND_EVENT=5,
  OEMDIAG_GET_BREW_VERSION=6,
  OEMDIAG_GET_ACTIVE_DLG_UNUSED = 7,  // Get the active dialog - Moved to FieldDebugApp
  OEMDIAG_GET_CTL_TEXT = 8,      // Get text from a control
  OEMDIAG_SELECT_MENU_ITEM = 9,  // Select a menu item
  OEMDIAG_SEND_EVENT_DELAY = 10, // General diag to be routed to app
  OEMDIAG_GET_APP_VERSION = 11,  // Get application version
  OEMDIAG_GET_DIAG_VERSION = 12, // Get the diag version
  OEMDIAG_MAX_CMD
} OEMDIAG_CommandsType;

/* OEMDIAG_CommandStatusType is a simple command status
 * enumeration used for some of the diags.
 */
typedef enum
{
    OEMDIAG_CMD_STATUS_OK = 0,
    OEMDIAG_CMD_STATUS_ERROR = 1
} OEMDIAG_CommandStatusType;

/************************************************************************
 *
 * Diagnostic Request/Response Definitione
 *   This section contains the definitions of the request and
 *   response structures used for each application subsystem
 *   diagnostic
 *
 ***********************************************************************/
/*============================================================================

PACKET   OEMDiag_CmdReqType

ID       N/A

PURPOSE  A template for the common part of several requests.  This is
         not a complete request structure, only a template.

RESPONSE N/A
       
============================================================================*/
typedef PACKED struct {
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
} OEMDiag_CmdReqType;


/*============================================================================

PACKET   OEMDiag_GetAppIdReqType

ID       APP_GET_ID

PURPOSE  Sent to DMSS, requesting the APPCLSID of the application.

RESPONSE The DM sends the name of the application for which the APPCLSID needs
         to be known. The ID thus returned is used in APP_START, APP_STOP
         APP_SUSPEND and APP_RESUME commands. 
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  char                           AppName[1];
} OEMDiag_GetAppIdReqType;

/*============================================================================

PACKET   OEMDiag_StartReqType

ID       APP_START

PURPOSE  Sent to DMSS, requesting starting a BREW app with optional args

RESPONSE Upon request DMSS starts the app.
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint32                         ArgLen;
  char                           Args[1];
} OEMDiag_StartReqType;

/*============================================================================

PACKET   OEMDiag_CmdStatusRspType

ID       APP_START, APP_STOP, APP_SUSPEND, APP_RESUME

PURPOSE  Sent by DMSS, reporting the status the command issued to the BREW
         app.

RESPONSE When this diag module receives a command, this forwards the command
         to the BREW ISHELL API. The API returns code for the command
         (for e.g. IShellStartApplet). This return code is returned in 
         shell_code field. 
       
/============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint8                          CmdStatus;
  uint32                         ShellCode;
} OEMDiag_CmdStatusRspType;

/*============================================================================

PACKET   OEMDiag_GetDiagVersionReqType

ID       OEMDIAG_GET_DIAG_VERSION

PURPOSE   Sent to DMSS, requesting the BREW version.

RESPONSE  OEMDiag_GetDiagVersionRspType  
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type    Hdr;
} OEMDiag_GetDiagVersionReqType;

/*============================================================================

PACKET   OEMDiag_GetDiagVersionRspType

ID       OEMDIAG_GET_DIAG_VERSION

PURPOSE  Sent by DMSS, reporting version of BREW

REQUEST  OEMDiag_GetDiagVersionReqType
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type    Hdr;
  uint16                        MajorVersion;
  uint16	                MinorVersion;	
} OEMDiag_GetDiagVersionRspType;

/*============================================================================

PACKET   OEMDiag_CmdGetBrewVersionReqType

ID       OEMDIAG_GET_BREW_VERSION

PURPOSE   Sent to DMSS, requesting the BREW version.

RESPONSE  OEMDiag_GetBrewVersionRspType  
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type    Hdr;
} OEMDiag_GetBrewVersionReqType;

/*============================================================================

PACKET   OEMDiag_GetBrewVersionRspType

ID       OEMDIAG_GET_BREW_VERSION

PURPOSE  Sent by DMSS, reporting version of BREW

REQUEST  OEMDiag_GetBrewVersionReqType
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint8                          VersionString[1];
} OEMDiag_GetBrewVersionRspType;

/*============================================================================

PACKET   OEMDiag_GetCtlTextReq
         OEMDiag_GetCtlTextRsp

ID       OEMDIAG_GET_CTL_TEXT

PURPOSE  Extract the text from an ITextCtl control

RESPONSE The DM sends the IDs of an application, dialog and text control;
         the response contains the text contained in that control.
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint16                         DlgId;
  int16                          CtlId;
} OEMDiag_GetCtlTextReq;

/* "text" is the first byte of the text string.  The response packet
 * is allocated to be longer than this structure and remaining bytes
 * of the text string follow the "text" member.
 */
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         Error;
  char                           Text[1];
} OEMDiag_GetCtlTextRsp;

/*============================================================================

PACKET   OEMDiag_SelectMenuItemReq
         OEMDiag_SelectMenuItemRsp

ID       OEMDIAG_SELECT_MENU_ITEM

PURPOSE  Select ("press") an item/key in a menu

RESPONSE The DM sends the IDs of an application, dialog, menu control and
         menu item; the response only indicates success or failure.
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint16                         DlgId;
  int16                          CtlId;
  uint16                         KeyId;
} OEMDiag_SelectMenuItemReq;

typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         Error;
} OEMDiag_SelectMenuItemRsp;

/*============================================================================

PACKET   OEMDiag_SendEventDelayReq
         OEMDiag_SendEventDelayReq_EventOnly
         OEMDiag_SendEventDelayRspHdr

ID       OEMDIAG_SENDEVENTDELAY

PURPOSE  Routes an event to an application to perform an application-
         specific diagnostic.

         OEMDiag_SendEventDelayReq_EventOnly defines the full message,
         including the payload format, for requests that do not require
         the interface to store the payload data.

RESPONSE The DM sends the request.  The first response is sent by
         the diag handler and includes the same payload as the request.
         The event is sent to the app, which is responsible for
         sending any subsequent response(s).
       
============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint16                         EventId;
  uint8                          Payload[1];  // First byte of payload data
} OEMDiag_SendEventDelayReq;

typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
  uint16                         EventId;
  uint16                         wp;
  uint32                         dwp;
} OEMDiag_SendEventDelayReq_EventOnly;

/* The following macro can be used to calculate the amount of payload
 * in a request packet, based on the total packet size.  It starts with the
 * packet size and subtracts the size of all fields except the Payload field.
 */
#define SEND_EVENT_REQ_PAYLOAD_SIZE(pkt_sz) \
  ((pkt_sz) - sizeof(diagpkt_subsys_header_type) - sizeof(uint32) - \
   sizeof(uint16))

typedef PACKED struct
{
  diagpkt_subsys_header_v2_type  Hdr;
  uint32                         AppId;
  uint16                         EventId;
  uint8                          Payload[1];  // First byte of payload data
} OEMDiag_SendEventDelayRspHdr;

/*============================================================================

PACKET   OEMDiag_GetAppVersionReq
         OEMDiag_GetAppVersionRsp

ID       OEMDIAG_GET_APP_VERSION

PURPOSE  Obtain the application's version information from the
         diag registration data.

RESPONSE The DM sends the request; the response contains the app version

============================================================================*/
typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         AppId;
} OEMDiag_GetAppVersionReq;

typedef PACKED struct
{
  diagpkt_subsys_header_type     Hdr;
  uint32                         Error;
  char                           AppVersion[MAX_APP_VERSION_SIZE];
} OEMDiag_GetAppVersionRsp;

#endif //OEMDIAG_H
