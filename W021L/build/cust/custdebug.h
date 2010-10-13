#ifndef CUSTDEBUG_H
#define CUSTDEBUG_H
/*===========================================================================

DESCRIPTION
  Configuration for debug feature using MSM6280

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custdebug.h#3 $ $DateTime: 2009/04/10 12:54:13 $ $Author: ehong $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/07   mp      removing L4/DM
05/30/07   ts      Added FEATURE_QPMD for L4 core dump support.
06/02/06   djd     Initial MSM6800 Version. 
===========================================================================*/

#define FEATURE_LOOPING_EXCEPTION_HANDLER

/* This feature displays on LCD and saves in NV which task caused dog fatal error,
** especially for FFA builds. Will be removed when ERR_FATAL macro has the same 
** capability via DIAG enhancement.
*/
#define FEATURE_ID_FATAL_TASK

/* Store core dump information to EFS
*/
/* Features will be defined in qsc1100.builds */

#ifdef BOOT_MODE_NAND
   #define FEATURE_QPMD
#endif

/* Needed to enable F3 trace by default */
// #define FEATURE_SAVE_TRACE_ON_BY_DEFAULT 

#define FEATURE_SAVE_TRACE_EXTENDED
#define FEATURE_OPTIMIZE_TRACE_RECORDS

/*LCD does not use brew heap for displaying error msg*/
#define FEATURE_ERROR_MESSAGE_STANDALONE

/*Check to see whether crash debugging feature is allowed for this target */
#define FEATURE_CRASH_DEBUG_DISABLED_CHECK

#endif /* CUSTDEBUG_H */

