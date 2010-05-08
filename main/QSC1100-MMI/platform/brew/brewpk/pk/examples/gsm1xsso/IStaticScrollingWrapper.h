#ifndef __ISTATIC_SCROLLING_WRAPPER_H__
#define __ISTATIC_SCROLLING_WRAPPER_H__

/*=========================================================================
FILE: IStaticScrollingWrapper.h

SERVICES: Allows ISTATIC text scrolling by a user defined timeout value.

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
#include "AEEShell.h"        // AEE IStatic & Shell Services

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// ISTATIC scroller data
typedef struct _IStaticScroller
{
    AEECallback    m_UIDialogTimerCB; // Timer callback
    IStatic       *m_pStaticCtl;      // Static Control for scrolling
    IShell        *m_pShell;          // IShell interface
    uint32         m_lineIndex;       // Currently displayed line
    uint32         m_timeout;         // Time in msecs to display each text line

} IStaticScroller;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

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
IStaticScroller* IStaticScroller_New(IShell* pShell);

/*=============================================================================
FUNCTION: IStaticScroller_Destroy

DESCRIPTION: Frees a IStaticScroller instance

PARAMETERS:
   pSelf [in]: pointer to a valid IStaticScroller

RETURN VALUE:
    None
=============================================================================*/
void IStaticScroller_Destroy(IStaticScroller *pSelf);

/*=============================================================================
FUNCTION: IStaticScroller_SetIStatic

DESCRIPTION: Set the Static control for scrolling.

PARAMETERS:
    pSelf [in]: pointer to a valid IStaticScroller
    pStaticCtl [in]: pointer to a valid IStatic control.

RETURN VALUE:
    None.
=============================================================================*/
void IStaticScroller_SetIStatic(IStaticScroller *pSelf, IStatic *pStaticCtl);

/*=============================================================================
FUNCTION: IStaticScroller_GetIStatic

DESCRIPTION: Returned the Static control that is used for scrolling.

PARAMETERS:
    None.

RETURN VALUE:
    pointer to the IStatic control.

=============================================================================*/
IStatic* IStaticScroller_GetIStatic(IStaticScroller *pSelf);

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
void IStaticScroller_StartScrolling(IStaticScroller *pSelf, uint32 msecs);

/*=============================================================================
FUNCTION: IStaticScroller_StopScrolling

DESCRIPTION: Stop the IStatic scrolling (if it is scrollable)

PARAMETERS:
    pSelf [in]: pointer to a valid IStaticScroller

RETURN VALUE:
    None.
=============================================================================*/
void IStaticScroller_StopScrolling(IStaticScroller *pSelf);

#endif  //__ISTATIC_SCROLLING_WRAPPER_H__
