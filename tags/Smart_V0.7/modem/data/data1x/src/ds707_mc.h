#ifndef DS707_MC_H
#define DS707_MC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ M C 

GENERAL DESCRIPTION
  This file contains functions that are used to interface with the CDMA2000
  MC.
  
EXTERNALIZED FUNCTIONS
 ds707_mc_get_cmd() 
   Returns a pointer to an MC command buffer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_mc.h_v   1.1   18 Nov 2002 18:20:46   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_mc.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

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
mc_msg_type *ds707_mc_get_cmd( void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_MC_H         */

