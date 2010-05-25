#ifndef __SS_COMMDEFS_H__
#define __SS_COMMDEFS_H__

/*=============================================================================
FILE: SS_CommDefs.h

SERVICES: Common SSO definition


                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================*/

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/


// Call Main State
// ---------------
#define SSCallState_NULL 0
#define SSCallState_MT_CALL_CONF 1
#define SSCallState_CALL_RECEIVED 2
#define SSCallState_CONNECT_REQUEST 3
#define SSCallState_ACTIVE 4
#define SSCallState_CALL_INIT 5
#define SSCallState_MO_CALL_PROCEEDING 6
#define SSCallState_RELEASE_REQUEST 7
#define SSCallState_DISCONNECT_REQUEST 8
    
typedef int SSCall_Main_State_type; /* like enum */


// Call Hold State
// ---------------
#define SSCallHoldState_IDLE 0
#define SSCallHoldState_HOLD_REQUEST 1
#define SSCallHoldState_RETRIEVE_REQUEST 2
#define SSCallHoldState_CALL_HELD 3
    
typedef int SSCall_Hold_State_type;  /* like enum */


// Call MPTY State
// ---------------
#define SSCallMPTYState_IDLE 0
#define SSCallMPTYState_MPTY_REQUEST 1
#define SSCallMPTYState_CALL_IN_MPTY 2
    
typedef int SSCall_MPTY_State_type;  /* like enum */


// Call state structure definition
typedef struct
{
  SSCall_Main_State_type mainState;  
  SSCall_Hold_State_type holdState;
  SSCall_MPTY_State_type mptyState;

} SSCall_State_type;


// Timers Identifiers
// ------------------

#define SSO_TIMER_BASE 0x0
#define SSO_TIMER_NONE      (SSO_TIMER_BASE) /* indicates invalid timer */

// CC timers
#define SSO_TIMER_CC_BASE 0x10
#define SSO_TIMER_T303      (SSO_TIMER_CC_BASE + 1)
#define SSO_TIMER_T305      (SSO_TIMER_CC_BASE + 2)
#define SSO_TIMER_T308      (SSO_TIMER_CC_BASE + 3)
#define SSO_TIMER_T313      (SSO_TIMER_CC_BASE + 4)

// Hold timers
#define SSO_TIMER_HOLD_BASE 0x20
#define SSO_TIMER_Thold     (SSO_TIMER_HOLD_BASE + 1)
#define SSO_TIMER_Tretrieve (SSO_TIMER_HOLD_BASE + 2)

// MPTY timers
#define SSO_TIMER_MPTY_BASE 0x30
#define SSO_TIMER_TbuildMPTY (SSO_TIMER_MPTY_BASE + 1)

typedef int SSO_TimerIdType; /* like enum */

// simulator acceleration
#if (defined U_TEST && defined AEE_SIMULATOR)
#define SSO_SEC_TO_MSEC(x) (x*300)
#else
#define SSO_SEC_TO_MSEC(x) (x*1000)
#endif

// Timers durations
// ----------------
// Times in milliseconds
#define SSO_TIMER_T303_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_T305_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_T308_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_T313_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_Thold_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_Tretrieve_DURATION SSO_SEC_TO_MSEC(30)
#define SSO_TIMER_TbuildMPTY_DURATION SSO_SEC_TO_MSEC(30)

typedef int SSO_TimerDurationType;

#endif // end __SS_COMMDEFS_H__
