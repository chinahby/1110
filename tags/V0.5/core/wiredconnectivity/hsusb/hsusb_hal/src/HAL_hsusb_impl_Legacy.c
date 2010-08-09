/*
===========================================================================

FILE:         HAL_hsusb_impl_Legacy.c

DESCRIPTION:  
This is the HAL implementation for the FS-USB Legacy core.

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

#include "HAL_hsusb_impl_Legacy.h"

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_ResetLegacy
**
**  Description:
**    This function is intended to return the FS-USB Legacy core
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
void HAL_hsusb_ResetLegacy(uint8 instance)
{
  HAL_hsusb_Write32(instance, 0x1040, 0x80);
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplLegacy
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  FS-USB Legacy core implementation.
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
void HAL_hsusb_ConstructImplLegacy(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImplDefault(this);
  this->Version = 0;
  this->Reset = HAL_hsusb_ResetLegacy;
}
