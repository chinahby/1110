/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               S I D 

GENERAL DESCRIPTION
  This file contains functions used to handle SID changes.

EXTERNALIZED FUNCTIONS
  DS707_SID_ENABLE_RECON
    Enables reconnection out of dormancy when the SID changes.
    
  DS707_SID_DISABLE_RECON
    Disables reconnection out of dormancy when the SID changes.
    
  DS707_SID_CHANGE_HANDLER
    Registered with MC. Called by MC when the SID changes.  This will send
    a msg to 1X Pkt mgr, telling it to come out of dormancy.
    
  DS707_SID_REGISTER
    Registers the SID change handler with MC.
    
  DS707_SID_DEREGISTER
    Deregisters the SID change handler with MC.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None, though should call ds707_sid_register() at startup if want the 
  phone to re-orig out of dormancy when the SID changes.

   Copyright (c) 1999 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sid.c_v   1.1   18 Nov 2002 18:32:54   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_sid.c#3 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/30/09   ms      Added Support for EPZID Hysteresis.
08/25/03   ak      Enable SID by default.
11/15/02   ak      Updated file header comments.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "assert.h"
#include "ds707.h"
#include "ds707_sid.h"
#ifndef FEATURE_DATA_EPZID
#include "ds707_pzid.h"
#else
#include "ds707_epzid.h"
#endif /* FEATURE_DATA_EPZID */
#include "ds707_dorm_timer.h"

#include "dstask.h"

#include "err.h"
#include "msg.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  SID based reconnection is turned on by default.  
---------------------------------------------------------------------------*/
LOCAL boolean sidi_recon_enabled = TRUE;


/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_SID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_enable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sidi_recon_enabled = TRUE;
}

/*===========================================================================
FUNCTION       DS707_SID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on SID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_sid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sidi_recon_enabled = FALSE;
}

/*===========================================================================
FUNCTION       DS707_SID_CHANGE_HANDLER

DESCRIPTION    Reset the Packet Data Dormant Timer to its default value, per
               IS-707-A.5/9 Section 2.2.6.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Resets the ds_dormant_timer value to its default value.
===========================================================================*/
void ds707_sid_change_handler
(
  word  old_sid,                                 /* Old System Id          */
  word  new_sid                                  /* New System Id          */
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(old_sid == new_sid)
  {
    return;
  }

  MSG_MED(" SID changed, old=%d, new=%d", old_sid, new_sid, 0);

  /*-------------------------------------------------------------------------
    Reset the Packet Data Dormant Timer to its default value.
  -------------------------------------------------------------------------*/
  ds707_dorm_timer_reset_val();

#ifdef FEATURE_DATA_IS707_PZID_RECON
  /*-------------------------------------------------------------------------
     Disable PZID based reconnection.
  -------------------------------------------------------------------------*/
#ifndef FEATURE_DATA_EPZID
  ds707_pzid_disable_recon();
#else
  ds707_epzid_disable_recon();
#endif /* FEATURE_DATA_PZID*/
#endif /* FEATURE_DATA_IS707_PZID_RECON */

  /*-------------------------------------------------------------------------
    May need to reconnect based on SID change.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_IS707_SID_CHNG_RECON
  if (sidi_recon_enabled == TRUE)
  {
    MSG_HIGH("SID Change - reconnect",0,0,0);
    if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
    {
      ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_707_PKT_SID_CHANGE_CMD;
      ds_put_cmd( cmd_ptr );
    }
  }
#else
  MSG_HIGH("SID Chng recon FEATURE OFF",0,0,0);
#endif /* FEATURE_DATA_IS707_SID_CHNG_RECON */

} /* ds707_sid_change_handler() */

#endif /* FEATURE_DATA_IS707 */
