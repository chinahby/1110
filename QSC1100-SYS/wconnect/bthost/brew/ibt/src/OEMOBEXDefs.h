#ifndef OEMOBEXDEFS_H
#define OEMOBEXDEFS_H

/*=============================================================================
FILE:  OEMOBEXDefs.h

SERVICES:  Exports functions/ data types that are invoked by the 
           AEE layer 

GENERAL DESCRIPTION:
      
        Copyright © 2004-2008, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXDefs.h#1 $
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

===========================================================================*/

#include "AEEStdLib.h"
#include "AEEOBEXComDefs.h"

//=============================================================================
// 
//
//=============================================================================

typedef uint16 OEMOBEX_SdUuid ; //#check

#define OEMOBEXSDUUID_INVALID 0xFFFF

typedef struct OEMOBEXSrv OEMOBEXSrv;
typedef struct OEMOBEXHdr OEMOBEXHdr;
typedef struct OEMOBEXCli OEMOBEXCli;

#define OEMOBEX_MAX_SERVICE_NAME_LEN    80
#define OEMOBEX_MAX_RECEIVE_BUFF        2
#define OEMOBEX_MAX_REALM_LEN           240
#define OEMOBEX_MAX_UID_LEN             20
#define OEMOBEX_BDADDR_LEN              6


#define OEMOBEX_EVT_SRV_DISC_COMPL     0x60
#define OEMOBEX_EVT_SRV_GET_COMPL      0x61
#define OEMOBEX_EVT_SRV_PUT_COMPL      0x62
#define OEMOBEX_EVT_SRV_SETPATH_COMPL  0x63    // these are events in addition to 
                                               // those defined in AEEOBEXComDefs.h

//=============================================================================
// 
//
//=============================================================================
#define OEMOBEX_TRANS_BT_RFCOMM    0x01


typedef uint32 OEMOBEX_TransportType;

//=============================================================================
// 
//
//=============================================================================
#define OEMOBEX_BT_SEC_NONE                        0x00
#define OEMOBEX_BT_SEC_AUTHENTICATE                0x01
#define OEMOBEX_BT_SEC_AUTHENTICATE_AND_ENCRYPT    0x02
#define OEMOBEX_BT_SEC_MAX                         0x03

typedef uint32 OEMOBEX_BtSecurity;
//=============================================================================
// 
//
//=============================================================================
#define OEMOBEX_SRV_AUTH_NONE         0x0
#define OEMOBEX_SRV_AUTH_PASSWORD     0x1
#define OEMOBEX_SRV_AUTH_USERID_AND_PASSWORD 0x2

typedef uint8 OEMOBEX_AuthType;

//=============================================================================
// 
//
//=============================================================================

typedef struct 
{
   boolean            bUserIdReq;
   boolean            bFullAccess;
   AEEOBEXCharSet     charSet;
   uint8              aRealm[OEMOBEX_MAX_REALM_LEN];
   int                nRealmLen;
}OEMOBEXAuthInfo;

//=============================================================================
// 
//
//=============================================================================
typedef struct
{
   boolean            bFullAccess;
   int                nUidLen;
   uint8              aUserId[OEMOBEX_MAX_UID_LEN];
}  OEMOBEXAuthResp;


//=============================================================================
// For Client Connection Purposes
//
//=============================================================================
typedef struct
{
    uint8                   bdAddr[OEMOBEX_BDADDR_LEN];
    uint8                   channel;
    OEMOBEX_SdUuid          uuid; 
    OEMOBEX_BtSecurity      security;
} OEMOBEX_BtRfcommConn;


typedef struct
{
    uint32 irdaDeviceAddr;
    const uint8 *iasQuery;
    uint8 iasQueryLen;
} OEMOBEX_IrdaConn;


typedef union
{
    OEMOBEX_BtRfcommConn    btConn;
    OEMOBEX_IrdaConn        irdaConn;
} OEMOBEX_TransportConn;

typedef struct
{
    OEMOBEX_TransportType transport;
    OEMOBEX_TransportConn transportConn;
} OEMOBEX_TransportConnInfo;


//=============================================================================
// For Server Registration Purposes
//
//=============================================================================
typedef struct
{
    OEMOBEX_SdUuid       uuid;
    OEMOBEX_BtSecurity   security;
    uint8                serviceName[OEMOBEX_MAX_SERVICE_NAME_LEN + 1];
} OEMOBEX_BtRfcommConfig;

typedef union
{
    OEMOBEX_BtRfcommConfig  btConfig;
} OEMOBEX_TransportConfig;

typedef struct
{
    OEMOBEX_TransportType   transport;
    int                     nTargetLen;
    OEMOBEX_AuthType        auth;

    uint8                   *pTargetId;
    OEMOBEX_TransportConfig transportConfig;
} OEMOBEX_TransportConfigInfo;


//=============================================================================
// 
//
//=============================================================================
typedef struct {
   uint32      status;
   uint32      flags;    // for set path
   uint16      connId;
   uint16      targetId;
   boolean     bFinal;
   OEMOBEXHdr  *pOemHdr;
}OEMOBEXSrvEvt;

typedef struct {
   uint32     status;
   OEMOBEXHdr *pOemHdr;
}OEMOBEXCliEvt;

typedef struct {
   uint8  event;
   union
   {
      OEMOBEXSrvEvt srv;
      OEMOBEXCliEvt cli;
   }u;
}OEMOBEXEvent;

//=============================================================================
// 
//
//=============================================================================
typedef void (*PFNOBEXEVTCB)( void            *cbData,
                              OEMOBEXEvent    *pEvt,
                              OEMOBEXAuthInfo *pAuthChal,
                              OEMOBEXAuthResp *pAuthResp);

   
void OEMOBEX_GetSupTransport(uint32 *bitmap);
int  OEMOBEXSrv_init(OEMOBEXSrv **pOemObj, void *cbData, PFNOBEXEVTCB eventCb);
int OEMOBEXSrv_Register( OEMOBEXSrv *me, const OEMOBEX_TransportConfigInfo *pRegInfo);
int OEMOBEXCli_Init(OEMOBEXCli **pOemobj, void* cbData, PFNOBEXEVTCB aeeEvtCb);
int OEMOBEXCli_Connect(OEMOBEXCli *pOemObj, OEMOBEXHdr *pOemHdr, OEMOBEXAuthInfo *pAuthInfo);
int OEMOBEXCli_Release(OEMOBEXCli *pOemObj);
int OEMOBEXCli_SetTransport(OEMOBEXCli *pOemObj, OEMOBEX_TransportConnInfo *pConnInfo);
int OEMOBEXCli_AuthResponse(OEMOBEXCli *me,
                            const uint8 *pUserID,
                            int         uidLen,
                            const uint8 *pPassword,
                            int        pswdLen);
int OEMOBEXCli_Put(OEMOBEXCli      *me,
                   OEMOBEXHdr      *pOemHdr,
                   boolean         bfinal,
                   OEMOBEXAuthInfo *pAuthInfo);
int OEMOBEXCli_Get(OEMOBEXCli      *me,
                   OEMOBEXHdr      *pOemHdr,
                   boolean         bfinal,
                   OEMOBEXAuthInfo *pAuthInfo);
int OEMOBEXCli_SetPath(OEMOBEXCli      *me,
                       OEMOBEXHdr      *pOemHdr,
                       boolean         upLevel,
                       boolean         dontCreate,
                       OEMOBEXAuthInfo *pAuthInfo);
int OEMOBEXCli_Abort(OEMOBEXCli *me, OEMOBEXHdr *pOemHdr);

int OEMOBEXCli_Disconnect(OEMOBEXCli *me, OEMOBEXHdr *pOemHdr);
boolean OEMOBEXCli_RecvHdrAvailable(OEMOBEXCli *me);
//=============================================================================
// 
//
//=============================================================================

typedef struct 
{
   uint32 headerId;
   int    nLen;
}OEMOBEXHdrNode;
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXHeader_Init(OEMOBEXHdr **pOemObj, int nSize);
int OEMOBEXHdr_GetCount(OEMOBEXHdr *pOemObj);
int OEMOBEXHdr_GetList(OEMOBEXHdr *pOemObj, OEMOBEXHdrNode *pHead, int nCnt);
int OEMOBEXHdr_AddString(OEMOBEXHdr *pHdr, uint8 headerId, const uint16 *pwSeq, int nwSeqLen);
int OEMOBEXHdr_AddU32(OEMOBEXHdr *pHdr, uint8 headerId, uint32 data);
int OEMOBEXHdr_AddByteSeq(OEMOBEXHdr *pHdr, uint8 headerId, const uint8 *data, int nLen);
int OEMOBEXHdr_AddByte(OEMOBEXHdr *pHdr, uint8 headerId, uint8 data);
int OEMOBEXHeader_GetHeader(OEMOBEXHdr *pOemHdr, int nIndex, int nOffset,
                            void *pData, int nLen);
int OEMOBEXHdr_AddRef(OEMOBEXHdr *pOemHdr);
boolean OEMOBEXHdr_IsFree(OEMOBEXHdr *me);
void OEMOBEXHdr_Release(OEMOBEXHdr *pHdr);
int OEMOBEXHdr_MaxBufSize(OEMOBEXHdr *pHdr);

//=============================================================================
// 
//
//=============================================================================
int  OEMOBEXSrv_Release(OEMOBEXSrv *me);
int OEMOBEXSrv_DeRegister(OEMOBEXSrv *me);

int OEMOBEXSrv_Authenticate(OEMOBEXSrv     *me,
                           OEMOBEXAuthInfo *pAuth,
                           uint16      connId);
int OEMOBEXSrv_ConnectResp(OEMOBEXSrv  *me,
                           uint16      connId,
                           OEMOBEXHdr  *pOemHdr,
                           uint32      cmdStatus);
int OEMOBEXSrv_PutResp(OEMOBEXSrv *me,
                       uint16 connId,
                       OEMOBEXHdr *pOemHdr,
                       uint32 cmdStatus);

int OEMOBEXSrv_GetResp(OEMOBEXSrv *me,
                       uint16 connId,
                       OEMOBEXHdr *pOemHdr,
                       uint32 cmdStatus);

int OEMOBEXSrv_SetPathResp(OEMOBEXSrv *me,
                           uint16 connId,
                           OEMOBEXHdr *pOemHdr,
                           uint32 cmdStatus);
int OEMOBEXSrv_DiscResp(OEMOBEXSrv *me,
                        uint16 connId,
                        OEMOBEXHdr *pOemHdr,
                        uint32 cmdStatus);


int OEMOBEXSrv_Disconnect(OEMOBEXSrv *me,
                          uint16     connId);

int OEMOBEXSrv_AuthResponse(OEMOBEXSrv  *me,
                            const uint8 *pUserID,
                            int         uidLen,
                            const uint8 *pPassword,
                            int         pswdLen);
boolean OEMOBEXSrv_RecvHdrAvailable(OEMOBEXSrv *me);

int OEMOBEXSrv_GetBda(OEMOBEXSrv *me, uint8* pBda);
int OEMOBEXSrv_GetScn(OEMOBEXSrv *me, uint32 * pScn);
int OEMOBEXCli_GetPacketSize(OEMOBEXCli *me, uint32 *pPacketSize);
int OEMOBEXSrv_GetPacketSize(OEMOBEXSrv *me, uint32 *pPacketSize);
int OEMOBEXCli_GetScn(OEMOBEXCli *me, uint32 * pScn);
int OEMOBEXCli_GetBda(OEMOBEXCli *me, uint8* pBda);

#endif /* OEMOBEXDEFS_H */

