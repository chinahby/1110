#ifndef _OEMBTEXTSML_H_
#define _OEMBTEXTSML_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_SML)
/*===========================================================================

FILE:      OEMBTExtSML.h

SERVICES:  BlueTooth SyncML Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth SyncML Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtSML.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtSML_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtSML *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtSML_Init( IBTExtSML* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtSML_Release( IBTExtSML* pParent );

// IBTEXTSML_RegisterClient (see AEEBTExtSML.h)
int OEMBTExtSML_RegisterClient( 
                          IBTExtSML*                  pParent, 
                          const char*                 pServiceUUID,
                          boolean                     bAuth );

// IBTEXTSML_RegisterServer (see AEEBTExtSML.h)
int OEMBTExtSML_RegisterServer( 
                          IBTExtSML*                   pParent, 
                          const char*                  pServiceUUID,
                          boolean                      bAuth );

// IBTEXTSML_DeregisterClient (see AEEBTExtSML.h)
int OEMBTExtSML_DeregisterClient( 
                          IBTExtSML*                   pParent );

// IBTEXTSML_DeregisterServer (see AEEBTExtSML.h)
int OEMBTExtSML_DeregisterServer( 
                          IBTExtSML*                   pParent );

// IBTEXTSML_AcceptConnection (see AEEBTExtSML.h)
int OEMBTExtSML_AcceptConnection( 
                          IBTExtSML*                   Parent,
                          boolean                      bAccept );

// IBTEXTOPP_Connect (see AEEBTExtSML.h)
int OEMBTExtSML_Connect( 
                          IBTExtSML*                   pParent,
                          const AEEBTBDAddr*           pBDAddr, 
                          uint8                        uChannelNumber,
                          const char*                  pTarget );

// IBTEXTSML_Disconnect (see AEEBTExtSML.h)
int OEMBTExtSML_Disconnect( 
                          IBTExtSML*                   pParent );

// IBTEXTSML_Get (see AEEBTExtSML.h)
int OEMBTExtSML_Get( 
                          IBTExtSML*                   pParent,
                          const char*                  pType );

// IBTEXTSML_Put (see AEEBTExtSML.h)
int OEMBTExtSML_Put( 
                          IBTExtSML*                   pParent, 
                          const char*                  pType,
                          const AEEBTSMLbyteSeqType*   pData );

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_SML)
#endif // _OEMBTEXTSML_H_
