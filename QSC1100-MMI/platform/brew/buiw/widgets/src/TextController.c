/*======================================================
FILE: TextController.c

SERVICES: 
   IController implementation for text widget

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "WidgetBase.h"
#include "ModelBase.h"
#include "wutil.h"
#include "ListWidget.h"
#include "StaticWidget.h"
#include "ScrollWidget.h"
#include "TextController.h"
#include "AEETextWidget.h"
#include "AEEWProperties.h"
#include "AEEHFont.h"
#include "widget_res.h"


// default multi-tap timeout
#define  MTTIMEOUT   1000


#ifndef SPCCHAR
#undef SPCCHAR
#endif
#define SPCCHAR            ('\040')

#ifndef CHARISIN
#undef CHARISIN
#endif
#define CHARISIN(ch, cBtm, cTop)   \
   ( (unsigned char) ((ch) - (cBtm)) <= ((unsigned char) (cTop) - (cBtm)) )

#ifndef ISWHITESPACE
#undef ISWHITESPACE
#endif
#define ISWHITESPACE(ch)   (CHARISIN((ch), '\001', SPCCHAR))


#define DECLARE_ME          TextController *me = (TextController*)po


static const uint32 aModeSeq[] = {
   AEE_TMODE_NUMBERS,
   AEE_TMODE_MULTITAP_MC,
   AEE_TMODE_MULTITAP_UC,
   AEE_TMODE_MULTITAP_LC,
   AEE_TMODE_MULTITAP_SC
};


///////////////////////////////////////////////////////////////////


static uint32 GetNextMode(uint32 nMode)
{
   int i;
   for (i=0; i < ARRAYSIZE(aModeSeq); i++) {
      if (nMode == aModeSeq[i]) {
         return aModeSeq[(i+1) % ARRAYSIZE(aModeSeq)];
      }
   }
   return AEE_TMODE_NUMBERS;
}

static boolean FindMode(uint32 nMode)
{
   int i;
   for (i=0; i < ARRAYSIZE(aModeSeq); i++) {
      if (nMode == aModeSeq[i]) {
         return 1;
      }
   }
   return 0;
}


static AECHAR WToUpper(AECHAR c)
{
   if ((c >= (AECHAR)'a') && (c <= (AECHAR)'z')) {
      c = (AECHAR)'A' + (c - (AECHAR)'a');
   }
   return c;
}

static AECHAR WToLower(AECHAR c)
{
   if ((c >= (AECHAR)'A') && (c <= (AECHAR)'Z')) {
      c = (AECHAR)'a' + (c - (AECHAR)'A');
   }
   return c;
}

static void ITextModel_Select(ITextModel *me, int nFromCaret)
{
   TextInfo ti;
   ITEXTMODEL_GetTextInfo(me, &ti);
   ITEXTMODEL_SetSel(me, ti.nSelEndCaret+nFromCaret, ti.nSelEndCaret);
}

static int TextModel_LimitReplaceSel(ITextModel *po, const AECHAR *pwText, int cchText, int cchMax)
{
   if (cchMax > 0) {

      // a wrapper for ITEXTMODEL_ReplaceSel that will limit
      // the text in the model to a predefined number of chars
      TextInfo ti;
       int cchDel, cchPostDel;
   
      ITEXTMODEL_GetTextInfo(po, &ti);
   
      cchDel = ABS(ti.nSelStart - ti.nSelEndCaret);
      cchPostDel = ti.cchText - cchDel;
   
      if (cchPostDel + cchText > cchMax) {
         cchText = cchMax - cchPostDel;
      }
   
      if (cchText <= 0) {
         return EFAILED;
      }
   }

   return ITEXTMODEL_ReplaceSel(po, pwText, cchText);
}



///////////////////////////////////////////////////////////////////
// TextController

static __inline void IModel_NotifyEx(IModel *po, uint32 evCode, uint32 dwParam) {
   if (po) {
      ModelEvent ev;
      ev.evCode   = evCode;
      ev.dwParam  = dwParam;
      IMODEL_Notify(po, &ev);
   }
}

static void TextController_MultiTapTimeout(TextController *me)
{
   me->bForceCaps = 0;
   ITextModel_Select(me->piTextModel, 0);

   // send multi-tap timeout notification
   IModel_NotifyEx(me->piViewModel, EVT_MDL_MODE_TIMEOUT, 0);
}


static __inline const char *GetMultitapString(int nKeyIndex)
{
   static const char pszzMulti[] = 
      "0\0"
      ".,-'@:!?/1\0"
      "abc2\0"
      "def3\0"
      "ghi4\0"
      "jkl5\0"
      "mno6\0"
      "pqrs7\0"
      "tuv8\0"
      "wxyz9\0";

   int nLen;
   const char *psz = pszzMulti;

   while (0 < (nLen = STRLEN(psz)) && 0 < nKeyIndex--) {
      psz += nLen + 1;
   }

   return psz;
}

// clear multi-tap selection
static void TextController_ClearMTSel(TextController *me) {
   if (CALLBACK_IsQueued(&me->cbkTimer)) {
      CALLBACK_Cancel(&me->cbkTimer);
      ITextModel_Select(me->piTextModel, 0);
   }
}

static boolean TextController_HandleModeKeys(TextController *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   if (evt == EVT_KEY) {

      // handle mode change key
      if (wParam == me->avkMode && wParam != AVK_UNDEFINED) {
         me->nMode = GetNextMode(me->nMode);
         TextController_ClearMTSel(me);
         IModel_NotifyEx(me->piViewModel, EVT_MDL_MODE_CHANGE, (uint32)me->nMode);
         return TRUE;
      }

      // handle '#' key the same in both modes (insert space)
      if (wParam == AVK_POUND) {
         AECHAR wch = (AECHAR)' ';
         TextController_ClearMTSel(me);
         TextModel_LimitReplaceSel(me->piTextModel, &wch, 1, me->nMaxChars);
         return TRUE;
      }

      switch (me->nMode) {
         
         // latin multitap text entry
         case AEE_TMODE_MULTITAP_LC:
         case AEE_TMODE_MULTITAP_UC:
         case AEE_TMODE_MULTITAP_MC:
         case AEE_TMODE_MULTITAP_SC:
            
            if (wParam >= AVK_0 && wParam <= AVK_9) {

               AECHAR awch[2];
               int nKey;
               const char *pszMulti = NULL;
               AECHAR wch;

               nKey = (wParam - AEE_AVK_BASE - 1);
               pszMulti = GetMultitapString(nKey);

               if (!*pszMulti) {
                  return FALSE;
               }
               
               wch = (AECHAR) pszMulti[0];
               
               if (CALLBACK_IsQueued(&me->cbkTimer)) {

                  char *pszPos;

                  ITEXTMODEL_GetSelText(me->piTextModel, awch, ARRAYSIZE(awch));

                  awch[0] = WToLower(awch[0]);

                  // find char in pwsz string
                  pszPos = STRCHR(pszMulti, (char)awch[0]);

                  if (!pszPos) {
                     TextController_ClearMTSel(me);
                     me->bForceCaps = 0;
                     // notify that character was committed (same action as multi-tap timeout)
                     IModel_NotifyEx(me->piViewModel, EVT_MDL_MODE_TIMEOUT, 0);

                  } else if (pszPos[1]) {
                     wch = (AECHAR) pszPos[1];
                  }
               }

               // upper case
               if (me->bForceCaps || me->nMode == AEE_TMODE_MULTITAP_UC) {
                  wch = WToUpper(wch);
               
               // mixed case (upper case at start of word)
               } else if (me->nMode == AEE_TMODE_MULTITAP_MC) {

                  TextInfo ti;
                  int index;

                  ITEXTMODEL_GetTextInfo(me->piTextModel, &ti);
                  index = ti.nSelEndCaret - 1;
                  if (index < 0 || ti.pwText[index] == (AECHAR)' ') {
                     wch = WToUpper(wch);
                     me->bForceCaps = 1;  // force next keypress of same key to be uppercase also
                  }

               // sentence case (upper case at start of sentence)
               } else if (me->nMode == AEE_TMODE_MULTITAP_SC) {

                  TextInfo ti;
                  int index;

                  ITEXTMODEL_GetTextInfo(me->piTextModel, &ti);
                  index = ti.nSelEndCaret - 1;

                  if (index < 0 || wch == (AECHAR)'.') {

                     me->bEndSentence = 1;
                  
                  } else if (ti.pwText[index] != (AECHAR)' ' && ti.pwText[index] != '.') {

                     me->bEndSentence = 0;
                  }

                  if (me->bEndSentence) {

                     wch = WToUpper(wch);
                     me->bForceCaps = 1;
                  }
               }

               if (SUCCESS == TextModel_LimitReplaceSel(me->piTextModel, &wch, 1, me->nMaxChars)) {
                  int nTime = (me->dwFlags & TWF_PASSWORD) ? me->nMaskTimeout : MTTIMEOUT;
                  ITextModel_Select(me->piTextModel, -1);
                  CALLBACK_Timer(&me->cbkTimer, TextController_MultiTapTimeout, me, 
                                 me->piShell, nTime);
               }

               return TRUE;
            }
            break;
   

         // numeric text entry
         default: //case TMODE_NUMBERS:
            if (wParam >= AVK_0 && wParam <= AVK_9) {
               AECHAR wch = (AECHAR)('0' + (wParam - AEE_AVK_BASE - 1));
               TextModel_LimitReplaceSel(me->piTextModel, &wch, 1, me->nMaxChars);

               if (me->dwFlags & TWF_PASSWORD) {
                  CALLBACK_Timer(&me->cbkTimer, TextController_MultiTapTimeout, me,
                                 me->piShell, me->nMaskTimeout);
               }

               return TRUE;
            }
            break;
      }
   }

   return FALSE;
}


static void TextController_JumpWord(TextController *me, boolean bFwd)
{
   TextInfo info;
   int nCaret, nDir;
   const AECHAR *pw, *pwMin, *pwMax;

   ITEXTMODEL_GetTextInfo(me->piTextModel, &info);

   pwMin = info.pwText;
   pw    = pwMin + info.nSelEndCaret;
   pwMax = pwMin + info.cchText;

   nDir = bFwd ? 1 : -1;

   if (!bFwd && (pw > pwMin)) {
      pw--;
   }

   while (BETWEEN(pw, pwMin+1, pwMax) && ISWHITESPACE(*pw)) {
     pw += nDir;
   }
   while (BETWEEN(pw, pwMin+1, pwMax) && !ISWHITESPACE(*pw)) {
     pw += nDir;
   }
   while (BETWEEN(pw, pwMin+1, pwMax) && ISWHITESPACE(*pw)) {
     pw++;
   }

   nCaret = pw - pwMin;
   ITEXTMODEL_SetSel(me->piTextModel, nCaret, nCaret);
}


// key auto-repeat constants
#define KEYREPMIN       80    // ending repeat delay value
#define KEYREPMAX       400   // starting repeat delay value
#define KEYDELAY        500   // time key must be held to autorepeat
#define ACCELPCT        90    // acceleration- 90% means accelerate by 10% each cycle


static void TextController_KeyHeldTimerCB(TextController *me)
{
   if (me->wParamKey == AVK_LEFT || me->wParamKey == AVK_RIGHT) {
      TextController_JumpWord(me, (boolean)(me->wParamKey == AVK_RIGHT));
      ISHELL_SetTimer(me->piShell, me->nKeyRep, (PFNNOTIFY)TextController_KeyHeldTimerCB, (void*)me);
      me->nKeyRep = MAX(KEYREPMIN, me->nKeyRep * ACCELPCT / 100);

   } else if (me->wParamKey >= AVK_0 && me->wParamKey <= AVK_9) {
      AECHAR wch = (AECHAR)('0' + (me->wParamKey - AEE_AVK_BASE - 1));
      TextModel_LimitReplaceSel(me->piTextModel, &wch, 1, me->nMaxChars);
   }
}


static __inline boolean TextController_GetNextLine(TextController *me, boolean bNext, int *pnPos)
{
   int pos = *pnPos;
   IWIDGET_GetProperty(me->base.piw, (uint16)(bNext ? PROP_NEXTLINE : PROP_PREVLINE), (uint32*)pnPos);
   return pos != *pnPos;    // return success if there is a next or previous line
}


static boolean TextController_DefHandleEvent(IController *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   DECLARE_ME;

   if (!me->base.piw) {
      return FALSE;
   }

   if (TextController_HandleModeKeys(me, evt, wParam, dwParam)) {
      return TRUE;
   }
   
   switch(evt) {
   
      case EVT_CHAR:
         TextModel_LimitReplaceSel(me->piTextModel, &wParam, 1, me->nMaxChars);
         return TRUE;

      case EVT_UPDATECHAR:
         TextModel_LimitReplaceSel(me->piTextModel, &wParam, 1, me->nMaxChars);
         ITextModel_Select(me->piTextModel, -1);
         return TRUE;

      case EVT_KEY_PRESS:
         me->wParamKey = wParam;
         me->nKeyRep = KEYREPMAX;
         ISHELL_SetTimer(me->piShell, KEYDELAY, (PFNNOTIFY)TextController_KeyHeldTimerCB, (void *)me);
         return FALSE;        // don't eat these events

      case EVT_KEY_RELEASE: 
         if (me->wParamKey) {
            ISHELL_CancelTimer(me->piShell, (PFNNOTIFY)TextController_KeyHeldTimerCB, (void *)me);
            me->wParamKey = 0;
            return FALSE;     // don't eat these events
         } 
         break;

      case EVT_KEY: 
         if (me->piTextModel) {
            TextInfo info;
            int nCaret;

            switch (wParam) {
               case AVK_CLR:
                  TextController_ClearMTSel(me);
                  ITEXTMODEL_GetTextInfo(me->piTextModel, &info);
                  if (info.nSelStart == info.nSelEndCaret) {
                     int start = MAX(0, info.nSelStart-1);
                     ITEXTMODEL_SetSel(me->piTextModel, start, info.nSelEndCaret);
                  }
                  ITEXTMODEL_ReplaceSel(me->piTextModel, NULL, 0);
                  return TRUE;
      
               case AVK_LEFT:
               case AVK_RIGHT:
                  {
                     int nErr = SUCCESS;
                     TextController_ClearMTSel(me);
                     ITEXTMODEL_GetTextInfo(me->piTextModel, &info);

                     nErr = IWIDGET_GetCursorMovement(
                           me->base.piw, 
                           info.nSelEndCaret, 
                           ( wParam==AVK_LEFT) ? MOVEMENT_CHARACTER_LEFT : MOVEMENT_CHARACTER_RIGHT, 
                           &nCaret);

                     if ( SUCCESS == nErr ) { 
                        if ( info.nSelEndCaret != nCaret ) {
                           ITEXTMODEL_SetSel(me->piTextModel, nCaret, nCaret);
                           return TRUE;
                        }
                        else {
                           // don't return TRUE if we can't move
                           break;
                        }
                     }
                  }
                  break;
   
               case AVK_UP:
               case AVK_DOWN:
                  if (me->dwFlags & TWF_VERTICALNAV) {
                     int pos, posStart, posMax, col;
                     boolean bDown = (wParam == AVK_DOWN);
   
                     TextController_ClearMTSel(me);
                     ITEXTMODEL_GetTextInfo(me->piTextModel, &info);
                     posStart = info.nSelEndCaret;

                     // try using the CURSOR_MOVE property.
                     {

                        int nErr = SUCCESS;
                        TextController_ClearMTSel(me);
                        ITEXTMODEL_GetTextInfo(me->piTextModel, &info);

                        nErr = IWIDGET_GetCursorMovement(
                              me->base.piw, 
                              info.nSelEndCaret, 
                              ( wParam==AVK_UP) ? MOVEMENT_LINE_UP : MOVEMENT_LINE_DOWN, 
                              &nCaret);

                        if ( SUCCESS == nErr ) { 
                           if ( info.nSelEndCaret != nCaret ) {
                              ITEXTMODEL_SetSel(me->piTextModel, nCaret, nCaret);
                              return TRUE;
                           }
                           else {
                              // don't return TRUE if we can't move
                              break;
                           }
                        }
                     }

                     // that didn't work...revert to the old method.
                     IWIDGET_GetProperty(me->base.piw, PROP_STARTLINE, (uint32*)&posStart);
                     pos = posStart;
                     col = info.nSelEndCaret - posStart;
   
                     if (TextController_GetNextLine(me, bDown, &pos)) {
                        posMax = posStart-1;    // if moving up, clip pos to line start (-1)
                        if (bDown) {
                           // if moving down, clip pos to length of next line
                           posMax = pos;
                           if (TextController_GetNextLine(me, 1, &posMax)) {
                              posMax--;
                           } else {
                              // no next line, clip to length of text
                              posMax = info.cchText;
                           }
                        }
                        // add column offset and check bounds
                        pos = MAX(0, MIN(pos+col, posMax));
                        
                        // move caret to next/prev line
                        ITEXTMODEL_SetSel(me->piTextModel, pos, pos);
                        return TRUE;
                     }
                     // don't return TRUE if we can't move
                  }
                  break;
            }
         }
         break;
   
      case EVT_WDG_SETPROPERTY:
         if (wParam == PROP_LIMITTEXT) {
            me->nMaxChars = (int)dwParam;
            return TRUE;
         
         } else if (wParam == PROP_INPUTMODE) {
            uint32 mode = (uint32)dwParam;
            if (FindMode(mode) && (mode != me->nMode)) {
               me->nMode = mode;
               TextController_ClearMTSel(me);
               IModel_NotifyEx(me->piViewModel, EVT_MDL_MODE_CHANGE, (uint32)me->nMode);
            }
            return TRUE;
         
         } else if (wParam == PROP_TEXTCONTROLLERMODEKEY) {
            me->avkMode = (AVKType) dwParam;
            return TRUE;

         } else if (wParam == PROP_FLAGS) {
            // keep a copy of the flags, but allow widget 
            // to handle this property also
            me->dwFlags = dwParam;

         } else if (wParam == PROP_PASSWORDMASKDELAY) {
            // allow widget to get at this one too
            me->nMaskTimeout = (int)dwParam;
         }

         break;   


      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_LIMITTEXT) {
            *(int*)dwParam = me->nMaxChars;
            return TRUE;
         
         } else if (wParam == PROP_INPUTMODE) {
            *(int*)dwParam = (uint32)me->nMode;
            return TRUE;

         } else if (wParam == PROP_TEXTCONTROLLERMODEKEY) {
            *(AVKType*)dwParam = me->avkMode;
            return TRUE;
         }

         break;
   }

   return ControllerBase_DefHandleEvent(po, evt, wParam ,dwParam);
}


static void TextController_OnModelChanged(TextController *me, ModelEvent *pev) 
{
   if (pev->evCode == EVT_MDL_SETMODEL) {
      RELEASEIF(me->piTextModel);
      IWIDGET_GetModel(me->base.piw, AEEIID_TEXTMODEL, (IModel**)&me->piTextModel);
   }
}


int TextController_SetWidget(IController *po, IWidget *piw)
{
   DECLARE_ME;
   ITextModel *piTextModel = 0;

   if (piw && 
       (SUCCESS == IWIDGET_GetModel(piw, AEEIID_TEXTMODEL, (IModel**)&piTextModel))) {

      RELEASEIF(me->piTextModel);
      me->piTextModel = piTextModel;

      ControllerBase_SetWidget(po, piw);

      RELEASEIF(me->piViewModel);
      IWIDGET_GetViewModel(piw, &me->piViewModel);

      // listen on view model for notification of data model change
      if (me->piViewModel) {
         IMODEL_AddListenerEx(me->piViewModel, &me->mlModelChange, 
                              (PFNLISTENER)TextController_OnModelChanged, me);
      }
      return SUCCESS;
   }

   return EFAILED;
}


uint32 TextController_Release(IController *po)
{
   DECLARE_ME;
   
   if (me->base.nRefs == 1) {
      TextController_Dtor(me);
   }
   return ControllerBase_Release(po);
}


void TextController_Dtor(TextController *me)
{
   LISTENER_Cancel(&me->mlModelChange);
   CALLBACK_Cancel(&me->cbkTimer);
   if (me->wParamKey) {
      ISHELL_CancelTimer(me->piShell, (PFNNOTIFY)TextController_KeyHeldTimerCB, (void *)me);
   }
   RELEASEIF(me->piShell);
   RELEASEIF(me->piTextModel);
   RELEASEIF(me->piViewModel);
}


void TextController_Ctor(TextController *me, AEEVTBL(IController) *pvt, IShell *piShell, 
                         IModule *piModule, PFNHANDLER pfnDefHandler)
{
   ControllerBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(TextController_DefHandleEvent));

   pvt->Release         = TextController_Release;
   pvt->SetWidget       = TextController_SetWidget;

   me->piShell = piShell;
   ISHELL_AddRef(me->piShell);

   me->nMode   = AEE_TMODE_MULTITAP_LC;
   me->avkMode = AVK_STAR;

   me->nMaskTimeout = MTTIMEOUT;
}


int TextController_New(IController **ppo, IShell *piShell, IModule *piModule)
{
   TextController *me = MALLOCREC_VTBL(TextController, IController);
   if (!me) {
      return ENOMEMORY;
   }
   TextController_Ctor(me, GETVTBL(me,IController), piShell, piModule, NULL);
   *ppo = (IController*) me;
   return SUCCESS;
}





