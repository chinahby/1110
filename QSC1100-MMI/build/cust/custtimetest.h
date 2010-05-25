#ifndef CUSTTIMETEST_H
#define CUSTTIMETEST_H
/*===========================================================================

DESCRIPTION
  Configuration for timetset feature using MSM6800

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custtimetest.h#1 $ $DateTime: 2008/03/06 14:38:01 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/06   djd     Initial MSM6800 Version. 
===========================================================================*/

/* Defines for enabling profiling using TIMETEST port */
#if !defined(BUILD_BOOT_CHAIN)
  #define TIMETEST
  #define TIMETEST_INV_TRIGGER
#endif
#endif /* CUSTTIMETEST_H */

