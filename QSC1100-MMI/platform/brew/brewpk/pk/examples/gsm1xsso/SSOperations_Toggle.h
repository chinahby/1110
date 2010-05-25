#ifndef __SS_OPERATIONS_TOGGLE_H__
#define __SS_OPERATIONS_TOGGLE_H__

/*=============================================================================
FILE: SSOperations_Toggle.h

SERVICES: Provides the Toggle sub state machine, which is part of the MN layer
          of the GSM1x SS Operations State Machine

GENERAL DESCRIPTION:
    The Toggle sub SM is part of the MN layer. It handles the Toggle (swap)
    between active and held calls procedure, which the user can execute.
    Toggle sub SM manages exactly 2 Toggle instances. Every Toggle instance 
    handles one call.
    Events received at the Toggle sub SM are routed to the appropriate Toggle
    instance.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "SSOperations_MN.h"
#include "SSCallDatabase.h"
#include "SSUtil.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

struct _CSSOperationsToggle;

// state event handler function pointer type
typedef boolean (*TOGGLE_STATE_EVENT_HANDLER)(
        struct _CSSOperationsToggle *pMe,
        AEEEvent                     eCode,
        uint16                       wParam,
        uint32                       dwParam
    );

// Toggle call information. a Toggle instance is actually defined by this 
// structure
typedef struct _ToggleCallInfo
{  
    SSCall_Info_type           *m_pCurrentCallInfo;
    // Current state event handler
    TOGGLE_STATE_EVENT_HANDLER  m_pCurrentStateHandler;
} ToggleCallInfo;

// the Toggle object
typedef struct _CSSOperationsToggle
{
    IShell                     *m_pIShell;       // Shell interface
    SSCallDatabase             *m_pCallDatabase; // Call database
    ISSUtilCls                 *m_pSSUtilCls;    // SS utilities class
    struct _CSSOperationsMN    *m_pMN;           // parent

     // array of exactly 2 calls (Toggle instances)
    ToggleCallInfo              m_Calls[2];
    // while processing an event, this member identify the processing Toggle
    // instance
    SSCall_Index_type           m_currentlyProcessedCallIndex;
    
} CSSOperationsToggle;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsToggle_New

DESCRIPTION: Create an Toggle object.

PARAMETERS:
     pIShell [in]      : Pointer to the IShell instance
     pMN [in]          : Pointer to the MN object.
     pCallDatabase [in]: Pointer to a valid CallDatabase instance

RETURN VALUE:
    Pointer to the created SSOperationsToggle or NULL on failure.

COMMENTS:
    in order to free SSOperationsToggle object, use SSOperationsToggle_Release.
    This function should be called only once in a life time of a client.
=============================================================================*/
CSSOperationsToggle* SSOperationsToggle_New(
    IShell                  *pIShell,
    struct _CSSOperationsMN *pMN,
    SSCallDatabase          *pCallDatabase
    );

/*=============================================================================
FUNCTION: SSOperationsToggle_Initialize

DESCRIPTION: Initialize an Toggle object. Initialize the 2 Toggle instances.

PARAMETERS:
     pMe [in] : Pointer to the Toggle object to be initialized.

RETURN VALUE:
    TRUE on success or FALSE on failure.
=============================================================================*/
boolean SSOperationsToggle_Initialize( CSSOperationsToggle *pMe );

/*=============================================================================
FUNCTION: SSOperationsToggle_Release

DESCRIPTION: Destroy an Toggle object.

PARAMETERS:
    pMe [in]: Pointer to the Toggle object to be destroyed.

RETURN VALUE:
    None
=============================================================================*/
void SSOperationsToggle_Release( CSSOperationsToggle *pMe );

/*=============================================================================
FUNCTION: SSOperationsToggle_HandleEvent

DESCRIPTION: Entry point for events to the Toggle object.

PARAMETERS:
   pMe [in]            : Pointer to the Toggle object. 
   eCode [in]          : Specifies the event sent to this object.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   TRUE if the event was handled, FALSE otherwise.
=============================================================================*/
boolean SSOperationsToggle_HandleEvent(
   CSSOperationsToggle *pMe,
   AEEEvent             eCode,
   uint16               wParam,
   uint32               dwParam
);

#endif // end __SS_OPERATIONS_TOGGLE_H__

