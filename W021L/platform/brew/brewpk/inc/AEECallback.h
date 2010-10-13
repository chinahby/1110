#ifndef AEECALLBACK_H
#define AEECALLBACK_H
/*
=======================================================================

FILE:        AEECallback.h

SERVICES:    AEECallback

DESCRIPTION: AEECallback definitions, typedefs, etc.

=======================================================================
        Copyright © 2004-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=======================================================================
*/

typedef struct _AEECallback AEECallback;

typedef void CallbackCancelFunc(AEECallback* pcb);

typedef void CallbackNotifyFunc(void* pUser);

struct _AEECallback
{
   AEECallback*        pNext;            // RESERVED
   void*               pmc;              // RESERVED
   CallbackCancelFunc* pfnCancel;        // Filled by callback handler
   void*               pCancelData;      // Filled by callback handler
   CallbackNotifyFunc* pfnNotify;         // Filled by caller
   void*               pNotifyData;      // Filled by caller
   void*               pReserved;        // RESERVED - Used by handler
};


#define CALLBACK_Init(pcb,pcf,pcx) \
   do { \
      (pcb)->pfnNotify = (CallbackNotifyFunc*)(pcf); \
      (pcb)->pNotifyData = (pcx); \
   } while (0);

#define CALLBACK_Cancel(pcb) \
   do { \
      CallbackCancelFunc* pfnCancel = (pcb)->pfnCancel; \
      if (0 != pfnCancel) { \
         pfnCancel(pcb); \
      } \
   } while (0)

#define CALLBACK_IsQueued(pcb)  (0 != (pcb)->pfnCancel)

/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

AEECallback

Description:

   AEECallback structures establish a generalized mechanism for scheduling
   asynchronous callbacks.

   When an AEECallback is scheduled, the caller reqeuests some object -- the
   scheduling object -- that the callback be called at some time in the
   future, perhaps tied to the occurrence of some event.

   AEECallbacks contain the following information:

   - Closure: this consists of a function pointer to be called, and a context
     pointer to be passed to the function.  This is supplied by the owner of
     the AEECallback when it is constructed using CALLBACK_Init().

   - Cancellation data: this information allows the owner of an AEECallback to
     cancel the callback -- prevent it from being called -- at any time.
     Cancelation does not require knowledge of which scheduling object
     currently holds the AEECallback -- this information is embodied in the
     AEECallback itself.

   - Queueing data: some fields are reserved for use by scheduling objects
     to keep track of the AEECallback and what it is waiting on.

Definition:
   typedef struct _AEECallback AEECallback;
   struct _AEECallback
   {
      AEECallback*        pNext;
      void*               pmc;
      CallbackCancelFunc* pfnCancel;
      void*               pCancelData;
      CallbackNotifyFunc* pfnNotify;
      void*               pNotifyData;
      void*               pReserved;
   };

Members:

  pNext: Reserved and the caller should not modify this member, used by the 
         callback handler (callee)

  pmc: Reserved and the caller should not modify this member, used by the 
         callback handler (callee)

  pfnCancel: Pointer to function called by the callback handler if this
             callback is cancelled. The caller should set this pointer to NULL.

  pCancelData: Data passed to pfnCancel. The caller should not modify
                this member.

  pfnNotify: This is the callback function that is invoked by the callee. The
           caller must set this pointer in order to be called back when the
           event of interest has occurred.

  pNotifyData: Data to be passed to pfnNotify, the caller must set this
          pointer to the data that must be passed to the pfnNotify function.

  pReserved: Reserved and this member will be used by the callback handler


Comments:

  Adhering to the following typical usage will ensure trouble-free AEECallback
  usage:

  - An object that requires asynchronous notification of events will contain
    one AEECallback for each type of notification it can independently wait on.

  - When the object is created, each of its AEECallbacks are initialized.  For
    a context pointer they will use the object instance, and for the callback
    function they will use an event-specific function.

  - When the object is deleted, it must cancel all of its AEECallbacks using
    CALLBACK_Cancel().  Canceling an un-scheduled callback is harmless.  If
    a callback were to be called after the object is deleted, it would
    de-reference a dangling pointer (because the object instance itself is the
    context).

See Also:
   None

=======================================================================

    CallbackCancelFunc

Description:
    This data structure specifies the prototype of the Cancel Function that can
be used to cancel a callback

Definition:
    typedef void CallbackCancelFunc(AEECallback* pcb);

Members:
    pcb : Pointer to the AEECallback that must be cancelled using this function

Comments:
    This is used in conjunction with AEECallback

See Also:
    AEECallback

=============================================================================
CallbackNotifyFunc

Description:
    CallbackNotifyFunc specifies a data type which is a function pointer 
    to a function used to dispatch an AEECallback

Definition:
    typedef void CallbackNotifyFunc(void* pData);

Members:
    None

Comments:
    None

See Also:
    None

=======================================================================
*/

/*
=======================================================================
MACROS DOCUMENTATION
=======================================================================

CALLBACK_Init()

Description:
  Set up internal members of an AEECallback for scheduling, sets the
  notify function and the notify context

Definition:

  CALLBACK_Init(pcb,pcf,pcx) \
     do { \
        (pcb)->pfnNotify = (CallbackNotifyFunc*)(pcf); \
        (pcb)->pNotifyData = (pcx); \
     } while (0);

Parameters:
  pcb: pointer to the AEECallback to set up

  pcf: notify function

  pcx: notify context

Evaluation Value:
   None

Comments:
   pcf(pcx) is called when the AEECallback comes due (either via Resume
   or other scheduling operation)

Side Effects:
   None


   

See Also:
   None

=======================================================================

CALLBACK_Cancel()

Description:
  fire an AEECallback's cancel function, if any.

Definition:

   CALLBACK_Cancel(pcb)    if (0 != (pcb)->pfnCancel) (pcb)->pfnCancel(pcb)

Parameters:
   pcb: pointer to the AEECallback to set up

Evaluation Value:
   None

Comments:
   Setting up the cancel function is the responsibility of the called
   scheduling API.  E.g.: the ISHELL_Resume cancel function is likely
   something that removes the callback from a list of pending notifies

   Similarly, clearing the cancel function during a cancel callback or
   before calling the notify function is* also* the responsibility of
   the called scheduling API. E.g.: before ISHELL_Resume fires the
   notify function, the AEECallback is taken out of a list of pending
   notifies, and so the cancel function in the callback must be set to
   0.

Side Effects:
   None


   

See Also:
   None

=======================================================================

CALLBACK_IsQueued()

Description:
   deduce whether an AEECallback is pending, scheduled

Definition:

   CALLBACK_IsQueued(pcb) (0 != (pcb)->pfnCancel)


Parameters:
  pcb: pointer to the AEECallback to set up

Evaluation Value:
   boolean TRUE if the AEECallback has a cancel function, is on
          somebody's scheduling, pending list

Comments:
   This is not an exhaustive test and is only a convenience for called
   scheduling APIs that use the cancel function (should be all of them,
   but callers must not assume so).

Side Effects:
   None


   

See Also:
   None

===========================================================================
*/

#endif // AEECALLBACK_H
