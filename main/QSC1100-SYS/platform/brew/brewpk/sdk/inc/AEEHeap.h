#ifndef AEEHEAP_H
#define AEEHEAP_H
/*===========================================================================

FILE:      AEEHeap.h
                 
 
SERVICES:  AEE Heap Interface services for application developers of mobile devices

 
DESCRIPTION: This file provides defintions for the Heap interfaces made available by
the AEE to application developers. This is a standard header file that must be included by
all applications using the heap services of the AEE


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEStdLib.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================



//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------


//********************************************************************************************************************************
//
// IHeap Interface
//
//********************************************************************************************************************************

QINTERFACE(IHeap)
{
   DECLARE_IBASE(IHeap)

   void *         (*Malloc)(IHeap * pIHeap, uint32 dwSize);
   void *         (*Realloc)(IHeap * pIHeap, void * pMemBlock, uint32 dwNewSize);
   void           (*Free)(IHeap * pIHeap, void * pMemFree);
   AECHAR *       (*StrDup)(IHeap * pIHeap, AECHAR * pStr);
   boolean        (*CheckAvail)(IHeap *pIHeap, uint32 dwSize); 
   uint32         (*GetMemStats)(IHeap *pIHeap);  
   int            (*GetModuleMemStats)(IHeap *pIHeap, AEECLSID cls, uint32 *pMaxHeap, uint32 *pCurHeapUsed);
};

// Access Macros

#define IHEAP_AddRef(p)            GET_PVTBL(p,IHeap)->AddRef(p)
#define IHEAP_Release(p)           GET_PVTBL(p,IHeap)->Release(p)
#if (AEE_DBG_HEAP == 1)
#define IHEAP_Malloc(p,s)          DBGHEAPMARK(GET_PVTBL(p,IHeap)->Malloc((p),(s)))
#define IHEAP_Realloc(p,pm,s)      DBGHEAPMARK(GET_PVTBL(p,IHeap)->Realloc((p),(pm),(s)))
#define IHEAP_StrDup(p,s)          DBGHEAPMARK(GET_PVTBL(p,IHeap)->StrDup((p),(s)))
#else
#define IHEAP_Malloc(p,s)          GET_PVTBL(p,IHeap)->Malloc((p),(s))
#define IHEAP_Realloc(p,pm,s)      GET_PVTBL(p,IHeap)->Realloc((p),(pm),(s))
#define IHEAP_StrDup(p,s)          GET_PVTBL(p,IHeap)->StrDup((p),(s))
#endif /* if (AEE_DBG_HEAP == 1) */

#define IHEAP_Free(p,pm)           GET_PVTBL(p,IHeap)->Free((p),(pm))
#define IHEAP_MallocRec(p,type)    (type*)IHEAP_Malloc((p),sizeof(type))

#define IHEAP_CheckAvail(p,s)      GET_PVTBL(p,IHeap)->CheckAvail(p,s)
#define IHEAP_GetMemStats(p)       GET_PVTBL(p,IHeap)->GetMemStats(p)
#define IHEAP_GetModuleMemStats(p,cls,pMax,pCur) GET_PVTBL(p,IHeap)->GetModuleMemStats((p),(cls),(pMax),(pCur))

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IHeap Interface

The IHeap Interface includes functions for allocating and freeing memory and for
obtaining information about the amount of device memory that is available and in
use. For simple memory allocation tasks, use the MALLOC() and FREE() macros that
are defined in the file AEEStdLib.h.

IHEAP_CheckAvail() checks whether there is enough memory available to allocate a
block of a specified size. IHEAP_GetMemStats() returns the amount (number of bytes)
of memory in the system that is currently in use. You can use ISHELL_GetDeviceInfo()
to obtain the total amount of memory.

IHEAP_Malloc() allocates a block of memory of a specified size and returns a pointer
to it. IHEAP_MallocRec() takes as input a data type; it allocates enough memory
for a single instance of this data type and returns a pointer to the memory allocated
(the pointer is cast to the specified type). IHEAP_Realloc() changes the size of
an allocated memory block. IHEAP_StrDup() makes a copy of a character string and
returns a pointer to the memory allocated to hold the copy. IHEAP_Free() frees the
memory blocks allocated by these functions.

The following header file is required:~
AEEHeap.h

=============================================================================

IHEAP_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced BREW Client 1.0

See Also: 
    IHEAP_Release()

=============================================================================

IHEAP_CheckAvail() 

Description:
    This function checks whether a memory block of the given size can be allocated.
The function does not do any actual allocation of memory. It returns TRUE or FALSE
indicating whether or not it is possible to allocate a block of the given size.

Prototype:
boolean IHEAP_CheckAvail(IHeap * pIHeap, uint32 dwSize) 

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    dwSize  :  Size of the block whose allocation needs to be verified.

Return Value:
    TRUE, if a block of the given size can be allocated.
    FALSE, if unsuccessful or if a block of the given size cannot be allocated.

Comments: 
None

Side Effects: 
    This function may walk through the heap and collapse any adjacent free blocks.

Version:
   Introduced BREW Client 1.0

See Also:
IHEAP_GetMemStats() 

=============================================================================

IHEAP_Free() 

Description:
    This function frees an allocated block of memory. It releases the memory back
to the memory pool.

Prototype:
void IHEAP_Free(IHeap * pIHeap, void * pMemFree) 

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    pMemFree  :  Pointer to the memory block that is to be freed.

Return Value: 
None

Comments: 
None

Version:
   Introduced BREW Client 1.0

See Also:
None

=============================================================================

IHEAP_GetMemStats() 

Description:
    This function returns statistics about the total memory currently in use. To
check if a block of a specific size can be allocated, the function IHEAP_CheckAvail()
must be called. To get the total memory in the system, the function ISHELL_GetDeviceInfo()
must be called.

Prototype:
    uint32 IHEAP_GetMemStats(IHeap * pIHeap)

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.

Return Value:
Total used memory in the system

Comments: 
None

Version:
   Introduced BREW Client 1.0

See Also:
IHEAP_CheckAvail() 
ISHELL_GetDeviceInfo() 

=============================================================================

IHEAP_Malloc() 

Description:
   This function allocates a block of memory of the requested size
and returns a pointer to that memory block.  By default, memory
allocated using IHEAP_Malloc() is initialized to zero.  This behavior
can be disabled by performing a bitwise OR of the flag ALLOC_NO_ZMEM
with the dwSize parameter.

Prototype:
void * IHEAP_Malloc(IHeap * pIHeap, uint32 dwSize) 

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    dwSize  :  Specifies the size of the memory block to be allocated (optionally ORed with ALLOC_NO_ZMEM).

Return Value:
    Pointer to the allocated memory block, if successful.
    NULL: if fails.

Comments: 
   Specifying a dwSize of zero will result in a return value of NULL.

Version:
   Introduced BREW Client 1.0

See Also:
IHEAP_Realloc() 
IHEAP_MallocRec() 

=============================================================================

IHEAP_MallocRec() 

Description:
This function allocates the memory required for a specified standard data type.
It then casts the allocated pointer to that data type before returning to the caller.
This function zero-initializes the memory that it returns.

Prototype:
(type * ) IHEAP_MallocRec(IHeap * pIHeap,type dataType) 

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    dataType  :  Specifies the standard data type for which memory needs to be allocated.

Return Value:
    Pointer to the requested data type, if successful.
    NULL: If fails.

Comments: 
None

Version:
   Introduced BREW Client 1.0

See Also:
IHEAP_Malloc() 
IHEAP_Realloc() 

=============================================================================

IHEAP_Realloc() 

Description:
This function reallocates a memory block and changes its size.  By
default, any extra memory allocated using IHEAP_Realloc() is
initialized to zero.  This behavior can be disabled by performing a
bitwise OR of the flag ALLOC_NO_ZMEM with the dwNewSize parameter.

Prototype:
    void * IHEAP_Realloc
    (
    IHeap * pIHeap, 
    void * pMemBlock, 
    uint32 dwNewSize
    )

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    pMemBlock  :  Pointer to the memory block that needs to be reallocated. If pMemblock
is NULL: this function behaves the same way as IHEAP_Malloc() and allocates a new
block of dwNewSize bytes.
    dwNewSize  :  Specifies the new size of the memory block (optionally ORed with ALLOC_NO_ZMEM).

Return Value:
    Pointer to the reallocated block, if successful.
    NULL: if fails.

Comments: 
    The return value is NULL if the size is 0 (zero) and pMemBlock is valid, or
if there is not enough available memory to expand pMemBlock to the given size. In
the first case, the original block is freed. In the second, the original block is
unchanged.

Version:
   Introduced BREW Client 1.0

See Also:
None

=============================================================================

IHEAP_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also: 
    IHEAP_AddRef() 

=============================================================================

IHEAP_StrDup() 

Description:
This function duplicates a given string. It allocates memory for the new string
and then copies the contents of the incoming string into this new string. It then
returns the new string.

Prototype:
AECHAR * IHEAP_StrDup(IHeap * pIHeap, AECHAR * pszIn) 

Parameters:
    pIHeap  :  Pointer to the IHeap Interface object.
    pszIn  :  Pointer to the string that needs to be duplicated.

Return Value:
    Pointer to the duplicated string, if successful.
    NULL: if fails.

Comments: 
None

Version:
   Introduced BREW Client 1.0

See Also:
AECHAR

=============================================================================
IHEAP_GetModuleMemStats() 

Description:
This function reports statistics on memory (RAM) used by each Module. It reports two pieces
of information: 1)Heap (in bytes) currently being used by this module. 2) Maximum heap
(in bytes) used by this module upto this point. 
ClassID denotes the app for which the heap statistics are required. This function returns the
heap statistics for the module to which this app belongs. This means, if there are two apps
that belong to the same module (same .mif), this function returns combined statistics for the
entire module and not for the individual apps. 
If a class ID 0 is given the stats are returned for the system. 
For system context the maximum heap used information is not provided and is 
set to 0, only heap used currently is provided.

If the class ID passed to the function is not the same as that of the calling 
app, the calling app needs to have a group privilege level of AEEGROUPID_MEMORY_STATS.
Any app wanting to get the system level memory stats needs to have the AEEGROUPID_MEMORY_STATS 
privilege level.

Prototype:
int IHEAP_GetModuleMemStats(IHeap *pIHeap, AEECLSID cls, uint32 *pMaxHeap, uint32 *pCurHeapUsed)

Parameters:
   pIHeap : [IN] Pointer to the IHeap Interface object.
   cls    : [IN] class ID for whose module the memory stats are requested.
   pMaxHeap : [OUT] uint32 pointer in which the maximum amount of heap used by the module is returned.
              The unit is bytes. For system level memory stats this value is not provided and is set to 0.
   pCurHeapUsed : [OUT] uint32 pointer in which the current amount of heap used by the module is returned.
                  The unit is bytes.

Return Value:
   SUCCESS: if able to find the memory stats for the given class id.
   ENOSUCH: if fails to find the app for the given class ID.
   EPRIVLEVEL: if the calling app does not have group privilege for getting memory stats for class ID specified.
   EBADPARM: if pMaxHeap and pCurHeapUsed is a null pointer when class ID is not 0. If pCurHeapUsed is NULL
            when class ID is 0.
               
Comments: 
   None.

Version:
   Introduced BREW Client 3.1.3

See Also:
   None

=============================================================================


=============================================================================*/

#endif    // AEEHEAP_H
