/*==============================================================================

  High Speed USB DSM Utilities
  
  GENERAL DESCRIPTION
    Implementation of various functions that assist with DSM related operations. 
    For example: DSM allocations, scattering and gathering using memcpy, etc...
      
  EXTERNALIZED FUNCTIONS
    hsu_al_dsm_utils_get_dsm_chain_capacity
    hsu_al_dsm_utils_generate_dsm_chain
    hsu_al_dsm_utils_scatter_buffer_without_dmov
    hsu_al_dsm_utils_gather_buffer_without_dmov
    hsu_al_dsm_utils_peek_wmq
    hsu_al_dsm_utils_split_dsm_chain

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_dsm_utils.c#3 $
  $DateTime: 2008/10/29 14:00:56 $ $Author: kimd $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  06/27/06  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_al_dsm_utils.h"
#include "hsu_common.h"
#include "dsm_item.h"
#include "dsm_pool.h"

#ifdef FEATURE_DSM_DYNAMIC_POOL_SELECTION
#include "dsmutil.h"
#endif /* FEATURE_DSM_DYNAMIC_POOL_SELECTION */

#ifdef T_REXNT
#error code not present
#endif /* T_REXNT */
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GET_DSM_CHAIN_CAPACITY

DESCRIPTION   
  Returns the capacity of the DSM chain (number of bytes allocated, not those 
  actually used)

DEPENDENCIES  
  None.

RETURN VALUE  
  The number of bytes allocated in the given chain.

SIDE EFFECTS    
  None.

==============================================================================*/
uint32 hsu_al_dsm_utils_get_dsm_chain_capacity
(
  const dsm_item_type* dsm_chain_ptr
)
{
  uint32 returned_value = 0;
  while (dsm_chain_ptr != NULL)
  {
    /* VC Compiler warns about this:
    returned_value += (dsm_chain_ptr->size); */
    returned_value = (dsm_chain_ptr->size) + returned_value;
    dsm_chain_ptr = dsm_chain_ptr->pkt_ptr;
  }
  return returned_value;
} /* hsu_al_dsm_utils_get_dsm_chain_capacity */

/*===========================================================================

FUNCTION      HSU_AL_DSM_UTILS_GENERATE_DSM_CHAIN

DESCRIPTION
  Based on the given size and a DSM mem pool ID, generates a DSM item chain.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to a dsm_item_type representing the chain if successful.
  NULL - Otherwise.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_DSM_DYNAMIC_POOL_SELECTION
dsm_item_type* hsu_al_dsm_utils_generate_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  uint32                    desired_dsm_chain_size
)
{
  uint16 dsm_size_requested = 0;
  uint32 current_dsm_chain_size = 0;

  dsm_item_type* returned_dsm_chain_ptr = NULL;
  dsm_item_type* dsm_chain_curr_ptr = NULL;

  uint16 size_of_dsm_item;

  size_of_dsm_item = DSM_POOL_ITEM_SIZE(pool_id);
  /* Protect against div. by zero */
  if (size_of_dsm_item == 0)
  {
    HSU_MSG_ERROR("*_generate_dsm_chain: DSM item size for pool_id %d is zero.\
                  No chain will be generated.", pool_id, 0, 0);
    HSU_ASSERT((uint32)"Zero size DSM!!!" == NULL);
    return NULL;
  }

  if (desired_dsm_chain_size == 0)
  {
    HSU_MSG_ERROR("*_generate_dsm_chain: desired_dsm_chain_size is zero.\
                  No chain will be generated.", 0, 0, 0);
    return NULL;
  }

  /* If requested DSM chain size is smaller than one DSM item size from the 
     current pool, just request the minimum size needed. */
  dsm_size_requested = MIN(desired_dsm_chain_size, size_of_dsm_item);
  
  /*  keep allocating item until we get desired_dsm_chain_size */
  while (current_dsm_chain_size < desired_dsm_chain_size)
  {
    /* Use the dynamic pool selection API only for DS DSM item allocation */
    if((DSM_DS_LARGE_ITEM_POOL == pool_id) || (DSM_DS_SMALL_ITEM_POOL == pool_id))
    {
      dsm_chain_curr_ptr = dsm_new_buffer_flexible(pool_id, dsm_size_requested, 
                                                TRUE, DSM_ITEM_SUBSYSTEM_HSUSB); 
    }
    else
    {
      dsm_chain_curr_ptr = dsm_new_buffer(pool_id);
    }
    
    if (dsm_chain_curr_ptr == NULL)
    {
      HSU_MSG_ERROR(
        "*_generate_dsm_chain: No more memory to allocate DSM items. \
        Pool ID: %d", pool_id, 0, 0);
      dsm_free_packet(&returned_dsm_chain_ptr);
      return NULL;        
    }

    /* Update the current chain size with returned dsm item size.
       we may get less than (or more than) what we asked for. */
    current_dsm_chain_size += DSM_ITEM_GET_SIZE(dsm_chain_curr_ptr);
    dsm_append(&returned_dsm_chain_ptr, &dsm_chain_curr_ptr);
  }
  
  return returned_dsm_chain_ptr;
} /* hsu_al_dsm_utils_generate_dsm_chain */

#else /* ! FEATURE_DSM_DYNAMIC_POOL_SELECTION */

dsm_item_type* hsu_al_dsm_utils_generate_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  uint32                    desired_dsm_chain_size
)
{
  uint32 dsm_item_itor;
  uint32 num_of_dsm_items;

  dsm_item_type* returned_dsm_chain_ptr = NULL;
  dsm_item_type* dsm_chain_curr_ptr = NULL;

  uint16 size_of_dsm_item;

  size_of_dsm_item = DSM_POOL_ITEM_SIZE(pool_id);
  /* Protect against div. by zero */
  if (size_of_dsm_item == 0)
  {
    HSU_MSG_ERROR("*_generate_dsm_chain: DSM item size for pool_id %d is zero.\
                  No chain will be generated.", pool_id, 0, 0);
    HSU_ASSERT((uint32)"Zero size DSM!!!" == NULL);
    return NULL;
  }

  if (desired_dsm_chain_size == 0)
  {
    HSU_MSG_ERROR("*_generate_dsm_chain: desired_dsm_chain_size is zero.\
                  No chain will be generated.", 0, 0, 0);
    return NULL;
  }

  num_of_dsm_items = 
    ((desired_dsm_chain_size - 1) / size_of_dsm_item) + 1; 
  /* always round up */

  returned_dsm_chain_ptr = dsm_new_buffer(pool_id);

  if (returned_dsm_chain_ptr == NULL)
  {
    HSU_MSG_ERROR(
      "*_generate_dsm_chain: No more memory to allocate DSM items. \
      Pool ID: %d", pool_id, 0, 0);
    return NULL;
  }

  for (dsm_item_itor = 1 ; dsm_item_itor < num_of_dsm_items ; ++dsm_item_itor)
  {
    dsm_chain_curr_ptr = dsm_new_buffer(pool_id);
    if (dsm_chain_curr_ptr == NULL)
    {
      HSU_MSG_ERROR(
        "*_generate_dsm_chain: No more memory to allocate DSM items. \
        Pool ID: %d", pool_id, 0, 0);
      dsm_free_packet(&returned_dsm_chain_ptr);
      return NULL;        
    }
    dsm_append(&returned_dsm_chain_ptr, &dsm_chain_curr_ptr);
  }
  return returned_dsm_chain_ptr;
} /* hsu_al_dsm_utils_generate_dsm_chain */

#endif /* FEATURE_DSM_DYNAMIC_POOL_SELECTION */

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SCATTER_BUFFER_WITHOUT_DMOV

DESCRIPTION   
  Scatters a contiguous buffer into the given (pre-allocated) DSM item chain.

DEPENDENCIES  
  dest_chain should be other than NULL, each DSM item in the chain should 
  have memory allocated for it (data_ptr should be other than NULL), and should 
  be empty (current_item->used should be 0).

RETURN VALUE  
  TRUE for success, FALSE for failure

SIDE EFFECTS    
  None.

==============================================================================*/
boolean hsu_al_dsm_utils_scatter_buffer_without_dmov
(
  const uint8 *   src_buffer, 
  uint32          src_buffer_size,
  dsm_item_type * dest_chain
)
{
  uint32 dsm_chain_capacity;
  uint32 bytes_copied = 0;
  dsm_item_type* current_item = NULL;

  if (src_buffer == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): src_buffer is NULL", 
      0,0,0);
    return FALSE;
  }
  if (src_buffer_size == 0)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): src_buffer_size is 0", 
      0,0,0);
    return FALSE;
  }
  if (dest_chain == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): dest_chain is NULL", 
      0,0,0);
    return FALSE;
  }

  dsm_chain_capacity = hsu_al_dsm_utils_get_dsm_chain_capacity(dest_chain);
  /* dsm_length_packet returns how much actual data is in a chain, NOT its capacity! */

  if (dsm_chain_capacity == 0)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_scatter_buffer_without_dmov:\
                  dest_chain has zero length", 0, 0, 0);
    return FALSE;
  }

  if (src_buffer_size > dsm_chain_capacity)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_scatter_buffer_without_dmov:\
                  src_buffer_size (%u) is bigger than dest_chain (%u)", 
                  src_buffer_size, dsm_chain_capacity, 0);
    return FALSE;
  }

  current_item = dest_chain;

  do 
  {
    uint16 how_much_to_copy = 0;

    HSU_ASSERT(current_item != NULL);
    HSU_ASSERT(current_item->data_ptr != NULL);
    HSU_ASSERT(current_item->used == 0); /* DSM item ought to be empty ! */

    if (current_item->size < (src_buffer_size - bytes_copied))
    {
      how_much_to_copy = current_item->size;
    }
    else
    {
      /* current_item->size (which is uint16) is bigger than 
      (src_buffer_size - bytes_copied), so it is safe to cast to uint16 */
      how_much_to_copy = (uint16)(src_buffer_size - bytes_copied);
    }

    memcpy(current_item->data_ptr, (void*)(((uint32)src_buffer)+bytes_copied), 
      how_much_to_copy);

    /* Update the used field in the DSM item */
    current_item->used = how_much_to_copy;

    bytes_copied += how_much_to_copy;

    /* Go to next packet */
    current_item = current_item->pkt_ptr;
  } while (bytes_copied < src_buffer_size);

  return TRUE;
} /* hsu_al_dsm_utils_scatter_buffer_without_dmov */

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GATHER_BUFFER_WITHOUT_DMOV

DESCRIPTION   
  Aggregates a DSM item chain into a contiguous buffer, using memcpy().

DEPENDENCIES  
  chain_to_gather should be other than NULL, each DSM item in the chain should 
  have memory allocated for it (data_ptr should be other than NULL), and should 
  not be empty (current_item->used should not be 0).

RETURN VALUE  
  TRUE for success, FALSE for failure

SIDE EFFECTS    
  None.

==============================================================================*/
boolean hsu_al_dsm_utils_gather_buffer_without_dmov
(
  dsm_item_type * chain_to_gather,
  void *          dest_buffer,
  uint32          dest_buffer_size
)
{
  uint32 required_length;
  uint32 bytes_copied = 0;
  dsm_item_type* current_item = NULL;

  HSU_ASSERT(chain_to_gather != NULL);
  HSU_ASSERT(dest_buffer != NULL);

  required_length = dsm_length_packet(chain_to_gather);
  current_item = chain_to_gather;

  if (required_length == 0)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_gather_buffer_without_dmov():\
                  chain_to_gather has zero length", 0, 0, 0);
    return FALSE;
  }

  if (dest_buffer_size < required_length)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_gather_buffer_without_dmov():\
                  chain_to_gather (%u) is bigger than dest_buffer_size (%u)", 
                  required_length, dest_buffer_size, 0);
    return FALSE;
  }

  do 
  {
    HSU_ASSERT(current_item != NULL);
    HSU_ASSERT(current_item->data_ptr != NULL);

    if (current_item->used == 0)
    {
      HSU_MSG_HIGH("*_gather_buffer_without_dmov(): current_item->used = 0",0,0,0);
    }
    else /*current_item->used != 0*/
    {
       memcpy((void*)(((uint32)dest_buffer)+bytes_copied), current_item->data_ptr, 
          current_item->used);
       bytes_copied += current_item->used;
    }

    /* Go to next packet */
    current_item = current_item->pkt_ptr;
  } while (bytes_copied < required_length);

  return TRUE;
} /* hsu_al_dsm_utils_gather_buffer_without_dmov */

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_PEEK_WMQ

DESCRIPTION   
  Gets a pointer to the first DSM chain in the given Watermark Queue, without 
  dequeuing it.

DEPENDENCIES  
  wmq_ptr must not be NULL

RETURN VALUE  
  Pointer to the first DSM chain in the given wmq. NULL if queue is empty

SIDE EFFECTS    
  None.

==============================================================================*/
dsm_item_type* hsu_al_dsm_utils_peek_wmq
(
  const dsm_watermark_type* wmq_ptr
)
{
  dsm_item_type* chain_to_return = NULL;

  HSU_ASSERT(wmq_ptr != NULL);
  HSU_ASSERT(wmq_ptr->q_ptr != NULL);

  chain_to_return = q_check(wmq_ptr->q_ptr);

  return chain_to_return;
} /* hsu_al_dsm_utils_peek_wmq */

/*=============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SPLIT_DSM_CHAIN

DESCRIPTION   
  Splits a given DSM chain in the following fashion:
  The original given chain will have no more than amount_to_leave_in_orig bytes
  , being the closest multiple of the DSM item size specified by pool ID.
  The returned DSM chain's length will be the remainder of the above value from
  the given chain's original length.

  So, if:
  DSMI_DS_LARGE_ITEM_SIZ is 668
  orig_chain has a length of DSMI_DS_LARGE_ITEM_SIZ*10 + 5
  Then the following call:

  hsu_al_dsm_utils_split_dsm_chain(DSM_DS_LARGE_ITEM_POOL, orig_chain, 1400)

  Would cause orig_chain to be trimmed to contain only it's first two DSM items
  and the rest of the chain will be returned, having a length of:
  DSMI_DS_LARGE_ITEM_SIZ*8 + 5.

DEPENDENCIES  
  pool_id must be a valid DSM pool
  orig_dsm_chain_ptr mustn't be NULL

RETURN VALUE  
  TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS    
  orig_dsm_chain_ptr is trimmed (unless its length is less or equal than
  amount_to_leave_in_orig_chain_in_bytes, in which case dest_dsm_chain_ptr is
  NULL).

=============================================================================*/
boolean hsu_al_dsm_utils_split_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_item_type*            orig_dsm_chain_ptr,
  dsm_item_type**           dest_dsm_chain_ptr_ptr,
  uint32                    amount_to_leave_in_orig_chain_in_bytes
)
{
  uint32 orig_chain_length;
  uint32 rounded_off_amount_to_leave_in_orig;
  uint32 dsm_split_result;
  dsm_item_type* current_item_ptr = NULL;

  HSU_ASSERT(orig_dsm_chain_ptr != NULL);
  HSU_ASSERT(dest_dsm_chain_ptr_ptr != NULL);
  HSU_ASSERT(amount_to_leave_in_orig_chain_in_bytes != 0);

  orig_chain_length = dsm_length_packet(orig_dsm_chain_ptr);
  HSU_ASSERT(orig_chain_length != 0);

  /* Ensure that at least the contents of the first DSM item will remain
  in the orig DSM chain */
  HSU_ASSERT(orig_dsm_chain_ptr->used <= amount_to_leave_in_orig_chain_in_bytes);

  if (orig_chain_length <= amount_to_leave_in_orig_chain_in_bytes)
  {
    /* original chain already fulfills the requirement that it be no
    longer than amount_to_leave_in_orig_chain_in_bytes*/
    return TRUE;
  }
  /* else: orig_chain_length > amount_to_leave_in_orig_chain_in_bytes */
  
  /* In order to avoid duplication of DSM items, make sure no DSM item will be split
  in the middle:
  Since dsm_split_packet is using the ->used of each DSM item in the chain
  and NOT ->size, and since a DSM chain doesn't mean all the items except 
  the last, are full, then:
  If the chain has 3 items, with lengths: l1 (the first), l2 and l3 (the last), where:

  l1+l2 < amount_to_leave_in_orig_chain_in_bytes
  l1+l2+l3 > amount_to_leave_in_orig_chain_in_bytes

  then to insure no dup occurs, the desired amount to split must be l1+l2,
  where l1 and/or l2 could be smaller than the size of one DSM item.
  */

  rounded_off_amount_to_leave_in_orig = 0;
  current_item_ptr = orig_dsm_chain_ptr;

  while (current_item_ptr != NULL)
  {
    if (rounded_off_amount_to_leave_in_orig + current_item_ptr->used > 
      amount_to_leave_in_orig_chain_in_bytes)
    {
      break;
    }
    rounded_off_amount_to_leave_in_orig += current_item_ptr->used;
    current_item_ptr = current_item_ptr->pkt_ptr;
  }

  /* pool_id is provided only for hypothetical reasons, as we already made sure
  no DSM item would be split in the middle, by the round off.
  In addition, the dsm_split_packet returns how many bytes remain in the original
  */
  dsm_split_result = dsm_split_packet(&orig_dsm_chain_ptr, dest_dsm_chain_ptr_ptr, 
    rounded_off_amount_to_leave_in_orig, pool_id);
  HSU_ASSERT(dsm_split_result == rounded_off_amount_to_leave_in_orig);

  /* Make sure split function didn't mess any of the args: */
  HSU_ASSERT(orig_dsm_chain_ptr != NULL);
  HSU_ASSERT((*dest_dsm_chain_ptr_ptr) != NULL);

  return TRUE;
} /* hsu_al_dsm_utils_split_dsm_chain */
