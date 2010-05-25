/*===========================================================================

                  E M B E D D E D   F I L E   S Y S T E M

DESCRIPTION

  Definitions necessary for the Embedded Filesystem Access Method Manager.

Copyright (c) 2000, 2001, 2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/fs_am.c_v   1.5   17 Sep 2002 15:53:32   davidb  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/src/fs_am.c#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $

when      who     what, where, why
--------  ---     ----------------------------------------------------------
02-27-07  dlb     Remove unsafe API calls.
04-02-04  gr      Added code to free filehandle if fs_enum_init fails.
11-12-03  pg      Corrected filename length checking in fs_am_allocate_handle().
09-17-02  dlb     Add EFS2 compat access method.
11-29-01  dlb     Free fs_statfs handle.
07-11-01  dlb     Implement fs_rename() and fs_remove_remote_links() with
                  access methods enabled.
03/09/01  fas     Added dummy code to inhibit compiler warnings.
01-29-01  dlb     Support card generation tracking.
11-09-00  pg      Initial version for common archive.
08-22-00  dlb     Add SFAT and RPC Access methods if enabled.
                  Add opened_handle_count to check for leaks.
                  Add fs_am_cleanup_after_command.
05-12-00  dlb     Initial version.

===========================================================================*/


#include <string.h>
#include "fs_am.h"
#include "fs.h"
#include "fsi.h"
#include "comdef.h"
#include "fs_mpoint.h"
#include "customer.h"
#include "err.h"
#include "fs_util.h"

#ifdef FEATURE_EFS_ACCESS_METHODS

/*======================================================================*/
/* Maximum length of a filename resulting from the access manager.  This
 * may be longer than the FS_FILENAME_MAX_LENGTH (because the text string
 * can be added.  But, we'll start with this.
 *
 * This should be a parameter in fs_params.h eventually. */
#define ACCESS_METHOD_NAME_MAX  FS_FILENAME_MAX_LENGTH

/* The maximum number of open access method handles. */
#define FS_AM_MAXIMUM_OPEN_HANDLES   FS_MAX_OPEN_FILES

/* One of these is defined for each access method. */
typedef struct fs_access_method {
  char *name;                   /* Name of access method. */
  fs_am_ops_cmd    ops_cmd;     /* Handler for commands. */
} *fs_access_method_t;

/* The "original" EFS Access method".  This will be registered as the
 * default, so that aside from mountpoints, the filesystem will appear to
 * operate normally. */
static struct fs_access_method efs_am = {
  "EFS", fs_ops_cmd
};

#ifdef FEATURE_EFS_SFAT
/* The "mmc" method uses the sfat filesystem with an MMC card. */
void fs_sfat_ops_cmd (fs_cmd_type *cmd_ptr);
static struct fs_access_method mmc_am = {
  "mmc", fs_sfat_ops_cmd
};
#endif /* FEATURE_EFS_SFAT */

#ifdef FEATURE_EFS_RPC
#error code not present
#endif /* FEATURE_EFS_RPC */

/* A test access method.  Doesn't do anything useful.  These feww lines are
 * all that should be in this file when adding a new access method. */
void fs_test_ops_cmd (fs_cmd_type *cmd_ptr);
static struct fs_access_method test_am = {
  "test", fs_test_ops_cmd
};


#ifdef FEATURE_EFS_COMPATIBILITY
/* Access method used to implement the compatibility interface.
*/
void fs_compat_ops_cmd (fs_cmd_type *cmd_ptr);
static struct fs_access_method compat_am = {
  "comp", fs_compat_ops_cmd
};
#endif /* FEATURE_EFS_COMPATIBILITY */

/* The list of available access methods. */
static fs_access_method_t access_methods[] = {
  &efs_am,
  &test_am,
#ifdef FEATURE_EFS_SFAT
  &mmc_am,
#endif

#ifdef FEATURE_EFS_RPC
#error code not present
#endif
#ifdef FEATURE_EFS_COMPATIBILITY
  &compat_am,
#endif
  0
};

/* For each open handle, we have one of these structures.  Additionally, a
 * handle needs to be temporarily allocated for any filesystem operations
 * that use filenames (such as fs_remove). */
typedef struct fs_am_handle {
  fs_am_ops_cmd   ops_cmd;
  fs_handle_type  down_handle;
  fs_handle_type  my_handle;
  struct fs_am_handle *next;
  char name[ACCESS_METHOD_NAME_MAX + 1];
  const char *mpoint, *text;
  dword           generation;
  fs_handle_type  chain_handle;
} *fs_am_handle_t;

/* Keep track of all of the handles. */
static struct fs_am_handle all_handles[FS_AM_MAXIMUM_OPEN_HANDLES];
static fs_am_handle_t allocated_handles = NULL;
static fs_am_handle_t free_handles = NULL;
static int opened_handle_count = 0;

/*======================================================================*/
/* Allocate the handle. */
boolean
fs_am_allocate_handle
(
 const char *filename,
 fs_handle_type *handle,
 const char **new_filename
)
{
  const char *nonmatch, *prefix_text;
  const char *mpoint;
  fs_am_handle_t the_handle;
  fs_access_method_t am;
  dword intsav;
  int change;

  INTLOCK_SAV (intsav);

  if (free_handles == NULL) {
    INTFREE_SAV (intsav);
    ERR ("Too many handles allocated in fs_am.c", 0, 0, 0);
    return FALSE;
  }

  the_handle = free_handles;
  free_handles = the_handle->next;

  /* It is not necessary to lock around the lookup.  The routines that add,
   * will perform a lock.  When we move to mutexes, then the mutex should
   * be held throughout the lookup. */
  am = (fs_access_method_t)
    fs_mpoint_lookup (filename, &nonmatch, &mpoint, &prefix_text);

  /* FIXME: If the mountpoint is removed and the prefix text and such are
   * accessed, then the pointers will not point to valid things.  There is
   * a larger issue here about what to do about open handles when
   * mountpoints are removed. */

  the_handle->mpoint = mpoint;
  the_handle->text   = prefix_text;

  INTFREE_SAV (intsav);

  the_handle->down_handle = 0;   /* Don't know this! */

  the_handle->generation = 0;    /* Zero is an invalid generation. */

  the_handle->chain_handle = FS_INVALID_HANDLE;

  change = strlen(mpoint) - strlen(prefix_text);

  /* Check for overflow of name. */
  if (strlen (prefix_text) + strlen (nonmatch) + change + 1 > ACCESS_METHOD_NAME_MAX) {
    ERR ("EFS: Resultant filename is too long", 0, 0, 0);

    /* Return the handle to the free list. */
    INTLOCK_SAV (intsav);
    the_handle->next = free_handles;
    free_handles = the_handle;
    INTFREE_SAV (intsav);

    return FALSE;
  }

  fs_strlcpy (the_handle->name, prefix_text, sizeof (the_handle->name));
  fs_strlcat (the_handle->name, nonmatch, sizeof (the_handle->name));

  /* Give them our buffer as the name. */
  *new_filename = the_handle->name;

  the_handle->ops_cmd = am->ops_cmd;

  /* Add the handle to the allocated list. */
  INTLOCK_SAV (intsav);
  the_handle->next = allocated_handles;
  allocated_handles = the_handle;
  INTFREE_SAV (intsav);

  *handle = the_handle->my_handle;
  opened_handle_count++;
  return TRUE;
}

/*======================================================================*/
/* Generalized lookup for a handle.  Used internally whenever a handle
 * needs to be looked up. */
/* FIXME: A performance enhancement can be made here by assuming that the
 * all_handles array has consistency of handle numbers, and just checking
 * the range and using subtraction.  There will have to then be a flag in
 * the handle itself indicating if it is allocated or not. (We do assign
 * them in order).  This operation happens frequently. */

static fs_am_handle_t
handle_lookup (fs_handle_type am_handle)
{
  fs_am_handle_t here;

  INTLOCK();
  for (here = allocated_handles; here != NULL; here = here->next) {
    if (here->my_handle == am_handle)
      break;
  }
  INTFREE();

  return here;
}

/*======================================================================*/
/* Free an allocated handle.  Simply returns it to the free list. */
void
fs_am_free_handle (fs_handle_type am_handle)
{
  fs_am_handle_t me = handle_lookup (am_handle);
  fs_am_handle_t *ptr;
  fs_handle_type chain = FS_INVALID_HANDLE;

  if (me == NULL) {
    ERR_FATAL ("fs_am_free_handle, handle is not allocated", 0, 0, 0);
  }

  chain = me->chain_handle;

  INTLOCK ();
  /* Scan through the allocated list, and remove ourself. */
  ptr = &allocated_handles;
  while (*ptr != NULL && *ptr != me) {
    ptr = &(*ptr)->next;
  }
  *ptr = me->next;

  /* Add ourselves to the free list. */
  me->next = free_handles;
  free_handles = me;
  opened_handle_count--;
  INTFREE ();

  /* Recursively free the chain handle.  Assume there isn't a big chain. */
  if (chain != FS_INVALID_HANDLE)
    fs_am_free_handle (chain);
}

/*======================================================================*/
/* Once the given access method has a "private" handle for a given access
 * method handle, it calls fs_am_register_handle so we can give that handle
 * back to it. */
void
fs_am_register_handle (fs_handle_type am_handle,
                       fs_handle_type down_handle)
{
  fs_am_handle_t me = handle_lookup (am_handle);

  if (me == NULL) {
    ERR_FATAL ("There is no handle registered", 0, 0, 0);
  }

  me->down_handle = down_handle;
}

/*======================================================================*/
/* Return the handle previously registered with fs_am_register_handle. */
fs_handle_type
fs_am_lookup_handle (fs_handle_type am_handle)
{
  fs_am_handle_t me = handle_lookup (am_handle);

  if (me == NULL) {
    return FS_INVALID_HANDLE;
  }

  return me->down_handle;
}

/*======================================================================*/
/* Get the ops cmd value for this access method. */
fs_am_ops_cmd
fs_am_get_ops_cmd (fs_handle_type am_handle)
{
  fs_am_handle_t me = handle_lookup (am_handle);

  /* If no handle is registered, return the NULL.  It will be checked by
   * the caller. */
  if (me == NULL) {
    return NULL;
  }

  return me->ops_cmd;
}

/*======================================================================*/
/* Initialize the access method manager. */
void
fs_am_init (void)
{
  int i;

  allocated_handles = NULL;
  free_handles = NULL;
  opened_handle_count = 0;

  /* Move all handles onto the free list. */
  for (i = 0; i < FS_AM_MAXIMUM_OPEN_HANDLES; i++) {
    /* This should be defined above. */
    all_handles[i].my_handle = 0xB000 + i;
    all_handles[i].next = free_handles;
    free_handles = &all_handles[i];
  }

  #ifdef FEATURE_EFS_COMPATIBILITY
    fs_mpoint_init (&compat_am);
  #else
    fs_mpoint_init (&efs_am);
  #endif /* FEATURE_EFS_COMPATIBILITY */
}

/*======================================================================*/
/* Access method implementation. */
fs_status_type
fs_add_access_dir (const char *mountpoint,
                   const char *access_method,
                   const char *prefix,
                   boolean     persistent)
{
  fs_access_method_t *p;
  boolean status;
  dword sav;

  INTLOCK_SAV (sav);
  /* Try looking up the access method. */
  for (p = access_methods; *p != 0; p++) {
    if (strcmp ((*p)->name, access_method) == 0)
      break;
  }

  if (*p == 0) {
    INTFREE_SAV (sav);
    ERR ("fs_am.c: Unsupported access method %s", access_method, 0, 0);
    return FS_FAIL_S;
  }

  status = fs_mpoint_register (mountpoint,
                               TRUE,
                               *p,
                               prefix);
  INTFREE_SAV (sav);
  if (status)
    return FS_OKAY_S;
  else
    return FS_FAIL_S;
}

/*======================================================================*/
fs_status_type
fs_remove_access_dir (const char *mountpoint)
{
  /* Not yet implemented. */
  return FS_FAIL_S;
}

void
fs_test_ops_cmd (fs_cmd_type *cmd_ptr)
{
  ERR_FATAL ("Test ops not implemented", 0, 0, 0);

  /* fsfat_po_open ("dummy", 0, 0); */
}


/*======================================================================*/
/* These utility functions are needed by various access methods to
 * transform prefixes of names back to the user-visible values. */

/* Transform a file or directory listing.  A listing consists of a sequence
 * of NUL terminated strings, with a final single NUL at the end of the
 * list.  Each string must already be prefixed with pre_prefix.  The new
 * buffer will have the same strings with pre_prefix replaced with
 * post_prefix.
 *
 * There are two possibilities (three, the the same size case fits either
 * one).
 *
 * post_prefix is longer than prefix: in this case, the final strings will
 * be longer than the original.  It may be possible that the result will no
 * longer fit in the buffer given.  In that case, the correct solution is
 * to remove some number of strings from the end of the buffer (this is
 * what EFS does).
 *
 * post_prefix is smaller:  The buffer shrinks.  There could be potentially
 * more room at the end of the buffer for more names, but we'll never know.
 * There is no way to ask the filesystem for more names.  Remember, this is
 * a deprecated interface to the filesystem, we don't need to make it work
 * any better than it already does. */

static int scan_back (char *buffer, int start);
void fs_am_xform_list (char *buffer,
                       int  buffer_length,
                       int  *length,
                       const char *pre_prefix,
                       const char *post_prefix)
{
  const int pre_length = strlen (pre_prefix);
  const int post_length = strlen (post_prefix);
  int source, dest;                    /* Temporary pointers. */
  int change;                          /* The amount of change for computing. */
  int len;
  int new_length;

  if (post_length > pre_length) {
    /* This is the case where we are making the buffer larger. */
    change = post_length - pre_length;
    source = 0;
    dest   = 0;
    new_length = *length;

    while (buffer[source] != 0) {
      len = strlen (&buffer[source]);
      if (dest + len + 1 + change < buffer_length) {
        source += len + 1;
        dest   += len + 1 + change;
      } else {
        break;
      }
    }

    /* Back up the pointer if we're sitting on the terminating nul. */
    if (source > 0 && buffer[source-1] == 0) {
      source--;
      dest--;
    }

    /* Put in the terminating NUL. */
    if (dest == 0) {
      buffer[dest] = 0;
      new_length = dest+1;
    } else {
      buffer[dest+1] = 0;
      new_length = dest+2;
    }

    /* Working in reverse, copy the strings back with the prefix. */
    while (source > 0) {
      //printf ("  source = 0x%x, dest = 0x%x\n", source, dest);
      //pdump (buffer, new_length);

      len = scan_back (buffer, source);
      //printf ("   (len = %d\n", len);
      memmove (buffer + (dest - (len - pre_length)),
          buffer + (source - (len - pre_length)),
          len - pre_length + 1);
      memcpy (buffer + (dest - (len - pre_length) - post_length),
          post_prefix, post_length);

      source -= len + 1;
      dest -= len + 1 + change;
    }

    //printf ("  source = 0x%x, dest = 0x%x\n", source, dest);
    //pdump (buffer, new_length);

    *length = new_length;
  } else {
    /* This is the case where we make the buffer smaller.  To do this, we
     * work from the beginning and shrink each string. */

    change = pre_length - post_length;

    source = 0;
    dest = 0;
    new_length = *length;

    while (source < *length - 1) {
      /* This would be the place to sanity check the prefixes. */
      //printf ("  new_length = 0x%x, source = 0x%x, dest = 0x%x\n",
          //new_length, source, dest);
      //pdump (buffer, *length);

      /* Insert the new prefix. */
      memcpy (buffer + dest, post_prefix, post_length);
      dest += post_length;

      source += pre_length;

      len = strlen (buffer + source);

      /* Copy down the string, including the NUL. */
      memcpy (buffer + dest, buffer + source, len + 1);
      dest += len + 1;
      source += len + 1;

      new_length -= change;
    }
    //printf ("  new_length = 0x%x, source = 0x%x, dest = 0x%x\n",
        //new_length, source, dest);
    //pdump (buffer, *length);

    /* Insert the new terminating NUL. */
    buffer[new_length - 1] = 0;

    *length = new_length;
  }
}

/*======================================================================*/
/* Fixup a list of names.  The names are NUL separated, and terminated
 * with an extra NUL.  Length is the number of bytes available in the
 * buffer, and used is a various that gives the number of bytes currently
 * used.  It also returns the number of bytes used in the new buffer.
 *
 * Keep in mind that if the user side prefix is longer than the access
 * method text then there may not be enough room for all of the names.  In
 * that case, this routine will truncate the list so that it fits. */
void
fs_am_fixup_list (char *buffer, int length, dword *used,
                  fs_handle_type handle)
{
  fs_am_handle_t me = handle_lookup (handle);
  int lused = (int) *used;

  fs_am_xform_list (buffer, length, &lused, me->text, me->mpoint);
  *used = (dword) lused;
}

/*======================================================================*/
/* Fix up a single name (for the enum iteration routines). */
void
fs_am_fixup_enum (fs_enum_data_type *data,
                  fs_enum_iterator_type *iterator)
{
  /* Replace the "text" prefix of the name with "mpoint". */
  fs_am_handle_t me = handle_lookup (iterator->am_handle);
  const int text_len = strlen (me->text);
  const int mpoint_len = strlen (me->mpoint);
  int change;

  change = mpoint_len - text_len;

  if (data->fullname_length + change + 1 > FS_FILENAME_MAX_LENGTH) {
    /* FIXME */
    ERR_FATAL ("fs_am_fixup_enum, filename is too long", 0, 0, 0);
  }

  if (data->fullname_length + change < 0) {
    ERR_FATAL ("fs_am_fixup_enum, filename is too short (negative)",
        0, 0, 0);
  }

  /* Move up the part of the name appropriately (with the NUL). */
  memmove (data->fullname + mpoint_len,
      data->fullname + text_len,
      data->fullname_length - text_len + 1);
  memcpy (data->fullname, me->mpoint, mpoint_len);

  data->fullname_length += change;
  data->dirname_length += change;
}

/*======================================================================*/
/* A utility function.  Kind of like strlen, except that it works backward. */
static int
scan_back (char *buffer, int start)
{
  int length = 0;

  while (start > 0 && buffer[start-1] != 0) {
    start--;
    length++;
  }

  return length;
}

/*======================================================================*/
/* The fs_rename operation requires two handles.  This function verifies
 * that these two handles are using the same access method.  It also marks
 * them such that freeing the first handle will also free the second
 * handle. */
boolean
fs_am_chain_handles (fs_handle_type first, fs_handle_type second)
{
  fs_am_handle_t fi = handle_lookup (first);
  fs_am_handle_t se = handle_lookup (second);

  /* Make sure they are both valid. */
  if (fi == NULL || se == NULL) {
    return FALSE;
  }

  /* Return false if they do not refer at least to the same ops command.
   * We can't really know if the rename is completely valid, but at least
   * the command would be handled by the same code. */
  if (fi->ops_cmd != se->ops_cmd) {
    return FALSE;
  }

  /* Now hook them together as a chain, so the second one will be freed as
   * well. */
  fi->chain_handle = second;

  return TRUE;
}

/*======================================================================*/
/* Clean up after performing an FS operation.  This is common logic used
 * by each access method.  This registers handles as needed, and
 * deallocates handles that aren't used.  This routine does not fix up
 * entries returned by directory enumerators.  That should be performed by
 * the access method itself. */

void
fs_am_cleanup_after_command (fs_cmd_type *op)
{
  /* All of the status is at the beginning. */
  fs_status_type status = op->rsp_msg->open.status;

  switch (op->fs_op.op) {
    /* Open is special. */
    case FS_OPEN_OP:
      if (status == FS_OKAY_S) {
        fs_am_register_handle (op->am_handle, op->rsp_msg->open.handle);
        /* Replace the given handle. */
        op->rsp_msg->open.handle = op->am_handle;
      } else {
        fs_am_free_handle (op->am_handle);
      }
      break;

    /* Operations that should free if the operation succeeded. */
    case FS_CLOSE_OP:
    case FS_ENUM_FINISH_OP:
      if (status == FS_OKAY_S) {
        fs_am_free_handle (op->am_handle);
      }
      break;

    /* Operations that should free if the operation failed. */
    case FS_ENUM_INIT_OP:
      if (status != FS_OKAY_S) {
        fs_am_free_handle (op->am_handle);
      }
      break;

    /* Operations that always free. */
    case FS_SET_ATTRIBS_OP:
    case FS_FILE_SIZE_OP:
    case FS_RMFILE_OP:
    case FS_MKDIR_OP:
    case FS_RMDIR_OP:
    case FS_RENAME_OP:
    case FS_NAMETEST_OP:
    case FS_NUM_FILES_OP:
    case FS_NUM_DIRS_OP:
    case FS_LIST_FILES_OP:
    case FS_LIST_DIRS_OP:
    case FS_REMOTE_LINK_OP:
    case FS_RM_REMOTE_LINK_OP:
    case FS_STATFS_OP:
      fs_am_free_handle (op->am_handle);
      break;

    /* This one frees if there is a filename. */
    case FS_GET_ATTRIBS_OP:
      if (op->fs_op.parms.get_attribs.filename != NULL) {
        fs_am_free_handle (op->am_handle);
      }
      break;
  }
}

/*======================================================================*/
/* Keeping track of generation counts in file handles. */

void
fs_am_set_generation (fs_handle_type am_handle, dword generation)
{
  fs_am_handle_t me = handle_lookup (am_handle);

  if (me == NULL) {
    ERR ("No handle registered in fs_am_set_generation", 0, 0, 0);
    return;
  }

  me->generation = generation;
}

dword
fs_am_get_generation (fs_handle_type am_handle)
{
  fs_am_handle_t me = handle_lookup (am_handle);

  if (me == NULL) {
    ERR ("No handle registered", 0, 0, 0);
    return 0;                   /* An invalid generation. */
  }

  return me->generation;
}

/*======================================================================*/
/* Debugging function for doing leak checks on the AM handles. */
int
fs_am_get_opened_handle_count (void)
{
  return opened_handle_count;
  MSG_LOW("",0,0,0) ;
}
#else
void fs_am_dummy( void )
{
  MSG_LOW("",0,0,0) ;
}
#endif /* FEATURE_EFS_ACCESS_METHODS */
