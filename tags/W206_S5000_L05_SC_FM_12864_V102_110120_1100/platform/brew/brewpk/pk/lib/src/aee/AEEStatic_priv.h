/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEStatic_Priv.h

SERVICES:  Data Structure and function definitions for IStatic Interface

GENERAL DESCRIPTION:  Provides support for IStatic
interfaces in AEE.

PUBLIC CLASSES AND STATIC FUNCTIONS: IStatic

Copyright ? 1999-2002 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

#ifndef AEESTATIC_PRIV_H
#define AEESTATIC_PRIV_H /* #ifndef AEESTATIC_PRIV_H */

#include "AEE_OEM.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef struct _AStatic          AStatic;

extern int     Static_New(IShell * pShell, AEECLSID cls,void **ppif) ;
extern uint32 ParagraphAlignment(AECHAR *pszText, int nChars);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef AEESTATIC_PRIV_H */
