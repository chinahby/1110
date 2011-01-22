/***********************************************************************
 * fs_fdi.h
 *
 * General FDI functions.
 * Copyright (C) 2008 QUALCOMM, Inc.
 *
 * This file defines the general FDI functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/inc/fs_fdi.h#2 $ $DateTime: 2008/06/03 15:15:30 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-05-23   rp    Create

===========================================================================*/
#ifndef __FS_FDI_H__
#define __FS_FDI_H__

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_sys_types.h"

/* Tell TMC-task that we have a fdi_bg_task() functon in the FDI layer. */
extern void fdi_bg_task (dword parm);

/* This function will convert an FDI error code to a matching EFS error code.*/
int fs_fdi_conv_fdi_to_efs_err_code(uint32 fdi_err);

#endif /* FEATURE_EFS_FDI */

#endif /* not __FS_FDI_H__ */
