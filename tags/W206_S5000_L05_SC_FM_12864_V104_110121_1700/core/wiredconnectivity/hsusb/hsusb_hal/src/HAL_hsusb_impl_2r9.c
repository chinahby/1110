/*
===========================================================================

FILE:         HAL_hsusb_impl_2r9.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core usb_otg_hs_x2r9.

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

#include "HAL_hsusb_impl_2r9.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_ConstructImpl2r9
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB core usb_otg_hs_x2r9 implementation.
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
void HAL_hsusb_ConstructImpl2r9(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImpl2r8(this);
  this->Version = 3;
}
