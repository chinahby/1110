/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  M A L L O C     M A N A G E R 

GENERAL DESCRIPTION

         Keeps track of malloc's and free's for heap usage statistics


INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002-2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/core/malloc_mgr/rel/1.0/src/malloc_mgr.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/08   vma     Fixed CR 157027 - When task blocks is running out, use
                   the first block - dedicated to non-recycling tasks which 
                   keep getting created.
08/17/07   vma     Removed dependency on stack unwind function from BREW by 
                   maintaining an own copy
08/08/07   vma     Tracking of multiple heaps
07/10/07   vma     Improve performance by using doubly-linked list and 
                   hashtable to hold usage blocks
05/12/07   vma     Use task name for matching instead of tcb pointer
04/17/07   vma     Use dynamic memory for tracking and stacktrace blocks
03/15/07   vma     Disable tracking by default
02/16/07   vma     Added checking for initialization (self-initialize if needed)
01/12/07   vma     Added support for turning on/off tracking/stack tracing on
                   the fly
11/29/06   vma     Major enhancement. Includes diag interface to communicate
                   with PC side tool/APS. Groups allocations by rex tasks.
10/24/06   vma     Initialize usage_blocks array, fixed bug not updating 
                   num_used_blocks in mm_free
05/17/06   vma     Added max heap reset functions, fixed typo
05/15/06   vma     Initial version

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include <stdlib.h>
#include "msg.h"
#include "malloc_mgr.h"
#include "malloc_mgr_diag.h"
#include "assert.h"
                       
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*=============================================================================

This section is copied from OEMARMFastBacktraceHelper.c in order to remove
dependency to BREW stack unwind function 

===============================================================================*/
#ifndef PLATFORM_LTK
#define EXTERN_RANGE(name)                       \
   extern unsigned long Image__##name##__Base;   \
   extern unsigned long Image__##name##__Length

EXTERN_RANGE(BOOT);
EXTERN_RANGE(APP_RAM);

#define BASE(name) Image__##name##__Base
#define LENGTH(name) Image__##name##__Length

#define BEFORE(address, name) (address < BASE(name))
#define AFTER(address, name)  (address > (BASE(name) + LENGTH(name)))

int MALLOCMGR_SYSTEM_CHECK_DATA_SECTION_POINTER(unsigned long address)
{
	return 1;
}

int MALLOCMGR_SYSTEM_CHECK_TEXT_SECTION_POINTER(unsigned long address)
{
  return address >= 0x20 && BEFORE(address, APP_RAM);
}
#endif//PLATFORM_LTK

#ifdef MALLOC_LOGGING

heap_t                mm_heaps[MAX_HEAPS];
task_t                mm_tasks[MAX_TASKS];
/* Casting of enum GROUP_ENUM to int is intentional here */
/*lint -save -e641 */
group_t               mm_groups[NUM_GROUPS] = {

  /* name              group_ID           parent_groups_array   #parents   usages and allocations */
  {"JPEG",             JPEG_GROUP,        {CAMERA_COMBINED},    1,         0, 0, 0, 0, 0, 0, 0},
  {"IPL",              IPL_GROUP,         {CAMERA_COMBINED},    1,         0, 0, 0, 0, 0, 0, 0},
  {"Camera Scvs",      CAMERA_SVCS_GROUP, {CAMERA_COMBINED},    1,         0, 0, 0, 0, 0, 0, 0},
  {"Camera combined",  CAMERA_COMBINED,   {0},                  0,         0, 0, 0, 0, 0, 0, 0}
};
/*lint -restore */

/* global variables */
malloc_usage_block_t *mm_usage_blocks = NULL;            /* usage blocks */
stacktrace_block_t   *mm_stacktrace_blocks = NULL;       /* blocks holding stacktrace information */

ordered_list_type     mm_usage_blocks_freelist;          /* list of usage blocks available */
ordered_list_type     mm_usage_blocks_reflist;           /* list of usage blocks that is made after the reference point */

list_type             mm_stacktrace_blocks_freelist;     /* list of usage blocks available */

list_type             mm_heap_list;                      /* list of heap info structures */
uint32                mm_num_usage_blocks = DEFAULT_MEMORY_USAGE_BLOCKS;   
                                                         /* number of usage blocks allocated */
uint32                mm_num_stacktrace_blocks = DEFAULT_STACKTRACE_BLOCKS; 
                                                         /* number of stacktrace blocks allocated */

heap_t*               mm_default_heap = NULL;            /* Default heap - used in 6K where there is only one heap */
heap_t                mm_grouping_heap;                  /* Grouping heap - treated as a separate heap to track all
                                                            allocations using the mm_malloc() interface */
uint32                mm_current_ref_id;                 /* Current ref ID */
total_mem_stat_t      mm_total_mem_stat;                 /* malloc mgr overall statistics */

boolean               mm_out_of_usage_blocks = FALSE;    /* flag indicating whether out of block state is hit
                                                            in which case all tracking will be turned off */
boolean               mm_enable_tracking = FALSE;        /* flag indicating whether tracking is enabled */
boolean               mm_enable_stacktrace = FALSE;      /* flag indicating whether stacktracing is enabled */
boolean               mm_initialized = FALSE;            /* flag indicating whether the manager is 
                                                            initialized properly */

boolean               mm_internal_allocation_active = FALSE; 
                                                         /* flag indicating whether internal allocation
                                                            is active or not */

/*===========================================================================

FUNCTION      mm_init

DESCRIPTION
              initialize malloc_mgr

DEPENDENCIES
  None

RETURN VALUE
  None
                
SIDE EFFECTS
  None

===========================================================================*/
void mm_init(void)
{
  uint16 i;
  if (mm_initialized) 
    return;

  mm_current_ref_id = 0;

  /* initialize mm_total_mem_stat */
  memset((void *)&mm_total_mem_stat, 0, sizeof(total_mem_stat_t));
  mm_total_mem_stat.total_groups = (group_ID_t)NUM_GROUPS;

  /* initialize all tasks blocks */
  memset((void *)mm_tasks, 0, (int)MAX_TASKS * sizeof(task_t));
  mm_tasks[0].task_ID = 0;
  memcpy(mm_tasks[0].task_name, "Undefined", 10);
  mm_total_mem_stat.total_tasks = 1;
  
  /* intialize all heap blocks */
  memset((void *)mm_heaps, 0, (int)MAX_HEAPS * sizeof(heap_t));

  ordered_list_init(&mm_usage_blocks_freelist, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);
  ordered_list_init(&mm_usage_blocks_reflist,  ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);

  /* initialize stacktrace block free lists */
  list_init(&mm_stacktrace_blocks_freelist);

  /* initialize heap lists */
  list_init(&mm_heap_list);
  
  /* initialize mm_diag module */
  mm_diag_init();

  /* initialize grouping heap */
  for (i = 0; i < MM_NUM_HASH_ENTRIES; i++)
    ordered_list_init(&mm_grouping_heap.blocklists[i], ORDERED_LIST_DESCENDING, ORDERED_LIST_PUSH_SLT);
  
  /* set flag to indicate intialization is done */
  mm_initialized = TRUE;
}

/*===========================================================================

FUNCTION      mm_internal_malloc

DESCRIPTION
              allocates storage for internal use

DEPENDENCIES
  None

RETURN VALUE
              pointer to allocation if successful, NULL otherwise
                
SIDE EFFECTS
  None

===========================================================================*/
static void *mm_internal_malloc(uint32 size)
{
  void *result;
  INTLOCK();
  // turn on flag to avoid internal allocation to be tracked, causing infinite looping
  mm_internal_allocation_active = TRUE;
  result = malloc(size);
  mm_internal_allocation_active = FALSE;
  INTFREE();
  return result;
}

/*===========================================================================

FUNCTION      mm_allocate_stacktrace_blocks

DESCRIPTION
              allocates storage for holding stacktraces

DEPENDENCIES
  None

RETURN VALUE
              TRUE if allocation is done successfully, FALSE otherwise
                
SIDE EFFECTS
  None

===========================================================================*/
boolean mm_allocate_stacktrace_blocks(void)
{
  uint32 i;

  if (mm_stacktrace_blocks || !mm_num_stacktrace_blocks) 
    return FALSE;

  mm_stacktrace_blocks = (stacktrace_block_t *)mm_internal_malloc(mm_num_stacktrace_blocks * sizeof(stacktrace_block_t));

  if (!mm_stacktrace_blocks) 
    return FALSE;

  memset((void*)mm_stacktrace_blocks, 0, mm_num_stacktrace_blocks * sizeof(stacktrace_block_t));

  /* insert blocks into free list */
  for (i = 0; i < mm_num_stacktrace_blocks; i++)
    list_push_back(&mm_stacktrace_blocks_freelist, (list_link_type *)&mm_stacktrace_blocks[i]);

  return TRUE;
}

/*===========================================================================

FUNCTION      mm_allocate_usage_blocks

DESCRIPTION
              allocates storage for holding allocation usages

DEPENDENCIES
  None

RETURN VALUE
              TRUE if allocation is done successfully, FALSE otherwise
                
SIDE EFFECTS
  None

===========================================================================*/
boolean mm_allocate_usage_blocks(void)
{
  uint32 i;

  if (mm_usage_blocks || !mm_num_usage_blocks) 
    return FALSE;

  mm_usage_blocks = (malloc_usage_block_t *)mm_internal_malloc(mm_num_usage_blocks * sizeof(malloc_usage_block_t));

  if (!mm_usage_blocks) 
    return FALSE;

  memset((void*)mm_usage_blocks, 0, mm_num_usage_blocks * sizeof(malloc_usage_block_t));

  /* insert blocks into free list */
  for (i = 0; i < mm_num_usage_blocks; i++)
    ordered_list_push(&mm_usage_blocks_freelist, (ordered_list_link_type *)&mm_usage_blocks[i], 0);

  return TRUE;
}

/*===========================================================================

FUNCTION      mm_allocate_heap_info

DESCRIPTION
              allocates storage for holding information about a heap

DEPENDENCIES
  None

RETURN VALUE
              pointer to the heap object if successful, NULL otherwise
                
SIDE EFFECTS
  None

===========================================================================*/
heap_t* mm_allocate_heap_info(void)
{
  uint16 i;
  heap_t* heap = (heap_t *)mm_internal_malloc(sizeof(heap_t));

  if (!heap) 
    return NULL;

  /* intialize heap info structure */
  memset((void*)heap, 0, sizeof(heap_t));

  for (i = 0; i < MM_NUM_HASH_ENTRIES; i++)
  {
    /* Always push item to the front of the list by making the weight the same
       on the orderred list; take advantage of the fact that allocations 
       are made and freed on a LIFO fashion */
    ordered_list_init(&heap->blocklists[i], ORDERED_LIST_DESCENDING, ORDERED_LIST_PUSH_SLT);
  }

  /* insert heap info structure to list */
  list_push_back(&mm_heap_list, (list_link_type *)heap);

  return heap;
}

/*===========================================================================

FUNCTION      mm_register_heap

DESCRIPTION
              exported function for different heap implementations or instantiations
              to register themselves to be tracked by malloc manager

DEPENDENCIES
  None

RETURN VALUE
              a heap handle if succsesful, 0 otherwise
                
SIDE EFFECTS
  None

===========================================================================*/
heap_handle_t mm_register_heap
(
  /* query function for heap to provide info about the heap */
  mm_heap_info_query_func_type        heap_info_query_func,       
  /* query function for heap to provide info about a particular allocation on the heap*/   
  mm_allocation_info_query_func_type  allocation_info_query_func, 
  /* the argument that would be passed to the query function */
  void*                               func_arg
)
{
  heap_t* heap;
  uint16 i;

  if (!mm_initialized)
    mm_init();

  if (mm_total_mem_stat.total_heaps == MAX_HEAPS)
    return (heap_handle_t)0;

    heap = &mm_heaps[mm_total_mem_stat.total_heaps];
    
    heap->heap_ID                = mm_total_mem_stat.total_heaps;
    heap->heap_info_q_func       = heap_info_query_func;
    heap->allocation_info_q_func = allocation_info_query_func;
    heap->func_arg               = func_arg;

    mm_total_mem_stat.total_heaps++;

    for (i = 0; i < MM_NUM_HASH_ENTRIES; i++)
    {
      /* Always push item to the front of the list by making the weight the same
         on the orderred list; take advantage of the fact that allocations 
         are made and freed on a LIFO fashion */
      ordered_list_init(&heap->blocklists[i], ORDERED_LIST_DESCENDING, ORDERED_LIST_PUSH_SLT);
    }


  return (heap_handle_t)heap;
}

/*===========================================================================

FUNCTION      mm_usage_block_search

DESCRIPTION
              search usage block for matching pointer
              
DEPENDENCIES
  None

RETURN VALUE
              1 if match, 0 otherwise
 
SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818 list_linear_search needs the searching function to have this signature */
int mm_usage_block_search(void *usage_block, void *pointer)
{
  return (((malloc_usage_block_t *)usage_block)->malloc_ptr == pointer);
}
/*lint -restore */

/*===========================================================================

FUNCTION      mm_get_current_task_id

DESCRIPTION
              retrieve the task id of the current rex task
DEPENDENCIES
  None

RETURN VALUE
  Task ID of current task
SIDE EFFECTS
  mm_tasks are updated accordingly
===========================================================================*/
task_ID_t mm_get_current_task_id()
{
  uint32 i;
  rex_tcb_type* curr_task = rex_self();
  /* linear scan through mm_tasks for a match */
  for (i = 1; i < mm_total_mem_stat.total_tasks; i++)
  {
    if (!strcmp(mm_tasks[i].task_name, curr_task->task_name))
    {
      /* found */
      return i;
    }
  }
  /* not found - assign new task id if mm_tasks is not exhausted */
  if (i < (int)MAX_TASKS)
  {
    mm_tasks[i].task_ID = i;
    memcpy(mm_tasks[i].task_name, curr_task->task_name, REX_TASK_NAME_LEN + 1);
    ordered_list_init(&mm_tasks[i].blocklists, ORDERED_LIST_ASCENDING, ORDERED_LIST_PUSH_LTE);
    mm_total_mem_stat.total_tasks++;
    return i;
  }
  else
  {
    // Use the 'Undefined' tasks since the all task blocks are exhausted
    return 0;
  }
}
/*===========================================================================

FUNCTION      mm_reset_mem_stat_reference

DESCRIPTION
              reset the reference stats to 0
              
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void mm_reset_mem_stat_reference(mem_stat_t *stat)
{
  stat->ref_current_usage = 0;
  stat->ref_allocations = 0;
  stat->ref_new_allocations = 0;
  stat->ref_max_usage = 0;
}

/*===========================================================================

FUNCTION      mm_update_mem_stat

DESCRIPTION
              update memory usage statistics
              
DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void mm_update_mem_stat(mem_stat_t *stat, uint32 delta_size, int8 delta_sign, uint32 ref_id)
{
  /* (positive) increase */
  if (delta_sign >= 0)
  {
    stat->current_usage += delta_size;
    stat->allocations ++;
    stat->ref_current_usage += (int32)delta_size;
    stat->ref_allocations ++;
    stat->ref_new_allocations ++;
    if (stat->max_usage < stat->current_usage)
    {
      stat->max_usage = stat->current_usage;
    }
    if (stat->ref_max_usage < stat->ref_current_usage)
    {
      stat->ref_max_usage = stat->ref_current_usage;
    }
  }
  /* (negative) decrease */
  else
  {
    stat->current_usage -= delta_size;
    stat->allocations --;
    stat->ref_current_usage -= (int32)delta_size;
    stat->ref_allocations --;
    if (ref_id == mm_current_ref_id)
    {
      stat->ref_new_allocations --;
    }
  }
}

/*===========================================================================

FUNCTION      mm_update_group_mem_stat

DESCRIPTION
              updates the usage stat of a group and the parent groups
              recursively
              
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  mm_groups is updated accordingly

===========================================================================*/
void mm_update_group_mem_stat(group_ID_t grp, uint32 delta_size, int8 delta_sign, uint32 ref_id)
{
  uint32 i;
  mm_update_mem_stat(&mm_groups[grp].mem_stat, delta_size, delta_sign, ref_id);

  for (i = 0; i < mm_groups[grp].num_parent_groups; i++)
  {
    mm_update_group_mem_stat(mm_groups[grp].parent_groups[i], delta_size, delta_sign, ref_id);
  }
}

/*===========================================================================

FUNCTION      mm_unwind_stack

DESCRIPTION
              unwind current call stack and store in internal buffers
              
DEPENDENCIES
  None

RETURN VALUE
              returns stacktrace_block holding the stacktrace if successful
              returns NULL otherwise
SIDE EFFECTS
  None
===========================================================================*/
stacktrace_block_t* mm_unwind_stack(void)
{
  stacktrace_block_t *block;
  /* make sure stacktrace is enabled */
  if (!mm_enable_stacktrace)
    return NULL;
  /* make sure stacktrace blocks are allocated */
  if (!mm_stacktrace_blocks && !mm_allocate_stacktrace_blocks())
    return NULL;
  /* make sure there is available free block to hold stacktrace */
  if (!list_size(&mm_stacktrace_blocks_freelist))
    return NULL;

  block = (stacktrace_block_t *)list_pop_front(&mm_stacktrace_blocks_freelist);
#ifndef PLATFORM_LTK
  block->stacktrace_len = MALLOCMGR_UNWIND_STACK(block->stacktrace, STACKTRACE_BUFSIZE);
#endif //PLATFORM_LTK
  return block;
}

/*===========================================================================

FUNCTION      mm_update

DESCRIPTION
              updates the malloc manager whenever an allocation information 
              is changed
              
DEPENDENCIES
  None

RETURN VALUE
              returns 1 if successful
              returns 0 otherwise
SIDE EFFECTS

===========================================================================*/
int mm_update(
  heap_handle_t  heap_handle, 
  uint32         old_size, 
  uint32         new_size, 
  void          *old_ptr, 
  void          *new_ptr
)
{
  task_t   *task; 
  heap_t   *heap = (heap_t*)heap_handle;

  /* make sure malloc manager is properly intialized */
  if (!mm_initialized) 
    mm_init();

  /* Malloc manager stops working once usage blocks run out */
  if (mm_out_of_usage_blocks || !mm_enable_tracking || mm_internal_allocation_active) 
    return 0;

  /* make sure usage blocks are allocated */
  if (!mm_usage_blocks && !mm_allocate_usage_blocks())
    return 0;

  /* validate new_ptr and heap pointer */
  if (new_ptr == NULL || heap == NULL)
    return 0;

  /* No tracking if allocations are made/freed in DIAG task to avoid confusion during
     DIAG packet transmissions for internal malloc manager uses */
  if (rex_self() == &diag_tcb)
    return 0;
  
  /* free operation */
  if (new_size == 0)
  {
    /* compute hash index from pointer */
    uint16 hash_index = HASH_INDEX(new_ptr);
    /* linear search for matching pointer */
    malloc_usage_block_t *block = (malloc_usage_block_t *)ordered_list_linear_search(
      &heap->blocklists[hash_index], mm_usage_block_search, new_ptr);

    if (block != NULL)
    {
      /* found */
      block->malloc_ptr = NULL;

      /* update heap mem stats */
      mm_update_mem_stat(&heap->mem_stat, block->malloc_size, -1, block->ref_id);

      /* update task mem stats */
      task = &mm_tasks[block->task];
      mm_update_mem_stat(&task->mem_stat, block->malloc_size, -1, block->ref_id);

      /* update overall mem stats */
      mm_update_mem_stat(&mm_total_mem_stat.mem_stat, block->malloc_size, -1, block->ref_id);

      if (block->stacktrace) 
      {
        list_push_front(&mm_stacktrace_blocks_freelist, (list_link_type *)block->stacktrace);
        block->stacktrace = NULL;
      }
      ordered_list_pop_item(&heap->blocklists[hash_index], (ordered_list_link_type *)block);

      /* remove block from task list */
      ordered_list_pop_item(&task->blocklists, (ordered_list_link_type *)&block->task_link);

      /* remove block from ref list if necessary */
      INTLOCK();
      if (block->ref_link.prev_ptr || block->ref_link.next_ptr || 
          mm_usage_blocks_reflist.front_ptr == &block->ref_link)
      {
        ordered_list_pop_item(&mm_usage_blocks_reflist, (ordered_list_link_type *)&block->ref_link);
        block->ref_link.prev_ptr = NULL;
        block->ref_link.next_ptr = NULL;
      }
      INTFREE();

      /* put the block back to the free list */
      ordered_list_push(&mm_usage_blocks_freelist, (ordered_list_link_type *)block, 0);
      return 1;
    }
    else
    {
      /* not found */
      MSG_ERROR("MALLOC_MANAGER: Pointer 0x%d freed but not tracked", new_ptr, 0, 0);
      return 0;
    }
  }
  /* malloc operation */
  else if (old_size == 0)
  {  
    /* free tracking blocks available */
    malloc_usage_block_t *block = (malloc_usage_block_t *)ordered_list_pop_front(&mm_usage_blocks_freelist);

    if (!block)
    {
      mm_out_of_usage_blocks = TRUE;
      MSG_ERROR("MALLOC_MANAGER: Not enough tracking blocks to track malloc: 0x%d", new_ptr, 0, 0);
      return 0;
    }

    block->malloc_ptr = new_ptr;
    block->malloc_size = new_size;
    block->group = UNGROUPED;
    block->file_name = NULL;
    block->line_number = 0;
    block->ref_id = mm_current_ref_id;
    block->task = mm_get_current_task_id();
    block->heap = heap->heap_ID;
    if (mm_enable_stacktrace) 
      block->stacktrace = mm_unwind_stack();
    else
      block->stacktrace = NULL;

    /* update heap mem stats */
    mm_update_mem_stat(&heap->mem_stat, block->malloc_size, 1, block->ref_id);

    /* update task mem stats */
    task = &mm_tasks[block->task];
    mm_update_mem_stat(&task->mem_stat, block->malloc_size, 1, block->ref_id);

    /* update overall mem_stats */
    mm_update_mem_stat(&mm_total_mem_stat.mem_stat, block->malloc_size, 1, block->ref_id);

    /* insert tracking block into the front of the used list */
    ordered_list_push(&heap->blocklists[HASH_INDEX(new_ptr)], 
                      (ordered_list_link_type *)block, 0);

    /* insert tracking block into task list */
    ordered_list_push(&task->blocklists, (ordered_list_link_type *)&block->task_link, 0);

    /* insert tracking block into ref list */
    ordered_list_push(&mm_usage_blocks_reflist, (ordered_list_link_type *)&block->ref_link, 0);
  }
  /* realloc operation */
  else 
  {
    malloc_usage_block_t *block;
    if (old_ptr == NULL)
    {
      return 0;
    }
    /* linear search for matching pointer */
    block = (malloc_usage_block_t *)ordered_list_linear_search(&heap->blocklists[HASH_INDEX(old_ptr)], 
                                                               mm_usage_block_search, old_ptr);
    if (block != NULL)
    {
      task_ID_t old_task;
      /* found */
      if (old_size != block->malloc_size)
      {
        MSG_ERROR("MALLOC_MANAGER: mm_update_task_usage: old_size != block->malloc_size", 0, 0, 0);
      }
      if (mm_enable_stacktrace) 
        block->stacktrace = mm_unwind_stack();
      else
        block->stacktrace = NULL;

      block->malloc_ptr = new_ptr;
      block->malloc_size = new_size;

      /* adjust the ref list */
      INTLOCK();
      if (!block->ref_link.prev_ptr && !block->ref_link.next_ptr)
      {
        ordered_list_push(&mm_usage_blocks_reflist, (ordered_list_link_type *)&block->ref_link, 0);
      }
      INTFREE();
      block->ref_id = mm_current_ref_id;      
      
      /* move from old list to new list */
      if (HASH_INDEX(old_ptr) != HASH_INDEX(new_ptr))
      {
        ordered_list_pop_item(&heap->blocklists[HASH_INDEX(old_ptr)], 
                              (ordered_list_link_type *)block);
        ordered_list_push(&heap->blocklists[HASH_INDEX(new_ptr)], 
                          (ordered_list_link_type *)block, 0);
      }

      /* update heap mem stats */
      mm_update_mem_stat(&heap->mem_stat, old_size, -1, block->ref_id);
      mm_update_mem_stat(&heap->mem_stat, new_size, 1, block->ref_id);

      /* update task mem stats */
      task = &mm_tasks[block->task];
      mm_update_mem_stat(&task->mem_stat, old_size, -1, block->ref_id);
      old_task = block->task;

      block->task = mm_get_current_task_id();
      /* remove block from old task list if necessary */
      if (block->task != old_task)
        ordered_list_pop_item(&task->blocklists, (ordered_list_link_type *)&block->task_link);
      
      task = &mm_tasks[block->task];
      mm_update_mem_stat(&task->mem_stat, new_size, 1, block->ref_id);    
      /* add back to new task list if necessary */
      if (block->task != old_task)
        ordered_list_push(&task->blocklists, (ordered_list_link_type *)&block->task_link, 0);

      /* update overall mem stats */
      mm_update_mem_stat(&mm_total_mem_stat.mem_stat, old_size, -1, block->ref_id);
      mm_update_mem_stat(&mm_total_mem_stat.mem_stat, new_size, 1, block->ref_id);
    }
    /* if (block != NULL) */
    else
    {
      /* not found */
      MSG_ERROR("MALLOC_MANAGER: Record not found for Realloc: 0x%d to 0x%d", old_ptr, new_ptr, 0);
      return 0;
    }
  } /* realloc operation */

  return 1;
}

/*===========================================================================

FUNCTION      mm_update_task_usage

DESCRIPTION
              updates the usage number of the current tasks;
              originally used in 6K for oemheap to notify the manager of 
              any changes made to allocations;
              the original logic has moved to a more generic mm_update()
              to allow multiple heaps to be tracked
              
DEPENDENCIES
  None

RETURN VALUE
              returns 1 if successful
              returns 0 otherwise
SIDE EFFECTS
  mm_tasks are updated accordingly

===========================================================================*/
int mm_update_task_usage(uint32 old_size, uint32 new_size, void* old_ptr, void* new_ptr)
{
  if (mm_internal_allocation_active)
    return 0;
  
  /* make sure malloc manager is properly intialized */
  if (!mm_initialized) 
    mm_init();

  if (!mm_default_heap)
  {
    mm_default_heap = (heap_t*)mm_register_heap(NULL, NULL, NULL);
  }

  return mm_update((heap_handle_t)mm_default_heap, old_size, new_size, old_ptr, new_ptr);
}

/*===========================================================================

FUNCTION      mm_malloc

DESCRIPTION
              stores relevant information regarding the malloc in additional
              to performing the malloc

              a group id is passed in to associate the malloc with a group

DEPENDENCIES
  None

RETURN VALUE
              returns exactly what regular malloc() would have returned even
              if there are errors internal to the manager
SIDE EFFECTS
  None

===========================================================================*/
void* mm_malloc(uint32 size, group_ID_t grp, const char* file_name, uint32 line_number)
{
  void* ptr = malloc(size);

  if (!mm_enable_tracking) 
    return ptr;
  
  if (ptr)
  {
    /* free tracking blocks available */
    malloc_usage_block_t *block = (malloc_usage_block_t *)ordered_list_pop_front(&mm_usage_blocks_freelist);

    if (!block)
    {
      mm_out_of_usage_blocks = TRUE;
      MSG_ERROR("MALLOC_MANAGER: Not enough tracking blocks to track malloc: 0x%d", ptr, 0, 0);
      return ptr;
    }

    block->malloc_ptr = ptr;
    block->malloc_size = size;
    block->group = grp;
    block->file_name = file_name;
    block->line_number = line_number;
    block->ref_id = mm_current_ref_id;
    block->task = mm_get_current_task_id();
    if (mm_enable_stacktrace) 
      block->stacktrace = mm_unwind_stack();
    else
      block->stacktrace = NULL;

    /* update heap mem stats */
    mm_update_mem_stat(&mm_grouping_heap.mem_stat, block->malloc_size, 1, block->ref_id);

    /* update group mem stats */
    mm_update_group_mem_stat(grp, block->malloc_size, 1, block->ref_id);

    /* insert tracking block into the front of the used list */
    ordered_list_push(&mm_grouping_heap.blocklists[HASH_INDEX(ptr)], 
                      (ordered_list_link_type *)block, 0);
  }
  return ptr;
}

/*===========================================================================

FUNCTION      mm_free

DESCRIPTION
              performs a regular free() as well as releasing internal book-keeping
              structures

              file_name and line_number can be obtained using __FILE__ and __LINE__

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 */
void mm_free(void* ptr, const char* file_name, uint32 line_number)
{
  malloc_usage_block_t *block = (malloc_usage_block_t *)ordered_list_linear_search(
      &mm_grouping_heap.blocklists[HASH_INDEX(ptr)], mm_usage_block_search, ptr);

  if (block != NULL)
  {
    /* found */
    block->malloc_ptr = NULL;
    block->file_name = file_name;
    block->line_number = line_number;

    /* update heap mem stats */
    mm_update_mem_stat(&mm_grouping_heap.mem_stat, block->malloc_size, -1, block->ref_id);

    /* update group mem stats */
    mm_update_group_mem_stat(block->group, block->malloc_size, -1, block->ref_id);

    if (block->stacktrace) 
    {
      list_push_front(&mm_stacktrace_blocks_freelist, (list_link_type *)block->stacktrace);
      block->stacktrace = NULL;
    }
    ordered_list_pop_item(&mm_grouping_heap.blocklists[HASH_INDEX(ptr)], (ordered_list_link_type *)block);
    ordered_list_push(&mm_usage_blocks_freelist, (ordered_list_link_type *)block, 0);
  }

  free(ptr);

}
/*lint -restore*/

/*===========================================================================

FUNCTION      mm_reset_reference

DESCRIPTION
              Mark new reference point

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_reference ( void )
{
  uint32 i;

  mm_current_ref_id++;
  /* clear the ref list by pop-ing everything in it */
  while (ordered_list_size(&mm_usage_blocks_reflist))
  {
    ordered_list_link_type* link = ordered_list_pop_back(&mm_usage_blocks_reflist);
    if (link)
    {
      link->next_ptr = NULL;
      link->prev_ptr = NULL;
    }
  }

  mm_reset_mem_stat_reference(&mm_total_mem_stat.mem_stat);

  for (i = 0; i < mm_total_mem_stat.total_groups; i++)
  {
    mm_reset_mem_stat_reference(&mm_groups[i].mem_stat);
  }
  for (i = 0; i < mm_total_mem_stat.total_tasks; i++)
  {
    mm_reset_mem_stat_reference(&mm_tasks[i].mem_stat);
  }
  for (i = 0; i < mm_total_mem_stat.total_heaps; i++)
  {
    mm_reset_mem_stat_reference(&mm_heaps[i].mem_stat);
  }
}

/*===========================================================================

FUNCTION      mm_get_mem_stat (decrecated)

DESCRIPTION
              retrieves memory usage information associated with the group
              such as current heap usage and maximum peak usage recorded

              the function name is confusing once grouping by tasks is 
              introduced, however it is kept here for backward compatibility

              the function is strongly discouraged to be used

DEPENDENCIES
  None

RETURN VALUE
              returns 1 if successful
              when successful, the destinations specified will contain the 
              requested information
              returns 0 if failed due to invalid group ID passed in
SIDE EFFECTS
  None

===========================================================================*/
int mm_get_mem_stat(group_ID_t grp, uint32* max_heap_ptr, uint32* cur_heap_ptr)
{
  if (grp >= (group_ID_t)NUM_GROUPS)
  {
    return 0;
  }
  /* Work around for backward compatibility, max and current usage can now be
     negative but not used to */
  if (mm_groups[grp].mem_stat.ref_max_usage > 0) 
    *max_heap_ptr = (uint32)mm_groups[grp].mem_stat.ref_max_usage;
  else
    *max_heap_ptr = 0;

  if (mm_groups[grp].mem_stat.ref_current_usage > 0)
    *cur_heap_ptr = (uint32)mm_groups[grp].mem_stat.ref_current_usage;
  else
    *cur_heap_ptr = 0;

  return 1;
}

/*===========================================================================

FUNCTION      mm_reset_all_max_heap

DESCRIPTION
              resets the reference max usage of all groups (kept here
              for backward compatibility)

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_all_max_heap()
{
  mm_reset_ref_max_all_group();
}

/*===========================================================================

FUNCTION      mm_reset_ref_max_group

DESCRIPTION
              resets the reference max usage of the specified group

DEPENDENCIES
  None

RETURN VALUE
              returns 1 if successful
              returns 0 if failed due to invalid group ID passed in
SIDE EFFECTS
  None

===========================================================================*/
int mm_reset_ref_max_group (group_ID_t grp)
{
  if (grp >= mm_total_mem_stat.total_groups)
  {
    return 0;
  }
  mm_groups[grp].mem_stat.ref_max_usage = mm_groups[grp].mem_stat.ref_current_usage;
  return 1;
}

/*===========================================================================

FUNCTION      mm_reset_ref_max_all_group

DESCRIPTION
              resets the reference max usage of all mm_groups

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_ref_max_all_group()
{
  uint32 i;
  for (i = 0; i < mm_total_mem_stat.total_groups; i++)
  {
    mm_groups[i].mem_stat.ref_max_usage = mm_groups[i].mem_stat.ref_current_usage;
  }
}

/*===========================================================================

FUNCTION      mm_reset_ref_max_task

DESCRIPTION
              resets the reference max usage of the specified task

DEPENDENCIES
  None

RETURN VALUE
              returns 1 if successful
              returns 0 if failed due to invalid task ID passed in
SIDE EFFECTS
  None

===========================================================================*/
int mm_reset_ref_max_task (task_ID_t tsk)
{
  if (tsk >= mm_total_mem_stat.total_tasks)
  {
    return 0;
  }
  mm_tasks[tsk].mem_stat.ref_max_usage = mm_tasks[tsk].mem_stat.ref_current_usage;
  return 1;
}

/*===========================================================================

FUNCTION      mm_reset_ref_max_all_task

DESCRIPTION
              resets the reference max usage of all mm_tasks

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_ref_max_all_task()
{
  uint32 i;
  for (i = 0; i < mm_total_mem_stat.total_tasks; i++)
  {
    mm_tasks[i].mem_stat.ref_max_usage = mm_tasks[i].mem_stat.ref_current_usage;
  }
}

/*===========================================================================

FUNCTION      mm_is_group_inside_group

DESCRIPTION
              determine whether grpA is contained inside grpB
              A group is defined to be containing itself as well
DEPENDENCIES
  None

RETURN VALUE
              returns 1 if it contains
              returns 0 otherwise
SIDE EFFECTS
  None

===========================================================================*/
int mm_is_group_inside_group(group_ID_t grpA, group_ID_t grpB)
{
  uint32 i;
  if (grpA >= (group_ID_t)NUM_GROUPS || grpB >= (group_ID_t)NUM_GROUPS)
    return 0;

  if (grpA == grpB)
    return 1;

  for (i = 0; i < mm_groups[grpA].num_parent_groups; i++)
  {
    if (mm_is_group_inside_group(mm_groups[grpA].parent_groups[i], grpB))
      return 1;
  }
  return 0;
}

#endif /* MALLOC_LOGGING */






