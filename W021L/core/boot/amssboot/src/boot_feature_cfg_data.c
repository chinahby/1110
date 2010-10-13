/*=============================================================================
                        BOOT FEATURE CONFIG DATA

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
  boot_process_cfg_data
  boot_read_cfg_data
  boot_get_general_cfg_item
  boot_get_feature_cfg_item
  boot_feature_cfg_data_set_table
  boot_feature_cfg_get_table

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/drivers/boot/boot_feature_cfg_data.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/24/07   amw     Fix lint errors.
03/17/05   tkuo    Use Trusted_BOOT definition and modify the code logic.
01/19/05   chs     Created for MSM6275.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boothw.h"
#include "boot_feature_cfg_data.h"
#include "err.h"
#include "miheader.h"
#ifndef BUILD_BOOT_CHAIN
  #include "boothw.h"
  #include "boot_cfg_table.h"
#endif
#if defined(BUILD_BOOT_CHAIN_OEMSBL)
  #include "custoemsbl.h"
#elif defined(BUILD_BOOT_CHAIN)
  #include "custqcsbl.h"
#endif

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* Table of configuration data types */
static boot_cfg_data_table_type *boot_cfg_data_table = NULL;


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  BOOT_GET_FEATURE_CFG_ITEM

DESCRIPTION
  This function retrieves feature configuration item stored in our static
  array.

DEPENDENCIES
  None

RETURN VALUE
  The value of the feature configuration item.

SIDE EFFECTS
  None

===========================================================================*/
uint32 boot_get_feature_cfg_item
(
  const boot_cfg_data_id_enum_type item_id /* feature config item to read. */
)
{
  /* Pointer to where the feature configuration data is stored. */
  boot_cfg_data_item_type *feature_cfg_data;

  /* Verify configuration table has already been processed. */
  if ((boot_cfg_data_table == NULL) ||
      (boot_cfg_data_table->cfg_data_processed == FALSE))
  {
    ERR_FATAL("Feature Config Data not read from flash in OEMSBL!", 0, 0, 0);
  }

  feature_cfg_data = boot_cfg_data_table->feature_cfg_data;

  /* Verify configuration item is a feature configuration item. */
  if ((item_id <= START_OF_FEATURE_CFG_DATA) ||
      (item_id >= END_OF_FEATURE_CFG_DATA) )
  {
    ERR_FATAL("there is no feature config data!", 0, 0, 0);
  }

  return feature_cfg_data[(uint32)item_id -
                          ((uint32)START_OF_FEATURE_CFG_DATA+1)].data;

} /* boot_get_feature_cfg_item () */

/*===========================================================================

FUNCTION  BOOT_GET_GENERAL_CFG_ITEM

DESCRIPTION
  This function retrieves feature configuration item stored in our static
  array.

DEPENDENCIES
  None

RETURN VALUE
  The value of the feature configuration item

SIDE EFFECTS
  None

===========================================================================*/
uint32 boot_get_general_cfg_item
(
  const boot_cfg_data_id_enum_type item_id /* feature config item to read. */
)
{
  /* Pointer where the general configuration data are stored. */
  boot_cfg_data_item_type *general_cfg_data;

  /* Verify configuration table has already been processed. */
  if ((boot_cfg_data_table == NULL) ||
      (boot_cfg_data_table->cfg_data_processed == FALSE))
  {
    ERR_FATAL("Feature Config Data not read from flash in OEMSBL!", 0, 0, 0);
  }

  general_cfg_data = boot_cfg_data_table->general_cfg_data;

  /* Verify configuration item is a general configuration item. */
  if ((item_id <= START_OF_GENERAL_CFG_DATA) ||
      (item_id >= END_OF_GENERAL_CFG_DATA) )
  {
    ERR_FATAL("there is no feature config data!", 0, 0, 0);
  }

  return general_cfg_data[(uint32)item_id -
                          ((uint32)START_OF_GENERAL_CFG_DATA+1)].data;

} /* boot_get_general_cfg_item () */

/*===========================================================================

FUNCTION  BOOT_PROCESS_CFG_DATA

DESCRIPTION
  This function processes the configuration data located in the IMEM and
  extracts the feature configuration data information it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_process_cfg_data
(
  boot_cfg_data_type**  cfg_data_ptr     /* Pointer to cfg data */
)
{
  uint32  i;                              /* index to customer cfg table   */
  boot_cfg_data_id_enum_type  cfg_data_id;/* ID from customer cfg data     */
  boot_cfg_data_item_type* item_ptr;    /* start pointers of cfg data item */

  /* Pointer where the general configuration data are stored. */
  boot_cfg_data_item_type *general_cfg_data;
  /* Pointer to where the feature configuration data are stored. */
  boot_cfg_data_item_type *feature_cfg_data;
  /* Verify cfg_data is not NULL. */
  if (*cfg_data_ptr == NULL)
  {
    ERR_FATAL("Invalid argument!", 0, 0, 0);
  }

  /* Verify internal structures are valid */
  if (boot_cfg_data_table == NULL)
  {
    ERR_FATAL("boot_cfg_data_table not initialized", 0, 0, 0);
  }
  general_cfg_data = boot_cfg_data_table->general_cfg_data;
  feature_cfg_data = boot_cfg_data_table->feature_cfg_data;
  item_ptr = (*cfg_data_ptr)->item;

  /* Advance forward to next item in configuration data.  This is the start
  ** of the register data.
  */
  for(i = 0; i < MAX_NUM_CFG_DATA_ITEMS; i++ )
  {
    cfg_data_id = item_ptr[i].id;
    if (cfg_data_id <= START_OF_GENERAL_CFG_DATA)
    {
      /* Skip to the next value, we are not saving REG_DATA or MISC_CMDS. */
      continue;
    }
    else if ((cfg_data_id > START_OF_GENERAL_CFG_DATA)
         && (cfg_data_id < END_OF_GENERAL_CFG_DATA))
    {
      /* Process general config data (non-hardware registers) */
      /* Configuration data IDs are set to zero array for non-register items
      ** not present in configuration data.
      */
      general_cfg_data[(uint32)cfg_data_id -
                       ((uint32)START_OF_GENERAL_CFG_DATA+1)].id   =
                        cfg_data_id;
      general_cfg_data[(uint32)cfg_data_id -
                       ((uint32)START_OF_GENERAL_CFG_DATA+1)].data =
                        item_ptr[i].data;
    }
    else if ((cfg_data_id > START_OF_FEATURE_CFG_DATA)
         && (cfg_data_id < END_OF_FEATURE_CFG_DATA))
    {
      /* Process feature config data (non-hardware registers) */
      /* Configuration data IDs are set to zero array for non-register items
      ** not present in configuration data.
      */
      feature_cfg_data[(uint32)cfg_data_id -
                       ((uint32)START_OF_FEATURE_CFG_DATA+1)].id   =
                        cfg_data_id;
      feature_cfg_data[(uint32)cfg_data_id -
                       ((uint32)START_OF_FEATURE_CFG_DATA+1)].data =
                        item_ptr[i].data;
    }
    else if (cfg_data_id >= END_OF_CLKRGM_CFG_DATA)
    {
      /* Exit loop if there is no clkregim_cfg_data or we read the end of
      ** feature_cfg_data.
      */
      break;
    }
  }

  if (cfg_data_id != END_OF_CLKRGM_CFG_DATA)
  {
    ERR_FATAL("Can't process config data!", 0, 0, 0);
  }

  /* Set flag stating configuration data has been processed successfully. */
  boot_cfg_data_table->cfg_data_processed = TRUE;

} /* boot_process_cfg_data() */

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_READ_CFG_DATA

DESCRIPTION

  This function extracts feature configuration data information from the
  configuration data located in the IMEM and stores it in a array so
  that it can be used later.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_read_cfg_data (void)
{
  /* Call Shared memory to init the configuration table */
  boot_cfg_table_init();
} /* boot_read_cfg_data () */
#endif /* BUILD_BOOT_CHAIN */

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
void boot_feature_cfg_data_set_table(boot_cfg_data_table_type *table)
{
  boot_cfg_data_table = table;
}

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
boot_cfg_data_table_type* boot_feature_cfg_get_table(void)
{
  return boot_cfg_data_table;
}

