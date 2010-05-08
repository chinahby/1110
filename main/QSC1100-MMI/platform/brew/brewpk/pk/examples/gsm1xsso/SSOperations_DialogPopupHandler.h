#ifndef __SS_OPERATIONS_DIALOG_POPUP_HANDLER_H__
#define __SS_OPERATIONS_DIALOG_POPUP_HANDLER_H__

/*=============================================================================
FILE: SSOperations_DialogPopupHandler.h

SERVICES: GSM1x SS Operations Pop-up Dialog Handlers

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "AEE.h"                  // Standard AEE Declarations

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandlePopupEvents

DESCRIPTION: Handle a BREW event by the pop-up component

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
boolean SSOperationsDialogHandler_HandlePopupEvents(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleUIDialogPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in a UI dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleUIDialogPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandlePopupDialogEnd

DESCRIPTION: handle the end of the current pop-up dialog

PARAMETERS:
   pSelf [in]: Pointer to the dialog handler structure

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandlePopupDialogEnd(
    CSSOperationsDialogHandler *pSelf
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleSwappingCallsPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Swapping Calls dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleSwappingCallsPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleSwappingCallsPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Building MPTY dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleBuildingMPTYPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleEndingCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the Ending Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleEndingCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleOriginatingSecondCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the 
             Originating Second Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleOriginatingSecondCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent

DESCRIPTION: Handle a pop-up event that was detected in the 
             Connecting Incoming Call dialog

PARAMETERS:
   pSelf [in]: Pointer to dialog handler instance.           
   eCode [in]: Specifies the Event sent.
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialogHandler_HandleConnectingIncomingCallPopupEvent(
    CSSOperationsDialogHandler *pSelf,
    AEEEvent                    eCode,
    uint16                      wParam,
    uint32                      dwParam
    );


#endif // end __SS_OPERATIONS_DIALOG_POPUP_HANDLER_H__

