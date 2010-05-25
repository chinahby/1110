#ifndef _OEMOBEX_H_
#define _OEMOBEX_H_

/*===========================================================================

FILE:      OEMOBEX.h

SERVICES:  OBEX BREW extension

GENERAL DESCRIPTION: BREW OEM interface to the OBEX Profile (MT-OBEX)

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright © 2004 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMOBEX.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
02/28/07   MH      Added support for disconnect functionality.
01/03/07   MH      Notification event free header function prototype added
10/26/06   MH      modified server confirm set path to support all headers
9/19/06    MH      Server Authenticate Command added.
8/25/06    bh      compile errors fixed
7/28/06    bh      modified for obex client authentication

6/23/06    jh      modified definition of IOBEXClient_Disconnect, SetPath,
                   IOBEXServer_AcceptConnect and Register to support JSR82.
3/24/04    sp      corrected featurization so that header file is not featurized
11/13/04   sp      initial checkin. unit tested for IOBEXServer API

=============================================================================*/


#include "AEEOBEX.h"
#include "BREWVersion.h"



//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================


//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMOBEX_Init()

Description:
   This is called from the AEE layer when an app tries to create an instance
   of a class ID associated with this module.

Parameters:
   IOBEX *pParent: Pointer to parent structure allocated by AEE layer

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates memory for object.

===========================================================================*/
//called from AEEOBEX_ServerInitialize()
int OEMOBEX_Initialize( IOBEX* pParent );

//IBase (see AEE.h)
uint32 OEMOBEX_Release(IOBEX *pParent);
uint32 OEMOBEX_AddRef(IOBEX *pParent);

int OEMOBEX_ServerRegister(IOBEX *p, const AEEOBEXRegistrationInfoType *pReg,
                           uint8* pSrvChNum);

//IOBEX_ServerDeregister (see AEEOBEX.h)
int OEMOBEX_ServerDeregister(IOBEX *p);


int OEMOBEX_ServerAcceptConnection(IOBEX *po,
                                   boolean bAccept, AEEOBEXHdrType *headersPtr,
                                   uint8 headersCnt, 
                                   AEEOBEXCmdStatusType cmdStatus);

int OEMOBEX_ServerConfirmSetPath(IOBEX *po, AEEOBEXHdrType *headersPtr,
                                 uint8   headersCnt,
                                 AEEOBEXCmdStatusType cmdStatus);

int OEMOBEX_ServerPutResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                              uint8   headersCnt,
                              AEEOBEXCmdStatusType cmdStatus);

int  OEMOBEX_ServerGetResponse(IOBEX *po, AEEOBEXHdrType    *headersPtr,
                               uint8   headersCnt,
                               AEEOBEXCmdStatusType cmdStatus);

int OEMOBEX_ServerDisconnectResponse(IOBEX *po, AEEOBEXHdrType *headersPtr,
                                     uint8   headersCnt,
                                     AEEOBEXCmdStatusType cmdStatus);

int OEMOBEX_ServerForceDisconnect(IOBEX *po);

int OEMOBEX_ServerAuthenticate(IOBEX *po,
                               AEEOBEXDeviceAddressType *pAddr,
                               AEEOBEXRealm *realmInfo,
                               boolean bUidReqd,
                               boolean bFullAccess);

int OEMOBEX_ClientConnect(IOBEX *po, const AEEOBEXDeviceAddressType *pAddr,
                          AEEOBEXHdrType  *headersPtr,
                          uint8            headersCnt);

int OEMOBEX_ClientDisconnect(IOBEX *po, AEEOBEXHdrType *headersPtr,
                             uint8 headersCnt);

int OEMOBEX_ClientPut(IOBEX *po, AEEOBEXHdrType  *headersPtr,
                      uint8  headersCnt, AEEOBEXCmdStatusType cmdStatus);

int OEMOBEX_ClientGet(IOBEX *po, AEEOBEXHdrType  *headersPtr,
                      uint8  headersCnt,
                      boolean final);

int OEMOBEX_ClientSetPath(  IOBEX *po, const AECHAR *pwPathName,
                            boolean upLevel, boolean dontCreate, 
                            AEEOBEXHdrType *headersPtr, uint8 headersCnt);
int OEMOBEX_ClientAbortOperation(IOBEX *po);

int OEMOBEX_AuthResponse(IOBEX *po , const char *pwUserID, const char *pwPassword);

int OEMOBEX_GetSupportedTransports(IOBEX *po);

void OEMOBEX_ClientInitData(IOBEX *po, uint16 index);


int OEMOBEX_ClientAuthenticate(IOBEX *po, 
                               AEEOBEXDeviceAddressType *pAddr,
                               AEEOBEXRealm *realmInfo,
                               boolean bUidReqd,
                               boolean bFullAccess);

void OEMOBEX_FreeAppNotificationEventHeaders(uint32 uID, OBEXNotificationDataType *notifyData);

#endif //_OEMOBEX_H_
