/*=============================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                              R A M  I n i t 

GENERAL DESCRIPTION
  This file contains the software that performs the OSBL RAM initialization.

EXTERNALIZED FUNCTIONS
  osbl_ram_init
  osbl_create_vector_table

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_ram_init.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/01/07   ly      Ported from 7225.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "osbl_ram_init.h"
#include "osbl.h"
#include "boot_util.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*============================================================================

FUNCTION OSBL_RAM_INIT

DESCRIPTION
  Copy initialized data (RW-DATA) into RAM. And, Zero-out (ZI-DATA) in RAM.
  See the file osbl.scl for a description of the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state.
  
============================================================================*/
void osbl_ram_init( void )
{
  /* Copy RW data from load to image region only if load and image
     regions are different. */
  if ( Load__OSBL_RAM_RW__Base != Image__OSBL_RAM_RW__Base )
  {
    /* Copy RW data from the load region to the execute region .*/
    qmemcpy( (uint8*) Image__OSBL_RAM_RW__Base,
             (uint8*) Load__OSBL_RAM_RW__Base,
             Image__OSBL_RAM_RW__Length );
  }

  /* Zero out the zero-init data segment. */
  qmemset( (uint8*) Image__OSBL_RAM_ZI__Base, 0, Image__OSBL_RAM_ZI__Length );
  
} /* osbl_ram_init() */

/*============================================================================

FUNCTION OSBL_CREATE_VECTOR_TABLE

DESCRIPTION
  Copy Vector table from flash into IRAM or IMEM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
============================================================================*/
void osbl_create_vector_table( void )
{
 register uint32 *dst32;
 register uint32 *src32;
 register uint32 *stop_point;

  /* Copy Vector table to IRAM or IMEM to create the ARM9 indirect
     vector table.*/
  stop_point = (uint32 *) ((uint32) Image__OSBL_VEC_TBL__Base + 
                           Image__OSBL_VEC_TBL__Length);

  /* Start to copy */
  for( src32 = (uint32 *) Load__OSBL_VEC_TBL__Base,
       dst32 = (uint32 *) Image__OSBL_VEC_TBL__Base; 
       dst32 < stop_point; src32++, dst32++ )
  {
    *dst32 = *src32;
  }
  
} /* osbl_create_vector_table() */
