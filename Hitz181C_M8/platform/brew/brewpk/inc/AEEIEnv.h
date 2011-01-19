#ifndef AEEIENV_H
#define AEEIENV_H
/*=============================================================================
Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEIRealloc.h"

#define AEEIID_IEnv 0x0102346d

#define INHERIT_IEnv(iname) \
   INHERIT_IRealloc(iname); \
   int   (*CreateInstance)(iname* p, AEECLSID cls, void** ppif); \
   int   (*AtExit)(iname* p, void (*pfnAtExit)(void* pCtx), void* pCtx)

AEEINTERFACE_DEFINE(IEnv);

static __inline uint32 IEnv_AddRef(IEnv* me)
{
   return AEEGETPVTBL(me,IEnv)->AddRef(me);
}

static __inline uint32 IEnv_Release(IEnv* me)
{
   return AEEGETPVTBL(me,IEnv)->Release(me);
}

static __inline int IEnv_QueryInterface(IEnv* me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IEnv)->QueryInterface(me, cls, ppo);
}

static __inline int IEnv_CreateInstance(IEnv* me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IEnv)->CreateInstance(me, cls, ppo);
}

static __inline int IEnv_AtExit(IEnv* me, 
                                void (*pfnAtExit)(void* pCtx), void* pCtx)
{
   return AEEGETPVTBL(me,IEnv)->AtExit(me, pfnAtExit, pCtx);
}

static __inline int IEnv_ErrReallocName(IEnv* me,  
                                            int nSize, void** pp, 
                                            const char* cpszName)
{
   return AEEGETPVTBL(me,IEnv)->ErrReallocName(me, nSize, 
                                                   pp, cpszName);
}

static __inline int IEnv_ErrMallocName(IEnv* me, int nSize, 
                                           void** pp, 
                                           const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(me,IEnv)->ErrReallocName(me, nSize, 
                                                   pp, cpszName);
}

static __inline int IEnv_ErrReallocNameNoZI(IEnv* me,
                                                int nSize, void** pp, 
                                                const char* cpszName)
{
   return AEEGETPVTBL(me,IEnv)->ErrReallocNameNoZI(me, nSize, 
                                                       pp, cpszName);
}

static __inline int IEnv_ErrMallocNameNoZI(IEnv* me, int nSize, 
                                               void** pp, 
                                               const char* cpszName)
{
   *pp = 0;
   return AEEGETPVTBL(me,IEnv)->ErrReallocNameNoZI(me, nSize, 
                                                       pp, cpszName);
}

static __inline int IEnv_Free(IEnv* me, void* p)
{
   return AEEGETPVTBL(me,IEnv)->ErrRealloc(me, 0, &p);
}

#if !defined(_DEBUG)

static __inline int IEnv_ErrRealloc(IEnv* me, int nSize, void** pp)
{
   return AEEGETPVTBL(me,IEnv)->ErrRealloc(me, nSize, pp);
}

static __inline int IEnv_ErrMalloc(IEnv* me, int nSize, void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(me,IEnv)->ErrRealloc(me, nSize, pp);
}

static __inline int IEnv_ErrReallocNoZI(IEnv* me, 
                                            int nSize, void** pp)
{
   return AEEGETPVTBL(me,IEnv)->ErrReallocNoZI(me, nSize, pp);
}

static __inline int IEnv_ErrMallocNoZI(IEnv* me, int nSize, 
                                           void** pp)
{
   *pp = 0;
   return AEEGETPVTBL(me,IEnv)->ErrReallocNoZI(me, nSize, pp);
}

#else /* #if !defined(_DEBUG) */

#define IEnv_ErrRealloc(me, nSize, pp)  \
    IEnv_ErrReallocName((me), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrMalloc(me, nSize, pp)  \
    IEnv_ErrMallocName((me), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrReallocNoZI(me, nSize, pp)  \
    IEnv_ErrReallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#define IEnv_ErrMallocNoZI(me, nSize, pp)  \
    IEnv_ErrMallocNameNoZI((me), (nSize), (pp), __FILE_LINE__)

#endif /* #if !defined(_DEBUG) */

#define IENV_FREEIF(me, p) \
    do { if (p) { (void)IEnv_ErrRealloc((me), 0,(void**)&(p)); (p) = 0; } } while (0)

#define IENV_ERRMALLOCRECEX(me, t, nSize, pp) \
    (*(pp)=(t*)0,IEnv_ErrMalloc((me), (int)sizeof(t)+(nSize), (void**)(pp)))

#define IENV_ERRMALLOCREC(me, t, pp) \
    (*(pp)=(t*)0,IEnv_ErrMalloc((me), (int)sizeof(t), (void**)(pp)))

/*
=====================================================================
MACROS DOCUMENTATION
=====================================================================

IENV_ERRMALLOCREC()

Description:
    This macro is inherited from IREALLOC_ERRMALLOCREC()

=====================================================================

IENV_ERRMALLOCRECEX()

Description:
    This macro is inherited from IREALLOC_ERRMALLOCRECEX()

=====================================================================

IENV_FREEIF()

Description:
    This macro is inherited from IREALLOC_FREEIF()

=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IEnv Interface

  IEnv is the basic set of services a component typically requires 
  to be instantiated and initialized.  It is passed to a component's
  CreateInstance method when an instance of a class from the component 
  is requested elsewhere in the system.

  An instance of IEnv is provided to a component's CreateInstance
  function, and must be AddRef'd to tell the component infrastructure
  that the dynamic code of the module is still in use.  This object 
  also typically provides ISingleton and ILocalStorage (for storage
  process-global objects).

=======================================================================

IEnv_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 

See Also:
    IEnv_Release()

=======================================================================

IEnv_Release()

Description:
    This function is inherited from IQI_Release(). 

See Also:
    IEnv_AddRef()

=======================================================================

IEnv_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

=======================================================================
IEnv_CreateInstance()

Description:

  Instantiates an instance of the class specified, if possible.


Prototype:

   int IEnv_CreateInstance(IEnv* pif, AEECLSID cls, void** ppif);

Parameters:
   pif :      Pointer to the IEnv interface
   cls :      The AEECLSID of the class to instantiate
   ppif[out]: pointer to be filled with new instance of the class
              requested

Return value:
    AEE_SUCCESS if instance of class created, otherwise a failure-specific
     error value from AEEError.h

Comments:  
   None

See Also:
    None

=============================================================================

IEnv_AtExit()

Description: 
   Allows caller to register (or deregister) a function to be called during
   Env cleanup.  Env cleanup occurs when the last reference to the Env is 
   released by its owner.  Sub-objects of an Env do not affect the Env's 
   lifetime.
   
   The AtExit callbacks are called in reverse order of subscription.

   This facility allows objects to "AddRef themselves", if necessary, 
   without incurring leaks when the containing environment is cleaned 
   up.

    Owner's reference to Env
===pre>
      o 
   +--|--------------------------------------------+
   | Env                                           | 
   |                                               | 
   |    Obj ref, held by AtExit()                  | 
   |    o                                          | 
   |  +-|----+                                     | 
   |  | Obj  |                                     | 
   |  |      |                                     | 
   |  | o--> piEnv (non-refcounting ref to Env)    |
   |  +------+                                     | 
   |                                               | 
   +-----------------------------------------------+
===/pre>
Prototype:
   int IEnv_AtExit(IEnv *me, void (*pfnAtExit)(void* pCtx), void* pCtx)

Parameters:
   me :       Pointer to the IEnv interface
   pfnAtExit: Function to call on system exit. A value of NULL cancels
              all function previously registered with the same pCtx value.
   pCtx:      Function context to be passed to pfnAtExit.

Return value:
   AEE_SUCCESS if all goes well, otherwise a failure-specific error value

Comments:  
   None

See Also:
   None
=======================================================================

IEnv_ErrRealloc()

Description:
  This function is inherited from IRealloc_ErrRealloc()

=======================================================================

IEnv_Free()

Description:
  This function is inherited from IRealloc_Free()

=======================================================================

IEnv_ErrMalloc()

Description:
  This function is inherited from IRealloc_ErrMalloc()

=======================================================================

IEnv_ErrReallocNoZI()

Description:
  This function is inherited from IRealloc_ErrReallocNoZI()

=======================================================================

IEnv_ErrMallocNoZI()

Description:
  This function is inherited from IRealloc_ErrMallocNoZI()

=======================================================================

IEnv_ErrReallocName()

Description:
  This function is inherited from IRealloc_ErrReallocName()

=======================================================================

IEnv_ErrMallocName()

Description:
  This function is inherited from IRealloc_ErrMallocName()

=======================================================================

IEnv_ErrReallocNameNoZI()

Description:
  This function is inherited from IRealloc_ErrReallocNameNoZI()

=======================================================================

IEnv_ErrMallocNameNoZI()

Description:
  This function is inherited from IRealloc_ErrMallocNameNoZI()

=======================================================================
*/

#endif /* #ifndef AEEIENV_H */

