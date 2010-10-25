#ifndef VOCVR_H
#define VOCVR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              V O C O D E R    V R             

GENERAL DESCRIPTION
  This header file contains declarations associated with vocoder driver
  Voice Services support.

REFERENCES
  MSM3100 Device Specification: 93-V0104-1

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/vocvr.h_v   1.1   04 Mar 2002 11:45:22   jackl  $
$Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/vocvr.h#3 $ $DateTime: 2009/05/24 23:18:37 $ $Author: mkiran $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/09    vm     Added check to ensure the sound file is in pause state
                   for resume to be called.
12/29/07    sj     Added support for .awb file format playback
10/26/07    hs     Added EVW file format support
03/21/07    at     Added VOC_PB_VB to voc_pb_len_rate_var_type for EVRC-B
                   file format support.
03/15/07    kan    Added 4GV-WB Code 
11/09/06    az     Merged the code for MM API changes.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
04/28/05   ymc     Added QCP file support for 4GV vocoder.
           ymc     Updated Copyright.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
04/17/03    sm     Added AMR file format support.
11/06/02    sm     Added support for QCP AMR files.
07/12/02    yl     Added new entry in voc_rec_parm_type and voc_rec_result_type.
03/04/02   jkl     Added Pufe Support under FEATURE_VR_PUFE
09/03/01    st     Modified code to add recording header type; added new 
                   recording status types; added voc_rec_put_buf() prototype;
                   removed voc_rec_free_buf() prototype.
05/21/01   jkl     Removed Enhanced VR feature.
04/04/01    st     Added support for Native QCP playback.
12/05/00    jkl    Merged VR4.0 functions. Added FEATURE_ENHANCED_VR,
                   FEATURE_DFM_ANSWER_MACHINE.
05/24/00    ro     Removed VOC_PB_DIR_DUPLICATE from voc_pb_dir_type (CR13557).
05/01/00    st     Removed voc_pb_notify_type; added VOC_END_ABORT_PB to 
                   voc_end_vpentry_retrun_type.
01/26/00    tc     Initial version.
            ro
            st
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"           /* Customer Specific Definitions           */
#ifdef FEATURE_VOICE_PLAYBACK
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "queue.h"              /* Definitions for queue services          */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                     VOICE PLAY BACK LENGTH/RATE VARIANCE

These values are used by the vocoder driver to determine if the frame control
bytes are included in the voice play back data.

  - If fixed length-fixed rate (FLFR), the vocoder driver assumes
    responsibility of building the frame control bytes to save flash memory
    (1 byte per packet).  All FLFR packets are the same length.  The vocoder
    driver outputs the frame control byte plus the same number of voice data
    bytes every frame.

  - If fixed length-variable rate (FLVR), the vocoder driver uses the frame
    control bytes embedded in the voice data.  All FLVR packets are the same
    length.  Packet length is derived from packet rate (see pb_rate below)
    and voice mode specified in the header.  Actual rate in the frame control
    byte may be equal to or less than packet rate.  Lower rate packets'
    (shorter packets') unused bytes are padded to the same length as packets
    with rate specified in the header.  The vocoder driver only outputs
    legitimate bytes.

  - If variable length-variable rate (VLVR), the vocoder driver uses the frame
    control bytes embedded in the voice data.  All VLVR packets are of
    variable length.  The vocoder driver outputs the correct number of bytes
    using the rate and voice mode information in the frame control byte.

  - If Native QCP format, the vocoder driver will use the FCB embedded in
    the voice data. All Native QCP packets are of variable length. The vocoder
    driver outputs the correct number of bytes using the rate and voice mode
    information in the frame control byte.

---------------------------------------------------------------------------*/
typedef enum {

  VOC_PB_FIX_LEN_FIX_RATE,     /* fixed length, fixed rate       */
  VOC_PB_FIX_LEN_VAR_RATE,     /* fixed length, variable rate    */
  VOC_PB_VAR_LEN_VAR_RATE,     /* variable length, variable rate */
  VOC_PB_NATIVE_QCP,           /* Native QCP format              */
  VOC_PB_AMR,                  /* AMR file format                */
  VOC_PB_AWB,                  /* AWB file format                */
  VOC_PB_EVB,                  /* EVB file format                */
  VOC_PB_EVW                   /* EVW file format                */

} voc_pb_len_rate_var_type;

/* Direction of playback.  Only one playback mode is supported at a given time.
*/
typedef enum
{
  /* Playback on the forward link is supported in or out of a call. */
  VOC_PB_DIR_FORWARD,

  /* Playback on the reverse link is only supported in a call. */
  VOC_PB_DIR_REVERSE,

  /* Playback on both forward and reverse links is not supported. */
  VOC_PB_DIR_BOTH,

  /* Validity check */
  VOC_MAX_PB_DIR_TYPE

} voc_pb_dir_type;

/*---------------------------------------------------------------------------
                   VOCODER VOICE PLAY BACK QUEUE ENTRY

This structure provides the vocoder driver information about the voice
utterance to play back.  Multiple voice utterances may be queued up for
play back.
---------------------------------------------------------------------------*/
#ifndef VOC_PB_HEADER_TYPE
#define VOC_PB_HEADER_TYPE
/* Forward definition of voc_pb_header_type */
typedef struct voc_pb_header_struct voc_pb_header_type;
#endif

typedef struct voc_pb_header_struct{

  /* Used to manage memory.  Should only be accessed by memory management
  ** functions.
  */
  q_link_type               link;

  /* Pointer to voice data to play back */
  byte                      *pb_data_ptr;

  /* Number of vocoder frames to play back voice */
  word                      pb_num_frames;

  /* Used by vocoder driver to command vocoder to change voice mode if
  ** current voice mode is different than voice mode in which voice
  ** utterance was recorded (8K, 13K or EVRC).
  */
  voc_capability_type       pb_voc_capability;

  /* Describes variance of length and rate of packet on frame by frame
  ** basis (see description of voc_pb_len_rate_var_type).
  */
  voc_pb_len_rate_var_type  pb_len_rate_var;

  /*
  ** -  If voc_pb_len_rate_var_type is fixed length-fixed rate, pb_rate is
  **    used to build frame control bytes.  Frame control bytes are same for
  **    all frames in voice utterance.
  **
  ** -  If voc_pb_len_rate_var_type is fixed length-variable rate, pb_rate
  **    is used to calculate frame length.  pb_rate is maximum rate of all
  **    packets in voice utterance.
  **
  ** -  If voc_pb_len_rate_var_type is variable length-variable rate,
  **    pb_rate is not used.
  */
  voc_rate_type             pb_rate;

  /* Play back direction */
  voc_pb_dir_type           pb_direction;

  /* If this is to be muxed with voice call */
  boolean                   in_call;

  /* Used for flash read/write.
  */
  word                      pb_length;

  /* Used to store pointer to client data for this voice prompt */
  const void                *client_data;

} voc_pb_header_struct;

/*---------------------------------------------------------------------------
                   VOCODER SETTINGS

The following structure describes the vocoder settings to use for voice
record and playback.

---------------------------------------------------------------------------*/
typedef struct
{
  voc_capability_type      capability;
  voc_rate_type            max_rate;
  voc_rate_type            min_rate;
  voc_pb_len_rate_var_type frame_format;

} voc_rate_set_type;

typedef enum {

  VOC_PB_OK,
    /* Request is accepted */
  VOC_PB_FULL,
    /* Request is rejected because the play back queue is full */
  VOC_PB_NO_CDMA,
    /* Request is rejected because the phone is not in CDMA mode */
  VOC_PB_DIFF_SO,
    /* Request is rejected because service option change is not
    ** allowed at this time
    */
  VOC_PB_ERR
    /* Request is rejected because it is erroneous */

} voc_add_pbq_return_type;

typedef enum {

  VOC_PB_STATUS_IDLE,
    /* Not playing voice prompt */
  VOC_PB_STATUS_PLAYING
    /* Playing voice prompt */

} voc_get_pbs_return_type;

typedef enum {

  VOC_END_PB,
    /* End play back session */
  VOC_END_ABORT_PB,
    /* Abort_play back session */
  VOC_END_Q_NOT_EMPTY
    /* Done with one voice prompt, more voice prompts to play */

} voc_end_vpentry_return_type;

/* A pointer to a function returning void with 3 parameters.
*/
typedef void (*voc_pb_notify_func_ptr_type)
  (
    voc_end_vpentry_return_type,
    voc_pb_header_type *,
    const void *
  );

#ifdef FEATURE_PHONE_VR
/* The following enum is used to allow/disallow talk over feature.
*/
typedef enum
{
  VOC_ENABLE_TALKOVER,     /* Start capture immediately                    */
  VOC_DISABLE_TALKOVER     /* Delay capture until playback is done         */

} voc_capture_mode_type;

#endif /* FEATURE_PHONE_VR */

typedef enum
{
  VOC_SND_STATUS_ACTIVE,   /* Sound is playing a tone                      */
  VOC_SND_STATUS_INACTIVE  /* Sound is done                                */

} voc_snd_active_status_type;

#if defined (FEATURE_VOICE_RECORD) || defined (FEATURE_PHONE_VR)

/* Different recording modes.  Only one recording mode is supported at a
** given time.
*/
typedef enum
{
  /* No recording in progress. */
  VOC_REC_NONE,

  /* Currently, recording for Voice Recognition is only supported when the
  ** phone is not in a call.
  */
  VOC_REC_VR,

  /* Recording the forward link is only supported in a call. */
  VOC_REC_FORWARD,

  /* Recording the reverse link is supported in or out of a call. */
  VOC_REC_REVERSE,

  /* Recording both the forward and reverse links is only supported in a call.
  */
  VOC_REC_BOTH,

  /* Validity check.
  */
  VOC_MAX_REC_TYPE

} voc_rec_type;
#endif /* FEATURE_PHONE_VR || FEATURE_VOICE_RECORD */

#ifdef FEATURE_VOICE_RECORD
/*---------------------------------------------------------------------------
** Vocoder record definitions
**---------------------------------------------------------------------------
*/
/* This structure is used to keep track of data that has been recording.
*/
typedef struct {
  byte   *rec_data_ptr;    /* Pointer to voice data recorded    */
  uint32 rec_buffer_size;  /* Size of rec_data_ptr in bytes     */
  word   rec_num_frames;   /* Number of vocoder frames recorded */
  word   rec_length;       /* Number of bytes recorded          */
} voc_rec_header_type;

/* Record command return values.
*/
typedef enum
{
  /* Record command executed successfully.                                 */
  VOC_REC_STAT_SUCCESS,

  /* Recording has stopped.                                                */
  VOC_REC_STAT_DONE,

  /* Recording has stopped due to the silence                              */
  VOC_REC_STAT_AUTO_STOP,

  /* Recording has paused.                                                 */
  VOC_REC_STAT_PAUSED,

  /* Recording has resumed.                                                */
  VOC_REC_STAT_RESUMED,

  /* Vocoder driver can no longer record due to an unrecoverable error
  ** condition.
  */
  VOC_REC_STAT_ERROR,

  /* Vocoder driver can no longer record due to lack of recording buffers. */
  VOC_REC_STAT_BUFFER_ERROR,

  /* Invalid input parameters, recording session has not started.          */
  VOC_REC_STAT_INVALID_PARAM,

  /* Number of frames recorded matches the interval callback time          */
  VOC_REC_STAT_INT_TIME,

  /* Recorded data buffers are available.                                  */
  VOC_REC_STAT_DATA,

  /* Phone is in analog mode and this forced recording to stop             */
  VOC_REC_STAT_ANALOG_ERROR,

  /* The vocoder has been configured to a different service option and     */
  /* this forced recording to stop.                                        */
  VOC_REC_STAT_NEW_CONFIG_ERROR,

  /* Service option is EVRC, recording will start with this service option */
  VOC_REC_STAT_INFORM_EVRC,

  /* Service option is 13k, recording will start with this service option  */
  VOC_REC_STAT_INFORM_13K,

  /* Service option is 4GV_NB, recording will start with this service option */
  VOC_REC_STAT_INFORM_4GV_NB,

  /* Service option is 4GV_WB, recording will start with this service option */
  VOC_REC_STAT_INFORM_4GV_WB,

  /* In an analog call and continuing to record.                           */
  VOC_REC_STAT_INFORM_ANALOG,

  /* Service option is AMR, recording will start with this service option  */
  VOC_REC_STAT_INFORM_AMR,

  /* Service option is EFR, recording will start with this service option  */
  VOC_REC_STAT_INFORM_EFR,

  /* Service option is FR, recording will start with this service option   */
  VOC_REC_STAT_INFORM_FR,

  /* Service option is HR, recording will start with this service option   */
  VOC_REC_STAT_INFORM_HR,

  /* For vocoder driver's internal use only.                               */
  VOC_REC_STAT_NOT_READY

} voc_rec_status_type;

/* This structure provides record status and ptrs to the forward
** link and reverse link record buffers.
*/
typedef struct
{
  /* Ptr to the forward link header. */
  voc_rec_header_type         *fwd_ptr;

  /* Ptr to the reverse link header. */
  voc_rec_header_type         *rev_ptr;

  /* Record status */
  voc_rec_status_type status;

} voc_rec_result_type;

#ifdef FEATURE_MM_REC
/*
 * Data structure for A/V sync report information.
 */
typedef struct {
  uint32 num_samples;         /* AV sync report, total samples just encoded   */
  uint32 time_stamp[2];       /* Time stamp at this moment                    */
  uint32 lost_samples;        /* Lost samples if any                          */
  uint32 frame_index;         /* Frame count/index for current frame          */
} voc_rec_av_report_type;

/* 
 * The following is the callback type used for sound server to client
 * A/V sync for MM recording.
 */
typedef void (* voc_rec_av_sync_cb_ptr) (
  voc_rec_av_report_type *av_report,
  const void             *client_data
);
#endif /* FEATURE_MM_REC */


/* This structure provides recording parameters to start a voice
** recording session.
*/
typedef struct
{
  /* Record report interval in ms */
  word                       rec_interval_ms;

  /* Silence time in ms to stop the recording */
  word                       auto_stop_ms;

  /* Vocoder settings for the record session */
  voc_rate_set_type          settings;

  /* Type of record */
  voc_rec_type               rec_type;

  /* Whether to enable DTX or not     */
  boolean                    dtx_enable;
#ifdef FEATURE_MM_REC

  /* Data request time interval in ms */
  uint32                     data_req_ms;

  
  /* AV sync callback function        */
  voc_rec_av_sync_cb_ptr     sync_cb_func;

  /* AV sync callback data to apps    */
  const void                 *client_data;
#endif /* FEATURE_MM_REC */
} voc_rec_parm_type;

/* A pointer to a function returning void with 1 parameter.
*/
typedef void (*voc_rec_cb_type) (voc_rec_result_type *voc_rec_result);

typedef enum
{
  /* Pause, flush the recording buffers now. */
  VOC_REC_PAUSE_RET_DATA,

  /* Pause, leave the recording buffers as is and return NULL. */
  VOC_REC_PAUSE_RET_NULL

} voc_rec_pause_type;
#endif /* FEATURE_VOICE_RECORD */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODE B/C VR SUPPORT

===========================================================================*/

/* Max number of endpoint hypotheses.
*/
#define VOC_MAX_EP_HYP        3        

/* VR Mode B/C Capture - Wideband VR switch
*/
typedef enum
{
  VOC_WIDEBAND_VR,            /* Wideband VR,  16K Hz input PCM samples    */
  VOC_NARROWBAND_VR           /* Narrowband VR, 8K Hz input PCM samples    */

} voc_wideband_vr_type;

/* VR Mode B/C Match - Direct pattern match selection
*/
typedef enum
{
  VOC_DIR_PAT_MATCH_ON,       /* Direct pattern match is ON                */  
  VOC_DIR_PAT_MATCH_OFF       /* Direct pattern match is OFF               */

} voc_dir_pat_match_type;

/* VR Mode B/C Match - Two stage match selection
*/
typedef enum
{
  VOC_MATCH_1ST_STAGE,        /* First stage match                         */
  VOC_MATCH_2ND_STAGE         /* Second stage match                        */

} voc_match_stage_type;

/* VR Mode B/C Match - DSP match type
*/
typedef enum
{
  VOC_MATCH_MODEB_SD,         /* ModeB Speaker Dependent (word-based)      */
  VOC_MATCH_MODEB_SI,         /* ModeB Speaker Independent (word-based)    */
  VOC_MATCH_MODEC_SI,         /* ModeC Speaker Independent (phoneme-based) */
  VOC_MATCH_MODEBC_SISA       /* ModeB & C Spk Independent, Spk Adaptation */

} voc_match_select_type;

/* Structure containing capture parameters used in function
** vocvr_capture_start ().
*/
typedef struct
{
  voc_rate_set_type           vocoder_settings;
                              /* Vocoder settings                          */
  voc_capture_mode_type       talk_over_prompt;
                              /* Talk-over-prompt feature setting          */
  voc_wideband_vr_type        wideband_switch;
                              /* Wideband/Narrowband switch                */
} voc_capture_start_type;
                                                              
/* Structure containing start frame and length (number of 10 ms frames) for 
** each endpoint hypothesis.
*/
typedef struct
{
  uint16                      start_frame;   
    /* Voice frame number at start of speech                               */
  uint16                      length;     
    /* Utterance length (number of 10 ms frames)                           */

} voc_ep_pair_type;

/* Structure containing capture end parameters used in function
** vocvr_capture_stop ().
*/
typedef struct
{
  uint16                      num_ep_hyps;   
    /* Number of total endpoint hypotheses generated by Endpoint Detector  */
  voc_ep_pair_type            ep_hyp_pair_buf [VOC_MAX_EP_HYP];
    /* Buffer to store start/endpoint pair of all endpoint hypotheses      */

} voc_capture_end_type;

/* Structure containing match parameters used in function 
** vocvr_match_start ().
*/
typedef struct 
{
  voc_dir_pat_match_type      direct_pat_match;
    /* Direct pattern match selection                                      */
  voc_match_stage_type        match_stage;
    /* First or second stage search for match                              */
  voc_match_select_type       match_selection;
    /* Match type to be performed in DSP                                   */
  uint16                      num_ep_hyps_in_cmd; 
    /* Number of endpoint hypotheses for this match cmd                    */
  uint16                      ep_hyp_index_buf [VOC_MAX_EP_HYP];         
    /* Buffer to store ep hyp indexes and the pattern type for each index  */

} voc_match_type;

/* Structure containing match result parameters from DSP.
*/
typedef struct
{
  volatile boolean            match_res_ready;
                              /* This is a new match result message        */
  uint16                      *out_score_data_ptr;
                              /* Ptr to read VR scores from DSP            */
  uint16                      out_num_vr_scores;                           
                              /* Number of valid VR scores                 */
  uint16                      *out_vr_pattern_data_ptr;
                              /* Ptr to DSP mem to read captured pattern   */
  uint16                      *in_vr_pattern_num_ptr; 
                              /* Ptr to DSP memory to write number of pats */
  uint16                      *in_vr_pattern_data_ptr;
                              /* Ptr to DSP memory to dnload pattern data  */
  uint16                      in_pattern_buf_size;
                              /* Size (in words) of next pattern download  */
  boolean                     valid_addr;
                              /* TRUE if DMA addresses are in range        */
} voc_match_result_type;

/* This call back function is used to call the end point detector function.
*/
typedef void (*voc_endpt_cb_type) 
(
  uint8                       *buf_ptr
  /* Ptr to buffer containing voc frame + vr diagnostic data + frame no    */
);

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_BYTES_TO_FRAMES

DESCRIPTION
  Given the specified vocoder and vocoder frame format, determine how many
  COMPLETE vocoder frames in the given format will fit into the number of bytes
  specified.  In the case of variable size formats, out_min_frames and
  out_max_frames may be set to different values.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Maximum number of frames
  Minimum number of frames

SIDE EFFECTS
  None

===========================================================================*/
extern void voc_bytes_to_frames
  (
    voc_capability_type vocoder,       /* Option to configure vocoder      */
    voc_pb_len_rate_var_type format,   /* Play back format                 */
    voc_rate_type max_rate,            /* Max vocoder rate to consider     */
    dword size_in_bytes,               /* Size of buffer                   */
    dword *out_min_frames,             /* Min number of frames possible    */
    dword *out_max_frames              /* Max number of frames possible    */
  );

/* <EJECT> */
/*===========================================================================

FUNCTION VOC_COUNT_FRAMES

DESCRIPTION
  Given the specified vocoder and vocoder frame format, examine the buffer of
  vocoder frames passed in and determine how many COMPLETE vocoder frames are
  present, up to the number of frames specified in inout_frame_count.  Return
  the number of COMPLETE vocoder frames and how many bytes those frames
  occupy.  Always assume that the first byte of the input buffer begins a new
  vocoder frame.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  Status of request
  Number of complete vocoder frames
  Number of bytes those frames occupy

SIDE EFFECTS
  None

===========================================================================*/
extern voc_status_type voc_count_frames
  (
    voc_capability_type vocoder,       /* Option to configure vocoder      */
    voc_pb_len_rate_var_type format,   /* Play back format                 */
    voc_rate_type max_rate,            /* Max vocoder rate to consider     */
    const void *in_buffer,             /* Pointer to input buffer          */
    dword in_buffer_bytes,             /* Size of input buffer             */
    dword *out_frame_bytes,            /* Number of bytes occupied         */
    dword *inout_frame_count           /* Number of complete frames        */
  );

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
extern void voc_vr_goto_dfm (void);

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
extern void voc_vr_check_so_change (void);

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
extern voc_add_pbq_return_type voc_pb_add_q
  (
    /* pointer to voice utterance header */
    voc_pb_header_type *voc_pb_header_ptr,
    /* pointer to client data associated with utterance header */
    const void         *client_data
  );

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
extern voc_get_pbs_return_type voc_pb_get_status (void);

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
extern void voc_pb_end (void);

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
extern void voc_pb_abort (void);

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
extern void voc_pb_pause (void);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_pb_resume

DESCRIPTION
  This function resumes voice play back in two cases:

    1.  If the play back was paused via a call to voc_pb_pause (), this
        function clears the flag which causes erasure frames to be sent to the
        vocoder.  Pausing play back via voc_pb_pause does not disable the
        decoder ISR.

    2.  If the play back was paused via a call to vmux_disable_dec_isr (),
        this function enables the decoder ISR and check for a change in
        the vocoder configuration.  If the new configuration can support the
        current voice play back, the play back is resumed.  Otherwise, play
        back is cancelled via a call back to SND.

DEPENDENCIES
  FEATURE_VOICE_PLAYBACK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Enable vocoder decoder ISR.  May re-configure vocoder to different voice
  mode.

===========================================================================*/
extern void voc_pb_resume (boolean pause);

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
extern void voc_pb_register (voc_pb_notify_func_ptr_type voc_func);

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
extern void voc_rx_vp_mute_ctl ( void );

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
extern void voc_snd_active_status (voc_snd_active_status_type snd_play_stat);

/* <EJECT> */
/*===========================================================================

FUNCTION voc_get_shared_mem_ptr

DESCRIPTION
  This function returns a pointer to the shared RAM area between the vocoder
  driver and the VR engine.  The vocoder shared RAM area is limited to two
  buffers used in voice recording.  The purpose of sharing RAM is to eliminate
  RAM usage for the recording buffers.  By sharing RAM, voice recording (for
  voice memo only at this point) and voice recognition can not be running
  concurrently.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  Pointer to shared RAM area.

SIDE EFFECTS
  None

===========================================================================*/
extern void *voc_get_shared_mem_ptr (void);
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_RECORD
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
extern void voc_rec_start
  (
    /* Call back function. */
    voc_rec_cb_type          cb,

    /* Recording parameters */
    voc_rec_parm_type        *voc_rec_parm
  );

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
extern void voc_rec_pause (voc_rec_pause_type data_option);

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
extern void voc_rec_resume (void);

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
extern void voc_rec_stop (void);

/* <EJECT> */
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
extern void voc_rec_put_buf (
  uint8  *buffer,
  uint32 num_bytes
);

#endif /* FEATURE_VOICE_RECORD */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

          EXTERNAL FUNCTION DECLARATIONS FOR MODE B/C VR SUPPORT

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

extern boolean vocvr_match_cmd_in_progress
(
  void
);

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

extern voc_status_type vocvr_capture_start
(
  voc_capture_start_type      *voc_capture_parameters,
                              /* Capture parameters                        */
  voc_endpt_cb_type           cb_ptr
                              /* Pointer to end point call back function   */
);

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

extern voc_status_type vocvr_capture_cont
(
  uint16                      num_frames_to_discard
  /* Discard this number of frames since they do not contain speech       */
);
             
/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_capture_stop

DESCRIPTION
  This function stops an utterance capture operation.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR PROMPT or VR CAPTURE state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None
  
===========================================================================*/

extern voc_status_type vocvr_capture_stop 
(
  voc_capture_end_type        *voc_capture_end_parameters
                              /* Capture end parameters                    */
);

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

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

extern voc_status_type vocvr_match_start
(
  voc_match_type              *voc_match_parameters
                              /* Match parameters                          */
);
             
/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_match_stop

DESCRIPTION
  This function stops a match operation.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.
  The DSP was configured with the appropriate VR parameters.
  The DSP is in VR MATCH state.

RETURN VALUE
  voc_status_type

SIDE EFFECTS
  None
  
===========================================================================*/

extern voc_status_type vocvr_match_stop (void);

/* <EJECT> */
/*===========================================================================

FUNCTION vocvr_match_result

DESCRIPTION
  If a VR match result message is available from the QDSP2, this function 
  returns the appropriate VR match result message data.

DEPENDENCIES
  This function depends on all VR mode B/C vocoder initialization taking 
  place first.
  The DSP is in VR MATCH state.
    
RETURN VALUE
  Ptr to match data

SIDE EFFECTS
  None

===========================================================================*/

extern voc_match_result_type *vocvr_match_result (void);

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

extern voc_status_type vocvr_pattern_load
(
  uint16                      num_vr_patterns,
                              /* Number of pattern(s) loaded               */
  uint16                      *pattern_ptr
                              /* Addr of pattern(s) (DSP DMA address)      */
);

#endif /* FEATURE_PHONE_VR */
#endif /* FEATURE_VOICE_PLAYBACK */
#endif /* VOCVR_H */

