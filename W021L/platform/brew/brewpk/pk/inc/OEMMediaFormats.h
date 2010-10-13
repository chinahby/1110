/*
  ============================================================================

FILE:  OEMMediaFormats.h

SERVICES:  
       IMedia-based classes.

GENERAL DESCRIPTION:

REVISION HISTORY: 

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef OEMMEDIAFORMATS_H
#define OEMMEDIAFORMATS_H

#include "AEEModTable.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*===========================================================================

                      ModTable of IMedia Classes

===========================================================================*/
extern const AEEStaticClass gAEEMediaClasses[];

extern boolean CMediaFormat_InModTable(AEECLSID cls, void * pfnInit, void * pfnNew);

#endif /* #ifndef OEMMEDIAFORMATS_H*/
