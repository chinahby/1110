/*==============================================================================

High Speed USB BT HID Wrapper 

GENERAL DESCRIPTION
This file provides BT HID wrapper functions to remove the linker errors when
FEATURE_BT_EXTPF_HID_HOST is not defined with HSU.lib provided as binary file.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
07/29/08  tk      creat 

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "jconfig.h"
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/*===========================================================================
FUNCTION  HSU_BT_PF_USB_HID_HOST_SEND_OUTPUT_REPORT
DESCRIPTION wrap bt_pf_usb_hid_host_send_output_report
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
uint8 hsu_bt_pf_usb_hid_host_send_output_report(
  uint16 hid_handle,
  uint8  report_type,
  uint8  report_id,
  uint8* data,
  uint16 size )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else /* FEATURE_BT_EXTPF_HID_HOST */
  return 0;
#endif /* ! FEATURE_BT_EXTPF_HID_HOST */
}

/*===========================================================================
FUNCTION  HSU_BT_PF_USB_HID_HOST_SET_IDLE
DESCRIPTION wrap bt_pf_usb_hid_host_set_idle
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
uint8 hsu_bt_pf_usb_hid_host_set_idle(uint16 hid_handle, uint8 idle_rate )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else /* FEATURE_BT_EXTPF_HID_HOST */
  return 0;
#endif /* ! FEATURE_BT_EXTPF_HID_HOST */
}

/*===========================================================================
FUNCTION  HSU_BT_PF_USB_HID_HOST_GET_IDLE
DESCRIPTION wrap bt_pf_usb_hid_host_get_idle
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
uint8 hsu_bt_pf_usb_hid_host_get_idle(uint16 hid_handle, uint8* idle_value )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else /* FEATURE_BT_EXTPF_HID_HOST */
  return 0;
#endif /* ! FEATURE_BT_EXTPF_HID_HOST */
}

/*===========================================================================
FUNCTION  HSU_BT_PF_USB_HID_HOST_SET_PROTOCOL
DESCRIPTION wrap bt_pf_usb_hid_host_set_protocol
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
uint8 hsu_bt_pf_usb_hid_host_set_protocol(uint16 hid_handle, uint8 protocol )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else /* FEATURE_BT_EXTPF_HID_HOST */
  return 0;
#endif /* ! FEATURE_BT_EXTPF_HID_HOST */
}

/*===========================================================================
FUNCTION  HSU_BT_PF_USB_HID_HOST_GET_PROTOCOL
DESCRIPTION wrap bt_pf_usb_hid_host_get_protocol
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
uint8 hsu_bt_pf_usb_hid_host_get_protocol(uint16 hid_handle, uint8* protocol )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#else /* FEATURE_BT_EXTPF_HID_HOST */
  return 0;
#endif /* ! FEATURE_BT_EXTPF_HID_HOST */
}

/*===========================================================================
FUNCTION  HSU_BT_PF_HID_HOST_REGISTER_USB_CBS
DESCRIPTION wrap bt_pf_hid_host_register_usb_cbs
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
===========================================================================*/
void hsu_bt_pf_hid_host_register_usb_cbs( void * hid_callbacks )
{
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
}

