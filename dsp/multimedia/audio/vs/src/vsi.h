#ifndef VSI_H
#define VSI_H
/*===========================================================================

          V O I C E   S E R V I C E S   T A S K   I N T E R N A L   
                           H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to the VS task.

REFERENCES
  Voice Services ISOD: 80-24875-6

Copyright (c) 1999 - 2005 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/6k/common/vs/main/latest/src/vsi.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/28/07    pl     Moved SLEEP macro to vs.c
03/23/05   ali     Changes for raw encoder engine
03/17/05    lg     Added support for SVG. Using 0x0100 SIG for SVG Frames.
02/23/05   jkl     Fixed Featurization.
12/08/04    aw     Include "rex.h"
08/10/04   ali     Changes for Video Encode frame overlay
12/20/02   rpw     Added definition of VS_RSVD_0080_SIG for the video encoder
                   feature.
07/11/02   jkl     Added list_ptr on vs_pufe_rec_utterance.
02/25/02   jkl     Added PUFE definitions.
10/03/01   jkl     Removed obsolete definitions.
09/14/01   jkl     Updated Sleep macros.
06/18/01    ch     Added synchronous VoiceMemo record stop and play stop
06/12/01   jkl     Removed Set/Get Recognition.
05/21/01   jkl     Added vs_recognize_multi.
04/11/01   jkl     Added vs_menu_selection.
04/04/01    st     Added support for SAF; added proper featurization.
03/19/01   jkl     Defined VS_PB_NUM_BUFFERS to 2 for Speaker Dependent Build.
01/17/01    ch     Added rex_* fields to vs_hdr_type to support synch abort.
12/05/00    sm     VS_RSVD_0020_SIG now defined if FEATURE_CMX is not defined.
10/31/00   jkl     Added featurization when no need for Voice playback calls.
07/28/00    ro     Change vs command to VS_VM_MEM_TO_QCP.
07/21/00   dpo     Added vs_modebc_recognize_utt_processing (for VR mode B/C
                   duplicate detection).
                   Added maximum voice tag utterance size limit support
                   (07/18/00 ch)
07/14/00    ch     Added memo to qcp and qcp to memo conversion support.
06/22/00    vl     Added mode B VR support.  Moved local VS function 
                   prototypes from vs.c to vsi.h.
06/09/00   jkl     Modified for VDB 2.0.
04/13/00    st     Added proper featurization; use VS_RSVD_0020_SIG for CMX 
                   command signal. 
02/11/00    st     Replaced vs_vactivate_cb_type with vs_rec_cb_type.
02/03/00    st     Added vs_status_type and removed diff_so from
                   vs_play_end_packet_type.
01/25/00    ro     Support for MSM3100.
                   Added VS_VOC_INFORM_SIG. Remove CAPTURE and CAPTURE_ABORT.
            st     Added support for capture timeout and voice activate.
07/19/99    vl     Added play direction to play command packets.
05/24/99    ch     vs_vm_get_param_packet_type has another field.
05/24/99    sa     Made changes to training and recognition interfaces to support
                           VDB recognizable word sets.
05/20/99    vl     Reduced number of playback buffer from 3 to 2 to save RAM.
05/10/99    ch     Supports direction in play_start and record_start.
05/06/99    vl     Changed abort packets to comply with Voice Services API.                  
04/27/99    vl     Removed obsolete enums in vs_cmd_type and obsolete VS 
                   command packet types.
04/23/99    vl     Removed 2 unused enums in vs_cmd_type, changed 
                   vs_vm_rec_voc_cb_packet_type.
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
#include "queue.h"      /* Definitions for queue services                  */
#include "snd.h"        /* Sound typedefs and prototypes                   */
#include "vs.h"         /* VS typedefs and prototypes                      */
#include "voc.h"        /* Vocoder typedefs and prototypes                 */
#include "vsdef.h"      /* VS task common definitions                      */
#include "rex.h"        /* definition of REX data types                    */
#ifdef FEATURE_PHONE_VR
#include "vrengine.h"   /* VR Engine prototypes                            */
#endif /* FEATURE_PHONE_VR */
#ifdef FEATURE_VOICE_MEMO
#include "cmx.h"        /* Vocoder typedefs and prototypes                 */
#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
** VS task REX signal definitions
**---------------------------------------------------------------------------
*/

/* Reserved (unused) signals.  Note the Upper Nibble is reserved for
** system wide "common signals".  (See task.h.)
*/
#define VS_RSVD_10000_SIG         0x10000  /* Reserved for async writes    */
#define VS_RSVD_0800_SIG           0x0800
#define VS_RSVD_0400_SIG           0x0400
#define VS_RSVD_0200_SIG           0x0200
#ifndef FEATURE_SVG
#define VS_RSVD_0100_SIG           0x0100
#endif /* FEATURE_SVG */
#ifndef FEATURE_VIDEO_ENCODE
#define VS_RSVD_0080_SIG           0x0080
#endif /* FEATURE_VIDEO_ENCODE */
#ifndef FEATURE_SAF
#define VS_RSVD_0040_SIG           0x0040
#ifndef FEATURE_CMX
#define VS_RSVD_0020_SIG           0x0020
#endif /* FEATURE_CMX */
#endif /* FEATURE_SAF */

/* This signal indicates when the delay for vocoder release expires.
*/
#define VS_VOC_RELEASE_TIMER_SIG   0x0010

/* This signal indicates when voice prompt play back is finished.
*/ 
#define VS_VP_DONE_SIG             0x0008

/* This signal is used to wait for the vocoder to get ready.
*/
#define VS_VOC_INFORM_SIG          0x0004

/* This signal is set when an item is put on the vs_cmd_q.
*/
#define VS_CMD_Q_SIG               0x0002

/* This signal is set when the diag_rpt_timer expires.
*/
#define VS_RPT_TIMER_SIG           0x0001
 
/* <EJECT> */
/*---------------------------------------------------------------------------
** VS command packet types
**---------------------------------------------------------------------------
*/
typedef enum 
{                       /* (parameter) Brief description                   */
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
  VS_PLAYBACK,          /* Start single voice play back                    */
  VS_PLAY_MULTI,        /* Start multi voice play back                     */
  VS_PLAYBACK_ABORT,    /* Abort voice play back                           */
  VS_PLAYBACK_END,      /* End play back, need to reclaim RAM buffer       */
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
  VS_CAPTURE_END,       /* Capture is done                                 */ 
  VS_ENDPT_DETECT,      /* Endpoint detected                               */ 
  VS_TRAIN_WORD,        /* Train control word command                      */
  VS_TRAIN_UTT,         /* Train utterance command                         */
  VS_TRAIN_ABORT,       /* Abort both capture and play back                */
  VS_RECOGNIZE_UTT,     /* Recognize utterance command                     */
  VS_RECOGNIZE_MULTI,   /* Recognize utterance command                     */
  VS_RECOGNIZE_ABORT,   /* Abort both capture and play back                */
  VS_VOICE_ACTIVATE,    /* Start voice activation                          */
  VS_VOICE_ACTIVATE_ABORT,
                        /* Abort voice activation                          */
  VS_IMPLICIT_YES,      /* Implicit yes command                            */
  VS_MENU_SELECTION,    /* Start Menu Selection                            */
  VS_MENU_SELECTION_ABORT,
                        /* Abort menu selection                            */
  VS_SET_RECOG_PARAMS,  /* Set recognition parameters command              */
  VS_GET_RECOG_PARAMS,  /* Get recognition parameters command              */
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */
#endif /* FEATURE_PHONE_VR */
#ifdef FEATURE_VOICE_MEMO
  VS_VM_REC_START,       /* Voice Memo Record start */
  VS_VM_REC_STOP,
  VS_VM_REC_SYNCH_STOP,
  VS_VM_REC_PAUSE,
  VS_VM_REC_RESUME,
  VS_VM_REC_CANCEL,

  VS_VM_GET_PARAM, 
  VS_VM_SET_PARAM,
  VS_VM_PLAY_START,
  VS_VM_PLAY_STOP,
  VS_VM_PLAY_SYNCH_STOP,
  VS_VM_PLAY_PAUSE,
  VS_VM_PLAY_RESUME,
  VS_VM_PLAY_REWIND,
  VS_VM_PLAY_FASTFORWARD,

  VS_VM_REC_CMX_CB,
  VS_VM_PLAY_CMX_CB,
#endif /* FEATURE_VOICE_MEMO */
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined (FEATURE_VIDEO_ENCODE_FADING)  || \
    defined (FEATURE_VIDEO_ENCODE_RAW_ENGINE)
  VS_CAMERA_CMD,
#endif

  VS_INVALID            /* (Leave Last) Invalid Command - Causes Error     */

} vs_cmd_type;

/* <EJECT> */
/* Header for VS commands
*/
typedef struct 
{
  q_link_type         link;            /* Command queue link               */
  vs_cmd_type         cmd;             /* VS command to perform            */
  rex_sigs_type       sigs;            /* Rex signals for task             */
  rex_tcb_type        *tcb_ptr;        /* Task to signal                   */

} vs_hdr_type;

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
/* Play utterance packet
*/  
typedef struct 
{
  vs_hdr_type         hdr;             /* Command Header                   */
  vdb_rwid_type       ut_id;           /* RWID                             */
  short               ut_index;        /* Which utterance in the RWID      */
  vs_snd_direction_type    play_direction;  /* Direction to play utterance */
  vs_play_cb_type     cb;              /* Call back function               */

} vs_play_packet_type;

/* Play multi packet
*/  
typedef struct 
{
  vs_hdr_type         hdr;             /* Command Header                   */
  vs_sound_type       *mlist_ptr;      /* Pointer to multi list            */
  vs_snd_direction_type    play_direction;  /* Direction to play multi     */
  vs_play_cb_type     cb;              /* Call back function               */

} vs_play_multi_packet_type;

/* Play abort packet
*/  
typedef struct 
{
  vs_hdr_type         hdr;             /* Command Header                   */
  vs_play_cb_type     cb;              /* Call back function               */

} vs_play_abort_packet_type;

/* End Playback packet
*/
typedef struct 
{
  vs_hdr_type         hdr;             /* Command Header                   */
  voc_pb_header_type  *buf_ptr;        /* Pointer to buffer to reclaim     */
  vs_status_type      status;          /* Playback status                  */
} vs_play_end_packet_type;
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

/* <EJECT> */
#if defined (FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined (FEATURE_VIDEO_ENCODE_FADING) || \
    defined(FEATURE_VIDEO_ENCODE_RAW_ENGINE)
typedef enum
{
  VS_CAMERA_IMAGES_READY,              /* Images Ready command */
  VS_RAW_VIDEO_ENCODE_DONE,
  VS_CAMERA_MAX
} vs_camera_cmd_type;
/* Camera packet
*/
typedef struct 
{

  vs_hdr_type                      hdr;             /* Command Header      */
  vs_camera_cmd_type               msg;             /* Command Message     */
  void                             *buf;            /* Pointer to data     */
  uint32                           size;            /* Size of Data        */
} vs_camera_packet_type;       
#endif

#ifdef FEATURE_PHONE_VR
/* Capture end packet
*/
typedef struct 
{
  vs_hdr_type             hdr;         /* Command Header                   */
  vs_status_type          status;      /* Capture status                   */

} vs_capture_end_packet_type;

/* recognize utterance packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_sound_type**          play_list;     /* List of prompts for recognition */
  vdb_rw_list_type**       list_ptr;      /* List of recognizable word sets  */
  boolean                  n_best;        /* set to use n-best resolution    */
  short                    match_cnt;     /* number of top candidates to keep*/
  vs_rec_cb_type           cb;            /* pointer to funct when recog done*/

} vs_recog_packet_type;

/* recognize multi utterance packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_sound_type**          play_list;     /* List of prompts for recognition */
  vdb_rw_list_type*        list_ptr;      /* List of recognizable word sets  */
  vdb_rw_list_type*        list_prompt_ptr;/* List of recognizable word sets */
  vs_rec_multi_type        gap_mode;      /* Intra Word gap for recognition  */
  vs_rec_cb_type           cb;            /* pointer to funct when recog done*/

} vs_recog_multi_packet_type;

/* train utterance packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_sound_type**          play_list;     /* List of prompts for recognition */
  vs_utt_echo_type         echo_utt;      /* echo utterance to user or not   */
  vdb_rwid_type            control_word;  /* RWID of control word to train   */  
  vdb_rw_list_type**       list_ptr;      /* List of recognizable word sets  */
  vs_train_cb_type         cb;            /* pointer to funct when recog done*/

} vs_train_packet_type;

/* abort training packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_train_cb_type         cb;            /* ptr to funct when abort    done */

} vs_train_abort_packet_type;


/* abort recognition packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_rec_cb_type           cb;            /* pointer to funct when abort done*/

} vs_recog_abort_packet_type;

/* voice activation packet
*/
typedef struct
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_sound_type**          play_list;     /* List of prompts for activation  */
  vdb_rw_list_type*        rwset_id;      /* recognizable word set ID        */
  vdb_rwid_type            rwid;          /* recognizable activation word ID */
  vs_rec_cb_type           cb;            /* pointer to funct when done      */
} vs_vactivate_packet_type;

/* abort voice activation packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_rec_cb_type           cb;            /* pointer to funct when abort done*/

} vs_vactivate_abort_packet_type;

/* menu selection packet
*/
typedef struct
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_sound_type**          play_list;     /* List of prompts for activation  */
  vdb_rw_list_type*        list_ptr;      /* List of RWIDs                   */
  vs_menu_sel_type         include;       /* Inlude List to be recognized    */
  vs_menu_play_prompt_type order;         /* Order of the prompts            */
  vs_rec_cb_type           cb;            /* pointer to funct when done      */
} vs_mselection_packet_type;

/* abort menu selection abort packet
*/
typedef struct 
{
  vs_hdr_type              hdr;           /* Command Header                  */
  vs_rec_cb_type           cb;            /* pointer to funct when abort done*/

} vs_mselection_abort_packet_type;

#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
#ifdef FEATURE_VOICE_MEMO
/* Voice Memo Start Record packet
*/
typedef struct 
{
  vs_hdr_type              hdr;         /* Command Header                */
  vs_vm_record_cb_type     cb;          /* Voice Memo Record Start cb    */
  vs_sound_type          **promptlist;  /* prompt list */
  vs_snd_direction_type    rec_direction; /* record direction           */
} vs_vm_start_rec_packet_type;

typedef struct 
{
  vs_hdr_type               hdr;           /* Command Header                */
  vs_vm_config_type        *config_data_ptr; /* configuration               */
  vs_vm_control_cb_type     callback_ptr;   /* Voice Memo Record Start cb   */
} vs_vm_set_param_packet_type;


typedef struct 
{
  vs_hdr_type               hdr;           /* Command Header                */
  vs_vm_config_type        *config_data_ptr; /* configuration               */
  vs_vm_control_cb_type     callback_ptr;  /* Voice Memo Config cb    */
} vs_vm_get_param_packet_type;

typedef struct 
{
  vs_hdr_type               hdr;         /* Command Header                */
  vdb_vmemo_id_type         vmid;
  uint32                    seconds;
} vs_vm_play_rewff_packet_type;

typedef struct 
{
  vs_hdr_type               hdr;                  /* Command Header    */
  vdb_vmemo_id_type         vmid;
  uint32                    play_rpt_interval_ms; /* configuration */
  vs_snd_direction_type     play_direction; /* play direction      */
  vs_vm_play_cb_type        callback_ptr;  /* Voice Memo Record Start cb */
} vs_vm_play_start_packet_type;

typedef struct 
{
  vs_hdr_type               hdr;         /* Command Header                */
  vdb_vmemo_id_type         vmid;
} vs_vm_vmidonly_packet_type;

typedef struct 
{
  vs_hdr_type               hdr;         /* Command Header                */
  cmx_status_type           status;      /* CMX status                    */
} vs_vm_cmx_cb_packet_type;
#endif /* FEATURE_VOICE_MEMO */

/* <EJECT> */
/* VS command packet union
*/
typedef union 
{
  vs_hdr_type                  hdr;          /* Plain header               */
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
  vs_play_packet_type          play;         /* VS play single utterance   */
  vs_play_multi_packet_type    playmulti;    /* VS play multi utterance    */
  vs_play_abort_packet_type    playabort;    /* VS abort play              */
  vs_play_end_packet_type      playend;      /* VS end playback            */
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */
#if defined(FEATURE_VIDEO_ENCODE_OVERLAY) || \
    defined(FEATURE_VIDEO_ENCODE_FADING) || \
    defined(FEATURE_VIDEO_ENCODE_RAW_ENGINE)
  vs_camera_packet_type        camerapkt;   /* VS capture end             */
#endif
#ifdef FEATURE_PHONE_VR
  vs_capture_end_packet_type   captureend;   /* VS capture end             */
  vs_recog_packet_type         recog;        /* VS recognize utterance     */
  vs_recog_multi_packet_type   recog_multi;  /* VS recognize multi         */
  vs_recog_abort_packet_type   recogabort;   /* VS abort recognition       */
  vs_train_packet_type         train;        /* VS train utterance/words   */
  vs_train_abort_packet_type   trainabort;   /* VS abort training          */
  vs_vactivate_packet_type     vactivate;    /* VS voice activation        */
  vs_vactivate_abort_packet_type vactivateabort;   
                                             /* VS abort voice activation  */
  vs_mselection_packet_type     menusel;     /* VS menu selection          */
  vs_mselection_abort_packet_type menuselabort;   
                                             /* VS abort menu selection    */
#endif /* FEATURE_PHONE_VR */

#ifdef FEATURE_VOICE_MEMO
  vs_vm_start_rec_packet_type    vmrecstart;   /* VM_VM_REC_START          */

  vs_vm_set_param_packet_type   vsvmsetparam;  /*  VS_VM_SET_PARAM, */

  vs_vm_get_param_packet_type   vsvmgetparam;  /*  VS_VM_GET_PARAM, */

  vs_vm_play_rewff_packet_type  vsvmrewff;     /*  VS_VM_PLAY_FASTFORWARD
                                                  VS_VM_PLAY_REWIND, */

  vs_vm_play_start_packet_type  vsvmplaystart;/* VS_VM_PLAY_START, */

  vs_vm_vmidonly_packet_type    vsvmvmidonly;/* VS_VM_REC_STOP,
                                                VS_VM_REC_PAUSE,
                                                VS_VM_REC_RESUME,
                                                VS_VM_REC_CANCEL,
                                                VS_VM_PLAY_STOP, 
                                                VS_VM_PLAY_PAUSE,
                                                VS_VM_PLAY_RESUME,*/
  vs_vm_cmx_cb_packet_type      cmx_cb;      /*VS_VM_REC_CMX_CB */
                                             /*VS_VM_PLAY_CMX_CB */
#endif /* FEATURE_VOICE_MEMO */

} vs_packet_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** VS General Defines
**---------------------------------------------------------------------------
*/
/* VS Command Queue Available Packets.
*/
#define VS_NUM_PACKETS 10               /* # of Free Packets Allocated     */

/* VS Play back Queue
*/
#ifdef FEATURE_VR_SI
#define VS_PB_NUM_BUFFERS 10            /* # of Free Buffers Allocated     */
#else
#define VS_PB_NUM_BUFFERS  2            /* # of Free Buffers Allocated     */
#endif

/* Max number of segments in a single play multi entry.
*/
#define MAX_NUM_MULTI  12               /* Max number of multi entries     */


/* Timer to delay releasing the vocoder.  The reason for this
** function is to keep the vocoder on for a period of time once it
** has been turned on for VR.  Keeping the vocoder on avoids
** repeated delays caused by switching on and off.
*/
#define VS_VOC_RELEASE_TIME  3000       /* 3 second delay                  */

/*---------------------------------------------------------------------------
** VS local typedefs
**---------------------------------------------------------------------------
*/
#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
/* This enum distinguishes a multi play back from a single play back.  This
** enum is used to send the appropriate abort command to SND and also to report
** abort condition in the play call back function.
*/
typedef enum
{
  VS_PLAY_NONE = 0,
  VS_PLAY_SINGLE_P,
  VS_PLAY_MULTI_P,
  VS_PLAY_ABORT_ALL

} vs_play_type;
#endif /* FEATURE_VOICE_DATABASE && FEATURE_VOICE_PLAYBACK */

#ifdef FEATURE_PHONE_VR
/* Maximum amount of NV space allocated to save voice tag utterances.
** This is not same as the number of voice tags that can be saved.
** This number should be reflected in NV configuration (e.g. fs_parms.h)
*/
#define VS_MAX_UTTERANCE_SPACE_ALLOCATED        100000 

#ifdef FEATURE_VR_PUFE
#error code not present
#else
/* Length of vocoder packet for endpoint detection. Should be the same as
** QDSP2_VR_PKT_LEN but in bytes (multiply by 2).
*/
#define VS_EP_BUF_LENGTH 52    

/* Number of vocoder packets to be processed by endpoint detection.  
*/
#define VS_MAX_EP_BUF_ENTRIES 20    
#endif /* FEATURE_VR_PUFE */
/* This structure is used to track if endpoint detection is keeping up with 
** incoming vocoder packets from DSP.
*/
typedef struct 
{
  uint16 index;               /* Buffer index to store/retrieve entries    */
  uint16 number;              /* Number of entries in buffer               */
  uint8  buffer [VS_MAX_EP_BUF_ENTRIES][VS_EP_BUF_LENGTH];

} vs_endpoint_buffer_type;
#endif /* FEATURE_PHONE_VR */

/* <EJECT> */
/*---------------------------------------------------------------------------
** VS local function prototypes
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_PHONE_VR
LOCAL void vs_capture_end_processing (vs_status_type);
LOCAL void vs_train_abort_processing (vs_train_cb_type);
LOCAL void vs_recognize_abort_processing (vs_rec_cb_type);
LOCAL void vs_vactivate_abort_processing (vs_rec_cb_type);
LOCAL void vs_vactivate_processing ( vs_sound_type**, vdb_rw_list_type*, 
                                     vdb_rwid_type, vs_rec_cb_type );
LOCAL void vs_implicit_yes_processing ( vs_sound_type**, vs_rec_cb_type );
void vs_mselection_processing ( vs_sound_type**, vdb_rw_list_type*, 
                                vs_menu_sel_type, vs_menu_play_prompt_type,
                                vs_rec_cb_type );

void vs_menu_selection_abort_processing (vs_rec_cb_type);
#endif /* FEATURE_PHONE_VR */

LOCAL void vs_task_init (void);
LOCAL void vs_task_offline (void);
LOCAL void vs_task_shutdown (void);

void vs_cmd (vs_packet_type *);
LOCAL void vs_cmd_parser (void);
vs_packet_type *vs_cmd_get_pkt (void);

#if (defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK))
LOCAL void vs_play_init_queue (void);
LOCAL void vs_play_free_all_buf (void);
LOCAL void vs_play_end_cb (const void *, snd_status_type);
LOCAL vs_ret_type vs_play_get_utt (vdb_rwid_type, int, voc_pb_header_type *);
LOCAL vs_cmd_error_type vs_play_processing (vdb_rwid_type, short, 
                                     vs_snd_direction_type, vs_play_cb_type);
#endif /* defined(FEATURE_VOICE_DATABASE) && defined(FEATURE_VOICE_PLAYBACK) */

#ifdef FEATURE_PHONE_VR
LOCAL void vs_recognize_utt_processing ( vs_sound_type**, 
           vdb_rw_list_type**, boolean, short, vs_rec_cb_type );
LOCAL void vs_recognize_multi_processing ( vs_sound_type**, 
           vdb_rw_list_type*, vdb_rw_list_type*, vs_rec_multi_type,
           vs_rec_cb_type );
LOCAL void vs_train_utt_processing( vs_sound_type**, vs_utt_echo_type,
           vdb_rwid_type, vdb_rw_list_type**, vs_train_cb_type );
LOCAL void vs_endpoint_processing (void);
LOCAL void vs_capture_start (void);
#ifdef FEATURE_VR_PUFE
#error code not present
#endif /* FEATURE_VR_PUFE */

#endif /* FEATURE_PHONE_VR */

#ifdef VS_TEST_CODE
#error code not present
#endif /* VS_TEST_CODE */

#endif /* FEATURE_VS_TASK */
#endif /* VSI_H */

