/* =======================================================================

                          qtv_task_if_task.cpp

DESCRIPTION
  Definition of the task function used by qtv_task_if.  This is in
  a seperate header and not part of the class definition because it is
  used by services/task, which is not C++.

  The task and associated functions in this file are intended as a "free-standing"
  QTV services/task-based task, to which qtv_task_if_class objects will attach
  from QTV in order to dispatch messages.  Nothing here should refer to
  qtv_task_if_class internals.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/src/qtv_task_if_task.cpp#11 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

extern "C"
{
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
  #include "task.h"
  #include "dog.h"
  #include "assert.h"
}

#ifndef FEATURE_QTV_STATIC_TASKS
#error FEATURE_QTV_STATIC_TASKS must be defined in custmp4.h!
#endif

#include "qtv_task_if_task.h"
#include "qtv_task_if.h"
/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Halts dog-monitoring for this task.  This is a temporary workaround
** for the fact that there exist tasks on the 4.5 branch with a lower
** priority than video-decoder.  video-decoder is allowed to hog CPU,
** and therefore tasks of lower priority must cope with being CPU starved. */
#define DISABLE_WATCHDOG_MONITORING

  /* services/tmc uses signals up to 0x0010.
  ** services/task uses 0x2000 to 0x8000. */
const rex_sigs_type QTV_TASK_DOG_RPT_SIG = 0x0020;
const rex_sigs_type QTV_TASK_INPUT_SIG   = 0x0040;
const rex_sigs_type FIRST_TIMER_SIG      = 0x0080;
const rex_sigs_type LAST_USABLE_SIG      = 0x1000;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Timer structures are used to keep track of pending delayed-dispatch
** messages. */
struct timer_record_struct
{
  rex_timer_type                  timer;
  qtv_task_if_class::link_struct* link_ptr;
};

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/*
** Setup info for qtv task functions.  Each task needs its own!
*/

qtv_task_info_struct qtv_task_video_renderer_info =
{
  DOG_QTV_VIDEO_RENDERER_RPT_TIME,
  { 0 }, /* input queue */
  { 0 }  /* input cs */
};

qtv_task_info_struct qtv_task_audio_info =
{
  DOG_QTV_AUDIO_RPT_TIME,
  { 0 }, /* input queue */
  { 0 }  /* input cs */
};

#ifdef DOG_QTV_TASK10_RPT_TIME
qtv_task_info_struct qtv_task10_info =
{
  DOG_QTV_TASK10_RPT_TIME,
  { 0 }, /* input queue */
  { 0 }  /* input cs */
};
#endif /* DOG_QTV_TASK10_RPT_TIME */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
static void do_qtv_signal_handling
(
  rex_sigs_type                   sigs,
  q_type*                         input_q_ptr,
  rex_crit_sect_type*             input_q_cs_ptr,
  timer_record_struct*            dispatch_timers,
  int                             num_timers
);

static void do_dispatch
(
  qtv_task_if_class::link_struct* link_ptr,
  timer_record_struct*            timers,
  int                             num_timers
);

static void do_cancellation
(
  rex_sigs_type&                  sigs,
  qtv_task_if_class::link_struct* link_ptr,
  q_type*                         input_q_ptr,
  timer_record_struct*            timers,
  int                             num_timers
);

static void release_link
(
  qtv_task_if_class::link_struct* link_ptr
);

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ========================================================================
FUNCTION:
  qtv_task_fn

DESCRIPTION:
  The task function for qtv tasks, to be used when starting static
  tasks from services/task.

PARAMETERS:
  unsigned long
    A pointer to our setup info, disguised as a ulong.

RETURN VALUE:
  None.
===========================================================================*/
void qtv_task_fn( unsigned long info_as_long )
{
  const int NUM_DISPATCH_TIMERS = 6;
  int i = 0;

  qtv_task_info_struct* info_ptr = ( qtv_task_info_struct* )info_as_long;

  timer_record_struct dispatch_timers[ NUM_DISPATCH_TIMERS ];
  rex_timer_type      dog_rpt_timer;
  rex_sigs_type       sigs;
  rex_sigs_type       wait_sigs = QTV_TASK_INPUT_SIG | 
                                  TASK_OFFLINE_SIG |
                                  TASK_STOP_SIG;
  /*----- INITIALIZATION -----*/

  q_init( &info_ptr->input_q );
  QCUtils::InitCritSect( &info_ptr->input_cs );

  rex_def_timer( &dog_rpt_timer,
                 rex_self(),
                 QTV_TASK_DOG_RPT_SIG );

  sigs = FIRST_TIMER_SIG;
  for ( i = 0; i < NUM_DISPATCH_TIMERS; ++i )
  {
    ASSERT( sigs <= LAST_USABLE_SIG );
    rex_def_timer( &( dispatch_timers[ i ].timer ), rex_self(), sigs );
    dispatch_timers[ i ].link_ptr = 0;
    wait_sigs |= sigs;
    sigs <<= 1;
  }

#ifndef FEATURE_WINCE
  task_start( QTV_TASK_DOG_RPT_SIG,
              rex_self()->dog_report_val,
              &dog_rpt_timer);
#endif

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events. */
  if ( rex_self()->dog_report_val != -1 )
  {
    #ifndef DISABLE_WATCHDOG_MONITORING
      /* Set the watchdog report timer signal so that we'll
      ** reset the watchdog as soon as we start checking for events. */
      wait_sigs |= QTV_TASK_DOG_RPT_SIG;
      ( void )rex_set_sigs( rex_self(), QTV_TASK_DOG_RPT_SIG );
    #else
      /* Disable the watchdog for this task, despite the fact that it was set 
      ** in services/task.  There's a little magic here.
      **
      ** dog_monitor_pause only blocks watchdog events on a task until a signal 
      ** is delivered to that task, at which point the watchdog is automatically 
      ** reenabled.
      **
      ** We get around this by breaking the TCB's link to the dog_task_state
      ** array, and then marking that element in the array BLOCKED, or paused.
      ** Subsuquent signals to the task are unable to reenable the watchdog
      ** because the dog_report_val entry has been set to -1, or disabled. 
      ** This makes the temporary watchdog block into a permanent one. */
      unsigned long dog_report_val;

      dog_report_val = rex_self()->dog_report_val;
      rex_self()->dog_report_val = -1;
#ifndef FEATURE_WINCE
      dog_monitor_pause( dog_report_val );
#endif
    #endif // not DISABLE_WATCHDOG_MONITORING
  }

  /*----- MAIN LOOP -----*/

  for (;;)
  {
    sigs = rex_wait( wait_sigs );
    rex_clr_sigs( rex_self(), sigs );

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
    QTV_MSG_PRIO2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                  "qtv_task_fn %x => sigs=0x%x", rex_self(), sigs );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

    /*-------------------------------------------------------------------------
    ** task services signal handling
    */
    #ifndef DISABLE_WATCHDOG_MONITORING
    if ( sigs & QTV_TASK_DOG_RPT_SIG )
    {
      dog_report( rex_self()->dog_report_val );
      ( void )rex_set_timer( &dog_rpt_timer, info_ptr->dog_rpt_interval );

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                  "qtv_task_fn %x woof", rex_self() );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG
    }
    #endif // not DISABLE_WATCHDOG_MONITORING

    if ( sigs & TASK_STOP_SIG )
    {
      break;
    } /* TASK_STOP_SIG? */

    if ( sigs & TASK_OFFLINE_SIG )
    {
      /* We don't actually go offline, but we must do the handshake. */
#ifndef FEATURE_WINCE
      task_offline();
#endif
    } /* TASK_OFFLINE_SIG? */

    /*-------------------------------------------------------------------------
    ** qtv_task signal handling
    */
    do_qtv_signal_handling( sigs,
                            &info_ptr->input_q,
                            &info_ptr->input_cs,
                            dispatch_timers,
                            NUM_DISPATCH_TIMERS );
  } /* forever */

  /*----- SHUTDOWN -----*/

  ( void )rex_clr_timer( &dog_rpt_timer );

  // Clear all the dispatch_timers
  for ( i = 0; i < NUM_DISPATCH_TIMERS; ++i )
  {
    rex_clr_timer( &( dispatch_timers[ i ].timer ));
  }

#ifndef FEATURE_WINCE
  task_stop();
#endif
}

/* ========================================================================
FUNCTION:
  get_link_from_protected_q

DESCRIPTION:
  Pulls a link from a critical-section guarded queue.

PARAMETERS:
  q_type* q_ptr,
    The queue.

  rex_crit_sect_type* cs_ptr
    The critical section.

RETURN VALUE:
  qtv_task_if_class::link_struct*
    The link.
===========================================================================*/
qtv_task_if_class::link_struct* get_link_from_protected_q
(
  q_type* q_ptr,
  rex_crit_sect_type* cs_ptr
)
{
  qtv_task_if_class::link_struct* link_ptr;
  QCUtils::EnterCritSect( cs_ptr );
  link_ptr  = ( qtv_task_if_class::link_struct* )q_get( q_ptr );
  QCUtils::LeaveCritSect( cs_ptr );
  return link_ptr;
}

/* ========================================================================
FUNCTION:
  do_qtv_signal_handling

DESCRIPTION:
  Handles all QTV-specific signals.

PARAMETERS:
  rex_sigs_type                   sigs,
    The signals which caused the function call in the first place.

  q_type*                         input_q_ptr,
    Pointers to the calling task's queues.

  timer_record_struct*            timers,
  int                             num_timers
    Information about the calling task's delayed-dispatch timers.

RETURN VALUE:
  None.
===========================================================================*/
void do_qtv_signal_handling
(
  rex_sigs_type        sigs,
  q_type*              input_q_ptr,
  rex_crit_sect_type*  input_q_cs_ptr,
  timer_record_struct* dispatch_timers,
  int                  num_timers
)
{
  qtv_task_if_class::link_struct* link_ptr           = 0;

  /*
  ** Process all outstanding commands.  This can result in dispatches
  ** being placed in timers or on the dispatch queue.
  */
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
  QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                    "do_qtv_signal_handling =>input_q_cnt=%d", q_cnt(input_q_ptr) );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

  if ( sigs & QTV_TASK_INPUT_SIG )
  {
    for( link_ptr  = get_link_from_protected_q( input_q_ptr, input_q_cs_ptr );
         link_ptr != 0;
         link_ptr  = get_link_from_protected_q( input_q_ptr, input_q_cs_ptr ))
    {
      switch( link_ptr->type )
      {
        case qtv_task_if_class::link_struct::DISPATCH:
        {
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
          QTV_MSG_PRIO4( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                        "do_qtv_signal_handling =>type=%d, recipient_ptr=0x%x, delay_ms=%d, input_q_cnt=%d",
                         link_ptr->type,
                         link_ptr->parcel.recipient_ptr,
                         link_ptr->parcel.delay_ms,
                         q_cnt(input_q_ptr) );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

          do_dispatch( link_ptr,
                       dispatch_timers,
                       num_timers );
          /* No release here!  If the message was immediately dispatched, we released
          ** the link then - otherwise it's on a timer struct and needs to stay around. */
          break;
        }
        case qtv_task_if_class::link_struct::CANCELLATION:
        {
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
          QTV_MSG_PRIO3( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                        "do_qtv_signal_handling =>type=%d, dispatch_id=0x%x, input_q_cnt=%d",
                         link_ptr->type,
                         link_ptr->dispatch_id,
                         q_cnt(input_q_ptr) );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

          QCUtils::EnterCritSect( input_q_cs_ptr );
          do_cancellation( sigs,
                           ( qtv_task_if_class::link_struct* )link_ptr->dispatch_id,
                           input_q_ptr,
                           dispatch_timers,
                           num_timers );
          QCUtils::LeaveCritSect( input_q_cs_ptr );
          release_link( link_ptr );
          break;
        }
        default:
        {
          ASSERT( 0 );
        }
      } /* switch on input type */
    } /* for each item in input q */
  } /* CMD_SIG? */

  /*
  ** Check for expired timers and dispatch them.
  */
  for ( int i = 0; i < num_timers; ++i )
  {
    if ( sigs & dispatch_timers[ i ].timer.sigs )
    {
      /* There exists a small chance that the timer was cancelled between
      ** the moment the timer went off and now, so we must make sure
      ** the link pointer is still valid before working on it. */
      if ( dispatch_timers[ i ].link_ptr != 0 )
      {
        link_ptr = dispatch_timers[ i ].link_ptr;
        dispatch_timers[ i ].link_ptr = 0;
        link_ptr->parcel.dispatch();
        release_link( link_ptr );

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
        QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                      "do_qtv_signal_handling =>timer expired" );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG
      }
    } /* dispatch_timers[ i ].sigs? */
  } /* foreach timer */

  /*
  ** F3 msg to check for the total number of pending timers.
  ** It should be zero after the clip terminates playback.
  */
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
  int numPendingTimers = 0;
  for ( int i = 0; i < num_timers; ++i )
  {
    if ( dispatch_timers[ i ].link_ptr )
    {
      ++numPendingTimers;
    }
  } /* foreach timer */
  QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                    "do_qtv_signal_handling =>pending_timer_cnt=%d", numPendingTimers );

#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG
}

/* ========================================================================
FUNCTION:
  do_dispatch

DESCRIPTION:
  Handles a DISPATCH command.  This may result in an immediate
  dispatch or a placement in the timer array.

PARAMETERS:
  qtv_task_if_class::link_struct* link_ptr,
    The command itself.

  q_type*                         dispatch_q_ptr,
    A pointer to the calling task's dispatch queue.

  timer_record_struct*            timers,
  int                             num_timers
    Information about the calling task's delayed-dispatch timers.

RETURN VALUE:
  None.
===========================================================================*/
void do_dispatch( qtv_task_if_class::link_struct* link_ptr,
                  timer_record_struct*            timers,
                  int                             num_timers )
{
  int i;

  /* Immediate dispatches are easy. */
  if ( link_ptr->parcel.delay_ms == 0 )
  {
    link_ptr->parcel.dispatch();
    release_link( link_ptr );
    return;
  }

  /* Delayed dispatches require us to find a free timer, set it, and park
  ** the link for later delivery. */

  for ( i = 0; i < num_timers; ++i )
  {
    if ( timers[ i ].link_ptr == 0 )
    {
      timers[ i ].link_ptr = link_ptr;
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                    "do_dispatch =>setting a timer for the msg!" );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

      ( void )rex_set_timer( &( timers[ i ].timer ), link_ptr->parcel.delay_ms );
      break;
    }
  } /* for each timer */

  if ( i == num_timers )
  {
    QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                  "delayed dispatch failed! dispatching immediately.." );
    link_ptr->parcel.dispatch();
    release_link( link_ptr );
  }
}

/* ========================================================================
FUNCTION:
  do_cancellation

DESCRIPTION:
  Handles a CANCELLATION command.

PARAMETERS:
  qtv_task_if_class::link_struct* link_ptr,
    A pointer to the command itself.

  q_type*                         input_q_ptr,
    Pointer to the calling task's queue.

  timer_record_struct*            timers,
  int                             num_timers
    Information about the calling task's delayed-dispatch timers.

RETURN VALUE:
  None.
===========================================================================*/
void do_cancellation
(
  rex_sigs_type&                  sigs,
  qtv_task_if_class::link_struct* link_ptr,
  q_type*                         input_q_ptr,
  timer_record_struct*            timers,
  int                             num_timers
)
{
  boolean link_was_removed;

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
  QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "do_cancellation START=> input_q_cnt=%d",
                 q_cnt(input_q_ptr));
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

  /* Remove the link from the input queue if it exists. */
  link_was_removed = 
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_delete_ext( input_q_ptr, &( link_ptr->link ));
#else
    q_delete_ext( &link_ptr->link );
#endif

  /* If that failed, check the timer records and try and pull it from there. */
  if ( link_was_removed == FALSE )
  {
    for ( int i = 0; i < num_timers; ++i )
    {
      if ( timers[ i ].link_ptr == link_ptr )
      {
        timers[ i ].link_ptr = 0;
        ( void )rex_clr_timer( &( timers[ i ].timer ));
        sigs &= ~( timers[ i ].timer.sigs );
        link_was_removed = TRUE;
        break;
      }
    } /* for each timer */
  }

  if ( link_was_removed != FALSE )
  {
#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
    QTV_MSG_PRIO3( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                  "do_cancellation successful =>type=%d, recipient_ptr=0x%x, delay_ms=%d",
                   link_ptr->type,
                   link_ptr->parcel.recipient_ptr,
                   link_ptr->parcel.delay_ms
                 );
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

    release_link( link_ptr );
  }
  else
  {
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                  "do_cancellation failed => dispatch_id=0x%x",
                   link_ptr );
  }

#ifdef QTV_TASK_IF_MSG_QUEUE_DEBUG
  QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "do_cancellation DONE=> input_q_cnt=%d",
                 q_cnt(input_q_ptr));
#endif //QTV_TASK_IF_MSG_QUEUE_DEBUG

}

/* ========================================================================
FUNCTION:
  release_link

DESCRIPTION:
  Releases all the memory held by a link.

PARAMETERS:
  qtv_task_if_class::link_struct* link_ptr,
    A pointer to the link.

RETURN VALUE:
  None.
===========================================================================*/
static void release_link
(
  qtv_task_if_class::link_struct* link_ptr
)
{
  if ( !link_ptr )
  {
    return;
  }

  if ( link_ptr->type == qtv_task_if_class::link_struct::DISPATCH )
  {
    /* dispatches contain sink references and parcels */
    if ( link_ptr->parcel.recipient_ptr )
    {
      link_ptr->parcel.recipient_ptr->release();
    }

    qtv_msg_struct::free( link_ptr->parcel.msg_ptr ); /* nulls are ok */
  }

  qtv_task_if_class::link_struct::free( link_ptr );
  //QCUtils::DinitCritSect( &info_ptr->input_cs );
}

