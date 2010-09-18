/*==============================================================================

DMOV API for High Speed USB Adaptation Layers - Implementation File For Targets 
                                                Using Data Mover

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

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_dmov_api.c#5 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/08/06  esh     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_dmov_api.h"

#include "comdef.h" /* For ALIGN() macro */

#include "rex.h"

#include "dsm.h"

#include "dmov.h"

#include "hsu_common.h"

#include "hsu_al_dsm_utils.h"
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
/* The maximal number of jobs the various ALs can enqueue when all are using one 
DMOV channel */
#define MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN \
  (DMOV_MAX_NUM_TRANSFERS_QUEUED / DMOV_NUM_CHANS)

/* Macros wrapping DMOV macros: */
/* Marks end of data for DMOV */
#define AL_DMOV_API_END_OF_DATA (DMOV_FIELD(DSCR_WORD1, LD, 1))
/* Marks start of command descriptor for DMOV */
#define AL_DMOV_API_START_OF_CMD (DMOV_FIELD(CMD_WORD, LC, 1))
/* Marks command type of scatter-gather for command descriptor for DMOV */
#define AL_DMOV_API_CMD_SCATTER_GATHER (AL_DMOV_API_START_OF_CMD | \
  DMOV_FIELD(CMD_WORD, ADDR_MODE, DMOV_ADDR_MODE_SCATTER_GATHER))
/* The typical value for the "reserved" field */
/* Note: It is assumed DMOV HW version 2.0 or above 
(otherwise this macro should be zero) */
#define AL_DMOV_API_SCATTER_GATHER_RESERVED (DMOV_FIELD(SG_INDEX, LI, 1) | \
  DMOV_FIELD(SG_INDEX, SRC_DSCR_INDX, 0) | DMOV_FIELD(SG_INDEX, DST_DSCR_INDX, 0))

#ifdef T_REXNT
#error code not present
#endif
/*==============================================================================
Typedefs
==============================================================================*/
typedef struct
{
  uint32 buff_addr;

  uint32 buff_length;

} buff_addr_and_length_pair_for_dmov_type;
  /* For a buffer to be used in a DMOV descriptor array, this represents a pair 
  of buffer's address and buffer's length */

typedef struct  
{
  /* Ptr to DSM chain descriptor array (see gather_scatter_chain_descs) */
  buff_addr_and_length_pair_for_dmov_type* chain_desc_array_ptr;

  /* Ptr to contiguous buffer descriptor (see gather_scatter_cont_buffs_descs) */
  buff_addr_and_length_pair_for_dmov_type* buff_desc_ptr;

  /* Ptr to command descriptor (see gather_scatter_cmds) */
  dmov_cmd_scatter_gather_s_type* cmd_desc_ptr;

  /* The DSM chain used by the DMOV */
  dsm_item_type*            dsm_item_chain_ptr; 

  /* The contiguous buffer used by the DMOV */
  void*                     buffer; 

  hsu_al_dmov_api_cbk_type  user_callback;

  void*                     user_data;

  /* Ptr to DSM chain descriptor array (see gather_scatter_chain_descs) */
  buff_addr_and_length_pair_for_dmov_type* chain_desc_array_ptr_for_tlp;
  /* Ptr to packet sizes array - used for TLP gathering */
  packet_size_and_addr_in_dest_buff_pair_type* packet_sizes_array;
  /* Ptr to the dsm chains array - used for TLP scattering*/
  dsm_item_type                 **dsm_chains_array_for_tlp_scatter;   
  hsu_al_dmov_api_tlp_cbk_type  user_callback_tlp;
  /* Total length (in bytes) of the data that was scatered/gathered */
  uint32        total_length;
  /* The DSM watermark queue used by the DMOV for TLP */
  dsm_watermark_type *        dsm_watermark_ptr  ;
  /* This flag indicated whether the operation performed was on a TLP buffer*/
  boolean                  tlp_gather_or_scatter;
} params_for_dmov_type; 
  /* Represents params needed by the DMOV */

/*==============================================================================
Variables
==============================================================================*/

/* The DMOV engine imposes the following alignment requirements:
- The data representing the "transfer command descriptor" must be 64-bit (8 byte) 
aligned.
- Though the following is not a requirement, it was added for consistency:
The descriptors used for source and destination shall also be 64-bit aligned.

Therefore, in params_for_dmov_type there are fields which point to the following 
8-byte aligned buffers, based on the used index which range is:
[0..MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN)
*/

static ALIGN(8) dmov_cmd_scatter_gather_s_type \
  gather_scatter_cmds[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN];

static ALIGN(8) buff_addr_and_length_pair_for_dmov_type \
  gather_scatter_cont_buffs_descs[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN];

static ALIGN(8) buff_addr_and_length_pair_for_dmov_type \
  gather_scatter_chain_descs[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN][CHAIN_DESC_ARRAY_SIZE];

#ifdef FEATURE_HS_USB_TLP
static ALIGN(8) buff_addr_and_length_pair_for_dmov_type \
  gather_scatter_chain_descs_for_tlp[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN][CHAIN_DESC_ARRAY_SIZE];
#endif /*FEATURE_HS_USB_TLP*/

static struct 
{
  /* This guards in case app #1 calls gather, and while updating params_array, 
  app #2 calls gather as well...*/
  rex_crit_sect_type params_array_access_guard;

  params_for_dmov_type params_array[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN];

  boolean params_array_entry_is_free[MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN];

  uint32 next_available_index;

  /* Allow the option of simulating DMOV via regular memcpy, for unit tests */
  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif /* FEATURE_HSU_TEST */

} hsu_al_dmov_api_static_data;
  /* This file's variable */

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static void init_params_array_crit_section_if_needed
(
  void
);

static void api_callback_for_dmov
(
  const dmov_result_s_type *result_ptr,
  void *user_ptr
);

static boolean param_array_index_is_valid
(
  uint32 param_array_index
);

static uint32 hsu_al_dmov_api_set_dmov_descriptors_for_gathering
(
 uint32 allocated_param_entry_index,
 uint32 *src_queue_total_length,
 void* dest_buffer,
 dsm_item_type* chain_ptr,
 uint32 desc_itor,
 boolean  fill_dest_array
 );

#ifdef FEATURE_HS_USB_TLP
static boolean hsu_al_dmov_api_set_dmov_descriptors_for_scattering
(
 void* src_buffer,
 uint32 allocated_param_entry_index,
 uint32 src_buffer_size,
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
 uint32 dsm_chains_array_size,
 dsm_mempool_id_enum_type  pool_id ,
 uint16 tlp_header_size,
 uint16 max_segment_size
 );
#endif /*FEATURE_HS_USB_TLP*/

static boolean perform_dmov_scatter_or_gather
(
  buff_addr_and_length_pair_for_dmov_type* src_desc_array_ptr, 
  buff_addr_and_length_pair_for_dmov_type* dest_desc_array_ptr,
  uint32  params_array_index
);

static uint32 alloc_param_array_index
(
  void
);

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

static uint32 hsu_al_dmov_api_set_dmov_descriptors_for_gathering
(
 uint32 allocated_param_entry_index,
 uint32 *src_queue_total_length,
 void* dest_buffer,
 dsm_item_type* current_item,
 uint32 desc_itor,
 boolean  fill_dest_array
 );
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
  int params_array_itor;
  params_for_dmov_type* param_array_entry_ptr = NULL;

  init_params_array_crit_section_if_needed();

  hsu_al_dmov_api_static_data.next_available_index = 0;
  
  for (params_array_itor = 0 ; 
    params_array_itor < MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN ;
    ++params_array_itor)
  {
    hsu_al_dmov_api_static_data.params_array_entry_is_free[params_array_itor] =
      TRUE;

    param_array_entry_ptr = 
      &(hsu_al_dmov_api_static_data.params_array[params_array_itor]);

    /* Fill the pointers to the 64-bit aligned descriptors: */
    param_array_entry_ptr->cmd_desc_ptr = 
      &(gather_scatter_cmds[params_array_itor]);
    param_array_entry_ptr->buff_desc_ptr = 
      &(gather_scatter_cont_buffs_descs[params_array_itor]);
    param_array_entry_ptr->chain_desc_array_ptr = 
      gather_scatter_chain_descs[params_array_itor];
#ifdef FEATURE_HS_USB_TLP
    param_array_entry_ptr->chain_desc_array_ptr_for_tlp = 
      gather_scatter_chain_descs_for_tlp[params_array_itor];
#endif /*FEATURE_HS_USB_TLP*/
  }

  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif /* FEATURE_HSU_TEST */
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
  uint32 allocated_param_entry_index;
  params_for_dmov_type* param_entry_ptr = NULL;

  uint32 desc_itor = 0;

  dsm_item_type* current_item = NULL;

  boolean result_of_dmov = FALSE;

  uint32 src_chain_length;

  if ((src_chain_to_gather == NULL) || (dest_buffer == NULL) 
      || (dmov_xfer_complete_cb_func == NULL))
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

  #ifdef FEATURE_HSU_TEST
#error code not present
  #endif /* FEATURE_HSU_TEST */

  /* The following code, from this point until after perform_dmov_scatter_or_gather,
  must not be interrupted by other tasks. This is due to the following scenario:

  1. Task A1 calls alloc_param_array_index, (so it receives slot i) 
  when it returns there is a context switch 
  (meaning A1 has not called perform_dmov_scatter_or_gather yet).
  2. Task A2 also calls alloc_param_array_index (so it receives slot 
  (i+1)mod(MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN)) 
  and continues execution to perform_dmov_scatter_or_gather.
  3. Now there are tasks A3..A5, doing the same 
  as task A2. The last one, A5, once it calls alloc_param_array_index, will run
  into an ASSERT, as it grabbed slot 
  (i-1)mod(MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN), 
  and is now verifying if slot i is avail; since task A1 still has not invoked 
  perform_dmov_scatter_or_gather, and therefore the slot is not freed yet. This 
  is because a slot is freed only when api_callback_for_dmov is invoked).
  */
  rex_enter_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  /* Grab next available param array entry */
  allocated_param_entry_index = alloc_param_array_index();
  param_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /* Save both the DSM chain and buffer: */
  param_entry_ptr->dsm_item_chain_ptr = src_chain_to_gather;
  param_entry_ptr->buffer = dest_buffer;

  param_entry_ptr->tlp_gather_or_scatter = FALSE;

  /* Save client callback and client data: */
  param_entry_ptr->user_callback = dmov_xfer_complete_cb_func;
  param_entry_ptr->user_data = dmov_xfer_complete_cb_user_data;
  param_entry_ptr->total_length = src_chain_length ;

  /* Set up src descriptors from dsm_item chain */
  desc_itor = hsu_al_dmov_api_set_dmov_descriptors_for_gathering(allocated_param_entry_index,
    NULL,NULL,current_item,desc_itor,FALSE);

  /* Assuming the source descriptor array is large enough for any DSM chain */
  HSU_ASSERT(desc_itor < CHAIN_DESC_ARRAY_SIZE);

  /* Now, desc_itor points to the buff_addr_and_length_pair_for_dmov_type of the last 
  item added to the destination descriptor array, so there is no need to 
  decrement desc_itor by one. */
  /* (param_entry_ptr->chain_desc_array_ptr)[desc_itor] now points to the last 
  item added to the src desc array, OR its length field it with DMOV end of data 
  value: */
  (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  /* Now set up the destination descriptor */
  (param_entry_ptr->buff_desc_ptr)->buff_addr = (uint32) dest_buffer;

  /* buff_length for the destination descriptor MUST be equal to the total 
  number of bytes in the src descriptor array (src_chain_length), or data mover 
  behavior is undefined. 
  We already made sure src_chain_length <= dest_buffer_size. */
  (param_entry_ptr->buff_desc_ptr)->buff_length = (uint32) src_chain_length; 

  /* OR the destination descriptor's length field with DMOV end of data value: */
  (param_entry_ptr->buff_desc_ptr)->buff_length |= AL_DMOV_API_END_OF_DATA;

  result_of_dmov = perform_dmov_scatter_or_gather(param_entry_ptr->chain_desc_array_ptr,
    param_entry_ptr->buff_desc_ptr, allocated_param_entry_index);

  /*If dmov failed free the index*/
  if (!result_of_dmov)
  {
    hsu_al_dmov_api_static_data.params_array_entry_is_free[allocated_param_entry_index] = TRUE;
  }
  /* Leave critical section */
  rex_leave_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  return result_of_dmov;
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
  uint32 allocated_param_entry_index;

  params_for_dmov_type* param_entry_ptr = NULL;

  uint32 desc_itor = 0;

  dsm_item_type* curr_chain_ptr = NULL; /*Pointer to the current dsm chain*/  
  uint16 curr_chain_length = 0;
  uint32 num_of_chains_in_wmq = 0;

  boolean result_of_dmov = FALSE;

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

  rex_enter_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  /* Grab next available param array entry */
  allocated_param_entry_index = alloc_param_array_index();
  param_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /* Save both the DSM chain and buffer: */
  param_entry_ptr->buffer = dest_buffer;

  /* Save client callback and client data: */
  param_entry_ptr->user_callback_tlp = dmov_xfer_complete_cb_func;
  param_entry_ptr->user_data = dmov_xfer_complete_cb_user_data;
  param_entry_ptr->packet_sizes_array = packet_sizes_array;
  param_entry_ptr->dsm_watermark_ptr = src_queue_to_gather;

  param_entry_ptr->tlp_gather_or_scatter = TRUE;

  desc_itor = 0;
  /* Go over all the dsm chains in src_queue_to_gather*/
  do {  
    /*TODO: add the code for hsu_test?*/
    /*Save the current dsm_chain size */
    packet_sizes_array[num_of_chains_in_wmq].chain_ptr = curr_chain_ptr;
    packet_sizes_array[num_of_chains_in_wmq].chain_size = curr_chain_length;

    num_of_chains_in_wmq++;
    /*Size of the length field is 2 bytes*/
    src_queue_total_length += tlp_header_size;

    /* Set up src&dest descriptors from dsm_item chain */
    desc_itor = hsu_al_dmov_api_set_dmov_descriptors_for_gathering(allocated_param_entry_index,
      &src_queue_total_length,dest_buffer,curr_chain_ptr,desc_itor,TRUE);

    /* Assuming the source descriptor array is large enough for any DSM chain */
    HSU_ASSERT(desc_itor < CHAIN_DESC_ARRAY_SIZE);

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
        desc_itor++;
      }
    }
  } while((curr_chain_ptr != NULL) && (packet_sizes_array_size > num_of_chains_in_wmq));

  packet_sizes_array[num_of_chains_in_wmq].chain_size = 0;
  /* Now, desc_itor points to the buff_addr_and_length_pair_for_dmov_type of the last 
  item added to the destination descriptor array, so there is no need to 
  decrement desc_itor by one. */
  /* (param_entry_ptr->chain_desc_array_ptr)[desc_itor] now points to the last 
  item added to the src desc array, OR its length field it with DMOV end of data 
  value: */
  (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  param_entry_ptr->total_length = src_queue_total_length ;
  /* OR the destination descriptor's length field with DMOV end of data value: */
  (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  result_of_dmov = perform_dmov_scatter_or_gather(param_entry_ptr->chain_desc_array_ptr,
    param_entry_ptr->chain_desc_array_ptr_for_tlp, allocated_param_entry_index);

  /*If dmov failed free the index*/
  if (!result_of_dmov)
  {
    hsu_al_dmov_api_static_data.params_array_entry_is_free[allocated_param_entry_index] = TRUE;
  }

  /* Leave critical section */
  rex_leave_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  return result_of_dmov;
}
#endif /*FEATURE_HS_USB_TLP*/

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
  uint32 allocated_param_entry_index;
  params_for_dmov_type* param_entry_ptr = NULL;

  uint32 desc_itor = 0;

  uint32 dest_chain_to_scatter_capacity;

  dsm_item_type* current_item = NULL;

  boolean result_of_dmov = FALSE;

  if ((src_buffer == NULL) || (src_buffer_size == 0) 
      || (dest_chain_to_scatter == NULL) || (dmov_xfer_complete_cb_func == NULL))
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

  #ifdef FEATURE_HSU_TEST 
#error code not present
  #endif /* FEATURE_HSU_TEST */

  /* The following code, from this point until after perform_dmov_scatter_or_gather,
  must not be interrupted by other tasks. See corresponding comment in 
  hsu_al_dmov_api_gather_dsm_chain.
  */
  rex_enter_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  /* Grab next available param array entry */
  allocated_param_entry_index = alloc_param_array_index();
  param_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /* Save both the DSM chain and buffer: */
  param_entry_ptr->dsm_item_chain_ptr = dest_chain_to_scatter;
  param_entry_ptr->buffer = src_buffer;

  param_entry_ptr->tlp_gather_or_scatter = FALSE;
   
  /* Save client callback and client data: */
  param_entry_ptr->user_callback = dmov_xfer_complete_cb_func;
  param_entry_ptr->user_data = dmov_xfer_complete_cb_user_data;

  /* Set up src descriptor: */
  (param_entry_ptr->buff_desc_ptr)->buff_addr = (uint32) src_buffer;
  (param_entry_ptr->buff_desc_ptr)->buff_length = (uint32) src_buffer_size; 
  /* OR the src descriptor's length field with DMOV end of data value: */
  (param_entry_ptr->buff_desc_ptr)->buff_length |= AL_DMOV_API_END_OF_DATA;

  /* Now set up the destination descriptor */
  for (desc_itor = 0 ; desc_itor < CHAIN_DESC_ARRAY_SIZE ; ++desc_itor)
  {
    (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_addr = 
      (uint32) current_item->data_ptr;
    /* DMOV does not update the ->used field, so this needs to be done now: */
    if (current_item->pkt_ptr == NULL)
    {
      /* This is the last DSM item, its ->used field is:
      Size of 1 DSM item - (DSM chain length - src. buffer size) :*/
      current_item->used = 
        current_item->size - \
          ((uint16)(dest_chain_to_scatter_capacity - src_buffer_size));

      (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length = 
        (uint32) current_item->used;
      break;
    }
    /* else: current_item->pkt_ptr != NULL */

    current_item->used = current_item->size;

    (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length = 
      (uint32) current_item->used;

    current_item = current_item->pkt_ptr;
  }

  /* Assuming the dest descriptor array is large enough for any DSM chain */
  HSU_ASSERT(desc_itor < CHAIN_DESC_ARRAY_SIZE);

  /* Now, desc_itor points to the buff_addr_and_length_pair_for_dmov_type of the last 
  item added to the destination descriptor array, so there is no need to 
  decrement desc_itor by one. */
  /* (param_entry_ptr->chain_desc_array_ptr)[desc_itor] now points to the last 
  item added to the src desc array, OR its length field it with DMOV end of data 
  value: */
  (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  result_of_dmov = perform_dmov_scatter_or_gather(param_entry_ptr->buff_desc_ptr, 
      param_entry_ptr->chain_desc_array_ptr, allocated_param_entry_index);

  /*If dmov failed free the index*/
  if (!result_of_dmov)
  {
    hsu_al_dmov_api_static_data.params_array_entry_is_free[allocated_param_entry_index] = TRUE;
  }

  /* Leave critical section */
  rex_leave_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  return result_of_dmov;
}

#ifdef FEATURE_HS_USB_TLP
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
  uint32 allocated_param_entry_index;
  params_for_dmov_type* param_entry_ptr = NULL;

  boolean result_of_dmov = FALSE;

  if ((src_buffer == NULL) || (src_buffer_size == 0) 
    || (dsm_chains_array_for_tlp_scatter == NULL) || (dmov_xfer_complete_cb_func == NULL) ||
    !(tlp_header_size > 0) || !(max_segment_size > 0))
  {
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain_for_tlp:\
                  At least one of the args is illegal (NULL, NON-ZERO, NULL):\
                  src_buffer=0x%x, src_buffer_size=%u, dsm_chains_array_for_tlp_scatter=0x%x",
                  src_buffer, src_buffer_size, dsm_chains_array_for_tlp_scatter);
    HSU_MSG_ERROR("hsu_al_dmov_api_scatter_dsm_chain_for_tlp:\
                  At least one of the args is illegal (cont) (NULL):\
                  dmov_xfer_complete_cb_func=0x%x, tlp_header_size=%u, max_segment_size=%u", 
                  dmov_xfer_complete_cb_func, tlp_header_size, max_segment_size);
    return FALSE;
  }

  rex_enter_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  /* Grab next available param array entry */
  allocated_param_entry_index = alloc_param_array_index();
  param_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /* Save both the DSM chain and buffer: */
  param_entry_ptr->dsm_chains_array_for_tlp_scatter = dsm_chains_array_for_tlp_scatter;
  param_entry_ptr->buffer = src_buffer;

  param_entry_ptr->tlp_gather_or_scatter = TRUE;

  /* Save client callback and client data: */
  param_entry_ptr->user_callback_tlp = dmov_xfer_complete_cb_func;
  param_entry_ptr->user_data = dmov_xfer_complete_cb_user_data;

  if (hsu_al_dmov_api_set_dmov_descriptors_for_scattering(src_buffer,
      allocated_param_entry_index,src_buffer_size,dsm_chains_array_for_tlp_scatter, 
      dsm_chains_array_size, pool_id, tlp_header_size, max_segment_size))
  {
    result_of_dmov = perform_dmov_scatter_or_gather(param_entry_ptr->chain_desc_array_ptr_for_tlp, 
      param_entry_ptr->chain_desc_array_ptr, allocated_param_entry_index);
    
    /*If dmov failed free the index*/
    if (!result_of_dmov)
    {
      hsu_al_dmov_api_static_data.params_array_entry_is_free[allocated_param_entry_index] = TRUE;
    }
  }
  else{
    /* update this entry to be no longer occupied: */
    hsu_al_dmov_api_static_data.params_array_entry_is_free[allocated_param_entry_index] = TRUE;
    /* Leave critical section */
    rex_leave_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));
    return FALSE;
  }

  /* Leave critical section */
  rex_leave_crit_sect(&(hsu_al_dmov_api_static_data.params_array_access_guard));

  return result_of_dmov;
}
#endif /*FEATURE_HS_USB_TLP*/

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION init_params_array_crit_section_if_needed

DESCRIPTION
Initializes the critical section variable used for preventing a race condition 
when more than one client is invoking the gather/scatter functions.
The data to protect against is merely the next available index
(hsu_al_dmov_api.next_available_index).
Note: The INT context (DMOV callback) does not use this variable.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Initializes a rex critical section once.
===========================================================================*/
static void init_params_array_crit_section_if_needed
(
  void
)
{
  static boolean params_array_crit_section_initialized = FALSE;

  if (params_array_crit_section_initialized == TRUE)
  {
    return ;
  }

  rex_init_crit_sect(
    &(hsu_al_dmov_api_static_data.params_array_access_guard));

  params_array_crit_section_initialized = TRUE;
}

/*===========================================================================

FUNCTION api_callback_for_dmov

DESCRIPTION
The DMOV callback which the "DMOV API" plants, to be invoked whenever DMOV is 
done with an operation. This callback simply invokes the client's callback
as well as updating the param_array structure (marking the entry which is related
to this invocation as free).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Updates the corresponding hsu_al_dmov_api_static_data.params_array_entry_is_free
entry as free.
===========================================================================*/
static void api_callback_for_dmov
(
  const dmov_result_s_type *result_ptr,
  void *user_ptr
)
{
  /* The following code is called from INT context */
  /* user_ptr is the index in the param_array */

  params_for_dmov_type* param_array_entry_ptr = NULL;
  boolean dmov_result_is_ok;

  HSU_ASSERT(
    param_array_index_is_valid((uint32)user_ptr) == TRUE);

  param_array_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[(uint32)user_ptr]);

  /* The following macro defined in dmov.h, uses the macro ERR without '('.
  Therefore, just for this line suppress lint error 123 */
  /*lint -e123*/
  dmov_result_is_ok = DMOV_RESULT_OK(result_ptr);
  /*lint +e123*/

#ifdef FEATURE_HS_USB_TLP
  if (param_array_entry_ptr->tlp_gather_or_scatter)
  {
    param_array_entry_ptr->user_callback_tlp(dmov_result_is_ok,
      param_array_entry_ptr->dsm_watermark_ptr, param_array_entry_ptr->buffer,
      param_array_entry_ptr->total_length,
      param_array_entry_ptr->packet_sizes_array,
      param_array_entry_ptr->dsm_chains_array_for_tlp_scatter,
      param_array_entry_ptr->user_data);
  }
  else
#endif /*FEATURE_HS_USB_TLP*/
  {
    param_array_entry_ptr->user_callback(dmov_result_is_ok,
      param_array_entry_ptr->dsm_item_chain_ptr, param_array_entry_ptr->buffer,
      param_array_entry_ptr->user_data);
  }

  /* Only when client has been notified, 
  update this entry to be no longer occupied: */
  hsu_al_dmov_api_static_data.params_array_entry_is_free[(uint32)user_ptr] 
    = TRUE;
}

#ifdef FEATURE_HS_USB_TLP
static boolean hsu_al_dmov_api_set_dmov_descriptors_for_scattering
(
 void* src_buffer,
 uint32 allocated_param_entry_index,
 uint32 src_buffer_size,
 dsm_item_type **dsm_chains_array_for_tlp_scatter,
  uint32 dsm_chains_array_size,
 dsm_mempool_id_enum_type  pool_id ,
 uint16 tlp_header_size
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

  uint32 i=0;
  uint32 desc_itor = 0;
  dsm_item_type* current_item = NULL;
  params_for_dmov_type* param_entry_ptr =  
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /*Do for each chain in the buffer */
  do 
  {
    curr_chain_size = *(uint16*)((uint32)src_buffer + curr_chain_offset_in_src_buffer); 
    if (curr_chain_size > max_segment_size)
    {
      HSU_MSG_ERROR("*_set_dmov_descriptors_for_scattering(): Received frame (size = %d) bigger than"
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
        HSU_MSG_ERROR("*_set_dmov_descriptors_for_scattering(): OUT of DSM items",0,0,0);
        return FALSE;
      }

      curr_chain_offset_in_src_buffer += tlp_header_size ;
      /* now src_buffer + curr_chain_offset_in_src_buffer is pointing to the
      actual data to scatter */

      /*Check that there is no overflow in the buffer*/
      if ((curr_chain_offset_in_src_buffer + curr_chain_size) > 
        src_buffer_size)
      {

      HSU_MSG_ERROR("*_set_dmov_descriptors_for_scattering: \src buffer overflow",0,0,0);
      /*Free allocated dsm_chains */
      for (i = 0; i < num_of_chains_in_src_buffer; i++)
      {
        dsm_free_packet(&dsm_chains_array_for_tlp_scatter[i]);
      }
      dsm_free_packet(&curr_chain_to_scatter);
        return FALSE;
      }

      dsm_chains_array_for_tlp_scatter[num_of_chains_in_src_buffer++] = curr_chain_to_scatter;
      /* curr_chain_to_scatter was already checked, so it is != NULL */
      current_item = curr_chain_to_scatter;

      /*Check src_buffer owerflow*/
      if (num_of_chains_in_src_buffer == dsm_chains_array_size)
      {
        HSU_MSG_ERROR(":dsm_chains_array_for_tlp_scatter is too small to hold all the received packets",0,0,0);
      /*Free allocated dsm_chains */
      for (i = 0; i < num_of_chains_in_src_buffer; i++)
      {
        dsm_free_packet(&dsm_chains_array_for_tlp_scatter[i]);
      }
        return FALSE;
      }

      /* Now set up the destination and the source descriptor */
      while(desc_itor < CHAIN_DESC_ARRAY_SIZE) 
      {
        /*dest*/
        (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_addr = 
          (uint32) current_item->data_ptr;
        /* DMOV does not update the ->used field, so this needs to be done now: */
        if (current_item->pkt_ptr == NULL)
        {
          /* This is the last DSM item, its ->used field is:
          Size of (DSM chain length - tmp_chain_length) because the whole chain 
          size is curr_chain_size:*/
          current_item->used = 
            ((uint16)(curr_chain_size - tmp_chain_length));

          (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length = 
            (uint32) current_item->used;
        }
        else /* current_item->pkt_ptr != NULL */
        {
          current_item->used = current_item->size;
        }

        tmp_chain_length += current_item->used;

        (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length = 
          (uint32) current_item->used;

        /* Set up src descriptor: */
        (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_addr = 
          (uint32) src_buffer + curr_chain_offset_in_src_buffer;
        (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_length = 
          current_item->used; 

        curr_chain_offset_in_src_buffer += current_item->used;

        if (current_item->pkt_ptr == NULL)
        {
          break;
        }
        desc_itor++;
        current_item = current_item->pkt_ptr;
      }	
      /* Assuming the dest descriptor array is large enough for any DSM chain */
      HSU_ASSERT(desc_itor < CHAIN_DESC_ARRAY_SIZE);
      if (curr_chain_offset_in_src_buffer < src_buffer_size)
      {
        desc_itor++;
      }
    }
  } while(curr_chain_offset_in_src_buffer < src_buffer_size);

  if (num_of_chains_in_src_buffer == 0)
  {
     HSU_MSG_ERROR("*:No valid chains in buffer",0,0,0);
     return FALSE;
  }
  /* Now, desc_itor points to the buff_addr_and_length_pair_for_dmov_type of the last 
  item added to the destination descriptor array, so there is no need to 
  decrement desc_itor by one. */
  /* (param_entry_ptr->chain_desc_array_ptr)[desc_itor] now points to the last 
  item added to the src desc array, OR its length field it with DMOV end of data 
  value: */
  (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  /* OR the src descriptor's length field with DMOV end of data value: */
  (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_length |= 
    AL_DMOV_API_END_OF_DATA;

  dsm_chains_array_for_tlp_scatter[num_of_chains_in_src_buffer] = 0;
  param_entry_ptr->dsm_chains_array_for_tlp_scatter = dsm_chains_array_for_tlp_scatter ;
  return TRUE;
}
#endif /*FEATURE_HS_USB_TLP*/
/*===========================================================================

FUNCTION hsu_al_dmov_api_set_dmov_descriptors_for_gathering

DESCRIPTION
todo

DEPENDENCIES
None

RETURN VALUE
desc_ittor - index of the last value we inserted to chain_desc_array_ptr 

SIDE EFFECTS
None
===========================================================================*/
static uint32 hsu_al_dmov_api_set_dmov_descriptors_for_gathering
(
 uint32 allocated_param_entry_index,
 uint32 *src_queue_total_length,
 void* dest_buffer,
 dsm_item_type* chain_ptr,
 uint32 desc_itor,
 boolean  fill_dest_array
 )
{
  dsm_item_type* current_item = chain_ptr;
  params_for_dmov_type* param_entry_ptr =  
    &(hsu_al_dmov_api_static_data.params_array[allocated_param_entry_index]);

  /* Set up src&dest descriptors from dsm_item chain */
  while(desc_itor < CHAIN_DESC_ARRAY_SIZE) 
  {
    /*src*/
    (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_addr = 
      (uint32) current_item->data_ptr;
    (param_entry_ptr->chain_desc_array_ptr)[desc_itor].buff_length = 
      (uint32) current_item->used;

    if (fill_dest_array == TRUE)
    {
      /*dest*/
      (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_addr = 
        (uint32) dest_buffer + *src_queue_total_length;
      (param_entry_ptr->chain_desc_array_ptr_for_tlp)[desc_itor].buff_length = 
        (uint32) current_item->used;
    }

    if (src_queue_total_length != NULL)
    {
      *src_queue_total_length += (uint32) current_item->used;
    }

    current_item = current_item->pkt_ptr;

    if (current_item == NULL)
    {
      /* No more items */
      break;
    }
    desc_itor++;
  }
  return desc_itor;
}

/*===========================================================================

FUNCTION param_array_index_is_valid

DESCRIPTION
Verifies if the given index is a valid one (in range, points to a non free entry).

DEPENDENCIES
None

RETURN VALUE
TRUE - Index is valid. FALSE - Otherwise.

SIDE EFFECTS
None
===========================================================================*/
static boolean param_array_index_is_valid
(
  uint32 param_array_index
)
{
  if (param_array_index >= MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN)
  {
    HSU_MSG_ERROR(
      "param_array_index_is_valid: index (%u) out of bounds [%u..%u]",
      param_array_index, 0, MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN);
    return FALSE;
  }

  if (hsu_al_dmov_api_static_data.params_array_entry_is_free[param_array_index]
    == TRUE)
  {
    HSU_MSG_ERROR(
      "param_array_index_is_valid: index (%u) corresponds to empty entry",
      param_array_index, 0, 0);
    return FALSE;    
  }
  
#if 0
  /* Removed due to flooding */
  HSU_MSG_LOW(
    "param_array_index_is_valid: index (%u) points to valid data", 
    param_array_index, 0, 0);
#endif /* 0 */

  return TRUE;
}

/*===========================================================================

FUNCTION perform_dmov_scatter_or_gather

DESCRIPTION
Performs code common to both gather and scatter operations which eventually 
calls dmov_transfer.

DEPENDENCIES
None

RETURN VALUE
TRUE - dmov_transfer successful. FALSE - Otherwise.

SIDE EFFECTS
None
===========================================================================*/
static boolean perform_dmov_scatter_or_gather
(
  buff_addr_and_length_pair_for_dmov_type* src_desc_array_ptr, 
  buff_addr_and_length_pair_for_dmov_type* dest_desc_array_ptr,
  uint32  params_array_index
)
{
  params_for_dmov_type* param_array_entry_ptr = NULL;
  dmov_xfer_desc_s_type  xfer_desc;

  /* Make sure this index is valid ! */
  HSU_ASSERT(param_array_index_is_valid(params_array_index) 
    == TRUE);

  param_array_entry_ptr = 
    &(hsu_al_dmov_api_static_data.params_array[params_array_index]);

  /* Set up the command descriptor */
  /* */
  param_array_entry_ptr->cmd_desc_ptr->command = AL_DMOV_API_CMD_SCATTER_GATHER;
  param_array_entry_ptr->cmd_desc_ptr->src_dscr_addr = (uint32)src_desc_array_ptr;
  param_array_entry_ptr->cmd_desc_ptr->dst_dscr_addr = (uint32)dest_desc_array_ptr;
  param_array_entry_ptr->cmd_desc_ptr->reserved = AL_DMOV_API_SCATTER_GATHER_RESERVED;

  /* Fill in transfer descriptor */
  xfer_desc.device = DMOV_DEVICE_USB; 

#if defined(FEATURE_HSU_EBI) || defined(FEATURE_HSU_TEST)
#error code not present
#else /* !FEATURE_HSU_EBI && !FEATURE_HSU_TEST */
  xfer_desc.chan = DMOV_CHAN_USB;
#endif /* FEATURE_HSU_EBI || FEATURE_HSU_TEST */

  xfer_desc.priority = DMOV_PRI_USB;
  xfer_desc.cmd_ptr = param_array_entry_ptr->cmd_desc_ptr;
  xfer_desc.cmd_type = DMOV_CMD_LIST;
  xfer_desc.callback_ptr = api_callback_for_dmov;
  xfer_desc.user_ptr = (void*)params_array_index;
  xfer_desc.options = DMOV_XFER_DEFAULT;

  if (!dmov_transfer(&xfer_desc))
  {
    HSU_MSG_ERROR("perform_dmov_scatter_or_gather: dmov_transfer failed!", 0, 0, 0);
    return FALSE;
  }

#ifdef T_REXNT
#error code not present
#endif
  return TRUE;
}

/*===========================================================================

FUNCTION alloc_param_array_index

DESCRIPTION
Gets the next available index of hsu_al_dmov_api_static_data.params_array[]
and marks the entry as not available before returning the index to caller.

DEPENDENCIES
To ensure correctness, must be called from within the 
hsu_al_dmov_api_static_data.params_array_access_guard critical section.

RETURN VALUE
uint32 - The index representing the params_array entry allocated to caller.

SIDE EFFECTS
None
===========================================================================*/
static uint32 alloc_param_array_index
(
  void
)
{
  uint32 current_used_index;

  /* Get next available index and save it */
  /* Note: Since there is a bounds check for next avail index, current_used_index 
  is always valid */
  current_used_index = hsu_al_dmov_api_static_data.next_available_index;

  /* Update the next avail index */
  ++hsu_al_dmov_api_static_data.next_available_index;
  if (hsu_al_dmov_api_static_data.next_available_index == 
    MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN)
  {
    hsu_al_dmov_api_static_data.next_available_index = 0;
  }

  /* Set the entry used as not available. 
  NOTE: It is assumed this is done from inside crit section 
  (see calls to hsu_al_dmov_gather_dsm_chain / hsu_al_dmov_api_scatter_dsm_chain) 
  due to the following:
  1. There are MAX_NUM_OF_ENQUEUED_DMOV_JOBS_PER_DMOV_CHAN+1 tasks using DMOV API.
  2. One by one they enter crit section, and just as they leave it the next task 
  enters the crit section (so no one is updating params_array_entry_is_free to FALSE.
  3. When the last task enters, it overruns the first task - there is no indication 
  an overrun occurred!
  */
  hsu_al_dmov_api_static_data.params_array_entry_is_free[current_used_index] = FALSE;

  /* Check if next_available_index is not overlapping data already enqueued : */
  /* No need to guard this from INT context (see api_callback_for_dmov), since 
  looking at/updating a boolean is an atomic operation */
  HSU_ASSERT(hsu_al_dmov_api_static_data.params_array_entry_is_free\
    [hsu_al_dmov_api_static_data.next_available_index] == TRUE);

  return current_used_index;
}


#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */
