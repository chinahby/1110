/*
  ========================================================================

  FILE:           CustomModel_priv.h

  SERVICES:       rivate CustomModel declarations and definitions.  
                  Used internally by the sample module only.

  PUBLIC CLASSES: Custom Model Class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CUSTOMMODEL_PRIV_H__
#define __CUSTOMMODEL_PRIV_H__

#include "CustomModel.h"

int CustomModel_New(ICustomModel **ppo, IShell *pIShell, IModule *piModule);

#endif // __CUSTOMMODEL_PRIV_H__