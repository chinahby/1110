/* =======================================================================

                          qtv_vrend.cpp

DESCRIPTION
  Implementation of the QTV video renderer.

  OVERVIEW DEFINITION OF STATES AND EVENTS
    See qtv_vrend.vsd for statechart.

    States are:
      initial
        The initial transition pseudostate.  See qtvhsm.h for a discussion
        on initial pseudostates.

      main
        A catch-all top-level state.  The renderer is never directly in this
        state, but many transitions are inherited from this state.

      disabled
        Exactly how it sounds  When disabled, the renderer will not attempt
        to draw frames automatically.  Single-stepping is only legal in
        this state.

      enabled
        The renderer is active and drawing frames.  Like main, the renderer
        is never directly in this state.

      idle
        A substate of enabled - the renderer is idle when enabled, but
        frame preroll has not been met.

      active
        In this state, the renderer is drawing frames.

  DISCUSSION OF PRE-ROLL
    Preroll is the number of frames which must be on the input queue before
    the renderer will transition to 'active'.  This is important for video
    streams in which the display order of frames does not match the decode
    order, because frames can be completed out of order.  Adding a preroll
    to the renderer removes this problem by allowing enough frames to queue
    that it's guaranteed that the frame at the head of the input queue
    is the correct one to display next.

    For decoders without these ordering problems, the default preroll of 1
    is just fine.

    For decoders with B frames, preroll should be no less than 2.

  HOW FRAMES MOVE THROUGH DATA STRUCTURES AS THEY GET RECEIVED AND
  PASSED TO THE UI, FREE'D, ETC.
    Frames are sent to the renderer via the render() function.  This function
    queues them for dispatch by the renderer task, which delivers them to the
    state machine as a parameterized EVENT_FRAME event.  The state machine
    stores these frames in sorted order on m_frame_q_ until they are rendered.
    It is the obligation of the application to make sure that frame pointers
    are not released until rendering is complete: that is, frame buffers
    must not be reused between the render() call to the renderer and the
    ReuseBuffer() call from the application.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/qtv_vrend.cpp#24 $
$DateTime: 2010/08/17 06:21:07 $
$Change: 1404651 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_vrend.h"
#include "Video.h"
#include "videoDec.h"
#include "assert.h"
#include "qtv_perf.h"
extern "C"
{
  #include "task.h"
}

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/* ======================================================================
FUNCTION:
  qtv_vrend_class

DESCRIPTION:
  Constructor for the video renderer.

PARAMETERS:
  VideoDec &videoDec,
    The videoDec with which we are to be associated.  We need the
    videoDec for the following functions:
      CheckFrameTime
      DoGenericVdecReuseFrame
      IncrementFrameCount
      IsItOkayToDropThisFrame
      RenderVideoFrame
      UpdateStats
      UpdateVideoDimensions

  const unsigned preroll
    Our preroll - we'll not render unless at least this many frames
    are in our input queue.  This is of interest to decoders which handle
    B-frames and others for which the decode order does not match the
    display order.

RETURN VALUE:
  None.
===========================================================================*/
qtv_vrend_class::qtv_vrend_class
(
   VideoDec       &videoDec,
   const unsigned preroll
)
:  hsm_class( ( qtvhsm::state_class::state_fn_type )&qtv_vrend_class::initial_state_ ),
   m_vrend_task_( &qtv_video_renderer_tcb, qtv_task_video_renderer_info ),
   m_videoDec_( videoDec ),
   m_preroll_( preroll ? preroll : 1 ),
   m_timer_dispatch_id_( 0 ),
   m_sequential_drops_( 0 ),
   m_transmitter_ptr_( 0 ),
   m_queueing_mode_( VideoPlayer::RENDERER_QUEUING_MODE_SORTING ),
   m_max_timestamp_( MAX_UINT32 )
{
   reset_last_frame_rendered_();
   ( void )q_init( &m_frame_q_ );
   QCUtils::InitCritSect( &m_frame_q_cs); 
   init();
}

/* ======================================================================
FUNCTION:
  ~qtv_vrend_class

DESCRIPTION:
  Destructor for the video renderer.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_vrend_class::~qtv_vrend_class( void )
{
  if ( m_timer_dispatch_id_ )
  {
    m_vrend_task_.cancel_dispatch( m_timer_dispatch_id_ );
  }

  flush_(); /*lint !e1551 */
  m_transmitter_ptr_ = 0; /* to quiet lint */

  QCUtils::DinitCritSect(&m_frame_q_cs);
}

/* ======================================================================
FUNCTION:
  enable

DESCRIPTION:
  Activates the video renderer - when enabled, the renderer will send frames
  to the application for rendering.  This should be called when the
  media application enters a playing state.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::enable( void )
{
  /* All the public API functions use the same algorithm as you see here:
  ** - allocate a task_if-level message buffer.
  ** - construct a qtvhsm::event_class in that buffer.  You might expect
  **   to see a placement new in this step, but we can't use that here
  **   due to the existing override of global new.
  ** - now that the event is constructed, it is dispatched to our
  **   task for processing.  The task will extract the event and dispatch
  **   it to the state machine. */

  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(frame_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(frame_event_class) ); 

  qtvhsm::event_class* event_ptr = ( qtvhsm::event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->kind = EVENT_ENABLE;
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  disable

DESCRIPTION:
  Deactivates the video renderer so it will not send frames to be displayed.
  This should be called when the media player leaves a playing state.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::disable( void )
{
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(frame_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(frame_event_class) ); 

  qtvhsm::event_class* event_ptr = ( qtvhsm::event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->kind = EVENT_DISABLE;
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  notify_clipInfo_Dimensions

DESCRIPTION:
  This is a sync funciton which updates the height & width of clipinfo and notify to UI for clipinfo validation.

PARAMETERS:
  int height : height of enoded frame.
  int width  : width of encoded frame.

RETURN VALUE:
    None.
=========================================================================*/
void qtv_vrend_class::notify_clipInfo_Dimensions( int width, int height )
{
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(frame_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(clipinfo_event_class) ); 

  clipinfo_event_class* event_ptr = ( clipinfo_event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->init( width, height );
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  render

DESCRIPTION:
  Submits a frame for rendering.  The frame will be queued and rendered
  at a later time.  The media player can learn when the frame is rendered
  by watching for broadcasts from the renderer or by watching for the
  ReleaseBuffer call from the application.

PARAMETERS:
  VDEC_FRAME* const frame_ptr
    The frame to be rendered.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::render( VDEC_FRAME* const frame_ptr )
{
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();
  
  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(frame_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(frame_event_class) ); 

  frame_event_class* event_ptr = ( frame_event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->init( EVENT_FRAME, frame_ptr );
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  single_step

DESCRIPTION:
  Performs a single-step: that is, renders the next frame in the frame queue
  immediately, regardless of timing.  This function blocks the calling task
  until the outcome of the step is known.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    If true, a frame was successfully rendered.  false otherwise.
===========================================================================*/
bool qtv_vrend_class::single_step( void )
{
  /* The blocking API functions add one more twist - they block
  ** the calling task after the message is sent and harvest the
  ** result once the renderer task waskes them back up. */

  bool result           = false;
  bool init_is_complete = false;

  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(single_step_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(single_step_event_class) ); 

  single_step_event_class* event_ptr = ( single_step_event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->init( EVENT_SINGLE_STEP, &init_is_complete, &result );
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
    WaitForFlag( init_is_complete, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC, 5 );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }

  return result;
}

/* ======================================================================
FUNCTION:
  flush

DESCRIPTION:
  flushes all video renderer queues.  Blocks the calling thread until
  the flush is complete.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::flush( void )
{
  bool flush_is_complete = false;

  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(simple_sync_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(simple_sync_event_class) ); 

  simple_sync_event_class* event_ptr =
    ( simple_sync_event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->init( EVENT_FLUSH, &flush_is_complete );
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
    WaitForFlag( flush_is_complete, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC, 5 );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  set_preroll

DESCRIPTION:
  Sets the renderer preroll.  Preroll is the number of frames which must
  be available on the input queue before the renderer will draw them.
  This is of import to decoders whose decode order does not match display
  order.

PARAMETERS:
  const unsigned preroll
    The new preroll.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::set_preroll( const unsigned preroll )
{
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(set_preroll_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(set_preroll_event_class) ); 

  set_preroll_event_class* event_ptr = ( set_preroll_event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->init( EVENT_SET_PREROLL, preroll ? preroll : 1 );
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

/* ======================================================================
FUNCTION:
  avsync

DESCRIPTION:
  Causes the renderer to re-sync with current avsync information.  This
  function should be called when the video player is in a playing state
  and timing information changes.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::avsync( void )
{
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(qtvhsm::event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(qtvhsm::event_class) ); 

  qtvhsm::event_class* event_ptr = ( qtvhsm::event_class* )msg_ptr; /*lint !e740 */

  if ( msg_ptr )
  {
    event_ptr->kind = EVENT_AVSYNC;
    ( void )m_vrend_task_.dispatch( this, msg_ptr );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
  }
}

/* ======================================================================
FUNCTION:
  set_queueing_mode

DESCRIPTION:
  Sets the queueing mode of the renderer.  Queueing modes are:

    SORTING: this is the default mode.  If frames are received out
    of display order, the renderer sorts them into correct order
    before display.

    FIFO: Out-of-display-order frames are dropped upon receipt.

PARAMETERS:
  queueing_mode: the new queueing mode.

RETURN VALUE:
  none.
=========================================================================*/
void qtv_vrend_class::set_queueing_mode( VideoPlayer::RendererQueuingMode m )
{
   qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(set_queueing_mode_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(set_queueing_mode_event_class) ); 

   set_queueing_mode_event_class* event_ptr =
      ( set_queueing_mode_event_class* )msg_ptr; /*lint !e740 */

   if ( msg_ptr )
   {
      event_ptr->init( m );
      ( void )m_vrend_task_.dispatch( this, msg_ptr );
   }
   else
   {
      QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
   }
}

/* ======================================================================
FUNCTION:
  set_max_timestamp

DESCRIPTION:
  Informs the renderer that timestamps have a rollover point other than
  the native uint32 boundary defined by the decoder.  Changing the
  max-timestamp value changes rollover detection and frame sorting.
  It does NOT change frame rendering times, as those are controlled
  by VideoDec::CheckFrameTime().

PARAMETERS:
  uint32: the maximum valid timestamp.  The renderer default
  is UINT32_MAX, 0xffffffff.

RETURN VALUE:
  none.
=========================================================================*/
void qtv_vrend_class::set_max_timestamp( uint32 max_ts )
{
   qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(set_max_timestamp_event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(set_max_timestamp_event_class) ); 

   set_max_timestamp_event_class* event_ptr =
      ( set_max_timestamp_event_class* )msg_ptr; /*lint !e740 */

   if ( msg_ptr )
   {
      event_ptr->init( max_ts );
      ( void )m_vrend_task_.dispatch( this, msg_ptr );
   }
   else
   {
      QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
   }
}

/* ======================================================================
FUNCTION:
  release_frame

DESCRIPTION:
  Informs the renderer that the UI is done rendering a previously submitted
  frame and that the renderer can release that frame back to the decoder
  for reuse.

PARAMETERS:
 QtvPlayer::FrameInfoT &frame
   The frame to be rendered.

RETURN VALUE:
  none.
=========================================================================*/
void qtv_vrend_class::release_frame( const QtvPlayer::FrameInfoT &frame )
{
   qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();
   release_frame_event_class* event_ptr =  ( release_frame_event_class* )msg_ptr; /*lint !e740 */

   if ( msg_ptr )
   {
      event_ptr->init((QtvPlayer::FrameInfoT *)&frame);
      ( void )m_vrend_task_.dispatch( this, msg_ptr );
   }
   else
   {
      QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" );
   }
}

/* ======================================================================
FUNCTION:
  dispatch

DESCRIPTION:
  Dispatches a message to the renderer task.  Not for public use.

PARAMETERS:
  qtv_msg_struct* const msg_ptr
    The message being dispatched.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::dispatch( qtv_msg_struct* const msg_ptr )
{
  static const char* event_names[] =
  {
    "EVENT_ENABLE",
    "EVENT_DISABLE",
    "EVENT_FRAME",
    "EVENT_SINGLE_STEP",
    "EVENT_FLUSH",
    "EVENT_SET_PREROLL",
    "EVENT_RENDER",
    "EVENT_DROP",
    "EVENT_AVSYNC",
    "EVENT_PCR_RESET",
    "EVENT_SET_QUEUEING_MODE",
    "EVENT_SET_MAX_TIMESTAMP",
    "EVENT_RELEASE_FRAME",
	"EVENT_CLIPINFO_DIMENSION"
  };

  /* In all cases, the message payload contains an event
  ** for submission to the state machine. */

  qtvhsm::event_class* event_ptr = ( qtvhsm::event_class* )msg_ptr; /*lint !e740 */

  QTV_MSG_SPRINTF_PRIO_2( QTVDIAG_VIDEO_TASK,
                          QTVDIAG_PRIO_LOW,
                          "renderer %x processing %s",
                          this,
                          event_names[ event_ptr->kind - qtvhsm::event_class::EVENT_USER ] ); /*lint !e641 */

  qtvhsm::hsm_class::dispatch( event_ptr );

  /* It is the sink's duty to delete the message.
  */
  qtv_msg_struct::free( msg_ptr );
}

/* ======================================================================
FUNCTION:
  initial_state_

DESCRIPTION:
  Implementation of the state machine's initial transition.

PARAMETERS:
  const qtvhsm::event_class* const
    An event containing any parameter information needed for state
    machine initialization.  In our case, ignored.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event ( we better in this case ) and
    a pointer to the parent state otherwise.
===========================================================================*/
qtvhsm::state_class qtv_vrend_class::initial_state_
(
  const qtvhsm::event_class* const
)
{
  HSM_INIT( qtv_vrend_class::main_state_ );
  return 0;
}

/* ======================================================================
FUNCTION:
  main_state_

DESCRIPTION:
  Implementation of the state machine's 'main' state.

PARAMETERS:
  const qtvhsm::event_class* const
    The incoming event.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event and  a pointer to the parent state otherwise.
===========================================================================*/
HSM_DEFINE_STATE( qtv_vrend_class::main_state_ )
{
   qtvhsm::state_class::state_fn_type result( 0 );
   QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
    "qtv_vrend_class::main_state_, event_ptr->kind=%d, m_frame_q_.cnt=%d",
    event_ptr->kind,m_frame_q_.cnt);
   switch ( event_ptr->kind )
   {
      case qtvhsm::event_class::EVENT_ENTRY:
      {
         QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                        "renderer %x entering state MAIN", this );
         break;
      }
      case qtvhsm::event_class::EVENT_INIT:
      {
         HSM_INIT( qtv_vrend_class::disabled_state_ );
         break;
      }
      case EVENT_FRAME:
      {
         const frame_event_class* re_ptr = ( frame_event_class* )event_ptr; /*lint !e1773 */

         queue_frame_( re_ptr->frame_ptr );
         break;
      }
      case EVENT_SINGLE_STEP:
      {
         single_step_event_class* sse_ptr = ( single_step_event_class* )event_ptr; /*lint !e1773 */

         *( sse_ptr->result_ptr ) = false;
         *( sse_ptr->sync_ptr )   = true;
         break;
      }
      case EVENT_FLUSH:
      {
         simple_sync_event_class* fe_ptr =
         ( simple_sync_event_class* )event_ptr; /*lint !e1773 */

         flush_();
         *( fe_ptr->sync_ptr ) = true;
         break;
      }
      case EVENT_SET_PREROLL:
      {
         set_preroll_( event_ptr );
         break;
      }
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
      case EVENT_SET_QUEUEING_MODE:
      {
         set_queueing_mode_event_class* pe =
            ( set_queueing_mode_event_class* )event_ptr; /*lint !e1773 */
         m_queueing_mode_ = pe->mode;
         break;
      }
      case EVENT_SET_MAX_TIMESTAMP:
      {
         set_max_timestamp_event_class* pe =
            ( set_max_timestamp_event_class* )event_ptr; /*lint !e1773 */
         m_max_timestamp_ = pe->max_ts;
         /* If the timestamp max is being set, that implies a change in
         ** the underlying media.  Frames in the queue should be considered
         ** invalid after such an event. */
         /* flush should not be done while changing timestamp maximum
         ** flush_();
         */
         break;
      }
      case EVENT_RELEASE_FRAME:
      {
         release_frame_event_class* pe( ( release_frame_event_class* )event_ptr ); /*lint !e1773 */         
         m_videoDec_.ReleaseBuffer( *(pe->pframe) );
         break;
      }
      case EVENT_CLIPINFO_DIMENSION:
      {
        clipinfo_event_class* pe = (clipinfo_event_class *)event_ptr ;
        m_videoDec_.UpdateClipInfoDimensions(pe->width,pe->height);
        break;
      }
      default:
      {
         result = &qtv_vrend_class::top;
      }
   } /*lint !e744 */

   return result;
}

/* ======================================================================
FUNCTION:
  disabled_state_

DESCRIPTION:
  Implementation of the state machine's 'disabled' state.

PARAMETERS:
  const qtvhsm::event_class* const
    The incoming event.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event and  a pointer to the parent state otherwise.
===========================================================================*/
HSM_DEFINE_STATE( qtv_vrend_class::disabled_state_ )
{
  QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
   "qtv_vrend_class::disabled_state_, event_ptr->kind=%d, m_frame_q_.cnt=%d",
   event_ptr->kind,m_frame_q_.cnt);
  switch( event_ptr->kind )
  {
    case qtvhsm::event_class::EVENT_ENTRY:
    {
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                     "renderer %x entering state DISABLED", this );
      return 0;
    }
    case EVENT_ENABLE:
    {
      HSM_TRANSITION( qtv_vrend_class::enabled_state_ );
      return 0;
    }
    case EVENT_SINGLE_STEP:
    {
      single_step_event_class* sse_ptr = ( single_step_event_class* )event_ptr; /*lint !e1773 */

      if ( preroll_is_met_() )
      {
        *( sse_ptr->result_ptr ) = render_frame_();
      }
      else
      {
        *( sse_ptr->result_ptr ) = false;
      }
      *( sse_ptr->sync_ptr ) = true;
      return 0;
    }
  } /*lint !e744 */

  HSM_RETURN( qtv_vrend_class::main_state_ );
}

/* ======================================================================
FUNCTION:
  enabled_state_

DESCRIPTION:
  Implementation of the state machine's 'enabled' state.

PARAMETERS:
  const qtvhsm::event_class* const
    The incoming event.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event and  a pointer to the parent state otherwise.
===========================================================================*/
HSM_DEFINE_STATE( qtv_vrend_class::enabled_state_ )
{
  QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
   "qtv_vrend_class::enabled_state_, event_ptr->kind=%d, m_frame_q_.cnt=%d", 
   event_ptr->kind,m_frame_q_.cnt);
  switch( event_ptr->kind )
  {
    case qtvhsm::event_class::EVENT_ENTRY:
    {
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                     "renderer %x entering state ENABLED", this );
      return 0;
    }
    case qtvhsm::event_class::EVENT_INIT:
    {
      if ( preroll_is_met_() )
      {
        HSM_INIT( qtv_vrend_class::active_state_ );
      }
      else
      {
        HSM_INIT( qtv_vrend_class::idle_state_ );
      }
      return 0;
    }
    case EVENT_DISABLE:
    {
      HSM_TRANSITION( qtv_vrend_class::disabled_state_ );
      return 0;
    }
  } /*lint !e744 */
  HSM_RETURN( qtv_vrend_class::main_state_ );
}

/* ======================================================================
FUNCTION:
  idle_state_

DESCRIPTION:
  Implementation of the state machine's 'idle' state.

PARAMETERS:
  const qtvhsm::event_class* const
    The incoming event.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event and  a pointer to the parent state otherwise.
===========================================================================*/
HSM_DEFINE_STATE( qtv_vrend_class::idle_state_ )
{
  QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
   "qtv_vrend_class::idle_state_, event_ptr->kind=%d,m_frame_q_.cnt=%d",
   event_ptr->kind,m_frame_q_.cnt);
  switch( event_ptr->kind )
  {
    case qtvhsm::event_class::EVENT_ENTRY:
    {
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                     "renderer %x entering state IDLE", this );
      return 0;
    }
    case EVENT_FRAME:
    {
      frame_event_class* re_ptr = ( frame_event_class* )event_ptr; /*lint !e1773 */

      queue_frame_( re_ptr->frame_ptr );
      if ( preroll_is_met_() )
      {
        HSM_TRANSITION( qtv_vrend_class::active_state_ );
      }
      return 0;
    }
    case EVENT_SET_PREROLL:
    {
      set_preroll_( event_ptr );
      if ( preroll_is_met_() )
      {
        HSM_TRANSITION( qtv_vrend_class::active_state_ );
      }
      return 0;
    }
  } /*lint !e744 */
  HSM_RETURN( qtv_vrend_class::enabled_state_ );
}

/* ======================================================================
FUNCTION:
  active_state_

DESCRIPTION:
  Implementation of the state machine's 'active' state.

PARAMETERS:
  const qtvhsm::event_class* const
    The incoming event.

RETURN VALUE:
  qtvhsm::hsm_class::pseudostate_type
    0 if we handled the event and  a pointer to the parent state otherwise.
===========================================================================*/
HSM_DEFINE_STATE( qtv_vrend_class::active_state_ )
{
  QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
   "qtv_vrend_class::active_state_, event_ptr->kind=%d,m_frame_q_.cnt=%d", 
   event_ptr->kind,m_frame_q_.cnt);
  switch( event_ptr->kind )
  {
    case qtvhsm::event_class::EVENT_ENTRY:
    {
      active_entry_();
      return 0;
    }
    case qtvhsm::event_class::EVENT_EXIT:
    {
      if ( m_timer_dispatch_id_ )
      {
        ( void )m_vrend_task_.cancel_dispatch( m_timer_dispatch_id_ );
      }
      m_timer_dispatch_id_ = 0;
      return 0;
    }
    case EVENT_FRAME:
    {
      frame_event_class* re_ptr = ( frame_event_class* )event_ptr; /*lint !e1773 */

      queue_frame_( re_ptr->frame_ptr );
      HSM_TRANSITION( qtv_vrend_class::active_state_ );
      return 0;
    }
    case EVENT_FLUSH:
    {
      simple_sync_event_class* fe_ptr =
        ( simple_sync_event_class* )event_ptr; /*lint !e1773 */

      flush_();
      *( fe_ptr->sync_ptr ) = true;
      HSM_TRANSITION( qtv_vrend_class::idle_state_ );

      return 0;
    }
    case EVENT_SET_PREROLL:
    {
      set_preroll_( event_ptr );
      if ( !preroll_is_met_() )
      {
        HSM_TRANSITION( qtv_vrend_class::idle_state_ );
      }
      return 0;
    }
    case EVENT_RENDER:
    {
      m_timer_dispatch_id_ = 0;
      ( void )render_frame_();
      HSM_TRANSITION( qtv_vrend_class::enabled_state_ );
      return 0;
    }
    case EVENT_DROP:
    {
      drop_frame_( QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_AVSYNC );
      HSM_TRANSITION( qtv_vrend_class::enabled_state_ );
      return 0;
    }
    case EVENT_AVSYNC:
    {
      HSM_TRANSITION( qtv_vrend_class::active_state_ );
      return 0;
    }
  } /*lint !e744 */
  HSM_RETURN( qtv_vrend_class::enabled_state_ );
}

/* ======================================================================
FUNCTION:
  active_entry_

DESCRIPTION:
  Handles the entry event for the active state, as it's a bit complicated
  and I don't want it cluttering up the state handler.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::active_entry_( void )
{
  unsigned long timestamp;
  bool frame_is_late;
  long sleep_ms;
  qtvhsm::state_class this_state( ( qtvhsm::state_class::state_fn_type )&qtv_vrend_class::active_state_ );
  /* Because every frame causes a self-transition and this can be
  ** confusing (and noisy) on the QXDM, we only report the entry
  ** if the source was not us.
  */
  if ( this_state.operator!=( get_state() ) )
  {
    QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                   "renderer %x entering state ACTIVE", this );
  }

  /* ASSERT( preroll_is_met_() );
  */
  ASSERT( m_timer_dispatch_id_ == 0 );

  /* It is possible to hit preroll issues when flush is issued 
  ** while the renderer is waiting for the delayed render dispatch
  ** to occur in active_state. 
  */ 
  if(!preroll_is_met_())
  {
    QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                   "renderer %x Preroll not met in state ACTIVE", 
                   this );
                   
    /* Handle the exception through an automatic transition to Idle state
    */
    HSM_INIT( qtv_vrend_class::idle_state_ );  
  }
  else
  {
    get_frame_timing_( frame_is_late, sleep_ms, &timestamp );
    if ( frame_is_late )
    {
      if ( m_sequential_drops_ <= SEQUENTIAL_FRAME_DROP_THRESHOLD )
      {
        send_event_to_self_( EVENT_DROP, 0 );
      }
      else
      {
        send_event_to_self_( EVENT_RENDER, 0 );
      }
    }
    else if(m_videoDec_.IsBroadcastLeadCheckFailed(sleep_ms))
    {
      /* Broadcast Lead Check failed ; Act on it by dropping the frame.
      */
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                   "renderer drop frame %d due to broadcast lead constraint", 
                   timestamp );
      send_event_to_self_( EVENT_DROP, 0 );
    }
    else
    {
      if ( sleep_ms < 0 )
      {
        sleep_ms = 0; /* Small negatives possible due to avsync window */
      }
      else if ( sleep_ms > 0 )
      {
        QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK,
                     QTVDIAG_PRIO_MED,
                     "renderer %x waiting %d ms before rendering frame %d",
                     this,
                     sleep_ms,
                     timestamp );
      }

      m_timer_dispatch_id_ =
        send_event_to_self_( EVENT_RENDER, ( unsigned long )sleep_ms );
    }
  }
  return; 
}

/* ======================================================================
FUNCTION:
  queue_frame_

DESCRIPTION:
  Adds a new frame to the sorted input queue.

PARAMETERS:
  VDEC_FRAME* const frame_ptr
    The frame being added.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::queue_frame_( VDEC_FRAME* const frame_ptr )
{
   ASSERT( frame_ptr );

   QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK,
                  QTVDIAG_PRIO_LOW,
                  "renderer %x queueing frame %d",
                  this,
                  frame_ptr->timestamp );
  CS_Locker lock( m_frame_q_cs );    

   /* Frames with timestamps beyond the set rollover point are unacceptable. */
   if ( ( frame_ptr->timestamp & invalid_timestamp_() ) == 0 &&
        ( frame_ptr->timestamp <= m_max_timestamp_ ) )
   {
      /* if this frame is to be queued, we must find its proper sorted place
      ** in the frame queue.  If we're in FIFO mode and that place is not
      ** at the end of the queue, we'll reject the frame. */
      frame_link_struct* after_ptr( ( frame_link_struct* )q_check( &m_frame_q_ ) );
      while ( after_ptr &&
              compare_timestamps_( frame_ptr->timestamp,
                                   after_ptr->frame_ptr->timestamp) == LEFT_IS_LARGER )
      {
         after_ptr = ( frame_link_struct* )q_next( &m_frame_q_, &after_ptr->link );
      }

      /* At this point, after_ptr is in one of two states:
      ** - it is 0, which means the new timestamp exceeds all those
      **   in the frame queue.
      ** - it is non-zero, pointing to the first frame in the queue whose timestamp
      **   matches or exceeds the new frame's.
      ** The side effect of such insertion on timestamp collisions is the
      ** eventual rejection of all but the last-submitted frame, which is
      ** exactly what we want - if two frames claim the same timestamp, the last
      ** one is to be trusted.
      ** If FIFO mode is enabled, the frame must also be rejected in all non-0
      ** after_ptr cases. */
      if ( after_ptr == 0 )
      {
         append_to_frame_q_( frame_ptr );
      }
      else
      {
         if ( frame_ptr->timestamp == after_ptr->frame_ptr->timestamp )
         {
            replace_frame_( frame_ptr, after_ptr );
         }
         else
         {
            insert_in_frame_q_( frame_ptr, after_ptr );
         }
      }

      /* After all changes to the frame queue, videoDec must be notified
      ** of the dimensions of the next frame to be rendered. */
      frame_link_struct* link_ptr( ( frame_link_struct* )q_check( &m_frame_q_ ) );
      if ( link_ptr )
      {
         m_videoDec_.UpdateVideoDimensions( *link_ptr->frame_ptr );
      }
   }
   else // timestamp is bogus
   {
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                     "renderer dropping frame due to invalid timestamp: %d",
                     frame_ptr->timestamp );

      drop_frame_( frame_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_FRAME_ERROR );
   }
}

/* ======================================================================
FUNCTION:
  append_to_frame_q_

DESCRIPTION:
  Places a frame at the end of the frame queue.

PARAMETERS:
  VDEC_FRAME* const frame_ptr
    The new frame.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::append_to_frame_q_( VDEC_FRAME* const frame_ptr )
{
   frame_link_struct* link_ptr( QTV_New( frame_link_struct ) );

   CS_Locker lock( m_frame_q_cs );      

   if ( link_ptr )
   {
     ( void )q_link( link_ptr, &( link_ptr->link ) );
     link_ptr->frame_ptr = frame_ptr;
     q_put( &m_frame_q_, &( link_ptr->link ) );
   }
   else // QTV_New failed
   {
      QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                    "append_to_frame_q_ out of memory" );
      drop_frame_( frame_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_RENDERING_FAILURE );
   }
}

/* ======================================================================
FUNCTION:
  replace_frame_

DESCRIPTION:
  Replaces a frame in the frame queue with a different one and releases
  the replaced frame.

PARAMETERS:
  VDEC_FRAME* const frame_ptr
  frame_link_struct* const after_ptr

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::replace_frame_( VDEC_FRAME* const frame_ptr,
                                      frame_link_struct* const after_ptr )
{
   QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                  "renderer dropping frame due to timestamp conflict: %d",
                  after_ptr->frame_ptr->timestamp );

   drop_frame_( after_ptr->frame_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_FRAME_ERROR );
   after_ptr->frame_ptr = frame_ptr;
}

/* ======================================================================
FUNCTION:
  insert_in_frame_q_

DESCRIPTION:
  Adds a frame to the frame queue in a position other than the tail.

PARAMETERS:
  VDEC_FRAME* const frame_ptr
    The frame being added to the queue.
  frame_link_struct* const after_ptr
    The link struct which will end up after the new insertion.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::insert_in_frame_q_( VDEC_FRAME* const frame_ptr,
                                          frame_link_struct* const after_ptr )
{
   CS_Locker lock( m_frame_q_cs );  
   if ( m_queueing_mode_ == VideoPlayer::RENDERER_QUEUING_MODE_SORTING )
   {
      frame_link_struct* link_ptr( QTV_New( frame_link_struct ) );
      if ( link_ptr )
      {
        ( void )q_link( link_ptr, &( link_ptr->link ) );
        link_ptr->frame_ptr = frame_ptr;
        q_insert( &m_frame_q_, &( link_ptr->link ), &( after_ptr->link ) );
      }
      else // QTV_New failed
      {
         QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL,
                       "append_to_frame_q_ out of memory" );
         drop_frame_( frame_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_RENDERING_FAILURE );
      }
   }
   else if ( m_queueing_mode_ == VideoPlayer::RENDERER_QUEUING_MODE_FIFO )
   {
      QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,
                     "renderer is in FIFO mode.  Rejecting out-of-order frame %d",
                     frame_ptr->timestamp );
      drop_frame_( frame_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_RENDERER_MODE );
   }
   else
   {
      ASSERT( 0 ); // A mode was added but adding the handler here was forgotten
   }
}

/* ======================================================================
FUNCTION:
  render_frame_

DESCRIPTION:
  Renders the video frame on the head of the queue.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    true on success, false otherwise.
===========================================================================*/
bool qtv_vrend_class::render_frame_( void )
{
  bool result = false;
  frame_link_struct* link_ptr;
  bool is_abort = false;
  bool free_frame = false;
  VDEC_FRAME vdecFrame;

  CS_Locker lock( m_frame_q_cs );   
  link_ptr = ( frame_link_struct* )q_get( &m_frame_q_ );
  if ( link_ptr )
  {
    ASSERT( link_ptr->frame_ptr );

    m_sequential_drops_ = 0;

    QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED,
                   "renderer %x rendering frame <%d> ts <%d>",
                    this, m_videoDec_.GetFrameCount(),
                    link_ptr->frame_ptr->timestamp );

    link_ptr->frame_ptr->frameStatus = FRAME_WITH_UI;

   /* Copy the frame buffer to local variable before sending to display and release.
   ** As there was a timing issue and accessing the invalid frame buffer in different context. */
    memcpy(&vdecFrame,link_ptr->frame_ptr,sizeof(VDEC_FRAME));

    result = m_videoDec_.RenderVideoFrame( is_abort,
                                           *link_ptr->frame_ptr
                                           #ifdef FEATURE_QTV_MDP
                                           , free_frame
                                           #endif /* FEATURE_QTV_MDP */
                                           );

    if ( !result )
    {
      drop_frame_( link_ptr, QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_RENDERING_FAILURE );
    }
    else
    {
      broadcast_( QTV_Broadcast::VIDEO_FRAME_RENDERED,
                  QTV_VideoRendererBroadcast::SUBTYPE_UNUSED,
                  vdecFrame );

      if ( free_frame )
      {
        m_videoDec_.DoGenericVdecReuseFrame( VideoDec::FRAME_RENDERED,
                                             link_ptr->frame_ptr );
      }
    }
    QTV_Delete( link_ptr );
  }
  return result;
}

/* ======================================================================
FUNCTION:
  drop_frame_

DESCRIPTION:
  Drops the frame at the head of the input queue.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::drop_frame_
(
  const QTV_VideoRendererBroadcast::Subtype reason
)
{
  frame_link_struct* link_ptr;
  CS_Locker lock( m_frame_q_cs );   

  link_ptr = ( frame_link_struct* )q_get( &m_frame_q_ );
  if ( link_ptr )
  {
    drop_frame_( link_ptr, reason );
    QTV_Delete( link_ptr );
  }
}

/* ======================================================================
FUNCTION:
  drop_frame_

DESCRIPTION:
  Drops the given frame.

PARAMETERS:
  frame_link_struct* link_ptr
    Link for the frame being dropped.
  const QTV_VideoRendererBroadcast::Subtype reason
    Reason for the drop.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::drop_frame_
(
  frame_link_struct* link_ptr,
  const QTV_VideoRendererBroadcast::Subtype reason
)
{
   ASSERT( link_ptr );
   drop_frame_( link_ptr->frame_ptr, reason, VideoDec::FRAME_DROPPED_AVSYNC );
}

/* ======================================================================
FUNCTION:
  drop_frame_

DESCRIPTION:
  Drops the given frame.

PARAMETERS:
  VDEC_FRAME* frame_ptr
    The frame being dropped.
  const QTV_VideoRendererBroadcast::Subtype broadcast_reason
    Reason for the drop that will be broadcast.
  const VideoDec::FrameReleaseReason        release_reason
    Reason for the drop which will be sent to VideoDec.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::drop_frame_
(
   VDEC_FRAME* const frame_ptr,
   const QTV_VideoRendererBroadcast::Subtype broadcast_reason,
   const VideoDec::FrameReleaseReason        release_reason
)
{
   ASSERT( frame_ptr );

   ++m_sequential_drops_;

   QTV_MSG_PRIO3( QTVDIAG_VIDEO_TASK,
                  QTVDIAG_PRIO_HIGH,
                  "renderer %x dropping frame %d: %d",
                  this,
                  frame_ptr->timestamp,
                  broadcast_reason );
   if( broadcast_reason == QTV_VideoRendererBroadcast::SUBTYPE_DROP_DUE_TO_AVSYNC)
   {
       m_videoDec_.UpdateStats(VideoDec::DROP_AVSYNC);
   }
   broadcast_( QTV_Broadcast::VIDEO_FRAME_DROPPED,
               broadcast_reason,
               *frame_ptr );

   QTV_PERF_FRAME_UPDATE( frame_ptr->timestamp, QTV_PERF_RENDER_DROP );
   QTV_PERF_FRAME_UPDATE( frame_ptr->timestamp, QTV_PERF_RENDER_STOP );

   /* No additional operations on frame_ptr can follow this, as it must
   ** be assumed that the memory is invalid as soon as the call returns. */
   m_videoDec_.DoGenericVdecReuseFrame( release_reason, frame_ptr );
}

/* ======================================================================
FUNCTION:
  flush_

DESCRIPTION:
  Flushes the input queue.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::flush_( void )
{
  frame_link_struct* link_ptr;

  CS_Locker lock( m_frame_q_cs );   
  for ( link_ptr  = ( frame_link_struct* )q_get( &m_frame_q_ );
        link_ptr != 0;
        link_ptr  = ( frame_link_struct* )q_get( &m_frame_q_ ) )
  {
    broadcast_( QTV_Broadcast::VIDEO_FRAME_FLUSHED,
                QTV_VideoRendererBroadcast::SUBTYPE_UNUSED,
                *link_ptr->frame_ptr );

    QTV_MSG_PRIO2( QTVDIAG_VIDEO_TASK,
                   QTVDIAG_PRIO_HIGH,
                   "renderer %x flushing frame %d",
                   this,
                   link_ptr->frame_ptr->timestamp );

    m_videoDec_.DoGenericVdecReuseFrame( VideoDec::FRAME_FLUSHED,
                                         link_ptr->frame_ptr );
    QTV_Delete( link_ptr );
  }

  reset_last_frame_rendered_();
}

/* ======================================================================
FUNCTION:
  set_preroll_

DESCRIPTION:
  Sets the renderer preroll.

PARAMETERS:
  const qtvhsm::event_class* const event_ptr
    The event containing the preroll information.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::set_preroll_( const qtvhsm::event_class* const event_ptr )
{
  set_preroll_event_class* spe_ptr = ( set_preroll_event_class* )event_ptr; /*lint !e1773 */

  m_preroll_ = spe_ptr->preroll;
}

/* ======================================================================
FUNCTION:
  preroll_is_met_

DESCRIPTION:
  Returns true if our preroll is met - that is, if the input queue is
  as long or longer than the preroll.

PARAMETERS:
  None.

RETURN VALUE:
  bool
    true if preroll is met, false otherwise.
===========================================================================*/
bool qtv_vrend_class::preroll_is_met_( void )
{
  CS_Locker lock( m_frame_q_cs );   
  return ( q_cnt( &m_frame_q_ ) >= m_preroll_ ); /*lint !e574 !e737 */
}

/* ======================================================================
FUNCTION:
  get_frame_timing_

DESCRIPTION:
  Gets timing information for the frame at the head of the input queue:
  whether or not it's late, and how far we are, in ms, from its display time.

PARAMETERS:
  bool &frame_is_late
    reference through which the 'lateness' of the frame is returned.
  long &sleep_ms
    reference through which the number of milliseconds until the frame's
    display time is returned.
  unsigned long* timestamp_ptr
    A pointer to a location into which to put the timestamp of the frame.
    Optional.  If this is zero, no timestamp will be written.
RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::get_frame_timing_
(
  bool &frame_is_late,
  long &sleep_ms,
  unsigned long* timestamp_ptr
)
{
  frame_link_struct* link_ptr;
  VDEC_FRAME* frame_ptr = NULL;

  /* Check the frame at the head of the queue, figure out how long
  ** we must wait before rendering it, and set up a delayed dispatch
  ** for the render. */
  CS_Locker lock( m_frame_q_cs );   
  link_ptr = ( frame_link_struct* )q_check( &m_frame_q_ );
  ASSERT( link_ptr );
  if (link_ptr)
  {
    frame_ptr = link_ptr->frame_ptr;
  }
  ASSERT( frame_ptr );

  get_frame_timing_( *frame_ptr, frame_is_late, sleep_ms, timestamp_ptr );
}

/* ======================================================================
FUNCTION:
  get_frame_timing_

DESCRIPTION:
  Gets timing information for the provided frame:
  whether or not it's late, and how far we are, in ms, from its display time.

PARAMETERS:
  VDEC_FRAME &frame
    our friend, the frame
  bool &frame_is_late
    reference through which the 'lateness' of the frame is returned.
  long &sleep_ms
    reference through which the number of milliseconds until the frame's
    display time is returned.
  unsigned long* timestamp_ptr
    A pointer to a location into which to put the timestamp of the frame.
    Optional.  If this is zero, no timestamp will be written.
RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::get_frame_timing_
(
  const VDEC_FRAME& frame,
  bool &frame_is_late,
  long &sleep_ms,
  unsigned long* timestamp_ptr
)
{
  bool frame_is_undroppable;

  sleep_ms = 0;

  if( timestamp_ptr )
  {
    *timestamp_ptr = frame.timestamp;
  }

  frame_is_undroppable =
    !( m_videoDec_.IsItOkayToDropThisFrame( &frame ) );

  frame_is_late =
    m_videoDec_.CheckFrameTime( frame.timestamp,
#ifdef SHOW_LATE
                               VideoDec::CHECK_BEFORE_DISPLAY,
#endif /* SHOW_LATE */
                               ( frame_is_undroppable ? 1 : 0 ),
                               &sleep_ms );
}

/* ======================================================================
FUNCTION:
  compare_timestamps

DESCRIPTION:
  A comparison function used by queue_frame_ to sort frames by timestamp.

PARAMETERS:
  uint64 left
  uint64 right
    The timestamps being compared.

RETURN VALUE:
  A compare_result.  Hopefully the enum is self-explanatory.
===========================================================================*/
qtv_vrend_class::compare_result
qtv_vrend_class::compare_timestamps_( const uint64& left, const uint64& right )
{
   /* Timestamps are technically a 32-bit value with a single bit at position
   ** 32 which indicates if the timestamp is undefined.  However, videoDec
   ** repairs all undefined timestamps before they get here so we should never
   ** see a timestamp with the undefined bit set.  This means we can just ignore
   ** everything in the top half of the timestamp field.
   **
   ** The above implies that we can safely cast timestamps to uint32s without
   ** the loss of important information. */
   const uint64 TIMESTAMP_INVALID_MASK( invalid_timestamp_() );
   ASSERT( (left  & TIMESTAMP_INVALID_MASK) == 0 );
   ASSERT( (right & TIMESTAMP_INVALID_MASK) == 0 );

   compare_result result;
   int64 difference( Difference( left, right, m_max_timestamp_ ) );

   if ( difference > 0 )
   {
      result = RIGHT_IS_LARGER;
   }
   else if ( difference < 0 )
   {
      result = LEFT_IS_LARGER;
   }
   else
   {
      result = EQUAL;
   }

   return result;
}

/* ======================================================================
FUNCTION:
  broadcast_

DESCRIPTION:
  Broadcasts information about a renderer event.

PARAMETERS:
  const QTV_Broadcast::Type                  type
    The event type
  const QTV_VideoRendererBroadcast::Subtype  subtype
    The event subtype
  const VDEC_FRAME                          &frame
    The frame about which we're broadcasting

RETURN VALUE:
  None.
===========================================================================*/
void qtv_vrend_class::broadcast_
(
  const QTV_Broadcast::Type                  type,
  const QTV_VideoRendererBroadcast::Subtype  subtype,
  const VDEC_FRAME                          &frame
)
{
  bool frame_is_late;
  long timeliness = 0;

  QTV_Broadcast* broadcast_ptr;

  if ( !m_transmitter_ptr_ )
  {
    m_transmitter_ptr_ = QTV_BroadcastTransmitter::Locate( "QTV" );
  }

  if ( m_transmitter_ptr_ )
  {
    if ( type != QTV_Broadcast::VIDEO_FRAME_FLUSHED )
    {
      get_frame_timing_( frame, frame_is_late, timeliness, 0 );
    }

    broadcast_ptr =
      QTV_New_Args( QTV_VideoRendererBroadcast, ( type, subtype, frame, timeliness ) );

    if ( broadcast_ptr )
    {
      m_transmitter_ptr_->Transmit( broadcast_ptr );
    }
  }
}

/* ======================================================================
FUNCTION:
  send_event_to_self_

DESCRIPTION:
  Constructs and dispatches an event to ourselves via qtv_task_if.

PARAMETERS:
  const int kind
    The event kind
  const unsigned long delay_ms
    The dispatch delay. 0 means immediate.

RETURN VALUE:
  None.
===========================================================================*/
qtv_task_if_class::dispatch_id_type qtv_vrend_class::send_event_to_self_
(
  const int kind,
  const unsigned long delay_ms
)
{
  qtv_task_if_class::dispatch_id_type result = 0;
  qtv_msg_struct* msg_ptr = qtv_msg_struct::malloc();

  /* here we are type casting "qtv_msg_struct" type object to "frame_event_class" type object
  ** to ensure safe type casting, sizeof(qtv_msg_struct) >= sizeof(qtvhsm::event_class)
  ** To live with this legacy code, a ASSERT is placed. if this ASSERT hits, then we have to 
  ** inrease the size of "qtv_msg_struct"
  */
  ASSERT( sizeof(qtv_msg_struct) >= sizeof(qtvhsm::event_class) ); 

  if( msg_ptr != NULL)
  {
    qtvhsm::event_class* evt_ptr = ( qtvhsm::event_class* )msg_ptr; /*lint !e740 */

    evt_ptr->kind = kind;
    result = m_vrend_task_.dispatch( this, msg_ptr, delay_ms );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_FATAL, "out of memory" ); 
  }

  return result;
}

