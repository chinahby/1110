/*=============================================================================
FILE:  OEMOBEXHeader.c

SERVICES:  OEM implementation of OBEX header interface

GENERAL DESCRIPTION:

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/OEMOBEXHeader.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "OEMOBEXDefs.h"
#include "OEMOBEXDefs_priv.h"
#include "mt_obex.h"
#include "btpfcmdi.h"
#include "btpf.h"
#include "OEMHeap.h"
#include "AEEStdErr.h"
#include "AEE_OEMHeap.h"
#include "AEEIOBEXHeader.h"
#ifdef BT_SIMULATION
#include "btmsg.h"
#else
#include "msg.h"
#endif

//=============================================================================
// 
//
//=============================================================================
#define OEMOBEX_MAX_HEADER_CNT     15
#define OEMOBEX_MAX_HEADER_CNT_INC   6 // increment max count by this number
#define OEMOBEX_MAX_MT_HEADER_CNT  255 // this is because count in mt_obex_hdr_list_type
                                       // is uint8
#define OEMOBEX_HDR_ID_UNICODE   0x00  
#define OEMOBEX_HDR_ID_BYTESEQ   0x40  
#define OEMOBEX_HDR_ID_UINT8     0X80  
#define OEMOBEX_HDR_ID_UINT32    0xC0  

#define OEMOBEX_HDR_KIND(h)      ((h) & 0xC0)

//=============================================================================
//  OEM Obex Header
//============================================================================= 
struct OEMOBEXHdr
{
   mt_obex_hdr_list_type  hdrList;     // count and list of hdr nodes
   uint8                  *pBuf;       // buffer to store the actual data 
   int                    nWriteIndex; // index into pBuf where the next chunk 
                                       // can be written
   int                    nMaxPacket;  // max size of the OBEX packet that can carry these
                                       // headers
   int                   nMaxHdrCnt;   // max nodes in the hdrList 
   int                   uRef;
};
//=============================================================================
// 
//
//=============================================================================
static int  OEMOBEXHdr_ReAllocateList(OEMOBEXHdr *pHdr, int nCnt);
static int OEMOBEXHdr_MtToAeeHdrFree(uint8 hdrCnt,
                                     mt_obex_hdr_type *pMtHdr,
                                     boolean bFlagDisc,
                                     bt_app_id_type appId);

static rex_crit_sect_type   gOBEXHdrCritSect; 
//=============================================================================
// 
//
//=============================================================================
boolean OEMOBEXHdr_IsFree(OEMOBEXHdr *me)
{
   boolean bFree = FALSE;
   
   // this check can be made from two different task context 
   // thus needs to be within a critic section
   rex_enter_crit_sect(&gOBEXHdrCritSect);

   if (me->uRef == 1)
   {
      bFree = TRUE;
   }
   rex_leave_crit_sect(&gOBEXHdrCritSect);

   return bFree;
}

//=============================================================================
// 
//
//=============================================================================

int OEMOBEXHdr_AddRef(OEMOBEXHdr *me)
{
   
  // this api can be called from two different task context 
  // thus needs to be within a critic section
  rex_enter_crit_sect(&gOBEXHdrCritSect);
  
  me->uRef++;

  rex_leave_crit_sect(&gOBEXHdrCritSect);

  return AEE_SUCCESS;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXHdr_MaxBufSize(OEMOBEXHdr *pHdr)
{
   return pHdr->nMaxPacket;
}
//=============================================================================
// 
//
//=============================================================================

int OEMOBEXHeader_Init(OEMOBEXHdr **ppme, int nSize)
{
   int ret = AEE_ENOMEMORY;

   // allocate the oem object
   OEMOBEXHdr  *pHdr = NULL; 

   pHdr = (OEMOBEXHdr*)OEM_Malloc(sizeof(OEMOBEXHdr)); 

   if (NULL != pHdr)
   {
      MSG_DEBUG("OEM OBEX Hdr alloc hdr %x",pHdr,0,0);
      pHdr->hdrList.list_ptr = NULL;
      pHdr->hdrList.count    = 0;
      pHdr->nMaxPacket       = 0;
      pHdr->pBuf             = NULL;
      pHdr->nWriteIndex      = 0;
      pHdr->nMaxHdrCnt       = OEMOBEX_MAX_HEADER_CNT;
      pHdr->uRef             = 1;


      pHdr->hdrList.list_ptr = (mt_obex_hdr_type*)OEM_Malloc(
                      sizeof(mt_obex_hdr_type) * OEMOBEX_MAX_HEADER_CNT);

      if (NULL != pHdr->hdrList.list_ptr)
      {
         pHdr->pBuf = (uint8*)OEM_Malloc(nSize);

         if (NULL != pHdr->pBuf)
         {
             MEMSET(pHdr->pBuf, 0, nSize);
             MEMSET(pHdr->hdrList.list_ptr,
                    0,
                    sizeof(mt_obex_hdr_type) * OEMOBEX_MAX_HEADER_CNT);
             pHdr->nMaxPacket = nSize; 
             ret = AEE_SUCCESS;
         }
      }
   }
   // clean up if we arent able to allocate memory 
   if (AEE_SUCCESS != ret)
   {
      OEMOBEXHdr_Release(pHdr);
   }
   *ppme = pHdr;
   return ret;
}
//=============================================================================
// 
//
//=============================================================================

void OEMOBEXHdr_Release(OEMOBEXHdr *pHdr)
{
   if (NULL != pHdr)
   {
      rex_enter_crit_sect(&gOBEXHdrCritSect);

      pHdr->uRef--;

      if (0 == pHdr->uRef)
      {
         MSG_DEBUG("OEM OBEX Hdr release uref %x, hdr %x",pHdr->uRef,pHdr,0);
         if (NULL != pHdr->hdrList.list_ptr)
         {
            OEM_Free(pHdr->hdrList.list_ptr);
            pHdr->hdrList.list_ptr = NULL;
            pHdr->hdrList.count    = 0;
         }
         if(NULL != pHdr->pBuf)
         {
            OEM_Free(pHdr->pBuf);
            pHdr->pBuf = NULL;
         }
         OEM_Free(pHdr);
      }
      else if (1 == pHdr->uRef)
      {
         MSG_DEBUG("OEM OBEX Hdr release uref %x, hdr %x",pHdr->uRef,pHdr,0);
         pHdr->hdrList.count    = 0;
         pHdr->nWriteIndex      = 0;
         MEMSET(pHdr->pBuf, 0, pHdr->nMaxPacket);
         MEMSET(pHdr->hdrList.list_ptr,
                0,
                sizeof(mt_obex_hdr_type) * pHdr->nMaxHdrCnt);
      }

      rex_leave_crit_sect(&gOBEXHdrCritSect);
   }
   return;
}

//=============================================================================
//  To cater to interfaces exposed to the application
//
//=============================================================================
int OEMOBEXHdr_AddString(OEMOBEXHdr *pHdr, uint8 headerId, const uint16 *pwSeq, int nwSeqLen)
{
   mt_obex_hdr_type *pHdrNode = NULL;
   int ret = AEE_SUCCESS;

   if (pHdr->nMaxHdrCnt <= pHdr->hdrList.count)
   {
      ret = OEMOBEXHdr_ReAllocateList(pHdr, pHdr->nMaxHdrCnt + OEMOBEX_MAX_HEADER_CNT_INC);
   }

   if (AEE_SUCCESS == ret)
   {
      pHdrNode                 = &pHdr->hdrList.list_ptr[pHdr->hdrList.count];
      pHdrNode->id              = headerId;
       // len can be >= 0
      pHdrNode->val.unicode.len = nwSeqLen; 

      if (nwSeqLen > 0)
      {
         pHdrNode->val.unicode.str = (uint16*)pHdr->pBuf + pHdr->nWriteIndex;
         WSTRNCOPYN( pHdrNode->val.unicode.str, 
                  nwSeqLen, pwSeq, nwSeqLen);
         // increment index into the buffer which stores byte sequences and strings
         pHdr->nWriteIndex += (nwSeqLen * sizeof(AECHAR));
      }
      // move to the next header in the list
      pHdr->hdrList.count++;
      
   }
   return ret;
}
//=============================================================================
//  
//
//=============================================================================
int OEMOBEXHdr_AddU32(OEMOBEXHdr *pHdr, uint8 headerId, uint32 data)
{
   mt_obex_hdr_type *pHdrNode = NULL;
   int ret = AEE_SUCCESS;

   if (pHdr->nMaxHdrCnt <= pHdr->hdrList.count)
   {
      ret = OEMOBEXHdr_ReAllocateList(pHdr, pHdr->nMaxHdrCnt + OEMOBEX_MAX_HEADER_CNT_INC);
   }

   if ( AEE_SUCCESS == ret )
   {
      pHdrNode = &pHdr->hdrList.list_ptr[pHdr->hdrList.count];
       pHdrNode->id              = headerId;
      pHdrNode->val.u_int_32    = data;
      // move to the next header in the list
      pHdr->hdrList.count++;
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================

int OEMOBEXHdr_AddByteSeq(OEMOBEXHdr *pHdr, uint8 headerId, const uint8 *data, int nLen)
{
   mt_obex_hdr_type *pHdrNode = NULL;
   int ret = AEE_SUCCESS;

   if (pHdr->nWriteIndex >= pHdr->nMaxPacket)
   { 
      // ideally the aee layer should take care of this and
      // we should not be reaching here
      return AEE_EBADPARM;
   }

   if (pHdr->nMaxHdrCnt <= pHdr->hdrList.count)
   {
      ret = OEMOBEXHdr_ReAllocateList(pHdr, pHdr->nMaxHdrCnt + OEMOBEX_MAX_HEADER_CNT_INC);
   }
   
   if (AEE_SUCCESS == ret)
   {
      pHdrNode = &pHdr->hdrList.list_ptr[pHdr->hdrList.count];
     pHdrNode->id              = headerId;
      pHdrNode->val.byteseq.len = nLen;

     if (nLen > 0)
      {
      pHdrNode->val.byteseq.data = pHdr->pBuf + pHdr->nWriteIndex;
      MEMCPY(pHdrNode->val.byteseq.data, data, nLen);
      // move to the next header in the list
      pHdr->hdrList.count++;
      pHdr->nWriteIndex += nLen;
     }
   }
   return ret;

}
//=============================================================================
//
//
//=============================================================================

int OEMOBEXHdr_AddByte(OEMOBEXHdr *pHdr, uint8 headerId, uint8 data)
{
   mt_obex_hdr_type *pHdrNode = NULL;
   int ret = AEE_SUCCESS;

   if (pHdr->nMaxHdrCnt <= pHdr->hdrList.count)
   {
      ret = OEMOBEXHdr_ReAllocateList(pHdr, pHdr->nMaxHdrCnt + OEMOBEX_MAX_HEADER_CNT_INC);
   }

   if ( AEE_SUCCESS == ret )
   {
      pHdrNode = &pHdr->hdrList.list_ptr[pHdr->hdrList.count];
      pHdrNode->val.u_int_8 = data;
     pHdrNode->id          = headerId;
      // move to the next header in the list
      pHdr->hdrList.count++;
   }
   return ret;

}
//=============================================================================
//  
//
//=============================================================================

int OEMOBEXHdr_GetCount(OEMOBEXHdr *pHdr)
{
   int cnt = 0;
   int ret = AEE_SUCCESS;

   if ((AEE_SUCCESS == ret) && (pHdr->hdrList.list_ptr))
   {
      cnt = pHdr->hdrList.count;
   }
   return cnt;
}
//=============================================================================
//  
//
//=============================================================================

int OEMOBEXHdr_GetList(OEMOBEXHdr *me, OEMOBEXHdrNode *pHdrNode,int nCnt)
{
   int ret = AEE_SUCCESS;
   int i =0;
   mt_obex_hdr_list_type *pHdr = &me->hdrList;

   if ((AEE_SUCCESS == ret) && (pHdr->list_ptr))
   {
      for (i = 0; (i < nCnt) && ( i < pHdr->count); i++)
      {
         pHdrNode[i].headerId = pHdr->list_ptr[i].id;

         switch(OEMOBEX_HDR_KIND(pHdr->list_ptr[i].id))
         {
            case OEMOBEX_HDR_ID_UNICODE:
               pHdrNode[i].nLen = pHdr->list_ptr[i].val.unicode.len;
               break;
            case OEMOBEX_HDR_ID_BYTESEQ:
               pHdrNode[i].nLen = pHdr->list_ptr[i].val.byteseq.len;
               break;
            case OEMOBEX_HDR_ID_UINT8:
            case OEMOBEX_HDR_ID_UINT32:
               pHdrNode[i].nLen = 1;
               break;
            default:
               break;
         }
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
int OEMOBEXHeader_GetHeader(OEMOBEXHdr *pHdr,
                            int nIndex,
                            int nOffset,
                            void *pData,
                            int nLen)
{

   mt_obex_hdr_type *pMtHdr = NULL ; 
   int ret = AEE_SUCCESS;

   // if this function is being called, then nIndex, pData, nlen  should be
   // validated by the AEE layer.
   pMtHdr = &pHdr->hdrList.list_ptr[nIndex];
   switch(OEMOBEX_HDR_KIND(pMtHdr->id))
   {
      case OEMOBEX_HDR_ID_UNICODE:
      {
         // nLen includes the string len
         if (pMtHdr->val.unicode.str)
         {
         WSTRNCOPYN((uint16*)pData, nLen, pMtHdr->val.unicode.str, nLen);
         }
         else
         {
            ret = AEE_EBADSTATE;
         }
         break;
      }
      case OEMOBEX_HDR_ID_BYTESEQ:
      {
         if (pMtHdr->val.byteseq.data)
         {
         MEMCPY((uint8*)pData, pMtHdr->val.byteseq.data + nOffset, nLen);
         }
         else
         {
            ret = AEE_EBADSTATE;
         }
         break;
      }
      case OEMOBEX_HDR_ID_UINT8:
      {
         uint8 *pU8 = (uint8*)pData;
         *pU8 = pMtHdr->val.u_int_8;
         break;
      }
      case OEMOBEX_HDR_ID_UINT32:
      {
         uint32 *pU32 = (uint32*)pData;
         *pU32 = pMtHdr->val.u_int_32;
         break;
      }
      default:
         break;
   }

   return ret;
}
//=============================================================================
//  
//
//=============================================================================
int OEMOBEXHdr_MtToAeeHeaders(uint8                    hdrCnt,
                              mt_obex_hdr_type         *pMtHdr,
                              OEMOBEXHdr               *pOemRecvHdr,
                              boolean                  bFlagDisc,
                              bt_app_id_type           appId )
{
   
   mt_obex_hdr_list_type  *pHdrList = NULL;
   int i = 0;

   if ((0 != hdrCnt) && (NULL != pMtHdr))
   {
      if (NULL != pOemRecvHdr)
      {
         if (pOemRecvHdr->nMaxHdrCnt < hdrCnt)
         { 
            OEMOBEXHdr_ReAllocateList(pOemRecvHdr, hdrCnt); 
         }
         if ((pOemRecvHdr->nMaxHdrCnt >= hdrCnt) &&
             (pOemRecvHdr->hdrList.list_ptr != NULL))
         {
            pHdrList = &pOemRecvHdr->hdrList;
            pOemRecvHdr->hdrList.count    = 0;
            pOemRecvHdr->nWriteIndex      = 0;

            for (i = 0; i < hdrCnt; i++)
            {
               pHdrList->list_ptr[i].id = pMtHdr[i].id;

               switch(OEMOBEX_HDR_KIND(pMtHdr[i].id))
               {
                  case OEMOBEX_HDR_ID_UNICODE:
                  {
                     int nLen = pMtHdr[i].val.unicode.len + 1;
                     uint8* pBuf = pOemRecvHdr->pBuf + pOemRecvHdr->nWriteIndex;

                     pHdrList->list_ptr[i].val.unicode.len = nLen; // includes the null terminator
                     pHdrList->list_ptr[i].val.unicode.str = (uint16*)pBuf;
                     WSTRNCOPYN((uint16*)pBuf, nLen, pMtHdr[i].val.unicode.str, nLen);
                     pOemRecvHdr->nWriteIndex += nLen * sizeof(uint16);
                     pHdrList->count++;
                     break;
                  }
                  case OEMOBEX_HDR_ID_BYTESEQ:
                  {
                     int nLen = pMtHdr[i].val.byteseq.len;
                     uint8* pBuf = pOemRecvHdr->pBuf + pOemRecvHdr->nWriteIndex;

                     pHdrList->list_ptr[i].val.byteseq.len = nLen;
                     pHdrList->list_ptr[i].val.byteseq.data = pBuf;
               
                     MEMCPY((uint8*)pBuf, pMtHdr[i].val.byteseq.data, nLen);
                     pOemRecvHdr->nWriteIndex += nLen * sizeof(uint8);
                     pHdrList->count++;

                     break;
                  }
                  case OEMOBEX_HDR_ID_UINT8:
                  {
                     pHdrList->list_ptr[i].val.u_int_8 = pMtHdr[i].val.u_int_8; 
                     pHdrList->count++;
                     break;
                  }
                  case OEMOBEX_HDR_ID_UINT32:
                  {
                     pHdrList->list_ptr[i].val.u_int_32 = pMtHdr[i].val.u_int_32;
                     pHdrList->count++;
                     break;
                  }
                  default:
                     break;
               }// end of switch( header kind)
            } //end of for loop
         } // sufficient nodes available
      }
      OEMOBEXHdr_MtToAeeHdrFree(hdrCnt, pMtHdr, bFlagDisc, appId);
   }
   return AEE_SUCCESS;
}

//=============================================================================
//  
//
//=============================================================================

static int OEMOBEXHdr_MtToAeeHdrFree(uint8             hdrCnt,
                                     mt_obex_hdr_type  *pMtHdr,
                                     boolean           bFlagDisc,
                                     bt_app_id_type    appId)
{
   int i = 0;

   if (TRUE == bFlagDisc)
   {
      for (i = 0; i < hdrCnt; i++)
      {
         switch(OEMOBEX_HDR_KIND(pMtHdr[i].id))
         {
            case OEMOBEX_HDR_ID_UNICODE:
            {
               bt_cmd_pf_goep_free_headers(appId,
                  (bt_pf_goep_obex_hdr_type*)pMtHdr[i].val.unicode.str);
               pMtHdr[i].val.unicode.str = NULL;
               break;
            }
            case OEMOBEX_HDR_ID_BYTESEQ:
            {
               bt_cmd_pf_goep_free_headers(appId,
                  (bt_pf_goep_obex_hdr_type*)pMtHdr[i].val.byteseq.data);
               pMtHdr[i].val.byteseq.data = NULL;
               break;
            }
            default:
            break;
         }// end of switch( header kind)
      } //end of for loop
   } // disconnect headers
   bt_cmd_pf_goep_free_headers(appId, pMtHdr);

   return AEE_SUCCESS;
}
//=============================================================================
//  
//
//=============================================================================

static int  OEMOBEXHdr_ReAllocateList(OEMOBEXHdr *pHdr, int nCnt)
{

   int ret = AEE_ENOMEMORY;
   mt_obex_hdr_type *pHdrList = NULL;

   // allocate a few more nodes
   pHdrList = (mt_obex_hdr_type*)OEM_Malloc(sizeof(mt_obex_hdr_type) * nCnt);

   if (NULL != pHdrList)
   {
      MEMSET((void*)pHdrList, 0, sizeof(mt_obex_hdr_type) * nCnt);
      // copy the list into the larger memory
      MEMCPY((void*)pHdrList,
           pHdr->hdrList.list_ptr,
          pHdr->hdrList.count * sizeof(mt_obex_hdr_type));
      // free the previous list
      OEM_Free(pHdr->hdrList.list_ptr);

      pHdr->hdrList.list_ptr = pHdrList;
      // save the new max count;
      pHdr->nMaxHdrCnt = nCnt;
      ret = AEE_SUCCESS;
   }
   else
   {
      MSG_ERROR(" OEM OBEX Hdr out of memory...",0,0,0);
   }
   return ret;
}

//=============================================================================
//  
//
//=============================================================================
int OEMOBEXHdr_GetNameHeader(OEMOBEXHdr *pOemHdr, uint16 *pathName, int nLen)
{
   int i = 0;
      int ret = AEE_SUCCESS;
   MEMSET((void*)pathName, 0, sizeof(uint16) * nLen);

   if ((NULL != pOemHdr) && (NULL != pOemHdr->hdrList.list_ptr))
   {
      mt_obex_hdr_list_type *pList = &pOemHdr->hdrList;

      for (i = 0; i < pList->count; i++)
      {
         if (AEEOBEX_HDR_NAME == pList->list_ptr[i].id)
         {
            AECHAR *pSrc = pList->list_ptr[i].val.unicode.str;

            if (nLen <= pList->list_ptr[i].val.unicode.len)
            {
               ret = AEE_EFAILED;
            }
            else
            {  if (NULL != pSrc) 
               {
                  WSTRLCPY(pathName, pSrc, pList->list_ptr[i].val.unicode.len);
               }
            }
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
mt_obex_hdr_list_type* OEMOBEXHdr_GetHeaderList(OEMOBEXHdr *pOemHdr)
{
   mt_obex_hdr_list_type* pList= NULL;

   if (pOemHdr)
   {
      pList = &pOemHdr->hdrList;
   }
   return pList;
}
#endif /* #if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */

