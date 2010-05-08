/*======================================================
FILE:  AEE_OEMNet.h

SERVICES:  AEE Net Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
net routines that are required by the OEM.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_NET_H)
#define AEE_OEM_NET_H  1

#if defined(__cplusplus)
extern "C" {
#endif

//
// AEE Net Helpers
//

extern void AEENetList_ReleasePPP(void);
extern void AEENetList_ClosePPP(void);

#if defined(__cplusplus)
}
#endif

/*=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================
AEE_OEMNet Interface
=========================================================================

===========================================================================
Function:  AEENetList_ReleasePPP()

Description:
   For each network:
      1) Releases the physical link.
      2) On targets that do not support dormancy, PPP will also be closed

Prototype:
   void AEENetList_ReleasePPP(void)

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

===========================================================================
Function:  AEENetList_ClosePPP()

Description:
   For each network:
      1) Close PPP

Prototype:
   void AEENetList_ClosePPP(void)

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

==============================================================
AEENet


See the INetMgr Interface and the ISocket Interface in the BREW API Reference.


=======================================================================*/

#endif // !defined(AEE_OEM_NET_H)