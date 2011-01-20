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
#include <jos.h>
#include <dev_monitor.h>

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

devmon_methods_t *dev_monitor = NULL;

jresult_t devmon_notify(devmon_msg_t msg, mdev_t dev, juint32_t param)
{
    if (dev_monitor->devmon_notify_h)
        return dev_monitor->devmon_notify_h(msg, dev, param);

    return 0;
}

jresult_t devmon_force_detach(mdev_t dev)
{
    DBG_I(DUTIL_DEVMON, ("DEVMON: Forcing detach for %p [%s]\n",
        dev, j_device_get_nameunit(dev)));
    
    return j_device_detach((jdevice_t)dev);
}

void devmon_get_info(mdev_t dev, devmon_info_t *info)
{
    usb_device_descriptor_t *desc;
    void *usbd_device;
 
    DBG_I(DUTIL_DEVMON, ("DEVMON: Info for %p\n", dev));

    j_memset(info, 0, sizeof(devmon_info_t));

    if (!dev)
        return;
    
    usbd_device = j_get_usbd_device(dev);
    if (!usbd_device)
        return;
    
    desc = &((usbd_device_handle)usbd_device)->ddesc;

    info->dev_class     = desc->bDeviceClass;
    info->subclass      = desc->bDeviceSubClass;
    info->protocol      = desc->bDeviceProtocol;
    info->vendor        = UGETW(desc->idVendor);
    info->product       = UGETW(desc->idProduct);
    info->version       = UGETW(desc->bcdDevice);
    info->name          = j_device_get_nameunit(dev);
    info->desc          = j_device_get_desc(dev);
    info->depth         = ((usbd_device_handle)usbd_device)->depth;
}

jresult_t devmon_register(devmon_methods_t *methods)
{
    DBG_I(DUTIL_DEVMON, ("DEVMON: Registered monitor %p\n", methods));
    
    if (dev_monitor)
        return JEINVAL;
    
    dev_monitor = methods;
    return 0;
}

void devmon_unregister(void)
{
    DBG_I(DUTIL_DEVMON, ("DEVMON: Un-Registered monitor\n"));
    
    dev_monitor = NULL;
}

void devmon_handle_detached(mdev_t dev, mdev_t parent)
{
    if (!dev_monitor || !dev || !dev_monitor->detached)
        return;

    DBG_I(DUTIL_DEVMON, ("DEVMON: Handling detach of %p [%s] parent [%p]\n",
        dev, j_device_get_nameunit(dev), parent));

    dev_monitor->detached(dev, parent);
}

void devmon_handle_attached(mdev_t dev, mdev_t parent)
{
    if (!dev_monitor || !dev || !dev_monitor->attached)
        return;

    DBG_I(DUTIL_DEVMON, ("DEVMON: Handling attach of %p [%s] parent [%p]\n",
        dev, j_device_get_nameunit(dev), parent));

    dev_monitor->attached(dev, parent);
}

jresult_t devmon_allow_attach(mdev_t dev)
{
    if (!dev_monitor || !dev || !dev_monitor->allow_attach)
        return 0;

    DBG_I(DUTIL_DEVMON, ("DEVMON: Checking if attach allowed for %p [%s]\n",
        dev, j_device_get_nameunit(dev)));

    return dev_monitor->allow_attach(dev);
}

