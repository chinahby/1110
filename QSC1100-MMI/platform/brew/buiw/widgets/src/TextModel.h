/*========================================================================

FILE:  TextModel.h

SERVICES:  

GENERAL DESCRIPTION:
  Implementation of an ITextModel 

========================================================================
========================================================================
 
            Copyright © 1999-2007 QUALCOMM Incorporated 
                  All Rights Reserved.
                QUALCOMM Proprietary/GTDR
 
========================================================================
========================================================================*/
#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include "AEETextModel.h"
#include "AEERichTextModel.h"
#include "AEEBase.h"
#include "ModelBase.h"


/////////////////////////////////////////////////////////////////////////////
// TextModel implementation

typedef struct TextModel     TextModel;
typedef struct RichTextModel RichTextModel;

struct IRichTextModel {
   AEEBASE_INHERIT(IRichTextModel, TextModel);
};

struct RichTextModel {
   IRichTextModel          richTextModel;
   AEEVTBL(IRichTextModel) vtRichTextModel;

   Vector                  vTags;
};

typedef struct {
   AECHAR*  pwText;        // text in model
   int      cchText;       // length of pwText
   int      nSelStart;     // start of selection
   int      nSelEndCaret;  // end of selection and caret position
} OpaqueTextInfo;

struct TextModel {
   ModelBase      base;

   OpaqueTextInfo ti;
   int            cbBuf;

   RichTextModel* pRichTextModel;
};


int      TextModel_New                 (ITextModel** ppo, IModule* piModule);
void     TextModel_Ctor                (TextModel* me, AEEVTBL(ITextModel)* pvt, IModule* piModule);
uint32   TextModel_Release             (ITextModel* po);
int      TextModel_QueryInterface      (ITextModel* po, AEECLSID id, void** ppo);
int      TextModel_ReplaceSel          (ITextModel* p, const AECHAR* pwText, int cchText);
int      TextModel_GetSelText          (ITextModel* p, AECHAR* pwBuf, int cchBuf);
void     TextModel_SetSel              (ITextModel* p, int nSelStart, int nSelEndCaret);
void     TextModel_GetTextInfo         (ITextModel* po, TextInfo* pTextInfo);

int      RichTextModel_AddListener     (IRichTextModel *p, ModelListener *pListener);
void     RichTextModel_Notify          (IRichTextModel *p, ModelEvent *pEvent);
uint32   RichTextModel_GetMarkupText   (IRichTextModel* po, AECHAR* pwMText, int* pnSize);
uint32   RichTextModel_SetMarkupText   (IRichTextModel* po, const AECHAR* pwMText, int nMTextLength);
uint32   RichTextModel_TagsAdd         (IRichTextModel* po, const Tag* pTag);
int      RichTextModel_TagsSize        (IRichTextModel* po);
uint32   RichTextModel_TagsGetAt       (IRichTextModel* po, uint32 nIndex, Tag* pTag);
uint32   RichTextModel_TagsDeleteAt    (IRichTextModel* po, uint32 nIndex);
void     RichTextModel_TagsDeleteAll   (IRichTextModel* po);

#endif   //TEXTMODEL_H
