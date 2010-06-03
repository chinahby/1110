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
#include <drivers.h>
#ifdef CONFIG_JHOST
# include <dev_monitor.h>
#endif
#include "jos_internal.h"
#include "jos_driver.h"

typedef struct driver_list_s {
    driver_methods_t            *methods;
    jint_t                       type;
    char                        *name;
    jint_t                       id;
    juint8_t                     usage_count;
    juint8_t                     curr_unit;
    juint16_t                    softsize;
    struct driver_list_s        *next;
    TAILQ_HEAD(dev_list_s, jdevice_s) dev_list;
} driver_list_t;

/* Lists of registered drivers */
driver_list_t driver_list = { NULL, 0, "root", 0, 0, 0, 0, NULL };

jresult_t jdrivers_init(void)
{
    jresult_t rc;
    jint_t i;
    
    DBG_I(DJOS_DRIVER, ("JDRV: Starting to load Non-USB drivers\n"));
    for (i = 0; non_usb_driver_list[i].init; i++)
    {
        rc = non_usb_driver_list[i].init();
        if (rc)
        {
            jdrivers_uninit();
            return rc;
        }
        non_usb_driver_list[i].init_done = TRUE;
    }
    
    DBG_I(DJOS_DRIVER, ("JDRV: Starting to load USB drivers\n"));
    for (i = 0; driver_load_list[i]; i++)
    {
        rc = driver_load_list[i]();
        if (rc)
        {
            jdrivers_uninit();
            return rc;
        }
    }

    DBG_I(DJOS_DRIVER, ("JDRV: Finished loading, total %d drivers\n", i));

    return 0;
}

void jdrivers_uninit(void)
{
    driver_list_t *curr, *next;
    jint_t i;

    DBG_I(DJOS_DRIVER, ("JDRV: Starting driver unload\n"));
    
    for (curr = driver_list.next; curr; curr = next)
    {
        DBG_I(DJOS_DRIVER, ("JDRV: Removing %s [%p] -> [%p]\n",
            curr->name, curr, curr->next));

        if (curr->usage_count)
        {
            DBG_E(DJOS_DRIVER, ("JDRV: Unloading driver [%s] type %d "
                "that is being used by %d devices !\n",
                curr->name, curr->type, curr->usage_count));
        }

        /* Run the driver's uninit function if its set */
        if (curr->methods->uninit)
            curr->methods->uninit();

        next = curr->next;
        jfree(curr);
    }
    
    driver_list.next = NULL;

    DBG_I(DJOS_DRIVER, ("JDRV: Starting to un-load non usb drivers\n"));
  
    /* Find the end of the list */
    for (i = 0; non_usb_driver_list[i].init; i++)
        ;

    /* Un-init in reverse from how we initialized */
    for (; i >= 0; i--)
    {
        if (non_usb_driver_list[i].init_done)
            non_usb_driver_list[i].uninit();
    }

    DBG_I(DJOS_DRIVER, ("JDRV: Finished driver unloading\n"));
}

jresult_t jdriver_register(jint_t type, driver_methods_t *methods,
    char *name, jint_t id, juint16_t size)
{
    driver_list_t *new_driver;

#ifdef JDEBUG 
    if (!name)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Cannot register, no name provided\n"));
        return JEINVAL;
    }

    if (!methods)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Cannot register %s, "
            "no methods\n", name));
        return JEINVAL;
    }
    
    if (!methods->attach || !methods->detach)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Cannot register %s, "
            "some methods are missing\n", name));
        return JEINVAL;
    }

    if (type != DRIVER_CONTROLLER && type != DRIVER_USB)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Cannot register %s, "
            "unknown driver type %d\n", name, type));
        return JEINVAL;
    }
#endif

    new_driver = jmalloc(sizeof(driver_list_t), 0);
    if (!new_driver)
        return JENOMEM;

    new_driver->type    = type;
    new_driver->methods = methods;
    new_driver->name    = name;
    new_driver->type    = type;
    new_driver->id      = id;
    new_driver->usage_count = 0;
    new_driver->softsize = size;
    new_driver->curr_unit = 0;

    /* Insert into list */
    new_driver->next    = driver_list.next;
    driver_list.next    = new_driver;

    TAILQ_INIT(&new_driver->dev_list);
    
    DBG_I(DJOS_DRIVER, ("JDRV: Registered new driver %p type %d [%s:%d]\n",
        new_driver, type, name, size));
    
    return 0;
}

void jdriver_detach(jdevice_t device)
{
    DBG_V(DJOS_DRIVER, ("JDRV: Starting detach for %p\n", device));

    if (!device->jdrv)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Trying to detach a device without "
            "a driver !\n"));
        return;
    }

    DBG_I(DJOS_DRIVER, ("JDRV: Detaching from %p [%s]\n", 
        device->jdrv, device->jdrv->name));

    if (device->jdrv->methods->detach)
        device->jdrv->methods->detach(device);

    if (device->softc)
        jfree(device->softc);

    device->jdrv->usage_count --;

    TAILQ_REMOVE(&device->jdrv->dev_list, device, next);

    device->jdrv = NULL;
    
#ifdef CONFIG_JHOST
    /* If usb driver, tell device manager that we detached */
    if (device->type == DRIVER_USB)
        devmon_handle_detached(device, device->parent);
#endif
}

jresult_t jdriver_controller_attach(jdevice_t device, jint_t id, jbus_h bus)
{
    driver_list_t *curr;
    jresult_t rc;

    if (!device)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Device is NULL"));
        return JEINVAL;
    }

    DBG_I(DJOS_DRIVER, ("JDRV: Trying to attach controller %x\n", id));

    for (curr = driver_list.next;
        curr && (curr->type != DRIVER_CONTROLLER || curr->id != id);
        curr = curr->next)
        ;

    if (!curr)
    {
        DBG_W(DJOS_DRIVER, ("JDRV: Cannot find driver for controller %x\n",
            id));
        return JENOENT;
    }

    DBG_X(DJOS_DRIVER, ("JDRV: Found driver for controller %p [%s:%d]\n",
        curr, curr->name, curr->softsize));

    /* Alloc softc if driver needs it */
    if (curr->softsize)
    {
        device->softc = jmalloc(curr->softsize, M_ZERO);
        if (!device->softc)
        {
            rc = JENOMEM;
            goto Error;
        }
    }

    device->bus_u.bus = bus;
    device->type = DRIVER_CONTROLLER;
    
    j_device_set_nameunit(device, curr->name, curr->curr_unit++);

    DBG_V(DJOS_DRIVER, ("JDRV: Created device %p softc %p [%d]\n",
        device, device->softc, curr->softsize));
    
    /* Try to attach */
    rc = curr->methods->attach(device);
    if (rc)
    {
        DBG_W(DJOS_DRIVER, ("JDRV: Error attaching to %s controller, "
            "error = %d\n", curr->name, rc));
        goto Error;
    }

    device->jdrv = curr;
    
    curr->usage_count ++;
    
    return 0;

Error:
    if (device->softc)
        jfree(device->softc);

    return rc;
}

jresult_t jdriver_usb_attach(jdevice_t device)
{
    static jint_t unknown_unit = 0;
#ifdef CONFIG_JHOST
    static jint_t forbidden_unit = 0;
#endif
    driver_list_t *curr, *best = NULL;
    
    jint_t match_level, best_level = 0;
    jresult_t rc;

    DBG_I(DJOS_DRIVER, ("JDRV: Usb Attach called %p\n", device));

    /* Find the best suitable driver for this device */
    for (curr = driver_list.next; curr; curr = curr->next)
    {
        DBG_V(DJOS_DRIVER, ("JDRV: Running match with %p [%s]\n", 
            curr, curr->name));

        if (curr->type != DRIVER_USB)
            continue;

        /* Alloc softc if driver needs it */
        if (curr->softsize)
        {
            device->softc = jmalloc(curr->softsize, M_ZERO);
            if (!device->softc)
                continue;
        }
 
        match_level = curr->methods->match(device);

        jfree(device->softc);
        device->softc = NULL;
        
        if (match_level > best_level)
        {
            best = curr;
            best_level = match_level;
        }
    }

    if (!best)
    {
        DBG_W(DJOS_DRIVER, ("JDRV: Cannot find driver for usb device %p\n",
            device));
        j_device_set_nameunit(device, "unknown", unknown_unit++);
        return JENOENT;
    }

    DBG_I(DJOS_DRIVER, ("JDRV: Trying to attach device to %p [%s]\n",
        best, best->name));

#ifdef CONFIG_JHOST
    /* Check that the device monitor allows this device to attach */
    if (devmon_allow_attach(device))
    {
        DBG_I(DJOS_DRIVER, ("JDRV: Device attach prohibited by " 
            "the device monitor\n"));
        j_device_set_nameunit(device, "forbidden", forbidden_unit++);
        return JEACCES;
    }
#endif
   
    /* Alloc softc if driver needs it */
    if (best->softsize)
    {
        device->softc = jmalloc(best->softsize, M_ZERO);
        if (!device->softc)
        {
            DBG_E(DJOS_DRIVER, ("JDRV: Cannot create softc size %d\n", 
                best->softsize));
            j_device_set_nameunit(device, "unknown", unknown_unit++);
            return JENOMEM;
        }
    }
   
    /* Give it a nice name */
    j_device_set_nameunit(device, best->name, best->curr_unit++);
    
    rc = best->methods->attach(device);
    if (rc)
    {
        DBG_E(DJOS_DRIVER, ("JDRV: Could not attach to %s, error %d\n",
            best->name, rc));

        jfree(device->softc);
        device->softc = NULL;
        j_device_set_nameunit(device, "unknown", unknown_unit++);
        return rc;
    }

    DBG_V(DJOS_DRIVER, ("JDRV: Attached %p to %s\n", device, best->name));

#ifdef CONFIG_JHOST
    /* Notify device manager that a device attached */
    devmon_handle_attached(device, device->parent);
#endif

    best->usage_count++;
    device->jdrv = best;
    device->type = DRIVER_USB;

    return 0;
}

jresult_t jdriver_attach(jint_t type, jdevice_t device, jint_t id, void *arg)
{
    jresult_t rc;
       
    switch (type)
    {
    case DRIVER_CONTROLLER:
        rc = jdriver_controller_attach(device, id, (jbus_h)arg);
        break;
        
    case DRIVER_USB:
        rc = jdriver_usb_attach(device);
        break;

    default:
        rc = JEINVAL;
        DBG_E(DJOS_DRIVER, ("JDRV: Attaching unknown dev type %d\n", type));
    }

    if (!rc)
        TAILQ_INSERT_TAIL(&(device->jdrv->dev_list), device, next);

    return rc;
}

jresult_t j_device_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg)
{
    /* If the device is not attached or the driver doesnt support ioctls */
    if (!dev->jdrv || !dev->jdrv->methods->ioctl)
        return JEINVAL;

    return dev->jdrv->methods->ioctl(dev, ioctl, arg);
}

jresult_t j_device_suspend(jdevice_t dev)
{
    jresult_t rc = 0;
    
    /* If the device is attached and the driver supports suspend */
    if (dev->jdrv && dev->jdrv->methods->suspend)
        rc = dev->jdrv->methods->suspend(dev);
    
    return rc;
}

jresult_t j_device_resume(jdevice_t dev)
{
    jresult_t rc = 0;
    
    /* If the device is attached and the driver supports resume */
    if (dev->jdrv && dev->jdrv->methods->resume)
        rc = dev->jdrv->methods->resume(dev);

    return rc;
}

