#ifndef NVIM_H
#define NVIM_H

/*===========================================================================

   N V   I T E M   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  All the declarations and definitions necessary to support the table  
  lookup of mapping between an NV item and the Flash File System files
  which simulate storage in an EEPROM.
  
Copyright (c) 2010 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/msmshared/services/nv1/orphans/1.0/nvim.h#1 $ $DateTime: 2010/08/04 19:51:06 $ $Author: pratapc $
  
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/05   pc      Creation of NV2

===========================================================================*/

/* ======================================================================

                       INCLUDE FILES FOR MODULE

========================================================================*/
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "comdef.h"
#include "nv.h"                  /* for nv_item_type */
#include "nvi.h"                 
#include "fs.h"                  /* for fs_ types */
#include "nv_items.h"
#include "nvdiag.h"

#if !defined(NV_LOCK)
#define NV_LOCK
#include "rex.h"
#define NV_GLOBAL_LOCK_INIT() rex_init_crit_sect (&nv_crit_sect)
#define NV_GLOBAL_LOCK() rex_enter_crit_sect (&nv_crit_sect)
#define NV_GLOBAL_UNLOCK() rex_leave_crit_sect (&nv_crit_sect)
#endif

#ifdef __cplusplus
extern "C" {
#endif
 

/* ======================================================================

                       DEFINITIONS AND DECLARATIONS 

========================================================================*/

#define NVIM_NONARRAY        1

/* #define NVIM_ITEM_INFO_TABLE_SIZE   NV_MAX_I */

#define NVIM_ITEM_INFO_TABLE_SIZE   NV_MAX_I

#define  NVIM_ITEM_NAME_MAX_LENGTH  120
#define  NVIM_FILE_NAME_MAX_LENGTH  128

/* The file containing the ESN and a_keys has a reserved area of FFs */ 
#define PERM_FILE_RESERVED  85     /* Reserved area, all 0xFF */

/* The file containing the factory data has a reserved area of FFs */ 
#define FACTORY_FILE_RESERVED  78     /* Reserved area, all 0xFF */

/* The factory data block has a fixed size */ 
#define NVIM_FACTORY_DATA_SIZE  128

/* Fixed value of dynamic pool area */
#define NVIM_DYNAMIC_POOL_CAPACITY 40960

/* The following defines are simply held over from NVM for convenience */
#define NVIKLTADDR        ((word) 0)

#define NV_FACTORY_RSVD       0x7F       /* give 127 bytes to factory test */

/* Start of factory reserved test area */
#define NVD_MAX_OFFSET       (0x1FFF - NV_FACTORY_RSVD)  /* 8kb */

/* Numbers of bytes represented by a bit in the allocation map */
#define NVIBLOCKSIZE       ((word)8)
/* Number of blocks in a long allocation block */
#define NVIBLOCKSPERLONGALLOC ((word)4)
/* Offset of data in a long block */
#define NVIDATAOFFSET    ((word)1)  
            
#define NVILONGBLOCK      ((word)(NVIBLOCKSIZE*NVIBLOCKSPERLONGALLOC))
#define NVILONGDATASIZE  ((word)(NVILONGBLOCK - NVIDATAOFFSET))

/* Maximum number of files containing SMS data */
#define NVIM_SMS_FILE_COUNT 101 

/* Maximum allowed NAMS, can not modify this value as it affects
   invariant data */
#define NVIM_MAX_NAMS 4

/*===========================================================================
                       TYPE DECLARATIONS
===========================================================================*/

typedef int nvim_item_size_type;


typedef struct {
  int      item_size;    /* Item size */
  uint16   array_size;   /* How many elements of this item */
  boolean  is_present;   /* Item present or not */
}nvim_item_info_type;


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVIM_OP_GET_PRESENCE

DESCRIPTION
  This function determines whether an entry is valid (each of its field
  has a meaningful value), or empty/invalid entry.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the entry is valid and FALSE otherwise.

SIDE EFFECTS

===========================================================================*/
boolean 
nvim_op_get_presence (
  nv_items_enum_type item_code   /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_OP_GET_ARRAY_SIZE

DESCRIPTION
  This function returns the array size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Array size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint16 
nvim_op_get_array_size (
  nv_items_enum_type   item_code    /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_OP_GET_SIZE

DESCRIPTION
  This function returns the size of an item, given its ID (item code).

DEPENDENCIES
  None.

RETURN VALUE
  Item size.

SIDE EFFECTS
  Only a meaningful return value if the entry is valid which is determined
  by calling nv_op_is_valid_entry

===========================================================================*/
uint32 
nvim_op_get_size (
  nv_items_enum_type   item_code   /* Item ID */
);


/*===========================================================================

FUNCTION NVIM_REMOVE_ITEM

DESCRIPTION
  Removes an NV fixed item from NVDB.

RETURN VALUE
  NV_DONE_S if the write operation is successed.
  NV_FAIL_S if the write operation is fail.

===========================================================================*/
nv_stat_enum_type 
nvim_remove_item (
  nv_cmd_type  *cmd_ptr         /* name of the item */
);


/*===========================================================================

FUNCTION NVIM_REMOVE_ALL

DESCRIPTION
  This removes all files under the NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_remove_all ( void );


/*===========================================================================

FUNCTION NVIM_INIT_PRL_DATA

DESCRIPTION
  This function initializes the "prl_valid_data" and "prl_version_data"
  state data items.  These are maintained as NV state data so the functions
  "nvim_prl_is_valid()" and "nvim_prl_version()" can return an immediate 
  response.  

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if the EEPROM access has failed

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void  
nvim_init_prl_data (void);

/*==================================================================
Function: NV_ACCESS_OP_CS_INIT

Description: This functions initialises kxmutex.
==================================================================*/

void 
nv_access_op_cs_init(void);

#ifdef __cplusplus
}
#endif

#endif  /* ifndef NVIM_H */

