#ifndef GRAPHI_H
#define GRAPHI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            G R A P H I C S  I N T E R N A L  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal definitions to GRAPHIC decoder

Copyright(c) 2001 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/graphi.h#45 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   jv      Allow new graph packets to be dynamically allocated.
12/13/06   jv      Increased GRAPH_NUM_PACKETS from 30 to 60.
11/28/06   Pavan   Support added for new Movie Editing Interface
09/07/06   dhaval  Added transcoder definition through transcoder_task.h file
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency 
                   When qcamcorder Feature Disabled 
05/03/06   wyh     Removing GRAPH_M4V2RGB
05/02/06    ss     Changes to synchronize QVP recording control path with
                   encoder running in graph task context
03/27/06   lip     Add SVGT 1.2 and FEATURE_SVGDOM support.
05/27/05    dw     Added raw mode support for video encoding.
04/20/05    lg     Modified the SVG decode packet to add filename.
04/19/05    lg     Modified the SVG frame command to support seek and advance.
04/05/05    lg     Modified Graph packet type for SVG event processing.
02/24/05    lg     Added support for SVG
01/19/05   wyh     Support the graph-task component of video_eng. Featurized
                   under FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
01/11/05   pdo     Removed FEATURE_I3D around #include graph.h 
11/04/04   ark     Changed graph_jpeg_decode_packet_type structure to accomodate
                   graph_jpeg_decode() changes
09/01/04   pdo     Added new CONTEXT_REQ for 6550 JPEG encoder
04/19/04   pdo     Added new parameter for graph_jpeg_decode() API.
04/08/04    st     Removed JPEG encoder software, this is no longer supported
                   through GRAPH layer.
03/22/04    st     Updated structures graph_png_buf_packet_type,
                   graph_jpeg_buf_packet_type, graph_saf_init_packet_type.
03/02/04   ali     Changes for Offline encoder
12/01/03   pdo     Added supports for PNG encoder.
11/21/03    aw     Added supports for QGLES dsp services.
09/16/03    dle    Expanded parameter list for graph_camera_func_packet_type.
05/13/03    dle    Renamed vod_buffer to buf.
05/12/03    dle    Added CAMERA_TIMER_SIG.
03/18/03    aw     Increase the number of packets for Q3D engine. Modified
                   code to reuse some of the Q3D functions.
02/20/03    aw     Added support for Q3D engine.
02/17/03    sa     Added functions for the video encoder when
                   FEATURE_VIDEO_ENCODE is enabled.
09/26/02    dle    Modified graph_camqdsp_cb_packet_type.
09/19/02    dle    Merged from 1.2.1 brahcn.
08/29/02    dle    Added functions for FEATURE_CAMERA.
09/17/02    ny     Replaced GRAPH_MPEG4_WAIT_SIG with the actual 0x0008 signal value.
09/09/02    ny     Changed graph_mpeg4_qdsp_msg_type to take a Mp4DecodeDoneType instead
                   of a MP4QDSPMessageInfoType.  That is, hide all but the command of
                   interest
07/22/02    sa     Added signal definition GRAPH_MPEG4_SLICE_REQUEST_SIG.
                   Added function prototype graph_mpeg4_send_video_slice.
                   Renamed MPEG4 message type.
06/21/02    sa     Added support for MPEG-4 decoder.
06/12/02    sa     Changed png.h to cmx_png.h to reflect name change.
04/02/02    sa     Featurized referenced header files for PNG, SAF, and JPEG.
03/22/02    sa     Added support for JPEG encoder and decoder
08/09/01    aw     Create file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#ifdef FEATURE_GRAPH_TASK
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Definitions for queue services                  */
#include "graph.h"
#if defined(FEATURE_PNG_ENCODER) || defined(FEATURE_PNG_DECODER)
#include "cmx_png.h"    /* PNG prototypes                                  */
#endif
#ifdef FEATURE_SAF
#include "saf.h"        /* SAF prototypes                                  */
#endif
#ifdef FEATURE_JPEG_DECODER
#include "jpeg.h"       /* JPEG prototypes                                 */
#endif
#ifdef FEATURE_VIDEO_ENCODE
#include "video_enc.h"
#endif
#ifdef FEATURE_MP4_TRANSCODER
#include "transcoder_task.h"
#endif
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#include "videoengi.h"
#endif
#ifdef FEATURE_SVG
#include "svg.h"
#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
** GRAPH General Defines
---------------------------------------------------------------------------*/
/* This signal is set when the diag_rpt_timer expires.
*/
#define GRAPH_RPT_TIMER_SIG           0x0001

/* This signal is set when an item is put on the vs_cmd_q.
*/
#define GRAPH_CMD_Q_SIG               0x0002

#ifdef FEATURE_CAMERA
#define CAMERA_TIMER_SIG              0x0010
#endif /* FEATURE_CAMERA */

#ifdef FEATURE_JPEG_DECODER

/* This signal is used for synchronize the get data call to cmx */
#define JPEG_DECODE_DATA_SIG          0x0020

#endif

#ifdef FEATURE_VIDEO_MOVIEEDIT
/* This signal is set when movieedit gets a callback from video engine.
*/
#define VIDEO_MOVIEEDIT_SIG           0x2000   

/* This signal is set when movieedit waits for a callback from video engine.
*/
#define VIDEO_MOVIEEDIT_TIMEOUT_SIG   0x4000   
#endif /* FEATURE_VIDEO_MOVIEEDIT */

/* GRAPHIC Command Queue Available Packets.
*/
#define GRAPH_NUM_PACKETS 60    /* # of Free Packets Allocated             */

/* This signal is for MPEG 4 QDSP data request
*/

#ifdef FEATURE_PATHFINDER 
#error code not present
#endif /* FEATURE_PATHFINDER */

typedef enum {
#ifdef FEATURE_PNG_ENCODER
  GRAPH_PNGE_CMD,
  GRAPH_PNGE_MSG,
#endif /* FEATURE_PNG_ENCODER */
#ifdef FEATURE_PNG_DECODER
  GRAPH_PNG_CMD_DECODE,         /* GRAPHIC decode PNG image command        */
  GRAPH_PNG_CMD_ABORT,          /* Abort GRAPHIC decode PNG image command  */
  GRAPH_PNG_CMD_SPEC,           /* Get PNG image specifications command    */
  GRAPH_PNG_CMD_BUF,            /* Sets PNG internal buffers command       */
  GRAPH_PNG_CMD_CB,             /* PNG command call back                   */
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_JPEG_DECODER
  GRAPH_JPEG_CMD_DECODE,        /* GRAPHIC decode JPEG image command       */
  GRAPH_JPEG_CMD_SPEC,          /* Get JPEG image specifications command   */
  GRAPH_JPEG_CMD_ABORT_DECODE,  /* Abort GRAPHIC JPEG decode image command */
  GRAPH_JPEG_CMD_DEC_QDSP_READY, /* JPEG decoder QDSP image ready          */
  GRAPH_JPEG_CMD_DEC_IMAGE_ABORT,/* JPEG decoder QDSP image abort          */
  GRAPH_JPEG_CMD_BUF,           /* Sets JPEG internal buffers command      */
  GRAPH_JPEG_CMD_CB,            /* JPEG command call back                  */
#endif /* FEATURE_JPEG_DECODER */
#ifdef FEATURE_SAF
  GRAPH_SAF_CMD_DECODE,         /* CMX decode SAF image command            */
  GRAPH_SAF_CMD_ABORT,          /* Abort GRAPHIC decode SAF image command  */
  GRAPH_SAF_CMD_STOP,           /* Stop GRAPHIC decode SAF image command   */
  GRAPH_SAF_CMD_INFO,           /* Get SAF image specifications command    */
  GRAPH_SAF_CMD_INIT,           /* Init SAF data command                   */
  GRAPH_SAF_CMD_FRAME,          /* SAF frame command call back             */
  GRAPH_SAF_CMD_CB,             /* SAF command call back                   */
#endif /* FEATURE_SAF */
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  GRAPH_VIDEO_ENG_CB,           /* video engine graph task interface       */
#endif
#ifdef FEATURE_CAMERA
  GRAPH_CAMQDSP_CB,
#ifdef FEATURE_MP4_TRANSCODER
  GRAPH_CAMQDSP_OFFLINE_ENCODE_CB,
#endif
#ifdef FEATURE_VIDEO_ENCODE_RAW
  GRAPH_CAMQDSP_RAW_ENCODE_CB,
  GRAPH_VIDEO_ENGINE_RAW_ENCODE_CB,
#endif /* FEATURE_VIDEO_ENCODE_RAW */
  GRAPH_CAMERA_FUNC,
#endif /* FEATURE_CAMERA */
  GRAPH_DSP_REQUEST,            /* DSP request command                     */
  GRAPH_CONTEXT_REQ,            /* General context switch request          */
#ifdef FEATURE_MP4_TRANSCODER
  GRAPH_TRANSCODER,
#endif
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  GRAPH_VIDEO_ENC_CMD_FRAME_DONE, /* video encode frame done command       */
#endif
#ifdef FEATURE_I3D
  GRAPH_Q3D_INIT,               /* Init command                            */
  GRAPH_Q3D_DRAW_MODEL,         /* Draw model command                      */
  GRAPH_Q3D_RENDER_FRAME,       /* Render frame command                    */
  GRAPH_Q3D_MODEL_RELEASE,      /* Model release command                   */
  GRAPH_Q3D_RELEASE,            /* Release command                         */
  GRAPH_Q3D_QDSP_CMD,           /* QDSP command                            */
#endif /* FEATURE_I3D */
#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */
#ifdef FEATURE_SVG
  GRAPH_SVG_CMD_DECODE,         /* CMX decode SVG file command             */
  GRAPH_SVG_CMD_STOP,           /* Stop CMX decode SVG file command        */
  GRAPH_SVG_CMD_ABORT,          /* Abort CMX decode SVG file command       */
  GRAPH_SVG_CMD_FRAME,          /* SVG frame render command                */
  GRAPH_SVG_CMD_EVENT,          /* SVG event callback command              */
  GRAPH_SVG_CMD_RESET,          /* SVG frame reset command                 */
  GRAPH_SVG_CMD_PAUSE,          /* SVG Animation pause command             */
  GRAPH_SVG_CMD_RESUME,         /* SVG Animation resume command            */
  GRAPH_SVG_CMD_REWIND,         /* SVG Animation rewind command            */
  GRAPH_SVG_CMD_FFORWARD,       /* SVG Animation fsat-forward command      */
  GRAPH_SVG_CMD_CB,             /* SVG command callback from CMX to SVG    */
  GRAPH_SVG_CMD_REQUEST_CB,     /* SVG request callback from CMX to SVG    */
  GRAPH_SVG_CMD_SET_VIEWPORT,   /* SVG sets viewport cmd                   */
#ifdef FEATURE_SVGDOM
  GRAPH_SVG_CMD_DOM,            /* SVG request to execute a DOM command    */
#endif /* FESTURE_SVGDOM */
#endif /* FESTURE_SVG */
#ifdef FEATURE_QVPHONE
  GRAPH_QVPHONE_ENC_TOGGLE_RECORD,
#endif
#ifdef FEATURE_VIDEO_MOVIEEDIT
  GRAPH_VIDEO_MOVIEEDIT_PARSE_FUNC, /* video file info parsing             */
#endif /* FEATURE_VIDEO_MOVIEEDIT */
  GRAPH_CMD_INVALID             /* Invalid command                         */
} graph_cmd_type;

/* Header for GRAPH commands
*/
typedef struct
{
  q_link_type                     link;      /* Command queue link         */
  graph_cmd_type                  cmd;       /* GRAPHIC command to perform */
  void*                           handle;    /* Graphics handle            */
  boolean                         overflow;  /* Dynamically Allocated pkt  */
} graph_hdr_type;                            /* Graphic header             */

#ifdef FEATURE_PNG_ENCODER
typedef struct {
  graph_hdr_type                  hdr;       /* Command Header             */
  uint32                          msg;       /* message type               */
} graph_pnge_packet_type;                    /* PNGE packet                */
#endif /* FEATURE_PNG_ENCODER */

#if defined(FEATURE_PNG_ENCODER) || defined(FEATURE_PNG_DECODER)
typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  void                            *handle;         /* PNG Handle           */
  png_cmx_decode_cb_func_type     cb_func;         /* PNG Call back func   */
} graph_png_packet_type;                           /* PNG packet           */
#endif
#ifdef FEATURE_PNG_DECODER
typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  uint8                           *client_buf;     /* Client data          */
  uint32                          client_buf_size; /* Client data size     */
  uint8                           *output_buf;     /* Output buffer        */
  uint32                          output_buf_size; /* Output buffer size   */
} graph_png_buf_packet_type;                       /* PNG buffer packet    */

typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  png_cmx_command_cb_func_type    cb_func;         /* Callback ptr         */
  void                            *client_data;    /* Client data          */
} graph_png_cb_packet_type;                        /* PNG callback packet  */
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  void                            *handle;         /* JPEG Handle          */
  jpeg_cmx_cb_func_type           cb_func;         /* JPEG Call back func  */
  jpeg_decodeInfo_type            jpegdInfo;
} graph_jpeg_decode_packet_type;                   /* JPEG decode packet   */

typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  uint8                           *client_buf;     /* Client data          */
  uint32                          client_buf_size; /* Client data size     */
  uint8                           *output_buf;     /* Output buffer        */
} graph_jpeg_buf_packet_type;                      /* JPEG buffer packet   */

typedef struct {
  graph_hdr_type                  hdr;             /* Command Header       */
  jpeg_cmx_command_cb_func_type    cb_func;        /* Callback ptr         */
  void                            *client_data;    /* Client data          */
} graph_jpeg_cb_packet_type;                       /* JPEG callback packet */

#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_SAF
typedef struct {
  graph_hdr_type                  hdr;                /* Command Header    */
  void                            *handle;            /* SAF handle        */
  saf_cmx_decode_cb_func_ptr_type cb_func;            /* SAF Call back func*/
} graph_saf_packet_type;                              /* SAF packet        */

typedef struct {
  graph_hdr_type                  hdr;                /* Command Header    */
  int32                           frameid;            /* Frame ID          */
  uint8                           *framebuf;          /* Frame buffer      */
  void                            *client_data;       /* Client data       */
} graph_saf_frame_packet_type;                        /* SAF frame packet  */

typedef struct {
  graph_hdr_type                  hdr;                /* Command Header    */
  saf_display_resource_info_type  *display_info_data; /* Display info      */
  uint8                           *g_obj_buf;         /* Object buffer     */
  uint32                          g_obj_buf_size;     /* Object buffer size*/
  uint8                           *g_cod_buf;         /* Coder buffer      */
  uint32                          g_cod_buf_size;     /* Coder buffer size */
  uint8                           *g_info_buf;        /* Info buffer       */
} graph_saf_init_packet_type;                         /* SAF init packet   */

typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  saf_cmx_command_cb_func_ptr_type cb_func;         /* Callback ptr        */
  void                             *client_data;    /* Client data         */
} graph_saf_cb_packet_type;                         /* SAF callback packet */
#endif /* FEATURE_SAF */

#ifdef FEATURE_CAMERA
typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  uint32                           func;
  void                             *callback;
  void                             *client_data;
  uint32                           info1;
  uint32                           info2;
  uint32                           info3;
  uint32                           info4;
} graph_camera_func_packet_type;

typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  uint32                           msg;
  void                             *buf;
  uint32                           size;
} graph_camqdsp_cb_packet_type;
#endif /* FEATURE_CAMERA */

typedef void (* graph_dsp_req_func_type) ( void *client_data );

typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  graph_dsp_req_func_type          function_ptr;    /* Function ptr        */
  void                             *client_data;    /* Client data         */
} graph_dsp_req_packet_type;

typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  graph_context_req_type           packet;    /* Function ptr        */
} graph_req_cont2_type;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
typedef struct {

  graph_hdr_type                hdr;
  graph_video_eng_msg_enum      msg;
  void *                       *response;

} graph_video_eng_msg_type;
#endif

#ifdef FEATURE_MP4_TRANSCODER
typedef struct {

  graph_hdr_type           hdr;
  mp4_transcoder_msg_type  msg;

} graph_transcoder_msg_type;

#endif

#ifdef FEATURE_VIDEO_ENCODE_RAW
typedef struct {
  graph_hdr_type          hdr;
  uint32                  status;
  void *                  client_data;  
} graph_raw_encoder_msg_type;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
typedef struct {
  graph_hdr_type                  hdr;                /* command header    */
  uint8                           *data_buf;          /* video data        */
} graph_video_enc_packet_type;                        /* SAF frame packet  */
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE*/

#ifdef FEATURE_I3D
typedef struct {
  graph_hdr_type                hdr;           /* command header           */
  graph_q3d_status_cb_func_type cb_func;       /* client callback function */
  void                          *client_data;  /* client callback data     */
  graph_q3d_init_func_ptr_type  func_ptr;      /* Init function pointer    */
} graph_q3d_init_packet_type;

typedef struct {
  graph_hdr_type                     hdr;      /* command header           */
  void                               *model;   /* model data               */
  void                               *ctx;     /* ctx data                 */
  graph_q3d_draw_model_func_ptr_type func_ptr; /* draw model function ptr  */
} graph_q3d_model_packet_type;

typedef struct {
  graph_hdr_type                      hdr;          /* command header      */
  void                                *ctx;         /* ctx data            */
  graph_q3d_frame_status_cb_func_type cb_func;      /* Callback function   */
  void                                *client_data; /* Client data         */
  graph_q3d_render_func_ptr_type      func_ptr;     /* render func pointer */
} graph_q3d_render_packet_type;

typedef struct {
  graph_hdr_type                  hdr;              /* command header      */
  void                            *data;            /* data                */
  graph_q3d_status_cb_func_type   cb_func;          /* client cb func      */
  void                            *client_data;     /* client cb data      */
  graph_q3d_release_func_ptr_type func_ptr;         /* function pointer    */
} graph_q3d_release_packet_type;

typedef struct {
  graph_hdr_type                   hdr;      /* command header             */
  int                              cmd;      /* qdsp command               */
  graph_q3d_qdsp_cmd_func_ptr_type func_ptr; /* qdsp cmd function pointer  */
} graph_q3d_qdsp_cmd_packet_type;
#endif /* FEATURE_I3D */

#ifdef FEATURE_VIDEO_MOVIEEDIT
typedef struct {
  graph_hdr_type                   hdr;             /* Command Header      */
  uint32                           func;
  void                             *callback;
  void                             *client_data;
  uint32                           info1;
  uint32                           info2;
  uint32                           info3;
  uint32                           info4;
} graph_video_movieedit_parse_packet_type;
#endif /* FEATURE_VIDEO_MOVIEEDIT */

#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */

#ifdef FEATURE_SVG
/* Thi struct is used for SVG Decode cmd */
typedef struct{
  graph_hdr_type   hdr;                   /* command header                   */
  svg_cmx_cb_type  cb_func;               /* general CMX callback             */
  void*            cb_data;
  uint8*           mem_buffer;            /* Memory allocated upfront         */
  char*            filename;              /* Filename                         */
  uint32           mem_buffer_size;       /* Size of memory allocated upfront */
  uint16           width;                 /* Width of display area            */
  uint16           height;                /* Height of display area           */
  uint16           dpi;                   /* Dots per inch                    */
  uint8            transparency;          /* true = transparency is turned on */
  uint8            max_requests;          /* Max requests allowed by CMX      */ 
  uint32           bk_color;              /* Background color, valid only if  */
                                          /* transparency is off              */
  boolean          progressive;
}graph_svg_decode_cmd_type;

/* This struct is used for SVG Frame cmd */
typedef struct{
  graph_hdr_type    hdr;                  /* command header                   */
  uint32            time_ms;              /* Time in ms of frame              */
  uint8*            frame_buf;            /* Frame buffer                     */
  uint32            frame_size;           /* size of frame buffer             */
  boolean           seek;                 /* Seek or Advance                  */
}graph_svg_frame_cmd_type;

/* This struct is used for SVG Event cmd */
typedef struct{
  graph_hdr_type          hdr;             /* command header                 */
  cmx_svg_event_enum_type event;           /* Event type                     */
  cmx_svg_event_type      event_data;      /* Event data                     */
  uint32                  time_ms;         /* Time at which event occured    */
}graph_svg_event_cmd_type;

/* This struct is used as cb for CMX */
typedef struct {
  graph_hdr_type          hdr;             /* Command Header                 */
  svg_decoder_cb_type     cb_func;         /* Callback ptr                   */
  void                   *client_data;     /* Client data                    */
} graph_svg_cb_cmd_type;                         

/* This struct is sued as cb for External Requests */ 
typedef struct {
  graph_hdr_type          hdr;             /* Command Header                 */
  void                   *client_data;     /* Client data                    */
  void                   *cmx_handle;      /* CMX handle                     */
  char                   filename[FS_FILENAME_MAX_LENGTH_P]; /* External filename */
} graph_svg_req_cb_cmd_type;                 

/* This struct is sued for rewind and fforward cmds */
typedef struct{
  graph_hdr_type          hdr;             /* Command Header                 */
  uint32                  time_ms;         /* Time to seek to                */
} graph_svg_seek_cmd_type;

/* This struct is used for Set Viewport cmd */
typedef struct{
  graph_hdr_type          hdr;             /* Command header                 */
  uint32                  width;           /* New width of viewport          */
  uint32                  height;          /* New height of viewport         */
} graph_svg_setviewport_cmd_type;

#ifdef FEATURE_SVGDOM
typedef struct{
  graph_hdr_type               hdr;
  svg_dom_api_enum_type        type;
  svg_dom_data_type            data;
} graph_svg_dom_api_type;
#endif /* FEATURE_SVGDOM */

#endif /* FEATURE_SVG */

typedef union {
  graph_hdr_type                  hdr;                /* Command Header    */
#ifdef FEATURE_PNG_ENCODER
  graph_pnge_packet_type          pnge_msg;
#endif /* FEATURE_PNG_ENCODER */
#if defined(FEATURE_PNG_ENCODER) || defined(FEATURE_PNG_DECODER)
  graph_png_packet_type           png_cmd;            /* PNG decode        */
#endif
#ifdef FEATURE_PNG_DECODER
  graph_png_buf_packet_type       png_buf;            /* PNG buffer        */
  graph_png_cb_packet_type        png_cb;             /* PNG callback      */
#endif /* FEATURE_PNG_DECODER */
#ifdef FEATURE_JPEG_DECODER
  graph_jpeg_decode_packet_type   jpeg_decode_cmd;    /* JPEG decode       */
  graph_jpeg_buf_packet_type      jpeg_buf;           /* JPEG buffer       */
  graph_jpeg_cb_packet_type       jpeg_cb;            /* JPEG callback     */
#endif /* FEATURE_JPEG_DECODER */
#ifdef FEATURE_SAF
  graph_saf_packet_type           saf_cmd;            /* SAF decode        */
  graph_saf_frame_packet_type     saf_frame;          /* SAF frame packet  */
  graph_saf_init_packet_type      saf_init;           /* SAF init packet   */
  graph_saf_cb_packet_type        saf_cb;             /* SAF callback      */
#endif /* FEATURE_SAF */
#ifdef FEATURE_CAMERA
  graph_camqdsp_cb_packet_type    camqdsp_cb;
  graph_camera_func_packet_type   camera_func;
#endif /* FEATURE_CAMERA */
  graph_dsp_req_packet_type       dsp_request;        /* DSP request packet*/
  graph_req_cont2_type            context_req;
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  graph_video_eng_msg_type        video_eng_msg;      /* video encoder msg  */
#endif
#ifdef FEATURE_MP4_TRANSCODER
  graph_transcoder_msg_type       transcoder;         /* transcoder message */
#endif
#ifdef FEATURE_VIDEO_ENCODE_RAW
  graph_raw_encoder_msg_type      raw_encoder;
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  graph_video_enc_packet_type     video_enc_frame;    /* video encode frame*/
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE*/
#ifdef FEATURE_I3D
  graph_q3d_init_packet_type      q3d_init;           /* q3d init          */
  graph_q3d_model_packet_type     q3d_model;          /* q3d draw model    */
  graph_q3d_render_packet_type    q3d_render;         /* render frame      */
  graph_q3d_release_packet_type   q3d_release;        /* release           */
  graph_q3d_qdsp_cmd_packet_type  q3d_qdsp;           /* qdsp command      */
#endif /* FEATURE_I3D */
#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */
#ifdef FEATURE_SVG
  graph_svg_decode_cmd_type       svg_decode_cmd;     /* SVG decode cmd    */
  graph_svg_frame_cmd_type        svg_frame_cmd;      /* SVG frame cmd     */
  graph_svg_event_cmd_type        svg_event_cmd;      /* SVG event cmd     */
  graph_svg_seek_cmd_type         svg_seek;           /* SVG rewinf/fforward */
  graph_svg_cb_cmd_type           svg_cb;             /* SVG callback cmd  */ 
  graph_svg_req_cb_cmd_type       svg_req_cb;
  graph_svg_setviewport_cmd_type  svg_setviewport;    /* SVG set viewport */
#ifdef FEATURE_SVGDOM
  graph_svg_dom_api_type          svg_dom_cmd;        /* SVG DOM API cmd */
#endif /* FEATURE_SVGDOM */
#endif /* FEATURE_SVG */
#ifdef FEATURE_VIDEO_MOVIEEDIT
  graph_video_movieedit_parse_packet_type video_movieedit_parse_func; 
#endif /* FEATURE_VIDEO_MOVIEEDIT */
} graph_packet_type;                                  /* Graphics packet   */


/* <EJECT> */
/*---------------------------------------------------------------------------
** GRAPH local function prototypes
**---------------------------------------------------------------------------
*/
LOCAL void graph_task_init (void);
LOCAL void graph_task_offline (void);
LOCAL void graph_task_shutdown (void);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmd_parser

DESCRIPTION
  This procedure processes requests received by graph_cmd(). See above
  for a description of the actual graphic packets.

DEPENDENCIES
  FEATURE_GRAPH_TASK is defined.
  graph_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_cmx_png_cb.
  graph_cmx_saf_cb.

===========================================================================*/
LOCAL void graph_cmd_parser(void);

/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION graph_png_callback

DESCRIPTION
  This function processes callback commands from PNG to Graphic

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_png_callback
(
  png_cmx_status_enum_type png_status,
  void                     *handle,
  void                     *server_data
);
#endif /* FEATURE_PNG_DECODER */

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION graph_jpeg_callback

DESCRIPTION
  This function processes callback commands from JPEG to Graphic

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_jpeg_callback
(
  jpeg_cmx_status_enum_type jpeg_status,
  void                      *handle,
  void                      *server_data
);
#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION graph_saf_callback

DESCRIPTION
  This function processes callbacks from SAF to Graphic.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_saf_frame_cmd_cb.

===========================================================================*/
extern void graph_saf_callback
(
  saf_cmx_status_type saf_status,
  void                *handle,
  void                *server_data
);
#endif /* FEATURE_SAF */

#ifdef FEATURE_SVG
/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_decode_processing

DESCRIPTION
  This function starts SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_decode_processing( 
  graph_packet_type* packet
);
#endif /* FEATURE_SVG */
#endif /* FEATURE_GRAPH_TASK */
#endif /* GRAPHI_H */

