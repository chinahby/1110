/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

  
EXTERNALIZED FUNCTIONS
  dbl_target_interface_init
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/dbl_target.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_target_if.h"
#include "boot_pbl_accessor.h"
#include "dbl_error_handler.h"
#include "dbl_hw.h"
#include "dbl_clk.h"
#include "dbl_ebi2.h"
#include "dbl_ebi1.h"
#include "dbl_flash_nor.h"
#include "boot_headers.h"
#include "dbl_target.h"
#include "dbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

static dbl_flash_device_driver_type *dbl_flash_if = NULL;

/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  DBL_TARGET_GET_DBL_HDR_PTR_IF_SHARED_BY_PBL

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
static boolean dbl_target_get_dbl_hdr_ptr_if_shared_by_pbl
(
  boot_sbl_header_type **dbl_image_hdr
)
{
  DBL_VERIFY((dbl_image_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);
  *dbl_image_hdr = boot_pbl_get_dbl_hdr_ptr();
  return TRUE;
  /* Return False if PBL doesn't share DBL header */
}

/*===========================================================================

FUNCTION  DBL_TARGET_IS_PARTITION_TABLE_SUPPORTED

DESCRIPTION
  This function would return TRUE if partition table is supported, otherwise 
  return FALSE. The return value would be per target basis

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
static boolean dbl_target_is_partition_table_supported( void )
{
  /* Return the decision based on the flash type and whether or not 
     we support NOR partitioning */
  if( boot_pbl_get_flash_type() != NOR_FLASH)
  {
    return TRUE;
  }
  else 
  {
    /* NOR flash in use. Return TRUE if NOR partitioning is supported. Otherwise FALSE */
    return FALSE;
  }
}

/*===========================================================================

FUNCTION  DBL_TARGET_CONFIGURE_HW_AT_LOW_SPEED

DESCRIPTION
  This function configures hardware to run at low speed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static const boot_clk_speed_type* dbl_target_configure_hw
(
  const boot_clk_speed_type* clk_speed
)
{
  /* 1. Configure the clock settings */
  clk_speed = dbl_clk_configure(clk_speed);
  /* 2. Configure the EBI2 settings */  
  dbl_ebi2_configure(clk_speed);
  /* 3. Configure the EBI1 settings */    
  dbl_ebi1_configure(clk_speed);
  return clk_speed;
}

/*===========================================================================

FUNCTION  DBL_TARGET_FLASH_DRIVER_IF_INIT

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
static void dbl_target_flash_driver_if_init
(
  dbl_flash_device_driver_type *flash_if
)
{

  DBL_VERIFY((flash_if != NULL), (uint32)DBL_ERR_NULL_PTR);
  dbl_flash_if = flash_if;
  switch (boot_pbl_get_flash_type())
  {
    case NOR_FLASH:
      dbl_flash_nor_detected(flash_if);
      break;

    default:
      DBL_ERR_FATAL((uint32)DBL_ERR_FLASH_TYPE_NONE);
  }

}

/*===========================================================================

FUNCTION  DBL_TARGET_VERIFY_PBL_DATA

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
static void dbl_target_verify_pbl_data( void )
{
  boolean auth_enabled;
  uint64  read_msm_hw_id;

  auth_enabled = sbl_hw_is_auth_enabled();

  DBL_VERIFY ( (auth_enabled == boot_pbl_is_auth_enabled()),
               (uint32) DBL_ERR_DATA_FROM_PBL_ALTERED
             );
  
  if (boot_pbl_is_auth_enabled() == TRUE)
  {
    read_msm_hw_id = sbl_hw_get_msm_hw_id();
    DBL_VERIFY ( (read_msm_hw_id == boot_pbl_get_msm_hw_id()),
                 (uint32) DBL_ERR_DATA_FROM_PBL_ALTERED
               );
  }
  
} /* dbl_target_verify_pbl_data() */

/*===========================================================================

FUNCTION  DBL_TARGET_CLOBBER_PROT_INIT

DESCRIPTION
  Initializes the boot clobber protection by setting up protection
  for the DBL and PBL address ranges.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dbl_target_clobber_prot_init( void )
{
  /* Setup protection for all DBL and PBL address ranges.  This is target 
     specific range. In ULC we protect 

     1. Lower 64K of IMEM which has PBL shared area + DBL Stack
     2. DBL code + DBL RW + DBL ZI
   */         
  uint8 *protection_region_start;
  uint32 protection_region_size;

  protection_region_start = (uint8*)DBL_IMEM_BASE;
  protection_region_size  = (uint32)( (uint32) boot_pbl_get_stack_base() - 
                                      (uint32) 0x2000 - 
                                      (uint32) protection_region_start);

  boot_clobber_add_global_protection_region( (void*) protection_region_start, 
                                             protection_region_size
                                           );

  protection_region_start = (uint8*) boot_pbl_get_dbl_dest_addr();
  protection_region_size  = ( uint32) ( Image__DBL_RAM_NZI__Base + 
                                        Image__DBL_RAM_NZI__Length - 
                                        (uint32)protection_region_start
                                      );

  boot_clobber_add_global_protection_region( (void*) protection_region_start, 
                                             protection_region_size
                                           );
} /* dbl_target_clobber_prot_init() */

/*===========================================================================

FUNCTION  DBL_TARGET_GET_SHARED_CONFIG_DATA

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
static void dbl_target_get_shared_config_data
(
  dbl_if_device_configuration_info_type *config_ptr
)
{
  dbl_flash_if->share_data( dbl_flash_if, config_ptr );
}

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
)
{
  boolean status = FALSE;
  DBL_VERIFY((pbl_shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((target_if != NULL), (uint32)DBL_ERR_NULL_PTR);

  status = boot_pbl_interface_init(pbl_shared_data);
  DBL_VERIFY((status == TRUE), (uint32)DBL_ERR_FAILED_TO_INIT_PBL_INTERFACE);
  
  target_if->get_clock_speed_set_by_pbl       = dbl_clk_get_speed_set_by_pbl;
  target_if->sha1_hash                        = boot_pbl_sha1_hash;
  target_if->get_root_cert_ptr_set_by_pbl     = boot_pbl_get_root_cert_ptr;
  target_if->get_dbl_hdr_ptr_if_shared_by_pbl = dbl_target_get_dbl_hdr_ptr_if_shared_by_pbl;
  target_if->verify_pbl_data                  = dbl_target_verify_pbl_data; 
  
  target_if->is_auth_enabled                  = sbl_hw_is_auth_enabled;
  target_if->get_configured_clock_speed       = dbl_hw_get_configured_clock_speed;
  target_if->is_high_speed_dload_supported    = dbl_hw_is_high_speed_dload_supported;
  target_if->clk_busy_wait                    = dbl_hw_clk_busy_wait;
  
  target_if->configure_hw_at_low_speed        = dbl_target_configure_hw;
  target_if->configure_hw_at_high_speed       = dbl_target_configure_hw;
  target_if->flash_driver_if_init             = dbl_target_flash_driver_if_init;
  target_if->get_shared_config_data           = dbl_target_get_shared_config_data;
  target_if->is_partition_table_supported     = dbl_target_is_partition_table_supported;

  target_if->clobber_prot_init             = dbl_target_clobber_prot_init;
}

