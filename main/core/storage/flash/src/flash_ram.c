
/**********************************************************************
 * flash_ram.c
 *
 * Flash functions located in RAM
 *
 * This file implements functions and data used to access partitions
 * for flash parts
 *
 * Copyright (c) 2007-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-07-01   jz      Added Samsung NOR flash support
2008-03-11   sv      Featurize inclusion of NSRW headers
2007-09-21   op      Added extended CFI support
2006-08-09   ps      Added support for PL parts 
2006-01-23   dp      Added Intel MLC support
2005-12-05   dp      NOR Flash Driver Unification
2005-05-23   dng     Created
===========================================================================*/

#include "flashi.h"
#include "clk.h"
#include "flash_nor_msm.h"
#include "flash_nor_device.h"

#ifdef FEATURE_NSRW_FLASH
  #include "flash_nor_nsrw.h"
  #include "flash_nor_spansion_nsrw.h"
#endif

#include "flash_msg.h"

#ifndef BOOT_LOADER
#endif /* !BOOT_LOADER */


/****************************************************************
 * Global Data
 ***************************************************************/
/*lint -e613 -e668 upper layers already check for null pointers */
//lint -e714 don't show undefined extern symbols

/****************************************************************
 * Forward function declarations
 ***************************************************************/
void flash_spansion_copy_cfi_data(volatile word *baseaddr, word *dest, word count);
void flash_samsung_copy_cfi_data(volatile word *baseaddr, word *dest, word count);
void flash_intel_copy_cfi_data(volatile word *baseaddr, word *dest, word count);
void flash_toshiba_copy_cfi_data(volatile word *baseaddr, word *dest, word count);

void flash_peek_twice (volatile word *wptr, word *prior, word *current);
void flash_spansion_get_id_codes(volatile word *baseaddr, word *dest);
void flash_samsung_get_id_codes(volatile word *baseaddr, word *dest);
void flash_amd_get_id_codes(volatile word *baseaddr, word *dest);
void flash_intel_get_id_codes(volatile word *baseaddr, word *dest);


#ifdef FEATURE_NSRW_FLASH
static flash_ptr_type spansion_write_and_commit_buffer (flash_ptr_type wptr, 
                                               const uint16* data_buffer,
                                               uint32 word_count, 
                                               flash_ptr_type baseaddr);

static nsrw_flash_status_type spansion_nsrw_suspend_erase (flash_ptr_type ptr);
static nsrw_flash_status_type spansion_nsrw_suspend_write (flash_ptr_type ptr);

nsrw_flash_status_type chk_toggle_and_reset_on_err (
  flash_ptr_type dest_ptr, boolean check_wr_abort);

static nsrw_flash_status_type spansion_resume_write_only (flash_ptr_type wptr);
#endif

/****************************************************************
 * Local and Global Data
 ***************************************************************/
LOCAL flash_copy_cfi_data_func cfi_copy_functions[FLASH_MAX_FAMILIES] =
    {
      flash_spansion_copy_cfi_data,    /* FLASH_SPANSION_FAMILY */
      flash_intel_copy_cfi_data,       /* FLASH_INTEL_FAMILY */
      flash_samsung_copy_cfi_data,     /* FLASH_SAMSUNG_FAMILY */
      flash_toshiba_copy_cfi_data,     /* FLASH_SAMSUNG_FAMILY *///Gemsea Add
    };

/****************************************************************
 * Functions
 ***************************************************************/

/***********************************************************************
FUNCTION      FLASH_PEEK_TWICE

DESCRIPTION   Read a word from a given address twice.
              The reads are performed by code executing out of ram.
              This works around an AMD defect where unrelated read cycles from
              code space mess up the bit toggle in the EFS space.

RETURN VALUE   NONE
**********************************************************************/
void 
flash_peek_twice (volatile word *wptr, word *prior, word *current)
{
  INTLOCK();
  *prior = *wptr;
  *current = *wptr;
  INTFREE();
}

/***********************************************************************
FUNCTION      FLASH_SPANSION_GET_ID_CODES

DESCRIPTION   This function puts the SPANSION flash into Autoselect mode and
              reads the MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
flash_spansion_get_id_codes(volatile word *baseaddr, word *dest)
{
   *(baseaddr) = 0x00F0 ;

  /* Write the command sequence to place the device into Autoselect mode */
   *(baseaddr + 0x555L) = 0x00AA;
   *(baseaddr + 0x2AAL) = 0x0055;
   *(baseaddr + 0x555L) = 0x0090;

   /* Check both words of the IID */
   dest[0] = *(baseaddr + 0x0000L);

   /* Write the command to reset the device to normal ROM mode */
   *(baseaddr) = 0x00F0 ;

    /* Write command to get device ID */
   *(baseaddr + 0x555L) = 0x00AA;
   *(baseaddr + 0x2AAL) = 0x0055;
   *(baseaddr + 0x555L) = 0x0090;

   dest[1] = *(baseaddr + 0x01L );
   dest[2] = *(baseaddr + 0x0EL );
   dest[3] = *(baseaddr + 0x0FL );

    /* Write the command to reset the device to normal ROM mode */
   *(baseaddr) = 0x00F0 ;
}

/***********************************************************************
FUNCTION      FLASH_TOSHIBA_GET_ID_CODES

DESCRIPTION   This function puts the SPANSION flash into Autoselect mode and
              reads the MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void flash_toshiba_get_id_codes(volatile word *baseaddr, word *dest)
{
  /* Reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;
  
  /* Write the command sequence to place the device into Autoselect mode */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x0090;
  
  /* Get three words of the IID */
  dest[0] = *(baseaddr + 0x0000L);
  dest[1] = *(baseaddr + 0x0001L);

  /* Reset device to read array mode */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x00F0;
}

/***********************************************************************
FUNCTION      FLASH_SAMSUNG_GET_ID_CODES

DESCRIPTION   This function puts the SAMSUNG flash into Autoselect mode and
              reads the MFG ID and device ID codes from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
flash_samsung_get_id_codes(volatile word *baseaddr, word *dest)
{
  /* Reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;

  /* Write command to get Manufacture ID */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x0090;

  dest[0] = *(baseaddr + 0x0000L);

  /* Reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;

  /* Write command to get device ID */
  *(baseaddr + 0x555L) = 0x00AA;
  *(baseaddr + 0x2AAL) = 0x0055;
  *(baseaddr + 0x555L) = 0x0090;
   
  dest[1] = *(baseaddr + 0x01L);

  /* Reset the device to normal ROM mode */
  *(baseaddr) = 0x00F0 ;
}

/***********************************************************************
FUNCTION      FLASH_AMD_GET_ID_CODES

DESCRIPTION   This function puts the AMD flash into Autoselect mode and
              reads the MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
flash_amd_get_id_codes(volatile word *baseaddr, word *dest)
{
  /* Write the command sequence to place the device into Autoselect mode */
   *(baseaddr + 0x555L) = 0x00AA;
   *(baseaddr + 0x2AAL) = 0x0055;
   *(baseaddr + 0x555L) = 0x0090;

   /* Get three words of the IID */
   dest[0] = *(baseaddr + 0x0000L);
   dest[1] = *(baseaddr + 0x0001L);
   dest[2] = *(baseaddr + 0x000EL);

   /* Reset device to read array mode */
   *(baseaddr) = 0x00F0 ;
}

/***********************************************************************
FUNCTION      FLASH_INTEL_GET_ID_CODES

DESCRIPTION   This function puts the INTEL flash into read mode and
              reads the Device ID and MFG ID code from the flash.

RETURN VALUE   NONE
**********************************************************************/
void
flash_intel_get_id_codes(volatile word *baseaddr, word *dest)
{
  /* Write the clear status command */
  *(baseaddr) = 0x50;

  /* Reset device to read mode */
  *(baseaddr) = 0xFF;

  /* Write the ID command */
  *(baseaddr) = 0x90;

  /* Check both words of the IID */
  dest[0] = *(baseaddr + 0x0L);
  dest[1] = *(baseaddr + 0x1L);

  /* Write the command to reset the intel device to normal read mode */
  *(baseaddr) = 0xFF;
}

/***********************************************************************
FUNCTION      FLASH_INTEL_COPY_CFI_DATA

DESCRIPTION   This function puts the INTEL flash into CFI read mode and
              copies count bytes from the CFI data into the dest

RETURN VALUE   NONE
**********************************************************************/
void
flash_intel_copy_cfi_data(volatile word *baseaddr, word *dest, word count)
{
  word tmp = 0;

  /* Set the Read CFI Query command */
  *(baseaddr) = 0x98;

  for (tmp = 0; tmp < count; tmp ++)
    *(dest++) = *(baseaddr+tmp);

  /* Set the Flash back to Read mode */
  *(baseaddr) = 0xFF;
}

/***********************************************************************
FUNCTION      FLASH_SPANSION_COPY_CFI_DATA

DESCRIPTION   This function puts the AMD flash into CFI read mode and
              copies count bytes from the CFI data into the dest

RETURN VALUE   NONE
**********************************************************************/
void
flash_spansion_copy_cfi_data(volatile word *baseaddr, word *dest, word count)
{
  word tmp = 0;
  
  /* Set the Read CFI Query command */
  *(baseaddr+0x0555L) = 0x98;        // datasheet says 0x55 but doesn't work!

  for (tmp = 0; tmp < count; tmp ++)
    *(dest++) = *(baseaddr+tmp);

  /* Set the Flash back to Read mode */
  *(baseaddr + 0x0) = 0x00F0;
}

/***********************************************************************
FUNCTION      FLASH_SAMSUNG_COPY_CFI_DATA

DESCRIPTION   This function puts the Samsung flash into CFI read mode and
              copies count bytes from the CFI data into the dest

RETURN VALUE   NONE
**********************************************************************/
void
flash_samsung_copy_cfi_data(volatile word *baseaddr, word *dest, word count)
{
  word tmp = 0;
  
  /* Set the Read CFI Query command */
  *(baseaddr+0x0555L) = 0x98;

  for (tmp = 0; tmp < count; tmp ++)
    *(dest++) = *(baseaddr+tmp);

  /* Set the Flash back to Read mode */
  *(baseaddr + 0x0) = 0x00F0;
}

/***********************************************************************
FUNCTION      FLASH_TOSHIBA_COPY_CFI_DATA

DESCRIPTION   This function puts the Samsung flash into CFI read mode and
              copies count bytes from the CFI data into the dest

RETURN VALUE   NONE
**********************************************************************/
void
flash_toshiba_copy_cfi_data(volatile word *baseaddr, word *dest, word count)
{
  word tmp = 0;
  
  /* Set the Read CFI Query command */
  *(baseaddr+0x0555L) = 0x98;

  for (tmp = 0; tmp < count; tmp ++)
    *(dest++) = *(baseaddr+tmp);

  /* Set the Flash back to Read mode */
  *(baseaddr + 0x0) = 0x00F0;
}

/***********************************************************************
FUNCTION      FLASH_READ_CFI

DESCRIPTION   This function ids the flash family and accordingly reads 
              CFI_WORD_SIZE of CFI data

RETURN VALUE   NONE
**********************************************************************/
void
flash_read_cfi(volatile word *baseaddr, flash_geometry_info *ginfo, word *cfi_data)
{
  word ext_cfi_data_offset;
  word *ext_cfi_data_ptr; 
 
  if (ginfo->family_type != FLASH_UNKNOWN_FAMILY)
  {
    /* fill out buffer for cfi_data */
  	cfi_copy_functions[ginfo->family_type](baseaddr, cfi_data, CFI_WORD_SIZE);

    /* append to cfi_data extended query table information */
    ext_cfi_data_ptr = cfi_data + CFI_WORD_SIZE;
    ext_cfi_data_offset = (((word)inp(cfi_data + EXT_CFI_OFFSET_HI) << 8) + ((word)inp(cfi_data +
                                       EXT_CFI_OFFSET_LO)));

    cfi_copy_functions[ginfo->family_type](baseaddr + ext_cfi_data_offset, ext_cfi_data_ptr, 
                                     CFI_EXT_WORD_SIZE);
  }

  return;
}


#ifdef FEATURE_NSRW_FLASH

#ifdef SPANSION_WRITE_SUSPEND_POLLING  
/* 2000 corresponds to 20uS assuming read time 10ns adjust this accordingly */
#define POLL_COUNT_FOR_20_US      2000

/* 
 * Per Spansion's suggestion: for workaround status problem while program suspend
 * Write and erase Reference pointers for determining status polling location */
static flash_ptr_type erase_ref_ptr, status_poll_location;
static uint16 status_poll_data;
extern uint32 nsrw_flash_efs_start, nsrw_flash_efs_end, nsrw_flash_sector_mask;

void spansion_get_poll_location_and_data(volatile word *wptr);

void spansion_get_poll_location_and_data(volatile word *wptr)
{
  uint32 reference_loc;

  /* Writes from EFS will not span more than a page of 512 bytes so there
   * is no worry of this getting into other sector during this course */
  reference_loc = (uint32)wptr;

  /* Get starting address of current write sector */
  reference_loc = reference_loc & (~nsrw_flash_sector_mask);

  /* Get to proper status poll location */
  while (1)
  {
    /* Jump to next sector */
    reference_loc += (nsrw_flash_sector_mask + 1);

    /* If we crossed over EFS region; start at the beggining of EFS */
    if (reference_loc >= nsrw_flash_efs_end)
    {
      reference_loc = nsrw_flash_efs_start;
    }

    /* If the erase is not happening then the reference pointer for
     * erase will be 0 and still our check would succeed since our
     * poll location should never be 0 and it never goes out of EFS
     * region. */
    if ((reference_loc != (uint32)erase_ref_ptr) && 
        (reference_loc != (uint32)wptr))
    {
      /* We got our reference point */
      break;
    }
  }

  /* Got the status poll location which is neither in writing sector
   * nor in erasing sector */
  status_poll_location = (flash_ptr_type)reference_loc;

  /* Get our reference data for later comparison */
  status_poll_data = *status_poll_location;
}

#endif

/******************************************************************************

FUNCTION       SPANSION_NSRW_BUFFERED_WRITE

DESCRIPTION    Write the data in buffer to the offset location indicated from 
               the flash base address.

   NOTE: This function will "NOT" perform byte aligned writes and buffer page
       mis aligned writes (page is aligned at 32 bytes)
   
RETURN VALUE   Status
*****************************************************************************/
flash_status spansion_nsrw_buffered_write (byte *buffer, flash_ptr_type baseaddr, 
                         dword offset, dword count)
{
  uint32 int_sav, write_count;
  nsrw_flash_status_type flash_write_status;
  volatile word *wptr;
  word* data_buffer;

  wptr = baseaddr + BYTE_TO_WORD_OFFSET(offset);

#ifdef SPANSION_WRITE_SUSPEND_POLLING  
  spansion_get_poll_location_and_data(wptr);
#endif

  SET_DEBUG(WRITE_REQUEST_BIT);

  /* treat count as number of words now on */
  count = count / 2;
  data_buffer = (word*) buffer;

  while (count > 0)
  {
    write_count = (count > (PAGE_BUFFER_SIZE/2)) ? (PAGE_BUFFER_SIZE/2) : count;

    KICK_DOG_AND_CHECK_DATA();

    INTLOCK_SAV(int_sav);
    SET_DEBUG(WRITE_PROGRESS_BIT);
    wptr = spansion_write_and_commit_buffer(wptr, data_buffer, write_count, baseaddr);

    for (;;)
    {
      if (INTERRUPTED())
      {
        CLEAR_DEBUG(WRITE_PROGRESS_BIT);
        flash_write_status = spansion_nsrw_suspend_write(wptr);

        INTFREE_SAV(int_sav);
        INTLOCK_SAV(int_sav);

        /* Here operation completed means, successfully suspended 
           (NOT for sure write Complete, but possible) */
        if (flash_write_status != OPERATION_COMPLETED)
        {
          break;  /* some error occurred.., breaks for loop */
        }

        SET_DEBUG(WRITE_PROGRESS_BIT);
        flash_write_status = spansion_resume_write_only(wptr);
        if (flash_write_status != OPERATION_IN_PROGRESS)
        {
          break; /* write completed or error occurred */
        }
      }    /* If interrupted */

      flash_write_status = chk_toggle_and_reset_on_err(wptr, TRUE);
      if (flash_write_status != OPERATION_IN_PROGRESS)
      {
        break;
      }
    }  /* for loop for one buffer write */

    CLEAR_DEBUG(WRITE_PROGRESS_BIT);
    INTFREE_SAV(int_sav);

    /* Check if any error occurred at any stage */
    if (flash_write_status == OPERATION_ERROR)
    {
      return FLASH_FAILURE;  /* Write failed */
    }

    wptr++;  /* Its already incremented till last location. */
    data_buffer += write_count;
    count -= write_count;
  } /* while we have some more bytes to write */ 
   
  CLEAR_DEBUG(WRITE_REQUEST_BIT);
  return FLASH_SUCCESS;
}

/******************************************************************************

FUNCTION         SPANSION_NSRW_ERASE_SECTOR 

DESCRIPTION      This function Erases a sector in NSRW device. If interrupt 
                 occurres then suspends resume, lets the other task to run 
                 then resumes again.

                 In AMSS this function must be called from NSRW Task only ..!!

RETURN VALUE   
  OPERATION_COMPLETED        - Erased spaecified sector Successfully
  OPERATION_ERROR            - Error occurred, flash has been reset
******************************************************************************/
nsrw_flash_status_type spansion_nsrw_erase_sector (flash_ptr_type erase_ptr)
{
  uint32 int_val;
  nsrw_flash_status_type toggle_status;

  INTLOCK_SAV(int_val);

  SET_DEBUG(ERASE_PROGRESS_BIT);

#ifdef SPANSION_WRITE_SUSPEND_POLLING  
  /* Backup the erase location into reference ptr */
  erase_ref_ptr = (flash_ptr_type) (((uint32)erase_ptr) & (~nsrw_flash_sector_mask));
#endif
        
  /* Unlock commands */
  *(erase_ptr + 0x555) = 0xAA;
  *(erase_ptr + 0x2AA) = 0x55;
  *(erase_ptr + 0x555) = 0x80;
  *(erase_ptr + 0x555) = 0xAA;
  *(erase_ptr + 0x2AA) = 0x55;

  /* Erase this sector */
  *erase_ptr = 0x30;

  while (1)  /*lint !e716 infinite loop */
  {
    if (INTERRUPTED())
    {
      CLEAR_DEBUG(ERASE_PROGRESS_BIT);
      toggle_status = spansion_nsrw_suspend_erase(erase_ptr);
      /* Now the flash is readable */

      INTFREE_SAV(int_val);
       /* ***  Yields to other tasks  *** */
      INTLOCK_SAV(int_val);

      /* Here operation completed means, successfully suspended
         (NOT Erase complete, but possible) */
      if (toggle_status != OPERATION_COMPLETED)
      {
        break; /* Some error occurred... Exit */
      }
      SET_DEBUG(ERASE_PROGRESS_BIT);
      *erase_ptr = 0x30;  /* Resume */
    }   /* If interrupted */

    KICK_DOG_AND_CHECK_DATA();

    toggle_status = chk_toggle_and_reset_on_err(erase_ptr, FALSE);

    if (toggle_status != OPERATION_IN_PROGRESS)
    {
      break;
    }

  }    /* While */

#ifdef SPANSION_WRITE_SUSPEND_POLLING  
  /* Clear it */
  erase_ref_ptr = 0;
#endif

  CLEAR_DEBUG(ERASE_PROGRESS_BIT);
  INTFREE_SAV(int_val);

  return toggle_status;
}

/******************************************************************************

FUNCTION       CHK_TOGGLE_AND_RESET_ON_ERR

DESCRIPTION    This function checks the toggle status DQ6 and performs error 
               check while toggling. If any error occurred then resets the flash 
               device to bring it back to readable state.

RETURN VALUE   
  OPERATION_IN_PROGRESS    - Still DQ6 toggling and NO error occurred.
  OPERATION_COMPLETED      - DQ6 not toggling any more, operation completed
  OPERATION_ERROR          - Error occurred, flash is reset
 
******************************************************************************/
nsrw_flash_status_type chk_toggle_and_reset_on_err (flash_ptr_type dest_ptr,
  boolean check_wr_abort)
{
  uint32 DQ6_toggle_bit, flash_progress_status;

  flash_progress_status = *dest_ptr;

  /* Check status of DQ6 Toggle bit if it is toggling */
  DQ6_toggle_bit = *dest_ptr;
  DQ6_toggle_bit = (DQ6_toggle_bit ^ (*dest_ptr)) & DQ6;

  if (DQ6_toggle_bit == 0)
  {
    return OPERATION_COMPLETED;
  }

  if (flash_progress_status & DQ5)
  {
    *dest_ptr = 0xF0;  /* flash reset */
    return OPERATION_ERROR;
  }

  if (check_wr_abort)
  {
    /*   DQ3 is set means erase is in progress, if yes, then don't consider
         DQ1 because we don't get Write Buffer Abort Reset error during erase.
         Probably for erase operation that flag is used for something else it
         is set most of the time during erase. So just ignore that */
    if ((flash_progress_status & DQ1) && ((flash_progress_status & DQ3) == 0))
    {
      flash_ptr_type baseaddr;

      /* Clear last 3 nibbles to get the address to issue unlock commands */
      baseaddr = (flash_ptr_type) (((uint32)dest_ptr) & (0xFFFFF000));

      *(baseaddr + 0x555) = 0xAA;
      *(baseaddr + 0x2AA) = 0x55;
      *(baseaddr + 0x555) = 0xF0;

      return OPERATION_ERROR;
    }
  }

  return OPERATION_IN_PROGRESS;
}
/******************************************************************************

FUNCTION      SPANSION_NSRW_SUSPEND_ERASE

DESCRIPTION   This function suspends erase if is in progress.
              This function doesn't return unless the flash device is in 
              readable state. Since we suspend the operation when interrupt 
              occurred, each attempt to suspend would consume 20uS, so if it 
              takes longer than 40uS there is nothing much we can do by 
              returning control. Since that would violate hard time deadlines.

RETURN VALUE
  OPERATION_COMPLETED        - Suspended Successfully
  OPERATION_ERROR            - Error occurred, flash has been reset
 
******************************************************************************/
static 
nsrw_flash_status_type spansion_nsrw_suspend_erase (flash_ptr_type ptr)
{
  nsrw_flash_status_type flash_susp_status;

  *ptr = 0xB0;   /* Suspend */
  clk_busy_wait(SUSPEND_WAIT_DELAY_US);

  flash_susp_status = chk_toggle_and_reset_on_err(ptr, FALSE);

  /* Check if toggling stopped. If it is not stopped, then probably we
     cannot return, since we cannot execute code. Causing more delay
     would break other things, so just do err fatal.  */
  if (flash_susp_status == OPERATION_IN_PROGRESS){
    *ptr = 0xF0;  /* Reset */
    FLASH_ERR_FATAL("Flash device not suspending operation within time",
      0, 0, 0);
  }

  return flash_susp_status;
}

/******************************************************************************

FUNCTION      SPANSION_NSRW_SUSPEND_WRITE

DESCRIPTION   This function suspends program if is in progress.
              This function doesn't return unless the flash device is in 
              readable state. Since we suspend the operation when interrupt 
              occurred, each attempt to suspend would consume 20uS, so if it 
              takes longer than 40uS there is nothing much we can do by 
              returning control. Since that would violate hard time deadlines.

RETURN VALUE
  OPERATION_COMPLETED        - Suspended Successfully
  OPERATION_ERROR            - Error occurred, flash has been reset
 
******************************************************************************/
static 
nsrw_flash_status_type spansion_nsrw_suspend_write (flash_ptr_type ptr)
{
  nsrw_flash_status_type flash_susp_status = OPERATION_COMPLETED;

#ifdef SPANSION_WRITE_SUSPEND_POLLING  

  uint16 status_count = 0;
  static uint16 max_status_count;

  *ptr = 0xB0;   /* Suspend */

  while (1)
  {
    /* Read it twice to rule out reading while transitioning */
    if ((status_poll_data == *status_poll_location) 
        && (status_poll_data == *status_poll_location))
      break;

    ++status_count;

    /* If we take over 20uS then there is a problem, we cannot blindly
     * assume response. */
    if (status_count > POLL_COUNT_FOR_20_US)
    {
      *ptr = 0xF0;  /* Reset */
      FLASH_ERR_FATAL("Flash device not suspending operation within time",
        0, 0, 0);
    }
  }

  if (max_status_count < status_count)
  {
    max_status_count = status_count;
  }

#else

  /*  CAUTION...!!   WARNING...!!
   *
   * Per spansion, assume that suspending is done by this time. 
   * But enabling this code might cause random Data aborts or
   * prefetch aborts. */

  *ptr = 0xB0;   /* Suspend */

  clk_busy_wait(SUSPEND_WAIT_DELAY_US);

#endif

  return flash_susp_status;
}

/******************************************************************************
FUNCTION       SPANSION_RESUME_WRITE_ONLY

DESCRIPTION    This function resumes only write in the event both erase and 
               write are suspended. If write had completed before suspending, 
               then resuming will resume erase instead of write. In such case 
               this function will suspend erase if it resumed.

RETURN VALUES
  OPERATION_IN_PROGRESS      - If write is still in progress after resume
  OPERATION_COMPLETED        - Write is completed.
  OPERATION_COMPLETED        - Write was completed and erase resumed 
                                        instead of write
   
*****************************************************************************/
static 
nsrw_flash_status_type spansion_resume_write_only (flash_ptr_type wptr)
{
  uint32 DQ6_toggle_bit, temp_flash_status;

  *wptr = 0x30;   /* Resume */

  /* Get status to check */
  temp_flash_status = *wptr;

  /* Check Toggle bit status, if it is toggling */
  DQ6_toggle_bit = *wptr;
  DQ6_toggle_bit = (DQ6_toggle_bit ^ (*wptr)) & DQ6;

  if (DQ6_toggle_bit == 0)
  {
    /* DQ6 is not toggling means write just completed */
    return OPERATION_COMPLETED;
  }

  /*  Write/Erase didn't complete yet, we can now check the status,
   *  if DQ3 is set, then erase resumed instead of write and write
   *  was completed before we suspended it. 
   */
  if (temp_flash_status & DQ3)
  {
    SET_DEBUG(ERROR_RESUME_BIT);
    if (spansion_nsrw_suspend_erase(wptr) == OPERATION_ERROR)
    {
      /* Error happened means that now we cannot resume erase
       *  if it is suspended now. Should we attempt to restart erase
       *  on that block here??
       */
      FLASH_ERR_FATAL("Error occurred during erase...", 0, 0, 0);
    }
    return OPERATION_COMPLETED;
  }

  /* write is still in progress */
  return OPERATION_IN_PROGRESS;
}

/******************************************************************************

FUNCTION         SPANSION_WRITE_AND_COMMIT_BUFFER

DESCRIPTION      Writes the data into the flash device's buffer and issues a 
                 commit data command to program buffer data into flash. Returns 
                 the last written location for polling.

RETURN VALUES    NONE
*****************************************************************************/
static
flash_ptr_type spansion_write_and_commit_buffer (flash_ptr_type wptr, 
                                        const uint16* data_buffer,
                                        uint32 word_count, 
                                        flash_ptr_type baseaddr)
{
  uint16* end_ptr;

  end_ptr = (word*) wptr + word_count; 

  *(baseaddr + 0x555) = 0xAA;
  *(baseaddr + 0x2AA) = 0x55;

  *wptr = 0x25;
  *wptr = word_count - 1; /*lint !e734 word count will not exceed 512 */

  while(wptr < end_ptr)
  {
    *wptr = *data_buffer;
    ++data_buffer;
    ++wptr;
  }

  --wptr;

  *wptr = 0x29;  /* commit buffer */

  return wptr;
}

/*===========================================================================

FUNCTION SPANSION_NSRW_FAST_WORD_WRITE

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
  Interupts are locked for the time 1 word is written

===========================================================================*/
flash_status
spansion_nsrw_fast_word_write (byte *buffer,
                               flash_ptr_type baseaddr,
                               dword offset,
                               dword count)
{

  volatile byte *part_base;
  volatile word *wptr, *check_ptr;
  word value;
  word tmp;
  byte timeout_cnt=0;
  uint32 int_sav;
  flash_status status = FLASH_SUCCESS;  
  volatile word *unlock_base;

  KICK_DOG_AND_CHECK_DATA();

  /* Base address of operation. */
  part_base = ((volatile byte *) baseaddr + offset);
  check_ptr = (volatile word *) ((dword) part_base & ~1);

  (void)check_ptr; // is this pointer and assignment above really needed...??!!

  INTLOCK_SAV(int_sav);

  unlock_base = (volatile word *) ((unsigned long) part_base & (~4095));
  unlock_base[0x555] = 0xAA; /* get into unlock bypass mode */
  unlock_base[0x2AA] = 0x55;
  unlock_base[0x555]= 0x20;
  
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
    while (1)    //lint !e716 while(1) has break and return..
    {
      tmp = *wptr;
 
      /* Exit when finished. */
      if ((tmp & DQ7) != (value & DQ7))
      {  
        if ((tmp & DQ5) != 0)
        {
          tmp = *wptr;
           /* DQ7 should be rechecked even if DQ5 = “1” because DQ7 
              may change simultaneously with DQ5. Refer Spansion data sheet */
          if ((tmp & DQ7) != (value & DQ7))
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

    if (INTERRUPTED())
    {
      unlock_base[0] = 0x90; /* get out of unlock bypass mode */
      unlock_base[0] = 0x00;
      INTFREE_SAV(int_sav);
      /* ***  Yields to other tasks  *** */
      INTLOCK_SAV(int_sav);
      unlock_base[0x555] = 0xAA; /* get into unlock bypass mode */
      unlock_base[0x2AA] = 0x55;
      unlock_base[0x555] = 0x20;      
    }
  }  

  unlock_base[0] = 0x90; /* get out of unlock bypass mode */
  unlock_base[0] = 0x00;

  INTFREE_SAV(int_sav);

  KICK_DOG_AND_CHECK_DATA();

  return status;
}

#endif  /* FEATURE_NSRW_FLASH */
