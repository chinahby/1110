/***********************************************************************
 * fs_hotplug_usbhost.h
 *
 * Header file for the glue logic between hotplug and the host usb
 * stack.
 *
 * Copyright (C) 2009 QUALCOMM, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_usbhost.h#2 $ $DateTime: 2009/05/20 16:03:23 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-18   rp    Fix compiler warning for 8200 which has gcc with more -Ws
2009-05-08   ebb   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_USBHOST_H__
#define __FS_HOTPLUG_USBHOST_H__

#include "customer.h"
#include "fs_hotplug_i.h"

#ifdef FS_HOTPLUG_USB
  #include "usbhost_api.h"

  void hotplug_usbhost_register (void);
  void hotplug_usbhost_open (void);
  void hotplug_usbhost_close (void);

  /* Unique ID that USBHOST uses to recognize clients (like Hotplug) */
  extern usbhost_client_id_type client_id;

#else /* FS_HOTPLUG_USB */

  static inline void hotplug_usbhost_register (void) { }
  static inline void hotplug_usbhost_open (void) { }
  static inline void hotplug_usbhost_close (void) { }

#endif /* FS_HOTPLUG_USB */

#endif /* not __FS_HOTPLUG_USBHOST_H__ */
