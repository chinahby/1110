#ifndef HAL_HSUSB_impl_3r0
#define HAL_HSUSB_impl_3r0
/*
===========================================================================

FILE:         HAL_hsusb_impl_3r0.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_x3r0 implementation.

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

#include "HAL_hsusb_impl_2r10.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* Qualcomm registers */
#define HAL_HSUSB_AHB_MODE_INCR_OVERRIDE_BMSK               0x10
#define HAL_HSUSB_AHB_MODE_INCR_OVERRIDE_SHFT                0x4
#define HAL_HSUSB_AHB_MODE_ASYNC_BRIDGES_BYPASS_BMSK   0x80000000
#define HAL_HSUSB_AHB_MODE_ASYNC_BRIDGES_BYPASS_SHFT         0x1f

#define HAL_HSUSB_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_BMSK   0x40
#define HAL_HSUSB_GEN_CONFIG_STREAM_RX_BYPASS_ENABLE_SHFT    0x6
#define HAL_HSUSB_GEN_CONFIG_STREAM_TX_AUTO_ENABLE_BMSK     0x80
#define HAL_HSUSB_GEN_CONFIG_STREAM_TX_AUTO_ENABLE_SHFT      0x7
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_BMSK  0xffff0000
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_HW_QVERSION_SHFT     0x10

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImpl3r0(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_3r0 */
