/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====
 *
 *                 Diagnostics Packet Processing
 *
 *
 *  Copyright (C) 2002--2006  QUALCOMM, Incorporated.
 *  All Rights Reserved.
 *
 *  Diagnostic packet processing routines for malloc manager
 *
 *
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


 /*===========================================================================

                           Edit History

  $Header: //source/qcom/qct/multimedia/core/malloc_mgr/rel/1.0/src/malloc_mgr_diag.c#3 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/08   vma     Fixed KW warning.
01/02/08   vma     Fixed banned API usages
08/08/07   vma     Tracking of multiple heaps
07/10/07   vma     Improve performance by using doubly-linked list and 
                   hashtable to hold usage blocks
04/17/07   vma     Use dynamic memory for tracking and stacktrace blocks
01/12/07   vma     Added support for turning on/off tracking/stack tracing on
                   the fly
10/31/06   vma     Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "malloc_mgr_diag.h"
#include "msg.h"
#include "assert.h"
#include "string.h"

#ifdef MALLOC_LOGGING

#include <string.h>
#include <stdlib.h>

/*
 * The diag packets frequently contain a filename as the last field.
 * This is listed in the structure as char filename[1], while the variable-length
 * filename extends past the end of the structure.  Not surprisingly, lint
 * sees this as:
 * Error (Warning) 661: Possible access of out-of-bounds pointer
 * (1 beyond end of data) by operator '['
 * This is intentional, so the warning is suppressed here.
 */
/*lint -e661*/

#include "diag.h"

/*lint -e661 Ignore irrelevant lint errors */

/* -------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */

#define MAX_MM_DIAG_PKT_SIZE  1000

PACKED void * mm_diag_hello_handler                (PACKED void *, uint16);
PACKED void * mm_diag_query_mem_stat_handler       (PACKED void *, uint16);
PACKED void * mm_diag_query_group_handler          (PACKED void *, uint16);
PACKED void * mm_diag_query_task_handler           (PACKED void *, uint16);
PACKED void * mm_diag_query_grp_malloc_handler     (PACKED void *, uint16);
PACKED void * mm_diag_query_tsk_malloc_handler     (PACKED void *, uint16);
PACKED void * mm_diag_query_ref_malloc_handler     (PACKED void *, uint16);
PACKED void * mm_diag_query_malloc_detail_handler  (PACKED void *, uint16);
PACKED void * mm_diag_reset_reference_handler      (PACKED void *, uint16);
PACKED void * mm_diag_reset_ref_max_ovrall_handler (PACKED void *, uint16);
PACKED void * mm_diag_reset_ref_max_grp_handler    (PACKED void *, uint16);
PACKED void * mm_diag_reset_ref_max_tsk_handler    (PACKED void *, uint16);
PACKED void * mm_diag_reset_ref_max_allgrp_handler (PACKED void *, uint16);
PACKED void * mm_diag_reset_ref_max_alltsk_handler (PACKED void *, uint16);
PACKED void * mm_diag_configure_handler            (PACKED void *, uint16);
PACKED void * mm_diag_query_heap_handler           (PACKED void *, uint16);
PACKED void * mm_diag_query_heap_malloc_handler    (PACKED void *, uint16);

/* -------------------------------------------------------------------------
** Local Data
** ------------------------------------------------------------------------- */
static uint8  mm_diag_pkt_buffer[MAX_MM_DIAG_PKT_SIZE];
static uint32 mm_diag_buffer_bytes_left;

/* The dispatch table for the Malloc manager subsystem.
*/
static const diagpkt_user_table_entry_type mm_diag_tbl[] =
{
  {MM_DIAG_HELLO,                MM_DIAG_HELLO,                mm_diag_hello_handler},
  {MM_DIAG_QUERY_MEM_STAT,       MM_DIAG_QUERY_MEM_STAT,       mm_diag_query_mem_stat_handler},
  {MM_DIAG_QUERY_GROUP,          MM_DIAG_QUERY_GROUP,          mm_diag_query_group_handler},
  {MM_DIAG_QUERY_TASK,           MM_DIAG_QUERY_TASK,           mm_diag_query_task_handler},
  {MM_DIAG_QUERY_GRP_MALLOC,     MM_DIAG_QUERY_GRP_MALLOC,     mm_diag_query_grp_malloc_handler},
  {MM_DIAG_QUERY_TSK_MALLOC,     MM_DIAG_QUERY_TSK_MALLOC,     mm_diag_query_tsk_malloc_handler},
  {MM_DIAG_QUERY_REF_MALLOC,     MM_DIAG_QUERY_REF_MALLOC,     mm_diag_query_ref_malloc_handler},
  {MM_DIAG_QUERY_MALLOC_DETAIL,  MM_DIAG_QUERY_MALLOC_DETAIL,  mm_diag_query_malloc_detail_handler},
  {MM_DIAG_RESET_REFERENCE,      MM_DIAG_RESET_REFERENCE,      mm_diag_reset_reference_handler}, 
  {MM_DIAG_RESET_REF_MAX_OVRALL, MM_DIAG_RESET_REF_MAX_OVRALL, mm_diag_reset_ref_max_ovrall_handler},
  {MM_DIAG_RESET_REF_MAX_GRP,    MM_DIAG_RESET_REF_MAX_GRP,    mm_diag_reset_ref_max_grp_handler},
  {MM_DIAG_RESET_REF_MAX_TSK,    MM_DIAG_RESET_REF_MAX_TSK,    mm_diag_reset_ref_max_tsk_handler},
  {MM_DIAG_RESET_REF_MAX_ALLGRP, MM_DIAG_RESET_REF_MAX_ALLGRP, mm_diag_reset_ref_max_allgrp_handler},
  {MM_DIAG_RESET_REF_MAX_ALLTSK, MM_DIAG_RESET_REF_MAX_ALLTSK, mm_diag_reset_ref_max_alltsk_handler},
  {MM_DIAG_CONFIGURE,            MM_DIAG_CONFIGURE,            mm_diag_configure_handler},
  {MM_DIAG_QUERY_HEAP,           MM_DIAG_QUERY_HEAP,           mm_diag_query_heap_handler},
  {MM_DIAG_QUERY_HEAP_MALLOC,    MM_DIAG_QUERY_HEAP_MALLOC,    mm_diag_query_heap_malloc_handler},
};

/*===========================================================================

FUNCTION MM_DIAG_INIT
DESCRIPTION
  Initialization function for this module. Registers the packet handlers
  and sets up local data structures.
============================================================================*/
void mm_diag_init (void)
{
  /* --- Register the malloc manager Diag Subsystem handler. --- */

  /* By default, processor ID "0" will handle the calls.  A build may
     optionally override this. */
#ifndef MM_DIAG_SUBSYS_PROCESSOR_ID
#define MM_DIAG_SUBSYS_PROCESSOR_ID 0
#endif

  /* On builds using processor 0, there may not exist the new DIAG
     macro to register with a processor number, so we use the old
     macro to be safe.  Nonzero ID always requires the new macro. */
#if MM_DIAG_SUBSYS_PROCESSOR_ID == 0
  DIAGPKT_DISPATCH_TABLE_REGISTER ((int)DIAG_SUBSYS_MEMORY, mm_diag_tbl);
#else
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC ((int)MM_DIAG_SUBSYS_PROCESSOR_ID,
                                        (int)DIAG_SUBSYS_MEMORY, mm_diag_tbl);
#endif
  return;
} /* END mm_diag_init */

/*===========================================================================

FUNCTION MM_DIAG_HELLO_HANDLER
DESCRIPTION
  Handles the MM_DIAG_HELLO packet. 
============================================================================*/
PACKED void *
mm_diag_hello_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_hello_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_hello_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_hello_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_hello_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_HELLO,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->version_number = 1;
  response->enable_tracking = mm_enable_tracking;
  response->enable_stacktrace = mm_enable_stacktrace; 
  response->num_tracking_blks_allocated = (mm_usage_blocks) ? mm_num_usage_blocks : 0;
  response->num_stacktrace_blks_allocated = (mm_stacktrace_blocks) ? mm_num_stacktrace_blocks : 0;
  response->num_tracking_blks_in_use = 
    response->num_tracking_blks_allocated - ordered_list_size(&mm_usage_blocks_freelist);
  response->num_stacktrace_blks_in_use = 
    response->num_stacktrace_blks_allocated - list_size(&mm_stacktrace_blocks_freelist);
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  
  return response;

} /* mm_diag_hello_handler */

/*===========================================================================

FUNCTION MM_DIAG_QUERY_MEM_STAT_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_MEM_STAT packet. 
============================================================================*/
PACKED void *
mm_diag_query_mem_stat_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_mem_stat_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_query_mem_stat_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure function
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_mem_stat_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_query_mem_stat_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_QUERY_MEM_STAT,
                          rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  response->total_groups = mm_total_mem_stat.total_groups;
  response->total_tasks = mm_total_mem_stat.total_tasks;
  response->total_heaps = mm_total_mem_stat.total_heaps;
  response->mem_stat = mm_total_mem_stat.mem_stat;
  return response;

} /* mm_diag_query_mem_stat_handler */

/*===========================================================================

FUNCTION MM_DIAG_QUERY_GROUP_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_GROUP packet. 
============================================================================*/
PACKED void *
mm_diag_query_group_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  uint32 i;
  mm_diag_query_group_req_type *request;
  mm_diag_query_group_rsp_type *response;
  group_t *group;
  uint16 rsp_len = sizeof (mm_diag_query_group_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_group_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_group_req_type *) req_ptr;

  if (request->group_id >= mm_total_mem_stat.total_groups)
  {
    response = (mm_diag_query_group_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_GROUP,
                            rsp_len);
    if (response == NULL)
      return NULL;

    response->diag_errno = MM_DIAG_INVALID_GROUP;
    return response;
  }

  group = &mm_groups[request->group_id];
  response = (mm_diag_query_group_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_GROUP,
                            rsp_len + (group->num_parent_groups - 1) * sizeof(uint32) + strlen(group->name));
  if (response == NULL)
    return NULL;

  response->stat        = group->mem_stat;
  response->num_parents = group->num_parent_groups;
  for (i = 0; i < response->num_parents ; i++)
    response->parent_id_array[i] = group->parent_groups[i];

  /*lint -save -e534 -e826 
    Intentional pointer manipulation in order to copy string into the dynamically allocated
    buffer */ 
  memcpy((void *)&(response->parent_id_array[i]), group->name, strlen(group->name)+1);
  ((char *)&response->parent_id_array[i])[strlen(group->name)] = '\0';
  /*lint -restore*/

  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_query_group_handler */

/*===========================================================================
FUNCTION MM_DIAG_QUERY_TASK_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_TASK packet. 
============================================================================*/
PACKED void *
mm_diag_query_task_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_task_req_type *request;
  mm_diag_query_task_rsp_type *response;
  task_t *task;
  uint16 rsp_len = sizeof (mm_diag_query_task_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_task_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_task_req_type *) req_ptr;

  if (request->task_id >= mm_total_mem_stat.total_tasks)
  {
    response = (mm_diag_query_task_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_TASK,
                            rsp_len);
    if (response == NULL)
      return NULL;

    response->diag_errno = MM_DIAG_INVALID_TASK;
    return response;
  }

  task = &mm_tasks[request->task_id];
  response = (mm_diag_query_task_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_QUERY_TASK,
                          rsp_len + strlen(task->task_name));
  if (response == NULL)
    return NULL;

  response->stat = task->mem_stat;

  /*lint -save -e534 -e826 
    Intentional pointer manipulation in order to copy string into the dynamically allocated
    buffer */ 
  memcpy((void *)response->task_name, (const void *)task->task_name, strlen(task->task_name));
  response->task_name[strlen(task->task_name)] = '\0';
  /*lint -restore */

  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_query_task_handler */

/*===========================================================================
FUNCTION MM_DIAG_QUERY_HEAP_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_HEAP packet. 
============================================================================*/
PACKED void *
mm_diag_query_heap_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_heap_req_type *request;
  mm_diag_query_heap_rsp_type *response;
  heap_t *heap;
  mm_heap_info_t heap_info;
  uint16 heap_name_length = 0;
  uint16 rsp_len = sizeof (mm_diag_query_heap_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_heap_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_heap_req_type *) req_ptr;

  if (request->heap_id >= mm_total_mem_stat.total_heaps)
  {
    response = (mm_diag_query_heap_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_HEAP,
                            rsp_len);
    if (response == NULL)
      return NULL;

    response->diag_errno = MM_DIAG_INVALID_HEAP;
    return response;
  }

  heap = &mm_heaps[request->heap_id];
  /* query heap info if the heap provides the query function for it */
  memset((void *)&heap_info, 0, sizeof(mm_heap_info_t));
  if (heap->heap_info_q_func)
  {
    heap->heap_info_q_func(heap->func_arg, &heap_info);
    if (heap_info.name)
    {
      heap_name_length = strlen(heap_info.name);
    }
  }
  response = (mm_diag_query_heap_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_QUERY_HEAP,
                          rsp_len + heap_name_length);
  if (response == NULL)
    return NULL;

  response->stat = heap->mem_stat;
  response->used_bytes = heap_info.used_bytes;
  response->total_bytes = heap_info.total_bytes;
  /* copy heap name if necessary */
  if (heap_name_length)
  {
    memcpy((void *)response->heap_name, (void *)heap_info.name, heap_name_length);
    response->heap_name[heap_name_length] = '\0';
  }

  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_query_heap_handler */


/*===========================================================================

FUNCTION MM_DIAG_WRITE_MALLOC_INFO_TO_BUFFER
DESCRIPTION
  Helper function to extract information from malloc usage block and 
  copy into mm_diag_pkt_buffer 
  Returns 1 if successful, 0 otherwise
============================================================================*/
int mm_diag_write_malloc_info_to_buffer(const malloc_usage_block_t *block)
{
  /* Intentionaly casting of pointer to calculate the right position in the buffer to be the destination */
  /*lint -save -e826 */
  mm_diag_malloc_info_type *dest = (mm_diag_malloc_info_type *)&mm_diag_pkt_buffer[MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left];
  /*lint -restore */
  if (block == NULL)
    return 0;

  if (block->file_name == NULL)
  {
    if (mm_diag_buffer_bytes_left < sizeof(mm_diag_malloc_info_type))
      return 0;
    dest->file_name[0] = '\0';
    mm_diag_buffer_bytes_left -= sizeof(mm_diag_malloc_info_type);
  }
  else
  {
    if (mm_diag_buffer_bytes_left < sizeof(mm_diag_malloc_info_type) + strlen(block->file_name))
      return 0;

    /*lint -save -e534 -e826 
      Intentional pointer manipulation in order to copy string into the dynamically allocated
      buffer */ 
    memcpy((void *)dest->file_name, (const void *)block->file_name, strlen(block->file_name));
    dest->file_name[strlen(block->file_name)] = '\0';
    /*lint -restore */
    mm_diag_buffer_bytes_left -= ((sizeof(mm_diag_malloc_info_type) + strlen(block->file_name)) / 4) * 4;
  }
  dest->ptr      = block->malloc_ptr;
  dest->size     = block->malloc_size;
  dest->line_num = block->line_number;
  dest->group_id = block->group;
  dest->task_id  = block->task;
  dest->heap_id  = block->heap;
  return 1;
}

/*===========================================================================
FUNCTION MM_DIAG_QUERY_GRP_MALLOC_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_GRP_MALLOC packet. 
============================================================================*/
PACKED void *
mm_diag_query_grp_malloc_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_grp_malloc_req_type *request;
  mm_diag_query_grp_malloc_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_query_grp_malloc_rsp_type);
  uint32 mallocs_returned = 0;
  uint32 mallocs_found = 0;
  malloc_usage_block_t *block;
  boolean search_end = FALSE;
  uint16 i;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_grp_malloc_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_grp_malloc_req_type *) req_ptr;

  /* error checking on group ID */
  if (request->group_id >= (uint32)NUM_GROUPS)
  {
    response = (mm_diag_query_grp_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_GRP_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    response->diag_errno = MM_DIAG_INVALID_GROUP;
    return response;
  }
  /* error checking on malloc IDs */
  if (request->malloc_end_id < request->malloc_start_id ||
      request->malloc_start_id >= mm_groups[request->group_id].mem_stat.allocations)
  {
    response = (mm_diag_query_grp_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_GRP_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    response->diag_errno = MM_DIAG_INVALID_MALLOC_ID;
    return response;
  }

  mm_diag_buffer_bytes_left = MAX_MM_DIAG_PKT_SIZE;
  /* traverse the malloc usage blocks to look for requested malloc info */
  for (i = 0; i < MM_NUM_HASH_ENTRIES; i++)
  {
    block = (malloc_usage_block_t *)mm_grouping_heap.blocklists[i].front_ptr;

    /* intentional endless while loop */
    /*lint -save -e716 */
    while (1)
    {
      if (block == NULL)
        break;
      if (mm_is_group_inside_group(block->group, request->group_id))
      {
        /* block should be recorded */
        if (mallocs_found >= request->malloc_start_id)
        { 
          if (!mm_diag_write_malloc_info_to_buffer(block))
          {
            search_end = TRUE;
            break;
          }
          mallocs_returned++;
          if (mallocs_returned >= (request->malloc_end_id - request->malloc_start_id) + 1)
          {
            search_end = TRUE;
            break;
          }
        }
        mallocs_found++;
      }
      block = (malloc_usage_block_t *)
        ordered_list_peek_next(&mm_grouping_heap.blocklists[i], (ordered_list_link_type *)block);
    } /*lint -restore */
    if (search_end) break;
  }
  
  response = (mm_diag_query_grp_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_GRP_MALLOC,
                            rsp_len + (MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left)
                            - sizeof(mm_diag_malloc_info_type));
  if (response == NULL)
    return NULL;

  response->allocations_returned = mallocs_returned;
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  memcpy((void *)response->minfo, mm_diag_pkt_buffer, MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left);  
  return response;

} /* mm_diag_query_grp_malloc_handler */

/*===========================================================================
FUNCTION MM_DIAG_QUERY_TSK_MALLOC_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_TSK_MALLOC packet. 
============================================================================*/
PACKED void *
mm_diag_query_tsk_malloc_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_tsk_malloc_req_type *request;
  mm_diag_query_tsk_malloc_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_query_tsk_malloc_rsp_type);
  uint32 mallocs_returned = 0;
  uint32 mallocs_found = 0;
  malloc_usage_block_t *block;
  ordered_list_type *blocklist;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_tsk_malloc_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_tsk_malloc_req_type *) req_ptr;

  /* error checking on task ID */
  if (request->task_id >= mm_total_mem_stat.total_tasks)
  {
    response = (mm_diag_query_tsk_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_TSK_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    response->diag_errno = MM_DIAG_INVALID_TASK;
    return response;
  }

  blocklist = &mm_tasks[request->task_id].blocklists;

  /* error checking on malloc IDs */
  if (request->malloc_end_id < request->malloc_start_id ||
      request->malloc_start_id >= ordered_list_size(blocklist))
  {
    response = (mm_diag_query_tsk_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_TSK_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    
    response->diag_errno = MM_DIAG_INVALID_MALLOC_ID;
    return response;
  }

  mm_diag_buffer_bytes_left = MAX_MM_DIAG_PKT_SIZE;
  /* traverse the malloc usage blocks to look for requested malloc info */
  block = (malloc_usage_block_t *)blocklist->front_ptr;

  /*lint -save -e716 */
  while (1)
  {
    if (block == NULL)
      break;
    // adjust offset 
    block = (malloc_usage_block_t *)((uint32)block - ((uint32)&block->task_link - (uint32)block));

    /* block should be recorded */
    if (mallocs_found >= request->malloc_start_id)
    { 
      if (!mm_diag_write_malloc_info_to_buffer(block))
        break;
      mallocs_returned++;
      if (mallocs_returned >= (request->malloc_end_id - request->malloc_start_id) + 1)
        break;
    }
    mallocs_found++;     
    block = (malloc_usage_block_t *)ordered_list_peek_next(blocklist, (ordered_list_link_type *)&block->task_link);
  } /*lint -restore */
  
  response = (mm_diag_query_tsk_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_TSK_MALLOC,
                            rsp_len + (MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left)
                            - sizeof(mm_diag_malloc_info_type));
  if (response == NULL)
    return NULL;

  response->allocations_returned = mallocs_returned;
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  memcpy((void *)response->minfo, mm_diag_pkt_buffer, MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left);  
  return response;

} /* mm_diag_query_tsk_malloc_handler */

/*===========================================================================
FUNCTION MM_DIAG_QUERY_HEAP_MALLOC_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_HEAP_MALLOC packet. 
============================================================================*/
PACKED void *
mm_diag_query_heap_malloc_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_heap_malloc_req_type *request;
  mm_diag_query_heap_malloc_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_query_heap_malloc_rsp_type);
  uint32 mallocs_returned = 0;
  uint32 mallocs_found = 0;
  malloc_usage_block_t *block;
  boolean search_end = FALSE;
  heap_t *heap;
  uint16 i;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_heap_malloc_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_heap_malloc_req_type *) req_ptr;

  /* error checking on heap ID */
  if (request->heap_id >= mm_total_mem_stat.total_heaps)
  {
    response = (mm_diag_query_heap_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_HEAP_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    response->diag_errno = MM_DIAG_INVALID_HEAP;
    return response;
  }

  heap = (heap_t*)&mm_heaps[request->heap_id];

  /* error checking on malloc IDs and heap handle */
  if (request->malloc_end_id < request->malloc_start_id ||
      request->malloc_start_id >= mm_heaps[request->heap_id].mem_stat.allocations)
  {
    response = (mm_diag_query_heap_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_HEAP_MALLOC,
                            rsp_len);
    if (response == NULL)
      return NULL;
    
    response->diag_errno = MM_DIAG_INVALID_MALLOC_ID;
    return response;
  }
  
  mm_diag_buffer_bytes_left = MAX_MM_DIAG_PKT_SIZE;
  /* traverse the malloc usage blocks to look for requested malloc info */
  for (i = 0; i < MM_NUM_HASH_ENTRIES; i++)
  {
    /* skip according to malloc_start_id */
    while (i < MM_NUM_HASH_ENTRIES && 
        (mallocs_found + heap->blocklists[i].size <= request->malloc_start_id))
    {
      mallocs_found += heap->blocklists[i].size;
      i++;
    }
    if (i == MM_NUM_HASH_ENTRIES)
    {
      break;
    }

    block = (malloc_usage_block_t *)heap->blocklists[i].front_ptr;

    /*lint -save -e716 */
    while (1)
    {
      if (block == NULL)
        break;

      /* block should be recorded */
      if (mallocs_found >= request->malloc_start_id)
      { 
        if (!mm_diag_write_malloc_info_to_buffer(block))
        {
          search_end = TRUE;
          break;
        }
        mallocs_returned++;
        if (mallocs_returned >= (request->malloc_end_id - request->malloc_start_id) + 1)
        {
          search_end = TRUE;
          break;
        }
      }
      mallocs_found++;
    
      block = (malloc_usage_block_t *)
        ordered_list_peek_next(&heap->blocklists[i], (ordered_list_link_type *)block);
    } /*lint -restore */
    if (search_end) break;
  }
  response = (mm_diag_query_heap_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_HEAP_MALLOC,
                            rsp_len + (MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left)
                            - sizeof(mm_diag_malloc_info_type));
  if (response == NULL)
    return NULL;

  response->allocations_returned = mallocs_returned;
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  memcpy((void *)response->minfo, mm_diag_pkt_buffer, MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left);  
  return response;

} /* mm_diag_query_heap_malloc_handler */

/*===========================================================================
FUNCTION MM_DIAG_QUERY_REF_MALLOC_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_REF_MALLOC  packet. 
============================================================================*/
PACKED void *
mm_diag_query_ref_malloc_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_ref_malloc_req_type *request;
  mm_diag_query_ref_malloc_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_query_ref_malloc_rsp_type);
  uint32 mallocs_returned = 0;
  uint32 mallocs_found = 0;
  malloc_usage_block_t *block;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_ref_malloc_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_ref_malloc_req_type *) req_ptr;

  /* error checking on malloc IDs */
  if (request->malloc_end_id < request->malloc_start_id ||
      request->malloc_start_id >= ordered_list_size(&mm_usage_blocks_reflist))
  {
    response = (mm_diag_query_ref_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_REF_MALLOC ,
                            rsp_len);
    if (response == NULL)
      return NULL;

    response->diag_errno = MM_DIAG_INVALID_MALLOC_ID;
    return response;
  }
  
  mm_diag_buffer_bytes_left = MAX_MM_DIAG_PKT_SIZE;
  /* traverse the malloc usage blocks to look for requested malloc info */
  block = (malloc_usage_block_t *)mm_usage_blocks_reflist.front_ptr;
  
  /*lint -save -e716 */
  while (1)
  {
    if (block == NULL)
      break;
    // adjust offset 
    block = (malloc_usage_block_t *)((uint32)block - ((uint32)&block->ref_link - (uint32)block));

    /* block should be recorded */
    if (mallocs_found >= request->malloc_start_id)
    { 
      if (!mm_diag_write_malloc_info_to_buffer(block))
        break;
      mallocs_returned++;
      if (mallocs_returned >= (request->malloc_end_id - request->malloc_start_id) + 1)
        break;
    }
    mallocs_found++;     
    block = (malloc_usage_block_t *)ordered_list_peek_next(
      &mm_usage_blocks_reflist, (ordered_list_link_type *)&block->ref_link);
  } /*lint -restore */

  response = (mm_diag_query_ref_malloc_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_REF_MALLOC ,
                            rsp_len + (MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left)
                            - sizeof(mm_diag_malloc_info_type));
  if (response == NULL)
    return NULL;

  response->allocations_returned = mallocs_returned;
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  memcpy((void *)response->minfo, mm_diag_pkt_buffer, MAX_MM_DIAG_PKT_SIZE - mm_diag_buffer_bytes_left);  
  return response;

} /* mm_diag_query_ref_malloc_handler */

/*===========================================================================

FUNCTION MM_DIAG_QUERY_MALLOC_DETAIL_HANDLER
DESCRIPTION
  Handles the MM_DIAG_QUERY_MALLOC_DETAIL packet. 
============================================================================*/
PACKED void *
mm_diag_query_malloc_detail_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_query_malloc_detail_req_type *request;
  mm_diag_query_malloc_detail_rsp_type *response;
  malloc_usage_block_t *block;
  int32 stack_size;
  uint32 filename_len;
  heap_t* heap;
  uint16 rsp_len = sizeof (mm_diag_query_malloc_detail_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_query_malloc_detail_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_query_malloc_detail_req_type *) req_ptr;

  heap = (heap_t*)&mm_heaps[request->heap_id];

  /* search for the malloc pointer */
  block = (malloc_usage_block_t *)ordered_list_linear_search(
    &heap->blocklists[HASH_INDEX(request->ptr)],
    mm_usage_block_search, request->ptr);

  /* pointer not found */
  if (block == NULL)
  {
    response = (mm_diag_query_malloc_detail_rsp_type *)
      diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                            MM_DIAG_QUERY_MALLOC_DETAIL,
                            rsp_len);
    if (response == NULL)
      return NULL;
    response->diag_errno = MM_DIAG_MALLOC_NOT_FOUND;
    return response;
  }

  stack_size = (block->stacktrace) ? block->stacktrace->stacktrace_len * 4 : 0;
  if (stack_size < 0)
    stack_size *= -1;
  filename_len = (block->file_name == NULL) ? 0 : strlen(block->file_name);
  
  response = (mm_diag_query_malloc_detail_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_QUERY_MALLOC_DETAIL,
                          rsp_len + ((uint32)stack_size - 4) + filename_len);
  if (response == NULL)
    return NULL;

  response->group_id    = block->group;
  response->task_id     = block->task;
  response->heap_id     = block->heap;
  response->ptr         = block->malloc_ptr;
  response->size        = block->malloc_size;
  response->line_num    = block->line_number;
  response->stack_depth = (block->stacktrace) ? block->stacktrace->stacktrace_len : 0;

  if (stack_size) 
    /* already made sure stack_size is positive, safe to cast */
    memcpy((void *)response->stack_trace, block->stacktrace->stacktrace, (uint32)stack_size);

  /*lint -save -e534 -e826 
    Intentional pointer manipulation in order to copy string into the dynamically allocated
    buffer */ 
  if (block->file_name != NULL)
    memcpy((void *)&response->stack_trace[stack_size/4], block->file_name, filename_len);
  ((char *)&response->stack_trace[stack_size/4])[filename_len] = '\0';
  /*lint -restore */
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;

  return response;

} /* mm_diag_query_malloc_detail_handler */

/*===========================================================================

FUNCTION MM_DIAG_RESET_REFERENCE_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REFERENCE packet. 
============================================================================*/
PACKED void *
mm_diag_reset_reference_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_general_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REFERENCE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  mm_reset_reference();
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_reset_reference_handler */

/*===========================================================================

FUNCTION MM_DIAG_RESET_REF_MAX_OVRALL_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REF_MAX_OVRALL packet. 
============================================================================*/
PACKED void *
mm_diag_reset_ref_max_ovrall_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_general_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REF_MAX_OVRALL,
                          rsp_len);
  if (response == NULL)
    return NULL;

  mm_total_mem_stat.mem_stat.ref_max_usage = mm_total_mem_stat.mem_stat.ref_current_usage;
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_reset_ref_max_ovrall_handler */

/*===========================================================================

FUNCTION MM_DIAG_RESET_REF_MAX_GRP_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REF_MAX_GRP packet. 
============================================================================*/
PACKED void *
mm_diag_reset_ref_max_grp_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_reset_ref_max_grp_req_type *request;
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_reset_ref_max_grp_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_reset_ref_max_grp_req_type *) req_ptr;

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REF_MAX_GRP,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (mm_reset_ref_max_group(request->group_id))
    response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  else
    response->diag_errno = MM_DIAG_INVALID_GROUP;

  return response;

} /* mm_diag_reset_ref_max_grp_handler */

/*===========================================================================

FUNCTION MM_DIAG_RESET_REF_MAX_TSK_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REF_MAX_TSK packet. 
============================================================================*/
PACKED void *
mm_diag_reset_ref_max_tsk_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_reset_ref_max_tsk_req_type *request;
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_reset_ref_max_tsk_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (mm_diag_reset_ref_max_tsk_req_type *) req_ptr;

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REF_MAX_TSK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  if (mm_reset_ref_max_task(request->task_id))
    response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  else
    response->diag_errno = MM_DIAG_INVALID_TASK;
    
  return response;

} /* mm_diag_reset_ref_max_tsk_handler */

/*===========================================================================
FUNCTION MM_DIAG_RESET_REF_MAX_ALLGRP_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REF_MAX_ALLGRP packet. 
============================================================================*/
PACKED void *
mm_diag_reset_ref_max_allgrp_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_general_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REF_MAX_ALLGRP,
                          rsp_len);
  if (response == NULL)
    return NULL;

  mm_reset_ref_max_all_group();

  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_reset_ref_max_allgrp_handler */

/*===========================================================================
FUNCTION MM_DIAG_RESET_REF_MAX_ALLTSK_HANDLER
DESCRIPTION
  Handles the MM_DIAG_RESET_REF_MAX_ALLTSK packet. 
============================================================================*/
PACKED void *
mm_diag_reset_ref_max_alltsk_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_general_rsp_type *response;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  if (pkt_len != sizeof (mm_diag_general_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_RESET_REF_MAX_ALLTSK,
                          rsp_len);
  if (response == NULL)
    return NULL;

  mm_reset_ref_max_all_task();
  
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_reset_ref_max_alltsk_handler */

/*===========================================================================

FUNCTION MM_DIAG_CONFIGURE_HANDLER
DESCRIPTION
  Handles the MM_DIAG_CONFIGURE packet. 
============================================================================*/
PACKED void *
mm_diag_configure_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  mm_diag_general_rsp_type   *response;
  mm_diag_configure_req_type *request = (mm_diag_configure_req_type *)req_ptr;
  uint16 rsp_len = sizeof (mm_diag_general_rsp_type);

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
    ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_ptr, pkt_len) );
  }

  /* allow backward compatibility */
  if (pkt_len > sizeof (mm_diag_configure_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (mm_diag_general_rsp_type *)
    diagpkt_subsys_alloc ((int)DIAG_SUBSYS_MEMORY,
                          MM_DIAG_CONFIGURE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  /* check to ensure backward compatibility */
  if (pkt_len > ((uint32)(&request->num_tracking_blocks) - (uint32)request)) 
  {
    /* set number of stacktrace blocks if necessary and allowed */
    if (request->num_stacktrace_blocks != 0xFFFFFFFF && !mm_stacktrace_blocks) 
      mm_num_stacktrace_blocks = request->num_stacktrace_blocks;
      
    /* set number of usage blocks if necessary and allowed */
    if (request->num_tracking_blocks != 0xFFFFFFFF && !mm_usage_blocks) 
      mm_num_usage_blocks = request->num_tracking_blocks;
  }

  /* 0xFF indicates no change */
  if (request->enable_stacktrace != 0xFF) 
  {
    mm_enable_stacktrace = request->enable_stacktrace & 1;
    if (mm_enable_stacktrace && !mm_stacktrace_blocks) 
      if (!mm_allocate_stacktrace_blocks())
      {
        response->diag_errno = MM_DIAG_OUT_OF_MEMORY;
        return response;
      }
  }
  /* 0xFF indicates no change */
  if (request->enable_tracking != 0xFF)
  {
    mm_enable_tracking = request->enable_tracking & 1;
    if (mm_enable_tracking && !mm_usage_blocks) 
      if (!mm_allocate_usage_blocks())
      {
        response->diag_errno = MM_DIAG_OUT_OF_MEMORY;
        return response;
      }
  }
    
  response->diag_errno = (mm_out_of_usage_blocks) ? MM_DIAG_OUT_OF_BLOCKS : 0;
  return response;

} /* mm_diag_reset_reference_handler */

#endif /* MALLOC_LOGGING */
