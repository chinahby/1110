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

//Start Dynamic ConstFile Externs
extern AEEConstFile gBREWAPPMGR_MIF;
extern AEEConstFile gBREWAPPMGRCOMMON_BAR;
extern AEEConstFile gBREWAPPMGRLS_BAR;
extern AEEConstFile gBREWAPPMGRLN_BAR;
extern AEEConstFile gOEMCFG_TXT;
extern AEEConstFile gBREWAPPMGRLS_BAR;
extern AEEConstFile gCOREAPP_MIF;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gTIANYION_PNG;
extern AEEConstFile gTIANYIOFF_PNG;
extern AEEConstFile gCHARGE_PNG;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gOEMMSGS_BAR;
extern AEEConstFile gAEECONTROLS_BAR;
extern AEEConstFile gVECTOR_MIF;
extern AEEConstFile gOEMANNUNCIATOR_BAR;
extern AEEConstFile gAPPSCOMMON_BAR;
extern AEEConstFile gAPPSCOMMON_BAR;
extern AEEConstFile gAPPSCOMMON_BAR;
extern AEEConstFile gAPPSCOMMON_COLOR_BAR;
extern AEEConstFile gSENDINGSMS_PNG;
extern AEEConstFile gVOLUME_PNG;
extern AEEConstFile gCALLIN_PNG;
extern AEEConstFile gCALLOUT_PNG;
extern AEEConstFile gCOREAPP_MIF;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gTIANYION_PNG;
extern AEEConstFile gTIANYIOFF_PNG;
extern AEEConstFile gCHARGE_PNG;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gCOREAPP_BAR;
extern AEEConstFile gMAINMENU_MIF;
extern AEEConstFile gMAINMENU_BAR;
extern AEEConstFile gMAINMENUIMAGES_BAR;
extern AEEConstFile gMAINMENU_BAR;
extern AEEConstFile gMAINMENU_BAR;
extern AEEConstFile gGAME1_PNG;
extern AEEConstFile gGALLERY2_PNG;
extern AEEConstFile gTOOLS3_PNG;
extern AEEConstFile gCAMERA4_PNG;
extern AEEConstFile gCONTACT5_PNG;
extern AEEConstFile gMESSAGE6_PNG;
extern AEEConstFile gRECENTCALL7_PNG;
extern AEEConstFile gSETTING8_PNG;
extern AEEConstFile gSPORTS9_PNG;
extern AEEConstFile gPROFILE10_PNG;
extern AEEConstFile gPLAYER11_PNG;
extern AEEConstFile gWAP12_PNG;
extern AEEConstFile gNUMBER10_PNG;
extern AEEConstFile gSURFING_FOCUS_PNG;
extern AEEConstFile gCUSTSERVICE_FOCUS_PNG;
extern AEEConstFile gCONTACT511_PNG;
extern AEEConstFile gCONTACT512_PNG;
extern AEEConstFile gCONTACT513_PNG;
extern AEEConstFile gMESSAGE607_PNG;
extern AEEConstFile gMESSAGE608_PNG;
extern AEEConstFile gMESSAGE609_PNG;
extern AEEConstFile gGAME1_1_PNG;
extern AEEConstFile gGALLERY2_1_PNG;
extern AEEConstFile gTOOLS3_1_PNG;
extern AEEConstFile gCAMERA4_1_PNG;
extern AEEConstFile gCONTACT5_1_PNG;
extern AEEConstFile gMESSAGE6_1_PNG;
extern AEEConstFile gRECENTCALL7_1_PNG;
extern AEEConstFile gSETTING8_1_PNG;
extern AEEConstFile gSPORTS9_1_PNG;
extern AEEConstFile gPROFILE10_1_PNG;
extern AEEConstFile gPLAYER11_1_PNG;
extern AEEConstFile gWAP12_1_PNG;
extern AEEConstFile gNUMBER10_1_PNG;
extern AEEConstFile gGAME1_2_PNG;
extern AEEConstFile gGALLERY2_2_PNG;
extern AEEConstFile gTOOLS3_2_PNG;
extern AEEConstFile gCAMERA4_2_PNG;
extern AEEConstFile gCONTACT5_2_PNG;
extern AEEConstFile gMESSAGE6_2_PNG;
extern AEEConstFile gRECENTCALL7_2_PNG;
extern AEEConstFile gSETTING8_2_PNG;
extern AEEConstFile gSPORTS9_2_PNG;
extern AEEConstFile gPROFILE10_2_PNG;
extern AEEConstFile gPLAYER11_2_PNG;
extern AEEConstFile gWAP12_2_PNG;
extern AEEConstFile gNUMBER10_2_PNG;
extern AEEConstFile gCALLAPP_MIF;
extern AEEConstFile gCALLAPP_BAR;
extern AEEConstFile gCALLAPP_BAR;
extern AEEConstFile gCALLAPP_BAR;
extern AEEConstFile gCONTACTAPP_MIF;
extern AEEConstFile gCONTACTAPP_BAR;
extern AEEConstFile gCONTACTAPP_BAR;
extern AEEConstFile gCONTACTAPP_BAR;
extern AEEConstFile gWMS_MIF;
extern AEEConstFile gWMS_BAR;
extern AEEConstFile gWMS_BAR;
extern AEEConstFile gWMS_BAR;
extern AEEConstFile gUTK_MIF;
extern AEEConstFile gUTK_BAR;
extern AEEConstFile gUTK_BAR;
extern AEEConstFile gUTK_BAR;
extern AEEConstFile gRECENTCALLS_MIF;
extern AEEConstFile gRECENTCALLS_BAR;
extern AEEConstFile gRECENTCALLS_BAR;
extern AEEConstFile gRECENTCALLS_BAR;
extern AEEConstFile gVIDEOPLAYER_MIF;
extern AEEConstFile gVIDEOPLAYER_BAR;
extern AEEConstFile gVIDEOPLAYER_IMAGES_BAR;
extern AEEConstFile gVIDEOPLAYER_BAR;
extern AEEConstFile gVIDEOPLAYER_BAR;
extern AEEConstFile gMUSICPLAYER_MIF;
extern AEEConstFile gMUSICPLAYER_BAR;
extern AEEConstFile gMUSICPLAYERIMAGE_BAR;
extern AEEConstFile gAWAY_MP3;
extern AEEConstFile gMUSICPLAYER_BAR;
extern AEEConstFile gMUSICPLAYER_BAR;
extern AEEConstFile gRECORDER_MIF;
extern AEEConstFile gRECORDER_BAR;
extern AEEConstFile gRECORDER_IMAGES_BAR;
extern AEEConstFile gRECORDER_BAR;
extern AEEConstFile gRECORDER_BAR;
extern AEEConstFile gMEDIAGALLERY_MIF;
extern AEEConstFile gMEDIAGALLERY_BAR;
extern AEEConstFile gMEDIAGALLERY_IMAGE_BAR;
extern AEEConstFile gMEDIAGALLERY_BAR;
extern AEEConstFile gMEDIAGALLERY_BAR;
extern AEEConstFile gSOUNDMENU_MIF;
extern AEEConstFile gSOUNDMENU_BAR;
extern AEEConstFile gBLUES_MID;
extern AEEConstFile gBLUE_LOAD_MID;
extern AEEConstFile gCHAMPAGNE_MID;
extern AEEConstFile gCLOCK_MID;
extern AEEConstFile gDANCE_CRAZE_MID;
extern AEEConstFile gGREEN_LINE_MID;
extern AEEConstFile gJUMPING_MID;
extern AEEConstFile gLUCKY_MID;
extern AEEConstFile gSTREET_TALE_MID;
extern AEEConstFile gTECHNO_METO_MID;
extern AEEConstFile gFEELING_ON_MID;
extern AEEConstFile gFEELING_OFF_MID;
extern AEEConstFile gSOUNDMENU_BAR;
extern AEEConstFile gSOUNDMENU_BAR;
extern AEEConstFile gDISPLAYMENU_MIF;
extern AEEConstFile gDISPLAYMENU_BAR;
extern AEEConstFile gWALLPAPER01_PNG;
extern AEEConstFile gWALLPAPER02_PNG;
extern AEEConstFile gWALLPAPER03_PNG;
extern AEEConstFile gWALLPAPER04_PNG;
extern AEEConstFile gWALLPAPER05_PNG;
extern AEEConstFile gWALLPAPER06_PNG;
extern AEEConstFile gDISPLAYMENU_BAR;
extern AEEConstFile gDISPLAYMENU_BAR;
extern AEEConstFile gSECURITYMENU_MIF;
extern AEEConstFile gSECURITYMENU_BAR;
extern AEEConstFile gSECURITYMENU_BAR;
extern AEEConstFile gSECURITYMENU_BAR;
extern AEEConstFile gSETTINGMENU_MIF;
extern AEEConstFile gSETTINGMENU_BAR;
extern AEEConstFile gSETTINGMENU_BAR;
extern AEEConstFile gSETTINGMENU_BAR;
extern AEEConstFile gADJUSTPENAPP_MIF;
extern AEEConstFile gADJUSTPENAPP_BAR;
extern AEEConstFile gADJUSTPENAPP_BAR;
extern AEEConstFile gADJUSTPENAPP_BAR;
extern AEEConstFile gCALCAPP_MIF;
extern AEEConstFile gCALCAPP_BAR;
extern AEEConstFile gCALCAPP_BAR;
extern AEEConstFile gCALCAPP_BAR;
extern AEEConstFile gCLOCKAPPS_MIF;
extern AEEConstFile gCLOCKAPPS_BAR;
extern AEEConstFile gCLOCKAPPS_IMAGES_BAR;
extern AEEConstFile gCLOCKAPPS_BAR;
extern AEEConstFile gCLOCKAPPS_BAR;
extern AEEConstFile gCONVERTER_MIF;
extern AEEConstFile gCONVERTER_BAR;
extern AEEConstFile gCONVERTER_BAR;
extern AEEConstFile gSCHEDULEAPP_MIF;
extern AEEConstFile gSCHEDULEAPP_BAR;
extern AEEConstFile gCALENDARAPP_BAR;
extern AEEConstFile gSCHEDULEAPP_BAR;
extern AEEConstFile gCALENDARAPP_BAR;
extern AEEConstFile gSCHEDULEAPP_BAR;
extern AEEConstFile gCALENDARAPP_BAR;
extern AEEConstFile gSTOPWATCH_MIF;
extern AEEConstFile gSTOPWATCH_BAR;
extern AEEConstFile gSTOPWATCH_BAR;
extern AEEConstFile gSTOPWATCH_BAR;
extern AEEConstFile gAPPTIMER_MIF;
extern AEEConstFile gAPPTIMERRES_BAR;
extern AEEConstFile gAPPTIMERRES_BAR;
extern AEEConstFile gWORLDTIME_MIF;
extern AEEConstFile gWORLDTIME_BAR;
extern AEEConstFile gWORLDTIME_IMAGES_BAR;
extern AEEConstFile gWORLDTIME_BAR;
extern AEEConstFile gWORLDTIME_BAR;
extern AEEConstFile gNOTEBOOK_MIF;
extern AEEConstFile gNOTEBOOK_BAR;
extern AEEConstFile gNOTEBOOK_BAR;
extern AEEConstFile gNOTEBOOK_BAR;
extern AEEConstFile gTETRIS_MIF;
extern AEEConstFile gTETRIS_BAR;
extern AEEConstFile gTETRISIMAGES_BAR;
extern AEEConstFile gBLINK_MID;
extern AEEConstFile gDROP_MID;
extern AEEConstFile gDROPPIECE_MID;
extern AEEConstFile gFAIL_MID;
extern AEEConstFile gHERO_MID;
extern AEEConstFile gMULLINES_MID;
extern AEEConstFile gOVER_MID;
extern AEEConstFile gSELECT_MID;
extern AEEConstFile gSINGLELINE_MID;
extern AEEConstFile gUNDROP_MID;
extern AEEConstFile gTETRIS_BAR;
extern AEEConstFile gTETRIS_BAR;
extern AEEConstFile gEXTRAMENU_MIF;
extern AEEConstFile gEXTRAMENU_BAR;
extern AEEConstFile gEXTRAMENU_BAR;
extern AEEConstFile gEXTRAMENU_BAR;
extern AEEConstFile gOEMEXTFOR3PART_MIF;
extern AEEConstFile gEDITAPPLET_MIF;
extern AEEConstFile gEDITEXTENSION_MIF;
extern AEEConstFile gBROWSERWIDGET_MIF;
extern AEEConstFile gBRW_APP_MIF;
extern AEEConstFile gDRM_MIF;
extern AEEConstFile gSEC_MIF;
extern AEEConstFile gWECSC_MIF;
extern AEEConstFile gWAP_MIF;
extern AEEConstFile gPUSHE_MIF;
extern AEEConstFile gPICTOGRAMCORESET_BAR;
extern AEEConstFile gIMG_BAR;
extern AEEConstFile gTHEME_BAR;
extern AEEConstFile gZHCN_EN_STRING_BAR;
extern AEEConstFile gZHCN_ZHCN_STRING_BAR;
extern AEEConstFile gZHCN_IMAGE_BAR;
extern AEEConstFile gREGBRW_DAT;
extern AEEConstFile gBIG5TOUCS_DAT;
extern AEEConstFile gGB2UCS_DAT;
extern AEEConstFile gUCS2BIG5_DAT;
extern AEEConstFile gUCS2GB_DAT;
extern AEEConstFile gBREWAPPMGR_MIF;
extern AEEConstFile gBREWAPPMGRCOMMON_BAR;
extern AEEConstFile gBREWAPPMGRLS_BAR;
extern AEEConstFile gBREWAPPMGRLN_BAR;
extern AEEConstFile gOEMCFG_TXT;
extern AEEConstFile gBREWAPPMGRLS_BAR;
extern AEEConstFile gESURFING_MIF;
extern AEEConstFile gESURFING_BAR;
extern AEEConstFile gESURFING_BAR;
extern AEEConstFile gESURFING_BAR;
extern AEEConstFile gCUSTSERVICE_MIF;
extern AEEConstFile gCUSTSERVICE_BAR;
extern AEEConstFile gCUSTSERVICE_BAR;
extern AEEConstFile gCUSTSERVICE_BAR;
extern AEEConstFile gQUICKTEST_MIF;
extern AEEConstFile gQUICKTEST_BAR;
extern AEEConstFile gQUICKTEST_BAR;
extern AEEConstFile gQUICKTEST_BAR;
extern AEEConstFile gSVCPRG_MIF;
extern AEEConstFile gSVCPRG_BAR;
extern AEEConstFile gSVCPRG_BAR;
extern AEEConstFile gSVCPRG_BAR;
extern AEEConstFile gNUMBERMANAGER_MIF;
extern AEEConstFile gNUMBERMANAGER_BAR;
extern AEEConstFile gNUMBERMANAGER_BAR;
extern AEEConstFile gNUMBERMANAGER_BAR;
//End Dynamic ConstFile Externs

static const AEEConstFile *  gpOEMConstFiles[] = {

//Start Dynamic ConstFile Symbols
      &gBREWAPPMGR_MIF,
      &gBREWAPPMGRCOMMON_BAR,
      &gBREWAPPMGRLS_BAR,
      &gBREWAPPMGRLN_BAR,
      &gOEMCFG_TXT,
      &gBREWAPPMGRLS_BAR,
      &gCOREAPP_MIF,
      &gCOREAPP_BAR,
      &gTIANYION_PNG,
      &gTIANYIOFF_PNG,
      &gCHARGE_PNG,
      &gCOREAPP_BAR,
      &gCOREAPP_BAR,
      &gOEMMSGS_BAR,
      &gAEECONTROLS_BAR,
      &gVECTOR_MIF,
      &gOEMANNUNCIATOR_BAR,
      &gAPPSCOMMON_BAR,
      &gAPPSCOMMON_BAR,
      &gAPPSCOMMON_BAR,
      &gAPPSCOMMON_COLOR_BAR,
      &gSENDINGSMS_PNG,
      &gVOLUME_PNG,
      &gCALLIN_PNG,
      &gCALLOUT_PNG,
      &gCOREAPP_MIF,
      &gCOREAPP_BAR,
      &gTIANYION_PNG,
      &gTIANYIOFF_PNG,
      &gCHARGE_PNG,
      &gCOREAPP_BAR,
      &gCOREAPP_BAR,
      &gMAINMENU_MIF,
      &gMAINMENU_BAR,
      &gMAINMENUIMAGES_BAR,
      &gMAINMENU_BAR,
      &gMAINMENU_BAR,
      &gGAME1_PNG,
      &gGALLERY2_PNG,
      &gTOOLS3_PNG,
      &gCAMERA4_PNG,
      &gCONTACT5_PNG,
      &gMESSAGE6_PNG,
      &gRECENTCALL7_PNG,
      &gSETTING8_PNG,
      &gSPORTS9_PNG,
      &gPROFILE10_PNG,
      &gPLAYER11_PNG,
      &gWAP12_PNG,
      &gNUMBER10_PNG,
      &gSURFING_FOCUS_PNG,
      &gCUSTSERVICE_FOCUS_PNG,
      &gCONTACT511_PNG,
      &gCONTACT512_PNG,
      &gCONTACT513_PNG,
      &gMESSAGE607_PNG,
      &gMESSAGE608_PNG,
      &gMESSAGE609_PNG,
      &gGAME1_1_PNG,
      &gGALLERY2_1_PNG,
      &gTOOLS3_1_PNG,
      &gCAMERA4_1_PNG,
      &gCONTACT5_1_PNG,
      &gMESSAGE6_1_PNG,
      &gRECENTCALL7_1_PNG,
      &gSETTING8_1_PNG,
      &gSPORTS9_1_PNG,
      &gPROFILE10_1_PNG,
      &gPLAYER11_1_PNG,
      &gWAP12_1_PNG,
      &gNUMBER10_1_PNG,
      &gGAME1_2_PNG,
      &gGALLERY2_2_PNG,
      &gTOOLS3_2_PNG,
      &gCAMERA4_2_PNG,
      &gCONTACT5_2_PNG,
      &gMESSAGE6_2_PNG,
      &gRECENTCALL7_2_PNG,
      &gSETTING8_2_PNG,
      &gSPORTS9_2_PNG,
      &gPROFILE10_2_PNG,
      &gPLAYER11_2_PNG,
      &gWAP12_2_PNG,
      &gNUMBER10_2_PNG,
      &gCALLAPP_MIF,
      &gCALLAPP_BAR,
      &gCALLAPP_BAR,
      &gCALLAPP_BAR,
      &gCONTACTAPP_MIF,
      &gCONTACTAPP_BAR,
      &gCONTACTAPP_BAR,
      &gCONTACTAPP_BAR,
      &gWMS_MIF,
      &gWMS_BAR,
      &gWMS_BAR,
      &gWMS_BAR,
      &gUTK_MIF,
      &gUTK_BAR,
      &gUTK_BAR,
      &gUTK_BAR,
      &gRECENTCALLS_MIF,
      &gRECENTCALLS_BAR,
      &gRECENTCALLS_BAR,
      &gRECENTCALLS_BAR,
      &gVIDEOPLAYER_MIF,
      &gVIDEOPLAYER_BAR,
      &gVIDEOPLAYER_IMAGES_BAR,
      &gVIDEOPLAYER_BAR,
      &gVIDEOPLAYER_BAR,
      &gMUSICPLAYER_MIF,
      &gMUSICPLAYER_BAR,
      &gMUSICPLAYERIMAGE_BAR,
      &gAWAY_MP3,
      &gMUSICPLAYER_BAR,
      &gMUSICPLAYER_BAR,
      &gRECORDER_MIF,
      &gRECORDER_BAR,
      &gRECORDER_IMAGES_BAR,
      &gRECORDER_BAR,
      &gRECORDER_BAR,
      &gMEDIAGALLERY_MIF,
      &gMEDIAGALLERY_BAR,
      &gMEDIAGALLERY_IMAGE_BAR,
      &gMEDIAGALLERY_BAR,
      &gMEDIAGALLERY_BAR,
      &gSOUNDMENU_MIF,
      &gSOUNDMENU_BAR,
      &gBLUES_MID,
      &gBLUE_LOAD_MID,
      &gCHAMPAGNE_MID,
      &gCLOCK_MID,
      &gDANCE_CRAZE_MID,
      &gGREEN_LINE_MID,
      &gJUMPING_MID,
      &gLUCKY_MID,
      &gSTREET_TALE_MID,
      &gTECHNO_METO_MID,
      &gFEELING_ON_MID,
      &gFEELING_OFF_MID,
      &gSOUNDMENU_BAR,
      &gSOUNDMENU_BAR,
      &gDISPLAYMENU_MIF,
      &gDISPLAYMENU_BAR,
      &gWALLPAPER01_PNG,
      &gWALLPAPER02_PNG,
      &gWALLPAPER03_PNG,
      &gWALLPAPER04_PNG,
      &gWALLPAPER05_PNG,
      &gWALLPAPER06_PNG,
      &gDISPLAYMENU_BAR,
      &gDISPLAYMENU_BAR,
      &gSECURITYMENU_MIF,
      &gSECURITYMENU_BAR,
      &gSECURITYMENU_BAR,
      &gSECURITYMENU_BAR,
      &gSETTINGMENU_MIF,
      &gSETTINGMENU_BAR,
      &gSETTINGMENU_BAR,
      &gSETTINGMENU_BAR,
      &gADJUSTPENAPP_MIF,
      &gADJUSTPENAPP_BAR,
      &gADJUSTPENAPP_BAR,
      &gADJUSTPENAPP_BAR,
      &gCALCAPP_MIF,
      &gCALCAPP_BAR,
      &gCALCAPP_BAR,
      &gCALCAPP_BAR,
      &gCLOCKAPPS_MIF,
      &gCLOCKAPPS_BAR,
      &gCLOCKAPPS_IMAGES_BAR,
      &gCLOCKAPPS_BAR,
      &gCLOCKAPPS_BAR,
      &gCONVERTER_MIF,
      &gCONVERTER_BAR,
      &gCONVERTER_BAR,
      &gSCHEDULEAPP_MIF,
      &gSCHEDULEAPP_BAR,
      &gCALENDARAPP_BAR,
      &gSCHEDULEAPP_BAR,
      &gCALENDARAPP_BAR,
      &gSCHEDULEAPP_BAR,
      &gCALENDARAPP_BAR,
      &gSTOPWATCH_MIF,
      &gSTOPWATCH_BAR,
      &gSTOPWATCH_BAR,
      &gSTOPWATCH_BAR,
      &gAPPTIMER_MIF,
      &gAPPTIMERRES_BAR,
      &gAPPTIMERRES_BAR,
      &gWORLDTIME_MIF,
      &gWORLDTIME_BAR,
      &gWORLDTIME_IMAGES_BAR,
      &gWORLDTIME_BAR,
      &gWORLDTIME_BAR,
      &gNOTEBOOK_MIF,
      &gNOTEBOOK_BAR,
      &gNOTEBOOK_BAR,
      &gNOTEBOOK_BAR,
      &gTETRIS_MIF,
      &gTETRIS_BAR,
      &gTETRISIMAGES_BAR,
      &gBLINK_MID,
      &gDROP_MID,
      &gDROPPIECE_MID,
      &gFAIL_MID,
      &gHERO_MID,
      &gMULLINES_MID,
      &gOVER_MID,
      &gSELECT_MID,
      &gSINGLELINE_MID,
      &gUNDROP_MID,
      &gTETRIS_BAR,
      &gTETRIS_BAR,
      &gEXTRAMENU_MIF,
      &gEXTRAMENU_BAR,
      &gEXTRAMENU_BAR,
      &gEXTRAMENU_BAR,
      &gOEMEXTFOR3PART_MIF,
      &gEDITAPPLET_MIF,
      &gEDITEXTENSION_MIF,
      &gBROWSERWIDGET_MIF,
      &gBRW_APP_MIF,
      &gDRM_MIF,
      &gSEC_MIF,
      &gWECSC_MIF,
      &gWAP_MIF,
      &gPUSHE_MIF,
      &gPICTOGRAMCORESET_BAR,
      &gIMG_BAR,
      &gTHEME_BAR,
      &gZHCN_EN_STRING_BAR,
      &gZHCN_ZHCN_STRING_BAR,
      &gZHCN_IMAGE_BAR,
      &gREGBRW_DAT,
      &gBIG5TOUCS_DAT,
      &gGB2UCS_DAT,
      &gUCS2BIG5_DAT,
      &gUCS2GB_DAT,
      &gBREWAPPMGR_MIF,
      &gBREWAPPMGRCOMMON_BAR,
      &gBREWAPPMGRLS_BAR,
      &gBREWAPPMGRLN_BAR,
      &gOEMCFG_TXT,
      &gBREWAPPMGRLS_BAR,
      &gESURFING_MIF,
      &gESURFING_BAR,
      &gESURFING_BAR,
      &gESURFING_BAR,
      &gCUSTSERVICE_MIF,
      &gCUSTSERVICE_BAR,
      &gCUSTSERVICE_BAR,
      &gCUSTSERVICE_BAR,
      &gQUICKTEST_MIF,
      &gQUICKTEST_BAR,
      &gQUICKTEST_BAR,
      &gQUICKTEST_BAR,
      &gSVCPRG_MIF,
      &gSVCPRG_BAR,
      &gSVCPRG_BAR,
      &gSVCPRG_BAR,
      &gNUMBERMANAGER_MIF,
      &gNUMBERMANAGER_BAR,
      &gNUMBERMANAGER_BAR,
      &gNUMBERMANAGER_BAR,
//End Dynamic ConstFile Symbols

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
