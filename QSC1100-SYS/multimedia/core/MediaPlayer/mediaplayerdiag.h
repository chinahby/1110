#ifndef MEDIAPLAYERDIAG_H
#define MEDIAPLAYERDIAG_H

/*============================================================================
   Enumerated types for audio settings diag
============================================================================*/

/*
    Enumerated type defining available application command codes for portmap
    settings.
*/

typedef enum
{
  MEDIAPLAYERDIAG_START = 0,
  MEDIAPLAYERDIAG_END,
  MEDIAPLAYERDIAG_PLAY,
  MEDIAPLAYERDIAG_STOP,
  MEDIAPLAYERDIAG_PAUSE,
//5
  MEDIAPLAYERDIAG_RESUME,
  MEDIAPLAYERDIAG_VOL_UP,
  MEDIAPLAYERDIAG_VOL_DOWN,
  MEDIAPLAYERDIAG_FAST_FORWARD,
  MEDIAPLAYERDIAG_REWIND,
//10
  MEDIAPLAYERDIAG_VOL_OFF,
  MEDIAPLAYERDIAG_VOL_LVL1,
  MEDIAPLAYERDIAG_VOL_LVL2,
  MEDIAPLAYERDIAG_VOL_LVL3,
  MEDIAPLAYERDIAG_VOL_LVL4,
//15
  MEDIAPLAYERDIAG_SET_CONT_MODE_OFF,
  MEDIAPLAYERDIAG_SET_CONT_MODE_SINGLE,
  MEDIAPLAYERDIAG_SET_CONT_MODE_ALL,
  MEDIAPLAYERDIAG_SET_AUTO_PLAY_ON,
  MEDIAPLAYERDIAG_SET_AUTO_PLAY_OFF,
//20
  MEDIAPLAYERDIAG_MAX_CMD,
  MEDIAPLAYERDIAG_FORCE_4_BYTES = 0x10000000
} MEDIAPLAYERDIAG_CommandsType;

/*============================================================================

PACKET   MEDIAPLAYERDIAG_CMD

PURPOSE  Sent from the DM to the DMSS to issue a particular Mobimon command.

RESPONSE The DM sends back an mediaplayerdiag_cmd_rsp_type packet to give the 
         status of the request.

============================================================================*/

typedef struct
{
  diagpkt_subsys_header_type header;

} mediaplayerdiag_cmd_rsp_type;

#endif /*  MEDIAPLAYERDIAG_H */



