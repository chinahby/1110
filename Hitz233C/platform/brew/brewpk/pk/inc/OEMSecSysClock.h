/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSecSysClock.h

SERVICES:  Secure Clock Class inherited from ISysClock Interface

GENERAL DESCRIPTION:
   Definitions, typedefs, etc. for OEM APIs which provide access to secure 
   system clock 

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef __CSECSYSCLOCK_H__
#define __CSECSYSCLOCK_H__

#include "AEEComdef.h"

/*========================================================================
                         Type Definitions
========================================================================*/

typedef struct _LastValidCfg {
   uint64 qwTime;
   AEECLSID OwnerCls;
} LastValidCfg;


#endif /* __CSECSYSCLOCK_H__ */
