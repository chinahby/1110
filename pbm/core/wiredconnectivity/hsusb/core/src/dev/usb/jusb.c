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
#include <usb_quirks.h>

struct usb_softc {
    jdevice_t            sc_dev;    /* base device */
    usbd_bus_handle     sc_bus;    /* USB controller */
    struct usbd_port    sc_port;   /* dummy port for root hub */
    char                sc_dying;
    jbool_t              suspended;
};

static void usb_discover(void *v);
static void explore_task(void *arg);

static const char *usbrev_str[] = USBREV_STR;

jresult_t controller_usb_attach(jdevice_t device, void **handle)
{
    struct usb_softc *sc;
    usbd_device_handle dev;
    usbd_status err;
    jint_t usbrev;
    juint8_t speed;
    juint32_t first_explore_delay = 0;

    sc = jmalloc(sizeof(struct usb_softc), M_ZERO);
    if (!sc)
        return JENOMEM;

    DBG_I(DHOST_USB, ("usbd_attach\n"));

    sc->sc_dev = device;
    sc->sc_bus = j_device_get_softc(device);
    sc->sc_bus->usbctl = sc;
    sc->sc_port.power = USB_MAX_POWER;
    sc->sc_bus->bdev = device;

    *handle = sc;

    jprintf("%s", j_device_get_nameunit(sc->sc_dev));

    usbrev = sc->sc_bus->usbrev;
    jprintf(": USB revision %s", usbrev_str[usbrev]);
    switch (usbrev) 
    {
    case USBREV_1_0:
    case USBREV_1_1:
        speed = USB_SPEED_FULL;
        break;
    case USBREV_2_0:
        speed = USB_SPEED_HIGH;
        break;
    default:
        jprintf(", not supported\n");
        sc->sc_dying = 1;
        return JENXIO;
    }
    jprintf("\n");

    if (jtask_init(&sc->sc_bus->explore_task, TASK_CLASS_DRIVER))
        return JENOMEM;

    err = usbd_new_device(sc->sc_dev, sc->sc_bus, 0, speed, 0, &sc->sc_port);
    if (!err) 
    {
        dev = sc->sc_port.device;
        if (dev->hub == NULL) 
        {
            sc->sc_dying = 1;
            jprintf("%s: root device is not a hub\n",
                j_device_get_nameunit(sc->sc_dev));
            return JENXIO;
        }
        sc->sc_bus->root_hub = dev;
    } 
    else
    {
        jprintf("%s: root hub problem, error=%d\n",
            j_device_get_nameunit(sc->sc_dev), err);
        sc->sc_dying = 1;
        return JENXIO;
    }

    /* Make sure first discover does something. */
    sc->sc_bus->explore_flags |= USBD_BUS_NEED_EXPLORE;

#ifdef CONFIG_EHCI
    /* In case EHCI is used, we need to make the companion controllers
     * postpone their explore to allow EHCI to take over the port. */
    if (j_strncmp(j_device_get_nameunit(sc->sc_dev), "ehci", 4) != 0)
        first_explore_delay = 1500;
#endif

#ifdef CONFIG_TEST_DICT
    jtest_dict_add(TEST_DEV_HCD, device);
#endif

    /* Schedule a bus explore */
    jtask_schedule(sc->sc_bus->explore_task, 
        first_explore_delay, explore_task, sc);

    return 0;
}

jresult_t controller_usb_suspend_self(void *handle)
{
    struct usb_softc *sc = (struct usb_softc *)handle;
    jresult_t rc;

    DBG_I(DHOST_POWER, ("USB: Starting controller self suspend %p\n", handle));

#ifdef CONFIG_JOTG
    if (!sc->sc_bus->methods->is_otg_host_enabled(sc->sc_bus))
    {
        DBG_E(DHCD_POWER, ("USB: Trying to suspend controller while host is "
            "disabled\n"));
        return JEINVAL;
    }
#endif

    if (sc->suspended)
        return 0;

    /* Suspend the controller */
    rc = sc->sc_bus->methods->suspend(sc->sc_bus);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Cannot suspend controller %d\n", rc));
        return rc;
    }
    
    sc->suspended = TRUE;
    
    jnotify_os(NOTIFY_HOST_SUSPEND, j_device_get_bus(sc->sc_bus->bdev));

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_HCD, sc->sc_dev, TEST_DEV_STATE_SUSPENDED);
#endif
    
    DBG_I(DHOST_POWER, ("USB: Finished suspend\n"));
    return 0;
}

jresult_t controller_usb_suspend(void *handle)
{
    struct usb_softc *sc = (struct usb_softc *)handle;
    jresult_t rc;

    DBG_I(DHOST_POWER, ("USB: Starting suspend %p\n", handle));

    if (!sc->sc_port.device || !sc->sc_port.device->bdev)
    {
        DBG_E(DHOST_POWER, ("USB: Trying to suspend controller with no hub\n"));
        return JEINVAL;
    }
   
    /* Suspend all devices */
    rc = j_device_suspend(sc->sc_port.device->bdev);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Cannot suspend devices %d\n", rc));
        return rc;
    }

    /* Suspend the controller */
    rc = controller_usb_suspend_self(handle);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Cannot suspend controller %d\n", rc));
        return rc;
    }
    
    DBG_I(DHOST_POWER, ("USB: Finished suspend\n"));
    return 0;
}

jresult_t controller_usb_resume_self(void *handle)
{
    struct usb_softc *sc = (struct usb_softc *)handle;
    jresult_t rc;

    DBG_I(DHOST_POWER, ("USB: Starting controller self resume %p\n", handle));

    if (!sc->suspended)
        return 0;

    rc = jnotify_os(NOTIFY_HOST_RESUME, j_device_get_bus(sc->sc_bus->bdev));
    if (rc)
    {
        DBG_W(DHOST_POWER, ("USB: OS disallowed controller resume, rc = %d\n",
            rc));
        return rc;
    }

    rc = sc->sc_bus->methods->resume(sc->sc_bus);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Cannot resume controller %d\n", rc));
        return rc;
    }

    sc->suspended = FALSE;

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_HCD, sc->sc_dev, TEST_DEV_STATE_NORMAL);
#endif

    DBG_I(DHOST_POWER, ("USB: Finished resume\n"));
    
    return 0;
}

jresult_t controller_usb_resume(void *handle)
{
    struct usb_softc *sc = (struct usb_softc *)handle;
    jresult_t rc;

    DBG_I(DHOST_POWER, ("USB: Starting resume %p\n", handle));

    if (!sc->sc_port.device || !sc->sc_port.device->bdev)
    {
        DBG_E(DHOST_POWER, ("USB: Trying to resume controller with no hub\n"));
        return JEINVAL;
    }

    rc = controller_usb_resume_self(handle);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Cannot resume controller %d\n", rc));
        return rc;
    }

    /* Resume all the devices */
    rc = j_device_resume(sc->sc_port.device->bdev);
    if (rc)
    {
        DBG_E(DHOST_POWER, ("USB: Error resuming device %d\n", rc));
        return rc;
    }

    DBG_I(DHOST_POWER, ("USB: Finished resume\n"));
    
    return 0;
}

void explore_task(void *arg)
{
    struct usb_softc *sc = arg;

    DBG_I(DHOST_USB, ("explore_task: started bus %p\n", sc->sc_bus));

    if (sc->sc_dying) 
        return;

    /* Mark bus as ready for explore.
     * The explore task is called  by the attach() for the first time,
     * only later usb_needs_explore() will be allowed */
    sc->sc_bus->explore_flags |= USBD_BUS_READY_4_EXPLORE;
    
    usb_discover(sc);

    jtask_schedule(sc->sc_bus->explore_task, 1000L * 60, explore_task, sc);
}

/* Explore device tree from the root. */
static void usb_discover(void *v)
{
    struct usb_softc *sc = v;

    DBG_V(DHOST_USB,("usb_discover\n"));

    /* We need mutual exclusion while traversing the device tree,
     * but this is guaranteed since this function cannot be interrupted
     * by any other explore (they all run from the same task level) */
    while ((sc->sc_bus->explore_flags & USBD_BUS_NEED_EXPLORE) &&
        !sc->sc_dying) 
    {
        DBG_I(DHOST_USB,("usb_discover running\n"));
        
        sc->sc_bus->explore_flags &= ~USBD_BUS_NEED_EXPLORE;

        sc->sc_bus->root_hub->hub->explore(sc->sc_bus->root_hub, TRUE);
    }
}

void usb_needs_explore(usbd_device_handle dev)
{
    DBG_V(DHOST_USB, ("USB: usb_needs_explore %p\n", dev));
    
    /* Check if the bus is ready to be explored */
    if (!(dev->bus->explore_flags & USBD_BUS_READY_4_EXPLORE))
    {
        DBG_I(DHOST_USB, ("USB: Not ready for explore %p - %p\n",
            dev, dev->bus));
        return;
    }
    
    /* Mark the need for explore */
    dev->bus->explore_flags |= USBD_BUS_NEED_EXPLORE;

     /* Run the explore as soon as possible */
    if (jtask_schedule(dev->bus->explore_task, 0, 
        explore_task, dev->bus->usbctl))
    {
        DBG_E(DHOST_USB, ("USB: Cannot schedule explore task\n"));
    }
}

jresult_t controller_usb_detach(void *handle)
{
    struct usb_softc *sc = (struct usb_softc *)handle;

    DBG_I(DHOST_USB, ("usb_detach: start\n"));

    sc->sc_dying = 1;

#ifdef CONFIG_TEST_DICT
    jtest_dict_del(TEST_DEV_HCD, sc->sc_dev);
#endif

    /* Make all devices disconnect. */
    if (sc->sc_port.device != NULL)
        usb_disconnect_port(&sc->sc_port, sc->sc_dev);

    jtask_uninit(sc->sc_bus->explore_task);

    jfree(handle);

    return (0);
}

