#ifndef BOOT_HW_H
#define BOOT_HW_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION
  This file provides an interface to device bootloader to set and get the
  root certificate.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/shared/boot_hw.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  SBL_HW_IS_AUTH_ENABLED

DESCRIPTION
  This function checks if the authentication is required. This is
  typically enabled by blowing a fuse.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if authentication is required
  FALSE if authentication is not enabled

SIDE EFFECTS
  None

===========================================================================*/
extern boolean sbl_hw_is_auth_enabled(void);

/*===========================================================================

FUNCTION  SBL_HW_GET_MSM_HW_ID

DESCRIPTION
  This function will return the MSM HW ID used for authentication purposes.
  This value is typically defined by blowing a set of fuses.

DEPENDENCIES
  None

RETURN VALUE
  The MSM HW ID to use for authentication. It is in the following format:
    _____________________________________________________
    |                                                    |
    | 63 ---------- 32 |31 ----------16 | 15---------- 0 |
    |    HW PART_NUM ID|   OEM ID       |   OEM MODEL ID |
    |____________________________________________________|
    HW PART_NUM ID is a portion of the MSM ID read from HW_REVISION_NUM
    OEM ID is a 16 bit QCT assigned OEM ID for each OEM
    OEM MODEL ID is an 16 bit OEM assigned handset model ID

SIDE EFFECTS
  None

===========================================================================*/
extern uint64 sbl_hw_get_msm_hw_id(void);

#endif  /* BOOT_HW_H */
