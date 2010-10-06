/**********************************************************************
 * flash_nor_samsung.c
 *
 * samsung specific flash support
 *
 * This file implements Samsung specific functions and data
 *
 * Copyright (c) 2008-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_samsung.c#6 $ $DateTime: 2009/05/11 12:22:13 $ $Author: c_surven $


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

/* Samsung specifics */
#define SAMSUNG_BUFFER_WRITE_SIZE 128

/* Low level status bits for Samsung */
#define FLASH_STATUS_DQ7      0x80
#define FLASH_STATUS_DQ6      0x40
#define FLASH_STATUS_DQ5      0x20
#define FLASH_STATUS_DQ3      0x08
#define FLASH_STATUS_DQ2      0x04

/* This function is located in flash_ram.c. */
extern void flash_samsung_get_id_codes(volatile word *baseaddr, word *dest);
extern void flash_peek_twice(volatile word *wptr, word *prior, word *current);

/* Forward function prototypes */
LOCAL flash_status fsi_samsung_configure (fsi_device_t self,
                                           flash_ptr_type baseaddr);
LOCAL flash_status fsi_samsung_erase_start (flash_ptr_type baseaddr,
                                             dword offset);
LOCAL flash_status fsi_samsung_erase_status (flashi_nor_device *nor_device,
                                              flash_ptr_type eraseaddr);
LOCAL flash_status fsi_samsung_suspend (flash_ptr_type wptr);
LOCAL flash_status fsi_samsung_resume (flash_ptr_type eraseaddr);
LOCAL flash_status fsi_samsung_write (byte *buffer, flash_ptr_type baseaddr,
                                       dword offset, dword count);

LOCAL flash_status fsi_samsung_byte_write (byte *buffer,
                                            flash_ptr_type baseaddr,
                                            dword offset, dword count);
LOCAL flash_status fsi_samsung_fast_byte_write (byte *buffer,
                                                 flash_ptr_type baseaddr,
                                                 dword offset, dword count);
LOCAL flash_status fsi_samsung_buffer_write (byte *buffer,
                                              flash_ptr_type baseaddr,
                                              dword offset, dword count);

LOCAL void fsi_samsung_unprotect_blocks(fsi_device_t self);

struct flashi_nor_dev_ops flash_samsung_op_functions =
{
  fsi_samsung_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                    /* Read operation. */
  fsi_samsung_write,                             /* Write for Samsung. */
  fsi_samsung_erase_start,                       /* Erase for Samsung. */
  fsi_samsung_erase_status,                      /* Status for Samsung. */
  fsi_samsung_suspend,
  fsi_samsung_resume,
  NULL,
  NULL,
  NULL
};

struct flashi_nor_dev_ops flash_samsung_op_word_write_functions =
{
  fsi_samsung_configure,                         /* Config of hardware */
  fsi_nor_device_worded_read,                    /* Read operation. */
  fsi_samsung_fast_byte_write,                   /* Write for Samsung. */
  fsi_samsung_erase_start,                       /* Erase for Samsung. */
  fsi_samsung_erase_status,                      /* Status for Samsung. */
  fsi_samsung_suspend,
  fsi_samsung_resume,
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
flash_geometry_info K5N5629ABM_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_128,  
  2,
  {
    {4, 32768},
    {255, 131072},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  259,
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
    { 243,  16,  258,  0},
}
};  /*lint !e785*/

flash_geometry_info K5N5666ATM_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_128,  
  2,
  {
    {255, 131072},
    {4, 32768},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  259,
  16,
  {
    { 0,    16,  15,  0},
    { 16,   16,  31,  0},
    { 32,   16,  47,  0},
    { 48,   16,  63,  0},
    { 64,   16,  79,  0},
    { 80,   16,  95,  0},
    { 96,   16,  111,  0},
    { 112,  16,  127,  0},
    { 128,  16,  143,  0},
    { 144,  16,  159,  0},
    { 160,  16,  175,  0},
    { 176,  16,  191,  0},
    { 192,  16,  207,  0},
    { 208,  16,  223,  0},
    { 224,  16,  239,  0},
    { 240,  19,  258,  0},
}
};  /*lint !e785*/

flash_geometry_info K5N6433ABM_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_8MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,  
  2,
  {
    {8, 8192},
    {127, 65536},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  135,
  16,
  {
    { 0,    15,  14,  0},
    { 15,   8,  22,  0},
    { 23,   8,  30,  0},
    { 31,   8,  38,  0},
    { 39,   8,  46,  0},
    { 47,   8,  54,  0},
    { 55,   8,  62,  0},
    { 63,  8,  70,  0},
    { 71,  8,  78,  0},
    { 79,  8,  86,  0},
    { 87,  8,  94,  0},
    { 95,   8,  102,  0},
    { 103,  8,  110,  0},
    { 111,  8,  118,  0},
    { 119,  8,  126,  0},
    { 127,  8,  134,  0},
  }
};  /*lint !e785*/


flash_geometry_info K5N6433ATM_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_8MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,  
  2,
  {
    {127, 65536},
    {8, 8192},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  135,
  16,
  {
    { 0,    16,  15,  0},
    { 16,   16,  31,  0},
    { 32,   16,  47,  0},
    { 48,   16,  63,  0},
    { 64,   16,  79,  0},
    { 80,   16,  95,  0},
    { 96,   16,  111,  0},
    { 112,  16,  127,  0},
    { 128,  16,  143,  0},
    { 144,  16,  159,  0},
    { 160,  16,  175,  0},
    { 176,  16,  191,  0},
    { 192,  16,  207,  0},
    { 208,  16,  223,  0},
    { 224,  16,  239,  0},
    { 240,  19,  258,  0},
  }
};  /*lint !e785*/

flash_geometry_info K5N2833ATB_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_16MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,  
  2,
  {
    {255, 65536},
    {8, 8192},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  263,
  16,
  {
    { 0,    8,   7,  0},
    { 8,    8,  15,  0},
    { 16,   8,  23,  0},
    { 24,   8,  31,  0},
    { 32,   8,  39,  0},
    { 40,   8,  47,  0},
    { 48,   8,  55,  0},
    { 56,   8,  63,  0},
    { 64,   8,  71,  0},
    { 72,   8,  79,  0},
    { 80,   8,  87,  0},
    { 88,   8,  95,  0},
    { 96,   8,  103, 0},
    { 104,  8,  111, 0},
    { 112,  8,  119, 0},
    { 120,  15, 134, 0},
  }
};  /*lint !e785*/

flash_geometry_info K5N2833ABB_geometry =
{
  FLASH_SAMSUNG_FAMILY,
  FLASH_SIZE_16MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,  
  2,
  {
    {8, 8192},
    {255, 65536},
    {0,      0},
    {0,      0},
  },  /*lint !e785*/
  263,
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
    { 243,  16,  258,  0},
  }
};  /*lint !e785*/

flashi_nor_device K5N5629ABM =
{
  "SAMSUNG K5N5629ABM",
  2,                            /* # of codes to match */
  {0x00EC, 0x2601, 0, 0},       /* Manufacture codes. */
                                /* 2600H(Top Boot), 2601H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_PAIRED_BITS, /* for MLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N5629ABM_geometry,
  &flash_samsung_op_functions
};
  
flashi_nor_device K5N5666ATM =
{
  "SAMSUNG K5N5666ATM",
  2,                            /* # of codes to match */
  {0x00EC, 0x2208, 0, 0},       /* Manufacture codes. */
                                /* 2208H(Top Boot), 2209H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_PAIRED_BITS, /* for MLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N5666ATM_geometry,
  &flash_samsung_op_functions
};

flashi_nor_device K5N6433ABM =
{
  "SAMSUNG K5N6433ABM",
  2,                            /* # of codes to match */
  {0x00EC, 0x2255, 0, 0},       /* Manufacture codes. */
                                /* 2254H(Top Boot), 2255H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N6433ABM_geometry,
  &flash_samsung_op_word_write_functions
};

flashi_nor_device K5N6433ATM =
{
  "SAMSUNG K5N6433ATM",
  2,                            /* # of codes to match */
  {0x00EC, 0x2254, 0, 0},       /* Manufacture codes. */
                                /* 2254H(Top Boot), 2255H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N6433ATM_geometry,
  &flash_samsung_op_word_write_functions
};

flashi_nor_device K5N2833ATB =
{
  "SAMSUNG K5N2833-66ATB",
  2,                            /* # of codes to match */
  {0x00EC, 0x2404, 0, 0},       /* Manufacture codes. */
                                /* 2254H(Top Boot), 2255H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N2833ATB_geometry,
  &flash_samsung_op_word_write_functions
};

flashi_nor_device K5N2833ABB =
{
  "SAMSUNG K5N2833-66ABB",
  2,                            /* # of codes to match */
  {0x00EC, 0x2405, 0, 0},       /* Manufacture codes. */
                                /* 2254H(Top Boot), 2255H(Bottom Boot)*/
  0,                         
  FS_DEVICE_WRITES_SIMPLE,      /* for SLC device */
  0x0,                          /* Device base address */
  0,                            /* Page Size */
  /* Use CFI to initialize flash geometry */
  INIT_USING_CFI_AT_RUNTIME,
  &K5N2833ABB_geometry,
  &flash_samsung_op_word_write_functions
};


#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  /* Buffer initialized to value of erased data to use with memcmp() */
  static uint8 erase_verify_buf[ERASE_VERIFY_BLK];
#endif


/*===========================================================================

FUNCTION SAMSUNG_PROBE

DESCRIPTION
  Detect the presence of an SAMSUNG flash device.

DEPENDENCIES
  

RETURN VALUE
  NULL           - No SAMSUNG device detected.
  otherwise      - Pointer to fsi_nor_device

SIDE EFFECTS
  None

===========================================================================*/


flashi_nor_device * 
samsung_nor_probe (volatile word *baseaddr)
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
  flash_samsung_get_id_codes(baseaddr, codes);
  
  INTFREE_SAV (saved_int);

  /* Scan for the codes.  Check indicated number of ids for
     each component in the list.  Stop when we find a match.
     our indicator of no match. */
  for (dev = samsung_parts; *dev != NULL; dev++) {
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

}/* samsung_nor_probe */

/*===========================================================================

FUNCTION FSI_SAMSUNG_ERASE

DESCRIPTION
  Initiate erase operation for an SAMSUNG part.

DEPENDENCIES
  

RETURN VALUE
  FLASH_SUCCESS            -  If erase operation was successfully initiated

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
fsi_samsung_erase_start (flash_ptr_type baseaddr,
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
  while((*wptr & FLASH_STATUS_DQ3) == 0);    //lint !e722  no stmts in while loop

  return FLASH_SUCCESS;
}


/*===========================================================================

FUNCTION FSI_SAMSUNG_ERASE_STATUS

DESCRIPTION
  Erase status for SAMSUNG components.

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
fsi_samsung_erase_status (flashi_nor_device *nor_device, 
                           flash_ptr_type eraseaddr )
{
  word prior, current;
  word failure_bit_seen=0;

  /*-----------------------------------------------------------------------*/

 reread:
  /*
   * The Erase Status bits are sensitive to read access from
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
  if (((prior ^ current) & FLASH_STATUS_DQ6) != 0)
  {
    /* It is toggling, check bit 5 to see if we hit an Erase Error */
    if ((current & FLASH_STATUS_DQ5) != 0)
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
     * complete.
     *
     * Rather than let the erase finish and corrupt the subsequent write
     * operation in chaotic ways, we trap any such failures here by
     * confirming that the block is readable and not still erasing.
     */
    current = *eraseaddr;           /* Read from erased block */
    if (current != 0xFFFF) {
       FLASH_ERR_FATAL ("SAMSUNG Erase completed prematurely! %x",
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
 * FUNCTION FSI_PEEK
 *
 * Read a single word from a given address.  The read itself is performed
 * by code executing out of ram.
 */
static word fsi_peek_code[] = {
  0x8800,                               /* ldrh  r0, [r0]    */
  0x46F7,                               /* mov   pc, lr      */
};

#define FSI_PEEK(addr) \
  (((word (*)()) ((unsigned char *) fsi_peek_code + 1)) (addr))

/*===========================================================================
FUNCTION FSI_SAMSUNG_SUSPEND

DESCRIPTION
  Suspend an erase operation on SAMSUNG.  Correctly handles the
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
static dword wait_count = 30;

LOCAL flash_status
fsi_samsung_suspend (flash_ptr_type eraseaddr)
{
  flash_status result = FLASH_SUCCESS;
  word tmp, tmp2;
  int done = 0;

  retry_count = 0;

  /* Issue the suspend erase command. */
  *eraseaddr = 0xB0;

  /* Wait 30 micro sec for status to be valid. This is erase_suspend_latency
   * which is minimum time needed for suspend to take effect. If an active
   * erase operation was in progress, status information is not available
   * during the transition from sector erase operation to erase suspend state.
   */
  clk_busy_wait(wait_count);

  /* Read the status register. */
 retry:
  (void)fsi_peek_code[0];
  tmp = FSI_PEEK (eraseaddr);

  /* Now look checking for status indicating that the suspend either
     happened, or that the part was already done erasing. */
  while (!done)
  {
    tmp2 = tmp;
    tmp = FSI_PEEK (eraseaddr);

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
          tmp = FSI_PEEK (eraseaddr);
        } while (tmp != 0xFFFF);

        result = FLASH_SUCCESS;
        done = 1;
        break;
      }

      *eraseaddr = 0xF0;
      result = FLASH_SUCCESS;
      done = 1;
      break;
    default:
      break;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION FSI_SAMSUNG_RESUME

DESCRIPTION
  Resume a suspended erase.

DEPENDENCIES
  The fsi_samsung_suspend must have sucessfully suspended the device.

RETURN VALUE
  FLASH_SUCCESS             - The erase has been resumed.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status
fsi_samsung_resume (flash_ptr_type eraseaddr)
{
  /* Issue the resume command. */
  *eraseaddr = 0x30;
  
  clk_busy_wait(30);

  return FLASH_SUCCESS;

}/* fsi_and_resume */

/*===========================================================================
FUNCTION FSI_SAMSUNG_CONFIGURE

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
fsi_samsung_configure (fsi_device_t self, flash_ptr_type baseaddr)
{
  flash_status status = FLASH_SUCCESS;
  flashi_nor_device *nor_device = &(self->nor_dev_info);
  status = flash_nor_geometry_init(nor_device, baseaddr);

  fsi_samsung_unprotect_blocks(self);

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  {
    word i;
    /* Initialize the erased value array for quick erase compares. */
    for (i = 0; i < ERASE_VERIFY_BLK; i++)
    {
      erase_verify_buf[i] = (uint8)0xFF;
    }
  }
#endif
  
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
      if( ((prior ^ current) & FLASH_STATUS_DQ2) != 0x0)
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
      if( ((prior ^ current) & FLASH_STATUS_DQ2) != 0x0)
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
FUNCTION FSI_SAMSUNG_UNPROTECT_BLOCKS

DESCRIPTION
  Unprotect all the blocks in the flash if running software download or
  unprotect the blocks in the EFS region if running AMSS.
  (Note: On power up, all blocks in samsung device are protected.)
  
DEPENDENCIES
  Must be done once before any erase/write operations are carried out
  on the flash after power up.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void fsi_samsung_unprotect_blocks(fsi_device_t self)
{
#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  flashi_nor_device *nor_device = &(self->nor_dev_info);
  volatile word *wptr;
  uint16 count, block;
  uint32 curraddr = 0;

  /* Unprotect all blocks for programming */
  for (count = 0; (count < nor_device->geometry->num_erase_blk_region);
    count++)
  {
    for (block = 0; block < 
      nor_device->geometry->blk_regions[count].numblocks; block++)
    {
      wptr = nor_device->base_addr + BYTE_TO_WORD_OFFSET(curraddr);
      /* Command to unprotect the block */
      wptr[0] = 0xF0;
      wptr[0] = 0x60;
      wptr[0] = 0x60;
      wptr[0x42] = 0x60;
      curraddr += nor_device->geometry->blk_regions[count].blocksize_bytes;;
      KICK_DOG_AND_CHECK_DATA();
    }        
  } /* for (count = 0; count... */
#elif !defined(FLASH_NOR_PARTI_TBL)
  flashi_nor_device *nor_device = &(self->nor_dev_info);
  volatile word *wptr;
  uint32 count, block_size;
  
  block_size = flash_nor_find_size_for_block_addr(nor_device, 
    (uint32)FLASH_NOR_EFS2_START_BYTE_OFFSET);
  if (block_size != 0)
  {    
    for (count = (uint32) FLASH_NOR_EFS2_START_BYTE_OFFSET; 
      count < (uint32) FLASH_NOR_EFS2_END_BYTE_OFFSET; count += block_size)
    {
      wptr = nor_device->base_addr + BYTE_TO_WORD_OFFSET(count);
      /* Command to unprotect the block */
      wptr[0] = 0xF0;
      wptr[0] = 0x60;
      wptr[0] = 0x60;
      wptr[0x42] = 0x60;
      KICK_DOG_AND_CHECK_DATA();
    }
  }         
#else
  FLASH_ERR_FATAL("Samsung NOR flash not supported in partition mode" ,0 ,0 ,0);
#endif
}

/*===========================================================================

FUNCTION FSI_SAMSUNG_WRITE

DESCRIPTION
  Write a block of byte to Samsung flash part.

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
fsi_samsung_write (byte *buffer,
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
    if ( write_offset & (SAMSUNG_BUFFER_WRITE_SIZE - 1) )
    {
      /* ZONE-1: The data is not aligned to the beginning of write buffer. */

      /* Note that this code segment will only get executed once. Possible 
       * subsequent writes, if any, will be write buffer aligned. Code could
       * further be optimized by moving this case out of the while loop.
       */

      /* Find the biggest length that can be written without crossing buffer 
       * boundary.
       */
      max_write_stride = ( (write_offset & ~(SAMSUNG_BUFFER_WRITE_SIZE - 1)) + 
                            SAMSUNG_BUFFER_WRITE_SIZE ) - write_offset;

      write_stride = MIN (num_to_write, max_write_stride);

    }
    else if (num_to_write >= SAMSUNG_BUFFER_WRITE_SIZE)
    {
      /* ZONE-2: The data is aligned to the beginning of write buffer. A full 
       *         length of buffer can be written.
       */
      write_stride = SAMSUNG_BUFFER_WRITE_SIZE;
      
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
    status = fsi_samsung_buffer_write(data_ptr, baseaddr, write_offset,
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
}/* fsi_samsung_write */

/*===========================================================================

FUNCTION FSI_SAMSUNG_BUFFER_WRITE

DESCRIPTION
  Write a block of bytes to a SAMSUNG part.

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
fsi_samsung_buffer_write (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)
{
  flash_status status = FLASH_SUCCESS;
  word *word_buf_ptr = (word *)buffer;
  dword word_count;
  dword last_word, status_word, i;
  volatile word *wptr;

  /*----------------------------------------------------------------*/
  KICK_DOG_AND_CHECK_DATA();

  /* To do word operations convert byte offset to word offset */
  wptr = (baseaddr + BYTE_TO_WORD_OFFSET(offset));

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

  /* Wait for the write. */
  while (1)    //lint !e716 while(1) has break and return..
  {
    status_word = *wptr;   //lint !e794 current_wptr will have ptr assigned

    /* Exit when finished. */
    if ((status_word & FLASH_STATUS_DQ7) != (last_word & FLASH_STATUS_DQ7))
    {
      if ((status_word & FLASH_STATUS_DQ5) != 0)
      {
        status_word = *wptr;
        if ((status_word & FLASH_STATUS_DQ7) != (last_word & FLASH_STATUS_DQ7))
        {
          status = FLASH_FAILURE;
          /* Issue Reset command when write failure occurs to bring
           *  the flash to Read mode
           */
          *wptr = 0xF0;
        }          
        break;
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

FUNCTION FSI_SAMSUNG_BYTE_WRITE

DESCRIPTION
  Write a block of bytes to an SAMSUNG part.

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
fsi_samsung_byte_write (byte *buffer,
               flash_ptr_type baseaddr,
               dword offset,
               dword count)
{

  volatile byte *part_base, *bptr;
  volatile word *wptr, *check_ptr;
  word value;
  word tmp;
  flash_status status = FLASH_SUCCESS;
  
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
  
      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
 
        /* Exit when finished. */
        if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
        {  
          if ((tmp & FLASH_STATUS_DQ5) != 0)
          {
            tmp = *wptr;
            if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
            {
              status = FLASH_FAILURE;
              /* Issue Reset command when write failure occurs to bring
               *  the flash to Read mode
               */
              *wptr = 0xF0;
            }          
            break;
          }
        }
        else
        {
          break; /* pass */
        }
      }

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
 
      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
  
        /* Exit when finished. */
        if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
        {
          if ((tmp & FLASH_STATUS_DQ5) != 0)
          {
            tmp = *wptr;
            if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
            {
              status = FLASH_FAILURE;
              /* Issue Reset command when write failure occurs to bring
               *  the flash to Read mode
               */
              *wptr = 0xF0;
            }          
            break;
          }
        }
        else
        {
          break; /* pass */
        }
      }

      buffer += 1;
      bptr   += 1;
      count  -= 1;
    }
  }

  KICK_DOG_AND_CHECK_DATA();

  return status;

}
/*===========================================================================

FUNCTION FSI_SAMSUNG_FAST_BYTE_WRITE

DESCRIPTION
  Write a block of bytes to an SAMSUNG part. This uses unlock bypass sequence 
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
fsi_samsung_fast_byte_write (byte *buffer,
                              flash_ptr_type baseaddr,
                              dword offset,
                              dword count)
{

  volatile byte *part_base, *bptr;
  volatile word *wptr, *check_ptr;
  word value;
  word tmp;
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
  
      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
 
        /* Exit when finished. */
        if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
        {  
          if ((tmp & FLASH_STATUS_DQ5) != 0)
          {
            tmp = *wptr;
            /* DQ7 should be rechecked even if DQ5 = “1” because DQ7 
               may change simultaneously with DQ5. Refer flowchart on page 26 of 
               Samsung data sheet (K5N5629ABM-AD11, Rev 0.0)*/
            if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
            {
              status = FLASH_FAILURE;
              /* Issue Reset command when write failure occurs to bring
               *  the flash to Read mode
               */
              *wptr = 0xF0;
            }          
            break;
          }
        }
        else
        {
          break; /* pass */
        }
      }

      if(status != FLASH_SUCCESS)
        break;
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

      /* Wait for the write. */
      while (1)    //lint !e716 while(1) has break and return..
      {
        tmp = *wptr;
  
        /* Exit when finished. */
        if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
        {  
          if ((tmp & FLASH_STATUS_DQ5) != 0)
          {
            tmp = *wptr;
            if ((tmp & FLASH_STATUS_DQ7) != (value & FLASH_STATUS_DQ7))
            {
              status = FLASH_FAILURE;
              /* Issue Reset command when write failure occurs to bring
               *  the flash to Read mode
               */
              *wptr = 0xF0;
            }          
            break;
          }
        }
        else
        {
          break; /* pass */
        }
      }

      if(status != FLASH_SUCCESS)
        break;
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
