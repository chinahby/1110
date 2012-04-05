/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Vocoder Support")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              V O C O D E R    V R           

GENERAL DESCRIPTION

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

EXTERNALIZED FUNCTIONS
  voc_pb_add_q
    Start playback or add another entry to the playback queue.
  voc_pb_get_status
    Get playback status: idle or playing.
  voc_pb_abort
    Abort playback.
  voc_pb_pause
    Pause playback.
  voc_pb_resume
    Resume playback.
  voc_pb_register
    Register call back function.
  voc_rx_vp_mute_ctl
    Control rx path muting for playback.
  voc_snd_active_status
    Sound information for talk-over.
  voc_rec_start
    Start voice memo recording.
  voc_rec_pause
    Pause recording.
  voc_rec_resume
    Resume recording.
  voc_rec_stop
    Stop recording.
  vocvr_capture_start
    Capture an utterance for Mode B/C VR.
  vocvr_capture_cont
    Send utterance capture continue command to DSP for Mode B/C VR.
  vocvr_capture_stop
    Stop an utterance capture operation for Mode B/C VR.
  vocvr_match_start
    Start a match operation for Mode B/C VR.  
  vocvr_match_stop
    Stop a match operation for Mode B/C VR.
  vocvr_match_result
    Return match result message data. 
  vocvr_pattern_load
    Send patterns to the DSP for a matching operation for Mode B/C VR.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocvr.c#10 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/14/09   yp      CMI changes for SCMM 
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
05/25/09    vm     Added check to ensure the sound file is in pause state
                   for resume to be called.
05/11/09    rm     QCamCorder FW's Vocoder State Paths has changed
                   to Voc Combo's state path. The SW chnages are done
                   to adapt to those changes.
12/29/08    sj     Added .awb file format support
12/19/08    rm      Fix for CR 164515 - Voice(Qcelp) recording fails after calling(4GV)
11/14/08    sj      Don't enable ECNS when we don't require it. Fixed CR 160542
01/30/08    az     Include the support for the In call Qtv.
06/27/07     az    Fixed the CR: 121693 - In the case of repeatting pause 
                   resume Qcamcoder can't record correctly.
05/04/07     at    CR 116741 - Fixed a bug in voc_rec_start().
03/15/07    kan    Added 4GV-WB Code 
01/09/07   suk     Added QDSP_IMAGE_VOC_COMBO_VOIP support.
                   Updated Copyright text.
12/20/06    sp     Fixed ThinUI linker error.
12/04/06   suk     Fixed 6260 ThinUI build compilation error.
11/09/06    az     Merged the code for MM API changes.
09/11/06    ak     Made changes in voc_rec_start function to handle a 
                   recording request in a call.
07/28/06    hz     Added fixes for CR#89558.
06/15/06    az     Fixed CR 81282 Error recording voice memo having
                   AMR audio format during a voice call 
03/13/06    act    Reverting change to voc_rx_vp_mute_ctl().
02/21/06    act    Modified voc_rx_vp_mute_ctl() to use new parameter for
                   voc_rx_mute_cmd().
09/15/05    hz     Added idle check in voc_pb_resume() to fix CR71960 and 
                   CR76675.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/25/05    sg     Modified code to enable DTX in AMR recording.
06/02/05    hz     Modified so that recording is started only in IDLE, VOCODER
                   and GRAPH_AUDIO states.
04/28/05    ymc    Added 4GV NB support.
04/01/05    act    Changed use of specific codec IDs to generic codec types
                   where applicable.
12/03/04    sm     Removed FEATURE_USB_ISOC_AUDIO
12/01/04    aw     Modified code to use voc_check_pb_capability to check 
                   supported voice playback type.
10/18/04   sm/yc   Fixed problem where rec settings weren't being set "in call."
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
06/01/04    aw     Modified code to call voc_restore_comfort_noise in
                   voc_rec_stop_status.
03/05/04    sg     Modified to set comfort noise only if vocoder has been
                   acquired in voc_rec_stop_status() (CR40720).
10/10/03    aw     Added MSMAUD_VOC_AMR_COMMON featurization.
07/24/03    sg     Added support for STEREO_HEADSET.
05/13/03    ymc    Fixed AMR rate setting during voice recording in a call.
            ymc    Replaced FEATURE_WCDMA with MSMAUD_VOC_AMR.
            ymc    Updated copyright.
03/13/03    st     Added support for recording in graphics images.
03/11/03    st     Modified use of INTLOCK_SAV() to INTLOCK().
            sm     Added half-rate defines support.
01/23/03    sm     Featurized AMR record/playback under MSMAUD_VOC_AMR.
01/13/03    jkl    Added BT Codec.
11/18/02    st     Modified QCP playback/recording code to centralize format
                   specific differences; featurized GSM/WCDMA specific code.
11/06/02    sm     Added support for QCP AMR files.
10/29/02    st     Added support for SPEAKER_PHONE. Enabled VR for INTERNAL_SDAC
08/13/02    st     Modified silence detection code to reset counters for every
                   recording session.
07/11/02    yl     Added the silence detection code in voc_rec_frame_processing().
                   Added the code to handle the new vocoder message 
                   VOC_REC_STAT_AUTO_STOP.
07/11/02    jkl    Added comdef.h. Moved voc_match_cmd_in_progress to after
                   prompt command.
03/04/02    jkl    Added Pufe Support under FEATURE_VR_PUFE
09/19/01    st     Modified voc_vr_goto_dfm() to abort prompt playback if
                   playback is to the forward link.
09/14/01    aw     Fixed FEATURE_ACP featurization.
09/11/01   st/aw   Modified voc_pb_resume() to handle changing config for
                   prompt playback to forward link even during CDMA call.
09/03/01    st     Prompt playback was modified to: not abort DFM prompt
                   playback in voc_vr_goto_dfm(), reject DFM playback to
                   forward link (unsupported mode), only reject prompt
                   playback of different formats in a CDMA call if not
                   playing to forward link, support playback to both links
                   in a DFM call.
                   Redesigned vocoder packet recording to: not require that
                   vocoder be acquired at time of record start, reject DFM
                   recording of reverse link, always set min/max rate for
                   recording in CDMA call to eighth/full, abort recording
                   when service option changes.
06/25/01   jkl     Fixed featurization on voc_pb_add_q.
06/19/01   jkl     Fixed featurization.
06/18/01   jkl     Added FEATURE_ACP. Replaced FEATURE_DFM_ANSWER_MACHINE.
05/21/01   jkl     Removed Enhanced VR feature.
04/26/00    sm     Added FEATURE_USB_ISOC_AUDIO support.
04/11/01    jkl    Added voc_retry_codec_gain after match state.
04/04/01    st     Lint updates.
02/08/01    jkl    Added Analog HFK when in capture mode.
12/05/00    jkl    Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
                   FEATURE_DFM_ANSWER_MACHINE.
07/27/00    ro     Remove size parameter from vocsup_rec_frame() call.
05/24/00    ro     Fix support for playback to BOTH in
                   voc_rx_vp_mute_ctl (CR13558).
05/01/00    st     Added voc_pb_abort_dfm() function.
01/26/00    tc     Initial version.
            ro
            st
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#ifdef FEATURE_VOICE_PLAYBACK
#include "target.h"             /* Target specific definitions             */
#include "assert.h"             /* Definitions for ASSERT                  */
#include "msmaud.h"             /* Target-specific audio features          */
#include "voc.h"                /* Vocoder interface definition            */
#include "vocsup.h"             /* Vocoder Support Routines                */
#include "vocstate.h"           /* Vocoder State Controller Definitions    */
#include "qdspcmd.h"            /* QDSP2 Driver Command Layer definitions  */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#include "vocdata.h"            /* Vocoder State and Cal Data Definitions  */
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
#ifdef FEATURE_PHONE_VR
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif
#ifdef FEATURE_CMI_MM
#error code not present
#endif               /* Clock prototypes                        */
#include <string.h>             /* String/Memory manipulation routines     */
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/* This flag indicates if we are playing back a voice utterance.  Two states
** possible, idle or playing.
*/
LOCAL voc_get_pbs_return_type voc_play_back_status = VOC_PB_STATUS_IDLE;

/* This function pointer is used to call back a registered function
** (registered by calling function voc_pb_register) at the end of
** playing a voice utterance.
*/
LOCAL voc_pb_notify_func_ptr_type vc_pb_notify_func;

/* This pointer is a parameter of vc_pb_notify_func.  This pointer is returned
** at the end of playing a voice utterance so the utterance buffer can be 
** re-used.
*/
LOCAL voc_pb_header_type *curr_pb_hdr = NULL;

LOCAL boolean voc_pb_incall_mux_possible = FALSE;

#ifdef FEATURE_VOICE_RECORD

/* Bit mask values for packet format
*/
#define VOCVR_FULL_RATE_PACKET_V     0x00
#define VOCVR_HALF_RATE_PACKET_V     0x01
#define VOCVR_QUARTER_RATE_PACKET_V  0x02
#define VOCVR_EIGHTH_RATE_PACKET_V   0x03
#define VOCVR_BLANK_RATE_PACKET_V    0x04
#define VOCVR_ERASURE_RATE_PACKET_V  0x08
#define VOCVR_INVALID_RATE_PACKET_V  0x10
#define VOCVR_13K_PACKET_DATA_V      0x80

#ifdef FEATURE_PHONE_VR
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */
/* Recording call back function.
*/
LOCAL voc_rec_cb_type voc_rec_cb = (voc_rec_cb_type) NULL;

/* Vocoder configuration for voice recording.
*/
#ifdef MSMAUD_VOC_AMR
LOCAL voc_capability_type voc_rec_capability = VOC_CAP_AMR;
#else
LOCAL voc_capability_type voc_rec_capability = VOC_CAP_IS733;
#endif /* MSMAUD_VOC_AMR */

/* This parameter is used to keep track of the recording parameters in
** case we have to start recording after acquiring the vocoder.
*/
LOCAL voc_rec_parm_type voc_rec_parm_config;

/* This flag is used to indicate that recording has already been started.
*/
LOCAL boolean           voc_rec_started         = FALSE;
LOCAL uint16            voc_rec_auto_stop_frame = 0;
LOCAL uint16            voc_rec_silence_counter = 0;
LOCAL uint16            voc_rec_noise_counter   = 0;

#endif /* FEATURE_VOICE_RECORD */


#if defined (FEATURE_PHONE_VR) || defined (FEATURE_VOICE_RECORD)

/* Identifies current recording type if any.
*/
LOCAL voc_rec_type  voc_rec_cmd_in_progress = VOC_REC_NONE;

#endif /* FEATURE_PHONE_VR || FEATURE_VOICE_RECORD */


/* Array to convert voc_rate_type to QDSP register values
*/
LOCAL const uint16 vocvr_rate_conv[] = {
  QDSP_RATE_BLANK_V,                /* Blank        */
  QDSP_RATE_EIGHTH_V,               /* 8th rate     */
  QDSP_RATE_QUARTER_V,              /* Quarter rate */
  QDSP_RATE_HALF_V,                 /* Half rate    */
  QDSP_RATE_FULL_V,                 /* Full rate    */
  QDSP_RATE_ERASURE_V               /* Erasure      */
};

#if defined(MSMAUD_VOC_AMR_COMMON)
/* Array to convert mode from vocoder to QDSP value. */
LOCAL const uint16 vocvr_amr_mode_conv_table[] = {
  0,
  0,
  0,
  0,
  QDSP_AMR_RX_SPEECH_GOOD_V,
  QDSP_AMR_RX_SPEECH_DEGRADED_V,
  QDSP_AMR_RX_ONSET_V,
  QDSP_AMR_RX_SPEECH_BAD_V,
  QDSP_AMR_RX_SID_FIRST_V,
  QDSP_AMR_RX_SID_UPDATE_V,
  QDSP_AMR_RX_SID_BAD_V,
  QDSP_AMR_RX_NO_DATA_V,
  0
};

/* Array to convert rate from vocoder to QDSP value. */
LOCAL const uint16 vocvr_amr_rate_conv_table[] = {
  QDSP_AMR_RATE_475,
  QDSP_AMR_RATE_515,
  QDSP_AMR_RATE_590,
  QDSP_AMR_RATE_670,
  QDSP_AMR_RATE_740,
  QDSP_AMR_RATE_795,
  QDSP_AMR_RATE_1020,
  QDSP_AMR_RATE_1220,
  0,
  0
};

/* This macro converts an AMR frame header from a vocoder value to a QDSP
** value.
*/
#define VOCVR_AMR_RATE_CONV(header) \
              (vocvr_amr_mode_conv_table \
                                 [vocsup_native_header_to_amr_mode(header)] | \
               vocvr_amr_rate_conv_table \
                                 [vocsup_native_header_to_amr_rate(header)])

#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

#if defined(MSMAUD_VOC_GSM_COMMON)
#error code not present
#endif /* defined(MSMAUD_VOC_GSM_COMMON) */

#ifdef FEATURE_PHONE_VR
/* This flag indicates that a VR match operation is in progress and the DSP 
** is in VR MATCH state.  
**
** While a match operation is in progress, the DSP is not configured to 
** play voice prompts or capture.
*/
LOCAL boolean voc_match_cmd_in_progress = FALSE;

/* This function pointer is used to give a frame of captured packet data to 
** the appropriate function.
*/
LOCAL voc_endpt_cb_type voc_endpt_cb = (voc_endpt_cb_type) NULL;
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
/*===========================================================================

FUNCTION RECORD_NOTIFY

DESCRIPTION
  Perform client callback for record functions

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void record_notify (
  voc_rec_result_type *voc_rec_result
) {
  if((voc_rec_result->status == VOC_REC_STAT_DONE) ||
     (voc_rec_result->status == VOC_REC_STAT_AUTO_STOP) ||
     (voc_rec_result->status == VOC_REC_STAT_ERROR) ||
     (voc_rec_result->status == VOC_REC_STAT_BUFFER_ERROR) ||
     (voc_rec_result->status == VOC_REC_STAT_INVALID_PARAM) ||
     (voc_rec_result->status == VOC_REC_STAT_ANALOG_ERROR) ||
     (voc_rec_result->status == VOC_REC_STAT_NEW_CONFIG_ERROR)) {
    /* If recording is completed, then release the vocoder
    */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
    voc_release(VOC_ACQ_VOCREC);
    vocsup_rec_clear_buffers();
    MSG_MED("Stop recording",0,0,0);
  }

  if (voc_rec_cb != NULL) {
    voc_rec_cb (voc_rec_result);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_STOP_STATUS

DESCRIPTION
  This function stops voice recording with the indicated status.  This
  function will call back the function registered with voc_rec_start ()
  with the remainder of the recorded vocoder frames.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Via call back function:
    Ptr to header of forward link record buffer
    Ptr to header of reverse link record buffer
    Record status

SIDE EFFECTS
  Disable vocoder encoder interrupt out of a call.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_stop_status (voc_rec_status_type status)
{
  voc_rec_result_type    rec_result;

  /* Check to see if we are recording voice.
  */
  if ((voc_rec_cmd_in_progress == VOC_REC_REVERSE) ||
      (voc_rec_cmd_in_progress == VOC_REC_FORWARD) ||
      (voc_rec_cmd_in_progress == VOC_REC_BOTH)) {
    /* Indicate that we are done recording. */
    voc_rec_cmd_in_progress = VOC_REC_NONE;

    qdsp_cmd_end_rec();

    voc_restore_comfort_noise();

    /* Stop the recording and get the last buffer info. */
    vocsup_rec_sync_up (&rec_result);

    /* Call back with the last buffer info. */
    rec_result.status  = status;
    record_notify (&rec_result);
    vocsup_rec_free_buf(&rec_result);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
#ifdef FEATURE_VOICE_PLAYBACK
/*===========================================================================

FUNCTION PLAYBACK_NOTIFY

DESCRIPTION
  Perform client callback for play functions

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void playback_notify (
  voc_end_vpentry_return_type result,
  voc_pb_header_type          *buf_ptr,
  const void                  *client_data
) {
  /* If playback is completed, then release the vocoder
  */ 
  if((result == VOC_END_PB) || (result == VOC_END_ABORT_PB)) {
    /* Indicate that we are not playing a voice utterance */
    voc_play_back_status = VOC_PB_STATUS_IDLE;
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
    voc_release(VOC_ACQ_VOCVS);
  }
  
  if (vc_pb_notify_func != NULL) {
    vc_pb_notify_func (result, buf_ptr, client_data);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_ABORT_DFM

DESCRIPTION
  This function aborts a play back session in mid stream due to DFM transition.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vc_send_erasure_frame
  vc_abort_reverse_play

===========================================================================*/
void voc_pb_abort_dfm (void)
{

  /* Stop voice playback immediately */
  qdsp_cmd_end_vp(QDSP_CMD_VP_RX);
  qdsp_cmd_end_vp(QDSP_CMD_VP_TX);

  /* Clear the voice play back queues */
  vocsup_pb_clear_q (VOC_PB_DIR_BOTH);

  /*
  ** When notify VOC_END_ABORT_PB, voice service won't look at curr_pb_hdr.
  */
  if (curr_pb_hdr != (voc_pb_header_type *) NULL)
  {
    playback_notify (VOC_END_ABORT_PB, curr_pb_hdr, curr_pb_hdr->client_data);
  }
  else
  {
    playback_notify (VOC_END_ABORT_PB, curr_pb_hdr, (void *) NULL);
  }
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_VR_GOTO_DFM

DESCRIPTION
  Function to end all voice services when the vocoder transitions to DFM

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void voc_vr_goto_dfm () 
{
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */


#ifdef FEATURE_VOICE_PLAYBACK
#ifdef FEATURE_ACP 
#error code not present
#else
  voc_pb_abort_dfm ();
#endif /* FEATURE_ACP */
#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_VOICE_RECORD
#ifdef FEATURE_ACP
#error code not present
#else
  voc_rec_stop_status(VOC_REC_STAT_ANALOG_ERROR);
#endif /* FEATURE_ACP */
#endif /* FEATURE_VOICE_RECORD */

}

/*===========================================================================

FUNCTION voc_check_is_voc_image

DESCRIPTION
  This function verifies if the current image is a vocoder image.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  True if in a non analog voice call.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean voc_check_is_voc_image (
  voc_state_control_type *state
) 
{
  boolean ret_val = FALSE;  

  switch (state->current_image) {
#ifdef QDSP_IMAGE_VOC_DEFINED
    case QDSP_IMAGE_VOC:
#endif /* QDSP_IMAGE_VOC_DEFINED */
#ifdef QDSP_IMAGE_VOC_COMBO_DEFINED
    case QDSP_IMAGE_VOC_COMBO:
#endif /* QDSP_IMAGE_VOC_COMBO_DEFINED */
#ifdef FEATURE_AMR_FIXED
    case QDSP_IMAGE_AMR:
#endif
#ifdef QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED
    case QDSP_IMAGE_VOC_COMBO_VOIP:
#endif /* QDSP_IMAGE_VOC_COMBO_VOIP_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_DEFINED
    case QDSP_IMAGE_VOC_EVRC:
#endif /* QDSP_IMAGE_VOC_EVRC_DEFINED */
#ifdef QDSP_IMAGE_VOC_E90_DEFINED
    case QDSP_IMAGE_VOC_E90:
#endif /* QDSP_IMAGE_VOC_E90_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_E90_DEFINED
    case QDSP_IMAGE_VOC_EVRC_E90:
#endif /* QDSP_IMAGE_VOC_EVRC_E90_DEFINED */
#ifdef QDSP_IMAGE_AMR_DEFINED
    case QDSP_IMAGE_AMR:
#endif /* QDSP_IMAGE_AMR_DEFINED */
#ifdef QDSP_IMAGE_HR_DEFINED
    case QDSP_IMAGE_HR:
#endif /* QDSP_IMAGE_HR_DEFINED */
#ifdef QDSP_IMAGE_FR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_FR_DEFINED */
#ifdef QDSP_IMAGE_EFR_DEFINED
#error code not present
#endif /* QDSP_IMAGE_EFR_DEFINED */
#ifdef QDSP_IMAGE_VOC_QTV_DEFINED
    case QDSP_IMAGE_VOC_QTV:
#endif /* QDSP_IMAGE_VOC_QTV_DEFINED */
#ifdef QDSP_IMAGE_4GV_DEFINED
    case QDSP_IMAGE_4GV:
#endif /* QDSP_IMAGE_4GV_DEFINED */
#ifdef QDSP_IMAGE_VOC_MP3_DEFINED
    case QDSP_IMAGE_VOC_MP3:
#endif /* QDSP_IMAGE_VOC_MP3_DEFINED */
#ifdef QDSP_IMAGE_VOC_EVRC_MP3_DEFINED
    case QDSP_IMAGE_VOC_EVRC_MP3:
#endif /* QDSP_IMAGE_VOC_EVRC_MP3_DEFINED */
#ifdef QDSP_IMAGE_VOC_AMR_MP3_DEFINED
    case QDSP_IMAGE_AMR_MP3:
#endif /* QDSP_IMAGE_AMR_MP3_DEFINED */
#ifdef QDSP_IMAGE_HR_MP3_DEFINED
    case QDSP_IMAGE_HR_MP3:
#endif /* QDSP_IMAGE_HR_MP3_DEFINED */
#ifdef QDSP_IMAGE_EFR_MP3_DEFINED
    case QDSP_IMAGE_EFR_MP3:
#endif /* QDSP_IMAGE_EFR_MP3_DEFINED */
#ifdef QDSP_IMAGE_FR_MP3_DEFINED
    case QDSP_IMAGE_FR_MP3:
#endif /* QDSP_IMAGE_FR_MP3_DEFINED */
#ifdef QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED
    case QDSP_IMAGE_QVP_H263P3_EVRC:
#endif /* QDSP_IMAGE_QVP_H263P3_EVRC_DEFINED */
#ifdef QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_EVRC:
#endif /* QDSP_IMAGE_QVP_MPEG4_EVRC_DEFINED */
#ifdef QDSP_IMAGE_QVP_MPEG4_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_4GVWB_DEFINED
#error code not present
#endif
#ifdef QDSP_IMAGE_QVP_H263P3_AMR_DEFINED
    case QDSP_IMAGE_QVP_H263P3_AMR:
#endif /* QDSP_IMAGE_QVP_H263P3_AMR_DEFINED */
#ifdef QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED
    case QDSP_IMAGE_QVP_MPEG4_AMR:
#endif /* QDSP_IMAGE_QVP_MPEG4_AMR_DEFINED */
#ifdef QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED
    case QDSP_IMAGE_QCAMCORDER_COMBO:
#endif /*QDSP_IMAGE_QCAMCORDER_COMBO_DEFINED*/
      ret_val = TRUE;
      break;

    default:
      ret_val = FALSE;
      break;
  }
    
  return (ret_val);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_VOC_TO_QDSP_FORMAT

DESCRIPTION
  This function converts a vocoder frame byte to QDSP format.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  QDSP format

SIDE EFFECTS
  None.

===========================================================================*/
uint16 voc_voc_to_qdsp_format (
  byte                 rate,
  voc_capability_type  capability
) {
  uint16 ret_val;

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(capability == VOC_CAP_AMR) {
    /* For an AMR frame, curr_rate actually holds the entire packet header */
    ret_val = VOCVR_AMR_RATE_CONV(rate);
  } else
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#if defined(MSMAUD_VOC_AMR_WB_COMMON)
  if(capability == VOC_CAP_AMR_WB) {
    ret_val = (((uint16)(rate & 0xf0)  << 4) | (uint16)(rate & 0x0f));
  } else
#endif /* MSMAUD_VOC_AMR_WB_COMMON */
#if defined(MSMAUD_VOC_GSM_COMMON)
#error code not present
#endif /* defined(MSMAUD_VOC_GSM_COMMON) */
  {
    ret_val = vocvr_rate_conv[rate];
  }
  return(ret_val);
}

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
/*===========================================================================

FUNCTION VOC_QDSP_TO_VOC_FORMAT

DESCRIPTION
  This function converts a vocoder frame byte to QDSP format.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  QDSP format

SIDE EFFECTS
  None.

===========================================================================*/
uint8 voc_qdsp_to_voc_format (
  uint16               frame_rate,
  voc_capability_type  capability
) {
  uint8 ret_val;

#if defined(MSMAUD_VOC_AMR_COMMON) || defined (MSMAUD_VOC_GSM_COMMON)
  voc_rate_type       rate;
  voc_gsm_frame_type  mode;

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(capability == VOC_CAP_AMR) {
    /* Select AMR rate for packet header */
    qdsp_to_voc_gsm_frate(
               (qdsp_cmd_gsm_frame_rate_type)frame_rate & QDSP_AMR_CODEC_MODE_M,
               &rate);

    qdsp_to_voc_gsm_ftype(
                    (qdsp_cmd_gsm_frame_type)frame_rate | QDSP_AMR_CODEC_MODE_M,
                    &mode);

    ret_val = vocsup_amr_to_native_header(rate, mode);
  } else 
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
#endif /* defined(MSMAUD_VOC_AMR_COMMON) || defined (MSMAUD_VOC_GSM_COMMON) */
  {
    switch (frame_rate) {
      case QDSP_RATE_FULL_V:
        ret_val = VOC_1_RATE;
        break;

      case QDSP_RATE_HALF_V:
        ret_val = VOC_2_RATE;
        break;

      case QDSP_RATE_QUARTER_V:
        ret_val = VOC_4_RATE;
        break;

      case QDSP_RATE_EIGHTH_V:
        ret_val = VOC_8_RATE;
        break;

      case QDSP_RATE_BLANK_V:
        ret_val = VOC_0_RATE;
        break;

      case QDSP_RATE_ERASURE_V:
        ret_val = VOC_ERASURE;
        break;

      default:
        MSG_HIGH ("voc_rec_frame: Invalid frame rate: %x",frame_rate,0,0);
        ret_val = VOC_ERR_RATE;
        break;
    }
  }
  return (ret_val);
}
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_RX_FRAME

DESCRIPTION
  This function gets one frame from the forward link queue.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  TRUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_get_rx_frame (
  uint16 *rate, 
  uint16 **frame
) {
  byte               curr_rate;
  vsp_get_frame_type get_frame_ret = SAME_PROMPT;      /* do nothing */

  if (vocsup_pb_q_size (VOC_PB_DIR_FORWARD) > 0)
  {
    /* Get a play back frame.
    */
    get_frame_ret = vocsup_pb_get_frame (
                      &curr_rate,
                      (byte**) frame,
                      &curr_pb_hdr,
                      VOC_PB_DIR_FORWARD);

    *rate = voc_voc_to_qdsp_format(curr_rate, curr_pb_hdr->pb_voc_capability);

    if (get_frame_ret == QUEUE_EMPTY)
    {
      qdsp_cmd_end_vp(QDSP_CMD_VP_RX);

      if (vocsup_pb_q_size (VOC_PB_DIR_BOTH) > 0) 
      {
        playback_notify (VOC_END_Q_NOT_EMPTY, curr_pb_hdr, 
                                   curr_pb_hdr->client_data);
      }
      else /* if (vocsup_pb_q_size (VOC_PB_DIR_BOTH ) == 0) */
      {
        playback_notify (VOC_END_PB, curr_pb_hdr, curr_pb_hdr->client_data);
      }
    } else if (get_frame_ret == NEXT_PROMPT) {
      playback_notify (VOC_END_Q_NOT_EMPTY, curr_pb_hdr, 
                                 curr_pb_hdr->client_data);
    }
  }
  return (TRUE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_GET_TX_FRAME

DESCRIPTION
  This function gets one frame from the reverse link queue.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  TRUE if this prompt is played to both links.

SIDE EFFECTS
  None.

===========================================================================*/
boolean voc_get_tx_frame (
  uint16 *rate,
  uint16 **frame
) {
  byte               curr_rate;
  vsp_get_frame_type get_frame_ret = SAME_PROMPT;      /* do nothing */

  if (vocsup_pb_q_size (VOC_PB_DIR_REVERSE) > 0)
  {
    /* Get a play back frame.
    */
    get_frame_ret = vocsup_pb_get_frame (
                      &curr_rate,
                      (byte**) frame,
                      &curr_pb_hdr,
                      VOC_PB_DIR_REVERSE);

    *rate = voc_voc_to_qdsp_format(curr_rate, curr_pb_hdr->pb_voc_capability);

    if (get_frame_ret == QUEUE_EMPTY)
    {
      qdsp_cmd_end_vp(QDSP_CMD_VP_TX);
    
      if (vocsup_pb_q_size (VOC_PB_DIR_BOTH) > 0) 
      {
        playback_notify (VOC_END_Q_NOT_EMPTY, curr_pb_hdr, 
                                   curr_pb_hdr->client_data);
      }
      else /* if (vocsup_pb_q_size (VOC_PB_DIR_BOTH ) == 0) */
      {
        playback_notify (VOC_END_PB, curr_pb_hdr, curr_pb_hdr->client_data);
      }
    } else if (get_frame_ret == NEXT_PROMPT) {
      playback_notify (VOC_END_Q_NOT_EMPTY, curr_pb_hdr, 
                                 curr_pb_hdr->client_data);
    }
  }
  return (curr_pb_hdr->pb_direction == VOC_PB_DIR_BOTH);
}
/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_GET_CAP

DESCRIPTION
  This function converts the cap. of the current prompt to 
  qdsp_cmd_second_dec_config_type for the second decoder

DEPENDENCIES

RETURN VALUE
  QDSP_CMD_SECOND_DEC_CONFIG_NONE 
  QDSP_CMD_SECOND_DEC_CONFIG_13K 
  QDSP_CMD_SECOND_DEC_CONFIG_EVRC
  QDSP_CMD_SECOND_DEC_CONFIG_AMR 
  QDSP_CMD_SECOND_DEC_CONFIG_EFR
  QDSP_CMD_SECOND_DEC_CONFIG_FR
SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
qdsp_cmd_second_dec_config_type voc_pb_get_cap(voc_capability_type cap)
{
  qdsp_cmd_second_dec_config_type config = QDSP_CMD_SECOND_DEC_CONFIG_NONE;
  switch (cap) {
    case VOC_CAP_IS733:
      config = QDSP_CMD_SECOND_DEC_CONFIG_13K;
      break;
    case VOC_CAP_IS127:
      config = QDSP_CMD_SECOND_DEC_CONFIG_EVRC;
      break;
    case VOC_CAP_AMR:
      config = QDSP_CMD_SECOND_DEC_CONFIG_AMR;
      break;
    case VOC_CAP_GSM_EFR:
      config = QDSP_CMD_SECOND_DEC_CONFIG_EFR;
      break;
    case VOC_CAP_GSM_FR:
      config = QDSP_CMD_SECOND_DEC_CONFIG_FR;
      break;
    case VOC_CAP_GSM_HR:
      config = QDSP_CMD_SECOND_DEC_CONFIG_HR;
      break;
  }
  return config;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_ADD_Q_FWD

DESCRIPTION
  This function adds one entry to the forward link vocoder play back queue.  
  Playing a voice prompt on the forward link is supported in or out of a call.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  VOC_PB_OK,
  VOC_PB_FULL,
  VOC_PB_ERR

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_add_pbq_return_type voc_pb_add_q_fwd (
  /* pointer to voice utterance header */
  voc_pb_header_type *voc_pb_header_ptr
)
{
  voc_add_pbq_return_type return_val = VOC_PB_OK;
  qdsp_cmd_second_dec_config_type dec_config = QDSP_CMD_SECOND_DEC_CONFIG_NONE;
  qdsp_cmd_vp_mux_type incall_mux = QDSP_CMD_VP_MUX_NONE;

  /* Return if the forward play back queue is full. */
  if (vocsup_pb_q_size (VOC_PB_DIR_FORWARD) == VSP_PBQ_MAX)
  {
    return (VOC_PB_FULL);
  }

  /* Add entry to queue and initiate play back */
  if (return_val == VOC_PB_OK)
  {
    vocsup_pb_add_entry (voc_pb_header_ptr, VOC_PB_DIR_FORWARD);

    /* Indicate that we are playing a voice utterance */
    voc_play_back_status = VOC_PB_STATUS_PLAYING;
    if ((voc_pb_header_ptr->in_call == TRUE) && (voc_pb_incall_mux_possible))
    {
      incall_mux = QDSP_CMD_VP_MUX_RX;
      dec_config = voc_pb_get_cap(voc_pb_header_ptr->pb_voc_capability);
    }

    qdsp_cmd_start_vp(QDSP_CMD_VP_RX, voc_get_rx_frame, incall_mux, dec_config);

  } /* end if (return_val == VOC_PB_OK) */  

  return (return_val);

} /* end voc_pb_add_q_fwd */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_ADD_Q_REV

DESCRIPTION
  This function adds one entry to the reverse link vocoder play back queue.
  Playing a voice prompt on the reverse link is only supported in a call.
  The format (service option) of the voice prompt has to match the format of
  the call.  This check has been performed in voc_pb_add_q ().

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  VOC_PB_OK,
  VOC_PB_FULL,
  VOC_PB_ERR

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_add_pbq_return_type voc_pb_add_q_rev (
  /* pointer to voice utterance header */
  voc_pb_header_type *voc_pb_header_ptr
)
{
  voc_add_pbq_return_type return_val = VOC_PB_OK;
  qdsp_cmd_second_dec_config_type dec_config = QDSP_CMD_SECOND_DEC_CONFIG_NONE;
  qdsp_cmd_vp_mux_type incall_mux = QDSP_CMD_VP_MUX_NONE;

  /* Return if the reverse play back queue is full. */
  if (vocsup_pb_q_size (VOC_PB_DIR_REVERSE) == VSP_PBQ_MAX)
  {
    return (VOC_PB_FULL);
  }

  /* Add entry to queue */
  vocsup_pb_add_entry (voc_pb_header_ptr, VOC_PB_DIR_REVERSE);

  /* Indicate that we are playing a voice utterance */
  voc_play_back_status = VOC_PB_STATUS_PLAYING;
  if ((voc_pb_header_ptr->in_call == TRUE) && (voc_pb_incall_mux_possible))
  {
    incall_mux = QDSP_CMD_VP_MUX_TX;
    dec_config = voc_pb_get_cap(voc_pb_header_ptr->pb_voc_capability);
  }

  qdsp_cmd_start_vp(QDSP_CMD_VP_TX, voc_get_tx_frame, incall_mux, dec_config);

  return (return_val);

} /* end voc_pb_add_q_rev */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_ADD_Q

DESCRIPTION
  This function adds one entry to the appropriate vocoder play back queue.
  The vocoder driver maintains two queues, one for the forward link and one for
  the reverse link.

  Each entry contains a voice utterance header.  The header contains information
  used by the vocoder driver to play a voice utterance.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  VOC_PB_OK,
  VOC_PB_FULL,
  VOC_PB_ERR

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
voc_add_pbq_return_type voc_pb_add_q (
  /* pointer to voice utterance header */
  voc_pb_header_type *voc_pb_header_ptr,
  /* pointer to client data associated with utterance header */
  const void         *client_data
)
{
  voc_add_pbq_return_type return_val = VOC_PB_OK;
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  if (voc_pb_header_ptr != (voc_pb_header_type *) NULL) {
    voc_pb_header_ptr->client_data = client_data;
  }

  if (
      /* Reject bad pointer */
      (voc_pb_header_ptr == (voc_pb_header_type *) NULL) ||

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
      
#ifdef FEATURE_PHONE_VR
      /* Reject if in Mode B/C VR Match state */
      (voc_match_cmd_in_progress == TRUE )  ||
#endif /* FEATURE_PHONE_VR */

#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#else
#ifdef FEATURE_ACP
#error code not present
#endif
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */

      /* Reject if capability is not supported */
      (!(voc_check_pb_capability(voc_pb_header_ptr->pb_voc_capability))) ||

      /* Reject zero-frame request */
      (voc_pb_header_ptr->pb_num_frames == 0)

     )
  {
    return_val = VOC_PB_ERR;
  }
  else if (
      /* Reject request if phone is in a CDMA call and the service option of 
      ** utterance is different than the current service option.
      */
      ((state != NULL) && (voc_check_in_call(state->acquire_mask)) &&
       (voc_pb_header_ptr->pb_voc_capability != state->config) && 
       (voc_pb_header_ptr->pb_direction != VOC_PB_DIR_FORWARD))
    )
  {
    MSG_HIGH ("Voc playback different service option", 0, 0, 0);

    return_val = VOC_PB_DIFF_SO;
  } else {
    /* When starting playback, acquire and config the vocoder.
    */
    if ((state != NULL) && voc_check_in_call(state->acquire_mask)) 
    {
      if (voc_pb_header_ptr->pb_voc_capability != state->config)
      {
        /* We are doing a playback in forward link (rx) but the service
        options of the call and the playback are not the same so we cannot
        do an incall mux */
        voc_pb_incall_mux_possible = FALSE;
      }
      else
      {
        voc_pb_incall_mux_possible = TRUE;
      }
    }
    if (voc_play_back_status == VOC_PB_STATUS_IDLE) 
    {
      voc_acquire(VOC_ACQ_VOCVS, NULL);
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
      if (voc_config(VOC_ACQ_VOCVS, voc_pb_header_ptr->pb_voc_capability) !=
          VOC_DONE_S) {
        playback_notify (VOC_END_PB, voc_pb_header_ptr, client_data);
        return(VOC_PB_ERR);
      }
    } else {
      /* Reject request if the service option of utterance is different than 
      ** the current playing utterance service option.
      */
      /* Get pointer to voice utterance currently playing.
      */
      curr_pb_hdr = vocsup_pb_get_hdr (VOC_PB_DIR_BOTH);
      if((curr_pb_hdr != (voc_pb_header_type *) NULL) &&
         (voc_pb_header_ptr->pb_voc_capability != 
                                      curr_pb_hdr->pb_voc_capability))
      {
         MSG_HIGH ("Voc playback different service option", 0, 0, 0);

         return_val = VOC_PB_DIFF_SO;
      }
    }
    
    /* Either add the request to the forward link playback queue or the reverse
    ** link playback queue.
    */
    switch (voc_pb_header_ptr->pb_direction)
    {
      case VOC_PB_DIR_FORWARD:

        return_val = voc_pb_add_q_fwd (voc_pb_header_ptr);
        break;

      case VOC_PB_DIR_REVERSE:
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#else
        /* Reject if phone is not in a CDMA call. */
        if ((state != NULL) && !(voc_check_in_call(state->acquire_mask))) {
          return_val = VOC_PB_ERR;
        }
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
        else {
          return_val = voc_pb_add_q_rev (voc_pb_header_ptr);
        }
        break;

      case VOC_PB_DIR_BOTH:

        /* Reject if phone is not in a call. */
        if ((state != NULL) && !(voc_check_in_call(state->acquire_mask)) &&
            !(state->acquire_mask & ((word) 1 << VOC_ACQ_ACP)))
        {
          return_val = VOC_PB_ERR;
        }
        /* Reject if forward link play back queue not empty, can't synchronize
        ** playing both links.
        */
        else if (vocsup_pb_q_size (VOC_PB_DIR_FORWARD) > 0)
        {
          return_val = VOC_PB_ERR;
        }
        else
        {
          /* If the play direction is to both links, the convention is the
          ** utterance header will be added to the reverse link queue and this
          ** header will be accessed to be played on both links.
          */
          return_val = voc_pb_add_q_rev (voc_pb_header_ptr);
        }
        break;

      case VOC_MAX_PB_DIR_TYPE:
      default:

        return_val = VOC_PB_ERR;
        break;

    } /* end switch */
  } /* end error checks */

  /* If nothing is playing, the request was rejected and there is nothing
  ** queued to play.  This particular case requires a call back to SND with 
  ** the indication that queue is empty in addition to returning ERROR to SND.
  **
  ** VOC_FORCE_END_PB or VOC_CHECK_Q_STAT is the same since nothing is playing.
  */
  if (voc_play_back_status == VOC_PB_STATUS_IDLE)
  {
    playback_notify (VOC_END_PB, voc_pb_header_ptr, client_data);
  }
  else
  {
    /* Current pb hdr is only updated after the first decoder ISR.  Since some
    ** operation requires it to be set immediately after a play request is added
    ** to the pb queue, make sure to update it here to avoid having the pointer
    ** pointing to the wrong header.
    */
    curr_pb_hdr = vocsup_pb_get_hdr (VOC_PB_DIR_BOTH);
  }

  return (return_val);

}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_GET_STATUS

DESCRIPTION
  This function indicates if a play back session is in progress.  It returns
  idle or playing.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  VOC_PB_STATUS_IDLE
  VOC_PB_STATUS_PLAYING

SIDE EFFECTS
  None

===========================================================================*/
voc_get_pbs_return_type voc_pb_get_status (void)
{
  return (voc_play_back_status);
} /* end voc_pb_get_status */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_END

DESCRIPTION
  This function stops a play back session by clearing the voice play back
  queues and disabling the vocoder ISR.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  Clear the voice play back queue.
  May call voc_config ().

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_pb_end (void)
{
} /* end voc_pb_end */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_ABORT

DESCRIPTION
  This function aborts a play back session in mid stream.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vc_send_erasure_frame
  vc_abort_reverse_play

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_pb_abort (void)
{
  
  /* Stop voice playback immediately */
  qdsp_cmd_end_vp(QDSP_CMD_VP_RX);
  qdsp_cmd_end_vp(QDSP_CMD_VP_TX);

  /* Clear the voice play back queues */
  vocsup_pb_clear_q (VOC_PB_DIR_BOTH);
  
  /* 
  ** When notify VOC_END_PB, voice service won't look at curr_pb_hdr.
  */
  if (curr_pb_hdr != (voc_pb_header_type *) NULL)
  {
    playback_notify (VOC_END_PB, curr_pb_hdr, curr_pb_hdr->client_data);
  }
  else
  {
    playback_notify (VOC_END_PB, curr_pb_hdr, (void *) NULL);
  }

  voc_pb_incall_mux_possible = FALSE;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_PAUSE

DESCRIPTION
  This function pauses voice prompt play back by sending erasure frames to
  the vocoder without disturbing the play back queue.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Send erasure frames to vocoder until play back is resumed.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_pb_pause (void)
{
  qdsp_cmd_pause_pb ();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pb_resume

DESCRIPTION
  This function resumes voice play back.

  If the play back was paused via a call to voc_pb_pause (), this
  function clears the flag which causes erasure frames to be sent to the
  vocoder.  Pausing play back via voc_pb_pause does not disable the
  decoder ISR. This function check for a change in
  the vocoder configuration.  If the new configuration can support the
  current voice play back, the play back is resumed.  Otherwise, play
  back is cancelled via a call back to SND.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  May re-configure vocoder to different voice mode.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_pb_resume (boolean pause)
{
  voc_capability_type utt_config;
  voc_state_control_type  *state;
  
  state = voc_state_get_state_data();

  /* Check to see if we are playing a voice utterance.
  */
  if (voc_play_back_status == VOC_PB_STATUS_PLAYING)
  {
    /* If the play back was paused via a call to voc_pb_pause (), this 
    ** function clears the flag which causes erasure frames to be sent to the
    ** vocoder.  Pausing play back via voc_pb_pause does not disable the 
    ** decoder ISR.  
    **
    ** In case the vocoder config was changed while the pause is in place,
    ** still check the vocoder config after resetting pause flag.
    */
 
    /* Get pointer to voice utterance currently playing.
    */
    curr_pb_hdr = vocsup_pb_get_hdr (VOC_PB_DIR_BOTH);
    if (curr_pb_hdr != (voc_pb_header_type *) NULL)
    {
       
      /* Get the voice utterance configuration.
      */
      utt_config  = curr_pb_hdr->pb_voc_capability;
      
      /* Check if the service option has changed.
      */
      if (utt_config != state->config) 
      {
        /* Terminate play back if phone is in a call.
        */
#if defined (FEATURE_ACP) && defined (FEATURE_VOICE_RECORD)
#error code not present
#else
        if(((voc_check_in_call(state->acquire_mask)) &&
            (utt_config != state->config) &&
            (curr_pb_hdr->pb_direction != VOC_PB_DIR_FORWARD)) ||
           (state->acquire_mask & ((word) 1 << VOC_ACQ_ACP)))
#endif /* FEATURE_ACP && FEATURE_VOICE_RECORD */
        {
          MSG_HIGH ("In call play resume failed SO=%d, Old=%d", 
                             utt_config, state->config, 0);

          /* Stop voice playback immediately */
          qdsp_cmd_end_vp(QDSP_CMD_VP_RX);
          qdsp_cmd_end_vp(QDSP_CMD_VP_TX);
        
          /* Clear the voice play back queues */
          vocsup_pb_clear_q (VOC_PB_DIR_BOTH);
          
          playback_notify (VOC_END_PB, curr_pb_hdr, curr_pb_hdr->client_data);
        }
        else
        {
          /* Only get here if we are playing an utterance on the forward link
          ** when the phone is not in a call.  It is not possible to playbak
          ** on the reverse link when the phone is not in a call.
          */
          MSG_HIGH ("Resume Playback, Config =%d, Old=%d", 
                            utt_config, state->config, 0);
          
          /* Resume voice prompt if config is successful.
          */
          if (voc_config (VOC_ACQ_VOCVS, utt_config) != VOC_DONE_S)
          {
            MSG_HIGH ("Resume Voice Playback, Voc Config failed", 0, 0, 0);
          
            /* Stop voice playback immediately */
            qdsp_cmd_end_vp(QDSP_CMD_VP_RX);
            qdsp_cmd_end_vp(QDSP_CMD_VP_TX);
          
            /* Clear the voice play back queues */
            vocsup_pb_clear_q (VOC_PB_DIR_BOTH);
  
            /* Fail configuration, so terminate voice play back by
            ** calling back to SND with VOC_END_PB parameter.
            */
            playback_notify (VOC_END_PB, curr_pb_hdr, curr_pb_hdr->client_data);
          }
          else
          {
            if(pause == FALSE)
            {
              qdsp_cmd_resume_pb ();
            }
          }
      }
      }
      else
      {
        /* We resume the playback and no one changed the config since the 
        ** vocoder was reset.
        */
        if(pause == FALSE)
        {
          qdsp_cmd_resume_pb ();
        }
      }
    }
    else
    {
      MSG_HIGH ("Play resume failed", 0, 0, 0);

      /* Stop voice playback immediately */
      qdsp_cmd_end_vp(QDSP_CMD_VP_RX);
      qdsp_cmd_end_vp(QDSP_CMD_VP_TX);
    
      /* Clear the voice play back queues */
      vocsup_pb_clear_q (VOC_PB_DIR_BOTH);
      
      playback_notify (VOC_END_PB, (voc_pb_header_type *) NULL, (void *) NULL);
    } /* end if (curr_pb_hdr != (voc_pb_header_type *) NULL) */
  } else if (voc_play_back_status == VOC_PB_STATUS_IDLE) {
    vc_pb_notify_func (VOC_END_PB, (voc_pb_header_type *) NULL, (void *) NULL); 
  } 
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_PB_REGISTER

DESCRIPTION
  This function registers a call back function for the vocoder driver to call
  back at the end of playing a voice utterance.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vc_pb_notify_func

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_pb_register (voc_pb_notify_func_ptr_type voc_func)
{
  vc_pb_notify_func = voc_func;
} /* end vocm2_pb_register */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_RX_MUTE_CTL

DESCRIPTION
  Mute the RX path if voice prompt queue for forward link is empty;
  otherwise unmute.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rx_vp_mute_ctl ( void )
{
  /* If playback is active and playing to forward link, unmute the earpiece.
  */
  if ((voc_play_back_status == VOC_PB_STATUS_IDLE) ||
      (curr_pb_hdr->pb_direction == VOC_PB_DIR_REVERSE)) {
    (void) voc_rx_mute_cmd(TRUE);
  } else {
    (void) voc_rx_mute_cmd(FALSE);
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
#ifdef FEATURE_PHONE_VR
/*===========================================================================

FUNCTION VOC_SND_ACTIVE_STATUS

DESCRIPTION
  This function is called by the Sound driver everytime Sound starts or
  stops playing a tone.  This function allows the Vocoder driver to handle
  talk over prompt feature in capture mode.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_snd_active_status (voc_snd_active_status_type snd_play_stat)
{
   /* inform qdsp driver sound status */
 
   qdsp_cmd_snd_status(snd_play_stat);

} /* end voc_snd_active_status */

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
/*===========================================================================

FUNCTION VOC_REC_PUT_BUF

DESCRIPTION
  This function accepts a buffer to be used for recording voice memo.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_put_buf (
  uint8  *buffer,
  uint32 num_bytes
) {
  vocsup_rec_put_buf(buffer, num_bytes);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_FRAME_PROCESSING

DESCRIPTION
  This function accepts the recorded frame and saves it in a buffer.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void voc_rec_frame_processing (
  voc_rec_type dir,
  uint16       frame_rate,
  uint8        *frame
) {
  voc_rec_result_type rec_result;
  byte                packet_format;

  packet_format = voc_qdsp_to_voc_format(frame_rate, voc_rec_capability);

  /* If a buffer is ready, call back with the buffer info.
  */
  vocsup_rec_frame(dir, packet_format, frame, &rec_result);

  if((voc_rec_capability == VOC_CAP_IS127) ||
#ifdef MSMAUD_VOC_4GV
     (voc_rec_capability == VOC_CAP_4GV_NB)   ||
#endif
#ifdef MSMAUD_VOC_4GV_WB
     (voc_rec_capability == VOC_CAP_4GV_WB)   ||
#endif
     (voc_rec_capability == VOC_CAP_IS733)) {

    if(voc_rec_auto_stop_frame != 0) {
      if(packet_format != VOC_ERASURE) {
        if((packet_format == VOC_1_RATE) || (packet_format == VOC_2_RATE) ||
           (packet_format == VOC_4_RATE)) {
          voc_rec_noise_counter++;
          voc_rec_silence_counter++;
          if(voc_rec_noise_counter >= 10) {
            voc_rec_noise_counter   = 0;
            voc_rec_silence_counter = 0;
          }
        } else {
          voc_rec_silence_counter++;
          voc_rec_noise_counter = 0;
        }

        if(voc_rec_silence_counter >= voc_rec_auto_stop_frame) {
          rec_result.status = VOC_REC_STAT_AUTO_STOP;
        }
      } else {
        voc_rec_silence_counter = 0;
      }
    }
  }

  if((rec_result.status == VOC_REC_STAT_ERROR) ||
     (rec_result.status == VOC_REC_STAT_BUFFER_ERROR) ||
     (rec_result.status == VOC_REC_STAT_AUTO_STOP) ) {
    voc_rec_stop_status(rec_result.status);
  } else if (rec_result.status != VOC_REC_STAT_NOT_READY) {
    /* Notify the requester via a call back.
    */
    record_notify (&rec_result);
    vocsup_rec_free_buf(&rec_result);
  }
} /* end voc_rec_frame_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_DO_REC_START

DESCRIPTION
  This function does the record start process after vocoder has been acquired.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_do_rec_start ( void )
{
  qdsp_cmd_rec_dir_enum_type dsp_path;
  voc_rec_result_type        rec_result;
  voc_state_control_type *state;
#if defined(MSMAUD_VOC_AMR_COMMON) || defined(MSMAUD_VOC_GSM_COMMON)
  qdsp_cmd_gsm_frame_rate_type min_amr_rate = QDSP_AMR_RATE_1220;
  qdsp_cmd_gsm_frame_rate_type max_amr_rate = QDSP_AMR_RATE_1220;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */

  rec_result.status  = VOC_REC_STAT_SUCCESS;
  rec_result.fwd_ptr = (voc_rec_header_type *) NULL;
  rec_result.rev_ptr = (voc_rec_header_type *) NULL;

  state = voc_state_get_state_data();

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(voc_rec_parm_config.settings.capability == VOC_CAP_AMR) {
    /* only initialize AMR rate and DTX mode if not in call */
    if (voc_rec_parm_config.settings.max_rate
        == voc_rec_parm_config.settings.min_rate)
    {
      voc_init_amr_tx_rate(voc_rec_parm_config.settings.max_rate);

      if (voc_rec_parm_config.dtx_enable == TRUE) {
        voc_set_gsm_enc_dtx(VOC_GSM_DTX_ON);
      } else {
        voc_set_gsm_enc_dtx(VOC_GSM_DTX_OFF);
      }
    }
  } else
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
  {
    voc_frame_cmd (voc_rec_parm_config.settings.max_rate,
                                voc_rec_parm_config.settings.min_rate);

    /* Set 1X TX rate factor to default value when not in call */
    if (!(voc_check_in_call(state->acquire_mask)) ) {
      voc_data_set_1x_tx_rate_factor(VOC_1X_TX_RATE_FACTOR_DEFAULT);
    }
  }

  if(voc_rec_cmd_in_progress == VOC_REC_REVERSE) {
    dsp_path = QDSP_CMD_REC_REVERSE;
  } else if(voc_rec_cmd_in_progress == VOC_REC_FORWARD) {
    dsp_path = QDSP_CMD_REC_FORWARD;
  } else {
    dsp_path = QDSP_CMD_REC_BOTH;
  }

#if defined(MSMAUD_VOC_AMR_COMMON)
  if(voc_rec_parm_config.settings.capability == VOC_CAP_AMR) {
    voc_to_qdsp_gsm_frate(voc_rec_parm_config.settings.min_rate,&min_amr_rate);
    voc_to_qdsp_gsm_frate(voc_rec_parm_config.settings.max_rate,&max_amr_rate);
  
    qdsp_cmd_start_rec(dsp_path, voc_rec_frame, 
                                    (uint16)min_amr_rate, (uint16)max_amr_rate);
  } else
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#if defined(MSMAUD_VOC_GSM_COMMON)
#error code not present
#endif /* defined(MSMAUD_VOC_GSM_COMMON) */
  {
    qdsp_cmd_start_rec(dsp_path, voc_rec_frame,
                       voc_rec_parm_config.settings.min_rate, 
                       voc_rec_parm_config.settings.max_rate);
  }

  /* Callback with SUCCESS status, recording has started. */
  record_notify (&rec_result);

#ifdef FEATURE_MM_REC
  vocsup_rec_do_av_sync();
#endif

  voc_rec_started = TRUE;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_ACQUIRE_CB

DESCRIPTION
  This function is used as the callback function for the voc acquire
  during recording. It is used mainly to ensure DSP is in proper
  state before starting recording.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void voc_rec_acquire_cb
(
  voc_inform_type status
    /* New Status of Vocoder */
)
{
  if((!voc_rec_started) && ((status == VOC_INFORM_IDLE) || 
                            (status == VOC_INFORM_VOCODER) 
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#ifdef FEATURE_GRAPH_AUDIO
                             || (status == VOC_INFORM_GRAPH_AUDIO)
#endif /* FEATURE_GRAPH_AUDIO */
                           )) {
    voc_do_rec_start();
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_START

DESCRIPTION
  This function starts voice recording.  While voice recording is in
  progress, the vocoder driver calls back everytime the record interval
  is reached.  The call back function returns pointer information to the
  recorded frames.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  May do a voc_config.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_start (
  /* Call back function. */
  voc_rec_cb_type          cb,

  /* Recording parameters */
  voc_rec_parm_type        *voc_rec_parm
)
{
  boolean                    change_config = FALSE;
  voc_rec_result_type        rec_result;
  voc_state_control_type     *state;

  ASSERT (cb != (voc_rec_cb_type) NULL);

  state = voc_state_get_state_data();

  /* Save call back function. */
  voc_rec_cb = cb;
  
  rec_result.status  = VOC_REC_STAT_SUCCESS;
  rec_result.fwd_ptr = (voc_rec_header_type *) NULL;
  rec_result.rev_ptr = (voc_rec_header_type *) NULL;

  if (
      /* Reject request if already recording */
      (voc_rec_cmd_in_progress != VOC_REC_NONE) ||
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

      /* Reject unsupported recording types */
      /* If in a call, recording will not be rejected even if requested
         format is not supported. Recording happens in the format
         same as that of the active voice call.
      */
      ((voc_rec_parm->rec_type != VOC_REC_FORWARD) &&
       (voc_rec_parm->rec_type != VOC_REC_REVERSE) &&
       (voc_rec_parm->rec_type != VOC_REC_BOTH))||
       (!((voc_check_in_call(state->acquire_mask) && 
       voc_check_is_voc_image(state))||
       voc_check_rec_capability(voc_rec_parm->settings.capability))))
  {
    rec_result.status  = VOC_REC_STAT_INVALID_PARAM;
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  } else {
    /* Acquire the vocoder at the start of recording
    */
    voc_acquire(VOC_ACQ_VOCREC, voc_rec_acquire_cb);
    
    /* Copy the command data in case it needs to change
    */
    voc_rec_parm_config = *voc_rec_parm;
    
    /* Determine the recording configuration.  If in call, use the call's
    ** service option. If not in call, use the command's service option.
    */
    if (voc_check_in_call(state->acquire_mask) &&
        voc_check_is_voc_image(state)) {

      /* If we are currently in QCamcorder image, fixed rate should be 
       * used for recording.
       */
      if(voc_rec_parm->settings.capability != state->config) {
        if((state->config != VOC_CAP_IS733) &&
           (state->config != VOC_CAP_IS127)
#ifdef MSMAUD_VOC_4GV
           && (state->config != VOC_CAP_4GV_NB)
#endif
#ifdef MSMAUD_VOC_4GV_WB
           && (state->config != VOC_CAP_4GV_WB)
#endif
#if defined(MSMAUD_VOC_AMR_COMMON)
           && (state->config != VOC_CAP_AMR)
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
           )
        {
          rec_result.status = VOC_REC_STAT_NEW_CONFIG_ERROR;
        } else {
          voc_rec_parm_config.settings.capability = state->config;
          change_config                           = TRUE;
        }
      }
      switch(voc_rec_parm_config.settings.capability) {
#if defined(MSMAUD_VOC_AMR_COMMON)
        case VOC_CAP_AMR:
          voc_rec_parm_config.settings.max_rate     = VOC_AMR_RATE_1220;
          voc_rec_parm_config.settings.min_rate     = VOC_AMR_RATE_475;
          break;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
        default:
          voc_rec_parm_config.settings.max_rate     = VOC_1_RATE;
          voc_rec_parm_config.settings.min_rate     = VOC_8_RATE;
	      voc_rec_parm_config.settings.frame_format = VOC_PB_NATIVE_QCP;										
          break;
      }
    }
  }

  if(rec_result.status == VOC_REC_STAT_SUCCESS) {
    /* Now configure the vocoder
    */
#ifdef FEATURE_ACP 
#error code not present
#endif /* FEATURE_ACP */
    if ( voc_config (VOC_ACQ_VOCREC, voc_rec_parm_config.settings.capability)
         != VOC_DONE_S ) {
      rec_result.status  = VOC_REC_STAT_ERROR;
      MSG_HIGH ("Start Voice Recording, Voc Config failed", 0, 0, 0);
    }
  }

  if(rec_result.status != VOC_REC_STAT_SUCCESS) {
    record_notify (&rec_result);
  } else {
    /* Indicate what type of recording is in progress to avoid accepting
    ** another record command.
    */
    voc_rec_cmd_in_progress = voc_rec_parm_config.rec_type;

    /* Save vocoder configuration in case it's necessary to reconfigure the
    ** vocoder.
    */
    voc_rec_capability = voc_rec_parm_config.settings.capability;
    
    /* Pass recording info down to the next level. */
    vocsup_rec_start (&voc_rec_parm_config);

    voc_rec_auto_stop_frame = voc_rec_parm_config.auto_stop_ms / 20;
    voc_rec_silence_counter = 0;
    voc_rec_noise_counter   = 0;

    /* Call back if service option is different. */
    if(change_config) {
      if(voc_rec_parm_config.settings.capability == VOC_CAP_IS733) {
        rec_result.status  = VOC_REC_STAT_INFORM_13K;
      } else if(voc_rec_parm_config.settings.capability == VOC_CAP_IS127) {
        rec_result.status  = VOC_REC_STAT_INFORM_EVRC;
#ifdef MSMAUD_VOC_4GV
      } else if(voc_rec_parm_config.settings.capability == VOC_CAP_4GV_NB) {
        rec_result.status  = VOC_REC_STAT_INFORM_4GV_NB;
#endif
#ifdef MSMAUD_VOC_4GV_WB
      } else if(voc_rec_parm_config.settings.capability == VOC_CAP_4GV_WB) {
        rec_result.status  = VOC_REC_STAT_INFORM_4GV_WB;
#endif
#if defined(MSMAUD_VOC_AMR_COMMON)
      } else if(voc_rec_parm_config.settings.capability == VOC_CAP_AMR) {
        rec_result.status  = VOC_REC_STAT_INFORM_AMR;
#endif /* defined(MSMAUD_VOC_AMR_COMMON) */
#ifdef MSMAUD_VOC_GSM_COMMON
#error code not present
#endif /* MSMAUD_VOC_GSM_COMMON */
      }
      record_notify (&rec_result);
    }

    if((voc_state_get_inform_val() == VOC_INFORM_IDLE) ||
       (voc_state_get_inform_val() == VOC_INFORM_VOCODER)
#ifdef FEATURE_ACP
#error code not present
#endif
#ifdef FEATURE_GRAPH_AUDIO
       || (voc_state_get_inform_val() == VOC_INFORM_GRAPH_AUDIO)
#endif
      ) {
      voc_do_rec_start();
    } else {
      voc_rec_started = FALSE;
    }
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_PAUSE

DESCRIPTION
  This function pauses voice recording.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_pause (voc_rec_pause_type data_option)
{
  voc_rec_result_type    rec_result;

  /* Check to see if we are recording voice.
  */
  if ((voc_rec_cmd_in_progress == VOC_REC_REVERSE) ||
      (voc_rec_cmd_in_progress == VOC_REC_FORWARD) ||
      (voc_rec_cmd_in_progress == VOC_REC_BOTH))
  {
    rec_result.fwd_ptr = (voc_rec_header_type *) NULL;
    rec_result.rev_ptr = (voc_rec_header_type *) NULL;

    /* Stop voice recording in qdsp command interrupt routine.
    */
    qdsp_cmd_pause_rec();
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    if (data_option == VOC_REC_PAUSE_RET_DATA) {
      /* Get the last buffer info without stopping the recording. */
      vocsup_rec_sync_up (&rec_result);
    }

    /* Call back with paused status */
    rec_result.status = VOC_REC_STAT_PAUSED;
    record_notify (&rec_result);
    vocsup_rec_free_buf(&rec_result);
  }
} /* end voc_rec_pause */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION voc_rec_resume

DESCRIPTION
  This function resumes recording if recording was paused.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  None

SIDE EFFECTS
  May re-configure vocoder to different voice mode.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_resume (void)
{
  voc_rec_result_type    rec_result;
 
  /* Check to see if we are recording voice.
  */
  if ((voc_rec_cmd_in_progress == VOC_REC_REVERSE) ||
      (voc_rec_cmd_in_progress == VOC_REC_FORWARD) ||
      (voc_rec_cmd_in_progress == VOC_REC_BOTH))
  {
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    /* Resume voice recording in qdsp command interrupt routine.
    */
    qdsp_cmd_resume_rec();

    /* Initialize call back structure.
    */
    rec_result.fwd_ptr = (voc_rec_header_type *) NULL;
    rec_result.rev_ptr = (voc_rec_header_type *) NULL;
    rec_result.status  = VOC_REC_STAT_RESUMED;
#ifdef FEATURE_MM_REC
    vocsup_rec_reset_frm_count();
#endif /* FEATURE_MM_REC */
    record_notify (&rec_result);
    /* After resume, we need to call the below function to avoid the av sync 
       issues */
#ifdef FEATURE_MM_REC
    vocsup_rec_do_av_sync();
#endif
  }
} /* end voc_rec_resume */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_REC_STOP

DESCRIPTION
  This function stops voice recording.  This function will call back
  the function registered with voc_rec_start () with the remainder of the
  recorded vocoder frames.

DEPENDENCIES
  FEATURE_VOICE_RECORD is defined.

RETURN VALUE
  Via call back function:
    Ptr to header of forward link record buffer
    Ptr to header of reverse link record buffer
    Record status

SIDE EFFECTS
  Disable vocoder encoder interrupt out of a call.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void voc_rec_stop (void)
{
  voc_rec_stop_status(VOC_REC_STAT_DONE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_VOICE_RECORD */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_VR_CHECK_SO_CHANGE

DESCRIPTION
  Check for possible service option change while voice service operation is in
  progress.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May terminate voice playback or voice recording.

===========================================================================*/
void voc_vr_check_so_change ()
{
  voc_state_control_type *state;

  state = voc_state_get_state_data();

  /* Check if the phone is in a CDMA call.
  */ 
  if ((state != NULL) && (voc_check_in_call(state->acquire_mask)))
  {
#ifdef FEATURE_VOICE_PLAYBACK
    /* Check if a play back operation is in progress.
    */
    if (voc_play_back_status == VOC_PB_STATUS_PLAYING)
    {
      /* Get pointer to voice utterance currently playing.
      */
      curr_pb_hdr = vocsup_pb_get_hdr (VOC_PB_DIR_BOTH);
      if (curr_pb_hdr != (voc_pb_header_type *) NULL)
      {
        /* Check if the service option has changed.
        */
        if (curr_pb_hdr->pb_voc_capability != state->config) 
        {
          /* Terminate play if play service option is different.
          */
          voc_pb_abort ();
        }
      }
      else
      {
        /* Error condition, stop voice play back.
        */
        voc_pb_abort ();
      }
    } /* end if (voc_play_back_status == VOC_PB_STATUS_PLAYING) */
#endif /* FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_VOICE_RECORD
    /* Check if a voice recording session is in progress. 
    */
    if ((voc_rec_cmd_in_progress == VOC_REC_REVERSE) ||
        (voc_rec_cmd_in_progress == VOC_REC_FORWARD) ||
        (voc_rec_cmd_in_progress == VOC_REC_BOTH))
    {
      /* If the new service option is different than the old one, end
      ** recording.
      */
      if (voc_rec_capability != state->config) 
      {
        voc_rec_stop_status(VOC_REC_STAT_NEW_CONFIG_ERROR);
      } /* end if (voc_rec_capability != state->config) */
    } /* end if ((voc_rec_cmd_in_progress == VOC_REC_REVERSE) || ... ) */
#endif /* FEATURE_VOICE_RECORD */

  } /* end if (state != NULL) && ... */

} /* end voc_vr_check_so_change */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

                FUNCTIONS FOR MODE B/C VR SUPPORT

===========================================================================*/
/*===========================================================================

FUNCTION vocvr_match_cmd_in_progress

DESCRIPTION
  This function is called to verify if Match procedures have started.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - In Match State
  FALSE - Not in Match State

SIDE EFFECTS
  None

===========================================================================*/

boolean vocvr_match_cmd_in_progress
(
  void
)

{
  return (voc_match_cmd_in_progress);
} /* end vocvr_match_cmd_in_progress */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_voc_frame_ready

DESCRIPTION
  This function is called to report that 1 vocoder frame is available from 
  the DSP for endpoint detection.
  
DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR CAPTURE state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void vocvr_voc_frame_ready
(
  uint8                         *buf_ptr
  /* Ptr to buffer containing voc frame + vr diagnostic data + frame no    */
)

{
  /* Call back function registered via vocvr_capture_start ().
  */
  if (voc_endpt_cb != (voc_endpt_cb_type) NULL)
  {
    voc_endpt_cb (buf_ptr);
  }

} /* end vocvr_voc_frame_ready */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_go_to_vr_prompt

DESCRIPTION
  This function returns the DSP to VR PROMPT state.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR IDLE, VR PROMPT, VR CAPTURE or VR MATCH state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None
  
===========================================================================*/

voc_status_type vocvr_go_to_vr_prompt (void)

{
  voc_state_control_type      *state;
  sint15                      dsp_state;
  volatile boolean            qdsp_change_flag = FALSE;
  uint16                      i;
#ifdef FEATURE_CMI
#error code not present
#endif
  /* Get vocoder driver state information.
  */
  state = voc_state_get_state_data();

  /* Check which state the DSP is in.
  */
  dsp_state = qdsp_cmd_get_state();

  /* Reject request if:
  **   - The vocoder is in reset
  **   - Current DSP image is not VR Mode B/C
  **   - DSP is not in VR PROMPT, VR CAPTURE or VR MATCH state
  */
  if ((voc_state_get_inform_val() == VOC_INFORM_RESET   ) ||
      (state->vr_mode != VOC_VR_MODE_BC                 ) ||
      ((dsp_state != QDSP_STATE_VR_PROMPT)  && 
       (dsp_state != QDSP_STATE_VR_CAPTURE) &&
       (dsp_state != QDSP_STATE_VR_MATCH)               ) 
     )
  {
    MSG_ERROR ("VR PROMPT cmd invalid in state %d",dsp_state,0,0);
    return (VOC_CMD_FAILED_S);
  }

  /* Move DSP to VR PROMPT state so capture and play back operations
  ** can work.  
  */
  if (qdsp_cmd_vr_prompt () != QDSP_CMD_SUCCESS)
  {
    MSG_ERROR ("DSP rejects VR PROMPT cmd",0,0,0);
    voc_match_cmd_in_progress = FALSE;

    return (VOC_CMD_FAILED_S);
  }

  /* Wait for QDSP to arrive in VR PROMPT state, max 1 sec */ 
  for( i = 0; i < 10000; i++ )
  {
    if ( qdsp_cmd_get_state() == QDSP_STATE_VR_PROMPT )
    {
      qdsp_change_flag = TRUE;
      break;
    }
    
         #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)

              
    		clk_busy_wait(200);
 	      #else    		
#error code not present
	      #endif
  }
  
  /* Check if DSP is in VR PROMPT state, report result.  Reset flag
  ** to allow capture and play back operations.
  */
  if (qdsp_change_flag == FALSE)
  {
    MSG_ERROR ("DSP is NOT in VR PROMPT state!!!", 0,0,0 );
    voc_match_cmd_in_progress = FALSE;
    return (VOC_CMD_FAILED_S);
  }
  /* Indicate that we are done with match.
  */
  voc_match_cmd_in_progress = FALSE;
  
  voc_retry_codec_select();
  
  voc_retry_codec_gain();

  MSG_HIGH ("DSP in VR PROMPT state", 0,0,0 );
  return (VOC_DONE_S);

} /* end vocvr_go_to_vr_prompt */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_capture_start

DESCRIPTION
  This function is called to capture an utterance for Mode B/C VR.
  
DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None

===========================================================================*/

voc_status_type vocvr_capture_start
(
  voc_capture_start_type      *voc_capture_parameters,
                              /* Capture parameters                        */
  voc_endpt_cb_type           cb_ptr
                              /* Pointer to end point call back function   */
)
             
{
  voc_state_control_type      *state;
  sint15                      dsp_state;
  voc_rate_type               max_rate;
  voc_rate_type               min_rate;
  uint16                      qdsp_min_rate;
  uint16                      qdsp_max_rate;
  boolean                     talkover_capture;
  uint16                      capture_control = 0;
  voc_codec_vr_type           vr_type;

  /* Get vocoder driver state information.
  */
  state = voc_state_get_state_data();

  /* Check which state the DSP is in.
  */
  dsp_state = qdsp_cmd_get_state();
  
  max_rate = voc_capture_parameters->vocoder_settings.max_rate;
  min_rate = voc_capture_parameters->vocoder_settings.min_rate;

  /* Reject request if:
  **   - The vocoder is in reset
  **   - Current DSP image is not VR Mode B/C
  **   - A recording operation is in progress
  **   - A match operation is in progress
  **   - Phone is in an Analog call
  **   - Capture rates are illegal
  **   - Capture parameter pointer is NULL
  **   - Callback function pointer is NULL
  **   - DSP is not in VR PROMPT or VR CAPTURE state
  */
  if ((voc_state_get_inform_val() == VOC_INFORM_RESET           ) || 
      (state->vr_mode != VOC_VR_MODE_BC                         ) ||
      (voc_rec_cmd_in_progress != VOC_REC_NONE                  ) ||
      (state->acquire_mask & ((uint16) 1 << VOC_ACQ_ACP)        ) ||
      (max_rate > VOC_1_RATE                                    ) || 
      (min_rate > VOC_1_RATE                                    ) || 
      (min_rate > max_rate                                      ) ||
      (voc_capture_parameters == (voc_capture_start_type *) NULL) ||
      (cb_ptr == (voc_endpt_cb_type) NULL                       ) ||
      ((dsp_state != QDSP_STATE_VR_PROMPT ) &&                  
       (dsp_state != QDSP_STATE_VR_CAPTURE)                     )
     )
  {
    MSG_ERROR ("Reject capture start cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }
  /* Just to re-confirm that DSP is not in Match   */
  voc_match_cmd_in_progress = FALSE;
  
  /* Prepare to capture.
  */
  qdsp_min_rate = vocvr_rate_conv[min_rate];
  qdsp_max_rate = vocvr_rate_conv[max_rate];

  if (voc_capture_parameters->talk_over_prompt == VOC_ENABLE_TALKOVER)
  {
    talkover_capture = TRUE;
  }
  else
  {
    talkover_capture = FALSE;
  }

  /* Create capture control for DSP.
  ** 1.  Set VR wideband switch.
  */
  if (voc_capture_parameters->wideband_switch == VOC_WIDEBAND_VR)
  {
    capture_control |= QDSP_VR_WIDEBAND_V;
  }
  else
  {
    capture_control |= QDSP_VR_NARROWBAND_V;
    if (qdsp_cmd_set_narrowb () != QDSP_CMD_SUCCESS)
    {
      MSG_ERROR ("Failed setting nband parameters",0,0,0);
      return (VOC_CMD_FAILED_S);
    }
  }

  /* 2.  Always turn on recording.
  */
  capture_control |= QDSP_VR_REC_ON_V;

  /* 3.  Get codec path from database to set vr operation mode.
  */
  (void) voc_data_get_codec_vr_type(&vr_type);

  switch (vr_type) 
  {
    case VOC_CODEC_VR_TYPE_HANDSET:
      capture_control |= QDSP_VR_HANDSET_MODE_V;       
      break;                       

    case VOC_CODEC_VR_TYPE_HEADSET:
      capture_control |= QDSP_VR_HEADSET_MODE_V;       
      break;                       

    case VOC_CODEC_VR_TYPE_CARKIT:
      capture_control |= QDSP_VR_HFK_MODE_V;       
      break;

    case VOC_CODEC_VR_TYPE_NONE:
    default:                       /* Codec path not valid for VR          */
      return (VOC_CMD_FAILED_S);
      break;

  } /* end switch (pcm_path) */

  /* Save callback function pointer.
  */
  voc_endpt_cb = cb_ptr;

  /* Send request to QDSP Command layer, call back to wrapper function.
  */
  if (qdsp_cmd_capture_start 
                          (qdsp_max_rate, 
                           qdsp_min_rate, 
                           talkover_capture,
                           capture_control,
                           vocvr_voc_frame_ready) != QDSP_CMD_SUCCESS)
  {
    /* Clear callback function pointer.
    */
    INTLOCK();
    voc_endpt_cb = (voc_endpt_cb_type) NULL;
    INTFREE();

    MSG_ERROR ("DSP rejects capture start cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  /* Indicate what type of recording is in progress to avoid accepting
  ** another record command.
  */
  voc_rec_cmd_in_progress = VOC_REC_VR;

  return (VOC_DONE_S);

} /* end vocvr_capture_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_capture_cont

DESCRIPTION
  This function sends a capture continue command to the DSP.
  
DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None

===========================================================================*/

voc_status_type vocvr_capture_cont
(
  uint16                      num_frames_to_discard
  /* Discard this number of frames since they do not contain speech       */
)

{
  /* If ARM is capturing for VR, then send capture continue command to DSP.
  */
  if (voc_rec_cmd_in_progress != VOC_REC_VR) 
  {
    MSG_ERROR ("Reject capture cont cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  /* Send request to QDSP Command layer.
  */
  if (qdsp_cmd_capture_cont (num_frames_to_discard) !=
      QDSP_CMD_SUCCESS)
  {
    MSG_ERROR ("DSP rejects capture cont cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  return (VOC_DONE_S);

} /* end vocvr_capture_cont */
             
/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_capture_stop

DESCRIPTION
  This function stops an utterance capture operation.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR PROMPT or VR CAPTURE.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None
  
===========================================================================*/

voc_status_type vocvr_capture_stop 
(
  voc_capture_end_type        *voc_capture_end_parameters
                              /* Capture end parameters                    */
)

{
  qdsp_cmd_capture_end_cmd_type data;
  uint16                        i;

  /* Return if a match operation is not in progress.
  */
  if (voc_rec_cmd_in_progress != VOC_REC_VR)
  {
    return (VOC_CMD_FAILED_S);
  }

  /* Reject request if:
  **   - Capture end parameter pointer is NULL
  **   - Capture parameters are invalid
  */
  if ((voc_capture_end_parameters == (voc_capture_end_type *) NULL) ||
      (voc_capture_end_parameters->num_ep_hyps > VOC_MAX_EP_HYP)
     )
  {
    MSG_ERROR ("Reject capture stop cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  /* Indicate that we are done with capture. 
  */
  voc_rec_cmd_in_progress = VOC_REC_NONE;

  /* Clear callback function pointer.
  */
  INTLOCK();
  voc_endpt_cb = (voc_endpt_cb_type) NULL;
  INTFREE();
  
  /* Prepare command data.
  */
  data.total_num_hyps  = voc_capture_end_parameters->num_ep_hyps;
  for (i = 0; i < data.total_num_hyps; i++)
  {
    data.ep_pair_buf[i].start_frame = 
      voc_capture_end_parameters->ep_hyp_pair_buf[i].start_frame;
    data.ep_pair_buf[i].length =
      voc_capture_end_parameters->ep_hyp_pair_buf[i].length;
  }
  
  /* Send request to QDSP Command layer.
  */
  if (qdsp_cmd_capture_stop (&data) != QDSP_CMD_SUCCESS)
  {
    MSG_ERROR ("DSP rejects capture stop cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  return (VOC_DONE_S);

} /* end vocvr_capture_stop */

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_match_result

DESCRIPTION
  This function is called to report that VR score data may be available from 
  the DSP.  
  
DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR MATCH state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

voc_match_result_type *vocvr_match_result ()

{
  qdsp_cmd_match_result_type   *mres_ptr;
  static voc_match_result_type voc_match_result;

  /* Clear match result.
  */
  (void) memset (&voc_match_result, 0, sizeof (voc_match_result));

  /* Check if a match result message is available.
  */
  mres_ptr = qdsp_cmd_match_result ();
  if (mres_ptr->match_res_avail == TRUE)
  {

    /* Copy to match result.
    */
    voc_match_result.match_res_ready         = mres_ptr->match_res_avail;
    voc_match_result.out_score_data_ptr      = mres_ptr->dma_score_addr;
    voc_match_result.out_num_vr_scores       = mres_ptr->num_vr_scores;
    voc_match_result.out_vr_pattern_data_ptr = mres_ptr->dma_tr_pat_addr;
    voc_match_result.in_vr_pattern_num_ptr   = mres_ptr->dma_dl_num_addr;
    voc_match_result.in_vr_pattern_data_ptr  = mres_ptr->dma_dl_data_addr;
    voc_match_result.in_pattern_buf_size     = mres_ptr->pat_buf_size;
    voc_match_result.valid_addr              = mres_ptr->valid_addr;
  }

  return (&voc_match_result);

} /* end vocvr_match_result */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_match_start

DESCRIPTION
  This function is called to start a match operation.
  
  Special Note:  
  
  While a match operation is in progress, the DSP is not configured to 
  play voice prompts or capture.
  
DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR CAPTURE or VR MATCH state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None

===========================================================================*/

voc_status_type vocvr_match_start 
(
  voc_match_type              *voc_match_parameters
                              /* Match parameters                          */
)
             
{
  voc_state_control_type      *state;
  sint15                      dsp_state;
  qdsp_cmd_match_cmd_type     data;
  uint16                      i;
  volatile boolean            qdsp_change_flag = FALSE;
#ifdef FEATURE_CMI
#error code not present
#endif
  /* Get vocoder driver state information.
  */
  state = voc_state_get_state_data();

  /* Check which state the DSP is in.
  */
  dsp_state = qdsp_cmd_get_state();

  /* Reject request if:
  **   - The vocoder is in reset
  **   - Current DSP image is not VR Mode B/C
  **   - A recording operation is in progress
  **   - Phone is in an Analog call
  **   - Match parameter pointer is NULL
  **   - DSP is not in VR PROMPT, VR CAPTURE or VR MATCH state
  */
  if ((voc_state_get_inform_val() == VOC_INFORM_RESET   ) ||
      (state->vr_mode != VOC_VR_MODE_BC                 ) ||
      (voc_rec_cmd_in_progress != VOC_REC_NONE          ) ||
      (state->acquire_mask & ((uint16) 1 << VOC_ACQ_ACP)) ||
      (voc_match_parameters == (voc_match_type*) NULL   ) ||
      ((dsp_state != QDSP_STATE_VR_PROMPT ) && 
       (dsp_state != QDSP_STATE_VR_CAPTURE) && 
       (dsp_state != QDSP_STATE_VR_MATCH  )             )
     )
  {
    MSG_ERROR ("Reject match cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }
  
  /* Create match control for DSP.
  ** 1.  Set DSP match type.
  */
  data.vr_match_control = 0;
  switch (voc_match_parameters->match_selection) 
  {
    case VOC_MATCH_MODEB_SD:    /* ModeB SD (word-based)                   */
      data.vr_match_control |= QDSP_VR_MODEB_SD_V;                                  
      break;                                             
                                                        
    case VOC_MATCH_MODEB_SI:    /* ModeB SI (word-based)                   */
      data.vr_match_control |= QDSP_VR_MODEB_SI_V; 
      break;           

    case VOC_MATCH_MODEC_SI:    /* ModeC SI (phoneme-based)                */
      data.vr_match_control |= QDSP_VR_MODEC_SI_V;
      break;

    case VOC_MATCH_MODEBC_SISA: /* ModeB & C SI, Speaker Adaptation        */
      data.vr_match_control |= QDSP_VR_MODEBC_SISA;
      break;

    default:                    /* Invalid match selection                 */
      return (VOC_CMD_FAILED_S);
      break;

  } /* end switch (voc_match_parameters->match_selection) */

  /* 2.  Set match stage selection.
  */
  if (voc_match_parameters->match_stage == VOC_MATCH_2ND_STAGE)
  {
    data.vr_match_control |= QDSP_VR_2ND_STAGE_MATCH_V;
  }
  else
  {
    data.vr_match_control |= QDSP_VR_1ST_STAGE_MATCH_V;
  }

  /* 3.  Set direct pattern match selection.
  */
  if (voc_match_parameters->direct_pat_match == VOC_DIR_PAT_MATCH_ON)
  {
    data.vr_match_control |= QDSP_VR_DIRECT_PAT_MATCH_ON_V;
  }
  else
  {
    data.vr_match_control |= QDSP_VR_DIRECT_PAT_MATCH_OFF_V;
  }

  /* Prepare remaining command data.
  */
  data.num_hyps  = voc_match_parameters->num_ep_hyps_in_cmd;
  if (data.num_hyps > VOC_MAX_EP_HYP)
  {
    MSG_ERROR ("Reject capture stop command, bad count",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  for (i = 0; i < data.num_hyps; i++)
  {
    data.ep_index_buf [i] = voc_match_parameters->ep_hyp_index_buf [i];
  }
  
  /* Indicate that DSP is in VR MATCH state (inhibits capture & play back).
  */
  voc_match_cmd_in_progress = TRUE;
  
  /* Send request to QDSP Command layer, call back to wrapper function.
  */
  if (qdsp_cmd_match_start (&data) != QDSP_CMD_SUCCESS)
  {
    voc_match_cmd_in_progress = FALSE;
    MSG_ERROR ("DSP rejects match cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  /* Wait for DSP to arrive in VR MATCH state, max 5 ms.
  */ 
  for( i = 0; i < 100; i++ )
  {
    if ( qdsp_cmd_get_state() == QDSP_STATE_VR_MATCH )
    {
      qdsp_change_flag = TRUE;
      break;
    }
    
     #if defined (FEATURE_CMI_MM) || !defined(FEATURE_CMI)

              
    clk_busy_wait( 50 );
 	      #else    		
#error code not present
	      #endif
      
  }
  
  /* Check if DSP is in VR MATCH state, report result.  While
  ** DSP is in VR MATCH state, reject capture and play back commands.
  */
  if (qdsp_change_flag == FALSE)
  {
    voc_match_cmd_in_progress = FALSE;
    MSG_ERROR ("DSP in NOT in VR MATCH state!!!", 0,0,0 );
    return (VOC_CMD_FAILED_S);
  }

  return (VOC_DONE_S);

} /* end vocvr_match_start */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_match_stop

DESCRIPTION
  This function stops a match operation and returns the DSP to VR PROMPT
  state.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR MATCH state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None
  
===========================================================================*/

voc_status_type vocvr_match_stop (void)

{
  /* Return if a match operation is not in progress.
  */
  if (voc_match_cmd_in_progress == FALSE)
  {
    return (VOC_CMD_FAILED_S);
  }

  /* Move DSP to VR PROMPT state.
  */
  if (vocvr_go_to_vr_prompt () != VOC_DONE_S)
  {
    MSG_ERROR ("Bad VR PROMPT cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }
  
  return (VOC_DONE_S);
  
} /* end vocvr_match_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_pattern_load

DESCRIPTION
  This function is called to send patterns to the DSP (via DMA) for a 
  matching operation.
  
  Each DMA write may contain one pattern or multiple patterns.  Each pattern 
  must be preceeded with a pattern header.  The aggregate length of all the  
  patterns in each entry must be less than or equal to the length of the 
  DSP pattern buffer (as specified by the DSP).
  
  The vocoder driver assumes that the first byte in each entry is the VR
  pattern header with the format expected by the DSP.  The vocoder driver
  does not verify the header information.
  
DEPENDENCIES
  The DMA address and length of pattern buffer are specified by the DSP.
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR MATCH state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None

===========================================================================*/

voc_status_type vocvr_pattern_load
(
  uint16                      num_vr_patterns,
                              /* Number of pattern(s) loaded               */
  uint16                      *pattern_ptr
                              /* Addr of pattern(s) (DSP DMA address)      */
)

{
  /* Check if a match operation is in progress.
  */
  if (voc_match_cmd_in_progress == FALSE)
  {
    MSG_ERROR ("Reject pattern load cmd", 0,0,0 );
    return (VOC_CMD_FAILED_S);
  }

  /* Send request to QDSP Command layer.
  */
  if (qdsp_cmd_pattern_load (num_vr_patterns, pattern_ptr) != 
      QDSP_CMD_SUCCESS)
  {
    MSG_ERROR ("DSP rejects pattern load cmd",0,0,0);
    return (VOC_CMD_FAILED_S);
  }

  return (VOC_DONE_S);

} /* end vocvr_pattern_load */

#endif /* FEATURE_PHONE_VR */
#endif /* FEATURE_VOICE_PLAYBACK */
