/**********************************************************************
 * flash_shimm.c
 *
 * NAND Flash device level operations
 *
 * This file implements all the functions common to all the NAND devices
 *
 * Copyright (c) 2006-2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_flash_dal_shim.c#1 $ $DateTime: 2009/01/29 17:39:06 $ $Author: richardp $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-01-19   ebb     Inherited from flash drivers.
2008-11-10   mh      Created
===========================================================================*/
#include "customer.h"

/* Hide all symbols if DAL is not used */
#ifdef FLASH_USES_DAL

#include "fs_flash_dal_shim.h"

#include "DALFramework.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "cache_mmu.h"
#include "dmov.h"

#include "mibib.h"
#include "msm.h"


/*  NAND MPU definitions for access privileges. */
#define NO_ACCESS 0x0000
#define READ_ONLY  0x0001
#define WRITE_ONLY 0x0002
#define READ_WRITE 0x0003
#define MAIN_BUFF_DESC  0
#define SPARE_BUFF_DESC 1
#define FLASH_MAX_CLIENTS 3
/****************************************************************
 * Data structure definitions
 ***************************************************************/

/****************************************************************
 * The client device structure, the client structure array and
 * current client count
 ***************************************************************/
struct flash_client_device_data
{
  struct fs_device_data flash_device_structure;
  uint32               available;
};
static struct flash_info  info;
static struct flash_client_device_data flash_device_clients[FLASH_MAX_CLIENTS];
static uint32 flash_client_count = 0;

/* Indicates whether the flash client structure array is initialized */
static uint32 flash_device_clients_initialized = FALSE;

/* DAL Memory descriptor for page operations */
static DALSYS_MEM_DESC_LIST_OBJECT (page_buff_desc, 64*2);

static DALBOOL buff_desc_initialized = FALSE;

static __inline void
flash_init_buffer_descriptor (struct flash_info *dev_info,
                              uint32 op_type, uint32 page_buff_addr,
                              int32 spare_buff_addr,
                              dalsys_mem_desc_list *buffer_desc,
                              struct flash_page_iovec  *page_data_iovec)
{
  (void) dev_info;

  DALSysMemDescBuf *main_desc_buff;
  DALSysMemDescBuf *spare_desc_buff;

  DALFW_MemDescInit (NULL, (DALSysMemDescList *) &page_buff_desc, 2);

  main_desc_buff = DALFW_MemDescBufPtr(buffer_desc, MAIN_BUFF_DESC);
  main_desc_buff->VirtualAddr = page_buff_addr;

  spare_desc_buff = DALFW_MemDescBufPtr(buffer_desc, SPARE_BUFF_DESC);
  spare_desc_buff->VirtualAddr = spare_buff_addr;

  switch (op_type)
  {
    case FLASH_READ_SPARE:
      spare_desc_buff->size = page_data_iovec->page_count *
        page_data_iovec->spare_size_bytes;
      main_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      main_desc_buff->size = 0;

      break;

    case FLASH_READ_MAIN:
      main_desc_buff->size = page_data_iovec->page_count *
        page_data_iovec->main_size_bytes;
      spare_desc_buff->VirtualAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
      spare_desc_buff->size = 0;
    break;

    case FLASH_READ_MAIN_SPARE:
      main_desc_buff->size = page_data_iovec->page_count *
        page_data_iovec->main_size_bytes;
      spare_desc_buff->size = page_data_iovec->page_count *
        page_data_iovec->spare_size_bytes;
      break;
  }
}

/****************************************************************
 * Global Data
 ***************************************************************/
/*===========================================================================
FUNCTION FLASH_INITIALIZE_DEVICE_STRUCTURE

DESCRIPTION
  Initialize the flash device structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void
flash_initialize_device_structure (void)
{
  int i=0;

  /* Initialization is only done on the first call */
  if (flash_device_clients_initialized == FALSE)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      flash_device_clients[i].available = TRUE;
    }
    flash_device_clients_initialized = TRUE;
  }
}

/*===========================================================================
FUNCTION FLASH_ALLOCATE_DEVICE_STRUCTURE

DESCRIPTION
  Allocate the flash device structure.

DEPENDENCIES
  None.

RETURN VALUE
  Available device structure. NULL if none is available.

SIDE EFFECTS
  None
===========================================================================*/
fs_device_t
flash_allocate_device_structure (void)
{
  int i = 0;
  fs_device_t available_client = NULL;

  /* Check to see if a free slot is available */
  if (flash_client_count <= FLASH_MAX_CLIENTS)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      if (flash_device_clients[i].available == TRUE)
      {
        flash_device_clients[i].available = FALSE;
        available_client = &flash_device_clients[i].flash_device_structure;
        flash_client_count++;
        break;
      }
    }
  }
  return available_client;
}

/*===========================================================================
FUNCTION FLASH_RELEASE_DEVICE_STRUCTURE

DESCRIPTION
  Release the device structure.

DEPENDENCIES
  None.

RETURN VALUE
  Frees if the device structure exists.

SIDE EFFECTS
  None
===========================================================================*/
void
flash_release_device_structure (fs_device_t device_desc)
{
  int i = 0;

  /* Check to prevent from getting overflow */
  if (flash_client_count != 0)
  {
    for (i=0; i<FLASH_MAX_CLIENTS; i++)
    {
      if (&flash_device_clients[i].flash_device_structure == device_desc &&
          flash_device_clients[i].available == FALSE)
      {
        flash_device_clients[i].available = TRUE;
        flash_client_count--;
        break;
      }
    }
  }
}


/*===========================================================================
FUNCTION FLASH_CLOSE

DESCRIPTION
  Close the flash and free the associated device structure

DEPENDENCIES
   Should always follow a flash_open

RETURN VALUE
  FS_DEVICE_OK             - Success
  FS_DEVICE_FAILED         - An error occurred.

SIDE EFFECTS
  None
===========================================================================*/
int
flash_close (fs_device_t flash_device)
{
  flash_device_close(flash_device->handle_flash);
  flash_device_detach(flash_device->handle_flash);
  flash_release_device_structure((fs_device_t)flash_device);

  return (int)FS_DEVICE_OK;
}

/***********************************************************************
FUNCTION      flash_nand_write_page

DESCRIPTION   This functions writes a page data into the given page
              on the device by using the Nand Controller

RETURN VALUE  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_write_page (fs_device_t self, page_id page, void *data)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 write_type = FLASH_WRITE_MAIN;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), write_type, (uint32) data, 0,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = flash_write_pages(self->handle_flash,
                                 (enum page_write_opcode) write_type,
                                 (DALSysMemDescList *) &page_buff_desc,
                                 &page_data_iovec, 1);

  return ret_status;
} /* End of flash_nand_write_page */

/***********************************************************************
FUNCTION      flash_nand_write_page_and_udata

DESCRIPTION   Write page data and spare user data bytes to given page with
              any hardware/software ECC for spare user data bytes.

RETURN VALUE  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_write_page_and_udata (fs_device_t self, page_id page,
  void *page_data, void *spare_udata, uint32 spare_udata_blen)
{

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 write_type = FLASH_WRITE_MAIN_SPARE;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), write_type, (uint32) page_data,
                               (uint32) spare_udata,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  ret_status = flash_write_pages(self->handle_flash,
                                 (enum page_write_opcode) write_type,
                                 (DALSysMemDescList *) &page_buff_desc,
                                 &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_write_page_and_udata */

/***********************************************************************
FUNCTION      flash_nand_erase_block

DESCRIPTION   This function erases the contents of the given block using
              the Nand Controller.

RETURN VALUE  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_erase_block (fs_device_t self, block_id block)
{
  struct flash_block_vector block_vector;
  int result_vector[1];

  block_vector.start_block = block;
  block_vector.block_count = 1;
  block_vector.result_vector =  (int *)&result_vector[0];

  return (flash_erase_blocks(self->handle_flash, &block_vector, 1));

} /* End of flash_nand_erase_block */

/***********************************************************************
FUNCTION      flash_nand_is_page_erased

DESCRIPTION   This function checks if every byte of a given page is
              erased, using the Nand Controller.

RETURN VALUE  TRUE if page is erased
              FALSE if page is not erased, or any error occurs during
                    checking
**********************************************************************/
int
flash_nand_is_page_erased (fs_device_t self, page_id page)
{

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;
  DALSysMemDescBuf *desc_buff;
  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type,
                               (uint32)(self->page_buf), 0,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  /* For this buffer which the driver has allocated we are responsible for
     setting the Physical Address and not rely of DALFW */
  desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &page_buff_desc,
                                  MAIN_BUFF_DESC);
  desc_buff->PhysicalAddr = desc_buff->VirtualAddr;

  ret_status = (flash_read_pages(self->handle_flash,
                                 (enum page_read_opcode) read_type,
                                 (DALSysMemDescList *) &page_buff_desc,
                                 &page_data_iovec, 1)  ==
                FLASH_DEVICE_FAIL_PAGE_ERASED);

  return ret_status;
} /* END flash_nand_is_page_erased */

/***********************************************************************
FUNCTION      flash_nand_is_block_erased

DESCRIPTION   This function checks if every byte of every page in a given
              block is erased, using the Nand Controller.

RETURN VALUE  TRUE if block is erased
              FALSE if block is not erased, or any error occurs during
                    checking
**********************************************************************/
int
flash_nand_is_block_erased (fs_device_t self, block_id block)
{
  page_id      start_page_no;
  uint32       index, limit, start;
  int          status = TRUE;
  uint32       pages_per_block;

  start = self->info.partition.physical_start_block;
  limit = self->info.partition.physical_start_block +
              self->info.partition.block_count;
  pages_per_block = self->info.partition.pages_per_block;

  /* If the block number passed in is out of range, return an error */
  if (block >= (limit - start))
  {
    return FALSE;
  }
  start_page_no = (page_id) (block * pages_per_block);

  for (index = 0; index < pages_per_block; index++)
  {
    if (!flash_nand_is_page_erased (self, start_page_no + index))
    {
      status = FALSE;
      break;
    }
  }

  return status;

} /* END flash_nand_is_block_erased */

/***********************************************************************
FUNCTION      flash_nand_read_page

DESCRIPTION   This function read page data of the given page into the
              given buffer using the Nand Controller.

RETURN VALUE  FS_DEVICE_DONE if read was successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_read_page (fs_device_t self, page_id page, void *data)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;

  //dcache_inval_region((void *) data, self->info.partition.page_size_bytes);

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;


  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type, (uint32) data, 0,
    (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = flash_read_pages(self->handle_flash,
                                (enum page_read_opcode) read_type,
                                (DALSysMemDescList *) &page_buff_desc,
                                &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_read_page */

/***********************************************************************
FUNCTION      flash_nand_read_spare_udata

DESCRIPTION   Read only the user data in spare location of given page.
              Data is ECC verified/corrected.

RETURN VALUE  FS_DEVICE_DONE if read was successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_read_spare_udata (fs_device_t self, page_id page, void *spare_udata,
  uint32 spare_udata_blen)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_SPARE;
  DALSysMemDescBuf *spare_desc_buff;

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = 0;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type, 0,
                               (uint32) spare_udata,
                               (DALSysMemDescList *) &page_buff_desc,
                               &page_data_iovec);

  spare_desc_buff = DALFW_MemDescBufPtr((DALSysMemDescList *) &page_buff_desc,
                                        SPARE_BUFF_DESC);
  spare_desc_buff->size  = spare_udata_blen;

  ret_status = flash_read_pages(self->handle_flash,
                                (enum page_read_opcode) read_type,
                                (DALSysMemDescList *) &page_buff_desc,
                                &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_read_spare_udata */

/***********************************************************************
FUNCTION      flash_nand_read_page_and_udata

DESCRIPTION   This function read page data and spare bytes of the given
              page into the given buffer using the Nand Controller.

RETURN VALUE  FS_DEVICE_DONE if read was successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_read_page_and_udata (fs_device_t self, page_id page, void *data,
  void *spare, uint32 spare_udata_blen)
{
  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  uint32 read_type = FLASH_READ_MAIN_SPARE;
  struct flash_page_iovec page_data_iovec;

  //dcache_inval_region((void *) data, self->info.partition.page_size_bytes);

  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;;
  page_data_iovec.spare_size_bytes = spare_udata_blen;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

    /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type,  (uint32) data,
       (uint32)spare, (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = flash_read_pages(self->handle_flash,
                                (enum page_read_opcode) read_type,
                                (DALSysMemDescList *) &page_buff_desc,
                                &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
   ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_read_page_and_udata */

/***********************************************************************
FUNCTION      flash_nand_mark_block_bad

DESCRIPTION   Clears the spare byte of the given page. This is done by
              writing the entire page with the ecc turned off.

RETURN VALUE  FS_DEVICE_OK if successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_mark_block_bad (fs_device_t self, block_id block)
{

 return (flash_block_set_state(self->handle_flash,
            block, FLASH_BLOCK_BAD));

} /* End of flash_nand_mark_block_bad */

/***********************************************************************
FUNCTION      flash_nand_read_spare_bytes

DESCRIPTION   Read len bytes from spare area starting from spare_offset
              of given page. No ECC is checked on the data read.

RETURN VALUE  FS_DEVICE_OK if successful else
              FS_DEVICE_FAIL
**********************************************************************/
int
flash_nand_read_spare_bytes (fs_device_t self, page_id page,
  uint32 spare_offset, uint32 len, void *data)
{
  (void) spare_offset;
  (void) self;

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  uint32 read_type = FLASH_READ_SPARE;
   struct flash_page_iovec page_data_iovec;
  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = 0;
  page_data_iovec.spare_size_bytes = len;
  page_data_iovec.start_page = page;
  page_data_iovec.page_count = 1;

  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type, 0,  (uint32)data,
     (DALSysMemDescList *) &page_buff_desc, &page_data_iovec);

  ret_status = flash_read_pages(self->handle_flash,
                                (enum page_read_opcode) read_type,
                                (DALSysMemDescList *) &page_buff_desc,
                                &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
    ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_read_spare_bytes */

/***********************************************************************
FUNCTION      flash_nand_lock_ops

DESCRIPTION   This function determines the offset of bad block flag, in the
              Code Word correctly based on flash width and page_size.

DEPENDENCIES  None

RETURN VALUE
              Returns FS_DEVICE_FAIL if operation fails or inputs out of range
              Returns FS_DEVICE_OK otherwise
**********************************************************************/
int
flash_nand_lock_ops (fs_device_t self, block_id block_start,
  block_id block_end, uint32 opcode)
{
  (void) self;
  (void) block_start;
  (void) block_end;
  (void) opcode;

  return FS_DEVICE_DONE;
} /* End of flash_nand_lock_ops */

/***********************************************************************
FUNCTION      flash_nand_read_mpages

DESCRIPTION   This function reads num_pages from start_page into location
              pointed to by data. All the pages read at a time SHOULD fall
              within a single good block

DEPENDENCIES  It calls the wdog_kick_fn_ptr, if not set to NULL, after
              every individual page read. This function also does not handle
              any bad block check and assumes all pages from start_page to
              (start_page + num_pages) are within a single good block.

RETURN VALUE
              Returns FS_DEVICE_FAIL if operation fails or inputs out of range
              Returns FS_DEVICE_OK otherwise
**********************************************************************/
int
flash_nand_read_mpages (fs_device_t self, page_id start_page,
                        page_id num_pages,
                        void *data, void (*wdog_kick_fn_ptr)(void))
{
  (void) wdog_kick_fn_ptr;

  int ret_status = FS_DEVICE_OK;     /* return status of read page operation */
  struct flash_page_iovec  page_data_iovec;
  uint32 read_type = FLASH_READ_MAIN;


  /* Initialize the page data IOVEC structure */
  page_data_iovec.main_size_bytes = self->info.partition.page_size_bytes;
  page_data_iovec.spare_size_bytes = 0;
  page_data_iovec.start_page = start_page;
  page_data_iovec.page_count = num_pages;


  /* Initialize buffer descriptors */
  flash_init_buffer_descriptor(&(self->info), read_type, (uint32) data, 0,
     (DALSysMemDescList *) &page_buff_desc,  &page_data_iovec);

  ret_status = flash_read_pages(self->handle_flash,
                                (enum page_read_opcode) read_type,
                                (DALSysMemDescList *) &page_buff_desc,
                                &page_data_iovec, 1);

  if (ret_status != FLASH_DEVICE_DONE)
  {
      ret_status = FS_DEVICE_FAIL;
  }
  return ret_status;
} /* End of flash_nand_read_mpages */

static int
flash_nand_bad_block_check (fs_device_t self, block_id block)
{
  int          result = FS_DEVICE_OK;    /* return status of block check */
  enum flash_block_state block_state;

  if (DAL_SUCCESS == flash_block_get_state(self->handle_flash,
                                           block, &block_state))
  {
    if (FLASH_BLOCK_BAD == block_state)
    {
      result = FS_DEVICE_BAD_BLOCK;
    }
  }
  return result;
} /* End of flash_nand_bad_block_check */

/***********************************************************************
FUNCTION      flash_nand_copy_back_write_page

DESCRIPTION   This function supports the copy back write from src_page to
              dest_page without doing any copy from NAND buffer to RAM.

DEPENDENCIES  None

RETURN VALUE
              Returns FS_DEVICE_FAIL if operation fails or inputs out of range
              Returns FS_DEVICE_OK otherwise
**********************************************************************/
int
flash_nand_copy_page (fs_device_t self, page_id src_page, page_id dest_page)
{
  struct flash_copy_iovec copy_page_iovec;

  /* If the block number passed in is out of range, return an error */
  if ((src_page >= (self->info.partition.block_count *
                    info.partition.pages_per_block)) ||
      (dest_page >= (self->info.partition.block_count *
                     info.partition.pages_per_block)))
  {
    return FS_DEVICE_FAIL;
  }

  /* if src and dest page are the same, just return operation done */
  if (src_page == dest_page)
  {
    return FS_DEVICE_OK;
  }

  copy_page_iovec.src_page = src_page;
  copy_page_iovec.dest_page = dest_page;
  copy_page_iovec.copy_count = 1  ;  /* MONA TO CHECK ON THIS */

  flash_copy_pages(self->handle_flash, &copy_page_iovec, 1);

  return FS_DEVICE_OK;
} /* End of flash_nand_copy_page */


/***********************************************************************
FUNCTION      flash_nand_read_otp_page_and_spare

DESCRIPTION   This function reads page data from OTP area of the device.
              A unique commandset required to access OTP area is not
              known to the driver and is passed in by the caller. Since
              OTP area does not have a partition associated with it, no
              strict range checking is performed.Currently this function
              only supports reading OTP area without ECC check and does
              perform reassembly of page data.

DEPENDENCIES  None

RETURN VALUE
              Returns FS_DEVICE_FAIL if operation fails or inputs out of range
              Returns FS_DEVICE_OK otherwise
**********************************************************************/
int
flash_nand_read_otp_page_and_spare (fs_device_t self, page_id page, void *data,
  void *spare, uint32 spare_blen, void *dev_otp_params)
{
  (void) self; (void) page; (void) data;
  (void) spare; (void) spare_blen; (void) dev_otp_params;

  return 0;
} /* End of flash_nand_read_otp_page_and_spare */

/***********************************************************************
FUNCTION      flash_nand_get_partition_phy_limits

DESCRIPTION   Return Physical boundaries of a partition.

RETURN VALUE  Return FS_DEVICE_FAIL if the fs_device is NULL
              Return FS_DEVICE_OK if the fs_device is not NULL and
              partition boundaries start and limit have been populated
              rsvd* is an unused pointer
**********************************************************************/
int
flash_nand_get_partition_phy_limits (fs_device_t self, uint32 *start,
  uint32 *limit, void *rsrvd)
{
  (void) rsrvd;
  int status = FS_DEVICE_OK;

   if (self->handle_flash != NULL)
   {
     *start = self->info.partition.physical_start_block;
     *limit = self->info.partition.physical_start_block +
              self->info.partition.block_count;
     status = FS_DEVICE_OK;
   }
   else
   {
     status = FS_DEVICE_FAIL;
   }
   return status;
}

/***********************************************************************
FUNCTION      flash_nand_get_partition_attribs

DESCRIPTION   This function returns partition attrubute stored
              in private data structure.

RETURN VALUE  Return FS_DEVICE_FAIL if the fs_device is NULL
              Return FS_DEVICE_OK if the fs_device is not NULL and
              variable attribute has been successfully populated.
**********************************************************************/
int
flash_nand_get_partition_attribs(fs_device_t self, uint32 *attribute)
{
  int status = FS_DEVICE_OK;

   if (self->handle_flash != NULL)
   {
     *attribute =   self->info.partition.attributes;
   }
   else
   {
     status = FS_DEVICE_FAIL;
   }
   return status;
}

/***********************************************************************
FUNCTION      Various generic functions

DESCRIPTION   Each function returns a member of the private data
              structure to the caller.  Type are various and are the
              return type of the function.  They cannot fail.
**********************************************************************/
char *
flash_nand_device_name (fs_device_t self)
{
  return  (char *)self->info.device_name;
} /* End of flash_nand_device_name */

int
flash_nand_maker_id (fs_device_t self)
{
  return  self->info.maker_id;
} /* End of flash_nand_maker_id */

int
flash_nand_device_id (fs_device_t self)
{
  return  self->info.device_id;
} /* End of flash_nand_device_id */

uint32
flash_nand_block_count (fs_device_t self)
{
  return  self->info.partition.block_count;
} /* End of flash_nand_block_count */

uint32
flash_nand_block_size (fs_device_t self)
{
  return  self->info.partition.pages_per_block;
} /* End of flash_nand_block_size */

uint32
flash_nand_page_size (fs_device_t self)
{
  return  self->info.partition.page_size_bytes;
} /* End of flash_nand_page_size */

uint32
flash_nand_total_page_size (fs_device_t self)
{
  return  self->info.partition.total_page_size_bytes;
} /* End of flash_nand_total_page_size */

uint32
flash_nand_device_type (fs_device_t self)
{

  return  self->info.type;
} /* End of flash_nand_device_type */

uint32
flash_nand_device_width (fs_device_t self)
{
  return self->info.width;
} /* End of flash_nand_device_width*/

/***********************************************************************
FUNCTION      flash_nand_get_write_style

DESCRIPTION
              Always return device write style as PAGED for NAND

**********************************************************************/
fs_device_write_style_t
flash_nand_get_write_style (fs_device_t self)
{
  (void) self;
  return FS_DEVICE_WRITES_PAGED;
}
/***********************************************************************
FUNCTION      flash_nand_get_spare_corrected_bcount

DESCRIPTION   This function returns total number of bytes available in the
              NAND spare area that can be corrected by the controller via
              some ECC algorithm, if any.

RETURN VALUE  Number of bytes of spare area that can be corrected via ECC
**********************************************************************/
int
flash_nand_get_spare_corrected_bcount (fs_device_t self)
{
  return  self->info.partition.max_spare_udata_bytes;
}
/***********************************************************************
FLASH NOR FUNCTIONS
**********************************************************************/

/***********************************************************************
FUNCTION      flash_nor_device_read_ptr

DESCRIPTION   This function returns an address pointer to a page.

DEPENDENCIES  None

RETURN VALUE  Pointer to the page beginning

SIDE EFFECTS  None
**********************************************************************/
void *
flash_nor_device_read_ptr (fs_device_t self, page_id page)
{
  (void) self; (void) page;

  return NULL;
}


/*===========================================================================

FUNCTION flash_nor_device_start_erase_block

DESCRIPTION   Starts an erase operation on a block of the flash.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if erase was successful else FS_DEVICE_FAIL

SIDE EFFECTS   An erase operation is begun upon the flash.  This operation
  must be suspended before writes can occur.

===========================================================================*/
int
flash_nor_device_start_erase_block (fs_device_t self, block_id block)
{
  (void) self; (void) block;

  return FS_DEVICE_DONE;
} /* End of fs_nor_device_start_erase_block */

/*===========================================================================
FUNCTION      flash_nor_device_resume_erase

DESCRIPTION   This function resumes the suspended erase.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_BUSY if the erase is running again.
              FS_DEVICE_FAIL if the resume failed.
===========================================================================*/
int
flash_nor_device_resume_erase (fs_device_t self)
{
  (void) self;

  return FS_DEVICE_BUSY;
}


/*===========================================================================
FUNCTION      flash_nor_device_suspend_erase

DESCRIPTION   This function suspends the current erase.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if the erase has completed early.
              FS_DEVICE_BUSY if the erase is suspended.
              FS_DEVICE_FAIL if the suspend failed.
===========================================================================*/
int
flash_nor_device_suspend_erase (fs_device_t self)
{
  (void) self;

  return FS_DEVICE_FAIL;
}

/*===========================================================================
FUNCTION      flash_nor_device_erase_block_status

DESCRIPTION   This function checks and returns the status of the
              flash device.

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if the erase has completed.
              FS_DEVICE_BUSY if the erase is still running.
===========================================================================*/
int
flash_nor_device_erase_block_status(fs_device_t self)
{
  (void) self;

  return FS_DEVICE_DONE;
}


/***********************************************************************
FUNCTION      flash_nor_device_write_partial_page

DESCRIPTION   This function writes a partial page of data

DEPENDENCIES  None

RETURN VALUE  FS_DEVICE_DONE if write was successful else FS_DEVICE_FAIL

SIDE EFFECTS  None
**********************************************************************/
int
flash_nor_device_write_partial_page (fs_device_t self, page_id page,
      void *data, int offset, int length)
{
  (void) self; (void) page; (void) data;
  (void) offset; (void) length;

  return FS_DEVICE_DONE;
}



/*===========================================================================
FUNCTION FLASH_OPEN

DESCRIPTION
  Probe for the flash and open the requested partition if a partition table
  exists.

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  Device structure is returned. In case of error, the return value will be
  NULL.

SIDE EFFECTS
  None
===========================================================================*/
fs_device_t flash_open(char *partition_name, int whichflash)
{
  (void) whichflash;
  flash_handle_t handle = NULL;
  fs_device_t fs_device = NULL;
  struct flash_info  info;

  uint32 nand_page_buf = 0;
  DALSYSMemHandle  handle_mem;   /* HAL Workspace mem handle */
  DALSYSMemInfo    mem_info;     /* HAL Workspace mem info */


  /* Initialize the client device structure array */
  flash_initialize_device_structure();
    /* Find the free slot availble */
  fs_device = flash_allocate_device_structure();

  /* Initialization is only done on the first call */
  DALSYS_InitMod(NULL);
  if(DAL_SUCCESS != flash_device_attach(DALDEVICEID_FLASH_DEVICE_1, //Device Id
                             &(fs_device->handle_flash)))           //Handle
  {
     return NULL;
  }
  handle = fs_device->handle_flash;
   //Open 'test' device...
  if(DAL_SUCCESS == flash_open_partition(handle,
                                       (const unsigned char *)partition_name))
  {
     // FLASHLOG(3, ("flash_open_partition successful\n"));
    /* Device ops table common to any NAND like device */
    /* Device Name                    */
    fs_device->device_name               = flash_nand_device_name;

    /* Maker Identification           */
    fs_device->device_maker_id           = flash_nand_maker_id;

    /* Device Identification          */
    fs_device->device_id                 = flash_nand_device_id;

    /* Block Count                    */
    fs_device->block_count               = flash_nand_block_count;

    /* Pages in block                 */
    fs_device->block_size                = flash_nand_block_size;

    /* Page Size                      */
    fs_device->page_size                 = flash_nand_page_size;

    /* Total Page Size                */
    fs_device->total_page_size           = flash_nand_total_page_size;

    /* Device Type                    */
    fs_device->device_type               = flash_nand_device_type;

    /* Device Width                    */
    fs_device->device_width              = flash_nand_device_width;

    /* Bad Block Check                */
    fs_device->bad_block_check           = flash_nand_bad_block_check;

    /* Mark Bad Block                 */
    fs_device->mark_block_bad            = flash_nand_mark_block_bad;

    /* Write Page                     */
    fs_device->write_page                = flash_nand_write_page;

    /* Erase Block                    */
    fs_device->erase_block               = flash_nand_erase_block;

    /* Read Pointer                   */
    fs_device->read_pointer              = 0;

    /* Read Page                      */
    fs_device->read_page                 = flash_nand_read_page;

    /* Is Page Erased                 */
    fs_device->is_erased                 = flash_nand_is_page_erased;

    /* Partial Write                  */
    fs_device->partial_write             = 0;

    /* Begin Erase Block              */
    fs_device->begin_erase_block         = 0;

    /* Suspend Erase                  */
    fs_device->suspend_erase             = 0;

    /* Resume Erase                   */
    fs_device->resume_erase              = 0;

    /* Erase Status                   */
    fs_device->erase_status              = 0;

    /* Is Block erased                */
    fs_device->is_block_erased           = flash_nand_is_block_erased;

    /* Read spare bytes               */
    fs_device->read_spare_bytes          = flash_nand_read_spare_bytes;

    /* Read multiple pages            */
    fs_device->read_mpages               = flash_nand_read_mpages;

    /* Copy page                      */
    fs_device->copy_page                 = flash_nand_copy_page;

    /* Lock Unlock LockTight          */
    fs_device->lock_ops                  = flash_nand_lock_ops;

    /* Write Page & Spare udata       */
    fs_device->write_page_and_udata      = flash_nand_write_page_and_udata;

    /* Read spare user data           */
    fs_device->read_spare_udata          = flash_nand_read_spare_udata;

    /* Read Page and spare user data  */
    fs_device->read_page_and_udata       = flash_nand_read_page_and_udata;

    /* Read OTP page and spare data   */
    fs_device->read_otp_page_and_spare   = flash_nand_read_otp_page_and_spare;

    /* Read OTP page and spare data   */
    fs_device->open_partition            = NULL;

    /* Get partition physical limits  */
    fs_device->get_partition_phy_limits  = flash_nand_get_partition_phy_limits;

    /* Get partition attributes       */
    fs_device->get_partition_attribs     = flash_nand_get_partition_attribs;

    /* Set ECC state */
    fs_device->set_ecc_state             =  0;

    /* Get write style                */
    fs_device->get_write_style           = flash_nand_get_write_style;

    fs_device->get_spare_corrected_bytes =
      flash_nand_get_spare_corrected_bcount;

    (void)flash_get_info(handle, FLASH_DEVICE_INFO,  &info);
    fs_device->info = info;

     if (DAL_SUCCESS == DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
        DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED,
        DALSYS_MEM_ADDR_NOT_SPECIFIED, 2112,
        &handle_mem, NULL))
      {
        /* If the allocation is success, get the memory address
           information */
        DALSYS_MemInfo(handle_mem, &mem_info);

        nand_page_buf = (uint32) mem_info.VirtualAddr;
        fs_device->page_buf = (uint8 *) nand_page_buf;
        /*  Need to touch the memory  for it  to be configured in
            MMU table for ARM11 */
        DALSYS_memset(nand_page_buf, 0xFF, 10);
      }

      /* need to have at least two entries in buff desc: one for main and the
       * other for spare */
      if (FALSE == buff_desc_initialized)
      {
       DALFW_MemDescInit(NULL, (DALSysMemDescList *) &page_buff_desc, 2);
       buff_desc_initialized = TRUE;
      }

       return (fs_device_t) fs_device;
  }
  else
  {
     return NULL;
  }

}

#else
extern int __dont_complain_about_empty_file;
#endif /* FLASH_USES_DAL */
