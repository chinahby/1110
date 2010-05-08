/*=============================================================================

                         B o o t  C F G  T a b l e
 
GENERAL DESCRIPTION
  This file contains the code such that the processed config data can be
  stored in shared memory by OEMSBL and then later used by AMSS code.
  
EXTERNALIZED FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800b/drivers/boot/boot_cfg_table.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#ifdef BUILD_BOOT_CHAIN
#include "oemsbl_msm.h"
#include "string.h"
#else
#include "target.h"
#endif

#include "boot_cfg_table.h"
#include "boot_shared_seg.h"
#include "boot_feature_cfg_data.h"

#ifdef BUILD_BOOT_CHAIN
#include "sbl.h"
#else
#include "err.h"
#endif

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#ifdef FEATURE_SHARED_SEGMENT
/* Magic number indicating that config data was written into shared memory */
#define CFG_TABLE_MAGIC_NUMBER     0x103B5D7F

typedef struct
{
  BOOT_SHARED_SEG_HEADER
  uint32                   magic_number;  /* magic number                */
  boot_cfg_data_table_type cfg_table;     /* memory to store config data */
} cfg_table_type;

/* Config Data's Shared segment in the shared memory */
#ifndef BUILD_BOOT_CHAIN
#pragma arm section zidata = "SHARED_DATA"
cfg_table_type             cfg_table_shared;
#pragma arm section zidata
#endif
#endif

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

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
void boot_cfg_table_oemsbl_init(void)
{
#ifdef FEATURE_SHARED_SEGMENT
  cfg_table_type * cfg_table_shared_info_ptr = NULL;
  /* Allocate cfg_table_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( cfg_table_shared, sizeof(cfg_table_type), 
                         FALSE, (void**)(&cfg_table_shared_info_ptr)
                       );

  if(cfg_table_shared_info_ptr == NULL)
    return;

  /* Magic Number to indicate that the data is valid */
  cfg_table_shared_info_ptr->magic_number     = CFG_TABLE_MAGIC_NUMBER;
  memcpy( (byte *)&cfg_table_shared_info_ptr->cfg_table,
          (byte *)qcsbl_ptr->get_cfg_data_table(),
          sizeof(boot_cfg_data_table_type)
        );
#endif
}
#endif /* BUILD_BOOT_CHAIN */

#ifndef BUILD_BOOT_CHAIN
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
void boot_cfg_table_init(void)
{

#ifdef FEATURE_SHARED_SEGMENT
  cfg_table_type * cfg_table_shared_info_ptr = NULL;

  /* Get the pointer to cfg_table_shared */
  BOOT_SHARED_SEG_GET_DATA( cfg_table_shared,
                            (void**)(&cfg_table_shared_info_ptr)
                          );

  if (cfg_table_shared_info_ptr != NULL)
  {
    if (cfg_table_shared_info_ptr->magic_number == CFG_TABLE_MAGIC_NUMBER)
    {
      boot_feature_cfg_data_set_table( (boot_cfg_data_table_type *)
                                       &cfg_table_shared_info_ptr->cfg_table);
      return;
    }
  }

  ERR_FATAL("Shared segment for Config data not found ", 0, 0, 0);

#endif /* FEATURE_SHARED_SEGMENT */  

}
#endif /* BUILD_BOOT_CHAIN */
