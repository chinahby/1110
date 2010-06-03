/*
  ========================================================================

  FILE:                 TextLayout.h
  
  SERVICES:             ITextLayout

  GENERAL DESCRIPTION:  Private declarations for the TextLayout interface,
                        an object that lays out and draws text.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __TEXTLAYOUT_H__
#define __TEXTLAYOUT_H__

#include "AEETextLayout.h"

// classid of widgets-based implementation of TextLayout
#include "bid/AEECLSID_TEXTLAYOUT.BID"

int TextLayout_New(ITextLayout **ppo, IShell *piShell, IModule *piModule);

#endif   //__TEXTLAYOUT_H__
