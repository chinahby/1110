/*
===========================================================================

FILE:         HAL_hsusb_impl_2r8.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_x2r8.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_2r8.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetAhbBurstModeImpl2r8
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The mode of the AMBA AHB master interface m_hburst signal.
**
** ======================================================================== */
static HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstModeImpl2r8(uint8 instance)
{
  return (HAL_HSUSB_AhbBurstMode)
    (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHB_BURST_ADDR, HAL_HSUSB_AHB_BURST_AHB_BURST_BMSK)
    >> HAL_HSUSB_AHB_BURST_AHB_BURST_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAhbBurstModeImpl2r8
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines mode of the AMBA AHB master interface 
**             m_hburst signal.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetAhbBurstModeImpl2r8(uint8 instance, HAL_HSUSB_AhbBurstMode eMode)
{
  HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_AHB_BURST_ADDR, 
    HAL_HSUSB_AHB_BURST_AHB_BURST_BMSK, 
    eMode<<HAL_HSUSB_AHB_BURST_AHB_BURST_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_GetFs3Wire2WireSelectImpl2r8
**
**  Description:
**    This function reports the two wire interface on the fs_dat and 
**    fs_se0 pins mode for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The two wire interface on the fs_dat and 
**              fs_se0 pins is enabled.
**    FALSE   - The two wire interface on the fs_dat and 
**              fs_se0 pins is disabled.
**
** ======================================================================== */
static boolean HAL_hsusb_GetFs3Wire2WireSelectImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetFs3Wire2WireSelectImpl2r8
**
**  Description:
**    This function enables/disables the two wire interface on the fs_dat and 
**    fs_se0 pins for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Enable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**             FALSE - Disable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetFs3Wire2WireSelectImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetSe0GlitchFixCtrlImpl2r8
**
**  Description:
**    This function reports whether the SE0 glitch fix mechanism is activated 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SE0 glitch fix mechanism is activated.
**    FALSE   - The SE0 glitch fix mechanism is not activated.
**
** ======================================================================== */
static boolean HAL_hsusb_GetSe0GlitchFixCtrlImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSe0GlitchFixCtrlImpl2r8
**
**  Description:
**    This function activates/disactivate the SE0 glitch fix mechanism 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Activate the SE0 glitch fix mechanism
**             FALSE - Deactivate the SE0 glitch fix mechanism
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetSe0GlitchFixCtrlImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetXtorBypassImpl2r8
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The AHB Transactor is bypassed.
**    FALSE   - The AHB Transactor is not bypassed.
**
** ======================================================================== */
static boolean HAL_hsusb_GetXtorBypassImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_XTOR_BYPASS_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetXtorBypassImpl2r8
**
**  Description:
**    This function sets the AHB Transactor bypass mode for the HS-USB 
**    core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the AHB Transactor bypass mode.
**             Values are:
**             TRUE  - Bypass the AHB Transactor
**             FALSE - Do not bypass the AHB Transactor
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetXtorBypassImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_XTOR_BYPASS_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_XTOR_BYPASS_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetGrantStolenImpl2r8
**
**  Description:
**    This function reports whether the arbiter removed the hgrant signal 
**    prior to completing a transaction for the HS-USB core usb_otg_hs_x2r8 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The arbiter removed the hgrant signal 
**              prior to completing a transaction.
**    FALSE   - The arbiter doesn’t removed the hgrant signal 
**              prior to completing a transaction.
**
** ======================================================================== */
static boolean HAL_hsusb_GetGrantStolenImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_XTOR_STS_ADDR, HAL_HSUSB_XTOR_STS_GRANT_STOLEN_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetGrantStolenImpl2r8
**
**  Description:
**    This function sets the arbiter removing the hgrant signal 
**    prior to completing a transaction for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the arbiter removing the hgrant signal 
**             prior to completing a transaction mode.
**             Values are:
**             TRUE  - Enable the arbiter removed the hgrant signal 
**                   prior to completing a transaction
**             FALSE - Disable the arbiter doesn’t removed the hgrant signal 
**                   prior to completing a transaction
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetGrantStolenImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_XTOR_STS_GRANT_STOLEN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_XTOR_STS_GRANT_STOLEN_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetHprotModeImpl2r8
**
**  Description:
**    This function reports the HPROT signal mode for the HS-USB core 
**    usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HPROT signal mode.
**
** ======================================================================== */
static HAL_HSUSB_HprotMode HAL_hsusb_GetHprotModeImpl2r8(uint8 instance)
{
  return (HAL_HSUSB_HprotMode)
    (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_HPROT_MODE_BMSK)
    >> HAL_HSUSB_AHB_MODE_HPROT_MODE_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHprotModeImpl2r8
**
**  Description:
**    This function sets the HPROT signal mode for the HS-USB core 
**    usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines the HPROT signal mode.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetHprotModeImpl2r8(uint8 instance, HAL_HSUSB_HprotMode eMode)
{
  (void)HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, 
    HAL_HSUSB_AHB_MODE_HPROT_MODE_BMSK, 
    eMode<<HAL_HSUSB_AHB_MODE_HPROT_MODE_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelectorImpl2r8
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The group of the internal signals that is routed to the test bus.
**
** ======================================================================== */
static HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelectorImpl2r8(uint8 instance)
{
  return (HAL_HSUSB_TestBusSelectorType)
    (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK)
    >> HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelectorImpl2r8
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines which group of the internal signals is routed 
**             to the test bus.
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetTestBusSelectorImpl2r8(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  (void)HAL_hsusb_ClearAndSetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, 
    HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK, 
    eMode<<HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdModeImpl2r8
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB reset on the ULPI are enabled.
**    FALSE   - The timers used for the USB reset on the ULPI are disabled.
**
** ======================================================================== */
static boolean HAL_hsusb_GetHostSimTimersStdModeImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdModeImpl2r8
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB reset on the ULPI mode.
**             Values are:
**             TRUE  - Enable the timers used for the USB reset on the ULPI
**             FALSE - Disable the timers used for the USB reset on the ULPI
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetHostSimTimersStdModeImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersSuspModeImpl2r8
**
**  Description:
**    This function reports the timers used for the USB suspend process mode 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB suspend process short 
**              for faster simulation and ATE time.
**    FALSE   - The timers used for the USB suspend process are 
**              according to the USB specification.
**
** ======================================================================== */
static boolean HAL_hsusb_GetHostSimTimersSuspModeImpl2r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersSuspModeImpl2r8
**
**  Description:
**    This function sets the timers used for the USB suspend process mode 
**    for the HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB suspend process mode.
**             Values are:
**             TRUE  - The timers used for the USB suspend process short 
**                     for faster simulation and ATE time
**             FALSE - The timers used for the USB suspend process are 
**                     according to the USB specification
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetHostSimTimersSuspModeImpl2r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImpl2r8
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_x2r8 implementation.
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    this     - Points to the HS-USB core interface object.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ConstructImpl2r8(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImplChipIdea(this);
  this->Version = 2;
  this->GetAhbBurstMode = HAL_hsusb_GetAhbBurstModeImpl2r8;
  this->SetAhbBurstMode = HAL_hsusb_SetAhbBurstModeImpl2r8;
  this->GetFs3Wire2WireSelect = HAL_hsusb_GetFs3Wire2WireSelectImpl2r8;
  this->SetFs3Wire2WireSelect = HAL_hsusb_SetFs3Wire2WireSelectImpl2r8;
  this->GetSe0GlitchFixCtrl = HAL_hsusb_GetSe0GlitchFixCtrlImpl2r8;
  this->SetSe0GlitchFixCtrl = HAL_hsusb_SetSe0GlitchFixCtrlImpl2r8;
  this->GetXtorBypass = HAL_hsusb_GetXtorBypassImpl2r8;
  this->SetXtorBypass = HAL_hsusb_SetXtorBypassImpl2r8;
  this->GetGrantStolen = HAL_hsusb_GetGrantStolenImpl2r8;
  this->SetGrantStolen = HAL_hsusb_SetGrantStolenImpl2r8;
  this->GetHprotMode = HAL_hsusb_GetHprotModeImpl2r8;
  this->SetHprotMode = HAL_hsusb_SetHprotModeImpl2r8;
  this->GetTestBusSelector = HAL_hsusb_GetTestBusSelectorImpl2r8;
  this->SetTestBusSelector = HAL_hsusb_SetTestBusSelectorImpl2r8;
  this->GetHostSimTimersStdMode = HAL_hsusb_GetHostSimTimersStdModeImpl2r8;
  this->SetHostSimTimersStdMode = HAL_hsusb_SetHostSimTimersStdModeImpl2r8;
  this->GetHostSimTimersSuspMode = HAL_hsusb_GetHostSimTimersSuspModeImpl2r8;
  this->SetHostSimTimersSuspMode = HAL_hsusb_SetHostSimTimersSuspModeImpl2r8;
}
