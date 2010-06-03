#ifndef __MMGR_DIAG_H
#define __MMGR_DIAG_H
/*==========================================================================

                   Diagnostic Packet Definitions for EFS2

  Description: Definitions of malloc manager packets. These define the diagnostic
  interface between EFS2 and the external device.

  Copyright (c) 2002, 2003, 2004, 2005, 2006 by QUALCOMM, Incorporated.
  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //source/qcom/qct/multimedia/core/malloc_mgr/rel/1.0/inc/malloc_mgr_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/07   vma     Tracking of multiple heaps
04/17/07   vma     Use dynamic memory for tracking and stacktrace blocks
01/12/07   vma     Added support for turning on/off tracking/stack tracing on
                   the fly
10/31/06   vma     Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "malloc_mgr.h"

#ifdef MALLOC_LOGGING

/* -------------------------------------------------------------------------
 * Definitions of malloc manager diagnostic packets.
 * ------------------------------------------------------------------------- */

/*
 * Permitted operations.
 */
#define MM_DIAG_HELLO                 0 /* Hello packet               */
#define MM_DIAG_QUERY_MEM_STAT        1 /* Query_mem_stat packet      */
#define MM_DIAG_QUERY_GROUP           2 /* Query_group packet         */
#define MM_DIAG_QUERY_TASK            3 /* Query_task packet          */
#define MM_DIAG_QUERY_GRP_MALLOC      4 /* Query_grp_malloc packet    */
#define MM_DIAG_QUERY_TSK_MALLOC      5 /* Query_tsk_malloc packet    */
#define MM_DIAG_QUERY_MALLOC_DETAIL   6 /* Query_malloc_detail packet */
#define MM_DIAG_QUERY_REF_MALLOC      7 /* Query_ref_malloc packet    */
#define MM_DIAG_RESET_REFERENCE       8 /* Reset_reference packet     */
#define MM_DIAG_RESET_REF_MAX_OVRALL  9 /* reset_ref_max_ovrall packet*/
#define MM_DIAG_RESET_REF_MAX_GRP     10 /* reset_ref_max_grp packet  */
#define MM_DIAG_RESET_REF_MAX_TSK     11 /* reset_ref_max_task pakcet */
#define MM_DIAG_RESET_REF_MAX_ALLGRP  12 /* reset_ref_max_allgrp      */
#define MM_DIAG_RESET_REF_MAX_ALLTSK  13 /* reset_ref_max_alltsk      */
#define MM_DIAG_CONFIGURE             14 /* configure malloc manger   */
#define MM_DIAG_QUERY_HEAP            15 /* Query_heap packet         */
#define MM_DIAG_QUERY_HEAP_MALLOC     16 /* Query_heap_malloc packet  */

/*
 * Error number
 */
#define MM_DIAG_GENERIC_ERROR       1 /* Generic error number         */
#define MM_DIAG_INVALID_GROUP       2 /* Invalid group ID             */
#define MM_DIAG_INVALID_TASK        3 /* Invalid task ID              */
#define MM_DIAG_INVALID_MALLOC_ID   4 /* Invalid malloc ID            */
#define MM_DIAG_MALLOC_NOT_FOUND    5 /* Malloc not found             */
#define MM_DIAG_OUT_OF_BLOCKS       6 /* Malloc is running out of usage blocks */
#define MM_DIAG_OUT_OF_MEMORY       7 /* Not enough memory for manager to 
                                         allocate memory for internal use */
#define MM_DIAG_INVALID_HEAP        8 /* Invalid heap handle          */

/* -------------------------------------------------------------------------
 * EXTERN FUNCTIONS
 * ------------------------------------------------------------------------- */
void mm_diag_init(void);

/* -------------------------------------------------------------------------
 * PACKET STRUCTURES
 * ------------------------------------------------------------------------- */

/* 
 * General request/response packet 
 * (Request packet containing no extra parameters and
 * response packet containing only error number)
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
} mm_diag_general_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
} mm_diag_general_rsp_type;

/*
 * Hello Packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
} mm_diag_hello_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
  uint32 version_number;           /* Version number */
  uint8 enable_tracking;           /* Flag indicating whether tracking is enabled */
  uint8 enable_stacktrace;         /* Flag indicating whether stacktracing is enabled */
  uint32 num_tracking_blks_allocated;   /* number of tracking blocks allocated */
  uint32 num_stacktrace_blks_allocated; /* number of stacktrace blocks allocated */
  uint32 num_tracking_blks_in_use;   /* number of tracking blocks allocated */
  uint32 num_stacktrace_blks_in_use; /* number of stacktrace blocks allocated */
} mm_diag_hello_rsp_type;

/*
 * Query_mem_stat Packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
} mm_diag_query_mem_stat_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32       diag_errno;      /* Error code if error, 0 otherwise */
  uint32      total_groups;    /* Total number of groups tracked */
  uint32      total_tasks;     /* Total number of tasks tracked */
  mem_stat_t  mem_stat;        /* memory statistics */  
  uint32      total_heaps;     /* Total number of heaps tracked (added to the end
                                  of structure for backward compatibility) */
} mm_diag_query_mem_stat_rsp_type;

/*
 * Query_group Packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 group_id;           /* ID number for the group to be queried. If n is the total number 
                                of groups reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid group IDs */
} mm_diag_query_group_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32  diag_errno;            /* Error code if error, 0 otherwise             */
  mem_stat_t stat;              /* memory usage statistics */
  uint32 num_parents;           /* Number of parents of this group */
  uint32 parent_id_array[1];    /* Array of parent groups id (only one placeholder is put here) */
  char   group_name[1];         /* Null-terminated string */
} mm_diag_query_group_rsp_type;

/*
 * Query_task Packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 task_id;           /* ID number for the task to be queried. If n is the total number 
                               of tasks reported by the QUERY_MEM_STAT message, only 0 
                               through n-1 would be valid task IDs */
} mm_diag_query_task_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32  diag_errno;            /* Error code if error, 0 otherwise             */
  mem_stat_t stat;              /* memory usage statistics */
  char   task_name[1];          /* null-terminated string */

} mm_diag_query_task_rsp_type;

/*
 * Query_heap Packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 heap_id;           /* ID number for the heap to be queried. If n is the total number 
                               of heaps reported by the QUERY_MEM_STAT message, only 0 
                               through n-1 would be valid heap IDs */
} mm_diag_query_heap_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32           diag_errno;   /* Error code if error, 0 otherwise             */
  mem_stat_t      stat;         /* memory usage statistics */
  uint32          used_bytes;   /* used size of the heap in bytes */
  uint32          total_bytes;  /* total size of the heap in bytes */
  char            heap_name[1]; /* null-terminated string */

} mm_diag_query_heap_rsp_type;

/*
 * Query group malloc packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 group_id;           /* ID number for the group to be queried. If n is the total number 
                                of groups reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid group IDs */
  uint32 malloc_start_id;    /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_GROUP message, only 0 through 
                                n-1 would be valid malloc IDs */ 
  uint32 malloc_end_id;      /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_GROUP message, only 0 through 
                                n-1 would be valid malloc IDs */ 
} mm_diag_query_grp_malloc_req_type;

typedef PACKED struct {
  void    *ptr;          /* pointer of allocation */
  uint32   size;         /* size of allocation */
  uint32   line_num;     /* line number of allocation */
  uint32   group_id;     /* group id */
  uint32   task_id;      /* task id  */
  uint32   heap_id;      /* heap id  */
  char     file_name[4]; /* file name where the allocations were made (4 chars to make this structure
                            4 bytes-aligned*/
} mm_diag_malloc_info_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
  uint32 allocations_returned;     /* Number of allocations returned. May be less than what 
                                      was requested; possibly due to:
                                        -	out of range IDs requested
                                        -	allocations were freed
                                        -	limited by the maximum response message size */
  mm_diag_malloc_info_type minfo[1];    /* malloc info array */
} mm_diag_query_grp_malloc_rsp_type;

/*
 * Query task malloc packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 task_id;            /* ID number for the task to be queried. If n is the total number 
                                of tasks reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid task IDs */
  uint32 malloc_start_id;    /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_TASK message, only 0 through 
                                n-1 would be valid malloc IDs */ 
  uint32 malloc_end_id;      /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_TASK message, only 0 through 
                                n-1 would be valid malloc IDs */ 
} mm_diag_query_tsk_malloc_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
  uint32 allocations_returned;     /* Number of allocations returned. May be less than what 
                                      was requested; possibly due to:
                                        -	out of range IDs requested
                                        -	allocations were freed
                                        -	limited by the maximum response message size */
  mm_diag_malloc_info_type minfo[1];    /* malloc info array */
} mm_diag_query_tsk_malloc_rsp_type;

/*
 * Query heap malloc packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 heap_id;            /* ID number for the heap to be queried. If n is the total number 
                                of heaps reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid heap IDs */
  uint32 malloc_start_id;    /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_HEAP message, only 0 through 
                                n-1 would be valid malloc IDs */ 
  uint32 malloc_end_id;      /* Start of the ID number of the malloc. If n is the total number 
                                of allocations reported by the QUERY_HEAP message, only 0 through 
                                n-1 would be valid malloc IDs */ 
} mm_diag_query_heap_malloc_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
  uint32 allocations_returned;     /* Number of allocations returned. May be less than what 
                                      was requested; possibly due to:
                                        -	out of range IDs requested
                                        -	allocations were freed
                                        -	limited by the maximum response message size */
  mm_diag_malloc_info_type minfo[1];    /* malloc info array */
} mm_diag_query_heap_malloc_rsp_type;

/*
 * Query malloc detail packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  void  *ptr;                /* pointer of allocation */
  uint32 heap_id;            /* ID number for the heap the allocation belongs to */
} mm_diag_query_malloc_detail_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;          /* Error code if error, 0 otherwise             */
  uint32  group_id;          /* Group ID */
  uint32  task_id;           /* Task ID */
  uint32  heap_id;           /* Heap ID */
  void   *ptr;               /* Malloc pointer */
  uint32  size;              /* size of malloc */
  uint32  line_num;          /* Line number */
  int32   stack_depth;       /* Depth of stack trace */
  void   *stack_trace[1];    /* Stack trace (array of pointers) */
  char    file_name[1];      /* File name of the malloc; null-terminated string */
} mm_diag_query_malloc_detail_rsp_type;

/*
 * Query mem malloc packet since last ref point
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 malloc_start_id;    /* Start of the ID number of the malloc. If n is the total number 
                                of ref_allocations reported by the QUERY_MEM_STAT message, 
                                only 0 through n-1 would be valid malloc IDs */
  uint32 malloc_end_id;      /* Start of the ID number of the malloc. If n is the total number 
                                of ref_allocations reported by the QUERY_MEM_STAT message, 
                                only 0 through n-1 would be valid malloc IDs */
} mm_diag_query_ref_malloc_req_type;

typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  int32 diag_errno;                /* Error code if error, 0 otherwise             */
  uint32 allocations_returned;     /* Number of allocations returned. May be less than what 
                                      was requested; possibly due to:
                                        -	out of range IDs requested
                                        -	allocations were freed
                                        -	limited by the maximum response message size */
  mm_diag_malloc_info_type minfo[1];    /* malloc info array */
} mm_diag_query_ref_malloc_rsp_type;

/*
 * Reset max group packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 group_id;           /* ID number for the group to be resetted. If n is the total number 
                                of groups reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid group IDs */
} mm_diag_reset_ref_max_grp_req_type;

/*
 * Reset max task packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint32 task_id;           /* ID number for the task to be resetted. If n is the total number 
                                of tasks reported by the QUERY_MEM_STAT message, only 0 
                                through n-1 would be valid task IDs */
} mm_diag_reset_ref_max_tsk_req_type;

/*
 * Configuration packet
 *
 * Description
 */
typedef PACKED struct {
  diagpkt_subsys_header_type hdr;
  uint8  enable_tracking;       /* flag to turn tracking on/off, fill with all 1's to stay unchanged */
  uint8  enable_stacktrace;     /* flag to turn stacktracing on/off; fill with all 1's to stay unchanged */
  uint32 num_tracking_blocks;   /* specifies how many tracking blocks to allocate for storage, 
                                   fill with all 1's to stay unchanged */
  uint32 num_stacktrace_blocks; /* specifies how many blocks to allocate for stacktrace storage, 
                                   fill with all 1's to stay unchanged */
} mm_diag_configure_req_type;

#endif /* MALLOC_LOGGING */

#endif /* __MMGR_DIAG_H */

