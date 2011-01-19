/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                        D A T A   S E R V I C E S

                                   N I D 

GENERAL DESCRIPTION
  This file contains functions used to handle NID changes.

EXTERNALIZED FUNCTIONS
  DS707_NID_ENABLE_RECON
    Enables a packet reconnection when the NID changes.
    
  DS707_NID_DISABLE_RECON
    Disables a packet reconnection when the NID changes.
    
  DS707_NID_CHANGE_HANDLER
    Registered with MC.  Called when the NID changes.  If enabled, will 
    send a msg to DS pkt mgr, telling it to come out of dormancy.
    
  DS707_NID_REGISTER
    Registers the NID change handler with MC.
    
  DS707_NID_DEREGISTER
    De-registers the NID change handler with MC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None, though to have this function work, need to call ds707_nid_register(),
  so that MC can call the NID change handler when the NID changes.

   Copyright (c) 1999 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_nid.c_v   1.1   18 Nov 2002 18:26:00   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_nid.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/03   ak      Enable NID by default.
11/15/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707.h"
#include "ds707_nid.h"

#include "dstask.h"
#include "msg.h"
#include "err.h"
#include "assert.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*---------------------------------------------------------------------------
  NID based reconnection is turned on by default.  
---------------------------------------------------------------------------*/
LOCAL boolean nidi_recon_enabled = TRUE;


/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_NID_ENABLE_RECON

DESCRIPTION    Enables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_enable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nidi_recon_enabled = TRUE;
}

/*===========================================================================
FUNCTION       DS707_NID_DISABLE_RECON

DESCRIPTION    Disables reconnection based on NID change.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_nid_disable_recon(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nidi_recon_enabled = FALSE;
}

/*===========================================================================
FUNCTION       DS707_NID_CHANGE_HANDLER

DESCRIPTION    Causes a data reconnection.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_nid_change_handler
(
  word  old_nid,                                 /* Old Network Id         */
  word  new_nid                                  /* New Network Id         */
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(old_nid == new_nid)
  {
    return;
  }

  MSG_MED(" NID changed, old=%d, new=%d", old_nid, new_nid, 0);

  /*-------------------------------------------------------------------------
    May need to reconnect based on NID change.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DATA_IS707_NID_CHNG_RECON
  if (nidi_recon_enabled == TRUE)
  {
    MSG_HIGH("NID Change - reconnect",0,0,0);
    if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
    {
      ERR_FATAL( "CM can't get cmd buf from DS task", 0, 0, 0 );
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_707_PKT_NID_CHANGE_CMD;
      ds_put_cmd( cmd_ptr );
    }
  }
#else
  MSG_HIGH("NID Recon FEATURE OFF", 0, 0, 0);
#endif /* FEATURE_DATA_IS707_NID_CHNG_RECON */

} /* ds707_nid_change_handler() */

#endif /* FEATURE_DATA_IS707 */
