#ifndef AEEMODTABLE_H
#define AEEMODTABLE_H
/*======================================================
FILE:  AEEModTable.h

SERVICES:  Static Module Table Definition

GENERAL DESCRIPTION:

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEShell.h"

//
// Internal Class Table - This table is used to support external
// objects and libraries that need to be called at initialization, to
// build class objects or both.  Examples would include such things
// as:
//
// - Clipboard Interfaces - Memory allocation at startup and creation
//   of clipboard interfaces
//
// - Serial Interfaces - Both initialization and creation.
//

#define ASCF_UPGRADE    (0x0001)    // If set, a downloaded module will take precedence
#define ASCF_DISABLED   (0x0002)    // If set, class is disabled by default (unused as yet)
#define ASCF_PRIV       (0x0004)    // If set, the class must be specified in the external classes section of MIF

typedef struct AEEStaticClass
{
   AEECLSID    cls;
   uint16      wFlags;
   uint16      wMinPriv;
   void        (*pfnInit)(IShell * piShell);
   int         (*pfnNew)(IShell *piShell,AEECLSID cls,void **ppif);
} AEEStaticClass;

//
// Module Table - The Static Module Table contains references to data
// structures that expose the association between persistent MIFs and
// the underlying supporting code.  This allows BREW to treat persistent
// applications the same way that dynamic applications are loaded.  
//

typedef struct _AEEStaticModule
{
   const char *   pszMIF;        // static MIF base file name - example: "appmgr.mif"
   PFNMODENTRY    pfnModEntry;   // static module entry-point
} AEEStaticMod;

#endif    // AEEMODTABLE_H

