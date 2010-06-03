/*===========================================================================

                         D S 7 0 7 _ P _ R E V
GENERAL DESCRIPTION
  This file contains functions relating the system P_REV, which is determined
  by MC/CM.
  
EXTERNAL FUNCTIONS
  DS707_P_REV_GET_INDEX
    Returns an enum indicating the current p_rev.
    
  DS707_P_REV_CHANGE_HANDLER
    Called by MC when the system P_REV changes.
  
  DS707_P_REV_REGISTER  
    Registers te P_REV change handler with MC.

INITIALIZATION
  Need to register P_REV handler with MC.
    
 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_p_rev.c_v   1.1   18 Nov 2002 21:28:58   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_p_rev.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
0/14/03   sy      Added P_REV 8 & 9 support for Rel B & Rel C.
11/17/02   ak      Updated header comments.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "ds707_p_rev.h"
#include "ds707_so_pkt.h"
#include "err.h"
#include "mccdma.h"

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_P_REV_GET_INDEX

DESCRIPTION   Based on the p_rev_in_use, returns the index into the enum list
              of p_revs.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
word ds707_p_rev_get_index(void)
{
  byte  p_rev_index = 0;                    /* the sys's p_rev index       */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cdma.p_rev_in_use <= 3)
  {
    p_rev_index = (word)DS707_P_REV_3;
  }
  else if (cdma.p_rev_in_use <= 5)
  {
    p_rev_index = (word)DS707_P_REV_5;
  }
  else if (cdma.p_rev_in_use == 6)
  {
    p_rev_index = (word)DS707_P_REV_6;
  }
  else if (cdma.p_rev_in_use == 7)
  {
    p_rev_index = (word)DS707_P_REV_7;
  }
  else if (cdma.p_rev_in_use == 8)
  {
    p_rev_index = (word)DS707_P_REV_8;
  }
  else if (cdma.p_rev_in_use == 9)
  {
    p_rev_index = (word)DS707_P_REV_9;
  }
  else
  {
    ERR_FATAL("Unknown system p_rev %d",cdma.p_rev_in_use,0,0);
  }
  return (p_rev_index);
} /* ds707_p_rev_get_index() */


/*===========================================================================
FUNCTION      DS707_P_REV_CHANGE_HANDLER

DESCRIPTION   Enable/Disable the appropriate Packet Data service options and 
              set up the originating Packet Data service options, based on
              the new base station protocol revision.  This allows the phone
              to intelligently know whether or not to try IS95B or 3G data
              service options.
  
              This function is a callback registered with CM in dsi_cm_init.
              It is called whenever the prev_in_use changes.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_change_handler
(
  cm_ss_event_e_type          ss_event,     /* Type of change that occured */
  const cm_mm_ss_info_s_type *ss_info_ptr   /* Holds the new sys info      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ss_event == CM_SS_EVENT_SRV_CHANGED)               &&
      (ss_info_ptr->changed_fields & CM_SS_BS_P_REV_MASK) &&
      ((ss_info_ptr->sys_mode == SYS_SYS_MODE_CDMA)       ||
       (ss_info_ptr->sys_mode == SYS_SYS_MODE_HDR)            /* UNFINISHED - HDR? */
      )
     )
  {
    /*-------------------------------------------------------------------------
      Re-initialize the service options used, since the p_rev_in_use has
      changed, and system is digital.
    -------------------------------------------------------------------------*/
    ds707_so_pkt_recal();
  }
} /* ds707_p_rev_change_handler() */


/*===========================================================================
FUNCTION      DS707_P_REV_REGISTER

DESCRIPTION   Registers the P_REV with MC/CM.  Called once, at startup.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_register
(
  cm_client_id_type        cm_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_mm_client_ss_reg(cm_id,
                      ds707_p_rev_change_handler,
                      CM_CLIENT_EVENT_REG,
                      CM_SS_EVENT_SRV_CHANGED,
                      CM_SS_EVENT_SRV_CHANGED,
                      NULL);

} /* ds707_p_rev_register() */

#endif /* FEATURE_DATA_IS707 */
