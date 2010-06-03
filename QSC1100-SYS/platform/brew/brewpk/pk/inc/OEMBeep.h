/*======================================================
FILE:  OEMBeep.h

SERVICES:   OEM beep interface

GENERAL DESCRIPTION:

Beep related definitions for the OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEM_BEEP_H)
#define OEM_BEEP_H  1

#include "AEEShell.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern boolean          OEM_SimpleBeep(BeepType nBeepType, boolean bLoud);

#if defined(__cplusplus)
}
#endif

/*=====================================================================
  INTERFACE  DOCUMENTATION
=======================================================================
OEMBeep Interface
=========================================================================
Function: OEM_SimpleBeep()

Description:
   This function plays a standard OEM beep tone and vibration given
the duration, and returns TRUE if successful.

Prototype:
   boolean OEM_SimpleBeep(BeepType nBeepType, boolean bLoud);

Parameters:
   nBeepType: beep type which can be one of the following:
      - BEEP_OFF: stop playback of the current beep or vibration
      - BEEP_ALERT: alert beep tone
      - BEEP_REMINDER: reminder beep tone
      - BEEP_MSG: message beep tone
      - BEEP_ERROR: error beep tone
      - BEEP_VIBRATE_ALERT: alert vibration
      - BEEP_VIBRATE_REMIND: reminder vibration
   bLoud: boolean flag that sets the playback volume high or low

Return Value:
   TRUE if successfuly played or stopped the tone or vibration;
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================*/
#endif // !defined(OEM_BEEP_H)
