#ifndef DBL_MC_H
#define DBL_MC_H

/*===========================================================================

                  D E V I C E    B O O T    L O A D E R 
                         M a i n  C o n t r o l 
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This header file contains declarations and definitions for main control
  functionality of the DBL.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_mc.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "dbl_target_if.h"
#include "boot_dbl_if.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

typedef struct dbl_auth_info_type           dbl_auth_info_type;
struct boot_clk_speed_type;

/*---------------------------------------------------------------------------
  DBL's state machine.....
---------------------------------------------------------------------------*/
typedef enum
{
  DBL_STATE_NONE,
  DBL_STATE_FLASH_DRIVER_INIT,
  DBL_STATE_CONFIGURE_HW,
  DBL_STATE_FIND_PARTITION,
  DBL_STATE_LOAD_FSBL_IMG,
  DBL_STATE_INIT_FSBL_IMG,
  DBL_STATE_LOAD_OSBL_IMG,
  DBL_STATE_AUTH_IMG,
  DBL_STATE_ENUM            = 0x7FFFFFFF          /* Size enum to 32 bits. */
} dbl_state_enum_type;

/*---------------------------------------------------------------------------
  DBL's Data.
---------------------------------------------------------------------------*/
struct dbl_shared_data_type
{
  /* Variable to store DBL's state                           */
  dbl_state_enum_type                    dbl_state;
  /* Pointer to data structure passed by PBL                 */
  boot_pbl_shared_data_type              *pbl_shared_data;
  /* Pointer to data structure supported by target interface */
  dbl_target_if_type                     *target_if;
  /* Structure containing the generic flash driver           */
  dbl_flash_device_driver_type           *flash_dev_driver;
  /* Clock speed structure                                   */
  const struct boot_clk_speed_type       *clk_speed;
  /* Pointer to system partition table                       */
  void                                   *sys_partition_pointer; 
  /* Structure to store image information for authentication */
  dbl_auth_info_type                     *auth_info;
  /* Variables to indicate the FSBL has been called          */  
  boolean                                fsbl_init_called;    
  /* Pointer to FSBL's interface                             */
  boot_fsbl_if_type                      *fsbl_if_ptr;
  /* Variables to store indicate the validity                */  
  boolean                                entry_ptr_init;
  /* of the DBL fn pointer, and the pointer                  */
  void                                   (*entry_ptr)(void *); 
  /* Pointer to shared data to be sent to OSBL               */
  boot_dbl_if_shared_info_type           *dbl_data_for_osbl;
};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  DBL_MAIN_CTL

DESCRIPTION
   The DBL Main Controller performs the following functions:       
      - 
      
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
void dbl_main_ctl(boot_pbl_shared_data_type *pbl_shared);

#endif  /* DBL_MC_H */

