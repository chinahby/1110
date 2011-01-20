/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          M S M 6 0 5 0   D E V I C E   I N I T I A L I Z A T I O N

GENERAL DESCRIPTION
  This module has extra initialization for the MSM6050

EXTERNALIZED FUNCTIONS
  MSM_device_init()
        It has extra initialization for the MSM6050 that was not done in
        the boot code or on the different modules init code.

Copyright (c) 1998,1999,2000,2001 by QUALCOMM Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/msm6050_init.c_v   1.2   06 Nov 2001 09:55:50   dbrackma  $
$Header: //depot/asic/qsc1100/drivers/hw/msm_init.c#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/05   jrp     Changed LED macro name for one that points to virtual memory.
08/19/04   ap      Deleted dependency for DFM.
09/21/01   dsb     Created from msm510x_init.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Definition for target specific defs */
#include "msm.h"


/*===========================================================================

  FUNCTION msm_device_init()

  DESCRIPTION
    Initialize MSM to default operating mode.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
===========================================================================*/

void msm_device_init(void)
{
  HWIO_OUT(HAPPY_LED, 0);
}
