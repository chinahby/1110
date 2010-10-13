/*=============================================================================
FILE:  OEMRats.h

SERVICES:  OEM functions for supporting BREW Remote API Test Suite

GENERAL DESCRIPTION:
      This file exports the functions used by the oemmodtable.c file.


        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/04    at     Initial release.
=============================================================================*/
#include "comdef.h"
#include "err.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEM.h"


#include "RATS.bid"

extern int OEMRats_New(IShell *pIShell, AEECLSID cls, void **ppif);
