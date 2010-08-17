#ifndef EFUSE_H
#define EFUSE_H

/*===========================================================================

                       E F U S E  H e a d e r  F i l e

DESCRIPTION
 Contains common defintion and apis to be used to read CONFIG Chain EFUSE
 values.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/efuse/src/efuse.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09   dk      Branched for ULC.
01/25/07   amw     Update efuses for 60x5.
07/05/06   anb     Added support to program/blow fuses via FTM.
05/22/06   anb     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

#define SW_CRASH_DEBUG_JTAG_DIS    0x00000001UL

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Defines logical ID's to be used to access/read config chain efuse value */
typedef enum
{
  INCLUDE_OEM_HW_ID,          /* Include OEM HW ID                            */
  SOFTWARE_CRASH_DEBUG,       /* Determine whether crash debugging is disabled*/
  JTAG_EN_N,                  /* Determine whether JTAG is diabled            */
  SECURE_BOOT,                /* Determine whether to authenticate images     */
  TRUSTED_BOOT,               /* Determine whether it is TRUSTED BOOT         */
  ROOT_KEY_SELECT,            /* Root key to use for authentication           */
  OEM_HW_ID,                  /* OEM HW ID programmed in EFUSE register       */
  SW_FUSE_PGM_DSBL,           /* Software fuse programming is disabled        */
  HW_KEY_PGM_DSBL,            /* HW key programming is disabled               */
  MEM_REDUN_AND_CHIP_ID_DSBL, /*                                              */
  CONFIG_PGM_DSBL,            /*                                              */
  MAX_CONFIG_CHAIN_ITEM = 0x7FFFFFFF   /* To ensure it's 32 bits wide */
}efuse_item_type;

/*---------------------------------------------------------------------------
  Error type returned by EFUSE API.
---------------------------------------------------------------------------*/
typedef enum
{
  EFUSE_NO_ERR= 0,                       /* 0x000 */
  EFUSE_DATA_PTR_NULL_ERR,               /* 0x001 */
  EFUSE_NO_SPECIAL_ITEM_FOUND_ERR,       /* 0x010 */
  EFUSE_NO_CONFIG_CHAIN_ITEM_FOUND_ERR,  /* 0x011 */
  EFUSE_OPERATION_NOT_ALLOWED,           /* 0x100 */
  EFUSE_FAILED_TO_CHANGE_VOLTAGE,
  EFUSE_MAX_ERR_CODE = 0x7FFFFFFF        /* To ensure it's 32 bits wide */
}efuse_err_enum_type;

typedef enum
{
  EFUSE_CONFIG_CHAIN_LSB_SEL  = 0,
  EFUSE_CONFIG_CHAIN_MSB_SEL  = 1,
  EFUSE_MAX_CONFIG_CHAIN_SEL  = 0x7FFFFFFF /* To ensure it's 32 bits wide */
} efuse_config_chain_select_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION  EFUSE_GET_ITEM

DESCRIPTION
 Config Chain EFUSE registers stores information which needs to read and
 retrieved for the caller.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Data read from the EFUSE register.
         Any Errors while reading the specified item.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_get_item
(
  efuse_item_type   config_item,
  uint32            *config_chain_data
);

#if defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN)
/*===========================================================================

FUNCTION  EFUSE_WRITE_ITEM_INIT

DESCRIPTION
 Here the assumption is that the QFUSE circuit is connected to Vreg_UIM. This
 function is going to use PMIC library API to control the voltage level for
 this LDO.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_item_init( void );

/*===========================================================================

FUNCTION  EFUSE_WRITE_ITEM_CLEANUP

DESCRIPTION
  This function is going to use PMIC library API to trun OFF the LDO.

DEPENDENCIES
 None.

RETURN VALUE
 uint32   Any Errors while doing the cleanup.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_item_cleanup( void );

/*===========================================================================

FUNCTION  EFUSE_READ_CONFIGURATION_CHAIN

DESCRIPTION
 Config Chain EFUSE registers stores information which needs to read and
 retrieved for the caller.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Data read from the EFUSE register.
         Any Errors while reading the specified item.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_read_configuration_chain
(
  uint32   *config_chain_data
);

/*===========================================================================

FUNCTION  EFUSE_WRITE_CONFIGURATION_CHAIN

DESCRIPTION
 Write to Config Chain EFUSE registers,

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Any Errors while writing/blowing configuration chain.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_configuration_chain
(
  uint32   *config_chain_data
);

#endif /*defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN) */

#endif /* EFUSE_H */

