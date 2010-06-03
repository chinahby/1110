/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Settings Diagnostic Interface 

Supports automated test of Settings via DIAG commands  

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/apps/Core/settingsdiag.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/03   kar     Fixed keymapping due to removal of time and date menu
02/27/03   kp      Created
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "diagcmd.h"
#include "diagpkt.h"

#include "settingsdiag.h"

#include "hsdiag.h"

#ifdef FEATURE_SETTINGS_TEST_AUTOMATION

/*===========================================================================
    Define the keypress table here.
===========================================================================*/

#define SETTINGSDIAG_MAX_KEY_PRESSES      32

HSDIAG_KEYMAP_DECLARE (settingsdiag_keymap)

  /*  Key sequence to get to the Settings screen (3 keys)*/
  HSKM_ACT( SETTINGSDIAG_START ), HSKM( DOWN ), HSKM( RIGHT ), 
    HSKM( SEL ),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (3 keys) */
  HSKM_ACT( SETTINGSDIAG_END ), HSKM( END ), HSKM( LEFT ),
    HSKM( UP ),

/** DISPLAY **/
  /*  Key sequence to get to the Display screen (5 keys)*/
  HSKM_ACT( SETTINGSDIAG_DISPLAY_START ), HSKM_SEQ( SETTINGSDIAG_START), 
    HSKM( DOWN ), HSKM( SEL),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (4 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_END ), HSKM( END ), 
    HSKM_SEQ( SETTINGSDIAG_END),

  /*  Key sequence to get to the top of the Backlight duration setting menu 
      (10 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_MENU_HOME ), 
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_START ), HSKM( SEL ), HSKM( SEL ), HSKM( UP ),
	HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Backlight duration to Always ON and return to main 
      menu home (15 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_ALWAYS_ON ),
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_MENU_HOME ), HSKM( SEL ),
	HSKM( END ), HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ),
	
  /*  Key sequence to set Backlight duration to 10 sec and return to main menu
      home (16 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_10_SEC ), 
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_MENU_HOME ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ), 
	
  /*  Key sequence to set Backlight duration to 30 sec and return to main menu
      home (17 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_30_SEC ), 
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ),

  /*  Key sequence to set Backlight duration to Always OFF and return to main
      menu home (18 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_ALWAYS_OFF ), 
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_BACKLIGHT_DURATION_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( END ), 
	HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ), 

  /*  Key sequence to set Display Auto Hyphen ON and return to main menu
      home (13 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_AUTO_HYPHEN_ON ),
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( UP ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ), 

  /*  Key sequence to set Display Auto Hyphen OFF and return to main menu
      home (13 keys) */
  HSKM_ACT( SETTINGSDIAG_DISPLAY_AUTO_HYPHEN_OFF ),
    HSKM_SEQ( SETTINGSDIAG_DISPLAY_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_DISPLAY_END ), 

  /** EXTRAS **/
  /*  Key sequence to get to the Extras screen (8 keys)*/
  HSKM_ACT( SETTINGSDIAG_EXTRAS_START ), HSKM_SEQ( SETTINGSDIAG_START),
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (4 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_END ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_END),

  /*  Key sequence to get to the SIO Data Baud Menu home (18 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ),
	HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ), 
	HSKM( UP ), 

  /*  Key sequence to set Data Baud Rate to Autobaud and return to main menu
      home (24 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_AUTOBAUD ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( SEL ),
	HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Data Baud Rate to 9600 and return to main menu
      home (25 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_9600 ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Data Baud Rate to 19200 and return to main menu
      home (26 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_19200 ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Data Baud Rate to 38400 and return to main menu
      home (27 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_38400 ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( END ), 
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Data Baud Rate to 57600 and return to main menu
      home (28 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_57600 ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ),HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( END ), 
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Data Baud Rate to 115200 and return to main menu
      home (29 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_115200 ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ),HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Data Baud Rate to 239400 and return to main menu
      home (30 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_239400 ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ),HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Data Baud Rate to 460800 and return to main menu
      home (31 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_460800 ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_SIO_DSBAUD_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ),HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ),HSKM( END ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Auto Answer ON and return to main menu
      home (16 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_ANSWER_ON ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Auto Answer OFF and return to main menu
      home (16 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_ANSWER_OFF ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to get to the Auto Retry Menu home (14 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_RETRY_MENU_HOME ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

/*  Key sequence to set Auto Retry OFF and return to main menu
      home (19 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_RETRY_OFF ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_AUTO_RETRY_MENU_HOME ), HSKM( SEL ),
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Auto Retry 10 sec and return to main menu
      home (20 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_RETRY_10SEC ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_AUTO_RETRY_MENU_HOME ), HSKM( DOWN ),
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Auto Retry 30 sec and return to main menu
      home (21 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_RETRY_30SEC ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_AUTO_RETRY_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Auto Retry 60 sec and return to main menu
      home (22 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_AUTO_RETRY_60SEC ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_AUTO_RETRY_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set One touch ON and return to main menu
      home (18 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_ONE_TOUCH_ON ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),  HSKM( SEL ),
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set One touch OFF and return to main menu
      home (18 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_ONE_TOUCH_OFF ), 
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 

  /*  Key sequence to set Privacy Standard and return to main menu
      home (19 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_PRIVACY_STANDARD ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ),

  /*  Key sequence to set Privacy Enhanced and return to main menu
      home (19 keys) */
  HSKM_ACT( SETTINGSDIAG_EXTRAS_PRIVACY_ENHANCED ),
    HSKM_SEQ( SETTINGSDIAG_EXTRAS_START ),HSKM( DOWN ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_EXTRAS_END ), 
  
/** KEYGAURD **/
  /*  Key sequence to get to the Key Gaurd screen (12 keys)*/
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_START ), HSKM_SEQ( SETTINGSDIAG_START),
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ),
	  HSKM( SEL), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (3 keys) */
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_END ), HSKM_SEQ( SETTINGSDIAG_END),

  /*  Key sequence to set Key Gaurd New and return to main menu
      home (16 keys) */
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_GAURD_NEW ),
    HSKM_SEQ( SETTINGSDIAG_KEYGAURD_START ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_KEYGAURD_END ), 

  /*  Key sequence to set Key Gaurd 15S Autogaurd and return to main menu
      home (17 keys) */
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_15S_AUTOGAURD ), 
    HSKM_SEQ( SETTINGSDIAG_KEYGAURD_START ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_KEYGAURD_END ),

  /*  Key sequence to set Key Gaurd 30S Autogaurd and return to main menu
      home (18 keys) */
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_30S_AUTOGAURD ), 
    HSKM_SEQ( SETTINGSDIAG_KEYGAURD_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_KEYGAURD_END ), 

  /*  Key sequence to set Key Gaurd 1M Autogaurd and return to main menu
      home (19 keys) */
  HSKM_ACT( SETTINGSDIAG_KEYGAURD_1M_AUTOGAURD ), 
    HSKM_SEQ( SETTINGSDIAG_KEYGAURD_START ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_KEYGAURD_END ), 

/** NETWORK **/
  /*  Key sequence to get to the Network screen (10 keys)*/
  HSKM_ACT( SETTINGSDIAG_NETWORK_START ), HSKM_SEQ( SETTINGSDIAG_START), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (4 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_END ), HSKM( END ), HSKM_SEQ( SETTINGSDIAG_END),

  /*  Key sequence to get to the Network Mode Preference menu
      home (19 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_START ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), 
	HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),
 
  /*  Key sequence to set Mode Preference to Automatic and return to main menu
      home (24 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_AUTOMATIC ),
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Mode Preference to AMPS Only and return to main menu
      home (25 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_AMPS_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ), 
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Mode Preference to HDR Only and return to main menu
      home (26 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_HDR_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),  

  /*  Key sequence to set Mode Preference to CDMA & AMPS and return to main menu
      home (27 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_CDMA_AMPS ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),  

  /*  Key sequence to set Mode Preference to CDMA Only and return to main menu
      home (28 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_CDMA_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Mode Preference to Energency calls and return to 
      main menu home (29 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_EMERGENCY_CALLS_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Mode Preference to GSM Only and return to main menu
      home (30 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_GSM_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),  

  /*  Key sequence to set Mode Preference to Digital Only and return to main menu
      home (31 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_DIGITAL_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Mode Preference to WCDMA Only and return to main menu
      home (32 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_MODE_PERF_WCDMA_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_MODE_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),  

  /*  Key sequence to get to the Network Roam Preference menu
      home (15 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_ROAM_PREF_MENU_HOME ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_START ), HSKM( DOWN ), HSKM( DOWN ), 
	HSKM( SEL ), HSKM( UP ), HSKM( UP ),  

  /*  Key sequence to set Roam Preference to Any and return to main menu
      home (20 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_ROAM_PREF_ANY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_ROAM_PREF_MENU_HOME ), HSKM( SEL ),
	HSKM_SEQ( SETTINGSDIAG_NETWORK_END ), 

  /*  Key sequence to set Roam Preference to Home Only and return to main menu
      home (21 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_ROAM_PREF_HOME_ONLY ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_ROAM_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),

  /*  Key sequence to set Roam Preference to Affiliated and return to main menu
      home (22 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_ROAM_PREF_AFFILIATED ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_ROAM_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),

  /*  Key sequence to get to the Network Select Preference menu
      home (16 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_NW_SEL_PREF_MENU_HOME ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_START ), HSKM( DOWN ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), 

  /*  Key sequence to set Network Select Preference to Automatc and return 
      to main menu home (21 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_NW_SEL_PREF_AUTOMATIC ),
    HSKM_SEQ( SETTINGSDIAG_NETWORK_NW_SEL_PREF_MENU_HOME ), HSKM( SEL ),
	HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),

  /*  Key sequence to set Network Select Preference to Mannual and return 
      to main menu home (22 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_NW_SEL_PREF_MANUAL ), 
    HSKM_SEQ( SETTINGSDIAG_NETWORK_NW_SEL_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ),

  /*  Key sequence to set Network Select Preference to Limited Service and 
      return to main menu home (23 keys) */
  HSKM_ACT( SETTINGSDIAG_NETWORK_NW_SEL_PREF_LIMITED_SERVICE ),
    HSKM_SEQ( SETTINGSDIAG_NETWORK_NW_SEL_PREF_MENU_HOME ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( SETTINGSDIAG_NETWORK_END ) 
  

HSDIAG_KEYMAP_END


/*===========================================================================

FUNCTION SETTINGSDIAG_CMD

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * settingsdiag_handler (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)

{
  settingsdiag_cmd_rsp_type *rsp  = NULL;

  SETTINGSDIAG_CommandsType command_code = SETTINGSDIAG_START;

  if (req)
  {
    command_code = diagpkt_subsys_get_cmd_code (req);

    /*  Verify that the command code is valid */
    if (command_code >= SETTINGSDIAG_MAX_CMD)
    {
      rsp = (settingsdiag_cmd_rsp_type *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len);
    }
    else
    {
      rsp = (settingsdiag_cmd_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_SETTINGS,
        command_code, pkt_len);

      if (rsp)
      {
        /*  Send out our packet */
        diagpkt_commit (rsp);

        rsp = NULL;
      }

      /* Call keypress_handler with proper table reference.  The table is 
         constructed using command code as the action code. */
//XXX
        hsdiag_keymap (command_code, settingsdiag_keymap);
    }
  
  }

  return (rsp);

}

/*  Map the diag functions to the proper commands */

static const diagpkt_user_table_entry_type settingsdiag_diag_tbl[] = {
  {SETTINGSDIAG_START, SETTINGSDIAG_MAX_CMD, settingsdiag_handler}
};

void settingsdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_SETTINGS, settingsdiag_diag_tbl);
}


#endif /*FEATURE_SETTINGS_TEST_AUTOMATION*/
