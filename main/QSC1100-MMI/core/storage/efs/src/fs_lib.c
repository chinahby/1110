/***********************************************************************
 * fs_lib.c
 *
 * Public interface for EFS2
 * Copyright (C) 2002-2009 Qualcomm, Inc.
 *
 * This file contains Library API functions which EFS2
 * can utilize.
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_lib.c#6 $ $DateTime: 2009/01/29 17:24:49 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-01-27    sh      Added PUT/GET babylon messages to track NV use
2008-12-12    rp      In efs_iter, handle efs_mkdir failures.
2008-12-11    rp      Fix bug in efs_iter() to iterate the entire "/" dir.
2008-12-05    rp      Allow efs_iter to iterate the entire "/" dir in simulator
2008-10-17    yog     Included err.h for usgae of macro ERR_FATAL.
2008-03-31    yg      Fix klocwork error.
2008-02-22    np      Added md5sum calculation function.
2007-11-16    rp      Added efs_chown_recursive() and efs_chmod_recursive().
2007-08-19    rp      Include fs_global_lock.h due to FS_GLOBAL_LOCK() cleanup.
2007-01-12    dlb     Use safer string functions.
2006-12-05    dlb     Max transaction size configurable.
2006-11-02     rp     Moved fs_path_buffer[] to fs_patbuff.c
2006-10-17    umr     Added FS_PATH_BUFFER_LOCK around fs_path_buffer
2006-09-25    umr     Added efs_deltree() for deleting a whole tree.
2006-09-20    dlb     Lint fixes
2006-06-19    nrs     Allow renames on different size items
2006-03-30    sh      Lint fixes
2005-11-17    nrs     Fixed bug in efs_get, efs_errno
2005-10-28    nrs     Changed length fields from int to fs_size_t
2004-12-16    nrs     Fixed possible cases where Global Unlock may not occur
2004-11-01    nrs     Removed excessive string copies from auto create
2004-11-01    nrs     Changed l_temp_path from local stack to global variable.
2004-10-28    nrs     Add functionality to autocreate directories on efs_put ()
2004-10-08    nrs     Added rename in efs_put () for robustness.
2004-10-08    dlb     Cleanup indenting.
2004-09-30    nrs     Created.

===========================================================================*/

#include "fs_errno.h"
#include "fs_lib.h"
#include "fs_pathbuff.h"
#include "fs_public.h"
#include "fs_efs2.h"
#include "fs_util.h"
#include "assert.h"
#include "fs_global_lock.h"
#include "fs_privatedir.h"
#include "md5.h"
#include "err.h"
#include <stdio.h>
#include "fs_babylon.h"

/* This is the prototype of the callback function which wil be called
   by efs_iter() function. */
typedef int (*EFS_ITER_CALLBACK)(char *path, struct fs_stat *stat_buf,
                                 const void *param);

/* This structure is used to store the User-Id and Group-Id to help
   efs_chown_recursive() to use them later in its callback function. */
struct chown_recursive_callback_param
{
  int uid_val;
  int gid_val;
};

static int efs_put_mkdir (const char * path);
static int efs_put_aux (const char *path, void *data, fs_size_t length,
                        int oflag, int mode);

static char g_temp_path [FS_PATH_MAX+1];


/**********************************************************************
 * FUNCTION      efs_put
 *
 * Store a value in a special item file.
 */
int
efs_put (const char *path, void *data, fs_size_t length, int oflag, int mode)
{
  int result = 0;

  EFS_BAB_START (EFS_BAB_PUT);
  EFS_BAB_PRINT (EFS_BAB_PUT, path);
  EFS_BAB_VALUE (EFS_BAB_PUT, (uint32)length, TRUE, TRUE, TRUE);

  result = efs_put_aux(path, data, length, oflag, mode);
  if (result < 0)
  {
    if ((efs_errno == ENOENT) && (O_AUTODIR & oflag))
    {
      result = efs_put_mkdir((char *) path);
      if (result >= 0)
        result = efs_put_aux(path, data, length, oflag, mode);
    }
  }
  EFS_BAB_RETURN (EFS_BAB_PUT, result);
  EFS_BAB_END (EFS_BAB_PUT);

  return result;
}

/* Read contents of item. */
static int
efs_get_core (const char *path, void *data, fs_size_t length)
{
  int result = 0, fd = 0, index = 0;

  result = efs_raw_get (path, data, length);

  /* efs_raw_get is successful, kick it back */
  if (result >= 0)
    return result;

  /* efs_raw_get couldn't read the item. Return an error unless the path
   * wasn't an S_ITM type. */

  if (efs_errno != EINVAL)
  {
    return result;
  }

  fd = efs_open (path, O_RDONLY);

  /* If the file does not exist, then the current file handle will be less
   * than zero */

  if (fd < 0)
  {
    efs_errno = -EBADF;
    return -1;
  }

  while (length > 0)
  {
    if (length > FS_MAX_TRANSACTION)
      index = FS_MAX_TRANSACTION;
    else
      index = length;

    result = efs_read (fd, data, index);
    /* Check to see if the read failed. If it did, then deterime
     * why it failed. */

    if ((result <= 0) || (result != index))
    {
      if ((result == 0) || (result < index))
        result = -FS_ERANGE;
      efs_errno = -result;
      if (efs_close (fd))
        efs_errno = -EBADF;
      return -1;
    }

    data = (void *) ((uint8 *) data + index);
    length -= index;
  }

  if (efs_close (fd))
  {
    efs_errno = -EBADF;
    return -1;
  }
  return result;
}

/* Read contents of item. */
int
efs_get (const char *path, void *data, fs_size_t length)
{
  int result;

  EFS_BAB_START (EFS_BAB_GET);
  EFS_BAB_PRINT (EFS_BAB_GET, path);
  EFS_BAB_VALUE (EFS_BAB_GET, (uint32)length, TRUE, TRUE, TRUE);

  result = efs_get_core (path, data, length);

  EFS_BAB_RETURN (EFS_BAB_GET, result);
  EFS_BAB_END (EFS_BAB_GET);

  return result;
}

/**********************************************************************
 * deltree_truncate_path
 * Chops off the characters starting from the last character in the passed
 * string all the way upto the occurrence of '/' character.
 */
static int
deltree_truncate_path (char* path, word max_count)
{
  word slen = 0;
  int status = -1, i;

  slen = strlen (path);
  for (i = slen - 1; i >= max_count; i--)
  {
    if (path[i] == '/')
    {
      /* Found the delimitation */
      path[i] = 0;
      status = 0;
      break;
    }
  }
  /* Have not found any sub directories */
  return status;
}

/**********************************************************************
 * readdir_helper
 *
 * Reads a directory and returns the next occurring node by skipping if a
 * "." or ".." is found. Assumes the directory stream pointer is valid.
 * No stats or any validation is performed. It is up to the calling functions
 * to make sure the passed pointer is validated.
 */

static struct fs_dirent*
readdir_helper (EFSDIR *dir_stream_ptr)
{
  struct fs_dirent  *dirent = 0;
  int skip = 0;

  do
  {
    skip = 0;
    dirent = efs_readdir (dir_stream_ptr);
    if (dirent
        && dirent->d_name[0] == '.'
        && dirent->d_name[1] == 0)
    {
      /* We have found a '.' file so proceed to read further */
      skip = TRUE;
    }
    else if (dirent
             && dirent->d_name[0] == '.'
             && dirent->d_name[1] == '.'
             && dirent->d_name[2] == 0)
    {
      /* We have found a '..' file so proceed for next entry */
      skip = TRUE;
    }
  } while (skip);

  return dirent;
}

/**********************************************************************
 * efs_deltree
 *
 * Deletes the directory and all its contents. 'path' can be a directory
 * file, node or sym link. If a non directory is passed as argument it is
 * simply deleted. If the argument is a directory all its contents and itself
 * is deleted. Returns 0 on success and -1 on failure. If a failure occurs,
 * the tree is only partially deleted. On a failure it is unknown as to how
 * many files or directories are left in the parent directory.
 */

int
efs_deltree (const char *path)
{
  EFSDIR            *dir_stream_ptr = 0;
  struct fs_dirent  *dirent = 0;
  struct fs_stat     stat_buf;
  int status = TRUE;
  word dlen = 0;

  if (efs_lstat (path, &stat_buf) != 0)
  {
    /* Unable to stat the path. Return an error */
    return -1;
  }

  if (!S_ISDIR (stat_buf.st_mode))
  {
    /* This is not a directory */
    if (efs_unlink (path) != 0)
    {
      /* Failed to delete the entity return fail */
      status = -1;
    }
    /* We are done here so return 0 */
    status = 0;
  }
  else
  {
    /* This is a directory */
    dir_stream_ptr = efs_opendir (path);
    if (!dir_stream_ptr)
    {
      /* Unable to open the directory */
      status = -1;
    }
    dlen = strlen (path);

    /*
     * Lock the Buffer so that other functions do not use it while the buffer
     * is in use.
     */
    FS_PATH_BUFF_LOCK ();
    fs_strlcpy (fs_path_buff[0], path, sizeof (fs_path_buff[0]));

    while (status > 0)
    {
      dirent = readdir_helper (dir_stream_ptr);

      if (dirent == NULL)
      {
        /* Empty. Delete this directory and return */
        if (efs_closedir (dir_stream_ptr) < 0)
        {
          status = -1;
        }
        if ((status > 0) && (efs_rmdir (fs_path_buff[0]) < 0))
        {
          status = -1;
        }
        /* Make sure we have reached the head directory */
        if ((status > 0) && (dlen == strlen (fs_path_buff[0])))
        {
          /* We have completely erased the tree successfully return. */
          status = 0;
          break;
        }
        else
        {
          /* Go one path back. There is more to do */
          if (deltree_truncate_path (fs_path_buff[0], dlen) == 0)
          {
            /* It has to be a directory don't have to lstat again */
            /* Open the directory and continue the while loop */
            dir_stream_ptr = efs_opendir (fs_path_buff[0]);
            if (!dir_stream_ptr)
            {
              status = -1;
            }
          }
        }
      }
      else
      {
        /* There is some thing more, check it out and process */
        fs_strlcat (fs_path_buff[0], "/", sizeof (fs_path_buff[0]));
        fs_strlcat (fs_path_buff[0], dirent->d_name, sizeof (fs_path_buff[0]));

        if (efs_lstat (fs_path_buff[0], &stat_buf) == -1)
        {
          /* Unable to stat the file or directory. Return an error */
          status = -1;
        }
        else if (S_ISDIR (stat_buf.st_mode))
        {
          /* Close the parent directory so that we could reuse
           * dir_stream_ptr.
           */
          if (efs_closedir (dir_stream_ptr) < 0)
          {
            /* Unable to Close directory. Return an error */
            status = -1;
          }

          /* Open the directory and continue the while loop */
          if (status > 0)
          {
            dir_stream_ptr = efs_opendir (fs_path_buff[0]);
          }
          if (!dir_stream_ptr)
          {
            status = -1;
          }
        }
        else
        {
          /* It should be a file or a link delete this and get back to
           * while loop to look for another.
           */
          if (efs_unlink (fs_path_buff[0]) != 0)
          {
            /* Failed to unlink */
            status = -1;
          }

          /* Get the directory name to the path so as to use again */
          if (deltree_truncate_path (fs_path_buff[0], dlen) < 0)
          {
            status = -1;
          }
        }
      }
    }
    /*
     * Release the buffer so other functions could use.
     */
    FS_PATH_BUFF_UNLOCK ();
  }
  /* We might get here even if one of the above efs_closedir () fails
   * It should be OK to try closing the directory once more.
   */
  efs_closedir (dir_stream_ptr);
  /* Lets make sure one last time if the function was successful */
  if ((status == 0) && (efs_lstat (path, &stat_buf) == 0))
  {
    /* Status is 0, no errors found and the directory still exists,
     * which is seriously wrong. Directory should have not exist if there
     * were no errors. ERR_FATAL for further investigation.
     */
    ERR_FATAL (" efs_deltree () Failed ",0,0,0);
  }

  return status;
}

/**********************************************************************
 * efs_put_mkdir
 *
 * When using efs_put to insert a new item, the directory desired may
 * not exist. Therefore this function will scan the path, and then create
 * the directory. After the directory is created, it will return to efs_put ()
 * to continue inserting the item.
 *
 * Returns a zero on success, or -1 for an error.  Upon error, efs_errno
 * will be set to one of the following:
 *   ENOENT - The item or a component of the path does not exist.
 *   FS_ERANGE - The item file is larger than the specified buffer.
 */

static int
efs_put_mkdir(const char * path)
{
  int result = 0;
  int index = 0;
  int path_len = 0;

  if (path == NULL)
  {
    return -1;
  }

  path_len = strlen(path);

  /* If path length is more than we can store in the buffer we have,
   * then return with error. The buffer has an extra space allocated
   * for the nul terminator also. */
  if (path_len > FS_PATH_MAX)
  {
    efs_errno = ENAMETOOLONG;
    return -1;
  }

  fs_strlcpy (g_temp_path, path, sizeof (g_temp_path));

  index = path_len - 1;

  do
  {
    while ((path[index] != '/') && (index > 0))
    {
      index--;
    }

    if (index > 0)
      g_temp_path[index] = 0x00;

    /* Attempt to make subdirectories */
    result = efs_mkdir (g_temp_path, 0777);
    if (result == 0)
      break;

    /* Move the index pointer down for next subpath */
    index--;

  } while ((index >= 0) || (efs_errno == EEXIST));

  if ((EEXIST == efs_errno) && (result != 0))
  {
    /* The subdirectory exists, so we just need to exit */
    return 0;
  }

  if (result == 0)
  {
    while (index < path_len)
    {
      /* Go back up the path until we reach the end */

      ASSERT (index >= 0);
      g_temp_path[index] = '/';
      /* Need to increment the array index to start going back up the path */
      index++;
      while ((path[index] != '/') && (index < path_len))
      {
        index++;
      }

      if (index >= path_len)
      {
        /* It's the end of the path, now we have to stop */
        break;
      }

      /* Attempt to make subdirectories */
      result = efs_mkdir(g_temp_path, 0777);

      if (0 != result)
        return -1;
    }
  }

  if (result != 0)
    return -1;

  return 0;
}


static int
efs_put_aux (const char *path, void *data, fs_size_t length, int oflag,
    int mode)
{
  int fd = 0;
  int result = 0;
  int index = 0;

  index = strlen(path);

  if (index > FS_PATH_MAX)
  {
    efs_errno = ENAMETOOLONG;
    return -1;
  }

  FS_GLOBAL_LOCK ();
  fs_strlcpy (g_temp_path, path, sizeof (g_temp_path));

  if (index < FS_PATH_MAX)
    fs_strlcat (g_temp_path, "$", sizeof (g_temp_path));
  else
    g_temp_path[FS_PATH_MAX-1] = '$';

  index = 0;
  /* This check forces the behavior to be exactly as before if a normal item
   * is being retrieved.
   */
  if (length <= FS_ITEM_MAX)
  {
    result = efs_raw_put (path, data, length, oflag, mode);
    /* If it is not an item, then we need to create the item and
     * then we need to handle things differently. */
    if ((result < 0)  && (efs_errno == EEXIST))
    {
      result = efs_raw_put (g_temp_path, data, length, oflag, mode);
      if (result == 0)
        result = efs_rename (g_temp_path, path);
    }
    FS_GLOBAL_UNLOCK ();
    return result;
  }

  /* Since the length is too long for a normal item, we need to open/create the
   * file at the path specified
   */


  /* In order to properly handle large files we need to loop the writes.
   * For normal items since the length isn't greater than 8k, we can call
   * the efs_raw_put function from fs_public.c
   */

  /* In order to make this function robust, when the rename() function is
   * fixed close the newly opened file, open a new temp file with a unique
   * name, write the data to the temp file, then after completion rename the
   * file to the desired name. */


  index = 0;

  fd = efs_open(g_temp_path, O_RDWR | O_CREAT | O_TRUNC, 0666);

  if (fd < 0)
  {
    /* There was an error opening the temporary file */
    FS_GLOBAL_UNLOCK ();
    return -1;
  }

  do
  {
    if (length > FS_MAX_TRANSACTION)
      index = FS_MAX_TRANSACTION;
    else
      index = length;
    result = efs_write (fd, data, index);
    if ((result <= 0) || (result < index))
    {
      if (result == 0)
        result = -ENOSPC;
      efs_errno = -result;
      if (result < index)
        efs_errno = -FS_ERANGE;
      if (efs_close (fd))
        efs_errno = -EBADF;
      FS_GLOBAL_UNLOCK ();
      return -1;
    }
    data = (void *) ((uint8 *) data + index);
    length -= index;
  }
  while (length > 0);

  /* If it comes out of the loop properly, then the write was successful. Now
   * we need to move the file from the temporary file to the desired file using
   * rename() */

  /* And now don't forget to close the file */

  if (efs_close (fd))
  {
    efs_errno = -EBADF;
    FS_GLOBAL_UNLOCK ();
    return -1;
  }

  result = efs_rename (g_temp_path, path);

  FS_GLOBAL_UNLOCK ();

  return result;

}

/*=============================================================================
FUNCTION    efs_iter_helper_push_bookmark

DESCRIPTION
    This function creates a symlink pointing which points to the supplied
    bookmark_path in our private folder. This bookmark_path can be retreived
    later by calling efs_iter_helper_pop_bookmark().

PARAMETERS
    bookmark_path -> [IN]
        The directory-path for which we have to create a symbolic link in our
        private folder.
    symlink_nbr <- [OUT]
        This dword gets incremented by 1 and is used to construct the name for
        the symlink. Pass the same dword again for pushing more bookmark_path's
        and each time this dword will get incremented by 1.

Return value
    [-1] if error.
    [ 0] if a symlink to the given bookmark_path was created successfully.
=============================================================================*/
static int
efs_iter_helper_push_bookmark (const char *bookmark_path, dword *symlink_nbr)
{
  /* see efs_iter() function to see why slot 3 */
  char *symlink_path = fs_path_buff[3];

  /* Validate the supplied parameters. */
  ASSERT (bookmark_path != NULL);
  ASSERT (symlink_nbr != NULL);

  /* Do not push empty paths */
  if (bookmark_path[0] == '\0')
  {
    return 0;
  }

  /* construct a name for symlink. */
  ++(*symlink_nbr);
  (void)snprintf (symlink_path, sizeof(fs_path_buff[0]),
      FS_EFS_PRIVATE_DIR_ITER_PATH"/%08ld.lnk", *symlink_nbr);

  return efs_symlink (bookmark_path, symlink_path);
}

/*=============================================================================
FUNCTION    efs_iter_helper_pop_bookmark

DESCRIPTION
    This function retreives the bookmark_path that was pushed earlier by
    calling the efs_iter_helper_push_bookmark() function.

PARAMETERS
    bookmark_path <- [OUT]
        The place holder to store the retreived path, must be atleast as big
        as FS_PATH_MAX+1 bytes.

Return value
    [-1] if error.
    [ 0] if there are no more paths to retreive or if we have retreived a valid
         path.
=============================================================================*/
static int
efs_iter_helper_pop_bookmark (char *bookmark_path)
{
  /* see efs_iter() function to see why slot 3 */
  char *symlink_path = fs_path_buff[3];
  EFSDIR *dirp = NULL;
  struct fs_dirent *direntp;
  struct fs_stat stat_buf;
  int result = 0;

  (void)fs_strlcpy (symlink_path, FS_EFS_PRIVATE_DIR_ITER_PATH,
          sizeof(fs_path_buff[0]));

  dirp = efs_opendir (symlink_path);
  if (dirp == NULL)
  {
    result = -ENOENT;
    goto Error;
  }

  direntp = readdir_helper (dirp);
  if (direntp == NULL)
  {
    /* no more symlinks found, so return success. */
    goto Success;
  }

  (void)fs_strlcpy (symlink_path, FS_EFS_PRIVATE_DIR_ITER_PATH,
                      sizeof(fs_path_buff[0]));
  (void)fs_strlcat (symlink_path, "/", sizeof(fs_path_buff[0]));
  (void)fs_strlcat (symlink_path, direntp->d_name, sizeof(fs_path_buff[0]));
  result = efs_stat (symlink_path, &stat_buf);
  if ( (result != 0) || (!S_ISDIR (stat_buf.st_mode)) )
  {
    /* The symlink is not pointing to a directory, so something wrong. */
    result = -ENOTDIR;
    goto Error;
  }

  memset (bookmark_path, '\0', sizeof(fs_path_buff[0]));
  result = efs_readlink (symlink_path, bookmark_path, sizeof(fs_path_buff[0]));
  if (result == -1)
  {
    goto Error;
  }

  result = efs_unlink (symlink_path);

  goto Success;

Success:
  goto Cleanup;

Error:
  ASSERT (result < 0);
  goto Cleanup;

Cleanup:
  if (dirp != NULL)
  {
    efs_closedir(dirp);
  }

  return result;
}

/*=============================================================================
FUNCTION    efs_iter

DESCRIPTION
    This function can be used to iterate through all the entries of any given
    directory recursively. For every entry found in the directory the supplied
    callback function will be invoked once. Pass 1 as want_dirs if you want to
    be called for sub-directories too.

PARAMETERS
    path -> [IN]
        The directory whose entries we have to iterate over recursively.
    want_dirs -> [IN]
        Pass 1, if you want the callback to happen for sub-directories too.
    efs_iter_callback -> [IN]
        The callback function. Please refer to the callback function prototype
        for more information. If the callback function's return value is not 0
        then an error is assumed and the iteration proceeds with the next
        entry.
    param -> [IN]
        This supplied param will be passed to the callback function.

Return value
    [-ve] if error, the number returned is equal to the -ve count of callback
            invokations that returned error.
    [  0] if we have iterated through all entries and found no error.
=============================================================================*/
static int
efs_iter (const char *path, int want_dirs, EFS_ITER_CALLBACK efs_iter_callback,
          const void *param)
{
  int result = 0, deltree_result;
  int callback_fail_cnt = 0, callback_efs_errno = 0;
  dword symlink_nbr = 0;
  int make_callback;
  EFSDIR *dirp;
  struct fs_dirent *direntp;
  struct fs_stat stat_buf;

  /* For this algorithm we are using 4 path-buffs as follows:
     slot [0] -> is used to store the full-path name, The full-path name is
                 concatenation of dir-path and direntp->d_name.
     slot [1] -> is used to store the dir-path whose content we are reading now
     slot [2] -> is used to store the bookmark_path. The bookmark_path is the
                 latest dir that we have encountered, but have not pushed
                 this dir yet, we have it in RAM for optimization. We push this
                 latest-dir upon encountering one more dir in the same level.
     slot [3] -> is used to store the symlink-path used by the above push and
                 pop functions. */
  char *full_path = fs_path_buff[0];
  char *dir_name = fs_path_buff[1];
  char *bookmark_path = fs_path_buff[2];

  /* Validate the supplied args. */
  ASSERT (path != NULL);
  ASSERT (efs_iter_callback != NULL);
  ASSERT (strlen(path) <= FS_PATH_MAX);

  FS_PATH_BUFF_LOCK();

  /* Prepare the iter directory for storing the symlinks, if there exists any
     file by the same name first remove it, then create the directory. */
  (void)efs_unlink (FS_EFS_PRIVATE_DIR_ITER_PATH);
  result = efs_mkdir (FS_EFS_PRIVATE_DIR_ITER_PATH, 0644);
  if (result < 0)
    return -1;

  (void)fs_strlcpy (full_path, path, sizeof(fs_path_buff[0]));

  result = efs_lstat (full_path, &stat_buf);
  if (result != 0)
  {
    /* Unable to stat the path, so bail. */
    --callback_fail_cnt;
    callback_efs_errno = efs_errno;
    goto Error;
  }

  if (!S_ISDIR (stat_buf.st_mode))
  {
    /* Given path is a file, so just invoke callback for this single file
       and bail. */
    result = (*efs_iter_callback)(full_path, &stat_buf, param);
    if (result != 0)
    {
      --callback_fail_cnt;
      callback_efs_errno = efs_errno;
      goto Error;
    }
    goto Success;
  }

  if (want_dirs)
  {
    result = (*efs_iter_callback)(full_path, &stat_buf, param);
    if (result != 0)
    {
      /* callback function failed, so just record and proceed. */
      --callback_fail_cnt;
      callback_efs_errno = efs_errno;
    }
  }

  /*===========================================================================
  Alogorithm:
  ----------
  1. Start with empty bookmark_path
  2. Loop through all the items in the current directory
    2.1. If it is a file,then just invoke the callback and proceed to next item
    2.2  If it is a directory,push the bookmark_path,mark the current-directory
         as the bookmark_path
  3. When we reach end-of-directory, then consult bookmark_path
  4. If bookmark_path is non-empty then open this dir and proceed to step-2
  5. If boomark_path is empty then pop the pushed item and proceed to step-2
  6. If there are no more items to pop, then return.

  Note: While pushing bookmark_path, we shall ignore empty bookmarks
  ===========================================================================*/

  bookmark_path[0] = '\0';
  (void)fs_strlcpy (dir_name, path, sizeof(fs_path_buff[0]));

  dirp = efs_opendir (path);
  while (dirp != NULL)
  {
    direntp = readdir_helper (dirp);
    while (direntp != NULL)
    {
      /* construct the full-path. */
      (void)fs_strlcpy (full_path, dir_name, sizeof(fs_path_buff[0]));
      (void)fs_strlcat (full_path, "/", sizeof(fs_path_buff[0]));
      (void)fs_strlcat (full_path, direntp->d_name, sizeof(fs_path_buff[0]));

      result = efs_lstat (full_path, &stat_buf);
      if (result != 0)
      {
        int ignore_this_failure = 0;
#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

        if (ignore_this_failure == 0)
        {
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
          goto Error;
        }
      }

      make_callback = 1;

      if (S_ISDIR (stat_buf.st_mode))
      {
        result = efs_iter_helper_push_bookmark(bookmark_path, &symlink_nbr);
        if (result != 0)
        {
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
          goto Error;
        }
        (void)fs_strlcpy (bookmark_path, full_path, sizeof(fs_path_buff[0]));
        make_callback = want_dirs;
      }

      if (make_callback)
      {
        result = (*efs_iter_callback)(full_path, &stat_buf, param);
        if (result != 0)
        {
          /* callback function failed, so just record and proceed. */
          --callback_fail_cnt;
          callback_efs_errno = efs_errno;
        }
      }

      /* proceed to the next item in the current directory. */
      direntp = readdir_helper (dirp);
    }

    /* Visited all the items in this directory, so consult bookmark. */
    efs_closedir (dirp);
    if (strcmp (bookmark_path, "") == 0)
    {
      /* bookmark_path is empty, so try to pop the first pushed item. */
      result = efs_iter_helper_pop_bookmark (bookmark_path);
      if (result != 0)
      {
        --callback_fail_cnt;
        callback_efs_errno = efs_errno;
        goto Error;
      }
      if (strcmp (bookmark_path, "") == 0)
      {
        /* bookmark_path is empty and no-more items to pop, so we should have
           visited all the items in the supplied path, so return success. */
        goto Success;
      }
    }

    /* Now bookmark_path will point to the dir that we should proceed with. */
    (void)fs_strlcpy (dir_name, bookmark_path, sizeof(fs_path_buff[0]));
    bookmark_path[0] = '\0';
    dirp = efs_opendir (dir_name);
  }

  goto Success;

Success:
  goto Cleanup;

Error:
  ASSERT(result < 0);
  goto Cleanup;

Cleanup:
  FS_PATH_BUFF_UNLOCK();
  /* Remove the iter directory along with its contents if any. */
  deltree_result = efs_deltree (FS_EFS_PRIVATE_DIR_ITER_PATH);
  ASSERT (deltree_result == 0);

  result = callback_fail_cnt;
  efs_errno = callback_efs_errno;

  return result;
}

#ifdef FS_UNIT_TEST
#error code not present
#endif /* FS_UNIT_TEST */

/*=============================================================================
FUNCTION    efs_chown_recursive_callback

DESCRIPTION
    Callback function to support efs_chown_recursive(). For every file
    found in the directory supplied to efs_chown_recursive()API, this function
    will get called once, so we could efs_chown() every single file here.

PARAMETERS
    path -> [IN]
        The file-path whom we have to chown().
    stat_buf -> [IN]
        stat info of this file, unused in this function.
    param -> [IN]
        This is a pointer to the struct chown_recursive_callback_param.

Return value
    [-ve] if error.
    [  0] if efs_chown() is success.
=============================================================================*/
static int
efs_chown_recursive_callback(char *path, struct fs_stat *stat_buf,
    const void *param)
{
  struct chown_recursive_callback_param *chown_data_ptr;

  /* Validate the supplied args. */
  ASSERT (path != NULL);
  ASSERT (stat_buf != NULL);
  ASSERT (param != NULL);

  chown_data_ptr = (struct chown_recursive_callback_param *)param;
  return efs_chown (path, chown_data_ptr->uid_val, chown_data_ptr->gid_val);
}

int
efs_chown_recursive (const char *path, int uid_val, int gid_val)
{
  struct chown_recursive_callback_param chown_data;

  chown_data.uid_val = uid_val;
  chown_data.gid_val = gid_val;
  return efs_iter(path, 1, efs_chown_recursive_callback, &chown_data);
}

/*=============================================================================
FUNCTION    efs_chmod_recursive_callback

DESCRIPTION
    Callback function to support efs_chmod_recursive(). For every entry
    found in the directory supplied to efs_chmod_recursive()API, this function
    will get called once, so we could efs_chmod() every single entry here.

PARAMETERS
    path -> [IN]
        The file-path whom we have to chown().
    stat_buf -> [IN]
        stat info of this entry, unused in this function.
    param -> [IN]
        This is the mode of type fs_mode_t.

Return value
    [-ve] if error.
    [  0] if efs_chmod() is success.
=============================================================================*/
static int
efs_chmod_recursive_callback(char *path, struct fs_stat *stat_buf,
    const void *param)
{
  uint32 param_mode  = (uint32 )param;
  fs_mode_t mode = (fs_mode_t )param_mode;

  /* Validate the args. */
  ASSERT (path != NULL);
  ASSERT (stat_buf != NULL);

  return efs_chmod (path, mode);
}

int
efs_chmod_recursive (const char *path, fs_mode_t mode)
{
  uint32 param_mode  = mode;
  return efs_iter(path, 1, efs_chmod_recursive_callback,
              (const void *)param_mode);
}

/* For efs_md5sum to be re-entrant, the memory for data read has to be
 * allocated for each call. Caller passing the memory or dynamic memory were
 * some of the options. The chosen option is allocating a small chunk of
 * memory.  MD5_BUF_SIZE is the amount of memory used for reading the
 * data from the file.
 */
#define MD5_BUF_SIZE 64
/* Calculate the md5 hash file with name "path". */
int
efs_md5sum (const char *path, fs_md5sum_t hash_out_data)
{
  int read_size;
  uint8 mem_read_buffer[MD5_BUF_SIZE];
  MD5_CTX hash_context;
  int fd;

  if (NULL == hash_out_data) {
    return -1;
  }
  fd = efs_open (path, O_RDONLY);

  if (fd < 0) {
    return -1;
  }

  /* Initialize md5 hash calculation. */
  MD5Init (&hash_context);

  while ((read_size = efs_read (fd, mem_read_buffer, MD5_BUF_SIZE)) > 0) {

    /* Perform hash calculation for the latest data-read. */
    MD5Update (&hash_context, (void *) mem_read_buffer, read_size);
  }

  /* Copy the md5sum value to hash_out_data.
   */
  MD5Final (hash_out_data->hash, &hash_context);
  if (efs_close (fd)) {
    return -1;
  }

  /* Check to see if the read failed.
   */
  if (read_size < 0) {
    return -1;
  }

  return 0;
}
