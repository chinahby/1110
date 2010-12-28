#ifndef FS_PARMS
#define FS_PARMS

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   E M B E D D E D   F I L E   S Y S T E M


GENERAL DESCRIPTION
  Embedded File System parameters file.  This module contains client
  specified global parameters.

Copyright (c) 1999,2000 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/fs_parms.h_v   1.10   26 Nov 2002 11:31:28   pingguan  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/inc/fs_parms.h#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-22-05   sh      Reduced FS_FILENAME_MAX_LENGTH_P to 255 for 'byte' limits.
08-02-05   sch     Increased FS_FILENAME_MAX_LENGTH_P to 260
04-01-05   ck      Removed featurizations around FS_FILENAME_MAX_LENGTH_P.
03-23-05   ck      Included the feature FEATURE_UI_CORE_REMOVED where
                   FS_FILENAME_MAX_LENGTH_P is defined to 80.
03-04-05   pc      Increased FS_MAX_NUM_FILES_NVIM_P from 30 to 45
                   Increased FS_MAX_OPEN_FILES_BASE_P from 6 to 20
06-17-03   pg      Added new parameter FS_MAX_FILE_SPACE_LIMIT_P.
                   It is used in defining FS_MAX_FBL_NODES_P when
                   FS_FILE_SPACE_LIMIT_P == 0.
03-20-03   pg      Merged changes from MSM6100 archive.
02-14-03   pg      Increased FS_MAX_REMOTE_FILES_P from 10 to 20.
11-26-02   ny      Changed (for MPEG4) FS_FILE_SPACE_LIMIT_P to 1Meg when
                   FEATURE_PHONE_VR is not defined.
10-29-02   pg      Increased FS_FILENAME_MAX_LENGTH_P for all BREW release.
10-01-02   ny      Changed (for MPEG4) FS_FILE_SPACE_LIMIT_P to 1Meg
08-07-02   pg      Added support for MPEG4 in FS_FILE_SPACE_LIMIT_P under
                   FEATURE_MP4_DECODER.
03-12-02   pg      Increased FS_MAX_NUM_FILES_NVIM_P to 30 to cover some
                   extra NVM files.
02-21-02   pg      Increased FS_MAX_NUM_FILES_NVIM_P to 25 to cover some
                   extra NVM files.
12-05-01   dlb     Fixed unit test support.
11-12-01   drh     Added enable FS_DEV_ENABLE_SHARP
07-19-01   pg      Undef FS_DEV_ENABLE_INTEL until more demand of fixing
                   Intel probe.  For now, Intel part is not supported by
                   default.
04-23-01   pg      Made the constant definitions more meaningful.
                   Changed FEATURE_AUDIO_FORMAT to FEATURE_MULTIMEDIA.
11-09-00   pg      Initial version for common archive.
                   Add support for AMD on SURF.
09-27-00   pg      Added definitions for CMX build with VR.
08-22-00   dlb     Fixed some support for unit testing.
07-25-00   pg      Took out some command for testing purpose.
07-24-00   pg      Added support for directly accessing the file system
                   with FS_DIR_INIT_DIRECT_ACCESS turned on.
07-20-00   dlb     Support Intel and Mitsubishi dynamically.
06-29-00   dlb     Add support for Mitsubishi on SURF.
05-23-00   pg      Added definitions for CMX build.
02-23-00   rp      Modified FS_PHYSICAL_BLKSIZE_P from 1024 to 256 bytes
                   on the FFA for NV Item Manager and VR support; also
                   adjusted FS_MAX_FBL_NODES_P, etc. for this block size.
01-20-00   rp      Adjusted FS_MAX_NUM_FILES_P values for an additonal 10
                   files (voice memos) under FEATURE_PHONE_VR.
12-08-99   rp      Changed use of T_SURF to FEATURE_SURF_FLASH so FFA
                   can use the appropriate Flash devices.
11-08-99   rp      Added section for the use of the NV Item Manager in
                   combination with VR.
                   Adjusted FS_MAX_NUM_FILES_P values to simply add 10
                   files to the required user file count to account for six
                   system files and some padding.
09-28-99   rp      Removed undef of FEATURE_FS_RAM_DRIVER.
                   Updated FS_MAX_NUM_FILES_P to 100 for NV Item Manager
                   due to an unusual high number of transaction log files.
09-27-99   rw      Added FEATURE_FS_RAM_DRIVER and associated parameters when
                   using the new driver for RAM-based devices.
08-13-99   rp      Featurized for NV Item Manager and VR support.
                   using the new driver for RAM-based devices.
08-13-99   rp      Featurized for NV Item Manager and VR support.
07-20-99   rp      Increased FS_MAX_NUM_FILES_P from 75 to 85 for VR.
05/27/99   sa      Changed FS_FILE_SPACE_LIMIT_P and FS_MAX_FBL_NODES_P
05/27/99   sa      Disabled FEATURE_FS_BACKGROUND_ERASE
05/27/99   sa      Changed number of simultaneously open files from 4 to 6.
05-24-99   sa      Changed parameters to reduce RAM usage.
05-21-99   ls      Configured for SURF.
05-21-99   ls      Configured for Digby.
05-20-99   ls      Added space limit parameter.
04-30-99   ls      Re-enable background erase.
04-29-99   ls      Temporarily turn off background erase.
04-27-99   ls      Added space available offset parameter.
04-25-99   ls      Tuned garbage collection parameters for maximum activity.
04-23-99   ls      Added second garbage collection threshold parameter.
04-22-99   ls      Removed enclosing ifdef.
04-21-99   ls      Version 2.2 release.
04-03-99   ls      Updated parameter values.
04-01-99   dlb     Changed flash type to be automatic.
03-04-99   ls      Initial version.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This file contains user-configurable system parameters.  Great caution
is advised before changing any parameters.

===========================================================================*/


/* Selection of flash components must be based on addressing issues.
   At this time, we support two platforms:
   SURF, a separate intel flash at 0x200000.
   5GP+ (Dibgy), RWW either AMD or MITS at 0x000000 and up.

   We also support SURF using Mitsutishi parts.  This selection is made
   with the FEATURE_EFS_MITSUBISHI feature.

   We also support SURF using AMD parts.  This selection is made with
   the FEATURE_EFS_AMD feature.

   We also support SURF using Sharp parts.  This selection is made with
   the FEATURE_EFS_SHARP feature.

   The selection of devices must be carefully chosen because the
   probes of some devices will render the code space unreadable. */
#ifdef FEATURE_SURF_FLASH
  #define FS_DIR_INIT_DIRECT_ACCESS
  #ifdef FEATURE_EFS_MITSUBISHI
    #define FS_DEV_ENABLE_MITS
  #endif
  #ifdef FEATURE_EFS_AMD
    #define FS_DEV_ENABLE_AMD
  #endif
  #ifdef FEATURE_EFS_SHARP
    #define FS_DEV_ENABLE_SHARP
  #endif
  #undef FS_DEV_ENABLE_INTEL
#else /* not FEATURE_SURF_FLASH */
  #define FS_DIR_INIT_DIRECT_ACCESS
  #define FS_DEV_ENABLE_AMD
  #define FS_DEV_ENABLE_MITS
  #define FS_DEV_ENABLE_SHARP
#endif

/* Undefine direct access for unit test. */
#ifdef FS_DEV_UNIT_TEST
#error code not present
#endif /* FS_DEV_UNIT_TEST */

/* Allow commands which reset the flash device (i.e., erases everything) */
#define FEATURE_FS_ALLOW_MASTER_RESET

/* Device erase suspend/resume setting */
#ifndef T_MSM6100
#error code not present
#else
  #undef FEATURE_FS_BACKGROUND_ERASE
#endif

/* Directory separator character */
#define  FS_DIRECTORY_CHAR_P        ('/')

/* Maximum file name length (cannot exceed block size) 
 * Also limited to 255 by the use of 'byte' for lengths. */
#ifndef FS_FILENAME_MAX_LENGTH_P
#define FS_FILENAME_MAX_LENGTH_P 255 
#endif

/* Threshold at which garbage collection nominally begins on a sector */
#ifdef FEATURE_PHONE_VR
  #define FS_GC_INIT_THRESHOLD_P    20
#else
  #define FS_GC_INIT_THRESHOLD_P    50
#endif

/* Threshold at which garbage collection runs aggressively (this parameter */
/* specifies the desired minimum number of clean blocks to be maintained   */
/* at all times, if possible)                                              */
#ifdef FEATURE_FS_RAM_DRIVER
  #define FS_GC_CLEAN_RESERVE_P     1
#elif defined(FEATURE_PHONE_VR)
  #define FS_GC_CLEAN_RESERVE_P     32
#else
  #define FS_GC_CLEAN_RESERVE_P     128
#endif

/* The number of blocks to subtract from the "space_available" function so */
/* the reported ceiling is a little less than the actual ceiling           */
#ifdef FEATURE_FS_RAM_DRIVER
  #define FS_SPACE_REPORT_OFFSET_P  1
#else
  #define FS_SPACE_REPORT_OFFSET_P  32
#endif

/* Maximum number of NVIM files in the system */
/* Maximum number of simultaneously "open" NVIM files in the system */
#ifdef FEATURE_NV_ITEM_MGR
  #define FS_MAX_NUM_FILES_NVIM_P   45
  #define FS_MAX_OPEN_FILES_NVIM_P  2*FS_MAX_NUM_FILES_NVIM_P
#else
  #define FS_MAX_NUM_FILES_NVIM_P   0
  #define FS_MAX_OPEN_FILES_NVIM_P  0
#endif

/* Maximum number of VR files in the system */
/* Maximum number of simultaneously "open" VR files in the system */
#ifdef FEATURE_PHONE_VR
  #define FS_MAX_NUM_FILES_VR_BASE_P    10
  #define FS_MAX_NUM_FILES_VR_TAGS_P    60
  #define FS_MAX_NUM_FILES_VR_MEMOS_P   30

  #define FS_MAX_NUM_FILES_VR_P \
          (FS_MAX_NUM_FILES_VR_BASE_P   + \
           FS_MAX_NUM_FILES_VR_TAGS_P   + \
           FS_MAX_NUM_FILES_VR_MEMOS_P)

  #define FS_MAX_OPEN_FILES_VR_P    6
#else
  #define FS_MAX_NUM_FILES_VR_P     0
  #define FS_MAX_OPEN_FILES_VR_P    0
#endif

/* Maximum number of MULTIMEDIA files in the system */
/* Maximum number of simultaneously "open" MULTIMEDIA files in the system */
#ifdef FEATURE_MULTIMEDIA
  #define FS_MAX_NUM_FILES_MM_P     60
  #define FS_MAX_OPEN_FILES_MM_P    6
#else
  #define FS_MAX_NUM_FILES_MM_P     0
  #define FS_MAX_OPEN_FILES_MM_P    0
#endif

#ifdef FS_DEV_UNIT_TEST
#error code not present
#else
  #define FS_MAX_NUM_FILES_UTEST_P  0
  #define FS_MAX_OPEN_FILES_UTEST_P 0
#endif

/* Maximum number of overhead files in the system */
/* Maximum number of simultaneously "open" overhead files in the system */
#define FS_MAX_NUM_FILES_BASE_P     20
#define FS_MAX_OPEN_FILES_BASE_P    20

/* Maximum number of files in the system */
#define FS_MAX_NUM_FILES_P \
        (FS_MAX_NUM_FILES_BASE_P + \
         FS_MAX_NUM_FILES_NVIM_P + \
         FS_MAX_NUM_FILES_VR_P   + \
         FS_MAX_NUM_FILES_MM_P   + \
         FS_MAX_NUM_FILES_UTEST_P)

/* Maximum number of simultaneously "open" files in the system */
#define FS_MAX_OPEN_FILES_P \
        (FS_MAX_OPEN_FILES_BASE_P + \
         FS_MAX_OPEN_FILES_NVIM_P + \
         FS_MAX_OPEN_FILES_VR_P   + \
         FS_MAX_OPEN_FILES_MM_P   + \
         FS_MAX_OPEN_FILES_UTEST_P)



/* Size of a physical block in bytes */
#ifdef FEATURE_PHONE_VR
  #if (defined(FEATURE_MULTIMEDIA) || defined(FEATURE_FFA))
    #define FS_PHYSICAL_BLKSIZE_P   256
  #else
    #define FS_PHYSICAL_BLKSIZE_P   1024
  #endif
#else
  #define FS_PHYSICAL_BLKSIZE_P     256
#endif

/* Maximum total number of blocks in any sector */
#ifdef FEATURE_FS_RAM_DRIVER
  #define FS_MAX_BLKS_ALL_SECTORS_P (1024/FS_PHYSICAL_BLKSIZE_P)
#else
  #define FS_MAX_BLKS_ALL_SECTORS_P (65536/FS_PHYSICAL_BLKSIZE_P)
#endif

/* Maximum total number of sectors in the device */
#ifdef FEATURE_FS_RAM_DRIVER
  #define FS_NUM_SECTORS_P          30
#else
  #define FS_NUM_SECTORS_P          40
#endif

/* Maximum amount of file space to be managed by the file system (this  */
/* parameter can be used to limit the amount of RAM required when the   */
/* file system is used with devices that provide more capacity than     */
/* needed).  A setting of 0 indicates "no limitation": the full         */
/* capacity of the device will be available.  Otherwise, the maximum    */
/* amount of file system space will be either the full capacity of the  */
/* device or the values of this parameter, whichever is smaller.  The   */
/* units are bytes (NOTE: the value will be rounded down to the nearest */
/* complete sector).                                                    */
#ifdef FEATURE_NV_ITEM_MGR
  #ifdef FEATURE_PHONE_VR
    #ifdef FEATURE_MULTIMEDIA
      #ifdef FEATURE_MP4_DECODER
        #define FS_FILE_SPACE_LIMIT_P 1048576
      #else  /* !FEATURE_MP4_DECODER */
        #define FS_FILE_SPACE_LIMIT_P 0
      #endif /* FEATURE_MP4_DECODER */
    #elif defined(FEATURE_FFA)
#error code not present
    #else
      #define FS_FILE_SPACE_LIMIT_P 655360
    #endif
  #elif defined(FEATURE_MULTIMEDIA)
    #ifdef FEATURE_MP4_DECODER
      #define FS_FILE_SPACE_LIMIT_P 1048576
    #else  /* !FEATURE_MP4_DECODER */
      #define FS_FILE_SPACE_LIMIT_P 0
    #endif /* FEATURE_MP4_DECODER */
  #else
    #define FS_FILE_SPACE_LIMIT_P   196608
  #endif
#else
  #ifdef FEATURE_PHONE_VR
    #ifdef FEATURE_FS_RAM_DRIVER
      #define FS_FILE_SPACE_LIMIT_P 30720
    #else
      #define FS_FILE_SPACE_LIMIT_P 655360
    #endif
  #else
    #define FS_FILE_SPACE_LIMIT_P   0
  #endif
#endif

/* Maximum size of EFS supported */
/* This number should be updated each time to the largest size of EFS
   we support on all devices. */
#define  FS_MAX_FILE_SPACE_LIMIT_P  2097152

/* Number of entries per file block list node */
#define  FS_FBL_ENTRIES_PER_NODE_P  1

/* Maximum number of file block list nodes in the system */
#if (FS_FILE_SPACE_LIMIT_P != 0)
  #define FS_MAX_FBL_NODES_P        (FS_FILE_SPACE_LIMIT_P / FS_PHYSICAL_BLKSIZE_P)
#elif defined(FEATURE_NV_ITEM_MGR)
  #define FS_MAX_FBL_NODES_P        (FS_MAX_FILE_SPACE_LIMIT_P / FS_PHYSICAL_BLKSIZE_P)
#else
  #define FS_MAX_FBL_NODES_P        3000
#endif

/* Maximum number of remote files */
#define  FS_MAX_REMOTE_FILES_P      20

/* Maximum size in bytes of transaction log system files */
#define  FS_MAX_TLOG_SIZE_P         476

/* Maximum number of outstanding file space reservations */
#define  FS_MAX_RESERVATIONS_P      10

/* Maximum number of command packets to the file system task */
#define  FS_MAX_CMD_PACKETS_P       25

/* Maximum number of commands on the internal sub-command queue */
#define  FS_MAX_SUBCMDS_P           30

#endif /* FS_PARMS */














