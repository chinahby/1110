/*==============================================================================

                      High Speed USB Sample Loopback Class Driver
  
GENERAL DESCRIPTION
Implementation of loopback class Driver.
This includes implementation of the API functions defined in hsu_cd_sample_loop.h.
      
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: 
$DateTime: 


when      who     what, where, why
--------  ---     ----------------------------------------------------------
06/01/08  tk      created file

==============================================================================*/

#include <jos.h>
#include <jusb.h>
#include <usbdi.h>
#include <usbdi_util.h>
#include <jusb_chp9.h>

typedef enum
{
    NOT_ATTACHED = 0,
    ATTACHED,		/* Device is attached */
    ENABLED		/* Device is Attached, and loopback transfer 
    			   initiated */
} hsu_cd_sample_loopback_status;

struct hsu_cd_sample_loopback_softc;

/* Transfer context */
typedef struct hsu_cd_sample_loopback_xfer
{
    usbd_xfer_handle  xfer;	/* CORE transfer context */
    juint8_t           *buf;	/* Data buffer */
    struct hsu_cd_sample_loopback_softc* sc; /* Device context */
} hsu_cd_sample_loopback_xfer_t;

/* hsu_cd_sample_loopback software context */
#define LOOPBACK_BUFFER_SIZE 64
typedef struct hsu_cd_sample_loopback_softc
{
    jdevice_t			sc_dev;		/* base device */
    usbd_interface_handle	iface;		/* CORE interface handle */
    juint8_t			bulk_in_ep_addr;
    usbd_pipe_handle		bulk_pipe_in;	/* CORE bulk in pipe handle */
    juint8_t			bulk_out_ep_addr;
    usbd_pipe_handle		bulk_pipe_out;  /* CORE bulk out pipe handle */
    usbd_device_handle		dev;		/* CORE device handle */
    hsu_cd_sample_loopback_status		status;
    hsu_cd_sample_loopback_xfer_t            *request_in;
    hsu_cd_sample_loopback_xfer_t            *request_out;

    /* Cyclic data (0 to 255) for checksum */
    juint8_t data;
} hsu_cd_sample_loopback_softc_t;

/* Driver methods - need to be implemented for driver registration with the 
 * core */
static jresult_t hsu_cd_sample_loopback_probe(jdevice_t device);
static jresult_t hsu_cd_sample_loopback_attach(jdevice_t self);
static jresult_t hsu_cd_sample_loopback_detach(jdevice_t self);
static jresult_t hsu_cd_sample_loopback_suspend(jdevice_t self);
static jresult_t hsu_cd_sample_loopback_resume(jdevice_t self);
static jresult_t hsu_cd_sample_loopback_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg);

/* Allocation of hsu_cd_sample_loopback transfer context */
static hsu_cd_sample_loopback_xfer_t *hsu_cd_sample_loopback_alloc_xfer(hsu_cd_sample_loopback_softc_t *sc);
static void hsu_cd_sample_loopback_free_xfer(hsu_cd_sample_loopback_xfer_t *lb_xfer);

static jresult_t hsu_cd_sample_loopback_start(hsu_cd_sample_loopback_softc_t *sc);
static void hsu_cd_sample_loopback_stop(hsu_cd_sample_loopback_softc_t *sc);

static jresult_t hsu_cd_sample_loopback_write(hsu_cd_sample_loopback_softc_t *sc);
static jresult_t hsu_cd_sample_loopback_read(hsu_cd_sample_loopback_softc_t *sc);
static void hsu_cd_sample_loopback_write_complete (usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status);
static void hsu_cd_sample_loopback_read_complete (usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status);

/**
 * Function name:  hsu_cd_sample_loopback_alloc_xfer
 * Description:    Allocate transfer handle for hsu_cd_sample_loopback driver. Allocates the
 *                 data buffer as well.
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context 
 *
 * Return value:   Pointer to new allocated hsu_cd_sample_loopback_xfer_t, or NULL on failure
 * Scope:          Local
 **/
static hsu_cd_sample_loopback_xfer_t *hsu_cd_sample_loopback_alloc_xfer(hsu_cd_sample_loopback_softc_t *sc)
{
    hsu_cd_sample_loopback_xfer_t *lb_xfer;

    lb_xfer = (hsu_cd_sample_loopback_xfer_t *)jmalloc(sizeof(hsu_cd_sample_loopback_xfer_t), M_ZERO);
    if (!lb_xfer)
        goto Error;

    /* Allocate core transfer context */
    lb_xfer->xfer = usbd_alloc_xfer(sc->dev);
    if (!lb_xfer->xfer)
        goto Error;

    /* Save context to be used upon transfer completion */
    lb_xfer->sc = sc;
    
    /* Allocate data buffer to be used by the core */
    lb_xfer->buf = usbd_alloc_buffer(lb_xfer->xfer, LOOPBACK_BUFFER_SIZE);
    if (!lb_xfer->buf)
        goto Error;
    
    return lb_xfer;

Error:
    hsu_cd_sample_loopback_free_xfer(lb_xfer);
    return NULL;
}

/**
 * Function name:  hsu_cd_sample_loopback_free_xfer
 * Description:    Free a pre-allocated hsu_cd_sample_loopback transfer handle
 * Parameters:
 *     @lb_xfer:   (IN) hsu_cd_sample_loopback transfer handle 
 *
 * Return value:   None
 * Scope:          Local
 **/
static void hsu_cd_sample_loopback_free_xfer(hsu_cd_sample_loopback_xfer_t *lb_xfer)
{
    if (lb_xfer)
    {
        if (lb_xfer->buf)
            usbd_free_buffer(lb_xfer->xfer);
        if (lb_xfer->xfer)
            usbd_free_xfer(lb_xfer->xfer);
        jfree(lb_xfer);
    }
}

/**
 * Function name:  hsu_cd_sample_loopback_setup_xfer
 * Description:    Set up a hsu_cd_sample_loopback transfer.
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context
 *     @is_in:     (IN) Transfer direction
 *
 * Return value:   None
 * Scope:          Local
 **/
static void hsu_cd_sample_loopback_setup_xfer(hsu_cd_sample_loopback_softc_t *sc, jbool_t is_in)
{
    hsu_cd_sample_loopback_xfer_t *lb_xfer;

    if (!is_in)
    {
        lb_xfer = sc->request_out;

	/* Fill the data to be sent to the device */
        j_memset(lb_xfer->buf, sc->data, LOOPBACK_BUFFER_SIZE);
	sc->data = (sc->data + 1) % 256;

	/* Setup the core write transfer */
        usbd_setup_xfer(lb_xfer->xfer, sc->bulk_pipe_out, lb_xfer, lb_xfer->buf,
            LOOPBACK_BUFFER_SIZE, USBD_NO_COPY, 0, hsu_cd_sample_loopback_write_complete);
    }
    else
    {
        lb_xfer = sc->request_in;

	/* Setup the core read transfer */
        usbd_setup_xfer(lb_xfer->xfer, sc->bulk_pipe_in, lb_xfer, lb_xfer->buf, 
            LOOPBACK_BUFFER_SIZE, USBD_NO_COPY, 0, hsu_cd_sample_loopback_read_complete);
    }
}

/**
 * Function name:  hsu_cd_sample_loopback_attach
 * Description:    Setup the driver for loopback operation. Called after
 *                 hsu_cd_sample_loopback_probe was called and found that the device fit to
 *                 hsu_cd_sample_loopback specific criteria.
 * Parameters:     
 *     @self:      (IN) Device handle 
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_attach(jdevice_t self)
{
    usb_endpoint_descriptor_t *ed;
    juint8_t num_of_endpoints, i;
    hsu_cd_sample_loopback_softc_t *sc = j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);

    /* Save CORE device handle for later use */
    sc->sc_dev = self;

    /* Save CORE device and interface contexts */
    sc->dev = uaa->device;
    sc->iface = uaa->iface;
    sc->bulk_in_ep_addr = 0;
    sc->bulk_out_ep_addr = 0;
    sc->status = NOT_ATTACHED;

    DBG_I(DVENDOR, ("LOOPBACK: Attach Started\n"));

    if (usbd_endpoint_count(sc->iface, &num_of_endpoints))
    {
        DBG_E(DVENDOR,("LOOPBACK ATTACH: Invalid Interface Descriptor\n"));
        goto Error;
    }

    /* Look for two bulk endpoints, in and out direction */
    for (i = 0; i < num_of_endpoints; ++i)
    {
	/* Get endpoint descriptor from the core */
        ed = usbd_interface2endpoint_descriptor(sc->iface, i);
        if (!ed)
        {
            DBG_E(DVENDOR, ("LOOPBACK ATTACH: could not read endpoint "
                "descriptor num %d / %d\n", i, num_of_endpoints));
            goto Error;
        }
        
        if (UE_GET_XFERTYPE(ed->bmAttributes) == UE_BULK)
        {
            if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN)
                sc->bulk_in_ep_addr = ed->bEndpointAddress;
            else 
                sc->bulk_out_ep_addr = ed->bEndpointAddress;
        }
    }

    if ((sc->bulk_in_ep_addr == 0) || (sc->bulk_out_ep_addr == 0))
    {
        DBG_E(DVENDOR, ("LOOPBACK ATTACH: The loopback device %s BULK IN and "
            "%s BULK OUT pipes not allowed\n",
            sc->bulk_in_ep_addr == 0xff ? "without" : "with",
            sc->bulk_out_ep_addr == 0xff ? "without" : "with"));
        goto Error;
    }
  
    sc->request_in = hsu_cd_sample_loopback_alloc_xfer(sc);
    if (!sc->request_in)
    {
        DBG_E(DVENDOR, ("LOOPBACK ATTACH: Out of memory \n"));
        goto Error;
    }

    sc->request_out = hsu_cd_sample_loopback_alloc_xfer(sc);
    if (!sc->request_out)
    {
        DBG_E(DVENDOR, ("LOOPBACK ATTACH: Out of memory \n"));
        goto Error;
    }

    sc->status = ATTACHED; 

    /* Start loopback transfers sequence */
    if (hsu_cd_sample_loopback_start(sc))
    {
	DBG_E(DVENDOR, ("LOOPBACK ATTACH: Failed to start loopback "
            "sequence\n"));
	goto Error;
    }

    return 0;

Error:

    hsu_cd_sample_loopback_detach(self);

    return JENXIO;
}

/**
 * Function name:  hsu_cd_sample_loopback_detach
 * Description:    Performs clean up upon device disconnect
 * Parameters:
 *     @self:      (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_detach(jdevice_t self)
{
    hsu_cd_sample_loopback_softc_t *sc = j_device_get_softc(self);
    
    /* Abort & Close all the pipes */
    hsu_cd_sample_loopback_stop(sc);

    /* Free all allocated transfers handles */
    if (sc->request_in)
    {
        hsu_cd_sample_loopback_free_xfer(sc->request_in);
        sc->request_in = NULL;
    }

    if (sc->request_out)
    {
        hsu_cd_sample_loopback_free_xfer(sc->request_out);
        sc->request_out = NULL;
    }
    
    sc->sc_dev = NULL;
    sc->dev = NULL;
    sc->iface = NULL;
    sc->bulk_in_ep_addr = 0;
    sc->bulk_out_ep_addr = 0;

    sc->status = NOT_ATTACHED;

    return 0;
}

/**
 * Function name:  hsu_cd_sample_loopback_probe
 * Description:    Check if hsu_cd_sample_loopback driver can handle the device. Called
 *                 prior to hsu_cd_sample_loopback_attach callback.
 * Parameters:
 *     @self:      (IN) Device handle
 *
 * Return value:   Matching level. UMATCH_NONE for no matching or error, 
 *                 otherwise UMATCH_GENERIC (or one of UMATCH_* values as
 *                 defined in usbdi.h
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_probe(jdevice_t self)
{
    usb_endpoint_descriptor_t *ed1, *ed2;
    usb_interface_descriptor_t *id;
    jresult_t rt = UMATCH_NONE;
    struct usb_attach_arg *uaa;
    
    DBG_I(DVENDOR, ("hsu_cd_sample_loopback PROBE: Probe Started\n"));

    /* We use the attach arguments for matching criteria */
    uaa = j_device_get_ivars(self);
    
    /* we attach to interfaces only */
    if (!uaa->iface)
        goto Exit;

    /* Check that this is an interface with bulk in and bulk out endpoints */
    id = usbd_get_interface_descriptor(uaa->iface);

    if(!(id->bInterfaceClass == USB_CLASS_VENDOR_SPEC) || !(id->bInterfaceSubClass == USB_CLASS_VENDOR_SPEC))
    {
      goto Exit;
    }
    
    if (!id)
    {
        DBG_E(DVENDOR, ("hsu_cd_sample_loopback PROBE: could not read interface "
            "descriptors\n"));
        goto Exit;
    }

    if (id->bNumEndpoints != 2)
    {
        DBG_E(DVENDOR,("hsu_cd_sample_loopback PROBE: could not match interface with "
            "more then two endpoints\n"));
        goto Exit;
    }

    
    /* Get both endpoint descriptors from the CORE */
    ed1 = usbd_interface2endpoint_descriptor(uaa->iface, 0);
    ed2 = usbd_interface2endpoint_descriptor(uaa->iface, 1);
    if (!ed1 || !ed2) 
    {
        DBG_E(DVENDOR, ("hsu_cd_sample_loopback PROBE: could not read endpoint "
            "descriptors\n"));
        goto Exit;
    }

    /* Check for match: two bulk endpoint, 1 IN and 1 OUT */
    if (((ed1->bmAttributes & UE_XFERTYPE) == UE_BULK &&
        (ed2->bmAttributes & UE_XFERTYPE) == UE_BULK) &&
        ((UE_GET_DIR(ed1->bEndpointAddress) == UE_DIR_IN && 
        UE_GET_DIR(ed2->bEndpointAddress) == UE_DIR_OUT) || 
        (UE_GET_DIR(ed1->bEndpointAddress) == UE_DIR_OUT && 
        UE_GET_DIR(ed2->bEndpointAddress) == UE_DIR_IN))) 
    {
	/* Return the lowest matching rate in case there is more suitable
	 * driver for this device */
	      /* Qualcomm modifed : force it registed as high priority */
     //        rt = UMATCH_GENERIC; 
        rt = UMATCH_VENDOR_PRODUCT_REV; 
    }

Exit:
    return rt;
}

/**
 * Function name:  hsu_cd_sample_loopback_start
 * Description:    Start the loopback cycle. Open the relevant pipes, and issue
 *                 the first write to the device.
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_start(hsu_cd_sample_loopback_softc_t *sc)
{
    jresult_t rc = JEINVAL;
    
    if (sc->status != ATTACHED)
    {
        DBG_E(DVENDOR,("hsu_cd_sample_loopback_start: device is\n", 
            sc->status == NOT_ATTACHED ? "not attached yet" : 
            "already enabled"));
        goto Error;
    }

    /* Open the pipes for transfers */
    if (usbd_open_pipe(sc->iface, sc->bulk_in_ep_addr, USBD_EXCLUSIVE_USE, 
        &sc->bulk_pipe_in))
    {
        DBG_E(DVENDOR,("hsu_cd_sample_loopback_start: Unable to open BULK IN pipe\n"));
        goto Error;
    }

    if (usbd_open_pipe(sc->iface, sc->bulk_out_ep_addr, USBD_EXCLUSIVE_USE, 
        &sc->bulk_pipe_out))
    {
        DBG_E(DVENDOR,("hsu_cd_sample_loopback_start: Unable to open BULK OUT pipe\n"));
        goto Error;
    }

    sc->status = ENABLED;
    sc->data = 0;

    /* Issue the first write transfer */
    rc = hsu_cd_sample_loopback_write(sc);

Error:
    if(rc)
	hsu_cd_sample_loopback_stop(sc);
    return rc;
}

/**
 * Function name:  hsu_cd_sample_loopback_stop
 * Description:    Stop the loopback cycle. Abort all active transfers and close
 *                 the relevant pipes.
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context 
 *
 * Return value:   None
 * Scope:          Local
 **/
static void hsu_cd_sample_loopback_stop(hsu_cd_sample_loopback_softc_t *sc)
{
    if (sc->status == ENABLED)
        sc->status = ATTACHED;
    
    /* Abort all transfers and close the pipes */
    if (sc->bulk_pipe_in)
    {
        usbd_abort_pipe(sc->bulk_pipe_in);
        usbd_close_pipe(sc->bulk_pipe_in);
        sc->bulk_pipe_in = NULL;
    }
        
    if (sc->bulk_pipe_out)
    {
        usbd_abort_pipe(sc->bulk_pipe_out);
        usbd_close_pipe(sc->bulk_pipe_out);
        sc->bulk_pipe_out = NULL;
    }
}

/**
 * Function name:  hsu_cd_sample_loopback_read_complete
 * Description:    Completion callback for hsu_cd_sample_loopback read transfer. 
 *                 Verify that the data match to the loopback current data, 
 *                 and issue the next write transfer.
 * Parameters:
 *     @xfer:      (IN) The complete CORE transfer handle
 *     @priv:      (IN) Private context of hsu_cd_sample_loopback, as set in
 *                      hsu_cd_sample_loopback_setup_xfer 
 *     @status:    (IN) Transfer status
 *
 * Return value:   None
 * Scope:          Local
 **/
static void hsu_cd_sample_loopback_read_complete (usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status)
{
    /* Restore the transfer context */
    hsu_cd_sample_loopback_xfer_t *lb_xfer = (hsu_cd_sample_loopback_xfer_t *)priv;
    hsu_cd_sample_loopback_softc_t *sc = lb_xfer->sc;
    juint32_t act_transfered_size = 0;
    jresult_t rc = 0;
    jint_t i;
    
    if (status)
    {
        DBG_W(DVENDOR, ("hsu_cd_sample_loopback_read_complete: failed on read, status=%d\n",
            status));
    }

    /* Get the actual transfer size */
    usbd_get_xfer_status(xfer, NULL, NULL, &act_transfered_size, NULL);

    /* Check that the data we read match the data we sent */
    for (i = 0; i < act_transfered_size; i++)
    {
        if (sc->request_in->buf[i] != (sc->data == 0 ? 255 : sc->data - 1))
        {
            DBG_E(DVENDOR, ("hsu_cd_sample_loopback_read_complete: checksum error on %d "
                "byte", i));
            break;
        }
    }

    /* Issue the next write */
    if (sc->status == ENABLED)
        rc = hsu_cd_sample_loopback_write(sc);
    
    if (rc)
    {
        DBG_E(DVENDOR, ("hsu_cd_sample_loopback_read_complete: failed to start new write "
	    "request rc=%d!\n",rc));
    }
}

/**
 * Function name:  hsu_cd_sample_loopback_read
 * Description:    Issue a read transfer to the loopback device
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context 
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_read(hsu_cd_sample_loopback_softc_t *sc)
{
    jresult_t rc;
	
    hsu_cd_sample_loopback_setup_xfer(sc, 1);

    rc = usbd_transfer(sc->request_in->xfer);
    if (rc && (rc != USBD_IN_PROGRESS))
    {
	DBG_E(DVENDOR,("hsu_cd_sample_loopback_read: Unable to start read transfer %d\n", 
	    rc));
	return rc;
    }

    return 0;
}

/**
 * Function name:  hsu_cd_sample_loopback_write_complete
 * Description:    Completion callback for hsu_cd_sample_loopback read transfer. 
 *                 Verify that the data match to the loopback current data, 
 *                 and issue the next write transfer.
 * Parameters:
 *     @xfer:      (IN) The complete CORE transfer handle
 *     @priv:      (IN) Private context of hsu_cd_sample_loopback, as set in
 *     @status:    (IN) Transfer status
 *
 * Return value:   None
 * Scope:          Local
 **/
static void hsu_cd_sample_loopback_write_complete (usbd_xfer_handle xfer, 
    usbd_private_handle priv, usbd_status status)
{
    /* Restore the transfer context */
    hsu_cd_sample_loopback_xfer_t *lb_xfer = (hsu_cd_sample_loopback_xfer_t *)priv;
    hsu_cd_sample_loopback_softc_t *sc = lb_xfer->sc;
    
    if (status)
    {
        DBG_E(DVENDOR, ("hsu_cd_sample_loopback_write_complete: failed on write.status=%d\n",
            status));
    }

    /* Issue read transfer to read the previously written data */
    if (sc->status == ENABLED && hsu_cd_sample_loopback_read(sc))
    {
	DBG_E(DVENDOR, ("hsu_cd_sample_loopback_write_complete: Failed to issue read "
	    "transfer\n"));
    }
}

/**
 * Function name:  hsu_cd_sample_loopback_write
 * Description:    Issue a write transfer to the loopback device
 * Parameters:
 *     @sc:        (IN) hsu_cd_sample_loopback context 
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_write(hsu_cd_sample_loopback_softc_t *sc)
{
    jresult_t rc = JEIO;

    hsu_cd_sample_loopback_setup_xfer(sc, 0);
    
    rc = usbd_transfer(sc->request_out->xfer);
    if (rc && (rc != USBD_IN_PROGRESS))
    {
        DBG_E(DVENDOR,("hsu_cd_sample_loopback_write: Unable to start write transfer %d \n",
            rc));
        return rc;
    }

    return 0;
}

/**
 * Function name:  hsu_cd_sample_loopback_suspend
 * Description:    Handles SUSPEND power change. Stop hsu_cd_sample_loopback operation 
 * Parameters:
 *     @self:      (IN) Device handle
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_suspend(jdevice_t self)
{
    hsu_cd_sample_loopback_softc_t *sc = j_device_get_softc(self);

    hsu_cd_sample_loopback_stop(sc);

    return 0;
}

/**
 * Function name:  hsu_cd_sample_loopback_resume
 * Description:    Handles RESUME power change. Resume hsu_cd_sample_loopback operation
 * Parameters:
 *     @self:      (IN) Device handle 
 *
 * Return value:   0 on success, otherwise an error code 
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_resume(jdevice_t self)
{
    hsu_cd_sample_loopback_softc_t *sc = j_device_get_softc(self);

    hsu_cd_sample_loopback_start(sc);

    return 0;
}

/**
 * Function name:  hsu_cd_sample_loopback_ioctl
 * Description:    Handles driver specific IOCTL
 * Parameters:
 *     @self:      (IN) Device handle 
 *     @ioctl:     (IN) IO control code 
 *     @arg:       (IN/OUT) Specific data structure for ioctl
 *
 * Return value:   0 on success, otherwise an error code
 * Scope:          Local
 **/
static jresult_t hsu_cd_sample_loopback_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    DBG_E(DVENDOR, ("hsu_cd_sample_loopback: Unsupported IOCTL %x\n", ioctl));
    return JEINVAL;
}

/* Register the driver with the core:
 * Defines hsu_cd_sample_loopback_init function, and initializes driver_method structure.
 */
DRIVER_USB_DECLARE(hsu_cd_sample_loopback)
