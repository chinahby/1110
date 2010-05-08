/*===========================================================================
                              OEMDisp.c

  This file contains sample source to the IOEMDisp interface.

  Please search for OEM-Note in the file for points of interest.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "OEMFeatures.h"
#include "AEEFont.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"


//---------------------   OEM Font support  -----------------------

extern const VTBL(IBitmap) gOEMBitmapFuncs;

//
//-----------------------------------------------------------------



static uint32 OEMFont_AddRef(IFont *po);
static uint32 OEMFont_Release(IFont *po);
static int    OEMFont_QueryInterface(IFont *po, AEECLSID id, void **ppif);
static int    OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y,
                               const AECHAR *pcText, int nChars, 
                               NativeColor clrFg, NativeColor clrBg, 
                               const AEERect *prcClip, uint32 dwFlags);
static int    OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars,
                                  int nMaxWidth, int * pnFits, int *pnPixels);
static int    OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize);



static const VTBL(IFont) gOEMFontFuncs =   { OEMFont_AddRef,
                                             OEMFont_Release,
                                             OEMFont_QueryInterface,
                                             OEMFont_DrawText,
                                             OEMFont_MeasureText,
                                             OEMFont_GetInfo};
                                    
struct IFont {
   const AEEVTBL(IFont) *pvt;
   AEEFont     nFont;   // lower-level interfaces still use AEE_FONT_... IDs
};




// Three static singleton IFont's to represent the BREW-enumerated fonts:
//
IFont gFontNormal = {&gOEMFontFuncs, AEE_FONT_NORMAL};
IFont gFontBold   = {&gOEMFontFuncs, AEE_FONT_BOLD};
IFont gFontLarge  = {&gOEMFontFuncs, AEE_FONT_LARGE};



int OEMFont_GetSystemFont(AEEFont nFont, IFont **ppif)
{
   if (nFont == AEE_FONT_NORMAL)
      *ppif = &gFontNormal;
   else if (nFont == AEE_FONT_BOLD)
      *ppif = &gFontBold;
   else if (nFont == AEE_FONT_LARGE)
      *ppif = &gFontLarge;
   else
      return EUNSUPPORTED;

   return SUCCESS;
}


static uint32 OEMFont_AddRef(IFont *pme)
{
   return 1;
}


static uint32 OEMFont_Release(IFont *pme)
{
   return 0;
}


static int OEMFont_QueryInterface(IFont *pMe, AEECLSID id, void **pvtbl)
{
   void *po = 0;
   
   if (id == AEECLSID_FONT) {
      IFONT_AddRef(pMe);
      po = (void*)pMe;
   }

   *pvtbl = po;
   return (po != 0 ? SUCCESS : ECLASSNOTSUPPORT);
}


static int
OEMFont_DrawText(IFont *pMe, IBitmap *pDst, int x, int y, const AECHAR *pcText, int nChars,
                 NativeColor foreground, NativeColor background, const AEERect *prcClip, uint32 dwFlags)
{
   int nFits = 0, nHeight = 0;
   int nWidth = 0;
   AEERect rca;

   // ASSERT(pcText);

   // This needs to be done by OEM:
   // Draw text here.
   
   return SUCCESS;
}


static int
OEMFont_GetInfo(IFont *pMe, AEEFontInfo *pInfo, int nSize)
{
   if (nSize != sizeof(AEEFontInfo))
      return EUNSUPPORTED;
   
   if (pInfo)
   {
      // The ascent and descent value must be filled in by OEM
	   if ( pMe->nFont == AEE_FONT_NORMAL )
	   {
         // pInfo->nAscent = NORMAL_ASCENT;
         // pInfo->nDescent = NORMAL_DESCENT;
      }
      else if ( pMe->nFont == AEE_FONT_BOLD )
      {
         // pInfo->nAscent = BOLD_ASCENT;
         // pInfo->nDescent = BOLD_DESCENT;
      }
      else 
      {
         // pInfo->nAscent = LARGE_ASCENT;
         // pInfo->nDescent = LARGE_DESCENT;
      }
   }   
   return SUCCESS;
}


static int
OEMFont_MeasureText(IFont *pMe, const AECHAR *pcText, int nChars, int nMaxWidth, int *pnCharFits, int *pnPixels)
{
   // OEMs need to implement this function
   return SUCCESS;
}

