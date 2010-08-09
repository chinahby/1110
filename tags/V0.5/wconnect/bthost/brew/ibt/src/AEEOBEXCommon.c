/*======================================================
FILE:  AEEOBEXCommon.c

SERVICES:  Implementation of Common OBEX functionality

GENERAL DESCRIPTION:


===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEOBEXCommon.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "AEEOBEXDefs_priv.h"
#include "OEMHeap.h"
#include "AEEStdErr.h"

static int AEEOBEX_VerifyBdAddrStr(const char *string);
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXEvtQ_Init(AEEOBEXEvtQueue *pEvtQ, int nMaxCnt, int nMaxEvtSize)
{
   int ret = AEE_SUCCESS;

   pEvtQ->nIndRead  = 0;
   pEvtQ->nIndWrite = 0;
   pEvtQ->nMaxCnt   = nMaxCnt;
   pEvtQ->nMaxEvtSize = nMaxEvtSize;

   if (0 != nMaxCnt)
   {
      pEvtQ->pBuf = OEM_Malloc(nMaxEvtSize * nMaxCnt); //#check, take a param for app memory
      if (NULL == pEvtQ->pBuf)
      {
         ret = AEE_ENOMEMORY;
      }
   }
   else
   {
      ret = AEE_EFAILED;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================


int AEEOBEXEvtQ_Put(AEEOBEXEvtQueue *pEvtQ, uint8 *pEvt)
{
   int nIndNextFree = 0;
   int ret = AEE_SUCCESS;

   uint8 *pBuf;

   // copy the contents
   pBuf = pEvtQ->pBuf + (pEvtQ->nIndWrite * pEvtQ->nMaxEvtSize);

   MEMCPY((void*)pBuf, pEvt, pEvtQ->nMaxEvtSize);

   // check if there is space in the queue before incrementing the index
   nIndNextFree = (pEvtQ->nIndWrite + 1) % pEvtQ->nMaxCnt;

   if (nIndNextFree == pEvtQ->nIndRead)
   {
      MSG_ERROR(" OBEX EVT Queue full: Dropping events ",0,0,0); 
      ret = AEE_EFAILED;
   }
   else
   {
      pEvtQ->nIndWrite = nIndNextFree;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEXEvtQ_Get(AEEOBEXEvtQueue *pEvtQ, uint8 *pEvt )
{
   int ret = AEE_ENOMORE;
   uint8 *pBuf = NULL;

   if ( pEvtQ->nIndWrite != pEvtQ->nIndRead)
   {
      // copy the contents
      pBuf = pEvtQ->pBuf + (pEvtQ->nIndRead * pEvtQ->nMaxEvtSize);

      MEMCPY((void*)pEvt, (void*)pBuf, pEvtQ->nMaxEvtSize);

      MEMSET((void*)pBuf, 0, pEvtQ->nMaxEvtSize);

      pEvtQ->nIndRead = (pEvtQ->nIndRead + 1) % pEvtQ->nMaxCnt;
      ret = AEE_SUCCESS;
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEXEvtQ_Free(AEEOBEXEvtQueue *pEvtQ)
{
   if (NULL != pEvtQ->pBuf)
   {
      OEM_Free(pEvtQ->pBuf);
      pEvtQ->pBuf      = NULL;
      pEvtQ->nIndRead  = 0;
      pEvtQ->nIndWrite = 0;
      pEvtQ->nMaxCnt   = 0;
   }
   return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

boolean AEEOBEX_VerifyPacketSize(int nMaxPacket, OEMOBEXHdr *pOemHdr, boolean bSinglePacket)
{
   return TRUE ; //#check
}

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXStrTok_Init(AEEOBEXStrToken* pStrParser, const char* str, const char delimiters)
{
   if (NULL == str)
   {
     MSG_ERROR("AEE OBEX Srv String Parser,NULL string str %x ",str,0,0);
     return AEE_EFAILED;
   }

   if (STRLEN(str) >= AEEOBEX_MAX_STR_LEN)
   {
     MSG_ERROR("AEE OBEX String Parser, buffer insufficient",0,0,0);
     return AEE_ENOMEMORY;
   }
   pStrParser->pOrigStr = str;
   pStrParser->delimiter = delimiters;
   AEEOBEXStrTok_Reset(pStrParser);

   return AEE_SUCCESS;
}

//=============================================================================
// 
//
//=============================================================================

int AEEOBEXStrTok_Reset(AEEOBEXStrToken* pStrParser)
{
   MEMSET(pStrParser->token,0,AEEOBEX_MAX_STR_LEN);
   STRLCPY(pStrParser->token, pStrParser->pOrigStr, AEEOBEX_MAX_STR_LEN);
   pStrParser->pTok = &pStrParser->token[0];
   return TRUE;
}
//=============================================================================
// 
//
//=============================================================================
char* AEEOBEXStrTok_GetNext(AEEOBEXStrToken* pStrParser)
{
   char *pChar = NULL;
   char *pDelim = NULL;

   if (*pStrParser->pTok != '\0')
   {
      // save the current token pointer
      pChar = pStrParser->pTok;

      // find the next occurence of the delimiter
      pDelim = STRCHR(pStrParser->pTok, pStrParser->delimiter);
      if (pDelim != NULL)
      {
         // if not null
         *pDelim = '\0';
         pStrParser->pTok = pDelim + 1;
      }
      else
      {
         // no delimiter, so pTok should point to end of string
         pStrParser->pTok = &pStrParser->token[AEEOBEX_MAX_STR_LEN - 1];
      }

   }
   return pChar;
}

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXTok_GetBdAddr(const char *string, uint8* pBdAddr)
{
   char num[3];
   uint32  i = 0;
   int ret = AEE_EBADPARM;
   char *pBda = NULL;
   int len = AEEOBEX_MAX_BDA_LEN - 1;

   pBda = STRCHR(string, '=');

   if (pBda)
   {
      pBda++;
      ret = AEEOBEX_VerifyBdAddrStr(pBda);
      if (AEE_SUCCESS == ret)
      {
         for ( i = 0; i < AEEOBEX_MAX_BDA_LEN; i++ )
         {
            num[2] = 0;
            MEMCPY(&num[0], &pBda[i*2], 2);
            pBdAddr[len - i] = (uint8)STRTOUL(num,NULL,16);
         }
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
static int AEEOBEX_VerifyBdAddrStr(const char *string)
{
   int ret = AEE_EBADPARM;
   int len = STRLEN(string);

   if (len == (AEEOBEX_MAX_BDA_LEN * 2)) 
   {
      ret = AEEOBEX_IsDigitStr(string, AEEOBEX_MAX_BDA_LEN * 2);
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEX_IsDigitStr(const char *string, int maxLen)
{
   int ret = AEE_SUCCESS;
   int len = STRLEN(string);

   if ((len > maxLen) || (0 == len))
   {
      ret = AEE_EBADPARM;
   }
   else
   {
      int i = 0;
      for (i = 0; i < len; i++)
      {
         char c = string[i];
         if (!( ((c >= '0') && (c <= '9')) ||
            ((c >= 'a') && (c <= 'f')) ||
            ((c >= 'A') && (c <= 'F'))))
         {
            ret = AEE_EBADPARM;
            break;
         }
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================
int AEEOBEXTok_GetScn(const char *pszTok, uint8 *pChannel)
{
   char *pScn = NULL;
   int ret = AEE_EBADPARM;
   
   pScn = STRCHR(pszTok, '=');

   if (pScn)
   {
      pScn++;

      ret = AEEOBEX_IsDigitStr(pScn,8);
      if (AEE_SUCCESS == ret)
      {
         *pChannel = (uint8)STRTOUL(pScn,NULL,16); 

         if ( *pChannel > 31 )
         {
            ret = AEE_EBADPARM;
         }
      }
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXTok_GetSecurity(const char *pszTok, uint32 *pSec)
{
   char *pStr = NULL;
   int ret = AEE_EBADPARM;

   pStr = STRCHR(pszTok, '=');

   if (pStr)
   {
      pStr++;
      ret = AEEOBEX_IsDigitStr(pStr,8);
      if (AEE_SUCCESS == ret)
      {
         *pSec = STRTOUL(pStr,NULL,16); 
      }
   }
   return ret;
}

//=============================================================================
// 
//
//=============================================================================
int AEEOBEXTok_GetUuid(const char *pszTok, uint32 *pUuid)
{
   char *pStr = NULL;
   int ret = AEE_EBADPARM;

   pStr = STRCHR(pszTok, '=');

   if (pStr)
   {
      pStr++;
      ret = AEEOBEX_IsDigitStr(pStr,8);
      if ( AEE_SUCCESS == ret)
      {
         *pUuid = (uint16)STRTOUL(pStr,NULL,16); 
      }
   }
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

int AEEOBEX_GetSupportedTransports( uint32 suppTransport,
                                    AEEOBEXTransport *pTransport,
                                    int nLen,
                                    int *pnLenReq)
{
   int ret = AEE_SUCCESS;
   int cnt = 0;

   // add more transport entries here
   //AEEOBEXTransTable trans[] ={{OEMOBEX_TRANS_BT_RFCOMM, AEEOBEX_TRANS_BT_RFCOMM}};

   cnt = AEEOBEXGet_TransportCnt(suppTransport);

   if ((NULL != pnLenReq) &&
       (AEE_CHECK_PTR("AEEOBEX_GetSupportedTransports",
                        pnLenReq, sizeof(int), TRUE)))
   {
      // return the actual number of transports available
      *pnLenReq = cnt;
   }


   if (nLen > 0)
   {
      if ((NULL  == pTransport) ||
          (!AEE_CHECK_PTR("AEEOBEX_GetSupportedTransports",
                        pTransport, sizeof(AEEOBEXTransport) * nLen, TRUE)))
      {
         ret = AEE_EBADPARM;
      }
      else
      {
         pTransport[0] = AEEOBEX_TRANS_BT_RFCOMM; //#check
      }
   }
   else
   {
      ret = AEE_EBUFFERTOOSMALL;
   }
   return ret;
}



//=============================================================================
// 
//
//=============================================================================
int AEEOBEXGet_TransportCnt(uint32 bitMap)
{
  int nCnt = 0;
  uint32 trans = bitMap;

  for (nCnt = 0; trans != 0; trans = trans & (trans -1), nCnt++);

  return nCnt;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */

