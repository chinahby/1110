/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ M C 

GENERAL DESCRIPTION
  This file contains functions that are used to interface with the CDMA2000
  MC.
  
EXTERNALIZED FUNCTIONS
  Returns a pointer to an MC command buffer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_mc.c_v   1.1   18 Nov 2002 18:20:40   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_mc.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/17/02   ak         Updated file header comments.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_mc.h"
#include "err.h"
#include "mc.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_MC_GET_CMD

DESCRIPTION   This function will unqueue an MC command from the
              mc_cmd_free_q and set the REX-related fields to the appropriate
              values prior to returning the pointer to the link.

DEPENDENCIES  None.

RETURN VALUE  A pointer to the initialized MC command link, or NULL

SIDE EFFECTS  None.
===========================================================================*/
mc_msg_type *ds707_mc_get_cmd( void)
{
  mc_msg_type *mc_cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Get an MC command link, load up the REX-related fields, and return the
    pointer
  -------------------------------------------------------------------------*/
  if(( mc_cmd_ptr = (mc_msg_type *)q_get( &mc_cmd_free_q)) == NULL)
  {
    ERR_FATAL( "No MC command links available", 0, 0, 0);
  }
  else /* load up the fields */
  {
    mc_cmd_ptr->hdr.cmd_hdr.task_ptr   = NULL;
    mc_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    mc_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &mc_cmd_free_q;
  }
  return( mc_cmd_ptr);
} /* ds707_mc_get_cmd */


#endif /* FEATURE_DATA_IS707 */
