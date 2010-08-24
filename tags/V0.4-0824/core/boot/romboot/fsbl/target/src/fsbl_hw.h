#ifndef FSBL_HW_H
#define FSBL_HW_H

/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
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
$Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_hw.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_hw.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  
---------------------------------------------------------------------------*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=============================================================================
FUNCTION      FSBL_HW_CLK_BUSY_WAIT

DESCRIPTION   
  This function stalls the PAUSE timer. Conversion from microseconds to 
  pause time is done in the function.

DEPENDENCIES  
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
  
=============================================================================*/
void fsbl_hw_clk_busy_wait(uint32 micro_sec);

/*===========================================================================

FUNCTION  FSBL_HW_INIT_AND_ENTER_DLOAD

DESCRIPTION
  This function performs hardware initialization to setup USB/UART and
  other hardware for the downloader.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
void fsbl_hw_init_and_enter_dload(void);

/*============================================================================

FUNCTION  FSBL_HW_INIT

DESCRIPTION
  This function performs hardware initialization.  It returns the
  MSM hardware revsion number in the location passed in as an
  argument.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void fsbl_hw_init(void);

#endif  /* FSBL_HW_H */

