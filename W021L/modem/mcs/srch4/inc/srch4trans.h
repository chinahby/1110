#ifndef SRCH4TRANS_H
#define SRCH4TRANS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   T R A N S L A T I O N S
                        H E A D E R F I L E

GENERAL DESCRIPTION
   This file contains the translation functions used by external tasks
   (1X, HDR, AFLT, etc) to translate their structures into srch4's
   generic task structure.

EXTERNALIZED FUNCTIONS
   srch4_mdsp_get_app_valid     Returns status of searcher 4's MDSP App
   srch4_enable                 Enables srch4 per technology
   srch4_mdsp_app_reg_cb        Registers a callback to be called on MDSP
                                 enable/disable
   srch4_register_mdsp_app      Registers srch4 app with the MDSP

   srch4_flush_sample_ram       Pass through function that flushes the
                                 sample RAM
   srch4_get_est_srch_time      Returns the search time for a (512,1) srch
   srch4_trans_cleanup_dump     Called from the drivers so the trans layer
                                 can cleanup after a search dump

   srch4_set_win_center         Sets searcher 4's window center
   srch4_get_win_center         Gets searcher 4's window center
   srch4_slew_win_center        Slews searcher 4's window center

   srch4_XXX_register           Registers technology XXX with srch4, sets
                                 priority and callbacks
   srch4_XXX_deregister         De-registers technology XXX with srch4
   srch4_XXX_active             Checks to see if technology XXX's has a
                                 srch in progress
   srch4_XXX_start_search       Starts technology XXX's search
   srch4_XXX_abort_search       Aborts technology XXX's search
   srch4_XXX_add_task           Adds a single technology XXX task 1x to the
                                 search queue

   srch4_aflt_search_pending    Checks to see if any AFLT searches are pending
   srch4_aflt_continue_search   Programs more searches and starts another
                                 AFLT search

   srch4trans_init              Initializes srch4's translation layer
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2002 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/inc/srch4trans.h#3 $

when       who     what, where, why 
--------   ---     ----------------------------------------------------------
09/08/08   adw     Added SRCH4_AFLT_ENABLED to featurize AFLT.
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
08/14/07   mca     Featurized hdr functions
04/10/06   mt      Added 1x pilot search capability to srch4_hdr_add_task().
02/26/07   sst     Lint fixes
01/19/07   sst     Modified srch4 mdsp app request/release functionality
09/08/06   awj     Condensed offset param in add task to streamline interface
06/15/06   pa      Moved definition of srch4_1x_get_assigned_queue()
                   to srch4drv.h to avoid exporting srch4drv_t.h.
05/31/06   awj     Further sect and srch4 updates
01/16/06   awj     Lint cleanup
12/19/05   bt      Added srch4_1x_get_assigned_queue.
11/18/05   va      Added function to reset AFLT variables
07/19/05   grl     Added flushing the sample ram of the requested chain.
02/04/05   bt      Add scheduled sample ram flush.
01/17/05   sst     Added access function srch4_mdsp_get_app_valid()
12/07/04   sst     Changed srch4_1x_add_task() and srch_sect_srch4_append/_r()
                    to remove offset and add receive diversity field parameters
11/17/04   sst     Update dependency field of srch4_mdsp_app_reg_cb
09/07/04   sq      Changed srch4_hdr_abort_search definition
08/13/04   va      Changed function srch4_aflt_add_task to allow free_q and 
                   win center to be passed as parameters.
08/09/04   ejv     Add new function prototype srch4_1x_wait_and_discard.
07/14/04   jcm     Added srch4_1x_num_tasks_added() and 
                    srch4_1x_num_tasks_req()
07/12/04   sst     Added registration callbacks (implemented for 1x only),
                    this allows for "delayed" assignment of queues when none
                    are available, or un-registered queues are "abort active"
06/30/04   dna     Added a function srch4_disable_mdsp_app() that is to be  
                    called from an SRM object when 1x and HDR both vote they 
                    don't need the SRCH4 dsp image                           
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_HDR_ENABLED, and 
                   SRCH4_AFLT_ENABLED
06/21/04   sst     Updated srch4_enable header
05/28/04   jcm     Sector module integration
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/14/04   sst     dev-Added partial results param to srch4_1x_abort_search()
03/25/04   sst     dev-Moved (de)registering support into tech specific funcs
03/25/04   va      dev-Added function declarations for srch4_aflt_(de)register
03/18/04   sst     dev-Added support for srch4 (de)registering
                   dev-Modified srch4_1x_start_search() parameters
03/04/04   sst     dev-Changed return of srch4_enable() to be a boolean
02/12/04   ddh     Added corrections for lint.
02/02/04   sst     Added synchronous field to 1X abort function
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sst     Added function srch4_get_est_srch_time()
01/06/04   sst     Added parameter age_searches to srch4_1x_add_task to
                    facilitate proper use of search sector aging
11/04/03   bt      Change srch4 voting interface for MSM6500.
10/28/03   aar     Added declaration of function srch4_register_mdsp_app.
10/28/03   ddh     Replaced integer values in srch4_1x_disable with enums
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500 (w/o HDR)
06/17/03   ejv     Define SRCH4_HDR_ENABLED for 6500.
06/15/03   rdh     Added srch4_reserved.
02/18/03   va      SRCH4 AFLT support.
02/12/03   bt      Change sleep_1x_enable to select clk_src.
01/23/03   bt      Added clock voting/control scheme for various technologies.
01/13/03   sst     Added return values to start, add, and abort functions,
                    added func to translate drv return values to trans values,
                    added parameter to lost_dump_cb
12/05/02   sst     Code Clean up
11/07/02   sst     Moved #define SRCH4_WIN_CENTER (debug till we support this)
                   Code cleanup
11/05/02   sst     Merge changes
11/04/02   sst     srch4 win center function moved here from srch4drv
10/22/02   sst     Created File 

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"

#include "comdef.h"
#include "rex.h"
#include "srch_hw_t.h"
#include "sample_server.h"

#include "srch4trans_t.h"

#ifdef SRCH4_HDR_ENABLED
#include "hdrsrchdrv.h"
#endif
#ifdef SRCH4_AFLT_ENABLED
#include "queue.h"
#endif


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*===========================================================================

           M D S P    A P P    R E L A T E D    F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_MDSP_GET_APP_VALID

DESCRIPTION    This function returns TRUE the SRCH4 MDSP app is registered
                and enabled.

DEPENDENCIES   None

RETURN VALUE   Boolean indicating if SRCH4's MDSP App is valid.

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_mdsp_get_app_valid( void );

/*===========================================================================

FUNCTION       SRCH4_ENABLE
DESCRIPTION    This function enables which technology needs searcher 4. If no
               technology needs searcher 4 it will vote for DSP sleep.
DEPENDENCIES   Only one technology's bit shall be set via a given call

RETURN VALUE   True if app is awake and valid
               False if app is not valid or if app was put to sleep
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_enable
(
  srch4_tech_type tech,                    /* technology                   */
  boolean  enable                          /* whether to enable   */
);

/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_REG_CB
DESCRIPTION    This function registers mdsp callbacks to be called upon 
                enable/disable of the srch4 mdsp app
DEPENDENCIES   The registered callback function must handle the case where
                the MDSP app is disabled and searches were pending. In this
                case the searches will be aborted without notification to
                the corresponding task, though this callback will be called

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_mdsp_app_reg_cb
(
  void     (*callback)( boolean ),
  boolean    reg_cb
);

/*===========================================================================

FUNCTION       SRCH4_REGISTER_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   This is legacy support until the use of this interface is
                removed from tmc.c

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_register_mdsp_app
(
  rex_tcb_type *task_tcb,                 /* task requesting searcher 4    */
  rex_sigs_type task_sig                  /* signal to set upon completion */
);

/*===========================================================================

FUNCTION       SRCH4_REQUEST_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
extern void srch4_request_mdsp_app
(
  srch4_tech_type tech_type,              /* mask for technology           */
  rex_tcb_type   *task_tcb,               /* task requesting searcher 4    */
  rex_sigs_type   task_sig                /* signal to set upon completion */
);

/*===========================================================================

FUNCTION       SRCH4_RELEASE_MDSP_APP
DESCRIPTION    Disables the SRCH4 module entirely (not just for sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch4_release_mdsp_app
(
  srch4_tech_type   tech                  /* technology                    */
);


/*===========================================================================

           S R C H 4 T R A N S    U T I L I T Y    F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_FLUSH_SAMPLE_RAM
DESCRIPTION    This function flushes the sample ram in the DSP if no searches
                are active
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   Sample ram in DSP is flushed

===========================================================================*/
extern int32 srch4_flush_sample_ram
(
  sample_server_rx_chain_type samp_serv_chain, /* The sample server chain */
  uint16                      position,        /* position in chips */
  boolean                     scheduled        /* do a scheduled flush? */
);

/*===========================================================================

FUNCTION       SRCH4_GET_EST_SRCH_TIME
DESCRIPTION    This function returns the estimated search time of a single
                search based on the parameters given
DEPENDENCIES   Search Time estimates are for HPQ only
               To estimate total srch time, the queue's overhead 
                (SRCH4_SW_QUEUE_OVHD_US) must be added
               Must be passed coh_len/non_coh_len pairs [512,1]

RETURN VALUE   Estimated search time in us (value of "-1" indicates an error)
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch4_get_est_srch_time
(
  uint16    window_size,               /* window size (chips x8)   */
  uint16    coherent_len,              /* coherent length          */
  uint8     non_coherent_len           /* non-coherent length      */
);

/*===========================================================================

FUNCTION       SRCH4_TRANS_CLEANUP_DUMP
DESCRIPTION    This is used to do general cleanup for queues after a status
               dump is received (abort, lost, dump)
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_trans_cleanup_dump
(
  srch4_srch_type tech                  /* technology releasing it's queue */
);


/*===========================================================================

           S R C H _ W I N _ C E N T E R    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_SET_WIN_CENTER
DESCRIPTION    This function sets searcher 4's window center value
DEPENDENCIES   Window center is in ChipX8

RETURN VALUE   None
SIDE EFFECTS   srch4_win_center is set

===========================================================================*/
extern void srch4_set_win_center
(
  int32 win_center                       /* new window center (ChipX8) */
);

/*===========================================================================

FUNCTION       SRCH4_GET_WIN_CENTER
DESCRIPTION    This function gets searcher 4's window center value (chipX8)
DEPENDENCIES   None

RETURN VALUE   srch4's window center (chipX8) (in the range 0 ... 262143)
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_get_win_center( void );

/*===========================================================================

FUNCTION       SRCH4_SLEW_WIN_CENTER
DESCRIPTION    This function slews searcher 4's window center value and
                returns the new window center (chipX8)
DEPENDENCIES   None

RETURN VALUE   New window center value for srch4  (chipX8)
SIDE EFFECTS   srch4_win_center is slewed

===========================================================================*/
extern int32 srch4_slew_win_center
(
  int32 slew_value                       /* slew amount */
);


/*===========================================================================

           1 X    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_1X_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
                1x will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void (*reg_cb_func)(void *),          /* pointer to a callback function  */
                                        /*  to be called upon successful   */
                                        /*  assignment to a queue          */
  void  *reg_cb_data                    /* register callback data          */
);

/*===========================================================================

FUNCTION       SRCH4_1X_DEREGISTER
DESCRIPTION    This function deregisters 1x from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_1x_deregister( void );

/*===========================================================================

FUNCTION       SRCH4_1X_OFFTL_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
                1x will "own" a queue, and its callbacks will be set.
DEPENDENCIES   If reg_cb_func() is not specified, this is a one shot attempt
                at registering for a queue, if reg_cb_func() is defined, then
                a queue will be assigned to 1X sometime in the future.

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_offtl_register( void );

/*===========================================================================

FUNCTION       SRCH4_1X_OFFTL_DEREGISTER
DESCRIPTION    This function deregisters 1x from its assigned queue
               This function can also be used to unregister a reg_cb_func on
                a failed registration and before the queue has been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_1x_offtl_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_1X_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns TRUE if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_1x_active( void );

/*===========================================================================

FUNCTION       SRCH4_1X_START_SEARCH
DESCRIPTION    This function gets the queue and sends the command to start 
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_start_search
(
  void *(cb_data)                       /* callback data                   */
);

/*===========================================================================

FUNCTION SRCH4_1X_WAIT_AND_DISCARD

DESCRIPTION
      This function is called to wait for the end of a search and 
      then discard it. In case the search has already finished it 
      will just clear the signal

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      sig_mask sigs in srch_tcb are cleared

===========================================================================*/
extern void srch4_1x_wait_and_discard
(
  uint32 sig_mask  /* Which signal(s) to wait for */
);

/*===========================================================================

FUNCTION       SRCH4_1X_ABORT_SEARCH
DESCRIPTION    This function gets the queue and sends the command to abort 
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_abort_search
(
  void    (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
);

/*===========================================================================

FUNCTION       SRCH4_1X_ADD_TASK
DESCRIPTION    This function adds a 1x sector to a 1x srch4 task structure.
DEPENDENCIES   None

RETURN VALUE   Number of tasks that can still be programmed
               Negative value if an error occurred
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_1x_add_task
(
  srch_sect_parm_type    *parms,             /* search parameters           */
  srch_sect_result_type  *results,           /* search results              */
  boolean                 receive_diversity, /* flag to indicate use of RD  */
  boolean                 transmit_diversity /* flag to indicate use of TD  */
);

/*===========================================================================

FUNCTION       SRCH4_1X_NUM_TASKS_ADDED
DESCRIPTION    This function returns the number of tasks that have been 
               successfully added and are not contained in the srch4 queue.
DEPENDENCIES   None

RETURN VALUE   Number of tasks in srch4 queue.
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch4_1x_num_tasks_added(void);

/*===========================================================================

FUNCTION       SRCH4_1X_NUM_TASKS_REQ
DESCRIPTION    This function returns the number of tasks required by a 
               specified sector.
DEPENDENCIES   None

RETURN VALUE   Number of tasks required if sector is added to a srch4 queue.
               
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch4_1x_num_tasks_req
(
  srch_sect_struct_type* sector,            /* sector in question          */
  boolean                transmit_diversity /* flag to indicate use of TD  */
);


#ifdef SRCH4_HDR_ENABLED
/*===========================================================================

           H D R    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_HDR_REGISTER
DESCRIPTION    This function registers hdr with searcher 4. Once registered
                hdr will "own" a queue, and it's priority will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_register
(
  srch4_priority_type priority          /* search priority                 */
);

/*===========================================================================

FUNCTION       SRCH4_1X_DEREGISTER
DESCRIPTION    This function deregisters hdr from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_hdr_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_HDR_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_hdr_active( void );

/*===========================================================================

FUNCTION       SRCH4_HDR_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_start_search
(
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void *(cb_data)                       /* callback data                   */

#ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
#endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
);

/*===========================================================================

FUNCTION       SRCH4_HDR_ADD_TASK
DESCRIPTION    This function adds an HDR sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_hdr_add_task
(
  hdrsrchdrv_srch_list_struct_type  *slist

#ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
#endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */
);
#endif /* SRCH4_HDR_ENABLED */

#ifdef SRCH4_AFLT_ENABLED
/*===========================================================================

           A F L T    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_AFLT_RESET
DESCRIPTION    This function resets pending tasks and callback data for AFLT 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_aflt_reset (void);

/*===========================================================================

FUNCTION       SRCH4_AFLT_REGISTER
DESCRIPTION    This function registers aflt with searcher 4. Once registered
                aflt will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 2.34375 Hz/LSB )  */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *)          /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_DEREGISTER
DESCRIPTION    This function deregisters AFLT from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4_aflt_deregister ( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_SEARCH_PENDING
DESCRIPTION    This function checks if there are any pending AFLT searches
DEPENDENCIES   None

RETURN VALUE   Returns True if AFLT searches are pending
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_aflt_search_pending( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch4_aflt_active( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_start_search
(
  void *(cb_data)                       /* callback data                   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void *(cb_data)                     /* callback data                   */
);

/*===========================================================================

FUNCTION       SRCH4_AFLT_CONTINUE_SEARCH
DESCRIPTION    This function programs the searches on the pending queue and
               starts the searcher again.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_continue_search( void );

/*===========================================================================

FUNCTION       SRCH4_AFLT_ADD_TASK
DESCRIPTION    This function adds an AFLT sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
extern int32 srch4_aflt_add_task
(
  q_type  *aflt_q_ptr,                      /* queue of aflt sectors to add*/
  q_type  *free_q_ptr,                      /* queue of free aflt sectors  */
  int32   ppm_win_center                    /* Win center reference for PPM*/  
);

#endif /* SRCH4_AFLT_ENABLED */


/*===========================================================================

           S R C H 4 T R A N S    I N I T    F U N C T I O N

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4TRANS_INIT
DESCRIPTION    This function initializes the srch4 translation layer. 
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch4trans_init(void);

#endif /* SRCH4TRANS_H */

