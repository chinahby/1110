/**********************************************************************
 * flash_os_rex.c
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

  $Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_os_rex.c#7 $ $DateTime: 2009/03/13 11:02:32 $ $Author: jmeinke $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
2009-03-05   jm      Revised to use Babylon
2008-11-11   dp      Updated to support DM cache maintenance optimization
2008-08-30   mh      Fix failure in targets which use KXMUTEX when in REX_MODE
2008-07-30   mh      Reorder header file to make flash_msg.h the second last
                     preceding flash_os.h
2008-07-24   sv      Featurize PMEM usage and fix klocwork warning
2008-06-30   mh      Set REX signal after setting result, received to TRUE
2008-06-17   mh      Remove GET_PHYS_ADDR
2008-05-30   mh      File created

===========================================================================*/
#include "rex.h"
#include "task.h"
#include "flash_msm.h"

#ifndef FLASH_COMPILE_NOR_ONLY
  #include "pmem.h"
#endif

#include "flash.h"

#ifdef FLASH_USES_DM
  #include "flash_dm_d.h"
#endif

#include "flash_msg.h"
#include "flash_os.h"

#ifdef FEATURE_USE_KXMUTEX_SHARED
 
  #include "KxMutex.h"

  #define CSTYPE            KxMutex* 
  #define LOCK_CS(x)        (void)KxMutex_Lock(x)
  #define UNLOCK_CS(x)      (void)KxMutex_Unlock(x)

  /* Critical section id for each KxMutex 
   * NOTE:  For now L4 has the kxmutex id hard coded to KXMUTEX_FLASH_DRV_SHARED
            restricting to only one mutex at a time. So we have the same name 
            for all the entries.  This table will be changed later when support
            for multiple ID is mainlined
   */
  static char* flash_os_kxmutex_cs_id[]=
 {
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED", 
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED",
    "KXMUTEX_FLASH_DRV_SHARED"
  };

 #else

  #define CSTYPE            rex_crit_sect_type
  #define LOCK_CS(x)        rex_enter_crit_sect(&(x))
  #define UNLOCK_CS(x)      rex_leave_crit_sect(&(x))

#endif

/* FLASH_DM_POLL_PAUSE_US can be defined in flash_nand_msm.h */
#ifndef FLASH_DM_POLL_PAUSE_US
#define FLASH_DM_POLL_PAUSE_US 2   /* 2 microsends */
#endif

/*===========================================================================
  OS related defines 
===========================================================================*/

typedef struct flash_os_cs_t
{
   CSTYPE            ctlr_cs;       /* Critical section */
   boolean           initialized;   /* Indicate if critical section is initilaized */
}flash_os_cs_type ;

static flash_os_cs_type flash_cs_array[FLASH_OS_MAX_CS]; 
static uint32 flash_os_cs_count = 0;

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
   return (flash_os_mode_e_type)FLASH_OS_REX_MODE;
}

/*===========================================================================
FUNCTION flash_os_init_lock

DESCRIPTION
  Initilaizes a critical section structure for REX. 

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
      FLASH_ERR_FATAL("init_crit_sect Failed: Number of request exceeds max",0,0,0); 
    }

 #ifdef FEATURE_USE_KXMUTEX_SHARED

    flash_cs_array[flash_os_cs_count].ctlr_cs = 
       KxMutex_Init_Shared(flash_os_kxmutex_cs_id[flash_os_cs_count]);

    if (!flash_cs_array[flash_os_cs_count].ctlr_cs ) 
    { 
      FLASH_ERR_FATAL("KxMutex_Alloc Failed",0,0,0); 
    }

    flash_cs_array[flash_os_cs_count].initialized = TRUE;
    *flash_os_cs = (void*)&flash_cs_array[flash_os_cs_count];      
 
 #else
    
    if (rex_is_initialized ())
    {
      INTLOCK();
    
      if  (!flash_cs_array[flash_os_cs_count].initialized)
      {
        rex_init_crit_sect (&(flash_cs_array[flash_os_cs_count].ctlr_cs));
        flash_cs_array[flash_os_cs_count].initialized = TRUE;
        *flash_os_cs = (void*)&flash_cs_array[flash_os_cs_count];
      }

      INTFREE();
    }/* if (rex_is_initialized ())  */

  #endif /*  #ifdef FEATURE_USE_KXMUTEX_SHARED */

  }/*  if (flash_os_cs == NULL) */
}

/*===========================================================================
FUNCTION flash_os_lock

DESCRIPTION
  Enter critical section if rex critical section is initialized

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
     LOCK_CS(((flash_os_cs_type*)flash_os_cs)->ctlr_cs);
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
void flash_os_unlock(void* flash_os_cs)
{
  if (((flash_os_cs_type*)flash_os_cs)->initialized)
  {
     UNLOCK_CS(((flash_os_cs_type*)flash_os_cs)->ctlr_cs);
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
   pmem_free(ptr);
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

/* flash_cmd_mode_tbl is defined in MSM speific source file flash_msm.c 
   and is used to tweak the performance by using interupt or polling 
   mechanism on the flash DM  channel*/

extern flash_dm_d_cmd_id_type  flash_cmd_mode_tbl[FLASH_CMD_LAST];
static flash_dm_mode_e_type current_flash_dm_mode = INTERRUPT_MODE;

/*===========================================================================
  DM transfer related APIs
===========================================================================*/

/*===========================================================================
FUNCTION      flash_dm_transfer_rex

DESCRIPTION   This is a DM transfer wrapper for NAND driver.  It hides
              DM interface code from the high-level NAND operations.
              It is used by the AMSS main image when Flash DM channel is
              set to operate in interrupt mode

INPUT         The "cmd_ptr" is a pointer to a command pointer list
              that may not be double-word aligned. This function will
              align it.

RETURN VALUE  TRUE    is transfer success
              FASLE   is transfer failure
===========================================================================*/
static int
flash_dm_transfer_rex(uint32 * cmd_ptr)
{ 
  flash_dm_d_xfer_s_type *xfer_ptr;
  rex_sigs_type          sigs;
  boolean                flash_dm_sig_set = FALSE;
  boolean                wait_again = FALSE;

  xfer_ptr = &flash_dm_d_xfer;
  xfer_ptr->dmov_xfer.user_ptr = xfer_ptr;
  xfer_ptr->dmov_xfer.cmd_ptr = (void *)GET_PHYS_ADDR(cmd_ptr);
  xfer_ptr->received = FALSE;
  xfer_ptr->results.result = 0xFFFFFFFF;

  /* Before queueing the command in the data mover, check to see which
   * task is calling this function */
  xfer_ptr->caller_task = (void *)rex_self();

  xfer_ptr->result_valid = FALSE;

  /* Record whether or not FLASH_DM_SIG was already set coming into
   * this function. */
  if ((rex_get_sigs((rex_tcb_type *)(xfer_ptr->caller_task)) 
                                            & FLASH_DM_SIG) != 0)
  {
    /* Sig is set, so record it. */
    flash_dm_sig_set = TRUE;
  
    /* Clear the signal before waiting on it */
    (void) rex_clr_sigs((rex_tcb_type *)(xfer_ptr->caller_task), FLASH_DM_SIG);
  }

  /* Pass transfer structure to DM driver */
  if(!dmov_transfer(&xfer_ptr->dmov_xfer))
  {
    TPRINTF(3, ("Flash NAND DM internal transfer failed: dmov_transfer()", 
        0, 0, 0));
    FLASH_ERR_FATAL("Flash NAND DM internal transfer failed: dmov_transfer()", 
      0, 0, 0);
  }

  do
  {
    rex_wait(FLASH_DM_SIG);

    INTLOCK();
    /* Get the signals for this task */
    sigs = rex_get_sigs((rex_tcb_type *)(xfer_ptr->caller_task));

    if ((sigs & FLASH_DM_SIG) == 0)
    {
      FLASH_ERR_FATAL("Flash DM internal callback not received", 0, 0, 0);
    }
    else
    {
      /* Check to see if the result is valid */
      if (xfer_ptr->result_valid == FALSE)
      {
        /* The result is not valid, so record that we received this signal
         * but it didn't come from the data mover. */
        flash_dm_sig_set = TRUE;
        wait_again = TRUE;

        /* Clear the signal before waiting on it */
        (void)rex_clr_sigs((rex_tcb_type *)(xfer_ptr->caller_task), 
                                                     FLASH_DM_SIG );
      }
      else
      {
        /* We got the data mover interrupt, so we don't have to 
         * loop anymore. */
        wait_again = FALSE;
      }
    }
    INTFREE();
  } while (wait_again == TRUE);

  /* Clear the signal if it was not set by an outside source.  If it was
   * set by an outside source, it will be left set. */
  if (flash_dm_sig_set == FALSE )
  {
    /* The signal from the data mover was the only one that was received,
     * so clear the signal. */
    (void)rex_clr_sigs((rex_tcb_type *)(xfer_ptr->caller_task), FLASH_DM_SIG );
  }

  /* Check the callback results for valid transfer */
  if (xfer_ptr->results.result & FLASH_DM_ERR ) 
  {
    TPRINTF(3, ("Flash NAND DM results indicate failed transaction", 0, 0, 0));
    xfer_ptr->received = FALSE;
    FLASH_ERR_FATAL("Flash NAND DM results indicate failed transaction", 
      0, 0, 0);
  }

  return TRUE;
} /* END of flash_dm_transfer_rex */


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
int flash_dm_poll_bw_us = FLASH_DM_POLL_PAUSE_US;
static int 
flash_dm_transfer_poll(uint32 * cmd_ptr)
{ 
  flash_dm_d_xfer_s_type *xfer_ptr;
  uint32                 loops = FLASH_DM_MAX_LOOPS;

  xfer_ptr = &flash_dm_d_xfer;
  xfer_ptr->dmov_xfer.user_ptr = xfer_ptr;
  xfer_ptr->dmov_xfer.cmd_ptr = (void *)GET_PHYS_ADDR(cmd_ptr);
  xfer_ptr->received = FALSE;
  xfer_ptr->results.result = 0xFFFFFFFF;


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
    #if !defined(QUARTZ_BSP) && !defined(BUILD_JNAND) && !defined(BUILD_HOSTDL)
    FLASH_CLK_BUSY_WAIT(flash_dm_poll_bw_us);
    #endif
  }

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
FUNCTION      flash_dm_transfer_os_rex

DESCRIPTION   This is a DM transfer wrapper for NAND driver.  It hides
              DM interface code from the high-level NAND operations.

INPUT         The "cmd_ptr" is a pointer to a command pointer list
              that may not be double-word aligned. This function will
              align it.
              cmd_id is the type of operation (read/write/copy....)

RETURN VALUE  TRUE    is transfer success
              FASLE   is transfer failure
===========================================================================*/
int 
flash_dm_transfer_os_rex(uint32 * cmd_ptr, flash_cmd_e_type cmd_id)
{ 
  int result;

  if (current_flash_dm_mode != flash_cmd_mode_tbl[cmd_id].mode)
  {
    current_flash_dm_mode = flash_cmd_mode_tbl[cmd_id].mode;
    dmov_chan_set_mode(DMOV_DEVICE_NAND, DMOV_CHAN_NAND,    
                     ((current_flash_dm_mode == INTERRUPT_MODE) ? 
                                  DMOV_MODE_IRQ : DMOV_MODE_POLL));
  }

  FLASH_BAB_START_DM (FLASH_BAB_DM_TRANSFER, cmd_id);
  if (current_flash_dm_mode == INTERRUPT_MODE)
  {
    result = flash_dm_transfer_rex(cmd_ptr);
  }
  else 
  {
    result = flash_dm_transfer_poll(cmd_ptr);
  }    
  FLASH_BAB_END_DM (FLASH_BAB_DM_TRANSFER, cmd_id);
    
  return result;
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
FUNCTION flash_dm_transfer_os_none

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
    TPRINTF(3, ("Flash NAND DM callback already received", 0, 0, 0));
  }

  if(!((result_ptr->result & FLASH_DM_ERR) == 0))
  {
    FLASH_ERR_FATAL("DM transfer failed: RSLT %x, FLUSH %x %x",
     result_ptr->result, result_ptr->flush_state[0], result_ptr->flush_state[1]);
  }

  xfer_ptr->result_valid = TRUE;
  
  xfer_ptr->results = *result_ptr;
  xfer_ptr->received = TRUE;

  /* Set the signals for this task */
  rex_set_sigs((rex_tcb_type *)(xfer_ptr->caller_task), FLASH_DM_SIG);

} /* END of flash_dm_d_callback */

#endif /* #ifdef FLASH_USES_DM */
