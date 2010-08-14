/*========================================================================

FILE:  TextModel.c

SERVICES:  

GENERAL DESCRIPTION:
  Implementation of an ITextModel and IRichTextModel

========================================================================
========================================================================
 
            Copyright © 1999-2007 QUALCOMM Incorporated 
                  All Rights Reserved.
                QUALCOMM Proprietary/GTDR
 
========================================================================
========================================================================*/
#include "AEE.h"
#include "AEESource.h"
#include "TextModel.h"
#include "wutil.h"

#define TEXTMODEL_FROM_IRICHTEXTMODEL      TextModel* me = (TextModel*)po->pMe

typedef enum TokenType
{
   TOKEN_TYPE_UNKNOWN,
   TOKEN_TYPE_LEFT_BRACKET,
   TOKEN_TYPE_TAG,
   TOKEN_TYPE_TEXT,
   TOKEN_TYPE_END
} TokenType;

typedef struct {
   int      nStart;
   int      nLength;
   int      nNameLength;
   AECHAR*  pwName;
} OpaqueTag;

static void ReplaceSelUpdateTags(
   Vector*        pvTags,
   int            nSelStart,
   int            nSelLength,
   int            nNewTextLength);

static void ReplaceSelUpdateTag(
   OpaqueTag*     pOpaqueTag,
   int            nSelStart,
   int            nSelLength,
   int            nNewTextLength);

static int GetIntersectionLength(
   OpaqueTag*     pOpaqueTag,
   int            nSelStart,
   int            nSelLength);

static uint32 NewTag(
   int            nStart,
   int            nLength,
   int            nNameLength,
   const AECHAR*  pwName,
   OpaqueTag**    ppOpaqueTag);

static void FreeTag(
   void*          pItem);

static void GetMarkupTextFromText(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax,
   const AECHAR*  pwText,
   int*           pnTextPos,
   int            nTextLength);

static void GetMarkupTextFromStartTag(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax,
   const AECHAR*  pwTagName,
   int            nTagNameLength);

static void GetMarkupTextFromEndTag(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax);

static uint32 GetToken(
   const AECHAR*  pwMText,
   int            nMTextLength,
   int            nMTextPos,
   TokenType*     pttTokenType,
   int*           pnTokenStart,
   int*           pnTokenLength);

static uint32 TagsGetIndex(
   const Vector*  pvTags,
   const Tag*     pTag,
   uint32*        pnIndex);

/////////////////////////////////////////////////////////////////////////////
// TextModel


int TextModel_New(ITextModel** ppo, IModule* piModule)
{
   TextModel* me = MALLOCREC_VTBL(TextModel, ITextModel);
   
   *ppo = (ITextModel*)me;

   if (!me) {
      return ENOMEMORY;
   }

   TextModel_Ctor(me, GETVTBL(me,ITextModel), piModule);

   // initialize the model buffer
   return MemGrow((void**)&me->ti.pwText, &me->cbBuf, 0, 48);
}


void TextModel_Ctor(TextModel* me, AEEVTBL(ITextModel)* pvt, IModule* piModule)
{
   ModelBase_Ctor(&me->base, (AEEVTBL(IModel)*)pvt, piModule);

   // override vtable methods
   pvt->Release         = TextModel_Release;
   pvt->QueryInterface  = TextModel_QueryInterface;

   // initialize ITextModel methods
   pvt->ReplaceSel   = TextModel_ReplaceSel;
   pvt->GetSelText   = TextModel_GetSelText;
   pvt->SetSel       = TextModel_SetSel;
   pvt->GetTextInfo  = TextModel_GetTextInfo;
}


uint32 TextModel_Release(ITextModel* po)
{
   TextModel* me = (TextModel*)po;

   if (me->base.nRefs == 1) {
      FREEIF(me->ti.pwText);
      if (NULL != me->pRichTextModel) {
         Vector_Dtor(&me->pRichTextModel->vTags);
         FREEIF(me->pRichTextModel);
      }
   }

   return ModelBase_Release((IModel*)me);
}


static void TextModel_InitializeRichTextModel(TextModel* me) {
   // Container functions
   me->pRichTextModel = MALLOCREC_VTBL(RichTextModel, IRichTextModel);

   if ( me->pRichTextModel ) {

      me->pRichTextModel->richTextModel.pvt = &me->pRichTextModel->vtRichTextModel;
      me->pRichTextModel->richTextModel.pMe = me;

      me->pRichTextModel->vtRichTextModel.AddRef         = AEEBASE_AddRef(IRichTextModel);
      me->pRichTextModel->vtRichTextModel.Release        = AEEBASE_Release(IRichTextModel);
      me->pRichTextModel->vtRichTextModel.QueryInterface = AEEBASE_QueryInterface(IRichTextModel);
      me->pRichTextModel->vtRichTextModel.AddListener    = RichTextModel_AddListener;
      me->pRichTextModel->vtRichTextModel.Notify         = RichTextModel_Notify;
      me->pRichTextModel->vtRichTextModel.GetMarkupText  = RichTextModel_GetMarkupText;
      me->pRichTextModel->vtRichTextModel.SetMarkupText  = RichTextModel_SetMarkupText;
      me->pRichTextModel->vtRichTextModel.TagsAdd        = RichTextModel_TagsAdd;
      me->pRichTextModel->vtRichTextModel.TagsSize       = RichTextModel_TagsSize;
      me->pRichTextModel->vtRichTextModel.TagsGetAt      = RichTextModel_TagsGetAt;
      me->pRichTextModel->vtRichTextModel.TagsDeleteAt   = RichTextModel_TagsDeleteAt;
      me->pRichTextModel->vtRichTextModel.TagsDeleteAll  = RichTextModel_TagsDeleteAll;

      Vector_Ctor(&me->pRichTextModel->vTags);
      (void)Vector_SetPfnFree(&me->pRichTextModel->vTags, FreeTag);
   }
}


int TextModel_QueryInterface(ITextModel* po, AEECLSID id, void** ppo)
{
   TextModel* me = (TextModel*)po;

   if (SUCCESS == ModelBase_QueryInterface((IModel*)me, id, ppo)) {
      return SUCCESS;
   }

   if (id == AEEIID_TEXTMODEL) {
      *ppo = me;
      ITEXTMODEL_AddRef(po);
      return SUCCESS;
   }

   if (id == AEEIID_RICHTEXTMODEL) {
      *ppo = me;
      if ( 0 == me->pRichTextModel ) {
         TextModel_InitializeRichTextModel(me);
      }
      if ( me->pRichTextModel ) {
         *ppo = me->pRichTextModel;
         ITEXTMODEL_AddRef(po);
         return SUCCESS;
      }
   }
   return ECLASSNOTSUPPORT;
}


void TextModel_NotifyEvent(IModel* po, uint32 evCode, int pos)
{
   ModelEvent ev;
   ev.evCode   = evCode;
   ev.dwParam  = (uint32)pos;
   IMODEL_Notify(po, &ev);
}


int TextModel_ReplaceSel(ITextModel* po, const AECHAR* pwText, int cchText)
{
   TextModel* me = (TextModel*)po;
   int nErr = SUCCESS;

   int nSelStart, nSelEnd;
   int nSelMin, nSelMax;
   int nSelLen;
   int nGrow, nMove, cch;

   // Check for invalid parameters.
   if ((NULL == pwText) && (cchText != 0)) {
      return EBADPARM;
   }

   if (cchText < 0) {
      cchText = WSTRLEN(pwText);
   }

   nSelStart = me->ti.nSelStart;
   nSelEnd = me->ti.nSelEndCaret;
   cch = me->ti.cchText;
   
   nSelMin = MIN(nSelStart, nSelEnd);
   nSelMax = MAX(nSelStart, nSelEnd);

   nSelLen = nSelMax - nSelMin;

   nMove = cchText - nSelLen;
   nGrow = (nMove > 0) ? nMove : 0;
   
   // Used text size is cch+1 (include zero terminator)
   nErr = MemGrowEx((void**)&me->ti.pwText, &me->cbBuf, 
                    ((cch+1) * sizeof(AECHAR)), (nGrow * sizeof(AECHAR)), 48);

//   nErr = WStr_AppendGrow((AECHAR**)&me->ti.pwText, &me->cbBuf, nGrow, NULL);
   if (SUCCESS == nErr) {

      AECHAR* pw = (AECHAR*)me->ti.pwText;         // start of text

      if (nMove != 0) {
         int nMoveLen = cch - nSelMax + 1;   // include zero terminator
         MEMMOVE(pw + nSelMax + nMove, pw + nSelMax, (nMoveLen * sizeof(AECHAR)) );
      }
      // copy in the new text
      if (cchText) {
         MEMCPY(pw + nSelMin, pwText, (cchText * sizeof(AECHAR)) );
      }

      me->ti.nSelStart = me->ti.nSelEndCaret = nSelMax + nMove;
      me->ti.cchText += nMove;

      if (NULL != me->pRichTextModel) {
         ReplaceSelUpdateTags(&me->pRichTextModel->vTags, nSelMin, nSelLen, cchText);
      }

      TextModel_NotifyEvent((IModel*)po, EVT_MDL_TEXT_CHANGE, nSelMin);
   }

   return nErr;
}


// always returns the number of characters selected
// To obtain the entire selected text, cchBuf must be the
// return value + 1 (because the return buffer is always 
// zero-terminated)
// 
// cchBuf may be set to zero to just get the count of selected chars
// 
int TextModel_GetSelText(ITextModel* po, AECHAR* pwBuf, int cchBuf)
{
   TextModel* me = (TextModel*)po;

   int nSelMin = MIN(me->ti.nSelStart, me->ti.nSelEndCaret);
   int nSelMax = MAX(me->ti.nSelStart, me->ti.nSelEndCaret);
   int cchSel = nSelMax-nSelMin;

   if (cchBuf > 0) {
      int cchCopy = MIN(cchBuf, cchSel+1);
      WSTRLCPY(pwBuf, me->ti.pwText + nSelMin, cchCopy);
   }

   return cchSel;
}


void TextModel_SetSel(ITextModel* po, int nSelStart, int nSelEndCaret)
{
   TextModel* me = (TextModel*)po;
   int nSelMin;

   // lower boundary check
   if (nSelStart < 0) {
      nSelStart = me->ti.cchText;
   }
   if (nSelEndCaret < 0) {
      nSelEndCaret = me->ti.cchText;
   }

   // upper boundary check
   if (nSelStart > me->ti.cchText) {
      nSelStart = me->ti.cchText;
   }
   if (nSelEndCaret > me->ti.cchText) {
      nSelEndCaret = me->ti.cchText;
   }

   me->ti.nSelStart     = nSelStart;
   me->ti.nSelEndCaret  = nSelEndCaret;

   nSelMin = MIN(nSelStart, nSelEndCaret);

   TextModel_NotifyEvent((IModel*)po, EVT_MDL_TEXT_SELECT, nSelMin);
}


void TextModel_GetTextInfo(ITextModel* po, TextInfo* pTextInfo)
{
   TextModel* me = (TextModel*)po;

   pTextInfo->pwText       = me->ti.pwText;
   pTextInfo->cchText      = me->ti.cchText;
   pTextInfo->nSelStart    = me->ti.nSelStart;
   pTextInfo->nSelEndCaret = me->ti.nSelEndCaret;
}


int      RichTextModel_AddListener     (IRichTextModel *p, ModelListener *pListener)
{
   AEEBase* po = (AEEBase*) p;
   return IMODEL_AddListener((IModel*) po->pMe, pListener);
}

void     RichTextModel_Notify          (IRichTextModel *p, ModelEvent *pEvent)
{
   AEEBase* po = (AEEBase*) p;
   IMODEL_Notify((IModel*) po->pMe, pEvent);
}

uint32 RichTextModel_GetMarkupText(IRichTextModel* po, AECHAR* pwMText, int* pnSize)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   int            nErr              = SUCCESS;
   int            nMTextPos         = 0;
   int            nMTextLengthMax   = 0;
   const AECHAR*  pwText            = me->ti.pwText;
   int            nTextPos          = 0;
   int            nTextLength       = me->ti.cchText;
   int            nTextLengthCurr   = 0;
   int            nTagIndex         = 0;
   int            nTagCount         = Vector_Size(&me->pRichTextModel->vTags);

   // Check for invalid parameters.
   if (NULL == pnSize) {
      return EBADPARM;
   }

   if (NULL == pwMText) {
      *pnSize = 0;
   }

   nMTextLengthMax = (*pnSize / sizeof(AECHAR)) - 1;

   // Iterate over the tags.
   nTagIndex = 0;
   while (nTagIndex < nTagCount) {
      const OpaqueTag* pOpaqueTag = NULL;

      // Get the tag.
      nErr = Vector_GetAt(&me->pRichTextModel->vTags, nTagIndex, (void**)&pOpaqueTag);
      if (SUCCESS != nErr) {
         // This is an internal error.
         (void)MEMSET(pwMText, 0, *pnSize);
         *pnSize = 0;
         return nErr;
      }

      // Put the text prior to the start tag into the markup text buffer.
      nTextLengthCurr = MIN(pOpaqueTag->nStart, nTextLength);
      GetMarkupTextFromText(pwMText, &nMTextPos, nMTextLengthMax, pwText, &nTextPos, nTextLengthCurr);

      // Put the start tag into the markup text buffer.
      GetMarkupTextFromStartTag(pwMText, &nMTextPos, nMTextLengthMax, pOpaqueTag->pwName, pOpaqueTag->nNameLength);

      // Put the text between the start and end tags into the markup text buffer.
      nTextLengthCurr = MIN((pOpaqueTag->nStart + pOpaqueTag->nLength), nTextLength);
      GetMarkupTextFromText(pwMText, &nMTextPos, nMTextLengthMax, pwText, &nTextPos, nTextLengthCurr);

      // Put the end tag into the markup text buffer.
      GetMarkupTextFromEndTag(pwMText, &nMTextPos, nMTextLengthMax);

      ++nTagIndex;
   }

   // Put the remaining text into the markup text buffer.
   GetMarkupTextFromText(pwMText, &nMTextPos, nMTextLengthMax, pwText, &nTextPos, nTextLength);

   // Terminate the markup text.
   if (nMTextPos < (nMTextLengthMax + 1)) {
      pwMText[nMTextPos] = '\0';
   }
   ++nMTextPos;

   *pnSize = nMTextPos * sizeof(AECHAR);

   return nErr;
}


uint32 RichTextModel_SetMarkupText(IRichTextModel* po, const AECHAR* pwMText, int nMTextLength)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   int         nErr           = SUCCESS;
   Vector*     pvTags         = &me->pRichTextModel->vTags;
   int         nMTextPos      = 0;
   int         nTagLevel      = 0;
   int         nTagIndex      = 0;
   TokenType   ttTokenType    = TOKEN_TYPE_UNKNOWN;
   int         nTokenStart    = 0;
   int         nTokenLength   = 0;

   // Check for invalid parameters.
   if (NULL == pwMText) {
      return EBADPARM;
   }

   if (nMTextLength < 0) {
      nMTextLength = WSTRLEN(pwMText);
   }

   // Free the text buffer.
   FREEIF(me->ti.pwText);
   me->cbBuf = 0;
   me->ti.cchText = 0;

   // Delete any existing tags.
   Vector_DeleteAll(pvTags);

   // Iterate over the tokens.
   nMTextPos = 0;
   nTagLevel = 0;
   nTagIndex = 0;
   while ((SUCCESS == nErr) && (nMTextPos < nMTextLength)) {
      nErr = GetToken(pwMText, nMTextLength, nMTextPos, &ttTokenType, &nTokenStart, &nTokenLength);
      if (SUCCESS != nErr) {
         continue;
      }

      switch (ttTokenType) {
      case TOKEN_TYPE_LEFT_BRACKET:
         nErr = MemGrowEx((void**)&me->ti.pwText, &me->cbBuf, (me->ti.cchText * sizeof(AECHAR)), sizeof(AECHAR), 48);
         if (SUCCESS != nErr) {
            continue;
         }

         me->ti.pwText[me->ti.cchText] = '[';
         ++me->ti.cchText;
         break;

      case TOKEN_TYPE_TAG:
         {
            int         nTagNameStart  = nTokenStart + 1;
            int         nTagNameLength = nTokenLength - 2;
            OpaqueTag*  pOpaqueTag     = NULL;

            // See if this is a start tag.
            if (nTagNameLength > 0) {
               ++nTagLevel;

               // Currently, only one level of nesting is allowed.
               if (nTagLevel > 1) {
                  nErr = EUNSUPPORTED;
                  continue;
               }

               nErr = NewTag(me->ti.cchText, 0, nTagNameLength, pwMText + nTagNameStart, &pOpaqueTag);
               if (SUCCESS != nErr) {
                  continue;
               }

               nErr = Vector_InsertAt(pvTags, nTagIndex, pOpaqueTag);
               if (SUCCESS != nErr) {
                  FreeTag(pOpaqueTag);
                  continue;
               }
            }

            // Else, see if this is an end tag.
            else if (nTagNameLength == 0) {
               --nTagLevel;

               if (nTagLevel < 0) {
                  nErr = EFAILED;
                  continue;
               }

               nErr = Vector_GetAt(pvTags, nTagIndex, (void**)&pOpaqueTag);
               if (SUCCESS != nErr) {
                  continue;
               }
               pOpaqueTag->nLength = me->ti.cchText - pOpaqueTag->nStart;
               ++nTagIndex;
            }

            // Else, this is an invalid tag.
            else {
               nErr = EFAILED;
               continue;
            }
         }
         break;

      case TOKEN_TYPE_TEXT:
         nErr = MemGrowEx((void**)&me->ti.pwText, &me->cbBuf, (me->ti.cchText * sizeof(AECHAR)), (nTokenLength * sizeof(AECHAR)), 48);
         if (SUCCESS != nErr) {
            continue;
         }

         (void)WSTRLCPY(me->ti.pwText + me->ti.cchText, pwMText + nTokenStart, nTokenLength + 1);
         me->ti.cchText += nTokenLength;
         break;

      case TOKEN_TYPE_END:
         break;

      case TOKEN_TYPE_UNKNOWN:
      default:
         nErr = EFAILED;
         break;
      }

      nMTextPos += nTokenLength;
   }

   if (SUCCESS == nErr) {
      if (nTagLevel != 0) {
         nErr = EFAILED;
      }
   }

   if (SUCCESS == nErr) {
      nErr = MemGrowEx((void**)&me->ti.pwText, &me->cbBuf, (me->ti.cchText * sizeof(AECHAR)), sizeof(AECHAR), 48);
   }

   if (SUCCESS == nErr) {
      // Terminate the text.
      me->ti.pwText[me->ti.cchText] = '\0';

      me->ti.nSelStart     = me->ti.cchText;
      me->ti.nSelEndCaret  = me->ti.cchText;
      TextModel_NotifyEvent((IModel*)me, EVT_MDL_TEXT_CHANGE, 0);
   }

   if (SUCCESS != nErr) {
      // Free the text buffer.
      FREEIF(me->ti.pwText);
      me->cbBuf = 0;
      me->ti.cchText = 0;

      // Delete any existing tags.
      Vector_DeleteAll(pvTags);
   }

   return nErr;
}


uint32 RichTextModel_TagsAdd(IRichTextModel* po, const Tag* pTag)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   int         nErr           = SUCCESS;
   int         nTagNameLength = 0;
   uint32      nIndex         = 0;
   OpaqueTag*  pOpaqueTag     = NULL;

   // Check for invalid parameters.
   if ((NULL == pTag)
      || (pTag->nStart < 0)
      || (pTag->nLength < 1)
      || ((pTag->nStart + pTag->nLength) > me->ti.cchText)
      || (pTag->nNameLength == 0)
      || (NULL == pTag->pwName)) {

      return EBADPARM;
   }

   if (SUCCESS == nErr) {
      nTagNameLength = pTag->nNameLength;
      if (nTagNameLength < 0) {
         nTagNameLength = WSTRLEN(pTag->pwName);

         // Check for invalid parameters.
         if (nTagNameLength == 0) {
            return EBADPARM;
         }
      }
   }

   if (SUCCESS == nErr) {
      // Get the index within the vector to insert the tag at.
      nErr = TagsGetIndex(&me->pRichTextModel->vTags, pTag, &nIndex);
   }

   if (SUCCESS == nErr) {
      // Create a new tag.
      nErr = NewTag(pTag->nStart, pTag->nLength, nTagNameLength, pTag->pwName, &pOpaqueTag);
   }

   if (SUCCESS == nErr) {
      // Insert the tag into the vector at the index.
      nErr = Vector_InsertAt(&me->pRichTextModel->vTags, nIndex, pOpaqueTag);
   }

   if (SUCCESS == nErr) {
      // Transfer ownership of the tag to the vector.
      pOpaqueTag = NULL;
   }

   if (SUCCESS != nErr) {
      FreeTag(pOpaqueTag);
   }

   return nErr;
}


int RichTextModel_TagsSize(IRichTextModel* po)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   return Vector_Size(&me->pRichTextModel->vTags);
}


uint32 RichTextModel_TagsGetAt(IRichTextModel* po, uint32 nIndex, Tag* pTag)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   int         nErr        = SUCCESS;
   OpaqueTag*  pOpaqueTag  = NULL;

   // Check for invalid parameters.
   if (NULL == pTag) {
      return EBADPARM;
   }

   (void)MEMSET(pTag, 0, sizeof(*pTag));

   nErr = Vector_GetAt(&me->pRichTextModel->vTags, nIndex, (void**)&pOpaqueTag);
   if (SUCCESS == nErr) {
      pTag->nStart       = pOpaqueTag->nStart;
      pTag->nLength      = pOpaqueTag->nLength;
      pTag->nNameLength  = pOpaqueTag->nNameLength;
      pTag->pwName       = pOpaqueTag->pwName;
   }

   return nErr;
}


uint32 RichTextModel_TagsDeleteAt(IRichTextModel* po, uint32 nIndex)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   return Vector_DeleteAt(&me->pRichTextModel->vTags, nIndex);
}


void RichTextModel_TagsDeleteAll(IRichTextModel* po)
{
   TEXTMODEL_FROM_IRICHTEXTMODEL;

   Vector_DeleteAll(&me->pRichTextModel->vTags);
}


static void ReplaceSelUpdateTags(
   Vector*        pvTags,
   int            nSelStart,
   int            nSelLength,
   int            nNewTextLength)
{
   int   nErr        = SUCCESS;
   int   nTagIndex   = 0;
   int   nTagCount   = Vector_Size(pvTags);

   // Iterate over the tags.
   nTagIndex = 0;
   while (nTagIndex < nTagCount) {
      OpaqueTag* pOpaqueTag = NULL;

      // Get the tag.
      nErr = Vector_GetAt(pvTags, nTagIndex, (void**)&pOpaqueTag);
      if (SUCCESS != nErr) {
         // This is an internal error.
         Vector_DeleteAll(pvTags);
         return;
      }

      ReplaceSelUpdateTag(pOpaqueTag, nSelStart, nSelLength, nNewTextLength);

      if (pOpaqueTag->nLength <= 0) {
         nErr = Vector_DeleteAt(pvTags, nTagIndex);
         if (SUCCESS != nErr) {
            // This is an internal error.
            Vector_DeleteAll(pvTags);
            return;
         }
         --nTagCount;
      }
      else {
         ++nTagIndex;
      }
   }
}


static void ReplaceSelUpdateTag(
   OpaqueTag*     pOpaqueTag,
   int            nSelStart,
   int            nSelLength,
   int            nNewTextLength)
{
   int   nTagStart   = pOpaqueTag->nStart;
   int   nTagLength  = pOpaqueTag->nLength;
   int   nTagEnd     = nTagStart + nTagLength;
   int   nSelEnd     = nSelStart + nSelLength;

   if (nSelLength == 0) {
      if (nSelStart == 0) {
         if (nTagStart == 0) {
            nTagLength += nNewTextLength;
         }
         else {
            nTagStart += nNewTextLength;
         }
      }
      else if (nTagEnd < nSelStart) {
      }
      else if (nTagStart < nSelStart) {
         nTagLength += nNewTextLength;
      }
      else {
         nTagStart += nNewTextLength;
      }
   }
   else {
      if (nTagEnd <= nSelStart) {
      }
      else if (nTagStart <= nSelStart) {
         nTagLength += nNewTextLength - GetIntersectionLength(pOpaqueTag, nSelStart, nSelLength);
      }
      else if (nTagEnd <= nSelEnd) {
         nTagStart = nSelStart + nNewTextLength;
         nTagLength = 0;
      }
      else if (nTagStart < nSelEnd) {
         nTagStart = nSelStart + nNewTextLength;
         nTagLength -= GetIntersectionLength(pOpaqueTag, nSelStart, nSelLength);
      }
      else {
         nTagStart += nNewTextLength - nSelLength;
      }
   }

   pOpaqueTag->nStart   = nTagStart;
   pOpaqueTag->nLength  = nTagLength;
}


static int GetIntersectionLength(
   OpaqueTag*     pOpaqueTag,
   int            nSelStart,
   int            nSelLength)
{
   int   nTagEnd              = pOpaqueTag->nStart + pOpaqueTag->nLength;
   int   nSelEnd              = nSelStart + nSelLength;
   int   nIntersectionStart   = MAX(pOpaqueTag->nStart, nSelStart);
   int   nIntersectionEnd     = MIN(nTagEnd, nSelEnd);
   int   nIntersectionLength  = MAX(0, (nIntersectionEnd - nIntersectionStart));

   return nIntersectionLength;
}


static uint32 NewTag(
   int            nStart,
   int            nLength,
   int            nNameLength,
   const AECHAR*  pwName,
   OpaqueTag**    ppOpaqueTag)
{
   int         nErr        = SUCCESS;
   OpaqueTag*  pOpaqueTag  = NULL;

   // Check for invalid parameters.
   if ((nNameLength < 1) || (NULL == pwName) || (NULL == ppOpaqueTag)) {
      return EBADPARM;
   }

   // Initialize the tag pointer.
   *ppOpaqueTag = NULL;

   pOpaqueTag = MALLOCREC_EX(OpaqueTag, ((nNameLength + 1) * sizeof(AECHAR)));
   if (NULL == pOpaqueTag) {
      nErr = ENOMEMORY;
   }
   else {
      pOpaqueTag->nStart      = nStart;
      pOpaqueTag->nLength     = nLength;
      pOpaqueTag->nNameLength = nNameLength;
      pOpaqueTag->pwName      = (AECHAR*)(pOpaqueTag + 1);

      (void)WSTRLCPY(pOpaqueTag->pwName, pwName, nNameLength + 1);
   }

   // Return the tag pointer.
   *ppOpaqueTag = pOpaqueTag;

   return nErr;
}


static void FreeTag(
   void*          pItem)
{
   OpaqueTag* pOpaqueTag = (OpaqueTag*)pItem;

   FREEIF(pOpaqueTag);
}


static void GetMarkupTextFromText(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax,
   const AECHAR*  pwText,
   int*           pnTextPos,
   int            nTextLength)
{
   int   nMTextPos   = *pnMTextPos;
   int   nTextPos    = *pnTextPos;

   // Put the text into the markup text buffer.
   while (nTextPos < nTextLength) {
      if (pwText[nTextPos] == '[') {
         if (nMTextPos < nMTextLengthMax) {
            pwMText[nMTextPos] = '[';
         }
         ++nMTextPos;
         if (nMTextPos < nMTextLengthMax) {
            pwMText[nMTextPos] = '[';
         }
         ++nMTextPos;
         ++nTextPos;
      }
      else {
         if (nMTextPos < nMTextLengthMax) {
            pwMText[nMTextPos] = pwText[nTextPos];
         }
         ++nMTextPos;
         ++nTextPos;
      }
   }

   *pnMTextPos = nMTextPos;
   *pnTextPos  = nTextPos;
}


static void GetMarkupTextFromStartTag(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax,
   const AECHAR*  pwTagName,
   int            nTagNameLength)
{
   int   nMTextPos   = *pnMTextPos;
   int   nTagNamePos = 0;

   // Put the start tag into the markup text buffer.
   if (nMTextPos < nMTextLengthMax) {
      pwMText[nMTextPos] = '[';
   }
   ++nMTextPos;
   for (nTagNamePos = 0; nTagNamePos < nTagNameLength; ++nTagNamePos) {
      if (nMTextPos < nMTextLengthMax) {
         pwMText[nMTextPos] = pwTagName[nTagNamePos];
      }
      ++nMTextPos;
   }
   if (nMTextPos < nMTextLengthMax) {
      pwMText[nMTextPos] = ']';
   }
   ++nMTextPos;

   *pnMTextPos = nMTextPos;
}


static void GetMarkupTextFromEndTag(
   AECHAR*        pwMText,
   int*           pnMTextPos,
   int            nMTextLengthMax)
{
   int nMTextPos = *pnMTextPos;

   // Put the end tag into the markup text buffer.
   if (nMTextPos < nMTextLengthMax) {
      pwMText[nMTextPos] = '[';
   }
   ++nMTextPos;
   if (nMTextPos < nMTextLengthMax) {
      pwMText[nMTextPos] = ']';
   }
   ++nMTextPos;

   *pnMTextPos = nMTextPos;
}


static uint32 GetToken(
   const AECHAR*  pwMText,
   int            nMTextLength,
   int            nMTextPos,
   TokenType*     pttTokenType,
   int*           pnTokenStart,
   int*           pnTokenLength)
{
   TokenType   ttTokenType       = TOKEN_TYPE_UNKNOWN;
   int         nTokenStart       = nMTextPos;
   int         nTokenLength      = 0;
   int         nTokenLengthMax   = 0;

   // Check for invalid parameters.
   if ((NULL == pwMText)
      || (nMTextLength < 0)
      || (nMTextPos < 0)
      || (nMTextPos > nMTextLength)
      || (NULL == pttTokenType)
      || (NULL == pnTokenStart)
      || (NULL == pnTokenLength)) {

      return EBADPARM;
   }

   // Initialize the token information.
   *pttTokenType  = TOKEN_TYPE_UNKNOWN;
   *pnTokenStart  = 0;
   *pnTokenLength = 0;

   nTokenLengthMax = nMTextLength - nMTextPos;
   if (nTokenLengthMax <= 0) {
      ttTokenType    = TOKEN_TYPE_END;
      nTokenLength   = 0;
   }
   else if (pwMText[nMTextPos] != '[') {
      while ((nMTextPos < nMTextLength) && (pwMText[nMTextPos] != '[')) {
         ++nMTextPos;
      }

      ttTokenType    = TOKEN_TYPE_TEXT;
      nTokenLength   = nMTextPos - nTokenStart;
   }
   else if ((nTokenLengthMax >= 2) &&
      (pwMText[nMTextPos + 1] == '[')) {

      ttTokenType    = TOKEN_TYPE_LEFT_BRACKET;
      nTokenLength   = 2;
   }
   else {
      boolean bFound = FALSE;

      ++nMTextPos;
      while (!bFound && (nMTextPos < nMTextLength)) {
         bFound = (pwMText[nMTextPos] == ']');
         ++nMTextPos;
      }

      if (bFound) {
         ttTokenType    = TOKEN_TYPE_TAG;
         nTokenLength   = nMTextPos - nTokenStart;
      }
      else {
         return EFAILED;
      }
   }

   // Return the token information.
   *pttTokenType = ttTokenType;
   *pnTokenStart = nTokenStart;
   *pnTokenLength = nTokenLength;

   return SUCCESS;
}


static uint32 TagsGetIndex(
   const Vector*  pvTags,
   const Tag*     pTag,
   uint32*        pnIndex)
{
   int      nErr     = SUCCESS;
   boolean  bFound   = FALSE;
   uint32   nIndex   = 0;
   uint32   nCount   = 0;

   // Check for invalid parameters.
   if ((NULL == pvTags) || (NULL == pTag) || (NULL == pnIndex)) {
      return EBADPARM;
   }

   // Initialize the index.
   *pnIndex = 0;

   // Iterate over the tags.
   bFound = FALSE;
   nIndex = 0;
   nCount = Vector_Size(pvTags);
   while (!bFound && (nIndex < nCount)) {
      const OpaqueTag* pOpaqueTag = NULL;

      // Get the tag.
      nErr = Vector_GetAt(pvTags, nIndex, (void**)&pOpaqueTag);
      if (SUCCESS != nErr) {
         // This is an internal error.
         return nErr;
      }

      if (pOpaqueTag->nStart > pTag->nStart) {
         bFound = TRUE;
      }
      else {
         ++nIndex;
      }
   }

   // Return the index.
   *pnIndex = nIndex;

   return nErr;
}
