#if !defined(OEMBATTERY_H)
#define OEMBATTERY_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMBattery.h

SERVICES:  AEE OEM Battery Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
Battery.

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEE.h"
#include "AEEBattery.h"

#define FARF_BATTERY              0

#ifdef FARF
#undef FARF
#endif // FARF

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

typedef struct _OEMBatteryMgr OEMBatteryMgr;

#ifdef __cplusplus
extern "C" {
#endif

int OEMBattery_New(OEMBatteryMgr **ppme);
uint32 OEMBattery_AddRef(OEMBatteryMgr *pme);
uint32 OEMBattery_Release(OEMBatteryMgr *pme);
int OEMBattery_GetBatteryItem(OEMBatteryMgr *pme, uint32 dwItemID, uint32 *pdwSata);
void OEMBattery_OnBatteryItemUpdate(OEMBatteryMgr *pme, uint32 dwItemID, AEECallback *pcb);
void OEMBattery_Refresh(void);

#ifdef __cplusplus
}
#endif

#endif // OEMBATTERY_H

