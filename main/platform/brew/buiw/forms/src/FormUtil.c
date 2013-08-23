/*
  ========================================================================

  FILE:  FormUtil.c
  
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

#include "FormUtil.h"
#include "AEEResFile.h"
#include "AEEStaticWidget.h"
#include "AEEImageWidget.h"
#include "AEERootForm.h"
#include "AEEMimeTypes.h"


/* Form_SetPrefRect

   Calculates the desired rectangle of a dialog based on percentage size,
   and offset given in percentage or absolute coordinates.
   
   nPctWidth      desired width in percentage of client width
   nPctHeigh      desired height in percentage of client height
   xOffset        x-dim offset (neg value is absolute coordinates, pos value means pct.)
   yOffset        y-dim offset (neg value is absolute coordinates, pos value means pct.)
   bRefRight      TRUE means xOffset is referenced from dialog/client right edge (both abs and %)
   bRefRight      TRUE means yOffset is referenced from dialog/client bottom edge (both abs and %)
*/
void Form_SetPrefRect(IForm *po, int nPctWidth, int nPctHeight, int xOffset, int yOffset, 
                      boolean bRefRight, boolean bRefBottom, AEERect *prc)
{
   IRootForm *piRoot = 0;
   IWidget *piw = 0;

   if (SUCCESS == IFORM_GetRootForm(po, &piRoot)) {

      WExtent wePercent, we, weMax;
      int xSpace, ySpace, xAdjust, yAdjust;

      IROOTFORM_GetClientRect(piRoot, NULL, prc);
      SETWEXTENT(&weMax, prc->dx, prc->dy);

      nPctWidth = CONSTRAIN(nPctWidth, 0, 100);
      nPctHeight = CONSTRAIN(nPctHeight, 0, 100);

      // Have the dialog take up a certain percentage of the width of the screen
      SETWEXTENT(&wePercent, 
                 (prc->dx * nPctWidth) / 100, 
                 (prc->dy * nPctHeight) / 100);

      if (SUCCESS == IFORM_GetWidget(po, WID_FORM, &piw)) {
         // Initially make dialog that big
         IWIDGET_SetExtent(piw, &wePercent);
         // Then ask it how big it really wants to be
         IWIDGET_GetPreferredExtent(piw, &we);
      } else {
         // No widget, fuhgeddaboudit
         SETWEXTENT(&we, 0, 0);
      }

      // And set the extent as the maximum of the two, but no larger than
      // the rootform's client rect
      we.width    = MAX(wePercent.width, we.width);
      we.width    = MIN(we.width, weMax.width);
      we.height   = MAX(wePercent.height, we.height);
      we.height   = MIN(we.height, weMax.height);

      xSpace = prc->dx - we.width;
      ySpace = prc->dy - we.height;

      if (xOffset < 0) {
         xAdjust = -xOffset;
      } else {
         xOffset = CONSTRAIN(xOffset, 0, 100);
         xAdjust = xSpace * xOffset / 100;
      }
      if (bRefRight) {
         xAdjust = xSpace - xAdjust;
      }

      if (yOffset < 0) {
         yAdjust = -yOffset;
      } else {
         yOffset = CONSTRAIN(yOffset, 0, 100);
         yAdjust = ySpace * yOffset / 100;
      }
      if (bRefBottom) {
         yAdjust = ySpace - yAdjust;
      }

      prc->x = prc->x + (int16)xAdjust;
      prc->y = prc->y + (int16)yAdjust;
      prc->dx = (int16)we.width;
      prc->dy = (int16)we.height;
   }
   RELEASEIF(piRoot);
   RELEASEIF(piw);
}


int FormUtil_ISHELL_CreateFont(IShell *piShell, AEECLSID fontClass, IFont **ppo)
{
   int result;
   IFont *piFont = 0;
   IDisplay *piDisplay = 0;

   result = ISHELL_CreateInstance(piShell, fontClass, (void **)&piFont);

   if (result == ECLASSNOTSUPPORT 
       && ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void **)&piDisplay) == 0) {

      switch(fontClass) {
      case AEECLSID_FONTSYSNORMAL:
         piFont = FormUtil_IDISPLAY_GetFont(piDisplay, AEE_FONT_NORMAL);
         result = 0;
         break;
      case AEECLSID_FONTSYSBOLD:
         piFont = FormUtil_IDISPLAY_GetFont(piDisplay, AEE_FONT_BOLD);
         result = 0;
         break;
      case AEECLSID_FONTSYSLARGE:
         piFont = FormUtil_IDISPLAY_GetFont(piDisplay, AEE_FONT_LARGE);
         result = 0;
         break;
	  case AEECLSID_FONTSYSITALIC:
	  	 piFont = FormUtil_IDISPLAY_GetFont(piDisplay, AEE_FONT_SMALL);
         result = 0;
         break;		 
	  case AEECLAID_FONTSMALL:
         piFont = FormUtil_IDISPLAY_GetFont(piDisplay, AEE_FONT_SMALL);
         result = 0;
         break;	 

      default:
         break;
      }
   }

   *ppo = piFont;
   RELEASEIF(piDisplay);
   return result;
}

static int ISHELL_GetResStringSize(IShell *piShell, const char *pszFile, uint16 idRes)
{
   uint32 cbBufsize = 0;
   (void)ISHELL_LoadResDataEx(piShell, pszFile, idRes, RESTYPE_STRING, (void*)-1, &cbBufsize);
   cbBufsize *= sizeof(AECHAR);
   return (int)cbBufsize;
}

int ISHELL_LoadResStringEx(IShell *piShell, const char *resFile, uint16 resid, AECHAR **dest)
{
   AECHAR *pw = NULL;
   int err = SUCCESS;

   int cb = ISHELL_GetResStringSize(piShell, resFile, resid);

   if (cb) {
      pw = MALLOC((unsigned long)cb);
      if (!pw) {
         err = ENOMEMORY;
      }
   } else {
      err = ERESOURCENOTFOUND;
   }

   if (!err) {
      err = ISHELL_LoadResString(piShell, resFile, resid, pw, cb) ? SUCCESS : ERESOURCENOTFOUND;
   }

   if (err) {
      FREEIF(pw);
   }

   *dest = pw;
   return err;
}


int ISHELL_LoadResImageEx(IShell *piShell, const char *resFile, uint16 resid, IImage **ppo)
{
   if ((*ppo = ISHELL_LoadResImage(piShell, resFile, resid)) == 0)
      return ERESOURCENOTFOUND;

   if (ppo) {
      
      // We need to check for PNG images, which do not require a raster operation
      //  as they have alpha / transparency channels already
      boolean bPNG = FALSE;
      void* pData = ISHELL_LoadResData(piShell, resFile, resid, RESTYPE_IMAGE);

      // Image Data: 
      //    1st byte contains data offset
      //    2nd byte is not used (0)
      //    3rd byte through offset is MIME TYPE.  We compare against MT_PNG ("image/png")
      if (pData && ((byte*)pData)[0] > 2) {         
         if (0 == STRCMP((char*)((byte*)pData)+2, MT_PNG)) {
            bPNG = TRUE;
         }
      }

      if (pData) {
         ISHELL_FreeResData(piShell, pData);
         pData = NULL;
      }


      if (!bPNG) {
         IIMAGE_SetParm(*ppo, IPARM_ROP, AEE_RO_MASK, 0);
      }
   }

   return 0;
}

IFont *FormUtil_IDISPLAY_GetFont(IDisplay *piDisplay, AEEFont fontId)
{
   IFont *piFont;

   piFont = IDISPLAY_SetFont(piDisplay, fontId, 0);
   (void)IDISPLAY_SetFont(piDisplay, fontId, piFont);
   IFONT_AddRef(piFont);
   return piFont;
}


int FormRes_GetText(FormRes *me, IShell *piShell, AECHAR **ppo)
{
   if (me->id) {
      return ISHELL_LoadResStringEx(piShell, (const char*)me->pdata, me->id, ppo);
   }
   *ppo = (me->pdata ? WSTRDUP((AECHAR*)me->pdata) : 0);
   return 0;
}

int FormRes_GetImage(FormRes *me, IShell *piShell, IImage **ppo)
{
   if (me->id) {
      return ISHELL_LoadResImageEx(piShell, (const char*)me->pdata, me->id, ppo);
   }
   *ppo = (IImage*)me->pdata;
   ADDREFIF((*ppo));
   return 0;
}

int IWIDGET_SetFormText(IWidget *piWidget, IShell *piShell, FormRes *pfr)
{
   int result;
   AECHAR *txt;

   result = FormRes_GetText(pfr, piShell, &txt);
   
   if (result == 0)
      (void)IWIDGET_SetText(piWidget, txt, TRUE);

   return result;
   
}

int IWIDGET_SetFormTextStatic(IWidget *piWidget, IShell *piShell, FormRes *pfr)
{
   AECHAR *txt = NULL;
   int nErr = SUCCESS;
   boolean bFree = 0;

   // load from resource file if res type
   if (pfr->id) {
      nErr = FormRes_GetText(pfr, piShell, &txt);
      bFree = 1;
   } else {
      txt = (AECHAR*)pfr->pdata;
   }
   
   if (!nErr)
      (void)IWIDGET_SetText(piWidget, txt, bFree);

   return nErr;
}


int IWIDGET_SetFormImage(IWidget *piWidget, IShell *piShell, FormRes *fi)
{
   int result;
   IImage *piImage = 0;

   result = FormRes_GetImage(fi, piShell, &piImage);
   
   if (result == 0)
      (void)IWIDGET_SetImage(piWidget, piImage);

   RELEASEIF(piImage);
   return result;
}

int Theme_ApplyListProps(IResFile *po, IWidget *piList, IWidget *piItem, const char *baseName)
{
   (void)IWIDGET_SetProperties(piList, po, baseName, "List.Properties", 0);
   return IWIDGET_SetProperties(piItem, po, baseName, "List.Item.Properties", 0);
}
