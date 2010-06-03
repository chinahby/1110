#if !defined(OEMAUTH_H)
#define OEMAUTH_H


/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMAuth.h

SERVICES:  AEE OEM Auth Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
OEM's auth functionality.

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"

typedef struct _OEMAuth_Challenge
{
   byte*    pChallenge;    // server's NN challege passed through AEE
   uint32   nChallengeLen; // number of bytes in pChallenge buffer
   byte*    pResponse;     // OEM's response to server's challenge
   uint32   nResponseLen;  // number of bytes in pResponse or needed for response
   uint32   nResult;       // Result of operation
} OEMAuth_Challenge;


extern void OEMAuth_GetChallengeResponse(OEMAuth_Challenge* pChallenge,  AEECallback *pcb);

/*========================================================================
   DATA STRUCTURE DOCUMENTATION
==========================================================================

OEMAuth_Challenge

Description:
   This structure is used to pass challenge and response between AEE and OEM layer.

Definition:
   typedef struct _OEMAuth_Challenge
   {
      byte*          pChallenge;          // [in] server's NN challege passed through AEE  
      uint32         nChallengeLen;       // [in] number of bytes in pChallenge buffer
      byte*          pResponse;           // [out] OEM's response to server's challenge
      uint32         nResponseLen;        // [in/out] on input length of pRespone buffer, on output length needed
                                          // to fill the response
      uint32         nResult;             // [out] Result of operation 
                                          // SUCCESS: if successful
                                          // ENEEDMORE: if the length of pResponse buffer is not enough. Return needed length in nResponseLen
                                          // other e.g ENOMEMORY (See AEEError.h)
   } OEMAuth_Challenge;

Comments:
   None.

See Also:
   None.


==========================================================================
  INTERFACES DOCUMENTATION
==========================================================================
Interface Name: OEMAuth
=========================================================================

Function: OEMAuth_GetChallengeResponse()

Description:
   This function is called by the AEE-layer in order to pass an Auth Challenge
   to an Operator-defined algorithm implemented in the OEM-layer. The challenge 
   parameters will be passed in the pChallenge structure. After the challenge 
   algorithm has completed the [out] members of pChallenge need to be filled in
   and the pcb callback needs to be scheduled.
 
   NOTE: Please check Operator requirements to determine if this capability
   is required to be implemented.
Prototype:
   void OEMAuth_GetChallengeResponse(OEMAuth_Challenge* pChallenge,  AEECallback *pcb)

Parameters:
   pChallenge: pointer to OEMAuth_Challenge structure
   pcb: AEE callback to call when response is available

Return Value:
   none

Comments:
   None.

Side Effects:
   None.

See Also:
   OEMAuth_Challenge
   CFGI_OEMAUTH_CHALLENGE_CAP
   CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN


==========================================================================*/
#endif
