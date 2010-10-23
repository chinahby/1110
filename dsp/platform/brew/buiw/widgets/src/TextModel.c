/*========================================================================

FILE:  TextModel.c

SERVICES:  

GENERAL DESCRIPTION:
  Implementation of an ITextModel 

========================================================================
========================================================================
 
            Copyright © 1999-2006 QUALCOMM Incorporated 
                  All Rights Reserved.
                QUALCOMM Proprietary/GTDR
 
========================================================================
========================================================================*/
#include "AEE.h"
#include "AEESource.h"
#include "TextModel.h"
#include "wutil.h"


/////////////////////////////////////////////////////////////////////////////
// TextModel


int TextModel_New(ITextModel **ppo, IModule *piModule)
{
   TextModel *me = MALLOCREC_VTBL(TextModel, ITextModel);
   
   *ppo = (ITextModel*)me;

   if (!me) {
      return ENOMEMORY;
   }

   TextModel_Ctor(me, GETVTBL(me,ITextModel), piModule);

   // initialize the model buffer
   return MemGrow((void**)&me->ti.pwText, &me->cbBuf, 0, 48);
}


void TextModel_Ctor(TextModel *me, AEEVTBL(ITextModel) *pvt, IModule *piModule)
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



uint32 TextModel_Release(ITextModel *po)
{
   TextModel *me = (TextModel*)po;

   if (me->base.nRefs == 1) {
      FREEIF(me->ti.pwText);
   }

   return ModelBase_Release((IModel*)me);
}



int TextModel_QueryInterface(ITextModel *po, AEECLSID id, void **ppo)
{
   TextModel *me = (TextModel*)po;

   if (SUCCESS == ModelBase_QueryInterface((IModel*)me, id, ppo)) {
      return SUCCESS;
   }

   if (id == AEEIID_TEXTMODEL) {
      *ppo = me;
      ITEXTMODEL_AddRef(po);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}


void TextModel_NotifyEvent(IModel *po, uint32 evCode, int pos)
{
   ModelEvent ev;
   ev.evCode   = evCode;
   ev.dwParam  = (uint32)pos;
   IMODEL_Notify(po, &ev);
}


int TextModel_ReplaceSel(ITextModel *po, const AECHAR *pwText, int cchText)
{
   TextModel *me = (TextModel*)po;
   int nErr = SUCCESS;

   int nSelStart, nSelEnd;
   int nSelMin, nSelMax;
   int nSelLen;
   int nGrow, nMove, cch;

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
   if (nErr == SUCCESS) {

      AECHAR *pw = (AECHAR*)me->ti.pwText;         // start of text

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
int TextModel_GetSelText(ITextModel *po, AECHAR *pwBuf, int cchBuf)
{
   TextModel *me = (TextModel*)po;

   int nSelMin = MIN(me->ti.nSelStart, me->ti.nSelEndCaret);
   int nSelMax = MAX(me->ti.nSelStart, me->ti.nSelEndCaret);
   int cchSel = nSelMax-nSelMin;

   if (cchBuf > 0) {
      int cchCopy = MIN(cchBuf, cchSel+1);
      WSTRLCPY(pwBuf, me->ti.pwText + nSelMin, cchCopy);
   }

   return cchSel;
}


void TextModel_SetSel(ITextModel *po, int nSelStart, int nSelEndCaret)
{
   TextModel *me = (TextModel*)po;
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


void TextModel_GetTextInfo(ITextModel *po, TextInfo *pTextInfo)
{
   TextModel *me = (TextModel*)po;
   *pTextInfo = me->ti;
}

