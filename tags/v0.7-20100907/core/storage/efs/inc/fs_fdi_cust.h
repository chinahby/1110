/***********************************************************************
 * fs_fdi_cust.h
 *
 * Configure the FDI file-system.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * Configure the features of the FDI file system.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_fdi_cust.h#3 $ $DateTime: 2008/07/31 16:37:17 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-07-28   rp    Use FDI's default #defines.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-01-01   rp    Create

===========================================================================*/
#ifndef __FS_FDI_CUST_H__
#define __FS_FDI_CUST_H__

#include "fs_config.h"
#include "assert.h"


#ifdef FEATURE_EFS_FDI

/* Run FDI for either target or simulator. */
#ifdef FS_UNIT_TEST
#error code not present
#else
  #define FFS_QUALCOMM_TARGET
#endif


/* The max number of files that can be kept opened simultaneously in FDI.
 * The extra 1 is for the symlinks in FDI. In FDI when we create a
 * symlink, a file by the name of symlink is created and the link it
 * points to is written as file-data inside the new symlink file. */
#define FDI_MAX_OPEN_FILES            (FS_MAX_DESCRIPTORS + 1)

/* Set size limits for Item files. */
#define   MAX_SMALLFILE_NAME_SIZE     (FS_SHORTNAME_THRESHOLD-1)
#define   MAX_SMALLFILE_DATA_SIZE     FS_ITEM_MAX
#define   MAX_SMALLFILE_ENTRY_SIZE    256

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

#define FDI_ASSERT(condn)             ASSERT (condn)

/* For the simulator turn ON FDI Debugging. */
#ifdef FS_UNIT_TEST
#error code not present
#endif

#ifdef FDI_DEBUGGING
  #define FDI_DBG_ASSERT(condn)       ASSERT (condn)
#else
  #define FDI_DBG_ASSERT(condn)
#endif

#else
  extern int __dont_complain_about_empty_file;

  #ifdef FS_UNIT_TEST
#error code not present
  #endif

#endif /* FEATURE_EFS_FDI */

#endif /* not __FS_FDI_CUST_H__ */
