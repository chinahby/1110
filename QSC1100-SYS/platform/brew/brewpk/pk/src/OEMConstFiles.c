/*=============================================================================

FILE: OEMConstFiles.c

SERVICES: OEM exported Const file declarations

GENERAL DESCRIPTION:

Contains the data structures populated by the OEM with the list of Const
files.  These files consist of data structures that are accessed by OEMFS.c
as virtual files.  This allows the OEM to reduce RAM overhead by placing
Const files (AEEControls.bar, etc.) into virtual const data that
consumes code space but not RAM.
  
PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMFS_GetConstFileList

INITIALIZATION AND SEQUENCING REQUIREMENTS:

OEMs must add references below

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
//
// New Const file entries must be declared and added below.
//

#include "OEMFeatures.h"
#include "OEMFS.h"

#if !defined(_AEEWIN)
extern AEEConstFile gBREWAPPMGR_MIF;

#if defined(AEE_SIMULATOR) // temporary
extern AEEConstFile gAPPMGR_BAR;
extern AEEConstFile gOEMMSGS_BAR;
extern AEEConstFile gAEECONTROLS_BAR;
extern AEEConstFile gEMUIDLEAPP_MIF;
#endif
#endif

#if defined(FEATURE_BREW_SSL) && !defined(AEE_SIMULATOR)
extern AEEConstFile gAEESSL_MIF;
extern AEEConstFile gAEESSL_MOD;
#endif
#ifndef FEATURE_MANGO_BREW
#define FEATURE_CODESIG
#endif
#if defined(FEATURE_CODESIG) && !defined(AEE_SIMULATOR)
extern AEEConstFile gCODESIG_MIF;
extern AEEConstFile gCODESIG_MOD;
#endif

#if defined(FEATURE_DEFAULT_VIEW_APP)
extern AEEConstFile gVIEWAPP_MIF;
#endif

//entries for pdiparsers & vCalStore
#if !defined(AEE_SIMULATOR)
#ifndef FEATURE_MANGO_BREW
extern AEEConstFile gAEEVCALSTORE_MIF;
extern AEEConstFile gAEEVCALSTORE_MOD;
extern AEEConstFile gAEEPDIPARSERS_MIF;
extern AEEConstFile gAEEPDIPARSERS_MOD;
#endif
#endif

// Const MIF files for MCF
#ifndef FEATURE_MANGO_BREW
extern AEEConstFile g10888_MIF;
extern AEEConstFile g10889_MIF;
extern AEEConstFile g10890_MIF;
extern AEEConstFile g10891_MIF;
extern AEEConstFile g10892_MIF;
extern AEEConstFile g16225_MIF;
extern AEEConstFile g18067_MIF;
extern AEEConstFile g19240_MIF;
extern AEEConstFile g19917_MIF;
extern AEEConstFile g19918_MIF;
extern AEEConstFile g19919_MIF;
extern AEEConstFile g19920_MIF;
extern AEEConstFile g19922_MIF;
extern AEEConstFile g19923_MIF;
extern AEEConstFile g19924_MIF;
#endif

#if defined(FEATURE_APP_QTCAM)
extern AEEConstFile gQTCAM_MIF;
#endif

// const MIF files for USB Host Mass Storage
#ifndef FEATURE_MANGO_BREW
extern AEEConstFile g26108_MIF;
extern AEEConstFile g26109_MIF;
#endif
#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER) || defined(FEATURE_ODM_UI)
extern const AEEConstFile *  gpODMAppsConstFiles[];
#endif

static const AEEConstFile *  gpOEMConstFiles[] = {
#if   !defined(_AEEWIN)   
#ifndef FEATURE_MANGO_BREW
                                                            &gBREWAPPMGR_MIF,
#endif
#if defined(AEE_SIMULATOR)
                                                            &gAPPMGR_BAR,
                                                            &gOEMMSGS_BAR,
                                                            &gAEECONTROLS_BAR,
                                                            &gEMUIDLEAPP_MIF,
#endif
#if defined(FEATURE_BREW_SSL) && !defined(AEE_SIMULATOR)
                                                            &gAEESSL_MIF,
                                                            &gAEESSL_MOD,
#endif
#if defined(FEATURE_CODESIG) && !defined(AEE_SIMULATOR)
                                                            &gCODESIG_MIF,
                                                            &gCODESIG_MOD,
#endif
                                                            // Const MIF files for MCF
#ifndef FEATURE_MANGO_BREW
                                                            &g10888_MIF,
                                                            &g10889_MIF,
                                                            &g10890_MIF,
                                                            &g10891_MIF,
                                                            &g10892_MIF,
                                                            &g16225_MIF,
                                                            &g18067_MIF,
                                                            &g19240_MIF,
                                                            &g19917_MIF,
                                                            &g19918_MIF,
                                                            &g19919_MIF,
                                                            &g19920_MIF,
                                                            &g19922_MIF,
                                                            &g19923_MIF,
                                                            &g19924_MIF,
#endif

#if defined(FEATURE_DEFAULT_VIEW_APP)
                                                            &gVIEWAPP_MIF,
#endif
#if defined(FEATURE_APP_QTCAM)
                                                            &gQTCAM_MIF,
#endif
#ifndef FEATURE_MANGO_BREW                                                            // Const MIF files for USB Host Mass Storage
                                                            &g26108_MIF,
                                                            &g26109_MIF,
#if !defined(AEE_SIMULATOR)
                                                            &gAEEVCALSTORE_MIF,
                                                            &gAEEVCALSTORE_MOD,
                                                            &gAEEPDIPARSERS_MIF,
                                                            &gAEEPDIPARSERS_MOD,                                                            
#endif
#endif      
                                                            
#endif
                                                            NULL};


static const AEEConstFile ** gpConstFiles[]  = {
  gpOEMConstFiles,
#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER) || defined (FEATURE_ODM_UI)
  gpODMAppsConstFiles,
#endif
  NULL};

//
// Function called by OEMFS to retrieve the Const file list.
//

const AEEConstFile *** OEMFS_GetConstFileList(void)
{
   return(gpConstFiles);
}
