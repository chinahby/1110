/***********************************************************************
 * fs_hotplug_i.h
 *
 * Hotplug definitions Internal to the Hotplug components themselves.
 *
 * Copyright (C) 2006 - 2009 QUALCOMM, Inc.
 *
 * This is the PRIVATE header file for Hotplug.
 * Any code outside EFS should include only "fs_hotplug.h" instead.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_i.h#6 $ $DateTime: 2009/07/06 18:30:05 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-08   ebb   Separate hotplug from block device driver.
2008-12-12   rp    Added write_iovec, reliable write functions to hdev.
2008-11-24   yog   Added idle_dispatch func pointer in hotplug_dev_funcs ops.
2008-09-16   yog   Remove trailing commas from the enum declaration.
2008-04-28    np   Added support for FTL interface change.
2008-03-07    yg   Removed FS_HOTPLUG_SD_LOCK macro.
2007-04-02    rp   Replaced DEV_SKIP with DEV_UNUSABLE and added DEV_LOCKED.
2006-11-09   sch   Added CPRM specific definitions
2006-11-09    sh   Removed remote hotplug
2006-10-10    sh   Export hotplug_find_dev_by_type() as public.
2006-09-20    sh   Export hotplug_find_dev_by_path() as public.
2006-09-12    sh   Move SFAT/HFAT feature checks into fs_extfs.h
2006-08-27    sh   Add SFAT vs HFAT exclusion protection.
2006-06-25    sh   Replaced sd-specific "SD_STATE" with was_present
2006-06-06    sh   Moved much of the safeguards from hotplug.c into here
2006-06-02    sh   Added device reset function
2006-05-25    sh   Simplified device selection, pushed upward to target
2006-05-16    sh   Added hotplug_dev_is_mounted()
2006-05-11   s h   Renamed efs_hotplug_... to hotplug_...
2006-05-10   s h   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-08   s h   Removed umount from device-specific calls.
2006-05-05   s h   Moved hotplug_hdev() into external header
2006-05-04    sh   Protect against redundant open/close
2006-05-03    sh   Added FTL
2006-04-22   s h   Create local FS_HOTPLUG_XX flags to use internally
2006-04-22   s h   Create

===========================================================================*/

#ifndef __FS_HOTPLUG_I_H__
#define __FS_HOTPLUG_I_H__

#include "comdef.h"
#include "customer.h"

/* Pick up the public definitions as well */
#include "fs_hotplug.h"
#include "fs_iovec.h"
#include "fs_blockdev.h"

#ifdef FEATURE_EFS_HOTPLUG
#include "fs_osal.h"                /* Task TCB not needed for COLDPLUG */
#endif

/*
 * So, which devices do we support?
 *
 * These selections come from "custefs.h" or any target-defined area
 * to choose which devices are managed by hotplug.
 *
 * We map these external flags onto our own internal short names here.
 */

/* Define the gratuitous /dev/null device for all targets with Hotplug. */
#if defined FEATURE_EFS_HOTPLUG || defined FEATURE_EFS_COLDPLUG
#  define FEATURE_EFS_HOTPLUG_DEVNULL
#endif

/* The null device accepts all requests (for benchmarking) */
#ifdef FEATURE_EFS_HOTPLUG_DEVNULL
#  define FS_HOTPLUG_DEVNULL
#  define FS_BLOCKDEV_DEVNULL
#endif

/* SD/MMC/MicroSD/MiniSD/T-Flash/Transflash removable media */
#ifdef FEATURE_EFS_HOTPLUG_SD
#  define FS_HOTPLUG_SD
#  define FS_BLOCKDEV_SD
#endif

/* The CPRM Protected area of SD cards */
#ifdef FEATURE_EFS_HOTPLUG_SD_CPRM
#  define FS_HOTPLUG_SD_CPRM
#  define FS_BLOCKDEV_SD_CPRM
#endif

/* USB Host Mass Storage: Thumbdrive, hard drive, card reader */
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#  define FS_HOTPLUG_USB
#  define FS_BLOCKDEV_USB
#  include "usbhost_api.h"
#else
  typedef int usbhost_dev_instance_type; /* placeholder types */
  typedef int usbhost_dev_handle_type;
#endif

/* Flash Translation Layer (FAT-on-NAND */
#ifdef FEATURE_EFS_FTL
#  define FS_HOTPLUG_FTL
#  define FS_BLOCKDEV_FTL
#endif

/*************************************************************
 * FEATURE INTERLOCKS
 *
 * Some combinations are disallowed or required.
 * Try to be helpful here.
 *************************************************************/

/* The old flag FEATURE_EFS_SFAT would enable an SFAT task and EFS1
   Access Methods.  That doesn't work with this SFAT release, so make
   sure nobody does it accidentally. */
#ifdef FEATURE_EFS_SFAT
#error "FEATURE_EFS_SFAT is no longer used.  See FEATURE_EFS_EXTFS & _HOTPLUG."
#endif

/* If Hotplug is NOT enabled, make sure no dependent features are requested */
#if !defined FEATURE_EFS_HOTPLUG  && !defined FEATURE_EFS_COLDPLUG
#  if defined FEATURE_EFS_EXTFS_SFAT || defined FEATURE_EFS_EXTFS_HFAT
#    error "One of FEATURE_EFS_HOTPLUG or _COLDPLUG is required for FAT"
#  endif
#  if defined FS_HOTPLUG_SD     || \
      defined FS_HOTPLUG_USB    || \
      defined FS_HOTPLUG_FTL
#    error "Hotplug devices enabled without FEATURE_EFS_HOTPLUG/_COLDPLUG?"
#  endif
#endif

/* Only one of HOTPLUG or COLDPLUG is allowed */
#if defined FEATURE_EFS_HOTPLUG && defined FEATURE_EFS_COLDPLUG
#  error "FEATURE_EFS_HOTPLUG and FEATURE_EFS_COLDPLUG are mutually exclusive"
#endif

/* USB can not be used with Coldplug.  Hotplug only */
#if !defined FEATURE_EFS_HOTPLUG && defined FS_HOTPLUG_USB
#  error "USB requires a hotplug task (FEATURE_EFS_HOTPLUG)"
#endif

/*************************************************************
 * INTERNAL (PRIVATE) INTERFACE
 *
 * These definitions are for use only within Hotplug itself
 *************************************************************/

#ifdef FEATURE_EFS_HOTPLUG
extern fs_os_tcb_type fs_hotplug_tcb; /* Task TCB for Hotplug */
#endif

#define HOTPLUG_POLL_SIG                0x0001 /* Timer signal. */
#define HOTPLUG_STATE_SIG               0x0002 /* Polling state change. */
#define HOTPLUG_USBHOST_OPEN_SIG        0x0004
#define HOTPLUG_USBHOST_CLOSE_SIG       0X0008
#define HOTPLUG_USBHOST_SEM_WAIT_SIG    0x0010
#define HOTPLUG_IDLE_DEVICE_SIG         0X0020 /* Timeout event signal. */

typedef enum
{
  DEV_UNUSABLE,      /* Unknown device, hotplug never polled this device */
  DEV_MOUNTED,       /* Currently mounted */
  DEV_UNMOUNTED,     /* Not mounted (idle) */
  DEV_FORMATTING,    /* Being formatted */
  DEV_LOCKED         /* Currently locked */
} hotplug_dev_state;


/*
 * This structure defines all functions related to a particular device
 * "slot" in the Hotplug table.  Each slot describes one potential
 * mountpoint.
 */
struct hotplug_device
{
  int                   was_present; /* was device present in previous poll? */
  hotplug_dev_state     dev_state;   /* state of the device: (un)mounted,etc */
  unsigned int          index;       /* index of the device in table */
  struct block_device   *bdev;       /* underlying block device */
};


/* Perform a single hotplug poll.  (Useful during test)
 * Remember, this is a private function.  Keep away. */
void hotplug_poll (void);

/* Perform a single hotplug idle device event generation.
 * Remember this is private function and on target is invoked only
 * when signal HOTPLUG_IDLE_DEVICE_SIG is received. (Useful during Test)*/
#ifdef FS_UNIT_TEST
#error code not present
#endif

#endif /* not __FS_HOTPLUG_I_H__ */
