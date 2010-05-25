#ifndef OSBL_AARM_BOOT_H
#define OSBL_AARM_BOOT_H

/*===========================================================================

                 O E M  S e c o n d a r y  B o o t  L o a d e r
                              a A R M  B o o t 
                           H e a d e r  F i l e


DESCRIPTION
  This header file contains declarations and definitions for the OEM SBL
  loading and booting of the applications ARM.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_aarm_boot.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/06   PS      Changes to remove image_header for Apps & Modem AMSS image
01/22/06   SD      Add support for boot from aARM boot loader.
04/11/05   SD      Added and modified functions exported.
03/07/05   SJD     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "osbl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
  
/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_LOAD_APPSBL

DESCRIPTION
  This function loads the applications ARM boot loader to its
  destination RAM.  It may load the aARM from flash or from data
  contained within the OEM SBL image depending on OEM security
  requirements.  
  
DEPENDENCIES
  aARM boot loader image has its exception vector table as the
  first 32 bytes in the image.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_load_appsbl( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_AUTH_APPSBL

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_auth_appsbl( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_CREATE_AARM_PARTITION_TBL

DESCRIPTION
  This function creates the aARM partition table from the mARM
  based partition table and places it in shared memory.  Note,
  partition tables apply to NAND based targets.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_create_aarm_partition_tbl
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION  OSBL_CREATE_AARM_BAD_BLOCK_TABLE

DESCRIPTION
  This function copies the NAND bad block table to shared memory for
  use by the aARM boot loader.  The bad block table is only valid for
  a NAND boot.  This function does nothing if it is not a NAND boot.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_create_aarm_bad_block_table
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION  OSBL_BOOT_AARM

DESCRIPTION
  This function releases the aARM from reset to boot the aARM.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Initiates execution of the aARM.

===========================================================================*/
extern void osbl_boot_aarm( bl_shared_data_type *bl_shared_data );

#endif  /* OSBL_AARM_BOOT_H */

