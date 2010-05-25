#ifndef HAL_HSUSB_impl_2r10
#define HAL_HSUSB_impl_2r10
/*
===========================================================================

FILE:         HAL_hsusb_impl_2r10.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_x2r10 implementation.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_2r9.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* Qualcomm registers */
#define HAL_HSUSB_GEN_CONFIG_TX_BUF_COLLISSION_MODE_BMSK    0x20
#define HAL_HSUSB_GEN_CONFIG_TX_BUF_COLLISSION_MODE_SHFT     0x5

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImpl2r10(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_2r10 */
