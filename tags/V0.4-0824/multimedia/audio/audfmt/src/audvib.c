/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Formats Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        A U D I O   F O R M A T s   V I B R A T O R   S E R V I C E S

GENERAL DESCRIPTION
  These functions implement a vibrator pattern look-up table for the 
  WebAudio player.

EXTERNALIZED FUNCTIONS
  audvib_get_pattern
    Gets the indicated pattern.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2000 - 2002 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audvib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/05   ayu     Modified code to conform to POSIX filenames for includes.
03/16/02    st     Modified code to limit VIB patterns to 8.
07/26/00    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */
#include "audfmt.h"             /* Audio Format definitions and prototypes */
#include "audvib.h"             /* Internal Audio Format definitions       */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/
#ifdef FEATURE_WEBAUDIO
/* Number of supported vib patterns */
#define AUDVIB_NUM_PATTERNS 8

/*=========================================================================

            E X A M P L E   V I B R A T I O N   P A T T E R N S

The following is a set of example vibrator patterns. These patterns can be 
modified to implement any sort of vibrator on/off pattern. This section is 
here as a guide on how to design and format these vibration patterns.

const audfmt_vib_ctl_type vib_pattern1[] = { {AUDFMT_VIB_ON,  500}, 
                                             {AUDFMT_VIB_OFF, 500},
                                             {AUDFMT_VIB_RPT, 0  } };

const audfmt_vib_ctl_type vib_pattern2[] = { {AUDFMT_VIB_ON,  500 }, 
                                             {AUDFMT_VIB_OFF, 1000},
                                             {AUDFMT_VIB_ON,  1000},
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern3[] = { {AUDFMT_VIB_ON,  500}, 
                                             {AUDFMT_VIB_OFF, 100},
                                             {AUDFMT_VIB_RPT, 0  } };

const audfmt_vib_ctl_type vib_pattern4[] = { {AUDFMT_VIB_ON,   5000}, 
                                             {AUDFMT_VIB_STOP, 0   } };

const audfmt_vib_ctl_type vib_pattern5[] = { {AUDFMT_VIB_ON,  50}, 
                                             {AUDFMT_VIB_OFF, 50},
                                             {AUDFMT_VIB_RPT, 0 } };

const audfmt_vib_ctl_type vib_pattern6[] = { {AUDFMT_VIB_ON,  50}, 
                                             {AUDFMT_VIB_OFF, 50},
                                             {AUDFMT_VIB_ON,  100},
                                             {AUDFMT_VIB_OFF, 100},
                                             {AUDFMT_VIB_ON,  500},
                                             {AUDFMT_VIB_OFF, 500},
                                             {AUDFMT_VIB_ON,  1000},
                                             {AUDFMT_VIB_OFF, 1000},
                                             {AUDFMT_VIB_STOP,0   } };

const audfmt_vib_ctl_type vib_pattern7[] = { {AUDFMT_VIB_ON,  100}, 
                                             {AUDFMT_VIB_OFF, 1000},
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern8[] = { {AUDFMT_VIB_ON,   5000}, 
                                             {AUDFMT_VIB_STOP, 0   } };

===========================================================================*/

/* The vibration patterns used here simply turn the vibrator on indefinitely.
** The vibrator will be turned off when a vibrator off command is received
** from the WebAudio file. These patterns can be easily replaced, please see
** above for examples on how to create vibrator patterns.
*/
const audfmt_vib_ctl_type vib_pattern1[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern2[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern3[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern4[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern5[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern6[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern7[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern8[] = { {AUDFMT_VIB_ON,  5000}, 
                                             {AUDFMT_VIB_RPT, 0   } };

const audfmt_vib_ctl_type vib_pattern_null[] = { {AUDFMT_VIB_OFF,  0},
                                                 {AUDFMT_VIB_STOP, 0} };

/* The following is an array of patterns for the compact midi vibrator
** function.
*/
const audfmt_vib_ctl_type *audvib_pattern_array[] = 
    { vib_pattern1, vib_pattern2, vib_pattern3, vib_pattern4,
      vib_pattern5, vib_pattern6, vib_pattern7, vib_pattern8 };

/*===========================================================================

FUNCTION audvib_get_pattern

DESCRIPTION
  This function gets the indicated vibrator pattern.

DEPENDENCIES
  audvib_pattern_array

RETURN VALUE
  audfmt_vib_ctl_type - vibration pattern

SIDE EFFECTS
  None

===========================================================================*/
audfmt_vib_ctl_type *audvib_get_pattern (
  uint8 index
) {
  if(index < AUDVIB_NUM_PATTERNS) {
    return((audfmt_vib_ctl_type *) (audvib_pattern_array[index]));
  } else {
    return((audfmt_vib_ctl_type *) vib_pattern_null);
  }
}

#endif /* FEATURE_WEBAUDIO */

