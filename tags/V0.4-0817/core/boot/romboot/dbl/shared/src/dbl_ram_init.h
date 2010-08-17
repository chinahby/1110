#ifndef DBL_RAM_INIT_H
#define DBL_RAM_INIT_H

/*===========================================================================

                  D E V I C E    B O O T    L O A D E R 
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the DBL RAM 
  initialization.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_ram_init.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
void dbl_ram_init(void);

/*============================================================================

FUNCTION DBL_CREATE_VECTOR_TABLE

DESCRIPTION
  Copy Vector table from flash into IRAM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
============================================================================*/
void dbl_create_vector_table( void );

#endif  /* DBL_RAM_INIT_H */
