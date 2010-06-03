#ifndef DBL_FLASH_GENERIC_H
#define DBL_FLASH_GENERIC_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/flash/dbl_flash_generic.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Page size as set by the PBL code.
---------------------------------------------------------------------------*/
#define PAGE_SIZE_BYTES_512        512
#define PAGE_SIZE_BYTES_2K         2048
#define PAGE_SIZE_BYTES_4K         4096

/*---------------------------------------------------------------------------
  Data Width as set by the PBL code.
---------------------------------------------------------------------------*/
#define DBL_FLASH_DATA_WIDTH_8_BIT     8
#define DBL_FLASH_DATA_WIDTH_16_BIT    16
#define DBL_FLASH_DATA_WIDTH_32_BIT    32

typedef enum
{
  /* Data bus width represents bit[0] of index into table. */
  DBL_DATA_BUS_8_BIT        = 0x00000000,         /* 8 bit and 16 bit enums are*/
  DBL_DATA_BUS_16_BIT       = 0x00000001,         /* used as index into a table*/
  DBL_DATA_BUS_32_BIT       = 0x00000002,         /* for NAND ctrl devices. Do */
                                                  /* not change the fixed      */
                                                  /* values                    */
  DBL_DB_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} dbl_data_bus_width_enum_type;

typedef enum
{
  /* Page size represents bits[2:1] of index into table. */
  DBL_PAGE_SIZE_512         = 0x00000000,
  DBL_PAGE_SIZE_2K          = 0x00000002,
  DBL_PAGE_SIZE_4K          = 0x00000004,
  DBL_PAGE_SIZE_RESERVED_0  = 0x00000006,
  DBL_PG_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} dbl_page_size_enum_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

#endif  /* DBL_FLASH_GENERIC_H */

