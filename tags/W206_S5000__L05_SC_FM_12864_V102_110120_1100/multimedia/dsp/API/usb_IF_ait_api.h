#ifndef AIT_USB_API_H
#define AIT_USB_API_H

#include "A8_common.h"
#include "AIT700_EBIbus.h"
#include "A8_usb.h"
#include "sys_IF_ait_api.h"

A8_ERROR_MSG usb_IF_ait_open_usb(A8_USB_MODE mode);
A8_ERROR_MSG usb_IF_ait_set_usb_mode(A8_USB_MODE mode);
A8_ERROR_MSG usb_IF_ait_close_usb(void);
#endif
