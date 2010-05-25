#ifndef __SS_OPERATIONS_USER_EVENTS_H__
#define __SS_OPERATIONS_USER_EVENTS_H__

/*=============================================================================
FILE: SSOperations_UserEvents.h

SERVICES: GSM1x SS Operations User events definitions

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

// SSO user events

// Dummy! indicates the starting value of the user event
#define INITIAL_USER_EVENT_ECODE_VALUE      EVT_USER+0x200

// Sent when various UI dialog timers expire
#define EVT_USER_DIALOG_TIMEOUT             EVT_USER+0x201
// Used by UI dialogs
#define EVT_USER_DRAW                       EVT_USER+0x202
// Used by the Call Details dialog
#define EVT_USER_UPDATE_DIALOG              EVT_USER+0x203
// Used by the Pop-up Dialog
#define EVT_USER_POPUP_REDRAW               EVT_USER+0x204
// Used by the Pop-up Dialog
#define EVT_POPUP_DIALOG_TIMEOUT            EVT_USER+0x205
// Used between the Entry Menu and Call Details Dialogs
#define EVT_ENTRY_MENU_HOLD_REQUESTED       EVT_USER+0x206

/*==========================================================
// SSO primitives for the UI <=> MN and MN <=> CC interfaces
===========================================================*/

// First value for SSO primitives
#define EVT_SSO_PRIMITIVES_FIRST            EVT_USER+0x300

////////////////////////
// UI <=> MN events
////////////////////////

// First value for UI <=> MN events
#define EVT_UIMN_FIRST                      EVT_USER+0x400

////////////////////////
// MN => UI events :  
////////////////////////

// Used by FSM to indicate call acceptance by remote party
#define UIMN_SETUP_CNF                      EVT_UIMN_FIRST+1
// Used by FSM to indicate Call Waiting
#define UIMN_SETUP_IND                      EVT_UIMN_FIRST+2
// Used by FSM to indicate Call Setup completion
#define UIMN_SETUP_COMPLETE_IND             EVT_UIMN_FIRST+3
// Used by FSM to indicate Disconnection
#define UIMN_DISC_IND                       EVT_UIMN_FIRST+4
// Used by FSM to indicate Call Release
#define UIMN_REL_IND                        EVT_UIMN_FIRST+5
// Used by FSM to indicate Call Release Confirmation
#define UIMN_REL_CNF                        EVT_UIMN_FIRST+6
// Used by FSM to confirm multi party
#define UIMN_BUILD_MPTY_CNF                 EVT_UIMN_FIRST+7
// Used by FSM to indicate Various call state changes
#define UIMN_FACILITY_IND                   EVT_UIMN_FIRST+8
// Used by FSM to indicate Call Hold Confirmation
#define UIMN_HOLD_CNF                       EVT_UIMN_FIRST+9
// Used by FSM to indicate Call Hold Rejection
#define UIMN_HOLD_REJ_IND                   EVT_UIMN_FIRST+10
// Used by FSM to indicate Call Retrieve Confirmation
#define UIMN_RETRIEVE_CNF                   EVT_UIMN_FIRST+11
// Used by FSM to indicate Call Retrieve Rejection
#define UIMN_RETRIEVE_REJ_IND               EVT_UIMN_FIRST+12
// Used by FSM to indicate Toggle Confirmation
#define UIMN_TOGGLE_CNF                     EVT_UIMN_FIRST+13
// Used by FSM to indicate Toggle Rejection
#define UIMN_TOGGLE_REJ_IND                 EVT_UIMN_FIRST+14
// Used by FSM to indicate Call Waiting's Answer and Hold Confirmation
#define UIMN_ANSWER_AND_HOLD_CNF            EVT_UIMN_FIRST+15
// Used by FSM to indicate Call Waiting's Answer and Hold Rejection
#define UIMN_ANSWER_AND_RELEASE_CNF         EVT_UIMN_FIRST+16
// Used by FSM to indicate an Error
#define UIMN_ERR_IND                        EVT_UIMN_FIRST+17
// Used by FSM to indicate a dropped traffic channel
#define UIMN_DROP_TCH_IND                   EVT_UIMN_FIRST+18

////////////////////////
// UI => MN events :  
////////////////////////

// Used by UI to request call setup
#define UIMN_SETUP_REQ                      EVT_UIMN_FIRST+50
// Used to accept MT call
#define UIMN_SETUP_RSP                      EVT_UIMN_FIRST+51
// Used after UIMN_SETUP_IND to indicate that the user has been alerted
// to the waiting call
#define UIMN_ALERT_REQ                      EVT_UIMN_FIRST+52
// Used to request call disconnection
#define UIMN_DISC_REQ                       EVT_UIMN_FIRST+53
// Used by UI to request multi party
#define UIMN_BUILD_MPTY_REQ                 EVT_UIMN_FIRST+54
// Used to request to hold a call
#define UIMN_HOLD_REQ                       EVT_UIMN_FIRST+55
// Used to request to retrieve a call
#define UIMN_RETRIEVE_REQ                   EVT_UIMN_FIRST+56
// Used to request call Swapping (Toggling of the calls)
#define UIMN_TOGGLE_REQ                     EVT_UIMN_FIRST+57
// Used to request a hold of the current call and answer of the incoming call
#define UIMN_ANSWER_AND_HOLD_REQ            EVT_UIMN_FIRST+58
// Used to request a release of the current call and answer of the incoming call
#define UIMN_ANSWER_AND_RELEASE_REQ         EVT_UIMN_FIRST+59

// Last value for UI <=> MN events
#define EVT_UIMN_LAST                       EVT_USER+0x4ff

////////////////////////
// MN <=> CC events
////////////////////////

// First value for MN <=> CC events
#define EVT_MNCC_FIRST                      EVT_USER+0x500

////////////////////////
// CC => MN events :  
////////////////////////

// Used to indicate Call Waiting
#define MNCC_SETUP_IND                      EVT_MNCC_FIRST+1
// Used to indicate call acceptance by remote party
#define MNCC_SETUP_CNF                      EVT_MNCC_FIRST+2
// Used to indicate MT call acceptance
#define MNCC_SETUP_COMPLETE_IND             EVT_MNCC_FIRST+3
// Used to indicate Disconnection
#define MNCC_DISC_IND                       EVT_MNCC_FIRST+4
// Used to indicate Call Release
#define MNCC_REL_IND                        EVT_MNCC_FIRST+5
// Used to indicate Call Release Confirmation
#define MNCC_REL_CNF                        EVT_MNCC_FIRST+6
// Used to confirm multi party
#define MNCC_BUILD_MPTY_CNF                 EVT_MNCC_FIRST+7
// Used to indicate Various call state changes
#define MNCC_FACILITY_IND                   EVT_MNCC_FIRST+8
// Used to indicate Call Hold Confirmation
#define MNCC_HOLD_CNF                       EVT_MNCC_FIRST+9
// Used to indicate Call Hold Rejection
#define MNCC_HOLD_REJ_IND                   EVT_MNCC_FIRST+10
// Used to indicate Call Retrieve Confirmation
#define MNCC_RETRIEVE_CNF                   EVT_MNCC_FIRST+11
// Used to indicate Call Retrieve Rejection
#define MNCC_RETRIEVE_REJ_IND               EVT_MNCC_FIRST+12
// Used to indicate an Error
#define MNCC_ERR_IND                        EVT_MNCC_FIRST+13
// Used to indicate a dropped traffic channel
#define MNCC_DROP_TCH_IND                   EVT_MNCC_FIRST+14
// Merge of CC instances confirmation, after successful Build MPTY request
#define MNCC_MERGE_CALLS_CNF                EVT_MNCC_FIRST+15

////////////////////////
// MN => CC events :  
////////////////////////

// Used to request call setup
#define MNCC_SETUP_REQ                      EVT_MNCC_FIRST+16
// Used to accept MT call
#define MNCC_SETUP_RSP                      EVT_MNCC_FIRST+17
// Used after MNCC_SETUP_IND to indicate that the user has been alerted
// to the waiting call
#define MNCC_ALERT_REQ                      EVT_MNCC_FIRST+18
// Used to request call disconnection
#define MNCC_DISC_REQ                       EVT_MNCC_FIRST+19
// Used to request multi party
#define MNCC_BUILD_MPTY_REQ                 EVT_MNCC_FIRST+20
// Used to request to hold a call
#define MNCC_HOLD_REQ                       EVT_MNCC_FIRST+21
// Used to request to retrieve a call
#define MNCC_RETRIEVE_REQ                   EVT_MNCC_FIRST+22
// Used to forward incoming GSM1x messages to the CC
#define MNCC_DATA_INDICATION                EVT_MNCC_FIRST+23
// Merge CC instances after successful Build MPTY request
#define MNCC_MERGE_CALLS_REQ                EVT_MNCC_FIRST+24

// Last value for MN <=> CC events
#define EVT_MNCC_LAST                       EVT_USER+0x5ff

// Last value for SSO primitives
#define EVT_SSO_PRIMITIVES_LAST             EVT_USER+0x6ff

////////////////////////
// Internal MN events :  
////////////////////////

// Used to indicate completion of a single Toggle FSM.
#define MN_TOGGLE_FSM_DONE                  EVT_USER+0x700

////////////////////////
// Internal CC and HOLD events :  
////////////////////////

// Timeout event
#define CC_TIMEOUT                          EVT_USER+0x701
// OnEnter event
#define CC_ONENTER                          EVT_USER+0x702
// OnExit event
#define CC_ONEXIT                           EVT_USER+0x703
// sent from HOLD to CC to indicate that a timer (Thold or Tretrieve) has 
// expired and the call is to be cleared.
#define CC_HOLD_TIMEOUT_IND                 EVT_USER+0x704

// end of SSO user events

#endif // end __SS_OPERATIONS_USER_EVENTS_H__
