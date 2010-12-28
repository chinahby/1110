#ifndef RING_H
#define RING_H
/*===========================================================================

              R I N G E R   D R I V E R   H E A D E R   F I L E

DESCRIPTION

  This header file contains all the definitions necessary for other
  tasks to interface with the ringer driver.

Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/ring.h_v   1.0   06 Oct 2001 12:54:20   ropalsky  $
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/ring.h#4 $ $DateTime: 2008/09/24 07:11:01 $ $Author: vsudhir $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/08   vsud    Added feature guard for Ext Ringer.
10/01/01    ro     Include and use msmaud.h. Update copyright.
07/24/01   ymc     Removed RINGER_OUT support.  Used MSM_OUT in RINGER_STOP. 
                   Added T_MSM5200 support.
06/30/00   dsb     Featurized RING_USE_RINGER_ENABLE.
11/15/99    st     Removed MSM2300 and vibrator support; removed unnecessary
                   function prototypes.
08/02/99    jc     Removed new gpio_xxx MSM3100 support.
07/15/99    jc/mk  Added new gpio_xxx MSM3100 support.
03/04/99    st     Creation (mainly cut and paste from sndring.h and sndihw.h)

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "msmaud.h"             /* MSM HW interfaces for audio             */

/* <EJECT> */
#ifndef FEATURE_NO_EXT_RINGER
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/

/* Structure containing the two values required to generate a
** tone using the M & N counters for the ringer.
*/
typedef struct {
  uint16 m;
  int    n;
} ring_mn_counter_type;

/* <EJECT> */
/*===========================================================================

MACRO RINGER_STOP

DESCRIPTION
  Stop the M/N counter driving the Ringer.

PARAMETERS
  None

DEPENDENCIES
  This macro may be used in an error handler or interrupt routine.
  It does not call any subroutines or use any variables. It consists
  solely of I/O instructions.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define RINGER_STOP() MSMAUD_RINGER_STOP();

/* <EJECT> */
/*===========================================================================

FUNCTION ring_init

DESCRIPTION
  Generic Low Level Hardware Initialization for ringer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ring_msm23_plus_inuse is set.

===========================================================================*/
extern void ring_init ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION ring_click

DESCRIPTION
  Turn the ringer on and starts counter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ring_click ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION ring_tone

DESCRIPTION
  Start a DTMF emitting from the ringer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void ring_tone ( 
  uint16 freq_a,             /* frequency in hz of 1st tone to play */
  uint16 freq_b,             /* frequency in hz of 2nd tone to play */
  int    ring_vol,           /* duty cycle of tone, 5000 = 50% */
  uint16 duration_ms         /* duration of tone, 0 means turn ringer off */
);
#endif/*FEATURE_NO_EXT_RINGER*/
#endif /* RING_H */
