/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           G E N E R I C   F L A S H   E P R O M   S U P P O R T

GENERAL DESCRIPTION
  This module implements the programming interface for flash memory
  devices.
  
  Everything in this file is MSM independent.
  

EXTERNALIZED FUNCTIONS
  flash_device_init_data
    Does any necessary data initialization
  
  flash_device_probe
    Probes to identify a flash
  
  flash_device_type
    Returns a code indicating which exact Flash device was detected.
    
  flash_device_name
    Returns a pointer to a string containing the flash name
    
  flash_device_sectors
    Stuffs the buffer with sector information and returns the number
    of bytes stuffed in the buffer
    
  do_open
    Stub for function only used in next version of the protocol

  do_close
    Stub for function only used in next version of the protocol

  do_simple_write
    Writes a packet into a block of memory.
  
  do_stream_write
    Writes a packet into a block of memory and handles any necessary
    erases before writing.

  do_read
    Reads a block of data to the given buffer.  In the case of NOR flash
    this is not strictly necessary, since NOR flash is directly readable
    as memory, but this needs to be dispatched for flash types (NAND)
    which cannot be read directly as memory.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call flash_device_init_data before any others.
  Must call flash_device_probe before any others except init.
  
  Only one of these routines may be executing at once, and they must be
  executed out of some other memory device than the one being manipulated.

Copyright (c) 2003, 2005, 2007-2008 Qualcomm Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/NORflash/ap_flash.c#4 $ $DateTime: 2008/04/11 20:13:00 $ $Author: c_surven $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/NORflash/ap_flash.c#1 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 03/17/08   sv      Modified to use offset based address
 03/12/08   sv      Use flash_open instead of directly calling driver routines 
 03/09/08   sv      Adapt to change in internal flash driver API
 03/01/07   mm      Fixed buffer Overrun issue for NOR Flashes with higher 
                    number of sectors. The sectors are limited to 
                    MAX_BLOCKS_IN_REPLY.
 12/05/05   dp      NOR flash driver unification 
 09/23/03   drh     Extensive rewrite to make drivers usable in a variety
                    of flash tools.
                    Previous history removed because it no longer applies.

============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include "comdef.h"
#include "ap_armprg.h"
#include "ap_flash.h"
#include "ap_msm.h"
#include "flash_nor_device.h"
#include "flash_nor_msm.h"
#include "mibib.h"
#ifdef CUST_EDITION
#include "miheader.h"
#endif

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------
/* This is the number of sectors which will fit in the reply buffer.
 * This really should be done by calculations, but due to the nature of
 * the parameter request reply, it must be done by estimation.  This is
 * because one part of the reply is variable length, and that is the flash
 * name.  So, take all the fixed length fields and add up their length, then
 * take a very long length for the flash name and add that to it.  Subtract
 * this value from the reply length buffer size.  Now you have the number of 
 * bytes that are available for storing sector sizes.  Divide this by 4
 * to get the number of sectors we can put in, and then subtract some to be
 * absolutely sure we stay below the limit.
 */
 
#define MAX_BLOCKS_IN_REPLY 300


//--------------------------------------------------------------------------
// Global Data - Generic to every flash device layer
//       Also used by packet layer
//--------------------------------------------------------------------------

/* This needs to be instantiated in every flash device layer that we
 * create.  It is generic to the flash layer */

/* enum to indicate status of probe */
probe_code_type flash_dev_type = FLASH_DEVICE_NOT_PROBED;


//--------------------------------------------------------------------------
// Local Data - Specific to NOR flash device layer, do not replicate
//               to other drivers.
//--------------------------------------------------------------------------

flashi_nor_device *nor_device = NULL;
struct fsi_device_data *hostdl_dev = NULL;

/* Caches the current flash sector size bytes of the sector being erased  */
/* If FLASH_CHECK defined in flash driver, this variable gets used
   to verify the sector has been erased */
dword curr_erase_sector_bsize;    

static block_descriptor_type flash_block[FSD_MAX_SECTORS];
static uint32 total_sectors=0;
static dword flash_base_addr = 0;

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

/*===========================================================================

FUNCTION flash_device_init_data

DESCRIPTION
  This function does one time initialization of data structures used in
  flash device access

DEPENDENCIES
  Must be called from main before any other flash access functions.

RETURN VALUE
  none
  
SIDE EFFECTS
  none

===========================================================================*/

void flash_device_init_data (void)
{
  /* Since device is not probed yet set appropriate variables to force
   * device probing and initialization of required structures
  */
  flash_dev_type = FLASH_DEVICE_NOT_PROBED;
  
  hostdl_dev = NULL;
  nor_device  = NULL;
}



/*===========================================================================

FUNCTION flash_device_probe

DESCRIPTION
  This function calls the flash device specific function to probe the
  flash

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void 
flash_device_probe(void)
{
  uint32 status = TRUE;
  word count;
  dword xcount, curraddr;
  int32 sector_indx = 0;
  int32 bank_end = -1, total_banks, bank_indx = -1;
  uint32 start_addr, limit_addr;

  /* Only probe device if it has not yet been probed. */
  if (hostdl_dev == NULL)
  {
    hostdl_dev =(fsi_device_t) flash_open(MIBIB_DEVICE_ALL_PARTI_NAME, 0);
    if (hostdl_dev != NULL)
    {
      nor_device = &(hostdl_dev->nor_dev_info);     
      if (nor_device == NULL)
      {
        DPRINTF (("Error: Bad device descriptor found \n"));
        flash_close((fs_device_t)hostdl_dev);
        hostdl_dev = NULL;
        flash_dev_type = FLASH_DEVICE_UNKNOWN;
      }
      else /* if (nor_device != NULL) */
      {
        fs_device_t self = (fs_device_t)hostdl_dev;
        flash_dev_type = FLASH_DEVICE_FOUND;
        self->get_partition_phy_limits((fs_device_t)hostdl_dev, &start_addr,
                                        &limit_addr, NULL);
        flash_base_addr = start_addr;
        curraddr = 0;
        total_banks = nor_device->geometry->num_banks;

        /* Iterate through the geometry structure to fill the block info
         * structure.  Outer loop index keeps track of erase block regions 
         * and the inner loop index keeps track of the number of blocks in 
         * each region.
         */ 
        for (count = 0; (count < nor_device->geometry->num_erase_blk_region) && 
                        (TRUE == status); count++)
        {
          for (xcount = 0; xcount < 
               nor_device->geometry->blk_regions[count].numblocks; xcount++)
          {
            if (sector_indx > bank_end)
            {
              if (++bank_indx >= total_banks)
              {
                DPRINTF(("fudi: Bank index 0x%x exceeded bank count 0x%x",
                         bank_indx, total_banks));
                status = FALSE;
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
  
        if(status != TRUE)
        {   
          flash_close((fs_device_t)hostdl_dev);
          hostdl_dev = NULL;
          nor_device = NULL;
          flash_dev_type = FLASH_DEVICE_UNKNOWN;
        } 
      } /* if (nor_device != NULL) */
    }
    else /* if (hostdl_dev == NULL) */
    {
      DPRINTF (("Error: Bad device descriptor found \n"));
      flash_dev_type = FLASH_DEVICE_UNKNOWN;
    } /* if (hostdl_dev == NULL) */
  }
}  // flash_device_probe



/*===========================================================================

FUNCTION do_open

DESCRIPTION
  This function is a stub for a function needed in the next revision of
  the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/

response_code_type
do_open (byte mode)
{
  return ACK;
}                               /* do_open() */

/*===========================================================================

FUNCTION do_close

DESCRIPTION
  This function is a stub for a function needed in the next revision of
  the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/

response_code_type
do_close ()
{
  return ACK;
}                               /* do_close() */





/*===========================================================================

FUNCTION do_simple_write

DESCRIPTION
  This function writes a provided string of bytes into a specified
  block of memory, by calling the appropriate device-specific function.
  For NOR flash, this is not really necessary, however, the interface is here
  to support other flash types that need the differentation between write,
  which requires a prior erase and simple_write which does not.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  See device-specific functions for details..

===========================================================================*/

response_code_type
do_simple_write (byte * buf,
/* Pointer to the string of bytes to write */
          dword addr,
/* 20-bit linear physical address of the first byte of memory to write */
          word len
/* Count of bytes to write */
  )
{
  response_code_type ret_code = ACK;
  flash_ptr_type baseaddr = (flash_ptr_type)flash_base_addr;
  
  /* If we have not probed flash, do so now */
  if (flash_dev_type == FLASH_DEVICE_NOT_PROBED)
  {
    flash_device_probe();
  }
  
#ifdef CUST_EDITION
  // 软件工具下载的时候只允许更新AMSS部分的代码，以保证BOOT正常
  if(addr+len < MI_NOR_AMSS_HASH_START_ADDR)
  {
     return ACK;
  }
  else
  {
     if(addr < MI_NOR_AMSS_HASH_START_ADDR)
     {
        dword ilen = MI_NOR_AMSS_HASH_START_ADDR - addr;
        len -= ilen;
        buf += ilen;
        addr = MI_NOR_AMSS_HASH_START_ADDR;
     }
  }
#endif

  if (flash_dev_type != FLASH_DEVICE_FOUND)
  {
    return NAK_WRONG_IID;
  }
  
  /* do the simple write */
  if (nor_device->ops->write (buf, baseaddr, addr, len) != FLASH_SUCCESS)
    ret_code = NAK_FAILED;
  
  if (ret_code != ACK)
  {
    DPRINTF (("dsiw: write failed\n"));
  }
  else
  {
    TPRINTF(3, ("dsiw: complete\n"));
  }

  return ret_code;

}                               /* do_simple_write() */


/*===========================================================================

FUNCTION do_stream_write

DESCRIPTION
  This function writes a provided string of bytes into a specified
  block of memory, by calling the appropriate device-specific function.
  Erase blocks as necessary before writing.
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  See device-specific functions for details..

===========================================================================*/

response_code_type
do_stream_write (byte * buf,
/* Pointer to the string of bytes to write */
          dword addr,
/* 20-bit linear physical address of the first byte of memory to write */
          word len
/* Count of bytes to write */
  )
{
  response_code_type retVal=ACK;
  /* For now this function only supports NOR devices at base addresses 0x0. */
  flash_ptr_type baseaddr = (flash_ptr_type)flash_base_addr;
  word block_start = 0, block_end = 0;
  dword tmp;
  int i = 0;

  
  /* If we have not probed flash, do so now. */
  if (flash_dev_type == FLASH_DEVICE_NOT_PROBED)
  {
    flash_device_probe();
  }

  if (flash_dev_type != FLASH_DEVICE_FOUND)
  {
    return NAK_WRONG_IID;
  }
  
  TPRINTF(4,("dstw: addr 0x%x  len 0x%x\n", addr, len));

  /* bail out if zero length write */
  if (len == 0)
  {
    DPRINTF (("dstw: INVALID LEN \n"));
    return NAK_INVALID_LEN;
  }
 
  /* Determine which sector this write is supposed to be in.
   * It may begin in a sector which is already erased, in which
   * case, we want to bump to the next block.
   */
  tmp = 0;
  while (block_start < total_sectors)
  {
    tmp += flash_block[block_start].len;
    if (tmp > addr)
      break;
    block_start++;
  }
 
  if (block_start == total_sectors)
  {
    DPRINTF (("dstw: INVALID LEN 2\n"));
    return NAK_INVALID_LEN;
  }
 
  TPRINTF (3, ("dstw: block st 0x%x\n", block_start));
 
  block_end = block_start;
  while (block_end < total_sectors)
  {
    if ((addr + len - 1) < tmp)
      break;
    tmp += flash_block[block_end].len;
    block_end++;
  }
 
  KICK_WATCHDOG();
  CHECK_FOR_DATA ();

  TPRINTF (3, ("dstw: block end 0x%x\n", block_end));
 
  if (block_end == total_sectors)
  {
    DPRINTF (("dstw: INVALID LEN 3\n"));
    return NAK_INVALID_LEN;
  }
 
  /* Call erase function on blocks to erase */
  for (i = block_start; i <= block_end; i++)
  {
    if (flash_block[i].is_erased == FALSE)
    {

      curr_erase_sector_bsize = flash_block[i].len;

      if (nor_device->ops->erase_start(baseaddr, flash_block[i].start) != 
          FLASH_SUCCESS)
      {
        retVal = NAK_FAILED;
      }

      if (retVal == ACK)
      {
        flash_status status;

        do
        { 
          status = nor_device->ops->erase_status(nor_device, baseaddr + 
          BYTE_TO_WORD_OFFSET(flash_block[i].start));
          KICK_DOG_AND_CHECK_DATA();
        } while (status == FLASH_OP_NOT_COMPLETE);

        if (status != FLASH_SUCCESS) 
        {
          retVal = NAK_FAILED;
        }  
        else
        {  
          retVal = ACK;
        }  
      }
  
      if (retVal != ACK)
      {
        return retVal;
      }
      else
      {
        flash_block[i].is_erased = TRUE;
      }
    }
  }

  /* Block is ready to write, do the write */
  if (nor_device->ops->write (buf, baseaddr, addr, len) != FLASH_SUCCESS)
    retVal = NAK_FAILED;

  if (retVal != ACK)
  {
    DPRINTF (("dstw: write failed\n"));
  }
  else
  {
    TPRINTF(3, ("dstw: complete\n"));
  }

  return retVal;
}                               /* do_stream_write() */

/*===========================================================================

FUNCTION do_read

DESCRIPTION
  This function reads string of bytes into a specified
  block of memory.

DEPENDENCIES

RETURN VALUE
  Returns a response_code_type enum indicating what happened.  This
  code can be used directly to select a response packet to send back.

SIDE EFFECTS
  None

===========================================================================*/
response_code_type 
do_read (byte * buf, dword addr, word length)
{
  int i, j;

  for (i=0, j=0; j < length; j++)
  {
    buf[i++] = ((byte *) addr)[j];
  }
  return ACK;
}



/*===========================================================================

FUNCTION flash_device_name

DESCRIPTION
  This function returns a pointer to a string containing the flash
  device name

DEPENDENCIES
  Uses the dev_type regional variable.

RETURN VALUE
  string pointer

SIDE EFFECTS
  none
===========================================================================*/

char *
flash_device_name (void)
{
  /* If we have not probed flash, do so now. */
  if (flash_dev_type == FLASH_DEVICE_NOT_PROBED)
  {
    flash_device_probe();
  }

  if (flash_dev_type != FLASH_DEVICE_FOUND)
  {
    return 0;
  }
  else
  {
    return (char *)nor_device->name;
  }
}                               /* flash_device_name() */

/*===========================================================================

FUNCTION flash_device_sectors

DESCRIPTION
  This function fills the indicated handle_hello response buffer with the 
  number of sectors, and a length for each sector.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the number of bytes added to the indicated buffer

SIDE EFFECTS
  None

===========================================================================*/

int
flash_device_sectors (uint8 *reply_buffer)
{
  uint32 nbytes;
  uint32 i, p;
  int nblocks;
  int num_of_shifts=0;

  p = 0;

  /* If we have not probed flash, do so now. */
  if (flash_dev_type == FLASH_DEVICE_NOT_PROBED)
  {
    flash_device_probe();
  }

  if (flash_dev_type != FLASH_DEVICE_FOUND)
  {
    return NAK_WRONG_IID;
  }
  
  nblocks = total_sectors;

  /*  Adjust blocks and pages so we don't overflow reply buffer.
   *  The number of blocks and the size of the blocks in reality
   *  only need to add up to the actual space available.  The host tool
   *  only uses these in a progress message, and never uses this
   *  information in reality to manipulate anything else.  So, we keep 
   *  shifting down the number blocks while shifting up the size of
   *  the blocks until the number of blocks will fit in the reply buffer.
   */
  while (nblocks > MAX_BLOCKS_IN_REPLY)
  {
    nblocks = nblocks >> 1;
    num_of_shifts++;
  }

  /* Block.  Number of sectors. */
  reply_buffer[p++] = nblocks & 0xFF;
  reply_buffer[p++] = (nblocks >> 8) & 0xFF;     /*lint !e702 */
 
  for (i = 0; i < nblocks; i++)
  {
    nbytes = (flash_block[i].len << num_of_shifts);
   
    TPRINTF (3, ("fds:  block 0x%x size 0x%x \n", i, nbytes));
    reply_buffer[p++] = (nbytes) & 0xFF;
    reply_buffer[p++] = (nbytes >> 8) & 0xFF;
    reply_buffer[p++] = (nbytes >> 16) & 0xFF;
    reply_buffer[p++] = (nbytes >> 24) & 0xFF;
  }

  return (int)p;
}


