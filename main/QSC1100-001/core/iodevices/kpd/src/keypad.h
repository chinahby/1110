#ifndef KEYPAD_H
#define KEYPAD_H
/*===========================================================================

              K E Y P A D   H E A D E R   F I L E

DESCRIPTION
  This file contains the data structures used to interface to the keypad
  device driver for the various DMSS phones uses Toucan handset.

  When a key is pressed, a key event callback is made. When the pressed
  key is release, another key event (RELEASE) is made.

  Multiple keys may be pressed at the same time and multiple key press and
  release will be reported.

  The holding of end key for a duration may be considered as power
  down key. The duration is programmed in milli-seconds.

REFERENCES

  Copyright (c) 2001, 2002, 2004, 2006-2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/drivers/kpd/keypad.h#6 $ $DateTime: 2008/12/05 12:12:15 $ $Author: rtilak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/08   rt      Add support for PMIC based headset insertion detection
05/18/07   gdp     Changes to support "*"/"#" while in phone is crashed state
03/14/07   gmy     Lint cleanup
12/20/06   gmy     Added keypad_init_iface() & keypad_is_key_pressed() for SC2x
05/18/04   ajn     Added FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
05/10/04   ajn     Exported keypad_resume_polling for PMIC pwr key ISR
03/15/04   cjr     added PMIC Keypad LED definition
10/10/02   pd      Moved keypad_key_event_type definition to hs.h
11/12/01   dl      Initial revision

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "customer.h"   /* Customer specific definitions        */
#include "target.h"     /* Target specific definitions          */
#include "hs.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define KEYPAD_DEFAULT_POWER_KEY_DURATION 500 /* default to 0.5 second */

#define KEYPAD_BACKLIGHT_LVL_0 PM_LED_KBD_SETTING__LEVEL0
#define KEYPAD_BACKLIGHT_LVL_1 PM_LED_KBD_SETTING__LEVEL1
#define KEYPAD_BACKLIGHT_LVL_2 PM_LED_KBD_SETTING__LEVEL2
#define KEYPAD_BACKLIGHT_LVL_3 PM_LED_KBD_SETTING__LEVEL3
#define KEYPAD_BACKLIGHT_LVL_4 PM_LED_KBD_SETTING__LEVEL4

/*===========================================================================

                      MACRO DECLARATIONS

===========================================================================*/

#define KEYPAD_PASS_KEY_CODE(key, release_key) \
          keypad_pass_key_code( (byte)(key), (byte)(release_key) )


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION KEYPAD_INIT_IFACE

DESCRIPTION
  This procedure initializes the keypad physical interface.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_init_iface( void );

/*===========================================================================

FUNCTION KEYPAD_INIT_MATRIX

DESCRIPTION
  This procedure initializes basic keypad resources required to scan the 
  keypad matrix. It does not initialize PMIC, clock, or sleep resources.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_init_matrix( void );

/*===========================================================================

FUNCTION KEYPAD_INIT

DESCRIPTION
  This procedure initializes the keypad operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void keypad_init( void );

/*===========================================================================

FUNCTION KEYPAD_REGISTER

DESCRIPTION
  This procedure registers the keypad callback function.
  Call this function with a function pointer to register.
  Call this function with NULL pointer to de-register.
  Only one client is supported.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if register successfully
  FALSE if keypad service was already registered.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean keypad_register( keypad_key_event_cb_f_type *cb);

/*===========================================================================

FUNCTION KEYPAD_PASS_KEY_CODE

DESCRIPTION
  Pass a keycode from the keypad device driver to the callback function or the
  ring buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void keypad_pass_key_code( byte key_code, byte key_parm );

/*===========================================================================

FUNCTION KEYPAD_SIMULATE_KEY

DESCRIPTION
  This procedure simulates a key entry.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void keypad_simulate_key( byte );

/*===========================================================================

FUNCTION KEYPAD_LOCK_KEYPAD

DESCRIPTION
  This procedure locks (TRUE) /unlocks (FALSE) the keypad.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void keypad_lock_keypad( boolean lock );

/*===========================================================================

FUNCTION KEYPAD_GET_KEY

DESCRIPTION
  Read a keycode from the input buffer.

DEPENDENCIES
  None

RETURN VALUE
  This procedure returns the oldest unread keycode.  If no keycode
  is currently available, HS_NONE_K is returned.

SIDE EFFECTS
  None

===========================================================================*/
keypad_key_event_type keypad_get_key( void );

/*===========================================================================

FUNCTION KEYPAD_SET_POWER_KEY_DURATION

DESCRIPTION
  This procedure set the power key duration in milli-seconds. When END key
  is pressed for over this duration, HS_PWR_K pressed event is generated.
  Default duration is KEYPAD_DEFAULT_POWER_KEY_DURATION msec.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void keypad_set_power_key_duration( word );

/*===========================================================================

FUNCTION KEYPAD_SET_BACKLIGHT

DESCRIPTION
  This procedure sets the back light level. If level is 0, back light is
  turned off.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void keypad_set_backlight( byte level );

/*===========================================================================

FUNCTION KEYPAD_IS_POWER_KEY_PRESSED

DESCRIPTION
  This function returns the status of the power key.

DEPENDENCIES
  If FEATURE_PMIC is defined then pm_init must have been called.

RETURN VALUE
  Boolean: TRUE,    power on key is pressed.
           FALSE    power on key is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_power_key_pressed(void);

/*===========================================================================

FUNCTION KEYPAD_IS_PANIC_CHORD_PRESSED

DESCRIPTION
  This function returns if the panic key chord is pressed.

DEPENDENCIES

RETURN VALUE
  Boolean: TRUE,    panic chord is pressed.
           FALSE    panic chord is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_panic_chord_pressed(void);

#ifdef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
/*===========================================================================

FUNCTION KEYPAD_ENABLE_POLLING

DESCRIPTION
  Installs a keypad polling timer, and informs the sleep task that we are NOT
  in a state where we can sleep.

DEPENDENCIES
  Externalized for use by PMIC for PWR key.

RETURN VALUE
  None

SIDE EFFECTS
  Uninstalls the keypress ISR, if installed.

===========================================================================*/
void keypad_enable_polling(void);

#endif /* FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

/*===========================================================================

FUNCTION KEYPAD_IS_KEY_PRESSED

DESCRIPTION
  This function returns if the specified key is pressed.

DEPENDENCIES
  Keypad initialization is expected to have been performed prior to calling
  this routine. Only keypad interface init is required for calls from boot 
  since that functionality does not need the full capability of the keypad 
  driver.

RETURN VALUE
  Boolean: TRUE,    key is pressed.
           FALSE    key is not pressed.

SIDE EFFECTS
  Sleep control is not manipulated by this routine.

===========================================================================*/
boolean keypad_is_key_pressed(hs_key_type key);

#if defined(FEATURE_PMIC_HS_SEND_END_DETECT)
/*===========================================================================

FUNCTION KEYPAD_HEADSET_KEY_CLEARED

DESCRIPTION
  This function clears the status of the hsed key after Keypad scan has read it.

DEPENDENCIES
  If FEATURE_PMIC and T_PMIC_PM6640 is defined then hsed_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void keypad_headset_key_cleared(void);

/*===========================================================================

FUNCTION KEYPAD_IS_HEADSET_KEY_PRESSED

DESCRIPTION
  This function returns the status of the hsed key.

DEPENDENCIES
  If FEATURE_PMIC and T_PMIC_PM6640 is defined then pm_init must have been called.
  
RETURN VALUE
  Boolean: TRUE,    hsed key is pressed.
           FALSE    hsed key is not pressed.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_headset_key_pressed(void);

/*===========================================================================

FUNCTION KEYPAD_IS_HEADSET_PRESENT

DESCRIPTION
  This function returns status of headset insertion.

DEPENDENCIES
  If FEATURE_PMIC is defined then pm_init must have been calle
  
RETURN VALUE
  Boolean: TRUE,    headset is inserted.
           FALSE    headset is not inserted.

SIDE EFFECTS
  None

===========================================================================*/
boolean keypad_is_headset_present(void);
#endif

#endif /* KEYPAD_H */

