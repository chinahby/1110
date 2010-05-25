#ifndef _OEMBTEXTBPP_H_
#define _OEMBTEXTBPP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP)
/*===========================================================================

FILE:      OEMBTExtBPP.h

SERVICES:  BlueTooth Basic Printing Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Basic Printing Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtBPP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtBPP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtBPP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtBPP_Init( IBTExtBPP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtBPP_Release( IBTExtBPP* pParent );

// IBTEXTBPP_Register (see AEEBTExtBPP.h)
int OEMBTExtBPP_Register( 
                          IBTExtBPP*             pParent, 
                          const char*            pSrvName );

// IBTEXTBPP_Deregister (see AEEBTExtBPP.h)
int OEMBTExtBPP_Deregister( 
                          IBTExtBPP*             pParent );

// IBTEXTBPP_Authenticate (see AEEBTExtBPP.h)
int OEMBTExtBPP_Authenticate( 
                          IBTExtBPP*             pParent,
                          AEEBTBPPTargetType     target,
                          const AECHAR*          pUserID,
                          const AECHAR*          pPassword );

// IBTEXTBPP_AcceptConnection (see AEEBTExtBPP.h)
int OEMBTExtBPP_AcceptConnection( 
                          IBTExtBPP*             pParent );

// IBTEXTOPP_Connect (see AEEBTExtBPP.h)
int OEMBTExtBPP_Connect( 
                          IBTExtBPP*             pParent,
                          const AEEBTBDAddr*     pBDAddr, 
                          uint8                  uChannelNumber,
                          AEEBTBPPTargetType     target );

// IBTEXTBPP_Disconnect (see AEEBTExtBPP.h)
int OEMBTExtBPP_Disconnect( 
                          IBTExtBPP*             pParent,
                          AEEBTBPPTargetType     target );

// IBTEXTBPP_Abort (see AEEBTExtBPP.h)
int OEMBTExtBPP_Abort( 
                          IBTExtBPP*             pParent );

// IBTEXTBPP_SendFile (see AEEBTExtBPP.h)
int OEMBTExtBPP_SendFile( 
                          IBTExtBPP*             pParent,
                          AEEBTBPPdocType        documentType,
                          const char*            pFileName,
                          const AECHAR*          pDescriptor,
                          uint32                 jobID );

// IBTEXTBPP_SOAPrequest (see AEEBTExtBPP.h)
int OEMBTExtBPP_SOAPRequest( 
                          IBTExtBPP*             pParent, 
                          const char*            pSndrSOAPmsg,
                          char*                  pSOAPmsg,
                          uint16                 SOAPmsgMaxLen,
                          AEEBTBPPTargetType     target );

// IBTEXTBPP_GetRefObjResponse (see AEEBTExtBPP.h)
int OEMBTExtBPP_GetRefObjResponse( 
                          IBTExtBPP*             pParent, 
                          const char*            pFileName,
                          uint32                 offset,
                          int32                  count );

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_BPP)
#endif // _OEMBTEXTBPP_H_
