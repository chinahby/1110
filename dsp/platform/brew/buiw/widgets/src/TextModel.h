/*========================================================================

FILE:  TextModel.h

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
#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include "AEETextModel.h"
#include "ModelBase.h"


/////////////////////////////////////////////////////////////////////////////
// TextModel implementation

typedef struct TextModel TextModel;

struct TextModel {
   ModelBase            base;
   TextInfo             ti;
   int                  cbBuf;
};


int    TextModel_New             (ITextModel **ppo, IModule *piModule);
void   TextModel_Ctor            (TextModel *me, AEEVTBL(ITextModel) *pvt, IModule *piModule);
uint32 TextModel_Release         (ITextModel *po);
int    TextModel_QueryInterface  (ITextModel *po, AEECLSID id, void **ppo);
int    TextModel_ReplaceSel      (ITextModel *p, const AECHAR *pwText, int cchText);
int    TextModel_GetSelText      (ITextModel *p, AECHAR *pwBuf, int cchBuf);
void   TextModel_SetSel          (ITextModel *p, int nSelStart, int nSelEndCaret);
void   TextModel_GetTextInfo     (ITextModel *po, TextInfo *pTextInfo);



#endif   //TEXTMODEL_H


