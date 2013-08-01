/**********************************************************************
 * flash_nor_toshiba.c
 *
 * toshiba specific flash support
 *
 * This file implements Samsung specific functions and data
 *
 * Copyright (c) 2008-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_toshiba.c#6 $ $DateTime: 2009/05/11 12:22:13 $ $Author: c_surven $


when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-01-02   sv      Added 8MB Samsung NOR(K5N6433ABM) support
2009-03-12   sv      Fix bad powerdown issue with K5N5629ABM
2008-08-14   sv      Changed #error to ERR_FATAL
2008-07-28   jz      Changed from FS_DEVICE_WRITES_SIMPLE to
                     FS_DEVICE_WRITES_PAIRED_BITS
2008-07-01   jz      Added Samsung NOR flash support
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "flash_nor_device.h"
#include "flash_nor_msm.h"
#include "clk.h"
#include "flash_msg.h"

/*lint -e613 -e668 upper layers already check for null pointers */
/*lint -e526 Don't warn about undefined extern symbols*/
/*lint -emacro(746, FSI_PEEK)*/
/*lint -emacro(611, FSI_PEEK) function call pointer reference */
/*lint -esym(551, erase_verify_buf) Variable used only in tools */

/* Checks every write and erase operation and make sures the flash contains the 
 * correct data after each operation is finished. Slows down performance of the
 * flash driver but good to check it up especially when testing a new device
 */
#undef FLASH_CHECK   

/* This should always be defined as the AMD part is broken with regards
 * to both fast write and suspend
 */
#define FEATURE_EFS_AMD_SUSPEND_FETCH_BUG

#define TOSHIBA_MAX_TIMEOUT_CNT   100
/* Low level status bits for AMD. */
#define FS_AMD_DQ7      0x80
#define FS_AMD_DQ6      0x40
#define FS_AMD_DQ5      0x20
#define FS_AMD_DQ3      0x08
#define FS_AMD_DQ2      0x04

/* This function is located in flash_ram.c. */
extern void flash_toshiba_get_id_codes(volatile word *baseaddr, word *dest);
extern void flash_peek_twice(volatile word *wptr, word *prior, word *current);

/* Forward function prototypes */
LOCAL flash_status fsi_toshiba_configure (fsi_device_t self,
                                           flash_ptr_type baseaddr);
LOCAL flash_status fsi_toshiba_erase_start (flash_ptr_type baseaddr,
                                             dword offset);
LOCAL flash_status fsi_toshiba_erase_status (flashi_nor_device *nor_device,
                                              flash_ptr_type eraseaddr);
LOCAL flash_status fsi_toshiba_suspend (flash_ptr_type wptr);
LOCAL flash_status fsi_toshiba_resume (flash_ptr_type eraseaddr);

LOCAL flash_status fsi_toshiba_fast_byte_write (byte *buffer,
                                                 flash_ptr_type baseaddr,
                                                 dword offset, dword count);

struct flashi_nor_dev_ops flash_toshiba_op_word_write_functions =
{
  fsi_toshiba_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                    /* Read operation. */
  fsi_toshiba_fast_byte_write,                   /* Write for Samsung. */
  fsi_toshiba_erase_start,                       /* Erase for Samsung. */
  fsi_toshiba_erase_status,                      /* Status for Samsung. */
  fsi_toshiba_suspend,
  fsi_toshiba_resume,
  NULL,
  NULL,
  NULL
};
 /* The fsi_nor_device structure will have minimal configuration information 
  * such as number of manufacture codes, actual manufacture codes, device
  * geometry info, partition info and the necessary function pointers. 
  * The other data such as number of sectors and the sector layout, etc 
  * are computed at run time depending on the flash used. So they have 
  * intialized value set as ZERO
  */  

flash_geometry_info TC58FYM7T8C_TOP_geometry =
{
  FLASH_TOSHIBA_FAMILY,
  FLASH_SIZE_16MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,
  2,
  {
    {127, 131072},
    {8, 16384},
    {0, 0},
  },  /*lint !e785*/
  135,
  8,
  {
    { 0,    16,  15,  0},
    { 16,   16,  31,  0},
    { 32,   16,  47,  0},
    { 48,   16,  63,  0},
    { 64,   16,  79,  0},
    { 80,   16,  95,  0},
    { 96,   16,  111,  0},
    { 112,  23,  134,  0},
  }
};  /*lint !e785*/

flash_geometry_info TC58FYM7T8C_BOT_geometry =
{
  FLASH_TOSHIBA_FAMILY,
  FLASH_SIZE_16MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,  
  2,
  {
    {8, 16384},
    {127, 131072},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  135,
  8,
  {
    { 0,    23,  22,  0},
    { 23,   16,  38,  0},
    { 39,   16,  54,  0},
    { 55,   16,  70,  0},
    { 71,   16,  86,  0},
    { 87,   16,  102, 0},
    { 103,  16,  118, 0},
    { 119,  16,  134, 0},
  }
};  /*lint !e785*/

flashi_nor_device TC58FYM7T8C_TOP =
{
  "TOSHIBA TC58FYM7T8C_TOP",
  2,                            /* # of codes to match */
  {0x0098, 0x009E, 0, 0},       /* Manufacture codes. */
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  &TC58FYM7T8C_TOP_geometry,
  &TC58FYM7T8C_TOP_geometry,
  &flash_toshiba_op_word_write_functions
};

flashi_nor_device TC58FYM7T8C_BOT =
{
  "TOSHIBA TC58FYM7T8C_BOT",
  2,                            /* # of codes to match */
  {0x0098, 0x009F, 0, 0},       /* Manufacture codes. */
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  &TC58FYM7T8C_BOT_geometry,
  &TC58FYM7T8C_BOT_geometry,
  &flash_toshiba_op_word_write_functions
};


/*===========================================================================

FUNCTION TOSHIBA_PROBE

DESCRIPTION
  Detect the presence of an TOSHIBA flash device.

DEPENDENCIES
  

RETURN VALUE
  NULL           - No TOSHIBA device detected.
  otherwise      - Pointer to fsi_nor_device

SIDE EFFECTS
  None

===========================================================================*/


flashi_nor_device * 
toshiba_nor_probe (volatile word *baseaddr)
{
  flashi_nor_device const **dev;

  word saved_int;
  word codes[4];
  int ids, ids_found;

  /*----------------------------------------------------------------*/
  /* Perform the ARM specific probe. */
  INTLOCK_SAV (saved_int);
  /* With ARM/THUMB, calling a pointer determins ARM or Thumb mode by
     the low bit of the address.  Adding one here causes it to call
     this as thumb code. */

  /* Call function to go get the codes from the device */
  flash_toshiba_get_id_codes(baseaddr, codes);
  
  INTFREE_SAV (saved_int);

  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  for (dev = toshiba_parts; *dev != NULL; dev++) {
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

  return (flashi_nor_device *) *dev;

}/* toshiba_nor_probe */

/*===========================================================================

FUNCTION FSI_TOSHIBA_ERASE

DESCRIPTION
  Initiate erase operation for an TOSHIBA part.

DEPENDENCIES
  

RETURN VALUE
  FLASH_SUCCESS            -  If erase operation was successfully initiated

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_toshiba_erase_start (flash_ptr_type baseaddr,
         dword offset)
{
  volatile word *wptr;

  wptr = baseaddr + BYTE_TO_WORD_OFFSET(offset);

  /* Command to do block erase */
  wptr[0x555] = 0xAA;
  wptr[0x2AA] = 0x55;
  wptr[0x555] = 0x80;
  wptr[0x555] = 0xAA;
  wptr[0x2AA] = 0x55;
  wptr[0x555] = 0x30;

  /* Do not wait for DQ3 based on Samsung recommendation */
  while((*wptr & FS_AMD_DQ3) == 0);    //lint !e722  no stmts in while loop

  return FLASH_SUCCESS;
}


/*===========================================================================

FUNCTION FSI_TOSHIBA_ERASE_STATUS

DESCRIPTION
  Erase status for TOSHIBA components.

DEPENDENCIES
  The device must be in the erasing state.

RETURN VALUE
  FLASH_SUCCESS               - The erase is finished, component is not in
                                read state
  FLASH_OP_NOT_COMPLETE       - The erase is still happening.
  FLASH_ERR_FATAL             - Something went wrong with the erase.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_toshiba_erase_status (flashi_nor_device *nor_device, 
                           flash_ptr_type eraseaddr )
{
  word prior, current;
  word failure_bit_seen=0;

  /*-----------------------------------------------------------------------*/

 reread:
  /*
   * The SPANSION Erase Status bits are sensitive to read access from
   * ANY area of the flash, not just the EFS area (contrary to the data sheet).
   *
   * For this reason, it's critical that we mask interrupts, to prevent
   * other tasks from executing from flash, and also execute from RAM
   * to prevent this code itself to be fetched from ROM in between the reads.
   *
   * It's important that these two back-to-back accesses to flash be the
   * ONLY accesses to flash. -SH 6-May-2005
   */
  
  flash_peek_twice(eraseaddr, &prior, &current);

  /* Check the "toggling bit". */
  if (((prior ^ current) & FS_AMD_DQ6) != 0)
  {
    /* It is toggling, check bit 5 to see if we hit an Erase Error */
    if ((current & FS_AMD_DQ5) != 0)
    {
      if (failure_bit_seen) 
      {

        /****************************************************************
        This indicates that we saw DQ5 set, verified that the toggle bits
        are still toggling, and DQ5 continues to be set for a second time. 
        This can only mean the flash device has failed and 
        we can not recover. 
        ****************************************************************/
        FLASH_ERR_FATAL("Device reported hard Erase Failure - DQ5", 0, 0, 0);
      }
      else 
      {
        /* The first time we see the DQ5 bit, we must check the toggle bits a
        the erase is still "running" and we did not just finish the erase */
               
        failure_bit_seen = 1; 
        goto reread;   /*lint !e801*/
      }
    } 

    /* It is still busy. */
    return FLASH_OP_NOT_COMPLETE;
  }
  else
  {
    /*
     * Here the DQ6 bit isn't toggling any more, indicating that Erase is
     * complete. (p49)
     *
     * There have been problems with the SPANSION part accidentally
     * indicating Erase Complete when it was not, in fact, complete.
     *
     * Rather than let the erase finish and corrupt the subsequent write
     * operation in chaotic ways, we trap any such failures here by
     * confirming that the block is readable and not still erasing.
     */
    current = *eraseaddr;           /* Read from erased block */
    if (current != 0xFFFF) {
       FLASH_ERR_FATAL ("SPANSION Erase completed prematurely! %x",
                         current, 0, 0);
    }

  #ifdef FLASH_CHECK  
    {
      dword i;
    #if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
      extern dword curr_erase_sector_bsize;
      dword size= curr_erase_sector_bsize >> 1;
    #else
      dword size = flash_nor_find_size_for_block_addr (nor_device,  
                                                       (dword)eraseaddr);       
      size >>= 1;
    #endif

      for (i = 0; i < size; i++)
      {
        if(eraseaddr[i] != 0xFFFF)
        {
          FLASH_ERR_FATAL("Erase verify failed",0,0,0);
        }
        if ((i % 16) == 0)
        {
          KICK_DOG_AND_CHECK_DATA();
        }
      }
    }
  #endif

    return FLASH_SUCCESS;
  }
}

/*======================================================================
 * FUNCTION FSI_AMD_PEEK
 *
 * Read a single word from a given address.  The read itself is performe
 * by code executing out of ram.  This works around a problem with
 * consecutive read cycles messing up the bit toggle in the AMD.
 */
static word amd_peek_code[] = {
  0x8800,                               /* ldrh  r0, [r0]    */
  0x46F7,                               /* mov   pc, lr      */
};

#define FSI_AMD_PEEK(addr) \
  (((word (*)()) ((unsigned char *) amd_peek_code + 1)) (addr))

/*===========================================================================
 * FUNCTION FSI_AMD_POKE
 *
 * Write a single word to a given address.  The write itself is performed
 * by code executing out of RAM.  This works around a ??? problem.
 */
#ifdef FEATURE_EFS_AMD_SUSPEND_FETCH_BUG
static word amd_poke_code[] = {
  0x8001,                               /* strh  r1, [r0]    */
  0x46F7,                               /* mov   pc, lr      */
};

#define FSI_AMD_POKE(addr, value) \
  (((word (*)()) ((unsigned char *) amd_poke_code + 1)) (addr, value))
#endif

/*===========================================================================
FUNCTION FSI_TOSHIBA_SUSPEND

DESCRIPTION
  Suspend an erase operation on TOSHIBA.  Correctly handles the
  race condition of the suspend finishing near the time of this call.

DEPENDENCIES
  The device be in erasing state and the fsi_erase_location has been set
  properly.

RETURN VALUE
  FLASH_SUCCESS             - The erase has completed.
  FLASH_OP_NOT_COMPLETE     - The erase was suspended, reads may be done.

SIDE EFFECTS
  None
===========================================================================*/
static int retry_count = 0;
static dword wait_amd_count = 20;

LOCAL flash_status
fsi_toshiba_suspend (flash_ptr_type eraseaddr)
{
  flash_status result = FLASH_SUCCESS;
  word tmp, tmp2;
  int done = 0;

  retry_count = 0;

  /* Issue the suspend erase command. */
  // *wptr = 0xB0;
#ifdef FEATURE_EFS_AMD_SUSPEND_FETCH_BUG
  INTLOCK ();
  (void)amd_poke_code[0];
  FSI_AMD_POKE (eraseaddr, 0xB0);
  INTFREE ();
#else
  *eraseaddr = 0xB0;
#endif

  /* Wait 20 micro sec for status to be valid. This is erase_suspend_latency
   * which is minimum time needed for suspend to take effect. If an active
   * erase operation was in progress, status information is not available
   * during the trasition from sector erase operation to erase suspend state.
   */
  clk_busy_wait(wait_amd_count);

  /* Read the status register. */
 retry:
  (void)amd_peek_code[0];
  tmp = FSI_AMD_PEEK (eraseaddr);

  /* Now look checking for status indicating that the suspend either
     happened, or that the part was already done erasing. */
  while (!done)
  {
    tmp2 = tmp;
    tmp = FSI_AMD_PEEK (eraseaddr);

    /* Possible conditions. */
    switch ((tmp ^ tmp2) & 0x44)
    {
    case 0x40: case 0x44:
      /* Bit 6 changing indicates that the erase hasn't suspended
         yet. */
      break;
    case 0x04:
      /* The suspend was successful. */
      result = FLASH_OP_NOT_COMPLETE;
      done = 1;
      break;
    case 0x00:
      /* The erase has finished.  The suspend didn't really
         happen. */

      /* Sometimes, the parts don't return status correctly the first
         time.  A normally completed erase should read with the value
         of 0xFFFF.  If this isn't what we got, try to read the status
         register again. */
      if (tmp != 0xFFFF) {
        retry_count++;
        if (retry_count < 5)
          goto retry;  /*lint !e801*/

        /* The device is not giving us meaningful status.  This seems to
         * happen (rarely) once in a while.  About the only thing to do
         * here, is start the erase back up again, and wait for it to
         * finish. */
        *eraseaddr = 0x30;

        /* The bit toggling seems to break until the erase itself finish
         * The only way that gives us to determine when the erase is don
         * is that the value 0xFFFF will appear.  This comes in about 5m
         * after the erase itself finishes.  If something is really wron
         * and the erase doesn't ever finish, the dog will eventually ha
         * to kick in. */
        do {
          tmp = FSI_AMD_PEEK (eraseaddr);
        } while (tmp != 0xFFFF);

        result = FLASH_SUCCESS;
        done = 1;
        break;
      }

      *eraseaddr = 0xF0;
      result = FLASH_SUCCESS;
      done = 1;
      break;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION FSI_TOSHIBA_RESUME

DESCRIPTION
  Resume a suspended erase.

DEPENDENCIES
  The fsi_toshiba_suspend must have sucessfully suspended the device.

RETURN VALUE
  FLASH_SUCCESS             - The erase has been resumed.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status
fsi_toshiba_resume (flash_ptr_type eraseaddr)
{
  /* Issue the resume command. */
#ifdef FEATURE_EFS_AMD_SUSPEND_FETCH_BUG
  INTLOCK ();
  FSI_AMD_POKE (eraseaddr, 0x30);
  INTFREE ();
#else
  *eraseaddr = 0x30;
#endif

  clk_busy_wait(4);
  return FLASH_SUCCESS;

}/* fsi_and_resume */

/*===========================================================================
FUNCTION FSI_TOSHIBA_CONFIGURE

DESCRIPTION
  Do any necessary initializtion.

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.

RETURN VALUE
  FLASH_SUCCESS             - Success

SIDE EFFECTS
  None
===========================================================================*/

LOCAL flash_status
fsi_toshiba_configure (fsi_device_t self, flash_ptr_type baseaddr)
{
  flash_status status = FLASH_SUCCESS;
  flashi_nor_device *nor_device = &(self->nor_dev_info);
  status = flash_nor_geometry_init(nor_device, baseaddr);
  
  if(status != FLASH_SUCCESS)
  {
    return status;
  }

  /* The following code resumes suspended erase if any. If not resumed here, the 
     next erase command would not actually erase but would resume the earlier 
     erase.  There may or may not be a suspended erase but here we make sure to 
     resume any if present. We perform this check only for tools and not for 
     AMSS as it is assumed that EFS starts with re-erasing any sectors if it 
     were suspended in last run. 
  */

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)

  #if !defined(FLASH_NOR_PARTI_TBL)
  {
    word prior, current;
    flash_ptr_type sector_addr;

    for( sector_addr = (flash_ptr_type) FLASH_NOR_EFS2_START_BYTE_OFFSET; 
         sector_addr < (flash_ptr_type) FLASH_NOR_EFS2_END_BYTE_OFFSET;
         sector_addr = sector_addr + FLASH_NOR_EFS2_SECTOR_BYTE_SIZE/2
       )
    { 
      KICK_DOG_AND_CHECK_DATA();

      flash_peek_twice(sector_addr, &prior , &current);
      if( ((prior ^ current) & FS_AMD_DQ2) != 0x0)
      {
        nor_device->ops->resume_erase( sector_addr );
        while (TRUE)
        {
          KICK_DOG_AND_CHECK_DATA();
          status = nor_device->ops->erase_status( nor_device, sector_addr ); 
          if( status != FLASH_OP_NOT_COMPLETE )
          {
            return status;             
          }
        } /* while */
      }
    } /* for */
  }
  #else
  {
    uint32 sector;
    word prior, current;
    flash_ptr_type sector_addr;
    for(sector = 0; sector < nor_device->geometry->num_total_blocks; sector++)
    {
      KICK_DOG_AND_CHECK_DATA();
      sector_addr = (flash_ptr_type)flash_nor_find_block_start_addr(nor_device, 
                                                                    sector);
      flash_peek_twice(sector_addr, &prior , &current);
      if( ((prior ^ current) & FS_AMD_DQ2) != 0x0)
      {
        nor_device->ops->resume_erase( sector_addr );
        while (TRUE)
        {
          KICK_DOG_AND_CHECK_DATA();
          status = nor_device->ops->erase_status( nor_device, sector_addr ); 
          if( status != FLASH_OP_NOT_COMPLETE )
          {
            return status;             
          }
        } /* while */
      }
    } /* for */
  }
  #endif /* !defined(FLASH_NOR_PARTI_TBL) */
#endif /* defined(BUILD_JFLASH) || defined(BUILD_ARMPRG) */

  return status;
}

/*===========================================================================

FUNCTION FSI_TOSHIBA_FAST_BYTE_WRITE

DESCRIPTION
  Write a block of bytes to an TOSHIBA part. This uses unlock bypass sequence 
  which enables fast writes supported by the Samsung parts.

DEPENDENCIES
  The fs_dev_init must have been called.  The device must not be in
  erasing state.

RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_TIMEOUT      -  If write operation timed out

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_toshiba_fast_byte_write (byte *buffer,
                              flash_ptr_type baseaddr,
                              dword offset,
                              dword count)
{
  volatile byte *part_base, *bptr;
  volatile word *wptr, *check_ptr;
  word value;
  word tmp;
  byte timeout_cnt=0;
  flash_status status = FLASH_SUCCESS;
  volatile word *unlock_base;

  /*---------------------------------------------------------------*/
  KICK_DOG_AND_CHECK_DATA();

  /* Base address of operation. */
  part_base = ((volatile byte *) baseaddr + offset);
  check_ptr = (volatile word *) ((dword) part_base & ~1);

  (void)check_ptr; // is this pointer and assignment above really needed...??!!

  unlock_base = (volatile word *) ((unsigned long) part_base & (~4095));
  unlock_base[0x555] = 0xAA; /* get into unlock bypass mode */
  unlock_base[0x2AA] = 0x55;
  unlock_base[0x555] = 0x20;

  /* Determine if the block is aligned or not. */
  if (((dword) buffer & 1) == 0
      && (offset & 1) == 0
      && (count & 1) == 0)
  {
    /* Do word operations. */
    wptr = (volatile word *) part_base;

    /* Since we're already aligned, we could compare with 0, but the
       1 will be needed for future optimizations. */
    while (count > 1)
    {
      KICK_DOG_AND_CHECK_DATA();
      value = *((word *) buffer);

      unlock_base[0x555] = 0xA0;
      *wptr = value;

      /* Wait 4 micro second before check for status.
         Per Spansion recommendation */
      clk_busy_wait(4);
  
  
      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
 
        /* Exit when finished. */
        if ((tmp & FS_AMD_DQ7) != (value & FS_AMD_DQ7))
        {  
          if ((tmp & FS_AMD_DQ5) != 0)
          {
            tmp = *wptr;
            /* DQ7 should be rechecked even if DQ5 = “1” because DQ7 
               may change simultaneously with DQ5. Refer flowchart on page 76 of 
               Spansion data sheet (S71PL254/127/064/032J_00 Revision A)*/
            if ((tmp & FS_AMD_DQ7) != (value & FS_AMD_DQ7))
            {
              status = FLASH_TIMEOUT;
            }          
            break;
          }
          else
          {
            clk_busy_wait(1);
            timeout_cnt++;            
            if(timeout_cnt >= TOSHIBA_MAX_TIMEOUT_CNT)
            {
              status = FLASH_TIMEOUT;
              break;
            }
          }
        }
        else
        {
          break; /* pass */
        }
      }

      if(status == FLASH_TIMEOUT)
        break;
      timeout_cnt = 0;
      buffer += 2;
      wptr   += 1;
      count  -= 2;
    }
  }
  else
  {
    /* Do byte operations. */
    bptr = part_base;

    while (count > 0)
    {
      KICK_DOG_AND_CHECK_DATA();
      value = *buffer;

      /* The command accesses need to be word oriented or the data
         will be placed on the wrong data lines.  Compute a word
         pointer from the byte pointer that is always at an even
         address. */
      wptr = ((volatile word *) ((dword) bptr & ~1));

      /* Get old value. */
      tmp = *wptr;

      unlock_base[0x555] = 0xA0;     

      /* Assumes little endian. */
      if (((dword) bptr & 1) == 0)
        value = tmp & (value | 0xFF00);
      else
        value = tmp & ((value << 8) | 0x00FF);
      *wptr = value;
 
      /* Wait 4 micro second before check for status.
         Per Spansion recommendation */
      clk_busy_wait(4);

      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
  
        /* Exit when finished. */
        if ((tmp & FS_AMD_DQ7) != (value & FS_AMD_DQ7))
        {  
          if ((tmp & FS_AMD_DQ5) != 0)
          {
            tmp = *wptr;
            /* DQ7 should be rechecked even if DQ5 = “1” because DQ7 
               may change simultaneously with DQ5. Refer flowchart on page 76 of 
               Spansion data sheet (S71PL254/127/064/032J_00 Revision A)*/
            if ((tmp & FS_AMD_DQ7) != (value & FS_AMD_DQ7))
            {
              status = FLASH_TIMEOUT;
            }          
            break;
          }
          else
          {
            clk_busy_wait(1);
            timeout_cnt++;            
            if(timeout_cnt >= TOSHIBA_MAX_TIMEOUT_CNT)
            {
              status = FLASH_TIMEOUT;
              break;
            }
          }
        }
        else
        {
          break; /* pass */
        }
      }

      if(status == FLASH_TIMEOUT)
        break;
      timeout_cnt = 0;
      buffer += 1;
      bptr   += 1;
      count  -= 1;
    }
  }

  unlock_base[0x0] = 0x90; /* get out of unlock bypass mode */
  unlock_base[0x0] = 0x00;
  
  KICK_DOG_AND_CHECK_DATA();

  return status;

}
