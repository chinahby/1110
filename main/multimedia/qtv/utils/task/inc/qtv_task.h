#ifndef QTV_TASK_H
#define QTV_TASK_H
/* =======================================================================

                          qtv_task.h

DESCRIPTION
  Definition of QTV_Task class, which provides...tasks!

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/task/main/latest/inc/qtv_task.h#2 $
$DateTime: 2009/01/29 06:14:06 $
$Change: 829425 $

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
#include "rex.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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

class QTV_Task
{
public:
  // These two definitions are critical to users of QTV_Task. Users
  // of this class (and implementers of task functions) have two 
  // responsibilities:
  //
  // The task function which will be run by this QTV_Task object must
  // be of the defined TASK type.  
  //
  // The task function must exit when a QTV_TASK_EXIT_SIG signal is received.
  //
  // And that's it!  Create a QTV_Task, and your supplied task function
  // will run under control of the QTV_Task.  Destroy the object, the task
  // is torn down!  Easy.
  
  typedef void ( *TASK )( void* userData );
  
  /* ======================================================================
  FUNCTION:
    QTV_Task

  DESCRIPTION:
    Constructor.  Starts the task.
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

    const bool                initOnConstruction
      If true, task is started when object is constructed.
      Otherwise, task startup is deferred until Init is called.
  RETURN VALUE:
    None.
  ===========================================================================*/
  QTV_Task
  (
    const rex_priority_type   priority,
    const char * const        taskName,
    const int                 dogReportVal,
    TASK                      taskFn,
    void*                     userData,
    const bool                initOnConstruction = true
  );

  /* ======================================================================
  FUNCTION:
    Init

  DESCRIPTION:
    Starts the task.  Automatically called from constructor by default,
    but can be deferred for those cases when starting the task on construction
    is unwise (think static globals)

  PARAMETERS:
    None.

  RETURN VALUE:
    bool.
      'true' if we could verify that everything started successfully.
      'false' otherwise - we probably still started OK but couldn't verify it.
  ===========================================================================*/
  bool Init( void );

  /* ======================================================================
  FUNCTION:
    ~QTV_Task

  DESCRIPTION:
    Destructor.  Stops the task.
    This call is synchronous - it will not return
    until the renderer task has terminated.

  PARAMETERS:
    None.

  RETURN VALUE:
    None.
  ===========================================================================*/
  virtual ~QTV_Task( void );

  /* ======================================================================
  FUNCTION:
    SetSignal

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
  rex_sigs_type SetSignal( const rex_sigs_type sigs );

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
  rex_sigs_type GetAvailableSignal( void );

protected:
  static const rex_sigs_type QTV_TASK_EXIT_SIG;

  enum State
  {
    CONSTRUCTING,
    RUNNING,
    DESTRUCTING,
#ifndef FEATURE_WINCE
    ERROR
#else
#error code not present
#endif

  };

  State m_state;

  /* 
  ** Subclasses need to know about our TCB sometimes (usually for setting
  ** up timers and the like ) 
  */
  rex_tcb_type* GetTCB( void ) { return &m_tcb; }

private:
  /* Intra-class constants */
  enum
  {
    POLL_SLEEP_MS   = 10,
    POLL_TIMEOUT_MS = 100,
    STACK_SIZE      = (8*1024)
  };

  rex_sigs_type m_sigs_in_use;

  rex_tcb_type  m_tcb;
  unsigned char m_stack[ STACK_SIZE ];

  TASK                    m_taskFn;
  void*                   m_userData;
  const rex_priority_type m_priority;
  char                    m_taskName[ REX_TASK_NAME_LEN + 1 ];
  const int               m_dogReportVal;
  
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
  static void TaskHarness( unsigned long param );
};

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#endif // QTV_TASK_H

