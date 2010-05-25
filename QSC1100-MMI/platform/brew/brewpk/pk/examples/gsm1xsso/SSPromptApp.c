/*===========================================================================
FILE: SSPromptApp.c

SERVICES: GSM1x SS Operations - Prompt Applet

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
#include "AEEStdLib.h"

#include "SSPromptApp.h"
#include "SSOperations_ClassId.h"

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================

FUNCTION:  SSPromptApp_InitAppData()

DESCRIPTION:
    This function initializes the app specific data.
    It is called from the module's AEEClsCreateInstance, 
    in file SSOperations.c.

PARAMETERS:
    pMe: Pointer to the applet structure, a CSSPromptApp.

RETURN VALUE:
    TRUE If successful, FALSE if failed

==============================================================================*/
int SSPromptApp_InitAppData(CSSPromptApp* pMe)
{
    if(!pMe)
        return EFAILED;

    pMe->m_msgStr[0]  = 0; 
    pMe->m_AppTimeout = 2000;
    pMe->m_AppCanRun  = FALSE;
    pMe->m_AppStarted = FALSE;
    pMe->m_pStaticMsg = NULL;
    
    if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC, 
            (void**)&pMe->m_pStaticMsg) != SUCCESS)
    {
        return EFAILED;
    }

    return SUCCESS;
}

/*===========================================================================

FUNCTION:  SSPromptApp_FreeAppData()

DESCRIPTION:
    This function frees the app data. This function is registered with the
    applet framework when the applet is created (inside AEEClsCreateInstance() function).
    This function is called by the app framework when the reference count of the 
    applet reaches zero. This function must free all the app data that has been
    allocated by this app. For ex: if their is data that was remembered when
    the app is suspended and resumed, those data must be freed here.

PARAMETERS:
    pMe: Pointer to the applet structure. This is of the type defined
        to store applet data. (CSSPromptApp*, in this case).

RETURN VALUE:
    None

==============================================================================*/
void SSPromptApp_FreeAppData(CSSPromptApp* pMe)
{
    if (pMe->m_pStaticMsg != NULL)
    {
        (void) ISTATIC_Release(pMe->m_pStaticMsg);
        pMe->m_pStaticMsg = NULL;
    }
}

/*=============================================================================
FUNCTION: SSPromptApp_TimerCB

DESCRIPTION:
   A callback function, that closes the applet when a timer expires.
   
PARAMETERS:
   pUserData: assumed to be pointer to a the applet structure.

RETURN VALUE:
    None

=============================================================================*/

static void SSPromptApp_TimerCB(void *pUserData)
{
    CSSPromptApp* pMe = (CSSPromptApp*) pUserData;

    ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
}

/*=============================================================================
FUNCTION: SSPromptApp_DisplayMessage

DESCRIPTION:
    Display a message string, using the applet's static text object.
   
PARAMETERS:
    pMe:    The applet structure.
    msgStr: String to display on screen.

RETURN VALUE:
    None

=============================================================================*/
static void SSPromptApp_DisplayMessage(CSSPromptApp* pMe, AECHAR* msgStr)
{
    AEEDeviceInfo di;
    AEERect cMenuFrame;
    
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &di);
    SETAEERECT(&cMenuFrame, 0, 0, di.cxScreen, di.cyScreen);
    ISTATIC_SetRect(pMe->m_pStaticMsg, &cMenuFrame);
    
    ISTATIC_SetProperties(pMe->m_pStaticMsg, ST_CENTERTEXT | ST_MIDDLETEXT);
    
    (void) ISTATIC_SetText(pMe->m_pStaticMsg, NULL, msgStr, AEE_FONT_NORMAL, AEE_FONT_BOLD);
    (void) ISTATIC_Redraw(pMe->m_pStaticMsg);
}

/*===========================================================================

FUNCTION: SSPromptApp_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled in this
    function.

PARAMETERS:
    pMe: Pointer to the AEEApplet structure. This structure contains information specific
        to this applet. It was initialized during the AEEClsCreateInstance() function.
    eCode: Specifies the Event sent to this applet
    wParam, dwParam: Event specific data.

RETURN VALUE:
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

===========================================================================*/
boolean SSPromptApp_HandleEvent(
    CSSPromptApp *pMe,
    AEEEvent     eCode, 
    uint16       wParam, 
    uint32       dwParam
    )
{
    switch (eCode) 
    {
    case EVT_APP_START:
    case EVT_APP_RESUME:
        if (pMe->m_AppCanRun == FALSE)
        {
            return FALSE;
        }

        pMe->m_AppStarted = TRUE;
        SSPromptApp_DisplayMessage(pMe, pMe->m_msgStr);

        CALLBACK_Init(&pMe->m_TimerCB, SSPromptApp_TimerCB, (void*)pMe);

        (void) ISHELL_SetTimerEx(
                  pMe->a.m_pIShell,
                  pMe->m_AppTimeout,
                  &pMe->m_TimerCB
                  );
        return TRUE;

    case EVT_APP_STOP:
        CALLBACK_Cancel(&pMe->m_TimerCB); // cancel timer callback

        return TRUE;

    case EVT_APP_SUSPEND:
        return TRUE; // must return TRUE, or the applet will quit.

    case EVT_SS_PROMPT_UPDATE:
        {
            uint16 timeout = wParam;
            AECHAR* pMsgStr = (AECHAR*)dwParam;

            if (pMsgStr == NULL)
                return TRUE;
            
            WSTRLCPY(pMe->m_msgStr, pMsgStr, SS_PROMPT_MAX_MSG_LENGTH);
            pMe->m_AppTimeout = timeout;
            pMe->m_AppCanRun = TRUE;
            
            if (pMe->m_AppStarted)
            {
                SSPromptApp_DisplayMessage(pMe, pMe->m_msgStr);
                CALLBACK_Init(&pMe->m_TimerCB, SSPromptApp_TimerCB, (void*)pMe);

                (void) ISHELL_SetTimerEx(
                          pMe->a.m_pIShell,
                          pMe->m_AppTimeout,
                          &pMe->m_TimerCB
                          );
            }
            else
            {
                ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_SS_PROMPTAPP);
            }
        }
        return TRUE;
    }

    return FALSE;
}
