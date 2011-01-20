#ifndef GRAPH_H
#define GRAPH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  G R A P H I C S  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the definitions necessary to interface to the
  various image, graphics, and games features.


Copyright(c) 2001 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/graph.h#27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/06   Pavan   Support added for Moviedit Interface
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency 
                   When qcamcorder Feature Disabled 
03/27/06   lip     Add SVGT 1.2 and FEATURE_SVGDOM support.
11/03/05   nrw     Changed type of argument to graph_queue_pnge_msg() from
                   graph_png_request_type to PNGE_status_enum_type to match
                   cb parameters.
05/27/05    dw     Added raw mode support for video encoding.
02/24/05    lg     Added support for SVG
11/04/04   ark     Updated graph_jpeg_decode() API with new parameters.
09/01/04   pdo     Added new CONTEXT_REQ for 6550 JPEG encoder
04/19/04   pdo     Updated graph_jpeg_decode() API with new parameters. 
                   Provided needed interfaces to synchronize the JPEG 
                   async. data request. 
04/08/04    st     Removed JPEG encoder software, this is no longer supported
                   through GRAPH layer.
03/22/04    st     Updated graph_png_set_buf(), graph_jpeg_set_buf(),
                   graph_saf_global_init() function prototypes.
02/12/04    jz     Added Registration and De-registration for QVP Video Encoder
12/01/03   pdo     Added supports for PNG encoder. 
11/21/03    aw     Added supports for QGLES dsp services.
09/16/03   dle     Expanded parameter list for graph_queue_camera_func().
05/13/03    aw     Added return status for Q3D functions prototype. 
05/13/03    dle    Renamed vod_buffer to buf.
03/18/03    aw     Modified code to reuse some of the Q3D functions.
02/20/03    aw     Added support for Q3D engine.
02/17/03    sa     Added functions for the video encoder when 
                   FEATURE_VIDEO_ENCODE is enabled.
09/30/02    dle    Name change for Camera.
09/19/02    dle    Merged from 1.2.1 branch.
08/29/02    dle    Added functions for FEATURE_CAMERA.
09/09/02    ny     Removed code for unsupported TEST_MPEG4_QDSP_INTERFACE and 
                   CODEC_INTERFACE testing modes.Removed dependency on mp4graph.h 
                   by moving all MPEG 4 function prototypes to mp4graph.h
08/21/02    ny     Replaced mp4qdsp_drv.h with mp4graph.h
07/22/02    sa     Removed graph_send_video_data_request_cmd() function
                   definition.
06/21/02    sa     Added support for MPEG-4 decoder.
06/12/02    sa     Changed png.h to cmx_png.h to reflect name change.
04/02/02    sa     Featurized referenced header files for PNG, SAF, and JPEG.
03/22/02    sa     Added support for JPEG encoder and decoder.
08/09/01    aw     Create file.


===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "comdef.h"     /* Customer specific definitions                   */

#ifdef FEATURE_GRAPH_TASK
#ifdef FEATURE_PNG_DECODER
#include "cmx_png.h"    /* PNG prototypes                                  */
#endif
#ifdef FEATURE_PNG_ENCODER
#include "pnge.h"
#endif
#ifdef FEATURE_SAF
#include "saf.h"        /* SAF prototypes                                  */
#endif
#ifdef FEATURE_JPEG_DECODER
#include "jpeg.h"       /* JPEG prototypes                                 */
#endif
#ifdef FEATURE_SVG
#include "svg.h"       /* SVG prototypes                                   */
#endif

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_I3D
/* Q3D Graphics status type is returned in callback functions to indiacate
** event and return data to client 
*/
typedef enum {
  GRAPH_Q3D_FRAME_START,       /* Frame start rendering                   */
  GRAPH_Q3D_RENDER_DONE,       /* Render process is done                  */
  GRAPH_Q3D_FRAME_DONE,        /* Frame updated                           */
  GRAPH_Q3D_FRAME_ERROR,       /* Error generate frame                    */
  GRAPH_Q3D_FRAME_ABORT,       /* Current render frame process is aborted */
} graph_q3d_frame_status_type;

typedef enum {
  GRAPH_Q3D_ERROR,             /* Error occured with requested command    */
  GRAPH_Q3D_DONE,              /* Requested command has been carried out  */
  GRAPH_Q3D_SUCCESS,           /* Requested command was accepted          */
  GRAPH_Q3D_FAILURE,           /* Error occured with requested command    */
} graph_q3d_status_type;

/* Client status callback function type */
typedef void (* graph_q3d_status_cb_func_type) (
  graph_q3d_status_type status,
  void                  *data
);

/* Client frame status callback function type */
typedef void (* graph_q3d_frame_status_cb_func_type) (
  graph_q3d_frame_status_type status,
  void                        *data
);

/* Q3D init function type */
typedef void (* graph_q3d_init_func_ptr_type) (
  graph_q3d_status_cb_func_type cb_func,
  void                          *client_data
);

/* Q3D draw model function type */
typedef void (* graph_q3d_draw_model_func_ptr_type) (
  void                 *model,
  void                 *ctx
);

/* Q3D render function type */
typedef void (* graph_q3d_render_func_ptr_type) ( 
  void                                *ctx, 
  graph_q3d_frame_status_cb_func_type cb_func,
  void                                *client_dta
);

/* Q3D model release function type */
typedef void (* graph_q3d_release_func_ptr_type) (
  void                          *data,
  graph_q3d_status_cb_func_type cb_func,
  void                          *client_data
);

/* Q3D qdsp command function type */
typedef void (* graph_q3d_qdsp_cmd_func_ptr_type) ( uint16 msg);
#endif /* FEATURE_I3D */

#ifdef FEATURE_GRAPHICS_OPENGLES_CL
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_CL */

/* General context switch for 2 parameters function callback */
typedef void (* graph_context_req_func_type) 
             ( int32 client_ID, int32 client_data );

typedef struct 
{
  graph_context_req_func_type   function_ptr;
  int32                         client_ID;
  int32                         client_data;
} graph_context_req_type;


typedef enum {
  GRAPH_PNG_CONTINUE,          /* PNG requests to continue      */
  GRAPH_PNG_CB_DONE,           /* PNG encode done notification  */
  GRAPH_PNG_CB_ABORT           /* PNG encode abort notification */
} graph_png_request_type;

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
/* Video Encoder 
*/
typedef void ( * video_encode_func_type)(uint8* buf_ptr);
#endif /* FEATURE_VIDEO_ENCODE  || FEATURE_QVPHONE*/

/* <EJECT> */
/*---------------------------------------------------------------------------
** Graphics Public Function Prototypes
**---------------------------------------------------------------------------
*/
/* <EJECT> */
/*===========================================================================

FUNCTION graph_task

DESCRIPTION
  This procedure is the entrance procedure for the GRAPHIC
  Task.  It contains the main processing loop for the GRAPHIC Task.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_tcb.

===========================================================================*/
extern void graph_task
(
  dword ignored
);

#ifdef FEATURE_PNG_ENCODER
/* <EJECT> */
/*===========================================================================

FUNCTION graph_queue_png_request

DESCRIPTION
  This function request a process using graphic task.

DEPENDENCIES
  FEATURE_PNG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_queue_pnge_msg( PNGE_status_enum_type msg );

/*===========================================================================

FUNCTION graph_png_encode

DESCRIPTION
  This function issues a command in graphic task to encode a PNG image.

DEPENDENCIES
  FEATURE_PNG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_encode( void );

#endif /* FEATURE_PNG_ENCODER */

/*===========================================================================

FUNCTION graph_context_req

DESCRIPTION
  This function perform context switch to Graphic task

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_context_req
(
  graph_context_req_type context_data
);

#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION graph_png_decode

DESCRIPTION
  This function issues a command in graphic task to decode PNG image.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_decode
(
  void                        *handle,
  png_cmx_decode_cb_func_type callback_ptr
);


/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_decode_abort

DESCRIPTION
  This function issues a command in graphic task to abort PNG decode.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_decode_abort (void);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_get_image_specs

DESCRIPTION
  This function issues a command to pass the PNG image width and height,
  bit depth, color type, compression method, filter method, and interlace
  method.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_get_image_specs
(
  void                        *handle,
  png_cmx_decode_cb_func_type callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_set_buf

DESCRIPTION
  This function issues a command to set PNG internal buffers to client
  supplied buffers.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_set_buf
(
  uint8 *client_buffer,                     /* client buffer               */
  uint32 client_buffer_size,                /* size of client buffer       */
  uint8 *output_buffer,                     /* output buffer               */
  uint32 output_buffer_size                 /* size of output buffer       */
);

#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION graph_jpeg_decode

DESCRIPTION
  This function issues a command in graphic task to decode JPEG image.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_decode
(
  void                        *handle,
  jpeg_cmx_cb_func_type       callback_ptr,
  jpeg_decodeInfo_type        jpegdInfo
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_dec_qdsp_ready

DESCRIPTION
  This function is called when JPEG decode image is ready.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_dec_qdsp_ready( void );

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_dec_image_abort

DESCRIPTION
  This function is called when JPEG decode image has been aborted.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_dec_image_abort( void );

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_get_image_specs

DESCRIPTION
  This function issues a command to pass the JPEG image width and height,
  bit depth.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_get_image_specs
(
  void                        *handle,
  jpeg_cmx_cb_func_type       callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_abort_decode

DESCRIPTION
  This function issues a command in graphic task to abort JPEG decode.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_abort_decode (void);

 /*===========================================================================

FUNCTION GRAPH_GET_ASYNC_DATA

DESCRIPTION

  This operation waits till data is received for asynchronous data callbacks.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if data is received before 3 secs
  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean graph_get_async_data
( 
  jpeg_cmx_status_enum_type jpeg_status,
  void                     *handle,
  void                     *server_data
);

#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_ENCODER
/*===========================================================================

FUNCTION graph_jpeg_encode

DESCRIPTION
  This function issues a command in graphic task to encode JPEG image.

DEPENDENCIES
  FEATURE_JPEG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_encode
(
  cmx_jpeg_spec_type          image_spec,
  void                        *handle,
  jpeg_cmx_cb_func_type       callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_enc_qdsp_ready

DESCRIPTION
  This function is called when JPEG encode image is ready.

DEPENDENCIES
  FEATURE_JPEG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_enc_qdsp_ready( void );

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_enc_image_abort

DESCRIPTION
  This function is called when JPEG encoder image has been aborted.

DEPENDENCIES
  FEATURE_JPEG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_enc_image_abort( void );

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_abort_encode

DESCRIPTION
  This function issues a command in graphic task to abort JPEG encode.

DEPENDENCIES
  FEATURE_JPEG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_abort_encode ( void );

#endif /* FEATURE_JPEG_ENCODER */

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_set_buf

DESCRIPTION
  This function issues a command to set JPEG internal buffers to client
  supplied buffers.

DEPENDENCIES
  FEATURE_JPEG_DECODER
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_set_buf
(
  uint8  *client_buffer,
  uint32  client_buffer_size,
  uint8   *output_buffer                    /* output buffer               */
);

#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION graph_saf_decode

DESCRIPTION
  This function issues a command to play a SAF animation in graphic
  task.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_saf_decode
(
  void                            *handle,
  saf_cmx_decode_cb_func_ptr_type callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_get_info

DESCRIPTION
  This function issues a command in graphic task to pass specifications
  for the animation contained in the SAF file.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_saf_get_info
(
  void                            *handle,
  saf_cmx_decode_cb_func_ptr_type callback_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_decode_abort

DESCRIPTION
  This function issues a command in graphic task to abort the
  animation playback.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_saf_decode_abort(void);

/* <EJECT> */
/*===========================================================================

FUNCTION  graph_saf_decode_stop

DESCRIPTION
  This function issues a command in graphic task to to stop SAF decoding

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_saf_decode_stop(void);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_global_init

DESCRIPTION
  This function issues a command in graphic task to initial the SAF decoder
  global data/buffers.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_saf_display_info_data.

===========================================================================*/
extern void graph_saf_global_init
(
  saf_display_resource_info_type *display_info_data,
  uint8                          *g_object_buf,
  uint32                         g_object_buf_size,
  uint8                          *g_coder_buf,
  uint32                         g_coder_buf_size,
  uint8                          *g_info_buf
);

#endif /* FEATURE_SAF */

#ifdef FEATURE_CAMERA
/* <EJECT> */
/*===========================================================================

FUNCTION graph_queue_camera_func

DESCRIPTION
  This function queues camera function

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_camera_func
(
  uint32 func,
  void *callback,
  void *client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
);

/*===========================================================================

FUNCTION graph_queue_camqdsp_msg

DESCRIPTION
  This function queues CAMQDSP message

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_camqdsp_msg
(
  uint32 event,
  void * buf,
  uint32 size
);
#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

FUNCTION graph_queue_offline_encode_qdsp_msg

DESCRIPTION
  This function queues CAMQDSP message

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

extern void graph_queue_offline_encode_qdsp_msg
(
  uint32 msg,
  void * buf,
  uint32 size
);
#endif
#endif /* FEATURE_CAMERA */

#ifdef FEATURE_VIDEO_ENCODE_RAW
/*===========================================================================

FUNCTION graph_queue_raw_encode_qdsp_msg

DESCRIPTION
  This function queues CAMQDSP message

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_raw_encoder_qdsp_msg
(
  uint32 msg,
  void * buf,
  uint32 size
);

/*===========================================================================

FUNCTION graph_queue_raw_encode_video_engine_msg

DESCRIPTION
  This function queues Video Engine message for raw encoding

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_raw_encoder_video_engine_msg
(
  uint32 status,
  void * client_data
);
#endif /* FEATURE_VIDEO_ENCODE_RAW */


#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
/*===========================================================================

FUNCTION graph_video_enc_frame_done

DESCRIPTION
  This function processes the encode video frame command

DEPENDENCIES
  FEATURE_VIDEO_ENCODE.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_video_enc_frame_done
(
  uint8 *data_buf
);
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE*/

/* <EJECT> */
#ifdef FEATURE_VIDEO_MOVIEEDIT
/*===========================================================================

FUNCTION graph_queue_movieedit_func

DESCRIPTION
  This function queues a video file parsing function

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_movieedit_func
(
  uint32  func,
  void   *callback,
  void   *client_data,
  uint32  info1,
  uint32  info2,
  uint32  info3,
  uint32  info4
);
#endif /* FEATURE_VIDEO_MOVIEEDIT */
#ifdef FEATURE_I3D
/*===========================================================================

FUNCTION graph_q3d_init

DESCRIPTION
  This function queue q3d init function.

DEPENDENCIES
  FEATURE_I3D
  FEATURE_GRAPH_TASK.

RETURN VALUE
  GRAPH_Q3D_SUCCESS - Command is queued up successfully
  GRAPH_Q3D_FAILURE - Command is not queued up.

SIDE EFFECTS
  None.

===========================================================================*/
extern graph_q3d_status_type graph_q3d_init
(
  graph_q3d_status_cb_func_type cb_func,
  void                          *client_data,
  graph_q3d_init_func_ptr_type  func_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_draw_model

DESCRIPTION
  This function queue q3d draw model function.

DEPENDENCIES
  FEATURE_I3D
  FEATURE_GRAPH_TASK.

RETURN VALUE
  GRAPH_Q3D_SUCCESS - Command is queued up successfully
  GRAPH_Q3D_FAILURE - Command is not queued up.

SIDE EFFECTS
  None.

===========================================================================*/
extern graph_q3d_status_type graph_q3d_draw_model
(
  void                               *model,
  void                               *ctx,
  graph_q3d_draw_model_func_ptr_type func_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_render

DESCRIPTION
  This function queue up q3d render function.

DEPENDENCIES
  FEATURE_I3D
  FEATURE_GRAPH_TASK.

RETURN VALUE
  GRAPH_Q3D_SUCCESS - Command is queued up successfully
  GRAPH_Q3D_FAILURE - Command is not queued up.

SIDE EFFECTS
  None.

===========================================================================*/
extern graph_q3d_status_type graph_q3d_render
(
  void                                *ctx,
  graph_q3d_frame_status_cb_func_type cb_func,
  void                                *client_data,
  graph_q3d_render_func_ptr_type      func_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_release

DESCRIPTION
  This function queue up q3d relase function.

DEPENDENCIES
  FEATURE_I3D
  FEATURE_GRAPH_TASK.

RETURN VALUE
  GRAPH_Q3D_SUCCESS - Command is queued up successfully
  GRAPH_Q3D_FAILURE - Command is not queued up.

SIDE EFFECTS
  None.

===========================================================================*/
extern graph_q3d_status_type graph_q3d_release
(
  void                            *data,
  graph_q3d_status_cb_func_type   cb_func,
  void                            *client_data,
  graph_q3d_release_func_ptr_type func_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_qdsp_cmd

DESCRIPTION
  This function queue up q3d qdsp message

DEPENDENCIES
  FEATURE_I3D
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_q3d_qdsp_cmd
(
  uint16                           msg,
  graph_q3d_qdsp_cmd_func_ptr_type func_ptr
);
#endif /* FEATURE_I3D */

#ifdef FEATURE_GRAPHICS_OPENGLES_CL
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_CL */  

#ifdef FEATURE_QVPHONE
/*===========================================================================

FUNCTION graph_qvp_register

DESCRIPTION
  This function registers a QVP Encoder with the Graph Task

DEPENDENCIES

  NONE

RETURN VALUE
  True - Registered Successfully
  False - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean graph_qvp_register
(
  video_encode_func_type video_encoder
);

/*===========================================================================

FUNCTION graph_qvp_deregister

DESCRIPTION
  This function deregisters a QVP Encoder with the Graph Task

DEPENDENCIES

  NONE

RETURN VALUE

 None

SIDE EFFECTS
  None.

===========================================================================*/
void graph_qvp_deregister(void) ;
#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_SVG
/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_decode

DESCRIPTION
  This function issues a command to start SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_decode( 
  svg_load_data_type*  svg_load_data 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_frame_cmd

DESCRIPTION
  This function issues a command to render SVG frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_frame_cmd( 
  void* handle, 
  svg_frame_data_type* frame_data 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_process_event

DESCRIPTION
  This function issues a command to process users events for SVG file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_process_event( 
  void* handle,
  cmx_svg_event_enum_type event, 
  void* event_data, 
  uint32 time_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_stop

DESCRIPTION
  This function issues a command to stop SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_stop(
  void* handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_abort

DESCRIPTION
  This function issues a command to abort SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_abort( 
  void* handle 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_pause

DESCRIPTION
  This function issues a command to pause SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_pause( 
  void* handle 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_resume

DESCRIPTION
  This function issues a command to resume SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_resume( 
  void* handle 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_rewind

DESCRIPTION
  This function issues a command to rewind SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_rewind( 
  void* handle, 
  uint32 time_ms 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_fforward

DESCRIPTION
  This function issues a command to fast-forward SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_fforward( 
  void* handle, 
  uint32 time_ms 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_frame_reset

DESCRIPTION
  This function issues a command to reset Zoom and Pan factors for SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_frame_reset( 
  void* handle 
);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_set_viewport

DESCRIPTION
  This function issues a command to Set the Viewport of SVG Decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_set_viewport( 
  void* handle,
  uint32 width,
  uint32 height
);

#ifdef FEATURE_SVGDOM
/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_dom_apis

DESCRIPTION
  This function issues a DOM command to SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_dom_apis(
  void* handle,
  svg_dom_api_enum_type type,
  void* data
);
#endif /* FEATURE_SVGDOM */

#endif /* FEATURE_SVG */

#endif /* FEATURE_GRAPH_TASK */
#endif /* GRAPH_H */

