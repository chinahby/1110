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

#include <jos.h>

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

#include <td243_rev2_reg.h>
#include <td243_rev2_var.h>

static jresult_t td243hc_rev2_attach(jdevice_t self);
static jresult_t td243hc_rev2_detach(jdevice_t self);
static jresult_t td243hc_rev2_suspend(jdevice_t self);
static jresult_t td243hc_rev2_resume(jdevice_t self);
static jresult_t td243hc_rev2_ioctl(jdevice_t self, drv_ioctl_t ioctl, 
    void *arg);

static jresult_t td243hc_rev2_probe(jdevice_t self)
{
    return 0;
}

static jresult_t td243hc_rev2_attach(jdevice_t self)
{
    jresult_t rc = 0;
    td243hc_rev2_softc_t *sc = j_device_get_softc(self);
    jbus_h bus = j_device_get_bus(self);
    DECLARE_FNAME("td243hc_rev2_attach");

    sc->sc_bus.bdev = self;
    DBG_V(DHCD_INIT, ("%s: Entered\n", fname));

    /* Allocate resources */
#ifdef CONFIG_PCI
#define MEM_RES_ID 1
#else
#define MEM_RES_ID 0
#endif
    sc->mem_res = j_bus_alloc_resource(bus, SYS_RES_MEMORY, MEM_RES_ID);
    if (!sc->mem_res)
    {
        DBG_E(DHCD_INIT, ("%s: Error allocating memory resource\n", fname));
        rc = JEINVAL;
        goto Error;
    }

    sc->irq_res = j_bus_alloc_resource(bus, SYS_RES_IRQ, 0);
    if (!sc->irq_res)
    {
        DBG_E(DHCD_INIT, ("DCD: Error allocating interrupt resource\n"));
        rc = JEINVAL;
        goto Error;
    }

    j_device_set_ivars(sc->sc_bus.bdev, sc);

    jinterrupt_setup(sc->irq_res, td243hc_rev2_intr, sc, td243hc_rev2_softintr,
        sc, &sc->interrupt_h);

    /* Init hardware */
    td243hc_rev2_hw_init(sc);

    /* Connect controller to Host Core */
    controller_usb_attach(self, &sc->sc_bus.usb_wrap);
    
    return 0;

Error:
    td243hc_rev2_detach(self);
    return rc;
}

static jresult_t td243hc_rev2_detach(jdevice_t self)
{
    td243hc_rev2_softc_t *sc = j_device_get_softc(self);
    jbus_h bus = j_device_get_bus(self);

    td243hc_rev2_hw_uninit(sc, 0);

    if (sc->interrupt_h)
        jinterrupt_teardown(sc->irq_res, sc->interrupt_h);
    
    controller_usb_detach(sc->sc_bus.usb_wrap);

    if (sc->irq_res)
        jbus_release_resource(bus, sc->irq_res);
    if (sc->mem_res)
        jbus_release_resource(bus, sc->mem_res);
    
    return 0;
}

static jresult_t td243hc_rev2_suspend(jdevice_t self)
{
    td243hc_rev2_softc_t *sc = j_device_get_softc(self);
    return controller_usb_suspend(sc->sc_bus.usb_wrap);
}

static jresult_t td243hc_rev2_resume(jdevice_t self)
{
    td243hc_rev2_softc_t *sc = j_device_get_softc(self);
    return controller_usb_resume(sc->sc_bus.usb_wrap);
}

static jresult_t td243hc_rev2_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    td243hc_rev2_softc_t *sc = (td243hc_rev2_softc_t *)j_device_get_softc(self);
    jresult_t rc = 0;

    rc = td243hc_rev2_hw_ioctl(sc, ioctl, arg);

    return rc;
}

DRIVER_CONTROLLER_DECLARE(td243hc_rev2, sizeof(td243hc_rev2_softc_t))

