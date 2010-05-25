#ifndef __SS_OPERATIONS_APP_H__
#define __SS_OPERATIONS_APP_H__

/*=============================================================================
FILE: SSOperations_App.h

SERVICES: 
    GSM1x SS Operations Applet

GENERAL DESCRIPTION:

    The GSM1x SS Operations Applet (for short, the SSO) consists of the 
    following parts:

    1.	Operation FSM - This is the Finite State Machine that processes and 
        exchanges signaling messages with the network. 

    2.	Operation SSUI - This is the user interface of the applet. It uses 
        BREW dialogs to interact with the user. 

    3.	Call Database - Stores call related information to be used by both 
        the SSUI and FSM.

    The main event handler of the applet waits for events to arrive from the 
    BREW-AEE layer. Upon the reception of an event, the event is forwarded to 
    the SSUI for processing. The SSUI indicates whether or not it have 
    processed the event. If the SSUI did not process the event, the event is 
    forwarded to the FSM for processing. 

    At run time, the SSUI interacts with the FSM. This interaction is 
    performed by exchanging BREW events over the BREW-AEE layer, and by 
    accessing the Call Database. 

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"              // Standard AEE Declarations
#include "SSOperations_AEEAppGen.h"        // AEEApplet definitions
#include "SSUtil.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsFSM;
struct _SSCallDatabase;
struct _CSSOperationsDialogHandler;
struct IGSM1xSig;

/*===========================================================================

                              CLASS DEFINITIONS

===========================================================================*/


/* 
    CSSOperationsApp
    ----------------
    The CSSOperationsApp structure contains applet data. This is the Applet 
    structure that will be allocated for the applet. Its main parts were 
    depicted in the file header.

*/
typedef struct _CSSOperationsApp
{
    AEEApplet a; // Mandatory AEEApplet data member.

    struct _SSCallDatabase               *m_pCallDatabase;
    struct _CSSOperationsDialogHandler   *m_pDialogHandler;
    struct _CSSOperationsFSM             *m_pFSM;

    ISSUtilCls *m_pSSUtilCls; // SS utilities class

    struct IGSM1xSig* m_pIGSM1xSig; // GSM1x Signalling
              
    boolean m_AppStarted; // applet received EVT_APP_START

} CSSOperationsApp;



/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

int 
SSOperationsApp_InitAppData(CSSOperationsApp *pMe);

void 
SSOperationsApp_FreeAppData(CSSOperationsApp* pMe);

boolean 
SSOperationsApp_HandleEvent(
    CSSOperationsApp *pMe, 
    AEEEvent          eCode,
    uint16            wParam, 
    uint32            dwParam
    );


#endif // end __SS_OPERATIONS_APP_H__
