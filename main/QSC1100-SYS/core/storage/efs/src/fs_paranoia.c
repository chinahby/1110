/***********************************************************************
 * fs_paranoia.c
 *
 * Check on the EFS private data structures for any data corruption.
 * Copyright (C) 2007-2008 QUALCOMM, Inc.
 *
 * This feature does run-time checks to verify that EFS private data
 * structures are not corrupted while EFS is not executing.
 *
 * When releasing the lock, we compute and store a hash of all private
 * data.  When taking the global lock, we compare the data structures
 * to the hashed values and announce any discrepancies.  Clearly, this
 * takes some processing time, so it should be enabled only as needed.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_paranoia.c#2 $ $DateTime: 2008/12/12 11:11:10 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-12-12   rp    Increased Paranoia array size to include mNand items.
2007-12-05   rp    Create

===========================================================================*/

#include "fs_config.h"
#include "fs_paranoia.h"

#ifdef FS_PARANOIA_CHECKS

#include "crc.h"
#include <string.h>
#include "assert.h"
#include "fs_err.h"

/* Maximum number of data-structures that we should watch. */
#define FS_MAX_PARANOIA_DATA        20

/* We record the details of each data-pointer that we should watch
   in this structure. */
struct fs_paranoia_data
{
  void    *data_ptr; /* Start-Address of the data that we should watch. */
  int32   data_size; /* How many bytes from start-address to watch? */
  char    *name;     /* A name to identify this data. */
  int16   crc;       /* CRC on the data. */
};

/* This is the table of data-pointers that we should watch. */
static struct fs_paranoia_data paranoia_data[FS_MAX_PARANOIA_DATA];

/* We can use this flag to skip the paranoia check. */
static uint8 skip_paranoia_check = 0;

/* Until FS_GLOBAL_UNLOCK() is called we do not have any crc-data available
   to verify the data integrity, so this flag is used to detect if
   FS_GLOBAL_LOCK() is called nestedly in which case, we should avoid
   crc-check until unlock is called. */
static uint8 did_we_compute_crc = 0;

static uint16
fs_compute_crc (void *data, int byte_count)
{
  uint16 crc;
  uint8 *bytes = (uint8 *)data;
  crc = CRC_16_L_SEED;
  while (byte_count--)
    crc = CRC_16_L_STEP (crc, bytes[byte_count]);
  return crc;
}

#endif /* FS_PARANOIA_CHECKS */

void
fs_paranoia_init (void)
{
#ifdef FS_PARANOIA_CHECKS
  memset (paranoia_data, 0, sizeof (paranoia_data));
  did_we_compute_crc = 0;
#endif
}

void
fs_paranoia_register (void *data_ptr, int32 data_size, char *name)
{
#ifdef FS_PARANOIA_CHECKS
  int i;

  /* Validate the supplied args. */
  ASSERT (data_ptr != NULL);
  ASSERT (data_size > 0);
  ASSERT (name != NULL);

  /* Find an empty slot to insert this new entry. */
  for (i=0; i<FS_MAX_PARANOIA_DATA; ++i)
  {
    if (paranoia_data[i].data_ptr == NULL)
    {
      break;
    }
  }

  /* Make sure we fond an empty slot. */
  ASSERT (i < FS_MAX_PARANOIA_DATA);

  paranoia_data[i].data_ptr = data_ptr;
  paranoia_data[i].data_size = data_size;
  paranoia_data[i].name = name;
  paranoia_data[i].crc = 0;

#else
  (void) data_ptr;
  (void) data_size;
  (void) name;
#endif
}

void
fs_paranoia_unregister (void *data_ptr)
{
#ifdef FS_PARANOIA_CHECKS
  int i;

  /* Validate the supplied args. */
  ASSERT (data_ptr != NULL);

  /* Find this given entry in our table. */
  for (i=0; i<FS_MAX_PARANOIA_DATA; ++i)
  {
    if (paranoia_data[i].data_ptr == data_ptr)
    {
      break;
    }
  }

  /* Make sure we fond the given entry. */
  ASSERT (i < FS_MAX_PARANOIA_DATA);

  /* Free this slot. */
  paranoia_data[i].data_ptr = NULL;
  paranoia_data[i].data_size = 0;
  paranoia_data[i].name = NULL;
  paranoia_data[i].crc = 0;

#else
  (void) data_ptr;
#endif
}

void
fs_paranoia_set_skip_check (uint8 skip_check)
{
#ifdef FS_PARANOIA_CHECKS
  skip_paranoia_check = skip_check;
#else
  (void) skip_check;
#endif
}

void
fs_paranoia_record (void)
{
#ifdef FS_PARANOIA_CHECKS
  int i;

  /* if paranoia check is turned-off, then bail. */
  if (skip_paranoia_check)
    return;

  /* Loop through every valid slot in the table, compute and store the CRC. */
  for (i=0; i<FS_MAX_PARANOIA_DATA; ++i)
  {
    if (paranoia_data[i].data_ptr != NULL)
    {
      paranoia_data[i].crc = fs_compute_crc (paranoia_data[i].data_ptr,
                                             paranoia_data[i].data_size);
      did_we_compute_crc = 1;
    }
  }
#endif
}

void
fs_paranoia_check (void)
{
#ifdef FS_PARANOIA_CHECKS
  int i;
  int16 crc_now;

  /* if paranoia check is turned-off, then bail. */
  if (skip_paranoia_check)
    return;

  /* Bail if we do not have crc-data to verify.  */
  if (!did_we_compute_crc)
  {
    return;
  }

  /* Loop through every valid slot in the table, compute the CRC and check
     it against the stored CRC. */
  for (i=0; i<FS_MAX_PARANOIA_DATA; ++i)
  {
    if (paranoia_data[i].data_ptr != NULL)
    {
      crc_now = fs_compute_crc (paranoia_data[i].data_ptr,
                                paranoia_data[i].data_size);
      if (crc_now != paranoia_data[i].crc)
      {
        FS_ERR_FATAL ("Paranoia: FS-Ram corruption! %08X vs %08X",
                      paranoia_data[i].crc, crc_now, 0);
      }
    }
  }
#endif
}

