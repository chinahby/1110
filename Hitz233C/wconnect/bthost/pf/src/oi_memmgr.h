#ifndef _MEMMGR_H
#define _MEMMGR_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_memmgr.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
  This header file provides memory management functions.
  
  The memory allocator assumes that all dynamic memory is allocated from a fixed
  size pool. For a desktop environment this pool can be large, but for embedded
  applications this pool may be quite small. A block-based allocation scheme is used to
  prevent memory fragmentation. The size and number of blocks of each allocation
  size will be tuned to the needs of the specific embedded application.  
  
  See @ref memMgr_docpage for more information on the basic function of the Memory Manager.

*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_bt_stack_config.h"
#include "oi_debug.h"

#ifdef OI_DEBUG
#error code not present
#endif


/*
 * OI_Malloc, OI_Calloc
 *
 * Allocates the smallest available memory block that is large enough to hold
 * the specified number of bytes. Returns 0 if there is no memory block of the
 * required size available
 *
 * For debugging, OI_Malloc is a macro which adds file and line number to the
 * malloc call so that we can trace memory hogs and memory leaks.  In order to
 * accomodate a mixed build (some modules compiled for debug, others compiled
 * for release), we need to declare both implementations as external.  The
 * particular function invoked depends then on the state of OI_DEBUG in the
 * module being compiled.
 *
 * OI_Calloc does the allocation and then zeroes the memory before returning it.
 */

#ifdef MEMMGR_DEBUG
#error code not present
#else

#define AV_Malloc(size) _AV_Malloc(size)
void* _AV_Malloc(size_t size);

#define OI_Malloc(size) _OI_Malloc(size)
void* _OI_Malloc(OI_INT32 size);

#define OI_Calloc(size) _OI_Calloc(size)
void* _OI_Calloc(OI_INT32 size);

#endif /* MEMMGR_DEBUG */


/*
 * OI_StaticMalloc
 *
 * Allocates memory from the static allocation pool. This is data that is
 * required for the stack to function and will never explicitly freed.
 * 
 * The memory allocated will be zero'd before returning to caller.
 */

#ifdef MEMMGR_DEBUG
#error code not present
#else

#define OI_StaticMalloc(size) _OI_StaticMalloc(size)
void* _OI_StaticMalloc(OI_INT32 size);

#endif /* MEMMGR_DEBUG */


/*
 * OI_Free
 *
 * Returns a memory block to the memory pool and makes it available for
 * subsequent allocations. Fails with an assertion error if the memory was not
 * allocated by OI_Malloc or if the memory block has become corrupt.
 *
 * OI_FreeIf
 *
 * Passed a reference to a pointer to a dynamic memory block. If the value
 * referenced is non-null, frees the memory and sets the referenced value to
 * null.
 */

#ifdef MEMMGR_DEBUG
#error code not present
#else

#define AV_Free(block) _AV_Free(block)
void _AV_Free(void *block);

#define OI_Free(block) _OI_Free(block)
void _OI_Free(void *block);

#define OI_FreeIf(p) _OI_FreeIf((OI_BYTE**) (p))
void _OI_FreeIf(OI_BYTE **blockRef);

#endif /* MEMMGR_DEBUG */

/*
 * OI_MaxAllocBlock
 *
 * Returns the size of the largest memory block currently available for
 * allocation by a OI_Malloc
 */


OI_UINT32 OI_MaxAllocBlock(void);


/*
 * OI_MinAllocBlock
 *
 * Returns the size of the smallest memory block currently available for
 * allocation by a OI_Malloc
 */


OI_UINT32 OI_MinAllocBlock(void);


/*
 * OI_MemCopy
 *
 * Copy an arbitrary number of bytes from one memory address to another.
 * The underlying implementation is the ANSI memmove() or equivalant, so
 * overlapping memory copies will work correctly.
 */

void OI_MemCopy(void *To, void const *From, OI_UINT32 Size);


/*
 * OI_MemSet
 *
 * Sets all bytes in a block of memory to the same value
 */

void OI_MemSet(void *Block, OI_UINT8 Val, OI_UINT32 Size);


/*
 * OI_MemZero
 *
 * Sets all bytes in a block of memory to zero
 */

void OI_MemZero(void *Block, OI_UINT32 Size);


/*
 * OI_MemCmp
 * 
 * Compare two blocks of memory
 * 
 * Returns:
 *        0, if s1 == s2
 *      < 0, if s1 < s2
 *      > 0, if s2 > s2
 */

OI_INT OI_MemCmp(void const *s1, void const *s2, OI_UINT32 n);


/*
 * OI_MEMMGR_Check, OI_MEMMGR_CheckSize, OI_MEMMGR_DumpPools
 *
 * For debugging only
 *
 * Checks the memory address is a valid memory block address and that has been
 * allocated (i.e. not on the free list) and checks that the contents at that
 * memory block has not been corrupted.
 * 
 * Returns TRUE if the address and contents are ok, FALSE otherwise.
 *
 * OI_MEMMGR_CheckSize also checks that the allocated storage for the memory block
 * is >= Size.
 *
 * Intended for use with the ASSERT macro, for example:
 *
 *     OI_ASSERT(OI_MEMMGR_Check(ptr));
 *     OI_ASSERT(OI_MEMMGR_CheckSize(buffer, 96));
 *
 * OI_MEMMGR_DumpPools uses OI_DbgPrint to print out information about the memory
 * allocation pools. 
 */

#ifdef MEMMGR_DEBUG
#error code not present
#else

#define OI_MEMMGR_Checksum(B)        (0)

#define OI_MEMMGR_Check(B)        (TRUE)

#define OI_MEMMGR_CheckSize(B, S) (TRUE)

#define OI_MEMMGR_DumpPools()

#define OI_MEMMGR_DumpUsedBlocks()

#define OI_MEMMGR_Dump()

#define OI_MEMMGR_CurrentDynamicAllocation()       (0)
#endif /* MEMMGR_DEBUG */


/*****************************************************************************/
#endif /* _MEMMGR_H */

