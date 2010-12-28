/*=============================================================================
FILE:  OEMRats.c

SERVICES:  OEM functions for supporting BREW Remote API Test Suite

GENERAL DESCRIPTION:
      The interface is created by a BREW interface.  Until a BREW interface
      creates the first object, no callbacks are registered with the
      underlying DMSS code.  If the underlying DMSS interface is not
      compiled in, the creation of the object will fail.  This is one
      method by which an application can determine whether the remote API
      test is supported or not.

      This file contains routines divided into the following different areas.
      * Applications calls.
      * DMSS API Callbacks.
      * BREW necessitated functions.

      The Application calls are the functions called in the BREW Application
      context and implement the API described above.  These functions maintain
      the necessary status information and perform the necessary BREW and DMSS
      calls.

      The DMSS API callbacks provide the results of various commands or the
      responses from the server when it is running.  These calls run in the
      RPC task’s context and must therefore create a copy of the results and
      pass this to the BREW context.  Any work which is RPC thread safe can
      be performed in this context, but is limited to checking the results of
      the command, enqueuing the response to the brew callback, and scheduling
      the response to be processed.

      BREW necessitated functions are used to receive the results from the
      DMSS API Callbacks, and process these in the BREW context.  In this
      context, the results are written to the file (if appropriate) and the
      correct BREW Notifications are sent to the applications.

      The object will contain private data to keep track of the number of
      references to it, the pointer to the shell, the pointer to the DMSS
      client object.  The cached client and server status, and the notifier
      mask of registered clients.  When data needs to be sent from the RPC
      task context up to the BREW context, a queue is used.  Each item in
      the queue contains the message to be displayed, the notification mask
      to be sent, along with the brew AEECallback structure and a pointer for
      the linked list.

      The object also utilizes global variables to maintain the integrity of
      the object.  First, local_rat is a pointer to the allocated object. 
      If NULL, no object is currently allocated.  
      OEMRAT_q_first and OEMRAT_q_last are used to enqueue and dequeue
      messages into our local queue.  And OEMRats_crit_sect is used to
      maintain the integrity of that queue.

PUBLIC CLASSES AND STATIC FUNCTIONS:

      OEMRATS_AddRef:  This function simply increments the reference count
                       returning the new reference count. 
      OEMRATS_Release: This function decrements the reference count.  
                       If the reference count reaches 0, the object will be
                       freed providing that the server is not currently
                       running.  If the server is running, the object will
                       remain in tact to handle any incoming messages.
                       It always returns the reference count. 
      OEMRATS_SetMask: This function is part of the INotifier interface. 
                       It is used called by BREW to inform this object when
                       some application registers for a notification.
                       The notifications masks are or’ed together and the
                       results are saved in the object.
      OEMRATS_GetServerStatus:
                       This function returns the cached Server status.
      OEMRATS_SetServerStatus:
                       This function sets the server status. 
      OEMRATS_SetClientStatus:
                       This function will attempt to set the client status.
      OEMRATS_GetRemoteIP:
                       This function returns the remote IP as was saved in the
                       DMSS layer.
      OEMRATS_GetLocalIP:
                       This function returns the local IP as was saved in the
                       DMSS layer.
      OEMRATS_SetRemoteIP
                       The Remote IP address can be set at any time, and
                       corresponds to the address to be used on the next
                       connection.
      OEMRATS_GetFile: This function attempts to open the results file in
                       read only mode.
      OEMRATS_RunTest: This function runs the specified test.

      OEMRats_New:     This function creates a new object as a result of the
                       ISHELL_CreateInstance being called.

Private Interfacing functions:
      OEMRats_enqueue:  Takes an item and places it in a queue.
      OEMRats_dequeue:  Retrieves an item from the queue.
      OEMRats_SendInfo: Interfaces to the SendInfoCb to tell Brew context
                        what to put into the file and what notifications to
                        send.
      OEMRats_SendInfoCb:
                        Always runs in the brew context.  Appends the message
                        if present to the FILE and sends the notifications if
                        anyone is registered for them.
                      
Private DMSS API Callbacks:
      OEMRats_TestCb:   This function is called by the RPC task after a test
                        is started.
      OEMRats_StartServerCb:
                        This function is called by the RPC task after the
                        Server is started. 
      OEMRats_StartClientCb:
                        This function is called by the RPC task after the
                        client is started.
      OEMRats_TextCb:   This function is called by the RPC whenever the
                        server has a message.


        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/05    at     Added informative log messages.
09/10/04    vp     Replaced the inclusion of psglobal.h with ps_in.h
03/31/04    at     Initial release.
=============================================================================*/

#include "comdef.h"
#include "customer.h"
#include "BREWVersion.h"

#include "err.h"
#include "AEE.h"
#include "AEERats.h"
#include "AEEShell.h"
#include "AEEFile.h"
#include "AEE_OEM.h"

#include "OEMHeap.h"

#ifdef FEATURE_STA_RAT
#error code not present
#endif


/*===========================================================================

FUNCTION: OEMRats_New

DESCRIPTION
  This function is called as a result of the ISHELL_CreateInstance
  being called.  This function exists regardless of the state of
  FEATURE_STA_RAT.  If the function is called when FEATURE_STA_RAT is
  not enabled, we return EUNSUPPORTED and do not allocate any memory.
  If the features indicate we are to allocate the object, we first
  check to see if we already have one.  If the object already exists,
  we increment the reference count and return the existing object.
  All applications can share the one object.  If the object has to be
  created new, we allocate the memory for it and initialize it to 0’s.
  Next we save the pointer to the vector table, pointer to the Shell,
  and initialize our status as down.  Finally, it tells the DMSS layer
  to call OEMRats_TextCb if the server ever has anything it wishes to say.

DEPENDENCIES
  none

PARAMETERS
  pIShell:  Shell pointer used to create this object.
  cls:      ClassID of the object being created.
  ppif:     Pointer to the interface once created.

RETURN VALUE
  EBADPARM: Parameters not legal.
  EUNSUPPORTED: The class they are creating is not currently available.
  ENOMEMORY: Not enough memory to allocate the object.
  AEE_SUCCESS: All went well

SIDE EFFECTS
  None

===========================================================================*/
int OEMRats_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
  if (!pIShell || !ppif)
    return EBADPARM;

  if (cls != AEECLSID_RATS)
    return EUNSUPPORTED;

#ifdef FEATURE_STA_RAT
#error code not present
#else
  return EUNSUPPORTED;
#endif
}
