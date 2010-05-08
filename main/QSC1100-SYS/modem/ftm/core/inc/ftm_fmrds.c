/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                F T M   F M R D S   A P I   E X T E N S I O N 

GENERAL DESCRIPTION
  This is the FTM file which contains fmrds api specific commands

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/fmradio/fmrds/dev/silab_ftm_test/ftm_fmrds.c#4 $$DateTime: 2008/10/01 10:53:09 $$Author: abhis $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
2008-07-25   as      Added support for seekstation()
2008-07-07   ap      Added support for Raw RDS, example service available
                     & AF support APIs
2008-07-07   as      Updated with build fixes
2008-06-27   ap      Initial Revision
===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#include "snd.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmdiag.h"
#ifdef FEATURE_FTM_FMRDS
#include "ftm_fmrds.h"

#ifdef FEATURE_FM
#include "fmrds_api.h"
#include "fmrds_generic.h"
#endif /* FEATURE_FM */

  /* --------------------------------------------------------------------- */
  /*                    Constants                                          */
  /* --------------------------------------------------------------------- */


  /* --------------------------------------------------------------------- */
  /*                    Global variables                                   */
  /* --------------------------------------------------------------------- */

static uint8 FmrdsClientId; //holds current user id
static tsFmrdsEvMsgBodyType EVENT_BODY; //holds last event sent to API(for logs)
static rdsStd_type PrgmType;

  /*===========================================================================

                          INTERNAL PROTOTYPES

===========================================================================*/


  /* --------------------------------------------------------------------- */
  /*                    External Functions                                 */
  /* --------------------------------------------------------------------- */

  /* --------------------------------------------------------------------- */
  /*                    Data Structures                                    */
  /* --------------------------------------------------------------------- */

/*===========================================================================

FUNCTION PRINT_ENABLE_FM

DESCRIPTION
   These functions print various data structures returned by events in F3 log messages.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void printRadioSettings(tsFmrdsRadioSettingsType radioSettings)
{
 FTM_MSG_HIGH(" radioSettings:", 0,0,0);
 switch(radioSettings.radioBand)
 {
  case US_EUROPE_BAND: FTM_MSG_HIGH("  radioBand = US_EUROPE_BAND", 0,0,0); break;
  case JAPAN_WIDE_BAND: FTM_MSG_HIGH("  radioBand = JAPAN_WIDE_BAND", 0,0,0); break;
  case JAPAN_STANDARD_BAND: FTM_MSG_HIGH("  radioBand = JAPAN_STANDARD_BAND", 0,0,0); break;
  case USER_DEFINED_BAND: FTM_MSG_HIGH("  radioBand = USER_DEFINED_BAND", 0,0,0); break;
  default: FTM_MSG_HIGH("  radioBand = NOT DEFINED", 0,0,0); break;
 }

 switch(radioSettings.deEmphasis)
 {
  case DE_EMP75:FTM_MSG_HIGH("  deEmphasis = DE_EMP75", 0,0,0);break;
  case DE_EMP50:FTM_MSG_HIGH("  deEmphasis = DE_EMP50", 0,0,0);break;
  default: FTM_MSG_HIGH("  deEmphasis = NOT DEFINED", 0,0,0); break;
 }

 switch(radioSettings.chSpacing)
 {
  case CHSPACE_200_KHZ: FTM_MSG_HIGH("  chSpacing = CHSPACE_200_KHZ",0,0,0);break;
  case CHSPACE_100_KHZ: FTM_MSG_HIGH("  chSpacing = CHSPACE_100_KHZ",0,0,0);break;
  case CHSPACE_50_KHZ: FTM_MSG_HIGH("  chSpacing = CHSPACE_50_KHZ",0,0,0);break;
  case CHSPACE_RESERVED: FTM_MSG_HIGH("  chSpacing = CHSPACE_RESERVED",0,0,0);break;
  default: FTM_MSG_HIGH("  chSpacing = NOT DEFINED", 0,0,0); break;
 }

 switch(radioSettings.rdsStd)
 {
  case RDS_STD_RBDS: FTM_MSG_HIGH("  rdsStd = RDS_STD_RBDS",0,0,0);break;
  case RDS_STD_RDS: FTM_MSG_HIGH("  rdsStd = RDS_STD_RDS",0,0,0);break;
  case RDS_STD_NONE: FTM_MSG_HIGH("  rdsStd = RDS_STD_NONE",0,0,0);break;
  default: FTM_MSG_HIGH("  rdsStd = NOT DEFINED", 0,0,0); break;
 }

 FTM_MSG_HIGH("  bandRange.lowerLimit = %d",radioSettings.bandRange.lowerLimit,0,0);
 FTM_MSG_HIGH("  bandRange.upperLimit = %d",radioSettings.bandRange.upperLimit,0,0);
}

void printRadioMode(teFmrdsRadioModeType radioMode)
{
 switch(radioMode)
 {
  case RADIO_MODE_OFF: FTM_MSG_HIGH(" radioMode = RADIO_MODE_OFF",0,0,0);break;
  case RADIO_MODE_RX: FTM_MSG_HIGH(" radioMode = RADIO_MODE_RX",0,0,0);break;
  case RADIO_MODE_TX: FTM_MSG_HIGH(" radioMode = RADIO_MODE_TX",0,0,0);break;
  case RADIO_MODE_TX_RX: FTM_MSG_HIGH(" radioMode = RADIO_MODE_TX_RX",0,0,0);break;
  default: FTM_MSG_HIGH(" radioMode = NOT DEFINED", 0,0,0); break;
 }
}

void printFmPwrUpMode(teFmrdsFmPwrUpMode fmPwrUpMode)
{
 switch(fmPwrUpMode)
 {
  case FM_OFF: FTM_MSG_HIGH(" fmPwrUpMode = FM_OFF",0,0,0);break;
  case FM_RX_ON: FTM_MSG_HIGH(" fmPwrUpMode = FM_RX_ON",0,0,0);break;
  case FM_TX_ON: FTM_MSG_HIGH(" fmPwrUpMode = FM_TX_ON",0,0,0);break;
  case FM_RESET: FTM_MSG_HIGH(" fmPwrUpMode = FM_RESET",0,0,0);break;
  default: FTM_MSG_HIGH(" fmPwrUpMode = NOT DEFINED", 0,0,0); break;
 }
}

void printRdsPwrUpMode(teFmrdsRdsPwrUpMode rdsPwrUpMode)
{
 switch(rdsPwrUpMode)
 {
  case RDS_OFF: FTM_MSG_HIGH(" rdsPwrUpMode = RDS_OFF",0,0,0);break;
  case RDS_ON: FTM_MSG_HIGH(" rdsPwrUpMode = RDS_ON",0,0,0);break;
  default: FTM_MSG_HIGH(" rdsPwrUpMode = NOT DEFINED", 0,0,0); break;
 }
}

void printRadioCap(teFmrdsRadioCapType radioCap)
{
 switch(radioCap)
 {
  case RADIO_CAP_NONE: FTM_MSG_HIGH(" radioCap = RADIO_CAP_NONE",0,0,0);break;
  case RADIO_CAP_RX: FTM_MSG_HIGH(" radioCap = RADIO_CAP_RX",0,0,0);break;
  case RADIO_CAP_TX: FTM_MSG_HIGH(" radioCap = RADIO_CAP_TX",0,0,0);break;
  case RADIO_CAP_TX_RX: FTM_MSG_HIGH(" radioCap = RADIO_CAP_TX_RX",0,0,0);break;
  default: FTM_MSG_HIGH(" radioCap = NOT DEFINED", 0,0,0); break;
 }
}

void printPowerMode(teFmrdsPwrModeType pwrMode)
{
 switch(pwrMode)
 {
  case FMRDS_NORMAL_MODE: FTM_MSG_HIGH(" pwrMode = FMRDS_NORMAL_MODE", 0,0,0);break;
  case FMRDS_LOW_POWER_MODE: FTM_MSG_HIGH(" pwrMode = FMRDS_LOW_POWER_MODE", 0,0,0);break;
  default: FTM_MSG_HIGH(" pwrMode = NOT DEFINED", 0,0,0); break;
 }
}

void printMuteMode(teFmrdsMuteLftRgtType muteMode)
{
 switch(muteMode)
 {
  case NOMUTE: FTM_MSG_HIGH(" muteMode = NOMUTE", 0,0,0); break;
  case MUTELEFT: FTM_MSG_HIGH(" muteMode = MUTELEFT", 0,0,0); break;
  case MUTERIGHT: FTM_MSG_HIGH(" muteMode = MUTERIGHT", 0,0,0); break;
  case MUTEBOTH: FTM_MSG_HIGH(" muteMode = MUTEBOTH", 0,0,0); break;
  default: FTM_MSG_HIGH(" muteMode = NOT DEFINED", 0,0,0); break;
 }
}

void printStereoMode(teFmrdsStereoMonoModeType monoStereo)
{
 switch(monoStereo)
 {
  case AUTO_MONO_STEREO_MODE: FTM_MSG_HIGH(" monoStereo = AUTO_MONO_STEREO_MODE", 0,0,0); break;
  case FORCE_MONO_ONLY_MODE: FTM_MSG_HIGH(" monoStereo = FORCE_MONO_ONLY_MODE", 0,0,0); break;
  default: FTM_MSG_HIGH(" monoStereo = NOT DEFINED", 0,0,0); break;
 }
}

void printRssi(uint8 rssi)
{
 FTM_MSG_HIGH(" rssi = %d",rssi,0,0);
}

void printRadioStation(uint16 stationFreq)
{
 FTM_MSG_HIGH(" stationFreq = %d", stationFreq,0,0);
}

void printVolumeLevel(teFmrdsVolumeLevType fmrdsVolLev)
{
 FTM_MSG_HIGH(" fmrdsVolLev = FM_VOLUME_LEVEL_%d", (uint8)fmrdsVolLev,0,0);
}

void printPsNum(uint8 psNum)
{
 FTM_MSG_HIGH(" psNum = %d", psNum,0,0);
}

void printPId(teFmrdsPrgmIdType piPrgmId)
{
 FTM_MSG_HIGH(" piPrgmId = %d", (uint16)piPrgmId,0,0);
}

void printPTY(tsFmrdsPrgmType ptyPrgmType)
{
 switch (PrgmType)
 {
  case RDS: FTM_MSG_HIGH(" RDS ptyPrgmType = %d", (uint8)ptyPrgmType.rdsOrRbdsMode.rdsPrgmType,0,0);break;
  case RBDS: FTM_MSG_HIGH(" RBDS ptyPrgmType = %d", (uint8)ptyPrgmType.rdsOrRbdsMode.rbdsPrgmType,0,0);break;
 }
}

void printTP(boolean taPrgmCodeFlag)
{
 if (taPrgmCodeFlag)FTM_MSG_HIGH(" taPrgmCodeFlag = 1",0,0,0);
 else FTM_MSG_HIGH(" taPrgmCodeFlag = 0",0,0,0);
}

void printTAC(boolean taAnnouncementCodeFlag)
{
 if (taAnnouncementCodeFlag)FTM_MSG_HIGH(" taAnnouncementCodeFlag = 1",0,0,0);
 else FTM_MSG_HIGH(" taAnnouncementCodeFlag = 0",0,0,0);
}

void printMSF(boolean msSwitchCodeFlag)
{
 if (msSwitchCodeFlag)FTM_MSG_HIGH(" msSwitchCodeFlag = 1",0,0,0);
 else FTM_MSG_HIGH(" msSwitchCodeFlag = 0",0,0,0);
}

void printDICC(boolean decIdCtrlCodeFlag)
{
 if (decIdCtrlCodeFlag)FTM_MSG_HIGH(" decIdCtrlCodeFlag = 1",0,0,0);
 else FTM_MSG_HIGH(" decIdCtrlCodeFlag = 0",0,0,0);
}

void printPSN(char psText[MAX_PS_INFO_CHARS][MAX_PS_INFO_CHAR_LEN])
{
 FTM_MSG_HIGH(" psText = %s", psText,0,0);
}

void printPSText(char *pCmptPSText)
{
 if (pCmptPSText==NULL)FTM_MSG_HIGH(" pCmptPSText = NULL",0,0,0);
 else FTM_MSG_HIGH(" pCmptPSText = %s", pCmptPSText,0,0);
}

void printRTlength(uint8 length)
{
 FTM_MSG_HIGH(" length = %d", (uint8)length,0,0);
}
 
void printABF(boolean textAbFlag)
{
 if (textAbFlag)FTM_MSG_HIGH(" textAbFlag = 1",0,0,0);
 else FTM_MSG_HIGH(" textAbFlag = 0",0,0,0);
}

void printRText(char *pRadioText)
{
 if (pRadioText==NULL) FTM_MSG_HIGH(" pRadioText = NULL",0,0,0);
 else FTM_MSG_HIGH(" pRadioText = %s", pRadioText,0,0);
}

void printIFCNT(uint8 ifcnt)
{
 FTM_MSG_HIGH(" ifcnt = %d",ifcnt,0,0);
}

void printRDScap(boolean rdsCapability)
{
 if (rdsCapability)FTM_MSG_HIGH(" rdsCapability = 1",0,0,0);
 else FTM_MSG_HIGH(" rdsCapability = 0",0,0,0);
}

void printRDSLockStatus(teFmrdsSyncStateChangeType rdsSyncStatus)
{
 if (rdsSyncStatus==NO_RDS_SYNC) FTM_MSG_HIGH(" rdsSyncStatus = NO_RDS_SYNC",0,0,0);
 else FTM_MSG_HIGH(" rdsSyncStatus = RDS_SYNC",0,0,0);
}

void printStereoProg(teFmrdsStereoProgramType stereoProgram )
{
 if (stereoProgram==FM_MONO_PROGRAM) FTM_MSG_HIGH(" StereoIndicator = FM_MONO_PROGRAM", 0,0,0);
  else 
   if (stereoProgram==FM_STEREO_PROGRAM) FTM_MSG_HIGH(" StereoIndicator = FM_STEREO_PROGRAM", 0,0,0);
    else FTM_MSG_HIGH(" StereoIndicator = UNDEFINED", 0,0,0);
}


/*===========================================================================

FUNCTION PRINT_ENABLE_FM

DESCRIPTION
   These functions print structure of events returned to callback function in F3 log messages.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void printEnableFmEv(tsFmrdsEnableFmEvType event)
{
 FTM_MSG_HIGH("FmrdsEnableFmEv contents:", 0,0,0);
 printRadioSettings(event.radioSettings);
 printRadioMode(event.radioMode);
 printRadioCap(event.radioCap);
 printFmPwrUpMode(event.fmPwrUpMode);
 printRdsPwrUpMode(event.rdsPwrUpMode);
}

void printDisableFmEv(tsFmrdsDisableFmEvType event)
{
 FTM_MSG_HIGH("FmrdsDisableFmEv contents:", 0,0,0);
 printRadioSettings(event.radioSettings);
 printRadioMode(event.radioMode);
 printFmPwrUpMode(event.fmPwrUpMode);
 printRdsPwrUpMode(event.rdsPwrUpMode);
}

void printPowerModeEv(tsFmrdsPwrModeEvType event)
{
 FTM_MSG_HIGH("FmrdsPwrModeEv contents:", 0,0,0);
 printPowerMode(event.pwrMode);
}

void printRadioConfigEv(tsFmrdsConfigRadioEvType event)
{
 FTM_MSG_HIGH("FmrdsConfigRadioEv contents:", 0,0,0);
 printRadioSettings(event.radioSettings);
}

void printMuteModeEv(tsFmrdsMuteModeEvType event)
{
 FTM_MSG_HIGH("FmrdsMuteModeEv contents:", 0,0,0);
 printMuteMode(event.muteMode);
}

void printStereoModeEv(tsFmrdsStereoMonoModeEvType event)
{
 FTM_MSG_HIGH("FmrdsStereoMonoModeEv contents:", 0,0,0);
 printStereoMode(event.monoStereo);
}

void printRadioStationEv(tsFmrdsRadioStationSetEvType event)
{
 FTM_MSG_HIGH("FmrdsRadioStationSetEv contents:", 0,0,0);
 printRadioStation(event.stationFreq);
}

void printRadioModeEv(tsFmrdsRadioModeEvType event)
{
 FTM_MSG_HIGH("FmrdsRadioModeEv contents:", 0,0,0);
 printRadioMode(event.radioMode);
}

void printTuneStatusEv(tsFmrdsRadioTuneStatusEvType event)
{
 FTM_MSG_HIGH("FmrdsRadioTuneStatusEv contents:", 0,0,0);
 printRadioStation(event.stationFreq);
 printRssi(event.rssi);
 if (event.stereoProgram==FM_MONO_PROGRAM) FTM_MSG_HIGH(" StereoIndicator = FM_MONO_PROGRAM", 0,0,0);
   
}

void printRxVolumeEv(tsFmrdsVolLevEvType event)
{
 FTM_MSG_HIGH("FmrdsRxVolLevEv contents:", 0,0,0);
 printVolumeLevel(event.fmrdsVolLev);
}

void printTxVolumeEv(tsFmrdsVolLevEvType event)
{
 FTM_MSG_HIGH("FmrdsTxVolLevEv contents:", 0,0,0);
 printVolumeLevel(event.fmrdsVolLev);
}

void printPSSummaryEv(tsFmrdsPSSummaryEvType event)
{
 FTM_MSG_HIGH("FmrdsPSSummaryEv contents:", 0,0,0);
 printPsNum(event.psNum);
 printPId(event.piPrgmId);
 printPTY(event.ptyPrgmType);
 printTP(event.taPrgmCodeFlag);
 printTAC(event.taAnnouncementCodeFlag);
 printMSF(event.msSwitchCodeFlag);
 printDICC(event.decIdCtrlCodeFlag);
 printPSN(event.psText);
 printPSText(event.pCmptPSText);
}

void printRTSummaryEv(tsFmrdsRTSummaryEvType event)
{
 FTM_MSG_HIGH("FmrdsRTSummaryEv contents:", 0,0,0);
 printRTlength(event.length);
 printPId(event.piPrgmId);
 printPTY(event.ptyPrgmType);
 printTP(event.taPrgmCodeFlag);
 printABF(event.textAbFlag);
 printRText(event.pRadioText);
}

void printParamAvbleEv(tsFmrdsParamsAvbleEvType event)
{
 FTM_MSG_HIGH("FmrdsParamsAvbleEv contents:", 0,0,0);
 printRadioStation(event.stationFreq);
 printRDSLockStatus(event.rdsSyncStatus);
 printMuteMode(event.muteMode);
 //printStereoMode(event.stereoMonoMode);
 printRssi(event.rssi);
 printIFCNT(event.ifcnt);
 printStereoProg(event.stereoProgram);
}

void printRdsLockStatusEv(tsFmrdsRdsLockStatusEvType event)
{
 FTM_MSG_HIGH("FmrdsRdsLockStatusEv contents:", 0,0,0);
 printRadioStation(event.stationFreq);
 printRssi(event.rssi);
 printRDScap(event.rdsCapability);
}

void printRssiInfoEv(tsFmrdsRssiInfoEvType event)
{
 FTM_MSG_HIGH("FmrdsRssiInfoEv contents:", 0,0,0);
 printRssi(event.rssi);
 printIFCNT(event.ifcnt);
}

void printAFInfoEv(tsFmrdsRdsAFSummaryEvType event)
{
 uint8 i;
 FTM_MSG_HIGH("FmrdsRdsAFSummaryEv contents:", 0,0,0);
 FTM_MSG_HIGH(" radioFreq: %d", event.radioFreq,0,0);
 if (event.noOfAFInList==0) FTM_MSG_HIGH(" noOfAFInList: EMPTY", 0,0,0);else FTM_MSG_HIGH(" noOfAFInList: %d", event.noOfAFInList,0,0);
 FTM_MSG_HIGH(" piPrgmId: %d", event.piPrgmId,0,0);
 for(i=0; i<event.noOfAFInList;i++) FTM_MSG_HIGH("  AFFreqList[%d]: %d", i,event.AFFreqList[i],0);
 }

void printServiceEv(tsFmrdsServiceAvailableEvType event)
{
 FTM_MSG_HIGH("FmrdsServiceAvailableEv contents:", 0,0,0);
 if (event.serviceAvailable == SERVICE_NOT_AVAILABLE) FTM_MSG_HIGH(" serviceAvailable = SERVICE_NOT_AVAILABLE", event.serviceAvailable,0,0);
 else FTM_MSG_HIGH(" serviceAvailable = SERVICE_AVAILABLE", event.serviceAvailable,0,0);
   
}

void printRdsRxOptEv(tsFmrdsRxProcOptEvType event)
{
 FTM_MSG_HIGH("FmrdsRxProcOptEv contents:", 0,0,0);
  FTM_MSG_HIGH(" enRdsGrps = %d",event.rxProcOpt.enRdsGrps,0,0);
  FTM_MSG_HIGH(" rdsBufSz = %d",event.rxProcOpt.rdsBufSz,0,0);
  FTM_MSG_HIGH(" enRdsChangeFiltr = %d",(uint8)event.rxProcOpt.enRdsChangeFiltr,0,0);
  FTM_MSG_HIGH(" enPSUncrtbleBlks = %d",(uint8)event.rxProcOpt.enPSUncrtbleBlks,0,0);
  FTM_MSG_HIGH(" enRdsBlkEProc = %d",(uint8)event.rxProcOpt.enRdsBlkEProc,0,0);
  FTM_MSG_HIGH(" enRdsAutoAF = %d",(uint8)event.rxProcOpt.enRdsAutoAF,0,0);
  FTM_MSG_HIGH(" rdsReturnRawBlocks = %d",(uint8)event.rxProcOpt.rdsReturnRawBlocks,0,0);
 }

void printRawRdsDataEv(tsFmrdsRawRdsGrpsEvType event)
{
 uint8 i;
 uint8 j;
 
 FTM_MSG_HIGH("FmrdsRawRdsGrpsEv contents:", 0,0,0);
 FTM_MSG_HIGH(" noOfRdsGrps = %d",event.rawRdsGrps.noOfRdsGrps,0,0);
 for(i=0; i<event.rawRdsGrps.noOfRdsGrps;i++) 
  {
    FTM_MSG_HIGH("  rdsGrps[%d]:", i,0,0);
    for(j=0; j<MAX_NO_OF_RX_BKS_IN_GRP;j++) FTM_MSG_HIGH("   rdsBlkErrStat[%d]: %d", j,(uint8)event.rawRdsGrps.rdsGrps[i].rdsBlkErrStat[j],0);
	for(j=0; j<MAX_NO_OF_RX_BKS_IN_GRP;j++) FTM_MSG_HIGH("   rdsBlkId[%d]: %d", j,(uint8)event.rawRdsGrps.rdsGrps[i].rdsBlkId[j],0);
    for(j=0; j<MAX_NO_OF_RX_BKS_IN_GRP;j++) FTM_MSG_HIGH("   rdsWord[%d]: %d", j,event.rawRdsGrps.rdsGrps[i].rdsWord[j],0);
 }
}

void printRawRdsBlockDataEv(tsFmrdsRawRdsBlockEvType event)
{
 FTM_MSG_HIGH("FmrdsRawRdsBlockEv contents:", 0,0,0);
 FTM_MSG_HIGH(" usRawBlockBuffSize = %d",event.usRawBlockBuffSize,0,0);
 FTM_MSG_HIGH(" &psRawBlockBuffPtr: ",0,0,0);
 FTM_MSG_HIGH("  usBlockA = %d",(event.psRawBlockBuffPtr)->usBlockA,0,0);
 FTM_MSG_HIGH("  usBlockB = %d",(event.psRawBlockBuffPtr)->usBlockB,0,0);
 FTM_MSG_HIGH("  usBlockC = %d",(event.psRawBlockBuffPtr)->usBlockC,0,0);
 FTM_MSG_HIGH("  usBlockD = %d",(event.psRawBlockBuffPtr)->usBlockD,0,0);

}



/*===========================================================================

FUNCTION FTM_FMRDS_API_REGISTER_CLIENT

DESCRIPTION
   This function registers a client for the service.

DEPENDENCIES
   None.

RETURN VALUE
   error_code: 0 indicates success, 1 = failure

SIDE EFFECTS
   None.

===========================================================================*/
ftm_fmrds_api_result_type ftm_fmrds_api_register_client(tfFmrdsEvCbFnType *pfnFmrdsEvCb,teFmrdsCbEvMaskType fmrdsCbEvMask)
{
 ftm_fmrds_api_result_type result;

 #ifdef FEATURE_FM
 if(FmrdsClientId == 0)
 {
   FmrdsClientId = FmrdsApi_RegisterClient(pfnFmrdsEvCb,fmrdsCbEvMask);
   FTM_MSG_HIGH(" Client Registration (ClientId = %d) SUCCESS ", FmrdsClientId,0,0);
   result.error_code = FTM_SUCCESS;
 }

 else//if already registred
 {
   FTM_MSG_HIGH("ONE CLIENT ALREADY REGISTERED (%d)", FmrdsApi_RegisterClient(pfnFmrdsEvCb, fmrdsCbEvMask),0,0);
   FTM_MSG_HIGH("Client Registration FAIL (already registered)",0,0,0);
	result.error_code = FTM_FAIL;
	return result;
 }

 #else
    result.error_code = FTM_TEST_NOT_IMPLEMENTED;
 #endif /* FEATURE_FM */

  return result;

}

/*===========================================================================

FUNCTION FTM_FMRDS_API_DEREGISTER_CLIENT

DESCRIPTION
   This function deregisters a client.

DEPENDENCIES
   None.

RETURN VALUE
   error_code: 0 indicates success, 1 = failure

SIDE EFFECTS
   None.

===========================================================================*/
ftm_fmrds_api_result_type ftm_fmrds_api_deregister_client(uint8 ucFmrdsClientId)
{
 ftm_fmrds_api_result_type result;

 #ifdef FEATURE_FM

 FmrdsApi_DeRegisterClient(ucFmrdsClientId);

 if (FmrdsClientId != 0)   
  {
   FmrdsClientId = 0;
   FTM_MSG_HIGH(" Client Deregistration SUCCESS",0,0,0);
   result.error_code = FTM_SUCCESS;
  }
 else 
  { FTM_MSG_HIGH(" Client Deregistration FAIL (no clients)",0,0,0);
    result.error_code = FTM_FAIL;
  }
 #else
    result.error_code = FTM_TEST_NOT_IMPLEMENTED;
 #endif /* FEATURE_FM */

  return result;

}

/*===========================================================================

FUNCTION FTM_FMRDS_API_EV_CB_FN

DESCRIPTION
   This function is used to catch events sent to API.
   New events are added here.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_fmrds_api_ev_cb_fn(tsFmrdsEvMsgType fmrdsEvMsg)
{

 uint8 success = 0;
  if (fmrdsEvMsg.fmrdsEvResult == FMRDS_EV_SUCCESS) success=1;

 EVENT_BODY = fmrdsEvMsg.fmrdsEventMsgBody;//for logging

 switch(fmrdsEvMsg.fmrdsEventName)
 {
  case FMRDS_EV_ENABLE_FM:

       if (success)
        FTM_MSG_HIGH(" ENABLE_FM_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" ENABLE_FM_EV FAIL ", 0, 0,0);

       printEnableFmEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsEnableFmEv);
       break;

  case FMRDS_EV_DISABLE_FM:

       if (success)
        FTM_MSG_HIGH(" DISABLE_FM_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" DISABLE_FM_EV FAIL ", 0, 0,0);

       printDisableFmEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsDisableFmEv);
       break;

  case FMRDS_EV_PWR_MODE_SET:

       if (success)
        FTM_MSG_HIGH(" PWR_MODE_SET_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" PWR_MODE_SET_EV FAIL ",0, 0,0);

       printPowerModeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsPwrModeEv);
       break;

  case FMRDS_EV_RADIO_MODE_SET:

       if (success)
        FTM_MSG_HIGH(" RADIO_MODE_SET_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" RADIO_MODE_SET_EV FAIL ", 0, 0,0);

       printRadioModeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRadioModeEv);
       break;

  case FMRDS_EV_CFG_RADIO:

       if (success)
        FTM_MSG_HIGH(" CFG_RADIO_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" CFG_RADIO_EV FAIL ", 0, 0,0);

       printRadioConfigEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsConfigRadioEv);
       break;

  case FMRDS_EV_MUTE_MODE_SET:

       if (success)
        FTM_MSG_HIGH(" MUTE_MODE_SET_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" MUTE_MODE_SET_EV FAIL ", 0, 0,0);

       printMuteModeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsMuteCtrlEv);
       break;

  case FMRDS_EV_STEREO_MODE_SET:

       if (success)
        FTM_MSG_HIGH(" STEREO_MODE_SET_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" STEREO_MODE_SET_EV FAIL ", 0, 0,0);

       printStereoModeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsStereoMonoModeEv);
   	 break;

  case FMRDS_EV_RADIO_STATION_SET:

       if (success)
        FTM_MSG_HIGH(" STATION_SET_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" STATION_SET_EV FAIL ", 0, 0,0);

       printRadioStationEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsStationSetEv);
       break;

  case FMRDS_EV_RADIO_TUNE_STATUS:

       if (success)
        FTM_MSG_HIGH(" GET_TUNE_STATUS_EV SUCCESS ", 0, 0,0);
       else
        FTM_MSG_HIGH(" GET_TUNE_STATUS_EV FAIL ", 0, 0,0);

       printTuneStatusEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsTuneStatusEv);
       break;

  case FMRDS_EV_RX_VOLUME_LEVEL_SET:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RX_VOLUME_LEVEL_SET SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RX_VOLUME_LEVEL_SET FAIL ", 0, 0,0);

       printRxVolumeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsVolLevEv);
       break;

   case FMRDS_EV_TX_VOLUME_LEVEL_SET:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_TX_VOLUME_LEVEL_SET SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_TX_VOLUME_LEVEL_SET FAIL ", 0, 0,0);
       printTxVolumeEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsVolLevEv);
       break;

   case FMRDS_EV_RDS_PS_INFO:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RDS_PS_INFO SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RDS_PS_INFO FAIL ", 0, 0,0);
       printPSSummaryEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsPSSummaryEv);
	   break;

   case FMRDS_EV_RDS_RT_INFO:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RDS_RT_INFO SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RDS_RT_INFO FAIL ", 0, 0,0);
       printRTSummaryEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRTSummaryEv);
	   break;

   case FMRDS_EV_STATION_PARAMETERS:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_STATION_PARAMETERS SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_STATION_PARAMETERS FAIL ", 0, 0,0);

	   printParamAvbleEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsParamAvbleEv);
	   break;

   case FMRDS_EV_RDS_LOCK_STATUS:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RDS_LOCK_STATUS SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RDS_LOCK_STATUS FAIL ", 0, 0,0);

	   printRdsLockStatusEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRdsLockStatusEv);
	   break;

   case FMRDS_EV_RSSI_INFO:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RSSI_INFO SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RSSI_INFO FAIL ", 0, 0,0);

	   printRssiInfoEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRssiInfoEv);
	   break;

   case FMRDS_EV_RDS_AF_INFO:
   
	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RDS_AF_INFO SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RDS_AF_INFO FAIL ", 0, 0,0);

	   printAFInfoEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsAFSummaryEv);
	   break;

   case FMRDS_EV_SERVICE_AVAILABLE:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_SERVICE_AVAILABLE SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_SERVICE_AVAILABLE FAIL ", 0, 0,0);

	   printServiceEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsFmServiceAvailableEv);
	   break;
 
   case FMRDS_EV_RDS_RX_OPTIONS_SET:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RDS_RX_OPTIONS_SET SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RDS_RX_OPTIONS_SET FAIL ", 0, 0,0);

	   printRdsRxOptEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRxProcOptEv);
	   break;

   case FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA FAIL ", 0, 0,0);

	   printRawRdsBlockDataEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRawRdsBlockEv);
	   break;

    case FMRDS_EV_RAW_RDS_RBDS_DATA:

	   if (success)
        FTM_MSG_HIGH(" FMRDS_EV_RAW_RDS_RBDS_DATA SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_RAW_RDS_RBDS_DATA FAIL ", 0, 0,0);

	   printRawRdsDataEv(EVENT_BODY.tufmrdsStatusParameters.fmrdsRawRdsGrpsEv);
	   break;

    case FMRDS_EV_SEARCH_COMPLETE:
       
       if (success)
        FTM_MSG_HIGH(" FMRDS_EV_SEARCH_COMPLETE SUCCESS", 0, 0,0);
       else
        FTM_MSG_HIGH(" FMRDS_EV_SEARCH_COMPLETE FAIL ", 0, 0,0);
       break;

  default:

       FTM_MSG_HIGH(" UNRECOGNIZED EVENT (FAIL) ", 0, 0,0);
       break;
 }

//return void;

}

/*===========================================================================

FUNCTION FTM_FMRDS_TEST

DESCRIPTION
   This function used to test API functions that return teFmrdsCmdQStatusType.

DEPENDENCIES
   None.

RETURN VALUE
   error_code: 0 indicates success, 1 = failure

SIDE EFFECTS
   None.

===========================================================================*/

ftm_fmrds_api_result_type ftm_fmrds_test(char str[], teFmrdsCmdQStatusType q_status)
{
 ftm_fmrds_api_result_type result;

 #ifdef FEATURE_FM

 switch(q_status)
 {
   case FMRDS_CMD_Q_PENDING:

       result.error_code = FTM_FMRDS_API_CMD_PENDING;
       //FTM_MSG_HIGH(" %s Q_PENDING ", str, 0,0);
       FTM_MSG_HIGH(" %s ", str, 0,0);
       break;

 
   case FMRDS_CMD_Q_FULL:

       result.error_code = FTM_FMRDS_API_QFULL;
       FTM_MSG_HIGH(" %s - Q_FULL ", str, 0,0);
       break;

   case  FMRDS_CMD_Q_UNRECOGNIZED_CMD:

       result.error_code = FTM_FMRDS_API_UNRECOGNIZED_CMD;
       FTM_MSG_HIGH(" %s - UNRECOGNIZED CMD ", str, 0,0);
       break;

  default:
    result.error_code = FTM_FAIL;
    FTM_MSG_HIGH("%s FAIL ",str,0,0);
    break;
  }

 #else
    result.error_code = FTM_TEST_NOT_IMPLEMENTED;
 #endif /* FEATURE_FM */

  return result;
}


/*===========================================================================

FUNCTION FTM_FMRDS_API_DISPATCH

DESCRIPTION
   This function handles FMRDS API FTM requests to run tests and
   other primitives by dispatching the appropriate functions.
   New CMDs are added here.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_fmrds_api_dispatch(ftm_fmrds_req_pkt_type *req_pkt)
{
    ftm_rsp_pkt_type rsp_pkt;
    byte *local_buffer = NULL;
    uint16 extra_data_len = 0;
	tsFmrdsRadioSettingsType radioSettings;
    teFmrdsCmdQStatusType q_status = FMRDS_CMD_Q_FULL;

    ftm_composite_cmd_header_type rsp_headers;
    ftm_fmrds_api_result_type result;

    
	 tsFmrdsVolLevOptsType fmrdsVolLev;
    tsFmrdsRxProcOptType fmrdsRxProcOpt;
    tsFmrdsRadioSrchModeType fmrdsRadioSrchMode;

    uint16 param = *((uint16*)&(req_pkt->data));// used for radioSettings => 0 - EUROPE and anything else - USA
    //FTM_MSG_HIGH("PASSED PARAMETER: %d",param ,0,0);


    /* Initialize response packet size to length of the common headers */
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size = sizeof(ftm_composite_cmd_header_type);
    /* the size of the return parameters will be added in the dispatch cases below */

    switch (req_pkt->ftm_hdr.cmd_id)
    {
    case FTM_FMRDS_REGISTER_CLIENT:

      result = ftm_fmrds_api_register_client(&ftm_fmrds_api_ev_cb_fn,(teFmrdsCbEvMaskType)param);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_DEREGISTER_CLIENT:

      result = ftm_fmrds_api_deregister_client(FmrdsClientId);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_DISABLE_FM:

      q_status = FmrdsApi_DisableFm();
      result = ftm_fmrds_test("CMD: DISABLE FM",q_status);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_ENABLE_FM:

      //fmrdsRadioSettings = *((tsFmrdsRadioSettingsType*)&(req_pkt->data));

      
 
	  switch (param)
	  {

      radioSettings.bandRange.upperLimit =0;
      radioSettings.bandRange.lowerLimit =0;

	  case 0: 
		  radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		  
          q_status = FmrdsApi_EnableFm(radioSettings);
		  FTM_MSG_HIGH("EUROPE RADIO SETTINGS WITH RDS", 0,0,0);
	      break;

	  case 1: 
          radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		  
	      q_status = FmrdsApi_EnableFm(radioSettings);
		  FTM_MSG_HIGH("EUROPE RADIO SETTINGS W/O RDS", 0,0,0);
	      break;

	  case  2:
          
	      radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP75;
		  radioSettings.chSpacing = CHSPACE_200_KHZ;
		  radioSettings.rdsStd = RDS_STD_RBDS ;
		 
          q_status = FmrdsApi_EnableFm(radioSettings);
		  FTM_MSG_HIGH("US RADIO SETTINGS WITH RBDS", 0,0,0);
	      break;

	  case 3:
          radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP75;
		  radioSettings.chSpacing = CHSPACE_200_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE;
		 
          q_status = FmrdsApi_EnableFm(radioSettings);
	      FTM_MSG_HIGH("US RADIO SETTINGS W/0 RBDS", 0,0,0);
	      break;
         
	   case 4:

		  radioSettings.radioBand = JAPAN_WIDE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		 
          q_status = FmrdsApi_EnableFm(radioSettings);
	      FTM_MSG_HIGH("JAPAN WIDEBAND RADIO SETTINGS WITH RDS", 0,0,0);
	      break;
 
	   case 5:

          radioSettings.radioBand = JAPAN_WIDE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		 
          q_status = FmrdsApi_EnableFm(radioSettings);
	      FTM_MSG_HIGH("JAPAN WIDEBAND RADIO SETTINGS W/0 RDS", 0,0,0);
	      break;
         
		 case 6:

          radioSettings.radioBand = JAPAN_STANDARD_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		  
          q_status = FmrdsApi_EnableFm(radioSettings);
		  FTM_MSG_HIGH("JAPAN STD RADIO SETTINGS WITH RDS", 0,0,0);
	      break;

		 case 7:

	      radioSettings.radioBand = JAPAN_STANDARD_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		  
          q_status = FmrdsApi_EnableFm(radioSettings);
		  FTM_MSG_HIGH("JAPAN STD RADIO SETTINGS W/0 RDS", 0,0,0);
	      break;

		 default: FTM_MSG_HIGH("UNDEFINED RADIO SETTINGS OPTION", 0,0,0);
	 
	  }
      
      result = ftm_fmrds_test("CMD: ENABLE FM",q_status);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_SET_LOW_PWR_MODE:

      //fmrdsPwrMode = *((teFmrdsPwrModeType*)&(req_pkt->data));

      q_status = FmrdsApi_SetLowPowerMode( (teFmrdsPwrModeType)param );

      if (param==0) result = ftm_fmrds_test("CMD: Set NORMAL POWER MODE",q_status);
      else
       if (param==1) result = ftm_fmrds_test("CMD: Set LOW POWER MODE",q_status);
       else
       {
        FTM_MSG_HIGH(" INVALID ARGUMENT ", 0, 0,0);
        result.error_code = FTM_SUCCESS;
       }
       
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);

      break;

	case FTM_FMRDS_CONFIGURE_RADIO:

      //fmrdsRadioSettings =*((tsFmrdsRadioSettingsType*)(req_pkt->data));
      switch (param)
	  {
	  case 0: 
		  radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
		  FTM_MSG_HIGH("EUROPE RADIO SETTINGS WITH RDS", 0,0,0);
	      break;

	  case 1: 
          radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		
	      q_status = FmrdsApi_ConfigureRadio(radioSettings);
		  FTM_MSG_HIGH("EUROPE RADIO SETTINGS W/O RDS", 0,0,0);
	      break;

	  case  2:
          
	      radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP75;
		  radioSettings.chSpacing = CHSPACE_200_KHZ;
		  radioSettings.rdsStd = RDS_STD_RBDS ;
		 
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
		  FTM_MSG_HIGH("US RADIO SETTINGS WITH RBDS", 0,0,0);
	      break;

	  case 3:
          radioSettings.radioBand = US_EUROPE_BAND;
          radioSettings.deEmphasis = DE_EMP75;
		  radioSettings.chSpacing = CHSPACE_200_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE;
		  
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
	      FTM_MSG_HIGH("US RADIO SETTINGS W/0 RBDS", 0,0,0);
	      break;
         
	   case 4:

		  radioSettings.radioBand = JAPAN_WIDE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		 
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
	      FTM_MSG_HIGH("JAPAN WIDEBAND RADIO SETTINGS WITH RDS", 0,0,0);
	      break;
 
	   case 5:

          radioSettings.radioBand = JAPAN_WIDE_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		  
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
	      FTM_MSG_HIGH("JAPAN WIDEBAND RADIO SETTINGS W/0 RDS", 0,0,0);
	      break;
         
		 case 6:

          radioSettings.radioBand = JAPAN_STANDARD_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_RDS ;
		  
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
		  FTM_MSG_HIGH("JAPAN STD RADIO SETTINGS WITH RDS", 0,0,0);
	      break;

		 case 7:

	      radioSettings.radioBand = JAPAN_STANDARD_BAND;
          radioSettings.deEmphasis = DE_EMP50;
		  radioSettings.chSpacing = CHSPACE_100_KHZ;
		  radioSettings.rdsStd = RDS_STD_NONE ;
		  
          q_status = FmrdsApi_ConfigureRadio(radioSettings);
		  FTM_MSG_HIGH("JAPAN STD RADIO SETTINGS W/0 RDS", 0,0,0);
	      break;

		 default: FTM_MSG_HIGH("UNDEFINED RADIO SETTINGS OPTION", 0,0,0);
	 
	  }

       result = ftm_fmrds_test("CMD: CFG RADIO",q_status);
	   rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
	   break;

    case FTM_FMRDS_SET_MUTE_MODE:

      //fmrdsMuteCtrl = *((teFmrdsMuteLftRgtType*)&(req_pkt->data));

      q_status = FmrdsApi_SetMuteMode( (teFmrdsMuteLftRgtType)param );

      if (param==0) 
      {
         snd_set_aux_line_in(NULL,NULL,SND_AUX_LINE_IN_MODE_ON);
         FMRDS_MSG_HIGH("AUDIO API TURNED ON",0,0,0);
         result = ftm_fmrds_test("Set NOMUTE mode",q_status);
      }
      else
       if (param ==1) result = ftm_fmrds_test("Set MUTELEFT mode",q_status);
       else
        if (param ==2) result = ftm_fmrds_test("Set MUTERIGHT mode",q_status);
        else if (param ==3) 
        {
            snd_set_aux_line_in(NULL,NULL,SND_AUX_LINE_IN_MODE_OFF);
            FMRDS_MSG_HIGH("AUDIO API TURNED OFF",0,0,0);
            result = ftm_fmrds_test("Set MUTEBOTH mode",q_status);

        }
         else
         {
          FTM_MSG_HIGH(" INVALID ARGUMENT to mute cmd ", 0, 0,0);
          result.error_code = FTM_SUCCESS;
         }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_SET_RADIO_MODE:

      //fmrdsTxRxMode = *((teFmrdsRadioModeType*)&(req_pkt->data));


      q_status = FmrdsApi_SetRadioMode( (teFmrdsRadioModeType)param );

      if (param==0) result = ftm_fmrds_test("CMD: Set RADIO_MODE_OFF mode",q_status);
      else
       if (param ==1) result = ftm_fmrds_test("CMD: Set RADIO_MODE_RX mode",q_status);
       else
        if (param ==2) result = ftm_fmrds_test("CMD: Set RADIO_MODE_TX mode",q_status);
        else
         if (param ==3) result = ftm_fmrds_test("CMD: Set RADIO_MODE_TX_RX mode",q_status);
         else
          {
           FTM_MSG_HIGH(" INVALID ARGUMENT to radio mode cmd", 0, 0,0);
           result.error_code = FTM_SUCCESS;
          }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;


    case FTM_FMRDS_SET_STEREO_MODE:

      //fmrdsStroMode = *((teFmrdsStereoMonoModeType*)&(req_pkt->data));

      q_status = FmrdsApi_SetStereoMode( (teFmrdsStereoMonoModeType)param );

      if (param==0) result = ftm_fmrds_test("CMD: Set AUTO_MONO_STEREO mode",q_status);
      else
       if (param ==1) result = ftm_fmrds_test("CMD: Set FORCE_MONO_ONLY mode",q_status);
        else
         {
           FTM_MSG_HIGH(" INVALID ARGUMENT to stereo mode cmd", 0, 0,0);
           result.error_code = FTM_SUCCESS;
         }

      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_GET_TUNE_STATUS:

      q_status = FmrdsApi_GetTuneStatus();
      result = ftm_fmrds_test("CMD: GET TUNE STATUS",q_status);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_SET_STATION:

      //usFmrdsFreq = *((unit16*)&(req_pkt->data));

      q_status = FmrdsApi_SetStation(param);
      result = ftm_fmrds_test("CMD: SET STATION",q_status);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;

    case FTM_FMRDS_SILAB_READ_REGS:

	FTM_MSG_HIGH("CMD: Silabs Register summary (refer si4703 datasheet)", 0,0,0);
    FtmFmrdsDev_ReadRegs();
    break;   

	case FTM_FMRDS_SET_VOLUME_LEVEL:

	if (param>15) { fmrdsVolLev.volMode = FM_VOLUME_MODE_TX; fmrdsVolLev.volLev = (teFmrdsVolumeLevType)(param - 16);}
	else {fmrdsVolLev.volMode = FM_VOLUME_MODE_RX; fmrdsVolLev.volLev = (teFmrdsVolumeLevType)(param);}
    
	q_status = FmrdsApi_SetVolume( fmrdsVolLev );
    result = ftm_fmrds_test("CMD: SET VOLUME",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
    break;

	case FTM_FMRDS_GET_RDS_PS_INFO:
    
    q_status = FmrdsApi_GetPSInfo();
    result = ftm_fmrds_test("CMD: GET PS INFO",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
	break;

	case FTM_FMRDS_GET_RDS_RT_INFO:

	q_status = FmrdsApi_GetRTInfo();
    result = ftm_fmrds_test("CMD: GET RT INFO",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	 
    break;

	case FTM_FMRDS_GET_RDS_LOCK_STATUS:

	q_status = FmrdsApi_GetRdsSyncStatus();
    result = ftm_fmrds_test("CMD: GET RDS LOCK STATUS",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	 
    break;

	case FTM_FMRDS_GET_RSSI_INFO:

	q_status = FmrdsApi_GetRssiInfo();
    result = ftm_fmrds_test("CMD: GET RSSI INFO",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	 
    break;

	case FTM_FMRDS_GET_STATION_PARAMETERS:

	q_status = FmrdsApi_GetStationParameters();
    result = ftm_fmrds_test("CMD: GET STATION PARAMETERS",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	 
    break;

	case FTM_FMRDS_GET_RDS_AF_INFO:
     
    q_status = FmrdsApi_GetAFInfo();
    result = ftm_fmrds_test("CMD: GET RDS AF INFO",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	 
    break;

	case FTM_FMRDS_SERVICE_AVAILABLE:

	q_status = FmrdsApi_FMServiceAvailable((uint8)param);
	result = ftm_fmrds_test("CMD: GET SERVICE AVAILABILITY",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	
    
	
	break;

	case FTM_FMRDS_SET_RDS_RX_PROCESSING_OPTIONS:

 	fmrdsRxProcOpt.enRdsGrps = 0xFFFFFFFF;
	if (param>=128)
	  {
	   fmrdsRxProcOpt.rdsBufSz=(uint8)param-128;
	   fmrdsRxProcOpt.rdsReturnRawBlocks = 1;
	  }
	else
	 {	
		fmrdsRxProcOpt.rdsBufSz=(uint8)param;
        fmrdsRxProcOpt.rdsReturnRawBlocks = 0;
	 }
	fmrdsRxProcOpt.enRdsChangeFiltr = 1;
	fmrdsRxProcOpt.enPSUncrtbleBlks = 1;
	fmrdsRxProcOpt.enRdsBlkEProc = 1;
	fmrdsRxProcOpt.enRdsAutoAF = 1;
	
	q_status = FmrdsApi_SetRdsRxProcessingOptions(fmrdsRxProcOpt);
	result = ftm_fmrds_test("CMD: SET RDS RX PROCESSING OPTIONS",q_status);
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);	
	break;

   case FTM_FMRDS_SEEK_STATION:
      fmrdsRadioSrchMode.scanTime = 0;
      fmrdsRadioSrchMode.srchMode = SEARCHMODE_SEEK;
      fmrdsRadioSrchMode.srchWrap  = WRAP_AT_END_OF_BAND;

      if (param>=128)
      {
         fmrdsRadioSrchMode.srchRssiLev = (uint8)param-128;
         fmrdsRadioSrchMode.srchDir = 1; //search dir down
      }
      else
      {	
         fmrdsRadioSrchMode.srchRssiLev = (uint8)param;
         fmrdsRadioSrchMode.srchDir = 0; //search dir up
      }
      q_status = FmrdsApi_SearchStations(fmrdsRadioSrchMode);
      result = ftm_fmrds_test("CMD: SEEK STATION",q_status);
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);

      break;

   default:

      FTM_MSG_HIGH(" FTM TEST NOT IMPLEMENTED", 0, 0,0);
      result.error_code = FTM_TEST_NOT_IMPLEMENTED;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_fmrds_api_result_type);
      break;
    }

    /* Create response packet */
    /* Copy common header information from the request to the response */
    rsp_headers.diag_hdr = req_pkt->diag_hdr;
    rsp_headers.ftm_hdr.cmd_id = req_pkt->ftm_hdr.cmd_id;
    rsp_headers.ftm_hdr.cmd_data_len = req_pkt->ftm_hdr.cmd_data_len;

    /* At this point the response packet size equals the size of the headers plus
       the size of the fixed return parameters. If the response contains extra data
       whose size is determined by a parameter in the request, then we add it here. */

    rsp_pkt = ftmdiag_create_new_pkt(rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len);

    if (rsp_pkt.pkt_payload != NULL)
    {
      /* copy header information */
      memcpy((void*)rsp_pkt.pkt_payload, (void*)&rsp_headers, sizeof(ftm_composite_cmd_header_type));
      /* copy fixed return parameters */
      memcpy((void*)((byte*)rsp_pkt.pkt_payload + sizeof(ftm_composite_cmd_header_type)), (void*)&result,
             (rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type)));
      if (extra_data_len > 0)
      {
        /* copy extra variable length return data if present */
        memcpy((void*)((byte*)rsp_pkt.pkt_payload + rsp_headers.ftm_hdr.cmd_rsp_pkt_size), local_buffer, extra_data_len);
      }
    }
    else
    {
      FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %d", req_pkt->ftm_hdr.cmd_id, 0, 0);
    }
    if (local_buffer != NULL)
    {
      ftm_free(local_buffer);
    }
    return rsp_pkt;
}

#endif /* FEATURE_FTM_FMRDS */
#endif /* FEATURE_FACTORY_TESTMODE */
