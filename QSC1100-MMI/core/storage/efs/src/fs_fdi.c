/***********************************************************************
 * fs_fdi.c
 *
 * General FDI functions.
 * Copyright (C) 2007-2009 QUALCOMM, Inc.
 *
 * This file defined the general FDI functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi.c#4 $ $DateTime: 2009/01/29 17:39:06 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-01-26   ebb   Moved from flash.h => fs_flash.h
2008-07-28   rp    Handle Exceed quota condition (EDQUOT).
2008-05-23   rp    Changes fror new SCM.
2007-12-10   rp    Added FDI support to the efs_get_device_info() API.
2007-10-01   rp    Create

===========================================================================*/

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_fdi_i.h"
#include "fsi.h"
#include "sys_api.h"
#include "fs_util.h"
#include "fs_vnode.h"
#include "fs_desc.h"
#include "fs_fcntl.h"
#include "task.h"
#include "dog.h"
#include "fs_flash.h"
#include "fs_fdi_mnt.h"


/* For the Simulator, the FDI's flash image is stored in these filepaths. */
#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */


/* FDI Initialize. Actual Initializatipn of the FDI file-system will happen
   at the time of mounting. */
void
fs_fdi_init (void)
{
  fs_fdi_mnt_init ();
}

/* Convert an FDI result-code to an EFS result-code. */
int
fs_fdi_conv_fdi_to_efs_err_code(uint32 fdi_err)
{
  int efs_err = ENOERR;

  switch (fdi_err) {
    case IFM_ENOENT:
      efs_err = ENOENT;
      break;

    case IFM_EBADF:
      efs_err = EBADF;
      break;

    case IFM_EACCES:
      efs_err = EACCES;
      break;

    case IFM_EBUSY:
      efs_err = EBUSY;
      break;

    case IFM_EEXIST:
      efs_err = EEXIST;
      break;

    case IFM_EISDIR:
      efs_err = EISDIR;
      break;

    case IFM_EINVAL:
      efs_err = EINVAL;
      break;

    case IFM_ENFILE:
      efs_err = EMFILE;
      break;

    case IFM_ENOSPC:
      efs_err = ENOSPC;
      break;

    case IFM_ERAMFULL:
      efs_err = ENOMEM;
      break;

    case IFM_EFORMAT:
      efs_err = ENOTITM;
      break;

    case IFM_EINIT:
      efs_err = ENODEV;
      break;

    case IFM_ENAMETOOLONG:
      efs_err = ENAMETOOLONG;
      break;

    case IFM_ENOTEMPTY:
      efs_err = ENOTEMPTY;
      break;

    case IFM_ENOTDIR:
      efs_err = ENOTDIR;
      break;

    case IFM_EPERM:
      efs_err = EPERM;
      break;

    case IFM_ETXTBSY:
      efs_err = ETXTBSY;
      break;

    case IFM_EDQUOT:
      efs_err = EDQUOT;
      break;

    case IFM_ENOPKG: // uninitialized data volume
    case IFM_ESYSTEM:
    case IFM_ENONSUPPORT:
    case IFM_EAGAIN:
      /* We are not sure what these error types and so not expecting it. */
      FDI_ASSERT (FALSE);
      break;

    default:
      /* unknown error code, we are not ready for this */
      FDI_ASSERT (FALSE);
  }

  return (-efs_err);
}

/* Convert EFS oflag to an FDI oflag. */
UINT32
fs_fdi_conv_efs_to_fdi_oflag (int efs_oflag)
{
  UINT32 fdi_oflag = 0;

  if (efs_oflag == O_RDONLY) {
    fdi_oflag |= (int)IFM_O_RDONLY;
  }
  else {
    if ((efs_oflag & O_WRONLY) == O_WRONLY) {
      fdi_oflag |= (int)IFM_O_WRONLY;
    }

    if ((efs_oflag & O_RDWR) == O_RDWR) {
      fdi_oflag |= (int)IFM_O_RDWR;
    }

    if ((efs_oflag & O_CREAT) == O_CREAT) {
      fdi_oflag |= (int)IFM_O_CREAT;
    }

    if ((efs_oflag & O_EXCL) == O_EXCL) {
      fdi_oflag |= (int)IFM_O_EXCL;
    }

    if ((efs_oflag & O_TRUNC) == O_TRUNC) {
      fdi_oflag |= (int)IFM_O_TRUNC;
    }

    if ((efs_oflag & O_APPEND) == O_APPEND) {
      fdi_oflag |= (int)IFM_O_APPEND;
    }
  }

  return fdi_oflag;
}

/* Convert an EFS file-mode to an FDI file-mode. */
UINT16
fs_fdi_conv_efs_to_fdi_mode (fs_mode_t efs_mode)
{
  /* The file mode bits are exactly same across FDI and EFS2 */
  UINT16 fdi_mode = (UINT16 )efs_mode;
  return fdi_mode;
}

/* Convert an FDI file-mode to an EFS file-mode. */
fs_mode_t
fs_fdi_conv_fdi_to_efs_mode (UINT16 fdi_mode)
{
  /* The file mode bits are exactly same across FDI and EFS2 */
  fs_mode_t efs_mode = (fs_mode_t )fdi_mode;
  return efs_mode;
}

int
fs_fdi_get_device_attr (struct fs_device_attr *dev_attr)
{
  FDI_BOOLEAN result;
  Device_Attr fdi_dev_attr;
  extern TCHAR fs_fdi_volume_name[];

  result = IFM_Ioctl( (UINT8_PTR)fs_fdi_volume_name,
                       mOS_StrLen(fs_fdi_volume_name),
                       (UINT8_PTR)(&fdi_dev_attr),
                       sizeof(Device_Attr),
                       NULL,
                       DEVICE_ATTR );
  if(!result)
  {
     return fs_fdi_conv_fdi_to_efs_err_code(IFM_Errno);
  }

  dev_attr->device_maker_id = fdi_dev_attr.device_maker_id;
  dev_attr->device_id = fdi_dev_attr.device_id;
  dev_attr->block_count = fdi_dev_attr.block_count;
  dev_attr->block_size = fdi_dev_attr.block_size;
  dev_attr->page_size = fdi_dev_attr.page_size;
  dev_attr->total_page_size = fdi_dev_attr.total_page_size;
  dev_attr->device_type = fdi_dev_attr.device_type;
  dev_attr->total_blocks = fdi_dev_attr.total_blocks;
  dev_attr->bus_width = fdi_dev_attr.bus_width;
  (void)fs_strlcpy (dev_attr->device_name, fdi_dev_attr.device_name,
                    sizeof(dev_attr->device_name));

  return 0;
}

#ifdef FDI_DEBUGGING
void
fdi_dbg_check_vnode (struct fs_vnode *vp)
{
  FDI_DBG_ASSERT (vp != NULL);
  FDI_DBG_ASSERT (vp->p.named.key_length > 0);
}
#endif /* FDI_DEBUGGING */

#endif /* FEATURE_EFS_FDI */
