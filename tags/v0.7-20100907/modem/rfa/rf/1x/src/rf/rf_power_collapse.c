
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

  
Copyright (c) 2006, 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_power_collapse.c#1 $
$DateTime: 2008/04/11 05:11:57 $
$Author: ppottier $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   avi     Mainlined clockl regime management (previously featurized for
                   not MSM7600 targets)
03/03/07   dyc     Merge QSC60x5 to common.
           vm      Made changes for RX Front Clock Management to be 
                   based on voting mechanism in clock regime
                   Merged the RX Front for SC2X into the mainline code
03/01/07   sar     Removed compiler warnings.
02/07/07   vm/adk  Added support for firmware TX AGC
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
01/09/07   adk     Updated RXFRONT clk regimes.
01/05/07   adk     Updated RXFRONT clk regimes.
11/15/06   jfc     Removed HWIO shadow defines. Replaced with RF HAL macros.
                    Added perforce keywords
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial 7600 build.
06/23/06   hkk     Changes for Power collapse
06/15/06   pa      Removed srch_afc.h to pack correctly.
06/06/06   hkk     Enabling the Featurizing with RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
06/05/06   hkk     Created the file 

===========================================================================*/

#include "rficap.h"
#include "rf_hal.h"

#ifdef RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
#error code not present
#endif /* RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE */

/* Dummy function to prevent a compiler warning */
void rf_power_collapse_dummy_function(void);

#ifdef RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE
#error code not present
#else /*RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE */

void rf_power_collapse_dummy_function(void)
{
  int i;
  for (i=0; i<1; i++)
  {
    i=i;
  }
}

#endif /*RF_HAS_SUPPORT_FOR_FULL_POWER_COLLAPSE */



