#ifndef AEEBEARERTECHNOLOGY_H
#define AEEBEARERTECHNOLOGY_H
/*===========================================================================

FILE: AEEBearerTechnology.h

SERVICES: AEE Bearer Technology definitions

DESCRIPTION: Base level definitions, typedefs, etc. for AEE Bearer Technology

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "AEEWebOpts.h"
#include "../../inc/AEEIBearerTechnology.h"

#define AEEIID_BEARERTECHNOLOGY    AEEIID_IBearerTechnology

#define IBEARERTECHNOLOGY_AddRef          IBearerTechnology_AddRef
#define IBEARERTECHNOLOGY_Release         IBearerTechnology_Release
#define IBEARERTECHNOLOGY_QueryInterface  IBearerTechnology_QueryInterface
#define IBEARERTECHNOLOGY_AddOpt          IBearerTechnology_AddOpt
#define IBEARERTECHNOLOGY_RemoveOpt       IBearerTechnology_RemoveOpt
#define IBEARERTECHNOLOGY_GetOpt          IBearerTechnology_GetOpt

#endif  // AEEBEARERTECHNOLOGY_H
