#ifndef _OSBL_HW_INIT_H_
#define _OSBL_HW_INIT_H_

/*===========================================================================

                    OEM Secondary  B o o t   L o a d e r
                        HARDWARE INITIALIZATION 
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the QC-SBL
  hardware initialization. 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc1100/romboot/osbl/osbl_hw_init.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_HW_INIT

DESCRIPTION
  This function perform hardware initialization.It disables interrupts. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_hw_init( bl_shared_data_type *bl_shared_data );

/*===========================================================================

FUNCTION  OSBL_INCREASE_CLK_SPEED

DESCRIPTION
  This function increase clk speed to normal operation speed

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_increase_clk_speed
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION  OSBL_HW_SET_JTAG_STATE

DESCRIPTION

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
extern void osbl_hw_set_jtag_state( boolean enable_jtag );

/*===========================================================================

FUNCTION  OSBL_HW_SMC_LOCK

DESCRIPTION
  Disable modem Tx, enable JTAG access, and lock e-fuse override
  registers.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
extern void osbl_hw_smc_lock( void );

/*===========================================================================

FUNCTION  osbl_usb_chg_init

DESCRIPTION
  The function call to start USB Dead Battery charging
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Initialises the PMIC for autonomous charging

===========================================================================*/

extern void osbl_usb_chg_init( bl_shared_data_type *bl_shared_data );

#endif /* _OSBL_HW_INIT_H_ */
