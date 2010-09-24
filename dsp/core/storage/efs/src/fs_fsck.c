/***********************************************************************
 * fs_fsck.c
 *
 * File system sanity check code.
 * Copyright (C) 2006-2008 QUALCOMM, Inc.
 *
 * Code for checking sanity of the filesystem.  Generally used in
 * off-target tests.
 *
 * This test makes use of dynamic memory allocation.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fsck.c#2 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-11-03   rp    Introduce device abstraction.
2007-10-18   rp    Featurized QEFS2.
2006-09-20   dlb   Lint cleanup.
2006-04-13   dlb   Create

===========================================================================*/

#include "fs_efs2_cust.h"


#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */
