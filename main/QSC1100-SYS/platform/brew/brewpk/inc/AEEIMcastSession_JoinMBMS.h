#ifndef AEEIMCASTSESSION_JOINMBMS_H
#define AEEIMCASTSESSION_JOINMBMS_H

/*===================================================

FILE: AEEIMcastSession_JoinMBMS.h

SERVICES:  IMcastSession - JoinMBMS extension

DESCRIPTION:
   Interface definitions, data structures, etc. 
   for IMcastSession JoinMBMS extension

======================================================

      Copyright (c) 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEIMcastSession.h"
#include "AEEMcastTypes_MBMS.h"

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Interface
//-----------------------------------------------------------------

// Unique IDs for MBMS Join ExtFunc() 
#define IMCASTSESSION_EXT_FUNC_MBMS_JOIN  0x0105abc9

/*
/ for IMcastSession_JoinMBMS()
*/
typedef struct AEEMcastSessionJoinMBMS
{
   void*                  pGroupAddr;
   AEEMcast_MBMSJoinInfo* pMBMSJoinInfo;
} AEEMcastSessionJoinMBMS;

//-----------------------------------------------------------------
// Methods
//-----------------------------------------------------------------

#if defined(AEEINTERFACE_CPLUSPLUS)
int CDECL IMcastSession_JoinMBMS(
                                 IMcastSession* me, void* pGroupAddr, AEEMcast_MBMSJoinInfo* pMBMSInfo
                                 )
#else
static __inline int IMcastSession_JoinMBMS(
   IMcastSession* me, void* pGroupAddr, AEEMcast_MBMSJoinInfo* pMBMSInfo
   )
#endif // #if defined(AEEINTERFACE_CPLUSPLUS)
{   
   AEEMcastSessionJoinMBMS mbmsJoin;

   mbmsJoin.pGroupAddr    = pGroupAddr;
   mbmsJoin.pMBMSJoinInfo = pMBMSInfo;
#if defined(AEEINTERFACE_CPLUSPLUS)
   return me->ExtFunc(me, IMCASTSESSION_EXT_FUNC_MBMS_JOIN, 
      (void*)&mbmsJoin, sizeof(AEEMcastSessionJoinMBMS));
#else
   return AEEGETPVTBL(me, IMcastSession)->ExtFunc(me, IMCASTSESSION_EXT_FUNC_MBMS_JOIN, 
      (void*)&mbmsJoin, sizeof(AEEMcastSessionJoinMBMS));
#endif // #if defined(AEEINTERFACE_CPLUSPLUS)
}

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

=======================================================================

IMcastSession_JoinMBMS()

Description:
   This function issues a request for joining an MBMS multicast group.
   See IMcastSession_Join for further details.
   
Prototype:
   int IMcastSession_JoinMBMS(
      IMcastSession* me, 
      void* pGroupAddr, 
      AEEMcast_MBMSJoinInfo* pMBMSJoinInfo,
      );
   
Parameters:
   me: the interface pointer
   pGroupAddr: multicast group address and port to join - a pointer to 
               an initialized AEESockAddrStorage or AEESockAddrInet structure
   pMBMSJoinInfo: MBMS parameters for multicast join operation.

Return Value:
   AEE_SUCCESS:    request was submitted.
   AEE_EBADSTATE:  object state is not suitable for this operation.
   AEE_EBADPARM:   pGroupAddr is NULL or invalid.
   AEE_NET_WOULDBLOCK: the operation was not completed - register a retry 
               callback via IMcastSession_AddRetryCB().
   
   Other error codes are also possible.

Comments:
   None.

Version:
   Introduced BREW Client 4.0.2

See Also:
   IMcastSession_Join

=======================================================================*/

#endif    // AEEIMCASTSESSION_JOINMBMS_H
