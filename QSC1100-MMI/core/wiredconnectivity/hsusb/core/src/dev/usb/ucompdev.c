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
#include <jusb.h>
#include <usbdi.h>
#include <usbdi_util.h>
#include <usbdivar.h>

typedef struct {
    jdevice_t   dev;
    juint8_t     ifnum;
} device_info_t;

struct ucompdev_softc {
    jdevice_t     sc_dev;
    device_info_t *subdevs;
    juint8_t       subdev_count;
    juint8_t       subdev_active;
};

/**
 * Function name:  ucompdev_probe
 * Description: Checks if the usb device can be handled by this driver
 * Parameters: 
 *     @self: Pointer to the device's structure
 *
 * Return value: The match level
 * Scope: local
 **/
static jresult_t ucompdev_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);

    /* We only match wildcard devices, if interface or config is set
     * this is a per-interface match, not for us */
    if (uaa->ifaceno != UHUB_UNK_INTERFACE ||
        uaa->configno != UHUB_UNK_CONFIGURATION)
    {
        DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Probe %p not ours [%d:%x]\n", 
            self, uaa->configno, uaa->ifaceno));

        return (UMATCH_NONE);
    }

    DBG_I(DCOMPOSITE_ATTACH, ("UCOMP: Probe %p ok\n", self));

    /* We are always ready to try if no one else wants to */
    return (UMATCH_GENERIC);
}

static char devinfo[1024];

#ifdef CONFIG_JHOST_AUDIO
static void claim_class_specfic_ifaces(usbd_device_handle dev, juint8_t ifnum)
{
    audio_control_iface_desc_t *adesc;
    usbd_interface_handle iface = &dev->ifaces[ifnum];
    juint_t i;

    /* Not Audio device */
    if (iface->idesc->bInterfaceClass != UICLASS_AUDIO || 
        iface->idesc->bInterfaceSubClass != UISUBCLASS_AUDIOCONTROL)
    {
        return;
    }
        
    /* Audio Control Interface (release 2.0) use standard IAD */
    if (iface->idesc->bInterfaceProtocol)
        return;

    /* Class-Specific Audio Control Interface Header Descriptor */
    adesc = (audio_control_iface_desc_t *)((juint8_t *)iface->idesc + 
        iface->idesc->bLength);
           
   for (i = 0; i < adesc->bInCollection; i++)
   {
       DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Interface %d assigned to Audio\n",
           adesc->baInterfaceNr[i]));

       dev->ifaces[adesc->baInterfaceNr[i]].collection.used = 1;
       dev->ifaces[adesc->baInterfaceNr[i]].collection.head = ifnum;
   }
}
#endif

/**
 * Function name:  attach_try_config
 * Description: Attempt to attach the devices using a specific config
 * Parameters: 
 *     @self: Pointer to the device's structure
 *     @confi: Index of the configuration to try
 *
 * Return value: 0 if any interface was attached or an error otherwise
 * Scope: local
 **/
static jresult_t attach_try_config(jdevice_t self, juint8_t confi)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usbd_device_handle dev = uaa->device;
    usbd_status err;
    jdevice_t parent = uaa->parent;
    jdevice_t bdev = NULL;
    const char *err_str = "";
    int i;
#ifdef CONFIG_JOTG
    jbool_t unsupported_iface = 0;
#endif

    DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Attach: trying config %d\n", confi));
    
    err = usbd_set_config_index(dev, confi, 1);
    if (err) 
    {
#ifdef JDEBUG
        err_str = usbd_errstr(err);
#endif
        jprintf("%s: port %d, set config failed %s\n",
            j_device_get_nameunit(parent), uaa->port, err_str);

        return JEINVAL;
    }
   
    /* Set config above updates the ifaces struct */
    uaa->nifaces = dev->cdesc->bNumInterface;

    /* We reuse the same uaa for all child devices since 
     * this is basicaly the same device, only change the 
     * config & iface numbers for correct probe_and_attach */
    uaa->configno = dev->cdesc->bConfigurationValue;

    DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Trying %d interfaces\n",
        uaa->nifaces));

    /* Allocate array to hold sub devs data */
    sc->subdevs = jmalloc(uaa->nifaces * sizeof(device_info_t), 0);
    if (!sc->subdevs) 
        return JENOMEM;

    sc->subdev_count = 0;
    sc->subdev_active = 0;
    
    for (i = 0; i < uaa->nifaces; i++) 
    {
#ifdef CONFIG_JHOST_AUDIO
        /* Skip interface that is part of a class specific IAD */
        if (dev->ifaces[i].collection.used)
            continue;
#endif

        DBG_X(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Trying interface %d "
            "so far bound %d\n", i, sc->subdev_count));
        
        /* Check if we already created a child and just didn't use it */
        if (!bdev)
        {
            /* Create child for this interface */
            bdev = jdevice_add(parent);
            if (!bdev) 
            {
                DBG_E(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Cannot create "
                    "a new child.\n"));
                break;
            }
        }

#ifdef CONFIG_JOTG
        if (dev->bus->otg_port)
        {
            otg_peripheral_state_t support_state = OTG_PERIPH_NOT_SUPPORTED;

            err = usbd_check_iface_support(dev, dev->ifaces[i].idesc, 
                &support_state);
            if (err)
                break;
            if (support_state != OTG_PERIPH_SUPPORTED)
            {
                DBG_W(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Interface %d is not "
                    "supported\n", i));
                unsupported_iface = 1;
                continue;
            }
        }
#endif
        /* Update attach args and save them in the new device */
        uaa->iface = &dev->ifaces[i];
        uaa->ifaceno = dev->ifaces[i].idesc->bInterfaceNumber;
        j_device_set_ivars(bdev, uaa);

        if (j_device_probe_and_attach(bdev, (void *)dev) == 0)
        {
#ifdef CONFIG_JHOST_AUDIO
            /* Check interface for class specific IAD */
            claim_class_specfic_ifaces(dev, i);
#endif

            /* Save the new dev */
            sc->subdevs[sc->subdev_count].dev = bdev;
            sc->subdevs[sc->subdev_count].ifnum = i;
            
            /* Count the number of attached ifaces */
            sc->subdev_count++;

            /* Prevent the device from accessing the UAA after probe & attach */
            j_device_set_ivars(bdev, NULL);

            /* Make loop alloc new device */
            bdev = NULL;

            DBG_X(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Bound iface %d "
                "to driver\n", i));
        }
    }

    /* Remove the last created child.  It is unused */
    if (bdev)
        jdevice_delete(bdev);

    if (sc->subdev_count != 0) 
    {
        DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Attach done. Bound %d interfaces\n",
            sc->subdev_count));
        
        sc->subdev_active = sc->subdev_count;

#ifdef CONFIG_JOTG
        dev->otg_support = (unsupported_iface)? OTG_PERIPH_PARTIAL_SUPPORTED : 
            OTG_PERIPH_SUPPORTED;
#endif

        return 0;
    }

    DBG_I(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Could not bind any ifaces\n"));

    jfree(sc->subdevs);
    sc->subdevs = NULL;

    return JENOENT;
}

/**
 * Function name:  ucompdev_attach
 * Description: Attempt to attach this driver to a device
 * Parameters: 
 *     @self: Pointer to the device's structure
 *
 * Return value: 0 on success or an error otherwise
 * Scope: local
 **/
static jresult_t ucompdev_attach(jdevice_t self)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    usb_device_descriptor_t *dd = usbd_get_device_descriptor(uaa->device);
    usbd_device_handle dev = uaa->device;
    juint8_t confi;

    DBG_I(DCOMPOSITE_ATTACH, ("UCOMP: Attach %p\n", self));
    sc->sc_dev = self;
    
    usbd_devinfo(dev, 1, devinfo, sizeof(devinfo));
    j_device_set_desc(self, devinfo);
    device_printf(self, ("%s\n", devinfo));

    DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Attach: Trying %d configurations\n",
        dd->bNumConfigurations));

    uaa->parent = self;
    
    /* Try for each configuration */
    for (confi = 0; confi < dd->bNumConfigurations; confi++) 
    {
        if (attach_try_config(self, confi) == 0)
            return 0;
    }
    
    DBG_V(DCOMPOSITE_ATTACH, ("UCOMP: Attach: bound no interface drivers\n"));
   
    /* No interfaces were attached in any of the configurations. */
    if (dd->bNumConfigurations > 1) /* don't change if only 1 config */
        usbd_set_config_index(dev, 0, 0);

    return JENOENT;
}

/**
 * Function name:  ucompdev_detach
 * Description: Detach the driver from a device
 * Parameters: 
 *     @self: Pointer to the device's structure
 *
 * Return value: Always 0 for success
 * Scope: local
 **/
static jresult_t ucompdev_detach(jdevice_t self)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    const char *name = j_device_get_nameunit(self);
    juint8_t i;
        
    DBG_V(DCOMPOSITE_DETACH, ("UCOMP: Detaching %s\n", name));

    if (!sc->subdevs)
        return 0;

    for (i = 0; i < sc->subdev_count; i++)
    {
        jprintf("%s: at %s (iface %d) disconnected\n", 
            j_device_get_nameunit(sc->subdevs[i].dev),
            name, sc->subdevs[i].ifnum);

        j_device_detach(sc->subdevs[i].dev);
        jdevice_delete(sc->subdevs[i].dev);
    }
    jfree(sc->subdevs);

    DBG_X(DCOMPOSITE_DETACH, ("UCOMP: Detach finished\n"));

    return 0;
}

/**
 * Function name:  ucompdev_suspend
 * Description: Suspend the device
 * Parameters: 
 *     @self: Pointer to the device's structure
 *
 * Return value: 0 on success or an error otherwise
 * Scope: local
 **/
static jresult_t ucompdev_suspend(jdevice_t self)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    jresult_t rc = 0;
    jint_t i, j;
    
    DBG_I(DCOMPOSITE_POWER, ("UCOMP: %s Suspend request\n",
        j_device_get_nameunit(self)));

    /* Suspend all ports */
    for (i = 0; i < sc->subdev_count && !rc; i++)
        rc = j_device_suspend(sc->subdevs[i].dev);

    if (rc)
        goto Error;

    sc->subdev_active = 0;
    
    return 0;

Error:

    DBG_E(DCOMPOSITE_POWER, ("UCOMP: Failed suspending %d err %d\n",
        i, rc));

    /* Resume all those we suspended */
    for (j = i - 1; j >= 0; j--)
        j_device_resume(sc->subdevs[j].dev);
    
    return rc;
}

/**
 * Function name:  ucompdev_resume
 * Description: Resume a device
 * Parameters: 
 *     @self: Pointer to the device's structure
 *
 * Return value: 0 on success or an error otherwise
 * Scope: local
 **/
static jresult_t ucompdev_resume(jdevice_t self)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    jresult_t rc = 0;
    jint_t i;
    
    DBG_I(DCOMPOSITE_POWER, ("UCOMP: %s Resume request\n",
        j_device_get_nameunit(self)));

    for (i = 0; i < sc->subdev_count; i++)
    {
        rc = j_device_resume(sc->subdevs[i].dev);

        /* If it didn't wakeup, try the others */
        if (rc)
            continue;

        /* Count active devices */
        sc->subdev_active ++;
    }

    /* If we managed to resume something, its a great success ! */
    if (sc->subdev_active)
        return 0;
    
    /* If no devices woke up, return the error the last one given us */
    return rc;
}

static jresult_t selective_suspend(jdevice_t self, jdevice_t child)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    int i;

    /* 1. Suspend the child's port */

    /* Find out which device to suspend */
    for (i = 0; i < sc->subdev_count && sc->subdevs[i].dev != child; i++)
        ;

    if (i == sc->subdev_count)
        return JEINVAL;

    DBG_I(DCOMPOSITE_POWER, ("DCOMP: [%s] Selective suspend %s\n",
        j_device_get_nameunit(self), j_device_get_nameunit(child)));

    sc->subdev_active--;

    /* 2. Suspend self if nothing is running */

    /* If there are more active children, stay alive */
    if (sc->subdev_active)
        return 0;

    /* 3. Notify parent if self suspend happened */

    DBG_I(DCOMPOSITE_POWER, ("DCOMP: [%s] Self suspend\n", 
        j_device_get_nameunit(self)));

    /* Notify parent that we are inactive and want to suspend */
    j_device_ioctl(j_device_get_parent(self), 
        DRV_IOCTL_PWR_CHILD_SUSPEND, (void *)self);

    return 0;
}

static jresult_t selective_resume(jdevice_t self, jdevice_t child)
{
    struct ucompdev_softc *sc = j_device_get_softc(self);
    jresult_t rc;
    int i;

    /* Find out which device to resume */
    for (i = 0; i < sc->subdev_count && sc->subdevs[i].dev != child; i++)
        ;

    if (i == sc->subdev_count)
        return JEINVAL;

    DBG_I(DCOMPOSITE_POWER, ("DCOMP: [%s] Selective resume %s\n",
        j_device_get_nameunit(self), j_device_get_nameunit(child)));

    /* Resume ourselves if we are suspended */
    if (sc->subdev_active == 0)
    {
        DBG_I(DCOMPOSITE_POWER, ("DCOMP: [%s] Self resume\n",
            j_device_get_nameunit(self)));

        /* Notify parent that we are trying to resume */
        rc = j_device_ioctl(j_device_get_parent(self), 
            DRV_IOCTL_PWR_CHILD_RESUME, (void *)self);
        if (rc)
            return rc;
    }

    sc->subdev_active ++;

    return 0;
}

static jresult_t ucompdev_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    switch (ioctl)
    {
    case DRV_IOCTL_PWR_CHILD_SUSPEND:
        return selective_suspend(self, (jdevice_t)arg);

    case DRV_IOCTL_PWR_CHILD_RESUME:
        return selective_resume(self, (jdevice_t)arg);

    default:
        break;
    }

    DBG_E(DCOMPOSITE_ATTACH, ("UCOMP: Unknown IOCTL %x\n", ioctl));
    return JEINVAL;
}

DRIVER_USB_DECLARE(ucompdev)

