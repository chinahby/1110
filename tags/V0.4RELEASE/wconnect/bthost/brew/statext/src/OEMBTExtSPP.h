#ifndef _OEMBTEXTSPP_H_
#define _OEMBTEXTSPP_H_
/*===========================================================================

FILE:      OEMBTExtSPP.h

SERVICES:  BlueTooth SPP QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth SPP

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005,2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary/
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtSPP.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

/*===========================================================================

Function:  OEMBTExtSPP_Init()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.

Parameters:
   IBTExtSPP *pParent: Pointer to the IBTExtSPP allocated by AEE

Return Value:  AEE_SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates app-associated memory for this object

===========================================================================*/
int OEMBTExtSPP_Init( IBTExtSPP* po );

// IBase (see AEE.h)
int OEMBTExtSPP_Release( IBTExtSPP* po );

// IBTEXTSPP_Open (see AEEBTExtSPP.h)
int    OEMBTExtSPP_Open( IBTExtSPP* pMe, 
                         const AEEBTSppOpenConfig* pOpenCfg );

// IBTEXTSPP_IOCtl (see AEEBTExtSPP.h)
int    OEMBTExtSPP_IOCtl( IBTExtSPP* pMe, AEEBTIOCtlCommand cmd, 
                          AEEBTIoCtlParam* pParam );

// IBTEXTSPP_Read (see AEEBTExtSPP.h)
int32  OEMBTExtSPP_Read( IBTExtSPP* po, char* pDst, int32 uMaxBytes );

// IBTEXTSPP_Readable (see AEEBTExtSPP.h)
void   OEMBTExtSPP_Readable( IBTExtSPP* po, AEECallback* pCb );

// IBTEXTSPP_Write (see AEEBTExtSPP.h)
int32  OEMBTExtSPP_Write( IBTExtSPP* po, char* pSrc, int32 uNumBytes );

// IBTEXTSPP_Writeable (see AEEBTExtSPP.h)
void   OEMBTExtSPP_Writeable( IBTExtSPP* po, AEECallback* pCb );

// IBTEXTSPP_Close (see AEEBTExtSPP.h)
int    OEMBTExtSPP_Close( IBTExtSPP* pMe );

int OEMBTExtSPP_GetOEMHandle(IBTExtSPP *pParent, int32 *pnHandle);

int OEMBTExtSPP_Clone(IBTExtSPP *pParent, int32 nHandle);

int OEMBTExtSPP_GetuID(IBTExtSPP *pParent, uint16 *puID);

int OEMBTExtSPP_GetStatus(IBTExtSPP *pParent, AEEBTSppStatus *pssStatus);

#endif /* _OEMBTEEXTSPP_H_ */
