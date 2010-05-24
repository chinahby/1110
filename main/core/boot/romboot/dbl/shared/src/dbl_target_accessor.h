#ifndef DBL_TARGET_ACCESSOR_H
#define DBL_TARGET_ACCESSOR_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION
  This file provides an interface to device bootloader to set and get the
  root certificate.
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_target_accessor.h#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "dbl.h"
#include "dbl_target_if.h"
#include "boot_clk_speed.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern void dbl_clk_busy_wait(uint32 us);

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
extern const boot_clk_speed_type* dbl_get_clock_speed_set_by_pbl( void );

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
extern void dbl_verify_pbl_data( void );

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
extern void dbl_sha1_hash
(
  uint8  *buff_ptr, 
  uint32  buff_size, 
  uint8  *digest_ptr
);

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
extern const uint8* dbl_get_root_cert_ptr( void );

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
extern boolean dbl_is_auth_enabled( void );

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
extern const boot_clk_speed_type* dbl_get_configured_clock_speed( void );

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
extern boolean dbl_is_high_speed_dload_supported(void);

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
extern const boot_clk_speed_type* dbl_configure_hw_at_low_speed
(
  const boot_clk_speed_type* clk_speed
);

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
extern const boot_clk_speed_type* dbl_configure_hw_at_high_speed
(
  const boot_clk_speed_type* hclk_speed
);

/*===========================================================================

FUNCTION  DBL_GET_DBL_HDR_PTR_IF_SHARED_BY_PBL

DESCRIPTION
  This function returns the DBL header ptr which is shared by PBL. 
  If the DBL header is not shared by PBL, it return False

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dbl_get_dbl_hdr_ptr_if_shared_by_pbl
(
  boot_sbl_header_type **dbl_image_hdr
);

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
extern void dbl_flash_driver_if_init
(
  dbl_flash_device_driver_type *flash_if
);

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
extern boolean dbl_is_partition_table_supported( void );

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
extern void dbl_clobber_prot_init( void );

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
extern void dbl_get_shared_config_data
(
  dbl_if_device_configuration_info_type *config_ptr
);

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
extern void dbl_target_accessor_init
(
  dbl_shared_data_type   *shared_data
);

#endif  /* DBL_TARGET_ACCESSOR_H */

