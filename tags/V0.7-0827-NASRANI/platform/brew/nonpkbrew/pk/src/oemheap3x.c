/*===========================================================================
                              OEMHeap.c

  OEM REFERENCE HEAP ROUTINES

  This file contains a reference implementation of a heap.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/nonpkbrew_3.1.5/pk/src/oemheap3x.c#1 $

============================================================================*/

#include "OEMFeatures.h"

#ifdef OEMHEAP

#include "AEEComdef.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "rex.h"
#include "AEEkHeap.h"

#ifdef MALLOC_LOGGING
#include "malloc_mgr.h"
#endif

#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "OEMOS.h"
#include "dog.h"

#ifdef T_QSC60X0
#include "uih.h"
#else
#include "ui_base.h"
#endif

#include "nv_items.h"

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#include "assert.h"
#include "fs_public.h"

// Define HEAP_DEBUGGING to enable heap debugging
// #define HEAP_DEBUGGING


// The HEAP_DEBUGGING features can not coexist with FEATURE_BREW_HEAP_TRACKER features
#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#if defined(HEAP_DEBUGGING)
#error Cannot use FEATURE_BREW_HEAP_TRACKER and HEAP_DEBUGGING together
#endif

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)


// Define HEAP_DEBUGGING_DEBUG_MEM_OUT to dump contents of heap to a file
// and throw an asset when a memory allocation fails.  Requires that
// HEAP_DEBUGGING be defnied as well
// #define HEAP_DEBUGGING_DEBUG_MEM_OUT

// Enable HEAP_DEBUGGING_FIND_NODE_IN_HEAP to check the
// entire heap for the node as part of ValidateNode.  This takes
// a significant amout of time. Requires HEAP_DEBUGGING be enabled
// #define HEAP_DEBUGGING_FIND_NODE_IN_HEAP

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
// Define HEAP_FREE_DELAY to a number of frees to delay by
// This is used to help catch code that free's memory twice.
#define HEAP_FREE_DELAY 1000
static uint32 heap_free_delay = HEAP_FREE_DELAY;
#define DELAYED_FREE 2
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef HEAP_DEBUGGING
#define MEMORY_OVERWRITE_GUARD 0xABCDEF98
#define MEMORY_OVERWRITE_BLOCK(node) (((uint32 *)(node))[(node)->dwSize / 4 - 1])
#define MEMORY_OVERWRITE_SIZE sizeof(uint32)
#endif

//
// Local definitions and structures...
//

#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

  nv_item_type heaptracker_nv_data = { 0 };

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#ifdef FEATURE_BREW_HEAP_INIT_MEM_ON_FREE
#define INIT_MEM_ON_FREE__DEFAULT_VALUE (0xFE)
boolean zf_enabled = FALSE; // zf -- zap on free

// Since the OEM heap is used by tasks other than UI, it is possible
// that these tasks abuse the heap in a way that is not easy to 
// resolve (this has, in fact, happened with the VS task so far).  
// For this reason we restrict the initialize-on-free feature to the
// UI task only.  

static boolean zap_mem_for_all_tasks = FALSE;
static __inline boolean is_ui_task(void)
{
#ifdef FEATURE_L4
   rex_tcb_type *rex_curr_task = rex_self();
#else
   extern rex_tcb_type *rex_curr_task;
#endif
   extern rex_tcb_type ui_tcb;
   boolean zapit;

   INTLOCK(); /* enter scope */
   zapit = rex_curr_task == &ui_tcb;
   INTFREE(); /* leave scope */
   return zapit;
}

#endif//FEATURE_BREW_HEAP_INIT_MEM_ON_FREE

#ifdef FEATURE_BREW_HEAP_TRACKER
static int ht_num_failed_allocations;
boolean ht_enabled = FALSE; // memory-leak detection
#define HT_ENABLED (ht_enabled)
#else
#define HT_ENABLED (0)
#endif//FEATURE_BREW_HEAP_TRACKER

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
boolean df_enabled = FALSE; // double free
#define DF_ENABLED (df_enabled)
#else
#define DF_ENABLED (0)
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
boolean od_enabled = TRUE; // overwrite detect
// Array overwrite_guard[] must not be empty!
static const byte overwrite_guard[] = { 0xDE, 0xAD, 0xBE, 0xEF };
static __inline boolean set_guard(byte *mem)
{
   uint32 __s = sizeof(overwrite_guard);
   do 
   {
      *mem++ = overwrite_guard[--__s];
   } while(__s);
}
static __inline boolean check_guard(byte *mem)
{
   uint32 __s = sizeof(overwrite_guard);
   do 
   {
      if (*mem++ != overwrite_guard[--__s]) 
         return FALSE;
   } while(__s);
   return TRUE;
}
#define OVERWRITE_PADDING (sizeof(uint32) * 10) // must be >= 2*sizeof(uint32)
static uint32 heap_overwrite_padding = OVERWRITE_PADDING;
#define OD_ENABLED (od_enabled)
#else
#define OD_ENABLED (0)
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT

/* Critical sections are handled by the AEEHeap layer in BREW 3.x and above
#define BEGIN_CRITICAL_SECTION()   rex_enter_crit_sect(&crit_sect)
#define END_CRITICAL_SECTION()     rex_leave_crit_sect(&crit_sect)
#define INITIALIZE_CRITICAL_SECTION()    rex_init_crit_sect(&crit_sect)
static rex_crit_sect_type crit_sect;

*/
#define BEGIN_CRITICAL_SECTION()
#define END_CRITICAL_SECTION()
#define INITIALIZE_CRITICAL_SECTION()

#define FREE_BIN_COUNT     32 // do not change this number without also modifying FastFindBin()
#define INVALID_BIN 0xFFFFFFFF

typedef struct _MemHeap MemHeap;

#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

typedef struct common_info_struct {
   uint32 user_allocation_size;
} common_info_t;

typedef struct backtrace_info_struct {
  signed long    bt_length;  // length (depth) of the backtrace buffer in bytes
  boolean        bt_partial; // TRUE: trace is full, FALSE: partial
  uint32         bt_timestamp;  // number of milliseconds since the last reset.
  void **        backtrace;
} backtrace_info_t;

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
typedef struct doublefree_info_struct {
   struct _HNode *pFreeTrace;
} doublefree_info_t;
static boolean print_double_free_allocation(MemHeap *heap, const char *modname, void *ptr);
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
static boolean print_overwritten_allocation(void *ptr);
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT

#define MAX_BT_BUFFER_LENGTH (100)

#if defined(FEATURE_BREW_HEAP_TRACKER) \
 || defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) \
 || defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

static int bt_buffer_length = MAX_BT_BUFFER_LENGTH*sizeof(void *);
static void* bt_buffer[MAX_BT_BUFFER_LENGTH];
static int open_file_to_append(const char *);
static int close_file(int);

static
boolean
print_allocation_to_open_file(int fd, /* throwback pointer */
                              const backtrace_info_t *bt_info, /* backtrace info */
                              const void *alloc_data, /* allocation data */
                              unsigned long alloc_size, /* size of allocation as req by app */
                              const char * msg); /* textual description of trace */

#endif // defined(FEATURE_BREW_HEAP_TRACKER) ||
       // defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) ||
       // defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

typedef struct _HNode
{
   uint32         dwSize;
   struct _HNode *pPrev;
   byte           bFree; // FALSE (0), TRUE (1), and DELAYED (2)
   byte           bLast;
   byte           nLocks;
   byte           nExtra;
} HNode;

typedef struct _HBusyNode
{
   HNode     header;
   void    **ppHandle;
} HBusyNode;

typedef struct _HLink
{
   HNode    *pPrev;
   HNode    *pNext;
} HLink;

typedef struct _HFreeNode
{
   HNode  header;
   HLink  link;
} HFreeNode;

struct _MemHeap
{
   HNode *        pFirstNode;
   HNode *        pFirstFreeNode[FREE_BIN_COUNT];
#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
   HNode *        pFirstDelayFreeNode;
   HNode *        pLastDelayFreeNode;
   uint32         pDelayFreeCount;
#endif
   uint32         dwTotalBytes;
   uint32         dwUsedBytes; // number of bytes in use, + overhead of HNodes for Free blocks
   uint32         dwFreeBlocks; // number of free blocks, used to calculate overhead (x HNode)
   uint32         init;
};

// Any value  > BINSIZE(X-1) and <= BINSIZE(X) belongs in BIN X
#define BINSIZE_OVERHEAD (sizeof(HNode))
#define BINSIZE0  (0x00000004 + BINSIZE_OVERHEAD)
#define BINSIZE1  (0x00000006 + BINSIZE_OVERHEAD)
#define BINSIZE2  (0x00000008 + BINSIZE_OVERHEAD)
#define BINSIZE3  (0x0000000C + BINSIZE_OVERHEAD)
#define BINSIZE4  (0x00000010 + BINSIZE_OVERHEAD)
#define BINSIZE5  (0x00000018 + BINSIZE_OVERHEAD)
#define BINSIZE6  (0x00000020 + BINSIZE_OVERHEAD)
#define BINSIZE7  (0x00000030 + BINSIZE_OVERHEAD)
#define BINSIZE8  (0x00000040 + BINSIZE_OVERHEAD)
#define BINSIZE9  (0x00000060 + BINSIZE_OVERHEAD)
#define BINSIZE10 (0x00000080 + BINSIZE_OVERHEAD)
#define BINSIZE11 (0x000000C0 + BINSIZE_OVERHEAD)
#define BINSIZE12 (0x00000100 + BINSIZE_OVERHEAD)
#define BINSIZE13 (0x00000180 + BINSIZE_OVERHEAD)
#define BINSIZE14 (0x00000200 + BINSIZE_OVERHEAD)
#define BINSIZE15 (0x00000300 + BINSIZE_OVERHEAD)
#define BINSIZE16 (0x00000400 + BINSIZE_OVERHEAD)
#define BINSIZE17 (0x00000600 + BINSIZE_OVERHEAD)
#define BINSIZE18 (0x00000800 + BINSIZE_OVERHEAD)
#define BINSIZE19 (0x00000C00 + BINSIZE_OVERHEAD)
#define BINSIZE20 (0x00001000 + BINSIZE_OVERHEAD)
#define BINSIZE21 (0x00001800 + BINSIZE_OVERHEAD)
#define BINSIZE22 (0x00002000 + BINSIZE_OVERHEAD)
#define BINSIZE23 (0x00003000 + BINSIZE_OVERHEAD)
#define BINSIZE24 (0x00004000 + BINSIZE_OVERHEAD)
#define BINSIZE25 (0x00006000 + BINSIZE_OVERHEAD)
#define BINSIZE26 (0x00008000 + BINSIZE_OVERHEAD)
#define BINSIZE27 (0x0000C000 + BINSIZE_OVERHEAD)
#define BINSIZE28 (0x00010000 + BINSIZE_OVERHEAD)
#define BINSIZE29 (0x00018000 + BINSIZE_OVERHEAD)
#define BINSIZE30 (0x00020000 + BINSIZE_OVERHEAD)
#define BINSIZE31 (0xFFFFFFFF)

#ifdef HEAP_DEBUGGING
unsigned long bin_sizes_array[FREE_BIN_COUNT] =
{
   BINSIZE0,
   BINSIZE1,
   BINSIZE2,
   BINSIZE3,
   BINSIZE4,
   BINSIZE5,
   BINSIZE6,
   BINSIZE7,
   BINSIZE8,
   BINSIZE9,
   BINSIZE10,
   BINSIZE11,
   BINSIZE12,
   BINSIZE13,
   BINSIZE14,
   BINSIZE15,
   BINSIZE16,
   BINSIZE17,
   BINSIZE18,
   BINSIZE19,
   BINSIZE20,
   BINSIZE21,
   BINSIZE22,
   BINSIZE23,
   BINSIZE24,
   BINSIZE25,
   BINSIZE26,
   BINSIZE27,
   BINSIZE28,
   BINSIZE29,
   BINSIZE30,
   BINSIZE31
};
#endif

static MemHeap         *gHeap3_x;

static void *pHeapWalkNode = NULL;
static void *pUpdatedHeapWalkNode = NULL;


// Private functions...

static HNode *       FindFreeNode(MemHeap *heap, uint32 inSizeNeeded);
static HNode *       SplitNode(MemHeap *heap, HNode * pb,uint32 dwActual);
static HNode *       FreeNode(MemHeap *heap, HNode *pb);
#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
static void          FreeAncillaryNode(MemHeap *heap, HNode *pb);
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
static void          UnlinkFreeNode(MemHeap *heap, HNode *pb);
static void          LinkFreeNode(MemHeap *heap, HNode *pb);
static void          UnlinkNode(HFreeNode *pb, HNode **pList);
static void          LinkNode(HNode *pb, HNode **pList);

static void          InitFreeNode(MemHeap *heap, HNode *pb, HNode *prev, uint32 inSize, byte bLast);
static HNode *       MergeFreeNode(MemHeap *heap, HNode *pb);
static HNode *       PushUnlockedNode(MemHeap *heap, HNode * pb);
static void          Sweep(MemHeap *heap);
static unsigned long FastFindBin(unsigned long value);
static boolean       QuickValidateNode(MemHeap *heap, HNode *node);

#define FREE_BYTE                ((byte)(0xfe))
#define GET_REAL_SIZE(nWant)     ((uint32)((((uint32)(nWant) + sizeof(HBusyNode) + sizeof(uint32) - 1) & ~(sizeof(uint32) - 1))))
#define PTR_TO_NODE(ptr)         ((HNode *)(((HBusyNode *)ptr) - 1))
#define NODE_TO_PTR(pb)          ((void *)((HBusyNode *)(pb) + 1))
#define NEXT_RIGHT_NODE(pb)      ((HNode *)((pb)->bLast ? NULL : (byte *)(pb) + (pb)->dwSize))
#define NODE_NEXT(pb)            ((pb)->bLast ? heap->pFirstNode : (HNode *)((byte *)(pb) + (pb)->dwSize))
#define NEXT_NODE(pb)            ((HNode *)((byte *)(pb) + (pb)->dwSize))
#define NODE_ALIGNED_SIZE(pbh)   ((pbh)->dwSize - sizeof(HBusyNode))
#define NODE_SIZE(pbh)           (NODE_ALIGNED_SIZE(pbh) - (pbh)->nExtra)
#define IS_FREE_NODE(phn)        ((phn)->bFree)
#define IS_USED_NODE(phn)        (!IS_FREE_NODE(phn))

#if defined(FEATURE_BREW_HEAP_TRACKER) || \
    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || \
    defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE) || \
    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

static __inline common_info_t* NODE_TO_COMMON_INFO(HNode *pb)
{
   return (common_info_t *)(((byte *)(((HBusyNode *)pb) + 1)) + 
                            NODE_ALIGNED_SIZE(pb) - 
                            sizeof(common_info_t));
}

static __inline uint32 USER_SIZE(HNode *pb)
{
   return NODE_TO_COMMON_INFO(pb)->user_allocation_size;
}

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
//
// NOTE: This function must be used only on busy (i.e., allocated) nodes!
// 
static __inline doublefree_info_t * NODE_TO_DF(HNode *pb)
{
   return ((doublefree_info_t *)NODE_TO_COMMON_INFO(pb)) - 1;
}
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_TRACKER
//
// NOTE: This function must be used only on busy (i.e., allocated) nodes!
// 
static __inline backtrace_info_t * NODE_TO_BT(HNode *pb)
{
   return (backtrace_info_t *)(((byte *)NODE_TO_COMMON_INFO(pb)) - sizeof(backtrace_info_t)
#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
                               - (df_enabled ? sizeof(doublefree_info_t) : 0)
#endif      
                               );
}

static __inline void INIT_ALLOC_BT(backtrace_info_t *bt,
                                   void **trace,
                                   signed long length,
                                   boolean partial)
{
   bt->bt_length    = length;
   bt->bt_partial   = partial;
   bt->bt_timestamp = OEMOS_GetUpTime();
   bt->backtrace    = (void **)((byte *)(bt) - length);
   memcpy(bt->backtrace, trace, length);
}

#endif//FEATURE_BREW_HEAP_TRACKER

#ifdef HEAP_DEBUGGING
static void        ValidateNode(MemHeap *heap, HNode *pb);
static void        ValidateAllNodes(boolean bUseQuickValidate);
#endif

#ifdef HEAP_DEBUGGING
#include "fs_public.h"
unsigned long heap_mark_malloc_count = 0;
unsigned long heap_mark_free_count   = 0;
unsigned long heap_mark_free_size    = 0;
unsigned long heap_mark_used_size    = 0;
unsigned long heap_mark_used_count   = 0;
unsigned long heap_mark_all_size     = 0;
unsigned long heap_mark_all_count    = 0;
unsigned long heap_malloc_count      = 0;

#define HEAPDUMPSTRSIZE 4096
static char HeapDumpStr[HEAPDUMPSTRSIZE];
static void HeapDump(MemHeap *heap, const char *tag, char increment)
{
   static int cnt = 0;
   int num = 0;
   int bin;
   int fd;
   unsigned long totalUsed    = 0;
   unsigned long totalFree    = 0;
   unsigned long totalMovable = 0;
   unsigned long countUsed    = 0;
   unsigned long countFree    = 0;
   unsigned long countMovable = 0;
   unsigned long maxUsed      = 0;
   unsigned long maxFree      = 0;
   unsigned long maxMovable   = 0;

   BEGIN_CRITICAL_SECTION();
   if(increment)
   {
      cnt++;
   }
   snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "heapdump%d-%s.txt", cnt, tag);

   fd = efs_open(HeapDumpStr, O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE);
   if(fd != -1)
   {
      register HNode *pb     = heap->pFirstNode;
      const char     *status = NULL;
      byte           *pEnd   = (byte *)pb + heap->dwTotalBytes;

      STRLCPY(HeapDumpStr, "   Num |   Addr   |   Used   |   Free   |  Movable |   Status  |\r\n", HEAPDUMPSTRSIZE);
      efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
      STRLCPY(HeapDumpStr, "-------|----------|----------|----------|----------|-----------|\r\n", HEAPDUMPSTRSIZE);
      efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
      while(pb && pb >= heap->pFirstNode && (byte *)pb < pEnd)
      {
         num++;

         status = "GOOD";
         if (IS_USED_NODE(pb) && MEMORY_OVERWRITE_BLOCK(pb) != MEMORY_OVERWRITE_GUARD)
         {
            status = "OverWrite";
         }
         else if (pb->pPrev != NULL && (pb->pPrev < heap->pFirstNode || pb->pPrev >= pb))
         {
            status = "BadPrev";
         }
         else if (NEXT_RIGHT_NODE(pb) != NULL && ((byte *)(NEXT_RIGHT_NODE(pb)) >= pEnd))
         {
            status = "BadNext";
         }

         snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "%6d | %8ld | %8ld | %8ld | %8ld | %9s \r\n", num, (unsigned long)pb, 
            (IS_FREE_NODE(pb) ? 0 : pb->dwSize), (IS_FREE_NODE(pb) ? pb->dwSize : 0), 
            (pb->nLocks == 0 ? pb->dwSize : 0), status);
         efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
         if(IS_FREE_NODE(pb))
         {
            totalFree += pb->dwSize;
            countFree++;
            if(pb->dwSize > maxFree)
            {
               maxFree = pb->dwSize;
            }
         }
         else
         {
            totalUsed += pb->dwSize;
            countUsed++;
            if(pb->dwSize > maxUsed)
            {
               maxUsed = pb->dwSize;
            }
         }
         if(pb->nLocks==0)
         {
            totalMovable += pb->dwSize;
            countMovable++;
            if(pb->dwSize > maxMovable)
            {
               maxMovable = pb->dwSize;
            }
         }
         pb = NEXT_RIGHT_NODE(pb);
      }
      snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, " Total |          | %8ld | %8ld | %8ld \r\n", totalUsed,
         totalFree, totalMovable);
      efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
      snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "   Max |          | %8ld | %8ld | %8ld \r\n", maxUsed,
         maxFree, maxMovable);
      efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
      snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, " Count |          | %8ld | %8ld | %8ld \r\n\r\n\r\nFree Bin Stats:\r\n", countUsed,
         countFree, countMovable);
      efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));

      for (bin = 0; bin < FREE_BIN_COUNT; bin++)
      {
         num = 0;
         pb = heap->pFirstFreeNode[bin];
         if (pb)
         {
            snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "Bin %d Size = %ld (+ node size(%d)) \r\n", bin,
               bin_sizes_array[bin] - BINSIZE_OVERHEAD, BINSIZE_OVERHEAD);
            efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
            STRLCPY(HeapDumpStr, "Entries: ", HEAPDUMPSTRSIZE);
            efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
            while(pb)
            {
               num++;
               snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "%ld", pb->dwSize);
               efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
               pb = ((HFreeNode *)pb)->link.pNext;
               if (pb)
               {
                  efs_write(fd, ", ", 2);
               }
               else
               {
                  efs_write(fd, "\r\n", 2);
               }
            }
            snprintf(HeapDumpStr, HEAPDUMPSTRSIZE, "Count: %d\r\n\r\n", num);
            efs_write(fd, HeapDumpStr, strlen(HeapDumpStr));
         }
      }

      efs_close(fd);
   }

   END_CRITICAL_SECTION();
}
#endif


/*==================================================================
Function: HeapFreeInternal

Description: Frees an allocated buffer

Prototype:
   void HeapFreeInternal(void * pBuff)

Parameter(s):
   pBuff:      Buffer to free

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
static void  HeapFreeInternal(MemHeap *heap, void * ptr)
{
   HNode *  pb;
   HNode * node;

   if(!heap->init || !ptr)
   {
      return;
   }

   pb = PTR_TO_NODE(ptr);

   if(pb < heap->pFirstNode || (char *) ptr >= (char *) heap->pFirstNode + heap->dwTotalBytes)
   {
      return; // Outside heap memory is NOP
   }

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
   if (pb->bFree == DELAYED_FREE && df_enabled)
   {
       print_double_free_allocation(heap, NULL, ptr);
       MSG_ERROR("HeapFreeInternal free block %p: DOUBLE FREE (SAVED LOGS)!", ptr, 0, 0);
#ifdef FEATURE_ASSERT_ON_DOUBLE_FREE
       ASSERT(FALSE);
#endif//FEATURE_ASSERT_ON_DOUBLE_FREE
       return;
   }
   else
#endif
   if(pb->bFree == TRUE)
   {
      MSG_ERROR("HeapFreeInternal free block %p: DOUBLE FREE!", ptr, 0, 0);
#ifdef FEATURE_ASSERT_ON_DOUBLE_FREE
      ASSERT(FALSE);
#endif//FEATURE_ASSERT_ON_DOUBLE_FREE
      return;
   }
// #ifdef HEAP_DEBUGGING
   else if (QuickValidateNode(heap, pb) == FALSE)
   {
      MSG_ERROR("HeapFreeInternal free block %p: FREEING GARBAGE!", ptr, 0, 0);
      ASSERT(FALSE);
      return;
   }
// #endif

   // Make sure nExtra is reasonable

   BEGIN_CRITICAL_SECTION();

   if(IS_USED_NODE(pb))
   {
      node = FreeNode(heap, pb);
      if (ptr == pHeapWalkNode)
      {
         pUpdatedHeapWalkNode = NODE_TO_PTR(node);
      }
   }

   END_CRITICAL_SECTION();
#ifdef MALLOC_LOGGING
   mm_update_task_usage(0, 0, NULL, ptr);
#endif
}

static __inline void HeapNode_AdjSentinel(HNode *phn, int nDiff)
{
   if ((void **)0 != ((HBusyNode *)phn)->ppHandle) 
   {
      *((HBusyNode *)phn)->ppHandle = (char *)(*((HBusyNode *)phn)->ppHandle) + nDiff;
   }
}

static __inline boolean HeapNode_CanMove(HNode *phn)
{
   return (IS_USED_NODE(phn) && (0 == phn->nLocks) &&
           ((void **)0 != ((HBusyNode *)phn)->ppHandle));
}


/*==================================================================
Function: HeapReallocInternal

Description: Allocates/Reallocates a buffer from the heap.

Prototype:
   void * HeapReallocInternal(MemHeap *heap, void *pIn, uint32 size)

Parameter(s):
   pBuff:      Buffer or NULL
   dwNewSize:  Size to realloc

Return Value:  void *
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/

#if defined(FEATURE_BREW_HEAP_TRACKER) \
 || defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) \
 || defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

extern signed long HT_ARM926EJS_UNWIND_STACK(void **bt_buffer, unsigned long bt_size);

static __inline int open_file_to_append(const char *fname)
{
  return efs_open(fname,
                  O_CREAT | O_APPEND | O_WRONLY,
                  S_IREAD | S_IWRITE);
}

static __inline int close_file(int fd)
{
  return efs_close(fd);
}

static __inline int write_to_file(int fileHandle, void *buffer, unsigned int buflen)
{
  int num_written = 0, num_written_now;
  while (num_written < buflen)
  {
    num_written_now = efs_write(fileHandle, buffer, buflen);
    if (num_written_now <= 0)
    {
      // EFS ran out of space or there was some other error.
      break;
    }
    num_written += num_written_now;
  }
  return num_written;
}

static
boolean
print_allocation_to_open_file(int fileHandle, /* throwback pointer */
                              const backtrace_info_t *bt_info, /* backtrace info */
                              const void *alloc_data, /* allocation data */
                                          unsigned long alloc_size, /* size of allocation as req by app */
                                          const char * msg) /* textual description of trace */
{
    static char buffer[512];
    int buflen;
    uint32 num_written;

    if (msg)
    {
        buflen = SPRINTF(buffer,
                         "*** ALLOCATION OF %d BYTES AT 0x%08X IS %dMS OLD (TRACE: %d FRAMES%s): %s ***\n",
                         alloc_size,
                         alloc_data,
                         bt_info->bt_timestamp,
                         bt_info->bt_length >> 2,
                         (bt_info->bt_partial ? ", (PARTIAL)" : ""),
                         msg);
    }
    else {
        buflen = SPRINTF(buffer,
                         "*** ALLOCATION OF %d BYTES AT 0x%08X IS %dMS OLD (TRACE: %d FRAMES%s) ***\n",
                         alloc_size,
                         alloc_data,
                         bt_info->bt_timestamp,
                         bt_info->bt_length >> 2,
                         (bt_info->bt_partial ? ", (PARTIAL)" : ""));
    }

    num_written = write_to_file(fileHandle, buffer, buflen);

    if (num_written == buflen)
    {
        uint16 index = 0, bt_number = bt_info->bt_length / 4;
        uint32 *trace = (uint32 *)bt_info->backtrace;

        buflen = num_written = 0;
        for(; index < bt_number; index++)
        {
            buflen = SPRINTF(buffer,
                             "\t%03d: %08x\n",
                             bt_number - 1 - index,
                             trace[index]);

            num_written = write_to_file(fileHandle, buffer, buflen);

            if (num_written != buflen)
            {
              break;
            }
        }

        // Put a blank space line traces.

        if (num_written == buflen)
        {
          buflen = 1;
          num_written = write_to_file(fileHandle, "\n", buflen);
        }

    }

    return num_written == buflen;
}

void print_trace(const char *fname, const char *desc)
{
  int fileHandle;
  backtrace_info_t bt_info;

  bt_info.bt_length = HT_ARM926EJS_UNWIND_STACK(bt_buffer, bt_buffer_length);
  bt_info.backtrace = bt_buffer;
  bt_info.bt_partial = FALSE;
  bt_info.bt_timestamp  = OEMOS_GetUpTime();

  if (bt_info.bt_length < 0)
  {
    bt_info.bt_partial = TRUE;
    bt_info.bt_length *= -1;
  }

  fileHandle = open_file_to_append(fname);
  if (fileHandle >= 0)
  {
    print_allocation_to_open_file(fileHandle,
                                  &bt_info,
                                  0, // no alloc data
                                  0, // no alloc size
                                  desc); // description
    close_file(fileHandle);
  }
}

void print_trace_buffer(const char *fname, 
                        const char *desc,
                        void **buffer,
                        unsigned long buffer_length,
                        boolean partial,
                        unsigned long timestamp_ms)
{
  int fileHandle;
  backtrace_info_t bt_info;

  bt_info.backtrace = buffer;
  bt_info.bt_length = buffer_length;
  bt_info.bt_partial = partial;
  bt_info.bt_timestamp  = timestamp_ms;

  fileHandle = open_file_to_append(fname);
  if (fileHandle >= 0)
  {
    print_allocation_to_open_file(fileHandle,
                                  &bt_info,
                                  0, // no alloc data
                                  0, // no alloc size
                                  desc); // description
    close_file(fileHandle);
  }
}

#endif//defined(FEATURE_BREW_HEAP_TRACKER)
      //defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT)
      //defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      //defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)

#if defined(FEATURE_BREW_HEAP_TRACKER)
boolean print_allocation(const char *fname, void *ptr)
{

    // The assumption is that ptr points to the memory returend by the heap
    // to the heap tracker, and not the memory returned by the heap tracker
    // to the application.

    if (ht_enabled)
    {
        HNode *pb;
        backtrace_info_t * bt_info;
        int fileHandle;

        if (!ptr) return FALSE;

        pb = PTR_TO_NODE(ptr);

        bt_info = NODE_TO_BT(pb);

      fileHandle = open_file_to_append(fname);
      if (fileHandle < 0)
      {
          return FALSE;
      }

      ui_kick_dog();

      print_allocation_to_open_file(fileHandle,
                                    bt_info,
                                    ptr,
                                    USER_SIZE(pb),
                                    NULL); // no description

      close_file(fileHandle);
    }

    return TRUE;
}

#endif//defined(FEATURE_BREW_HEAP_TRACKER)

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

void report_double_free_if_necessary(const char *modname, void *ptr)
{
  HNode *pb = PTR_TO_NODE(ptr);

  if (QuickValidateNode(gHeap3_x, pb) == TRUE) 
  {
    if (df_enabled && pb->bFree == DELAYED_FREE)
    {
      print_double_free_allocation(gHeap3_x, modname, ptr);
      MSG_ERROR("HeapFreeInternal free block %p: DOUBLE FREE (SAVED LOGS)!", ptr, 0, 0);
#ifdef FEATURE_ASSERT_ON_DOUBLE_FREE
      ASSERT(FALSE);
#endif//FEATURE_ASSERT_ON_DOUBLE_FREE
    }
  }
}

static boolean print_double_free_allocation(MemHeap *heap, const char *modname, void *ptr)
{
    int handle;
    static char name[100];
    static backtrace_info_t bt_info;
    extern signed long HT_ARM926EJS_UNWIND_STACK(void **, unsigned long);

    if (ptr == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    // We generate this stacktrace in the beginning of the function, because
    // doing it later causes HT_ARM926EJS_UNWIND_STACK to get confused.

    bt_info.bt_length = HT_ARM926EJS_UNWIND_STACK(bt_buffer, bt_buffer_length);
    bt_info.backtrace = bt_buffer;
    bt_info.bt_partial = FALSE;
    bt_info.bt_timestamp  = OEMOS_GetUpTime();

    if (bt_info.bt_length < 0)
    {
        bt_info.bt_partial = TRUE;
        bt_info.bt_length *= -1;
    }

    if (modname) 
    {
      SPRINTF(name, "/err/doublefree.%s.bt", modname);
    }
    else
    {
      SPRINTF(name, "/err/doublefree.%p.bt", ptr);
    }

    handle = open_file_to_append(name);
    if (handle >= 0)
    {
        HNode *pb = PTR_TO_NODE(ptr);
        HNode *pFreeTrace = NODE_TO_DF(pb)->pFreeTrace;

        if (pFreeTrace)
        {
#ifdef FEATURE_BREW_HEAP_TRACKER

            if (ht_enabled)
            {
               backtrace_info_t * bt_info = NODE_TO_BT(pb);

               // Write the allocation, if it hasn't been written already.  We know
               // that the allocation has not been written out to a file already
               // if pFreeTrace != NULL.

               print_allocation_to_open_file(handle,
                                             bt_info,
                                             ptr,
                                             USER_SIZE(pb),
                                             "ALLOCATION");
            }

#endif//FEATURE_BREW_HEAP_TRACLER

            // Write the first deallocation, if it hasn't been written already.
            // We know that the deallocation has not been witten out to a file
            // already if pFreeTrace != NULL.

            print_allocation_to_open_file(handle,
                                          ((backtrace_info_t *)NODE_TO_PTR(pFreeTrace)),
                                          ptr,
                                          USER_SIZE(pb),
                                          "FIRST FREE");

            // Now free the ancillary allocation that contained the backtrace
            // at the time this allocation was first freed.

            FreeAncillaryNode(heap, pFreeTrace);
            NODE_TO_DF(pb)->pFreeTrace = NULL;
        }

        // Now, append this free (which is redundant) to the file.

        ASSERT(handle);
        print_allocation_to_open_file(handle,
                                      &bt_info,
                                      ptr,
                                      USER_SIZE(pb),
                                      "REDUNDANT FREE");

        close_file(handle);
        return TRUE;
    }

    return FALSE;
}
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
static boolean print_overwritten_allocation(void *ptr)
{
    int handle;
    static char name[100];
    static backtrace_info_t bt_info;
    extern signed long HT_ARM926EJS_UNWIND_STACK(void **, unsigned long);

    if (ptr == NULL)
    {
        ASSERT(0);
        return FALSE;
    }

    // We generate this stacktrace in the beginning of the function, because
    // doing it later causes HT_ARM926EJS_UNWIND_STACK to get confused.

    bt_info.bt_length = HT_ARM926EJS_UNWIND_STACK(bt_buffer, bt_buffer_length);
    bt_info.backtrace = bt_buffer;
    bt_info.bt_partial = FALSE;
    bt_info.bt_timestamp  = OEMOS_GetUpTime();

    if (bt_info.bt_length < 0)
    {
        bt_info.bt_partial = TRUE;
        bt_info.bt_length *= -1;
    }

    SPRINTF(name, "/err/overrun.%p.bt", ptr);

    handle = open_file_to_append(name);
    if (handle >= 0)
    {
        HNode *pb = PTR_TO_NODE(ptr);

#ifdef FEATURE_BREW_HEAP_TRACKER

        if (ht_enabled)
        {
           backtrace_info_t * bt_info = NODE_TO_BT(pb);
          
           // Write the allocation, if it hasn't been written already.  We know
           // that the allocation has not been written out to a file already
           // if pFreeTrace != NULL.

           print_allocation_to_open_file(handle,
                                         bt_info,
                                         ptr,
                                         USER_SIZE(pb),
                                         "ALLOCATION");
        }

#endif//FEATURE_BREW_HEAP_TRACLER

        // Now, append the free to the file.

        print_allocation_to_open_file(handle,
                                      &bt_info,
                                      ptr,
                                      USER_SIZE(pb),
                                      "FREE OF CORRUPTED NODE");

        close_file(handle);
        return TRUE;
    }

    return FALSE;
}
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT

void *HeapReallocInternal(MemHeap *heap, void * pIn, uint32 size)
{
   register int    nTries;
   register HNode *pb;
   size_t          nOldSize = 0;
   uint32          dwActual;
   byte            nExtra;
   long            nSizeChange = 0;
   HNode          *pbNext;
   void           *pAnswer;
   uint32          sizeNeeded;
   byte            nLocks;
   boolean         bZero = TRUE;

   if(!heap->init)
   {
      DBGPRINTF("Fatal Heap Error. Heap not initialized");
      return(NULL);
   }

#ifdef HEAP_DEBUGGING
   if (pIn != NULL)
   {
      ValidateNode(heap, PTR_TO_NODE(pIn));
   }
#endif

   // Free the node

   if(!size) 
   {
      HeapFreeInternal(heap, pIn);
      return(NULL);
   }

   // Sweep the heap...

   if(size == 0xdeadbeef)
   {
      Sweep(heap);
      return(NULL);
   }

#ifdef HEAP_DEBUGGING

   // Validate all heap nodes

   if(size == 0xdeadbead)
   {
      ValidateAllNodes(FALSE);
      return(NULL);
   }
#endif

   // See if they want the memory uninitialized...

   if(size & ALLOC_NO_ZMEM)
   {
      size &= ~ALLOC_NO_ZMEM;
      bZero = FALSE;
   }

   if(size > heap->dwTotalBytes || size == 0)
   {
      return(NULL);
   }

   dwActual    = GET_REAL_SIZE(size);
#ifdef HEAP_DEBUGGING
   // reserve space for the overwrite block
   dwActual += MEMORY_OVERWRITE_SIZE;
#endif
   nExtra      = (byte)(dwActual - size - sizeof(HBusyNode));

   //
   // Important Note
   //
   // If the node is being tracked by a sentinel, we do not want to just
   // lock it as that would be bad if the realloc fails.  So, what we do
   // is save the sentinel and then use it before we copy/free the node below.
   //

   if(!pIn)
   {
      pb       = NULL;
      nLocks   = 1; // nodes start out locked
      goto ORNew;
   }

   pb = PTR_TO_NODE(pIn);

   // Node validation...

   if(pb < heap->pFirstNode || (char *) pIn >= (char *) heap->pFirstNode + heap->dwTotalBytes)
   {
      return(NULL);
   }

   // Pointing to free memory?

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
   if (df_enabled && pb->bFree == DELAYED_FREE)
   {
       print_double_free_allocation(heap, NULL, pIn);
       MSG_ERROR("HeapReallocInternal free block %p: DOUBLE FREE (SAVED LOGS)!", pIn, 0, 0);
#ifdef FEATURE_ASSERT_ON_DOUBLE_FREE
       ASSERT(FALSE);
#endif//FEATURE_ASSERT_ON_DOUBLE_FREE
       return (NULL);
   }
   else
#endif
   if(pb->bFree == TRUE)
   {
      MSG_ERROR("HeapReallocInternal free block %p: DOUBLE FREE!", pIn, 0, 0);
#ifdef FEATURE_ASSERT_ON_DOUBLE_FREE
      ASSERT(FALSE);
#endif//FEATURE_ASSERT_ON_DOUBLE_FREE
      return (NULL);
   }
// #ifdef HEAP_DEBUGGING
   else if (QuickValidateNode(heap, pb) == FALSE)
   {
      MSG_ERROR("HeapReallocInternal free block %p: FREEING GARBAGE!", pIn, 0, 0);
      ASSERT(FALSE);
      return (NULL);
   }
// #endif

   nOldSize = (size_t) NODE_SIZE(pb);

   if(nOldSize == size)
   {
      return(pIn);
   }

   nSizeChange = (long) size - (long) nOldSize;

   //
   // New size <= existing.  Shrink the block...
   //

   if(pb->dwSize >= dwActual)
   {
      BEGIN_CRITICAL_SECTION();

      // Clip the block into 2...

      if(pb->dwSize > dwActual + sizeof(HFreeNode))
      {
         SplitNode(heap, pb,dwActual);
      }
      else
      {
         // Fix - Compute the actual extra size of the node after the
         // adjustment.  If the new node filled some of the
         // old node's "extra" space, set it to 0...

         nExtra = (byte)(pb->dwSize - size - sizeof(HBusyNode));

         // zero out previously free extra bytes
         if(bZero && nExtra < pb->nExtra)
         {
            memset((byte *)pb + pb->dwSize - pb->nExtra, 0, (pb->nExtra - nExtra));
         }
      }

      pb->nExtra = nExtra;

#if defined(MARK_FREE_BYTES)
      if(nExtra)
      {
         memset((byte *)pb + pb->dwSize - nExtra, FREE_BYTE, nExtra);
      }
#endif

      END_CRITICAL_SECTION();

#ifdef HEAP_DEBUGGING
      MEMORY_OVERWRITE_BLOCK(pb) = MEMORY_OVERWRITE_GUARD;
#endif

#ifdef MALLOC_LOGGING
      mm_update_task_usage(nOldSize, size, pIn, pIn);
#endif

      return(pIn);
   }

   pAnswer     = NULL;
   sizeNeeded  = dwActual - pb->dwSize;
   pbNext      = NODE_NEXT(pb);

   BEGIN_CRITICAL_SECTION();

   if(pbNext > pb && pbNext->bFree == TRUE)
   {
      // Found enough space.

      if(pbNext->dwSize >= sizeNeeded)
      {
         // zero out previously free extra bytes
         if(bZero && pb->nExtra)
         {
            memset((byte *)pb + pb->dwSize - pb->nExtra, 0, pb->nExtra);
         }

         // If the block is split, adjust the size to use what we need
         // and init the next node.

         // Otherwise, consume the entire next node

         UnlinkFreeNode(heap, pbNext);

         if(pbNext->dwSize > sizeNeeded + sizeof(HBusyNode)) 
         {
            heap->dwUsedBytes += sizeNeeded;

            pb->dwSize += sizeNeeded;
            pb->bLast = FALSE;
            InitFreeNode(heap, NEXT_RIGHT_NODE(pb),pb,pbNext->dwSize - sizeNeeded,pbNext->bLast);
         }
         else
         {
            heap->dwUsedBytes += pbNext->dwSize;
            heap->dwFreeBlocks--;

            sizeNeeded = pbNext->dwSize;  // Set sizeNeeded for memset below.
            pb->dwSize += sizeNeeded;
            pb->bLast = pbNext->bLast;
            if(!pb->bLast)
            {
               NEXT_NODE(pb)->pPrev = pb;
            }
         }

         // Fix - Compute the actual extra size of the node after the
         // adjustment.  If the new node filled some of the
         // old node's "extra" space, set it to 0...

         nExtra = (byte)(pb->dwSize - size - sizeof(HBusyNode));

         // Initialize the new extra memory to 0

         if(bZero)
         {
            memset(pbNext,0,sizeNeeded);
         }

         pb->nExtra = nExtra;

#ifdef HEAP_DEBUGGING
         MEMORY_OVERWRITE_BLOCK(pb) = MEMORY_OVERWRITE_GUARD;
#endif

#if defined(MARK_FREE_BYTES)
         if(nExtra)
         {
            memset((byte *)pb + pb->dwSize - nExtra, FREE_BYTE, nExtra);
         }
#endif

         pAnswer = pIn;
      }
   }
   END_CRITICAL_SECTION();

   if(pAnswer)
   {
#ifdef HEAP_DEBUGGING
      ValidateNode(heap, PTR_TO_NODE(pAnswer));
#endif

#ifdef MALLOC_LOGGING
      mm_update_task_usage(nOldSize, size, pIn, pAnswer);
#endif
      return(pAnswer);
   }

   //
   // See above - copy the node sentinel and lock count...
   //

   nLocks   = pb->nLocks;
ORNew:

   nTries = 0;

   while(1)
   {
      BEGIN_CRITICAL_SECTION();

      // Do only what we must in the critical section...

      pb = FindFreeNode(heap, dwActual);

      if (pb)
      {
         UnlinkFreeNode(heap, pb);

         if(pb->dwSize > dwActual + sizeof(HFreeNode)) 
         {
            SplitNode(heap, pb,dwActual);
         } else
         {
            nExtra = (byte)(pb->dwSize - size - sizeof(HBusyNode));
         }

         pb->bFree = FALSE;
         heap->dwUsedBytes += pb->dwSize;
         heap->dwFreeBlocks--;
      }

      END_CRITICAL_SECTION();

      // Done...

      if(pb)
      {
         pAnswer        = NODE_TO_PTR(pb);
         pb->nExtra     = nExtra;
         pb->nLocks     = nLocks;
         ((HBusyNode *)pb)->ppHandle = NULL;     // Make sure the sentinel is clear...
#ifdef HEAP_DEBUGGING
         MEMORY_OVERWRITE_BLOCK(pb) = MEMORY_OVERWRITE_GUARD;
#endif


         // If we re-allocated then copy the old to the new and memset what is left.
         // Otherwise, just set it to 0.

         if(pIn)
         {
            memcpy(pAnswer, pIn, nOldSize);
            if(bZero && nSizeChange > 0)
            {
               memset((byte *)pAnswer + nOldSize,0,nSizeChange);
            }

            HeapNode_AdjSentinel(PTR_TO_NODE(pIn), ((char *)pb-(char*)PTR_TO_NODE(pIn)));

            HeapFreeInternal(heap, pIn);

         }
         else
         {
            if(bZero)
            {
               memset(pAnswer,0,size);
            }
         }

#if defined(MARK_FREE_BYTES)
         if(nExtra)
         {
            memset((byte *)pb + pb->dwSize - nExtra, FREE_BYTE, nExtra);
         }
#endif

#ifdef HEAP_DEBUGGING
         heap_malloc_count++;
#endif
#ifdef MALLOC_LOGGING
         mm_update_task_usage(0, size, NULL, pAnswer);
#endif
         return(pAnswer);
      }

      if(nTries > 1)
      {
         break;
      }
      Sweep(heap);
      nTries++;
   }

#ifdef HEAP_DEBUGGING_DEBUG_MEM_OUT
   if(size > 0)
   {
      HeapDump(heap, "MemOut", TRUE);
      ASSERT(FALSE);
   }
#endif
   return(NULL);
}

/*===========================================================================

Function:  AEEkHeap_CheckMemAvail()

Description:
      This function checks if a memory block of the given size
      can be allocated. This function does not
      do any actual allocation of memory. It just returns TRUE or FALSE
      indicating whether or not it is possible to allocate a block of the
      given size.

Prototype:
   boolean AEEkHeap_CheckMemAvail(AEEkHeap *me, uint32 dwSize)

Parameters:
   dwSize: Size of the block whose allocation needs to be verified.


Return Value:
   Returns TRUE if a block of the given size can be allocated
   Returns FALSE if failed or if a block of the given size cannot be allocated.

Comments:  None

Side Effects: This function may walk-through the heap (and collapse
adjacent free blocks if any)

==============================================================================*/
boolean AEEkHeap_CheckMemAvail(AEEkHeap *me, uint32 dwSize)
{
   boolean  bRet;
   MemHeap *heap = (MemHeap *)me;
   uint32   size = dwSize;

   BEGIN_CRITICAL_SECTION();

   if(size & ALLOC_NO_ZMEM)
   {
      size &= ~ALLOC_NO_ZMEM;
   }
   bRet = (boolean)(FindFreeNode(heap, GET_REAL_SIZE(size)) ? TRUE : FALSE);

   END_CRITICAL_SECTION();

   return(bRet);
}

/*==================================================================
Function: AEEkHeap_GetMemStats

Description:

 This function returns the number of free bytes in the heap.  It conditionally fills
 the values of the total heap size and largest block that can be allocated.

Prototype:
   uint32 AEEkHeap_GetMemStats(AEEkHeap *me, uint32 * pdwTotal, uint32 * pdwMax);

Parameter(s):

  pdwTotal: Pointer to value to set with total space in the heap.
  pdwMax:   Pointer to value to set with the maximum size block that can be allocated.

Return Value:  Number of free bytes in the heap
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
uint32 AEEkHeap_GetMemStats(AEEkHeap *me, uint32 * pdwTotal, uint32 * pdwMax)
{
   int bin;
   uint32 max = 0;
   uint32 free;
   register HNode *n = NULL;
   MemHeap *heap = (MemHeap *)me;

   if(pdwMax)
   {
      // Find the largest free block.  The largest free block will always be
      // in the highest numbered bin that is not empty, so start at bin
      // number (FREE_BIN_COUNT - 1) and work down to bin number zero
      for (bin = FREE_BIN_COUNT - 1; bin >= 0 && max == 0; bin--)
      {
         BEGIN_CRITICAL_SECTION();
         n = heap->pFirstFreeNode[bin];
         while (n != NULL)
         {
            if (max < n->dwSize)
            {
               max = n->dwSize;
            }
            n = ((HFreeNode *)n)->link.pNext;
         }
         END_CRITICAL_SECTION();
      }
      max -= sizeof(HBusyNode);
#ifdef HEAP_DEBUGGING
      max -= MEMORY_OVERWRITE_SIZE;
#endif

// in the event HEAPTRACKER is turned on
#if defined(FEATURE_BREW_HEAP_TRACKER) || defined (FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)
      if (heaptracker_nv_data.heaptracker_enabled)
      {
         uint32 overhead = sizeof(common_info_t);

#ifdef FEATURE_BREW_HEAP_TRACKER
         if (ht_enabled) 
         {
            // AEEkHeap_Realloc() uses HT_ARM926EJS_UNWIND_STACK()
	    // to unwind the stack. The return value is then added
	    // to overhead. If HT_ARM926EJS_UNWIND_STACK() is used here,
	    // overhead calculation will not match the overhead calculated
	    // in AEEkHeap_Realloc(). Thus, using the max is appropriate.
            overhead += sizeof(backtrace_info_t) + MAX_BT_BUFFER_LENGTH;
         }
#endif//FEATURE_BREW_HEAP_TRACKER

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
         if (df_enabled) 
         {
            overhead += sizeof(doublefree_info_t);
         }
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
         if (od_enabled)
         {
            overhead += heap_overwrite_padding;
         }
#endif
	 max -= overhead;
      }
#endif

      *pdwMax = max;
   }

   if(pdwTotal)
   {
      // Total bytes includes the heap structure overhead.
      *pdwTotal = heap->dwTotalBytes + ((unsigned long)heap->pFirstNode - (unsigned long)heap);
   }
   /* DEBUG Code
   MSG_ERROR("GetMemStats: Tot: %d, Max: %d, Free: %d", heap->dwTotalBytes, max,
      heap->dwTotalBytes - heap->dwUsedBytes - (heap->dwFreeBlocks * sizeof(HNode)));
   */
   // Get free bytes.  This is the usable heap size, minus the used bytes, minus the
   // node and debug overhead each free block incurs when becoming a used block.
   // This gives the true number of available bytes, rather than just
   // the number of bytes that are not in use.
   free = heap->dwTotalBytes - heap->dwUsedBytes - (heap->dwFreeBlocks * sizeof(HBusyNode));
#ifdef HEAP_DEBUGGING
   free -= heap->dwFreeBlocks * MEMORY_OVERWRITE_SIZE;
#endif
   return free;
}

/*===========================================================================


===========================================================================*/
static HNode *FindFreeNode(MemHeap *heap, uint32 inSizeNeeded)
{
   register HNode *pb;
   register HNode *pbret = NULL;
   unsigned long bin = FastFindBin(inSizeNeeded);

   // Grab the first free node.

   while (pbret == NULL && bin < FREE_BIN_COUNT)
   {
      pb = heap->pFirstFreeNode[bin];

      // Run the loop.
      while(pb)
      {
         if(pb->dwSize >= inSizeNeeded)
         {
            // Fragmentation Reduction
            if(pb->dwSize == inSizeNeeded)
            {
               // exact fit found
               return pb;
            }
            else if(pbret == NULL || pbret->dwSize > pb->dwSize)
            {
               // closest fit so far, update the return value
               pbret = pb;
            }
         }

         pb = ((HFreeNode *)pb)->link.pNext;
      }
      bin++;
   }
   return(pbret);
}

/*===========================================================================


===========================================================================*/
static HNode * SplitNode(MemHeap *heap, HNode * pb,uint32 dwActual)
{
   register HNode *pbNew;

#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
#endif

   if(dwActual < GET_REAL_SIZE(sizeof(uint32)))
   {
#ifdef HEAP_DEBUGGING
      ASSERT(FALSE);
#endif
   }

   if (IS_USED_NODE(pb))
   {
      // Block is used, adjust the used byte count
      heap->dwUsedBytes -= pb->dwSize;
      heap->dwUsedBytes += dwActual;
   }

   pbNew = (HNode *) ((char *) pb + dwActual);

   InitFreeNode(heap, pbNew, pb, pb->dwSize - dwActual, pb->bLast);

   // Additional free block added, update the free block count
   heap->dwFreeBlocks++;

   // Update the node header
   pb->dwSize = dwActual;
   pb->bLast = FALSE;


#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pbNew);
#endif

   return(pbNew);
}

/* ===========================================================================
FUNCTION UNLINKNODE
DESCRIPTION
   Remove from the given node list
   It is safe to call this function on a node that has already been removed
   from the given node list, as long as it is not on any other node list.
============================================================================== */
static void UnlinkNode(HFreeNode *pb, HNode **pList)
{
   if(pb->link.pPrev)
   {
      ((HFreeNode *)pb->link.pPrev)->link.pNext = pb->link.pNext;
   }
   else if(*pList == (HNode *)pb)
   {
      *pList = pb->link.pNext;
   }
   if(pb->link.pNext)
   {
      ((HFreeNode *)pb->link.pNext)->link.pPrev = pb->link.pPrev;
   }
   pb->link.pPrev = NULL;
   pb->link.pNext = NULL;
   return;
} /* END UnlinkNode */

/* ===========================================================================
FUNCTION LINKNODE
DESCRIPTION
============================================================================== */
static void LinkNode(HNode *pb, HNode **pList)
{
   ((HFreeNode *)pb)->link.pPrev = NULL;
   ((HFreeNode *)pb)->link.pNext = *pList;
   if(*pList)
   {
      ((HFreeNode *)(*pList))->link.pPrev = pb;
   }
   *pList = pb;
   return;
} /* END LinkNode */

/* ===========================================================================
FUNCTION UNLINKFREENODE
DESCRIPTION
   Remove from the used node list.
   It is safe to call this function on a node that has already been removed
   from the free node list, as long as it is not on the used node list.
============================================================================== */
void UnlinkFreeNode(MemHeap *heap, HNode *pb)
{
   UnlinkNode((HFreeNode *)pb, &heap->pFirstFreeNode[FastFindBin(pb->dwSize)]);
   return;
} /* END UnlinkFreeNode */

/* ===========================================================================
FUNCTION LINKFREENODE
DESCRIPTION
   Add to the free node list
============================================================================== */
void LinkFreeNode(MemHeap *heap, HNode *pb)
{
   LinkNode(pb, &heap->pFirstFreeNode[FastFindBin(pb->dwSize)]);
   return;
} /* END LinkFreeNode */

/* ===========================================================================
FUNCTION FREENODE
DESCRIPTION
   The function sets the current node to free.
============================================================================== */

static __inline HNode *DoFreeNode(MemHeap *heap, HNode *pb)
{
    heap->dwUsedBytes -= pb->dwSize;
    heap->dwFreeBlocks++;
    InitFreeNode(heap, pb, pb->pPrev, pb->dwSize, pb->bLast);
    return MergeFreeNode(heap, pb);
}

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
static void FreeAncillaryNode(MemHeap *heap, HNode *pb)
{
#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
   heap_malloc_count--;
#endif
   DoFreeNode(heap, pb);
}
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

static HNode *FreeNode(MemHeap *heap,
                       HNode *pb)
{
#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
   heap_malloc_count--;
#endif
#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
   if (df_enabled)
   {
       ASSERT(HEAP_FREE_DELAY > 2); // limit min to 3 to make managing the list simpler 

       pb->bFree      = DELAYED_FREE;
       ((HFreeNode *)pb)->link.pPrev = heap->pLastDelayFreeNode;
       ((HFreeNode *)pb)->link.pNext = NULL;

       // Make an allocation and attach it to the node we are freeing via delayed
       // free.  In this allocation, we save a stack-trace buffer that we generate
       // at the point the allocation is freed.  If the allocation is freed more
       // than once, we will (a) write the trace at the point the allocation was
       // made, if the heap-tracker is enabled; (b) write the trace at the point
       // the allocation was freed the first time; (c) write the trace at the point
       // the allocation was freed each subsequent time for each such occurrence.

#if 1
       {
           doublefree_info_t *df_info = NODE_TO_DF(pb);
           void *free_trace;
           signed long bt_depth;
           boolean bt_partial = FALSE;

           extern signed long HT_ARM926EJS_UNWIND_STACK(void **, unsigned long);

           bt_depth = HT_ARM926EJS_UNWIND_STACK(bt_buffer, bt_buffer_length);
           if (bt_depth < 0)
           {
               bt_partial = TRUE;
               bt_depth *= -1;
           }

           free_trace = HeapReallocInternal( heap, NULL,
                                             sizeof(backtrace_info_t) + bt_depth | ALLOC_NO_ZMEM );
           if ( NULL != free_trace )
           {
               df_info->pFreeTrace = PTR_TO_NODE( free_trace );

               ((backtrace_info_t *)free_trace)->backtrace = (void **)((backtrace_info_t *)free_trace + 1);
               ((backtrace_info_t *)free_trace)->bt_length = bt_depth;
               ((backtrace_info_t *)free_trace)->bt_partial = bt_partial;
               ((backtrace_info_t *)free_trace)->bt_timestamp  = OEMOS_GetUpTime();

               memcpy(((backtrace_info_t *)free_trace)->backtrace,
                      bt_buffer,
                      bt_depth);
           }
       }
#else
       {
         doublefree_info_t *df_info = NODE_TO_DF(pb);
         df_info->pFreeTrace = NULL;
       }
#endif

       if (heap->pLastDelayFreeNode)
       {
          ((HFreeNode *)heap->pLastDelayFreeNode)->link.pNext = pb;
       }
       heap->pLastDelayFreeNode = pb;
       if (heap->pFirstDelayFreeNode == NULL)
       {
          heap->pFirstDelayFreeNode = pb;
       }
       if (heap->pDelayFreeCount < heap_free_delay)
       {
          heap->pDelayFreeCount++;
       }
       else
       {
          HNode *node = heap->pFirstDelayFreeNode;
          doublefree_info_t *df_info = NODE_TO_DF(node);

          heap->pFirstDelayFreeNode = ((HFreeNode *)heap->pFirstDelayFreeNode)->link.pNext;
          ((HFreeNode *)heap->pFirstDelayFreeNode)->link.pPrev = NULL;
          node->bFree = FALSE;

          // Free the ancillary allocation that contains a stack trace for the first
          // time this memory was deallocated.  Of course, we cannot free that node
          // through HeapFreeInternal(), because that itself calls FreeNode(), which
          // would in turn cause infinite recursion.  HeapFeeInternal() just pefoms
          // some checks on the node that we know will succeed (since we allocate
          // the node internally), and then calls FreeNode(), which in turn does
          // the actual work of deallocating the node in DoFreeNode()... therefore,
          // here we can call DoFreeNode() directly.

          if (df_info->pFreeTrace) {
             FreeAncillaryNode(heap, df_info->pFreeTrace);
             df_info->pFreeTrace = NULL;
          }
          (void)DoFreeNode(heap, node);
       }
       return pb;
   }
   else
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
   return DoFreeNode(heap, pb);
}

/*===========================================================================

FUNCTION InitFreeNode

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
  Merges with the previous and/or next nodes if they are free and returns
  the pointer to the new node.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void InitFreeNode(MemHeap *heap, HNode *pb, HNode *prev, uint32 inSize, byte bLast)
{
#if defined(MARK_FREE_BYTES)
   memset(pb,0,sizeof(HFreeNode));
   memset((byte *)pb + sizeof(HFreeNode),FREE_BYTE,inSize - sizeof(HFreeNode));
#endif

   pb->bFree  = TRUE;
   pb->bLast  = bLast;
   pb->nLocks = 0;
   pb->nExtra = 0;
   pb->dwSize = inSize;
   pb->pPrev  = prev;
   if(!bLast)
   {
      NEXT_NODE(pb)->pPrev = pb;
   }

   LinkFreeNode(heap, pb);
}

/*===========================================================================

FUNCTION MergeFreeNode

DESCRIPTION
  Merges with the previous and/or next nodes if they are free and returns
  the pointer to the new node.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static HNode *MergeFreeNode(MemHeap *heap, HNode *pb)
{
   HNode *prev = pb->pPrev;
   HNode *next = NEXT_RIGHT_NODE(pb);

   // Combine with the prev and/or the next node if they are free
   if(prev != NULL && prev->bFree == TRUE)
   {
      heap->dwFreeBlocks--;

      UnlinkFreeNode(heap, pb);
      UnlinkFreeNode(heap, prev);
      if(next != NULL && next->bFree == TRUE)
      {
         heap->dwFreeBlocks--;
         UnlinkFreeNode(heap, next);
         InitFreeNode(heap, prev, prev->pPrev, prev->dwSize + pb->dwSize + next->dwSize, next->bLast);
         // zero out the removed node headers to help catch invalid nodes in QuickValidateNode
         memset(pb, 0, sizeof(HBusyNode));
         memset(next, 0, sizeof(HFreeNode));
      }
      else
      {
         InitFreeNode(heap, prev, prev->pPrev, prev->dwSize + pb->dwSize, pb->bLast);
         // zero out the removed node header to help catch invalid nodes in QuickValidateNode
         memset(pb, 0, sizeof(HBusyNode));
      }
      pb = prev;
   }
   else if(next != NULL && next->bFree == TRUE)
   {
      heap->dwFreeBlocks--;
      UnlinkFreeNode(heap, pb);
      UnlinkFreeNode(heap, next);
      InitFreeNode(heap, pb, prev, pb->dwSize + next->dwSize, next->bLast);
      // zero out the removed node header to help catch invalid nodes in QuickValidateNode
      memset(next, 0, sizeof(HFreeNode));
   }

#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
#endif
   return pb;
}


/*==================================================================

==================================================================*/
static void Sweep(MemHeap *heap)
{
   register HNode *pb, *pbUnlocked, *pbLastUnlocked;

   // Sweeping works by pushing all of the unlocked nodes we can to the right.
   //
   // This is accomplished by running a loop that finds the last unlocked
   // block that we have not already examined.  If found, we attempt to push
   // it right. If we run out of blocks, we stop.
   //

   pbLastUnlocked = NULL;

   BEGIN_CRITICAL_SECTION();

   while(1)
   {
      if (pbLastUnlocked == NULL)
      {
         pb = heap->pFirstNode;
         pbUnlocked = NULL;
         while(pb != pbLastUnlocked && !pb->bLast)
         {
            if(HeapNode_CanMove(pb))
               pbUnlocked = pb;
            pb = NEXT_NODE(pb);
         }
      }
      else
      {
         pb = pbLastUnlocked->pPrev;
         pbUnlocked = NULL;
         while(pb != NULL && pbUnlocked == NULL)
         {
            if (HeapNode_CanMove(pb))
               pbUnlocked = pb;
            pb = pb->pPrev;
         }
      }

      if(!pbUnlocked)
         break;

      pbLastUnlocked = PushUnlockedNode(heap, pbUnlocked);
   }

   END_CRITICAL_SECTION();
}


/*==================================================================

==================================================================*/
static HNode * PushUnlockedNode(MemHeap *heap, HNode * pb)
{
   HNode *  pbNext;
   HNode *  pbPrev;
   HNode *  pbNextNext = NULL;
   uint32   dwSize;
   byte     bLast;
   HNode *  pbIn = pb;

   // Cannot move "locked" memory...
   if(pb->nLocks > 0)
   {
      return(pb);
   }

#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
#endif

   if (!QuickValidateNode(heap, pb))
   {
      MSG_ERROR("PushUnlockedNode invalid node ptr %p", pb, 0, 0);
#ifdef HEAP_DEBUGGING
      ASSERT(FALSE);
#endif
      return(NULL);
   }

   BEGIN_CRITICAL_SECTION();
   // We cannot push to the right if we are at the end...
   if(!pb->bLast)
   {
      // Move the node over the free node to the right...
      pbNext = NEXT_NODE(pb);
      if(pbNext->bFree == TRUE)
      {
         if (!pbNext->bLast)
         {
            pbNextNext = NEXT_NODE(pbNext);
         }
         UnlinkFreeNode(heap, pbNext);

         // Move the node to the right and reset the free node...
         dwSize   = pbNext->dwSize;
         bLast    = pbNext->bLast;
         pbNext   = (HNode *)((byte *)pb + dwSize);
         pbPrev   = pb->pPrev;
         memmove((void *)pbNext, (void *)pb, pb->dwSize);
         pbNext->pPrev = pb;
         InitFreeNode(heap, pb,pbPrev,dwSize,FALSE);
         MergeFreeNode(heap, pb);
         pb = pbNext;
         pb->bLast = bLast;
#ifdef HEAP_DEBUGGING
         ValidateNode(heap, pb);
#endif
         if (pbNextNext)
         {
            pbNextNext->pPrev = pb;
         }
      }
   }

   if(((HBusyNode *)pb)->ppHandle && pbIn != pb)
   {
      // pb has moved, adjust the sentinel accordingly
      HeapNode_AdjSentinel(pb, ((char *)pb-(char*)pbIn));
   }
   END_CRITICAL_SECTION();

#ifdef HEAP_DEBUGGING
   ValidateNode(heap, pb);
#endif

#ifdef MALLOC_LOGGING
   if (pbIn != pb)
   {
      mm_update_task_usage(NODE_SIZE(pb), NODE_SIZE(pb), NODE_TO_PTR(pbIn), NODE_TO_PTR(pb));
   }
#endif
   return(pb);
}

static boolean QuickValidateNode(MemHeap *heap, HNode *node)
{
   boolean result = FALSE;
   register HNode *next = NEXT_RIGHT_NODE(node);
   register HNode *prev = node->pPrev;
   // Always on node validation to protect against heap corruption
   // a node is valid if all of the following are true:
   // 1) The node is >= first node and the node is < first node + heap size
   // 2) The node + node size is <= first node + heap size
   // 3) The node size is greater than zero:
   //        node->dwSize > 0
   // 4) node does not equal the previous node, or the next node
   //        node != prev && node != next
   // 5) This is the last node, or next is not NULL and next's prev pointer points to this node
   //        node->bLast == TRUE || (next != NULL && next->pPrev == node)
   // 6) This is the first node, or prev is not NULL and the prev + prev->dwSize equals this node.
   //        node == heap->pFirstNode || (prev != NULL && NEXT_RIGHT_NODE(prev) == node)
   // For this to work correctly, when a node is merged with it's previous node,
   // the node block must be zeroed out.  This is done in MergeFreeNode.
   if ((node >= heap->pFirstNode) && (node < (HNode *)(((char *)heap->pFirstNode) + heap->dwTotalBytes)) &&
       (((unsigned long)node) + node->dwSize <= ((unsigned long)heap->pFirstNode) + heap->dwTotalBytes) &&
       (node->dwSize > 0) && (node != prev && node != next) &&
       (node->bLast == TRUE || (next != NULL && next->pPrev == node)) &&
       (node == heap->pFirstNode || (prev != NULL && NEXT_RIGHT_NODE(prev) == node)))
   {
      result = TRUE;
   }
   return result;
}

#ifdef HEAP_DEBUGGING
static void ValidateNode(MemHeap *heap, HNode *node)
{
   register HNode   *pb, *nodeNext,*nodePrev;
#ifdef HEAP_DEBUGGING_FIND_NODE_IN_HEAP
   boolean           nodeFound      = FALSE;
   boolean           nodeNextFound  = FALSE;
   boolean           nodePrevFound  = FALSE;
#endif
   boolean           nodeValid      = FALSE;
   boolean           nodeOrderValid = TRUE;
   boolean           overwrite      = FALSE;
   byte *            pEnd;

   BEGIN_CRITICAL_SECTION();
   pb       = heap->pFirstNode;
   nodeNext = NEXT_RIGHT_NODE(node);
   nodePrev = node->pPrev;
   pEnd     = (byte *)pb + heap->dwTotalBytes;

   if ((node >= heap->pFirstNode && (byte *)node < pEnd) &&
       (nodeNext == NULL || (nodeNext >= heap->pFirstNode && (byte *)nodeNext < pEnd)) &&
       (nodePrev == NULL || (nodePrev >= heap->pFirstNode && (byte *)nodePrev < pEnd)))
   {
      nodeValid = TRUE;
   }

   if ((nodeNext != NULL && node >= nodeNext) || node <= nodePrev)
   {
      nodeOrderValid = FALSE;
   }

   // Free blocks don't have an overwrite guard
   overwrite = IS_USED_NODE(node) && MEMORY_OVERWRITE_BLOCK(node) != MEMORY_OVERWRITE_GUARD;

#ifdef HEAP_DEBUGGING_FIND_NODE_IN_HEAP
   while((byte *)pb < pEnd && (!nodeFound || !nodeNextFound || !nodePrevFound))
   {
      if (node == pb)
      {
         nodeFound = TRUE;
      }
      if (nodeNext == NULL || nodeNext == pb)
      {
         nodeNextFound = TRUE;
      }
      if (nodePrev == NULL || nodePrev == pb)
      {
         nodePrevFound = TRUE;
      }
      if(pb->bLast)
      {
         break;
      }
      pb = NODE_NEXT(pb);
   }
#endif
   END_CRITICAL_SECTION();

   if (!nodeValid)
   {
      HeapDump(heap, "BadNode", TRUE);
      ASSERT(FALSE);
   }
#ifdef HEAP_DEBUGGING_FIND_NODE_IN_HEAP
   else if (!nodeFound || !nodeNextFound || !nodePrevFound)
   {
      HeapDump(heap, "NodeNotInHeap", TRUE);
      ASSERT(FALSE);
   }
#endif
   else if (!nodeOrderValid)
   {
      HeapDump(heap, "BadNodeOrder", TRUE);
      ASSERT(FALSE);
   }
   else if (overwrite)
   {
      HeapDump(heap, "Overwrite", TRUE);
      ASSERT(FALSE);
   }
   return;
}
#endif


/* ===========================================================================
FUNCTION ValidateAllNodes
DESCRIPTION
   This function walks the heap and validates each node.
============================================================================== */

#ifdef HEAP_DEBUGGING
static void ValidateAllNodes(boolean bUseQuickValidate)
{
   register HNode *pb;
   byte           *pEnd;

   if((! gHeap3_x) || (! gHeap3_x->init))
  {
      DBGPRINTF("ValidateAllNodes: Heap not initialized");
      return;
   }

   pb     = gHeap3_x->pFirstNode;
   pEnd   = (byte *)pb + gHeap3_x->dwTotalBytes;

   while(pb && pb >= gHeap3_x->pFirstNode && (byte *)pb < pEnd)
   {
      if (bUseQuickValidate)
      {
         if (! QuickValidateNode(gHeap3_x, pb))
         {
            ASSERT(FALSE);
         }
      }
      else
      {
         ValidateNode(gHeap3_x, pb);
      }

      if(pb->bLast)
      {
         break;
      }
      pb = NEXT_RIGHT_NODE(pb);
   }
}
#endif


/* ===========================================================================
FUNCTION FASTFINDBIN
DESCRIPTION
   This function is a effectivly an opened up binary search algorith hard
   coded for 32 values, with the exception that it looks for the
   closest match without going over instead of the exact match.  This function
   is large but extremely fast, speed is much more important here than size.
============================================================================== */
unsigned long FastFindBin(unsigned long value)
{
   unsigned long bin;
   if (value <= BINSIZE15)
   {
      if (value <= BINSIZE7)
      {
         if (value <= BINSIZE3)
         {
            if (value <= BINSIZE1)
            {
               if (value <= BINSIZE0)
               {
                  bin = 0;
               }
               else
               {
                  bin = 1;
               }
            }
            else // > BINSIZE1
            {
               if (value <= BINSIZE2)
               {
                  bin = 2;
               }
               else
               {
                  bin = 3;
               }
            }
         }
         else // > BINSIZE3
         {
            if (value <= BINSIZE5)
            {
               if (value <= BINSIZE4)
               {
                  bin = 4;
               }
               else
               {
                  bin = 5;
               }
            }
            else // > BINSIZE5
            {
               if (value <= BINSIZE6)
               {
                  bin = 6;
               }
               else
               {
                  bin = 7;
               }
            }
         }
      }
      else // value > BINSIZE7
      {
         if (value <= BINSIZE11)
         {
            if (value <= BINSIZE9)
            {
               if (value <= BINSIZE8)
               {
                  bin = 8;
               }
               else
               {
                  bin = 9;
               }
            }
            else // > BINSIZE9
            {
               if (value <= BINSIZE10)
               {
                  bin = 10;
               }
               else
               {
                  bin = 11;
               }
            }
         }
         else // > BINSIZE11
         {
            if (value <= BINSIZE13)
            {
               if (value <= BINSIZE12)
               {
                  bin = 12;
               }
               else
               {
                  bin = 13;
               }
            }
            else // > BINSIZE13
            {
               if (value <= BINSIZE14)
               {
                  bin = 14;
               }
               else
               {
                  bin = 15;
               }
            }
         }
      }
   }
   else // > BINSIZE15
   {
      if (value <= BINSIZE23)
      {
         if (value <= BINSIZE19)
         {
            if (value <= BINSIZE17)
            {
               if (value <= BINSIZE16)
               {
                  bin = 16;
               }
               else
               {
                  bin = 17;
               }
            }
            else  // > BINSIZE17
            {
               if (value <= BINSIZE18)
               {
                  bin = 18;
               }
               else
               {
                  bin = 19;
               }
            }
         }
         else // > BINSIZE19
         {
            if (value <= BINSIZE21)
            {
               if (value <= BINSIZE20)
               {
                  bin = 20;
               }
               else
               {
                  bin = 21;
               }
            }
            else // > BINSIZE21
            {
               if (value <= BINSIZE22)
               {
                  bin = 22;
               }
               else
               {
                  bin = 23;
               }
            }
         }
      }
      else // > BINSIZE23
      {
         if (value <= BINSIZE27)
         {
            if (value <= BINSIZE25)
            {
               if (value <= BINSIZE24)
               {
                  bin = 24;
               }
               else
               {
                  bin = 25;
               }
            }
            else // > BINSIZE25
            {
               if (value <= BINSIZE26)
               {
                  bin = 26;
               }
               else
               {
                  bin = 27;
               }
            }
         }
         else // > BINSIZE27
         {
            if (value <= BINSIZE29)
            {
               if (value <= BINSIZE28)
               {
                  bin = 28;
               }
               else
               {
                  bin = 29;
               }
            }
            else // > BINSIZE29
            {
               if (value <= BINSIZE30)
               {
                  bin = 30;
               }
               else
               {
                  bin = 31;
               }
            }
         }
      }
   }
   return bin;
}

#endif

// BREW 3.x heap interface wrapper

/* ===========================================================================
FUNCTION TRIVIAL_SBRK
DESCRIPTION
============================================================================== */
static void *trivial_sbrk(void *pSbrkCx, int32 nIncrement)
{
   (void)nIncrement;
   return pSbrkCx;
}


/* ===========================================================================
FUNCTION AEEKHEAP_INIT
DESCRIPTION
============================================================================== */
int AEEkHeap_Init(AEEkHeap *pHeap, void *(*pfnSbrk)(void *pSbrkCx, int32 nIncrement), void *pSbrkCx)
{
   MemHeap *heap = (MemHeap *)pHeap;
   char  *pStart = (char *)pHeap + sizeof(MemHeap);
   char  *pEnd;
   int32  nSize;
   uint32   dwChunks;
   int i;

   if ((void *(*)(void *, int32))0 == pfnSbrk)
   {
      pfnSbrk = trivial_sbrk;
   }
   pEnd = pfnSbrk(pSbrkCx,0);
   nSize = pEnd - pStart;

   if (gHeap3_x == NULL)
   {
      gHeap3_x = heap;
   }

   memset(heap,0,sizeof(MemHeap));
   heap->init = FALSE;

   if(!pStart || nSize < sizeof(HBusyNode) * 2) 
   {
      return(EFAILED);
   }

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT

  // If overwrite detection is compiled in, always enabled by default.  If the
  // feature is disabled through the NV item, the flag will be set to false
  // eventually.  Enabling the feature by default avoids the case where some
  // calls to AEEkHeap_Realloc() to allocate memory initially test the flag, 
  // which would be FALSE, do nothing, then we enable the flag, and the memory
  // is freed, in which case we will think mistakenly that the memory allocation
  // has been corrupted.  If we go the other way round--enable the flag by 
  // default, we avoid this scenario, at the cost of the few very first 
  // allocation being a bit larger.

  heaptracker_nv_data.heaptracker_enabled = 8;
#endif

   // Advance to the nearest paragraph boundary.
   // This while loop should work regardless of how
   // many bits are required for address pointers
   // near or far, etc.

   // Turn off lint "size incompatibility" warning because cast
   // from pointer to uint32 will lose bits, but we don't
   // care because we're only interested in the least significant
   // bits and we never cast back into a pointer, so the warning
   // can be safely ignored

   while((((uint32) pStart) & 0x000FUL))
   {
      ++pStart;
   }

   dwChunks = (uint32) ((pEnd - pStart) / sizeof(uint32));

   nSize = dwChunks * sizeof(uint32);

   heap->pFirstNode     = (HNode *)pStart;

   for (i = 0; i < FREE_BIN_COUNT; i++)
   {
      heap->pFirstFreeNode[i] = NULL;
   }

   InitFreeNode(heap, heap->pFirstNode,NULL,nSize,TRUE);

   heap->dwTotalBytes   = nSize;
   heap->dwUsedBytes    = 0;
   heap->dwFreeBlocks   = 1;

#if defined(FEATURE_HEAP_RESERVE_16MB_BOUNDARY_BLOCKS) || defined(FEATURE_HEAP_RESERVE_32MB_BOUNDARY_BLOCKS)
#if defined(FEATURE_HEAP_RESERVE_16MB_BOUNDARY_BLOCKS) && defined(FEATURE_HEAP_RESERVE_32MB_BOUNDARY_BLOCKS)
#error Cannot define both FEATURE_HEAP_RESERVE_16MB_BOUNDARY_BLOCKS and FEATURE_HEAP_RESERVE_32MB_BOUNDARY_BLOCKS
#endif

   // reserve the first block on every 16/32MB boundary to prevent an
   // issue with the DSP crossing 16MB boundaries
   {
#if defined(FEATURE_HEAP_RESERVE_16MB_BOUNDARY_BLOCKS)
      unsigned long boundary_size = 0x01000000;
      unsigned long boundary_mask = 0xff000000;
#elif defined(FEATURE_HEAP_RESERVE_32MB_BOUNDARY_BLOCKS)
      unsigned long boundary_size = 0x02000000;
      unsigned long boundary_mask = 0xfe000000;
#endif
      long boundary_blocks = 0;
      unsigned long boundary;
      HNode * startBlockNode = heap->pFirstNode;
      HNode * pb;
      unsigned long blockSize = sizeof(HBusyNode);
#ifdef HEAP_DEBUGGING
            // When debugging, but maintain the memory overwrite flag since this is
            // considered a used block
            blockSize += MEMORY_OVERWRITE_SIZE;
#endif
      for(boundary = (((unsigned long)heap->pFirstNode) & boundary_mask) + boundary_size; 
          boundary < ((unsigned long)heap->pFirstNode) + heap->dwTotalBytes; boundary += boundary_size)
      {
         if (boundary + sizeof(HBusyNode) <= ((unsigned long)heap->pFirstNode) + heap->dwTotalBytes)
         {
            // split the start block node at the 16/32MB boundary 
            // This will return the block directly following the boundary
            pb = SplitNode(heap, startBlockNode, boundary - (unsigned long)startBlockNode);

            // Mark the boundary node as used
            UnlinkFreeNode(heap, pb);
            pb->bFree = FALSE;
            pb->nLocks = 1;
            heap->dwFreeBlocks--;
            heap->dwUsedBytes += pb->dwSize;

#ifdef HEAP_DEBUGGING
            // When debugging, maintain the memory overwrite flag since this is
            // considered a used block
            MEMORY_OVERWRITE_BLOCK(pb) = MEMORY_OVERWRITE_GUARD;
#endif

            // split the boundary node at the size of an HNode
            // This will return the new start of this 16MB block
            startBlockNode = SplitNode(heap, pb, blockSize);
            boundary_blocks++;
         }
         else
         {
            // not enough space left over to create a used node,
            // just clip the heap size.
            heap->dwTotalBytes = boundary - ((unsigned long)heap->pFirstNode);
         }
      }
   }
#endif

   INITIALIZE_CRITICAL_SECTION();

   heap->init = TRUE;

   return SUCCESS;
}

/* ===========================================================================
FUNCTION AEEKHEAP_REALLOC
DESCRIPTION
============================================================================== */
void *AEEkHeap_Realloc ( AEEkHeap *pHeap, void *p, uint32 uSize )
{
   void *result = NULL;

   if ( uSize == 0 )
   {
      // Testing heaptracker_nv_data.heaptracker_enabled is equivalent to
      // testing the expression (HT_ENABLED || DF_ENABLED || OD_ENABLED || 
      // ZF_ENABLED), but is more efficient when all three features are 
      // disabled, because it is a single test.  If all debugging features
      // are disabled, we incur a performance hit of a single variable check.

#if defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT) || defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
      if (heaptracker_nv_data.heaptracker_enabled)
      {
         HNode *pb = PTR_TO_NODE(p);
#if defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)
         if (od_enabled)
         {
            if (p)
            {
               if (FALSE == check_guard((byte*)(p) + USER_SIZE(pb)))
               {
                  // Memory was overwritten.   If the heap tracker is enabled, 
                  // print out the stack trace at the point of the allocation,
                  // then print out the stack trace right now, and then fail an
                  // assertion.
                  
                  print_overwritten_allocation(p);
                  ASSERT(!"Freeing corrupted node!");
               }
            }
         }
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT
#if defined(FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)
         if (zf_enabled) 
         {
            if (p) 
            {
               // Since the OEM heap is used by tasks other than UI, it is possible
               // that these tasks abuse the heap in a way that is not easy to 
               // resolve (this has, in fact, happened with the VS task so far).  
               // For this reason we restrict the initialize-on-free feature to the
               // UI task only.
               
               if (zap_mem_for_all_tasks || is_ui_task()) 
               {
                  memset(p,
                         INIT_MEM_ON_FREE__DEFAULT_VALUE,
                         USER_SIZE(pb));
               }
            }
         }
#endif//FEATURE_BREW_HEAP_INIT_MEM_ON_FREE
      }
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT || FEATURE_BREW_HEAP_INIT_MEM_ON_FREE)

      HeapFreeInternal( (MemHeap *) pHeap, p );
   }
   else
   {
#if defined(FEATURE_BREW_HEAP_TRACKER) || defined (FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT) 
      HNode *pb;

      // Testing heaptracker_nv_data.heaptracker_enabled is equivalent to
      // testing the expression (HT_ENABLED || DF_ENABLED) || OD_ENABLED), but 
      // is more efficient when all three features are disabled, because it is
      // a single test.

      if (heaptracker_nv_data.heaptracker_enabled)
      {
         uint32 newSize  = uSize & ~ALLOC_NO_ZMEM;
         uint32 origSize = p ? NODE_TO_COMMON_INFO(PTR_TO_NODE(p))->user_allocation_size : 0;
         uint32 overhead = sizeof(common_info_t);
	 // additional variables to be used to detect wrapped-around
	 // values in the event uSize is a large number, specifically
	 // 0xFFFFFFFX or 0x7FFFFFFX. The MSB is used to detect whether this
	 // event has happened. If the MSB is flipped after adding the
	 // overhead, we can safely return NULL without allocating
	 // any memory for this request.
	 uint32 no_zmem_flag_pre = uSize & ALLOC_NO_ZMEM;
         uint32 no_zmem_flag_post = 0;
#ifdef FEATURE_BREW_HEAP_TRACKER
         signed long bt_depth = 0;
         boolean bt_partial = FALSE;

         if (ht_enabled) 
         {
            bt_depth = HT_ARM926EJS_UNWIND_STACK(bt_buffer, bt_buffer_length);
            if (bt_depth < 0)
            {
              bt_partial = TRUE;
              bt_depth *= -1;
            }

            overhead += sizeof(backtrace_info_t) + bt_depth;
         }
#endif//FEATURE_BREW_HEAP_TRACKER

#ifdef FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT
         if (df_enabled) 
         {
            overhead += sizeof(doublefree_info_t);
         }
#endif//FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
         if (od_enabled)
         {
            overhead += heap_overwrite_padding;
         }
#endif

	 // uSize + overhead can cause a wrap around if uSize is
	 // a very large number i.e. uSize=0xFFFFFFFF. Thus, it's
	 // a good idea to check this prior to calling HeapReallocInternal()
	 // by looking at the MSB.
         no_zmem_flag_post = (uint32)((uSize + overhead) & ALLOC_NO_ZMEM);
	 if (no_zmem_flag_post != no_zmem_flag_pre)
	 {
            result = NULL;
	 }
	 else
	 {
            result = HeapReallocInternal( (MemHeap *) pHeap, p,
                                          uSize +
                                          overhead );
	 }

         if ( NULL != result )
         {
            pb = PTR_TO_NODE( result );

            NODE_TO_COMMON_INFO(pb)->user_allocation_size = newSize;

#ifdef FEATURE_BREW_HEAP_OVERWRITE_DETECT
            if (od_enabled)
            {
               set_guard(((byte *)result) + newSize);
            }
#endif//FEATURE_BREW_HEAP_OVERWRITE_DETECT

#if defined(FEATURE_BREW_HEAP_TRACKER)
            if (ht_enabled) 
            {
               backtrace_info_t * bt_info = NULL;
               bt_info = NODE_TO_BT(pb);
               INIT_ALLOC_BT(bt_info, bt_buffer, bt_depth, bt_partial);
            }
#endif//FEATURE_BREW_HEAP_TRACKER

            /* We cannot rely on HeapReallocInternal() to zero-initialize the
               extended memory area, because HeapReallocInternal() considers the
               number of bytes for the backtrace as a part of the users data,
               and will move those bytes along with everything else in the
               reallocated memory.  That is why, if the user has requested the
               memory to be zero-initialized (as is the default), we will
               calculate the difference between the orignal and new user's data,
               and if the request is for more memory, we will zero-initialize
               the difference.  The new backtrace is guaranteed to follow the
               newly zero-initialized data.

               The test below is equivalent to if((signed)uSize > 0), but more
               readable and more maintainable, if the bit position of
               ALLOC_NO_ZMEM changes.
            */

            if(p && ((uSize & ALLOC_NO_ZMEM) == 0))
            {
               if (origSize < newSize)
               {
                  memset(((char *)result) + origSize, 0, newSize - origSize);
               }
            }
         } // if ( NULL != pb )
      } // if (heaptracker_nv_data.heaptracker_enabled)
      else
#endif//#if defined(FEATURE_BREW_HEAP_TRACKER) || 
      //    defined(FEATURE_BREW_HEAP_DOUBLE_FREE_DETECT) || 
      //    defined(FEATURE_BREW_HEAP_OVERWRITE_DETECT)
      {
         result = HeapReallocInternal( (MemHeap *) pHeap, p, uSize );
      }

#if defined(FEATURE_EXCEPTION_AT_MALLOC_FAIL) || \
    defined(FEATURE_BREW_HEAP_TRACKER)

      if ( NULL == result )
      {
      #if defined(FEATURE_EXCEPTION_AT_MALLOC_FAIL)
        void *ptrSysHeap = AEE_GetSysHeap() ;

        if(ptrSysHeap)
        {
          if(ptrSysHeap != (void*)pHeap)
          {
            if(AEE_FreeMemory(uSize) == FALSE)
            {
              {
                struct fs_stat fst;
                // We check if a specified file exists here or not - if it exists,
                // Phone won't err_fatal but just continue
                // In other case, we will error fatal.
                // This is required to pass OAT UMTS - which requires phone not
                // to crash when a large sized memory is tried to be allocated.
                if (efs_stat("/brew/shared/mem_fatal_bypass.txt", &fst) < 0 )
                {
                  ERR_FATAL("Malloc failed to allocate memory %d", uSize, 0, 0);
                }
              }
            } // AEE_FreeMemory
          } // if(ptrSysHeap != (void*)pHeap)
        } // ptrSysHeap
      #endif // defined(FEATURE_EXCEPTION_AT_MALLOC_FAIL)

      #if defined(FEATURE_BREW_HEAP_TRACKER)
        ht_num_failed_allocations++;
      #endif // defined(FEATURE_BREW_HEAP_TRACKER)
      }

#endif// defined(FEATURE_EXCEPTION_AT_MALLOC_FAIL) ||
      // defined(FEATURE_BREW_HEAP_TRACKER)
   }

   return result;
} /* END AEEkHeap_Realloc */

/* ===========================================================================
FUNCTION AEEKHEAP_LOCK
DESCRIPTION
============================================================================== */
unsigned long AEEkHeap_Lock(AEEkHeap *pHeap, void *p)
{
   MemHeap   *heap = (MemHeap *)pHeap;
   HNode *pb = PTR_TO_NODE(p);

   if ((void *)0 == p)
   {
      return 0;
   }

   if(QuickValidateNode(heap, pb))
   {
      return ( (byte)0xFF == pb->nLocks ? 0 : pb->nLocks++ );
   }
   else
   {
#ifdef HEAP_DEBUGGING
      ASSERT(FALSE);
#endif
   }
   return 0;
} /* END AEEkHeap_Lock */

/* ===========================================================================
FUNCTION AEEKHEAP_UNLOCK
DESCRIPTION
============================================================================== */
unsigned long AEEkHeap_Unlock(AEEkHeap *pHeap, void *p)
{
   unsigned long lockCount = 0;
   MemHeap   *heap = (MemHeap *)pHeap;
   HNode *pb = PTR_TO_NODE(p);

   if ((void *)0 == p)
   {
      return 0;
   }

   if(QuickValidateNode(heap, pb))
   {
      //
      // Memory is moved to the end of the heap when it's unlocked.
      // This differs from other implementations that allocate from
      // the right for such memory.  However, doing it this way
      // allows for all of the standard memory allocation routines
      // and still provides the benefit.
      //
      // NOTE: This only works when we are allocating from the last
      // contiguous free block...
      //

      lockCount = pb->nLocks;
      if (pb->nLocks)
      {
         if (--pb->nLocks)
         {
            return lockCount;
         }
      }

      PushUnlockedNode(heap, pb);
      return lockCount;
   }
   else
   {
      MSG_ERROR("AEEkHeap_Unlock free block %p", p, 0, 0);
#ifdef HEAP_DEBUGGING
      ASSERT(FALSE);
#endif
   }
   return 0;
} /* END AEEkHeap_Unlock */

/* ===========================================================================
FUNCTION AEEKHEAP_SENTINEL
DESCRIPTION
============================================================================== */
void **AEEkHeap_Sentinel(AEEkHeap *pHeap, void *p, void **ppHandle)
{
   HNode    *phn;
   void    **ppOld;
   MemHeap  *heap = (MemHeap *)pHeap;

   if ((void *)0 == p)
   {
      return 0;
   }

   phn = PTR_TO_NODE(p);

   BEGIN_CRITICAL_SECTION();
   ppOld = ((HBusyNode *)phn)->ppHandle;

   ((HBusyNode *)phn)->ppHandle = ppHandle;

   PushUnlockedNode(heap, phn);
   END_CRITICAL_SECTION();

   return ppOld;
} /* END AEEkHeap_Sentinel */

/* ===========================================================================
FUNCTION AEEKHEAP_NODEINFO
DESCRIPTION
============================================================================== */
int AEEkHeap_NodeInfo(AEEkHeap *pHeap, AEEkHeapNodeInfo *pNodeInfoBuf,
                             unsigned uNodeInfoBufSize)
{
   HNode   *phn;
   MemHeap *heap = (MemHeap *)pHeap;

   if (uNodeInfoBufSize < sizeof(AEEkHeapNodeInfo)) {
      return EBADPARM;
   }

   phn = PTR_TO_NODE(pNodeInfoBuf->p);

   if (!QuickValidateNode(heap, phn))
   {
      return EMEMPTR;
   }

   pNodeInfoBuf->ppSentinel = ((HBusyNode *)phn)->ppHandle;
   pNodeInfoBuf->uFlags     = (IS_FREE_NODE(phn) ? KHNF_FREE : 0);
   pNodeInfoBuf->uLockCount = phn->nLocks;
   pNodeInfoBuf->uOverhead  = phn->nExtra;
   pNodeInfoBuf->uSize      = NODE_SIZE(phn);

   return SUCCESS;
} /* END AEEkHeap_NodeInfo */

/* ===========================================================================
FUNCTION AEEKHEAP_DEFAULTNODEINFO
DESCRIPTION
============================================================================== */
int AEEkHeap_DefaultHeapNodeInfo(AEEkHeapNodeInfo *pNodeInfoBuf,
                                 unsigned uNodeInfoBufSize)
{
   if (gHeap3_x != NULL)
   {
       return AEEkHeap_NodeInfo((AEEkHeap *)gHeap3_x, 
                                pNodeInfoBuf, 
                                uNodeInfoBufSize);
   }
   return EFAILED;
} /* END AEEkHeap_DefaultNodeInfo */

/* ===========================================================================
FUNCTION AEEKHEAP_COMPRESS
DESCRIPTION
============================================================================== */
void AEEkHeap_Compress(AEEkHeap *pHeap)
{
   Sweep((MemHeap *)pHeap);
   return;
} /* END AEEkHeap_Compress */

/* ===========================================================================
FUNCTION AEEKHEAP_WALK
DESCRIPTION
============================================================================== */
int AEEkHeap_Walk(AEEkHeap *pHeap,
                  AEEkHeapNodeInfo *pWalkBuf,
                  unsigned uWalkBufSize)
{
   HNode *phn;
   MemHeap *heap = (MemHeap *)pHeap;

   if (uWalkBufSize < sizeof(AEEkHeapNodeInfo)) {
      return EBADPARM;
   }

   if ((void *)0 == pWalkBuf->p)
   {
      phn = heap->pFirstNode;
   }
   else
   {
      // Workaround in case free is called while walking the heap.
      // The current version of AEEHeap_FreeGroup does this (but shouldn't)
      // pHeapWalkNode and pUpdatedHeapWalkNode will always point to the 
      // same node, unless someone frees that node 
      if (pHeapWalkNode == pWalkBuf->p)
      {
         pWalkBuf->p = pUpdatedHeapWalkNode;
      }

      phn = PTR_TO_NODE(pWalkBuf->p);

      if (!QuickValidateNode(heap, phn))
      {
         return EMEMPTR;
      }

      phn = NEXT_RIGHT_NODE(phn);

      if (phn == NULL)
      {
         return ENOMEMORY;
      }
   }

   if (!QuickValidateNode(heap, phn))
   {
      return EHEAP;
   }

   pWalkBuf->p = NODE_TO_PTR(phn);
   pHeapWalkNode = pWalkBuf->p;
   pUpdatedHeapWalkNode = pWalkBuf->p;

   return AEEkHeap_NodeInfo(pHeap, pWalkBuf, uWalkBufSize);
} /* END AEEkHeap_Walk */
