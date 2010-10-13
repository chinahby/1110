/*==========================================================================

                   E M B E D D E D   F I L E   S Y S T E M

DESCRIPTION

  This file contains definitions to deal with filesystem mount
  points.  This module is only to be used internally by the
  filesystem.

Copyright (c) 2000 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/FS_MPOINT.H_v   1.0   09 Nov 2000 08:38:38   pingguan  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/inc/fs_mpoint.h#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $

when        who    what, where, why
----------  ---    ---------------------------------------------------------
11-09-00    pg     Initial version for common archive.
02-29-2000  dlb    Created.

===========================================================================*/

/*
  File system mount point handling.

  All operations through the filesystem first pass through this mount
  point handling code.  Mount points are textual, based on the concept
  of prefix matching, and string matching.

  This module provides an implementation of the mount-point handling
  code.  It does not attempt to maintain persistence of these mount
  points, or any meaning to them.
*/

#ifndef __FS_MPOINT_H__
#define __FS_MPOINT_H__

#include "comdef.h"

/* The data associated with a mountpoint is an arbitrary user
   pointer.  Since C provides no generic or template mechanism, using
   a generic pointer allows us to provide abstraction.  The gains of
   the abstraction and code separation greatly outweigh the
   difficulties associated with a generic pointer. */
typedef void *fs_mount_data_t;

/* Initialze the mountpoint mechanism.  Sets the default mountpoint
   handler if there are no matches. */
void fs_mpoint_init (fs_mount_data_t default_data);

/* Register a prefix mount.  Returns TRUE if the mountpoint could be
   added.  The prefix flag indicates if this mountpoint should be
   matched with a prefix match.  If [prefix] is false, then the
   string given must match the name entirely. */
boolean fs_mpoint_register
(
 const char *name,              /* Name of mountpoint. */
 boolean is_prefix,             /* Is this a prefix mount. */
 fs_mount_data_t data,          /* Data to associate with this
                                   mountpoint. */
 const char *text               /* Text to associate with this mountpoint. */
);

/* Remove the registration of a mountpoint.  This mountpoint will no
   longer match.  Returns TRUE if the mountpoint could be removed, or
   FALSE if this was not possible (for example, if it was never
   registered). */
boolean fs_mpoint_unregister
(
 const char *name               /* Name of mountpoint. */
);

/* Lookup a mountpoint.  Returns the data associated with the given
   mountpoint.  Always returns a value (since there is a default).  If
   [nonmatch] is set it will point to the first character in the given
   string that was not matched.  If this was not a prefix match, it
   will point to the null at the end of the string. */
fs_mount_data_t fs_mpoint_lookup
(
 const char *name,              /* Name to lookup. */
 const char **nonmatch,
 const char **mpoint,           /* Mountpoint that was used. */
 const char **text              /* Text associated with it. */
);

#endif /* __FS_MPOINT_H__ */
