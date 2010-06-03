#ifndef HW_MM_H
#define HW_MM_H
/*===========================================================================

                      M U L T I - M E D I A
                H A R D W A R E   U T I L I T I E S
                      H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the utilities contained
  in hw.c.

Copyright (c) 2005, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/hw/hw_mm.h#1 $ $DateTime: 2007/05/17 13:25:59 $ $Author: hvo $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/05   rs      Initial version.

===========================================================================*/

#include "target.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* VFE types supported by various HW versions */
typedef enum
{
  VFE_EAGLE_05, /* Used for MSM6260 or 6255A) */
  VFE_EAGLE_10, /* Used for MSM6550, 6275, 6700, and 6800-R1.0 or R1.1 */
  VFE_EAGLE_11, /* Used for MSM6280 and 6275A */
  VFE_EAGLE_12, /* Used for MSM6800-130nm R2.0*/
  VFE_EAGLE_20, /* Used for MSM6800-65nm */
  VFE_PHEONIX_10, /* Used for MSM7500 */
  VFE_PHEONIX_11  /* Used for MSM7500-R2 */
} hw_vfe_type;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HW_GET_VFE_TYPE

DESCRIPTION
  This function determines the VFE options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  None.

RETURN VALUE
  VFE type supported by HW.

SIDE EFFECTS
  None

===========================================================================*/
extern hw_vfe_type hw_get_vfe_type( void );

#endif  /* HW_MM_H */

