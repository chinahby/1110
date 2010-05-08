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
#include <jutil.h>

#include "jms_device.h"
#include "jms_devmgr.h"
#include "jms_transport.h"

typedef struct ms_devmgr_t {
    ms_device_t **device_luns;
    jmutex_h    access_lock;
    juint8_t     active_devs;
} ms_devmgr_t;

void device_lock(void *devmgr)
{
    jmutex_lock(((ms_devmgr_t*)devmgr)->access_lock);
}

void device_unlock(void *devmgr)
{
    jmutex_unlock(((ms_devmgr_t*)devmgr)->access_lock);
}

jresult_t jms_devmgr_attach(void *transport, void *usbd_dev, juint8_t maxlun, 
    juint8_t subclass, void **context)
{
    juint8_t i;
    jresult_t rc = 0;
    jbool_t success = 0;
    ms_devmgr_t *dev = NULL;

    DBG_I(DMASS_DEVICE, ("DEVMGR: Got attach. Luns:%d Sub:%d\n", maxlun, 
        subclass));

    if (maxlun >= MAX_POSSIBLE_LUNS)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: Maxlun is over limit %d/%d\n", 
            maxlun, MAX_POSSIBLE_LUNS));
        return JEINVAL;
    }

    dev = jmalloc(sizeof(ms_devmgr_t), 0);
    if (!dev)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: Error allocating devmgr struct\n"));
        return JENOMEM;
    }

    dev->device_luns = jmalloc(sizeof(ms_device_t*) * MAX_POSSIBLE_LUNS, 
        M_ZERO);
    if (!dev->device_luns)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: Error allocating device_luns\n"));
        rc = JENOMEM;
        goto Exit;
    }

    rc = jmutex_init(&dev->access_lock);
    if (rc)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: Error allocating mutex %d\n", rc));
        goto Exit;
    }

    for (i = 0; i < maxlun + 1; i++) 
    {
        dev->device_luns[i] = device_init(transport, usbd_dev, i, subclass,
            dev);

        if (dev->device_luns[i])
        {
            success = TRUE;
            dev->active_devs ++;
        }
    }

    if (!success)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: No devices attached !\n"));
        rc = JENOENT;
        goto Exit;
    }

    *context = (void *)dev;

    DBG_I(DMASS_DEVICE, ("DEVMGR: Attach finished, active %d\n",
        dev->active_devs));

    return 0;

Exit:
    if (dev && dev->access_lock)
        jmutex_uninit(dev->access_lock);

    if (dev && dev->device_luns)
        jfree(dev->device_luns);

    if (dev)
        jfree(dev);

    DBG_E(DMASS_DEVICE, ("DEVMGR: Attach failed %d\n", rc));

    return rc;
}

void jms_devmgr_detach(void *context)
{
    ms_devmgr_t *dev = context;
    juint8_t i;

    DBG_I(DMASS_DEVICE, ("DEVMGR: Got detach %p\n", context));

    if (!dev || !dev->device_luns)
    {
        DBG_E(DMASS_DEVICE, ("DEVMGR: Detach called with invalid context !\n"));
        return;
    }

    for (i = 0; i < MAX_POSSIBLE_LUNS; i++) 
    {
        if (dev->device_luns[i])
            device_uninit(dev->device_luns[i]);
    }

    jmutex_uninit(dev->access_lock);
    jfree(dev->device_luns);
    jfree(dev);
    DBG_I(DMASS_DEVICE, ("DEVMGR: Detach finished\n"));
}

jresult_t jms_devmgr_suspend_device(ms_device_t *device)
{
    jresult_t rc;
    ms_devmgr_t *devmgr;

    if (!device)
        return JEINVAL;

    rc = device_suspend(device);
    if (rc)
        return rc;

    devmgr = (ms_devmgr_t *)device->devmgr;

    devmgr->active_devs --;

    /* If there are no more active devices, we suspend the whole usb device */
    if (devmgr->active_devs != 0)
        return 0;

    DBG_I(DMASS_POWER, ("DEVMGR: [%s] All LUNs inactive, suspend usb device\n",
        device->product));

    rc = jms_transport_suspend_self(device->transport);
    if (rc)
        goto Error;

    return 0;

Error:
    if (!device_resume(device))
        devmgr->active_devs ++;

    return rc;
}

jresult_t jms_devmgr_resume_device(ms_device_t *device)
{
    jresult_t rc;
    ms_devmgr_t *devmgr;

    if (!device)
        return JEINVAL;

    devmgr = (ms_devmgr_t *)device->devmgr;

    DBG_I(DMASS_POWER, ("DEVMGR: [%s] Resume device, active %d\n", 
        device->product, devmgr->active_devs));

    /* If there are no active devices, we need to resume ourselves */
    if (devmgr->active_devs == 0)
    {

        DBG_I(DMASS_POWER, ("DEVMGR: [%s] All logical devices suspended, "
            "resuming physical device\n", device->product));

        rc = jms_transport_resume_self(device->transport);
        if (rc)
            return rc;
    }

    rc = device_resume(device);
    if (rc)
        return rc;

    devmgr->active_devs ++;

    return 0;
}

jresult_t jms_devmgr_suspend(void *context)
{
    ms_devmgr_t *dev = context;
    juint8_t i;
    jresult_t rc;

    DBG_I(DMASS_POWER, ("DEVMGR: Suspend request %p\n", context));

    if (!dev || !dev->device_luns)
    {
        DBG_E(DMASS_POWER, ("DEVMGR: Suspend called with invalid context\n"));
        return JEINVAL;
    }

    for (i = 0; i < MAX_POSSIBLE_LUNS; i++) 
    {
        if (!dev->device_luns[i])
            continue;

        if (dev->device_luns[i]->flags.suspended)
            continue;

        rc = device_suspend(dev->device_luns[i]);
        if (rc)
            goto Error;

        dev->active_devs --;
    }

    return 0;

Error:
    DBG_W(DMASS_POWER, ("DEVMGR: Error %d while suspending\n", rc));
    jms_devmgr_resume(context);
    return rc;
}

void jms_devmgr_resume(void *context)
{
    ms_devmgr_t *dev = context;
    juint8_t i;

    DBG_I(DMASS_POWER, ("DEVMGR: Resume request %p\n", context));

    if (!dev || !dev->device_luns)
    {
        DBG_E(DMASS_POWER, ("DEVMGR: Resume called with invalid context !\n"));
        return;
    }

    for (i = 0; i < MAX_POSSIBLE_LUNS; i++) 
    {
        if (!dev->device_luns[i])
            continue;

        if (!dev->device_luns[i]->flags.suspended)
            continue;

        if (!device_resume(dev->device_luns[i]))
            dev->active_devs ++;
    }
}

