/*
  ========================================================================

  FILE:           CustomController_priv.h

  SERVICES:       Private CustomController declarations and definitions.  
                  Used internally by the sample module only.

  PUBLIC CLASSES: Custom Controller Class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CUSTOMCONTROLLER_PRIV_H__
#define __CUSTOMCONTROLLER_PRIV_H__

#include "CustomController.h"

int CustomController_New(IController **ppo, IShell *pIShell, IModule *piModule);

#endif // __CUSTOMCONTROLLER_PRIV_H__