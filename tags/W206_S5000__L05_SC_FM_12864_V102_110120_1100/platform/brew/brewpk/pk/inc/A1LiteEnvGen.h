#ifndef __A1LITEENVGEN_H__
#define __A1LITEENVGEN_H__
/*===========================================================================

FILE: A1LiteEnv.h

DESCRIPTION
  This file contains code for implementation of the Env interface in BREW 3.x.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


          Copyright © 1999-2007 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
  ========================================================================== */

#include "AEEIEnv.h"

void *A1LiteEnv_Alloc(void);
void A1LiteEnv_Free(void *);
IEnv *A1LiteEnv_New(void *, IShell *, IModule *);
int A1LiteEnv_EnvFromShell(IShell *, IEnv **);
int A1LiteEnv_CIFromShell(int (*)(IEnv *, AEECLSID, void **),
                          IShell *, AEECLSID, void **);

#endif /* __A1LITEENVGEN_H__ */
