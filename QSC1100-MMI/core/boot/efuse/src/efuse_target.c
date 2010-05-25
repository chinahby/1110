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

$Header: //depot/asic/qsc1100/drivers/efuse/src/efuse_target.c#3 $

when          who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   dk      Removing lint warnings.
04/01/09   ddk     Fixing compilation issue seen related to featurisation.
03/30/09   dk       Updated the changes needed.
01/28/09   mp      Initial revision.

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "efuse_target.h"
#include "msm.h"
#include "pm.h"
#include "clk.h"
#include "clkrgm_msm.h"

#if defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN)
/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
static boolean efuse_write_operation_allowed = FALSE;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

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
void efuse_target_read_init(void)
{
    /*Enable the EFUSE_CLK_EN bit in the MISC_CFG_REG in order to access the
      EF_CONFIG_LSB and EF_CONFIG_MSB regs for the efuse read and write*/
 
    /* Check if the EFUSE clock is already on */
    if (!clk_regime_msm_is_on(CLK_RGM_EFUSE_CLK)) 
      clk_regime_msm_enable (CLK_RGM_EFUSE_CLK);  /* If it is off then switch it on */
}

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
void efuse_target_read_deinit(void)
{
    /* Switch off the clock as it is no longer required */
    clk_regime_msm_disable (CLK_RGM_EFUSE_CLK); 
}

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
void efuse_target_write_init(void)
{
    /*Enable the EFUSE_CLK_EN bit in the MISC_CFG_REG in order to access the
      EF_CONFIG_LSB and EF_CONFIG_MSB regs for the efuse read and write*/

     /* Check if the EFUSE clock is already on */
    if (!clk_regime_msm_is_on(CLK_RGM_EFUSE_CLK)) 
      clk_regime_msm_enable (CLK_RGM_EFUSE_CLK);  /* If it is off then switch it on */
}

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
void efuse_target_write_deinit(void)
{
    /* Switch off the clock as it is no longer required */
    clk_regime_msm_disable (CLK_RGM_EFUSE_CLK); 
}

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
efuse_err_enum_type efuse_target_write_item_init(void)
{
    efuse_err_enum_type ret_val = EFUSE_NO_ERR;

    (void)efuse_write_item_cleanup();

    /* Need to setup PMIC voltage and wait*/
    if (pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M ) == PM_ERR_FLAG__SUCCESS)
    {
      clk_busy_wait(200);
      /* Voltage is applied enable the logic to write */
      HWIO_OUT(EF_PWR_CTL, 0);    
      if (pm_vreg_set_level(PM_VREG_RUIM_ID, 2900) == PM_ERR_FLAG__SUCCESS)
      {
        clk_busy_wait(200);
        efuse_write_operation_allowed = TRUE;
      }
      else
      {
        (void)efuse_write_item_cleanup();
        ret_val = EFUSE_FAILED_TO_CHANGE_VOLTAGE;
      }
    }
    else
    {
      ret_val = EFUSE_FAILED_TO_CHANGE_VOLTAGE;
    }

    return ret_val;
}

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
efuse_err_enum_type efuse_target_write_item_cleanup( void )
{

  (void)pm_vreg_control( PM_OFF_CMD, PM_VREG_RUIM_M );

  /* Voltage is disabled disable the logic to write */
  HWIO_OUT(EF_PWR_CTL, 1);

  efuse_write_operation_allowed = FALSE;

  return EFUSE_NO_ERR;
}

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
boolean efuse_is_write_operation_allowed( void )
{
    return efuse_write_operation_allowed;
}
#endif /* #if defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN)*/
