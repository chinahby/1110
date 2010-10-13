#ifndef _OEMBTEXTOPP_H_
#define _OEMBTEXTOPP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP)
/*===========================================================================

FILE:      OEMBTExtOPP.h

SERVICES:  BlueTooth Object Push Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth Object Push Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtOPP.h#2 $
$DateTime: 2009/04/17 11:28:04 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtOPP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtOPP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtOPP_Init( IBTExtOPP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtOPP_Release( IBTExtOPP* pParent );

// IBTEXTOPP_Register (see AEEBTExtOPP.h)
int OEMBTExtOPP_Register( IBTExtOPP*     pParent, 
                          AEEBTOPPFormat supportedFormat,
                          const char*    szServiceName );

// IBTEXTOPP_Deregister (see AEEBTExtOPP.h)
int OEMBTExtOPP_Deregister( IBTExtOPP* pParent );

// IBTEXTOPP_AcceptConnection (see AEEBTExtOPP.h)
int OEMBTExtOPP_AcceptConnection( IBTExtOPP*         pParent, 
                                  const AEEBTBDAddr* pBDAddr, 
                                  boolean            bAllowPush, 
                                  boolean            bAllowPull );

// IBTEXTOPP_Connect (see AEEBTExtOPP.h)
int OEMBTExtOPP_Connect( IBTExtOPP*         pParent,
                         const AEEBTBDAddr* pBDAddr, 
                         uint8              uChannelNumber );

// IBTEXTOPP_Abort (see AEEBTExtOPP.h)
int OEMBTExtOPP_Abort( IBTExtOPP* pParent );

// IBTEXTSD_Push (see AEEBTExtOPP.h)
int OEMBTExtOPP_Push( IBTExtOPP*      pParent, 
                      const AECHAR*   pwFileName, 
                      AEEBTObjectType objType );

// IBTEXTSD_Pull (see AEEBTExtOPP.h)
int OEMBTExtOPP_Pull( IBTExtOPP* pParent, const AECHAR* pwFileName );

// IBTEXTOPP_Disconnect (see AEEBTExtOPP.h)
int OEMBTExtOPP_Disconnect( IBTExtOPP* pParent );

// IBTEXTOPP_GetProgressInfo (see AEEBTExtOPP.h)
int OEMBTExtOPP_GetProgressInfo(IBTExtOPP* pParent, AEEBTProgressInfo* pProgressInfo);
                           

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_OPP)
#endif // _OEMBTEXTOPP_H_
