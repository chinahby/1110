#ifndef AEEIREALLOC_H
#define AEEIREALLOC_H
/*=============================================================================
Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEIQI.h"

#define AEEIID_IRealloc 0x0102345e

#define INHERIT_IRealloc(iname) \
   INHERIT_IQI(iname); \
   int (*ErrRealloc)(iname* me, int nSize, void** ppOut); \
   int (*ErrReallocName)(iname* me, int nSize, void** ppOut, \
                         const char* cpszName);\
   int (*ErrReallocNoZI)(iname* me, int nSize, void** ppOut); \
   int (*ErrReallocNameNoZI)(iname* me, int nSize, void** ppOut, \
                             const char* cpszName)

AEEINTERFACE_DEFINE(IRealloc);

static __inline uint32 IRealloc_AddRef(IRealloc* me)
{
   return AEEGETPVTBL(me,IRealloc)->AddRef(me);
}

static __inline uint32 IRealloc_Release(IRealloc* me)
{
   return AEEGETPVTBL(me,IRealloc)->Release(me);
}

static __inline int IRealloc_QueryInterface(IRealloc* me, AEECLSID cls, 
                                            void** ppo)
{
   return AEEGETPVTBL(me,IRealloc)->QueryInterface(me, cls, ppo);
}

static __inline int IRealloc_ErrReallocName(IRealloc* me,  
                                            int nSize, void** pp, 
                                            const char* cpszName)
{
   return AEEGETPVTBL(me,IRealloc)->ErrReallocName(me, nSize, 
                                                   pp, cpszName);
}

static __inline int IRealloc_ErrMallocName(IRealloc* me, int nSize, 
                                           void** pp, 
                                           const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(me,IRealloc)->ErrReallocName(me, nSize, 
                                                   pp, cpszName);
}

static __inline int IRealloc_ErrReallocNameNoZI(IRealloc* me,
                                                int nSize, void** pp, 
                                                const char* cpszName)
{
   return AEEGETPVTBL(me,IRealloc)->ErrReallocNameNoZI(me, nSize, 
                                                       pp, cpszName);
}

static __inline int IRealloc_ErrMallocNameNoZI(IRealloc* me, int nSize, 
                                               void** pp, 
                                               const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(me,IRealloc)->ErrReallocNameNoZI(me, nSize, 
                                                       pp, cpszName);
}

static __inline int IRealloc_Free(IRealloc* me, void* p)
{
   return AEEGETPVTBL(me,IRealloc)->ErrRealloc(me, 0, &p);
}

#if !defined(_DEBUG)

static __inline int IRealloc_ErrRealloc(IRealloc* me, int nSize, void** pp)
{
   return AEEGETPVTBL(me,IRealloc)->ErrRealloc(me, nSize, pp);
}

static __inline int IRealloc_ErrMalloc(IRealloc* me, int nSize, void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(me,IRealloc)->ErrRealloc(me, nSize, pp);
}

static __inline int IRealloc_ErrReallocNoZI(IRealloc* me, 
                                            int nSize, void** pp)
{
   return AEEGETPVTBL(me,IRealloc)->ErrReallocNoZI(me, nSize, pp);
}

static __inline int IRealloc_ErrMallocNoZI(IRealloc* me, int nSize, 
                                           void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(me,IRealloc)->ErrReallocNoZI(me, nSize, pp);
}

#else /* #if !defined(_DEBUG) */

#define IRealloc_ErrRealloc(me, nSize, pp)  \
    IRealloc_ErrReallocName((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrMalloc(me, nSize, pp)  \
    IRealloc_ErrMallocName((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrReallocNoZI(me, nSize, pp)  \
    IRealloc_ErrReallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#define IRealloc_ErrMallocNoZI(me, nSize, pp)  \
    IRealloc_ErrMallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#endif /* #if !defined(_DEBUG) */

#define IREALLOC_FREEIF(me, p) \
   do { if (p) { IRealloc_ErrRealloc((me), 0,(void**)&(p)); (p) = 0; } } while (0)

#define IREALLOC_ERRMALLOCRECEX(me, t, nSize, pp) \
    (*(pp)=(t*)0,IRealloc_ErrMalloc((me), (int)sizeof(t)+(nSize), (void**)(pp)))

#define IREALLOC_ERRMALLOCREC(me, t, pp) \
    (*(pp)=(t*)0,IRealloc_ErrMalloc((me), (int)sizeof(t), (void**)(pp)))


/*
=====================================================================
MACROS DOCUMENTATION
=====================================================================

IREALLOC_ERRMALLOCREC()

Description:
    Allocate a typed block of memory

Definition:
   IREALLOC_ERRMALLOCREC(me, t, pp) \
       (*(pp)=(t*)0,IRealloc_ErrMalloc(me, sizeof(t), (void**)pp))

Parameters:
  me: pointer to an instance of IRealloc to get memory from
  t: C datatype for which the memory will be used
  pp: where to store newly allocated memory

Evaluation Value:
   integer error value, AEE_SUCCESS if all goes well, otherwise 
     a failure-specific error value

Comments:
   on failure, pp will be set to NULL

=======================================================================

IREALLOC_ERRMALLOCRECEX()

Description:
    Allocate a typed block of memory plus some extra space

Definition:
   IREALLOC_ERRMALLOCRECEX(me, t, u, pp) \
       (*(pp)=(t*)0,IRealloc_ErrMalloc(me, sizeof(t)+u, (void**)pp))

Parameters:
  me: pointer to an instance of IRealloc to get memory from
  t: C datatype for which the memory will be used
  u: integer number of extra bytes to allocate
  pp: where to store newly allocated memory

Evaluation Value:
   integer error value, AEE_SUCCESS if all goes well, otherwise 
     a failure-specific error value

Comments:
   on failure, pp will be set to NULL

=======================================================================

IREALLOC_FREEIF()

Description:
    Free a pointer and set to NULL

Definition:
   IREALLOC_FREEIF(me, p) \
      (void)IRealloc_ErrRealloc((me),0,(void**)&(p))

Parameters:
  me: pointer to an instance of IRealloc to get memory from
  p: pointer to free and clear

Evaluation Value:
   void

Comments:
   p will bet set to NULL

=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IRealloc Interface

  IRealloc is an interface that allows allocating and freeing memory.

  In IRealloc, the pointer value NULL is used to represent a zero-length
  block of memory.  It is a valid result when attempting to allocate
  zero bytes, and can be passed to methods that realloc or free.

  Some methods allow names to be associated with names, primarily for
  debugging purposes.  Names are zero-terminated strings.  The name is copied
  when called, so the caller does not have to preserve the validity of the
  pointer after the IRealloc method returns.  Implementations may place limits
  on the amount of name data they retain.

=======================================================================

IRealloc_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    IRealloc_Release()

=======================================================================

IRealloc_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    IRealloc_AddRef()

=======================================================================

IRealloc_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

=======================================================================

IRealloc_ErrRealloc()

Description:
  Reallocs a block of memory.  Any newly allocated memory is zero-initialized.

  If a size of zero is requested, the function succeeds and
  returns a pointer of NULL.

Prototype:

   int IRealloc_ErrRealloc(IRealloc* pif, int nSize, void** pp);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: new size of the block
   pp [in/out]: on input, the pointer to block of memory to resize,
               on output: the resulting resized block of memory. this may 
              be a new pointer if the block cannot be resized in place

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   IRealloc_ErrRealloc(pIRealloc, p, 0) is the same as IRealloc_Free(pIRealloc,p)
   
   (p=0,IRealloc_ErrRealloc(pIRealloc, size, &p)) is the same as 
         IRealloc_ErrMalloc(pIRealloc, size, &p).

   On failure, *pp is unmolested.

=======================================================================

IRealloc_Free()

Description:
  Frees a block of memory, given its pointer.  If passed NULL, it
  does nothing.

Prototype:
   int IRealloc_Free(IRealloc* pif, void* p);

Parameters:
   pif: Pointer to an IRealloc interface
   p: the pointer to block of memory to free

Return Value:
   AEE_SUCCESS if all goes well (pointer freed or is NULL), otherwise, 
     a failure-specific error value

=======================================================================

IRealloc_ErrMalloc()

Description:
   Allocates a block of memory.  The memory is zero-initialized.

Prototype:
   int IRealloc_ErrMalloc(IRealloc* pif, int nSize, void** pp);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: size of the block to allocate
   pp [out]: pointer to pointer to fill with new block

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is set to NULL.

See Also:
   IRealloc_ErrRealloc()

=======================================================================

IRealloc_ErrReallocNoZI()

Description:
   This function is the same as IRealloc_ErrRealloc(), except that the new
   memory is not zero-initializated.

Prototype:
   int IRealloc_ErrReallocNoZI(IRealloc* pif, int nSize, void** pp);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: new size of the block
   pp [in/out]: on input, the pointer to block of memory to resize,
               on output: the resulting resized block of memory. this may 
              be a new pointer if the block cannot be resized in place

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is unmolested.

=======================================================================

IRealloc_ErrMallocNoZI()

Description:
  This function is exactly the same as IRealloc_ErrMalloc(), except that
  the new memory is not zero-initializated.

Prototype:
   int IRealloc_ErrMallocNoZI(IRealloc* pif, int nSize, void** pp);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: size of the block to allocate
   pp [out]: pointer to pointer to fill with new block

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is set to NULL.

=======================================================================

IRealloc_ErrReallocName()

Description:
   Reallocates a block of memory to a new size, assigning it a name.  Any
   newly allocated memory is zero-initialized.

Prototype:

   int IRealloc_ErrReallocName(IRealloc* me, int nSize, 
                               void** pp, const char* cpszName)

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: new size of the block
   pp [in/out]: on input, the pointer to block of memory to resize,
               on output: the resulting resized block of memory. this may 
              be a new pointer if the block cannot be resized in place
   cpszName: a null-terminated string to associate with the new node;
              may be NULL

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is unmolested.

=======================================================================

IRealloc_ErrMallocName()

Description:
   Allocates a block of memory, and tags it with a name.

Prototype:
   int IRealloc_ErrMallocName(IRealloc* pif, int nSize, void** pp,
                               const char* cpszName);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: size of the block to allocate
   pp [out]: pointer to pointer to fill with new block
   cpszName: a null-terminated string to associate with the new node;
              may be NULL

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is set to NULL.

=======================================================================

IRealloc_ErrReallocNameNoZI()

Description:
   This function is the same as IRealloc_ErrReallocName(), except that
   the new memory is not zero-initializated.

Prototype:
   int IRealloc_ErrReallocNameNoZI(IRealloc* pif, int nSize
                                   void** pp, const char* cpszName);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: new size of the block
   pp [in/out]: on input, the pointer to block of memory to resize,
               on output: the resulting resized block of memory. this may 
              be a new pointer if the block cannot be resized in place
   cpszName: a null-terminated string to associate with the new node.
              may be NULL

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is unmolested.

=======================================================================

IRealloc_ErrMallocNameNoZI()

Description:
   This function is the same as IRealloc_ErrMallocName(), except that
   the new memory is not zero-initializated.

Prototype:
   int IRealloc_ErrMallocNameNoZI(IRealloc* pif, int nSize, void** pp,
                                  const char* cpszName);

Parameters:
   pif: Pointer to an IRealloc interface
   nSize: size of the block to allocate
   pp [out]: pointer to pointer to fill with new block
   cpszName: a null-terminated string to associate with the new node.
              may be NULL

Return Value:
   AEE_SUCCESS if all goes well, otherwise, a failure-specific error value

Comments:
   On failure, *pp is set to NULL.

======================================================================= */

#endif /* #ifndef AEEIREALLOC_H */
