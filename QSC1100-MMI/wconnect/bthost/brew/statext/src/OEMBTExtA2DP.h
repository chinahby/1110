#ifndef _OEMBTEXTA2DP_H_
#define _OEMBTEXTA2DP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
/*===========================================================================

FILE:      OEMBTExtA2DP.h

SERVICES:  BlueTooth A2DP QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth A2DP

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtA2DP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtA2DP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtA2DP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtA2DP_Init( IBTExtA2DP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtA2DP_Release( IBTExtA2DP* pParent );

// See AEEBTExtA2DP
int OEMBTExtA2DP_SetDevice(IBTExtA2DP*  pParent,
                           AEEBTBDAddr* pBDAddr);

// See AEEBTExtA2DP
int OEMBTExtA2DP_Connect(IBTExtA2DP*  pParent,
                         AEEBTBDAddr* pBDAddr);

// See AEEBTExtA2DP
int OEMBTExtA2DP_Disconnect(IBTExtA2DP* po);

// See AEEBTExtA2DP
int OEMBTExtA2DP_Ctl(IBTExtA2DP* po, uint8 ctl, uint32 data);

// See AEEBTExtA2DP
int OEMBTExtA2DP_Start(IBTExtA2DP* po);

// See AEEBTExtA2DP
int OEMBTExtA2DP_Suspend(IBTExtA2DP* po);

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */
#endif /* _OEMBTEXTA2DP_H_ */
