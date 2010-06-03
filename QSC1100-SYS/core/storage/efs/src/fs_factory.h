/***********************************************************************
 * fs_factory.h
 *
 * Factory image construction/deconstruction.
 * Copyright (C) 2004-2008 QUALCOMM, Inc.
 *
 * This module is responsible for both the construction and deconstruction
 * of a canonicalized EFS factory image.  The factory image is a compact
 * representation that can be placed in a fresh flash device (occupying
 * consecutive good blocks).  Upon startup, this code is then able to
 * reconstruct a live filesystem from the image.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_factory.h#3 $ $DateTime: 2008/12/12 09:42:18 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-11-03   rp    Introduce device abstraction.
2008-06-02   rp    Return super-page id after writing super-page.
2007-10-18   rp    Featurized QEFS2.
2006-03-09   dlb   Add a few function exports for testing.
2005-01-04   dlb   Update copyright line.
2004-12-14   dlb   Create

===========================================================================*/

#ifndef __FS_FACTORY_H__
#define __FS_FACTORY_H__

#include "fs_efs2_cust.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_QEFS2
#if defined (FEATURE_EFS_EFS2_ON_NAND) ||   \
    defined (FEATURE_EFS_EFS2_ON_NOR)

#include "fs_sys_types.h"
#include "fs_buffer.h"

/* Read the next chunk of the factory image data.  Uses the fields in the
 * read_info structure to keep track of state (this structure is sent back
 * and forth with the protocol).  The specific fields are historical,
 * because they were incorporated into a protocol.
 *
 * Returns 0 on success, or a negative errno value to indicate an error.
 * Normal completion is indicated by a return with the
 * info->stream_state==0.  Note that when the end is indicated this way, a
 * bogus page of data is also returned.  This page should not be included
 * in the image.
 *
 * The assumption is made that the filesystem is locked for the entire
 * duration of the image transfer.  Changes to the filesystem between calls
 * to fs_fact_get_chunk will almost certainly result in a corrupted
 * filesystem in the target system.
 */
int fs_fact_get_chunk (struct fs_factimage_read_info *info,
    byte *page_data,
    struct fsb_nand_data *buf);

/* This function is called on startup to check for the presence of a
 * factory image, and if detected, reconstruct the necessary tables, and
 * superblocks.  It should return the offset of the constructed superblock,
 * which will allow the ordinary startup code to proceed. */
fs_page_id fs_fact_startup_check (fs_super_t super, efs_device_t dev);

#ifdef FS_UNIT_TEST
#error code not present
#endif

#endif /* defined (FEATURE_EFS_EFS2_ON_NAND) || (FEATURE_EFS_EFS2_ON_NOR) */
#endif /* FEATURE_EFS_QEFS2 */

#endif /* not __FS_FACTORY_H__ */
