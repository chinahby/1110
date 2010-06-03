#ifndef MSM_INIT_H
#define MSM_INIT_H
/*===========================================================================

 M S M 5 1 0 5   D E V I C E   I N I T I A L I Z A T I O N   H E A D E R   F I L E

DESCRIPTION
  This header file contains the definition necessary to do any extra initialization 
required by the MSM5105.

REFERENCES
  None

Copyright (c) 1992, 1995, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998, 1999       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath:  L:/src/asw/MSM6050/vcs/msm_init.h_v   1.2   06 Nov 2001 09:55:48   dbrackma  $
  $Header: //depot/asic/qsc1100/drivers/hw/msm_init.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/00   rmd     Initial revision

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


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

void msm_device_init(void);

#endif /* MSM_INIT_H */

