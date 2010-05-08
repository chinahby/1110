/*
  ========================================================================

  FILE:       FormControl.c

  SERVICES:   

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEHtmlWidget.h"
#include "AEECheckWidget.h"
#include "AEEStaticWidget.h"
#include "AEEImageWidget.h"
#include "SourceAStream.h"
#include "DoxPos.h"
#include "wutil.h"
#include "htmlwidget_res.h"
#include "FormControl.h"


/////////////////////////////////////////////////////////////////////////////
// const strings for elements, attributes and attribute names

//static const AECHAR TEXTAREA[]   = { 'T','E','X','T','A','R','E','A', 0 };
static const AECHAR INPUT[]      = { 'I','N','P','U','T', 0 };
static const AECHAR TYPE[]       = { 'T','Y','P','E', 0 };
static const AECHAR VALUE[]      = { 'V','A','L','U','E', 0 };
static const AECHAR MAXLENGTH[]  = { 'M','A','X','L','E','N','G','T','H', 0 };
static const AECHAR SIZE[]       = { 'S','I','Z','E', 0 };
static const AECHAR CHECKBOX[]   = { 'C','H','E','C','K','B','O','X', 0 };
static const AECHAR RADIO[]      = { 'R','A','D','I','O', 0 };
static const AECHAR SUBMIT[]     = { 'S','U','B','M','I','T', 0 };
static const AECHAR RESET[]      = { 'R','E','S','E','T', 0 };
static const AECHAR IMAGE[]      = { 'I','M','A','G','E', 0 };
static const AECHAR HIDDEN[]     = { 'H','I','D','D','E','N', 0 };
static const AECHAR SELECT[]     = { 'S','E','L','E','C','T', 0 };
static const AECHAR IMG[]        = { 'I','M','G', 0 };
static const AECHAR CHECKED[]    = { 'C','H','E','C','K','E','D', 0 };
static const AECHAR ALT[]        = { 'A','L','T', 0 };
static const AECHAR WIDTH[]      = { 'W','I','D','T','H', 0 };
static const AECHAR HEIGHT[]     = { 'H','E','I','G','H','T', 0 };
static const AECHAR SRC[]        = { 'S','R','C', 0 };


typedef void (*PFNDTOR) (void *p);


__inline
boolean STREQI(const char *s1, const char *s2)
{
   return (s1 && s2 && STRICMP(s1,s2) == 0);
}

__inline
int ERR_WSTRDUP(const AECHAR *pwsz, AECHAR **ppwszOut) {
   *ppwszOut = WSTRDUP(pwsz);
   return (*ppwszOut ? SUCCESS : ENOMEMORY);
}


static int IShell_GetDestBitmapSize(IShell *piShell, int *pcx, int *pcy)
{
   int nErr;
   IBitmap *pbm = 0;
   IDisplay *piDisplay = 0;
   
   AEEBitmapInfo bi;
   ZEROAT(&bi);

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void**)&piDisplay);

   if (!nErr) {
      pbm = IDISPLAY_GetDestination(piDisplay);
   }

   if (!nErr && pbm) {
      nErr = IBITMAP_GetInfo(pbm, &bi, sizeof(bi));      
   }

   *pcx = bi.cx;
   *pcy = bi.cy;

   RELEASEIF(pbm);
   RELEASEIF(piDisplay);

   return nErr;
}


static int StrAppend(char **pp, int *pcb, const char *pAppend, int cbAppendLen)
{
   int nUsed = 0;
   int nErr;

   if (*pp) {
      nUsed = STRLEN(*pp);
   }
   nErr = MemAppendEx((void**)pp, pcb, &nUsed, (void*)pAppend, cbAppendLen, 1);
   if (!nErr) {
      char chz = 0;
      nErr = MemAppend((void**)pp, pcb, &nUsed, &chz, 1);
   }
   return nErr;
}


static int StrReplace(char **pp, int *pcb, const char *pReplace, int cbReplaceLen)
{
   if (*pp && (*pcb > 0)) {
      (*pp)[0] = 0;
   }
   return StrAppend(pp, pcb, pReplace, cbReplaceLen);
}


static int IDoxUtil_LoadImageAndGetSize(IDoxUtil *me, uint16 id, IImage **ppii, 
                                        int *pcx, int *pcy)
{
   int nErr = IDOXUTIL_GetImage(me, id, ppii);
   if (!nErr) {
      AEEImageInfo aii;
      IIMAGE_GetInfo(*ppii, &aii);
      if (pcx) *pcx = aii.cx;
      if (pcy) *pcy = aii.cy;
   }
   return nErr;
}

static int IDoxUtil_EncodeAlloc(IDoxUtil *me, const AECHAR *pwszIn, char **ppszOut)
{
   int cb;
   int nUsed;
   int nErr = 0;

   *ppszOut = 0;

   // allocate mem for single-byte relative URL string
   cb = IDOXUTIL_Encode(me, NULL, 0, pwszIn, WSTRLEN(pwszIn), &nUsed, 1);
   if (!cb) {
      nErr = EFAILED;
   }
   if (!nErr) {
      *ppszOut = MALLOC(cb);
      if (!*ppszOut) {
         nErr = ENOMEMORY;
      }
   }
   if (!nErr) {
      IDOXUTIL_Encode(me, *ppszOut, cb, pwszIn, WSTRLEN(pwszIn), &nUsed, 1);
   }

   return nErr;
}


static int IDoxUtil_MakeUrlAlloc(IDoxUtil *me, const AECHAR *pcwszRel, char **ppszUrlOut)
{
   char *pcRel = 0;
   int nErr;
   int cb = 0;
   
   nErr = IDoxUtil_EncodeAlloc(me, pcwszRel, &pcRel);

   if (!nErr) {
      cb = IDOXUTIL_MakeUrl(me, pcRel, NULL, 0);
      *ppszUrlOut = MALLOC(cb);
      if (!*ppszUrlOut) {
         nErr = ENOMEMORY;
      }
   }

   // construct full URL
   if (!nErr) {
      IDOXUTIL_MakeUrl(me, pcRel, *ppszUrlOut, cb);
   }

   FREEIF(pcRel);
   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// FormControl (Base)


struct Fct {
   IWidget *      piw;
   HandlerDesc    hd;
   IShell *       piShell;
   IDoxPos *      piDoxPos;
   PFNDTOR        pfnDtor;
   PFNFCTNOTIFY   pfn;
   void *         pv;
   uint32         dwExtra;
   flg            fBaseline : 1;       // flag: PROP_BASELINE is supported
};


static void Fct_Dtor(Fct *me)
{
   if (me->pfnDtor) {
      me->pfnDtor(me);
   }
   RELEASEIF(me->piShell);
   RELEASEIF(me->piDoxPos);
   RELEASEIF(me->piw);

}

static void Fct_Delete(Fct *me)
{
   Fct_Dtor(me);
   FREE(me);
}

static void Fct_Notify(Fct *me, int nCode)
{
   if (me->pfn) {
      me->pfn(me->pv, me->piw, nCode);
   }
}

static boolean Fct_HandleEvent(Fct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {

      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_BASELINE && !me->fBaseline) {
            IFont *piFont = 0;
            WExtent we;
            int nPad = 0;
            int nWidth = 0;
            int nDescent = 0;
            IWIDGET_GetExtent(me->piw, &we);
            IWIDGET_GetBottomPadding(me->piw, &nPad);
            IWIDGET_GetActiveBorderWidth(me->piw, &nWidth);
            if (SUCCESS == IWIDGET_GetFont(me->piw, &piFont)) {
               AEEFontInfo fi;
               IFONT_GetInfo(piFont, &fi, sizeof(fi));
               nDescent = fi.nDescent;
               IFONT_Release(piFont);
            }
            *((int*)dwParam) = we.height - (nPad + nWidth + nDescent);
            return TRUE;
         }
         break;

      case EVT_FCT_GETEXTRA:
         *((uint32*)dwParam) = me->dwExtra;
         return TRUE;
      
      case EVT_FCT_SETEXTRA:
         me->dwExtra = dwParam;
         return TRUE;
   }

   return HANDLERDESC_Call(&me->hd, evt, wParam, dwParam);
}


static int Fct_New(Fct **ppo, int nSize, IShell *piShell, IDoxPos *piDoxPos, AEECLSID clsidW, 
                   PFNHANDLER pfnHandler, PFNDTOR pfnDtor, PFNFCTNOTIFY pfn, void *pv)
{
   int nErr;
   Fct *me = 0;

   *ppo = 0;

   if (nSize < sizeof(Fct)) {
      nSize = sizeof(Fct);
   }

   me = MALLOCREC_EX(Fct, (nSize - sizeof(Fct)));
   if (!me) {
      return ENOMEMORY;
   }

   nErr = ISHELL_CreateInstance(piShell, clsidW, (void**)&me->piw);
   if (!nErr) {

      me->pfn  = pfn;
      me->pv   = pv;
      
      me->pfnDtor = pfnDtor;

      me->piShell = piShell;
      ISHELL_AddRef(me->piShell);
      
      me->piDoxPos = piDoxPos;
      IDOXPOS_AddRef(me->piDoxPos);

      // check if PROP_BASELINE is supported
      // (must do this before SetHandler!)
      {
         int nTemp;
         me->fBaseline = (SUCCESS == IWIDGET_GetProperty(me->piw, PROP_BASELINE, (uint32*)&nTemp));
      }

      HANDLERDESC_Init(&me->hd, pfnHandler, me, 0);
      IWIDGET_SetHandler(me->piw, &me->hd);

      *ppo = me;

   } else {
      Fct_Delete(me);
   }

   return nErr;
}



/////////////////////////////////////////////////////////////////////////////
// CheckFormControl


static void CheckFct_Reset(Fct *me)
{
   IValueModel *pivm = 0;
   AECHAR awchTemp[10];
   
   boolean bChecked = 
      (0 != IDOXPOS_GetAttrValue(me->piDoxPos, CHECKED, awchTemp, ARRAYSIZE(awchTemp)));

   if (SUCCESS == IWIDGET_GetModel(me->piw, AEEIID_VALUEMODEL, (IModel**)&pivm)) {
      IVALUEMODEL_SetBool(pivm, bChecked);
      IVALUEMODEL_Release(pivm);
   }
}


static boolean CheckFct_HandleEvent(Fct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_BASELINE && !me->fBaseline) {
            WExtent we;
            IWIDGET_GetExtent(me->piw, &we);
            *((int*)dwParam) = we.height - 2;
            return TRUE;
         }
         break;

      case EVT_FCT_RESET:
         CheckFct_Reset(me);
         return TRUE;
      
      case EVT_FCT_STOP:
         return TRUE;
      
      case EVT_FCT_GETDATA:
         {
            int nCount = 0;
            int nDest = wParam;
            int nDestUsed = 0;
            char *pszDest = NULL;
            AECHAR *pawchDest = NULL;
            IDoxUtil* pitDoxUtil;

            if ( 0 == nDest ) {
               // counting mode
               nCount = IDOXPOS_GetAttrValue( me->piDoxPos, VALUE, NULL, 0 );
               *(int *) dwParam = nCount;
            } else {
               // data mode
               pawchDest = (AECHAR *) MALLOC( nDest * sizeof(AECHAR) );
               if ( NULL != pawchDest ) {
                  IDOXPOS_GetAttrValue( me->piDoxPos, VALUE, pawchDest, nDest );
		            pszDest = (char *)dwParam;
                  if (SUCCESS == IDOXPOS_QueryInterface( me->piDoxPos, AEEIID_DOXUTIL, (void**)&pitDoxUtil ) ) {
                     (void)IDOXUTIL_Encode(pitDoxUtil, pszDest, nDest, pawchDest, nDest, &nDestUsed, TRUE);
                     RELEASEIF(pitDoxUtil);
                  }
		  FREE(pawchDest);
               } else {
                  return FALSE;
               }
            }
            return TRUE;
         }      
      case EVT_FCT_SETDATA:
         return TRUE;
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)FCT_CHECKBOX;
         return TRUE;
   }

   return Fct_HandleEvent(me, evt, wParam, dwParam);
}


static int CheckFormControl_New(Fct **ppo, IShell *piShell, 
                                IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv)
{
   Fct *me = 0;
   int nErr = Fct_New(&me, 0, piShell, piDoxPos, AEECLSID_CHECKWIDGET, 
                      (PFNHANDLER)CheckFct_HandleEvent, 0, (PFNFCTNOTIFY)pfn, pv);
   if (!nErr) {
      IWIDGET_SetPreferredExtent(me->piw, NULL);
      CheckFct_Reset(me);
      *ppo = me;
   } else {
      *ppo = 0;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// RadioFormControl


static void RadioFct_Reset(Fct *me)
{
   IValueModel *pivm = 0;
   AECHAR awchTemp[10];
   
   boolean bChecked = 
      (0 != IDOXPOS_GetAttrValue(me->piDoxPos, CHECKED, awchTemp, ARRAYSIZE(awchTemp)));

   if (SUCCESS == IWIDGET_GetModel(me->piw, AEEIID_VALUEMODEL, (IModel**)&pivm)) {
      IVALUEMODEL_SetBool(pivm, bChecked);
      IVALUEMODEL_Release(pivm);
   }
}


static boolean RadioFct_HandleEvent(Fct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_WDG_GETPROPERTY:
         if (wParam == PROP_BASELINE && !me->fBaseline) {
            WExtent we;
            IWIDGET_GetExtent(me->piw, &we);
            *((int*)dwParam) = we.height - 2;
            return TRUE;
         }
         break;

      case EVT_FCT_RESET:
         RadioFct_Reset(me);
         return TRUE;
      
      case EVT_FCT_STOP:
         return TRUE;
      
      case EVT_FCT_GETDATA:
         {
            int nCount = 0;
            int nDest = wParam;
            int nDestUsed = 0;
            char *pszDest = NULL;
            AECHAR *pawchDest = NULL;
            IDoxUtil* pitDoxUtil;

            if ( 0 == nDest ) {
               // counting mode
               nCount = IDOXPOS_GetAttrValue( me->piDoxPos, VALUE, NULL, 0 );
               *(int *) dwParam = nCount;
            } else {
               // data mode
               pawchDest = (AECHAR *) MALLOC( nDest * sizeof(AECHAR) );
               if ( NULL != pawchDest ) {
                  IDOXPOS_GetAttrValue( me->piDoxPos, VALUE, pawchDest, nDest );
		            pszDest = (char *)dwParam;
                  if (SUCCESS == IDOXPOS_QueryInterface( me->piDoxPos, AEEIID_DOXUTIL, (void**)&pitDoxUtil ) ) {
                     (void)IDOXUTIL_Encode(pitDoxUtil, pszDest, nDest, pawchDest, nDest, &nDestUsed, TRUE);
                     RELEASEIF(pitDoxUtil);
                  }
		  FREE(pawchDest);
               } else {
                  return FALSE;
               }
            }
            return TRUE;
         }      
      
      case EVT_FCT_SETDATA:
         return TRUE;
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)FCT_RADIO;
         return TRUE;
      
      case EVT_KEY:
			switch (wParam) {
			   // SELECT
			   case AVK_SELECT:
               {
                  Fct_Notify((Fct*)me, FCN_RADIOSELECT);
                  return TRUE;
               }
         }
         break;
   }

   return Fct_HandleEvent(me, evt, wParam, dwParam);
}


static int RadioFormControl_New(Fct **ppo, IShell *piShell, 
                                IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv)
{
   Fct *me = 0;
   int nErr = Fct_New(&me, 0, piShell, piDoxPos, AEECLSID_RADIOWIDGET, 
                      (PFNHANDLER)RadioFct_HandleEvent, 0, 
                      (PFNFCTNOTIFY)pfn, pv);
   if (!nErr) {
      IWIDGET_SetPreferredExtent(me->piw, NULL);
      RadioFct_Reset(me);
      *ppo = me;
   } else {
      *ppo = 0;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// ButtonFormControl (Submit, Reset, Button)

typedef struct ButtonFct {
   Fct      base;
   uint32   dwType;
} ButtonFct;


static boolean ButtonFct_HandleEvent(ButtonFct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_FCT_RESET:
         return TRUE;
      
      case EVT_FCT_STOP:
         return TRUE;
      
      case EVT_FCT_GETDATA:
         {
            int nCount = 0;
            int nDest = wParam;
            int nDestUsed = 0;
            char *pszDest = NULL;
            AECHAR *pawchDest = NULL;
            IDoxUtil* pitDoxUtil;

            if ( 0 == nDest ) {
               // counting mode
               nCount = IDOXPOS_GetAttrValue( me->base.piDoxPos, VALUE, NULL, 0 );
               *(int *) dwParam = nCount;
            } else {
               // data mode
               pawchDest = (AECHAR *) MALLOC( nDest * sizeof(AECHAR) );
               if ( NULL != pawchDest ) {
                  IDOXPOS_GetAttrValue( me->base.piDoxPos, VALUE, pawchDest, nDest );
		            pszDest = (char *)dwParam;
                  if (SUCCESS == IDOXPOS_QueryInterface( me->base.piDoxPos, AEEIID_DOXUTIL, (void**)&pitDoxUtil ) ) {
                     (void)IDOXUTIL_Encode(pitDoxUtil, pszDest, nDest, pawchDest, nDest, &nDestUsed, TRUE);
                     RELEASEIF(pitDoxUtil);
                  }
		  FREE(pawchDest);
               } else {
                  return FALSE;
               }
            }
            return TRUE;
         }      
         
      case EVT_FCT_SETDATA:
         return TRUE;
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)me->dwType;
         return TRUE;

      case EVT_KEY:
			switch (wParam) {
			   // SELECT
			   case AVK_SELECT:
               {
                  int nDest = 0;
                  AECHAR *pawchDest = NULL;
                  AECHAR awchSubmit[] = {'s','u','b','m','i','t',0};
                  AECHAR awchReset[] = {'r','e','s','e','t',0};

                  nDest = IDOXPOS_GetAttrValue( me->base.piDoxPos, VALUE, NULL, 0 );
                  pawchDest = (AECHAR *) MALLOC( nDest * sizeof(AECHAR) );
                  if ( NULL != pawchDest ) {
                     IDOXPOS_GetAttrValue( me->base.piDoxPos, TYPE, pawchDest, nDest );
                     // check if submit or reset
                     if (0 == WSTRCMP (awchSubmit, pawchDest)){
                        Fct_Notify((Fct*)me, FCN_SUBMIT);
                     } else if (0 == WSTRCMP (awchReset, pawchDest)){                     
                        Fct_Notify((Fct*)me, FCN_RESET);
                     }                  
                     FREEIF(pawchDest);
                     return TRUE;
                  }
               }
         }
         break;
   }

   return Fct_HandleEvent((Fct*)me, evt, wParam, dwParam);
}


static int ButtonFct_New(Fct **ppo, IShell *piShell, 
                         IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv,
                         uint32 dwType)
{
   ButtonFct *me = 0;
   AECHAR *pwValue = 0;

   int nErr = Fct_New((Fct**)&me, sizeof(ButtonFct), piShell, piDoxPos, AEECLSID_STATICWIDGET, 
                      (PFNHANDLER)ButtonFct_HandleEvent, 0, 
                      (PFNFCTNOTIFY)pfn, pv);
   if (!nErr) {

      AECHAR awchTemp[30];
      awchTemp[0] = 0;

      me->dwType = dwType;
      
      IDOXPOS_GetAttrValue(me->base.piDoxPos, VALUE, awchTemp, ARRAYSIZE(awchTemp));

      // set text of button
      if (0 == WSTRLEN(awchTemp)) {
         WSTRLCPY(awchTemp, ((dwType == FCT_SUBMIT) ? SUBMIT : RESET), ARRAYSIZE(awchTemp));
      }
      pwValue = WSTRDUP(awchTemp);
      if (!pwValue)  {
         nErr = ENOMEMORY;
      }
   }

   if (!nErr) {
      IWidget *piw = me->base.piw;

      IWIDGET_SetText(piw, pwValue, TRUE);

      // setup button border on static widget
      IWIDGET_SetBGColor(piw, MAKE_RGBA(208,208,208,255));
      IWIDGET_SetFGColor(piw, RGBA_BLACK);
      IWIDGET_SetBorderColor(piw, RGBA_BLACK);
      IWIDGET_SetActiveBorderWidth(piw, 2);
      IWIDGET_SetInactiveBorderWidth(piw, 1);
      IWIDGET_SetBorderStyle(piw, BORDERSTYLE_BEVELED);
      IWIDGET_SetLeftPadding(piw, 2);
      IWIDGET_SetRightPadding(piw, 2);

      IWIDGET_SetPreferredExtent(piw, NULL);
   
      *ppo = (Fct*)me;

   } else {
      Fct_Delete((Fct*)me);
      *ppo = 0;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// SubmitFormControl


static __inline int SubmitFormControl_New(Fct **ppo, IShell *piShell, 
                                          IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv) {
   return ButtonFct_New(ppo, piShell, piDoxPos, pfn, pv, FCT_SUBMIT);
}


/////////////////////////////////////////////////////////////////////////////
// ResetFormControl

static __inline int ResetFormControl_New(Fct **ppo, IShell *piShell, 
                                         IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv) {
   return ButtonFct_New(ppo, piShell, piDoxPos, pfn, pv, FCT_RESET);
}


/////////////////////////////////////////////////////////////////////////////
// ButtonFormControl

//static int ButtonFormControl_New(Fct **ppo, IShell *piShell, 
//                                 IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv) {
//   return ButtonFct_New(ppo, piShell, piDoxPos, pfn, pv, FCT_BUTTON);
//}


/////////////////////////////////////////////////////////////////////////////
// SelectFormControl


static void SelectFct_Reset(Fct *me)
{
}


static boolean SelectFct_HandleEvent(Fct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_FCT_RESET:
         SelectFct_Reset(me);
         return TRUE;
      
      case EVT_FCT_STOP:
         return TRUE;
      
      case EVT_FCT_GETDATA:
         return TRUE;
      
      case EVT_FCT_SETDATA:
         return TRUE;
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)FCT_SELECT;
         return TRUE;
   }

   return Fct_HandleEvent(me, evt, wParam, dwParam);
}


static int SelectFormControl_New(Fct **ppo, IShell *piShell, 
                                 IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv)
{
   Fct *me = 0;
   int nErr = Fct_New(&me, 0, piShell, piDoxPos, AEECLSID_STATICWIDGET, 
                      (PFNHANDLER)SelectFct_HandleEvent, 0, 
                      (PFNFCTNOTIFY)pfn, pv);
   if (!nErr) {
      WExtent we;
      IWIDGET_GetPreferredExtent(me->piw, &we);
      IWIDGET_SetExtent(me->piw, &we);
      SelectFct_Reset(me);
      *ppo = me;
   } else {
      *ppo = 0;
   }

   return nErr;
}




/////////////////////////////////////////////////////////////////////////////
// TextFormControl


typedef struct TextFct {
   Fct   base;
   int   nMaxLen;
   int   nBufSize;
   // text buffer follows
} TextFct;


static void TextFct_Reset(TextFct *me)
{
   AECHAR *pw = (AECHAR*)(me + 1);
   
   IDOXPOS_GetAttrValue(me->base.piDoxPos, VALUE, pw, me->nBufSize);
   IWIDGET_SetText(me->base.piw, pw, FALSE); // FALSE means widget does not free the string
}


static boolean TextFct_HandleEvent(TextFct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_WDG_CANTAKEFOCUS:
         // we take focus
         *((boolean *)dwParam) = 1;
         return TRUE;

      case EVT_FCT_RESET:
         TextFct_Reset(me);
         return TRUE;
      
      case EVT_FCT_STOP:
         return TRUE;   // nothing to do (?)
      
      case EVT_FCT_GETDATA:
		 {
          int nErr = 0;
          int nachLen = 0;
          int nEncCount = 0;
          int nDest = wParam;
          int nDestUsed = 0;
          char *pszDest = NULL;
          AECHAR *pachText = NULL;
          IValueModel *pivm = NULL;
          IDoxUtil* pitDoxUtil;

          nErr = IWIDGET_GetModel(me->base.piw, AEEIID_VALUEMODEL, (IModel **)&pivm);
          if (SUCCESS == nErr) {
             pachText = (AECHAR *)IVALUEMODEL_GetText(pivm, &nachLen);
		       // encode
             nErr = IDOXPOS_QueryInterface(me->base.piDoxPos, AEEIID_DOXUTIL, (void**)&pitDoxUtil);
             if (SUCCESS == nErr) {
                if (0 == nDest) {
                   // counting mode
                   nEncCount = IDOXUTIL_Encode(pitDoxUtil, NULL, nDest, pachText, nachLen, &nDestUsed, TRUE);
                   *(int *)dwParam = nEncCount;
                } else {
			          pszDest = (char *)dwParam;
                   nEncCount = IDOXUTIL_Encode(pitDoxUtil, pszDest, nDest, pachText, nachLen, &nDestUsed, TRUE);
                }
             }
             RELEASEIF(pitDoxUtil);
          }
          RELEASEIF(pivm);

          if (SUCCESS != nErr) {
             return FALSE;
          }

			 return TRUE;
		 }
      
      case EVT_FCT_SETDATA:
         {
             int nDecCount = 0;
             const char *pszSrc = (const char *) dwParam;
             int nSrcLen = STRLEN(pszSrc);
             int nSrcUsed = 0;
             AECHAR *pwszDest = NULL;
             AECHAR achNull = 0;
             IDoxUtil* pitDoxUtil;

             if (SUCCESS != IDOXPOS_QueryInterface( me->base.piDoxPos, AEEIID_DOXUTIL, (void **)&pitDoxUtil) ) {
                return FALSE;
             }

             // counting mode
             nDecCount = IDOXUTIL_Decode( pitDoxUtil, NULL, 0, pszSrc, nSrcLen, &nSrcUsed, TRUE );
             if ( nDecCount > 0 ) {
                pwszDest = (AECHAR *) MALLOC( nDecCount * sizeof(AECHAR) );
             } else if ( nDecCount == 0 ) {
               pwszDest = (AECHAR *) MALLOC( 1 * sizeof(AECHAR) );
               WSTRNCOPYN( pwszDest, 1, &achNull, 1 );
             }
             if ( NULL != pwszDest ) {
                nDecCount = IDOXUTIL_Decode( pitDoxUtil, pwszDest, nDecCount, pszSrc, nSrcLen, &nSrcUsed, TRUE );
                IWIDGET_SetText( me->base.piw, pwszDest, TRUE );
             }
             RELEASEIF(pitDoxUtil);
             return TRUE;
         }
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)FCT_TEXT;
         return TRUE;
   }

   return Fct_HandleEvent(&me->base, evt, wParam, dwParam);
}


static int TextFormControl_New(Fct **ppo, IShell *piShell, 
                               IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv)
{
   TextFct *me = 0;
   int nMaxLen, nBufSize;
   int nErr;

   nMaxLen = IDOXPOS_GetAttrInt(piDoxPos, MAXLENGTH, -1);
   nBufSize = (nMaxLen < 0) ? 80 : nMaxLen + 1;

   nErr = Fct_New((Fct**)&me, (sizeof(Fct) + nBufSize), piShell, piDoxPos, 
                  AEECLSID_STATICWIDGET, (PFNHANDLER)TextFct_HandleEvent, 
                  0, (PFNFCTNOTIFY)pfn, pv);

   if (!nErr) {
      // get pointer to text buffer
      AECHAR *pw = (AECHAR*)(me + 1);
      int nSize, i;
      IWidget *piw = me->base.piw;

      me->nMaxLen  = nMaxLen;
      me->nBufSize = nBufSize;

      IWIDGET_SetActiveBorderWidth(piw, 2);
      IWIDGET_SetInactiveBorderWidth(piw, 1);
      IWIDGET_SetBorderColor(piw, RGBA_BLACK);
      IWIDGET_SetBGColor(piw, RGBA_WHITE);
      IWIDGET_SetPadding(piw, 2);

      // set the extent based on size & maxlength attributes
      nSize = IDOXPOS_GetAttrInt(me->base.piDoxPos, SIZE, 7);
      nSize = MIN(nSize, nBufSize-1);
      for (i=0; i < nSize; i++) {
         pw[i] = (AECHAR)'0';   // nominal char width
      }
      pw[i] = 0;
      IWIDGET_SetText(piw, pw, FALSE);   // FALSE means widget does not free the string
      IWIDGET_SetPreferredExtent(piw, NULL);
      
      TextFct_Reset(me);

      *ppo = (Fct*)me;
   } else {
      *ppo = 0;
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// ImageFormControl


typedef struct ImageFct {
   Fct            base;
   IDoxUtil*      piDoxUtil;
   IWeb *         piWeb;
   IWebResp *     piWebResp;
   AEECallback    cbkResp;
   char *         pszUrl;
   AECHAR *       pwszAlt;
   int            dx;
   int            dy;
   int            dxDest;     // width of destination bitmap
   int            dyDest;     // height of destination bitmap
   int            nImageSize;
   flg            bAttrWidth : 1;   // width attribute present
   flg            bAttrHeight: 1;   // height attribute present
   flg            bAnimated : 1;
   flg            bError : 1;
   flg            bInvalid : 1;
} ImageFct;


static void ImageFct_Dtor(ImageFct *me)
{
   CALLBACK_Cancel(&me->cbkResp);

   RELEASEIF(me->piDoxUtil);
   RELEASEIF(me->piWeb);
   RELEASEIF(me->piWebResp);

   FREEIF(me->pszUrl);
   FREEIF(me->pwszAlt);
}

static void ImageFct_Reset(ImageFct *me)
{ /* nada */ }


static boolean ImageFct_HandleEvent(ImageFct *me, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch (evt) {
      
      case EVT_WDG_CANTAKEFOCUS:
         // we take focus
         *((boolean *)dwParam) = 1;
         return TRUE;

      case EVT_FCT_RESET:
         ImageFct_Reset(me);
         return TRUE;
      
      case EVT_FCT_STOP:
         // stop image loading here
         return TRUE;
      
      case EVT_FCT_GETDATA:
         return TRUE;
      
      case EVT_FCT_SETDATA:
         return TRUE;
      
      case EVT_FCT_GETTYPE:
         *((uint32*)dwParam) = (uint32)FCT_IMAGE;
         return TRUE;
   }

   return Fct_HandleEvent(&me->base, evt, wParam, dwParam);
}


static const char *GetMimeType(const char *pszSrc)
{
   /* TODO - call the shell for help with some content */
   if (pszSrc && *pszSrc) {
      char *pext = STRRCHR(pszSrc, '.');
      if (pext) {
         if (STREQI(pext+1, "gif")) {
            return "image/gif";
         
         } else if (STREQI(pext+1, "jpeg") || STREQI(pext+1, "jpg") ) {
            return "image/jpeg";
         
         } else if (STREQI(pext+1, "bmp")) {
            return "image/bmp";
         
         } else if (STREQI(pext+1, "png")) {
            return "image/png";
         
         } else if (STREQI(pext+1, "bci")) {
            return "image/bci";
         }
      }
   }
   return NULL;
}

// check if the extension in the URL matches a common but unsupported type
static boolean CheckSourceExt(IShell *piShell, const char *pszSrc)
{
   const char * pszMimeType = GetMimeType(pszSrc);
   
   return (0 == pszMimeType ||
           0 != ISHELL_GetHandler(piShell, HTYPE_VIEWER, pszMimeType));
}


static void ImageFct_ReadStatusCB(ImageFct *me, int32 rvRead)
{
#if 0
   if (rvRead > 0) {
      // don't let nBytes overflow nImageSize
      me->nBytes = MIN(me->nImageSize, me->nBytes + rvRead);
   }
   // Invalidate ???
#endif
}


static void ImageFct_Loaded(ImageFct *me, IImage *pii, AEEImageInfo *paii, int nErr)
{
   if (!nErr) {
      
      if (me->dx != paii->cx || me->dy != paii->cy) {

         if (me->bAttrWidth && !me->bAttrHeight) {
            me->dy = paii->cy * me->dx / (MAX(1,paii->cx));

         } else if (!me->bAttrWidth && me->bAttrHeight) {
            me->dx = paii->cx * me->dy / (MAX(1,paii->cy));

         } else if (!me->bAttrWidth) { /* neither specified */
            me->dy = paii->cy;
            me->dx = paii->cx;
         }
         
         if (me->dy > me->dyDest || me->dx > me->dxDest) {
            goto toobig;
         }

         IIMAGE_SetParm(pii, IPARM_SCALE, me->dx, me->dy);
      }

      me->bAnimated = paii->bAnimated;

      if (me->bAnimated) {
         IWIDGET_Animate(me->base.piw, 1);
      }

   } else {

   toobig:
      {
         IImage *piiDef = 0;
         if (SUCCESS == IDOXUTIL_GetImage(me->piDoxUtil, IDI_BROKENIMAGE, &piiDef)) {
            IWIDGET_SetImage(me->base.piw, piiDef);
         }
         RELEASEIF(piiDef);
      }
      
      me->bError = 1;
   }

   Fct_Notify((Fct*)me, FCN_OBJECTLOADED);
   Fct_Notify((Fct*)me, FCN_REWRAP);
}



static void ImageFct_GotResp(ImageFct *me)
{
   IImage *piImage = 0;
   int nErr = 0;
   
   WebRespInfo *pwri = IWEBRESP_GetInfo(me->piWebResp);

   if (WEB_ERROR_SUCCEEDED(pwri->nCode) && pwri->pisMessage) {
      AEECLSID  clsid = 0;
      const char *pc;

      me->nImageSize = pwri->lContentLength;
   
      // see if shell has an IImage to support mime type
      if (pwri->cpszContentType) {

         pc = STRCHRSEND(pwri->cpszContentType," ;");

         // don't need URL anymore, we can overwrite it
         {
            int cb = STRLEN(me->pszUrl) + 1;
            nErr = StrReplace(&me->pszUrl, &cb, 
                              pwri->cpszContentType, pc-pwri->cpszContentType);
         }
         if (!nErr) {
            clsid = ISHELL_GetHandler(me->base.piShell, HTYPE_VIEWER, me->pszUrl);
         }
      } 

      // if nothing found on cpszContentType, make a guess based on extension
      // @@@ TODO - call the shell with a little data...
      if (0 == clsid && NULL != (pc = GetMimeType(me->pszUrl))) {
         int cb = STRLEN(me->pszUrl) + 1;
         nErr = StrReplace(&me->pszUrl, &cb, pc, STRLEN(pc));
         if (!nErr) {
            clsid = ISHELL_GetHandler(me->base.piShell, HTYPE_VIEWER, me->pszUrl);
         }
      }
      
      if (!nErr && 
            0 != clsid && 
            SUCCESS == ISHELL_CreateInstance(me->base.piShell, clsid, (void**)&piImage)) {
         IAStream *pias;
         if (NULL != (pias = SourceAStream_Wrap(pwri->pisMessage, 
                                               (PFNREADSTATUS)ImageFct_ReadStatusCB,
                                               me, me->base.piShell))) {
            IIMAGE_Notify(piImage, (PFNIMAGEINFO)ImageFct_Loaded, me);
            
            IIMAGE_SetStream(piImage, pias);
            IASTREAM_Release(pias);    // pias now owned by IImage (SetStream)

            IWIDGET_SetImage(me->base.piw, piImage);  // piImage now owned by widget

            {  WExtent we;
               we.width = me->dx;
               we.height = me->dy;
               IWIDGET_SetExtent(me->base.piw, &we);
            }
         }
      }
   }
   // don't need this thing anymore, and if there's a stream we're 
   // not reading, this'll clean it up 
   RELEASEIF(me->piWebResp);
   
   // if IImage could not be created, set a default/broken image
   if (!piImage) {
      if (SUCCESS == IDOXUTIL_GetImage(me->piDoxUtil, IDI_BROKENIMAGE, &piImage)) {
         IWIDGET_SetImage(me->base.piw, piImage);
         RELEASEIF(piImage);
      }
      Fct_Notify((Fct*)me, FCN_OBJECTLOADED);
      Fct_Notify((Fct*)me, FCN_REWRAP);
   }

   RELEASEIF(piImage);
}


static int ImageFct_LoadAltText(ImageFct *me)
{
   IWidget *piw = 0;
   int nErr = 0;
   int cchTemp;
   AECHAR *pwTemp;
   
   pwTemp = IDOXUTIL_GetTempBuffer(me->piDoxUtil, &cchTemp);

   if (0 == IDOXPOS_GetAttrValue(me->base.piDoxPos, ALT, pwTemp, cchTemp)) {
      nErr = EFAILED;
   }

   if (!nErr) {
      nErr = ERR_WSTRDUP(pwTemp, &me->pwszAlt);
   }

   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->base.piShell, AEECLSID_STATICWIDGET, (void**)&piw);
   }

   // replace image widget with a static widget and set alt text
   if (!nErr) {
      WExtent we;
      IWidget *piwOld;
      
      we.width = me->dx;
      we.height = me->dy;
      IWIDGET_SetExtent(piw, &we);

      // get the original widget
      piwOld = me->base.piw;

      // remove the handler so we don't get freed
      // this swaps all HandlerDesc data back
      IWIDGET_SetHandler(piwOld, &me->base.hd);

      IWIDGET_SetHandler(piw, &me->base.hd);

      // shove us back into the DoxObj table
      me->base.piw = piw;

      // finally release the old widget
      IWIDGET_Release(piwOld);

      IWIDGET_SetBorderWidth(piw, 1);
      IWIDGET_SetBorderColor(piw, RGBA_BLACK);
      IWIDGET_SetFlags(piw, SWF_WRAPTEXT);
      nErr = IWIDGET_SetText(piw, me->pwszAlt, 0);
      
      Fct_Notify((Fct*)me, FCN_OBJECTLOADED);
      Fct_Notify((Fct*)me, FCN_REWRAP);
   }

   return nErr;
}


/* ImageFct_LoadImage
||
|| Get source URL for image from document, expand it to full URL, 
|| check file extension, then initiate IWeb transaction to load
|| the image.  Load alternate text if image type is not supported.
||
*/
static int ImageFct_LoadImage(ImageFct *me)
{
   int nErr = 0;
   
   int cchTemp;
   AECHAR *pwTemp = IDOXUTIL_GetTempBuffer(me->piDoxUtil, &cchTemp);

   if (!nErr) {
      if (0 == IDOXPOS_GetAttrValue(me->base.piDoxPos, SRC, pwTemp, cchTemp)) {
         goto alttext;
      }
   }

   if (!nErr) {
      FREEIF(me->pszUrl);
      nErr = IDoxUtil_MakeUrlAlloc(me->piDoxUtil, pwTemp, &me->pszUrl);
   }

   if (!nErr) {
      if (CheckSourceExt(me->base.piShell, me->pszUrl)) {
         // initiate web request
         CALLBACK_Init(&me->cbkResp, ImageFct_GotResp, me);
         IWEB_GetResponseV(me->piWeb, &me->piWebResp, &me->cbkResp,
                           me->pszUrl, NULL);

         // have our 'DoxUtil' count the image
         IDOXUTIL_IncrementImageCount(me->piDoxUtil, 1);
      
      } else {
alttext:   
         nErr = ImageFct_LoadAltText(me);
      }
   }

   // no longer need the IWeb!
   RELEASEIF(me->piWeb);
   return nErr;
}


static int ImageFormControl_New(Fct **ppo, IShell *piShell, 
                                IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pv)
{
   ImageFct *me = 0;
   int nErr;

   nErr = Fct_New((Fct**)&me, sizeof(ImageFct), piShell, piDoxPos, 
                  AEECLSID_IMAGEWIDGET, (PFNHANDLER)ImageFct_HandleEvent, 
                  (PFNDTOR)ImageFct_Dtor, (PFNFCTNOTIFY)pfn, pv);

   // get the IDoxUtil interface
   if (!nErr) {
      nErr = IDOXPOS_QueryInterface(me->base.piDoxPos, AEEIID_DOXUTIL, (void**)&me->piDoxUtil);
   }

   // get the IWeb interface
   if (!nErr) {
      nErr = IDOXUTIL_GetIWeb(me->piDoxUtil, &me->piWeb);
   }

   // get the size of the destination bitmap
   if (!nErr) {
      nErr = IShell_GetDestBitmapSize(piShell, &me->dxDest, &me->dyDest);
   }

   if (!nErr) {

      IWidget *piw = me->base.piw;

      IImage *pii = 0;
      int dxDef=0, dyDef=0;

      // load the default image
      if (SUCCESS == IDoxUtil_LoadImageAndGetSize(me->piDoxUtil, IDI_IMAGE, &pii, 
                                                  &dxDef, &dyDef)) {
         WExtent we;
         IWIDGET_SetImage(piw, pii);
         we.width = dxDef;
         we.height = dyDef;
         IWIDGET_SetExtent(piw, &we);
         IIMAGE_Release(pii);
      }

      // get the width and height attributes
      me->dx = IDOXPOS_GetAttrInt(piDoxPos, WIDTH, -999);
      me->dy = IDOXPOS_GetAttrInt(piDoxPos, HEIGHT, -999);
      
      // remember whether attributes were actually present
      me->bAttrWidth = (me->dx != -999);
      me->bAttrHeight = (me->dy != -999);
      
      if (HWF_LOADALLIMAGES & IDOXUTIL_GetFlags(me->piDoxUtil)) {
         me->dx = CONSTRAIN(me->dx, dxDef, me->dxDest);
         me->dy = CONSTRAIN(me->dy, dyDef, me->dyDest);
      }

      // fixup invalid size values
      if (me->dx < 0 || me->dy < 0 || me->dx > me->dxDest || me->dy > me->dyDest) {
         me->dx = dxDef;
         me->dy = dxDef;
         me->bInvalid = 1;
      }

   }
   if (!nErr && !me->bInvalid) {
      nErr = ImageFct_LoadImage(me);
   }

   if (!nErr) {
      *ppo = (Fct*)me;
   } else {
      Fct_Delete((Fct*)me);
      *ppo = 0;
   }

   return nErr;
}



/////////////////////////////////////////////////////////////////////////////
//


int FormControl_New(Fct **ppo, IShell *piShell, IDoxPos *piDoxPos, PFNFCTNOTIFY pfn, void *pCtx)
{
   int nErr = EFAILED;
   
   // <INPUT> element
   if (IDOXPOS_IsElementName(piDoxPos, INPUT)) {

      // type=checkbox
      if (IDOXPOS_IsAttrValue(piDoxPos, TYPE, CHECKBOX)) {
         nErr = CheckFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);
      
      // type=radio
      } else if (IDOXPOS_IsAttrValue(piDoxPos, TYPE, RADIO)) {
         nErr = RadioFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);

      // type=submit
      } else if (IDOXPOS_IsAttrValue(piDoxPos, TYPE, SUBMIT)) {
         nErr = SubmitFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);

      // type=reset
      } else if (IDOXPOS_IsAttrValue(piDoxPos, TYPE, RESET)) {
         nErr = ResetFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);

      // type=image
      } else if (IDOXPOS_IsAttrValue(piDoxPos, TYPE, IMAGE)) {
         nErr = ImageFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);

      // type=hidden
      } else if (!IDOXPOS_IsAttrValue(piDoxPos, TYPE, HIDDEN)) {
         nErr = TextFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);
      }
   
   // <SELECT> element
   }  else if (IDOXPOS_IsElementName(piDoxPos, SELECT)) {
      nErr = SelectFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);
   
   // <IMG> element
   } else if (IDOXPOS_IsElementName(piDoxPos, IMG)) {
      nErr = ImageFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);
   
   // <TEXTAREA> element
//   } else if (IDOXPOS_IsElementName(piDoxPos, TEXTAREA)) {
//      nErr = TextAreaFormControl_New(ppo, piShell, piDoxPos, pfn, pCtx);
   }

   return nErr;
}



/////////////////////////////////////////////////////////////////////////////
// DoxObjMgr

#define MAXDOXOBJECTS   40

int DoxObjMgr_Construct(DoxObjMgr *me, Dox *pDox, PFNFCTNOTIFY pfn, void *pctx)
{
   me->nSize = 0;
   me->pdo = MALLOC(sizeof(DoxObj) * MAXDOXOBJECTS);
   if (!me->pdo) {
      return ENOMEMORY;
   }
   
   me->nSize   = MAXDOXOBJECTS;
   me->pDox    = pDox;
   me->pfn     = pfn;
   me->pctx    = pctx;
   return SUCCESS;
}


void DoxObjMgr_Dtor(DoxObjMgr *me)
{
   DoxObjMgr_Reset(me);
   FREEIF(me->pdo);
}


void DoxObjMgr_Reset(DoxObjMgr *me)
{
   DoxObj *pdo;
   for (pdo=me->pdo; pdo < me->pdo + me->nSize; pdo++) {
      if (pdo->pfct) {
         Fct_Delete(pdo->pfct);
      }
      pdo->pfct = 0;
      pdo->id = 0;
   }
}


// find DoxObj associated with pos or the next free slot
static DoxObj *DoxObjMgr_Find(DoxObjMgr *me, int pos)
{
   DoxObj *pdo;
   DoxObj *pdoMax = me->pdo + me->nSize;
   
   for (pdo=me->pdo; pdo < pdoMax && pdo->id != 0; pdo++) {
      if (pdo->id == pos+1) {
         break;
      }
   }

   return (pdo < pdoMax) ? pdo : NULL;
}


IFormControl *DoxObjMgr_ObtainObject(DoxObjMgr *me, int pos)
{
   DoxObj *pdo = DoxObjMgr_Find(me, pos);
   if (!pdo) {
      return NULL;   // no more room
   }

   if (!pdo->pfct) {

      Fct *pfct = 0;

      // create IDoxPos
      IDoxPos *pdp = IDoxPos_New(me->pDox, pos);
      
      // create form control
      if (pdp && 
            SUCCESS == FormControl_New(&pfct, me->pDox->piShell, pdp, me->pfn, me->pctx)) {
         pdo->id     = pos+1;
         pdo->pfct   = pfct;
         IWIDGET_SetParent(pfct->piw, (IContainer*)me->pDox);
      }
   
      RELEASEIF(pdp);
   }

   return (pdo->pfct ? pdo->pfct->piw : NULL);
}


int DoxObjMgr_GetObjectPos(DoxObjMgr *me, IFormControl *pifc)
{
   DoxObj *pdo;
   for (pdo=me->pdo; pdo < (me->pdo + me->nSize) && pdo->id != 0; pdo++) {
      if (pdo->pfct && pdo->pfct->piw == (IWidget*)pifc) {
         return (pdo->id - 1);
      }
   }

   return -1;
}



