/*=======================================================================*//**
  @file         qhsusb_fd_hdlc.c

  @brief        Function Driver for HDLC stream function

  @details      
                This FD implements HDLC streaming protocol.
                It can use one of the following USB interfaces, depending on
                value of global variable qhsusb_hdlc_use_cdcacm:

                - 0 : use vendor specific class with 2 bulk EP's
                - 1 : use CDC/ACM class with 2 bulk and 1 interrupt EP's

                HDLC implementation: For H->D direction, double buffers used
                to maximize throughput. Upon packet receive, the user supplied
                callback executed. This callback should handle the data arrived.
                For D->H direction, byte oriented interface provided. Data
                accumulated in internal buffer till it is full or 
                HDLC terminator (0x7e) submitted. In this case, packet get 
                transmitted to the USB bus.

                CDC/ACM implementation: FD supports ACM class specific commands:

                - SET_LINE_CODING
                - GET_LINE_CODING
                - SET_CONTROL_LINE_STATE

                Immediately after configuration (SET_CONFIG), FD sends 
                SERIAL_STATE notification indicating DCD and DSR

                CDC/ACM uses 2 interfaces, thus it requires 
                QHSUSB_HAVE_MULTI_FUNCTIONS to be defined.

                To use CDC/ACM, set qhsusb_hdlc_use_cdcacm to 1 prior to
                QHSUSB stack initialization
 
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/func/qhsusb_fd_hdlc.c#15 $$DateTime: 2009/04/02 00:57:38 $$Author: vkondrat $

  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2008-12-17  vkondrat  CDC/ACM support
  2008-08-14  vkondrat  redesign - use simple 1 buffer mode, Rx callback
  2008-08-10  vkondrat  initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qhsusb_fd_hdlc.h"

#include "qhsusb_dcd.h"

#include "qhsusb_cdc.h"
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
#include "qhsusb_dci_api.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define RX_BUF_SIZE (4096) /**< size of each (of 2) Rx buffers */
#define TX_BUF_SIZE (4096) /**< size of  Tx buffer */

#define USB_PRODUCT_CDCACM (0xf00a)

#define CUSTOMIZE NULL

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
#if !defined(QHSUSB_HAVE_MULTI_FUNCTIONS)
   #error "CDC/ACM require multi-function support - define QHSUSB_HAVE_MULTI_FUNCTIONS"
#endif
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

static void hdlc_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event);
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static int cdc_setup(struct qhsusb_dcd_dsc_interface* ifc, 
                     struct usb_setup_packet* req);
static int hdlc_use_cdcacm = 0;
static int hdlc_shutdown_in_progress = 0;
#endif

/*****hardware descriptors*****/
/*****OBEX ********************/
static usb_desc_device_t hdlc_usb_device_obex = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   0,                              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
   64,                             //  uint8  bMaxPacketSize0;
   USB_VENDOR_ID_QUALCOMM,         //  uint16 idVendor;
   #if ( defined(T_MSM6290) )
   0x9002, // Hawk                 //  uint16 idProduct;
   #else
   0x9004, // ULC                  //  uint16 idProduct;
   #endif
   0x0,                            //  uint16 bcdDevice;
   1,                              //  uint8  iManufacturer;
   2,                              //  uint8  iProduct;
//    3,                              //  uint8  iSerialNumber;
   0,                              //  uint8  iSerialNumber;
   1                               //  uint8  bNumConfigurations;
};

static PACKED struct hdlc_config_struct_obex {
   usb_desc_configuration_t conf1;
   usb_desc_interface_t ifc_data;
   usb_desc_endpoint_t ep_in;
   usb_desc_endpoint_t ep_out;
} PACKED_POST hdlc_usb_config_obex = {
   {                                     // conf1
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct hdlc_config_struct_obex), // uint16 wTotalLength;
      1,                                 //      uint8  bNumInterfaces;
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1,                                 //      uint8  bMaxPower;
   },
   {                                     // ifc_data
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      0xff,                              //      uint8  bInterfaceClass;
      0xff,                              //      uint8  bInterfaceSubClass;
      0xff,                              //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },

};
/*****CDC/ACM******************/
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static usb_desc_device_t hdlc_usb_device_cdcacm = {
   sizeof(usb_desc_device_t),      //  uint8  bLength;
   USB_DESCTYPE_DEVICE,            //  uint8  bDescriptorType;
   0x0200,                         //  uint16 bcdUSB;
   USB_DEV_CLASS_CDC,              //  uint8  bDeviceClass;
   0,                              //  uint8  bDeviceSubClass;
   0,                              //  uint8  bDeviceProtocol;
   64,                             //  uint8  bMaxPacketSize0;
   USB_VENDOR_ID_QUALCOMM,         //  uint16 idVendor;
   USB_PRODUCT_CDCACM,             //  uint16 idProduct;
   0x0,                            //  uint16 bcdDevice;
   1,                              //  uint8  iManufacturer;
   2,                              //  uint8  iProduct;
//    3,                              //  uint8  iSerialNumber;
   0,                              //  uint8  iSerialNumber;
   1                               //  uint8  bNumConfigurations;
};

static PACKED struct hdlc_config_struct_cdcacm {
   usb_desc_configuration_t conf1;
   usb_desc_interface_t ifc_cdc;       /**< CDC */
   usb_desc_cdc_header_t cdc_header;   /**< CDC */
   usb_desc_cdc_acm_t cdc_acm;         /**< CDC */
   usb_desc_cdc_union_t cdc_union;     /**< CDC */
   usb_desc_cdc_cm_t cdc_cm;           /**< CDC */
   usb_desc_endpoint_t ep_notify;      /**< CDC */
   usb_desc_interface_t ifc_data;
   usb_desc_endpoint_t ep_in;
   usb_desc_endpoint_t ep_out;
} PACKED_POST hdlc_usb_config_cdcacm = {
   {                                     // conf1
      sizeof(usb_desc_configuration_t),  //      uint8  bLength;
      USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
      sizeof(struct hdlc_config_struct_cdcacm),//uint16 wTotalLength;
      2,                                 //      uint8  bNumInterfaces;
      1,                                 //      uint8  bConfigurationValue;
      0,                                 //      uint8  iConfiguration;
      0x80,                              //      uint8  bmAttributes;
      1,                                 //      uint8  bMaxPower;
   },
   {                                     // ifc_cdc
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      0,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      1,                                 //      uint8  bNumEndpoints;
      USB_IFC_CLASS_CDC,                 //      uint8  bInterfaceClass;
      USB_IFC_SUBCLASS_CDC_ACM,          //      uint8  bInterfaceSubClass;
      0x1,                               //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {                                     // cdc_header
      sizeof(usb_desc_cdc_header_t),     //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_HEADER,      //      uint8  bDescriptorSubType;
      0x0110,                            //      uint16 bcdCDC;
   },
   {                                     // cdc_acm
      sizeof(usb_desc_cdc_acm_t),        //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_ACM,         //      uint8  bDescriptorSubType;
      0x02,                              //      uint8  bmCapabilities;
   },
   {                                     // cdc_union
      sizeof(usb_desc_cdc_union_t)+0,    //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_UNION,       //      uint8  bDescriptorSubType;
      0,                                 //      uint8  bControlInterface;
      {1,},                              //      uint8  bSubordinateInterface[1];
   },
   {                                     // cdc_cm
      sizeof(usb_desc_cdc_cm_t),         //      uint8  bLength;
      USB_DESCTYPE_CS_INTERFACE,         //      uint8  bDescriptorType;
      USB_DESCSUBTYPE_CSIFC_CM,          //      uint8  bDescriptorSubType;
      0x03,                              //      uint8  bmCapabilities;
      1,                                 //      uint8  bDataInterface;
   },
   {                                     // ep_notify
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      2 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_INTR,                  //      uint8  bmAttributes;
      16, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      32,                                //      uint8  bInterval;
   },
   {                                     // ifc_data
      sizeof(usb_desc_interface_t),      //      uint8  bLength;
      USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
      1,                                 //      uint8  bInterfaceNumber;
      0,                                 //      uint8  bAlternateSetting;
      2,                                 //      uint8  bNumEndpoints;
      USB_IFC_CLASS_CDC_DATA,            //      uint8  bInterfaceClass;
      0,                                 //      uint8  bInterfaceSubClass;
      0,                                 //      uint8  bInterfaceProtocol;
      0,                                 //      uint8  iInterface;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },
   {
      sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
      USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
      1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
      USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
      64, // HS=512 , FS=64              //      uint16 wMaxPacketSize;
      0,                                 //      uint8  bInterval;
   },

};
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

static uint8 str0[4] = {
   sizeof(str0),USB_DESCTYPE_STRING,
   0x09,0x04,                            // langid : US_EN
};

static uint8 str_manuf[62] = {                       // iManufacturer
   sizeof(str_manuf),USB_DESCTYPE_STRING,
   'Q',0,
   'u',0,
   'a',0,
   'l',0,
   'c',0,
   'o',0,
   'm',0,
   'm',0,
   ' ',0,
   'C',0,
   'D',0,
   'M',0,
   'A',0,
   ' ',0,
   'T',0,
   'e',0,
   'c',0,
   'h',0,
   'n',0,
   'o',0,
   'l',0,
   'o',0,
   'g',0,
   'i',0,
   'e',0,
   's',0,
   ' ',0,
   'M',0,
   'S',0,
   'M',0,
};

static uint8 qhsusb_str_product[36] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'P',0,
   'r',0,
   'o',0,
   'd',0,
   'u',0,
   'c',0,
   't',0,
   ' ',0,
   'n',0,
   'a',0,
   'm',0,
   'e',0,
   ' ',0,
   'h',0,
   'e',0,
   'r',0,
   'e',0,
};

static uint8 str_serial[18] = {                       // iSerialNumber
   sizeof(str_serial),USB_DESCTYPE_STRING,
   '1',0,
   '2',0,
   '3',0,
   '4',0,
   '5',0,
   '6',0,
   '7',0,
   '8',0,
};

/*****software descriptors*****/
static struct qhsusb_dcd_dsc_device sw_device;
static struct qhsusb_dcd_dsc_configuration sw_config;
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_dcd_dsc_interface sw_cdc_control_ifc;
#endif
static struct qhsusb_dcd_dsc_interface sw_data_ifc;

static struct qhsusb_dcd_dsc_string sw_strings[] = {
   {
      &sw_strings[1],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str0, //   struct usb_desc_header* descriptor;
      0,                             //   uint16 langid;
      0,                             //   uint8 index;
   },
   {
      &sw_strings[2],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str_manuf, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      1,                             //   uint8 index;
   },
   {
      &sw_strings[3],                //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)qhsusb_str_product, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      2,                             //   uint8 index;
   },
   {
      NULL,                          //   struct qhsusb_dcd_dsc_string* next;
      (struct usb_desc_header*)str_serial, //   struct usb_desc_header* descriptor;
      0x0409,                        //   uint16 langid;
      3,                             //   uint8 index;
   },
};

static struct qhsusb_dcd_dsc_endpoint sw_data_eps[2] = {
   {  // IN
      &sw_data_eps[1],         //   struct qhsusb_dcd_dsc_endpoint* next;
      CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
      &sw_data_ifc,            //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
   {  // OUT
      NULL,                    //   struct qhsusb_dcd_dsc_endpoint* next;
      CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
      &sw_data_ifc,            //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
};

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_dcd_dsc_endpoint sw_cdc_eps[] = {
   {
      NULL,                    //   struct qhsusb_dcd_dsc_endpoint* next;
      &hdlc_usb_config_cdcacm.ep_notify, //   struct usb_desc_endpoint* descriptor;
      &sw_cdc_control_ifc,     //   struct qhsusb_dcd_dsc_interface* interface;
      NULL,                    //   int (*setup)(...);
   },
};

static struct qhsusb_dcd_dsc_interface sw_cdc_control_ifc = {
   &sw_data_ifc,                //  struct qhsusb_dcd_dsc_interface* next;
   &hdlc_usb_config_cdcacm.ifc_cdc,    //  struct usb_desc_interface* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configuration;
   sw_cdc_eps,                  //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   cdc_setup,                   //  int (*setup)(...);
};
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

static struct qhsusb_dcd_dsc_interface sw_data_ifc = {
   NULL,                        //  struct qhsusb_dcd_dsc_interface* next;
   CUSTOMIZE,                   //  struct usb_desc_interface* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configuration;
   sw_data_eps,                 //  struct qhsusb_dcd_dsc_endpoint* endpoints;
   NULL,                        //  int (*setup)(...);
};

static struct qhsusb_dcd_dsc_configuration sw_config = {
   NULL,                        //  struct qhsusb_dcd_dsc_configuration* next;
   CUSTOMIZE,                   //  struct usb_desc_configuration* descriptor;
   &sw_device,                  //  struct qhsusb_dcd_dsc_device* device;
   CUSTOMIZE,                   //  struct qhsusb_dcd_dsc_interface* interfaces;
};

static struct qhsusb_dcd_dsc_device sw_device = {
   CUSTOMIZE,                   //  struct usb_desc_device* descriptor;
   &sw_config,                  //  struct qhsusb_dcd_dsc_configuration* configurations;
   sw_strings,                  //  struct qhsusb_dcd_dsc_string* strings;
   0,                           //  uint32 core_id;
   hdlc_notify_cb,              //  void (*notify)(struct qhsusb_dcd_dsc_device* dev, uint32 event);
   NULL,                        //  int (*setup)(...);
};

/************* Buffers for Tx/Rx *************/

static uint8 hdlc_tx_buffer[TX_BUF_SIZE];
static struct qhsusb_urb hdlc_tx_urb;
/* double buffer for Rx */
static uint8 hdlc_rx_buffer[2][RX_BUF_SIZE];
static struct qhsusb_urb hdlc_rx_urb[2];
static void (*user_rx_callback)(struct qhsusb_urb* urb);

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
static struct qhsusb_urb cdc_notify_urb;
static usb_cdc_line_coding_t line_coding = {
   1200,    // uint32 dwDTERate;       /**< Data Terminal Rate, bits per second */
   0,       // uint8  bCharFormat;     /**< Stop bits: 0 - 1 stop bit, 1 - 1.5, 2 - 2 */
   0,       // uint8  bParityType;     /**< 0 - none, 1 - odd, 2 - even, 3 - mark, 4 - space */
   8,       // uint8  bDataBits;       /**< one of {5, 6, 7, 8, 16} */
};

/**
 * Control IN serial lines
 * 
 * [0] - bRxCarrier (DCD)
 * [1] - bTxCarrier (DSR)
 * [2] - bBreak
 * [3] - bRingSignal
 * [4] - bFraming (error)
 * [5] - bParity (error)
 * [6] - bOverRun (error)
 * [7..15] - reserved
 */
static uint16 uart_ctrl_in = 0;
/**
 * Control OUT serial lines
 * 
 * [0] - DTR
 * [1] - RTS
 * [2..15] - reserved
 */
static uint16 uart_ctrl_out = 0;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/** 
 * CDC/ACM command handler
 * 
 * @param ifc
 * @param req
 * 
 * @return int
 */
static int cdc_setup(struct qhsusb_dcd_dsc_interface* ifc, 
                     struct usb_setup_packet* req)
{
   switch (req->bRequest) {
   case USB_SETUP_REQ_CDC_SET_LINE_CODING:
      {
         if ( req->wLength == sizeof(usb_cdc_line_coding_t) ) {
            int len = qhsusb_dcd_control_transfer(ifc->configuration->device, 0,
                                               (void*)&line_coding, 
                                               sizeof(usb_cdc_line_coding_t));
            if ( len == (int)(sizeof(usb_cdc_line_coding_t)) ) {
               return 0;
            }
         }
      }
      break;
   case USB_SETUP_REQ_CDC_GET_LINE_CODING:
      {
         uint32 len = sizeof(usb_cdc_line_coding_t);
         if ( req->wLength < len ) {
            len = req->wLength;
         }
         return qhsusb_dcd_control_transfer(ifc->configuration->device, 1, 
                                     (void*)&line_coding, len);
      }
      break;
   case USB_SETUP_REQ_CDC_SET_CONTROL_LINE_STATE:
      {
         uart_ctrl_out = req->wValue;
         return 0;
      }
      break;
   default:
      ;
   }
   return -1;
}

static int cdc_set_ctrl_in(const struct qhsusb_dcd_dsc_device* dev, uint16 ctrl_in)
{
   int rc;
   /* send notification */
   static usb_cdc_notify_serial_state_t frame;
   frame.notify.bmRequestType = 0xA1;
   frame.notify.bNotificationCode = USB_CDC_NOTIFICATION_SERIAL_STATE;
   frame.notify.wValue = 0;
   frame.notify.wIndex = sw_cdc_control_ifc.descriptor->bInterfaceNumber;
   frame.notify.wLength = 2;
   frame.ctrl_in = ctrl_in;
   cdc_notify_urb.transfer_buffer_ptr = (void*)&frame;
   cdc_notify_urb.transfer_length = sizeof(frame);
   rc = qhsusb_submit_urb(&cdc_notify_urb);
   /* save value */
   if (0 == rc) {
      uart_ctrl_in = ctrl_in;
   }
   return rc;
}

void qhsusb_hdlc_use_cdcacm(int use_cdc_acm)
{
   hdlc_use_cdcacm = use_cdc_acm;
}

int qhsusb_hdlc_get_use_cdcacm(void)
{
   return hdlc_use_cdcacm;
}

#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

/**
 * Try to submit Rx URB
 * 
 * No more then 1 URB may be active at a time, thus check for Rx 
 * idle. If no Rx is in progress, find 1-st available URB that 
 * is not currently used, i.e. actual_length==0. 
 *  
 * @note May block waiting while Rx URB is successfully 
 *       submitted
 *  
 */
static void hdlc_init_rx(const struct qhsusb_dcd_dsc_device* dev)
{
   /**
    * If we are in the middle of the shutdown process, attempt to 
    * re-init Rx path will lead to endless loop. Relevant for 
    * CDC/ACM only 
    */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
      if ( 0 != hdlc_shutdown_in_progress ) {
         return;
      }
#endif
   if ( ( QHSUSB_URB_STATUS_ACTIVE != hdlc_rx_urb[0].transfer_status ) &&
        ( QHSUSB_URB_STATUS_ACTIVE != hdlc_rx_urb[1].transfer_status ) ) {
      int i;
      for (i = 0; i < 2; i++) {
         struct qhsusb_urb* urb = &hdlc_rx_urb[i];
         if ( 0 == urb->actual_length ) {
            while ( 0 != qhsusb_submit_urb(urb) ) {
               qhsusb_dcd_poll(&sw_device);
               if ( 0 == dev->config_value ) {
                  return;
               }
            }
            return;
         }
      }
   }
}

static void hdlc_notify_cb(struct qhsusb_dcd_dsc_device* dev, uint32 event)
{
   switch (event) {
   case QHSUSB_DCD_EVT_CONFIG:
      hdlc_init_rx(dev);
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
      if ( 0 != hdlc_use_cdcacm ) {
         cdc_set_ctrl_in(dev,0x3); /**< DCD | DSR */
      }
#endif
      break;
   case QHSUSB_DCD_EVT_SPEED:
      {
         uint16 wMaxPacketSize = 0;
         /**
          * Depending on speed, set bulk EP max. packet size 
          *  
          * USB 2.0 #5.8.3 specifies bulk max. packet size 
          *  - for low speed - none, bulk EP not supported 
          *  - for full speed one of {8,16,32,64}
          *  - for high speed - 512
          */
         switch (dev->speed) {
         case QHSUSB_DCD_SPEED_FULL:
            wMaxPacketSize = 64;
            break;
         case QHSUSB_DCD_SPEED_HIGH:
            wMaxPacketSize = 512;
            break;
         default:
            ; /* either low speed or not connected. can't function */
         }
         sw_data_eps[0].descriptor->wMaxPacketSize = wMaxPacketSize;
         sw_data_eps[1].descriptor->wMaxPacketSize = wMaxPacketSize;
      }
      break;
   default:
      break;
   }
}

/**
 * At the end of Tx transfer, mark packet as done
 * 
 * @param urb
 */
static void hdlc_tx_callback(struct qhsusb_urb* urb)
{
   urb->transfer_length = 0;
}

/**
 * As soon as Rx completed,
 * Rx is idle and it is possible to submit another URB 
 *  
 * After this, (possibly time consuming process) user callback 
 * called. 
 *  
 * @note may block 
 *  
 * @param urb
 */
static void hdlc_rx_callback(struct qhsusb_urb* urb)
{
   hdlc_init_rx(urb->usb_device);
   user_rx_callback(urb);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

struct qhsusb_dcd_dsc_device* qhsusb_fd_hdlc_init(void (*rx_callback)(struct qhsusb_urb* urb),
                                                  uint8* str_product)
{
   int i;
   struct qhsusb_urb* urb;
   if ( NULL != str_product ) {
      sw_device.strings[2].descriptor = (struct usb_desc_header*)(void*)str_product;
   }
   /* set up sw -> hw links */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   hdlc_shutdown_in_progress = 0;
   if ( 0 != hdlc_use_cdcacm ) {
      sw_data_eps[0].descriptor = &hdlc_usb_config_cdcacm.ep_in;
      sw_data_eps[1].descriptor = &hdlc_usb_config_cdcacm.ep_out;
      sw_data_ifc.descriptor = &hdlc_usb_config_cdcacm.ifc_data;
      sw_config.descriptor = &hdlc_usb_config_cdcacm.conf1;
      sw_device.descriptor = &hdlc_usb_device_cdcacm;
      sw_config.interfaces = &sw_cdc_control_ifc;
   } else 
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
   {
      sw_data_eps[0].descriptor = &hdlc_usb_config_obex.ep_in;
      sw_data_eps[1].descriptor = &hdlc_usb_config_obex.ep_out;
      sw_data_ifc.descriptor = &hdlc_usb_config_obex.ifc_data;
      sw_config.descriptor = &hdlc_usb_config_obex.conf1;
      sw_device.descriptor = &hdlc_usb_device_obex;
      sw_config.interfaces = &sw_data_ifc;
   }
   qhsusb_dcd_init(&sw_device);
   user_rx_callback = rx_callback;
   /* Data buffers & URB's */
   /* tx */
   urb                      = &hdlc_tx_urb;
   urb->usb_device          = &sw_device;
   urb->endpoint_address    = sw_data_eps[0].descriptor->bEndpointAddress;
   urb->transfer_buffer_ptr = hdlc_tx_buffer;
   urb->transfer_length     = 0;
   urb->complete_callback   = hdlc_tx_callback;
   /* rx */
   for (i = 0; i < 2; i++) {
      urb                      = &hdlc_rx_urb[i];
      urb->usb_device          = &sw_device;
      urb->endpoint_address    = sw_data_eps[1].descriptor->bEndpointAddress;
      urb->transfer_buffer_ptr = hdlc_rx_buffer[i];
      urb->transfer_length     = RX_BUF_SIZE;
      urb->complete_callback   = hdlc_rx_callback;
      urb->actual_length       = 0;
   }
   /* cdc_notify_urb */
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   if ( 0 != hdlc_use_cdcacm ) {
      urb = &cdc_notify_urb;
      urb->usb_device          = &sw_device;
      urb->endpoint_address    = hdlc_usb_config_cdcacm.ep_notify.bEndpointAddress;
      urb->transfer_length     = 0;
      urb->complete_callback   = NULL;
   }
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
   return &sw_device;
}

void qhsusb_fd_hdlc_shutdown(void)
{
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   hdlc_shutdown_in_progress = 1;
   if ( 0 != hdlc_use_cdcacm ) {
      uint32 core_id = sw_device.core_id;
      qhsusb_dci_cancel_transfer(core_id, 1, QHSUSB_EP_DIR_RX);
      qhsusb_dci_cancel_transfer(core_id, 1, QHSUSB_EP_DIR_TX);
      qhsusb_dci_cancel_transfer(core_id, 2, QHSUSB_EP_DIR_RX);
      qhsusb_dci_cancel_transfer(core_id, 2, QHSUSB_EP_DIR_TX);
      return;
   }
#endif
   qhsusb_dcd_shutdown(&sw_device);
}

void qhsusb_fd_hdlc_tx_byte(unsigned char chr)
{
   struct qhsusb_urb* urb = &hdlc_tx_urb;
   /**
    * @invariant current buffer not full 
    *  
    * Proof: After each Tx byte, buffer state checked and in case 
    * buffer full, it get transmitted and emptied. 
    */
   while ( QHSUSB_URB_STATUS_ACTIVE == urb->transfer_status ) {
      qhsusb_dcd_poll(&sw_device);
   }
   ((uint8*)(urb->transfer_buffer_ptr))[urb->transfer_length++] = chr;
   if ( ( ( 0x7e == chr ) && ( urb->transfer_length > 1 ) ) || 
        (urb->transfer_length >= TX_BUF_SIZE) ) { /* send */
      qhsusb_submit_urb(urb);
   }
}

int qhsusb_fd_hdlc_is_tx_active(void)
{
   if ( ( QHSUSB_URB_STATUS_ACTIVE == hdlc_tx_urb.transfer_status ) || 
        ( QHSUSB_URB_STATUS_ACTIVE == sw_device.setup_tx_urb.transfer_status ) ) {
      return 1;
   }
   return 0;
}

void qhsusb_fd_hdlc_rx_completed(struct qhsusb_urb* urb)
{
  urb->actual_length = 0;
  hdlc_init_rx(urb->usb_device);
}

