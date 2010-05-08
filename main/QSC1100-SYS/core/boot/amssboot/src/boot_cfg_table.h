#ifndef BOOT_CFG_TABLE_H
#define BOOT_CFG_TABLE_H

/*===========================================================================

                         B o o t  C F G  T a b l e
 
GENERAL DESCRIPTION
  This file contains the code to such that the processed config data can be
  stored in shared memory by OEMSBL and then later used by AMSS code.
  
EXTERNALIZED FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800b/drivers/boot/boot_cfg_table.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
void boot_cfg_table_init(void);

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
void boot_cfg_table_oemsbl_init(void);

#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_CFG_TABLE_H */
