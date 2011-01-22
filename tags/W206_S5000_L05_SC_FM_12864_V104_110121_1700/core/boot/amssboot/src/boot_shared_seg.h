#ifndef BOOT_SHARED_SEG_H
#define BOOT_SHARED_SEG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B O O T   S H A R E D   S E G M E N T   R O U T I N E S

GENERAL DESCRIPTION
  This module contains functions for accessing data shared between
  the boot loader and the AMSS

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  boot_shared_seg_get_data    - get the shared data from the shared segment
  boot_shared_seg_alloc       - allocate space for a piece of shared data that
                               is not already existing in the shared segment
  boot_shared_seg_init        - initialize the shared segment in AMSS
  boot_shared_seg_oemsbl_init - initialize the shared segment in OEMSBL
  boot_shared_seg_reinit      - Re-INIT Shared Segment.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_seg.h#3 $

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps      Adding support for SECBOOT 2.0
06/13/07   anb     Externalized function boot_shared_seg_reinit.
02/09/07   rjain   Added sw_id_shared to BOOT_SHARED_SEG_DATA
12/15/06   anb     Added support for partition table for NOR builds.
06/22/06   anb     Added code to create a tail segment.
05/29/06   anb     Added shared segment to share config data betweem OEMSBL
                   and AMSS.
05/06/06   anb     Added code to detect abnormal HW reset.
                   Added code to check whether crash debugging is allowed.
12/12/05   ty      Initial revision

===========================================================================*/

#include "comdef.h"


/**
 * User modifiable list of shared data. Must be defined in terms of
 * BOOT_SHARED_DATA(variable_name)
 */

#define BOOT_SHARED_SEG_DATA_LIST \
  BOOT_SHARED_SEG_DATA(boot_shared_seg_data) \
  BOOT_SHARED_SEG_DATA(boot_shared_seg_tail_data) \
  BOOT_SHARED_SEG_DATA(boot_trap_shared) \
  BOOT_SHARED_SEG_DATA(bio_shared) \
  BOOT_SHARED_SEG_DATA(dload_shared) \
  BOOT_SHARED_SEG_DATA(hw_reset_shared) \
  BOOT_SHARED_SEG_DATA(crash_debug_nv_shared) \
  BOOT_SHARED_SEG_DATA(cfg_table_shared) \
  BOOT_SHARED_SEG_DATA(pboot_block_shared) \
  BOOT_SHARED_SEG_DATA(nor_partition_table_shared) \
  BOOT_SHARED_SEG_DATA(sw_id_shared) \
  BOOT_SHARED_SEG_DATA(hsu_shared)

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/**
 * Shared Segment Header definitions to support the header stuck in front of
 * a shared data object in boot_shared_seg_data.c
 *
 * Header is of this format:
 *   32 bits magic number + 32 bit item enum - identify type of shared data
 *   32 bits size - size to next node
 *   32 bits reinitialize field - whether to always initialize the data on
 *                                call to boot_shared_seg_init
 *               BOOT_SHARED_SEG_REINIT_UNINIT - do not touch
 *               BOOT_SHARED_SEG_REINIT_INVALID - INVALID, re-init needed
 *               BOOT_SHARED_SEG_DATA_REINIT_VALID - VALID, no re-init needed
 */
#define BOOT_SHARED_SEG_MAGIC_NUMBER      0xA1B2C3D4UL
#define BOOT_SHARED_SEG_DATA_REINIT_UNINIT     0xABCDEF00UL
#define BOOT_SHARED_SEG_DATA_REINIT_INVALID    0xABCDEF01UL
#define BOOT_SHARED_SEG_DATA_REINIT_VALID      0xABCDEF02UL

/* Convert a boot_shared_seg_data_type item into a shared segment ID */
#define BOOT_SHARED_SEG_DATA_ITEM_TO_ID(item) \
   ((uint32)(item) + BOOT_SHARED_SEG_MAGIC_NUMBER)

/* Get the ID from the shared segment header */
#define BOOT_SHARED_SEG_DATA_GET_ID(header) \
   ((header)->shared_id)

/* Get the boot_shared_seg_data_type item enum from shared segment header */
#define BOOT_SHARED_SEG_DATA_GET_ITEM(header) \
   ((boot_shared_seg_list_type)((header)->shared_id - BOOT_SHARED_SEG_MAGIC_NUMBER))

/* Get the size of the shared data structure from the header */
#define BOOT_SHARED_SEG_DATA_GET_SIZE(header) \
   ((header)->size)

/* Check if data has been marked as reinitialize on boot up, currently invalid */
#define BOOT_SHARED_SEG_DATA_IS_INVALID(header) \
   ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_INVALID)

/* Check if data has been marked as reinitialize on boot up, currently valid */
#define BOOT_SHARED_SEG_DATA_IS_VALID(header) \
   ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_VALID)

/* Check if data has been marked as uninit */
#define BOOT_SHARED_SEG_DATA_IS_UNINIT(header) \
   ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_UNINIT)

/* Macro to be called to allocate memory for use in shared segment */
#define BOOT_SHARED_SEG_ALLOC(item, size, reinit, data_ptr) \
     boot_shared_seg_alloc(BOOT_SHARED_SEG_DATA_##item, size, (reinit), (data_ptr))

/* Macro to be called to get data from the shared segment */
#define BOOT_SHARED_SEG_GET_DATA(item, data_ptr) \
     boot_shared_seg_get_data(BOOT_SHARED_SEG_DATA_##item, (data_ptr))

/* Macro to define shared data structure header in front of shared data structures */
#define BOOT_SHARED_SEG_HEADER \
  boot_shared_seg_header_type boot_shared_seg_header;

/* Header that marks the beginning of a shared data */
typedef struct{
  uint32 shared_id;
  uint32 size;
  uint32 reinit;
  uint32 duplicate_size;
}boot_shared_seg_header_type;

/**
 * Make an enum type for each shared data. This is used as the item type.
 * It is converted to an ID using the BOOT_SHARED_SEG_ITEM_TO_ID() macro.
 */
#define BOOT_SHARED_SEG_DATA(name) BOOT_SHARED_SEG_DATA_##name,
/* make an enum spanning all items being shared */
typedef enum{
  BOOT_SHARED_SEG_DATA_UNUSED_ITEM = 0x00000000,
  BOOT_SHARED_SEG_DATA_LIST
  BOOT_SHARED_SEG_DATA_LAST_ITEM,
  BOOT_SHARED_SEG_DATA_MAX_ITEM    = 0xFFFFFFFF
}boot_shared_seg_list_type;
#undef BOOT_SHARED_SEG_DATA

/* First shared data structure in shared segment for shared segment usage */
typedef struct
{
  BOOT_SHARED_SEG_HEADER
  void *start_ptr;
  void *end_ptr;
}boot_shared_seg_data_type;

/* Last shared data structure in shared segment for shared segment usage */
typedef struct
{
  BOOT_SHARED_SEG_HEADER
}boot_shared_seg_tail_data_type;

/* shared data for shared segment use */
extern boot_shared_seg_data_type boot_shared_seg_data;
extern boot_shared_seg_tail_data_type boot_shared_seg_tail_data;
/*===========================================================================

  FUNCTION  boot_shared_seg_get_data                         EXTERNAL FUNCTION

  DESCRIPTION
    This function is used to find a specific item from an enumerated list.

    If datum associated with specified item is found, *data_ptr will point to
    the datum. If it is not found, *data_ptr=NULL.

    Function returns TRUE if the datum needs to be initialized.
    Function returns FALSE otherwise.
    ie: data marked REINIT_VALID - returns false
        data marked REINIT_INVALID - returns true
        data marked REINIT_UNINIT - returns false

  PARAMETERS
    item      - item to search for
    data_ptr  - used to return pointer to memory

  RETURN VALUE
    TRUE if datum needs initialization
    FALSE if datum not found, or if it does not need initialization.

  DEPENDENCIES
    None

  SIDE EFFECTS
    *data_ptr will be mutated
===========================================================================*/
extern boolean boot_shared_seg_get_data(boot_shared_seg_list_type item, void **data_ptr);
/*===========================================================================

  FUNCTION  boot_shared_seg_alloc                            EXTERNAL FUNCTION

  DESCRIPTION
    This function is used to find a specific item from an enumerated list.
    If the item is not found, memory is allocated for it if possible.

    If datum associated with specified item is found, *data_ptr will point to
    the datum. If it is not found, *data_ptr will point to memory allocated
    for the datum. If the requested size is too big to be accomodated, then
    *data_ptr=NULL. This should be an exceptional case since all variables
    should be predeclared to lie in the shared segment at compile time.

    Function returns TRUE if the datum needs to be initialized.
    Function returns FALSE otherwise.
    ie: data marked REINIT_VALID - returns false
        data marked REINIT_INVALID - returns true
        data marked REINIT_UNINIT - returns false

  PARAMETERS
    item      - item to search for
    data_ptr  - used to return pointer to memory

  RETURN VALUE
    TRUE if datum needs initialization
    FALSE if datum not found, or if it does not need initialization.

  DEPENDENCIES
    None

  SIDE EFFECTS
    *data_ptr will be mutated
===========================================================================*/
extern boolean boot_shared_seg_alloc(boot_shared_seg_list_type item, uint32 size, boolean reinit, void **data_ptr);

/*===========================================================================

  FUNCTION  boot_shared_seg_init                             EXTERNAL FUNCTION

  DESCRIPTION
    This function is used in AMSS to initialize the shared segment.
    It assumes that the shared segment has already been initialized in
    an earlier stage of the boot process, and that the boot_shared_seg_data
    structure is the first shared datum in the shared segment.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    boot_shared_seg_osbl_init() has been previously called
    boot_shared_seg_data is first shared datum in shared segment

  SIDE EFFECTS
    None
===========================================================================*/
extern void boot_shared_seg_init(void);

/*===========================================================================

  FUNCTION  boot_shared_seg_osbl_init                      EXTERNAL FUNCTION

  DESCRIPTION
    This function is used in the OEMSBL to initialize the shared segment.

  PARAMETERS
    ss_ptr  -  pointer to beginning of shared segment memory
    length  -  length of the shared segment memory

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    All shared data in the shared segment that was marked REINIT_VALID
    is marked REINIT_INVALID
===========================================================================*/
extern void boot_shared_seg_osbl_init(void *ss_ptr, uint32 length);

/*===========================================================================

  FUNCTION  boot_shared_seg_reinit

  DESCRIPTION
    This function is used locally to walk through shared data and mark all
    data from REINIT_VALID to REINIT_INVALID. This should be called once
    on reset.
   
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
    
  SIDE EFFECTS
    All shared data in the shared segment that was marked REINIT_VALID
    is marked REINIT_INVALID
===========================================================================*/
extern void boot_shared_seg_reinit(boolean force_seg_invalid);

#endif /* BOOT_SHARED_SEG_H */
