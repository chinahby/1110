#ifndef __SSMANAGEMENT_USEREVENT_H__
#define __SSMANAGEMENT_USEREVENT_H__

/*=============================================================================
FILE: SSManagement_UserEvent.h

SERVICES: GSM1x SS Management User events definitions

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

#include "AEE.h"              // Standard AEE Declarations

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/

// See the comment at the top of EVT_DIALOG_START in TimedMsgBox_HandleEvent()
// at the DialogHandler for details.
#define EVT_WAIT_FOR_REDRAW (EVT_USER+1)

// This event is sent to a dialog when the dialog timer runs out.
// (wParam = dwParam = 0)
#define EVT_DIALOG_TIMEOUT (EVT_USER+2)

// This event is sent to a dialog when it should continue.
#define EVT_DIALOG_CONTINUE (EVT_USER+3)

// this event is sent to the state machine to invoke a new management operation
#define EVT_SS_REQUEST (EVT_USER+4)

// this event is sent to the state machine when a response is received 
// from the network
#define EVT_SS_RESPONSE (EVT_USER+5)

// This event is sent to the state machine when a timer runs out.
// (wParam = dwParam = 0)
#define EVT_STATE_MACHINE_TIMEOUT (EVT_USER+6)

// this event is sent to the state machine when a status report of sending a
// GSM1x signaling message is received
#define EVT_SIGNALING_STATUS (EVT_USER+7)

// this event is internally sent to the current state handler as a result of
// an IPhone notification
#define EVT_PHONE_STATUS_CHANGE (EVT_USER+8)

// sent from the State machine to the Dialogs as an indication that the SS
// operation has ended successfully or unsuccessfully
#define EVT_SS_OPER_END (EVT_USER+9)

// Internal State Machine's OnEnter event
#define ONENTER (EVT_USER+10)

// Internal State Machine's OnExit event
#define ONEXIT (EVT_USER+11)

#endif // end __SSMANAGEMENT_USEREVENT_H__
