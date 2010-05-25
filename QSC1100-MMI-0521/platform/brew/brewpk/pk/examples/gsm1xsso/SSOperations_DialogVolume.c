/*===========================================================================
FILE: SSOperations_DialogVolume.c

SERVICES: SS Operations - Dialog for volume control

GENERAL DESCRIPTION:
    
PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                              INCLUDE FILES
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"
#include "SSOperations_UserEvents.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// Time in milliseconds before the Call Volume dialog will close
#define TIMEOUT_MS_CALLVOLUME_TIMER            (2*TIMEOUT_MS_ONE_SECOND)

/*===========================================================================
                              MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

static void SSOperationsDialog_CallVolumeUpdateDisplay(
    CSSOperationsDialogHandler *pSelf
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_CallVolHandleEvent

DESCRIPTION: Call Volume dialog event handler (IDD_CALLVOLUME)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_CallVolHandleEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
        {
            (void) SS_PostEvent(
                      pSelf->m_pShell,
                      AEECLSID_SS_OPERATIONS,
                      EVT_USER_DRAW,
                      0,
                      0
                      );

            // Start timer
            CALLBACK_Init(
               &pSelf->m_UIDialogTimerCB, 
               SSOperationsDialog_HandleDialogTimeout, 
               (void*)pSelf
               );

            (void) ISHELL_SetTimerEx(
                      pSelf->m_pShell,
                      TIMEOUT_MS_CALLVOLUME_TIMER,
                      &pSelf->m_UIDialogTimerCB
                      );
            return TRUE;
        }

        case EVT_USER_DRAW:
        {
            SSOperationsDialog_CallVolumeUpdateDisplay(pSelf);
            return TRUE;
        }

        case EVT_USER_DIALOG_TIMEOUT:
        {
            SETUP_NEW_UI_DIALOG( GET_VOLUME_PREV_DIALOG_SETUP );
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
            CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback
            return TRUE;
        }

        case EVT_KEY:
            switch ((AVKType)wParam) 
            {
                case AVK_UP:
                case AVK_VOLUME_UP:
                    SSOperationsDialog_ChangeCallVolume(pSelf, TRUE);
                    break;

                case AVK_DOWN:
                case AVK_VOLUME_DOWN:
                    SSOperationsDialog_ChangeCallVolume(pSelf, FALSE);
                    break;

                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                case AVK_STAR:
                case AVK_POUND:
                case AVK_CLR:
                    SETUP_NEW_UI_DIALOG( GET_VOLUME_PREV_DIALOG_SETUP );
                    return TRUE;

                default:
                    return FALSE;
            }

            // Start the timer over again
            (void) ISHELL_SetTimerEx(
                      pSelf->m_pShell,
                      TIMEOUT_MS_CALLVOLUME_TIMER,
                      &pSelf->m_UIDialogTimerCB
                      );

            SSOperationsDialog_CallVolumeUpdateDisplay(pSelf);
            return TRUE;
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSOperationsDialog_CallVolHandleNonDisplayEvent

DESCRIPTION: Call Volume non dialog event handler (IDD_CALLVOLUME)

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    eCode: BREW event code
    wParam:
    dwParam:

RETURN VALUE:
    boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
static boolean SSOperationsDialog_CallVolHandleNonDisplayEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    )
{
    switch (eCode)
    {
        case UIMN_SETUP_IND:
        {
            // change the current dialog setup handler to be the previous one
            // so that the incoming dialog will return to the "Previous" dialog and 
            // not to the volume dialog
            pSelf->m_pDialogSetupHandler = GET_VOLUME_PREV_DIALOG_SETUP;

            SSOperationsDialog_MoveToTheIncomingDialog(
                pSelf, 
                (SSEventData*)dwParam
                );
            return TRUE;
        }
    }

    return FALSE;
}

/*===========================================================================
FUNCTION: SetupCallVolumeDialog

DESCRIPTION:
    This function creates the Call Volume dialog

PARAMETERS:
    *pSelf: Pointer to the dialog handler object
     createNewDialog: specifies whether to create a new displayed dialog

RETURN VALUE:
    None

===========================================================================*/
void SSOperationsDialog_SetupCallVolumeDialog(
    CSSOperationsDialogHandler *pSelf,
    boolean                     createNewDialog
    )
{
    if (createNewDialog)
    {
        SET_DISPLAY_DIALOG_HANDLER( SSOperationsDialog_CallVolHandleEvent );
        SHOW_DIALOG( IDD_CALLVOLUME );
    }

    SET_NON_DISPLAY_EVENT_HANDLER( 
        SSOperationsDialog_CallVolHandleNonDisplayEvent 
        );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_CallVolumeUpdateDisplay

DESCRIPTION: update the Call Volume dialog's display

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_CallVolumeUpdateDisplay(
    CSSOperationsDialogHandler *pSelf
    )
{
    uint16  img;
    IImage *pLevel;
    AECHAR  szBuf[16];

    switch (pSelf->m_CallVolume) 
    {
    case SS_1ST_VOLUME:
        img = IDB_VOLUME_1;
        break;

    case SS_2ND_VOLUME:
        img = IDB_VOLUME_2;
        break;

    case SS_3RD_VOLUME:
        img = IDB_VOLUME_3;
        break;

    case SS_4TH_VOLUME:
        img = IDB_VOLUME_4;
        break;

    case SS_5TH_VOLUME:
        img = IDB_VOLUME_5;
        break;

    case SS_6TH_VOLUME:
        img = IDB_VOLUME_6;
        break;

    case SS_MAX_VOLUME:
        img = IDB_VOLUME_7;
        break;

    default:
        assert(!"Unknown call volume value");
        return;
    }

    IDISPLAY_ClearScreen(pSelf->m_pDisplay);
    szBuf[0] = 0;

    (void) ISHELL_LoadResString(pSelf->m_pShell,
                                SSOPERATIONS_RES_FILE,
                                IDS_VOLUME_TITLE,
                                szBuf,
                                sizeof(szBuf));

    pLevel = ISHELL_LoadResImage(pSelf->m_pShell,
                                 SSOPERATIONS_RES_FILE,
                                 img);
    if (pLevel) 
    {
        (void) IDISPLAY_DrawText(pSelf->m_pDisplay,
                                 AEE_FONT_NORMAL,
                                 (AECHAR*)szBuf,
                                 -1,
                                 0,
                                 0,
                                 NULL,
                                 IDF_ALIGN_LEFT);
        IIMAGE_Draw( pLevel, 0, 17);
        IIMAGE_Release( pLevel );
    }

    IDISPLAY_Update(pSelf->m_pDisplay);
}

/*=============================================================================
FUNCTION: SSOperationsDialog_ChangeCallVolume

DESCRIPTION: Increase or decreases the current call volume

PARAMETERS:
   *pSelf: CSSOperationsDialogHandler object pointer
    louder: TRUE to increase the call volume
            FALSE to decrease the call volume

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_ChangeCallVolume(
    CSSOperationsDialogHandler *pSelf,
    boolean                     louder
    )
{
    uint16 normalizedVolume; 

    if (louder) 
    {
        switch (pSelf->m_CallVolume) 
        {
        case SS_1ST_VOLUME:
            pSelf->m_CallVolume = SS_2ND_VOLUME;
            break;

        case SS_2ND_VOLUME:
            pSelf->m_CallVolume = SS_3RD_VOLUME;
            break;

        case SS_3RD_VOLUME:
            pSelf->m_CallVolume = SS_4TH_VOLUME;
            break;

        case SS_4TH_VOLUME:
            pSelf->m_CallVolume = SS_5TH_VOLUME;
            break;

        case SS_5TH_VOLUME:
            pSelf->m_CallVolume = SS_6TH_VOLUME;
            break;

        case SS_6TH_VOLUME:
        case SS_MAX_VOLUME:
            pSelf->m_CallVolume = SS_MAX_VOLUME;
            break;

        default:
            assert(!"Unknown call volume value");
            return;
        }
    } 
    else 
    {
        switch (pSelf->m_CallVolume) 
        {

        case SS_1ST_VOLUME:
        case SS_2ND_VOLUME:
            pSelf->m_CallVolume = SS_1ST_VOLUME;
            break;

        case SS_3RD_VOLUME:
            pSelf->m_CallVolume = SS_2ND_VOLUME;
            break;

        case SS_4TH_VOLUME:
            pSelf->m_CallVolume = SS_3RD_VOLUME;
            break;

        case SS_5TH_VOLUME:
            pSelf->m_CallVolume = SS_4TH_VOLUME;
            break;

        case SS_6TH_VOLUME:
            pSelf->m_CallVolume = SS_5TH_VOLUME;
            break;

        case SS_MAX_VOLUME:
            pSelf->m_CallVolume = SS_6TH_VOLUME;
            break;

        default:
            assert(!"Unknown call volume value");
            return;
        }
    }

    normalizedVolume = (pSelf->m_CallVolume*AEE_MAX_VOLUME) / SS_MAX_VOLUME;
    assert(normalizedVolume <= AEE_MAX_VOLUME);

    ISOUND_SetVolume(pSelf->m_pSound, normalizedVolume);
}
