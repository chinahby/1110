/*
================================================================================

  FILE:  OEMPhysicalDisplay_Priv.h
  
  SERVICES: Defines the PhysicalDisplay Private shared data

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMPhysicalDisplay_Priv.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
   
================================================================================
===============================================================================
*/
#ifndef __OEMPHYSICALDISPLAY_PRIV_H__
#define __OEMPHYSICALDISPLAY_PRIV_H__

#include "AEEPhysicalDisplay.h"

#ifdef FEATURE_MMOVERLAY
#include "AEE.h"
#include "AEE_OEMDispatch.h"
#include "AEEPhysicalDisplayOverlay.h"
#include "AEEMMOverlay.h"
#include "mdp_drv_api.h"
#include "OEMOverlayHelper.h"
#endif

#include "disp.h"
/*======================================================================= 
=======================================================================*/
#ifndef DISPLAY_TYPE_1
typedef unsigned int LCD_TYPE;
#define DISPLAY_TYPE_1	0      /* logical first display*/
#define DISPLAY_TYPE_2  1      /* logical second display */
#define DISPLAY_TYPE_3  2      /* logical third display */
#define DISPLAY_TYPE_4  3      /* logical fourth display */
#define MAX_LCD_NUM     4 
#endif

#ifdef FEATURE_MMOVERLAY
typedef struct IPhysicalDisplayModel {
   AEEBASE_INHERIT( IModel, IPhysicalDisplayOverlay );
} IPhysicalDisplayOverlayModel;

#define MAX_PENDING_UPDATES 10

typedef struct {
   boolean           bInUse;           //Is this entry in use
   IBitmap          *pBitmap;          //The bitmap for this update
   IPhysicalDisplayOverlay *pPhysicalDisplay; //The PhysicalDisplay pointer for this update
   AEECallback       cb;               //The Callback to switch back to the UI context
   IMMOverlay         *pOverlay;         //Pointer to the IMMOverlay that is being updated
   uint32            dwUpdateID;       //The Overlay Update ID.
   int               nErr;             //The Status of this update
#ifdef FEATURE_DISP_TASK
   MDP_CB            mdp_cb;           //The MDP Callback Structure
#endif
} PhysicalDisplayUpdateEntry;

struct IPhysicalDisplayOverlay
{
   AEEBASE_INHERIT( IPhysicalDisplayOverlay, IPhysicalDisplay );
   IPhysicalDisplayOverlayModel      theModel;         // Model for this interface
   OverlayHelper_Listener *   pListeners;      //The listeners on the model
   PhysicalDisplayUpdateEntry updateEntries[MAX_PENDING_UPDATES]; //The list of pending updates for this display
};

   extern const VTBL(IModel) gPhysicalDisplayOverlayModelFuncs;
   extern const VTBL(IPhysicalDisplayOverlay) gPhysicalDisplayOverlayFuncs;
#endif

//IPhysicalDisplay Data structure
struct IPhysicalDisplay
{
   const AEEVTBL(IPhysicalDisplay) *pvt;        // Function Table
   IShell*               pShell;                  // Pointer to the shell
   uint32                dwRefs;                  // Reference count for this object
   AEECLSID              clsID;
   boolean *             pbIsEnabled;
   AEECLSID *            pDispHandler;
#ifdef FEATURE_MMOVERLAY
   IPhysicalDisplayOverlay physicalDisplayOverlay;
#endif
};

extern AEECLSID gPhysicalDisp1_Handler;
extern AEECLSID gPhysicalDisp2_Handler;
extern AEECLSID gPhysicalDisp3_Handler;
extern AEECLSID gPhysicalDisp4_Handler;


#endif //__OEMPHYSICALDISPLAY_PRIV_H__
