/*
===========================================================================

FILE:         HAL_hsusb_impl_Default.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB default core.

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

#include "HAL_hsusb_impl_Default.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_ResetDefault
**
**  Description:
**    This function is intended to return the HS-USB core
**    to its  power-on status.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ResetDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_ReadUlpiPhyDefault
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB default core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Returns the read data.
**
**  Return:
**    HAL_HSUSB_NotSupported - The function is not supported by the HS-USB core.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhyDefault(uint8 instance, uint8 port, uint8 reg, uint8* data)
{
  (void)port;
  (void)reg;
  (void)data;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_WriteUlpiPhyDefault
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB default core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Determines the write data.
**
**  Return:
**    HAL_HSUSB_NotSupported - The function is not supported by the HS-USB core.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhyDefault(uint8 instance, uint8 port, uint8 reg, uint8 data)
{
  (void)port;
  (void)reg;
  (void)data;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetAhbBurstModeImplDefault
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_AHB_BURST_INCR;
}

/* ===========================================================================
**
**  HAL_hsusb_SetAhbBurstModeImplDefault
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetAhbBurstModeImplDefault(uint8 instance, HAL_HSUSB_AhbBurstMode eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetSe0GlitchFixCtrlImplDefault
**
**  Description:
**    This function reports whether the SE0 glitch fix mechanism is activated 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetSe0GlitchFixCtrlImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetSe0GlitchFixCtrlImplDefault
**
**  Description:
**    This function activates/disactivate the SE0 glitch fix mechanism 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetSe0GlitchFixCtrlImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetFs3Wire2WireSelectImplDefault
**
**  Description:
**    This function reports the two wire interface on the fs_dat and 
**    fs_se0 pins mode for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetFs3Wire2WireSelectImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetFs3Wire2WireSelectImplDefault
**
**  Description:
**    This function enables/disables the two wire interface on the fs_dat and 
**    fs_se0 pins for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetFs3Wire2WireSelectImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetXtorBypassImplDefault
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetXtorBypassImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetXtorBypassImplDefault
**
**  Description:
**    This function sets the AHB Transactor bypass mode for the default 
**    HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetXtorBypassImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetPostedWritesImplDefault
**
**  Description:
**    This function reports the USB Core AHB posted data writes mode 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The AHB master interface enables posted data writes
**    FALSE - The AHB master interface disables posted data writes
**
** ======================================================================== */
static boolean HAL_hsusb_GetPostedWritesImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetPostedWritesImplDefault
**
**  Description:
**    This function sets the USB Core AHB posted data writes mode 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core AHB posted data writes mode.
**             Values are:
**             TRUE  - The AHB master interface enables posted data writes
**             FALSE - The AHB master interface disables posted data writes
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetPostedWritesImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetGrantStolenImplDefault
**
**  Description:
**    This function reports whether the arbiter removed the hgrant signal 
**    prior to completing a transaction for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetGrantStolenImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetGrantStolenImplDefault
**
**  Description:
**    This function sets the arbiter removing the hgrant signal 
**    prior to completing a transaction for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetGrantStolenImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetHprotModeImplDefault
**
**  Description:
**    This function reports the HPROT signal mode for the default HS-USB 
**    cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static HAL_HSUSB_HprotMode HAL_hsusb_GetHprotModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_HPROT_NONPOSTED;
}

/* ===========================================================================
**
**  HAL_hsusb_SetHprotModeImplDefault
**
**  Description:
**    This function sets the HPROT signal mode for the default HS-USB 
**    cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetHprotModeImplDefault(uint8 instance, HAL_HSUSB_HprotMode eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetIncrOverrideImplDefault
**
**  Description:
**    This function reports the USB Core INCR bursts transformation 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The all INCR bursts from the USB Core will be 
**            internally transformed into SINGLE transfers
**    FALSE - If the USB Core issues an INCR burst, it will 
**            propagate to the external master AHB port
**
** ======================================================================== */
static boolean HAL_hsusb_GetIncrOverrideImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetIncrOverrideImplDefault
**
**  Description:
**    This function sets the USB Core INCR bursts transformation 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core INCR bursts transformation.
**             Values are:
**             TRUE  - The all INCR bursts from the USB Core will be 
**                   internally transformed into SINGLE transfers
**             FALSE - If the USB Core issues an INCR burst, it will 
**                   propagate to the external master AHB port
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetIncrOverrideImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetAsyncBridgesBypassImplDefault
**
**  Description:
**    This function reports the asynchronous bridge bypass mode on 
**    the master AHB interface for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The asynchronous bridge on the master AHB interface is bypassed.
**    FALSE   - The asynchronous bridge on the master AHB interface issued.
**
** ======================================================================== */
static boolean HAL_hsusb_GetAsyncBridgesBypassImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetAsyncBridgesBypassImplDefault
**
**  Description:
**    This function sets the asynchronous bridge bypass mode on 
**    the master AHB interface for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the asynchronous bridge bypass mode on 
**             the master AHB interface.
**             Values are:
**             TRUE  - The asynchronous bridge on the master AHB interface 
**                   is bypassed
**             FALSE - The asynchronous bridge on the master AHB interface 
**                   issued
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetAsyncBridgesBypassImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelectorImplDefault
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelectorImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return HAL_HSUSB_TEST_BUS_KSM;
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelectorImplDefault
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetTestBusSelectorImplDefault(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  (void)eMode;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetTxBufCollisionModeImplDefault
**
**  Description:
**    This function reports the TX buffer controller mode for the default 
**    HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX buffer controller direct mode.
**    FALSE   - The TX buffer controller revert mode.
**
** ======================================================================== */
static boolean HAL_hsusb_GetTxBufCollisionModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetTxBufCollisionModeImplDefault
**
**  Description:
**    This function sets the TX buffer controller mode for the default 
**    HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX buffer controller mode.
**             Values are:
**             TRUE  - The TX buffer controller direct mode
**             FALSE - The TX buffer controller revert mode
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetTxBufCollisionModeImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamRxBypassModeImplDefault
**
**  Description:
**    This function reports the RX streaming bypass mode for the default 
**    HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The RX streaming bypass is enabled.
**    FALSE   - The RX streaming bypass is disabled.
**
** ======================================================================== */
static boolean HAL_hsusb_GetStreamRxBypassModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamRxBypassModeImplDefault
**
**  Description:
**    This function sets the RX streaming bypass mode for the default 
**    HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the RX streaming bypass mode.
**             Values are:
**             TRUE  - Enable the RX streaming bypassing
**             FALSE - Disable the RX streaming bypassing
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetStreamRxBypassModeImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamTxAutoModeImplDefault
**
**  Description:
**    This function reports the TX streaming automatic determination mode 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX streaming automatic determination is enabled.
**    FALSE   - The TX streaming automatic determination s is disabled
**
** ======================================================================== */
static boolean HAL_hsusb_GetStreamTxAutoModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamTxAutoModeImplDefault
**
**  Description:
**    This function sets the TX streaming automatic determination mode 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable TX streaming automatic determination
**             FALSE - Disable TX streaming automatic determination
**
**  Return:
**    None.
**
** ======================================================================== */
static void HAL_hsusb_SetStreamTxAutoModeImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdModeImplDefault
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetHostSimTimersStdModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdModeImplDefault
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetHostSimTimersStdModeImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersSuspModeImplDefault
**
**  Description:
**    This function reports the timers used for the USB suspend process mode 
**    for the default HS-USB cores' implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static boolean HAL_hsusb_GetHostSimTimersSuspModeImplDefault(uint8 instance)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
  return FALSE;
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersSuspModeImplDefault
**
**  Description:
**    This function sets the timers used for the USB suspend process mode 
**    for the default HS-USB cores' implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB default HW.
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
static void HAL_hsusb_SetHostSimTimersSuspModeImplDefault(uint8 instance, boolean state)
{
  (void)state;

  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplDefault
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core default implementation.
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
void HAL_hsusb_ConstructImplDefault(struct HAL_HSUSB_CoreIfType* this)
{
  this->Reset = HAL_hsusb_ResetDefault;
  this->ReadUlpiPhy = HAL_hsusb_ReadUlpiPhyDefault;
  this->WriteUlpiPhy = HAL_hsusb_WriteUlpiPhyDefault;
  this->GetAhbBurstMode = HAL_hsusb_GetAhbBurstModeImplDefault;
  this->SetAhbBurstMode = HAL_hsusb_SetAhbBurstModeImplDefault;
  this->GetSe0GlitchFixCtrl = HAL_hsusb_GetSe0GlitchFixCtrlImplDefault;
  this->SetSe0GlitchFixCtrl = HAL_hsusb_SetSe0GlitchFixCtrlImplDefault;
  this->GetFs3Wire2WireSelect = HAL_hsusb_GetFs3Wire2WireSelectImplDefault;
  this->SetFs3Wire2WireSelect = HAL_hsusb_SetFs3Wire2WireSelectImplDefault;
  this->GetXtorBypass = HAL_hsusb_GetXtorBypassImplDefault;
  this->SetXtorBypass = HAL_hsusb_SetXtorBypassImplDefault;
  this->GetPostedWrites = HAL_hsusb_GetPostedWritesImplDefault;
  this->SetPostedWrites = HAL_hsusb_SetPostedWritesImplDefault;
  this->GetGrantStolen = HAL_hsusb_GetGrantStolenImplDefault;
  this->SetGrantStolen = HAL_hsusb_SetGrantStolenImplDefault;
  this->GetHprotMode = HAL_hsusb_GetHprotModeImplDefault;
  this->SetHprotMode = HAL_hsusb_SetHprotModeImplDefault;
  this->GetIncrOverride = HAL_hsusb_GetIncrOverrideImplDefault;
  this->SetIncrOverride = HAL_hsusb_SetIncrOverrideImplDefault;
  this->GetAsyncBridgesBypass = HAL_hsusb_GetAsyncBridgesBypassImplDefault;
  this->SetAsyncBridgesBypass = HAL_hsusb_SetAsyncBridgesBypassImplDefault;
  this->GetTestBusSelector = HAL_hsusb_GetTestBusSelectorImplDefault;
  this->SetTestBusSelector = HAL_hsusb_SetTestBusSelectorImplDefault;
  this->GetTxBufCollisionMode = HAL_hsusb_GetTxBufCollisionModeImplDefault;
  this->SetTxBufCollisionMode = HAL_hsusb_SetTxBufCollisionModeImplDefault;
  this->GetStreamRxBypassMode = HAL_hsusb_GetStreamRxBypassModeImplDefault;
  this->SetStreamRxBypassMode = HAL_hsusb_SetStreamRxBypassModeImplDefault;
  this->GetStreamTxAutoMode = HAL_hsusb_GetStreamTxAutoModeImplDefault;
  this->SetStreamTxAutoMode = HAL_hsusb_SetStreamTxAutoModeImplDefault;
  this->GetHostSimTimersStdMode = HAL_hsusb_GetHostSimTimersStdModeImplDefault;
  this->SetHostSimTimersStdMode = HAL_hsusb_SetHostSimTimersStdModeImplDefault;
  this->GetHostSimTimersSuspMode = HAL_hsusb_GetHostSimTimersSuspModeImplDefault;
  this->SetHostSimTimersSuspMode = HAL_hsusb_SetHostSimTimersSuspModeImplDefault;
}
