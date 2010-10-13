/*======================================================

FILE:      OEMModTableExt.h

SERVICES:  Static extensions supplied by OEM

GENERAL DESCRIPTION:

  Declaration of ext lists of AEEStaticClasses and
    AEEStaticMods that come from OEMModTableExt.c

PUBLIC CLASSES AND STATIC FUNCTIONS:

    none

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef OEMMODTABLEEXT_H
#define OEMMODTABLEEXT_H /* #ifndef OEMMODTABLEEXT_H */

#include "AEEModTable.h"

extern const AEEStaticMod     *gOEMStaticModLists[];
extern const AEEStaticClass   *gOEMStaticClassLists[];

/*
//========================================================
//  Example implementation of OEMModTableExt.c
//
//   This file contains the list of static modules and static classes
//   that are specific to OEM platform. These get included as part of
//   the static tables defined in OEMModTable.c.
//
//========================================================

#include "OEMFeatures.h"
#include "OEMModTableExt.h"

extern int OEMApp1_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int OEMApp2_Load(IShell *ps, void * pHelpers, IModule ** pMod);

#include "OEMExtension1.bid"
#include "OEMExtension2.bid"

extern int OEMExtension1_New(IShell *ps, AEECLSID cls, void ** ppobj);
extern int OEMExtension2_New(IShell *ps, AEECLSID cls, void ** ppobj);

//========================================================
//   Statically linked application list.
//========================================================
static const AEEStaticMod gOEMStaticModList[] = 
{ 
   {AEEFS_MIF_DIR"oemapp1.mif", OEMApp1_Load},
   {AEEFS_MIF_DIR"oemapp2.mif", OEMApp2_Load},
   {NULL, NULL}
};

//========================================================
//   statically linked list of lists of AEEStaticApp
//========================================================
const AEEStaticMod *gOEMStaticModLists[] = { 
   gOEMStaticModList,
   NULL
};

//========================================================
//   Statically linked class list.
//========================================================
static const AEEStaticClass gOEMStaticClassList[] = {
   {AEECLSID_OEMEXTENSION1, ASCF_PRIV,0,NULL,OEMExtension1_New},
   {AEECLSID_OEMEXTENSION2, ASCF_PRIV,0,NULL,OEMExtension2_New},
   {0,0,0,NULL,NULL}
};

//========================================================
//   statically linked list of lists of AEEStaticClasses
//========================================================
const AEEStaticClass *gOEMStaticClassLists[] = { 
   gOEMStaticClassList,
   NULL
};

*/

#endif /* #ifndef OEMMODTABLEEXT_H */

