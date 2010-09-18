/*=============================================================================

                         B o o t  C F G  T a b l e

GENERAL DESCRIPTION
  This file contains the code such that the processed config data can be
  stored in shared memory by OEMSBL and then later used by AMSS code.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/boot/boot_shared_progressive_boot_block.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/30/07   ps       Adding support for SECBOOT 2.0
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/


#ifdef BUILD_BOOT_CHAIN
#include BOOT_CUSTSBL_H
#else
#include "target.h"
#endif

#if defined(FEATURE_SHARED_SEGMENT) && defined(FEATURE_PBOOT_BLOCK_SHARED_SEG)

#include "boot_shared_seg.h"
#include "boot_shared_progressive_boot_block.h"
#include "miprogressive.h"

#ifdef BUILD_BOOT_CHAIN
#include BOOT_SBL_H
#include <string.h> 
#include "elf_loader.h"
#else
#include "err.h"
#endif

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

progressive_boot_block_type *pboot_block_ptr;


typedef struct
{
  BOOT_SHARED_SEG_HEADER
  progressive_boot_block_type pbt_table;     /* memory to store Progressive Boot Block */
} pboot_block_type;

/* Config Data's Shared segment in the shared memory */
#pragma arm section zidata = "SHARED_DATA"
pboot_block_type      pbt;
#pragma arm section zidata


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

#ifdef BUILD_BOOT_CHAIN


/*===========================================================================

FUNCTION  BOOT_CFG_TABLE_OEMSBL_INIT

DESCRIPTION
 This function will init shared segment for the configuration table.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.
===========================================================================*/
void boot_pboot_block_oemsbl_init(void)
{
  pboot_block_type * pbt_ptr = NULL;
  /* Allocate cfg_table_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( pboot_block_shared, sizeof(pboot_block_type),
                         FALSE, (void**)(&pbt_ptr)
                       );

  if(pbt_ptr == NULL)
    return;

  memcpy( (byte *)&pbt_ptr->pbt_table,
          (byte *)pboot_block_ptr,
          sizeof(progressive_boot_block_type)
        );
}
#endif /* BUILD_BOOT_CHAIN */



#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_CFG_TABLE_INIT

DESCRIPTION
 This function will init the configuration table

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.
===========================================================================*/
void boot_pboot_block_amss_get(void)
{

   pboot_block_type * pbt_ptr = NULL;

  /* Get the pointer to cfg_table_shared */
  BOOT_SHARED_SEG_GET_DATA( pboot_block_shared,
                            (void**)(&pbt_ptr)
                          );

  if (pbt_ptr != NULL)
  {
      pboot_block_ptr =   (progressive_boot_block_type *) &pbt_ptr->pbt_table;
      return;
  }

  ERR_FATAL("Progressive Boot Block not found in Shared Segment ", 0, 0, 0);


}
#endif /* BUILD_BOOT_CHAIN */

#endif  /* FEATURE_SHARED_SEGMENT && FEATURE_PBOOT_BLOCK_SHARED_SEG  */

