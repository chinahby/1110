#ifndef DBL_TARGET_H
#define DBL_TARGET_H

/*===========================================================================

                     D E V I C E    B O O T    L O A D E R 
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the HW 
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
$Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_msm.h"
#include "boot_headers.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Define the base address of IMEM and the size of IMEM in bytes.
---------------------------------------------------------------------------*/
#define DBL_IMEM_BASE             0x58000000
#define DBL_IMEM_SIZE             0x00020000  /* 128K bytes */

/*---------------------------------------------------------------------------
  Define the base address for DBL location in IMEM
---------------------------------------------------------------------------*/
#define DBL_BUFFER_BASE           0x58010000

/* Define the MAGIC number of DBL */
#define DBL_MAGIC  MAGIC_NUM

/*---------------------------------------------------------------------------
  Definitions of address range where SDRAM, PSRAM, and NOR can exist on
  the MSM.
---------------------------------------------------------------------------*/
#define RANGE_END_ADDR          0x3FFFFFFFUL
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* DBL_TARGET_H */
