#ifndef _BOOT_FEATURE_CFG_DATA_H
#define _BOOT_FEATURE_CFG_DATA_H

/*=============================================================================

                        BOOT FEATURE CONFIG DATA HEADER FILE

GENERAL DESCRIPTION

  This function contains code to read the feature configuration data from the
  flash.
  It extracts the PBL/ROM code configuration data ,but 
  keep feature configuration data information from the
  configuration data file and stores it in an array so that it can be used 
  later by either OEMSBL or AMSS.

  The function boot_get_feature_cfg_item can be used to retrieve the feature
  configuration item once its read from the flash.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //depot/asic/msm6800b/drivers/boot/boot_feature_cfg_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/06/06   ty      Changes to process config data in QCSBL
03/17/05   tkuo    Change the filename.
01/19/05   chs     Created for MSM6275.
=============================================================================*/

#include "boot_cfg_data.h"
#ifdef BUILD_BOOT_CHAIN
  #include "boot_comdef.h"
#else
#include "comdef.h"
  #include "boothw.h"
#endif

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

/*---------------------------------------------------------------------------
  Max number of different types of configuration data.
---------------------------------------------------------------------------*/
#define MAX_GENERAL_CFG_DATA_ITEMS  \
               END_OF_GENERAL_CFG_DATA -(START_OF_GENERAL_CFG_DATA+1)
#define MAX_FEATURE_CFG_DATA_ITEMS  \
               END_OF_FEATURE_CFG_DATA -(START_OF_FEATURE_CFG_DATA+1)
/* This value should ideally be equal to CLKRGM_HCLK_NUM_SPEEDS, but do not
 * make cfg_data dependent on clkrgm */
#define MAX_NUM_OF_CLKRGM_HCLK_NUM_SPEEDS  10
#define MAX_CLKRGM_CFG_DATA_ITEMS  \
               ( (END_OF_CLKRGM_CFG_DATA -(START_OF_CLKRGM_CFG_DATA+1)) * \
                  MAX_NUM_OF_CLKRGM_HCLK_NUM_SPEEDS )

/* Target type. FFA/SURF */
typedef enum
{
    BOOT_BOARD_SURF_TYPE = 0,
    BOOT_BOARD_FFA_TYPE  = 1,
    BOOT_SIZE_BOARD_TYPE = 0x7FFFFFFF /* We will keep 4 bytes for alignment 
                                      ** issue 
                                      */
} boot_hw_board_type;
/*---------------------------------------------------------------------------
  Defines the data structure that provides a table of configuration data
---------------------------------------------------------------------------*/
typedef struct
{
  boolean cfg_data_processed;
  /* Arrays where the general configuration data will be stored. */
  boot_cfg_data_item_type general_cfg_data[MAX_GENERAL_CFG_DATA_ITEMS];
  /* Arrays where the feature configuration data will be stored. */
  boot_cfg_data_item_type feature_cfg_data[MAX_FEATURE_CFG_DATA_ITEMS];
  /* Arrays where the clock regime configuration data will be stored. */
  uint32 num_clkrgm_cfg_data;
  boot_cfg_data_item_type clkrgm_cfg_data[MAX_CLKRGM_CFG_DATA_ITEMS];
} boot_cfg_data_table_type;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  BOOT_PROCESS_CFG_DATA 

DESCRIPTION
  This function processes the configuration data located in the IRAM and
  extracts the feature configuration data information it.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_process_cfg_data
(
  boot_cfg_data_type**  cfg_data_ptr     /* Pointer to cfg data */
);

/*===========================================================================

FUNCTION  BOOT_READ_CFG_DATA 

DESCRIPTION

  This function extracts feature configuration data information from the
  configuration data located in the IRAM and stores it in a array so
  that it can be used later.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void boot_read_cfg_data (void);

/*===========================================================================

FUNCTION  BOOT_GET_GENERAL_CFG_ITEM

DESCRIPTION
  This function retrieves general configuration item stored in our static
  array.

DEPENDENCIES
  None.
  
RETURN VALUE
    The value of the general configuration item.

SIDE EFFECTS
    None.

===========================================================================*/

extern uint32 boot_get_general_cfg_item
(
  const boot_cfg_data_id_enum_type /* general config item to read. */
);

/*===========================================================================

FUNCTION  BOOT_GET_FEATURE_CFG_ITEM

DESCRIPTION
  This function retrieves feature configuration item stored in our static
  array.

DEPENDENCIES
  None.
  
RETURN VALUE
    The value of the feature configuration item.

SIDE EFFECTS
    None.

===========================================================================*/

extern uint32 boot_get_feature_cfg_item
(
  const boot_cfg_data_id_enum_type /* feature config item to read. */
);

/*===========================================================================

FUNCTION  BOOT_FEATURE_CFG_GET_TABLE

DESCRIPTION

  This function returns the config data table
  
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the config data table

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_feature_cfg_data_set_table(boot_cfg_data_table_type *table);


/*===========================================================================

FUNCTION  BOOT_FEATURE_CFG_GET_TABLE

DESCRIPTION

  This function returns the config data table
  
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the config data table

SIDE EFFECTS
  None

===========================================================================*/
extern boot_cfg_data_table_type* boot_feature_cfg_get_table(void);

#endif  /* _BOOT_FEATURE_CFG_DATA_H */
