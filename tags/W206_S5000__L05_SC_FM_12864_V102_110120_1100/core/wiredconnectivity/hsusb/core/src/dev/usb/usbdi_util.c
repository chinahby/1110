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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <jos.h>
#include <jusb.h>
#include <usbdi.h>
#include <usbdi_util.h>

usbd_status usbd_get_desc_flags(usbd_device_handle dev, juint8_t type, 
    juint8_t index, juint16_t len, void *desc, juint16_t flags, juint32_t *actlen)
{
    usb_device_request_t req;

    DBG_V(DHOST_USBDI, ("usbd_get_desc: type=%d, index=%d, len=%d\n",
        type, index, len));

    req.bmRequestType = UT_READ_DEVICE;
    req.bRequest = UR_GET_DESCRIPTOR;
    USETW2(req.wValue, type, index);
    USETW(req.wIndex, 0);
    USETW(req.wLength, len);
    return (usbd_do_request_flags(dev, &req, desc, flags, actlen,
        USBD_DEFAULT_TIMEOUT));
}

usbd_status usbd_get_config_desc(usbd_device_handle dev, juint8_t confidx,
    usb_config_descriptor_t *d)
{
    usbd_status err;

    DBG_V(DHOST_USBDI, ("usbd_get_config_desc: confidx=%d\n", confidx));
    err = usbd_get_desc(dev, UDESC_CONFIG, confidx,
        USB_CONFIG_DESCRIPTOR_SIZE, (void *)d);
    if (err)
        return (err);
    if (d->bDescriptorType != UDESC_CONFIG) 
    {
        DBG_E(DHOST_USBDI, ("usbd_get_config_desc: confidx=%d, bad desc "
            "len=%d type=%d\n", confidx, d->bLength, d->bDescriptorType));
        return (USBD_INVAL);
    }
    return (USBD_NORMAL_COMPLETION);
}

usbd_status usbd_get_config_desc_full(usbd_device_handle dev, juint8_t index,
    usb_config_descriptor_t **cdp)
{
    usb_config_descriptor_t cd;
    usbd_status err;
    juint16_t len;
    jint_t i;

    /* Get the short descriptor. */
    err = usbd_get_config_desc(dev, index, &cd);
    if (err)
        return (err);
    len = UGETW(cd.wTotalLength);
    *cdp = jmalloc(len, M_ZERO);
    if (*cdp == NULL)
        return (USBD_NOMEM);

    /* Get the full descriptor.  Try a few times for slow devices. */
    for (i = 0; i < 3; i++)
    {
        err = usbd_get_desc(dev, UDESC_CONFIG, index, len, (void*)(*cdp));
        if (!err)
            break;
        jdelay_ms(200);
    }
    if (err)
        goto bad;
    if ((*cdp)->bDescriptorType != UDESC_CONFIG) 
    {
        DBG_E(DHOST_SUBR, ("usbd_get_config_desc_full: bad desc %d\n",
            (*cdp)->bDescriptorType));
        err = USBD_INVAL;
        goto bad;
    }

    return USBD_NORMAL_COMPLETION;

bad:
    jfree((void *)*cdp);
    *cdp = NULL;
    return err;
}

usbd_status usbd_get_device_desc(usbd_device_handle dev, 
    usb_device_descriptor_t *d)
{
    DBG_V(DHOST_USBDI, ("usbd_get_device_desc:\n"));
    return (usbd_get_desc(dev, UDESC_DEVICE,
        0, USB_DEVICE_DESCRIPTOR_SIZE, (void *)d));
}

usbd_status usbd_get_device_qualifier(usbd_device_handle dev, 
    usb_device_qualifier_t *d)
{
    DBG_V(DHOST_USBDI, ("usbd_get_device_desc:\n"));
    return (usbd_get_desc(dev, UDESC_DEVICE_QUALIFIER,
        0, USB_DEVICE_QUALIFIER_SIZE, (void *)d));
}

usbd_status usbd_get_device_status(usbd_device_handle dev, usb_status_t *st)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_DEVICE;
    req.bRequest = UR_GET_STATUS;
    USETW(req.wValue, 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, USB_STATUS_SIZE);
    return (usbd_do_request(dev, &req, (void *)st));
}

usbd_status usbd_get_hub_status(usbd_device_handle dev, usb_hub_status_t *st)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_CLASS_DEVICE;
    req.bRequest = UR_GET_STATUS;
    USETW(req.wValue, 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, USB_HUB_STATUS_SIZE);
    return (usbd_do_request(dev, &req, (void *)st));
}

usbd_status usbd_set_address(usbd_device_handle dev, juint8_t addr)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_DEVICE;
    req.bRequest = UR_SET_ADDRESS;
    USETW(req.wValue, addr);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return usbd_do_request(dev, &req, 0);
}

usbd_status usbd_get_port_status(usbd_device_handle dev, juint8_t port, 
    usb_port_status_t *ps)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_CLASS_OTHER;
    req.bRequest = UR_GET_STATUS;
    USETW(req.wValue, 0);
    USETW(req.wIndex, port);
    USETW(req.wLength, USB_PORT_STATUS_SIZE);
    return (usbd_do_request(dev, &req, (void *)ps));
}

usbd_status usbd_clear_hub_feature(usbd_device_handle dev, juint16_t sel)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_CLASS_DEVICE;
    req.bRequest = UR_CLEAR_FEATURE;
    USETW(req.wValue, sel);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_set_hub_feature(usbd_device_handle dev, juint16_t sel)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_CLASS_DEVICE;
    req.bRequest = UR_SET_FEATURE;
    USETW(req.wValue, sel);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_clear_port_feature(usbd_device_handle dev, juint8_t port,
    juint16_t sel)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_CLASS_OTHER;
    req.bRequest = UR_CLEAR_FEATURE;
    USETW(req.wValue, sel);
    USETW(req.wIndex, port);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_set_port_feature(usbd_device_handle dev, juint8_t port, 
    juint16_t sel)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_CLASS_OTHER;
    req.bRequest = UR_SET_FEATURE;
    USETW(req.wValue, sel);
    USETW(req.wIndex, port);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_get_config(usbd_device_handle dev, juint8_t *conf)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_DEVICE;
    req.bRequest = UR_GET_CONFIG;
    USETW(req.wValue, 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 1);
    return (usbd_do_request(dev, &req, conf));
}

#ifdef CONFIG_PORT_INTERCHIP
jresult_t usbd_get_port_interchip(usbd_device_handle dev, juint8_t port, 
    juint8_t *interchip)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_VENDOR_OTHER;
    req.bRequest = UR_GET_INTERCHIP;
    USETW(req.wValue, port);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 1);
    return (usbd_do_request(dev, &req, interchip));
}

jresult_t usbd_get_interface_power(usbd_device_handle dev, juint8_t *buffer)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_VENDOR_DEVICE;
    req.bRequest = UR_GET_POWER;
    USETW(req.wValue, 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 2);
    return (usbd_do_request(dev, &req, buffer));
}

jresult_t usbd_set_interface_power(usbd_device_handle dev, juint8_t *buffer)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_VENDOR_DEVICE;
    req.bRequest = UR_SET_POWER;
    USETW(req.wValue, 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 2);
    return (usbd_do_request(dev, &req, buffer));
}
#endif

static void usbd_bulk_transfer_cb(usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status)
{
    UNUSED_VAR(priv);
    UNUSED_VAR(status);
    jwakeup(usbd_xfer_get_sleep_h(xfer));
}

usbd_status usbd_bulk_transfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    juint16_t flags, juint32_t timeout, void *buf, juint32_t *size)
{
    usbd_status err;
    jresult_t error;

    usbd_setup_xfer(xfer, pipe, 0, buf, *size,
        flags, timeout, usbd_bulk_transfer_cb);
    DBG_I(DHOST_USBDI, ("usbd_bulk_transfer: start transfer %ld bytes\n", 
        *size));
    err = usbd_transfer(xfer);
    if (err != USBD_IN_PROGRESS)
        return (err);

    error = jmsleep(usbd_xfer_get_sleep_h(xfer), 0);
    if (error)
    {
        DBG_E(DHOST_USBDI, ("usbd_bulk_transfer: jmsleep=%d\n", error));
        usbd_abort_pipe(pipe);
        return (USBD_INTERRUPTED);
    }
    usbd_get_xfer_status(xfer, NULL, NULL, size, &err);
    DBG_I(DHOST_USBDI, ("usbd_bulk_transfer: transferred %d\n", *size));
    if (err)
    {
        DBG_E(DHOST_USBDI, ("usbd_bulk_transfer: error=%d\n", err));
        usbd_clear_endpoint_stall(pipe);
    }
    return (err);
}

static void usbd_intr_transfer_cb(usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status)
{
    UNUSED_VAR(status);
    UNUSED_VAR(priv);
    jwakeup(usbd_xfer_get_sleep_h(xfer));
}

usbd_status usbd_intr_transfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    juint16_t flags, juint32_t timeout, void *buf, juint32_t *size)
{
    usbd_status err;
    jresult_t error;

    usbd_setup_xfer(xfer, pipe, 0, buf, *size, flags, timeout,
        usbd_intr_transfer_cb);
    DBG_I(DHOST_USBDI, ("usbd_intr_transfer: start transfer %ld bytes\n",
        *size));
    err = usbd_transfer(xfer);
    if (err != USBD_IN_PROGRESS)
        return (err);

    error = jmsleep(usbd_xfer_get_sleep_h(xfer), 0);
    if (error) 
    {
        DBG_E(DHOST_USBDI, ("usbd_intr_transfer: jmsleep=%d\n", error));
        usbd_abort_pipe(pipe);
        return (USBD_INTERRUPTED);
    }
    usbd_get_xfer_status(xfer, NULL, NULL, size, &err);
    DBG_I(DHOST_USBDI, ("usbd_intr_transfer: transferred %ld\n", *size));
    if (err) 
    {
        DBG_E(DHOST_USBDI, ("usbd_intr_transfer: error=%d\n", err));
        usbd_clear_endpoint_stall(pipe);
    }
    return (err);
}

