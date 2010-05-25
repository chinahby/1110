#ifndef __SSMANAGEMENT_STATEMACHINE_H__
#define __SSMANAGEMENT_STATEMACHINE_H__

/*=============================================================================
FILE: SSManagement_StateMachine.h

SERVICES: GSM1x SS Management State Machine (FSM)

GENERAL DESCRIPTION:
      This file defines the SS Management state machine.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: StateMachine_HandleEvent

DESCRIPTION: handle a BREW event by the state machine (FSM)

PARAMETERS:
   pMe [in]: Pointer to the Applet structure.           
   eCode [in]: Specifies the Event sent to the FSM
   wParam, dwParam [in]: Event specific data.

RETURN VALUE:
   boolean: TRUE if the event was handled, FALSE otherwise.

=============================================================================*/
boolean StateMachine_HandleEvent
(
   struct _CGSM1xSSManagementApp *pMe,
   AEEEvent     eCode,
   uint16       wParam,
   uint32       dwParam
);


/*=============================================================================
FUNCTION: StateMachine_Initialize

DESCRIPTION: initialize the state machine

PARAMETERS:
     pMe [in]: Pointer to the Applet structure.

RETURN VALUE:
    TRUE for success, FALSE for failure.

=============================================================================*/
boolean StateMachine_Initialize( struct _CGSM1xSSManagementApp *pMe );


/*=============================================================================
FUNCTION: StateMachine_Destroy

DESCRIPTION: destroy the state machine

PARAMETERS:
     pMe [in]: Pointer to the Applet structure.

RETURN VALUE:
    None

=============================================================================*/
void StateMachine_Destroy( struct _CGSM1xSSManagementApp *pMe );


#endif // end __SSMANAGEMENT_STATEMACHINE_H__

