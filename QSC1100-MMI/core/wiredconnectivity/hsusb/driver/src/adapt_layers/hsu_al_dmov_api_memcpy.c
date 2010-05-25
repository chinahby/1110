/*==============================================================================

DMOV API for High Speed USB Adaptation Layers - Implementation File For Targets 
NOT Using Data Mover

GENERAL DESCRIPTION
Contains data structures and API for a Data Mover wrapper, allowing
operations of gathering/scattering between contiguous buffers and DSM chains.

EXTERNALIZED FUNCTIONS
hsu_al_dmov_api_init
hsu_al_dmov_api_gather_dsm_chain
hsu_al_dmov_api_scatter_dsm_chain
hsu_al_dmov_api_gather_dsm_queue_for_tlp
hsu_al_dmov_api_scatter_dsm_chain_for_tlp

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_dmov_api_init must be called prior to usage of any other functions
exposed by this API.


Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_dmov_api_memcpy.c#6 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/08/06  esh     First revision of file

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_dmov_api.h"

#include "dsm.h"

#include "hsu_common.h"

#include "hsu_al_dsm_utils.h"


/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static void invoke_dmov_callback
(
 hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func, 
 dsm_item_type* dsm_chain_ptr,
 void* buffer_ptr,
 void* dmov_xfer_complete_cb_user_data
 );

#ifdef FEATURE_HS_USB_TLP
static void invoke_dmov_callback_for_tlp
(
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func, 
 dsm_watermark_type *        dsm_watermark_ptr,
 void* buffer_ptr,
 uint32 total_length,
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 void* dmov_xfer_complete_cb_user_data
 );
#endif /*FEATURE_HS_USB_TLP*/

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================
FUNCTION hsu_al_dmov_api_init()
===========================================================================*/
void hsu_al_dmov_api_init
(
 void
 )
{
}

/*===========================================================================
FUNCTION hsu_al_dmov_api_gather_dsm_chain()
===========================================================================*/
boolean hsu_al_dmov_api_gather_dsm_chain
(
 dsm_item_type* src_chain_to_gather,
 void* dest_buffer,
 uint32 dest_buffer_size,
 hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func,
 void* dmov_xfer_complete_cb_user_data
 )
{
  dsm_item_type* current_item = NULL;
  uint32 src_chain_length;
  uint32 bytes_copied = 0;

  if ((src_chain_to_gather == NULL) || (dest_buffer == NULL))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_chain: At least one of the args is NULL:\
                  src_chain_to_gather=0x%x, dest_buffer=0x%x, dmov_xfer_complete_cb_func=0x%x",
                  src_chain_to_gather, dest_buffer, dmov_xfer_complete_cb_func);
    return FALSE;
  }

  src_chain_length = dsm_length_packet(src_chain_to_gather);
  if (src_chain_length == 0)
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_chain: src_chain_to_gather length is zero!",
      0, 0, 0);
    return FALSE;
  }

  if (dest_buffer_size < src_chain_length)
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_chain: \
                  dest_buffer_size (%u) is smaller than src_chain_to_gather (%u)!",
                  dest_buffer_size, src_chain_length, 0);
    return FALSE;
  }

  /* src_chain_to_gather was already checked, so it is != NULL */
  current_item = src_chain_to_gather;

  do 
  {
    if (current_item == NULL || current_item->data_ptr == NULL || 
      current_item->used == 0)
    {
      HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_chain:\
                    while copying the DSM chain, \
                    encountered an item with bad param(s): NULL, NULL, ZERO:\
                    current_item=0x%x, current_item->data_ptr=0x%x, \
                    current_item->used=%u",
                    current_item, current_item->data_ptr, current_item->used);
      return FALSE;
    }

    memcpy((void*)(((uint32)dest_buffer)+bytes_copied), current_item->data_ptr, 
      current_item->used);

    bytes_copied += current_item->used;

    /* Go to next packet */
    current_item = current_item->pkt_ptr;
  } while (bytes_copied < src_chain_length);

  invoke_dmov_callback(dmov_xfer_complete_cb_func, src_chain_to_gather, 
    dest_buffer, dmov_xfer_complete_cb_user_data);

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_dmov_api_scatter_dsm_chain()
===========================================================================*/
boolean hsu_al_dmov_api_scatter_dsm_chain
(
 void* src_buffer,
 uint32 src_buffer_size,
 dsm_item_type* dest_chain_to_scatter,
 hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func,
 void* dmov_xfer_complete_cb_user_data
 )
{
  uint32 dest_chain_to_scatter_capacity;
  dsm_item_type* current_item = NULL;
  uint32 bytes_copied = 0;

  if ((src_buffer == NULL) || (src_buffer_size == 0) 
    || (dest_chain_to_scatter == NULL))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain:\
                  At least one of the args is illegal (NULL, NON-ZERO, NULL):\
                  src_buffer=0x%x, src_buffer_size=%u, dest_chain_to_scatter=0x%x",
                  src_buffer, src_buffer_size, dest_chain_to_scatter);
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain:\
                  At least one of the args is illegal (cont) (NULL):\
                  dmov_xfer_complete_cb_func=0x%x", dmov_xfer_complete_cb_func, 0, 0);
    return FALSE;
  }

  dest_chain_to_scatter_capacity = 
    hsu_al_dsm_utils_get_dsm_chain_capacity(dest_chain_to_scatter);

  if (src_buffer_size > dest_chain_to_scatter_capacity)
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain: src_buffer_size (%u) \
                  is bigger than dest_chain_to_scatter (%u)", src_buffer_size, 
                  dest_chain_to_scatter_capacity, 0);
    return FALSE;
  }

  if ((dest_chain_to_scatter_capacity - src_buffer_size) >= 
    dest_chain_to_scatter->size)
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain: Given chain to scatter \
                  into is longer than buffer by more than one DSM item size: \
                  its capacity (%u), src_buffer_size (%u), sizeof 1 DSM item (%u)",
                  dest_chain_to_scatter_capacity, src_buffer_size, 
                  dest_chain_to_scatter->size);
    return FALSE;
  }

  /* dest_chain_to_scatter was already checked, so it is != NULL */
  current_item = dest_chain_to_scatter;

  do 
  {
    uint16 how_much_to_copy = 0;

    if (current_item == NULL || current_item->data_ptr == NULL 
      || current_item->used != 0) /* DSM item ought to be empty ! */
    {
      HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain:\
                    while copying to the DSM chain, \
                    encountered an item with bad param(s): NULL, NULL, NON-ZERO:\
                    current_item=0x%x, current_item->data_ptr=0x%x, \
                    current_item->used=%u",
                    current_item, current_item->data_ptr, current_item->used);
      return FALSE;
    }

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

  invoke_dmov_callback(dmov_xfer_complete_cb_func, dest_chain_to_scatter, 
    src_buffer, dmov_xfer_complete_cb_user_data);

  return TRUE;
}

#ifdef FEATURE_HS_USB_TLP
/*===========================================================================
FUNCTION hsu_al_dmov_api_gather_dsm_queue_for_tlp()
===========================================================================*/
boolean hsu_al_dmov_api_gather_dsm_queue_for_tlp
(
 dsm_watermark_type *src_queue_to_gather,
 void* dest_buffer,
 uint32 dest_buffer_size,
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
 uint32 packet_sizes_array_size,
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func,
 void* dmov_xfer_complete_cb_user_data,
 uint16 tlp_header_size
 )
{

  uint32 bytes_copied = 0;
  dsm_item_type* current_item = NULL;
  dsm_item_type* curr_chain_ptr = NULL; /*Pointer to the current dsm chain*/  
  uint16 curr_chain_length = 0;
  uint32 num_of_chains_in_wmq = 0;

  uint32 src_queue_total_length = 0;  /* This variable holds the total number 
                                      of bytes that was gathered into the continuous
                                      buffer, including the 2 bytes gap that was 
                                      saved for the packet size*/

  if ((src_queue_to_gather == NULL) || (dest_buffer == NULL) 
    || (dmov_xfer_complete_cb_func == NULL))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp: At least one of the args is NULL:\
                  src_chain_to_gather=0x%x, dest_buffer=0x%x, dmov_xfer_complete_cb_func=0x%x",
                  src_queue_to_gather, dest_buffer, dmov_xfer_complete_cb_func);
    return FALSE;
  }

  if (dsm_is_wm_empty(src_queue_to_gather))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp: src_queue_to_gather is empty!",
      0, 0, 0);
    return FALSE;
  }

  if ((packet_sizes_array == NULL) || (packet_sizes_array_size == 0))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp: packet_sizes_array isnt valid!",
      0, 0, 0);
    return FALSE;
  }

  curr_chain_ptr = dsm_dequeue(src_queue_to_gather);
  curr_chain_length = dsm_length_packet(curr_chain_ptr);
  /* We check against (curr_chain_length + 2) because TLP header (pkt length)
  is 2 bytes long */
  if (dest_buffer_size < (uint32)(curr_chain_length + tlp_header_size))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp: \
                  dest_buffer_size (%u) is smaller than the first dsm chain to gather (%u)!",
                  dest_buffer_size, curr_chain_length, 0);
    return FALSE;
  }

  /* Go over all the dsm chains in src_queue_to_gather*/
  do {  
    /*Save the current dsm_chain size */
    packet_sizes_array[num_of_chains_in_wmq].chain_ptr = curr_chain_ptr;
    packet_sizes_array[num_of_chains_in_wmq].chain_size = curr_chain_length;

    num_of_chains_in_wmq++;
    /*Size of the length field is 2 bytes*/
    src_queue_total_length += tlp_header_size;

    /* curr_chain_ptr was already checked by 
    dsm_is_wm_empty(src_queue_to_gather), so it is != NULL */
    current_item = curr_chain_ptr;
    bytes_copied = 0;

    /*Copy one chain to the destination buffer*/
    do 
    {
      if (current_item == NULL || current_item->data_ptr == NULL || 
        current_item->used == 0)
      {
        HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp:\
                      while copying the DSM chain, \
                      encountered an item with bad param(s): NULL, NULL, ZERO:\
                      current_item=0x%x, current_item->data_ptr=0x%x, \
                      current_item->used=%u",
                      current_item, current_item->data_ptr, current_item->used);
        return FALSE;
      }

      memcpy((void*)((uint32)dest_buffer + src_queue_total_length), 
        current_item->data_ptr, current_item->used);

      bytes_copied += current_item->used;
      src_queue_total_length += current_item->used;

      /* Go to next packet */
      current_item = current_item->pkt_ptr;
    } while (bytes_copied < curr_chain_length);

    if (bytes_copied > curr_chain_length)
    {
      HSU_MSG_ERROR("hsu_al_dmov_api_gather_dsm_queue_for_tlp:\
                    bytes_copied > curr_chain_length",0,0,0);
      return FALSE;
    }

    /*Check that there is room for one more dsm_chain in the buffer*/
    curr_chain_ptr = hsu_al_dsm_utils_peek_wmq(src_queue_to_gather);
    if (curr_chain_ptr != NULL)
    {
      curr_chain_length = dsm_length_packet(curr_chain_ptr);
      if (src_queue_total_length + tlp_header_size + curr_chain_length > dest_buffer_size)
      {
        break;
      }
      else
      {
        curr_chain_ptr = dsm_dequeue(src_queue_to_gather);
      }
    }
  } while((curr_chain_ptr != NULL) && (packet_sizes_array_size > num_of_chains_in_wmq));

  packet_sizes_array[num_of_chains_in_wmq].chain_size = 0;

  invoke_dmov_callback_for_tlp(dmov_xfer_complete_cb_func,src_queue_to_gather,
    dest_buffer,src_queue_total_length,packet_sizes_array,NULL,dmov_xfer_complete_cb_user_data);

  return TRUE;
}


/*===========================================================================
FUNCTION hsu_al_dmov_api_scatter_dsm_chain_for_tlp()
===========================================================================*/
boolean hsu_al_dmov_api_scatter_dsm_chain_for_tlp
(
 void* src_buffer,
 uint32 src_buffer_size,
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func,
 void* dmov_xfer_complete_cb_user_data,
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 uint32 dsm_chains_array_size,
 uint16 tlp_header_size,
 dsm_mempool_id_enum_type  pool_id,
 uint16 max_segment_size
 )
{
  uint16 curr_chain_size = 0 ;  /*Size of the current chain we are taking care 
                                of. This value is read from the 2 bytes header
                                that is added to each Ethernet packet in the 
                                received buffer */
  uint32 curr_chain_offset_in_src_buffer = 0; /*offset from the beginning of the 
                                              buffer, pointing to the current 
                                              Ethernet frame we are working with*/

  uint32 tmp_chain_length = 0;  /*helping variable. holding current chain length*/

  dsm_item_type* curr_chain_to_scatter = NULL; /*The dsm chain we are scattering*/

  uint32  num_of_chains_in_src_buffer = 0;   /* Number of dsm chains we are 
                                             about to scatter from the buffer */
  uint32 i = 0;

  dsm_item_type* current_item = NULL;

  if ((src_buffer == NULL) || (src_buffer_size == 0) 
    || (dsm_chains_array_for_tlp_scatter == NULL) || (dmov_xfer_complete_cb_func == NULL) ||
    !(tlp_header_size > 0) || !(max_segment_size > 0))
  {
    HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp:\
                  At least one of the args is illegal (NULL, NON-ZERO, NULL):\
                  src_buffer=0x%x, src_buffer_size=%u, dsm_chains_array_for_tlp_scatter=0x%x",
                  src_buffer, src_buffer_size, dsm_chains_array_for_tlp_scatter);
    HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp:\
                  At least one of the args is illegal (cont) (NULL):\
                  dmov_xfer_complete_cb_func=0x%x, tlp_header_size=%u, max_segment_size=%u", 
                  dmov_xfer_complete_cb_func, tlp_header_size, max_segment_size);
    return FALSE;
  }

  /*Do for each chain in the buffer */
  do 
  {
    curr_chain_size = *(uint16*)((uint32)src_buffer + curr_chain_offset_in_src_buffer);
    if (curr_chain_size > max_segment_size)
    {
      HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp(): Received frame (size = %d) bigger than"
        " max segment size (%d)",curr_chain_size,max_segment_size,0);
      curr_chain_offset_in_src_buffer += tlp_header_size + curr_chain_size;
    }
    else
    {
      tmp_chain_length = 0;  
      /*Allocate chain*/
      curr_chain_to_scatter = hsu_al_dsm_utils_generate_dsm_chain(
        pool_id, curr_chain_size);  
      if(curr_chain_to_scatter == NULL)
      {
        HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp(): OUT of DSM items",0,0,0);
        return FALSE;
      }

      curr_chain_offset_in_src_buffer += tlp_header_size ;
      /* now src_buffer + curr_chain_offset_in_src_buffer is pointing to the
      actual data to scatter */

      /*Check that there is no overflow in the buffer*/
      if ((curr_chain_offset_in_src_buffer + curr_chain_size) > 
        src_buffer_size)
      {
        HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp: src buffer overflow",0,0,0);
        /*Free allocated dsm_chains */
        for (i = 0; i < num_of_chains_in_src_buffer; i++)
        {
          dsm_free_packet(&dsm_chains_array_for_tlp_scatter[i]);
        }
        dsm_free_packet(&curr_chain_to_scatter);
        return FALSE;
      }

      dsm_chains_array_for_tlp_scatter[num_of_chains_in_src_buffer++] 
      = curr_chain_to_scatter;
      /* curr_chain_to_scatter was already checked, so it is != NULL */
      current_item = curr_chain_to_scatter;

      /*Check src_buffer owerflow*/
      if (num_of_chains_in_src_buffer == dsm_chains_array_size)
      {
        HSU_MSG_ERROR("*_scatter_dsm_chain_for_tlp: \
                      dsm_chains_array_for_tlp_scatter is too small to hold all \
                      the received packets",0,0,0);
        /*Free allocated dsm_chains */
        for (i = 0; i < num_of_chains_in_src_buffer; i++)
        {
          dsm_free_packet(&dsm_chains_array_for_tlp_scatter[i]);
        }
        return FALSE;
      }

      /* scatter current chain */
      do
      {
        /* Update the ->used field*/
        if (current_item->pkt_ptr == NULL)
        {
          /* This is the last DSM item, its ->used field is:
          Size of (DSM chain length - tmp_chain_length) because the whole chain 
          size is curr_chain_size:*/
          current_item->used = 
            ((uint16)(curr_chain_size - tmp_chain_length));
        }
        else /* current_item->pkt_ptr != NULL */
        {
          current_item->used = current_item->size;
        }

        tmp_chain_length += current_item->used;

        memcpy(current_item->data_ptr, 
          (void*)((uint32) src_buffer + curr_chain_offset_in_src_buffer), 
          current_item->used);

        curr_chain_offset_in_src_buffer += current_item->used;

        if (current_item->pkt_ptr == NULL)
        {
        break;
        }
        
        current_item = current_item->pkt_ptr;
      }	while(tmp_chain_length < curr_chain_size);
    }
  } while(curr_chain_offset_in_src_buffer < src_buffer_size);

  if (num_of_chains_in_src_buffer == 0)
  {
     HSU_MSG_ERROR("*:No valid chains in buffer",0,0,0);
     return FALSE;
  }

  dsm_chains_array_for_tlp_scatter[num_of_chains_in_src_buffer] = 0;

  invoke_dmov_callback_for_tlp(dmov_xfer_complete_cb_func,NULL,src_buffer,
    src_buffer_size,NULL,dsm_chains_array_for_tlp_scatter,
    dmov_xfer_complete_cb_user_data);

  return TRUE;
}
#endif /*FEATURE_HS_USB_TLP*/
/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION invoke_dmov_callback

DESCRIPTION
Causes the internal DMOV callback to be invoked, typically when DMOV is simulated.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void invoke_dmov_callback
(
 hsu_al_dmov_api_cbk_type dmov_xfer_complete_cb_func, 
 dsm_item_type* dsm_chain_ptr,
 void* buffer_ptr,
 void* dmov_xfer_complete_cb_user_data
 )
{
  /* Allow hsu_al_dmov_api_gather_dsm_chain & hsu_al_dmov_api_scatter_dsm_chain
     to be called without any callback (simplifies implementation) */
  if (dmov_xfer_complete_cb_func == NULL)
  {
    return;
  }

  /* All arguments were checked in the functions invoking this one,
  so these checks are hypothetical */
  HSU_ASSERT(dsm_chain_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(dmov_xfer_complete_cb_user_data != NULL);

  dmov_xfer_complete_cb_func(TRUE, dsm_chain_ptr, buffer_ptr, 
    dmov_xfer_complete_cb_user_data);
}

#ifdef FEATURE_HS_USB_TLP
/*===========================================================================

FUNCTION invoke_dmov_callback_for_tlp

DESCRIPTION
Causes the internal DMOV callback to be invoked, typically when DMOV is simulated.
Used for TLP function callbacks

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void invoke_dmov_callback_for_tlp
(
 hsu_al_dmov_api_tlp_cbk_type dmov_xfer_complete_cb_func, 
 dsm_watermark_type *        dsm_watermark_ptr,
 void* buffer_ptr,
 uint32 total_length,
 packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array,
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 void* dmov_xfer_complete_cb_user_data
 )
{
  /* All arguments were checked in the functions invoking this one,
  so these checks are hypothetical */
  HSU_ASSERT(dmov_xfer_complete_cb_func != NULL);
  HSU_ASSERT(dsm_watermark_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(dmov_xfer_complete_cb_user_data != NULL);

  dmov_xfer_complete_cb_func(TRUE, dsm_watermark_ptr, buffer_ptr,
    total_length, packet_sizes_array, dsm_chains_array_for_tlp_scatter,
    dmov_xfer_complete_cb_user_data);
}
#endif /*FEATURE_HS_USB_TLP*/





