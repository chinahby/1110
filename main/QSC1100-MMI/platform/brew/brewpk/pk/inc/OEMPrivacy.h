/*======================================================
FILE:  OEMPrivacy.h

SERVICES:   OEM privacy interface

GENERAL DESCRIPTION:

Privacy related definitions for the OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEM_PRIVACY_H)
#define OEM_PRIVACY_H  1

#include "AEE.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
   PRT_POSITION,
   PRT_DIAL_VOICE
} OEMPrivacyReqType;

extern void OEM_CheckPrivacy(OEMPrivacyReqType t, AEECallback *pCB, int *pStatus);

#if defined(__cplusplus)
}
#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

OEMPrivacyReqType

Description:
   The type of API request application made.

Definition:

   typedef enum {
      PRT_POSITION,
      PRT_DIAL_VOICE
   } OEMPrivacyReqType;

Supported Values:

   PRT_POSITION : is when application requests IPOSDET_GetGPSInfo().
   PRT_DIAL_VOICE : is when application requests ITAPI_MakeVoiceCall().

Comments:
   None.

See Also:
   None.

======================================================================= 
INTERFACES   DOCUMENTATION
======================================================================= 
OEM Interface Reference
======================================================================
Function: OEM_CheckPrivacy

Description:
   Allows the OEM to specify the correct action for various outbound requests.

Prototype:
   void OEM_CheckPrivacy(OEMPrivacyReqType t, AEECallback *pcb, int *pnStatus);

Parameters:
   t : [in] : The type of API request made by application.
   pcb : [in] : Callback to resume when privacy validation is complete.
   pnStatus : [out] : Response buffer of sizeof(int).
   
Return Value:
   None

Comments:

   OEM may implement this function to conform to the requirements for the carrier.  
Note that the function works asynchronously.  This allows the OEM to display a message/prompt 
to the user, contact the network, etc. The callback is then called with the appropriate error 
code (0 - success) and the request will either proceed or will fail based upon the policy 
decision.

   An example implementation is available inside FEATURE_BREW_PRIVACY_PROMPT.

Side Effects:
   None

See Also:
   IPOSDET_GetGPSInfo()
   ITAPI_MakeVoiceCall()

==================================================================*/
#endif // !defined(OEM_PRIVACY_H)
