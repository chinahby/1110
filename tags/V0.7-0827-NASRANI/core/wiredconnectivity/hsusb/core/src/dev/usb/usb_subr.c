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
#include <usbdivar.h>
#include "usbdevs.h"
#include <usb_quirks.h>
#ifdef CONFIG_JOTG
#include <otg_core_host.h>
#include <uw_otg_app.h>
#endif

static void usbd_devinfo_vp(usbd_device_handle dev, char *v, char *p, 
    jint_t usbdev);
static char *usbd_get_string(usbd_device_handle dev, juint8_t si, char *buf);
static void usbd_free_iface_data(usbd_device_handle dev, jint_t ifcno);
static void usbd_kill_pipe(usbd_pipe_handle pipe);
static usbd_status usbd_probe_and_attach(jdevice_t parent,
    usbd_device_handle dev, juint8_t port, juint8_t addr);
#ifdef CONFIG_JOTG
static usbd_status usbd_check_device_support(usbd_device_handle dev, 
    otg_peripheral_state_t *support_state);
static usbd_status usbd_set_otg_support(usbd_device_handle dev, juint8_t port); 
static usbd_status usbd_notify_dev_support(usbd_device_handle dev, 
    otg_peripheral_state_t support_state);
#endif

#ifdef USBVERBOSE
typedef juint16_t usb_vendor_id_t;
typedef juint16_t usb_product_id_t;

/*
 * Descriptions of of known vendors and devices ("products").
 */
struct usb_knowndev {
    usb_vendor_id_t     vendor;
    usb_product_id_t    product;
    jint_t               flags;
    char                *vendorname;
    char                *productname;
};
#define USB_KNOWNDEV_NOPROD     0x01            /* match on vendor only */

#include "usbdevs_data.h"
#endif /* USBVERBOSE */

static const char * const usbd_error_strs[] = {
    "NORMAL_COMPLETION",
    "IN_PROGRESS",
    "PENDING_REQUESTS",
    "NOT_STARTED",
    "INVAL",
    "NOMEM",
    "CANCELLED",
    "BAD_ADDRESS",
    "IN_USE",
    "NO_ADDR",
    "SET_ADDR_FAILED",
    "NO_POWER",
    "TOO_DEEP",
    "IOERROR",
    "NOT_CONFIGURED",
    "TIMEOUT",
    "SHORT_XFER",
    "STALLED",
    "INTERRUPTED",
    "TEST_FAILURE",
    "INVALID_STATE",
    "XXX",
};

const char *usbd_errstr(usbd_status err)
{
    static char buffer[5];

    if ((juint_t)err < USBD_ERROR_MAX) 
        return usbd_error_strs[err];
    else
    {
        j_snprintf(buffer, sizeof(buffer), "%d", err);
        return buffer;
    }
}

usbd_status usbd_get_string_desc(usbd_device_handle dev, juint8_t sindex, 
    jint16_t langid, usb_string_descriptor_t *sdesc, juint32_t *sizep)
{
    usb_device_request_t req;
    usbd_status err;
    juint32_t actlen;

    req.bmRequestType = UT_READ_DEVICE;
    req.bRequest = UR_GET_DESCRIPTOR;
    USETW2(req.wValue, UDESC_STRING, sindex);
    USETW(req.wIndex, langid);
    USETW(req.wLength, 2);      /* only size byte first */
    err = usbd_do_request_flags(dev, &req, (void *)sdesc, USBD_SHORT_XFER_OK,
        &actlen, USBD_DEFAULT_TIMEOUT);
    if (err)
        return (err);

    if (actlen < 2)
        return (USBD_SHORT_XFER);

    USETW(req.wLength, sdesc->bLength); /* the whole string */
    err = usbd_do_request_flags(dev, &req, (void *)sdesc, USBD_SHORT_XFER_OK,
        &actlen, USBD_DEFAULT_TIMEOUT);
    if (err)
        return (err);

    if (actlen != sdesc->bLength) 
    {
        DBG_E(DHOST_SUBR, ("usbd_get_string_desc: expected %d, "
            "got %ld\n", sdesc->bLength, actlen));
    }

    *sizep = actlen;
    return (USBD_NORMAL_COMPLETION);
}

char *usbd_get_string(usbd_device_handle dev, juint8_t si, char *buf)
{
    jint_t swap = dev->quirks->uq_flags & UQ_SWAP_UNICODE;
    static usb_string_descriptor_t us;
    char *s;
    juint32_t i, n;
    juint16_t c;
    usbd_status err;
    juint32_t size;

    if (si == 0)
        return (0);
    if (dev->quirks->uq_flags & UQ_NO_STRINGS)
        return (0);
    if (dev->langid == USBD_NOLANG) 
    {
        /* Set up default language */
        err = usbd_get_string_desc(dev, USB_LANGUAGE_TABLE, 0, &us,
            &size);
        if (err || size < 4) 
            dev->langid = 0; /* Well, just pick something then */
        else 
        {
            /* Pick the first language as the default. */
            dev->langid = UGETW(us.bString[0]);
        }
    }
    err = usbd_get_string_desc(dev, si, dev->langid, &us, &size);
    if (err)
        return (0);
    s = buf;
    n = size / 2 - 1;
    for (i = 0; i < n; i++) 
    {
        c = UGETW(us.bString[i]);
        /* Convert from Unicode, handle buggy strings. */
        if ((c & 0xff00) == 0)
            *s++ = (char)c;
        else if ((c & 0x00ff) == 0 && swap)
            *s++ = c >> 8;
        else
            *s++ = '?';
    }
    *s++ = 0;
    return (buf);
}

static void usbd_trim_spaces(char *p)
{
    char *q, *e;

    if (p == NULL)
        return;
    q = e = p;
    while (*q == ' ')   /* skip leading spaces */
        q++;
    while (*q)  /* copy string */
    {
        *p = *q++;
        if (*p++ != ' ') /* remember last non-space */
            e = p;
    }
    *p = 0;
    *e = 0;                     /* kill trailing spaces */
}

static void usbd_devinfo_vp(usbd_device_handle dev, char *v, char *p,
    jint_t usedev)
{
    usb_device_descriptor_t *udd = &dev->ddesc;
    char *vendor = 0, *product = 0;
#ifdef USBVERBOSE
    const struct usb_knowndev *kdp;
#endif

    if (dev == NULL)
    {
        v[0] = p[0] = '\0';
        return;
    }

    if (usedev) 
    {
        vendor = usbd_get_string(dev, udd->iManufacturer, v);
        usbd_trim_spaces(vendor);
        product = usbd_get_string(dev, udd->iProduct, p);
        usbd_trim_spaces(product);
        if (vendor && !*vendor)
            vendor = NULL;
        if (product && !*product)
            product = NULL;
    }
    else
    {
        vendor = NULL;
        product = NULL;
    }
#ifdef USBVERBOSE
    if (vendor == NULL || product == NULL) 
    {
        for(kdp = usb_knowndevs; kdp->vendorname != NULL; kdp++) 
        {
            if (kdp->vendor == UGETW(udd->idVendor) &&
                (kdp->product == UGETW(udd->idProduct) ||
                (kdp->flags & USB_KNOWNDEV_NOPROD) != 0))
            {
                break;
            }
        }
        if (kdp->vendorname != NULL) 
        {
            if (vendor == NULL)
                vendor = kdp->vendorname;
            if (product == NULL)
            {
                product = (kdp->flags & USB_KNOWNDEV_NOPROD) == 0 ?
                    kdp->productname : NULL;
            }
        }
    }
#endif
    if (vendor != NULL && *vendor)
    {
        j_strlcpy(v, vendor, USB_MAX_STRING_LEN);
    }
    else
    {
        j_snprintf(v, USB_MAX_STRING_LEN, "vendor 0x%04x",
            UGETW(udd->idVendor));
    }
    if (product != NULL && *product)
    {
        j_strlcpy(p, product, USB_MAX_STRING_LEN);
    }
    else
    {
        j_snprintf(p, USB_MAX_STRING_LEN, "product 0x%04x", 
            UGETW(udd->idProduct));
    }
}

void usbd_devinfo(usbd_device_handle dev, jint_t showclass, char *cp, 
    juint32_t len)
{
#define CP_SNPRINTF(args) \
    n = j_snprintf args; \
    cp += n; \
    len -= n;

    usb_device_descriptor_t *udd = &dev->ddesc;
    usbd_interface_handle iface;
    static char vendor[USB_MAX_STRING_LEN];
    static char product[USB_MAX_STRING_LEN];
    jint_t bcdDevice, bcdUSB;
    usb_interface_descriptor_t *id;
    jint32_t n;

    usbd_devinfo_vp(dev, vendor, product, 1);
    CP_SNPRINTF((cp, len, "%s %s", vendor, product));
    if (showclass & USBD_SHOW_DEVICE_CLASS)
    {
        CP_SNPRINTF((cp, len, ", class %d/%d", udd->bDeviceClass, 
            udd->bDeviceSubClass));
    }
    bcdUSB = UGETW(udd->bcdUSB);
    bcdDevice = UGETW(udd->bcdDevice);
    CP_SNPRINTF((cp, len, ", rev "));
    CP_SNPRINTF((cp, len, "%x.%02x", bcdUSB >> 8, bcdUSB & 0xff));
    *cp++ = '/';
    len--;
    CP_SNPRINTF((cp, len, "%x.%02x", bcdDevice >> 8, bcdDevice & 0xff));
    CP_SNPRINTF((cp, len, ", addr %d", dev->address));
    if (showclass & USBD_SHOW_INTERFACE_CLASS)
    {
        /* fetch the interface handle for the first interface */
        (void)usbd_device2interface_handle(dev, 0, &iface);
        id = usbd_get_interface_descriptor(iface);
        CP_SNPRINTF((cp, len, ", iclass %d/%d", id->bInterfaceClass, 
            id->bInterfaceSubClass));
    }
    *cp = 0;
#undef CP_SNPRINTF
}

usbd_status usbd_reset_port(usbd_device_handle dev, juint8_t port, 
    usb_port_status_t *ps)
{
    usb_device_request_t req;
    usbd_status err;
    jint_t n;

    req.bmRequestType = UT_WRITE_CLASS_OTHER;
    req.bRequest = UR_SET_FEATURE;
    USETW(req.wValue, UHF_PORT_RESET);
    USETW(req.wIndex, port);
    USETW(req.wLength, 0);
    err = usbd_do_request(dev, &req, 0);
    DBG_V(DHOST_SUBR, ("usbd_reset_port: port %d reset done, error=%s\n", port,
        usbd_errstr(err)));
    if (err)
        return (err);
    n = 10;
    do {
        /* Wait for device to recover from reset. */
        jdelay_ms(USB_PORT_RESET_DELAY);
        err = usbd_get_port_status(dev, port, ps);
        if (err) 
        {
            DBG_W(DHOST_SUBR, ("usbd_reset_port: get status failed %d\n", err));
            return (err);
        }
        /* If the device disappeared, just give up. */
        if (!(UGETW(ps->wPortStatus) & UPS_CURRENT_CONNECT_STATUS))
            return (USBD_NORMAL_COMPLETION);
    } while ((UGETW(ps->wPortChange) & UPS_C_PORT_RESET) == 0 && --n > 0);
    
    err = usbd_clear_port_feature(dev, port, UHF_C_PORT_RESET);
#ifdef USB_DEBUG
    if (err)
    {
        DBG_W(DHOST_SUBR, ("usbd_reset_port: clear port feature failed %d\n",
            err));
    }
#endif
    if (n == 0)
    {
        DBG_W(DHOST_SUBR, ("usbd_reset_port: Reset failed due to timeout\n"));
        err = USBD_TIMEOUT;
    }

    /* Wait for the device to recover from reset. */
    jdelay_ms(USB_PORT_RESET_RECOVERY);
    return (err);
}

usb_interface_descriptor_t *usbd_find_idesc(usb_config_descriptor_t *cd, 
    jint_t ifaceidx, jint_t altidx)
{
    char *p = (char *)cd;
    char *end = p + UGETW(cd->wTotalLength);
    usb_interface_descriptor_t *d;
    jint_t curidx, lastidx, curaidx = 0;

    for (curidx = lastidx = -1; p < end; ) 
    {
        d = (usb_interface_descriptor_t *)p;
        DBG_V(DHOST_SUBR, ("usbd_find_idesc: idx=%d(%d) altidx=%d(%d) len=%d "
            "type=%d\n",
            ifaceidx, curidx, altidx, curaidx,
            d->bLength, d->bDescriptorType));
        if (d->bLength == 0) /* bad descriptor */
            break;
        p += d->bLength;
        if (p <= end && d->bDescriptorType == UDESC_INTERFACE) 
        {
            if (d->bInterfaceNumber != lastidx) 
            {
                lastidx = d->bInterfaceNumber;
                curidx++;
                curaidx = 0;
            } 
            else
                curaidx++;
            if (ifaceidx == curidx && altidx == curaidx)
                return (d);
        }
    }
    return (NULL);
}

usb_endpoint_descriptor_t *usbd_find_edesc(usb_config_descriptor_t *cd,
    jint_t ifaceidx, jint_t altidx, jint_t endptidx)
{
    char *p = (char *)cd;
    char *end = p + UGETW(cd->wTotalLength);
    usb_interface_descriptor_t *d;
    usb_endpoint_descriptor_t *e;
    jint_t curidx;

    d = usbd_find_idesc(cd, ifaceidx, altidx);
    if (d == NULL)
        return (NULL);
    if (endptidx >= d->bNumEndpoints) /* quick exit */
        return (NULL);

    curidx = -1;
    for (p = (char *)d + d->bLength; p < end; ) 
    {
        e = (usb_endpoint_descriptor_t *)p;
        if (e->bLength == 0) /* bad descriptor */
            break;
        p += e->bLength;
        if (p <= end && e->bDescriptorType == UDESC_INTERFACE)
            return (NULL);
        if (p <= end && e->bDescriptorType == UDESC_ENDPOINT) 
        {
            curidx++;
            if (curidx == endptidx)
                return (e);
        }
    }
    return (NULL);
}

usbd_status usbd_fill_iface_data(usbd_device_handle dev,
    jint_t ifaceidx, jint_t altidx)
{
    usbd_interface_handle ifc = &dev->ifaces[ifaceidx];
    usb_interface_descriptor_t *idesc;
    char *p, *end;
    jint_t endpt, nendpt;

    DBG_V(DHOST_SUBR, ("usbd_fill_iface_data: ifaceidx=%d altidx=%d\n",
        ifaceidx, altidx));
    idesc = usbd_find_idesc(dev->cdesc, ifaceidx, altidx);
    if (idesc == NULL)
        return (USBD_INVAL);
    ifc->device = dev;
    ifc->idesc = idesc;
    ifc->index = ifaceidx;
    ifc->altindex = altidx;
    nendpt = ifc->idesc->bNumEndpoints;
    DBG_V(DHOST_SUBR, ("usbd_fill_iface_data: found idesc nendpt=%d\n", 
        nendpt));
    if (nendpt != 0) 
    {
        ifc->endpoints = jmalloc(nendpt * sizeof(struct usbd_endpoint), M_ZERO);
        if (ifc->endpoints == NULL)
            return (USBD_NOMEM);
    } 
    else
        ifc->endpoints = NULL;
    ifc->priv = NULL;
    p = (char *)ifc->idesc + ifc->idesc->bLength;
    end = (char *)dev->cdesc + UGETW(dev->cdesc->wTotalLength);
#define ed ((usb_endpoint_descriptor_t *)p)
    for (endpt = 0; endpt < nendpt; endpt++)
    {
        DBG_X(DHOST_SUBR, ("usbd_fill_iface_data: endpt=%d\n", endpt));
        for (; p < end; p += ed->bLength) 
        {
            DBG_X(DHOST_SUBR, ("usbd_fill_iface_data: p=%p end=%p len=%d "
                "type=%d\n", p, end, ed->bLength, ed->bDescriptorType));
            if (p + ed->bLength <= end && ed->bLength != 0 &&
                ed->bDescriptorType == UDESC_ENDPOINT)
            {
                goto found;
            }
            if (ed->bLength == 0 || ed->bDescriptorType == UDESC_INTERFACE)
                break;
        }
        /* passed end, or bad desc */
        jprintf("usbd_fill_iface_data: bad descriptor(s): %s\n",
            ed->bLength == 0 ? "0 length" :
            ed->bDescriptorType == UDESC_INTERFACE ? "iface desc":
            "out of data");
        goto bad;
found:
        ifc->endpoints[endpt].edesc = ed;
        if (dev->speed == USB_SPEED_HIGH) 
        {
            juint_t mps;
            /* Control and bulk endpoints have max packet limits. */
            switch (UE_GET_XFERTYPE(ed->bmAttributes)) 
            {
            case UE_CONTROL:
                mps = USB_2_MAX_CTRL_PACKET;
                goto check;
            case UE_BULK:
                mps = USB_2_MAX_BULK_PACKET;
check:
                if (UGETW(ed->wMaxPacketSize) != mps) 
                {
                    USETW(ed->wMaxPacketSize, mps);
#ifdef DIAGNOSTIC
                    jprintf("usbd_fill_iface_data: bad max "
                        "packet size\n");
#endif
                }
                break;
            default:
                break;
            }
        }
        ifc->endpoints[endpt].refcnt = 0;
        p += ed->bLength;
    }
#undef ed
    LIST_INIT(&ifc->pipes);
    return (USBD_NORMAL_COMPLETION);

bad:
    if (ifc->endpoints != NULL) 
    {
        jfree(ifc->endpoints);
        ifc->endpoints = NULL;
    }
    return (USBD_INVAL);
}

void usbd_free_iface_data(usbd_device_handle dev, jint_t ifcno)
{
    usbd_interface_handle ifc = &dev->ifaces[ifcno];
    if (ifc->endpoints)
    {
        jfree(ifc->endpoints);
        ifc->endpoints = NULL;
    }
}

usbd_status usbd_set_config(usbd_device_handle dev, juint16_t conf)
{
    usb_device_request_t req;

    req.bmRequestType = UT_WRITE_DEVICE;
    req.bRequest = UR_SET_CONFIG;
    USETW(req.wValue, conf);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_set_remote_wakeup(usbd_device_handle dev, jbool_t set)
{
    usb_device_request_t req;

    if (!(dev->cdesc->bmAttributes & UC_REMOTE_WAKEUP))
        return USBD_INVAL;

    req.bmRequestType = UT_WRITE_DEVICE;
    req.bRequest = set ? UR_SET_FEATURE : UR_CLEAR_FEATURE;
    USETW(req.wValue, UF_DEVICE_REMOTE_WAKEUP);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

usbd_status usbd_set_config_no(usbd_device_handle dev, juint8_t no, jint_t msg)
{
    juint8_t index;
    usb_config_descriptor_t cd;
    usbd_status err;

    if (no == USB_UNCONFIG_NO)
        return (usbd_set_config_index(dev, USB_UNCONFIG_INDEX, msg));

    DBG_V(DHOST_SUBR, ("usbd_set_config_no: %d\n", no));
    /* Figure out what config index to use. */
    for (index = 0; index < dev->ddesc.bNumConfigurations; index++) 
    {
        err = usbd_get_config_desc(dev, index, &cd);
        if (err)
            return (err);
        if (cd.bConfigurationValue == no)
            return (usbd_set_config_index(dev, index, msg));
    }
    return (USBD_INVAL);
}

usbd_status usbd_set_config_index(usbd_device_handle dev, juint8_t index, 
    jint_t msg)
{
    usb_status_t ds;
    usb_config_descriptor_t *cdp;
    usbd_status err;
    jint_t selfpowered, power;
    juint8_t ifcidx, nifc;
    juint8_t default_config = dev->default_cdesc->bConfigurationValue;

    DBG_V(DHOST_SUBR ,("usbd_set_config_index: dev=%p index=%d\n", dev, index));

    if (dev->config != USB_UNCONFIG_NO)
    {
        nifc = dev->cdesc->bNumInterface;

        /* Check that all interfaces are idle */
        for (ifcidx = 0; ifcidx < nifc; ifcidx++)
        {
            if (LIST_EMPTY(&dev->ifaces[ifcidx].pipes))
                continue;
            DBG_E(DHOST_SUBR, ("usbd_set_config_index: open pipes exist\n"));
            return (USBD_IN_USE);
        }

        DBG_V(DHOST_SUBR, ("usbd_set_config_index: free old config\n"));
        /* Free all configuration data structures. */
        for (ifcidx = 0; ifcidx < nifc; ifcidx++)
            usbd_free_iface_data(dev, ifcidx);
        jfree(dev->ifaces);
        dev->ifaces = NULL;
        /* Free the cdesc only if not running the default configuration */
        if (dev->config != default_config)
            jfree((void *)dev->cdesc);
        dev->cdesc = NULL;
        dev->config = USB_UNCONFIG_NO;
    }

    KASSERT(!(dev->ifaces || dev->cdesc), ("USBSUBR: No config set but "
        "dev->ifaces is %p and dev->cdesc is %p\n", dev->ifaces, dev->cdesc));

    if (index == USB_UNCONFIG_INDEX) 
    {
        /* We are unconfiguring the device, so leave unallocated. */
        DBG_V(DHOST_SUBR, ("usbd_set_config_index: set config 0\n"));
        err = usbd_set_config(dev, USB_UNCONFIG_NO);
        if (err)
        {
            DBG_E(DHOST_SUBR, ("usbd_set_config_index: setting config=0 "
                "failed, error=%s\n", usbd_errstr(err)));
        }
        return (err);
    }

    /* Get the Configuration Descriptor. */
    if (index == USB_UNCONFIG_NO)
        cdp = dev->default_cdesc;
    else
    {
        DBG_X(DHOST_SUBR, ("usbd_set_config_index: Getting device's (0x%x) "
            "configuration descriptor for index %d\n", dev, index));
        err = usbd_get_config_desc_full(dev, index, &cdp);
        if (err)
            return (err);
    }
    
    /* Figure out if the device is self or bus powered. */
    selfpowered = 0;
    if (!(dev->quirks->uq_flags & UQ_BUS_POWERED) &&
        (cdp->bmAttributes & UC_SELF_POWERED)) 
    {
        /* May be self powered. */
        if (cdp->bmAttributes & UC_BUS_POWERED) 
        {
            /* Must ask device. */
            if (dev->quirks->uq_flags & UQ_POWER_CLAIM) 
            {
                /*
                 * Hub claims to be self powered, but isn't.
                 * It seems that the power status can be
                 * determined by the hub characteristics.
                 */
                usb_hub_descriptor_t hd;
                usb_device_request_t req;
                req.bmRequestType = UT_READ_CLASS_DEVICE;
                req.bRequest = UR_GET_DESCRIPTOR;
                USETW(req.wValue, 0);
                USETW(req.wIndex, 0);
                USETW(req.wLength, USB_HUB_DESCRIPTOR_SIZE);
                err = usbd_do_request(dev, &req, (void *)&hd);
                if (!err && 
                    (UGETW(hd.wHubCharacteristics) & UHD_PWR_INDIVIDUAL))
                {
                    selfpowered = 1;
                }
                DBG_V(DHOST_SUBR, ("usbd_set_config_index: charac=0x%04x"
                    ", error=%s\n", UGETW(hd.wHubCharacteristics),
                    usbd_errstr(err)));
            }
            else 
            {
                err = usbd_get_device_status(dev, &ds);
                if (!err && (UGETW(ds.wStatus) & UDS_SELF_POWERED))
                    selfpowered = 1;
                DBG_V(DHOST_SUBR, ("usbd_set_config_index: status=0x%04x"
                    ", error=%s\n", UGETW(ds.wStatus), usbd_errstr(err)));
            }
        } 
        else
            selfpowered = 1;
    }
    DBG_V(DHOST_SUBR, ("usbd_set_config_index: (addr %d) cno=%d attr=0x%02x, "
        "selfpowered=%d, power=%d\n", cdp->bConfigurationValue,
        dev->address, cdp->bmAttributes, selfpowered, cdp->bMaxPower * 2));

    /* Check if we have enough power. */
#ifdef USB_DEBUG
    if (dev->powersrc == NULL) 
    {
        DBG_E(DHOST_SUBR, ("usbd_set_config_index: No power source?\n"));
        return (USBD_IOERROR);
    }
#endif
    power = cdp->bMaxPower * 2;
    if (power > dev->powersrc->power)
    {
        DBG_I(DHOST_SUBR, ("power exceeded %d %d\n", power,
            dev->powersrc->power));
        if (msg)
        {
            jprintf("%s: device addr %d (config %d) exceeds power "
                "budget, %d mA > %d mA\n",
                j_device_get_nameunit(dev->bus->bdev), dev->address,
                cdp->bConfigurationValue, power, dev->powersrc->power);
        }
        err = USBD_NO_POWER;
        goto bad;
    }
    dev->power = (juint16_t)power;
    dev->self_powered = (juint8_t)selfpowered;

    /* Set the actual configuration value. */
    DBG_V(DHOST_SUBR, ("usbd_set_config_index: set config %d\n",
        cdp->bConfigurationValue));
    err = usbd_set_config(dev, cdp->bConfigurationValue);
    if (err)
    {
        DBG_E(DHOST_SUBR, ("usbd_set_config_index: setting config=%d failed, "
            "error=%s\n", cdp->bConfigurationValue, usbd_errstr(err)));
        goto bad;
    }

    /* Allocate and fill interface data. */
    nifc = cdp->bNumInterface;
    dev->ifaces = jmalloc(nifc * sizeof(struct usbd_interface), M_ZERO);
    if (dev->ifaces == NULL) 
    {
        err = USBD_NOMEM;
        goto bad;
    }
    DBG_V(DHOST_SUBR, ("usbd_set_config_index: dev=%p cdesc=%p\n", dev, cdp));
    dev->cdesc = cdp;

    for (ifcidx = 0; ifcidx < nifc; ifcidx++) 
    {
        err = usbd_fill_iface_data(dev, ifcidx, 0);
        if (err) 
        {
            juint8_t i;
            for (i = 0; i < ifcidx; i++)
                usbd_free_iface_data(dev, i);
            goto bad;
        }
    }

    dev->config = cdp->bConfigurationValue;
    return (USBD_NORMAL_COMPLETION);

bad:
    if (dev->ifaces)
    {
        jfree(dev->ifaces);
        dev->ifaces = NULL;
    }

    dev->cdesc = NULL;
    if (index != USB_UNCONFIG_NO)
        jfree((void *)cdp);
    return (err);
}

usbd_status usbd_setup_pipe(usbd_device_handle dev, usbd_interface_handle iface,
    struct usbd_endpoint *ep, jint_t ival, usbd_pipe_handle *pipe)
{
    usbd_pipe_handle p;
    usbd_status err;

    DBG_I(DHOST_SUBR, ("usbd_setup_pipe: dev=%p iface=%p ep=%p pipe=%p\n",
        dev, iface, ep, pipe));
    p = jmalloc(dev->bus->pipe_size, M_ZERO);
    if (p == NULL)
        return (USBD_NOMEM);
    p->device = dev;
    p->iface = iface;
    p->endpoint = ep;
    ep->refcnt++;
    p->refcnt = 1;
    p->interval = ival;
    TAILQ_INIT(&p->queue);

    /* For root hub route the pipe control directly to the generic driver */
    if (dev->address == root_hub_get_address(&(dev->bus->rootsoftc)))
    {
        DBG_I(DHOST_SUBR, ("usbd_setup_pipe: Opening root hub pipe\n"));
        err = root_hub_open_pipe(p);
    }
    else
        err = dev->bus->methods->open_pipe(p);

    if (err) 
    {
        DBG_E(DHOST_SUBR, ("usbd_setup_pipe: endpoint=0x%x failed, error=%s\n",
            ep->edesc->bEndpointAddress, usbd_errstr(err)));
        jfree(p);
        return (err);
    }
    /* Clear any stall and make sure DATA0 toggle will be used next. */
    if (UE_GET_ADDR(ep->edesc->bEndpointAddress) != USB_CONTROL_ENDPOINT) 
    {
        if (dev->quirks->uq_flags & UQ_NO_CLEAR_HALT)
            usbd_clear_endpoint_toggle(p);
        else
        {
            err = usbd_clear_endpoint_stall(p);
            /* Some devices reject this command, so ignore a STALL. */
            if (err && err != USBD_STALLED) 
            {
                jprintf("usbd_setup_pipe: failed to start endpoint, %s\n", 
                    usbd_errstr(err));
                jfree(p);
                return (err);
            }
        }
    }
    *pipe = p;
    return (USBD_NORMAL_COMPLETION);
}

/* Abort the device control pipe. */
void usbd_kill_pipe(usbd_pipe_handle pipe)
{
    usbd_abort_pipe(pipe);
    pipe->methods->close(pipe);
    pipe->endpoint->refcnt--;
    jfree(pipe);
}

juint8_t usbd_getnewaddr(usbd_bus_handle bus)
{
    juint8_t addr;

    for (addr = 1; addr < USB_MAX_DEVICES; addr++)
    {
        if (bus->devices[addr] == 0)
            return (addr);
    }
    return 0;
}


usbd_status usbd_probe_and_attach(jdevice_t parent, usbd_device_handle dev,
    juint8_t port, juint8_t addr)
{
    usb_device_descriptor_t *dd = &dev->ddesc;
    struct usb_attach_arg uaa;
    jresult_t rc;

    dev->bdev = jdevice_add(parent);
    if (!dev->bdev) 
    {
        device_printf(parent, ("Device creation failed\n"));
        return USBD_INVAL;
    }

    j_memset(&uaa, 0, sizeof(uaa));

    /* Allow devices to use the UAA during probe & attach */
    j_device_set_ivars(dev->bdev, &uaa);

    uaa.device = dev;
    uaa.port = port;
    uaa.configno = UHUB_UNK_CONFIGURATION;
    uaa.ifaceno = UHUB_UNK_INTERFACE;
    uaa.vendor = UGETW(dd->idVendor);
    uaa.product = UGETW(dd->idProduct);
    uaa.release = UGETW(dd->bcdDevice);
    uaa.parent = parent;
   
    /* First try with device specific drivers. */
    DBG_V(DHOST_SUBR, ("usbd_probe_and_attach: trying device specific "
        "drivers\n"));

    /* Attach to driver */
    rc = j_device_probe_and_attach(dev->bdev, dev);

    /* Prevent the devices from using the UAA anymore */
    j_device_set_ivars(dev->bdev, NULL);

    if (rc == 0)
        return (USBD_NORMAL_COMPLETION);

#ifdef CONFIG_JOTG
    /* If no driver was attached set the OTG flag to Not Supported */
    dev->otg_support = OTG_PERIPH_NOT_SUPPORTED;
#endif

    /* Even if the generic attach failed, we leave the device as it is.
     * We just did not find any drivers, that's all.  The device is
     * fully operational and not harming anyone.  */

    devmon_notify(DM_NEW_DEVICE_NO_DRIVER, dev, 0);

    jprintf("%s: at %s port %d (addr %d) connected. "
        "No driver was found for this device.\n",
        j_device_get_nameunit(dev->bdev), 
        j_device_get_nameunit(parent), port,
        dev->address);

    return (USBD_NORMAL_COMPLETION);
}

#ifdef CONFIG_PORT_INTERCHIP
jresult_t usbd_port_interchip(usbd_device_handle dev)
{
    juint8_t power[2];
    juint8_t set_voltage_class, set_max_current;
    jbool_t continue_enum;
    jresult_t err = 0;

    /* Getting the voltage class and the max current for the intechip */
    err = usbd_get_interface_power(dev, power);
    if (err) 
    {
        DBG_E(DHOST_SUBR, ("usbd_port_interchip: getting power interface for "
            "interchip failed\n"));
        goto Exit;
    }

    /* Notifiy the application of the received voltage class and 
     * max_current, and receive information for the set command */
    continue_enum = j_power_request(power[0], power[1], &set_voltage_class, 
        &set_max_current);
    if (!continue_enum)
    {
        DBG_E(DHOST_SUBR, ("usbd_port_interchip: stopping anumeration due to "
            "the application request\n"));
        err = ECANCEL;
        goto Exit;
    }

    /* Setting the voltage class and the max current for the intechip */
    power[0] = set_voltage_class;
    power[1] = set_max_current;

    err = usbd_set_interface_power(dev, power);
    if (err) 
    {
        DBG_E(DHOST_SUBR, ("usbd_port_interchip: getting power interface for "
            "interchip failed\n"));
        goto Exit;
    }

Exit:
    return err;
}
#endif

/*
 * Called when a new device has been put in the powered state,
 * but not yet in the addressed state.
 * Get initial descriptor, set the address, get full descriptor,
 * and attach a driver.
 */
usbd_status usbd_new_device(jdevice_t parent, usbd_bus_handle bus, 
    juint8_t depth, juint8_t speed, juint8_t port, struct usbd_port *up)
{
    usbd_device_handle dev = NULL;
    usb_device_descriptor_t *dd;
    usb_port_status_t ps;
    usbd_status err = USBD_NORMAL_COMPLETION;
    juint8_t addr;
    jint_t i;

    jprintf("%s: new device port=%d depth=%d speed=%s\n",
        j_device_get_nameunit(parent), port, depth, 
        (speed == USB_SPEED_LOW ? "low" : 
        (speed == USB_SPEED_FULL ? "full" : 
        (speed == USB_SPEED_HIGH ? "high" : "unknown"))));

    addr = usbd_getnewaddr(bus);
    if (!addr) 
    {
        jprintf("%s: No free USB addresses, new device ignored.\n",
            j_device_get_nameunit(bus->bdev));
        err = USBD_NO_ADDR;
        goto Error;
    }

    dev = usbd_create_device(parent, bus, depth, speed, port, up);
    if (!dev)
    {
        err = USBD_NOMEM;
        goto Error;
    }

    devmon_notify(DM_NEW_DEVICE, dev, 0);

    /* Set the address.  Do this early; some devices need that. */
    /* Try a few times in case the device is slow (i.e. outside specs.) */
    DBG_V(DHOST_SUBR, ("usbd_new_device: setting device address=%d\n", addr));
    for (i = 0; i < 7; i++) 
    {
        err = usbd_set_address(dev, addr);
        if (!err)
            break;
        jdelay_ms(200);

        /* Try a port reset every 3rd time */
        if ((i % 3) == 2)
        {
            if (up->parent)
            {
                DBG_W(DHOST_SUBR, ("usb_new_device: set address %d failed - "
                    "trying a port reset\n", addr));
                usbd_reset_port(up->parent, port, &ps);
            }
            else
            {
                DBG_W(DHOST_SUBR, ("usb_new_device: set address %d failed for "
                    "root hub.\n", addr));
                break;
            }
        }
    }
    if (err)
    {
        devmon_notify(DM_ERROR_CANNOT_SET_ADDR, dev, (juint32_t)err);
        DBG_E(DHOST_SUBR, ("usb_new_device: set address %d failed\n", addr));
        err = USBD_SET_ADDR_FAILED;
        goto Error;
    }
    /* Allow device time to set new address */
    jdelay_ms(USB_SET_ADDRESS_SETTLE);
    dev->address = addr;        /* New device address now */
    bus->devices[addr] = dev;

#ifdef CONFIG_PORT_INTERCHIP
    if (up->is_interchip)
    {
        err = usbd_port_interchip(dev);
        if (err) 
        {
            DBG_E(DHOST_SUBR, ("usbd_new_device: addr=%d, power negotiation "
                "for interchip failed\n", addr));

            devmon_notify(DM_ERROR_POWER_NEGOTIATION_FAILED, dev, 
                (juint32_t)err);
            goto Error;
        }
    }
#endif

    dd = &dev->ddesc;
    /* Get the first 8 bytes of the device descriptor. */
    err = usbd_get_desc(dev, UDESC_DEVICE, 0, USB_MAX_IPACKET, (void*)dd);
    if (err) 
    {
        devmon_notify(DM_ERROR_CANNOT_GET_FIRST_DESC, dev, (juint32_t)err);
        DBG_E(DHOST_SUBR, ("usbd_new_device: addr=%d, getting first desc "
            "failed\n", addr));
        goto Error;
    }

    if (speed == USB_SPEED_HIGH) 
    {
        /* Max packet size must be 64 (sec 5.5.3). */
        if (dd->bMaxPacketSize != USB_2_MAX_CTRL_PACKET) 
        {
#ifdef DIAGNOSTIC
            jprintf("usbd_new_device: addr=%d bad max packet size\n", addr);
#endif
            dd->bMaxPacketSize = USB_2_MAX_CTRL_PACKET;
        }
    }

    DBG_V(DHOST_SUBR, ("usbd_new_device: adding unit addr=%d, rev=%02x, "
        "class=%d, subclass=%d, protocol=%d, maxpacket=%d, len=%d, speed=%d\n",
        addr,UGETW(dd->bcdUSB), dd->bDeviceClass, dd->bDeviceSubClass,
        dd->bDeviceProtocol, dd->bMaxPacketSize, dd->bLength, dev->speed));

    if (dd->bDescriptorType != UDESC_DEVICE) 
    {
        /* Illegal device descriptor */
        devmon_notify(DM_ERROR_BAD_DEV_DESC, dev, 0);
        DBG_E(DHOST_SUBR, ("usbd_new_device: illegal descriptor %d\n",
            dd->bDescriptorType));
        err = USBD_INVAL;
        goto Error;
    }

    if (dd->bLength < USB_DEVICE_DESCRIPTOR_SIZE) 
    {
        devmon_notify(DM_ERROR_BAD_DEV_DESC, dev, 0);
        DBG_E(DHOST_SUBR, ("usbd_new_device: bad length %d\n", dd->bLength));
        err = USBD_INVAL;
        goto Error;
    }

    USETW(dev->def_ep_desc.wMaxPacketSize, dd->bMaxPacketSize);

    err = usbd_reload_device_desc(dev);
    if (err) 
    {
        devmon_notify(DM_ERROR_BAD_DEV_DESC, dev, 0);
        DBG_E(DHOST_SUBR, ("usbd_new_device: addr=%d, getting full desc "
            "failed\n", addr));
        goto Error;
    }

    /* Assume 100mA bus powered for now. Changed when configured. */
    dev->power = USB_MIN_POWER;
    dev->self_powered = 0;

    DBG_X(DHOST_SUBR, ("usbd_new_device: Getting the device's (0x%x) default "
        "configuration descriptor\n", dev)); 
    err = usbd_get_config_desc_full(dev, 0, &dev->default_cdesc);
    if (err)
    {
        devmon_notify(DM_ERROR_CANNOT_GET_CONFIG, dev, 0);
        DBG_E(DHOST_SUBR, ("usbd_new_device: Failed getting device's default "
            "configuration descriptor - %d\n", err));
        goto Error;
    }

    DBG_I(DHOST_SUBR, ("usbd_new_device: new dev (addr %d), dev=%p, "
        "parent=%p\n", addr, dev, parent));

#ifdef CONFIG_JOTG
    if (dev->bus->otg_port)
    {
        /* Check if the device is supported at the device descriptor level */
        err = usbd_check_device_support(dev, &dev->otg_support);
        if (err)
            goto Error;

        /* HSU temp fix */ 
        if (dev->otg_support == OTG_PERIPH_SUPPORTED) 
        { 
          devmon_notify(DM_DEVICE_SUPPORTED, dev, 0); 
        } 
        else 
        { 
          if (dev->otg_support == OTG_PERIPH_PARTIAL_SUPPORTED) 
          { 
            devmon_notify(DM_DEVICE_PARTIAL_SUPPORTED, dev, 0); 
          } 
          else 
          { 
            devmon_notify(DM_DEVICE_NOT_SUPPORTED, dev, 0); 
          } 
        } 
        /* HSU temp fix */ 


        /* If the device is not supported do not attach it */
        if (dev->otg_support != OTG_PERIPH_SUPPORTED)
            goto Exit;

        /* If the connected device is an OTG device, Inform it about 
         * its connection status */
        err = usbd_set_otg_support(dev, port);
        if (err)
            goto Error;
    }
#endif
    
    err = usbd_probe_and_attach(parent, dev, port, addr);
    if (err)
    {
        devmon_notify(DM_ERROR_CANNOT_ATTACH, dev, (juint32_t)err);
        goto Error;
    }

    devmon_notify(DM_NEW_DEVICE_READY, dev, 0);

#ifdef CONFIG_JOTG
Exit:
    if (up->parent == bus->root_hub &&
        dev->bus->otg_port && port == dev->bus->otg_port)
    {
        err = usbd_notify_dev_support(dev, dev->otg_support);
        if (err)
            goto Error;
    }
#endif

    return (USBD_NORMAL_COMPLETION);
Error:
    usbd_remove_device(dev, up);
    return (err);
}

usbd_device_handle usbd_create_device(jdevice_t parent, usbd_bus_handle bus, 
    juint8_t depth, juint8_t speed, juint8_t port, struct usbd_port *up)
{
    usbd_status err;
    usbd_device_handle dev;

    UNUSED_VAR(parent);
    dev = jmalloc(sizeof *dev, M_ZERO);
    if (dev == NULL)
        return NULL;

    dev->bus = bus;

    /* Set up default endpoint handle. */
    dev->def_ep.edesc = &dev->def_ep_desc;

    /* Set up default endpoint descriptor. */
    dev->def_ep_desc.bLength = USB_ENDPOINT_DESCRIPTOR_SIZE;
    dev->def_ep_desc.bDescriptorType = UDESC_ENDPOINT;
    dev->def_ep_desc.bEndpointAddress = USB_CONTROL_ENDPOINT;
    dev->def_ep_desc.bmAttributes = UE_CONTROL;
    USETW(dev->def_ep_desc.wMaxPacketSize, USB_MAX_IPACKET);
    dev->def_ep_desc.bInterval = 0;

    dev->quirks = &usbd_no_quirk;
    dev->address = USB_START_ADDR;
    dev->ddesc.bMaxPacketSize = 0;
    dev->depth = (juint8_t)depth;
    dev->powersrc = up;
    dev->myhub = up->parent;

    /* If parent is HighSpeed he is our high-hub */
    if (!up->parent || up->parent->speed == USB_SPEED_HIGH)
    {
        dev->myhighhub = up->parent;
        dev->myhighport = port;
    }
    /* Otherwise our high-hub is the same as our parent's */
    else
    {
        dev->myhighhub = up->parent->myhighhub;
        dev->myhighport = up->parent->myhighport;
    }

    dev->speed = (juint8_t)speed;
    dev->langid = USBD_NOLANG;

    /* Establish the default pipe. */
    err = usbd_setup_pipe(dev, 0, &dev->def_ep, USBD_DEFAULT_INTERVAL,
        &dev->default_pipe);
    if (err)
        return NULL;

    up->device = dev;

    return dev;
}

usbd_status usbd_reload_device_desc(usbd_device_handle dev)
{
    usbd_status err = USBD_NORMAL_COMPLETION;
    jint_t i;

    /* Get the full device descriptor. */
    for (i = 0; i < 3; ++i) 
    {
        err = usbd_get_device_desc(dev, &dev->ddesc);
        if (!err)
            break;
        jdelay_ms(200);
    }
    if (err)
        return (err);

    /* Figure out what's wrong with this device. */
    dev->quirks = usbd_find_quirk(&dev->ddesc);

    return (USBD_NORMAL_COMPLETION);
}

void usbd_remove_device(usbd_device_handle dev, struct usbd_port *up)
{
    DBG_V(DHOST_SUBR, ("usbd_remove_device: %p\n", dev));

    if (!dev)
        return;

    if (dev->default_pipe != NULL)
        usbd_kill_pipe(dev->default_pipe);
    while (dev->ctrl_xfer_count)
        jdelay_ms(100);

    up->device = 0;
    dev->bus->devices[dev->address] = 0;

    jfree(dev);
}

void usb_free_device(usbd_device_handle dev)
{
    jint_t ifcidx, nifc;

    if (dev->default_pipe != NULL)
        usbd_kill_pipe(dev->default_pipe);
    while (dev->ctrl_xfer_count)
        jdelay_ms(100);
    if (dev->ifaces != NULL) 
    {
        nifc = dev->cdesc->bNumInterface;
        for (ifcidx = 0; ifcidx < nifc; ifcidx++)
            usbd_free_iface_data(dev, ifcidx);
        jfree(dev->ifaces);
    }
    if (dev->default_cdesc != NULL)
    {
        /* The current configuration's descriptor might be the same as the
         * default and therefore free it only if it is different */
        if (dev->cdesc && (dev->cdesc != dev->default_cdesc))
            jfree((void *)dev->cdesc);
        jfree((void *)dev->default_cdesc);
    }

    jfree(dev);
}

/*
 * The general mechanism for detaching drivers works as follows: Each
 * driver is responsible for maintaining a reference count on the
 * number of outstanding references to its softc (e.g.  from
 * processing hanging in a read or write).  The detach method of the
 * driver decrements this counter and flags in the softc that the
 * driver is dying and then wakes any sleepers.  It then sleeps on the
 * softc.  Each place that can sleep must maintain the reference
 * count.  When the reference count drops to -1 (0 is the normal value
 * of the reference count) the a wakeup on the softc is performed
 * signaling to the detach waiter that all references are gone.
 */

/*
 * Called from process context when we discover that a port has
 * been disconnected.
 */
void usb_disconnect_port(struct usbd_port *up, jdevice_t parent)
{
    usbd_device_handle dev = up->device;
    const char *hubname = j_device_get_nameunit(parent);

    DBG_I(DHOST_SUBR, ("uhub_disconnect: up=%p dev=%p port=%d\n",
        up, dev, up->portno));

#ifdef DIAGNOSTIC
    if (dev == NULL) 
    {
        jprintf("usb_disconnect_port: no device\n");
        return;
    }
#endif

    if (dev->bdev != NULL) 
    {
        jprintf("%s: at %s", j_device_get_nameunit(dev->bdev), hubname);

        if (up->portno != 0)
            jprintf(" port %d", up->portno);

        jprintf(" (addr %d) disconnected\n", dev->address);

        devmon_notify(DM_DEVICE_DETACHING, dev, 0);

        j_device_detach(dev->bdev);
        jdevice_delete(dev->bdev);
        dev->bdev = NULL;

        devmon_notify(DM_DEVICE_DETACHED, dev, 0);
    }

    devmon_notify(DM_DEVICE_DISCONNECTED, dev, 0);

    dev->bus->devices[dev->address] = NULL;
    up->device = NULL;
#ifdef CONFIG_JOTG
    if (up->portno == dev->bus->otg_port)
    {
        jresult_t rc = j_device_ioctl(dev->bus->bdev, 
            DRV_IOCTL_HOST_REMOTE_DISCONNECTED, NULL);
        if (rc)
        {
            DBG_W(DHOST_SUBR, ("Failed to inform OTG Core on device "
                "disconnect - %d\n", rc));
        }        
    }
#endif
    usb_free_device(dev);
}

#ifdef CONFIG_JOTG
static usbd_status usbd_check_support(usbd_device_handle dev, 
    juint8_t usb_class, juint8_t usb_subclass, juint8_t usb_protocol,
    otg_peripheral_state_t *support_state)
{
    periph_support_t periph_support; 
    jresult_t rc;
    usbd_status err = USBD_NORMAL_COMPLETION;

    *support_state = OTG_PERIPH_SUPPORTED;
    
    /* If the Class is HUB or 
     * Class is defined at interface level or
     * The device contains IAD -->
     * Supported at this level */
    if (usb_class == UDCLASS_HUB || usb_class == UDCLASS_IN_INTERFACE ||
       (usb_class == UDCLASS_MISC && usb_subclass == UDSUBCLASS_COMMON &&
       usb_protocol == UDPROTO_IAD))
    {
        goto Exit;
    }
    
    periph_support.dev_details.vendor_id = UGETW(dev->ddesc.idVendor);
    periph_support.dev_details.product_id = UGETW(dev->ddesc.idProduct);
    periph_support.dev_details.bcd_device = UGETW(dev->ddesc.bcdDevice);
    periph_support.dev_details.usb_class = usb_class;
    periph_support.dev_details.usb_sub_class = usb_subclass;
    periph_support.dev_details.usb_protocol = usb_protocol;

    rc = j_device_ioctl(dev->bus->bdev, DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED, 
        &periph_support);
    if (rc)
    {
        DBG_E(DHOST_SUBR, ("usbd_check_device_support : Failed to send IOCTL -"
            " %d\n", rc));
        err = USBD_INVAL;
    }
    else if (!periph_support.is_supported)
        *support_state = OTG_PERIPH_NOT_SUPPORTED;

Exit:
    return err;
}

static usbd_status usbd_check_device_support(usbd_device_handle dev, 
    otg_peripheral_state_t *support_state)
{
    return usbd_check_support(dev, dev->ddesc.bDeviceClass, 
        dev->ddesc.bDeviceSubClass, dev->ddesc.bDeviceProtocol,
        support_state);
}

usbd_status usbd_check_iface_support(usbd_device_handle dev,
    usb_interface_descriptor_t *iface, otg_peripheral_state_t *support_state)
{
    return usbd_check_support(dev, iface->bInterfaceClass, 
        iface->bInterfaceSubClass, iface->bInterfaceProtocol,
        support_state);
}

usbd_status usbd_set_otg_mode(usbd_device_handle dev, juint16_t value)
{
    usb_device_request_t req;
    
    req.bmRequestType = UT_WRITE_DEVICE;
    req.bRequest = UR_SET_FEATURE;
    USETW(req.wValue, value);
    USETW(req.wIndex, 0);
    USETW(req.wLength, 0);
    return (usbd_do_request(dev, &req, 0));
}

static usbd_status usbd_set_otg_support(usbd_device_handle dev, juint8_t port) 
{
    usb_otg_descriptor_t *otg_desc;
    usbd_status err = USBD_NORMAL_COMPLETION;

    /* Get the OTG descriptor from the default configuration descriptor */
    otg_desc = usbd_get_otg_descriptor(dev);

    /* If the connected device is OTG capable let it know if it is connected
     * to the OTG capable port or not */
    if (otg_desc)
    {
        juint16_t otg_mode = ((dev->bus->root_hub == dev->myhub) && 
            (port == dev->bus->otg_port))?
            UOTG_A_HNP_SUPPORT : UOTG_A_ALT_HNP_SUPPORT;

        DBG_I(DHOST_SUBR, ("usbd_new_device: Connected device is OTG "
            "capable. SRP support = %d, HNP support = %d\n", 
            ((otg_desc->bmAttributes & UOTG_SRP)? 1 : 0),
            ((otg_desc->bmAttributes & UOTG_HNP)? 1 : 0)));

        err = usbd_set_otg_mode(dev, otg_mode);
    }

    return err;
}

static usbd_status usbd_notify_dev_support(usbd_device_handle dev, 
    otg_peripheral_state_t support_state)
{
    drv_ioctl_t ioctl;
    jresult_t rc;
    DECLARE_FNAME("usbd_notify_dev_support");

    switch (support_state)
    {
    case OTG_PERIPH_SUPPORTED:
        ioctl = DRV_IOCTL_HOST_REMOTE_CONNECTED; 
        break;
    case OTG_PERIPH_PARTIAL_SUPPORTED:
        ioctl = DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED;
        break;
    case OTG_PERIPH_NOT_SUPPORTED:
        ioctl = DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED;
        break;
    default:
        DBG_E(DHOST_SUBR, ("%s: Unknow peripheral support state - %d\n", fname, 
            support_state));
        goto Error;
    }

    DBG_I(DHOST_SUBR, ("usbd_new_device: Connected device is %ssupported\n",
        ((support_state == OTG_PERIPH_SUPPORTED) ? "" :
        (support_state == OTG_PERIPH_NOT_SUPPORTED)? "not " : "partially ")));

    rc = j_device_ioctl(dev->bus->bdev, ioctl, NULL); 
    if (rc)
        goto Error;

    return (USBD_NORMAL_COMPLETION);

Error:
    return (USBD_INVAL);
}
#endif
