/*
  ========================================================================

  FILE:  ImageBase.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of ImageWidget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __IMAGEBASE_H__
#define __IMAGEBASE_H__

#include "AEEComdef.h"
#include "AEEShell.h"
#include "AEEImage.h"
#include "AEEWidget.h"
#include "AEEWProperties.h"

typedef void (*PFNINVALIDATE) (void *p, uint32 dwFlags);


typedef struct ImageBase ImageBase;

struct ImageBase {
   IImage *       pii;           // out image
   IImage *       piAnimImage;   // cache image when animating for callback cancellation
   IShell *       piShell;       // IShell needed for animation timer
   AEECallback    cbk;           // animation timer callback
   IModel**       ppiViewModel;  // location of parent's viewmodel

   PFNINVALIDATE  pfnInval;      // invalidation callback function
   void *         pvInval;       // invalidation callback pointer

   int            x;             // x offset
   int            y;             // y offset
   int            dx;            // dx size
   int            dy;            // dy size
   int            odx;           // original dx
   int            ody;           // original dy
   int            sdx;           // scaled dx
   int            sdy;           // scaled dy
   int            nFrames;       // number of frames
   int            nCurFrame;     // which frame to draw
   int            nSaveCurFrame; // save the current frame while animating
   int            nImageFrames;  // number of frames the IImage thinks it has (GIF only currently)
   int            nRate;         // animation rate (for fixed rates)
   int            nCurStep;      // current step of animation
   int            nCurRep;       // current repetition of animation

   uint32         dwAnimFlags;   // animation specific flags
   uint32         dwAlignment;   // IDF_ALIGN_xxx flags

   uint16         bdt, bdb, bdl, bdr;	//tiling border distances see AEEImageWidget.h)

   flg            bHasSelectImage : 1; // xx_HASSELECTIMAGE flag has been set
   flg            bTiled          : 1; // xx_TILED flag has been set
   flg            bScaled         : 1; // set to true if the image has been scaled
   flg            bAnimated       : 1; // set to true if image should be animated
   flg            bVarFrameDelay  : 1; // variable frame delays supported
   flg            bGetNFrames     : 1; // IPARM_GETNFRAMES supported
   flg            bInfoAnimated   : 1; // IIMAGE_GetInfo(), aii.bAnimated state
   flg            bFramesSet      : 1; // # of frames explicitly set by app
   flg            bExternalAnim   : 1; // use native IImage animation (compatability mode for BCI images)
   flg            bSelected       : 1; // selected flag
};


void ImageBase_Ctor        (ImageBase *me, IShell *piShell, PFNINVALIDATE pfnInval, 
                            void *pvInval, IModel **ppiViewModel);

void ImageBase_Dtor        (ImageBase *me);

void ImageBase_SetImage    (ImageBase *me, IImage *pii);

void ImageBase_Animate     (ImageBase *me, boolean bOn);

void ImageBase_Draw        (ImageBase *me, ICanvas *piCanvas, int x, int y,
                            int dxClient, int cyClient);

void ImageBase_GetPreferredExtent  (ImageBase *me, WExtent *pweOut);

void ImageBase_SetParm  (ImageBase *me, ImageParm *pParm);
void ImageBase_SetSelected (ImageBase *me, boolean bSelected);

boolean ImageBase_SetFrame    (ImageBase *me, int nFrame);


#endif   //__IMAGEBASE_H__

