#ifndef _BOOTHW_TARGET_H_
#define _BOOTHW_TARGET_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      B O O T    H A R D W A R E    T A R G E T 	S P E C I F I C
                I N I T I A L I Z A T I O N    H E A D E R

GENERAL DESCRIPTION
  This header file contains function definitions specific for the
  various targets. The code here is meant to be common across various platforms. 
  These definitions can be used by other modules in DMSS.

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800/drivers/boot/2.1/boothw_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/04   tkuo    Ported from MSM6550.
===========================================================================*/

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

  FUNCTION boot_hw_ctrl_init()

  DESCRIPTION
    Initialize MSM external bus interface (EBI) registers to enable access
    to the target memory and display devices.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The above hardware circuits are initialized.

===========================================================================*/
void boot_hw_ctrl_init(void);

#endif /*_BOOTHW_TARGET_H_*/

