/***********************************************************************
 * fs_hotplug_i.h
 *
 * Hotplug definitions Internal to the Hotplug components themselves.
 *
 * Copyright (C) 2006 - 2008 QUALCOMM, Inc.
 *
 * This is the PRIVATE header file for Hotplug.
 * Any code outside EFS should include only "fs_hotplug.h" instead.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_i.h#4 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
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

#ifdef FEATURE_EFS_HOTPLUG
#include "rex.h"                /* Task TCB not needed for COLDPLUG */
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
#endif

/* SD/MMC/MicroSD/MiniSD/T-Flash/Transflash removable media */
#ifdef FEATURE_EFS_HOTPLUG_SD
#  define FS_HOTPLUG_SD
#endif

/* The CPRM Protected area of SD cards */
#ifdef FEATURE_EFS_HOTPLUG_SD_CPRM
#  define FS_HOTPLUG_SD_CPRM
#endif

/* USB Host Mass Storage: Thumbdrive, hard drive, card reader */
#ifdef FEATURE_EFS_HOTPLUG_USB_HOST_MS
#  define FS_HOTPLUG_USB
#  include "usbhost_api.h"
#else
  typedef int usbhost_dev_instance_type; /* placeholder types */
  typedef int usbhost_dev_handle_type;
#endif

/* Flash Translation Layer (FAT-on-NAND */
#ifdef FEATURE_EFS_FTL
#  define FS_HOTPLUG_FTL
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
extern rex_tcb_type fs_hotplug_tcb; /* Task TCB for Hotplug */
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

typedef enum
{
  USBSTATUS_CONNECTED,          /* New device connected */
  USBSTATUS_OPEN_REQUESTED,     /* We are trying to open it */
  USBSTATUS_OPEN,               /* The open was approved! */
  USBSTATUS_CLOSED,             /* Disconnect notify */
  USBSTATUS_UNCONNECTED         /* Nothing connected */
} hotplug_usbhost_state;

/* These are the USB Instance and Handle default assignments.  They
   are magic values so that we can spot cases where calls are made with
   an uninitialized handle.   The specific value is unimportant.  */
#define HOTPLUG_NO_INSTANCE 0xDEAD
#define HOTPLUG_NO_HANDLE   0xDEAF

/*
 * This structure defines all functions related to a particular device
 * "slot" in the Hotplug table.  Each slot describes one potential
 * mountpoint.
 */
struct hotplug_device
{
  /* --- These fields are configured for each device --- */
  const char *name; /* This is mount point for the dev (eg. "mmc1") */
  int16 driveno;                /* SD/MMC "driveno" ("INT16") */
  hotplug_dev_type device_type; /* USB vs MMC/SD */
  struct hotplug_dev_funcs *dev;
  const char *remote_name;      /* Pathname on the server */

  /* --- The following fields are set from common default values --- */
  int                 was_present; /* was device present in previous poll? */
  hotplug_dev_state   dev_state;

  /* Fields specifically of interest to USB devices.
   * Note that hotplug task must not modify these.. they're "locked"
   * by the USB task and callbacks.  */
  usbhost_dev_instance_type usb_instance;
  usbhost_dev_handle_type   usb_handle;
  hotplug_usbhost_state     usb_state;

  /* Fields of interest to SD */
  unsigned char             sd_is_open;
  int                       sd_was_present;

 /* FTL specific code */
  void   *fdev;               /* ftl device needs a device handle pointer.*/

  unsigned int index;          /* index in the table */
  boolean  device_idle_state;  /* device_idle_state flag. */
};

struct hotplug_dev_funcs
{
  int (*mount) (struct hotplug_device *);
  int (*open) (struct hotplug_device *);
  int (*close) (struct hotplug_device *);
  int (*read)  (struct hotplug_device *,
                uint32, unsigned char *, uint16);
  int (*write) (struct hotplug_device *,
                uint32, unsigned char *, uint16);
  int (*user_write) (struct hotplug_device *,
                     uint32, unsigned char *, uint16);
  int (*erase) (struct hotplug_device *, uint32, uint16);
  int (*get_size) (struct hotplug_device *,
                   uint32 *, uint16 *);
  int (*is_present) (struct hotplug_device *);
  int (*format_prep) (struct hotplug_device *);
  int (*reset) (struct hotplug_device *);
  int (*idle_dispatch) (struct hotplug_device *);
  int (*read_iovec) (struct hotplug_device *, fs_iovec_t, uint32);
  int (*write_iovec) (struct hotplug_device *, fs_iovec_t, uint32, int);
  int (*reliable_write) (struct hotplug_device *,
                         uint32, uint32 *, uint32);
  int (*get_device_info) (struct hotplug_device *,
                          struct hotplug_device_info *);
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

/* These stub functions are exported for USB/SD to use in device tables */
int hotplug_success_stub (struct hotplug_device *hdev);
int hotplug_fail_stub (struct hotplug_device *hdev);
int hotplug_no_erase (struct hotplug_device *hdev,
                      uint32 lba, uint16 n_to_erase);
int hotplug_fat_mount (struct hotplug_device *hdev);

/* Return TRUE if the pathname is currently mounted.
 *
 * This uses a string pathname (due to RPC constraints), so it private
 * and not suited for application use on the local processor.
 *
 * This is for use only by the Remote API on dual-processors.
 */
int hotplug_is_mounted (const char *name);

struct hotplug_device* hotplug_disable_dev_by_path (const char *path);
int hotplug_dev_reset (struct hotplug_device *hdev);
int hotplug_dev_read_iovec (struct hotplug_device *hdev, fs_iovec_t iovec,
                            uint32 iovec_cnt);
int hotplug_dev_write_iovec (struct hotplug_device *hdev, fs_iovec_t iovec,
                             uint32 iovec_cnt, int reliable_write);
int hotplug_dev_reliable_write (struct hotplug_device *hdev, uint32 lba,
                                uint32 *buf, uint32 num_sectors);

int hotplug_no_dev_read_iovec (struct hotplug_device *hdev, fs_iovec_t iovec,
                               uint32 iovec_cnt);
int hotplug_no_dev_write_iovec (struct hotplug_device *hdev, fs_iovec_t iovec,
                                uint32 iovec_cnt, int reliable_write);
int hotplug_no_dev_reliable_write (struct hotplug_device *hdev, uint32 lba,
                                   uint32 *buf, uint32 num_sectors);
int hotplug_no_dev_get_device_info (struct hotplug_device *hdev,
                                 struct hotplug_device_info *dev_info);

#ifdef FS_UNIT_TEST
#error code not present
#endif



#endif /* not __FS_HOTPLUG_I_H__ */
