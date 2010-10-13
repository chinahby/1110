/**********************************************************************
 * fs_pm_log.c
 *
 * File System Log.
 * Copyright (C) 2002-2009, QUALCOMM, Inc.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_pm_log.c#7 $ $DateTime: 2009/05/20 16:03:23 $ $Author: richardp $

when          who     what, where, why
--------      ---     ------------------------------------------------------
2009-05-18    rp      Removed obsolete feature FS_FEATURE_RMTEFS_SERVER.
2009-04-29    yg      Catch read failure or data corruption of written log pg.
2009-04-22    rp      Avoid marking all blocks as bad in the log region.
2008-11-03    rp      Introduce device abstraction.
2008-08-12    yg      Added new function to flush on not enough buffer space
2008-06-04    np      Lint Error fix using a new variable.
2008-05-15    np      Lint Error fix.
2008-05-08    rp      Handle write-failure in log-region.
2007-10-18    rp      Featurized QEFS2.
2007-08-10    rp      Align buffers passed to flash on a 32-byte boundary.
2007-05-25    umr     Added support for EFS memory protection feature.
2007-02-23    dlb     Move read failures into device wrappers.
2006-09-20    dlb     Lint cleanup.
2006-09-11    dlb     Use flash driver wrappers.
2006-08-23    dlb     Disable log write verification by default.
2006-08-23    dlb     Zero entire remainder of page.
2006-08-17    dlb     New CRC format with in-place upgrades.
2005-12-14    dlb     Export log checking.
2005-11-03    dlb     Track last written log page.
2004-10-15    dlb     Update copyright header.
2003-06-17    jkl     Clean up code.
2003-05-22    dlb     Add some debugging in unit test.
2003-05-07    dlb     Add log grouping.
2003-02-13    dlb     Replay logs for Nand operations.
2002-08-08    drh     Created by dlb.  Added history header.  #ifdef out
                      printf debug statements.

===========================================================================*/

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

#include <string.h>
#ifdef FS_UNIT_TEST
#error code not present
#endif

#include "customer.h"
#include "fs_sys_types.h"
#include "fs_err.h"
#include "comdef.h"
#include "assert.h"

#include "fs_pm_log.h"
#include "fs_pm_super.h"

#include "fs_upgrade.h"

#include "crc.h"
#include "fs_protected_memory.h"

/* These macros offer some additional hints to the simulation environment
 * that we are not rewriting paired bits in this code. */
#ifdef FS_UNIT_TEST
#error code not present
#else
  #define IGNORE_PAIR_VIOLATIONS(dev) \
    do { } while (0)
  #define CHECK_PAIR_VIOLATIONS(dev) \
    do { } while (0)
#endif

#define LOG_MAGIC               0x9F11
#define LOG_FIRST_GROUP_OFFSET  8

#define LOG_PAD                 4

/* The log page is a byte coded block of data.  The first 4 bytes of the
 * page are reserved for a state marker.  This is used for different
 * purposes in different types of flash devices.
 *
 * Following this is the byte coded data.  The top two bits of each code
 * indicate how many 32 bit operands follow the byte code.  The special
 * code LOG_CODE_GROUP_END is followed by a CRC-16 of the previous data
 * (not counting the first 4 bytes).  Encountering an 0xFF in the data
 * stream probably indicates that the log was not successfully written out.
 *
 * The above LOG_CODE_GROUP_END is used for legacy log pages (which can
 * still be read).  Log pages will always be written with
 * LOG_CODE_EXTENDED_END which is followed by a CRC-16 of the data
 * (unlike the LOG_CODE_GROUP_END, this includes the first 4 bytes, skips 4
 * bytes of state, and then the rest of the data).  The remaining bytes
 * following this will be written in a separate write, and must be zero for
 * the log page to be considered valid.  This makes detection of partially
 * written logs more reliable.
 */

#define LOG_CODE_EXTENDED_END  0xFD
#define LOG_END_MARKER         0x00

/* Increment a Queue variable handling the wrapping. */
#define Q_INCR(x)    \
  do { \
    if ((++x) >= FS_LOG_QUEUE_SIZE) \
      ((x) = 0); \
  } while (0)

void
fs_log_init (fs_log_t log, efs_device_t dev,
                  fs_log_page_write_cb_t log_page_write_cb, void *priv_data)
{
  /* Make sure that the log-buffer is aligned as per DMA's expectations. */
  ASSERT (FS_OKAY_FOR_DM (log->buffer));

  log->dev = dev;

  log->q_count = 0;
  log->q_free = 0;
  log->q_flush = 0;
  log->q_alloc = 0;

  log->group_offset = LOG_FIRST_GROUP_OFFSET;

  log->log_page_write_cb = log_page_write_cb;
  log->priv_data = priv_data;
  log->last_log_seq = FS_INVALID_PAGE_ID;

#ifdef FS_UNIT_TEST
#error code not present
#endif
}

int
fs_log_page_count (fs_log_t log)
{
  int alloc = log->q_alloc;
  int flush = log->q_flush;

  if (alloc >= flush)
    return alloc - flush;
  else
    return FS_LOG_QUEUE_SIZE + alloc - flush;
}

int
fs_log_written_count (fs_log_t log)
{
  int free = log->q_free;
  int flush = log->q_flush;

  if (log->q_count == 0)
    return 0;
  if (flush >= free)
    return flush - free;
  else
    return FS_LOG_QUEUE_SIZE + flush - free;
}

fs_page_id
fs_log_peek_page (fs_log_t log)
{
  if (log->q_flush == log->q_alloc)
    FS_ERR_FATAL ("Out of log pages to write", 0, 0, 0);

  return log->queue[log->q_flush];
}

fs_page_id
fs_log_peek_written (fs_log_t log)
{
  if (log->q_count == 0 || log->q_free == log->q_flush)
    FS_ERR_FATAL ("No written page!", 0, 0, 0);

  return log->queue[log->q_free];
}

fs_page_id
fs_log_get_written (fs_log_t log)
{
  fs_page_id result;

  if (log->q_count == 0 || log->q_free == log->q_flush)
    return FS_INVALID_PAGE_ID;
  else {
    result = log->queue[log->q_free];
    Q_INCR (log->q_free);
    log->q_count--;

    return result;
  }
}

void
fs_log_add_page (fs_log_t log, fs_page_id page)
{
  if (log->q_count >= FS_LOG_QUEUE_SIZE)
    FS_ERR_FATAL ("Log queue overflow", 0, 0, 0);

  log->queue[log->q_alloc] = page;
  Q_INCR (log->q_alloc);
  log->q_count++;
}

void
fs_log_add_written_page (fs_log_t log, fs_page_id page)
{
  if (log->q_flush != log->q_alloc)
    FS_ERR_FATAL ("fs_log_add_written_page called after fs_log_add_page",
        0, 0, 0);
  fs_log_add_page (log, page);

  log->q_flush = log->q_alloc;
}

void
fs_log_set_sequence (fs_log_t log, uint32 seqno)
{
  if (seqno == 0xFFFFFFFF)
    seqno = 0;

  log->seqno = seqno;
}

void
fs_log_add (fs_log_t log,
    fs_log_code_t       code,
    uint32              *args)
{
  int i, j, k;
  int count = FS_LOG_CODE_ARGC (code);
  uint32 tmp;

  /* Check for overflow, and flush if necessary.  The LOG_PAD accounts for
   * the bytes needed by the LOG_CODE_GROUP_END and the CRC-16. */
  if (log->group_offset + 1 + (4*count) + LOG_PAD > FS_LOG_PAGE_SIZE) {
#ifdef FS_UNIT_TEST
#error code not present
#endif
    fs_log_flush (log);
  }

#ifdef FS_UNIT_TEST
#error code not present
#endif

  /* Add the single entry. */
  i = log->group_offset;
  log->buffer[i++] = code;
  for (j = 0; j < count; j++) {
    /* Encode 32 bit value. */
    tmp = args[j];
    for (k = 0; k < 4; k++) {
      log->buffer[i++] = tmp & 0xFF;
      tmp >>= 8;
    }
  }

  log->group_offset = i;
}

void
fs_log_flush_if_overflows (fs_log_t log, fs_log_code_t a, uint32 count)
{
  if (log->group_offset + count
      + (count * (4 * FS_LOG_CODE_ARGC (a)))
      + LOG_PAD
      > FS_LOG_PAGE_SIZE)
  {
#ifdef FS_UNIT_TEST
#error code not present
#endif
    fs_log_flush (log);
  }
}

void
fs_log_group (fs_log_t log, fs_log_code_t a, fs_log_code_t b)
{
  if (log->group_offset + 1 + 1
      + (4 * FS_LOG_CODE_ARGC (a))
      + (4 * FS_LOG_CODE_ARGC (b))
      + LOG_PAD
      > FS_LOG_PAGE_SIZE)
  {
#ifdef FS_UNIT_TEST
#error code not present
#endif
    fs_log_flush (log);
  }
}

static void
fs_log_write_page (fs_log_t log)
{
  int result;

  /* If we have partial write in this device, use it to write out the early
   * part of the log before the CRC, and write the CRC separately. */
  if (log->dev->efs_device_type == FS_DEVICE_TYPE_EFS_NOR)
  {
    unsigned ending;

    IGNORE_PAIR_VIOLATIONS (log->dev);
    /* Write out all but the last piece of the data.  The last word is
     * written separately, to detect unfinished writes. */
    ending = FS_LOG_PAGE_SIZE - 4;
    result = fs_device_partial_write (log->dev,
                                     log->queue[log->q_flush],
                                     log->buffer, 0, ending);
    if (result != FS_DEVICE_OK)
      FS_ERR_FATAL ("Error writing log", 0, 0, 0);

    /* Write the remaining 4 bytes, possibly containing the CRC and the
     * ending marker. */
    result = fs_device_partial_write (log->dev,
                                     log->queue[log->q_flush],
                                     log->buffer + ending, ending, 4);
    CHECK_PAIR_VIOLATIONS (log->dev);
  }
  else
  {
    log->log_page_write_cb (log->priv_data, log->buffer);
    result = FS_DEVICE_OK;
    /* Now the log is written successfully. */
  }

  if (result != FS_DEVICE_OK)
    FS_ERR_FATAL ("Error writing log", 0, 0, 0);
}

/* Flush the current log page to the device.  When this code was originally
 * developed, NOR flash devices were written in 16-bit chunks.  Newer
 * devices use larger write buffers (such as 32-bytes).  The CRC check
 * reliably detects write failures on the 16-bit writes, but not the larger
 * write buffers.  The compensate for this, write the log in two pieces,
 * with the last chunks containing the END marker coming last. */
void
fs_log_flush (fs_log_t log)
{
  uint16        crc;
  int i;
  int limit;

#ifdef FS_UNIT_TEST
#error code not present
#endif

  /* If the group offset is already at the beginning, just return.  It is a
   * redundant flush, which doesn't hurt anything. */
  if (log->group_offset == LOG_FIRST_GROUP_OFFSET)
    return;

  log->buffer[log->group_offset++] = LOG_CODE_EXTENDED_END;

  /* Advance the sequence number.  To avoid both all 1's and all 0's as the
   * sequence number, we skip these two values. */
  log->seqno++;
  if (log->seqno == 0xFFFFFFFF)
    log->seqno = 1;

  /* Set the first 32 bits of the field to the sequence number. */
  ((uint32 *) log->buffer)[0] = log->seqno;

  /* Compute a CRC over the chunk of data here. */
  limit = log->group_offset;
  crc = CRC_16_L_SEED;
  for (i = 0; i < 4; i++)
    crc = CRC_16_L_STEP (crc, log->buffer[i]);
  for (i = LOG_FIRST_GROUP_OFFSET; i < limit; i++)
    crc = CRC_16_L_STEP (crc, log->buffer[i]);
  crc = ~crc;

  /* Store the CRC so that it can be checked. */
  log->buffer[log->group_offset++] = crc & 0xFF;
  log->buffer[log->group_offset++] = (crc >> 8) & 0xFF;

  /* Set remainder of buffer to the end marker. */
  memset (log->buffer + log->group_offset,
      LOG_END_MARKER,
      FS_LOG_PAGE_SIZE - log->group_offset);
  log->group_offset++;

  /* The next group of 32 bits is a mask that can have various parts
   * cleared to indicate "phases" this log page goes through. */
  ((uint32 *) log->buffer)[1] = 0xFFFFFFFF;

  if (log->q_flush == log->q_alloc)
    FS_ERR_FATAL ("Out of log pages to write", 0, 0, 0);

  /* Write the current log-buffer at [flush] */
  fs_log_write_page (log);
  Q_INCR (log->q_flush);

  /* Prepare to write the next log. */
  log->group_offset = LOG_FIRST_GROUP_OFFSET;
#ifdef FS_UNIT_TEST
#error code not present
#endif
}

/* Determine if a region of data matches the LOG_END_MARKER.  Used to check
 * that the end marker has been properly written to the end of the log. */
static unsigned
fs_log_is_blanked (const uint8 *data, unsigned length)
{
  while (length > 0) {
    if (*data != LOG_END_MARKER)
      return 0;
    length--;
    data++;
  }
  return 1;
}

/* XXX: Share this buffer, since it is only happening during upgrade. */
static FS_ALIGN uint8 fs_log_upgrade_buffer[FS_LOG_PAGE_SIZE];

/* Check the validity of the log.  This is a little bit complicated,
 * because of the combinations we have due to upgrades, NAND and NOR.  We
 * have three types of CRCs that are possible.
 *
 *   1.  legacy, not followed by anything in particular.
 *   2.  legacy, with a LOG_END_MARKER after it.
 *   3.  New, which always has the LOG_END_MARKER.
 *
 * For NAND, we always just accept any of these, but for NOR, we need to
 * detect case #1, and correct it, only if upgrading.  Once the upgrade is
 * finished, only #2 and #3 are to be considered valid.
 *
 * The reasoning here is that NAND already contains an ECC, which will keep
 * us from even seeing the log pages that are corrupt.  Also, NAND can't be
 * corrected with partial writes.
 */
int
fs_log_is_valid (const void *data, efs_device_t dev, fs_page_id upgrade_page)
{
  const uint8 *buffer = (const uint8 *) data;
  fs_page_id invalid_page_id = FS_INVALID_PAGE_ID;
  int done = 0;
  int offset = LOG_FIRST_GROUP_OFFSET;
  int code;
  int count;
  uint16 crc;
  int i;

  while (!done) {
    if (offset >= FS_LOG_PAGE_SIZE)
      return 0;

    code = buffer[offset++];

    /* Check for a legacy end code.  While upgrading, we allow the log
     * pages to have a non-zero value after the CRC, which we then force to
     * be zero as part of the upgrade.  Once the upgrade is finished,
     * require the zero. */
    if (code == LOG_CODE_GROUP_END) {
      /* Check that this isn't off of the end. */
      if (offset + 2 > FS_LOG_PAGE_SIZE)
        return 0;

      /* Verify the CRC. */
      crc = CRC_16_L_SEED;
      for (i = LOG_FIRST_GROUP_OFFSET; i < offset + 2; i++)
        crc = CRC_16_L_STEP (crc, buffer[i]);
      crc = ~crc;

      if (crc != CRC_16_L_OK) {
        /* CRC is invalid. */
        return 0;
      }

      /* Check the log end marker.  Only applies to NOR flash. */
      if (dev->efs_device_type == FS_DEVICE_TYPE_EFS_NOR &&
          !fs_log_is_blanked (buffer + offset + 2,
            FS_LOG_PAGE_SIZE - (offset + 2)))
      {
        if (fs_upgrade_check (FS_UPGRADE_LOG_ZERO_AFTER_CRC)) {
          /* invalid_page_id variable is used to avoid a lint error. */
          if (upgrade_page == invalid_page_id) {
            FS_ERR_FATAL ("Detected upgrade need, but not in right place",
                0, 0, 0);
          } else {
            int result;
            unsigned update_base;

            /* Write out the LOG_END_MARKER for the entire rest of the
             * page. */
            /* Round down to the nearest location. */
            update_base = (offset + 2) & ~3;
            memcpy (fs_log_upgrade_buffer, buffer, offset + 2);
            memset (fs_log_upgrade_buffer + offset + 2,
                LOG_END_MARKER,
                FS_LOG_PAGE_SIZE - (offset + 2));
            IGNORE_PAIR_VIOLATIONS (dev);
            result = fs_device_partial_write (dev, upgrade_page,
                fs_log_upgrade_buffer + update_base,
                update_base,
                FS_LOG_PAGE_SIZE - update_base);
            CHECK_PAIR_VIOLATIONS (dev);
            if (result != FS_DEVICE_DONE)
              FS_ERR_FATAL ("Unable to upgrade log page for new version",
                  0, 0, 0);
          }
        } else {
          /* Not upgrading, so this is not valid. */
          return 0;
        }
      }

      done = 1;
    } else if (code == LOG_CODE_EXTENDED_END) {
      /* Verify the newer CRC, as well as the end marker. */

      /* First, make sure the code fits in the rest of the page. */
      if (offset + 3 > FS_LOG_PAGE_SIZE)
        return 0;

      if (!fs_log_is_blanked (buffer + offset + 2,
            FS_LOG_PAGE_SIZE - (offset + 2)))
      {
        /* End marker was not written out successfully. */
        return 0;
      }

      crc = CRC_16_L_SEED;
      for (i = 0; i < 4; i++)
        crc = CRC_16_L_STEP (crc, buffer[i]);
      for (i = LOG_FIRST_GROUP_OFFSET; i < offset + 2; i++)
        crc = CRC_16_L_STEP (crc, buffer[i]);
      crc = ~crc;

      if (crc != CRC_16_L_OK) {
        /* The CRC is not correct. */
        return 0;
      }

      done = 1;
    } else if (code == 0xFF)
      return 0;
    else {
      count = FS_LOG_CODE_ARGC (code);

      offset += 4 * count;
    }
  }

  /* If we made it here, all is well. */
  return 1;
}

static int
check_log (fs_log_t log)
{
  if (fs_log_is_valid (log->buffer, log->dev, FS_INVALID_PAGE_ID))
    return FS_LOG_ITER_GOOD;
  else
    return FS_LOG_ITER_CORRUPT;
}

int
fs_log_iter_set_page (fs_log_t log, fs_page_id p, uint32 *header)
{
  int result;
  uint32 curr_log_seq;

  result = fs_device_read_page (log->dev, p, log->buffer,
      FS_FOP_LOG, FS_FLASH_FAIL_OK);
  if (result != FS_DEVICE_DONE) {
    if (fs_device_is_erased (log->dev, p))
      return FS_LOG_ITER_ERASED;
    else
      return FS_LOG_ITER_CORRUPT;
  }

  curr_log_seq = *((uint32 *) log->buffer);
  if (curr_log_seq == 0xFFFFFFFF)
    return FS_LOG_ITER_ERASED;

  /* Check if the log successfully read is valid with our CRC check. */
  result = check_log (log);

  /* We still want to track the sequence number even if the above log
   * check failed, since this would help any holes in the replayed logs */
  if ((log->last_log_seq == FS_INVALID_PAGE_ID) || (curr_log_seq == 1))
  {
    log->last_log_seq = curr_log_seq;
  }
  else
  {
    /* The sequence number is found, now it should be continuation of the
     * previously known sequence number or 1, if its not the case, then its
     * possibly a failure to read a successfully written log page or getting
     * corrupted data because of processor cache related issues. */
    if ((log->last_log_seq + 1) != curr_log_seq)
    {
      /* Not matching...!! read fail of written log page or corrupted data..!
       * Generally results from CPU L1 L2 cache issues...
       * Since the CPU cache could potentially return corrupt data we could
       * catch potential data corruptions here since the ptable flushes
       * assume that the successfully written logs are ALWAYS read back
       * successfully without any corruption. If this is not true it results
       * in actual data corruption of EFS meta data as well as client data
       * leading to various EFS crashes. So crash here early before corrupting
       * any EFS data, otherwise it could result into bricked phone. */
      FS_ERR_FATAL ("Broken log sequence, log page corruption..!! %d %d %d",
                    log->last_log_seq, curr_log_seq, p);
    }
    log->last_log_seq = curr_log_seq;
  }

  if (result != FS_LOG_ITER_GOOD)
    return result;

  log->iter_offset = LOG_FIRST_GROUP_OFFSET;
  log->iter_page = p;

  if (header != NULL) {
    header[0] = ((uint32 *) log->buffer)[0];
    header[1] = ((uint32 *) log->buffer)[1];
  }

  return FS_LOG_ITER_GOOD;
}

fs_page_id
fs_log_iter_peek_page (fs_log_t log)
{
  return log->iter_page;
}

void
fs_log_iter_step (fs_log_t log,
    fs_log_code_t *code,
    uint32 *args)
{
  int lcode, i, j, count;
  uint32 tmp;
  int io;

  if (log->iter_page == FS_INVALID_PAGE_ID) {
    FS_ERR_FATAL ("Usage error: iter step before setup", 0, 0, 0);
  }

  io = log->iter_offset;

  lcode = log->buffer[io++];

  if (lcode == LOG_CODE_GROUP_END || lcode == LOG_CODE_EXTENDED_END) {
    /* Return the legacy end code, even for new logs. */
    *code = LOG_CODE_GROUP_END;
    return;
  }

  count = FS_LOG_CODE_ARGC (lcode);
  for (i = 0; i < count; i++) {
    tmp = 0;
    for (j = 0; j < 4; j++) {
      tmp >>= 8;
      tmp |= (log->buffer[io++] << 24);
    }
    args[i] = tmp;
  }

  log->iter_offset = io;

  *code = lcode;

  return;
}

/* Mark the current position, usually called after a transaction start in
 * the log.  Resetting the log pointer with reset will continue at this
 * point. */
void
fs_log_iter_mark (fs_log_t log)
{
  log->mark_page = log->iter_page;
  log->mark_offset = log->iter_offset;
}

/* Reset the iter pointer to the place it was at with mark. */
void
fs_log_iter_reset (fs_log_t log)
{
  if (log->iter_page != log->mark_page) {
    fs_log_iter_set_page (log, log->mark_page, NULL);
  }

  log->iter_offset = log->mark_offset;
}

void
fs_log_remove_bad_pages (fs_log_t log, uint32 blk_shift, fs_block_id bad_blk)
{
  fs_block_id temp_blk;
  int p1, p2;
  int alloced_pg_cnt, removed_pg_cnt = 0;

  /* We have to retain one bad page at the flush to account for all the
   * journal entries that are recorded against this bad block. Remember
   * the age in the journal entries is the last 2 digits of the log-page. */
  temp_blk = log->queue[log->q_flush] >> blk_shift;
  if (temp_blk == bad_blk)
    Q_INCR (log->q_flush);

  /* See how many log-pages are alloced right now. */
  alloced_pg_cnt = fs_log_page_count (log);

  /* Now from the log-queue, we have to remove all the bad-pages which are
   * between [flush] and [alloc] */
  p1 = p2 = log->q_flush;
  while (p2 != log->q_alloc)
  {
    /* Move p2 until it hits a good block */
    temp_blk = log->queue[p2] >> blk_shift;
    if (temp_blk == bad_blk)
    {
      Q_INCR (p2);
      ++removed_pg_cnt;
      continue;
    }

    /* Now p2 should have landed on a good block, copy it at p1 */
    log->queue[p1] = log->queue[p2];
    Q_INCR (p2);
    Q_INCR (p1);
  }
  log->q_alloc = p1;

  /* Adjust the queue count to account for the pages we just now removed. */
  ASSERT (alloced_pg_cnt >= removed_pg_cnt);
  ASSERT (log->q_count >= removed_pg_cnt);
  log->q_count -= removed_pg_cnt;
}

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

