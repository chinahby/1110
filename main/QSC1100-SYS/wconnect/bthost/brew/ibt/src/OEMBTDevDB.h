#ifndef _OEMBTDEVDB_H_
#define _OEMBTDEVDB_H_
/*=============================================================================
FILE:         OEMBTDEVDB.h

SERVICES:     IBTDEVDB Interfaces

DESCRIPTION:  The OEM implementation of IBTDEVDB

===============================================================================
        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBT.h"
#include "AEEBTCommon.h"
#include "AEEBTDevDB.h"

#define DEV_DB_INVALID_INDX  0x7FFFFFFF

/*============================================================================
 DevDB Handle that is used to identify an instance of a OEM object
============================================================================*/
typedef struct  OEMBTDEVDB OEMBTDEVDB;

typedef struct _OEMBTDEV_ITERATOR
{
   uint32            index; 
   BTDevDBEnumerator devEnum;
}OEMBTDEV_ITERATOR;

typedef uint32 BTHandle;
/*============================================================================
  OEM APIs exported to the AEE layer   
============================================================================*/
void OEMBTDevDB_Release( BTHandle hBT );
int OEMBTDEVDB_New( BTHandle* phBT );

BTResult OEMBTDevDB_Add( BTHandle hBT,
                         const BTDeviceInfo* pDeviceInfo,
                         IShell *pIShell);

BTResult OEMBTDevDB_Read( BTHandle hBT, BTDBEntry* pEntry);

BTResult OEMBTDevDB_Update( BTHandle hBT,
                            BTModDevField fields2Update,
                            const BTDBEntry* pEntry,
                            IShell* pIShell
                          );

BTResult OEMBTDevDB_EnumInit( BTHandle hBT,
                              const BTDevDBEnumerator* pEnumerator);

BTResult OEMBTDevDB_EnumNext( BTHandle hBT,
                              BTDBEntry* pEntry,
                              OEMBTDEV_ITERATOR* iter
                            );

int OEMBTDevDB_Bond( BTHandle hBT,
                     const BDAddress* pBDAddr,
                     const uint8* pszPin,
                     boolean bReplying,
                     BTResult* pResult,
                     AEECallback* pResultCb);
#endif
