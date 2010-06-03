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
#include <usbdivar.h>

#include <ehcireg.h>
#include <ehcivar.h>

#ifdef CONFIG_TDI_4X
static const char *ehci_controller_name = "TDI EHCI OTG Controller";
#else
static const char *ehci_controller_name = "Generic EHCI Controller";
#endif

static jresult_t ehci_local_attach(jdevice_t self);
static jresult_t ehci_local_detach(jdevice_t self);
static jresult_t ehci_local_suspend(jdevice_t self);
static jresult_t ehci_local_resume(jdevice_t self);
static jresult_t ehci_local_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg);

static jresult_t ehci_local_suspend(jdevice_t self)
{
    ehci_softc_t *sc = j_device_get_softc(self);
    return controller_usb_suspend(sc->sc_bus.usb_wrap);
}

static jresult_t ehci_local_resume(jdevice_t self)
{
    ehci_softc_t *sc = j_device_get_softc(self);
    return controller_usb_resume(sc->sc_bus.usb_wrap);
}

static jresult_t ehci_local_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    ehci_softc_t *sc = (ehci_softc_t *)j_device_get_softc(self);
    jresult_t rc = 0;

    if (ioctl == DRV_IOCTL_HOST_PORT_ENABLE)
    {
        rc = jinterrupt_setup(sc->irq_res, ehci_intr, sc, ehci_softintr, sc, 
            &sc->interrupt_h);
        if (rc)
            goto Exit;
    }

    rc = ehci_ioctl(sc, ioctl, arg);

    if (ioctl == DRV_IOCTL_HOST_PORT_DISABLE && sc->interrupt_h)
    {
        jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
        sc->interrupt_h = NULL;
    }

Exit:
    return rc;
}

/* TODO - This code should probably be called in ehci_local_detach - recheck
 * it when power managment will be implemented 
static jresult_t ehci_local_uninit(jdevice_t self)
{
    ehci_softc_t *sc = j_device_get_softc(self);
    jresult_t err;

    err = j_bus_generic_shutdown(self);
    if (err)
        return (err);
    ehci_shutdown(sc);

    return 0;
}
*/

static const char *ehci_local_match(jdevice_t self)
{
    return (ehci_controller_name);
}

static jresult_t ehci_local_probe(jdevice_t self)
{
    const char *desc = ehci_local_match(self);

    if (!desc) 
        return JENXIO;

    j_device_set_desc(self, desc);
    return 0;
}

static jresult_t ehci_local_attach(jdevice_t self)
{
    ehci_softc_t *sc = j_device_get_softc(self);
    jbus_h bus = j_device_get_bus(self);
    jint_t resource_id = 0;
    jresult_t err;

    sc->sc_bus.usbrev = USBREV_2_0;
    sc->sc_bus.bdev = self;
    sc->itc_num = EHCI_CMD_ITC_2;

#ifdef CONFIG_JOTG
    sc->ocd_handle = j_device_get_ivars(self);
    /* Make sure that the bus.c adds the MEM and IRQ resources for the DCD 
     * with ID = 2 */
    if (sc->ocd_handle)
        resource_id = 2;
#endif
    
    sc->io_res = j_bus_alloc_resource(bus, SYS_RES_MEMORY, resource_id);
    if (!sc->io_res) 
    {
        device_printf(self, ("Could not map memory\n"));
        return JENXIO;
    }

    sc->irq_res = j_bus_alloc_resource(bus, SYS_RES_IRQ, resource_id);
    if (sc->irq_res == NULL) 
    {
        device_printf(self, ("Could not allocate irq\n"));
        ehci_local_detach(self);
        return JENXIO;
    }

    j_device_set_ivars(sc->sc_bus.bdev, sc);

    /* ehci_local_match will never return NULL if ehci_local_probe succeeded */
    j_device_set_desc(sc->sc_bus.bdev, ehci_local_match(self));
    j_snprintf(sc->sc_vendor, sizeof(sc->sc_vendor), "Intel");

#ifdef CONFIG_JOTG
    if (!sc->ocd_handle)
#endif
    {
        err = jinterrupt_setup(sc->irq_res, ehci_intr, sc, ehci_softintr, sc, 
            &sc->interrupt_h);
        if (err) 
        {
            device_printf(self, ("Could not setup irq, %d\n", err));
            sc->interrupt_h = NULL;
            ehci_local_detach(self);
            return JENXIO;
        }        
    }

    /*
     * No companion controllers
     */

    sc->sc_ncomp = 0;

    err = ehci_init(sc);
    if (!err) 
    {
        sc->sc_flags |= EHCI_SCFLG_DONEINIT;
        err = controller_usb_attach(self, &sc->sc_bus.usb_wrap);
    }

    if (err) 
    {
        device_printf(self, ("USB init failed err=%d\n", err));
        j_device_detach(self);
        return JEIO;
    }
    return 0;
}

static jresult_t ehci_local_detach(jdevice_t self)
{
    ehci_softc_t *sc = j_device_get_softc(self);
    jbus_h bus = j_device_get_bus(self);

    if (sc->sc_flags & EHCI_SCFLG_DONEINIT)
        ehci_detach(sc, 0);

    /*
     * disable interrupts that might have been switched on in ehci_init
     */

    if (sc->interrupt_h) 
    {
        jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
        sc->interrupt_h = NULL;
    }

    if (sc->sc_bus.usb_wrap) 
    {
        controller_usb_detach(sc->sc_bus.usb_wrap);
        sc->sc_bus.usb_wrap = NULL;
    }

    if (sc->irq_res) 
    {
        jbus_release_resource(bus, sc->irq_res);
        sc->irq_res = NULL;
    }
    if (sc->io_res) 
    {
        jbus_release_resource(bus, sc->io_res);
        sc->io_res = NULL;
    }
    if (sc->sc_flags & EHCI_SCFLG_DONEINIT) 
    {
        ehci_freemem(sc);
        sc->sc_flags &= ~EHCI_SCFLG_DONEINIT;
    }
    return 0;
}

DRIVER_CONTROLLER_DECLARE(ehci_local, sizeof(ehci_softc_t))

