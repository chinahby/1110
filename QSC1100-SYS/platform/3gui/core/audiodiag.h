#ifndef AUDIODIAG_H
#define AUDIODIAG_H

/*============================================================================
   Subsystem command code
============================================================================*/

#define APPS_SUBSYS_AUDIO_SETTINGS_CMD 0x0001


/*============================================================================
   Enumerated types for audio settings diag
============================================================================*/

/*
    Enumerated type defining available application command codes for portmap
    settings.
*/

typedef enum
{
  AUDIODIAG_START = 0,
  AUDIODIAG_END,
  AUDIODIAG_TTY_MENU_HOME,
  AUDIODIAG_TTY_FULL,
  AUDIODIAG_TTY_TALK,
//5
  AUDIODIAG_TTY_HEAR,
  AUDIODIAG_TTY_OFF,
  AUDIODIAG_SILENCE_ALL_ENABLE,
  AUDIODIAG_SILENCE_ALL_DISABLE,
  AUDIODIAG_SERVICE_ALERT_ENABLE,
//10
  AUDIODIAG_SERVICE_ALERT_DISABLE,
  AUDIODIAG_SMART_SOUND_ENABLE,
  AUDIODIAG_SMART_SOUND_DISABLE,
  AUDIODIAG_BEEP_LENGTH_NORMAL,
  AUDIODIAG_BEEP_LENGTH_LONG,
//15  
  AUDIODIAG_RING_TYPE_MENU_HOME,
  AUDIODIAG_RING_TYPE_STANDARD,
  AUDIODIAG_RING_TYPE_OPTION_1,
  AUDIODIAG_RING_TYPE_OPTION_2,
  AUDIODIAG_RING_TYPE_OPTION_3,
//20
  AUDIODIAG_RING_TYPE_OPTION_4,
  AUDIODIAG_RINGER_VOL_MENU_HOME,
  AUDIODIAG_RINGER_VOL_OFF,
  AUDIODIAG_RINGER_VOL_LVL_1,
  AUDIODIAG_RINGER_VOL_LVL_2,
//25  
  AUDIODIAG_RINGER_VOL_LVL_3,
  AUDIODIAG_RINGER_VOL_LVL_4,
  AUDIODIAG_KEY_BEEP_VOL_MENU_HOME,
  AUDIODIAG_KEY_BEEP_VOL_OFF,
  AUDIODIAG_KEY_BEEP_VOL_LVL_1,
//30
  AUDIODIAG_KEY_BEEP_VOL_LVL_2,
  AUDIODIAG_KEY_BEEP_VOL_LVL_3,
  AUDIODIAG_KEY_BEEP_VOL_LVL_4,
  AUDIODIAG_EARPIECE_VOL_MENU_HOME,
  AUDIODIAG_EARPIECE_VOL_OFF,
//35
  AUDIODIAG_EARPIECE_VOL_LVL_1,
  AUDIODIAG_EARPIECE_VOL_LVL_2,
  AUDIODIAG_EARPIECE_VOL_LVL_3,
  AUDIODIAG_EARPIECE_VOL_LVL_4,
  AUDIODIAG_MESSAGE_VOL_MENU_HOME,
//40
  AUDIODIAG_MESSAGE_VOL_OFF,
  AUDIODIAG_MESSAGE_VOL_LVL_1,
  AUDIODIAG_MESSAGE_VOL_LVL_2,
  AUDIODIAG_MESSAGE_VOL_LVL_3,
  AUDIODIAG_MESSAGE_VOL_LVL_4,
//45
  AUDIODIAG_MAX_CMD,
  AUDIODIAG_FORCE_4_BYTES = 0x10000000
} AUDIODIAG_CommandsType;

/*============================================================================

PACKET   AUDIODIAG_CMD

PURPOSE  Sent from the DM to the DMSS to set a particular audio setting.

RESPONSE The DM sends back an audiodiag_cmd_rsp_type packet to give the 
         status of the request.

============================================================================*/

typedef struct
{
  diagpkt_subsys_header_type header;

} audiodiag_cmd_rsp_type;

#endif /*  AUDIODIAG_H */
