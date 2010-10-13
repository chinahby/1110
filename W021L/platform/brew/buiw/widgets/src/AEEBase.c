/*======================================================
FILE:  AEEBase.c

SERVICES:  AEEBase member functions

GENERAL DESCRIPTION:

   Class for supporting multiple inheritance from abstract base classes.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEBase.h"

uint32 AEEBase_AddRef(AEEBase *po)
{
   return IQI_AddRef(po->pMe);
}


uint32 AEEBase_Release(AEEBase *po)
{
   return IQI_Release(po->pMe);
}

int AEEBase_QueryInterface(AEEBase *po, AEECLSID id, void **ppNew)
{
   return IQI_QueryInterface(po->pMe, id, ppNew);
}

