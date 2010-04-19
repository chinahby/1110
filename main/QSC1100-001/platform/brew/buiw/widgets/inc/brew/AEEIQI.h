#if !defined(AEEIQIPATCH_H)
#define      AEEIQIPATCH_H

/*======================================================
FILE: AEEIQI.h

GENERAL DESCRIPTION:
       A1-Lite include file allowing usage of BREW 4.0 style
        IQI declarations with prior versions of BREW

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEQueryInterface.h"
#if !defined(AEEIQI_H)
#define AEEIID      uint32
#define AEEIID_IQI  AEECLSID_QUERYINTERFACE
#define INHERIT_IQI INHERIT_IQueryInterface
#define IQI IQueryInterface
#define IQIVtbl IQueryInterfaceVtbl
#endif /* !defined(AEEIQI_H) */

#endif /* !defined(AEEIQIPATCH_H) */
