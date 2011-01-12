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
#define INCLUDE_DEVICE
#define INCLUDE_GENERAL
#include "jos_internal.h"
#include "jos_driver.h"

#define MAXINT_LEGTH 10

void *j_device_get_ivars(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Getting ivars of %p\n", dev));
    KASSERT(dev != NULL, ("device_get_ivars(NULL, ...)\n"));
    return dev->ivars;
}

void j_device_set_ivars(jdevice_t dev, void * ivars)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Setting ivars of %p to %p\n", dev, ivars));
    KASSERT(dev != NULL, ("device_set_ivars(NULL, ...)\n"));
    dev->ivars = ivars;
}

char *j_device_get_nameunit(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Get nameunit of %p [%s]\n",
        dev, dev ? dev->nameunit : "none"));
    KASSERT(dev != NULL, ("device_get_nameunit(NULL, ...)\n"));
    return (dev->nameunit ? dev->nameunit : "");
}

jbus_h j_device_get_bus(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Get bus of %p [%s] - %p\n",
        dev, dev->nameunit, dev->bus_u.bus));
    KASSERT(dev != NULL, ("device_get_bus(NULL, ...)\n"));
    return dev->bus_u.bus;
}

void j_device_set_nameunit(jdevice_t dev, char *name, jint_t unit)
{
    jint_t len;
    DBG_V(DJOS_DEVICE, ("JDEV: Set nameunit of %p [%s] %d\n",
        dev, name, unit));
    KASSERT(dev != NULL, ("device_set_nameunit(NULL, ...)\n"));

    if (dev->nameunit)
        jfree(dev->nameunit);

    len = j_strlen(name) + MAXINT_LEGTH;
    dev->nameunit = jmalloc(len, M_ZERO);
    if (!dev->nameunit)
        return;
    
    dev->unit = unit;
    j_snprintf(dev->nameunit, len, "%s%ld", name, unit);
}

jdevice_t j_device_get_parent(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Get parent of %p\n", dev));
    KASSERT(dev != NULL, ("device_get_parent(NULL, ...)\n"));
    return dev->parent;
}

void *j_get_usbd_device(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Get usbd dev %p\n", dev));
    KASSERT(dev != NULL, ("device_get_usbd_device(NULL, ...)\n"));
    return dev->bus_u.usbd_device;
}

void *j_device_get_softc(jdevice_t dev)
{
#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DJOS_DEVICE, ("JDEV: Get softc %p\n", dev));
#endif /* CONFIG_DATA_PATH_DEBUG */
    KASSERT(dev != NULL, ("device_get_softc(NULL, ...)\n"));
    return dev->softc;
}

jint16_t j_device_get_unit(jdevice_t dev)
{
    DBG_V(DJOS_DEVICE, ("JDEV: Get unit %p\n", dev));
    KASSERT(dev != NULL, ("device_get_unit(NULL, ...)\n"));
    return dev->unit;
}

jbool_t j_device_attached(jdevice_t dev)
{
    KASSERT(dev != NULL, ("device_attached(NULL, ...)\n"));
    return (dev->jdrv ? TRUE : FALSE);
}

/* Scan the driver tree for a driver for a device. Use the 
 * MATCH/ATTACH mechanics to find a driver 
 * Parameters:
 * IN   dev             - Device to try to attach
 * IN   usbd_device     - Device pointer to use with core
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t j_device_probe_and_attach(jdevice_t dev, void *usbd_device)
{
    DBG_I(DJOS_DEVICE, ("JDEV: Attach %p [%p]\n", dev, usbd_device));
    dev->bus_u.usbd_device = usbd_device;
    return jdriver_attach(DRIVER_USB, dev, 0, NULL);
}

jbool_t j_device_is_attached(jdevice_t dev)
{
    return (dev->jdrv != 0);
}

void j_device_set_desc(jdevice_t dev, const char* desc)
{
    if (!desc)
        return;

    DBG_V(DJOS_DEVICE, ("JDEV: Setting desc for %p to [%s]\n",
        dev, desc));
    
    if (dev->desc)
        jfree(dev->desc);
    dev->desc = j_strdup(desc);
}

char *j_device_get_desc(jdevice_t dev)
{
    if (!dev)
        return 0;

    return dev->desc;
}

jresult_t j_device_detach(jdevice_t dev)
{
    DBG_I(DJOS_DEVICE, ("JDEV: Detach %p\n", dev));

    if (dev->jdrv)
        jdriver_detach(dev);

    return 0;
}
void jdevice_delete(jdevice_t device)
{
    DBG_I(DJOS_DEVICE, ("JDEV: Delete %p\n", device));

    if (!device)
        return;

    if (device->nameunit)
        jfree(device->nameunit);
    if (device->desc)
        jfree(device->desc);
    jfree(device);
}

jdevice_t jdevice_add(jdevice_t parent)
{
    jdevice_t device;

    DBG_I(DJOS_DEVICE, ("JDEV: Add %p\n", parent));

    device = jmalloc(sizeof(struct jdevice_s), M_ZERO);
    if (!device)
        return NULL;

    device->parent = parent;

    return device;
}

