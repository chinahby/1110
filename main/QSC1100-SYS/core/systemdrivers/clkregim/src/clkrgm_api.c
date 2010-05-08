/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Q S C 1 1 0 0    C L O C K    R E G I M E    I N T E R F A C E


GENERAL DESCRIPTION

This module provides clock regime functions that may be featurized so that 
other clock regime sources do have to recompile.


  Copyright (c) 2005 - 2008  by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_api.c#2 $
$DateTime: 2008/01/16 11:20:14 $
$Author: bcalder $

when       who     what, where, why
--------   ---  ----------------------------------------------------------
04/20/05   ev  Removed unused declarations
04/14/05   ev   Removed clk_regime_manage_lcd_for_sleep(void) because it is not
                needed anymore.
4/5/05     ev  Fixed featurization for OEMSBL
4/5/05     ev  Added clk_regime_manage_lcd_for_sleep to be featurized for 
                   ThinUI builds
3/2/05     ev  Added clk_regime_get_usb_xtal_warmup_time_usecs(void).  
                   Added comments.    
2/18/05    ev  Created
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "clkregim.h"

/*===========================================================================

                     INTERNAL DEFINITIONS FOR MODULE

===========================================================================*/
#define CLK_RGM_USB_XTAL_WARMUP_TIME_USECS_DEFAULT 100;


#if 0

/*===========================================================================

FUNCTION CLK_REGIME_GET_MCLK_INIT_SPEED

DESCRIPTION
    Customizable function that allows user to configure intial processor speed. 

DEPENDENCIES
  None

RETURN VALUE
  Desired processor initial speed.

SIDE EFFECTS
  None

===========================================================================*/
clkrgm_cpu_speed_type clk_regime_get_mclk_init_speed(void) 
{
	clkrgm_cpu_speed_type init_speed;
#ifdef FEATURE_CLKREGIM_MCLK_220P8_MHZ
  init_speed =  CLKRGM_CPU_220P8_MHZ;
#else 
  init_speed =  CLKRGM_CPU_146P8_MHZ;
#endif /* FEATURE_CLKREGIM_MCLK_220P8_MHZ */
  return init_speed;
} /* clk_regime_get_mclk_init_speed(void) */
#endif  /* 0 */

/*===========================================================================

FUNCTION CLK_REGIME_GET_USB_XTAL_WARMUP_TIME_USECS

DESCRIPTION
  Customizable function that allows user to configure warmup time on based on 
  USB xtal specs.

DEPENDENCIES
  None

RETURN VALUE
  USB xtal warmup time in microseconds.

SIDE EFFECTS
  None

===========================================================================*/
uint32 clk_regime_get_usb_xtal_warmup_time_usecs(void) 
{
	uint32 warmup_time_usecs;

	warmup_time_usecs = CLK_RGM_USB_XTAL_WARMUP_TIME_USECS_DEFAULT;

	return warmup_time_usecs;
} /* clk_regime_get_usb_xtal_warmup_time_usecs(void) */
