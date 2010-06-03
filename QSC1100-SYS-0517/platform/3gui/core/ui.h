#ifndef UI_H
#define UI_H
/*===========================================================================

           U S E R   I N T E R F A C E   M A I N   H E A D E R

DESCRIPTION
  This is the external interface for all external modules into the user
  interface.  Calls, commands,  and external definitions.  DMSS software.


 Copyright (c) 1991-2003 by QUALCOMM INCORPORATED  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/msmshared/apps/Core/ui.h#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/07   jks     Mainlined Brew App Coordination. Proxy App to represent App
                   from Alternate Environments.
07/28/07   cvs     Add USB driver signals
12/07/05   jas     Changed signal name for media synchronization
10/14/05   jg      Added signal for CMX related sync with UI task
07/29/05   tml     Added signal for ICard
07/29/05   AT      Add signal for BREW OEM Layer Task blocking.
12/20/04   cvs     Add signal for autoanswer in ThinUI
11/01/04   AT      Moved the NET signal as it conflicted with a task signal.
10/28/04   AT      Added the AEE_SLEEP_SIG for improved BREW sleeping.
10/28/04   AT      Added the NET signal for BREW processing.
03/25/04   sun     Use the correct value for IPC_SIGNAL
03/08/04   sun     UI Task Changes due to IPC
12/11/03   kar     Added RUIM, SIO signals
09/30/03   tml     Added function prototype
09/26/03   tml     Initial revision

===========================================================================*/

#include "comdef.h"               /* basic definitions */
#include "target.h"               /* Target configuration file */
#include "customer.h"             /* Customer Specific Definitions */
#include "tmc.h"


/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* Signals for the UI task */
/* avoid common signals listed in task.h */
#define UI_KEY_SIG         0x0002  /* Key from HS task */
#define UI_RPT_TIMER_SIG   0x0004  /* Time to kick watchdog */
#define UI_NV_SIG          0x0008  /* Return from NV */
#define UI_AUTOANSWER_SIG  0x0010  /* autoanswer timer expired */
#define UI_OEM_SIG         0x0020  /* APP COM FUNC signal to block UI task. */
#define UI_ICARD_SIG       0x0040  /* Signal Used for ICARD related events */
#define AEE_MEDIA_SIG      0x0080  /* Signal for media-related syncronization */
#define UI_IPC_SIG         0x0100
#define AEE_SIO_SIG        0x0200
#define UI_RUIM_SIG        0x0400
#define AEE_NET_SIG        0x0800 /* Used to help BREW process Network signals */
#define AEE_APP_SIG        0x1000
//From task.h, avoid overlap
//define  TASK_START_SIG   0x8000
//define  TASK_STOP_SIG    0x4000
//define  TASK_OFFLINE_SIG 0x2000

#define AEE_USBDRIVER_SIG  0x20000
#define AEE_SLEEP_SIG      0x80000 /* AEE_SLEEP_SIG for Brew 3.1 Sleep. */

#define UI_BAC_SIG         0x100000 /*Used for BREW App Coordination with 
                                      alternate environment applications */
#define UI_TS_EVENT_SIG    0x200000

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern void ui_task (
  dword dummy
);

/* <EJECT> */
/*===========================================================================
FUNCTION handle_keys

DESCRIPTION
  Handle a key pressed.

DEPENDENCIES
  REX is running.  NV and DIAG and HS are running.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void handle_keys(void);

#endif /* UI_H */
