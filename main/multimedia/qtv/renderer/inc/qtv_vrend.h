#ifndef QTV_VREND_H
#define QTV_VREND_H
/* =======================================================================

                          qtv_vrend.h

DESCRIPTION
  Definition of the public API for managing the QTV video renderer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/inc/qtv_vrend.h#9 $
$DateTime: 2009/08/14 03:00:59 $
$Change: 996932 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_task_if.h"
#include "qtv_hsm.h"
#include "vdecoder_types.h"
#include "video_renderer_broadcast.h"
#include "videoDec.h"
#include "video.h"
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
class QTV_BroadcastTransmitter;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Macro Definitions
** ======================================================================= */

/* =======================================================================
**                        Class & Function Declarations
** ======================================================================= */

class qtv_vrend_class : private qtvhsm::hsm_class, private qtv_msg_sink_class
{
public:
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
  =========================================================================*/
  qtv_vrend_class( VideoDec &videoDec,
                   const unsigned preroll = 1 );

  /* ======================================================================
  FUNCTION:
    ~qtv_vrend_class

  DESCRIPTION:
    Destructor for the video renderer.

  PARAMETERS:
    None.

  RETURN VALUE:
    None.
  =========================================================================*/
  virtual ~qtv_vrend_class( void );

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
  =========================================================================*/
  void enable( void );

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
  =========================================================================*/
  void disable( void );

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
  =========================================================================*/
  void render( VDEC_FRAME* const frame_ptr );

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
  void notify_clipInfo_Dimensions( int width, int height );

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
  =========================================================================*/
  bool single_step( void );

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
  =========================================================================*/
  void flush( void );

  /* ======================================================================
  FUNCTION:
    get/set_preroll

  DESCRIPTION:
    Gets/sets the renderer preroll.  Preroll is the number of frames which must
    be available on the input queue before the renderer will draw them.
    This is of import to decoders whose decode order does not match display
    order.

  PARAMETERS:
    const unsigned preroll
      The new preroll.

  RETURN VALUE:
    None.
  =========================================================================*/
  unsigned get_preroll( void ) const { return m_preroll_; }
  void set_preroll( const unsigned preroll );

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
  =========================================================================*/
  void avsync( void );

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
  void set_queueing_mode( VideoPlayer::RendererQueuingMode );

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
  void set_max_timestamp( uint32 );

  /**************************************************************************
  =========================  end of public API  =============================
  **************************************************************************/

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
  =========================================================================*/
  virtual void dispatch( qtv_msg_struct* const msg_ptr );
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
  void release_frame( const QtvPlayer::FrameInfoT &frame );

  HSM_DEFINE_CALL_STATE_FN();

  enum
  {
    EVENT_ENABLE = qtvhsm::event_class::EVENT_USER,
    EVENT_DISABLE,
    EVENT_FRAME,
    EVENT_SINGLE_STEP,
    EVENT_FLUSH,
    EVENT_SET_PREROLL,
    EVENT_RENDER,
    EVENT_DROP,
    EVENT_AVSYNC,
    EVENT_PCR_RESET,
    EVENT_SET_QUEUEING_MODE,
    EVENT_SET_MAX_TIMESTAMP,
    EVENT_RELEASE_FRAME,
	EVENT_CLIPINFO_DIMENSION
  };

private:

  enum
  {
    /* The renderer will drop no more than this many frames in a row.
    ** This allows the user to see some screen activity even if we're
    ** hopelessly late.
    */
    SEQUENTIAL_FRAME_DROP_THRESHOLD = 6
  };

  /* Parameterized events for the state machine.
  */
  class clipinfo_event_class : public qtvhsm::event_class
  {
  public:
    void init( int width, int height )
    {
      kind        = EVENT_CLIPINFO_DIMENSION;
      this->width  = width;
      this->height = height;
    }

    int width;
	int height;
  };

  class frame_event_class : public qtvhsm::event_class
  {
  public:
    void init( event_kind_type k, VDEC_FRAME* f )
    {
      kind      = k;
      frame_ptr = f;
    }

    VDEC_FRAME* frame_ptr;
  };

  class single_step_event_class : public qtvhsm::event_class
  {
  public:
    void init( event_kind_type k,
               bool*           s,
               bool*           r )
    {
      kind       = k;
      sync_ptr   = s;
      result_ptr = r;
    }

    bool* sync_ptr;
    bool* result_ptr;
  };

  class simple_sync_event_class : public qtvhsm::event_class
  {
  public:
    void init( event_kind_type k,
               bool*           s )
    {
      kind     = k;
      sync_ptr = s;
    }

    bool* sync_ptr;
  };

  class set_preroll_event_class : public qtvhsm::event_class
  {
  public:
    void init( event_kind_type k,
               unsigned        p )
    {
      kind    = k;
      preroll = p;
    }

    unsigned preroll;
  };

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  class set_queueing_mode_event_class : public qtvhsm::event_class
  {
  public:
    void init( VideoPlayer::RendererQueuingMode m )
    {
      kind = EVENT_SET_QUEUEING_MODE;
      mode = m;
    }

    VideoPlayer::RendererQueuingMode mode;
  };

  class set_max_timestamp_event_class : public qtvhsm::event_class
  {
  public:
    void init( uint32 m )
    {
      kind = EVENT_SET_MAX_TIMESTAMP;
      max_ts = m;
    }

    uint32 max_ts;
  };
   class release_frame_event_class : public qtvhsm::event_class
   {
   public:
      void init(QtvPlayer::FrameInfoT* frame)
      {
         kind = EVENT_RELEASE_FRAME;
         pframe = frame;
      }

      QtvPlayer::FrameInfoT* pframe;
  };

  /* These links allow frame pointers to be stored in a q_type.
  */
  struct frame_link_struct
  {
    q_link_type link;
    VDEC_FRAME* frame_ptr;
  };

  /* states!  See comments in qtvhsm.h for detail on these.
  */
  HSM_DECLARE_STATE( initial_state_ );
  HSM_DECLARE_STATE( main_state_ );
  HSM_DECLARE_STATE( disabled_state_ );
  HSM_DECLARE_STATE( enabled_state_ );
  HSM_DECLARE_STATE( idle_state_ );
  HSM_DECLARE_STATE( active_state_ );
  /*
  ** End of states */

  /* Handles event entries in the active state to keep the main state
  ** function nice and neat.
  */
  void active_entry_( void );

  /* Places a queue in its proper sorted position on the input queue.
  */
  void queue_frame_( VDEC_FRAME* const frame_ptr );

  /* Renders the video frame on the head of the queue.
  */
  bool render_frame_( void );

  /* Drops the frame at the head of the input queue.
  */
  void drop_frame_( const QTV_VideoRendererBroadcast::Subtype reason );

  /* Drops the given frame.
  */
  void drop_frame_( frame_link_struct* link_ptr,
                    const QTV_VideoRendererBroadcast::Subtype reason );
  void drop_frame_( VDEC_FRAME* const frame_ptr,
                    const QTV_VideoRendererBroadcast::Subtype broadcast_reason,
                    const VideoDec::FrameReleaseReason        release_reason = VideoDec::FRAME_DROPPED_RENDER_FAILURE);

  /* Flushes the input queue.
  */
  void flush_( void );

  /* Sets the renderer preroll.
  */
  void set_preroll_( const qtvhsm::event_class* const event_ptr );

  /* Returns true if our preroll is met - that is, if the input queue is
  ** as long or longer than the preroll.
  */
  bool preroll_is_met_( void );

  /* Gets timing information for the frame at the head of the input queue:
  ** whether or not it's late, and how far we are, in ms, from its display time.
  */
  void get_frame_timing_( bool &frame_is_late,
                          long &sleep_ms,
                          unsigned long* timestamp_ptr );

  /* Gets timing information for the provided frame.
  ** Otherwise, just like the other one.
  */
  void get_frame_timing_( const VDEC_FRAME& frame,
                          bool &frame_is_late,
                          long &sleep_ms,
                          unsigned long* timestamp_ptr );

  /* Broadcasts information about a renderer event.
  */
  void broadcast_( const QTV_Broadcast::Type                  type,
                   const QTV_VideoRendererBroadcast::Subtype  subtype,
                   const VDEC_FRAME                          &frame );

  /* Constructs and dispatches an event to ourselves via qtv_task_if.
  */
  qtv_task_if_class::dispatch_id_type
    send_event_to_self_( const int kind, const unsigned long delay_ms );

  /* Compares timestamps and identifies the bigger one, paying attention
  ** to validity bits and rollover. */
  enum compare_result
  {
     LEFT_IS_LARGER,
     EQUAL,
     RIGHT_IS_LARGER
  };

  compare_result compare_timestamps_( const uint64& left, const uint64& right );

  /* Utility functions used by queue_frame to handle all the funky
  ** sub-cases in that operation */
  void append_to_frame_q_( VDEC_FRAME* const frame_ptr );
  void replace_frame_( VDEC_FRAME* const frame_ptr,
                       frame_link_struct* const after_ptr );
  void insert_in_frame_q_( VDEC_FRAME* const frame_ptr,
                           frame_link_struct* const after_ptr );

  /* Creates a timestamp with the decoder-defined 'invalid' bit set. */
  static uint64 invalid_timestamp_( void ) { return 0x0000000100000000ULL; }

  /* Utility functions to hide the nitty-gritty of last-frame-rendered accounting.
  ** Really, this is pretty trivial and could probably be omitted if not for
  ** the complication of invalid timestamps */
 public:
  void set_last_frame_rendered_( const uint64& ts ) { m_last_frame_rendered_ = ts; }
private:
  void reset_last_frame_rendered_( void )    { m_last_frame_rendered_ = invalid_timestamp_(); }
  uint64 get_last_frame_rendered_( void )    { return m_last_frame_rendered_; }
  bool last_frame_rendered_is_valid_( void ) { return ( m_last_frame_rendered_ & invalid_timestamp_() ) == 0; }

  qtv_task_if_class m_vrend_task_;
    /* All renderers share the same task. */
  VideoDec &m_videoDec_;
    /* The videoDec object for our player instance.
    ** It provides us with timing information, display abilities,
    ** and administrative sundries. */
  unsigned m_preroll_;
    /* Our preroll - we'll not render unless at least this many frames
    ** are in our input queue. */
  qtv_task_if_class::dispatch_id_type m_timer_dispatch_id_;
    /* If we have a timer/delayed dispatch running, we store its id here. */
  unsigned m_sequential_drops_;
    /* The number of frames we've dropped sequentially.  Every successful
    ** render resets this. */
  q_type m_frame_q_;
  /*frame queue critical section*/
   rex_crit_sect_type m_frame_q_cs;
    /* Our frame queue holds frames waiting to be rendered. */
  QTV_BroadcastTransmitter* m_transmitter_ptr_;
    /* We broadcast frame-render messages via this transmitter */
  VideoPlayer::RendererQueuingMode m_queueing_mode_;
  uint32 m_max_timestamp_;
    /* Timestamps larger than this are not allowed.  If a value bigger
    ** than this is received, it will be rejected as out of bounds.  This is
    ** only 32 bits because only 32 bits of the decoder's timestamp type
    ** are the time value - the rest are bit-flags */
  uint64 m_last_frame_rendered_;
    /* The timestamp of the last frame rendered.  This one *is* 64 bits
    ** because the is-invalid bit is used in cases where no frame has
    ** yet been rendered. */
};

#endif // QTV_VREND_H

