#ifndef VSDEF_H
#define VSDEF_H
/*===========================================================================

          V O I C E   S E R V I C E S   T A S K   D E F I N I T I O N   
                           H E A D E R   F I L E


DESCRIPTION
  This header file contains common definitions for the VS task.

REFERENCES
  Voice Services ISOD: 80-24875-6

Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002             by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/common/vs/main/latest/src/vsdef.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/02   jkl     Added list_ptr on vr_pufe_recognize_init.
02/25/02   jkl     Added Pufe Support under FEATURE_VR_PUFE.
10/03/01    st     Added vs_vm_can_record_memo_processing().
09/14/01   jkl     Added function to support GPS DSP take over.
06/18/01    ch     Added synchronous VoiceMemo record stop and play stop
06/12/01   jkl     Added vs_menusel1_playback_cb.
05/21/01   jkl     Added Recognize Multi function.
04/11/01   jkl     Added Menu selection function.
08/22/00    vl     Added VR abort function prototypes.
07/28/00    ro     Updated function prototypes for mem_to_qcp, qcp_to_mem.
07/14/00    ch     Added memo to qcp and qcp to memo conversion support.
06/22/00    vl     Added mode B VR support.
05/18/00    st     Added proper featurization.
05/02/00    st     Modified function prototype for vs_voc_acquire().
01/18/00    ro     MSM3100 support. Remove capture functions and
                   vs_voc_release.
            st     Added support for capture timeout and voice activate.
10/15/99    st     Added function prototype vs_capture_processing().
08/02/99    ch     Added vs_vm_can_record_memo_processing()
07/19/99    vl     Changed memory management functions (for playback buffers).  
                   Added play direction to play functions.
05/24/99    ch     vs_vm_get_parameters_processing() has another field.
05/10/99    ch     Supports direction in play_start and record_start.
05/06/99    vl     Implemented callback handshaking functions.  Changed abort    
                   functions to comply with Voice Services API.                  
04/21/99    vl     Added vs_iyes_capture_cb and vs_iyes_playback_cb function
                   externs.  Changed param of vm_rec_cb_from_voc_processing.
04/16/99    sa     Created
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#ifdef FEATURE_VS_TASK
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "vs.h"         /* VS typedefs and prototypes                      */
#ifdef FEATURE_VOICE_DATABASE
#include "vdbrw.h"      /* Voice Database interfaces                       */
#endif
#ifdef FEATURE_VOICE_MEMO
#include "cmx.h"         /* VS typedefs and prototypes                      */
#endif

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
** VS general definitions
**---------------------------------------------------------------------------
*/

/* Max length of an utterance is 3 second.  Each second consists of 50 
** vocoder frames.
*/
#define VS_FRAMES_PER_SECOND      50
#define VS_MAX_UTT_SECONDS         3
#define VS_MAX_FRAMES             (VS_FRAMES_PER_SECOND * VS_MAX_UTT_SECONDS)

/* Size of 13K 1/2 rate vocoder packets (including frame control byte). 
*/
#define VS_13K_HALF_RATE_BYTES    17 

/* Maximum size of an utterance buffer using 13K half rate voice frames.   */
#define VS_MAX_UTTERANCE_BYTES    (VS_MAX_FRAMES * VS_13K_HALF_RATE_BYTES)

/* Maximum number of attempts to capture utterance                         */
#define VS_MAX_CAPTURE_ATTEMPTS     3

/* Maximum number of retrys for training                                   */
#define VS_MAX_TRAIN_RETRYS         3

/*---------------------------------------------------------------------------
** VS Data Types
**---------------------------------------------------------------------------
*/

/* All VS functions return vs_cmd_error_type to indicate the success or 
** failure of processing the command.
*/
typedef enum
{                                                                    
  VS_CMD_NOERR = 0,     /* Command put in command queue successfully       */
  VS_CMD_ERROR,         /* General command error                           */
  VS_CMD_Q_FULL,        /* Command queue full                              */
  VS_CMD_MAX            /* For determining number of error codes           */

} vs_cmd_error_type;

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
typedef enum {
  
  VS_PLAY_IDLE,         /* play in idle state                              */
  VS_PLAY_OK,           /* Play request done                               */
  VS_PLAY_ERR,          /* Request is rejected because it is erroneous     */
  VS_PLAY_ABORTED,      /* Abort request done                              */
  VS_PLAY_IN_PROGRESS   /* In progress, more utterances queued to play.    */

} vs_play_ret_type;
#endif

#ifdef FEATURE_PHONE_VR
typedef struct vs_match_struct {
  vdb_rwid_type match_id;         /* recog. word ID for possible match     */
  byte          utt_index;        /*index of the utt for the recog. word   */
  byte          pattern_index;    /* index of the pattern for the RW       */
} vs_match_type;

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */

/* VS function return values.
*/
typedef enum
{
  VS_RET_OK  = 0,
  VS_RET_ERR = -1

} vs_ret_type;

/* <EJECT> */
/*===========================================================================

MACRO VS_DEBUG_MSG

DESCRIPTION
  Allows leaving certain Debug messages in the Voice Services task that can 
  be turned on/off regionally.  This macro is a NOP unless /DVS_DEBUG is 
  added to the compile line for the routines.

PARAMETERS
  fmt Format string for message (printf style).  Note, this parameter
      must be a string literal (e.g. "Tuned to %lx"), not a variable.
  p1  1st parameter for format string
  p2  2nd parameter for format string
  p3  3rd parameter for format string

DEPENDENCIES
  Turned on/off by /DVS_DEBUG at compile time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef VS_DEBUG
/* VS_DEBUG is meaningless unless at we are compiling with at least
** MSG_LVL_FATAL.  Force either no VS_DEBUG or else MSG_LVL_FATAL.
*/
#if MSG_LEVEL > MSG_LVL_FATAL
#error /DVS_DEBUG Requires at least MSG_LVL_FATAL
#endif
#define VS_DEBUG_MSG(fmt,p1,p2,p3)     MSG_FATAL(fmt,p1,p2,p3)
#else
#define VS_DEBUG_MSG(fmt,p1,p2,p3)
#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
** VS Regional Function Prototypes (for Voice Recognition (vr.c) and Voice
** Memo (vm.c) use only.)
**---------------------------------------------------------------------------
*/

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
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
extern void vs_play_abort_processing
(
  vs_play_cb_type   abort_callback_ptr
    /* Function to call when playback abort is complete. */
);

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
extern vs_cmd_error_type vs_play_multi_processing 
(
  vs_sound_type         *multi_list_ptr,
  vs_snd_direction_type play_direction,
  vs_play_cb_type       cb_func
);

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
extern voc_pb_header_type *vs_play_get_buf (void);

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
extern vs_ret_type vs_play_free_buf_immediate 
  (
    voc_pb_header_type *buf_ptr            /* Pointer to unused buffer.        */
  ); 
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
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
extern vs_status_type vs_voc_acquire( boolean acquire );

/* <EJECT> */
/*===========================================================================

FUNCTION vs_voice_activate_preempted

DESCRIPTION
  This function indicates that the vocoder was acquired by other
  application.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
extern vs_status_type vs_voice_activate_preempted ( void );

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
extern boolean vs_handshake_query (void);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_capture_end

DESCRIPTION
  This function is registered as a call back function to the VR Engine 
  Endpoint Detection.  This function is called when endpoint detection is 
  done.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Queue a request in the VS command queue.

===========================================================================*/
extern void vs_capture_end 
( 
  vs_status_type status
);

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
extern vs_cmd_error_type vs_play_temp_utterance 
(
  vs_play_cb_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for training.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_train_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for training.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_train_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  recognition.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recog_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for
  recognition.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recog_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_multi_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  recognition.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recog_multi_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recog_multi_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for
  recognition.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recog_multi_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_iyes_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  implicit yes.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_iyes_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_iyes_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for
  implicit yes.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_iyes_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_playback_cb

DESCRIPTION
  This function signals the VS task to update the playback status for
  voice activation.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_vactivate_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_capture_cb

DESCRIPTION
  This function signals the VS task to update the capture status for
  voice activation.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_vactivate_capture_cb
(
   vs_status_type       status  /* capture status returned from engine  */
);

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
extern void vs_menusel_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

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
extern void vs_menusel1_playback_cb
(
  vs_status_type       status  /* playback status returned from VS  */
);

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
extern void vs_menusel_capture_cb (
   vs_status_type       status  /* capture status returned from engine  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_abort_processing

DESCRIPTION
  This function processes the recognize abort request in the Voice Services 
  task command queue.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recognize_abort_processing
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_abort_processing

DESCRIPTION
  This function processes the train abort request in the Voice Services task 
  command queue.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_train_abort_processing 
(
  vs_train_cb_type         abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_abort_processing

DESCRIPTION
  This function processes the recognize abort request in the Voice Services 
  task command queue.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recognize_abort_processing
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vactivate_abort_processing

DESCRIPTION
  This function processes the voice activate abort request in the Voice 
  Services task command queue.

DEPENDENCIES
  FEATURE_PHONE_VR is defined.

RETURN VALUE
  None

SIDE EFFECTS
  vs_play_cb

===========================================================================*/
extern void vs_vactivate_abort_processing
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

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
extern void vs_menu_selection_abort_processing (
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);
/*===========================================================================

FUNCTION vs_endpoint

DESCRIPTION
  This function queues a request to execute the VR end point detection in VS 
  task context.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_endpoint 
(
  uint8                         *buf_ptr
  /* Ptr to buffer containing voc frame + vr diagnostic data + frame no    */
); 

/*===========================================================================

FUNCTION vs_capture_start

DESCRIPTION
  This function initializes the VS task for a capture operation.

DEPENDENCIES
  The current DSP image must support VR Mode B/C.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_capture_start (void);


#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/*===========================================================================

                        VOICE MEMO FUNCTION PROTOTYPES

===========================================================================*/

extern void vm_init(void);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_start_processing

DESCRIPTION
  This function starts recording the utterances for VoiceMemo using the 
  specified   configuration information.  Will finish recording if the 
  SND task reports error,  or Voice Database reports error or reached the 
  maximum capacity at the storage.

    ->  promptList,             list of prompts to use during processing 
                                of the function.
    ->  record_callback,        A pointer to the call back function

DEPENDENCIES
  None.

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
extern void vs_vm_record_start_processing
(
    vs_sound_type**         promptList,
    vs_snd_direction_type   rec_direction,
    vs_vm_record_cb_type    record_callback
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_stop_processing

DESCRIPTION
  This function stops recording the memo specified.
    ->    vmid,        VoiceMemo id to stop        
    ->    synchmode,   stop(synch) or asynch_stop

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
extern void vs_vm_record_stop_processing
(
    vdb_vmemo_id_type    vmid,
    boolean		 synchmode
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_pause_processing

DESCRIPTION
  This function pauses recording the memo specified
    ->    vmid,        VoiceMemo id to pause        

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_PAUSED,            Recording has been paused successfully.
    VS_VM_INVALID_MEMO_ID    Specified memo is invalid

SIDE EFFECTS
  Once the record_pause has been called, 'record_resume' has to be called to
  resume recording.  
  Calling "record_start" won't resume recording but generate unexpected result
  Calling "record_stop" will stop and put it out ot "record_pause" mode.
===========================================================================*/
extern void vs_vm_record_pause_processing
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_resume_processing

DESCRIPTION
  This function resumes recording the memo specified.
    ->    vmid,        VoiceMemo id to resume        

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_RESUMED,           Recording has been resumed successfully
    VS_VM_INVALID_MEMO_ID    Specified memo is invalid


SIDE EFFECTS
  Once the recording has been paused, 'record_resume' has to be called to
  resume recording.  
  Calling "record_start" won't resume recording but generate unexpected result
  Calling "record_stop" will stop and put it out ot "record_pause" mode.
===========================================================================*/
extern void vs_vm_record_resume_processing
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_cancel_processing

DESCRIPTION
  This function cancels recording the memo specified.  
  Removes the voice memo that might have been stored in the storage.  
  The specified vmid is no longer valid.

    ->    vmid,        VoiceMemo id to cancel        

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_CANCELED,         Cancelled recording of the voice memo specified
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid


SIDE EFFECTS
 user can't use "record_cancel" to remove the memo, after it had already called
  "record_stop" previously on the same memo.
===========================================================================*/
extern void vs_vm_record_cancel_processing
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================
FUNCTION VS_VM_CAN_RECORD_MEMO_PROCESSING

DESCRIPTION
  This function determines whether there is sufficient storage space to
  record another voice memo.  If so, TRUE is returned.  Otherwise,
  FALSE is returned.

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
boolean vs_vm_can_record_memo_processing
(
    void
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_start_processing

DESCRIPTION
  This function plays the specified voice memo.  
  After the completion of the specified number of miliseconds
  (20 should be the minium value and multiple of 20) of the memo, 
  the callback_ptr function will be called.

    ->    vmid,                  VoiceMemo id to pause
    ->    play_rpt_interval_ms,  Number of miliseconds to play for 
                                 each notification
    ->    callback_ptr,          A pointer to the call back function

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_STARTED             Got the VM from the storage to be played
    VS_VM_REQUESTED_MS        Played the specified amount of miliseconds.
    VS_VM_INVALID_MEMO_ID     Specified memo is invalid
    VS_VM_INVALID_PARAMETER   min of  20 and  multiple of 20 for interval
    VS_VM_FINISHED            Finished playing the voice memo, EOF

SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_start_processing
(
    vdb_vmemo_id_type     vmid,
    uint32                play_rpt_interval_ms,
    vs_snd_direction_type play_direction,
    vs_vm_play_cb_type    callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_stop_processing

DESCRIPTION
  This function stops playing the specified voice memo
    ->    vmid,            VoiceMemo id to stop
    ->    synchmode,       stop(synch) or asynch_stop

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_STOPPED,            Stopped playing memo successfully.
    VS_VM_INVALID_MEMO_ID,    Specified memo is invalid 

SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_stop_processing
(
    vdb_vmemo_id_type    vmid,
    boolean		 synchmode
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_pause_processing
    ->    vmid,            VoiceMemo id to pause

DESCRIPTION
  This function pause playing the specified voice memo


DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_PAUSED,            Paused playing memo successfully.
    VS_VM_INVALID_MEMO_ID,   Specified memo is invalid


SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_pause_processing
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_resume_processing
    ->    vmid,            VoiceMemo id to resume

DESCRIPTION
  This function resume playing the specified voice memo


DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_RESUMED            Resumed playing memo successfully.
    VS_VM_INVALID_MEMO_ID,   Specified memo is invalid


SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_resume_processing
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_fastforward_processing

DESCRIPTION
  This function FastForwards the specified amount of seconds and continues 
  playing the specified voice memo

    ->    vmid,           VoiceMemo id to identify the function 
    ->    seconds,        requested number of seconds to be fastforwarded

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_FINISHED,            Finished playing the memo, reached EOF.
    VS_VM_FORWARDED,           Forwarded and playing the memo.
    VS_VM_INVALID_MEMO_ID,     Specified memo is invalid
    VS_VM_INVALID_PARAMETER    Can't be negative or zero to skip fastforward


SIDE EFFECTS
  Before jumping ahead specified amount of seconds, the current position is 
  considered to be the very first frame of the current "second"(truncated). 
  It'll always start playing at the beginning of the "second" boundary. 
  (Current position at 5.6 sec FF 4 seconds  plays from 9 seconds into 
 the memo).

===========================================================================*/
extern void vs_vm_play_fastforward_processing
(
    vdb_vmemo_id_type   vmid, 
    uint32              seconds
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_rewind_processing

DESCRIPTION
  This function rewinds the specified amount of seconds and continues playing
  the specified voice memo

    ->    vmid           VoiceMemo id to identify the function 
    ->    seconds        requested number of seconds to be rewound


DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_REWOUND,            Rewound and playing the memo.
    VS_VM_INVALID_MEMO_ID,    Specified memo is invalid
    VS_VM_INVALID_PARAMETER   Can't be negative or zero to skip backwards.

SIDE EFFECTS
    Before jumping back specified amount of seconds, the current position is 
    considered to be the very first frame of the current "second"(truncated).
    It'll always start playing at the beginning of the "second" boundary. 
    (Current position at 5.6 sec… Rewind 4 seconds… plays from 1 second into
    the memo).Will play from the beginning of the memo if the requested rew 
    seconds become  a negative number based on the current position.
===========================================================================*/
extern void vs_vm_play_rewind_processing
(
    vdb_vmemo_id_type   vmid,
    uint32              seconds
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_set_parameters_processing

DESCRIPTION
  This function sets the configuration settings for recording.

  This is an optional call as Voice Memo has a default value for each 
  attribute.

    ->    *config_data_ptr

       which points to a structure with:
          capability.
          frame_format,
          max_rate,
          min_rate,
          rec_type,
          record_report_interval_ms,
          warning_at_t_minus_ms,
          maximum_system_vm_sec,
          maximum_individual_vm_sec,

    ->    callback_ptr

DEPENDENCIES
  None.

RETURN VALUE via the call back function
    VS_VM_SUCCESS           Successfully set configuration values.
    VS_VM_INVALID_PARAMETER Invalide parameter was set

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_vm_set_parameters_processing
(
    vs_vm_config_type           *config_data_ptr,
    vs_vm_control_cb_type        callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_get_parameters_processing

DESCRIPTION
  This function returns the configuration settings for recording.

  This is an optional call as Voice Memo has a default value for each 
  attribute.

   <->    config_data_ptr
    ->    callback_ptr

DEPENDENCIES
  None.

RETURN VALUE via the call back function
  VS_VM_SUCCESS           Successfully set configuration values.

SIDE EFFECTS
  If the callback_ptr is invalid, there is no way to report back any status.

===========================================================================*/
extern void  vs_vm_get_parameters_processing
(
    vs_vm_config_type           *config_data_ptr,
    vs_vm_control_cb_type        callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION vm_rec_cb_from_cmx_processing

DESCRIPTION
  This function processed the callback function from CMX.

  Based on the status, call the appropriate local functions to process
  the recording result.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void vm_rec_cb_from_cmx_processing
(
  cmx_status_type status
);

/* <EJECT> */
/*===========================================================================

FUNCTION vm_play_cb_from_cmx_processing

DESCRIPTION
  This function processes the call back from CMX for the playback
  functions.

  Calls the appropriate local functions to process different statuses.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void vm_play_cb_from_cmx_processing
(
  cmx_status_type status
);

#endif /* FEATURE_VOICE_MEMO */

/* <EJECT> */
#endif /* FEATURE_VS_TASK */
#endif /* VSDEF_H */

