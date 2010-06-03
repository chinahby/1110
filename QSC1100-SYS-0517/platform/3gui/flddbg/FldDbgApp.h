#ifndef FLDDBGAPP_H
#define FLDDBGAPP_H

/*=============================================================================

FILE: FldDbgApp.h

SERVICES: FieldDebug Applet

GENERAL DESCRIPTION: Main applet functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/FldDbg/FldDbgApp.h#4 $
$DateTime: 2005/10/19 10:29:05 $
$Author: skota $
$Change: 252940 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/15/05   sada    Request & response payload defns for new diags: Get Active Applet, Get App Name & Get Selected Menu Ctl Text
12/04/04   sada    Rerouted APPGetActiveDlgIf to Field Debug App to support Brew 2.1     
10/14/04   sada    Rerouted APP_SEND_EVENT to Field Debug APP to support BREW 3.1
08/30/04   JAS     Created Field Debug Header File

=============================================================================*/
/*=============================================================================

                       INCLUDES

=============================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#endif
#include "BREWVersion.h"


/*=============================================================================

                       CONSTANTS

=============================================================================*/
// Version number for Field Debug App
#define FLDDBG_SW_VERSION "1.2"


/*=============================================================================

                       FIELD DEBUG APP USER EVENTS

=============================================================================*/
/*
 * Event values for diagnostic events
 */
#ifdef FEATURE_APP_TEST_AUTOMATION
#define EVT_FLDDBG_DIAG_START_DIAGS           (EVT_USER + 0x100)
// App Get ID Diagnostic
#define EVT_FLDDBG_DIAG_APP_GET_ID            (EVT_FLDDBG_DIAG_START_DIAGS + 1)
// App Start Diagnostic
#define EVT_FLDDBG_DIAG_APP_START             (EVT_FLDDBG_DIAG_START_DIAGS + 2)
// App Stop Diagnostic
#define EVT_FLDDBG_DIAG_APP_STOP              (EVT_FLDDBG_DIAG_START_DIAGS + 3)
// Send Event 
#define EVT_FLDDBG_DIAG_APP_SEND_EVENT        (EVT_FLDDBG_DIAG_START_DIAGS + 4)
//Get ActiveDlgId event
#define EVT_FLDDBG_DIAG_GETACTIVEDLG          (EVT_FLDDBG_DIAG_START_DIAGS + 5)
//Get selected menu item text
#define EVT_FLDDBG_DIAG_GETSELMENUCTLTXT     (EVT_FLDDBG_DIAG_START_DIAGS + 6)
// App Get Name Diagnostic
#define EVT_FLDDBG_DIAG_APP_GET_NAME          (EVT_FLDDBG_DIAG_START_DIAGS + 7)
// App Get active applet
#define EVT_FLDDBG_DIAG_GETACTIVEAPPLET       (EVT_FLDDBG_DIAG_START_DIAGS + 8)       


#if !MIN_BREW_VERSION(3, 1)
// Event for closing applets before BREW 3.1
#define EVT_FLDDBG_DIAG_APP_CLOSE_COMPLETE    (EVT_USER + 0x200)
#endif
// Event for finishing the Get ActiveDlgID evt
#define EVT_FLDDBG_DIAG_GETACTIVEDLG_COMPLETE (EVT_USER + 0x201)  	
// Event for finishing the get selected menu ctl text evt
#define EVT_FLDDBG_DIAG_GETSELMENUCTLTXT_COMPLETE     (EVT_USER + 0x202)

// Max size of app name for diags
#define FLDDBG_DIAG_MAX_APPNAME_LEN  256
// Max size of file name for diags
#define FLDDBG_DIAG_MAX_FILENAME_LEN  256

// The largest transaction ID; need to explicitly rollover if we hit this
#define FLDDBG_DIAG_MAX_TRANSACTION_ID 0xFFFF

// Clipbaord max name length 
#define CLIPBOARD_NAME_MAX_LENGTH 15

// Callback tiemr value: 2 secs
#define FLDDBG_DIAG_TIMERVALUE 2000

#endif // FEATURE_APP_TEST_AUTOMATION

#define MAX_RES_SIZE    64   /* Size of a resource file string. */

/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION

/************************************************
 *  App Get Id
 ***********************************************/

// Diag request payload for App Get Id diagnostic
typedef struct
{
  char AppName[FLDDBG_DIAG_MAX_APPNAME_LEN];
} CFldDbg_GetAppIdReq;

// Diag response structure for App Get Id diagnostic
typedef struct
{
  uint32 AppId;
} CFldDbg_GetAppIdRsp;

/************************************************
 *  App Get Name
 ***********************************************/

// Diag request payload for App Get Name diagnostic
typedef struct
{
  uint32 AppId;
} CFldDbg_GetAppNameReq;

// Diag response payload for App Get Name diagnostic
typedef struct
{
  char AppName[FLDDBG_DIAG_MAX_APPNAME_LEN];
} CFldDbg_GetAppNameRsp;

/************************************************
 *  Get Active Applet
 ***********************************************/
// Diag response payload for Get Active Applet diagnostic
typedef struct       
{
  uint32 ActiveApplet;
} CFldDbg_GetActiveAppletRsp;


/************************************************
 *  Start App
 ***********************************************/

// Diag request payload for the Start App diagnostic
#define FLDDBG_MAX_START_ARG_LEN 300
typedef struct
{
  uint32  AppId;
  uint32  ArgLen;
  char    Args[FLDDBG_MAX_START_ARG_LEN];
} CFldDbg_StartAppReq;

// Diag response payload for the Start App diagnostic
typedef struct
{
  uint32  ShellCode;
} CFldDbg_StartAppRsp;

/************************************************
 *  Stop App
 ***********************************************/

// Diag request payload for the Stop App diagnostic
typedef struct
{
  uint32  AppId;
} CFldDbg_StopAppReq;

// Diag response payload for the Stop App diagnostic
typedef struct
{
  uint32  ShellCode;
} CFldDbg_StopAppRsp;

// Diag Request payload for Send Event diagnostic
typedef PACKED struct
{
  uint32                         AppId;
  uint32                         EventId;
  uint16                         DataLen;
  uint8                          Data[FLDDBG_DIAG_MAX_FILENAME_LEN];
} CFldDbg_CmdSendEventReqType;

// Diag Response payload for Send Event diagnostic
typedef PACKED struct
{
  uint32                         Status;
  uint16                         Reserved;
  uint32                         AppId;
  uint32                         EventId;
  uint16                         DataLen;
  uint8                          Data[FLDDBG_DIAG_MAX_FILENAME_LEN];
} CFldDbg_CmdSendEventRspType;

/*============================================================================

PACKET   CFieldDbg_GetActiveDlgReq
         CFieldDbg_GetActiveDlgRsp

ID       CFIELDDBG_GET_ACTIVE_DLG

PURPOSE  Obtain ID of current active dialog for the indicated app

RESPONSE The DM sends the ID of an application; the response gives
         the ID of the current active dialog.
       
============================================================================*/
typedef struct
{
  uint32  			AppId;
} CFldDbg_GetActiveDlgReqType;

typedef struct
{
  uint32                         DlgId;
} CFldDbg_GetActiveDlgRspType;

/*============================================================================

PACKET   CFieldDbg_GetMenuCtlTxtReq
         CFieldDbg_GetMenuCtlTxtRsp

ID       CFIELDDBG_GETSELMENUCTLTXT

PURPOSE  Obtain the text of current active menu item

RESPONSE The response gives the text from the current menu item.
       
============================================================================*/
typedef struct
{
  uint32        AppId;
  uint32        DlgId;
  uint32        CtlId;
} CFldDbg_GetMenuCtlTxtReqType;

typedef struct
{
  char          CtlText[1];
} CFldDbg_GetMenuCtlTxtRspType;
  
/* CFldDbg_CommandStatusType is a simple command status
 * enumeration used for some of the diags.
 */
typedef enum
{
    CFLDDBG_CMD_STATUS_OK = 0,
    CFLDDBG_CMD_STATUS_ERROR = 1
} CFldDbg_CommandStatusType;

#if ! MIN_BREW_VERSION(3, 1)
// Callback data for BREW 3.0.x and earlier
typedef struct
{
  IShell      *pIShell;            // Shell pointer
  uint16       XactId;             // Transaction ID
} CFldDbg_CloseAppCBData;

#if ! MIN_BREW_VERSION(3, 0) // For BREW 2.1 and earlier only
typedef struct
{
  PACONTEXT pNextCtxt;
} CFldDbg_AppCtxtCast;
#endif // ! MIN_BREW_VERSION(3, 0) 
#endif // ! MIN_BREW_VERSION(3, 1)

typedef struct
{
  IShell      *pIShell;            // Shell pointer
  uint16       XactId;             // Transaction ID
} CFldDbg_GetActiveDlgCBData;

typedef struct
{
  IShell      *pIShell;            // Shell pointer
  uint16       XactId;             // Transaction ID
  uint32       Error;              // Error
  uint32       DlgId;              // Dialog Id
  uint32       CtlId;              // Control ID
  char         CtlText[MAX_RES_SIZE + 1];
} CFldDbg_GetMenuCtlTxtCBData;

typedef struct
{
  IDiag         *pIDiag;          // Diag pointer
  uint16        XactId;           // Transaction ID
  uint32        nDiagEvtId;       // Evt Id
} CFldDbg_CallbackTimerData;

#endif // FEATURE_APP_TEST_AUTOMATION

#endif // FLDDBGAPP_H
