/*
 * SFAT external filesystem for EFS2.
 *
 * Copyright (C) 2004-2007 Qualcomm, Inc.
 *
 * Support using the SFAT filesystem as a mountpoint type of EFS2.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $$Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_ext_sfat.c#1 $$ $$DateTime: 2008/05/29 19:19:11 $$ $$Author: richardp $$

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2007-12-07    rp   Enhancements to the statvfs struct.
2006-12-05    dlb  Max transaction size configurable.
2006-11-10    sh   Add failing chmod stub.
2006-11-10    sh   Add chmod stub.
2006-09-12    sh   Conditionally compile this file only for SFAT
2006-09-08    yg   Use index val of drive letter to get hotplug dev offset.
2006-08-27    sh   Include shared declarations in fs_ext_sfat.h.
2006-07-19   dlb   Move transaction limits down to each FS.
2006-07-07   dbl   Implement truncate.
2006-07-06   dlb   Remove 'size' field from vnode.
2006-06-27   dlb   Pass stat through readdir, and extra information from SFAT.
2006-05-11   sch   Removed fs_hotplug_i.h inclusion
2006-05-10    sh   Replaced DRV_GEOMETRY_DESC with block size & count
2006-05-09   sch   fixed e_sfat_statvfs() to return correct total blocks
2006-05-08    sh   Fixed more 0xFFFFFFFF vs -1 warnings
2006-05-07    sh   Fixed hardalloc vs -1 compiler warning
2006-04-16   sch   Corrected the return values of e_sfat_read and e_sfat_write
2006-03-17   sch   Added USB mass storage functionality
2006-01-11   nrs   Fixed Copyright header
2005-12-22   dlb   Extensions to statvfs results.
2005-11-09    sh   Renamed FS_FEATURE_EXTFS to FEATURE_EFS_EXTFS
2005-10-21   dlb   Support both MMC and SDCC.
2005-10-21   dlb   Eliminate warnings.
2005-10-11   dlb   Support multiple devices.
2005-10-07   dlb   Add rename.
2005-09-02   dlb   Fix up directory iteration, allowing multiple iterators.
2005-08-11   dlb   Add umount support.
2005-06-06   dlb   Initial checkin.

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_EFS_EXTFS_SFAT

#include "fs_extfs.h"
#include "fs_errno.h"
#include "fs_err.h"

#include "fs_fcntl.h"

#include "fs_hotplug.h"

#include "msg.h"

#include "fs_ext_sfat.h"
#include "fsfat_sdapi.h"
#include "fs_sfat_i.h"

static int sfat_fixup_error (int driveno);

static int sfat_devno (const char *name);

/* Limit of sfat transaction size.  The SD device is faster, but we still
 * hold the global lock during these operations, so we don't want the value
 * to be too large.  Also, the SFAT API uses 16-bit counts, so it needs to
 * be smaller than that. */
#define FS_MAX_SFAT_XACT_SIZE   32768


/* Maximum length of the 'name' component in the filepath. Deduct 3 for
 * for (drive-letter, colon, trailing-slash) like a:/ */
#define SFAT_MAX_NAME_LEN_UTF8 (EMAXPATH-3)

/* These routines are all expected to return EFS2 error codes.  The return
 * result should either be 0 for success, or a negative error value. */

/* Start the filesystem. */
static int
e_sfat_start (const char *name)
{
  SDBOOL fat_result;

  fat_result = fsfat_pc_system_init (sfat_devno (name));
  if (fat_result) {
    return 0;
  } else {
    return -ENODEV;
  }
}

static int
e_sfat_stop (const char *name)
{
  fsfat_pc_system_close (sfat_devno (name));

  return 0;
}

/* base_stat:
 * Used to determine the fundamental type of the file.  For regular files,
 * it should also fill in the size of the file. */
static int
e_sfat_base_stat (const char *name, fs_mode_t *mode)
{
  STAT sbuf;
  int fat_result;

  fat_result = fsfat_pc_stat ((char *) name, &sbuf);
  if (fat_result != 0) {
    return -(sfat_fixup_error (sfat_devno (name)));
  } else {
    *mode = sbuf.st_mode;

    /* Ugh.  The Sandisk code seems to think that the root directory is a
     * file, not a directory. */
    if (name[0] != 0 && name[1] == ':' && name[2] == '/' && name[3] == 0)
    {
      *mode = (*mode & ~S_IFMT) | S_IFDIR;
    }

    return 0;
  }
}

static int
e_sfat_open (const char *name, int mode)
{
  int result;

  switch (mode) {
    case O_RDONLY:
      result = fsfat_po_open ((char *) name, PO_RDONLY, S_IREAD);
      break;

    case O_RDWR:
      result = fsfat_po_open ((char *) name, PO_RDWR, S_IREAD | S_IWRITE);
      break;

    case (O_CREAT | O_EXCL | O_RDWR):
      result = fsfat_po_open ((char *) name,
          PO_RDWR | PO_CREAT | PO_EXCL,
          S_IREAD | S_IWRITE);
      break;

    default:
      FS_ERR_FATAL ("Invalid internal mode used", 0, 0, 0);

      /* Eliminate compiler warning. */
      return -EINVAL;
  }

  if (result < 0) {
    result = -(sfat_fixup_error (sfat_devno (name)));
  }

  return result;
}

static int
e_sfat_lseek (int fd, fs_off_t pos)
{
  int result;
  INT16 errcode;

  if (pos == FS_OFFSET_APPEND)
    result = fsfat_po_lseek (fd, 0, PSEEK_END, &errcode);
  else
    result = fsfat_po_lseek (fd, pos, PSEEK_SET, &errcode);
  if (result == -1) {
    return -EUNKNOWN_SFAT;
  }

  return 0;
}

static int
e_sfat_read (int fd, void *buf, fs_size_t count)
{
  int result;

  if (count > FS_MAX_SFAT_XACT_SIZE)
    count = FS_MAX_SFAT_XACT_SIZE;

  result = fsfat_po_read (fd, buf, count);

  if ( (fs_size_t) result > count)
    return -EUNKNOWN_SFAT;
  else if (result < 0)
    return -EUNKNOWN_SFAT;

  return result;
}

static int
e_sfat_write (int fd, const void *buf, fs_size_t count)
{
  int result;

  if (count > FS_MAX_SFAT_XACT_SIZE)
    count = FS_MAX_SFAT_XACT_SIZE;

  result = fsfat_po_write (fd, (void *) buf, count);

  if ((fs_size_t) result > count)
    return -EUNKNOWN_SFAT;
  else if (result < 0)
    return -EUNKNOWN_SFAT;

  return result;
}

static int
e_sfat_close (int fd)
{
  int result;

  result = fsfat_po_close (fd);
  if (result != 0)
    return -EUNKNOWN_SFAT;

  return result;
}

static int
e_sfat_mkdir (const char *name)
{
  SDBOOL fat_result;

  fat_result = fsfat_pc_mkdir ((char *) name);
  if (fat_result)
    return 0;
  else
    return -EINVAL;
}

static int
e_sfat_rmdir (const char *name)
{
  SDBOOL fat_result;

  fat_result = fsfat_pc_rmdir ((char *) name);
  if (fat_result)
    return 0;
  else {
    return -sfat_fixup_error (sfat_devno (name));
  }
}

static int
e_sfat_unlink (const char *name)
{
  SDBOOL fat_result;

  fat_result = fsfat_pc_unlink ((char *) name);
  if (fat_result)
    return 0;
  else
    return -sfat_fixup_error (sfat_devno (name));
}

static int
e_sfat_rename (const char *oldname, const char *newname)
{
  SDBOOL fat_result;

  fat_result = fsfat_pc_mv ((char *) oldname, (char *) newname);
  if (fat_result)
    return 0;
  else
    return -sfat_fixup_error (sfat_devno (newname));
}

static int
e_sfat_statvfs (const char *name, struct fs_statvfs *buf)
{
  int dev = sfat_devno (name);
  ULONG space;
  struct hotplug_device *hdev;
  uint16 bytes_per_block;
  uint32 blocks;

  hdev = hotplug_hdev (dev);
  (void) hotplug_dev_get_size (hdev, &blocks, &bytes_per_block);

  space = fsfat_pc_free (dev);
  buf->f_bsize = 512;
  buf->f_bfree = space / 512;
  buf->f_blocks = blocks * (bytes_per_block / 512);
  buf->f_bavail = buf->f_bfree;
  buf->f_files = FS_INVALID_FSFILCNT;
  buf->f_ffree = FS_INVALID_FSFILCNT;
  buf->f_favail = FS_INVALID_FSFILCNT;
  buf->f_fsid = 1;
  buf->f_flag = 0;
  buf->f_namemax = SFAT_MAX_NAME_LEN_UTF8;
  buf->f_balloc = FS_INVALID_FSBLKCNT;
  buf->f_hardalloc = FS_INVALID_HARDALLOC;
  buf->f_maxwrite = FS_MAX_SFAT_XACT_SIZE;

  buf->f_pathmax = EMAXPATH;
  buf->f_is_case_sensitive = 0; /* Filepath is NOT case-sensitive. */
  buf->f_is_case_preserving = 1; /* Path is case-preserved. */
  buf->f_max_file_size = FS_MAX_FILE_SIZE;
  buf->f_max_file_size_unit = 1;
  buf->f_max_open_files = NUSERFILES;
  buf->f_name_rule = FS_FILENAME_RULE_FAT;
  buf->f_name_encoding = FS_FILENAME_ENCODING_UTF8;

  return 0;
}

/**********************************************************************
 * Directory iterators.
 **********************************************************************/
/* Although the SFAT code appears that it intends to allow multiple
 * iterators to be opened, it just doesn't work.  To compensate for this,
 * we keep track of the dirnames for each of the public iterators, and when
 * the iterator changes between calls to readdir, we rewind, and seek
 * forward again.  It is important to do the gfirst on the opendir, so that
 * we can return any error at opendir time, rather than later.
 */

/* The various states that a given iterator can be in.  The 'dot dot'
 * aspect is needed because the FAT doesn't have a "." and ".." entry in
 * the root directory. */
enum dirstate {
  DS_UNUSED,
  DS_FAKE_DOT_EMPTY,
  DS_FAKE_DOTDOT_EMPTY,
  DS_FAKE_DOT,
  DS_FAKE_DOTDOT,
  DS_NORMAL,
  DS_EOF,
};

static DSTAT sfat_dstat;
struct sfat_iter {
  int   state;
  int   index;
  char  dirname[FS_PATH_MAX+1];
};
typedef struct sfat_iter *sfat_iter_t;

#define FS_SFAT_MAX_ITERATORS FS_MAX_ITERATORS

static struct sfat_iter sfat_iters[FS_SFAT_MAX_ITERATORS];

/* State used by sfat_update_iter to position the iterator properly. */
static sfat_iter_t last_iter = NULL;
static int last_iter_index;

/* Update the single, global SFAT iterator to correspond with the desired
 * virtual iterator.  If iterators are being interleaved, close and reopen
 * the proper one, as well as seek to the appropriate position. */
static int
sfat_update_iter (sfat_iter_t iter)
{
  int result;
  int skip = 0;

  if (iter != last_iter || iter->index < last_iter_index) {
    if (last_iter != NULL) {
      fsfat_pc_gdone (&sfat_dstat);
    }
    result = fsfat_pc_gfirst (&sfat_dstat, (char *) iter->dirname);
    last_iter_index = 0;
    if (!result) {
      last_iter = NULL;
      return -ENOENT;
    }
    last_iter = iter;
  }

  /* Although the SFAT iterators will return names for various non-regular
   * file things (such as volume labels), these will return an error on
   * stat.  Set the 'skip' flag to indicate that this entity is not a
   * regular entity (file or dir) and should be skipped over, eliminiating
   * it from the result of the readdir.  There is a similar line below at
   * the end of the loop.
   *
   * This could be changed to only skip volume labels, rather than both
   * hidden and system files, but hidden and system files are of limited
   * use on an embedded device. */
  skip = (sfat_dstat.fattribute & (AHIDDEN | ASYSTEM | AVOLUME)) != 0;

  /* Advance to proper iter. */
  while (last_iter_index < iter->index || skip) {
    result = fsfat_pc_gnext (&sfat_dstat);
    if (!skip)
      last_iter_index++;
    if (result == 0) {
      iter->state = DS_EOF;
      fsfat_pc_gdone (&sfat_dstat);
      last_iter = NULL;
      return -EEOF;
    }
    skip = (sfat_dstat.fattribute & (AHIDDEN | ASYSTEM | AVOLUME)) != 0;
  }

  return 0;
}

/* Directory operations. */
static void *
e_sfat_opendir (const char *name)
{
  int result;
  int len = strlen (name);
  int i;
  sfat_iter_t iter;

  if (name[len-1] == '/')
    len--;

  if (len + 4 > FS_PATH_MAX) {
    /* XXX: Fix result code so we can return errors. */
    /* return -ENAMETOOLONG; */
    return 0;
  }

  for (i = 0; i < FS_SFAT_MAX_ITERATORS; i++) {
    if (sfat_iters[i].state == DS_UNUSED)
      break;
  }
  if (i == FS_SFAT_MAX_ITERATORS)
    return 0;

  iter = &sfat_iters[i];

  memcpy (iter->dirname, name, len);
  strcpy (iter->dirname + len, "/" "*.*");

  iter->index = 0;
  result = sfat_update_iter (iter);

  /* FAT doesn't have "." or ".." entries in the root directory.  As such,
   * an attempt to iterate an empty root directory will fail.  Detect all
   * of this, and try to clean up the semantics a bit. */
  if (name[0] != 0 && name[1] == ':' && name[2] == '/' && name[3] == 0) {
    if (result == 0) {
      iter->state = DS_FAKE_DOT;
    } else {
      iter->state = DS_FAKE_DOT_EMPTY;
    }
  } else {
    if (result != 0) {
      /* XXX: How do we cleanup from this? */
      return 0;
    }

    iter->state = DS_NORMAL;
  }

  return iter;
}

static int
to_lower (int c)
{
  if (c >= 'A' && c <= 'Z')
    return c + ('a' - 'A');
  else
    return c;
}

/* Attributes we care about in files. */
#define ATTR_MASK (~(ARCHIVE | ARDONLY))

static int
e_sfat_readdir (void *dir, struct fs_dirent *dirent)
{
  sfat_iter_t iter = (sfat_iter_t) dir;
  char *out_name;
  int advance = 0;
  int result;

  dirent->d_stats_present = 0;

  switch (iter->state) {
    case DS_FAKE_DOT_EMPTY:
      iter->state = DS_FAKE_DOTDOT_EMPTY;
      out_name = ".";
      break;

    case DS_FAKE_DOTDOT_EMPTY:
      iter->state = DS_EOF;
      out_name = "..";
      break;

    case DS_FAKE_DOT:
      iter->state = DS_FAKE_DOTDOT;
      out_name = ".";
      break;

    case DS_FAKE_DOTDOT:
      iter->state = DS_NORMAL;
      out_name = "..";
      break;

    case DS_NORMAL:
      result = sfat_update_iter (iter);

      if (result != 0)
        return result;

      /* Fill in a few fields in the stat structure to make things a bit
       * more efficient. */
      switch (sfat_dstat.fattribute & ATTR_MASK) {
        case ANORMAL:
          dirent->d_stat.st_mode = S_IFREG;
          break;

        case ADIRENT:
          dirent->d_stat.st_mode = S_IFDIR;
          break;

        default:
          /* This should have been skipped above, so leave as invalid. */
          dirent->d_stat.st_mode = 0;
          break;
      }

      dirent->d_stat.st_size = sfat_dstat.fsize;

      dirent->d_stats_present =
        FS_DIRENT_HAS_TYPE | FS_DIRENT_HAS_ST_SIZE;

      /* Parse either the long or short filename. */
      if (sfat_dstat.fsfat_longFileName[0] != 0)
        out_name = sfat_dstat.fsfat_longFileName;
      else {
        char *dest;
        int i;

        /* We need to construct a filename out of the MSDOS short name.
         * They are stored as upperase but need to be converted to
         * lowercase, since if they were created in uppercase, the
         * longfilename would be used. */

        out_name = 0;    /* Indicate we've already copied it. */

        if (8+1+3 > FS_NAME_MAX)
          return -ENAMETOOLONG;

        /* XXX: Note that technically, shortnames can contains spaces.  It
         * may be better to change this to search backward for the first
         * non-space character. */
        dest = dirent->d_name;
        for (i = 0; i < 8 && sfat_dstat.fname[i] != ' '; i++) {
          *dest++ = to_lower (sfat_dstat.fname[i]);
        }
        if (sfat_dstat.fext[0] != ' ')
          *dest++ = '.';
        for (i = 0; i < 3 && sfat_dstat.fext[i] != ' '; i++) {
          *dest++ = to_lower (sfat_dstat.fext[i]);
        }
        *dest = 0;

      }
      advance = 1;
      break;

    case DS_EOF:
      return -EEOF;

    default:
      FS_ERR_FATAL ("unhandled internal dirstate: %d", iter->state, 0, 0);

      /* Eliminate compiler warning. */
      return -EINVAL;
  }

  if (out_name != NULL) {
    if ((int) strlen (out_name) > FS_NAME_MAX)
      return -ENAMETOOLONG;
    strcpy (dirent->d_name, out_name);
  }

  if (advance) {
    iter->index++;
  }

  return 0;
}

static int
e_sfat_closedir (void *dir)
{
  sfat_iter_t iter = (sfat_iter_t) dir;

  /* If we are closing the active iterator, close the real sfat iterator as
   * well.  Otherwise, there is no sfat iterator open for this EFS
   * iterator. */
  if (last_iter == iter) {
    fsfat_pc_gdone (&sfat_dstat);
    last_iter = NULL;
  }
  iter->state = DS_UNUSED;
  return 0;
}

static int
e_sfat_truncate (int fd, fs_off_t pos)
{
  int result;
  INT16 errcode;

  /* SFAT is "bothered" by trying to truncate a file before one of its
   * descriptors.  Seek the descriptor to the beginning to make sure we're
   * OK. */

  result = fsfat_po_lseek (fd, 0, PSEEK_SET, &errcode);

  /* Ignore the result of this. */
  (void) result;

  result = fsfat_po_truncate (fd, pos);
  if (result != YES)
    // XXX: Fixup this error code.
    return -EUNKNOWN_SFAT;
  else
    return 0;
}

static int
e_sfat_getstat (const char *name, struct fs_stat *buf)
{
  STAT sbuf;
  int result;

  result = fsfat_pc_stat ((char *) name, &sbuf);
  if (result != 0) {
    return -sfat_fixup_error (sfat_devno (name));
  } else {
    /* Note that 'dev' and 'inum' aren't filled in.  They have already been
     * set to valid values, and can be left alone if they don't make sense.
     * Also, dev shouldn't be set, since we want the EFS2 internal value,
     * not the local filesystem's concept. */
    buf->st_mode = sbuf.st_mode;
    buf->st_nlink = sbuf.st_nlink;
    buf->st_size = sbuf.st_size;
    buf->st_blksize = sbuf.st_blksize;
    buf->st_blocks = sbuf.st_blocks;

    /* The Sandisk code seems to think that the root directory is a file,
     * and not a directory. */
    if (name[0] != 0 && name[1] == ':' && name[2] == '/' && name[3] == 0)
    {
      buf->st_mode = (buf->st_mode & ~S_IFMT) | S_IFDIR;
    }

    /* These time fields are different format. */
    /* XXX: Convert the dos timestamps appropriately. */
    buf->st_mtime = 0;
    buf->st_atime = 0;
    buf->st_ctime = 0;
    return 0;
  }
}

static int
e_sfat_chmod (const char *name, fs_mode_t mode)
{
  (void) name;
  (void) mode;
  return -EUNKNOWN_SFAT;
}

static int
sfat_fixup_error (int driveno)
{
  int code = fsfat_pc_get_error (driveno);

  switch (code) {
    case PENOENT:
      return ENOENT;

    case PENOSPC:
      return ENOSPC;

    case PEACCES:
      /* This seems to happen on remove dir of notempty dir as well as
       * access violations. */
      return ENOTEMPTY;

    case PEEXIST:
      return EEXIST;

    default:
      MSG_HIGH ("Unknown error code from sfat: %d", code, 0, 0);
      return EINVAL;
  }
}

/* Figure out which SFAT device to use. */
static int
sfat_devno (const char *name)
{
  if (((name[0] >= 'A' && name[0] <= 'Z') ||
       (name[0] >= 'a' && name[0] <= 'z')) &&
      name[1] == ':')
  {
    if (name[0] >= 'A' && name[0] <= 'Z')
      return name[0] - 'A';
    else
      return name[0] - 'a';
  } else {
    FS_ERR_FATAL ("Invalid drive letter used for sfat mount", 0, 0, 0);

    /* Eliminate compier warning. */
    return 0;
  }

}

void
fs_sfat_init (void)
{
  int i;

  for (i = 0; i < FS_SFAT_MAX_ITERATORS; i++)
    sfat_iters[i].state = DS_UNUSED;
}

struct fs_extfs_ops fs_sfat_ops = {
  e_sfat_start,
  e_sfat_stop,
  e_sfat_base_stat,
  e_sfat_getstat,
  e_sfat_open,
  e_sfat_lseek,
  e_sfat_read,
  e_sfat_write,
  e_sfat_chmod,
  e_sfat_close,
  e_sfat_mkdir,
  e_sfat_rmdir,
  e_sfat_unlink,
  e_sfat_rename,
  e_sfat_statvfs,
  e_sfat_opendir,
  e_sfat_readdir,
  e_sfat_closedir,
  e_sfat_truncate,
};

#else /* FEATURE_EFS_EXTFS_SFAT */
extern int __dont_complain_about_empty_file;
#endif
