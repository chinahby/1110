/* =======================================================================

                          broadcast_task.cpp

DESCRIPTION
  Implementation of the base QTV_BroadcastTransmitterTask class.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/src/broadcast_task.cpp#6 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "broadcast_task.h"
#include "assert.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifdef QTV_BROADCAST_VERBOSE_DIAG
#define DIAG_BUFSIZE (48)
#endif /* QTV_BROADCAST_VERBOSE_DIAG */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
const rex_sigs_type 
  QTV_BroadcastTransmitterTask::SIG_BCAST_PENDING( 0x0002 );

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
  QTV_BroadcastTransmitterTask::QTV_BroadcastTransmitterTask
  
DESCRIPTION:
  Constructor.
  
PARAMETERS:
  const rex_priority_type   priority
    The priority at which the task will run.

  const char * const        taskName,
    The task's name - useful for debugging.

  const int                 dogReportVal
    The task ID used to report to the rex watchdog task.

  const char* szName
    Name of the transmitter.  Optional.  If a name is provided, 
    receivers will be able to locate this transmitter with the static
    Locate function.  If NULL...well, they won't.
  
RETURN VALUE:
  None.
  
===========================================================================*/
QTV_BroadcastTransmitterTask::QTV_BroadcastTransmitterTask
(
  const rex_priority_type   priority,
  const char * const        taskName,
  const int                 dogReportVal,
  const char*               szTransmitterName
)
: QTV_Task( priority,
            taskName,
            dogReportVal,
            TaskStub,
            this ),
  QTV_BroadcastTransmitter( szTransmitterName ),
  m_bIsAcceptingTransmissions( m_state == RUNNING )
{
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitterTask::~QTV_BroadcastTransmitterTask
  
DESCRIPTION:
  Destructor.
  
PARAMETERS:
  None.
    
RETURN VALUE:
  None.
  
===========================================================================*/
QTV_BroadcastTransmitterTask::~QTV_BroadcastTransmitterTask( void )
{
  m_bIsAcceptingTransmissions = false;
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitterTask::~Transmit
  
DESCRIPTION:
  Accepts broadcasts for transmission.  Actually queues them and signals
  our task to transmit them later.
  
PARAMETERS:
  QTV_Broadcast* &pMessage.  Pointer to the message to transmit.
    
RETURN VALUE:
  None.
  
===========================================================================*/
void QTV_BroadcastTransmitterTask::Transmit( QTV_Broadcast* &pMessage )
{
  bool bFailure             ( true );
  bool bAreListenersWaiting ( false );

  if ( pMessage == 0 )
  {
    return;
  }

  if ( m_bIsAcceptingTransmissions )
  {
    // Check the registry of receivers.  If nobody's listening for this
    // message type, avoid waking the broadcast task.
    //
    QCUtils::EnterCritSect( &m_sinkAccessLock );

    bAreListenersWaiting = 
      ( q_cnt( &m_aInstalledSinks[ pMessage->m_type ] ) > 0 );

    QCUtils::LeaveCritSect( &m_sinkAccessLock );

    if ( bAreListenersWaiting )
    {
      BcastNode* pNode( QTV_New( BcastNode ) );
      
      if ( pNode )
      {
        q_type* pQ = m_bcastQ.Lock();
  
        if ( pQ )
        {
          (void)q_link( pNode, &( pNode->link ) );
          pNode->pBcast = pMessage;
          q_put( pQ, &( pNode->link ) );
          m_bcastQ.Unlock( pQ );
  
          (void)SetSignal( SIG_BCAST_PENDING );
  
          bFailure = false;
        }
        else
        {
          QTV_Delete( pNode );
        }
      }
    } // bAreListenersWaiting?
    else
    {
      // Nobody's listening for this, so just nuke it.  Nobody will miss it.
      //
      bFailure = false;
      QTV_Delete( pMessage );
    }// !bAreListenersWaiting?
  } // m_bIsAcceptingTransmissions?

  if ( bFailure )
  {
    #ifdef QTV_BROADCAST_VERBOSE_DIAG

    char diagBuf[ DIAG_BUFSIZE ];
    
    // Failure means the message wasn't sent for transmit, but the 
    // sender doesn't know that.  Clean up the broadcast now to avoid leaks.
    (void)pMessage->Describe( diagBuf, DIAG_BUFSIZE ); /*lint!e449 */
    QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                            "Transmit failure: %s", diagBuf );

    #endif /* QTV_BROADCAST_VERBOSE_DIAG */

    QTV_Delete( pMessage );
  }

  // In all cases, the message has been moved out of the caller's control.  
  // Null the caller's pointer to prevent shenanigans.
  pMessage = 0;
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitterTask::Task

DESCRIPTION:
  The task.

PARAMETERS:
  The stub has one:
    void* userData
      The single param allowed by QTV_Task.  It is actually a this*
      in disguise.

RETURN VALUE:
  None.
===========================================================================*/
void QTV_BroadcastTransmitterTask::TaskStub( void* userData )
{
  QTV_BroadcastTransmitterTask* 
    pThis( ( QTV_BroadcastTransmitterTask* )userData );

  ASSERT( pThis );
  pThis->Task();
}

void QTV_BroadcastTransmitterTask::Task( void )
{
  const rex_sigs_type ALL_SIGS =
    QTV_TASK_EXIT_SIG | // Remember, we're using QTV_Task!
    SIG_BCAST_PENDING;
    
  rex_sigs_type sigs ( 0 );
  BcastNode*    pNode( 0 );

  #ifdef QTV_BROADCAST_VERBOSE_DIAG

  char          diagBuf[ DIAG_BUFSIZE ];

  #endif /* QTV_BROADCAST_VERBOSE_DIAG */

  //--- Startup ---

  QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW,
                 "QTV_BroadcastTransmitterTask task started %x", this );

  //--- Main processing loop ---

  for( ;; ) // forever
  {
    sigs = rex_wait( ALL_SIGS );
    (void)rex_clr_sigs( rex_self(), sigs );

    if ( ( sigs & QTV_TASK_EXIT_SIG ) != 0 )
    {
      // Shutdown! Clean up and get out.

      break;  
    }
      
    if ( ( sigs & SIG_BCAST_PENDING ) != 0 )
    {
      for
      (
        pNode  = GetNextBroadcast();
        pNode != 0;
        pNode  = GetNextBroadcast()
      )
      {
        // null pBcast just shouldn't happen, but safety don't hurt.
        if ( pNode->pBcast )
        {
          #ifdef QTV_BROADCAST_VERBOSE_DIAG

          (void)pNode->pBcast->Describe( diagBuf, DIAG_BUFSIZE );
          if ( m_pThisInRegistry )
          {
            QTV_MSG_SPRINTF_PRIO_2( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                                    "%s xmit: %s",
                                    m_pThisInRegistry->name, 
                                    diagBuf);
          }
          else
          {
            QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                                    "anonymous xmit: %s",
                                    diagBuf);
          }

          #endif /* QTV_BROADCAST_VERBOSE_DIAG */

          QTV_BroadcastTransmitter::Transmit( pNode->pBcast );
        } // if ( pNode->pBcast )
        QTV_Delete( pNode );
      } // for ( get every queued broadcast )
    } // if SIG_CMD_PENDING
  } // forever 
}

/* ======================================================================
FUNCTION:
  QTV_BroadcastTransmitterTask::GetNextBroadcast

DESCRIPTION:
  A helper function to pull the next pending broadcast from the queue.

PARAMETERS:
  None.
  
RETURN VALUE:
  BcastNode* 
    The next broadcast, or NULL if we're all out.
===========================================================================*/

QTV_BroadcastTransmitterTask::BcastNode* 
QTV_BroadcastTransmitterTask::GetNextBroadcast( void )
{
  BcastNode* pNode( 0 );
  q_type*    pQ   ( 0 );

  pQ= m_bcastQ.Lock();

  if ( pQ )
  {
    pNode = ( BcastNode* )q_get( pQ );
    m_bcastQ.Unlock( pQ );
  }

  return pNode;
}

