/*
  ========================================================================

  FILE:  ImageWidget.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Implementation of ImageWidget

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _IMAGEWIDGET_H_
#define _IMAGEWIDGET_H_

#include "AEEImageWidget.h"
#include "DecoratorWidget.h"
#include "Border.h"

typedef struct ImageWidget {
   Decorator      base;

   int            x;          // x offset
   int            y;          // y offset
   int            dx;         // dx size
   int            dy;         // dy size
   int            odx;        // original dx
   int            ody;        // original dy
   int            sdx;        // scaled dx
   int            sdy;        // scaled dy
   boolean        bScaled;    // set to true if the image has been scaled
   boolean        bAnimated;  // set to true if image should be animated
   int            nFrames;    // number of frames
   int            nCurFrame;  // which frame to draw
   Border         border;
   uint32         dwFlags;
   IImage *       piAnimImage; // cache image when animating for callback cancellation
   ModelListener  modelListener;
   uint16         bdt, bdb, bdl, bdr;	//tiling border distances - see Image Widget cls description
   uint32         nCurStep, nCurRep;   // current step and repetition of animation
   uint32         dwAnimFlags;         // animation specific flags
} ImageWidget;

// Safe up-cast
static __inline IWidget *IMAGEWIDGET_TO_IWIDGET(ImageWidget *me) { 
   return (IWidget *) me; 
}
static __inline IDecorator *IMAGEWIDGET_TO_IDECORATOR(ImageWidget *me) { 
   return (IDecorator *) me; 
}

int      ImageWidget_New         (IDecorator **ppo, IModule *piModule);
void     ImageWidget_Ctor        (ImageWidget *me, AEEVTBL(IDecorator) *pvt, IModule *piModule, 
                                  PFNHANDLER pfnDefHandler);
void     ImageWidget_Dtor        (ImageWidget *me);

int      ImageWidget_Initialize  (ImageWidget *me);

uint32   ImageWidget_Release     (IDecorator *po);
void     ImageWidget_SetExtent   (IDecorator *po, WExtent *pExtent);
void     ImageWidget_Draw        (IDecorator *po, ICanvas *piCanvas, int x, int y);
void     ImageWidget_GetPreferredExtent(IDecorator *po, WExtent *pweOut);
boolean  ImageWidget_HandleEvent (IDecorator *po, AEEEvent evt, uint16 wParam, uint32 dwParam);
int      ImageWidget_SetModel    (IDecorator *po, IModel *piModel);


#endif // _IMAGEWIDGET_H_
