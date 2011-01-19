/*===================================================================
FILE: AEEWebOpts.c

SERVICES:  IWebOpts Interface, AEEWebOptMgr implementation

DESCRIPTION:

PUBLIC CLASSES AND FUNCTIONS:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===================================================================*/
#include "AEEShell.h"
#include "AEEWebOptMgr.h"
#include "AEEWebOpts_static.h"

#include "AEEStdLib.h"
#include "asn1thin.h"

struct IWebOpts
{
   const IWebOptsVtbl *pvt;
   IWebOptsVtbl        vt; /* vtable allocated inline */
   
   uint32              uRef;
   WebOptMgr          *pwom;
};

static uint32 IWebOpts_AddRef(IWebOpts *me);
static uint32 IWebOpts_Release(IWebOpts *me);
static int    IWebOpts_AddOpt(IWebOpts *me, WebOpt *apwoz);
static int    IWebOpts_RemoveOpt(IWebOpts *me, int32 nOptId, int32 nIndex);
static int    IWebOpts_GetOpt(IWebOpts *me, int32 nOptId, int32 nIndex, WebOpt *pwo);

static int IWebOpts_QueryInterface(IWebOpts *me, AEECLSID id, void **ppo);

static int  WebOpt_Count(WebOpt **pawozList);
static int  WebOptMgr_OptSearch(WebOptMgr *me, int32 nOptId, int32 nIndex,
                                WebOpt *pwo);
static int  WebOptMgr_SetCopy(WebOptMgr *me, boolean bCopy);
static void WebOpt_PValFree(WebOpt *pwo);
static int  WebOpt_PValCopy(WebOpt *pwo);


/*====================================================================
  utility macro
  ===================================================================*/
#ifdef RELEASEIF
#undef RELEASEIF
#endif

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(void *)(x)); (x) = 0; }


/*====================================================================
   WebOptMgr.c stuff
  ===================================================================*/
typedef struct WebOptBlock WebOptBlock;

struct WebOptBlock {
   WebOptBlock *pNext;   /* singly-linked list */
   WebOpt       wo;
};

struct WebOptMgr
{
   int              nNumOpts;  /* number of opts (not including removed) */
   boolean          bCopy;     /* whether we're copying */

   WebOptBlock*     pNext;     /* added options, head of linked list */

   WebOpt           awozList[1]; /* constructed options, keep last, IWebReq over-allocated */
};


static int WebOpt_Count(WebOpt **pawozList)
{
   static WebOpt   woDummy = {WEBOPT_END,0};
   int             nOpts;
   int             i;
   int             nId;

   if ((WebOpt *)0 == *pawozList) { /* fake out empty list */
      *pawozList = &woDummy;
   }

   for (i = nOpts = 0; (nId = (*pawozList)[i].nId) != WEBOPT_END; i++) {
      if (nId > WEBOPT_RESERVED_LAST) { /* skip reserveds */
         nOpts++;
      }
   }
   
   return nOpts;
}

int WebOptMgr_GetSize(WebOpt *awozList)
{
   int nOpts = WebOpt_Count(&awozList); // struct always has room for END

   return sizeof(WebOptMgr) + (nOpts * sizeof(WebOpt));
}

static int WebOpt_PValCopy(WebOpt *pwo)
{
   if ((pwo->nId >= WEBOPT_SZ) && 
       (pwo->nId <= WEBOPT_SZ_LAST)) {

      if ((void *)0 == (pwo->pVal = (void *)STRDUP((char *)pwo->pVal))) {
         return ENOMEMORY;
      }
      
   } else if ((pwo->nId >= WEBOPT_WSZ) && 
              (pwo->nId <= WEBOPT_WSZ_LAST)) {
      
      if ((void *)0 == (pwo->pVal = (void *)WSTRDUP((AECHAR *)pwo->pVal))) {
         return ENOMEMORY;
      }
      
   } else if ((pwo->nId >= WEBOPT_INTERFACE) && 
              (pwo->nId <= WEBOPT_INTERFACE_LAST)) {

      if ((void *)0 != pwo->pVal) {
         IBASE_AddRef((IBase *)pwo->pVal);

         if (pwo->nId == WEBOPT_DEFAULTS) {
            IWebOpts *piwo = pwo->pVal;
            WebOpt awo[2];

            awo[0].nId = WEBOPT_COPYOPTS;
            awo[0].pVal = (void*)TRUE;
            
            awo[1].nId  = WEBOPT_END;
            awo[1].pVal = 0;
            
            IWEBOPTS_AddOpt(piwo, awo);
         }
      }
   } else if (((pwo->nId >= WEBOPT_VARIABLE) && 
              (pwo->nId <= WEBOPT_VARIABLE_LAST)) ||
             ((pwo->nId >= WEBOPT_VARBUFFER) && 
              (pwo->nId <= WEBOPT_VARBUFFER_LAST)) ||
             ((pwo->nId >= WEBOPT_ASN1) && 
              (pwo->nId <= WEBOPT_ASN1_LAST))) {
      char *pTmp;
      // First the length for variable which we'll overwrite if we're not 
      // variable (save a branch) (is that like save a tree?)
      unsigned int uLen = pwo->nId & 0xfff;
      if ((pwo->nId >= WEBOPT_VARBUFFER) && 
          (pwo->nId <= WEBOPT_VARBUFFER_LAST))
         // length for VARBUFFER
          uLen = WEBOPT_GETVARBUFFERLENGTH(pwo->pVal) + sizeof(uint16);
      if ((pwo->nId >= WEBOPT_ASN1) && (pwo->nId <= WEBOPT_ASN1_LAST)) {
         // Length of whole ASN.1 object, not just value
         uLen = ASN1Thin_ReturnSignedLen(pwo->pVal);
      }
      pTmp = MALLOC(uLen);
      if ((char *)0 == pTmp) {
         pwo->pVal = 0; // make sure we don't try to free the mem passed in
                        // should really have a better fix one level up
                        // so options is not partially added.
         return ENOMEMORY;
      }
      MEMMOVE(pTmp, pwo->pVal, uLen);
      pwo->pVal = pTmp;
   }

   return SUCCESS;
}

static void WebOpt_PValFree(WebOpt *pwo)
{
   if (((pwo->nId >= WEBOPT_SZ) && 
        (pwo->nId <= WEBOPT_SZ_LAST)) ||
       ((pwo->nId >= WEBOPT_WSZ) && 
        (pwo->nId <= WEBOPT_WSZ_LAST)) ||
       ((pwo->nId >= WEBOPT_VARIABLE) && 
        (pwo->nId <= WEBOPT_VARIABLE_LAST)) ||
       ((pwo->nId >= WEBOPT_VARBUFFER) && 
        (pwo->nId <= WEBOPT_VARBUFFER_LAST)) ||
       ((pwo->nId >= WEBOPT_ASN1) && 
        (pwo->nId <= WEBOPT_ASN1_LAST))) {
      
      FREEIF(pwo->pVal);

   } else if ((pwo->nId >= WEBOPT_INTERFACE) && 
              (pwo->nId <= WEBOPT_INTERFACE_LAST) &&
              ((void *)0 != pwo->pVal)) {
      IBASE_Release((IBase *)pwo->pVal);
      pwo->pVal = 0;
   } 
}

//
// This is called with the argument to a DEFAULTS option, which the
// only thing we know about it is that it has an IWEBOPTS interface.
//
// *pnIndex contains a pointer to the desired index.  If an exact match
// is not found, *pnIndex needs to be decremented by the number actually
// present.
//
static int IWebOpts_SearchChain(IWebOpts *pWebOpts, int32 nOptId, 
                               int32 *pnIndex, WebOpt *pwo)
{
   int nErr;

   /* check to see if we can get the nth right off */
   if ((WebOpt *)0 != pwo) {
      nErr = IWEBOPTS_GetOpt(pWebOpts, nOptId, *pnIndex, pwo);
   } else {
      nErr = IWEBOPTS_RemoveOpt(pWebOpts, nOptId, *pnIndex);
   }

   if (SUCCESS == nErr) {
      *pnIndex = -1;
      return SUCCESS; /* got it */
   }

   /* else decrement nIndex by number actually present in *this* 
      default (num to skip) */
   {
      int32   nSkip;
      WebOpt  wotoss;

      for (nSkip = *pnIndex - 1; 
           nSkip >= 0;
           nSkip--) {

         nErr = IWEBOPTS_GetOpt(pWebOpts, nOptId, nSkip, &wotoss);
         
         if (SUCCESS == nErr) {
            break;
         }
      }
      
      *pnIndex -= nSkip+1; /* decrement *pnIndex by number matching in
                              pWebOpts (might be zero) */
   }  

   return ((*pnIndex == -1) ? SUCCESS : EFAILED);
}

#define WEBOPTID_MATCH(idSrch, idTry) ((((idSrch) == WEBOPT_ANY) && ((idTry) > WEBOPT_RESERVED_LAST)) || ((idSrch) == (idTry)))

// NULL pwo causes delete
static int WebOptMgr_OptSearch(WebOptMgr *me, int32 nOptId, int32 nIndex,
                               WebOpt *pwo)
{
   WebOptBlock *pwob;
   WebOptBlock *pPrev;
   int32        i;

   // search the added options first
   pPrev = 0;
   for (pwob = me->pNext; (WebOptBlock *)0 != pwob; pwob = pwob->pNext) {

      if (WEBOPTID_MATCH(nOptId, pwob->wo.nId) && (nIndex-- == 0)) {

         if ((WebOpt *)0 != pwo) { /* means retrieve */
            *pwo = pwob->wo;
         } else { /* means remove */
            --me->nNumOpts;
            if (me->bCopy) {
               WebOpt_PValFree(&pwob->wo);
            }
            
            if ((WebOptBlock *)0 == pPrev) {  // first element?
               me->pNext = pwob->pNext;
            } else {
               pPrev->pNext = pwob->pNext;
            }

            FREE(pwob);
         }
         break;
      }

      // search chain before next
      if ((pwob->wo.nId == WEBOPT_DEFAULTS) && 
          (SUCCESS == IWebOpts_SearchChain(pwob->wo.pVal, 
                                           nOptId, &nIndex, pwo))) {
         break;
      }

      pPrev = pwob;
   }

   // search the constructed options next
   if (-1 != nIndex) {
      int nId;
      for (i = 0; (nId = me->awozList[i].nId) != WEBOPT_END; i++) {
         if (WEBOPTID_MATCH(nOptId,nId) && (nIndex-- == 0)) {
            if ((WebOpt *)0 != pwo) {
               *pwo = me->awozList[i];
            } else {
               --me->nNumOpts;
               if (me->bCopy) {
                  WebOpt_PValFree(&me->awozList[i]);
               }
		  
               for ( ; me->awozList[i].nId != WEBOPT_END; i++) {
                  me->awozList[i] = me->awozList[i+1];
               }
            }
            break;
         }

         // search chain before next
         if ((me->awozList[i].nId == WEBOPT_DEFAULTS) && 
             (SUCCESS == IWebOpts_SearchChain(me->awozList[i].pVal, 
                                              nOptId,&nIndex,pwo))) {
            break;
         }
      }
   }

   return ((nIndex == -1) ? SUCCESS : EFAILED);
}

static int WebOptMgr_SetCopy(WebOptMgr *me, boolean bCopy)
{
   int     nRet = SUCCESS;

   if ((me->nNumOpts && me->bCopy)  || (me->nNumOpts && !bCopy)) {
      /* already copying, or tried to turn it off when there are opts 
         already copied,  copy is sticky */
      return SUCCESS;
   }

   me->bCopy = bCopy;

   /* this loop will be empty if nNumOpts is right, and bCopy is FALSE */
   {
      WebOptBlock *pwob;
      int i;
      
      for (pwob = me->pNext; (WebOptBlock *)0 != pwob; pwob = pwob->pNext) {
         int nErr = WebOpt_PValCopy(&pwob->wo);
         
         if (SUCCESS != nErr) { /* record an error, but keep trying to copy */
            nRet = nErr;
         }
      }
      for (i = 0; me->awozList[i].nId != WEBOPT_END; i++) {
         int nErr = WebOpt_PValCopy(&me->awozList[i]);
         
         if (SUCCESS != nErr) { /* record an error, but keep trying to copy */
            nRet = nErr;
         }
      }
   }

   return nRet;
}

int WebOptMgr_CtorZ(WebOptMgr *me, WebOpt *awozList)
{
   int nOpts = WebOpt_Count(&awozList);

   me->nNumOpts = nOpts;
   me->pNext = 0;

   // add these in forward order, just like a group add
   {
      WebOpt *pwo = me->awozList;
      boolean bCopy = FALSE;

      while (nOpts) {
         WebOpt wo = *awozList++;
         
         if (wo.nId > WEBOPT_RESERVED_LAST) {
            nOpts--;
            *pwo++ = wo;
         }
      }
      pwo->nId = WEBOPT_END;

      for (pwo = awozList; !bCopy && pwo->nId != WEBOPT_END; pwo++) {
         if (pwo->nId == WEBOPT_COPYOPTS) {
            bCopy = pwo->pVal ? TRUE : FALSE;
         }
      }

      return WebOptMgr_SetCopy(me,bCopy);
   }
}

void WebOptMgr_Dtor(WebOptMgr *me)
{
   WebOptBlock *pwob;
   int32        i;

   // free the added options first
   pwob = me->pNext;
   while ((WebOptBlock *)0 != pwob) {
      WebOptBlock *pTmp;

      if (me->bCopy) {
         WebOpt_PValFree(&pwob->wo);
      }

      pTmp = pwob;
      pwob = pwob->pNext;
      FREE(pTmp);
   }

   // free the constructed options next
   if (me->bCopy) {
      for (i = 0; me->awozList[i].nId != WEBOPT_END; i++) {
         WebOpt_PValFree(&me->awozList[i]);
      }
   }
}


int WebOptMgr_AddOptV(WebOptMgr *me, WebOpt *awozList)
{
   int     nRv = SUCCESS;
   WebOpt *pwo;

   /* run to end so we can run next loop backwards */
   for (pwo = awozList; WEBOPT_END != pwo->nId; pwo++);

   /* have to run this loop backwards to effect "push" semantics */
   while ((SUCCESS == nRv) && (pwo != awozList)) {

      pwo--;

      /* apply copyopts */
      if (pwo->nId == WEBOPT_COPYOPTS) {
         nRv = WebOptMgr_SetCopy(me, (boolean)(pwo->pVal ? TRUE : FALSE));
         continue;
      }

      /* skip other non-addables */
      if (pwo->nId <= WEBOPT_RESERVED_LAST) {
         continue;
      }

      {
         WebOptBlock *pwob = MALLOCREC(WebOptBlock);
            
         if ((WebOptBlock *)0 == pwob) {
            nRv = ENOMEMORY;
            break;
         }

         // insert element at front
         pwob->pNext = me->pNext;
         me->pNext = pwob;
      }
         
      me->pNext->wo = *pwo;
      me->nNumOpts++;
         
      if (me->bCopy) {
         nRv = WebOpt_PValCopy(&me->pNext->wo);
      }
   }

   return nRv;
}



int WebOptMgr_RemoveOpt(WebOptMgr *me, int32 nOptId, int32 nIndex)
{
  return WebOptMgr_OptSearch(me, nOptId, nIndex, 0);
}

int WebOptMgr_GetOpt(WebOptMgr *me, int32 nOptId, int32 nIndex, WebOpt *pwo)
{
   if ((WebOpt*)0 == pwo) {
      return EBADPARM;
   }
   return WebOptMgr_OptSearch(me, nOptId, nIndex, pwo);
}

///////////////////////////////////////////////////////////////////////////
// not necessary, yet
///////////////////////////////////////////////////////////////////////////
//
//void WebOptMgr_Delete(WebOptMgr *me)
//{
//   WebOptMgr_Dtor(me);
//   
//   FREE(me);
//}
//
//WebOptMgr *WebOptMgr_New(WebOpt *awozList)
//{
//   WebOptMgr *me = (WebOptMgr *)MALLOC(WebOptMgr_GetSize(awozList));
//
//   if ((WebOptMgr *)0 != me) {
//      if (SUCCESS != WebOptMgr_CtorZ(me, awozList)) {
//         WebOptMgr_Dtor(me);
//         FREEIF(me); /* sets me to 0 */
//      }
//   }
//
//   return me;
//}

/*====================================================================
   end WebOptMgr.c stuff
  ===================================================================*/

/*====================================================================
   IWebOpt stuff (default implementation for extension clients to leverage)
  ===================================================================*/


static void IWebOpts_Delete(IWebOpts *me)
{
   WebOptMgr_Dtor(me->pwom);
   FREEIF(me);
}

static uint32 IWebOpts_AddRef(IWebOpts *me)
{
   return ++me->uRef;
}

static uint32 IWebOpts_Release(IWebOpts *me)
{
   uint32 uRef = --me->uRef;
   
   if (0 == uRef) {
      IWebOpts_Delete(me);
   }
   return uRef;
}

static int IWebOpts_QueryInterface(IWebOpts *me, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) || (id == AEECLSID_WEBOPTS)) {
      *ppo = me;
      IWebOpts_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int IWebOpts_AddOpt(IWebOpts *me, WebOpt *awozList)
{
   return WebOptMgr_AddOptV(me->pwom,awozList);
}

static int IWebOpts_RemoveOpt(IWebOpts *me, int32 nOptId, int32 nIndex)
{
   return WebOptMgr_RemoveOpt(me->pwom,nOptId, nIndex);
}

static int IWebOpts_GetOpt(IWebOpts *me, int32 nOptId, int32 nIndex, WebOpt *pwo)
{
   return WebOptMgr_GetOpt(me->pwom,nOptId, nIndex, pwo);
}

static int IWebOpts_CreateInstance(IShell *piShell, AEECLSID cls, void **ppif)
{
   IWebOpts *me = 0;
   int       nErr;
   
   nErr = ERR_MALLOCREC_EX(IWebOpts,WebOptMgr_GetSize(0),&me);
   
   if (SUCCESS == nErr) {
      me->pvt = &me->vt;

      me->uRef = 1;
      {
         WebOpt awoz[2];
         
         awoz[0].nId  = WEBOPT_COPYOPTS; /* set copyopts by default */
         awoz[0].pVal = (void *)TRUE;

         awoz[1].nId  = WEBOPT_END;

         me->pwom = (WebOptMgr *)(me + 1);
         
         WebOptMgr_CtorZ(me->pwom,awoz); /* doesn't fail if no options */
      }
      me->vt.AddRef          = IWebOpts_AddRef;
      me->vt.Release         = IWebOpts_Release;
      me->vt.QueryInterface  = IWebOpts_QueryInterface;
      me->vt.AddOpt          = IWebOpts_AddOpt;
      me->vt.RemoveOpt       = IWebOpts_RemoveOpt;
      me->vt.GetOpt          = IWebOpts_GetOpt;
   }

   *ppif = me;

   return nErr;
}


#ifdef AEE_STATIC

#include "AEE_OEM.h"
#include "AEEWebOpts_static.h"
/* insert me in OEMModTable, in AEEStaticClasses */
const AEEStaticClass gascIWebOpts[] = 
{
   {AEECLSID_WEBOPTS, 0, 0, NULL, IWebOpts_CreateInstance},
   {0},
};

#endif /* ifdef AEE_STATIC */

