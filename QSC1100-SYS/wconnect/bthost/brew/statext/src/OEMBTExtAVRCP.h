#ifndef _OEMBTEXTAVRCP_H_
#define _OEMBTEXTAVRCP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV)
/*===========================================================================

FILE:      OEMBTExtAVRCP.h

SERVICES:  BlueTooth AVRCP QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth AVRCP

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtAVRCP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtAVRCP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtAVRCP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtAVRCP_Init( IBTExtAVRCP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtAVRCP_Release( IBTExtAVRCP* pParent );

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Enable(IBTExtAVRCP*  pParent,
                         uint8         uSupportedCategories);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Connect(IBTExtAVRCP*  pParent,
                          AEEBTBDAddr* pBDAddr);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Disconnect(IBTExtAVRCP* po);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_Disable(IBTExtAVRCP* po);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_SendReply(IBTExtAVRCP*           pParent,
                            uint8                  uTxnID,
                            AEEBTAVRCPOpStatusType uStatus,
                            AEEBTAVRCPEventType    Evt,
                            AEEBTAVRCPRspType      Rsp);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_SendGenericReply(IBTExtAVRCP*             pParent,
                                   AEEBTAVRCPGenericOpType* pFrame,
                                   uint8*                   pRsp);

// See AEEBTExtAVRCP
int OEMBTExtAVRCP_GetGenericOp(IBTExtAVRCP*             pParent,
                               uint8*                   pBuf,
                               int                      nSize );

#endif /* defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_AV) */
#endif /* _OEMBTEXTAVRCP_H_ */
