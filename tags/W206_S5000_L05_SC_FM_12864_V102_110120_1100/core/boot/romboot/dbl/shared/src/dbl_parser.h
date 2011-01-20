#ifndef DBL_PARSER_H
#define DBL_PARSER_H

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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_parser.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/01/08   ps      Added support for READ_16_BIT, READ_32_BIT
02/20/08   ps      Added support for WRITE_8_BIT, WRITE_16_BIT & WRITE_32_BIT
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Defines the Logical IDs that identify each element that can exist in
  the boot configuration data. WARNING: DON'T CHANGE the ORDER of THIS
  ENUM. 
---------------------------------------------------------------------------*/
typedef enum
{
  START_OF_REG_DATA,
  HWIO_OPERATION,
  SYSTEM_PAUSE,
  END_OF_REG_DATA,
  /*-----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
  START_OF_MISC_CMDS = END_OF_REG_DATA,
  WRITE_8_BIT,                  /* 8 bit write to device  */
  WRITE_16_BIT,                 /* 16 bit write to device */
  WRITE_32_BIT,                 /* 32 bit write to device */
  READ_8_BIT,                   /* 8 bit read - contains address to read */
  READ_16_BIT,                  /* 16 bit read - contains address to read */
  READ_32_BIT,                  /* 32 bit read - contains address to read */
  READ_CMP_DATA,                /* Used to compare the value which was read
                                   before */
  READ_REGISTER,                /* Read previous register in configuration
                                   data - contains mask of bits to read. */
  READ_POLLING_TIMEOUT,         /* Read polling timeout - contains timeout
                                   value in units of microseconds (assuming
                                   the cfg data has not already changed the
                                   clock frequency. */
  BRANCH_ON_READ_FAIL,          /* Branch on read failure by offset given
                                   relative to current location in
                                   configuration data. */
  BRANCH_ALWAYS,                /* Branch unconditionally by offset given
                                   relative to current location in
                                   configuration data. */
  QUEUE_REG_WRITES,             /* Place all register writes following this
                                   command in a queue up to the point a
                                   register flush command in encountered. */
  FLUSH_REG_WRITES,             /* Flush all register writes queued up by
                                   writing them to the registers. */
  WRITE_REGISTER_MASK,          /* Read the previous register in configuration
                                   data - contains mask of bits to write. */
  END_OF_MISC_CMDS,
  END_OF_CFG_DATA               = END_OF_MISC_CMDS,    
  SIZE_ENUM_VALUE               = 0x7FFFFFFF    /* Make enum 32 bits */

} dbl_parser_cfg_data_id_enum_type;

/*---------------------------------------------------------------------------
  Defines the configuration data type stored in external flash and
  read by the boot ROM code.
---------------------------------------------------------------------------*/
typedef struct
{
  dbl_parser_cfg_data_id_enum_type id;
  uint32                           reg_addr;
  uint32                           data;
} dbl_parser_cfg_data_item_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION  DBL_PARSE_CFG_DATA 

DESCRIPTION

 
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dbl_parse_cfg_data
(
  const dbl_parser_cfg_data_item_type*  cfg_data_ptr     /* Pointer to cfg data */
);

#endif  /* DBL_PARSER_H */

