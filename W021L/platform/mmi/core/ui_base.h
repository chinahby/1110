#ifndef _UI_BASE_H
#define _UI_BASE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           U S E R   I N T E R F A C E   M A I N  M O D U L E

GENERAL DESCRIPTION
  This is the minimal "user interface".  It is responsible for ensuring
  the UI task starts and handles the phone events not related to the
  user.  This code will exist to ensure the ui_task runs properly on
  a phone without a UI or with a UI on a different processor.  If the
  phone has a UI on this processor.

EXTERNALIZED FUNCTIONS

  ui_task
    The main user interface task.  Handle events

  ui_kick_dog
    Kicks the watchdog.

  Defines uisleep & uisleepold global variables.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ui_task should be created by MC before anyone calls ui_cmd.
  NV, HS, SND, and DIAG should be started before ui_task.
  All other functions are internal to UI.

  Copyright (c) 1999 through 2006 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/ui_base.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/09   Krishnac Error fatal nv_task()
09/06/06   jks     Added support for FEATURE_NEW_SLEEP_API
04/13/06   cvs     Move call control to its own file
01/19/06   cvs     support for emergency call categories
12/20/04   cvs     Featurization for msm7500
08/03/04   PA      Added ui_is_in_call and ui_is_in_sms_call
02/26/04   AT      Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_NEW_SLEEP_API

#define UI_ENABLE_SLEEP()                                \
{                                                        \
  if (!ui_ok_to_sleep) {                                 \
    (void)sleep_assert_okts(gNewSleepHandle);            \
    MSG_LOW( "UI Sleep enable", 0, 0, 0 );               \
    ui_ok_to_sleep = TRUE;                               \
  }                                                      \
}

#define UI_DISABLE_SLEEP()                               \
{                                                        \
  if (ui_ok_to_sleep) {                                  \
    (void)sleep_negate_okts(gNewSleepHandle);            \
    MSG_LOW( "UI Sleep disable", 0, 0, 0 );              \
    ui_ok_to_sleep = FALSE;                              \
  }                                                      \
}

#elif defined(FEATURE_SLEEP)

#define UI_ENABLE_SLEEP()                                \
{                                                        \
  if (!ui_ok_to_sleep) {                                 \
    (void)rex_set_sigs( &sleep_tcb, SLEEP_UI_OKTS_SIG ); \
    MSG_LOW( "UI Sleep enable", 0, 0, 0 );               \
    ui_ok_to_sleep = TRUE;                               \
  }                                                      \
}

#define UI_DISABLE_SLEEP()                               \
{                                                        \
  if (ui_ok_to_sleep) {                                  \
    (void)rex_clr_sigs( &sleep_tcb, SLEEP_UI_OKTS_SIG ); \
    MSG_LOW( "UI Sleep disable", 0, 0, 0 );              \
    ui_ok_to_sleep = FALSE;                              \
  }                                                      \
}

#else

#define UI_ENABLE_SLEEP()                                \
{                                                        \
    MSG_LOW( "UI Sleep enable", 0, 0, 0 );               \
    ui_ok_to_sleep = TRUE;                               \
}

#define UI_DISABLE_SLEEP()                               \
{                                                        \
    MSG_LOW( "UI Sleep disable", 0, 0, 0 );              \
    ui_ok_to_sleep = FALSE;                              \
}

#endif /* FEATURE_NEW_SLEEP_API */

/* <EJECT> */
/*===========================================================================
FUNCTION ui_complete_pwrdown

DESCRIPTION
  This function acknoleges MC that the powerdown has completed.
  It can be called from the CoreApp or from the thin_ui's handling
  of the stop signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ui_complete_pwrdown(void);

/* <EJECT> */
/*===========================================================================
FUNCTION HandleStopSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleStopSig(void);

/* <EJECT> */
/*===========================================================================
FUNCTION HandleOfflineSig

DESCRIPTION
  This function handles offline signals received by UI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleOfflineSig(void);


/* EJECT */
/*===========================================================================
FUNCTION ui_kick_dog

DESCRIPTION
  This function reports to dog task that UI is alive and kicking!

RETURN VALUE
  None

DEPENDENCIES
  Dog Task is running.
===========================================================================*/
void ui_kick_dog( void );

/* <EJECT> */
/*===========================================================================
FUNCTION HandleSignals

DESCRIPTION
  This function handles all signals received by UI task. This function is to
  be invoked for every signal UI recives. It calls the appropriate functions
  to handle each signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void HandleSignals ( rex_sigs_type sigs );

/*===========================================================================

FUNCTION ui_wait

DESCRIPTION
  Main Wait routine for UI task. This routine performs the following functions:
    - Waits for a signal in the requested mask to be set.
    - Handles Watchdog report signal
    - Handles Key release to finish the keybeep if Core UI is built in

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type ui_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
);

/*===========================================================================

FUNCTION ui_wait_for_nv

DESCRIPTION
  Main Wait routine for UI task. This routine performs the following functions:
    - Waits for a signal in the requested mask to be set.
    - Handles Watchdog report signal
    - Handles Key release to finish the keybeep if Core UI is built in

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type ui_wait_for_nv
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
);


/* <EJECT> */
/*===========================================================================
FUNCTION UI_TASK

DESCRIPTION
  The main user interface function.  This is the big loop outside of the
  state machine which handles signals and all sorts of inputs.  It mostly
  serves to convert inputs into events for the state machine, but it
  also handles keypad incidentals - backlighting and key beeps.

DEPENDENCIES
  REX is running.  NV and DIAG and HS are running.

RETURN VALUE
  Never exits.

SIDE EFFECTS
  Plays with variable ui.

===========================================================================*/
void ui_task (
  dword dummy                     /*lint -esym(715,dummy) */
                                  /*ignore fact that this isn't used */
);

#if defined FEATURE_UI_CORE_REMOVED && !defined (FEATURE_UI_DUALPROC_MDM)
/* <EJECT> */
/*===========================================================================
FUNCTION   ui_is_in_call

DESCRIPTION
  This function checks whether we are in any calls.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_is_in_call(void);

/* <EJECT> */
/*===========================================================================
FUNCTION   ui_is_in_sms_call

DESCRIPTION
  This function checks whether we are in an SMS call.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_is_in_sms_call(void);
#define ui_curr_call_is_sms() ui_is_in_sms_call()
#endif //FEATURE_UI_CORE_REMOVED && !FEATURE_UI_DUALPROC_MDM

#endif //_UI_BASE_H
