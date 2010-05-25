/*======================================================

FILE:      AEESecurity_static.h

SERVICES:  ISecurity's static class entries

GENERAL DESCRIPTION:

  declaration of a StaticClass table, for inclusion 
    in OEMModTable.c

PUBLIC CLASSES AND STATIC FUNCTIONS:

    gakscIHashCTX

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEESECURITY_STATIC_H
#define AEESECURITY_STATIC_H

#include "AEEModTable.h"

extern const AEEStaticClass gakscIHashCTX[];
extern const AEEStaticClass gakscIHash[];
extern const AEEStaticClass gakscICipher[];

#endif /* #ifndef AEESECURITY_STATIC_H */

