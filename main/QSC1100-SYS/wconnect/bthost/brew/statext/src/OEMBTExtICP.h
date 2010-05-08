#ifndef _OEMBTEXTICP_H_
#define _OEMBTEXTICP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)
/*===========================================================================

FILE:      OEMBTExtICP.h

SERVICES:  BlueTooth Intercom Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Cordless Telephony 
                     Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtICP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtICP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtICP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtICP_Init( IBTExtICP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtICP_Release( IBTExtICP* pParent );

// IBTEXTICP_Connect (see AEEBTExtICP.h)
int OEMBTExtICP_Connect( 
                          IBTExtICP*                pParent,
                          const AEEBTBDAddr*        pBDAddr);

// IBTEXTICP_AcceptConnection (see AEEBTExtICP.h)
int OEMBTExtICP_AcceptConnection( 
                          IBTExtICP*                pParent,
                          const AEEBTBDAddr*        pBDAddr,
                          boolean                   bAccept);

// IBTEXTICP_Disconnect (see AEEBTExtICP.h)
int OEMBTExtICP_Disconnect( 
                          IBTExtICP*                pParent);

// IBTEXTICP_ClearCall (see AEEBTExtICP.h)
int OEMBTExtICP_ClearCall( 
                          IBTExtICP*                pParent,
                          AEEBTICPCauseType         cause);

// IBTEXTICP_EstablishCall (see AEEBTExtICP.h)
int OEMBTExtICP_EstablishCall( 
                          IBTExtICP*                pParent,
                          const AEEBTICPNumberType* pCallingNum,
                          const AEEBTICPNumberType* pCalledNum);

// IBTEXTICP_IncomingStatus (see AEEBTExtICP.h)
int OEMBTExtICP_IncomingStatus(
                          IBTExtICP*                pParent,
                          AEEBTICPCallStatusType    status);

// IBTEXTICP_AcceptCall (see AEEBTExtICP.h)
int OEMBTExtICP_AcceptCall(
                          IBTExtICP*                pParent);

// IBTEXTICP_Register (see AEEBTExtICP.h)
int OEMBTExtICP_Register( 
                          IBTExtICP*                pParent);

// IBTEXTICP_Deregister (see AEEBTExtICP.h)
int OEMBTExtICP_Deregister( 
                          IBTExtICP*                pParent);

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP) */
#endif // _OEMBTEXTICP_H_
