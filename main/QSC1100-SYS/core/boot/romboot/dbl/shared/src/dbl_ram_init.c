/*=============================================================================

                  D E V I C E    B O O T    L O A D E R 
                    R A M  I n i t i a l i z a t i o n

GENERAL DESCRIPTION
  This file contains the software that performs the DBL RAM initialization.

EXTERNALIZED FUNCTIONS
  dbl_ram_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_ram_init.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_ram_init.h"
#include "dbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*============================================================================

FUNCTION DBL_RAM_INIT_RW

DESCRIPTION
  Copy initialized data (RW-DATA) into RAM. 
  See the file dbl.scl for a description of the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state. 
============================================================================*/
static void dbl_ram_init_rw(void)
{
  register uint32 *dst32; /* Destination address      */
  register uint32 *stop_point; /* Upper limit address */
  register uint32 *src32; /* Destination address      */

  /* Copy RW data from load to image region only if load and image
     regions are different. */
  if ( Load__DBL_RAM_RW__Base != (uint32 *)Image__DBL_RAM_RW__Base )
  {
    /* Copy ROM RW data to internal RAM to create RW region.*/
    stop_point = (uint32 *) ((uint32) Image__DBL_RAM_RW__Base +
                             Image__DBL_RAM_RW__Length);

    /* Start to copy */
    for ( src32 = (uint32 *) Load__DBL_RAM_RW__Base,
            dst32 = (uint32 *) Image__DBL_RAM_RW__Base; 
          dst32 < stop_point;
          src32++, dst32++ )
    {
      *dst32 = *src32;
    }
  }

}/* dbl_ram_init_rw */

/*============================================================================

FUNCTION DBL_RAM_INIT_ZI

DESCRIPTION
  Zero-out (ZI-DATA) in RAM. See the file dbl.scl for a description of 
  the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state. 
============================================================================*/
static void dbl_ram_init_zi(void)
{
  register uint32 *dst32; /* Destination address      */
  register uint32 *stop_point; /* Upper limit address */

  /* Zero out the zero-init data segment.
   * This writes zeros into the RAM in the zero-init region of the
   * internal ram.  This area starts at Image__RAM__ZI_Base and
   * continues for Image__RAM__ZI_Length dwords.
   */
  stop_point = (uint32 *) ((uint32) Image__DBL_RAM_ZI__Base + \
                           Image__DBL_RAM_ZI__Length); 
  /* Start to write 0 */
  for( dst32=(uint32 *) Image__DBL_RAM_ZI__Base; dst32 < stop_point; 
                                                            dst32++ ) 
  {
    *dst32 = 0;
  }

}/* dbl_ram_init_zi */

/*============================================================================

FUNCTION DBL_RAM_INIT

DESCRIPTION
  Copy initialized data (RW-DATA) into RAM.Zero-out (ZI-DATA) in RAM. 
  See the file dbl.scl for a description of the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state. 
============================================================================*/
void dbl_ram_init(void)
{
  /*------------------------------------------------------------------
   Initialize the RW according to scatter load file.
  ------------------------------------------------------------------*/
  dbl_ram_init_rw();

  /*------------------------------------------------------------------
   Initialize the ZI according to scatter load file.
  ------------------------------------------------------------------*/
  dbl_ram_init_zi();
}/* dbl_ram_init */

/*============================================================================

FUNCTION DBL_CREATE_VECTOR_TABLE

DESCRIPTION
  Copy Vector table from RAM into IRAM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
============================================================================*/
void dbl_create_vector_table( void )
{
 register uint32 *dst32;
 register uint32 *src32;
 register uint32 *stop_point;

  /* Copy Vector table to internal RAM to create vector table in IRAM.*/
  stop_point = (uint32 *) ((uint32) Image__DBL_IRAM_VEC_TBL__Base + 
                           Image__DBL_IRAM_VEC_TBL__Length);

  /* Start to copy */
  for( src32 = (uint32 *) Load__DBL_IRAM_VEC_TBL__Base,
       dst32 = (uint32 *) Image__DBL_IRAM_VEC_TBL__Base; 
       dst32 < stop_point; src32++, dst32++ )
  {
    *dst32 = *src32;
  }
  
} /* dbl_create_vector_table() */


