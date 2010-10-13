#ifndef CUSTUI_H
#define CUSTUI_H
/*===========================================================================

DESCRIPTION
  Configuration for UI

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custui.h#2 $ $DateTime: 2009/04/21 05:35:38 $ $Author: gnedanur $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** User Interface
** ----------------------------------------------------------------------- */

/* Support BSP LCD/KEYPAD API
*/
#define FEATURE_BSPUI_API

/* Support of 5GP UI Board
*/
#define FEATURE_ODIE_FONT

/* UI To Indicate on Display if Auto Answer is Enabled
*/
#undef  FEATURE_UI_AUTO_ANS_INDICATE

/* UI "case" screen to use "A->a" (otherwize "X->x")
*/
#undef  FEATURE_UI_CASE_USE_A

/* UI To Support Sending PIN when requested
*/
#undef  FEATURE_UI_SEND_PIN

/* UI To Show "Dropped Call"
*/
#define FEATURE_UI_SHOW_DROP_CALL

/* UI Provides some Alternate Behaviors for the Power Key on Power On and
** Power Off.
*/
#define  FEATURE_UI_PWR_KEY_ALT

/* In alpha edit, the second line will prompt "Enter Name: "
*/
#undef  FEATURE_UI_ENTER_NAME_PROMPT

/* Save the redial number through a power-cycle
*/
#undef  FEATURE_UI_SAVE_REDIAL_BUFFER

/* When this feature is turned on, Old Markov Call is supported.
*/
#undef  FEATURE_UI_OLD_MARKOV

/* Envelope icon is used for voice mail also, just like text messages
*/
#undef  FEATURE_UI_ENVELOPE_VM_ICON

/* Default the IMSI to all 0's is okay
*/
#undef  FEATURE_UI_IMSI_ALL_0_OK

/* Voice Privacy Alert if the Service Alert is enabled.  The first entry
** into enhanced mode per call is ignored so that is doesn't beep in every
** call.  From then on beep every time it toggles.
*/
#undef  FEATURE_UI_PRIVACY_ALERT

/*  Shows preferred Roaming List Version
*/
#define FEATURE_UI_PRL_VER

/* handle forward DTMF */
#define FEATURE_HANDLE_FWD_DTMF

/* Reorganize Menus when using FEATURE_AUDIO_AGC.
** Puts Audio AGC in the sounds menu rather than features menu.
*/
#define FEATURE_MENU_REORG

/* Enable Battery and Charger Displays
*/
#undef FEATURE_UI_LOW_BATT_W_EXT_PWR

#ifdef FEATURE_BUILD_CDMA_TRIMODE
   /* UI To Support "Force Call" in its.
   */
   #define FEATURE_UI_FORCE_CALL
   #define FEATURE_UI_FORCE_MODE_SEL
#endif

/* Enables the Phone Book Manager interface with existing UI menus 
*/
#ifndef FEATURE_MANGO_BREW
//#define FEATURE_UI_PBM
#endif
/* Enable dynamic memory management
*/
#define FEATURE_REX_DYNA_MEM_UI
  
#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#endif /* CUSTUI_H */

