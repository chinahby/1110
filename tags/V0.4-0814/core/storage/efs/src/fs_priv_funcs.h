/***********************************************************************
 * fs_priv_funcs.h
 *
 * Short description
 * Copyright (C) 2009 QUALCOMM, Inc.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_priv_funcs.h#1 $ $DateTime: 2009/05/20 16:03:23 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-05-18   rp    Create

===========================================================================*/

#ifndef __FS_PRIV_FUNCS_H__
#define __FS_PRIV_FUNCS_H__

#include "fs_config.h"
#include "fs_sys_types.h"


/***********************************************************************
FUNCTION      efs_get_fs_data

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  If successful, returns 0.
              Otherwise, returns -1, and sets errno to indicate the error.

SIDE EFFECTS  None
**********************************************************************/
int efs_get_fs_data (struct fs_factimage_read_info * image_info_ptr,
                     byte* page_buffer);

/***********************************************************************
 * FUNCTION      efs_get_device_info
 *
 * DESCRIPTION   This function will get the attributes of the flash device
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  If successful efs_get_device_info will return zero else -1.
 *
 * SIDE EFFECTS  None
 ***********************************************************************/
int efs_get_device_info (struct fs_device_attr * dev_attr);


/***********************************************************************
 * FUNCTION      efs_wait_for_xact
 *
 * DESCRIPTION   This function waits for a transaction to finish.
 *
 ************************************************************************/
void efs_wait_for_xact (void);

#endif /* not __FS_PRIV_FUNCS_H__ */
