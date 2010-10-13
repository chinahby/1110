
/*===========================================================================

FILE: xmodstub.h
 
SERVICES:  
    
   Not Applicable

 
DESCRIPTION: 

   Not Applicable


PUBLIC CLASSES:
   
   Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  
   
   Not Applicable


        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef XMOD_H
#define XMOD_H /* #ifndef XMOD.H */

#include "AEE.h"


/**
  || Function
  || --------
  || int xModule_CreateInstance(IModule *p, IShell *piShell, 
  ||                            AEECLSID idClass, void **pp);
  || 
  || Description
  || -----------
  || xModule create instance function (used in xModule's vtable)
  ||  to be implemented by the client xmodstub
  ||
  || Parameters
  || ----------
  || IModule *p: the IModule itself, please copy and AddRef() in your object 
  ||             constructor (if successful).  Failure to do so will result 
  ||             in the module being unloaded by BREW
  || IShell *piShell: the IShell pointer, everybody needs one ;)
  || AEECLSID idClass: the class id about to be instantiated
  || void **pp: where to put the object, once created
  ||
  || Returns
  || -------
  || SUCCESS on success, some interesting error value otherwise
  ||
  || Remarks
  || -------
  || see AEE.h for more info
  ||
  */
typedef int (*PFNCREATEINSTANCE)(IModule *p, IShell *piShell, 
                                 AEECLSID idClass, void **pp);


int xModule_New(IShell *piShell, IModule **pp, PFNCREATEINSTANCE pfnCreateInstance);

#endif /* #ifndef XMOD_H */
