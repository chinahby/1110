/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Audio Settings Diagnostic Interface 

Supports automated test of the audio settings via DIAG commands  

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/apps/Core/audiodiag.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/06   cvs     Fix compile warnings
02/27/03   kp      Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diagcmd.h"
#include "diagpkt.h"

#include "audiodiag.h"

#include "hsdiag.h"

#ifdef FEATURE_AUDIOSETTINGS_TEST_AUTOMATION

/*===========================================================================
    Define the keypress table here.
===========================================================================*/


HSDIAG_KEYMAP_DECLARE (audiodiag_keymap)

  /*  Key sequence to get to the audio settings menu from the main
      menu's top left icon (3 keys)*/
  HSKM_ACT( AUDIODIAG_START ), HSKM( DOWN ), HSKM( RIGHT ), HSKM( SEL ), HSKM(SEL),

  /*  Key sequence to get back to the main menu and highlight the top left 
      icon (3 keys) */
  HSKM_ACT( AUDIODIAG_END   ), HSKM( END ), HSKM( UP ), HSKM( LEFT ),

  /*  Key sequence to get to the top of the TTY setting menu (13 keys) */
  HSKM_ACT( AUDIODIAG_TTY_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set TTY Full and return to main menu home (17 keys) */
  HSKM_ACT( AUDIODIAG_TTY_FULL ), HSKM_SEQ( AUDIODIAG_TTY_MENU_HOME ), 
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set TTY Talk and return to main menu home (18 keys) */
  HSKM_ACT( AUDIODIAG_TTY_TALK ), HSKM_SEQ( AUDIODIAG_TTY_MENU_HOME ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set TTY Hear and return to main menu home (19 keys) */
  HSKM_ACT( AUDIODIAG_TTY_HEAR ), HSKM_SEQ( AUDIODIAG_TTY_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set TTY Off and return to main menu home (20 keys) */
  HSKM_ACT( AUDIODIAG_TTY_OFF ), HSKM_SEQ( AUDIODIAG_TTY_MENU_HOME ), HSKM( DOWN ), 
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Enable Silence All and return to main menu home (14 keys) */
  HSKM_ACT( AUDIODIAG_SILENCE_ALL_ENABLE ), HSKM_SEQ( AUDIODIAG_START ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Disable Silence All and return to main menu home (15 keys) */
  HSKM_ACT( AUDIODIAG_SILENCE_ALL_DISABLE ), HSKM_SEQ( AUDIODIAG_START ), HSKM( DOWN ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Enable Service Alert and return to main menu home (13 keys) */
  HSKM_ACT( AUDIODIAG_SERVICE_ALERT_ENABLE ), HSKM_SEQ( AUDIODIAG_START ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Enable Service Alert and return to main menu home (14 keys) */
  HSKM_ACT( AUDIODIAG_SERVICE_ALERT_DISABLE ), HSKM_SEQ( AUDIODIAG_START ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Enable Smart Sound and return to main menu home (12 keys) */
  HSKM_ACT( AUDIODIAG_SMART_SOUND_ENABLE ), HSKM_SEQ( AUDIODIAG_START ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to Disable Smart Sound and return to main menu home (13 keys) */
  HSKM_ACT( AUDIODIAG_SMART_SOUND_DISABLE ), HSKM_SEQ( AUDIODIAG_START ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set normal beep length and return to main menu home (11 keys) */
  HSKM_ACT( AUDIODIAG_BEEP_LENGTH_NORMAL ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set long beep length and return to main menu home (12 keys) */
  HSKM_ACT( AUDIODIAG_BEEP_LENGTH_LONG ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( DOWN ), HSKM( SEL ), 
	HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to get to the top of the ring type menu (9 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Ring Type to Standard (13 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_STANDARD ), HSKM_SEQ( AUDIODIAG_RING_TYPE_MENU_HOME ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ring Type to Option 1 (14 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_OPTION_1 ), HSKM_SEQ( AUDIODIAG_RING_TYPE_MENU_HOME ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ring Type to Option 2 (15 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_OPTION_2 ), HSKM_SEQ( AUDIODIAG_RING_TYPE_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ring Type to Option 3 (16 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_OPTION_3 ), HSKM_SEQ( AUDIODIAG_RING_TYPE_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ring Type to Option 4 (17 keys) */
  HSKM_ACT( AUDIODIAG_RING_TYPE_OPTION_4 ), HSKM_SEQ( AUDIODIAG_RING_TYPE_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to get to the top of the ringer volume menu (9 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( SEL ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Ringer Volume to Off (13 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_OFF ), HSKM_SEQ( AUDIODIAG_RINGER_VOL_MENU_HOME ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ringer Volume to Level 1 (14 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_LVL_1 ), HSKM_SEQ( AUDIODIAG_RINGER_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ringer Volume to Level 2 (15 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_LVL_2 ), HSKM_SEQ( AUDIODIAG_RINGER_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ringer Volume to Level 3 (16 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_LVL_3 ), HSKM_SEQ( AUDIODIAG_RINGER_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Ringer Volume to Level 4 (17 keys) */
  HSKM_ACT( AUDIODIAG_RINGER_VOL_LVL_4 ), HSKM_SEQ( AUDIODIAG_RINGER_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to get to the top of the Key Beep Volume menu (9 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( SEL ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Key Beep Volume to Off (13 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_OFF ), HSKM_SEQ( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Key Beep Volume to Level 1 (14 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_LVL_1 ), HSKM_SEQ( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Key Beep Volume to Level 2 (15 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_LVL_2 ), HSKM_SEQ( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Key Beep Volume to Level 3 (16 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_LVL_3 ), HSKM_SEQ( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Key Beep Volume to Level 4 (17 keys) */
  HSKM_ACT( AUDIODIAG_KEY_BEEP_VOL_LVL_4 ), HSKM_SEQ( AUDIODIAG_KEY_BEEP_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to get to the top of the Earpiece Volume menu (9 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Earpiece Volume to Off (13 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_OFF ), HSKM_SEQ( AUDIODIAG_EARPIECE_VOL_MENU_HOME ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Earpiece Volume to Level 1 (14 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_LVL_1 ), HSKM_SEQ( AUDIODIAG_EARPIECE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Earpiece Volume to Level 2 (15 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_LVL_2 ), HSKM_SEQ( AUDIODIAG_EARPIECE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Earpiece Volume to Level 3 (16 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_LVL_3 ), HSKM_SEQ( AUDIODIAG_EARPIECE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Earpiece Volume to Level 4 (17 keys) */
  HSKM_ACT( AUDIODIAG_EARPIECE_VOL_LVL_4 ), HSKM_SEQ( AUDIODIAG_EARPIECE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to get to the top of the Message Volume menu (9 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_MENU_HOME ), HSKM_SEQ( AUDIODIAG_START ),
    HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set Message Volume to Off (13 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_OFF ), HSKM_SEQ( AUDIODIAG_MESSAGE_VOL_MENU_HOME ),
	HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Message Volume to Level 1 (14 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_LVL_1 ), HSKM_SEQ( AUDIODIAG_MESSAGE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Message Volume to Level 2 (15 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_LVL_2 ), HSKM_SEQ( AUDIODIAG_MESSAGE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Message Volume to Level 3 (16 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_LVL_3 ), HSKM_SEQ( AUDIODIAG_MESSAGE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END ),

  /*  Key sequence to set Message Volume to Level 4 (17 keys) */
  HSKM_ACT( AUDIODIAG_MESSAGE_VOL_LVL_4 ), HSKM_SEQ( AUDIODIAG_MESSAGE_VOL_MENU_HOME ),
	HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( AUDIODIAG_END )

HSDIAG_KEYMAP_END



/*===========================================================================

FUNCTION AUDIODIAG_HANDLER

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * audiodiag_handler (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)

{
  audiodiag_cmd_rsp_type *rsp  = NULL;

  AUDIODIAG_CommandsType command_code = AUDIODIAG_START;

  if (req)
  {
    command_code = (AUDIODIAG_CommandsType) diagpkt_subsys_get_cmd_code (req);

    /*  Verify that the command code is valid */
    if (command_code >= AUDIODIAG_MAX_CMD)
    {
      rsp = (audiodiag_cmd_rsp_type *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len);
    }
    else
    {
      rsp = (audiodiag_cmd_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_AUDIO_SETTINGS,
        (uint16) command_code, pkt_len);

      if (rsp)
      {
        /*  Send out our packet */
        diagpkt_commit (rsp);

        rsp = NULL;
      }

      /* Call keypress_handler with proper table reference.  The table is 
         constructed using command code as the action code. */
//XXX
      hsdiag_keymap (command_code, audiodiag_keymap);
    }
  
  }

  return (rsp);

}

/*  Map the diag functions to the proper commands */

static const diagpkt_user_table_entry_type audiodiag_diag_tbl[] = {
  {AUDIODIAG_START, AUDIODIAG_MAX_CMD, audiodiag_handler}
};

void audiodiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_AUDIO_SETTINGS, audiodiag_diag_tbl);
}

#endif /* FEATURE_AUDIOSETTINGS_TEST_AUTOMATION */

