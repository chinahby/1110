#ifndef FSBL_RAM_INIT_H
#define FSBL_RAM_INIT_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the FSBL RAM 
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
$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_ram_init.h#1 $

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

FUNCTION FSBL_RAM_INIT

DESCRIPTION
  Copy initialized data (RW-DATA) into RAM. And, Zero-out (ZI-DATA) in RAM.
  See the file fsbl.scl for a description of the structure of RAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Memory not in the RW regsion and Zero-Init region is  
  not affected by this function -- those locations are left in an
  unknown state. 
============================================================================*/
void fsbl_ram_init(void);


#endif  /* FSBL_RAM_INIT_H */
