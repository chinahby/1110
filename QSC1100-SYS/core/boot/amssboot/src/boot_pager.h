#ifndef BOOT_PAGER_H
#define BOOT_PAGER_H
/*===========================================================================

              P A G E R    T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Pager Task.
  This module is used in the portable only.

      Copyright (c)  2004 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "task.h"
#include "rex.h"
#include "queue.h"

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/boot/boot_pager.h#1 $ $DateTime: 2005/05/18 14:00:12 $ $Author: tkuo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/05   rp      Changed background loading to eliminate potential
                   priority inversion problem.
10/22/04   rp      Changed prototype for pager_queue_next_page_request
                   to return the pointer to the task to be scheduled
10/15/04   rp      Removed amss_modem_initialized
10/10/04   rp      Added declaration for amss_modem_initialized
09/01/04   rp      created

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/

#define  PAGER_START_SIG          TASK_START_SIG
  /* After creation and initialization, the Interrupt Task waits for the Main
     Control Task to set this signal before beginning its normal execution. */

#define  PAGER_STOP_SIG           TASK_STOP_SIG
  /* The Main Control Task never sets this signal to tell the Interrupt Task to
     stop execution. */

#define  PAGER_OFFLINE_SIG        TASK_OFFLINE_SIG
  /* This signal is set to tell Interrupt task to go into offline mode.
     Actually,  this doesn't do anything to Interrupt task. */

#define  PAGER_PAGE_REQUEST_SIG   0x00000001

  /* This signal is set when there is a page fault to load the page
     immediately */

#define  PAGER_RPT_TIMER_SIG      0x00000002

  /* This signal is set when the pager_rpt_timer expires.
     This timer is used to enable Interrupt task to kick the watchdog on time
     during a wait for some event. */

												  
#define  PAGER_BACKGROUND_LOAD_SIG   0x00000004

  /* This signal is set when there is a pages lest to be loaded	and those
     pages are not immediately needed */


#define PAGER_ASSERT_PAGE_REQUEST_SIG( ) \
  (void) rex_set_sigs( &pager_tcb, PAGER_PAGE_REQUEST_SIG )

#define PAGER_CLEAR_PAGE_REQUEST_SIG( ) \
  (void) rex_clr_sigs( &pager_tcb, PAGER_PAGE_REQUEST_SIG )



/* Type for queued page requests */

typedef struct 
{
  q_link_type   link;                      /* Queue link                  */
  rex_tcb_type* tcb_ptr;
  uint32        virtual_address	;

} pager_page_req_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/




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

extern void pager_task
(
  uint32 ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

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

extern rex_tcb_type* pager_queue_next_page_request
(
  rex_tcb_type* tcb_ptr,
  uint32        virtual_address
);


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

extern void pager_load_partial_pages(void );



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

LOCAL void pager_load_page( uint32 virtual_address);
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
);


#endif /* BOOT_PAGER_H */
