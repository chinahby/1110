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
#include <uw_host_hid_common.h>
#include "jhid_util.h"

usbd_status usbd_set_protocol(usbd_interface_handle iface, juint16_t report)
{
    usb_interface_descriptor_t *id = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;
    
    if (id == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_protocol: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }
    
    DBG_V(DHOST_HID_LIB, ("usbd_set_protocol: iface=%p, report=%d, endpt=%d\n",
        iface, report, id->bInterfaceNumber));
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    req.bRequest = UR_SET_PROTOCOL;
    USETW(req.wValue, report);
    USETW(req.wIndex, id->bInterfaceNumber);
    USETW(req.wLength, 0);
    rc = usbd_do_request(dev, &req, NULL);
    
Exit:
    return rc;
}

usbd_status usbd_get_protocol(usbd_interface_handle iface, juint8_t *data)
{
    usb_interface_descriptor_t *id = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;
    
    if (id == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_protocol: Unable to get interface "
            "descriptor from iface %p\n",iface));
        goto Exit;
    }
    
    DBG_V(DHOST_HID_LIB, ("usbd_get_protocol: iface=%p, endpt=%d\n", iface, 
        id->bInterfaceNumber));
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_READ_CLASS_INTERFACE;
    req.bRequest = UR_GET_PROTOCOL;
    USETW(req.wValue, 0);
    USETW(req.wIndex, id->bInterfaceNumber);
    USETW(req.wLength, 1);
    rc = usbd_do_request(dev, &req, data);
    
Exit:
    return rc;
}

usbd_status usbd_set_report(usbd_interface_handle iface, juint8_t type,
    juint8_t id, void *data, juint16_t len)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;

    DBG_V(DHOST_HID_LIB, ("usbd_set_report: len=%d iface=0x%p\n", len, iface));
    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_report: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    req.bRequest = UR_SET_REPORT;
    USETW2(req.wValue, type, id);
    USETW(req.wIndex, ifd->bInterfaceNumber);
    USETW(req.wLength, len);
    rc = usbd_do_request(dev, &req, data);

Exit:
    return rc;
}

usbd_status usbd_set_report_pipe(usbd_interface_handle iface, 
    usbd_pipe_handle pipe, void *data, juint16_t len)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usbd_xfer_handle xfer = NULL;
    usbd_status rc = USBD_INVAL;

    DBG_V(DHOST_HID_LIB, ("usbd_set_report_pipe: len=%d\n", len));

    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_report_pipe: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }

    usbd_interface2device_handle(iface, &dev);
    xfer = usbd_alloc_xfer(dev);
    if (!xfer)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_report_pipe: Unable to allocate "
            "xfer\n"));
        rc = USBD_NOMEM;
        goto Exit;
    }

    usbd_setup_xfer(xfer, pipe, NULL, data, len, 0, 0, NULL);

    rc = usbd_sync_transfer(xfer);
#ifdef JDEBUG
    if (rc)
    {
        DBG_E(DHOST_HID_LIB, ("Failed to send data over intterrupt out pipe "
            "rc=%d\n", rc));
    }
#endif

Exit:
    if (xfer)
        usbd_free_xfer(xfer);

    return rc;
}

usbd_status usbd_set_report_async(usbd_interface_handle iface, juint8_t type, 
    juint8_t id, void *data, juint16_t len)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;

    DBG_V(DHOST_HID_LIB, ("usbd_set_report_async: len=%d\n", len));
    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_report_async: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    req.bRequest = UR_SET_REPORT;
    USETW2(req.wValue, type, id);
    USETW(req.wIndex, ifd->bInterfaceNumber);
    USETW(req.wLength, len);
    rc = usbd_do_request_async(dev, &req, data);

Exit:
    return rc;
}

usbd_status usbd_get_report(usbd_interface_handle iface, juint8_t type,
    juint8_t id, void *data, juint16_t len)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;

    DBG_V(DHOST_HID_LIB, ("usbd_get_report: len=%d\n", len));
    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_get_report: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_READ_CLASS_INTERFACE;
    req.bRequest = UR_GET_REPORT;
    USETW2(req.wValue, type, id);
    USETW(req.wIndex, ifd->bInterfaceNumber);
    USETW(req.wLength, len);
    rc = usbd_do_request(dev, &req, data);

Exit:
    return rc;
}

usbd_status usbd_set_idle(usbd_interface_handle iface, juint8_t duration,
    juint8_t id)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;

    DBG_V(DHOST_HID_LIB, ("usbd_set_idle: %d %d\n", duration, id));
    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_set_idle: Unable to get interface "
            "descriptor from iface %p\n", iface));
        goto Exit;
    }
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    req.bRequest = UR_SET_IDLE;
    USETW2(req.wValue, duration, id);
    USETW(req.wIndex, ifd->bInterfaceNumber);
    USETW(req.wLength, 0);
    rc = usbd_do_request(dev, &req, NULL);

Exit:
    return rc;
}

usbd_status usbd_get_idle(usbd_interface_handle iface, juint8_t id, 
    juint8_t *duration)
{
    usb_interface_descriptor_t *ifd = usbd_get_interface_descriptor(iface);
    usbd_device_handle dev;
    usb_device_request_t req;
    usbd_status rc = USBD_IOERROR;

    DBG_V(DHOST_HID_LIB, ("usbd_get_idle: %d\n", id));
    if (ifd == NULL)
    {
        DBG_E(DHOST_HID_LIB, ("usbd_get_idle: Unable to get interface "
            "descriptor from iface %p\n",iface));
        goto Exit;
    }
    usbd_interface2device_handle(iface, &dev);
    req.bmRequestType = UT_READ_CLASS_INTERFACE;
    req.bRequest = UR_GET_IDLE;
    USETW2(req.wValue, 0, id);
    USETW(req.wIndex, ifd->bInterfaceNumber);
    USETW(req.wLength, 1);
    rc = usbd_do_request(dev, &req, duration);

Exit:
    return rc;
}

usbd_status usbd_get_report_descriptor(usbd_device_handle dev, juint16_t ifcno,
    juint16_t size, void *d, juint8_t report_id)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_INTERFACE;
    req.bRequest = UR_GET_DESCRIPTOR;
    USETW2(req.wValue, UDESC_REPORT, report_id);
    USETW(req.wIndex, ifcno);
    USETW(req.wLength, size);
    return (usbd_do_request(dev, &req, d));
}

usb_hid_descriptor_t * usbd_get_hid_descriptor(usbd_interface_handle ifc)
{
    usb_interface_descriptor_t *idesc = usbd_get_interface_descriptor(ifc);
    usbd_device_handle dev;
    usb_config_descriptor_t *cdesc;
    usb_hid_descriptor_t *hd = NULL;
    juint8_t *p, *end;

    if (idesc == NULL)
    {
        DBG_E(DHOST_HID_LIB, 
            ("usbd_get_hid_descriptor: Unable to get interface descriptor "
            "from iface %p\n",ifc));
        goto Exit;
    }
    
    usbd_interface2device_handle(ifc, &dev);
    cdesc = usbd_get_config_descriptor(dev);

    p = (juint8_t *)idesc + idesc->bLength;
    end = (juint8_t *)cdesc + UGETW(cdesc->wTotalLength);

    for (; p < end; p += hd->bLength) {
        hd = (usb_hid_descriptor_t *)p;
        if (p + hd->bLength <= end && hd->bDescriptorType == UDESC_HID)
            break;
        if (hd->bDescriptorType == UDESC_INTERFACE)
        {
            hd = NULL;
            break;
        }
    }
Exit:
    return hd;
}

usbd_status usbd_read_report_desc(usbd_interface_handle ifc, void **descp, 
    juint16_t *sizep, juint8_t report_id)
{
    usb_interface_descriptor_t *id;
    usb_hid_descriptor_t *hid;
    usbd_device_handle dev;
    usbd_status rc = USBD_NORMAL_COMPLETION;

    usbd_interface2device_handle(ifc, &dev);
    id = usbd_get_interface_descriptor(ifc);
    if (id == NULL)
    {
        rc = USBD_INVAL;
        goto Exit;
    }
    
    hid = usbd_get_hid_descriptor(ifc);
    if (hid == NULL)
    {
        rc = USBD_IOERROR;
        goto Exit;
    }

    if (report_id > hid->bNumDescriptors)
    {
        rc = USBD_INVAL;
        goto Exit;
    }
    
    *sizep = UGETW(hid->descrs[report_id].wDescriptorLength);
    *descp = jmalloc(*sizep, 0);
    if (*descp == NULL)
    {
        rc = USBD_NOMEM;
        goto Exit;
    }
    
    rc = usbd_get_report_descriptor(dev, id->bInterfaceNumber, *sizep, *descp,
        report_id);
    if (rc) 
    {
        jfree(*descp);
        *descp = NULL;
    }

Exit:
    return rc;
}
