/*
================================================================================

FILE:  OEMOverlaySuiteModTable.c

SERVICES: Provides overlay Mod Table related services.


================================================================================

Copyright © 1999-2005 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlaySuiteModTable.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
*/
#include "OEMFeatures.h"

#ifdef FEATURE_OVERLAY

//ModTable
#ifdef FEATURE_MMOVERLAY
#include "AEECLSID_MMOVERLAYMGR.BID"
#include "AEECLSID_SysMMOverlayRegistry.bid"
#endif

#ifdef FEATURE_OVERLAY2
#include "OVERLAY2.BID"
#include "OVERLAY2MGR.BID"
#include "DISPLAYCFG2.BID"
#endif


#include "AEECLSID_PHYSICALDISPLAY.BID"
#include "Overlay.bid"
#include "AEECLSID_LOGICALDISPLAYCFG.BID"
#include "AEEModTable.h"

#include "OEMOverlay_priv.h"

const AEEStaticClass gOEMOverlayClasses[] = {
#ifdef FEATURE_MMOVERLAY
   {AEECLSID_MMOVERLAYMGR,        (ASCF_UPGRADE), 0, NULL, OverlayMgr_New},             
   {AEECLSID_SysMMOverlayRegistry,(ASCF_UPGRADE), 0, NULL, SysMMOverlayRegistry_New}, 
#endif
#ifdef FEATURE_OVERLAY2
   {AEECLSID_OVERLAY2MGR,        (ASCF_UPGRADE), 0, NULL, Overlay2Mgr_New},             
   {AEECLSID_DISPLAYCFG1,      (ASCF_UPGRADE), 0, NULL, DisplayCfg2_New},
   {AEECLSID_DISPLAYCFG1,      (ASCF_UPGRADE), 0, NULL, DisplayCfg2_New},
   {AEECLSID_DISPLAYCFG1,      (ASCF_UPGRADE), 0, NULL, DisplayCfg2_New},
   {AEECLSID_DISPLAYCFG1,      (ASCF_UPGRADE), 0, NULL, DisplayCfg2_New},
#endif
   {AEECLSID_LOGICALDISPLAYCFG,   (ASCF_UPGRADE), 0, NULL, LogicalDisplayCfg_New},
   {AEECLSID_PHYSICALDISPLAY1,    (ASCF_UPGRADE), 0, PhysicalDisplay_Init, PhysicalDisplay_New},
   {AEECLSID_PHYSICALDISPLAY2,    (ASCF_UPGRADE), 0, NULL, PhysicalDisplay_New},
   {AEECLSID_PHYSICALDISPLAY3,    (ASCF_UPGRADE), 0, NULL, PhysicalDisplay_New},
   {AEECLSID_PHYSICALDISPLAY4,    (ASCF_UPGRADE), 0, NULL, PhysicalDisplay_New},
   {AEECLSID_OVERLAY1,     ASCF_UPGRADE, 0,  0, OEMOverlay_New},
   {AEECLSID_OVERLAY2,     ASCF_UPGRADE, 0,  0, OEMOverlay_New},
   {AEECLSID_OVERLAY3,     ASCF_UPGRADE, 0,  0, OEMOverlay_New},
   {AEECLSID_OVERLAY4,     ASCF_UPGRADE, 0,  0, OEMOverlay_New},               
   {0,0,0,NULL,NULL}
};

#endif /* FEATURE_OVERLAY */

