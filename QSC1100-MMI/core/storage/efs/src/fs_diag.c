/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====
 *
 *                 Diagnostics Packet Processing
 *
 *
 *  Copyright (C) 2002--2009  QUALCOMM, Incorporated.
 *  All Rights Reserved.
 *
 *  Diagnostic packet processing routines for EFS2
 *
 *
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


 /*===========================================================================

                           Edit History

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_diag.c#8 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/09   yog     Add support for hotplug_dev_get_device_info diag packet.
12/12/08    yg     Included fs_diag_i.h internal header file.
12/04/08   yog     Add support for idle_device_event_duration diag packet.
10/06/08    np     Add support for efs_shred diagnostic packet.
09/19/08   wek     Changed fs_diag_hotplug_format to use hotplug_format().
08/22/08    yg     Remove all simulator specific preprocessor macros.
07/23/08   wek     Added support for hotplug format diagnostic packet.
05/22/08    rp     Added back PACKED and PACKED_POST for void * arguments.
05/20/08    eb     Removed PACKED and PACKED_POST for void * arguments
04/03/08    eb     Added PACKED_POST directive for GCC support.
03/20/08    sh     Check resonses for NULL.
03/14/08    rp     Fix memset() compiler error in fs_diag_statvfs_v2_handler.
01/11/08    eb     Added handler fs_diag_statvfs_v2_handler
02/22/08    np     Add support for md5sum calculation diag packet.
10/29/07    sch    Assigned 0 to diag_errno if deltree/truncate succeeds
08/30/07    np     Add support for efs_truncate diagnostic packets.
08/19/07    rp     Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
03/20/07    sh     Avoid the use of the MIN macro.
02/12/07    sh     Don't pass packed strings to strlcpy.
01/23/07    sh     Correctly implement alternate Diag subsystem ID
01/12/07   dlb     Use safer string functions.
12/22/06    sh     Remove accidental Hotplug dependency.
12/21/06    sh     Lint refinement.
12/14/06    sh     Lint changes.
12/14/06    sh     Access validation cleanup.
12/13/06   umr     Add sequence_number support to efs_deltree handler.
12/12/06    sh     Lint fixes. (db)
12/06/06   umr     Added diag interface for Sequential [In Order] put / get
12/08/06   umr     Deprecated put / get request handlers.
10/19/06   umr     Added diag interface for efs_deltree ()
10/26/06    sh     Featurize CEFS Factory Image code (security risk).
10/26/06    sh     Perform Access Validation against READ/WRITE during OPEN.
10/10/06    sh     Allow target conf to override EFS Subsystem ID
09/19/06    sh     Rename all instances of response errno to diag_errno.
07/12/06    sh     Allow target to select processor for FS Diag subsys.
06/29/06    sh     Remove mistaken PACKED.
06/24/06    sh     Replace request->path with a non-__packed ptr.
06/23/06    sh     Fix benchmark packets so they can be built.
06/22/06   dlb     Validate paths for nul termination, for all paths.
06/20/06   dlb     Fix fstat handler for new descriptors.
06/19/06   dlb     Get max rx/tx packets from diagtune.h
06/14/06   dlb     Cleanup of operations involving handles or descriptors.
06/14/06   dlb     Change rsp/req to response/request for clarity.
06/09/06   dlb     More major cleanup and fixing: open, write, read.
06/08/06    sh     Moved fs_diag_efs2_error_rsp into fs_diag.h
05/16/06   dlb     Updates to compile again off target.
05/15/06    sh     Skip any directory entry that errors lstat.
05/08/06    sh     Fixed RVCT warning about packed path
05/07/06    sh     Removed superfluous break/return statements.
04/13/06    sh     Corrected case indenting
03/31/06    sh     Lint cleanup
03/28/06    sh     Removed all EFS1 calls from fs_diag, using only EFS2 now.
02/22/06   nrs     Convert pointers to correct type in new Q&R diag packets
02/06/06    sh     Correct diag seek to use the right file handle
02/01/06    sh     Correcting diag use of stdio
01/26/06    sh     Added packets for chown, and get/set reservation/quota.
12/12/05    sh     Lint cleanup.
12/12/05   nrs     Bug write_handler causes infinite loop
11/28/05   nrs     Merge 7500 changes
11/01/05    sh     Added fs_benchmark handlers
11/01/05   nrs     Fix bug for when MMC is in the build, but not USB
10/31/05   nrs     Fixed USB support in fs_statfs causing checks on MMC
10/04/05   nrs     Initialize file lookup table for stdio interface
10/12/05    sh     Removed some Access Validation checks (READ,WRITE,etc)
10/12/05    sh     Added ZAP macros for debug messages (disabled by default)
10/12/05    sh     Corrected error value for MMC iterator busy.
08/25/05    sh     Featurized stdio interface for MSM7500
08/19/05    sh     Fixed typo '==' instead of assignment.
08/08/05   sch     Added USB support in fs_diag_test_mmc_path
08/05/05    sh     Fix close_status warning in fs_diag_close_handler
08/01/05    sh     fs_diag_open_handler() needs to detect efs_fopen() failure
06/22/05   dlb     Add extended info query packet.
04/27/05   nrs     Re-fix uninitialized close status.
04/26/05   dlb     Fix uninitialized close status.
04/22/05   nrs     Explicitly set close response pointer to 0 in
                   fs_diag_close_handler on success.
04/14/05   nrs     Fix access permission checks in fs_diag_open_handler()
04/07/05   nrs     Implemented write buffering
03/22/05   dlb     Fix RVCT error in efs_get.
03/07/05   nrs     Ignore irrelevant lint warnings
03/04/05   as      Moved validation check before allocating close rsp pkt.
03/03/05   nrs     Fixed lint errors
02/24/05   nrs     Added Generic EFS2 Diag Error packet
02/15/05   nrs     Removed bytes written from put response packet.
02/10/05   nrs     Added packet structures for put and get
01/31/04   sch     Fixed the problem of overwriting an existing file in MMC
12/07/04   nrs     Added temporary fix for delays using QPST
10/14/04   nrs     Fixed leading slash compatability issue from EFS1
10/07/04   dlb     Whitespace cleanup.
06/23/04    gr     Changes to keep the RVCT 2.1 compiler happy.
06/15/04    gr     Disabled security check in the unit test environment.
05/18/04    as     Added security check to fs diag packets.
05/18/04    gr     Fixes for compiler warnings.
05/18/04    gr     Fixed some bad formatting of code.
05/17/04    gr     Set the fstype field correctly in the statfs response.
04/08/04    gr     Modifications to not attempt an MMC enumeration if an MMC
                   card is not present.
10/17/03    gr     Modifications to get this module to work on target as well
                   as in the unit test framework.
09/30/03    pg     Added fs_diag_get_fs_errno to convert efs1 error code to
                   efs2 error code.
                   Modified all handler to return more accurate error codes
                   from MMC operations.
09/14/03   adm     Added support for efs_image_prepare function
08/27/03    gr     Changes to get rid of warnings and errors produced by the
                   RVCT 2.0 compiler.
07/16/03    gr     Moved access validation code into its own file.
07/03/03   spf     Empty directory returned instead of error when MMC/SD card
                   not present.
06/17/03   jkl     Clean up code.
06/16/03   spf     Added factory image support.
                   Minor code cleanup.
06/12/03    gr     Added support for validating access to files and
                   directories.
05/30/03   spf     Added MMC support using EFS1 interface.
04/03/03    gr     Modified the mkdir and rmdir handlers to ignore trailing
                   slashes. This keeps the tools happy.
10/25/02    gr     Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "fs_util.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "fs_diag_i.h"
#include "fs_err.h"
#include "fs_efs2.h"
#include "fs_stdio.h"
#include "fs_benchmark.h"
#include "msg.h"
#include "assert.h"

#ifdef FEATURE_EFS2

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_hotplug.h"

#include "diag.h"
#include "fs_diag_access.h"
#include "jzap.h"
#include "fs_global_lock.h"
#include "fs_util.h"

/*
 * The diag packets frequently contain a pathname as the last field.
 * This is listed in the structure as char foo[1], while the variable-length
 * pathname extends past the end of the structure.  Not surprisingly, lint
 * sees this as:
 * Error (Warning) 661: Possible access of out-of-bounds pointer
 * (1 beyond end of data) by operator '['
 * This is intentional, so the warning is avoided by casting to
 * dereferencing the string within.  Such is lint.
 */
#define LONG_STRING_CAST(x) ((char *)(void *****)(x))

/* By default, we use the well-defined EFS Subsystem ID
 * (DIAG_SUBSYS_FS) for all EFS diag commands.
 *
 * On multi-processor builds, the build may want to use an alternate
 * EFS Subsystem ID to address the second (slave) processor.  We allow
 * the build configuration to override the EFS Subsystem ID here, by
 * declaring EFS_DIAG_USE_THIS_SUBSYS_ID externally.  If none is
 * defined, we use the well-known value as our default.
 */
#ifdef            EFS_DIAG_USE_THIS_SUBSYS_ID
#define FS_SUBSYS EFS_DIAG_USE_THIS_SUBSYS_ID
#else
#define FS_SUBSYS DIAG_SUBSYS_FS
#endif

/* -------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */

/* External functions for factory image output */
extern int efs_get_fs_data (struct fs_factimage_read_info *image_info_ptr,
                            byte *page_buffer);
extern int efs_get_device_info (struct fs_device_attr *dev_attr);

typedef struct {
  uint32 targ_pkt_window;       /* Target window size in packets            */
  uint32 targ_byte_window;      /* Target window size in bytes              */
  uint32 host_pkt_window;       /* Host window size in packets              */
  uint32 host_byte_window;      /* Host window size in bytes                */
  uint32 iter_pkt_window;       /* Dir iteration window size in packets     */
  uint32 iter_byte_window;      /* Dir iteration window size in bytes       */
  int32  version;               /* Protocol version number                  */
  int32  min_version;           /* Minimum supported protocol version       */
  int32  max_version;           /* Maximum supported protocol version       */
  int32  feature_bits;          /* Bit mask of supported features           */
} fs_diag_params_type;

/* The maximum number of file descriptors open at any time
*/
#define FS_DIAG_MAX_FDS  6

/* The maximum number of directories open for reading at any time
*/
#define FS_DIAG_MAX_OPEN_DIRS  4

/* Lint wants parens around macro but compile will fail */
/*lint -save -e773 */
#define VOID_PACKED_PTR        PACKED void * PACKED_POST
#define DECLARE_DIAG_FN(fn)    VOID_PACKED_PTR fn (VOID_PACKED_PTR, uint16)
/*lint -restore */


DECLARE_DIAG_FN (fs_diag_hello_handler);
DECLARE_DIAG_FN (fs_diag_query_handler);
DECLARE_DIAG_FN (fs_diag_open_handler);
DECLARE_DIAG_FN (fs_diag_close_handler);
DECLARE_DIAG_FN (fs_diag_read_handler);
DECLARE_DIAG_FN (fs_diag_write_handler);
DECLARE_DIAG_FN (fs_diag_symlink_handler);
DECLARE_DIAG_FN (fs_diag_readlink_handler);
DECLARE_DIAG_FN (fs_diag_unlink_handler);
DECLARE_DIAG_FN (fs_diag_mkdir_handler);
DECLARE_DIAG_FN (fs_diag_rmdir_handler);
DECLARE_DIAG_FN (fs_diag_opendir_handler);
DECLARE_DIAG_FN (fs_diag_readdir_handler);
DECLARE_DIAG_FN (fs_diag_closedir_handler);
DECLARE_DIAG_FN (fs_diag_rename_handler);
DECLARE_DIAG_FN (fs_diag_stat_handler);
DECLARE_DIAG_FN (fs_diag_lstat_handler);
DECLARE_DIAG_FN (fs_diag_fstat_handler);
DECLARE_DIAG_FN (fs_diag_chmod_handler);
DECLARE_DIAG_FN (fs_diag_statfs_handler);
DECLARE_DIAG_FN (fs_diag_access_handler);
DECLARE_DIAG_FN (fs_diag_device_info_handler);
DECLARE_DIAG_FN (fs_diag_factimage_start_handler);
DECLARE_DIAG_FN (fs_diag_factimage_read_handler);
DECLARE_DIAG_FN (fs_diag_factimage_end_handler);
DECLARE_DIAG_FN (fs_diag_prep_factimage_handler);
DECLARE_DIAG_FN (fs_diag_put_deprecated_handler);
DECLARE_DIAG_FN (fs_diag_get_deprecated_handler);
DECLARE_DIAG_FN (fs_diag_put_handler);
DECLARE_DIAG_FN (fs_diag_get_handler);
DECLARE_DIAG_FN (fs_diag_extended_info_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_start_test_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_get_results_handler);
DECLARE_DIAG_FN (fs_diag_benchmark_init_handler);
DECLARE_DIAG_FN (fs_diag_chown_handler);
DECLARE_DIAG_FN (fs_diag_set_reservation_handler);
DECLARE_DIAG_FN (fs_diag_set_quota_handler);
DECLARE_DIAG_FN (fs_diag_get_group_info_handler);
DECLARE_DIAG_FN (fs_diag_deltree_handler);
DECLARE_DIAG_FN (fs_diag_truncate_handler);
DECLARE_DIAG_FN (fs_diag_ftruncate_handler);
DECLARE_DIAG_FN (fs_diag_statvfs_v2_handler);
DECLARE_DIAG_FN (fs_diag_md5sum_handler);
DECLARE_DIAG_FN (fs_diag_hotplug_format_handler);
DECLARE_DIAG_FN (fs_diag_shred_handler);
DECLARE_DIAG_FN (fs_diag_set_idle_dev_evt_dur_handler);
DECLARE_DIAG_FN (fs_diag_hotplug_device_info_handler);

/* -------------------------------------------------------------------------
** Local Data
** ------------------------------------------------------------------------- */
static fs_diag_params_type fs_diag_params;
static const fs_diag_params_type fs_diag_default_params = {
  FS_TARG_PKT_WINDOW_DEFAULT,
  FS_TARG_BYTE_WINDOW_DEFAULT,
  FS_HOST_PKT_WINDOW_DEFAULT,
  FS_HOST_BYTE_WINDOW_DEFAULT,
  FS_ITER_PKT_WINDOW_DEFAULT,
  FS_ITER_BYTE_WINDOW_DEFAULT,
  FS_DIAG_VERSION,
  FS_DIAG_MIN_VERSION,
  FS_DIAG_MAX_VERSION,
  FS_FEATURE_BITS
};

/* The descriptor used external to diag is merely an index into this array,
 * not a real EFS2 descriptor.  We only allow access by diag to files that
 * were opened through diag.  For each opened descriptor, there is a single
 * element here for this.  There will always be an EFS2 descriptor
 * associated with each open file, and that will sometimes be associated
 * with a buffered EFS_FILE pointer.  A -1 for the fd field indicates that
 * this entry is unused. */
typedef struct {
  int       fd;
  EFS_FILE *file;
} desc_entry;
static desc_entry fsd_desc_table[FS_DIAG_MAX_FDS];

/* Directory 'dirp' descriptors are entries into this table.  NULL entries
 * indicate a directory that is not open.  Because the protocol defines a
 * 'dirp' of 0 as an error condition, we access this table with an offset
 * of 1. */
typedef struct {
  EFSDIR        *dirp;
  int32         seqno;
  char          path[FS_PATH_MAX+1];
} dir_table_entry;
static dir_table_entry fsd_dir_table[FS_DIAG_MAX_OPEN_DIRS];

/* The dispatch table for the EFS subsystem.
*/
static const diagpkt_user_table_entry_type fs_diag_tbl[] =
{
  {EFS2_DIAG_HELLO,    EFS2_DIAG_HELLO,    fs_diag_hello_handler},
  {EFS2_DIAG_QUERY,    EFS2_DIAG_QUERY,    fs_diag_query_handler},
  {EFS2_DIAG_OPEN,     EFS2_DIAG_OPEN,     fs_diag_open_handler},
  {EFS2_DIAG_CLOSE,    EFS2_DIAG_CLOSE,    fs_diag_close_handler},
  {EFS2_DIAG_READ,     EFS2_DIAG_READ,     fs_diag_read_handler},
  {EFS2_DIAG_WRITE,    EFS2_DIAG_WRITE,    fs_diag_write_handler},
  {EFS2_DIAG_SYMLINK,  EFS2_DIAG_SYMLINK,  fs_diag_symlink_handler},
  {EFS2_DIAG_READLINK, EFS2_DIAG_READLINK, fs_diag_readlink_handler},
  {EFS2_DIAG_UNLINK,   EFS2_DIAG_UNLINK,   fs_diag_unlink_handler},
  {EFS2_DIAG_MKDIR,    EFS2_DIAG_MKDIR,    fs_diag_mkdir_handler},
  {EFS2_DIAG_RMDIR,    EFS2_DIAG_RMDIR,    fs_diag_rmdir_handler},
  {EFS2_DIAG_OPENDIR,  EFS2_DIAG_OPENDIR,  fs_diag_opendir_handler},
  {EFS2_DIAG_READDIR,  EFS2_DIAG_READDIR,  fs_diag_readdir_handler},
  {EFS2_DIAG_CLOSEDIR, EFS2_DIAG_CLOSEDIR, fs_diag_closedir_handler},
  {EFS2_DIAG_RENAME,   EFS2_DIAG_RENAME,   fs_diag_rename_handler},
  {EFS2_DIAG_STAT,     EFS2_DIAG_STAT,     fs_diag_stat_handler},
  {EFS2_DIAG_LSTAT,    EFS2_DIAG_LSTAT,    fs_diag_lstat_handler},
  {EFS2_DIAG_FSTAT,    EFS2_DIAG_FSTAT,    fs_diag_fstat_handler},
  {EFS2_DIAG_CHMOD,    EFS2_DIAG_CHMOD,    fs_diag_chmod_handler},
  {EFS2_DIAG_STATFS,   EFS2_DIAG_STATFS,   fs_diag_statfs_handler},
  {EFS2_DIAG_ACCESS,   EFS2_DIAG_ACCESS,   fs_diag_access_handler},
  {EFS2_DIAG_NAND_DEV_INFO,
   EFS2_DIAG_NAND_DEV_INFO,
   fs_diag_device_info_handler},
#ifdef FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE
  {EFS2_DIAG_FACT_IMAGE_START,
   EFS2_DIAG_FACT_IMAGE_START,
   fs_diag_factimage_start_handler},
  {EFS2_DIAG_FACT_IMAGE_READ,
   EFS2_DIAG_FACT_IMAGE_READ,
   fs_diag_factimage_read_handler},
  {EFS2_DIAG_FACT_IMAGE_END,
   EFS2_DIAG_FACT_IMAGE_END,
   fs_diag_factimage_end_handler},
  {EFS2_DIAG_PREP_FACT_IMAGE,
   EFS2_DIAG_PREP_FACT_IMAGE,
   fs_diag_prep_factimage_handler},
#endif /* FEATURE_EFS_ENABLE_FACTORY_IMAGE_SECURITY_HOLE */
  {EFS2_DIAG_PUT_DEPRECATED,
   EFS2_DIAG_PUT_DEPRECATED,
   fs_diag_put_deprecated_handler},
  {EFS2_DIAG_GET_DEPRECATED,
   EFS2_DIAG_GET_DEPRECATED,
   fs_diag_get_deprecated_handler},
  { EFS2_DIAG_EXTENDED_INFO,
    EFS2_DIAG_EXTENDED_INFO,
    fs_diag_extended_info_handler },
  {EFS2_DIAG_BENCHMARK_START_TEST,
   EFS2_DIAG_BENCHMARK_START_TEST,
   fs_diag_benchmark_start_test_handler},
  {EFS2_DIAG_BENCHMARK_GET_RESULTS,
   EFS2_DIAG_BENCHMARK_GET_RESULTS,
   fs_diag_benchmark_get_results_handler},
  {EFS2_DIAG_BENCHMARK_INIT,
   EFS2_DIAG_BENCHMARK_INIT,
   fs_diag_benchmark_init_handler},
  {EFS2_DIAG_CHOWN,
   EFS2_DIAG_CHOWN,
   fs_diag_chown_handler},
  {EFS2_DIAG_SET_RESERVATION,
   EFS2_DIAG_SET_RESERVATION,
   fs_diag_set_reservation_handler},
  {EFS2_DIAG_SET_QUOTA,
   EFS2_DIAG_SET_QUOTA,
   fs_diag_set_quota_handler},
  {EFS2_DIAG_GET_GROUP_INFO,
   EFS2_DIAG_GET_GROUP_INFO,
   fs_diag_get_group_info_handler},
  {EFS2_DIAG_DELTREE,
   EFS2_DIAG_DELTREE,
   fs_diag_deltree_handler},
  {EFS2_DIAG_PUT,
   EFS2_DIAG_PUT,
   fs_diag_put_handler},
  {EFS2_DIAG_GET,
   EFS2_DIAG_GET,
   fs_diag_get_handler},
  {EFS2_DIAG_TRUNCATE,
   EFS2_DIAG_TRUNCATE,
   fs_diag_truncate_handler},
  {EFS2_DIAG_FTRUNCATE,
   EFS2_DIAG_FTRUNCATE,
   fs_diag_ftruncate_handler},
  {EFS2_DIAG_STATVFS_V2,
   EFS2_DIAG_STATVFS_V2,
   fs_diag_statvfs_v2_handler},
  {EFS2_DIAG_MD5SUM,
   EFS2_DIAG_MD5SUM,
   fs_diag_md5sum_handler},
  {EFS2_DIAG_HOTPLUG_FORMAT,
   EFS2_DIAG_HOTPLUG_FORMAT,
   fs_diag_hotplug_format_handler},
  {EFS2_DIAG_SHRED,
   EFS2_DIAG_SHRED,
   fs_diag_shred_handler},
  {EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
   EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
   fs_diag_set_idle_dev_evt_dur_handler},
  {EFS2_DIAG_HOTPLUG_DEVICE_INFO,
   EFS2_DIAG_HOTPLUG_DEVICE_INFO,
   fs_diag_hotplug_device_info_handler},
};

/* The dispatch table can be registered before runtime if this file is
** being built as a C++ file.
*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER (FS_SUBSYS, fs_diag_tbl);
#endif

/**********************************************************************
 * Packet memoization.
 *
 * The diag protocol is not robust, and therefore clients may retransmit
 * packets that were not acknowledged.  Unfortunately, this is not hidden
 * from us as it would be in a reasonable protocol.  Since many of the diag
 * EFS2 requests are not idempotent, it is necessary to detect these
 * retransmissions and return the same response, without redoing the work
 * itself.
 **********************************************************************/

/* The size of the largest possible request packet.  */
#define LARGEST_DIAG_PACKET     DIAG_MAX_RX_PKT_SIZ
#define LARGEST_DIAG_RESPONSE   DIAG_MAX_TX_PKT_SIZ

static char fsd_last_packet[LARGEST_DIAG_PACKET];
static unsigned fsd_last_packet_len;
static char fsd_last_response[LARGEST_DIAG_RESPONSE];
static unsigned fsd_last_response_len;

/* Memoize this packet and response. */
static void fsd_memoize_packet
(PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void *  PACKED_POST response, unsigned rsp_len);

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void fsd_invalidate_memoization (void);

/* Check if this request is the same as a previous one.  Will allocate a
 * response and return a copy of that last response.  Note that this macro
 * has a 'return' in it that will return a result if no work should be
 * done. */
#define FSD_MEMOIZED_PACKET_CHECK(pkt, len, code) \
  do { \
    if ((len) == fsd_last_packet_len && \
        memcmp ((void *)(pkt), fsd_last_packet, fsd_last_packet_len) == 0) \
    { \
      PACKED void *  PACKED_POST __rsp = diagpkt_subsys_alloc (FS_SUBSYS, \
          (code), fsd_last_response_len); \
      if (__rsp != NULL) \
        memcpy ((void *) __rsp, fsd_last_response, fsd_last_response_len); \
      return __rsp; \
    } \
  } while (0)

static void
fsd_memoize_packet (PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void * PACKED_POST response, unsigned rsp_len)
{
  /* If we somehow get too large of a packet or response, don't remember
   * it. */
  if (req_len > LARGEST_DIAG_PACKET ||
      rsp_len > LARGEST_DIAG_RESPONSE)
  {
    fsd_invalidate_memoization ();
  } else {
    memcpy (fsd_last_packet, (void *) request, req_len);
    fsd_last_packet_len = req_len;

    memcpy (fsd_last_response, (void *) response, rsp_len);
    fsd_last_response_len = rsp_len;
  }
}

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void
fsd_invalidate_memoization (void)
{
  fsd_last_packet_len = 0;
  fsd_last_response_len = 0;
}

/**********************************************************************
 * Packet pathname validation.
 *********************************************************************/

/* Count the number of nul characters in the given string. */
static unsigned
fsd_count_nulls (PACKED char *  PACKED_POST name, unsigned length)
{
  unsigned i;
  unsigned result = 0;

  for (i = 0; i < length; i++) {
    if (name[i] == '\0')
      result++;
  }

  return result;
}

/* Convenience macro for checking pathnames.  Takes its argument, and
 * verifies that it contains at least as many nulls.  Arguments are:
 *   request - The typed request packet.
 *   type    - The type of the request packet.
 *   field   - The name of the field containing the pathname
 *   req_len - The total length of the packet
 *   min_nul - Miminum number of nul characters in the string.
 * This macro contains a return, and will return a diag error packet
 * indicating a bad parameter if the request doesn't contain sufficient
 * nulls. */
#define FSD_CHECK_NULLS(_req, _type, _field, _req_len, _min_nul) \
  do { \
    if (fsd_count_nulls ((_req)->_field, \
          (_req_len) - offsetof (_type, _field)) < (_min_nul)) \
    { \
      return diagpkt_err_rsp (DIAG_BAD_PARM_F, \
          (PACKED void *  PACKED_POST) (_req), (_req_len)); \
    } \
  } while (0)

/**********************************************************************
 * Close all open handles.  Used to reset everything when a new connection
 * is established.
 *********************************************************************/

/* Close a single slot.  Returns zero on success or -1 on error (the result
 * of either efs_close or efs_fclose). */
static int
fsd_close_slot (unsigned slot)
{
  int result;

  ASSERT (slot < FS_DIAG_MAX_FDS);
  ASSERT (fsd_desc_table[slot].fd >= 0);

  if (fsd_desc_table[slot].file != NULL) {
    result = efs_fclose (fsd_desc_table[slot].file);
    /* efs_fclose also calls efs_close, so that isn't needed. */
  } else {
    result = efs_close (fsd_desc_table[slot].fd);
  }

  fsd_desc_table[slot].file = NULL;
  fsd_desc_table[slot].fd = -1;

  return result;
}

static void
fsd_close_open_handles (void)
{
  unsigned slot;

  for (slot = 0; slot < FS_DIAG_MAX_FDS; slot++) {
    if (fsd_desc_table[slot].fd >= 0)
      (void) fsd_close_slot (slot);
  }
}

static void
fsd_close_open_dirs (void)
{
  unsigned slot;

  for (slot = 0; slot < FS_DIAG_MAX_OPEN_DIRS; slot++) {
    if (fsd_dir_table[slot].dirp != NULL) {
      (void) efs_closedir (fsd_dir_table[slot].dirp);
      fsd_dir_table[slot].dirp = NULL;
    }
  }
}

/*===========================================================================

FUNCTION FS_DIAG_INIT
DESCRIPTION
  Initialization function for this module. Registers the packet handlers
  and sets up local data structures.
============================================================================*/
void fs_diag_init (void)
{
  unsigned i;

  fs_diag_params = fs_diag_default_params;

  for (i = 0; i < FS_DIAG_MAX_FDS; i++) {
    fsd_desc_table[i].fd = -1;
    fsd_desc_table[i].file = NULL;
  }

  for (i = 0; i < FS_DIAG_MAX_OPEN_DIRS; i++) {
    fsd_dir_table[i].dirp = NULL;
    fsd_dir_table[i].seqno = 0;
  }

  /* --- Register our EFS Diag Subsystem handler. --- */
#ifndef __cplusplus

  /* By default, processor ID "0" will handle the calls.  A build may
     optionally override this. */
#ifndef EFS_DIAG_SUBSYS_PROCESSOR_ID
#define EFS_DIAG_SUBSYS_PROCESSOR_ID 0
#endif

  /* On builds using processor 0, there may not exist the new DIAG
     macro to register with a processor number, so we use the old
     macro to be safe.  Nonzero ID always requires the new macro. */
#if EFS_DIAG_SUBSYS_PROCESSOR_ID == 0
  DIAGPKT_DISPATCH_TABLE_REGISTER (FS_SUBSYS, fs_diag_tbl);
#else
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (EFS_DIAG_SUBSYS_PROCESSOR_ID,
                                        FS_SUBSYS,
                                        fs_diag_tbl);
#endif
#endif /* ! __cplusplus */

  return;
} /* END fs_diag_init */


/*===========================================================================

FUNCTION FS_DIAG_HELLO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_HELLO packet. Initializes the EFS2 Diag interface
  and passes the values of some parameters back to the tool.
============================================================================*/
VOID_PACKED_PTR
fs_diag_hello_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_hello_req_type *request;
  efs2_diag_hello_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_hello_rsp_type);

  ZAP ("_HELLO");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_hello_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_hello_req_type *) req_ptr;

  response = (efs2_diag_hello_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_HELLO,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Initialize this module: close any open handles. */
  fsd_close_open_handles ();
  fsd_close_open_dirs ();

  /*------------------------------------------------------
    Fill in the fields in the response packet
    ------------------------------------------------------*/
  response->targ_pkt_window  = fs_diag_params.targ_pkt_window;
  response->targ_byte_window = fs_diag_params.targ_byte_window;
  response->host_pkt_window  = fs_diag_params.host_pkt_window;
  response->host_byte_window = fs_diag_params.host_byte_window;
  response->iter_pkt_window  = fs_diag_params.iter_pkt_window;
  response->iter_byte_window = fs_diag_params.iter_byte_window;
  response->version          = fs_diag_params.version;
  response->min_version      = fs_diag_params.min_version;
  response->max_version      = fs_diag_params.max_version;
  response->feature_bits     = fs_diag_params.feature_bits;

  /* Update relevant parameters using values specified in the request
   * packet.  _EVERY_ single one of these fields is completely meaningless.
   * Why are we doing any of this?
  */
  if (fs_diag_params.targ_pkt_window > request->targ_pkt_window)
    fs_diag_params.targ_pkt_window = request->targ_pkt_window;
  if (fs_diag_params.targ_byte_window > request->targ_byte_window)
    fs_diag_params.targ_byte_window = request->targ_byte_window;
  if (fs_diag_params.host_pkt_window > request->host_pkt_window)
    fs_diag_params.host_pkt_window = request->host_pkt_window;
  if (fs_diag_params.host_byte_window > request->host_byte_window)
    fs_diag_params.host_byte_window = request->host_byte_window;
  if (fs_diag_params.iter_pkt_window > request->iter_pkt_window)
    fs_diag_params.iter_pkt_window = request->iter_pkt_window;
  if (fs_diag_params.iter_byte_window > request->iter_byte_window)
    fs_diag_params.iter_byte_window = request->iter_byte_window;
  fs_diag_params.version       = request->version;
  fs_diag_params.min_version   = request->min_version;
  fs_diag_params.max_version   = request->max_version;
  fs_diag_params.feature_bits &= request->feature_bits;

  return response;

} /* fs_diag_hello_handler */


/*===========================================================================

FUNCTION FS_DIAG_QUERY_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_QUERY packet. Passes back information about some
  ESF2 parameters.
============================================================================*/
VOID_PACKED_PTR
fs_diag_query_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_query_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_query_rsp_type);

  ZAP ("_QUERY");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_query_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_query_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_QUERY,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /*------------------------------------------------------
    Fill in the fields in the response packet
    ------------------------------------------------------*/
  response->max_name         = FS_NAME_MAX;
  response->max_path         = FS_PATH_MAX;
  response->max_link_depth   = 0;
  response->max_file_size    = 0;
  response->max_dir_entries  = FS_DIR_ENTRY_SIZE;
  response->max_mounts       = FS_MAX_MOUNTS;

  return response;

} /* fs_diag_query_handler */


/*===========================================================================

FUNCTION FS_DIAG_OPEN_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_OPEN packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_open_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_open_req_type *request;
  efs2_diag_open_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_open_rsp_type);
  const char *request_path;
  int32  fd;
  unsigned slot;

#ifndef FEATURE_NO_STDIO_FROM_DIAG
  char *buff_mode = "";
#endif

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_OPEN);

  if (pkt_len < sizeof (efs2_diag_query_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_open_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_open_req_type, path, pkt_len, 1);

  ZPRINTF ("_OPEN: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, request_path);

  /* Old versions of fs_diag would perform Access Validation for each
   * and every Read/Write request, but this was incurring a
   * significant performance penalty.  Later (Oct 2005), these checks
   * were removed entirely from Read/Write packet handlers, which
   * violated some assumptions made by BREW SPAR.  Now we do one test
   * of READ and one test of WRITE during open to confirm that the
   * file mode is acceptable.  If any of these read/write permissions
   * are denied, then the file can not be opened. */
  switch (request->oflag & O_ACCMODE) {
    case O_RDONLY:
      FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);
      break;

    case O_WRONLY:
      FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, request_path);
      break;

    case O_RDWR:
    default:
      FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);
      FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, request_path);
      break;
  }

  response = (efs2_diag_open_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_OPEN,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Find a descriptor slot to use. */
  for (slot = 0; slot < FS_DIAG_MAX_FDS; slot++) {
    if (fsd_desc_table[slot].fd == -1)
      break;
  }

  /* If we are out of diag descriptors, don't try opening, just return the
   * error. */
  if (slot >= FS_DIAG_MAX_FDS) {
    response->fd = -1;
    response->diag_errno = FS_DIAG_TOO_MANY_OPEN_FILES;
    goto open_end;
  }

  /* Open the handle. */
  fd = efs_open (request_path, request->oflag, request->mode);

  /* Check for open failure. */
  if (fd < 0) {
    response->fd = -1;
    response->diag_errno = efs_errno;
    goto open_end;
  }

  /* Update the desc slot. */
  fsd_desc_table[slot].fd = fd;
  fsd_desc_table[slot].file = NULL;

  response->fd = slot;
  response->diag_errno = 0;

  goto open_end;

#ifndef FEATURE_NO_STDIO_FROM_DIAG
  /* Open the stdio handle to use for reading and writing.  Determining the
   * mode from the oflags. */
  switch (request->mode & O_ACCMODE) {
    case O_RDONLY:
      buff_mode = "r";
      break;
    case O_WRONLY:
      buff_mode = "w";
      break;
    case O_RDWR:
    default:
      buff_mode = "r+";
      break;
  }

  fsd_desc_table[slot].file = efs_fdopen (fd, buff_mode);
  /* If we didn't get a handle, the file pointer in the slot will be NULL,
   * and the unbuffered operations will be used.  TODO: log a warning about
   * this? */
#endif /* FEATURE_NO_STDIO_FROM_DIAG */

  open_end:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;

} /* END fs_diag_open_handler */


/*===========================================================================

FUNCTION FS_DIAG_CLOSE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CLOSE packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_close_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_close_req_type *request;
  efs2_diag_close_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_close_rsp_type);
  int result;

  ZAP ("_CLOSE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_CLOSE);

  if (pkt_len != sizeof (efs2_diag_close_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_close_req_type *) req_ptr;

  response = (efs2_diag_close_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CLOSE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = 0;

  /* If this handle is not open, or is out of range, return an error. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = EBADF;
    goto close_done;
  }

  result = fsd_close_slot (request->fd);

  if (result != 0)
    response->diag_errno = efs_errno;

  close_done:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_close_handler */


/*===========================================================================

FUNCTION FS_DIAG_READ_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READ packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_read_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_read_req_type *request;
  efs2_diag_read_rsp_type *response;
  desc_entry *slot;
  unsigned base_length = offsetof (efs2_diag_read_rsp_type, data);
  unsigned rsp_len = base_length;
  fs_ssize_t bytes_left;
  fs_off_t   offset;
  fs_ssize_t count;

  ZAP ("_READ");
  fsd_invalidate_memoization ();

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_read_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_read_req_type *) req_ptr;

  /* Validate the requested descriptor. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response = (efs2_diag_read_rsp_type *)
      diagpkt_subsys_alloc (FS_SUBSYS,
          EFS2_DIAG_READ, base_length);
    if (response == NULL)
      return NULL;

    response->fd = request->fd;
    response->offset = request->fd;
    response->bytes_read = -1;
    response->diag_errno = EBADF;
    goto read_end;
  }

  slot = &fsd_desc_table[request->fd];

  /* Begin by allocating a full-sized read packet.  We will shorten it if
   * we get an error, or a short read. */
  bytes_left = request->nbyte;
  if (bytes_left > FS_DIAG_MAX_READ_REQ)
    bytes_left = FS_DIAG_MAX_READ_REQ;

  rsp_len += bytes_left;
  response = (efs2_diag_read_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_READ, rsp_len);
  if (response == NULL)
    return NULL;

  response->fd = request->fd;
  response->offset = request->offset;

  /* Seek to the specified location, and attempt the read. */
  if (slot->file == NULL) {
    /* Unbuffered. */
    offset = efs_lseek (slot->fd, request->offset, SEEK_SET);
    if (offset != (fs_off_t) request->offset) {
      response->bytes_read = -1;
      response->diag_errno = efs_errno;
      diagpkt_shorten (response, base_length);
      goto read_end;
    }

    count = efs_read (slot->fd, (void *) response->data, bytes_left);
  } else {
    if (0 != efs_fseek (slot->file, request->offset, SEEK_SET)) {
      response->bytes_read = -1;
      response->diag_errno = efs_errno;
      diagpkt_shorten (response, base_length);
      goto read_end;
    }

    count = efs_fread ((void *) response->data, 1, bytes_left, slot->file);
  }

  /* Fixup the response packet, depending on the result. */
  if (count < 0) {
    response->bytes_read = -1;
    response->diag_errno = efs_errno;
  } else {
    response->bytes_read = count;
    response->diag_errno = 0;
    if (count < bytes_left)
      diagpkt_shorten (response, base_length + count);
  }

  read_end:
  return response;
} /* END fs_diag_read_handler */


/*===========================================================================

FUNCTION FS_DIAG_WRITE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_WRITE packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_write_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_write_req_type *request;
  efs2_diag_write_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_write_rsp_type);
  fs_off_t   offset;
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  fs_ssize_t bytes_left;
  char      *write_locn;
  desc_entry *slot;

  ZAP ("_WRITE");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_write_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_write_req_type *) req_ptr;

  response = (efs2_diag_write_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_WRITE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->fd     = request->fd;
  response->offset = request->offset;

  nbyte = (fs_ssize_t)
    (pkt_len - (uint16) offsetof (efs2_diag_write_req_type, data));

  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = EBADF;
    response->bytes_written = 0;
    goto write_end;
  }

  slot = &fsd_desc_table[request->fd];

  /* Seek to the specified offset and write the specified number of bytes. */

  if (slot->file == NULL) {
    /* Unbuffered */
    offset = efs_lseek (slot->fd, request->offset, SEEK_SET);
    if (offset != (fs_off_t) request->offset) {
      response->bytes_written = -1;
      response->diag_errno = efs_errno;
      goto write_end;
    }
  } else {
    /* Buffered */
    if (0 != efs_fseek (slot->file, request->offset,
                        SEEK_SET)) {
      response->bytes_written = -1;
      response->diag_errno = efs_errno;
      goto write_end;
    }
  }

  offset = (fs_off_t) request->offset;
  bytes_left   = nbyte;
  write_locn   = (char *) request->data;

  while (bytes_left > 0) {
    if (slot->file == NULL) {
      bytes_written = efs_write (slot->fd, (void *) write_locn, bytes_left);
    } else {
      bytes_written = efs_fwrite ((void *) write_locn, 1, bytes_left,
          slot->file);
    }

    if (bytes_written <= 0) {
      /* XXX: This is an incorrect response, but I suspect at this point
       * there are external tools that depend on this behavior. */
      response->bytes_written = -1;
      response->diag_errno = efs_errno;
      goto write_end;
    }

    bytes_left -= bytes_written;
    write_locn += bytes_written;
  }

  /* (bytes_written >= 0) when it gets here */
  response->bytes_written = nbyte;
  response->diag_errno         = 0;

  write_end:
  return response;
} /* END fs_diag_write_handler */


/*===========================================================================

FUNCTION FS_DIAG_SYMLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_SYMLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_symlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_symlink_req_type *request;
  efs2_diag_symlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_symlink_rsp_type);
  const char *oldpath;
  const char *newpath;
  int32  oldpath_len;
  int32  symlink_result;

  ZAP ("_SYMLINK");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SYMLINK);

  if (pkt_len < sizeof (efs2_diag_symlink_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_symlink_req_type *) req_ptr;

  FSD_CHECK_NULLS (request, efs2_diag_symlink_req_type,
      oldpath, pkt_len, 2);

  /* Do a sanity check on the input parameters.
   */
  oldpath = (const char *) request->oldpath;
  oldpath_len = (int32) strlen (oldpath);
  newpath = (const char *) (request->oldpath + oldpath_len + 1);
  if (oldpath_len + strlen (newpath) + 2 > pkt_len)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (SYMLINK, oldpath);
  FS_DIAG_VALIDATE_PATH_ACCESS (SYMLINK, newpath);

  response = (efs2_diag_symlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SYMLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to create the symbolic link. Do not report an error if the
  ** creation fails because the link already exists.
  */
  symlink_result = efs_symlink (oldpath, newpath);

  if ((symlink_result == 0) || (efs_errno == EEXIST))
    response->diag_errno = 0;
  else
    response->diag_errno = efs_errno;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_symlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_READLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_readlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_readlink_req_type *request;
  efs2_diag_readlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_readlink_rsp_type);
  char   buf[FS_PATH_MAX];
  int32  readlink_result;
  const char *request_path;

  buf[0] = 0;                   /* Praise lint */
  ZAP ("_READLINK");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_readlink_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_readlink_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_readlink_req_type,
      path, pkt_len, 1);



  FS_DIAG_VALIDATE_PATH_ACCESS (READLINK, request_path);

  readlink_result = efs_readlink (request_path, buf, FS_PATH_MAX);

  if (readlink_result > 0)
  {
    rsp_len += readlink_result;
  }

  response = (efs2_diag_readlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_READLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (readlink_result >= 0)
  {
    response->diag_errno = 0;
    memcpy ((char *)response->name, buf, readlink_result);
    response->name[readlink_result] = '\0';
  }
  else
  {
    response->diag_errno = efs_errno;
    response->name[0] = '\0';
  }

  return response;
} /* END fs_diag_readlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_UNLINK_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_UNLINK packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_unlink_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_unlink_req_type *request;
  efs2_diag_unlink_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_unlink_rsp_type);
  int32  unlink_result;
  const char *request_path;

  ZAP ("_UNLINK");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_UNLINK);

  if (pkt_len < sizeof (efs2_diag_unlink_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_unlink_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_unlink_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (UNLINK, request_path);

  response = (efs2_diag_unlink_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_UNLINK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to unlink the file. Do not report an error if the operation fails
  ** because the file or link does not exist.
  */
  unlink_result = efs_unlink (request_path);

  if ((unlink_result == 0) || (efs_errno == ENOENT))
    response->diag_errno = 0;
  else
    response->diag_errno = efs_errno;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_unlink_handler */


/*===========================================================================

FUNCTION FS_DIAG_MKDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_MKDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_mkdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_mkdir_req_type *request;
  efs2_diag_mkdir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_mkdir_rsp_type);
  int32  mkdir_result;
  int dirname_len;
  int i;
  char *request_path;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SYMLINK);

  if (pkt_len < sizeof (efs2_diag_mkdir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_mkdir_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  FSD_CHECK_NULLS (request, efs2_diag_mkdir_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_MKDIR \"%s\"\n\r", request_path);

  /* EFS2 does not ignore trailing slashes at the end of directory names, but
   * tools expect this behavior. So remove trailing slashes here.
   */
  dirname_len = strlen (request_path);
  for (i = dirname_len-1; i > 0 && request_path[i] == '/'; i--)
    request_path[i] = '\0';

  FS_DIAG_VALIDATE_PATH_ACCESS (MKDIR, request_path);

  response = (efs2_diag_mkdir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_MKDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Remove the directory and then create it. The remove will do nothing if
  ** the directory doesn't exist, but it won't hurt. If the directory exists
  ** and is not empty, the remove will fail and so will the mkdir call that
  ** follows.
  */


  (void) efs_rmdir (request_path);
  mkdir_result = efs_mkdir (request_path, request->mode);
  if (mkdir_result == -1)
    response->diag_errno = efs_errno;
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_mkdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_RMDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_RMDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_rmdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_rmdir_req_type *request;
  efs2_diag_rmdir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_rmdir_rsp_type);
  int32  rmdir_result;
  int i;
  int dirname_len;
  char *request_path;

  ZAP ("_RMDIR");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_RMDIR);

  if (pkt_len < sizeof (efs2_diag_rmdir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_rmdir_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  FSD_CHECK_NULLS (request, efs2_diag_rmdir_req_type,
      path, pkt_len, 1);

  /* EFS2 does not ignore trailing slashes at the end of directory names, but
   * tools expect this behavior. So remove trailing slashes here.
   */
  dirname_len = strlen (request_path);
  for (i = dirname_len-1; i > 0 && request_path[i] == '/'; i--)
    request_path[i] = '\0';

  FS_DIAG_VALIDATE_PATH_ACCESS (RMDIR, request_path);

  response = (efs2_diag_rmdir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_RMDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Try to remove the directory. If the directory does not exist, do not
  ** report an error.
  */
  rmdir_result = efs_rmdir (request_path);
  if ((rmdir_result == 0) || (efs_errno == ENOENT))
    response->diag_errno = 0;
  else
    response->diag_errno = efs_errno;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_rmdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_OPENDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_OPENDIR packet.

============================================================================*/
VOID_PACKED_PTR
fs_diag_opendir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_opendir_req_type *request;
  efs2_diag_opendir_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_opendir_rsp_type);
  EFSDIR *dirp;
  unsigned slot;
  const char *request_path;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_OPENDIR);

  if (pkt_len < sizeof (efs2_diag_opendir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_opendir_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_opendir_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_OPENDIR %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (OPENDIR, request_path);

  response = (efs2_diag_opendir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_OPENDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Find a descriptor slot to use. */
  for (slot = 0; slot < FS_DIAG_MAX_OPEN_DIRS; slot++) {
    if (fsd_dir_table[slot].dirp == NULL)
      break;
  }

  /* If there are too many open directories, return an error. */
  if (slot >= FS_DIAG_MAX_OPEN_DIRS) {
    response->dirp = 0;
    response->diag_errno = FS_DIAG_TOO_MANY_OPEN_DIRS;
    goto opendir_end;
  }

  /* Check that the name length doesn't exceed the FS_PATH_MAX, since we're
   * about to copy it.
   * TODO: Check for a proper NULL termination of name. */
  if (strlen (request_path) > FS_PATH_MAX) {
    response->dirp = 0;
    response->diag_errno = ENAMETOOLONG;
    goto opendir_end;
  }

  dirp = efs_opendir (request_path);

  if (dirp == 0)
  {
    response->dirp  = 0;
    response->diag_errno = efs_errno;
    goto opendir_end;
  }

  fsd_dir_table[slot].dirp = dirp;

  /* The protocol expects the first directory entry to have a 'seqno' of
   * '1'.  This would give ".." seqno 0, and "." seqno -1.  The
   * fsd_dir_table 'seqno' field indicates the sequence number of the last
   * entry read.  Therefore, we set the last seqno to '-2', since that is
   * the logical entry before ".". */
  fsd_dir_table[slot].seqno = -2;
  fs_strlcpy (fsd_dir_table[slot].path, request_path,
      sizeof (fsd_dir_table[slot].path));

  response->dirp = slot + 1;
  response->diag_errno = 0;

  opendir_end:
  ZPRINTF (" >>>OPENDIR: %d\n\r", response->diag_errno);
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_opendir_handler */

/*===========================================================================
 * Allocate a response packet for readdir, with the specified filename
 * length and error code.  'dirp' and 'seqno' will be filled in from the
 * request packet. */
static efs2_diag_readdir_rsp_type *
fsd_readdir_alloc (efs2_diag_readdir_req_type *request,
    int the_errno,
    const char *entry_name)
{
  efs2_diag_readdir_rsp_type *response;
  unsigned name_length = strlen (entry_name);

  response = (efs2_diag_readdir_rsp_type *) diagpkt_subsys_alloc
    (FS_SUBSYS, EFS2_DIAG_READDIR,
     offsetof (efs2_diag_readdir_rsp_type, entry_name) + name_length + 1);
  if (response != NULL)
  {
    response->dirp = request->dirp;
    response->seqno = request->seqno;
    response->diag_errno = the_errno;
    response->entry_type = 0;
    response->mode = 0;
    response->size = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
    fs_strlcpy ((char *)response->entry_name, entry_name, name_length + 1);
  }

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_READDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_READDIR packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_readdir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_readdir_req_type *request;
  efs2_diag_readdir_rsp_type *response;
  dir_table_entry *slot;
  struct fs_dirent *dirent = NULL;
  struct fs_stat sbuf;
  static char path_buffer[FS_PATH_MAX + 1];
  int result;

  ZAP ("_READDIR");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_readdir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_readdir_req_type *) req_ptr;

  /* Validate the passed in dirp. */
  if (request->dirp < 1 || request->dirp > FS_DIAG_MAX_OPEN_DIRS ||
      fsd_dir_table[request->dirp - 1].dirp == NULL)
  {
    response = fsd_readdir_alloc (request, FS_DIAG_DIR_NOT_OPEN, "");
    goto readdir_end;
  }
  slot = &fsd_dir_table[request->dirp - 1];

  /* Validate the seqno. */
  if (request->seqno <= 0) {
    response = fsd_readdir_alloc (request, FS_DIAG_INVALID_SEQ_NO, "");
    goto readdir_end;
  }

  /* If this request is earlier than previous requests, close and reopen,
   * to reset the sequence number. */
  if (request->seqno <= slot->seqno) {
    (void) efs_closedir (slot->dirp);
    slot->dirp = efs_opendir (slot->path);

    /* If the directory vanish in the interim, return as if the dir failed
     * to open. */
    if (slot->dirp == NULL) {
      MSG_MED ("fs_diag_readdir: dir handle failed to reopen", 0, 0, 0);
      response = fsd_readdir_alloc (request, FS_DIAG_DIR_NOT_OPEN, "");
      goto readdir_end;
    }

    /* Start at the ".." entry. */
    slot->seqno = -2;
  }

  /* Advance to the entry we are trying to read. */
  while (request->seqno > slot->seqno) {
    dirent = efs_readdir (slot->dirp);
    if (dirent == NULL) {
      /* Past end of directory.  Could also indicate an error, but the API
       * doesn't distinguish that. */
      response = fsd_readdir_alloc (request, 0, "");
      goto readdir_end;
    }
    slot->seqno++;
  }

  /* Check for logic error above. */
  ASSERT (dirent != NULL);

  /* Construct a full path to lstat the entry with. */
  if (strlen (slot->path) + strlen (dirent->d_name) + 1 > FS_PATH_MAX) {
    response = fsd_readdir_alloc (request, FS_DIAG_PATH_TOO_LONG, "");
    goto readdir_end;
  }

  fs_strlcpy (path_buffer, slot->path, sizeof (path_buffer));
  fs_strlcat (path_buffer, "/", sizeof (path_buffer));
  fs_strlcat (path_buffer, dirent->d_name, sizeof (path_buffer));

  /* We have our filename, so we can allocate a response, no matter what it
   * will be. */
  response = fsd_readdir_alloc (request, 0, dirent->d_name);
  if (response == NULL)
    return NULL;
  result = efs_lstat (path_buffer, &sbuf);
  if (result != 0) {
    /* We were unable to stat the entry, so we know very little about it.
     * Fill in what we can. */
    response->entry_type = FS_DIAG_FTYPE_UNKNOWN;
  } else {
    /* Copy all of the stat information that we can. */
    response->mode = sbuf.st_mode;
    response->size = sbuf.st_size;
    response->atime = sbuf.st_atime;
    response->mtime = sbuf.st_mtime;
    response->ctime = sbuf.st_ctime;
    switch (sbuf.st_mode & S_IFMT) {
      case S_IFREG:
        response->entry_type = FS_DIAG_FTYPE_REG;
        break;

      case S_IFDIR:
        response->entry_type = FS_DIAG_FTYPE_DIR;
        break;

      case S_IFLNK:
        response->entry_type = FS_DIAG_FTYPE_LINK;
        break;

      default:
        response->entry_type = FS_DIAG_FTYPE_UNKNOWN;

        /* This used to be an error, but shouldn't be. */
        /* response->diag_errno = FS_DIAG_UNKNOWN_FILETYPE; */
        break;
    }
  }

  readdir_end:
  return response;
} /* END fs_diag_readdir_handler */


/*===========================================================================

FUNCTION FS_DIAG_CLOSEDIR_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CLOSEDIR packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_closedir_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_closedir_req_type *request;
  efs2_diag_closedir_rsp_type *response;
  unsigned rsp_len = sizeof (efs2_diag_closedir_rsp_type);
  dir_table_entry *slot;
  int result;

  ZAP ("_CLOSEDIR");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_CLOSEDIR);

  if (pkt_len != sizeof (efs2_diag_closedir_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_closedir_req_type *) req_ptr;

  response = (efs2_diag_closedir_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CLOSEDIR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Validate the slot. */
  if (request->dirp < 1 || request->dirp > FS_DIAG_MAX_OPEN_DIRS ||
      fsd_dir_table[request->dirp - 1].dirp == NULL)
  {
    response->diag_errno = FS_DIAG_DIR_NOT_OPEN;
    goto closedir_end;
  }
  slot = &fsd_dir_table[request->dirp - 1];

  result = efs_closedir (slot->dirp);
  slot->dirp = NULL;

  if (result == 0)
    response->diag_errno = 0;
  else
    response->diag_errno = efs_errno;

  closedir_end:
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;
} /* END fs_diag_closedir_handler */


/*===========================================================================

FUNCTION FS_DIAG_RENAME_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_RENAME packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_rename_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_rename_req_type *rename_req_ptr;
  efs2_diag_rename_rsp_type *rename_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_rename_rsp_type);
  const char *oldpath;
  const char *newpath;
  int32  oldpath_len;

  ZAP ("_RENAME");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_RENAME);

  if (pkt_len < sizeof (efs2_diag_rename_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  rename_req_ptr = (efs2_diag_rename_req_type *) req_ptr;

  FSD_CHECK_NULLS (rename_req_ptr, efs2_diag_rename_req_type,
      oldpath, pkt_len, 2);

  /* Do a sanity check on the input parameters.
   */
  oldpath = (const char *) rename_req_ptr->oldpath;
  oldpath_len = (int32) strlen (oldpath);
  newpath = (const char *) (rename_req_ptr->oldpath + oldpath_len + 1);
  if (oldpath_len + strlen (newpath) + 2 > pkt_len)
  {
    return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (RENAME, oldpath);
  FS_DIAG_VALIDATE_PATH_ACCESS (RENAME, newpath);

  rename_rsp_ptr = (efs2_diag_rename_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_RENAME,
                          rsp_len);
  if (rename_rsp_ptr == NULL)
    return NULL;


  if (efs_rename (oldpath, newpath) == 0)
    rename_rsp_ptr->diag_errno = 0;
  else
    rename_rsp_ptr->diag_errno = efs_errno;

  fsd_memoize_packet (req_ptr, pkt_len, rename_rsp_ptr, rsp_len);
  return rename_rsp_ptr;
} /* END fs_diag_rename_handler */


/*===========================================================================

FUNCTION FS_DIAG_STAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_stat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_stat_req_type *request;
  efs2_diag_stat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_stat_rsp_type);
  struct fs_stat sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_stat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_stat_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_stat_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_STAT: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STAT, request_path);

  response = (efs2_diag_stat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_stat (request_path, &sbuf) == 0)
  {
    response->mode   = (int32) sbuf.st_mode;
    response->size   = (int32) sbuf.st_size;
    response->nlink  = (int32) sbuf.st_nlink;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  }
  else
  {
    response->mode   = 0;
    response->size   = 0;
    response->nlink  = 0;
    response->atime  = 0;
    response->mtime  = 0;
    response->ctime  = 0;
    response->diag_errno  = efs_errno;
  }

  return response;
} /* END fs_diag_stat_handler */


/*===========================================================================

FUNCTION FS_DIAG_LSTAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_LSTAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_lstat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_lstat_req_type *request;
  efs2_diag_lstat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_lstat_rsp_type);
  struct fs_stat sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_lstat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_lstat_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_lstat_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_LSTAT: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (LSTAT, request_path);

  response = (efs2_diag_lstat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_LSTAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* XXX: Figure out which of the following fields make sense for a
  ** symbolic link.
  */
  if (efs_lstat (request_path, &sbuf) == 0)
  {
    response->mode   = (int32) sbuf.st_mode;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  }
  else
  {
    response->mode   = 0;
    response->atime  = 0;
    response->mtime  = 0;
    response->ctime  = 0;
    response->diag_errno  = efs_errno;
  }

  return response;
} /* END fs_diag_lstat_handler */


/*===========================================================================

FUNCTION FS_DIAG_FSTAT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FSTAT packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_fstat_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_fstat_req_type *request;
  efs2_diag_fstat_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_fstat_rsp_type);
  desc_entry *slot;
  struct fs_stat sbuf;

  ZAP ("_FSTAT");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_fstat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_fstat_req_type *) req_ptr;
  response = (efs2_diag_fstat_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FSTAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Validate the requested descriptor. */
  if (request->fd < 0 || request->fd >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fd].fd < 0)
  {
    response->diag_errno = EBADF;
    response->mode = 0;
    response->size = 0;
    response->nlink = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
    goto fstat_end;
  }

  slot = &fsd_desc_table[request->fd];

  if (efs_fstat (slot->fd, &sbuf) == 0) {
    response->mode   = (int32) sbuf.st_mode;
    response->size   = (int32) sbuf.st_size;
    response->nlink  = (int32) sbuf.st_nlink;
    response->atime  = (int32) sbuf.st_atime;
    response->mtime  = (int32) sbuf.st_mtime;
    response->ctime  = (int32) sbuf.st_ctime;
    response->diag_errno  = 0;
  } else {
    response->diag_errno  = efs_errno;
    response->mode = 0;
    response->size = 0;
    response->nlink = 0;
    response->atime = 0;
    response->mtime = 0;
    response->ctime = 0;
  }

  fstat_end:
  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_CHMOD_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_CHMOD packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_chmod_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_chmod_req_type *request;
  efs2_diag_chmod_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_chmod_rsp_type);
  int32  chmod_result;
  const char *request_path;

  ZAP ("_CHMOD");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_chmod_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_chmod_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_chmod_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (CHMOD, request_path);

  response = (efs2_diag_chmod_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CHMOD,
                          rsp_len);
  if (response == NULL)
    return NULL;

  chmod_result = efs_chmod (request_path, request->mode);
  if (chmod_result == 0)
    response->diag_errno  = 0;
  else
    response->diag_errno  = efs_errno;

  return response;
} /* END fs_diag_chmod_handler */


/*===========================================================================

FUNCTION FS_DIAG_STATFS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STATFS packet. This has been extended to
  EFS2_DIAG_STATVFS_V2_HANDLER which contains more filesystem specific info
============================================================================*/
VOID_PACKED_PTR
fs_diag_statfs_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_statfs_req_type *request;
  efs2_diag_statfs_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_statfs_rsp_type);
  struct fs_statvfs sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_statfs_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_statfs_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_statfs_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_STATFS: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STATFS, request_path);

  response = (efs2_diag_statfs_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STATFS,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_statvfs (request_path, &sbuf) == 0)
  {
    /* XXX: Fill in all the fields here.
     */
    response->diag_errno         = 0;
    response->fs_id         = (int32) sbuf.f_fsid;
    memset ((void *)response->fs_type, 0x00, EFS_FSTYPE_LEN);
    response->block_size    = (int32) sbuf.f_bsize;
    response->total_blocks  = (int32) sbuf.f_blocks;
    response->avail_blocks  = (int32) sbuf.f_bavail;
    response->free_blocks   = (int32) sbuf.f_bfree;
    response->max_file_size = 0;
    response->nfiles        = 0;
    response->max_nfiles    = 0;
  }
  else
  {
    response->diag_errno  = efs_errno;
  }

  return response;
} /* END fs_diag_statfs_handler */


/*===========================================================================

FUNCTION FS_DIAG_ACCESS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_ACCESS packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_access_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_access_req_type *request;
  efs2_diag_access_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_access_rsp_type);
  const char *request_path;

  ZAP ("_ACCESS");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_access_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_access_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_access_req_type,
      path, pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (ACCESS, request_path);

  response = (efs2_diag_access_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_ACCESS,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (efs_access (request_path, request->perm_bits) == 0)
    response->diag_errno  = 0;
  else
    response->diag_errno  = efs_errno;

  return response;
} /* END fs_diag_access_handler */


/*===========================================================================

FUNCTION FS_DIAG_DEVICE_INFO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_NAND_DEV_INFO packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_device_info_handler (VOID_PACKED_PTR req_ptr,
                             uint16 pkt_len)
{
  efs2_diag_dev_info_req_type *dev_info_req_ptr;
  efs2_diag_dev_info_rsp_type *dev_info_rsp_ptr = NULL;
  uint16 rsp_len = sizeof (efs2_diag_dev_info_rsp_type);
  struct fs_device_attr device_attr;

  ZAP ("_NAND_DEV_INFO");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_dev_info_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  dev_info_req_ptr = (efs2_diag_dev_info_req_type *)req_ptr;

  if (efs_get_device_info(&device_attr) == 0)
  {
    unsigned name_length = strlen (device_attr.device_name);
    rsp_len += name_length + 1;

    dev_info_rsp_ptr = (efs2_diag_dev_info_rsp_type *)
      diagpkt_subsys_alloc (FS_SUBSYS,
                            EFS2_DIAG_NAND_DEV_INFO,
                            rsp_len);

    if (dev_info_rsp_ptr == NULL)
      return NULL;

    if (device_attr.device_type == FS_DIAG_NAND_DEV)
    {
      dev_info_rsp_ptr->diag_errno = 0;
      dev_info_rsp_ptr->total_no_of_blocks = device_attr.block_count;
      dev_info_rsp_ptr->no_of_pages_per_block = device_attr.block_size;
      dev_info_rsp_ptr->page_size = device_attr.page_size;
      dev_info_rsp_ptr->total_page_size = device_attr.total_page_size;
      dev_info_rsp_ptr->maker_id = device_attr.device_maker_id;
      dev_info_rsp_ptr->device_id = device_attr.device_id;

      /* Yes, we're passing the source length in.  We computed it above
       * when we allocated the buffer, so we know there is room here.  A
       * regular 'strcpy' would be safe, but that is forbidden. */
      fs_strlcpy ((char*)dev_info_rsp_ptr->device_name,
          device_attr.device_name, name_length + 1);
    } else
      dev_info_rsp_ptr->diag_errno = FS_DIAG_NOT_NAND_FLASH;
  }
  else
  {
    fs_diag_efs2_error_rsp(-1, dev_info_req_ptr, pkt_len);
  }

  return dev_info_rsp_ptr;

} /* END fs_diag_device_info_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_START_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_START packet. This function put the phone
  in offline mode prepares for image dump to start.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_start_handler (VOID_PACKED_PTR req_ptr,
                                 uint16 pkt_len)
{
  efs2_diag_factimage_start_rsp_type *factimage_start_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_start_rsp_type);

  ZAP ("_FACT_IMAGE_START");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FACT_IMAGE_START);

  if (pkt_len != sizeof (efs2_diag_factimage_start_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  factimage_start_rsp_ptr = (efs2_diag_factimage_start_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_START,
                          rsp_len);

  if (factimage_start_rsp_ptr == NULL)
    return NULL;

  /* Acquire exclusive access during factory image output */
  FS_GLOBAL_LOCK();

  factimage_start_rsp_ptr->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, factimage_start_rsp_ptr, rsp_len);

  return factimage_start_rsp_ptr;

} /* END fs_diag_factimage_start_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_READ_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_READ packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_read_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  int image_data_errno;
  struct fs_factimage_read_info read_info;
  byte *page_data;

  efs2_diag_factimage_read_req_type *image_data_req_ptr;
  efs2_diag_factimage_read_rsp_type *image_data_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_read_rsp_type);

  ZAP ("_FACT_IMAGE_READ");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_factimage_read_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  image_data_req_ptr = (efs2_diag_factimage_read_req_type *)req_ptr;

  /* Filesystem page size is 512 bytes */
  rsp_len += 512;

  image_data_rsp_ptr = (efs2_diag_factimage_read_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_READ,
                          rsp_len);
  if (image_data_rsp_ptr == NULL)
    return NULL;

  read_info.stream_state       = image_data_req_ptr->stream_state;
  read_info.info_cluster_sent  = image_data_req_ptr->info_cluster_sent;
  read_info.cluster_map_seqno  = image_data_req_ptr->cluster_map_seqno;
  read_info.cluster_data_seqno = image_data_req_ptr->cluster_data_seqno;

  page_data = (byte *) image_data_rsp_ptr->page;

  image_data_errno = efs_get_fs_data (&read_info, page_data);

  image_data_rsp_ptr->diag_errno              = image_data_errno;
  image_data_rsp_ptr->stream_state       = read_info.stream_state;
  image_data_rsp_ptr->info_cluster_sent  = read_info.info_cluster_sent;
  image_data_rsp_ptr->cluster_map_seqno  = read_info.cluster_map_seqno;
  image_data_rsp_ptr->cluster_data_seqno = read_info.cluster_data_seqno;

  return image_data_rsp_ptr;

} /* END fs_diag_factimage_read_handler */


/*===========================================================================

FUNCTION FS_DIAG_FACTIMAGE_END_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_FACT_IMAGE_END packet. This function handles phone
  state after image dump.
============================================================================*/
VOID_PACKED_PTR
fs_diag_factimage_end_handler (VOID_PACKED_PTR req_ptr,
                               uint16 pkt_len)
{
  efs2_diag_factimage_end_rsp_type *factimage_end_rsp_ptr;
  uint16 rsp_len = sizeof (efs2_diag_factimage_end_rsp_type);

  ZAP ("_FACT_IMAGE_END");
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FACT_IMAGE_END);

  if (pkt_len != sizeof (efs2_diag_factimage_end_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  factimage_end_rsp_ptr = (efs2_diag_factimage_end_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FACT_IMAGE_END,
                          rsp_len);
  if (factimage_end_rsp_ptr == NULL)
    return NULL;

  /* Release exclusive access */
  FS_GLOBAL_UNLOCK();

  factimage_end_rsp_ptr->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, factimage_end_rsp_ptr, rsp_len);

  return factimage_end_rsp_ptr;

} /* END fs_diag_factimage_end_handler */

/*===========================================================================

FUNCTION FS_DIAG_PREP_FACTIMAGE_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_PREP_FACT_IMAGE packet. This function helps indicate
    to file system to prepare for a factory image dump.
============================================================================*/
VOID_PACKED_PTR
fs_diag_prep_factimage_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  int32 status = 1;
  qword now;
  qword start_time;
  qword elapsed;

  efs2_diag_prep_factimage_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_prep_factimage_rsp_type);

  ZAP ("_PREP_FACT_IMAGE");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (efs2_diag_prep_factimage_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_prep_factimage_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS, EFS2_DIAG_PREP_FACT_IMAGE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  fs_time_ms (start_time);

  while(status != 0)
  {
    status = efs_image_prepare ();
    if(status == 0)
      break;
    /* If elapsed time is too large (approx greater than 500 millisec)
       return */
    fs_time_ms (now);
    qw_sub (elapsed, now, start_time);
    if(qw_hi (elapsed) != 0 || qw_lo (elapsed) > 400)
      break;
  }

  response->diag_errno = status;

  return response;

} /* END fs_diag_prep_factimage_handler */


/*===========================================================================

FUNCTION FS_DIAG_GET_DEPRECATED_HANDLER
DESCRIPTION
  Handles the FS_DIAG_GET_DEPRECATED_HANDLER packet, which is a old way of
  EFS2_DIAG_GET with out any sequence number support.

============================================================================*/
VOID_PACKED_PTR
fs_diag_get_deprecated_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_get_deprecated_req_type *request;
  efs2_diag_get_deprecated_rsp_type *response;
  unsigned rsp_len = 0;
  unsigned base_length = 0;
  fs_ssize_t bytes_read = 0;
  const char *request_path;

  ZAP ("_GET_DEPRECATED");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_get_deprecated_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_get_deprecated_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_get_deprecated_req_type, path,
      pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);

  if (request->data_length > FS_DIAG_MAX_READ_REQ) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  base_length = offsetof (efs2_diag_get_deprecated_rsp_type, data);
  rsp_len = base_length + request->data_length;

  response = (efs2_diag_get_deprecated_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET_DEPRECATED,
                          rsp_len);
  if (response == NULL)
    return NULL;

  bytes_read = efs_get (request_path,
                        (void *) response->data,
                        request->data_length);

  if (bytes_read >= 0) {
    response->num_bytes  = bytes_read;
    response->diag_errno      = 0;

    if ((unsigned) bytes_read < request->data_length)
      diagpkt_shorten (response, base_length + bytes_read);
  } else {
    response->num_bytes  = -1;
    response->diag_errno      = efs_errno;
    diagpkt_shorten (response, base_length);
  }

  return response;
} /* END fs_diag_get_deprecated_handler */


/*===========================================================================

FUNCTION FS_DIAG_PUT_DEPRECATED_HANDLER
DESCRIPTION
  Handles the FS_DIAG_PUT_DEPRECATED_HANDLER packet, which is a old way of
  EFS2_DIAG_PUT with out any sequence number support.

============================================================================*/
VOID_PACKED_PTR
fs_diag_put_deprecated_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_put_deprecated_req_type *request;
  efs2_diag_put_deprecated_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_put_deprecated_rsp_type);
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  unsigned   data_offset;
  char      *put_locn;
  char      *put_path;

  ZAP ("_PUT_DEPRECATED");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_put_deprecated_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_put_deprecated_req_type *) req_ptr;

  put_locn = (char *) request->data;
  nbyte    = request->data_length;
  put_path = (char *) request->data + nbyte;
  data_offset = offsetof (efs2_diag_put_deprecated_req_type, data);

  /* Validate this packet, more complicated than others, because there is a
   * null-terminated string after variable length data. */
  if ((nbyte + data_offset >= pkt_len) ||
      (fsd_count_nulls (put_path, pkt_len - (data_offset + nbyte)) < 1))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len));
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, put_path);

  response = (efs2_diag_put_deprecated_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_PUT_DEPRECATED,
                          rsp_len);
  if (response == NULL)
    return NULL;

  bytes_written = efs_put (put_path,
                           (void *) put_locn,
                           nbyte, request->flags,
                           0777);

  if (bytes_written < 0)
  {
    response->diag_errno         = efs_errno;
  }
  else
  {
    /* (bytes_written >= 0) when it gets here */
    response->diag_errno         = 0;
  }

  return response;
} /* END fs_diag_put_deprecated_handler */


/*===========================================================================

FUNCTION FS_DIAG_GET_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_GET packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
fs_diag_get_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_get_req_type *request;
  efs2_diag_get_rsp_type *response;
  unsigned rsp_len = 0;
  unsigned base_length = 0;
  fs_ssize_t bytes_read = 0;
  const char *request_path;
  uint16 seq_num;

  ZAP ("_GET");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (efs2_diag_get_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_get_req_type *) req_ptr;
  request_path = (const char *) request->path;
  /*
   * Fetch the Sequence number commonly used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  FSD_CHECK_NULLS (request, efs2_diag_get_req_type, path,
      pkt_len, 1);

  FS_DIAG_VALIDATE_PATH_ACCESS (READ, request_path);

  if (request->data_length > FS_DIAG_MAX_READ_REQ) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  base_length = FPOS (efs2_diag_get_rsp_type, data);
  rsp_len = base_length + request->data_length;

  response = (efs2_diag_get_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;
  bytes_read = efs_get (request_path,
                        (void *) response->data,
                        request->data_length);

  if (bytes_read >= 0) {
    response->num_bytes  = bytes_read;
    response->diag_errno      = 0;

    if ((unsigned) bytes_read < request->data_length)
      diagpkt_shorten (response, base_length + bytes_read);
  } else {
    response->num_bytes  = -1;
    response->diag_errno      = efs_errno;
    diagpkt_shorten (response, base_length);
  }

  return response;
} /* END fs_diag_get_handler */


/*===========================================================================

FUNCTION FS_DIAG_PUT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_PUT packet. The field sequence_number is used for the
  purpose of Synchronization / Correlation purpose. The sequence_number
  received is fetched out of request packet and tucked back into the response
  packet. This would benefit the application with synchronized requests and
  responses.

============================================================================*/
VOID_PACKED_PTR
fs_diag_put_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_put_req_type *request;
  efs2_diag_put_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_put_rsp_type);
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  unsigned   data_offset;
  char      *put_locn;
  char      *put_path;
  uint16 seq_num;

  ZAP ("_PUT");
  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (efs2_diag_put_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_put_req_type *) req_ptr;

  put_locn = (char *) request->data;
  nbyte    = request->data_length;
  put_path = (char *) request->data + nbyte;
  data_offset = FPOS (efs2_diag_put_req_type, data);
  /*
   * Fetch the Sequence number usually used by the calling application
   * for Synchronization / Correlation purpose.
   */
  seq_num = request->sequence_number;

  /* Validate this packet, more complicated than others, because there is a
   * null-terminated string after variable length data. */
  if ((nbyte + data_offset >= pkt_len) ||
      (fsd_count_nulls (put_path, pkt_len - (data_offset + nbyte)) < 1))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len));
  }

  FS_DIAG_VALIDATE_PATH_ACCESS (WRITE, put_path);

  response = (efs2_diag_put_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_PUT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = seq_num;

  bytes_written = efs_put (put_path,
                           (void *) put_locn,
                           nbyte, request->flags,
                           0777);
  if (bytes_written < 0)
  {
    response->diag_errno         = efs_errno;
  }
  else
  {
    /* (bytes_written >= 0) when it gets here */
    response->diag_errno         = 0;
  }
  return response;
} /* END fs_diag_put_handler */

VOID_PACKED_PTR
fs_diag_efs2_error_rsp (int32 errno, VOID_PACKED_PTR req_pkt,
                        uint16 req_len)
{
  efs2_diag_error_rsp_type *response;
  unsigned int rsp_len;

  /* Lint is too stupid to realize that offsetof() has no side
   * effects, so it prohibits the use of the MIN() macro here (error
   * 666).  So we spell it out using small words instead. */
  rsp_len = req_len + offsetof (efs2_diag_error_rsp_type, pkt);
  if (rsp_len > sizeof (efs2_diag_error_rsp_type))
    rsp_len = sizeof (efs2_diag_error_rsp_type);

  response = (efs2_diag_error_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_ERROR,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = errno;

  memcpy ((void *) response->pkt,
          (void *) req_pkt,
          rsp_len - offsetof (efs2_diag_error_rsp_type, pkt));

  return ((void *) response);
}/* diagkt_err_rsp */

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_START_TEST_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_START_TEST packet. Initializes the EFS2 Diag
  interface and passes the values of some parameters back to the tool.

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_start_test_handler (VOID_PACKED_PTR req_ptr,
                                      uint16 pkt_len)
{
  efs2_diag_benchmark_start_test_rsp_type *response;
  efs2_diag_benchmark_start_test_req_type *request =
    (efs2_diag_benchmark_start_test_req_type *) req_ptr;
  char temp_path[FS_PATH_MAX];
  const char *request_path;

  fsd_invalidate_memoization ();

  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    ZPRINTF("Unexpected start_test packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  /* RVCT won't allow a packed string into strlcpy */
  request_path = (const char *) request->cmd.path;
  fs_strlcpy (temp_path, request_path, sizeof(temp_path));

  /* Make sure they're not benchmarking a forbidden pathname */
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, temp_path);

  response = (efs2_diag_benchmark_start_test_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_START_TEST,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->cmd = request->cmd;
  response->success = fs_benchmark_put_cmd (&request->cmd) ? 0 : 1;

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_GET_RESULTS_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_GET_RESULTS packet. Polls about the response
  queue from fs_benchmark().

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_get_results_handler (VOID_PACKED_PTR req_ptr,
                                       uint16 pkt_len)
{
  efs2_diag_benchmark_get_results_rsp_type *response;

  fsd_invalidate_memoization ();

  if (pkt_len != sizeof (efs2_diag_benchmark_get_results_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_benchmark_get_results_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_GET_RESULTS,
                          sizeof(*response));
  if (response == NULL)
    return NULL;

  response->queue_status = fs_benchmark_get_rpt (&response->report);

  return response;
}

/*===========================================================================

FUNCTION FS_DIAG_BENCHMARK_INIT_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_BENCHMARK_INIT packet. Resets the command and report
  queues. Needs further modification in fs_benchmark_reset_all() for proper
  functioning

============================================================================*/
VOID_PACKED_PTR
fs_diag_benchmark_init_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_benchmark_init_rsp_type *response;

  fsd_invalidate_memoization ();

  if (pkt_len != sizeof (efs2_diag_benchmark_init_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (efs2_diag_benchmark_init_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_BENCHMARK_INIT,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  fs_benchmark_flush();

  /*
    Future implementation may address aborting a pending/running
    command.  The queues are emptied explicitly, but nothing is done
    to abort a command that got off the queue and is running Right
    Now.  That response will still appear after this.
  */
  return response;
}

VOID_PACKED_PTR
fs_diag_chown_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_chown_req_type *request = (efs2_diag_chown_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();

  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    ZPRINTF("Unexpected packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_chown_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;

  /* Make sure we're not chowning a forbidden pathname */
  FS_DIAG_VALIDATE_PATH_ACCESS (OPEN, request_path);

  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_CHOWN,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->diag_errno = (efs_chown (request_path,
                                request->uid_val,
                                request->gid_val) ? efs_errno : 0);

  return response;
}

VOID_PACKED_PTR
fs_diag_set_reservation_handler (VOID_PACKED_PTR req_ptr,
                                 uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_set_reservation_req_type *request =
    (efs2_diag_set_reservation_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    ZPRINTF("Unexpected packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_set_reservation_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;
  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_RESERVATION,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->diag_errno = (efs_set_reservation (request_path,
                                          request->gid,
                                          request->bytes) ? efs_errno : 0);

  return response;
}


VOID_PACKED_PTR
fs_diag_set_quota_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_errno_rsp_type *response;
  efs2_diag_set_quota_req_type *request =
    (efs2_diag_set_quota_req_type *) req_ptr;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    ZPRINTF("Unexpected packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_set_quota_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;
  response = (efs2_diag_errno_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_QUOTA,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->diag_errno = (efs_set_quota (request_path,
                                    request->gid,
                                    request->bytes) ? efs_errno : 0);

  return response;
}

VOID_PACKED_PTR
fs_diag_get_group_info_handler (VOID_PACKED_PTR req_ptr,
                                uint16 pkt_len)
{
  efs2_diag_get_group_info_rsp_type *response;
  efs2_diag_get_group_info_req_type *request =
    (efs2_diag_get_group_info_req_type *) req_ptr;
  struct fs_group_info info;
  const char *request_path;

  fsd_invalidate_memoization ();
  /* Require SPC Unlock */
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  if (pkt_len < sizeof (*request))
  {
    ZPRINTF("Unexpected packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  FSD_CHECK_NULLS (request, efs2_diag_get_group_info_req_type,
      path, pkt_len, 1);

  request_path = (const char *) request->path;
  response = (efs2_diag_get_group_info_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_GET_GROUP_INFO,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  response->diag_errno = (efs_get_group_info (request_path,
                                         request->gid,
                                         &info) ? efs_errno : 0);

  response->info.quota_size = info.quota_size;
  response->info.reservation_size = info.reservation_size;
  response->info.space_used = info.space_used;
  return response;
}

VOID_PACKED_PTR
fs_diag_deltree_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_deltree_req_type *request;
  efs2_diag_deltree_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_deltree_rsp_type);
  const char *request_path;
  const char *valid_path = "/public/";

  ZAP ("_DELTREE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }
  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_DELTREE);

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_deltree_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_deltree_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_deltree_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_deltree_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_DELTREE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /*
   * Check for "/public/" to be the root directory. None other than
   * /public is permitted to be deleted in EFS.
   */
  if ((strncmp (request_path, valid_path, strlen (valid_path)) == 0)
      && (strlen (request_path) > strlen (valid_path)))
  {
    /* Good to deltree the path. */
    if (efs_deltree (request_path) != 0)
      response->diag_errno = efs_errno;
    else
      response->diag_errno = 0;
  }
  else
  {
    /* Does not match with "/public/" Return Error */
    response->diag_errno = EPERM;
  }
  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_deltree_handler */

VOID_PACKED_PTR
fs_diag_truncate_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_truncate_req_type *request;
  efs2_diag_truncate_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_truncate_rsp_type);
  const char *request_path;

  ZAP ("_TRUNCATE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_TRUNCATE);

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_truncate_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_truncate_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_truncate_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_truncate_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_TRUNCATE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Good to truncate the path. */
  if (efs_truncate (request_path, request->length) != 0)
    response->diag_errno = efs_errno;
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;
} /* END fs_diag_truncate_handler */

VOID_PACKED_PTR
fs_diag_ftruncate_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_ftruncate_req_type *request;
  efs2_diag_ftruncate_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_ftruncate_rsp_type);
  desc_entry *slot;

  ZAP ("_FTRUNCATE");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }
  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_FTRUNCATE);

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_ftruncate_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_ftruncate_req_type *) req_ptr;

  response = (efs2_diag_ftruncate_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_FTRUNCATE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* Validate the requested descriptor. */
  if (request->fildes < 0 || request->fildes >= FS_DIAG_MAX_FDS ||
      fsd_desc_table[request->fildes].fd < 0)
  {
    response->diag_errno = EBADF;
    goto ftruncate_end;
  }

  slot = &fsd_desc_table[request->fildes];
  /* Good to ftruncate the path. */
  if (efs_ftruncate (slot->fd, request->length) != 0)
    response->diag_errno = efs_errno;
  else
    response->diag_errno = 0;

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  ftruncate_end:
  return response;
} /* END fs_diag_ftruncate_handler */


/*===========================================================================

FUNCTION FS_DIAG_STATVFS_V2_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_STATVFS_V2 packet.
============================================================================*/
VOID_PACKED_PTR
fs_diag_statvfs_v2_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_statvfs_v2_req_type *request;
  efs2_diag_statvfs_v2_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_statvfs_v2_rsp_type);
  struct fs_statvfs sbuf;
  const char *request_path;

  fsd_invalidate_memoization ();
  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len < sizeof (efs2_diag_statvfs_v2_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_statvfs_v2_req_type *) req_ptr;
  request_path = (const char *) request->path;

  FSD_CHECK_NULLS (request, efs2_diag_statvfs_v2_req_type,
      path, pkt_len, 1);

  ZPRINTF ("_STATVFS_V2: %s\n\r", request_path);
  FS_DIAG_VALIDATE_PATH_ACCESS (STATFS, request_path);

  response = (efs2_diag_statvfs_v2_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_STATVFS_V2,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  if (efs_statvfs (request_path, &sbuf) == 0)
  {
    /* Fill in packet with returned values */
    response->diag_errno         = 0;
    response->fs_id         = sbuf.f_fsid;
    memset ((void *)response->fs_type, 0, sizeof(response->fs_type));
    response->block_size    = sbuf.f_bsize;
    response->total_blocks  = sbuf.f_blocks;
    response->avail_blocks  = sbuf.f_bavail;
    response->free_blocks   = sbuf.f_bfree;
    response->max_file_size = sbuf.f_max_file_size;
    response->nfiles        = 0;
    response->max_nfiles    = 0;

    response->f_maxwrite           = sbuf.f_maxwrite;
    response->f_pathmax            = sbuf.f_pathmax;
    response->f_is_case_sensitive  = sbuf.f_is_case_sensitive;
    response->f_is_case_preserving = sbuf.f_is_case_preserving;
    response->f_max_file_size      = sbuf.f_max_file_size;
    response->f_max_file_size_unit = sbuf.f_max_file_size_unit;
    response->f_max_open_files     = sbuf.f_max_open_files;
    response->f_name_rule          = sbuf.f_name_rule;
    response->f_name_encoding      = sbuf.f_name_encoding;

    response->diag_errno = 0;
  }
  else
  {
    response->diag_errno  = efs_errno;
  }

  return response;
} /* END fs_diag_statvfs_v2_handler */


VOID_PACKED_PTR
ext_simple_query (efs2_diag_extended_info_req_type *request, unsigned len);

/*======================================================================
 * FUNCTION FS_DIAG_EXTENDED_INFO_HANDLER
 * Handle the extended info request packet.
 */
VOID_PACKED_PTR
fs_diag_extended_info_handler (VOID_PACKED_PTR req_ptr,
                               uint16 pkt_len)
{
  efs2_diag_extended_info_req_type *request;

  fsd_invalidate_memoization ();

  /* Check security. */
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED) {
    return diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len);
  }

  if (pkt_len < sizeof (*request)) {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  request = (efs2_diag_extended_info_req_type *) req_ptr;

  /* Validate nul termination of path. */
  FSD_CHECK_NULLS (request, efs2_diag_extended_info_req_type,
      path, pkt_len, 1);

  /* XXX: Most queries are made through the existing efs_get_device_info
   * request, which doesn't take a path.  When multiple device support is
   * added to EFS2, this mechanism will have to change. */
  switch (request->query) {
    case FS_DIAG_EXT_QUERY_DEVICE_NAME:
    case FS_DIAG_EXT_QUERY_DEVICE_ID:
    case FS_DIAG_EXT_QUERY_FILESYSTEM_BLOCKS:
    case FS_DIAG_EXT_QUERY_PAGES_PER_BLOCK:
    case FS_DIAG_EXT_QUERY_PAGE_SIZE:
    case FS_DIAG_EXT_QUERY_FLASH_BUS_WIDTH:
    case FS_DIAG_EXT_QUERY_TOTAL_BLOCKS:
      return ext_simple_query (request, pkt_len);

    default:
      return diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len);
  }
}

VOID_PACKED_PTR
ext_simple_query (efs2_diag_extended_info_req_type *request, unsigned len)
{
  int result;
  unsigned rsp_len;
  efs2_diag_extended_info_rsp_type *response = NULL;
  PACKED uint32 * PACKED_POST item = NULL;
  static struct fs_device_attr attr;
  unsigned dev_name_length;

  result = efs_get_device_info (&attr);
  if (result != 0)
    return fs_diag_efs2_error_rsp (efs_errno, (VOID_PACKED_PTR)
                                   request, len);

  if (request->query == FS_DIAG_EXT_QUERY_DEVICE_ID
      && request->data > 1)
    return fs_diag_efs2_error_rsp (FS_DIAG_UNAVAILABLE_INFO,
                                   (VOID_PACKED_PTR) request, len);

  rsp_len = sizeof (*response) + sizeof (uint32);

  dev_name_length = strlen (attr.device_name);
  if (request->query == FS_DIAG_EXT_QUERY_DEVICE_NAME)
    rsp_len = sizeof (*response) + dev_name_length + 1;

  response = (efs2_diag_extended_info_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_EXTENDED_INFO, rsp_len);
  if (response == NULL)
    return NULL;

  item = (PACKED uint32 * PACKED_POST)
    ((void *)(response + 1)); /* lint: hop over void */
  response->kind = FS_DIAG_EXT_KIND_INT;

  switch (request->query) {
    case FS_DIAG_EXT_QUERY_DEVICE_ID:
      if (request->data == 0)
        *item = attr.device_maker_id;
      else
        *item = attr.device_id;
      break;

    case FS_DIAG_EXT_QUERY_FILESYSTEM_BLOCKS:
      *item = attr.block_count;
      break;

    case FS_DIAG_EXT_QUERY_PAGES_PER_BLOCK:
      *item = attr.block_size;
      break;

    case FS_DIAG_EXT_QUERY_PAGE_SIZE:
      *item = attr.page_size;
      break;

    case FS_DIAG_EXT_QUERY_TOTAL_BLOCKS:
      *item = attr.total_blocks;
      break;

    case FS_DIAG_EXT_QUERY_FLASH_BUS_WIDTH:
      *item = attr.bus_width;
      break;

    case FS_DIAG_EXT_QUERY_DEVICE_NAME:
      response->kind = FS_DIAG_EXT_KIND_STRING;
      fs_strlcpy ((char *) (response + 1), attr.device_name,
          dev_name_length + 1);
      break;

    default:
      /* Something is wrong. */
      MSG_HIGH ("Internal error in ext_num_query", 0, 0, 0);
      *item = 0xFFFFFFFF;
  }

  return response;
}

VOID_PACKED_PTR
fs_diag_md5sum_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  efs2_diag_md5sum_req_type *request;
  efs2_diag_md5sum_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_md5sum_rsp_type);
  const char *request_path;

  ZAP ("_MD5SUM");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_md5sum_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_md5sum_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_md5sum_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_md5sum_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_MD5SUM,
                          rsp_len);

  if (response)
  {
    /* Acknowledge back with the Sequence number */
    response->sequence_number = request->sequence_number;

    /* md5 hash calculation */
    if (efs_md5sum (request_path, &response->hash_result) != 0)
      response->diag_errno = efs_errno;
    else
      response->diag_errno = 0;
  }

  return response;
} /* END fs_diag_md5sum_handler */


/*===========================================================================

FUNCTION   FS_DIAG_HOTPLUG_FORMAT_HANDLER
DESCRIPTION
   Handles the request packet for hotplug format.

   This function receives a request packet to format a storage
   media. It returns a packet indicating if successful or if
   an error occurred.

PARAMETERS
   req_ptr  pointer to request packet
   pkt_len  received packet length

RETURN
 Pointer to response packet. If pointer is NULL packet could not be created.
 On success the packet should contain a zero in the error field. If format did
 not occurred because there is no device or hotplug_format failed the error
 code ENODEV is returned.

===========================================================================*/

VOID_PACKED_PTR
fs_diag_hotplug_format_handler (VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_hotplug_format_req_type *request;
  efs2_diag_hotplug_format_rsp_type *response;
  uint16 rsp_len = sizeof (efs2_diag_hotplug_format_rsp_type);
  const char *request_path;

  ZAP ("_HOTPLUG_FORMAT");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }
  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_HOTPLUG_FORMAT);

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_hotplug_format_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_hotplug_format_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_hotplug_format_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_hotplug_format_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_HOTPLUG_FORMAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  if (hotplug_format (request_path)){ /* 0 if it formats it, -1 if error */
    response->diag_errno = ENODEV; // no such device or error.
  }
  else{
    response->diag_errno = 0;
  }

  fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return (response);
}/* End of diag_hotplug_format_handler */

VOID_PACKED_PTR
fs_diag_shred_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  efs2_diag_shred_req_type *request;
  efs2_diag_shred_rsp_type *response;
  const char *request_path;
  uint16 rsp_len = sizeof (efs2_diag_shred_rsp_type);

  ZAP ("_SHRED");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state () != DIAG_SEC_UNLOCKED)
  {
    return (diagpkt_err_rsp (DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }
  FSD_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, EFS2_DIAG_SHRED);

  /*
   * Check for minimum length of packet
   */
  if (pkt_len < sizeof (efs2_diag_shred_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (efs2_diag_shred_req_type *) req_ptr;
  request_path = LONG_STRING_CAST (request->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request, efs2_diag_shred_req_type,
                   path, pkt_len, 1);

  response = (efs2_diag_shred_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SHRED,
                          rsp_len);

  if (response)
  {
    /* Acknowledge back with the Sequence number */
    response->sequence_number = request->sequence_number;

    response->shred_result = efs_shred (request_path);
    if (response->shred_result < 0)
      response->diag_errno = -response->shred_result;
    else
      response->diag_errno = 0;
    fsd_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  }

  return response;
} /* END fs_diag_shred_handler */

VOID_PACKED_PTR
fs_diag_set_idle_dev_evt_dur_handler (
    VOID_PACKED_PTR req_ptr, uint16 pkt_len)
{
  efs2_diag_set_idle_dev_evt_dur_rsp_type *response;
  efs2_diag_set_idle_dev_evt_dur_req_type *request =
    (efs2_diag_set_idle_dev_evt_dur_req_type *) req_ptr;

  fsd_invalidate_memoization ();

  ZAP ("_IDLE_DEVICE_EVENT_DURATION");
  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return (diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len));
  }

  /*
   * Check for the length of packet
   */
  if (pkt_len != sizeof (*request))
  {
    ZPRINTF("Unexpected packet length = %d, SIZEOF = %d",
            pkt_len, sizeof (*request));
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  response = (efs2_diag_set_idle_dev_evt_dur_rsp_type *)
    diagpkt_subsys_alloc (FS_SUBSYS,
                          EFS2_DIAG_SET_IDLE_DEV_EVT_DUR,
                          sizeof (*response));
  if (response == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response->sequence_number = request->sequence_number;

  /* call the hotplug_set_idle_device_event_dur api to set event_dur value. */
  hotplug_set_idle_device_event_duration (request->event_dur_minutes);
  response->diag_errno = 0;

  return response;
} /* END fs_diag_set_idle_dev_evt_dur_handler. */

/*===========================================================================

FUNCTION FS_DIAG_HOTPLUG_DEVICE_INFO_HANDLER
DESCRIPTION
  Handles the EFS2_DIAG_HOTPLUG_DEVICE_INFO packet.
  This command is used to get the device info maintained by hotplug module
  referred by the given path string.
============================================================================*/
VOID_PACKED_PTR
fs_diag_hotplug_device_info_handler (VOID_PACKED_PTR req_ptr,
                                     uint16 pkt_len)
{
  efs2_diag_hotplug_device_info_req_type *request_ptr;
  efs2_diag_hotplug_device_info_rsp_type *response_ptr = NULL;

  struct hotplug_device_info hdev_info;
  const char *request_path;

  uint16 rsp_len = sizeof (efs2_diag_hotplug_device_info_rsp_type);

  ZAP ("_HOTPLUG_DEVICE_INFO");

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED)
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );

  if (pkt_len < sizeof (efs2_diag_hotplug_device_info_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request_ptr = (efs2_diag_hotplug_device_info_req_type *)req_ptr;
  request_path = LONG_STRING_CAST (request_ptr->path);

  /* Check for a NULL char */
  FSD_CHECK_NULLS (request_ptr, efs2_diag_hotplug_device_info_req_type,
                   path, pkt_len, 1);

  /* Set all the members of struct hdev_info with 0. */
  memset (&hdev_info, 0, sizeof (hdev_info));

  response_ptr = (efs2_diag_hotplug_device_info_rsp_type *)
                    diagpkt_subsys_alloc (FS_SUBSYS,
                                          EFS2_DIAG_HOTPLUG_DEVICE_INFO,
                                          rsp_len);

  if (response_ptr == NULL)
    return NULL;

  /* Acknowledge back with the Sequence number */
  response_ptr->sequence_number = request_ptr->sequence_number;

  /* call the hotplug api to populate the struct hotplug_device_info. */
  if (hotplug_dev_get_device_info (hotplug_find_dev_by_path (request_path),
                                   &hdev_info) == 0)
  {
    response_ptr->diag_errno      = 0;
    /* populate the response_ptr struct from the
     * struct hotplug_device_info. */
    response_ptr->manufacturer_id      = hdev_info.manufacturer_id;
    response_ptr->oem_id               = hdev_info.oem_id;
    response_ptr->prod_rev             = hdev_info.prod_rev;
    response_ptr->prod_serial_num      = hdev_info.prod_serial_num;

    fs_strlcpy ((char*) response_ptr->product_name,
                (const char *) hdev_info.product_name,
                 sizeof (hdev_info.product_name));
    fs_strlcpy ((char*) response_ptr->manufactured_date,
                (const char *) hdev_info.manufactured_date,
                 sizeof (hdev_info.manufactured_date));
  }
  else
  {
    response_ptr->diag_errno = -1;
    fs_diag_efs2_error_rsp (response_ptr->diag_errno, request_ptr, pkt_len);
  }

  return response_ptr;
} /* END fs_diag_hotplug_device_info_handler */

#endif /* FEATURE_EFS2 */
