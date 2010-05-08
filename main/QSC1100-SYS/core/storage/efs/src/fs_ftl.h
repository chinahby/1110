#ifndef __FTL_H__
#define __FTL_H__
/******************************************************************************
 * fs_ftl.h
 *
 * This file declares the interface for the NAND Flash Translation Layer to
 *   present the NAND flash as logical sectors.
 *
 * Copyright (C) 2006-2008 Qualcomm, Inc.  All Rights Reserved
 *
 *****************************************************************************/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_ftl.h#2 $
  $DateTime: 2008/12/12 09:42:18 $
  $Author: richardp $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-11-03   rp      Introduce device abstraction.
2008-04-28   np      Added support for multiple FTL partitions.
2008-04-04   rp      Expose FTL_PARTITION_NAME.
2008-03-06   yg      Removed flash driver header file.
2007-07-25   umr     Added support for 4k Page size.
2006-09-01   yg      Fixed compilation issue for non FTL enabled builds.
2006-08-28   yg      Changed function name for 2K page dev & cache support.
2006-05-28   sh      Cosmetic changes
2006-05-21   yg      Cleanup
2006-04-10   yg      Initial file for FTL layer
=============================================================================*/

#include "customer.h"
#include "fs_device.h"

#ifdef FEATURE_EFS_FTL

#include "comdef.h"

/* Set these values to what the maximum size the software has to support */
#define MAX_BLOCK_COUNT          4096
#define MAX_DATA_PAGE_SIZE       4096
#define SPARE_AREA_SIZE            16

/* Return codes for FTL module */
#define FTL_SUCCESS                     0
#define FTL_FAILURE                   (-1)

/* FTL partition handle definition.*/
typedef void* ftl_raw_device_t;

/* Define an invalid TL partition handle. */
#define FTL_NO_DEVICE          ((ftl_raw_device_t)NULL)

/* This structure defines the interface that need to be used while flushing
 * cached pages, where the pages might not be contiguous, so FTL can really
 * look through the list to check if any page is available in the cache.
 * The list should be sorted by sector_num since a moving pointer will be
 * used for checking, so no real search is performed.
 */
typedef struct
{
  uint32    sector_num;
  byte*     data_ptr;
}
cached_buffer_list_type;

/******************************************************************************
FUNCTION      ftl_raw_open

DESCRIPTION   Initialize the FTL layer

PARAMETERS
              driveno  : ftl drive number. Drive number 0 usaually notes ftl
              partition path "0:FTL".

RETURN VALUE
     Valid partition handle : Success
     NULL : Failure
******************************************************************************/
ftl_raw_device_t
ftl_raw_open (int16 driveno);

/**********************************************************************
FUNCTION      ftl_raw_close

DESCRIPTION   Clear all information related a specific partition.

PARAMETERS
              dev : FTL partition handle.
RETURN VALUE
******************************************************************************/
void
ftl_raw_close (ftl_raw_device_t dev);

/******************************************************************************
FUNCTION      ftl_raw_read

DESCRIPTION   Read specified number of sectors from mentioned sector onwards.

PARAMETERS
          dev       : FTL partition device handle.
          sector    : Starting Sector number to read from (page number in flash
                        terminology).
          buffer    : buffer where read data has to be placed.
          count     : Number of sectors to read starting from mentioned sector.

RETURN VALUE
              0 : Success
     Error code : Failure
******************************************************************************/
int ftl_raw_read (ftl_raw_device_t dev, uint32 sector, byte* buffer,
                  uint16 count);

/******************************************************************************
FUNCTION      ftl_raw_write

DESCRIPTION   Write multiple logical sectors starting from the specified one.

PARAMETERS
          dev       : FTL partition device handle.
          sector    : Starting Sector number to write to (page number in flash
                        terminology).
          buffer    : buffer where new data to be written is passed.
          count     : Number of sectors to wrote starting from mentioned sector

RETURN VALUE  Success / Failure
******************************************************************************/
int ftl_raw_write (ftl_raw_device_t dev, uint32 sector,
                   cached_buffer_list_type* buffer, uint16 count);

/******************************************************************************
FUNCTION      ftl_get_raw_dev_params

DESCRIPTION   Get physical flash device parameters.

RETURN VALUE  0
******************************************************************************/
int ftl_get_raw_dev_params (ftl_raw_device_t dev, uint32 * pblock_count,
                            uint32 * ppg_per_block, uint16 * pg_size,
                            uint32 *psector_cnt);


#ifdef FS_UNIT_TEST
#error code not present
#endif

#endif /* FEATURE_EFS_FTL */
#endif /* __FTL_H__ */


