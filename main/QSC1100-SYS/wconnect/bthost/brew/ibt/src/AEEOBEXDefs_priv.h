#ifndef AEEOBEXDEFS_PRIV_H
#define AEEOBEXDEFS_PRIV_H
/*=============================================================================
FILE:  AEEOBEXDefs_priv.h

SERVICES:  AEE OBEX private defines

GENERAL DESCRIPTION:

        Copyright © 2004-2008, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEOBEXDefs_priv.h#1 $
$DateTime: 2009/01/07 18:14:54 $

when       who  what, where, why
--------   ---  ---------------------------------------------------------------
09/15/08   sp  Compiler warning fix

===========================================================================*/


#include "AEEIOBEXHeader.h"
#include "AEEIOBEXSrvSession.h"
#include "OEMOBEXDefs.h"

//=============================================================================
// 
//
//=============================================================================
#define AEEOBEX_MAX_SEND_BUFF    2
#define AEE_OBEX_DEFAULT_MAX_PACKET      16000
#define AEEOBEX_MAX_EVENTS   5
#define AEEOBEX_MAX_STR_LEN 500
#define AEEOBEX_MAX_BDA_LEN   6

#define RELEASEIF(x) if (x) { IQI_Release((IQI *)(x)); (x) = 0; }

#ifdef MSG_ERROR 
   #undef MSG_ERROR
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif

#define DBGPRINTF_DEBUG  DBGPRINTF

#define FARF_ERROR   1
#define FARF_HIGH    1
#define FARF_MED     0
#define FARF_LOW     0

#define OBEX_DEBUG 1
#if defined OBEX_DEBUG
#define FARF_DEBUG   1
#else
#define FARF_DEBUG   0
#endif

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF_##x p 

#define MSG_DEBUG( sf, a, b, c )    FARF(DEBUG, (sf, a, b, c))
#define MSG_ERROR( sf, a, b, c )    FARF(ERROR, (sf, a, b, c))
#define MSG_HIGH( sf, a, b, c )     FARF(ERROR, (sf, a, b, c))
#define MSG_MED( sf, a, b, c )      FARF(ERROR, (sf, a, b, c))
#define MSG_LOW( sf, a, b, c )      FARF(ERROR, (sf, a, b, c))





//=============================================================================
// 
//
//=============================================================================
typedef struct 
{
   uint8       *pBuf;
   int         nIndRead;
   int         nIndWrite;
   int         nMaxCnt;
   int         nMaxEvtSize;
}AEEOBEXEvtQueue;

//=============================================================================
// 
//
//=============================================================================

typedef struct
{
  char   token[AEEOBEX_MAX_STR_LEN]; // tokenised string where delimiters replaced with \0
  const  char   *pOrigStr; // reference to original string
  char   *pTok;            // pointer to the next token
  char   delimiter;
} AEEOBEXStrToken;


typedef int (*PFNOBEXPARSE)(const char *string, void* data, void* data2);

typedef struct 
{
   char *tok;
   PFNOBEXPARSE pfnParse;
}AEEOBEXTokTable;

typedef struct 
{
   uint32 bitMask;
   AEEUID uid;
}AEEOBEXTransTable;

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXEvtQ_Init(AEEOBEXEvtQueue *pEvtQ, int nMaxCnt, int nMaxEvtSize);
int AEEOBEXEvtQ_Put(AEEOBEXEvtQueue *pEvtQ, uint8 *pEvt);
int AEEOBEXEvtQ_Get(AEEOBEXEvtQueue *pEvtQ, uint8 *pEvt );
int AEEOBEXEvtQ_Free(AEEOBEXEvtQueue *pEvtQ);
int AEEOBEX_GetSupportedTransports( uint32 suppTransport,
                                    AEEOBEXTransport *pTransport,
                                    int nLen,
                                    int *pnLenReq);
int AEEOBEXHeader_GetObject(IShell* pIShell,
                            void **ppiHdr,
                            OEMOBEXHdr *pOemHdr,
                            boolean bWritable);
boolean AEEOBEX_VerifyPacketSize(int nMaxPacket, OEMOBEXHdr *pOemHdr, boolean bSinglePacket);

boolean AEEOBEX_BufAvailable(OEMOBEXHdr *pBufList, int nSize);

int AEEOBEXStrTok_Reset(AEEOBEXStrToken* pStrParser);
int AEEOBEXStrTok_Init(AEEOBEXStrToken* pStrParser, const char* str, const char delimiters);
int AEEOBEXStrTok_Reset(AEEOBEXStrToken* pStrParser);
char* AEEOBEXStrTok_GetNext(AEEOBEXStrToken* pStrParser);
int AEEOBEXTok_GetBdAddr(const char *pszTok, uint8* pBdAddr);
int AEEOBEXTok_GetScn(const char *pszTok, uint8 *pChannel);
int AEEOBEXTok_GetSecurity(const char *pszTok, uint32 *pSec);
int AEEOBEXTok_GetUuid(const char *pszTok, uint32 *pUuid);
int AEEOBEXGet_TransportCnt(uint32 bitMap);
int AEEOBEXHdr_MarkReadOnly(IOBEXHeader *pHdr);
OEMOBEXHdr* AEEOBEXHdr_GetOemHdr(IOBEXHeader *pHdr);
int AEEOBEX_IsDigitStr(const char *string, int maxLen);      

#endif /* AEEOBEXDEFS_PRIV_H */

