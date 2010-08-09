/***********************************************************************
 * fs_diag_access.c
 *
 * Diag access validation for FS Diag.
 * Copyright (C) 2006, 2008 QUALCOMM, Inc.
 *
 * Implementation of Diag's "Access Validation".
 * This file was migrated from:
 * //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.1/fs_diag_access.c#2
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_diag_access.c#3 $ $DateTime: 2008/10/29 18:04:54 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-10-16   yog   Add stub func for !(FEATURE_DIAG_FS_ACCESS_VALIDATION).
2008-07-10    yg   Made FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS configurable.
2006-12-12   s h   Lint fixes
2006-11-02    rp   Increased FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS from 4 to 5.
2006-11-01   s h   Created from MSMSHARED_DIAG_1.1/fs_diag_access.c#2
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "fs_diag_access.h"
#include "fs_stdlib.h"
#include <string.h>

#ifdef FEATURE_DIAG_FS_ACCESS_VALIDATION

/* -------------------------------------------------------------------------
** DIAG Access Validation API for EFS access through Diag
** ------------------------------------------------------------------------- */

/* Number of registered clients */
static int num_clients = 0;

/* Access client data */
static struct {
  const char                     **dir_list_ptr;
  uint32                           dir_list_size;
  diag_fs_access_check_fptr_type   access_cb_fptr;
} access_clients[FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS];

/* Variables used to hold the input path and the canonical version of a path
 * in the directory list.  Not created locally on the stack because of
 * possible overruns to the stack w/o a corresponding change to task.h.
 * This is safe because all FS ops via DIAG are handled in the DIAG
 * context in single threaded order.
 */
static char temp_access_path[FS_DIAG_ACCESS_PATH_MAX+1];
static char temp_dir_entry[FS_DIAG_ACCESS_PATH_MAX+1];

/*===========================================================================

FUNCTION DIAG_FS_REGISTER_ACCESS_CHECK

DESCRIPTION
   Registers a function to be called for all EFS operations via DIAG for
   the list of directories provided by dir_list_ptr.  If dir_list_ptr
   is NULL, then the callback should be called for ALL access to ANY
   file in the file system accessed via DIAG.

   If dir_list_ptr is not NULL, then dir_list_ptr points to an array
   of dir_list_size NULL terminated strings naming the directories for
   which the access callback should be made.

   DIAG will point to the directory list if it is non NULL, clients must
   not alter this memory after registering with DIAG.
============================================================================*/
void
diag_fs_register_access_check (const char** dir_list_ptr,
   uint32 dir_list_size,
   diag_fs_access_check_fptr_type access_cb_fptr)
{
  int i;

  for (i = 0; i < num_clients; i++) {
    if (access_clients[i].access_cb_fptr == access_cb_fptr)
      break;
  }

  if ((i < FS_DIAG_MAX_ACCESS_VALIDATOR_CLIENTS) &&
      (access_cb_fptr != NULL)) {
    access_clients[i].dir_list_ptr = dir_list_ptr;

    if (dir_list_ptr == NULL)
      access_clients[i].dir_list_size = 0;
    else
      access_clients[i].dir_list_size = dir_list_size;

    access_clients[i].access_cb_fptr = access_cb_fptr;

    if (i == num_clients)
      num_clients++;
  }
} /* END diag_fs_register_access_check */


/*===========================================================================

FUNCTION FS_DIAG_IS_SUBDIR

DESCRIPTION
  Takes two null-terminated paths as input and returns 1 if the second path
  is identical to the first or it is a subdirectory of the first and 0
  otherwise.

  NOTE 1: Both arguments must be non-NULL.
============================================================================*/
static int
fs_diag_is_subdir (const char *dir1, const char *dir2)
{
  int dir1len;
  int dir2len;

  if (!dir1 || !dir2)
    return 0;                   /* Should this be fatal? */

  /* Ignore leading slashes. */
  while (*dir1 == '/')
    dir1++;
  while (*dir2 == '/')
    dir2++;

  dir1len = strlen (dir1);
  dir2len = strlen (dir2);

  if (dir1len > dir2len ||
      memcmp (dir1, dir2, dir1len) != 0 ||
      (dir1len != dir2len && dir2[dir1len] != '/'))
    return 0;
  else
    return 1;
} /* END fs_diag_is_subdir */


/*===========================================================================

FUNCTION FS_DIAG_ACCESS_CHECK

DESCRIPTION
  Validates the given operation on the named file. Returns TRUE if the
  operation is allowed and FALSE otherwise. If no clients have registered,
  the operation is permitted by default.
============================================================================*/
boolean
fs_diag_access_check (
    const char *path,         /* Pathname of file being validated        */
    diag_fs_op_type op_type   /* Operation type given by fs_diag_op_type */
)
{
  int i;
  unsigned int j;
  char *input_path;
  char *access_path;

  if ((path == NULL) || (strlen (path) > FS_DIAG_ACCESS_PATH_MAX)) {
    return FALSE;
  }

  /* Make a temporary copy of the input path after canonicalizing it if
   * necessary. This is to take care of multiple ways of specifying the
   * same path.
   */
  input_path = (char *) path;
  if (efs_realpath (input_path, temp_access_path) != NULL)
    input_path = (char *) temp_access_path;

  for (i = 0; i < num_clients; i++) {
    /* If dir_list_size is 0, check ops no matter what the input path is.
     * Return FALSE if the callback forbids the operation.  No need to check
     * the other clients - one NO vote means the operation should be
     * forbidden.
     */
    if (access_clients[i].dir_list_size == 0) {
      if (access_clients[i].access_cb_fptr ((char *) path, op_type)
          == FALSE) {
        return FALSE;
      }
      continue;
    }

    for(j = 0 ; j < access_clients[i].dir_list_size; j++) {
      /* If the input path is in the directory being validated by this client,
       * call the callback.  Note the validity of the callback pointer was
       * validated at registration time.
       *
       * The file is in the directory if the root of the file matches
       * the directory being checked.  e.g.
       *   Directory /brew/media
       *   File      /brew/media/foo.mp3
       * would be checked since the root /brew/media matches
       *
       * Canonicalize the path before doing the comparison to prevent us
       * from being fooled by different ways of specifying the same path.
       */
      access_path = (char *) access_clients[i].dir_list_ptr[j];
      if (efs_realpath (access_path, temp_dir_entry) != NULL)
        access_path = temp_dir_entry;
      if (fs_diag_is_subdir (access_path, input_path)) {
        /* Return FALSE if the callback forbids the operation.  No need to
         * check the other accessors - one NO vote means the operation should
         * be forbidden. Otherwise keep looking for other callbacks so that
         * all the registered clients get a chance to vote NO.
         */
        if (access_clients[i].access_cb_fptr ((char*)path, op_type)
            == FALSE) {
           return FALSE;
        }
      }
    }
  }
  return TRUE;
} /* END fs_diag_access_check */

#else /* !FEATURE_DIAG_FS_ACCESS_VALIDATION */
boolean
fs_diag_access_check (
    const char *path,         /* Pathname of file being validated        */
    diag_fs_op_type op_type)  /* Operation type given by fs_diag_op_type */
{
  (void) path;
  (void) op_type;

  /* Return TRUE as FEATURE_DIAG_FS_ACCESS_VALIDATION is not being defined so
   * no need to check the diag access and have to allow the diag access
   * in unrestricted way. */
  return TRUE;
} /* END fs_diag_access_check */

void
diag_fs_register_access_check (
    const char **dir_list_ptr,
    uint32 dir_list_size,
    diag_fs_access_check_fptr_type access_cb_fptr)
{
  /* As FEATURE_DIAG_FS_ACCESS_VALIDATION is not defined so no need to
   * register an access_check callback function, as have to allow the diag
   * access in unrestricted way. */

  (void) dir_list_ptr;
  (void) dir_list_size;
  (void) access_cb_fptr;
}

#endif /* FEATURE_DIAG_FS_ACCESS_VALIDATION */
