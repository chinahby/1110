/**********************************************************************
 * flash_os_none.c
 *
 * OS interface.
 *
 * This source file includes OS interface and DM transfer APIs
 * Copyright (c) 2008-2009 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_os_none.c#7 $ $DateTime: 2009/03/13 11:02:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-05   jm      Revised to use Babylon
2008-08-12   sv      Featurize to exclude DM codes for NOR Flash tools
2008-08-01   mh      Add FLASH_OS_WM7_BOOT_MODE and FLASH_OS_WM6_WINCE_BOOT_MODE
2008-07-30   mh      Reorder header file to make flash_msg.h the second last
                     preceding flash_os.h
2008-06-17   mh      Remove GET_PHYS_ADDR
2008-05-30   mh      File created

===========================================================================*/
#include "flash.h"

#if defined(FLASH_USES_DM) && !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) 
  #include "flash_dm_d.h"
#endif

#include "flash_msg.h"
#include "flash_os.h"

/*===========================================================================
   OS API/stubs
===========================================================================*/

/*===========================================================================
FUNCTION flash_os_get_type

DESCRIPTION
    This API is used to report the OS type

DEPENDENCIES
  None

RETURN VALUE
  OS type: FLASH_OS_WM7_BOOT_MODE, FLASH_OS_WM6_WINCE_BOOT_MODE  or
           FLASH_OS_NONE_MODE

SIDE EFFECTS
  None
===========================================================================*/
flash_os_mode_e_type flash_os_get_type(void) 
{
  #ifdef FEATURE_WINCE_BOOTLOADER
#error code not present
  #else
   return (flash_os_mode_e_type) FLASH_OS_NONE_MODE;
  #endif

}


/*===========================================================================
FUNCTION flash_os_init_lock stub

DESCRIPTION
     Stub function

DEPENDENCIES
  None

RETURN VALUE
  NONE.

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_init_lock(void** flash_os_cs) 
{
}

/*===========================================================================
FUNCTION flash_os_lock stub

DESCRIPTION
     Stub function

DEPENDENCIES
  None

RETURN VALUE
  NONE.

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_lock(void* flash_os_cs)
{
}

/*===========================================================================
FUNCTION flash_os_unlock stub

DESCRIPTION
  Stub function

DEPENDENCIES
  None

RETURN VALUE
  NONE.

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_unlock(void* flash_os_cs)
{
}

/*===========================================================================
FUNCTION flash_os_mem_malloc

DESCRIPTION
  Stub function

DEPENDENCIES
  None

RETURN VALUE
  0.

SIDE EFFECTS
  None
===========================================================================*/
void *flash_os_mem_malloc (unsigned int size, unsigned int id )
{
  return NULL;
}

/*===========================================================================
FUNCTION flash_os_mem_free

DESCRIPTION
  Does nothing.  Not used

INPUT 
  Pointer to the memory block that needs to be deallocated 

DEPENDENCIES
  None

RETURN VALUE
  NONE.

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_mem_free(void *ptr)
{
}


/*===========================================================================
FUNCTION  REX_INT_LOCK

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  0

SIDE EFFECTS
  None
===========================================================================*/
dword rex_int_lock(void)
{
  return 0;
}

/*===========================================================================
FUNCTION  REX_INT_FREE

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  0

SIDE EFFECTS
  None
===========================================================================*/
dword rex_int_free(void)
{
  return 0;
}


#if defined(FLASH_USES_DM) && !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) 

/* DM callback function for NAND driver */  
static void flash_dm_d_callback( const dmov_result_s_type *result_ptr,
  void *user_ptr);
/*===========================================================================
  DM defines
===========================================================================*/
flash_dm_d_xfer_s_type flash_dm_d_xfer = 
{
  {
    DMOV_DEVICE_NAND,      /* DM unit number (0 to DMOV_NUM_DEVICES-1) */
    DMOV_CHAN_NAND,        /* DM channel number (0 to DMOV_NUM_CHANS-1) */
    DMOV_HIGHEST_PRIORITY, /* Priority queue number, 0 = highest */
    NULL,                  /* Pointer to Data Mover command(s) */
    DMOV_CMD_PTR_LIST,     /* What type of transfer command */
    flash_dm_d_callback,   /* User callback function or NULL */
    NULL,                  /* User callback data */
    DMOV_XFER_DEFAULT,     /* Transfer options flags, DMOV_XFER_xxx */
  }
}; 

/*===========================================================================
   DM Related functions/stubs
===========================================================================*/

/*===========================================================================
FUNCTION      flash_dm_transfer_none

DESCRIPTION   This is a DM transfer wrapper for NAND driver.  It hides
              DM interface code from the high-level NAND operations.
              It is used by the AMSS main image and tools when Flash 
              DM channel is set to operate in polling mode

INPUT         The "cmd_ptr" is a pointer to a command pointer list
              that may not be double-word aligned. This function will
              align it.

RETURN VALUE  TRUE    is transfer success
              FASLE   is transfer failure
===========================================================================*/
int 
flash_dm_transfer_os_none(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  flash_dm_d_xfer_s_type *xfer_ptr;
  uint32                 loops = FLASH_DM_MAX_LOOPS;

  xfer_ptr = &flash_dm_d_xfer;
  xfer_ptr->dmov_xfer.user_ptr = xfer_ptr;
  xfer_ptr->dmov_xfer.cmd_ptr = (void *)GET_PHYS_ADDR(cmd_ptr);
  xfer_ptr->received = FALSE;
  xfer_ptr->results.result = 0xFFFFFFFF;


  FLASH_BAB_START_DM (FLASH_BAB_DM_TRANSFER, cmd_id); 
  /* Pass transfer structure to DM driver */
  if(!dmov_transfer(&xfer_ptr->dmov_xfer))
  {
    TPRINTF(3, ("Flash NAND DM internal transfer failed: dmov_transfer()", 
        0, 0, 0));
    FLASH_ERR_FATAL("Flash NAND DM internal transfer failed: dmov_transfer()", 
      0, 0, 0);
  }

  /* Loop until callback is received */
  while (!xfer_ptr->received && loops > 0)
  {
    loops--;
    dmov_poll_single_chan( DMOV_DEVICE_NAND, DMOV_CHAN_NAND );
  }

  FLASH_BAB_END_DM (FLASH_BAB_DM_TRANSFER, cmd_id);
  if (!xfer_ptr->received)
  {
    TPRINTF(3, ("Flash NAND DM internal callback not received", 0, 0, 0));
    return FALSE;   
  }  
  xfer_ptr->received = FALSE;

  /* Check the callback results for valid transfer */
  if (xfer_ptr->results.result & FLASH_DM_ERR ) 
  {
    TPRINTF(3, ("Flash NAND DM results indicate failed transaction", 0, 0, 0));
    xfer_ptr->received = FALSE;
    FLASH_ERR_FATAL("Flash NAND DM results indicate failed transaction", 
      0, 0, 0);
  }
  return TRUE;
} /* END of flash_dm_transfer */

/*===========================================================================
FUNCTION flash_dm_transfer_os_wm

DESCRIPTION
    Stub function.

DEPENDENCIES
  0

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int 
flash_dm_transfer_os_wm(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  return 0;
}

/*===========================================================================
FUNCTION flash_dm_transfer_os_rex

DESCRIPTION
    Stub function.

DEPENDENCIES
  None

RETURN VALUE
  0

SIDE EFFECTS
  None
===========================================================================*/
int 
flash_dm_transfer_os_rex(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  return 0;
}

/*===========================================================================
FUNCTION      flash_dm_d_callback

DESCRIPTION   Prototype for user callback function registered with 
              dmov_transfer().

RETURN VALUE  NONE
===========================================================================*/
static void 
flash_dm_d_callback (const dmov_result_s_type *result_ptr,
  void *user_ptr)
{
  flash_dm_d_xfer_s_type *xfer_ptr = user_ptr;

  FLASH_BAB_EVENT_DM(FLASH_BAB_DM_CALLBACK);
  if (xfer_ptr->received)
  {
    TPRINTF(3, ("Flash NAND DM callback already received", 0, 0, 0));
  }

  if(!((result_ptr->result & FLASH_DM_ERR) == 0))
  {
    FLASH_ERR_FATAL("DM transfer failed: RSLT %x, FLUSH %x %x",
     result_ptr->result, result_ptr->flush_state[0], result_ptr->flush_state[1]);
  }

  xfer_ptr->results = *result_ptr;
  xfer_ptr->received = TRUE;
} /* END of flash_dm_d_callback */

#endif /* #if defined(FLASH_USES_DM) && !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) */
