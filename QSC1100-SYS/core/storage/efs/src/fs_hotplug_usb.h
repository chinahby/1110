/***********************************************************************
 * fs_hotplug_usb.h
 *
 * Copyright (C) 2006 QUALCOMM, Inc.
 *
 * This is not a stand-alone header file.
 * It is used to communicate the exported device functions to hotplug.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_usb.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-04-22   s h   Simplified fs_hotplug_xxx.h header files
2006-03-31   s h   Made USB-specific functions local
2006-03-15   sch   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_USB_H__
#define __FS_HOTPLUG_USB_H__

/* Initialize USB connectivity */
void hotplug_usbhost_register (void);

/* Handle an incoming Open request */
void hotplug_usbhost_open (void);

/* Handle an incoming Close request */
void hotplug_usbhost_close (void);

extern struct hotplug_dev_funcs hotplug_usb_dev;

#endif
