#ifndef SMSAgentApp_H
#define SMSAgentApp_H

/*=============================================================================
FILE: SMSAgentApp.h

SERVICES: SMS Agent Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
ISMSApp

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 QUALCOMM Incorporated. All Rights Reserved.
    QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES FOR MODULE
===========================================================================*/

#include "SMSAgent_ClassId.h"
#include "GSM1xSMSApp.h"


/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/


// SMS Agent applet class 
typedef struct SMSAgentApp
{
   AEEApplet      a;                            // Standard applet information
   IPhone         *m_pIPhone;                   // Phone
} SMSAgentApp;


/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
int SMSAgentApp_New(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    SMSAgentApp   **ppObj
    );

boolean SMSAgentApp_HandleEvent(
    IApplet *pMe,
    AEEEvent     eCode, 
    uint16       wParam, 
    uint32       dwParam
    );

// Various applet event handlers
boolean SMSAgentApp_HandleNotifyEvent(SMSAgentApp *pMe, AEENotify *pN);

int SMSAgentApp_InitAppData(IApplet *pMe);
void SMSAgentApp_FreeAppData(IApplet *pMe);

#endif // end SMSAgentApp_H
