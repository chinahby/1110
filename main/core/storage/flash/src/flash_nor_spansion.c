/**********************************************************************
 * flash_nor_spansion.c
 *
 * Spansion specific flash support
 *
 * This file implements Spansion specific functions and data
 *
 * Copyright (c) 2005-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_spansion.c#13 $ $DateTime: 2008/06/09 14:50:52 $ $Author: c_surven $


when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-05-23   sv      Fix RVCT compiler warnings for tools compilation
2008-03-11   sv      Modified to support FLASH_CHECK functionality
2007-02-28   rt      Incorporate NOR partitioning structural changes
2006-09-25   rt      For Spansion WS parts use buffered_write only 
2006-08-09   ps      Added support for PL parts 
2005-12-05   dp      NOR Flash Driver Unification

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "flash_nor_device.h"
#include "flash_nor_msm.h"
#include "clk.h"
#include "flash_msg.h"

/*lint -e613 -e668 upper layers already check for null pointers */
/*lint -e526 Don't warn about undefined extern symbols*/
/*lint -emacro(611, FSI_AMD_POKE) function call pointer reference */
/*lint -emacro(746, FSI_AMD_POKE)*/
/*lint -emacro(522, FSI_AMD_POKE)*/
/*lint -emacro(746, FSI_AMD_PEEK)*/
/*lint -emacro(611, FSI_AMD_PEEK) function call pointer reference */

/* Checks every write and erase operation and make sures the flash contains the 
 * correct data after each operation is finished. Slows down performance of the
 * flash driver but good to check it up especially when testing a new device
 */
#undef FLASH_CHECK   

/* This should always be defined as the AMD part is broken with regards
 * to both fast write and suspend
 */
#define FEATURE_EFS_AMD_SUSPEND_FETCH_BUG

/* Spansion specifics */
#define SPANSION_BUFFER_WRITE_SIZE 64
#define SPANSION_MAX_TIMEOUT_CNT   20

/* Low level status bits for AMD. */
#define FS_AMD_DQ7      0x80
#define FS_AMD_DQ6      0x40
#define FS_AMD_DQ5      0x20
#define FS_AMD_DQ3      0x08
#define FS_AMD_DQ2      0x04

/* Low level status bits for SPANSION. */
#define DEV_RDY_MASK			(0x80)	/* Device Ready Bit */
#define DEV_ERASE_SUSP_MASK		(0x40)	/* Erase Suspend Bit */
#define DEV_ERASE_MASK			(0x20)	/* Erase Status Bit */
#define DEV_PROGRAM_MASK		(0x10)	/* Program Status Bit */
#define DEV_RFU_MASK			(0x08)	/* Reserved */
#define DEV_PROGRAM_SUSP_MASK	(0x04)	/* Program Suspend Bit */
#define DEV_SEC_LOCK_MASK		(0x02)	/* Sector lock Bit */
#define DEV_BANK_MASK			(0x01)	/* Operation in current bank */


/* This function is located in flash_ram.c. */
extern void flash_spansion_get_id_codes(volatile word *baseaddr, word *dest);
extern void flash_peek_twice(volatile word *wptr, word *prior, word *current);

/* Forward function prototypes */
LOCAL flash_status fsi_spansion_configure (fsi_device_t self,
                                           flash_ptr_type baseaddr);
LOCAL flash_status fsi_spansion_erase_start (flash_ptr_type baseaddr,
                                             dword offset);
LOCAL flash_status fsi_spansion_erase_status (flashi_nor_device *nor_device,
                                              flash_ptr_type eraseaddr);
LOCAL flash_status fsi_spansion_suspend (flash_ptr_type wptr);
LOCAL flash_status fsi_spansion_resume (flash_ptr_type eraseaddr);
LOCAL flash_status fsi_spansion_write (byte *buffer, flash_ptr_type baseaddr,
                                       dword offset, dword count);

LOCAL flash_status fsi_spansion_byte_write (byte *buffer,
                                            flash_ptr_type baseaddr,
                                            dword offset, dword count);
LOCAL flash_status fsi_spansion_fast_byte_write (byte *buffer,
                                                 flash_ptr_type baseaddr,
                                                 dword offset, dword count);
LOCAL flash_status fsi_spansion_buffer_write (byte *buffer,
                                              flash_ptr_type baseaddr,
                                              dword offset, dword count);

LOCAL flash_status fsi_spansion_erase_start_vs (flash_ptr_type baseaddr,
                                             dword offset);
LOCAL flash_status fsi_spansion_erase_status_vs (flashi_nor_device *nor_device,
                                              flash_ptr_type eraseaddr);
LOCAL flash_status fsi_spansion_suspend_vs (flash_ptr_type wptr);
LOCAL flash_status fsi_spansion_resume_vs (flash_ptr_type eraseaddr);
LOCAL flash_status fsi_spansion_write_vs (byte *buffer,
                                                 flash_ptr_type baseaddr,
                                                 dword offset, dword count);
LOCAL flash_status fsi_spansion_buffer_write_vs (byte *buffer,
                                              flash_ptr_type baseaddr,
                                              dword offset, dword count);

struct flashi_nor_dev_ops flash_spansion_op_functions =
{
  fsi_spansion_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                     /* Read operation. */
  fsi_spansion_write,                             /* Write for Spansion. */
  fsi_spansion_erase_start,                       /* Erase for Spansion. */
  fsi_spansion_erase_status,                      /* Status for Spansion. */
  fsi_spansion_suspend,
  fsi_spansion_resume,
};

struct flashi_nor_dev_ops flash_spansion_pl_op_functions =
{
  fsi_spansion_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                     /* Read operation. */
  fsi_spansion_fast_byte_write,                   /* Fast Write for Spansion */
  fsi_spansion_erase_start,                       /* Erase for Spansion. */
  fsi_spansion_erase_status,                      /* Status for Spansion. */
  fsi_spansion_suspend,
  fsi_spansion_resume,
};

struct flashi_nor_dev_ops flash_spansion_vs_op_functions =
{
  fsi_spansion_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                     /* Read operation. */
  fsi_spansion_write_vs,                          /* Write for Spansion */
  fsi_spansion_erase_start_vs,                    /* Erase for Spansion. */
  fsi_spansion_erase_status_vs,                   /* Status for Spansion. */
  fsi_spansion_suspend_vs,
  fsi_spansion_resume_vs,
};

 /* The fsi_nor_device structure will have minimal configuration information 
  * such as number of manufacture codes, actual manufacture codes, device
  * geometry info, partition info and the necessary function pointers. 
  * The other data such as number of sectors and the sector layout, etc 
  * are computed at run time depending on the flash used. So they have 
  * intialized value set as ZERO
  */  

flash_geometry_info S29WS256N0SB_geometry =
{
  FLASH_SPANSION_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  3,
  {
    {4, 32768},
    {254, 131072},
    {4, 32768},
  },  /*lint !e785*/
  262,
  16,
  {
    { 0,    19,  18,  0},
    { 19,   16,  34,  0},
    { 35,   16,  50,  0},
    { 51,   16,  66,  0},
    { 67,   16,  82,  0},
    { 83,   16,  98,  0},
    { 99,   16,  114,  0},
    { 115,  16,  130,  0},
    { 131,  16,  146,  0},
    { 147,  16,  162,  0},
    { 163,  16,  178,  0},
    { 179,  16,  194,  0},
    { 195,  16,  210,  0},
    { 211,  16,  226,  0},
    { 227,  16,  242,  0},
    { 243,  19,  261,  0},
  }
};  /*lint !e785*/
 
flash_geometry_info S29WS512P_geometry =
{
  FLASH_SPANSION_FAMILY,
  FLASH_SIZE_64MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  3,
  {
    {  4,  32768},
    {510, 131072},
    {  4,  32768},
    {  0,      0},
    {  0,      0},
  },  /*lint !e785*/
  518,
  16,
  {
    {  0,  35,  34,  0},
    { 35,  32,  66,  0},
    { 67,  32,  98,  0},
    { 99,  32, 130,  0},
    {131,  32, 162,  0},
    {163,  32, 194,  0},
    {195,  32, 226,  0},
    {227,  32, 258,  0},
    {259,  32, 290,  0},
    {291,  32, 322,  0},
    {323,  32, 354,  0},
    {355,  32, 386,  0},
    {387,  32, 418,  0},
    {419,  32, 450,  0},
    {451,  32, 482,  0},
    {483,  35, 517,  0},
  }
};  /*lint !e785*/

flashi_nor_device S29WS256N0SB = 
{
  "SPANSION S29WS256N0SB",
  4,                                              /* # of codes to match */
  {1,  0x227e, 0x2230, 0x2200 },                  /* Manufacture codes. */
  0, 
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29WS256N0SB_geometry,                         /* Handcoded Geometry info */
  &flash_spansion_op_functions
};

flashi_nor_device S29WS512P =
{
  "SPANSION S29WS512P",
  4,                                              /* # of codes to match */
  {1,  0x227e, 0x223D, 0x2200 },                  /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29WS512P_geometry,
  &flash_spansion_op_functions
};

flash_geometry_info S71PL032J80_srw_geometry =
{
  FLASH_SPANSION_FAMILY,
  FLASH_SIZE_4MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,
  3,            
  {
    {8, 8192},
    {62, 65536},
    {8, 8192}
  },  /*lint !e785*/
  78,
  4,
  {
    {  0,  15,  14,  0},
    { 15,  24,  38,  0},
    { 39,  24,  62,  0},
    { 63,  15,  77,  0},
  }  /*lint !e785*/
};  /*lint !e785*/
 
flashi_nor_device S71PL032J80_srw = 
{
  "SPANSION S71PL032J80",
  4,                                              /* # of codes to match */
  {1,  0x227e, 0x220A, 0x2201 },                  /* Manufacture codes. */
  0, 
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  INIT_USING_CFI_AT_RUNTIME,
  &S71PL032J80_srw_geometry,
  &flash_spansion_pl_op_functions
};

flash_geometry_info S29VS256R_TOP_geometry =
{
  FLASH_SPANSION_2_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  2,
  {
    {255, 131072},
    {4, 32768},
    {0, 0},
  },  /*lint !e785*/
  259,
  8,
  {
    { 0,    32,  31,  0},
    { 32,   32,  63,  0},
    { 64,   32,  95,  0},
    { 96,   32,  127,  0},
    { 128,  32,  159,  0},
    { 160,  32,  191,  0},
    { 192,  32,  223,  0},
    { 224,  35,  258,  0},
  }
};  /*lint !e785*/

flash_geometry_info S29VS256R_BOT_geometry =
{
  FLASH_SPANSION_2_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  2,
  {
    {4, 32768},
    {255, 131072},
    {0, 0},
  },  /*lint !e785*/
  259,
  8,
  {
    { 0,    35,  34,  0},
    { 35,   32,  66,  0},
    { 67,   32,  98,  0},
    { 99,   32,  130,  0},
    { 131,  32,  162,  0},
    { 163,  32,  194,  0},
    { 195,  32,  226,  0},
    { 227,  32,  258,  0},
  }
};  /*lint !e785*/


flash_geometry_info S29VS128R_TOP_geometry =
{
  FLASH_SPANSION_2_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  2,
  {
    {127, 131072},
    {4, 32768},
    {0, 0},
  },  /*lint !e785*/
  131,
  8,
  {
    { 0,    16,  15,  0},
    { 16,   16,  31,  0},
    { 32,   16,  47,  0},
    { 48,   16,  63,  0},
    { 64,   16,  79,  0},
    { 80,   16,  95,  0},
    { 96,   16,  111,  0},
    { 112,  19,  130,  0},
  }
};  /*lint !e785*/

flash_geometry_info S29VS128R_BOT_geometry =
{
  FLASH_SPANSION_2_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  2,
  {
    {4, 32768},
    {127, 131072},
    {0, 0},
  },  /*lint !e785*/
  131,
  8,
  {
    { 0,    19,  18,  0},
    { 19,   16,  34,  0},
    { 35,   16,  50,  0},
    { 51,   16,  66,  0},
    { 67,   16,  82,  0},
    { 83,   16,  98,  0},
    { 99,   16,  114,  0},
    { 115,  16,  130,  0},
  }
};  /*lint !e785*/

flashi_nor_device S29VS128R_TOP =
{
  "SPANSION S29VS128R_TOP",
  4,                                              /* # of codes to match */
  {1,  0x007E, 0x0063, 0x0001 },                  /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29VS128R_TOP_geometry,
  &flash_spansion_vs_op_functions
};

flashi_nor_device S29VS128R_BOT =
{
  "SPANSION S29VS128R_BOT",
  4,                                              /* # of codes to match */
  {1,  0x007E, 0x0065, 0x0001 },                  /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29VS128R_BOT_geometry,
  &flash_spansion_vs_op_functions
};

flashi_nor_device S29VS256R_TOP =
{
  "SPANSION S29VS256R_TOP",
  4,                                              /* # of codes to match */
  {1,  0x007E, 0x0064, 0x0001 },                  /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29VS256R_TOP_geometry,
  &flash_spansion_vs_op_functions
};

flashi_nor_device S29VS256R_BOT =
{
  "SPANSION S29VS256R_BOT",
  4,                                              /* # of codes to match */
  {1,  0x007E, 0x0066, 0x0001 },                  /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,
  0x0,                                            /* Device base address */
  0,                                              /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &S29VS256R_BOT_geometry,
  &flash_spansion_vs_op_functions
};

/*===========================================================================

FUNCTION SPANSION_PROBE

DESCRIPTION
  Detect the presence of an SPANSION flash device.

DEPENDENCIES
  

RETURN VALUE
  NULL           - No SPANSION device detected.
  otherwise      - Pointer to fsi_nor_device

SIDE EFFECTS
  None

===========================================================================*/


flashi_nor_device * 
spansion_probe (volatile word *baseaddr)
{
  flashi_nor_device const **dev;

  word saved_int;
  word  codes[4];
  int ids, ids_found;


  /*----------------------------------------------------------------*/


  /* Perform the ARM specific probe. */
  INTLOCK_SAV (saved_int);
  /* With ARM/THUMB, calling a pointer determins ARM or Thumb mode by
     the low bit of the address.  Adding one here causes it to call
     this as thumb code. */


  /* Call function to go get the codes from the device */
  flash_spansion_get_id_codes(baseaddr, codes);
  

  INTFREE_SAV (saved_int);


  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  for (dev = spansion_parts; *dev != NULL; dev++) {
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

}/* spansion_probe */

/*===========================================================================

FUNCTION wait_data_polling

DESCRIPTION
  This function uses DQ7 polling to detect a program completion.
  to complete.

DEPENDENCIES
  Uses perm.

RETURN VALUE
  Returns : FLASH_SUCCESS if the operation succeeds
            FLASH_TIMEOUT if it fails.

SIDE EFFECTS
  The watchdog is reset.

  The transport mechanism is checked for data while waiting.

  The device is left in normal (Read Array) ROM mode.
  This routine may take anywhere from microseconds to many seconds
  to execute, depending on the device operation.

===========================================================================*/

LOCAL flash_status
wait_data_polling (flash_ptr_type baseaddr, flash_ptr_type dest_wptr, word val)
{
  int timeout_count = 0;
  word temp;

  /* Start a new vote.  No toggles or  timeouts seen so far. */
  for (;;)
  {
    KICK_DOG_AND_CHECK_DATA();

    temp = *dest_wptr;
    /* Wait for the write to complete.  When it does, the Data
       Polling Bit will become equal to data bit #7 */
    if ((temp & 0x0080) != (val & 0x0080))
    {
      if ((temp & 0x0020) != 0)
      {
        timeout_count++;
        if (timeout_count >= 20)
        {
          /* Reset the device to get out of timeout mode */
          *(baseaddr + 0x0) = 0x00F0;

          DPRINTF (("wdp: Timeout @ addr 0x%x \n", dest_wptr));
          break;    /* Device timed out. */
        }
      }
    }
    else
      return FLASH_SUCCESS;
  }                             /* end for inner-loop */

  return FLASH_TIMEOUT;  /* Device timed out. */
}


/*===========================================================================

FUNCTION FSI_SPANSION_ERASE

DESCRIPTION
  Initiate erase operation for an SPANSION part.

DEPENDENCIES
  

RETURN VALUE
  FLASH_SUCCESS            -  If erase operation was successfully initiated

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_spansion_erase_start (flash_ptr_type baseaddr,
         dword offset)
{
  volatile word *wptr;

  wptr = baseaddr + BYTE_TO_WORD_OFFSET(offset);

  wptr[0x555] = 0xAA;
  wptr[0x2AA] = 0x55;
  wptr[0x555] = 0x80;
  wptr[0x555] = 0xAA;
  wptr[0x2AA] = 0x55;
  wptr[0x555] = 0x30;

  /* Do not wait for DQ3 based on Spansion recommendation */
  while((*wptr & FS_AMD_DQ3) == 0);    //lint !e722  no stmts in while loop

  return FLASH_SUCCESS;
}


/*===========================================================================

FUNCTION FSI_SPANSION_ERASE_STATUS

DESCRIPTION
  Erase status for SPANSION components.

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
fsi_spansion_erase_status (flashi_nor_device *nor_device, 
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
FUNCTION FSI_SPANSION_SUSPEND

DESCRIPTION
  Suspend an erase operation on SPANSION.  Correctly handles the
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
dword wait_amd_count = 20;

LOCAL flash_status
fsi_spansion_suspend (flash_ptr_type eraseaddr)
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
FUNCTION FSI_SPANSION_RESUME

DESCRIPTION
  Resume a suspended erase.

DEPENDENCIES
  The fsi_spansion_suspend must have sucessfully suspended the device.

RETURN VALUE
  FLASH_SUCCESS             - The erase has been resumed.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status
fsi_spansion_resume (flash_ptr_type eraseaddr)
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
FUNCTION FSI_SPANSION_CONFIGURE

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
fsi_spansion_configure (fsi_device_t self, flash_ptr_type baseaddr)
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

FUNCTION FSI_SPANSION_WRITE

DESCRIPTION
  Write a block of byte to Spansion flash part.

DEPENDENCIES
  The device must not be in  erasing state.

RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_FAILURE      -  If write operation failed
  FLASH_TIMEOUT      -  If write operation timed out
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_spansion_write (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)

{
  flash_status status = FLASH_SUCCESS;
  dword write_offset = offset;
  dword write_stride;
  dword num_to_write = count;
  byte *data_ptr = buffer;
  dword max_write_stride;
#ifdef FLASH_CHECK
  dword size = count;
  byte *source = buffer;
#endif

  /*----------------------------------------------------------------*/

  KICK_DOG_AND_CHECK_DATA();

  /* Since we are only using buffer write, we cannot handle non-word aligned
   * offsets and odd write lengths. Make sure that we are alright.
   */
  if ((offset & 0x1) || (count & 0x1))
  {
    /* Not word aligned or/and not even number of bytes */
    /* Assuming count != 0 */
    FLASH_ERR_FATAL("Not word aligned or/and odd number of bytes", 0, 0, 0);
  }

  /* Algorithm to split an arbitrary length of word-aligned non-write_buffer     
   * aligned data into buffer writetable chunks.
   *
   * In its most general for any given length of data can be divided into three
   * zones namely ZONE-1, ZONE-2 and ZONE-3 in that order, based on alignment of
   * their start and end addresses with the write buffer.
   *
   *
   *   ----------------------------------------------------------------------
   *       ZONE-1    |                ZONE-2                 |     ZONE-3
   *   ----------------------------------------------------------------------
   *
   *
   *       ZONE        Start Addr.         End Addr.              Length
   *      ------      -------------       -----------         ---------------
   *        1          Un-aligned      Aligned/Un-aligned      < WR_BUF_SIZE
   *        2           Aligned             Aligned          n * WR_BUF_SIZE
   *        3           Aligned            Un-aligned          < WR_BUF_SIZE
   * 
   *      where n is an integer which can take a values statring from 0.
   * 
   * All the ZONEs may not be present in a given length of data buffer. Note     
   * that ZONEs 1 and 3 are always less than size of write buffer. Zone 2 is     
   * an integral multiple of write buffer size.
   */

  while (num_to_write)
  {
    if ( write_offset & (SPANSION_BUFFER_WRITE_SIZE - 1) )
    {
      /* ZONE-1: The data is not aligned to the beginning of write buffer. */

      /* Note that this code segment will only get executed once. Possible 
       * subsequent writes, if any, will be write buffer aligned. Code could
       * further be optimized by moving this case out of the while loop.
       */

      /* Find the biggest length that can be written without crossing buffer 
       * boundary.
       */
      max_write_stride = ( (write_offset & ~(SPANSION_BUFFER_WRITE_SIZE - 1)) + 
                            SPANSION_BUFFER_WRITE_SIZE ) - write_offset;

      write_stride = MIN (num_to_write, max_write_stride);

    }
    else if (num_to_write >= SPANSION_BUFFER_WRITE_SIZE)
    {
      /* ZONE-2: The data is aligned to the beginning of write buffer. A full 
       *         length of buffer can be written.
       */
      write_stride = SPANSION_BUFFER_WRITE_SIZE;
      
    }
    else
    {
      /* ZONE-3: The data is aligned to the beginning of write buffer. Data 
       *         available to be written is less than full length of the write   
       *         buffer.
       */
      write_stride = num_to_write;
      
    }

    /* If further optimization is needed, a switch statement can be used here
     * to selectively use byte or buffer write for different lenghts that
     * need to be written.
     */
    status = fsi_spansion_buffer_write(data_ptr, baseaddr, write_offset,
                                       write_stride);

    if (status != FLASH_SUCCESS)
    {
      return status;
    }
    
    //KICK_DOG_AND_CHECK_DATA();
    data_ptr += write_stride;
    write_offset += write_stride;
    num_to_write -= write_stride;

  }

#ifdef FLASH_CHECK
  {
    dword i;
    volatile byte *part_base = ((volatile byte *) baseaddr + offset);

    for (i = 0; i < size; i++)
    {
      if(part_base[i] != source[i])
      {
        FLASH_ERR_FATAL("Write verify failed",0,0,0);
      }

      if ((i % 16)==0)
      {
        KICK_DOG_AND_CHECK_DATA();
      }
    }
  }
#endif

  return status;
}/* fsi_spansion_write */

/*===========================================================================

FUNCTION FSI_SPANSION_BUFFER_WRITE

DESCRIPTION
  Write a block of bytes to a SPANSION part.

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
fsi_spansion_buffer_write (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)
{
  flash_status status = FLASH_SUCCESS;
  word *word_buf_ptr = (word *)buffer;
  dword word_offset, word_count;
  dword last_word, status_word, i;
  volatile word *wptr;
  dword timeout_cnt = 0;

  /*----------------------------------------------------------------*/
  KICK_DOG_AND_CHECK_DATA();

  /* To do word operations convert byte offset to word offset */
  word_offset = offset>>1;
  wptr = (baseaddr + word_offset);

  /*========================================================
   *
   * Write buffer programming command sequences
   *
   * 1. Issue the unlock command 1   Addr: 0x555  Data: 0x00AA
   *
   * 2. Issue the unlock command 2   Addr: 0x2AA  Data: 0x0055
   *
   * 3. Write the buffer load
   *       Addr:  starting address   Data: 0x0025
   *
   * 4. Specify the number of program Location
   *       Addr:  Starting addresss     Data: word len
   *
   * 5. Load 1st data word
   *        Addr:  Starting Address     Data: program data
   *
   * 6. Load next data word
   *        Addr: Write buffer location Data: program data
   *
   * 7. ......
   *
   * 8. Load last data word
   *       Addr: Write buffer location  Data: program data
   *
   * 9. Write buffer program confirm.  This command must follow the last
   *     write buffer location loaded, or the operation will ABORT.
   *       Addr: Sector Address         Data: 0x0029
   *
   *==================================================================*/

  /* Commands to unlock the device */
  *(baseaddr + 0x0555L) = 0x00AA;
  *(baseaddr + 0x02AAL) = 0x0055;

  /* Command to do the buffer write */
  *wptr = 0x0025;

  /* Load the len of the buffer to the starting address */
  word_count = count>>1;
  *wptr = (word_count - 1);

  /* Prime the variable so that we can do increment the pointer inside the
   * loop. This way we do not need to keep track of the last location we
   * wrote to, to poll the status. Variable 'wptr' will always point to the
   * last location when for loop is exited
   */
  wptr--;

  /* Write to the buffer */
  for (i = 0; i < word_count; i++)
  {
    /* Put together the next word to write to Flash. */
    wptr++;
    *wptr = word_buf_ptr[i];     //lint !e734 will never exceed 2 bytes
  }

  /* Write buffer program confirm */
  *wptr = 0x29;

  /* Since index 'i' would have been incremented once before the loop exited,
   * adjust it to fetch the last word from data buffer.
   */
  last_word = word_buf_ptr[i - 1];

  clk_busy_wait(4);

  /* Wait for the write. */
  while (1)    //lint !e716 while(1) has break and return..
  {
    status_word = *wptr;   //lint !e794 current_wptr will have ptr assigned

    /* Exit when finished. */
    if ((status_word & FS_AMD_DQ7) != (last_word & FS_AMD_DQ7))
    {
      if ((status_word & FS_AMD_DQ5) != 0)
      {
        timeout_cnt++;
        if(timeout_cnt >= SPANSION_MAX_TIMEOUT_CNT)
        {
          *(baseaddr + 0x0555) = 0x00AA;
          *(baseaddr + 0x02AA) = 0x0055;
          *(baseaddr + 0x0555) = 0x00F0;
          FLASH_ERR_FATAL("Buffer Write status timeout",0,0,0);
        }
      }
    }
    else
    {
      break;
    }
  }/* while */

  KICK_DOG_AND_CHECK_DATA();

  return status;
}

/*===========================================================================

FUNCTION FSI_SPANSION_BYTE_WRITE

DESCRIPTION
  Write a block of bytes to an SPANSION part.

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
fsi_spansion_byte_write (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)
{

  volatile byte *part_base, *bptr;
  volatile word *wptr, *check_ptr;
  word value;
  word tmp;
  byte timeout_cnt=0;
  
  /*---------------------------------------------------------------*/
  KICK_DOG_AND_CHECK_DATA();

  /* Base address of operation. */
  part_base = ((volatile byte *) baseaddr + offset);
  check_ptr = (volatile word *) ((dword) part_base & ~1);

  (void)check_ptr; // is this pointer and assignment above really needed...??!!

  /* Determine if the block is aligned or not. */
  if (((dword) buffer & 1) == 0
      && (offset & 1) == 0
      && (count & 1) == 0)
  {
    /* Do word operations. */
    wptr = (volatile word *) part_base; /*lint !e826*/

    /* Since we're already aligned, we could compare with 0, but the
       1 will be needed for future optimizations. */
    while (count > 1)
    {
      value = *((word *) buffer); /*lint !e826*/

      *(baseaddr + 0x555) = 0xAA;
      *(baseaddr + 0x2AA) = 0x55;
      *(baseaddr + 0x555) = 0xA0;

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
            timeout_cnt++;            
            if(timeout_cnt >= SPANSION_MAX_TIMEOUT_CNT)
            {
              *wptr = 0xF0;
              return FLASH_TIMEOUT;
            }
          }
        }
        else
        {
          break;
        }
      }

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
      value = *buffer;

      /* The command accesses need to be word oriented or the data
         will be placed on the wrong data lines.  Compute a word
         pointer from the byte pointer that is always at an even
         address. */
      wptr = ((volatile word *) ((dword) bptr & ~1));

      /* Get old value. */
      tmp = *wptr;

      *(baseaddr + 0x555) = 0xAA;
      *(baseaddr + 0x2AA) = 0x55;
      *(baseaddr + 0x555) = 0xA0;

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
            timeout_cnt++;
            if(timeout_cnt >= SPANSION_MAX_TIMEOUT_CNT)
            {
              *(baseaddr + 0x0) = 0xF0;
              return FLASH_TIMEOUT;
            }
          }
        }
        else
        {
          break;
        }
      }

      timeout_cnt = 0;
      buffer += 1;
      bptr   += 1;
      count  -= 1;
    }
  }

  KICK_DOG_AND_CHECK_DATA();

  return FLASH_SUCCESS;

}
/*===========================================================================

FUNCTION FSI_SPANSION_FAST_BYTE_WRITE

DESCRIPTION
  Write a block of bytes to an SPANSION part. This uses unlock bypass sequence 
  which enables fast writes supported by the Spansion parts.

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
fsi_spansion_fast_byte_write (byte *buffer,
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
            if(timeout_cnt >= SPANSION_MAX_TIMEOUT_CNT)
            {
              status = FLASH_TIMEOUT;
              break;
            }
          }
        }
        else
        {
          break;
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
            if(timeout_cnt >= SPANSION_MAX_TIMEOUT_CNT)
            {
              status = FLASH_TIMEOUT;
              break;
            }
          }
        }
        else
        {
          break;
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

#define SA_OFFSET_MASK	0xFFFFF000	 /* mask off the offset */
/* LLD Command Definition */
#define NOR_CFI_QUERY_CMD                ((0x98)*LLD_DEV_MULTIPLIER)
#define NOR_CHIP_ERASE_CMD               ((0x10)*LLD_DEV_MULTIPLIER)
#define NOR_ERASE_SETUP_CMD              ((0x80)*LLD_DEV_MULTIPLIER)
#define NOR_RESET_CMD                    ((0xF0)*LLD_DEV_MULTIPLIER)
#define NOR_SECSI_SECTOR_ENTRY_CMD       ((0x88)*LLD_DEV_MULTIPLIER)
#define NOR_SECTOR_ERASE_CMD             ((0x30)*LLD_DEV_MULTIPLIER)
#define NOR_WRITE_BUFFER_LOAD_CMD        ((0x25)*LLD_DEV_MULTIPLIER)
#define NOR_WRITE_BUFFER_PGM_CONFIRM_CMD ((0x29)*LLD_DEV_MULTIPLIER) 
#define NOR_SET_CONFIG_CMD			     ((0xD0)*LLD_DEV_MULTIPLIER)
#define NOR_BIT_FIELD_CMD				 ((0xBF)*LLD_DEV_MULTIPLIER)

#define NOR_ERASE_SUSPEND_CMD			 ((0xB0)*LLD_DEV_MULTIPLIER)
#define NOR_ERASE_RESUME_CMD			 ((0x30)*LLD_DEV_MULTIPLIER)
#define NOR_PROGRAM_SUSPEND_CMD			 ((0x51)*LLD_DEV_MULTIPLIER)
#define NOR_PROGRAM_RESUME_CMD			 ((0x50)*LLD_DEV_MULTIPLIER)
#define NOR_STATUS_REG_READ_CMD			 ((0x70)*LLD_DEV_MULTIPLIER)
#define NOR_STATUS_REG_CLEAR_CMD		 ((0x71)*LLD_DEV_MULTIPLIER)
#define NOR_BLANK_CHECK_CMD				 ((0x33)*LLD_DEV_MULTIPLIER)

#define LLD_DEV_MULTIPLIER 0x00000001
#define LLD_DB_READ_MASK   0x0000FFFF
#define LLD_DEV_READ_MASK  0x0000FFFF
#define LLD_UNLOCK_ADDR1   0x00000555
#define LLD_UNLOCK_ADDR2   0x000002AA
#define LLD_BYTES_PER_OP   0x00000002
#define LLD_CFI_UNLOCK_ADDR1 0x00000555

#define FLASH_OFFSET(b,o)       (*(( (flash_ptr_type)(b) ) + (o)))
#define FLASH_WR(b,o,d) FLASH_OFFSET((b),(o)) = (d)
#define FLASH_RD(b,o)   FLASH_OFFSET((b),(o))

void lld_StatusRegReadCmd(flash_ptr_type base_addr, dword offset)
{         
  FLASH_WR(base_addr, (offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR1, NOR_STATUS_REG_READ_CMD); 
}

void lld_ProgramSuspendCmd(flash_ptr_type base_addr)
{         
  /* Write Suspend Command */
  FLASH_WR(base_addr, 0, NOR_PROGRAM_SUSPEND_CMD);
}

void lld_EraseSuspendCmd(flash_ptr_type base_addr)
{       
  /* Write Suspend Command */
  FLASH_WR(base_addr, 0, NOR_ERASE_SUSPEND_CMD);
}

void lld_EraseResumeCmd(flash_ptr_type base_addr)
{       
  /* Write Resume Command */
  FLASH_WR(base_addr, 0, NOR_ERASE_RESUME_CMD);
}

void lld_ProgramResumeCmd(flash_ptr_type base_addr)
{       
  /* Write Resume Command */
  FLASH_WR(base_addr, 0, NOR_PROGRAM_RESUME_CMD);
}

void lld_SectorEraseCmd(flash_ptr_type base_addr, dword offset)
{
  /* Issue Sector Erase Command Sequence */
  FLASH_WR(base_addr, (offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR1, NOR_ERASE_SETUP_CMD);
  FLASH_WR(base_addr, (offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR2, NOR_SECTOR_ERASE_CMD);
}

void lld_WriteToBufferCmd(flash_ptr_type base_addr, dword offset)
{  
  FLASH_WR(base_addr, (offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR1, NOR_WRITE_BUFFER_LOAD_CMD);
}
 
void lld_ProgramBufferToFlashCmd(flash_ptr_type base_addr, dword offset)
{
  /* Transfer Buffer to Flash Command */
  FLASH_WR(base_addr, (offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR1, NOR_WRITE_BUFFER_PGM_CONFIRM_CMD); 
}

LOCAL word lld_Poll(flash_ptr_type base_addr, dword offset)
{       
  unsigned long polling_counter = 0xFFFFFFFF;
  volatile word status_reg;

  do
  {
    polling_counter--;
	lld_StatusRegReadCmd( base_addr, offset );		/* Issue status register read command */
	status_reg = FLASH_RD(base_addr, offset);       /* read the status register */
	if( (status_reg & DEV_RDY_MASK) == DEV_RDY_MASK  )  /* Are all devices done bit 7 is 1 */
		break;

  }while(polling_counter);
  
  return( status_reg );          /* retrun the status reg. */
}

/*===========================================================================
FUNCTION FSI_SPANSION_SUSPEND

DESCRIPTION
  Suspend an erase operation on SPANSION.  Correctly handles the
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
LOCAL flash_status
fsi_spansion_suspend_vs (flash_ptr_type eraseaddr)
{
  word status_reg;

  /* Issue the suspend erase command. */
  lld_EraseSuspendCmd( eraseaddr );		/* issue erase suspend command */  

  status_reg = lld_Poll(eraseaddr, 0);        /* wait for device done */

  if( (status_reg & DEV_ERASE_SUSP_MASK) == DEV_ERASE_SUSP_MASK )
        return( FLASH_SUCCESS  );        /* Erase suspend  */
  return( FLASH_FAILURE );       /* Erase suspend error */
}

/*===========================================================================
FUNCTION FSI_SPANSION_RESUME

DESCRIPTION
  Resume a suspended erase.

DEPENDENCIES
  The fsi_spansion_suspend must have sucessfully suspended the device.

RETURN VALUE
  FLASH_SUCCESS             - The erase has been resumed.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status fsi_spansion_resume_vs (flash_ptr_type eraseaddr)
{
  /* Issue the resume command. */
  lld_EraseResumeCmd(eraseaddr);
  return FLASH_SUCCESS;
}/* fsi_and_resume */

/*===========================================================================

FUNCTION FSI_SPANSION_ERASE

DESCRIPTION
  Initiate erase operation for an SPANSION part.

DEPENDENCIES
  

RETURN VALUE
  FLASH_SUCCESS            -  If erase operation was successfully initiated

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status fsi_spansion_erase_start_vs (flash_ptr_type baseaddr, dword offset)
{
  lld_SectorEraseCmd(baseaddr,BYTE_TO_WORD_OFFSET(offset));
  return FLASH_SUCCESS;
}

/*===========================================================================

FUNCTION FSI_SPANSION_ERASE_STATUS

DESCRIPTION
  Erase status for SPANSION components.

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
LOCAL flash_status fsi_spansion_erase_status_vs (flashi_nor_device *nor_device, flash_ptr_type eraseaddr )
{
  word status_reg;
  
  lld_StatusRegReadCmd( eraseaddr, 0 );		/* Issue status register read command */
  status_reg = FLASH_RD(eraseaddr, 0);       /* read the status register */
  if( (status_reg & DEV_RDY_MASK) == DEV_RDY_MASK  )  /* Are all devices done bit 7 is 1 */
  {
    if( status_reg & DEV_SEC_LOCK_MASK )
	  return( FLASH_FAILURE );		/* sector locked */

    if( (status_reg & DEV_ERASE_MASK) == DEV_ERASE_MASK )
	  return( FLASH_FAILURE);		/* erase error */
    
	return FLASH_SUCCESS;
  }
  return FLASH_OP_NOT_COMPLETE;
}

/*===========================================================================

FUNCTION FSI_SPANSION_WRITE

DESCRIPTION
  Write a block of byte to Spansion flash part.

DEPENDENCIES
  The device must not be in  erasing state.

RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_FAILURE      -  If write operation failed
  FLASH_TIMEOUT      -  If write operation timed out
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_spansion_write_vs (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)

{
  flash_status status = FLASH_SUCCESS;
  dword write_offset = offset;
  dword write_stride;
  dword num_to_write = count;
  byte *data_ptr = buffer;
  dword max_write_stride;
#ifdef FLASH_CHECK
  dword size = count;
  byte *source = buffer;
#endif

  /*----------------------------------------------------------------*/

  KICK_DOG_AND_CHECK_DATA();

  /* Since we are only using buffer write, we cannot handle non-word aligned
   * offsets and odd write lengths. Make sure that we are alright.
   */
  if ((offset & 0x1) || (count & 0x1))
  {
    /* Not word aligned or/and not even number of bytes */
    /* Assuming count != 0 */
    FLASH_ERR_FATAL("Not word aligned or/and odd number of bytes", 0, 0, 0);
  }

  /* Algorithm to split an arbitrary length of word-aligned non-write_buffer     
   * aligned data into buffer writetable chunks.
   *
   * In its most general for any given length of data can be divided into three
   * zones namely ZONE-1, ZONE-2 and ZONE-3 in that order, based on alignment of
   * their start and end addresses with the write buffer.
   *
   *
   *   ----------------------------------------------------------------------
   *       ZONE-1    |                ZONE-2                 |     ZONE-3
   *   ----------------------------------------------------------------------
   *
   *
   *       ZONE        Start Addr.         End Addr.              Length
   *      ------      -------------       -----------         ---------------
   *        1          Un-aligned      Aligned/Un-aligned      < WR_BUF_SIZE
   *        2           Aligned             Aligned          n * WR_BUF_SIZE
   *        3           Aligned            Un-aligned          < WR_BUF_SIZE
   * 
   *      where n is an integer which can take a values statring from 0.
   * 
   * All the ZONEs may not be present in a given length of data buffer. Note     
   * that ZONEs 1 and 3 are always less than size of write buffer. Zone 2 is     
   * an integral multiple of write buffer size.
   */

  while (num_to_write)
  {
    if ( write_offset & (SPANSION_BUFFER_WRITE_SIZE - 1) )
    {
      /* ZONE-1: The data is not aligned to the beginning of write buffer. */

      /* Note that this code segment will only get executed once. Possible 
       * subsequent writes, if any, will be write buffer aligned. Code could
       * further be optimized by moving this case out of the while loop.
       */

      /* Find the biggest length that can be written without crossing buffer 
       * boundary.
       */
      max_write_stride = ( (write_offset & ~(SPANSION_BUFFER_WRITE_SIZE - 1)) + 
                            SPANSION_BUFFER_WRITE_SIZE ) - write_offset;

      write_stride = MIN (num_to_write, max_write_stride);

    }
    else if (num_to_write >= SPANSION_BUFFER_WRITE_SIZE)
    {
      /* ZONE-2: The data is aligned to the beginning of write buffer. A full 
       *         length of buffer can be written.
       */
      write_stride = SPANSION_BUFFER_WRITE_SIZE;
      
    }
    else
    {
      /* ZONE-3: The data is aligned to the beginning of write buffer. Data 
       *         available to be written is less than full length of the write   
       *         buffer.
       */
      write_stride = num_to_write;
      
    }

    /* If further optimization is needed, a switch statement can be used here
     * to selectively use byte or buffer write for different lenghts that
     * need to be written.
     */
    status = fsi_spansion_buffer_write_vs(data_ptr, baseaddr, write_offset,
                                       write_stride);

    if (status != FLASH_SUCCESS)
    {
      return status;
    }
    
    //KICK_DOG_AND_CHECK_DATA();
    data_ptr += write_stride;
    write_offset += write_stride;
    num_to_write -= write_stride;

  }

#ifdef FLASH_CHECK
  {
    dword i;
    volatile byte *part_base = ((volatile byte *) baseaddr + offset);

    for (i = 0; i < size; i++)
    {
      if(part_base[i] != source[i])
      {
        FLASH_ERR_FATAL("Write verify failed",0,0,0);
      }

      if ((i % 16)==0)
      {
        KICK_DOG_AND_CHECK_DATA();
      }
    }
  }
#endif

  return status;
}/* fsi_spansion_write */

/*===========================================================================

FUNCTION FSI_SPANSION_BUFFER_WRITE

DESCRIPTION
  Write a block of bytes to a SPANSION part.

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
fsi_spansion_buffer_write_vs (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)
{
  flash_status status = FLASH_SUCCESS;
  word *word_buf_ptr = (word *)buffer;
  dword word_offset, word_count;
  dword status_reg, i;
  volatile word *wptr;

  /*----------------------------------------------------------------*/
  KICK_DOG_AND_CHECK_DATA();

  /* To do word operations convert byte offset to word offset */
  word_offset = BYTE_TO_WORD_OFFSET(offset);
  wptr = (baseaddr + word_offset);
  /*========================================================
   *
   * Write buffer programming command sequences
   *
   * 1. Write Buffer Load Command Sector Address + 555h 0025h
   *
   * 2. Write Word Count Sector Address + 2AA Word Count (N¨C1)h
   *
   * 3. Load Buffer Word N Program Address, Word N Word N
   *
   * 4. Last Write Buffer to Flash Sector Address + 555h 0029h
   *
   *==================================================================*/

  /* Issue Load Write Buffer Command Sequence */
  lld_WriteToBufferCmd(baseaddr, word_offset);

  /* Write # of locations to program */
  word_count = count>>1;
  word_count = (word)word_count - 1;
  word_count *= LLD_DEV_MULTIPLIER;			/* For interleaving devices */
  
  /* In the datasheets of some latest Spansion devices, such as GLP, GLS, etc, the 
  command sequence of "write to buffer" command states the address of word count is 
  "Sector Address". Notice that to make LLD backward compatibility, the actual word 
  count address implemented is "Sector Address + LLD_UNLOCK_ADDR2", since the lower 
  address bits (a0-a15) are "don't care" bits and will be ignored anyway.
  */
  FLASH_WR(baseaddr, (word_offset & SA_OFFSET_MASK) + LLD_UNLOCK_ADDR2 , word_count);
  
  /* Write to the buffer */
  for (i = 0; i <= word_count; i++)
  {
    /* Put together the next word to write to Flash. */
    *wptr++ = word_buf_ptr[i];     //lint !e734 will never exceed 2 bytes
  }

  /* Issue Program Buffer to Flash command */
  lld_ProgramBufferToFlashCmd(baseaddr, word_offset);

  status_reg = lld_Poll(baseaddr, word_offset+word_count);

  if( status_reg & DEV_SEC_LOCK_MASK )
  {
	  status = FLASH_FAILURE;
  }

  if( (status_reg & DEV_PROGRAM_MASK) == DEV_PROGRAM_MASK )
  {
	  status = FLASH_FAILURE;		/* program error */
  }

  KICK_DOG_AND_CHECK_DATA();

  return status;
}

