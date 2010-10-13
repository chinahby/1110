/*
===========================================================================

FILE:         HAL_hsusb_impl_2r10.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_x2r10.

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

#include "HAL_hsusb_impl_2r10.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_GetTxBufCollisionModeImpl2r10
**
**  Description:
**    This function reports the TX buffer controller mode for the HS-USB 
**    core usb_otg_hs_x2r10 implementation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
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
static boolean HAL_hsusb_GetTxBufCollisionModeImpl2r10(uint8 instance)
{
  return (HAL_hsusb_GetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_TX_BUF_COLLISSION_MODE_BMSK) != 0);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTxBufCollisionModeImpl2r10
**
**  Description:
**    This function sets the TX buffer controller mode for the HS-USB 
**    core usb_otg_hs_x2r10 implementation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
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
static void HAL_hsusb_SetTxBufCollisionModeImpl2r10(uint8 instance, boolean state)
{
  (void)(state ? 
    HAL_hsusb_SetBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_TX_BUF_COLLISSION_MODE_BMSK) : 
    HAL_hsusb_ClearBits32(instance, HAL_HSUSB_GEN_CONFIG_ADDR, HAL_HSUSB_GEN_CONFIG_TX_BUF_COLLISSION_MODE_BMSK)
    );
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImpl2r10
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_x2r10 implementation.
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
void HAL_hsusb_ConstructImpl2r10(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImpl2r9(this);
  this->Version = 4;
  this->GetTxBufCollisionMode = HAL_hsusb_GetTxBufCollisionModeImpl2r10;
  this->SetTxBufCollisionMode = HAL_hsusb_SetTxBufCollisionModeImpl2r10;
}
