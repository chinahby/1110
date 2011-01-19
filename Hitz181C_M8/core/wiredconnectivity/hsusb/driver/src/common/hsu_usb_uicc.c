/*==============================================================================

H S U _ U S B _U I C C . C


GENERAL DESCRIPTION
This file define functions that used by the USB UICC functionality.


EXTERNALIZED FUNCTIONS
hsu_uicc_usb_conn_detected
hsu_uicc_usb_conn_timer_def
hsu_uicc_usb_start_conn_timer
hsu_uicc_usb_shutdown

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/15/08  msh     Initial version

==============================================================================*/
/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include <rex.h>
#include "err.h"
#include "hsu_common.h"
#ifndef FEATURE_HSU_TEST
#include "hsu_apis.h"
#endif
#include "hsu_usb_uicc.h"
#include "hsu_config_selector.h"
#include "pm.h"
#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
#include "bio.h"
#endif /* FEATURE_MULTIPROCESSOR */
#include "uim.h"
#include "hsu_mdm_apis.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define HSU_UICC_USB_CONNECT_TIMEOUT	500

static boolean	hsu_uicc_usb_conn_timer_expired = FALSE;
static hsu_uicc_voltage_class_enum hsu_uicc_voltage_class;
/*==============================================================================
Externalized Variables
==============================================================================*/
/*==============================================================================
File Static Variables
==============================================================================*/
rex_timer_type	hsu_uicc_usb_conn_timer;


/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
#ifdef FEATURE_HS_USB_UICC

/*==============================================================================

FUNCTION          HSU_UICC_USB_CONN_TIMER_CB

DESCRIPTION
A timer callback that called if no USB UICC connection was detected
for 20ms after initialization.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void hsu_uicc_usb_conn_timer_cb(unsigned long callback_param)
{
  hsu_uicc_voltage_class_enum* uicc_voltage_class = 
    (hsu_uicc_voltage_class_enum*)callback_param;

  if (*uicc_voltage_class == HSU_UICC_VOLTAGE_CLASS_C) /* 1.8V */
  {
    HSU_MSG_HIGH(
      "hsu_uicc_usb_conn_timer_cb(): No UICC USB connection was detected for 20ms\
      for voltage class C",0,0,0);
    /* Start the UICC with 3V */ 
    hsu_uicc_set_voltage_class(HSU_UICC_VOLTAGE_CLASS_B);
    hsu_api_start_uicc();
    hsu_uicc_usb_start_conn_timer();
  }
  else if (*uicc_voltage_class == HSU_UICC_VOLTAGE_CLASS_B) /* 3V */ 
  {
    hsu_uicc_usb_conn_timer_expired = TRUE;

    /* Notify UIM that the enumeration has failed (no USB connect) */
    (void)uim_usb_enumeration_failed();

    HSU_MSG_HIGH(
      "hsu_uicc_usb_conn_timer_cb(): No UICC USB connection was detected for 20ms\
      for voltage class B",0,0,0);
  }
  else
  {
    /* That can only happen when the parameter to this function is currupted */ 
    hsu_uicc_usb_conn_timer_expired = TRUE;

    /* Notify UIM that the enumeration has failed (no USB connect) */
    (void)uim_usb_enumeration_failed();

    HSU_MSG_ERROR("hsu_uicc_usb_conn_timer_cb(): Corrupted parameter",0,0,0);
  }
} /* hsu_uicc_usb_conn_timer_cb */

/*==============================================================================

FUNCTION          HSU_UICC_USB_CONN_DETECTED

DESCRIPTION
This function is called by os_notify when a UICC USB connection was detected

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_usb_conn_detected(void)
{
  if (!hsu_uicc_usb_conn_timer_expired && hsu_uicc_is_enabled_by_nv())
  {
    hsu_api_disconnect_pulldowns_uicc();
    /* First stop the timer */
    (void)rex_clr_timer(&hsu_uicc_usb_conn_timer);
    /* TODO: Need to call the UIM function to notify the UIM that a USB
    * connection was detected (Optional)
    */	
  }
  HSU_MSG_HIGH("hsu_uicc_usb_conn_detected():UICC USB connection was detected"
    ,0,0,0);
} /* hsu_uicc_usb_conn_detected */


/*==============================================================================

FUNCTION          HSU_UICC_USB_CONN_TIMER_DEF

DESCRIPTION
This function define the timer that will be used for connection detection

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_usb_conn_timer_def(void)
{
  /* Define the USB UICC connection timer for power-up */
  rex_def_timer_ex(&hsu_uicc_usb_conn_timer,
    hsu_uicc_usb_conn_timer_cb,
    (unsigned long)(&hsu_uicc_voltage_class));
}

/*==============================================================================

FUNCTION          HSU_UICC_USB_START_CONN_TIMER

DESCRIPTION
This function start the a 20ms timer. This timer is used for USB UICC
connection detection.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_usb_start_conn_timer(void)
{
  (void)rex_set_timer(&hsu_uicc_usb_conn_timer, HSU_UICC_USB_CONNECT_TIMEOUT);
}

/*==============================================================================

FUNCTION          HSU_UICC_USB_SHUTDOWN

DESCRIPTION
This function allows the UIM to shutdown the UICC when enumeration fails or
when UICC connect fails.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_usb_shutdown(void)
{
  /* If from any reason this function is called when the timer is on, stop it */
  (void)rex_clr_timer(&hsu_uicc_usb_conn_timer);

#if (defined(T_MSM6246) || defined(T_MSM6290))

  /* Request from PMIC to stop powering VREG_RUIM */
  pm_vreg_control(PM_OFF_CMD, PM_VREG_RUIM_M);

  /* Disable GPIOs 106 & 107 for USB D+ and D- */
  gpio_tlmm_config(GPIO_OUTP_106);
  gpio_tlmm_config(GPIO_OUTP_107);

#elif defined(T_QSC6270)

  /* Request from PMIC to stop powering VREG_USIM */
  pm_vreg_control(PM_OFF_CMD, PM_VREG_USIM_M);

  /* Disable GPIOs 72 & 73 for USB D+ and D- */
  gpio_tlmm_config(GPIO_OUTP_72);
  gpio_tlmm_config(GPIO_OUTP_73);

  /* Enabling UIM clock and UIM data */
  gpio_tlmm_config(USIM_CLK);
  gpio_tlmm_config(USIM_DATA); 

#endif /* defined(T_MSM6246 || T_MSM6290 || T_QSC6270) */
}

/*==============================================================================

FUNCTION          HSU_UICC_USB_SET_VOLTAGE_CLASS

DESCRIPTION
This function sets the current voltage class of the UICC. 
Voltage class C is 1.8V, and voltage class B is 3.0V.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_set_voltage_class(hsu_uicc_voltage_class_enum uicc_voltage_class)
{
  hsu_uicc_voltage_class = uicc_voltage_class;
}

/*==============================================================================

FUNCTION          HSU_UICC_USB_GET_VOLTAGE_CLASS

DESCRIPTION
This function returns the current voltage class of the UICC. 
Voltage class C is 1.8V, and voltage class B is 3.0V.

DEPENDENCIES
None.

RETURN VALUE
Current voltage class of the UICC.

SIDE EFFECTS
None.

==============================================================================*/
hsu_uicc_voltage_class_enum hsu_uicc_get_voltage_class(void)
{
  return hsu_uicc_voltage_class;
}

/*==============================================================================

FUNCTION          HSU_UICC_IS_ENABLED_BY_NV

DESCRIPTION
This function return TRUE if UICC is on in NV.

DEPENDENCIES
None.

RETURN VALUE
True iff UICC is enabled in NV.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_uicc_is_enabled_by_nv(void)
{
  nv_uim_first_inst_class_type nv_uim_first_inst_class = 
    hsu_conf_sel_uim_first_inst_class();

    return ((nv_uim_first_inst_class != NV_UIM_FIRST_INST_CLASS_GSM_SIM) &&
      (nv_uim_first_inst_class != NV_UIM_FIRST_INST_CLASS_UMTS_SIM));
}
#endif /* FEATURE_HS_USB_UICC */
