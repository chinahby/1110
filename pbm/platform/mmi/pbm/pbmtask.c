/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES 

GENERAL DESCRIPTION
  This file contains the functions used to manage the phonebook task and 
  queues 

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmtask.c#26 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files 
                   during power up. Only the EF-PBR is read.
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
07/10/08   sg      Handle the PBM_GSDI_SEEK_CB commands from GSDI
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/28/08   clm     Remove posibility of entering infinite loop, if gargabe collect fails
                   to clean up properly when EFS is FULL.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
10/10/06   cvs     Move messages to PBM's diag msg ID
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
02/23/06   cvs     rework memory allocation macros
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for read lock
01/18/05   PA      Fixed compilation issues in non-MMGSDI builds
01/12/05   PA      PBM USIM Support
01/10/05   PA      Kick dog before NV writes.
12/08/04   PA      Kick dog before init to be safe.
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK
#include "pbm.h"
#include "pbmtask.h"
#include "pbmefs.h"
#include "pbmutils.h"

#ifdef FEATURE_MMGSDI
#include "pbmgsdi.h"
#include "pbmuim.h"
#endif /* FEATURE_MMGSDI */

#include "pbm_conversion.h"

static q_type pbm_client_cmd_q;       /* Queue to manage client commands */

static q_type pbm_gsdi_resp_q;        /* Queue to manage GSDI responses */ 

static q_type pbm_notify_evt_q;       /* Queue to manage notify events */

static rex_timer_type pbm_rpt_timer;  /* watchdog timer */

/* Utility macro used within this file to check whether a command size matches
 * its type */
#define PBM_CMD_CHECK_SIZE(ptr, type, action)                             \
          if (!PBM_CMD_RIGHT_SIZE(ptr, type))                             \
          {                                                               \
            PBM_MSG_ERR("Invalid command size (%d) found for %d",         \
                         ptr->size, ptr->cmd_type, 0);                    \
            action;                                                       \
          }

static void pbm_gsdi_resp_handle(void);

/* <EJECT> */
/*===========================================================================
FUNCTION PBM_GET_NV

DESCRIPTION
  Read an item from the NV memory.

  Note that NV read is done in a synchronous fashion; that is, this
  function waits till the read is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_get_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr)
{
  static nv_cmd_type pbm_nv_cmd;

  pbm_nv_cmd.item       = nv_item;
  pbm_nv_cmd.cmd        = NV_READ_F;
  pbm_nv_cmd.data_ptr   = data_ptr;
  pbm_nv_cmd.tcb_ptr    = rex_self();
  pbm_nv_cmd.sigs       = PBM_NV_SIG;
  pbm_nv_cmd.done_q_ptr = NULL;

  pbm_kick_dog();

  (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);
  nv_cmd(&pbm_nv_cmd);
  (void) pbm_wait(PBM_NV_SIG);
  (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);

  if (pbm_nv_cmd.status != NV_DONE_S) {
    PBM_MSG_HIGH("Unable to read nv item %d, error %d", nv_item, 
                  pbm_nv_cmd.status, 0);
  }

  return pbm_nv_cmd.status;
}

/*===========================================================================
FUNCTION PBM_PUT_NV

DESCRIPTION
  Write an item to NV memory.

  Note that NV write is done in a synchronous fashion; that is, this
  function waits till the write is done.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
nv_stat_enum_type pbm_put_nv(nv_items_enum_type nv_item, nv_item_type *data_ptr)
{
  static nv_cmd_type pbm_nv_cmd;

  pbm_nv_cmd.item       = nv_item;
  pbm_nv_cmd.cmd        = NV_WRITE_F;
  pbm_nv_cmd.data_ptr   = data_ptr;
  pbm_nv_cmd.tcb_ptr    = rex_self();
  pbm_nv_cmd.sigs       = PBM_NV_SIG;
  pbm_nv_cmd.done_q_ptr = NULL;

  pbm_kick_dog();

  (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);
  nv_cmd(&pbm_nv_cmd);
  (void) pbm_wait(PBM_NV_SIG);
  (void) rex_clr_sigs(rex_self(), PBM_NV_SIG);

  if (pbm_nv_cmd.status != NV_DONE_S) {
    PBM_MSG_ERR("Unable to write nv item %d, error %d", nv_item, 
                  pbm_nv_cmd.status, 0);
  }

  return pbm_nv_cmd.status;
}

/*===========================================================================
FUNCTION PBM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_kick_dog(void)
{
  dog_report( DOG_PBM_RPT );
  (void) rex_set_timer( &pbm_rpt_timer, DOG_PBM_RPT_TIME );
} /* pbm_kick_dog() */


/*===========================================================================

FUNCTION PBM_CMD_ALLOC

DESCRIPTION
  Allocate a PBM command buffer. The size of command is provided as input.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
void *pbm_cmd_alloc(uint16 size)
{
  pbm_cmd_s_type *cmd;

  if (size < sizeof(pbm_cmd_s_type)) 
  {
    PBM_MSG_ERR("Invalid size for a PBM command %d", size, 0, 0);
    return NULL;
  }

  PBM_MEM_ALLOC(cmd, size);
  PBM_CHECK_PTR_RET(cmd, NULL);

  memset(cmd, 0, size);

  cmd->size = size;
  
  return (void *)cmd;
} 

/*===========================================================================

FUNCTION PBM_CMD_FREE

DESCRIPTION
  Deallocate a PBM command buffer.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pbm_cmd_free(void *ptr)
{
  PBM_MEM_FREEIF(ptr);
} 

/*===========================================================================
FUNCTION PBM_CMD_Q_PUT

DESCRIPTION
  Add an item to the specified queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void pbm_cmd_q_put( q_type *q_ptr, pbm_cmd_s_type *cmd_ptr)
{
  PBM_CHECK_PTR_RET(q_ptr, VOID);
  PBM_CHECK_PTR_RET(cmd_ptr, VOID);

  (void) q_link(cmd_ptr, &cmd_ptr->link);
  q_put(q_ptr, &cmd_ptr->link);
}

/*===========================================================================
FUNCTION PBM_CMD_Q_GET

DESCRIPTION
  Obtain the first item from the specified queue

DEPENDENCIES
  None

RETURN VALUE
  NULL if queue is empty
  pbm_cmd_s_type pointer if successful

SIDE EFFECTS

===========================================================================*/
static pbm_cmd_s_type * pbm_cmd_q_get(q_type *q_ptr)
{
  PBM_CHECK_PTR_RET(q_ptr, NULL);

  return (pbm_cmd_s_type *) q_get(q_ptr);
}

/*===========================================================================
FUNCTION PBM_CLIENT_CMD_Q_PUT

DESCRIPTION
  Add an item to the client command queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_client_cmd_q_put(pbm_cmd_s_type *cmd_ptr)
{
  PBM_CHECK_PTR_RET(cmd_ptr, VOID);

  pbm_cmd_q_put(&pbm_client_cmd_q, cmd_ptr);
  (void) rex_set_sigs(&pbm_tcb, PBM_CLIENT_CMD_SIG);
}

/*===========================================================================
FUNCTION PBM_GSDI_RESP_Q_PUT

DESCRIPTION
  Add an item to the GSDI response queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_gsdi_resp_q_put(pbm_cmd_s_type *cmd_ptr)
{
  PBM_CHECK_PTR_RET(cmd_ptr, VOID);

  pbm_cmd_q_put(&pbm_gsdi_resp_q, cmd_ptr);
  (void) rex_set_sigs(&pbm_tcb, PBM_GSDI_RESPONSE_SIG);
}

/*===========================================================================
FUNCTION PBM_NOTIFY_EVT_Q_PUT

DESCRIPTION
  Add an item to the NOTIFY event queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_notify_evt_q_put(pbm_cmd_s_type *cmd_ptr)
{
  PBM_CHECK_PTR_RET(cmd_ptr, VOID);

  pbm_cmd_q_put(&pbm_notify_evt_q, cmd_ptr);
  (void) rex_set_sigs(&pbm_tcb, PBM_NOTIFY_EVT_SIG);
}

/*===========================================================================
FUNCTION PBM_CLIENT_CMD_HANDLE

DESCRIPTION
  Read the client commands from the queue and handle it

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void pbm_client_cmd_handle(void)
{
  pbm_cmd_s_type *cmd_ptr;
  pbm_write_record_s_type *write_cmd; 
  pbm_write_lock_s_type *write_lock;
  pbm_return_type ret;
  rex_sigs_type sigs;
  boolean bGarbageCollect = FALSE;

  while ((cmd_ptr = pbm_cmd_q_get(&pbm_client_cmd_q)) != NULL) {

    switch (cmd_ptr->cmd_type) {

      case PBM_CMD_WRITE:
        /* PBM Write */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_record_s_type, break);
        write_cmd = PBM_WRITE_CMD_PTR(cmd_ptr); //lint !e826 suspicious cast
        ret = pbm_int_record_write(write_cmd);
        PBM_CHECK_ERR(ret, "PBM internal record write failed");
        bGarbageCollect = TRUE;
        break;

      case PBM_CMD_WRITE_LOCK:
        /* PBM Write lock */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_lock_s_type, break);
        write_lock = PBM_WRITE_LOCK_CMD_PTR(cmd_ptr); //lint !e826 suspicious cast

        g_lock.done = TRUE;

        /* Call callback sent with cmd */
        if (write_lock->proc_func)
        {
          pbm_writecb_data_s_type cb_data;

          cb_data.ret = PBM_SUCCESS;
          cb_data.rec_id = PBM_GET_FIRST_AVAILABLE;
          cb_data.user_data = write_lock->user_data;
          
          (*write_lock->proc_func)(&cb_data);
        }
          

        /* Send notification to registered clients */
        PBM_NOTIFY(PBM_EVENT_LOCKED, lock_type, write_lock->lock_type);

        pbm_cmd_free(cmd_ptr);
        (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
        break;

      case PBM_CMD_WRITE_UNLOCK:
        /* PBM Write unlock */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_write_lock_s_type, break);
        write_lock = PBM_WRITE_LOCK_CMD_PTR(cmd_ptr);  //lint !e826 suspicious cast

        /* Nothing to do except send notification to registered clients */
        PBM_NOTIFY(PBM_EVENT_UNLOCKED, lock_type, write_lock->lock_type);

        pbm_cmd_free(cmd_ptr);
        (void) rex_set_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
        break;        

      default:
        PBM_MSG_ERR("Unknown command %d", cmd_ptr->cmd_type, 0, 0);
        continue;
    }

    /* Handle client commands one at a time.
     * While waiting for command complete signal we have to handle GSDI
     * responses. */
    sigs = 0x0;
    while (!(sigs & PBM_CMD_COMPLETE_SIG))
    {
      sigs = pbm_wait(PBM_CMD_COMPLETE_SIG | PBM_GSDI_RESPONSE_SIG);
      if (sigs & PBM_GSDI_RESPONSE_SIG)
      {
        (void) rex_clr_sigs(&pbm_tcb, PBM_GSDI_RESPONSE_SIG);
        pbm_gsdi_resp_handle();
      }
    }
    (void) rex_clr_sigs(&pbm_tcb, PBM_CMD_COMPLETE_SIG);
  }
  
  if (bGarbageCollect) 
  {
    pbm_efs_garbage_recollect(FALSE);
  }

}

/*===========================================================================
FUNCTION PBM_GSDI_RESP_HANDLE

DESCRIPTION
  Read the GSDI responses from the queue and handle it.
  Typically we do not have to keep the gsdi response data around, so it
  is freed immediately after processing.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void pbm_gsdi_resp_handle(void)
{
#ifdef FEATURE_MMGSDI
  pbm_cmd_s_type *cmd_ptr;
  pbm_gsdi_resp_s_type *gsdi_resp;
  pbm_mmgsdi_resp_s_type *mmgsdi_resp;
  pbm_mmgsdi_card_status_s_type *card_status;
  void *data_ptr = NULL;

  while ((cmd_ptr = pbm_cmd_q_get(&pbm_gsdi_resp_q)) != NULL) {
    switch (cmd_ptr->cmd_type) {
      case PBM_MMGSDI_WRITE_CB:
        /* GSDI write callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
        mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_gsdi_uim_write_cb(mmgsdi_resp->status, 
                              mmgsdi_resp->data_type, 
                              &mmgsdi_resp->data);
        break;

      case PBM_MMGSDI_READ_CB:
        /* GSDI read callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
        mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_gsdi_read_cb(mmgsdi_resp->status, 
                         mmgsdi_resp->data_type, 
                         &mmgsdi_resp->data);
        // do not move this line line into the below if cond..due to some compiler limitations 
        // if we move this line into the if, it is passing value of data_len to 
        // PBM_MEM_FREEIF(as it is not updating register R0)
        data_ptr = (void *)mmgsdi_resp->data.read_cnf.read_data.data_ptr;
        if(mmgsdi_resp->data.read_cnf.read_data.data_len > 0)  
        {
          //free the memory allocated for deep copy
          PBM_MEM_FREEIF(data_ptr);
        }        
        break;

      case PBM_MMGSDI_FILE_INFO_CB:
        /* GSDI file info callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
        mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_fileinfo_cb(mmgsdi_resp->status, 
                        mmgsdi_resp->data_type, 
                        &mmgsdi_resp->data);
        break;

      case PBM_GSDI_SIM_CAP_CB:
        /* GSDI SIM capability callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_gsdi_resp_s_type, break);
        gsdi_resp = PBM_GSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_sim_cap_cb(&gsdi_resp->gsdi_data);
        break;

      case PBM_GSDI_PIN_STATUS_CB:
        /* GSDI PIN status callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_gsdi_resp_s_type, break);
        gsdi_resp = PBM_GSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_pin_status_cb(&gsdi_resp->gsdi_data);
        break;

      case PBM_MMGSDI_CARD_STATUS_CB:
        /* GSDI Card status callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_card_status_s_type, break);
        card_status = PBM_MMGSDI_CARDSTATUS_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_card_status_cb(card_status->mmgsdi_event);
        break;
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_GSDI_READ_PBR_CB:
        /* GSDI PBR read callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_gsdi_resp_s_type, break);
        gsdi_resp = PBM_GSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_usim_read_pbr_cb(&gsdi_resp->gsdi_data);
        break;
#endif /* FEATURE_PBM_USIM_SUPPORT */

#ifdef FEATURE_MMGSDI_ENHANCED_SEARCH
     case PBM_MMGSDI_SEEK_CB:
        /* GSDI seek callback */
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
        mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);  //lint !e826 suspicious cast
        pbm_seek_cb(mmgsdi_resp->status, 
                    mmgsdi_resp->data_type, 
                    &mmgsdi_resp->data);
        //no need to check whether data_len > 0 ,since we are assigning it to NULL in pbm_mmgsdi_seek_cb
        //if it  is data_len <= 0     
        data_ptr = (void *)mmgsdi_resp->data.search_cnf.searched_record_nums.data_ptr; 
        //free the memory allocated for deep copy
        PBM_MEM_FREEIF(data_ptr);      
       break;
#endif /* FEATURE_MMGSDI_ENHANCED_SEARCH */

      default:
        PBM_MSG_ERR("Unknown command %d", cmd_ptr->cmd_type, 0, 0);
        break;
    }

    pbm_cmd_free(cmd_ptr);
  }
#else
  PBM_MSG_ERR("Feature not available", 0, 0, 0);
#endif /* FEATURE_MMGSDI */
}

/*===========================================================================
FUNCTION PBM_NOTIFY_EVT_HANDLE

DESCRIPTION
  Read the Notify events from the queue and handle it.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void pbm_notify_evt_handle(void)
{
  pbm_cmd_s_type        *cmd_ptr = NULL;
  pbm_notify_evt_s_type *notify_evt = NULL;

  while ((cmd_ptr = pbm_cmd_q_get(&pbm_notify_evt_q)) != NULL) {
    if (PBM_CMD_NOTIFY_EVT == cmd_ptr->cmd_type)
    {
      /* PBM Notify Event */
      PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_notify_evt_s_type, break);
      notify_evt = (pbm_notify_evt_s_type*)cmd_ptr;  //lint !e826 suspicious cast
      pbm_notify(&(notify_evt->notify_data));
    } 
    else 
    {
      PBM_MSG_ERR("Unknown command %d", cmd_ptr->cmd_type, 0, 0);
    }
    pbm_cmd_free(cmd_ptr);
  }
}

/*===========================================================================
FUNCTION PBM_TASK_INIT

DESCRIPTION
  Initialization of PBM task structures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void pbm_task_init(void)
{
  rex_init_crit_sect(&pbm_crit_sect);
  rex_init_crit_sect(&pbm_write_lock_crit_sect);
  rex_init_crit_sect(&pbm_read_lock_crit_sect);

  /* Initialize heap */
  (void) pbm_heap_init();

  /* Initialize the Conversion tables. */
  pbmconvert_init_conversion();

  /* Initialize queues */
  (void) q_init(&pbm_client_cmd_q);
  (void) q_init(&pbm_gsdi_resp_q);
  (void) q_init(&pbm_notify_evt_q);

  /* Initialize timer */
  rex_def_timer( &pbm_rpt_timer, &pbm_tcb, PBM_RPT_TIMER_SIG );

  /* Wait for start signal from Main Control task */
  task_start( PBM_RPT_TIMER_SIG, DOG_PBM_RPT, &pbm_rpt_timer );

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Set the watchdog timer */
  (void) rex_set_timer( &pbm_rpt_timer, DOG_PBM_RPT_TIME );
}

/*===========================================================================
FUNCTION PBM_WAIT

DESCRIPTION
  Suspend PBM from executing until at least one of the specified signals
  in the input mask is set. 

DEPENDENCIES
  pbm_rpt_timer must have been initialized

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type pbm_wait(rex_sigs_type mask)
{
  rex_sigs_type    sigs;

  for (;;) {
     
    /* Wait for specified signal or watchdog timer */
    sigs = rex_wait(mask | PBM_RPT_TIMER_SIG);

    if (sigs & PBM_RPT_TIMER_SIG)
    {
      sigs &= ~PBM_RPT_TIMER_SIG; 
      (void) rex_clr_sigs(&pbm_tcb, PBM_RPT_TIMER_SIG);
      pbm_kick_dog();
    }

    if (sigs & mask)
      return sigs;

  } /* forever */
}

/*===========================================================================
FUNCTION PBM_TASK

DESCRIPTION
  The main PBM task.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
//lint -e{715} suppress unused parameters
void pbm_task(dword dummy)
{
  rex_sigs_type sigs;
  pbm_return_type ret;

  pbm_task_init();

  pbm_kick_dog();

  ret = pbm_init();
  PBM_CHECK_ERR(ret, "Failed to initialize PBM");

  for (;;) {      /* Wait forever for rex signals */

    sigs = pbm_wait( PBM_CLIENT_CMD_SIG |
                     PBM_GSDI_RESPONSE_SIG |
                     TASK_OFFLINE_SIG |
                     TASK_STOP_SIG |
                     PBM_NOTIFY_EVT_SIG
                   );


    /* Kick watchdog */
    if (sigs & PBM_RPT_TIMER_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, PBM_RPT_TIMER_SIG);
      MSG_MED("PBM_RPT_TIMER_SIG received", 0, 0, 0);
      pbm_kick_dog();
    }

    /* OFFLINE signal */
    if (sigs & TASK_OFFLINE_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, TASK_OFFLINE_SIG);
      MSG_MED(" TASK_OFFLINE_SIG received",0,0,0);
      task_offline();
    }    

    /* GSDI command */
    if (sigs & PBM_GSDI_RESPONSE_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, PBM_GSDI_RESPONSE_SIG);
      MSG_MED(" PBM_GSDI_RESPONSE_SIG received",0,0,0);
      pbm_gsdi_resp_handle();
    }

    /* Client command */
    if (sigs & PBM_CLIENT_CMD_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, PBM_CLIENT_CMD_SIG);
      MSG_MED(" PBM_CLIENT_CMD_SIG received",0,0,0);
      pbm_client_cmd_handle();
    }

    /* Notify event command */
    if (sigs & PBM_NOTIFY_EVT_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, PBM_NOTIFY_EVT_SIG);
      MSG_MED(" PBM_NOTIFY_EVT_SIG received",0,0,0);
      pbm_notify_evt_handle();
    }

    /* STOP signal */
    if (sigs & TASK_STOP_SIG) {
      (void) rex_clr_sigs(&pbm_tcb, TASK_STOP_SIG);
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      task_stop();
    }

  } /* for (;;) */
}
#endif /* FEATURE_PBM_TASK */
