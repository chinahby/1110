#ifndef HSU_USB_UICC_H
#define HSU_USB_UICC_H

/*==============================================================================

H S U _ U S B _ U I C C . H


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


/*==============================================================================

INCLUDE FILES

==============================================================================*/
/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
typedef enum 
{
  /* UICC Voltage classes */
  HSU_UICC_VOLTAGE_CLASS_C = 1800,
  HSU_UICC_VOLTAGE_CLASS_B = 3000
} hsu_uicc_voltage_class_enum;

/*==============================================================================
Constants and Macros
==============================================================================*/


/*==============================================================================

FUNCTION DECLARATIONS

==============================================================================*/

#ifdef FEATURE_HS_USB_UICC

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
void hsu_uicc_usb_conn_detected(void);

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
void hsu_uicc_usb_conn_timer_def(void);

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
void hsu_uicc_usb_start_conn_timer(void);

/*==============================================================================

FUNCTION          HSU_UICC_USB_SHUTDOWN

DESCRIPTION
This function allows the UIM to shutdown the UICC when enumeration fails or
when UICC connect failes.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_uicc_usb_shutdown(void);

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
void hsu_uicc_set_voltage_class(hsu_uicc_voltage_class_enum uicc_voltage_class);

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
hsu_uicc_voltage_class_enum hsu_uicc_get_voltage_class(void);

/*==============================================================================

FUNCTION          HSU_UICC_IS_ENABLED_BY_NV

DESCRIPTION
This function returns TRUE if UICC is on in NV.

DEPENDENCIES
None.

RETURN VALUE
True if UICC is enabled in NV.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_uicc_is_enabled_by_nv(void);

#endif /* FEATURE_HS_USB_UICC */
#endif /* HSU_USB_UICC_H */
