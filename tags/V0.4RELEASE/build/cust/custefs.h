#ifndef CUSTEFS_H
#define CUSTEFS_H
/*===========================================================================

            " C u s t - E F S "   H E A D E R   F I L E

DESCRIPTION
  Configuration for EFS Feature.

  Copyright (c) 2001, 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custefs.h#8 $ $DateTime: 2008/09/25 16:39:50 $ $Author: c_surven $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/08    sv     Align FS_BUFFER_FREEMAP_SIZE size to EFS_PAGE_SIZE
08/07/08    rp     Further increase the FDI heap-size to make it boot for 2MB.
08/04/08    yg     Turn ON test flag to enable higher memory for FDI.
08/01/08    rp     Removed FEATURE_EFS_FDI_HEAP_SIZE as it is autocomputed now.
12/11/02    gr     Modified to work with non-EFS2 and EFS2 builds.
11/25/02    gr     Added FEATURE_EFS2 and FS_NAND_DMSS.
07/28/02   jct     Added FEATURE_BUILD_xxx to add build env. control over
                   inclusion of MMC/SFAT
04/30/02   dlb     Removed the AMD_SUSPEND_FETCH_BUG feature.
02/08/02   drh     Added FEATURE_EFS_AMD_DISABLE_FAST_WRITE and
                   FEATURE_EFS_AMD_SUSPEND_FETCH_BUG
11/12/01   drh     Added FEATURE_EFS_SHARP
04/25/01   bkm     Moved FEATURE_4MB_ROM to custsurf.h
04/22/01   bkm     Added FEATURE_4MB_ROM, but left undefined.
03/16/01   vlc     Enable FEATURE_EFS_SFAT, updated comments.
03/09/01   bkm     Added FEATURE_EFS_2ND_FLASH_ONLY, but left it undef'd. 
03/01/01   vlc     Created from custefs.h r1.0 of MSM5105 archive.
===========================================================================*/


/*---------------------------------------------------------------------------
                          MMC / SFAT
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
Removable media and Hotplug
---------------------------------------------------------------------------*/
#ifdef FEATURE_BUILD_MMC
#define FEATURE_MMC
#endif

#if defined FEATURE_MMC \
|| defined FEATURE_DRV_SDCC \
|| defined FEATURE_SDCC_EXTERNAL_DM
#define FEATURE_EFS_HOTPLUG_SD
#endif


/* If any removable media is enabled, we need Hotplug and SFAT as well */
#if defined FEATURE_EFS_HOTPLUG_USB_HOST_MS \
|| defined FEATURE_EFS_HOTPLUG_SD \
|| defined FEATURE_EFS_HOTPLUG_FTL

/* This enables the active (polling) hotplug task */
#define FS_HOTPLUG_DEFAULT_POLL_INTERVAL_MS   10000 /* 10 seconds */

#define FEATURE_EFS_HOTPLUG
#define FEATURE_EFS_EXTFS
#define FEATURE_EFS_EXTFS_HFAT
#endif

/*---------------------------------------------------------------------------
                          Embedded File System
---------------------------------------------------------------------------*/

#define FEATURE_EFS_NAND_FACTORY_START

/* Use NV Item Manager with Embedded file system.
*/
#define FEATURE_NV_ITEM_MGR

/* Define to enable the Embedded file system.
*/
#define FEATURE_EFS
/* Use NV Item Manager with Embedded file system.
*/
#define FEATURE_SURF_FLASH

/* Define to enable the Access Methods.
*/
#define FEATURE_EFS_ACCESS_METHODS

/* Define for AMD device support.
** Undefined for non-AMD.
*/
#define FEATURE_EFS_AMD

/* Define for Mitsubishi device support.
** Undefined for non-Mitsubishi.
*/
#undef FEATURE_EFS_MITSUBISHI

/* Define for Sharp device support
 * Undefined for non-Sharp
 */
#undef FEATURE_EFS_SHARP

/* Forces EFS to use the second flash, requires 2 flash parts on the SURF
** and updated load script.
*/
#ifdef T_QUASAR
#error code not present
#else
#undef FEATURE_EFS_2ND_FLASH_ONLY
#endif /* T_QUASAR */

/* Define this to disable fast writes operations.  Newer AMD devices do
 * not support fast writes during erase suspends.  With faster CPU's,
 * there isn't much of a speed change, but if only older devices are
 * being used, and the CPU is slow, it might be advantageous to turn
 * this off.  (The fast write works on the 29DL163). */
#define FEATURE_EFS_AMD_DISABLE_FAST_WRITE


/* ------------------------------------------------------------------------
 * Define EFS Access Validation
 *   Provides an interface for a client to register an accessor validator
 *   function.  When registered, DIAG will call the function for each of
 *   the file system operations given by fs_diag_op_type.  The callback
 *   returns a value indicating whether the operation can proceed.  If
 *   the call back indicates the operation is not ok, DIAG returns an
 *   ERR packet indicating this is a bad command - otherwise the operation
 *   proceeds as normal.
 * ------------------------------------------------------------------------*/
#define FEATURE_DIAG_FS_ACCESS_VALIDATION

/*---------------------------------------------------------------------------
                               EFS2
---------------------------------------------------------------------------*/
#ifdef FEATURE_EFS2

  #ifndef FEATURE_IG_EFS_EXT_SERVER
  /* For EFS1 compatibility                                           */
  #define FEATURE_EFS_COMPATIBILITY
  #endif

  /* For builds with NAND flash                                       */
  #define FS_NAND_DMSS
  /* Undef to indicate that EFS1 will not be used on the flash device */
  #undef FEATURE_EFS_FLASH
#else
  /* Indicate that EFS1 will be used on the flash device              */
  #define FEATURE_EFS_FLASH
  /* Enable probing Flash devices according to a device table list.
  */
  #define FEATURE_EFS_TABLE_PROBE
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_2K_NAND
   #define EFS_PAGE_SIZE            2048
#else
   #define EFS_PAGE_SIZE            512
#endif

/* Limit max NOR pages to the amount just needed */
#define FS_MAX_NOR_PAGES            (((FLASH_NOR_EFS2_END_BYTE_OFFSET -   \
                                       FLASH_NOR_EFS2_START_BYTE_OFFSET) + 1) \
                                       / EFS_PAGE_SIZE)

/**
 * Free bitmap size configuration.  This is the amount of buffer space we
 * set aside to hold the freemap.  The value is a size in bytes for the
 * cache.  It must be large enough to hold the number of clusters needed to
 * hold the freemap (1 bit per data cluster).  It needs to be at least
 *   ceil (total_clusters / 8)
 * rounded up to the nearest EFS_PAGE_SIZE.
 */
#define FS_BUFFER_FREEMAP_SIZE  \
          ((((FS_MAX_NOR_PAGES)/8) & (EFS_PAGE_SIZE - 1)) ? \
           ((((FS_MAX_NOR_PAGES)/8) | (EFS_PAGE_SIZE - 1)) + 1) : \
           ((FS_MAX_NOR_PAGES)/8))


#ifdef FEATURE_FS_LOW_MEMORY_USAGE

  #if !defined(FEATURE_HFAT) && !defined(FEATURE_EFS_FDI)
    /**
     * Maximum length of a full pathname, not including a trailing '\\0'
     * character.  The filesystem only has a few buffers of this length, so it
     * doesn't increase EFS2's RAM usage much, however, other code (such as
     * fs_compat, and fs_am) us this value to allocate a large number of
     * buffers. */
    #define FS_PATH_MAX 260

    /**
     * Maximum length of the 'name' component of a file.  The name is the
     * longest string of characters delimited by beginning of string, the '/'
     * character or the end of the string.  This is the maximum length for a
     * filename or directory name.
     */
    #define FS_NAME_MAX 128
  #endif

  /**
   * Maximum number of files that can be kept opened simultaneously.
   */
  #define EFS_MAX_FILE_DESCRIPTORS      64
#endif /* FEATURE_FS_LOW_MEM_USAGE */

/* Using lower memory with 4MB config is causing boot failure since, FDI
 * ran out of RAM. So turn ON Test config that uses more memory. */
#define FFS_QUALCOMM_TEST_BUILD

/* Even with the big-heap, 2MB EFS partitions did not boot as FDI ran out
 * of RAM again, so further increasing the heap-size as per their new
 * computations. */
#define FFS_FDI_MAX_RAM_USAGE_PER_1K      (133)

/*
 * Maximum number of volumes.
 */
#define FS_HFAT_MAXVOLUME_COUNT 2

#endif /* CUSTEFS_H */

