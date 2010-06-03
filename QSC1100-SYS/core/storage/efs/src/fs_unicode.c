/***********************************************************************
 * fs_unicode.c
 *
 * Code common to Unicode conversion
 * Copyright (C) 2007 QUALCOMM, Inc.
 *
 * These functions are not specific to the particular encoding.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_unicode.c#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-05-25   umr   Added support for EFS memory protection feature.
2007-02-07   sh    Define a default conversion.
2007-01-05   sh    Create

===========================================================================*/

#include "fs_unicode.h"
#include "fs_protected_memory.h"

/* This is a global reference to the active set of conversion functions. */
struct unicode_conversion_ops *fs_convert = &EFS_DEFAULT_UNICODE_CONVERSION;
