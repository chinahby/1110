/*======================================================
FILE:  OEMNotify.h

SERVICES:   OEM notify interface

GENERAL DESCRIPTION:

Notify related definitions for the OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEM_NOTIFY_H)
#define OEM_NOTIFY_H  1

#include "AEE.h"

#if defined(__cplusplus)
extern "C" {
#endif

// OEM Notifications - These notifications are made by BREW to the OEM layer.  They
// replace some of the former functions (OEM_AppCanStart, etc.)

typedef enum { OEMNTF_APP_START,             // Can app be started? (dwParam == AEECLSID) (formerly OEM_AppCanStart)
               OEMNTF_ACTIVATE,              // dwParam == OEMAppState *
               OEMNTF_IDLE,                  // dwParam == OEMAppState *
               OEMNTF_RESET,                 // BREW wants the OEM to reset the device
               OEMNTF_CLOSE_FILE,            // BREW wants the OEM to close a file (dwParam == file name)
               OEMNTF_SHOW_CALL_DIALOGS,     // BREW is displaying call status UI - (formerly OEM_SetSystemAlertMask)
               OEMNTF_GET_CONTEXT,           // dwParam == OEMAppState *
               OEMNTF_APP_EVENT,             // dwParam == OEMAppEvent *
               OEMNTF_EXT_STOP,              // dwParam == 0 terminated list of AEECLSIDs of extension
               OEMNTF_SYS_ERROR,             // dwParam == AEESysError *
               OEMNTF_CREATEINSTANCE,        // dwParam == OEMNotifyCreate *
               OEMNTF_MODLOAD,               // dwParam == OEMNotifyModLoad * 
               OEMNTF_MODUNLOAD,             // dwParam == OEMNotifyModLoad * 
               OEMNTF_NEXT_ALARM,            // dwParam == MilliSeconds before next alarm
               OEMNTF_RESET_BREW,            // Request to reset BREW. 
               OEMNTF_APP_STARTING,          // App Starting/Resuming/Suspending/Stopping dwParam == OEMAppStart *
               OEMNTF_APP_CTXT_NEW,          // New application context created, dwParam == OEMAppCtxNtfy *
               OEMNTF_APP_CTXT_SWITCH,       // Application context switch, dwParam == OEMAppCtxNtfySwch *
               OEMNTF_APP_CTXT_DELETE        // Application Context deleted, dwParam == OEMAppCtxNtfy *
             } OEMNotifyEvent;

extern int              OEM_Notify(OEMNotifyEvent evt, uint32 dw);

typedef struct _OEMAppState
{
   AEECLSID cls;        // Class starting or being closed.
   uint32   dwOEMCtx;   // See below.
} OEMAppState;

// OEMAppCtxNtfy::cls - class whose OEM app context is being passed in
// OEMAppCtxNtfy::dwOEMCtx.

typedef struct _OEMAppCtxNtfy
{
   AEECLSID cls;             // Class whose OEMAPPCONTEXT is passed.
   void**   ppOEMCtx;   // OEMAPPCONTEXT pointer for class ID passed in cls.
                             // The OEMAPPCONTEXT is stored in ACONTEXT.
} OEMAppCtxNtfy;

typedef struct _OEMAppCtxNtfySwch
{
   OEMAppCtxNtfy lastOemAppCtx; // oem context info for app context switched out
   OEMAppCtxNtfy oemAppCtx;     // oem context info for app context switched in
   
} OEMAppCtxNtfySwch;

typedef struct _OEMAppEvent
{
   AEECLSID cls;
   AEEEvent evt;
   uint16   wp;
   uint32   dwp;
   boolean        bRet;
} OEMAppEvent;

typedef struct _OEMNotifyCreate
{
   AEECLSID cls;  // class ID
   void    *pif;  // pointer to interface
} OEMNotifyCreate;

typedef struct _OEMNotifyModLoad
{
   const char *cpszFile;  // name of module file, in fs:/ terms
   void       *pLocation; // where the code image landed (might be a const file)
   uint32      dwSize;    //Size of the module loaded
} OEMNotifyModLoad;


#define OEMAS_START_TO_TOPVISIBLE      (0)
#define OEMAS_MOVE_TO_TOPVISIBLE       (1)
#define OEMAS_MOVE_FROM_TOPVISIBLE     (2)
#define OEMAS_CLOSE_FROM_TOPVISIBLE    (3)
#define OEMAS_CLOSE_APPLET             (4)

typedef struct _OEMAppStart {
   uint32         nSize;   // Size of me
   uint32         nType;   // OEMAS_ move type
   AEECLSID       cls;     // Class that is relocating
} OEMAppStart;

#if defined(__cplusplus)
}
#endif


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
OEMAS_

OEMAS_XXXXX is the format for App Start/Stop OEM Notifications to OEM_Notify with 
OEMNTF_APP_STARTING
=======================================================================

OEM Notifications 

Description:
This section describes the conditions of each OEMAppStart::nType when OEM_Notify is
called with OEMNTF_APP_STARTING. OEM Notifications for asynchronous start/stop/move are included.


Definition:
uint32

Members:

The following Items are supported:

~
   Name:
   OEMAS_START_TO_TOPVISIBLE
~
   Description:
   This is given when an applet is requested to be started with any method as a top visible 
   applet. The notification will occur after the start request callback is queued.
===pre>
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);
===/pre>

   Parameters:
      evt: Specifies OEMNTF_APP_STARTING
      dwParam: Pointer to a OEMAppStart structure
*

~
   Name:
   OEMAS_MOVE_TO_TOPVISIBLE
~
   Description:
   This is given when an applet is requested to be moved to top visible 
   applet. The notification will occur after the move request callback is queued.
   OEMAS_MOVE_TO_TOPVISIBLE is sent only when an applet is requested to be moved 
   to top visible due to calls to IAppHistory interfaces. It is not sent when a 
   suspended application gets resumed.

===pre>
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);
===/pre>

   Parameters:
      evt: Specifies OEMNTF_APP_STARTING
      dwParam: Pointer to a OEMAppStart structure
*

~
   Name:
   OEMAS_MOVE_FROM_TOPVISIBLE
~
   Description:
   This is given when an applet is requested to be moved from top visible 
   applet. The notification will occur after the move request callback is queued.
   OEMAS_MOVE_FROM_TOPVISIBLE is sent only if the top visible applet is moved due 
   to calls to IAppHistory interfaces. It is not sent when an applet gets suspended 
   because another applet is getting started.

===pre>
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);
===/pre>

   Parameters:
      evt: Specifies OEMNTF_APP_STARTING
      dwParam: Pointer to a OEMAppStart structure
*

~
   Name:
   OEMAS_CLOSE_FROM_TOPVISIBLE
~
   Description:
   This is given when a topvisible applet or all applets are requested to be closed. The 
   notification will occur after the close request callback is queued.
===pre>
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);
===/pre>

   Parameters:
      evt: Specifies OEMNTF_APP_STARTING
      dwParam: Pointer to a OEMAppStart structure

   Comments:
      When a CloseAll is occuring and there are apps to close, OEMAS_CLOSE_FROM_TOPVISIBLE 
      will be called whether or not there is an executing top visible at the time. The 
      OEMAppStart::cls will be set to 0 in this case. This will be called in this
      way when the first queuing is performed

*

~
   Name:
   OEMAS_CLOSE_APPLET
~
   Description:
   This is given when an applet is requested to be closed. The notification will 
   occur after the close request callback is queued.
===pre>
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);
===/pre>

   Parameters:
      evt: Specifies OEMNTF_APP_STARTING
      dwParam: Pointer to a OEMAppStart structure

=======================================================================
OEMAppStart

Description:
This structure is passed to OEM_Notify() for OEMNTF_APP_STARTING.

Definition:
typedef struct _OEMAppStart
{
   uint32   nSize;
   uint32   nType;
   AEECLSID cls;
} OEMAppStart;

Members:
   nSize:      Size of this structure
   nType:      OEMAS_* type of notification
   cls:        AEECLSID involved in the notification

Comments:
   None

See Also:
   OEM_Notify()
   
==================================================================
OEMAppCtxNtfy

Description:
This structure is passed to OEM_Notify() for OEMNTF_APP_CTXT_NEW and
OEMNTF_APP_CTXT_DELETE notifications.

Definition:
typedef struct _OEMAppStart
{
AEECLSID cls;
void**   ppOEMCtx;
} OEMAppCtxNtfy;

Members:
cls:        AEECLSID involved in the notification (context for which is 
            created (OEMNTF_APP_CTXT_NEW) or deleted (OEMNTF_APP_CTXT_DELETE))
ppOEMCtx:   pointer to the OEMAPPCONTEXT associated with this application context.

Comments:
None

See Also:
OEM_Notify()

==================================================================
OEMAppCtxNtfysSwch

Description:
This structure is passed to OEM_Notify() for OEMNTF_APP_CTXT_SWITCH
notification.

Definition:
typedef struct _OEMAppCtxNtfySwch
{
OEMAppCtxNtfy lastOemAppCtx; // oem context info for app context switched out
OEMAppCtxNtfy oemAppCtx;     // oem context info for app context switched in

} OEMAppCtxNtfySwch;

Members:
lastOemAppCtx: contains class ID and OEMAPPCONTEXT pointer for the application
               contexted switched out just before the current running application.
oemAppCtx:     contains class ID and OEMAPPCONTEXT pointer for the application
               currently running.

Comments:
None

See Also:
OEM_Notify()

==================================================================
OEM DOCUMENTATION
=======================================================================

Function: OEM_Notify

Description:
   This function is called by BREW to notify the OEM regarding
critical system functions.  These include:

OEMNTF_APP_START - A query to determine if the specified app can
be started.  The class ID of the app is specified in the dwParam.

OEMNTF_ACTIVATE - This query is performed when BREW starts an applet
when either application history is empty or OEM has called AEE_Suspend
on BREW.

OEMNTF_IDLE - This notification is sent to the OEM when BREW is 
closing an applet that is returning control to the OEM's native UI.  
In this case, the OEMAppState->dwOEMCtx value contains the value 
passed to BREW via the OEMNTF_GET_CONTEXT call when the app was 
started.

OEMNTF_RESET - A request by BREW to reset the device.

OEMNTF_CLOSE_FILE - This notification is not used.

OEMNTF_SHOW_CALL_DIALOGS - Indicates whether the OEM should show
call dialogs or not.  If dwParam is TRUE, then the OEM should show
these dialogs.  If FALSE, they should be inhibited.  This call is
usually made only by the network layer to inhibit/re-enable display
of call status dialogs during PPP sessions.

OEMNTF_GET_CONTEXT - This call is made by BREW when the OEM starts 
a BREW app directly to retrieve the OEMAppStart->dwOEMCxt.

OEMNTF_APP_EVENT - Sent soon after sending any event in the
range 0 - EVT_APP_LAST_EVENT to the apps.  This allows the OEM to 
monitor events sent to applications and the return values from the 
apps for each event.

OEMNTF_EXT_STOP - This notification is sent to notify OEM layer about
extensions affected by download. dwParam is zero terminated list of 
AEECLSIDs of the affected extensions. This notification is sent when
BREW is about to upgrade extensions from the ADS catalog. If OEM layer
is using any of the extentions in the list, it must release it in order 
for upgrade to finish successfully. Applications are sent similar
notification with EVT_EXT_STOP.

OEMNTF_SYS_ERROR - Sent when a system error occurs. dwParam is a 
pointer to structure of type AEESysError

OEMNTF_CREATEINSTANCE - This notification is not used.

OEMNTF_MODLOAD - This notification is sent when a module is loaded.               
dwParam points to OEMNotifyModLoad type structure specifying module
details.

OEMNTF_MODUNLOAD - This notification is sent when a module is unloaded.               
dwParam points to OEMNotifyModLoad type structure specifying module
details.

OEMNTF_NEXT_ALARM - This function is invoked to notify the OEM layer 
about the duration pending before the next BREW Alarm can expire. 
The dwParam contains the exact duration in milliseconds before the
next BREW alarm can expire. When there are no BREW alarms pending, 
the dwParam is set to 0. If there are two or more BREW alarms pending, 
the dwParam always points to the shortest of the BREW Alarms that are 
pending. BREW alarms are persistent (i.e. remembered across power-cycles), 
but it requires the device to be powered up for the alarm to expire. 
If the alarm expired while the device is powered down, the app that set 
the alarm will be notified only when the device is powered up next time. 
On devices that support alarms in the RTC (Real Time Clock), this 
notification (OEMNTF_NEXT_ALARM) can be used by the OEMs to set the next 
BREW Alarm in the RTC. When the RTC alarm expires, if the device is 
currently powered down, the device should be powered up by the OEM. 
Powering up the device and initializing BREW (AEE_Init) will take care 
of expiring the BREW Alarm and notifying the app that set the alarm. 
If the device is already powered up when the RTC alarm expires, 
no action needs to be taken from the OEM Layer. In this case, the BREW 
alarm will be notified automatically to the app that set the alarm.

OEMNTF_RESET_BREW - This indicates a request to Reset BREW on the 
device. The entire device must not be reset. Instead, just BREW must 
be reset. Resetting BREW involves exiting BREW (AEE_Exit) and 
re-initializing BREW (AEE_Init).

OEMNTF_APP_STARTING - This notification indicates that an app is 
asynchronously changing its state with certain close/start/movements. 
The OEMAS_ given as OEMAppStart::nType defines the exact action 
that is occurring with respect to the applet given in the 
OEMAppStart::cls and its requested relationship to top visible. 
Since the request is given after the callback is queued you must 
override the behavior in a different manner: ResArbiter or 
OEM_Notify(OEMNTF_APP_START). The internal app state may be changed to
a transitionary state when this notification is given. That is when a 
top visible app is closing it may be set to a closing state at the 
time of this function.

OEMNTF_APP_CTXT_NEW - This notification indicates that a new application 
context has been created. The OEMAPPCONTEXT pointer passed can be used to 
assign an OEM's application context to OEMAPPCONTEXT member in the 
application context.

OEMNTF_APP_CTXT_SWITCH - This notification indicates that an application 
context switch has taken place. The notification  is called after the switch 
and has OEMAPPCONTEXT pointer and class ID information for the current app 
and the app context switched out.

OEMNTF_APP_CTXT_DELETE - This notification indicates that the application 
context is being deleted. It is OEM's responsibility to free the OEM app 
context for this app. The OEMAPPCONTEXT pointer is passed in this notification 
which can be used to free the OEM context.

Prototype:
   int   OEM_Notify(OEMNotifyEvent evt, uint32 dwParam);

Parameters:
   evt:     The event code.
   dwParam: Context sensitive data.

Return Value:
   0 - SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None

==================================================================*/
#endif // !defined(OEM_NOTIFY_H)
