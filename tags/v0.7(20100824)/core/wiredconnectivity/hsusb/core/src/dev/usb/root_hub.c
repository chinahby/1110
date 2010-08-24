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

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

#include <root_hub.h>

static void root_hub_intr_enable(void *arg)
{
    root_hub_softc_t *sc = (root_hub_softc_t *)arg;

    DBG_X(DHOST_ROOT, ("ROOT: Hub enable intr\n"));
    sc->methods->port_enable_intr(sc->context, 1);
}

void root_hub_init(root_hub_softc_t *sc, struct root_hub_methods *methods,
    root_hub_desc_t *desc, void *context)
{
    if (!desc)
    {
        DBG_E(DHOST_ROOT, ("ROOT: No root hub descriptor provided\n"));
        return;
    }

    if (!desc->pwr_on_2_pwr_good)
        DBG_W(DHOST_ROOT, ("ROOT: Power on to power good is very short\n"));

    if (!desc->noport)
    {
        DBG_E(DHOST_ROOT, ("ROOT: Root hub with zero ports !\n"));
        return;
    }

    DBG_I(DHOST_ROOT, ("ROOT: Initializing hub %p  Ports %d Speed %d\n",
        sc, desc->noport, desc->speed));
    
    sc->conf    = 0;
    sc->addr    = 0;
    sc->xfer    = NULL;
    sc->context = context;
    sc->methods = methods;
    sc->desc    = desc;
    if (jtask_init(&sc->intr_enable, TASK_CONTROLLER))
        DBG_E(DHOST_ROOT, ("ROOT: Failed initialize intr_enable task\n"));
}

void root_hub_uninit(root_hub_softc_t *sc)
{
    DBG_I(DHOST_ROOT, ("ROOT: Un-Initializing hub %p\n", sc));
    jtask_stop(sc->intr_enable);
    jtask_uninit(sc->intr_enable);
}

juint8_t root_hub_get_address(root_hub_softc_t *sc)
{
    return sc->addr;
}

void root_hub_port_change(root_hub_softc_t *sc)
{
    juint8_t *p;
    juint8_t i, m;
    root_hub_desc_t *desc = sc->desc;

    DBG_V(DHOST_ROOT, ("ROOT: HCD Reports port change ! [%p]\n", sc));

    /* Disable port interrupt notification for 100 msec */
    sc->methods->port_enable_intr(sc->context, 0);
    jtask_schedule(sc->intr_enable, 100, root_hub_intr_enable, sc);

    if (sc->xfer == NULL) 
    {
        DBG_W(DHOST_ROOT, ("ROOT: XFER is null\n"));
        return;
    }

    p = KERNADDR(&sc->xfer->dmabuf, 0); 
    /* TODO - Understand this */
    m = (juint8_t)(MIN(desc->noport, sc->xfer->length * 8 - 1));
    j_memset(p, 0, sc->xfer->length);

    for (i = 1; i <= m; i++) 
    {
        if (sc->methods->port_get_change(sc->context, i) & UPS_C_PORT_CHANGE)
            p[i/8] |= 1 << (i%8);
    }

    sc->xfer->actlen = sc->xfer->length;
    sc->xfer->status = USBD_NORMAL_COMPLETION;

    usb_transfer_complete(sc->xfer);
}

static juint16_t root_hub_get_desc_device(root_hub_softc_t *sc, juint16_t len, 
    void *buf, juint16_t *actlen)
{
    root_hub_desc_t *desc = sc->desc;

    usb_device_descriptor_t ddesc = {
        USB_DEVICE_DESCRIPTOR_SIZE,
        UDESC_DEVICE,           /* type */
        {0x00, 0x02},           /* USB version */
        UDCLASS_HUB,            /* class */
        UDSUBCLASS_HUB,         /* subclass */
        UDPROTO_HSHUBSTT,       /* protocol */  
        64,                     /* max packet */
        {0x06, 0},{0x0c, 0},{0x0, 0xaf}, /* device id */
        1,2,3,                  /* string indicies */
        1                       /* # of configurations */
    };

    if (desc->speed == USB_SPEED_HIGH)
    {
        USETW(ddesc.bcdUSB, 0x0020);
        ddesc.bDeviceProtocol = UDPROTO_HSHUBSTT;
    }
    else
    {
        USETW(ddesc.bcdUSB, 0x0011);
        ddesc.bDeviceProtocol = UDPROTO_FSHUB;
    }

    *actlen = MIN(USB_DEVICE_DESCRIPTOR_SIZE, len);
    j_memcpy(buf, (void *)&ddesc, *actlen);
    return 0;
}

static juint16_t root_hub_get_desc_config(root_hub_softc_t *sc, juint16_t len, 
    void *buf, juint16_t *actlen)
{
    juint16_t l;
    root_hub_desc_t *desc = sc->desc;
    
    usb_config_descriptor_t root_hub_confd = {
        USB_CONFIG_DESCRIPTOR_SIZE,
        UDESC_CONFIG,
        { USB_CONFIG_DESCRIPTOR_SIZE +
            USB_INTERFACE_DESCRIPTOR_SIZE +
                USB_ENDPOINT_DESCRIPTOR_SIZE, 0
        },
        1, 1, 0, UC_SELF_POWERED, 0 
    };
    usb_interface_descriptor_t root_hub_ifcd = {
        USB_INTERFACE_DESCRIPTOR_SIZE,
        UDESC_INTERFACE,
        0, 0, 1,
        UICLASS_HUB,
        UISUBCLASS_HUB,
        UIPROTO_HSHUBSTT, 0     
    };
    usb_endpoint_descriptor_t root_hub_endpd = {
        USB_ENDPOINT_DESCRIPTOR_SIZE,
        UDESC_ENDPOINT,
        UE_DIR_IN | HCD_INTR_ENDPT,
        UE_INTERRUPT,
        {8, 0},
        255
    };
    
    if (desc->speed == USB_SPEED_HIGH)
        root_hub_endpd.bInterval = 0x0c;
    else
        root_hub_endpd.bInterval = 255;

    if (desc->self_powered)
        root_hub_confd.bmAttributes = UC_SELF_POWERED;
    else
        root_hub_confd.bmAttributes = UC_BUS_POWERED;

    if (desc->remote_wakeup)
        root_hub_confd.bmAttributes |= UC_REMOTE_WAKEUP;

    l    = MIN(USB_CONFIG_DESCRIPTOR_SIZE, len);
    j_memcpy(buf, (void *)&root_hub_confd, l);
    len -= l;
    buf  = (char *)buf + l;
    *actlen = l;

    l    = MIN(USB_INTERFACE_DESCRIPTOR_SIZE, len);
    j_memcpy(buf, (void *)&root_hub_ifcd, l);
    len -= l;
    buf  = (char *)buf + l;
    *actlen += l;
 
    l    = MIN(USB_ENDPOINT_DESCRIPTOR_SIZE, len);
    j_memcpy(buf, (void *)&root_hub_endpd, l);
    len -= l;
    buf  = (char *)buf + l;
    *actlen += l;

    return 0;
}

static jresult_t root_hub_get_desc_string(jint_t index, juint16_t len, void* buf, 
    juint16_t *actlen)
{
    jint_t i;
    char *s;
    usb_string_descriptor_t *str_desc = buf;

    if (len < 2)
        return 0;

    switch (index)
    {
    case 0: /* Laguage ID */
        s = "0";
        break;
    case 1:
        s = "Jungo";
        break;
    case 2:
        s = "Generic Root Hub";
        break;
    case 3:
        s = "0001";
        break;
    default:
        return JEINVAL;
    }

    str_desc->bLength           = (juint8_t)(2 * j_strlen(s) + 2);
    str_desc->bDescriptorType   = UDESC_STRING;
    
    len -= 2; 
 
    for (i = 0; s[i] && len > 1; i++, len -= 2)
        USETW2(str_desc->bString[i], 0, s[i]);

    *actlen = (2 * i + 2);

    return 0;
}

static jint_t root_hub_get_desc_qual(root_hub_softc_t *sc, juint16_t len,
    void *buf, juint16_t *actlen)
{
    root_hub_desc_t *desc = sc->desc;

    usb_device_qualifier_t root_hub_odevd = {
        USB_DEVICE_DESCRIPTOR_SIZE,
        UDESC_DEVICE_QUALIFIER, /* type */
        {0x00, 0x02},           /* USB version */
        UDCLASS_HUB,            /* class */
        UDSUBCLASS_HUB,         /* subclass */
        UDPROTO_FSHUB,          /* protocol */
        64,                     /* max packet */
        1,                      /* # of configurations */
        0
    };

    if (desc->speed == USB_SPEED_LOW || desc->speed == USB_SPEED_FULL)
        return JEINVAL;

    *actlen = MIN(USB_DEVICE_DESCRIPTOR_SIZE, len);
    j_memcpy(buf, (void *)&root_hub_odevd, *actlen);

    return 0;
}

static jint_t root_hub_get_desc_hub(root_hub_softc_t *sc, juint16_t len, 
    void *buf, juint16_t *actlen)
{
    jint_t i, l;
    root_hub_desc_t *desc = sc->desc;

    usb_hub_descriptor_t hubd = {
        USB_HUB_DESCRIPTOR_SIZE,
        UDESC_HUB,
        0,
        {0,0},
        0,
        0,
        {0},
    };

    hubd.bNbrPorts = (uByte)desc->noport;
   
    USETW(hubd.wHubCharacteristics,
        UHD_PWR_INDIVIDUAL      | 
        UHD_COMPOUND            | /* Root is always compound */
        UHD_OC_GLOBAL           | /* Assume global over current */
        UHD_TT_THINK_8          | /* No TT in root hub anyway */
        (desc->port_indication ? UHD_PORT_IND : 0));  /* Port indication */

    hubd.bPwrOn2PwrGood = (uByte)desc->pwr_on_2_pwr_good; 

    /* No support for non removable devices */
    for (i = 0, l = desc->noport; l > 0; i++, l -= 8)
        hubd.DeviceRemovable[i++] = 0; 
    
    hubd.bDescLength = USB_HUB_DESCRIPTOR_SIZE + i;

    *actlen = MIN(hubd.bDescLength, len);
    j_memcpy(buf, (void *)&hubd, *actlen);

    return 0;
}

static jint_t root_hub_get_desc(root_hub_softc_t *sc, jint_t type, juint16_t len,
    void *buf, juint16_t *actlen)
{
    jint_t rc = JEINVAL;

    DBG_V(DHOST_ROOT, ("ROOT: Returning descriptor type %d len %d [%p]\n",
        type >> 8, len, sc));
    
    switch (type >> 8)
    {
    case UDESC_DEVICE:
        rc = root_hub_get_desc_device(sc, len, buf, actlen);
        break;
    case UDESC_CONFIG:
    case UDESC_OTHER_SPEED_CONFIGURATION:
        rc = root_hub_get_desc_config(sc, len, buf, actlen);
        break;
    case UDESC_STRING:
        rc = root_hub_get_desc_string(type & 0xff, len, buf, actlen);
        break;
    case UDESC_DEVICE_QUALIFIER:
        rc = root_hub_get_desc_qual(sc, len, buf, actlen);
        break;

    case UDESC_INTERFACE:
    case UDESC_ENDPOINT:
    default:
        break;
    }
    
    return rc;
}

usbd_status root_hub_ctrl_start(usbd_xfer_handle xfer)
{
    root_hub_softc_t *sc = &(xfer->pipe->device->bus->rootsoftc);
    root_hub_desc_t *desc = sc->desc;

    usbd_bus_handle bus = xfer->pipe->device->bus;
    usb_device_request_t *req;
    void *buf = NULL;
    juint8_t index;
    juint16_t len, value, l, actlen = 0;
    juint16_t change, status;
    usbd_status err;
    jresult_t rc;
    usb_port_status_t ps;

    if (!bus->methods->is_active(bus))
        return (USBD_CANCELLED);

    req         = &xfer->request;
    len         = UGETW(req->wLength);
    value       = UGETW(req->wValue);
    index       = UGETW(req->wIndex);

    if (xfer->flags & USBD_DMA_BUF)
    {
        DBG_E(DHOST_ROOT, ("control transfer called with USBD_DMA_BUF"));
        return USBD_INVAL;
    }

    if (len != 0)
        buf = KERNADDR(&xfer->dmabuf, 0);

    DBG_X(DHOST_ROOT, ("ROOT: Starting root control transfer [%d:%d] %p [%p]\n",
        req->bRequest, req->bmRequestType, sc, xfer->pipe->device->bus));

#define C(x,y) ((x) | ((y) << 8))
    switch(C(req->bRequest, req->bmRequestType)) 
    {
    case C(UR_GET_CONFIG        , UT_READ_DEVICE):
        if (len > 0) 
        {
            *(juint8_t *)buf = (uByte)sc->conf;
            actlen = 1;
        }
        break;
        
    case C(UR_GET_DESCRIPTOR    , UT_READ_DEVICE):
        if ((value & 0xff) != 0 && (value >> 8 != UDESC_STRING)) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        rc = root_hub_get_desc(sc, value, len, buf, &actlen);
        if (rc < 0)
        {
            err = USBD_IOERROR;
            goto ret;
        }
        break;
        
    case C(UR_GET_INTERFACE     , UT_READ_INTERFACE):
        if (len > 0) 
        {
            *(juint8_t *)buf = 0;
            actlen = 1;
        }
        break;
        
    case C(UR_GET_STATUS        , UT_READ_DEVICE):
    case C(UR_GET_STATUS        , UT_READ_INTERFACE):
    case C(UR_GET_STATUS        , UT_READ_ENDPOINT):
        if (len > 1) 
        {
            USETW(((usb_status_t *)buf)->wStatus,
                req->bmRequestType == UT_READ_DEVICE ? UDS_SELF_POWERED : 0);
            actlen = 2;
        }
        break;
        
    case C(UR_SET_ADDRESS       , UT_WRITE_DEVICE):
        if (value >= USB_MAX_DEVICES) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        sc->addr = (juint8_t)value;
        break;
        
    case C(UR_SET_CONFIG        , UT_WRITE_DEVICE):
        if (value != 0 && value != 1) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        sc->conf = value;
        break;
        
    case C(UR_GET_DESCRIPTOR    , UT_READ_CLASS_DEVICE):
        if ((value & 0xff) != 0) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        rc = root_hub_get_desc_hub(sc, len, buf, &actlen);
        if (rc < 0)
        {
            err = USBD_IOERROR;
            goto ret;
        }
        break;
        
    case C(UR_GET_STATUS        , UT_READ_CLASS_DEVICE):
        if (len != 4) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        j_memset(buf, 0, len); /* TODO: Report power/overcurrent */
        actlen = len;
        break;
        
    case C(UR_GET_STATUS        , UT_READ_CLASS_OTHER):
        if (index < 1 || index > desc->noport || len != 4) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        status = sc->methods->port_get_status(sc->context, index);
        change = sc->methods->port_get_change(sc->context, index);

        USETW(ps.wPortStatus, status);
        USETW(ps.wPortChange, change);
        l = MIN(len, USB_PORT_STATUS_SIZE);
        j_memcpy(buf, (void *)&ps, l);
        actlen = l;
        break;
        
    case C(UR_CLEAR_FEATURE     , UT_WRITE_CLASS_OTHER):
    case C(UR_SET_FEATURE       , UT_WRITE_CLASS_OTHER):
        if (index < 1 || index > desc->noport) 
        {
            err = USBD_IOERROR;
            goto ret;
        }
        switch(value) 
        {
        case UHF_PORT_RESET:
        case UHF_PORT_ENABLE:
        case UHF_PORT_SUSPEND:
        case UHF_PORT_POWER:
        case UHF_PORT_TEST:
        case UHF_PORT_INDICATOR:
        case UHF_C_PORT_RESET:
        case UHF_C_PORT_CONNECTION:
        case UHF_C_PORT_ENABLE:
        case UHF_C_PORT_SUSPEND:
        case UHF_C_PORT_OVER_CURRENT:
            err = sc->methods->port_do_change(sc->context, index, value,
                req->bRequest == UR_SET_FEATURE ? 1 : 0);
            if (err)
                goto ret;
            break;
            
        default:
            err = USBD_IOERROR;
            goto ret;
        }
        break;
        
    case C(UR_GET_TT_STATE      , UT_READ_CLASS_OTHER):
    case C(UR_SET_DESCRIPTOR    , UT_WRITE_DEVICE):
    case C(UR_CLEAR_TT_BUFFER   , UT_WRITE_CLASS_OTHER):
    case C(UR_RESET_TT          , UT_WRITE_CLASS_OTHER):
    case C(UR_STOP_TT           , UT_WRITE_CLASS_OTHER):
    case C(UR_SET_FEATURE       , UT_WRITE_CLASS_DEVICE):
    case C(UR_SET_INTERFACE     , UT_WRITE_INTERFACE):
    case C(UR_SYNCH_FRAME       , UT_WRITE_ENDPOINT):
    case C(UR_CLEAR_FEATURE     , UT_WRITE_DEVICE):
    case C(UR_CLEAR_FEATURE     , UT_WRITE_CLASS_DEVICE):
    case C(UR_CLEAR_FEATURE     , UT_WRITE_INTERFACE):
    case C(UR_CLEAR_FEATURE     , UT_WRITE_ENDPOINT):
        break;
    
    case C(UR_SET_FEATURE       , UT_WRITE_DEVICE):
        if (value == UF_DEVICE_REMOTE_WAKEUP)
            break;
#ifdef CONFIG_PORT_INTERCHIP        
    case C(UR_GET_INTERCHIP     , UT_READ_VENDOR_OTHER):
        *(juint8_t *)buf = 
          sc->methods->is_port_interchip(sc->context, value);
        actlen = 1;
        break;
#endif
    case C(UR_SET_DESCRIPTOR    , UT_WRITE_CLASS_DEVICE):
    case C(UR_SET_FEATURE       , UT_WRITE_INTERFACE):
    case C(UR_SET_FEATURE       , UT_WRITE_ENDPOINT):
    default:
        err = USBD_IOERROR;
        goto ret;
    }
    
    xfer->actlen = actlen;
    err          = USBD_NORMAL_COMPLETION;

ret:
    xfer->status = err;

    if (err)
        DBG_W(DHOST_ROOT, ("ROOT: Ctrl transfer completed with error %d\n", err));

    usb_transfer_complete(xfer);
    return (USBD_IN_PROGRESS);
}

void root_hub_generic_noop(usbd_xfer_handle xfer)
{
    UNUSED_VAR(xfer);
}

void root_hub_generic_noop_pipe(usbd_pipe_handle pipe)
{
    UNUSED_VAR(pipe);
}

usbd_status root_hub_intr_start(usbd_xfer_handle xfer)
{
    usbd_pipe_handle pipe = xfer->pipe;
    usbd_bus_handle  bus = pipe->device->bus;
    root_hub_softc_t *sc = &(pipe->device->bus->rootsoftc);

    DBG_X(DHOST_ROOT, ("ROOT: Interrupt start %p\n", xfer));

    if (!bus->methods->is_active(bus))
        return (USBD_CANCELLED);

    sc->xfer = xfer;

    return (USBD_IN_PROGRESS);
}

/* Abort a root interrupt request. */
void root_hub_intr_abort(usbd_xfer_handle xfer)
{
    DBG_V(DHOST_ROOT, ("ROOT: Interrupt abort %p\n", xfer));

    xfer->status = USBD_CANCELLED;
    usb_transfer_complete(xfer);
}

/* Close the root pipe. */
void root_hub_intr_close(usbd_pipe_handle pipe)
{
    root_hub_softc_t *sc = &(pipe->device->bus->rootsoftc);
    
    DBG_V(DHOST_ROOT, ("ROOT: Interrupt close %p\n", pipe));
    sc->xfer = NULL;
}

static usbd_status root_hub_intr_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    DBG_V(DHOST_ROOT, ("ROOT: Interrupt transfer %p\n", xfer));

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /* Pipe isn't running, start first */
    return (root_hub_intr_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

static usbd_status root_hub_ctrl_transfer(usbd_xfer_handle xfer)
{
    usbd_status err;

    DBG_V(DHOST_ROOT, ("ROOT: Ctrl transfer %p\n", xfer));

    /* Insert last in queue. */
    err = usb_insert_transfer(xfer);
    if (err)
        return (err);

    /* Pipe isn't running, start first */
    return (root_hub_ctrl_start(TAILQ_FIRST(&xfer->pipe->queue)));
}

struct usbd_pipe_methods root_hub_ctrl_methods = {
    root_hub_ctrl_transfer,
    root_hub_ctrl_start,
    root_hub_generic_noop,
    root_hub_generic_noop_pipe,
    root_hub_generic_noop_pipe,
    root_hub_generic_noop
};

struct usbd_pipe_methods root_hub_intr_methods = {
    root_hub_intr_transfer,
    root_hub_intr_start,
    root_hub_intr_abort,
    root_hub_intr_close,
    root_hub_generic_noop_pipe,
    root_hub_generic_noop
};

usbd_status root_hub_open_pipe(usbd_pipe_handle pipe)
{
    root_hub_softc_t *sc = &pipe->device->bus->rootsoftc;

    usbd_device_handle dev = pipe->device;
    usb_endpoint_descriptor_t *ed = pipe->endpoint->edesc;

    DBG_X(DHOST_ROOT, ("ROOT: Openning pipe %d\n", dev->address));

    if (dev->address == sc->addr) 
    {
        switch (ed->bEndpointAddress) 
        {
        case USB_CONTROL_ENDPOINT:
            DBG_V(DHOST_ROOT, ("ROOT: Openning ctrl pipe\n"));
            pipe->methods = &root_hub_ctrl_methods;
            break;
        case UE_DIR_IN | HCD_INTR_ENDPT:
            DBG_V(DHOST_ROOT, ("ROOT: Openning intr pipe\n"));
            pipe->methods = &root_hub_intr_methods;
            break;
        default:
            return (USBD_INVAL);
        }
    }
    return (USBD_NORMAL_COMPLETION);
}

