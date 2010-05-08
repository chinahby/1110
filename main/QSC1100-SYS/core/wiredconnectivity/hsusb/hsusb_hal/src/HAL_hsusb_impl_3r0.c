/*
===========================================================================

FILE:         HAL_hsusb_impl_3r0.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_x3r0.

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

#include "HAL_hsusb_impl_3r0.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetIncrOverrideImpl3r0
**
**  Description:
**    This function reports the USB Core INCR bursts transformation 
**    for the HS-USB core usb_otg_hs_x3r0 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static boolean HAL_hsusb_GetIncrOverrideImpl3r0(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_INCR_OVERRIDE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetIncrOverrideImpl3r0
**
**  Description:
**    This function sets the USB Core INCR bursts transformation 
**    for the HS-USB core usb_otg_hs_x3r0 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static void HAL_hsusb_SetIncrOverrideImpl3r0(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_INCR_OVERRIDE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_INCR_OVERRIDE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetAsyncBridgesBypassImpl3r0
**
**  Description:
**    This function reports the asynchronous bridge bypass mode on 
**    the master AHB interface for the HS-USB core usb_otg_hs_x3r0 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static boolean HAL_hsusb_GetAsyncBridgesBypassImpl3r0(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAsyncBridgesBypassImpl3r0
**
**  Description:
**    This function sets the asynchronous bridge bypass mode on 
**    the master AHB interface for the HS-USB core usb_otg_hs_x3r0 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static void HAL_hsusb_SetAsyncBridgesBypassImpl3r0(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHB_MODE_ADDR, HAL_HSUSB_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamRxBypassModeImpl3r0
**
**  Description:
**    This function reports the RX streaming bypass mode for the HS-USB 
**    core usb_otg_hs_x3r0 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static boolean HAL_hsusb_GetStreamRxBypassModeImpl3r0(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamRxBypassModeImpl3r0
**
**  Description:
**    This function sets the RX streaming bypass mode for the HS-USB 
**    core usb_otg_hs_x3r0 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static void HAL_hsusb_SetStreamRxBypassModeImpl3r0(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamTxAutoModeImpl3r0
**
**  Description:
**    This function reports the TX streaming automatic determination mode 
**    for the HS-USB core usb_otg_hs_x3r0 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static boolean HAL_hsusb_GetStreamTxAutoModeImpl3r0(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_TX_AUTO_ENABLE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamTxAutoModeImpl3r0
**
**  Description:
**    This function sets the TX streaming automatic determination mode 
**    for the HS-USB core usb_otg_hs_x3r0 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
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
static void HAL_hsusb_SetStreamTxAutoModeImpl3r0(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_TX_AUTO_ENABLE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_STREAM_TX_AUTO_ENABLE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImpl3r0
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_x3r0 implementation.
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
void HAL_hsusb_ConstructImpl3r0(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImpl2r10(this);
  this->Version = 5;
  this->GetIncrOverride = HAL_hsusb_GetIncrOverrideImpl3r0;
  this->SetIncrOverride = HAL_hsusb_SetIncrOverrideImpl3r0;
  this->GetAsyncBridgesBypass = HAL_hsusb_GetAsyncBridgesBypassImpl3r0;
  this->SetAsyncBridgesBypass = HAL_hsusb_SetAsyncBridgesBypassImpl3r0;
  this->GetStreamRxBypassMode = HAL_hsusb_GetStreamRxBypassModeImpl3r0;
  this->SetStreamRxBypassMode = HAL_hsusb_SetStreamRxBypassModeImpl3r0;
  this->GetStreamTxAutoMode = HAL_hsusb_GetStreamTxAutoModeImpl3r0;
  this->SetStreamTxAutoMode = HAL_hsusb_SetStreamTxAutoModeImpl3r0;
}
