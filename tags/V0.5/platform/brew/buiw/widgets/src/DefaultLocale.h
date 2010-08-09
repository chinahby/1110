/*
  ========================================================================

  FILE:          DefaultLocale.h

  SERVICES:      Locale specific string and text formatting utilities.

  LANGUAGE:      US English.  Widgets falls back on this locale if
                 now other ILocale objects are available to format text.

  PUBLIC CLASSES:ILocale

  DESCRIPTION:   This file describes an interface that provides language
                 dependent string formatting for a specific language. The
                 intent is that there be one of these objects for each
                 language your device supports.  Addition implementations
                 may be added later to support additional languages.
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __DEFAULTLOCALE_H__
#define __DEFAULTLOCALE_H__

#include "AEELocale.h"

int DefaultLocale_New(ILocale **ppo, IShell *pIShell, IModule *piModule);

#endif // __DEFAULTLOCALE_H__