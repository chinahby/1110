#ifndef _OEMBTEXTPBAP_H_
#define _OEMBTEXTPBAP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP)
/*===========================================================================

FILE:      OEMBTExtPBAP.h

SERVICES:  BlueTooth Phone Book Access Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Phone Book 
           Access Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2005-2006 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtPBAP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtPBAP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtPBAP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtPBAP_Init( IBTExtPBAP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtPBAP_Release( IBTExtPBAP* pParent );

// IBTEXTPBAP_RegisterServer (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_RegisterServer( 
                          IBTExtPBAP*             pParent, 
                          const char*             pSrvName,
                          AEEBTPBAPRepositories   supRepositories,
                          AEEBTPBAPAuthentication auth );

// IBTEXTPBAP_DeregisterServer (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_DeregisterServer( 
                          IBTExtPBAP*             pParent );

// IBTEXTPBAP_Authenticate (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Authenticate( 
                          IBTExtPBAP*             pParent,
                          const AECHAR*           pUserID,
                          const AECHAR*           pPIN );

// IBTEXTPBAP_AcceptConnection (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_AcceptConnection( 
                          IBTExtPBAP*             pParent,
                          boolean                 bAccept );

// IBTEXTPBAP_Connect (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Connect( 
                          IBTExtPBAP*             pParent,
                          const AEEBTBDAddr*      pBDAddr, 
                          uint8                   uChannelNumber,
                          boolean                 bAuth );

// IBTEXTPBAP_Disconnect (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Disconnect( 
                          IBTExtPBAP*             pParent );

// IBTEXTPBAP_RegisterClient (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_RegisterClient( 
                          IBTExtPBAP*             pParent, 
                          const char*             pCliName );

// IBTEXTPBAP_DeregisterClient (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_DeregisterClient( 
                          IBTExtPBAP*             pParent );

// IBTEXTPBAP_Abort (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_Abort( 
                          IBTExtPBAP*             pParent );

// IBTEXTPBAP_PullPhoneBook (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullPhoneBook( 
                          IBTExtPBAP*             pParent,
                          const AECHAR*           pwObjName,
                          const AEEBTPBAPAttribMask* pAttribMask,
                          const AEEBTPBAPFormat*  pFormat,
                          const uint16*           pMaxListCount,
                          const uint16*           pListStartOffset );

// IBTEXTPBAP_SetPhoneBook (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_SetPhoneBook( 
                          IBTExtPBAP*             pParent, 
                          const AECHAR*           pwFolder,
                          AEEBTPBAPSetPathFlag    path );

// IBTEXTPBAP_PullvCardListing (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardListing( 
                          IBTExtPBAP*             pParent, 
                          const AECHAR*           pwFolder,
                          const AEEBTPBAPOrder*   pSearchOrder,
                          const AECHAR*           pwSearchValue,
                          const AEEBTPBAPSearchAttrib* pSearchAttrib,
                          const uint16*           pMaxListCount,
                          const uint16*           pListStartOffset );

// IBTEXTPBAP_PullvCardEntry (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardEntry( 
                          IBTExtPBAP*             pParent, 
                          const AECHAR*           pwName,
                          const AEEBTPBAPAttribMask* pAttribMask,
                          const AEEBTPBAPFormat*  pFormat );

// IBTEXTPBAP_PullPhoneBookResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullPhoneBookResponse( 
                          IBTExtPBAP*             pParent,
                          const AECHAR*           pwFileName,
                          const uint16*           pPhoneBookSize,
                          const uint8*            pNewMissedCalls,
                          AEEBTError              error );

// IBTEXTPBAP_SetPhoneBookResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_SetPhoneBookResponse( 
                          IBTExtPBAP*             pParent, 
                          AEEBTError              error );

// IBTEXTPBAP_PullvCardListingResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardListingResponse( 
                          IBTExtPBAP*             pParent, 
                          const AECHAR*           pwFileName,
                          const uint16*           pPhoneBookSize,
                          const uint8*            pNewMissedCalls,
                          AEEBTError              error );

// IBTEXTPBAP_PullvCardEntryResponse (see AEEBTExtPBAP.h)
int OEMBTExtPBAP_PullvCardEntryResponse( 
                          IBTExtPBAP*             pParent, 
                          const AECHAR*           pwFileName,
                          AEEBTError              error);


#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_PBAP)
#endif // _OEMBTEXTPBAP_H_
