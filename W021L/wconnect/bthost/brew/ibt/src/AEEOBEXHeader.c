/*======================================================
FILE:  AEEOBEXHeader.c

SERVICES:  Implementation of  IOBEXHeader

GENERAL DESCRIPTION:
   This file contains implementation for AEECLSID_OBEXHeader

===============================================================================
        Copyright (c) 2007-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

$Header: //source/qcom/qct/wconnect/bthost/brew/ibt/rel/00.00.26/src/AEEOBEXHeader.c#1 $ 
$DateTime: 2009/01/07 18:14:54 $
$Author: deepikas $

when        who  what, where, why
----------  ---  ------------------------------------------------------------
2008-10-02  rs   Removing compiler and Lint warnings.
=============================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP)
#include "AEEStdLib.h"
#include "AEEStdErr.h"
#include "AEEShell.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMHeap.h"
#include "AEEIOBEXHeader.h"
#include "AEEOBEXDefs_priv.h"
#include "AEEBTUtils.h"

//=============================================================================
//   Type definitions
//=============================================================================
#define AEEOBEXHDR_IS_VALID(x)  (!(x & 0xFF00))
#define AEEOBEXHDR_TYPE(h)      ((h) & (0xC0))
#define AEEOBEX_UNICODE_SIZE(x) (1 + 2 + (x * sizeof(AECHAR)))
#define AEEOBEX_UINT32_SIZE      (1 + 4)
#define AEEOBEX_BYTE_SEQ_SIZE(x) (1 + 2 + (x))
#define AEEOBEX_BYTE_SIZE        (1 + 1)

//=============================================================================
//
//=============================================================================
typedef struct 
{
   OEMOBEXHdrNode  *pHead;
   int             nCnt;
   boolean         bFetchList;
}AEEOBEXHdrList;

//=============================================================================
//
//=============================================================================
struct IOBEXHeader
{
   const IOBEXHeaderVtbl*  pvt;
   uint32                  uRef;
   IShell*                 pIShell;
   AEECallback             cbAppClose;
   OEMOBEXHdr              *pOemHdr;
   AEEOBEXHdrList          hdrList;
   int                     nMaxWritable;
   boolean                 bWritable;
};

//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXHeader_AddRef( IOBEXHeader *p );
static uint32 AEEOBEXHeader_Release( IOBEXHeader *p );
static int AEEOBEXHeader_QueryInterface( IOBEXHeader *p,
                                          AEECLSID idReq, void **ppo );
static int AEEOBEXHeader_AddString(IOBEXHeader      *p ,
                                   AEEOBEXHeaderID  headerId,
                                   const AECHAR     *pwSeq,
                                   int              nwSeqLen);
static  int AEEOBEXHeader_AddU32(IOBEXHeader       *p,
                                 AEEOBEXHeaderID   headerId,
                                 uint32            data);
static  int AEEOBEXHeader_AddByteSeq(IOBEXHeader     *p,
                                     AEEOBEXHeaderID headerId,
                                     const uint8     *pSeq,
                                     int             nSeqLen);

static  int AEEOBEXHeader_GetU32(IOBEXHeader       *p,
                                 AEEOBEXHeaderID   headerId,
                                 uint32            *pdwData);


static  int AEEOBEXHeader_GetByteSeq(IOBEXHeader     *p,
                                     AEEOBEXHeaderID headerId,
                                     int             nOffSet,
                                     uint8           *pSeq,
                                     int             nSeqLen,
                                     int             *pnSeqLenReq);

static  int AEEOBEXHeader_GetString(IOBEXHeader     *p ,
                                    AEEOBEXHeaderID headerId,
                                    AECHAR          *pwSeq,
                                    int             nwSeqLen,
                                    int             *pnwSeqLenReq);

static  int AEEOBEXHeader_GetList(IOBEXHeader       *p,
                                  AEEOBEXHeader     *pHdr,
                                  int               nOffset,
                                  int               nHdrLen,
                                  int               *pnHdrLenReq);
static  int AEEOBEXHeader_GetU32AtIndex(IOBEXHeader   *p,
                                        int           nIrndex,
                                        uint32        *pData);
static  int AEEOBEXHeader_GetByteSeqAtIndex(IOBEXHeader     *p,
                                            int             nIndex,
                                            int             nOffSet,
                                            uint8           *pBuf,
                                            int             nBufLen);
static  int AEEOBEXHeader_GetStringAtIndex(IOBEXHeader     *p,
                                           int             nIndex,
                                           AECHAR          *pwSeq,
                                           int             nwSeqLen);

static  int AEEOBEXHeader_AddByte(IOBEXHeader       *me,
                                  AEEOBEXHeaderID   headerId,
                                  uint32            data);

//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXHeader_Delete(IOBEXHeader *me);
static int AEEOBEXHdr_FindHeader(IOBEXHeader *pHdr, uint32 headerId, int *pnIndex,
                         int *nReqLen);
static int AEEOBEXHdr_CopyList(IOBEXHeader *me, AEEOBEXHeader *pHdr, int nOffset,
                        int nHdrLen);


static const IOBEXHeaderVtbl g_Vtbl_OBEXHeader = {
   AEEOBEXHeader_AddRef,
   AEEOBEXHeader_Release,
   AEEOBEXHeader_QueryInterface,
   AEEOBEXHeader_AddString,
   AEEOBEXHeader_AddU32,
   AEEOBEXHeader_AddByteSeq,
   AEEOBEXHeader_GetU32,
   AEEOBEXHeader_GetByteSeq,
   AEEOBEXHeader_GetString,
   AEEOBEXHeader_GetList,
   AEEOBEXHeader_GetU32AtIndex,
   AEEOBEXHeader_GetByteSeqAtIndex,
   AEEOBEXHeader_GetStringAtIndex
};

//=============================================================================
// This API is called by the client or server APIs when a IOBEXHeader object
// needs to be sent to the application along with an event. This API
// associates an AEE object with a given OEM object.
//
//=============================================================================
int AEEOBEXHeader_GetObject(IShell* pIShell, void **ppiHdr, OEMOBEXHdr *pOemHdr,
                            boolean bWritable)
{
   IOBEXHeader *me;

   if (NULL == ppiHdr)
   {
      return AEE_EBADPARM;
   }
   // allocate me
   me = MALLOCREC(IOBEXHeader);

   if (NULL == me)
   {
      MSG_ERROR("No memory for header object dropping headers...",0,0,0);
      OEMOBEXHdr_Release(pOemHdr);
      return AEE_ENOMEMORY;
   }
   me->pvt                = &g_Vtbl_OBEXHeader;
   me->pIShell            = pIShell;
   me->uRef               = 1;
   me->hdrList.bFetchList = TRUE;
   me->hdrList.nCnt       = 0;
   me->hdrList.pHead      = NULL;
   me->bWritable          = bWritable;

   // associate the given OEM object with the AEE object
   me->pOemHdr = pOemHdr;

   if (bWritable)
   {
      OEMOBEXHdr_AddRef(pOemHdr);
   }
   me->nMaxWritable = OEMOBEXHdr_MaxBufSize(pOemHdr);

   ISHELL_AddRef(pIShell);

   // Initialize the cb that will be called in case the app closes
   CALLBACK_Init(&me->cbAppClose, AEEOBEXHeader_Delete, me);
   AEE_LinkSysObject(&me->cbAppClose);

   *ppiHdr = (void *)me;

   return AEE_SUCCESS;
}



//=============================================================================
// 
//
//=============================================================================
static int AEEOBEXHdr_FetchList(IOBEXHeader *me)
{
   int ret = AEE_SUCCESS;
   int nCnt = 0;
   nCnt = OEMOBEXHdr_GetCount(me->pOemHdr);

   FREEIF(me->hdrList.pHead);
   me->hdrList.pHead = NULL;
   me->hdrList.nCnt  = 0;

   if ( 0 != nCnt )
   {
      me->hdrList.pHead = (OEMOBEXHdrNode*)
                           MALLOC(nCnt * sizeof (OEMOBEXHdrNode));
      if (NULL != me->hdrList.pHead)
      {
         ret = OEMOBEXHdr_GetList(me->pOemHdr, me->hdrList.pHead, nCnt);
         me->hdrList.bFetchList = FALSE;
           me->hdrList.nCnt = nCnt;
      }
      else
      {
         ret = AEE_EFAILED;
         MSG_ERROR("No memory to fetch list",0,0,0);
      }
   }
   return ret;

}
//=============================================================================
// 
//
//=============================================================================
static uint32 AEEOBEXHeader_Delete(IOBEXHeader *me)
{
   if (NULL != me)
   {
      CALLBACK_Cancel(&me->cbAppClose);
      
      if (me->pOemHdr != NULL)
      {
         MSG_MED("AEE OBEX hdr calling oem release phdr %x",me,0,0);
         // freeing the oem object is done by the cli or srv module
         OEMOBEXHdr_Release(me->pOemHdr);
      }
      me->pOemHdr = NULL;
      FREEIF(me->hdrList.pHead);
      me->hdrList.pHead = NULL;
      me->hdrList.nCnt  = 0;
      RELEASEIF(me->pIShell);
      FREEIF(me);
   }
   return AEE_SUCCESS;
}


//=============================================================================
//
//
//=============================================================================
static uint32 AEEOBEXHeader_AddRef(IOBEXHeader *me)
{
   MSG_DEBUG("AEE OBEX hdr Add ref me->uRef %x",me->uRef,0,0);
   return ++me->uRef;
}

//=============================================================================
//
//
//=============================================================================
static uint32 AEEOBEXHeader_Release(IOBEXHeader *me)
{
   uint32 uRef = --me->uRef;

   MSG_DEBUG("AEE OBEX hdr Add Rel me->uRef %x",me->uRef,0,0);
   if (0 == uRef)
   {
      AEEOBEXHeader_Delete(me);
   }
   return uRef;
}

//=============================================================================
//
//
//=============================================================================
static int AEEOBEXHeader_QueryInterface(IOBEXHeader *me, AEECLSID cls, void **ppo)
{
   if (NULL == ppo )
   {
      return EBADPARM;
   }

   if ( !AEE_CHECK_PTR("AEEOBEXHeader_QueryInterface",
                       ppo, sizeof(void*), TRUE) )
   {
      return EMEMPTR;
   }

   switch (cls)
   {
   case AEEIID_IQI:
   case AEEIID_IOBEXHeader:
      *ppo = me;
   break;

   default:
      *ppo = 0;
      return AEE_ECLASSNOTSUPPORT;
   }

   IQI_AddRef(*ppo);
   return AEE_SUCCESS;
}


//=============================================================================
//
//
//=============================================================================

static int AEEOBEXHeader_AddString(IOBEXHeader      *me ,
                                   AEEOBEXHeaderID  headerId,
                                   const AECHAR     *pwSeq,
                                   int              nwSeqLen)
{
   int ret = AEE_SUCCESS;

   // the header should be writable 
   if ((FALSE == me->bWritable) || (NULL == me->pOemHdr))
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add str, bWr %x, pOem %x",
                 me->bWritable,me->pOemHdr,0);
      return AEE_EREADONLY;
   }

   // Header IDs are 8 bit values
   // see if the header is of type string
   // we allow zero length strings
   if ((FALSE == AEEOBEXHDR_IS_VALID(headerId)) ||
      (AEEOBEX_HDR_UNICODE != AEEOBEXHDR_TYPE(headerId)) ||
      ((NULL == pwSeq) && (nwSeqLen != 0)) ||
       (nwSeqLen < 0))
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add str id %x, pwSeq %x, len %x",
                 headerId,pwSeq,nwSeqLen);
      return AEE_EBADPARM;
   }
   // see that the string is null terminated
   if ((nwSeqLen > 0) && 
       ((NULL == pwSeq) ||
        (0 != pwSeq[nwSeqLen -1 ]) ||
        (!AEE_CHECK_PTR("AEEOBEXHeader_AddString",
                       pwSeq, sizeof(AECHAR)*nwSeqLen, FALSE))))
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add str id %x, pwSeq %x, len %x",
                 headerId,pwSeq,nwSeqLen);
      return AEE_EBADPARM;
   }

   if (AEEOBEX_UNICODE_SIZE(nwSeqLen) > (uint32)me->nMaxWritable)
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add str data exceeded buf len",
                 0,0,0);
      return AEE_ENOMEMORY;
   }
   // send the string to OEM layer to save it in a format best 
   // suited to it
   ret = OEMOBEXHdr_AddString(me->pOemHdr,
                             (uint8)headerId, 
                             (uint16*)pwSeq, nwSeqLen);

   if (AEE_SUCCESS == ret)
   {
      // the list has been updated,in the oem layer
      // fetch the list when returning information to the application
      me->hdrList.bFetchList = TRUE;
      me->nMaxWritable -= AEEOBEX_UNICODE_SIZE(nwSeqLen);
   }
   return ret;
}

//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_AddU32(IOBEXHeader       *me,
                                 AEEOBEXHeaderID   headerId,
                                 uint32            data)
{
   int ret = AEE_SUCCESS;

   // the header should be writable 
   //the oem obj should not have been delinked ( incase of send hdrs)
   if ((FALSE == me->bWritable) || (NULL == me->pOemHdr))
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add u32, bWr %x, pOem %x",
                 me->bWritable,me->pOemHdr,0);
      return AEE_EREADONLY;
   }

    // Header IDs are 8 bit values
   if (FALSE == AEEOBEXHDR_IS_VALID(headerId))
   {
      MSG_ERROR("AEE OBEX Hdr Cannot add u32, bWr %x, pOem %x",
                 me->bWritable,me->pOemHdr,0);
      return AEE_EBADPARM;
   }
   // see if the header is a single byte value
   if (AEEOBEX_HDR_BYTE == AEEOBEXHDR_TYPE(headerId))
   {
      ret = AEEOBEXHeader_AddByte(me, headerId, data);
   }
   else
   {
      // see if the header is a uint32 byte value
      if ((AEEOBEX_HDR_UINT32 != AEEOBEXHDR_TYPE(headerId)) ||
          (AEEOBEX_UINT32_SIZE > me->nMaxWritable))
      {
         MSG_ERROR("AEE OBEX Hdr Add U32 %x, ret %x",
                 data,ret,0);
         ret = AEE_EBADPARM;
      }
      else
      {
         ret = OEMOBEXHdr_AddU32(me->pOemHdr, (uint8)headerId, data);

         if (AEE_SUCCESS == ret)
         {
            // the list has been updated,in the oem layer
            // fetch the list when returning information to the application
            me->hdrList.bFetchList = TRUE;
            me->nMaxWritable -= AEEOBEX_UINT32_SIZE;
         }
      }
   }
   MSG_DEBUG("AEE OBEX Hdr Add U32 %x, ret %x",
                 data,ret,0);
   return ret;
}

//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_AddByteSeq(IOBEXHeader     *me,
                                     AEEOBEXHeaderID headerId,
                                     const uint8     *pSeq,
                                     int             nSeqLen)
{
   int ret = AEE_SUCCESS;

   // the header should be writable 
   //the oem obj should not have been delinked ( incase of send hdrs)
   if ((FALSE == me->bWritable) || (NULL == me->pOemHdr))
   {
      MSG_ERROR(" AEE OBEX Hdr,cannot write to read only hdr",0,0,0);
      return AEE_EREADONLY;
   }

    // Header IDs are 8 bit values
   if (FALSE == AEEOBEXHDR_IS_VALID(headerId))
   {
      MSG_ERROR(" AEE OBEX Hdr,invalid id",0,0,0);
      return AEE_EBADPARM;
   }
   // see if the header is of type byte sequence
   if ((AEEOBEX_HDR_BYTE_SEQ != AEEOBEXHDR_TYPE(headerId)) || 
        (nSeqLen < 0))
   {
      MSG_ERROR(" AEE OBEX Hdr,invalid id",0,0,0);
      return AEE_EBADPARM;
   }

   if ((AEEOBEX_BYTE_SEQ_SIZE(nSeqLen)) > me->nMaxWritable)
   {
      MSG_ERROR(" AEE OBEX Hdr, exceded packet size, max alowed %x",
                  me->nMaxWritable,0,0);
      return AEE_ENOMEMORY;
   }

   ret = OEMOBEXHdr_AddByteSeq(me->pOemHdr, (uint8)headerId, pSeq, nSeqLen);

   if (AEE_SUCCESS == ret)
   {
      // the list has been updated,in the oem layer
      // fetch the list when returning information to the application
      me->hdrList.bFetchList = TRUE;
      me->nMaxWritable -= AEEOBEX_BYTE_SEQ_SIZE(nSeqLen);
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_AddByte(IOBEXHeader       *me,
                                  AEEOBEXHeaderID   headerId,
                                  uint32            data)
{
   int ret = AEE_SUCCESS;

   // the header should be writable 
   //the oem obj should not have been delinked ( incase of send hdrs)
   if ((FALSE == me->bWritable) || (NULL == me->pOemHdr))
   {
      MSG_ERROR(" AEE OBEX Hdr, non writable %x",
                  me->bWritable,0,0);
      return AEE_EREADONLY;
   }

   if (AEEOBEX_BYTE_SIZE > me->nMaxWritable)
   {
      MSG_ERROR(" AEE OBEX Hdr exceeded writable %x", me->nMaxWritable,0,0);
      return AEE_ENOMEMORY;
   }

   ret = OEMOBEXHdr_AddByte(me->pOemHdr, (uint8)headerId, (uint8)data);

   if (AEE_SUCCESS == ret)
   {
      // the list has been updated,in the oem layer
      // fetch the list when returning information to the application
      me->hdrList.bFetchList = TRUE;
      me->nMaxWritable -= AEEOBEX_BYTE_SIZE;
   }
   return ret;
}

//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_GetList(IOBEXHeader         *me,
                                  AEEOBEXHeader       *pHdr,
                                  int                 nOffset,
                                  int                 nHdrLen,
                                  int                 *pnHdrLenReq)
{
   int ret = AEE_SUCCESS;
   int nRetLen = 0;

   if ((NULL == me->pOemHdr) || (0 > nHdrLen) || (0 > nOffset))
   {
      return AEE_EBADPARM;
   }
   // get the header count from the OEM layer
   if (me->hdrList.bFetchList)
   {
      ret = AEEOBEXHdr_FetchList(me);
   }
   // this check is in case fetch list fails
   if (AEE_SUCCESS != ret)
   {
      return ret;
   }

   nRetLen = me->hdrList.nCnt;

   if (me->hdrList.nCnt > 0)
   {
      // check if the offset is within the range
      if (nOffset >= me->hdrList.nCnt)
      {
         ret = AEE_ENOSUCH;
      }
      else
      {
         //  nHdrLen == 0; return the number of headers in the list
         if (0 == nHdrLen)
         {
            ret = AEE_EBUFFERTOOSMALL;
         }
         else
         {
            // verify pointers
            if ((NULL == pHdr) ||
               (!AEE_CHECK_PTR("AEEOBEXHeader_GetList",
                        pHdr, sizeof(AEEOBEXHeader) * nHdrLen, TRUE)))
            {
               ret = AEE_EBADPARM;
            }
            else
            {
               // return as many headers as possible from the list
               // upto a max count of nHdrLen
               ret = AEEOBEXHdr_CopyList(me, pHdr, nOffset, nHdrLen);
               if (ret == AEE_SUCCESS)
               {
                  // lesser of (actual number of headers copied and nHdrLen)
                  nRetLen =  nHdrLen < ((me->hdrList.nCnt - nOffset)) ?
                             nHdrLen :
                             (me->hdrList.nCnt - nOffset);
               }
            }
         }
      }
   }
   else
   {
      ret = AEE_ENOMORE;
   }

   if (NULL != pnHdrLenReq)
   {
      *pnHdrLenReq = nRetLen;
   }

   return ret;
}
//=============================================================================
//
//
//=============================================================================
static int AEEOBEXHdr_CopyList(IOBEXHeader *me,
                               AEEOBEXHeader *pHdr,
                               int nOffset,
                               int nHdrLen)
{
   int i =0;
   OEMOBEXHdrNode *pCur = NULL;

   if (NULL == me->hdrList.pHead)
   {
      return AEE_EFAILED;
   }

   for (i = 0; (i < nHdrLen) && ((nOffset + i) < me->hdrList.nCnt); i++)
   {
      pCur = &me->hdrList.pHead[nOffset + i];
      pHdr[i].headerId = pCur->headerId;
      pHdr[i].headerType = AEEOBEXHDR_TYPE(pCur->headerId);
      pHdr[i].nHeaderSize = pCur->nLen;
   }
   return AEE_SUCCESS;
}
//=============================================================================
//
//
//=============================================================================
static int AEEOBEXHeader_GetString(IOBEXHeader       *me,
                                   AEEOBEXHeaderID   headerId,
                                   AECHAR            *pwSeq,
                                   int               nwSeqLen,
                                   int               *pnwSeqLenReq)
{
   int nReqLen = 0;
   int nIndex  = 0;
   int ret = AEE_SUCCESS;

    // Validate parameters
   if ((FALSE == AEEOBEXHDR_IS_VALID(headerId)) ||
       (AEEOBEX_HDR_UNICODE != AEEOBEXHDR_TYPE(headerId)) ||
       (NULL == pnwSeqLenReq) ||
       (NULL == me->pOemHdr) ||
       (!AEE_CHECK_PTR("AEEOBEXHeader_GetString",
                       pnwSeqLenReq, sizeof(int*), TRUE)))
   {
      return AEE_EBADPARM;
   }

   ret = AEEOBEXHdr_FindHeader(me, headerId, &nIndex, &nReqLen);

   if (AEE_SUCCESS == ret)
   {
      // return the actual length
      *pnwSeqLenReq = nReqLen;
      // can be a string of zero length
      if (nReqLen > 0)
      {
         if (nReqLen > nwSeqLen)
         {
            ret = AEE_EBUFFERTOOSMALL;
         }
         else
         {
            if ((NULL == pwSeq) ||
                (!AEE_CHECK_PTR("AEEOBEXHeader_GetString",
                          pwSeq, sizeof(AECHAR) * nwSeqLen, TRUE)))
            {
               ret = AEE_EBADPARM;
            }
            else
            {
               MEMSET((void*)pwSeq, 0, nwSeqLen);
               ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, pwSeq, nwSeqLen);
            }
         }
      }
   } //else ret != AEE_SUCCESS and header not found
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static int AEEOBEXHeader_GetByteSeq(IOBEXHeader   *me,
                                    AEEOBEXHeaderID headerId,
                                    int             nOffSet,
                                    uint8           *pSeq,
                                    int             nSeqLen,
                                    int             *pnSeqLenReq)
{
   int nReqLen = 0;
   int nIndex  = 0;
   int ret = AEE_SUCCESS;
   int bytesToRet = 0;

    // Validate parameters
   if ((FALSE == AEEOBEXHDR_IS_VALID(headerId)) ||
       (AEEOBEX_HDR_BYTE_SEQ != AEEOBEXHDR_TYPE(headerId)) ||
       (NULL == me->pOemHdr) ||
       (nSeqLen < 0) || (nOffSet < 0 ))
   {
      return AEE_EBADPARM;
   }

   ret = AEEOBEXHdr_FindHeader(me, headerId, &nIndex, &nReqLen);

   if (AEE_SUCCESS == ret)
   {
      // return the total length
      bytesToRet = nReqLen;

       // can be a bytesequence of zero length
      if (nReqLen > 0)
      {
         // not a zero len byte seq
         if (nOffSet >= nReqLen)
         {
            ret = AEE_ENOSUCH;
         }
         else if (0 == nSeqLen)
         {
            ret = AEE_EBUFFERTOOSMALL;
         }
         else
         {  // i/p buffer len (nSeqLen) !=0 
            if ((NULL == pSeq) ||
                (!AEE_CHECK_PTR("AEEOBEXHeader_GetString",
                          pSeq, sizeof(uint8) * nSeqLen, TRUE)))
            {
               ret = AEE_EBADPARM;
            }
            else
            {   // nSeqLen != 0, valid offset, 
               bytesToRet = (nReqLen - nOffSet) > nSeqLen ?
                             nSeqLen :
                             (nReqLen - nOffSet);
               MEMSET((void*)pSeq, 0, nSeqLen);

               ret = OEMOBEXHeader_GetHeader(me->pOemHdr, 
                                             nIndex, 
                                             nOffSet,
                                             pSeq, bytesToRet);
            }
         }
      }
   } //else ret != AEE_SUCCESS and header not found

   if ((NULL != pnSeqLenReq) &&
           (AEE_CHECK_PTR("AEEOBEXHeader_GetByteSeq",
                       pnSeqLenReq, sizeof(int), TRUE)))
   {
      *pnSeqLenReq = bytesToRet;
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_GetU32(IOBEXHeader     *me,
                                       AEEOBEXHeaderID   headerId,
                                       uint32            *pdwData)
{
   int nIndex  = 0;
   int ret = AEE_EFAILED;

    // Validate parameters
   if ((FALSE == AEEOBEXHDR_IS_VALID(headerId)) ||
       (NULL == me->pOemHdr) ||
       (!AEE_CHECK_PTR("AEEOBEXHeader_GetU32",
                       pdwData, sizeof(uint32), TRUE)))
   {
      return AEE_EBADPARM;
   }

   ret = AEEOBEXHdr_FindHeader(me, headerId, &nIndex, NULL);

   if (AEE_SUCCESS == ret)
   {
      if (AEEOBEXHDR_TYPE(headerId) == AEEOBEX_HDR_BYTE)
      {
         uint8 byte = 0;
         *pdwData = 0;
         ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, &byte, sizeof(uint8));
         *pdwData = byte;
      }
      else if (AEEOBEXHDR_TYPE(headerId) == AEEOBEX_HDR_UINT32)
      {
         ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, pdwData, sizeof(uint32));
      }
   } //else ret != AEE_SUCCESS and header not found
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_GetByteSeqAtIndex(IOBEXHeader     *me,
                                            int             nIndex,
                                            int             nOffSet,
                                            uint8           *pBuf,
                                            int             nBufLen)
{
   int ret = AEE_SUCCESS;
   OEMOBEXHdrNode *pCur = NULL;
   int nReqLen = 0;

    // Validate parameters
   if ((NULL == me->pOemHdr) || (NULL == pBuf) || (nBufLen <= 0) ||
       (!AEE_CHECK_PTR("AEEOBEXHeader_GetByteSeqAtIndex",
                       pBuf, nBufLen, TRUE)))
   {
      return AEE_EBADPARM;
   }
   // get the header list from the OEM layer
   if (me->hdrList.bFetchList)
   {
      AEEOBEXHdr_FetchList(me);
   }

   if (nIndex >= me->hdrList.nCnt)
   {
      return AEE_ENOSUCH;
   }

   pCur = &me->hdrList.pHead[nIndex];

   if (AEEOBEXHDR_TYPE(pCur->headerId) != AEEOBEX_HDR_BYTE_SEQ)
   {
      return AEE_ENOSUCH;
   }

   nReqLen = pCur->nLen;
   // can be a bytesequence of zero length
   if (nReqLen > 0)
   {
      // not a zero len byte seq
      if (nOffSet >= nReqLen)
      {
         ret = AEE_ENOSUCH;
      }
      else
      {  
         int bytesToRet = 0;
         // nSeqLen != 0, valid offset, 
         bytesToRet = (nReqLen - nOffSet) > nBufLen ?
                       nBufLen :
                       (nReqLen - nOffSet);
         MEMSET((void*)pBuf, 0, nBufLen);

         ret = OEMOBEXHeader_GetHeader(me->pOemHdr, 
                                       nIndex, 
                                       nOffSet,
                                       pBuf, bytesToRet);
      }
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_GetStringAtIndex(IOBEXHeader     *me,
                                           int             nIndex,
                                           AECHAR          *pwSeq,
                                           int             nwSeqLen)
{
   int ret = AEE_SUCCESS;
   OEMOBEXHdrNode *pCur = NULL;

    // Validate parameters
   if ((NULL == me->pOemHdr) || (NULL == pwSeq) || (nwSeqLen < 0) ||
       (!AEE_CHECK_PTR("AEEOBEXHeader_GetStringAtIndex",
                       pwSeq, nwSeqLen, TRUE)))
   {
      return AEE_EBADPARM;
   }
   // get the header list from the OEM layer
   if (me->hdrList.bFetchList)
   {
      AEEOBEXHdr_FetchList(me);
   }

   if (nIndex >= me->hdrList.nCnt)
   {
      return AEE_ENOSUCH;
   }

   pCur = &me->hdrList.pHead[nIndex];

   if (AEEOBEXHDR_TYPE(pCur->headerId) != AEEOBEX_HDR_UNICODE)
   {
      return AEE_ENOSUCH;
   }

   if (nwSeqLen < pCur->nLen)
   {
      return AEE_EBUFFERTOOSMALL;
   }

   if ((0 != pCur->nLen) && (0 != nwSeqLen))
   {
    MEMSET((void*)pwSeq, 0, nwSeqLen);
    ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, pwSeq, nwSeqLen);
   }

   return ret;
}
//=============================================================================
//
//
//=============================================================================
static int AEEOBEXHdr_FindHeader(IOBEXHeader *me, uint32 headerId, int *pnIndex,
                         int *nReqLen)
{
   int i   = 0;
   int ret = AEE_ENOSUCH;

   AEEOBEXHdrList *pHdrList = &me->hdrList;

   *pnIndex = 0;

   if (pHdrList->bFetchList)
   {
      AEEOBEXHdr_FetchList(me);
   }

   for (i =0; i < pHdrList->nCnt; i++)
   {
      if (pHdrList->pHead[i].headerId == headerId)
      {
         *pnIndex = i;
       if (NULL != nReqLen)
       {
          *nReqLen = pHdrList->pHead[i].nLen;
       }
         ret = AEE_SUCCESS;
         break;
      }
   }
   return ret;
}
//=============================================================================
//
//
//=============================================================================
static  int AEEOBEXHeader_GetU32AtIndex(IOBEXHeader   *me,
                                        int           nIndex,
                                        uint32        *pData)
{
   int ret = AEE_ENOSUCH;
   OEMOBEXHdrNode *pCur = NULL;

    // Validate parameters
   if ((NULL == me->pOemHdr) || (NULL == pData) ||
       (!AEE_CHECK_PTR("AEEOBEXHeader_GetU32AtIndex",
                       pData, sizeof(uint32), TRUE)))
   {
      return AEE_EBADPARM;
   }
   // get the header list from the OEM layer
   if (me->hdrList.bFetchList)
   {
      AEEOBEXHdr_FetchList(me);
   }

   if (nIndex >= me->hdrList.nCnt)
   {
      return AEE_ENOSUCH;
   }

   pCur = &me->hdrList.pHead[nIndex];

   if (AEEOBEXHDR_TYPE(pCur->headerId) == AEEOBEX_HDR_BYTE)
   {
      uint8 byte = 0;
      *pData = 0;
      ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, &byte, sizeof(uint8));
      *pData = byte;
   }
   else if (AEEOBEXHDR_TYPE(pCur->headerId) == AEEOBEX_HDR_UINT32)
   {
      ret = OEMOBEXHeader_GetHeader(me->pOemHdr, nIndex, 0, pData, sizeof(uint32));
   }

   return ret;
}
//=============================================================================
//
//
//=============================================================================

int AEEOBEXHdr_MarkReadOnly(IOBEXHeader *pHdr)
{
  pHdr->bWritable = FALSE;
  return AEE_SUCCESS;
}
//=============================================================================
//
//
//=============================================================================

OEMOBEXHdr* AEEOBEXHdr_GetOemHdr(IOBEXHeader *pHdr)
{
   OEMOBEXHdr *pOemHdr = NULL;

   if (pHdr->pvt->AddRef == AEEOBEXHeader_AddRef)   
   {
      pOemHdr = pHdr->pOemHdr;
   }
   return pOemHdr;
}
#endif /* if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_GOEP) */

