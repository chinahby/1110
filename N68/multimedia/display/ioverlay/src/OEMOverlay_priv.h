/*
================================================================================

  FILE:  OEMOverlayHelper.h
  
  SERVICES: Defines the Overlay Helper interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlay_priv.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
    
================================================================================
===============================================================================
*/
#ifndef __OEMOVERLAY_PRIV_H__
#define __OEMOVERLAY_PRIV_H__

#include "AEEMMOverlay.h"
#include "AEEModTable.h"

extern const AEEStaticClass gOEMOverlayClasses[];

#ifdef FEATURE_MMOVERLAY
extern IMMOverlay * Overlay_Create(IShell * pShell);
#endif

/*==============================================================================
OBJECT CONSTRUCTORS
==============================================================================*/
#ifdef FEATURE_MMOVERLAY
extern int OverlayMgr_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
extern int SysMMOverlayRegistry_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
#endif

#ifdef FEATURE_OVERLAY2
extern int Overlay2Mgr_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
extern int DisplayCfg2_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
#endif

extern int LogicalDisplayCfg_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
extern int PhysicalDisplay_New(IShell *pShell, AEECLSID ClsId, void **ppObj);
extern void PhysicalDisplay_Init(IShell *pShell);
extern int OEMOverlay_New(IShell *pShell, AEECLSID ClsId, void **ppObj);


#endif //__OEMOVERLAY_PRIV_H__
