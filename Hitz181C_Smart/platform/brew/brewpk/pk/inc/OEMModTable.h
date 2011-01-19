#ifndef OEMMODTABLE_H
#define OEMMODTABLE_H
/*======================================================
FILE:  AEEModTable.h

SERVICES:  Static Module Table Definition

GENERAL DESCRIPTION:

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

NOTE: OEMs should not modify this file to add new classes/modules. 
   This should be done through the creation or modification of OEMModTableExt.c. 
   For more information on the format of this file see OEMModTableExt.h. 
   OEMs who are using OEMModTableExt.c should verify that 
   FEATURE_BREW_EXTENDED_MODTABLE is defined in OEMFeatures.h
=====================================================*/

#include "AEEModTable.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern const AEEStaticMod    ***OEMMod_GetStaticModLists(void);
extern const AEEStaticClass  ***OEMMod_GetStaticClassLists(void);

#if defined(__cplusplus)
}
#endif

#endif    // OEMMODTABLE_H

