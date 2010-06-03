/*======================================================
FILE:  AEE_OEMDispatch.h

SERVICES:  AEE OEM dispatch coordination

GENERAL DESCRIPTION:

AEE dispatch functions for use by the OEM

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_DISPATCH_H)
#define AEE_OEM_DISPATCH_H  1

#include "AEE_OEMComdef.h"
#include "AEEShell.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// Not supported unless in debug mode.
//

#if defined(BREW_COMMERCIAL_BUILD)
#if defined(AEE_DBG_SYSOBJ)
#undef AEE_DBG_SYSOBJ
#endif
#define AEE_DBG_SYSOBJ  0
#endif

//
//  AEE Required Entry Points
//
//  These routines are provided to the OEM by AEE.  They must be called for the AEE
//  to function correctly.  See documentation below.
//

extern AEE_EXPORTS IShell *            AEE_Init(uint32 dwUnused);
extern AEE_EXPORTS void                AEE_Exit(void);
extern AEE_EXPORTS uint32              AEE_Dispatch(void);

//
// The following mask is returned by AEE_Dispatch.  OEMs should note that the
// flags are a good way to detect whether BREW is in a state where the processor
// or OS can be put into a sleep state.  A return value of 0 means that
// BREW has no high-priority callbacks or timers pending.
//

#define AEE_DISPATCH_TIMERS_PENDING    0x1      // BREW has active short-term timers pending
#define AEE_DISPATCH_CALLBACKS_PENDING 0x2      // BREW has active callbacks pending
#define AEE_DISPATCH_THROTTLING        0x4      // BREW is consuming too much time - throttling

//
// This routine is provided for the OEM's use in developing custom objects that
// require inter-thread notifications.  This function posts the AEECallback into the
// BREW system resume queue.
//
// This is the same mechanism used by the BREW standard SoundPlayer interface.  In
// that case, a callback is issued from another thread, the resume callback is posted
// and the API is then called-back in the UI thread's context.
//
// A NULL ACONTEXT pointer tells the BREW layer to associate the callback with the system rather than
// the currently active application.  This is done to support callbacks that may need to be called when
// apps are not running or across applications.
//

extern AEE_EXPORTS void AEE_ResumeCallback(AEECallback * pcb, ACONTEXT *);

#define AEE_APP_RESUME(pcb,pa)      AEE_ResumeCallback(pcb,pa)
#define AEE_SYS_RESUME(pcb)         AEE_ResumeCallback(pcb,0)

extern AEE_EXPORTS int AEE_SetTimer(int32 nMSecs, PFNNOTIFY pfn, void * pUser);
extern AEE_EXPORTS int AEE_SetSysTimer(int32 nMSecs, PFNNOTIFY pfn, void * pUser);
extern AEE_EXPORTS int AEE_SetTimerCallback(int32 nMSecs, AEECallback * pcb, ACONTEXT* pac);
extern AEE_EXPORTS int AEE_CancelTimer(PFNNOTIFY pfn, void * pUser);

#define AEE_SetSysTimerCallback(t, pcb) AEE_SetTimerCallback((t),(pcb), 0);

//
// OEM routines to access system callbacks.  See AEEShell.h for type definitions
//

extern AEE_EXPORTS void                AEE_RegisterSystemCallback(AEECallback * pcb, int nType, ACONTEXT * pac);
extern AEE_EXPORTS boolean             AEE_IssueSystemCallback(int nType);

extern AEE_EXPORTS AEECallback *       AEE_LinkSysCallback(AEECallback *pcbApp);
extern AEE_EXPORTS AEECallback *       AEE_LinkSysCallbackEx(AEECallback *pcbApp, int nExtra);
extern AEE_EXPORTS void                AEE_LinkSysObject(AEECallback *pcb);

//
// yet another way to get a callback at app exit...
//
// these functions allow one to set and get an app-specific "global",
//  where a module needs to keep some memory around on the behalf of
//  an application and have the memory freed at app exit, for example,
//  the application's default IDisplay...
//
extern AEE_EXPORTS int AEE_SetAppGlobal(const char *cpszKey, const void *cpGlobal, AEECallback *pcbCleanup);
extern AEE_EXPORTS int AEE_GetAppGlobal(const char *cpszKey, void **cpGlobal);

#if defined(__cplusplus)
}
#endif

/*=======================================================
   AEE DOCUMENTATION
=========================================================

Function:  AEE_Init()

Description:
   This function initializes the AEE.  During initialization, the AEE performs
   the following tasks:

- Builds the list of loaded modules (static and dynamic).
- Initializes a timer if an alarm has been set.
- Initializes any pending notification objects.
- Initializes the SMS layer if available.

The AEE passes this signal to the AEE_Init() function when the
AEE_Dispatch() function must be called.  The OEM layer must call the
AEE_Dispatch() function during user interface (UI) initialization before
making any other AEE calls.


Prototype:

   IShell * AEE_Init(uint32 dwUnused)

Parameters:
   dwUnused:  unused

Return Value:
   Pointer to the AEE shell object

Comments:
   None

Side Effects:
   None

See Also:
   AEE_Exit()

============================================================================
Function:  AEE_Exit()

Description:
   This function closes the AEE.  It must be called to effectively close the
   BREW layers.  The function performs the following tasks:

- Unloads any active applications or modules.
- Frees any memory used by the AEE.
- Releases any open files used by the AEE.
- Releases the SMS layer if the AEE had opened it.

This function MUST be called by the OEM layer to close the AEE.

Prototype:

   void AEE_Exit(void)

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEE_Init()

=======================================================================
Function:  AEE_Dispatch()

Description:
   This function must be called by the OEM layer whenever the AEE signal
   has been detected within the thread where the AEE_Init() call was made.
   This function performs the following tasks:

- Checks the status of any pending BREW Resume function and calls it.
- Checks to see if any BREW-related timers have expired.

Prototype:

   uint32 AEE_Dispatch(void)

Parameters:
   None

Return Value:
    AEE_DISPATCH_TIMERS_PENDING if BREW has active short-term timers pending
    AEE_DISPATCH_CALLBACKS_PENDING if BREW has active callbacks pending
    AEE_DISPATCH_THROTTLING if BREW is consuming too much time - throttling
    0 (zero) if BREW has no high-priority callbacks or timers pending.

Comments:
   None

Side Effects:
   None

See Also:
   AEE_Init()
   AEE_Exit()

=======================================================================
Function:  AEE_ResumeCallback()

Description:
   This routine is provided for the OEM's use in developing custom objects that
   require inter-thread notifications.  This function posts the AEECallback into the
   BREW system resume queue.

   This is the same mechanism used by the BREW standard SoundPlayer interface.  In
   that case, a callback is issued from another thread, the resume callback is posted,
   and the API is then called back in the UI thread's context.

Prototype:
   void AEE_ResumeCallback(AEECallback * pcb, void *pa);

Parameters:
   pcb:    pointer to the callback
   pa:     pointer to the application context.  A NULL pointer tells
           the BREW layer to associate the callback with the system
           rather than the currently active application.  This is done
           to support callbacks that may need to be called when applications
           are not running or across applications.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEE_Resume()

=======================================================================
Function:  AEE_SYS_RESUME()

Description:
   This routine is provided for the OEM's use in developing custom objects that
   require inter-thread notifications.  This function posts the AEECallback into the
   BREW supervisor mode resume queue.

Prototype:
   void AEE_SYS_RESUME(AEECallback * pcb);

Parameters:
   pcb:    pointer to the callback

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEE_Resume()

=======================================================================
Function:  AEE_SetSysTimer()

Description:
   This function allows the OEM layer to set a short-term timer. Upon
expiration, the specified callback function is called, passing it the
specified user data pointer as its first argument. Note the following:

- The timer will expire at Current Time + <Milliseconds specified>.

- Any normal processing can be done in the callback.  This includes
drawing to the screen, writing to files, and so forth.

- Timers do not repeat.  The OEMs must reset the timer if they desire a
repeating timer.

- Specifying the same callback/data pointers will automatically override
a pending timer with the same callback/data pointers.


Prototype:
   int AEE_SetSysTimer(int32 nMSecs, PFNNOTIFY pfn, void * pUser);

Parameters:
   nMSecs: timer expiration in milliseconds.  The expiration will occur at
           Current Time + dwMSecs.
   pfn:    the callback that will be called when the timer expires
   pUser:  the data pointer that will be passed as the only parameter to the
           callback

Return Value:
   EBADPARM - Invalid time or callback specified
   ENOMEMORY - Memory allocation fails
   0(zero) on SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function:  AEE_SetTimer

Description:
   This function allows the OEM layer to set a short-term timer in the 
current application context.  It is equivalent to ISHELL_SetTimer(). 
Upon expiration, the specified callback function is called, passing it 
the specified user data pointer as it's first argument. Note the 
following:

- The timer will expire at Current Time + <Milliseconds specified> 

- Any normal processing can be done in the callback.  This includes
drawing to the screen, writing to files, etc. 

- Timers do not repeat.  The OEMs must reset the timer if they desire a
repeating timer.

- Specifying the same callback/data pointers will automatically override 
a pending timer with the same callback/data pointers.


Prototype:
   int AEE_SetTimer(int32 nMSecs, PFNNOTIFY pfn, void * pUser);

Parameters:
   nMSecs: Timer expiration in milliseconds.  The expiration will occur at 
           Current Time + dwMSecs
   pfn:    The callback that will be called when the timer expires
   pUser:  The data pointer that will be passed as the only parameter to the
           callback.

Return Value:
   EBADPARM - Invalid time or callback specified
   ENOMEMORY - Memory allocation fails
   0(zero) on SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================

Function: AEE_LinkSysObject()

Description:

  This function associates an AEECallback with the currently running
  application, if any.  It's designed to help clean up "system" resources
  when an application exits.  Its intended use is for the AEECallback
  to be embedded in the implementing interface object.

  The AEECallback's completion function is called after the application
  has been shut down, but before all the memory for the application has
  been freed.

Prototype:
   void AEE_LinkSysObject(AEECallback *pcb)

Parameters:

  AEECallback *pcb: the callback to be dispatched on application exit

Return Value:
   None

Comments:
   None

Side Effects:
   None

==================================================================

Function: AEE_LinkSysCallback()

Description:

  This function calls AEE_LinkSysCallbackEx() with nExtra set to zero.
  See the documentation of AEE_LinkSysCallbackEx() for more details.

Prototype:
  AEECallback* AEE_LinkSysCallback(AEECallback* pcbApp)

Parameters:
  pcb:    the application callback to be linked into the system

Return Value:  
  AEECallback* : the linked system AEECallback, or NULL in case of error

See Also: 
   AEE_LinkSysCallbackEx()
   AEECallback

==================================================================

Function: AEE_LinkSysCallbackEx()

Description:

  This function wraps an application AEECallback inside a dynamically
  (over)allocated system AEECallback.  The system AEECallback should
  then be used in place of the original application AEECallback.  

  If the application exits before the system AEECallback is resumed,
  the application AEECallback will be cancelled.

  If the application AEECallback is cancelled, the system AEECallback
  will be cancelled first and then deallocated.

  If the system AEECallback is resumed (in system context) it will
  first cancel and then resume the application AEECallback in the
  associated application context.

Prototype:
  AEECallback*  AEE_LinkSysCallbackEx(AEECallback* pcbApp, int nExtra)

Parameters:
  pcb:    the application callback to be linked into the system
  nExtra: the amount by which to over allocate the system AEECallback

Return Value:
  AEECallback* : the linked system AEECallback, or NULL in case of error

Comments:
  The system AEECallback should be used _exactly_ as if it came from
  the application.  For example, by inserting it into a linked list,
  setting the pfnCancel and pCancelData members appropriately, and
  eventually calling AEE_SYS_RESUME() and/or CALLBACK_Cancel().

  The rules for accessing the any extra allocated memory are the same
  as those for AEECallbacks in general: after cancellation, the memory
  must not be accessed.

See Also:
   AEE_LinkSysCallback()
   AEECallback

=======================================================================*/

#endif // !defined(AEE_OEM_DISPATCH_H)

