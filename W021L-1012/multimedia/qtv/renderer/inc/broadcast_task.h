#ifndef _BROADCAST_TASK_H_
#define _BROADCAST_TASK_H_
/* =======================================================================

                           broadcast_task.h

DESCRIPTION
  Definition of QTV_BroadcastTransmitterTask.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright 2005 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/renderer/main/latest/inc/broadcast_task.h#1 $
$DateTime: 2008/05/09 08:14:38 $
$Change: 657007 $

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
#include "broadcast.h"
#include "qtv_task.h"
#include "qtv_utils.h"
#include "qcplayer.h"

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

//===========================================================================
//                    QTV_BroadcastTransmitterTask
// 
// The QTV_BroadcastTransmitterTask is a standalone task that supports
// the QTV_BroadcastTransmitter API. Broadcasts sent to this transmitter
// will be queued to the task for asynchronous transmission.

class QTV_BroadcastTransmitterTask : public QTV_Task,
                                     public QTV_BroadcastTransmitter 
{
public:

  QTV_BroadcastTransmitterTask( const rex_priority_type   priority,
                                const char * const        taskName,
                                const int                 dogReportVal,
                                const char*               szTransmitterName );
  virtual ~QTV_BroadcastTransmitterTask( void );

  virtual void Transmit( QTV_Broadcast* &pMessage );
  
private:
  struct BcastNode
  {
    q_link_type    link;
    QTV_Broadcast* pBcast;
  };
  
  static const rex_sigs_type SIG_BCAST_PENDING;

  QTV_LockableQueue m_bcastQ;
  bool m_bIsAcceptingTransmissions;
  
  // QTV_Task wants a static (non-member) function, so we'll give it a 
  // little stub which converts userData to this and then calls our
  // actual member function so we don't need to write a function full
  // of pThis-> garbage.
  
  static void TaskStub( void* userData );
  void Task( void );

  // A little helper function to clean up the node-reading loop in our task.

  BcastNode* GetNextBroadcast( void );
};

#endif // _BROADCAST_TASK_H_


