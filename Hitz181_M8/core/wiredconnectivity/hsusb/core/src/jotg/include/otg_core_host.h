/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef __OTG_CORE_HOST_H__
#define __OTG_CORE_HOST_H__

typedef struct {
    juint16_t vendor_id;
    juint16_t product_id;
    juint16_t bcd_device;
    juint8_t usb_class;
    juint8_t usb_sub_class;
    juint8_t usb_protocol;
} device_details_t;

/* Type for DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED IOCTL parameter */
typedef struct {
    /* IN parameter of the ioctl - The current peripheral details */
    device_details_t dev_details;
    /* OUT parameter of the ioctl - Whether the peripheral is supported by the
     * OTG Host or not */
    jbool_t is_supported;
} periph_support_t;

/* The Host Controller must register itself with the OTG Core during attach */
jresult_t jotg_register_host(jdevice_t host, jdevice_t *otg);

/* The Host Controller must unregister itself from the OTG Core before the OCD
 * is unregistered  */
jresult_t jotg_unregister_host(jdevice_t otg);

#endif
