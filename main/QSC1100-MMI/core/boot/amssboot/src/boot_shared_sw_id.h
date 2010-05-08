#ifndef BOOT_SHARED_SW_ID_H
#define BOOT_SHARED_SW_ID_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        BOOT SHARED SW ID


GENERAL DESCRIPTION
  APIs for storing and retrieveing the AMSS and OEMSBL SW_ID in Shared segment

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_sw_id.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   rjain   Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  OSBL_SAVE_SW_ID_SHARED_INFO

DESCRIPTION
 This function will save sw_id info to shared segment

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.
===========================================================================*/
void osbl_save_sw_id_shared_info(void);
#endif

#if defined (FEATURE_SHARED_SEGMENT) && !defined (BUILD_BOOT_CHAIN)
/*===========================================================================
 FUNCTION BOOT_GET_SW_ID_SHARED_INFO

 DESCRIPTION
    This function will populate the sw_id data in the correcponding pointers

 PARAMETERS

 DEPENDENCIES
    None.

 RETURN VALUE
    None.

 SIDE EFFECTS
    None
===========================================================================*/
void boot_get_sw_id_shared_info(uint64 *amss_sw_id, uint64 *oemsbl_sw_id);
#endif

#endif /* BOOT_SHARED_SW_ID_H */
