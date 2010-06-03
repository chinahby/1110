#ifndef CUSTREX_H
#define CUSTREX_H
/*===========================================================================

DESCRIPTION
  Configuration for REX.

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custrex.h#1 $ $DateTime: 2008/03/06 14:38:01 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/02   cah     Added GSM features.
05/01/02   jct     Created
===========================================================================*/

/* Pre-emptive interrupts
*/
#define FEATURE_REX_PREEMPT_INTS

/* Use a larger FIQ stack for more processing in the FIQ interrupt
*/
#define FEATURE_REX_LARGE_FIQ_STACK

/* Turn on ability to obtain CPU utilization information for each
** task via the UI debug menu
*/
#undef FEATURE_SOFTWARE_PROFILE

/* Turn on run time task utilization information
*/
#define FEATURE_REX_PROFILE

/* Turn on ability to do core dumps
*/
#define FEATURE_REX_CORE_DUMP

/* Turn on DIAG interface to REX
*/
#define FEATURE_REX_DIAG

/* Turn on ability to create tasks with dynamically allocated TCB's
** and stacks
*/
#undef FEATURE_REX_CREATE_TASK

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifndef BUILD_BOOT_CHAIN
#define FEATURE_REX_IPC
#endif
/* Auto-kick the dog while a task is busy to reduce dog timeouts
*/
#define FEATURE_ENHANCED_REX_TASK

#ifdef FEATURE_L4
#error code not present
#endif

/* Turn on qmutex support
*/
#define FEATURE_QMUTEX

#endif /* CUSTREX_H */

