/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION
  This file provides functionality to set and get root certificate location.

EXTERNALIZED FUNCTIONS
  dbl_clk_busy_wait
  dbl_get_clock_speed_set_by_pbl
  dbl_is_auth_enabled
  dbl_verify_pbl_data
  dbl_sha1_hash
  dbl_get_root_cert_ptr
  dbl_get_configured_clock_speed
  dbl_is_high_speed_dload_supported
  dbl_flash_driver_if_init
  dbl_get_shared_config_data
  dbl_configure_hw_at_low_speed
  dbl_configure_hw_at_high_speed
  dbl_is_partition_table_supported
  dbl_clobber_prot_init
  dbl_get_dbl_hdr_ptr_if_shared_by_pbl
  dbl_target_accessor_init
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_target_accessor.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_target_accessor.h"
#include "dbl_target_if.h"
#include "dbl_error_handler.h"
#include "dbl_mc.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

static dbl_target_if_type  dbl_target_if;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_CLK_BUSY_WAIT

DESCRIPTION
  This function provides a busy wait for the caller.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_clk_busy_wait(uint32 us)
{
  dbl_target_if.clk_busy_wait(us);  
}

/*=========================================================================

FUNCTION  DBL_GET_CLOCK_SPEED_SET_BY_PBL

DESCRIPTION
  This function returns the clock speed set by PBL.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_get_clock_speed_set_by_pbl(void)
{
  return (dbl_target_if.get_clock_speed_set_by_pbl());
}

/*===========================================================================

FUNCTION  DBL_IS_AUTH_ENABLED

DESCRIPTION
  This function checks if the authentication is required. This is
  typically enabled by blowing a fuse.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if authentication is required
  FALSE if authentication is not enabled

SIDE EFFECTS
  None

===========================================================================*/
boolean dbl_is_auth_enabled(void)
{
  return (dbl_target_if.is_auth_enabled());
}

/*===========================================================================

FUNCTION  DBL_VERIFY_PBL_DATA

DESCRIPTION
  This function makes sure that the authentication variables are not altered
  in anyway.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_verify_pbl_data(void)
{
  dbl_target_if.verify_pbl_data();
}

/*===========================================================================

FUNCTION  DBL_SHA1_HASH

DESCRIPTION
  This function exposes the sha1_hash routine exposed by PBL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_sha1_hash
(
  uint8  *buff_ptr, 
  uint32  buff_size, 
  uint8  *digest_ptr
)
{
  dbl_target_if.sha1_hash (buff_ptr, buff_size, digest_ptr);
}

/*===========================================================================

FUNCTION  DBL_GET_ROOT_CERT_PTR

DESCRIPTION
  This function returns the root_cert_ptr determined by PBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
const uint8* dbl_get_root_cert_ptr
(
  void
)
{
  return (dbl_target_if.get_root_cert_ptr_set_by_pbl());
}

/*===========================================================================

FUNCTION  DBL_GET_CONFIGURED_CLOCK_SPEED

DESCRIPTION
  This function will return configured clock speed.

DEPENDENCIES
  None

RETURN VALUE
  uint32 configured clock speed in MHz

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_get_configured_clock_speed(void)
{
  return (dbl_target_if.get_configured_clock_speed());
}

/*===========================================================================

FUNCTION  DBL_IS_HIGH_SPEED_DLOAD_SUPPORTED

DESCRIPTION
  Even though FSBL might support high speed DLOAD mode the HW might not allow
  DBL to use this funtionality. FSBL is a library and it shares the stack of
  the calling SBL (DBL and OSBL).
  
  For example:
  -----------
  In targets where ADSP memory is use as IMEM... bytes access to ADSP memory
  is not allowed. PMIC Library and SBI library are doing lot of byte access
  and hence DBL cannot use this method to enter into DLOAD mode.  

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if High Speed DLOAD Supported
  FALSE if High Speed DLOAD Supported

SIDE EFFECTS
  None

===========================================================================*/
boolean dbl_is_high_speed_dload_supported(void)
{
  return (dbl_target_if.is_high_speed_dload_supported());
}

/*===========================================================================

FUNCTION  DBL_FLASH_DRIVER_IF_INIT

DESCRIPTION
  This function updates the flash driver interface which will be used 
  by generic DBL code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_flash_driver_if_init
(
  dbl_flash_device_driver_type *flash_if
)
{
  dbl_target_if.flash_driver_if_init(flash_if);
}

/*===========================================================================

FUNCTION  DBL_GET_SHARED_CONFIG_DATA

DESCRIPTION
  This function will share the flash and DDR/SDRAM specific data with 
  generic DBL code.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_get_shared_config_data
(
  dbl_if_device_configuration_info_type *config_ptr
)
{
  dbl_target_if.get_shared_config_data(config_ptr);
}

/*===========================================================================

FUNCTION  DBL_CONFIGURE_HW_AT_LOW_SPEED

DESCRIPTION
  This function configures hardware to run at low speed..

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_configure_hw_at_low_speed
(
  const boot_clk_speed_type* clk_speed
)
{
  return dbl_target_if.configure_hw_at_low_speed(clk_speed);
}

/*===========================================================================

FUNCTION  DBL_CONFIGURE_HW_AT_HIGH_SPEED

DESCRIPTION
  This function configures hardware to run at high speed..

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
const boot_clk_speed_type* dbl_configure_hw_at_high_speed
(
  const boot_clk_speed_type* clk_speed
)
{
  return dbl_target_if.configure_hw_at_high_speed(clk_speed);
}

/*===========================================================================

FUNCTION  DBL_IS_PARTITION_TABLE_SUPPORTED

DESCRIPTION
  This function returns whether the partition table is supported by the 
  FLASH Target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if flash target supports partition table
  FALSE if flash target doesn't support partition table

SIDE EFFECTS
  None

===========================================================================*/
boolean dbl_is_partition_table_supported
(
  void 
)
{
  return (dbl_target_if.is_partition_table_supported());
}

/*===========================================================================

FUNCTION  DBL_CLOBBER_PROT_INIT

DESCRIPTION
  This function initializes DBL clobber protection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_clobber_prot_init
(
  void 
)
{
  dbl_target_if.clobber_prot_init();
}

/*===========================================================================
FUNCTION  DBL_GET_DBL_HDR_PTR_IF_SHARED_BY_PBL

DESCRIPTION
  This function returns the DBL header ptr which is shared by PBL. 
  If the DBL header is not shared by PBL, it return False

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean dbl_get_dbl_hdr_ptr_if_shared_by_pbl
(
  boot_sbl_header_type **dbl_image_hdr
)
{
  return (dbl_target_if.get_dbl_hdr_ptr_if_shared_by_pbl(dbl_image_hdr));
}

/*===========================================================================

FUNCTION  DBL_TARGET_ACCESSOR_INIT

DESCRIPTION
  This function initializes the target accessor functionality.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_target_accessor_init
(
  dbl_shared_data_type   *shared_data
)
{
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  dbl_target_interface_init(shared_data->pbl_shared_data, &dbl_target_if);
  shared_data->target_if = &dbl_target_if;
}

