#ifndef AUDVIB_H
#define AUDVIB_H
/*===========================================================================

              A U D I O   F O R M A T S   V I B R A T O R
                        H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the audio formats vibrator module.

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audvib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/00    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_WEBAUDIO
#include "audfmt.h"             /* Audfmt typedefs and prototypes         */

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
extern audfmt_vib_ctl_type *audvib_get_pattern (
  uint8 index
);
#endif /* FEATURE_WEBAUDIO */

#endif /* AUDVIB_H */
