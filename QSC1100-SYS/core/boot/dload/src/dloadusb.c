/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   U S B   D R I V E R

GENERAL DESCRIPTION
  This module contains a very simple-minded polling USB driver
  for the DMSS boot downloader facility.

EXTERNALIZED FUNCTIONS
   dloadusb_drain
      To wait for the USB IN transfer to complete.
   
   dloadusb_init
      Initializes the USB for transmit and receive.

   dloadusb_receive_byte
      Returns a single byte from the rx_buffer.

   dloadusb_transmit_byte
      Buffers single byte, transmits USB packets to the host.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal' CPU operation must have
  been performed before entering this module.  dloadusb_init() should
  be called before any other functions.

   Copyright (c) 2003, 2004 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/dload/2_0/dloadusb.c#2 $ $DateTime: 2007/07/03 10:28:15 $ $Author: evedar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/07   vs      Support for zero-length-packet sent from the host.
11/21/05   sd      Fixed RVCT compiler warning.
06/28/05   amw     Corrected registed used for checking USB interrupts.
06/09/05   cc      MSM7500 support, which HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK
                   palarity changed and become HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK.   
12/22/04   rp      Removed sending a response in dloadusb_init().
                   Cleaned up use of tx_sending_short_pkt.
                   Added use of dloadusb_data.init_rx_dma_ptr for checking
                   for stale bytes.
09/20/04   rp      Updated dloadusb_init() to properly setup for control
                   transfers when already configured.
08/13/04   rp      Updated to use HWIO_USB_EPn_SYS_MEM_START_ADDR_OUTI
                   rather then out_dword(). 
08/06/04   rp      Removed code under FEATURE_PMIC as PMIC will always be
                   initialized prior to dload executing.
                   Removed xcvr type selection for SURF builds.
07/08/04   rp      Updated to use usbotg_xcvr_type in usbotg.h
06/07/04   rp      Updated gloabl data declarations to inusre that any non
                   const variables are initialized after a boot error.
05/27/04   rp      Updated dloadusb_ep_disable_ready() to use
                   USB_ENDPOINT_READY_CLR register if its defined.
05/05/04   rp      Modified dloadusb_do_usb_check() to occasionally
                   check for a full frame that needs to be processed.
                   Also updated dloadusb_data.rx_buffer to 2 buffers so
                   a new transfer maybe initiated while current data
                   is processed.
04/22/04   rp      Cleaned up header file inclusions.
04/19/04   rp      Updated processing of configuration  descriptor to 
                   indicate that device is self powered with 5 wire xcvr
                   and bus powered with OTG xcvr. 
03/25/04   rp      Added device enumeration support.
                   Modified dloadusb_do_usb_check() to process
                   all enabled function interrupts.
                   Added support for determing xcvr type (via switch setting)
                   for SURF builds.
01/27/04   rp      Updated dloadusb_init() to send a response to host 
                   since prior DMA operation may not have properly finished.
11/06/03   rp      Add clock enable to dloadusb_init().
10/29/03   rp      Fixed bug in dloadusb_ep_disable_done_int() call in
                   dloadusb_wait_for_dma_done().
                   Updated OUT transfer check in dloadusb_init().
10/13/03   rp      File cleanup for OTG peripheral support, removed serial
                   number string.
09/30/03   rp      Updated dloadusb_do_usb_check() to properly use 
                   mmu_invalidate_data_cache_lines().  
09/25/03   rp      Removed mmu_invalidate_data_cache_lines() until it is
                   supported.
09/19/03   rp      File creation for USB OTG support.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "task.h"
#ifndef FEATURE_USB
/* Make the linker happy */
const int dloadusb_dummy;
#else

#define CPU ARM
#include <string.h>
#include "comdef.h"
#include "boothw.h"
#include "usbdc.h"
#include "usbcdc.h"
#include "usbdci.h"
#include "sio.h"
#include "dloadusb.h"
#include "clkregim.h"
#include "bio.h"

#ifdef FEATURE_USB_DIAG
#include "usbqcdev.h"
#endif


extern void mmu_invalidate_data_cache_lines( uint32* start_ptr ,
                                             uint32 num_bytes );
/*===========================================================================
            GLOBAL DEFINITION
===========================================================================*/
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*--------------------------------------------------------------------------
usbdc_ep defines all endpoints
---------------------------------------------------------------------------*/
static const dloadusb_endpoint_type dloadusb_ep[USBDC_NUM_LOGICAL_ENDPOINTS][2] = 
{
  {{ /* Control OUT */
    USBDC_CONTROL_TRANSFER,       /* transfer type */
    USBDC_CONTROL_OUT_EP,         /* ep_num */
    USBDC_CONTROL_PKT_SIZE,       /* max packet size */
    USBDC_CONTROL_PKT_SIZE,       /* Size of X and Y buffers */
  },
  { /* Control IN */
    USBDC_CONTROL_TRANSFER,       /* transfer type */
    USBDC_CONTROL_IN_EP,          /* ep_num */
    USBDC_CONTROL_PKT_SIZE,       /* max packet size */
    USBDC_CONTROL_PKT_SIZE,       /* Size of X and Y buffers */
  },},
  {{ /* Unused */
    0,                            /* transfer type */
    0,                            /* ep_num */
    0,                            /* max packet size */
    0,                            /* Size of X and Y buffers */
  },
  { /* Interrupt IN */
    USBDC_INTERRUPT_TRANSFER,     /* transfer type */
    USBDC_NOTIFICATION_EP,        /* ep_num */
    USBDC_NOTIFICATION_PKT_SIZE,  /* max packet size */
    USBDC_NOTIFICATION_PKT_SIZE,  /* Size of X and Y buffers */
  },},
  {{ /* Bulk OUT */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT2,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },
  { /* Bulk IN */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT2,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },},
  {{ /* Bulk OUT */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT3,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },
  { /* Bulk IN */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT3,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },}, 
  {{ /* Bulk OUT */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT4,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },
  { /* Bulk IN */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT4,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },},
  {{ /* Bulk OUT */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT5,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  },
  { /* Bulk IN */
    USBDC_BULK_TRANSFER,           /* transfer type */
    USBDC_ENDPOINT5,               /* ep_num */
    USBDC_BULK_PKT_SIZE,           /* max packet size */
    USBDC_BULK_PKT_SIZE,           /* Size of X and Y buffers */
  }}
};

#ifdef FEATURE_USB_DIAG
/*---------------------------------------------------------------------------
 The CDC Device Descriptor.  See the USB and CDC Specs for the format.
---------------------------------------------------------------------------*/
/* Composite device */
static const __align(4) usbdc_device_descriptor_type  dloadusb_device_descriptor =
{
   0x12,
   USBDC_DESCRIPTOR_DEVICE,
   USBCDC_USB_VERSION,
   USBQCDEV_DEVICE_CLASS_CODE,
   USBQCDEV_DEVICE_SUBCLASS_CODE,
   USBQCDEV_DEVICE_PROTOCOL_CODE,
   USBDC_CONTROL_PKT_SIZE,
   USBCDC_QC_ID_CODE,
   USBQCDEV_QC_PRODUCT_ID,
   USBCDC_QC_DEVICE_ID,
   USBCDC_MANUFACTURER_STRING_INDEX,
   USBCDC_PRODUCT_STRING_INDEX,
   USBCDC_SERIAL_NUM_STRING_INDEX,
   USBDC_MAX_NUM_CONFIG
};

static const __align(4) usbqcdev_config_descriptor_type dloadusb_config_descriptor =
{
  sizeof(usbdc_configuration_descriptor_type), 
  USBDC_DESCRIPTOR_CONFIGURATION,
  sizeof(usbqcdev_config_descriptor_type),
  USBQCDEV_NUM_ACTIVE_INTERFACES,
  USBDC_FIRST_CONFIGURATION,
  0x0,                                       /* Interface index (0) */
  0,               /* Attributes will be updated based on XCVR type */
  0,               /* Max power will be updated based on XCVR type */

  /* Modem Data Interface */
  sizeof(usbdc_interface_descriptor_type),
  USBDC_DESCRIPTOR_INTERFACE,
  0,                                    /* interface number, 1 */
  USBDC_DEFAULT_ALTERNATE_SETTING,
  0x03,
  USBQCDEV_MDM_I_CLASS,
  USBQCDEV_MDM_I_SUBCLASS,
  USBQCDEV_MDM_I_PROTOCOL,
  USBCDC_DATA_INTERFACE_STRING_INDEX,

  /* Modem Notification Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_NOTIFICATION_STREAM_EP_ADDR,
  USBDC_BMATTRIB_INTERRUPT,
  USBDC_NOTIFICATION_PKT_SIZE,
  0x80,

  /* Modem Data In Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DATA_STREAM_IN_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DATA_STREAM_IN_EP_PKT_SIZE,
  0x00,

  /* Modem Data Out Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DATA_STREAM_OUT_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DATA_STREAM_OUT_EP_PKT_SIZE,
  0x00,

  /* QC DM Interface */
  sizeof(usbdc_interface_descriptor_type),
  USBDC_DESCRIPTOR_INTERFACE,
  1,                                 /* interface number, 2 */
  USBDC_DEFAULT_ALTERNATE_SETTING,
  0x02,                       
  USBQCDEV_DM_I_CLASS,
  USBQCDEV_DM_I_SUBCLASS,
  USBQCDEV_DM_I_PROTOCOL,
  USBCDC_DATA_INTERFACE_STRING_INDEX, /* point to data ionterface string  */

  /* QC DM In Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DIAG_STREAM_IN_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DIAG_STREAM_IN_EP_PKT_SIZE,
  0x00,

  /* QC DM Out Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DIAG_STREAM_OUT_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DIAG_STREAM_OUT_EP_PKT_SIZE,
  0x00,

  #ifdef FEATURE_USB_DIAG_NMEA
   /* QC NMEA Interface */
   sizeof(usbdc_interface_descriptor_type),
   USBDC_DESCRIPTOR_INTERFACE,
   2,                                 /* interface number, 3 */
   USBDC_DEFAULT_ALTERNATE_SETTING,
   0x02,                       
   USBQCDEV_DM_I_CLASS,
   USBQCDEV_DM_I_SUBCLASS,
   USBQCDEV_DM_I_PROTOCOL,
   USBCDC_DATA_INTERFACE_STRING_INDEX,/* point to data ionterface string  */

   /* QC NMEA In Endpoint */
   sizeof(usbdc_endpoint_descriptor_type),
   USBDC_DESCRIPTOR_ENDPOINT,
   USBDC_NMEA_STREAM_IN_EP_ADDR,
   USBDC_BMATTRIB_BULK,
   USBDC_NMEA_STREAM_IN_EP_PKT_SIZE,
   0x00,

   /* QC NMEA Out Endpoint */
   sizeof(usbdc_endpoint_descriptor_type),
   USBDC_DESCRIPTOR_ENDPOINT,
   USBDC_NMEA_STREAM_OUT_EP_ADDR,
   USBDC_BMATTRIB_BULK,
   USBDC_NMEA_STREAM_OUT_EP_PKT_SIZE,
   0x00,
  #endif
};
#else
/*---------------------------------------------------------------------------
 The CDC Device Descriptor.  See the USB and CDC Specs for the format.
---------------------------------------------------------------------------*/
/* Modem device */
static const __align(4) usbdc_device_descriptor_type dloadusb_device_descriptor =
{
   0x12,
   USBDC_DESCRIPTOR_DEVICE,
   USBCDC_USB_VERSION,
   USBCDC_CDC_CLASS_CODE,
   USBCDC_CDC_SUBCLASS_CODE,
   USBCDC_CDC_PROTOCOL_CODE,
   USBDC_CONTROL_PKT_SIZE,
   USBCDC_QC_ID_CODE,
   USBCDC_QC_PRODUCT_ID,
   USBCDC_QC_DEVICE_ID,
   USBCDC_MANUFACTURER_STRING_INDEX,
   USBCDC_PRODUCT_STRING_INDEX,
   USBCDC_SERIAL_NUM_STRING_INDEX,
   USBDC_MAX_NUM_CONFIG
};

static const __align(4) usbdc_config1_descriptor_type dloadusb_config_descriptor =
{
  sizeof(usbdc_configuration_descriptor_type),
  USBDC_DESCRIPTOR_CONFIGURATION,
  sizeof(usbdc_config1_descriptor_type),
  USBDC_NUM_INTERFACES,
  USBDC_FIRST_CONFIGURATION,
  0x0,                                       /* Interface index (0)  */
  0,               /* Attributes will be updated based on XCVR type */
  0,               /* Max power will be updated based on XCVR type */

  /* First Interface  */
  /* CDC Control Interface */
  sizeof(usbdc_interface_descriptor_type),
  USBDC_DESCRIPTOR_INTERFACE,
  USBCDC_CDC_INTERFACE,
  USBDC_DEFAULT_ALTERNATE_SETTING,
  0x01,                                     /* Num of endpoints   */
  USBCDC_CDC_I_CLASS,
  USBCDC_CDC_I_SUBCLASS_AC,
  USBCDC_CDC_I_PROTOCOL_V25TER,
  0x00,

  /* Header Functional Descriptor */
  sizeof(usbdc_header_functional_descriptor_type),
  USBCDC_DESCRIPTOR_CS_INTERFACE,
  USBCDC_HEADER,
  USBCDC_CDC_VERSION,

  /* Call Management Functional Descriptor */
  sizeof(usbdc_call_management_functional_descriptor_type),
  USBCDC_DESCRIPTOR_CS_INTERFACE,
  USBCDC_CALL_MANAGEMENT,
  0x03,
  USBCDC_DATA_INTERFACE,

  /* Abstract Control Management Functional Descriptor */
  sizeof(usbdc_abstract_control_functional_descriptor_type),
  USBCDC_DESCRIPTOR_CS_INTERFACE,
  USBCDC_ABSTRACT_CONTROL,
  0x0F,

  /* Union Interface Functional Descriptor  */
  sizeof(usbdc_union_functional_descriptor_type),
  USBCDC_DESCRIPTOR_CS_INTERFACE,
  USBCDC_UNION,
  USBCDC_CDC_INTERFACE,
  USBCDC_DATA_INTERFACE,

  /* CDC Notification Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_NOTIFICATION_STREAM_EP_ADDR,
  USBDC_BMATTRIB_INTERRUPT,
  USBDC_NOTIFICATION_PKT_SIZE,
  0x20,

  /* Second Interface */
  /* CDC Data Interface */
  sizeof(usbdc_interface_descriptor_type),
  USBDC_DESCRIPTOR_INTERFACE,
  USBCDC_DATA_INTERFACE,
  USBDC_DEFAULT_ALTERNATE_SETTING,
  0x02,
  USBCDC_CDC_DATA_CLASS,
  USBCDC_CDC_DATA_SUBCLASS,
  USBCDC_CDC_DATA_PROTOCOL,
  0x04,

  /* CDC Data In Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DATA_STREAM_IN_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DATA_STREAM_IN_EP_PKT_SIZE,
  0x00,

  /* CDC Data Out Endpoint */
  sizeof(usbdc_endpoint_descriptor_type),
  USBDC_DESCRIPTOR_ENDPOINT,
  USBDC_DATA_STREAM_OUT_EP_ADDR,
  USBDC_BMATTRIB_BULK,
  USBDC_DATA_STREAM_OUT_EP_PKT_SIZE,
  0x00,
};
#endif

/* String definitions */
static const __align(4) usbdc_manufacturer_string_descriptor_type
      dloadusb_manufacturer_string_descriptor =
{
   sizeof(usbdc_manufacturer_string_descriptor_type),
   USBDC_DESCRIPTOR_STRING,
   {
      0x0051, /*  Q  */
      0x0075, /*  u  */
      0x0061, /*  a  */
      0x006C, /*  l  */
      0x0063, /*  c  */
      0x006F, /*  o  */
      0x006D, /*  m  */
      0x006D, /*  m  */
      0x002C, /*  ,  */
      0x0020, /* [space] */
      0x0049, /*  I */
      0x006E, /*  n */
      0x0063, /*  c */
      0x006F, /*  o */
      0x0072, /*  r */
      0x0070, /*  p */
      0x006F, /*  o */
      0x0072, /*  r */
      0x0061, /*  a */
      0x0074, /*  t */
      0x0065, /*  e */
      0x0064  /*  d */
   }
};

static const __align(4) usbdc_product_string_descriptor_type
      dloadusb_product_string_descriptor =
{
   sizeof(usbdc_product_string_descriptor_type),
   USBDC_DESCRIPTOR_STRING,
   {
      0x0051, /*  Q */
      0x0075, /*  u */
      0x0061, /*  a */
      0x006C, /*  l */
      0x0063, /*  c */
      0x006F, /*  o */
      0x006D, /*  m */
      0x006D, /*  m */
      0x0020, /*  [space] */
      0x0043, /*  C */
      0x0044, /*  D */
      0x004D, /*  M */
      0x0041, /*  A */
      0x0020, /* [space] */
      0x0054, /*  T */
      0x0065, /*  e */
      0x0063, /*  c */
      0x0068, /*  h */
      0x006E, /*  n */
      0x006F, /*  o */
      0x006C, /*  l */
      0x006F, /*  o */
      0x0067, /*  g */
      0x0069, /*  i */
      0x0065, /*  e */
      0x0073, /*  s */
      0x0020, /*  [space] */
      0x004D, /*  M  */
      0x0053, /*  S  */
      0x004D  /*  M  */
   }
};

static const __align(4) usbdc_data_interface_string_descriptor_type
      dloadusb_data_interface_string_descriptor =
{
   sizeof(usbdc_data_interface_string_descriptor_type),
   USBDC_DESCRIPTOR_STRING,
   {
      0x0044, /*  D */
      0x0061, /*  a */
      0x0074, /*  t */
      0x0061, /*  a */
      0x0020, /*  [space] */
      0x0049, /*  I */
      0x006E, /*  n */
      0x0074, /*  t */
      0x0065, /*  e */
      0x0072, /*  r */
      0x0066, /*  f */
      0x0061, /*  a */
      0x0063, /*  c */
      0x0065  /*  e */
   }
};

/* This is the language descriptor.  Only English is supported.
 * This descriptor is define in Table 9-11 of the USB spec.
 */
static const __align(4) usbdc_lang_support_string_type dloadusb_lang_support_string =
{
   sizeof(usbdc_lang_support_string_type),
   USBDC_DESCRIPTOR_STRING,
   USBDC_ENGLISH_LANGUAGE
};


static const serial_iface_info_type default_serial_info =
{
  115200,                          /* 115,200 baud data rate */
  USBDC_CHAR_FORMAT_1STOP_BIT,     /* 1 stop bit */
  USBDC_PARITY_NONE,               /* no parity */
  8,                               /* 8 data bits/byte */
  FALSE,                           /* carrier detect */
  FALSE,                           /* ring indicator */
  FALSE,                           /* data set ready */
  FALSE,                           /* state of break detection */
  FALSE                            /* request to send */
};

/* Local data required for all bus transfers */
static dloadusb_data_type dloadusb_data;

static void  dloadusb_process_setup_get_descriptor
(
  usbdc_setup_type *setup,             /* setup message parameters */
  word *bytes_to_transfer,
  byte **dscriptor_data
);

static void dloadusb_init_hw
(
  void
);

static void dloadusb_wait_for_dma_done
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
);

static void dloadusb_ep_clear_xbuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);

static void dloadusb_ep_clear_ybuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);

static void dloadusb_ep_disable_done_int
(
  int ep_num,               /* logical EP number */
  usbdc_ep_direction_type ep_dir, /* disable IN, OUT or both directions
                               DONE int of a logic EP             */
  boolean flush_flag
);

static void dloadusb_ep_disable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
);


/*===========================================================================
 FUNCTION dloadusb_ep_get_descriptor_addr


DESCRIPTION
  This function retune the address of DWORD0 of EP's descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The end point must be cleared by the host.
===========================================================================*/
static dword * dloadusb_ep_get_descriptor_addr
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword *ep_mem_ptr;

  ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  /* point to dowrd0 of OUT EP */
  ep_mem_ptr += (4*2*ep_num );

  if ( ep_dir == USBDC_EP_DIR_IN)
  {
    ep_mem_ptr += 4;
  }

  return(ep_mem_ptr);
}

/*===========================================================================
FUNCTION dloadusb_ep_clear_xfilled

DESCRIPTION
  This clears the Xfilled register corresponding to the ep_num's OUT EndPoint.
  This indicates to the HW that Xbuffer has beed drained and function cntroller
  can begin receiving data from the next OUT token.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_xfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_xfilled_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_xfilled_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_xfilled_mask = 2 << ep_num*2;
    break;
  }

  if (HWIO_IN( USB_F_X_FILLED_STATUS) & ep_xfilled_mask)
  {
      HWIO_OUT( USB_F_X_FILLED_STATUS, ep_xfilled_mask);
  }
  /* else, xfilled is already cleared, do nothing */
}

/*===========================================================================
FUNCTION dloadusb_ep_clear_yfilled

DESCRIPTION
  This clears the Yfilled register corresponding to the ep_num's OUT EndPoint.
  This indicates to the HW that Ybuffer has beed drained and function cntroller
  can begin receiving data from the next OUT token.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_yfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_yfilled_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_yfilled_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_yfilled_mask = 2 << ep_num*2;
    break;
  }

  if (HWIO_IN( USB_F_Y_FILLED_STATUS) & ep_yfilled_mask)
  {
    HWIO_OUT( USB_F_Y_FILLED_STATUS, ep_yfilled_mask);
  }
}

/*===========================================================================
FUNCTION dloadusb_ep_clear_done_status
DESCRIPTION
  This clears the USB_F_ENDPOINT_DONE_STATUS register corresponding to the
  ep_num's ep_dir EndPoint.

  Note:   USB_F_ENDPOINT_DONE_STATUS register bits flip when write to it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_done_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_clear_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_clear_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_clear_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_F_ENDPOINT_DONE_STATUS) & ep_clear_mask )
  { /* clear DONE_STATUS only if it is set */
    HWIO_OUT( USB_F_ENDPOINT_DONE_STATUS, ep_clear_mask);
  }
}

/*===========================================================================

FUNCTION dloadusb_ep_enable_done_int

DESCRIPTION
  This routine enables EndPoint Done interrupt associated with an
  endpoint.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dloadusb_ep_enable_done_int
(
  int ep_num,
  usbdc_ep_direction_type ep_dir /* enable IN, OUT or both directions
                                    DONE int of a logic EP           */
)
{
  dword ep_done_enable_val;

  dword ep_done_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_done_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_done_mask = 2 << ep_num*2;
    break;
  }

  ep_done_enable_val = HWIO_IN(USB_ENDPOINT_DONE_ENA);
  ep_done_enable_val |= ( ep_done_mask );  /*enable both IN and OUT DONE */
  HWIO_OUT( USB_ENDPOINT_DONE_ENA, ep_done_enable_val);

} /* usbdc_ep_enable_done_int */

/*===========================================================================  
FUNCTION dloadusb_ep_enable_endpoint 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_enable_endpoint
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_enable_mask = 0; 
 
  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_enable_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_enable_mask = 2 << ep_num*2;
    break;
  }
  
  if ( !(HWIO_IN(USB_ENDPOINT_ENA) & ep_enable_mask) ) 
  { 
    /* note, This register is "Write to Clear" type */
    /* the goal here is to make sure it has "1" as desired*/
    HWIO_OUT(USB_ENDPOINT_ENA, ep_enable_mask);
  }
}

/*===========================================================================  
FUNCTION dloadusb_ep_disable_endpoint 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_disable_endpoint
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_disable_mask = 0; 

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_disable_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_disable_mask = 2 << ep_num*2;
    break;
  }
  
  if ( HWIO_IN(USB_ENDPOINT_ENA) & ep_disable_mask )
  {
    HWIO_OUT( USB_ENDPOINT_ENA, ep_disable_mask);
  }
}

/*===========================================================================
FUNCTION dloadusb_ep_enable_ready

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_enable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_ready_mask =0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_ready_mask = 1<<ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_ready_mask = 2 << ep_num*2;
    break;
  }

  if ( !(HWIO_IN(USB_ENDPOINT_READY) & ep_ready_mask))
  {
    HWIO_OUT( USB_ENDPOINT_READY, ep_ready_mask);
  }
}

/*===========================================================================  
FUNCTION dloadusb_ep_disable_ready 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_disable_ready
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_ready_mask = 0; 

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_ready_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_ready_mask = 2 << ep_num*2;
    break;
  }

  /* Clear the ready bit */
#ifdef HWIO_USB_ENDPOINT_READY_CLR_ADDR
  HWIO_OUT(USB_ENDPOINT_READY_CLR, ep_ready_mask);  
#else
  /*--------------------------------------------------------------------- 
     USB_ENDPOINT_READY register toggles, HW can write to it too. 
     TDI provided HW fix, see CITS#74 for detail.  
  ----------------------------------------------------------------------*/   
  HWIO_OUT( USB_ENDPOINT_READY, (ep_ready_mask|0x40000000) );  
#endif
}

/*===========================================================================
FUNCTION dloadusb_setup_rx_DMA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_setup_rx_DMA
(
  int ep_num,
  byte *rx_buffer,
  int total_byte_count
)
{
  dword *ep_mem_ptr;
  dword temp_dword;

  /* set DWORD3 of EP for ttlbtecnt */
  ep_mem_ptr = dloadusb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_OUT);
  temp_dword = *(ep_mem_ptr +3 );
  temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
  temp_dword += total_byte_count;
  *(ep_mem_ptr +3 ) = temp_dword;

  /* set system mem start addr */
  HWIO_USB_EPn_SYS_MEM_START_ADDR_OUTI(2*ep_num, rx_buffer);

  /* Save this value for stale check */
  dloadusb_data.init_rx_dma_ptr = (dword) rx_buffer;

  /* Tell USB core that RX buffer is drained & ready for rxing more data */
  dloadusb_ep_clear_xfilled(ep_num, USBDC_EP_DIR_OUT);
  dloadusb_ep_clear_yfilled(ep_num, USBDC_EP_DIR_OUT);

  /* Enable DMA */
  HWIO_OUT (USB_EP_DMA_ENA,  ( 1 << 2*ep_num ) );

  /* Clear the done status from any previous transfer */
  dloadusb_ep_clear_done_status(ep_num, USBDC_EP_DIR_OUT);
  
  /* Enable the done interrupt status */
  dloadusb_ep_enable_done_int(ep_num, USBDC_EP_DIR_OUT);

  /* Enable EP */
  dloadusb_ep_enable_ready(ep_num, USBDC_EP_DIR_OUT);
  
} /* usbdc_setup_rx_DMA */

/*===========================================================================
FUNCTION dloadusb_setup_tx_DMA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_setup_tx_DMA
(
  int ep_num,
  byte *tx_buffer,
  int total_byte_count
)
{
  dword *ep_mem_ptr;
  dword temp_dword;

  ep_mem_ptr = dloadusb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_IN );

  /* There should not an active transfer */
  if ( ((*(ep_mem_ptr +3)) & USBDC_ENDPOINT_TTLBTECNT_M) != 0)
  {
    /* Flush to get back to a known state */
    dloadusb_ep_disable_done_int(ep_num, USBDC_EP_DIR_IN, TRUE);
  }

  dloadusb_ep_clear_xfilled(ep_num, USBDC_EP_DIR_IN);
  dloadusb_ep_clear_yfilled(ep_num, USBDC_EP_DIR_IN);
                                    
  /* set DWORD3 of EP for ttlbtecnt */
  temp_dword = *(ep_mem_ptr +3 );
  temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
  temp_dword += total_byte_count;
  *(ep_mem_ptr +3 ) = temp_dword;

  /* point to the data */
  HWIO_USB_EPn_SYS_MEM_START_ADDR_OUTI((2*ep_num + 1), tx_buffer);

  /* enable this DMA transfer */
  HWIO_OUT(USB_EP_DMA_ENA,  ( 2 << 2*ep_num ) );

  dloadusb_ep_clear_xbuf_int_status(ep_num,  USBDC_EP_DIR_IN);
  dloadusb_ep_clear_ybuf_int_status(ep_num,  USBDC_EP_DIR_IN);
  
  /* ready EP */
  dloadusb_ep_enable_ready(ep_num, USBDC_EP_DIR_IN);

  /* Enable the done interrupt status */
  dloadusb_ep_enable_done_int(ep_num, USBDC_EP_DIR_IN);

}  /* dloadusb_setup_tx_DMA */

/*===========================================================================
FUNCTION dloadusb_ep_set_toggle_bit

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_set_toggle_bit
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_toggle_val;
  dword ep_toggle_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_toggle_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_toggle_mask = 2 << ep_num*2;
    break;
  }

  ep_toggle_val = HWIO_IN(USB_ENDPOINT_TOGGLE_BITS);
  if ((ep_toggle_val & ep_toggle_mask) == 0)
  {
    ep_toggle_val |=  ep_toggle_mask;
    HWIO_OUT(USB_ENDPOINT_TOGGLE_BITS, ep_toggle_val);
  }

}

/*===========================================================================
FUNCTION dloadusb_ep_clear_toggle_bit

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_toggle_bit
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_toggle_val;
  dword ep_toggle_mask = 0;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_toggle_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_toggle_mask = 2 << ep_num*2;
    break;
  }

  ep_toggle_val = HWIO_IN(USB_ENDPOINT_TOGGLE_BITS);
  if (ep_toggle_val & ep_toggle_mask)
  {
    /* Disable endpoint */
    dloadusb_ep_disable_endpoint(ep_num, ep_dir);

    if (ep_toggle_val & ep_toggle_mask)
    {
      HWIO_OUT( USB_ENDPOINT_TOGGLE_BITS, ep_toggle_mask);
    }
    
    /* Enable endpoint */
    dloadusb_ep_enable_endpoint(ep_num, ep_dir);
  }
}

/*===========================================================================

FUNCTION dloadusb_ep_clear_stall

DESCRIPTION 
  This function clears an end point stall status.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  The end point must be cleared by the host.

===========================================================================*/
static void dloadusb_ep_clear_stall
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword *ep_mem_ptr;

  ep_mem_ptr = dloadusb_ep_get_descriptor_addr(ep_num,ep_dir );

  (*ep_mem_ptr) &= ~USBDC_ENDPOINT_STALL_M; 
}

/*===========================================================================
FUNCTION dloadusb_ep_enable_xfilled

DESCRIPTION
  Set the Xfilled registor for the bit corresponding to ep_num's IN Endpoint.
  This indicates to HW that EP's Xbuffer has been filled and the function
  controller can begin sending the data on the next IN token.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_set_xfilled
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{

  dword ep_xfilled_mask = 0;

  ep_xfilled_mask = 2 << ep_num*2;

  if ( !(HWIO_IN(USB_F_X_FILLED_STATUS) & ep_xfilled_mask) )
  { /* note:  This Register toggles when white to it */
    /* write to set only if it is not set yet */
    HWIO_OUT( USB_F_X_FILLED_STATUS, ep_xfilled_mask);
  }
}


/*===========================================================================
FUNCTION dloadusb_clear_DMA_channel

DESCRIPTION
  Per TDI OTG-IP data sheet, this would "clear the DMA channel corresponding
  too whichever bit is set and also disable the DMA transfer.  It can be used to
  abort a DMA transfer."   It "also clear the DMA enable bit for the EP".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_clear_DMA_channel
(
  int ep_num,
  usbdc_ep_direction_type ep_dir
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case  USBDC_EP_DIR_OUT:
    ep_mask =  1 << (2*ep_num);
    break;

  case USBDC_EP_DIR_IN:
    ep_mask =  2 << (2*ep_num);
    break;
  }
  if ( HWIO_IN(USB_EP_DMA_ENA) & ep_mask)
  {
    HWIO_OUT(USB_EP_DMA_CHANNEL_CLEAR, ep_mask );
  }
}  /* dloadusb_clear_DMA_channel */

/*===========================================================================

FUNCTION dloadusb_disable_ep_done_int

DESCRIPTION
  This routine disables EndPoint DONE interrupt associated with an
  endpoint IN or OUT. It will also optionly flush the buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dloadusb_ep_disable_done_int
(
  int ep_num,               /* logical EP number */
  usbdc_ep_direction_type ep_dir, /* disable IN, OUT or both directions
                               DONE int of a logic EP             */
  boolean flush_flag
)
{
  dword ep_done_enable_val;
  dword ep_done_mask = 0;
  dword *ep_mem_ptr;

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_done_mask = 1 << ep_num*2;
    break;
  case USBDC_EP_DIR_IN:
    ep_done_mask = 2 << ep_num*2;
    break;
  }

  ep_done_enable_val = HWIO_IN(USB_ENDPOINT_DONE_ENA);
  ep_done_enable_val &= ~( ep_done_mask );   /*disable EP DONE INT*/
  HWIO_OUT(USB_ENDPOINT_DONE_ENA, ep_done_enable_val);
  
  if (flush_flag )
  {
    dloadusb_ep_disable_ready(ep_num,ep_dir);
    dloadusb_ep_clear_done_status(ep_num,ep_dir);

    /* clear total_byte_cnt */
    ep_mem_ptr = dloadusb_ep_get_descriptor_addr(ep_num,ep_dir);
    *(ep_mem_ptr+3) &=  ~USBDC_ENDPOINT_TTLBTECNT_M;  

    /* Only clear the DMA channel if byte count has not beene reset */
    if ( (*(ep_mem_ptr+3) &  ~USBDC_ENDPOINT_TTLBTECNT_M) != 0 ) 
    {
      dloadusb_clear_DMA_channel(ep_num,ep_dir);
    }

    dloadusb_ep_clear_xfilled(ep_num,ep_dir);
    dloadusb_ep_clear_yfilled(ep_num,ep_dir);
  }

  /* clear INT_DONE_STATUS register */
  dloadusb_ep_clear_done_status(ep_num,ep_dir);

} /* dloadusb_ep_disable_done_int */

/*===========================================================================  
FUNCTION dloadusb_ep_clear_xbuf_int_status 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_xbuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_mask = 0; 

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_mask = 2 << ep_num*2;
    break;
  }

  if ( HWIO_IN(USB_F_X_BUFFER_INT_STATUS) & ep_mask )
  {
    HWIO_OUT(USB_F_X_BUFFER_INT_STATUS, ep_mask);
  }
}

/*===========================================================================  
FUNCTION dloadusb_ep_clear_ybuf_int_status 

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_ep_clear_ybuf_int_status
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT*/
)
{
  dword ep_mask = 0; 

  switch (ep_dir)
  {
  case USBDC_EP_DIR_OUT:
    ep_mask = 1 << ep_num*2;
    break;

  case USBDC_EP_DIR_IN:
    ep_mask = 2 << ep_num*2;
    break;
  }
  
  if ( HWIO_IN(USB_F_Y_BUFFER_INT_STATUS) & ep_mask )
  {
    HWIO_OUT(USB_F_Y_BUFFER_INT_STATUS, ep_mask);
  }
}

/*===========================================================================
FUNCTION dloadusb_send_zero_length_pkt

DESCRIPTION
   Initiate a 0 lenght packet transfer for the given IN endpoint.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_send_zero_length_pkt
(
  int ep_num                       /* logic ep umber */
)
{
  dloadusb_ep_set_xfilled(ep_num, USBDC_EP_DIR_IN);

  /* Enable the done interrupt status */
  dloadusb_ep_enable_done_int(ep_num, USBDC_EP_DIR_IN);

  /* ready the EP */
  dloadusb_ep_enable_ready(ep_num, USBDC_EP_DIR_IN);

  /* Update Flag */
  dloadusb_data.tx_sending_short_pkt = TRUE;
}

/*===========================================================================
FUNCTION dloadusb_wait_for_dma_done

DESCRIPTION
  Wait for the DMA hardware to indicate the completion
  of a transfer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_wait_for_dma_done
(
  int ep_num,                       /* logic ep umber */
  usbdc_ep_direction_type ep_dir    /* ep direction, IN or OUT */
)
{
  dword ep_mask = 0;

  switch (ep_dir)
  {
  case  USBDC_EP_DIR_OUT:
    ep_mask =  1 << (2*ep_num);
    break;

  case USBDC_EP_DIR_IN:
    ep_mask =  2 << (2*ep_num);
    break;
  }

  /* Wait for completion */
  while (HWIO_IN(USB_EP_DMA_ENA) & ep_mask)
  {
  }

  /* Clear done condition */
  dloadusb_ep_disable_done_int(ep_num, ep_dir, FALSE);
}

/*===========================================================================

FUNCTION dloadusb_do_handle_setup_msgs

DESCRIPTION
  Processes a received setup request message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

static void dloadusb_do_handle_setup_msgs
(
  void
)
{
  word reply_length;
  byte *reply_data_ptr;
  dword temp_dword;
  dword *buf_dword_ptr = (dword *) HWIO_ADDR(USB_DATA_MEMORY_BASE);
  dword *data_mem_ptr, *ep_mem_ptr;
  
  /* struct to save the setup request message parameters */
  usbdc_setup_type setup;

  word bytes_to_transfer;
  byte *dscriptor_data;

  int ep_num;                        /* logic ep umber */
  usbdc_ep_direction_type ep_dir;    /* ep direction, IN or OUT*/

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
               
  /* Assume no data to return */
  reply_length = 0;
  reply_data_ptr = NULL;

  /* Process any pending setup message */
  BOOTHW_KICK_WATCHDOG ();

  if (dloadusb_data.setup_data_stage_flag)
  {
    /* Process the data stage of standard usb setup request. So far, only
       SET_LINE_CODING has this stage. */
    dloadusb_data.setup_data_stage_flag = FALSE;

    data_mem_ptr = (dword *) HWIO_ADDR(USB_DATA_MEMORY_BASE);

    *((dword *) &dloadusb_data.line_coding[0]) =
       *(data_mem_ptr++);

    temp_dword = *(data_mem_ptr++);
    dloadusb_data.line_coding[4] = (byte) (temp_dword & 0xff);

    temp_dword = temp_dword >>8;
    dloadusb_data.line_coding[5] = (byte) (temp_dword & 0xff);

    temp_dword = temp_dword >>8;
    dloadusb_data.line_coding[6] =  (byte) (temp_dword & 0xff);
  }
  else
  {
    /* Read in the setup message from EP 0 OUT X buffer */
    temp_dword = *(buf_dword_ptr++);
    setup.bmRequest_and_Type = temp_dword;
    setup.wValue.value = temp_dword >> 16;
    temp_dword = *(buf_dword_ptr++);
    setup.wIndex = temp_dword;
    setup.wLength = temp_dword >> 16;

    /*
    ** For standard device requests that require no Data stage, a device
    ** must be able to complete the request and be able to sucessfully
    ** complete the Status stage of the request within 50 ms of receipt
    ** of the request.
    **/

    switch (setup.bmRequest_and_Type)
    {
    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_GET_LINE_CODING:
      {
        /* The line coding information is already extracted from the
           usbdc_device structure and stored in the line_coding array. */
        reply_data_ptr = dloadusb_data.line_coding;
        reply_length = setup.wLength;
        break;
      }

    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_SET_CONTROL_LINE_STATE_I:
      {
        break;
      }

    /* Section 6.2.1 Send Encapsulated Command */
    case USBDC_SET_LINE_CODING:
      {
        /* The set line coding (setup) message transfer includes a data
           packet with all of the line information present. We read the data
           from the next OUT transfer. */
        dloadusb_data.setup_data_stage_flag = TRUE;
        break;
      }  /* end case - USBDC_SET_LINE_CODING */

    /* 9.4.3 Get Descriptor:
     *
     * The wValue field specifices the descriptor type in the high
     * byte and the descriptor index in the low byte.  The wIndex
     * filed specifies the Language ID for string descriptors or
     * is reset to zero for other descriptors.  The wLength field
     * specifies the number of bytes' to return.  If the descriptor
     * is longer that the wLength field, only the intital bytes
     * of the descriptor are returned.
     */
    case USBDC_SETUP_GET_DESCRIPTOR:
      {
        /* Process the selected descriptor. */
        dloadusb_process_setup_get_descriptor(&setup,
                                         &bytes_to_transfer,
                                         &dscriptor_data);

        /* Send the dscriptor to the host. */
        reply_data_ptr = dscriptor_data;
        reply_length = bytes_to_transfer;
        break;
      } /* end case - USBDC_SETUP_GET_DESCRIPTOR */

      /*
       *  9.4.6 Set Address
       *  The request sets the device address for all future device
       *  accesses.
       *  Transaction stages after the initial Setup packet assume
       *  the same device address as the Setup packet.
       *  The USB device does NOT change
       *  its device address until after the Status stage of this
       *  request is completed successfully.
       */
    case USBDC_SETUP_SET_ADDRESS:
      {
        /* Save the address */
        dloadusb_data.address = setup.wValue.value;

        /* The address will be set after the done interrupt */
        break;
      } /* end case - USBDC_SET_ADDRESS */

    /*
     *  9.4.7 Set Configuration
     *  This request sets the device configuration.
     */
    case USBDC_SETUP_SET_CONFIGURATION:
      {
        /* Setup to receive data from the host */
        dloadusb_ep_enable_done_int(dloadusb_data.usb_out_endpoint,
                                     USBDC_EP_DIR_OUT);
        dloadusb_data.configured = TRUE;
        dloadusb_setup_rx_DMA(dloadusb_data.usb_out_endpoint,
                              dloadusb_data.rx_buffer[0],
                              sizeof(dloadusb_data.rx_buffer[0]));
        dloadusb_data.rx_dma_index = 0;
        break;
      }

    case USBDC_SETUP_CLEAR_FEATURE_E:
      {
        /* 
         *  A clear_feature endpoint_stall message. Therefore,
         *  clear the stalled endpoint.
         */
        if ( (setup.wIndex & 0x80) == 0)
        {
          ep_dir = USBDC_EP_DIR_OUT;
        }
        else
        {
          ep_dir = USBDC_EP_DIR_IN;
        }
        ep_num = (int)  (setup.wIndex & 0x0f);


        dloadusb_ep_clear_toggle_bit(ep_num, ep_dir);   

        dloadusb_ep_clear_stall(ep_num, ep_dir);
        break;
      }

    default:
      /* for now, let's just say that we are happy with any command from
         the host */
      break;
    } /* end of switch (bmRequest_and_Type) */
  }

  dloadusb_ep_clear_xfilled( USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);
  dloadusb_ep_clear_yfilled( USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);

  /* Clear Setup Message Received */
  ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  *ep_mem_ptr &= ~USBDC_ENDPOINT_SETUP_RXED_M;

  /* Increment by 3 dwords for EP 0 OUT dword3 */
  ep_mem_ptr += 3;

  /* clear TBC before set its value */
  (*ep_mem_ptr) &= ~USBDC_ENDPOINT_TTLBTECNT_M;

  if (dloadusb_data.setup_data_stage_flag)
  {
    *(ep_mem_ptr) += setup.wLength;
  }
  else
  {
    /* Set the transfer count */
     *(ep_mem_ptr) += (USBDC_CONTROL_PKT_SIZE << 1);
  }

  dloadusb_ep_enable_ready(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);

  if (dloadusb_data.setup_data_stage_flag == FALSE)
  {
    /* Initiate transfer of data or 0 lenght packet */
    if (reply_length > 0)
    {
      dloadusb_setup_tx_DMA(USBDC_CONTROL_IN_EP, 
                         reply_data_ptr, 
                         reply_length);
    }
    else
    {
      /* Send 0 lenght packet to complete control transfer */
      dloadusb_send_zero_length_pkt(USBDC_CONTROL_IN_EP);
    }
  }  /* end of--- if(!usbdc_setup_data_stage_flag) */
} /* end of dloadusb_do_handle_setup_msgs() */

/*===========================================================================

FUNCTION dloadusb_process_setup_get_descriptor

DESCRIPTION
9.4.3 Get Descriptor:

The wValue field specifices the descriptor type in the high byte and the descriptor index
in the low byte.  The wIndex filed specifies the Language ID for string descriptors or
is reset to zero for other descriptors.  The wLength field specifies the number of bytes'
to return.  If the descriptor is longer that the wLength field, only the intital bytes
of the descriptor are returned.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void  dloadusb_process_setup_get_descriptor
(
  usbdc_setup_type *setup,             /* setup message parameters */
  word *bytes_to_transfer,
  byte **dscriptor_data
)
{
  usbdc_configuration_descriptor_type *data_config_ptr;

  /* parse by the descriptor type requested */
  switch (setup->wValue.descriptor.type)
  {
    /* Send the device descriptor */
    case USBDC_DESCRIPTOR_DEVICE:
      {
        /* Check the parameters for this request */
        if (setup->wIndex == 0)
        {
          *bytes_to_transfer =
              MIN(setup->wLength,
                   sizeof(usbdc_device_descriptor_type));
          *dscriptor_data = (byte *) &dloadusb_device_descriptor;
        }
        break;
      } /* end case - USBDC_DESCRIPTOR_DEVICE */

    /* Send the configuration descriptor */
    case USBDC_DESCRIPTOR_CONFIGURATION:
      {
        /* Check the parameters for this request */
        if ( (setup->wIndex == 0)     &&
             (setup->wValue.descriptor.index <=
              USBDC_MAX_NUM_CONFIG)            )
        {
          /* Copy the config descriptor const data */
          memcpy(dloadusb_data.tx_buffer,
                 (byte*) &dloadusb_config_descriptor,
                 sizeof(dloadusb_config_descriptor));
          
          /* Update the power attributes based on xcvr type */
          data_config_ptr = (usbdc_configuration_descriptor_type *) 
                                dloadusb_data.tx_buffer;
          if (dloadusb_data.xcvr_type == USBOTG_FIVE_WIRE_XCVR)
          {
            data_config_ptr->attributes = USBCDC_CONFIG_ATTR_FIVE_WIRE_XCVR;
            data_config_ptr->max_power = USBCDC_MAX_POWER;
          }
          else
          {
            data_config_ptr->attributes = USBCDC_CONFIG_ATTR_OTG_XCVR;
            data_config_ptr->max_power = 0;
          }

          *bytes_to_transfer =
              MIN(setup->wLength,
                   sizeof(dloadusb_config_descriptor));
          *dscriptor_data = dloadusb_data.tx_buffer;
        }
        break;
      } /* end case - USBDC_DESCRIPTOR_CONFIGURATION */

    /* Send the string descriptor */
    case USBDC_DESCRIPTOR_STRING:
      {
        switch (setup->wValue.descriptor.index)
        {
          case USBCDC_LANGUAGE_SUPPORTED_INDEX:
            {
              *bytes_to_transfer =
                  MIN(setup->wLength,
                    sizeof(usbdc_lang_support_string_type));
              *dscriptor_data = (byte *) &dloadusb_lang_support_string;
              break;
            } /* end case - USBCDC_LANGUAGE_SUPPORTED_INDEX */

          case USBCDC_MANUFACTURER_STRING_INDEX:
            {
              *bytes_to_transfer =
                MIN(setup->wLength,
                    sizeof(usbdc_manufacturer_string_descriptor_type));
              *dscriptor_data = (byte *)
                                 &dloadusb_manufacturer_string_descriptor;
              break;
            } /* end case - USBCDC_MANUFACTURER_STRING_INDEX */

          case USBCDC_PRODUCT_STRING_INDEX:
            {
              *bytes_to_transfer =
                  MIN(setup->wLength,
                        sizeof(usbdc_product_string_descriptor_type));
              *dscriptor_data = (byte *) &dloadusb_product_string_descriptor;
              break;
            } /* end case - USBCDC_PRODUCT_STRING_INDEX */

          case USBCDC_DATA_INTERFACE_STRING_INDEX:
            {
              *bytes_to_transfer =
                  MIN(setup->wLength,
                    sizeof(usbdc_data_interface_string_descriptor_type));
              *dscriptor_data =
                  (byte *) &dloadusb_data_interface_string_descriptor;
              break;
            } /* end case - USBCDC_DATA_INTERFACE_STRING_INDEX */

          default:
            {
              break;
            }
         } /* end switch - STRING index */
      } /* end case - USBCDC_DESCRIPTOR_STRING:  */
      break;


      /* Currently not handled. */
    default:
    {
      break;
    } /* end case - default */
  } /* end switch - descriptor type */
} /* end dloadusb_process_setup_get_descriptor */

/*==========================================================================
FUNCTION usbdc_process_tx_done(int ep_num)

DESCRIPTION
  Processes interrupt associated with the bulk IN endpoint.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dloadusb_process_tx_done
( 
  int ep_num
)
{
  dword *ep_mem_ptr;
  int num_bytes_leftover;

  ep_mem_ptr = dloadusb_ep_get_descriptor_addr(ep_num, USBDC_EP_DIR_IN);

  /* read DWORD3 of EP for ttlbtecnt to insure transfer is done */
  num_bytes_leftover =( *(ep_mem_ptr + 3) ) & USBDC_ENDPOINT_TTLBTECNT_M;

  /* Work around for HW anomaly, CITS56b */ 
  if (num_bytes_leftover > 0)
  {
    return;
  }
  dloadusb_ep_clear_xbuf_int_status(ep_num, USBDC_EP_DIR_IN);
  dloadusb_ep_clear_ybuf_int_status(ep_num, USBDC_EP_DIR_IN);

  /* Check if need to send a 0 length packet */
  if (dloadusb_data.tx_sending_short_pkt == FALSE)
  {
    dloadusb_send_zero_length_pkt(ep_num);
  }
  else
  {
    /* Write is complete */
    dloadusb_ep_disable_done_int(ep_num, 
                              USBDC_EP_DIR_IN,
                              FALSE);

    /* clear_done_staus to eliminate any possible false DONE int */
    dloadusb_ep_clear_done_status(ep_num, USBDC_EP_DIR_IN);
  }

} /* dloadusb_process_tx_done */

/*===========================================================================

FUNCTION dloadusb_do_usb_check

DESCRIPTION
  This function checks for the assertion of any enabled
  USB function controller interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/
word dloadusb_do_usb_check
(
  void
)
{
  static dword loop_count = 0;
  word num_bytes_read = 0;
  dword sys_int_status;
  dword which_ep;
  dword ep_mask = 0;
  dword *ep_mem_ptr;
  dword current_dma_ptr;

  if (HWIO_USB_INT_STATUS_INM(HWIO_USB_INT_STATUS_FUNC_INT_STATUS_BMSK)
      == 0)
  {
    if (dloadusb_data.configured) 
    {
      if (loop_count++ == 50000)
      {
        /* Check for full frame and no short packet */
        current_dma_ptr = HWIO_USB_EPn_SYS_MEM_START_ADDR_INI(
          2*dloadusb_data.usb_out_endpoint);

        if ( current_dma_ptr != dloadusb_data.init_rx_dma_ptr )
        { 

          /* Get dword3 */
          ep_mem_ptr = dloadusb_ep_get_descriptor_addr(
               dloadusb_data.usb_out_endpoint,
               USBDC_EP_DIR_OUT);

          /* Calculate bytes received */
          num_bytes_read = sizeof(dloadusb_data.rx_buffer[0]) -
              ((word) (*(ep_mem_ptr +3) & USBDC_ENDPOINT_TTLBTECNT_M));

          if (num_bytes_read)
          {
            /* Must be a full frame with no short packet */
            dloadusb_ep_disable_ready(dloadusb_data.usb_out_endpoint,
                                       USBDC_EP_DIR_OUT);
            dloadusb_ep_disable_done_int(dloadusb_data.usb_out_endpoint,
                                          USBDC_EP_DIR_OUT, FALSE);
            dloadusb_ep_clear_done_status(dloadusb_data.usb_out_endpoint,
                                           USBDC_EP_DIR_OUT);

            /* Insure that data is read from memory and not data cache */
            mmu_invalidate_data_cache_lines((uint32*) 
                     dloadusb_data.rx_buffer[dloadusb_data.rx_dma_index],
                     num_bytes_read);
          }
        }
        /* Reset loop count */
        loop_count = 0;
      }
    }

    /* No new function controller interrupt to process */
    return num_bytes_read;
  }
  else
  {
    /* Clear function controller interrupt */
    HWIO_OUT(USB_CORE_INT_STATUS, HWIO_USB_CORE_INT_STATUS_FCINT_BMSK);
  }

  BOOTHW_KICK_WATCHDOG ();

  /* Process all function controller interrupts */
  sys_int_status = HWIO_IN(USB_F_SYSTEM_INT_STATUS);

  /*-------------------------------------------------------------------------
    Enumeration support
  -------------------------------------------------------------------------*/
  /* Check for cable pull (suspend) */
  if (sys_int_status & USBDC_F_INT_SUSPEND_M)
  {
    /* indicate to HW that we get suspend */
    HWIO_OUT(USB_FUNCTION_CMD_STATUS, 
             HWIO_USB_FUNCTION_CMD_STATUS_SUSPDET_BMSK);

    /* Clear the suspend and resume interrupt status */
    HWIO_OUT(USB_F_SYSTEM_INT_STATUS, 
             USBDC_F_INT_SUSPEND_M | USBDC_F_INT_RESET_M);

    sys_int_status = HWIO_IN(USB_F_SYSTEM_INT_STATUS);
  }

  /* Check for reset */
  if (sys_int_status & USBDC_F_INT_RESET_M)
  {
    /* Reset USB HW */
    dloadusb_init_hw();

    /* Reset address and flags */
    dloadusb_data.setup_data_stage_flag = FALSE;
    dloadusb_data.address = 0;

    /* Clear the reset interrupt status */
    HWIO_OUT(USB_F_SYSTEM_INT_STATUS, USBDC_F_INT_RESET_M);

    /* Get the status again */
    sys_int_status = HWIO_IN(USB_F_SYSTEM_INT_STATUS);
  }

  if (sys_int_status & USBDC_F_INT_DONE_M)
  {
    /* which EP is done */ 
    which_ep = HWIO_IN(USB_F_ENDPOINT_DONE_STATUS); 

    /* only serve and clear those be enabled */
    which_ep &= HWIO_IN( USB_ENDPOINT_DONE_ENA);

    /* clear out the EP bits that has been set */
    HWIO_OUT( USB_F_ENDPOINT_DONE_STATUS, which_ep );

    /*------------------------------------------------------------
      Clear interrupt after USB_F_ENDPOINT_DONE_STATUS is cleared
    -------------------------------------------------------------*/
    HWIO_OUT( USB_F_SYSTEM_INT_STATUS,  USBDC_F_INT_DONE_M);

    if ( which_ep & USBDC_CONTROL_IN_EP_M)
    { 
      /* EP0 IN done */
      dloadusb_process_tx_done(USBDC_CONTROL_IN_EP);

      /* Insure that the next transfer starts with data1 */
      dloadusb_ep_set_toggle_bit(USBDC_CONTROL_IN_EP, USBDC_EP_DIR_IN);

      /* Update address if necessary */
      if ( (HWIO_IN(USB_DEVICE_ADDRESS) == 0)  &&
            (dloadusb_data.address > 0) )
      {
        HWIO_OUT(USB_DEVICE_ADDRESS, dloadusb_data.address);
      }
    }

    if (which_ep & USBDC_CONTROL_OUT_EP_M)
    { 
      /* EP0 OUT done */
      
      /* Put the IN endpoint in a known state */ 
      dloadusb_ep_disable_done_int(USBDC_CONTROL_IN_EP,
                                   USBDC_EP_DIR_IN, 
                                   TRUE);
      /* Process setup message */
      dloadusb_do_handle_setup_msgs();
    }

    /* Check for completion of IN bulk transfer */
    ep_mask =  2 << (2*dloadusb_data.usb_in_endpoint);
    if (which_ep & ep_mask)
    { 
      dloadusb_process_tx_done(dloadusb_data.usb_in_endpoint);
    }

    /* Check for completion of OUT bulk transfer */
    ep_mask =  1 << (2*dloadusb_data.usb_out_endpoint);
    if (which_ep & ep_mask)
    { 
      /* Determine data received count */

      /* Get dword3 */
      ep_mem_ptr = dloadusb_ep_get_descriptor_addr(
           dloadusb_data.usb_out_endpoint,
           USBDC_EP_DIR_OUT);

      /* Calculate bytes received */
      num_bytes_read = sizeof(dloadusb_data.rx_buffer[0]) -
          ((word) (*(ep_mem_ptr +3) & USBDC_ENDPOINT_TTLBTECNT_M));

      /* if num_bytes_read is 0 - we received a ZLP - we need to
	     setup the DMA again */
      if ( num_bytes_read == 0 ) 
      {
	    /* Setup a new transfer */
	    dloadusb_setup_rx_DMA(
          dloadusb_data.usb_out_endpoint,
	      dloadusb_data.rx_buffer[dloadusb_data.rx_dma_index],
	      sizeof(dloadusb_data.rx_buffer[0]));
      }
    }
  }
  
  if (num_bytes_read > 0)
  {
    /* Insure that data is read from memory and not data cache */
    mmu_invalidate_data_cache_lines((uint32*) 
                  dloadusb_data.rx_buffer[dloadusb_data.rx_dma_index],
                  num_bytes_read);

    /* Reset loop count */
    loop_count = 0;
  }

  return num_bytes_read;
} /* end of dloadusb_do_usb_check() */

/*===========================================================================

FUNCTION dloadusb_receive_byte

DESCRIPTION
  This function returns one character at a time to the calling function
  if data is available in the applicable buffer.
  This approach is choosen to have a consistency between the UART and
  USB modules.

DEPENDENCIES
  The USB device must be enumerated.

RETURN VALUE
  Returns the next character from the receive buffer. If there is nothing
  in the receive buffer then it returns an error.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

int dloadusb_receive_byte
(
  void
)
{
  int n, ret_value;

  if (dloadusb_data.rx_index == dloadusb_data.rx_num_bytes_read)
  {
    /* The rx_index is same as rx_num_bytes_read. This means that all
       the bytes received from the out endpoint are returned to the
       downloader. */
    dloadusb_data.rx_index = 0;

    /* Time to perform a new read on the out endpoint. */
    dloadusb_data.rx_num_bytes_read = dloadusb_do_usb_check();

    if (dloadusb_data.rx_num_bytes_read == 0)
    {
      return USB_NO_CHAR;
    }

    /* Toggle buffer */
    if (dloadusb_data.rx_dma_index == 0)
    {
      dloadusb_data.rx_dma_index = 1;
    }
    else
    {
      dloadusb_data.rx_dma_index = 0;
    }
    
    /* Setup a new transfer */
    dloadusb_setup_rx_DMA(dloadusb_data.usb_out_endpoint,
                 dloadusb_data.rx_buffer[dloadusb_data.rx_dma_index],
                 sizeof(dloadusb_data.rx_buffer[0]));
    
    /* ok. we received some bytes from the host and the
       rx_num_bytes_read active buffer are updated */
  }

  /* The data is available in the rx_buffer. So, we return the next byte
     based on the rx_index. */
  if (dloadusb_data.rx_dma_index == 0)
  {
    n = 1;
  }
  else
  {
    n = 0;
  }
  ret_value = (int) (dloadusb_data.rx_buffer[n][dloadusb_data.rx_index++]);

  return ret_value;
} /* end of dloadusb_receive_byte() */

/*===========================================================================

FUNCTION dloadusb_transmit_byte

DESCRIPTION
  This function buffers a byte into USB tx buffer. If the end of message
  (0x7E) is received from the packet module, then this function will send
  the data.

DEPENDENCIES
  The USB device must be enumerated.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/
void dloadusb_transmit_byte
(
  byte chr
)
{
  /* Copy the byte to the trasmit buffer. */
  dloadusb_data.tx_buffer[dloadusb_data.tx_buf_index++] = chr;

  /* Check for terminating character */
  if ((chr == 0x7E) && (dloadusb_data.tx_buf_index != 1))
  {
    BOOTHW_KICK_WATCHDOG ();

    /* Send this response */
    dloadusb_setup_tx_DMA(dloadusb_data.usb_in_endpoint,
                          dloadusb_data.tx_buffer,
                          dloadusb_data.tx_buf_index);

    /* Determine if 0 lenght packet is necessary */
    if ((dloadusb_data.tx_buf_index & (USBDC_BULK_PKT_SIZE - 1)) == 0)
    {
      /* Set flag to send 0 length packet in dloadusb_process_tx_done */
      dloadusb_data.tx_sending_short_pkt = FALSE;
    }
    else
    {
      dloadusb_data.tx_sending_short_pkt = TRUE;
    }
    
    /* Reset Index */
    dloadusb_data.tx_buf_index = 0;
  }

} /* end of dloadusb_transmit_byte() */

/*===========================================================================

FUNCTION dloadusb_spoof_disconnect

DESCRIPTION
  Drives pins low to initiate disconnect from host.

DEPENDENCIES
  The USB device must be running.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dloadusb_spoof_disconnect
(
  void
)
{
  volatile int i;
  
  /* Spoof disconnect */
  if (dloadusb_data.xcvr_type == USBOTG_FIVE_WIRE_XCVR)
  {
  #ifdef HWIO_USB_PIN_SEL_OUT

    /* Enable write of 0 to the VPO, VMO, and OE USB pins */
   #ifdef HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK
    /* MSM7500 */
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK );
   #else
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK);
   #endif
    HWIO_USB_PIN_SEL_OUT(HWIO_USB_PIN_SEL_USB_REG_SEL_BMSK);
  #else
    /* Enable write of 0 to the VPO, VMO, and OE USB pins */
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_USB_REG_SEL_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK);
  #endif
  }
  else
  {
  #ifdef HWIO_USB_PIN_SEL_OUT

    /* Enable write of 0 to the VPO OE USB pins */
    /* Write 1 to the VMO pin */
    #ifdef HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK
    /* MSM7500 */
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OUT_BMSK);
    #else
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OUT_BMSK);
    #endif
    HWIO_USB_PIN_SEL_OUT(HWIO_USB_PIN_SEL_USB_REG_SEL_BMSK);
  #else
    /* Enable write of 0 to the VPO OE USB pins */
    /* Write 1 to the VMO pin */
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_USB_REG_SEL_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_DAT_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OE_BMSK |
                            HWIO_USB_PIN_CONFIG_REG_SE0_OUT_BMSK);
  #endif
  }

  /* Delay */
  for (i=0; i <100000; i++)
  {
  }

  /* Let the core now write to the pins */
  /* Enable write of 0 to the VPO, VMO, and OE USB pins */
#ifdef HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK
 /* MSM7500 */
  HWIO_USB_PIN_CONFIG_OUTM(HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK, 0);
#else
  HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK);
#endif
#ifdef HWIO_USB_PIN_SEL_OUT
  HWIO_USB_PIN_SEL_OUT(0);
#endif
}

/*===========================================================================

FUNCTION dloadusb_init_hw

DESCRIPTION
  Initializes the USB Device hardware.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Powers up the USB device controller hardware, and initializes the hardware.

===========================================================================*/
static void dloadusb_init_hw
(
  void
)
{
  int i,j;
  dword *ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  word buff_offset = (word) HWIO_ADDR(USB_DATA_MEMORY_BASE);
  dword ep_enable_bit = 1;
  dword ep_enable_value = 0;
  dword sys_int_status; 

  /* Reset the func controller and serial interface engine */
  HWIO_OUT( USB_RESET_CTL, 
            HWIO_USB_RESET_CTL_RSTFC_BMSK |
            HWIO_USB_RESET_CTL_RSTFSIE_BMSK);

  if (dloadusb_data.xcvr_type == USBOTG_FIVE_WIRE_XCVR)
  {
    /* force USB core work as device bit1:0 = 10b,
       and XCVR mode bit 7:6 = 0 */ 
    HWIO_OUT( USB_HARDWARE_MODE,
               ( 0<< HWIO_USB_HARDWARE_MODE_OTGXCVR_SHFT)  |
               ( 0<< HWIO_USB_HARDWARE_MODE_HOSTXCVR_SHFT)  |
               ( HWIO_USB_HARDWARE_MODE_CRECFG_BMSK & 2  ) );
  }
  else
  {
    /* force USB core work as device bit1:0 = 10b,
       and XCVR mode bit 7:6 = 11b */ 
    HWIO_OUT( USB_HARDWARE_MODE,
               ( 3<< HWIO_USB_HARDWARE_MODE_OTGXCVR_SHFT)  |
               ( 3<< HWIO_USB_HARDWARE_MODE_HOSTXCVR_SHFT)  |
               ( HWIO_USB_HARDWARE_MODE_CRECFG_BMSK & 2  )   );
  }

  /* Enable function controller clock, bit2 */
  HWIO_OUT(USB_CLOCK_CTL, HWIO_USB_CLOCK_CTL_FUNCCLK_BMSK);
  
  /* enable function controller interrupt (bit1) from CORE level  */
  HWIO_OUT( USB_CORE_INT_ENA, HWIO_USB_CORE_INT_ENA_FCINTEN_BMSK); 

  /* clear interrupts */
  sys_int_status = HWIO_IN( USB_F_SYSTEM_INT_STATUS);
  HWIO_OUT( USB_F_SYSTEM_INT_STATUS, sys_int_status);

  /* Disable EPs */
  HWIO_OUT(USB_ENDPOINT_ENA, 0);
  
  /* Setup EP descriptors */
  for (i=0; i < USBDC_NUM_LOGICAL_ENDPOINTS; i++)
  {
    /* Each logical endpoint has 2 physical endpoints */ 
    for (j=0; j < 2; j++)
    {
      /* An unused endpoint has a 0 max packet size */ 
      if (dloadusb_ep[i][j].max_pkt_sz > 0)
      {
        /* Setup dword0 */
        switch (dloadusb_ep[i][j].transfer_type)
        {
        case USBDC_CONTROL_TRANSFER:
          *ep_mem_ptr = 0;
          break;
        
        case USBDC_INTERRUPT_TRANSFER:
          *ep_mem_ptr = 0xC000;
          break;
        
        case USBDC_BULK_TRANSFER:
          *ep_mem_ptr = 0x8000;
          break;
        }
        *(ep_mem_ptr++) += (dloadusb_ep[i][j].max_pkt_sz << 16);

        /* Setup dword1 */
        *ep_mem_ptr = buff_offset;  /* XBUFSRTAD */
        buff_offset += dloadusb_ep[i][j].buffersize;
        *(ep_mem_ptr++) += (buff_offset << 16);  /* YBUFSRTAD */
        buff_offset += dloadusb_ep[i][j].buffersize;


        /* Setup dword2 */
        ep_mem_ptr++;  /* reserved */

        /* Setup dword3 */
        *(ep_mem_ptr++) = (( dloadusb_ep[i][j].buffersize-1) << 21);  
        
        /* Update EP enable value */
        ep_enable_value += ep_enable_bit;
      }
      else
      {
        /* Increment by 4 dwords */
        ep_mem_ptr += 4;
      }
      
      /* Update EP enable bit for next EP */
      ep_enable_bit = ep_enable_bit << 1;
    }
  }

  /* Insure that the next transfer starts with data1 */
  HWIO_OUT(USB_ENDPOINT_TOGGLE_BITS, 2);

  /* Enable EPs */
  HWIO_OUT(USB_ENDPOINT_ENA, ep_enable_value);

  /* Indicate EP 0 OUT is ready for a transfer */
  ep_mem_ptr = (dword *) HWIO_ADDR(USB_ENDPOINT_MEMORY_BASE);
  
  /* Increment by 3 dwords for EP 0 OUT dword3 */
  ep_mem_ptr += 3;
  
  /* Set the transfer count */
  *(ep_mem_ptr) += (dloadusb_ep[USBDC_CONTROL_OUT_EP][0].buffersize << 1); 
    
  /* clear Fuction interrupt status */
  HWIO_OUT( USB_F_SYSTEM_INT_STATUS, HWIO_USB_F_SYSTEM_INT_STATUS_RMSK);

  /* Enable the done interrupt status */
  dloadusb_ep_enable_done_int(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);
  
  /* Ready for transfer */
  dloadusb_ep_enable_ready(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);

  /* initialization of DMA */
  /* clear all EpDMA channels */
  HWIO_OUT( USB_EP_DMA_CHANNEL_CLEAR, HWIO_USB_EP_DMA_CHANNEL_CLEAR_RMSK);
  /* clear all EpDMA error status */
  HWIO_OUT( USB_EP_DMA_ERROR_STATUS,  HWIO_USB_EP_DMA_ERROR_STATUS_RMSK);
  /* clear EP DMA interrupt  */
  HWIO_OUT( USB_DMA_INT_STATUS, HWIO_USB_DMA_INT_STATUS_EPERR_BMSK);
  /* Enable EP DMA interrupt */
  HWIO_OUT( USB_DMA_INT_ENA, HWIO_USB_DMA_INT_ENA_EPERRINTEN_BMSK);

  /* enable interrupts from function controller only */
  HWIO_OUT ( USB_F_SYSTEM_INT_ENA,
             HWIO_USB_F_SYSTEM_INT_ENA_DONEREGIEN_BMSK |
             HWIO_USB_F_SYSTEM_INT_ENA_RESETIEN_BMSK |
             HWIO_USB_F_SYSTEM_INT_ENA_SUSPDETIEN_BMSK |
             HWIO_USB_F_SYSTEM_INT_ENA_RSMFINIEN_BMSK
           );

  dloadusb_data.configured = FALSE;
}

/*===========================================================================

FUNCTION dloadusb_drain

DESCRIPTION
  This function waits for the IN transfer to complete.
  This allows the caller to be sure that all
  characters are transmitted.

DEPENDENCIES
  The USB device must be enumerated.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog is reset.

===========================================================================*/

void dloadusb_drain
(
  void
)
{
  /* Wait for transfer to complete */
  dloadusb_wait_for_dma_done(dloadusb_data.usb_in_endpoint,
                             USBDC_EP_DIR_IN);

} /* end of dloadusb_drain() */

/*===========================================================================

FUNCTION dloadusb_init

DESCRIPTION
  This function initializes the MSM USB device controller with no
  interrupts enabled but both receiver and transmitter running.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dloadusb_init
(
  void
)
{
  dword *ep_mem_ptr;
  dword temp_dword;
  
  dloadusb_data.tx_buf_index = 0;
  dloadusb_data.rx_index = 0;
  dloadusb_data.rx_dma_index = 0;
  dloadusb_data.rx_num_bytes_read = 0;
  dloadusb_data.setup_data_stage_flag = FALSE;
  dloadusb_data.address = 0;

  /* Indentify endpoints */
#ifdef FEATURE_USB_DIAG
  dloadusb_data.usb_out_endpoint = USBDC_DIAG_STREAM_OUT_EP;
  dloadusb_data.usb_in_endpoint  = USBDC_DIAG_STREAM_IN_EP;
#else
  dloadusb_data.usb_out_endpoint = USBDC_DATA_STREAM_OUT_EP;
  dloadusb_data.usb_in_endpoint  = USBDC_DATA_STREAM_IN_EP;
#endif

#ifdef FEATURE_USB_FIVE_WIRE_XCVR
  dloadusb_data.xcvr_type = USBOTG_FIVE_WIRE_XCVR;
#endif
#ifdef FEATURE_USB_OTG_XCVR
  dloadusb_data.xcvr_type = USBOTG_OTG_XCVR;
#endif

  /* Init line coding data */
  memcpy(dloadusb_data.line_coding,
         (byte*) &(default_serial_info.data_rate),
         sizeof(dword));
  dloadusb_data.line_coding[4] = default_serial_info.stop_bits;
  dloadusb_data.line_coding[5] = default_serial_info.parity;
  dloadusb_data.line_coding[6] = default_serial_info.data_bits;

  /* Enable clock as it may not be enabled */
  /* Turn on the 48MHz clock. */
  clk_regime_usb_xtal_on();

  /* Enable function controller clock, bit2 */
  HWIO_OUT(USB_CLOCK_CTL, HWIO_USB_CLOCK_CTL_FUNCCLK_BMSK);
  
  /* Check if USB is configured */
  if (HWIO_IN(USB_DEVICE_ADDRESS) == 0)
  {
    /* Enable USB HW */
    dloadusb_init_hw();

    if (dloadusb_data.xcvr_type == USBOTG_FIVE_WIRE_XCVR)
    {
#ifndef T_FFA
      /* Enable transceiver, enable suspend GPIO and drive it low */
      BIO_TRISTATE(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_M);
      BIO_OUT(BIO_USB_SUSPEND_REG, BIO_USB_SUSPEND_M, BIO_USB_SUSPEND_OFF_V);
#endif
    }
    
    /* Enable OE */
#ifdef HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK
    /* MSM7500 */
    HWIO_USB_PIN_CONFIG_OUTM(HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK, 0);
#else
    HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK);
#endif
#ifdef HWIO_USB_PIN_SEL_OUT
    HWIO_USB_PIN_SEL_OUT(0);
#endif
  
    HWIO_OUT(USB_MISC_CTL, 0x6); /* round Robin Arbiter, skipOnRetry */

    /* Spoof disconnecting from host */
    dloadusb_spoof_disconnect();
  }
  else
  {
    /* Device is already enumerated */
    dloadusb_data.configured = TRUE;
    
    /* The usb driver is already initialized by the DMSS software and
     * is running. We are not sure whether the interrupts are disabled or
     * not. So, we disable them anyway. */
    HWIO_OUT(USB_CORE_INT_ENA, HWIO_USB_CORE_INT_ENA_FCINTEN_BMSK);
    HWIO_OUT(USB_ENDPOINT_DONE_ENA, 0);

    /* clear Function interrupt status */
    HWIO_OUT( USB_F_SYSTEM_INT_STATUS, HWIO_USB_F_SYSTEM_INT_STATUS_RMSK);

    /* Clear the other OUT bulk endpoint so armprg will know which
       set of endpoints to use */
#ifdef FEATURE_USB_DIAG
    dloadusb_ep_disable_done_int(USBDC_DATA_STREAM_OUT_EP,
                                 USBDC_EP_DIR_OUT,
                                 TRUE);
    /* set DWORD3 of EP for no transfer */
    ep_mem_ptr = dloadusb_ep_get_descriptor_addr(USBDC_DATA_STREAM_OUT_EP,
                                                 USBDC_EP_DIR_OUT);
    temp_dword = *(ep_mem_ptr +3 );
    temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
    *(ep_mem_ptr +3 ) = temp_dword;
#else
    dloadusb_ep_disable_done_int(USBDC_DIAG_STREAM_OUT_EP,
                                 USBDC_EP_DIR_OUT,
                                 TRUE);
    /* set DWORD3 of EP for no transfer */
    ep_mem_ptr = dloadusb_ep_get_descriptor_addr(USBDC_DIAG_STREAM_OUT_EP,
                                                 USBDC_EP_DIR_OUT);
    temp_dword = *(ep_mem_ptr +3 );
    temp_dword &= ~USBDC_ENDPOINT_TTLBTECNT_M;
    *(ep_mem_ptr +3 ) = temp_dword;
#endif

    /* Insure that the next IN control transfer starts with data1 */
    HWIO_OUT(USB_ENDPOINT_TOGGLE_BITS, 2);

    /* Enable the done interrupt status for control */
    dloadusb_ep_enable_done_int(USBDC_CONTROL_OUT_EP, USBDC_EP_DIR_OUT);

    /* enable interrupts from function controller only */
    HWIO_OUT ( USB_F_SYSTEM_INT_ENA,
               HWIO_USB_F_SYSTEM_INT_ENA_DONEREGIEN_BMSK |
               HWIO_USB_F_SYSTEM_INT_ENA_RESETIEN_BMSK |
               HWIO_USB_F_SYSTEM_INT_ENA_SUSPDETIEN_BMSK |
               HWIO_USB_F_SYSTEM_INT_ENA_RSMFINIEN_BMSK
            );

    /* Enable function controller interrupt (bit1) from CORE level  */
    HWIO_OUT( USB_CORE_INT_ENA, HWIO_USB_CORE_INT_ENA_FCINTEN_BMSK);

    /* Assume last response was a short packet */
    dloadusb_data.tx_sending_short_pkt = TRUE;

    /* Setup to receive data from the host */
    dloadusb_setup_rx_DMA(dloadusb_data.usb_out_endpoint,
                          dloadusb_data.rx_buffer[0],
                          sizeof(dloadusb_data.rx_buffer[0]));
  }

} /* end of dloadusb_init() */

/*===========================================================================

FUNCTION dloadusb_shutdown

DESCRIPTION
  This function will disable and de-configure all
  confgured endpoints.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dloadusb_shutdown
(
  void
)
{
  /* De-configure all configred endpoints */
  /* reset function controller */
  HWIO_OUT(USB_FUNCTION_CMD_STATUS,
           HWIO_USB_FUNCTION_CMD_STATUS_SOFTRESET_BMSK);

} /* end of dloadusb_shutdown() */

#endif
