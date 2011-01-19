#ifndef AEEDIAG_H
#define AEEDIAG_H

/*============================================================================
FILE:	AEEDiag.h

SERVICES:  BREW Diag Services

DESCRIPTION:
   This file defines IDiag interface that facilitates application diagnostics
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEDiag.h#8 $

============================================================================*/
/*==============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------
// Max size of an app version string
#define MAX_APP_VERSION_SIZE 10

// Max size of the clipboard entry for SendEvent diag
#define CLIPBOARD_NAME_MAX_LENGTH 15

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct IDiag IDiag;

/* The IDIAG_ERROR_CODE values are defined here so that
 * applications can populate response structure payloads
 * with these values.
 */
#define IDIAG_SUCCESS           0x00000000
#define IDIAG_BAD_CTL_ID        0x00000001
#define IDIAG_BAD_MENU_ITEM_ID  0x00000002
#define IDIAG_NO_ACTIVE_DLG     0x00000003
#define IDIAG_WRONG_DLG         0x00000004
#define IDIAG_DATA_UNAVAILABLE  0x00000005
#define IDIAG_NO_APP_SUPPORT    0x00000006  // App isn't registered
#define IDIAG_APP_NOT_ACTIVE    0x00000007  // App isn't the active app
#define IDIAG_DIAG_ERROR        0xE0000000
#define IDIAG_OTHER_INT_ERROR   0xF0000000

typedef uint32 IDIAG_ERROR_CODE;

/* The IDiagEvtReg structure is used to register events with
 * the IDiag object.  A diagnostic event should be registered
 * if it:
 *   1) Expects the IDiag object to store the request data for
 *     later retrieval by the app
 *   OR
 *   2) Will send delayed responses to the diagnostic request
 * Other events do not need to be exlicitly registered, as long
 * as the application is registered.
 */
typedef struct
{
    AEEEvent    EventId;
    boolean     UsesPayload;           // IDiag should store request payload
    boolean     UsesDelayedResponse;   // Event will yield delayed responses
} IDiagEvtReg;


//*****************************************************************************
//
// IDiag Interface
//
//*****************************************************************************

AEEINTERFACE(IDiag)
{

    INHERIT_IBase(IDiag);

    uint32 (*RegisterApp)(IDiag *pMe, AEECLSID AppId, AECHAR *AppVersion,
                          uint16 NumRegEvents,
                          IDiagEvtReg *RegEvents);
    void (*UnregisterApp)(IDiag *pMe, AEECLSID AppId);
    uint16 (*GetTransactionId)(IDiag *pMe, AEECLSID AppId, AEEEvent EventId,
                               uint16 wp, uint32 dwp);
    uint32 (*GetRequestPayload)(IDiag *pMe, uint16 XactId, uint16 PayloadSize,
                                void *Payload);
    uint32 (*SendDiagResponse)(IDiag *pMe, uint32 Status, uint16 XactId,
                               boolean IsFinalRsp, uint32 PayloadSize,
                               void *Payload);

};


#define IDIAG_AddRef(p)                        AEEGETPVTBL(p, IDiag)->AddRef(p)
#define IDIAG_Release(p)                      AEEGETPVTBL(p, IDiag)->Release(p)
#define IDIAG_RegisterApp(p, cls, ver, num, evts) \
                     AEEGETPVTBL(p, IDiag)->RegisterApp(p, cls, ver, num, evts)
#define IDIAG_UnregisterApp(p, cls)       \
                                   AEEGETPVTBL(p, IDiag)->UnregisterApp(p, cls)
#define IDIAG_GetTransactionId(p, cls, evt, wp, dwp) \
                  AEEGETPVTBL(p, IDiag)->GetTransactionId(p, cls, evt, wp, dwp)
#define IDIAG_GetRequestPayload(p, xid, paysz, pay) \
                   AEEGETPVTBL(p, IDiag)->GetRequestPayload(p, xid, paysz, pay)
#define IDIAG_SendDiagResponse(p, stat, xid, fin, paysz, pay) \
         AEEGETPVTBL(p, IDiag)->SendDiagResponse(p, stat, xid, fin, paysz, pay)

/*===========================================================================

                       INTERFACES DOCUMENTATION

===========================================================================*/
/*---------------------------------------------------------------------------
                          CALL FUNCTIONS
---------------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: IDIAG_AddRef

DESCRIPTION:
  Increments the reference count for the IDIAG object.  This enables
  the IDIAG object to determine when the last reference is removed and
  cleanup the object's resources.

PROTOTYPE:
  uint32 IDIAG_AddRef(IDiag *pMe);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface

RETURN VALUE:
  Returns the incremented reference count.  (Returns zero if the
  object does not exist.)

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IDIAG_Release

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_Release

DESCRIPTION:
  Decrements the reference count for the IDIAG object.  When no more
  references exist, it releases all of the object's resources.

PROTOTYPE:
  uint32 IDIAG_Release(IDiag *pMe);

PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface

RETURN VALUE:
  Returns the decremented reference count for the object.  If the object
  does not exist, or if the call releases the last reference, the return
  value is zero.

COMMENTS:
  If the app has registered via the IDIAG_RegisterApp call, the app
  must unregister by calling IDIAG_UnregisterApp BEFORE calling this
  function.  Failure to do so may result in a memory leak.

SIDE EFFECTS:

SEE ALSO:
  IDIAG_UnregisterApp
  IDIAG_AddRef

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_RegisterApp

DESCRIPTION:
  Registers an app with the IDIAG interface.  Registration enables app
  diagnostics for the app and allows it to register specific events for
  special handling.

PROTOTYPE:
  uint32 IDIAG_RegisterApp(IDiag *pMe, AEECLSID AppId, AECHAR *AppVersion,
                           uint16 NumRegEvents,
                           IDiagEvtReg *RegEvents);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface
  AEECLSID AppId:          Class ID for the app that is registering
  AECHAR *AppVersion:    Version string for the application.  The string
                         must be no longer than MAX_APP_VERSION_SIZE AECHARs,
                         including the NULL terminator
  uint16 NumRegEvents:     Number of events in the RegEvents array
  IDiagEvtReg *RegEvents:  An array of event registrations, indicating how
                           certain events should be handled.  IDiag makes
                           a copy of the registration structure, so that caller
                           may free the memory after this call, if necessary.
                           (See COMMENTS below.)

RETURN VALUE:
  Returns AEE return values

  AEE_SUCCESS:  Registration was successful
  EBADPARM:     One of the parameters had an invalid value (e.g. NULL pointer)
  EFAILED:      An unspecified internal error occurred

COMMENTS:
  If an application successfully registers more than once without
  unregistering, only the most recent registration will be recorded.

  Data from diagnostic requests can be passed to the app through the event
  sent to the app (if the data is small enough to fit into the event
  parameters), or it may be stored in the IDiag object and retrieved with
  the IDIAG_GetRequestPayload routine.

  An app only needs to include an event in its registration if that event
  will result in delayed responses, or if the event requires IDiag to store
  the data for later retrieval (rather than sending it in the event
  parameters).

  Note that the data pointed to by the RegEvents parameter is copied into
  a local IDiag structure.  The caller may release the memory after the
  call is complete.
   

SIDE EFFECTS:

SEE ALSO:
  IDIAG_UnregisterApp

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_UnregisterApp

DESCRIPTION:
  Removes an application's registration with the IDIAG interface.

PROTOTYPE:
  void IDIAG_UnregisterApp(IDiag *pMe, AEECLSID AppId);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface
  AEECLSID AppId:        Class ID for the app that is unregistering

RETURN VALUE:
  None (void)

COMMENTS:
  If, after unregistering, the app no longer intends to use the
  IDIAG interface, it should release its reference by calling
  IDIAG_Release.

SIDE EFFECTS:

SEE ALSO:
  IDIAG_RegisterApp
  IDIAG_Release

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_GetTransactionId

DESCRIPTION:
  Obtains the transaction ID associated with a particular request.  Apps
  need this transaction ID to extract the request payload (when needed)
  and to send diagnostic response messages.

PROTOTYPE:
  uint16 IDIAG_GetTransactionId(IDiag *pMe, AEECLSID AppId, AEEEvent EventId,
                                uint16 wp, uint32 dwp);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface
  AEECLSID AppId:        Class ID from the event that the app received
  AEEEvent EventId:      Event ID from the event that the app received
  uint16 wp:             Word parameter from the event that the app received
  uint32 dwp:            Double word parameter from the event that the
                         app received

RETURN VALUE:
  uint16 Transaction ID
  0:      Indicates that there was no outstanding request matching the
          event values
  Any other value: Transaction ID associated with that request

COMMENTS:
  Note that, for any event registered with the UsesPayload or
  UsesDelayedResponse set to TRUE, this function may be used to determine
  if a received event originated from a diagnostic request or from another
  source.  If the return value is non-zero, the event did originate from
  a diagnostic request; a return value of zero indicates another source
  (e.g. user interaction).

  Note also that this function assumes that no duplicate requests are being
  processed by an app at the same time.  If duplicate requests are received,
  from whatever source, the results of this function may be spurious.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_GetRequestPayload

DESCRIPTION:
  This function MUST be invoked when processing any event that is registered
  (via IDIAG_RegisterApp) with the UsesPayload flag set to TRUE when that
  event originates from a diagnostic request.  See COMMENTS below.

  Obtains the payload portion of a diagnostic request, which is stored in
  the IDiag object.  After this method is invoked, the data storage for
  that request in IDiag is released, and the payload may not be obtained
  again for that request.

PROTOTYPE:
  uint32 IDIAG_GetRequestPayload(IDiag *pMe, uint16 XactId,
                                 uint16 PayloadSize, void *Payload);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface
  uint16 XactId:         Transaction ID associated with the request.  The
                         transaction ID must be obtained with the
                         IDIAG_GetTransactionId method.
  uint16 PayloadSize:    The size, in bytes, of the buffer pointed to by
                         the Payload parameter.
  void *Payload:         A buffer into which the data from the request will
                         be copied, up to a maximum of PayloadSize bytes.

RETURN VALUE:
  Returns the number of bytes copied.

  0 indicates some kind of error occurred, and no payload was copied
  Any other value indicates the number of bytes that were copied

COMMENTS:
  Some important notes:
    1) The request payload is stored in IDiag ONLY for the events that are
       registered with the UsesBulkPayload flag set to TRUE.
    2) After this function executes, the memory for the request's payload is
       released, and the payload cannot be obtained again for that request.
    3) When an event is registered with the UsesPayload flag set to
       TRUE, this function **MUST** be invoked whenever the application
       receives that event due to a diagnostic request.  (Whether an event
       was due to a diagnostic request or not may be determined by calling
       IDIAG_GetTransactionId.)  Failure to invoke this function for events
       registered as using payload will result in a memory leak.
    4) This function assumes that Payload pointer to word-aligned memory.

SIDE EFFECTS:

SEE ALSO:
  IDIAG_GetTransactionId

=============================================================================*/

/*=============================================================================
FUNCTION: IDIAG_SendDiagResponse

DESCRIPTION:
  Sends a diagnostic response with payload data.  Header information
  for the response message will be obtained from IDIAG internal structures.

PROTOTYPE:
    uint32 IDIAG_SendDiagResponse(IDiag *pMe, uint32 Status, uint16 XactId,
                                  boolean IsFinalRsp, uint32 PayloadSize,
                                  void *Payload);


PARAMETERS:
  IDiag *pMe:            Pointer to the IDIAG interface
  uint32 Status:         An error code for the response message.  Zero
                         indicates success, all other values indicate failure.
                         This value may be an IDIAG_ERROR_CODE or any other
                         value.  Note that a non-zero Status always
                         indicates a final response, regardless of the value
                         of IsFinalRsp.
  uint16 XactId:         A transaction ID that associates this response with
                         a previous request.  It is used to determine the
                         header information for the response.
  boolean IsFinalRsp:    Boolean flag indicating if this is the final response
                         that will be sent for a diag request.
  uint32 PayloadSize:    Size of the payload structure
  void *Payload:         Payload structure; the data to be sent in the
                         response.  (Caller must manage this memory, see
                         COMMENTS below.)

RETURN VALUE:
  Returns AEE return values

  AEE_SUCCESS:  Diagnostic response message was sent
  EBADPARM:     One of the function arguments was invalid (e.g. NULL pointer)
  EFAILED:      An unspecified error occurred

COMMENTS:
  This function allocates memory for the outgoing message and copies the
  contents of the Payload argument into that message.  Thus the caller
  will need to manage (e.g. FREE) the memory referenced by Payload.

  This function is NOT re-entrant.  It modifies the vector of
  event headers and does not ensure mutually-exclusive access.

  Note that, if IsFinalRsp is TRUE or Status is non-zero, this call will
  result in a final response being sent.  In such cases, the stored header
  and payload information is deleted after sending the final response.

SIDE EFFECTS:

SEE ALSO:
  IDIAG_GetTransactionId

=============================================================================*/

#endif // AEEDIAG_H
