

/*                            RF   POWER  COLLAPSE

GENERAL DESCRIPTION

This unit controls the RF portion of the "power collapse" feature. The functions 
in this file saves the registers before power collapse and restore them after 
power collapse.

EXTERNALIZED FUNCTIONS

rf_power_collapse_restore

This function acts as a wrapper for restoring register values while coming 
out of power collapse.

rf_power_collapse_register_save

This function saves the registers before power collapse.

rf_power_collapse_register_restore

This function restores the registers after power collapse.
  
Copyright (c) 2006             by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
06/06/06   hkk     Enabling the Featurizing with RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
06/05/06   hkk     Created the file 

===========================================================================*/

#include "rf.h"
#include "rficap.h"

#ifdef RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
#error code not present
#endif /* RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE */
