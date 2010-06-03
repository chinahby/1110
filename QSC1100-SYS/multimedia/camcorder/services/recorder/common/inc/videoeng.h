#ifndef VIDEOENG_H
#define VIDEOENG_H
/*===========================================================================

                  V I D E O   E N C O D E   E N G I N E
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the video encode engine module.

Copyright(c) 2003-2004 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/videoeng.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/09   sanal   Handle fast pause resume without AVSync mismatch
12/17/08   sanal   Add return types for videoeng APIS to support synchronous usage by clients
11/18/08   girip   Featurized audio recording for FTM camcorder
07/08/08   shiven  Adding support for loci atom
07/04/08   ps      Removed RVCT compiler warnings
04/04/08   sanal   Fix for last frame dropped in post editing
03/17/08   anil    Added support for cprt field in udta atom
02/11/08   sanal   Adding Idiag support for frames dropped in recorded clip
11/17/07   Yogi    Added Post Card Recording Support
11/05/07   dhaval  Added copyright atom support
09/03/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
03/12/07   ali     Added max_bitrate flag
02/09/07   ali     Changes for dynamic RC based on feedback from EFS
11/28/06   Pavan   Added Support for Movieedit Interface
11/21/06   Shahed  AV Sync related changes to fix fragmented file recording.
10/30/06   dhaval  Setting space_limit_threshold and space_imminent_threshold
                   value according to user's settings from UI.
09/04/06   dhaval  Added code to support VENC API for transcoder
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency
                   When qcamcorder Feature Disabled
07/11/06   nag     Added init parameter to reorder of moov mdat atoms according to KDDI specification
06/20/06   Gopi    Added support for dynamically updating titl,descp
                   auth atoms.
05/04/06   Shahed  Changes for new A/V sync implementation for AAC with
                   CMX MM API sync signal.
03/27/06   dw      Allow video_encode_set/get_config through video engine
09/09/05   dw      Changes for video driver to access N-frame video buffers
07/18/05   jk      Merged from videoeng.h#13 in msmshared.
                   Added video_eng_get_config and video_eng_set_config.
06/14/05   jk      Added support for the AAC UI.
03/22/05   jk      Added 'fixed_fps_mode' to 'video_init_type'
02/24/05   drl     Added support for user_bitrate.
02/22/05   wyh     Raw Encoder API changes.
02/16/05   jk      Merged in changes from mainline #11. Major changes are:
                   - Added file size limit feature.
                   - Fixed file limit reached logic.
                   - Now collapsing all 'mdat' atoms together when recording.
                   - Replaced all TASKLOCK/TASKFREE with REX critical sections.
01/31/05   wh      Moved video_eng_transcode_normal_cb to mp4_transcoder.c
01/15/05   wh      Added video_eng_frame() API.
12/02/04   ali     Changes for transcoder
07/14/04   rpw     Added timed text encoding support, and telop to timed
                   text converter.
03/12/04   ali     Changes for offline encoder
08/04/03   rpw     Revamp of code to separate out movie file writer and
                   integrate it into videofmt.
07/31/03   rpw     Added file brand identifier and short header mode flag.
03/28/03   rpw     Removed "reflection_flag" from video_init_type.
03/24/03   rpw     Changed frame rate to time increment and resolution.
03/10/03   rpw     New enhanced feature set added for "offline"
                   movie writing.
                   Added synchronous call capability for "offline" mode.
                   Added API for writing UUID atoms.
11/22/02   rpw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "video_common.h"       /* Common video definitions                */
#include "videofmt_common.h"    /* Common videofmt definitions             */
#include "video_enc.h"
#include "video_buffers.h"
#include "camcorder_util.h"  /* for memory leak detection code */
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#include "video_enc_special_effects.h"
#endif

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* This optional callback is used to report the current movie file size to the
** client, as it is updated.
*/
typedef void (*video_eng_size_cb_func_ptr_type) (
  uint32  size,
  void    *client_data  /* Client data from calling function */
);

/* This optional callback is used to report the very first data callback
** from CMX to client.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
typedef void (*video_eng_cmx_data1_cb_func_ptr_type) (void);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif//FEATURE_APP_TEST_AUTOMATION

/* This enumerated type lists the different formats of input that the video
** engine can accept for timed text.
*/
typedef enum {
    VIDEO_ENG_TEXT_INPUT_NONE      /* use to specify no timed text track */
#ifdef FEATURE_VIDEO_ENG_TELOP_PARSER
    ,VIDEO_ENG_TEXT_INPUT_TELOP    /* KDDI Telop Text (XML-based)        */
#endif /* FEATURE_VIDEO_ENG_TELOP_PARSER */
    ,VIDEO_ENG_TEXT_INPUT_3GPP     /* 3GPP timed text                    */
} video_eng_text_input_type;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
/* This enumerated type lists the different channels of audio source and
** encoded audio for AAC encoding.
*/
typedef enum {
  VIDEO_ENG_AUDIO_CHANNEL_MONO_2_MONO,    /* mono source, mono output     */
  VIDEO_ENG_AUDIO_CHANNEL_STEREO_2_MONO,  /* stereo source, mono output   */
  VIDEO_ENG_AUDIO_CHANNEL_STEREO_2_STEREO /* stereo source, stereo output */
} video_eng_audio_channel_type;

/* This enumerated type lists the different sampling rates that the video
** engine supports for AAC encoding.
*/
typedef enum {
  VIDEO_ENG_AUDIO_SAMPLE_RATE_8000,
  VIDEO_ENG_AUDIO_SAMPLE_RATE_11025,
  VIDEO_ENG_AUDIO_SAMPLE_RATE_16000,
  VIDEO_ENG_AUDIO_SAMPLE_RATE_22050,
  VIDEO_ENG_AUDIO_SAMPLE_RATE_24000
} video_eng_audio_sample_rate_type;

/* This enumerated type lists the different bit rates profile that the
** video engine accepts for AAC encoding.
*/
typedef enum {
  VIDEO_ENG_AUDIO_PROFILE_LOW,
  VIDEO_ENG_AUDIO_PROFILE_MED,
  VIDEO_ENG_AUDIO_PROFILE_HIGH
} video_eng_audio_profile_type;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* This structure is passed by the client of camcorder.
** Holds information of titl,auth,descp,cprt atoms
*/

typedef struct{
    boolean                               atom_is_valid; /* Indicates whether the included data is valid */
	boolean 							  titl_auth_dscp_cprt_valid; /* To verify that other info (title,author etc are valid*/	
    video_fmt_language_coded_string_type  title;
    video_fmt_language_coded_string_type  author;
    video_fmt_language_coded_string_type  description;
    video_fmt_language_coded_string_type  copyright;
    video_fmt_location_type               loci;
}video_eng_user_data_atom_type;

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
/*Callback function prototype that needs to be registered by client*/
typedef void (*video_eng_user_atoms_client_cb)(
  video_eng_user_data_atom_type *puseratominfo,
  void                          *client_data
);
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

#ifdef FEATURE_VIDEO_ENCODE

typedef enum
{
  VENCODER_FRAMETYPE_I,
  VENCODER_FRAMETYPE_P
 }
vencoder_ivop_type;

/* Structure used to fill in Y and CbCr addresses*/
typedef struct vencoder_frame_address_type_struct
{
  uint8 *Y_Address;      /* Luma Address   */
  uint8 *CbCr_Address;   /* Chroma Address */
} vencoder_frame_address_type;

/* Stats from BE */
typedef struct vencoder_frame_stats_type_struct
{
  uint32 psnr;  /*psnr passed back from BE*/
} vencoder_frame_stats_type;

typedef struct vencoder_frame_done_type_struct
{
  uint32            len;        /* Length of data returned      */
  video_frame_type     type;       /* I_FRAME or P_FRAME           */
  int32             delta;      /* Time diff between t and t-1 frame */
  void              *client_data; /* Encoded frame */
  vencoder_frame_address_type *frame_ptr; /* Source pointer of frame encoded */
  vencoder_frame_stats_type        *frame_stats;
} vencoder_frame_done_type;

typedef void (*vencoder_frame_cb_fn)( video_status_type    status,
                                      vencoder_frame_done_type *pFrame,
                                      void * const         client_data);

/* This structure is passed into the video_eng_init function in order to
** configure the engine and the underlying audio/video encoders.
*/
typedef struct {
  video_fmt_type               file_format;      /* i.e. MP4, AVI, ...   */
  video_fmt_brand_type         file_brand;       /* i.e. 3GP, AMC, ...   */
  video_fmt_stream_video_type  video_format;     /* i.e. MPEG-4, ...     */
  video_fmt_stream_audio_type  audio_format;     /* i.e. EVRC, AMR, ...  */
  uint16                       frame_width;      /* frame width          */
  uint16                       frame_height;     /* frame height         */
  uint32                       time_increment;   /* ticks per frame      */
  uint32                       time_resolution;  /* ticks per second     */
  video_profile_type           profile;          /* video profile        */
  int32                        user_bitrate;     /* user specified b.r.  */
  int16                        rotation_flag;    /* rotation value       */
  video_handle_type            destination;      /* where output goes    */
  boolean                      short_header;     /* H.263 compatibility  */
  boolean                      fixed_fps_mode;   /* fixed frames per sec */
  const char                   *text;            /* optional timed text  */
  uint32                       text_size;        /* size of "text"       */
  video_eng_text_input_type    text_type;        /* format of "text"     */
  uint32                       movie_size_limit; /* if >0, max file size */
  video_eng_size_cb_func_ptr_type  size_cb;      /* indicates size file  */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
  video_eng_cmx_data1_cb_func_ptr_type audio_data1_cb; /* First data cb
                                                       indication from CMX*/
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#ifdef FEATURE_CAMCORDER_AAC_REC
  video_eng_audio_channel_type     audio_channel;
  video_eng_audio_sample_rate_type audio_sample_rate;
  video_eng_audio_profile_type     audio_profile;
#endif /* FEATURE_CAMCORDER_AAC_REC */
  VideoBufferHandle*           bufferHandle;
  uint32                       bufferCount;
  boolean                      reorder_atom;     /* atom reorder according to KDDI specification*/
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  video_eng_user_atoms_client_cb useratom_cbptr;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/
 boolean dynamic_bitrate_enable;                /* whether bitrate should   */
                                                /* be controlled by external*/
                                                /* input on a frame-by-frame*/
                                                /* basis                    */

  boolean                      max_bitrate_enable;
  boolean                      no_coding;
  uint8                        space_limit;      /* Space limit threshold value */

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

  vencoder_frame_cb_fn         frame_callback_ptr;
  void*                        client_data;

} video_init_type;

/* Structure for video location information */
typedef struct {
  const char 							*name;
  uint16								name_lang;
  uint8                                 role;
  uint32                                longitude;
  uint32                                latitude;
  uint32                                altitude;
  const char 							*astr_body;
  uint16								astr_body_lang;
  const char 							*add_notes;
  uint16								add_notes_lang;
} video_location_info_type;

/* This structure is passed into the video_eng_get_config and
** video_eng_set_config function in order to configure the engine
** and the underlying audio/video encoders.
*/
typedef struct {
  boolean                      still_image;      /* discard frames > 1   */
  uint16                       audio_rate_set;   /* i.e. AMR bitrate     */
  boolean                      no_interlace;     /* don't mix vid/aud    */
  uint32                       movie_duration_limit; /* if >0, max dur   */
  boolean                      include_drm;      /* Include "drm " atom  */
  uint16                       drm_distribution_rights;
  const char                   *title;           /* for 'titl' atom      */
  uint16                       title_lang;
  const char                   *author;          /* for 'auth' atom      */
  uint16                       author_lang;
  const char                   *description;     /* for 'dscp' atom      */
  uint16                       description_lang;
  const char                   *copyright;       /* for 'cprt' atom      */
  uint16                       copyright_lang;
  video_location_info_type 	   loci;
  video_enc_config_type        enc_config;
  video_enc_raw_config_type    enc_adv_config;
  boolean                      adjust_es_time;   /* adjust elementary stream time*/
} video_config_type;

/* This structure has audio video sync relates information, it is
** used and shared by camera services and video engine/CMX.
**      ! ! !    C  A  U  T  I  O  N    ! ! !
** Care must be taken to set the audio,video config done flags in the
** same task context to prevent any unwated behaviour  */
typedef struct
{
  boolean  videoConfig_done;          /* Indicates Video config is done   */
  boolean  audioConfig_done;          /* Indicates audio config is done   */
  uint32   timelog_video_start;       /* Indicates video start time(msec) */
  uint32   timelog_audio_start;       /* Indicates audio start time(msec) */
} glb_avs_info_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
** VIDEOENG Public Function Prototypes
**---------------------------------------------------------------------------
*/

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_init

DESCRIPTION
  This procedure initializes the engine command queues and prepares to record
  a new movie file.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_init (
  const video_init_type   *init_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_start

DESCRIPTION
  This procedure starts recording audio/video.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_start (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_get_config

DESCRIPTION
  This procedure gets the video engine config data.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_get_config (
  video_config_type       *config_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_set_config

DESCRIPTION
  This procedure sets the video engine config data.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_set_config (
  const video_config_type *config_data,
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_uuid

DESCRIPTION
  This procedure is used to pass along user data atom information to be
  written to the header of the movie file.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.
  Must be called between calls to video_eng_init and video_eng_start.

RETURN VALUE
  None.

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_write_uuid (
  const uint8             *uuid,         // must point to 16-element array
  const void              *data,         // data for atom contents
  uint32                  size,          // size of atom contents
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_pause

DESCRIPTION
  This procedure pauses the current audio/video file being recorded.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_pause (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_resume

DESCRIPTION
  This procedure resumes the recording of the current audio/video file.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_resume (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_stop

DESCRIPTION
  This procedure stops recording audio/video, saving the output file.

DEPENDENCIES
  FEATURE_VS_TASK and FEATURE_VIDEO_ENCODE are defined.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  video_eng_free_q
  video_eng_free_packet

===========================================================================*/
extern video_status_type video_eng_stop (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_exit

DESCRIPTION
  This function cleans up and exits the video engine.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_SUCCESS
  VIDEO_FAILURE

SIDE EFFECTS
  None.

===========================================================================*/
extern video_status_type video_eng_exit (
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/*===========================================================================

FUNCTION video_eng_write_header

DESCRIPTION
  This procedure writes decoder specific info to output file.
  It is used in post editing recording.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_header (
  video_fmt_stream_type     stream_type,
  uint32                    header_size,
  const uint8              *header_data,
  video_cb_func_ptr_type    callback_ptr,
  void                     *client_data
);

/*===========================================================================

FUNCTION video_eng_write_segment_header

DESCRIPTION
  This procedure queues a command which will writes text stream 
  segment header to output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_segment_header (
  video_fmt_stream_type            stream_type,
  uint32                           header_size,
  uint8                            *header_data,
  video_cb_func_ptr_type           callback_ptr,
  void                             *client_data
);

/*===========================================================================

FUNCTION video_eng_write_stream

DESCRIPTION
  This procedure writes video/audio stream to output file.
  It is used in post editing recording.

DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
extern video_status_type video_eng_write_stream (
  video_fmt_stream_type            stream_type,
  uint32                           num_samples,
  video_fmt_sample_info_type      *sample_info,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
);


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_frame

DESCRIPTION
  This procedure encodes a single raw frame.
  The input values and data format are specified in an IPL matrix.

  Allowed function:
     Process Special Effects and return: The input memory buffer will
        be replaced by the IPL output.

     Process Special Effects and send to video_encoder.  In this case
        video_eng_init() and video_eng_start() must first have been called.
        If special effects pointer is NULL the frame will be passed to
        video_encoder without any special effects.

  Special effects will be processed in the VS task, video_encoder will run
  in the GRAPH task. When finished callback_ptr() will be called with the
  appropriate result (VIDEO_SUCCESS, etc..) and the client_data will be
  returned.

  This function can be called from the ISR.


DEPENDENCIES
  FEATURE_VS_TASK is defined.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
extern void video_eng_frame(
  uint8           *Y,       /* input image buffer */
  uint8           *CrCb,    /* chroma component of image */
  ipl_col_for_type type,    /* image type         */
  uint32          msec,    /* render time -- neded by video_encode and     */
                           /* certain special effects engines              */
  video_cb_func_ptr_type  callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_frame_encode_permitted

DESCRIPTION
Checks if current frame is permitted by RC


DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
extern boolean video_eng_check_frame_encode_permitted(VideoBufferHandle *bufferHandle,
                                         uint32 time_ms);

#endif  /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */


#endif /* FEATURE_VIDEO_ENCODE */

#endif /* VIDEOENG_H */

