/*=======================================================================*//**
  @file         qhsusb_fd_chg.c

  @brief        Function Driver for dead battery charger

  @details      
                Simple HID device presented to the host.
 
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/func/qhsusb_fd_chg.c#12 $$DateTime: 2008/12/30 02:48:29 $$Author: vkondrat $

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2008-08-28  vkondrat  initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_fd_chg.h"

#include "qhsusb_dcd.h"
#include "qhsusb_urb.h"

#include "qhsusb_ch9.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#ifndef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
#error "Charger code require FEATURE_QHSUSB_DEAD_BATTERY_CHARGING"
#endif
#ifndef QHSUSB_HAVE_MULTI_CONFIG
#error "Charger code require QHSUSB_HAVE_MULTI_CONFIG"
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static void chg_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event);
static int hid_setup(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req);

/*****hardware descriptors*****/
static usb_desc_device_t chg_usb_device = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   0,                              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
// @todo bMaxPacketSize0 is 8 bytes for LS
   64,                             //  uint8  bMaxPacketSize0;
   USB_VENDOR_ID_QUALCOMM,         //  uint16 idVendor;
   0xf006,                         //  uint16 idProduct;
   0x0,                            //  uint16 bcdDevice;
   0,                              //  uint8  iManufacturer;
   0,                              //  uint8  iProduct;
   0,                              //  uint8  iSerialNumber;
   2,                              //  uint8  bNumConfigurations;
};

static uint8 the_report_descriptor[] = {
   0x06, 0xA0, 0xFF, 0x09, 0xA5, 0xA1, 0x01, 0x09,
   0xA6, 0x09, 0xA7, 0x15, 0x80, 0x25, 0x7F, 0x75,
   0x08, 0x95, 0x02, 0x81, 0x02, 0x09, 0xA9, 0x15,
   0x80, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, 0x91, 
   0x02, 0xC0,
};

typedef PACKED struct chg_config_struct {
   usb_desc_configuration_t conf1;
   usb_desc_interface_t ifc0;
   usb_desc_hid_t hid;
   usb_desc_endpoint_t ep_in;
} PACKED_POST chg_config_t;

static chg_config_t chg_usb_config_500 = {
   {
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(chg_config_t),              //      uint16 wTotalLength;
      1,                                 //      uint8  bNumInterfaces;
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      250                                //      uint8  bMaxPower;
   },
   {
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      1,                                 //      uint8  bNumEndpoints;
      0x3,    /* HID */                  //      uint8  bInterfaceClass;
      0,                                 //      uint8  bInterfaceSubClass;
      0,                                 //      uint8  bInterfaceProtocol;
      0                                  //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_hid_t),            //      uint8  bLength;
      USB_DESCTYPE_HID,                  //      uint8  bDescriptorType;
      0x0111,                            //      uint16 bcdHID;
      0,                                 //      uint8  bCountryCode;
      1,                                 //      uint8  bNumDescriptor;
      USB_DESCTYPE_HID_REPORT,           //      uint8  bDescriptorType;
      sizeof(the_report_descriptor),     //      uint16 wDescriptorLen;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_INTR,                  //      uint8  bmAttributes;
      1,                                 //      uint16 wMaxPacketSize;
      /**
       * @ref USB 2.0, 9.6.6 
       * interval for interrupt endpoints: 
       * - full/low speed : 1..255 
       * - high speed : 1..16 (means 2^(x-1)) 
       *  
       * Need to select safe value for all speeds
       */
      16,                                //      uint8  bInterval;
   },
};

static chg_config_t chg_usb_config_100 = {
   {
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(chg_config_t),              //      uint16 wTotalLength;
      1,                                 //      uint8  bNumInterfaces;
      2,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      50                                 //      uint8  bMaxPower;
   },
   {
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      1,                                 //      uint8  bNumEndpoints;
      0x3,    /* HID */                  //      uint8  bInterfaceClass;
      0,                                 //      uint8  bInterfaceSubClass;
      0,                                 //      uint8  bInterfaceProtocol;
      0                                  //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_hid_t),            //      uint8  bLength;
      USB_DESCTYPE_HID,                  //      uint8  bDescriptorType;
      0x0111,                            //      uint16 bcdHID;
      0,                                 //      uint8  bCountryCode;
      1,                                 //      uint8  bNumDescriptor;
      USB_DESCTYPE_HID_REPORT,           //      uint8  bDescriptorType;
      sizeof(the_report_descriptor),     //      uint16 wDescriptorLen;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_INTR,                  //      uint8  bmAttributes;
      1,                                 //      uint16 wMaxPacketSize;
      /**
       * @ref USB 2.0, 9.6.6 
       * interval for interrupt endpoints: 
       * - full/low speed : 1..255 
       * - high speed : 1..16 (means 2^(x-1)) 
       *  
       * Need to select safe value for all speeds
       */
      16,                                //      uint8  bInterval;
   },
};

/*****software descriptors*****/
static struct qhsusb_dcd_dsc_device qhsusb_device;

static struct qhsusb_dcd_dsc_configuration the_config_500;
static struct qhsusb_dcd_dsc_interface the_ifc_500;

static struct qhsusb_dcd_dsc_configuration the_config_100;
static struct qhsusb_dcd_dsc_interface the_ifc_100;

static struct qhsusb_dcd_dsc_endpoint the_eps_500[1] = {
   {
      NULL,                        //   struct qhsusb_dcd_dsc_endpoint* next;
      &chg_usb_config_500.ep_in,   //   struct usb_desc_endpoint* descriptor;
      &the_ifc_500,                //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                        //   int (*setup)(...);
   },
};

static struct qhsusb_dcd_dsc_interface the_ifc_500 = {
   NULL,                           //  struct qhsusb_dcd_dsc_interface* next;
   &chg_usb_config_500.ifc0,       //  struct usb_desc_interface* descriptor;
   &the_config_500,                //  struct qhsusb_dcd_dsc_configuration* configuration;
   the_eps_500,                    //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   hid_setup,                      //  int (*setup)(...);
};

static struct qhsusb_dcd_dsc_configuration the_config_500 = {
   &the_config_100,                //  struct qhsusb_dcd_dsc_configuration* next;
   &chg_usb_config_500.conf1,      //  struct usb_desc_configuration* descriptor;
   &qhsusb_device,                 //  struct qhsusb_dcd_dsc_device* device;
   &the_ifc_500                    //  struct qhsusb_dcd_dsc_interface* interfaces;
};

static struct qhsusb_dcd_dsc_endpoint the_eps_100[1] = {
   {
      NULL,                        //   struct qhsusb_dcd_dsc_endpoint* next;
      &chg_usb_config_100.ep_in,   //   struct usb_desc_endpoint* descriptor;
      &the_ifc_100,                //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                        //   int (*setup)(...);
   },
};

static struct qhsusb_dcd_dsc_interface the_ifc_100 = {
   NULL,                           //  struct qhsusb_dcd_dsc_interface* next;
   &chg_usb_config_100.ifc0,       //  struct usb_desc_interface* descriptor;
   &the_config_100,                //  struct qhsusb_dcd_dsc_configuration* configuration;
   the_eps_100,                    //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   hid_setup,                      //  int (*setup)(...);
};

static struct qhsusb_dcd_dsc_configuration the_config_100 = {
   NULL,                           //  struct qhsusb_dcd_dsc_configuration* next;
   &chg_usb_config_100.conf1,      //  struct usb_desc_configuration* descriptor;
   &qhsusb_device,                 //  struct qhsusb_dcd_dsc_device* device;
   &the_ifc_100                    //  struct qhsusb_dcd_dsc_interface* interfaces;
};

static struct qhsusb_dcd_dsc_device qhsusb_device = {
   &chg_usb_device,                //  struct usb_desc_device* descriptor;
   &the_config_500,                //  struct qhsusb_dcd_dsc_configuration* configurations;
   NULL,                           //  struct qhsusb_dcd_dsc_string* strings;
   0,                              //  uint32 core_index;
   chg_notify_cb,                  //  void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
   NULL,                           //  int (*setup)(...);
};

/************* Buffers for Tx/Rx *************/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static void chg_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event)
{
   switch (event) {
   case QHSUSB_DCD_EVT_CURRENT:
      break;
   default:
      ;
   }
}

static int hid_setup(struct qhsusb_dcd_dsc_interface* ifc, struct usb_setup_packet* req)
{
   switch (req->bRequest) {
   case USB_SETUP_REQ_GET_DESCRIPTOR:
      {
         switch (req->wValue) {
         case 0x2200:   // REPORT_DESCRIPTOR
            {
               return qhsusb_dcd_control_transfer(ifc->configuration->device, 1, 
                                           the_report_descriptor, 
                                           sizeof(the_report_descriptor));
            }
            break;
         default:
            ;
         }
      }
      break;
   default:
      ;
   }
   return -1;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

struct qhsusb_dcd_dsc_device* qhsusb_fd_chg_init(void)
{
   qhsusb_dcd_init(&qhsusb_device);
   return &qhsusb_device;
}

void qhsusb_fd_chg_shutdown(void)
{
   qhsusb_dcd_shutdown(&qhsusb_device);
}


