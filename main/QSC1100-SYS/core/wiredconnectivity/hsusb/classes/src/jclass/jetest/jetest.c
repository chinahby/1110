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
   These test modes are required for the "USB-IF Embedded Host Compliance
   Plan (Revision 0.91 April 2006)" which specifies:

   1. All embedded HS hosts must pass downstream signal quality tests as
   performed on standard hosts at each supported speed.

   a. High-speed embedded hosts must support test modes [USB2:7.1.20]
   b. High-speed embedded hosts must support the high-speed electrical
   test modes as described in Section 6.6.6.1 of the USB On-The-Go
   Supplement, Revision 1.2.
*/

/* Only applicable to high speed hosts */
#ifdef CONFIG_FULL_SPEED_ONLY
#error "jetest class can't be build for full speed host"
#endif

#include <jos.h>

#include <jusb.h>
#include <usbdi.h>
/* For usbd_get_device_desc() */
#include <usbdi_util.h>
#include <usbdivar.h>

#include "jetest.h"

#define JETEST_VID                 0x1a0a
#define JETEST_TEST_SE0_NAK_PID                 0x0101
#define JETEST_TEST_J_PID                       0x0102
#define JETEST_TEST_K_PID                       0x0103
#define JETEST_PACKET_PID                       0x0104
#define JETEST_FORCE_ENABLE_PID                 0x0105
#define JETEST_SUSPEND_RESUME_PID               0x0106
#define JETEST_SINGLE_STEP_GET_DEV_PID          0x0107
#define JETEST_SINGLE_STEP_GET_FEATURE_PID      0x0108
   
typedef struct jetest_softc
{
   usbd_device_handle   bdev;           /* base device */
} jetest_softc_t;

static void get_desc_test(usbd_device_handle dev)
{
    usb_device_descriptor_t tmp;

    /* Host issues SOF for 15 seconds (it should be doing this already) */
    DBG_I(DHOST_ETEST, ("get_desc_test: Waiting 15 seconds before sending "
        "request\n"));
    jdelay_ms(15*1000);

    /* Host sends GetDescriptor(Device) */
    DBG_I(DHOST_ETEST, ("get_desc_test: Sending GetDescriptor(DEVICE) "
        "request\n"));
    usbd_get_device_desc(dev, &tmp);
    return;
}

static jresult_t jetest_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    jresult_t result;

    result = UMATCH_NONE;

    if (uaa->vendor != JETEST_VID)
        goto Exit;
    switch (uaa->product)
    {
    case JETEST_TEST_SE0_NAK_PID:
    case JETEST_TEST_J_PID:
    case JETEST_TEST_K_PID:
    case JETEST_PACKET_PID:
    case JETEST_FORCE_ENABLE_PID:
    case JETEST_SUSPEND_RESUME_PID:
    case JETEST_SINGLE_STEP_GET_DEV_PID:
    case JETEST_SINGLE_STEP_GET_FEATURE_PID:
        result = UMATCH_VENDOR_PRODUCT;
        break;
    default:
        break;
    }

Exit:
    return result;
}

static jresult_t jetest_attach(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    jetest_softc_t *sc = j_device_get_softc(self);
    etest_param_t param;
    jresult_t rc = 0;

    sc->bdev = uaa->device;

    DBG_I(DHOST_ETEST, ("jetest_attach: vid 0x%x pid 0x%x\n", uaa->vendor, 
        uaa->product));

    param.test_dev = sc->bdev;
    param.mode = TEST_MODE_DISABLE;

    /* Select the test mode based on the device PID */
    switch (uaa->product)
    {
    case JETEST_TEST_SE0_NAK_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_SE0_NAK\n"));
        param.mode = TEST_MODE_SE0NAK;
        break;
    case JETEST_TEST_J_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_J\n"));
        param.mode= TEST_MODE_J_STATE;
        break;
    case JETEST_TEST_K_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_K\n"));
        param.mode= TEST_MODE_K_STATE;
        break;
    case JETEST_PACKET_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_PACKET\n"));
        param.mode = TEST_MODE_PACKET;
        break;
    case JETEST_FORCE_ENABLE_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_FORCE_ENABLE\n"));
        param.mode = TEST_MODE_FORCE_ENABLE;
        break;
    case JETEST_SUSPEND_RESUME_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_SUSPEND_RESUME\n"));
        param.mode = TEST_MODE_SUSPEND_RESUME;
        break;
    case JETEST_SINGLE_STEP_GET_DEV_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_SINGLE_STEP_GET_DEV\n"));
        get_desc_test(sc->bdev);
        break;
    case JETEST_SINGLE_STEP_GET_FEATURE_PID:
        DBG_I(DHOST_ETEST, ("test mode is TEST_SINGLE_STEP_GET_FEATURE\n"));
        param.mode = TEST_MODE_SINGLE_STEP_SET_FEATURE;
        break;
    default:
        break;
    }

    if (param.mode != TEST_MODE_DISABLE)
    {
        rc = j_device_ioctl(sc->bdev->bus->bdev, DRV_IOCTL_SET_ETEST_MODE,
            (void *)&param);
    }
    return rc;
}

static jresult_t jetest_detach(jdevice_t self)
{
    jetest_softc_t *sc = j_device_get_softc(self);   

    sc->bdev = NULL;

    return 0;
}

static jresult_t jetest_suspend(jdevice_t self)
{
    return 0;
}

static jresult_t jetest_resume(jdevice_t self)
{
    return 0;
}

static jresult_t jetest_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *args)
{
    return 0;
}

DRIVER_USB_DECLARE(jetest)
