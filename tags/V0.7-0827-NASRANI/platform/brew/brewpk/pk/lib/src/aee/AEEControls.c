/*=====================================================================
FILE:  AEEControls.c

SERVICES:  Entry Points for the different Controls supported in BREW

DESCRIPTION: This file contains the module entry points for the different controls defined in BREW

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2001 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

//=====================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=====================================================================


//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEETime_priv.h"
#include "AEEClassIDs.h"
#include "AEEDate_priv.h"
#include "AEEAClock_priv.h"
#include "AEEImageCtl_priv.h"
#include "AEEModTable.h"

#if defined(__cplusplus)
extern "C" {
#endif

const AEEStaticClass gControlClassList[] = 
{
   {AEECLSID_CLOCKCTL,        ASCF_UPGRADE,0,NULL,TimeCtl_New},
   {AEECLSID_STOPWATCHCTL,    ASCF_UPGRADE,0,NULL,TimeCtl_New},
   {AEECLSID_COUNTDOWNCTL,    ASCF_UPGRADE,0,NULL,TimeCtl_New},
   {AEECLSID_DATECTL,         ASCF_UPGRADE,0,NULL,DateCtl_New},
   {AEECLSID_DATEPICKCTL,     ASCF_UPGRADE,0,NULL,DateCtl_New},
   {AEECLSID_ACLOCKCTL,       ASCF_UPGRADE,0,NULL,AClockCtl_New},
   {AEECLSID_IMAGECTL,        ASCF_UPGRADE,0,NULL,ImageCtl_New},
   {0,0,0,NULL,NULL},
};

#if defined(__cplusplus)
}
#endif
