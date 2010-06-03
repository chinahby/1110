#ifndef AEETHREAD_H
#define AEETHREAD_H /* #ifndef AEETHREAD_H */
/*=====================================================

FILE:  AEEThread.h

SERVICES:  
        user-level threads

GENERAL DESCRIPTION:
        interface definition for a cooperatively-scheduled threads 
        implementation

=====================================================

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/

#include "AEE.h"
#include "AEERscPool.h"

typedef struct IThread IThread;

/*
  ---- IThread start function type 
*/
typedef int (*PFNTHREAD)(IThread *piThread, void *pvCxt);

#define INHERIT_IThread(iname)\
   INHERIT_IRscPool(iname);\
   int          (*Start)        (iname *p, int nStackSz, \
                                 PFNTHREAD pfStart, void *pvStart);\
   int          (*Exit)         (iname *p, int nRv);\
   void         (*Join)         (iname *p, AEECallback *pcb, int *pnRv);\
   void         (*Suspend)      (iname *p);\
   AEECallback *(*GetResumeCBK) (iname *p)


/*
  ---- IThread interface def
*/
AEEINTERFACE(IThread) {
   INHERIT_IThread(IThread);
};



#define ITHREAD_AddRef(p)             AEEGETPVTBL((p),IThread)->AddRef((p))
#define ITHREAD_Release(p)            AEEGETPVTBL((p),IThread)->Release((p))
#define ITHREAD_QueryInterface(p,i,o) AEEGETPVTBL((p),IThread)->QueryInterface((p),(i),(o))

#define ITHREAD_Malloc(p,s)           AEEGETPVTBL((p),IThread)->Malloc((p),(s))
#define ITHREAD_Free(p,m)             AEEGETPVTBL((p),IThread)->Free((p),(m))
#define ITHREAD_HoldRsc(p,r)     AEEGETPVTBL((p),IThread)->HoldRsc((p),(r))   
#define ITHREAD_ReleaseRsc(p,r)  AEEGETPVTBL((p),IThread)->ReleaseRsc((p),(r))
#define ITHREAD_Start(p,s,f,x)   AEEGETPVTBL((p),IThread)->Start((p),(s),(f),(x)) 
#define ITHREAD_Exit(p,r)        AEEGETPVTBL((p),IThread)->Exit((p),(r))      
#define ITHREAD_Join(p,cb,r)     AEEGETPVTBL((p),IThread)->Join((p),(cb),(r))   
#define ITHREAD_Suspend(p)       AEEGETPVTBL((p),IThread)->Suspend((p))     
#define ITHREAD_GetResumeCBK(p)  AEEGETPVTBL((p),IThread)->GetResumeCBK((p))

#define ITHREAD_Stop(p) ITHREAD_Exit((p),-1) /* presumably don't care about thread 
                                                return value, use -1... */


/*
======================================================================= 
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================

PFNTHREAD

Description:
  PFNTHREAD is the prototype of a thread start function, It is
  the first function a new AEEThread calls.

Definition:
   typedef int (*PFNTHREAD)(IThread *piThread, void *pvCxt);

Parameters:
piThread: the current thread, the thread that is starting.
pvCxt: the user data, the context passed to ITHREAD_Start().

Return Value:
   The thread's return value. Collect it with ITHREAD_Join()

Comments:
When this function returns, any callback waiting for the thread to 
  finish is resumed.

See Also:
  ITHREAD_Start
   ITHREAD_Join


======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IThread

Description: 
   IThread is a cooperatively-scheduled thread that layers over BREW's 
   callback-based APIs.  It exports the methods to start and stop threads 
   and the methods necessary to implement blocking APIs  when given an 
   arbitrary callback-based API.

   IThreads are not re-useable, for example: An _Start() may only be called once.
After a thread returns from its start function, then calling an _Exit() 
   or a  _Stop() , the thread  may  not  be restarter with a _Start().

======================================================================= 

Function: ITHREAD_Start()

Description: 
   This begins the execution of an IThread.

Prototype:

   int ITHREAD_Start(IThread *p, int nStackSz, 
                     PFNTHREAD pfStart, void *pvStart)

Parameters:
   p: Pointer to the IThread
   nStackSz: number of bytes to allocate for the IThread's stack
   pfStart: the IThread start function, what is called when the
                         IThread begins execution
   pvStart: the argument to the IThread start function
   
Return Value:
   SUCCESS:    if everything's ok
   EALREADY:   if _Start() has already been called
   ENOMEMORY:  if the stack size requested could not be allocated

Comments:  
     the nStackSz parameter should be specified as low as 
	 possible to accomodate the IThread's start function 
	 and its user-level descendants. Additional stack bytes 
	 will be allocated for system calls.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Exit()
   ITHREAD_Stop()

======================================================================= 

Function: ITHREAD_Exit()


Description: 
   Both  ITHREAD_Exit() and ITHREAD_Stop() end an IThread's execution and set its return value.

Prototype:

   int ITHREAD_Exit(IThread *p, int nRv)

Parameters:
   p: Pointer to the IThread
   nRv: the IThread's return value. This is used when the callbacks registered via 
  _Join() are resumed.
   
Return Value:
   SUCCESS:  if everything's ok
   EFAILED:  if the IThread's never been _Start()ed
   EALREADY: if the IThread is already stopped

Comments:
   The method _Exit() may be called from inside or outside the thread.
   If this method is called from within the thread, it doesn't return and 
      execution of the thread stops

  The method  _Stop() sets the return value to -1. It is called from 
      outside the thread, when the caller doesn't care about return value

Side Effects: 
   All callbacks registered via _Join() are resumed.

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_HoldRsc, 
   ITHREAD_ReleaseRsc

======================================================================= 
Function: ITHREAD_Stop()

Description: 
   Both  ITHREAD_Exit() and ITHREAD_Stop() end an IThread's execution and set its return value.

Prototype:

   int ITHREAD_Stop(IThread *p)

Parameters:
   p: Pointer to the IThread
   
Return Value:
	Described in ITHREAD_Exit()

======================================================================= 
Function: ITHREAD_Join()

Description: 
  The  ITHREAD_Join() schedules a callback to fire when an IThread's execution ends. 
    It  collects the return value in pnRv.

Prototype:

   void ITHREAD_Join(IThread *p, AEECallback *pcb, int *pnRv)

Parameters:
   p: Pointer to the IThread
   pcb: where to call when the thread is done
   pnRv: pointer to be filled with the IThread's return value
   
Return Value:
   None

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

======================================================================= 

Function: ITHREAD_Malloc()

Description: 
  The ITHREAD_Malloc() allocates memory and associates it with the lifetime of an IThread.

Prototype:

   void *ITHREAD_Malloc(IThread *p, unsigned uSize);

Parameters:
   p: Pointer to the IThread
   uSize: number of bytes to allocate
   
Return Value:
   Fresh memory on success or 0 if memory could not be allocated

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Free, 
   ITHREAD_Start

======================================================================= 

Function: ITHREAD_Free()

Description: 
   The ITHREAD_Free() de-associates a memory pointer with the lifetime of an IThread and
   frees it.

Prototype:

   void ITHREAD_Free(IThread *p, void *pData);

Parameters:
   p: Pointer to the IThread
   pData: pointer to free
   
Return Value:
   None

Comments:
   The pointer pData must have come from ITHREAD_Malloc().

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Malloc, 
   ITHREAD_Start, 
   ITHREAD_ReleaseRsc

======================================================================= 

Function: ITHREAD_HoldRsc()

Description: 
   The ITHREAD_HoldRsc() associates a generic IBase pointer with the lifetime of an IThread.

Prototype:

   int ITHREAD_HoldRsc(IThread *p, IBase *pResource);

Parameters:
   p: Pointer to the IThread
   pResource: thing to keep track of
   
Return Value:
   SUCCESS:   if everything's cool
   ENOMEMORY: if memory could not be allocated to hold the resource

Comments:
  IThread does not increment _AddRef() for the IBase passed. Do not IBase_Release() 
  to release it after calling _HoldRsc(). Use ITHREAD_ReleaseRsc() instead.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Start, 
   ITHREAD_ReleaseRsc()

======================================================================= 

Function: ITHREAD_ReleaseRsc()

Description: 
   The ITHREAD_ReleaseRsc() de-associates a generic IBase pointer with the lifetime of an IThread, 
   It releases the IBase.

Prototype:

   uint32 ITHREAD_ReleaseRsc(IThread *p, IBase *pResource);

Parameters:
   
     p: Pointer to the IThread
     pResource: thing to forget
   
Return Value:
   Value of -1 if the IBase could not be found in the IThread's resource list, 
   
  Otherwise, the return value of IBASE_Release() on the resource.

Comments:
   None

Side Effects: 
   The IBase referenced is released, so do use _Release() again.

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Start, 
   ITHREAD_HoldRsc()

======================================================================= 

Function: ITHREAD_Suspend()

Description: 
   The ITHREAD_Suspend()  pauses the execution of the IThread. It returns the 
    execution state to that of the "primordial" BREW thread.  This function is 
   used for implementing/scheduling blocking calls.

Prototype:

   void ITHREAD_Suspend(IThread *p)

Parameters:
   p: Pointer to the IThread
   
Return Value:
   None

Comments:
   This function must only be called from within the IThread. Calling from 
     outside the IThread has no effect.

   To resume execution of the IThread, call ISHELL_Resume() on the IThread's 
     resume AEECallback (_ResumeCBK()) before calling _Suspend().

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_GetResumeCBK

======================================================================= 

Function: ITHREAD_GetResumeCBK()

Description: 
   The ITHREAD_GetResumeCBK() returns an initialized AEECallback for resumption of the 
      IThread. AEECallback is initialized  by using the CALLBACK_Init().

Prototype:

   AEECallback *ITHREAD_GetResumeCBK(IThread *p)

Parameters:
   p: Pointer to the IThread
   
Return Value:
   AEECallback for resumption of the IThread

Comments:
   Do not call CALLBACK_Init() on the resulting AEECallback, doing so will
     break the thread.

Side Effects: 
   None

Version:
   Introduced BREW Client 3.1

See Also:
   ITHREAD_Suspend

======================================================================= */



#endif /* #ifndef AEETHREAD_H */

