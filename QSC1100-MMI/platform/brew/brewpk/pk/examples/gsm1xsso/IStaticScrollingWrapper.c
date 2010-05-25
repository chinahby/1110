/*=========================================================================
FILE: IStaticScrollingWrapper.c

SERVICES: Allows ISTATIC text scrolling by a user defined timeout value..

GENERAL DESCRIPTION:
    A Wrapper object around an IStatic control, that allows text scrolling
    by a user defined timeout value.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    IStaticScroller

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "SS_CommonMacros.h"
#include "AEEStdLib.h"
#include "IStaticScrollingWrapper.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/
static void SSOperationsDialog_ScrollerTimerCB(void*);

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: IStaticScroller_New

DESCRIPTION: Create a IStaticScroller instance, that uses a IStatic control.

PARAMETERS:
    pShell [in]: pointer to a IShell instance

RETURN VALUE:
    pointer to the new IStaticScroller or NULL on MALLOC failure.

COMMENTS:
    In order to free IStaticScroller use IStaticScroller_Destroy().
=============================================================================*/
IStaticScroller* IStaticScroller_New(IShell *pShell)
{
    IStaticScroller* pSelf;
    assert(pShell != NULL);

    pSelf = MALLOC(sizeof(IStaticScroller));
    if (pSelf != NULL)
    {        
        pSelf->m_pShell = pShell;
        ISHELL_AddRef(pSelf->m_pShell);
    }

    return pSelf;
}

/*=============================================================================
FUNCTION: IStaticScroller_Destroy

DESCRIPTION: Frees a IStaticScroller instance

PARAMETERS:
   pSelf [in]: pointer to a valid IStaticScroller

RETURN VALUE:
    None
=============================================================================*/
void IStaticScroller_Destroy(IStaticScroller *pSelf)
{
    //assert(pSelf != NULL);

    // make sure that scrolling is stopped
    IStaticScroller_StopScrolling(pSelf);

    if (pSelf->m_pShell) ISHELL_Release(pSelf->m_pShell);
    if (pSelf->m_pStaticCtl) ISTATIC_Release(pSelf->m_pStaticCtl);

    FREE(pSelf);
}

/*=============================================================================
FUNCTION: IStaticScroller_SetIStatic

DESCRIPTION: Set the Static control for scrolling.

PARAMETERS:
    pSelf [in]: pointer to a valid IStaticScroller
    pStaticCtl [in]: pointer to a valid IStatic control.

RETURN VALUE:
    None.

=============================================================================*/
void IStaticScroller_SetIStatic(IStaticScroller *pSelf, IStatic *pStaticCtl)
{
    //assert(pSelf != NULL);

    if (pSelf->m_pStaticCtl != NULL)
    {
        // if removing a valid static control, make sure that scrolling is disabled
        if (pStaticCtl == NULL)
        {
            IStaticScroller_StopScrolling(pSelf);
        }

        // release prev control
        ISTATIC_Release(pSelf->m_pStaticCtl);
    }

    // set new control
    pSelf->m_pStaticCtl = pStaticCtl;

    if (pSelf->m_pStaticCtl != NULL)
    {
        ISTATIC_AddRef(pSelf->m_pStaticCtl);
    }
}

/*=============================================================================
FUNCTION: IStaticScroller_GetIStatic

DESCRIPTION: Returned the Static control that is used for scrolling.

PARAMETERS:
    None.

RETURN VALUE:
    pointer to the IStatic control.

=============================================================================*/
IStatic* IStaticScroller_GetIStatic(IStaticScroller *pSelf)
{
    //assert(pSelf != NULL);
    return pSelf->m_pStaticCtl;
}

/*=============================================================================
FUNCTION:IStaticScroller_StartScrolling

DESCRIPTION: Starts the IStatic scrolling (if it is scrollable)
             using a user specified timeout value that indicates how long
             to display each one of the IStatic's display lines.

PARAMETERS:
    pSelf [in]: pointer to a valid IStaticScroller
    msecs [in]: time in milliseconds. how long to display each scroll line.

RETURN VALUE:
    None.

COMMENTS:
    A valid IStatic control must already be set. (using IStaticScroller_SetIStatic)
=============================================================================*/
void IStaticScroller_StartScrolling(IStaticScroller *pSelf, uint32 msecs)
{
    //assert(pSelf != NULL);
    assert(pSelf->m_pStaticCtl != NULL);

    pSelf->m_timeout = msecs;
    pSelf->m_lineIndex = 0;

    // Disable IStatic's native scrolling
    ISTATIC_SetProperties(pSelf->m_pStaticCtl, ST_NOSCROLL);
    
    // Start dialog update timer
    CALLBACK_Init(
        &pSelf->m_UIDialogTimerCB, 
        SSOperationsDialog_ScrollerTimerCB, 
        (void*)pSelf
        );

    if (ISTATIC_IsScrollable(pSelf->m_pStaticCtl))
    {
        SSOperationsDialog_ScrollerTimerCB(pSelf);
    }
}

/*=============================================================================
FUNCTION: IStaticScroller_StopScrolling

DESCRIPTION: Stop the IStatic scrolling (if it is scrollable)

PARAMETERS:
    pSelf [in]: pointer to a valid IStaticScroller

RETURN VALUE:
    None.
=============================================================================*/
void IStaticScroller_StopScrolling(IStaticScroller *pSelf)
{
    //assert(pSelf != NULL);
    CALLBACK_Cancel(&pSelf->m_UIDialogTimerCB); // cancel timer callback
}

/*=============================================================================
FUNCTION: SSOperationsDialog_ScrollerTimerCB

DESCRIPTION: Timer callback used to update the current IStatic displayed
             text line.

PARAMETERS:
   *pUser: IStaticScroller object pointer

RETURN VALUE:
    None

=============================================================================*/
static void SSOperationsDialog_ScrollerTimerCB(
    void *pUser
    )
{
    IStaticScroller *pSelf = (IStaticScroller *)pUser;
    uint32 prevLineIndex;

    //assert(pSelf != NULL);

    ISTATIC_GoToLine(pSelf->m_pStaticCtl, pSelf->m_lineIndex);
    prevLineIndex = ISTATIC_GoToLine(pSelf->m_pStaticCtl, pSelf->m_lineIndex);

    // Check if the new index was set or not
    if (prevLineIndex != pSelf->m_lineIndex)
    {
        // Index was not set.
        // Assume that Max static lines has been reached, and the next line 
        // is the first one

        // show the first line
        pSelf->m_lineIndex = 0;
        ISTATIC_GoToLine(pSelf->m_pStaticCtl, pSelf->m_lineIndex);
    }

    // move to the next line
    pSelf->m_lineIndex +=1;

    // Start/Restart update timer
    (void) ISHELL_SetTimerEx(
              pSelf->m_pShell,
              pSelf->m_timeout,
              &pSelf->m_UIDialogTimerCB
              );
}

