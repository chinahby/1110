/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 T A S K    R E S O U R C E    F I L E

GENERAL DESCRIPTION
  This file contains declarations for task resources, and the task_start
  function.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1990, 1991, 1992 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1998, 1999 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/services/tmc/vcs/task_iram.c_v   1.0   13 May 2002 15:52:44   vtawker  $
$MSM6200: //depot/asic/msm6200/services/task/task_iram.c#1 DateTime: 2002/05/13 15:52:44 Author: vtawker $
$Header: //depot/asic/MSMSHARED/services/task/task_iram.c#4 $ $DateTime: 2005/11/11 10:27:46 $ $Author: pratapc $
   
when        who     what, where, why
--------    ---     ----------------------------------------------------------
03/17/03    jqi     Initial merged from MSM6200 archive.
                    

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "rex.h"
#include "task.h"
#include "tmc.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_WCDMA 
#error code not present
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_PS
/*---------------------------------------------------------------------------

                     PACKET DATA PROTOCOL STACK DECLARATIONS

---------------------------------------------------------------------------*/
rex_stack_word_type ps_stack [PS_STACK_SIZ]; /* Stack for DATA Pro. Task.  */
#endif /* FEATURE_DATA_PS */
#endif /* FEATURE_DATA */

#if defined FEATURE_USB
/*---------------------------------------------------------------------------

                     USBDC STACK DECLARATIONS

---------------------------------------------------------------------------*/

rex_stack_word_type usbdc_stack [USBDC_STACK_SIZ];  /* Stack for USB task.  */
#endif /* FEATURE_USB */

