#ifndef __SS_PROMPT_APP_H__
#define __SS_PROMPT_APP_H__

/*=============================================================================
FILE: SSPromptApp.h

SERVICES: 
    GSM1x SS Operations - Prompt Applet

GENERAL DESCRIPTION:
    The Prompt applet displays messages for a limited period of time.
    It starts with some message. Then it might either time out and exit, 
    or receive a message update, that also resets the timer.

    Currently it only serves for displaying messages for the SSO applet,
    while the SSO applet is not the active applet. 

INITIALIZATION AND SEQUENCING REQUIREMENTS:
    The only entry point to the SS Prompt applet is by sending a
    EVT_SS_PROMPT_UPDATE event to it.
    If the applet is not running, it will start itself. Else, it will just 
    update the message string and refresh the display.

    The applet cannot run in the regular manner, by just receiving
    EVT_APP_START.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"              // Standard AEE Declarations
#include "SSOperations_AEEAppGen.h"        // AEEApplet definitions

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

#define SS_PROMPT_MAX_MSG_LENGTH 64

// sent to the SSPrompt applet to display a message.
// wParam is the timeout value, dwParam points to AECHAR string to display.
#define EVT_SS_PROMPT_UPDATE EVT_USER+0x200

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              CLASS DEFINITIONS
===========================================================================*/
typedef struct _CSSPromptApp
{
    AEEApplet a;    // Mandatory AEEApplet data member appears first

    boolean     m_AppStarted; // applet received EVT_APP_START
    IStatic*    m_pStaticMsg; // static text object, to be displayed
    AECHAR      m_msgStr[SS_PROMPT_MAX_MSG_LENGTH]; // string for m_pStaticMsg
    AEECallback m_TimerCB;    // timer callback
    uint32      m_AppTimeout; // timeout value for applet exit, in milliseconds.
    boolean     m_AppCanRun;  // applet is allowed to run.

} CSSPromptApp;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
boolean SSPromptApp_HandleEvent(
    CSSPromptApp *pMe,
    AEEEvent     eCode, 
    uint16       wParam, 
    uint32       dwParam
    );

int SSPromptApp_InitAppData(CSSPromptApp* pMe);
void SSPromptApp_FreeAppData(CSSPromptApp* pMe);

#endif // end __SS_PROMPT_APP_H__
