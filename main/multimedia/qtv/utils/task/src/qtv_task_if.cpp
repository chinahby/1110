/* =======================================================================

                          qtv_task_if.cpp

DESCRIPTION
  Implementation of types and classes to support the message/IPC-based qtv 
  common-task primitive class.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/src/qtv_task_if.cpp#8 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_task_if.h"
#include "assert.h"
#include "qcplayer.h" /* for COMMON_STACK_SIZE */

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

/* ========================================================================
FUNCTION:
  qtv_task_if_class::qtv_task_if_class

DESCRIPTION:
  Constructor for qtv_task_if_class.  Starts a task.

PARAMETERS:
  const rex_priority_type   priority
    The priority at which the task will run.

  const char * const        task_name_ptr
    The task's name - useful for debugging.

  const bool              init_on_construct
    If true, the task starts on constrution.  Otherwise, Init() must
    be called before the task can be used.

RETURN VALUE:
  None.
===========================================================================*/
qtv_task_if_class::qtv_task_if_class
(
  rex_tcb_type* const task_tcb_ptr,
  qtv_task_info_struct& task_info  
)
: m_task_tcb_ptr_( task_tcb_ptr ),
  m_task_info_( task_info )
{
}

/* ========================================================================
FUNCTION:
  qtv_task_if_class::~qtv_task_if_class

DESCRIPTION:
  Destructor.  Stops a task.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_task_if_class::~qtv_task_if_class( void )
{
}

/* ========================================================================
FUNCTION:
  qtv_task_if_class::dispatch

DESCRIPTION:
  Accepts a message from the outside world for dispatching.  What this means
  is that we'll queue it and signal our task that something's up.

  This function can be thought of as an 'indirect dispatch'.  Clients
  give the task a sink and a message and let the task dispatch the message
  to the sink.

PARAMETERS:
  qtv_msg_sink_class* const recipient_ptr
    The sink for which the message is destined.

  qtv_msg_struct* const msg_ptr
    The message being submitted.

RETURN VALUE:
  dispatch_id_type
    An identifier for the dispatch which can be used to cancel the dispatch
    before it executes.
===========================================================================*/
qtv_task_if_class::dispatch_id_type qtv_task_if_class::dispatch
(
  qtv_msg_sink_class* const recipient_ptr, 
  qtv_msg_struct* const msg_ptr,
  const uint32 delay_ms
)
{
  bool         msg_is_dispatched = false;
  link_struct* link_ptr = 0;

  if ( recipient_ptr )
  {
    link_ptr = link_struct::malloc();
    if ( link_ptr )
    {
      ( void )q_link( link_ptr, &( link_ptr->link ) );
      link_ptr->type = link_struct::DISPATCH;
      link_ptr->parcel.fill( this, recipient_ptr, msg_ptr, delay_ms );
      QCUtils::EnterCritSect( &m_task_info_.input_cs );
      q_put( &m_task_info_.input_q, &( link_ptr->link ) );

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
      QTV_MSG_PRIO4( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                    "qtv_task_if_class::dispatch =>recipient_ptr=0x%x, delay_ms=%d, type=%d, input_q_cnt=%d",
                     link_ptr->parcel.recipient_ptr,
                     link_ptr->parcel.delay_ms,
                     link_ptr->type,
                     q_cnt(&m_task_info_.input_q) );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG
      QCUtils::LeaveCritSect( &m_task_info_.input_cs );

      ( void )rex_set_sigs( m_task_tcb_ptr_, QTV_TASK_INPUT_SIG );
      msg_is_dispatched = true;
    }
  }

  if ( !msg_is_dispatched )
  {
    QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "qtv_task_if_class::dispatch failed! message lost." );
    if ( msg_ptr )
    {
      qtv_msg_struct::free( msg_ptr );
    }
  }

  return ( dispatch_id_type )link_ptr;
}

/* ========================================================================
FUNCTION:
  qtv_task_if_class::cancel_dispatch

DESCRIPTION:
  Aborts a pending delayed dispatch.

PARAMETERS:
  dispatch_id_type dispatch_id
    The id of the pending dispatch.  A link pointer in disguise.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_task_if_class::cancel_dispatch( dispatch_id_type dispatch_id )
{
  link_struct* link_ptr = 0;
  link_struct* head_ptr = 0;

  link_ptr = link_struct::malloc();
  if ( link_ptr )
  {
    ( void )q_link( link_ptr, &( link_ptr->link ) );
    link_ptr->type = link_struct::CANCELLATION;
    link_ptr->dispatch_id = dispatch_id;

    /* cancellations are higher priority than regular dispatches.  To
    ** close the hole caused by submission of everything through a single
    ** FIFO queue, cancellations are pushed into the FRONT of the input
    ** queue.  This is intentended only as a short-term fix. */
    QCUtils::EnterCritSect( &m_task_info_.input_cs );

    head_ptr = ( link_struct* )q_check( &m_task_info_.input_q );
    if ( head_ptr != 0 )
    {
      q_insert(
#ifdef FEATURE_Q_NO_SELF_QPTR
                &m_task_info_.input_q,
#endif
                &link_ptr->link,
                &head_ptr->link );
    }
    else
    {
      q_put( &m_task_info_.input_q, &( link_ptr->link ) );
    }
    QCUtils::LeaveCritSect( &m_task_info_.input_cs );

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                  "qtv_task_if_class::cancel_dispatch => dispatch_id=0x%x",
                   dispatch_id );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

    ( void )rex_set_sigs( m_task_tcb_ptr_, QTV_TASK_INPUT_SIG );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "qtv_task_if_class::cancel_dispatch failed!" );
  }
}

/* ========================================================================
FUNCTION:
  qtv_task_if_class::parcel_struct_dispatch

DESCRIPTION:
  Dispatches the message contained in a parcel to its final destination.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_task_if_class::parcel_struct::dispatch( void )
{
  if ( recipient_ptr )
  {
    if ( !recipient_ptr->dispatch_to_sink( msg_ptr ) )
    {
      /* Remember that the sink deletes the message after it's done
      ** processing it, so we must only free it if the dispatch failed. */
      QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "qtv_task_if_class::parcel_struct::dispatch failed =>recipient_ptr=0x%x, delay_ms=%d",
                     recipient_ptr, delay_ms );

      qtv_msg_struct::free( msg_ptr );
    }
  
    recipient_ptr->release();
  }
  else
  {
    qtv_msg_struct::free( msg_ptr );
  }

  fill( 0, 0, 0, 0 );
}

/* ========================================================================
FUNCTION:
  qtv_task_if_class::parcel_struct::fill

DESCRIPTION:
  A convenience function for filling in parcels in a nice tidy type-safe way.

PARAMETERS:
  qtv_msg_sink_class* const recipient_ptr_in
  qtv_msg_struct*     const msg_ptr_in
    The stuff to put in the parcel.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_task_if_class::parcel_struct::fill
(
  qtv_task_if_class* const  sender_obj_ptr_in,
  qtv_msg_sink_class* const recipient_ptr_in,
  qtv_msg_struct* const     msg_ptr_in,
  uint32                    delay_ms_in
)
{
  sender_obj_ptr = sender_obj_ptr_in;
  recipient_ptr  = ( recipient_ptr_in ? recipient_ptr_in->get_handle() : 0 );
  msg_ptr        = msg_ptr_in;
  delay_ms       = delay_ms_in;
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::qtv_msg_sink_handle_class

DESCRIPTION:
  Constructor.  And a boring one at that.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_msg_sink_handle_class::qtv_msg_sink_handle_class( void )
: m_ref_cnt_( 0 ),
  m_sink_ptr_( 0 )
{
  QCUtils::InitCritSect( &m_ref_cnt_cs_ );
  QCUtils::InitCritSect( &m_sink_ptr_cs_ );
  add_ref();
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::~qtv_msg_sink_handle_class

DESCRIPTION:
  Destructor.  Does nothing, and should stay that way!  Since handles
  are self-destructing, destruction must stay dirt-simple to be safe.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_msg_sink_handle_class::~qtv_msg_sink_handle_class( void )
{
  QCUtils::DinitCritSect( &m_ref_cnt_cs_ );
  QCUtils::DinitCritSect( &m_sink_ptr_cs_ );
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::add_ref

DESCRIPTION:
  Increments the handle's reference count.

PARAMETERS:
  None.

RETURN VALUE:
  qtv_msg_sink_handle_class*.
    A self pointer - makes "copy by add_ref" easier.
===========================================================================*/
qtv_msg_sink_handle_class* qtv_msg_sink_handle_class::add_ref( void )
{
  /* In add-ref and release, we must protect the ref-cnt arithmetic
  ** to avoid corruption of the count by multiple simultaneous operations.
  ** Imagine:
  ** 1) task A reads m_ref_cnt_, swaps out.
  ** 2) task B reads m_ref_cnt_, increments, stores, swaps out.
  ** 3) task A increments and stores...and now ref_cnt is wrong. */
  QCUtils::EnterCritSect( &m_ref_cnt_cs_ );
  ++m_ref_cnt_;
  QCUtils::LeaveCritSect( &m_ref_cnt_cs_ );

  return this;
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::release

DESCRIPTION:
  Decrements the handle's reference count.  Clients must not use the handle
  after releasing it, as the handle will delete itself when its reference
  count goes to zero.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_msg_sink_handle_class::release( void )
{
  uint32 cached_ref_cnt;

  /* See comment in add_ref about the critical section */
  QCUtils::EnterCritSect( &m_ref_cnt_cs_ );
  cached_ref_cnt = --m_ref_cnt_;
  QCUtils::LeaveCritSect( &m_ref_cnt_cs_ );

  if ( cached_ref_cnt == 0 )
  {
    QTV_Delete( this );
  }
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::set_sink

DESCRIPTION:
  Sets the sink pointed to by the handle.  This is intended for use only
  by the creating sink, both to set up the handle at startup and to clear
  it when the sink is deleted.

PARAMETERS:
  qtv_msg_sink_class* sink_ptr
    The sink!

RETURN VALUE:
  None.
===========================================================================*/
void qtv_msg_sink_handle_class::set_sink( qtv_msg_sink_class* sink_ptr )
{
  /* This operation does not need to be locked by the same mechanism as
  ** add_ref & release, as it does not touch the reference count.
  ** However, access to the sink_ptr must still be locked - see 
  ** dispatch_to_sink for more discussion. */
  QCUtils::EnterCritSect( &m_sink_ptr_cs_ );
  m_sink_ptr_ = sink_ptr;
  QCUtils::LeaveCritSect( &m_sink_ptr_cs_ );
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_handle_class::dispatch_to_sink

DESCRIPTION:
  Attempts to dispatch a message to the sink referenced by this handle.

PARAMETERS:
  qtv_msg_struct* const msg_ptr
    The message to be dispatched.

RETURN VALUE:
  bool
    true if dispatch succeeded, false otherwise.
===========================================================================*/
bool qtv_msg_sink_handle_class::dispatch_to_sink
(
  qtv_msg_struct* const msg_ptr
)
{
  bool msg_was_dispatched = false;

  /* Notes on reentrancy controls in dispatch_to_sink:
  ** 
  ** destruction of 'this' during dispatch execution is not a concern,
  ** since dispatch_to_sink is only called via parcel_struct.  Even if
  ** all other references to the handle are lost, the parcel will not
  ** release until after dispatch_to_sink returns.
  **
  ** the sink pointer must be preserved while the dispatch is running.
  ** otherwise a sink could remove itself from the handle (by setting
  ** sink_ptr to 0) and then be destructed during a dispatch.
  ** This problem is avoided by blocking outside access to sink_ptr
  ** during dispatch or set_sink operations. */
  QCUtils::EnterCritSect( &m_sink_ptr_cs_ );
  if ( m_sink_ptr_ )
  {
    m_sink_ptr_->dispatch( msg_ptr );
    msg_was_dispatched = true;
  }
  QCUtils::LeaveCritSect( &m_sink_ptr_cs_ );

  return msg_was_dispatched;
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_class::qtv_msg_sink_class

DESCRIPTION:
  Constructor for msg_sinks.  Creates a handle for use in dispatching
  and points it to self.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_msg_sink_class::qtv_msg_sink_class( void )
: m_handle_ptr_( QTV_New( qtv_msg_sink_handle_class ))
{
  QCUtils::InitCritSect( &m_handle_cs_ );

  QCUtils::EnterCritSect( &m_handle_cs_ );

  if ( m_handle_ptr_ )
  {
    m_handle_ptr_->set_sink( this );
  }
  else
  {
    QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                  "qtv_msg_sink_class out of memory" );
  }
  QCUtils::LeaveCritSect( &m_handle_cs_ );
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_class::~qtv_msg_sink_class

DESCRIPTION:
  Destructor for msg_sinks.  Nulls our handle's sink pointer so any 
  remaining references to us will know we've gone away.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
qtv_msg_sink_class::~qtv_msg_sink_class( void )
{
  /* We 0 out m_handle_ptr_ early to avoid race conditions which would allow
  ** new messages to be queued which reference this object while we're 
  ** destructing. */
  qtv_msg_sink_handle_class* cached_handle_ptr = 0;

  QCUtils::EnterCritSect( &m_handle_cs_ );
  if ( m_handle_ptr_ )
  {
    cached_handle_ptr = m_handle_ptr_;
    /* After this point, further get_handle calls will return 0, which will 
    ** cause parcel::dispatch to silently free messages submitted after this
    ** point.  This does NOT address messages already in queue. 
    **
    ** Really, this should never happen - for this to occur, clients 
    ** would have to keep calling dispatch after our destruction has
    ** begun, which is against all the rules of man and beast. */
    m_handle_ptr_ = 0; 
    /* After this point, qtv_msg_sink_handle_class::dispatch_to_sink
    ** will fail - this will prevent anything already in queue from
    ** referencing this sink.
    **
    ** Unlike the previous case, this one is commonplace. */
  }
  QCUtils::LeaveCritSect( &m_handle_cs_ );

  if (cached_handle_ptr != NULL)
  {
    cached_handle_ptr->set_sink( 0 );
    cached_handle_ptr->release();
  }
  QCUtils::DinitCritSect(&m_handle_cs_);

  // no else: empty handles do not require cleanup
}

/* ========================================================================
FUNCTION:
  qtv_msg_sink_class::get_handle

DESCRIPTION:
  Returns a sink's handle.  Intended for use by dispatchers to obtain
  the 'safe' handle by which they can refer to this sink in the future
  with no danger of stale (post-destruction) references.

  get_handle does an implicit add_ref on the handle.

PARAMETERS:
  None.

RETURN VALUE:
  qtv_msg_sink_handle_class*
    The handle by which this sink can be referenced in the future.
===========================================================================*/
qtv_msg_sink_handle_class* qtv_msg_sink_class::get_handle( void )
{
  qtv_msg_sink_handle_class* handle_ptr = 0;

  QCUtils::EnterCritSect( &m_handle_cs_ );
  if ( m_handle_ptr_ )
  {
    handle_ptr = m_handle_ptr_->add_ref();
  }
  QCUtils::LeaveCritSect( &m_handle_cs_ );

  return handle_ptr;
}
