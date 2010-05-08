/*===========================================================================

                   T I M E T E S T    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6500/services/utils/timetest.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/01   om      Added support for nested interrupts.
07/16/01   om      Fixed includes.
03/08/01   om      Created module.

===========================================================================*/
/*** This file is empty if timetest is disabled ***/
#include "customer.h"

#ifdef TIMETEST
#include "target.h"
#include "memory.h"
#include "msg.h"

#include "timetest.h"

#ifdef TIMETEST_PROF


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
unsigned long timetest_parse_token = TT_PARSE_TOKEN;

unsigned char timetest_isr_markers[32];
unsigned char timetest_isr_level = 0;

#ifdef TIMETEST_FUNCS
/* Array to store markers during context switches */
unsigned long timetest_markers[TIMETEST_MAX_TASK_ID][TIMETEST_MAX_FUNC_LEVEL+1];
#endif


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

void timetest_prof_init( void )
{
#ifdef TIMETEST_FUNCS
  memset( timetest_markers, 
      0, 
      TIMETEST_MAX_TASK_ID * TIMETEST_MAX_FUNC_LEVEL+1 );
#endif

  MSG_ERROR( "TIMETEST initialized.", 0, 0, 0 );
}

#endif  /* TIMETEST_PROF */

#else  /* TIMETEST */
/* Function to suppress compiler warning */
void timetest_foo(void);
#endif
