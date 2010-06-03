#ifndef _OEMBTEXTCTP_H_
#define _OEMBTEXTCTP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP)
/*===========================================================================

FILE:      OEMBTExtCTP.h

SERVICES:  BlueTooth Cordless Telephony Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Cordless Telephony 
                     Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtCTP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtCTP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtCTP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtCTP_Init( IBTExtCTP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtCTP_Release( IBTExtCTP* pParent );

// IBTEXTCTP_Connect (see AEEBTExtCTP.h)
int OEMBTExtCTP_Connect( 
                          IBTExtCTP*                pParent,
                          const AEEBTBDAddr*        pBDAddr);

// IBTEXTCTP_AcceptConnection (see AEEBTExtCTP.h)
int OEMBTExtCTP_AcceptConnection( 
                          IBTExtCTP*                pParent,
                          const AEEBTBDAddr*        pBDAddr,
                          boolean                   bAccept);

// IBTEXTCTP_Disconnect (see AEEBTExtCTP.h)
int OEMBTExtCTP_Disconnect( 
                          IBTExtCTP*                pParent);

// IBTEXTCTP_ClearCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_ClearCall( 
                          IBTExtCTP*                pParent,
                          AEEBTCTPCauseType         cause);

// IBTEXTCTP_EstablishCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_EstablishCall( 
                          IBTExtCTP*                pParent,
                          const AEEBTCTPNumberType* pCallingNum,
                          const AEEBTCTPNumberType* pCalledNum);

// IBTEXTCTP_Info (see AEEBTExtCTP.h)
int OEMBTExtCTP_Info(
                          IBTExtCTP*                po,
                          boolean                   bSendingCmplt,
                          char                      keypadFacility,
                          const AEEBTCTPNumberType* pCallingNum,
                          const AEEBTCTPNumberType* pCalledNum,
                          AEEBTCTPAudioControlType  audioControl);

// IBTEXTCTP_IncomingStatus (see AEEBTExtCTP.h)
int OEMBTExtCTP_IncomingStatus(
                          IBTExtCTP*                pParent,
                          AEEBTCTPCallStatusType    status);

// IBTEXTCTP_AcceptCall (see AEEBTExtCTP.h)
int OEMBTExtCTP_AcceptCall(
                          IBTExtCTP*                pParent);

// IBTEXTCTP_StartDTMF (see AEEBTExtCTP.h)
int OEMBTExtCTP_StartDTMF(
                          IBTExtCTP*                pParent,
                          char                      tone);

// IBTEXTCTP_StopDTMF (see AEEBTExtCTP.h)
int OEMBTExtCTP_StopDTMF(
                          IBTExtCTP*                pParent);

// IBTEXTCTP_Register (see AEEBTExtCTP.h)
int OEMBTExtCTP_Register( 
                          IBTExtCTP*                pParent);

// IBTEXTCTP_Deregister (see AEEBTExtCTP.h)
int OEMBTExtCTP_Deregister( 
                          IBTExtCTP*                pParent);

// IBTEXTCTP_RegisterRecall (see AEEBTExtCTP.h)
int OEMBTExtCTP_RegisterRecall( 
                          IBTExtCTP*                pParent);

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_CTP) */
#endif // _OEMBTEXTCTP_H_
