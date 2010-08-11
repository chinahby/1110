/*
  ========================================================================

  FILE:           CustomContainer_priv.h

  SERVICES:       Private CustomContainer declarations and definitions.  
                  Used internally by the sample module only.

  PUBLIC CLASSES: Custom Container Class

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __CUSTOMCONTAINER_PRIV_H__
#define __CUSTOMCONTAINER_PRIV_H__

#include "CustomContainer.h"

int CustomContainer_New(ICustomContainer **ppo, IShell *pIShell, IModule *piModule);

#endif // __CUSTOMCONTAINER_PRIV_H__