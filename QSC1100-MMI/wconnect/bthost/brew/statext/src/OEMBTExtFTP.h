#ifndef _OEMBTEXTFTP_H_
#define _OEMBTEXTFTP_H_

#if defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP)
/*===========================================================================

FILE:      OEMBTExtFTP.h

SERVICES:  BlueTooth File Transfer Profile QCT extension

GENERAL DESCRIPTION: BREW OEM interface to the Bluetooth File Transfer Profile

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004-2005 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtFTP.h#2 $
$DateTime: 2009/04/17 11:28:04 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtFTP_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IBTExtFTP *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
int OEMBTExtFTP_Init( IBTExtFTP* pParent );

// IBase (see AEE.h)
uint32 OEMBTExtFTP_Release( IBTExtFTP* pParent );

// IBTEXTFTP_Register (see AEEBTExtFTP.h)
int OEMBTExtFTP_Register( IBTExtFTP*                  pParent, 
                          const AEEBTFTPRegistration* pReg );

// IBTEXTFTP_AuthData (see AEEBTExtFTP.h)
int OEMBTExtFTP_AuthData( IBTExtFTP *pParent,
                          const AEEBTFTPRegistration* pReg );

// IBTEXTFTP_Deregister (see AEEBTExtFTP.h)
int OEMBTExtFTP_Deregister( IBTExtFTP* pParent );

// IBTEXTFTP_AcceptConnection (see AEEBTExtFTP.h)
int OEMBTExtFTP_AcceptConnection( IBTExtFTP*         pParent, 
                                  const AEEBTBDAddr* pBDAddr, 
                                  boolean            bAccept,
                                  const AECHAR*      pwRootFolder );

// IBTEXTFTP_SendResult (see AEEBTExtFTP.h)
int OEMBTExtFTP_SendResult( IBTExtFTP*          pParent,
                            AEEBTFTPRequestType reqType, 
                            AEEBTFTPStatus      result,
                            const AECHAR*       pwName );

// IBTEXTFTP_Connect (see AEEBTExtFTP.h)
int OEMBTExtFTP_Connect( IBTExtFTP*         pParent,
                         const AEEBTBDAddr* pBDAddr, 
                         uint8              uChannelNumber );

// IBTEXTFTP_Abort (see AEEBTExtFTP.h)
int OEMBTExtFTP_Abort( IBTExtFTP* pParent );

// IBTEXTFTP_SetPath (see AEEBTExtFTP.h)
int OEMBTExtFTP_SetPath( IBTExtFTP*          pParent, 
                         const AECHAR*       pwPathName,
                         AEEBTFTPSetPathFlag flag );

// IBTEXTFTP_CreateFolder (see AEEBTExtFTP.h)
int OEMBTExtFTP_CreateFolder( IBTExtFTP*    pParent, 
                              const AECHAR* pwDirName );

// IBTEXTFTP_ListFolder (see AEEBTExtFTP.h)
int OEMBTExtFTP_ListFolder( IBTExtFTP*    pParent, 
                            const AECHAR* pwDirName,
                            const AECHAR* pwDirList );

// IBTEXTSD_Put (see AEEBTExtFTP.h)
int OEMBTExtFTP_Put( IBTExtFTP*     pParent, 
                     AEEBTFTPObject objType,
                     const AECHAR*  pwLocalName,
                     const AECHAR*  pwRemoteName );

// IBTEXTSD_Pull (see AEEBTExtFTP.h)
int OEMBTExtFTP_Get( IBTExtFTP*     pParent, 
                     AEEBTFTPObject objType,
                     const AECHAR*  pwLocalName,
                     const AECHAR*  pwRemoteName );

// IBTEXTFTP_Delete (see AEEBTExtFTP.h)
int OEMBTExtFTP_Delete( IBTExtFTP*    pParent, 
                        const AECHAR* pwRemoteName );

// IBTEXTFTP_Authenticate (see AEEBTExtFTP.h)
int OEMBTExtFTP_Authenticate( IBTExtFTP*    pParent, 
                              const AEEBTBDAddr* pBDAddr, 
                              const AECHAR* pwUserID,
                              const AECHAR* pwPassword );

// IBTEXTFTP_Disconnect (see AEEBTExtFTP.h)
int OEMBTExtFTP_Disconnect( IBTExtFTP* pParent );

// IBTEXTFTP_GetProgressInfo (see AEEBTExtFTP.h)
int OEMBTExtFTP_GetProgressInfo( IBTExtFTP* pParent,
                                 AEEBTProgressInfo* pProgressInfo );

#endif // defined(FEATURE_IBT) && defined(FEATURE_BT_EXTPF_FTP)
#endif // _OEMBTEXTFTP_H_
