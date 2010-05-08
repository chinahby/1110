#ifndef _HSU_AL_DMOV_API_H_
#define _HSU_AL_DMOV_API_H_

/*==============================================================================

  DMOV API for High Speed USB Adaptation Layer --- H E A D E R   F I L E
  
  GENERAL DESCRIPTION
  Contains data structures and API for a Data Mover wrapper, allowing
  operations of gathering/scattering between contiguous buffers and DSM chains.
  
  EXTERNALIZED FUNCTIONS
  hsu_al_dmov_api_init
  hsu_al_dmov_api_gather_dsm_chain
  hsu_al_dmov_api_scatter_dsm_chain
    
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_al_dmov_api_init must be called prior to usage of any other functions
  exposed by this API.
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_dmov_api.h#8 $
    
    when     who  what, where, why
    -------- ---  ---------------------------------------------------------
    05/21/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

  INCLUDE FILES FOR MODULE
  
==============================================================================*/
#if (defined FEATURE_HS_USB_TLP) && !(defined FEATURE_HS_USB_ECM)
#error FEATURE_HS_USB_TLP is defined without FEATURE_HS_USB_ECM defined!!!
#endif

#include "dsm.h"

#if !defined(T_REXNT) && defined(FEATURE_DMOV)
#include "dmov.h"
#endif
/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef T_REXNT
#error code not present
#endif

/* Maximal length of a source/destination descriptor representing a DSM chain 
for gather/scatter operation */
/* Note: DMOV_MAX_NUM_DSCR is not per uint32 but rather per the pair: 
Ptr to data and size of data (see: buff_addr_and_length_pair_for_dmov_type) */
#if defined(FEATURE_DMOV) || defined (T_REXNT)
#define CHAIN_DESC_ARRAY_SIZE  (DMOV_MAX_NUM_DSCR)
#endif

/*==============================================================================
Typedefs
==============================================================================*/
/* Type of the array that will hold for each dsm chain (packet) that 
   is gathered it's size. Used in TLP gathering only*/
typedef struct 
{
  uint16 chain_size;
  dsm_item_type *chain_ptr;
} packet_size_and_addr_in_dest_buff_pair_type;

/* Callback which allows its client not only to provide user data, 
but also receive back the DSM item chain and buffer which were used
in the DMOV operaion, which resulted in this callback. In addition
it also indicates whether the operation was successful or not*/
typedef void (*hsu_al_dmov_api_cbk_type)(boolean operation_succeeded,
                                         dsm_item_type* dsm_chain_ptr,
                                         void* buffer_ptr,
                                         void* usr_data);

/* Callback which allows its client not only to provide user data, 
but also receive back the DSM item chain and buffer which were used
in the DMOV operaion, which resulted in this callback. In addition
it also indicates whether the operation was successful or not*/
typedef void (*hsu_al_dmov_api_tlp_cbk_type)(boolean operation_succeeded,
              dsm_watermark_type *src_queue_to_gather,
              void* buffer_ptr,
              uint32 num_bytes_copied,
              packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
              dsm_item_type **dsm_chains_array_for_tlp_scatter,
              void* user_data);

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_dmov_api_init

DESCRIPTION
Initializes inner data structures, in order to allow proper usage of the API.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS


===========================================================================*/
void hsu_al_dmov_api_init
(
  void
);

/*===========================================================================

FUNCTION hsu_al_dmov_api_gather_dsm_chain

DESCRIPTION
  Aggregates a DSM item chain into a contiguous buffer. 
  Upon completion, a supplied callback is invoked bearing the result of the 
  operation as well as passing user given data.
  
DEPENDENCIES
  None
    
RETURN VALUE
  TRUE - Operation successful. FALSE - Otherwise
  
SIDE EFFECTS
  
  
===========================================================================*/
boolean hsu_al_dmov_api_gather_dsm_chain
(
  dsm_item_type* chain_to_aggregate, 
  /**< DSM chain to aggregate */
  void* dest_buffer, 
  /**< Contiguous buffer to "DMA" into */
  uint32 dest_buffer_size, 
  /**< size of the above buffer in bytes */
  hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func, 
  /**< Callback func to invoke when DMOV API is done with this operation */
  void* dmov_xfer_complete_cb_user_data
  /**< Data passed to the above callback, when invoked */
);

#ifdef FEATURE_HS_USB_TLP
/*===========================================================================

FUNCTION hsu_al_dmov_api_gather_dsm_chain_for_tlp

DESCRIPTION
Aggregates all the DSM item chains from a given wmq into a contiguous buffer, 
leaving a 2 bytes gap between the chains. 
Upon completion, a supplied callback is invoked bearing the result of the 
operation as well as passing user given data.

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise

SIDE EFFECTS


===========================================================================*/
boolean hsu_al_dmov_api_gather_dsm_queue_for_tlp
(
 dsm_watermark_type *src_queue_to_gather,
 /**< DSM watermark queue to aggregate */
 void* dest_buffer,
 /**< Contiguous buffer to "DMA" into */
 uint32 dest_buffer_size,
 /**< size of the above buffer in bytes */
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
 /**< array that will hold the sizes of the dsm chains (packets) that were "DMA"ed*/
 uint32 packet_sizes_array_size,
 /**< size of the array above in bytes*/
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func,
 /**< Callback func to invoke when DMOV API is done with this operation */
 void* dmov_xfer_complete_cb_user_data,
 /**< Data passed to the above callback, when invoked */
 uint16 tlp_header_size
 );

/*===========================================================================

FUNCTION hsu_al_dmov_api_scatter_dsm_chain_for_tlp

DESCRIPTION
Scatters a contiguous buffer into one or several DSM item chains.
The DSM chains are allocated by this function from the given pool.
Upon completion, a supplied callback is invoked bearing the result of the 
operation as well as passing user given data.    
DEPENDENCIES

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise

SIDE EFFECTS

===========================================================================*/

boolean hsu_al_dmov_api_scatter_dsm_chain_for_tlp
(
 void* src_buffer,
 /**< Contiguous buffer to scatter */
 uint32 src_buffer_size,
 /**< Size in bytes of above buffer */
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func,
 /**< Callback func to invoke when DMOV API is done with this operation */
 void* dmov_xfer_complete_cb_user_data,
 /**< Data passed to the above callback, when invoked */
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 /**< this aray will hold the dsm chains that were scattered*/
 uint32 dsm_chains_array_size,
 /**< Size of above array */
 uint16 tlp_header_size,
 /**< size of the TLP header*/
 dsm_mempool_id_enum_type  pool_id,  
 /**<pool id to allocate dsm chains from*/
 uint16 max_segment_size
 /**<max packet size that can be transfered (Ususaly size of Ethernet frame)*/
 );
#endif /*FEATURE_HS_USB_TLP*/

/*===========================================================================

FUNCTION hsu_al_dmov_api_scatter_dsm_chain

DESCRIPTION
    Scatters a contiguous buffer into a DSM item chain.
    Upon completion, a supplied callback is invoked bearing the result of the 
    operation as well as passing user given data.    
DEPENDENCIES
      
RETURN VALUE
  TRUE - Operation successful. FALSE - Otherwise
    
SIDE EFFECTS


===========================================================================*/
boolean hsu_al_dmov_api_scatter_dsm_chain
(
  void* src_buffer,
  /**< Contiguous buffer to scatter */
  uint32 src_buffer_size,
  /**< Size in bytes of above buffer */
  dsm_item_type* chain_to_scatter,
  /**< DSM chain to scatter into */
  hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func, 
  /**< Callback func to invoke when DMOV API is done with this operation */
  void* dmov_xfer_complete_cb_user_data
  /**< Data passed to the above callback, when invoked */
);

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

#endif /* _HSU_AL_DMOV_API_H_ */

