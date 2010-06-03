#ifndef __FS_COMPAT_H__
#define __FS_COMPAT_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  E M B E D D E D   F I L E   S Y S T E M

GENERAL DESCRIPTION
  Compatibility interface for EFS2. Users of EFS2 should use the POSIX
  interface as much as possible because this interface is deprecated.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  fs_compat_init should be called before any calls to the API are made.
  FEATURE_EFS_ACCESS_METHODS needs to be turned on.

Copyright (C) 2006 by QUALCOMM Inc. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_compat.h#1 $ $DateTime: 2008/05/29 19:19:11 $ $Author: richardp $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
09/08/06   yg     Fixed compilation error in tmc.c
08/31/06  dlb     Fixed garbled RCS header.
08/30/06   yg     Created new file

===========================================================================*/

#include "customer.h"

#include "fsi.h"
#include "rex.h"

extern rex_tcb_type fs_compat_tcb;

void fs_compat_init (void);
void fs_compat_ops_cmd (fs_cmd_type *cmd_ptr);
void fs_start_ftl_sync_timer (void);

#endif /* __FS_COMPAT_H__ */
