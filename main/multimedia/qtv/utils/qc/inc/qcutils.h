#ifndef _QCUTILS_H_
#define _QCUTILS_H_
/* =======================================================================
                               qcutils.h
DESCRIPTION
  Utilities include file               
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/inc/qcutils.h#3 $
$DateTime: 2009/12/03 02:35:48 $
$Change: 1101863 $


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
//#include "rex.h"
#include "qcplayer.h"

// Include the definitions for
//    type  QCEventType
//    type  QCEventHandleType
//    type  QCEventSinkType
//    struct  QCEventReceive
//    macro QCCreateEvent
//    type  QCSyncEventType
//    type  QCCritSectType
//    struct  QCThreadControl 
//    type  QCThreadFuncType
//    macro TRY
//    macro TIMED_TRY
//    macro TRY_END
//    macro CATCH
//    macro CATCH_ALL
//    macro CATCH_END
//    macro THROW
//    macro EVENT
//    macro EVENT_END
//    macro EVENT_TABLE
//    macro EVENT_SINK
//    macro EVENT_TABLE_END
//    macro EVENT_POOL
//    macro EVENT_SOURCE

#include "QCUtilsZrex.h"

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
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  QCUtils

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
class QCUtils 
{

public:

  /////////////////////////////////////////
  // Condition Variables
  /////////////////////////////////////////

  static void CreateCondition(QCConditionType * p, bool bManualReset, bool bInitialState);
  static void DestroyCondition(QCConditionType * p);

  static bool WaitForCondition(QCConditionType * p, const unsigned long delay_msec);

  static void SetCondition(QCConditionType * p);
  static void ResetCondition(QCConditionType * p) ;

  /////////////////////////////////////////
  // Critical Sections
  /////////////////////////////////////////

  static void InitCritSect(QCCritSectType *p);
  static void DinitCritSect(QCCritSectType *p);
  static void EnterCritSect(QCCritSectType *p);
  static void LeaveCritSect(QCCritSectType *p);

  /////////////////////////////////////////
  // Threads
  /////////////////////////////////////////

  static bool CreateThread
    (QCThreadControl * pTC, unsigned long inputParam, bool bStartSuspended) ;

  static void ResumeThread(QCThreadControl * pTC) ;
  static void PauseThread(QCThreadControl * pTC) ;
  static void KillThread(QCThreadControl * pTC);
  static bool IsThreadActive(QCThreadControl * pTC);
  static void SetThreadName(QCThreadControl * pTC,Common::ThreadNameType name);

  static void ReleaseEventQ(QCThreadControl *pTC);

  /////////////////////////////////////////
  // Message Manager
  /////////////////////////////////////////

  static void PostMessage
    (QCMessageType * pEvent, int urgentpri, QCMessageReceive * pMC);

  static void SetMessageDispatchTable
    (QCMessageSinkType table_ptr[],QCMessageReceive * pMC);

  static QCMessageType * GetNextMessage( QCMessageReceive * pMC);

  static void DispatchMessage
    (QCMessageType *event_ptr, QCMessageReceive * pMC );

  static void ReleaseMessage
    (QCMessageType *event_ptr, QCMessageReceive * pMC );

  //Note: To create events, use QCCreateMessage macro.

  /////////////////////////////////////////
  // Sleep
  /////////////////////////////////////////

  //note: this is lowcase because "Sleep" is redefined 
  //in the OSCL header files.
  static void sleep(int nMsec);

  /////////////////////////////////////////
  // Init & Cleanup
  /////////////////////////////////////////
  static bool Init();
  static void Terminate();


private:


};  

#endif //_QCUTILS_H_
