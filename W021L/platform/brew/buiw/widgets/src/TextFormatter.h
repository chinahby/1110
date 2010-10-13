/*
  ========================================================================

  FILE:          AEETextFormatter.h

  SERVICES:      String composition interface

  PUBLIC CLASSES:ITextFormatter

  DESCRIPTION:   This file describes an interface of language dependent
                 data formatting routines.  These can be used to 
                 compose strings of text according to a specific
                 set of culture specific formatting rules.
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __TEXTFORMATTER_H__
#define __TEXTFORMATTER_H__

#include "AEETextFormatter.h"

int TextFormatter_New(ITextFormatter **ppo, IShell *pIShell, IModule *piModule);

#endif // __TEXTFORMATTER_H__