#ifndef CORE_UI_H
#define CORE_UI_H
/*===========================================================================

FILE: core_ui.h

DESCRIPTION
   Header file for core_ui.c

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/core/core_ui.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/21/07   jks     Mainlined changes for set TTYHeadset device status 
                   in HDK build when FEATURE_UIXSND_IMPLEMENTATION is defined.
08/17/07   cvs     Add function to get backlight duration
02/20/07   jas     Created
===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "target.h"
#include "customer.h"
#include "comdef.h"

#include "hs.h"
#include "uixsnd.h"
#ifdef FEATURE_BREW
#include "AEE.h"
#include "AEE_OEM.h"
#include "OEMFeatures.h"
#endif /* FEATURE_BREW */

#define EVT_UI_EXIT                 1150
#define EVT_MC_PWR_DOWN             1155

/*===========================================================================
FUNCTION CoreAppHandleSignals

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
void CoreAppHandleSignals (
    rex_sigs_type sigs
);

/*===========================================================================
FUNCTION CoreTask_init

DESCRIPTION
  Initialize the user interface - called immediately after UI_TASK is
  started.  Initialize screen handler, queues, timers, etc.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from CoreTask_Init

===========================================================================*/
void CoreTask_init( void );

/*===========================================================================
FUNCTION CoreApp_Getsigs

DESCRIPTION
  Give the signal that we will need from this.

DEPENDENCIES
  None

RETURNS
  

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type CoreApp_Getsigs( void );

/*===========================================================================
FUNCTION CoreTask_GetSilenceAll

DESCRIPTION
  Get the SILENCEALL flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifndef FEATURE_UIONE_HDK
boolean CoreTask_GetSilenceAll(void);
#else
static __inline boolean CoreTask_GetSilenceAll(void)
{
   IShell *pShell = AEE_GetShell();
   uint32 mannerMode = AEE_MANNER_MODE_NORMAL;
   int size = sizeof(mannerMode);

   (void) ISHELL_GetDeviceInfoEx(
      pShell,
      AEE_DEVICEITEM_MANNER_MODE,
      &mannerMode,
      &size
   );

   return (mannerMode != AEE_MANNER_MODE_NORMAL);
}
#endif /* FEATURE_UIONE_HDK */

#ifndef FEATURE_UIONE_HDK
/*===========================================================================
FUNCTION CoreTask_SetPwrDnComplete

DESCRIPTION
  Set the ui_pwrdown_complete static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetPwrDnComplete(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_GetPwrDnComplete

DESCRIPTION
  Get the ui_pwrdown_complete static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean CoreTask_GetPwrDnComplete(void);

/*===========================================================================
FUNCTION CoreTask_SetGameMode

DESCRIPTION
  Set the ui_game_mode static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetGameMode(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_GetGameMode

DESCRIPTION
  Get the ui_game_mode static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean CoreTask_GetGameMode(void);

/*===========================================================================
FUNCTION CoreTask_SetDeepSleep

DESCRIPTION
  Set the ui_deepsleep static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetDeepSleep(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_SetBurst

DESCRIPTION
  Set the ui_burst static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetBurst(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_GetBurst

DESCRIPTION
  Get the ui_burst static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean CoreTask_GetBurst(void);

/*===========================================================================
FUNCTION CoreTask_GetKeyDown
 
DESCRIPTION
   Get the key_down static variable
 
 DEPENDENCIES
   None
  
SIDE EFFECTS
   None
===========================================================================*/       
hs_key_type CoreTask_GetKeyDown(void);

#endif /* FEATURE_UIONE_HDK */

/*===========================================================================
FUNCTION CoreTask_SetKeyDownTone

DESCRIPTION
  Set the ui_key_down_tone static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetKeyDownTone(snd_tone_type new_value);

/*===========================================================================
FUNCTION CoreTask_SetKeyDownDtmf

DESCRIPTION
  Set the ui_key_down_dtmf static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetKeyDownDtmf(snd_tone_type new_value);

/*===========================================================================
FUNCTION CoreTask_SetDemoAlert

DESCRIPTION
  Set the ui_demo_alert static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetDemoAlert(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_GetDemoAlert

DESCRIPTION
  Returns the ui_demo_alert static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
#ifndef FEATURE_UIONE_HDK
extern boolean CoreTask_GetDemoAlert(void);
#else
static __inline boolean CoreTask_GetDemoAlert(void)
{
   // Not supported in HDK
   return FALSE;
}
#endif /* FEATURE_UIONE_HDK */

/*===========================================================================
FUNCTION CoreTask_SetBackVal

DESCRIPTION
  Set the ui_backval static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetBackVal(byte new_value);

/*===========================================================================
FUNCTION CoreTask_SetBackDuration

DESCRIPTION
  Set the ui_back_duration static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetBackDuration(byte new_value);

/*===========================================================================
FUNCTION CoreTask_GetBackDuration

DESCRIPTION
  Get the ui_backval static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte CoreTask_GetBackDuration(void);

/*===========================================================================
FUNCTION CoreTask_SetPowerUp

DESCRIPTION
  Set the ui_powerup static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetPowerUp(boolean new_value);

#ifndef FEATURE_UIONE_HDK
/*===========================================================================
FUNCTION CoreTask_SetSilenceAll

DESCRIPTION
  Set the SILENCEALL flag

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetSilenceAll(boolean new_value);

#ifdef FEATURE_TTY
/*===========================================================================
FUNCTION CoreTask_SetTTY

DESCRIPTION
  Set the ui_powerup static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
void CoreTask_SetTTY(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_GetTTY

DESCRIPTION
  Get the ui_headset static variable

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/       
boolean CoreTask_GetTTY(void);
#endif /* FEATURE_TTY */

/*===========================================================================
FUNCTION CoreTask_GetCoreLPM

DESCRIPTION
    Get The LPM Mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreTask_GetCoreLPM(void);

/*===========================================================================
FUNCTION CoreTask_SetCoreLPM

DESCRIPTION
    Set The LPM Mode

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetCoreLPM(boolean new_value);

/*===========================================================================
FUNCTION CoreTask_SetLCDTimer

DESCRIPTION


DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void CoreTask_SetLCDTimer(uint32 timer);

#endif /* FEATURE_UIONE_HDK */

#if defined(FEATURE_UIONE_HDK) && defined(FEATURE_UIXSND_IMPLEMENTATION) && defined(FEATURE_TTY)
int CoreUI_SetTTYDeviceStatus(uisnd_device_status  status);
#endif

#endif // CORE_UI_H

