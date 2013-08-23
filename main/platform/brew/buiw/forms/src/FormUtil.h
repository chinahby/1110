/*
  ========================================================================

  FILE:  FormUtil.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Utilities for forms

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __FORMUTIL_H__
#define __FORMUTIL_H__

#include "AEEWidget.h"
#include "AEEFont.h"
#include "AEEForm.h"
#include "AEEResFile.h"

// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt)     MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)) )
#endif

// get pointer to vtbl from object allocated w/ MALLOCREC_VTBL
#ifndef GETVTBL
#define GETVTBL(po,vtt)             ((AEEVTBL(vtt)*)(void*)((po)+1))
#endif

#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif

#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**) (void *) &p)

static __inline void RELEASEPPIF(IBase **p) {
   if (*p) {
      IBASE_Release(*p);
      *p = 0;
   }
}
#endif


// Logging for theme performance - enable #define to turn on logging
// #define BUIT_LOGGING 1 
#ifdef BUIT_LOGGING
#define BUIT_LOG(msg, s) if(s){ DBGPRINTF(msg, s);}
#else
#define BUIT_LOG(msg, s) ;
#endif


#ifndef AEECLSID_FONTSYSNORMAL
#define AEECLSID_FONTSYSNORMAL   0x01012786  // normal system font
#endif

#ifndef AEECLSID_FONTSYSLARGE
#define AEECLSID_FONTSYSLARGE    0x01012787  // large system font
#endif 

#ifndef AEECLSID_FONTSYSBOLD
#define AEECLSID_FONTSYSBOLD     0x01012788  // bold system font
#endif

#ifndef AEECLAID_FONTSMALL
#define AEECLAID_FONTSMALL       0x0101402f
#endif

#ifndef AEECLSID_FONTSYSITALIC
#define AEECLSID_FONTSYSITALIC      0x0101402c
#endif



void Form_SetPrefRect(IForm *po, int nPctWidth, int nPctHeight, int xOffset, int yOffset, 
                      boolean bRefRight, boolean bRefBottom, AEERect *prc);

int ISHELL_LoadResStringEx(IShell *piShell, const char *resFile, uint16 resid, AECHAR **dest);
int ISHELL_LoadResImageEx(IShell *piShell, const char *resFile, uint16 resid, IImage **ppo);

// Prefix the next two functions by FormUtil_ to avoid Linker conflicts 
// with wutil:
int FormUtil_ISHELL_CreateFont(IShell *piShell, AEECLSID fontClass, IFont **ppo);
IFont *FormUtil_IDISPLAY_GetFont(IDisplay *piDisplay, AEEFont fontId);

int Theme_ApplyListProps(IResFile *po, IWidget *piList, IWidget *piItem, const char *baseName);

int FormRes_GetText           (FormRes *ft, IShell *piShell, AECHAR **ppo);
int FormRes_GetImage          (FormRes *fi, IShell *piShell, IImage **ppo);

int IWIDGET_SetFormText       (IWidget *piWidget, IShell *piShell, FormRes *ft);
int IWIDGET_SetFormImage      (IWidget *piWidget, IShell *piShell, FormRes *fi);
int IWIDGET_SetFormTextStatic(IWidget *piWidget, IShell *piShell, FormRes *ft);




#endif /* __FORMUTIL_H__ */
