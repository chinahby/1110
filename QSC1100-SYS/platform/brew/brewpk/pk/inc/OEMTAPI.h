#ifndef __OEMTAPI_H__
#define __OEMTAPI_H__
/*============================================================================

FILE:  OEMTAPI.h


==============================================================================

               Copyright © 1999-2003 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "AEEComdef.h"
#include "AEETAPI.h"

extern boolean AEETAPI_IsDebugMsgOn(void);

#define FARF_TAPI              AEETAPI_IsDebugMsgOn()

#ifdef FARF
#undef FARF
#endif // FARF

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

typedef struct _OEMTAPI OEMTAPI;

#ifdef __cplusplus
extern "C" {
#endif

   AEE_EXPORTS int              OEMTAPI_New( OEMTAPI **ppif );
   AEE_EXPORTS int              OEMTAPI_Destroy( OEMTAPI * po );
   AEE_EXPORTS int              OEMTAPI_GetStatus( OEMTAPI * po, TAPIStatus * ps);
   AEE_EXPORTS int              OEMTAPI_MakeVoiceCall(OEMTAPI * po, const char * pszNumber, AEECallback *pcbDial);
   AEE_EXPORTS boolean          OEMTAPI_GetCallerID(OEMTAPI * po, AECHAR * psz, int nSize);
   AEE_EXPORTS int              OEMTAPI_OnCallStatus(OEMTAPI * po, AEECallback *pcb);
   AEE_EXPORTS boolean          OEMTAPI_IsVoiceCall(OEMTAPI * po);
   AEE_EXPORTS boolean          OEMTAPI_IsDataSupported(OEMTAPI * po);
   AEE_EXPORTS uint16           OEMTAPI_GetCallStatus( OEMTAPI *po );
   AEE_EXPORTS int              OEMTAPI_OnPhoneStatus(OEMTAPI * po, AEECallback *pcb);

#ifdef __cplusplus
}
#endif

/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: OEMTAPI

Description:
   This interface is the implements some of the Telephony API. This interface
has been created to serve as the entry into OEM layer.
=======================================================================

Function: OEMTAPI_New()

Description:
   Creates the instance of OEMTAPI object. Caller gets the handle representing
OEMTAPI object

Prototype:
   int OEMTAPI_New( IShell * ps, void **ppif );

Parameters:
   ps:  Pointer to Shell.
   ppif: Returns OEMTAPI object.

Return Value:
   SUCCESS
   EBADPARM
   ENOMEMORY
   EFAILED

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_Destroy()

Description:
   Destroys the OEMTAPI object represented by the given handle.

Prototype:
   int OEMTAPI_Destroy( OEMTAPI *pMe );

Parameters:
   pMe: A handle to OEMTAPI object.

Return Value:

  SUCCESS
  EBADPARM

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_SetMask()

Description:
   Saves the mask of events on which the caller is requesting notifications.

Prototype:
   void OEMTAPI_SetMask( OEMTAPI * pme, uint32 dwMask );

Parameters:
   pme: A handle representing OEMTAPI object.
   dwMask: the Mask from NMASK_TAPI_*

Return Value:
   None.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_GetStatus()

Description:
   This function obtains the current status of the telephony device, including
service and call status.

Prototype:
   int OEMTAPI_GetStatus( OEMTAPI * po, TAPIStatus * ps);

Parameters:
   po: A handle representing OEMTAPI object.
   ps: The buffer in which TAPIStatus will be written.

Return Value:

  SUCCESS
  EBADPARM

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_MakeVoiceCall()

Description:
   Places an originate request with the system. The Callback is resumed when the
   request is honored.

Prototype:
   int OEMTAPI_MakeVoiceCall(OEMTAPI * po, const char * pszNumber, AEECallback *pcbDial);

Parameters:
   po: Handle representing OEMTAPI object.
   pszNumber: NULL terminated digits to dial.
   pcbDial: The callback to resume.

Return Value:

  SUCCESS
  EBADPARM
  ENOMEMORY

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_GetCallerID()

Description:
   Retrieves the calling Line ID into the buffer provided.

Prototype:
   boolean OEMTAPI_GetCallerID(OEMTAPI * po, AECHAR * psz, int nSize);

Parameters:
   po: Handle representing OEMTAPI object.
   psz: The buffer for placing the CallerID in the AECHAR format.
   nSize: Size of buffer in bytes.

Return Value:

  TRUE
  FALSE

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_OnCallStatus()

Description:
   Registers a Persistent Callback to be invoked for Call Events.

Prototype:
   int OEMTAPI_OnCallStatus(OEMTAPI * po, AEECallback *pcb);

Parameters:
   po: Handle representing OEMTAPI object.
   pcb: Persistent callback to register.

Return Value:

  SUCCESS

Comments:
   The pcb item if any exists in OEMTAPI object is over-written. The registered
   callback lives in the OEMTAPI object until over-written or the OEMTAPI object
   is destroyed.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_IsVoiceCall()

Description:
   Identifies the call if any exists at that time is a voice call.

Prototype:
   boolean OEMTAPI_IsVoiceCall(OEMTAPI * po);

Parameters:
   po: Handle representing OEMTAPI object.

Return Value:

  TRUE
  FALSE

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_IsDataSupported()

Description:
   Identifies if the device supports Data calls.

Prototype:
   boolean OEMTAPI_IsDataSupported(OEMTAPI * po);

Parameters:
   po: Handle representing OEMTAPI object.

Return Value:

  TRUE
  FALSE

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_GetCallStatus()

Description:
   Reads the events that transpired since the last invokation of this function.

Prototype:
   uint16 OEMTAPI_GetCallStatus( OEMTAPI *po );

Parameters:
   po: Handle representing OEMTAPI object.

Return Value:

   A 32 bit mask of events that occured since the last invokation of this function.

Comments:
   This does not inform how many times an event occured.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_OnPhoneStatus()

Description:
   Registers a persistent callback to be invoked whenever the phone status changes.

Prototype:
   int OEMTAPI_OnPhoneStatus( OEMTAPI *po, AEECallback *pcb );

Parameters:
   po: Handle representing OEMTAPI object.
   pcb: The persistent callback.

Return Value:

   SUCCESS
   EBADPARM

Comments:
   Overwrites the pcb item held for the purpose in OEMTAPI object.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTAPI_OnMOMessage()

Description:
   Registers a persistent callback for Mobile originated SMS message

Prototype:
   int OEMTAPI_OnMOMessage( OEMTAPI *po, AEECallback *pcb );

Parameters:
   po: Handle representing OEMTAPI object
   pcb: The callback to be invoked whenever the MOSMS is processed.

Return Value:

  SUCCESS
  EBADPARM

Comments:
   None

Side Effects:
   None

See Also:
   None

==============================================================
AEETAPI

See the ITAPI Interface in the BREW API Reference.


=======================================================================*/

#endif /* !__OEMTAPI_H__ */
