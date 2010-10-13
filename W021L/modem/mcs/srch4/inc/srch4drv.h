#ifndef SRCH4DRV_H
#define SRCH4DRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R    H E A D E R

GENERAL DESCRIPTION
   This file contains the hardware drivers for searcher 4.

EXTERNALIZED FUNCTIONS
   srch4_get_tasks              Returns a ptr requested queue's tasks
   srch4_queue_available        Checks if a queue is available
   srch4_tasks_left             Checks if a queue is active and returns
                                 number of tasks left to be programmed
                                 
   srch4_init                   Set up necessary parameters for srch4

   srch4_flush_s_ram            Sends a command to flush the sample RAM

   srch4_add_task               Adds task to a specified queue (HPQ/LPQ)
   srch4_start_search           Start a specified (HPQ/LPQ)
   srch4_abort_queue            Aborts a specified queue (HPQ/LPQ)
   srch4_halt                   Stops all searches and disables all callbacks

      Copyright (c) 2002 - 2006
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/inc/srch4drv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/06   pa      Added prototype for srch4_1x_get_assigned_queue()
01/16/06   awj     Lint cleanup
07/19/05   grl     Added flushing the sample ram of the requested chain.
05/20/05   ejv     Remove srch_pn.h.
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                    srch4_srch_type to uint32 in logging functions
02/04/05   bt      Add scheduled sample ram flush.
11/17/04   sst     Update function header for srch4_halt
09/08/04   jcm     Rename exported genlog types to include srch_
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/09/04   sst     Removed extern of srch4_dump_isr()
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/14/04   sst     dev-Added partial results param to srch4_abort_queue()
03/04/04   sst     dev-Added function srch4_halt() to "virtually stop" srch4
02/19/04   sst     Lint corrections
                   Changed srch4_tasks_left() to return int32
02/02/04   sst     Added synchronous field to srch4_abort_queue() to stall
                    until the abort has completed
01/28/04   ejv     Mainline T_MSM6500.
12/15/03   sst     Added a technology parameter to srch4_tasks_left()
11/05/03   bt      Check whether different technologies try to abort 
                    each other and remove srch4_abort.
07/30/03   ejv     Removed sample server references.
07/08/03   sst     Moved SRCH4_MAX_NUM_TASKS, COH_TRUNC_*, and
                    SRCH2_SRCH4_OFFSET from srch4drvi.h to preserve scope
                    of srch4drvi.h
03/24/03   sst     Modified comment to reflect task peak pos stored in Cx16
01/13/03   sst     More return codes, added return values to some functions
12/05/02   sst     Code Clean up
11/14/02   sst     Added return type enum, Removed unused timers
11/11/02   sst     Removed srch4_remove_last_task, deemed unneeded
11/11/02   sst     Added srch4_remove_last_task
11/07/02   sst     Added support for Jaguar, moved MAX_SECTOR_GAIN_VAL here
                   Code cleanup
11/05/02   sst     Merge changes
11/04/02   sst     Moved srch4 win center functionality to srch4trans
10/30/02   bt      Moved SRCH4_NUM_PEAKS here
10/30/02   sst     Added support for srch4's own window center var
10/29/02   sst     Removed temporary functions
                   Separated file, created srch4trans.c .h
                   Made function calls more generic to queues
10/18/02   sst     Temporary functions to "ease" transition to tasks
10/18/02   sst     Changed HPQ to use generic task structure
                   Added support for Transmit Diversity in 1x searching
                   Added translation functions (skeletons) for 1x, HDR, AFLT
10/08/02   sst     Added support for callback functions upon dump/lost dump
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/01/02   sst     Added $Header line
09/26/02   sst     Removal of debug functions.
                   MSM6100 implementation, testing, and code review changes
08/02/02   gs      initial merge into perforce
07/01/02   sst     Added support for LPQ
06/07/02   bt      initial implementation

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "mdspext.h"
#include "rex.h"
#include "srch_genlog.h"
#include "srch_hw_t.h"

#include "srch4drv_t.h"
#include "srch4drvi.h"


/*===========================================================================
              FUNCTION DECLARATIONS FOR MODULE
  ==========================================================================*/

/*===========================================================================

              S I M P L I F I C A T I O N   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_GET_TASKS
DESCRIPTION    This function returns the requested task structure.
DEPENDENCIES   None

RETURN VALUE   Ptr to requested task structure
SIDE EFFECTS   None

===========================================================================*/
extern srch4_task_struct_type *srch4_get_tasks
(
  srch4_queue_type queue_type            /* queue to start                 */
);


/*===========================================================================

                   U T I L I T Y   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_QUEUE_AVAILABLE
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is available/empty
DEPENDENCIES   None

RETURN VALUE   TRUE   if the queue is available
               FALSE  if the queue is active or being programmed
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_queue_available
(
  srch4_queue_type        queue_type      /* queue to check            */
);

/*===========================================================================

FUNCTION       SRCH4_TASKS_LEFT
DESCRIPTION    This function checks if the specified searcher queue is active
                and returns how many searches can still be programmed
                It also checks to ensure that the requesting technology
                is using this queue
DEPENDENCIES   None

RETURN VALUE   Negative if searcher is active/tech mismatch/invalid/etc
               Zero if searcher is full
               Positive value is the number of tasks available to be
                programmed
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_tasks_left
(
  srch4_srch_type         task_srch_type, /* search type               */
  srch4_queue_type        queue_type      /* queue to check            */
);

/*===========================================================================

FUNCTION       SRCH4_BUILD_SRCH4_SUB
DESCRIPTION    Builds and commits a 1X searcher4 generalized subpacket
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_build_srch4_sub
(
  srch_genlog_packet_id_type  id,         /* id of packet to commit sub    */   
  sub_commit_func_type        commit_fp,  /* f(x) to commit the subpacket  */
  uint32                      queue_type, /* queue to log                  */
  int32                       num_results /* number of srches returned     */
);


/*===========================================================================

                   D R I V E R   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_INIT
DESCRIPTION    This function initializes anything and everything srch4drv
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   Timers, Completed Tasks variables, etc. are initialized

===========================================================================*/
extern void srch4_init( void );

/*===========================================================================

FUNCTION       SRCH4_FLUSH_S_RAM
DESCRIPTION    This function flushes the sample ram in the DSP if no searches
                are active
DEPENDENCIES   None

RETURN VALUE   Srch4dev error value
SIDE EFFECTS   Sample ram in DSP is flushed

===========================================================================*/
extern int32 srch4_flush_s_ram
(
  sample_server_rx_chain_type samp_serv_chain, /* The sample server chain */
  uint16                      position,        /* position in chips */
  boolean                     scheduled        /* do a scheduled flush? */
);

/*===========================================================================

FUNCTION       SRCH4_ADD_TASK
DESCRIPTION    This function adds a task to specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified, tasks are added to the DSP

===========================================================================*/
extern int32 srch4_add_task
(
  srch4_srch_type         task_srch_type, /* search type                  */
  srch4_queue_type        queue_type,     /* queue to add tasks to        */
  srch4_task_struct_type *task            /* task to add                  */
);

/*===========================================================================

FUNCTION       SRCH4_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the
                specified priority queue.
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified. A queue of searches is started

===========================================================================*/
extern int32 srch4_start_search
(
  srch4_srch_type  srch4_type,            /* type of search                 */
  srch4_queue_type queue_type,            /* queue to start                 */
  int16  freq_offset,                     /* freq offset ( 4.6875 Hz/LSB )  */
  uint16 priority,                        /* search priority (32767=highest)*/
  void (*callback_func)(void *),          /* pointer to a callback function */
                                          /*  to be called upon status dump */
  void (*lost_dump_callback_func)(void *),/* pointer to a callback function */
                                          /*  to be called upon lost dump   */
  void *(cb_data)                         /* data to be passed to callback  */
);

/*===========================================================================

FUNCTION       SRCH4_ABORT_QUEUE
DESCRIPTION    This function aborts the specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   One queue in srch4 is aborted

===========================================================================*/
extern int32 srch4_abort_queue
(
  srch4_srch_type         task_srch_type, /* search type                    */
  srch4_queue_type        queue_type,     /* queue to abort tasks to        */
  void                  (*abort_cb_func)( uint32, void * ),
                                          /* pointer to a callback function */
                                          /*  to be called upon abort dump  */
  void                  *(cb_data),       /* callback data                  */
  boolean                 sync,           /* synchronous abort flag         */
  boolean                 part_res        /* partial results flag           */
);

/*===========================================================================

FUNCTION       SRCH4_HALT
DESCRIPTION    This function should only be called in preparation for the
                MDSP app being unregistered for 1x, HDR, and AFLT. Also all
                active searches will be aborted, without any callbacks
                specified. 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   All pending searches will not be processed when a dump occurs
               Any task waiting for a callback from srch4 will NOT recieve 
               them, as all callbacks have been cleared

===========================================================================*/
extern void srch4_halt( void );

/*===========================================================================

FUNCTION       SRCH4_1X_GET_ASSIGNED_QUEUE
DESCRIPTION    This function returns the assigned queue for 1X.
DEPENDENCIES   None

RETURN VALUE   which queue is assigned.
               
SIDE EFFECTS   None

===========================================================================*/
extern srch4_queue_type srch4_1x_get_assigned_queue( void );

#endif /* SRCH4DRV_H */
