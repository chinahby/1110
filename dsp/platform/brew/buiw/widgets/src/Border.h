/*======================================================
FILE:       Border.h

SERVICES:   Widget border drawing/managment

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the
  written consent of QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __BORDER_H__
#define __BORDER_H__

#include "WidgetBase.h"
#include "AEEContainer.h"


#define RGBINDEX_INACTIVE    0     // index into rgb and width arrays
#define RGBINDEX_ACTIVE      1     // index into rgb and width arrays
#define RGBINDEX_SINACTIVE   2     // index into rgb arrays
#define RGBINDEX_SACTIVE     3     // index into rgb arrays 

typedef void (*PFNINVALIDATE) (void *p, uint32 dwFlags);


typedef struct BackImage BackImage;
struct BackImage {
   IImage *       piImage;

   PFNINVALIDATE  pfnInval;   
   void *         pvInval;

   boolean        bAnimating;

   int            x;          // x offset
   int            y;          // y offset
   int            dx;         // dx size
   int            dy;         // dy size
   int            odx;        // original dx
   int            ody;        // original dy
   int            sdx;        // scaled dx
   int            sdy;        // scaled dy
   boolean        bScaled;    // set to true if the image has been scaled

   int            nFrames;
   uint32         nCurStep, nCurRep;
   int            nCurFrame;  // which frame to draw

   uint32         dwFlags;
   uint16         bdl, bdr, bdt, bdb;

   IModel**       piViewModel;
   uint32         dwAnimFlags;   // flags specific to animation
};


typedef struct Shadow Shadow;
struct Shadow {
   RGBVAL   rgba;
   int8     nOffsetX;
   int8     nOffsetY;
};


typedef struct DynRGB DynRGB;
struct DynRGB {
   union {
      RGBVAL   rgb;
      RGBVAL * prgb;
   } u;
   uint8 nMaxIndex    : 7;       // max index (if expanded)
   uint8 bDominantSet : 1;       // if the dominant color slot has been set (BG, FG, ...)
};

typedef struct DynShadow DynShadow;
struct DynShadow {
   union {
      Shadow shadow;
      Shadow *pshadow;
   } u;
   uint8 nMaxIndex;
};


typedef struct Border Border;
struct Border {

   WExtent *      pExtent;       // pointer to extent struct in WidgetBase or ContainerBase 
   PFNINVALIDATE  pfnInval;   
   void *         pvInval;

   AEERect     rcClient;         // client area - rectangle in widget coordinates
                                 // subtracts the border and padding
   
   DynRGB      dynBorder;        // RGBA values for border are stored in this object
   DynRGB      dynBG;            // RGBA values for background are stored in this object 
   DynRGB      dynBGE;           // RGBA values for background gradient are stored in this object  

   DynShadow   dynShadow;        // Shadow values for border are stored in this object
   uint8       nWidth[2];        // border width: [0] inactive, [1] active

   uint8       nStyle;           // border style
   uint8       nGradientStyle;   // gradient style
   // flags
   uint8       bActive : 1;      // active flag
   uint8       bSelected : 1;    // selected flag
   uint8       bCanTakeFocus : 1; // whether widget can take focus
   uint8       bUpdateStop : 1;  // update in progress
   
   // padding
   struct {
      uint8    left;
      uint8    right;
      uint8    top;
      uint8    bottom;
   } padding;

   // rounded corners
   uint8       nRadius;          // radius of rounded corners

   BackImage *    pBackImage;    // pointer to BackImage struct, if set

   IModel**       piViewModel;   // viewmodel of widget wrapped in border
};

void     DynRGB_Ctor(DynRGB *me, RGBVAL rgbDef);
void     DynRGB_Dtor(DynRGB *me);
int      DynRGB_Set(DynRGB *me, int nIndex, RGBVAL rgb);
boolean  DynRGB_Get(DynRGB *me, int nIndex, RGBVAL *prgb);
void     DynRGB_Collapse(DynRGB *me, RGBVAL rgb);
boolean  DynRGB_Compare(DynRGB *me, int n1, int n2);

void     Border_CtorZ            (Border *me, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel);
void     Border_Ctor             (Border *me, PFNINVALIDATE pfnInval, void *pvInval, WExtent *pExtent, boolean bCanTakeFocus, IModel **piViewModel);
void     Border_Dtor             (Border *me);
boolean  Border_IntersectOpaque  (Border *me, AEERect *prcOut, const AEERect *prcIn);
boolean  Border_HandleEvent      (Border *me, AEEEvent evt, uint16 wParam, uint32 dwParam);
void     Border_Draw             (Border *me, ICanvas *piCanvas, int x, int y);
void     Border_CalcPreferredExtent(Border *me, WExtent *pweOut, WExtent *pweClientIn);
void     Border_AdjustDisplayClipRect(Border *me, ICanvas *piCanvas, int x, int y);
void     Border_CalcClientRect   (Border *me);

RGBVAL   Border_GetCurrentBorderColor (Border *me);
RGBVAL   Border_GetCurrentBGColor     (Border *me);

void     Border_SetBorderColor   (Border *me, int index, RGBVAL rgb);
void     Border_SetBGColor       (Border *me, int index, RGBVAL rgb); 
void     Border_GetBorderColor   (Border *me, int index, RGBVAL *prgb);
void     Border_GetBGColor       (Border *me, int index, RGBVAL *prgb); 

void     Border_GetShadowOffsets (Border *me, int index, int *pnOffsetX, int *pnOffsetY);

static __inline void Border_SetPfnInval(Border *me, PFNINVALIDATE pfnInval, void *pvInval) {
   me->pfnInval = pfnInval;
   me->pvInval  = pvInval;
}

static __inline void Border_GetCurrentShadowOffsets(Border *me, int *pnOffsetX, int *pnOffsetY) {
   Border_GetShadowOffsets(me, -1, pnOffsetX, pnOffsetY);
}

#endif   //__BORDER_H__

