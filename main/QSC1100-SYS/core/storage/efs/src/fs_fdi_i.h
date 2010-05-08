/***********************************************************************
 * fs_fdi.h
 *
 * General FDI functions.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * This file defines the general FDI functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_fdi_i.h#2 $ $DateTime: 2008/07/31 16:37:17 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-07-28   rp    Integrate Q&R Patch
2008-05-23   rp    Renamed fs_fdi.h to fs_fdi_i.h for /inc folder.
2007-12-10   rp    Added FDI support to the efs_get_device_info() API.
2007-10-01   rp    Create

===========================================================================*/
#ifndef __FS_FDI_I_H__
#define __FS_FDI_I_H__

#include "fs_fdi_cust.h"

#ifdef FEATURE_EFS_FDI

#include "fs_sys_types.h"
#include "fs_vnode.h"
#include "ifm_cmn.h"
#include "ifm_api.h"
#include "fs_fdi.h"

/* Invalid FDI File-Descriptor. */
#define FDI_INVALID_FD              (0)

/* FDI Intialize functions. */
void fs_fdi_init (void);


/* Tell TMC-task that we have a fdi_bg_task() functon in the FDI layer. */
extern void fdi_bg_task (dword parm);


/* Functions to convert EFS values to FDI values and vice-versa. */
int fs_fdi_conv_fdi_to_efs_err_code(UINT32 fdi_err);
UINT32 fs_fdi_conv_efs_to_fdi_oflag (int efs_oflag);
UINT16 fs_fdi_conv_efs_to_fdi_mode (fs_mode_t efs_mode);
fs_mode_t fs_fdi_conv_fdi_to_efs_mode (UINT16 fdi_mode);


/* vNode-ops that are common to all file-types. */
int v_fdi_dops_getstat (struct fs_vnode *vp, struct fs_stat *buf);
int v_fdi_dops_getstatvfs (struct fs_vnode *vp, struct fs_statvfs *buf);
int v_fdi_dops_chmod (struct fs_vnode *vp, fs_mode_t new_mode);
int v_fdi_dops_chown (struct fs_vnode *vp, int uid_val, int gid_val);
int v_fdi_dops_set_reservation (struct fs_vnode *vp, uint32 fdi_gid,
    uint32 size);
int v_fdi_dops_set_quota (struct fs_vnode *vp, uint32 fdi_gid, uint32 size);
int v_fdi_dops_get_group_info (struct fs_vnode *vp, uint32 fdi_gid,
    struct fs_group_info *ginfo);

/* Generic functions. */
int fs_fdi_get_device_attr (struct fs_device_attr *dev_attr);


/* Functions used only for debugging. */
#ifdef FDI_DEBUGGING
  void fdi_dbg_check_vnode (struct fs_vnode *vp);
  #define FDI_DBG_CHECK_VNODE(vp)    fdi_dbg_check_vnode (vp)
#else
  #define FDI_DBG_CHECK_VNODE(vp)    (void)0
#endif


#endif /* FEATURE_EFS_FDI */

#endif /* not __FS_FDI_I_H__ */

