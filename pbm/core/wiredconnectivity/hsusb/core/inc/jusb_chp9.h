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
#ifndef _JUSB_CHP9_H_
#define _JUSB_CHP9_H_

#include <jusb_common.h>

/* CONTROL REQUEST SUPPORT */
#define USB_DIR_OUT                     0       /* to device */
#define USB_DIR_IN                      0x80    /* to host */

/* USB types, the second of three bRequestType fields */
#define USB_TYPE_MASK                   (0x03 << 5)
#define USB_TYPE_STANDARD               (0x00 << 5)
#define USB_TYPE_CLASS                  (0x01 << 5)
#define USB_TYPE_VENDOR                 (0x02 << 5)
#define USB_TYPE_RESERVED               (0x03 << 5)

/* USB recipients, the third of three bRequestType fields */
#define USB_RECIP_MASK                  0x1f
#define USB_RECIP_DEVICE                0x00
#define USB_RECIP_INTERFACE             0x01
#define USB_RECIP_ENDPOINT              0x02
#define USB_RECIP_OTHER                 0x03

/* Retrive the type of the request recieved */
#define USB_REQ_TYPE                    (0x01 << 7)
#define USB_REQ_GET                     1
#define USB_REQ_SET                     0

/* Standard requests, for the bRequest field of a SETUP packet. */
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C

/* USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE */
#define USB_DEVICE_SELF_POWERED         0       /* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP        1       /* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE            2       /* (high speed only) */
#define USB_DEVICE_B_HNP_ENABLE         3       /* dev may initiate HNP */
#define USB_DEVICE_A_HNP_SUPPORT        4       /* RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT    5       /* other RH port does */

#define USB_ENDPOINT_HALT               0       /* IN/OUT will STALL */

/* Retrive EP/Interface number from Request structure */
#define USB_REQ_ENDPOINT_NUMBER(wIndex)         (le16toh(wIndex) & 0x000f)
#define USB_REQ_ENDPOINT_DIRECTION(wIndex)      (le16toh(wIndex) & (0x01 << 7))
#define USB_REQ_INTERFACE_NUMBER(wIndex)        (le16toh(wIndex) & 0x00ff)

/* struct usb_ctrlrequest - SETUP data for a USB device control request */
#define USB_CONTROL_REQUEST_SIZE        8
struct usb_ctrlrequest {
    juint8_t bRequestType;
    juint8_t bRequest;
    juint16_t wValue;
    juint16_t wIndex;
    juint16_t wLength;
};

/* STANDARD DESCRIPTORS - returned by GET_DESCRIPTOR */

/* Descriptor types ... USB 2.0 spec table 9.5 */
#define USB_DT_DEVICE                   0x01
#define USB_DT_CONFIG                   0x02
#define USB_DT_STRING                   0x03
#define USB_DT_INTERFACE                0x04
#define USB_DT_ENDPOINT                 0x05
#define USB_DT_DEVICE_QUALIFIER         0x06
#define USB_DT_OTHER_SPEED_CONFIG       0x07
#define USB_DT_INTERFACE_POWER          0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG                      0x09
#define USB_DT_DEBUG                    0x0a
#define USB_DT_INTERFACE_ASSOCIATION    0x0b

/* conventional codes for class-specific descriptors */
#define USB_DT_CS_DEVICE                0x21
#define USB_DT_CS_CONFIG                0x22
#define USB_DT_CS_STRING                0x23
#define USB_DT_CS_INTERFACE             0x24
#define USB_DT_CS_ENDPOINT              0x25

/* All standard descriptors have these 2 fields at the beginning */
struct usb_descriptor_header {
    juint8_t  bLength;
    juint8_t  bDescriptorType;
};

/* USB_DT_DEVICE: Device descriptor */
#define USB_DT_DEVICE_SIZE              18
struct usb_device_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint16_t bcdUSB;
    juint8_t  bDeviceClass;
    juint8_t  bDeviceSubClass;
    juint8_t  bDeviceProtocol;
    juint8_t  bMaxPacketSize0;
    juint16_t idVendor;
    juint16_t idProduct;
    juint16_t bcdDevice;
    juint8_t  iManufacturer;
    juint8_t  iProduct;
    juint8_t  iSerialNumber;
    juint8_t  bNumConfigurations;
};

/* Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents */
#define USB_CLASS_PER_INTERFACE         0       /* for DeviceClass */
#define USB_CLASS_AUDIO                 1
#define USB_CLASS_COMM                  2
#define USB_CLASS_HID                   3
#define USB_CLASS_PHYSICAL              5
#define USB_CLASS_STILL_IMAGE           6
#define USB_CLASS_PRINTER               7
#define USB_CLASS_MASS_STORAGE          8
#define USB_CLASS_HUB                   9
#define USB_CLASS_CDC_DATA              0x0a
#define USB_CLASS_CSCID                 0x0b    /* chip+ smart card */
#define USB_CLASS_CONTENT_SEC           0x0d    /* content security */
#define USB_CLASS_VIDEO                 0x0e
#define USB_CLASS_MISC                  0xef
#define USB_CLASS_APP_SPEC              0xfe
#define USB_CLASS_VENDOR_SPEC           0xff

/* Defines for USB_CLASS_MISC subclass */
#define USB_MISC_SUBCLASS_COMMON 0x02
/* Defines for USB_CLASS_MISC  and USB_MISC_SUBCLASS_COMMON protocol */
#define USB_MISC_SUBCLASS_COMMON_PRTCL_IAD 0x01
/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
#define USB_DT_CONFIG_SIZE      9
struct usb_config_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint16_t wTotalLength;
    juint8_t  bNumInterfaces;
    juint8_t  bConfigurationValue;
    juint8_t  iConfiguration;
    juint8_t  bmAttributes;
    juint8_t  bMaxPower;
};

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE              (1 << 7)        /* must be set */
#define USB_CONFIG_ATT_SELFPOWER        (1 << 6)        /* self powered */
#define USB_CONFIG_ATT_WAKEUP           (1 << 5)        /* can wakeup */

/* USB_DT_STRING: String descriptor */
struct usb_string_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint16_t wData[1];          /* UTF-16LE encoded */
};
/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters. */

/* USB_DT_INTERFACE: Interface descriptor */
#define USB_DT_INTERFACE_SIZE   9
struct usb_interface_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint8_t  bInterfaceNumber;
    juint8_t  bAlternateSetting;
    juint8_t  bNumEndpoints;
    juint8_t  bInterfaceClass;
    juint8_t  bInterfaceSubClass;
    juint8_t  bInterfaceProtocol;
    juint8_t  iInterface;
};

/* USB_DT_ENDPOINT: Endpoint descriptor */
#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_AUDIO_ENDPOINT_SIZE      9
struct usb_endpoint_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint8_t  bEndpointAddress;
    juint8_t  bmAttributes;
    juint16_t wMaxPacketSize;
    juint8_t  bInterval;
#ifdef CONFIG_FD_AUDIO
    juint8_t  bRefresh;
    juint8_t  bSynchAddress;
#endif
};

/* Endpoints */
#define USB_ENDPOINT_NUMBER_MASK        0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL       0
#define USB_ENDPOINT_XFER_ISOC          1
#define USB_ENDPOINT_XFER_BULK          2
#define USB_ENDPOINT_XFER_INT           3

#ifndef CONFIG_FULL_SPEED_ONLY
/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
#define USB_DT_QUALIFIER_SIZE   10
struct usb_qualifier_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint16_t bcdUSB;
    juint8_t  bDeviceClass;
    juint8_t  bDeviceSubClass;
    juint8_t  bDeviceProtocol;
    juint8_t  bMaxPacketSize0;
    juint8_t  bNumConfigurations;
    juint8_t  bRESERVED;
};
#endif

#ifdef CONFIG_JOTG
#define USB_DT_OTG_SIZE 3
/* USB_DT_OTG (from OTG 1.0a supplement) */
struct usb_otg_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint8_t  bmAttributes;      /* support for HNP, SRP, etc */
};

/* from usb_otg_descriptor.bmAttributes */
#define USB_OTG_SRP             (1 << 0)
#define USB_OTG_HNP             (1 << 1)        /* swap host/device roles */

#endif

/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
#define USB_DT_INTERFACE_ASSOC_SIZE 8
struct usb_interface_assoc_descriptor {
    juint8_t  bLength;
    juint8_t  bDescriptorType;

    juint8_t  bFirstInterface;
    juint8_t  bInterfaceCount;
    juint8_t  bFunctionClass;
    juint8_t  bFunctionSubClass;
    juint8_t  bFunctionProtocol;
    juint8_t  iFunction;
};

#define USB_GET_STATUS_SIZE     2

#endif

