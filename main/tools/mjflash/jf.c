/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32

GENERAL DESCRIPTION

  This program is essentially a plug-in for Lauterbach JTAG ICD TRACE32
  Debugger FLASH command. The programming can be achieved in two different 
  modes: emulator controlled or target controlled. The emulator controlled
  programming needs no program running on the target, but is slow
  (typically about 1 to 5 KBytes/sec). The target controlled programming 
  requires a user designed FLASH programming routine (and hence this plug-in)
  to be available on the target. This plug-in is called by TRACE32 with 
  appropriate parameters to program FLASH devices. The advantage of this
  method is the higher programming speed and more flexibility in the 
  implementation of the programming algorithm.

  To use the plug-in, FLASH command has the FLASH.TARGET command.
  For example: (this assumes the programming voltage is setup beforehand)

    ; reset our flash command
    flash.reset
    ; let flash command know where our routine and buffer is located
    ; code at 0x1000000, data at 0x1008000, buffer size = 64K
    flash.target 1000000 1008000 10000 
    ; define our flash range and tell flash command we're using
    ; our own flash programming routine via keyword "target"
    flash.create 0--0FFFFF target word
    ; first erase our flash
    flash.erase 0--0FFFFF

    ; put into program mode and do it
    flash.program all
    ; the actual programming of FLASH, this will call our plug-in
    d.load.aif "surf.aif" 
    ; ok we're finished, turn off programming mode
    flash.program off

  In order for the Debugger to get control back after the plug-in completes,  
  a breakpoint is assumed to be in place after the last instruction
  executed in the plug-in.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Must place a breakpoint right after the last instruction in the plug-in
  in order for the Debugger to get back control.

  Copyright (c) 2006-2008 Qualcomm Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/jf.c#11 $ $DateTime: 2008/06/09 12:32:26 $ $Author: c_surven $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/08   sv      Fix RVCT compiler warning 
04/21/08   sv      Remove banned API; check for buffer overflow in helper funcs 
04/18/08   sv      Fix lint errors - banned api access, range overflow
03/19/08   sv      Modified to use offset based addr and JFLASH return codes 
03/09/08   sv      Adapt to use flash_open() API 
02/28/08   op      call do_probe to get nor info
01/03/08   op      Add support for single-image NOR targets
09/17/07   op      Add support for Sibley Nor Intel
02/28/07   rt      Add support for multi-image NOR targets
12/06/06   rt      Create initial version from JFLASH 2.1
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "jfi.h"
#include "flash_nor_device.h"
#include "flash_nor_msm.h"
#include "jflash_ext_error.h"
#include "miboot.h"
#include "miparti.h"
#include "mibib.h"
#include "string.h"
#include "flash_partition.h"
#include "flash_nor_partition.h"
#include "flash_nor.h"
#ifdef BUILD_FOR_X86
  #include <stdlib.h>
#endif

/*===========================================================================

                           DEFINITIONS

===========================================================================*/

/* This structure is used to communicate between on target programmer and
 * TRACE32. */
extern struct type_jtag_flash_param jtag_flash_param;

/* Main stack */
dword svc_stack[0x4000/sizeof(dword)];

char * parti_buf[JFLASH_PARTI_BSIZE];
char * flash_parti_buffer[JFLASH_PARTI_BSIZE];

volatile word * nor_partition_ptr;

flash_partable_t sys_parti_buffer = (flash_partable_t) &parti_buf[0];
flash_usr_partable_t usr_parti_buffer = 
  (flash_usr_partable_t) &parti_buf[NOR_FLASH_PARTI_TBL_LENGTH];

/* Device structure and pointer to the structure*/
struct fsi_device_data flash_dev;
struct fsi_device_data * mjflash_dev = NULL;
flashi_nor_device * nor_device;

/* Caches the current flash sector size bytes of the sector being erased  */
/* If FLASH_CHECK defined in flash driver, this variable gets used
   to verify the sector has been erased */
dword curr_erase_sector_bsize;


/* Forward Function Prototypes */

static uint32 flash_program(void);
static uint32 flash_update_device_info(void);
static uint32 flash_erase_chip(void);
static uint32 flash_init_usr_parti (void);
static uint32 flash_init_sys_parti (void);
static uint32 flash_init_boot_vec_tbl (void);
static uint32 flash_init_cfg_data_qcsbl(void);
static uint32 flash_init_oemsbl(void);
static uint32 flash_init_amss(void);
static uint32 flash_init_all(void);
static uint32 flash_init_efs(void);
static uint32 init_for_partition_image (char *pname);
static uint32 flash_finalize(void);

#ifdef BUILD_FOR_X86
  static uint32 jflash_strcpy(char *dir_name, const char *filepath, 
                              uint32 dir_name_buf_sz);
  static uint32 jflash_makepath(char *dir_name, const char *filepath,
                                const char *filename, uint32 dir_name_buf_sz);
  static uint32 jflash_strcat(char *dest_str, const char *src_str, 
                              uint32 dest_buf_size);
#endif

uint32 global_count = 0;

block_descriptor_type flash_block[FSD_MAX_SECTORS];
uint32 total_sectors=0;

static uint32 partition_rcvd = FALSE;
static uint32 partition_table_committed = FALSE;
static uint32 flash_has_parti_tbl = FALSE;
static uint32 hw_present = FALSE;

struct parti_params {
  uint32 start_addr_offset;
  uint32 end_addr_offset;
  uint32 length;
  uint16 start_blk;
  uint16 end_blk;
  uint16 num_blks;
};

struct parti_params parti_info;

/*===========================================================================

FUNCTION main_c

DESCRIPTION
  This function calls the appropriate routine based on what TRACE32 told us.

DEPENDENCIES
   Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

int main_c(void)
{
   uint32 ret_code = JFLASH_FAIL;   

   switch ( jtag_flash_param.status )
   {
     case FLASH_USR_PARTI_TBL:
       ret_code = flash_init_usr_parti();
       break;

     case FLASH_SYS_PARTI_TBL:
       ret_code = flash_init_sys_parti();
       break;

     case FLASH_INIT_BOOT_VEC_TBL:
       ret_code = flash_init_boot_vec_tbl();
       break;

     case FLASH_INIT_QCSBL:
       ret_code = flash_init_cfg_data_qcsbl();
       break;

     case FLASH_INIT_OEMSBL:
       ret_code = flash_init_oemsbl();
       break;

     case FLASH_INIT_AMSS:
       ret_code = flash_init_amss();
       break;
              
     case FLASH_INIT_ALL:
       ret_code = flash_init_all();
       break;
   
     case FLASH_INIT_CEFS:
       ret_code = flash_init_efs();
       break;
              
     case FLASH_PROGRAM:
       ret_code = flash_program();
       break;
      
     case FLASH_ERASE_CHIP:
        ret_code = flash_erase_chip();
        break;
     
     case FLASH_FINALIZE:
        ret_code = flash_finalize();
        break;
         
     case FLASH_ERASE:
     case FLASH_ERASE5:
        /* 
         * Accept both 2 and 5 since Lauterbach JTAG sometimes passes
         * 2 and sometimes passes 5 to indicate erase, depending
         * on revision of Lauterbach software.
         *
         *  NOTE:  We no longer do an erase by itself, so we now take
         *         the erase code to be failure
         */
         ret_code = JFLASH_FAIL;
         break;
     default:
       ret_code = JFLASH_FAIL;
       break;
   }

   /* return error code back to TRACE32, 0 = success, 0x100 = fail */
   jtag_flash_param.status = ret_code;
   return ret_code;

} /* main_c */



/*===========================================================================

FUNCTION flash_update_device_info

DESCRIPTION
  This function fills flash device geometry data in a static structure
  which is used by the other routines.

DEPENDENCIES
  none

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/
uint32 
flash_update_device_info(void)
{
  uint32 status = JFLASH_FAIL;

  if (nor_device != FLASH_NO_DEVICE)
  {
    word count;
    dword xcount, curraddr = 0;
    int32 sector_indx = 0;
    int32 bank_end = -1, total_banks, bank_indx = -1;
  
    status = (uint32) JFLASH_SUCCESS;
        
    total_banks = nor_device->geometry->num_banks;

    /* Iterate through the geometry structure to fill the block info
     * structure.  Outer loop index keeps track of erase block regions 
     * and the inner loop index keeps track of the number of blocks in 
     * each region.
     */ 
    for (count = 0; (count < nor_device->geometry->num_erase_blk_region) && \
                    (status == JFLASH_SUCCESS); count++)
    {
      for (xcount = 0; xcount < 
           nor_device->geometry->blk_regions[count].numblocks; xcount++)
      {
        if (sector_indx > bank_end)
        {
          if (++bank_indx >= total_banks)
          {
            DPRINTF(("fudi: Bank indx 0x%x exceeded bank count 0x%x", bank_indx,
                      total_banks));
            status = JFLASH_FAIL;
            break; 
          }
          bank_end = nor_device->geometry->bank_info[bank_indx].end_block;
        }
        flash_block[sector_indx].start = curraddr;
        flash_block[sector_indx].len = 
          nor_device->geometry->blk_regions[count].blocksize_bytes;
        flash_block[sector_indx].is_erased = FALSE;
              
        curraddr += flash_block[sector_indx].len;
        sector_indx++;
        KICK_WATCHDOG();
        CHECK_FOR_DATA ();
      }        
    } /* for (count = 0; count... */
    total_sectors = sector_indx;
  } /* if (nor_device != FLASH_NO_DEVICE) */
  return status;
}  /* flash_update_device_info */




/*===========================================================================

FUNCTION flash_program

DESCRIPTION
  This function is a wrapper for the real flash write routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else return JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

uint32 flash_program(void)
{
   byte *data;
   dword addr, write_addr_offset;
   flash_ptr_type baseaddr;
   word  size;
   word start_blk = parti_info.start_blk;
   word end_blk = 0;
   word roof_blk = parti_info.num_blks + parti_info.start_blk;
   dword tmp;
   int i = 0;
   flash_status status;

   data = jtag_flash_param.data + jtag_flash_param.offset;
   addr = jtag_flash_param.addr + jtag_flash_param.offset - \
          jtag_flash_param.flashaddr;
   size = jtag_flash_param.size;
   
   baseaddr = (flash_ptr_type)nor_device->base_addr;

   TPRINTF (4, ("fpgm: jata 0x%x jaddr 0x%x joff 0x%x\n", jtag_flash_param.data,
                jtag_flash_param.addr, jtag_flash_param.offset));
   TPRINTF (3, ("fpgm: data 0x%x addr 0x%x base 0x%x len 0x%x\n", data, addr,
                 baseaddr, size));

   KICK_WATCHDOG();
   CHECK_FOR_DATA();     /* Just to be sure */
 
   /* bail out if zero length write */
   if (size == 0)
   {
     DPRINTF (("fpgm: Invalid size '0' provided by T32 \n"));
     return JFLASH_FAIL;
   }

   if (addr >= parti_info.length)
   {
     DPRINTF (("fpgm: Addr provided by T32 is beyond device limit\n"));
     return JFLASH_FAIL;
   }
 
   /* Determine which sector this write is supposed to be in.
    * It may begin in a sector which is already erased, in which
    * case, we want to bump to the next block.
    */
   tmp = 0;
   while (start_blk < roof_blk)
   {
     tmp += flash_block[start_blk].len;
     if (tmp > addr)
       break;
     start_blk++;
   }
 
   if (start_blk == roof_blk)
   {
     DPRINTF (("fpgm: Start addr provided by T32 is beyond device limit\n"));
     return JFLASH_FAIL;
   }
 
   TPRINTF (3, ("fpgm: block st 0x%x\n", start_blk));
 
   end_blk = start_blk;
   while (end_blk < roof_blk)
   {
     if ((addr + size - 1) < tmp)
       break;
     tmp += flash_block[end_blk].len;
     end_blk++;
   }
 
   KICK_WATCHDOG();
   CHECK_FOR_DATA ();
 
   if (end_blk == roof_blk)
   {
     DPRINTF (("fpgm: End addr provided by T32 is beyond device limit\n"));
     return JFLASH_FAIL;
   }

   TPRINTF (3, ("fpgm: block end 0x%x\n", end_blk));
 
   /* Call erase function on blocks to erase */
   for (i = start_blk; i <= end_blk; i++)
   {
      
     if (flash_block[i].is_erased == FALSE)
     {

       curr_erase_sector_bsize = flash_block[i].len;
       
       TPRINTF (3, ("fpgm: Erasing block 0x%x\n", flash_block[i].start));
       status = nor_device->ops->erase_start(baseaddr, flash_block[i].start);

       if (status != FLASH_SUCCESS)
       {
         return JFLASH_FAIL;
       }

       do
       { 
         status = nor_device->ops->erase_status(nor_device, baseaddr + 
           BYTE_TO_WORD_OFFSET(flash_block[i].start));
         KICK_DOG_AND_CHECK_DATA();
       } while (status == FLASH_OP_NOT_COMPLETE);
  
       if (status != FLASH_SUCCESS)
       {
         return JFLASH_FAIL;
       }
       else
       {
         flash_block[i].is_erased = TRUE;
       }
       
     }
   }
 
   /* Block is ready to write, do the write */
   /* Calculate the write address offset which is the offset from the start of
    * the partition 
    */
   write_addr_offset = addr + parti_info.start_addr_offset;
   status = nor_device->ops->write (data, baseaddr, write_addr_offset, size);

   if (status != FLASH_SUCCESS)
   {
     DPRINTF (("fpgm: write failed\n"));
     return JFLASH_FAIL;
   }
   else
   {
     TPRINTF(3, ("fpgm: complete\n"));
     return JFLASH_SUCCESS;
   }
 
} /* flash_program */



/*===========================================================================

FUNCTION flash_erase_chip

DESCRIPTION
  This function is a wrapper for the real flash erase chip routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

uint32 flash_erase_chip(void)
{
   flash_ptr_type baseaddr;
   flash_status status = FLASH_SUCCESS;
   dword i;

   hw_present = jtag_flash_param.width;

   if (init_for_partition_image(MIBIB_DEVICE_ALL_PARTI_NAME) != JFLASH_SUCCESS)
   {
     DPRINTF (("Error: no nor_device found \n"));
     SET_ERR (ERR_DEV_MISSING);
     return JFLASH_FAIL;
   }

   baseaddr = nor_device->base_addr;
   /* Erase every block in the flash, one at a time */
   for (i = 0; i < total_sectors; i++)
   {
     KICK_WATCHDOG();
 
     TPRINTF (3, ("fec: block 0x%x\n", i));

     curr_erase_sector_bsize = flash_block[i].len;
 
     status = nor_device->ops->erase_start(baseaddr, flash_block[i].start);

     if (status != FLASH_SUCCESS)
     {
       return JFLASH_FAIL;
     }

     do
     { 
       status = nor_device->ops->erase_status(nor_device, baseaddr + 
         BYTE_TO_WORD_OFFSET(flash_block[i].start));
       KICK_DOG_AND_CHECK_DATA();

     } while (status == FLASH_OP_NOT_COMPLETE);
 
     if (status != FLASH_SUCCESS)
     {
       return JFLASH_FAIL;
     }
   }
 
   return JFLASH_SUCCESS;

}  /* flash_erase_chip */


/*===========================================================================

FUNCTION flash_init_usr_parti

DESCRIPTION
  This function receives the user partition table to be used for subsequent
  programming.  It validates that it is not different from the one stored
  on flash, if one exists.  It will fail if the two are different and the
  override parameter is not given in the command buffer.
  
  The device structure pointer that is used here to begin with, allows
  the flash drivers to see the entire flash.  So, we temporarily stuff
  the limits with a default value so that we do not go into the second
  partition.

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/
static uint32 
flash_init_usr_parti()
{
  flash_usr_partable_t jtag_parti_ptr;
  flash_usr_partable_t flash_usr_parti_ptr = NULL;
  flash_partable_t sys_parti_ptr;
  flash_partable_t flash_sys_parti_ptr = 0;
  uint32 usr_parti_matches = FALSE;
  uint32 sys_parti_matches = FALSE;
  flash_status status;
  flash_ptr_type baseaddr = NULL;

  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Receive User Partition Table ------\n"));

  jtag_parti_ptr = (flash_usr_partable_t) jtag_flash_param.data;
  sys_parti_ptr = (flash_partable_t) parti_buf;
  hw_present = jtag_flash_param.width;

  /* Sanity check that this is a valid user parition table */
  if ( (jtag_parti_ptr->magic1 != (uint32)FLASH_USR_PART_MAGIC1) ||
      (jtag_parti_ptr->magic2 != (uint32)FLASH_USR_PART_MAGIC2) ||
      (jtag_parti_ptr->version != (uint32)FLASH_PARTITION_VERSION) ||
      (jtag_parti_ptr->numparts > (uint32)FLASH_NUM_PART_ENTRIES))
  {
    SET_ERR(ERR_USR_PARTI_NOMATCH);
    return JFLASH_FAIL;
  }

  /* This flag indicates whether we need to update parition table on flash. For
   * now assume that it does not need to be updated.
   */
  partition_table_committed = TRUE;
 
  if (init_for_partition_image(MIBIB_DEVICE_ALL_PARTI_NAME) != JFLASH_SUCCESS)
  {
    DPRINTF (("Error: no nor_device found \n"));
    SET_ERR (ERR_DEV_MISSING);
    return JFLASH_FAIL;
  }
  
  baseaddr = nor_device->base_addr;

  KICK_WATCHDOG();

  if (hw_present == TRUE)
  {

    /* Since there are no bad blocks in NOR, we can use a well known fixed block
     * offset to find the partition table
     */
    flash_sys_parti_ptr =
          (flash_partable_t)flash_block[MI_BLOCK_PARTI_TBL].start;
    flash_usr_parti_ptr = (flash_usr_partable_t)((byte *) flash_sys_parti_ptr + 
                          NOR_FLASH_PARTI_TBL_LENGTH);

    TPRINTF(3,("\n MIPIB sys partition ptr = 0x%x ::", flash_sys_parti_ptr));
    TPRINTF(3,("\n MIPIB usr partition ptr = 0x%x ::\n", flash_usr_parti_ptr));

    /* Find out if user and system partition tables are written. Can can only
     * rely on magic numbers to find out if they are really written. Both should
     * be present because they are written together. If only one is found, there 
     * is an error.
     */

    if((flash_sys_parti_ptr->magic1 == (uint32)FLASH_PART_MAGIC1)      &&
       (flash_sys_parti_ptr->magic2 == (uint32)FLASH_PART_MAGIC2)      &&
       (flash_usr_parti_ptr->magic1 == (uint32)FLASH_USR_PART_MAGIC1)  && 
       (flash_usr_parti_ptr->magic2 == (uint32)FLASH_USR_PART_MAGIC2))
    {
      flash_has_parti_tbl = TRUE;
      usr_parti_matches = flash_usr_partition_tables_are_equal (jtag_parti_ptr,
                                                 flash_usr_parti_ptr);

      if (usr_parti_matches == FALSE)
      {
        DPRINTF (("\n\nUser Partition Table does not match w/ one on flash\n"));

        /* Check to see if we have override flag */
        if (jtag_flash_param.reserved != FLASH_PARTI_OVERRRIDE)
        {
          /* Fail, user partition tables are not equal and no override */
          DPRINTF(("Partition Tables do not match\n"));
          SET_ERR (ERR_PARTI_NOMATCH);
          return JFLASH_FAIL;
        }
        else
        {
          flash_has_parti_tbl = FALSE;

          /* Erase partition table */
          status = nor_device->ops->erase_start(baseaddr,
                                                 (dword) flash_sys_parti_ptr);
            
          if (status == FLASH_SUCCESS)
          {
            do
            { 
              status = nor_device->ops->erase_status(nor_device, baseaddr + 
                BYTE_TO_WORD_OFFSET((dword)flash_sys_parti_ptr));
              KICK_DOG_AND_CHECK_DATA();
            } while (status == FLASH_OP_NOT_COMPLETE);
          }
          else
          {
            /* Fail. Partition table cannot be erased */
            DPRINTF(("Could not erase partition table\n"));
            return JFLASH_FAIL;
          }
          /* If we got an override flag, we will need to write the new partition
           * table into flash. Set up a flag to remember we do this in finalize
           */
          partition_table_committed = FALSE;
        }
      }
    }
    else
    {

      /* Erase partition table */
      status = nor_device->ops->erase_start(baseaddr,
                                            (dword) flash_sys_parti_ptr);

      if (status == FLASH_SUCCESS)
      {
        do
        { 
          status = nor_device->ops->erase_status(nor_device, baseaddr + 
            BYTE_TO_WORD_OFFSET((dword)flash_sys_parti_ptr));
          KICK_DOG_AND_CHECK_DATA();
        } while (status == FLASH_OP_NOT_COMPLETE);
      }
      else
      {
        /* Fail. Partition table cannot be erased */
        DPRINTF(("Could not erase partition table\n"));
        return JFLASH_FAIL;
      }
      /* There is no partition tbl on flash. We need to write the new partition
       * table into flash. Set up a flag to remember we do this in finalize
       */
      partition_table_committed = FALSE; 
    }
  }

  /* Initialize the whole buffer so that no garbage gets written on to the
   * flash.
   */
  memset(&parti_buf[0], 0xFF, (NOR_FLASH_PARTI_TBL_LENGTH * 2));

  if (!flash_nor_convert_usr_parti_tbl_to_sys((fs_device_t) mjflash_dev,
                                              jtag_parti_ptr,
                                              sys_parti_ptr, flash_block))
  {
    DPRINTF(("Error:  Partition table conversion failed\n"));
    SET_ERR(ERR_PARTI_CNV_FAIL);
    return JFLASH_FAIL;
  }

  /* If there was a good partition table on flash compare the newly generated
   * system partition table with the one on flash.
   */
  if ((flash_has_parti_tbl == TRUE) && (usr_parti_matches == FALSE))
  {
    sys_parti_matches = flash_partition_tables_are_equal (sys_parti_ptr,
                                               flash_sys_parti_ptr);

    if (sys_parti_matches == FALSE)
    {
      /* Fail, System partition tables are not equal and no override */
      DPRINTF(("System Partition Tables do not match\n"));
      SET_ERR (ERR_PARTI_NOMATCH);
      return JFLASH_FAIL;
    }
  }

#ifdef PRINT_PARTITION_TABLE
  pdump ((void*)jtag_parti_ptr, NOR_FLASH_PARTI_TBL_LENGTH);
  print_usr_partition_table (jtag_parti_ptr);
  print_partition_table (sys_parti_ptr);
#endif

  KICK_WATCHDOG();

  /* If the conversion succeeded then copy the user parition table from JTAG
   * data area to the internal buffer caching the newly generated system
   * partition table. The system patition table should alread be place in the
   * buffer by the partition conversion routine.
   */


  KICK_WATCHDOG();
  if ((sizeof(struct flash_partition_table) > NOR_FLASH_PARTI_TBL_LENGTH) || 
       (sizeof(struct flash_usr_partition_table) > NOR_FLASH_PARTI_TBL_LENGTH))
  {
    DPRINTF(("Error: Partition Tbl Structures cannot exceed allocated size\n"));
    SET_ERR(ERR_PARTI_TOO_BIG);
    return JFLASH_FAIL;
  } 

  TPRINTF(3,("Copying user partition table from RAM to oldest\n"));
  memcpy((void *)((char *)(sys_parti_ptr) + NOR_FLASH_PARTI_TBL_LENGTH),
                 (void *)jtag_parti_ptr,   
          sizeof(struct flash_usr_partition_table));

  /* Indicate that we have a valid partition table and set up to use
   * the correct partitions for QCSBL and OEMSBL images.
   */
  partition_rcvd = TRUE;
  
  KICK_WATCHDOG();

  TPRINTF(3,("\n\n"));
  TPRINTF (3,("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;
} // flash_init_usr_parti


/*===========================================================================

FUNCTION flash_init_sys_parti

DESCRIPTION
  This function receives the system partition table to be used for subsequent
  programming.  It validates that it is not different from the one stored
  on flash, if one exists.  It will fail if the two are different and the
  override parameter is not given in the command buffer.
  
  The device structure pointer that is used here to begin with, allows
  the flash drivers to see the entire flash.  So, we temporarily stuff
  the limits with a default value so that we do not go into the second
  partition.

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/
static uint32 
flash_init_sys_parti()
{
  flash_partable_t jtag_parti_ptr;
  flash_partable_t sys_parti_ptr;
  uint32 sys_parti_matches = FALSE;

  KICK_WATCHDOG();

  DPRINTF (("\n\n------ Receive System Partition Table ------\n"));

  jtag_parti_ptr = (flash_partable_t) jtag_flash_param.data;
  sys_parti_ptr = (flash_partable_t) parti_buf;

  /* Sanity check that this is a valid user parition table */
  if ((jtag_parti_ptr->magic1 != (uint32)FLASH_PART_MAGIC1)        ||
      (jtag_parti_ptr->magic2 != (uint32)FLASH_PART_MAGIC2)        ||
      (jtag_parti_ptr->version != (uint32)FLASH_PARTITION_VERSION) ||
      (jtag_parti_ptr->numparts > (uint32)FLASH_NUM_PART_ENTRIES))
  {
    DPRINTF (("\n\nMagic or version number mismatch\n"));
    return JFLASH_FAIL;
  }

  KICK_WATCHDOG();

#ifdef PRINT_PARTITION_TABLE
  pdump ((void*)sys_parti_ptr, NOR_FLASH_PARTI_TBL_LENGTH);
  print_partition_table (sys_parti_ptr);
#endif

  sys_parti_matches = flash_partition_tables_are_equal (sys_parti_ptr,
                                                        jtag_parti_ptr);

  if (sys_parti_matches == FALSE)
  {
    /* Fail, System partition tables are not equal and no override */
    DPRINTF(("Compile time system Partition Table\n"));
    DPRINTF(("does not match with runtime version\n"));
    SET_ERR (ERR_PARTI_NOMATCH);
    return JFLASH_FAIL;
  }

  KICK_WATCHDOG();

  TPRINTF(3,("\n\n"));
  TPRINTF (3,("===========  Done ===========\n\n"));


  return JFLASH_SUCCESS;
} // flash_init_sys_parti



/*===========================================================================

FUNCTION flash_init_boot_vec_tbl

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for AMSS Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.
  Partition table must have been received

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_init_boot_vec_tbl(void)
{
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for BOOT VECTOR TABLE Image ------\n"));

  if (!partition_rcvd)
  {
    DPRINTF (("Error: no partition table sent yet\n"));
    SET_ERR (ERR_PARTI_NOT_RCVD);
    return JFLASH_FAIL;
  }
  
  /* Open the partition to use for this image */
  if ((init_for_partition_image (MIBIB_NOR_BOOT_PARTI_NAME)) != JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_NO_BVT);
    return JFLASH_FAIL;
  }
  KICK_WATCHDOG();

#ifdef DUMP_AMSS_HEADER
  /* print out data area as it contains the AMSS header */
  TPRINTF(3,("\nDump of AMSS header from parameter block\n\n"));
  pdump ((char *)jtag_flash_param.data, MAX_PAGE_SIZE);
  TPRINTF(3,("\n\n"));
  KICK_WATCHDOG();
#endif

  DPRINTF (("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;

} // flash_init_boot_vec_tbl



/*===========================================================================

FUNCTION init_for_partition_image

DESCRIPTION
  This function opens the device, then the named partition and sets up
  device structure to use that partition.

DEPENDENCIES
  NAND device already opened successfully

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS
  Updates block start and limit in the image structure

===========================================================================*/
static uint32
init_for_partition_image (char *pname)
{
  flash_partable_t sys_parti_ptr = sys_parti_buffer;
  word start_blk = 0, end_blk = 0;
  dword tmp, addr;

  KICK_WATCHDOG();
  TPRINTF(3,("ifpi:  trying to find partition %s\n", pname));

  /* Open QCSBL Partition using RAM copy of partition table.
   * The partition table has been sent and accepted, whether 
   * it matched, or we had an override and erased the flash.
   * This function call will update the start and limit values
   * if the partition is present and valid in the RAM copy of
   * the partition table.
   */
  KICK_WATCHDOG();

  if (strncmp(pname, MIBIB_DEVICE_ALL_PARTI_NAME, FLASH_PART_NAME_LENGTH) == 0)
  {
    if (mjflash_dev == NULL)
    {
      mjflash_dev =(fsi_device_t) flash_open(MIBIB_DEVICE_ALL_PARTI_NAME, 0);
      if (mjflash_dev != NULL)
      {
        /* Perform a dummy copy operation to fool the compiler */
        save_idtable();
        nor_device = &(mjflash_dev->nor_dev_info);     
        if (nor_device != NULL)
        {
          if (flash_update_device_info() == JFLASH_FAIL)
          {
            DPRINTF (("Error: Update device info failed \n"));
            flash_close ((fs_device_t)mjflash_dev);
            mjflash_dev = NULL;
            nor_device = NULL;
            return JFLASH_FAIL;
          }
        }
        else /* nor_device == NULL*/
        {   
          DPRINTF (("Error: Bad device descriptor found\n"));
          SET_ERR (ERR_BAD_DEVICE);
          flash_close ((fs_device_t)mjflash_dev);
          mjflash_dev = NULL;
          return JFLASH_FAIL;
        }
      }
      else /* mjflash_dev == NULL */
      {
        DPRINTF (("Error: Bad device descriptor found\n"));
        SET_ERR (ERR_BAD_DEVICE);
        return JFLASH_FAIL;
      }
    }
    else /* mjflash_dev != NULL */
    {
      /* Update the device info */
      if (nor_device != NULL)
      {	    
        if (flash_update_device_info() == JFLASH_FAIL)
        {
          DPRINTF (("Error: Update device info failed \n"));
          return JFLASH_FAIL;
        }
      }
      else
      {  
        return JFLASH_FAIL;
      } 
       return JFLASH_SUCCESS;
    } /* mjflash_dev != NULL */  
  }
  else
  {
    /* The partition table is available in a global buffer. For now just 
     * RAM copy. Later on this should be read from the flash and then a call to
     * open_parition should be used to find the required bounds.
     */
    if(mjflash_dev == NULL)
    {
      DPRINTF (("Error: Corrputed Device descriptor found \n"));
      SET_ERR (ERR_BAD_DEVICE);
      return JFLASH_FAIL;
    }
  
    if ((flash_nor_update_limits((fs_device_t)mjflash_dev, sys_parti_ptr, 
                                 pname)) != TRUE)
    {
      DPRINTF (("Error: Could not find partition %s\n", pname));
      return JFLASH_FAIL;
    }
  }

  if (mjflash_dev != (fsi_device_t) 0)
  {
    /* We have found the partition, update the device limits in the 
     * caller's data structure.  
     * Ensure that we did not go beyond the physical limits of the
     * the device.  This is because we allow the last partition
     * defined to grow to the device size by giving it a
     * special value.  This code ensures that we remain
     * within the actual device limits.
     */

    parti_info.start_addr_offset = mjflash_dev->partition_start_baddr;
    TPRINTF(3, ("fnul:  Set partition start to requested address 0x%x\n",
                parti_info.start_addr_offset));
    
    parti_info.end_addr_offset = mjflash_dev->partition_limit_baddr;
    parti_info.length = parti_info.end_addr_offset - \
	                  parti_info.start_addr_offset;
  
    TPRINTF(3, ("fnul:  Set partition limit to requested address 0x%x\n",
                 parti_info.end_addr_offset));

  }
  else
  {
    TPRINTF(3, ("fnul:  Did not get a partition entry match for %s\n", pname));
    return JFLASH_FAIL;
  }


  /* Determine which block these addresses are supposed to be in.
   * These sectors should begin and end on block boundary only. If not
   * the partition table conversion did not happen correctly.
   */
   /* Start our search from the base of flash */
  tmp = 0;
  addr =  parti_info.start_addr_offset;

  while (start_blk < total_sectors)
  {
    if (tmp == addr)
      break;
    tmp += flash_block[start_blk].len;
    start_blk++;
  }

  if (start_blk == total_sectors)
  {
    DPRINTF (("fpgm: Invalid partition start data in partition table\n"));
    return JFLASH_FAIL;
  }
  else
  {
    parti_info.start_blk = start_blk;
  }

  TPRINTF (3, ("fpgm: block st 0x%x\n", start_blk));

  addr =  parti_info.end_addr_offset;
  end_blk = start_blk;

  while (end_blk < total_sectors)
  {
    tmp += flash_block[end_blk].len;
    if (addr == tmp)
      break;
    end_blk++;
  }

  KICK_WATCHDOG();
  CHECK_FOR_DATA ();

  TPRINTF (3, ("fpgm: block end 0x%x\n", end_blk));

  if (end_blk == total_sectors)
  {
    DPRINTF (("fpgm: Invalid partition end data in partition table\n"));
    return JFLASH_FAIL;
  }
  else
  {


    /* This code is just to make sure that the translations done by the
     * driver routines match up with the ones calculated by tools code.
     * If they do not fail to catch the problem. These checks will be taken
     * out eventually.
     */

    if(start_blk != mjflash_dev->partition_blk_start)
    {
      DPRINTF (("fpgm: Buggy block coversion from the driver\n"));
      return JFLASH_FAIL;
    }


    if((end_blk + 1) != mjflash_dev->partition_blk_limit)
    {
      DPRINTF (("fpgm: Buggy block coversion from the driver\n"));
      return JFLASH_FAIL;
    }

    parti_info.end_blk = end_blk;
    parti_info.num_blks = end_blk - start_blk + 1;

  }

  TPRINTF(3,("Found partition %s\n", pname));

  TPRINTF(3,("\t%s:: blk_start 0x%x\n", pname, parti_info.start_blk));
  TPRINTF(3,("\t%s:: blk_limit 0x%x\n", pname, (parti_info.start_blk +
             parti_info.num_blks)));

  
  return JFLASH_SUCCESS;
}




/*===========================================================================

FUNCTION flash_finalize

DESCRIPTION
  This function will write any remaining data that is still buffered.
  It will then do any remaining tasks based on programming mode currently
  in force.
  Then just before exit, it will reset a bunch of variables.


DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_finalize()
{
  byte *data = (byte *) parti_buf;
  dword write_addr = flash_block[MI_BLOCK_PARTI_TBL].start;
  flash_ptr_type baseaddr = (flash_ptr_type) nor_device->base_addr;
  word  size = sizeof(parti_buf);
  flash_status status = FLASH_SUCCESS;
  uint32 ret_status = JFLASH_SUCCESS;

  KICK_WATCHDOG();
  
  DPRINTF (("\n----- FINALIZE ---------\n"));

  /* Commit the RAM copy of partition table in RAM */

  if (partition_table_committed == FALSE)
  {
    DPRINTF (("\nffin: Committing partition table to the flash\n"));

    TPRINTF (3, ("ffin: Writing: 0x%x base: 0x%x p_addr: 0x%x size: 0x%x\n",
                  data, baseaddr, write_addr, size));
    status = nor_device->ops->write (data, baseaddr, write_addr, size);

    if (status != FLASH_SUCCESS)
    {
      DPRINTF (("ffin: write failed\n"));
      
      /* Failure code expected by TRACE32 is 0x100 */
      ret_status = JFLASH_FAIL;
    }
    else
    {
      TPRINTF(3, ("ffin: complete\n"));
    }

    partition_table_committed = TRUE;
  }
  else
  {
    TPRINTF (3, ("\nffin: No need update partition table in the flash\n"));
  }
  
  DPRINTF (("===========  Done ===========\n\n"));

  return ret_status;

} // flash_finalize
 
  

/*===========================================================================

FUNCTION flash_init_cfg_data_qcsbl

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for AMSS Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.
  Partition table must have been received

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_init_cfg_data_qcsbl(void)
{
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for CFG DATA QCSBL Image ------\n"));

  if (!partition_rcvd)
  {
    DPRINTF (("Error: no partition table sent yet\n"));
    SET_ERR (ERR_PARTI_NOT_RCVD);
    return JFLASH_FAIL;
  }
  
  /* Open the partition to use for this image */
  if ((init_for_partition_image (MIBIB_QCSBL_PARTI_NAME)) != JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_NO_QCSBL);
    return JFLASH_FAIL;
  }
  KICK_WATCHDOG();

  DPRINTF (("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;

} // flash_init_cfg_data_qcsbl


   


/*===========================================================================

FUNCTION flash_init_oemsbl

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for AMSS Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.
  Partition table must have been received

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_init_oemsbl(void)
{
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for OEMSBL Image ------\n"));

  if (!partition_rcvd)
  {
    DPRINTF (("Error: no partition table sent yet\n"));
    SET_ERR (ERR_PARTI_NOT_RCVD);
    return JFLASH_FAIL;
  }
  
  if ((init_for_partition_image (MIBIB_OEMSBL_PARTI_NAME)) != JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_NO_QCSBL);
    return JFLASH_FAIL;
  }
  KICK_WATCHDOG();

  DPRINTF (("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;

} // flash_init_oemsbl



/*==========================================================================

FUNCTION flash_init_amss

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for AMSS Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.
  Partition table must have been received

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_init_amss(void)
{
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for AMSS Image ------\n"));

  if (!partition_rcvd)
  {
    DPRINTF (("Error: no partition table sent yet\n"));
    SET_ERR (ERR_PARTI_NOT_RCVD);
    return JFLASH_FAIL;
  }
  
  /* Open the partition to use for this image */
  if ((init_for_partition_image (MIBIB_AMSS_PARTI_NAME)) != JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_NO_AMSS);
    return JFLASH_FAIL;
  }
  KICK_WATCHDOG();

  DPRINTF (("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;

} // flash_init_amss


/*==========================================================================

FUNCTION flash_init_all

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for composite Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
flash_init_all(void)
{
  uint32 ret_status = JFLASH_SUCCESS;
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for ALL Image ------\n"));

  hw_present = TRUE;

  /* Open logical device to program provided image */
  if ((init_for_partition_image (MIBIB_DEVICE_ALL_PARTI_NAME)) != 
      JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_INVALID); 
    ret_status = JFLASH_FAIL;
  }
  KICK_WATCHDOG();

  DPRINTF (("===========  Done ===========\n\n"));

  return ret_status;

} // flash_init_all


/*==========================================================================

FUNCTION flash_init_efs

DESCRIPTION
  This function calls the flash probe function and sets up everything to be
  ready to erase and program the device for EFS Image 

DEPENDENCIES
  Assumes jtag_flash_param (global variable) has valid parameters.
  Partition table must have been received

RETURN VALUE
  If success, return JFLASH_SUCCESS else JFLASH_FAIL

SIDE EFFECTS

===========================================================================*/

static uint32 
 flash_init_efs(void)
{
  KICK_WATCHDOG();
  
  DPRINTF (("\n\n------ Initialize for EFS Image ------\n"));

  if (!partition_rcvd)
  {
    DPRINTF (("Error: no partition table sent yet\n"));
    SET_ERR (ERR_PARTI_NOT_RCVD);
    return JFLASH_FAIL;
  }


  /* If a good build was not alreay present on the flash, EFS image cannot be
   * programmed.
   */
  if (flash_has_parti_tbl == FALSE)
  {
    DPRINTF (("Error: No partition table found on flash\n"));
    SET_ERR (ERR_NO_FLASH_PARTI);
    return JFLASH_FAIL;
  }

  /* Open the partition to use for this image */
  if ((init_for_partition_image (MIBIB_EFS2_MODEM_PARTI_NAME)) != 
      JFLASH_SUCCESS)
  {
    SET_ERR (ERR_PARTI_NO_EFS2);
    return JFLASH_FAIL;
  }
  KICK_WATCHDOG();

  DPRINTF (("===========  Done ===========\n\n"));

  return JFLASH_SUCCESS;

} //  flash_init_efs

/*==========================================================================

FUNCTION save_idtable

DESCRIPTION
  This function makes use of the id_tbl data variable so that space will be 
  allocated for id_tbl and won't get removed during optimization by the 
  compiler

DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void
save_idtable(void)
{
  int i;
  extern const char *id_tbl[];
  volatile char dummy_id_blk[] = "DUMMY";
  /* Copy bytes so the we do not lose ID block. */
  for (i = 0; i < sizeof(dummy_id_blk); i++)
  {
    dummy_id_blk[i] = id_tbl[0][i];
  }
  
  /* Dummy operation to avoid compiler warning */ 
  (void)dummy_id_blk;

  return;
}

#ifdef BUILD_FOR_X86

/*===========================================================================

FUNCTION main

DESCRIPTION
  This function is a wrapper for calling main_c from a console application to
  generate system parition table for a known part.

DEPENDENCIES
  Takes as input the user partition table binary and device IDs.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

int main(int argc, char *argv[])
{
  FILE *usr_parti_file, *sys_parti_file;
  FILE *log_file, *cfg_file;
  uint32 output_log = FALSE;
  uint32 status;
  int count = 0;
  char ch, log_flag[JFLASH_MAX_FNAME_BSIZE];
  char fname1[JFLASH_MAX_PATH_BSIZE], fname2[JFLASH_MAX_PATH_BSIZE];
  char fname3[JFLASH_MAX_PATH_BSIZE], bin_path[JFLASH_MAX_PATH_BSIZE];
  char usr_bin_fname[JFLASH_MAX_FNAME_BSIZE];
  char sys_bin_fname[JFLASH_MAX_FNAME_BSIZE]; 
  char usr_bin_path[JFLASH_MAX_PATH_BSIZE];
  char sys_bin_path[JFLASH_MAX_PATH_BSIZE];
  char log_txt_path[JFLASH_MAX_PATH_BSIZE];
  char format[]="ID0=0x%x\n", flash_name[JFLASH_MAX_FNAME_BSIZE];
  char * buf_ptr = (char *) jtag_flash_param.data;
  word  codes[4];
  int ids, ids_found;
  flashi_nor_device const **dev;
  flashi_nor_device const ***partlist;
  int i;
  flash_partable_t sys_parti_ptr;
  int bin_path_known = FALSE;
  int use_device_name = FALSE;

  if(argc < 2)
  {
    printf("\n\nConfiguration file not provided\n");
    printf("Usage: %s flash.cfg\n\n", argv[0]);
    printf ("Or using command line\n\n");
    printf("Usage:\n");
    printf("-f <\"Flash device  name\">            Flash Device name\n");
    printf("-u <usr partition filename>          Usr partition table filename\n");
    printf("-s <sys partition filename>          System partition table filename\n");
    printf("-p <directory path>                  Path to store the partition table files\n\n");
    printf("Example:mjflash -f \"INTEL 1024 M18 ADMux\" -u usr_partition.mbn");
    printf(" -s sys_partition.mbn -p .\\..\\..\\build\\ms\\bin\\SBAXL\\ \n");

    exit(EXIT_FAILURE);
  }
  else if (argc == 2)
  {
    if ((cfg_file = fopen(argv[1], "r")) == NULL)
    {
      printf("\nCould not open config file: %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }

    for (i = 0; i < 4; i++)
    {
      fscanf(cfg_file, format, &codes[i]);
      printf("CODE[%d]=0x%x\n", i, codes[i]);
      format[2]++;
    }

    printf("\n");

    fscanf(cfg_file, "USR_PARTITION_BINARY=%s\n", fname1);
    fscanf(cfg_file, "SYS_PARTITION_BINARY=%s\n", fname2);

    printf("\nUsing user partition table in file: %s\n", fname1);
    printf("\nCreating system partition table in file: %s\n", fname2);

    fscanf(cfg_file, "CREATE_LOG=%s\n", log_flag);
 
    if( (log_flag[0] == 'Y') &&
        (log_flag[1] == 'E') &&
        (log_flag[2] == 'S') )
    {
      fscanf(cfg_file, "LOG_FILE=%s\n", fname3);
      printf("\nLogs will be put in file: %s\n", fname3);

      if ((log_file = fopen(fname3, "w")) == NULL)
      {
        printf("\nError opening log file: %s\n", fname3);
        exit(EXIT_FAILURE);
      }      
      output_log = TRUE;
    }
    else
    {
      printf("\nNo Logs will be created\n");
    }

    if ((usr_parti_file = fopen(fname1, "r")) == NULL)
    {
      printf("\nError opening user partition table file: %s\n", fname1);
      exit(EXIT_FAILURE);  
    }

    if ((sys_parti_file = fopen(fname2, "w")) == NULL)
    {
      printf("\nError opening system partition table file: %s\n", fname1);
      exit(EXIT_FAILURE);
    }

  }
  else
  {
    /* Process the given parameters. We will need to have the flash device
     * name string, path to the bin directory and the file names for user and
     * system partition tables.
     */      
    if (argc != 9)
    {
      printf("\n\nRequired info not provided\n");
      printf("argc=%d\n", argc);
      printf("Usage: %s flash.cfg\n", argv[0]);
      exit(EXIT_FAILURE);
    }

    bin_path_known = FALSE;

    for (i = 1; i < 9; i = i + 2)
    {

      if (argv[i][0] != '-')
      {
        printf("\n\nOptions not specified properly\n");
        exit(EXIT_FAILURE);
      }

      switch (argv[i][1])
      {
        case 'p':
          printf("Using binaries in folder: %s\n", argv[i+1]);
          status = jflash_strcpy(bin_path, argv[i+1], JFLASH_MAX_PATH_BSIZE);
          bin_path_known = TRUE;
          break;
        case 'u':
          printf("Using usr parti binary: %s\n", argv[i+1]);
          status = jflash_strcpy(usr_bin_fname, argv[i+1], 
                                 JFLASH_MAX_FNAME_BSIZE);
          break;
        case 's':
          printf("Using sys parti binary: %s\n", argv[i+1]);
          status = jflash_strcpy(sys_bin_fname, argv[i+1], 
                                 JFLASH_MAX_FNAME_BSIZE);
          break;
        case 'f':
          printf("Using Flash part: %s\n", argv[i+1]);
          status = jflash_strcpy(flash_name, argv[i+1], JFLASH_MAX_FNAME_BSIZE);
          use_device_name = TRUE;
          break;
        default:
          printf("Unknown switch -%c specified\n", argv[i][1]);
          break;
      }
      if (status == JFLASH_FAIL)
      {
        exit(EXIT_FAILURE);
      }
    }

    /* Construct the paths if all needed info is available */
    if (bin_path_known == TRUE)
    {
      status = jflash_makepath(usr_bin_path, bin_path, usr_bin_fname, 
                               JFLASH_MAX_PATH_BSIZE);
      
      if (status == JFLASH_FAIL)
      {
        printf("\nmake path failed for usr parti binary\n");
        exit(EXIT_FAILURE);
      }

      printf("Using usr parti binary: %s\n", usr_bin_path);

      if ((usr_parti_file = fopen(usr_bin_path, "r")) == NULL)
      {
        printf("\nCould not open User parti binary: %s\n", usr_bin_path);
        exit(EXIT_FAILURE);
      }

      status = jflash_makepath(sys_bin_path, bin_path, sys_bin_fname, 
                               JFLASH_MAX_PATH_BSIZE);
      if (status == JFLASH_FAIL)
      {
        printf("\nmake path failed for sys parti binary\n");
        exit(EXIT_FAILURE);
      }

      printf("Using sys parti binary: %s\n", sys_bin_path);

      if ((sys_parti_file = fopen(sys_bin_path, "wb")) == NULL)
      {
        printf("\nCould not open Sys parti binary: %s\n", sys_bin_path);
        exit(EXIT_FAILURE);
      }

      status = jflash_makepath(log_txt_path, bin_path, flash_name, 
                               JFLASH_MAX_PATH_BSIZE);
      status |= jflash_strcat(log_txt_path, ".txt", JFLASH_MAX_PATH_BSIZE);
    
      if (status != JFLASH_SUCCESS)
      {
        printf("\ncreate path failed for log txt\n");
        exit(EXIT_FAILURE);
      }

      printf("Generating logs in: %s\n", log_txt_path);

      if ((log_file = fopen(log_txt_path, "w")) == NULL)
      {
        printf("\nCould not open log file: %s\n", log_txt_path);
        exit(EXIT_FAILURE);
      }

    }

  }

  /* Read partition table data from the file in to a buffer where the
    * conversion routine can manipulate it.
    */
  while (!feof(usr_parti_file))
  {
    ch = getc(usr_parti_file);
    if (ferror(usr_parti_file))
    {
       printf("\nFailure reading User Partition Table\n");
    }
    if(!feof(usr_parti_file))
    {
       *buf_ptr = ch;
       ++buf_ptr;
       count++;
       if (count >= JFLASH_JTAG_COMM_BUFFER)
       {
         printf("\nBuffer overflow\n");
       }
    }
    else
    {
       break;
    }
  }
 
  /* Since do not have hardware, we will need to populate the device structure
   * based on the IDs passed into us. The pedefined device geomery information
   * will be used.
   */
  
  /* Scan for the codes.  Check indicated number of ids for
   * each component in the list.  Stop when we find a match.
   * our indicator of no match.
   */
    
  for (partlist = global_parts; *partlist != NULL; partlist++)
  {
    for (dev = *partlist; *dev != NULL; dev++)
    {

      if (use_device_name == FALSE)
      {
        ids_found = 0;
        for (ids = 0; ids < (*dev)->num_ids; ids++)
        {
          if (codes[ids] == (*dev)->codes[ids])
          {
            /* Every time we find a match, increment ids_found.
             * When we exit the loop, if ids_found equals the number
             * of IDs we had to match we found the part
             */
            ids_found++;
          }
        }
        if (ids_found == (*dev)->num_ids)
        {
          break;
        }
      }
      else
      {
        /* Search by device name */
        if (!strncmp((*dev)->name, flash_name, sizeof(flash_name)))
        {
          break;
        }

      }
    }
    if (*dev != NULL)
    {
         break;
    }
  }
  
  if (*dev != NULL)
  {
    nor_device = (flashi_nor_device *) *dev;
    printf("Found: %s\n", nor_device->name);
  }
  else
  {
    printf("Device not found\n");
    exit(EXIT_FAILURE);
  }

  if (nor_device->handcoded_geometry != NULL)
  {
    nor_device->geometry = nor_device->handcoded_geometry;
  }

  mjflash_dev = &flash_dev;
  mjflash_dev->nor_dev_info = *nor_device;

  jtag_flash_param.status =  FLASH_USR_PARTI_TBL;
  jtag_flash_param.size =  count;
  jtag_flash_param.width = FALSE;
  jtag_flash_param.reserved = FALSE;  /* No override */

  status = main_c();
 
   /* return error code back to TRACE32, 0 = success, 0x100 = fail */
   if (jtag_flash_param.status == JFLASH_FAIL)
   {
     exit(EXIT_FAILURE);
     /* Close all the open files here */
   }


   /* Read partition table data from the file in to a buffer where the
    * conversion routine can manipulate it.
    */

  count = 0;
  buf_ptr = (char *) sys_parti_buffer;
  sys_parti_ptr = (flash_partable_t) buf_ptr;

  printf("\n\n");
  printf("Magic1=0x%X\n", (int) sys_parti_ptr->magic1);
  printf("Magic2=0x%X\n", (int) sys_parti_ptr->magic2);
  printf("Version=0x%X\n", (int) sys_parti_ptr->version);
  printf("Number of partitions=0x%X\n", (int) sys_parti_ptr->numparts);

  printf("\n\nPartition Name\t Offset\t\tLength\t\tWhich Flash\n");
  printf("---------------\t-------\t\t--------\t------------\n\n");

  for (i=0; i < sys_parti_ptr->numparts; i++)
  {
    printf("%-16s", sys_parti_ptr->part_entry[i].name);
    printf("0x%08X\t", (int) sys_parti_ptr->part_entry[i].offset);
    printf("0x%08X\t", (int) sys_parti_ptr->part_entry[i].length);
    printf("0x%-3X\n", sys_parti_ptr->part_entry[i].which_flash);
  }

  printf("\n\n");

  fprintf(log_file, "\n\n");
  fprintf(log_file, "Magic1=0x%X\n", (int) sys_parti_ptr->magic1);
  fprintf(log_file, "Magic2=0x%X\n", (int) sys_parti_ptr->magic2);
  fprintf(log_file, "Version=0x%X\n", (int) sys_parti_ptr->version);
  fprintf(log_file, "Number of partitions=0x%X\n", 
           (int) sys_parti_ptr->numparts);

  fprintf(log_file, "\n\nPartition Name\t Offset\t\tLength\t\tWhich Flash\n");
  fprintf(log_file, "---------------\t-------\t\t--------\t------------\n\n");

  for (i=0; i < sys_parti_ptr->numparts; i++)
  {
    fprintf(log_file, "%-16s", sys_parti_ptr->part_entry[i].name);
    fprintf(log_file, "0x%08X\t", (int) sys_parti_ptr->part_entry[i].offset);
    fprintf(log_file, "0x%08X\t", (int) sys_parti_ptr->part_entry[i].length);
    fprintf(log_file, "0x%-3X\n", sys_parti_ptr->part_entry[i].which_flash);
  }

  fprintf(log_file, "\n\n");


  printf("Generating System partition file: \n");

  count = sizeof( struct flash_partition_table);

  if(fwrite(buf_ptr, sizeof(char), count, sys_parti_file) != count)
  {
    printf("\nFailure writing system Partition Table\n");
  }

  printf("Total Bytes written: %d\n", count);

  /* Call exit() function to flush all open streams with unwritten buffered
   * data, close all open streams and exit.
   */

  exit(EXIT_SUCCESS);

} /* main_c */


/*===========================================================================

FUNCTION jflash_strcpy

DESCRIPTION
  This function copy the source char string to destination string using memcpy 
  
RETURN VALUE
  JFLASH_SUCCESS - Success
  JFLASH_FAIL    - Buffer overflow 

SIDE EFFECTS
   None
===========================================================================*/
static uint32 
jflash_strcpy(char *dir_name, const char *filepath, uint32 dir_name_buf_sz)
{
  uint32 len = strlen(filepath);
  if ((len + 1)  > dir_name_buf_sz )
  {
    return JFLASH_FAIL;
  }  
  memcpy(dir_name, filepath, len);
  dir_name[len] ='\0';
  return JFLASH_SUCCESS;
} 


/*===========================================================================

FUNCTION jflash_makepath

DESCRIPTION
  This function make a full file path name based on file path and file name 
  
RETURN VALUE
  JFLASH_SUCCESS - Success
  JFLASH_FAIL    - Buffer overflow 

SIDE EFFECTS
  None

===========================================================================*/
static uint32 
jflash_makepath(char *dir_name, const char *filepath, const char *filename, 
                uint32 dir_name_buf_sz)
{
  uint32 file_path_len;
  uint32 file_name_len;

  file_path_len = strlen(filepath);
  file_name_len = strlen(filename);
  
  if ((file_path_len + file_name_len + 2) > dir_name_buf_sz)
  {
    return JFLASH_FAIL;
  }
  
  memcpy(dir_name, filepath, file_path_len);
  dir_name[file_path_len] = '/';
  memcpy(&dir_name[file_path_len + 1], filename, file_name_len);
  dir_name[file_path_len + file_name_len + 1] = '\0';
  return JFLASH_SUCCESS;
} 

/*===========================================================================

FUNCTION jflash_strcat

DESCRIPTION
  This function concatenates the source string to the destination string
  
RETURN VALUE
  JFLASH_SUCCESS - Success
  JFLASH_FAIL    - Buffer overflow 


SIDE EFFECTS
   None
===========================================================================*/
static uint32 
jflash_strcat(char *dest_str, const char *src_str, uint32 dest_buf_size)
{
  uint32 sstr_len;
  uint32 dstr_len;

  dstr_len = strlen(dest_str);
  sstr_len = strlen(src_str);
  
  if ((dstr_len + sstr_len + 1) > dest_buf_size)
  {
    return JFLASH_FAIL;
  }
  memcpy(&dest_str[dstr_len], src_str, sstr_len);
  dest_str[sstr_len + dstr_len] = '\0';
  return JFLASH_SUCCESS;
} 

#endif
