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

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_sw_id.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   rjain   Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "boot_shared_sw_id.h"

#ifdef FEATURE_SHARED_SEGMENT
  #include "boot_shared_seg.h"
#endif /* FEATURE_SHARED_SEGMENT */

#ifndef BUILD_BOOT_CHAIN
  #include "err.h"
#endif

#ifdef BUILD_BOOT_CHAIN
  #include BOOT_SBL_H
#endif


/*=============================================================================

            GLOBAL DECLARATIONS AND EXTERNS FOR MODULE

=============================================================================*/

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
#ifdef FEATURE_SHARED_SEGMENT
/* SW_ID structure to be stored in shared segment */
typedef struct
{
  BOOT_SHARED_SEG_HEADER
  uint64 oemsbl_sw_id;
  uint64 amss_sw_id;
} sw_id_shared_type;

#ifndef BUILD_BOOT_CHAIN
  #pragma arm section zidata = "SHARED_DATA"
  sw_id_shared_type   sw_id_shared;
  #pragma arm section zidata
#endif

#endif /* FEATURE_SHARED_SEGMENT */


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

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
void osbl_save_sw_id_shared_info(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  sw_id_shared_type *sw_id_shared_ptr;

  /* Allocate sw_id_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC(sw_id_shared, sizeof(sw_id_shared_type), TRUE, (void**)(&sw_id_shared_ptr));

  if (sw_id_shared_ptr == NULL)
  {
    return;
  }

  /* Save OEMSBL/AMSS SW_ID in the shared segment */
  //Laura need to fix sw_id_shared_ptr->amss_sw_id = qcsbl_ptr->amss_sw_id;
  //Laura need to fix sw_id_shared_ptr->oemsbl_sw_id = qcsbl_ptr->oemsbl_sw_id;
#endif
}
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
void boot_get_sw_id_shared_info(uint64 *amss_sw_id, uint64 *oemsbl_sw_id)
{
  sw_id_shared_type *sw_id_shared_ptr;

  /* Get the pointer to sw_id_shared */
  BOOT_SHARED_SEG_GET_DATA(sw_id_shared, (void**)(&sw_id_shared_ptr));

  if (sw_id_shared_ptr == NULL)
  {
    MSG_ERROR("SW_ID shared data doesn't exist", 0, 0, 0);
    return;
  }

  /* Get OEMSBL/AMSS SW_ID from the shared segment */
  *amss_sw_id = sw_id_shared_ptr->amss_sw_id;
  *oemsbl_sw_id = sw_id_shared_ptr->oemsbl_sw_id;
}
#endif
