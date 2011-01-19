/***********************************************************************
 * fs_blockdev_usb.h
 *
 * Blockdev USB driver header.
 * Copyright (C) 2006, 2009 QUALCOMM, Inc.
 *
 * Contains USB device specfic data and a handle to the USB device driver.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_blockdev_usb.h#1 $ $DateTime: 2009/05/18 17:19:57 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-08   ebb   Moved from hotplug to blockdev.
2006-04-22   s h   Simplified fs_hotplug_xxx.h header files
2006-03-31   s h   Made USB-specific functions local
2006-03-15   sch   Create

===========================================================================*/

#ifndef __FS_BLOCKDEV_USB_H__
#define __FS_BLOCKDEV_USB_H__

#include "fs_blockdev_i.h"

#ifdef FS_BLOCKDEV_USB
#include "usbhost_api.h"

/* These are the USB Instance and Handle default assignments.  They
   are magic values so that we can spot cases where calls are made with
   an uninitialized handle.   The specific value is unimportant.  */
#define USB_NO_INSTANCE 0xDEAD
#define USB_NO_HANDLE   0xDEAF

typedef enum
{
  USBSTATUS_CONNECTED,          /* New device connected */
  USBSTATUS_OPEN_REQUESTED,     /* We are trying to open it */
  USBSTATUS_OPEN,               /* The open was approved! */
  USBSTATUS_CLOSED,             /* Disconnect notify */
  USBSTATUS_UNCONNECTED         /* Nothing connected */
} blockdev_usbhost_state;

/* USB specific device data */
typedef struct
{
  usbhost_dev_instance_type  usb_instance;
  usbhost_dev_handle_type    usb_handle;
  blockdev_usbhost_state     usb_state;
} usb_device_data;

/* Export handle to usb device driver */
extern blockdev_driver blockdev_usb_driver;

/* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
extern usbhost_client_id_type client_id;

#else
extern int __dont_complain_about_empty_file;
#endif /* FS_BLOCKDEV_USB */
#endif /* not __FS_BLOCKDEV_USB_H__ */
