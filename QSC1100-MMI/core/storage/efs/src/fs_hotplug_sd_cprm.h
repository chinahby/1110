/***********************************************************************
 * fs_hotplug_sd_cprm.h
 *
 * Copyright (C) 2006 QUALCOMM, Inc.
 *
 * This is not a stand-alone header file.
 * It is used to communicate the exported device functions to hotplug.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_hotplug_sd_cprm.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2006-11-12   s h   Cleanup
2006-09-28   sep   Created.

===========================================================================*/

#ifndef __FS_HOTPLUG_SD_CPRM_H__
#define __FS_HOTPLUG_SD_CPRM_H__

/* Structure of CPRM function pointers to pass to the file system */

extern struct hotplug_dev_funcs hotplug_sd_cprm_functions;

#endif  /* __FS_HOTPLUG_SD_CPRM_H__ */
