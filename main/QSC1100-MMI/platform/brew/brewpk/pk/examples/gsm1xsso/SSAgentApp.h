#ifndef __SS_AGENT_APP_H__
#define __SS_AGENT_APP_H__

/*=========================================================================
FILE: SSAgentApp.h

SERVICES: Holds the SSUtil class singleton and keeps it alive

GENERAL DESCRIPTION:
    This is the declaration of the SSAgentApp applet.

    The SSAgentApp loads itself upon shell initialization by using the 
    NMASK_SHELL_INIT notification from IShell. After loading, SSAgentApp stays
    loaded and runs in the background.
    The reason for staying in the background are:
    - Keeping the SSUtil class singleton
    - Storing the first call details.

    For achieving that, we take the opportunity of handling an event
    notification, and "demand" to keep running in the background.

    The invoking event is an IPhone notification. To stay running, we add a 
    reference to the applet structure with IAPPLET_AddRef. As we finish 
    handling the IPhone notification, applet memory will not be released, 
    because of this additional reference.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    CSSAgentApp class

INITIALIZATION & SEQUENCING REQUIREMENTS:
    This applet must be the first one to create the SSUtil singleton.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"              // Standard AEE Declarations
#include "SSOperations_AEEAppGen.h"        // AEEApplet definitions
#include "SSUtil.h"
#include "SSAgent_ClassId.h"
#include "SSCallCommonDefinitions.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// This event is sent from the SSUtil class in order to solve the problem of 
// creating a new log file for use in a singleton.
// This is sent by an event in order for the running applet's context to
// change to the SSAgent applet. This is needed because only the SSAgent applet
// always remains up and can keep the file open (otherwise it is owned by the 
// running applet, and when it ends the file is forcibly cleaned up.
#define EVT_USER_SWAP_LOG_FILES_BY_CALLING_LOG (EVT_USER+1)

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/
#ifndef HACK_A_SHAQ_33
typedef struct _CSSAgentApp CSSAgentApp;
#define HACK_A_SHAQ_33
#endif

// Mandatory structure.
// Structure to contain app data. This is the Applet structure that will be allocated
// for the app. Define this structure if the app needs to store any data that needs
// to be remembered across different events or callbacks. The first element of this
// structure MUST be of type AEEApplet. This is extremely important. Following this
// element, store the app-specific data. Note that AEEApplet structure already exposes
// the most commonly needed interface pointers such as Shell, Display, etc. 
struct _CSSAgentApp
{
    AEEApplet   a;            // Mandatory AEEApplet data member appears first
    ISSUtilCls *m_pSSUtilCls; // SS utilities class

    // Storage for the first call info
    SSCall_Origination_type m_FirstCallType;
    SSCall_PI_type          m_FirstCallPi;
    char                    m_FirstCallCallString[MAX_SIZE_DIALER_TEXT];
    boolean                 m_FirstCallTypeAllowed;
    uint32                  m_FirstCallCallStartTime;
    boolean                 m_FirstCallDetailsExist;
};

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
int SSAgentApp_New(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    CSSAgentApp   **ppObj
    );

boolean SSAgentApp_HandleEvent(
    CSSAgentApp *pMe,
    AEEEvent     eCode, 
    uint16       wParam, 
    uint32       dwParam
    );

int SSAgentApp_InitAppData(CSSAgentApp* pMe);
void SSAgentApp_FreeAppData(CSSAgentApp* pMe);

/*=============================================================================
FUNCTION: SSAgentApp_SetFirstCallDetails

DESCRIPTION: Store the first call details

PARAMETERS:
    pIShell [in]         : Pointer to the Shell interface.
    type [in]            : call type, MO or MT
    pi [in]              : presentation indicator
    callString [in]      : pointer to a string that contains the call's number. 
                           the string is copied into a local buffer.
    callTypeAllowed [in] : indicates if this call is a voice or test call. The
                           SSO can be activated only if the first call is of 
                           these call types.
    startTime [in]       : call start time.

RETURN VALUE:
    None
=============================================================================*/
void SSAgentApp_SetFirstCallDetails(
    IShell *pIShell,
    SSCall_Origination_type  type,
    SSCall_PI_type           pi,
    const char               *callString,
    boolean                  callTypeAllowed,
    uint32                   startTime
    );

/*=============================================================================
FUNCTION: SSAgentApp_GetFirstCallDetails

DESCRIPTION: Get the first call details

PARAMETERS:
    pIShell [in]          : Pointer to the Shell interface.
    type [out]            : returns the call type, MO or MT
    pi [out]              : returns the presentation indicator
    callString [out]      : points to the call's number. The memory for this
                            string is owned by the Call Database; the caller
                            must not change or free it.
    callTypeAllowed [out] : returns an indication if this call is a voice or
                            test call. The SSO can be activated only if the 
                            first call is of these call types.
    startTime [out]       : returns the call start time.

RETURN VALUE:
    TRUE if a first call details exists, otherwise FALSE.
=============================================================================*/
boolean SSAgentApp_GetFirstCallDetails(
    IShell *pIShell,
    SSCall_Origination_type   *type,
    SSCall_PI_type            *pi,
    const char                **callString,
    boolean                   *callTypeAllowed,
    uint32                    *startTime
    );

#endif // end __SS_AGENT_APP_H__
