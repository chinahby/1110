#ifndef AEERSCPOOL_H
#define AEERSCPOOL_H /* #ifndef AEERSCPOOL_H */
/*============================================================================

FILE:  AEERscPool.h

SERVICES:  
        IRscPool, an interface for grouping resources

GENERAL DESCRIPTION:
        A resource pool allows for grouping of objects with identical 
        lifetimes or objects that must be cleaned up in case of an 
        exception, thereby simplifying code that would otherwise have to
        track the resources independently.

============================================================================

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "AEE.h"

typedef struct IRscPool IRscPool;

/*
  ---- The IRscPool base class
*/
#define INHERIT_IRscPool(iname)\
   INHERIT_IQueryInterface(iname);\
   void    *(*Malloc)     (iname *me, unsigned uSize);\
   void     (*Free)       (iname *me, void *p);\
   int      (*HoldRsc)    (iname *me, IBase *pResource);\
   uint32   (*ReleaseRsc) (iname *me, IBase *pResource)


/*
  ---- IRscPool interface definition
*/
AEEINTERFACE(IRscPool) {
   INHERIT_IRscPool(IRscPool);
};

#define IRSCPOOL_AddRef(p)             AEEGETPVTBL((p),IRscPool)->AddRef((p))
#define IRSCPOOL_Release(p)            AEEGETPVTBL((p),IRscPool)->Release((p))
#define IRSCPOOL_QueryInterface(p,i,o) AEEGETPVTBL((p),IRscPool)->QueryInterface((p),(i),(o))
#define IRSCPOOL_Malloc(p,s)           AEEGETPVTBL((p),IRscPool)->Malloc((p),(s))
#define IRSCPOOL_Free(p,m)             AEEGETPVTBL((p),IRscPool)->Free((p),(m))
#define IRSCPOOL_HoldRsc(p,r)          AEEGETPVTBL((p),IRscPool)->HoldRsc((p),(r))
#define IRSCPOOL_ReleaseRsc(p,r)       AEEGETPVTBL((p),IRscPool)->ReleaseRsc((p),(r))


/*
======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IRscPool

Description: 
   IRscPool is a memory- and interface-grouping API useful when lifetimes
   of many resources are identical, or are tedious to free/release.

   IRscPool is particularly useful for threaded applications where
   exceptions may occur, and things must be cleaned up outside the normal
   code path.

   IBases and memory pointers associated with an IRscPool are freed when
   the last reference to the IRscPool goes away.

======================================================================= 

Function: IRSCPOOL_Malloc()

Description: 
   Allocates memory and associates it with the lifetime of an IRscPool

Prototype:

   void *IRSCPOOL_Malloc(IRscPool *p, unsigned uSize);

Parameters:
   p: Pointer to the IRscPool
   uSize: number of bytes to allocate
   
Return Value:
   fresh memory on success, 

   NULL: if memory could not be allocated

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IRSCPOOL_Free

======================================================================= 

Function: IRSCPOOL_Free()

Description: 
   de-associates a memory pointer with the lifetime of an IRscPool and
   frees it

Prototype:

   void IRSCPOOL_Free(IRscPool *p, void *pData);

Parameters:
   p: Pointer to the IRscPool
   pData: pointer to free
   
Return Value:
   None

Comments:
   The pointer pData *must* have come from IRSCPOOL_Malloc

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IRSCPOOL_Malloc

======================================================================= 

Function: IRSCPOOL_HoldRsc()

Description: 
   Associates a generic IBase pointer with the lifetime of an IRscPool

Prototype:

   int IRSCPOOL_HoldRsc(IRscPool *p, IBase *pResource);

Parameters:
   p: Pointer to the IRscPool
   pResource: thing to keep track of
   
Return Value:
   SUCCESS:   if pResource is successfully added to the pool
   ENOMEMORY: if memory could not be allocated to hold the resource

Comments:
  IRscPool does not call IBASE_AddRef() on pResource.  For this reason, the
   caller should not call IBASE_Release() on pResource without first 
   calling IRSCPOOL_ReleaseRsc().  The caller is freed from the 
   responsibility of releasing pResource.

  IRscPool will call IBASE_Release() on pResource when the IRscPool 
   is destroyed (unless IRscPool_ReleaseRsc() is called), accomplishing
   clean-up of pResource.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IRSCPOOL_ReleaseRsc()

======================================================================= 

Function: IRSCPOOL_ReleaseRsc()

Description: 
   De-associates a generic IBase pointer with the lifetime of an IRscPool, 
    _Release()s the IBase

Prototype:

   uint32 IRSCPOOL_ReleaseRsc(IRscPool *p, IBase *pResource);

Parameters:
   p: Pointer to the IRscPool
   pResource: thing to forget
   
Return Value:
   Value of -1 if the IBase could not be found in the IRscPool's resource list, 

   otherwise, the return value of IBASE_Release() on the resource

Comments:
   This method is intended to free pResource.  If the caller wishes merely
     to remove the resource from the pool, the caller must call 
     IBASE_AddRef() on pResource before calling IRSCPOOL_ReleaseRsc()
  
Side Effects: 
   The IBase referenced is _Release()d

Version:
   Introduced BREW Client 3.1

See Also:
   IRSCPOOL_HoldRsc()

======================================================================= 
*/

#endif /* #ifndef AEERSCPOOL_H */
