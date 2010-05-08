#ifndef HAL_HSUSB_impl_1r8
#define HAL_HSUSB_impl_1r8
/*
===========================================================================

FILE:         HAL_hsusb_impl_1r8.h

DESCRIPTION:  
This is the external interface for the HS-USB core usb_otg_hs_x1r8 implementation.

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
#define HAL_HSUSB_AHBMODE_ADDR                        (0x00000090)
#define HAL_HSUSB_AHBMODE_RMSK                               0x3
#define HAL_HSUSB_AHBMODE_SHFT                                 0
#define HAL_HSUSB_AHBMODE_AHB_POSTED_WRITE_EN_BMSK           0x2
#define HAL_HSUSB_AHBMODE_AHB_POSTED_WRITE_EN_SHFT           0x1
#define HAL_HSUSB_AHBMODE_TRANSACTOR_BYPASS_BMSK             0x1
#define HAL_HSUSB_AHBMODE_TRANSACTOR_BYPASS_SHFT               0

#define HAL_HSUSB_XTOR_STS_ADDR                       (0x00000094)
#define HAL_HSUSB_XTOR_STS_RMSK                       0xffffffff
#define HAL_HSUSB_XTOR_STS_SHFT                                0

#define HAL_HSUSB_SIM_TIMERS_ADDR                     (0x00000098)
#define HAL_HSUSB_SIM_TIMERS_RMSK                            0x1
#define HAL_HSUSB_SIM_TIMERS_SHFT                              0
#define HAL_HSUSB_SIM_TIMERS_HOST_SIM_TIMERS_ENABLE_BMSK     0x1
#define HAL_HSUSB_SIM_TIMERS_HOST_SIM_TIMERS_ENABLE_SHFT       0

/* -----------------------------------------------------------------------
**                           INTERFACES DESCRIPTION
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

void HAL_hsusb_ConstructImpl1r8(struct HAL_HSUSB_CoreIfType* this);

#ifdef __cplusplus
}
#endif

#endif /* HAL_HSUSB_impl_1r8 */
