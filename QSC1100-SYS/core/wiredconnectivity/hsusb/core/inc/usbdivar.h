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

#ifndef _USBDIVAR_H_
#define _USBDIVAR_H_

#include "root_hub.h"
#if defined CONFIG_USB_TESTING
#include "usbdi_test_internal.h"
#endif

#ifdef CONFIG_JOTG
#include <uw_otg_app.h>
#endif

struct usbd_xfer;
struct usbd_pipe;

struct usbd_endpoint {
    usb_endpoint_descriptor_t *edesc;
    jint_t                       refcnt;
};

struct usbd_bus_methods {
    usbd_status       (*open_pipe)(struct usbd_pipe *pipe);
    usbd_status       (*allocm)(struct usbd_bus *bus, jdma_handle *dma,
        juint32_t size);
    void              (*freem)(struct usbd_bus *bus, jdma_handle *dma);

    struct usbd_xfer *(*allocx)(struct usbd_bus *bus);
    void              (*freex)(struct usbd_bus *bus, struct usbd_xfer *xfer);
    jbool_t            (*is_active)(struct usbd_bus *bus);
    jresult_t          (*suspend)(struct usbd_bus *bus);
    jresult_t          (*resume)(struct usbd_bus *bus);
#ifdef CONFIG_JOTG
    jbool_t            (*is_otg_host_enabled)(struct usbd_bus *bus);
#endif
};

struct usbd_pipe_methods {
    usbd_status       (*transfer)(usbd_xfer_handle xfer);
    usbd_status       (*start)(usbd_xfer_handle xfer);
    void              (*abort)(usbd_xfer_handle xfer);
    void              (*close)(usbd_pipe_handle pipe);
    void              (*cleartoggle)(usbd_pipe_handle pipe);
    void              (*done)(usbd_xfer_handle xfer);
};

struct usbd_port {
    usb_port_status_t status;
    juint16_t          power;    /* mA of current on port */
    juint8_t           portno;
    juint8_t           restartcnt;
#define USBD_RESTART_MAX 5
    struct usbd_device *device;/* Connected device */
    struct usbd_device *parent;/* The ports hub */
#define USBD_PORT_SOFT_SUSPENDED     0x01
#define USBD_PORT_USB_SUSPENDED      0x02
    juint8_t           flags;
#if defined CONFIG_USB_TESTING
    struct usb_test_cb test_cb;
#endif
#ifdef CONFIG_PORT_INTERCHIP
    jbool_t is_interchip;
#endif
};

struct usbd_hub {
    usbd_status         (*explore)(usbd_device_handle hub, jbool_t find_new);
    void                *hubsoftc;
    usb_hub_descriptor_t        hubdesc;
    struct usbd_port    ports[1];
};

struct usb_softc;

/*****/

struct usbd_bus {
    /* Filled by HC driver */
    jdevice_t                    bdev; /* base device, host adapter */
    void                        *usb_wrap;
    struct usbd_bus_methods     *methods;
    root_hub_softc_t            rootsoftc;
    root_hub_desc_t             root_desc; /* Root hub descriptor */
    juint32_t                    pipe_size; /* size of a pipe struct */

#ifdef CONFIG_JOTG
    juint8_t                     otg_port;
#endif
    
    /* Filled by usb driver */
    struct usbd_device          *root_hub;
    usbd_device_handle          devices[USB_MAX_DEVICES];
    struct usb_softc            *usbctl;
    juint32_t                    intr_context;
    juint_t                      no_intrs;
    jint_t                       usbrev; /* USB revision */
#define USBREV_UNKNOWN  0
#define USBREV_PRE_1_0  1
#define USBREV_1_0      2
#define USBREV_1_1      3
#define USBREV_2_0      4
#define USBREV_STR { "unknown", "pre 1.0", "1.0", "1.1", "2.0" }

    jtask_h                     explore_task;
#define USBD_BUS_READY_4_EXPLORE     0x01
#define USBD_BUS_NEED_EXPLORE        0x02
#define USBD_BUS_IGNORE_NEW_DEVICES  0x04
    juint8_t                     explore_flags;
};

struct usbd_device {
    struct usbd_bus    *bus;           /* our controller */
    struct usbd_pipe   *default_pipe;  /* pipe 0 */
    juint8_t             address;       /* device address */
    juint8_t             config;        /* current configuration # */
    juint8_t             depth;         /* distance from root hub */
    juint8_t             speed;         /* low/full/high speed */
    juint8_t             self_powered;  /* flag for self powered */
    juint16_t            power;         /* mA the device uses */
    jint16_t             langid;        /* language for strings */
#define USBD_NOLANG (-1)
    struct usbd_port   *powersrc;      /* upstream hub port, or 0 */
    struct usbd_device *myhub;         /* upstream hub */
    struct usbd_device *myhighhub;     /* closest high speed hub */
    juint8_t             myhighport;    /* port in high speed hub */
    struct usbd_endpoint def_ep;       /* for pipe 0 */
    usb_endpoint_descriptor_t def_ep_desc; /* for pipe 0 */
    struct usbd_interface  *ifaces;    /* array of all interfaces */
    usb_device_descriptor_t ddesc;     /* device descriptor */
    usb_config_descriptor_t *cdesc;    /* full config descriptor */
    usb_config_descriptor_t *default_cdesc; /* Default configuration descr*/
    const struct usbd_quirks *quirks;  /* device quirks, always set */
    struct usbd_hub    *hub;           /* only if this is a hub */
    jdevice_t           bdev;           /* base device handle */
    juint_t             ctrl_xfer_count; 
#ifdef CONFIG_JOTG
   otg_peripheral_state_t otg_support; 
#endif
};

struct usbd_interface {
    struct usbd_device         *device;
    usb_interface_descriptor_t *idesc;
    jint_t                       index;
    jint_t                       altindex;
    struct usbd_endpoint       *endpoints;
    void                       *priv;
    LIST_HEAD(usbdi_pipes, usbd_pipe) pipes;
#ifdef CONFIG_JHOST_AUDIO
    struct {
        jbool_t                  used;
        juint8_t                 head;
    } collection;
#endif
};

struct usbd_pipe {
    struct usbd_interface  *iface;
    struct usbd_device     *device;
    struct usbd_endpoint   *endpoint;
    juint32_t                refcnt;
    char                    running;
    char                    aborting;
    TAILQ_HEAD(usbdi_xfer_queue, usbd_xfer) queue;
    LIST_ENTRY(usbd_pipe)   next;

    usbd_xfer_handle        intrxfer; /* used for repeating requests */
    char                    repeat;
    jint_t                   interval;

    /* Filled by HC driver. */
    struct usbd_pipe_methods *methods;
};

struct usbd_xfer {
	struct usbd_pipe           *pipe;
    void                       *priv;
    void                       *buffer;
    juint32_t                    length;
    juint32_t                    actlen;
    juint16_t                    flags;
    juint32_t                    timeout;
    usbd_status                 status;
    usbd_callback               callback;
#ifdef DIAGNOSTIC
    juint32_t                    busy_free;
#define XFER_FREE 0x46524545
#define XFER_BUSY 0x42555359
#define XFER_ONQU 0x4f4e5155
#endif
	/* For control pipe */
    usb_device_request_t        request;

    /* For isoc */
    juint16_t                    *frlengths;
    juint32_t                    nframes;

    /* For memory allocation */
    struct usbd_device          *device;
    jdma_handle                 dmabuf;
    __VOLATILE__ char           done;
    juint8_t                     rqflags;
#define URQ_REQUEST     0x01
#define URQ_AUTO_DMABUF 0x10
#define URQ_DEV_DMABUF  0x20

    TAILQ_ENTRY(usbd_xfer)     next;

    void                       *hcpriv; /* private use by the HC driver */
    juint8_t                     hcflags; /* private use by the HC driver */
#define UXFER_ABORTING  0x01    /* xfer is aborting. */
#define UXFER_ABORTWAIT 0x02    /* abort completion is being awaited. */

    jtask_h                    timeout_handle;
    jmsleep_h                  sleep_handle;
    jmsleep_h                  abort_sleep_h;
};

#ifdef USB_DEBUG
void usbd_dump_iface(usbd_interface_handle iface);
void usbd_dump_device(usbd_device_handle dev);
void usbd_dump_endpoint(struct usbd_endpoint *endp);
void usbd_dump_queue(usbd_pipe_handle pipe);
void usbd_dump_pipe(usbd_pipe_handle pipe);
#endif

/* Routines from usb_subr.c */
usbd_status     usbd_setup_pipe(usbd_device_handle dev,
    usbd_interface_handle iface, struct usbd_endpoint *ep, jint_t ival,
    usbd_pipe_handle *pipe);
usbd_status     usbd_new_device(jdevice_t parent,
    usbd_bus_handle bus, juint8_t depth, juint8_t lowspeed, juint8_t port,
    struct usbd_port *up);
void            usbd_remove_device(usbd_device_handle, struct usbd_port *up);
usbd_status     usbd_fill_iface_data(usbd_device_handle dev, jint_t ifaceidx, 
    jint_t altindex);
void            usb_free_device(usbd_device_handle dev);

usbd_status     usb_insert_transfer(usbd_xfer_handle xfer);
void            usb_transfer_complete(usbd_xfer_handle xfer);
void            usb_disconnect_port(struct usbd_port *up, jdevice_t parent);
juint8_t         usbd_getnewaddr(usbd_bus_handle bus);
usbd_device_handle usbd_create_device(jdevice_t parent, usbd_bus_handle bus,
    juint8_t depth, juint8_t speed, juint8_t port, struct usbd_port *up);
usbd_status     usbd_set_config(usbd_device_handle dev, juint16_t conf);
usbd_status     usb_device_suspend(usbd_device_handle dev);
usbd_status     usb_device_resume(usbd_device_handle dev);

/* Routines from usb.c */
void            usb_needs_explore(usbd_device_handle dev);
jresult_t        controller_usb_attach(jdevice_t device, void **handle);
jresult_t        controller_usb_detach(void *handle);
jresult_t        controller_usb_suspend(void *handle);
jresult_t        controller_usb_resume(void *handle);
jresult_t        controller_usb_suspend_self(void *handle);
jresult_t        controller_usb_resume_self(void *handle);

/* Locator stuff. */

#define UHUB_UNK_PORT           -1 /* wildcarded 'port' */
#define UHUB_UNK_CONFIGURATION  -1 /* wildcarded 'configuration' */
#define UHUB_UNK_INTERFACE      -1 /* wildcarded 'interface' */
#define UHUB_UNK_VENDOR         -1 /* wildcarded 'vendor' */
#define UHUB_UNK_PRODUCT        -1 /* wildcarded 'product' */
#define UHUB_UNK_RELEASE        -1 /* wildcarded 'release' */

#endif

