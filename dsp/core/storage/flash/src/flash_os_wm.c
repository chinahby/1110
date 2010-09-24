/**********************************************************************
 * flash_os_wm.c
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

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_os_wm.c#6 $ $DateTime: 2009/03/13 11:02:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-05   jm      Revised to use Babylon
2008-08-01   mh      Add check for WM version for including pmem.h
2008-07-30   mh      Reorder header file to make flash_msg.h the second last
                     preceding flash_os.h
2008-07-25   sv      Fix Klocwork warning
2008-06-17   mh      Remove GET_PHYS_ADDR
2008-05-30   mh      File created

===========================================================================*/
#include <windows.h>
#include <ceddk.h>
#include <ceddkex.h>
#include <dm.h>
#include "flash.h"

 #if (QC_WINCE_VERSION < 700)
  #include "pmem.h" 
#endif 

#include "flash_dm_d.h"
#include "flash_msg.h"
#include "flash_os.h"

/*===========================================================================
  OS related defines
===========================================================================*/

typedef struct flash_os_cs_t
{
   CRITICAL_SECTION ctlr_cs;
   boolean       initialized;
}flash_os_cs_type ;

static flash_os_cs_type flash_cs_array[FLASH_OS_MAX_CS]; 
static uint32 flash_os_cs_count = 0;

/*===========================================================================
  DM related defines
===========================================================================*/

/* DM callback function for NAND driver */  
static void flash_dm_d_callback( const dmov_result_s_type *result_ptr,
                                                           void *user_ptr);

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

/* flash_cmd_mode_tbl is defined in MSM speific source file flash_msm.c 
   and is used to tweak the performance by using interupt or polling 
   mechanism on the flash DM  channel*/

extern flash_dm_d_cmd_id_type  flash_cmd_mode_tbl[FLASH_CMD_LAST];
static flash_dm_mode_e_type current_flash_dm_mode = INTERRUPT_MODE;

extern HANDLE m_hDM;

/*===========================================================================
  OS APIs/stubs 
===========================================================================*/
/*===========================================================================
FUNCTION flash_os_get_type

DESCRIPTION
    This API is used to report the OS type

DEPENDENCIES
  None

RETURN VALUE
  OS type.

SIDE EFFECTS
  None
===========================================================================*/
flash_os_mode_e_type flash_os_get_type(void) 
{
  #ifndef BUILD_WM700
   return (flash_os_mode_e_type)FLASH_OS_WM6_WINCE_MODE;
  #else
   return (flash_os_mode_e_type)FLASH_OS_WM7_MODE;
  #endif
}

/*===========================================================================
FUNCTION flash_os_init_lock

DESCRIPTION
  Initilaizes a critical section structure for WM OS if not already intialized
  NOTE: This function exists but today it is not used in WM as the FMD APIs
        are wrapped with crtical sections.  These APIs exist should we need to 
        use it in future

INPUT
  Address of the void pointer (masking as a pointer to flash_os_cs_type). 
   

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_init_lock(void** flash_os_cs) 
{
  /*  Check if flash_init_lock has alread been called on this cs 
   *  and if not find a available unused one
   */
  
  if (*flash_os_cs == NULL)
  {
     while ( (flash_os_cs_count < FLASH_OS_MAX_CS) &&
             (flash_cs_array[flash_os_cs_count].initialized) ) 
    {
      flash_os_cs_count++;
    }

    if (flash_os_cs_count >= FLASH_OS_MAX_CS) 
    { 
      FLASH_ERR_FATAL("InitializeCriticalSection Failed: Number of request exceeds max",0,0,0); 
    }  

    if  (!flash_cs_array[flash_os_cs_count].initialized)
    {
      InitializeCriticalSection (&(flash_cs_array[flash_os_cs_count].ctlr_cs));
      flash_cs_array[flash_os_cs_count].initialized= TRUE;

      *flash_os_cs = (void*)&flash_cs_array[flash_os_cs_count];
    }/*  if (flash_cs_array[flash_os_cs_count].initialized == 0)  */
  }/*   if (flash_os_cs == NULL)   */

}

/*===========================================================================
FUNCTION flash_os_lock

DESCRIPTION
  Enter critical section if WM critical section is initialized

INPUT
  Pointer to a flash_os_cs_type

DEPENDENCIES
  flash_init_lock must have been called and ctlr_cs intialized 

RETURN VALUE
  None .

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_lock(void* flash_os_cs)
{
  if (((flash_os_cs_type*)flash_os_cs)->initialized)
  {
    EnterCriticalSection(&(((flash_os_cs_type*)flash_os_cs)->ctlr_cs));
  }
}

/*===========================================================================
FUNCTION flash_os_unlock

DESCRIPTION
  Exit critical section if critical section is initialized

INPUT
  Pointer to a flash_os_cs_type

DEPENDENCIES
  flash_init_lock must have been called and ctlr_cs intialized 

RETURN VALUE
  None .

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_unlock(void* flash_os_cs)
{
  if (((flash_os_cs_type*)flash_os_cs)->initialized)
  {
    LeaveCriticalSection(&(((flash_os_cs_type*)flash_os_cs)->ctlr_cs));
  }
}

/*===========================================================================
FUNCTION flash_os_mem_malloc

DESCRIPTION
  Allocates a block of size bytes from the heap that matches the requested
  flags/attributes. 

INPUT
  Size of the buffer to allocate
  id for WM7, this is an UNUSED parameter 
     for WM6, is is PMEM indentifier, see PMEM_*_ID in pmem_ids.h 

DEPENDENCIES
  None

RETURN VALUE
  void* - A pointer to the newly allocated block, or NULL if the block
  could not be allocated. The pointer is 1k aligned for WM7.

SIDE EFFECTS
  None
===========================================================================*/
void *flash_os_mem_malloc
(
  unsigned int size,       /* Size of allocation request */
  unsigned int id         
)
{
#ifndef BUILD_WM700
  return (void *)pmem_malloc(size, id);
#else
    ULONG PhysicalAddress;
    DWORD basePtr;

    basePtr = (DWORD)AllocPhysMem(size, PAGE_READWRITE, 0, 0, &PhysicalAddress);
    driver_virt_to_phys_offset = (DWORD) PhysicalAddress  - 
                                 (DWORD) basePtr;

    return (void *)basePtr;
#endif
   
}


/*===========================================================================
FUNCTION flash_os_mem_free

DESCRIPTION
   Deallocates a block of memory and returns it to the right heap.

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
  FreePhysMem(ptr);
}

/*===========================================================================
 DM transfer APIs
===========================================================================*/

/*===========================================================================
FUNCTION      flash_dm_transfer_os_wm

DESCRIPTION   This is a DM transfer wrapper for NAND driver.  It hides
              DM interface code from the high-level NAND operations.
              This is used by WM main image only

INPUT         The "cmd_ptr" is a pointer to a command pointer list
              that may not be double-word aligned. This function will
              align it.
             
RETURN VALUE  TRUE    is transfer success
              FASLE   is transfer failure
===========================================================================*/
int 
flash_dm_transfer_os_wm(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  flash_dm_d_xfer_s_type *xfer_ptr;
  uint32                 loops = FLASH_DM_MAX_LOOPS;
  int                    result = 1;

  xfer_ptr = &flash_dm_d_xfer;
  xfer_ptr->dmov_xfer.user_ptr = xfer_ptr;
  xfer_ptr->dmov_xfer.cmd_ptr = (void *)GET_PHYS_ADDR(cmd_ptr);
  xfer_ptr->received = FALSE;
  xfer_ptr->results.result = 0xFFFFFFFF;

  if (current_flash_dm_mode != flash_cmd_mode_tbl[cmd_id].mode)
  {
    current_flash_dm_mode = flash_cmd_mode_tbl[cmd_id].mode;
    DMChannelSetMode(m_hDM, DMOV_DEVICE_NAND, DMOV_CHAN_NAND,  
                   ((current_flash_dm_mode== INTERRUPT_MODE) ? 
                                  DMOV_MODE_IRQ : DMOV_MODE_POLL));
  }

  FLASH_BAB_START_DM (FLASH_BAB_DM_TRANSFER, cmd_id);

  /* Pass transfer structure to DM driver */
  if(!DMTransfer(m_hDM, &xfer_ptr->dmov_xfer, driver_virt_to_phys_offset))
  {
    result = 0;
    RETAILMSG(1, (L"Flash DM internal xfer failed: DMTransfer()\r\n"));
    FLASH_ERR_FATAL("DM transfer failed", 0, 0, 0);
  }
  
  if (result == 1)
  {
    if (current_flash_dm_mode == INTERRUPT_MODE)
    {
      if(WAIT_FAILED == DMWait(m_hDM, INFINITE))
      {
        result = 0;
        RETAILMSG(1, (L"Flash DM internal xfer failed: DMWait()\r\n"));
        FLASH_ERR_FATAL("DM transfer failed", 0, 0, 0);
      }
    } /* if (current_flash_dm_mode == INTERRUPT_MODE)*/
    else /* Using polling mechanism */
    {
      while (!xfer_ptr->received && loops > 0)
      {
        loops--;
        DMPollSingleChannel(m_hDM, DMOV_DEVICE_NAND, DMOV_CHAN_NAND);
      }

      if (!xfer_ptr->received)
      {
        result = 0;
        RETAILMSG(1, (L"Flash NAND DM internal callback not received\r\n"));
      }  
      
      if (result == 1)
      {
        xfer_ptr->received = FALSE;

        /* Check the callback results for valid transfer */
        if (xfer_ptr->results.result & FLASH_DM_ERR ) 
        {
          RETAILMSG(1, (L"lash NAND DM results indicate failed transaction\r\n"));
          xfer_ptr->received = FALSE;
          FLASH_ERR_FATAL("Flash NAND DM results indicate failed transaction", 
            0, 0, 0);
        }
      }/* if (result) */
    }/* Using polling mechanism */
  } /* if (result) */

  FLASH_BAB_END_DM (FLASH_BAB_DM_TRANSFER, cmd_id);

  if(result == 1)
    return TRUE;
  else
    return FALSE;

} /* END of flash_dm_transfer */

/*===========================================================================
FUNCTION flash_dm_transfer_os_rex

DESCRIPTION
  Stub function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int 
flash_dm_transfer_os_rex(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  return 0;
}

/*===========================================================================
FUNCTION flash_dm_transfer_os_none

DESCRIPTION
    Stub function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int 
flash_dm_transfer_os_none(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
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
    RETAILMSG(1, (L"Flash NAND DM callback already received\r\n"));
  }

  if(!((result_ptr->result & FLASH_DM_ERR) == 0))
  {
    FLASH_ERR_FATAL("DM transfer failed: RSLT %x, FLUSH %x %x",
     result_ptr->result, result_ptr->flush_state[0], result_ptr->flush_state[1]);
  }

  xfer_ptr->results = *result_ptr;
  xfer_ptr->received = TRUE;

} /* END of flash_dm_d_callback */
