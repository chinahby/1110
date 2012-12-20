/* =======================================================================
                               qcutilszrex.cpp
DESCRIPTION
//  Description: Some miscellaneous low-level utilities
//    This is the zrex implementation

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/src/qcutilszrex.cpp#4 $
$DateTime: 2009/12/03 02:35:48 $
$Change: 1101863 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "QCUtils.h"
#include "assert.h"

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
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  QCUtils::SetThreadName

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::SetThreadName(QCThreadControl * pTC,Common::ThreadNameType name)
{
  switch (name)
  {
  case Common::PLAYER_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Player",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::AUDIO_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Audio",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::VIDEO_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Video",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::DISPLAY_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Display",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::STREAMER_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Streamer",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::DOWNLOAD_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Download",COMMON_MAX_ZREX_THREADNAME);
    break;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  case Common::CPVTEXT_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV CPV Text",COMMON_MAX_ZREX_THREADNAME);
    break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  case Common::PARSER_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Parser",COMMON_MAX_ZREX_THREADNAME);
    break;
  case Common::RENDERER_THREAD_NAME:
    ZUtils::StrcpyN(pTC->name,"PV Renderer",COMMON_MAX_ZREX_THREADNAME);
    break;

#ifdef FEATURE_QTV_DRM_DCF
    //QtvPlayer thread name
    case Common::QTV_UTIL_THREAD_NAME:
      ZUtils::StrcpyN(pTC->name,"QTV UTIL",COMMON_MAX_ZREX_THREADNAME);
      break;
#endif

#ifdef FEATURE_QTV_STREAM_RECORD
    case Common::RECORDER_THREAD_NAME:
      ZUtils::StrcpyN(pTC->name,"QTV Recorder",COMMON_MAX_ZREX_THREADNAME);
      break;
#endif 

  default:
    ZUtils::StrcpyN(pTC->name,"PV ????",COMMON_MAX_ZREX_THREADNAME);
    break;
  }
}

/* ======================================================================
FUNCTION
  QCUtils::CreateThread

DESCRIPTION
//
//  Create a thread
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool QCUtils::CreateThread(QCThreadControl *pTC, unsigned long inputParam, bool bStartSuspended)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "CreateThread");
  if (pTC)
  {
    pTC->threadHandle = zrex_create_thread(
      pTC->stackSize,
      pTC->priority,
      pTC->pThreadFunction,
      inputParam, //input to the thread function
      pTC->name, //thread name
      bStartSuspended);
    if (pTC->threadHandle != NULL)
    {
      return true;
    }
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Mpeg4Player::CreateThread failed");
  return false;
}

/* ======================================================================
FUNCTION
  QCUtils::ResumeThread

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::ResumeThread(QCThreadControl *pTC)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "In Mpeg4Player::ResumeThread");

  if (pTC)
  {
    if (pTC->threadHandle)
    {
      zrex_resume_thread(pTC->threadHandle);
    }
  }
}

/* ======================================================================
FUNCTION
  QCUtils::PauseThread

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::PauseThread(QCThreadControl *pTC)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "In Mpeg4Player::PauseThread");

  if (pTC)
  {
    if (pTC->threadHandle)
    {
      zrex_pause_thread(pTC->threadHandle);
    }
  }
}

/* ======================================================================
FUNCTION
  QCUtils::KillThread

DESCRIPTION
//
// kill a thread if it's active, and clear the handle.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::KillThread(QCThreadControl *pTC)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "In Mpeg4Player::KillThread");
  if (pTC)
  {
    if (pTC->threadHandle != NULL)
    {
      zrex_kill_thread(pTC->threadHandle);
      pTC->threadHandle = NULL;
    }
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::ReleaseEventQ

DESCRIPTION
//
// Release all the events from the threads event-q.
//

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void QCUtils::ReleaseEventQ(QCThreadControl *pTC)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "In QCUtils::DeleteEventQ");
  if (pTC)
  {
    if (pTC->threadHandle != NULL)
    {
      zrex_release_event_q(pTC->threadHandle);
    }
  }
}

/* ======================================================================
FUNCTION
  QCUtils::IsThreadActive

DESCRIPTION
//tell if a thread is active

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
bool QCUtils::IsThreadActive(QCThreadControl * pTC)
{
  if (pTC)
  {
    return (pTC->threadHandle != NULL);
  }
  return false;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::CreateCondition

DESCRIPTION
//
//  create a sync event.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::CreateCondition(QCConditionType * p, bool bManualReset, bool bInitialState)
{
  zrex_init_sync_event(p, bManualReset, bInitialState);
}

/* ======================================================================
FUNCTION
  QCUtils::DestroyCondition

DESCRIPTION
//
//  destroys a sync event, freeing up REX signals for the creation of new event syncs
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::DestroyCondition(QCConditionType * p)
{
  zrex_release_sync_event( p );
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::WaitForCondition

DESCRIPTION
//
//  wait on a sync event to be signalled.  return TRUE if signalled as opposed to
//  timed out.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
bool QCUtils::WaitForCondition(QCConditionType * p, const unsigned long delay_msec)
{
  //if you tell zrex to wait for zero, it waits forever!
  ASSERT( delay_msec > 0 );
  return (zrex_wait_for_sync_event(p, delay_msec) == ZREX_SYNC_EVENT_SET);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::SetCondition

DESCRIPTION
//
//  set a sync event to the signalled state.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::SetCondition(QCConditionType * p)
{
  zrex_set_sync_event(p);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::ResetCondition

DESCRIPTION
//
//  set a sync event to the non-signalled state.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::ResetCondition(QCConditionType * p)
{
  zrex_reset_sync_event(p);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::sleep

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::sleep(int nMsec)
{
  //zrex sleeps forever if zero is input, so prevent
  //it here.
  if (nMsec>0)
  {
    zrex_sleep(nMsec);
  }
}

/* ======================================================================
FUNCTION
  QCUtils::InitCritSect

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::InitCritSect(QCCritSectType *p)
{
  rex_init_crit_sect(p);
}

/* ======================================================================
FUNCTION
  QCUtils::DinitCritSect

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::DinitCritSect(QCCritSectType *p)
{

/* ------
Commented out, since this code goes to all targets as common and 
below REX API is not currently ported to all active targets and baselines.
--------- */

  //rex_del_crit_sect(p);
}

/* ======================================================================
FUNCTION
  QCUtils::EnterCritSect

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::EnterCritSect(QCCritSectType *p)
{
  rex_enter_crit_sect(p);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::LeaveCritSect

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::LeaveCritSect(QCCritSectType *p)
{
  rex_leave_crit_sect(p);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
	
/* ======================================================================
FUNCTION
  QCUtils::PostMessage

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::PostMessage(
  QCMessageType * pEvent,
  int,
  QCMessageReceive *)
{
  zrex_post_event(pEvent);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
	
/* ======================================================================
FUNCTION
  QCUtils::SetMessageDispatchTable

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::SetMessageDispatchTable(
  QCMessageSinkType table_ptr[],
  QCMessageReceive *)
{
  zrex_set_event_dispatch_table(table_ptr);
}
/* ======================================================================
FUNCTION
  QCUtils::GetNextMessage

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QCMessageType * QCUtils::GetNextMessage( QCMessageReceive * )
{
  return zrex_get_next_event();
}

/* ======================================================================
FUNCTION
  QCUtils::DispatchMessage

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::DispatchMessage(QCMessageType *event_ptr, QCMessageReceive * )
{
  zrex_dispatch_event(event_ptr);
}

/* ======================================================================
FUNCTION
  QCUtils::ReleaseMessage

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QCUtils::ReleaseMessage(QCMessageType *event_ptr, QCMessageReceive * )
{
  zrex_event_release(event_ptr);
}

/* ======================================================================
FUNCTION
  QCThreadControl::QCThreadControl

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QCThreadControl::QCThreadControl()
{
  stackSize = NULL;
  threadHandle = NULL;
  pThreadFunction = NULL;
  priority = NULL;
  for(int i=0;i<COMMON_MAX_ZREX_THREADNAME;i++)
  {
    name[i]='0';
  }

}

/* ======================================================================
FUNCTION
  QCThreadControl::~QCThreadControl

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
QCThreadControl::~QCThreadControl()
{
  if (threadHandle)
  {
    zrex_kill_thread(threadHandle);
    threadHandle = NULL;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::Terminate

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void QCUtils::Terminate()
{
  /* clear all pending(if any) sync events */
  zrex_release_all_sync_event();
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* ======================================================================
FUNCTION
  QCUtils::Init

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool QCUtils::Init()
{
  // Don't have to do anything for zrex
  // zrex2rex_init();
  return true;
}

