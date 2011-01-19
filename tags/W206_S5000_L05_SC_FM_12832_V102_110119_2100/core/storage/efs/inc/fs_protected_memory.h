/***********************************************************************
 * fs_protected_memory.h
 *
 * Section markers for EFS protected memory.
 *
 * Copyright (C) 2007-2009 QUALCOMM, Inc.
 *
 * Files that contain private memory include this header to mark the
 * sections appropriately.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_protected_memory.h#5 $ $DateTime: 2009/05/21 18:37:02 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-21   sh    Changed featurizing to exclude gcc.
2009-05-18   rp    Featurized for ARM compiler.
2008-10-17   yog   Featurized pragma directive to exclude for FS_STANDALONE.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-05   umr   Create

===========================================================================*/

#ifndef __FS_PROTECTED_MEMORY_H__
#define __FS_PROTECTED_MEMORY_H__

/* Use pragma directive for ARM, non-GCC, full AMSS builds */
#if defined __arm && !defined FS_STANDALONE && !defined __GNUC__
  #pragma arm section rwdata = "efs_rw", rodata = "efs_ro", zidata = "efs_zi"
#endif

#endif /* not __FS_PROTECTED_MEMORY_H__ */
