#ifndef __FLASH_H__
#define __FLASH_H__

/**********************************************************************
 * flash.h
 *
 * Flash device public interface.
 *
 * This module implements a device-independent view of the low-level
 * flash system devices.  This interface hides the implementation details
 * and device peculiarities of a diverse range of devices supported by
 * this module.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash.h#17 $ $DateTime: 2008/07/17 15:26:17 $ $Author: opham $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-07-14   op      Support Factory and Raw Images
2008-04-17   mh      Remove customer.h
2008-04-17   mm      Updated return values for bad_block_check API
2008-04-02   eo      Add return status to set_ecc_state API 
2008-03-26   mh      Add return values in the header comment for function 
                     READ_PAGE_AND_UDATA
2007-12-26   rt      Add an API to export attributes of a partition, clean up
2007-10-02   dp      Clean up redundant lines
2007-09-17   op      Added API to get Physical boundaries of a partition.
2007-06-15   mm      Added headers for flash API's according to the Coding
                     standard. Moved device types and ecc states to public file.                
2007-06-03   rt      Move public definitions to header "flash_public_defs.h"
2007-05-10   rt      Add support for reading pages from OTP area.
2007-02-28   mm      Add flash_open() and flash_close() APIs.
2006-06-21   dp      Adding read_page_and_udata API
2006-06-27   dp      Adding Flash API updates for copy page & spare data bytes
2006-05-23   dp      Changed feature macro names to be consistent
2006-05-04   dp      Adding controller abstraction layer
2006-01-27   dp      Added Intel MLC support
2004-09-30   drh     Moved partition support from one file to another
2004-08-09   drh     Added support for ECC manipulation
2004-03-19   drh     Move partition definitions to separate header file
2004-02-23   drh     Added support for partition tables
2004-01-21   drh     Came from services/efs/fs_device.h

===========================================================================*/

#include "comdef.h"
#include "flash_public_defs.h"

/* Enable support for devices with different write styles - eg MLC NOR */
#define HAVE_FLASH_WRITE_STYLE

/* Support copy_page API */
#define HAVE_FLASH_COPY_PAGE

/* Support read_multiple_pages API */
#define HAVE_FLASH_READ_MPAGES

/* Support a generic read_spare_bytes API */
#define HAVE_FLASH_READ_SPARE_BYTES

/* Support the API for reading/writing user bytes in spare area */
#define HAVE_FLASH_RW_SPARE_UDATA_BYTES

/* Support a generic 'flash_open' API that probes for all (NOR, NAND, OneNAND)
 * available flash devices, searches for the partition name provided by the
 * caller and opens it if one exists */ 
#define HAVE_FLASH_OPEN_AND_CLOSE

/*------------------------------------------------------------
 *
 *  Flash Device Definitions
 *
 * ---------------------------------------------------------*/

/* Device structure */
struct fs_device_data;  

/* Device descriptor which is a pointer to the device structure */
typedef struct fs_device_data *fs_device_t;

/* Public interface to the flash device */
struct fs_device_data {

  /*===========================================================================
  FUNCTION DEVICE_NAME

  DESCRIPTION
      Retrieves a descriptive string name for the device.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      A pointer to a string containing device name.

  SIDE EFFECTS
      None
  ===========================================================================*/
  char * (*device_name) (fs_device_t self);

  /*===========================================================================
  FUNCTION DEVICE_MAKER_ID

  DESCRIPTION
      Retrieves an 8-bit ID, unique to the manufacturer of the device.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An integer representing the manufacturer ID.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*device_maker_id) (fs_device_t self);

  /*===========================================================================
  FUNCTION DEVICE_ID

  DESCRIPTION
      Retrieves a an 8-bit ID unique to the device.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An integer representing the device ID.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*device_id) (fs_device_t self);

  /*===========================================================================
  FUNCTION BLOCK_COUNT

  DESCRIPTION
      Retrieves a one based count of number of blocks in the device.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An unsigned number representing block count.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*block_count) (fs_device_t self);

  /*===========================================================================
  FUNCTION BLOCK_SIZE 

  DESCRIPTION
      Retrieves a one based count of number of logical/physical pages within 
      a sector/block

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An unsigned number representing number of pages in a block.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*block_size) (fs_device_t self);

  /*===========================================================================
  FUNCTION PAGE_SIZE 

  DESCRIPTION
      Retrieves page size in bytes. For NAND devices, the size number does not
      not include the spare bytes.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An unsigned number representing page size in bytes.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*page_size) (fs_device_t self);

  /*===========================================================================
  FUNCTION TOTAL_PAGE_SIZE 

  DESCRIPTION
      Retrieves total page size in bytes including the spare area for NAND
      devices. This function is not supported for NOR devices.

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  PARAMETERS:
      IN :  self - device descriptor 

  RETURN VALUE
      An unsigned number representing total page size in bytes.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*total_page_size) (fs_device_t self);

  /*===========================================================================
  FUNCTION DEVICE_TYPE 

  DESCRIPTION
      Retrieves the device type and technology. At present this function is 
      only supported for NAND devices which can be NAND/OneNAND/ORNAND type.

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An unsigned number representing encoded device type.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*device_type) (fs_device_t self);

  /*===========================================================================
  FUNCTION DEVICE_WIDTH 

  DESCRIPTION
      Retrieves the device width and technology. At present this function is 
      only supported for NAND devices 

  PARAMETERS:
      IN :  self - device descriptor 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      An unsigned number representing encoded device width.

  SIDE EFFECTS
      None
  ===========================================================================*/
  uint32 (*device_width) (fs_device_t self);

  /*===========================================================================
  FUNCTION BAD_BLOCK_CHECK 

  DESCRIPTION
      Checks if a block is bad on a NAND device. This API is not supported for 
      NOR.

  PARAMETERS:
      IN :  self - device descriptor 
      IN :  block - block to be checked 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK        : The block is good
      FS_DEVICE_BAD_BLOCK : The block is bad
      FS_DEVICE_FAIL      : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*bad_block_check) (fs_device_t self, block_id block);

  /*===========================================================================
  FUNCTION MARK_BLOCK_BAD 

  DESCRIPTION
      Marks the specified block as bad for a NAND Device. On NAND, this API
      marks a given block as bad by writing all the pages in the block with
      a Non-0xFF data with ECC disabled. On NOR devices, this API doesnt have 
      any effect.

  PARAMETERS:
      IN  :  self  - pointer to the device structure 
      IN  :  block - block to be marked as bad 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 
  
  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*mark_block_bad) (fs_device_t self, block_id block);

  /*===========================================================================
  FUNCTION WRITE_PAGE 

  DESCRIPTION
      Writes data to the specified page on the device.

  PARAMETERS:
      IN  :  self - pointer to the device structure 
      IN  :  page - page to be written 
      IN  :  data - data to be written to the page 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*write_page) (fs_device_t self, page_id page, void *data);

  /*===========================================================================
  FUNCTION ERASE_BLOCK

  DESCRIPTION
      Erases the contents of the complete block.

  PARAMETERS:
      IN  :  self - pointer to the device structure 
      IN  :  block - block to be erased 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*erase_block) (fs_device_t self, block_id block);

  /*===========================================================================
  FUNCTION READ_POINTER

  DESCRIPTION
      Gets the address of read data.  This function returns an address pointer 
      to a page. This function is only supported on NOR Flash Memories.

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  PARAMETERS:
      IN  :  self - pointer to the device structure
      IN  :  page - page to be read 

  RETURN VALUE
      Address of read data. Returns NULL if this device doesn't support direct 
      mapping of read data. 

  SIDE EFFECTS
      None
  ===========================================================================*/
  void *(*read_pointer) (fs_device_t self, page_id page);

  /*===========================================================================
  FUNCTION READ_PAGE 

  DESCRIPTION
      Read the data into a buffer. 

  PARAMETERS:
      IN  :  self - pointer to the device structure
      IN  :  page - page to be read 
      OUT : data - data being read from the page

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_DONE : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_page) (fs_device_t self, page_id page, void *data);

  /*===========================================================================
  FUNCTION IS_ERASED 

  DESCRIPTION
      Determine erase status of page. This function checks if a specified page 
      is erased on a NAND device. It checkes if every byte of a given page 
      including the spare area is erased.

  PARAMETERS:
      IN  : self - pointer to the device structure
      IN  : page - page being checked for erase status
 
  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      TRUE  : If page is erased
      FALSE : If page is not erased, or any error occurs during checking.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*is_erased) (fs_device_t self, page_id page);  

  /*===========================================================================
  FUNCTION PARTIAL_WRITE 

  DESCRIPTION
      This function performs a partial page write. The write must be contained 
      entirely within a page, and is only guaranteed to work on 32 bit 
      boundaries. Partial writes are supported only on NOR devices.
      
  PARAMETERS:
      IN  : self - pointer to the device structure
      IN  : page - page to be written
      IN  : data - data to be written to the page
      IN  : offset - Offset within the page at which to start writing the data
      IN  : length - lenth of the data to be written  

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_DONE : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*partial_write) (fs_device_t self, page_id page, void *data, 
                        int offset, int length);

  /*===========================================================================
  FUNCTION BEGIN_ERASE_BLOCK 

  DESCRIPTION
      Starts erase process on a block. This function begins a block erase 
      process. This function is only available for NOR flash Memories. This 
      operation must be suspended before writes can occur. This function
      does not wait for the process to complete.

  PARAMETERS:
      IN  : self  - pointer to the device structure
      IN  : block - block to erase 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      An erase operation is begun upon the flash. This operation must be 
      suspended before writes can occur. 
  ===========================================================================*/
  int (*begin_erase_block) (fs_device_t self, block_id block);

  /*===========================================================================
  FUNCTION SUSPEND_ERASE 

  DESCRIPTION
      Suspends the current erase. This function is used to suspend the erase
      process. This is useful when reading/programming other blocks is 
      necessary. This function is only available for NOR Memories.

  PARAMETERS:
      IN  : self  - pointer to the device structure

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The erase has completed early.
      FS_DEVICE_BUSY : The erase is suspended.
      FS_DEVICE_FAIL : The suspend failed.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*suspend_erase) (fs_device_t self);

  /*===========================================================================
  FUNCTION RESUME_ERASE 

  DESCRIPTION
      Resumes the suspended erase. This function resumes the erase on a 
      suspended block. This function is only available for NOR Flash Memories. 
      The device is not in read mode during an erase. The erase needs to be 
      suspended before reading data from any other block.

  PARAMETERS:
      IN  : self  - pointer to the device structure

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_BUSY : The erase is running again.
      FS_DEVICE_FAIL : The resume failed.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*resume_erase) (fs_device_t self);

  /*===========================================================================
  FUNCTION ERASE_STATUS 

  DESCRIPTION
      This function checks if the erase block operation started with 
      begin_erase_block function call is completed. This function is only 
      availabel in NOR.

  PARAMETERS:
      IN  : self  - pointer to the device structure

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_DONE : Erase has completed.
      FS_DEVICE_BUSY : Erase is still running.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*erase_status) (fs_device_t self);

  /*===========================================================================
  FUNCTION IS_BLOCK_ERASED

  DESCRIPTION
      Checks if given block is erased. 

  PARAMETERS:
      IN  : self  - pointer to the device structure
      IN  : block - block to check the erase status 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      TRUE  : If page is erased
      FALSE : If page is not erased, or any error occurs during checking.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*is_block_erased) (fs_device_t self, block_id block);  

  /*===========================================================================
  FUNCTION READ_SPARE_BYTES 

  DESCRIPTION
      Read the specified number of bytes from spare_offset in the spare area in 
      given page and copy into given data buffer. Data is read with ECC turned 
      OFF. This function is only supported for NAND devices.

  PARAMETERS:
      IN  : self         - pointer to the device structure
      IN  : page         - page to read the spare bytes data 
      IN  : spare_offset - offset at which to start reading the spare bytes.
      IN  : len          - length of spare data to be read
      OUT : data         - Output buffer pointer for the read spare data

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_spare_bytes) (fs_device_t self, page_id page, uint32 spare_offset, 
                           uint32 len, void *data);

  /*===========================================================================
  FUNCTION READ_MPAGES 

  DESCRIPTION
      Reads num_pages from start_page and copy them into given data location/
      address. The wdog_kick_fn_ptr needs to be provided by the caller to kick
      the watchdog after every individual read. In the case that watchdog is 
      disabled and wdog_kick_fn_ptr is not required, then this pointer should 
      be set to NULL. This function is only supported for NAND devices.


  PARAMETERS:
      IN  :  self             - pointer to the device structure
      IN  :  start_page       - start page to read the data from
      IN  :  num_pages        -  the number of pages to read
      OUT :  data             -  the data being read
      IN  :  wdog_kick_fn_ptr - the watchdog kick function pointer
  
  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_mpages) (fs_device_t self, page_id start_page, page_id num_pages, 
                      void *data, void (*wdog_kick_fn_ptr)(void));

  /*===========================================================================
  FUNCTION COPY_PAGE 

  DESCRIPTION
      Copy data from source page and write it into dest_page. This function is 
      only supported for NAND devices.

  PARAMETERS:
      IN  : self      - pointer to the device structure
      IN  : src_page  - the source page for the copy 
      IN  : dest_page - the destination page to write the copied data to

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*copy_page) (fs_device_t self, page_id src_page, page_id dest_page);

  /*===========================================================================
  FUNCTION LOCK_OPS 

  DESCRIPTION
      Support lock/unlock/locktight of blocks from start_page to end_page. This 
      is only supported on OneNAND devices. Currently, only Unlock operation is 
      supported. 

  PARAMETERS:
      IN  : self       - pointer to the device structure
      IN  : start_page - the start page to perform the operation
      IN  : end_page   - the end page to perform the operation 
      IN  : opcode     -  the operation to be performed.

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*lock_ops) (fs_device_t self, page_id start_page, page_id end_page,
                   uint32 opcode);

  /*===========================================================================
  FUNCTION GET_SPARE_CORRECTED_BYTES 

  DESCRIPTION
      Get corrected spare user data bytes available from controller. In the 
      software ECC approach not all spare user bytes available as 
      a few bytes are needed to store the ECC for the spare user bytes. Hence 
      this API should be used by the clients to actually determine how many 
      spare user bytes are available to the client. This function is only 
      supported for NAND devices.

  PARAMETERS:
      IN  : self  - pointer to the device structure

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      Number of bytes of spare aread that can be used to write user data. 

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*get_spare_corrected_bytes) (fs_device_t self);

  /*===========================================================================
  FUNCTION WRITE_PAGE_AND_UDATA 

  DESCRIPTION
      Write page data and spare user data bytes to given page with any
      hardware/software ECC for spare user data bytes if needed. This function 
      is only supported for NAND devices.

  PARAMETERS:
      IN  : self             - pointer to the device structure
      IN  : page             - page to be written 
      IN  : page_data        - main page data to be written 
      IN  : spare_udata      - spare user data to be written 
      IN  : spare_udata_blen - length of the spare data to be written 
  

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*write_page_and_udata) (fs_device_t self, page_id page, void *page_data,
                               void *spare_udata, uint32 spare_udata_blen);

  /*===========================================================================
  FUNCTION READ_SPARE_UDATA 

  DESCRIPTION
      Read only the user data in spare location of given page with ECC
      correction/detection. This function is only supported for NAND devices.

  PARAMETERS:
      IN  : self  - pointer to the device structure
      IN  : page  - page to read
      OUT : spare_udata - spare data being read
      IN  : spare_udata_blen - length of the spare data to read

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_spare_udata) (fs_device_t self, page_id page, void *spare_udata,
                           uint32 spare_udata_blen);

  /*===========================================================================
  FUNCTION READ_PAGE_AND_UDATA

  DESCRIPTION
      Read the data and Spare bytes into buffers. This function is only 
      supported for NAND devices.

  PARAMETERS:
      IN  :  self            - pointer to the device structure
      IN  :  page            - page to read 
      OUT :  data            - buffer for page data to be read
      OUT :  spare_udata           - buffer spare data to be read
      IN  :  spare_udata_len - length of the spare data to read 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range
      FS_DEVICE_FAIL_PAGE_ERASED:  The operation failed because the
                      page being requested to read is an erased page.
                      This is returned only the case of WM reads
  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_page_and_udata) (fs_device_t self, page_id page, void *data, 
                              void *spare_udata, uint32 spare_udata_blen);

  /*===========================================================================
  FUNCTION READ_OTP_PAGE_AND_SPARE

  DESCRIPTION
      Read user data and spare bytes from a page in OTP area in the
      flash device.

      Please note that though this API has been designed to allow
      reading just the spare area of a page and reading pages with
      ECC check enabled, these functionalities are not yet supported.  


  PARAMETERS:
      IN  :  self            - pointer to the device structure
      IN  :  page            - page to read
      OUT :  data            - buffer for page data to be read
      OUT :  spare_udata     - buffer spare data to be read
      IN  :  spare_udata_len - length of the spare data to read 
      IN  :  dev_otp_params  - pointer to OTP parameter structure

  DEPENDENCIES
      Device should be probed before calling this API. Device 
      descriptor 'self' must be Non-NULL.

      OTP parameter structure should be populated with device specific
      IDs and command codes to access OTP area on the flash device.
      Access codes are made available by device manufacturers based
      on special arrangement or licensing agreement.

      This release does not support reading pages with ECC check
      enabled. ECC check should be disabled by the caller before
      attempting to read a page.

  RETURN VALUE
      FS_DEVICE_OK   : The operation completed successfully
      FS_DEVICE_FAIL : The operation failed or inputs are out of range

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*read_otp_page_and_spare) (fs_device_t self, page_id page,  void *data,
                                  void *spare_udata, uint32 spare_udata_len,
                                  void *dev_otp_params);

  /*===========================================================================
  FUNCTION OPEN_PARTITION 

  DESCRIPTION
      Open a particular partition. 

  PARAMETERS:
      IN  : self  - pointer to the device structure
      IN  : partition_name - partition to be opened 

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK                     : if successful
      FS_DEVICE_FAIL                   : if any fatal device errors
      FS_DEVICE_INVALID_DEVICE         : if device number is not supported
      FS_DEVICE_INVALID_PARTITION_NAME : if ":" is not in the correct place in 
                                         the device name
      FS_DEVICE_NO_PARTITION_TABLE     : if no partition table found
      FS_DEVICE_PARTITION_NOT_FOUND    : if partition table is found, but 
                                         requested partition is not present.

  SIDE EFFECTS
      Caller passes device structure filled in during probe, this function 
      call may modify it if successful. 
  ===========================================================================*/
  int (*open_partition) (fs_device_t self, char *partition_name);

  /*===========================================================================
  FUNCTION get_partition_phy_limits 

  DESCRIPTION
      Get Physical boundaries of a partition.

      Caller passes device descriptor. This function will return the physical
      limits of the partition on the flash device. For a NOR device, return
      values are start and end byte addresses where as for a NAND device
      return values are start and end block numbers.

  PARAMETERS:
      IN  : self  - pointer to the device structure
      OUT : start - NOR start byte address / NAND start block number  
      OUT : limit - physical limits of NOR / NAND device 
      IN  : rsrvd  - reserved pointer
     
  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK                     : if successful
      FS_DEVICE_FAIL                   : if any fatal device errors

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*get_partition_phy_limits) (fs_device_t self, uint32 *start,
                                   uint32 *limit, void *rsrvd);

  /*===========================================================================
  FUNCTION get_partition_attribs

  DESCRIPTION
      Get attributes of current partition.

      Caller passes in device descriptor. This function will return the
      attributes for the partition currently in use.

  PARAMETERS:
      IN  : self  - pointer to the device structure
      OUT : attribs - Unsigned number representing a group of attributes
                      
                      For definitions of specific bits of this number please
                      refer to definition of partition attributes in header
                      "miparti.h". 

  DEPENDENCIES
      The device should be probed before calling this API and a logical device
      must be openend. The device descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK                     : if successful
      FS_DEVICE_FAIL                   : if any fatal device errors

  SIDE EFFECTS
      None.
  ===========================================================================*/
  int (*get_partition_attribs) (fs_device_t self, uint32 *attribs);

  /*===========================================================================
  FUNCTION SET_ECC_STATE 

  DESCRIPTION
      Change the state of ECC generation. Some operations may modify
      this because they must use a particular state, but they must 
      return the state to its default when finished. 

  PARAMETERS:
      IN  : self  - pointer to the device structure
      IN  : state - ecc state to set. The possible state values are
            - FLASH_NAND_ECC_DISABLED
            - FLASH_NAND_ECC_ENABLED
            - FLASH_NAND_MAIN_AND_SPARE_ECC_ENABLED

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      FS_DEVICE_OK if the state was successfully configured/supported
      FS_DEVICE_FAIL otherwise.

  SIDE EFFECTS
      None
  ===========================================================================*/
  int (*set_ecc_state) (fs_device_t self, int state);
  
  /*===========================================================================
  FUNCTION FS_DEVICE_WRITE_STYLE 

  DESCRIPTION
      Retrieves the device write style. 

  PARAMETERS:
      IN  : self  - pointer to the device structure

  DEPENDENCIES
      The device should be probed before calling this API. The device 
      descriptor 'self' must be Non-NULL.

  RETURN VALUE
      The write style for the flash is returned.

  SIDE EFFECTS
      None
  ===========================================================================*/
  fs_device_write_style_t (*get_write_style) (fs_device_t self);

}; /* End of public interface to the flash device */

/*===========================================================================
FUNCTION FLASH_OPEN

DESCRIPTION
  Probe for the flash and open the requested partition if a partition table
  exists.

PARAMETERS:
  IN  : partition_name - name of the partition to be opened
  IN  : whichflash     - numeric identifier of the flash part 

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  Device structure is returned. In case of error, the return value will be
  NULL.

SIDE EFFECTS
  None
===========================================================================*/
fs_device_t flash_open(char *partition_name, int whichflash);

/*===========================================================================
FUNCTION FLASH_CLOSE

DESCRIPTION
  Close the flash device. This API will free the flash allocated
  device structure.

PARAMETERS:
  IN  : flash_device - pointer to the device structure 

DEPENDENCIES
  Should follow a flash_open operation.

RETURN VALUE
  FS_DEVICE_OK             : Success
  FS_DEVICE_FAILED         : An error occurred.

SIDE EFFECTS
  None
===========================================================================*/
int flash_close(fs_device_t flash_device);

#endif /* __FLASH_H__ */
