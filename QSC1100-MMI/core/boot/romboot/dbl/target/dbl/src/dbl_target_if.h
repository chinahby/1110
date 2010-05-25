#ifndef DBL_TARGET_IF_H
#define DBL_TARGET_IF_H

/*===========================================================================

                  F a i l  S a f e   B o o t  L o a d e r
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the FSBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl_target_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/31/07   anb     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#include "boot_flash.h"
#include "boot_headers.h"
#include "boot_clk_speed.h"
#include "dbl_flash_if.h"

#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#ifndef _ARM_ASM_

typedef struct dbl_target_if_type
{
  /*=========================================================================
    This function returns the clock speed set by PBL.
  ===========================================================================*/
  const boot_clk_speed_type* (*get_clock_speed_set_by_pbl)(void);

  /*===========================================================================
    This function makes sure that the authentication variables are not altered
    in anyway.
  ===========================================================================*/
  void (*verify_pbl_data)(void);

  /*===========================================================================
    This function exposes the sha1_hash routine exposed by PBL.
  ===========================================================================*/
  void (*sha1_hash)(const uint8 *buff_ptr, uint32 buff_size, uint8 *digest_ptr);

  /*=========================================================================
    This function returns the root_cert_ptr determined by PBL
  ===========================================================================*/
  uint8* (*get_root_cert_ptr_set_by_pbl)(void);

  /*=========================================================================
    This function returns TRUE if PBL shares dbl header otherwise return FALSE. 
    If return value is TRUE, this function sets the dbl_imd_hdr pointer
  ===========================================================================*/
  boolean (*get_dbl_hdr_ptr_if_shared_by_pbl) (boot_sbl_header_type **dbl_image_hdr);

  /*=========================================================================
    This function returns whether Authentication is enabled
  ===========================================================================*/
  boolean (*is_auth_enabled)(void);

  /*===========================================================================
    This function will return configured clock speed.
  ===========================================================================*/
  const boot_clk_speed_type* (*get_configured_clock_speed)(void);

  /*===========================================================================
    Even though FSBL might support high speed DLOAD mode the HW might not allow
    DBL to use this funtionality. FSBL is a library and it shares the stack of
    the calling SBL (DBL and OSBL).
    
    For example:
    -----------
    In targets where ADSP memory is use as IMEM... bytes access to ADSP memory
    is not allowed. PMIC Library and SBI library are doing lot of byte access
    and hence DBL cannot use this method to enter into DLOAD mode.  
  ===========================================================================*/
  boolean (*is_high_speed_dload_supported)(void);

  /*===========================================================================
    This function provides a busy wait for the caller.
  ===========================================================================*/
  void (*clk_busy_wait)(uint32 us);

  /*===========================================================================
    This function updates the flash driver interface which will be used 
    by generic DBL code.
  ===========================================================================*/
  void (*flash_driver_if_init)(dbl_flash_device_driver_type *flash_if);

  /*===========================================================================
    This function returns whether the partition table is supported by the 
    FLASH Target.
  ===========================================================================*/
  boolean (*is_partition_table_supported)(void);
  
  /*===========================================================================
    This function configures hardware to run at low speed.
  ===========================================================================*/
  const boot_clk_speed_type* (*configure_hw_at_low_speed)
  (
    const boot_clk_speed_type* clk_speed
  );

  /*===========================================================================
    This function configures hardware to run at high speed.
  ===========================================================================*/
  const boot_clk_speed_type* (*configure_hw_at_high_speed)
  (
    const boot_clk_speed_type* clk_speed
  );

  /*===========================================================================
    This function will share the flash and DDR/SDRAM specific data
  ===========================================================================*/
  void (*get_shared_config_data)(dbl_if_device_configuration_info_type* config_ptr);
  
  /*===========================================================================
    This function will initialize the boot clobber protection by setting
    up protection for the DBL and PBL address ranges
  ===========================================================================*/
  void (*clobber_prot_init)(void);
} dbl_target_if_type;

/*------------------------------------------------------------
**  Interface to target related interface.
** ---------------------------------------------------------*/
/*===========================================================================

FUNCTION  DBL_TARGET_INTERFACE_INIT

DESCRIPTION
  Function that will initialize the target interface so that generic DBL
  code can use it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_target_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data,  /* Pointer to data coming from PBL */  
  dbl_target_if_type          *target_if         /* Pointer to target interface     */
);

#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* DBL_TARGET_IF_H */


