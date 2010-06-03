#ifndef HAL_HSUSB_impl_2r8
#define HAL_HSUSB_impl_2r8
/*
===========================================================================

FILE:         HAL_hsusb_impl_2r8.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_x2r8 implementation.

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

#include "HAL_hsusb_impl.h"
#include "HAL_hsusb_impl_ChipIdea.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* Qualcomm registers */
#define HAL_HSUSB_AHB_BURST_ADDR                      (0x00000090)
#define HAL_HSUSB_AHB_BURST_AHB_BURST_BMSK                   0x7
#define HAL_HSUSB_AHB_BURST_AHB_BURST_SHFT                     0

#define HAL_HSUSB_XTOR_STS_ADDR                       (0x00000094)
#define HAL_HSUSB_XTOR_STS_GRANT_STOLEN_BMSK                 0x2
#define HAL_HSUSB_XTOR_STS_GRANT_STOLEN_SHFT                 0x1

#define HAL_HSUSB_AHB_MODE_ADDR                       (0x00000098)
#define HAL_HSUSB_AHB_MODE_XTOR_BYPASS_BMSK                  0x1
#define HAL_HSUSB_AHB_MODE_XTOR_BYPASS_SHFT                    0
#define HAL_HSUSB_AHB_MODE_GRANT_STOLEN_CLEAR_BMSK           0x2
#define HAL_HSUSB_AHB_MODE_GRANT_STOLEN_CLEAR_SHFT           0x1
#define HAL_HSUSB_AHB_MODE_HPROT_MODE_BMSK                   0xc
#define HAL_HSUSB_AHB_MODE_HPROT_MODE_SHFT                   0x2

#define HAL_HSUSB_GEN_CONFIG_ADDR                     (0x0000009c)
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_BMSK     0xf
#define HAL_HSUSB_GEN_CONFIG_USB_OTG_HS_TESTMUX_SEL_SHFT       0
#define HAL_HSUSB_GEN_CONFIG_DEV_PE_NAK_CTRL_BMSK           0x10
#define HAL_HSUSB_GEN_CONFIG_DEV_PE_NAK_CTRL_SHFT            0x4
#define HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_BMSK   0x100
#define HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_STD_SHFT     0x8
#define HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_BMSK  0x200
#define HAL_HSUSB_GEN_CONFIG_HOST_SIM_TIMERS_EN_SUSP_SHFT    0x9

#define HAL_HSUSB_USBCMD_FS_3_WIRE_2_WIRE_SELECT_BMSK  0x8000000
#define HAL_HSUSB_USBCMD_FS_3_WIRE_2_WIRE_SELECT_SHFT       0x1b
#define HAL_HSUSB_USBCMD_SE0_GLITCH_FIX_CTRL_BMSK     0x10000000
#define HAL_HSUSB_USBCMD_SE0_GLITCH_FIX_CTRL_SHFT           0x1c

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImpl2r8(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_2r8 */
