/*==========================================================================

                   E M B E D D E D   F I L E   S Y S T E M

DESCRIPTION

  This file contains definitions to deal with filesystem mount
  points.  This module is only to be used internally by the
  filesystem.

Copyright (c) 2000, 2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/FS_MPOINT.C_v   1.0   09 Nov 2000 08:38:36   pingguan  $
$Header: //source/qcom/qct/core/storage/efs1_compat/main/latest/src/fs_mpoint.c#1 $ $DateTime: 2008/06/03 18:36:07 $ $Author: richardp $

when        who    what, where, why
----------  ---    ---------------------------------------------------------
03-16-07    dlb    Use correct header file.
02-27-07    dlb    Remove unsafe calls.
11-09-00    pg     Initial version for common archive.
02-29-2000  dlb    Created.

===========================================================================*/

/* File system mount point handling. */

#include <string.h>
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "fs_mpoint.h"
#include "fs_util.h"

/*======================================================================*/
/* Because we do not have dynamic memory, these constants define the
   limits on resources to store mountpoints. */

/* The total limit on the number of bytes (including terminating
   nulls) of all mount point matching strings. */
#define FS_MPOINT_TOTAL_STRINGS   1024

/* The total limit of string space available to store strings. */
#define FS_MPOINT_TEXT_SPACE 1024

/* The total number of allowed mountpoints. */
#define FS_MPOINTS_MAX          10

/*======================================================================*/
/* Here is the data we store for all mount points. */

/* First, this string pool is where all of our strings go. */
static char string_pool[FS_MPOINT_TOTAL_STRINGS];

/* How many bytes of the string pool have we used? */
static int string_pool_count = 0;

/* The text pool stores all of the additional text. */
static char text_pool[FS_MPOINT_TEXT_SPACE];

/* How much is used. */
static int text_pool_count = 0;

/* This structure represents the knowledge about a given
   mountpoint.  This is basically the information given to us. */
struct mpoint_info {
  char              *name;
  int                length;    /* Length of name. */
  char              *text;
  int                tlength;
  boolean            is_prefix;
  fs_mount_data_t    data;
};

/* The array of this information is sorted by prefix length.  The
   longest prefix comes first. */
static struct mpoint_info mount_points[FS_MPOINTS_MAX];

/* How many of these do we have? */
static int mount_point_count = 0;

/* The user's given default mount point handler. */
static fs_mount_data_t default_mount_point;

/*======================================================================*/
/* Initialze the mountpoint mechanism.  Sets the default mountpoint
   handler if there are no matches. */
void
fs_mpoint_init (fs_mount_data_t default_data)
{
  string_pool_count = 0;
  mount_point_count = 0;
  default_mount_point = default_data;
}

/*======================================================================*/
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
 const char *text               /* Text to associate with mount. */
)
{
  int length = strlen (name);
  int tlength = strlen (text);
  int index;

  /* Make sure we have room in our pool. */
  if (length + 1 + string_pool_count > FS_MPOINT_TOTAL_STRINGS) {
    ERR("fs_mpoint string pool exceeded, mountpoint dropped: %s",
        name, 0, 0);
    /* Consider increasing FS_MPOINT_TOTAL_STRINGS. */
    return FALSE;
  }

  if (tlength + 1 + text_pool_count > FS_MPOINT_TEXT_SPACE) {
    ERR("fs_mpoint text pool exceeded, mountpoint dropped: %s",
        name, 0, 0);
    /* Consider increasing FS_MPOINT_TEXT_SPACE. */
    return FALSE;
  }

  /* Do we have enough mount points available. */
  if (mount_point_count >= FS_MPOINTS_MAX) {
    ERR("fs_mpoint mount point maximum exceeded", 0, 0, 0);
    /* Consider increasing FS_MPOINTS_MAX. */
    return FALSE;
  }

  /* Find a slot to insert mountpoint.  These are sorted strictly by
     length. */
  for (index = 0; index < mount_point_count; index++) {
    /* For prefixes that are the same length, try to avoid moving the
       existing ones, so go after them. */
    if (mount_points[index].length < length)
      break;
  }

  /* Slide down any additional mount points. */
  if (index < mount_point_count) {
    memmove (&mount_points[index+1], &mount_points[index],
             sizeof (struct mpoint_info) * (mount_point_count - index));
  }

  /* Create our new mount point. */
  mount_points[index].name = &string_pool[string_pool_count];
  mount_points[index].length = length;
  mount_points[index].is_prefix = is_prefix;
  mount_points[index].data = data;
  mount_points[index].text = &text_pool[text_pool_count];
  mount_points[index].tlength = tlength;

  /* Copy over the string. */
  fs_strlcpy (&string_pool[string_pool_count], name,
      sizeof (string_pool) - string_pool_count);
  string_pool_count += length + 1;

  fs_strlcpy (&text_pool[text_pool_count], text,
      sizeof (text_pool) - text_pool_count);
  text_pool_count += tlength + 1;

  mount_point_count++;

  return TRUE;
}

/*======================================================================*/
/* Remove the registration of a mountpoint.  This mountpoint will no
   longer match.  Returns TRUE if the mountpoint could be removed, or
   FALSE if this was not possible (for example, if it was never
   registered). */
boolean fs_mpoint_unregister
(
 const char *name               /* Name of mountpoint. */
)
{
  int index, i;
  int length;
  int base_offset;
  int tlength;
  int tbase_offset;

  for (index = 0; index < mount_point_count; index++) {
    if (strcmp (name, mount_points[index].name) == 0)
      break;
  }
  if (index == mount_point_count) {
    MSG_MED ("fs_mpoint.c: Trying to remove non-existant mountpoint %s",
             name, 0, 0);
    return FALSE;
  }

  /* Keep the stuff we need to clean up the string pool. */
  length = mount_points[index].length;
  base_offset = (mount_points[index].name - string_pool);
  tlength = mount_points[index].tlength;
  tbase_offset = (mount_points[index].text - text_pool);

  /* Remove the index. */
  if (index < mount_point_count - 1) {
    memmove (&mount_points[index], &mount_points[index+1],
             sizeof (struct mpoint_info) * (mount_point_count - (index + 1)));
  }
  mount_point_count--;

  /* We may need to slide the strings down. */
  if (base_offset + length + 1 < string_pool_count) {
    memmove (string_pool + base_offset,
             string_pool + base_offset + length + 1,
             string_pool_count - (base_offset + length + 1));

    /* Go through and adjust any pointers. */
    for (i = 0; i < mount_point_count; i++) {
      if ((mount_points[i].name - string_pool) > base_offset) {
        mount_points[i].name -= length + 1;
      }
    }
  }

  if (tbase_offset + tlength + 1 < text_pool_count) {
    memmove (text_pool + tbase_offset,
             text_pool + tbase_offset + tlength + 1,
             text_pool_count - (tbase_offset + length + 1));

    /* Go through an adjust any pointers. */
    for (i = 0; i < mount_point_count; i++) {
      if ((mount_points[i].text - text_pool) > tbase_offset) {
        mount_points[i].text -= tlength + 1;
      }
    }
  }

  return TRUE;
}

/*======================================================================*/
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
)
{
  int index;
  int length = strlen (name);

  for (index = 0; index < mount_point_count; index++) {

    if (mount_points[index].is_prefix) {
      /* For prefix mounts, check if this prefix could work. */
      if (length >= mount_points[index].length
          && strncmp (name, mount_points[index].name,
                      mount_points[index].length) == 0) {
        /* The prefix match worked. */
        if (nonmatch != NULL) {
          *nonmatch = name + mount_points[index].length;
        }
        if (text != NULL) {
          *text = mount_points[index].text;
        }
        if (mpoint != NULL) {
          *mpoint = mount_points[index].name;
        }
        return mount_points[index].data;
      }

    } else {
      /* For non-prefix mounts, the entire name must match exactly. */
      if (length == mount_points[index].length
          && strcmp (name, mount_points[index].name) == 0) {
        if (nonmatch != NULL) {
          *nonmatch = name + length;
        }
        if (text != NULL) {
          *text = mount_points[index].text;
        }
        if (mpoint != NULL) {
          *mpoint = mount_points[index].name;
        }
        return mount_points[index].data;
      }
    }
  }

  /* Nothing was found, so return the default. */
  if (nonmatch != NULL) {
    *nonmatch = name;
  }
  if (text != NULL) {
    *text = "";
  }
  if (mpoint != NULL) {
    *mpoint = "";
  }
  return default_mount_point;
}

/* For debugging, we have this iterator. */
void
fs_mpoint_iterate (void (*thump)(char *name, boolean is_prefix,
                                 fs_mount_data_t data, char *text))
{
  int index;

  for (index = 0; index < mount_point_count; index++) {
    (*thump) (mount_points[index].name,
              mount_points[index].is_prefix,
              mount_points[index].data,
              mount_points[index].text);
  }
}
