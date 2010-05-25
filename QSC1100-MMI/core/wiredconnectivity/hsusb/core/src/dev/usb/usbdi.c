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

static void usbd_ar_pipe(usbd_pipe_handle pipe);
static void usbd_do_request_async_cb(usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status);
static void usbd_start_next(usbd_pipe_handle pipe);
static usbd_status usbd_open_pipe_ival(usbd_interface_handle iface, 
    juint8_t address, juint8_t flags, usbd_pipe_handle *pipe, jint_t ival);
static jint_t usbd_xfer_isread(usbd_xfer_handle xfer);

static jint_t usbd_xfer_isread(usbd_xfer_handle xfer)
{
    if (xfer->rqflags & URQ_REQUEST)
        return (xfer->request.bmRequestType & UT_READ);
    else
        return (xfer->pipe->endpoint->edesc->bEndpointAddress & UE_DIR_IN);
}

#ifdef USB_DEBUG
void usbd_dump_iface(struct usbd_interface *iface)
{
    jprintf("usbd_dump_iface: iface=%p\n", iface);
    if (iface == NULL)
        return;
    jprintf(" device=%p idesc=%p index=%d altindex=%d priv=%p\n",
        iface->device, iface->idesc, iface->index, iface->altindex,
        iface->priv);
}

void usbd_dump_device(struct usbd_device *dev)
{
    jprintf("usbd_dump_device: dev=%p\n", dev);
    if (dev == NULL)
        return;
    jprintf(" bus=%p default_pipe=%p\n", dev->bus, dev->default_pipe);
    jprintf(" address=%d config=%d depth=%d speed=%d self_powered=%d "
        "power=%d langid=%d\n",
        dev->address, dev->config, dev->depth, dev->speed,
        dev->self_powered, dev->power, dev->langid);
}

void usbd_dump_endpoint(struct usbd_endpoint *endp)
{
    jprintf("usbd_dump_endpoint: endp=%p\n", endp);
    if (endp == NULL)
        return;
    jprintf(" edesc=%p refcnt=%d\n", endp->edesc, endp->refcnt);
    if (endp->edesc)
        jprintf(" bEndpointAddress=0x%02x\n", endp->edesc->bEndpointAddress);
}

void usbd_dump_queue(usbd_pipe_handle pipe)
{
    usbd_xfer_handle xfer;

    jprintf("usbd_dump_queue: pipe=%p\n", pipe);
    TAILQ_FOREACH(xfer, &pipe->queue, next) 
    {
        jprintf("  xfer=%p\n", xfer);
    }
}

void usbd_dump_pipe(usbd_pipe_handle pipe)
{
    jprintf("usbd_dump_pipe: pipe=%p\n", pipe);
    if (pipe == NULL)
        return;
    usbd_dump_iface(pipe->iface);
    usbd_dump_device(pipe->device);
    usbd_dump_endpoint(pipe->endpoint);
    jprintf(" (usbd_dump_pipe:)\n refcnt=%d running=%d aborting=%d\n",
        pipe->refcnt, pipe->running, pipe->aborting);
    jprintf(" intrxfer=%p, repeat=%d, interval=%d\n",
        pipe->intrxfer, pipe->repeat, pipe->interval);
}
#endif

usbd_status usbd_open_pipe(usbd_interface_handle iface, juint8_t  address,
    juint8_t flags, usbd_pipe_handle *pipe)
{
    return (usbd_open_pipe_ival(iface, address, flags, pipe,
        USBD_DEFAULT_INTERVAL));
}

usbd_status usbd_open_pipe_ival(usbd_interface_handle iface, juint8_t  address,
    juint8_t flags, usbd_pipe_handle *pipe, jint_t ival)
{
    usbd_pipe_handle p;
    struct usbd_endpoint *ep;
    usbd_status err;
    jint_t i;

    DBG_V(DHOST_USBDI,("usbd_open_pipe: iface=%p address=0x%x flags=0x%x\n",
        iface, address, flags));

    for (i = 0; i < iface->idesc->bNumEndpoints; i++) 
    {
        ep = &iface->endpoints[i];
        if (ep->edesc == NULL)
            return (USBD_IOERROR);
        if (ep->edesc->bEndpointAddress == address)
            goto found;
    }
    return (USBD_BAD_ADDRESS);
found:
    if ((flags & USBD_EXCLUSIVE_USE) && ep->refcnt != 0)
        return (USBD_IN_USE);
    err = usbd_setup_pipe(iface->device, iface, ep, ival, &p);
    if (err)
        return (err);
    LIST_INSERT_HEAD(&iface->pipes, p, next);
    *pipe = p;
    return (USBD_NORMAL_COMPLETION);
}

usbd_status usbd_open_pipe_intr(usbd_interface_handle iface, juint8_t  address,
    juint8_t flags, usbd_pipe_handle *pipe, usbd_private_handle priv,
    void *buffer, juint32_t len, usbd_callback cb, jint_t ival)
{
    usbd_status err;
    usbd_xfer_handle xfer;
    usbd_pipe_handle ipipe;

    DBG_V(DHOST_USBDI, ("usbd_open_pipe_intr: address=0x%x flags=0x%x "
        "len=%ld\n", address, flags, len));

    err = usbd_open_pipe_ival(iface, address, USBD_EXCLUSIVE_USE,
        &ipipe, ival);
    if (err)
        return (err);
    xfer = usbd_alloc_xfer(iface->device);
    if (xfer == NULL) 
    {
        err = USBD_NOMEM;
        goto bad1;
    }
    usbd_setup_xfer(xfer, ipipe, priv, buffer, len, flags,
        USBD_NO_TIMEOUT, cb);
    ipipe->intrxfer = xfer;
    ipipe->repeat = 1;
    err = usbd_transfer(xfer);
    *pipe = ipipe;
    if (err != USBD_IN_PROGRESS && err)
        goto bad2;
    return (USBD_NORMAL_COMPLETION);

bad2:
    ipipe->intrxfer = NULL;
    ipipe->repeat = 0;
    usbd_free_xfer(xfer);
bad1:
    usbd_close_pipe(ipipe);
    return (err);
}

void usbd_close_pipe(usbd_pipe_handle pipe)
{
#ifdef DIAGNOSTIC
    if (pipe == NULL) 
    {
        jprintf("usbd_close_pipe: pipe==NULL\n");
        return;
    }
#endif

    if (--pipe->refcnt != 0)
        return;
    if (TAILQ_FIRST(&pipe->queue))
    {
        DBG_W(DHOST_USBDI, ("usbd_close_pipe: Trying to clode pipe with "
            "pending requests\n"));
        return;
    }

    LIST_REMOVE(pipe, next);
    pipe->endpoint->refcnt--;
    pipe->methods->close(pipe);
    if (pipe->intrxfer != NULL)
        usbd_free_xfer(pipe->intrxfer);
    jfree(pipe);
}

usbd_status usbd_transfer(usbd_xfer_handle xfer)
{
    usbd_pipe_handle pipe = xfer->pipe;
    jdma_handle *dmap = &xfer->dmabuf;
    usbd_status err;
    jresult_t rc;
    juint32_t size;

    DBG_X(DHOST_USBDI, ("usbd_transfer: xfer=%p, flags=%d, pipe=%p, "
        "running=%d size=%ld\n", xfer, xfer->flags, pipe, pipe->running,
        xfer->length));
#ifdef USB_DEBUG
    if (DBG_GET_LEVEL(DHOST_USBDI) >= DL_EX_VERBOSE)
        usbd_dump_queue(pipe);
#endif
    xfer->done = 0;

    if (pipe->aborting)
        return (USBD_CANCELLED);

    size = xfer->length;
    if (xfer->flags & USBD_DMA_BUF)
    {
        rc = jdma_alloc(0, 0, 0, 0, 0,&xfer->dmabuf);
        if (rc)
            return (USBD_NOMEM);

        dmap = &xfer->dmabuf;

        jdma_dummy_set(size, NULL, xfer->buffer, xfer->dmabuf);
        xfer->flags |= USBD_NO_COPY;
    }
    else if (!(xfer->rqflags & URQ_DEV_DMABUF) && size != 0)
    {
        /* If there is no buffer, allocate one. */
        struct usbd_bus *bus = pipe->device->bus;

#ifdef DIAGNOSTIC
        if (xfer->rqflags & URQ_AUTO_DMABUF)
            jprintf("usbd_transfer: has old buffer!\n");
#endif
        err = bus->methods->allocm(bus, dmap, size);
        if (err)
            return (err);
        xfer->rqflags |= URQ_AUTO_DMABUF;
    }

    /* Copy data if going out. */
    if (!(xfer->flags & USBD_NO_COPY) && size != 0 && !usbd_xfer_isread(xfer))
        j_memcpy(KERNADDR(dmap, 0), xfer->buffer, size);

    if (size && !usbd_xfer_isread(xfer))
        j_cache_flush(jdma_phys_addr(*dmap, 0), size);

    err = pipe->methods->transfer(xfer);

    if (err != USBD_IN_PROGRESS && err) 
    {
        struct usbd_bus *bus = pipe->device->bus;
        /* The transfer has not been queued, so free buffer. */
        if (xfer->rqflags & URQ_AUTO_DMABUF) 
        {
            bus->methods->freem(bus, &xfer->dmabuf);
            xfer->rqflags &= ~URQ_AUTO_DMABUF;
        }
        if (xfer->flags & USBD_DMA_BUF)
        {
            jdma_dummy_set(0, NULL, NULL, xfer->dmabuf);
            jdma_free(xfer->dmabuf);
        }
    }

    if (!(xfer->flags & USBD_SYNCHRONOUS))
        return (err);

    /* Sync transfer, wait for completion. */
    if (err != USBD_IN_PROGRESS)
        return (err);
    if (!xfer->done) 
        jmsleep(xfer->sleep_handle, 0);

    return (xfer->status);
}

/* Like usbd_transfer(), but waits for completion. */
usbd_status usbd_sync_transfer(usbd_xfer_handle xfer)
{
    xfer->flags |= USBD_SYNCHRONOUS;
    return (usbd_transfer(xfer));
}

void *usbd_alloc_buffer(usbd_xfer_handle xfer, juint32_t size)
{
    struct usbd_bus *bus = xfer->device->bus;
    usbd_status err;

    if (xfer->flags & USBD_DMA_BUF)
    {
        DBG_E(DHOST_USBDI, ("usbd_alloc_buffer called with USBD_DMA_BUF"));
        return NULL;
    }

#ifdef DIAGNOSTIC
    if (xfer->rqflags & (URQ_DEV_DMABUF | URQ_AUTO_DMABUF))
        jprintf("usbd_alloc_buffer: xfer already has a buffer\n");
#endif
    err = bus->methods->allocm(bus, &xfer->dmabuf, size);
    if (err)
        return (NULL);
    xfer->rqflags |= URQ_DEV_DMABUF;
    return (KERNADDR(&xfer->dmabuf, 0));
}

void usbd_free_buffer(usbd_xfer_handle xfer)
{
    if (xfer->flags & USBD_DMA_BUF)
    {
        DBG_E(DHOST_USBDI, ("usbd_free_buffer called with USBD_DMA_BUF"));
        return;
    }

#ifdef DIAGNOSTIC
    if (!(xfer->rqflags & (URQ_DEV_DMABUF | URQ_AUTO_DMABUF))) 
    {
        jprintf("usbd_free_buffer: no buffer\n");
        return;
    }
#endif
    xfer->rqflags &= ~(URQ_DEV_DMABUF | URQ_AUTO_DMABUF);
    xfer->device->bus->methods->freem(xfer->device->bus, &xfer->dmabuf);
}

void *usbd_get_buffer(usbd_xfer_handle xfer)
{
    if (xfer->flags & USBD_DMA_BUF)
    {
        DBG_E(DHOST_USBDI, ("usbd_get_buffer called with USBD_DMA_BUF"));
        return NULL;
    }

    if (!(xfer->rqflags & URQ_DEV_DMABUF))
        return (0);
    return (KERNADDR(&xfer->dmabuf, 0));
}

usbd_xfer_handle usbd_alloc_xfer(usbd_device_handle dev)
{
    usbd_xfer_handle xfer;
    jresult_t rc = 0;

    xfer = dev->bus->methods->allocx(dev->bus);
    if (xfer == NULL)
    {
        DBG_E(DHOST_USBDI, ("usbd_alloc_xfer: failed allocate xfer\n"));
        goto Error;
    }

    xfer->device = dev;

    rc = jmsleep_init(&xfer->sleep_handle);
    rc |= jmsleep_init(&xfer->abort_sleep_h);
    if (rc) 
    {
        DBG_E(DHOST_USBDI, ("usbd_alloc_xfer() sleep_handle FAILED\n"));
        goto Error;
    }

    rc = jtask_init(&xfer->timeout_handle, TASK_CORE);
    if (rc)
    {
        DBG_E(DHOST_USBDI, ("usbd_alloc_xfer() jtask_init failed\n"));
        goto Error;
    }

    DBG_X(DHOST_USBDI, ("usbd_alloc_xfer() = %p\n", xfer));
    return (xfer);

Error:
    if (xfer)
    {
        if (xfer->sleep_handle)
            jmsleep_uninit(xfer->sleep_handle);
        if (xfer->abort_sleep_h)
            jmsleep_uninit(xfer->abort_sleep_h);
        
        xfer->device->bus->methods->freex(xfer->device->bus, xfer);
    }
    return NULL;
}

void usbd_free_xfer(usbd_xfer_handle xfer)
{
    DBG_X(DHOST_USBDI, ("usbd_free_xfer: %p\n", xfer));
    if (xfer->rqflags & (URQ_DEV_DMABUF | URQ_AUTO_DMABUF))
    {
        usbd_free_buffer(xfer);
    }
    else if (xfer->rqflags & USBD_DMA_BUF)
    {
        jdma_dummy_set(0, NULL, NULL, xfer->dmabuf);
        jdma_free(xfer->dmabuf);
    }
    jtask_stop(xfer->timeout_handle);
    jtask_uninit(xfer->timeout_handle);
    jmsleep_uninit(xfer->sleep_handle);
    xfer->sleep_handle = NULL;
    jmsleep_uninit(xfer->abort_sleep_h);
    xfer->abort_sleep_h = NULL;
    xfer->device->bus->methods->freex(xfer->device->bus, xfer);
}

void usbd_setup_xfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    usbd_private_handle priv, void *buffer, juint32_t length, juint16_t flags,
    juint32_t timeout, usbd_callback callback)
{
    xfer->pipe = pipe;
    xfer->priv = priv;
    xfer->buffer = buffer;
    xfer->length = length;
    xfer->actlen = 0;
    xfer->flags = flags;
    xfer->timeout = timeout;
    xfer->status = USBD_NOT_STARTED;
    xfer->callback = callback;
    xfer->rqflags &= ~URQ_REQUEST;
    xfer->nframes = 0;
}

void usbd_setup_default_xfer(usbd_xfer_handle xfer, usbd_device_handle dev,
    usbd_private_handle priv, juint32_t timeout, usb_device_request_t *req, 
    void *buffer, juint32_t length, juint16_t flags, usbd_callback callback)
{
    xfer->pipe = dev->default_pipe;
    xfer->priv = priv;
    xfer->buffer = buffer;
    xfer->length = length;
    xfer->actlen = 0;
    xfer->flags = flags;
    xfer->timeout = timeout;
    xfer->status = USBD_NOT_STARTED;
    xfer->callback = callback;

#ifdef FEATURE_HSU_EBI
#error code not present
#else
    xfer->request = *req;
#endif

    xfer->rqflags |= URQ_REQUEST;
    xfer->nframes = 0;
}

void usbd_setup_isoc_xfer(usbd_xfer_handle xfer, usbd_pipe_handle pipe,
    usbd_private_handle priv, juint16_t *frlengths, juint32_t nframes, 
    juint16_t flags, usbd_callback callback)
{
    xfer->pipe = pipe;
    xfer->priv = priv;
    xfer->buffer = 0;
    xfer->length = 0;
    xfer->actlen = 0;
    xfer->flags = flags;
    xfer->timeout = USBD_NO_TIMEOUT;
    xfer->status = USBD_NOT_STARTED;
    xfer->callback = callback;
    xfer->rqflags &= ~URQ_REQUEST;
    xfer->frlengths = frlengths;
    xfer->nframes = nframes;
}

void usbd_get_xfer_status(usbd_xfer_handle xfer, usbd_private_handle *priv,
    void **buffer, juint32_t *count, usbd_status *status)
{
    if (priv != NULL)
        *priv = xfer->priv;
    if (buffer != NULL)
        *buffer = xfer->buffer;
    if (count != NULL)
        *count = xfer->actlen;
    if (status != NULL)
        *status = xfer->status;
}

usb_config_descriptor_t *usbd_get_config_descriptor(usbd_device_handle dev)
{
#ifdef DIAGNOSTIC
    if (dev == NULL) 
    {
        jprintf("usbd_get_config_descriptor: dev == NULL\n");
        return (NULL);
    }
#endif
    return (dev->cdesc);
}

usb_interface_descriptor_t *usbd_get_interface_descriptor(
    usbd_interface_handle iface)
{
#ifdef DIAGNOSTIC
    if (iface == NULL) 
    {
        jprintf("usbd_get_interface_descriptor: dev == NULL\n");
        return (NULL);
    }
#endif
    return (iface->idesc);
}

usb_device_descriptor_t *usbd_get_device_descriptor(usbd_device_handle dev)
{
    return (&dev->ddesc);
}

usb_endpoint_descriptor_t *usbd_interface2endpoint_descriptor(
    usbd_interface_handle iface, juint8_t  address)
{
    if (address >= iface->idesc->bNumEndpoints)
        return (0);
    return (iface->endpoints[address].edesc);
}

void usbd_abort_pipe(usbd_pipe_handle pipe)
{
#ifdef DIAGNOSTIC
    if (pipe == NULL) 
    {
        jprintf("usbd_close_pipe: pipe==NULL\n");
        return;
    }
#endif
    usbd_ar_pipe(pipe);
}

usbd_status usbd_clear_endpoint_stall(usbd_pipe_handle pipe)
{
    usbd_device_handle dev = pipe->device;
    usb_device_request_t req;
    usbd_status err;

    DBG_X(DHOST_USBDI, ("usbd_clear_endpoint_stall\n"));

    /*
     * Clearing en endpoint stall resets the endpoint toggle, so
     * do the same to the HC toggle.
     */
    pipe->methods->cleartoggle(pipe);

    req.bmRequestType = UT_WRITE_ENDPOINT;
    req.bRequest = UR_CLEAR_FEATURE;
    USETW(req.wValue, UF_ENDPOINT_HALT);
    USETW(req.wIndex, pipe->endpoint->edesc->bEndpointAddress);
    USETW(req.wLength, 0);
    err = usbd_do_request(dev, &req, 0);
    return (err);
}

usbd_status usbd_clear_endpoint_stall_async(usbd_pipe_handle pipe)
{
    usbd_device_handle dev = pipe->device;
    usb_device_request_t req;
    usbd_status err;

    pipe->methods->cleartoggle(pipe);

    req.bmRequestType = UT_WRITE_ENDPOINT;
    req.bRequest = UR_CLEAR_FEATURE;
    USETW(req.wValue, UF_ENDPOINT_HALT);
    USETW(req.wIndex, pipe->endpoint->edesc->bEndpointAddress);
    USETW(req.wLength, 0);
    err = usbd_do_request_async(dev, &req, 0);
    return (err);
}

void usbd_clear_endpoint_toggle(usbd_pipe_handle pipe)
{
    pipe->methods->cleartoggle(pipe);
}

usbd_status usbd_endpoint_count(usbd_interface_handle iface, juint8_t *count)
{
    if (iface == NULL || iface->idesc == NULL) 
    {
        jprintf("usbd_endpoint_count: NULL pointer\n");
        return (USBD_INVAL);
    }

    *count = iface->idesc->bNumEndpoints;
    return (USBD_NORMAL_COMPLETION);
}

usbd_status usbd_interface_count(usbd_device_handle dev, juint8_t *count)
{
    if (dev->cdesc == NULL)
        return (USBD_NOT_CONFIGURED);
    *count = dev->cdesc->bNumInterface;
    return (USBD_NORMAL_COMPLETION);
}

void usbd_interface2device_handle(usbd_interface_handle iface,
    usbd_device_handle *dev)
{
    *dev = iface->device;
}

usbd_status usbd_device2interface_handle(usbd_device_handle dev,
    juint8_t ifaceno, usbd_interface_handle *iface)
{
    if (dev->cdesc == NULL)
        return (USBD_NOT_CONFIGURED);
    if (ifaceno >= dev->cdesc->bNumInterface)
        return (USBD_INVAL);
    *iface = &dev->ifaces[ifaceno];
    return (USBD_NORMAL_COMPLETION);
}

usbd_device_handle usbd_pipe2device_handle(usbd_pipe_handle pipe)
{
    return (pipe->device);
}

usbd_status usbd_set_interface(usbd_interface_handle iface, jint_t altidx)
{
    usb_device_request_t req;
    usbd_status err;
    void *endpoints;

    if (LIST_FIRST(&iface->pipes) != 0)
        return (USBD_IN_USE);

    endpoints = iface->endpoints;
    err = usbd_fill_iface_data(iface->device, iface->index, altidx);
    if (err)
        return (err);

    /* new setting works, we can free old endpoints */
    if (endpoints != NULL)
        jfree(endpoints);

#ifdef DIAGNOSTIC
    if (iface->idesc == NULL) 
    {
        jprintf("usbd_set_interface: NULL pointer\n");
        return (USBD_INVAL);
    }
#endif

    req.bmRequestType = UT_WRITE_INTERFACE;
    req.bRequest = UR_SET_INTERFACE;
    USETW(req.wValue, iface->idesc->bAlternateSetting);
    USETW(req.wIndex, iface->idesc->bInterfaceNumber);
    USETW(req.wLength, 0);
    return (usbd_do_request(iface->device, &req, 0));
}

jint_t usbd_get_no_alts(usb_config_descriptor_t *cdesc, jint_t ifaceno)
{
    char *p = (char *)cdesc;
    char *end = p + UGETW(cdesc->wTotalLength);
    usb_interface_descriptor_t *d;
    jint_t n;

    for (n = 0; p < end; p += d->bLength) 
    {
        d = (usb_interface_descriptor_t *)p;
        if (p + d->bLength <= end && d->bDescriptorType == UDESC_INTERFACE &&
            d->bInterfaceNumber == ifaceno)
        {
            n++;
        }
    }
    return (n);
}

jint_t usbd_get_interface_altindex(usbd_interface_handle iface)
{
    return (iface->altindex);
}

usbd_status usbd_get_interface(usbd_interface_handle iface, juint8_t  *aiface)
{
    usb_device_request_t req;

    req.bmRequestType = UT_READ_INTERFACE;
    req.bRequest = UR_GET_INTERFACE;
    USETW(req.wValue, 0);
    USETW(req.wIndex, iface->idesc->bInterfaceNumber);
    USETW(req.wLength, 1);
    return (usbd_do_request(iface->device, &req, aiface));
}

#ifdef CONFIG_JOTG
usb_otg_descriptor_t *usbd_get_otg_descriptor(usbd_device_handle dev)
{
    char *p = (char *)dev->default_cdesc;
    char *end; 
    usb_otg_descriptor_t *d = NULL;

    if (!p)
        goto Exit;

    end = p + UGETW(dev->default_cdesc->wTotalLength);

    for (; p < end; p += d->bLength) 
    {
        d = (usb_otg_descriptor_t *)p;
        if (p + d->bLength <= end && d->bDescriptorType == UDESC_OTG)
            break;
    }

    if (p >= end)
        d = NULL;
Exit:
    return d;
}
#endif

void usbd_abort_request(usbd_xfer_handle xfer)
{
    usbd_pipe_handle pipe = xfer->pipe;
    jbool_t found = 0;
    
    if (!xfer->rqflags & URQ_REQUEST)
    {
        DBG_E(DHOST_USBDI, ("usbd_abort_request: Trying to abort xfer that "
            "is not a control one\n"));
        return;
    }

    DBG_I(DHOST_USBDI, ("usbd_abort_request: pipe = %x xfer = %x status %s\n",
        pipe, xfer, usbd_errstr(xfer->status)));

    /* Check if its the active xfer */
    if (xfer == TAILQ_FIRST(&pipe->queue) && pipe->running)
    {
        DBG_I(DHOST_USBDI, ("usbd_abort_request: Abort active xfer %x\n",
            xfer));

        /* Ask the controller to abort */
        xfer->status = USBD_CANCELLED;
        pipe->methods->abort(xfer);

        return;
    } 

    /* Check if just on queue */
    TAILQ_FIND(&pipe->queue, xfer, usbd_xfer_handle, next, found);

    if (found)
    {
        DBG_I(DHOST_USBDI, ("usbd_abort_request: Abort pending request\n"));

        xfer->status = USBD_CANCELLED;

        /* Call completion cb */
        usb_transfer_complete(xfer);
    }
    else
    {
       DBG_W(DHOST_USBDI, ("usbd_abort_request: Cannot abort, "
            "xfer not active or queued: %d\n", xfer->status));
    }
}

/*** Internal routines ***/

/* Dequeue all pipe operations */
static void usbd_ar_pipe(usbd_pipe_handle pipe)
{
    usbd_xfer_handle xfer;

    DBG_V(DHOST_USBDI, ("usbd_ar_pipe: pipe=%p\n", pipe));
#ifdef USB_DEBUG
    if (DBG_GET_LEVEL(DHOST_USBDI) >= DL_EX_VERBOSE)
        usbd_dump_queue(pipe);
#endif
    pipe->repeat = 0;
    pipe->aborting = 1;
    while ((xfer = TAILQ_FIRST(&pipe->queue)) != NULL) 
    {
        DBG_V(DHOST_USBDI, ("usbd_ar_pipe: pipe=%p xfer=%p (methods=%p)\n",
            pipe, xfer, pipe->methods));
            /* Make the HC abort it (and invoke the callback). */
            pipe->methods->abort(xfer);
    }
    pipe->aborting = 0;
}

void usb_transfer_complete(usbd_xfer_handle xfer)
{
    usbd_pipe_handle pipe = xfer->pipe;
    jdma_handle *dmap = &xfer->dmabuf;
    jint_t sync = xfer->flags & USBD_SYNCHRONOUS;
    jint_t erred = xfer->status == USBD_CANCELLED || 
        xfer->status == USBD_TIMEOUT;
    jint_t repeat = pipe->repeat;
    jint_t start_next = 1;


    DBG_X(DHOST_USBDI, ("usb_transfer_complete: pipe=%p xfer=%p status=%d "
        "actlen=%ld\n", pipe, xfer, xfer->status, xfer->actlen));
#ifdef DIAGNOSTIC
    if (xfer->busy_free != XFER_ONQU) 
    {
        jprintf("usb_transfer_complete: xfer=%p not busy 0x%08x\n",
            xfer, xfer->busy_free);
        return;
    }

    if (pipe == NULL) 
    {
        jprintf("usbd_transfer_cb: pipe==0, xfer=%p\n", xfer);
        return;
    }
#endif
    if (xfer->actlen && usbd_xfer_isread(xfer))
        j_cache_invalidate(jdma_phys_addr(*dmap, 0), xfer->actlen);

    if (!(xfer->flags & USBD_NO_COPY) && xfer->actlen != 0 &&
        usbd_xfer_isread(xfer)) 
    {
        if (xfer->actlen > xfer->length) 
        {
            j_panic("usb_transfer_complete: actlen > len %ld > %ld\n",
                xfer->actlen, xfer->length);
            xfer->actlen = xfer->length;
        }
        j_memcpy(xfer->buffer, KERNADDR(dmap, 0), xfer->actlen);
    }

    if (xfer->status == USBD_IOERROR && repeat)
        repeat = pipe->repeat = 0; /* Stop reposting on error */

    /* if we allocated the buffer in usbd_transfer() we free it here. */
    if (!repeat) 
    {
        struct usbd_bus *bus = pipe->device->bus;

        if (xfer->rqflags & URQ_AUTO_DMABUF) 
        {
            bus->methods->freem(bus, dmap);
            xfer->rqflags &= ~URQ_AUTO_DMABUF;
        }

        if (xfer->flags & USBD_DMA_BUF)
        {
            jdma_dummy_set(0, NULL, NULL, xfer->dmabuf);
            jdma_free(xfer->dmabuf);
        }
    }

    if (!repeat)
    {
        KASSERT(xfer == TAILQ_FIRST(&pipe->queue) || pipe->iface == NULL,
            ("usb_transfer_complete: bad dequeue %p != %p\n",
            xfer, TAILQ_FIRST(&pipe->queue)));
        
        /* Control transfers may be canceled out of order */
        if (xfer != TAILQ_FIRST(&pipe->queue))
            start_next = 0;

        TAILQ_REMOVE(&pipe->queue, xfer, next);
    }
    DBG_X(DHOST_USBDI,("usb_transfer_complete: repeat=%d new head=%p\n",
        repeat, TAILQ_FIRST(&pipe->queue)));

    xfer->done = 1;
    if (!xfer->status && xfer->actlen < xfer->length &&
        !(xfer->flags & USBD_SHORT_XFER_OK)) 
    {
        DBG_W(DHOST_USBDI, ("usbd_transfer_cb: short transfer "
            "%ld<%ld\n", xfer->actlen, xfer->length));
        xfer->status = USBD_SHORT_XFER;
    }

    if (xfer->callback)
        xfer->callback(xfer, xfer->priv, xfer->status);

#ifdef DIAGNOSTIC
    if (pipe->methods->done != NULL)
        pipe->methods->done(xfer);
    else
        jprintf("usb_transfer_complete: pipe->methods->done == NULL\n");
#else
    pipe->methods->done(xfer);
#endif

    if (sync)
        jwakeup(xfer->sleep_handle);

#ifdef SINGLE_THREADED
    /* We must let the pipe drain naturally even if there are errors */
    erred = 0;
#endif
    if (!repeat) 
    {
        if (erred && pipe->iface != NULL)       /* not control pipe */
            pipe->running = 0;
        else if (start_next)
            usbd_start_next(pipe);
    }
}

#ifdef CONFIG_JOTG
static jbool_t usbd_is_pipe_enabled(usbd_pipe_handle pipe)
{
    usbd_bus_handle bus = pipe->device->bus;
    usbd_device_handle root_hub = pipe->device->bus->root_hub;
    usbd_device_handle dev; 
    juint8_t otg_port = pipe->device->bus->otg_port;
    jbool_t is_enabled = 1;

    /* If there is no OTG port then the pipe is always enabled */
    if (!otg_port)
        goto Exit;

    KASSERT(bus->methods->is_otg_host_enabled, ("Controller is part of OTG and "
        "is_otg_host_enabled callback is not implemented\n"));

    /* If part of OTG and Host enabled then pipe is always enabled */
    if (bus->methods->is_otg_host_enabled(bus))
        goto Exit;

    /* If root_hub == 0 it means that we are attaching it */
    if (!root_hub || pipe->device == root_hub)
        goto Exit;

    /* We need to figure out if the device is connected to the OTG port
     * (directly or through hubs) */

    /* Go through the device tree and find the device/hub connected to the root
     * hub */
    for(dev = pipe->device; dev->myhub != root_hub; dev = dev->myhub)
        ;
    /* If the device is connected to the OTG port then the pipe is disabled */
    if (root_hub->hub->ports[otg_port - 1].device == dev)
        is_enabled = 0;

Exit:

    DBG_X(DHCD_TRANSFER, ("ehci_is_pipe_enabled: pipe 0x%x is %s\n", pipe,
        ((is_enabled)? "enabled" : "disabled")));

    return is_enabled;    
}
#endif

usbd_status usb_insert_transfer(usbd_xfer_handle xfer)
{
    usbd_pipe_handle pipe = xfer->pipe;
    usbd_bus_handle bus = pipe->device->bus;
    usbd_status err;

    if (!bus->methods->is_active(bus))
        return (USBD_CANCELLED);

#ifdef CONFIG_JOTG
    if (!usbd_is_pipe_enabled(pipe))
        return USBD_CANCELLED;
#endif

    DBG_X(DHOST_USBDI,("usb_insert_transfer: pipe=%p running=%d "
        "timeout=%ld\n", pipe, pipe->running, xfer->timeout));
#ifdef DIAGNOSTIC
    if (xfer->busy_free != XFER_BUSY) 
    {
        jprintf("usb_insert_transfer: xfer=%p not busy 0x%08x\n",
            xfer, xfer->busy_free);
        return (USBD_INVAL);
    }
    xfer->busy_free = XFER_ONQU;
#endif
    TAILQ_INSERT_TAIL(&pipe->queue, xfer, next);

    if (pipe->running)
        err = USBD_IN_PROGRESS;
    else 
    {
        pipe->running = 1;
        err = USBD_NORMAL_COMPLETION;
    }
    return (err);
}

void usbd_start_next(usbd_pipe_handle pipe)
{
    usbd_xfer_handle xfer;
    usbd_status err;

#ifdef DIAGNOSTIC
    if (pipe == NULL) 
    {
        jprintf("usbd_start_next: pipe == NULL\n");
        return;
    }
    if (pipe->methods == NULL || pipe->methods->start == NULL) 
    {
        jprintf("usbd_start_next: pipe=%p no start method\n", pipe);
        return;
    }
#endif

    /* Get next request in queue. */
    xfer = TAILQ_FIRST(&pipe->queue);
    DBG_X(DHOST_USBDI, ("usbd_start_next: pipe=%p, xfer=%p\n", pipe, xfer));
    if (xfer == NULL) 
    {
        pipe->running = 0;
    }
    else
    {
        err = pipe->methods->start(xfer);
        if (err != USBD_IN_PROGRESS) 
        {
            jprintf("usbd_start_next: error=%d\n", err);
            pipe->running = 0;
        }
    }
}

usbd_status usbd_do_request(usbd_device_handle dev, usb_device_request_t *req,
    void *data)
{
    return (usbd_do_request_flags(dev, req, data, 0, 0,
        USBD_DEFAULT_TIMEOUT));
}

usbd_status usbd_do_request_flags(usbd_device_handle dev, 
    usb_device_request_t *req, void *data, juint16_t flags, juint32_t *actlen, 
    juint32_t timeout)
{
    return (usbd_do_request_flags_pipe(dev, dev->default_pipe, req,
        data, flags, actlen, timeout));
}

usbd_status usbd_do_request_flags_pipe(usbd_device_handle dev, 
    usbd_pipe_handle pipe, usb_device_request_t *req, void *data, 
    juint16_t flags, juint32_t *actlen, juint32_t timeout)
{
    usbd_xfer_handle xfer;
    usbd_status err;

#ifdef DIAGNOSTIC
    if (dev->bus->intr_context) 
    {
        jprintf("usbd_do_request: not in process context\n");
        return (USBD_INVAL);
    }
#endif
    dev->ctrl_xfer_count++;
    xfer = usbd_alloc_xfer(dev);
    if (xfer == NULL)
    {
        err = USBD_NOMEM;
        goto bad;
    }
    usbd_setup_default_xfer(xfer, dev, 0, timeout, req, data,
        UGETW(req->wLength), flags, 0);
    xfer->pipe = pipe;
    err = usbd_sync_transfer(xfer);
#if defined(USB_DEBUG) || defined(DIAGNOSTIC)
    if (xfer->actlen > xfer->length)
    {
        DBG_I(DHOST_USBDI, ("usbd_do_request: overrun addr=%d type=0x%02x "
            "req=0x %02x val=%d index=%d rlen=%d length=%ld actlen=%ld\n",
            dev->address, xfer->request.bmRequestType,
            xfer->request.bRequest, UGETW(xfer->request.wValue),
            UGETW(xfer->request.wIndex),
            UGETW(xfer->request.wLength),
            xfer->length, xfer->actlen));
    }
#endif
    if (actlen != NULL)
        *actlen = xfer->actlen;
    if (err == USBD_STALLED)
    {
        /*
         * The control endpoint has stalled.  Control endpoints
         * should not halt, but some may do so anyway so clear
         * any halt condition.
         */
        usb_device_request_t treq;
        usb_status_t status;
        juint16_t s;
        usbd_status nerr;

        treq.bmRequestType = UT_READ_ENDPOINT;
        treq.bRequest = UR_GET_STATUS;
        USETW(treq.wValue, 0);
        USETW(treq.wIndex, 0);
        USETW(treq.wLength, USB_STATUS_SIZE);
        usbd_setup_default_xfer(xfer, dev, 0, USBD_DEFAULT_TIMEOUT, &treq, 
            (void *)&status, USB_STATUS_SIZE, 0, 0);
        nerr = usbd_sync_transfer(xfer);
        if (nerr)
            goto bad;
        s = UGETW(status.wStatus);
        DBG_V(DHOST_USBDI, ("usbd_do_request: status = 0x%04x\n", s));
        if (!(s & UES_HALT))
            goto bad;
        treq.bmRequestType = UT_WRITE_ENDPOINT;
        treq.bRequest = UR_CLEAR_FEATURE;
        USETW(treq.wValue, UF_ENDPOINT_HALT);
        USETW(treq.wIndex, 0);
        USETW(treq.wLength, 0);
        usbd_setup_default_xfer(xfer, dev, 0, USBD_DEFAULT_TIMEOUT, &treq,
            (void *)&status, 0, 0, 0);
        nerr = usbd_sync_transfer(xfer);
        if (nerr)
            goto bad;
    }

bad:
    if (xfer)
        usbd_free_xfer(xfer);
    dev->ctrl_xfer_count--;
    return (err);
}

void usbd_do_request_async_cb(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status status)
{
#if defined(USB_DEBUG) || defined(DIAGNOSTIC)
    if (xfer->actlen > xfer->length)
        DBG_W(DHOST_USBDI, ("usbd_do_request: overrun addr=%d type=0x%02x "
            "req=0x %02x val=%d index=%d rlen=%d length=%ld actlen=%ld\n",
            xfer->pipe->device->address,
            xfer->request.bmRequestType,
            xfer->request.bRequest, UGETW(xfer->request.wValue),
            UGETW(xfer->request.wIndex),
            UGETW(xfer->request.wLength),
            xfer->length, xfer->actlen));
#endif
    UNUSED_VAR(status);
    UNUSED_VAR(priv);
    usbd_free_xfer(xfer);
}

/*
 * Execute a request without waiting for completion.
 * Can be used from interrupt context.
 */
usbd_status usbd_do_request_async(usbd_device_handle dev, 
    usb_device_request_t *req, void *data)
{
    usbd_xfer_handle xfer;
    usbd_status err;

    xfer = usbd_alloc_xfer(dev);
    if (xfer == NULL)
        return (USBD_NOMEM);
    usbd_setup_default_xfer(xfer, dev, 0, USBD_DEFAULT_TIMEOUT, req,
        data, UGETW(req->wLength), 0, usbd_do_request_async_cb);
    err = usbd_transfer(xfer);
    if (err != USBD_IN_PROGRESS && err) 
    {
        usbd_free_xfer(xfer);
        return (err);
    }
    return (USBD_NORMAL_COMPLETION);
}

const struct usbd_quirks *usbd_get_quirks(usbd_device_handle dev)
{
#ifdef DIAGNOSTIC
    if (dev == NULL) 
    {
        jprintf("usbd_get_quirks: dev == NULL\n");
        return 0;
    }
#endif
    return (dev->quirks);
}

usb_endpoint_descriptor_t *usbd_get_endpoint_descriptor(
    usbd_interface_handle iface, juint8_t  address)
{
    struct usbd_endpoint *ep;
    jint_t i;

    for (i = 0; i < iface->idesc->bNumEndpoints; i++) 
    {
        ep = &iface->endpoints[i];
        if (ep->edesc->bEndpointAddress == address)
            return (iface->endpoints[i].edesc);
    }
    return (0);
}

jmsleep_h usbd_xfer_get_sleep_h(usbd_xfer_handle xfer)
{
    return xfer->sleep_handle;
}

