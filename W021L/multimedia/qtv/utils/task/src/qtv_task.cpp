/* =======================================================================

                          qtv_task.cpp

DESCRIPTION
  Implementation of types and classes to support the basic qtv
  task class.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/src/qtv_task.cpp#8 $
$DateTime: 2008/07/22 04:17:48 $
$Change: 706995 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_task.h"
#include "qtv_msg.h"
#include "AEEstd.h"  /* std_strlcpy */

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
const rex_sigs_type QTV_Task::QTV_TASK_EXIT_SIG = 0x00000001;

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
  QTV_Task

DESCRIPTION:
  Constructor.

PARAMETERS:
  const rex_priority_type   priority
    The priority at which the task will run.

  const char * const        taskName,
    The task's name - useful for debugging.

  const int                 dogReportVal
    The task ID used to report to the rex watchdog task.

  TASK                      taskFn
    The task function to be used.

  void*                     userData
    user data to be passed to the task function.

RETURN VALUE:
  None.
===========================================================================*/
QTV_Task::QTV_Task
(
  const rex_priority_type   priority,
  const char * const        taskName,
  const int                 dogReportVal,
  TASK                      taskFn,
  void*                     userData,
  const bool                initOnConstruction
)
: m_state       ( CONSTRUCTING ),
  m_sigs_in_use ( QTV_TASK_EXIT_SIG ),
  m_taskFn      ( taskFn ),
  m_userData    ( userData ),
  m_priority    ( priority ),
  m_dogReportVal( dogReportVal )
{
  std_strlcpy( m_taskName, taskName, REX_TASK_NAME_LEN+1 );

  if ( initOnConstruction )
  {
    Init();
  }
}

/* ======================================================================
FUNCTION:
  QTV_Task::Init

DESCRIPTION:
  Starts the task.
  This call is synchronous - it will not return
  until the task has completed initialization.

PARAMETERS:
  const rex_priority_type   priority
    The priority at which the task will run.

  const char * const        taskName,
    The task's name - useful for debugging.

  const int                 dogReportVal
    The task ID used to report to the rex watchdog task.

  TASK                      taskFn
    The task function to be used.

  void*                     userData
    user data to be passed to the task function.

RETURN VALUE:
  bool.
    'true' if we could verify that everything started successfully.
    'false' otherwise - we probably still started OK but couldn't verify it.
===========================================================================*/
bool QTV_Task::Init( void )
{
  unsigned long sleepTotal( 0 );

  if ( m_state != CONSTRUCTING )
  {
    return true; /* No intializing twice! */
  }

  /* This is a workaround for a shortcoming in rex_def_task - it does not
  ** initialize ALL of the tcb, so if we don't zero it all out explicitly
  ** junk gets left in places which make for sad behaviors.
  */
  memset( &m_tcb, 0, sizeof( m_tcb ) );
  memset( m_stack, 0, sizeof( m_stack ) );

  rex_def_task_ext2( &m_tcb,
                     m_stack,
                     STACK_SIZE,
                     m_priority,
                     TaskHarness,
                     ( dword )this,
                     m_taskName,
                     FALSE,
                     m_dogReportVal );

  // Because we don't know anything about the calling task,
  // there's no good and consistent mechanism for idling it until
  // the task is up and running.  We could force callers to provide
  // a signal which could be used to block, but that's clunky, and
  // what about the destruction side?  We have to wait there too,
  // and who's to say the same task'll tear us down as brought us up?
  // Polling's clunky, but I think it's the best solution in this case.
  //
  // BUGFIX Nov07 2006 - in a very small percentage of executions, the
  // s_state variable will never be seen to change from CONSTRUCTING.
  // The task does seem to be starting, but we don't see the state variable
  // change here and that leads to an infinite loop.  Cache problems?
  // In any case, I am adding a timeout here to prevent lockups in those cases.
  while ( m_state == CONSTRUCTING && sleepTotal < POLL_TIMEOUT_MS )
  {
    sleepTotal += POLL_SLEEP_MS;
    rex_sleep( POLL_SLEEP_MS );
  }

  if ( m_state != CONSTRUCTING )
  {
    QTV_MSG_SPRINTF_PRIO_2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                            "QTV_Task %s started. %d ms spent waiting for startup",
                            m_taskName, sleepTotal );
    return true;
  }
  else
  {
    /* The timeout does not mean the task failed to start - in fact,
    ** it probably doesn't.  There is a (cache-related?) bug which
    ** causes changes to variables made in one task to not appear
    ** to tight-check loops on other tasks, which results in hangs.
    ** In this case, squawking is plenty. */
    QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                            "QTV_Task %s init timed out waiting for startup",
                            m_taskName );
    return false;
  }
}
/*lint +e1541 */

/* ======================================================================
FUNCTION:
  ~QTV_Task

DESCRIPTION:
  Stops the task.  This call is synchronous - it will not return
  until the task has terminated.

PARAMETERS:

RETURN VALUE:

===========================================================================*/
QTV_Task::~QTV_Task( void )
{
  unsigned long sleepTotal( 0 );

  if ( m_state == RUNNING )
  {
#ifndef PLATFORM_LTK
    (void)rex_set_sigs( &m_tcb, QTV_TASK_EXIT_SIG );

    // See the comment in the constructor about polling.

    while ( m_state == RUNNING )
    {
      sleepTotal += POLL_SLEEP_MS;
      rex_sleep( POLL_SLEEP_MS );
    }
#endif
    // At this point, the task is waiting for death.  Shuffle it off
    // this mortal coil and release its resources.  We do it the harsh way
    // (rex_kill_task) because any mechanism by which the task can signal
    // its own completion brings with it the risk that it will still be
    // using its stack when we free it, and that's scary-dangerous.
    //
    // Imagine a task whose last statement signals completion.  There
    // will always be a case where the task can be swapped out after
    // the signal is set but before the function exits.  If the stack
    // is destroyed, then we're doomed.

    QTV_MSG_SPRINTF_PRIO_2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                            "QTV_Task %s terminating. %d ms spent waiting for shutdown",
                            m_tcb.task_name, sleepTotal );

    rex_kill_task( &m_tcb );
  }
  else
  {
    QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                            "QTV_Task destruction, but task not running: %d",
                            m_state );
    rex_kill_task( &m_tcb );
  }
}

/* ======================================================================
FUNCTION:
  QTV_Task::SetSignal

DESCRIPTION:
  Sends the given signals to the task.

PARAMETERS:
  const rex_sigs_type sigs
    The signals to send.

RETURN VALUE:
  Signals mask after setting, note, that if the set causes a higher priority
  task to run, and it clears the sigs, the mask returned may indicate
  that the signal was never set (meaning, it was set and processed)
===========================================================================*/
rex_sigs_type QTV_Task::SetSignal( const rex_sigs_type sigs )
{
  if ( m_state == RUNNING )
  {
    return rex_set_sigs( &m_tcb, sigs );
  }

  return 0;
}

/* ======================================================================
FUNCTION:
  GetAvailableSignal

DESCRIPTION:
  Returns a signal which the task believes to be available for use
  by clients.  Of course, QTV_Task can't keep track of signals you allocate
  outside of the QTV_Task infrastructure, so this is only meaningful
  if you get all your signals via this method.

PARAMETERS:
  None.

RETURN VALUE:
  A signal which can be safely used without tripping over signals used
  by the QTV_Task infrastructure.  0 if there are no such signals
  available.
===========================================================================*/
rex_sigs_type QTV_Task::GetAvailableSignal( void )
{
  static const rex_sigs_type MAX_SIG = 0x00008000;
    /* The top word is reserved for the REX system. */

  rex_sigs_type sig = QTV_TASK_EXIT_SIG << 1;
    /* We know we'll never be able to provide TASK_EXIT_SIG, so there's
    ** no point in even testing it. */

  while ( sig <= MAX_SIG )
  {
    if (( sig & m_sigs_in_use ) == 0 )
    {
      m_sigs_in_use |= sig;
      return sig;
    }

    sig <<= 1;
  }

  return 0;
}

/* ======================================================================
FUNCTION:
  TaskHarness

DESCRIPTION:
  The task harness which runs the user-supplied task.

PARAMETERS:
  unsigned long param
    The single param allowed by rex.  It's a this pointer in disguise.

RETURN VALUE:
  None.
===========================================================================*/
void QTV_Task::TaskHarness( unsigned long param )
{
  QTV_Task* pThis( ( QTV_Task* )param );

  pThis->m_state = RUNNING;

  ( pThis->m_taskFn )( pThis->m_userData );

  pThis->m_state = DESTRUCTING;

 (void) rex_wait( 0 ); // wait patiently for death from the dtor.
}
