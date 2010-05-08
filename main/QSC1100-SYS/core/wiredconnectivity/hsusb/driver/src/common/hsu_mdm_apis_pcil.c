/*==============================================================================

PCIL APIs for High Speed USB Module Remote APIs on Modem processor 

GENERAL DESCRIPTION
This module defines PCIL APIs for High Speed USB APIs that are implemented on the modem 
processor. 

EXTERNALIZED FUNCTIONS
hsu_api_init_phy_pcil

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/01/09  me  Initial version
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_mdm_apis.h"
#include "hsu_mdm_apis_pcil.h"
#include "smem_pcmod.h"

/*===========================================================================

FUNCTION      HSU_API_INIT_PHY_PCIL

DESCRIPTION
The function performs HSUSB PHY intialization.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
The function allways returns TRUE.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_api_init_phy_pcil(uint32* ignored_param_a, uint32* ignored_param_b)
{
  hsu_api_init_phy(); 
  return TRUE;
}


/*===========================================================================

FUNCTION      HSU_REGISTER_PCIL_CMDS

DESCRIPTION
Registers the functions in hsu_mdm_apis_pcil.c with proc_comm.

DEPENDENCIES
FEATURE_HS_USB must be defined. 

RETURN VALUE
TRUE if all functions were successfully registered.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_register_pcil_cmds(void)
{
  boolean ret_val = FALSE;

  ret_val = SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_MSM_HSUSB_PHY_RESET, 
    &hsu_api_init_phy_pcil,
    FALSE);
}
