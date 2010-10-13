#ifndef _HSU_AL_DSM_UTILS_H_
#define _HSU_AL_DSM_UTILS_H_

/*==============================================================================

  High Speed USB DSM Utilities - H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Implementation of various function that assist with DSM related operations. 
    For example: DSM allocations, scattering and gathering using memcpy, etc...
  
  EXTERNALIZED FUNCTIONS
    hsu_al_dsm_utils_get_dsm_chain_capacity
    hsu_al_dsm_utils_generate_dsm_chain
    hsu_al_dsm_utils_scatter_buffer_without_dmov
    hsu_al_dsm_utils_gather_buffer_without_dmov
    hsu_al_dsm_utils_peek_wmq

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_dsm_utils.h#1 $
  $DateTime: 2008/02/04 22:58:53 $ $Author: lmanor $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/27/06  ke      Initial version
    
==============================================================================*/
#include "dsm.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
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
);

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
dsm_item_type* hsu_al_dsm_utils_generate_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  uint32                    desired_dsm_chain_size
);

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
);

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
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_PEEK_WMQ

DESCRIPTION   
  Gets the first DSM chain in the given Watermark Queue, without dequeuing it.

DEPENDENCIES  
  wmq_ptr must not be NULL

RETURN VALUE  
  The first DSM chain in the given wmq. NULL if queue is empty

SIDE EFFECTS    
  None.

==============================================================================*/
dsm_item_type* hsu_al_dsm_utils_peek_wmq
(
  const dsm_watermark_type* wmq_ptr
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SPLIT_DSM_CHAIN

DESCRIPTION   
  Splits a given DSM chain in the following fashion:
  The original given chain will have no more than amount_to_leave_in_orig bytes,
  being the closest multiple of the DSM item size specified by pool ID.
  The returned DSM chain's length will be the remainder of the above value from
  the given chain's original length.

  So, if:
  DSMI_DS_LARGE_ITEM_SIZ is 668
  orig_chain has a length of DSMI_DS_LARGE_ITEM_SIZ*10 + 5
  Then the following call:

  hsu_al_dsm_utils_split_dsm_chain(DSM_DS_LARGE_ITEM_POOL, orig_chain, 1400)

  Would cause orig_chain to be trimmed to contain only it's first two DSM items,
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

==============================================================================*/
boolean hsu_al_dsm_utils_split_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_item_type*            orig_dsm_chain_ptr,
  dsm_item_type**           dest_dsm_chain_ptr_ptr,
  uint32                    amount_to_leave_in_orig_chain_in_bytes
);

#endif /* _HSU_AL_DSM_UTILS_H_ */
