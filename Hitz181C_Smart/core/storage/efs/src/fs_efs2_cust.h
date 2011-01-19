/***********************************************************************
 * fs_efs2_cust.h
 *
 * Configure the EFS2 file system.
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * This is customer.h equivalent for the QEFS2 related files.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_efs2_cust.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-10-18   rp    Create

===========================================================================*/

#ifndef __FS_EFS2_CUST_H__
#define __FS_EFS2_CUST_H__

#include "fs_config.h"

#ifndef FEATURE_EFS_QEFS2

/* Make sure FS_PARANOIA_CHECKS is not enabled. */
#ifdef FS_PARANOIA_CHECKS
  #error "FS_PARANOIA_CHECKS is available only for QEFS2"
#endif

extern int __dont_complain_about_empty_file;

#endif /* not FEATURE_EFS_QEFS2 */


#endif /* not __FS_EFS2_CUST_H__ */
