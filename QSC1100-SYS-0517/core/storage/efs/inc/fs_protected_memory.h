/***********************************************************************
 * fs_protected_memory.h
 *
 * Short description
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_protected_memory.h#3 $ $DateTime: 2008/10/29 18:03:15 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-10-17   yog   Featurized pragma directive to exclude for FS_STANDALONE.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2007-07-05   umr   Create

===========================================================================*/

#ifndef __FS_PROTECTED_MEMORY_H__
#define __FS_PROTECTED_MEMORY_H__

/* Exclude pragma directive for both FS_STANDALONE and FS_UNIT_TEST. */
#if !defined (FS_UNIT_TEST) && !defined (FS_STANDALONE)
  #pragma arm section rwdata = "efs_rw", rodata = "efs_ro", zidata = "efs_zi"
#endif

#endif /* not __FS_PROTECTED_MEMORY_H__ */
