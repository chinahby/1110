#ifndef NVIO_H
#define NVIO_H
/*==========================================================================

  N V   I T E M    M A N A G E R    I/O   S E R V I C E S  
                 H E A D E R    F I L E

DESCRIPTION

  This header file contains all the definitions necessary for to interface
  with the Non Volatile Memory Read / Write Services, in order to read /write
  NVM items using EFS.

Copyright (c)  2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  
when       who     what, where, why
--------   ---     ----------------------------------------------------------

   

===========================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include <stddef.h>
#include <stdio.h>
#include "comdef.h"
#include "nv.h"
#include "nvi.h"
#include "nvim.h"
#include "fs.h"
#include "fs_public.h"


#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================
 
                         DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVIO_READ_ITEM

DESCRIPTION
  Reads an NV item from NV database.

RETURN VALUE
  NV_DONE_S if the read operation is successed.
  NV_FAIL_S or NV_BADPARM_S if the read operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvio_read_item (
 nv_items_enum_type item,   /* name of the item */
 byte index,
 void * nv_data_ptr,    /* data buffer to copy to */
 int item_size
);


/*===========================================================================

FUNCTION NVIO_READ

DESCRIPTION
  This procedure processes NVM read requests.  It checks to make sure
  that a valid item has been requested, it reads the NVM item using EFS
  services and it performs the translation from internal NVM format to
  external format. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

extern nv_stat_enum_type 
nvio_read
(
  nv_cmd_type  *cmd_ptr          /* Command block */
);


/*===========================================================================

FUNCTION NVIO_READ_PRL_ITEM

DESCRIPTION
  This function reads a PRL item from NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_read_prl_item (
  int num,
  PACKED void * data_ptr,
  int item_size
);

/*===========================================================================

FUNCTION NVIO_WRITE_ITEM

DESCRIPTION
  Writes an NV fixed item to NV database.

RETURN VALUE
  NV_DONE_S if the write operation is successed.
  NV_FAIL_S if the write operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvio_write_item (
 nv_items_enum_type item,        /* name of the item */
 byte index,
 void*    nv_data_ptr,   /* data buffer to copy from */
 int item_size
);


/*===========================================================================

FUNCTION NVIO_WRITE

DESCRIPTION
  This function processes NVM write requests.  The function checks
  to make sure that a valid item has been requested, it performs any
  translations necessary from external format to internal NVM format
  and writes the item using EFS write services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

extern nv_stat_enum_type 
nvio_write (
  nv_cmd_type  *cmd_ptr          /* Command block */
);



/*===========================================================================

FUNCTION NVIO_WRITE_RENTAL_TIMER

DESCRIPTION
  This function writes an arbitrary value to the rental timer item.  
  Since this item is expected to be updated with very high frequency,
  it is stored in a way that minimizes EEPROM cell wear.  This unusual
  storage format calls for a special routine to write and read the
  value.
  
  The NV "increment" command would normally be used to update this item;
  this routine should only be used to initialize the item to some desired
  specific value.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif


/*===========================================================================

FUNCTION NVIMW_INCREMENT

DESCRIPTION
  This function processes NVM item "increment" requests.  Currently, only
  the rental timer item can use this operation.  This special processing
  is needed because the rental timer item is an unusually high wear item,
  expected to be written very frequently.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the increment operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type nvimw_increment
(
  nv_cmd_type  *cmd_ptr          /* Command block */
);
/*===========================================================================

FUNCTION NVIO_WRITE_PRL_ITEM

DESCRIPTION
  This function writes a PRL item into NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_write_prl_item (
  int num,
  void * data_ptr,
  int item_size
);


/*===========================================================================

FUNCTION NVIM_INCREMENT

DESCRIPTION
  This function processes NVM item "increment" requests.  Currently, only
  the rental timer item can use this operation.  This special processing
  is needed because the rental timer item is an unusually high wear item,
  expected to be written very frequently.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the increment operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_increment (
  nv_cmd_type  *cmd_ptr          /* Command block */
);

#ifdef __cplusplus
}
#endif

#endif /* NVIMW.H */

