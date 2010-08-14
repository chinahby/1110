#ifndef VS_H
#define VS_H
/*===========================================================================

    V O I C E    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface 
  with Voice Services.

REFERENCES
  Voice Services ISOD: 80-24875-6

Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated. All Rights Reserved.
Copyright (c) 2002             by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/common/vs/main/latest/src/vs.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/02   yl      Changed the NUM_OF_RECORDING_PROMPTS and added the 
                   new entry to vs_vm_status_type.
07/11/02   jkl     Added list_ptr on vs_pufe_rec_utterance.
02/25/02   jkl     Added structures for PUFE under FEATURE_VR_PUFE.
09/18/01   jkl     Modified to include vs_set_vr_engine.
09/14/01   jkl     Added GPS support and Voice Memo QCP format.
07/26/01   jkl     Corrected featurization.
07/17/01    ch     Removed vs_implicit_adaptation().  No longer needed.
06/18/01    ch     Added synchronous VoiceMemo record stop and play stop
06/12/01   jkl     Removed get/set recognition parameters functions.
05/21/01   jkl     Added vs_recognize_multi. Removed ENHANCED VR Feature.
04/11/01   jkl     Added vs_menu_selection.
09/13/00   jkl     Removed _enhanced_ from vs_enhanced_train_utterance.
09/12/00   jkl     Renamed and kept same function names for different engines.
08/22/00   jkl     Added vs_modebc_train_control_word ().
08/22/00    vl     Added VS_VOC_ABORTED (used in VS layer only).
07/28/00    ro     Updated function prototypes for mem_to_qcp, qcp_to_mem.
07/27/00    vl     Deleted obsolete function vs_modebc_endpoint_setting ().  
07/21/00   dpo     Added vs_modebc_train_utterance (needed to be able to do 
                   VR mode B/C duplicate detection).
07/14/00    ch     Added memo to qcp and qcp to memo conversion support.
                   Added FEATURE_VR_IMPLICIT_ADAPTATION.
06/30/00   dpo/vl  Added function vs_modebc_diagnostic_settings ().
06/28/00    vl     Added function vs_modebc_continuous_recognition_enable ().
06/22/00    vl     Added mode B VR support.
06/14/00   jkl     Modified for VDB 2.0.
04/13/00    st     Added proper featurization.
02/11/00    st     Replaced vs_vactivate_cb_type with vs_rec_cb_type.
02/03/00    st     Added VS_BAD_VP to vs_status_type.
01/25/00    ro     Support for MSM3100.  Remove recognition thresholds from
                   vs_vr_config_type.
            st     Added support voice activate.
08/02/99    ch     Added vs_vm_can_record_memo function.
07/19/99    vl     Added play direction type to play functions.
                   Added VS_VM_DIFF_SO enum for vs_vm_status_type.
05/24/99    ch     vs_vm_control_info has fewer fields
                   vs_vm_get_parameters() has another parameter
05/24/99    sa     Made changes to training and recognition interfaces to support
                   VDB recognizable word sets.
05/13/99    sa     Removed vs_match_type.
05/13/99    ro     Added utt_capture_timeout_msec, capture_features, and
                   max_vocabulary_words to vs_vr_config_type.
05/10/99    ch     Supports CANNED UI parameters for set_confing and get_config.
                   Supports direction in play_start and record_start.
05/06/99    vl     Implemented callback handshaking functions.  Changed abort    
                   functions to comply with Voice Services API.                  
04/21/99    vl     Added VS_ABORT_SUCCESS and VS_ABORT_FAILURE to 
                   vs_status_type.
04/16/99    vl     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#ifdef FEATURE_VS_TASK
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "snd.h"        /* Sound typedefs and prototypes                   */
#ifdef FEATURE_VOICE_DATABASE
#include "vdbrw.h"      /* Voice Database interfaces                       */
#endif

#ifdef FEATURE_VOICE_MEMO
#include "vdbvmemo.h"
#endif /* FEATURE_VOICE_MEMO */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
** VS general definitions
**---------------------------------------------------------------------------
*/

/* Usually, this code is returned as a parameter in Voice Services call back 
** functions.
*/
typedef enum
{
  VS_SUCCESS = 0,       /* VR/VM function was successful                   */
  VS_FAILURE,           /* VR/VM function failed                           */
  VS_CONTINUING,        /* VR/VM function completed stage, go to next      */
  VS_TIMEOUT,           /* VR/VM function timed out                        */
  VS_ABORTED,           /* VR/VM function aborted                          */
  VS_ABORT_SUCCESS,     /* VR/VM abort function was successful             */
  VS_ABORT_FAILURE,     /* VR/VM abort function failed                     */
  VS_TOO_SOON,          /* User spoke to soon on utterance capture         */
  VS_TOO_SHORT,         /* utterance captured was too short                */
  VS_TOO_LONG,          /* utterance captured was too long                 */
  VS_NBEST,             /* recognition has gone into N-best                */
  VS_DUPLICATE,         /* Training found duplicated templates             */
  VS_UNMATCHED,         /* training or recognition produced no match       */
  VS_YESNO_TIMEOUT,     /* YES/NO Device timed out                         */
  VS_YESNO_TOO_SOON,    /* User spoke YES/NO too soon                      */
  VS_YESNO_TOO_SHORT,   /* YES/NO capture too short                        */
  VS_YESNO_TOO_LONG,    /* YES/NO capture too long                         */
  VS_YESNO_UNMATCHED,   /* YES/NO produced no match                        */
  VS_YES_SUCCESS,       /* YES recognition successful                      */
  VS_NO_SUCCESS,        /* NO recognition successful                       */
  VS_VDB_ERR,           /* Voice DB error                                  */
  VS_FFS_ERR,           /* Flash File System error                         */
  VS_NV_MEM,            /* NV memory operation failed                      */
  VS_IDLE,              /* VS operation is idle                            */
  VS_INPROGRESS,        /* VS operation in progress                        */
  VS_DIFF_SO,           /* Play request with different SO was rejected     */
  VS_BAD_VP,            /* Play request with bad voice prompt was rejected */
  VS_STOP_CAPTURE,      /* Intermediate status (used in VS only)           */
  VS_VOC_ABORTED,       /* VR op aborted by Voc driver (used in VS only)   */   
  VS_ERR_MAX            /* Maximum number of status/error codes            */

} vs_status_type;

#ifdef FEATURE_PHONE_VR
/* Control values for utterance capture 
*/
typedef enum {
  VS_CAPTURE_STS_DIS,               /* Spoke-too-Soon Disable             */
  VS_CAPTURE_STS_ENA                /* Spoke-too-Soon Enable              */
} vs_capture_features_type;

/* Return type for intra word gap. Usually Digits are short
** and/or smaller gaps/pause in between syllables.
** The option DIGIT allows faster recognition and the 
** DEFAULT allows longer words.
*/
typedef enum {
  VS_RECOG_DEFAULT,
  VS_RECOG_DIGIT
} vs_rec_multi_type;

/* Type of Engine to use. This allows option to use only SD templates.
*/
typedef enum {
  VS_PATT_SD,
  VS_PATT_SD_SI
} vs_patt_settings_type;

#endif /* FEATURE_PHONE_VR */

#if (defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK))
/* This type specifies an utterance in the Voice Database. */
typedef struct {
  vdb_rwid_type utt_id;      /* ID of the recog. word in DB        */
  short         utt_index;   /* index of the utt in the recog. word*/
} vs_utt_type;


/* This type specifies a playable sound. */
typedef struct {
   snd_tone_type  sound;
   word           duration;
   vdb_rwid_type  utt_id;
   word           utt_index;
} vs_sound_type;

/* Direction to play or record. */
typedef enum 
{ 
   VS_SND_REVERSE, 
   VS_SND_FORWARD, 
   VS_SND_BOTH 
} vs_snd_direction_type;

/* Type for echoing or playing back utterance during training      */
typedef enum {
  VS_UTT_ECHO_NONE, /* Do not playback the utterance after capture */
  VS_UTT_ECHO_ALL,  /* Playback the utterance after each capture   */
  VS_UTT_ECHO_LAST, /* Echo after the last capture only            */
  VS_UTT_ECHO_FIRST,/* Echo after first capture only               */
  VS_UTT_ECHO_MAX   /* Maximum number of echo types                */
} vs_utt_echo_type;

/* Include list in the Menu Selection */
typedef enum 
{ 
   VS_MENU_LIST_INCLUDED,
   VS_MENU_LIST_NOT_INCLUDED
} vs_menu_sel_type;

/* Order on which the prompts are to be played */
typedef enum 
{ 
   VS_MENU_RWID_PROMPT,
   VS_MENU_PROMPT_RWID
} vs_menu_play_prompt_type;

/* A pointer to a function returning void with 1 parameter.        */
typedef void (*vs_play_cb_type) (vs_status_type);

#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
/* vs_handshake_cb_type for handshaking with UI.                   */
typedef void (*vs_handshake_cb_type) (void);

/* A pointer to a function returning void with 3 parameters,       */
/* vs_status_type for status, vdb_rwid_type for voice database     */
/* ID, and vs_handshake_cb_type for handshaking with the UI        */
/* For control word training the ID passed in is echoed back       */
typedef void (*vs_train_cb_type) (vs_status_type, vdb_rwid_type,
                                      vs_handshake_cb_type);

/* A pointer to a function returning void with 3 parameters,       */
/* vs_status_type for status, short for number of valid matches,   */
/* and vs_handshake_cb_type for passing back results of recogntion.*/
typedef void (*vs_rec_cb_type) (vs_status_type, vdb_rwid_type,
                                    short, vs_handshake_cb_type);

/* A pointer to a callback function for passing back results of    */
/* a control operation.                                            */
typedef void (*vs_control_cb_type)
(
   vs_status_type      status
);

#endif /* FEATURE_PHONE_VR */


/*---------------------------------------------------------------------------
** VS Public Function Prototypes
**---------------------------------------------------------------------------
*/

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
extern void vs_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
);

/* <EJECT> */
#if (defined(FEATURE_VOICE_DATABASE) && defined (FEATURE_VOICE_PLAYBACK))
/*===========================================================================

FUNCTION vs_play_utterance

DESCRIPTION
  Requests Voice Services to play the specified utterance from the database.

===========================================================================*/

extern void vs_play_utterance 
(
  vdb_rwid_type     utterance_id,
  short             utterance_index,
  vs_snd_direction_type   play_direction,
  vs_play_cb_type   cb_func
);


/* <EJECT> */
/*===========================================================================

FUNCTION vs_play_multi

DESCRIPTION
  Requests Voice Services to play the specified list of sounds.

===========================================================================*/

extern void vs_play_multi 
(
  vs_sound_type*    multi_list,
    /* Pointer to a list of sounds to play. */

  vs_snd_direction_type   play_direction,
    /* Direction to play multi sounds. */

  vs_play_cb_type   cb_func
    /* Function to call when playback is complete. */
);



/* <EJECT> */
/*===========================================================================

FUNCTION VS_PLAY_ABORT

DESCRIPTION
  This function aborts the current play back function in Voice Services (if
  any).

===========================================================================*/
extern void vs_play_abort 
(
  vs_play_cb_type   abort_callback_ptr
    /* Function to call when playback abort is complete. */
);

#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
/* <EJECT> */
/*===========================================================================

FUNCTION VS_HANDSHAKE_ENABLE

DESCRIPTION
  This function enables handshaking for synchronization on callbacks from
  Voice Services.

===========================================================================*/
extern void vs_handshake_enable
(
  vs_control_cb_type       callback_ptr 
    /* Function to call after handshaking is enabled. */
);


/* <EJECT> */
/*===========================================================================

FUNCTION VS_HANDSHAKE_DISABLE

DESCRIPTION
  This function disables handshaking for synchronization on callbacks from
  Voice Services.

===========================================================================*/
extern void vs_handshake_disable
(
  vs_control_cb_type       callback_ptr
    /* Function to call after handshaking is disabled. */
);



/* <EJECT> */
/*===========================================================================

FUNCTION vs_train_abort

DESCRIPTION
  This function aborts the current train process. 

===========================================================================*/

extern void vs_train_abort 
(
  vs_train_cb_type         abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_implicit_yes

DESCRIPTION
  This function signals the VS task to start implicit yes control function.  
  The caller routine passes a call back function for VS to call back at 
  the end of the implicit yes process.  The call back function returns
  intermediate status and the final result of implicit yes.

===========================================================================*/

extern void vs_implicit_yes
(
  vs_sound_type**     prompt_list, /* List of prompts for implicit yes    */
  vs_rec_cb_type      cb_back_ptr /* ptr to funct when implicit yes done */
);



/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_abort

DESCRIPTION
  This function aborts the current recognition process.  

===========================================================================*/

extern void vs_recognize_abort 
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_multi_abort

DESCRIPTION
  This function aborts the current recognition process.  

===========================================================================*/

extern void vs_recognize_multi_abort 
(
  vs_rec_cb_type      abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY)  || \
    defined (FEATURE_VIDEO_ENCODE_FADING)

/*===========================================================================

FUNCTION vs_queue_images_ready_camera_cmd

DESCRIPTION
  This function queues a camera images ready command to VS task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern int vs_queue_images_ready_camera_cmd(void* buffer, int32 count);
#endif
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
  vs_sound_type**      prompt_list,     /* List of prompts for recognition */
  vdb_rw_list_type*    list_ptr,        /* recognizable word set ID        */
  vdb_rwid_type        rwid,            /* recognizable word ID            */
  vs_rec_cb_type       cb_back_ptr      /* pointer to funct when done      */
);

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
extern void vs_voice_activate_abort 
(
  vs_rec_cb_type            abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_menu_selection

DESCRIPTION
  This function starts the listing Names/Commands and accepts voice in
  between prompts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_menu_selection
(
  vs_sound_type**      prompt_list,     /* List of prompts for recognition */
  vdb_rw_list_type*    list_ptr,        /* recognizable word set ID        */
  vs_menu_sel_type     include_list,    /* Include List to be recognized   */
  vs_menu_play_prompt_type order_prompt,/* Order of the prompts            */
  vs_rec_cb_type       cb_back_ptr      /* pointer to funct when done      */
);

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
extern void vs_menu_selection_abort
(
  vs_rec_cb_type            abort_callback_ptr
    /* Function to call when the abort operation is complete. */
);

#endif /* FEATURE_PHONE_VR */
/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO

/*===========================================================================

                        VOICE MEMO DATA DECLARATIONS

===========================================================================*/
#define NUM_OF_RECORDING_PROMPTS    4       /* 4 prompts used for recording*/

typedef enum
{
    VS_FMT_DEFAULT,
    VS_FMT_PUREVOICE,
    VS_FMT_EVRC
} vs_rec_format_type;

typedef enum
{
    VS_MODE_DEFAULT,
    VS_MODE_STANDARD,
    VS_MODE_LONG
} vs_rec_mode_type;

typedef struct vs_vm_config_struct {
    vs_rec_format_type      rec_format;
    vs_rec_mode_type        rec_mode;
    uint16                  record_report_interval_ms;
    uint32                  warning_at_t_minus_ms;
    uint32                  max_system_vm_sec;
    uint32                  max_individual_vm_sec;
} vs_vm_config_type;

/* VOICE MEMO record and play status values back to the client */
typedef enum {
    VS_VM_MEMO_ID = 0,     /* voice id returend by the database        */
    VS_VM_REQUESTED_MS,     /* collected or played the requested frames */
    VS_VM_WARNING_MARK,     /* reached the warning level for recording  */
    VS_VM_MAX_MARK,         /* reached the maximum level for recording  */
    VS_VM_STARTED,          /* started successfully                     */
    VS_VM_PAUSED,           /* paused successfully                      */
    VS_VM_RESUMED,          /* resumed successfully                     */
    VS_VM_CANCELED,         /* cancelled recording successfully         */
    VS_VM_STOPPED,          /* user had asked to be stopped             */
    VS_VM_AUTO_STOP,        /* VM stopped recording automatically by    */
                            /* detecting the end of the speech          */
    VS_VM_FINISHED,         /* VM subsystem finished on its own,        */
                            /* no more resource or eof                  */
    VS_VM_REWOUND,          /* buffer was rewond                        */
    VS_VM_FORWARDED,        /* buffer was forwarded                     */
    
    VS_VM_INVALID_MEMO_ID,  /* invalid memo id to access VDB            */
    VS_VM_INVALID_PARAMETER,/* one or more of the parameter was invalid */

    VS_VM_SUCCESS,          /* should be used for set & get config      */
    VS_VM_ERR_DATABASE,     /* database can't support recording/playing */
    VS_VM_ERR_VOC,          /* voc layer can't support recording/playing*/
    VS_VM_ERR_RAM,          /* couldn't get the buffer for playing      */
    VS_VM_DIFF_SO           /* different Service Option from file's     */
} vs_vm_status_type;

typedef struct {

    /* status returned for the previous call made to VM*/
    vs_vm_status_type    vs_vm_record_status;

    /* vmid that was just created for recording, */
    /* only valid for VS_VM_MEMO_ID */
    vdb_vmemo_id_type    vmid;

} vs_vm_record_info;

typedef struct {

    vs_vm_status_type    vs_vm_play_status;

} vs_vm_play_info;

typedef struct 
{

  vs_vm_status_type     vs_vm_control_status;

} vs_vm_control_info;

/* A pointer to a function returning void with 1 parameter. */
typedef void ( *vs_vm_record_cb_type)   (vs_vm_record_info *vs_vm_rec_result);

/* A pointer to a function returning void with 1 parameter. */
typedef void ( *vs_vm_play_cb_type)     (vs_vm_play_info *vs_vm_play_result);

/* A pointer to a function returning void with 1 parameter. */
typedef void ( *vs_vm_control_cb_type)  (vs_vm_control_info *vs_vm_control_result);

/*===========================================================================

FUNCTION vs_vm_set_parameters

DESCRIPTION
  This function sets the configuration settings for recording.  
  This is an optional call as Voice Memo has a default value for each 
  attribute.

    <-    config_data_ptr.
    <-    callback_ptr,

DEPENDENCIES
  None.

RETURN VALUE
    VS_VM_INVALID_PARAMETER,     Invalid parameter was set
    VS_VM_SUCCESS                Successfully set configuration values.

SIDE EFFECTS
  None


===========================================================================*/
extern void  vs_vm_set_parameters
(
  vs_vm_config_type           *config_data_ptr,
  vs_vm_control_cb_type        callback_ptr
);


/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_get_parameters

DESCRIPTION
  This function returns the configuration settings for recording

   <->    config_data_ptr
    ->    callback_ptr,

DEPENDENCIES
  None.

RETURN VALUE via callback
    VS_VM_INVALID_PARAMETER,     Invalid parameter was set
    VS_VM_SUCCESS                Completed successfully

SIDE EFFECTS
  None

===========================================================================*/
extern void  vs_vm_get_parameters
(
  vs_vm_config_type           *config_data_ptr,
  vs_vm_control_cb_type        callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_start

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

extern void vs_vm_record_start
(
    vs_sound_type         **promptList,
    vs_snd_direction_type   rec_direction,
    vs_vm_record_cb_type    record_callback
);


/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_stop

DESCRIPTION
  This function stops recording the memo specified.
    ->    vmid,        VoiceMemo id to pause        

DEPENDENCIES
  The caller is expected to remove the memo via DB calls, if the caller 
  decides  not to save the memo after 'record_stop' was called
  (record_cancel will remove the memo).

RETURN VALUE IN A CALLBACK
    VS_VM_STOPPED,          Recording has been stopped successfully.
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid

SIDE EFFECTS
  caller can't call "record_cancel" to remove the memo, after it has called
  "record_stop" previously on the same memo.
===========================================================================*/
extern void vs_vm_record_stop
(
   vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_synch_stop

DESCRIPTION
  This synchronous function stops recording the memo specified.
    ->    vmid,        VoiceMemo id to pause

DEPENDENCIES
  The caller is expected to remove the memo via DB calls, if the caller
  decides  not to save the memo after 'record_stop' was called
  (record_cancel will remove the memo).

RETURN VALUE IN A CALLBACK
   There is no callback

SIDE EFFECTS
  caller can't call "record_cancel" to remove the memo, after it has called
  "record_stop" previously on the same memo.
===========================================================================*/
extern void vs_vm_record_synch_stop
(
   vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_pause

DESCRIPTION
  This function pauses recording the memo specified
    ->    vmid,        VoiceMemo id to pause        

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_PAUSED,           Recording has been paused successfully.
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid

SIDE EFFECTS
  Once the record_pause has been called, 'record_resume' has to be called to
  resume recording.  
  Calling "record_start" won't resume recording but generate unexpected result
  Calling "record_stop" will stop and put it out ot "record_pause" mode.
===========================================================================*/
extern void vs_vm_record_pause
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_resume

DESCRIPTION
  This function resumes recording the memo specified.
    ->    vmid,        VoiceMemo id to pause        

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
extern void vs_vm_record_resume
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_record_cancel

DESCRIPTION
  This function cancels recording the memo specified.  
  Removes the voice memo that might have been stored in the storage.  
  The specified vmid is no longer valid.

    ->    vmid,        VoiceMemo id to pause        

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_CANCELED,         Cancelled recording of the voice memo specified
    VS_VM_INVALID_MEMO_ID   Specified memo is invalid


SIDE EFFECTS
 user can't use "record_cancel" to remove the memo, after it had already called
  "record_stop" previously on the same memo.
===========================================================================*/
extern void vs_vm_record_cancel
(
    vdb_vmemo_id_type    vmid
);
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
extern boolean vs_vm_can_record_memo
(
    void
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_start

DESCRIPTION
  This function plays the specified voice memo.  
  After the completion of the specified number of miliseconds
  (20 should be the minium value and multiple of 20) of the memo, 
  the callback_ptr function will be called.

    ->    vmid,                 VoiceMemo id to pause
    ->    play_rpt_interval_ms, Number of miliseconds to play for 
                                each notification
    ->    callback_ptr,         A pointer to the call back function

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_STARTED              Got the VM from the storage to be played
    VS_VM_REQUESTED_MS         Played the specified amount of miliseconds.
    VS_VM_INVALID_MEMO_ID, Specified memo is invalid
    VS_VM_INVALID_PARAMETER    min of  20 and  multiple of 20 for interval
    VS_VM_FINISHED             Finished playing the voice memo, EOF

SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_start
(
    vdb_vmemo_id_type     vmid,
    uint32                play_rpt_interval_ms,
    vs_snd_direction_type play_direction,
    vs_vm_play_cb_type    callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_stop

DESCRIPTION
  This function stops playing the specified voice memo
    ->    vmid,            VoiceMemo id to stop

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    VS_VM_STOPPED,            Stopped playing memo successfully.
    VS_VM_INVALID_MEMO_ID,    Specified memo is invalid 

SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_stop
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_synch_stop

DESCRIPTION
  This synchronous function stops playing the specified voice memo
    ->    vmid,            VoiceMemo id to stop

DEPENDENCIES
  None.

RETURN VALUE IN A CALLBACK
    There is no callback 

SIDE EFFECTS
  None
===========================================================================*/
extern void vs_vm_play_synch_stop
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_pause
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
extern void vs_vm_play_pause
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_resume
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
extern void vs_vm_play_resume
(
    vdb_vmemo_id_type    vmid
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_fastforward

DESCRIPTION
  This function FastForwards the specified amount of seconds and continues 
  playing the specified voice memo

    ->    vmid,            VoiceMemo id to identify the function 
    ->    seconds,        requested number of seconds to be fastforwarded

DEPENDENCIES
  None.

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
extern void vs_vm_play_fastforward
(
    vdb_vmemo_id_type   vmid,
    uint32              seconds
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_vm_play_rewind

DESCRIPTION
  This function rewinds the specified amount of seconds and continues playing 
  the specified voice memo

    ->    vmid            VoiceMemo id to identify the function 
    ->    seconds        requested number of seconds to be rewound


DEPENDENCIES
  None.

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
extern void vs_vm_play_rewind
(
    vdb_vmemo_id_type    vmid,
    uint32               seconds
);

#endif /* FEATURE_VOICE_MEMO */

#ifdef FEATURE_PHONE_VR
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
extern void vs_train_utterance
(
  vs_sound_type**     prompt_list,      /* List of prompts for training    */
  vs_utt_echo_type    echo_utterance,   /* echo utterance to user or not   */ 
  vdb_rw_list_type**  list_ptr,         /* recognizable word set ID        */ 
  vs_train_cb_type    cb_back_ptr       /* ptr to funct when training done */
);

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
extern void vs_train_control_word
(
  vs_sound_type**          prompt_list,      /* List of prompts for training    */
  vs_utt_echo_type         echo_utterance,   /* echo utterance to user or not   */
  vdb_rwid_type            train_word,       /* RWID of control word to train   */  
  vdb_rw_list_type**       list_ptr,         /* List of RWID set                */ 
  vs_train_cb_type         cb_back_ptr       /* ptr to funct when training done */
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_recognize_utterance

DESCRIPTION
  This function signals the VS task to start the recognize utterance process.  
  The caller routine passes a call back function for VS to call back at 
  the end of the recognition process.  The call back function returns the 
  result of recognition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_recognize_utterance
(
  vs_sound_type**          prompt_list, /* List of prompts for recognition */
  vdb_rw_list_type**       list_ptr,    /* List of recognizable word sets  */
  boolean                  use_n_best,  /* set to use n-best resolution    */
  short                    match_count, /* number of top candidates to keep*/
  vs_rec_cb_type           cb_back_ptr  /* pointer to funct when recog done*/
);

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
);

/* <EJECT> */
/*===========================================================================

FUNCTION vs_set_vr_engine

DESCRIPTION
  This function sets the engine to use: Speaker Dependent only or Hybrid.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void vs_set_vr_engine (
  vs_patt_settings_type pattern_settings
);
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
extern int vs_get_max_vocabulary_size ( void );

#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */
#endif /* FEATURE_VS_TASK */
#endif /* VS_H */

