/**********************************************************************
 * flash_os_quartz.c
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

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_os_quartz.c#8 $ $DateTime: 2009/03/13 11:02:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-05   jm      Revised to use Babylon
2008-11-26   mh      Rename all flash_os_cs_id's to KXMUTEX_FLASH_DRV_SHARED"
2008-09-10   jz      Check KxMutex_Lock() return value and do ERR_FATAL if FALSE
2008-07-31   mh      Fix compile error
2008-07-30   mh      Reorder header file to make flash_msg.h the second last
                     preceding flash_os.h
2008-07-24   sv      Featurize PMEM and fix Klocwork warning
2008-06-17   mh      Add string identifiers for each KXMutex
2008-05-30   mh      File created

===========================================================================*/

#include "KxMutex.h" 
#include "flash_msm.h"

#ifndef FLASH_COMPILE_NOR_ONLY
  #include "pmem.h"
#endif

#ifdef FLASH_USES_DM
  #include "flash_dm_d.h"
#endif

#include "flash_msg.h"
#include "flash_os.h" 

/*===========================================================================
  OS related defines
===========================================================================*/

typedef struct flash_os_cs_t
{
   KxMutex*    ctlr_cs;      /* Critical section */
   boolean     initialized;  /* Indicate if critical section is initilaized */
}flash_os_cs_type ;

static flash_os_cs_type flash_cs_array[FLASH_OS_MAX_CS]; 
static uint32 flash_os_cs_count = 0;

/* Critical section id for each KxMutex */
static char* flash_os_cs_id[]=
{
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED", 
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED"
};

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
  OS type.

SIDE EFFECTS
  None
===========================================================================*/
flash_os_mode_e_type flash_os_get_type(void) 
{
   return (flash_os_mode_e_type)FLASH_OS_QUARTZ_MODE;
}
/*===========================================================================
FUNCTION flash_os_init_lock

DESCRIPTION
  Initilaizes a critical section structure for QUARTZ. 

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
  INTLOCK();

  /*  Check if flash_init_lock has alread been called on this cs 
   *  and if not find a available unused one
   */
  
  if (*flash_os_cs == NULL)
  {
    while ( (flash_os_cs_count < FLASH_OS_MAX_CS) &&
            (flash_cs_array[flash_os_cs_count].initialized ) )            
    {
      flash_os_cs_count++;
    }


    if (flash_os_cs_count >= FLASH_OS_MAX_CS) 
    { 
      FLASH_ERR_FATAL("KxMutex_Alloc Failed: Number of request exceeds max",0,0,0); 
    }  
    
    flash_cs_array[flash_os_cs_count].ctlr_cs = 
               KxMutex_Init_Shared(flash_os_cs_id[flash_os_cs_count]);

    if (!flash_cs_array[flash_os_cs_count].ctlr_cs ) 
    { 
      FLASH_ERR_FATAL("KxMutex_Alloc Failed",0,0,0); 
    } 
    flash_cs_array[flash_os_cs_count].initialized = TRUE;
 
    *flash_os_cs = (void*)&flash_cs_array[flash_os_cs_count];
  }

  INTFREE();
}

/*===========================================================================
FUNCTION flash_os_lock

DESCRIPTION
  Enter critical section if critical section is initialized

INPUT
  Pointer to a flash_os_cs_type

DEPENDENCIES
  flash_os_init_lock must have been called and ctlr_cs intialized 

RETURN VALUE
  None .

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_lock(void*  flash_os_cs)
{
  if (((flash_os_cs_type*)flash_os_cs)->initialized)
  { 
    if (!KxMutex_Lock(((flash_os_cs_type*)flash_os_cs)->ctlr_cs))
    {
      FLASH_ERR_FATAL("KxMutex_Lock Failed",0,0,0);
    }
  }
}

/*===========================================================================
FUNCTION flash_os_unlock

DESCRIPTION
  Exit critical section if critical section is initialized

INPUT
  Pointer to a flash_os_cs_type

DEPENDENCIES
  flash_os_init_lock must have been called and ctlr_cs intialized 

RETURN VALUE
  None .

SIDE EFFECTS
  None
===========================================================================*/
void flash_os_unlock(void*  flash_os_cs)
{
  if (((flash_os_cs_type*)flash_os_cs)->initialized)
  {
    KxMutex_Unlock(((flash_os_cs_type*)flash_os_cs)->ctlr_cs);
  }
}

#ifndef FLASH_COMPILE_NOR_ONLY

/*===========================================================================
FUNCTION flash_os_mem_malloc

DESCRIPTION
  Allocates a block of size bytes from the heap that matches the requested
  flags/attributes. 

INPUT
  Size of allocation request
  id PMEM indentifier, see PMEM_*_ID in pmem_ids.h 

DEPENDENCIES
  None

RETURN VALUE
  void* - A pointer to the newly allocated block, or NULL if the block
  could not be allocated. 

SIDE EFFECTS
  None
===========================================================================*/
void *flash_os_mem_malloc (unsigned int size, unsigned int id )
{
    return (void *)pmem_malloc(size, id);
}

/*===========================================================================
FUNCTION flash_os_mem_free

DESCRIPTION
   Stub function

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

#endif /* #ifndef FLASH_COMPILE_NOR_ONLY */

#ifdef FLASH_USES_DM

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
  DM transfer related functions
===========================================================================*/


/*===========================================================================
FUNCTION      flash_dm_transfer_poll

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
  Stub function..

DEPENDENCIES
  None

RETURN VALUE
  0

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
  0

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

#endif /* #ifdef FLASH_USES_DM */
