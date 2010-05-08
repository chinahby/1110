/* linesize(758)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("GRAPHICS")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        G R A P H I C S     T A S K

GENERAL DESCRIPTION
  This module contains graphics software.

REFERENCES

EXTERNALIZED FUNCTIONS

  graph_task
    This function is the entrance procedure for the  PNG and SAF
    Task.  It contains the main processing loop for the Graphics Task.
  graph_png_decode
    This function issues a command to start the PNG decoder.
  graph_png_decode_abort
    This function issues a command to end the current PNG decoder session.
  graph_png_get_image_specs
    This function issues a command to extract the header information
    from the PNG image.
  graph_png_set_buf
    This function issues a command to set the LZ window size for
    buffering.
  graph_jpeg_decode
    This function issues a command to start the JPEG decoder.
  graph_jpeg_decode_abort
    This function issues a command to abort the JPEG decoder.
  graph_jpeg_get_specs
    This function issues a command to get the JPEG image header information.
  graph_jpeg_encode
    This function issues a command to start the JPEG encoder.
  graph_jpeg_encode_abort
    This function issues a command to abort the JPEG encoder.
  graph_saf_decode
    This function issues a command to start the SAF decoder.
  graph_saf_get_info
    This function issues a command to provide information about the
    animateon in the SAF file.
  graph_saf_decode_abort
    This function issues a command to end the current SAF decoder session.
  graph_saf_decode_stop
    This function issues a command to stop the current SAF decoder session.
  graph_saf_global_init
    This function issues a command to setup SAF player's display parameter
    information.
  graph_q3d_init
    This function queue up q3d init function.
  graph_q3d_draw model
    This function queue up q3d draw model function.
  graph_q3d_render
    This function queue up q3d render function.
  graph_q3d_release
    This function queue up q3d release function.
  graph_q3d_qdsp_cmd
    This function queues up qdsp request message.
  graph_mpeg4_free_rgb_buffer
    This function tells graph_mpeg4_send_video_slice() an rgb buffer
  is available.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/graph.c#75 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   sanal   Removed usage of strncpy
05/15/08   sanal   Featurized changes for synchronous stopping in transcoder
04/03/08   sanal   Synchronous stopping in transcoder
02/07/08   cg      Included the header file stdlib.h to get rid of compiler warnings
02/12/06   vma     Fixed the mis-placement of mm_init() under an unrelated 
                   feature define
12/15/06   jv      Allow new graph packets to be dynamically allocated.
11/28/06   Pavan   Support added for Movieedit Interface
11/22/06   arv     Added support for multiple parameters in jpeg decoder
10/24/06   vma     Initialize malloc mgr if feature is present
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency 
                   When qcamcorder Feature Disabled 
05/03/06   wyh     Removing m4v2rgb_decode_done().
05/02/06    ss     Changes to synchronize QVP recording control path with
                   encoder running in graph task context
03/27/06   lip     Add SVGT 1.2 and FEATURE_SVGDOM support.
11/11/05    AE     Assert graphics ok to sleep signal on graph_task init 
11/03/05   nrw     Changed type of argument to graph_queue_pnge_msg() from
                   graph_png_request_type to PNGE_status_enum_type to match
                   cb parameters.
07/21/05    jk     Added a call to dog_report before video_encode_frame.
06/30/05    lg     Modified code to check if graph_svg_cmx_cb is NULL. CR# 67498.
05/27/05    dw     Added raw mode support for video encoding.
04/20/05    lg     Modified the SVG decode packet to add filename.
04/19/05    lg     Modified the SVG frame command to support seek and advance.
04/06/05   ddh     Commented out call to SLEEP_ASSERT_OKTS 
04/05/05    lg     Modified code to copy SVG event data to Graph packet.
04/04/05   ddh     Replaced OKTS MASK with OKTS SIG
02/24/05   lg      Added support for SVG
01/25/05   wh      Cosmetic changes from code review of video_eng_frame api
11/19/04   wh      Support the graph-task component of video_eng. Featurized
                   under FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
11/04/04   ark     Updated graph_jpeg_decode() API with new parameters.
09/14/04   drl     Move graph_context_req to outside ifdef FEATURE_PNG_DECODER
                   scope.
09/01/04   pdo     Added GRAPH_CONTEXT_REQ case for 6550 JPEG encoder
08/25/04   pdo     Added #if.. CAMERA_FEATURE for PNGE that needs camera function
05/03/04   pdo     Added sync/async option for DSP version decoder
04/23/04    lg     Added support for SAF_CMX_GET_FILE_SIZE
04/19/04   pdo     Updated graph_jpeg_decode() API with new parameters.
                   Provided needed interfaces to synchronize the JPEG
                   async. data request.
04/08/04    st     Removed JPEG encoder software, this is no longer supported
                   through GRAPH layer.
03/22/04    st     Modified code to support client buffers for various decoders;
                   updated graph_jpeg_callback() to handle updated
                   JPEG_CMX_GET_BUF command; modified JPEG_DSP_REQUEST command
                   to not call the client callback.
03/02/04   ali     Changes for Offline encoder
02/19/04    aw     Modified code to allow to sleep after 3D graphics init.
02/12/04    jz     Added Registration and De-registration for QVP Video Encoder
12/01/03   pdo     Added supports for PNG encoder
11/21/03    aw     Added supports for QGLES dsp services.
09/16/03   dle     Expanded parameter list for graph_queue_camera_func().
05/19/03    bh     Added graph_mpeg4_free_rgb_buffer and global variables
                   numRGBBuffersUsed and fRGBBufferPending to address
                   video "jerks" CR.
05/13/03    aw     Added return status for Q3D functions, added init Q3D timer
                   function.
05/13/03    dle    Renamed vod_buffer to buf.
03/18/03    aw     Modified code to reuse some of the Q3D functions.
02/20/03    aw     Added support for Q3D engine.
02/17/03    sa     Added functions for the video encoder when
                   FEATURE_VIDEO_ENCODE is enabled.
11/29/02    sa     Added JPEG_CMX_GET_BUF status to switch statement in
                   function graph_jpeg_callback.
09/26/02   dle     Name change from camera_handle_* to camera_process_*.
09/19/02   dle     Merged from 1.2.1 branch.
08/29/02   dle     Added functions for FEATURE_CAMERA.
06/07/02   jqi     Updated with multimode task operation interface.
09/17/02    ny     Creation of graph_mpeg4_timer in the graph task context
                   that is used in the decode done processing for yuv
                   logging and stats buffer allocation.  The timer is used
                   in the cases where buffers are not available and the code
                   must wait for buffers to become available.
                   Renamed graph_set_mpeg4_slice_ready_event to
                   graph_mpeg4_set_slice_ready_event.
                   Renamed graph_queue_mpeg4_decode_done_cmd to
                   graph_mpeg4_queue_decode_done_cmd.
009/09/02    ny    Removed code for unsupported TEST_MPEG4_QDSP_INTERFACE and
                   CODEC_INTERFACE testing modes.
                   Added MPEG4 decode frame done handler to graph command parser.
                   Moved external declarations of mp4_slice_q and mp4_slice_free_q
                   to mp4graph.h
                   Added new function graph_queue_mpeg4_decode_done_cmd().
08/13/02    ny     Inclusion of INTLOCK/INTFREE statements when PLATFORNM_LTK enabled
                   for the function graph_mpeg4_send_video_slice (to supplement
                   TASKLOCK/TASKFREE for the LTK)
08/02/02    ny     Cleanup of function graph_mpeg4_send_video_slice. All modifications
                   are local to this function, which is invoked only when
                   FEATURE_MP$_DECODER is enabled.
07/22/02    sa     Added include file mp4graph.h.  Added new signal
                   GRAPH_MPEG4_SLICE_REQUEST_SIG and replaces
                   MP4_VIDEO_PKT_REQUEST.   Added function graph_set_mpeg4_
                   slice_ready_event().
06/21/02    sa     Added support for MPEG-4 decoder.
03/22/02    sa     Added support for JPEG encoder and decoder.
03/08/02    aw     Removing unnecessary multiple commands from the command
                   queue.
08/08/01    aw     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "jpeg.h"
#ifdef FEATURE_GRAPH_TASK
#include "rex.h"                /* definition of REX data types            */
#include "task.h"               /* Task definitions and prototypes         */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "dog.h"                /* Watchdog signal and time definitions    */
#include "mc.h"                 /* Need for MC signals and prototypes      */
#include "graph.h"              /* GRAPH typedefs and prototypes           */
#include "graphi.h"             /* GRAPH internal typedefs and prototypes  */
#include "sleep.h"              /* Sleep interfaces                        */
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* malloc and free routines */ 
#ifdef FEATURE_CAMERA
#include "camera.h"
#endif /* FEATURE_CAMERA */
#ifdef DSP_3D_ENGINE
#error code not present
#endif
#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_CL */
#ifdef FEATURE_PNG_ENCODER
#include "pnge.h"
#endif /* FEATURE_PNG_ENCODER */
#ifdef FEATURE_VIDEO_MOVIEEDIT
#include "movieedit_task.h"
#endif /* FEATURE_VIDEO_MOVIEEDIT */
#ifdef MALLOC_LOGGING
#include "malloc_mgr.h"
#endif /* MALLOC_LOGGING */

#ifdef GRAPH_USES_LIBSTD
#include "AEEstd.h"
#endif /* GRAPH_USES_LIBSTD */

/* <EJECT> */
/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
** GRAPHIC Command Queue, Free Queue and Available Packets
**---------------------------------------------------------------------------
*/
LOCAL q_type             graph_cmd_q;      /* Request Packets Queue        */
LOCAL q_type             graph_free_q;     /* Available Packets Queue      */
LOCAL graph_packet_type  graph_free_packet [GRAPH_NUM_PACKETS];
                                           /* Available Packets            */
LOCAL rex_timer_type     graph_rpt_timer;  /* Watch Dog Timer              */

/*---------------------------------------------------------------------------
** Call back function for CMX
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_PNG_DECODER
/* Callback function from graphic to cmx
*/
LOCAL png_cmx_decode_cb_func_type     graph_cmx_png_cb;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
/* Callback function from graphic to cmx
*/
LOCAL jpeg_cmx_cb_func_type     graph_cmx_jpeg_cb;

#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_SAF
/* Callback function from graphic to cmx
*/
LOCAL saf_cmx_decode_cb_func_ptr_type graph_cmx_saf_cb;

/* SAF display information structure
*/
saf_display_resource_info_type        graph_saf_display_info_data;

/* SAF frame command function pointer
*/
LOCAL saf_cmx_frame_cb_func_ptr_type  graph_saf_frame_cmd_cb;
#endif /*FEATURE_SAF */

#ifdef FEATURE_SVG
/* Callback function from graphics to CMX */
svg_cmx_cb_type                      graph_cmx_svg_cb;
#endif /* FEATURE_SVG */

#ifdef FEATURE_QVPHONE

extern boolean qvp_g_venc_record_flag;
extern void qvpio_record_status_cb
(
  boolean qvp_g_vrx_record_flag,
  int module
);
#endif /*FEATURE_QVPHONE */

/* This structure is used to retain information related to the various
** callback
*/
typedef struct {
  void *callback_ptr;               /* Callback function to PNG or SAF    */
  void *client_data;                /* Callback client data to PNG or SAF */
} graph_cmx_cb_type;                /* Graphic to CMX callback data type  */

#define GRAPH_CMX_CB_MAX_NUM 10     /* Maximum number of callback data    */
LOCAL graph_cmx_cb_type
    graph_cmx_cb_array[GRAPH_CMX_CB_MAX_NUM]; /* Callback data structures */

#ifdef FEATURE_VIDEO_ENCODE
/*  Video Encoder Default to Camcorder Video Encoder
*/
LOCAL video_encode_func_type graph_video_encoder = video_encode_frame;
#elif defined FEATURE_QVPHONE
LOCAL video_encode_func_type graph_video_encoder = NULL;
#endif /* FEATURE_VIDEO_ENCODE */

/*---------------------------------------------------------------------------
** Function declaration & extern of global variables
**---------------------------------------------------------------------------
*/

void graph_manage_cmd_q_ex(graph_cmd_type cmd, void* handle);

/* <EJECT> */
/*===========================================================================

FUNCTION graph_task

DESCRIPTION
  This procedure is the entrance procedure for the GRAPHICS
  Task.  It contains the main processing loop for the GRAPHICS Task.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None

SIDE EFFECTS
  graph_tcb.

===========================================================================*/
void graph_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;

#ifdef FEATURE_CAMERA
  camera_init();
#endif /* FEATURE_CAMERA */

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
  jpeg_register();
#endif /* FEATURE_JPEG_DECODER || FEATURE_JPEG_ENCODER */

#if defined(FEATURE_I3D) || defined(FEATURE_GRAPHICS_OPENGLES_CL)
#ifdef DSP_3D_ENGINE
#error code not present
#endif

#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */

  SLEEP_ASSERT_OKTS( SLEEP_GRAPHICS_OKTS_SIG ); 
#endif /* FEATURE_I3D || FEATURE_GRAPHICS_OPENGLES_CL */

#ifdef MALLOC_LOGGING
  mm_init();
#endif /* MALLOC_LOGGING */

  /* Initialize GRAPHIC Task
  */
  graph_task_init ();

#ifdef FEATURE_MULTIMODE_ARCH
  /*  Process task start procedure from task controller. */
  (void) task_start (GRAPH_RPT_TIMER_SIG, DOG_GRAPH_RPT, &graph_rpt_timer);
#else /* FEATURE_MULTIMODE_ARCH */
  /* Wait for start signal from Main Control task
  */
  mc_task_start (GRAPH_RPT_TIMER_SIG, DOG_GRAPH_RPT, &graph_rpt_timer);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal
  */
  (void) rex_clr_sigs (&graph_tcb, TASK_START_SIG);

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs (&graph_tcb, GRAPH_RPT_TIMER_SIG);

  /* Infinite loop -- process events forever
  */
  for (;;) {

    /* Wait for something to happen
    */
    sigs = rex_wait( GRAPH_RPT_TIMER_SIG     |   /* Watchdog report timer  */
                     GRAPH_CMD_Q_SIG         |   /* Command queue signal   */
                     TASK_STOP_SIG           |   /* Task stop from MC      */
                     TASK_OFFLINE_SIG);          /* Task offline from MC   */

    /* If watchdog report timer expired, report and restart the timer
    */
    if ((sigs & GRAPH_RPT_TIMER_SIG) != 0)
    {
      dog_report (DOG_GRAPH_RPT);
      (void) rex_set_timer (&graph_rpt_timer, DOG_GRAPH_RPT_TIME);
    }

    /*---------------------------------------------------------------------*/


    /* Process the queued commands
    */
    if ((sigs & GRAPH_CMD_Q_SIG) != 0 )
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&graph_tcb, GRAPH_CMD_Q_SIG);
      graph_cmd_parser ();
    }

    /*---------------------------------------------------------------------*/
    /* We're supposed to shutdown.
    */
    if ((sigs & TASK_STOP_SIG) != 0)
    {
      graph_task_shutdown ();
    }

    /*---------------------------------------------------------------------*/

    /* We're supposed to go offline.
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
      graph_task_offline ();
    }

    /*---------------------------------------------------------------------*/

  } /* end of infinite loop */
} /* end graph_task */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_task_init

DESCRIPTION
  This procedure initializes the graphic task.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_tcb.
  graphic command queue and free queue.

===========================================================================*/
LOCAL void graph_task_init (void)
{
  int i;

  /* Initialize timers
  */
  rex_def_timer (&graph_rpt_timer, &graph_tcb, GRAPH_RPT_TIMER_SIG);

  /* Initialize the command queues.
  */
  (void) q_init (&graph_cmd_q);           /* Initialize the command queue  */
  (void) q_init (&graph_free_q);          /* Initialize the free queue     */

  /* Add items to graph_free_q.
  */
  for (i = 0; i < GRAPH_NUM_PACKETS; i++)
  {
    q_link_type *link_item;
    link_item = q_link( &graph_free_packet[i],
                        &graph_free_packet[i].hdr.link );
    q_put( &graph_free_q, link_item );
  }

  MSG_LOW("GRAPHIC task initialized",0,0,0);

} /* end graph_task_init */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmd

DESCRIPTION
  Queue a command for processing by the graphic Service.

DEPENDENCIES
  FEATURE_GRAPH_TASK.
  graph_task_init() needs to have initialized the queues.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_tcb.

===========================================================================*/
LOCAL void graph_cmd
(
  graph_packet_type *graph_cmd_ptr
    /* pointer to GRAPHICS command */
)
{
  (void) q_link (graph_cmd_ptr, &graph_cmd_ptr->hdr.link);
                                                    /* Init link field     */
  q_put (&graph_cmd_q, &graph_cmd_ptr->hdr.link);   /* Put on command queue*/
  (void) rex_set_sigs(&graph_tcb, GRAPH_CMD_Q_SIG); /* Signal a queue event*/

} /* end graph_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmd_get_pkt

DESCRIPTION
  Get a graphic Command Packet.  Use by callers of graph_cmd().

DEPENDENCIES
  FEATURE_GRAPH_TASK.
  graph_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available graph_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  graph_free_q (and the packet fetched off graph_free_q).

===========================================================================*/
LOCAL graph_packet_type *graph_cmd_get_pkt (void)
{
  graph_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (graph_packet_type *) (q_get (&graph_free_q));

  if (packet == NULL)
  {
    if(!rex_is_in_irq_mode())
    {
      MSG_ERROR("Ran out of GRAPHICS Packets! Spawning a new packet!!!", 0, 0, 0);
      packet = (graph_packet_type *)malloc(sizeof(graph_packet_type));
      if(packet != NULL)
      {
        (void) memset (packet, 0, sizeof (graph_packet_type));
        packet->hdr.cmd = GRAPH_CMD_INVALID;     /* Invalid Command Value   */
        packet->hdr.overflow = TRUE;
      }
      else
      {
        MSG_ERROR("Out of memory could not allocate overflow graph packet!!!",0,0,0);
      }
    }
    else
    {
      MSG_ERROR("Ran out of GRAPHICS Packets. Can not allocate packet in irq mode!!!",0,0,0);
    }
  }
  else
  {
    /* Fill the entire GRAPH packet with 0's to get default behavior on
    ** un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (graph_packet_type));
    packet->hdr.cmd = GRAPH_CMD_INVALID;     /* Invalid Command Value   */
  }

  return (packet);

} /* end graph_cmd_get_pkt */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmd_parser

DESCRIPTION
  This procedure processes requests received by graph_cmd(). See graphi.h for
  a description of the actual CMX packets.

DEPENDENCIES
  FEATURE_GRAPH_TASK.
  graph_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_cmx_png_cb.
  graph_cmx_saf_cb.

===========================================================================*/
LOCAL void graph_cmd_parser (void)
{
  graph_packet_type *cmd_ptr;

  /* While there are commands to process, process each command.
  */
  if ((cmd_ptr = (graph_packet_type *) q_get (&graph_cmd_q)) != NULL) {

    /* Process each command type.
    */
    switch (cmd_ptr->hdr.cmd)
    {
#ifdef FEATURE_MP4_TRANSCODER

    case GRAPH_TRANSCODER:
      xcoder_graph_cmd_parser(&cmd_ptr->transcoder.msg);
      break;

#endif  /* FEATURE_MP4_TRANSCODER */


#ifdef FEATURE_PNG_ENCODER
      case GRAPH_PNGE_CMD:
        pnge_image_encode();
        break;

#ifdef FEATURE_CAMERA
      case GRAPH_PNGE_MSG:
        camera_process_pnge_msg(cmd_ptr->pnge_msg.msg);
        break;
#endif /* FEATURE_CAMERA */
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_PNG_DECODER
      case GRAPH_PNG_CMD_DECODE:
        graph_cmx_png_cb = cmd_ptr->png_cmd.cb_func;
        png_decode(cmd_ptr->png_cmd.handle, graph_png_callback);
        break;

      case GRAPH_PNG_CMD_ABORT:
        png_decode_abort();
        break;

      case GRAPH_PNG_CMD_SPEC:
        graph_cmx_png_cb = cmd_ptr->png_cmd.cb_func;
        png_get_image_specs(cmd_ptr->png_cmd.handle, graph_png_callback);
        break;

      case GRAPH_PNG_CMD_BUF:
        png_set_buf(cmd_ptr->png_buf.client_buf,
                    cmd_ptr->png_buf.client_buf_size,
                    cmd_ptr->png_buf.output_buf,
                    cmd_ptr->png_buf.output_buf_size);
        break;

      case GRAPH_PNG_CMD_CB:
        if(cmd_ptr->png_cb.cb_func != NULL) {
          cmd_ptr->png_cb.cb_func( cmd_ptr->png_cb.client_data );
        }
        break;
#endif /* FEATURE_PNG_DECODER */

#ifdef FEATURE_JPEG_DECODER
      case GRAPH_JPEG_CMD_DECODE:
         graph_cmx_jpeg_cb = cmd_ptr->jpeg_decode_cmd.cb_func;
         jpeg_decode(cmd_ptr->jpeg_decode_cmd.handle, graph_jpeg_callback, cmd_ptr->jpeg_decode_cmd.jpegdInfo);
        break;

      case GRAPH_JPEG_CMD_SPEC:
        graph_cmx_jpeg_cb = cmd_ptr->jpeg_decode_cmd.cb_func;
        jpeg_get_image_specs(cmd_ptr->jpeg_decode_cmd.handle, graph_jpeg_callback);
        break;

      case GRAPH_JPEG_CMD_ABORT_DECODE:
        jpeg_abort_decode();
        break;

      case GRAPH_JPEG_CMD_DEC_QDSP_READY:
        jpeg_decode_continue();
        break;

      case GRAPH_JPEG_CMD_DEC_IMAGE_ABORT:
        jpeg_decode_image_abort();
        break;

#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_JPEG_DECODER
      case GRAPH_JPEG_CMD_BUF:
        jpeg_set_buf(cmd_ptr->jpeg_buf.client_buf,
                     cmd_ptr->jpeg_buf.client_buf_size,
                     cmd_ptr->jpeg_buf.output_buf);
        break;

      case GRAPH_JPEG_CMD_CB:
        if(cmd_ptr->jpeg_cb.cb_func != NULL) {
          cmd_ptr->jpeg_cb.cb_func( cmd_ptr->jpeg_cb.client_data );
        }
        break;
#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_SAF
      case GRAPH_SAF_CMD_DECODE:
        graph_cmx_saf_cb = cmd_ptr->saf_cmd.cb_func;
        saf_decode(cmd_ptr->saf_cmd.handle, graph_saf_callback);
        break;

      case GRAPH_SAF_CMD_ABORT:
        saf_decode_abort();
        break;

      case GRAPH_SAF_CMD_STOP:
        saf_decode_stop();
        break;

      case GRAPH_SAF_CMD_INFO:
        graph_cmx_saf_cb = cmd_ptr->saf_cmd.cb_func;
        saf_get_info(cmd_ptr->saf_cmd.handle, graph_saf_callback);
        break;

      case GRAPH_SAF_CMD_INIT:
        saf_global_init(cmd_ptr->saf_init.display_info_data,
                        cmd_ptr->saf_init.g_obj_buf,
                        cmd_ptr->saf_init.g_obj_buf_size,
                        cmd_ptr->saf_init.g_cod_buf,
                        cmd_ptr->saf_init.g_cod_buf_size,
                        cmd_ptr->saf_init.g_info_buf);
        break;

      case GRAPH_SAF_CMD_CB:
        if(cmd_ptr->saf_cb.cb_func != NULL) {
          cmd_ptr->saf_cb.cb_func( cmd_ptr->saf_cb.client_data );
        }
        break;

      case GRAPH_SAF_CMD_FRAME:
        if(graph_saf_frame_cmd_cb != NULL) {
          graph_saf_frame_cmd_cb(cmd_ptr->saf_frame.frameid,
                                 cmd_ptr->saf_frame.framebuf,
                                 cmd_ptr->saf_frame.client_data);
        }
        break;
#endif /* FEATURE_SAF */

      case GRAPH_DSP_REQUEST:
#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */
        if(cmd_ptr->dsp_request.function_ptr != NULL) {
          cmd_ptr->dsp_request.function_ptr( cmd_ptr->dsp_request.client_data );
        }
        break;

      case GRAPH_CONTEXT_REQ:
        if(cmd_ptr->context_req.packet.function_ptr != NULL) {
          cmd_ptr->context_req.packet.function_ptr(cmd_ptr->context_req.packet.client_ID,
                                            cmd_ptr->context_req.packet.client_data);
        }
        break;


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    case GRAPH_VIDEO_ENG_CB:
      /* Call dog_report before we call video_encode_frame. */
      if (cmd_ptr->video_eng_msg.msg == VIDENG_GRAPH_FRAME_CMD) {
        dog_report(DOG_GRAPH_RPT);
      }
      video_eng_graph_cmd_parser(cmd_ptr->video_eng_msg.msg,
                                 cmd_ptr->video_eng_msg.response);
      break;
#endif

#ifdef FEATURE_CAMERA
      case GRAPH_CAMQDSP_CB:
        camera_process_qdsp_msg(
            cmd_ptr->camqdsp_cb.msg,
            cmd_ptr->camqdsp_cb.buf,
            cmd_ptr->camqdsp_cb.size);
        break;
#ifdef FEATURE_MP4_TRANSCODER
  case GRAPH_CAMQDSP_OFFLINE_ENCODE_CB:
         xcoder_process_camqdsp_msg(
            cmd_ptr->camqdsp_cb.msg,
            cmd_ptr->camqdsp_cb.buf,
            cmd_ptr->camqdsp_cb.size);
    break;
#endif

#ifdef FEATURE_VIDEO_ENCODE_RAW
      case GRAPH_CAMQDSP_RAW_ENCODE_CB:
         raw_encoder_process_camqdsp_msg(
            cmd_ptr->camqdsp_cb.msg,
            cmd_ptr->camqdsp_cb.buf,
            cmd_ptr->camqdsp_cb.size);
        break;

    case GRAPH_VIDEO_ENGINE_RAW_ENCODE_CB:
         raw_encoder_process_video_engine_callback(
            cmd_ptr->raw_encoder.status,
            cmd_ptr->raw_encoder.client_data); 
         break;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

      case GRAPH_CAMERA_FUNC:
        camera_process_func(
            cmd_ptr->camera_func.func,
            cmd_ptr->camera_func.callback,
            cmd_ptr->camera_func.client_data,
            cmd_ptr->camera_func.info1,
            cmd_ptr->camera_func.info2,
            cmd_ptr->camera_func.info3,
            cmd_ptr->camera_func.info4);
        break;
#endif /* FEATURE_CAMERA */

#ifdef FEATURE_VIDEO_ENCODE
      case GRAPH_VIDEO_ENC_CMD_FRAME_DONE:
        graph_video_encoder(
            cmd_ptr->video_enc_frame.data_buf);
        break;
#endif

#ifdef FEATURE_I3D
      case GRAPH_Q3D_INIT:
        if (cmd_ptr->q3d_init.func_ptr != NULL) {
          cmd_ptr->q3d_init.func_ptr(cmd_ptr->q3d_init.cb_func,
                                     cmd_ptr->q3d_init.client_data);
        }
        break;

      case GRAPH_Q3D_DRAW_MODEL:
        if (cmd_ptr->q3d_model.func_ptr != NULL) {
          cmd_ptr->q3d_model.func_ptr(cmd_ptr->q3d_model.model,
                                      cmd_ptr->q3d_model.ctx);
        }
        break;

      case GRAPH_Q3D_RENDER_FRAME:
        if (cmd_ptr->q3d_render.func_ptr != NULL) {
          cmd_ptr->q3d_render.func_ptr(cmd_ptr->q3d_render.ctx,
                                       cmd_ptr->q3d_render.cb_func,
                                       cmd_ptr->q3d_render.client_data);
        }
        break;

      case GRAPH_Q3D_RELEASE:
        if (cmd_ptr->q3d_release.func_ptr != NULL) {
          cmd_ptr->q3d_release.func_ptr(cmd_ptr->q3d_release.data,
                                        cmd_ptr->q3d_release.cb_func,
                                        cmd_ptr->q3d_release.client_data);
        }
        break;

      case GRAPH_Q3D_QDSP_CMD:
        if (cmd_ptr->q3d_qdsp.func_ptr != NULL) {
          cmd_ptr->q3d_qdsp.func_ptr(cmd_ptr->q3d_qdsp.cmd);
        }
        break;

#endif /* FEATURE_I3D */

#ifdef FEATURE_GRAPHICS_OPENGLES_FIRMWARE
#error code not present
#endif /* FEATURE_GRAPHICS_OPENGLES_FIRMWARE */

#ifdef FEATURE_SVG
    case GRAPH_SVG_CMD_DECODE:
      graph_svg_decode_processing( cmd_ptr);
      break;

    case GRAPH_SVG_CMD_FRAME:
      if( cmd_ptr->hdr.handle != NULL){
        svg_frame_data_type  frame_data;
        frame_data.time_ms         = cmd_ptr->svg_frame_cmd.time_ms;
        frame_data.frame_buf       = cmd_ptr->svg_frame_cmd.frame_buf;
        frame_data.frame_size      = cmd_ptr->svg_frame_cmd.frame_size;
        frame_data.seek            = cmd_ptr->svg_frame_cmd.seek;
        (void)svg_render_frame( cmd_ptr->hdr.handle, &frame_data );
      }
      break;

    case GRAPH_SVG_CMD_EVENT:
      if( cmd_ptr->hdr.handle != NULL){
        svg_process_event( cmd_ptr->hdr.handle,
                           cmd_ptr->svg_event_cmd.event,
                           (void*)&cmd_ptr->svg_event_cmd.event_data,
                           cmd_ptr->svg_event_cmd.time_ms
                         );
      }
      break;

    case GRAPH_SVG_CMD_STOP:
      if( cmd_ptr->hdr.handle != NULL){
        svg_decode_stop( cmd_ptr->hdr.handle);
      }
      graph_cmx_svg_cb     = NULL;
      break;

    case GRAPH_SVG_CMD_ABORT:
      if( cmd_ptr->hdr.handle != NULL){

        //Remove GRAPH_SVG_CMD_CB from Graph Queue
	    graph_manage_cmd_q_ex(GRAPH_SVG_CMD_CB,cmd_ptr->hdr.handle);

        svg_decode_abort( cmd_ptr->hdr.handle);
        graph_cmx_svg_cb     = NULL;
      }
      break;

    case GRAPH_SVG_CMD_PAUSE:
      if( cmd_ptr->hdr.handle != NULL){
        svg_decode_pause( cmd_ptr->hdr.handle);
      }
      break;

    case GRAPH_SVG_CMD_RESUME:
      if( cmd_ptr->hdr.handle != NULL){
        svg_decode_resume( cmd_ptr->hdr.handle);
      }
      break;

    case GRAPH_SVG_CMD_REWIND:
      if( cmd_ptr->hdr.handle != NULL){
        svg_frame_rewind( cmd_ptr->hdr.handle, cmd_ptr->svg_seek.time_ms);
      }
      break;

    case GRAPH_SVG_CMD_FFORWARD:
      if( cmd_ptr->hdr.handle != NULL){
        svg_frame_fforward( cmd_ptr->hdr.handle, cmd_ptr->svg_seek.time_ms);
      }
      break;

    case GRAPH_SVG_CMD_RESET:
      if( cmd_ptr->hdr.handle != NULL){
        svg_reset_frame( cmd_ptr->hdr.handle);
      }
      break;

    case GRAPH_SVG_CMD_CB:
      if( cmd_ptr->svg_cb.cb_func != NULL){
        cmd_ptr->svg_cb.cb_func( cmd_ptr->svg_cb.client_data);
      }
      break;

    case GRAPH_SVG_CMD_REQUEST_CB:
      svg_process_request( cmd_ptr->svg_req_cb.client_data,
                           cmd_ptr->svg_req_cb.cmx_handle,
                           cmd_ptr->svg_req_cb.filename);
      break;

    case GRAPH_SVG_CMD_SET_VIEWPORT:
      if( cmd_ptr->hdr.handle != NULL){
        svg_set_viewport( cmd_ptr->hdr.handle, cmd_ptr->svg_setviewport.width,
                          cmd_ptr->svg_setviewport.height);
      }
      break;

#ifdef FEATURE_SVGDOM
    case GRAPH_SVG_CMD_DOM:
      svg_dom_apis( cmd_ptr->hdr.handle, cmd_ptr->svg_dom_cmd.type,
                    &cmd_ptr->svg_dom_cmd.data);
      break;
#endif /* FEATURE_SVGDOM */

#endif /* FEATURE_SVG */

#ifdef FEATURE_VIDEO_MOVIEEDIT
      case GRAPH_VIDEO_MOVIEEDIT_PARSE_FUNC:
        movieedit_parse_process_func(
            cmd_ptr->video_movieedit_parse_func.func,
            cmd_ptr->video_movieedit_parse_func.callback,
            cmd_ptr->video_movieedit_parse_func.client_data,
            cmd_ptr->video_movieedit_parse_func.info1,
            cmd_ptr->video_movieedit_parse_func.info2,
            cmd_ptr->video_movieedit_parse_func.info3,
            cmd_ptr->video_movieedit_parse_func.info4);
        break;
#endif /* FEATURE_VIDEO_MOVIEEDIT */

#ifdef FEATURE_QVPHONE
    case GRAPH_QVPHONE_ENC_TOGGLE_RECORD:
        qvp_g_venc_record_flag = !qvp_g_venc_record_flag;
        qvpio_record_status_cb(qvp_g_venc_record_flag,
                               0 //QVPIO_VENC
                               );
      break;
#endif

      /* Illegal command, does not return.
      */
      case GRAPH_CMD_INVALID:
      default:
        ERR_FATAL ("Illegal vs command %x", cmd_ptr->hdr.cmd, 0, 0);
    }
    /* Return GRAPHICS packet to free q
    */
    if ( cmd_ptr )
    {
      if(cmd_ptr->hdr.overflow)
      {
        free(cmd_ptr);
      }
      else
      {
        q_put (&graph_free_q, &cmd_ptr->hdr.link);
      }
    }

  } /* end of while "commands to process" */

  if ( q_check( &graph_cmd_q ) != NULL)
  {
    (void) rex_set_sigs (&graph_tcb, GRAPH_CMD_Q_SIG);
  }
} /* end graph_cmd_parser */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the graphic task.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_tcb.

===========================================================================*/
LOCAL void graph_task_offline (void)
{

#ifdef FEATURE_MULTIMODE_ARCH
  /* Process task offline procedure from task controller.
  */
  (void) task_offline();
#else /* FEATURE_MULTIMODE_ARCH */
  /* Acknowledge the offline signal
  */
  (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs (&graph_tcb, TASK_OFFLINE_SIG);

} /* end graph_task_offline */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the graphic task.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_tcb.

===========================================================================*/
LOCAL void graph_task_shutdown (void)
{

#if defined(TEST_MPEG4_CODEC_INTERFACE) || defined(TEST_MPEG4_QDSP_INTERFACE)
    mp4test_shutdown();
#endif  /* TEST_MPEG4_CODEC_INTERFACE || TEST_MPEG4_QDSP_INTERFACE */

#ifdef FEATURE_MULTIMODE_ARCH
  /* Process task stop procedure from task controller.
  */
  (void) task_stop();
#else /* FEATURE_MULTIMODE_ARCH */
  /* Acknowledge the stop signal
  */
  (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs (&graph_tcb, TASK_STOP_SIG);

} /* end graph_task_shutdown */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_get_cb_entry

DESCRIPTION
  This procedure gets a graph_cmx_cb_type structure.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL graph_cmx_cb_type *graph_get_cb_entry (void)
{
  graph_cmx_cb_type *ret_val = NULL;
  int               counter;

  for(counter = 0; counter < GRAPH_CMX_CB_MAX_NUM; counter++) {
    if(graph_cmx_cb_array[counter].callback_ptr == NULL) {
      ret_val = &graph_cmx_cb_array[counter];
      break;
    }
  }
  return(ret_val);
} /* end graph_get_cb_entry */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_free_cb_entry

DESCRIPTION
  This procedure frees a graph_cmx_cb_type structure.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void graph_free_cb_entry
(
  graph_cmx_cb_type *cb_entry
)
{
  if(cb_entry != NULL) {
    cb_entry->callback_ptr = NULL;
  }
} /* end graph_free_cb_entry */

/* <EJECT> */
#ifdef FEATURE_PNG_DECODER
/*===========================================================================

FUNCTION graph_cmx_png_cmd_cb

DESCRIPTION
  This function issues a command in graphic task for get data and
  continue command callbacks.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_cmx_png_cmd_cb
(
  void *client_data
)
{
  graph_packet_type *packet;
  graph_cmx_cb_type *cb_entry;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    cb_entry                   = (graph_cmx_cb_type *) client_data;
    packet->hdr.cmd            = GRAPH_PNG_CMD_CB;
    packet->png_cb.cb_func     =
                   (png_cmx_command_cb_func_type) cb_entry->callback_ptr;
    packet->png_cb.client_data = cb_entry->client_data;
    graph_free_cb_entry(cb_entry);
    graph_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_callback

DESCRIPTION
  This function processes callback commands from PNG to Graphic

DEPENDENCIES
  FEATURE_PNG_DECODER,
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_callback
(
  png_cmx_status_enum_type png_status,
  void                     *handle,
  void                     *server_data
)
{
  png_cmx_get_data_type png_get_data;
  png_cmx_continue_type png_continue;
  graph_cmx_cb_type     *cb_entry;
  void                  *graph_data = NULL;

  if (graph_cmx_png_cb != NULL) {
    if (server_data != NULL) {
      switch(png_status) {
        case PNG_CMX_GET_DATA:
          png_get_data = *((png_cmx_get_data_type *) server_data);
          if(png_get_data.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) png_get_data.callback_ptr;
              cb_entry->client_data     = png_get_data.client_data;
              png_get_data.callback_ptr = graph_cmx_png_cmd_cb;
              png_get_data.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *)(&png_get_data);
          break;


        case PNG_CMX_CONTINUE:
          png_continue = *((png_cmx_continue_type *) server_data);
          if (png_continue.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) png_continue.callback_ptr;
              cb_entry->client_data     = png_continue.client_data;
              png_continue.callback_ptr = graph_cmx_png_cmd_cb;
              png_continue.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *) (&png_continue);
          break;

        case PNG_CMX_SPEC:
        case PNG_CMX_GET_FILE_SIZE:
        case PNG_CMX_DATA:
        case PNG_CMX_TEXT:
        case PNG_CMX_ABORT:
        case PNG_CMX_FAILURE:
        case PNG_CMX_DONE:
          graph_data = server_data;
          break;

        default:
          MSG_LOW("Unknown png status",0,0,0);
          graph_data = server_data;
          break;
      }
    }
    graph_cmx_png_cb(png_status, handle, graph_data);
  }
} /* end graph_png_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmd_search

DESCRIPTION
  This function compares the value and returns TRUE or FALSE.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  TRUE  - search item found.
  FALSE - search item not found.

SIDE EFFECTS
  None.

===========================================================================*/
int graph_cmd_search
(
  void* item_ptr,
  void* compare_val
)
{
  if (((graph_packet_type *)item_ptr)->hdr.cmd ==
      *((graph_cmd_type *)compare_val)) {
    return TRUE;
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_manage_cmd_q

DESCRIPTION
  This function checks for and removes the indicated command from the command
  queue.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_free_q.

===========================================================================*/
void graph_manage_cmd_q
(
  graph_cmd_type cmd
)
{
  graph_packet_type *cmd_ptr;

  cmd_ptr = (graph_packet_type *)q_linear_search(&graph_cmd_q,
                                                 graph_cmd_search, &cmd);
  if (cmd_ptr != NULL) {
    q_delete(&graph_cmd_q, &cmd_ptr->hdr.link);
    q_put(&graph_free_q, &cmd_ptr->hdr.link);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_manage_cmd_q_ex

DESCRIPTION
  This function checks for and removes the indicated command and handle from 
  the command queue. 

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_free_q.

===========================================================================*/
void graph_manage_cmd_q_ex
(
  graph_cmd_type cmd,
  void*          handle
)
{
  graph_packet_type *cmd_ptr;

  cmd_ptr = (graph_packet_type *)q_linear_search(&graph_cmd_q,
                                                 graph_cmd_search, &cmd);
  if (cmd_ptr != NULL && cmd_ptr->hdr.handle == handle) {
    q_delete(&graph_cmd_q, &cmd_ptr->hdr.link);
    q_put(&graph_free_q, &cmd_ptr->hdr.link);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_decode

DESCRIPTION
  This function issues a command in graphic task for png_decode

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_decode
(
  void                        *handle,
  png_cmx_decode_cb_func_type callback_ptr
)
{
  graph_packet_type *packet;

  graph_manage_cmd_q(GRAPH_PNG_CMD_DECODE);

  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_PNG_CMD_DECODE;
    packet->png_cmd.handle   = handle;
    packet->png_cmd.cb_func  = callback_ptr;
    graph_cmd(packet);
  }
} /* end graph_png_decode */

#ifdef FEATURE_PNG_ENCODER
/* <EJECT> */
/*===========================================================================

FUNCTION graph_queue_png_msg

DESCRIPTION
  This function issues a command in graphic task for png encode requests

DEPENDENCIES
  FEATURE_PNG_ENCODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_encode(void )

{
  graph_packet_type *packet;

  //graph_manage_cmd_q(GRAPH_PNG_CMD_ENCODE);

  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_PNGE_CMD;
    packet->png_cmd.handle   = NULL;;
    packet->png_cmd.cb_func  = NULL; //callback_ptr;
    graph_cmd(packet);
  }
} /* end graph_png_encode */

void graph_queue_pnge_msg( PNGE_status_enum_type cmd )

{
  graph_packet_type *packet;


  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_PNGE_MSG;
    packet->png_cmd.handle   = (void *) cmd;
    packet->png_cmd.cb_func  = NULL; //callback_ptr;
    graph_cmd(packet);
  }
} /* end graph_queue_png_msg */

#endif /* FEATURE_PNG_ENCODER */
/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_decode_abort

DESCRIPTION
  This function issues a command in graphic task for png_decode_abort

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_decode_abort (void)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_PNG_CMD_ABORT;
    graph_cmd(packet);
  }
} /* end graph_png_decode_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_png_get_image_specs

DESCRIPTION
  This function issues a command to pass PNG image's width and height,
  bit depth, color type, compression method, filter method, and
  interlace method.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_get_image_specs
(
  void                        *handle,
  png_cmx_decode_cb_func_type callback_ptr
)
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd           = GRAPH_PNG_CMD_SPEC;
    packet->png_cmd.handle    = handle;
    packet->png_cmd.cb_func   = callback_ptr;
    graph_cmd(packet);
  }
} /* end graph_png_set_image_specs */

/* <EJECT> */
/*===========================================================================
FUNCTION graph_png_set_buf

DESCRIPTION
  This function issues a command to setup the PNG internal buffers
  to client supplied buffers.

DEPENDENCIES
  FEATURE_PNG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_png_set_buf
(
  uint8 *client_buffer,                     /* client buffer               */
  uint32 client_buffer_size,                /* size of client buffer       */
  uint8 *output_buffer,                     /* output buffer               */
  uint32 output_buffer_size                 /* size of output buffer       */
)
{
  graph_packet_type *packet;

  graph_manage_cmd_q(GRAPH_PNG_CMD_BUF);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                 = GRAPH_PNG_CMD_BUF;
    packet->png_buf.client_buf      = client_buffer;
    packet->png_buf.client_buf_size = client_buffer_size;
    packet->png_buf.output_buf      = output_buffer;
    packet->png_buf.output_buf_size = output_buffer_size;
    graph_cmd(packet);
  }
} /* end graph_png_set_buf */

#endif /* FEATURE_PNG_DECODER */

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
)
{
    graph_packet_type *packet;

    packet = graph_cmd_get_pkt ();
    if (packet != NULL) {
       packet->hdr.cmd                  = GRAPH_CONTEXT_REQ;
       packet->context_req.packet.function_ptr = context_data.function_ptr;
       packet->context_req.packet.client_ID    = context_data.client_ID;
       packet->context_req.packet.client_data  = context_data.client_data;
       graph_cmd (packet);
    }
}

/* <EJECT> */
#ifdef FEATURE_JPEG_DECODER
/*===========================================================================

FUNCTION graph_cmx_jpeg_cmd_cb

DESCRIPTION
  This function issues a command in graphic task for get data and
  continue command callbacks.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_cmx_jpeg_cmd_cb
(
  void *client_data
)
{
  graph_packet_type *packet;
  graph_cmx_cb_type *cb_entry;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    cb_entry                   = (graph_cmx_cb_type *) client_data;
    packet->hdr.cmd            = GRAPH_JPEG_CMD_CB;
    packet->jpeg_cb.cb_func     =
                   (jpeg_cmx_command_cb_func_type) cb_entry->callback_ptr;
    packet->jpeg_cb.client_data = cb_entry->client_data;
    graph_free_cb_entry(cb_entry);
    graph_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_callback

DESCRIPTION
  This function processes callback commands from PNG to Graphic

DEPENDENCIES
  FEATURE_JPEG_DECODER,
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_jpeg_callback
(
  jpeg_cmx_status_enum_type jpeg_status,
  void                     *handle,
  void                     *server_data
)
{
  jpeg_cmx_get_data_type jpeg_get_data;
  jpeg_cmx_continue_type jpeg_continue;
  jpeg_cmx_buf_req_type  jpeg_buf_req;
  jpeg_dsp_req_type      jpeg_dsp_req;
  graph_packet_type      *packet;
  graph_cmx_cb_type     *cb_entry;
  void                  *graph_data = NULL;

  if (graph_cmx_jpeg_cb != NULL) {
    if (server_data != NULL) {
      switch(jpeg_status) {

        case JPEG_CMX_GET_DATA:
          jpeg_get_data = *((jpeg_cmx_get_data_type *) server_data);
          if(jpeg_get_data.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) jpeg_get_data.callback_ptr;
              cb_entry->client_data     = jpeg_get_data.client_data;
              jpeg_get_data.callback_ptr = graph_cmx_jpeg_cmd_cb;
              jpeg_get_data.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *)(&jpeg_get_data);
          break;


        case JPEG_CMX_CONTINUE:
          jpeg_continue = *((jpeg_cmx_continue_type *) server_data);
          if (jpeg_continue.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) jpeg_continue.callback_ptr;
              cb_entry->client_data     = jpeg_continue.client_data;
              jpeg_continue.callback_ptr = graph_cmx_jpeg_cmd_cb;
              jpeg_continue.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *) (&jpeg_continue);
          break;

        case JPEG_CMX_GET_BUF:
          jpeg_buf_req = *((jpeg_cmx_buf_req_type *) server_data);
          if (jpeg_buf_req.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) jpeg_buf_req.callback_ptr;
              cb_entry->client_data     = jpeg_buf_req.client_data;
              jpeg_buf_req.callback_ptr = graph_cmx_jpeg_cmd_cb;
              jpeg_buf_req.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *) (&jpeg_buf_req);
          break;

        case JPEG_DSP_REQUEST:
          jpeg_dsp_req = *((jpeg_dsp_req_type *) server_data);

          packet = graph_cmd_get_pkt ();
          if (packet != NULL) {
            packet->hdr.cmd                     = GRAPH_DSP_REQUEST;
            packet->dsp_request.function_ptr    = jpeg_dsp_req.function_ptr;
            packet->dsp_request.client_data     = jpeg_dsp_req.client_data;
            graph_cmd (packet);
          }
          return;
          break;

        case JPEG_CMX_SPEC:
        case JPEG_CMX_GET_FILE_SIZE:
        case JPEG_CMX_DATA:
        case JPEG_CMX_ABORT:
        case JPEG_CMX_FAILURE:
        case JPEG_CMX_DONE:
          graph_data = server_data;
          break;

        default:
          MSG_LOW("Unknown JPEG status",0,0,0);
          graph_data = server_data;
          break;
      }
    }
    graph_cmx_jpeg_cb(jpeg_status, handle, graph_data);
  }
} /* end graph_jpeg_callback */

/* <EJECT> */
/*===========================================================================
FUNCTION graph_jpeg_set_buf

DESCRIPTION
  This function issues a command to setup the JPEG internal buffers
  to client supplied buffers.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_jpeg_set_buf
(
  uint8   *client_buffer,
  uint32  client_buffer_size,
  uint8   *output_buffer                    /* output buffer               */
)
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                 = GRAPH_JPEG_CMD_BUF;
    packet->jpeg_buf.client_buf      = client_buffer;
    packet->jpeg_buf.client_buf_size = client_buffer_size;
    packet->jpeg_buf.output_buf      = output_buffer;
    graph_cmd(packet);
  }
} /* end graph_jpeg_set_buf */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_decode

DESCRIPTION
  This function issues a command in graphic task for jpeg_decode

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_jpeg_decode
(
  void                        *handle,
  jpeg_cmx_cb_func_type       callback_ptr,
  jpeg_decodeInfo_type      jpegdInfo
)
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_JPEG_CMD_DECODE;
    packet->jpeg_decode_cmd.handle   = handle;
    packet->jpeg_decode_cmd.cb_func  = callback_ptr;
    packet->jpeg_decode_cmd.jpegdInfo = jpegdInfo;

    
    graph_cmd(packet);
  }
} /* end graph_jpeg_decode */

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
extern void graph_jpeg_dec_qdsp_ready()
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_JPEG_CMD_DEC_QDSP_READY;
    graph_cmd(packet);
  }
}

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
extern void graph_jpeg_dec_image_abort()
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt())!=NULL) {
    packet->hdr.cmd          = GRAPH_JPEG_CMD_DEC_IMAGE_ABORT;
    graph_cmd(packet);
    rex_set_sigs( &graph_tcb, JPEG_DECODE_DATA_SIG);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_get_image_specs

DESCRIPTION
  This function issues a command to pass JPEG image's width and height,
  and bit depth.

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_jpeg_get_image_specs
(
  void                        *handle,
  jpeg_cmx_cb_func_type       callback_ptr
)
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd           = GRAPH_JPEG_CMD_SPEC;
    packet->jpeg_decode_cmd.handle    = handle;
    packet->jpeg_decode_cmd.cb_func   = callback_ptr;
    graph_cmd(packet);
  }
} /* end graph_jpeg_set_image_specs */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_jpeg_abort_decode

DESCRIPTION
  This function issues a command in graphic task for jpeg_abort_decode

DEPENDENCIES
  FEATURE_JPEG_DECODER.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_jpeg_abort_decode (void)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_JPEG_CMD_ABORT_DECODE;
    graph_cmd(packet);
    rex_set_sigs( &graph_tcb, JPEG_DECODE_DATA_SIG);
  }
} /* end graph_jpeg_abort_decode */

/*===========================================================================

FUNCTION GRAPH_GET_ASYNC_DATA_CB

DESCRIPTION

  This operation sets JPEG_DECODE_DATA_SIG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  JPEG_DECODE_DATA_SIG.
===========================================================================*/
LOCAL void graph_get_async_data_cb( void* cb_data)
{
  (void) rex_set_sigs(&graph_tcb, JPEG_DECODE_DATA_SIG);
  }

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
boolean graph_get_async_data(
jpeg_cmx_status_enum_type jpeg_status,
void                     *handle,
void                     *server_data
)
{
  rex_sigs_type sigs;
  boolean       ret_val = TRUE;
  jpeg_cmx_get_data_type *jpeg_get_data = (jpeg_cmx_get_data_type*)server_data;

  jpeg_get_data->callback_ptr = (jpeg_cmx_command_cb_func_type)graph_get_async_data_cb;
  jpeg_get_data->client_data  = NULL;

  (void) rex_clr_sigs(&graph_tcb, JPEG_DECODE_DATA_SIG);
  graph_cmx_jpeg_cb(jpeg_status, handle, server_data);

  sigs = rex_get_sigs(&graph_tcb);

  while((sigs & JPEG_DECODE_DATA_SIG) != JPEG_DECODE_DATA_SIG)
{
     dog_report(DOG_GRAPH_RPT);
     (void) rex_set_timer( &graph_rpt_timer, DOG_GRAPH_RPT_TIME );
     sigs = rex_wait(JPEG_DECODE_DATA_SIG|GRAPH_RPT_TIMER_SIG);

}

  (void) rex_clr_sigs(&graph_tcb, JPEG_DECODE_DATA_SIG);

  return(ret_val);

  }

#endif /* FEATURE_JPEG_DECODER */

/* <EJECT> */
#ifdef FEATURE_SAF
/*===========================================================================

FUNCTION graph_cmx_saf_cmd_cb

DESCRIPTION
  This function issues a command in graphic task for get data command
  callback from cmx to graphic

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_cmx_saf_cmd_cb
(
  void *client_data
)
{
  graph_packet_type *packet;
  graph_cmx_cb_type *cb_entry;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    cb_entry                   = (graph_cmx_cb_type *) client_data;
    packet->hdr.cmd            = GRAPH_SAF_CMD_CB;
    packet->saf_cb.cb_func     =
                   (saf_cmx_command_cb_func_ptr_type) cb_entry->callback_ptr;
    packet->saf_cb.client_data = cb_entry->client_data;
    graph_free_cb_entry(cb_entry);
    graph_cmd(packet);
  }
} /* end graph_cmx_saf_cmd_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmx_saf_frame_cmd

DESCRIPTION
  This function issues a command in graphic task for prepare frame done
  command between cmx and graphic.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_cmx_saf_frame_cmd
(
  int32   frameid,
  uint8   *framebuf,
  void    *client_data
)
{
  graph_packet_type *packet;

  graph_manage_cmd_q(GRAPH_SAF_CMD_FRAME);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd               = GRAPH_SAF_CMD_FRAME;
    packet->saf_frame.frameid     = frameid;
    packet->saf_frame.framebuf    = framebuf;
    packet->saf_frame.client_data = client_data;
    graph_cmd(packet);
  }
} /* end graph_cmx_saf_frame_cmd */

/* <EJECT> */
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
void graph_saf_callback
(
  saf_cmx_status_type saf_status,
  void                *handle,
  void                *server_data
)
{
  saf_cmx_get_data_type     saf_get_data;
  saf_cmx_continue_type     saf_continue;
  saf_cmx_prepare_done_type saf_prepare_done;
  graph_cmx_cb_type         *cb_entry;
  void                      *graph_data = NULL;

  if (graph_cmx_saf_cb != NULL) {
    if (server_data != NULL) {
      switch(saf_status) {
        case SAF_CMX_GET_DATA:
          saf_get_data = *((saf_cmx_get_data_type *) server_data);
          if (saf_get_data.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) saf_get_data.callback_ptr;
              cb_entry->client_data     = saf_get_data.client_data;
              saf_get_data.callback_ptr = graph_cmx_saf_cmd_cb;
              saf_get_data.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *) (&saf_get_data);
          break;

        case SAF_CMX_CONTINUE:
          saf_continue = *((saf_cmx_continue_type *) server_data);
          if (saf_continue.callback_ptr != NULL) {
            cb_entry = graph_get_cb_entry();
            if(cb_entry != NULL) {
              cb_entry->callback_ptr    = (void *) saf_continue.callback_ptr;
              cb_entry->client_data     = saf_continue.client_data;
              saf_continue.callback_ptr = graph_cmx_saf_cmd_cb;
              saf_continue.client_data  = (void *) cb_entry;
            } else {
              MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
              return;
            }
          }
          graph_data = (void *) (&saf_continue);
          break;

        case SAF_CMX_PREPARE_DONE:
          saf_prepare_done = *((saf_cmx_prepare_done_type *) server_data);
          if (saf_prepare_done.callback_ptr != NULL) {
            graph_saf_frame_cmd_cb        = saf_prepare_done.callback_ptr;
            saf_prepare_done.callback_ptr = graph_cmx_saf_frame_cmd;
          }
          graph_data = (void *) (&saf_prepare_done);
          break;

        case SAF_CMX_GET_FILE_SIZE:
        case SAF_CMX_INFO:
        case SAF_CMX_FRAME_DONE:
        case SAF_CMX_ABORT:
        case SAF_CMX_FAIL:
          graph_data = server_data;
          break;

        default:
          MSG_LOW("Unknown status",0,0,0);
          graph_data = server_data;
          break;
      } /* end of switch */
    } /* end of server_data */

    graph_cmx_saf_cb(saf_status, handle, graph_data);
  }
} /* end graph_saf_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_decode

DESCRIPTION
  This function issues a command to start SAF decoder.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_saf_decode
(
  void                            *handle,
  saf_cmx_decode_cb_func_ptr_type callback_ptr
)
{
  graph_packet_type               *packet;

  graph_manage_cmd_q(GRAPH_SAF_CMD_DECODE);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SAF_CMD_DECODE;
    packet->saf_cmd.handle      = handle;
    packet->saf_cmd.cb_func     = callback_ptr;
    graph_cmd(packet);
  }
} /* graph_saf_decode */

/* <EJECT> */
/*===========================================================================

FUNCTION  graph_saf_decode_stop

DESCRIPTION
  This function issues a command to stop the current SAF decoder session.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_saf_decode_stop(void)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_SAF_CMD_STOP;
    graph_cmd(packet);
  }
} /* graph_saf_decode_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_decode_abort

DESCRIPTION
  This function issues a command to abort the animation playback.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_saf_decode_abort(void)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_SAF_CMD_ABORT;
    graph_cmd(packet);
  }
} /* graph_saf_decode_abort */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_get_info

DESCRIPTION
  This function issues a commad to pass the specifications for the animation
  contained in the SAF file.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_saf_get_info
(
  void                            *handle,
  saf_cmx_decode_cb_func_ptr_type callback_ptr
)
{
  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SAF_CMD_INFO;
    packet->saf_cmd.handle      = handle;
    packet->saf_cmd.cb_func     = callback_ptr;
    graph_cmd(packet);
  }
} /* graph_saf_get_info */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_saf_global_init

DESCRIPTION
  This function issues a command in graphic task to initial the
  SAF decoders global data/buffers.

DEPENDENCIES
  FEATURE_SAF.
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_saf_display_info_data.

===========================================================================*/
void graph_saf_global_init
(
  saf_display_resource_info_type    *display_info_data,
  uint8                             *g_object_buf,
  uint32                            g_object_buf_size,
  uint8                             *g_coder_buf,
  uint32                            g_coder_buf_size,
  uint8                             *g_info_buf
)
{
  graph_packet_type *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                    = GRAPH_SAF_CMD_INIT;
    graph_saf_display_info_data        = *display_info_data;
    packet->saf_init.display_info_data = &graph_saf_display_info_data;
    packet->saf_init.g_obj_buf         = g_object_buf;
    packet->saf_init.g_obj_buf_size    = g_object_buf_size;
    packet->saf_init.g_cod_buf         = g_coder_buf;
    packet->saf_init.g_cod_buf_size    = g_coder_buf_size;
    packet->saf_init.g_info_buf        = g_info_buf;
    graph_cmd(packet);
  }

} /* graph_saf_global_init */
#endif /* FEATURE_SAF */

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

/*===========================================================================

FUNCTION graph_video_eng_queue_cmd

DESCRIPTION
  This function sends a command to the graph task requesting that it
  apply a pending video engine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void graph_video_eng_queue_cmd(graph_video_eng_msg_enum msg,
                               void *response)
{
  graph_packet_type *packet;

  packet = graph_cmd_get_pkt();
  if(packet != NULL )
  {
    packet->hdr.cmd                = GRAPH_VIDEO_ENG_CB;
    packet->video_eng_msg.msg      = msg;
    packet->video_eng_msg.response = response;

     graph_cmd(packet);
  }
  else
  {
    ERR_FATAL( "No graph cmd buffers", 0, 0, 0 );
  }
}

#endif /* ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */


#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================
Function: graph_transcoder_queue_cmd

Description:  Call this function to queue a command to transcoder.

Return Value:
   void

Comments:

Side Effects:

============================================================================*/
void graph_transcoder_queue_cmd( mp4_transcoder_state cmd, void *handle
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
								,uint32 tcb
#endif /*FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE*/
								)
{
  graph_packet_type *packet;

  if ( (packet = graph_cmd_get_pkt()) != NULL)
  {
    packet->hdr.cmd = GRAPH_TRANSCODER;
    packet->transcoder.msg.state = cmd;
    packet->transcoder.msg.handle = handle;
#ifdef FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE
    packet->transcoder.msg.tcb    = tcb;
#endif /*FEATURE_VIDEO_IMOVIEEDIT_TRANSCODE*/	
    graph_cmd(packet);
  }
  else
  {
    ERR_FATAL("no graph cmd buffer",0,0,0);
  }

}
#endif // FEATURE_MP4_TRANSCODER


/* <EJECT> */
#ifdef FEATURE_CAMERA
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
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_CAMERA_FUNC;
    packet->camera_func.func = func;
    packet->camera_func.callback = callback;
    packet->camera_func.client_data = client_data;
    packet->camera_func.info1 = info1;
    packet->camera_func.info2 = info2;
    packet->camera_func.info3 = info3;
    packet->camera_func.info4 = info4;
    graph_cmd(packet);
  }
} /* graph_queue_camqdsp_msg */


/*===========================================================================

FUNCTION graph_queue_camqdsp_msg

DESCRIPTION
  This function queues CAMQDSP msg

DEPENDENCIES
  FEATURE_CAMERA
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void graph_queue_camqdsp_msg
(
  uint32 msg,
  void * buf,
  uint32 size
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_CAMQDSP_CB;
    packet->camqdsp_cb.msg = msg;
    packet->camqdsp_cb.buf = buf;
    packet->camqdsp_cb.size = size;
    graph_cmd(packet);
  }
} /* graph_queue_camqdsp_msg */
#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

FUNCTION graph_queue_offline_encode_qdsp_msg

DESCRIPTION
  This function queues CAMQDSP msg for transcoder

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
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_CAMQDSP_OFFLINE_ENCODE_CB;
    packet->camqdsp_cb.msg = msg;
    packet->camqdsp_cb.buf = buf;
    packet->camqdsp_cb.size = size;
    graph_cmd(packet);
  }
} /* graph_queue_offline_encode_qdsp_msg */
#endif

#endif /* FEATURE_CAMERA */


#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
#ifdef FEATURE_VIDEO_ENCODE_RAW
/*===========================================================================

FUNCTION graph_queue_raw_encode_qdsp_msg

DESCRIPTION
  This function queues CAMQDSP msg for transcoder

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
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_CAMQDSP_RAW_ENCODE_CB;
    packet->camqdsp_cb.msg = msg;
    packet->camqdsp_cb.buf = buf;
    packet->camqdsp_cb.size = size;
    graph_cmd(packet);
  }
} /* graph_queue_raw_encode_qdsp_msg */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_queue_raw_encode_qdsp_msg

DESCRIPTION
  This function queues CAMQDSP msg for transcoder

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
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_VIDEO_ENGINE_RAW_ENCODE_CB;
    packet->raw_encoder.status = status;
    packet->raw_encoder.client_data = client_data;
    graph_cmd(packet);
  }
} /* graph_queue_raw_encode_qdsp_msg */
#endif /* FEATURE_VIDEO_ENCODE_RAW */


/* <EJECT> */
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
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_VIDEO_ENC_CMD_FRAME_DONE;
    packet->video_enc_frame.data_buf = data_buf;
    graph_cmd(packet);
  }
} /* graph_video_enc_frame_done */

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
  uint32 func,
  void *callback,
  void *client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
)
{
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd = GRAPH_VIDEO_MOVIEEDIT_PARSE_FUNC;
    packet->video_movieedit_parse_func.func = func;
    packet->video_movieedit_parse_func.callback = callback;
    packet->video_movieedit_parse_func.client_data = client_data;
    packet->video_movieedit_parse_func.info1 = info1;
    packet->video_movieedit_parse_func.info2 = info2;
    packet->video_movieedit_parse_func.info3 = info3;
    packet->video_movieedit_parse_func.info4 = info4;
    graph_cmd(packet);
  }
} /* graph_queue_video_movieedit_parse_msg */
#endif /* FEATURE_VIDEO_MOVIEEDIT */


#ifdef FEATURE_I3D
/*===========================================================================

FUNCTION graph_q3d_init

DESCRIPTION
  This function queue up a q3d init function.

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
  graph_q3d_status_cb_func_type callback_ptr,
  void                          *client_data,
  graph_q3d_init_func_ptr_type  func_ptr
) {
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd              = GRAPH_Q3D_INIT;
    packet->q3d_init.cb_func     = callback_ptr;
    packet->q3d_init.client_data = client_data;
    packet->q3d_init.func_ptr    = func_ptr;
    graph_cmd(packet);
    return (GRAPH_Q3D_SUCCESS);
  }
  return (GRAPH_Q3D_FAILURE);
} /* graph_q3d_init */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_draw_model

DESCRIPTION
  This function queue up a draw model function.

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
) {
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd            = GRAPH_Q3D_DRAW_MODEL;
    packet->q3d_model.ctx      = ctx;
    packet->q3d_model.model    = model;
    packet->q3d_model.func_ptr = func_ptr;
    graph_cmd(packet);
    return (GRAPH_Q3D_SUCCESS);
  }
  return (GRAPH_Q3D_FAILURE);
} /* graph_q3d_dsp_draw_model */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_render

DESCRIPTION
  This function queues up a q3d render function.

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
) {
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                = GRAPH_Q3D_RENDER_FRAME;
    packet->q3d_render.ctx         = ctx;
    packet->q3d_render.cb_func     = cb_func;
    packet->q3d_render.client_data = client_data;
    packet->q3d_render.func_ptr    = func_ptr;
    graph_cmd(packet);
    return (GRAPH_Q3D_SUCCESS);
  }
  return (GRAPH_Q3D_FAILURE);
} /* graph_q3d_render */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_release

DESCRIPTION
  This function queue up a release function.

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
) {
  graph_packet_type   *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                 = GRAPH_Q3D_RELEASE;
    packet->q3d_release.data        = data;
    packet->q3d_release.cb_func     = cb_func;
    packet->q3d_release.client_data = client_data;
    packet->q3d_release.func_ptr    = func_ptr;
    graph_cmd(packet);
    return (GRAPH_Q3D_SUCCESS);
  }
  return (GRAPH_Q3D_FAILURE);
} /* graph_q3d_release */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_int_data_search

DESCRIPTION
  This function compares the value and returns TRUE or FALSE.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  TRUE  - search item found.
  FALSE - search item not found.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int graph_q3d_qdsp_cmd_search
(
  void* item_ptr,
  void* compare_val
)
{
  if ((((graph_packet_type *)item_ptr)->hdr.cmd == GRAPH_Q3D_QDSP_CMD) &&
      (((graph_packet_type *)item_ptr)->q3d_qdsp.cmd ==
                                                *((uint32*)compare_val))) {
    return TRUE;
  }
  return FALSE;
} /* graph_q3d_qdsp_cmd_search */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_manage_cmd_q

DESCRIPTION
  This function checks for and removes the indicated command from the command
  queue.

DEPENDENCIES
  FEATURE_GRAPH_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  graph_cmd_q.
  graph_free_q.

===========================================================================*/
LOCAL void graph_q3d_manage_cmd_q
(
  uint32      msg
)
{
  graph_packet_type *cmd_ptr;

  cmd_ptr = (graph_packet_type *)q_linear_search(&graph_cmd_q,
                                                 graph_q3d_qdsp_cmd_search,
                                                 &msg);
  if (cmd_ptr != NULL) {
    q_delete(&graph_cmd_q, &cmd_ptr->hdr.link);
    q_put(&graph_free_q, &cmd_ptr->hdr.link);
  }
} /* graph_q3d_manage_cmd_q */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_q3d_qdsp_cmd

DESCRIPTION
  This function queue up a qdsp request mssage.

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
  uint16                       cmd,
  graph_q3d_qdsp_cmd_func_ptr_type func_ptr
) {
  graph_packet_type   *packet;

  graph_q3d_manage_cmd_q (cmd);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd           = GRAPH_Q3D_QDSP_CMD;
    packet->q3d_qdsp.cmd      = cmd;
    packet->q3d_qdsp.func_ptr = func_ptr;
    graph_cmd(packet);
  }
} /* graph_queue_q3dq_dsp_cmd */
#endif /* FEATURE_I3D */

#ifdef FEATURE_GRAPHICS_OPENGLES_CL
#error code not present
#endif /*FEATURE_GRAPHICS_OPENGLES_CL */


/* <EJECT> */
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
)
{
  if(video_encoder != NULL)
  {
    TASKLOCK();
    graph_video_encoder = video_encoder;
    TASKFREE();
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* graph_qvp_register */

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
void graph_qvp_deregister()
{
  TASKLOCK();
#ifdef FEATURE_VIDEO_ENCODE
  graph_video_encoder = video_encode_frame;
#else
  graph_video_encoder = NULL;
#endif
  TASKFREE();

} /* graph_qvp_register */
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
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                   = GRAPH_SVG_CMD_DECODE;
    packet->svg_decode_cmd.cb_func    = svg_load_data->cb_func;
    packet->svg_decode_cmd.cb_data    = svg_load_data->cb_data;
    packet->svg_decode_cmd.mem_buffer = svg_load_data->mem_buffer;
    packet->svg_decode_cmd.mem_buffer_size   = svg_load_data->mem_buffer_size;
    packet->svg_decode_cmd.width      = svg_load_data->svg_frame_width;
    packet->svg_decode_cmd.height     = svg_load_data->svg_frame_height;
    packet->svg_decode_cmd.dpi        = svg_load_data->dpi;
    packet->svg_decode_cmd.bk_color   = svg_load_data->bk_color;
    packet->svg_decode_cmd.transparency      = svg_load_data->transparency;
    packet->svg_decode_cmd.max_requests      = svg_load_data->max_requests;
    packet->svg_decode_cmd.filename      = svg_load_data->filename;
    packet->svg_decode_cmd.progressive= svg_load_data->progressive;

    graph_cmd(packet);
  } else if( svg_load_data->cb_func != NULL){
    svg_load_data->cb_func( SVG_CMX_FAIL, svg_load_data->cb_data, 0);
  }
}

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
  void*                handle,
  svg_frame_data_type* frame_data
){
  graph_packet_type               *packet;
  graph_cmd_type                   cmd = GRAPH_SVG_CMD_FRAME;

  packet = (graph_packet_type *)q_linear_search(&graph_cmd_q,
                                               graph_cmd_search, &cmd);
  if (packet != NULL && packet->hdr.handle == handle) {
    packet->svg_frame_cmd.time_ms       = frame_data->time_ms;
    packet->svg_frame_cmd.frame_buf     = frame_data->frame_buf;
    packet->svg_frame_cmd.frame_size    = frame_data->frame_size;
    packet->svg_frame_cmd.seek          = frame_data->seek;
    return;
  }

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                     = GRAPH_SVG_CMD_FRAME;
    packet->hdr.handle                  = handle;
    packet->svg_frame_cmd.time_ms       = frame_data->time_ms;
    packet->svg_frame_cmd.frame_buf     = frame_data->frame_buf;
    packet->svg_frame_cmd.frame_size    = frame_data->frame_size;
    packet->svg_frame_cmd.seek          = frame_data->seek;
    graph_cmd(packet);
  }
}

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
  void*                   handle,
  cmx_svg_event_enum_type event, 
  void*                   event_data, 
  uint32                  time_ms
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd                  = GRAPH_SVG_CMD_EVENT;
    packet->hdr.handle               = handle;
    packet->svg_event_cmd.event      = event;
    packet->svg_event_cmd.time_ms    = time_ms;
    memcpy( (void*)&packet->svg_event_cmd.event_data, event_data,
            sizeof(cmx_svg_event_type));
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_DECODE, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_FRAME, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_EVENT, handle);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_STOP;
    packet->hdr.handle          = handle;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_DECODE, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_FRAME, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_EVENT, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_REQUEST_CB, handle);
  graph_manage_cmd_q_ex(GRAPH_SVG_CMD_ABORT, handle);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_ABORT;
    packet->hdr.handle          = handle;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  graph_manage_cmd_q_ex( GRAPH_SVG_CMD_PAUSE, handle);
  graph_manage_cmd_q_ex( GRAPH_SVG_CMD_RESUME, handle);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_PAUSE;
    packet->hdr.handle          = handle;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  graph_manage_cmd_q_ex( GRAPH_SVG_CMD_PAUSE, handle);
  graph_manage_cmd_q_ex( GRAPH_SVG_CMD_RESUME, handle);

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_RESUME;
    packet->hdr.handle          = handle;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_REWIND;
    packet->hdr.handle          = handle;
    packet->svg_seek.time_ms    = time_ms;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_FFORWARD;
    packet->hdr.handle          = handle;
    packet->svg_seek.time_ms    = time_ms;
    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_RESET;
    packet->hdr.handle          = handle;
    graph_cmd(packet);
  }
}

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
){
  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_SET_VIEWPORT;
    packet->hdr.handle          = handle;
    packet->svg_setviewport.width = width;
    packet->svg_setviewport.height = height;

    graph_cmd(packet);
  }
}

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
){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_DOM;
    packet->hdr.handle          = handle;
    packet->svg_dom_cmd.type    = type;
    packet->svg_dom_cmd.data    = *(svg_dom_data_type*)data;
    graph_cmd(packet);
  }
}
#endif /* FEATURE_SVGDOM */

/* <EJECT> */
/*===========================================================================

FUNCTION graph_cmx_svg_cmd_cb

DESCRIPTION
  This function is used as callback for CMX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_cmx_svg_cmd_cb( void* client_data){
  graph_packet_type *packet;
  graph_cmx_cb_type *cb_entry;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    cb_entry                   = (graph_cmx_cb_type *) client_data;
    packet->hdr.cmd            = GRAPH_SVG_CMD_CB;
    packet->svg_cb.cb_func     = (svg_decoder_cb_type) cb_entry->callback_ptr;
    packet->svg_cb.client_data = cb_entry->client_data;
    graph_free_cb_entry(cb_entry);
    graph_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_request_cb

DESCRIPTION
  This function issues a command to SVG decoder to process external requests 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_cmx_svg_request_cb( void* client_data, void* cmx_handle, char* filename){

  graph_packet_type               *packet;

  if ((packet = graph_cmd_get_pkt()) != NULL) {
    packet->hdr.cmd             = GRAPH_SVG_CMD_REQUEST_CB;
    packet->svg_req_cb.client_data = client_data;
    packet->svg_req_cb.cmx_handle  = cmx_handle;

#ifdef GRAPH_USES_LIBSTD
    std_strlcpy(packet->svg_req_cb.filename, filename, FS_FILENAME_MAX_LENGTH_P - 1);
#else
    strlcpy(packet->svg_req_cb.filename, filename, FS_FILENAME_MAX_LENGTH_P - 1);
#endif /* GRAPH_USES_LIBSTD */

    graph_cmd(packet);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION graph_svg_callback

DESCRIPTION
  This function is a cllback for SVG decoder

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void graph_svg_callback( svg_cmx_status_type status, void* handle, void* data){

  void* graph_data    = NULL;

  svg_cmx_get_data_type*  svg_get_data;
  svg_cmx_continue_type*  svg_continue;   
  svg_cmx_request_type    svg_request; 
  svg_cmx_req_data_type   svg_req_data;
  graph_cmx_cb_type*      cb_entry;
  graph_packet_type*      packet;

  switch( status){
  case SVG_CMX_GET_DATA:
    svg_get_data = ((svg_cmx_get_data_type *) data);

    if (svg_get_data->callback_ptr != NULL) {
      cb_entry = graph_get_cb_entry();
      if(cb_entry != NULL) {
        cb_entry->callback_ptr    = (void *) svg_get_data->callback_ptr;
        cb_entry->client_data     = svg_get_data->client_data;
        svg_get_data->callback_ptr = graph_cmx_svg_cmd_cb;
        svg_get_data->client_data  = (void *) cb_entry;
      } else {
        MSG_ERROR("Ran out of callback buffers!!!",0,0,0);
            return;
      }
    }
    graph_data = (void *) (svg_get_data);
    break;

  case SVG_CMX_CONTINUE:
    svg_continue = (svg_cmx_continue_type *) data;

    if (svg_continue->callback_ptr != NULL) {
      if ((packet = graph_cmd_get_pkt()) != NULL) {
        packet->hdr.cmd            = GRAPH_SVG_CMD_CB;
        packet->svg_cb.cb_func     = svg_continue->callback_ptr;
        packet->svg_cb.client_data = svg_continue->client_data;
        graph_cmd(packet);
      } else {
        MSG_ERROR("Ran out of packets!!!",0,0,0);
      }
    }
    return;

  case SVG_CMX_EXT_REQ:
    svg_request = *((svg_cmx_request_type *) data);

    if (svg_request.callback_ptr != NULL) {
      svg_request.callback_ptr  = graph_cmx_svg_request_cb;
    }
    graph_data = (void *) (&svg_request);
    break;

  case SVG_CMX_EXT_DATA:
    svg_req_data = *((svg_cmx_req_data_type *) data);

    if (svg_req_data.callback_ptr != NULL) {
      svg_req_data.callback_ptr  = graph_cmx_svg_request_cb;
    }
    graph_data = (void *) (&svg_req_data);
    break;

  case SVG_CMX_GET_FILE_SIZE:
  case SVG_CMX_INFO:
  case SVG_CMX_PREPARE_DONE:
  case SVG_CMX_EXT_FREE:
  case SVG_CMX_FRAME_DONE:
  case SVG_CMX_EVENT_DONE:
  case SVG_CMX_GET_TIME:
  case SVG_CMX_ABORT:
  case SVG_CMX_FAIL:
  case SVG_CMX_PAUSE:
  case SVG_CMX_RESUME:
  case SVG_CMX_REWIND:
  case SVG_CMX_FFORWARD:
    graph_data = data;
    break;

  default:
    MSG_LOW("Unknown status",0,0,0);
    graph_data = data;
    break;
  }

  if(  graph_cmx_svg_cb != NULL){
    graph_cmx_svg_cb( status, handle, graph_data);
  }
}

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
void graph_svg_decode_processing( graph_packet_type* packet){
  svg_load_data_type    svg_load_data;

  if( packet){
    graph_cmx_svg_cb               = packet->svg_decode_cmd.cb_func;
    svg_load_data.cb_func          = graph_svg_callback;
    svg_load_data.cb_data          = packet->svg_decode_cmd.cb_data;
    svg_load_data.dpi              = packet->svg_decode_cmd.dpi;
    svg_load_data.mem_buffer       = packet->svg_decode_cmd.mem_buffer;
    svg_load_data.mem_buffer_size  = packet->svg_decode_cmd.mem_buffer_size;
    svg_load_data.svg_frame_width  = packet->svg_decode_cmd.width;
    svg_load_data.svg_frame_height = packet->svg_decode_cmd.height;
    svg_load_data.transparency     = packet->svg_decode_cmd.transparency;
    svg_load_data.bk_color         = packet->svg_decode_cmd.bk_color;
    svg_load_data.max_requests     = packet->svg_decode_cmd.max_requests;
    svg_load_data.filename         = packet->svg_decode_cmd.filename;
    svg_load_data.progressive      = packet->svg_decode_cmd.progressive;

    (void) svg_load_animation( &svg_load_data);
  }
}
#endif /* FEATURE_SVG */

#ifdef FEATURE_QVPHONE
extern boolean graph_qvp_toggle_venc_record_flag()
{

  graph_packet_type *g_cmd;
  g_cmd = graph_cmd_get_pkt();
  if (g_cmd)
  {
    g_cmd->hdr.cmd = GRAPH_QVPHONE_ENC_TOGGLE_RECORD;
    graph_cmd(g_cmd);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif /*FEATURE_QVPHONE*/
#endif /* FEATURE_GRAPH_TASK */
