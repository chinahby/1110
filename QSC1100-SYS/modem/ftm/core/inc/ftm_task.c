/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   F T M  T A S K  M O D U L E

GENERAL DESCRIPTION
  This file contains functions specific/internal to the FTM Task.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2002 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_task.c#4 $
  $DateTime: 2008/11/19 11:46:56 $
  $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/25/08   sr      Fixed Lint errors
10/14/2008 vm      Merged from //depot/asic/qsc1100/modem/ftm/core/inc/ftm_task.c#3
10/08/2008 vm      Merged from //depot/asic/qsc1100/modem/ftm/core/inc/ftm_task.c#2
2008-07-07 as      Updated with FEATURE_FTM_FMRDS
10/16/06   hkk     WLAN check-ins
07/06/06   anb     Added support to blow fuses via FTM commands.
06/13/06   jac     Fixed compile errors for UMTS targets. Also fixed LINT errors.
06/08/06   hkk     Fixed compiler error/warning 
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/16/06   ra      Add support for vote to sleep
01/18/06   dp      Update to use WCDMA diversity commands.
09/12/05   hkk     Changes for TX/RX sweep cal - added case FTM_COMMON_C
06/16/05   dp      Add support for FTM EGPRS BER feature.
05/02/05   dbc     Added support for MFLO.
04/14/05   xw      Corrected ftm_tcb to tmc_tcb when setting TMC_ACK_SIG.
03/20/04   bmg     Added 1x calibration v2 API
03/11/05   ka      Added dispatch for gsm extended commands.
01/28/05   jfc     Added no_clear versions to ftm_sig_rex_wait(_cb)
01/25/05   ra      removed 0xFFCFFFFF mask in ftm_sign_rex_wait()
                   because this breaks 1x
12/07/04   jfc     Changed ftm_sign_rex_wait() to and mask with 0xFFCFFFFF
12/06/04   jfc     Changed ftm_sig_rex_wait() to NOT clear upper 3 bits of sigs when done
11/23/04   ra      removed obsolete header files
10/19/04   dp      Added dispatcher and cmd q handler for FTM WCDMA BER feature
10/15/04   ka      Removed unused code.
10/12/04   jac     Mainlined ASYNC packet code.
10/07/04   wd      Removed redundant functions ftm_gsm_wait and ftm_wait.
                   Added ftm_sig_rex_wait_cb function which can be passed
                   to drivers that will need to wait while executing on ftm
                   task context.
10/07/04   wd      Added missing break for log_dispatch.
10/06/04   lp      Added ftm_camera feature
10/05/04   ka      Added code to dispatch customer extensions.
10/01/04   wd      Added code to support 16 bit subsys_cmd_code and also
                   rsp_pkt_size in the ftm header.
10/01/04   hk      Added dispatch for Audio.
09/14/04   ra      reinserted 1x ftm_pmic support
09/13/04   ra      removed 1x ftm_pmic support for now
09/09/04   lcl     Bracked ftm_hwtc_bt_init_q with FEATURE_FTM_BT.
09/03/04   xw      Code clean-up.
                   Added ftm_wait() and fixed ftm_clk_rex_wait() bug.
                   Added ftm_gsm_ber_init() in ftm_task_init().
07/29/04   ra      Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "rex.h"



#ifdef FEATURE_FACTORY_TESTMODE
#include "AEEstd.h"
#include "ftm.h"
#include "ftm_task.h"
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_log.h"

#ifdef FTM_HAS_1X
#include "ftm_1x_dispatch.h"
#include "ftm_mc_dispatch.h"
#include "sleep.h"
#endif

#ifdef FTM_HAS_1X_CAL_V2
#include "ftm_1x_cal_v2.h"
#endif /* FTM_HAS_1X_CAL_V2 */

#ifdef FTM_HAS_I2C
#include "ftm_i2c.h"
#endif

#ifdef FTM_HAS_BT
#include "ftm_bt.h"
#endif

#ifdef FTM_HAS_WLAN
#error code not present
#endif

#ifdef FEATURE_PMIC
#include "ftm_pmic.h"
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif /* FTM_HAS_UMTS */

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

#ifdef FTM_HAS_AUDIO
#include "ftm_audio_dispatch.h"
#endif

#ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS
#include "ftm_customer_extensions.h"
#endif

#ifdef FTM_HAS_CAMERA
#include "ftm_camera.h"
#endif

#ifdef FTM_HAS_MFLO
#error code not present
#endif

#ifdef FTM_HAS_QFUSE
#include "ftm_qfuse.h"
#endif

#ifdef FTM_HAS_DSPDIAG
#include "ftm_dspdiag_dispatch.h"
#endif

#include "diagcmd.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"

#include "ftm_common_dispatch.h"

#ifdef FEATURE_FTM_FMRDS
#include "ftm_fmrds.h"
#endif

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

extern ftm_pkt_data_type ftm_pkt_data; /* holds information for current ftm cmd */

/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#ifdef FEATURE_FTM_LEDS
LOCAL rex_timer_type ftm_leds_timer;     /* Timer for the LEDs update     */
#endif

/* Timers used for watchdog reporting and generic purposes. */
LOCAL rex_timer_type ftm_rpt_timer;      /* Timer for the test task       */

/* timer used for ftm_rex_wait */
LOCAL rex_timer_type ftm_wait_timer;

/* Buffers which can be used to Send commands to this task. */
LOCAL ftm_msg_type ftm_task_msg_bufs[ FTM_NUM_CMDS ];


#ifdef FEATURE_FFA
#error code not present
#endif /* FEATURE_FFA */

/* Task command heap */
static mem_heap_type   ftm_heap;

#ifdef FTM_HAS_1X
sleep_okts_handle ftm_okts_handle;
#endif

#ifdef FEATURE_FTM_HWTC
#error code not present
#else
#define FTM_HEAP_SIZE 2048
#endif /* FEATURE_FTM_HWTC */

/* The DIAG memory buffer used by the heap manager. */
static char ftm_heap_buf[FTM_HEAP_SIZE];

/* Command queue for the test task.  All command to this task will be
 placed on this queue to await processing. */
extern q_type ftm_task_cmd_q;
extern q_type ftm_task_event_q;

/*===========================================================================

                               Macros

===========================================================================*/


/* Macro to kick the watchdog timer and set a timer to wake it again.      */

#define FTM_KICK_WATCHDOG() \
   dog_report(DOG_FTM_RPT); \
   (void) rex_set_timer(&ftm_rpt_timer, DOG_FTM_RPT_TIME);


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif



#ifdef FEATURE_FFA
#error code not present
#endif /* FEATURE_FFA */

static void ftm_heap_init (void);

/*==================================================================
FUNCTION FTM_HEAP_ALLOC_FAIL
DESCRIPTION
   Callback function that will be called when heal allocation fails.
===================================================================== */
void ftm_heap_alloc_fail(
   struct mem_heap_struct *heap_ptr,
      /* Heap on which allocation is attempted
      */
   size_t                  request_size
      /* Size of failed request
      */
)
{
  FTM_MSG_ERROR("FTM heap allocation failed bytes requested %d.",request_size,0,0);
}

/*==================================================================
FUNCTION FTM_HEAP_INIT
DESCRIPTION
   Initializes the heap that FTM uses to dole out memory.
===================================================================== */
static void ftm_heap_init (void)
{
  mem_init_heap( &ftm_heap, 
                 ftm_heap_buf, 
                 sizeof (ftm_heap_buf), 
                 &ftm_heap_alloc_fail ); /*lint !e546*/
  return;
} /* End ftm_heap_init */


/* ==================================================================
FUNCTION FTM_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *ftm_malloc
(
  unsigned long num_bytes
)
{
  return mem_malloc( &ftm_heap, num_bytes );
} /* END ftm_malloc */

/* ==================================================================
FUNCTION FTM_FREE
DESCRIPTION
   Frees a chunk of memory allocated using ftm_malloc.
===================================================================== */
void ftm_free
(
  void *mem_ptr
)
{
  mem_free( &ftm_heap, mem_ptr );
  return;
} /* END ftm_free */

/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_rex_wait
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */

  /* if mask is 0, then there is nothing to wait on */
  if (mask == 0)
  {
    return mask;
  }

  sigs = ftm_sig_rex_wait_no_clear(mask);

  (void) rex_clr_sigs(&ftm_tcb, mask);

  return(sigs);
} /* End ftm_sig_rex_wait */


/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The signals this function waits for are not cleared
  at the end.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_rex_wait_no_clear
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  rex_sigs_type   sigs = 0;   /* Signals returned by rex_get_sigs call */

#ifdef FEATURE_FTM_LEDS
  static ftm_wait_leds = 1;   /* wait leds counter */

  ftm_leds(FTM_WAIT_LEDS_MASK, ftm_wait_leds, FTM_WAIT_LEDS_SHIFT);
#endif

  /* if mask is 0, then there is nothing to wait on */
  if (mask == 0)
  {
    return mask;
  }

  ASSERT (rex_self () == &ftm_tcb);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for signals specified in mask and   */
  /* for watchdog signal.  If watchdog signal then kick it  */
  /* and loop again until some other signal in mask is set. */

  while ((sigs & mask) == 0)
  {
    sigs = rex_wait( mask              |     /* Custom SIG            */
                     FTM_RPT_TIMER_SIG |     /* Watchdog report timer */
                     FTM_LEDS_SIG            /* Update LEDs           */
                     );

    if ( sigs & FTM_RPT_TIMER_SIG )
    {
      FTM_KICK_WATCHDOG();
    }

  #ifdef FEATURE_FTM_LEDS
    if ( sigs & FTM_LEDS_SIG )
    {
      ftm_wait_leds++;
      ftm_leds(FTM_WAIT_LEDS_MASK, ftm_wait_leds, FTM_WAIT_LEDS_SHIFT);

      /* re-start timer */
      (void) rex_set_timer( &ftm_leds_timer, FTM_WAIT_LEDS_RATE);
    }
  #endif /* FEATURE_FTM_LEDS */
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) rex_clr_sigs(&ftm_tcb, FTM_RPT_TIMER_SIG |     /* Watchdog report timer */
                                FTM_LEDS_SIG            /* Update LEDs           */
                                );

#ifdef FEATURE_FTM_LEDS
  ftm_leds(FTM_WAIT_LEDS_MASK, 0x00, FTM_WAIT_LEDS_SHIFT);
#endif /* FEATURE_FTM_LEDS */

  /* kick watchdog one last time and return the signals. */
  FTM_KICK_WATCHDOG();
  return(sigs);
} /* End ftm_sig_rex_wait_no_clear */

/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_CB

DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait where a
  [ void func (uint32) ] function signature is needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_wait_cb
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  (void) ftm_sig_rex_wait(mask);
} /* End ftm_sig_rex_wait_cb */


/*===========================================================================

FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR_CB

DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait_no_clear where
  a [ void func (uint32) ] function signature is needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_wait_no_clear_cb
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  (void) ftm_sig_rex_wait_no_clear(mask);
} /* End ftm_sig_rex_wait_no_Clear_cb */


/*===========================================================================

FUNCTION FTM_SIG_REX_SET

DESCRIPTION
  This procedure sets a signal for ftm_task to handle.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_sig_rex_set
(
  rex_sigs_type   mask        /* Mask of signals to wait for */
)
{
  (void) rex_set_sigs(&ftm_tcb, mask);
} /* End ftm_sig_rex_set */

/*===========================================================================

FUNCTION FTM_CLK_REX_WAIT

DESCRIPTION
   This function does a wait in ms, but does not hug the cpu. instead it lets
   the OS (REX) run and other tasks run.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_clk_rex_wait (uint32 msec)
{
  /* Set wait timer to msec */
  (void) rex_set_timer(&ftm_wait_timer, msec);

  /* Waiting for timer to expire and set the sig */
  (void) ftm_sig_rex_wait(FTM_CLK_WAIT_SIG);
} /* End ftm_clk_rex_wait */

/*===========================================================================
FUNCTION FTM_SIG_CLK_REX_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
  Use FTM_WAIT_FOREVER for msec for this function to behaive just like
  ftm_sig_rex_wait.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call. The return
  value is the current siganls from REX, if this value is AND with
  FTM_WAIT_TIMEOUT and the result is FTM_WAIT_TIMEOUT then the operation
  timedout.


SIDE EFFECTS
  None.

===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
)
{
  rex_sigs_type sigs;

  if ( msec != FTM_WAIT_FOREVER )
  {
    (void) rex_set_timer(&ftm_wait_timer, msec);

    sigs = ftm_sig_rex_wait(FTM_CLK_WAIT_SIG | mask);

    if ( !(sigs & FTM_CLK_WAIT_SIG) )
    {
      (void)rex_clr_timer(&ftm_wait_timer);
    }
  }
  else
  {
    sigs = ftm_sig_rex_wait (mask);
  }

  return sigs;

} /* End ftm_sig_clk_rex_wait */

/*===========================================================================

FUNCTION FTM_TASK_INIT

DESCRIPTION
  This is the Factory Test Mode(FTM) task initialize file. It sets up all
  necessary items for FTM to run.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  ftm_task_init( void )
{

   /* Initialize command and "free" queues - - - - - - - - - - - - - - - - - -*/
  (void) q_init( &ftm_task_cmd_q );       /* Initialize command queue */

  (void) q_init( &ftm_task_event_q );     /* Initialize command queue */


  /* init our memory heap */
  ftm_heap_init();

  /* Initialize timers */
  rex_def_timer( &ftm_wait_timer, rex_self(), FTM_CLK_WAIT_SIG );

#ifdef FTM_HAS_1X
  ftm_okts_handle = sleep_register("FTM",TRUE);   
#endif

#ifdef FTM_HAS_DSPDIAG
  ftm_dspdiag_init();
#endif /* FTM_HAS_DSPDIAG */

  /*
  ** Wait for the start signal from the Task Main Controller
  */
  tmc_task_start();

  (void) rex_clr_sigs( rex_self(), TASK_START_SIG );

  /* Initialize timers - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  rex_def_timer(&ftm_rpt_timer, &ftm_tcb, FTM_RPT_TIMER_SIG); /* Dog Report */

#ifdef FEATURE_FTM_LEDS
  rex_def_timer(&ftm_leds_timer, &ftm_tcb, FTM_LEDS_SIG);     /* LED Update */
#endif /* FEATURE_FTM_LEDS */

#if 0 /* for future use */
  /* register function to hanlde async pkts */
  diag_idle_processing_register (ftmdiag_idle_processing_cb, NULL);
#endif

  /* call ftmdiag_init last, we need to make sure we have init all our
  resouces, before we can accept any cmds from diag.
  */
  ftmdiag_init();

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

#ifdef FTM_HAS_WLAN
#error code not present
#endif /* FTM_HAS_WLAN */

  FTM_MSG_LOW("FTM task Initialization finished",0,0,0);
} /* End ftm_task_init */



/*===========================================================================

FUNCTION FTM_PROCESS_EVENTS

DESCRIPTION
   This function handles delayed rsp requests from Diag + events from other 
   susbsystems. Dispatches the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

LOCAL void ftm_process_events(void)
{

  ftm_event_type   *evt_ptr;                     /* Pointer to a ftm event */

  FTM_MSG_MED("ftm_process_events",0,0,0);

  /* Get an event from the Queue.
  */
  while ( ( evt_ptr = (ftm_event_type *) q_get( &ftm_task_event_q) ) != NULL)
  {
    switch (evt_ptr->evt_cat)
    {
    case FTM_EVENT_DIAG:
      FTM_MSG_MED("ftm diag event",0,0,0);
      ftm_diag_evt_processor(evt_ptr->evt_data_ptr);
      break;

    case FTM_EVENT_ADSP:
      FTM_MSG_MED("ftm dsp_diag event",0,0,0);
#ifdef FTM_HAS_DSPDIAG
      ftm_dspdiag_evt_processor(evt_ptr->evt_data_ptr);
#endif
      break;

    default:
      /* message */
      break;
    }

    /* If there is a 'done queue' the queue the command on that queue
    ** as we are done processing it.
    */
    if (evt_ptr->hdr.done_q_ptr != NULL)
    {
      q_put(evt_ptr->hdr.done_q_ptr, &evt_ptr->hdr.link);
    }

    /* free up the event structure also */
    (void) ftm_free(evt_ptr);
  }
}



/*===========================================================================

FUNCTION FTM_ALLOCATE_DELAYED_RSP_BYTES

DESCRIPTION
   This function requests delayed response bytes from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

byte* ftm_allocate_delayed_rsp_bytes(
                           diagpkt_subsys_delayed_rsp_id_type delay_rsp_id,
                           diagpkt_subsys_cmd_code_type subsys_cmd_code,
                           uint16 rsp_cnt,
                           uint32 status, 
                           uint32 length)
{
  void* rsp_bytes = NULL;
  ftm_diagpkt_subsys_hdr_type_v2* rsp_hdr;

  rsp_bytes = (byte*) diagpkt_subsys_alloc_v2_delay(
                           (diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 
                           subsys_cmd_code, 
                           delay_rsp_id,
                           length);

  if (rsp_bytes)
  {
    rsp_hdr = (ftm_diagpkt_subsys_hdr_type_v2 *)rsp_bytes;
    // the response count is set to 1 by the API function (and status to 0)
    diagpkt_subsys_set_rsp_cnt(rsp_bytes, rsp_cnt);
    rsp_hdr->status = status;
  }

  return rsp_bytes;
}


/*===========================================================================

FUNCTION FTM_DIAG_EVT_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_evt_processor(void* pdata)
{

  ftm_diag_event_type            *diag_req;
  ftm_diagpkt_subsys_header_type *header;

  diag_req = (ftm_diag_event_type*) pdata;
  header = (ftm_diagpkt_subsys_header_type*) diag_req->ftm_data_ptr;

  /* route command depending on header information */

  // clean DIAG Payload bytes inside the 3 following fns
  switch (header->cmd_code)
  {
  case DIAG_SUBSYS_CMD_VER_2_F:
    ftm_diag_delayed_rsp_cmd_processor(pdata);
    break;

  case DIAG_EXT_MSG_CONFIG_F:
    ftm_diag_msg_config_cmd_processor(pdata);
    break;

  case DIAG_LOG_CONFIG_F:
    break;

  default:
    /* message */
    break;
  }

  /* clean request struct memory here */
  if (diag_req->ref_ct == 0)
  {
    ftm_free(diag_req);
  }
}



/*===========================================================================

FUNCTION FTM_DIAG_MSG_CONFIG_CMD_PROCESSOR

DESCRIPTION
   This function handles 125 requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_msg_config_cmd_processor(void* pdata)
{
  uint16                          ftm_subsys_code;
  ftm_diag_125_event_type        *diag_req;

  diag_req = (ftm_diag_125_event_type*) pdata;

  ftm_subsys_code = diag_req->ftm_subsys_code;

  switch (ftm_subsys_code)
  {
  case FTM_DSPDIAG_C:

#ifdef FTM_HAS_DSPDIAG
    ftm_dspdiag_msg_config_cmd_dispatch(pdata);
#endif
    break;

  case FTM_MODE_MAX_C:

#ifdef FTM_HAS_DSPDIAG
    ftm_dspdiag_msg_config_cmd_dispatch(pdata);
#endif
    // send it to other subsystems if interested

    break;

  default:

    break;
  }

  if (diag_req->ftm_diag_event.ftm_data_ptr)
  {
    if (diag_req->ftm_diag_event.ref_ct == 0)
    {
      ftm_free(diag_req->ftm_diag_event.ftm_data_ptr);
    }
  }
}



/*===========================================================================

FUNCTION FTM_DIAG_DELAYED_RESPONSE_CMD_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_delayed_rsp_cmd_processor(void* pdata)
{
  // ret_code Will be set to subsystem specific status like a 
  // dspdiag_ret_code_type. So 1st element of enum should be a
  // generic failure like DSPDIAG_FAILED = 0
  word ret_code = 0;  

  ftm_diag_128_event_type        *diag_req;
  ftm_subsys_cmd_packet_type     *subsys_payload;
  ftm_subsys_rsp_packet_type     *rsp_packet;
  boolean                         bypass_ftm_mode_check;
  uint32                          rsp_length;
//  uint32                          subsys_payload_len;

  diag_req = (ftm_diag_128_event_type*) pdata;

  subsys_payload = 
    (ftm_subsys_cmd_packet_type*)diag_req->ftm_diag_event.ftm_data_ptr;

  bypass_ftm_mode_check = FALSE; 
  /* route command depending on header information */

  FTM_MSG_HIGH("Start processing FTM packet: %d,%d,%d",
              subsys_payload->diag_header.cmd_code,
              subsys_payload->diag_header.subsys_id,
              subsys_payload->diag_header.subsys_cmd_code);

#ifdef FTM_HAS_DSPDIAG
  if(subsys_payload->diag_header.subsys_id == FTM && 
     subsys_payload->diag_header.subsys_cmd_code == FTM_DSPDIAG_C)
  {
    bypass_ftm_mode_check = TRUE;
  }
#endif

#ifndef FEATURE_FTM_HWTC
  if (ftm_mode == FTM_MODE || bypass_ftm_mode_check)
  {
#endif
    switch (subsys_payload->diag_header.subsys_id)
    {
    case FTM:
      switch (subsys_payload->diag_header.subsys_cmd_code)
      {
#ifdef FTM_HAS_DSPDIAG
      case FTM_DSPDIAG_C:
        ret_code = ftm_dspdiag_delayed_rsp_cmd_dispatch(diag_req);
        break;
#endif

      default:
        FTM_MSG_ERROR("Unknown FTM Command Code: %d",
                      subsys_payload->diag_header.subsys_cmd_code,0,0);
        // rsp_pkt.cmd = FTM_RSP_BAD_CMD;
        break;
      }
      break;

    default:
      FTM_MSG_ERROR("Unknown subsys id",subsys_payload->diag_header.subsys_id,0,0);
      // rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      break;
    } /* Switch */

#ifndef FEATURE_FTM_HWTC
  }

  else
  {
    FTM_MSG_ERROR("Mode = %d, Not in FTM mode, Aborting Command.",
                  ftm_mode,0,0);
    // rsp_pkt.cmd = FTM_RSP_BAD_MODE;
  }
#endif

  FTM_MSG_MED("Done processing FTM packet: %d,%d,%d",
              subsys_payload->diag_header.cmd_code,
              subsys_payload->diag_header.subsys_id,
              subsys_payload->diag_header.subsys_cmd_code);

  // send back a unified delayed response from here


  // subsys_payload_len = diag_req->ftm_diag_event.req_pkt_len - 
  //                       sizeof(ftm_diagpkt_subsys_header_type);


  rsp_length = sizeof(ftm_diagpkt_subsys_hdr_type_v2); // + 
               // subsys_payload_len;
               

  rsp_packet = 
    (ftm_subsys_rsp_packet_type*) ftm_allocate_delayed_rsp_bytes(
                                    diag_req->delay_rsp_id,
                                    subsys_payload->diag_header.subsys_cmd_code,
                                    (uint16) 1,
                                    (uint32) ret_code,  
                                    (uint32) rsp_length);


  // subsys_payload = (ftm_subsys_cmd_packet_type*)diag_req;
  /*
  if (rsp_packet)
  {
    memcpy((void*)rsp_packet->data, 
           (void*)subsys_payload->data, 
           subsys_payload_len);
  
    diagpkt_delay_commit(rsp_packet);
  }*/

  diagpkt_delay_commit(rsp_packet);
  

  /* free up the ftm pkt memory. Subsystem may incr. ref_ct
     if it wants to clean it up later on.
  */
  if (diag_req->ftm_diag_event.ftm_data_ptr)
  {
    if (diag_req->ftm_diag_event.ref_ct == 0)
      ftm_free(diag_req->ftm_diag_event.ftm_data_ptr);
  }
}


/*===========================================================================

FUNCTION FTM_REQ

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_req(void)
{
  ftm_msg_type                   *msg_ptr;          /* Pointer to a ftm msg               */
  ftm_diagpkt_subsys_header_type *header;           /* For shorthand access to parameters */
  boolean                         bypass_ftm_mode_check = FALSE;

  ftm_rsp_pkt_type            rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

#ifdef FEATURE_FTM_LEDS
  static word                ftm_cmd_leds = 0;
#endif /* FEATURE_FTM_LEDS */

  /* Get a command from the Queue.
  */
  while ( ( msg_ptr = (ftm_msg_type *) q_get( &ftm_task_cmd_q) ) != NULL)
  {
#ifdef FEATURE_FTM_LEDS
    /* display command led count */
    if (ftm_mode == FTM_MODE)
    {
      ftm_cmd_leds++;
      ftm_leds(FTM_CMD_LEDS_MASK,ftm_cmd_leds,FTM_CMD_LEDS_SHIFT);
    }
#endif /* FEATURE_FTM_LEDS */

    /* update current pkt data information */
    ftm_pkt_data.req_pkt_len = msg_ptr->req_pkt_len;
    ftm_pkt_data.rsp_pkt_len = msg_ptr->rsp_pkt_len;
    ftm_pkt_data.ftm_data_ptr = msg_ptr->ftm_data_ptr;
    ftm_pkt_data.data_rsp_ptr = msg_ptr->data_rsp_ptr;

    /* route command depending on header information */
    header = (ftm_diagpkt_subsys_header_type*) msg_ptr->ftm_data_ptr;

    FTM_MSG_HIGH("Start processing FTM packet: %d,%d,%d",
                header->cmd_code,
                header->subsys_id,
                header->subsys_cmd_code);

#ifdef FTM_HAS_LOGGING
    if(header->subsys_id == FTM && header->subsys_cmd_code == FTM_LOG_C)
    {
      bypass_ftm_mode_check = TRUE;
    }
#endif
    /* Currently have this outside of check for FTM_MODE so BT can be tested with
     * this interface in AMSS mode */
#ifdef FTM_HAS_BT
    if(header->subsys_id == FTM && header->subsys_cmd_code == FTM_BT_C)
    {
      bypass_ftm_mode_check = TRUE;
    }
#endif /* FEATURE_BT */

#ifdef FTM_HAS_WLAN
#error code not present
#endif /* FTM_HAS_WLAN */

#ifdef FTM_HAS_MFLO
#error code not present
#endif /* FTM_HAS_MEDIAFLO */

#ifndef FEATURE_FTM_HWTC
    if ( ftm_mode == FTM_MODE || bypass_ftm_mode_check)
    {
#endif
      switch (header->subsys_id)
      {
      case FTM:
        switch (header->subsys_cmd_code)
        {
#ifdef FTM_HAS_1X
        case FTM_1X_C:
          rsp_pkt = ftm_1x_dispatch(RFCOM_TRANSCEIVER_0, (ftm_pkt_type*)msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FEATURE_MULTI_RX_CHAIN
        case FTM_1X_RX_2_C:
          rsp_pkt = ftm_1x_dispatch(RFCOM_RECEIVER_1, (ftm_pkt_type*)msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FTM_HAS_I2C
        case FTM_I2C_C:
          rsp_pkt = ftm_i2c_dispatch ((ftm_i2c_pkt_type *)msg_ptr->ftm_data_ptr);
          break;
#endif /* FTM_HAS_I2C */

#ifdef FEATURE_FTM_MC
        case FTM_MC_C:
          rsp_pkt = ftm_mc_dispatch((ftm_mc_pkt_type*)msg_ptr->ftm_data_ptr);
          break;
#endif /*FEATURE_FTM_MC*/

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FTM_HAS_BT
        case FTM_BT_C:
          rsp_pkt = ftm_bt_dispatch((ftm_bt_pkt_type*) msg_ptr->ftm_data_ptr);
          break;
#endif /*FTM_HAS_BT*/

#ifdef FTM_HAS_WLAN
#error code not present
#endif /*FTM_HAS_BT*/


#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_GSM_EXTENDED
#error code not present
#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FTM_HAS_LOGGING
        case FTM_LOG_C:
          rsp_pkt = ftm_log_dispatch((ftm_log_req_pkt_type*)msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FEATURE_MMGPS
#error code not present
#endif

#ifdef FEATURE_PMIC
        case FTM_PMIC_C:
          rsp_pkt = ftm_pmic_dispatch( (ftm_pmic_pkt_type *) msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_AUDIO
        case FTM_AUDIO_C:
          rsp_pkt = ftm_audio_dispatch((ftm_audio_pkt_type*) msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif

#ifdef FEATURE_FTM_CUSTOMER_EXTENSIONS
        case FTM_MODE_FACTORY_C:
          rsp_pkt = ftm_customer_extensions_dispatch((ftm_custext_pkt_type *)msg_ptr->ftm_data_ptr);
          break;
#endif
#ifdef FEATURE_FTM_FMRDS
        case FTM_FMRDS_C:
           rsp_pkt = ftm_fmrds_api_dispatch((ftm_fmrds_req_pkt_type *)msg_ptr->ftm_data_ptr);
           break;
#endif

#ifdef FTM_HAS_CAMERA
        case FTM_CAMERA_C:
          rsp_pkt = ftm_camera_dispatch((ftm_camera_pkt_type*) msg_ptr->ftm_data_ptr);
          break;
#endif

#ifdef FTM_HAS_1X_CAL_V2
        case FTM_1X_CAL_V2_C:
          rsp_pkt = ftm_1x_cal_dispatch( msg_ptr->ftm_data_ptr );
          break;
#endif /* FTM_HAS_1X_CAL_V2 */

#ifdef FTM_HAS_MFLO
#error code not present
#endif /* FTM_HAS_MFLO */

        case FTM_COMMON_C:
          rsp_pkt = ftm_common_dispatch((ftm_common_pkt_type*) msg_ptr->ftm_data_ptr);
          break;

#ifdef FTM_HAS_QFUSE
        case FTM_QFUSE_C:
          rsp_pkt = ftm_qfuse_dispatch( (ftm_qfuse_pkt_type*)msg_ptr->ftm_data_ptr);
          break;
#endif          

        default:
          FTM_MSG_ERROR("Unknown FTM Command Code: %d",header->subsys_cmd_code,0,0);
          rsp_pkt.cmd = FTM_RSP_BAD_CMD;
          break;
        }
        break;


#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

      default:
        FTM_MSG_ERROR("Unknown subsys id",header->subsys_id,0,0);
        rsp_pkt.cmd = FTM_RSP_BAD_CMD;
        break;
      } /* Switch */

#ifndef FEATURE_FTM_HWTC
    }

    else
    {
      FTM_MSG_ERROR("Mode = %d, Not in FTM mode, Aborting Command.",ftm_mode,0,0);
      rsp_pkt.cmd = FTM_RSP_BAD_MODE;
    }
 #endif

    /* If there is a 'done queue' the queue the command on that queue
    ** as we are done processing it.
    */
    if (msg_ptr->hdr.done_q_ptr != NULL)
    {
      q_put(msg_ptr->hdr.done_q_ptr, &msg_ptr->hdr.link);
    }

    FTM_MSG_MED("Done processing FTM packet: %d,%d,%d",
                header->cmd_code,
                header->subsys_id,
                header->subsys_cmd_code);

    /* Set DIAG_ASYNC_BLOCK_SIG */
    //ftmdiag_send_async_pkt (msg_ptr);

    /* process rsp pkt */
    if (rsp_pkt.cmd == FTM_RSP_DO_LEGACY)
    {
      rsp_pkt.pkt_len = msg_ptr->rsp_pkt_len;
      rsp_pkt.pkt_payload = msg_ptr->ftm_data_ptr;
      rsp_pkt.delete_payload = FALSE;
    }

    (void)ftmdiag_process_async_rsp_pkt (rsp_pkt);

    /* free up the ftm pkt memory */
    ftm_free(msg_ptr->ftm_data_ptr);

    /* update current pkt data information */
    ftm_pkt_data.req_pkt_len = 0;
    ftm_pkt_data.rsp_pkt_len = 0;
    ftm_pkt_data.ftm_data_ptr = NULL;

    /* free up the message structure also */
    (void) ftm_free(msg_ptr);

  } /* While */
} /* End ftm_req */

/*===========================================================================

FUNCTION FTM_LEDS

DESCRIPTION
  This procedure writes to the LEDs port shifting and masking the data.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
#ifdef FEATURE_FTM_LEDS
void ftm_leds (word mask, word data, byte left_shift)
{
  static word leds;
  word data_copy;

  data_copy = data;

  /* shift bits left to the desired position out of the 16 LEDs available */
  data = data<<left_shift;

  /* do the mask */
  data = data & mask;
  leds = leds & ~mask;

  /* save LEDs state */
  leds = leds | data;

#ifndef FEATURE_FFA
  outpw(LEDS, leds);
#else
#error code not present
#endif /* !FEATURE_FFA */
}
#endif /* FEATURE_FTM_LEDS */

#ifdef FEATURE_FTM_LEDS
#ifdef FEATURE_FFA
#error code not present
#endif /* FEATURE_FFA */
#endif /* FEATURE_FTM_LEDS */

/*===========================================================================

FUNCTION       FTM_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but
  will output to string to TBD.

DEPENDENCIES
  none.

===========================================================================*/
void ftm_printf(ftm_msg_lvl_type msg_lvl_enum, const char *fmt, ... )
{
  char   *local_printf_buffer = NULL;
  AEEVaList arg_ptr;

  /* Allocate memory for local buffer with extra space because we don't
     yet know how big the string will be once the string formatting is
     done
  */
  local_printf_buffer = (char*) ftm_malloc(strlen(fmt) + strlen(fmt));

  /* check that we haev memory to work with */
  if (local_printf_buffer == NULL) return;

  /* first lets format the and copy the string. */
  //va_start( arg_ptr, fmt ); /*lint !e64*/
  AEEVA_START( arg_ptr, fmt );
  //(void)vsprintf(local_printf_buffer, fmt, arg_ptr);
  std_vstrlprintf (local_printf_buffer, strlen (local_printf_buffer), fmt, arg_ptr);
  AEEVA_END( arg_ptr );
  //va_end(arg_ptr);

  /* do string output here */
  switch (msg_lvl_enum)
  {
  case FTM_MSG_LVL_FATAL:
    FTM_MSG_FATAL("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_ERROR:
    FTM_MSG_ERROR("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_HIGH:
    FTM_MSG_HIGH("%s",local_printf_buffer,0,0);
    break;
  case FTM_MSG_LVL_MED:
    FTM_MSG_MED("%s",local_printf_buffer,0,0);
    break;
  default:
  case FTM_MSG_LVL_LOW:
    FTM_MSG_LOW("%s",local_printf_buffer,0,0);
    break;
  }

  /* clean up memeory */
  ftm_free ((void*)local_printf_buffer);

} /* End ftm_printf */

/*===========================================================================

FUNCTION FTM_TASK

DESCRIPTION
  This is the Factory Test Mode(FTM) task, created via rex_def_task
  from the Main Control task.  After processing task start procedure,
  the FTM task enters an infiniten loop, awaiting commands received on its
  command queue.  Each command received is processed to completion before
  the next command is processed.  While in the main loop the task kicks the
  watchdog periodically.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  ftm_task
(
  dword              parm       /* from REX - ignored                     */
)
{
  rex_sigs_type sigs;             /* REX signals word                     */

#ifdef FEATURE_FTM_LEDS
  word          ftm_idle_leds = 1;
  boolean       ftm_leds_toggle = TRUE;
  boolean       clean_leds = TRUE;
#endif /* FEATURE_FTM_LEDS */

/*-------------------------------------------------------------------------*/
  FTM_MSG_MED("FTM task started", 0, 0, 0);

  /* Initialize ftm task */
  ftm_task_init();

  (void)rex_set_sigs(&ftm_tcb, FTM_RPT_TIMER_SIG);

#ifdef FEATURE_FTM_LEDS
  rex_set_sigs(&ftm_tcb, FTM_LEDS_SIG);
#endif /* FEATURE_FTM_LEDS */

  /* Loop forever handling commands */
  for (;;)
  {
    sigs = rex_wait( FTM_RPT_TIMER_SIG    |     /* Watchdog report timer */
                     FTM_CMD_Q_SIG        |     /* Command Queue signal  */
                     TASK_STOP_SIG        |     /* Command to shutdown   */
                     FTM_LEDS_SIG         |     /* Update LEDs           */
                     FTM_EVT_Q_SIG        |     /* Event Queue signal    */

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif
                     #ifdef FTM_HAS_1X
                     FTM_ENABLE_PROT_SIG  |     /* FTM Enable protocol   */
                     #endif
                               FTM_BT_SIG                 /* FTM BT support        */
                   );

    if ( sigs & FTM_RPT_TIMER_SIG )
    {
      FTM_KICK_WATCHDOG();
    }

    if ( sigs & TASK_STOP_SIG)
    {
      (void) rex_set_sigs(&tmc_tcb, TMC_ACK_SIG); /* Acknowledge were done */
      (void) rex_clr_sigs(&ftm_tcb, TASK_STOP_SIG);
    }

    #ifdef FTM_HAS_1X
    if ( sigs & FTM_ENABLE_PROT_SIG)
    {
      (void) rex_clr_sigs( &ftm_tcb, FTM_ENABLE_PROT_SIG);

      FTM_MSG_MED("Wait for MC FTM ready signal.",0,0,0);

      if ( ftm_sig_clk_rex_wait( FTM_MC_READY_SIG, 10000 ) & FTM_WAIT_TIMEOUT )
      {
        ERR("MC is not FTM ready, ftm commands may reboot the phone.",0,0,0);
      }
      else
      {
        (void) rex_clr_sigs( &ftm_tcb, FTM_MC_READY_SIG);
        FTM_MSG_MED("Received MC FTM ready signal.",0,0,0);
      }
    }
    #endif

    #ifdef FTM_HAS_BT
    if ( sigs & FTM_BT_SIG)
    {
        (void)rex_clr_sigs(&ftm_tcb,FTM_BT_SIG);
        ftm_bt_handle_event();
    }
    #endif

    #ifdef FTM_HAS_WLAN
#error code not present
    #endif

    if ( sigs & FTM_CMD_Q_SIG)
    {
      /* Clear the signal prior to emptying the queue, to avoid the
      ** race condition where an item is added just as we are leaving.
      */
      (void) rex_clr_sigs( &ftm_tcb, FTM_CMD_Q_SIG);
      ftm_req();
    }

    if ( sigs & FTM_EVT_Q_SIG)
    {
      (void) rex_clr_sigs( &ftm_tcb, FTM_EVT_Q_SIG);
      FTM_MSG_MED("Rceived ftm event",0,0,0);
      ftm_process_events();
    }

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif

  #ifdef FEATURE_FTM_LEDS
    if (ftm_mode == FTM_MODE)
    {
      if (ftm_idle_leds == 1)
        ftm_leds_toggle = TRUE;
      else if (ftm_idle_leds == (1<<FTM_IDLE_LEDS_BITS))
        ftm_leds_toggle = FALSE;

      if (ftm_leds_toggle)
        ftm_idle_leds = ftm_idle_leds + ftm_idle_leds;
      else
        ftm_idle_leds = ftm_idle_leds / 2;

      ftm_leds(FTM_IDLE_LEDS_MASK,ftm_idle_leds,FTM_IDLE_LEDS_SHIFT);

      if ( (sigs & FTM_LEDS_SIG) || clean_leds == FALSE )
      {
        /* re-start timer */
        (void) rex_set_timer( &ftm_leds_timer, FTM_LEDS_RATE);
      }

      clean_leds = TRUE;
    }
    else if (clean_leds == TRUE)
    {
      clean_leds = FALSE;
    #ifndef FEATURE_FFA
      outpw(LEDS, 0x0000);
    #endif /* FEATURE_FFA */
    }
    else if ( sigs & FTM_LEDS_SIG )
    {
      (void) rex_clr_sigs( &ftm_tcb, FTM_LEDS_SIG);
    } /* if (ftm_mode == FTM_MODE) */
  #endif /* FEATURE_FTM_LEDS */

  } /* for (;;) */
} /* ftm_task */


#endif /* FEATURE_FACTORY_TESTMODE */
