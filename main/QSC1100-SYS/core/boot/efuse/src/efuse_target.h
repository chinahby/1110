#ifndef EFUSE_TARGET_H
#define EFUSE_TARGET_H

/*===========================================================================

                       E F U S E  H e a d e r  F i l e

DESCRIPTION
 Contains target specific defintion and apis to be used to do appropriate 
 initializations in order to be able to read and write to CONFIG Chain EFUSE.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/efuse/src/efuse_target.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   dk      Removing lint warnings.
01/28/09   mp      Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "efuse.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*===========================================================================

                      PRIVATE DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
//#if defined(FEATURE_QFUSE_PROGRAMMING)
/*===========================================================================

FUNCTION  efuse_target_read_init

DESCRIPTION
  Do the target specific hardware initialization for efuse read.
  For ULC:
  Enable the EFUSE_CLK_EN bit in the MISC_CFG_REG in order to access the
  EF_CONFIG_LSB and EF_CONFIG_MSB regs for the efuse read 
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.
  For ULC: The EFUSE_CLK_EN bit is set.

===========================================================================*/
void efuse_target_read_init(void);

/*===========================================================================

FUNCTION  efuse_target_read_deinit

DESCRIPTION
  DO the target specific hardware deinitialization done for efuse read.
  For ULC:
  Disable the EFUSE_CLK_EN bit in the MISC_CFG_REG after the efuse read is done
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is deinitialized.
  For ULC: The EFUSE_CLK_EN bit is set 0.

===========================================================================*/
void efuse_target_read_deinit(void);

/*===========================================================================

FUNCTION  efuse_target_write_init

DESCRIPTION
  Do the target specific hardware initialization for efuse write.
  For ULC:
  Enable the EFUSE_CLK_EN bit in the MISC_CFG_REG in order to access the
  EF_CONFIG_LSB and EF_CONFIG_MSB regs for the efuse write
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.
  For ULC: The EFUSE_CLK_EN bit is set.

===========================================================================*/
void efuse_target_write_init(void);

/*===========================================================================

FUNCTION  efuse_target_write_deinit

DESCRIPTION
  DO the target specific hardware deinitialization done for efuse write.
  For ULC:
  Disable the EFUSE_CLK_EN bit in the MISC_CFG_REG after the efuse write is done
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is deinitialized.
  For ULC: The EFUSE_CLK_EN bit is set 0.

===========================================================================*/
void efuse_target_write_deinit(void);

/*===========================================================================

FUNCTION  efuse_target_write_item_init

DESCRIPTION
  DO the target specific hardware initialization done for efuse write.
  
DEPENDENCIES
  NONE
  
RETURN VALUE
  efuse_err_enum_type

SIDE EFFECTS
  Hardware is initialized.
  
===========================================================================*/
efuse_err_enum_type efuse_target_write_item_init(void);

/*===========================================================================

FUNCTION  EFUSE_TARGET_WRITE_ITEM_CLEANUP

DESCRIPTION
 This function is going to use PMIC library API to trun OFF the LDO.

DEPENDENCIES
 None.
 
RETURN VALUE
 uint32   Any Errors while doing the cleanup.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_target_write_item_cleanup( void );

/*===========================================================================

FUNCTION  efuse_is_write_operation_allowed

DESCRIPTION
 This function is going to return the value of efuse_write_operation_allowed 
 variable which will be used to determine if the write operation is allowed
 or not.

DEPENDENCIES
 None.
 
RETURN VALUE
 boolean - TRUE if write allowed 
         - FALSE if write not allowed.

SIDE EFFECTS
 None.
===========================================================================*/
boolean efuse_is_write_operation_allowed( void );

//#endif /*defined(FEATURE_QFUSE_PROGRAMMING)*/

#endif /* EFUSE_TARGET_H */

