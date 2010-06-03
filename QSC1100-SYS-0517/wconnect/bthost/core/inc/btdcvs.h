/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   H O S T   D C V S   M O D U L E

GENERAL DESCRIPTION
  This file implements the api for DCVS specific functionalities
  for the Blutooth host 


EXTERNALIZED FUNCTIONS
  bt_vote_for_dcvs
   Vote for higher clock speed based on the bit mask provided by rm module.
   Profile layers indicate to rm whether they are active at any given point.
   RM layer processes these indications from the profile layers and provides
   a bit mask that indicates all currently active modules. DCVS layer 
   processes the bit mask and votes for appropriate clock speed based on the
   target.


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008-2009 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/inc/btdcvs.h#2 $
  $DateTime: 2009/06/03 12:14:23 $
  $Author: prgowda $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-06-02   pg  Fixed compiler warning.
  2008-10-29   ap  Fixed typo in comment
  2008-08-13   rb  Initial version with DCVS implementation
==========================================================================*/
#ifndef _BTDCVS_H
#define _BTDCVS_H

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT_DCVS

#include "clkregim.h"
#include "btmsg.h"
#include "bt.h"

/* Two Power modes for BT voting. High power and low power*/
typedef enum
{
  BT_DCVS_LOW_POWER_MODE =  0,
  BT_DCVS_HIGH_POWER_MODE 
} bt_dcvs_power_type;

/*==========================================================================

  FUNCTION       bt_vote_for_dcvs

  DESCRIPTION    This function takes a bit mask of active modules and based
                 on the voting plan for a target votes for the appropriate
                 clock speed.

  DEPENDENCIES   None.

  PARAMETERS     bt_module_id_type: bit mask of active modules

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_vote_for_dcvs( uint32 bit_mask );

#endif

#endif
