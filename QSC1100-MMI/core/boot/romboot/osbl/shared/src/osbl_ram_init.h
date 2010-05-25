#ifndef OSBL_RAM_INIT_H
#define OSBL_RAM_INIT_H

/*===========================================================================

              O S  S e c o n d a r y   B o o t   L o a d e r
                             R A M  I n i t 
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the QCSBL RAM
  test and initialization.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_ram_init.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
                                                                 
============================================================================*/


/*============================================================================
 
                           INCLUDE FILES

============================================================================*/


/*============================================================================

                      PUBLIC DATA DECLARATIONS

============================================================================*/

/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/


/*============================================================================

                      PUBLIC FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION OSBL_RAM_INIT

DESCRIPTION
  Copy initialized data (RW-DATA) into RAM. And, Zero-out (ZI-DATA) in RAM.
  See the file qcsbl.scl for a description of the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state.
  
============================================================================*/
void osbl_ram_init( void );

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
void osbl_create_vector_table( void );

#endif  /* OSBL_RAM_INIT_H */
