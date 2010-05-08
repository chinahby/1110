/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Media Player Diagnostic Interface 

Supports automated test of the media player via DIAG commands  

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/apps/MediaPlayer/mediaplayerdiag.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/03   kp      Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diagcmd.h"
#include "diagpkt.h"

#include "mediaplayerdiag.h"

#include "hsdiag.h"

#ifdef FEATURE_MEDIAPLAYER_TEST_AUTOMATION

/*===========================================================================
    Define the keypress table here.
===========================================================================*/
/* *INDENT-OFF* */
HSDIAG_KEYMAP_DECLARE (mediaplayerdiag_keymap)

  HSKM_ACT( MEDIAPLAYERDIAG_START ), HSKM( DOWN ), HSKM( RIGHT ), HSKM( RIGHT ), 
    HSKM( SEL ),

  HSKM_ACT( MEDIAPLAYERDIAG_END ), HSKM( END ),  

  HSKM_ACT( MEDIAPLAYERDIAG_PLAY ), HSKM( SEL ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( LEFT), 

  HSKM_ACT( MEDIAPLAYERDIAG_STOP ), HSKM( RIGHT ), HSKM( SEL ), 

  HSKM_ACT( MEDIAPLAYERDIAG_PAUSE ), HSKM( RIGHT ), HSKM( RIGHT ), HSKM( RIGHT ), 
  HSKM( SEL ), HSKM( LEFT ), HSKM( LEFT ), HSKM( LEFT ),

  HSKM_ACT( MEDIAPLAYERDIAG_RESUME), HSKM( SEL ), HSKM( LEFT ), HSKM( LEFT ),  
    HSKM( LEFT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_UP ), HSKM( LEFT ), HSKM( LEFT ), HSKM( SEL ), 
    HSKM( RIGHT ), HSKM( RIGHT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_DOWN), HSKM( LEFT ), HSKM( SEL ), HSKM( RIGHT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_FAST_FORWARD), HSKM( RIGHT ), HSKM( RIGHT ), 
    HSKM( RIGHT ), HSKM( RIGHT ), HSKM( SEL ), HSKM( LEFT ), HSKM( LEFT ), 
    HSKM( LEFT ), HSKM( LEFT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_REWIND), HSKM( RIGHT ), HSKM( RIGHT ),  
    HSKM( SEL ), HSKM( LEFT ), HSKM( LEFT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_OFF ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( SEL ), HSKM( SEL ), HSKM( RIGHT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_LVL1 ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( SEL ), HSKM( SEL ), HSKM( LEFT ), HSKM( SEL ), HSKM( RIGHT ), 
    HSKM( RIGHT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_LVL2 ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( SEL ), HSKM( SEL ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), HSKM( RIGHT ), 
    HSKM( RIGHT ), 

  HSKM_ACT( MEDIAPLAYERDIAG_VOL_LVL3 ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( SEL ), HSKM( SEL ), HSKM( LEFT ), HSKM( SEL ), HSKM( SEL ), HSKM( SEL ), 
    HSKM( RIGHT ), HSKM( RIGHT ), 
  
  HSKM_ACT( MEDIAPLAYERDIAG_VOL_LVL4 ), HSKM( LEFT ), HSKM( LEFT ), HSKM( SEL ), 
    HSKM( SEL ), HSKM( SEL ), HSKM( SEL ),  HSKM( RIGHT ), HSKM( RIGHT ),

  HSKM_ACT( MEDIAPLAYERDIAG_SET_CONT_MODE_OFF), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM( CLR ), HSKM( UP ), HSKM( UP ),   

  HSKM_ACT( MEDIAPLAYERDIAG_SET_CONT_MODE_SINGLE), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), 
    HSKM( SEL ), HSKM( CLR ), HSKM( UP ), HSKM( UP ),   

  HSKM_ACT( MEDIAPLAYERDIAG_SET_CONT_MODE_ALL), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( UP ), HSKM( SEL ), HSKM( CLR ), HSKM( UP ), HSKM( UP ),   

  HSKM_ACT( MEDIAPLAYERDIAG_SET_AUTO_PLAY_ON), HSKM( DOWN ), HSKM( DOWN ),  
    HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( SEL ), 
    HSKM( CLR ), HSKM( UP ), HSKM( UP ),

  HSKM_ACT( MEDIAPLAYERDIAG_SET_AUTO_PLAY_OFF), HSKM( DOWN ), HSKM( DOWN ),  
    HSKM( SEL ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM( CLR ), HSKM( UP ), HSKM( UP )

HSDIAG_KEYMAP_END
/*===========================================================================

FUNCTION MEDIAPLAYERDIAG_CMD

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void * mediaplayerdiag_handler (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)

{
  mediaplayerdiag_cmd_rsp_type *rsp  = NULL;

  MEDIAPLAYERDIAG_CommandsType command_code = MEDIAPLAYERDIAG_START;

  if (req)
  {
    command_code = (MEDIAPLAYERDIAG_CommandsType)diagpkt_subsys_get_cmd_code (req);

    /*  Verify that the command code is valid */
    if (command_code >= MEDIAPLAYERDIAG_MAX_CMD)
    {
      rsp = (mediaplayerdiag_cmd_rsp_type *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len);
    }
    else
    {
      rsp = (mediaplayerdiag_cmd_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_MEDIAPLAYER,
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
      hsdiag_keymap (command_code, mediaplayerdiag_keymap);
    }
  
  }

  return (rsp);

}

/*  Map the diag functions to the proper commands */

static const diagpkt_user_table_entry_type mediaplayerdiag_diag_tbl[] = {
  {MEDIAPLAYERDIAG_START, MEDIAPLAYERDIAG_MAX_CMD, mediaplayerdiag_handler}
};

void mediaplayerdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_MEDIAPLAYER, mediaplayerdiag_diag_tbl);
}

#endif /* FEATURE_MEDIAPLAYER_TEST_AUTOMATION */


