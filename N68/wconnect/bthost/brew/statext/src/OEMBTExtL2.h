#ifndef _OEMBTEXTL2_H_
#define _OEMBTEXTL2_H_
/*===========================================================================

FILE:      OEMBTExtL2.h

SERVICES:  BlueTooth L2 QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth L2

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005,2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtL2.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

/*===========================================================================

Function:  OEMBTExtL2_Init()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IBTExtL2 *pParent: Pointer to the IBTExtL2 allocated by AEE

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates app-associated memory for this object

===========================================================================*/
int    OEMBTExtL2_Init(IBTExtL2* pParent);
// IBase (see AEE.h)
uint32 OEMBTExtL2_Release( IBTExtL2* pParent );
int32  OEMBTExtL2_Read( IBTExtL2* pParent, void* buffer, uint32 bytes );
void   OEMBTExtL2_Readable( IBTExtL2* pParent, PFNNOTIFY pfn, void* pUser );
void   OEMBTExtL2_Cancel( IBTExtL2* pParent,  PFNNOTIFY pfn, void* pUser );
int    OEMBTExtL2_Register( IBTExtL2* pParent, uint32 PSM );
int    OEMBTExtL2_Deregister( IBTExtL2* pParent );
int    OEMBTExtL2_SetParams( IBTExtL2* pParent, uint16 proto_id, const AEEBTL2ConfigInfo* info );
int    OEMBTExtL2_Connect( IBTExtL2* pParent, uint16 proto_id, const AEEBTBDAddr* pBDAddr );
int    OEMBTExtL2_Disconnect( IBTExtL2* pParent );
int32  OEMBTExtL2_Write( IBTExtL2* pParent, const byte* buf, uint32 length );
int32  OEMBTExtL2_WriteV( IBTExtL2* pParent, const SockIOBlock* iov, uint16 nIovCount );
int32  OEMBTExtL2_ReadV( IBTExtL2* pParent, SockIOBlock* iov, uint16 nIovCount );
void   OEMBTExtL2_Writable( IBTExtL2* pParent, PFNNOTIFY pfn, void* pUser );
uint16 OEMBTExtL2_NumConn( IBTExtL2* pParent, const AEEBTBDAddr *pBdAddr );
int    OEMBTExtL2_DisconnImmediate( IBTExtL2* pParent );
int OEMBTExtL2_GetOEMHandle(IBTExtL2 *pParent, int32 *pnHandle);
int OEMBTExtL2_Clone(IBTExtL2 *pParent, int32 nHandle);
int OEMBTExtL2_GetStatus(IBTExtL2 *pParent, AEEBTL2Connected *pStatus);


#endif /* _OEMBTEXTL2_H_ */
