/*
  ========================================================================

  FILE:           HFontOutline.h

  SERVICES:       Extended IFont object that implements HandleEvent and
                  renders its glyphs with a glow effect.

  PUBLIC CLASSES: IHFont

  DESCRIPTION:    This file describes an interface that provides a wrapper
                  around IFont objects that measure and draw each glyph
                  ith an outline of user-specified width.
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __HFONT_OUTLINE_H__
#define __HFONT_OUTLINE_H__

#include "AEEHFontOutline.h"

int HFontOutline_New(IHFont **ppo, IShell *pIShell, IModule *piModule);

#endif // __HFONT_OUTLINE_H__