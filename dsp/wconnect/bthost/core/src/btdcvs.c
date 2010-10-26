/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   H O S T   D C V S   M O D U L E

GENERAL DESCRIPTION
  This file implements DCVS specific functionalities for the Blutooth host 


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

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btdcvs.c#2 $
  $DateTime: 2009/06/03 12:14:23 $
  $Author: prgowda $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-06-02   pg  Fixed compiler warning.
  2008-11-29   sa  Added Support for ULC/QSC1100
  2008-10-29   ap  Fixed compiler warnings
  2008-09-06   ap  Added DCVS featurization for Q6270
  2008-08-13   rb  Initial version with DCVS implementation

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT_DCVS

#include "btdcvs.h"

/* Helper macro to check if a module is active */

#define BT_MODULE_SET(x,y)   ( ( x & y) != 0 )

#ifdef FEATURE_BT_USE_CLKRGM_BT
#error code not present
#elif defined(FEATURE_BT_USE_CLKRGM_BT_SERVICES)
#define BT_DCVS_CLIENT CLKRGM_BT_SERVICES 
#endif /* #ifdef FEATURE_BT_USE_CLKRGM_BT */

#ifdef BT_NUM_ACTIVE_MODULES_NEEDED
/* Not needed for now... uncomment if required in future */
/*==========================================================================

  FUNCTION       bt_num_active_modules

  DESCRIPTION    This is a helper routine that returns the no. of active
                 modules

  DEPENDENCIES   None.

  PARAMETERS     bt_module_id_type: bit mask of active modules

  RETURN VALUE  uint8 - No. of active modules 

  SIDE EFFECTS   None.

==========================================================================*/

static uint8 bt_num_active_modules(bt_module_id_type bmask)
{
  uint8  cnt = 0;
  uint32 pf_mask = 0;

  pf_mask = (uint32) bmask;
  while(pf_mask)
  {
    pf_mask = pf_mask & (pf_mask - 1);
    cnt++;
  }
  return cnt;
}
#endif /* #ifdef BT_NUM_ACTIVE_MODULES_NEEDED */

/*==========================================================================

  FUNCTION       bt_switch_to_high_power_mode

  DESCRIPTION    This function maintains the current voting state (high power
                 or low power) for BT and based on the current state calls
                 the clk regime function with appropriate paramaters.

  DEPENDENCIES   None.

  PARAMETERS     bt_module_id_type: bit mask of active modules

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_switch_to_high_power_mode(boolean flag)

{
  static bt_dcvs_power_type clk_mode = BT_DCVS_LOW_POWER_MODE;

/* Currently in low power mode. So, switch to high power mode */
  if ( (flag == TRUE) && (clk_mode == BT_DCVS_LOW_POWER_MODE) )
  {

    BT_MSG_HIGH(" Switching to high power mode", 0, 0, 0); 
/* This API has not changed in a while. If it changes convert this into
 * a macro that uses appropriate api based on target
 */
 #ifdef FEATURE_BT_USE_CLKRGM_BT
    clk_regime_register_for_cpu_resource(BT_DCVS_CLIENT);
#endif
    clk_mode = BT_DCVS_HIGH_POWER_MODE;
  }

/* Currently in high power mode. So, switch to low power mode */
  else if ( (flag == FALSE) && (clk_mode == BT_DCVS_HIGH_POWER_MODE) )
  {
    BT_MSG_HIGH(" Switching to low power mode", 0, 0, 0); 
	 #ifdef FEATURE_BT_USE_CLKRGM_BT
    clk_regime_deregister_for_cpu_resource(BT_DCVS_CLIENT);
	 #endif
    clk_mode = BT_DCVS_LOW_POWER_MODE;
  }
  else
  {
/* Already in the correct power state. No switch needed */
    BT_MSG_HIGH(" Already in correct power state, switch flag = %d", flag, 0, 0); 
  }

  return;
}

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

void bt_vote_for_dcvs( uint32  bit_mask )
{

/* 
 * If FTP or DUN is active, vote for max clock speed.
 * The code segment depends on clk plan. Currently all targets
 * want a high CPU clk only when FTP or NA connection exist.
 */
  if ( BT_MODULE_SET(bit_mask, BT_MODULE_ID_FTP) ||
       BT_MODULE_SET(bit_mask,  BT_MODULE_ID_NA) 
     )
  {
    bt_switch_to_high_power_mode(TRUE); 
  }
  else
  {
    bt_switch_to_high_power_mode(FALSE); 
  }
}

#endif
