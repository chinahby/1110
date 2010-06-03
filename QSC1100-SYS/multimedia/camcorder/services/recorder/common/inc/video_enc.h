#ifndef VIDEO_ENC_H
#define VIDEO_ENC_H
/*===========================================================================

    V I D E O    E N C O D E R    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface 
  with the video encoder driver.

REFERENCES
  Video Encoder ISOD: 00-XXXXX-XX

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/video_enc.h#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/09   sanal   Support to set initial delta dynamically, if frames are dropped
                   initially during resume.
03/12/07   ali     Added max_bitrate flag.
02/09/07   ali     Changes for dynamic RC based on feedback from EFS
09/28/06   Gopi    AAC transcoding support.
09/15/06   jk      Corrected VIDEO_ENC_INTRA_MEMORY_SIZE.
09/06/06   dhaval  Removded the frame_callback_ptr which was added for transcoder
09/04/06   dhaval  Added transcoder definition through transcoder_task.h file
06/13/06   pavan   Removed VIDEO_ENCODER_SIG from global scope.  Moved it to
                   camcorder specific (non-global) scope.
04/20/06   Nag     wrapper added for malloc, free and realloc routines
03/27/06   dw      Added support for allowing user-defined advanced config
02/14/06   dw      Changes for RC_ON_TIGHT rate control
09/23/05   dw/aa   Added error message if GRAPH calls video_encode_exit
09/09/05   dw      Added support for adaptive VFE luma filtering
                   Changes for video driver to access N-frame video buffers
08/31/05   wyh     Implement swap for frame drop.
07/25/05   dw      Added VFE luma filter update mechanism
07/20/05   jk      Added support for frame time in ticks as well as in ms.
07/05/05   ali     Fixed the race condition where graph task could access 
                   buffers after VS had freed it
05/25/05   drl     Replaced include of custmp4.h with customer.h
05/25/05   jk      Added support to set first frame delta.
05/17/05   wyh     Pass brand from xcoder to init_video_encode_raw_frame()
03/18/05   jk      Synced up with csim.
02/22/05   jk      Changed video_enc_dynamic_config_type to a union.
02/22/05   wyh     Raw Encoder API changes.
02/11/05   pp      Deletion of the rho-qp table.
02/10/05   wyh     Allow users to specify bitrate in transcoder.
01/21/05   wh      Eliminate all malloc()'s from the mp4 library.
01/20/05   pp      Added rate control constants.
01/19/05   wh      Temporary API. Don't handshake with video_eng_frame
01/11/05   jk      Added fixed_fps_mode to video_enc_init_type
12/10/04   jk      Changed video_enc_init_type regarding short header.
11/29/04   drl     Added video_encode_permitted function.
11/03/04   jk      Changed frame callback function to get new bitstream 
                   buffer with its size for every frame.
10/29/04   pp      Added Parametric Rate Control.
10/21/04   jk      Changed type of rot_angle to int16 in video_enc_init_type
10/01/04   jk      Changed viden_encode_* API. 
                   Added function pointer types of the encoder.
08/24/04   pp/wh   Merged in rate control
05/25/04   sj      Removed Error Resilience and other Code Cleanup.
03/22/03   sa      Added reference to external function mp4_encode_raw_init()
03/12/03   ali     Code cleanup
03/02/04   ali     Changes for offline encoder
01/12/04   yye     Changed video_eng to frame_cb to match slice_cb
01/07/04   sj      Added video_client_slice_cb_func_ptr_type for per slice cb
11/19/03   sj      Added video_encode_dynamic_set_config to allow the caller
                   to dynamically set the config.
11/11/03   sj      Added video_encode_request_i_frame to dynamically
                   request an I Frame while recording.
11/08/03   sj/jz   Added VT optimizations.
10/08/03   sj      Enhanced video_enc_config_type for QVP support
10/08/03   sj/yye  Added vfmt to video_enc_config_type
09/08/03   sj      Added delta time to video_client_req_cb_func_ptr_type
08/01/03   sj      Added short_header to video_enc_init_type
                   Added frame_type to video_client_req_cb_func_ptr_type
07/18/03   sj      Added Resync Marker and HEC for QVideoPhone
                   Added error_res in video_enc_config_type
06/29/03   sj      Cleaned up code.  Added H263 Support.
03/20/03   sj      Added Code Optimizations
03/18/03   sj      Added Fractional Rate Support
03/11/03   sj/nm   Added Rate Control Support
03/07/03   rpw     Merged in optimizations identified by Scott, Naren, and
                   myself.
02/21/03   rpw     Merged in Nurit's rate control code.
02/14/03   sa      Modified to get to compile as library
01/22/03   sj      Support EFS debug and Alpha Release
12/17/02   sj      Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "video_common.h"

#ifdef FEATURE_VIDEO_ENCODE

#ifdef T_ARM
#include "customer.h"
#include "rex.h"
#endif

#ifdef FEATURE_MP4_TRANSCODER
#include "transcoder_task.h"
#endif

#include "video_buffers.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define QDSP_MB_LEN             779      /* For MPEG-4/H.263 - 779=11+6*2*64 */
/* 
** Parametric Rate Control.
** For MPEG-4/H.263:
** 32 entries of rho_qp_table and nz_count for rho-rate-control - total 132 bytes, 
** plus, alpha and beta - both 4 bytes long.
*/
#define QDSP_RHOINFO_LEN        12//140 

#ifdef FEATURE_H264_ENCODER
#define QDSP_MB_LEN_H264        852      /* For H.264        - 852=???       */
/* 
** Parametric Rate Control.
** For H.264:
** nz_count for rho-rate-control - total 4 bytes, 
** plus, alpha and beta - both 4 bytes long.
*/
#define QDSP_RHOINFO_LEN_H264   12
#endif


#define VIDEO_ENC_BUF_ALPHA   16
#define VIDEO_ENC_CALC_BUF_SIZE(target_bitrate, time_incr_res, time_incr)   \
              (VIDEO_ENC_BUF_ALPHA * (target_bitrate) / 8 /                 \
              (((time_incr_res) + ((time_incr) >> 1)) / (time_incr)))

/*Buffer size calculation for storing Image data
** for Mp4 
*/

#define VIDEO_ENC_INTERNAL_MEMORY_SIZE(frame_width,frame_height)         \
    (                                                                    \
      ((frame_width * frame_height * 9 / 129) * sizeof(uint16))          \
    )
/*RC buffer size calculation based on Time resolution
**for Mp4 and H.264
** DEFAULT_INTRA_PERIOD_MS = 2000, update this intra period value(2000) whenever 
** default value is changed in  video_enc_common.h
*/
#define VIDEO_ENC_INTRA_MEMORY_SIZE(time_incr_res, time_incr)             \
  (                                                                       \
   ((2000 * time_incr_res + 500) / 1000)                                 \
   <= (time_incr_res)                                                    \
    ? (1 * sizeof(int32))                                                 \
   : (((2000*time_incr_res + 500)*sizeof(int32) / 1000)                  \
   / time_incr) +10000                                                         \
  )

#ifdef FEATURE_VIDEO_ENCODE
#define VIDEO_ENCODER_SIG     0x01000000
#endif

/*
** Enum for the Video Driver State Machine
*/
typedef enum {
  VIDEO_NOT_INIT = 0,      /* Driver has not been initialized or has exit */
  VIDEO_INIT,              /* Has been initialized                        */
  VIDEO_ENCODING_STARTED,  /* Has started encoding frames                 */
  VIDEO_ENCODING_FRAME,    /* Encoding of a frame in progress.            */  
  VIDEO_ENCODING_STOPPED   /* Isn't currently encoding frames             */
} video_enc_state_type;


/*
** This enum is used for specifying the Rate Control Setting
*/
typedef enum
{
  RC_ON = 1,
  RC_ON_VAR_FPS,
  RC_ON_TIGHT
} video_enc_rc_type;


/*
** Structure used to initialize MPEG4/H.263 encoder 
*/
typedef struct mp4_init_struct {
  boolean            short_header;       /* H.263 compatibility               */

  /* Internal Memory required for an MP4 decoder must be of size
  ** frame_width*frame_height*9/129.  
  **
  ** or frame_width*frame_height*9/129 * sizeof(uint16)
  **
  **Client is responsible for allocating and freeing it.
  */
} mp4_init_type;


#ifdef FEATURE_H264_ENCODER
/*
** Structure used to initialize H.264 encoder 
*/
typedef struct h264_init_struct {
  boolean            avc_sample;       /* ISO file format friendly output     */
} h264_init_type;
#endif


/*
** Structure used to codec specifically initialize the video encoder 
*/
typedef union video_enc_codec_init_union {
  mp4_init_type      mp4;               /* MPEG4/H.263 specific init type     */
#ifdef FEATURE_H264_ENCODER
  h264_init_type     h264;              /* H.264 specific init type           */
#endif
} video_enc_codec_init_type;


/*
** Structure used to inform the video interface the specifications for the 
** config to be used in video processing
*/
typedef struct video_enc_init_struct {
  uint16             frame_width;        /* Frame width                       */
  uint16             frame_height;       /* Frame height                      */
  uint16             time_incr_res;      /* # of ticks in 1 second            */
  uint16             time_incr;          /* # of ticks b/w consecutive frames */
  int32              target_bitrate;     /* Target bit rate                   */
  video_profile_type profile;            /* Video profile                     */
  video_fmt_brand_type vbrand;           /* Video Brand Type                  */
  video_fmt_stream_video_type vfmt;      /* Video Format Type                 */
  int16              rot_angle;          /* Rotation value                    */
  boolean            fixed_fps_mode;     /* Fixed frames per second mode      */
  uint8*             vop_recon_buf_ptr1; /* Recon frame buf 1                 */
  uint8*             vop_recon_buf_ptr2; /* Recon frame buf 2                 */
  uint8*             enc_pkt_ptr1;       /* DSP output frame buf-1            */
  uint8*             enc_pkt_ptr2;       /* DSP output frame buf-2            */
  uint32             enc_pkt_size;       /* DSP output frame buffer size      */
  uint8*             ved_buf_ptr;        /* Video enc output data             */
  uint32             ved_buf_size;       /* Video enc output buffer size      */
  VideoBufferHandle* bufferHandle;       /* N-frames video buffer handle      */
  uint32             bufferCount;        /* Value of N in N-frames buffer     */
  video_enc_codec_init_type codec_init;  /* Codec specific init type          */
  boolean            dynamic_bitrate_enable;
  int16              *internal_memory;  
  uint32             inter_memory_size;
  boolean            max_bitrate_enable;
} video_enc_init_type;


/*
** This enum is used for specifying time unit.
*/
typedef enum {
  VIDEO_ENC_TIME_MS,       /* Millisecond     */
  VIDEO_ENC_TIME_TICK,     /* Tick            */  
  VIDEO_ENC_TIME_INVALID   /* Invalid type    */
} video_enc_time_type;


/* 
** The video_enc_config_type has MPEG configuration data which doesn't 
** change per VOP.  If these parameters are not updated with 
** video_encode_set_config, they have the following initial values.
**
** - target_bitrate     = determined by 'profile' in video_encode_init.
** - intra_period       = 2000 [ms]
** - first_frame_delta  = 0
** - time_unit          = VIDEO_ENC_TIME_MS
** - rc_ctl             = RC_ON_VAR_FPS
**
** If 'time_unit' is set to VIDEO_ENC_TIME_MS, the frame capture time
** passed in video_encode_frame should be in ms.  If it is set to
** VIDEO_ENC_TIME_TICK, the frame capture time should be in tick.
*/
typedef struct video_enc_config_struct
{
  int32               target_bitrate;     /* Target bit rate              */
  int32               intra_period;       /* Intra frame period in ms     */
  int32               first_frame_delta;  /* First frame delta            */
                                          /* ignored if <= 0              */
  video_enc_time_type time_unit;          /* Unit of time passed in       */
                                          /* video_encode_frame           */
  video_enc_rc_type   rc_ctl;             /* Rate control flag            */
} video_enc_config_type;

/*
** Placeholder enums
*/
typedef enum
{
  LF_A = 0,
  LF_B,
  LF_C
} video_enc_raw_lf_type;

typedef union codec_specific_raw_config_union
{
  int8 raw_codec_type;
  int8 mp4_raw_cfg;
#ifdef FEATURE_H264_ENCODER
  int8 h264_raw_cfg;
#endif
} codec_specific_raw_config_type;

typedef enum search_method_raw_struct
{
  MP4E_SEARCH_A,
  MP4E_SEARCH_B,
  MP4E_SEARCH_C
} search_method_raw_type;


typedef struct video_enc_raw_dsp_cfg_struct {
  uint32 cookie;               /* This cookie enables this config          */
  uint16 intra_16x16_bias;
  uint16 intra_4x4_bias;
  uint16 inter_one_mv_bias;
  uint16 inter_four_mv_bias;
  uint16 four_mv_threshold;    /* Threshold to decide between 1MV and 4MV  */
  uint8  search_type;          /* MB search, Block search or both          */
  uint8  use_16x16_srch_min;   /* use MB min as anchor for block search    */
  uint8  use_16x16_anch_pnt;   /* use MB anchor as Block anchor            */
  uint8  ac_pred_flag;
  uint8  rounding_ctrl;
  uint8  video_hw_me_profile;  /* HW ME profiling flag                     */
  search_method_raw_type search_method;
} video_enc_raw_dsp_cfg_type;


typedef struct video_enc_raw_config_struct
{
  uint32  cookie;                       /* This cookie enables this config          */
  uint16  quantizer_scale_intra;        /* quant_scale for I-VOPs                   */
  uint16  quantizer_scale_inter;        /* quant_scale for P-VOPs                   */
  uint8   frac_pixel_enable;            /* Enable fractional pixel search           */

  uint16  rcw_overflow_factor;          /* RC Window Overflow Factor                */
  uint16  pframe_overflow_factor;       /* P-Frame Overflow Factor                  */
  uint16  loose_pframe_overflow_factor; /* Loose P-Frame Overflow Factor            */
  uint16  max_qp_frame_range_down;      /* max. qp range down from frame qp         */
  uint16  max_qp_frame_range_up;        /* max. qp range up from frame qp           */
  uint32  log2_nc_square;               /* log2 of square of number coefficients    */
  int32   alpha;                        /* alpha in rate control parmetric equation */ 
  int32   beta;                         /* beta in rate control parmetric equation  */ 

  uint16  quantizer_scale_intra_low;    /* Lower bound for quant_scale for I-VOPs   */
  uint16  quantizer_scale_intra_high;   /* Higher bound for quant_scale for I-VOPs  */
  uint16  quantizer_scale_inter_low;    /* Lower bound for quant_scale for P-VOPs   */
  uint16  quantizer_scale_inter_high;   /* Higher bound for quant_scale for P-VOPs  */

  video_enc_raw_dsp_cfg_type     dsp_cfg;     /* Advanced DSP selection cmd configs */
  video_enc_raw_lf_type          luma_filter; /* Placeholder Luma Filter            */
  codec_specific_raw_config_type codec_cfg;   /* Placeholder codec config           */
  
} video_enc_raw_config_type;

typedef enum 
{
  VIDEO_ENC_CONFIG_BITRATE,
  VIDEO_ENC_CONFIG_INTRA_PERIOD,
  VIDEO_ENC_CONFIG_TIMING,
  VIDEO_ENC_CONFIG_INITIAL_DELTA
} video_enc_config_ops_type ;


typedef struct 
{
  video_enc_config_ops_type  op;
  int32                      target_bitrate;
} video_enc_config_bitrate_type ;


typedef struct 
{
  video_enc_config_ops_type  op;
  int32                      intra_period;
} video_enc_config_iperiod_type ;


typedef struct 
{
  video_enc_config_ops_type  op;
  int32                      time_ms;
} video_enc_config_timing_type ;

typedef struct 
{
  video_enc_config_ops_type  op;
  int32                      delta_ms;
} video_enc_config_initial_delta_type ;

typedef union video_enc_dynamic_config_union
{
  video_enc_config_ops_type                   op;
  video_enc_config_bitrate_type          bitrate;
  video_enc_config_iperiod_type          iperiod;
  video_enc_config_timing_type            timing;
  video_enc_config_initial_delta_type  initdelta;
} video_enc_dynamic_config_type ;


typedef struct video_enc_enc_to_client_struct
{
  uint32            len;        /* Length of data returned                */
  video_frame_type  type;       /* I_FRAME or P_FRAME                     */
  int32             delta;      /* Time diff between t and t-1 frame      */
} video_enc_enc_to_client_type;


typedef struct video_enc_client_to_enc_struct
{
  uint8     *buf_ptr;         /* Output buffer for the next encoded data  */
  uint32    buf_size;         /* Size of the buffer                       */
  boolean   limit_near;       /* Flag that indicates limit is near        */
} video_enc_client_to_enc_type;


typedef struct video_enc_3x3_luma_filter_struct
{
  int8  A11;                   /* row: top     col: left   */
  int8  A12;                   /* row: top     col: center */
  int8  A13;                   /* row: top     col: right  */

  int8  A21;                   /* row: center  col: left   */
  int8  A22;                   /* row: center  col: center */
  int8  A23;                   /* row: center  col: right  */

  int8  A31;                   /* row: bottom  col: left   */
  int8  A32;                   /* row: bottom  col: center */
  int8  A33;                   /* row: bottom  col: right  */

  uint8 coefficient_Q_Factor;  /* Q factor: Q0 - Q7        */
  boolean enable;              /* Enable/Disable           */

}video_enc_3x3_luma_filter_type;


/*
** This is the callback used to inform the video engine that the data
** is ready to be processed
*/
typedef void (* video_client_req_cb_func_ptr_type) 
(
  video_status_type                   status,       /* Status of encoding   */
  const video_enc_enc_to_client_type  *enc_to_client_data,
  video_enc_client_to_enc_type        *client_to_enc_data,
  const void                          *client_data  /* Client data          */
);

/* 
 * This is the video encoder init function.
 */
typedef video_status_type (* video_encode_init_func_ptr_type)  
                          (video_enc_init_type* init_data, void *param);

/* 
 * This is the video encoder exit function.
 */
typedef video_status_type (* video_encode_exit_func_ptr_type) (void);

/* 
 * This is the video encoder start function.
 */
typedef video_status_type (* video_encode_start_func_ptr_type) 
                          (video_client_req_cb_func_ptr_type cb_ptr, 
                           const void* client_data);

/* 
 * This is the video encoder stop function.
 */
typedef video_status_type (* video_encode_stop_func_ptr_type) (void);

/* 
 * This is the video encoder config function.
 */
typedef video_status_type (* video_encode_config_func_ptr_type) 
                          (video_profile_type profile);

/* 
 * This is the video encoder get_config function.
 */
typedef video_status_type (* video_encode_get_config_func_ptr_type) 
                          (video_enc_config_type* cfg_ptr, void *param);

/* 
 * This is the video encoder set_config function.
 */
typedef video_status_type (* video_encode_set_config_func_ptr_type) 
                          (video_enc_config_type* new_cfg_ptr, void *param);

/* 
 * This is the video encoder dynamic set_config function.
 */
typedef video_status_type (* video_encode_dynamic_set_config_func_ptr_type) 
                          (video_enc_dynamic_config_type* new_cfg_ptr, 
                            void *param);

/* 
 * This is the video encoder encode_frame function.
 */
typedef void (* video_encode_frame_func_ptr_type) (uint8* bufPtr);

/* 
 * This is the video encoder request_i_frame function.
 */
typedef video_status_type (* video_encode_req_i_frame_func_ptr_type) (void);

/* 
 * This is the video encoder get_decoder_specific_info function.
 */
typedef void (* video_encode_get_decoder_specific_info_func_ptr_type) 
              (uint32** buf_ptr, uint32* len);

typedef boolean (* video_encode_permitted_func_ptr_type) 
                 (VideoBufferHandle *bufferHandle);

/* 
 * This is for updating the VFE 3x3 luma filter
 */
typedef boolean (* video_encode_update_filter_func_ptr_type) 
                 (video_enc_3x3_luma_filter_type *filter);

#ifdef FEATURE_MP4_TRANSCODER
/* 
 * This is the video encoder offline_encode_start_from_middle function.
 */
typedef video_status_type (* video_offline_encode_start_from_middle_func_ptr_type) 
                          (video_client_req_cb_func_ptr_type cbk_ptr, 
                           const void* client_data);

/* 
 * This is the video encoder raw_init function.
 */
typedef video_status_type (* video_encode_raw_init_func_ptr_type) 
                          (video_enc_init_type* init_data);
#endif


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
typedef video_status_type (* video_encode_swap_func_ptr_type)(void);
#endif


typedef struct video_encode_ops_struct {
  video_encode_init_func_ptr_type                 init;
  video_encode_exit_func_ptr_type                 exit;
  video_encode_start_func_ptr_type                start;
  video_encode_stop_func_ptr_type                 stop;
  video_encode_config_func_ptr_type               config;
  video_encode_get_config_func_ptr_type           get_config;
  video_encode_set_config_func_ptr_type           set_config;
  video_encode_dynamic_set_config_func_ptr_type   dynamic_set_config;
  video_encode_frame_func_ptr_type                encode_frame;
  video_encode_req_i_frame_func_ptr_type          req_i_frame;
  video_encode_get_decoder_specific_info_func_ptr_type 
                                                  get_decoder_specific_info;
  video_encode_permitted_func_ptr_type            encode_permitted;
  video_encode_update_filter_func_ptr_type        update_filter;


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  video_encode_swap_func_ptr_type                 swapper;
#endif

#ifdef FEATURE_MP4_TRANSCODER
  video_offline_encode_start_from_middle_func_ptr_type  
                                                  offline_start_from_middle;
  video_encode_raw_init_func_ptr_type             raw_init;
#endif


} video_encode_ops_type;


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_init

DESCRIPTION
  This function is used to initialize the driver/.  
  This can only be called in VIDEO_NOT_INIT state.
  The state transitions to VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_init_type* init_data:  Used to init the driver and dsp 
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_init(video_enc_init_type* init_data, 
                                    void *param,
                                    video_cb_func_ptr_type callback_ptr, 
                                    void* client_data);

#ifdef FEATURE_MP4_TRANSCODER

/*===========================================================================

FUNCTION init_video_encode_raw_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame for 
  offline encoding. It is called every frame
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  inputBuf: data to be encoded
  target_height: target height of the encoded frame
  target_width: target width of the encoded frame
  frame_duration: Duration of frame
  initialize: state
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
 
===========================================================================*/
video_status_type 
init_video_encode_raw_frame( uint16 target_height, 
                             uint16 target_width, 
                             uint16 frame_duration,
                             video_profile_type profile,
                             video_fmt_brand_type brand,
                             uint32 target_bitrate_bps,
                             video_client_req_cb_func_ptr_type cb);


/*===========================================================================

FUNCTION video_encode_raw_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame for offline
  encoding. It is called every frame
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  inputBuf: data to be encoded
  frame_duration: Duration of frame
  width:  width of encoded frame
  height: height of encoded frame
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
 
===========================================================================*/
video_status_type 
video_encode_raw_frame(uint8 *inputBuf, uint32 frame_duration, 
                       uint16 width, uint16 height);



/*===========================================================================

FUNCTION video_encode_raw_init

DESCRIPTION
  This function is used to initialize the offline encoder driver.  
  This can only be called in VIDEO_NOT_INIT state.
  The state transitions to VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_init_type* init_data:  Used to init the driver and dsp 
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_raw_init(video_enc_init_type* init_data,
                            video_cb_func_ptr_type cbk_ptr, 
                            void* client_data);
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_start

DESCRIPTION
  This function is used to start the lossless encoding.  
  This function can be called from VIDEO_INIT or VIDEO_STOPPED state
  The state transitions to VIDEO_ENCODING
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 cb_ptr: Used every frame to send data to the video engine
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_start(video_client_req_cb_func_ptr_type cb_ptr, 
                                     const void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_request_i_frame

DESCRIPTION
  This function is used to request an I Frame in the near future.
  This function can only be called in the VIDEO_ENCODING_STARTED state
  (i.e when recording)
  The state remains the same after this function call.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_request_i_frame(video_cb_func_ptr_type cbk_ptr, 
                                               void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_stop

DESCRIPTION
  This function is used to stop the lossless encoding.  
  This function can be called all states except VIDEO_EXIT
  The state transitions to VIDEO_STOPPED
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Function to call after stopping
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_stop(video_cb_func_ptr_type cb_ptr,
                                    void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_exit

DESCRIPTION
  This function is used to exit the driver.
  The exit can be called in any state.
  The state transitions to VIDEO_EXIT
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Function to call after exiting
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_exit(video_cb_func_ptr_type cb_ptr, 
                                    void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_config

DESCRIPTION
  This function is used to configure the driver parameters.
  It can be called numerous times.  
  It can only be called in the VIDEO_INIT state.
  The state remains at VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_profile_type profile: Configure the profile to use in encoding
 video_cb_func_ptr_type cb_ptr: Function to call after config is complete
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_config(video_profile_type profile, 
                                      video_cb_func_ptr_type cb_ptr, 
                                      void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_get_config

DESCRIPTION
  This function is used to get the current Video Driver Configuration
  This function can be called only in the VIDEO_INIT or VIDEO_ENCODING_STARTED
  state and it doesnt change the state machine.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* cfg_ptr:  The output cfg ptr
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_get_config
(
  video_enc_config_type   *cfg_ptr, 
  void                    *param,
  video_cb_func_ptr_type  cbk_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_set_config

DESCRIPTION
  This function is used to set the Video Driver Configuration
  It should only be used when the Caller has intimate knowledge 
  of the internals ofthe Video Encoder or when recommended by Qualcomm.
  This function can be called only in the VIDEO_INIT state and it doesnt
  change the state machine.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* new_cfg:  The new configuration to use.
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_set_config
(
  video_enc_config_type   *new_cfg_ptr, 
  void                    *param,
  video_cb_func_ptr_type  cbk_ptr,
  void                    *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_dynamic_set_config

DESCRIPTION
  This function is used to set the Video Driver Configuration
  It should only be used when the Caller has intimate knowledge 
  of the internals ofthe Video Encoder or when recommended by Qualcomm.
  This function can be called in VIDEO_INIT or VIDEO_ENCODING_STARTED state.
  It doesnt change the state machine.
  It will only change the following parameters: 
    quantizer_scale_intra, quantizer_scale_inter, quant_precision,
    intra_period, target_bitrate, rc_window 
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* new_cfg:  The new configuration to use.
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_dynamic_set_config
(
  video_enc_dynamic_config_type *new_cfg_ptr, 
  void                          *param,
  video_cb_func_ptr_type        cbk_ptr,
  void                          *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame
  It is called every frame.  it assumes bit streams have been initialized and all
  relevant headers have been written.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_ERROR    otherwise
      
SIDE EFFECTS
  None
  
AUTHOR
  Suhail Jalil
  
MODIFIED
  11/14/02  Created
  
===========================================================================*/
void video_encode_frame(uint8* bufPtr);   /* called by frame isr to do encode */

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_get_decoder_specific_info

DESCRIPTION
  This function is used to get the decoder specific info.
  This function can be called only in the VIDEO_INIT 
  state and it doesnt change the state machine.
  The ptr to the decoder specific information (buf_ptr) is valid
  until video_encode_start is called.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  buf_ptr: Ptr to the decoder specific info.
  len:     Number of Bytes of decoder specific info.
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_get_decoder_specific_info(uint32** buf_ptr,
                                    uint32* len, 
                                    video_cb_func_ptr_type cbk_ptr,
                                    void* client_data);

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_permitted

DESCRIPTION
  This function is used to indicate if the current frame is permitted to 
  be encoded.  A value of true means that the frame should be encoded,
  a value of false means that the frame should not be encoded.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None

RETURN VALUE
  boolean: true means encode frame, false means drop the frame.
  
===========================================================================*/
boolean video_encode_permitted (VideoBufferHandle *bufferHandle);


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_swap

DESCRIPTION
  This function swaps the encode Ping-pong buffer. Encoding is done using
  pong-pong buffer to enable parallel processing and to keep track of 
  prediction frame.  Normally the BackEnd is used to keep track of the
  ping-pong buffers. However, in certain cases, as in the case of frame
  dropping due to rate control, we might need to enforce a swap on top of
  the swapping already done by qdsp.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None

RETURN VALUE
  VIDEO_SUCCESS
  VIDEO_FAILURE
  
===========================================================================*/
video_status_type video_encode_swap (void);
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_update_filter

DESCRIPTION
  This function is used to indicate if the video encoder driver has updated
  the VFE 3x3 luma filter. A return value of TRUE means that the argument
  has been updated with the latest filter coefficients and that VFE update
  should be sent at the next possible instance. A return value of FALSE means
  that no filter is necessary.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None

RETURN VALUE
  boolean: true means update filter, false means no update needed.
  
===========================================================================*/
boolean video_encode_update_filter (video_enc_3x3_luma_filter_type *filter);


#ifdef FEATURE_MP4_TRANSCODER

/*===========================================================================

FUNCTION video_offline_encode_free_buffer

DESCRIPTION
  This function is used to free the buffers allocated for
  offline encoding.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    

ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
void video_offline_encode_free_buffer(void);
#endif


#if defined(FEATURE_AAC) && defined(FEATURE_AAC_TRANSCODER)
/*===========================================================================*
 *           MACROS                                                          *
 *===========================================================================*/
#define     Q11_SHIFT   11
#define     Q15_SHIFT   15
#define     Q18_SHIFT   18
#define     MAX_Q15     32767
#define     MIN_Q15     -32768
#define     MAX_FREQ    48000

#define  SLP_UP_SAMP_FMEM_LEN       4
#define     int40       __int64
#define   SCRA_MEM_LEN   512

#define   ONE_OV_SIX_Q15          (uint16)(1.0/6.0*(1<<Q15_SHIFT)+1)
#define   ONE_OV_THR_Q15          (uint16)(1.0/3.0*(1<<Q15_SHIFT)+1)
#define   TWO_OV_THR_Q15          (uint16)(2.0/3.0*(1<<Q15_SHIFT)+1)

static     int16 const dnSampCoefVec[] = 
    {   /* down-sampler filter coefficients, different from first release */
 
    4,     4,     4,     4,     3,     2,     0,     -2,
    -4,   -7,    -9,   -11,   -12,   -13,   -12,    -11,
    -8,   -5,     0,     5,    11,    17,    22,     26,
    29,   29,    28,    24,    18,    10,     0,    -11,

    -23, -34,   -44,   -51,   -56,   -57,   -54,    -46,
    -34, -19,     0,    20,    41,    61,    78,     92,
    99,  100,    94,    80,    59,    32,     0,    -34,
    -69,-102,  -130,  -151,  -163,  -163,  -152,   -129,

    -95, -51,     0,    55,   110,   161,   204,    236,
    253, 254,   235,   199,   146,    78,     0,    -83,
    -167,-244, -308,  -355,  -380,  -379,  -351,   -296,
    -216,-115,    0,   123,   245,   357,   451,    519,

    554,  552,  510,   429,   313,   167,     0,   -177,
    -353,-514, -649,  -745,  -795,  -791,  -730,   -614,
    -448,-239,    0,   253,   504,   735,   927,   1065,
    1137,1132, 1046,   880,   642,   343,     0,   -365,

    -727,-1061,-1342,-1545, -1653, -1650, -1529,  -1291,
    -945,-506,    0,   543,  1088,  1597,  2031,   2353,
    2534,2547, 2380,  2026,  1496,   810,     0,   -889,
    -1804,-2685,-3467,-4087,-4483, -4602, -4399,  -3843,

    -2920,-1633,  0,  1939,  4129,  6501,  8974,  11456,
    13856,16079,18036,19649,20852, 21594, 21845, 21594,
    20852,19649,18036,16079,13856, 11456,  8974,   6501,
    4129,  1939,   0, -1633,-2920, -3843, -4399,  -4602,

    -4483,-4087,-3467,-2685,-1804,  -889,     0,    810,
    1496,  2026, 2380, 2547, 2534,  2353,  2031,   1597,
    1088,   543,    0, -506, -945, -1291, -1529,  -1650,
    -1653,-1545,-1342,-1061, -727,  -365,     0,    343,

    642,    880, 1046, 1132, 1137,  1065,   927,    735,
    504,    253,    0, -239, -448,  -614,  -730,   -791,
    -795,  -745, -649, -514, -353,  -177,     0,    167,
    313,    429,  510,  552,  554,   519,   451,    357,

    245,    123,    0, -115,  -216,  -296,  -351,   -379,
    -380,  -355, -308, -244,  -167,   -83,     0,     78,
    146,    199,  235,  254,   253,   236,   204,    161,
    110,     55,    0,  -51,   -95,  -129,  -152,   -163,

    -163,  -151, -130, -102,   -69,   -34,     0,     32,
    59,      80,   94,  100,    99,    92,    78,     61,
    41,      20,    0,  -19,   -34,   -46,   -54,    -57,
    -56,    -51,  -44,  -34,   -23,   -11,     0,     10,

    18,      24,   28,   29,    29,    26,    22,     17,
    11,       5,    0,   -5,    -8,   -11,   -12,    -13,
    -12,    -11,   -9,   -7,    -4,    -2,     0,      2,
    3,        4,    4,    4,     4,
   
    };

#define    DN_SAMP_FILT_LEN       (sizeof(dnSampCoefVec)/sizeof(int16))
#define    DN_SAMP_FMEM_LEN       ((DN_SAMP_FILT_LEN+1)>>1)

/* Resampler */
typedef struct
{
    uint16    inFreq;           /* input frequency, used as phase 
                                 * update step size */
    uint16    outFreq;          /* output frequency, used as phase 
                                 * limiter */
    uint16    phaseConst;       /* constant used to evaluate
                                 * fractional phase */
    uint16    shiftConst;       /* shift constant used to evaluate
                                 * fractional phase */
    uint32    curPhase;         /* current phase, needs to be evaluated
                                 * with formula curPhase/outFreq. */                                      
    uint16    filtMemIndex;     /* index to filter memory */
    int16     filtMem[SLP_UP_SAMP_FMEM_LEN];     
                                /* filter memory */
}   typeSlpUpSampStruct;


typedef struct 
{
    /* up-sampler's structure.  The down-sampler reusesup-sampler to 
     * up-sample input signal to 2, 3, 4, 6 times output frequency, 
     * then down-sample to desired output frequency */
    typeSlpUpSampStruct slpUpSampStruct;


    int16    maxInBufLen;      /* used to determine the maximum length of
                                * input buffer that won't overflow inter-
                                * mediate output buffer.*/
    int16    dnSampFactor;     /* down sample factor, used to determine
                                * actual down-sampling rate */
    int16    dnSampStep;       /* down sample filter coefficient incre-
                                * ment step size */
    int16    filtMemConvIndex; /* index to filter memory for convolution 
                                * purpose */
    int16    filtMemLoadIndex; /* index to filter memory for load purpose*/
    int16    filtMem[DN_SAMP_FMEM_LEN]; 
                               /* filter memory used for down-sample filter
                                * in circular buffer form */
} typeDnSampStruct;
/*===========================================================================* 
FUNCTION NAME  : frac_div()  
 
DESCRIPTION    : Compute fractional division, output rounded quotient in  
               : Qn format.                                                

INPUT ARGUMENTS:                                                           
       num        : Numerator.                                                
       den        : Denominator.                                              
       Qfac       : Q-factor.                                                 
   RETURN VALUE   : Quotient.                                                 
   COMMENTS       : It can be replaced by DSP function that provide similar   
                  : functionality.                                            
 *===========================================================================*/
uint32 frac_div(uint32 num, uint32 den, uint16 Qfac);

/*===========================================================================* 
   FUNCTION NAME  : int_div()                                                
   DESCRIPTION    : Compute integer division, output quotient as well as      
                  : residual.                                                 
   INPUT ARGUMENTS:                                                           
       num        : Numerator.                                                
       den        : Denominator.                                             * 
       rem        : Pointer to remainder.                                    * 
   RETURN VALUE   : Quotient.                                                * 
   COMMENTS       : Pointer rem needs to point to some solid physical        * 
                  : address.  Otherwise, there could be memory overriding    * 
                  : problem.  This subroutine can be replace by simular      * 
                  : function in existing fixed point library.                * 
 *===========================================================================*/
uint16 int_div(uint32 num, uint32 den, uint16* rem);

/*===========================================================================* 
 * FUNCTION NAME  : init_slp_up_sampler()                                    * 
 * DESCRIPTION    : Initialize super low power up-sampler data structure.    * 
 * INPUT ARGUMENTS:                                                          * 
 *     inFreq     : Input frequency, 16 bit unsigend.                        * 
 *     outFreq    : Output frequency, 16 bit unsigned.                       * 
 *     ptrSlpUpSampStruct: Pointer to super low power up-sampler data        *
 *                : structure                                                * 
 * RETURN VALUE   :                                                          *  
 *     0          : If input frequency is greater than output frequency      * 
 *                : or output frequency is greater than 48kHz                * 
 *     1          : If input frequency == output frequency.                  * 
 *     outFreq    : Otherwise                                                * 
 * COMMENTS       : The up-sample data structure will carry all information  * 
 *                : needed to do upsample conversion.  Output frequency is   * 
 *                : kept to serve as phase cap.  Shift constant fractional   * 
 *                : constant are generated here.  Step sizes for  phase      * 
 *                : update are also generated                                * 
 *===========================================================================*/
uint16  init_slp_up_sampler(uint16  inFreq, uint16  outFreq, typeSlpUpSampStruct *ptrSlpUpSampStruct);

/*===========================================================================* 
 * FUNCTION NAME  : init_down_sampler()                                      * 
 * DESCRIPTION    : Initialize down-sampler data structure.                  * 
 * INPUT ARGUMENTS:                                                          * 
 *     inFreq     : Input frequency, 16 bit unsigend.                        * 
 *     outFreq    : Output frequency, 16 bit unsigned.                       * 
 *     ptrDnSampStruct: Pointer to down-sampler data structure               * 
 * RETURN VALUE   :                                                          * 
 *     0          : If inFreq > 48000 or inFreq < outFreq or the up-sampling * 
 *                : rate is not suppoted or inFreq > outFreq * 6             * 
 *     1          : outFreq == inFreq;                                       * 
 *     medFreq    : Otherwise.                                               * 
 * SIEDE EFFECT   : Anything inside the output buffer will be overwitten.    * 
 *                : The phase information inside the up-sample structure will* 
 *                : be fully updated.                                        * 
 * DEPENDENCIES   : The caller needs to make sure the output buffer is big   * 
 *                : enough.  The up-sample structure needs to be initialized * 
 *                : and updated up-to-date.                                  * 
 * COMMENTS       : The down-sample data structure will carry all information* 
 *                : needed to do down-sample conversion.  Absolute I/O       * 
 *                : frequency will not be kept.  Only relative information   * 
 *                : are kept                                                 * 
 *===========================================================================*/
uint16   init_down_sampler(uint16  inFreq, uint16  outFreq,typeDnSampStruct *ptrDnSampStruct);

/*===========================================================================* 
 * FUNCTION       : sat_Q15()                                                * 
 * DESCRIPTION    : Saturation operation to a Q15 number                     * 
 * INPUT ARGUMENTS:                                                          * 
 *     in         : Input data                                               * 
 *     satMax     : uppper threshold                                         * 
 *     satMin     : lower threshold                                          * 
 * RETURN VALUE   : Data after saturation operation                          * 
 * COMMENTS       : This is for csim only.  The DSP will take care of satu-  * 
 *                : ration automaticall.                                     * 
 *===========================================================================*/
int16 sat_Q15(int40 in, int16 satMax, int16 satMin);

/*===========================================================================* 
 * FUNCTION NAME  : slp_up_sampler()                                         * 
 * DESCRIPTION    : Implementing super low power up-sample algorithm         * 
 * INPUT ARGUMENTS:                                                          * 
 *     numInSamps : Number of input samples, 16 bit unsigned                 * 
 *     ptrInBuf   : Pointer to input buffer, data in buffer: 16 bit signed   * 
 *     ptrOutBuf  : Pointer to output buffer, data in buffer: 16 bit signed  * 
 *     ptrSlpUpSampStruct: Pointer to super low power up-sampler data        * 
 *                : structure                                                * 
 * RETURN VALUE   : Number of output samples.  It varies from frame to frame.* 
 * SIEDE EFFECT   : Anything inside the output buffer will be overwitten.    * 
 *                : The phase information inside the up-sample structure will* 
 *                : be fully updated.                                        * 
 * DEPENDENCIES   : The caller needs to make sure the output buffer is big   * 
 *                : enough.  The up-sample structure needs to be initialized * 
 *                : and updated up-to-date.                                  * 
 * COMMENTS       : It will stop conversion when the last input data is      * 
 *                : loaded in filter memory.  The filter memory may still be * 
 *                : capable to generate output data.  However, the algorithm * 
 *                : will wait until next frame of data is available.  Will by*
 *                : pass if input freq == output freq.                       * 
 *===========================================================================*/
uint16  slp_up_sampler(uint16  numInSamps,int16   *ptrInBuf,int16   *ptrOutBuf,typeSlpUpSampStruct *ptrSlpUpSampStruct);

/*===========================================================================* 
 * FUNCTION NAME  : down_by_n()                                              * 
 * DESCRIPTION    : Implementing down-sample by n algorithm.  The actual down* 
 *                : sample rate is determined by dnSampFactor and dnSampStep * 
 *                : elements in typeDnSampStruct.  The dnSampFactor element  * 
 *                : determines actual down sample rate.  The dnSampStep ele- * 
 *                : ment determines subsample interval in anti-aliasing fil- * 
 *                : ter.                                                     * 
 * INPUT ARGUMENTS:                                                          * 
 *     numInSamps : Number of input samples, 16 bit unsigned                 * 
 *     ptrInBuf   : Pointer to input buffer, data in buffer: 16 bit signed   * 
 *     ptrOutBuf  : Pointer to output buffer, data in buffer: 16 bit signed  * 
 *     ptrUpSampStruct: Pointer to down-sampler data structure               * 
 * RETURN VALUE   : Number of output samples                                 * 
 * COMMENTS       : The down-sampler will load the data to filter memory to  * 
 *                : make sure filter memory is updated for next convolution. * 
 *                : If the input buffer is empty before the filter memory is * 
 *                : ready for convolution, this function will return.  Then, * 
 *                : convolution is computed.  Two separate index are used to * 
 *                : indicate data load and convolution start location.       * 
 *===========================================================================*/
uint16  down_by_n(uint16  numInSamps,int16*  ptrInBuf,int16*  ptrOutBuf,typeDnSampStruct *ptrDnSampStruct);

/*===========================================================================* 
 * FUNCTION NAME  : down_sampler()                                           * 
 * DESCRIPTION    : Implementing down-sample algorithm                       * 
 * INPUT ARGUMENT :                                                          * 
 *     numInSamps : Number of input samples, 16 bit unsigned                 * 
 *     ptrInBuf   : Pointer to input buffer, data in buffer: 16 bit signed   * 
 *     ptrOutBuf  : Pointer to output buffer, data in buffer: 16 bit signed  * 
 *     ptrUpSampStruct: Pointer to down-sampler data structure               * 
 * RETURN VALUE   : Number of output samples.  It varies from frame to frame.* 
 * COMMENTS       : It will first up-sample input data to intermediate freq- * 
 *                : uency as specified in up-sampler's structure.  Then, it  * 
 *                : down-samples to desired output frequency as specified in * 
 *                : in down-sampler's structure.  Only one convolution is    * 
 *                : needed for each output sample.  No linear interpolation  * 
 *                : is needed.  The down-sampler will first split up input   * 
 *                : buffer to avoid overflow of intermediate output buffer.  * 
 *                : The input buffer is splitted based on the size of inter- * 
 *                : mediate buffer as well as conversion ratio.              * 
 *===========================================================================*/
uint16  down_sampler(uint16  numInSamps,int16*  ptrInBuf,  int16*  ptrOutBuf, typeDnSampStruct *ptrDnSampStruct);
#endif /*defined(FEATURE_AAC) && defined(FEATURE_AAC_TRANSCODER)*/
#endif /* FEATURE_VIDEO_ENCODE */
#endif /* VIDEO_ENC_H */

