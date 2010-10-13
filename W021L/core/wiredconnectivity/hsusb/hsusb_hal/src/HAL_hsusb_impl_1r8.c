/*
===========================================================================

FILE:         HAL_hsusb_impl_1r8.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_x1r8.

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

#include "HAL_hsusb_impl_1r8.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetXtorBypassImpl1r8
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed 
**    for the HS-USB core usb_otg_hs_x1r8 implementation.
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
static boolean HAL_hsusb_GetXtorBypassImpl1r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_TRANSACTOR_BYPASS_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetXtorBypassImpl1r8
**
**  Description:
**    This function sets the AHB Transactor bypass mode for the HS-USB 
**    core usb_otg_hs_x1r8 implementation.
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
static void HAL_hsusb_SetXtorBypassImpl1r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_TRANSACTOR_BYPASS_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_TRANSACTOR_BYPASS_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetPostedWritesImpl1r8
**
**  Description:
**    This function reports the USB Core AHB posted data writes mode 
**    for the HS-USB core usb_otg_hs_x1r8 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
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
static boolean HAL_hsusb_GetPostedWritesImpl1r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_AHB_POSTED_WRITE_EN_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetPostedWritesImpl1r8
**
**  Description:
**    This function sets the USB Core AHB posted data writes mode 
**    for the HS-USB core usb_otg_hs_x1r8 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
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
static void HAL_hsusb_SetPostedWritesImpl1r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_AHB_POSTED_WRITE_EN_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_AHBMODE_ADDR, HAL_HSUSB_AHBMODE_AHB_POSTED_WRITE_EN_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdModeImpl1r8
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode 
**    for the HS-USB core usb_otg_hs_x1r8 implementation.
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
static boolean HAL_hsusb_GetHostSimTimersStdModeImpl1r8(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_SIM_TIMERS_ADDR, HAL_HSUSB_SIM_TIMERS_HOST_SIM_TIMERS_ENABLE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdModeImpl1r8
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode 
**    for the HS-USB core usb_otg_hs_x1r8 implementation.
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
static void HAL_hsusb_SetHostSimTimersStdModeImpl1r8(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_SIM_TIMERS_ADDR, HAL_HSUSB_SIM_TIMERS_HOST_SIM_TIMERS_ENABLE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_SIM_TIMERS_ADDR, HAL_HSUSB_SIM_TIMERS_HOST_SIM_TIMERS_ENABLE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImpl1r8
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_x1r8 implementation.
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
void HAL_hsusb_ConstructImpl1r8(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImplChipIdea(this);
  this->Version = 1;
  this->GetXtorBypass = HAL_hsusb_GetXtorBypassImpl1r8;
  this->SetXtorBypass = HAL_hsusb_SetXtorBypassImpl1r8;
  this->GetPostedWrites = HAL_hsusb_GetPostedWritesImpl1r8;
  this->SetPostedWrites = HAL_hsusb_SetPostedWritesImpl1r8;
  this->GetHostSimTimersStdMode = HAL_hsusb_GetHostSimTimersStdModeImpl1r8;
  this->SetHostSimTimersStdMode = HAL_hsusb_SetHostSimTimersStdModeImpl1r8;
}
