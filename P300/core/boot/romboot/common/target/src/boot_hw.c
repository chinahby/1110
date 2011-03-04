/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION
  This file provides the minimal hardware setup required for the PBL to
  execute.
  
EXTERNALIZED FUNCTIONS
  sbl_hw_is_auth_enabled
  sbl_hw_get_msm_hw_id
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/shared/boot_hw.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_hw.h"
#include "boot_msm.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*---------------------------------------------------------------------------
  Value to mask out foundry ID bits and the MSM HW Revision 
  from HW_REVISION_NUMBER.
---------------------------------------------------------------------------*/
#define HW_REVISION_AUTHENTICATION_MASK 0x01FFFFFFUL

/*---------------------------------------------------------------------------
  Number of shifts required to format the MSM HW Revision number for the
  authentication code.
---------------------------------------------------------------------------*/
#define HW_REVISION_NUM_SHFT_BITS       32

/*---------------------------------------------------------------------------
  Number of shifts required to format the MSBs of OEM HW ID for the
  authentication code.
---------------------------------------------------------------------------*/
#define OEM_HW_ID_MSB_SHFT_BITS         28


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
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
boolean sbl_hw_is_auth_enabled(void)
{
  /* Read the SECURE_BOOT e-fuse.  If blown (high), authentication
     is required. */
  if ( HWIO_INM( EF_CONFIG_MSB, HWIO_FMSK( EF_CONFIG_MSB, SECURE_BOOT ) ) )
  {
    /* SECURE_BOOT e-fuse is blown, so authentication is required. */
    return TRUE;
  }
  return FALSE;
}

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
uint64 sbl_hw_get_msm_hw_id(void)
{
  uint64 msm_hw_id;

  /* Get the MSM HW Revision number for authentication purposes.  The
     MSM part number is required, excluding the most significant 7 bits,
     which are used to identify different foundry IDs, and version IDs. */

  msm_hw_id = (uint64) HWIO_INM(HW_REVISION_NUMBER, 
                                HW_REVISION_AUTHENTICATION_MASK);

  /* Adjust the MSM HW ID so it is in the format required by the
     authentication SW.  Authentication SW expects the part of the
     HW_REVISION_NUMBER used for authentication to start at bit 32 */
  msm_hw_id <<= HW_REVISION_NUM_SHFT_BITS;

  /* OEM HW ID is always used for authentication; OEM must blow fuses
   * appropriately if authentication is enabled. This used to be an
   * optional field, controlled by a fuse.
   */
  msm_hw_id |= (uint64) ( ( HWIO_EF_CONFIG_MSB_INM(
                             HWIO_FMSK( EF_CONFIG_MSB, OEM_HW_ID ) ) <<
                            OEM_HW_ID_MSB_SHFT_BITS ) |
                          ( HWIO_EF_CONFIG_LSB_INM(
                             HWIO_FMSK( EF_CONFIG_LSB, OEM_HW_ID ) ) >>
                            HWIO_SHFT( EF_CONFIG_LSB, OEM_HW_ID ) ) );

  return msm_hw_id;
}

