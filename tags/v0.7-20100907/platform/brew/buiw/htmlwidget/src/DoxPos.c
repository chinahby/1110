/*======================================================

FILE: DoxPos.c

SERVICES: 
   IDoxPos

GENERAL DESCRIPTION:
   IDoxPos document traversal interface

   Reproduction and/or distribution of this file without the written consent of
   QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEBase.h"
#include "AEEStdLib.h"
#include "wutil.h"
#include "DoxPos.h"
//#include "FormControl.h"


/////////////////////////////////////////////////////////////////////////////
// IDoxUtil

struct IDoxUtil {
   AEEBASE_INHERIT(IDoxUtil, IDoxPos);
};


/////////////////////////////////////////////////////////////////////////////
// IDoxPos


struct IDoxPos {
   VTBL(IDoxPos)*    pvt;
   IDoxUtil          w;
   int               nRef;
   Dox *             pdox;
   int               pos;
};


#define DOXPOS_P(pdp)            ((pdp)->pdox->pcBuf + (pdp)->pos)
#define DOXPOS_PMAX(pdp)         ((pdp)->pdox->pcBuf + (pdp)->pdox->cbData)
#define DOXPOS_GETPOS(pdp,pch)   ((pch) - (pdp)->pdox->pcBuf)

#define IS_ELEM(pc)              (*(pc) == DOX_SE)
#define IS_ELEMEND(pc)           (*(pc) == DOX_EE)

//////////////////////////////////////////////////////////////////////////
// forward decls


// IDoxPos interface methods
static uint32  IDoxPos_AddRef          (IDoxPos *me);
static uint32  IDoxPos_Release         (IDoxPos *me);
static int     IDoxPos_QueryInterface  (IDoxPos *me, AEECLSID id, void **ppo);
static int     IDoxPos_Clone           (IDoxPos *me, IDoxPos **ppNew);
static boolean IDoxPos_NextSibling     (IDoxPos *me);
static boolean IDoxPos_PrevSibling     (IDoxPos *me);
static boolean IDoxPos_Parent          (IDoxPos *me);
static boolean IDoxPos_Child           (IDoxPos *me);
static boolean IDoxPos_NextDocOrder    (IDoxPos *me, int *pnLevel);
static boolean IDoxPos_PrevDocOrder    (IDoxPos *me, int *pnLevel);
static int     IDoxPos_GetElementName  (IDoxPos *me, AECHAR *pwBuf, int cchBuf);
static int     IDoxPos_GetContent      (IDoxPos *me, AECHAR *pwBuf, int cchBuf, int nchIndex, int *pnUsed);
static int     IDoxPos_GetNameByIndex  (IDoxPos *me, int nIndex, AECHAR *pwBuf, int cchBuf);
static int     IDoxPos_GetValueByIndex (IDoxPos *me, int nIndex, AECHAR *pwBuf, int cchBuf);
static int     IDoxPos_GetAttrValue    (IDoxPos *me, const AECHAR *pwszAttr, AECHAR *pwBuf, int cchBuf);
static int     IDoxPos_GetAttrInt      (IDoxPos *me, const AECHAR *pwszAttr, int nDefault);
static boolean IDoxPos_IsAttrValue     (IDoxPos *me, const AECHAR *pwszAttr, const AECHAR *pwszValue);
static boolean IDoxPos_IsElement       (IDoxPos *me);
static boolean IDoxPos_IsElementName   (IDoxPos *me, const AECHAR *pwszElem);
static boolean IDoxPos_GetObject       (IDoxPos *me, IFormControl **ppo);
//static int     IDoxPos_MakeURL         (IDoxPos *me, const AECHAR *pwszAttr, char *pBuf, int cchBuf);


// other functions
static const char *  IDoxPos_NextAttr  (IDoxPos *me, const char *pch);
static const char *  IDoxPos_FindAttrW (IDoxPos *me, const AECHAR *pwszAttr, int *pnIndex);
static const char *  IDoxPos_FindAttr  (IDoxPos *me, const char *pszAttr, int *pnIndex);
static const char *  IDoxPos_FromIndex (IDoxPos *me, int nIndex);
static int           IDoxPos_GetValue  (IDoxPos *me, const char *pch, AECHAR *pwBuf, int cchBuf);
static boolean       IDoxPos_IsElement (IDoxPos *me);

static const char *  dox_NextItem      (const char *pch, const char *pchMax, int *pnLevel);
static const char *  dox_PrevItem      (const char *pch, int *pnLevel);

// IDoxUtil interface methods
static int     IDoxUtil_GetIWeb        (IDoxUtil *po, IWeb **ppo);
static int     IDoxUtil_MakeUrl        (IDoxUtil *po, const char *pszRel, char *pBufOut, int cbBufOut);
static int     IDoxUtil_Encode         (IDoxUtil *po, char *pcDest, int cbDest, const AECHAR *pwcSrc,
                                        int cwSrc, int *pnUsed, boolean bTerminate);
static int     IDoxUtil_Decode         (IDoxUtil *po, AECHAR *pwcDest, int cwDest, const char *pcSrc,
                                        int cbSrc, int *pcbUsed, boolean bTerminate);
static int     IDoxUtil_GetImage       (IDoxUtil *po, int16 idImage, IImage **ppii);
static uint32  IDoxUtil_GetFlags       (IDoxUtil *po);

static void    IDoxUtil_IncrementImageCount  (IDoxUtil *po, boolean bIncrement);
static AECHAR* IDoxUtil_GetTempBuffer  (IDoxUtil *po, int *pnSize);


//////////////////////////////////////////////////////////////////////////
// utils


__inline
boolean WSTREQI(const AECHAR *s1, const AECHAR *s2)
{
   return (s1 && s2 && WSTRICMP(s1,s2) == 0);
}

__inline
boolean STREQI(const char *s1, const char *s2)
{
   return (s1 && s2 && STRICMP(s1,s2) == 0);
}


// allocates an object with 2 appended vtbls
#define MALLOCREC_VTBLS(typ,vtt1,vtt2) \
   MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt1)) + sizeof(AEEVTBL(vtt2)) )

#define GETVTBL1(po,vtt1) \
   ((AEEVTBL(vtt1)*)(void *)((po)+1))

#define GETVTBL2(po,vtt1,vtt2) \
   ( (AEEVTBL(vtt2)*) ((byte*)((po) + 1) + sizeof(AEEVTBL(vtt1))) )

  
#define AEEBASE_INIT_IQI(me,fld,pv,iname) \
   AEEBASE_INIT(me,fld,pv);\
   (pv)->AddRef         = AEEBASE_AddRef(iname);\
   (pv)->Release        = AEEBASE_Release(iname);\
   (pv)->QueryInterface = AEEBASE_QueryInterface(iname)
   

//////////////////////////////////////////////////////////////////////////
// IDoxPos creation


/////////////////////////////////////////////////////////////////////////////
// Isolate all of the Static vs. Dynamic crap here
/////////////////////////////////////////////////////////////////////////////
#if 0
//#if defined(AEE_STATIC)

/*
|| Static version
*/
static int IDoxPos_NewVT(IDoxPos **ppo)
{
   // IDoxUtil vtable
   static AEEVTBL(IDoxUtil) gvtDoxUtil = {
       AEEBASE_AddRef(IDoxUtil)
      ,AEEBASE_Release(IDoxUtil)
      ,AEEBASE_QueryInterface(IDoxUtil)
      ,IDoxUtil_GetIWeb
      ,IDoxUtil_MakeUrl
      ,IDoxUtil_Encode
      ,IDoxUtil_Decode
      ,IDoxUtil_IncrementImageCount
      ,IDoxUtil_GetImage
      ,IDoxUtil_GetFlags
      ,IDoxUtil_GetTempBuffer
   };

   // IDoxPos vtable
   static AEEVTBL(IDoxPos) gvtDoxPos = {
       IDoxPos_AddRef
      ,IDoxPos_Release
      ,IDoxPos_QueryInterface
      ,IDoxPos_Clone
      ,IDoxPos_NextSibling
      ,IDoxPos_PrevSibling
      ,IDoxPos_Parent
      ,IDoxPos_Child
      ,IDoxPos_NextDocOrder
      ,IDoxPos_PrevDocOrder
      ,IDoxPos_GetElementName
      ,IDoxPos_GetContent
      ,IDoxPos_GetNameByIndex
      ,IDoxPos_GetValueByIndex
      ,IDoxPos_GetAttrValue
      ,IDoxPos_GetAttrInt
      ,IDoxPos_IsAttrValue
      ,IDoxPos_IsElement
      ,IDoxPos_IsElementName
      ,IDoxPos_GetObject
   //   ,IDoxPos_MakeURL
   };

   IDoxPos *me = MALLOCREC(IDoxPos);
   *ppo = me;
   if (!me) {
      return ENOMEMORY;
   }

   // 
   me->pvt = &gvtDoxPos;

   // initialize the AEEBase interface
   AEEBASE_INIT(me, w, &gvtDoxUtil);

   return SUCCESS;
}

#else    // AEE_STATIC

/* 
|| Dynamic version
*/
static int IDoxPos_NewVT(IDoxPos **ppo)
{
   AEEVTBL(IDoxPos) *pvt1;
   AEEVTBL(IDoxUtil) *pvt2;

   IDoxPos *me = MALLOCREC_VTBLS(IDoxPos, IDoxPos, IDoxUtil);
   *ppo = me;
   if (!me) {
      return ENOMEMORY;
   }

   pvt1 = GETVTBL1(me,IDoxPos);
   me->pvt = (AEEVTBL(IDoxPos)*)pvt1;
   
#define VTFUNC(name)    pvt1->name = IDoxPos_##name
   VTFUNC( AddRef );
   VTFUNC( Release );
   VTFUNC( QueryInterface );
   VTFUNC( Clone );
   VTFUNC( NextSibling );
   VTFUNC( PrevSibling );
   VTFUNC( Parent );
   VTFUNC( Child );
   VTFUNC( NextDocOrder );
   VTFUNC( PrevDocOrder );
   VTFUNC( GetElementName );
   VTFUNC( GetContent );
   VTFUNC( GetNameByIndex );
   VTFUNC( GetValueByIndex );
   VTFUNC( GetAttrValue );
   VTFUNC( GetAttrInt );
   VTFUNC( IsAttrValue );
   VTFUNC( IsElement );
   VTFUNC( IsElementName );
   VTFUNC( GetObject );
#undef VTFUNC

   pvt2 = GETVTBL2(me,IDoxPos,IDoxUtil);
   
   // init vtbl ptr and assign IQueryInterface methods
   AEEBASE_INIT_IQI(me, w, pvt2, IDoxUtil);

#define VTFUNC(name)    pvt2->name = IDoxUtil_##name
   VTFUNC( GetIWeb );
   VTFUNC( MakeUrl );
   VTFUNC( Encode  );
   VTFUNC( Decode  );
   VTFUNC( IncrementImageCount);
   VTFUNC( GetImage );
   VTFUNC( GetFlags );
   VTFUNC( GetTempBuffer );

#undef VTFUNC

   return SUCCESS;
}

#endif   // AEE_STATIC

/////////////////////////////////////////////////////////////////////////////


/* public, but not part of interface */
IDoxPos * IDoxPos_New(Dox *pdox, int pos)
{
   IDoxPos *me;
   int nErr = IDoxPos_NewVT(&me);
    
   if (!nErr && me) {
      me->nRef    = 1;
      me->pdox    = pdox;
      me->pos     = pos;
      return me;
   }

   return 0;
}


/* private */
static void IDoxPos_Delete(IDoxPos *me)
{
   FREE(me);
}


/////////////////////////////////////////////////////////////////////////////
// IDoxPos interface methods


/* IDoxPos_AddRef
|| public
*/
static uint32 IDoxPos_AddRef(IDoxPos *me)
{
   return ++me->nRef;
}


/* IDoxPos_Release
|| public
*/
static uint32 IDoxPos_Release(IDoxPos *me)
{
   uint16 nRef = --me->nRef;

   if (nRef == 0) {
      IDoxPos_Delete(me);
   }
   return nRef;
}

/* IDoxPos_QueryInterface
|| public
*/
static int IDoxPos_QueryInterface(IDoxPos *me, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) || (id == AEEIID_DOXPOS)) {
      *ppo = me;
   } else if (id == AEEIID_DOXUTIL) {
      *ppo = &me->w;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }

   IDoxPos_AddRef(me);
   return SUCCESS;
}


/* IDoxPos_Clone
|| public
*/
static int IDoxPos_Clone(IDoxPos *me, IDoxPos **ppNew)
{
   IDoxPos *pNew = IDoxPos_New(me->pdox, me->pos);
   *ppNew = pNew;
   return (pNew ? SUCCESS : ENOMEMORY);
}


/* IDoxPos_Next
|| public
|| Navigate to next sibling node
*/
static boolean IDoxPos_NextSibling(IDoxPos *me)
{
   const char *pch = DOXPOS_P(me);
   const char *pchMax = DOXPOS_PMAX(me);
   int nLevel = 0;
   
   while (pch < pchMax && nLevel >= 0) {
      pch = dox_NextItem(pch, pchMax, &nLevel);
      if (nLevel == 0) {
         me->pos = DOXPOS_GETPOS(me, pch);
         return TRUE;
      }
   }
   return FALSE;
}


/* IDoxPos_Prev
|| public
|| Navigate to previous sibling node
*/
static boolean IDoxPos_PrevSibling(IDoxPos *me)
{
   const char *pch = DOXPOS_P(me);
   const char *pchStart = me->pdox->pcBuf;
   int nLevel = 0;

   while (pch > pchStart && nLevel >= 0) {
      pch = dox_PrevItem(pch, &nLevel);
      if (nLevel == 0) {
         me->pos = DOXPOS_GETPOS(me, pch);
         return TRUE;
      }
   }
   return FALSE;
}


/* IDoxPos_Parent
|| public
|| Navigate to parent node
*/
static boolean IDoxPos_Parent(IDoxPos *me)
{
   // back up until level changes
   const char *pch = DOXPOS_P(me);
   const char *pchStart = me->pdox->pcBuf;
   int nLevel = 0;

   while (pch > pchStart) {
      pch = dox_PrevItem(pch, &nLevel);
      if (nLevel < 0) {
         me->pos = DOXPOS_GETPOS(me, pch);
         return TRUE;
      }
   }
   return FALSE;
}


/* IDoxPos_Child
|| public
|| Navigate to first child node (if existant)
*/
static boolean IDoxPos_Child(IDoxPos *me)
{
   const char *pch = DOXPOS_P(me);
   const char *pchMax = DOXPOS_PMAX(me);
   int nLevel = 0;
   
   if (pch < pchMax && IS_ELEM(pch)) {
      pch = dox_NextItem(pch, pchMax, &nLevel);
      if (nLevel > 0) {
         me->pos = DOXPOS_GETPOS(me, pch);
         return TRUE;
      }

   }
   return FALSE;
}


/* IDoxPos_NextDocOrder
|| public
*/
static boolean IDoxPos_NextDocOrder(IDoxPos *me, int *pnLevel)
{
   const char *pch = DOXPOS_P(me);
   const char *pchMax = DOXPOS_PMAX(me);

   if (pch >= pchMax) {
      return FALSE;
   }

   pch = dox_NextItem(pch, pchMax, pnLevel);

   if (pch >= pchMax) {
      return FALSE;
   }

   me->pos = DOXPOS_GETPOS(me, pch);

   return TRUE;
}


/* IDoxPos_PrevDocOrder
|| public
*/
static boolean IDoxPos_PrevDocOrder(IDoxPos *me, int *pnLevel)
{
   const char *pch = DOXPOS_P(me);
   const char *pchStart = me->pdox->pcBuf;

   if (pch <= pchStart) {
      return FALSE;
   }

   pch = dox_PrevItem(pch, pnLevel);
   me->pos = DOXPOS_GETPOS(me, pch);

   return TRUE;
}


/* IDoxPos_GetElementName
|| public
*/
static int IDoxPos_GetElementName(IDoxPos *me, AECHAR *pwBuf, int cchBuf)
{
   const char *pch = DOXPOS_P(me);

   if (IS_ELEM(pch)) {
      const char *pszName = Dox_GetElemName(me->pdox, IDFromChar(*(pch+1)));
      return Dox_DecodeTerminate(me->pdox, pwBuf, cchBuf, pszName, -1, NULL);
   }

   return 0;
}


/* IDoxPos_GetContent
|| public
|| Note that IDoxPos_GetContent does not zero terminate the content string
*/
static int IDoxPos_GetContent(IDoxPos *me, AECHAR *pwBuf, int cchBuf, int nchIndex, int *pnUsed)
{
   const char *pch = DOXPOS_P(me);

   // check if we are on an element
   if (!IS_ELEM(pch)) {

      const char *pe;
      int cbLen;
      
      // find the end of the content
      for (pe=pch; !DoxIsElemDelim(*pe); pe++) 
         ;

      cbLen = pe - pch;

      // check if nchIndex is valid
      if (nchIndex < cbLen) {
         
         pch += nchIndex;
         cbLen -= nchIndex;

         // do decode mojo here
         return Dox_Decode(me->pdox, pwBuf, cchBuf, pch, cbLen, pnUsed);
      }
   }

   return 0;
}


/* IDoxPos_GetNameByIndex
|| public
*/
static int IDoxPos_GetNameByIndex(IDoxPos *me, int nIndex, AECHAR *pwBuf, int cchBuf)
{
   const char *pszName = NULL;
   const char *pch = IDoxPos_FromIndex(me, nIndex);
   
   if (!pch) {
      return 0;
   }

   // *pch is AttrID
   pszName = Dox_GetAttrName(me->pdox, IDFromChar(*pch));
   return Dox_DecodeTerminate(me->pdox, pwBuf, cchBuf, pszName, -1, NULL);
}


/* IDoxPos_GetValueByIndex
|| public
*/
static int IDoxPos_GetValueByIndex(IDoxPos *me, int nIndex, AECHAR *pwBuf, int cchBuf)
{
   const char *pch = IDoxPos_FromIndex(me, nIndex);
   return IDoxPos_GetValue(me, pch, pwBuf, cchBuf);
}


/* IDoxPos_GetAttrValue
|| public
*/
static int IDoxPos_GetAttrValue(IDoxPos *me, const AECHAR *pwszAttr, AECHAR *pwBuf, int cchBuf)
{
   const char *pch = IDoxPos_FindAttrW(me, pwszAttr, NULL);
   return IDoxPos_GetValue(me, pch, pwBuf, cchBuf);
}


/* IDoxPos_IsElement
|| public
*/
static boolean IDoxPos_IsElement(IDoxPos *me)
{
   return IS_ELEM(DOXPOS_P(me));
}


/* IDoxPos_IsElementName
|| public
*/
static boolean IDoxPos_IsElementName(IDoxPos *me, const AECHAR *pwszElem)
{
   const char *pch = DOXPOS_P(me);
   boolean bMatch = 0;
   
   if (IS_ELEM(pch)) {
      const char *pszName;

      char aBuf[20];
      Dox_EncodeTerminate(me->pdox, aBuf, ARRAYSIZE(aBuf), pwszElem, -1, NULL);
      
      pszName = Dox_GetElemName(me->pdox, IDFromChar(*(pch+1)));
      bMatch = STREQI(aBuf, pszName);

//      LOCALSTR_FREEIF(&s);
   }
   return bMatch;
}


/* IDoxPos_IsAttrValue
|| public
*/
static boolean IDoxPos_IsAttrValue(IDoxPos *me, const AECHAR *pwszAttr, const AECHAR *pwszValue)
{
   const char *pch = DOXPOS_P(me);
   boolean bMatch = 0;
   
   if (IS_ELEM(pch)) {

      pch = IDoxPos_FindAttrW(me, pwszAttr, NULL);
      if (pch) {
         AECHAR awBuf[20];
         IDoxPos_GetValue(me, pch, awBuf, ARRAYSIZE(awBuf));
         bMatch = WSTREQI(awBuf, pwszValue);
      }
   }

   return bMatch;
}


/* IDoxPos_GetObject
|| public
*/
static boolean IDoxPos_GetObject(IDoxPos *me, IFormControl **ppo)
{
   const char *pch = DOXPOS_P(me);

   if (*pch == DOX_SE) {
      *ppo = xDox_ObtainObject(me->pdox, me->pos);
      if (*ppo) {
         IFORMCONTROL_AddRef(*ppo);
         return 1;
      }
   }

   return 0;
}


/* IDoxPos_GetAttrInt
|| public
|| returns integer attribute value or value of 'nDefault' if not found
*/
static int IDoxPos_GetAttrInt(IDoxPos *me, const AECHAR *pwszAttr, int nDefault)
{
   const char *pch = IDoxPos_FindAttrW(me, pwszAttr, NULL);
   if (pch && pch[1] != DOX_NV) {
      return ATOI(pch+1);
   }
   return nDefault;
}


/* IDoxPos_MakeURL
|| public
|| if pBuf!=NULL and cchBuf>0, returns number of chars written or 0 if attribute not found
|| if pBuf==NULL or cchBuf==0, returns sizeof required buffer or 0 if attribute not found
*/
//static int IDoxPos_MakeURL(IDoxPos *me, const AECHAR *pwszAttr, char *pBuf, int cchBuf)
//{
//   const char *pch = IDoxPos_FindAttrW(me, pwszAttr, NULL);
//   if (pch && pch[1] != DOX_NV) {
//      return Dox_MakeURL(me->pdox, me->pos, pch+1, pBuf, cchBuf);
//   }
//   return 0;
//}


/////////////////////////////////////////////////////////////////////////////
// helper functions 

// Note: meaning of *pnLevel may not be intuitive. *pnLevel is incremented
//       when descending into children elements, and decremented when ascending
//       up to parent elements
static const char * dox_NextItem(const char *pch, const char *pchMax, int *pnLevel)
{
   int level = 0;

   // we are descending a level if 
   // pointing to element-start on entry
   if (IS_ELEM(pch)) {
      level++;
   }

   while (pch < pchMax) {
      pch = xDoxNextItem(pch, pchMax);
      if (!IS_ELEMEND(pch)) {
         break;
      }
      level--;
   }

   if (pnLevel) {
      *pnLevel += level;
   }

   return pch;
}


// Note: meaning of *pnLevel may not be intuitive. *pnLevel is incremented
//       when descending into children elements, and decremented when ascending
//       up to parent elements
static const char * dox_PrevItem(const char *pch, int *pnLevel)
{
   int level = 0;

   while (1) {
      pch = xDoxPrevItem(pch);
      if (!IS_ELEMEND(pch)) {
         break;
      }
      // skip end-element and count levels
      level++;
   }

   // we ascended a level if pointing 
   // to element-start on exit
   if (IS_ELEM(pch)) {
      level--;
   }

   if (pnLevel) {
      *pnLevel += level;
   }

   return pch;
}


/* private */
static const char *IDoxPos_NextAttr(IDoxPos *me, const char *pch)
{
   // pch can be pointing at end of attr list
   if (!DoxIsAttrDelim(*pch)) {

   // pch can be pointing at an element
      if (IS_ELEM(pch) || pch[1] == DOX_NV) {
         // pointing to element or at attribute with
         // no value, skip to start of element
         pch += 2;
      } else {
         // skip value string
         do {
            ++pch;
         } while (*pch);
         ++pch;
      }
   }

   if (DoxIsAttrDelim(*pch)) {
      pch = NULL;
   }

   return pch;
}


/* private */
static const char *IDoxPos_FindAttrW(IDoxPos *me, const AECHAR *pwszAttr, int *pnIndex)
{
   char aBuf[32];

   // encode pwszAttr to single-byte
   Dox_EncodeTerminate(me->pdox, aBuf, ARRAYSIZE(aBuf), pwszAttr, -1, NULL);
   return IDoxPos_FindAttr(me, aBuf, pnIndex);
}

/* private */
static const char *IDoxPos_FindAttr(IDoxPos *me, const char *pszAttr, int *pnIndex)
{
   const char *pch = DOXPOS_P(me);
   const char *pchRet = NULL;
   
   int nIndex;

   if (!IS_ELEM(pch)) {
      return NULL;
   }

   for (nIndex=0; 0 != (pch = IDoxPos_NextAttr(me, pch)); nIndex++) {
      // *pch is AttrID
      const char *pszName = Dox_GetAttrName(me->pdox, IDFromChar(*pch));
      // compare to single-byte version of pwszAttr here
      if (0 == STRICMP(pszAttr, pszName)) {
         pchRet = pch;
         break;
      }
   }

   if (pnIndex) {
      *pnIndex = nIndex;
   }

   return pchRet;
}


/* private */
static const char *IDoxPos_FromIndex(IDoxPos *me, int nIndex)
{
   const char *pch = DOXPOS_P(me);
   
   if (!IS_ELEM(pch)) {
      return NULL;
   }

   nIndex++;

   while (nIndex-- && (0 != (pch = IDoxPos_NextAttr(me, pch))) ) {
      if (DoxIsAttrDelim(*pch)) {
         return NULL;
      }
   }

   return pch;
}


/* private */
static int IDoxPos_GetValue(IDoxPos *me, const char *pch, AECHAR *pwBuf, int cchBuf)
{
   if (!pch) {
      return 0;
   } 

   if (pch[1] == DOX_NV) {
      pch = Dox_GetAttrName(me->pdox, IDFromChar(*pch));
   } else {
      pch++;
   }

   return Dox_DecodeTerminate(me->pdox, pwBuf, cchBuf, pch, -1, NULL);
}



/////////////////////////////////////////////////////////////////////////////
// IDoxUtil methods


static int IDoxUtil_GetIWeb(IDoxUtil *po, IWeb **ppo)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   xDox_GetIWeb(me->pdox, ppo);
   return (*ppo != NULL) ? SUCCESS : EFAILED;
}


static int IDoxUtil_MakeUrl(IDoxUtil *po, const char *pszRel, char *pBufOut, int cbBufOut)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   return xDox_MakeUrl(me->pdox, me->pos, pszRel, pBufOut, cbBufOut);
}

static int IDoxUtil_Encode(IDoxUtil *po, char *pcDest, int cbDest, const AECHAR *pwcSrc,
                           int cwSrc, int *pnUsed, boolean bTerminate)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   if (bTerminate) {
      return CharSet_EncodeTerminate(&me->pdox->cs, pcDest, cbDest, pwcSrc, cwSrc, pnUsed);
   } else {
      return CharSet_Encode(&me->pdox->cs, pcDest, cbDest, pwcSrc, cwSrc, pnUsed);
   }
}


static int IDoxUtil_Decode(IDoxUtil *po, AECHAR *pwcDest, int cwDest, const char *pcSrc,
                           int cbSrc, int *pcbUsed, boolean bTerminate)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   if (bTerminate) {
      return CharSet_DecodeTerminate(&me->pdox->cs, pwcDest, cwDest, pcSrc, cbSrc, pcbUsed);
   } else {
      return CharSet_Decode(&me->pdox->cs, pwcDest, cwDest, pcSrc, cbSrc, pcbUsed);
   }
}


static void IDoxUtil_IncrementImageCount(IDoxUtil *po, boolean bIncrement)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   me->pdox->nRefImages += (bIncrement ? 1 : -1);
}

// get a cached IImage from the DoxUtil
static int IDoxUtil_GetImage(IDoxUtil *po, int16 idImage, IImage **ppii)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   return Dox_GetImage(me->pdox, idImage, ppii);
}


static uint32 IDoxUtil_GetFlags(IDoxUtil *po)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   return Dox_GetFlags(me->pdox);
}


static AECHAR* IDoxUtil_GetTempBuffer(IDoxUtil *po, int *pnSize)
{
   IDoxPos *me = (IDoxPos*)po->pMe;
   return Dox_GetTempBuffer(me->pdox, pnSize);
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Unit test

//#define DOX_UNITTEST

#ifdef DOX_UNITTEST

static void DoxPos_Dump(IDoxPos *me, int nLevel)
{
   AECHAR wBuf[128];
   char buf[128];
   int used;
   int c1;
   int c2;
   int i;
   char indent[128];

   indent[0] = 0;
   for (i=0; i < nLevel; i++) {
      STRCAT(indent, "     ");
   }

   if (IDoxPos_IsElement(me)) {

      int nn = 0;

      // get element....
      c1 = IDOXPOS_GetElementName(me, NULL, 0);
      c2 = IDOXPOS_GetElementName(me, wBuf, ARRAYSIZE(wBuf));
      ASSERT(c1==c2);

      me->pdox->dc.pfnEncode(buf, sizeof(buf), wBuf, c2, &used);

      DBGPRINTF("Level:%d  %sELEM<%s>", nLevel, indent, buf);

#if 0
      // enumerate thru the attributes
      do {
         int ss = (ARRAYSIZE(buf) / 2) - 1;
         
         c = IDOXPOS_GetNameByIndex(me, nn, wBuf, ARRAYSIZE(wBuf));

         if (c > 0) {
            me->pdox->dc.pfnEncode(buf, ss, wBuf, c, &used);
            c = IDOXPOS_GetValueByIndex(me, nn, wBuf, ARRAYSIZE(wBuf));
            me->pdox->dc.pfnEncode(buf+ss, ss, wBuf, c, &used);
            DBGPRINTF("Attrib(%d) %s = %s", nn, buf, buf+ss);
         }

         nn++;
      } while (c > 0);
#endif

   } else {

      // get content
      c1 = IDOXPOS_GetContent(me, NULL, 0, 0, NULL);
      c2 = IDOXPOS_GetContent(me, wBuf, ARRAYSIZE(wBuf), 0, NULL);
      ASSERT(c1==c2);

      me->pdox->dc.pfnEncode(buf, sizeof(buf), wBuf, c2, &used);
      DBGPRINTF("Level:%d  %s%s", nLevel, indent, buf);

   }

}


// return last level that was greater than zero
static int DoxPos_InOrderTraversal(IDoxPos *me, boolean bFwd, int nLevel)
{
   int nLastLevel = nLevel;

   do {
   
      DoxPos_Dump(me, nLevel);
      nLastLevel = nLevel;

   } while (bFwd ? IDOXPOS_NextDocOrder(me, &nLevel)
                 : IDOXPOS_PrevDocOrder(me, &nLevel) );

   return nLastLevel;
}


static int DoxPos_TraverseFwd(IDoxPos *me, int nStartLevel)
{
   boolean bDone = 0;
   int nLevel = nStartLevel;
   
   while (!bDone) {
      
      DoxPos_Dump(me, nLevel);
      
      if (IDOXPOS_Child(me)) {
         nLevel++;
         continue;
      }
       
      if (IDOXPOS_NextSibling(me)) {
         continue;
      }

      while (1) {
         if (!IDOXPOS_Parent(me)) {
            bDone = 1;
            break;
         }
         nLevel--;
         if (IDOXPOS_NextSibling(me)) {
            break;
         }
      }
   }
   return nLevel;
}

// traverse to the last in-order descendant of 'me'
static int DoxPos_Descend(IDoxPos *me, int nLevel)
{
   while (1) {
      if (!IDOXPOS_Child(me)) {
         break;
      }
      nLevel++;
      while (IDOXPOS_NextSibling(me)) 
         ;
   }

   return nLevel;
}


static int DoxPos_TraverseBack(IDoxPos *me, int nLevel)
{
   while (1) {
      
      DoxPos_Dump(me, nLevel);
      
      if (IDOXPOS_PrevSibling(me)) {
         nLevel = DoxPos_Descend(me, nLevel);
      } else if (IDOXPOS_Parent(me)) {
         nLevel--;
      } else {
         break;
      }
   }

   return nLevel;
}


const AECHAR *WideString(IDoxPos *me, const char *psz)
{
   static AECHAR awBuffer[80];
   Dox_DecodeTerminate(me->pdox, awBuffer, ARRAYSIZE(awBuffer), psz, -1, NULL);
   return (const AECHAR*)awBuffer;
}

         
boolean IDoxPos_FindElement(IDoxPos *me, const char *pszElement)
{
   while (IDOXPOS_NextDocOrder(me, NULL)) {

      if (IDoxPos_IsElement(me)) {
         
         AECHAR wBuf[80];
         IDOXPOS_GetElementName(me, wBuf, ARRAYSIZE(wBuf));

         if (0 == WSTRICMP(wBuf, WideString(me, pszElement))) {
            return 1;
         }
      }
   }

   return 0;
}

int IDoxPos_GetAttrValueA(IDoxPos *me, const char *pszAttr, char *pc, int cb)
{
   AECHAR awBuffer[80];

   // get values of type, name, value, checked
   if (0 == IDOXPOS_GetAttrValue(me, WideString(me, pszAttr), awBuffer, ARRAYSIZE(awBuffer))) {
      return 0;
   }
   return Dox_EncodeTerminate(me->pdox, pc, cb, awBuffer, -1, NULL);
}



void DoxPos_UnitTest(Dox *pdox, int pos)
{
   IDoxPos *me = IDoxPos_New(pdox, pos);
   if (me) {
      
      int nLevel;
      int nn = 0;

#if 0
      // traverse to <input> element and find attributes
      while (IDoxPos_FindElement(me, "input")) {

         char aBuf[80];
         int ndx;

         DBGPRINTF("%d input", nn++);

//         ndx = IDOXPOS_FindAttrIndex(me, WideString(me, "type"));
//         DBGPRINTF("'type' found at %d", ndx);
//   
//         ndx = IDOXPOS_FindAttrIndex(me, WideString(me, "name"));
//         DBGPRINTF("'name' found at %d", ndx);
//   
//         ndx = IDOXPOS_FindAttrIndex(me, WideString(me, "value"));
//         DBGPRINTF("'value' found at %d", ndx);
//   
//         ndx = IDOXPOS_FindAttrIndex(me, WideString(me, "checked"));
//         DBGPRINTF("'checked' found at %d", ndx);

         // get values of type, name, value, checked
         if (0 < IDoxPos_GetAttrValueA(me, "type", aBuf, sizeof(aBuf))) {
            DBGPRINTF("type=%s", aBuf);
         }

         if (0 < IDoxPos_GetAttrValueA(me, "name", aBuf, sizeof(aBuf))) {
            DBGPRINTF("name=%s", aBuf);
         }

         if (0 < IDoxPos_GetAttrValueA(me, "value", aBuf, sizeof(aBuf))) {
            DBGPRINTF("value=%s", aBuf);
         }

         if (0 < IDoxPos_GetAttrValueA(me, "checked", aBuf, sizeof(aBuf))) {
            DBGPRINTF("checked=%s", aBuf);
         }
      }
#endif

      // Reset IDoxPos to begining of document
      DBGPRINTF("Resetting...");
      IDOXPOS_ResetPos(me);

      // forward in-order traversal
      DBGPRINTF("Forward in-order...");
      nLevel = DoxPos_InOrderTraversal(me, 1, 0);
      
      // Reset IDoxPos to begining of document
      DBGPRINTF("Resetting...");
      IDOXPOS_ResetPos(me);

      // forward in-order traversal
      DBGPRINTF("Forward in-order...");
      nLevel = DoxPos_InOrderTraversal(me, 1, 0);
      
#if 0
      {
         IDoxPos *pdp = NULL;
         int nErr;
   
         nErr = IDOXPOS_Clone(me, &pdp);

         if (SUCCESS != nErr) {
   
            DBGPRINTF("Error IDOXPOS_Clone");
         } else {
            
            // reverse in-order traversal
            DBGPRINTF("Backward in-order...");
            DoxPos_InOrderTraversal(me, 0, nLevel);
   
            // Traverse using nav functions
            DBGPRINTF("Forward hierarchical...");
            DoxPos_TraverseFwd(me, 0);
      
            // Traverse backwards using nav functions
            DBGPRINTF("Backward hierarchical...");
            DoxPos_TraverseBack(pdp, nLevel);
   
            IDOXPOS_Release(pdp);
         }
      }
#endif

      // finished w/ object
      IDOXPOS_Release(me);
   }

}


#endif // DOX_UNITTEST



