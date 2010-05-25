#ifndef AEECALLBACKUTIL_H
#define AEECALLBACKUTIL_H /* #ifndef AEECALLBACKUTIL_H */
/*============================================================================

FILE:  AEECallbackUtil.h

SERVICES:  
     making AEECallback easier to deal with, mapping shell/socket stuff to
     AEECallback

GENERAL DESCRIPTION:
     The following stubs create AEECallback-based versions of socket and timer
     calls:
     
        AEECallback_SetTimer
        AEECallback_Readable
        AEECallback_Writeable
        AEECallback_Connect
     
     AEECallback-based scheduling corresponds perfectly to a thread's
     needs.  The AEECallback record keeps track of which thing is
     scheduled so that only one thing is scheduled and whatever is
     scheduled can always be canceled readily.
     
     Given a AEECallback-based asynchronous function, we can usually
     construct a blocking threaded version with two lines of code:
     
          DoSomethingCBK( <args> , ITHREAD_GetResumeCallback(piThread));
          ITHREAD_Suspend(piThread);
============================================================================

Copyright © 1999,2001-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================ */
#include "AEE.h" /* for AEECallback def */
#include "AEEShell.h" /* for IShell */
#include "AEENet.h"   /* for ISocket */


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */



typedef void (*PFNSCHEDNOTIFY)(void *pSchedObj, PFNNOTIFY pfn, void *pcx);

void AEECallback_SchedNotifyWait(AEECallback *pcb, void *pSchedNotifyObj, 
                                 PFNSCHEDNOTIFY pfnSched, 
                                 PFNSCHEDNOTIFY pfnCancel);
   
#define AEECallback_Readable(pcb,psno)   AEECallback_SchedNotifyWait((pcb), (psno), (PFNSCHEDNOTIFY)(psno)->pvt->Readable, (PFNSCHEDNOTIFY)(psno)->pvt->Cancel)
#define AEECallback_Writeable(pcb,psno)  AEECallback_SchedNotifyWait((pcb), (psno), (PFNSCHEDNOTIFY)(psno)->pvt->Writeable, (PFNSCHEDNOTIFY)(psno)->pvt->Cancel)


void AEECallback_Connect(AEECallback *pcb, ISocket *piSock, 
                         INAddr a, INPort p, int *pnError);

void AEECallback_SetTimer(AEECallback *pcb, IShell *piShell, int nMS);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/* 
======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

AEECallback Interface

Description: 
   Some interfaces in BREW use "callback function/callback context"
   semantics instead of using AEECallback to specify both.
   Cancellation of the callback is usually accomplished by
   rescheduling the operation with a NULL callback function and the
   same callback context.

   AEECallbacks are a generalizing structure for scheduling API
   operations that notify the client of the API via a procedure 
   "callback." They encapsulate the most common methods for keeping
   track of where to call, with what, and abstract the notion of 
   cancellation via a pfnCancel member.  AEECallback is a *public*
   data structure, and so can be allocated by API clients instead of 
   another dedicated API about which all users of AEECallbacks 
   must then know.

   This library is designed to unify the two schools of thought, 
   hopefully simplifying and shrinking code that uses this library 
   (AEEThreadUtil in particular).

======================================================================= 

Function: AEECallback_SetTimer()

Description: 
   Schedule a callback nMS milliseconds in the future
   resources.

Prototype:

   void AEECallback_SetTimer(AEECallback *pcb, IShell *piShell, int nMS);

Parameters:
   pcb: AEECallback to use to schedule/cancel op
   piShell: BREW Shell interface pointer (has SetTimer())
   nMS: duration of timer in milliseconds
   
Return Value:
   None

Comments:  
   pcb's pfnNotify and pNotifyData should be initialized before calling this 
      function

Side Effects: 
   pcb is populated with a function inside AEECallbackUtil.lib for 
      cancellation (pfnCancel and pCancelData)

See Also:
   None

======================================================================= 

Function: AEECallback_SocketWait()

Description: 
   Schedule a callback for some normal PFNNOTIFY-type socket event.

Prototype:

   void AEECallback_SocketWait(AEECallback *pcb, ISocket *piSock, 
                               void (*pfn)(ISocket *, PFNNOTIFY, void *));

Parameters:
   pcb: AEECallback to use to schedule/cancel op

   piSock: BREW socket on which op is scheduled

   pfn:  void (*pfn)(ISocket *, PFNNOTIFY, void *) - the function to use to 
                     schedule/cancel op
   
Return Value:
   None

Comments:  
   pcb's pfnNotify and pNotifyData should be initialized before calling this 
      function.

   This function probably shouldn't be used directly;use 
      AEECallback_Readable/AEECallback_Writeable, which are macro wrappers 
      of this function

Side Effects: 
   pcb is populated with a function inside AEECallbackUtil.lib for 
      cancellation (pfnCancel and pCancelData).

See Also:
   None

======================================================================= 

Function: AEECallback_Readable()

Description: 
   Schedule a callback for socket readable

Prototype:

   void AEECallback_Readable(AEECallback *pcb, ISocket *piSock)

Parameters:
   pcb: AEECallback to use to schedule/cancel op
   piSock: BREW Socket on which op is scheduled
   
Return Value:
   None

Comments:  
   pcb's pfnNotify and pNotifyData should be initialized before calling this 
      function.

Side Effects: 
   pcb is populated with a function inside AEECallbackUtil.lib for 
      cancellation (pfnCancel and pCancelData).

See Also:
   None

======================================================================= 

Function: AEECallback_Writeable()

Description: 
   Schedule a callback for socket writeable

Prototype:

   void AEECallback_Writeable(AEECallback *pcb, ISocket *piSock)

Parameters:
   pcb: AEECallback to use to schedule/cancel op
   piSock: BREW socket on which op is scheduled
   
Return Value:
   None

Comments:  
   pcb's pfnNotify and pNotifyData should initialized before calling this 
      function.

Side Effects: 
   pcb is populated with a function inside AEECallbackUtil.lib for 
      cancellation (pfnCancel and pCancelData).

See Also:
   None

======================================================================= 

Function: AEECallback_Connect()

Description: 
   Schedule a callback for socket connect

Prototype:

    void AEECallback_Connect(AEECallback *pcb, ISocket *piSock, 
                             INAddr a, INPort p, int *pnError);


Parameters:
   pcb: AEECallback to use to schedule/cancel op
   piSock: BREW Socket on which op is scheduled
   a: IP address to connect to, in *network byte order*
   p: port to connect to, in *network byte order*
   pnError: pointer to return value, is set to connect error
   
Return Value:
   None

Comments:  
   pcb's pfnNotify and pNotifyData should be initialized before calling this 
      function.

Side Effects: 
   pcb is populated with a function inside AEECallbackUtil.lib for 
      cancellation (pfnCancel and pCancelData).

See Also:
   None

======================================================================= 

*/   


#endif /* #ifndef AEECALLBACKUTIL_H */
