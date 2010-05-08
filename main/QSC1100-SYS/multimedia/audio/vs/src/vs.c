/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Voice Services Task")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                V O I C E    S E R V I C E S    T A S K

GENERAL DESCRIPTION
  This module contains voice services software.

REFERENCES
  Voice Services ISOD: 80-24875-6

EXTERNALIZED FUNCTIONS

  vs_task
    This function is the entrance procedure for the Voice Services
    Task.  It contains the main processing loop for the VS Task.
  vs_play_utterance
    This function queues a command to VS task to start voice play back.
  vs_play_multi 
    This function queues a command to VS task to start multi play back.
  vs_play_abort
    This function queues a command to VS task to abort voice play back.  
  vs_train_utterance
    This function performs voice tag training.
  vs_train_control_word
    This function performs control word training.
  vs_train_abort
    This function aborts the current training process.
  vs_recognize_utterance
    This function performs the recognition of voice tags and control words.
  vs_recognize_abort
    This function aborts the current recognizing process.
  vs_voice_activate
    Start voice activation.
  vs_voice_activate_abort    
    Abort voice activation.
  vs_implicit_yes
    This function starts an implicit yes/no control.
  vs_menu_selection
    Start prompts listing.
  vs_menu_selection_abort
    Abort menu selection.
  vs_recognize_multi
    This function performs multiple recognitions of voice tags/control words.
  vs_recognize_multi_abort
    This function aborts the current recognizing process.
  vs_set_vr_engine
    This function allows Engine options.  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 1999, 2000, 2001 by QUALCOMM, Incorporated. All Rights Reserved.
Copyright(c) 2002, 2003       by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/vs/main/latest/src/vs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/07    pl     Modified to use new SLEEP API
07/25/06   wyh     Use FEATURE_VIDEO_ENCODE_VERSION for compatibility w/ 6550+
03/22/05   ali     Changes for raw encoder
02/15/05   ali     Reverted changes in rev 9. Use the video effects
                   engine instead of camera
12/22/04   sm      Only set INFORM_SIG when IS733 is acquired.
08/10/04   ali     Changes for video encode overlay
07/10/03   jkl     Added synchronous cb for vs_menusel_abort.
12/20/02   rpw     Added signal VS_VIDEO_ENG_CMD_Q_SIG to vs_task for support
                   of video encoder.
09/10/02   jkl     Updated with multimode task operation interface.
08/30/02   jkl     Moved vr_menu_selection_init parameters check to VS.
07/11/02   jkl     Added list_ptr on vs_pufe_rec_utterance.
           ch      Added PUFE vocoder packet saving support.
02/27/02   jkl     Modified vs_pufe_init to include calibration mode.
02/25/02   jkl     Added PUFE (Universal Front End) Support. 
11/21/01   jkl     Fixed boundary check on Endpoint arrays.
10/03/01   jkl     Modified vs_vm_can_record_memo().
09/18/01   jkl     Modified to include vs_set_vr_engine.
09/14/01   jkl     Added GPS support and Voice Memo QCP format.
07/26/01   jkl     Corrected featurization.
07/17/01    ch     Removed vs_implicit_adaptation(). No longer needed.
06/18/01    ch     Added synchronous VoiceMemo record stop and play stop
06/12/01   jkl     Removed Set/Get Recognition parameters.
05/21/01   jkl     Added vs_recognize_multi. Removed Enhance VR feature.
04/11/01   jkl     Added vs_menu_selection.
04/04/01    st     Added support for SAF.
01/30/01   jkl     Fixed racing on the endpoint routine.
01/17/00   kar     Support for synchronous aborts and modified Message level.
12/05/00    sm     Replaced FEATURE_AUDIO_FORMAT with FEATURE_CMX.
11/21/00   jkl     Added vs_play_abort_processing for TALK OVER.
10/31/00   jkl     Added call to vdb_init().
09/22/00    vl     Reject play request if play pointer is NULL in functions
                   vs_play_multi_processing () and vs_play_memo_utterance ().
09/19/00    vl     Added proper featurization to function vr_play_end_cb ().
09/12/00   jkl     Renamed and kept same function names for different engines.
09/08/00    vl     Moved set timer code to the beginning of vs_voc_acquire ()
                   to avoid condition where VS_VOC_INFORM_SIG is cleared too
                                   early.
08/30/00    vl     Downgraded an error message from ERR_FATAL to ERR.
08/22/00   jkl     Added vs_modebc_train_control_word ().
08/22/00    vl     Call vre_modebc_continuous_vr_enable () in 
                   vs_modebc_continuous_vr_enable ().
08/16/00   jkl     Moved vre_init (calls fs functions) after mc_task_start.
08/04/00    vl     Enable multiple endpoint hypotheses when digit dialing is
                   over.
07/28/00    ro     Change vs command to VS_VM_MEM_TO_QCP.
07/27/00    vl     Deleted obsolete function vs_modebc_endpoint_setting ().  
                   Disable multiple endpoint hypotheses for digit dialing in 
                   function vs_modebc_continuous_vr_enable ().
07/21/00    vl     No longer terminate continuous VR when UI aborts recognition.
                   Release vocoder in continous VR function when continous VR is
                   disabled.
                   Added vs_modebc_train_utterance (needed to be able to do 
                   VR mode B/C duplicate detection) (07/20/00 dpo).
                   Added maximum voice tag utterance size limit support
                   (07/18/00 ch).
07/14/00    ch     Added memo to qcp and qcp to memo conversion support.
                   Added FEATURE_VR_IMPLICIT_ADAPTATION.
06/30/00   dpo/vl  Added function vs_modebc_diagnostic_settings ().
06/28/00    vl     Added function vs_modebc_continuous_recognition_enable ().
06/22/00    vl     Added mode B VR support.  Moved local VS function prototypes
                   from vs.c to vsi.h.
06/09/00   jkl     Modified for VDB 2.0.
05/18/00    st     Added proper featurization; added support for CMX module.
05/03/00    st     Modified code to return FAILURE to VM if playback has
                   been aborted by the vocoder.
05/01/00    st     Added code to handle the ABORTED status for voice playback;
                   added a voc_acquire bail-out timer, if voc_inform callback
                   does not happen within 2 seconds, a failure will be returned.
02/11/00    st     Replaced vs_vactivate_cb_type with vs_rec_cb_type.
02/03/00    st     In each of the VR state machine playback callback routines
                   if a DIFF_SO or BAD_VP status is returned from SND, save 
                   the intermediate status until the play multi is completely
                   done and then call back the state machine with VS_FAILURE
                   so that the state machine exits correctly.
01/25/00    ro     Support for MSM3100.
                   Fix handling of NULL pointers in VS_PLAYBACK_END
                   Remove vs_capture and vs_capture_abort.
                   Remove vs_voc_release.
            st     Use Sound Server API
                   Support for capture timeout and voice activate
10/15/99    st     Function vr_capture_processing() now returns status.
10/08/99    vl     Send command to the Sound driver to turn off the codec when 
                   capture, training or recognition is aborted.
09/29/99    sa     Removed featurization of codec support.
09/29/99    sa     Featurized codec support.
09/28/99    vl     In VR capture state, added commands to the Sound Driver to 
                   disable/enable codec muting for the codec interface 
                   when acquiring/releasing the vocoder.
09/07/99    vl     Added callback functions to report VS queue full error to
                   vs_implicit_yes(), vs_get_recognition_parameters() and
                   vs_set_recognition_parameters().
08/31/99    vl     Changed memory management functions to eliminate the need
                   to reinitialize playback queue in case of error.
08/02/99    ch     Added vs_vm_can_record_memo function.
07/21/99   rw/vl   Changed vdb calls in function vs_play_get_utt().
07/19/99    vl     Changed memory management functions (for playback buffers).  
                   Added play direction to play functions.
06/11/99    vl     Moved error call back handling from play functions to 
                   vs_cmd_parser() to fix recursive call problem.
05/24/99    ch     vs_vm_get_parameters() has another field.
05/24/99    sa     Made changes to training and recognition interfaces to 
                       support VDB recognizable word sets.
05/20/99    vl     Releases vocoder and allows phone to sleep when capture, 
                   training or recognition is aborted.
05/10/99    ch     Supports CANNED UI parameters for set_confing and get_config.
                   Supports direction in play_start and record_start.
05/06/99    vl     Implemented callback handshaking functions.  Changed abort    
                   functions to comply with Voice Services API.                  
04/27/99    vl     Added vs_cmd () call to vs_play_abort ().  Removed call to
                   vs_voc_acquire () from VR functions.  No longer queue VR
                   training, recognition and implicit yes continue states to
                   VR command queue.
04/23/99    vl     Added code to call back if VS command queue is full.
04/16/99    vl     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#ifdef FEATURE_VS_TASK
#include "rex.h"                /* definition of REX data types            */
#include "task.h"               /* Task definitions and prototypes         */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "dog.h"                /* Watchdog signal and time definitions    */
#include "mc.h"                 /* Need for MC signals and prototypes      */
#include "snd.h"                /* Sound typedefs and prototypes           */
#include "voc.h"                /* Vocoder typedefs and prototypes         */
#include "vsi.h"                /* VS internal typedefs and prototypes     */
#include "vs.h"                 /* VS typedefs and prototypes              */
#include "vsdef.h"              /* common definitions among VS             */
#ifdef FEATURE_PHONE_VR
#include "vr.h"                 /* VR typedefs and prototypes              */
#include "vrengine.h"           /* VR Engine typedefs and prototypes       */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */
#include "sleep.h"              /* Sleep interfaces                        */
#ifdef FEATURE_VOICE_DATABASE
#include "vdbrw.h"              /* Voice Database interfaces               */
#include "vdbdata.h"            /* Voice Database interfaces               */
#include "vdbvtag.h"            /* Voice Database interfaces               */
#endif
#include <memory.h>             /* Memory manipulation routines            */

#ifdef FEATURE_CMX
#include "cmxi.h"               /* CMX internal typedefs and prototypes    */
#endif

#ifdef FEATURE_VIDEO_ENCODE
#include "videoengi.h"          /* engine internal typedefs and prototypes */
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined (FEATURE_VIDEO_ENCODE_FADING)
  #if defined(FEATURE_VIDEO_ENCODE_VERSION) && (FEATURE_VIDEO_ENCODE_VERSION < 40)
      #include "video_enceffects.h"
  #endif
#endif
#endif

/* <EJECT> */
/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/

#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO) || \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined (FEATURE_VIDEO_ENCODE_FADING))
/*---------------------------------------------------------------------------
** VS Command Queue, Free Queue and Available Packets
**---------------------------------------------------------------------------
*/
LOCAL q_type           vs_cmd_q;        /* Request Packets Queue           */
LOCAL q_type           vs_free_q;       /* Available Packets Queue         */
LOCAL vs_packet_type   vs_free_packet [VS_NUM_PACKETS];
#endif

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
/*---------------------------------------------------------------------------
** VS Play back Queue and Available Buffers
**---------------------------------------------------------------------------
*/
LOCAL q_type      vs_pb_free_q;    /* Available buffers Queue         */
LOCAL byte        vs_pb_free_buf [VS_PB_NUM_BUFFERS][VS_MAX_UTTERANCE_BYTES];
LOCAL voc_pb_header_type vs_pb_hdr [VS_PB_NUM_BUFFERS];

/* Used to identify voice prompts needed from flash                        
*/
LOCAL int              rwid_array [VS_PB_NUM_BUFFERS];

/* Array of ptrs to voice prompt header (for SND command)                  
*/
LOCAL voc_pb_header_type     *pb_hdr_array [VS_PB_NUM_BUFFERS]; 

/* Sound Multi command buffer (for SND command)                            
*/
LOCAL snd_compact_tone_type         s_multi_array [MAX_NUM_MULTI]; 

/* Identifies type of play back in progress.  Used to reject play requests
** in certain cases.  Also used to send the appropriate abort command to 
** SND.
*/
LOCAL vs_play_type     pb_cmd_in_progress = VS_PLAY_NONE;

/* This local function pointer is used to call back a registered function
** (registered as an input parameter of routine vs_play_utterance,
** vs_play_multi, vs_play_temp_utterance, or vs_play_abort) at the end of 
** the playback process.
*/
LOCAL vs_play_cb_type  vs_play_cb;

/* Holds status for playback */
LOCAL vs_status_type   vs_play_status;
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

/* Watch Dog Timer
*/
LOCAL rex_timer_type   vs_rpt_timer;      

#ifdef FEATURE_PHONE_VR
/* voc_acquire bail-out timer
*/
#define VS_VOC_ACQUIRE_TIME   2000
LOCAL rex_timer_type   vs_voc_acquire_timer;      

/* Enables/disables the use of handshaking on callback functions.
*/
LOCAL boolean          vs_handshake_enable_flag = FALSE;
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO 
/* For Voice Memo, call back after playing each voice utterance.
*/
LOCAL boolean          callback_vm_inprogress = FALSE;
#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_PHONE_VR
/* This flag is set when Voice Activate function is interrupted by DSP
** Image switching.
*/
LOCAL boolean          vs_voice_activate_pending = FALSE;

/* This buffer is used to track if endpoint detection is keeping up with 
** incoming vocoder packets from DSP.
*/
LOCAL vs_endpoint_buffer_type  vs_endpoint_buffer;

/* This flag is set when the VR Endpoint Detector requires vocoder frames.
*/
LOCAL boolean vs_endpoint_capture = FALSE;

/* This flag indicates that the VR Endpoint Detector is being serviced in the
** background task so it's not neccessary to queue up a VS command.
*/
LOCAL volatile boolean vs_endpoint_in_progress = FALSE;

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */


/* Macros to enable/disable sleep.
*/
#ifdef FEATURE_NEW_SLEEP_API
const char *const vs_sleep_str = "VS_SLEEP";
sleep_okts_handle vs_sleep_handle;

#define VS_ENABLE_SLEEP() \
          sleep_assert_okts(vs_sleep_handle)

#define VS_DISABLE_SLEEP() \
          sleep_negate_okts(vs_sleep_handle)

#else 

#define VS_ENABLE_SLEEP() \
          SLEEP_ASSERT_OKTS ( SLEEP_VS_OKTS_SIG ) 

#define VS_DISABLE_SLEEP() \
          SLEEP_NEGATE_OKTS ( SLEEP_VS_OKTS_SIG)

#endif /* FEATURE_NEW_SLEEP_API */




/* <EJECT> */
#ifdef VS_TEST_CODE
#error code not present
#endif /* VS_TEST_CODE */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_task

DESCRIPTION
  This procedure is the entrance procedure for the Voice Services
  Task.  It contains the main processing loop for the VS Task.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  Does not return

SIDE EFFECTS
  vs_tcb

===========================================================================*/
void vs_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type   sigs;

  /* Initialize VS Task
  */
  vs_task_init ();

  /* Process task startup procedure from task controller.
  */
  (void) task_start (VS_RPT_TIMER_SIG, DOG_VS_RPT, &vs_rpt_timer);

  /* Clear start signal
  */
  (void) rex_clr_sigs (&vs_tcb, TASK_START_SIG);

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs (&vs_tcb, VS_RPT_TIMER_SIG);

#ifdef FEATURE_PHONE_VR
  /* Initialize Voice Database
  */
  
  vdb_init();
  /* Initialize the VR Engine.
  */
  vre_init( voc_get_shared_mem_ptr() );

#endif /* FEATURE_PHONE_VR */
  
  /* Save battery by allowing phone to sleep.
  */
  VS_ENABLE_SLEEP();
  
  /* Infinite loop -- process events forever
  */
  for (;;) {

    /* Wait for something to happen
    */
    sigs = rex_wait( VS_RPT_TIMER_SIG          |   /* Watchdog report timer  */
#ifdef FEATURE_VIDEO_ENCODE
                     VS_VIDEO_ENG_CMD_Q_SIG    |   /* video engine queue sig */
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_CMX
                     VS_CMX_CMD_Q_SIG          |   /* CMX cmd queue signal   */
#ifdef FEATURE_SAF
                     VS_CMX_SAF_FRAME_SIG      |   /* CMX SAF frame signal   */
#endif /* FEATURE_SAF */
#endif /* FEATURE_CMX */
#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO) || \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined(FEATURE_VIDEO_ENCODE_FADING))
                     VS_CMD_Q_SIG              |   /* Command queue signal   */
                     VS_VOC_INFORM_SIG         |   /* DSP State change       */
#endif
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
                     VS_VP_DONE_SIG            |   /* Done playing voice     */
#endif
                     TASK_STOP_SIG             |   /* Task stop from MC      */
                     TASK_OFFLINE_SIG);            /* Task offline from MC   */

    /* If watchdog report timer expired, report and restart the timer
    */
    if ((sigs & VS_RPT_TIMER_SIG) != 0) 
    {
      dog_report (DOG_VS_RPT);
      (void) rex_set_timer (&vs_rpt_timer, DOG_VS_RPT_TIME);
    }

    /*---------------------------------------------------------------------*/

#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO) || \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined(FEATURE_VIDEO_ENCODE_FADING))
    /* Process the queued commands
    */
    if ((sigs & VS_CMD_Q_SIG) != 0 ) 
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_CMD_Q_SIG);
      vs_cmd_parser ();
    }
#endif
#ifdef FEATURE_PHONE_VR
    if ( ((sigs & VS_VOC_INFORM_SIG) != 0) && vs_voice_activate_pending ) 
    {
      /* Clear the 'Inform' signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_VOC_INFORM_SIG);
      vs_voice_activate_pending = FALSE;
      vr_voice_activate ();
    }
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_CMX
#ifdef FEATURE_SAF
    /* Process SAF frame commands */
    if ((sigs & VS_CMX_SAF_FRAME_SIG) != 0 )
    {
      /* Clear the SAF frame signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_CMX_SAF_FRAME_SIG);
      cmx_saf_frame();
    }
#endif /* FEATURE_SAF */

    /* Process the queued CMX commands
    */
    if ((sigs & VS_CMX_CMD_Q_SIG) != 0 )
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_CMX_CMD_Q_SIG);
      cmx_cmd_parser ();
    }

#endif /* FEATURE_CMX */

#ifdef FEATURE_VIDEO_ENCODE
    /* Process the queued video engine commands
    */
    if ((sigs & VS_VIDEO_ENG_CMD_Q_SIG) != 0 )
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_VIDEO_ENG_CMD_Q_SIG);
      video_eng_cmd_parser ();
    }
#endif /* FEATURE_VIDEO_ENCODE */

    /*---------------------------------------------------------------------*/

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
    /* Voice playback is done
    */
    if ((sigs & VS_VP_DONE_SIG) != 0) 
    {
      MSG_HIGH ("VS-Signal:  Finish playing voice prompts",0,0,0);
      /* Clear the signal.
      */
      (void) rex_clr_sigs (&vs_tcb, VS_VP_DONE_SIG);

      /* Free all the buffers.
      */
      vs_play_free_all_buf ();
 
      /* Indicate nothing is in progress (no play prompt, multi prompt).
      */
      pb_cmd_in_progress = VS_PLAY_NONE;

#ifdef FEATURE_VOICE_MEMO
      /* Clear flag to call back after each utterance for Voice Memo.
      */
      callback_vm_inprogress = FALSE;
#endif /* FEATURE_VOICE_MEMO */

      /* Call back if function is not NULL.
      */
      if (vs_play_cb != (vs_play_cb_type) NULL)
      {
        vs_play_cb (VS_SUCCESS);
        
        /* Play command in progress was cleared earlier in this function, but 
        ** the play call back may issue another play command.  So make sure no
        ** play command is in progress before clearing the callback function 
        ** pointer.
        */
        if (pb_cmd_in_progress == VS_PLAY_NONE)
        {
          vs_play_cb = (vs_play_cb_type) NULL;
        }
      }
    }
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

    /*---------------------------------------------------------------------*/

    /* We're supposed to shutdown.
    */
    if ((sigs & TASK_STOP_SIG) != 0) 
    {
      vs_task_shutdown ();
    }

    /*---------------------------------------------------------------------*/

    /* We're supposed to go offline.
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0) 
    {
      vs_task_offline ();
    }

    /*---------------------------------------------------------------------*/

#ifdef VS_TEST_CODE
#error code not present
#endif /* VS_TEST_CODE */

  } /* end of infinite loop */
} /* end vs_task */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_task_init

DESCRIPTION
  This procedure initializes the VS task.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vs_tcb
  VS command queue and free queue.

===========================================================================*/
LOCAL void vs_task_init (void)
{
#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO) || \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined(FEATURE_VIDEO_ENCODE_FADING))
  int i;
#endif

  /* Initialize timers
  */
  rex_def_timer (&vs_rpt_timer, &vs_tcb, VS_RPT_TIMER_SIG);
 
#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO)|| \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined(FEATURE_VIDEO_ENCODE_FADING))
  /* Initialize the command queues.
  */
  (void) q_init (&vs_cmd_q);              /* Initialize the command queue  */
  (void) q_init (&vs_free_q);             /* Initialize the free queue     */

  /* Add items to vs_free_q.
  */
  for (i = 0; i < VS_NUM_PACKETS; i++) 
  {
    q_link_type *link_item;
    link_item = q_link( &vs_free_packet[i], &vs_free_packet[i].hdr.link );
    q_put( &vs_free_q, link_item );
  }
#endif

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
  /* Initialize the voice play back queues.
  */
  vs_play_init_queue ();
#endif

#ifdef FEATURE_PHONE_VR
  /* Initialize voc acquire bail-out timer
  */
  rex_def_timer (&vs_voc_acquire_timer, &vs_tcb, VS_VOC_INFORM_SIG);

#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO 
  /* Initialize Voice Memo.
  */
  vm_init();
#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_CMX
  cmx_init();
#endif

#ifdef FEATURE_NEW_SLEEP_API  
  vs_sleep_handle = sleep_register(vs_sleep_str, TRUE);
#endif /* FEATURE_NEW_SLEEP_API  */

  MSG_LOW("VS task initialized",0,0,0);

} /* end vs_task_init */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the VS task.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vs_tcb

===========================================================================*/
LOCAL void vs_task_offline (void)
{

  /* Process task offline procedure from task controller.
  */
  (void) task_offline();

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs (&vs_tcb, TASK_OFFLINE_SIG);

} /* end vs_task_offline */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the VS task.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vs_tcb

===========================================================================*/
LOCAL void vs_task_shutdown (void)
{

  /* Process task stop procedure from task controller.
  */
  (void) task_stop();

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs (&vs_tcb, TASK_STOP_SIG);

} /* end vs_task_shutdown */

/* <EJECT> */
#if ((defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK)) || \
     defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO) || \
     defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
     defined(FEATURE_VIDEO_ENCODE_FADING))
/*===========================================================================

FUNCTION vs_queue_cmd

DESCRIPTION
  Queue a command for processing by the Voice Services (VS) Task.
  Handles synchronous functions also... right now its only used by VR
  abort functions... eventually should be used by all VS functions for VR and
  VM to provide synchronous capabilities

DEPENDENCIES
  Calls vs_cmd

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void vs_queue_cmd (
  vs_packet_type *vs_cmd_ptr
    /* pointer to VS command */
  )
{
  switch (vs_cmd_ptr->hdr.cmd)
  {
    /* Synchronous invocation -- clear the signal, issue the command, */
    /* then wait for the response.                                    */
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
    case VS_PLAYBACK_ABORT:
      if (vs_cmd_ptr->playabort.cb == NULL) {
        (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
        vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
        vs_cmd_ptr->hdr.tcb_ptr = rex_self();
        vs_cmd(vs_cmd_ptr);
        (void) rex_wait(VS_OP_COMPLETE_SIG);
      }
      else {
        vs_cmd (vs_cmd_ptr);
      }
      break;
#endif
#ifdef FEATURE_PHONE_VR
    case VS_TRAIN_ABORT:
      if (vs_cmd_ptr->trainabort.cb == NULL) {
        (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
        vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
        vs_cmd_ptr->hdr.tcb_ptr = rex_self();
        vs_cmd(vs_cmd_ptr);
        (void) rex_wait(VS_OP_COMPLETE_SIG);
      }
      else {
        vs_cmd (vs_cmd_ptr);
      }
      break;
    case VS_RECOGNIZE_ABORT:
      if (vs_cmd_ptr->recogabort.cb == NULL) {
        (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
        vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
        vs_cmd_ptr->hdr.tcb_ptr = rex_self();
        vs_cmd(vs_cmd_ptr);
        (void) rex_wait(VS_OP_COMPLETE_SIG);
      }
      else {
        vs_cmd (vs_cmd_ptr);
      }
      break;
    case VS_VOICE_ACTIVATE_ABORT:
      if (vs_cmd_ptr->vactivateabort.cb == NULL) {
        (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
        vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
        vs_cmd_ptr->hdr.tcb_ptr = rex_self();
        vs_cmd(vs_cmd_ptr);
        (void) rex_wait(VS_OP_COMPLETE_SIG);
      }
      else {
        vs_cmd (vs_cmd_ptr);
      }
      break;
    case VS_MENU_SELECTION_ABORT:
      if (vs_cmd_ptr->menuselabort.cb == NULL) {
        (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
        vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
        vs_cmd_ptr->hdr.tcb_ptr = rex_self();
        vs_cmd(vs_cmd_ptr);
        (void) rex_wait(VS_OP_COMPLETE_SIG);
      }
      else {
        vs_cmd (vs_cmd_ptr);
      }
      break;
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO
    case VS_VM_REC_SYNCH_STOP:
    case VS_VM_PLAY_SYNCH_STOP:
      (void) rex_clr_sigs(rex_self(),VS_OP_COMPLETE_SIG);
      vs_cmd_ptr->hdr.sigs = VS_OP_COMPLETE_SIG;  /* With this signal */
      vs_cmd_ptr->hdr.tcb_ptr = rex_self();
      vs_cmd(vs_cmd_ptr);
      (void) rex_wait(VS_OP_COMPLETE_SIG);
      break;
#endif /* FEATURE_VOICE_MEMO */
    default:
      vs_cmd(vs_cmd_ptr);
      break;
  }
}
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
      defined (FEATURE_VIDEO_ENCODE_FADING)
/*===========================================================================

FUNCTION      VS_QUEUE_CAMERA_CMD

DESCRIPTION
              Queues camera command on to VS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int vs_queue_images_ready_camera_cmd(void* buffer, int32 count)
{   vs_packet_type      *vspacket;
  if ((vspacket = vs_cmd_get_pkt()) != NULL) {
    vspacket->hdr.cmd = VS_CAMERA_CMD;
    vspacket->camerapkt.msg = VS_CAMERA_IMAGES_READY;
    vspacket->camerapkt.buf = buffer;
    vspacket->camerapkt.size = count;
    vs_queue_cmd(vspacket);

    return 0;
  }
  return 1;
}

#endif /* nT_IO_CARD */


/*===========================================================================

FUNCTION vs_cmd

DESCRIPTION
  Queue a command for processing by the Voice Services (VS) Task.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  vs_task_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  vs_cmd_q
  vs_tcb

===========================================================================*/
void vs_cmd
(
  vs_packet_type *vs_cmd_ptr
    /* pointer to VS command */
)
{
  (void) q_link (vs_cmd_ptr, &vs_cmd_ptr->hdr.link);    /* Init link field */

  q_put (&vs_cmd_q, &vs_cmd_ptr->hdr.link);       /* Put on command queue  */
  (void) rex_set_sigs (&vs_tcb, VS_CMD_Q_SIG);    /* Signal a queue event  */

} /* end vs_cmd */
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined FEATURE_VIDEO_ENCODE_FADING  || \
    defined FEATURE_VIDEO_ENCODE_RAW_ENGINE	
/*===========================================================================

FUNCTION vs_camera_cmd_processing

DESCRIPTION
  Processes a camera command in VS task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void vs_camera_cmd_processing(vs_camera_cmd_type msg, void *buf, uint32 size)
{
 switch(msg)
 { case VS_CAMERA_IMAGES_READY:

#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined (FEATURE_VIDEO_ENCODE_FADING)
  #if defined(FEATURE_VIDEO_ENCODE_VERSION) && (FEATURE_VIDEO_ENCODE_VERSION < 40)
     video_enc_effects_apply(buf, size);
  #endif
#endif

     break;
#ifdef FEATURE_VIDEO_ENCODE_RAW_ENGINE
   case VS_RAW_VIDEO_ENCODE_DONE:
     video_enc_raw_handle_video_encode_done(buf);
     break;
#endif
   default:
    ERR_FATAL ("Illegal camera command %x", msg, 0, 0);
    break;
 }
}
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION vs_cmd_get_pkt

DESCRIPTION
  Get a VS Command Packet.  Use by callers of vs_cmd().
  
DEPENDENCIES
  FEATURE_VS_TASK is defined.
  vs_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available vs_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  vs_free_q (and the packet fetched off vs_free_q)

===========================================================================*/
vs_packet_type *vs_cmd_get_pkt (void)
{
  vs_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (vs_packet_type *) (q_get (&vs_free_q));

  if (packet == NULL) 
  {
    ERR ("Ran Out of VS Packets!", 0, 0, 0);
  } 
  else 
  {
    /* Fill the entire VS packet with 0's to get default behavior on 
    ** un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (vs_packet_type));
    packet->hdr.cmd        = VS_INVALID;        /* Invalid Command Value   */
  }

  return (packet);

} /* end vs_cmd_get_pkt */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_cmd_parser

DESCRIPTION
  This procedure processes requests received by vs_cmd().  See vs.h for
  a description of the actual VS packets.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  vs_task_init() must have been called to init the command queue.

RETURN VALUE
  None

SIDE EFFECTS
  vs_cmd_q

===========================================================================*/
LOCAL void vs_cmd_parser (void)
{
  vs_packet_type *cmd_ptr;
    /* pointer to VS command packet */
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
  vs_cmd_error_type  ret = VS_CMD_NOERR;
    /* report via callback function if request is rejected */
#endif

  /* While there are commands to process, process each command.
  */
  while ((cmd_ptr = (vs_packet_type *) q_get (&vs_cmd_q)) != NULL) {

    /* Process each command type.
    */
    switch (cmd_ptr->hdr.cmd) 
    {

#ifdef FEATURE_PHONE_VR
      case VS_ENDPT_DETECT:
        /* Perform VR endpoint detection.
        */
        vs_endpoint_processing();
        break;

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
      case VS_RECOGNIZE_UTT:
        vs_recognize_utt_processing
                                  (cmd_ptr->recog.play_list, 
                                   cmd_ptr->recog.list_ptr, 
                                   cmd_ptr->recog.n_best,
                                   cmd_ptr->recog.match_cnt, 
                                   cmd_ptr->recog.cb);
        break;

      case VS_TRAIN_UTT:
        vs_train_utt_processing
                                  (cmd_ptr->train.play_list,
                                   cmd_ptr->train.echo_utt,
                                   cmd_ptr->train.control_word,
                                   cmd_ptr->train.list_ptr,
                                   cmd_ptr->train.cb);
        break;
    
    case VS_RECOGNIZE_MULTI:
      vs_recognize_multi_processing
                                (cmd_ptr->recog_multi.play_list, 
                                 cmd_ptr->recog_multi.list_ptr, 
                                 cmd_ptr->recog_multi.list_prompt_ptr,
                                 cmd_ptr->recog_multi.gap_mode,
                                 cmd_ptr->recog_multi.cb);
      break;
#endif /* FEATURE_PHONE_VR */

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
      /* Start single voice playback command
      */
      case VS_PLAYBACK:
        ret =  vs_play_processing (cmd_ptr->play.ut_id, 
                                   cmd_ptr->play.ut_index,
                                   cmd_ptr->play.play_direction,
                                   cmd_ptr->play.cb);
        if (ret == VS_CMD_ERROR)
        {
          /* Call back if there is no outstanding play voice prompt request.
          */
          if ((pb_cmd_in_progress == VS_PLAY_NONE) &&
              (cmd_ptr->play.cb != (vs_play_cb_type) NULL))
          {
            cmd_ptr->play.cb (VS_FAILURE);
          }
        } 
        break;

      /* Start multi voice playback command
      */
      case VS_PLAY_MULTI:
        ret =  vs_play_multi_processing (cmd_ptr->playmulti.mlist_ptr,
                                         cmd_ptr->playmulti.play_direction,
                                         cmd_ptr->playmulti.cb);
        if (ret == VS_CMD_ERROR)
        {
          /* Call back if there is no outstanding play voice prompt request.
          */
          if ((pb_cmd_in_progress == VS_PLAY_NONE) &&
              (cmd_ptr->playmulti.cb != (vs_play_cb_type) NULL))
          {
            cmd_ptr->playmulti.cb (VS_FAILURE);
          }
        }  
        break;

      /* Abort voice playback command
      */
      case VS_PLAYBACK_ABORT:
        vs_play_abort_processing (cmd_ptr->playabort.cb);
        break;

      /* End voice playback command, reclaim RAM buffer
      */
      case VS_PLAYBACK_END:
        /* Play multi buffer pointers may be NULL
                */
                if (cmd_ptr->playend.buf_ptr != NULL) {
          (void) vs_play_free_buf_immediate (cmd_ptr->playend.buf_ptr);
        }

#ifdef FEATURE_VOICE_MEMO
        /* If a voice memo is in progress, call back to report after 
        ** playing each voice utterance.
        */
        if (callback_vm_inprogress == TRUE) 
        {
          if (vs_play_cb != (vs_play_cb_type) NULL)
          {
            if (cmd_ptr->playend.status == VS_DIFF_SO) 
            {
              MSG_HIGH("Un-playable memo wrong SO",0,0,0);
              vs_play_cb (VS_DIFF_SO);
            }
            else if (cmd_ptr->playend.status == VS_BAD_VP) 
            {
              vs_play_cb (VS_FAILURE);
            }
            else
            {
              vs_play_cb (VS_INPROGRESS);
            }
          }
        }
        else
#endif /* FEATURE_VOICE_MEMO */
        {
          /* Report status for all play commands
          */
          if (vs_play_cb != (vs_play_cb_type) NULL)
          {
            vs_play_cb (cmd_ptr->playend.status);
          }
        }
        break;
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR

      case VS_CAPTURE_END:

        /* Capture is done.
        */
        vs_capture_end_processing (cmd_ptr->captureend.status);
        break;


      case VS_VOICE_ACTIVATE:
        vs_vactivate_processing(cmd_ptr->vactivate.play_list, 
                                cmd_ptr->vactivate.rwset_id, 
                                cmd_ptr->vactivate.rwid, 
                                cmd_ptr->vactivate.cb);
        break;
        
      case VS_IMPLICIT_YES:
        vs_implicit_yes_processing(cmd_ptr->recog.play_list, 
                                                      cmd_ptr->recog.cb);
                                   
        break;

      case VS_MENU_SELECTION:
        vs_mselection_processing(cmd_ptr->menusel.play_list, 
                                 cmd_ptr->menusel.list_ptr, 
                                 cmd_ptr->menusel.include, 
                                 cmd_ptr->menusel.order, 
                                 cmd_ptr->menusel.cb);
        break;                  
      
      case VS_TRAIN_ABORT:
        vs_train_abort_processing (cmd_ptr->trainabort.cb);
        break;

      case VS_RECOGNIZE_ABORT:
        vs_recognize_abort_processing (cmd_ptr->recogabort.cb);
        break;

      case VS_VOICE_ACTIVATE_ABORT:
        vs_vactivate_abort_processing (cmd_ptr->vactivateabort.cb);
        break;
      
      case VS_MENU_SELECTION_ABORT:
        vs_menu_selection_abort_processing (cmd_ptr->menusel.cb);
        break;
      
        break;
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO
      case VS_VM_REC_START:       /* Voice Memo Record start                         */
        vs_vm_record_start_processing (cmd_ptr->vmrecstart.promptlist, 
           cmd_ptr->vmrecstart.rec_direction, 
           cmd_ptr->vmrecstart.cb);
        break;
      case VS_VM_REC_STOP:
        vs_vm_record_stop_processing (cmd_ptr->vsvmvmidonly.vmid, FALSE);
        break;
      case VS_VM_REC_SYNCH_STOP:
        vs_vm_record_stop_processing (cmd_ptr->vsvmvmidonly.vmid, TRUE);
        break;
      case VS_VM_REC_PAUSE:
        vs_vm_record_pause_processing (cmd_ptr->vsvmvmidonly.vmid);
        break;
      case VS_VM_REC_RESUME:
        vs_vm_record_resume_processing (cmd_ptr->vsvmvmidonly.vmid);
        break;
      case VS_VM_REC_CANCEL:
        vs_vm_record_cancel_processing (cmd_ptr->vsvmvmidonly.vmid);
        break;
      case VS_VM_GET_PARAM:
        vs_vm_get_parameters_processing (cmd_ptr->vsvmgetparam.config_data_ptr,
                cmd_ptr->vsvmgetparam.callback_ptr);
        break;
      case VS_VM_SET_PARAM:
        vs_vm_set_parameters_processing (cmd_ptr->vsvmsetparam.config_data_ptr,
                cmd_ptr->vsvmsetparam.callback_ptr);
        break;
      case VS_VM_PLAY_START:
        vs_vm_play_start_processing (cmd_ptr->vsvmplaystart.vmid,
                 cmd_ptr->vsvmplaystart.play_rpt_interval_ms,
                 cmd_ptr->vsvmplaystart.play_direction,
                 cmd_ptr->vsvmplaystart.callback_ptr);
        break;
      case VS_VM_PLAY_STOP:
        vs_vm_play_stop_processing (cmd_ptr->vsvmvmidonly.vmid, FALSE);
        break;
      case VS_VM_PLAY_SYNCH_STOP:
        vs_vm_play_stop_processing (cmd_ptr->vsvmvmidonly.vmid, TRUE);
        break;
      case VS_VM_PLAY_PAUSE:
        vs_vm_play_pause_processing (cmd_ptr->vsvmvmidonly.vmid);
        break;
      case VS_VM_PLAY_RESUME:
        vs_vm_play_resume_processing (cmd_ptr->vsvmvmidonly.vmid);
        break;
      case VS_VM_PLAY_REWIND:
        vs_vm_play_rewind_processing (cmd_ptr->vsvmrewff.vmid,
                cmd_ptr->vsvmrewff.seconds);
        break;
      case VS_VM_PLAY_FASTFORWARD:
        vs_vm_play_fastforward_processing (cmd_ptr->vsvmrewff.vmid,
                cmd_ptr->vsvmrewff.seconds);
        break;
      case VS_VM_REC_CMX_CB:
        vm_rec_cb_from_cmx_processing (cmd_ptr->cmx_cb.status);
        break;
      case VS_VM_PLAY_CMX_CB:
        vm_play_cb_from_cmx_processing (cmd_ptr->cmx_cb.status);
        break;
#endif /* FEATURE_VOICE_MEMO */
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined (FEATURE_VIDEO_ENCODE_FADING)
      case VS_CAMERA_CMD:
        vs_camera_cmd_processing(cmd_ptr->camerapkt.msg, 
                                 cmd_ptr->camerapkt.buf,
                                 cmd_ptr->camerapkt.size);
       break;
#endif
      /* Illegal command, does not return.
      */
      case VS_INVALID:
      default:
        ERR_FATAL ("Illegal vs command %x", cmd_ptr->hdr.cmd, 0, 0);
    }

    if (cmd_ptr->hdr.sigs == VS_OP_COMPLETE_SIG) {
      rex_set_sigs (cmd_ptr->hdr.tcb_ptr, VS_OP_COMPLETE_SIG);
    }

    /* Return VS packet to free q
    */
    q_put (&vs_free_q, &cmd_ptr->hdr.link);

  } /* end of while "commands to process" */

} /* end vs_cmd_parser */
#endif /* ((defined(FEATURE_VOICE_DATABASE) && 
            defined (FEATURE_VOICE_PLAYBACK)) || 
           defined(FEATURE_PHONE_VR) || defined (FEATURE_VOICE_MEMO)) */

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION vs_voc_inform

DESCRIPTION
  Receive state changes from vocoder.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void vs_voc_inform ( voc_inform_type status )
{
  if (status == VOC_INFORM_VOCODER) {
    /*  The vocoder is ready
    */
    if(voc_get_curr_config() == VOC_CAP_IS733) {
      (void) rex_set_sigs( &vs_tcb, VS_VOC_INFORM_SIG );
      MSG_HIGH("VR got vocoder ready",0,0,0);
    }
  } else {
    (void) rex_clr_sigs (&vs_tcb, VS_VOC_INFORM_SIG);
  }
} /* vs_voc_inform */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_voc_acquire

DESCRIPTION
  This function acquires or releases the vocoder for voice recognition
  functions.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
vs_status_type vs_voc_acquire( boolean acquire )
{
  static boolean voc_acquired = FALSE;  
  vs_status_type ret_val      = VS_SUCCESS;

  if ((acquire == TRUE) && (voc_acquired == FALSE)) {
    /* Enable Bailout timer */
    (void) rex_set_timer (&vs_voc_acquire_timer, VS_VOC_ACQUIRE_TIME);

    MSG_HIGH ("About to voc_set_vr_mode to mode B/C",0,0,0);
    voc_set_vr_mode (VOC_ACQ_VS,VOC_VR_MODE_BC);
    voc_acquire (VOC_ACQ_VS, vs_voc_inform);
    voc_config(VOC_ACQ_VS, VOC_CAP_IS733);
    voc_acquired = TRUE;
    
    /* Wait for the vocoder to be ready
    */
    (void) rex_wait (VS_VOC_INFORM_SIG);
    (void) rex_clr_sigs (&vs_tcb, VS_VOC_INFORM_SIG);
    if(rex_get_timer(&vs_voc_acquire_timer) == 0) {
      ret_val = VS_FAILURE;
    }
    (void) rex_clr_timer (&vs_voc_acquire_timer);
  }

  if ((acquire == FALSE) && (voc_acquired == TRUE)) {
    MSG_HIGH ("About to voc_set_vr_mode to mode off",0,0,0);
    voc_set_vr_mode(VOC_ACQ_VS,VOC_VR_MODE_OFF);
    voc_release (VOC_ACQ_VS);
    voc_acquired = FALSE;
    (void) rex_clr_sigs (&vs_tcb, VS_VOC_INFORM_SIG);
    (void) rex_clr_timer (&vs_voc_acquire_timer);
  }

  return(ret_val);

} /* vs_voc_acquire */
#endif /* FEATURE_PHONE_VR */


/* <EJECT> */
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
/*===========================================================================

FUNCTION vs_play_utterance

DESCRIPTION
  This function commands the Voice Services task to play the specified 
  utterance.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  This function must not be called until vs_task_init () has been called.

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
void vs_play_utterance 
(
  vdb_rwid_type     utterance_id,
  short             utterance_index,
  vs_snd_direction_type   play_direction,
  vs_play_cb_type   cb_func
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task to start voice playback
  */
  packet = vs_cmd_get_pkt ();
  
  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_func != (vs_play_cb_type) NULL)
    {
      cb_func (VS_FAILURE);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_PLAYBACK;
    packet->play.ut_id               = utterance_id;
    packet->play.ut_index            = utterance_index;
    packet->play.play_direction      = play_direction;
    packet->play.cb                  = cb_func;
    vs_cmd (packet);
  }
} /* end vs_play_utterance */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_processing

DESCRIPTION
  This function initiates voice playback.  This function will reject the 
  request if a PLAY MULTI request is playing.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  Error code indicating success or failure of the function  

SIDE EFFECTS
  Queue a request in the Sound command queue

===========================================================================*/
LOCAL vs_cmd_error_type vs_play_processing 
(
  vdb_rwid_type         ut2play_id,
  short                 ut2play_index,
  vs_snd_direction_type play_direction,
  vs_play_cb_type       cb_func
)
{
  voc_pb_header_type *pb_buf_ptr = NULL;
  vs_cmd_error_type  ret = VS_CMD_NOERR;

  /* If a play multi command is in progress, reject the request. 
  */
  if (pb_cmd_in_progress == VS_PLAY_MULTI_P)
  {                     
    MSG_HIGH ("Reject play request", 0, 0, 0);
    ret = VS_CMD_ERROR;
  }
  else
  {
    /* Be sure to find a free RAM buffer here and reserve it for this
    ** voice prompt.  If no buffer is available then reject the request.
    */
    pb_buf_ptr = vs_play_get_buf ();
    
    if (pb_buf_ptr == NULL)
    {
      ret = VS_CMD_ERROR;
    }
    else
    {
      /* Got a RAM buffer ready.  Now get ready to read voice data from 
      ** flash via the Voice Database manager.
      */
      if (vs_play_get_utt (ut2play_id, ut2play_index, pb_buf_ptr) != 
          VS_RET_OK)
      {
        ret = VS_CMD_ERROR;
      }
      else
      {
        /* Set play command in progress flag to the right value.
        */
        pb_cmd_in_progress = VS_PLAY_SINGLE_P;
         
        /* Save the call back function.
        */
        vs_play_cb = cb_func;
        vs_play_status = VS_SUCCESS;

        /* Specify the direction of play.
        */
        if (play_direction == VS_SND_REVERSE)
        {
          pb_buf_ptr->pb_direction = VOC_PB_DIR_REVERSE;  
        }
        else if (play_direction == VS_SND_FORWARD)
        {
          pb_buf_ptr->pb_direction = VOC_PB_DIR_FORWARD;  
        }
        else 
        {
          pb_buf_ptr->pb_direction = VOC_PB_DIR_BOTH;  
        }

        /* Command SND to start voice utterance play back.
        */
        snd_vp_start(SND_DEVICE_CURRENT, pb_buf_ptr, vs_play_end_cb,
                                        (void *)pb_buf_ptr);
    
      } /* end if (vs_play_get_utt (id, index, pb_buf_ptr) != VS_RET_OK) */
    } /* end if (pb_buf_ptr == NULL) */
  } /* end if (pb_cmd_in_progress == VS_PLAY_MULTI_P) */
    
  /* If error and a buffer was reserved, free the buffer since it wasn't used.
  */
  if ((ret == VS_CMD_ERROR) && (pb_buf_ptr != NULL))
  {
    /* Return unused buffer.
    */
    (void) vs_play_free_buf_immediate (pb_buf_ptr);
  }
  return (ret); 

} /* end vs_play_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_multi

DESCRIPTION
  This function commands the Voice Services task to play the specified 
  play list.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  This function must not be called until vs_task_init () has been called.

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
void vs_play_multi 
(
  vs_sound_type               *multi_list,
  vs_snd_direction_type       play_direction,
  vs_play_cb_type             cb_func
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task to start voice play multi.
  */
  packet = vs_cmd_get_pkt ();
  
  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_func != (vs_play_cb_type) NULL)
    {
      cb_func (VS_FAILURE);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_PLAY_MULTI;
    packet->playmulti.mlist_ptr      = multi_list;
    packet->playmulti.play_direction = play_direction;
    packet->playmulti.cb             = cb_func;
    vs_cmd (packet);
  }
} /* end vs_play_multi */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_multi_processing

DESCRIPTION
  This function initiates multi voice playback.  This function will reject the
  request if:
  
    - Either SINGLE or MULTI request is playing
    - The multi list has an invalid RWID
    - The multi list has more than MAX_NUM_MULTI entries

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  Error code indicating success or failure of the function  

SIDE EFFECTS
  Queue a request in the Sound command queue

===========================================================================*/
vs_cmd_error_type vs_play_multi_processing 
(
  vs_sound_type         *multi_list_ptr,
  vs_snd_direction_type play_direction,
  vs_play_cb_type       cb_func
)
{
  unsigned short     i, count = 0;
  boolean            match = FALSE;
  vs_cmd_error_type  ret = VS_CMD_NOERR;
  voc_pb_dir_type    play_dir = VOC_PB_DIR_FORWARD;

  snd_compact_tone_type  *s_multi_ptr = s_multi_array; 

  static snd_tone_and_voice_sound_type sound_buf   = { SND_TONE_AND_VOICE,
                                                       s_multi_array,
                                                       NULL };


  /* If either SINGLE or MULTI request is playing, reject the request.
  */
  if ((pb_cmd_in_progress != VS_PLAY_NONE) ||
      (multi_list_ptr == NULL))
  {                     
    vs_play_abort_processing ((vs_play_cb_type) NULL);
    MSG_HIGH ("Play multi request Reset", 0, 0, 0);
  }
  /* Specify the direction of play.
  */
  if (play_direction == VS_SND_REVERSE)
  {
    play_dir = VOC_PB_DIR_REVERSE;  
  }
  else if (play_direction == VS_SND_FORWARD)
  {
    play_dir = VOC_PB_DIR_FORWARD;  
  }
  else 
  {
    play_dir = VOC_PB_DIR_BOTH;  
  }

  /* Initialize the rwid "match" array with 0 (an invalid RWID).
  */
  for (i = 0; i < VS_PB_NUM_BUFFERS; i++)
  {
    rwid_array [i] = 0;
  }
  
  /* Go through all entries in the list.  If the entry is not a voice prompt,
  ** just copy it to s_multi_array (for SND).  If the entry is a 
  ** voice prompt, need to do additional processing.
  */
  while (multi_list_ptr->sound != SND_STOP)
  {
    /* Haven't found a match yet.
    */
    match = FALSE;
  
    /* Copy tone to s_multi_array.  Can't use the input list because
    ** there are things there that SND does not use (like rwid or utt
    ** index.
    */
    s_multi_ptr->tone = multi_list_ptr->sound;      
  
    /* Copy duration to s_multi_array if tone is not SND_VOICE_PROMPT.
    */
    if (multi_list_ptr->sound != SND_VOICE_PROMPT)
    {
      s_multi_ptr->param.duration_ms = multi_list_ptr->duration;      
    }
  
    /* This is a voice prompt.  First, find out how many RWIDs we need to 
    ** read from flash.  If the multi list contains duplicate RWIDs, e.g. 
    ** the string "2,2,4,4" has two RWIDs "2" and "4", only read the "2" 
    ** and "4" from flash.
    */
    else
    {
      /* Reject request if the multi list contains a RWID of 0.
      */
      if (multi_list_ptr->utt_id == 0)
      {
        ret = VS_CMD_ERROR;
        break;
      }

      /* For every RWID, look to see if it is already in rwid_array. 
      */
      for (i = 0; i < count; i++)
      {
        if (multi_list_ptr->utt_id == rwid_array [i])
        {
          match = TRUE;
  
          /* Converts RWID to point to the right prompt for SND.
          */
          s_multi_ptr->param.duration_ms = i;      
          break;
        }
      }
      /* If it's not already there, add it.
      */
      if (match == FALSE)
      {
        rwid_array [count] = multi_list_ptr->utt_id;
  
        /* Be sure to find a free RAM buffer here and reserve it.
        */
        pb_hdr_array [count] = vs_play_get_buf ();
        if (pb_hdr_array [count] == NULL)
        {
          ret = VS_CMD_ERROR;
          break;
        }
        else
        {
          /* Set direction of playback.
          */
          pb_hdr_array [count]->pb_direction = play_dir;  
      
          /* Got a RAM buffer ready.  Now read voice data from 
          ** flash via the Voice Database manager.
          */
          if (vs_play_get_utt (multi_list_ptr->utt_id, 
                               multi_list_ptr->utt_index, 
                               pb_hdr_array [count]) != VS_RET_OK)
          {      
            ret = VS_CMD_ERROR;
            break;
          }
        }
        /* Converts RWID to point to the right prompt for SND.
        */
        s_multi_ptr->param.duration_ms = count;      
        count++;
  
      } /* end if (match == FALSE) */
    } /* end if (multi_list_ptr->tone == SND_VOICE_PROMPT) */
  
    multi_list_ptr++;     /* Go to next entry in input list. */
    s_multi_ptr++;  /* Go to next entry in output list. */
  
    /* If there are too many entries which will cause the output
    ** s_multi_array to overflow, reject the request.
    */
    if (s_multi_ptr > &(s_multi_array [MAX_NUM_MULTI - 1]))
    {
      ret = VS_CMD_ERROR;
      break;
    }
  
  } /* end while loop */
  
  /* If everything is OK, queues the request in the SND request queue.
  */
  if (ret == VS_CMD_NOERR)
  {
    /* Copy SND_STOP tone and duration to s_multi_array. 
    */
    /*lint -e796*/
    s_multi_ptr->tone = multi_list_ptr->sound;           
    /*lint -e796*/
    s_multi_ptr->param.duration_ms = multi_list_ptr->duration;      
  
    /* Set play command in progress flag to the right value.
    */
    pb_cmd_in_progress = VS_PLAY_MULTI_P;
     
    /* Save call back function
    */
    vs_play_cb = cb_func;
    vs_play_status = VS_SUCCESS;

    /* Command SND to start voice utterance play back.
    */
    sound_buf.prompt_list = pb_hdr_array;
    snd_sound_start( SND_DEVICE_CURRENT, SND_METHOD_VOICE,
                     (snd_sound_type *) &sound_buf, SND_PRIO_HIGH, 
                     SND_APATH_LOCAL, vs_play_end_cb, NULL);

  } /* end if (ret == VS_CMD_NOERR) */

  /* If error, free all the buffers in the in-use queue.  Do it here so
  ** if there is a current play request, those buffers won't be freed.
  */
  if (ret == VS_CMD_ERROR)
  {
    /* Free all the buffers.
    */
    vs_play_free_all_buf ();

  } /* end if (ret == VS_CMD_ERR) */
    
  return (ret); 

} /* end vs_play_multi_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_abort

DESCRIPTION
  This function aborts the current play back function in Voice Services (if
  any).

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
void vs_play_abort
(
  vs_play_cb_type   abort_callback_ptr
    /* Function to call when playback abort is complete. */
)

{
  vs_packet_type               *packet;

  /* Signal VS task to abort play
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_play_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_PLAYBACK_ABORT;
    packet->playabort.cb       = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_play_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_abort_processing

DESCRIPTION
  This function initiates the play back abort.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  Error code indicating success or failure of the function  

SIDE EFFECTS
  Queue a request in the Sound command queue

===========================================================================*/
void vs_play_abort_processing
(
  vs_play_cb_type   abort_callback_ptr
    /* Function to call when playback abort is complete. */
)

{

  /* If nothing is playing, reject the request.
  */
  if (pb_cmd_in_progress == VS_PLAY_NONE)
  {                     
    MSG_HIGH ("Reject abort play request", 0, 0, 0);

    /* Call back if function is not NULL.
    */
    if (abort_callback_ptr != (vs_play_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE);
    }
  }
  else
  {
    /* Stop either voice play back or voice multi.
    */
    if (pb_cmd_in_progress == VS_PLAY_MULTI_P)
    {
      VS_DEBUG_MSG ("Abort Play Multi request", 0, 0, 0);
      snd_sound_stop(SND_PRIO_HIGH, NULL, NULL);
    }
    else
    {
      VS_DEBUG_MSG ("Abort Play Voice Prompt request", 0, 0, 0);
      snd_vp_stop(NULL, NULL);
    }

     
    /* Wait for done signal from SND.
    */
    (void) rex_wait (VS_VP_DONE_SIG);
    MSG_HIGH ("VS-Signal:  Complete Abort Play request",0,0,0);
   
    /* Clear the signal.
    */
    (void) rex_clr_sigs (&vs_tcb, VS_VP_DONE_SIG);
   
    /* Free all the buffers.
    */
    vs_play_free_all_buf ();
   
    /* Indicate nothing is in progress (no play prompt, multi prompt).
    */
    pb_cmd_in_progress = VS_PLAY_NONE;

    /* Call back to ack the abort command.
    */
    if (abort_callback_ptr != (vs_play_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_SUCCESS);
    }
    /* Call back to ack the play command.
    */
    if (vs_play_cb != (vs_play_cb_type) NULL)
    {
      vs_play_cb (VS_ABORTED);
      
      /* Play command in progress was cleared earlier in this function, but 
      ** the play call back may issue another play command.  So make sure no
      ** play command is in progress before clearing the callback function 
      ** pointer.
      */
      if (pb_cmd_in_progress == VS_PLAY_NONE)
      {
        vs_play_cb = (vs_play_cb_type) NULL;
      }
    }

  } /* end if (pb_cmd_in_progress == VS_PLAY_NONE) */
} /* end vs_play_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_end_cb

DESCRIPTION
  At the end of playing back a voice utterance, the RAM buffer used is released
  in the VS task context.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
LOCAL void vs_play_end_cb
(
  const void *pb_ptr,
  /* Pointer to play back header    
  */
  snd_status_type status
)
{
  vs_packet_type               *packet;

  /* Signal VS task to end playback and reclaim RAM buffer
  */
  if(status == SND_PLAY_DONE) {
    (void) rex_set_sigs(&vs_tcb, VS_VP_DONE_SIG);
  } else if((status == SND_PROMPT_DONE) || (status == SND_VP_DIFF_SO) ||
            (status == SND_BAD_VP)) {

    packet = vs_cmd_get_pkt ();

    if (packet == NULL) 
    {
      /* VS task queue is full, call back with error code.
      */
      if (vs_play_cb != (vs_play_cb_type) NULL)
      {
        vs_play_cb (VS_FAILURE);
      }
    }
    else
    {
      packet->hdr.cmd                  = VS_PLAYBACK_END;
      packet->playend.buf_ptr          = (voc_pb_header_type *) pb_ptr;
      switch (status) {
        case SND_BAD_VP:
          packet->playend.status       = VS_BAD_VP;
          break;
        case SND_VP_DIFF_SO:
          packet->playend.status       = VS_DIFF_SO;
          break;
        case SND_PROMPT_DONE:
          packet->playend.status       = VS_SUCCESS;
          break;
      }
      vs_cmd (packet);
    }
  } else if((status == SND_FAILURE) || (status == SND_PROMPT_BUSY)) {
    /* VS task queue is full, call back with error code.
    */
    if (vs_play_cb != (vs_play_cb_type) NULL)
    {
      vs_play_cb (VS_FAILURE);
    }
  } else if ( status == SND_PROMPT_NONE ) {
    /* There are no sound prompts in queue, set vs cmd in progress to none.
	*/
	  pb_cmd_in_progress = VS_PLAY_NONE;
  } else if(status == SND_ABORT) {
    if (vs_play_cb != (vs_play_cb_type) NULL)
    {
#ifdef FEATURE_VOICE_MEMO 
      if (callback_vm_inprogress == TRUE) {
        vs_play_cb (VS_FAILURE);
      } else 
#endif /* FEATURE_VOICE_MEMO */
      {
        vs_play_cb (VS_ABORTED);
      }
      vs_play_cb = NULL;
  }

  }

} /* end vs_play_end_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_init_queue

DESCRIPTION
  This function initializes the queues used in voice play back.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vs_pb_free_q

===========================================================================*/
LOCAL void vs_play_init_queue (void)
{
  int i;

  /* Initialize the playback header array to point to the appropriate 
  ** playback data buffer.
  */
  for (i = 0; i < VS_PB_NUM_BUFFERS; i++) 
  {
    vs_pb_hdr [i].pb_data_ptr = &(vs_pb_free_buf [i][0]);
  }

  /* Initialize the play back queue.
  */
  (void) q_init (&vs_pb_free_q);   

  /* Add items to vs_pb_free_q.
  */
  for (i = 0; i < VS_PB_NUM_BUFFERS; i++) 
  {
    q_link_type *link_item;
    link_item = q_link (&vs_pb_hdr[i], &vs_pb_hdr[i].link );
    q_put( &vs_pb_free_q, link_item );
  }
} /* end vs_play_init_queue */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_get_buf

DESCRIPTION
  The play back queue (RAM) is used to stored voice data read from flash.
  Return pointer to an available entry.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  This function must not be called until vs_task_init () has been called.

RETURN VALUE
  Ptr to RAM buffer or NULL if no entry is available.

SIDE EFFECTS
  vs_pb_free_q

===========================================================================*/
voc_pb_header_type *vs_play_get_buf (void)
{
  voc_pb_header_type *ret_ptr;

  /* Get a buffer and then either report an error, or clear the buffer header
  ** out so the previous user's garbage doesn't do something unexpected.
  */
  ret_ptr = (voc_pb_header_type *) (q_get (&vs_pb_free_q));

  if (ret_ptr == NULL) 
  {
    MSG_HIGH ("Ran out of play buffer!", 0, 0, 0);
  } 
  else 
  {
    /* Initialize the header portion of the buffer.
    */
    ret_ptr->pb_num_frames = 0;
    ret_ptr->pb_voc_capability = VOC_CAP_IS733;
    ret_ptr->pb_len_rate_var = VOC_PB_FIX_LEN_FIX_RATE;
    ret_ptr->pb_rate = VOC_2_RATE;
    ret_ptr->pb_direction = VOC_PB_DIR_FORWARD;  
    
    VS_DEBUG_MSG ("Get play buffer addr = %x", ret_ptr, 0, 0);
  }
  return (ret_ptr);

} /* end vs_play_get_buf */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_free_all_buf

DESCRIPTION
  This function frees all buffers in the in-use queue.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  This function must not be called until vs_task_init () has been called.

RETURN VALUE
  vs_ret_type

SIDE EFFECTS
  vs_pb_free_q

===========================================================================*/
LOCAL void vs_play_free_all_buf (void)
{
  int i;
  
  VS_DEBUG_MSG ("Free all buffers",0,0,0);

  /* Initialize the play back queue.
  */
  (void) q_init (&vs_pb_free_q);   

  /* Add items to vs_pb_free_q.
  */
  for (i = 0; i < VS_PB_NUM_BUFFERS; i++) 
  {
    q_link_type *link_item;
    link_item = q_link (&vs_pb_hdr[i], &vs_pb_hdr[i].link );
    q_put( &vs_pb_free_q, link_item );
  }
} /* end vs_play_free_all_buf */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_free_buf_immediate

DESCRIPTION
  This function returns the given buffer to the free queue.  This function is 
  called if a buffer was reserved but never used due to some error condition.
  The returned buffer has not yet been queued in the in-use queue.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  This function must not be called until vs_task_init () has been called.

RETURN VALUE
  vs_ret_type

SIDE EFFECTS
  vs_pb_free_q

===========================================================================*/
vs_ret_type vs_play_free_buf_immediate (voc_pb_header_type *buf_ptr)
{

  VS_DEBUG_MSG ("Free buffer %x", buf_ptr, 0, 0);

  /* Return unused buffer to free queue. */
  q_put (&vs_pb_free_q, &buf_ptr->link);
  return (VS_RET_OK);

} /* end vs_play_free_buf_immediate */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_get_utt

DESCRIPTION
  This function reads utterance data from the voice database.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  vs_ret_type

SIDE EFFECTS
  None

===========================================================================*/
LOCAL vs_ret_type vs_play_get_utt
(
  vdb_rwid_type       ut2play_id,
  int                 ut2play_index,
  voc_pb_header_type  *pb_buf_ptr
)
{
  vdb_rw_type     utt;
  vdb_err_type    vdb_ret;  
  int4            utt_len = 0L;
  vs_ret_type     ret = VS_RET_OK;
  
  /* Prepare for flash file read.
  */
  vdb_ret = vdb_rw_init (&utt, ut2play_id);

  if (vdb_ret != VDB_NOERR)
  {
    MSG_HIGH ("Voice Database Error = %d", vdb_ret ,0 ,0);
    ret = VS_RET_ERR;
  }
  else
  {
    /* Make sure length of utterance is greater than 0 and does not exceed 
    ** length of RAM buffer.
    */
    utt_len = vdb_rwdata_utt_length (&utt.base_rwdata, ut2play_index);
    if ((utt_len >= (int4) VS_MAX_UTTERANCE_BYTES) || (utt_len <= 0L))
    {
      MSG_HIGH ("Utterance length exceeds RAM buffer", 0, 0, 0);
      ret = VS_RET_ERR;
    }
    else
    {
      /* Length is OK, now copy the voice utterance data to RAM buffer.
      */
      vdb_ret = vdb_rwdata_get_utt (&utt.base_rwdata, ut2play_index, 
                pb_buf_ptr->pb_data_ptr);
    
      if (vdb_ret != VDB_NOERR)
      {
        MSG_HIGH ("Voice Database Error = %d", vdb_ret ,0 ,0);
        ret = VS_RET_ERR;
      }
      else
      {
        /* Voice utterance is now in RAM.  Need to construct the voice
        ** utterance header to play back.
        */
        pb_buf_ptr->pb_num_frames = 
          (word) vdb_rwdata_utt_frames (&utt.base_rwdata, ut2play_index);
        if (pb_buf_ptr->pb_num_frames == 0)
        {
          MSG_HIGH ("Utterance contains 0 frame",0 ,0 ,0);
          ret = VS_RET_ERR;
        }
        else
        {
          vdb_ret = vdb_rwdata_utt_vocoder (&utt.base_rwdata,
            ut2play_index, &pb_buf_ptr->pb_voc_capability);
          if (vdb_ret != VDB_NOERR)
          {
            MSG_HIGH ("Voice Database Error = %d", vdb_ret, 0, 0);
            ret = VS_RET_ERR;
          }
          if (vdb_ret == VDB_NOERR)
          {
            vdb_ret = vdb_rwdata_utt_format (&utt.base_rwdata,
              ut2play_index, &pb_buf_ptr->pb_len_rate_var);
            if (vdb_ret != VDB_NOERR)
            {
              MSG_HIGH ("Voice Database Error = %d", vdb_ret, 0, 0);
              ret = VS_RET_ERR;
            }
          }
          if (vdb_ret == VDB_NOERR)
          {
            vdb_ret = vdb_rwdata_utt_rate (&utt.base_rwdata,
              ut2play_index, &pb_buf_ptr->pb_rate);
            if (vdb_ret != VDB_NOERR)
            {
              MSG_HIGH ("Voice Database Error = %d", vdb_ret, 0, 0);
              ret = VS_RET_ERR;
            }
          }
        } /* end if (pb_buf_ptr->pb_hdr.pb_num_frames == 0) */
      } /* end if (vdb_ret != VDB_NOERR) */
    } /* end if (utt_len is invalid) */

    /* Release flash file.
    */
    vdb_ret = vdb_rw_release (&utt);
    if (vdb_ret != VDB_NOERR)
    {
      MSG_HIGH ("Voice Database Error = %d", vdb_ret ,0 ,0);
      ret = VS_RET_ERR;
    }
  } /* end if (vdb_ret != VDB_NOERR) */

  return (ret);

} /* end vs_play_get_utt */
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION VS_HANDSHAKE_ENABLE

DESCRIPTION
  This function enables handshaking for synchronization on callbacks from
  Voice Services.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_handshake_enable
(
  vs_control_cb_type       callback_ptr 
    /* Function to call after handshaking is enabled. */
)

{
  /* Enable the use of handshaking on callback functions and send ack.
  */
  vs_handshake_enable_flag = TRUE;

  if (callback_ptr != (vs_control_cb_type) NULL)
  {
    callback_ptr (VS_SUCCESS);
  }

} /* end vs_handshake_enable */

/* <EJECT> */
/*===========================================================================

FUNCTION VS_HANDSHAKE_DISABLE

DESCRIPTION
  This function disables handshaking for synchronization on callbacks from
  Voice Services.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_handshake_disable
(
  vs_control_cb_type       callback_ptr
    /* Function to call after handshaking is disabled. */
)

{
  /* Disable the use of handshaking on callback functions and send ack.
  */
  vs_handshake_enable_flag = FALSE;

  if (callback_ptr != (vs_control_cb_type) NULL)
  {
    callback_ptr (VS_SUCCESS);
  }

} /* end vs_handshake_disable */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_handshake_query

DESCRIPTION
  Returns boolean value of handshake enable flag.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  Handshake enable flag

SIDE EFFECTS
  None

===========================================================================*/
boolean vs_handshake_query (void)
{
  return (vs_handshake_enable_flag);

} /* end vs_handshake_query */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_capture_end

DESCRIPTION
  This function is registered as a call back function to the VR Engine 
  Endpoint Detection.  This function is called when endpoint detection is 
  done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
void vs_capture_end (vs_status_type status)
{
  vs_packet_type               *packet;


  /* If the VR Endpoint Detector no longer wants to receive vocoder packets,
  ** the intermediate status VS_STOP_CAPTURE is sent to VS.  The capture flag
  ** is cleared to stop sending vocoder packets to the VR Endpoint Detector.  It
  ** is not neccessary to queue up a command for the VS task to run to process 
  ** this status.
  */
  if (status == VS_STOP_CAPTURE)
  {
    vs_endpoint_capture = FALSE;
    return;
  }
  
  /* Signal VS task to process end of capture
  */
  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
    packet->hdr.cmd           = VS_CAPTURE_END;
    packet->captureend.status = status;
    vs_cmd (packet);
  }
} /* end vs_capture_end */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_capture_end_processing

DESCRIPTION
  This function processes the capture end request in the Voice Services task 
  command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May release vocoder

===========================================================================*/
LOCAL void vs_capture_end_processing (vs_status_type status)
{
  /* Call VR function to handle capture end.
  */ 
  vr_capture_end (status);

} /* end vs_capture_end_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_temp_utterance

DESCRIPTION
  This function is called to play the utterance just captured.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  vs_cmd_error_type

SIDE EFFECTS
  Queue a request in the Sound command queue.

===========================================================================*/
vs_cmd_error_type vs_play_temp_utterance 
(
  vs_play_cb_type cb_func
)

{
  vs_cmd_error_type   ret = VS_CMD_NOERR;
  voc_pb_header_type  *temp_ut_ptr = (voc_pb_header_type *) NULL;

  /* If a play multi command is in progress, reject the request. 
  */
  if (pb_cmd_in_progress == VS_PLAY_MULTI_P)
  {                     
    MSG_HIGH ("Reject play temp utterance request", 0, 0, 0);
    ret = VS_CMD_ERROR;
  }
  else
  {
    /* Call VR function to get ptr to temp utterance header.
    */
    temp_ut_ptr = vr_play_temp_utterance ();

    if (temp_ut_ptr == (voc_pb_header_type *) NULL)
    {
      ret = VS_CMD_ERROR;
    }
    else
    {
      /* Set play command in progress flag to the right value.
      */
      pb_cmd_in_progress = VS_PLAY_SINGLE_P;
       
      /* Save the call back function.
      */
      vs_play_cb = cb_func;
      vs_play_status = VS_SUCCESS;

      /* Command SND to start voice utterance play back.
      */
      snd_vp_start(SND_DEVICE_CURRENT, temp_ut_ptr, vs_play_end_cb, 
                   (void *)temp_ut_ptr);

    } /* end if (pb_buf_ptr == NULL) */
  } /* end if (pb_cmd_in_progress == VS_PLAY_MULTI_P) */
    
  return (ret);

} /* end vs_play_temp_utterance */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_abort

DESCRIPTION
  This function aborts the current training process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/

void vs_train_abort 
(
  vs_train_cb_type         abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  vs_packet_type               *packet;

  /* Signal VS task to abort train
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_train_cb_type) NULL)
    {
      abort_callback_ptr (VS_FAILURE, VDB_RWID_NONE, NULL);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_TRAIN_ABORT;
    packet->trainabort.cb      = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_train_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_abort_processing

DESCRIPTION
  This function processes the train abort request in the Voice Services task 
  command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_train_abort_processing 
(
  vs_train_cb_type         abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{

  /* Make sure call back does not happen after aborting play back.  Call back
  ** only after aborting capture also.
  */
  vs_play_cb = (vs_play_cb_type) NULL;
  vs_play_abort_processing ((vs_play_cb_type) NULL);

  /* Call VR function to handle train abort.
  */ 
  vr_train_abort (abort_callback_ptr);

} /* end vs_train_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_abort

DESCRIPTION
  This function aborts the current recognition process. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/

void vs_recognize_abort 
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)
{
  vs_packet_type               *packet;

  /* Signal VS task to abort recognize
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_rec_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_RECOGNIZE_ABORT;
    packet->recogabort.cb      = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_recognize_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_multi_abort

DESCRIPTION
  This function aborts the current recognition process. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/

void vs_recognize_multi_abort 
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)
{
  vs_packet_type               *packet;

  /* Signal VS task to abort recognize
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_rec_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_RECOGNIZE_ABORT;
    packet->recogabort.cb      = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_recognize_multi_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_abort_processing

DESCRIPTION
  This function processes the recognize abort request in the Voice Services 
  task command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_recognize_abort_processing
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  
  /* Make sure call back does not happen after aborting play back.  Call back
  ** only after aborting capture also.
  */
  vs_play_cb = (vs_play_cb_type) NULL;
  vs_play_abort_processing ((vs_play_cb_type) NULL);

  /* Call VR function to handle recognize abort.
  */ 
  vr_recognize_abort (abort_callback_ptr);

} /* end vs_recognize_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_voice_activate

DESCRIPTION
  This function starts the voice activation routine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_voice_activate
(
  vs_sound_type**          prompt_list, /* List of prompts for activation   */
  vdb_rw_list_type*        rwset_id,    /* recognizable word set ID         */
  vdb_rwid_type            rwid,        /* recognizable activation word ID  */
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when done       */
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_rec_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_VOICE_ACTIVATE;
    packet->vactivate.play_list      = prompt_list;
    packet->vactivate.rwset_id       = rwset_id;
    packet->vactivate.rwid           = rwid;
    packet->vactivate.cb             = cb_back_ptr;
    vs_cmd (packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_processing

DESCRIPTION
  This function processes the voice activation command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_vactivate_processing 
(
  vs_sound_type**          prompt_list, /* List of prompts for activation  */
  vdb_rw_list_type*        rwset_id,    /* recognizable word set ID        */
  vdb_rwid_type            rwid,        /* recognizable activation word ID */
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when done      */
)
{
  /* initialize voice activation function with the appropriate data.
  */
  vr_voice_activate_init(prompt_list, rwset_id, rwid, cb_back_ptr);

  (void) vr_voice_activate ();

} /* end vs_vactivate_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_voice_activate_abort

DESCRIPTION
  This function aborts the current voice activation process. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/

void vs_voice_activate_abort
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  vs_packet_type               *packet;

  /* Signal VS task to abort voice activation
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_rec_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_VOICE_ACTIVATE_ABORT;
    packet->vactivateabort.cb  = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_voice_activate_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_voice_activate_preempted

DESCRIPTION
  This function indicates that the vocoder interrupted VR application.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
vs_status_type vs_voice_activate_preempted ( void )
{
  vs_voice_activate_pending = TRUE;
  return (VS_SUCCESS);

} /* vs_voice_activate_preempted */
/*===========================================================================

FUNCTION vs_vactivate_abort_processing

DESCRIPTION
  This function processes the voice activate abort request in the Voice 
  Services task command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  vs_play_cb

===========================================================================*/
LOCAL void vs_vactivate_abort_processing
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  /* Make sure call back does not happen after aborting play back.  Call back
  ** only after aborting capture also.
  */
  vs_play_cb = (vs_play_cb_type) NULL;
  vs_play_abort_processing ((vs_play_cb_type) NULL);

  /* Call VR function to handle voice activation abort.
  */ 
  vr_voice_activate_abort (abort_callback_ptr);
  vs_voice_activate_pending = FALSE;

} /* end vs_vactivate_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_implicit_yes

DESCRIPTION
  This function signals the VS task to start implicit yes control function.  
  The caller routine passes a call back function for VS to call back at 
  the end of the implicit yes process.  The call back function returns
  intermediate status and the final result of implicit yes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_implicit_yes
(
  vs_sound_type**     prompt_list, /* List of prompts for implicit yes   */
  vs_rec_cb_type      cb_back_ptr  /* ptr to funct when implicit yes done*/
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task to start implicit yes
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_rec_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_IMPLICIT_YES;
    packet->recog.play_list          = prompt_list;
    packet->recog.cb                 = cb_back_ptr;
    vs_cmd (packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_implicit_yes_processing

DESCRIPTION
  This function initiates implicit yes.

DEPENDENCIES
  TBD

RETURN VALUE
  TBD

SIDE EFFECTS
  TBD

===========================================================================*/
LOCAL void vs_implicit_yes_processing 
(
  vs_sound_type**          prompt_list, /* List of prompts for recognition */
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when recog done*/
)
{
  /* initialize recognition function with the appropriate data.      
  */
  vr_implicit_yes_init(prompt_list,  cb_back_ptr);

  /* call function to start recognition, ignore return status since
  ** error handling and reporting are done at VR level.             
  */
  (void) vr_implicit_yes ();

} 

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menu_selection

DESCRIPTION
  This function starts prompts listing and accepts recognizable utterances 
  in between prompts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_menu_selection (
  vs_sound_type**          prompt_list, /* List of prompts for activation   */
  vdb_rw_list_type*        list_ptr,    /* List of RWIDs                    */
  vs_menu_sel_type         include_list,/* Include list to be recognized    */
  vs_menu_play_prompt_type order_prompt,/* Order of the prompts             */
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when done       */
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_rec_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_MENU_SELECTION;
    packet->menusel.play_list        = prompt_list;
    packet->menusel.list_ptr         = list_ptr;
    packet->menusel.include          = include_list;
    packet->menusel.order            = order_prompt;
    packet->menusel.cb               = cb_back_ptr;
    vs_cmd (packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_mselection_processing

DESCRIPTION
  This function processes the menu selection command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_mselection_processing (
  vs_sound_type**          prompt_list, /* List of prompts for activation  */
  vdb_rw_list_type*        list_ptr,    /* List of RWIDs                   */
  vs_menu_sel_type         include_list,/* Include List to be recognized   */
  vs_menu_play_prompt_type order_prompt,/* Order of the prompts             */
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when done      */
)
{
  if ( list_ptr->list == NULL || list_ptr->num_rws == 0)
  {
    /* there are no templates to match against so exit menu selection
    */
    cb_back_ptr( VS_FAILURE, VDB_RWID_NONE, 0, NULL );
    MSG_HIGH ("No RWIDs list, Exit menu selection.", 0, 0, 0);
    return;
  }
  
  /* initialize menu selection function with the appropriate data.
  */
  vr_menu_selection_init(prompt_list, list_ptr, include_list, 
                         order_prompt, cb_back_ptr);

  (void) vr_menu_selection ();

} /* end vs_mselection_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menu_selection_abort

DESCRIPTION
  This function aborts the current menu selection process. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/

void vs_menu_selection_abort (
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  vs_packet_type               *packet;

  /* Signal VS task to abort voice activation
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL)
  {
    /* VS task queue is full, call back with error code.
    */
    if (abort_callback_ptr != (vs_rec_cb_type) NULL)
    {
      abort_callback_ptr (VS_ABORT_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd            = VS_MENU_SELECTION_ABORT;
    packet->menuselabort.cb    = abort_callback_ptr;
    vs_queue_cmd (packet);
  }
} /* end vs_menu_selection_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menu_selection_abort_processing

DESCRIPTION
  This function processes the menu selection abort request in the Voice 
  Services task command queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  vs_play_cb

===========================================================================*/
void vs_menu_selection_abort_processing (
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
)

{
  /* Make sure call back does not happen after aborting play back.  Call back
  ** only after aborting capture also.
  */
  vs_play_cb = (vs_play_cb_type) NULL;
  vs_play_abort_processing ((vs_play_cb_type) NULL);

  /* Call VR function to handle voice activation abort.
  */ 
  vr_menu_selection_abort (abort_callback_ptr);

} /* end vs_vactivate_abort_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for 
  training.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_train_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }

    /* update playback status to training function.
    */
    vr_playback_callback(vs_play_status);

    /* call function to start training, ignore return status since
    ** error handling and reporting are done at VR level.             
    */
    (void) vr_train_utterance ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for training.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_train_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to training function.                   
  */
  vr_capture_callback(status);
 
  /* call function to start training, ignore return status since
  ** error handling and reporting are done at VR level.             
  */
  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    (void) vr_train_utterance ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for 
  recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recog_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }
    /* update playback status to recognition function.
    */
    vr_playback_callback(vs_play_status);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_multi_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for 
  recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recog_multi_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }
    /* update playback status to recognition function.
    */
    vr_playback_callback(vs_play_status);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status
  for recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recog_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to recognition function.                   
  */
  vr_capture_callback(status);

  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    /* call function to start recognition, ignore return status since 
    ** error handling and reporting are done at VR level.             
    */
    (void) vr_recognize_utterance ();
  }

} /* End of vs_recog_capture_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_multi_recog_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status
  for recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recog_multi_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to recognition function.                   
  */
  vr_capture_callback(status);

  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    /* call function to start recognition, ignore return status since 
    ** error handling and reporting are done at VR level.             
    */
    (void) vr_recognize_multi ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_iyes_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for 
  implicit yes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_iyes_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }

    /* update playback status to recognition function.
    */
    vr_playback_callback(vs_play_status);
       
  }
} 

/* <EJECT> */
/*===========================================================================

FUNCTION vs_iyes_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status
  for implicit yes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_iyes_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to recognition function.                   
  */
  vr_capture_callback(status);
   
  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    /* call function to start recognition, ignore return status since 
    ** error handling and reporting are done at VR level.             
    */
    (void) vr_implicit_yes ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  voice activation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_vactivate_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }

    /* update playback status to voice activation function.
    */
    vr_playback_callback(vs_play_status);

    /* call function to start voice activation, ignore return status since
    ** error handling and reporting are done at VR level.
    */
    (void) vr_voice_activate ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status
  for voice activation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_vactivate_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to voice activation function.
  */
  vr_capture_callback(status);

  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    /* call function to start voice activation, ignore return status since
    ** error handling and reporting are done at VR level.
    */
    (void) vr_voice_activate ();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menusel_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  menu selection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_menusel_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }
    /* update playback status to menu selection function.
    */
    vr_playback_callback(vs_play_status);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menusel1_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  menu selection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_menusel1_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
)
{
  if((status == VS_DIFF_SO) || (status == VS_BAD_VP)) {
    vs_play_status = VS_FAILURE;
  } else {
    if(status == VS_ABORTED) {
      vs_play_status = VS_ABORTED;
    }
    /* update playback status to menu selection function.
    */
    vr_playback_callback(vs_play_status);
    /* call function to start menu selection, ignore return status since
    ** error handling and reporting are done at VR level.
    */
    (void) vr_menu_selection ();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menusel_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status
  for menu selection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_menusel_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
)
{
  /* update capture status to menu selection function.
  */
  vr_capture_callback(status);

  if ( (status != VS_IDLE) && (status != VS_INPROGRESS) )
  {
    /* call function to start menu selection, ignore return status since
    ** error handling and reporting are done at VR level.
    */
    (void) vr_menu_selection ();
  }

}

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/*===========================================================================

FUNCTION vs_vm_set_parameters

DESCRIPTION
  This function sets the configuration settings for recording.  
  This is an optional call as Voice Memo has a default value for each 
  attribute.

    ->    config_data_ptr.
    ->    callback_ptr,

DEPENDENCIES
  None

RETURN VALUE
    VS_VM_INVALID_PARAMETER,     Invalid parameter was set
    VS_VM_SUCCESS                Successfully set configuration values.

SIDE EFFECTS
  None


===========================================================================*/
void  vs_vm_set_parameters
(
  vs_vm_config_type           *config_data_ptr,
  vs_vm_control_cb_type        callback_ptr
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_SET_PARAM;
      packet->vsvmsetparam.config_data_ptr = config_data_ptr;
      packet->vsvmsetparam.callback_ptr = callback_ptr;
      vs_cmd (packet);
  }

} /* end vs_vm_set_parameters */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_get_parameters

DESCRIPTION
  This function returns the configuration settings for recording

    <->   config_data_ptr
    <-    callback_ptr,

DEPENDENCIES
  None

RETURN VALUE
    VS_VM_INVALID_PARAMETER,     Invalid parameter was set
    VS_VM_SUCCESS                Completed successfully

SIDE EFFECTS
  None

===========================================================================*/
void  vs_vm_get_parameters
(
  vs_vm_config_type           *config_data_ptr,
  vs_vm_control_cb_type        callback_ptr
)
{
  vs_packet_type               *packet;


  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_GET_PARAM;
      packet->vsvmgetparam.config_data_ptr = config_data_ptr;
      packet->vsvmgetparam.callback_ptr = callback_ptr;
      vs_cmd (packet);
  }
} /* end vs_vm_get_parameters */


/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_start

DESCRIPTION
  This function starts recording the utterances for VoiceMemo using the 
  specified   configuration information.  Will finish recording if the 
  SND task reports error,  or Voice Database reports error or reached the 
  maximum capacity at the storage.

    ->  promptlist,             list of prompts to use during processing 
                                of the function.
    ->  record_callback,        A pointer to the call back function

DEPENDENCIES
  None

RETURN VALUE
    None

RETURN VALUE IN A CALLBACK
    VS_VM_STARTED,        SND task is ready to capture utterances
    VS_VM_MEMO_ID,        FLASH/VoiceDataBase is ready to store utterances
    VS_VM_WARNING_MARK,   Reached the warning level at the storage
    VS_VM_MAX_MARK,       Reached the maximum level at the storage
    VS_VM_REQUESTED_MS,   Enough frames ready to be stored to the storage
    VS_VM_FINISHED        Can't record any longer, thus finished recording.
    VS_VM_ERR_VOC,        snd task is not able to record
    VS_VM_ERR_DATABASE,   database can't support recording


SIDE EFFECTS
  None
===========================================================================*/

vs_sound_type *g_prompt_list[NUM_OF_RECORDING_PROMPTS]; 

void vs_vm_record_start
(
    vs_sound_type         **promptlist,
    vs_snd_direction_type   rec_direction,
    vs_vm_record_cb_type    record_callback
)
{
  uint16            cnt;
  vs_packet_type   *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_REC_START;
      packet->vmrecstart.cb = record_callback;

      /* save the pointer to VS context's global var */
      for (cnt = 0; cnt < NUM_OF_RECORDING_PROMPTS;  cnt++)
      {
        g_prompt_list[cnt] = promptlist[cnt];
      }

      packet->vmrecstart.promptlist = g_prompt_list;
      packet->vmrecstart.rec_direction = rec_direction;
      vs_cmd (packet);
  }

} /* end vs_vm_record_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_stop

DESCRIPTION
  This function stops recording the memo specified.
    ->    vmid,        VoiceMemo id to stop        

DEPENDENCIES
  The caller is expected to remove the memo vis DB calls, if the caller 
  decides  not to save the memo after 'record_stop' was called
  (record_cancel will remove the memo).

RETURN VALUE IN A CALLBACK
    VS_VM_STOPPED,          Recording has been stopped successfully.
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid

SIDE EFFECTS
  caller can't call "record_cancel" to remove the memo, after it has called
  "record_stop" previously on the same memo.
===========================================================================*/
void vs_vm_record_stop
(
   vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_REC_STOP;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }


} /* end vs_vm_record_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_synch_stop

DESCRIPTION
  This synchronous function stops recording the memo specified.
    ->    vmid,        VoiceMemo id to stop

DEPENDENCIES
  The caller is expected to remove the memo vis DB calls, if the caller
  decides  not to save the memo after 'record_stop' was called
  (record_cancel will remove the memo).

RETURN VALUE IN A CALLBACK
    There is no callback

SIDE EFFECTS
  caller can't call "record_cancel" to remove the memo, after it has called
  "record_stop" previously on the same memo.
===========================================================================*/
void vs_vm_record_synch_stop
(
   vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();

  if (packet != NULL)
  {
      packet->hdr.cmd =  VS_VM_REC_SYNCH_STOP;
      packet->vsvmvmidonly.vmid = vmid;
      vs_queue_cmd (packet);
  }

} /* end vs_vm_record_synch_stop */


/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_pause

DESCRIPTION
  This function pauses recording the memo specified
    ->    vmid,        VoiceMemo id to pause        

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_PAUSED,           Recording has been paused successfully.
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid

SIDE EFFECTS
  Once the record_pause has been called, 'record_resume' has to be called to
  resume recording.  
  Calling "record_start" won't resume recording but generate unexpected result
  Calling "record_stop" will stop and put it out ot "record_pause" mode.
===========================================================================*/
void vs_vm_record_pause
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_REC_PAUSE;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_record_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_resume

DESCRIPTION
  This function resumes recording the memo specified.
    ->    vmid,        VoiceMemo id to resume        

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_RESUMED,           Recording has been resumed successfully
    VS_VM_INVALID_MEMO_ID    Specified memo is invalid


SIDE EFFECTS
  Once the recording has been paused, 'record_resume' has to be called to
  resume recording.  
  Calling "record_start" won't resume recording but generate unexpected result
  Calling "record_stop" will stop and put it out ot "record_pause" mode.
===========================================================================*/
void vs_vm_record_resume
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_REC_RESUME;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_record_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_cancel

DESCRIPTION
  This function cancels recording the memo specified.  
  Removes the voice memo that might have been stored in the storage.  
  The specified vmid is no longer valid.

    ->    vmid,        VoiceMemo id to cancel        

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_CANCELED,         Cancelled recording of the voice memo specified
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid


SIDE EFFECTS
 user can't use "record_cancel" to remove the memo, after it had already called
  "record_stop" previously on the same memo.
===========================================================================*/
void vs_vm_record_cancel
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_REC_CANCEL;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_record_cancel */

/* <EJECT> */
/*===========================================================================
FUNCTION VS_VM_CAN_RECORD_MEMO

DESCRIPTION
  This function determines whether there is sufficient storage space to
  record another voice memo.  If so, TRUE is returned.  Otherwise,
  FALSE is returned.

  It'll look for
  - available EFS space 
  - number of memos
  - number of accumulated (system) length of memos.
  - ability to record more than 1 second of recording.

DEPENDENCIES
  FEATURE_VOICE_MEMO is defined.

RETURN VALUE
  TRUE if a new voice memo can be recorded.
  FALSE otherwise.

SIDE EFFECTS
  It's intended for UI's to find out whether the recording can
  start when the vs_vm_record_start function is called.  It's processed
  synchronously in caller's task.
===========================================================================*/
boolean vs_vm_can_record_memo (void)
{
  return vs_vm_can_record_memo_processing();
}

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_start

DESCRIPTION
  This function plays the specified voice memo.  
  After the completion of the specified number of miliseconds
  (20 should be the minimum value and multiple of 20) of the memo, 
  the callback_ptr function will be called.

    ->    vmid,                 VoiceMemo id to start
    ->    play_rpt_interval_ms, Number of miliseconds to play for 
                                each notification
    ->    callback_ptr,         A pointer to the call back function

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_STARTED              Got the VM from the storage to be played
    VS_VM_REQUESTED_MS         Played the specified amount of miliseconds.
    VS_VM_INVALID_MEMO_ID      Specified memo is invalid
    VS_VM_INVALID_PARAMETER    min of  20 and  multiple of 20 for interval
    VS_VM_FINISHED             Finished playing the voice memo, EOF

SIDE EFFECTS
  None
===========================================================================*/
void vs_vm_play_start
(
    vdb_vmemo_id_type     vmid,
    uint32                play_rpt_interval_ms,
    vs_snd_direction_type play_direction,
    vs_vm_play_cb_type    callback_ptr
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_START;
      packet->vsvmplaystart.vmid = vmid;
      packet->vsvmplaystart.play_rpt_interval_ms = play_rpt_interval_ms;
      packet->vsvmplaystart.play_direction = play_direction;
      packet->vsvmplaystart.callback_ptr = callback_ptr;
      vs_cmd (packet);
  }

} /* end vs_vm_play_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_stop

DESCRIPTION
  This function stops playing the specified voice memo
    ->    vmid,            VoiceMemo id to stop

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_STOPPED,            Stopped playing memo successfully.
    VS_VM_INVALID_MEMO_ID,    Specified memo is invalid 

SIDE EFFECTS
  None
===========================================================================*/
void vs_vm_play_stop
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_STOP;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_play_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_synch_stop

DESCRIPTION
  This synchronous function stops playing the specified voice memo
    ->    vmid,            VoiceMemo id to stop

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    There is no callback

SIDE EFFECTS
  None
===========================================================================*/
void vs_vm_play_synch_stop
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();

  if (packet != NULL)
  {
      packet->hdr.cmd =  VS_VM_PLAY_SYNCH_STOP;
      packet->vsvmvmidonly.vmid = vmid;
      vs_queue_cmd (packet);
  }

} /* end vs_vm_play_synch_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_pause
    ->    vmid,            VoiceMemo id to pause

DESCRIPTION
  This function pause playing the specified voice memo


DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_PAUSED,            Paused playing memo successfully.
    VS_VM_INVALID_MEMO_ID,   Specified memo is invalid


SIDE EFFECTS
  None
===========================================================================*/
void vs_vm_play_pause
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_PAUSE;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_play_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_resume
    ->    vmid,            VoiceMemo id to resume

DESCRIPTION
  This function resume playing the specified voice memo


DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_RESUMED            Resumed playing memo successfully.
    VS_VM_INVALID_MEMO_ID,   Specified memo is invalid


SIDE EFFECTS
  None
===========================================================================*/
void vs_vm_play_resume
(
    vdb_vmemo_id_type    vmid
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_RESUME;
      packet->vsvmvmidonly.vmid = vmid;
      vs_cmd (packet);
  }

} /* end vs_vm_play_resume */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_fastforward

DESCRIPTION
  This function FastForwards the specified amount of seconds and continues 
  playing the specified voice memo

    ->    vmid,            VoiceMemo id to identify the function 
    ->    seconds,        requested number of seconds to be fastforwarded

DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_FINISHED,           Finished playing the memo, reached EOF.
    VS_VM_FORWARDED,          Forwarded and playing the memo.
    VS_VM_INVALID_MEMO_ID,    Specified memo is invalid
    VS_VM_INVALID_PARAMETER   Can't be negative or zero to skip fastforward


SIDE EFFECTS
  Before jumping ahead specified amount of seconds, the current position is 
  considered to be the very first frame of the current "second"(truncated). 
  It'll always start playing at the beginning of the "second" boundary. 
  (Current position at 5.6 sec… FF 4 seconds … plays from 9 seconds into 
 the memo).

===========================================================================*/
void vs_vm_play_fastforward
(
    vdb_vmemo_id_type   vmid,
    uint32              seconds
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_FASTFORWARD;
      packet->vsvmrewff.vmid = vmid;
      packet->vsvmrewff.seconds = seconds;
      vs_cmd (packet);
  }

} /* end vs_vm_play_fastforward */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_rewind

DESCRIPTION
  This function rewinds the specified amount of seconds and continues playing 
  the specified voice memo

    ->    vmid           VoiceMemo id to identify the function 
    ->    seconds        requested number of seconds to be rewound


DEPENDENCIES
  None

RETURN VALUE IN A CALLBACK
    VS_VM_REWOUND,             Rewound and playing the memo.
    VS_VM_INVALID_MEMO_ID,     Specified memo is invalid
    VS_VM_INVALID_PARAMETER    Can't be negative or zero to skip backwards.

SIDE EFFECTS
    Before jumping back specified amount of seconds, the current position is 
    considered to be the very first frame of the current "second"(truncated).
    It'll always start playing at the beginning of the "second" boundary. 
    (Current position at 5.6 sec… Rewind 4 seconds… plays from 1 second into
    the memo).Will play from the beginning of the memo if the requested rew 
    seconds become  a negative number based on the current position.
===========================================================================*/
void vs_vm_play_rewind
(
    vdb_vmemo_id_type    vmid,
    uint32               seconds
)
{
  vs_packet_type               *packet;

  packet = vs_cmd_get_pkt ();
  
  if (packet != NULL) 
  {
      packet->hdr.cmd =  VS_VM_PLAY_REWIND;
      packet->vsvmrewff.vmid = vmid;
      packet->vsvmrewff.seconds = seconds;
      vs_cmd (packet);
  }
} /* end of vs_vm_play_rewind */

#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_utterance

DESCRIPTION
  This function signals the VS task to start the training utterance process.  
  The caller routine passes a call back function for VS to call back at 
  the end of the train utterance process.  The call back function returns
  intermediate status and the final result of training.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_train_utterance
(
  vs_sound_type**     prompt_list,      /* List of prompts for training    */
  vs_utt_echo_type    echo_utterance,   /* echo utterance to user or not   */ 
  vdb_rw_list_type**  list_ptr,         /* recognizable word set ID        */ 
  vs_train_cb_type    cb_back_ptr       /* ptr to funct when training done */
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task to start training the utterance
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_train_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, NULL);
    }
  }
  else
  {
    packet->hdr.cmd                  = VS_TRAIN_UTT;
    packet->train.play_list          = prompt_list;
    packet->train.echo_utt           = echo_utterance;
    packet->train.control_word       = VDB_RWID_NONE;
    packet->train.list_ptr           = list_ptr;
    packet->train.cb                 = cb_back_ptr;
    vs_cmd (packet);
  }
} /* End vs_train_utterance */ 

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_control_word

DESCRIPTION
  This function signals the VS task to start training the specified
  control word.
  The caller routine passes a call back function for VS to call back at 
  the end of the train word process.  The call back function returns
  intermediate status and the final result of training the control word.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_train_control_word
(
  vs_sound_type**      prompt_list,      /* List of prompts for training    */
  vs_utt_echo_type     echo_utterance,   /* echo utterance to user or not   */
  vdb_rwid_type        train_word,       /* RWID of control word to train   */  
  vdb_rw_list_type**   list_ptr,         /* List of RWID set                */ 
  vs_train_cb_type     cb_back_ptr       /* ptr to funct when training done */
)
{
  vs_packet_type           *packet;
  
  /* Signal VS task to start training the control word
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_train_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, NULL);
    }
  } else {
    packet->hdr.cmd            = VS_TRAIN_UTT;
    packet->train.play_list    = prompt_list;
    packet->train.echo_utt     = echo_utterance;
    packet->train.control_word = train_word;
    packet->train.list_ptr     = list_ptr;
    packet->train.cb           = cb_back_ptr;
    vs_cmd (packet);
  }
} /* End vs_train_control_word */ 

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_utterance

DESCRIPTION
  This function signals the VS task to start the recognize utterance process.  
  The caller routine passes a call back function for VS to call back at 
  the end of the recognition process.  The call back function returns the 
  result of recognition.
  This function cannot be called if vs_recognize_multi is not finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recognize_utterance
(
  vs_sound_type**          prompt_list, /* List of prompts for recognition */
  vdb_rw_list_type**       list_ptr,    /* List of recognizable word sets  */
  boolean                  use_n_best,  /* set to use n-best resolution    */
  short                    match_count, /* number of top candidates to keep*/
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when recog done*/
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_rec_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd           = VS_RECOGNIZE_UTT;
    packet->recog.play_list   = prompt_list;
    packet->recog.list_ptr    = list_ptr;
    packet->recog.n_best      = use_n_best;
    packet->recog.match_cnt   = match_count;
    packet->recog.cb          = cb_back_ptr;
    vs_cmd (packet);
  }
} /* end vs_recognize_utterance */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_multi

DESCRIPTION
  This function signals the VS task to start multiple recognize utterance
  process.  
  The caller routine passes a call back function for VS to call back at 
  the end of the recognition process.  The call back function returns the 
  result of recognition. 
  This function cannot be called if vs_recognize_utterance is not finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_recognize_multi
(
  vs_sound_type**     prompt_list,     /* List of prompts for recognition  */
  vdb_rw_list_type*   list_ptr,        /* List of recognizable word set    */
  vdb_rw_list_type*   list_prompt_ptr, /* Prompt correspondent to list_ptr */
  vs_rec_multi_type   gap_mode,        /* Intra Word gap for recognition   */ 
  vs_rec_cb_type      cb_back_ptr      /* pointer to funct when recog done */
)
{
  vs_packet_type               *packet;
  
  /* Signal VS task
  */
  packet = vs_cmd_get_pkt ();

  if (packet == NULL) 
  {
    /* VS task queue is full, call back with error code.
    */
    if (cb_back_ptr != (vs_rec_cb_type) NULL)
    {
      cb_back_ptr (VS_FAILURE, VDB_RWID_NONE, 0, NULL);
    }
  }
  else
  {
    packet->hdr.cmd                     = VS_RECOGNIZE_MULTI;
    packet->recog_multi.play_list       = prompt_list;
    packet->recog_multi.list_ptr        = list_ptr;
    packet->recog_multi.list_prompt_ptr = list_prompt_ptr;
    packet->recog_multi.gap_mode        = gap_mode;
    packet->recog_multi.cb              = cb_back_ptr;
    vs_cmd (packet);
  }
} /* end vs_recognize_multi */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_utt_processing

DESCRIPTION
  This function initiates the training of voice tags and control words
  within the VS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_train_utt_processing 
(
  vs_sound_type**       prompt_list,    /* List of prompts for training    */
  vs_utt_echo_type      echo_utterance, /* echo utterance to user or not   */
  vdb_rwid_type         train_word,     /* RWID of control word to train   */
  vdb_rw_list_type**    list_ptr,       /* ptr to RWID set                 */ 
  vs_train_cb_type      cb_back_ptr     /* ptr to funct when training done */
)
{
  vdb_err_type  vdb_ret;
  unsigned long space_used = 0;

  /* check for NV space availability before start training voice tags */
  if (train_word == VDB_RWID_NONE)      /* voice tag training */
  {
      /* get physical size (not logical size) that's used to store all
      ** all the voice tag files (utterances, not patterns) 
      */
      vdb_ret = vdb_vtag_utt_space_used(&space_used);
      if (vdb_ret == VDB_NOERR &&
          space_used >= VS_MAX_UTTERANCE_SPACE_ALLOCATED)
      {
         if (cb_back_ptr != (vs_train_cb_type) NULL)
         {
           cb_back_ptr ( VS_NV_MEM, VDB_RWID_NONE, NULL );
         }
         return;
      }
  }

  /* initialize recognition function with the appropriate data.      
  */
  vr_train_init (prompt_list,
                        echo_utterance,
                        train_word,
                        list_ptr,
                        cb_back_ptr);

  /* call function to start training, ignore return status since
  ** error handling and reporting are done at VR level.             
  */
  (void) vr_train_utterance ();

} /* end vs_train_utt_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_utt_processing

DESCRIPTION
  This function initiates the recognition process within the VS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_recognize_utt_processing 
(
  vs_sound_type**          prompt_list, /* List of prompts for recognition */
  vdb_rw_list_type**       list_ptr,    /* List of recognizable word sets  */
  boolean                  use_n_best,  /* set to use n-best resolution    */
  short                    match_count, /* number of top candidates to keep*/
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when recog done*/
)
{
  /* initialize recognition function with the appropriate data.      
  */
  vr_recognize_init(prompt_list, list_ptr, use_n_best, match_count, 
                                                                cb_back_ptr);

  /* call function to start recognition, ignore return status since
  ** error handling and reporting are done at VR level.             
  */
  (void) vr_recognize_utterance ();

} /* end vs_recognize_utt_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_multi_processing

DESCRIPTION
  This function initiates the recognition process within the VS task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void vs_recognize_multi_processing 
(
  vs_sound_type**     prompt_list,      /* List of prompts for recognition */
  vdb_rw_list_type*   list_ptr,         /* List of recognizable word set   */
  vdb_rw_list_type*   list_prompt_ptr,  /* Prompts correspondent to Set    */
  vs_rec_multi_type   gap_mode,         /* Intra Word gap for recognition   */ 
  vs_rec_cb_type      cb_back_ptr       /* pointer to funct when recog done*/
)
{
  /* initialize recognition function with the appropriate data.      
  */
  vr_recognize_multi_init(prompt_list, list_ptr, 
                          list_prompt_ptr, gap_mode, cb_back_ptr);

  /* call function to start recognition, ignore return status since
  ** error handling and reporting are done at VR level.             
  */
  (void) vr_recognize_multi ();

} /* end vs_recognize_multi_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_endpoint

DESCRIPTION
  This function queues a request to execute the VR end point detection in VS 
  task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_endpoint 
(
  uint8                         *buf_ptr
  /* Ptr to buffer containing voc frame + vr diagnostic data + frame no    */
) 
               
{
  uint16                        index;
  vs_packet_type                *packet;
  
  /* Return if the VR Endpoint Detector does not need vocoder packets.
  */
  if (vs_endpoint_capture == FALSE)
  {
    return;
  }
    
  /* Too many buffers to process.
  */
  if (vs_endpoint_buffer.number >= VS_MAX_EP_BUF_ENTRIES) 
  {
    ERR ("Endpoint buffer overflow!!!",0,0,0);
    return;
  }
    
  /* Find next entry in buffer to store vocoder frame, wrap around if 
  ** necessary.
  */
  index = vs_endpoint_buffer.index + vs_endpoint_buffer.number;
  if (index >= VS_MAX_EP_BUF_ENTRIES) 
  {
    index -= VS_MAX_EP_BUF_ENTRIES;
  }

  memcpy(vs_endpoint_buffer.buffer[index], buf_ptr, VS_EP_BUF_LENGTH);

  ++vs_endpoint_buffer.number;

  /* Queue up the request if Endpoint Detector is not already running.
  */
  if (vs_endpoint_in_progress == FALSE)
  {
    /* Endpoint Detector is running.
    */
    vs_endpoint_in_progress = TRUE;
    
    /* Signal VS task to process endpoint detection.
    */
    packet = vs_cmd_get_pkt ();
   
    if (packet != NULL) 
    {
      packet->hdr.cmd           = VS_ENDPT_DETECT;
      vs_cmd (packet);
    }
  }
} /* end vs_endpoint */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_endpoint_processing

DESCRIPTION
  This function executes the VR end point detection in VS task context rather
  than interrupt context due to the length of this operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_endpoint_processing (void) 

{
  uint8 *buf_ptr = NULL;

  /* Return if the VR Endpoint Detector does not need vocoder packets or there
  ** is not a frame ready.  The request may have been queued in the VS queue
  ** before these conditions become TRUE.
  */
  if ((vs_endpoint_capture == FALSE) || (vs_endpoint_buffer.number == 0))
  {
    return;
  }

  /* Process all frames in the buffer.
  */
  while ((volatile) vs_endpoint_in_progress == TRUE) {
    /* Get next vocoder frame in endpoint buffer, wrap around if necessary.  
    ** Lock interrupts since the endpoint buffer is also updated in interrupt 
    ** context.
    */
    buf_ptr = vs_endpoint_buffer.buffer [vs_endpoint_buffer.index];
   
    INTLOCK();
    vs_endpoint_buffer.index++;
    if (vs_endpoint_buffer.index >= VS_MAX_EP_BUF_ENTRIES) 
    {
      vs_endpoint_buffer.index = 0;
    }
    vs_endpoint_buffer.number--;
    if (vs_endpoint_buffer.number == 0) {
      vs_endpoint_in_progress = FALSE;
    }
    INTFREE();
    
    /* Make sure endpoint detection is keeping up with vocoder frame interrupts.
    */
    if (vs_endpoint_buffer.number > 4) 
    {
      MSG_HIGH ("%d frames in endpoint buf", vs_endpoint_buffer.number, 0, 0);
    }
    /* Call endpoint detection function.  Stop capture when endpoint detection
    ** is done.
    */
    if (vre_endpoint (buf_ptr) == VRE_ENDPOINT_DONE)
    {
      vs_endpoint_capture     = FALSE;
      vs_endpoint_in_progress = FALSE;
    }
  } /* end while */
} /* end vs_endpoint_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_capture_start

DESCRIPTION
  This function initializes the VS task for a capture operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_capture_start (void)
               
{
  /* Initialize capture variables.
  */
  vs_endpoint_buffer.index   = 0;
  vs_endpoint_buffer.number  = 0;
  vs_endpoint_capture        = TRUE;
  vs_endpoint_in_progress    = FALSE;

} /* end vs_capture_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_set_vr_engine

DESCRIPTION
  This function sets the Engine to use: Speaker Dependent only or Hybrid.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vs_set_vr_engine (
  vs_patt_settings_type pattern_settings
)
               
{
  if ( pattern_settings == VS_PATT_SD) {
    vre_engine_settings ( VRE_ENGINE_SD );
  } else {
    vre_engine_settings ( VRE_ENGINE_HYBRID );
  }

} /* End vs_set_vr_engine */

/* <EJECT> */
/*===========================================================================

FUNCTION vs_get_max_vocabulary_size

DESCRIPTION
  This function returns Max Vocabulary Size allowed by the Engine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int vs_get_max_vocabulary_size ( void )
{
 return (vre_get_max_vocabulary_size());
} /* End vs_get_max_vocabulary_size */

/* <EJECT> */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VIDEO_ENCODE_RAW_ENGINE
/*===========================================================================

FUNCTION vs_video_enc_raw_encode_handshake

DESCRIPTION
  This is called to queue a command to VS task from the mp4 encoder
  
DEPENDENCIES
  None

RETURN VALUE
  VS_FAILURE: If there was a problem with capture stop
  VS_SUCCESS: if there were no errors.

SIDE EFFECTS
  None

===========================================================================*/

void vs_video_enc_raw_encode_handshake(void* buffer)
{   vs_packet_type      *vspacket;
  
  if ((vspacket = vs_cmd_get_pkt()) != NULL) {
    vspacket->hdr.cmd = VS_CAMERA_CMD;
    vspacket->camerapkt.msg = VS_RAW_VIDEO_ENCODE_DONE;
    vspacket->camerapkt.buf = buffer;
    vspacket->camerapkt.size = 0;
    vs_queue_cmd(vspacket);
  }

} 
#endif 
/* <EJECT> */
#ifdef VS_TEST_CODE
#error code not present
#endif /* VS_TEST_CODE */

#endif /* FEATURE_VS_TASK */


