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

#define UHUB_INTR_INTERVAL 255  /* ms */

struct uhub_softc {
    jdevice_t            sc_dev;         /* base device */
    usbd_device_handle  sc_hub;         /* USB device */
    usbd_pipe_handle    sc_ipipe;       /* interrupt pipe */
    juint8_t             sc_status[1];   /* XXX more ports */
    juint8_t             sc_running;
    juint8_t             sc_suspended;
    juint8_t             active_ports;
    jbool_t              sc_allow_suspend;
#ifdef SINGLE_THREADED
#define UHUB_EXPLORE_INTERVAL   (1000 * 5)      /* 5 seconds */
    jtask_h            sc_explore_task;       /* Explore task */
#endif
#ifdef CONFIG_JOTG
    juint8_t            otg_port;
    jbool_t             otg_host_enabled;
    jbool_t             otg_dont_delay_suspend;
#endif
};

#define UHUB_PROTO(sc)          ((sc)->sc_hub->ddesc.bDeviceProtocol)
#define UHUB_IS_HIGH_SPEED(sc)  (UHUB_PROTO(sc) != UDPROTO_FSHUB)
#define UHUB_IS_SINGLE_TT(sc)   (UHUB_PROTO(sc) == UDPROTO_HSHUBSTT)
#define UHUB_IS_ROOT(sc)        (sc->sc_hub == sc->sc_hub->bus->root_hub)

static usbd_status uhub_explore(usbd_device_handle hub, jbool_t find_new);
static jresult_t remote_wakeup_resume(jdevice_t self);
static jresult_t resume_self(struct uhub_softc *sc, jbool_t notify_parent);
static jresult_t suspend_self(struct uhub_softc *sc, jbool_t notify_parent);
static void uhub_intr(usbd_xfer_handle xfer, usbd_private_handle addr,
    usbd_status status);

/*  HSU addition */
/*  Don't suspend a hub thats not the root hub.  */
/*  hsu_dont_suspend_hub_workaround_enabled = TRUE =>          */
/*  prevent the problem: hub is suspend, HSU goes to LPM,      */
/*  DOK connected to hub, hub is disconnected by SW by mistake. */
jbool_t hsu_dont_suspend_hub_workaround_enabled = TRUE;
/* End of HSU addition */

static jresult_t uhub_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usb_device_descriptor_t *dd = usbd_get_device_descriptor(uaa->device);

    DBG_I(DHUB_ATTACH, ("uhub_match, dd=%p\n", dd));
    /*
     * The subclass for hubs seems to be 0 for some and 1 for others,
     * so we just ignore the subclass.
     */
    if (uaa->iface == NULL && dd->bDeviceClass == UDCLASS_HUB)
        return (UMATCH_DEVCLASS_DEVSUBCLASS);
    return (UMATCH_NONE);
}

#ifdef SINGLE_THREADED
static void uhub_explore_task(void *arg)
{
    usbd_device_handle dev = (usbd_device_handle)arg;

    uhub_explore(dev, TRUE);

    jtask_schedule(((struct uhub_softc *)
        (dev->hub->hubsoftc))->sc_explore_task,
        UHUB_EXPLORE_INTERVAL, uhub_explore_task, (void *)arg);
}
#endif

static char devinfo[1024];

static jresult_t uhub_attach(jdevice_t self)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);

    usbd_device_handle dev = uaa->device;
    usbd_status err;
    struct usbd_hub *hub;
    usb_device_request_t req;
    usb_hub_descriptor_t hubdesc;
    juint8_t p, port, nports, nremov;
    jint32_t pwrdly;
    usbd_interface_handle iface;
    usb_endpoint_descriptor_t *ed;

    DBG_I(DHUB_ATTACH, ("uhub_attach\n"));
    sc->sc_hub = dev;
    usbd_devinfo(dev, 1, devinfo, sizeof(devinfo));
    sc->sc_dev = self;
    j_device_set_desc(self, devinfo);
    device_printf(self, ("%s\n", devinfo));

    if (UHUB_IS_HIGH_SPEED(sc)) 
    {
        DBG_I(DHUB_ATTACH, ("%s: %s transaction translator%s\n",
            j_device_get_nameunit(sc->sc_dev),
            UHUB_IS_SINGLE_TT(sc) ? "single" : "multiple",
            UHUB_IS_SINGLE_TT(sc) ? "" : "s"));
    }

    err = usbd_set_config_index(dev, 0, 1);
    if (err)
    {
        DBG_E(DHUB_ATTACH, ("%s: configuration failed, error=%s\n",
            j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        return JENXIO;
    }

    if (dev->depth > USB_HUB_MAX_DEPTH) 
    {
        jprintf("%s: hub depth (%d) exceeded, hub ignored\n",
            j_device_get_nameunit(sc->sc_dev), USB_HUB_MAX_DEPTH);
        return JENXIO;
    }

    /* Get hub descriptor. */
    req.bmRequestType = UT_READ_CLASS_DEVICE;
    req.bRequest = UR_GET_DESCRIPTOR;
    USETW2(req.wValue, (dev->address > 1 ? UDESC_HUB : 0), 0);
    USETW(req.wIndex, 0);
    USETW(req.wLength, USB_HUB_DESCRIPTOR_SIZE);
    DBG_V(DHUB_ATTACH, ("usb_init_hub: getting hub descriptor\n"));
    err = usbd_do_request(dev, &req, (void *)&hubdesc);
    nports = hubdesc.bNbrPorts;
    if (!err && nports > 7) 
    {
        USETW(req.wLength, USB_HUB_DESCRIPTOR_SIZE + (nports+1) / 8);
        err = usbd_do_request(dev, &req, (void *)&hubdesc);
    }
    if (err) 
    {
        DBG_E(DHUB_ATTACH, ("%s: getting hub descriptor failed, error=%s\n",
            j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        return JENXIO;
    }

    for (nremov = 0, port = 1; port <= nports; port++)
    {
        if (!UHD_NOT_REMOV(&hubdesc, port))
            nremov++;
    }

    /* If hub has NO removable ports, or(and) is part of compound device or
     * supports remote wakeup, we can suspend it */
    if (nremov == 0 || 
        (UGETW(hubdesc.wHubCharacteristics) & UHD_COMPOUND) ||
        (sc->sc_hub->cdesc->bmAttributes & UC_REMOTE_WAKEUP))
    {
      sc->sc_allow_suspend = TRUE;
    }
#ifdef JDEBUG
    else
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] might not suspend correctly\n",
        j_device_get_nameunit(sc->sc_dev)));
    }
#endif

    jprintf("%s: %d port%s with %d removable, %s powered\n",
        j_device_get_nameunit(sc->sc_dev), nports, nports != 1 ? "s" : "",
        nremov, dev->self_powered ? "self" : "bus");

    hub = jmalloc(sizeof(struct usbd_hub) + 
        (nports-1) * sizeof(struct usbd_port), M_ZERO);
    if (hub == NULL)
        return JENXIO;

    dev->hub = hub;
    dev->hub->hubsoftc = sc;
    hub->explore = uhub_explore;
    hub->hubdesc = hubdesc;

    DBG_I(DHUB_ATTACH,("usbhub_init_hub: selfpowered=%d, parent=%p, "
        "parent->selfpowered=%d\n", dev->self_powered, dev->powersrc->parent,
        dev->powersrc->parent ? dev->powersrc->parent->self_powered : 0));

#ifdef CONFIG_JOTG
    /* if uhub is root hub */
    if (!sc->sc_hub->bus->root_hub)
    {
        sc->otg_port = sc->sc_hub->bus->otg_port;

        if (sc->otg_port)
        {
            DBG_V(DHUB_ATTACH, ("uhub_attach: root hub has the OTG port %d\n", 
                sc->otg_port));
        }
        else
        {
            /* Not a part of OTG */
            sc->otg_host_enabled = 1;
        }
    }
#endif
    
    if (!dev->self_powered && dev->powersrc->parent != NULL &&
        !dev->powersrc->parent->self_powered) 
    {
        jprintf("%s: bus powered hub connected to bus powered hub, "
            "ignored\n", j_device_get_nameunit(sc->sc_dev));
        goto bad;
    }

    /* Set up interrupt pipe. */
    err = usbd_device2interface_handle(dev, 0, &iface);
    if (err) 
    {
        jprintf("%s: no interface handle\n", j_device_get_nameunit(sc->sc_dev));
        goto bad;
    }
    ed = usbd_interface2endpoint_descriptor(iface, 0);
    if (ed == NULL) 
    {
        jprintf("%s: no endpoint descriptor\n",
            j_device_get_nameunit(sc->sc_dev));
        goto bad;
    }
    if ((ed->bmAttributes & UE_XFERTYPE) != UE_INTERRUPT) 
    {
        jprintf("%s: bad interrupt endpoint\n", 
            j_device_get_nameunit(sc->sc_dev));
        goto bad;
    }

    err = usbd_open_pipe_intr(iface, ed->bEndpointAddress, USBD_SHORT_XFER_OK, 
        &sc->sc_ipipe, sc, sc->sc_status, sizeof(sc->sc_status), uhub_intr, 
        UHUB_INTR_INTERVAL);
    if (err)
    {
        jprintf("%s: cannot open interrupt pipe\n",
            j_device_get_nameunit(sc->sc_dev));
        goto bad;
    }

    /* Wait with power off for a while. */
    jdelay_ms(USB_POWER_DOWN_TIME);

    /*
     * To have the best chance of success we do things in the exact same
     * order as Windoze98.  This should not be necessary, but some
     * devices do not follow the USB specs to the letter.
     *
     * These are the events on the bus when a hub is attached:
     *  Get device and config descriptors (see attach code)
     *  Get hub descriptor (see above)
     *  For all ports
     *     turn on power
     *     wait for power to become stable
     * (all below happens in explore code)
     *  For all ports
     *     clear C_PORT_CONNECTION
     *  For all ports
     *     get port status
     *     if device connected
     *        wait 100 ms
     *        turn on reset
     *        wait
     *        clear C_PORT_RESET
     *        get port status
     *        proceed with device attachment
     */

    /* Set up data structures */
    for (p = 0; p < nports; p++) 
    {
        struct usbd_port *up = &hub->ports[p];
        up->device = 0;
        up->parent = dev;
        up->portno = p+1;
        if (dev->self_powered)
            /* Self powered hub, give ports maximum current. */
            up->power = USB_MAX_POWER;
        else
            up->power = USB_MIN_POWER;
        up->restartcnt = 0;

#ifdef CONFIG_PORT_INTERCHIP
        if (!sc->sc_hub->bus->root_hub)
        {
            juint8_t interchip;

            usbd_get_port_interchip(sc->sc_hub, up->portno, &interchip);
            up->is_interchip = interchip;
        }
#endif
    }

    pwrdly = dev->hub->hubdesc.bPwrOn2PwrGood * UHD_PWRON_FACTOR
        + USB_EXTRA_POWER_UP_TIME;
    for (port = 1; port <= nports; port++) 
    {
#ifdef CONFIG_JOTG
        if (port == sc->otg_port)
            continue;
#endif

        /* Turn the power on. */
        err = usbd_set_port_feature(dev, port, UHF_PORT_POWER);
        if (err)
        {
            jprintf("%s: port %d power on failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), port, usbd_errstr(err));
        }
        DBG_V(DHUB_ATTACH, ("usb_init_port: turn on port %d power\n", port));
        /* Wait for stable power. */
        jdelay_ms(pwrdly);
    }

    /* The usual exploration will finish the setup. */

    sc->sc_running = 1;

#ifdef SINGLE_THREADED
    if (jtask_init(&sc->sc_explore_task, TASK_CLASS_DRIVER))
        goto bad;

    DBG_V(DHUB_ATTACH, ("Running initial hub explore\n"));
    uhub_explore(dev, TRUE);

    DBG_V(DHUB_ATTACH, ("Starting uhub explore task, device: %p\n", dev));
    jtask_schedule(sc->sc_explore_task, UHUB_EXPLORE_INTERVAL,
        uhub_explore_task, (void *)dev);
#endif

#ifdef CONFIG_TEST_DICT
    jtest_dict_add(TEST_DEV_HUB, sc);
#endif

    return 0;

bad:
    jfree(hub);
    dev->hub = 0;
    return JENXIO;
}

usbd_status uhub_explore(usbd_device_handle dev, jbool_t find_new)
{
    usb_hub_descriptor_t *hd = &dev->hub->hubdesc;
    usb_hub_status_t st;
    struct uhub_softc *sc = dev->hub->hubsoftc;
    struct usbd_port *up;
    usbd_status err;
    juint8_t speed;
    juint8_t port;
    jresult_t rc;
    jint_t change = 0, status = 0;
  
    LOCK_CHECK;

    DBG_V(DHUB_EXPLORE, ("UHUB: [%s] uhub_explore dev=%p addr=%d %s %s\n",
        j_device_get_nameunit(sc->sc_hub->bdev), dev, dev->address, 
        find_new ? "find new" : "", sc->sc_suspended ? "suspended" : ""));
    
    if (sc->sc_suspended)
    {
        if (!UHUB_IS_ROOT(sc))
        {
            DBG_I(DHUB_EXPLORE, ("UHUB: [%s] Explore didn't start, "
                "uhub suspended\n", j_device_get_nameunit(sc->sc_hub->bdev)));
            return (USBD_NOT_STARTED);
        }
    
        DBG_I(DHUB_EXPLORE, ("UHUB: [%s] explore new devices prevented\n",
            j_device_get_nameunit(sc->sc_hub->bdev)));

        find_new = FALSE;
    }
    
    if (!sc->sc_running)
    {
        DBG_E(DHUB_EXPLORE, ("UHUB: [%s] Explore didnt start, "
            "uhub not running\n", j_device_get_nameunit(sc->sc_hub->bdev)));
        return (USBD_NOT_STARTED);
    }

    /* Ignore hubs that are too deep. */
    if (dev->depth > USB_HUB_MAX_DEPTH)
        return (USBD_TOO_DEEP);

    for(port = 1; port <= hd->bNbrPorts; port++) 
    {
        up = &dev->hub->ports[port-1];

#ifdef CONFIG_JOTG
        if (port == sc->otg_port && !sc->otg_host_enabled)
        {
            DBG_X(DHUB_EXPLORE, ("UHUB: [%s]: Exploring OTG port while "
                "disabled. Device is 0x%x\n", 
                j_device_get_nameunit(sc->sc_hub->bdev), up->device));
            if (!up->device)
                continue;
            else
                goto disco;
        }
#endif

        err = usbd_get_port_status(dev, port, &up->status);
        if (err) 
        {
            DBG_W(DHUB_EXPLORE, 
                ("UHUB: [%s] Explore: get port status failed addr %d, "
                "error=%s\n", j_device_get_nameunit(sc->sc_hub->bdev),
                dev->address, usbd_errstr(err)));
            continue;
        }

        status = UGETW(up->status.wPortStatus);
        change = UGETW(up->status.wPortChange);

#if defined CONFIG_USB_TESTING
        status |= up->test_cb.status; 
        change |= up->test_cb.change;
#endif
        DBG_V(DHUB_EXPLORE,("uhub_explore: %s port %d status 0x%04x 0x%04x\n",
            j_device_get_nameunit(sc->sc_dev), port, status, change));
        if (change & UPS_C_PORT_ENABLED) 
        {
            DBG_I(DHUB_EXPLORE, ("uhub_explore: C_PORT_ENABLED 0x%x\n",
                change));
            usbd_clear_port_feature(dev, port, UHF_C_PORT_ENABLE);
            if (change & UPS_C_CONNECT_STATUS) 
            {
                /* Ignore the port error if the device
                   vanished. */
            }
            else if (status & UPS_PORT_ENABLED) 
            {
                jprintf("%s: illegal enable change, port %d\n",
                    j_device_get_nameunit(sc->sc_dev), port);
            }
            else 
            {
                /* Port error condition. */
                if (up->restartcnt) /* no message first time */
                {
                    jprintf("%s: port error, restarting port %d\n",
                        j_device_get_nameunit(sc->sc_dev), port);
                }

                if (up->restartcnt++ < USBD_RESTART_MAX)
                    goto disco;
                else
                {
                    jprintf("%s: port error, giving up port %d\n",
                        j_device_get_nameunit(sc->sc_dev), port);
                }
            }
        }

        if (change & UPS_C_OVERCURRENT_INDICATOR)
        {
            DBG_W(DHUB_EXPLORE, ("uhub_explore: Overcurrent detected\n"));

            /* Clear the condition notification */
            usbd_clear_port_feature(dev, port, UHF_C_PORT_OVER_CURRENT);
           
            /* If hub supports power management */
            if (UGETW(hd->wHubCharacteristics) & 
                (UHD_PWR_GANGED | UHD_PWR_INDIVIDUAL))
            {
                /* Turn the port power back on */
                err = usbd_set_port_feature(dev, port, UHF_PORT_POWER);
                if (err)
                {
                    DBG_E(DHUB_EXPLORE, ("UHUB: Port %d power on result %s\n", 
                        port, usbd_errstr(err)));
                }
                else
                {
                    /* Wait for power to be enabled */
                    jdelay_ms(hd->bPwrOn2PwrGood * 2 + 10);
                }
            }
        }
       
        /* Catch power mode changes */
        if (change & UPS_C_SUSPEND)
        {
            DBG_I(DHUB_POWER, ("UHUB: [%s] Port %d suspend change detected "
                "%04x : %04x\n",
                j_device_get_nameunit(sc->sc_hub->bdev), port,
                status, change));

            /* Clear the change notification */
            usbd_clear_port_feature(dev, port, UHF_C_PORT_SUSPEND);

            /* Was this a resume event (and no port change) ? */
            if (!(status & UPS_SUSPEND) && !(change & UPS_C_CONNECT_STATUS))
            {
                up->flags &= ~USBD_PORT_USB_SUSPENDED;
                
                DBG_I(DHUB_POWER, ("UHUB: [%s] Detected resume on port %d\n",
                    j_device_get_nameunit(sc->sc_hub->bdev), port));

                /* If the port was software suspended, resume it */
                if (up->flags & USBD_PORT_SOFT_SUSPENDED)
                {
                    /* Resume the device's driver */
                    if (up->device->hub) 
                    {
                        /* For hub notify of remote wakeup so it doesn't resume
                         * all its ports */
                        rc = j_device_ioctl(up->device->bdev,
                            DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP, NULL);
                    }
                    else
                    {
                        /* Simply resume the driver */
                        rc = j_device_resume(up->device->bdev);
                    }

                    if (!rc)
                    {
                        up->flags &= ~USBD_PORT_SOFT_SUSPENDED;
                        sc->active_ports ++;
                    }
                    else
                    {
                        DBG_E(DHUB_POWER, ("UHUB: [%s] Error resuming device "
                            "on port %d - %d\n", 
                            j_device_get_nameunit(sc->sc_hub->bdev),
                            port, rc));
                    }
                }
            }
        }

        if (!(change & UPS_C_CONNECT_STATUS)) 
        {
            DBG_V(DHUB_EXPLORE,("uhub_explore: port=%d !C_CONNECT_STATUS\n",
                port));

            if (!find_new)
                continue;
            
            /* No status change, just do recursive explore. */
            if (up->device != NULL && up->device->hub != NULL)
                up->device->hub->explore(up->device, find_new);
#if defined(DIAGNOSTIC)
            if (up->device == NULL &&
                (status & UPS_CURRENT_CONNECT_STATUS))
            {
                jprintf("%s: connected, no device\n",
                    j_device_get_nameunit(sc->sc_dev));
            }
#endif
            continue;
        }

        /* We have a connect status change, handle it. */

        DBG_V(DHUB_EXPLORE, ("uhub_explore: status change hub=%d port=%d\n",
            dev->address, port));

        /*
         * If there is already a device on the port the change status
         * must mean that is has disconnected.  Looking at the
         * current connect status is not enough to figure this out
         * since a new unit may have been connected before we handle
         * the disconnect.
         */
disco:
        if (up->device != NULL) 
        {
            /* Disconnected */
            DBG_I(DHUB_EXPLORE, ("uhub_explore: device addr=%d disappeared "
                "on port %d\n", up->device->address, port));

            /* If device was not suspended and attached, update active count */
            if (!(up->flags & USBD_PORT_SOFT_SUSPENDED) && up->device->bdev &&
                j_device_attached(up->device->bdev))
            {
                sc->active_ports --;
            }

            usb_disconnect_port(up, sc->sc_dev);

            up->flags &= ~USBD_PORT_SOFT_SUSPENDED;
            up->flags &= ~USBD_PORT_USB_SUSPENDED;
        }

        if (!find_new 
#ifdef CONFIG_JOTG
            || (port == sc->otg_port && !sc->otg_host_enabled)
#endif
            )
        {
            DBG_W(DHUB_EXPLORE, ("uhub_explore: suppressing new device check "
                "port %d\n", port));
            continue;
        }

        if (change & UPS_C_CONNECT_STATUS)
            usbd_clear_port_feature(dev, port, UHF_C_PORT_CONNECTION);

        if (!(status & UPS_CURRENT_CONNECT_STATUS)) 
        {
            /* Nothing connected, just ignore it. */
            DBG_V(DHUB_EXPLORE,("uhub_explore: port=%d "
                "!CURRENT_CONNECT_STATUS\n", port));
            continue;
        }

        /* Connected */
        if (!(status & UPS_PORT_POWER))
        {
            jprintf("%s: strange, connected port %d has no power\n",
                j_device_get_nameunit(sc->sc_dev), port);
        }

#ifdef CONFIG_JOTG
        if (port != sc->otg_port)
#endif
        {
            if (up->parent == dev->bus->root_hub)
               jnotify_os(NOTIFY_HOST_CONNECT, (void *)((juint32_t)port));

#ifdef CONFIG_PORT_INTERCHIP
            if (up->is_interchip)
                jdelay_ms(USB_PORT_POWERUP_DELAY_INTERCHIP);
            else
#endif
                jdelay_ms(USB_PORT_POWERUP_DELAY);

            /* Reset port, which implies enabling it. */
            if (usbd_reset_port(dev, port, &up->status)) 
            {
                jprintf("%s: port %d reset failed\n",
                    j_device_get_nameunit(sc->sc_dev), port);
                continue;
            }
            /* Get port status again, it might have changed during reset */
            err = usbd_get_port_status(dev, port, &up->status);
            if (err) 
            {
                DBG_W(DHUB_EXPLORE, ("uhub_explore: get port status failed, "
                    "error=%s\n", usbd_errstr(err)));
                continue;
            }

            status = UGETW(up->status.wPortStatus); 
            change = UGETW(up->status.wPortChange);
#if defined CONFIG_USB_TESTING
            status |= up->test_cb.status; 
            change |= up->test_cb.change;
#endif
        }

        if (!(status & UPS_CURRENT_CONNECT_STATUS)) 
        {
            /* Nothing connected, just ignore it. */
#ifdef DIAGNOSTIC
            jprintf("%s: port %d, device disappeared after reset\n",
                j_device_get_nameunit(sc->sc_dev), port);
#endif
            continue;
        }

        /* Figure out device speed */
        if (status & UPS_HIGH_SPEED)
            speed = USB_SPEED_HIGH;
        else if (status & UPS_LOW_SPEED)
            speed = USB_SPEED_LOW;
        else
            speed = USB_SPEED_FULL;
        /* Get device info and set its address. */
#ifdef CONFIG_USB_TESTING
        if (up->test_cb.callback)
        {
            up->test_cb.callback(sc->sc_dev, dev->bus, dev->depth + 1, speed, 
                port, up, &up->test_cb);
            err = USBD_INVALID_STATE;
        }
        else
#endif
        {
            err = usbd_new_device(sc->sc_dev, dev->bus, dev->depth + 1, speed,
                port, up);
#ifdef CONFIG_USB_TESTING
            up->test_cb.status = 0; 
            up->test_cb.change = 0;
#endif

        }
        if (err) 
        {
            if (err == USBD_INVALID_STATE)
                DBG_I(DHUB_EXPLORE, ("uhub_explore: Continuing after test\n"));
            else 
            {
                DBG_E(DHUB_EXPLORE,("uhub_explore: usb_new_device failed, "
                    "error=%s\n", usbd_errstr(err)));
                /* Avoid addressing problems by disabling. */
                /* usbd_reset_port(dev, port, &up->status); */

                /*
                 * The unit refused to accept a new address, or had
                 * some other serious problem.  Since we cannot leave
                 * at 0 we have to disable the port instead.
                 */
                jprintf("%s: device problem, disabling port %d\n",
                    j_device_get_nameunit(sc->sc_dev), port);
                usbd_clear_port_feature(dev, port, UHF_PORT_ENABLE);
            }
        } 
        else
        {
            /* The port set up succeeded, reset error count. */
            up->restartcnt = 0;

            /* If the device was attached, mark port as active */
            if (up->device->bdev && j_device_attached(up->device->bdev))
                sc->active_ports ++;

            if (up->device->hub)
                up->device->hub->explore(up->device, find_new);
        }
    }

    /* Handle hub status change */
    err = usbd_get_hub_status(dev, &st);
    if (err)
        DBG_E(DHUB_EXPLORE, ("uhub_explore: Error getting hub status\n"));
    else
    {
        if (UGETW(st.wHubChange) & UHS_C_OVER_CURRENT)
        {
            DBG_W(DHUB_EXPLORE, ("uhub_explore: Hub overcurrent detected\n"));

            jdelay_ms(500);

            usbd_clear_hub_feature(dev, UHF_C_HUB_OVER_CURRENT);
        }

        if (UGETW(st.wHubChange) & UHS_C_LOCAL_POWER)
        {
            DBG_I(DHUB_EXPLORE, ("uhub_explore: UHS_C_LOCAL_POWER\n"));
            usbd_clear_hub_feature(dev, UHF_C_HUB_LOCAL_POWER);
        }
    }

     /* Are we doing nothing ? Suspend us */
    if (find_new && !sc->sc_suspended && sc->active_ports == 0 
        && !(sc->sc_hub->bus->explore_flags & USBD_BUS_NEED_EXPLORE)
#ifdef CONFIG_JOTG
        && (!UHUB_IS_ROOT(sc) || sc->otg_host_enabled)
        /*  HSU addition */
        && !(!UHUB_IS_ROOT(sc) && (hsu_dont_suspend_hub_workaround_enabled==TRUE))
        /* End of HSU addition */
#endif
        )
    {
        if (!sc->sc_allow_suspend)
        {

            DBG_E(DHUB_POWER, ("UHUB: [%s] Suspending this hub might cause "
                "it to miss connection events\n"));
        }
        /* Check if the hub supports Remote Wakeup */
        else if (sc->sc_hub->cdesc->bmAttributes & UC_REMOTE_WAKEUP)
        {
            err = usbd_set_remote_wakeup(sc->sc_hub, TRUE);
            if (err != USBD_NORMAL_COMPLETION)
            {
                DBG_E(DHUB_POWER, ("UHUB: [%s] Won't accept set_remote_wakeup"
                    "request %s\n", j_device_get_nameunit(sc->sc_hub->bdev),
                    usbd_errstr(err)));
            }
        }
#ifdef JDEBUG
        else
        {
            DBG_I(DHUB_POWER, ("UHUB: [%s] Suspending a hub that "
                "should not require Remote Wakeup\n",
                j_device_get_nameunit(sc->sc_hub->bdev)));
        }
#endif

        DBG_I(DHUB_POWER, ("UHUB: [%s] Suspending a non-active hub\n",
            j_device_get_nameunit(sc->sc_hub->bdev)));

        /* No more active ports, suspend the hub */
        suspend_self(sc, TRUE);
    }
    else
    {
        DBG_X(DHUB_POWER, ("UHUB: [%s] Suspend skipped: [%s%s] active: %d\n",
            j_device_get_nameunit(sc->sc_hub->bdev), 
            find_new ? "Find_New " : "", 
            sc->sc_suspended ? "Suspended" : "",
            sc->active_ports));
    }

    DBG_I(DHUB_EXPLORE, ("UHUB: [%s] uhub_explore done\n",
        j_device_get_nameunit(sc->sc_hub->bdev)));

    return (USBD_NORMAL_COMPLETION);
}

/*
 * Called from process context when the hub is gone.
 * Detach all devices on active ports.
 */
static jresult_t uhub_detach(jdevice_t self)
{
    struct uhub_softc *sc = j_device_get_softc(self);

    struct usbd_hub *hub = sc->sc_hub->hub;
    struct usbd_port *rup;
    jint_t port, nports;

    DBG_I(DHUB_ATTACH, ("uhub_detach: sc=%port\n", sc));

#ifdef CONFIG_TEST_DICT
    jtest_dict_del(TEST_DEV_HUB, sc);
#endif
    
    if (hub == NULL)            /* Must be partially working */
        return (0);

#ifdef SINGLE_THREADED
    jtask_stop(sc->sc_explore_task);
    jtask_uninit(sc->sc_explore_task);
#endif

    /* Close pipe if open (suspend closes them too) */
    if (sc->sc_ipipe)
    {
        usbd_abort_pipe(sc->sc_ipipe);
        usbd_close_pipe(sc->sc_ipipe);
    }

    nports = hub->hubdesc.bNbrPorts;
    for(port = 0; port < nports; port++) 
    {
        rup = &hub->ports[port];
        if (rup->device)
            usb_disconnect_port(rup, self);
    }

    jfree(hub);
    sc->sc_hub->hub = NULL;

    return (0);
}

/*
 * Hub interrupt.
 * This an indication that some port has changed status.
 * Notify the bus that an expore task execution is required
 */
void uhub_intr(usbd_xfer_handle xfer, usbd_private_handle addr, 
    usbd_status status)
{
    struct uhub_softc *sc = addr;

    LOCK_CHECK;

    DBG_V(DHUB_EXPLORE, ("uhub_intr: sc=%p status %s\n",
        sc, usbd_errstr(status)));

    /* Interrupt in suspend can only mean root hub being awoken */
    if (sc->sc_suspended)
        resume_self(sc, FALSE);

    if (status == USBD_STALLED)
        usbd_clear_endpoint_stall_async(sc->sc_ipipe);
    else if (status == USBD_NORMAL_COMPLETION)
    {
#ifdef SINGLE_THREADED
        uhub_explore(sc->sc_hub);
#else
        usb_needs_explore(sc->sc_hub);
#endif
    }
}

static jresult_t hub_port_suspend(struct uhub_softc *sc, juint8_t port)
{
    usbd_status err;

    if (sc->sc_hub->hub->ports[port-1].flags & USBD_PORT_USB_SUSPENDED)
        return 0;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Suspending port %d\n",
        j_device_get_nameunit(sc->sc_hub->bdev), port)); 

    /* Make the hub suspend the port */
    err = usbd_set_port_feature(sc->sc_hub, port, UHF_PORT_SUSPEND);
    if (err)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Cannot suspend port %d due to %s\n",
            j_device_get_nameunit(sc->sc_hub->bdev), port, usbd_errstr(err)));
        return JEIO;
    }

    /* The port on the hub is suspended */
    sc->sc_hub->hub->ports[port-1].flags |= USBD_PORT_USB_SUSPENDED;

    return 0;
}

static jresult_t hub_port_resume(struct uhub_softc *sc, juint8_t port)
{
    usb_port_status_t p_status;
    jint_t change, status;
    usbd_status err;

    /* If the port was not suspended, no need to resume */
    if (!(sc->sc_hub->hub->ports[port-1].flags & USBD_PORT_USB_SUSPENDED))
        return 0;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Resume port %d\n",
        j_device_get_nameunit(sc->sc_hub->bdev), port));
    
    /* Resume the port the device sits on */
    err = usbd_clear_port_feature(sc->sc_hub, port, UHF_PORT_SUSPEND);
    if (err)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Cannot resume port %d, %s\n",
            j_device_get_nameunit(sc->sc_hub->bdev), port, usbd_errstr(err)));
        return JEIO;
    }

    /* Let the resume propogate */
#ifdef CONFIG_JOTG    
    if (port == sc->otg_port)
    {
        jdelay_ms(USB_RESUME_DELAY_OTG);
    }
    else
#endif
    {
        jdelay_ms(USB_RESUME_DELAY);
    }

    /* Check if the port is operating correctly after the resume */
    err = usbd_get_port_status(sc->sc_hub, port, &p_status);
    if (err) 
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Cannot get port status after "
            "port resume %s\n", j_device_get_nameunit(sc->sc_hub->bdev), 
            usbd_errstr(err)));
        return JEIO;
    }

    status = UGETW(p_status.wPortStatus);
    change = UGETW(p_status.wPortChange);

    /* If the port change poped up, clear it */
    if (change & UPS_C_SUSPEND)
        usbd_clear_port_feature(sc->sc_hub, port, UHF_C_PORT_SUSPEND);

    /* Check that the port is no longer suspended */
    if (status & UPS_SUSPEND)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Port %d resume failed\n", 
            j_device_get_nameunit(sc->sc_hub->bdev), port));
        return JEIO;
    }

    /* Clear the usb suspend flag */
    sc->sc_hub->hub->ports[port-1].flags &= ~USBD_PORT_USB_SUSPENDED;

    return 0;
}

static jresult_t suspend_self(struct uhub_softc *sc, jbool_t notify_parent)
{
    jresult_t rc = 0;

    /* Are we already suspended ? */
    if (sc->sc_suspended)
        return 0;

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_HUB, sc, TEST_DEV_STATE_SUSPENDED);
#endif

    DBG_V(DHUB_POWER, ("UHUB: [%s] Suspending Self\n",
        j_device_get_nameunit(sc->sc_hub->bdev)));
    
    if (!UHUB_IS_ROOT(sc) && sc->sc_ipipe)
    {
       /* Close interrupt pipe */
        usbd_abort_pipe(sc->sc_ipipe);
        usbd_close_pipe(sc->sc_ipipe);
        sc->sc_ipipe = NULL;
    }

    sc->sc_suspended = 1;

    /* Notify parent if self suspend happened */
    if (notify_parent)
    {
        if (UHUB_IS_ROOT(sc))
        {
            /* Suspend the controller */
            rc = controller_usb_suspend_self(sc->sc_hub->bus->usb_wrap);
        }
        else
        {
            /* Notify parent that we suspended ourselves */
            rc = j_device_ioctl(j_device_get_parent(sc->sc_hub->bdev),
                DRV_IOCTL_PWR_CHILD_SUSPEND, (void *)sc->sc_hub->bdev);
        }
        if (rc)
        {
            DBG_E(DHUB_POWER, ("UHUB: [%s] Parent would not acknowlege "
                "our attempt to suspend - %d\n", 
                j_device_get_nameunit(sc->sc_hub->bdev), rc));
        }
    }

    return 0;
}

static jresult_t suspend_children(struct uhub_softc *sc)
{
    struct usbd_hub *hub = sc->sc_hub->hub;
    struct usbd_port *rup;
    jint_t port, nports;
    jresult_t rc = 0;

    if (hub == NULL)
        return 0;

    /* Suspend all child devices */
    nports = hub->hubdesc.bNbrPorts;
    for (port = 1; port <= nports && !rc; port++) 
    {
        rup = &hub->ports[port-1];

        /* Is there anything on that port ? */
        if (!rup->device)
            continue;

        /* Suspend the port of the lowest hub in the chain */
        if (UHUB_IS_ROOT(sc))
        {
            /* Suspend the port itself */
            rc = hub_port_suspend(sc, port);
            if (rc)
                return rc;
        }

        /* If the device was not attached there's no need to suspend it */
        if (!j_device_attached(rup->device->bdev))
            continue;

        /* Is there anything not-suspended on the port ? */
        if (rup->flags & USBD_PORT_SOFT_SUSPENDED)
            continue;

        DBG_V(DHUB_POWER, ("UHUB: [%s] Suspending port %d [%s]\n",
            j_device_get_nameunit(sc->sc_hub->bdev), port,
            j_device_get_nameunit(rup->device->bdev)));

        /* Suspend the port device's driver */
        rc = j_device_suspend(rup->device->bdev);
        if (rc)
            return rc;

        sc->active_ports --;
        
        rup->flags |= USBD_PORT_SOFT_SUSPENDED;
    }
    
#ifdef JDEBUG
    if (sc->active_ports != 0)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] After suspend active ports is %d\n",
            j_device_get_nameunit(sc->sc_hub->bdev), sc->active_ports));
    }
#endif

    return 0;
}

static jresult_t resume_self(struct uhub_softc *sc, jbool_t notify_parent)
{
    usb_endpoint_descriptor_t *ed;
    usbd_interface_handle iface;
    jdevice_t self = sc->sc_hub->bdev;
    usbd_status err;
    jresult_t rc;

    /* Resume ourselves if we were suspended */
    if (!(sc->sc_suspended))
        return 0;

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_HUB, sc, TEST_DEV_STATE_NORMAL);
#endif
    
    DBG_I(DHUB_POWER, ("UHUB: [%s] Self resume\n",
        j_device_get_nameunit(self)));

    if (notify_parent)
    {
        /* Notify parent that we want to resume */
        if (UHUB_IS_ROOT(sc))
        {
            /* If this is a root hub, resume the controller */
            rc = controller_usb_resume_self(sc->sc_hub->bus->usb_wrap);
        }
        else
        {
            rc = j_device_ioctl(j_device_get_parent(self),
                DRV_IOCTL_PWR_CHILD_RESUME, (void *)self);
        }

        if (rc)
        {
            DBG_E(DHUB_POWER, ("UHUB: [%s] Father would not acknowlege "
                "our attempt to resume - %d\n", 
                j_device_get_nameunit(self), rc));
            return rc;
        }
    }

    /* Clear the flag if it is supported */
    err = usbd_set_remote_wakeup(sc->sc_hub, FALSE);
    if (err)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Failed to unset remote wakeup %s\n", 
            j_device_get_nameunit(self), usbd_errstr(err)));
    }

    sc->sc_suspended = 0;

    /* If the interrupt pipe was not closed, we are done */
    if (sc->sc_ipipe)
        return 0;

    /* Set up interrupt pipe. */
    usbd_device2interface_handle(sc->sc_hub, 0, &iface);
    ed = usbd_interface2endpoint_descriptor(iface, 0);

    /* Open interrupt pipe */
    err = usbd_open_pipe_intr(iface, ed->bEndpointAddress, 
        USBD_SHORT_XFER_OK, &sc->sc_ipipe, sc, sc->sc_status, 
        sizeof(sc->sc_status), uhub_intr, UHUB_INTR_INTERVAL);
    if (err)
    {
        jprintf("%s: cannot re-open interrupt pipe after suspend\n",
            j_device_get_nameunit(sc->sc_dev));
    }

    return 0;
}

static void resume_children(struct uhub_softc *sc)
{
    struct usbd_hub *hub = sc->sc_hub->hub;
    struct usbd_port *rup;
    jint_t port, nports;
    jresult_t rc;

    if (hub == NULL)
        return;
 
    /* Resume all the suspended ports */
    nports = hub->hubdesc.bNbrPorts;
    for (port = 1; port <= nports; port++) 
    {
        rup = &hub->ports[port-1];
        if (!rup->device)
            continue;

        /* If the port was selectively suspended, resume it now */
        rc = hub_port_resume(sc, port);
        if (rc)
            goto Error;

        /* If the device was not attached there's no need to resume it */
        if (!j_device_attached(rup->device->bdev))
            continue;

        /* If the port's device driver is suspended, resume it */
        if (!(rup->flags & USBD_PORT_SOFT_SUSPENDED))
            continue;

        /* Resume the port device's driver */
        rc = j_device_resume(rup->device->bdev);
        if (rc)
            goto Error;

        sc->active_ports ++;

        rup->flags &= ~USBD_PORT_SOFT_SUSPENDED;
        
        continue;

Error:
        jprintf("%s: Cannot resume port %d with device %s - %d\n",
            j_device_get_nameunit(sc->sc_hub->bdev), port,
            j_device_get_nameunit(rup->device->bdev), rc);
    }
}

static jint_t find_child_port(struct uhub_softc *sc, jdevice_t child)
{
    jint_t port;
    struct usbd_port *ports = sc->sc_hub->hub->ports;

    for (port = 1; port <= sc->sc_hub->hub->hubdesc.bNbrPorts; port++) 
    {
        if (!ports[port-1].device)
            continue;

        if (ports[port-1].device->bdev == child)
            return port;
    }

    /* Port numbers are one based, so zero is a failure */
    return 0;
}

static jresult_t uhub_suspend(jdevice_t self)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    jresult_t rc;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Suspend request\n",
        j_device_get_nameunit(self)));

    /* Suspend all the hub's ports */
    rc = suspend_children(sc);
    if (rc)
        goto Error;

    /* Suspend the hub itself (close pipes etc) */
    rc = suspend_self(sc, FALSE);
    if (rc)
        goto Error;

    return 0;
    
Error:
    resume_children(sc);
    return rc;
}

static jresult_t uhub_resume(jdevice_t self)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    jresult_t rc;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Resume request\n", 
        j_device_get_nameunit(self)));

    rc = resume_self(sc, FALSE);
    if (rc)
        return rc;

    /* Check for any disconnected devices */
    uhub_explore(sc->sc_hub, FALSE);

    resume_children(sc);

    return rc;
}

static jresult_t selective_suspend_child(struct uhub_softc *sc, jdevice_t child)
{
    jresult_t rc;
    jint_t port;
    
    /* Find out on what port the child sits */
    port = find_child_port(sc, child);
    if (port < 1)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Cannot suspend %s because port "
            "was not found\n", j_device_get_nameunit(sc->sc_hub->bdev),
            j_device_get_nameunit(child)));
        return JEINVAL;
    }
   
    /* Make sure we don't suspend something twice */
    if (sc->sc_hub->hub->ports[port-1].flags & USBD_PORT_SOFT_SUSPENDED)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Trying to suspend a port that is "
            "already suspended %d\n", 
            j_device_get_nameunit(sc->sc_hub->bdev), port));
        return JEINVAL;
    }

    /* The port's driver is suspended */
    sc->sc_hub->hub->ports[port-1].flags |= USBD_PORT_SOFT_SUSPENDED;
   
    sc->active_ports --;

    /* Suspend the actual hub port */
    rc = hub_port_suspend(sc, port);
    if (rc)
        return rc;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Child suspend port %d device %s, "
        "active ports left %d [%d]\n", j_device_get_nameunit(sc->sc_hub->bdev),
        port, j_device_get_nameunit(child), 
        sc->active_ports, sc->sc_suspended)); 

    /* Let the suspend propogate */
#ifdef CONFIG_JOTG    
    if (port == sc->otg_port)
    {
        if (sc->otg_dont_delay_suspend)
            sc->otg_dont_delay_suspend = 0;
        else
            jdelay_ms(USB_SUSPEND_DELAY_OTG);
    }
    else
#endif
    {
        jdelay_ms(USB_SUSPEND_DELAY);
    }

    return 0;
}

static jresult_t selective_suspend(jdevice_t self, jdevice_t child)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    jresult_t rc;

    /* 1. Suspend the child's port */
    rc = selective_suspend_child(sc, child);
    if (rc)
        return rc;

    /* 2. Suspend self if nothing is running */
    if (!sc->active_ports && !sc->sc_suspended
#ifdef CONFIG_JOTG
        && (!UHUB_IS_ROOT(sc) || sc->otg_host_enabled)
#endif
        )
    {
        usb_needs_explore(sc->sc_hub);
    }
    
    return 0;
}

static jresult_t selective_resume(jdevice_t self, jdevice_t child)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    jresult_t rc;
    jint_t port;

    /* Find out on what port the child sits */
    port = find_child_port(sc, child);
    if (port < 1)
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Cannot resume %s because port "
            "was not found\n", j_device_get_nameunit(self),
            j_device_get_nameunit(child)));
        return JEINVAL;
    }

    /* Make sure we don't resume a port twice */
    if (!(sc->sc_hub->hub->ports[port-1].flags & USBD_PORT_SOFT_SUSPENDED))
    {
        DBG_E(DHUB_POWER, ("UHUB: [%s] Trying to resume a port that is "
            "already active %d\n", j_device_get_nameunit(self), port));
        return JEINVAL;
    }

    DBG_I(DHUB_POWER, ("UHUB: [%s] Resuming port %d with device %s\n",
        j_device_get_nameunit(self), port, j_device_get_nameunit(child)));

    /* Make sure this hub is up and running */
    rc = resume_self(sc, TRUE);
    if (rc)
        return rc;

    /* Make sure the hub's port isn't suspended */
    rc = hub_port_resume(sc, port);
    if (rc)
        return rc;

    /* The port's driver is no longer suspended 
     * It should not self-resume if parent resume failed */
    sc->sc_hub->hub->ports[port-1].flags &= ~USBD_PORT_SOFT_SUSPENDED;

    sc->active_ports ++;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Selective resume port %d device %s, "
        "active ports %d\n", j_device_get_nameunit(sc->sc_hub->bdev),
        port, j_device_get_nameunit(child), sc->active_ports)); 

    return 0;
}

static jresult_t remote_wakeup_resume(jdevice_t self)
{
    struct uhub_softc *sc = j_device_get_softc(self);
    jresult_t rc;

    DBG_I(DHUB_POWER, ("UHUB: [%s] Resuming the hub due to Remote-Wakeup\n", 
        j_device_get_nameunit(self)));

    rc = resume_self(sc, FALSE);
    if (rc)
        return rc;

    /* Check for changes */
    uhub_explore(sc->sc_hub, FALSE);

    return rc;
}

#ifdef CONFIG_JOTG
static jresult_t enable_host_port(struct uhub_softc *sc, jbool_t do_reset)
{
    usbd_status err;
    jresult_t rc = JEINVAL;
    DECLARE_FNAME("enable_host_port");


    if (sc->otg_host_enabled)
        goto Exit;

    DBG_V(DHUB_POWER, ("%s: Enable OTG Port %s Reset\n", fname,
        (do_reset ? "with" : "without")));

    if (do_reset)
    {
        /* Reset Port */
        err = usbd_set_port_feature(sc->sc_hub, sc->otg_port, UHF_PORT_RESET); 
        if (err)
        {
            DBG_E(DHUB_POWER, ("%s: Failure in setting Port Reset - %d\n", 
                fname, err));
            goto Exit;
        }
        /* Wait for device to recover from reset. */
        err = usbd_clear_port_feature(sc->sc_hub, sc->otg_port, 
            UHF_C_PORT_RESET);
        if (err)
        {
            DBG_E(DHUB_POWER, ("%s: Failure in clearing Port Reset - %d\n", 
                fname, err));
            goto Exit;
        }
    }

    sc->otg_host_enabled = 1;
    rc = 0;

Exit:
    return rc;
}

static jresult_t disable_host_port(struct uhub_softc *sc)
{
    if (!sc->otg_host_enabled)
        return JEINVAL;

    DBG_V(DHUB_POWER, ("disable_host_port: Disable OTG Port\n"));

    sc->otg_host_enabled = 0;

    if (sc->sc_suspended)
        resume_self(sc, TRUE);        

    /* If there is a connected device we must run explore for removing it */
    if (sc->sc_hub->hub->ports[sc->otg_port-1].device)
        usb_needs_explore(sc->sc_hub);

    return 0;
}

static jresult_t enable_hnp(struct uhub_softc *sc)
{
    struct usbd_port *port = &sc->sc_hub->hub->ports[sc->otg_port - 1];
    usb_otg_descriptor_t *otg_desc;
    usbd_status err;
    jresult_t rc = 0;
    DECLARE_FNAME("wait_for_hnp");

    if (!port->device)
    {
        DBG_E(DHUB_EXPLORE, ("%s: No Device connected on port 0\n", fname));
        rc = JEINVAL;
        goto Exit;
    }

    otg_desc = usbd_get_otg_descriptor(port->device);
    if (!otg_desc || !(otg_desc->bmAttributes & UOTG_HNP))
    {
        DBG_W(DHUB_EXPLORE, ("%s: Connected device is not HNP capable\n", 
            fname));
        rc = JENOTSUP;
        goto Exit;
    }

    err = usbd_set_otg_mode(port->device, UOTG_B_HNP_ENABLE);
    if (err)
    {
        DBG_E(DHUB_EXPLORE, ("%s: Failure to set HNP enable - %d\n", fname,
            err));
        rc = JEINVAL;
    }    

Exit:
    return rc;
}
#endif

static jresult_t uhub_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
#ifdef CONFIG_JOTG
    struct uhub_softc *sc = j_device_get_softc(self);
#endif
    jresult_t rc = JEINVAL;

    DBG_V(DHUB_POWER, ("UHUB: IOCTL %x arg=%p\n", ioctl, arg));

    LOCK_CHECK;

    switch (ioctl)
    {
    case DRV_IOCTL_PWR_CHILD_SUSPEND:
        rc = selective_suspend(self, (jdevice_t)arg);
        break;

    case DRV_IOCTL_PWR_CHILD_RESUME:
        rc = selective_resume(self, (jdevice_t)arg);
        break;

    case DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP:
        rc = remote_wakeup_resume(self);
        break;

#ifdef CONFIG_JOTG
    case DRV_IOCTL_HOST_ENABLE_HNP:
        rc = enable_hnp(sc);
        break;

    case DRV_IOCTL_HOST_PORT_ENABLE:
        rc = enable_host_port(sc, (jbool_t)arg);
        break;
        
    case DRV_IOCTL_HOST_PORT_DISABLE:        
        rc = disable_host_port(sc);
        break;

    case DRV_IOCTL_HOST_PORT_SUSPEND:
        sc->otg_dont_delay_suspend = 1;
        rc = uhub_ioctl(self, DRV_IOCTL_PWR_CHILD_SUSPEND,
            sc->sc_hub->hub->ports[sc->otg_port-1].device->bdev);
        break;
    case DRV_IOCTL_HOST_PORT_RESUME:
        rc = uhub_ioctl(self, DRV_IOCTL_PWR_CHILD_RESUME,
            sc->sc_hub->hub->ports[sc->otg_port-1].device->bdev);
        break;
#endif
    default:
        DBG_E(DHUB_EXPLORE, ("UHUB: Unknown IOCTL %x\n", ioctl));
        break;
    }

    return rc;
}

DRIVER_USB_DECLARE(uhub)

