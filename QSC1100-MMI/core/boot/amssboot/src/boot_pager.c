/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P A G E R     T A S K

GENERAL DESCRIPTION
  The pager task is responsible for paging memory from NAND to SDRAM.The 
  paging can occurr in two ways.
  1. On demand - This happens when a page fault occurrs due to a page access
     permissiion fault.
  2. backgound paging - This is done whenever the pager task is given CPU and
     no demand paging is pending.


EXTERNALIZED FUNCTIONS

  pager_task
    This is the entry procedure for the pager task called by rex_def_task.
  
  pager_queue_next_page_request
    This function puts the request for the next page load ino the pager queue  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The pager_task function contains the pager task.  It must be 
  specified in a call to rex_def_task and started by the Main Control task.

      Copyright (c) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
                    2001, 2002, 2003, 2004
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_pager.h"
#include "boot_read_pages.h"
#include "progressive_boot.h"
#include "miprogressive.h"
#include "err.h"
#include "armmmu.h"
#include "dog.h"
#include "boot_bad_block_list.h"
#include "rex.h"
#include "msg.h"

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_pager.c#3 $ $DateTime: 2005/11/28 13:04:39 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/05   rp      Changed background loading to eliminate potential
                   priority inversion problem.
11/05/04   rp      Added a message to indicate that all pages have been
                   loaded
10/22/04   rp      Avoid using special rex functions for suspending task
                   and changing priority.
				   
10/15/04   rp      Removed amss_modem_initialized and replaced with 
                   tmc_is_init_done()
10/15/04   rp      Removed task_start() from pager_task().

10/11/04   rp      Moved the background page loading to after the modem has
                   been initialized.
09/01/04   rp      created

===========================================================================*/



/*===========================================================================
** Page request Queue
===========================================================================*/
#define PAGER_MAX_PAGE_REQ       16
LOCAL q_type pager_page_req_q;

LOCAL pager_page_req_type  pager_page_req[PAGER_MAX_PAGE_REQ];
LOCAL uint32 current_page_req_index = 0;
LOCAL uint32 next_page_req_index    = 0;

/* Allocate a status table for each 4BK of memory to save the paged status of
each page. We support 64MB of physical memory. Total number of pages = 
64MB/4KB = 16384. Using 1 bit for the status means we need 16384/8 = 2048 
bytes  ( 512 uint32s) for the table */
LOCAL  uint32 page_status[512];

static uint32 pager_task_started = 0;

#define PAGE_4K_SHFT                    12
/* Number of bits to shift to get the index of the page status. This is 
  12 bits to get 4k boundary and another 5 bits for 32 status bit in the
  array
*/
#define PAGE_STATUS_INDEX_SHFT          17
#define PAGE_STATUS_INDEX_MASK          31

#define PAGE_LOAD_SIZE                  4096
boolean pager_all_pages_loaded   = FALSE;
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION PAGER_IS_PAGE_LOADED

DESCRIPTION
  This procedure checks to see if the page specified by the virtual address
  has been loaded. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE is the page has been loaded, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean pager_is_page_loaded
(
  uint32        virtual_address
)
{
    uint32  status_table_index;
    uint32  status_table_offset;
    uint32  status_table_mask;
    boolean load_status = FALSE;

    /* Find the index into the status table */
    status_table_index  = virtual_address >> PAGE_STATUS_INDEX_SHFT;
    status_table_offset = (virtual_address >> PAGE_4K_SHFT) &
                          PAGE_STATUS_INDEX_MASK;

    status_table_mask   = 1 << status_table_offset;
    if ((page_status[status_table_index] & status_table_mask) != 0) 
    {
      load_status = TRUE;
    }
    return  load_status;

}



/*===========================================================================

FUNCTION PAGER_HANDLE_PAGE_REQUEST

DESCRIPTION
  This procedure handles the page request.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
 void pager_handle_page_request(void)
 {
 
     pager_page_req_type* new_page_req;
     rex_tcb_type*        tcb_ptr;
     uint32               virtual_address;
     
     /* Process the priority page request if any are pending */
      while ((new_page_req = (pager_page_req_type *)q_get(&pager_page_req_q)) != NULL)

      {
         /* Get the tcb pointer and virtula_address */

         tcb_ptr         = new_page_req ->tcb_ptr;
         virtual_address = new_page_req ->virtual_address ;


         /* Load the page if it is not already loaded */
         pager_load_page( virtual_address);
         
         current_page_req_index  = (current_page_req_index +1 )&(PAGER_MAX_PAGE_REQ-1);
           
         /* resume the aborted task */
         rex_resume_task_from_abort(tcb_ptr);
      } /* end of while loop */
 }

/*===========================================================================

FUNCTION PAGER_QUEUE_NEXT_PAGE_REQUEST

DESCRIPTION
  This procedure puts the request for the next page load ino the pager queue.

DEPENDENCIES
  None

RETURN VALUE
  Return pointer to the page task is task lock is in effect. Otherwise
  returns pointer to the rex best task.

SIDE EFFECTS
  None.

===========================================================================*/

rex_tcb_type*  pager_queue_next_page_request
(
  rex_tcb_type*    tcb_ptr,
  uint32           virtual_address
)
{
    pager_page_req_type*  new_page_req;
    /* Get a free page request item */
    new_page_req         =  (pager_page_req_type*) &
                            pager_page_req[next_page_req_index];
    next_page_req_index  = (next_page_req_index +1 )&(PAGER_MAX_PAGE_REQ-1);
    /* See if we have too many requests pending. If PAGER_MAX_PAGE_REQ
       number of items are pending, this indicates that the pages are not 
       getting loaded. Indicate this as fatal error
    */   
    if (next_page_req_index == current_page_req_index) 
    {
        ERR_FATAL("Too many page requests pending", 0, 0, 0);

    }
    else
    /* Queue the request  */
    {
        new_page_req ->tcb_ptr =tcb_ptr;
        new_page_req ->virtual_address =virtual_address;

        /* Initialize link field  */
        (void) q_link(new_page_req, &new_page_req->link );  
        /* Put request on page req queue */
        q_put( &pager_page_req_q, &new_page_req->link );  

        /* Suspend the aborted task */
        rex_suspend_task(tcb_ptr);
        /* signal the pager task to let know that a page request is pending*/
        (void) rex_set_sigs( &pager_tcb, PAGER_PAGE_REQUEST_SIG );
    }
    if (rex_tasks_are_locked())
    {
      return &pager_tcb;
    }
    else
    {
      return rex_get_best_task();
    }

   return NULL;
}

#ifdef SEC_BOOT
boolean pager_authenticate_page(uint32 virtual_address)
{
  int i;
  boolean found = FALSE;
  int hash_entry = 0;
  uint32 *hash1, *hash2;

  
  for(i=paged_start_index; i<prog_hdr->numsegments; i++)
  {
    if(prog_hdr->segment[i].type == MI_PBT_NON_PAGED_SEGMENT)
    {
      hash_entry++;
    }
    else if(prog_hdr->segment[i].type == MI_PBT_PAGED_SEGMENT)
    {
      if((virtual_address >= prog_hdr->segment[i].v_address) &&
         (virtual_address < (prog_hdr->segment[i].v_address + prog_hdr->segment[i].size)))
      {
        /* We are in the correct segment */
        
        /* Subtract shifted segment base address (with adjustment for initial pad) from shifted 
         * virtual address to find the correct index into the hash table.
         */
        hash_entry += ( (virtual_address >> PAGE_4K_SHFT ) - 
                        ( (prog_hdr->segment[i].v_address + PAGE_LOAD_SIZE - 1) >> PAGE_4K_SHFT) );
        found = TRUE;
        break;
      }
      else
      {
        hash_entry += (prog_hdr->segment[i].size >> PAGE_4K_SHFT);
      }
    }
  }
  
  if(!found)
  {
    return FALSE;
  }
  
  Prog_Boot_SHA_1_Hash((uint8*)(virtual_address & (~(PAGE_LOAD_SIZE-1))), PAGE_LOAD_SIZE,
                       (uint8*)prog_digest_ptr);      
  
  hash1 = (uint32*)hash_tbl_ptr;
  hash1 += (hash_entry * (PROG_BOOT_DIGEST_SIZE/sizeof(uint32)));
  hash2 = (uint32*)prog_digest_ptr;
  
  for(i=0; i<(PROG_BOOT_DIGEST_SIZE)/sizeof(uint32); i++, hash1++, hash2++)
  {
    if(*hash1 != *hash2)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}   
#endif /* SEC_BOOT */

/*===========================================================================

FUNCTION PAGER_LOAD_PAGE

DESCRIPTION
  This procedure loads a page into memory specified by virtual_address.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern int rex_sched_allow;

void pager_load_page
(
  uint32        virtual_address
)
{
    uint32  status_table_index;
    uint32  status_table_offset;
    uint32  status_table_mask;
    uint32  saved_rex_sched_allow;
#ifndef FEATURE_MMU_ROM_PROTECTION    
    uint32  access_permission;
#endif

    /*
     * To avoid ASSERT errors in rex, we need to ensure that tasks are not
     * locked.  So, we save the value of rex_sched_allow, force it to TRUE,
     * and restore it prior to exiting the function.
     */
    saved_rex_sched_allow = rex_sched_allow;
    rex_sched_allow = TRUE;
    
    /* Load the page if it is not already loaded */
    /* Find the index into the status table */
    status_table_index  = virtual_address >> PAGE_STATUS_INDEX_SHFT;
    status_table_offset = (virtual_address >> PAGE_4K_SHFT) &
                          PAGE_STATUS_INDEX_MASK;

    status_table_mask   = 1 << status_table_offset;
    if ((page_status[status_table_index] & status_table_mask) == 0) 
    {
      /* Set permissions for all access to allow page to be loaded */
      mmu_set_access_permission(virtual_address, MMU_FULL_ACCESS_PERMISSIONS);

      /* Load the page into memory */
      if(boot_read_pages(virtual_address,PAGE_LOAD_SIZE) != TRUE)
      {
        if(pager_task_started)
          {
            ERR_FATAL("Error reading pages when paging in address 0x%x\n",virtual_address,0,0);
          }
          else
          {
            /* risky to ERR_FATAL here -- jump to boot error handler */
            BOOT_ERR();
          } 
      }
#ifndef FEATURE_MMU_ROM_PROTECTION
      /* 
        If ROM protection is enabled, changing permission here will only
        result in more page faults when accessing this area of memory,
        since the C bit is not enabled in the MMU yet.  Since we've already set
        permissions to FULL_ACCESS, we don't need to change them again.
        */
      
      /* Get the access permission for the page */
      access_permission =   mmu_get_access_permission(virtual_address);
      
      /* Set the access permission for the page now that it is loaded */
      mmu_set_access_permission(virtual_address, access_permission);
#endif      
      /* Set the page load status to 1 for this page */
      page_status[status_table_index] |= status_table_mask;
#ifdef SEC_BOOT
        if(!pager_authenticate_page(virtual_address))
        {
          if(pager_task_started)
          {
            ERR_FATAL("Bad hash value for address 0x%x\n",virtual_address,0,0);
          }
          else
          {
            /* risky to ERR_FATAL here -- jump to boot error handler */
            BOOT_ERR();
          }
        }
#endif
    }
    
    rex_sched_allow = saved_rex_sched_allow;
}



/*===========================================================================

FUNCTION PAGER_LOAD_PARTIAL_PAGES

DESCRIPTION
  This procedure loads a partial page at the beginning and end of the
  paged segment if the paged segment doesn't start or end in a 4K boundary.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void pager_load_partial_pages(void )
{
    uint32 partial_load_start_addr;
    uint32 partial_load_end_addr; 
    partial_load_start_addr = apps_partition_base;
    partial_load_end_addr   = apps_partition_base + 
                              apps_partition_size;
    /* Check to see if we need to copy partial pages at the beginning or end of
    the paged segment */
    if (
          ((partial_load_start_addr & (MMU_PAGE_SIZE-1)) != 0) ||
          (( partial_load_end_addr  & (MMU_PAGE_SIZE-1)) != 0)
       )
    {
        boot_read_partial_pages();
    }

}


/*===========================================================================

FUNCTION PAGER_TASK

DESCRIPTION
  The pager task is responsible for paging memory from NAND to SDRAM.The 
  paging can occurr in two ways.
  1. On demand - This happens when a page fault occurrs due to a page access
     permissiion fault.
  2. backgound paging - In this case a background loader task accesses the
     first memory address of each paged memory which will result in a page
	 fault if that page is not yet loaded. The page fault handler will then
	 load that page as in demand paging.
	 
DEPENDENCIES
  This procedure should be called by rex_def_task.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void pager_task
(
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type        pager_sigs;       /* signals returned from REX             */
  /* Initialize the page request queue.
  */
  (void) q_init( &pager_page_req_q );

  pager_task_started = 1;
  /* Loop forever
  */
  for (;;)
  {


      pager_sigs = rex_wait
                       ( 
                           PAGER_PAGE_REQUEST_SIG
                       );
      if ( (pager_sigs & PAGER_PAGE_REQUEST_SIG) != 0 )
      {
        /* Clear the pending signal, and continue
        */
        (void) rex_clr_sigs( &pager_tcb, PAGER_PAGE_REQUEST_SIG );

        pager_handle_page_request();
      }
      
      if (pager_all_pages_loaded==TRUE) 
      {

        rex_suspend_task(&pager_tcb);
      }
  } /* end of infinite loop */

} /* end of pager_task */


/*===========================================================================

FUNCTION PAGER_IS_TASK_STARTED

DESCRIPTION
  Returns 1 if pager task has been started and 0 if not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

uint32 pager_is_task_started(void)
{
    return  pager_task_started;
}

/*===========================================================================

FUNCTION BACKGROUND_LOADER_TASK

DESCRIPTION
  The background loader task is responsible for backgound paging.
  This is done whenever the background loader task task is given CPU and
  no demand paging is pending.

DEPENDENCIES
  This procedure should be called by rex_def_task.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void background_loader_task
(
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  uint32               background_load_start_addr;
  uint32               background_load_end_addr; 
  uint32               segment_base, segment_size;
  int i;
  volatile uint32      peek_page;         

  /* Loop forever
  */
  for (;;)
  {
    /* Tell Dog to stop monitoring the sleep task since sleep
     * task will not get to run till the back ground loading is over.
     */
    dog_monitor_pause( DOG_SLEEP_RPT );

    /* peek at each page to cause a data abort which will make
		 * that page to be loaded
		 */   
    for(i=paged_start_index; i<prog_hdr->numsegments; i++)          
    {
      if(prog_hdr->segment[i].type == MI_PBT_PAGED_SEGMENT)
      {
        segment_base = prog_hdr->segment[i].v_address;
        segment_size = prog_hdr->segment[i].size;
        
        /* Find the first full page from which the page loading should start. 
         * This will be the next 4k boundary. Note that any data before the 4k boundary
         * will be copied by the partial copy.
         */
        background_load_start_addr = 
          (segment_base + MMU_PAGE_SIZE-1) & (~(MMU_PAGE_SIZE-1));              
        /* Find the last full page till which the page loading will be done. This will be
         * the previous 4K boundary. Note that any data after the 4k boundary
         * will be copied by the partial copy.  
         */
        background_load_end_addr   = 
          (segment_base + segment_size + MMU_PAGE_SIZE-1) & (~(MMU_PAGE_SIZE-1));

        while (background_load_start_addr < background_load_end_addr) 
        {
          peek_page = *(uint32*)background_load_start_addr;
          peek_page++;
          background_load_start_addr += PAGE_LOAD_SIZE;
        }
      }
    } // end loop through segments
      
    dog_monitor_resume( DOG_SLEEP_RPT );
    pager_all_pages_loaded = TRUE;
    MSG_HIGH("All application pages loaded",0,0,0);
#ifdef FEATURE_MMU_ROM_PROTECTION
    /* We can turn this on now that everything has been paged in */
    mmu_enable_rom_protection();
#endif       
    rex_suspend_task(&background_loader_tcb);
  } /* end of infinite loop */
  
} /* end of background_loader_task */



