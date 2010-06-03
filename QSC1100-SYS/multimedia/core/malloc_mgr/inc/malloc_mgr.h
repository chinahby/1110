#ifndef MALLOC_MANAGER_H
#define MALLOC_MANAGER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  M A L L O C     M A N A G E R 

GENERAL DESCRIPTION

         Keeps track of malloc's and free's for heap usage statistics

Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/common/malloc_mgr.h#1

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/07   vma     Removed dependency on stack unwind function from BREW by 
                   maintaining an own copy
08/08/07   vma     Tracking of multiple heaps
07/10/07   vma     Improve performance by using doubly-linked list and 
                   hashtable to hold usage blocks
05/12/07   vma     Use task name for matching instead of tcb pointer
04/17/07   vma     Use dynamic memory for tracking and stacktrace blocks
01/12/07   vma     Added support for turning on/off tracking/stack tracing on
                   the fly
11/29/06   vma     Major enhancement. Includes diag interface to communicate
                   with PC side tool/APS. Groups allocations by rex tasks.
10/24/06   vma     Added mm_init function for initialization
05/17/06   vma     Added max heap reset functions, turn off logging by default
05/15/06   vma     Initial version

===========================================================================*/
#include "comdef.h"             /* customer specific definitions           */
#include "list.h"
#include "rex.h"
#include "task.h"

typedef enum {
  JPEG_GROUP = 0,
  IPL_GROUP,
  CAMERA_SVCS_GROUP,
  CAMERA_COMBINED,
  NUM_GROUPS
} GROUP_ENUM;

#ifndef MALLOC_LOGGING

#define MM_MALLOC(size,grp,file,line)  malloc(size)
#define MM_FREE(ptr,file,line)         free(ptr)

#else

#define MM_MALLOC(size, grp, file, line)  mm_malloc(size, (group_ID_t)grp, file, line)
#define MM_FREE(ptr, file, line)          mm_free(ptr, file, line)

#define UNGROUPED                    (0xffffffff)
#define MAX_TASKS                    MAX_PRI_ORDER_NUM
#define MAX_HEAPS                    20
#define MAX_PARENT_GROUPS            1
#define DEFAULT_MEMORY_USAGE_BLOCKS  6000
#define DEFAULT_STACKTRACE_BLOCKS    600
#define STACKTRACE_BUFSIZE           120
#define MM_NUM_HASH_ENTRIES          64
#define HASH_INDEX(ptr)              ((((uint32)ptr)>>5)%MM_NUM_HASH_ENTRIES)

/* 
   Reference stack unwind function from OEMARMFastBacktrace.s; 
   Make sure the following line is present in .builds file to enable the function:

   arm_stacktrace makeonly
*/
extern signed long MALLOCMGR_UNWIND_STACK(void **bt_buffer, unsigned long bt_size);

/* heap handle */
typedef uint32 heap_handle_t;

/* type of heap ID */
typedef uint32 heap_ID_t;

/* type of group ID */
typedef uint32 group_ID_t;

/* type of task ID */
typedef uint32 task_ID_t;

/* heap info that is to be filled by heap implementations if available */
typedef PACKED struct 
{
  uint32  version;         /* version of the heap_info_t in case additional info are to
                              be supported and appended to the end of the structure */
  char*   name;            /* name of the heap */
  uint32  used_bytes;      /* used size of the heap in bytes */
  uint32  total_bytes;     /* total size of the heap in bytes */

  /* version 1 of mm_heap_info_t up to here */
} mm_heap_info_t;

/* allocation info that is to be filled by heap implementations if available */
typedef struct 
{
  uint32  version;         /* version of the mm_allocation_info_t in case additional info
                              are to be supported and appended to the end of the structure */
  void*   payload_pointer; /* payload pointer of the allocation on heap */
  uint32  payload_size;    /* payload size of the allocation on heap */
  void*   actual_pointer;  /* actual pointer of the allocation on heap */
  uint32  actual_size;     /* actual size of the allocation on heap */
  char*   file_name;       /* name of the source file where the allocations are made */
  uint32  line_number;     /* line number in the source file where the allocations are made */  

  /* version 1 of mm_allocation_info_t up to here */
} mm_allocation_info_t;

/* callback functions that would be called on heap implementations so that different heaps can 
   provide information about their allocations or themselves */
typedef int (*mm_heap_info_query_func_type) (void* func_arg, mm_heap_info_t* heap_info);
typedef int (*mm_allocation_info_query_func_type) (void* func_arg, void* pointer, mm_allocation_info_t* allocation_info);

typedef PACKED struct 
{
  uint32       current_usage;                         /* Current usage */
  uint32       max_usage;                             /* Maximum usage */
  uint32       allocations;                           /* # allocations outstanding */
  int32        ref_current_usage;                     /* Current usage since last reset of reference point */
  int32        ref_max_usage;                         /* Maximum usage since last reset of reference point */
  int32        ref_allocations;                       /* # allocations outstanding since last reset of ref pt */
  uint32       ref_new_allocations;                   /* # new allocations outstanding since last reset of ref pt */
} mem_stat_t;

typedef PACKED struct 
{
  uint32       total_groups;                          /* Total number of groups tracked */
  uint32       total_tasks;                           /* Total number of tasks tracked */
  uint32       total_heaps;                           /* Total number of heaps tracked */
  mem_stat_t   mem_stat;                              /* memory usage stats */
} total_mem_stat_t;

/* structure of a heap */
typedef struct
{
  heap_ID_t                          heap_ID;         /* Unique heap ID */
  mm_heap_info_query_func_type       heap_info_q_func;  
                                                      /* query function pointer for heap info */
  mm_allocation_info_query_func_type allocation_info_q_func;
                                                      /* query function pointer for allocation info */
  void*                              func_arg;        /* the function argument to be passed to the 
                                                         query functions */
  mem_stat_t                         mem_stat;        /* memory usage stats */
  ordered_list_type                  blocklists[MM_NUM_HASH_ENTRIES];
                                                      /* A hashtable of lists of usage blocks; */
} heap_t;

/* structure of a group */
typedef struct 
{
  const char  *name;                                  /* Group name */
  group_ID_t   group_ID;                              /* Unique group ID */
  group_ID_t   parent_groups[MAX_PARENT_GROUPS];      /* Array of group IDs of the parents */
  uint32       num_parent_groups;                     /* Number of parents of the group */
  mem_stat_t   mem_stat;                              /* memory usage stats */
} group_t;

/* structure of a task */
typedef struct
{
  char         task_name[REX_TASK_NAME_LEN + 1];      /* Task name */
  task_ID_t    task_ID;                               /* Unique task ID */
  ordered_list_type      blocklists;                  /* Linked list of usage blocks */
  mem_stat_t   mem_stat;                              /* memory usage stats */
} task_t;

/* stacktrace holder block */
typedef struct
{
  /* list link */
  list_link_type     link;
  /* The length of the valid portion of the stack buffer */
  int32              stacktrace_len;
  /* The stack capture when the malloc was made */
  void*              stacktrace[STACKTRACE_BUFSIZE];
} stacktrace_block_t;

/* heap usage tracking block */
typedef struct
{
  /* list link */
  ordered_list_link_type  link;
  /* list link to be inserted into task list */
  ordered_list_link_type  task_link;
  /* list link to be inserted into the reference list */
  ordered_list_link_type  ref_link;
  /* The group the malloc belongs to */
  group_ID_t         group;
  /* The task which the malloc was made */
  task_ID_t          task;
  /* The heap where the allocation belongs */
  heap_ID_t          heap;
  /* Size of the malloc in bytes */
  uint32             malloc_size;
  /* Returned pointer of the malloc */
  void              *malloc_ptr;
  /* Specify in which file the malloc was located */
  const char        *file_name;
  /* The line number in which the malloc was called*/
  uint32             line_number;
  /* Ref ID */
  uint32             ref_id;
  /* stacktrace */
  stacktrace_block_t* stacktrace;
} malloc_usage_block_t;

/* extern variables */
extern uint32                mm_current_ref_id;               /* current ref id */
extern total_mem_stat_t      mm_total_mem_stat;               /* malloc mgr overall statistics */
extern task_t                mm_tasks[MAX_TASKS];             /* tasks information */
extern group_t               mm_groups[NUM_GROUPS];           /* groups information */
extern heap_t                mm_heaps[MAX_HEAPS];             /* heaps information */
extern heap_t               *mm_default_heap;                 /* default heap - used in 6K 
                                                                 as there is only one heap tracked */
extern heap_t                mm_grouping_heap;                /* Grouping heap - treated as a separate heap to track all
                                                                 allocations using the mm_malloc() interface */
//extern ordered_list_type     mm_usage_blocks_usedlist[];    /* list of usage blocks in use */
extern ordered_list_type     mm_usage_blocks_freelist;        /* list of usage blocks free */
extern ordered_list_type     mm_usage_blocks_reflist;         /* list of usage blocks made after reference point */
extern list_type             mm_heap_list;                    /* list of heap info structures */
extern list_type             mm_stacktrace_blocks_freelist;   /* list of stacktrace blocks free */
extern malloc_usage_block_t *mm_usage_blocks;                 /* usage blocks */
extern stacktrace_block_t   *mm_stacktrace_blocks;            /* blocks holding stacktrace information */
extern boolean               mm_out_of_usage_blocks;          /* flag indicating whether out of block state is hit*/
extern boolean               mm_enable_tracking;              /* flag indicating whether tracking is enabled */
extern boolean               mm_enable_stacktrace;            /* flag indicating whether stacktracing is enabled */

extern uint32  mm_num_usage_blocks;                           /* number of usage blocks allocated */
extern uint32  mm_num_stacktrace_blocks;                      /* number of stacktrace blocks allocated */

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
void mm_init(void);

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
boolean mm_allocate_stacktrace_blocks(void);

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
boolean mm_allocate_usage_blocks(void);

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
);

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
void* mm_malloc(
  uint32          size, 
  group_ID_t      grp, 
  const char     *file_name, 
  uint32          line_number
);

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
void mm_free(
  void* ptr, 
  const char* file_name, 
  uint32 line_number
);

/*===========================================================================

FUNCTION      mm_reset_reference

DESCRIPTION
              Mark a new reference point

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void mm_reset_reference ( void );

/*===========================================================================

FUNCTION      mm_get_mem_stat

DESCRIPTION
              retrieves memory usage information associated with the group
              such as current heap usage and maximum peak usage recorded

              the function name is confusing once grouping by tasks is 
              introduced, however it is kept here for backward compatibility

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
int mm_get_mem_stat(
  group_ID_t           grp, 
  uint32              *max_heap_ptr, 
  uint32              *cur_heap_ptr
);

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
void mm_reset_all_max_heap(void);

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
int mm_reset_ref_max_group (group_ID_t grp);

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
void mm_reset_ref_max_all_group( void );

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
int mm_reset_ref_max_task (task_ID_t tsk);

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
);

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
void mm_reset_ref_max_all_task( void );

int mm_update_task_usage(uint32 old_size, uint32 new_size, void* old_ptr, void* new_ptr);

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
int mm_is_group_inside_group(group_ID_t grpA, group_ID_t grpB);

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
int mm_usage_block_search(void *usage_block, void *ptr);

#endif /* MALLOC_LOGGING */

#endif /* MALLOC_MANAGER_H */

