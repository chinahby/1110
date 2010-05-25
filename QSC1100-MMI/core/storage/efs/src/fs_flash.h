/***********************************************************************
 * fs_flash.h
 *
 * This is the EFS wrapper for the flash interface.
 * Copyright (C) 2009 QUALCOMM, Inc.
 *
 * This is the only file needed by any EFS client using flash. It will
 * ensure that the approriate interface gets used on DAL and non-DAL
 * targets.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_flash.h#1 $ $DateTime: 2009/01/29 17:39:06 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-01-23   ebb   Create

===========================================================================*/

#ifndef __FS_FLASH_H__
#define __FS_FLASH_H__

/* Always include flash.h. This will point to the old flash interface on
 * non-DAL targets and point to the DAL interface on newer targets. It will
 * also define FLASH_USES_DAL on DAL targets */
#include "flash.h"

/* If flash uses DAL we must go through the shim layer until we completely
 * move to the DAL interface */
#ifdef FLASH_USES_DAL
  #include "fs_flash_dal_shim.h"
#endif

#endif /* not __FS_FLASH_H__ */
