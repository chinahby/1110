#ifndef _QCUTILSZREX_H_
#define _QCUTILSZREX_H_
/* =======================================================================
                               qcutilszrex.h
DESCRIPTION
  Constants, types, and macros for QCUtils class.
  This is the zREX version
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/inc/qcutilszrex.h#1 $
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
#include "qcplayer.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//this is a player limit, the OS allows more. 
#define COMMON_MAX_ZREX_THREADNAME 20

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
//types
typedef zrex_event_type QCMessageType;
typedef zrex_handle_type QCMessageHandleType;
typedef zrex_event_sink_type QCMessageSinkType;

//not really used for zrex
//but the compiler needs something in the structure.
struct QCMessageReceive
{ 
  bool b;
}; 

//macros
#define QCCreateMessage zrex_create_event

#define QCCreateStaticMessage zrex_create_static_event

/////////////////////////////////////////
// Thread Sync Events
/////////////////////////////////////////

typedef zrex_sync_event_type QCConditionType;

/////////////////////////////////////////
// Critical Sections
/////////////////////////////////////////

typedef rex_crit_sect_type QCCritSectType;

//define the thread func type.
typedef rex_task_func_type QCThreadFuncType;

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
  QCThreadControl

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
class QCThreadControl 
{
public:
  QCThreadControl();
  ~QCThreadControl();
  zrex_handle_type    threadHandle;
  rex_task_func_type    pThreadFunction;
  zrex_stack_size_type  stackSize;
  zrex_priority_type    priority;
  char          name[COMMON_MAX_ZREX_THREADNAME];
} ;

#endif //_QCUTILS_H_
