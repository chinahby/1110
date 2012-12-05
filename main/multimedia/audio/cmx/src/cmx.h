#ifndef CMX_H
#define CMX_H
/*===========================================================================

             C M X   S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMX Services.

REFERENCES
  CMX Services ISOD: 80-24875-12 X2

Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/cmx/main/latest/src/cmx.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/09   bk      Added featurization for fixing compilation errors
                   arised after removing web audio from the build.
08/26/08   anb     Added support for DSP based WMAPRO
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
01/15/08    sk     Fixed Misspelled reverb preset names. CR 135068.
12/19/07    hs     Added support ISDB-T dual mono output control
11/05/07   sk/lg   Added SVG Scaling support.
10/26/07    hs     Added EVW file format support
09/17/07   sk/lg   Added support to allow specifying SVG display
                   when with PMD.CR 118438. 
09/04/07    hs     Added more ID3V2 Txxx frame support for ZTE
08/24/07    hs     Integrated: Added support for PCR clk sync feature.
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support, 
                   support for URL link (W)frames .
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC 
                   frame in MP3 ID3V2.3 tag.
04/20/07    at     Added EVRC-B file format recording support.
03/21/07    at     Added EVRC-B file format playback support.
03/15/07    kan    Added 4GV-WB Code 
02/27/07    kk     Added support for AAC BSAC audio
01/14/06    ay     Fixed MM API merge which removed AMR-WB+ support.
                   Updated the copyright text.
12/19/06    kk     revert back the fix for the 44k sample rate issue.
12/04/06   suk     Fixed 6260 ThinUI build compilation error.
11/14/06    kk     Fixed,Normal playback issue for AAC file, due to wrong
                   selection of the Sampling freq.CR 102009,CR 102579
11/09/06    az     Merged the code for MM API changes.
09/04/06    ay     Added AMR-WB+ support.
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
07/28/06    pl     Added support for doppler, reverb, and env/src 3d enable.
03/24/06    lg     Added support for SVGT 1.2
03/13/06    lg     Added support for SMAF meta data in AUDIO_SPEC callback
03/13/06    st     Added support for delayed start for playback of audio
                   decoder files to MIDI device; added support for repeat
                   playback mode for audio decoder files to MIDI device;
                   added support for query of 3D positional source settings
                   at the end of playback.
01/30/06    pl     Added support to provide a way to select hybrid mode
                   for multisequencer mode and MIDI device.
10/21/05    st     Modified code to include client buffer request under
                   FEATURE_SVG.
09/18/05    st     Added support for QAudioFX: 3D positional commands and query
                   interface.
09/12/05    hz     Added frame byte support for A/V sync callback function.
08/11/05    hz     Added support for DTX and data requrest time for AMR
                   recording.
08/04/05    sp     Added support for mixing of qcp audio and voice call.
07/22/05    sm     Added Wav A/ULaw support.
07/20/05    sp     Added support for AAC LOAS.
07/19/05    pl     Defined cmx_abort_status_enum_type and appended it to
                   cmx_af_server_data_type.
07/08/05    pl     Defined the cmx_af_suprted_ops_mask_type and append the
                   new bit mask type to every audio format spec type.
06/15/05    st     Added SVG structures in cmx_af_server_data_type; added
                   CMX_AF_ANIM_SVG to cmx_af_anim_enum_type.
05/20/05    hz     Added support for MultiMedia AAC record functionality.
05/18/05    ac     Modified code to enhance AV sync functionality to allow
                   clients to specify callback interval in ms or
                   samples/packets.
04/28/05   ymc     Added 4GV NB support.
04/21/05    hz     Added VirtualPacketLength support for Windows Media
                   playback.
04/08/05   act     Reverted change to cmx_status_type for CMX_REC_RESUME and
                   CMX_REC_PAUSE.
04/05/05    lg     Added cmx_svg_event_type, for SVG event data types.
03/25/05   ac/sg   Added cmx_audfmt_play_codec_av_sync() API to support A/V
                   synchronization.
03/23/05    lg     Modified cmx_status_type to map CMX_REC_PAUSE to CMX_PAUSE and
                   CMX_REC_RESUME to CMX_RESUME.
03/17/05    lg     Added support for SVG.
03/10/05    st     Added support for MFi LED control.
02/14/05    hz     Added support for Windows Media codec.
02/05/05    st     Added CMX_AF_FILE_XMF; added cmx_audfmt_load_dls() and
                   cmx_audfmt_unload_dls() function prototypes.
01/11/05    st     Added CMX_AF_FILE_MFI.
10/12/04    st     Added buffer_supplied parameter to cmx_client_buf_req_type.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
04/16/04    st     Added support for animation control; added cue point and
                   jump point status types.
03/22/04    st     Added support for PCM recording and playback.
                   Updated cmx_smaf_graph_info_type and cmx_smaf_note_ctl_type;
                   added cmx_midi_out_qcp_pause_chan() and
                   cmx_midi_out_qcp_resume_chan() function prototypes.
                   Modified code to add client buffer request commands for
                   WebAudio and SAF playback.
01/26/04    lg     Added support for tune, pan & volume adjustments for
                   multi-sequencer.
01/26/04    aw     Added support to get file specs (similar to get_time).
01/21/04  lg/aw    Added support for SMAF led/vibrator/graphics info events.
12/02/03    sg     Added support for linear PCM playback.
11/22/03    sg     Added support for RealAudio codec.
10/16/03  aw/st    Modified code to support audfmt multi-sequences and SMAF.
09/19/03    sg     Modified to support additional AAC parameters required by the
                   DSP. Added support for a 'psuedo-raw' AAC mode. Added
                   support for streaming MP3.
04/30/03    st     Added new language support for WebAudio files.
04/17/03    sm     Added AMR file format support.
03/13/03    st     Removed MP4 decoder support, no longer necessary with new
                   MP4D interface.
02/19/03   aw/st   Added ADPCM file specification type.
01/25/03    st     Added cmx_af_pic_get_buf_type.
12/04/02    sa     Added CMX_GET_BUFFER to cmx_status_type.
11/15/02   ymc     Fixed compiling error by removing extra comma in definition
                   of CMX_AF_MP3_SAMPLE_RATE_UNKNOWN.
11/06/02    sm     Added support for QCP AMR files.
11/05/02    sg     Added support for iMelody format.
09/19/02    st     Removed featurization from enumerated types where it wasn't
                   necessary; added support for pixel poistioning for WebAudio
                   picture commands; added support to return content elements
                   information to client.
08/13/02    yl     Defined cmx_qcp_rec_para_type and add CMX_QCP_REC_AUTO_STOP
                   status in cmx_status_type. ALso changed the prototype of
                   cmx_qcp_record_reverse() and cmx_qcp_record_forward().
08/10/02   aw/st   Modified code to add support for ADPCM audio decoder on
                   MIDI device API.
07/25/02   sg/st   Added support for MP4 and AAC.
                   Added cmx_audfmt_play_codec(). Added support for flushing.
                   Added support for audio postcard API.
06/11/02    st     Added fixed fullrate EVRC and fixed halfrate 13k options
                   in cmx_qcp_format_type; modified code to use new
                   FEATURE_CMX_IMAGE feature.
03/22/02    sa     Added support for JPEG decode and encode.
10/03/01    st     Added cmx_qcp_fs_sapce_available() function prototype.
10/01/01    st     Modified cmx_midi_out_qcp() and cmx_midi_out_qcp_stop()
                   function prototypes in order to support multiple ADEC
                   channels; added pause, resume, volume and panning controls
                   for MIDI device audio decoders.
09/16/01    st     Added cmx_qcp_set_cnfg() function prototype.
09/03/01    st     Added code to support QCP recording and support functions
                   (set/get inforation); modified code to support playback
                   of QCP files to reverse/both links.
04/04/01    st     Updated cmx_audio_playback_control() function prototype.
                   Added support for SAF (simple animation format); added
                   support for direct MIDI device output interface; added
                   support for client controlled data source; removed scaling
                   support from PNG decoder; added cmx_af_callback_enum_type
                   to allow client to indicate what format they want the
                   callbacks to be made; added support for ISO8859 char code
                   set; added cmx_audio_playback() interface for remote
                   systems.
02/28/01    st     Modified CMX_TEMPO_MAX to 500 (or 5x normal playback speed).
12/04/00    sm     Added pan function and pan defaults.
11/27/00  st/sm    Added MP3 file info support.  Featurized EFS.
09/05/00    st     Added cmx_af_server_data_type; added support for picture
                   and text info callbacks.
08/07/00    st     Added support for client to specify number of cue point
                   iterations.
07/25/00    st     Added cmx_audfmt_start_enum_type; modified cmx_audfmt_play()
                   and cmx_audfmt_ringer() function prototypes.
07/22/00    st     Added CMX_AF_CHAR_SET_HANGUL for korean text support.
07/15/00    st     Added support for LED, audio spec, tempo and tune functions;
                   lint updates.
06/13/00    st     Added code to support text postioning.
05/18/00    st     Featurized WebAudio and PNG decoder specific code; added PNG
                   specification and text commands; changed adv_start to
                   seek_pause.
04/28/00    st     Modified cmx_handle_type to be union of all supported
                   types; modified cmx_handle_efs_type to contain the file
                   name; added cmx_audfmt_adv_start() function prototype.
04/13/00    st     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */

#ifdef FEATURE_AUDIO_FORMAT
#include "qw.h"
#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_CMX
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#ifdef FEATURE_EFS
#include "fs.h"         /* File system typedefs and prototypes             */
#endif

#ifdef FEATURE_SVG
#include "svgver.h"
#endif //FEATURE_SVG

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Status type is returned in callback functions to indicate events
** and return data to client.
*/
typedef enum {
  CMX_SUCCESS,              /* Requested command was accepted           */
  CMX_DONE,                 /* Command has been carried out             */
  CMX_ENABLED,              /* Status is enabled                        */
  CMX_DISABLED,             /* Status is disabled                       */
  CMX_TICK_UPDATE,          /* One second update during playback        */
  CMX_REPEAT,               /* Sound repeating                          */
  CMX_DATA_ACCESS_DELAY,    /* Playback is being delayed by data access */
  CMX_ABORTED,              /* Command was aborted                      */
  CMX_FLUSHED,              /* Data flush was executed                  */
  CMX_INVALID_DATA_SOURCE,  /* Invalid data source for selected file    */
  CMX_REWIND,               /* Playback is currently rewinding          */
  CMX_FFORWARD,             /* Playback is currently fast forwarding    */
  CMX_PAUSE,                /* Playback is currently paused             */
  CMX_RESUME,               /* Playback has resumed                     */
  CMX_STARTED,              /* Playback has started                     */
  CMX_TEMPO,                /* Playback tempo changed                   */
  CMX_TUNE,                 /* Playback tune changed                    */
  CMX_PAN,                  /* Playback stereo pan changed              */
  CMX_AUDIO_SPEC,           /* Audio spec information                   */
  CMX_CUE_POINT,            /* Cue point reached                        */
  CMX_JUMP_POINT,           /* Jump point reached                       */
  CMX_TEXT_CTL,             /* Text control information                 */
  CMX_TEXT_INFO,            /* Text information                         */
  CMX_PICTURE_STATUS,       /* Picture status information               */
  CMX_PICTURE_DATA,         /* Picture data                             */
  CMX_PICTURE_INFO,         /* Picture information                      */
  CMX_LED_CONTROL,          /* LED control information                  */
  CMX_BACKLIGHT_CONTROL,    /* Backlight control information            */
  CMX_ANIM_INFO,            /* Animation information                    */
  CMX_ANIM_CLEAR,           /* Animation clear command                  */
  CMX_ANIM_ENABLE,          /* Animation enable command                 */
  CMX_ANIM_DISABLE,         /* Animation disable command                */
  CMX_QCP_REC_AUTO_STOP,    /* Recording stopped with the silence       */
  CMX_QCP_SPACE_WARNING,    /* QCP recording space is running short     */
  CMX_QCP_SPACE_DONE,       /* QCP recording space done                 */
  CMX_CONTINUE,             /* More information/data to follow          */
  CMX_PNG_TEXT,             /* PNG image text data                      */
  CMX_PNG_SPEC,             /* PNG image specifications                 */
  CMX_JPEGD_SPEC,           /* JPEG image specifications                */
  CMX_GET_BUFFER,           /* Get buffer from client (for JPEG cmds)   */
  CMX_CLIENT_BUF_REQ,       /* Get buffer from client (general cmds)    */
  CMX_CLIENT_BUF_FREE,      /* Free buffer that was requested           */
  CMX_SAF_FRAME,            /* SAF frame data                           */
  CMX_SAF_INFO,             /* SAF specifications                       */
  CMX_SMAF_GRAPH_INFO,      /* SMAF Graphics Track control information  */
  CMX_SMAF_LED_CONTROL,     /* SMAF LED control information             */
  CMX_SMAF_VIB_CONTROL,     /* SMAF vibrator control information        */
  CMX_MFI_LED_CONTROL,      /* MFi LED control information              */
  CMX_REC_DROP_FRAMES,      /* Frames being dropped during recording    */
  CMX_REC_DROP_RESUME,      /* Frames starting to be recorded again     */
  CMX_REC_PAUSED,           /* Recording has been paused                */
  CMX_REC_RESUMED,          /* Recording has been resumed               */
  CMX_REC_INTERRUPTED,      /* Recording has been interrupted           */
  CMX_REC_INT_RESUME,       /* Interrupted recording has resumed        */
  CMX_REC_ENABLED,          /* Recording has been enabled               */
  CMX_SVG_INFO,             /* SVG specifications                       */
  CMX_SVG_PREPARE_DONE,     /* SVG Decode done                          */
  CMX_SVG_FRAME,            /* SVG Frame data                           */
  CMX_SVG_EVENT,            /* SVG Event callback                       */
  CMX_SVG_EXT_REQ,          /* External file request                    */
  CMX_SVG_EXT_FREE,         /* Free file request                        */
  CMX_SVG_URL_REQ,          /* SVG external URL request                 */
#ifdef FEATURE_SVGDOM
  CMX_SVG_DOM_DATA,         /* SVG DOM data callback                    */
#endif //FEATURE_SVGDOM
  CMX_PCM_SPACE_WARNING,    /* Memory space warning for recording PCM   */
  CMX_PCM_INSUFF_SPACE,     /* Insufficient memory to start PCM record  */
  CMX_MM_SPACE_WARNING,     /* Memory space warning for recording MM    */
  CMX_MM_INSUFF_SPACE,      /* Insufficient memory to start MM record   */
  CMX_ERROR,                /* Error occured with requested command     */
  CMX_FAILURE,              /* Error occured with requested command     */
  CMX_SBR_MODE,             /* SBR Mode command has been executed       */
  CMX_DUAL_MONO_MODE        /* Dual Mono command has been executed      */
} cmx_status_type;

typedef enum {
  CMX_ABORT_STATUS_REASON_UNKNOWN, /* Abort reason unknown               */
  CMX_ABORT_STATUS_DATA_ERR,       /* Abort data is invalid              */
  CMX_ABORT_STATUS_PREEMPTED,      /* Abort this sound is being prempted */
  CMX_ABORT_STATUS_RESOURCE_ERR,   /* Abort resource error               */
  CMX_ABORT_STATUS_CLIENT_ABORT,   /* Abort client issues the abort cmd  */
  CMX_ABORT_STATUS_DATA_ACCESS_ERR,/* Abort failed to retrieve data      */
  CMX_ABORT_STATUS_MAX
} cmx_abort_status_enum_type;

/* Enumerated type to indicate the source where CMX will find the
** data/file to be played.
*/
typedef enum {
  CMX_SOURCE_MEM,        /* Designates that data is found in the     */
                         /* memory area                              */
  CMX_SOURCE_EFS,        /* Designates that data is found in the     */
                         /* Embedded File System (EFS)               */
  CMX_SOURCE_CLIENT,     /* Designates a client-server data transfer */
  CMX_SOURCE_INVALID     /* Designates an invalid type               */
} cmx_source_type;

#ifdef FEATURE_EFS
/* CMX EFS handle type, used to indicate the file to play is found in EFS. */
typedef struct {
  cmx_source_type   source_type;   /* This should always be CMX_SOURCE_EFS   */
  char              filename[FS_FILENAME_MAX_LENGTH_P];
                                   /* The complete filename, including paths */
                                   /* of the file in EFS.                    */
} cmx_handle_efs_type;
#endif

/* CMX MEM handle type, used to indicate the file to play is found in memory.*/
typedef struct {
  cmx_source_type   source_type;   /* This should always be CMX_SOURCE_MEM   */
  uint8             *buf;          /* Pointer to memory location             */
  uint32            len;
} cmx_handle_mem_type;

/* *****************************************************
** CMX Client handle data types and function definitions
*/

/* Definition of data access callback to return data to CMX services
*/
typedef void (*cmx_client_req_cb_func_ptr_type) (
  cmx_status_type  status,       /* Status of callback                        */
  uint8            *data_ptr,    /* Pointer to client data buffer             */
  uint32           len,          /* Length of buffer returned                 */
  const void       *client_data  /* Client data from calling function         */
);

/* Definition of data access function to be called from CMX services
*/
typedef void (*cmx_client_req_func_ptr_type) (
  uint32                           position,    /* Pos to read (only if type
                                                   CMX_CLIENT_CONFIG_RANDOM)  */
  uint32                           max_len,     /* max len of CMX data buffer */
  cmx_client_req_cb_func_ptr_type  data_req_cb, /* Callback response fn       */
  const void                       *server_client_data,
                                                /* data for data_req_cb       */
  const void                       *client_data /* data identifying this call */
);

/* Client data configuration enum defines the availability of data to CMX
*/
typedef enum {
  CMX_CLIENT_DATA_CONTIGUOUS, /* Data available is guaranteed contiguous     */
  CMX_CLIENT_DATA_REAL_TIME,  /* Data available is in real time and may skip */
  CMX_CLIENT_DATA_RANDOM      /* Data available is random-accessable         */
} cmx_client_data_enum_type;

/* CMX client data handle type, indicates the data is available through
** access functions.
*/
typedef struct {
  cmx_source_type               source_type;  /* Should be CMX_SOURCE_CLIENT */
  cmx_client_data_enum_type     data_config;  /* Client data xfr method      */
  uint32                        data_len;     /* Len of data file, if known  */
  cmx_client_req_func_ptr_type  data_req_ptr; /* Data request access fn      */
} cmx_handle_client_type;

/* Base handle type. CMX routines will use the source_type to determine
** which handle type to use.
*/
typedef union {
  cmx_source_type         source_type;
  cmx_handle_mem_type     mem;
#ifdef FEATURE_EFS
  cmx_handle_efs_type     efs;
#endif
  cmx_handle_client_type  client;
} cmx_handle_type;

/* Standard CMX callback function type */
typedef void (*cmx_cb_func_ptr_type) (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data   /* Client data passed in to the command  */
);

#ifdef FEATURE_SAF
/* This enumerated type is used to indicate whether the SAF decoder should
** return frames according to the SAF file timing or whether it should
** wait to be prompted by the client.
*/
typedef enum {
  CMX_SAF_CTL_FREE_RUN,   /* Return frames as SAF file dictates    */
  CMX_SAF_CTL_CLIENT,     /* Wait for client to prompt for frames  */
  CMX_SAF_CTL_INVALID
} cmx_saf_ctl_type;

/* The following structure contains information regarding a SAF file. This
** is returned for every cmx_saf_decode() and cmx_saf_get_info() call.
** This is also returned when SAF files are found and decoded within
** WebAudio files.
*/
typedef struct {
  uint32  width;               /* Width of frame                           */
  uint32  height;              /* Height of frame                          */
  uint32  bitdepth;            /* Bitdepth of images                       */
  uint32  glob_trans_count;    /* Number of entries in global transparancy */
  uint8   *global_trans;       /* Global transparancy                      */
  uint32  loopcount;           /* Number of loops for this animation       */
  uint32  num_frames;          /* Number of frames in this animation       */
  uint32  num_palette_entry;   /* Number of entries in palette             */
  uint8   *palette;            /* Palette                                  */
} cmx_saf_info_type;
#endif /* FEATURE_SAF */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) || defined(FEATURE_SVG)
/* Definition of buffer request callback function
*/
typedef void (*cmx_client_buf_req_cb_func_ptr_type) (
  uint8  *buffer,        /* Pointer to buffer from client            */
  void   *client_data    /* Client data registered by CMX            */
);

/* The following structure is used to request a data buffer from
** the client
*/
typedef struct {
  cmx_client_buf_req_cb_func_ptr_type cb_func; /* CB func client will use */
                                               /* to return buffer to CMX */
  void    *client_data;     /* Client data                                */
  uint32  buffer_size;      /* Size of buffer                             */
  boolean buffer_supplied;  /* Flag for client to indicate if requested   */
                            /* buffer will be supplied                    */
} cmx_client_buf_req_type;
#endif /* FEATURE_WEBAUDIO || FEATURE_SAF || FEATURE_SVG */

#ifdef FEATURE_AUDIO_FORMAT
/* This enumerated type will be used to indicate whether client callbacks
** that contain data will be returned by value or by reference (in reference to
** the file that is currently playing).
*/
typedef enum {
  CMX_AF_CALLBACK_VALUE,        /* Do callbacks by value     */
  CMX_AF_CALLBACK_REFERENCE,    /* Do callbacks by reference */
  CMX_AF_CALLBACK_NONE
} cmx_af_callback_enum_type;

/* The following enumerated type is used to indicate whether the audio
** file should be started from the cue point or from the begining (normal).
*/
typedef enum {
  CMX_AUDFMT_START_NORMAL,      /* Start from normal position                */
  CMX_AUDFMT_START_CUE_POINT,   /* Start from cue point position if possible */
                                /* and repeat indefinitely.                  */
  CMX_AUDFMT_START_CUE_POINT_1, /* Start from cue point position if possible */
                                /* and play once.                            */
  CMX_AUDFMT_START_CUE_POINT_2, /* Start from cue point position if possible */
                                /* and play twice.                           */
  CMX_AUDFMT_START_CUE_POINT_3, /* Start from cue point position if possible */
                                /* and play 3 times.                         */
  CMX_AUDFMT_START_CUE_POINT_4, /* Start from cue point position if possible */
                                /* and play 4 times.                         */
  CMX_AUDFMT_START_CUE_POINT_5, /* Start from cue point position if possible */
                                /* and play 5 times.                         */

  /* Adding mutually exclusive entries to this enumerated type. The cue point
  ** enums are only applicable to WebAudio files, whereas the path enums
  ** are only applicable to QCP files. This was done in an effort to reuse
  ** parameters in existing APIs instead of adding a new enumerated type
  ** and modifying the APIs.
  */
  CMX_AUDFMT_START_PATH_FORWARD,  /* Play on the forward path            */
  CMX_AUDFMT_START_PATH_REVERSE,  /* Play on the reverse path            */
  CMX_AUDFMT_START_PATH_BOTH,     /* Play on both paths                  */
  CMX_AUDFMT_START_PATH_FORWARD_INCALL, /* Forward path and mixed with voice */
  CMX_AUDFMT_START_PATH_BOTH_INCALL,    /* Both paths and mixed with voice   */
} cmx_audfmt_start_enum_type;

/* The following type is used to indicate if repeat playback is enabled or
** disabled.
*/
typedef enum {
  CMX_AUDFMT_REPEAT_DISABLED,    /* Repeat playback mode is disabled    */
  CMX_AUDFMT_REPEAT_ENABLED,     /* Repeat playback mode is enabled     */
  CMX_AUDFMT_REPEAT_CUEP         /* Repeat playback mode is cue point   */
} cmx_audfmt_repeat_enum_type;

/* The following structure is used to setup all parameters for a
** multisequencer playback.
*/
typedef struct {
  cmx_audfmt_repeat_enum_type  repeat_state; /* Repeat mode                   */
  uint8                        repeat_count; /* Repeat count                  */
  uint16                       repeat_ms;    /* Silence in ms between repeats */
  cmx_af_callback_enum_type    cb_type;      /* Mode for data callbacks       */
} cmx_audfmt_multi_param_type;

/* The following type is used to indicate the command to execute on a
** multi-sequencer playback.
*/
typedef enum {
  CMX_AUDFMT_SEQ_CMD_STOP,      /* Stop sequence                       */
  CMX_AUDFMT_SEQ_CMD_PAUSE,     /* Pause sequence                      */
  CMX_AUDFMT_SEQ_CMD_RESUME,    /* Resume sequence                     */
  CMX_AUDFMT_SEQ_CMD_FFORWARD,  /* Fast-forward sequence               */
  CMX_AUDFMT_SEQ_CMD_REWIND,    /* Rewind sequence                     */
  CMX_AUDFMT_SEQ_CMD_TEMPO,     /* Change tempo of sequence            */
  CMX_AUDFMT_SEQ_CMD_TUNE,      /* Change tuning of sequence           */
  CMX_AUDFMT_SEQ_CMD_PAN,       /* Change pan of sequence              */
  CMX_AUDFMT_SEQ_CMD_VOLUME     /* Change volume of sequence           */
} cmx_audfmt_seq_cmd_type;

/* The following structure is used to issue a command to a sequence */
typedef struct {
  cmx_audfmt_seq_cmd_type cmd;  /* Command to execute                        */
  uint32                  num;  /* Parameter associated with command, if any */
} cmx_audfmt_seq_param_type;

/* Certain CMX commands have default values for startup.
** These commands include TUNE, TEMPO, and PAN.
*/
#define CMX_TEMPO_DEFAULT 100
#define CMX_TEMPO_MAX     500
#define CMX_TEMPO_MIN     25

#define CMX_TUNE_DEFAULT  0x40
#define CMX_TUNE_MAX      0x4C
#define CMX_TUNE_MIN      0x34

#define CMX_PAN_DEFAULT   64
#define CMX_PAN_CENTER    64
#define CMX_PAN_LEFT      0
#define CMX_PAN_RIGHT     128

/* This is an enumerated type that describes the format of the audio file */
typedef enum {
  CMX_AF_FILE_MIDI,                      /* MIDI file               */
  CMX_AF_FILE_CMID,                      /* WebAudio file           */
  CMX_AF_FILE_MFI,                       /* MFi file                */
  CMX_AF_FILE_SMAF,                      /* SMAF file               */
  CMX_AF_FILE_XMF,                       /* XMF file                */
  CMX_AF_FILE_MP3,                       /* MP3 file                */
  CMX_AF_FILE_AAC,                       /* AAC file                */
  CMX_AF_FILE_IMY,                       /* iMelody file            */
  CMX_AF_FILE_RA,                        /* RealAudio Media         */
  CMX_AF_FILE_WMA,                       /* Windows Media file type */
  CMX_AF_FILE_QCP_13K,                   /* 13K QCP file type       */
  CMX_AF_FILE_QCP_EVRC,                  /* EVRC QCP file type      */
  CMX_AF_FILE_QCP_4GV_NB,                /* 4GV QCP file type       */
  CMX_AF_FILE_QCP_4GV_WB,                /* 4GV_WB QCP file type   */
  CMX_AF_FILE_QCP_AMR,                   /* AMR QCP file type       */
  CMX_AF_FILE_QCP_EFR,                   /* EFR QCP file type       */
  CMX_AF_FILE_QCP_FR,                    /* FR QCP file type        */
  CMX_AF_FILE_QCP_HR,                    /* HR QCP file type        */
  CMX_AF_FILE_WAVE,                      /* WAVE file               */
  CMX_AF_FILE_ADPCM = CMX_AF_FILE_WAVE,  /* ADPCM file              */
  CMX_AF_FILE_AMR,                       /* .AMR file type          */
  CMX_AF_FILE_AWB,                       /* .AWB file type          */
  CMX_AF_FILE_AMR_WBPLUS,                /* AMR-WB+ file type       */
  CMX_AF_FILE_EVB,                       /* .EVB file type          */
  CMX_AF_FILE_EVW,                       /* .EVW file type          */
  CMX_AF_FILE_UNKNOWN,                   /* Unknown file type       */
  CMX_AF_FILE_AAC_BROADCAST              /* AAC Broadcast file type */
} cmx_af_file_enum_type;

/* This type is used by the client to indicate the audio codec to use
** to play the audio data. This type contains information on whether
** to use the MP4 decoder audio codec or the default codec.
*/
typedef struct {
  cmx_af_file_enum_type  file_type;         /* Format of audio data        */
} cmx_af_codec_type;

/* The following is used to indicate the type of AV Sync interval */
typedef enum {
  CMX_AV_SYNC_SAMPLES,         /* av sync interval in samples or frames    */
  CMX_AV_SYNC_TIME_MS,         /* av sync interval in milliseconds         */
  CMX_AV_SYNC_INVALID          /* Designates an invalid type               */
} cmx_av_sync_interval_type;

/* Audio Video Sync callback function */
typedef void (*cmx_av_sync_cb_func_ptr_type) (
  qword timestamp,         /* timestamp for the callback in ms               */
  qword num_of_samples,    /* sample/frame counter. Use only lower 48 bits   */
  qword num_of_bytes,      /* bytes/frame counter. Use only for window media */
  const void *client_data  /* av sync client data                            */
);

/* Define extended AV Sync structure to pass sample_rate
   information in the AV Sync feedback */
typedef struct {
  qword timestamp;           /* timestamp for the callback in ms               */    
  qword num_of_samples;      /* sample/frame counter. Use only lower 48 bits   */
  qword num_of_bytes;        /* bytes/frame counter. Use only for window media */
  uint32 sample_rate;        /*  sample rate */
} cmx_av_sync_info_type;

/* Audio Video Sync extendable callback function */
typedef void (*cmx_av_sync_ex_cb_func_ptr_type) (
  cmx_av_sync_info_type   avsync_info,
  const void              *client_data      /* av sync client data */
);

typedef struct {
  uint32                          av_sync_interval; /* freq ms or samples      */
  cmx_av_sync_interval_type       av_sync_mode;     /* ms or samples           */
  cmx_av_sync_ex_cb_func_ptr_type av_sync_cb_func;  /* cb to receive info      */
  const void                      *client_data;     /* client correlation data */ 
} cmx_av_sync_ex_type;

/* Descriptor of required information for AV sync callbacks */
typedef struct {
  uint32                       av_sync_interval; /* freq ms or samples      */
  cmx_av_sync_interval_type    av_sync_mode;     /* ms or samples           */
  cmx_av_sync_cb_func_ptr_type av_sync_cb_func;  /* cb to receive info      */
  const void                   *client_data;     /* client correlation data */
} cmx_av_sync_type;

/* This enum type defines the masks for different supported operations */
typedef enum{
  CMX_AF_SUPRTED_OPS_MASK_NONE    = 0x00, /* No supported operations */
  CMX_AF_SUPRTED_OPS_MASK_SEEK    = 0x01,
  CMX_AF_SUPRTED_OPS_MASK_GETTIME = 0x02
} cmx_af_suprted_ops_mask_type;

#ifdef FEATURE_MIDI
/* The following structure contains MIDI file specifications.  It is used
** with the CMX_AUDIO_SPEC playback callback. The sequence parameter contains
** the sequence handle for files started using multi-sequencer mode.
*/
typedef struct {
  cmx_af_file_enum_type        file_type;     /* Format of audio file        */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations        */
  void                         *sequence;     /* Sequence handle             */
} cmx_af_midi_spec_type;
#endif /* FEATURE_MIDI */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
/* The following type is used to indicate the LED colour to enable */
typedef enum {
  CMX_AF_LED_CTL_OFF,
  CMX_AF_LED_CTL_DARK_BLACK,
  CMX_AF_LED_CTL_DARK_BLUE,
  CMX_AF_LED_CTL_DARK_GREEN,
  CMX_AF_LED_CTL_DARK_CYAN,
  CMX_AF_LED_CTL_DARK_RED,
  CMX_AF_LED_CTL_DARK_PURPLE,
  CMX_AF_LED_CTL_DARK_YELLOW,
  CMX_AF_LED_CTL_DARK_WHITE,
  CMX_AF_LED_CTL_BRIGHT_BLACK,
  CMX_AF_LED_CTL_BRIGHT_BLUE,
  CMX_AF_LED_CTL_BRIGHT_GREEN,
  CMX_AF_LED_CTL_BRIGHT_CYAN,
  CMX_AF_LED_CTL_BRIGHT_RED,
  CMX_AF_LED_CTL_BRIGHT_PURPLE,
  CMX_AF_LED_CTL_BRIGHT_YELLOW,
  CMX_AF_LED_CTL_BRIGHT_WHITE
} cmx_af_led_ctl_enum_type;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#if !defined(FEATURE_WEBAUDIO) && defined(FEATURE_SVG)

/* The following type is used for text/picture positioning */
typedef enum {
  CMX_AF_POS_LT,          /* Left and/or top justify     */
  CMX_AF_POS_RB,          /* Right and/or bottom justify */
  CMX_AF_POS_CENTER,      /* Center justify              */
  CMX_AF_POS_PERCENT,     /* Use percentage for offset   */
  CMX_AF_POS_PIXEL        /* Use #pixel for offset       */
} cmx_af_pos_enum_type;

/* The following type is used for ANIM positioning */
typedef struct {
  cmx_af_pos_enum_type xpos_type; /* Horizontal positioning                */
  cmx_af_pos_enum_type ypos_type; /* Vertical positioning                  */
  uint8                xpos_num;  /* Horizontal positioning in % or pixels */
  uint8                ypos_num;  /* Veritcal positioning in % or pixels   */
} cmx_af_pos_struct_type;

#endif /*!defined(FEATURE_WEBAUDIO) && defined(FEATURE_SVG)*/


#ifdef FEATURE_WEBAUDIO
/* The following enumerated type is used to indicate the character set to be
** used for the text commands.
*/
typedef enum {
  CMX_AF_CHAR_SET_ANSI,         /* ASCII character set                */
  CMX_AF_CHAR_SET_ISO8859_1,    /* ISO8859-1 character set            */
  CMX_AF_CHAR_SET_ISO8859_2,    /* ISO8859-2 character set            */
  CMX_AF_CHAR_SET_ISO8859_3,    /* ISO8859-3 character set            */
  CMX_AF_CHAR_SET_ISO8859_4,    /* ISO8859-4 character set            */
  CMX_AF_CHAR_SET_ISO8859_5,    /* ISO8859-5 character set            */
  CMX_AF_CHAR_SET_ISO8859_6,    /* ISO8859-6 character set            */
  CMX_AF_CHAR_SET_ISO8859_7,    /* ISO8859-7 character set            */
  CMX_AF_CHAR_SET_ISO8859_8,    /* ISO8859-8 character set            */
  CMX_AF_CHAR_SET_ISO8859_9,    /* ISO8859-9 character set            */
  CMX_AF_CHAR_SET_ISO8859_10,   /* ISO8859-10 character set           */
  CMX_AF_CHAR_SET_SHIFTJIS,     /* ShiftJIS Japanese character set    */
  CMX_AF_CHAR_SET_HANGUL,       /* HANGUL Korean character set        */
  CMX_AF_CHAR_SET_CHINESE_SIMP, /* Chinese simplified character set   */
  CMX_AF_CHAR_SET_CHINESE_TRAD, /* Chinese traditional character set  */
  CMX_AF_CHAR_SET_HINDI,        /* Hindi character set                */
  CMX_AF_CHAR_SET_UNKNOWN       /* Unknown character set              */
} cmx_af_char_set_enum_type;

/* The following type is used for text/picture positioning */
typedef enum {
  CMX_AF_POS_LT,          /* Left and/or top justify     */
  CMX_AF_POS_RB,          /* Right and/or bottom justify */
  CMX_AF_POS_CENTER,      /* Center justify              */
  CMX_AF_POS_PERCENT,     /* Use percentage for offset   */
  CMX_AF_POS_PIXEL        /* Use #pixel for offset       */
} cmx_af_pos_enum_type;

/* The following type is used for ANIM positioning */
typedef struct {
  cmx_af_pos_enum_type xpos_type; /* Horizontal positioning                */
  cmx_af_pos_enum_type ypos_type; /* Vertical positioning                  */
  uint8                xpos_num;  /* Horizontal positioning in % or pixels */
  uint8                ypos_num;  /* Veritcal positioning in % or pixels   */
} cmx_af_pos_struct_type;

/* The folowing types are to be used with the CMX_TEXT_CTL playback status
** callbacks. These will be used in the server_data parameter in the callback.
*/
typedef enum {
  CMX_AF_TEXT_CTL_TITLE,    /* Indicates title information               */
  CMX_AF_TEXT_CTL_NEW,      /* Indicates new text string data            */
  CMX_AF_TEXT_CTL_APPEND,   /* Indicates text string to append data      */
  CMX_AF_TEXT_CTL_ADVANCE,  /* Tells client to advance text cursor       */
  CMX_AF_TEXT_CTL_REWIND,   /* Tells client to rewind text cursor        */
  CMX_AF_TEXT_CTL_CLEAR,    /* The current text string should be cleared */
  CMX_AF_TEXT_CTL_ENABLE,   /* Enables text display                      */
  CMX_AF_TEXT_CTL_DISABLE   /* Disables text display                     */
} cmx_af_text_ctl_enum_type;

typedef enum {
  CMX_AF_TEXT_VOCAL_DEFAULT,     /* Text command is for default vocals    */
  CMX_AF_TEXT_VOCAL_FEMALE,      /* Text command is for female vocals     */
  CMX_AF_TEXT_VOCAL_MALE,        /* Text command is for male vocals       */
  CMX_AF_TEXT_VOCAL_BOTH,        /* Text command is for duet vocals       */
  CMX_AF_TEXT_VOCAL_BACKGROUND   /* Text command is for background vocals */
} cmx_af_text_vocal_enum_type;

/* This is the actual structure that will be returned in the server_data
** parameter for the CMX_TEXT_CTL callbacks.
*/
typedef struct {
  cmx_af_text_ctl_enum_type    text_ctl;  /* Text command                    */
  cmx_af_text_vocal_enum_type  vocals;    /* Vocals to apply command         */
  cmx_af_pos_enum_type         xpos_type; /* Horizontal positioning          */
  cmx_af_pos_enum_type         ypos_type; /* Vertical positioning            */
  uint32                       num;       /* Number usage depends on command */
  uint8                        *buffer;   /* Buffer containing text string   */
} cmx_af_text_ctl_type;

/* This is the actual structure that will be returned in the server_data
** parameter for the CMX_TEXT_INFO callbacks.
*/
typedef struct {
  cmx_af_text_ctl_enum_type    text_ctl;  /* Text command                    */
  cmx_af_text_vocal_enum_type  vocals;    /* Vocals to apply command         */
  cmx_af_pos_enum_type         xpos_type; /* Horizontal positioning          */
  cmx_af_pos_enum_type         ypos_type; /* Vertical positioning            */
  uint32                       size;      /* Size of string in bytes         */
  uint32                       offset;    /* Position in file where text     */
                                          /* string can be found             */
} cmx_af_text_info_type;

/* The folowing types are to be used with the CMX_PICTURE_STATUS playback
** status callbacks. These will be used in the server_data parameter in the
** callback.
*/
typedef enum {
  CMX_AF_PIC_STATUS_INIT,     /* Init parameters for new picture             */
  CMX_AF_PIC_STATUS_CLEAR,    /* Clear picture display                       */
  CMX_AF_PIC_STATUS_ENABLE,   /* Enable picture display                      */
  CMX_AF_PIC_STATUS_DISABLE,  /* Disable Picture display                     */
  CMX_AF_PIC_STATUS_ERROR     /* Error occured while processing picture data */
} cmx_af_pic_status_enum_type;

typedef enum {
  CMX_AF_PIC_SPEED_NORMAL,    /* Draw/Clear picture at normal speed */
  CMX_AF_PIC_SPEED_SLOW,      /* Draw/Clear picture at slow speed   */
  CMX_AF_PIC_SPEED_MIDDLE,    /* Draw/Clear picture at middle speed */
  CMX_AF_PIC_SPEED_FAST       /* Draw/Clear picture at fast speed   */
} cmx_af_pic_speed_enum_type;

/* This is the actual structure that will be returned in the server_data
** parameter for the CMX_PICTURE_STATUS callbacks.
*/
typedef struct {
  cmx_af_pic_status_enum_type  pic_status;    /* Picture command           */
  cmx_af_pic_speed_enum_type   draw_speed;    /* Picture speed indicator   */
  cmx_af_pos_enum_type         xpos_type;     /* Horizontal positioning    */
  cmx_af_pos_enum_type         ypos_type;     /* Vertical positioning      */
  uint8                        xpos_percent;  /* Horizontal positioning in */
                                              /* percent or pixels         */
  uint8                        ypos_percent;  /* Veritcal positioning in   */
                                              /* percent or pixels         */
} cmx_af_picture_status_type;

/* The folowing types are to be used with the CMX_PICTURE_DATA playback status
** callbacks. These will be used in the server_data parameter in the callback.
*/
typedef enum {
  CMX_AF_PIC_DATA_TEXT,   /* Embedded text data                              */
  CMX_AF_PIC_DATA_APPEND, /* Append picture data to current data             */
  CMX_AF_PIC_DATA_DONE    /* Final block of picture data for current picture */
} cmx_af_pic_data_enum_type;

typedef enum {
  CMX_AF_PIC_PNG,        /* Data is in decoded PNG format (PNM format) */
  CMX_AF_PIC_BMP,        /* Data is in BMP format                      */
  CMX_AF_PIC_JPEG,       /* Data is in JPEG format                     */
  CMX_AF_PIC_UNKNOWN     /* Data is of unknown format                  */
} cmx_af_pic_enum_type;

/* This is the actual structure that will be returned in the server_data
** parameter for the CMX_PICTURE_DATA callbacks.
*/
typedef struct {
  cmx_af_pic_data_enum_type  pic_data;  /* Append or final picture data CB */
  cmx_af_pic_enum_type       pic_type;  /* Format of the picture data      */
  uint32                     num;       /* Number of bytes of picture data */
  uint8                      *buffer;   /* Buffer containing picture data  */
} cmx_af_pic_data_type;

/* This is the actual structure that will be returned in the server_data
** parameter for the CMX_PICTURE_INFO callbacks.
*/
typedef struct {
  cmx_af_pic_enum_type       pic_type;  /* Format of the picture data       */
  uint32                     size;      /* Size of the image in bytes       */
  uint32                     offset;    /* Position in file where the image */
                                        /* data can be found                */
} cmx_af_pic_info_type;

/* This enum is used to indicate the animation format found in a WebAudio
** file. Currently, only SAF is supported.
*/
typedef enum {
  CMX_AF_ANIM_SAF,        /* Data is in SAF format        */
  CMX_AF_ANIM_SVG,        /* Data is in SVG format        */
  CMX_AF_ANIM_UNKNOWN     /* Data is of unknown format    */
} cmx_af_anim_enum_type;

/* This enum contains the possible SAf commands encountered in a WebAudio
** file. Currently, the CMX_AF_ANIM_CMD_FRAME is not supported.
*/
typedef enum {
  CMX_AF_ANIM_CMD_DATA,         /* SAF data command             */
  CMX_AF_ANIM_CMD_FRAME,        /* SAF frame command            */
  CMX_AF_ANIM_CMD_FRAME_ID,     /* SAF frame ID command         */
  CMX_AF_ANIM_CMD_INVALID       /* Invalid command              */
} cmx_af_anim_cmd_type;

/* This is used as the server data type when returning animation commands
** by reference.
*/
typedef struct {
  cmx_af_anim_enum_type      anim_type; /* Format of the anim data          */
  cmx_af_anim_cmd_type       anim_cmd;  /* Command of the anim data         */
  uint32                     size;      /* Size of the image in bytes       */
  uint32                     offset;    /* Position in file where the image */
                                        /* data can be found                */
  cmx_af_pos_struct_type     frame_pos; /* Positioning of animation frame   */
} cmx_af_anim_info_type;

#ifdef FEATURE_SAF
/* The following structure is used to return a SAF frame from a WebAudio
** file.
*/
typedef struct {
  uint32                 num;       /* Number of bytes of SAF frame data */
  uint8                  *buffer;   /* Buffer containing SAF frame data  */
  cmx_af_pos_struct_type frame_pos; /* Positioning of animation frame    */
} cmx_saf_data_type;
#endif /* FEATURE_SAF */

/* Bitmask of elements contained in c-MIDI file */
typedef enum {
  CMX_AF_WA_CONTENTS_SONG    = 0x00000001, /* Content contains MIDI commands  */
  CMX_AF_WA_CONTENTS_WAVE    = 0x00000002, /* Content contains WAV data       */
  CMX_AF_WA_CONTENTS_TEXT    = 0x00000004, /* Content contains text events    */
  CMX_AF_WA_CONTENTS_PIC     = 0x00000008, /* Content contains picture events */
  CMX_AF_WA_CONTENTS_ANIM    = 0x00000010, /* Content contains animation      */
  CMX_AF_WA_CONTENTS_LED     = 0x00000020, /* Content contains LED events     */
  CMX_AF_WA_CONTENTS_VIB     = 0x00000040, /* Content contains VIB events     */
  CMX_AF_WA_CONTENTS_UNKNOWN = 0x80000000  /* Content contains unrecognized   */
                                           /* elements                        */
} cmx_af_wa_contents_mask_type;

/* The following structure contains webaudio file specifications.  It is used
** with the CMX_AUDIO_SPEC playback callback. The sequence parameter contains
** the sequence handle for files started using multi-sequencer mode.
*/
typedef struct {
  cmx_af_file_enum_type      file_type; /* Format of audio file               */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations         */
  cmx_af_char_set_enum_type  char_set;  /* Character code set to use for text */
  uint32                     contents;  /* Contents of WebAudio file          */
  void                       *sequence; /* Sequence handle                    */
} cmx_af_wa_spec_type;

#ifdef FEATURE_SMAF
/* The following enum contains the character codes in SMAF format */
typedef enum{
  CMX_AF_SMAF_CODE_SHIFT_JIS = 0x00,  /* Japanese */
  CMX_AF_SMAF_CODE_LATIN_1   = 0x1,     /* English, Frenach, German, Italian,
                                         Spanish, Portugese */
  CMX_AF_SMAF_CODE_EUC_KR    = 0x02,     /* Korean */
  CMX_AF_SMAF_CODE_HZ_GB_2312= 0x03,  /* Chinese simplified */
  CMX_AF_SMAF_CODE_BIG5      = 0x04,  /* Chinese traditional */
  CMX_AF_SMAF_CODE_KOI8_R    = 0x05,  /* Russian, etc */
  CMX_AF_SMAF_CODE_TCVN_5778 = 0x06,  /* Vietnamese */
  CMX_AF_SMAF_CODE_USC_2     = 0x20,  /* Unicode */
  CMX_AF_SMAF_CODE_USC_4     = 0x21,  /* Unicode */
  CMX_AF_SMAF_CODE_UTF_7     = 0x22,  /* Unicode */
  CMX_AF_SMAF_CODE_UTF_8     = 0x23,  /* Unicode */
  CMX_AF_SMAF_CODE_UTF_16    = 0x24,  /* Unicode */
  CMX_AF_SMAF_CODE_UTF_32    = 0x25,  /* Unicode */
  CMX_AF_SMAF_CODE_STREAM    = 0xFF   /* Binary value */
}cmx_af_smaf_code_enum_type;

/* The following structure contains SMAF file specifications.  It is used
** with the CMX_AUDIO_SPEC playback callback. The sequence parameter contains
** the sequence handle for files started using multi-sequencer mode.
*/
typedef struct {
  cmx_af_file_enum_type      file_type; /* Format of audio file               */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations         */
  void                       *sequence; /* Sequence handle                    */
  cmx_af_smaf_code_enum_type code_type; /* Character code type             */
  char                       title[30];   /* Song Title                      */
  char                       artist[30];  /* Artist Name                     */
  char                       copyright[30];/* Copyright                      */
} cmx_af_smaf_spec_type;
/* Server data for SMAF's graphics track chunk information */
typedef struct {
  uint8   num;                 /* Graphics Track number                       */
  uint32  pos;                 /* Location of the Graphics Track Chunk's body */
  uint32  size;                /* Size of the Graphics Track Chunk's body     */
} cmx_smaf_graph_info_type;

/* Server data for SMAF's LED or Vibrator note on/off information */
typedef enum {
	CMX_SMAF_LED_CTL_STATUS_OFF = 0,
	CMX_SMAF_LED_CTL_STATUS_ON,
} cmx_smaf_led_ctl_status_enum_type;

typedef enum {
	CMX_SMAF_VIB_CTL_STATUS_OFF = 0,
	CMX_SMAF_VIB_CTL_STATUS_ON,
} cmx_smaf_vib_ctl_status_enum_type;
#endif /* FEATURE_SMAF */

/* The following mask is used to indicate which LED types are enabled
** for MFi playback.
*/
typedef enum {
  CMX_MFI_LED_MASK_TYPE1 = 0x01,  /* MFi LED type 1 mask */
  CMX_MFI_LED_MASK_TYPE2 = 0x02,  /* MFi LED type 2 mask */
  CMX_MFI_LED_MASK_TYPE3 = 0x04,  /* MFi LED type 3 mask */
} cmx_mfi_led_mask_type;

/* This structure is used as server data type for CMX_MFI_LED_CONTROL */
typedef struct {
  uint32  led_ctl_mask;   /* Mask of enabled LED types */
} cmx_mfi_led_ctl_type;
#endif /* FEATURE_WEBAUDIO */

/* Common channel info */
typedef enum {
  CMX_AF_CHANNEL_UNKNOWN = 0,
  CMX_AF_CHANNEL_MONO,        /* Single channel (mono) data                */
  CMX_AF_CHANNEL_DUAL,        /* Stereo data                               */
  CMX_AF_CHANNEL_TRIPLE,      /* 3 channels: 1+2 (UNSUPPORTED)             */
  CMX_AF_CHANNEL_QUAD,        /* 4 channels: 1+2+1 (UNSUPPORTED)           */
  CMX_AF_CHANNEL_QUINTUPLE,   /* 5 channels: 1+2+2 (UNSUPPORTED)           */
  CMX_AF_CHANNEL_SEXTUPLE,    /* 5+1 channels: 1+2+2+1 (UNSUPPORTED)       */
  CMX_AF_CHANNEL_OCTUPLE      /* 7+1 channels: 1+2+2+2+1 (UNSUPPORTED)     */
} cmx_af_channel_enum_type;

#if defined(FEATURE_REAL_AUDIO)              || \
    defined(FEATURE_MPEG_AUDIO)              || \
    defined(FEATURE_WMA)                     || \
    defined(FEATURE_AMR_WB_AUDIO_DEC_COMMON)
/* MPEG sample rates (the MP3 values are included in this enum in order
** to keep things backwards compatible.
*/
typedef enum {
  CMX_AF_SAMPLE_RATE_8000     = 1,                                /* 8k      */
  CMX_AF_MP3_SAMPLE_RATE_8000 = CMX_AF_SAMPLE_RATE_8000,
#if defined(FEATURE_STEREO_DAC)
  CMX_AF_SAMPLE_RATE_11025,                                       /* 11.025k */
  CMX_AF_MP3_SAMPLE_RATE_11025 = CMX_AF_SAMPLE_RATE_11025,
  CMX_AF_SAMPLE_RATE_12000,                                       /* 12k     */
  CMX_AF_MP3_SAMPLE_RATE_12000 = CMX_AF_SAMPLE_RATE_12000,

  CMX_AF_SAMPLE_RATE_16000,                                       /* 16k     */
  CMX_AF_MP3_SAMPLE_RATE_16000 = CMX_AF_SAMPLE_RATE_16000,
  CMX_AF_SAMPLE_RATE_22050,                                       /* 22.050k */
  CMX_AF_MP3_SAMPLE_RATE_22050 = CMX_AF_SAMPLE_RATE_22050,
  CMX_AF_SAMPLE_RATE_24000,                                       /* 24k     */
  CMX_AF_MP3_SAMPLE_RATE_24000 = CMX_AF_SAMPLE_RATE_24000,

  CMX_AF_SAMPLE_RATE_32000,                                       /* 32k     */
  CMX_AF_MP3_SAMPLE_RATE_32000 = CMX_AF_SAMPLE_RATE_32000,
  CMX_AF_SAMPLE_RATE_44100,                                       /* 44.1k   */
  CMX_AF_MP3_SAMPLE_RATE_44100 = CMX_AF_SAMPLE_RATE_44100,
  CMX_AF_SAMPLE_RATE_48000,                                       /* 48k     */
  CMX_AF_MP3_SAMPLE_RATE_48000 = CMX_AF_SAMPLE_RATE_48000,

#ifdef FEATURE_AAC
  CMX_AF_SAMPLE_RATE_64000,                                       /* 64k     */
  CMX_AF_SAMPLE_RATE_88200,                                       /* 88.2k   */
  CMX_AF_SAMPLE_RATE_96000,                                       /* 96k     */
#endif /* FEATURE_AAC        */

#endif /* FEATURE_STEREO_DAC */

  CMX_AF_SAMPLE_RATE_UNKNOWN,                                /* Unknown rate */
  CMX_AF_MP3_SAMPLE_RATE_UNKNOWN = CMX_AF_SAMPLE_RATE_UNKNOWN
} cmx_af_sample_rate_enum_type;
#endif /* FEATURE_REAL_AUDIO || FEATURE_MPEG_AUDIO              ||
          FEATURE_WMA        || FEATURE_AMR_WB_AUDIO_DEC_COMMON */

#ifdef FEATURE_MPEG_AUDIO
/* MPEG layer info
*/
typedef enum {
  CMX_AF_MP3_LAYER_RESERVED,  /* Reserved                                    */
  CMX_AF_MPEG_AAC = CMX_AF_MP3_LAYER_RESERVED,  /* MPEG AAC compression      */
  CMX_AF_MP3_LAYER_3,         /* MPEG Layer 3 compression                    */
  CMX_AF_MP3_LAYER_2,         /* MPEG Layer 2 compression                    */
  CMX_AF_MP3_LAYER_1,         /* MPEG Layer 1 compression                    */
  CMX_AF_MP3_LAYER_UNKNOWN    /* Unable to determine layer information       */
} cmx_af_mpeg_layer_enum_type;
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_MP3
/* This typedef exists to keep things backwards compatible.
*/
typedef cmx_af_sample_rate_enum_type cmx_af_mp3_sample_rate_enum_type;

/* MP3 Version info
*/
typedef enum {
  CMX_AF_MP3_VER_25,        /* MPEG Version 2.5                              */
  CMX_AF_MP3_VER_RESERVED,  /* Reserved                                      */
  CMX_AF_MP3_VER_2,     /* MPEG Version 2.0                              */
  CMX_AF_MP3_VER_1,         /* MPEG Version 1.0                              */
  CMX_AF_MP3_VER_UNKNOWN    /* Unable to determine version information       */
} cmx_af_mp3_ver_enum_type;

/* MP3 bitrate info
*/
typedef enum {
  CMX_AF_MP3_BITRATE_FREE = 0,   /* Free bitrate (determined by software)    */
  CMX_AF_MP3_BITRATE_8K   = 8,   /* Fixed bitrates                           */
  CMX_AF_MP3_BITRATE_16K  = 16,  /*                                          */
  CMX_AF_MP3_BITRATE_24K  = 24,  /*                                          */
  CMX_AF_MP3_BITRATE_32K  = 32,  /*                                          */
  CMX_AF_MP3_BITRATE_40K  = 40,  /*                                          */
  CMX_AF_MP3_BITRATE_48K  = 48,  /*                                          */
  CMX_AF_MP3_BITRATE_56K  = 56,  /*                                          */
  CMX_AF_MP3_BITRATE_64K  = 64,  /*                                          */
  CMX_AF_MP3_BITRATE_80K  = 80,  /*                                          */
  CMX_AF_MP3_BITRATE_96K  = 96,  /*                                          */
  CMX_AF_MP3_BITRATE_112K = 112, /*                                          */
  CMX_AF_MP3_BITRATE_128K = 128, /*                                          */
  CMX_AF_MP3_BITRATE_144K = 144, /*                                          */
  CMX_AF_MP3_BITRATE_160K = 160, /*                                          */
  CMX_AF_MP3_BITRATE_176K = 176, /*                                          */
  CMX_AF_MP3_BITRATE_192K = 192, /*                                          */
  CMX_AF_MP3_BITRATE_224K = 224, /*                                          */
  CMX_AF_MP3_BITRATE_256K = 256, /*                                          */
  CMX_AF_MP3_BITRATE_288K = 288, /*                                          */
  CMX_AF_MP3_BITRATE_320K = 320, /*                                          */
  CMX_AF_MP3_BITRATE_352K = 352, /*                                          */
  CMX_AF_MP3_BITRATE_384K = 384, /*                                          */
  CMX_AF_MP3_BITRATE_416K = 416, /*                                          */
  CMX_AF_MP3_BITRATE_448K = 448, /*                                          */
  CMX_AF_MP3_BITRATE_VAR = 500,  /* Variable bitrate (Changes each frame)    */
  CMX_AF_MP3_BITRATE_UNK = 501   /* Unable to determine bitrate information  */
} cmx_af_mp3_bitrate_enum_type;

/* MP3 Channel info
*/
typedef enum {
  CMX_AF_MP3_CHANNEL_STEREO,       /* Stereo data                            */
  CMX_AF_MP3_CHANNEL_JOINT_STEREO, /* Joint stereo data                      */
  CMX_AF_MP3_CHANNEL_DUAL,         /* Dual channel (stereo) data             */
  CMX_AF_MP3_CHANNEL_SINGLE        /* Single channel (mono) data             */
} cmx_af_mp3_channel_enum_type;

/* MP3 Extension info
** The extension bits are valid when CMX_AF_MP3_CHANNEL_JOINT_STEREO is set.
*/
typedef enum {
  /* For Layer 1 & 2 files */
  CMX_AF_MP3_EXT_BAND_4_31 = 0,            /* Channel extension info, 4-31   */
  CMX_AF_MP3_EXT_BAND_8_31 = 1,            /*                         8-31   */
  CMX_AF_MP3_EXT_BAND_12_31 = 2,           /*                        12-31   */
  CMX_AF_MP3_EXT_BAND_16_31 = 3,           /*                        16-31   */
  /* For Layer 3 files */
  CMX_AF_MP3_EXT_INTENSITY_OFF_MS_OFF = 0, /* Intensity stereo off, MS off   */
  CMX_AF_MP3_EXT_INTENSITY_ON_MS_OFF = 1,  /*                  on      off   */
  CMX_AF_MP3_EXT_INTENSITY_OFF_MS_ON = 2,  /*                  off     on    */
  CMX_AF_MP3_EXT_INTENSITY_ON_MS_ON = 3    /*                  on      on    */
} cmx_af_mp3_ext_enum_type;

/* MP3 Emphasis info
*/
typedef enum {
  CMX_AF_MP3_EMPHASIS_NONE,     /* Emphasis flag                             */
  CMX_AF_MP3_EMPHASIS_50_15_MS, /*                                           */
  CMX_AF_MP3_EMPHASIS_RESERVED, /*                                           */
  CMX_AF_MP3_EMPHASIS_CCITT_J17 /*                                           */
} cmx_af_mp3_emphasis_enum_type;

/* ID3V2 tag inforamtion */
typedef enum {
  CMX_ID3V2_TAG_INFO_BY_POSITION,
  CMX_ID3V2_TAG_INFO_BY_CONTENT
} cmx_id3v2_tag_info_type;

/* ID3V2 Tag Generic frame content for other frames */

typedef struct {
  cmx_id3v2_tag_info_type info_type;
  char    id[5];             /* id of this frame              */
  int16   flags;             /* flags of this frame            */
  union {
    uint8   *frame_content;    /* content of this frame         */
    uint32  file_position;     /* Frame content position in file */
  } frame_data;
  uint32  frame_size;        /* content size                  */
} cmx_id3v2_generic_frame_type; 


/* ID3V2 Tag frame info for other frames. 
 *
 */
typedef struct {
  boolean found;             /* whether this frame existing   */
  uint16  flags;             /* flags of this frame           */
  uint32  file_position;     /* Frame content position in file */
  uint32 size;               /* Size of the content */
} cmx_id3v2_frame_info_type;

/* ID3V2 Tag frame content
 *
 */
typedef struct {
  uint8  *content;
  uint32 size;
} cmx_id3v2_tag_frame_type;

/* ID3V2 Tag content,
 * frames we supported
 */
typedef struct {
  uint16 tag_ver;

  cmx_id3v2_tag_frame_type  title;
  cmx_id3v2_tag_frame_type  subtitle;
  cmx_id3v2_tag_frame_type  copyright;
  cmx_id3v2_tag_frame_type  produced;
  cmx_id3v2_tag_frame_type  composer;
  cmx_id3v2_tag_frame_type  artist;
  cmx_id3v2_tag_frame_type  orchestra;
  cmx_id3v2_tag_frame_type  conductor;
  cmx_id3v2_tag_frame_type  lyricist;
  cmx_id3v2_tag_frame_type  album;
  cmx_id3v2_tag_frame_type  track;
  cmx_id3v2_tag_frame_type  year;
  cmx_id3v2_tag_frame_type  publisher;
  cmx_id3v2_tag_frame_type  genre;
  cmx_id3v2_tag_frame_type  station;
  cmx_id3v2_tag_frame_type  encoder;
  cmx_id3v2_tag_frame_type  length;
  cmx_id3v2_frame_info_type apic;   /* Attached picture Information */
  cmx_id3v2_generic_frame_type *gframes; /* Generic frames */
  uint32   gframes_cnt; /* Generic frame count */
  /* gframes support TIT1, TFLT, TBPM, TSIZ, TDAT as well as Wxxx
     but currently we have maximum number of gframes. Customer can
     customize AUDMPEG_GFRAME_PRE_ALLOC_MEM in audmpeg.h */
} cmx_id3v2_tag_type;

/* Supported no of cmx_id3v2_tag_frame_type */ 
#define CMX_ID3V2_TAG_FRAME_TYPE_MAX 17


/* The following structure contains MP3 file specifications.  It is used with
** the CMX_AUDIO_SPEC playback callback and the cmx_audfmt_get_specs() function.
*/
typedef struct {
  cmx_af_file_enum_type             file_type;   /* Format of audio file      */
  cmx_af_suprted_ops_mask_type      supported_ops;/* Supported operations     */
  cmx_af_mp3_ver_enum_type          version;     /* MPEG version              */
  cmx_af_mpeg_layer_enum_type       layer;       /* MPEG layer compression    */
  boolean                           crc_f;       /* true = MPEG CRC protection*/
  cmx_af_mp3_bitrate_enum_type      bitrate;     /* MPEG audio bitrate        */
  cmx_af_sample_rate_enum_type      sample_rate; /* MPEG audio sample rate    */
  cmx_af_mp3_channel_enum_type      channel;     /* MPEG audio channel        */
  cmx_af_mp3_ext_enum_type          extension;   /* MPEG audio chan. extension*/
  boolean                           copyright_f; /* true = copyrighted        */
  boolean                           original_f;  /* true = original recording */
  cmx_af_mp3_emphasis_enum_type     emphasis;    /* MPEG audio emphasis       */
  char                              title[30];   /* MP3 song title            */
  char                              artist[30];  /* MP3 artist name           */
  char                              album[30];   /* MP3 album name            */
  char                              year[4];     /* MP3 recording year        */
  char                              comment[30]; /* MP3 ID3 Tag comments      */
  byte                              genre;       /* MP3 ID3 genre number      */

  boolean                           v2_tag_found; /* ID3V2 found or not       */
  cmx_id3v2_tag_type                v2_tag;       /* ID3V2 content holder     */
} cmx_af_mp3_spec_type;

/* MP3 codec type, no additional information is necessary to configure
** MP3 codec in streaming mode.
*/
typedef cmx_af_codec_type cmx_mp3_codec_type;

#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
/* MPEG4 audioObjectType
*/
typedef enum {
  CMX_AF_MP4_AUDIO_OBJECT_NULL            = 0,
  CMX_AF_MP4_AUDIO_OBJECT_AAC_MAIN,
  CMX_AF_MP4_AUDIO_OBJECT_AAC_LC,
  CMX_AF_MP4_AUDIO_OBJECT_AAC_SSR,
  CMX_AF_MP4_AUDIO_OBJECT_AAC_LTP,
  CMX_AF_MP4_AUDIO_OBJECT_AAC_SCALABLE    = 6,
  CMX_AF_MP4_AUDIO_OBJECT_ER_AAC_LC       = 17,
  CMX_AF_MP4_AUDIO_OBJECT_ER_AAC_LTP      = 19,
  CMX_AF_MP4_AUDIO_OBJECT_ER_AAC_SCALABLE = 20,
#ifdef FEATURE_AVS_BSAC  
  CMX_AF_MP4_AUDIO_OBJECT_AAC_BSAC        = 22,
#endif /* FEATURE_AVS_BSAC */
  CMX_AF_MP4_AUDIO_OBJECT_ER_AAC_LD       = 23,

  CMX_AF_MP4_AUDIO_OBJECT_UNKNOWN
} cmx_af_mp4_audio_object_type;

/* AAC Channel info
*/
typedef enum {
  CMX_AF_AAC_CHANNEL_UNKNOWN = 0,
  CMX_AF_AAC_CHANNEL_MONO,       /* Single channel (mono) data                */
  CMX_AF_AAC_CHANNEL_DUAL,       /* Stereo data                               */
  CMX_AF_AAC_CHANNEL_TRIPLE,     /* 3 channels: 1+2 (UNSUPPORTED)             */
  CMX_AF_AAC_CHANNEL_QUAD,       /* 4 channels: 1+2+1 (UNSUPPORTED)           */
  CMX_AF_AAC_CHANNEL_QUINTUPLE,  /* 5 channels: 1+2+2 (UNSUPPORTED)           */
  CMX_AF_AAC_CHANNEL_SEXTUPLE,   /* 5+1 channels: 1+2+2+1 (UNSUPPORTED)       */
  CMX_AF_AAC_CHANNEL_OCTUPLE,    /* 7+1 channels: 1+2+2+2+1 (UNSUPPORTED)     */
  CMX_AF_AAC_CHANNEL_DUAL_MONO,  /* Dual Mono: 1+1 (Two SCEs) */
  CMX_AF_AAC_CHANNEL_UNSUPPORTED /* Indicating CMX is currently playing unsupported
                                    Channel mode. */
} cmx_af_aac_channel_enum_type;


/* Define codec information update callback structures */
typedef struct {
  cmx_af_file_enum_type          codec_type;
  cmx_af_aac_channel_enum_type   channel_mode;  /* Channel Mode update */
  uint32                         sample_rate;  /* Sample Rate Update */
} cmx_af_aac_info_type;

typedef union {
  cmx_af_file_enum_type                    codec_type;
#ifdef FEATURE_AAC
  cmx_af_aac_info_type                     aac_info;
#endif /* FEATURE_AAC */
} cmx_af_codec_info_type;

typedef void (*cmx_af_codec_info_update_cb_ptr_type) (
  cmx_af_codec_info_type                  codec_info,
  const void                              *client_data       /* Client data passed in cmx_audfmt_play_codec  */
);

/* Enum for mapping dual-mono contents to left and right channels */
typedef enum {
  CMX_AF_DUAL_MONO_MODE_FL_FR, /* 1st SCE to left & right */
  CMX_AF_DUAL_MONO_MODE_SL_SR, /* 2nd SCE to left & right */
  CMX_AF_DUAL_MONO_MODE_SL_FR, /* 2nd SCE to left, main to right */
  CMX_AF_DUAL_MONO_MODE_FL_SR, /* 1st SCE to left, sub to right default */
  CMX_AF_DUAL_MONO_MODE_DEFAULT = 
  CMX_AF_DUAL_MONO_MODE_FL_SR
} cmx_af_dual_mono_mode_enum_type;

/* Enum for enable/disable SBR */
typedef  enum {
  CMX_AF_SBR_MODE_DISABLE,
  CMX_AF_SBR_MODE_ENABLE,
  CMX_AF_SBR_MODE_DEFAULT = 
  CMX_AF_SBR_MODE_ENABLE
} cmx_af_sbr_mode_enum_type;


/* Track information that will be provided by the audaac module (for use in UI)
*/
typedef struct {
  cmx_af_file_enum_type             file_type;   /* Format of audio file      */
  cmx_af_suprted_ops_mask_type      supported_ops; /* Supported operations    */
  cmx_af_mpeg_layer_enum_type       layer;       /* MPEG layer compression    */
  boolean                           crc_f;       /* true = MPEG CRC protection*/
  cmx_af_mp4_audio_object_type      audio_object;/* AAC profile complexity    */
  cmx_af_sample_rate_enum_type      sample_rate; /* MPEG audio sample rate    */
  boolean                           private_f;   /* Unused                    */
  cmx_af_aac_channel_enum_type      channel;     /* MPEG audio channel        */
  boolean                           original_f;  /* true = original recording */
  boolean                           home_f;      /* true = on home media      */
  uint32                            bit_rate;    /* Computed bit rate         */
  char                              title[30];   /* MP3 song title            */
  char                              artist[30];  /* MP3 artist name           */
  char                              album[30];   /* MP3 album name            */
  char                              year[4];     /* MP3 recording year        */
  char                              comment[30]; /* MP3 ID3 Tag comments      */
  byte                              genre;       /* MP3 ID3 genre number      */
} cmx_af_aac_spec_type;

/* AAC Data formats
*/
typedef enum {
  CMX_AF_AAC_DATA_FORMAT_ADIF,       /* ADIF                   */
  CMX_AF_AAC_DATA_FORMAT_ADTS,       /* ADTS                   */
  CMX_AF_AAC_DATA_FORMAT_RAW,        /* Raw (PV)               */
  CMX_AF_AAC_DATA_FORMAT_PSUEDO_RAW, /* Raw with frame markers */
  CMX_AF_AAC_DATA_FORMAT_LOAS        /* LOAS                   */
} cmx_af_aac_data_format_enum_type;

/* Information required to playback AAC data in streaming mode
*/
typedef struct {
  cmx_af_codec_type                codec_type;    /* Codec type              */
  cmx_af_aac_data_format_enum_type format;        /* AAC encoding format     */
  cmx_af_mp4_audio_object_type     audio_object;  /* Profile complexity      */
  cmx_af_sample_rate_enum_type     sample_rate;   /* Sampling rate           */
  cmx_af_aac_channel_enum_type     num_channels;  /* Number of channels      */
  uint32                           bit_rate;      /* Bitrate                 */

  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  byte                             ep_config;
  byte                             aac_section_data_resilience_flag;
  byte                             aac_scalefactor_data_resilience_flag;
  byte                             aac_spectral_data_resilience_flag;
} cmx_aac_codec_type;


typedef struct {
  cmx_af_codec_type               codec_type;    /* Codec type              */
  cmx_af_sample_rate_enum_type    sample_rate;   /* Sampling rate set to 
                                                  CMX_AF_SAMPLE_RATE_DEFAULT 
                                                 if sample rate is unknown */
  cmx_af_dual_mono_mode_enum_type dual_mono_mode; 
  cmx_af_sbr_mode_enum_type       sbr_mode;
  /* Callback function pointer for getting channel information and 
     sample rate update*/
  cmx_af_codec_info_update_cb_ptr_type  codec_update_cb_func; 
} cmx_af_aac_bc_codec_type; /* aac codec type for broadcasting purpose 
    supporting dynamic switching of sample rate and channel mode */ 

#endif /* FEATURE_AAC */

#ifdef FEATURE_REAL_AUDIO

/* Number of audio channels
*/
typedef enum {
  CMX_AF_RA_CHANNEL_MONO = 1,   /* Single channel (mono) data                */
  CMX_AF_RA_CHANNEL_DUAL        /* Stereo data                               */
} cmx_af_ra_channel_type;

typedef enum {
  CMX_AF_RA_DECODER_GECKO1 = 1,
  CMX_AF_RA_DECODER_GECKO2
} cmx_af_ra_decoder_mode;

typedef enum {
  CMX_AF_RA_SAMPLES_PER_FRAME_256  = 256,
  CMX_AF_RA_SAMPLES_PER_FRAME_512  = 512,
  CMX_AF_RA_SAMPLES_PER_FRAME_1024 = 1024
} cmx_af_ra_samples_per_frame_type;

/* Structure used to indicate information about RealAudio file to be played
*/
typedef struct {
  cmx_af_codec_type                codec_type;    /* Codec type               */
  cmx_af_ra_decoder_mode           decoder_mode;  /* Decoder (Gecko) mode     */
  cmx_af_sample_rate_enum_type     sample_rate;   /* Sampling rate            */
  cmx_af_ra_channel_type           num_channels;  /* Number of channels       */
  cmx_af_ra_samples_per_frame_type samples_per_frame; /* Samples per frame    */
  uint16                           num_regions;   /* Number of regions        */
  uint16                           bits_per_frame; /* Number of bits per frame*/
  uint16                           coupling_start; /* Stereo coupling region  */
  uint16                           coupling_quant; /* Coupling quantization   */
  uint16                           frame_erasure;  /* Frame erasure flag      */
} cmx_ra_codec_type;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_IMELODY
typedef struct {
  cmx_af_file_enum_type           file_type;   /* Format of audio file      */
  cmx_af_suprted_ops_mask_type    supported_ops; /* Supported operations    */
  char                            name[76];
  char                            composer[76];
} cmx_af_imy_spec_type;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_WAVE_PB

typedef enum {
  CMX_AF_WAVE_FORMAT_LINEAR_PCM,  /* Linear PCM (0x01) */
  CMX_AF_WAVE_FORMAT_ALAW,        /* ALaw (0x06)       */
  CMX_AF_WAVE_FORMAT_MULAW,       /* uLaw (0x07)       */
  CMX_AF_WAVE_FORMAT_IMA_ADPCM,   /* IMA ADPCM  (0x11) */
  CMX_AF_WAVE_FORMAT_INVALID
} cmx_af_wave_format_type;

/* Track information about the clip that will be played (for use by the UI).
*/
typedef struct {
  cmx_af_file_enum_type        file_type;   /* Format of audio file         */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations       */
  uint16                       channels;    /* Number of channels           */
  uint32                       sample_rate; /* Samples per second           */
  uint32                       bytes_rate;  /* Average bytes per second     */
  uint16                       frame_size;  /* Block Align, num of bytes    */
                                            /* for n-channels               */
  uint16                       bits_per_sample; /* Number of bits per sample*/
  cmx_af_wave_format_type      format;      /* PCM encoding format          */
} cmx_af_wave_spec_type;

#ifdef FEATURE_ADPCM
/* Retained for backward compatibility */
typedef cmx_af_wave_spec_type cmx_af_adpcm_spec_type;
#endif /* FEATURE_ADPCM */

#ifdef FEATURE_PCM
/* This type indicates the format of PCM samples
*/
typedef enum {
  CMX_AF_WAVE_SAMPLE_FMT_SIGNED,     /* Samples are signed    */
  CMX_AF_WAVE_SAMPLE_FMT_UNSIGNED,   /* Samples are unsigned  */
  CMX_AF_WAVE_SAMPLE_FMT_INVALID
} cmx_af_wave_sample_fmt_type;

/* Structure used to indicate information about PCM file to be played
*/
typedef struct {
  cmx_af_file_enum_type       file_type;       /* Format of audio file       */
  cmx_af_wave_format_type     format;          /* PCM encoding format        */
  cmx_af_wave_sample_fmt_type sample_format;   /* format of PCM samples      */
  uint32                      sample_rate;     /* Sampling rate in Hz        */
  uint16                      bits_per_sample; /* Number of bits per sample  */
  uint16                      channels;        /* Number of channels         */
} cmx_af_wave_codec_type;
#endif /* FEATURE_PCM */
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_WMA
/* Structure used to indicate information about Window Media Audio to be
** played
*/
typedef struct {
  cmx_af_file_enum_type file_type;         /* Format of audio file      */
  cmx_af_sample_rate_enum_type sample_rate;/* Sampling rate in Hz       */
  uint8                 bits_per_sample;      /* Valid bits per sample               */
  uint16                version;           /* version number (2 only)   */
  uint16                channels;          /* Number of channels 1 or 2 */
  uint16                bytes_per_second;  /* Number of bits per second */
  uint16                encoder_options;   /* Encoder options           */
  uint16                virtual_pkt_len;   /* VirtulaPacketLength in ASF */
  uint16                advanced_encoder_opt1; /* Support for LBR bitstream */
  uint16                channel_mask;         /* For stero or mono streams  */
  uint16                format_tag;           /* Codec ID: 0x162 or 0x166 for WMAPRO 10 */
  uint32                advanced_encoder_opt2; /* Support for LBR bitstream */
} cmx_af_wma_codec_type;
#endif /* FEATURE_WMA */

#ifdef FEATURE_QCP
typedef struct {
  cmx_af_file_enum_type        file_type;     /* Format of audio file */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations */
} cmx_af_qcp_spec_type;

typedef struct {
  cmx_af_file_enum_type        file_type;     /* Format of audio file */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations */
} cmx_af_evb_spec_type;

typedef struct {
  cmx_af_file_enum_type        file_type;     /* Format of audio file */
  cmx_af_suprted_ops_mask_type supported_ops; /* Supported operations */
} cmx_af_evw_spec_type;

typedef cmx_af_qcp_spec_type cmx_af_amr_spec_type;
#endif /* FEATURE_QCP */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
typedef struct {
  cmx_af_codec_type            codec_type;   /* Codec type         */
  cmx_af_sample_rate_enum_type sample_rate;  /* Sampling rate      */
  cmx_af_channel_enum_type     channel_mode; /* Number of channels */
} cmx_af_ext_speech_codec_type;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

/* The following type is used to return the specifications of the audio
** file. Currently, it only has two fields but new fields can be added.
*/
typedef union {
  cmx_af_file_enum_type      file_type; /* Format of audio file               */
#ifdef FEATURE_MIDI
  cmx_af_midi_spec_type      midi_spec; /* MIDI file specifications           */
#endif /* FEATURE_MIDI */
#ifdef FEATURE_WEBAUDIO
  cmx_af_wa_spec_type        wa_spec;   /* WebAudio file specifications       */
#ifdef FEATURE_SMAF
  cmx_af_smaf_spec_type      smaf_spec; /* SMAF file specifications           */
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_MP3
  cmx_af_mp3_spec_type       mp3_spec;  /* MP3 file specifications            */
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
  cmx_af_aac_spec_type       aac_spec;  /* AAC file specifications            */
#endif /* FEATURE_AAC */
#ifdef FEATURE_IMELODY
  cmx_af_imy_spec_type       imy_spec;  /* IMY file specifications            */
#endif /* FEATURE_IMELODY */
#ifdef FEATURE_ADPCM
  /* Retained for backward compatibility */
  cmx_af_adpcm_spec_type     adpcm_spec;/* Specs for ADPCM files              */
#endif /* FEATURE_ADPCM */
#ifdef FEATURE_WAVE_PB
  cmx_af_wave_spec_type      wave_spec; /* Specs for WAVE files               */
#endif /* FEATURE_WAVE_PB */
#ifdef FEATURE_QCP
  cmx_af_qcp_spec_type       qcp_spec;  /* Specs for QCP/AMR files            */
  cmx_af_amr_spec_type       amr_spec;  /* Specs for AMR file                 */
  cmx_af_evb_spec_type       evb_spec;  /* Specs for EVB file                 */
  cmx_af_evw_spec_type       evw_spec;  /* Specs for EVW file                 */  
#endif /* FEATURE_QCP */
} cmx_af_audio_spec_type;

#ifdef FEATURE_SVG

/* This is used to set max limit of URL length
*/
#define CMX_SVG_MAX_URL_LENGTH     256

/* This is used as callback function type for external file/URL requests
*/
typedef void (*cmx_svg_request_cb_func_ptr)(
  void*              handle,            /* CMX Handle                        */
  uint8**             buffer,            /* Buffer with URL data in one chunk */
  uint32             num_bytes          /* No of bytes in buffer             */
);

/* This is used as callback function type for Frame Buffer
*/
typedef void (*cmx_svg_frame_cb_func_ptr)( void* data );

/* SVG decoder mode, in CMX mode frames are timed by CMX */
typedef enum{
  CMX_SVG_MODE_CMX,
  CMX_SVG_MODE_CLIENT,
  CMX_SVG_MODE_INVALID
} cmx_svg_decode_mode_enum_type;

/* Color scheme type; SVG color scheme can be changed at runtime */
typedef enum{
  CMX_COLOR_SCHEME_NONE = 1,    /* Unknown Color Scheme */
  CMX_COLOR_SCHEME_RGB565,  /* RGB 565 format, 16 bits/pixel */
  CMX_COLOR_SCHEME_RGB888,  /* RGB 888 format, 24 bits/pixel */
  CMX_COLOR_SCHEME_0RGB888,  /* RGB 888 format, 32 bits/pixel */
  CMX_COLOR_SCHEME_ARGB888  /*Alpha + RGB 888 format, 32 bits/pixel */
} cmx_color_scheme_enum_type;

/* DOM Document type */
typedef void* cmx_svgdom_document_type;

#ifdef FEATURE_SVGDOM
/* DOM float type */
/* Floats are represented in 16.16 fixed format */
typedef uint32 cmx_svgdom_fixed;

/* DOM Node */
typedef void* cmx_svgdom_node_type;

/* DOM SVGElement */
typedef cmx_svgdom_node_type cmx_svgdom_element_type;

/* DOM DOMString */
typedef uint16* cmx_svgdom_string_type;

/* DOM Event type */
typedef void* cmx_svgdom_event_type;

/* DOM Event Target type */
typedef void* cmx_svgdom_evttarget_type;

/* DOM Event Listener type */
typedef void* cmx_svgdom_evtlistener_type;

/* DOM HandleEvent macro */
typedef void (*cmx_svgdom_handle_event_type)( void* client_data, void *p, cmx_svgdom_event_type *evt );

/* DOM arguments to Event Target interface */
typedef struct{
  cmx_svgdom_node_type target;      /* Pointer to Event Target */
  cmx_svgdom_string_type  type;          /* Pointer to Event type */
  cmx_svgdom_evtlistener_type listener;      /* Pointer to Event Listener */
  boolean useCapture;
  cmx_svgdom_event_type event;
} cmx_svgdom_evttarget_data_type;

typedef struct{
  cmx_svgdom_evtlistener_type listener;
  cmx_svgdom_handle_event_type handler;
  void*                        client_data;
} cmx_svgdom_evthandler_type;

typedef struct{
  cmx_svgdom_event_type event;             /* Pointer to Event */
  cmx_svgdom_string_type  evttarget_type;  /* Event Target type */
  boolean canbubble;
  boolean cancelable;
} cmx_svgdom_event_data_type;

typedef void* cmx_svgdom_matrix_type;


typedef void* cmx_svgdom_path_type;


typedef struct{
  cmx_svgdom_fixed x, y;
} cmx_svgdom_point_type;


typedef struct{
  cmx_svgdom_fixed x, y, width, height;
} cmx_svgdom_rect_type;



typedef struct{
  uint32 red, green, blue;
} cmx_svgdom_color_type;

/* DOM API type */
typedef enum{
  CMX_SVGDOM_API_NONE = 0,

  CMX_SVGDOM_API_NODE_NSURI,            /* Node::namespaceURI */
  CMX_SVGDOM_API_NODE_LOCALNAME,        /* Node::localName */
  CMX_SVGDOM_API_NODE_PARENT,           /* Node::parentNode */
  CMX_SVGDOM_API_NODE_APPEND,           /* Node::appendChild() */
  CMX_SVGDOM_API_NODE_INSERT,           /* Node::insertBefore() */
  CMX_SVGDOM_API_NODE_REMOVE,           /* Node::removeChild() */
  CMX_SVGDOM_API_NODE_GETDOCUMENT,      /* Node::GetOwnerDocument */
  CMX_SVGDOM_API_NODE_GETCONTENT,       /* Node::GetTextContent */
  CMX_SVGDOM_API_NODE_SETCONTENT,       /* Node::SetTextContent */

  CMX_SVGDOM_API_DOC_CREATE,            /* Document::createElementNS()*/
  CMX_SVGDOM_API_DOC_ELEMENT,           /* Document::documentElement */
  CMX_SVGDOM_API_DOC_GETBYID,           /* Document::getElementById() */
  CMX_SVGDOM_API_DOC_DELETE,            /* Deletes an unused Node */
  CMX_SVGDOM_API_DOC_CREATEEVENT,       /* Document::createEvent()*/
  CMX_SVGDOM_API_DOC_DESTROYEVENT,      /* Destroys an unused Event */

  CMX_SVGDOM_API_ELEMENT_FIRSTCHILD,    /* ElementTraversal::firstElementChild */
  CMX_SVGDOM_API_ELEMENT_LASTCHILD,     /* ElementTraversal::lastElementChild */
  CMX_SVGDOM_API_ELEMENT_NEXTSIBLING,   /* ElementTraversal::nextElementSibling */
  CMX_SVGDOM_API_ELEMENT_PREVSIBLING,   /* ElementTraversal::previousElementSibling */

  CMX_SVGDOM_API_ELEMENT_GETID,         /* SVGElement::Id */
  CMX_SVGDOM_API_ELEMENT_SETID,         /* SVGElement::Id */

  CMX_SVGDOM_API_TRAIT_GETTRAIT,        /* TraitAccess::getTrait*/
  CMX_SVGDOM_API_TRAIT_SETTRAIT,        /* TraitAccess::setTrait*/
  CMX_SVGDOM_API_TRAIT_GETTRAITNS,      /* TraitAccess::getTraitNS*/
  CMX_SVGDOM_API_TRAIT_SETTRAITNS,      /* TraitAccess::setTraitNS*/
  CMX_SVGDOM_API_TRAIT_GETFLOAT,        /* TraitAccess::getFloatTrait*/
  CMX_SVGDOM_API_TRAIT_SETFLOAT,        /* TraitAccess::setFloatTrait*/
  CMX_SVGDOM_API_TRAIT_GETMATRIX,       /* TraitAccess::getMatrixTrait*/
  CMX_SVGDOM_API_TRAIT_SETMATRIX,       /* TraitAccess::setMatrixTrait*/
  CMX_SVGDOM_API_TRAIT_GETRECT,         /* TraitAccess::getRectTrait*/
  CMX_SVGDOM_API_TRAIT_SETRECT,         /* TraitAccess::setRectTrait*/
  CMX_SVGDOM_API_TRAIT_GETPATH,         /* TraitAccess::getPathTrait*/
  CMX_SVGDOM_API_TRAIT_SETPATH,         /* TraitAccess::setPathTrait*/
  CMX_SVGDOM_API_TRAIT_GETRGB,          /* TraitAccess::getRGBColorTrait*/
  CMX_SVGDOM_API_TRAIT_SETRGB,          /* TraitAccess::setRGBColorTrait*/

  CMX_SVGDOM_API_TRAIT_GETPRESTRAIT,    /* TraitAccess::getPresentationTrait */
  CMX_SVGDOM_API_TRAIT_GETPRESTRAITNS,  /* TraitAccess::getPresentationTraitNS */
  CMX_SVGDOM_API_TRAIT_GETPRESFLOAT,    /* TraitAccess::getFloatPresentationTrait */
  CMX_SVGDOM_API_TRAIT_GETPRESMATRIX,   /* TraitAccess::getMatrixPresentationTrait */
  CMX_SVGDOM_API_TRAIT_GETPRESPATH,     /* TraitAccess::getPathPresentationTrait */
  CMX_SVGDOM_API_TRAIT_GETPRESRECT,     /* TraitAccess::getRectPresentationTrait */
  CMX_SVGDOM_API_TRAIT_GETPRESCOLOR,    /* TraitAccess::getRGBColorPresentationTrait */


  CMX_SVGDOM_API_EVTTARGET_ADD,         /* EventTarget::addEventListener */
  CMX_SVGDOM_API_EVTTARGET_RMV,         /* EventTarget::removeEventListener */
  CMX_SVGDOM_API_EVTTARGET_DISPATCH,    /* EventTarget::DispatchEvent */
  CMX_SVGDOM_API_EVTLISTENER_CREATE,     /* Creates EventListner */
  CMX_SVGDOM_API_EVTLISTENER_FREE,       /* Frees EventListner */

  CMX_SVGDOM_API_EVENT_GETTARGET,       /* Event::currentTarget */
  CMX_SVGDOM_API_EVENT_GETTYPE,         /* Event::type */
  CMX_SVGDOM_API_EVENT_SCREENX,         /* MouseEvent::screenX */
  CMX_SVGDOM_API_EVENT_SCREENY,         /* MouseEvent::screenY */
  CMX_SVGDOM_API_EVENT_CLIENTX,         /* MouseEvent::clientX */
  CMX_SVGDOM_API_EVENT_CLIENTY,         /* MouseEvent::clientY */
  CMX_SVGDOM_API_EVENT_BUTTON,          /* MouseEvent::button */
  CMX_SVGDOM_API_EVENT_DATA,            /* TextEvent::data */
  CMX_SVGDOM_API_EVENT_KEY,             /* KeyEvent::keyIdentifier */
  CMX_SVGDOM_API_EVENT_DETAIL,          /* TimeEvent::detail */
  CMX_SVGDOM_API_EVENT_INIT,            /* Initializes an Event */

  CMX_SVGDOM_API_MATRIX_GET,            /* SVGMatrix::getComponent*/
  CMX_SVGDOM_API_MATRIX_MULTIPLY,       /* SVGMatrix::mMultiply*/
  CMX_SVGDOM_API_MATRIX_INVERSE,        /* SVGMatrix::mInverse*/
  CMX_SVGDOM_API_MATRIX_TRANSLATE,      /* SVGMatrix::mTranslate*/
  CMX_SVGDOM_API_MATRIX_SCALE,          /* SVGMatrix::mScale*/
  CMX_SVGDOM_API_MATRIX_ROTATE,         /* SVGMatrix::mRotate*/

  CMX_SVGDOM_API_LOCATABLE_GETBBOX,     /* SVGLocatableElement::getBBox */
  CMX_SVGDOM_API_LOCATABLE_GETSCTM,     /* SVGLocatableElement::getScreenCTM */
  CMX_SVGDOM_API_LOCATABLE_GETSBOX,     /* SVGLocatableElement::getScreenBBox*/

  CMX_SVGDOM_API_ELEMENT_BEGIN,       /* SVGAnimationElement::beginElement */
  CMX_SVGDOM_API_ELEMENT_BEGINAT,     /* SVGAnimationElement::beginElementAt*/
  CMX_SVGDOM_API_ELEMENT_END,         /* SVGAnimationElement::endElement*/
  CMX_SVGDOM_API_ELEMENT_ENDAT,       /* SVGAnimationElement::endElementAt */
  CMX_SVGDOM_API_ELEMENT_PAUSE,       /* SVGAnimationElement::unpauseElement*/
  CMX_SVGDOM_API_ELEMENT_UNPAUSE,     /* SVGAnimationElement::pauseElement*/
  CMX_SVGDOM_API_ELEMENT_PAUSED,      /* SVGAnimationElement::elementPaused*/

  CMX_SVGDOM_API_SVGELEMENT_CREATEMATRIX, /* SVGSVGElement::createSVGMatrixComponents */
  CMX_SVGDOM_API_SVGELEMENT_CREATERECT,   /* SVGSVGElement::createSVGRect */
  CMX_SVGDOM_API_SVGELEMENT_CREATEPATH,   /* SVGSVGElement::createSVGPath */
  CMX_SVGDOM_API_SVGELEMENT_CREATECOLOR,  /* SVGSVGElement::createSVGRGBColor */
  CMX_SVGDOM_API_SVGELEMENT_DELETEMATRIX, /* Deletes a SVGMatrix */
  CMX_SVGDOM_API_SVGELEMENT_DELETERECT,   /* Deletes a SVGRect */
  CMX_SVGDOM_API_SVGELEMENT_DELETEPATH,   /* Deletes a SVGPath */
  CMX_SVGDOM_API_SVGELEMENT_DELETECOLOR,  /* Deletes a SVGColor */
  CMX_SVGDOM_API_SVGELEMENT_GETSCALE,     /* SVGSVGElement::currentScale */
  CMX_SVGDOM_API_SVGELEMENT_GETROTATE,    /* SVGSVGElement::currentRotate */
  CMX_SVGDOM_API_SVGELEMENT_GETTRANSLATE, /* SVGSVGElement::currentTranslate */
  CMX_SVGDOM_API_SVGELEMENT_GETVIEWPORT,  /* SVGSVGElement::viewport */
  CMX_SVGDOM_API_SVGELEMENT_GETTIME,      /* SVGSVGElement::currentTime */
  CMX_SVGDOM_API_SVGELEMENT_SETSCALE,     /* SVGSVGElement::currentScale */
  CMX_SVGDOM_API_SVGELEMENT_SETROTATE,    /* SVGSVGElement::currentRotate */
  CMX_SVGDOM_API_SVGELEMENT_SETTIME,      /* SVGSVGElement::currentTime */
  CMX_SVGDOM_API_SVGELEMENT_SETTRANSLATE, /* SVGSVGElement::currentTranslate */
  CMX_SVGDOM_API_SVGELEMENT_PAUSE,        /* SVGSVGElement::pauseAnimations */
  CMX_SVGDOM_API_SVGELEMENT_UNPAUSE,      /* SVGSVGElement::unpauseAnimations */
  CMX_SVGDOM_API_SVGELEMENT_PAUSED,       /* SVGSVGElement::animationsPaused */
  CMX_SVGDOM_API_SVGELEMENT_MOVEFOCUS,       /* SVGSVGElement::moveFocus */
  CMX_SVGDOM_API_SVGELEMENT_SETFOCUS,        /* SVGSVGElement::setFocus */
  CMX_SVGDOM_API_SVGELEMENT_GETFOCUSEDOBJECT,/* SVGSVGElement::getCurrentFocusedObject */

  CMX_SVGDOM_API_SVGPATH_NOSEGMENTS,      /* SVGPath::numberOfSegments */
  CMX_SVGDOM_API_SVGPATH_GETSEGMENT,      /* SVGPath::getSegment */
  CMX_SVGDOM_API_SVGPATH_GETSEGMENTPARAM, /* SVGPath::getSegmentParam */
  CMX_SVGDOM_API_SVGPATH_MOVETO,          /* SVGPath::moveTo */
  CMX_SVGDOM_API_SVGPATH_LINETO,          /* SVGPath::lineTo */
  CMX_SVGDOM_API_SVGPATH_QUADTO,          /* SVGPath::quadTo */
  CMX_SVGDOM_API_SVGPATH_CURVETO,         /* SVGPath::curveTo */
  CMX_SVGDOM_API_SVGPATH_CLOSE,           /* SVGPath::Close */

  CMX_SVGDOM_API_MAX
}cmx_svgdom_api_enum_type;


/* DOM exception type */
typedef enum {
  CMX_SVGDOM_NO_ERROR = 0,
  CMX_SVGDOM_INDEX_SIZE_ERR = 1,
  CMX_SVGDOM_HIERARCHY_REQUEST_ERR = 3,
  CMX_SVGDOM_WRONG_DOCUMENT_ERR = 4,
  CMX_SVGDOM_NO_MODIFICATION_ALLOWED_ERR = 7,
  CMX_SVGDOM_NOT_FOUND_ERR = 8,
  CMX_SVGDOM_NOT_SUPPORTED_ERR = 9,
  CMX_SVGDOM_INVALID_STATE_ERR = 11,
  CMX_SVGDOM_INVALID_MODIFICATION_ERR = 13,
  CMX_SVGDOM_INVALID_ACCESS_ERR = 15,
  CMX_SVGDOM_TYPE_MISMATCH_ERR = 17,

  CMX_SVGDOM_EVENT_UNSPECIFIED_EVENT_TYPE_ERR = 100,
  CMX_SVGDOM_EVENT_OUT_OF_MEMORY_ERR,

  CMX_SVGDOM_SVG_INVALID_VALUE_ERR = 200,
  CMX_SVGDOM_SVG_MATRIX_NOT_INVERTABLE
} cmx_svgdom_exception_type;


/* DOM type of data */
typedef enum{
  CMX_SVGDOM_DATA_NONE = 0,
  CMX_SVGDOM_DATA_DOCUMENT,        /* doc is valid */
  CMX_SVGDOM_DATA_ELEMENT,     /* element is valid */
  CMX_SVGDOM_DATA_NODE,            /* node is valid */
  CMX_SVGDOM_DATA_NS,          /* ns_value is valid */

  CMX_SVGDOM_DATA_STRING,      /* string is valid */
  CMX_SVGDOM_DATA_FLOAT,       /* f is valid */
  CMX_SVGDOM_DATA_BOOL,            /* b is valid */
  CMX_SVGDOM_DATA_LONG,            /* l is valid */
  CMX_SVGDOM_DATA_SHORT,       /* s is valid */

  CMX_SVGDOM_DATA_EVENT,       /* event is valid */
  CMX_SVGDOM_DATA_EVTTARGET,    /* evttarget is valid */
  CMX_SVGDOM_DATA_EVTLISTENER,  /* evtlistener is valid */

  CMX_SVGDOM_DATA_MATRIX,      /* matrix is valid */
  CMX_SVGDOM_DATA_MATRIX_DATA,  /* matrix_data is valid */
  CMX_SVGDOM_DATA_POINT,       /* point is valid */
  CMX_SVGDOM_DATA_PATH,            /* path is valid */
  CMX_SVGDOM_DATA_RECT,            /* rect is valid */
  CMX_SVGDOM_DATA_COLOR,       /* color is valid */
  CMX_SVGDOM_DATA_COLOR_PTR,       /* color_ptr is valid */

  CMX_SVGDOM_DATA_SVGSVGELEMENT    /* element is valid */

} cmx_svgdom_data_enum_type;

/* DOM arguments to Node interface */
typedef struct{
  cmx_svgdom_node_type node;           /* Parent Node */
  cmx_svgdom_node_type child;          /* Child Node  */
  cmx_svgdom_node_type refchild;       /*Reference Child*/
  cmx_svgdom_string_type content;      /* Text Content of Node */
} cmx_svgdom_node_data_type;

/* DOM arguments to Document interface */
typedef struct{
  cmx_svgdom_document_type document;   /* Pointer to Document */
  cmx_svgdom_string_type   name;   /* Pointer to Qualified name or ID */
  union{
    cmx_svgdom_string_type   URI;        /* Pointer to NS URI */
    cmx_svgdom_element_type  element;       /* Pointer to Element */
    cmx_svgdom_event_type  event;       /* Pointer to Event */
  }data;
}cmx_svgdom_doc_data_type;

/* DOM arguments to SVGElement interface */
typedef struct{
  cmx_svgdom_element_type element;
  cmx_svgdom_string_type name;
  cmx_svgdom_data_enum_type type;    /* Type of data in union */
  union{
    cmx_svgdom_string_type string;
    cmx_svgdom_matrix_type matrix;
    cmx_svgdom_path_type   path;
    cmx_svgdom_rect_type   rect;
    cmx_svgdom_color_type   color;
    cmx_svgdom_fixed                  fl;
     struct{
      cmx_svgdom_string_type ns;
      cmx_svgdom_string_type value;
    }ns_value;
  }data;
} cmx_svgdom_element_data_type;

/* DOM arguments to SVGMatrix interface */
typedef struct{
  cmx_svgdom_matrix_type matrix;
  cmx_svgdom_exception_type error;
  cmx_svgdom_data_enum_type type;
  union{
    cmx_svgdom_matrix_type secondMatrix;   /* Used with mMultiply */
    cmx_svgdom_fixed f;            /* Used with mScale and mRotate */
    cmx_svgdom_point_type p;         /* Used with mTranslate */
    long  index;        /* Used with getComponent */
  }data;
}cmx_svgdom_matrix_data_type;

/* DOM arguments to SVGSVGElement interface */
typedef struct{
  cmx_svgdom_element_type element;
  cmx_svgdom_data_enum_type type;
  union{
    cmx_svgdom_fixed f ;               /* Used with currentScale, currentRotate & currentTime */
    struct{
      long red, green, blue;
    }color;                            /* Used with createSVGRGBColor */
    struct{
      cmx_svgdom_fixed a, b, c, d, e, f;
    }matrix_data;                     /* Used with createSVGMAtrixComponents */
    cmx_svgdom_point_type point;      /* Used with setCurrentTranslate */
    cmx_svgdom_path_type path;        /* Used with dleeteSVGPath */
    cmx_svgdom_matrix_type m;         /* Used with dleeteSVGMatrix */
    cmx_svgdom_rect_type *rect_ptr;   /* Used with dleeteSVGRect */
    cmx_svgdom_color_type *color_ptr; /* Used with dleeteSVGRGBColor */
    uint8                 s;          /* Used with MoveFocus */
    cmx_svgdom_node_type  node;       /* Used with setFocus */
  }data;
} cmx_svgdom_svgelement_data_type;

typedef struct{
  cmx_svgdom_path_type  path;
  union{
    uint32  index;                    /* Used with getSegement */
    struct{
      uint32 cmd, param;
    }param;                           /* Used with getSegmentParam */
    struct{
      cmx_svgdom_fixed x1, y1;
    }move;                            /* Used with moveTo */
    struct{
      cmx_svgdom_fixed x1, y1;
    }line;                            /* Used with lineTo */
    struct{
      cmx_svgdom_fixed x1, y1, x2, y2;
    }quad;                            /* used with quadTo */
    struct{
      cmx_svgdom_fixed x1, y1, x2, y2, x3, y3;
    }curve;                           /* used with curveTo */
  }data;
} cmx_svgdom_svgpath_data_type;

/* DOM callback function data */
typedef struct{
  cmx_svgdom_api_enum_type  api;       /* API called */
  cmx_svgdom_exception_type error; /* Exception if any */
  cmx_svgdom_data_enum_type type;    /* Type of data in union */
  uint32                    num_bytes;  /* No of bytes in data */
  union{
    cmx_svgdom_string_type      string;    /* DOMString data */
    struct{
      cmx_svgdom_string_type ns;
      cmx_svgdom_string_type value;
    }ns_value;                      /* Namespace data */
    cmx_svgdom_document_type    doc;    /* document data */
    cmx_svgdom_element_type    element;    /* element data */
    cmx_svgdom_node_type       node;   /* node data */
    cmx_svgdom_event_type  event;  /* Event data */
    cmx_svgdom_evttarget_type evttarget; /* EventTarget data */
    cmx_svgdom_evtlistener_type evtlistener; /* EventListener data */
    cmx_svgdom_point_type  point;  /* SVGPoint data */
    cmx_svgdom_matrix_type     matrix; /* SVGMatrix data */
    cmx_svgdom_path_type       path;   /* SVGPath data */
    cmx_svgdom_rect_type      *rect;   /* SVGRect data */
    cmx_svgdom_color_type     *color;  /* SVGRGBColor data */
    cmx_svgdom_fixed           f;      /* float data */
    boolean                    b;      /* bool data */
    int                        i;      /* int data */
    long                       l;      /* long data */
    uint16                     s;      /* unsigned short data */
  }data;
} cmx_svgdom_data_type;

/* DOM API data type */
typedef union{
  cmx_svgdom_node_type             node;
  cmx_svgdom_node_data_type        node_data;
  cmx_svgdom_document_type         doc;
  cmx_svgdom_doc_data_type         doc_data;
  cmx_svgdom_event_type            event;
  cmx_svgdom_event_data_type       event_data;
  cmx_svgdom_evttarget_data_type   evttarget_data;
  cmx_svgdom_evtlistener_type      evtlistener;
  cmx_svgdom_evthandler_type       evthandler;
  cmx_svgdom_element_type          element;
  cmx_svgdom_element_data_type     element_data;
  cmx_svgdom_matrix_data_type      matrix_data;
  cmx_svgdom_svgelement_data_type  svgelement_data;
  cmx_svgdom_svgpath_data_type     svgpath_data;
} cmx_svgdom_para_type;
#endif /* FEATURE_SVGDOM */

/* Type used to pass arguments to decode SVG content */
typedef struct{
  uint32      width, height;                        /* Display rectangle size */
  uint16      dpi;                                  /* Pixels per inch        */
  char       directory[FS_FILENAME_MAX_LENGTH_P];  /* Current directory of svg file */
  uint32      delay;                                /* delay between frames   */
  boolean     transparency:1;                         /* Transparency feature   */
  boolean     empty:1;                                /* Empty document         */
  cmx_svg_decode_mode_enum_type   cmx_svg_decode_mode:3;
  cmx_color_scheme_enum_type   color_scheme:3;  /* Color scheme used           */
  uint32      bk_color;                             /* Background color       */
  boolean     progressive;                          /* Progressive Rendering, allowed only for Client Frame generation */
  boolean     auto_stop;                            /* If true, decoder will stop after generating last frame */
} cmx_svg_decode_data_type;

/* Event types supported by SVG */
typedef enum{
  CMX_SVG_KEY_EVENT,                                /* Key Pressed            */
  CMX_SVG_KEY_DOWN_EVENT,                           /* Key Down               */
  CMX_SVG_KEY_UP_EVENT,                             /* Key Up                 */
  CMX_SVG_FOCUS_EVENT,                              /* Text Input gets Focus  */
  CMX_SVG_POINTER_CLICK_EVENT,                      /* Mouse Click            */
  CMX_SVG_POINTER_DOWN_EVENT,                       /* Mouse Down             */
  CMX_SVG_POINTER_MOVE_EVENT,                       /* Mouse Move             */
  CMX_SVG_POINTER_OVER_EVENT,                       /* Mouse Over             */
  CMX_SVG_POINTER_UP_EVENT,                         /* Mouse Up               */
  CMX_SVG_POINTER_LEAVE_EVENT,                      /* Mouse Leave            */
  CMX_SVG_ROTATE_EVENT,                             /* Rotate Event           */
  CMX_SVG_PAN_EVENT,                                /* Pan Event              */
  CMX_SVG_ZOOM_EVENT,                               /* Zoom Event             */
}cmx_svg_event_enum_type;

/* Virtual Keys supported by SVG */
typedef enum{
  CMX_SVG_VK_UNDEFINED,
  CMX_SVG_VK_RIGHT_ALT,
  CMX_SVG_VK_LEFT_ALT ,
  CMX_SVG_VK_LEFT_CONTROL,
  CMX_SVG_VK_RIGHT_CONTROL,
  CMX_SVG_VK_LEFT_SHIFT,
  CMX_SVG_VK_RIGHT_SHIFT,
  CMX_SVG_VK_LEFT_META,
  CMX_SVG_VK_RIGHT_META,
  CMX_SVG_VK_CAPS_LOCK,
  CMX_SVG_VK_DELETE,
  CMX_SVG_VK_END,
  CMX_SVG_VK_ENTER,
  CMX_SVG_VK_ESCAPE,
  CMX_SVG_VK_HOME,
  CMX_SVG_VK_INSERT,
  CMX_SVG_VK_NUM_LOCK,
  CMX_SVG_VK_PAUSE,
  CMX_SVG_VK_PRINTSCREEN,
  CMX_SVG_VK_SCROLL_LOCK,
  CMX_SVG_VK_LEFT,
  CMX_SVG_VK_RIGHT,
  CMX_SVG_VK_UP,
  CMX_SVG_VK_DOWN,
  CMX_SVG_VK_PAGE_DOWN,
  CMX_SVG_VK_PAGE_UP,
  CMX_SVG_VK_F1,
  CMX_SVG_VK_F2,
  CMX_SVG_VK_F3,
  CMX_SVG_VK_F4,
  CMX_SVG_VK_F5,
  CMX_SVG_VK_F6,
  CMX_SVG_VK_F7,
  CMX_SVG_VK_F8,
  CMX_SVG_VK_F9,
  CMX_SVG_VK_F10,
  CMX_SVG_VK_F11,
  CMX_SVG_VK_F12,
  CMX_SVG_VK_F13,
  CMX_SVG_VK_F14,
  CMX_SVG_VK_F15,
  CMX_SVG_VK_F16,
  CMX_SVG_VK_F17,
  CMX_SVG_VK_F18,
  CMX_SVG_VK_F19,
  CMX_SVG_VK_F20,
  CMX_SVG_VK_F21,
  CMX_SVG_VK_F22,
  CMX_SVG_VK_F23,
  CMX_SVG_VK_F24
}cmx_svg_virtual_key_enum_type;

/* Key Modifiers supported by SVG */
typedef enum{
  CMX_SVG_KEY__MODIFIER_NONE = 0x00,
  CMX_SVG_KEY__MODIFIER_RIGHT_ALT = 0x01,
  CMX_SVG_KEY__MODIFIER_LEFT_ALT = 0x02,
  CMX_SVG_KEY__MODIFIER_LEFT_CONTROL =  0x04,
  CMX_SVG_KEY__MODIFIER_RIGHT_CONTROL = 0x08,
  CMX_SVG_KEY__MODIFIER_LEFT_SHIFT = 0x10,
  CMX_SVG_KEY__MODIFIER_RIGHT_SHIFT = 0x20,
  CMX_SVG_KEY__MODIFIER_LEFT_META = 0x40,
  CMX_SVG_KEY__MODIFIER_RIGHT_META = 0x80
} cmx_svg_key_modifier_enum_type;

/* This struct is used for passing key events */
typedef struct{
  uint16                          key;          /* ASCII code for Key Pressed */
  cmx_svg_virtual_key_enum_type   virtual_key;  /* Virtual Key Pressed        */
  cmx_svg_key_modifier_enum_type  modifier;     /* Key Modifiers used         */
} cmx_svg_key_event_type;

/* Text events supported by SVG */
typedef enum{
  CMX_SVG_FOCUS_EVENT_NONE,     /* No Event                                   */
  CMX_SVG_FOCUS_EVENT_BEGIN,    /* Sets Focus to first element                */
  CMX_SVG_FOCUS_EVENT_NEXT,     /* Sets Focus to next element                 */
  CMX_SVG_FOCUS_EVENT_PREVIOUS, /* Sets Focus to previous element             */
  CMX_SVG_FOCUS_EVENT_ACTIVATE, /* Activates Focus to the element with focus  */
  CMX_SVG_FOCUS_EVENT_END,       /* Unsets focus to the frame                  */
  CMX_SVG_FOCUS_EVENT_NORTH,         /* Move to Focusable element at North */
  CMX_SVG_FOCUS_EVENT_NORTH_EAST,    /* Move to Focusable element at North-East */
  CMX_SVG_FOCUS_EVENT_EAST,          /* Move to Focusable element at East */
  CMX_SVG_FOCUS_EVENT_SOUTH_EAST,     /* Move to Focusable element at South-East */
  CMX_SVG_FOCUS_EVENT_SOUTH,          /* Move to Focusable element at South */
  CMX_SVG_FOCUS_EVENT_SOUTH_WEST,     /* Move to Focusable element at South-West */
  CMX_SVG_FOCUS_EVENT_WEST,           /* Move to Focusable element at West */
  CMX_SVG_FOCUS_EVENT_NORTH_WEST      /* Move to Focusable element at North-West */
} cmx_svg_focus_event_enum_type;

/* This struct is used for passing Pointer events to SVG */
typedef  struct{
  int16 x;                                          /* x-coordinate           */
  int16 y;                                          /* y-coordinate           */
  cmx_svg_key_modifier_enum_type  key_modifier;     /* Key Modifier           */
} cmx_svg_pointer_event_type;

/* This struct is used for passing Rotate Events */
typedef struct{
  uint32 cx;                           /* x-coordinate of Center of rotation */
  uint32 cy;                           /* y-coordinate of Center of rotation */
  int16   angle;                       /* angle of rotation */
} cmx_svg_rotate_event_type;

/* This struct is used for passing Zoom events to SVG */
typedef struct{
  int16 x;                                  /* x-coordinate of zoom position */
  int16 y;                                  /* y-coordinate of zoom position */
  uint32  zoom_factor;                      /* zoom factor in 16.16 fixed pt */
  boolean centered;                         /* output is centered            */
}cmx_svg_zoom_event_type;

/* This struct is used for passing Pan events to SVG */
typedef struct{
  int16 x, y;                               /* x & y coordinates for panning */
  boolean fast_scroll;                      /* fast scroll, recommended      */
}cmx_svg_pan_event_type;

/* This struct is used for SVG event processing */
typedef union{
  cmx_svg_key_event_type key;
  cmx_svg_focus_event_enum_type focus;
  cmx_svg_pointer_event_type ptr;
  cmx_svg_rotate_event_type rt;
  cmx_svg_zoom_event_type   zoom;
  cmx_svg_pan_event_type    pan;
} cmx_svg_event_type;

/* This struct is used to pass SVG specification back to the client */
typedef struct{
  boolean          animated;                  /* true=Animation otherwise image */
  boolean          zoom_pan_events;           /* Zoom and Pan events supported  */
  boolean          text_only;                 /* Text-only mode supported       */
  char             title[128];                /* Title defined in content    */
  cmx_color_scheme_enum_type   color_scheme;  /* Color scheme used           */
  boolean          transparency;              /* transparency feature        */
  void*            handle;                    /* CMX handle                  */
  uint32           width;                     /* Default viewport width      */
  uint32           height;                    /* Default Viewport height     */
  cmx_svgdom_document_type document;          /* DOM Document handle         */
  boolean          bk_seek;
  uint32           num_requests;
}cmx_svg_info_type;

/* This struct is used to pass frames back to client */
typedef struct{
  uint8*           frame_buf;                 /* Frame buffer                */
  uint32           frame_bufsize;             /* Size of frame buffer        */
  uint8*           bitmask_buf;                 /* bitmask buffer                */
  uint32           bitmask_bufsize;             /* Size of bitmask buffer        */
  uint32           time_ms;                   /* Time of frame in ms         */
  boolean          last_flag;                 /* treu = last frame           */
  cmx_svg_frame_cb_func_ptr frame_done_cb;    /* Callback for telling CMX that frame is consumed */
  void*            frame_done_data;           /* Data for Frame Done callback */
  cmx_af_pos_struct_type frame_pos; /* Positioning of animation frame    */
}cmx_svg_frame_done_type;

/* This struct is used for passing Event information back to client */
typedef struct{
  boolean                 handled;            /* true = Event handled        */
  cmx_svg_event_enum_type event;              /* Event type                  */
}cmx_svg_event_done_type;

/* This struct is used with cb for external request processing */
typedef struct{
  char*            filename;                 /* URL name                     */
  uint32           num_bytes;                /* Number of bytes in URL name  */
  cmx_svg_request_cb_func_ptr cb_func;       /* Callback function                  */
  void*            cb_data;                  /* Callback data                      */
  boolean          more_requests;            /* true = more requests pendin */
}cmx_svg_request_type;

/* This struct is used to indicate if external request processed correctly */
typedef struct{
  char         filename[FS_FILENAME_MAX_LENGTH_P];  /* name of external file */
  boolean      loaded;
}cmx_svg_request_free_type;

#endif /*FEATURE_SVG */

/* This type is a union of all the structures that are returned via the
** audio format play callback function.
*/
typedef union {
  uint32                      elapsed_time_ms;
  uint32                      tempo;
  uint32                      tune;
  uint32                      pan;
  cmx_af_audio_spec_type      audio_spec;
  cmx_abort_status_enum_type  abort_status; /* More information about the CMX_ABORTED sent back to the client */
#ifdef FEATURE_WEBAUDIO
  uint8                       *client_buf_free;
  cmx_client_buf_req_type     client_buf_req;
  cmx_af_picture_status_type  pic_status;
  cmx_af_pic_data_type        pic_data;
  cmx_af_pic_info_type        pic_info;
  cmx_af_text_ctl_type        text_ctl;
  cmx_af_text_info_type       text_info;
  cmx_af_anim_info_type       anim_info;
#ifdef FEATURE_SAF
  cmx_saf_info_type           saf_info;
  cmx_saf_data_type           saf_data;
#endif /* FEATURE_SAF */
#ifdef FEATURE_SVG
  cmx_svg_info_type           svg_info;
  cmx_svg_frame_done_type     svg_frame;
#endif /* FEATURE_SVG */
#ifdef FEATURE_SMAF
  cmx_smaf_graph_info_type    smaf_graph_info;
  cmx_smaf_led_ctl_status_enum_type smaf_led_ctl_status;
  cmx_smaf_vib_ctl_status_enum_type smaf_vib_ctl_status;
#endif /* FEATURE_SMAF */
  cmx_mfi_led_ctl_type        mfi_led_ctl;
#endif /* FEATURE_WEBAUDIO */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
  cmx_af_led_ctl_enum_type    led_ctl;
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
  boolean                     backlight_ctl;
#endif /* FEATURE_IMELODY */
} cmx_af_server_data_type;

/* CMX audio playback callback function type */
typedef void (*cmx_audfmt_play_cb_func_ptr_type) (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data,  /* Client data passed in to the command  */
  cmx_af_server_data_type
                     *server_data   /* Server data, structure of data is     */
                                    /* determined by the status parameter    */
);

/* CMX get total playback time callback function type */
typedef void (*cmx_audfmt_time_cb_func_ptr_type) (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data,  /* Client data passed in to the command  */
  uint32             time_ms        /* Total play time of requested CMX file */
);


/* CMX get audio spec callback function type */
typedef void (*cmx_audfmt_spec_cb_func_ptr_type) (
  cmx_status_type        status,       /* Status of event                     */
  const void             *client_data, /* Client data passed in to the command*/
  cmx_af_audio_spec_type *audio_spec   /* Audio spec of requested file        */
);

#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
typedef  struct {
  cmx_af_sbr_mode_enum_type mode;
} cmx_audio_pb_sbr_mode_cmd_type;

typedef  struct {
  cmx_af_dual_mono_mode_enum_type  chan_mode;
} cmx_audio_pb_dm_mode_cmd_type;

typedef union {
  cmx_audio_pb_dm_mode_cmd_type     dm_mode_cmd;  /* Parameter for dual-mono mode command */
  cmx_audio_pb_sbr_mode_cmd_type    sbr_mode_cmd; /* Parameter for sbr enable command */
} cmx_audio_playback_cmd_param_type;
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* Commands issued by the client during audio playback */
typedef enum {
  CMX_AUDIO_PB_CMD_STOP,            /* Stop command           */
  CMX_AUDIO_PB_CMD_PAUSE,           /* Pause command          */
  CMX_AUDIO_PB_CMD_RESUME,          /* Resume command         */
  CMX_AUDIO_PB_CMD_FLUSH,           /* Data flush command     */
#ifdef FEATURE_AUDIO_PCR_SUPPORT
  CMX_AUDIO_PB_CMD_ADD_SAMPLE,		/* Add one sample		 */
  CMX_AUDIO_PB_CMD_DROP_SAMPLE, 	/* Drop one sample		 */
#endif /* FEATURE_AUDIO_PCR_SUPPORT */
  CMX_AUDIO_PB_CMD_INVALID,         /* Invalid command        */
  CMX_AUDIO_PB_CMD_SBR_MODE,        /* SBR Mode command       */
  CMX_AUDIO_PB_CMD_DUAL_MONO_MODE   /* Dual-Mono mode command */  
} cmx_audio_playback_cmd_enum_type;

#ifdef FEATURE_QCP
/* This enumerated type is used to indicate the mode for QCP recording.
*/
typedef enum {
  CMX_QCP_FORMAT_FIXED_FULL_13K, /* Fixed-full rate, 13K vocoder       */
  CMX_QCP_FORMAT_VAR_FULL_13K,   /* Variable rate, full rate max, 13K  */
  CMX_QCP_FORMAT_FIXED_FULL_EVRC,/* Fixed-full rate, EVRC vocoder      */
  CMX_QCP_FORMAT_VAR_FULL_EVRC,  /* Variable rate, full rate max, EVRC */
  CMX_QCP_FORMAT_EVRC = CMX_QCP_FORMAT_VAR_FULL_EVRC,
                                 /* To keep code backwards compatible  */
  CMX_QCP_FORMAT_FIXED_HALF_13K, /* Fixed half rate, 13K vocoder       */
  CMX_QCP_FORMAT_VAR_HALF_13K,   /* Varibale rate, half rate max, 13K  */

  CMX_QCP_FORMAT_VAR_FULL_4GV_NB,   /* Variable rate, full rate max, 4GV  */
  CMX_QCP_FORMAT_FIXED_FULL_4GV_NB, /* Fixed rate, full rate max, 4GV     */

  CMX_QCP_FORMAT_VAR_FULL_4GV_WB,   /* Variable rate, full rate max, 4GV  */
  CMX_QCP_FORMAT_FIXED_FULL_4GV_WB, /* Fixed rate, full rate max, 4GV     */

  CMX_QCP_FORMAT_FIXED_4_75_AMR, /* Fixed- 4.75 kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_5_15_AMR, /* Fixed- 5.15 kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_5_9_AMR,  /* Fixed- 5.9  kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_6_7_AMR,  /* Fixed- 6.7  kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_7_4_AMR,  /* Fixed- 7.4  kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_7_95_AMR, /* Fixed- 7.95 kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_10_2_AMR, /* Fixed-10.2  kbit/s, AMR            */
  CMX_QCP_FORMAT_FIXED_12_2_AMR, /* Fixed-12.2  kbit/s, AMR            */

  CMX_QCP_FORMAT_FIXED_4_75_AMR_DTX, /* Fixed- 4.75 kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_5_15_AMR_DTX, /* Fixed- 5.15 kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_5_9_AMR_DTX,  /* Fixed- 5.9  kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_6_7_AMR_DTX,  /* Fixed- 6.7  kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_7_4_AMR_DTX,  /* Fixed- 7.4  kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_7_95_AMR_DTX, /* Fixed- 7.95 kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_10_2_AMR_DTX, /* Fixed-10.2  kbit/s, AMR w DTX enabled */
  CMX_QCP_FORMAT_FIXED_12_2_AMR_DTX, /* Fixed-12.2  kbit/s, AMR w DTX enabled */

#ifdef FEATURE_AUDFMT_AMR
  CMX_AMR_FORMAT_FIXED_4_75_AMR, /* Fixed- 4.75 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_5_15_AMR, /* Fixed- 5.15 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_5_9_AMR,  /* Fixed- 5.9  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_6_7_AMR,  /* Fixed- 6.7  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_7_4_AMR,  /* Fixed- 7.4  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_7_95_AMR, /* Fixed- 7.95 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_10_2_AMR, /* Fixed-10.2  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_12_2_AMR, /* Fixed-12.2  kbit/s, AMR Codec, AMR */

  CMX_AMR_FORMAT_FIXED_4_75_AMR_DTX, /* Fixed- 4.75 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_5_15_AMR_DTX, /* Fixed- 5.15 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_5_9_AMR_DTX,  /* Fixed- 5.9  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_6_7_AMR_DTX,  /* Fixed- 6.7  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_7_4_AMR_DTX,  /* Fixed- 7.4  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_7_95_AMR_DTX, /* Fixed- 7.95 kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_10_2_AMR_DTX, /* Fixed-10.2  kbit/s, AMR Codec, AMR */
  CMX_AMR_FORMAT_FIXED_12_2_AMR_DTX, /* Fixed-12.2  kbit/s, AMR Codec, AMR */
#endif /* FEATURE_AUDFMT_AMR */

  CMX_QCP_FORMAT_FIXED_FULL_EFR, /* Fixed full rate EFR                */
  CMX_QCP_FORMAT_FIXED_FULL_FR,  /* Fixed full rate FR                 */
  CMX_QCP_FORMAT_FIXED_FULL_HR,  /* Fixed full rate HR                 */

#ifdef FEATURE_AUDFMT_EVB
  CMX_EVB_FORMAT_VAR_FULL_4GV_NB,   /* .EVB format, variable rate, full rate max, 4GV */
#endif /* FEATURE_AUDFMT_EVB */

#ifdef FEATURE_AUDFMT_EVW
  CMX_EVW_FORMAT_VAR_FULL_4GV_WB,
#endif /* FEATURE_AUDFMT_EVW */
  CMX_QCP_FORMAT_INVALID
} cmx_qcp_format_type;

/*
 * This enumerated type is used to indicate the file format to be recorded
 * for Multi Media recording.
 */
typedef enum {
  CMX_MM_FORMAT_AAC,               /* AAC format, MultiMedia */
  CMX_MM_FORMAT_QCP,               /* AMR format, MultiMedia */
  CMX_MM_FORMAT_INVALID            /* Sanity check           */
} cmx_mm_rec_format_type;

/*
 * This type is used to indicate the direction for MultiMedia recording
 */
typedef enum {
  CMX_MM_REC_LINK_REVERSE,   /* Record reverse link */
  CMX_MM_REC_LINK_FORWARD,   /* Record forward link */
  CMX_MM_REC_LINK_BOTH,   /* Record forward link */
  CMX_MM_REC_LINK_INVALID
} cmx_mm_rec_link_type;

#ifdef FEATURE_MM_REC
/*
 * Data structure for A/V sync report information.
 */
typedef struct {
  uint32 num_samples;         /* AV sync report, total samples just encoded   */
  uint32 time_stamp[2];       /* Time stamp at this moment                    */
  uint32 lost_samples;        /* Lost samples if any                          */
  uint32 frame_index;         /* Frame count/index for current frame          */
} cmx_av_report_type;

/* The following is the callback type used for sound server to client
 * A/V sync for Multi Media recording.
 */
typedef void (* cmx_mm_rec_av_sync_cb_ptr) (
  cmx_av_report_type *av_report,
  const void         *client_data
);

/*
 * Recording mode
 */
typedef enum {
  CMX_MM_REC_OPERATION_NORMAL,
  CMX_MM_REC_OPERATION_MAX
} cmx_mm_rec_operation_mode_type;

#ifdef FEATURE_AAC_REC
/*
 * Structure specific for AAC recording parameters
 */
typedef struct {
  cmx_mm_rec_format_type mm_rec_format;      /* MM recording format type      */

  cmx_af_aac_data_format_enum_type
                           aac_file_format;  /* MM recording file format      */
  cmx_af_sample_rate_enum_type sample_rate;  /* MM recording input sample rate*/
  cmx_mm_rec_av_sync_cb_ptr    sync_cb_func; /* A/V sync callback function    */
  cmx_af_aac_channel_enum_type chan;         /* Mono or Stereo                */
  uint16                       bit_per_spl;  /* Bit per sample/channel        */
  uint32                       sync_ms;      /* Time(ms) to do A/V sync report*/
} cmx_mm_rec_aac_para_type;
#endif /* FEATURE_AAC_REC */

#endif /* FEATURE_MM_REC */

/* Structures for QCP recording configuration parameters */
typedef struct {
  cmx_mm_rec_format_type    mm_rec_format; /* MultiMedia record format       */
  cmx_qcp_format_type       format;        /* QCP file format                */
  cmx_mm_rec_link_type      rec_path;      /* QCP recorded direction         */
  uint32                    report_ms;     /* Time callback interval in ms   */
  uint32                    auto_stop_ms;  /* Silence time to end recording  */
#ifdef FEATURE_MM_REC
  cmx_mm_rec_av_sync_cb_ptr sync_cb_func;  /* A/V sync callback function     */
#endif /* FEATURE_MM_REC */
  uint32                    data_req_ms; /* Data request time interval in ms */
} cmx_mm_rec_qcp_para_type;

/* Structures for QCP recording configuration parameters */
typedef struct {
  cmx_qcp_format_type     format;        /* QCP file format                  */
  uint32                  report_ms;     /* Time callback interval in ms     */
  uint32                  auto_stop_ms;  /* Silence time to end recording    */
} cmx_qcp_rec_para_type;


/*
 * Union definition for different MultiMedia recording parameters
 */
typedef union {
  cmx_mm_rec_format_type   mm_rec_format;       /* MM recording format type   */
#ifdef FEATURE_AAC_REC
  cmx_mm_rec_aac_para_type cmx_mm_rec_aac_para; /* AAC recording parameters   */
#endif /* FEATURE_AAC_REC */
#ifdef FEATURE_QCP
  cmx_mm_rec_qcp_para_type cmx_mm_rec_qcp_para; /* AMR recording parameters  */
#endif /* FEATURE_QCP */
} cmx_mm_rec_aud_type;

#ifdef FEATURE_MM_REC
/*
 * Structures for general MultiMedia recording configuration parameters
 */
typedef struct {
  cmx_mm_rec_link_type   mm_rec_link;        /* Direction to record           */
  cmx_mm_rec_aud_type    mm_rec_param;       /* MM recording parameters       */
} cmx_mm_rec_param_type;
#endif /* FEATURE_MM_REC */

/* QCP codec type, no additional information is necessary to configure
** QCP codec.
*/
typedef cmx_af_codec_type cmx_qcp_codec_type;

#endif /* FEATURE_QCP */

/* Union type of supported audio codecs where format is specified by the
** client.
*/
typedef union {
  cmx_af_codec_type          codec_type;   /* Generic codec information      */
#ifdef FEATURE_MP3
  cmx_mp3_codec_type         mp3_codec;    /* MP3 codec                      */
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
  cmx_aac_codec_type         aac_codec;    /* AAC codec                      */
#endif /* FEATURE_AAC */
#ifdef FEATURE_QCP
  cmx_qcp_codec_type         qcp_codec;    /* QCP codec                      */
#endif /* FEATURE_QCP */
#ifdef FEATURE_REAL_AUDIO
  cmx_ra_codec_type          ra_codec;     /* RealAudio codec                */
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_PCM
  cmx_af_wave_codec_type     wave_codec;   /* PCM codec information          */
#endif /* FEATURE_PCM */
#ifdef FEATURE_WMA
  cmx_af_wma_codec_type      wma_codec;   /* Windows Media codec information */
#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  cmx_af_ext_speech_codec_type ext_speech_codec; /* Extended speech codec    */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
} cmx_codec_spec_type;

typedef union {
  cmx_af_codec_type         codec_type;   /* Generic codec information      */
#ifdef FEATURE_AAC
  cmx_af_aac_bc_codec_type  aac_bc_codec; /* AAC Brodcast codec information */
#endif /* FEATURE_AAC */
} cmx_codec_spec_ex_type;


#ifdef FEATURE_PCM_REC
/* This type is used to indicate the direction for PCM recording
*/
typedef enum {
  CMX_PCM_REC_LINK_REVERSE,   /* Record reverse link */
  CMX_PCM_REC_LINK_FORWARD,   /* Record forward link */
  CMX_PCM_REC_LINK_BOTH,   /* Record forward link */
  CMX_PCM_REC_LINK_INVALID
} cmx_pcm_rec_link_type;

/* This structure is used to indicate all the parameters for recording PCM
*/
typedef struct {
  cmx_pcm_rec_link_type link;  /* Direction to record */
} cmx_pcm_rec_para_type;
#endif /* FEATURE_PCM_REC */

#endif /* FEATURE_AUDIO_FORMAT */

#ifdef FEATURE_CMX_IMAGE
/* CMX PNG/JPEG/SAF callback function type */
typedef void (*cmx_image_cb_func_ptr_type) (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data,  /* Client data passed in to the command  */
  void               *data,         /* Decoded data                          */
  uint32             num_bytes      /* Number of bytes in data buffer        */
);
#endif /* FEATURE_CMX_IMAGE */

#ifdef FEATURE_PNG_DECODER
/* Structure of PNG image specifications */
typedef struct
{
  uint32 width;         /* Width of uncompressed image           */
  uint32 height;        /* Height of uncompressed image          */
  uint32 bit_depth;     /* Number of bits per sample             */
  uint32 color_type;    /* Color type code                       */
  uint32 compression;   /* Method used to compress image data    */
  uint32 filter;        /* Filter used prior to compressing data */
  uint32 interlace;     /* Transmission order of image data      */
} cmx_png_spec_type;
#endif /* FEATURE_PNG_DECODER */

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)

typedef enum
{
  JPEG_GRAY,
  JPEG_RGB,
  JPEG_YUV
} cmx_jpeg_color_type;

typedef enum
{
  JPEG_LOW_QUALITY = -5,
  JPEG_MEDIUM_QUALITY = 0,
  JPEG_HIGH_QUALITY = 5
} cmx_jpeg_quality_type;

typedef enum
{
  GRAY = 1,
  H1V1,
  H2V1,
  H1V2,
  H2V2
} cmx_jpeg_sample_type;

/* Structure of JPEG image specifications */
typedef struct
{
  uint32 width;                       /* Width of uncompressed image       */
  uint32 height;                      /* Height of uncompressed image      */
  uint32 num_components;              /* Samples per pixel                 */
  cmx_jpeg_color_type  color_space;   /* color space: YUV, RGB, Grayscale  */
  cmx_jpeg_sample_type sample_factor; /* downsampling factor               */
  cmx_jpeg_quality_type  quality;     /* one of three quality types        */
} cmx_jpeg_spec_type;

#endif /* FEATURE_JPEG_DECODER || FEATURE_JPEG_ENCODER */

#ifdef FEATURE_MIDI_OUT_QCP
/* This is used as the callback function type for MIDI out QCP function calls.
*/
typedef void (*cmx_midi_out_cb_func_ptr_type) (
  cmx_status_type    status,           /* Status of event                    */
  const void         *midi_out_qcp_id, /* ID of command for client reference */
  const void         *client_data      /* Client data passed in to the cmd   */
);

/* This type is used to indicate the start mode for ADEC playback.
*/
typedef enum {
  CMX_ADEC_START_MODE_NORMAL,  /* Normal start - start playback immediately  */
  CMX_ADEC_START_MODE_DELAYED, /* Delayed start - load file and wait for cmd */
  CMX_ADEC_START_MODE_INVALID
} cmx_adec_start_mode_enum_type;

/* This type is used to indicate the repeat mode for ADEC playback.
*/
typedef enum {
  CMX_ADEC_REPEAT_MODE_DISABLED, /* Repeat mode disabled - play file once   */
  CMX_ADEC_REPEAT_MODE_ENABLED,  /* Repeat mode enabled - repeat file the   */
                                 /* indicated number of times               */
  CMX_ADEC_REPEAT_MODE_INVALID
} cmx_adec_repeat_mode_enum_type;

/* This structure is used to indicate the playback parameters for ADEC
*/
typedef struct {
  uint16                         volume;      /* Volume of adec playback      */
  sint15                         pan;         /* Panning of adec playback     */
  cmx_adec_start_mode_enum_type  start_mode;  /* Start mode for playback      */
  cmx_adec_repeat_mode_enum_type repeat_mode; /* Repeat mode for playback     */
  uint32                         repeat_num;  /* Number of iterations for     */
                                              /* repeat: 0 = rpt indefinitely */
} cmx_midi_out_adec_param_type;
#endif /* FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_MIDI_OUT
/* Type specifications for QSynth audio decoder
*/
typedef enum {
  CMX_ADEC_MODE_DEFAULT,               /* Default type of QSynth ADEC      */
  CMX_ADEC_MODE_ADPCM,                 /* QSynth ADPCM audio decoder       */
  CMX_ADEC_MODE_QCP,                   /* QSynth QCELP audio decoder       */
  CMX_ADEC_MODE_PCM                    /* QSynth PCM audio decoder         */
} cmx_adec_mode_enum_type;
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/* Audio 3D positional processing commands. */
typedef enum {
  CMX_3D_POS_CMD_ENABLE,              /* Enable 3D positional environment */
  CMX_3D_POS_CMD_RESET,               /* Restes all 3D positional environment */
  CMX_3D_POS_CMD_SET_LSNR_POS,        /* Set listener position command    */
  CMX_3D_POS_CMD_SET_LSNR_ORIENT,     /* Set listener orientation command */
  CMX_3D_POS_CMD_SET_LSNR_VEL,        /* Set listener velocity command    */
  /* To enable reverb correctly, caller needs to send reverb preset command
     first before sending reverb enable command. There is no restriction to
     setting reverb level, reverb time, reverb ratio. However, it is not
     advised to set individual reverb parameter before preset command is
     is sent */
  CMX_3D_POS_CMD_ENABLE_REVERB,       /* Enables and set Reverb command   */
  CMX_3D_POS_CMD_DISABLE_REVERB,      /* Disable Reverb command           */
  CMX_3D_POS_CMD_SET_REVERB_PRESET,   /* Set Reverb presets               */
  CMX_3D_POS_CMD_SET_REVERB_LEVEL,    /* Set Reverb gain for the room     */
  CMX_3D_POS_CMD_SET_REVERB_TIME,     /* Set Reverb Decay Time at low freq*/
  CMX_3D_POS_CMD_SET_REVERB_RATIO,    /* Set Reverb Decay Ratio between high
                                         and low frequencies              */
  CMX_3D_POS_CMD_SRC_ENABLE,          /* Enable source positional effects */
  CMX_3D_POS_CMD_SRC_RESET,           /* Reset source positional parameters */
  CMX_3D_POS_CMD_SET_SRC_POS,         /* Set source position command      */
  CMX_3D_POS_CMD_SET_SRC_VOL,         /* Set source volume command        */
  CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN,   /* Set source volume attenuation    */
  CMX_3D_POS_CMD_SET_SRC_VEL,         /* Set source velocity command        */
  CMX_3D_POS_CMD_SET_SRC_REVERB_VOL,  /* Set source reverb volume command  */
  CMX_3D_POS_CMD_INVALID
} cmx_3d_pos_cmd_enum_type;

/* This is used to enable/disable 3D positional mode */
typedef enum {
  CMX_3D_POS_MODE_DISABLE,   /* Disable 3D positional mode */
  CMX_3D_POS_MODE_ENABLE,    /* Enable 3D positional mode  */
  CMX_3D_POS_MODE_INVALID
} cmx_3d_pos_mode_enum_type;

/* These parameters are set for the CMX_3D_POS_CMD_ENABLE command */
typedef struct {
  cmx_3d_pos_mode_enum_type  mode;  /* Mode         */
} cmx_3d_pos_enable_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_LSNR_POS command */
typedef struct {
  sint31 x_pos;         /* X position of listener                           */
  sint31 y_pos;         /* Y position of listener                           */
  sint31 z_pos;         /* Z position of listener                           */
  uint32 duration_ms;   /* Duration in ms to move to new location           */
} cmx_3d_pos_lsnr_pos_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_LSNR_ORIENT command */
typedef struct {
  sint31 x_forward;     /* X coordinate of forward vector                   */
  sint31 y_forward;     /* Y coordinate of forward vector                   */
  sint31 z_forward;     /* Z coordinate of forward vector                   */
  sint31 x_up;          /* X coordinate of up vector                        */
  sint31 y_up;          /* Y coordinate of up vector                        */
  sint31 z_up;          /* Z coordinate of up vector                        */
  uint32 duration_ms;   /* Duration in ms to move to new location           */
} cmx_3d_pos_lsnr_orient_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_LSNR_VEL command */
typedef struct {
  sint31 x_vel;         /* X velocity of listener                           */
  sint31 y_vel;         /* Y velocity of listener                           */
  sint31 z_vel;         /* Z velocity of listener                           */
  uint32 duration_ms;   /* Duration in ms to change velocity                */
} cmx_3d_pos_lsnr_vel_cmd_type;

typedef enum{
  CMX_3D_REVERB_GENERIC,                    /* Generic                       */
  CMX_3D_REVERB_PADDEDCELL,                 /* Padded Cell                   */
  CMX_3D_REVERB_ROOM,                       /* Room                          */
  CMX_3D_REVERB_BATHROOM,                   /* Bath Room                     */
  CMX_3D_REVERB_LIVINGROOM,                 /* Living Room                   */
  CMX_3D_REVERB_ARENA,                      /* Arena                         */
  CMX_3D_REVERB_AUDITORIUM,                 /* Auditorium                    */
  CMX_3D_REVERB_CAVE,                       /* Cave                          */
  CMX_3D_REVERB_CONCERTHALL,                /* Concert Hall                  */
  CMX_3D_REVERB_STONEROOM,                  /* Stone Room                    */
  CMX_3D_REVERB_HANGAR,                     /* Hangar                        */
  CMX_3D_REVERB_HALLWAY,                    /* Hallway                       */
  CMX_3D_REVERB_ALLEY,                      /* Alley                         */
  CMX_3D_REVERB_CARPETED_HALLWAY,           /* Carpeted Hall                 */
  CMX_3D_REVERB_STONE_CORRIDOR,             /* Stone Corridor                */
  CMX_3D_REVERB_FOREST,                     /* Forest                       */
  CMX_3D_REVERB_MOUNTAIN,                   /* Mountain                      */
  CMX_3D_REVERB_CITY,                       /* City                          */
  CMX_3D_REVERB_QUARRY,                     /* Quarry                        */
  CMX_3D_REVERB_PLAIN,                      /* Plain                         */
  CMX_3D_REVERB_DRUG,                       /* Drug                          */
  CMX_3D_REVERB_PARKINGLOT,                 /* Parking Lot                   */
  CMX_3D_REVERB_DIZZY,                      /* Dizzy                         */
  CMX_3D_REVERB_SEWERPIPE,                  /* Sewer Pipe                    */
  CMX_3D_REVERB_UNDERWATER,                 /* Under Water                   */
  CMX_3D_REVERB_SMALLROOM,                  /* Small Room                    */
  CMX_3D_REVERB_MEDIUMROOM,                 /* Medium Room                   */
  CMX_3D_REVERB_LARGEROOM,                  /* Large Room                    */
  CMX_3D_REVERB_PSYCHOTIC,                  /* Psychotic                     */
  CMX_3D_REVERB_PLATE,                      /* Plater                        */
  CMX_3D_REVERB_MEDIUMHALL,                 /* Medium Hall                   */
  CMX_3D_REVERB_LARGEHALL,                  /* Large Hall                    */
  CMX_3D_REVERB_USER,                       /* User configurable Environment */
  CMX_3D_REVERB_INVALID
} cmx_3d_reverb_enum_type;

/* This structure describes the 3D Reverb parameters */
/* This structure is valid only when env_type is CMX_3D_REVERB_USER */
typedef struct {
  cmx_3d_reverb_enum_type   env_type;
  sint31   room_gain;         /* Room Gain range between -9600 to 0           */
  uint32   decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32   decay_ratio;       /* Decay time ratio btwn high and low frequencies
                                 value greater than 65535 will cause overflow*/
  uint16   ref_delay;         /* Initial reflection delay time in ms          */
  sint31   reverb_gain;       /* Reverb gain relative to room effect -9600 to 0    */
  uint16   reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} cmx_3d_reverb_param_type;

/* These parameters are set for the CMX_3D_POS_CMD_ENABLE_REVERB command */
typedef union{
  cmx_3d_reverb_enum_type   env_type;     /* Environment type */
  cmx_3d_reverb_param_type  reverb_param; /* Reverb parameters valid only for
                                                 CMX_3D_REVERB_USER Environment */
} cmx_3d_reverb_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_REVERB_LEVEL command */
typedef struct{
  sint31   reverb_gain;       /* Reverb gain relative to room effect          */
} cmx_3d_reverb_level_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_REVERB_TIME command */
typedef struct{
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
} cmx_3d_reverb_decay_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_REVERB_RATIO command */
typedef struct{
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
} cmx_3d_reverb_ratio_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_SRC_POS command */
typedef struct {
  void   *id;           /* ID of source                                     */
  sint31 x_pos;         /* X position of source                             */
  sint31 y_pos;         /* Y position of source                             */
  sint31 z_pos;         /* Z position of source                             */
  uint32 duration_ms;   /* Duration in ms to move to new location           */
} cmx_3d_pos_src_pos_cmd_type;

typedef struct {
  void                           *id;         /* ID of source              */
  cmx_3d_pos_mode_enum_type      enable_mode; /* Enable mode               */
} cmx_3d_pos_src_enable_cmd_type;

typedef struct {
  void                           *id;         /* ID of source              */
} cmx_3d_pos_src_reset_cmd_type;

/* The following structure is used to set the volume of source object. This
** is used with the CMX_3D_POS_CMD_SET_SRC_VOL command.
*/
typedef struct {
  void    *id;          /* ID of source                                    */
  sint31  volume;       /* Volume of source in ddBm                        */
  uint32  duration_ms;  /* Duration in ms to move to new volume            */
} cmx_3d_pos_src_volume_cmd_type;

/* The following structure is used to set the volume distance mapping of
** source object. This is used with the CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN
** command.
*/
typedef struct {
  void    *id;          /* ID of source                                      */
  uint32  min_distance; /* Distance at which sounds are played at max volume */
  uint32  max_distance; /* Distance beyond which sounds are muted            */
  uint32  scale;        /* Scale (0...32767) 0: 0.0 (no rolloff)             */
  boolean mute_after_max;/* Mute after Max: TRUE-Mute, FALSE-Hold after Max  */
} cmx_3d_pos_src_vol_atten_cmd_type;

/* These parameters are set for the CMX_3D_POS_CMD_SET_SRC_VEL command */
typedef struct {
  void   *id;           /* ID of source                                     */
  sint31 x_vel;         /* X velocity of source                             */
  sint31 y_vel;         /* Y velocity of source                             */
  sint31 z_vel;         /* Z velocity of source                             */
  uint32 duration_ms;   /* Duration in ms to change to new velocity         */
} cmx_3d_pos_src_vel_cmd_type;

/* The following structure is used to set the volume of source object. This
** is used with the CMX_3D_POS_CMD_SET_SRC_VOL command.
*/
typedef struct {
  void    *id;          /* ID of source                                    */
  sint31  volume;       /* Reverb Volume of source in ddBm                 */
} cmx_3d_reverb_src_vol_cmd_type;

/* Union of all the command parameter structures. Use is determined by
** command.
*/
typedef union {
  cmx_3d_pos_enable_cmd_type        enable_cmd;        /* Enable/disable cmd */
  cmx_3d_pos_lsnr_pos_cmd_type      lsnr_pos_cmd;      /* Lsnr position cmd  */
  cmx_3d_pos_lsnr_orient_cmd_type   lsnr_orient_cmd;   /* Lsnr orient cmd    */
  cmx_3d_pos_lsnr_vel_cmd_type      lsnr_vel_cmd;      /* Lsnr velocity cmd  */
  cmx_3d_reverb_cmd_type            reverb_cmd;        /* Reverb cmd  */
  cmx_3d_reverb_level_cmd_type      reverb_level_cmd;  /* Reverb Decay cmd  */
  cmx_3d_reverb_decay_cmd_type      reverb_decay_cmd;  /* Reverb Decay cmd  */
  cmx_3d_reverb_ratio_cmd_type      reverb_ratio_cmd;  /* Reverb ratio cmd  */
  cmx_3d_pos_src_enable_cmd_type    src_enable_cmd;    /* Source enable cmd */
  cmx_3d_pos_src_reset_cmd_type     src_reset_cmd;     /* Source reset cmd  */
  cmx_3d_pos_src_pos_cmd_type       src_pos_cmd;       /* Src position cmd   */
  cmx_3d_pos_src_volume_cmd_type    src_volume_cmd;    /* Source volume cmd  */
  cmx_3d_pos_src_vol_atten_cmd_type src_vol_atten_cmd; /* Src vol atten cmd  */
  cmx_3d_pos_src_vel_cmd_type       src_vel_cmd;       /* Src velocity cmd   */
  cmx_3d_reverb_src_vol_cmd_type    src_reverb_vol_cmd;/* Src Reverb Vol cmd   */
} cmx_3d_pos_param_type;

/* The following is a list of parameters that can be queried */
typedef enum {
  CMX_3D_POS_PARAM_ENV_ENABLED,        /* Environment enabled flag */
  CMX_3D_POS_PARAM_LSNR_POS,           /* Listener position                */
  CMX_3D_POS_PARAM_LSNR_ORIENT,        /* Listener orientation             */
  CMX_3D_POS_PARAM_LSNR_VEL,       /* Listener velocity         */
  CMX_3D_POS_PARAM_REVERB_ENABLED,    /* Reverb Enabled or Disabled */
  CMX_3D_POS_PARAM_REVERB,         /* Reverb parameters         */
  CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS, /* Final source parameter settings  */
  CMX_3D_POS_PARAM_SRC_ENABLED,        /* Source 3D enaled flag            */
  CMX_3D_POS_PARAM_SRC_POS,            /* Source position                  */
  CMX_3D_POS_PARAM_SRC_VOLUME,         /* Source volume                    */
  CMX_3D_POS_PARAM_SRC_VOL_ATTEN,      /* Source volume attenuation        */
  CMX_3D_POS_PARAM_SRC_VEL,        /* Source velocity           */
  CMX_3D_POS_PARAM_SRC_REVERB_VOL, /* Source Reverb Volume      */
  CMX_3D_POS_PARAM_INVALID
} cmx_3d_pos_param_enum_type;

/* The following structure is used to query a parameter from the indicated
** source object
*/
typedef struct {
  cmx_3d_pos_param_enum_type param;   /* Source parameter being queried  */
  void                       *id;     /* ID of source                    */
} cmx_3d_pos_src_get_param_type;

/* This is a union of structures required to issue a get parameter command.
*/
typedef union {
  cmx_3d_pos_param_enum_type    param;     /* Parameter setting requested     */
  cmx_3d_pos_src_get_param_type src_param; /* Source get parameter structure  */
} cmx_3d_pos_get_param_type;

/* This structure is used to return the current position setting of the
** listener or a source object. This structure is used in the callback with
** CMX_3D_POS_PARAM_LSNR_POS and CMX_3D_POS_PARAM_SRC_POS parameter
** types.
*/
typedef struct {
  sint31 x_pos;   /* X position  */
  sint31 y_pos;   /* Y position  */
  sint31 z_pos;   /* Z position  */
} cmx_3d_pos_position_setting_type;

/* This structure is used to return the current orientation of the
** listener. This structure is used in the callback with the
** CMX_3D_POS_PARAM_LSNR_ORIENT parameter type.
*/
typedef struct {
  sint31 x_forward;     /* X coordinate of forward vector                   */
  sint31 y_forward;     /* Y coordinate of forward vector                   */
  sint31 z_forward;     /* Z coordinate of forward vector                   */
  sint31 x_up;          /* X coordinate of up vector                        */
  sint31 y_up;          /* Y coordinate of up vector                        */
  sint31 z_up;          /* Z coordinate of up vector                        */
} cmx_3d_pos_lsnr_orient_setting_type;

/* This structure is used to return the current Listener Velocity .         */
/* This structure is used with CMX_3D_POS_PARAM_LSNR_VEL parameter type     */
/* This structure is used with the CMX_3D_POS_PARAM_SRC_VEL parameter    */
typedef struct {
  sint31 x_vel;         /* X velocity of listener                           */
  sint31 y_vel;         /* Y velocity of listener                           */
  sint31 z_vel;         /* Z velocity of listener                           */
} cmx_3d_pos_velocity_setting_type;

/* This structure is used to return the current Reverb parameters           */
/* This structure is used with CMX_3D_POS_PARAM_REVERB parameter type     */
typedef struct{
  cmx_3d_reverb_enum_type      env_type;
  sint31 room_gain;         /* Room Gain                                    */
  uint32 decay_ms;          /* Decay Time in ms for low frequencies         */
  uint32 decay_ratio;       /* Decay time ratio btwn high and low frequencies */
  uint16 ref_delay;         /* Initial reflection delay time in ms          */
  sint31 reverb_gain;       /* Reverb gain relative to room effect          */
  uint16 reverb_delay;      /* Reverb delay relative to initial reflection in ms */
} cmx_3d_reverb_setting_type;

/* This structure is used to return the volume attenuation settings of a
** source object. This structure is used in the callback with the
** CMX_3D_POS_PARAM_SRC_VOL_ATTEN parameter type.
*/
typedef struct {
  uint32  min_distance; /* Distance at which sounds are played at max volume */
  uint32  max_distance; /* Distance beyond which sounds are muted            */
  uint32  scale;        /* Scale (0...1000), 1000: 1.0 (normal rolloff)      */
                        /*                      0: 0.0 (no rolloff)          */
  boolean mute_after_max;/* Mute after max: TRUE-Mute,FALSE-Hold after Max   */
} cmx_3d_pos_src_vol_atten_setting_type;

/* This structure is used to return the volume setting of a
** source object. This structure is used in the callback with the
** CMX_3D_POS_PARAM_SRC_VOLUME parameter type.
*/
typedef struct {
  sint31  volume;       /* Volume of source in ddBm */
} cmx_3d_pos_src_volume_setting_type;

/* This structure is used to return the source reverb volume.  */
/* This structure is used with CMX_3D_POS_PARAM_SRC_REEVRB_VOL parameter type. */
typedef struct {
  sint31  volume;       /* Volume of source in ddBm */
} cmx_3d_reverb_src_vol_setting_type;

/* This structure contains all the 3D positional source parameter settings.
** This structure is used in the callback with the
** CMX_3D_POS_PARAM_SRC_FINAL_SETTINGS.
*/
typedef struct {
  boolean                               src_enable; /* Source 3D enable flag  */
  cmx_3d_pos_position_setting_type      position;  /* Source position        */
  cmx_3d_pos_src_vol_atten_setting_type vol_atten; /* Source vol attenuation */
  cmx_3d_pos_src_volume_setting_type    volume;    /* Source volume          */
  cmx_3d_pos_velocity_setting_type      src_vel_cmd; /* Src velocity cmd   */
  cmx_3d_reverb_src_vol_setting_type    reverb_vol; /* Source Reverb Volume */
} cmx_3d_pos_src_all_setting_type;

/* The following is a union of the structures used to return parameter
** settings to the client.
*/
typedef union {
  boolean                               env_enable; /* Env Enabled    */
  cmx_3d_pos_position_setting_type      position;     /* Current position     */
  cmx_3d_pos_lsnr_orient_setting_type   lsnr_orient;  /* Listener orientation */
  cmx_3d_pos_velocity_setting_type      lsnr_vel;     /* Listener velocity    */
  boolean                               reverb_enable; /* Reverb Enabled    */
  cmx_3d_reverb_setting_type            reverb_param;  /* Reverb parameters   */
  boolean                               src_enable; /* Src Enabled    */
  cmx_3d_pos_src_vol_atten_setting_type src_vol_atten;/* Source vol atten.    */
  cmx_3d_pos_src_volume_setting_type    src_volume;   /* Source vol setting   */
  cmx_3d_pos_velocity_setting_type      src_vel;      /* Source vel setting   */
  cmx_3d_reverb_src_vol_setting_type    src_reverb_vol; /* Source Reverb vol */
  cmx_3d_pos_src_all_setting_type       src_all;      /* All source settings  */
} cmx_3d_pos_param_setting_type;

/* Callback function for a 3D positional query parameter command */
typedef void (*cmx_3d_pos_get_cb_ptr_type) (
  cmx_status_type               status,       /* Status of callback           */
  cmx_3d_pos_param_enum_type    param,        /* Parameter setting returned   */
  cmx_3d_pos_param_setting_type *setting,     /* Value of parameter           */
  const void                    *client_data  /* Client data                  */
);
#endif /* FEATURE_QAUDIOFX */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
typedef enum {
  CMX_AUDFMT_POLY_MODE_0,    /* All HP-mode, 72 HP voices  */
  CMX_AUDFMT_POLY_MODE_1,    /* 56 HP voices, 4 HQ voices */
  CMX_AUDFMT_POLY_MODE_2,    /* 46 HP voices, 8 HQ voices  */
  CMX_AUDFMT_POLY_MODE_3,    /* 38 HP voices, 12 HQ voices */
  CMX_AUDFMT_POLY_MODE_4,    /* 30 HP voices, 16 HQ voices */
  CMX_AUDFMT_POLY_MODE_5,    /* 20 HP voices, 20 HQ voices */
  CMX_AUDFMT_POLY_MODE_6,    /* 12 HP voices, 24 HQ voices */
  CMX_AUDFMT_POLY_MODE_7     /* All HQ-mode, 32 HQ voices  */
} cmx_audfmt_poly_mode_type ;
#endif /* FEATURE_AUDFMT_HYBRID_MODE */

/* <EJECT> */
/*---------------------------------------------------------------------------
** CMX Public Function Prototypes
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_AUDIO_FORMAT
/*===========================================================================

FUNCTION cmx_audfmt_play_codec

DESCRIPTION
  This function issues a command to play a specified audio codec type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_play_codec (
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_cb_func_ptr_type             cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_codec_av_sync_ex

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provide feedback that can be used to synchronize the audio with other media.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_play_codec_av_sync_ex (
  cmx_handle_type                 *handle,
  cmx_codec_spec_ex_type          *codec_spec,
  cmx_av_sync_ex_type             *av_sync, 
  cmx_cb_func_ptr_type            cb_func,
  void                            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_avsync_playback

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provide feedback that can be used to synchronize the audio with other media.
  This API is different from cmx_audfmt_play_codec_av_sync as it provides the 
  type of ABORT to the client (caller) via callback function of type 
  cmx_audfmt_play_cb_func_ptr_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audfmt_avsync_playback
(
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_av_sync_type                 *av_sync,
  cmx_audfmt_play_cb_func_ptr_type  cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_codec_av_sync

DESCRIPTION
  This function issues a command to play a specified audio codec type and
  provide feedback that can be used to synchronize the audio with other media.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_play_codec_av_sync (
  cmx_handle_type                  *handle,
  cmx_codec_spec_type              *codec_spec,
  cmx_av_sync_type                 *av_sync,
  cmx_cb_func_ptr_type             cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play

DESCRIPTION
  This function issues a command to play a specified file. The audio format
  will be automatically detected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_play (
  cmx_handle_type                  *handle,
  cmx_af_callback_enum_type        cb_type,
  cmx_audfmt_start_enum_type       start_position,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_play_multi

DESCRIPTION
  This function issues a command to play an audio file in multisequencer
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_play_multi (
  cmx_handle_type                  *handle,
  cmx_audfmt_multi_param_type      *param,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_ringer

DESCRIPTION
  This function issues a command to play a midi ring.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_ringer
(
  cmx_handle_type             *handle,
  uint16                      repeat_timer,
  cmx_audfmt_start_enum_type  start_position,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_stop

DESCRIPTION
  This function issues a command to stop midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_stop (
  cmx_cb_func_ptr_type       cb_func,
  void                       *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_rewind

DESCRIPTION
  This function issues a command to rewind midi/WebAudio playback the
  indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_rewind (
  uint32                time_ms,
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_fforward

DESCRIPTION
  This function issues a command to fast forward midi/WebAudio playback the
  indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_fforward (
  uint32                time_ms,
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pause

DESCRIPTION
  This function issues a command to pause midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_pause (
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_seek_pause

DESCRIPTION
  This function issues a command to prepare for a rewind/fast-forward
  command for midi/WebAudio playback.

DEPENDENCIES
  cmx_af_playing

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_seek_pause (
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_resume

DESCRIPTION
  This function issues a command to resume midi/WebAudio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_resume (
  cmx_cb_func_ptr_type  cb_func,
  void                  *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tempo

DESCRIPTION
  This function issues a command to adjust the midi/WebAudio playback tempo.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_tempo
(
  uint32               tempo_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_tune

DESCRIPTION
  This function issues a command to adjust the midi/WebAudio playback tune.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_tune
(
  uint32               tune_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_pan

DESCRIPTION
  This function issues a command to adjust the playback stereo pan.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_pan
(
  uint32               pan_factor,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_sequence_cmd

DESCRIPTION
  This function issues a command to adjust the playback of a sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_sequence_cmd
(
  void                      *sequence,
  cmx_audfmt_seq_param_type *seq_param,
  cmx_cb_func_ptr_type      cb_func,
  void                      *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time

DESCRIPTION
  This function issues a command to calculate the time of the indicated
  midi/WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_get_time (
  cmx_handle_type                   *handle,
  cmx_audfmt_time_cb_func_ptr_type  cb_func,
  void                              *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_time_abort

DESCRIPTION
  This function issues a command to abort the midi/WebAudio get time service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_get_time_abort (void);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_get_specs

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  audio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmx_audfmt_get_specs (
  cmx_handle_type                   *handle,
  cmx_audfmt_spec_cb_func_ptr_type  cb_func,
  void                              *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audio_playback

DESCRIPTION
  This function issues a command to play an audio file.

  Note: This function has been added to support remote client controlled data
        source audio playback. This interface is very similar to
        cmx_audfmt_play() except that cmx_client_req_func_ptr_type is
        included in the parameter list. It is assumed that callbacks are to be
        returned by reference (see cmx_audfmt_play()).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audio_playback (
  cmx_handle_type                  *handle,
  cmx_audfmt_play_cb_func_ptr_type cb_func,
  cmx_client_req_func_ptr_type     data_req_ptr,
  void                             *client_data
);

/* <EJECT> */
#ifdef FEATURE_AUDIO_ISDBT_DUAL_MONO
/*===========================================================================

FUNCTION cmx_audio_playback_control_ex

DESCRIPTION
This function issues a command to a currently active audio playback. Some of
commands could come with command  parameters. Caller of this function needs to
make sure CMX_SUCCESS has been returned for playback command before issuing
command to this function.

DEPENDENCIES
  This function should be called after play command has been carried out.   

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_audio_playback_control_ex (
  cmx_audio_playback_cmd_enum_type   command,
  cmx_audio_playback_cmd_param_type  *cmd_param,
  cmx_cb_func_ptr_type               cb_func,
  const void                         *client_data
);
#endif /* FEATURE_AUDIO_ISDBT_DUAL_MONO */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audio_playback_control

DESCRIPTION
  This function issues a command to the audio playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audio_playback_control (
  cmx_audio_playback_cmd_enum_type command,
  cmx_cb_func_ptr_type             cb_func,
  const void                       *client_data
);

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION cmx_qcp_get_data_size

DESCRIPTION
  This function retrieves the size of the data chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_data_size (
  cmx_handle_type        *handle,
  uint32                 *num_bytes,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_frames

DESCRIPTION
  This function retrieves the number of frames in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmx_qcp_get_frames (
  cmx_handle_type        *handle,
  uint32                 *num_frames,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_cnfg

DESCRIPTION
  This function retrieves the cnfg chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_get_cnfg (
  cmx_handle_type        *handle,
  uint16                 *cnfg,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_cnfg

DESCRIPTION
  This function sets the cnfg chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_set_cnfg (
  cmx_handle_type        *handle,
  uint16                 cnfg,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_labl

DESCRIPTION
  This function retrieves the labl chunk from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_get_labl (
  cmx_handle_type        *handle,
  uint32                 *labl_length,
  uint8                  *labl_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_labl

DESCRIPTION
  This function sets the labl chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_set_labl (
  cmx_handle_type        *handle,
  uint32                 labl_length,
  uint8                  *labl_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_get_text

DESCRIPTION
  This function retrieves the text chunk from a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_get_text (
  cmx_handle_type        *handle,
  uint32                 *text_length,
  uint8                  *text_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_set_text

DESCRIPTION
  This function sets the text chunk in a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_set_text (
  cmx_handle_type        *handle,
  uint32                 text_length,
  uint8                  *text_buffer,
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/* <EJECT> */
#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION cmx_qcp_fs_space_available

DESCRIPTION
  This function checks the EFS to see if there is enough space available
  to start recording a QCP file.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is space available to start recording.
  FALSE if there is not sufficient space available to start recording.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmx_qcp_fs_space_available ( void );
#endif /* FEATURE_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_reverse

DESCRIPTION
  This function sends a command to start QCP recording on reverse link.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_record_reverse (
  cmx_handle_type       *handle,
  cmx_qcp_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_forward

DESCRIPTION
  This function sends a command to start QCP recording on forward link.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_record_forward (
  cmx_handle_type       *handle,
  cmx_qcp_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_stop

DESCRIPTION
  This function sends a command to stop QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_record_stop (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_pause

DESCRIPTION
  This function sends a command to pause QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_record_pause (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_qcp_record_resume

DESCRIPTION
  This function sends a command to resume QCP recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_qcp_record_resume (
  cmx_cb_func_ptr_type cb_func,
  const void           *client_data
);
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION cmx_pcm_record

DESCRIPTION
  This function sends a command to start PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_pcm_record (
  cmx_handle_type       *handle,
  cmx_pcm_rec_para_type *rec_param,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_stop

DESCRIPTION
  This function sends a command to stop PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_pcm_record_stop (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_pause

DESCRIPTION
  This function sends a command to pause PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_pcm_record_pause (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_pcm_record_resume

DESCRIPTION
  This function sends a command to resume PCM recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_pcm_record_resume (
  cmx_pcm_rec_link_type link,
  cmx_cb_func_ptr_type  cb_func,
  const void            *client_data
);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION cmx_mm_record

DESCRIPTION
  This function sends a command to start MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_mm_record (
  cmx_handle_type                *handle,
  cmx_mm_rec_operation_mode_type op,
  cmx_mm_rec_param_type          *rec_param,
  cmx_cb_func_ptr_type           cb_func,
  const void                     *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_stop

DESCRIPTION
  This function sends a command to stop MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_mm_record_stop (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_pause

DESCRIPTION
  This function sends a command to pause MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_mm_record_pause (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);

/*===========================================================================

FUNCTION cmx_mm_record_resume

DESCRIPTION
  This function sends a command to resume MultiMedia recording.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_mm_record_resume (
  cmx_mm_rec_link_type   link,        /* Direction to record           */
  cmx_cb_func_ptr_type   cb_func,
  const void             *client_data
);
#endif /* FEATURE_MM_REC */
#endif /* FEATURE_AUDIO_FORMAT */

/* <EJECT> */
#ifndef FEATURE_MSP
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION cmx_png_decode

DESCRIPTION
  This function issues a command to decode a PNG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_png_decode(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_abort

DESCRIPTION
  This function issues a command to abort PNG decoding.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_png_abort(
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_png_get_specs

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  PNG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_png_get_specs (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION cmx_jpeg_decode

DESCRIPTION
  This function issues a command to decode a JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_jpeg_decode(
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_decode_abort

DESCRIPTION
  This function issues a command to abort JPEG decoding/encoding.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_jpeg_decode_abort(
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_get_specs

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_jpeg_get_specs (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);
#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_ENCODER
/*===========================================================================

FUNCTION cmx_jpeg_encode

DESCRIPTION
  This function issues a command to encode a JPEG file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_jpeg_encode(
  cmx_jpeg_spec_type         image_spec,
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_jpeg_encode_abort

DESCRIPTION
  This function issues a command to abort JPEG encode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_jpeg_encode_abort(
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

#endif /* FEATURE_JPEG_ENCODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION cmx_saf_decode

DESCRIPTION
  This function issues a command to decode a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_decode
(
  cmx_handle_type              *handle,
  cmx_saf_ctl_type             saf_ctl,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_abort

DESCRIPTION
  This function issues a command to abort a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_abort (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_frame_id_cmd

DESCRIPTION
  This function issues a SAF frame ID command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_frame_id_cmd (
  uint32                 frame_id,
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_pause

DESCRIPTION
  This function issues a command to pause a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_pause (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_resume

DESCRIPTION
  This function issues a command to resume a SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_resume (
  cmx_cb_func_ptr_type   callback_ptr,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_saf_get_info

DESCRIPTION
  This function issues a command to retrieve the specifications of the
  SAF file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_saf_get_info (
  cmx_handle_type              *handle,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);
#endif /* FEATURE_SAF */
#endif /* FEATURE_MSP */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT
/*===========================================================================

FUNCTION cmx_midi_out_open

DESCRIPTION
  This function issues a command to enable midi commands.
  Note: Audio decoder type will be defaulted to QCP for this function.
        Use cmx_midi_out_open_adec_format() to explicitly set ADEC type.
        This function is being kept around for backwards compatibility.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_open
(
  cmx_cb_func_ptr_type    cb_func,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_open_adec_format

DESCRIPTION
  This function issues a command to enable midi commands and allows
  client to specify the audio decoder type to use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_open_adec_format
(
  cmx_adec_mode_enum_type adec_mode,
  cmx_cb_func_ptr_type    cb_func,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_close

DESCRIPTION
  This function issues a command to disable midi commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_close
(
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_msg

DESCRIPTION
  This function issues midi commands to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_msg
(
  cmx_handle_type        *handle,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION cmx_midi_out_qcp

DESCRIPTION
  This function plays a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define CMX_MIDI_OUT_QCP_DEFAULT_VOLUME 0x2000
#define CMX_MIDI_OUT_QCP_DEFAULT_PAN    0
extern void cmx_midi_out_qcp
(
  cmx_handle_type                 *handle,
  uint16                          volume,
  sint15                          pan,
  cmx_midi_out_cb_func_ptr_type   cb_func,
  void                            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec

DESCRIPTION
  This function plays an audio file to the midi output device.
  Same as cmx_midi_out_qcp() but with some extra parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_adec
(
  cmx_handle_type                 *handle,
  cmx_midi_out_adec_param_type    *param,
  cmx_midi_out_cb_func_ptr_type   cb_func,
  void                            *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_adec_start_chan

DESCRIPTION
  This function starts playback of the indicated audio file that is in
  delayed start mode to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_adec_start_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_stop

DESCRIPTION
  This function stops a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_stop
(
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause

DESCRIPTION
  This function pauses QCP files to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_pause
(
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_resume

DESCRIPTION
  This function resumes QCP files to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_resume
(
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pause_chan

DESCRIPTION
  This function pauses the indicated QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_pause_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_resume_chan

DESCRIPTION
  This function resumes indicated QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_resume_chan (
  void                   *id,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_pan

DESCRIPTION
  This function changes the panning for a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_pan
(
  void                   *id,
  sint15                 pan,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_midi_out_qcp_volume

DESCRIPTION
  This function changes the volume for a QCP file to the midi output device.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_midi_out_qcp_volume
(
  void                   *id,
  uint16                 volume,
  cmx_cb_func_ptr_type   cb_func,
  void                   *client_data
);
#endif /* FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_MIDI_OUT */

/* <EJECT> */
#ifdef FEATURE_QAUDIOFX
/*===========================================================================

FUNCTION cmx_3d_pos_cmd

DESCRIPTION
  This function issues a 3D positional command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_3d_pos_cmd (
  cmx_3d_pos_cmd_enum_type cmd,
  cmx_3d_pos_param_type    *param,
  cmx_cb_func_ptr_type     cb_func,
  void                     *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_3d_pos_get_param

DESCRIPTION
  This function queries the indicated 3D positional parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_3d_pos_get_param (
  cmx_3d_pos_get_param_type  *param,
  cmx_3d_pos_get_cb_ptr_type cb_func,
  void                       *client_data
);
#endif /* FEATURE_QAUDIOFX */

/* <EJECT> */
#ifdef FEATURE_AUDIO_POSTCARD
/*===========================================================================

FUNCTION cmx_create_audiop

DESCRIPTION
  This function creates an audio postcard.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_create_audiop
(
  cmx_handle_type      *jpeg,
  cmx_handle_type      *qcp,
  cmx_handle_type      *output,
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);
#endif /* FEATURE_AUDIO_POSTCARD */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION cmx_audfmt_load_dls

DESCRIPTION
  This function loads the indicated DLS file to be used by all MIDI-based
  sequences.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_load_dls (
  cmx_handle_type             *handle,
  cmx_cb_func_ptr_type        cb_func,
  void                        *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_audfmt_unload_dls

DESCRIPTION
  This function unloads all DLS files that have been loaded by
  cmx_audfmt_load_dls().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmx_audfmt_unload_dls (
  cmx_cb_func_ptr_type cb_func,
  void                 *client_data
);
#endif /* FEATURE_DLS */

#if defined( FEATURE_SVG ) && defined (FEATURE_WEBAUDIO)
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_decode

DESCRIPTION
  This function issues a command to start playback of SVG file

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_decode
(
  cmx_handle_type              *handle,
  cmx_svg_decode_data_type     *cmx_svg_decode_data,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_frame_cmd

DESCRIPTION
  This function issues a command to render a SVG frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_frame_cmd
(
  void*                            handle,     /* CMX handle */
  uint32                           frame_time, /*Rendering time of frame */
  cmx_cb_func_ptr_type             callback_ptr,
  void                             *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_get_info

DESCRIPTION
  This function issues a command to get info of a SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_get_info
(
  cmx_handle_type              *handle,
  cmx_svg_decode_data_type     *cmx_svg_decode_data,
  cmx_image_cb_func_ptr_type   callback_ptr,
  void                         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_process_event

DESCRIPTION
  This function issues a command to process user events for SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_process_event
(
  void*                     handle,
  cmx_svg_event_enum_type   event_type,
  void*                     event_data,
  uint32                    num_bytes,
  uint32                    time_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_abort

DESCRIPTION
  This function issues a command to abort SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_abort
(
  void*                     handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_pause

DESCRIPTION
  This function issues a command to pause SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_pause
(
  void*                     handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_resume

DESCRIPTION
  This function issues a command to resume playback of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_resume
(
  void*                     handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_rewind

DESCRIPTION
  This function issues a command to rewind SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_rewind
(
  void*                     handle,
  uint32                  time_ms,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_fforward

DESCRIPTION
  This function issues a command to fast-forward SVG file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_fforward
(
  void*                     handle,
  uint32                  time_ms,
  cmx_cb_func_ptr_type     callback_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_reset_frame

DESCRIPTION
  This function issues a command to reset the Zoom and Pan factors of SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_reset_frame(
  void*                     handle,
  cmx_cb_func_ptr_type     callback_ptr,
  void                     *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_set_viewport

DESCRIPTION
  This function sets the viewport of SVG session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A new Client Buffer might be requested
  cmx_cmd_q
  cmx_free_q
  cmx_free_packet
===========================================================================*/
void cmx_svg_set_viewport(
  void*                  handle,
  uint32                 width,
  uint32                 height,
  cmx_cb_func_ptr_type   callback_ptr,
  void                  *client_data
);

#ifdef FEATURE_SVGDOM
/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svgdom_apis

DESCRIPTION
  This function calls the corresponding DOM API in SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmx_svgdom_apis(
  void*           handle,
  cmx_svgdom_api_enum_type type,
  cmx_svgdom_para_type*    data
);
#endif /* FEATURE_SVGDOM */

/* <EJECT> */
/*===========================================================================

FUNCTION cmx_svg_req_cb

DESCRIPTION
  This function is used as a callback by clients for returning external file
  requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Queues a command in CMX context
===========================================================================*/
void cmx_svg_req_cb(
  void* handle,
  uint8** buffer,            /* Buffer with URL data in one chunk */
  uint32 num_bytes
);

#endif /*FEATURE_SVG */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
/*==========================================================================
 FUNCTION cmx_audfmt_multi_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_multi_poly_mode(
  cmx_audfmt_poly_mode_type mode,
  cmx_cb_func_ptr_type      cb_ptr,
  void                      *client_data
);

/*==========================================================================
 FUNCTION cmx_audfmt_get_multi_poly_mode

 DESCRPTION
   This function gets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/
void cmx_audfmt_get_multi_poly_mode(
  cmx_audfmt_poly_mode_type *mode,
  cmx_cb_func_ptr_type      cb_ptr,
  void                      *client_data
);

#endif /* FEATURE_AUDFMT_HYBRID_MODE */

#ifdef FEATURE_WEBAUDIO

#ifdef FEATURE_CMX_SCREEN_DIMENSIONS
/*==========================================================================
 FUNCTION cmx_audfmt_set_screen_size

 DESCRPTION
   This function sets the SVG scaling.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
 ==========================================================================*/

extern void cmx_audfmt_set_screen_size (
  uint32   width,
  uint32   height
);

#endif  /* FEATURE_CMX_SCREEN_DIMENSIONS */

#endif  /* FEATURE_WEBAUDIO */
#endif /* FEATURE_CMX */
#endif /* CMX_H */
