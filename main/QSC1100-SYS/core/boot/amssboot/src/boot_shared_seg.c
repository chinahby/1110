/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B O O T   S H A R E D   S E G M E N T   R O U T I N E S

GENERAL DESCRIPTION
  This module contains functions for accessing data shared between
  the boot loader and the AMSS

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  boot_shared_seg_get_data - get the shared data from the shared segment
  boot_shared_seg_alloc - allocate space for a piece of shared data that
                         is not already existing in the shared segment
  boot_shared_seg_init - initialize the shared segment                         
  boot_shared_seg_reinit - Re-INIT Shared Segment.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/drivers/boot/boot_shared_seg.c#2 $

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps      Adding support for SECBOOT 2.0
06/13/07   anb     Externalized function boot_shared_seg_reinit.
06/22/06   anb     Added code to create a tail segment.
03/31/06   ty      Do not assume shared segment has been initialized in OEMSBL
12/12/05   ty      Initial revision    

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "boot_shared_seg.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Place in shared segment */
#pragma arm section zidata = "SHARED_DATA"
boot_shared_seg_data_type boot_shared_seg_data;
#pragma arm section zidata

boot_shared_seg_data_type *boot_shared_seg_data_ptr = NULL;

/* Macro to fill in the shared data header with provided data 
 * header - shared data header to fill in
 * item - the boot_shared_seg_data_type enumeration that this data belongs to
 * datasize - the size of the shared data type (including header size)
 * reinitialize - TRUE if this data needs to be reinitialized on reset,
 *              - FALSE otherwise
 */
#define BOOT_SHARED_SEG_DATA_FILL_HEADER(header, item, datasize, reinitialize) \
  do{ \
    (header)->shared_id = BOOT_SHARED_SEG_DATA_ITEM_TO_ID(item); \
    (header)->size = (datasize); \
    (header)->duplicate_size = (datasize); \
    if (reinitialize) \
    { \
      (header)->reinit = BOOT_SHARED_SEG_DATA_REINIT_VALID; \
    } \
    else \
    { \
      (header)->reinit = BOOT_SHARED_SEG_DATA_REINIT_UNINIT; \
    } \
  }while (0)

/* Check if shared data header is valid */
#define BOOT_SHARED_SEG_DATA_IS_HEADER_VALID(header) \
   ( \
     ((header)->shared_id > BOOT_SHARED_SEG_MAGIC_NUMBER) && \
     ((header)->shared_id < BOOT_SHARED_SEG_MAGIC_NUMBER + (uint32)BOOT_SHARED_SEG_DATA_LAST_ITEM) && \
     ((header)->size > 0) && \
     ((header)->size ==(header)->duplicate_size) && \
     (  ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_UNINIT) || \
        ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_INVALID) || \
        ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_VALID)  ) \
   )

/* If data has been marked reinitialize on boot up, mark it invalid */
#define BOOT_SHARED_SEG_DATA_SET_INVALID_AFTER_RESET(header) \
   do{ \
     if ((header)->reinit == BOOT_SHARED_SEG_DATA_REINIT_VALID) \
     { \
       (header)->reinit = BOOT_SHARED_SEG_DATA_REINIT_INVALID; \
     } \
   }while (0)

#define BOOT_SHARED_SEG_DATA_SET_INVALID(header) \
        (header)->reinit = BOOT_SHARED_SEG_DATA_REINIT_INVALID

#define BOOT_SHARED_SEG_DATA_IS_HEADER_FOR_TAIL_SEGMENT(header) \
        ( (header)->shared_id == BOOT_SHARED_SEG_DATA_ITEM_TO_ID(BOOT_SHARED_SEG_DATA_boot_shared_seg_tail_data) )
        
#define BOOT_SHARED_SEG_DATA_IS_HEADER_FOR_FIRST_NODE(header) \
        ( (header)->shared_id == BOOT_SHARED_SEG_DATA_ITEM_TO_ID(BOOT_SHARED_SEG_DATA_boot_shared_seg_data) )
        
/*===========================================================================

                        LOCAL FUNCTION DECLARATIONS

===========================================================================*/

static void boot_shared_seg_create_tail_segment(uint8* tail_segment_location);


/*===========================================================================

  FUNCTION  boot_shared_seg_create_tail_segment

  DESCRIPTION
    This function is used to create the tail segment in the shared segment
    region.
   
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None
    
  SIDE EFFECTS
    None
===========================================================================*/
void boot_shared_seg_create_tail_segment(uint8* tail_segment_location)
{
  boot_shared_seg_header_type *header;
  
  header = (boot_shared_seg_header_type*)tail_segment_location;
  BOOT_SHARED_SEG_DATA_FILL_HEADER(  header, 
                                     BOOT_SHARED_SEG_DATA_boot_shared_seg_tail_data,
                                     sizeof(boot_shared_seg_tail_data_type), 
                                     FALSE
                                  );
}

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
void boot_shared_seg_init()
{
  boot_shared_seg_header_type *header;
  header = (boot_shared_seg_header_type*)(&boot_shared_seg_data);
  if (BOOT_SHARED_SEG_DATA_IS_HEADER_VALID(header))
  {
    boot_shared_seg_data_ptr = &boot_shared_seg_data;
  }
  else
  {
    boot_shared_seg_data_ptr = NULL;
  }
}

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
void boot_shared_seg_osbl_init(void *ss_ptr, uint32 length)
{
  boot_shared_seg_header_type *header;
  boolean force_seg_invalid = TRUE;
  
  boot_shared_seg_data_ptr = (boot_shared_seg_data_type*)ss_ptr;
  
  header = (boot_shared_seg_header_type*)boot_shared_seg_data_ptr;
  
  if (boot_shared_seg_data_ptr == NULL)
  {
    return;
  }
  
  if (BOOT_SHARED_SEG_DATA_IS_HEADER_VALID(header))
    force_seg_invalid = FALSE;

  /* initialize first node for AMSS */
  BOOT_SHARED_SEG_DATA_FILL_HEADER(header, BOOT_SHARED_SEG_DATA_boot_shared_seg_data, sizeof(boot_shared_seg_data_type), FALSE);
  boot_shared_seg_data_ptr->start_ptr = (void*)ss_ptr;
  boot_shared_seg_data_ptr->end_ptr   = (void*)((uint8*)ss_ptr + length);

  /* mark all data that needs to be reinitialized invalid */
  boot_shared_seg_reinit(force_seg_invalid);

}

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
void boot_shared_seg_reinit(boolean force_seg_invalid)
{
  uint8 *current_seg_ptr;
  uint8 *end_seg_ptr;
  boolean found_tail_of_shared_segment = FALSE;
  
  if ( (boot_shared_seg_data_ptr == NULL) || 
       (boot_shared_seg_data_ptr->start_ptr == NULL) || 
       (boot_shared_seg_data_ptr->end_ptr == NULL) )
  {
    return;
  }
  current_seg_ptr = (uint8*)boot_shared_seg_data_ptr->start_ptr;
  end_seg_ptr = (uint8*)boot_shared_seg_data_ptr->end_ptr;

  while ( (current_seg_ptr < end_seg_ptr) && 
          (found_tail_of_shared_segment == FALSE) 
        )
  {
    boot_shared_seg_header_type *header;
    uint32 size;
    header = (boot_shared_seg_header_type*)current_seg_ptr;
    if (BOOT_SHARED_SEG_DATA_IS_HEADER_VALID(header))
    {
      if (BOOT_SHARED_SEG_DATA_IS_HEADER_FOR_TAIL_SEGMENT(header))
      {
        found_tail_of_shared_segment = TRUE;
        break;
      }
      /* header is valid force the data to be invalid */
      if (force_seg_invalid == TRUE)
      {
        if (!BOOT_SHARED_SEG_DATA_IS_HEADER_FOR_FIRST_NODE(header))
        BOOT_SHARED_SEG_DATA_SET_INVALID(header);
      }
#if 0 //Laura no longer needed
      /* header is valid, check if it needs to be reinitialized */
      else if (BOOT_SHARED_SEG_DATA_IS_VALID(header))
      {
        BOOT_SHARED_SEG_DATA_SET_INVALID_AFTER_RESET(header);
      }
#endif
      /* the condition size = 0 to prevent infinite loop is checked in
       * BOOT_SHARED_SEG_DATA_IS_HEADER_VALID()
       */
      size = BOOT_SHARED_SEG_DATA_GET_SIZE(header);
      current_seg_ptr += size;
    }
    else
    {
      break;
    }
  }

  /* We could not find the tail of the Shared segment region that means
   * there is some kind of corruption that was detected 
   */
  if (found_tail_of_shared_segment == FALSE)
  {
    boot_shared_seg_header_type *header;
    uint32 size;
    
    current_seg_ptr = (uint8*)boot_shared_seg_data_ptr->start_ptr;
    header = (boot_shared_seg_header_type*)current_seg_ptr;
    size = BOOT_SHARED_SEG_DATA_GET_SIZE(header);
    current_seg_ptr += size;

    /* Now create the tail segment in the shared region */
    boot_shared_seg_create_tail_segment(current_seg_ptr);
  }
}

/*===========================================================================

  FUNCTION  boot_shared_seg_find_id

  DESCRIPTION
    This function is used locally to walk through shared data and find 
    a datum with the requested ID.

    If datum is found, *data_ptr will point to it and return TRUE
    If datum is not found, *data_ptr will point to location with
       no valid shared data header.
   
  PARAMETERS
    start_seg_ptr - pointer to start of shared segment
    end_seg_ptr   - pointer to end of shared segment
    id            - ID of datum to look for
    data_ptr      - used to return pointer to memory

  RETURN VALUE
    TRUE if datum with ID is found, *data_ptr will point to start of datum
    FALSE if not found, *data_ptr will point to first invalid location
    
  DEPENDENCIES
    None
    
  SIDE EFFECTS
    *data_ptr will be mutated
===========================================================================*/
static boolean boot_shared_seg_find_id(uint8 *start_seg_ptr, uint8 *end_seg_ptr, uint32 id, void **data_ptr)
{
  boolean result = FALSE;
  uint8 *current_seg_ptr = start_seg_ptr;
  
  if ( (start_seg_ptr == NULL) || (end_seg_ptr == NULL) || (data_ptr == NULL) ||
       (end_seg_ptr < start_seg_ptr) )
  {
    return result;
  }
  *data_ptr = NULL;
  
  while ((current_seg_ptr < end_seg_ptr) && (result == FALSE))
  {
    boot_shared_seg_header_type *header;
    uint32 size;
    
    header = (boot_shared_seg_header_type*)current_seg_ptr;
    if ( BOOT_SHARED_SEG_DATA_IS_HEADER_VALID(header) &&
         !BOOT_SHARED_SEG_DATA_IS_INVALID(header) &&
         !BOOT_SHARED_SEG_DATA_IS_HEADER_FOR_TAIL_SEGMENT(header)
       )
    {
      if (BOOT_SHARED_SEG_DATA_GET_ID(header) == id)
      {
        /* found it */
        *data_ptr = (void*)header;
        result = TRUE;
        break;
      }
      else
      {
        /* the condition size = 0 to prevent infinite loop is checked in
         * BOOT_SHARED_SEG_DATA_IS_HEADER_VALID()
         */
        size = BOOT_SHARED_SEG_DATA_GET_SIZE(header);

        current_seg_ptr += size;
      }
    }
    else
    {
      /* Pass pointer to the current segment */
      *data_ptr = (void*)header;
      break;
    }
  }
  return result;
}

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
boolean boot_shared_seg_get_data(boot_shared_seg_list_type item, void **data_ptr)
{
  boot_shared_seg_header_type *header;
  uint8 *current_seg_ptr;
  uint8 *end_seg_ptr;
  boolean result;
  
  /* sanity check for inputs */
  if (data_ptr == NULL)
  {
    return FALSE;
  }
  *data_ptr = NULL;
  
  if ( (item <= BOOT_SHARED_SEG_DATA_UNUSED_ITEM)            || 
       (item >= BOOT_SHARED_SEG_DATA_LAST_ITEM)              ||
       (boot_shared_seg_data_ptr == NULL)            || 
       (boot_shared_seg_data_ptr->start_ptr == NULL) || 
       (boot_shared_seg_data_ptr->end_ptr == NULL) )
  {
    return FALSE;
  }
  
  current_seg_ptr = (uint8*)boot_shared_seg_data_ptr->start_ptr;
  end_seg_ptr = (uint8*)boot_shared_seg_data_ptr->end_ptr;

  result = boot_shared_seg_find_id(current_seg_ptr, 
                                  end_seg_ptr, 
                                  BOOT_SHARED_SEG_DATA_ITEM_TO_ID(item), 
                                  data_ptr);
  
  /* check if we found anything valid, if invalid, return NULL.
   * If valid, return true if needs initialization. */
  if (!result)
  {
    *data_ptr = NULL;
  }
  else
  {
    header = (boot_shared_seg_header_type*)(*data_ptr);
    if (BOOT_SHARED_SEG_DATA_IS_VALID(header) || BOOT_SHARED_SEG_DATA_IS_UNINIT(header))
    {
      result = FALSE;
    }
  }
  return result;
}

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
boolean boot_shared_seg_alloc(boot_shared_seg_list_type item, uint32 size, boolean reinit, void **data_ptr)
{
  boot_shared_seg_header_type *header;
  uint8 *current_seg_ptr;
  uint8 *end_seg_ptr;
  boolean result;
  
  /* sanity check for inputs */
  if (data_ptr == NULL)
  {
    return FALSE;
  }
  *data_ptr = NULL;
  
  if ( (size == 0)                                     ||
       (size % sizeof(uint32) != 0)                    ||
       (size < sizeof(boot_shared_seg_header_type))    ||
       (item <= BOOT_SHARED_SEG_DATA_UNUSED_ITEM)      || 
       (item >= BOOT_SHARED_SEG_DATA_LAST_ITEM)        ||
       (boot_shared_seg_data_ptr == NULL)              || 
       (boot_shared_seg_data_ptr->start_ptr == NULL)   || 
       (boot_shared_seg_data_ptr->end_ptr == NULL) )
  {
    return FALSE;
  }
  
  current_seg_ptr = (uint8*)boot_shared_seg_data_ptr->start_ptr;
  end_seg_ptr = (uint8*)boot_shared_seg_data_ptr->end_ptr;

  result = boot_shared_seg_find_id(current_seg_ptr, 
                                  end_seg_ptr, 
                                  BOOT_SHARED_SEG_DATA_ITEM_TO_ID(item), 
                                  data_ptr);
  
  header = (boot_shared_seg_header_type*)(*data_ptr);

  if (result)
  {
    /* found something valid, make sure size is the same */
    if (BOOT_SHARED_SEG_DATA_GET_SIZE(header) != size)
    {
      /* Size is not the same, do not return the pointer */
      result = FALSE;
      data_ptr = NULL;
    }
    else if (reinit)
    {
      header->reinit = BOOT_SHARED_SEG_DATA_REINIT_VALID;
      result = TRUE;
    }
    else
    {
      header->reinit = BOOT_SHARED_SEG_DATA_REINIT_UNINIT;
      result = FALSE;
    }
  }
  else if (*data_ptr != NULL)
  {
    /* did not find the item, and *data_ptr points to next available mem */

    /* make sure this node + the tail segment will fit within segment size */
    current_seg_ptr = (uint8*)(*data_ptr);
    if ( (current_seg_ptr + size + sizeof(boot_shared_seg_tail_data_type))
         > end_seg_ptr
       )
    {
      /* invalid size! do not do anything. */
      *data_ptr = NULL;
      result = FALSE;
    }
    else
    {
      /* size is good, fill up the header */
      BOOT_SHARED_SEG_DATA_FILL_HEADER(header, item, size, reinit);

      /* Now create the tail segment in the shared region */
      boot_shared_seg_create_tail_segment(current_seg_ptr+size);
      
      /* since the node previously did not exist, indicate it needs
       * initialization */
      result = TRUE;
    }
  }

  return result;
}

