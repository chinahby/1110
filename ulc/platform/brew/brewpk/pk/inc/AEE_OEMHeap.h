/*======================================================
FILE:  AEE_OEMHeap.h

SERVICES:  AEE OEM heap interface

GENERAL DESCRIPTION:

Heap related definitions for the AEE/OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_HEAP_H)
#define AEE_OEM_HEAP_H  1

#include "AEEComdef.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef FEATURE_DYNAMIC_OEM_HEAP
extern void * gpuHeapBytes;
extern uint32 guHeapBytesLen;
#endif

// AEE Helper Routines
extern void   *AEE_GetSysHeap(void);
extern boolean AEE_FreeMemory(uint32 nNeeded);
extern uint32  AEE_GetMemGroup(void);
extern void    AEE_GetMemGroupName(uint32 dwID, char *pszDest, int nDestLen);

//
// BREW Exception Handling - These definitions and functions allow the OEM to throw exceptions under
// special conditions.  This will cause:
//
// - An immediate jump from the current context back to the root of the dispatcher (or message pump);
// - A report of the error in a modal form (timer-based pause)
// - Termination of the offending BREW app
//

typedef enum {
   AEE_EXCEPTION_STACK = -1,     // Stack overflow
   AEE_EXCEPTION_MEMPTR = -2,    // Invalid memory pointer referenced
   AEE_EXCEPTION_HEAP = -3,      // Heap node corruption attempted
   AEE_EXCEPTION_PRIV = -4,      // Privilege Violation
   AEE_EXCEPTION_MALLOC = -5     // Debug exception - Malloc failing...
} AEEExceptionType;

extern AEE_EXPORTS void    AEE_Exception(const char *pszFunc, AEEExceptionType e);

extern AEE_EXPORTS boolean AEE_CheckStack(const char *pszFunc);
extern AEE_EXPORTS boolean AEE_CheckPtr(const char *pszFunc,void *pMem, uint32 nSize,
                            boolean bWrite);

/* Heap Functions */
extern AEE_EXPORTS int       AEEHeap_Init(void * pMem, uint32 dwSize);
extern AEE_EXPORTS char     *AEEHeap_StrDup(const char *psz);
extern AEE_EXPORTS int       AEEHeap_Unlock(void ** ppHandle);
extern AEE_EXPORTS int       AEEHeap_Lock(void ** ppHandle);
extern AEE_EXPORTS void     *AEEHeap_GroupMark(void *ptr, uint32 dwGroupID);
extern AEE_EXPORTS void     *AEEHeap_DbgMark(void *ptr,const char *cpszFile, int nLine);
extern AEE_EXPORTS void     *AEEHeap_Malloc(uint32 uSize);
extern AEE_EXPORTS void      AEEHeap_Free(void *ptr);
extern AEE_EXPORTS void     *AEEHeap_Realloc(void *ptr,uint32 uSize);
extern AEE_EXPORTS uint32    AEEHeap_GetRAMFree(uint32 * pdwTotal, uint32 * pdwMax);
extern AEE_EXPORTS boolean   AEEHeap_SetValidate(boolean bVal);
extern AEE_EXPORTS int       AEEHeap_ValidatePtr(void *pMem, uint32 uSize, uint32 dwFlags);
extern AEE_EXPORTS boolean   AEEHeap_CheckMemAvail(uint32 uSize);
extern AEE_EXPORTS void      AEEHeap_FreeGroup(uint32 dwGroupID);
extern AEE_EXPORTS uint32    AEEHeap_GroupUsed(uint32 dwGroupID);
extern AEE_EXPORTS void      AEEHeap_DumpGroup(uint32 dwGroupID);


#define AEEHEAP_ALL_NODES  (0xffffffff)
#define AEEHEAP_SYS_NODES  (0)

#define AEEHEAP_VPF_HEAPSRCH    0x001 /* walk heap for node */
#define AEEHEAP_VPF_WRITEABLE   0x002 /* check to make sure is in RAM */

// On the simulator, we report the file/line as a help, on a phone,
//   pszFunc is unused, so pass 0 to remove it from the code

#if defined(AEE_SIMULATOR)
#define AEE_CHECK_STACK(f)      AEE_CheckStack(f)
#define AEE_CHECK_PTR(f,p,n,w)  AEE_CheckPtr((f),(void *)(p),(uint32)(n),(w))
#define AEE_CHECK_MEM(f,p,n)    AEE_CHECK_PTR((f),(p),(n),TRUE)
#define AEE_EXCEPTION(f,et)     AEE_Exception((f),(et))
#else /* #if defined(AEE_SIMULATOR) */
#define AEE_CHECK_STACK(f)      AEE_CheckStack(0)
#define AEE_CHECK_PTR(f,p,n,w)  AEE_CheckPtr(0,(void *)(p),(uint32)(n),(w))
#define AEE_CHECK_MEM(f,p,n)    AEE_CHECK_PTR(0,(p),(n),TRUE)
#define AEE_EXCEPTION(f,et)     AEE_Exception(0,(et))
#endif /* #if defined(AEE_SIMULATOR) */


#if defined(__cplusplus)
}
#endif

/*==================================================================
  INTERFACE DOCUMENTATION
=====================================================================
AEE_OEMHeap Interface
====================================================================

Function:  AEE_FreeMemory()

Description:
   This function can be called by the OEM layer when RAM resources
reach a critical low level.  In this case, the AEE attempts to free
any unused RAM.

Prototype:

   boolean AEE_FreeMemory(uint32 nNeeded);

Parameters:
   nNeeded:  required RAM

Return Value:
   TRUE if needed RAM was made available

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_Exception()

Description:
   This function allows the OEM to throw exceptions under special conditions.
   This will cause:
   - An immediate jump from the current context back to the root of the dispatcher (or message pump)
   - A report of the error in a modal form (timer-based pause)
   - Termination of the offending BREW app

Prototype:
   void AEE_Exception(const char *pszFunc, AEEExceptionType e);

Parameters:
   pszFunc:  pointer to the string detailing the exception
   e:        exception type

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: AEE_CheckStack()

Description:
   This function checks whether a stack overflow has occurred or not.

Prototype:
   void AEE_CheckStack(const char *pszFunc);

Parameters:
   pszFunc:  pointer to the string detailing the exception if an exception occurs

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================
Function: AEE_CheckPtr()

Description:
   This function validates a chunk of memory.  It checks the following conditions:
   - Memory is non-NULL.
   - Memory is in valid range.
   - Memory is not in heap.
   - OR, memory update will not overwrite/overread a heap node.

Prototype:
   boolean AEE_CheckPtr(const char *pszFunc, void *pMem, uint32 nSize, boolean bWrite);

Parameters:
   pszFunc:  pointer to the string detailing the exception if an exception occurs
   pMem:     memory buffer
   dwSize:   size in bytes
   bWrite:   Pointer is writable memory or not.

Return Value:
   TRUE if valid pointer, FALSE otherwise

Comments:
   If the pointer is in RAM, but not in the heap, returns TRUE.

   Throws the following exceptions:
   - AEE_EXCEPTION_MEMPTR   if the pointer is totally bad
   - AEE_EXCEPTION_HEAP     if the pointer is in the heap, but would overread/write a
                            heap node

Side Effects:
   None

See Also:
   None

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
AEEHeap Interface
=======================================================================
Function: AEEHeap_Init()

Description: Explicitly initializes BREW's user-mode heap.

Prototype:
   void AEEHeap_Init(void * pMem, uint32 dwSize)

Parameters:
   pMem:    Memory buffer
   dwSize:  Size in bytes

Return Value:
   None

Comments:
   This function must be called prior to calling AEE_Init().  If this
    function is not called before AEE_Init(), OEM_GetInitHeapBytes() must
    be implemented properly.

Side Effects:
   None

See Also:
   OEM_GetInitHeapBytes()


============================================================================

Function: AEEHeap_StrDup()

Description: Copies a string using the AEE heap.

Prototype:
    char *AEEHeap_StrDup(const char *psz);

Parameters:
   psz: null-terminated string to copy

Return Value:
   new string, or null on allocation failure

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================

Function: AEEHeap_Unlock()

Description:
   Unlocks a heap node and sets its sentinel to ppHandle.

Prototype:
  int AEEHeap_Unlock(void ** ppHandle);

Parameters:
   ppHandle: sentinel to use when moving the node. The heap writes
              into this value whenever the node needs to move.

Return Value:
   if >= 0, the current lock count
   -EMEMPTR if ppHandle or *ppHandle is null

Comments:
   None

Side Effects:
   None

See Also:
   AEEHap_Lock()

============================================================================

Function: AEEHeap_Lock()

Description:
   Locks a pointer, making it non-movable.

Prototype:
   int AEEHeap_Lock(void ** ppHandle);

Parameters:
   ppHandle: pointer to pointer to memory, same sentinel as passed to
     Unlock

Return Value:
   Current lock count

Comments:
   None

Side Effects:
   None

See Also:
   AEEHap_Unlock()

============================================================================

Function: AEEHeap_GroupMark()

Description:
   Marks a node as belonging to a group.

Prototype:
   void   *AEEHeap_GroupMark(void *ptr,uint32 dwGroupID)

Parameters:
   ptr: the pointer
   dwGroupID: the group to mark the node for

Return Value:
   ptr

Comments:
   None

Side Effects:
   None

See Also:
   AEEHeap_FreeGroup()
   AEEHeap_DumpGroup()

============================================================================

Function: AEEHeap_DbgMark()

Description:
   Marks a heap node with some debugging info (a string and int,
      usually __FILE__ and __LINE__).

Prototype:
   void   *AEEHeap_DbgMark(void *ptr,const char *cpszFile, int nLine)

Parameters:
   ptr: pointer to mark
   cpszFile: string to mark with
   int nLine: int to mark with

Return Value:
   ptr

Comments:
   This function may have no effect in release builds of BREW.

Side Effects:
   None

See Also:
   AEEHeap_DumpGroup()

============================================================================

Function: AEEHeap_Realloc()

Description:
   Gets memory from and returns memory to the heap.

Prototype:
   void   *AEEHeap_Realloc(void *ptr,uint32 uSize)

Parameters:
   ptr: pointer to resize. If null, means malloc.
   uSize: new size, zero means free, maximum value of 0x7fffffff

Return Value:
   New memory

Comments:
   AEEHeap_Realloc() returns zero-initialized memory by default.  This
     can be suppressed by setting the high bit of uSize as follows:
===pre>
   p = AEEHeap_Realloc(0,10 | 0x80000000); // malloc 10, non-initialized bytes
===/pre>

Side Effects:
   None

See Also:
   AEEHeap_Realloc(), AEEHeap_Free()

============================================================================

Function:   AEEHeap_Free

Description:
   returns memory to the heap

Prototype:
   void   AEEHeap_Free(void *ptr)

Parameter(s):
   ptr: pointer to free

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEHeap_Realloc(), AEEHeap_Malloc()

============================================================================

Function: AEEHeap_GetRAMFree()

Description:
   Finds out how much heap is left.

Prototype:
   uint32  AEEHeap_GetRAMFree(uint32 * pdwTotal, uint32 * pdwMax)

Parameters:
   pdwTotal: out parameter. If non-null, it is filled with the total size of the heap.
   pdwMax: out parameter. If non-null, it is filled with the size of the largest free node.

Return Value:
   Total memory available to malloc

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================


Function: AEEHeap_SetValidate()

Description:
   Sets whether the heap validates itself on access.

Prototype:
   boolean AEEHeap_SetValidate(boolean bVal)

Parameters:
   bVal: whether to validate

Return Value:
   Old validate value

Comments:
   None

Side Effects:
   Each call to the heap can potentially cause a heap walk.

See Also:
   None

============================================================================

Function: AEEHeap_ValidatePtr()

Description:
   Checks a pointer for validity.

Prototype:
   int     AEEHeap_ValidatePtr(void *pMem, uint32 uSize, uint32 dwFlags)

Parameters:
   pMem: pointer to check
   uSize: planned size of pMem
   dwFlags: bit mask of
            AEEHEAP_VPF_HEAPSRCH: walk heap for pointer
            AEEHEAP_VPF_WRITEABLE: verify writability

Return Value:
   SUCCESS if everything's OK
   EMEMPTR if the pointer's not valid
   EHEAP if the heap's corrupt

Comments:
   AEEHeap_ValidatePtr may return SUCCESS if the pointer is not in the heap.

Side Effects:
   None

See Also:
   None

============================================================================

Function: AEEHeap_CheckMemAvail()

Description:
   Checks for memory availability.

Prototype:
   boolean AEEHeap_CheckMemAvail(uint32 uSize)

Parameters:
   uSize: desired allocation size

Return Value:
   TRUE if a node of at least uSize is available to be allocated
   FALSE otherwise

Comments:
   Failure of CheckMemAvail does not necessarily mean that malloc will
    fail, since AEEHeap_CheckMemAvail() does not run AEE_FreeMemory.

Side Effects:
   None

See Also:
   AEEHeap_GetRAMFree()

============================================================================

Function: AEEHeap_FreeGroup()

Description:
   Frees all the nodes marked with dwGroupID.

Prototype:
   void AEEHeap_FreeGroup(uint32 dwGroupID)

Parameters:
   dwGroupID: group to free

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEHeap_GroupMark()
   AEEHeap_GroupUsed()

============================================================================

Function: AEEHeap_GroupUsed()

Description:
   return number of bytes marked with dwGroupID

Prototype:
   uint32  AEEHeap_GroupUsed(uint32 dwGroupID)

Parameters:
   dwGroupID: group to gather use information about

Return Value:
   Number of bytes marked with dwGroupID

Comments:
   None

Side Effects:
   None

See Also:
   AEEHeap_GroupMark()
   AEEHeap_DumpGroup()

============================================================================

Function: AEEHeap_DumpGroup()

Description:
   Generates a debug dump of heap nodes in group dwGroupID.

Prototype:
   void    AEEHeap_DumpGroup(uint32 dwGroupID)

Parameters:
   dwGroupID: group to dump,
~
AEEHEAP_DUMP_ALL_NODES for all, ~
AEEHEAP_DUMP_SYS_NODES for "system" nodes~
*

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEHeap_GroupMark()
   AEEHeap_GroupUsed()
==============================================================
==================================================================*/
#endif // !defined(AEE_OEM_HEAP_H)

