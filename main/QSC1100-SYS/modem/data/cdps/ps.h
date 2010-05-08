#ifndef PS_H
#define PS_H
/*===========================================================================

                                P S . H

DESCRIPTION
  This is the header file for the Data protocol Task. Contained herein are
  the functions needed to initialize all the modules that execute in PS task 
  context and the main task processing loop.
  
EXTERNALIZED FUNCTIONS
  ps_task()
    Data Protocol Task entry point and main processing loop.  

 Copyright (c) 1993-1996 by QUALCOMM Incorporated.  All Rights Reserved.
 Copyright (c) 1998-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps.h_v   1.0   08 Aug 2002 11:19:56   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/05/02    usb    Externalized ps_dog_rpt_timer since its used in psi.c
03/14/02    usb    Initial version of file


===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA_PS

 
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Watchdog timer for PS task
---------------------------------------------------------------------------*/
extern rex_timer_type ps_dog_rpt_timer;          /* for watchdog reporting */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PS_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the Data
  Service(s) task.

DEPENDENCIES
  None.

RETURN VALUE
  Does not Return.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ps_task
(
  uint32 dummy      
);

#endif /* FEATURE_DATA_PS */

#endif /* PS_H */


