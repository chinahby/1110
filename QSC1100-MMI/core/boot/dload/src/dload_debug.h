#ifndef __DLOAD_DEBUG_H__
#define __DLOAD_DEBUG_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              M E M O R Y   D E B U G   I N F O R M A T I O N 

GENERAL DESCRIPTION
  This module defines the memory debug information to be sent to host

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/dload/1_0/dload_debug.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/06   tbg     Moved target specific #defines to doad_debug_xxxx.c 
10/20/05   tbg     Ported to msmshared. Moved some functions to target
                   specific file dload_debug_xxxx.c
08/01/05   ih      Created.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/

typedef enum
{
  OPTIONAL_DEF_NO_SAVE = 0,     /* optional, but do not save by default */
  OPTIONAL_DEF_SAVE,            /* optional, but save by default */
  MANDATORY                     /* mandatory, no user modification */
} dload_save_pref_type;



/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_INIT

DESCRIPTION
  This function initializes dload_debug_info[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_init(void);


/*===========================================================================

FUNCTION DLOAD_DEBUG_ADD_REGION

DESCRIPTION
  This function adds a region to dload_debug_info[]

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_debug_add_region(dload_save_pref_type save_pref, uint32 mem_base, uint32 length, 
                            char *desc, char *filename);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_TARGET_INIT

DESCRIPTION
  This function initializes dload_debug_info[] for the specific target.
  Must be implemented in dload_debug_xxxx.c file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_mem_debug_target_init(void);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_LEN

DESCRIPTION
  This function returns length of the debug info response 

DEPENDENCIES
  None.

RETURN VALUE
  length of the debug info response

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_len(void);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_VERIFY_ADDR

DESCRIPTION
  This function checks to see if it belongs to a region of memory that can be
  dumped.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address verifies, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_verify_addr(uint32 base, uint32 len);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_NUM_ENT

DESCRIPTION
  This function returns number of entries in the mem_debug_info table

DEPENDENCIES
  None.

RETURN VALUE
  number of entries in the mem_debug_info table

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_num_ent(void);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SAVE_PREF

DESCRIPTION
  This function returns the save preference of a given region

DEPENDENCIES
  None.

RETURN VALUE
  save preference of a given region
  0 if invalid region number
SIDE EFFECTS
  None.

===========================================================================*/
uint8 dload_mem_debug_save_pref(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_BASE

DESCRIPTION
  This function returns the memory base of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory base of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_mem_base(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_MEM_LENGTH

DESCRIPTION
  This function returns the memory length of a given region

DEPENDENCIES
  None.

RETURN VALUE
  memory length of a given region
  0 if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_mem_debug_mem_length(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_DESC

DESCRIPTION
  This function returns the description of a given region

DEPENDENCIES
  None.

RETURN VALUE
  description of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_desc(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_FILENAME

DESCRIPTION
  This function returns the filename of a given region

DEPENDENCIES
  None.

RETURN VALUE
  filename of a given region
  NULL if invalid region number

SIDE EFFECTS
  None.

===========================================================================*/
char *dload_mem_debug_filename(uint8 i);

/*===========================================================================

FUNCTION DLOAD_MEM_DEBUG_SUPPORT

DESCRIPTION
  This function returns if memory debug is supported.
  Must be implemented in dload_debug_xxxx.c file.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dload_mem_debug_supported(void);



#endif  /* __DLOAD_DEBUG_H__ */
