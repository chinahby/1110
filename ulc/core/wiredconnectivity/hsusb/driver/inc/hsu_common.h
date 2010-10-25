#ifndef HSU_COMMON_H
#define HSU_COMMON_H
/*==============================================================================

            hsu_common   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains common definitions for high speed usb.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_common.h#14 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/17/06  sk  Creation

==============================================================================*/

#include "customer.h"
#include "msg.h"
#include "assert.h"
#include "hsu_os_abstract_bus_ext.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/
#define HSU_JSUCCESS 0
  /* for use with the Jungo jresult_t type */

#define HSU_MAX_MSG_LEN 80 
  /* for use with the HSU_MSG_HIGH, HSU_MSG_MED etc... */

/* To overcome lint error 715:
"Symbol 'Symbol' (Location) not referenced" */
#define HSU_USE_PARAM(x) (void)x

/* To overcome lint error 818: 
"Pointer parameter 'Symbol' (Location) could be declared ptr to const" */
#define HSU_PARAM_COULD_BE_CONST(x) (void)x

/* Change a pointer to be 32 bytes alligned */
#define HSU_ALIGN_PTR_TO_32_BYTE(ptr) ((byte *)((((uint32)(ptr)) + 31) & ~31))
/* Get the closest multiple of 32 (round up) */
#define HSU_ROUND_UP_TO_32(size) (((size) + 31) & ~31)

/* TODO ECM - only used before the bring up... */
#ifdef FEATURE_HSU_EBI
#error code not present
#endif /* FEATURE_HSU_EBI */


/* Used for ECM performance test */
#ifdef FEATURE_HS_USB_ECM_PERFORMANCE_TESTS
#error code not present
#endif  /* FEATURE_HS_USB_ECM_PERFORMANCE_TESTS */

#ifdef T_REXNT
#error code not present
#else /* T_REXNT */

#ifdef FEATURE_HS_USB_BASIC

#define HSU_ASSERT(condition) if(condition) {}

#define HSU_MSG_FATAL(x_fmt, a, b, c)
#define HSU_MSG_ERROR(x_fmt, a, b, c)
#define HSU_MSG_HIGH(x_fmt, a, b, c)
#define HSU_MSG_MED(x_fmt, a, b, c)
#define HSU_MSG_LOW(x_fmt, a, b, c)

#else /* FEATURE_HS_USB_BASIC */

/*lint -emacro(717, HSU_ASSERT) 
** Suppress Lint warning about "do ... while(0)"
** introduced by the ASSERT macro.
*/

/*lint -emacro(779, HSU_ASSERT)
** Suppress Lint warning about string constant comparison
** when using HSU_ASSERT in the following form:
**
**    HSU_ASSERT("Error Message ..." == NULL)
*/
#if defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
#define HSU_ASSERT(condition) if(condition) {}
#elif !defined(IMAGE_MODEM_PROC)
#define HSU_ASSERT(condition)               \
do {                                        \
  boolean hsu_assert_condition =            \
          (boolean)(condition);             \
  if (!(hsu_assert_condition))              \
  {                                         \
    hsu_os_bus_save_mem_dump();             \
    ASSERT(hsu_assert_condition);           \
  }                                         \
} while(0)
#else	/*IMAGE_MODEM_PROC*/
#define HSU_ASSERT(condition) ASSERT(condition)
#endif

#define HSU_MSG_FATAL(x_fmt, a, b, c) \
  MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define HSU_MSG_ERROR(x_fmt, a, b, c) \
  MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define HSU_MSG_HIGH(x_fmt, a, b, c)  \
  MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_HIGH,  x_fmt, a, b, c)

#define HSU_MSG_MED(x_fmt, a, b, c)   \
  MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_MED,   x_fmt, a, b, c)

#define HSU_MSG_LOW(x_fmt, a, b, c)   \
  MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_LOW,   x_fmt, a, b, c)

#endif /* FEATURE_HS_USB_BASIC */
#endif /* T_REXNT */

/* When code is commercial, map this to (void)()*/
#define HSU_ASSUME HSU_ASSERT



#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

/* This should have appeared as part of USB 2.0 definitions file: */
#define HSU_MAX_FULL_SPEED_PACKET_SIZE 64

#define HSU_CRC_ERR_PADDING_ARRAY {0xDE, 0xAD, 0xBE, 0xEF}

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

/* This is HS-USB USB charging feature flag. If we will add new USB charging 
   feature flag to target .builds, we should add && to the following definition 
   with the new feature flag */
#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_TASK)
/* When the PMIC PHY is used, the HS_USB_CHARGING feature is disabled because 
   PMIC OTG state machines handles the charging procedures.  HS-USB only needs 
   to notify PMIC of the maximum current supported by the device when the USB 
   enumeration is complete. For LPM, HS-USB needs to call pm_app_otg_suspend 
   and pm_app_otg_resume APIs instead of charger APIs */
#if !defined(FEATURE_HS_USB_PMIC_PHY) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)
#define FEATURE_HS_USB_CHARGING
#endif /* !FEATURE_HS_USB_PMIC_PHY && !FEATURE_HS_USB_NO_HS_OTG_CORE */
#endif /* FEATURE_BATTERY_CHARGER || FEATURE_CHG_TASK */

/* Macros for adding debug capabilities to os_safe_enter/leave */
#define os_safe_enter() os_safe_enter_d(__FILE__, __LINE__)
#define os_safe_leave() os_safe_leave_d()
void os_safe_enter_d(char *file, uint32 line);
void os_safe_leave_d(void);

#endif /* HSU_COMMON_H */

