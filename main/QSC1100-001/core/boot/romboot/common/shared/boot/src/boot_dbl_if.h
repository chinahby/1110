#ifndef BOOT_DBL_IF_H
#define BOOT_DBL_IF_H

/*===========================================================================

                    D e v i c e   B o o t  L o a d e r
                      M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the DBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_dbl_if.h#3 $

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
#include "boot_fsbl_if.h"
#include "boot_clk_speed.h"
#include "boot_clobber_prot.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/
#ifndef _ARM_ASM_

/*---------------------------------------------------------------------------
  Strutucre to store DDR's information.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32       cs_addr;    /* Chip Select Address */
  uint32       ramsize;    /* Attached DDR Size   */
}dbl_if_shared_ddr_info_type;

typedef struct
{
  uint32                       noofddr;      /* Number of attached DDR */
  dbl_if_shared_ddr_info_type  *ddr_info; /* DDR's Information      */
}dbl_if_shared_ddr_device_info_type;

/*---------------------------------------------------------------------------
  Strutucre to store NAND/OneNAND/MDOC settings.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32       pages_per_block;
  uint32       no_of_blocks;
  uint32       max_num_of_bad_blocks;
}dbl_if_shared_flash_nand_device_info_type;

/*---------------------------------------------------------------------------
  Strutucre to store NOR settings.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32       sector_size;
}dbl_if_shared_flash_nor_device_info_type;

/*---------------------------------------------------------------------------
  DBL's Flash-RAM Shared Data.
---------------------------------------------------------------------------*/
typedef struct
{
  dbl_if_shared_ddr_device_info_type ddr;
  union
  {
    /* Information regarding NAND/OneNAND/MDOC */
    dbl_if_shared_flash_nand_device_info_type nand;  
    /* Information regarding NOR               */
    dbl_if_shared_flash_nor_device_info_type  nor;   
  }flash_dev_info;
  
}dbl_if_device_configuration_info_type;

/*---------------------------------------------------------------------------
  DBL's Functional Table
---------------------------------------------------------------------------*/
typedef struct 
{
  const boot_clk_speed_type* (*cfg_hw_for_high_speed)(const boot_clk_speed_type *clk_speed);
}boot_dbl_fn_tbl_type;

/*---------------------------------------------------------------------------
  Defines the data structure that is filled with data that is shared with
  the OSBL image. It may be necessary for the OSBL to make a deep copy of
  this structure for its own use.
---------------------------------------------------------------------------*/
typedef struct boot_dbl_if_shared_info_type
{
  /* Clock speed structure                                   */
  const boot_clk_speed_type              *clk_speed;     
  /* DBL's exposed Function pointer                          */
  const boot_dbl_fn_tbl_type             *fn_tbl;  
  /* Pointer to data structure passed by PBL                 */
  boot_pbl_shared_data_type              *pbl_shared_data;
  /* Pointer to system partition table                       */
  void                                   *sys_partition_pointer;
  /* Pointer to FLASH setting done by DBL                    */
  dbl_if_device_configuration_info_type  *configuration_shared_data;
  /* Pointer to DBL clobber protection table                 */
  const boot_clobber_global_type         *clobber_tbl_pointer;
  /* Variables to indicate the FSBL has been loaded          */  
  boolean                                fsbl_init;    
  /* Pointer to FSBL's interface                             */
  boot_fsbl_if_type                      *fsbl_if_ptr;  
}boot_dbl_if_shared_info_type;

#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


#endif /* BOOT_DBL_IF_H */

