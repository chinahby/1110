/*==============================================================================

H S U _ P H Y _ U T I L S . C


GENERAL DESCRIPTION
High Speed USB general utility functions for PHY (external/internal), 
to be used whether or not the HS-USB stack is running on the processor invoking
these functions.

EXTERNALIZED FUNCTIONS
hsu_os_bus_spoof_disconnect
hsu_phy_utils_spoof_connect
hsu_phy_utils_enable_lpm_exit
hsu_phy_utils_disable_lpm_exit

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2008, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_phy_utils.c#9 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/27/08  esh     Initial version

==============================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "msm.h"
#include "clkrgm_msm.h"
#include "hsu_common.h"
#include "HAL_hsusb.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Externalized Variables
==============================================================================*/
/*==============================================================================
File Static Variables
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_DISCONNECT

DESCRIPTION
This function causes the device to disconnect from host, 
while USB cable can still be attached.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_disconnect(void)
{
  #ifdef FEATURE_HS_USB_INTEGRATED_PHY
#ifndef FEATURE_HS_USB_HAL
  const uint32 MAX_SPOOF_DISCONNECT_GUARD_LOOP = 10000;
  uint32 loop_itor;
#endif /* FEATURE_HS_USB_HAL */
  #endif /* FEATURE_HS_USB_INTEGRATED_PHY */
  
  /* Stop the core */
#ifdef FEATURE_HS_USB_HAL
  HAL_hsusb_ClearBits32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK); 
#else /* FEATURE_HS_USB_HAL */
  HWIO_OUTF(USB_OTG_HS_USBCMD, RS, 0);
#endif /* FEATURE_HS_USB_HAL */

  #ifdef FEATURE_HS_USB_INTEGRATED_PHY
  /* SW workaround for intg. phy in which clearing the RS bit does not pull down D+   
     Write function control register: 
     bits [3,4] (OpMode) =   [0,1] (Non-Driving)
     bit  [6]   (SuspendM) = [1]   (Normal Power) */
#ifdef FEATURE_HS_USB_HAL
  if (HAL_hsusb_WriteUlpiPhy(0, 0, 0x4, 0x48) == HAL_HSUSB_Success)
  {
    return TRUE;
  }
#else /* FEATURE_HS_USB_HAL */
  HWIO_OUT(USB_OTG_HS_ULPI_VIEWPORT, 0x60040048);

  for (loop_itor = 0 ; loop_itor < MAX_SPOOF_DISCONNECT_GUARD_LOOP 
      ; ++loop_itor)
  {
      /* Wait until the ULPI running bit is cleared */
      if (!(HWIO_INF(USB_OTG_HS_ULPI_VIEWPORT, ULPIRUN)))
      {
          return TRUE;
      }
  }
#endif /* FEATURE_HS_USB_HAL */

  HSU_MSG_ERROR("hsu_os_bus_spoof_disconnect: couldn't write termselect", 
        0, 0, 0);
   
  return FALSE;

  #else /* !FEATURE_HS_USB_INTEGRATED_PHY */

  return TRUE;

  #endif /* FEATURE_HS_USB_INTEGRATED_PHY */
}  /* hsu_phy_utils_spoof_disconnect */
#endif /* T_REXNT */


#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_CONNECT

DESCRIPTION
This function causes the device to connect to the host, 
after calling the hsu_phy_utils_spoof_disconnect() function.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_connect(void)
{
  /* Run the core */
  HAL_hsusb_SetBits32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK); 

  return TRUE;
}  /* hsu_phy_utils_spoof_connect */
#endif /* T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_PHY_UTILS_AHB_CLOCK_IS_ON

DESCRIPTION
This function returns whether the AHB clock of the main HS-USB core is
on.

DEPENDENCIES
None.

RETURN VALUE
True if the AHB clock is on. Otherwise, False is returned.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_phy_utils_ahb_clock_is_on(void)
{
#if defined(T_MSM7625) || defined(T_MSM7600)
  return clk_regime_msm_is_on(CLKRGM_USB_HS_PCLK_STATUS);
#elif defined(T_MSM6246) || defined(T_MSM6290)
  return clk_regime_is_on(CLK_RGM_USB_OTG_1_M);
#elif defined(T_QSC6270)
  return clk_regime_is_on(CLK_RGM_USB_M);
#else
  return FALSE;
#endif
}  /* hsu_phy_utils_ahb_clock_is_on */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */


#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_PHY_UTILS_ENABLE_LPM_EXIT

DESCRIPTION
This function lets the HS-USB PHY start exiting from LPM when USB resume or 
reset events occur during TCXO shutdown, even before the HS-USB interrupt
is invoked.

This function comes to resolve issues with violating the USB specification
because sometimes it take a while until the phone wakes up from the shutdown 
state.

This function first checks whether the HS-USB AHB bus clock is enabled. If it
is not enabled, it means that the HS-USB port is not connected, and therefore
USB resume or reset events are not expected. Otherwise, the STP_CTRL bit in the
USBCMD register is cleared, letting any pending PHY LPM exit start executing.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_enable_lpm_exit(void)
{
#ifndef FEATURE_HS_USB_BASIC

  if (hsu_phy_utils_ahb_clock_is_on())
  {
    /* Enable HS-USB PHY LPM exit. */
    HAL_hsusb_ClearBits32((uint8)0,
                          HAL_HSUSB_USBCMD_ADDR,
                          HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

  }

#endif  /* !FEATURE_HS_USB_BASIC */
}  /* hsu_phy_utils_enable_lpm_exit */
#endif  /* !T_REXNT */


#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_PHY_UTILS_DISABLE_LPM_EXIT

DESCRIPTION
This function disables undesirable PHY LPM exits during TCXO shutdown state.

This function comes to revert the actions taken by the 
hsu_phy_utils_enable_lpm_exit() when there was TCXO shutdown mode exit which 
was not triggered by a USB event.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_disable_lpm_exit(void)
{
#ifndef FEATURE_HS_USB_BASIC

  if (hsu_phy_utils_ahb_clock_is_on())
  {
    /* Disable HS-USB PHY LPM exit. */
    HAL_hsusb_SetBits32((uint8)0,
                        HAL_HSUSB_USBCMD_ADDR,
                        HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 
  }

#endif  /* !FEATURE_HS_USB_BASIC */
}  /* hsu_phy_utils_disable_lpm_exit */
#endif  /* !T_REXNT */
