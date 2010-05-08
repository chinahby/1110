/*=============================================================================

FILE: CoreService.c

SERVICES: QCT UI Core BREW Application.

GENERAL DESCRIPTION:
This file contains the Core App's CM related functions.

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
Functions in this file are called from CoreApp.c. CM related event handler and its associated functions.

(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreService.c#15 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/08   sg      Update the RSSI bars using new UI signal strength indicator 
                   algorithm
12/21/07   cvs     Fix issues relating to slower CM init, and new ICard behavior
10/25/07   cvs     Lint fixes
09/28/07   clm     Add PLMN name update CFGI and get functions.
08/14/07   cvs     Fix case where 1x targets would toggle RSSI when not in hybrid
06/13/07   cvs     Handle LIMITED_REGIONAL like LIMITED service
06/12/07   cvs     Don't update hybrid RSSI when LCD off
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/18/06   cvs     Added debug messages
12/14/06   ypmw    Optimize the UI utilization time for AEECM_OPRT_MODE_NET_TEST_GW 
                   event
12/07/06   cvs     Fix featurization of RefreshHDRProtAnnunciator()
12/06/06   cvs     Fix lint issues
12/06/06   RT      CoreApp Refactoring: Moved CoreApp_DisplayPLMNAnn and associated 
                   functions from CoreApp.c
10/30/06   RT      Initial Revision - CoreApp Refactoring
===========================================================================*/


/*===============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModTable.h"
#include "AEEAppGen.h"
#include "AEEAnnunciator.h"

#include "CoreService.h"
#include "CoreApp.h"
#include "CoreMenu.h"
#include "CoreUtils.h"
#include "AppComFunc.h"
#include "CoreApp_res.h"
#include "CoreApp_qvga_res.h"
#include "CoreApp_vga_res.h"
#include "core_ui.h"

#include "uiutils.h"

#if defined (FEATURE_APP_FLDDBG)
#include "FLDDBGAPP.BID"
#endif

static boolean CoreApp_ProcessFTMMode(CCoreApp *pMe);

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
void SendRTREConfig (CCoreApp *pMe);
#endif

/*===========================================================================

                      Constant Definitions

===========================================================================*/

#define MAX_PLMN_ANNUN_SIZE 8
#define MAX_STR_SIZE 50
#define TOGGLE_HYBRID_RSSI_TIMER 2000

/*===========================================================================

                      STATIC and PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION InitAfterPhInfo

DESCRIPTION
  Executes startup sequence after phone information is available from ICM.
  If phone info is available at Core App startup, no need to wait for
  AEECM_EVENT_PH_INFO_AVAIL event.

DEPENDENCIES
  ICM interface should have been created previously.

SIDE EFFECTS
  None
===========================================================================*/
void InitAfterPhInfo(CCoreApp *pMe, AEECMOprtMode mode)
{
  if (pMe == NULL)
  {
    CORE_ERR("Null pMe",0,0,0);
    return;
  }

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
  /* Send RTRE config changed to CM - response from CM will */
  /* generate subscription changed event */
  SendRTREConfig(pMe);
#endif

  //Change modes if we're provisioned properly.
  if (mode != AEECM_OPRT_MODE_FTM)
  {
    if (ui_check_provisioning() && mode != AEECM_OPRT_MODE_OFFLINE &&
        mode != AEECM_OPRT_MODE_OFFLINE_CDMA &&
        mode != AEECM_OPRT_MODE_OFFLINE_AMPS) {
      ISHELL_PostEventEx (pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                          EVT_USER,EVT_SET_OPERATING_MODE,0L);
    }
    else
    {
      ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_OFFLINE);
    }
  }
  else
  {
    ICM_SetOperatingMode(pMe->m_pICM, AEECM_OPRT_MODE_OFFLINE);
  }
  pMe->m_bProvisioned = TRUE;

  switch (mode)
  {
  case AEECM_OPRT_MODE_FTM:
    if (!CoreApp_ProcessFTMMode(pMe)) {
      CORE_ERR("Error in ProcessFTMMode",0,0,0);
    }
    break;
  case AEECM_OPRT_MODE_OFFLINE:
    if (!CoreApp_ProcessOffLine(pMe)) {
      CORE_ERR("Error in ProcessFTMMode",0,0,0);
    }
    break;
  case AEECM_OPRT_MODE_ONLINE:

#if defined (FEATURE_APP_FLDDBG)
     // Close the FldDbg app, if its currently active
    if(ISHELL_ActiveApplet(pMe->a.m_pIShell) == AEECLSID_FIELDDEBUGAPP)
      ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);

    if(pMe->m_bSuspended)
    {
      if((ISHELL_StartApplet(pMe->a.m_pIShell, AEECLSID_CORE_APP)) != AEE_SUCCESS)
      {
       CORE_ERR("Could not start CoreApp", 0, 0, 0);
      }
    }
#endif
    (void) GetBanner(pMe);
    break;

    default:
      break;

  }
}

#ifdef FEATURE_CPHS_EONS
/*===========================================================================
FUNCTION CoreApp_DisplaySPN

DESCRIPTION
  Displays the Service Provider Name in the annunciators according to Algorithm.
  in EONS Feature Doc
DEPENDENCIES
  Service #17/#19 is available and activatedin SST/UST respectively

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplaySPN(CCoreApp *pMe, char* buf)
{
  AEECMPLMNID                 rplmn, *pPLMNList;
  boolean                     bInHomeNetwork = FALSE;
  int i=0;
  int len;
  
  if(!pMe || !buf)
  {
    CORE_ERR("EONS:Null Pointer Access",0,0,0);
    return FALSE;
  }

  MEMCPY(rplmn.identity, pMe->m_ssInfo.sys_id.id.plmn_lac.plmn.identity, COREAPP_NETWORK_PLMN_LEN);
  if (ICM_SysMatchPLMN(pMe->m_pICM,rplmn,pMe->m_hplmn))
  {
    bInHomeNetwork = TRUE;
  }

  //Not in HPLMN now check the SPDI entries
  if (!bInHomeNetwork) {
    pPLMNList = pMe->m_SPDInfo.pPLMNIds;

    for (i = 0; i < pMe->m_SPDInfo.numPLMNs; i++,pPLMNList++)
    {
      if (ICM_SysMatchPLMN(pMe->m_pICM,rplmn,(*pPLMNList)))
      {
        bInHomeNetwork = TRUE;
        break;
      }
    }
  }

  if (bInHomeNetwork)
  {
    MSG_HIGH("EONS:SPDI PLMN %d matches with HPLMN",i,0,0);
    if (pMe->m_SPNInfo.m_displayCond == 0x0) {
      MSG_HIGH("EONS:Disp Condition Equals Zero...SPN Name Display Not Needed",0,0,0);
      return FALSE;
    }
    else
    {
      MSG_HIGH("EONS:Decoded SPN Name is to be displayed",0,0,0);
      if (pMe->m_SPNInfo.m_displayCond & 0x01) {
        /* bit #1 is set ONS Needed */
        MSG_HIGH("EONS:Operator Name Display Needed",0,0,0);
        /* It is important to note that we return false here since we have
        only one annuntiator for op name and ONS string */
        return FALSE;
      }
      else
      {
        MSG_HIGH("EONS:Operator Name Display Not Needed",0,0,0);
        len = STRLEN((const char*)pMe->m_SPNInfo.m_operatorName);
        MEMCPY( buf, pMe->m_SPNInfo.m_operatorName,
            MIN( len, MAX_PLMN_ANNUN_SIZE));
        return TRUE;
      }
    }
  }
  else
  {
    MSG_HIGH("EONS:SPDI list not matching with HPLMN",0,0,0);
    if (pMe->m_SPNInfo.m_displayCond & 0x02) {
      MSG_HIGH("EONS:Disp Cond Bit Set...SPN Name Display Not Needed",0,0,0);
      return FALSE;
    }
    else
    {
      MSG_HIGH("EONS:Disp Cond Bit Not Set...SPN Name Display Needed",0,0,0);
      /* It is important to note that we return false here since we have
      only one annuntiator for op name and ONS string */
      return FALSE;
    }
  }
}

/*===========================================================================
FUNCTION CoreApp_CompareWildCardPLMN

DESCRIPTION
  Compares the PLMNs taking into consideration the wildcard characters 'D'
  So the idea is that if it is a wildcard character it shud always match
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static boolean CoreApp_CompareWildCardPLMN(CCoreApp *pMe, AEECMPLMNID hplmn, AEECMPLMNID rplmn)
{
  int i;
  // mask the wildcard in hplmn such that it will match with rplmn
  for (i=0; i<COREAPP_NETWORK_PLMN_LEN; i++) {
    if (COREAPP_PLMN_WILDCARD_CHAR == (hplmn.identity[i] & 0x0F))
    {
      hplmn.identity[i] = (hplmn.identity[i] & 0xF0) | (rplmn.identity[i] & 0x0F);
      MSG_MED("EONS: Wildcard match in lower nibble of byte %d hplmn= %x rplmn=%x",
              i,hplmn.identity[i],rplmn.identity[i]);
    }

    if (COREAPP_PLMN_WILDCARD_CHAR == ((hplmn.identity[i] & 0xF0) >> 4))
    {
      hplmn.identity[i] = (rplmn.identity[i] & 0xF0) | (hplmn.identity[i] & 0x0F);
      MSG_MED("EONS: Wildcard match in higher nibble of byte %d, hplmn= %x rplmn=%x",
              i,hplmn.identity[i],rplmn.identity[i]);
    }

  }
  if (ICM_SysMatchPLMN(pMe->m_pICM,
                   hplmn,
                   rplmn))
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION CoreApp_CompareOPLAndRegPLMN

DESCRIPTION
  Compares the HPLMN with the OPL List in the SIM card
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static boolean CoreApp_CompareOPLAndRegPLMN(CCoreApp *pMe, uint8 *recNum)
{
  int16 plmnToCompare=0;
  boolean matchFound = FALSE;

  if(!pMe || !recNum)
  {
    CORE_ERR("EONS:Null Pointer Access",0,0,0);
    return FALSE;
  }

  if (0 != pMe->m_OpListInfo.numPLMNs && pMe->m_OpListInfo.bIsOPLInfoAvailable) {
    while (plmnToCompare < pMe->m_OpListInfo.numPLMNs)
    {

      if (CoreApp_CompareWildCardPLMN(pMe,
                           pMe->m_OpListInfo.plmnInfo[plmnToCompare].PLMNId,
                           pMe->m_ssInfo.sys_id.id.plmn_lac.plmn))
      {
        //Compare LAC1 and LAC2 remove from ss info must be the same which NAS received in MM info msg
        if ((pMe->m_ssInfo.sys_id.id.plmn_lac.lac == pMe->m_OpListInfo.plmnInfo[plmnToCompare].lac1) &&
            (pMe->m_ssInfo.sys_id.id.plmn_lac.lac == pMe->m_OpListInfo.plmnInfo[plmnToCompare].lac2))
        {
          MSG_MED("EONS:LAC1 == LAC2 == Reg LAC match at %d",plmnToCompare,0,0);
          matchFound = TRUE;
          break;
        }
        else if ((pMe->m_OpListInfo.plmnInfo[plmnToCompare].lac1 <= pMe->m_ssInfo.sys_id.id.plmn_lac.lac) &&
                 (pMe->m_ssInfo.sys_id.id.plmn_lac.lac <= pMe->m_OpListInfo.plmnInfo[plmnToCompare].lac2))
        {
          MSG_MED("EONS:LAC1 <= Reg LAC <= LAC2 match at %d",plmnToCompare,0,0);
          matchFound = TRUE;
          break;
        }
      }

      plmnToCompare++;
    }
  }

  if(matchFound)
  {
    /* This rec number is the rec in the PNN list minus one is array index */
    *recNum = (pMe->m_OpListInfo.plmnInfo[plmnToCompare].pnnRecNum -1);
    MSG_MED("EONS:Record with Matching PLMN is %d",*recNum,0,0);
    return TRUE;
  }
  else
  {
    MSG_MED("EONS:Num PLMNs is %d and PLMN Info Available Flag is %d",
            pMe->m_OpListInfo.numPLMNs,pMe->m_OpListInfo.bIsOPLInfoAvailable,0);
    *recNum=0;
    return FALSE;
  }
}

/*===========================================================================
FUNCTION CoreApp_GetPNNName

DESCRIPTION
  Fill the buf with the proper PNN Name
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
static boolean CoreApp_GetPNNName(CCoreApp *pMe, char** buf, uint8 recNum)
{
  if(!pMe || !(*buf))
  {
    CORE_ERR("EONS:Null Pointer Access",0,0,0);
    return FALSE;
  }

  if ( (!pMe->m_PNNInfo.bIsPNNInfoAvailable) || (recNum >= COREAPP_MAX_PNN_ARRAY_SIZE) ) {
    CORE_ERR("EONS:PNN Info Invalid.  recnum %d",recNum,0,0);
    return FALSE;
  }

  if ((0 < pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen) && 
      ( pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen < MAX_PLMN_ANNUN_SIZE)) {
    MSG_MED("EONS:Added Long Name Len = %d ",pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen,0,0);
    MEMCPY(*buf,pMe->m_PNNInfo.m_pnnArray[recNum].szLongNetworkName,
                                pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen);
  }
  else if (( 0 < pMe->m_PNNInfo.m_pnnArray[recNum].shortNameLen) &&
            (pMe->m_PNNInfo.m_pnnArray[recNum].shortNameLen < MAX_PLMN_ANNUN_SIZE)) {
    MSG_MED("EONS:Added Short Name Len = %d ",pMe->m_PNNInfo.m_pnnArray[recNum].shortNameLen,0,0);
    MEMCPY(*buf,pMe->m_PNNInfo.m_pnnArray[recNum].szShortNetworkName,
                                pMe->m_PNNInfo.m_pnnArray[recNum].shortNameLen);
  }
  else
  {
    MSG_MED("EONS:Truncate Long Name Len = %d ",pMe->m_PNNInfo.m_pnnArray[recNum].longNameLen,0,0);
    MEMCPY(*buf,pMe->m_PNNInfo.m_pnnArray[recNum].szLongNetworkName,
                                (MAX_PLMN_ANNUN_SIZE-1));
  }

  return TRUE;
}

/*===========================================================================
FUNCTION CoreApp_DisplayONS

DESCRIPTION
  Displays the Operator Name String in the annunciators according to Algorithm.
  in EONS Feature Doc
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplayONS(CCoreApp *pMe, char* buf,uint8 slot, uint8 *recNum)
{
  AEECardServiceIndType svcInd,pnnSvcInd;
  boolean SrvAvailable = FALSE;

  if(!pMe || !buf || !recNum)
  {
    CORE_ERR("EONS:Null Pointer Access",0,0,0);
    return FALSE;
  }

  /* SPN Not available so check the OPL and PNN */
  if (  (SUCCESS == ICARD_GetServiceAvailableInd(pMe->m_pICard, slot,
                                   AEECARD_OPLMN_LIST,
                                   &svcInd)) &&
        (SUCCESS == ICARD_GetServiceAvailableInd(pMe->m_pICard, slot,
                                   AEECARD_PLMN_NTWRK_NAME,
                                   &pnnSvcInd)))
  {
    if((TRUE == svcInd.bGSMSvcOn || TRUE == svcInd.bUMTSSvcOn) &&
       (TRUE == pnnSvcInd.bGSMSvcOn || TRUE == pnnSvcInd.bUMTSSvcOn))
    {
      SrvAvailable = TRUE;
    }
  }
  else
  {
    SrvAvailable = FALSE;
  }

  if (SrvAvailable)
  {
    MSG_MED("EONS: PNN and OPL are Available in SIM",0,0,0);
    if ( TRUE == CoreApp_CompareOPLAndRegPLMN(pMe, recNum))
    {
      return (CoreApp_GetPNNName(pMe,&buf,*recNum));
    }
    else
    {
      MSG_MED("EONS: RPLMN comparision with OPL PLMNs Failed",0,0,0);
      return FALSE;
    }
  }
  else
  {
    MSG_HIGH("EONS: PNN and OPL are Not Available in SIM",0,0,0);
    /* Service not available if Reg PLMN is same as HPLMN read the first Rec
    from PNN file */
    if (ICM_SysMatchPLMN(pMe->m_pICM,
                         pMe->m_hplmn,
                         pMe->m_ssInfo.sys_id.id.plmn_lac.plmn))
    {
      MSG_HIGH("EONS: Since the Phone is in HPLMN Display 1st PNN Record",0,0,0);
      /* Read PNN record */
      /* buf = PNN Rec Name */
      return (CoreApp_GetPNNName(pMe,&buf,0));
    }
    else
    {
      MSG_HIGH("EONS: PNN Lookup Failed",0,0,0);
      return FALSE;
    }
  }
}

/*===========================================================================
FUNCTION CoreApp_DisplayFromCPHSInfo

DESCRIPTION
  Displays the Operator Name String from CPHS Info
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplayFromCPHSInfo(CCoreApp *pMe, char* buf)
{
  if(!pMe || !buf)
  {
    CORE_ERR("EONS:Null Pointer Access",0,0,0);
    return FALSE;
  }
  if (pMe->m_CPHSOpNameInfo.m_NameLen > 0)
  {
    MEMCPY(buf,pMe->m_CPHSOpNameInfo.m_operatorName,
           MIN(pMe->m_CPHSOpNameInfo.m_NameLen,COREAPP_MAX_CPHS_OPNAME_LEN));
    return TRUE;
  }
  return FALSE;
}
#endif /* FEATURE_CPHS_EONS */


#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
/*===========================================================================
FUNCTION CCoreApp_DisplayPLMNAnn

DESCRIPTION
  Displays the PLMN id in the annunciators.

DEPENDENCIES
  ui_netwk_get_network_info, IANNUNCIATOR_SetFieldEx

SIDE EFFECTS
  None
===========================================================================*/
static boolean CCoreApp_DisplayPLMNAnn(CCoreApp *pMe)
{
  char *buf = NULL;
  AECHAR wBuf[MAX_PLMN_ANNUN_SIZE+1];

  if (!pMe ||  !pMe->m_pIAnn)
    return FALSE;

  IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, ANNUN_STATE_OFF);

  (void)CoreUtils_GetPLMN(pMe, &buf, TRUE);

  if(STRLEN(buf) > MAX_PLMN_ANNUN_SIZE) buf[MAX_PLMN_ANNUN_SIZE] = '\0';
  (void) STRTOWSTR(buf, wBuf, sizeof(AECHAR)* (MAX_PLMN_ANNUN_SIZE+1));
  IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, wBuf);

  DisplayTimeWithRefresh(pMe);

  FREEIF(buf);

  return TRUE;
}
#endif

/*===========================================================================
FUNCTION DBToLevel

DESCRIPTION
  Converts RSSI absolute DB value to an annunciator level

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int DBToLevel (int nDBVal)
{
  if (nDBVal < 60)
    return ANNUN_STATE_RSSI_4;
  else if (nDBVal < 75)
    return ANNUN_STATE_RSSI_3;
  else if (nDBVal < 90)
    return ANNUN_STATE_RSSI_2;
  else if (nDBVal < 105)
    return ANNUN_STATE_RSSI_1;
  else
    return ANNUN_STATE_RSSI_0;
}

/*===========================================================================
FUNCTION Calculate_HDR_RSSI

DESCRIPTION
  Calculates RSSI bars based on DB level and SiNR

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_HDR
static int Calculate_HDR_RSSI (int nDBVal, int nSinr)
{
#ifdef FEATURE_SINR_RSSI
  int sinr_bars = (nSinr + 1)/2;

  int pwr_bars, display_bars;

  if (nDBVal <= 75)
        pwr_bars=4;
  else if (nDBVal <= 85)
        pwr_bars=3;
  else if (nDBVal <= 95)
        pwr_bars=2;
  else if (nDBVal <= 100)
        pwr_bars=1;
  else
        pwr_bars=0;

  CORE_MSG("UI calculated %d sinr_bars, %d pwr_bars, at %d dB", sinr_bars, pwr_bars, nDBVal);
  switch (MIN(sinr_bars, pwr_bars)) {
  case 0:
    display_bars = ANNUN_STATE_RSSI_0;
    break;
  case 1:
    display_bars = ANNUN_STATE_RSSI_1;
    break;
  case 2:
    display_bars = ANNUN_STATE_RSSI_2;
    break;
  case 3:
    display_bars = ANNUN_STATE_RSSI_3;
    break;
  case 4:
    display_bars = ANNUN_STATE_RSSI_4;
    break;
  default:
    display_bars = ANNUN_STATE_RSSI_0;
    break;
  }

  return display_bars;
#else
  return DBToLevel(nDBVal);
#endif //FEATURE_SINR_RSSI
}
#endif //FEATURE_HDR

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION CoreApp_ToggleHybridRSSI

DESCRIPTION
  Toggles the RSSI annunciators display for hybrid mode.

DEPENDENCIES
  ui_netwk_get_network_info, IANNUNCIATOR_SetFieldEx

SIDE EFFECTS
  None
===========================================================================*/
static void CoreApp_ToggleHybridRSSI(void *pUser)
{
  CCoreApp *pMe = (CCoreApp*) pUser;

  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if(!pMe->m_pIAnn)
    return;

  if( !GetDisplayStatus()){
    // if the LCD is off, do not update the annunciator
    ISHELL_CancelTimer (pMe->a.m_pIShell, (PFNNOTIFY) CoreApp_ToggleHybridRSSI, pMe);
    return;
  }

  pMe->m_bHDRDisplay = !pMe->m_bHDRDisplay;

  if(pMe->m_bHDRDisplay)
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_HDR_RSSI,
        Calculate_HDR_RSSI(pMe->m_ssInfo.hdr_rssi, pMe->m_ssInfo.hdr_sinr));
  }
  else
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI,
                           (uint32)DBToLevel(pMe->m_ssInfo.rssi));
  }
  ISHELL_SetTimer(pMe->a.m_pIShell, TOGGLE_HYBRID_RSSI_TIMER,
    CoreApp_ToggleHybridRSSI, pUser);
}
#endif  // FEATURE_HDR

#if defined(FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY)
/* RSSI ranges */
#define RSSI_IN_R1(x) ((x) >= -95)
#define RSSI_IN_R2(x) ((x) >= -99 && (x) < -95)
#define RSSI_IN_R3(x) ((x) >= -102 && (x) < -99)
#define RSSI_IN_R4(x) ((x) >= -105 && (x) < -102)

#define RSSI_IN_R1_OR_R2(x) (RSSI_IN_R1(x) || RSSI_IN_R2(x))
#define RSSI_IN_R3_OR_R4(x) (RSSI_IN_R3(x) || RSSI_IN_R4(x))
#define RSSI_IN_R1_OR_R2_OR_R3(x) (RSSI_IN_R1(x) || RSSI_IN_R2(x) || RSSI_IN_R3(x))

/* ECIO ranges */
#define ECIO_IN_E1(x) ((x) >= -5)
#define ECIO_IN_E2(x) ((x) >= -10 && (x) < -5)
#define ECIO_IN_E3(x) ((x) >= -13 && (x) < -10)
#define ECIO_IN_E4(x) ((x) >= -16 && (x) < -13)

/*=============================================================================
FUNCTION: GetWCDMADataNumRSSIDisplayBars

DESCRIPTION:
  Return the number of RSSI bars to display per the new UI signal strength 
  indicator algorithm

SIDE EFFECTS:
  None.

=============================================================================*/
static uint32 GetWCDMADataNumRSSIDisplayBars(CCoreApp *pMe)
{
  int16 rssi;
  uint32 num_bars = ANNUN_STATE_RSSI_0;

  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  rssi = pMe->m_ssInfo.rssi2;

  /* Data call */
  if (pMe->m_ssInfo.ecio_for_display)
  {
     int16 ecio = pMe->m_ssInfo.ecio;

     if (RSSI_IN_R1_OR_R2(rssi) && ECIO_IN_E1(ecio))
     {
        num_bars = ANNUN_STATE_RSSI_4;
     }
     else if ((RSSI_IN_R3_OR_R4(rssi) && ECIO_IN_E1(ecio)) ||
              (RSSI_IN_R1_OR_R2(rssi) && ECIO_IN_E2(ecio)))
     {
        num_bars = ANNUN_STATE_RSSI_3;
     }
     else if ((RSSI_IN_R3(rssi) && ECIO_IN_E2(ecio)) ||
              (RSSI_IN_R1_OR_R2_OR_R3(rssi) && ECIO_IN_E3(ecio)) ||
              (RSSI_IN_R1_OR_R2(rssi) && ECIO_IN_E4(ecio)))
     {
        num_bars = ANNUN_STATE_RSSI_2;
     }
     else if (RSSI_IN_R3(rssi) && ECIO_IN_E3(ecio))
     {
        num_bars = ANNUN_STATE_RSSI_1;
     }
  }
  /* Voice call */
  else
  {
     if (RSSI_IN_R1(rssi))
     {
        num_bars = ANNUN_STATE_RSSI_4;
     }
     else if (RSSI_IN_R2(rssi))
     {
        num_bars = ANNUN_STATE_RSSI_3;
     }
     else if (RSSI_IN_R3(rssi))
     {
        num_bars = ANNUN_STATE_RSSI_2;
     }
     else if (RSSI_IN_R4(rssi))
     {
        num_bars = ANNUN_STATE_RSSI_1;
     }
  }

  return num_bars;
}
#endif

/*=============================================================================
FUNCTION: RefreshRSSIAnnunciator

DESCRIPTION:
  Refreshes the RSSI annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

void RefreshRSSIAnnunciator(CCoreApp *pMe, boolean serviceChange)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI,ANNUN_STATE_OFF);

#ifdef FEATURE_HDR
  if (serviceChange)
  {
    ISHELL_CancelTimer(pMe->a.m_pIShell, CoreApp_ToggleHybridRSSI, (void*) pMe);
  }
  /* HYBRID_SRV_AVAIL checks for (hybrid mode available) AND 
     (service status == AEECM_SRV_STATUS_LIMITED OR AEECM_SRV_STATUS_SRV) 
     AND (HDR is available). The annunciator will not be turned on and the timer 
     will not be restarted if the service status is AEECM_SRV_STATUS_NO_SRV or
     AEECM_SRV_STATUS_PWR_SAVE */
  if (HYBRID_SRV_AVAIL(pMe->m_phInfo, pMe->m_ssInfo))
  {    
    if (serviceChange)
    {
      pMe->m_bHDRDisplay = FALSE;
      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI,
                             (unsigned long) DBToLevel(pMe->m_ssInfo.rssi));
      ISHELL_SetTimer(pMe->a.m_pIShell, TOGGLE_HYBRID_RSSI_TIMER,
                      CoreApp_ToggleHybridRSSI, (void*) pMe);
    }
  }
  else
#endif
  /* SYS_MODE_GW_1X  checks for (service status == AEECM_SRV_STATUS_LIMITED OR 
     AEECM_SRV_STATUS_SRV) AND (HDR is not available). The annunciator will not be 
     turned on if the service status is AEECM_SRV_STATUS_NO_SRV or
     AEECM_SRV_STATUS_PWR_SAVE */
  if (SYS_MODE_GW_1X(pMe->m_ssInfo.sys_mode, pMe->m_ssInfo.srv_status))
  {
     uint32 num_bars;

#if defined(FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY)
     num_bars = GetWCDMADataNumRSSIDisplayBars(pMe);
#else
     num_bars = DBToLevel(pMe->m_ssInfo.rssi);
#endif

     IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RSSI, num_bars);
  }
#ifdef FEATURE_HDR
  else if (SYS_MODE_HDR(pMe->m_ssInfo.sys_mode, pMe->m_ssInfo.hdr_srv_status))
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_HDR_RSSI,
                           Calculate_HDR_RSSI(pMe->m_ssInfo.hdr_rssi, 
                                              pMe->m_ssInfo.hdr_sinr));
  }
#endif
}


/*=============================================================================
FUNCTION: RefreshWLANRSSIAnnunciator

DESCRIPTION:
  Refreshes the WLAN RSSI annunciator using current WLAN service status

SIDE EFFECTS:
  None.

=============================================================================*/
#ifdef FEATURE_WLAN

static void RefreshWLANRSSIAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WLAN_RSSI, ANNUN_STATE_OFF);
  if (SRV_STATUS_AVAIL(pMe->m_ssInfo.wlan_srv_status))
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_WLAN_RSSI,
                           (unsigned long)DBToLevel(pMe->m_ssInfo.wlan_rssi));
  }
}
#endif /*#ifdef FEATURE_WLAN*/

/*=============================================================================
FUNCTION: RefreshInUseAnnunciator

DESCRIPTION:
  Refreshes the In Use annunciator using current system service status and ICM event

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshInUseAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  if (SRV_STATUS_AVAIL(pMe->m_ssInfo.srv_status) ||
      SRV_STATUS_AVAIL(pMe->m_ssInfo.hdr_srv_status))
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_OFF);
    if(pMe->m_phInfo.is_in_use)
    {
      IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_IN_USE);
    }
  }
  else if (((pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_NO_SRV) ||
            (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_PWR_SAVE))
           &&
           ((pMe->m_ssInfo.hdr_srv_status == AEECM_SRV_STATUS_NO_SRV) ||
            (pMe->m_ssInfo.hdr_srv_status == AEECM_SRV_STATUS_PWR_SAVE)))
  {
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_NO_SVC);
  }
}

#if defined(FEATURE_HDR) && defined (CM_API_HDR_ACTIVE_PROT)
/*=============================================================================
FUNCTION: RefreshHDRProtAnnunciator

DESCRIPTION:
  Refreshes the HDR Annunciator based on the Protocol Revision

SIDE EFFECTS:
  None.

=============================================================================*/
static void RefreshHDRProtAnnunciator(CCoreApp *pMe, uint8 annunciatorState)
{
   switch (annunciatorState) {
      case ANNUN_STATE_HDR_REV_A:
      case ANNUN_STATE_HDR:
         if ( AEECM_SYS_ACTIVE_PROT_HDR_RELA == pMe->m_ssInfo.hdr_active_prot) {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HDR_REV_A);    
         }
         else if (AEECM_SYS_ACTIVE_PROT_HDR_REL0 == pMe->m_ssInfo.hdr_active_prot) {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HDR);
         }
         else
         {
            CORE_MSG("Unknown HDR Protocol Rev",0,0,0);
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_OFF);
         }
         break;

      case ANNUN_STATE_HYBRID_REV_A:
      case ANNUN_STATE_HYBRID:
         if ( AEECM_SYS_ACTIVE_PROT_HDR_RELA == pMe->m_ssInfo.hdr_active_prot) {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HYBRID_REV_A);
         }
         else if (AEECM_SYS_ACTIVE_PROT_HDR_REL0 == pMe->m_ssInfo.hdr_active_prot) {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HYBRID);
         }
         else
         {
            CORE_MSG("Unknown HDR Protocol Rev",0,0,0);
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_OFF);
         }
         break;

      default:
      break;
  }
}
#endif //FEATURE_HDR && CM_API_HDR_ACTIVE_PROT

/*=============================================================================
FUNCTION: RefreshModeAnnunciator

DESCRIPTION:
  Refreshes the Mode annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshModeAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_OFF);
  if (SRV_STATUS_AVAIL(pMe->m_ssInfo.srv_status))
  {
    switch(pMe->m_ssInfo.sys_mode)
    {
      case AEECM_SYS_MODE_AMPS:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_ANALOG);
        break;

      case AEECM_SYS_MODE_CDMA:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_CDMA);
        break;

      case AEECM_SYS_MODE_GSM:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_GSM);
        break;

      case AEECM_SYS_MODE_WCDMA:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_WCDMA); 
        break;

      case AEECM_SYS_MODE_GPS:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_GPS);
        break;

      default:
        break;
    }
  }

  /* For HDR service icon, we have to look if HDR is available first and then
     check if hybrid which is a special case can override the standard HDR icon 
  */
  
#ifdef FEATURE_HDR
  if(HDR_SRV_AVAIL(pMe->m_phInfo, pMe->m_ssInfo))
  {

#ifdef CM_API_HDR_ACTIVE_PROT
/* This feature is defined so distinguish between Rel0 and RelA Icon */
     RefreshHDRProtAnnunciator(pMe,ANNUN_STATE_HDR_REV_A);

#else
    /* Feature not defined Just Display Plain old Icon */
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HDR);
#endif

    /* If this is a hybrid system we are restricting the search to look 
       for both CDMA AND HDR Availability and overwriting the icon if needed */
  if(HYBRID_SRV_AVAIL (pMe->m_phInfo, pMe->m_ssInfo))
  {
#ifdef CM_API_HDR_ACTIVE_PROT
     /* This feature is defined to distinguish between Rel0 and RelA Icon */
     RefreshHDRProtAnnunciator(pMe,ANNUN_STATE_HYBRID_REV_A);

#else
    IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_MODE, ANNUN_STATE_HYBRID);
#endif
    }
  }
#endif //FEATURE_HDR
}



/*=============================================================================
FUNCTION: RefreshDomainAnnunciator

DESCRIPTION:
  Refreshes the Domain annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshDomainAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_OFF);

  /* Showing domain is only necessary for new service events */
  if (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_SRV)
  {
    if (SYS_MODE_GW(pMe->m_ssInfo.sys_mode, pMe->m_ssInfo.srv_status))
    {
      switch(pMe->m_ssInfo.srv_capability)
      {
        case AEECM_SRV_CAPABILITY_CS_SERVICE_ONLY:
          if(pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_CS_ONLY ||
             pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_CS_PS)
          {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_CS);
          }
          break;

        case AEECM_SRV_CAPABILITY_PS_SERVICE_ONLY:
          if(pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_PS_ONLY ||
             pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_CS_PS)
          {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_PS);
          }
          break;

        case AEECM_SRV_CAPABILITY_CS_PS_SERVICE:
          if(pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_CS_ONLY)
          {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_CS);
          }
          else if(pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_PS_ONLY)
          {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_PS);
          }
          else if(pMe->m_ssInfo.srv_domain == AEECM_SRV_DOMAIN_CS_PS)
          {
            IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_DOMAIN, ANNUN_STATE_CS_PS);
          }
          break;

        default:
          break;
      }
    }
  }
}


/*=============================================================================
FUNCTION: RefreshRoamAnnunciator

DESCRIPTION:
  Refreshes the Roam annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshRoamAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_OFF);

  /* Showing roam is not necessary for limited service */
  if (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_SRV)
  {
    switch (pMe->m_ssInfo.roam_status)
    {
      case AEECM_ROAM_STATUS_OFF:
        /* Annunciator is already off */
        break;

      case AEECM_ROAM_STATUS_ON:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_ON);
        break;

      default:
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ROAM, ANNUN_STATE_ON|ANNUN_STATE_BLINK);
        break;
    }
  }
}


/*=============================================================================
FUNCTION: RefreshOperatorAnnunciator

DESCRIPTION:
  Refreshes the Operator/PLMN annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshOperatorAnnunciator(CCoreApp *pMe)
{     
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (pMe->m_pIAnn == NULL)
  {
    CORE_MSG("Null annunciator",0,0,0);
    return;
  }

  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, ANNUN_STATE_OFF);
  if (SRV_STATUS_LIMITED(pMe->m_ssInfo.srv_status))
  {
    //Display "Limited " in place of PLMN ID.
    AECHAR psLimitedSrv[MAX_PLMN_ANNUN_SIZE];

    ISHELL_LoadResString (pMe->a.m_pIShell,
                          gpszResourceFile,
                          IDS_LIMITED,
                          psLimitedSrv,
                          MAX_PLMN_ANNUN_SIZE*sizeof(AECHAR));
    IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_OPERATOR, psLimitedSrv);
  }
#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
  else if (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_SRV)
  {
    if(!CCoreApp_DisplayPLMNAnn(pMe))
    {
      CORE_ERR("Unable to display PLMN Id", 0, 0, 0);
    }
  }
#endif
}

/*=============================================================================
FUNCTION: RefreshCellSvcsAnnunciator

DESCRIPTION:
  Refresh the HSDPA/HSUPA annunciator.

SIDE EFFECTS:
  None.

=============================================================================*/
static void RefreshCellSvcsAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  switch(pMe->m_ssInfo.cell_srv_ind.hs_ind)
  {
  case AEECM_SYS_HS_IND_HSDPA_SUPP_CELL:
    /* Turn on the annunciator that indicates HSDPA service available */
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_HSDPA, ANNUN_STATE_HSDPA);
    break;
  case AEECM_SYS_HS_IND_HSUPA_SUPP_CELL:
    /* Turn on the annunciator that indicates HSUPA service available */
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_HSDPA, ANNUN_STATE_HSUPA);
    break;
  case AEECM_SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL:
    /* Turn on the annunciator that indicates both HSDPA/HSUPA service available */
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_HSDPA, ANNUN_STATE_HSDPA_HSUPA);
    break;
  case AEECM_SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL:
    /* Turn off the HSDPA/HSUPA annunciator region */
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_HSDPA, ANNUN_STATE_OFF);
    break;
  default:
    MSG_ERROR("Invalid hsdpa_hsupa_ind value: %d",pMe->m_ssInfo.cell_srv_ind.hs_ind,0,0);
  }

}

/*=============================================================================
FUNCTION: RefreshBcmcsAnnunciator

DESCRIPTION:
  Refresh the BCMCS annunciator.

SIDE EFFECTS:
  None.

=============================================================================*/
static void RefreshBcmcsAnnunciator(CCoreApp *pMe)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if ( pMe->m_ssInfo.bcmcs_hdr_srv_status == AEECM_BCMCS_SRV_STATUS_SRV ||
       pMe->m_ssInfo.bcmcs_main_srv_status == AEECM_BCMCS_SRV_STATUS_SRV )
  {
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_BCMCS, ANNUN_STATE_ON);
  }
  else
  {
    IANNUNCIATOR_SetField(pMe->m_pIAnn , ANNUN_FIELD_BCMCS, ANNUN_STATE_OFF);
  }

}

/*=============================================================================
FUNCTION: RefreshAnnunciators

DESCRIPTION:
  Refreshes annunciators using current ICM events

SIDE EFFECTS:
  None.

=============================================================================*/

static void RefreshAnnunciatorsOnServiceChange(CCoreApp *pMe)
{
  // Nine annunciators can change: 
  // 1. RSSI
  RefreshRSSIAnnunciator(pMe, TRUE);
#ifdef FEATURE_WLAN
  // 2. WLAN RSSI
  RefreshWLANRSSIAnnunciator(pMe);
#endif
  // 3. In Use 
  RefreshInUseAnnunciator(pMe);
  // 4. Mode 
  RefreshModeAnnunciator(pMe);
  // 5. Domain
  RefreshDomainAnnunciator(pMe);
  // 6. Roam
  RefreshRoamAnnunciator(pMe);
  // 7. PLMN
  RefreshOperatorAnnunciator(pMe);
  // 8. HSDPA/HSUPA
  RefreshCellSvcsAnnunciator(pMe);
  // 9. BCMCS
  RefreshBcmcsAnnunciator(pMe);

}

/*=============================================================================
FUNCTION: DisplayPowerSaveMode

DESCRIPTION:
  Displays power save mode.  Called from timer.

SIDE EFFECTS:
  None.

=============================================================================*/
static void DisplayPowerSaveMode (CCoreApp *pMe)
{
  AECHAR psBannerDisplay[BANNER_BUFFER_SIZE];

  memset(psBannerDisplay, 0, sizeof(psBannerDisplay));

  if (pMe == NULL)
  {
    CORE_ERR("Null pMe",0,0,0);
    return;
  }

  (void)ISHELL_LoadResString (pMe->a.m_pIShell,
                              gpszResourceFile,
                              IDS_POWER_SAVE_MODE,
                              psBannerDisplay,
                              BANNER_BUFFER_SIZE*sizeof(AECHAR));
  if (pMe->m_pIAnn != NULL) {
    IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, psBannerDisplay);
  }
}

/*=============================================================================
FUNCTION: HandlePowerSaveMode

DESCRIPTION:
  Displays power save after a delay if bEnter is TRUE.  Goes out of 
  power save if bEnter is FALSE.

SIDE EFFECTS:
  None.

=============================================================================*/

static void HandlePowerSaveMode(CCoreApp *pMe, boolean bEnter)
{
  if (pMe == NULL)
  {
    CORE_MSG("Null pMe",0,0,0);
    return;
  }

  if (bEnter)
  {
    //Don't set the timer if it's already set.
    if (ISHELL_GetTimerExpiration(pMe->a.m_pIShell,
                    (PFNNOTIFY) DisplayPowerSaveMode,
                    pMe) == 0)
    {
      (void) ISHELL_SetTimer(pMe->a.m_pIShell,
                             2 * 60 * 1000, //2 minutes
                             (PFNNOTIFY) DisplayPowerSaveMode,
                             pMe);
    }
  }
  else
  {
    (void) ISHELL_CancelTimer(pMe->a.m_pIShell, 
                              (PFNNOTIFY) DisplayPowerSaveMode, 
                              pMe);
    (void) GetBanner(pMe);
  }
}

/*===========================================================================
FUNCTION IsConfigCtrlReady

DESCRIPTION
  Check to see if config or control has invalid value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IsConfigCtrlReady(CCoreApp *pMe)
{
  if (NULL == pMe)
  {
    CORE_MSG("Null pMe",0,0,0);
    return FALSE;
  }

  /* check if we have valid config and control */
  if ((pMe->m_rtreConfig == AEECM_RTRE_CONFIG_NONE) ||
      (pMe->m_rtreControl == AEECM_RTRE_CONTROL_NONE)) {
    return FALSE;
  }

  return TRUE;
} /* IsConfigCtrlReady */

/*=============================================================================
FUNCTION: ProcessSubscriptionStatus

DESCRIPTION:
  Look at various parameters to see the subscription status and send it to CM

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_ProcessSubscriptionStatus (CCoreApp *pMe)
{
  boolean cdma_avail = TRUE, gsm_avail = TRUE;
  boolean umts_avail = TRUE;
  boolean sendsubscription_failed = FALSE;
#ifndef FEATURE_UI_INDIVIDUAL_STATUS
  AEECMSUB cdma_status, gw_status; /* Status sent to ICM */
#endif /* FEATURE_UI_INDIVIDUAL_STATUS */
  int     nReturnStatus;
  uint8   byNewSubMask = COREAPP_BOTH_SUBS_AVAIL_MASK;

#ifndef FEATURE_MMGSDI_DUAL_SLOT
  IDialog *pIDialog;
#endif

  if (pMe == NULL)
  {
    CORE_ERR("Null pMe ptr", 0, 0, 0);
    return;
  }

  /* Don't bother if we don't have a config and control yet */
  if (IsConfigCtrlReady(pMe) == FALSE) {
    return;
  }

  nReturnStatus = PopulateCardApplications(pMe);
  if (nReturnStatus != SUCCESS) {
    return;
  }

  /* gsm subscription */
  gsm_avail = IsSubscriptionAvail(pMe, COREAPP_GSM);

  /* umts subscription */
  umts_avail = IsSubscriptionAvail(pMe, COREAPP_UMTS);

  if (!gsm_avail && !umts_avail) {
    byNewSubMask &= ~COREAPP_GW_SUBS_AVAIL_MASK;
  }
  else {
    gsm_avail = TRUE;
  }
#ifdef FEATURE_MMGSDI_PERSONALIZATION
  switch (pMe->m_nPersoStatus)
  {
    case AEECARD_PERSO_NETWORK_CONTROL_BLOCKED:
    case AEECARD_PERSO_NETWORK_SUBSET_BLOCKED:
    case AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED:
    case AEECARD_PERSO_CORPORATE_BLOCKED:
    case AEECARD_PERSO_SIM_BLOCKED:
    case AEECARD_PERSO_FAILED:
      byNewSubMask &= ~COREAPP_GW_SUBS_AVAIL_MASK;
      gsm_avail = FALSE;
      break;
    default:
      break;
  }
#endif /*FEATURE_MMGSDI_PERSONALIZATION */

  /* cdma subscription */
  /* check rtre */
  if (((pMe->m_rtreConfig == AEECM_RTRE_CONFIG_RUIM_OR_DROP_BACK) &&
    (pMe->m_rtreControl == AEECM_RTRE_CONTROL_RUIM)) ||
    (pMe->m_rtreConfig == AEECM_RTRE_CONFIG_RUIM_ONLY)
#ifdef FEATURE_UIM_RUIM_W_GSM_ACCESS
      || (pMe->m_rtreConfig == AEECM_RTRE_CONFIG_SIM_ACCESS)
#endif /* FEATURE_UIM_RUIM_W_GSM_ACCESS */
    )
  { /* cdma has to be from the card */
    cdma_avail = IsSubscriptionAvail(pMe, COREAPP_CDMA);
#ifdef FEATURE_MMGSDI_PERSONALIZATION
    switch (pMe->m_nPersoStatus)
    {
      case AEECARD_PERSO_NETWORK_CONTROL_BLOCKED:
      case AEECARD_PERSO_NETWORK_SUBSET_BLOCKED:
      case AEECARD_PERSO_SERVICE_PROVIDER_BLOCKED:
      case AEECARD_PERSO_CORPORATE_BLOCKED:
      case AEECARD_PERSO_SIM_BLOCKED:
      case AEECARD_PERSO_FAILED:
        cdma_avail = FALSE;
        break;
      default:
        break;
    }
#endif /*FEATURE_MMGSDI_PERSONALIZATION */
  }

  if (!cdma_avail) {
    byNewSubMask &= ~COREAPP_CDMA_SUBS_AVAIL_MASK;
  }


  if ((pMe->m_bySubscriptionSent & COREAPP_NOT_READY_SUBSCRIPTION_MASK) != COREAPP_NOT_READY_SUBSCRIPTION_MASK) {
    /* has already sent a subscription before */
    /* check if we are trying to send down the same subscription */
    /* Don't send CM command if offline */
    if (byNewSubMask == pMe->m_bySubscriptionSent) {
#ifndef FEATURE_MMGSDI_DUAL_SLOT
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);

        if (byNewSubMask == 0x00) 
        {
          /*If SAP Is connected, then we should not go to NO Card
            Calls are still not possible in this mode except emergency because 
            there is no subscription available.*/
#if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)
            ICARD_GetCardStatus(pMe->m_pICard, (uint8 *) &(pMe->m_nCardStatus));
            if(pMe->m_nCardStatus == AEECARD_SAP_SERVER_CONNECTED || 
               pMe->m_nCardStatus == AEECARD_SAP_CLIENT_CONNECTED)
            {
              if (pIDialog == NULL) {
                /* start the idle menu */
                (void) CreateIdleMenuDlg(pMe);
              }
              return;
            }
#endif/*(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/

            /* check if the screen is already an emergency screen */
            if (pIDialog == NULL) 
            {
              /* Create emergency screen */
              nReturnStatus = ISHELL_CreateDialog(pMe->a.m_pIShell,
                              gpszResourceFile,  IDD_NOCARD_EMERGENCY,  NULL);
            }
        }
        else {/* has at least some services */
            if (pIDialog == NULL) {
              /* start the idle menu */
              (void) CreateIdleMenuDlg(pMe);
            }
        }
#endif /*#ifndef FEATURE_MMGSDI_DUAL_SLOT*/
        return;
    }
  }
  else {
    /* remove the not ready mask */
    /* set the mask to all zero */
    pMe->m_bySubscriptionSent = 0;
  }

#ifdef FEATURE_UI_INDIVIDUAL_STATUS
  /* Feature to use original (multiple) ICM_SetSubscription call. */
  /* don't send either subscription if that is the current state of that sub */
  /* This generates CM + ICM errors */
  /* If these functions don't return AEE_SUCCESS, either we've sent the sub already, */
  /* or we're in the wrong operating mode. */
  if ((byNewSubMask & COREAPP_CDMA_SUBS_AVAIL_MASK) !=
      (pMe->m_bySubscriptionSent & COREAPP_CDMA_SUBS_AVAIL_MASK)) {
    if (AEE_SUCCESS != ICM_SetSubscriptionStatus(pMe->m_pICM,
                           AEECM_SYS_MODE_CDMA,
                           cdma_avail))
    {
      sendsubscription_failed = TRUE;
      MSG_HIGH("SetSubscriptionStatus failed.  Wrong mode %d?", pMe->m_phInfo.oprt_mode, 0, 0);
    }
  }

  if ((byNewSubMask & COREAPP_GW_SUBS_AVAIL_MASK) !=
      (pMe->m_bySubscriptionSent & COREAPP_GW_SUBS_AVAIL_MASK)) {
    if (AEE_SUCCESS != ICM_SetSubscriptionStatus(pMe->m_pICM,
                           AEECM_SYS_MODE_GSM,
                           gsm_avail))
    {
      sendsubscription_failed = TRUE;
      MSG_HIGH("SetSubscriptionStatus failed.  Wrong mode %d?", pMe->m_phInfo.oprt_mode, 0, 0);
    }
  }

#else /* FEATURE_UI_INDIVIDUAL_STATUS */
  /* Set the cdma_status, gw_status, and wlan_status all according to
     what is desired. */
  cdma_status = cdma_avail?AEECM_SUB_AVAILABLE:AEECM_SUB_NOT_AVAILABLE;
  gw_status = gsm_avail?AEECM_SUB_AVAILABLE:AEECM_SUB_NOT_AVAILABLE;

  /* Tell ICM to set the subscription status. */
  if (AEE_SUCCESS != ICM_SetSubscriptionStatusEx(pMe->m_pICM,
        cdma_status, gw_status, AEECM_SUB_NO_CHANGE))
  {
    sendsubscription_failed = TRUE;
    MSG_ERROR("SetSubscriptionStatusEx failed.  cdma %d, gsm %d",
        cdma_status, gw_status, 0);
    MSG_ERROR("SetSubscriptionStatusEx failed (pt2).  operating mode %d",
        pMe->m_phInfo.oprt_mode, 0, 0);
  }
#endif /* FEATURE_UI_INDIVIDUAL_STATUS */

  if (!sendsubscription_failed) {
    pMe->m_bySubscriptionSent = byNewSubMask;
  }
  /* don't recreate dialog if LPM dialog is up */
  if(!pMe->m_bLPM)
  {
    if (ISHELL_GetActiveDialog(pMe->a.m_pIShell) != NULL) {
      return;
    }
    /* If there are no current active dialogs then try to see if we
    can enter the idle menu dialog */
    (void) CreateIdleMenuDlg(pMe);
  }
}

/*=============================================================================
FUNCTION: CoreApp_ProcessOffLine

DESCRIPTION:
  Display the corresponding message for offline mode

SIDE EFFECTS:
  None.

=============================================================================*/
boolean CoreApp_ProcessOffLine(CCoreApp *pMe)
{
  AECHAR psSvcReq[BANNER_BUFFER_SIZE];

  if (pMe == NULL)
  {
    CORE_ERR("Null pMe Ptr", 0, 0, 0);
    return FALSE;
  }
  if (((pMe->m_dwSecurityMask & COREAPP_PIN1_CODE_MASK) != COREAPP_PIN1_CODE_MASK) &&
      ((pMe->m_dwSecurityMask & COREAPP_NV_CODE_MASK) != COREAPP_NV_CODE_MASK)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      && ((pMe->m_dwSecurityMask & COREAPP_PIN1_SLOT2_CODE_MASK) != COREAPP_PIN1_SLOT2_CODE_MASK)
#endif
      ) { /* security init hasn't been called yet...  */
    if (!IsConfigCtrlReady(pMe)) {
      pMe->m_rtreConfig = AEECM_RTRE_CONFIG_NV_ONLY;
      pMe->m_rtreControl = AEECM_RTRE_CONTROL_NV;
    }
  }
  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_SVCREQD,
                  psSvcReq,
                  BANNER_BUFFER_SIZE*sizeof(AECHAR));

  if (pMe->m_pIAnn != NULL) {
    IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, psSvcReq);
    return TRUE;
  }

  CORE_ERR("Null Ann Ptr", 0, 0, 0);
  return FALSE;

}

/*=============================================================================
FUNCTION: CoreApp_ProcessFTMMode

DESCRIPTION:
  Display the corresponding message for FTM mode

SIDE EFFECTS:
  None.

=============================================================================*/
static boolean CoreApp_ProcessFTMMode(CCoreApp *pMe)
{
  AECHAR psFTMDisplay[BANNER_BUFFER_SIZE];

  if (pMe == NULL)
  {
    CORE_ERR("Null pMe ptr", 0, 0, 0);
    return FALSE;
  }
  ISHELL_LoadResString (pMe->a.m_pIShell,
                  gpszResourceFile,
                  IDS_FACTORYTEST,
                  psFTMDisplay,
                  BANNER_BUFFER_SIZE*sizeof(AECHAR));
  if (pMe->m_pIAnn != NULL) {
    IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, psFTMDisplay);

#if defined (FEATURE_APP_FLDDBG)
          // Launch the debug screen of the field debug app.
      if((ISHELL_StartAppletArgs(pMe->a.m_pIShell, AEECLSID_FIELDDEBUGAPP, "FTM")) != AEE_SUCCESS) {
        CORE_ERR("Could not start FldDbg App", 0, 0, 0);
      }
#endif

    return TRUE;
  }
  CORE_ERR("Null Ann Ptr", 0, 0, 0);
  return FALSE;

}

/*=============================================================================
FUNCTION: CoreApp_ProcessICMEvents

DESCRIPTION:
  Process ICM Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
boolean CoreApp_ProcessICMEvents(CCoreApp *pMe, uint16 wParam, uint32 dwParam)
{
  AECHAR psBannerDisplay[BANNER_BUFFER_SIZE];
  IDialog *pIDialog;
  uint16 wDlgID;
  AEECMNotifyInfo *event_info = (((AEENotify *) dwParam)->pData);

  boolean svc_changed;

  CORE_MSG("Coreapp: received ICM event.  Mask 0x%x", ((AEENotify*)dwParam)->dwMask,0,0);//tbd

  switch (((AEENotify*)dwParam)->dwMask)
  {
    case NMASK_CM_PHONE:
      /* copy the ph info to the pMe structure */
      /* Update our local PH Info struct */
      if (&event_info->event_data.ph != NULL) {
        /*Check to see if the previous mode was emergency and current mode is not*/
        if (pMe->m_phInfo.mode_pref == AEECM_MODE_PREF_EMERGENCY &&
           ((AEECMPhInfo*)&(event_info->event_data.ph))->mode_pref != AEECM_MODE_PREF_EMERGENCY)
        {
            /*Get out of emergency callback mode - Just need to update the banner*/
            (void)GetBanner(pMe);
        }
        (void) MEMCPY(&(pMe->m_phInfo), (AEECMPhInfo*)&(event_info->event_data.ph),
              sizeof (AEECMPhInfo));
      }

      switch(event_info->event)
      {
      case AEECM_EVENT_PH_SYSTEM_PREF:       /* System preference changed */
        if (pMe->m_phInfo.mode_pref == AEECM_MODE_PREF_EMERGENCY) {
          // Update annunciator to show that we are in E911 CB mode
          ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_EMERGENCY,
                    psBannerDisplay,
                    BANNER_BUFFER_SIZE*sizeof(AECHAR));
          if (pMe->m_pIAnn != NULL) {
            IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, psBannerDisplay);
          }
        }
        return TRUE;

      case AEECM_EVENT_PH_SUBSCRIPTION:      /* Subscription info has changed. */
      case AEECM_EVENT_PH_CDMA_LOCK_MODE:     /* CDMA lock mode was changed. */
      case AEECM_EVENT_PH_FUNDS_LOW:
      case AEECM_EVENT_PH_NAM_SEL:            /* NAM selection was changed */
      case AEECM_EVENT_PH_CURR_NAM:           /* Current NAM was changed. */
        /* do nothing */
        return TRUE;

      case AEECM_EVENT_PH_ANSWER_VOICE:       /* Answer voice as data was changed. */
        DisplayMessageDlg(pMe, IDS_ANSWER_VOICE_AS_DATA_SUCCESS, 3);
        return TRUE;

      case AEECM_EVENT_PH_STANDBY_SLEEP:      /* Entering powerdown sleep mode */
      case AEECM_EVENT_PH_STANDBY_WAKE:       /* Exiting powerdown sleep mode */
        //we'll get sleep/wake events for every scan.
        //only go in+out of pwr save on service events
        return TRUE;

      case AEECM_EVENT_PH_MAINTREQ:           /* CDMA maintenance required command. */
        {
          AECHAR *pBuf = (AECHAR *) MALLOC ((MAX_BROADCAST_MSG_SIZE + 1)*sizeof(AECHAR));
          if(pBuf == NULL)
          {
            CORE_ERR("Unable to malloc" , 0, 0, 0);
            return FALSE;
          }
          ISHELL_LoadResString (pMe->a.m_pIShell,
                      gpszResourceFile,
                      IDS_MAINTENANCE_REQ,
                      pBuf,
                      MAX_BROADCAST_MSG_SIZE*sizeof(AECHAR));
          if (! SetIdleMenuText(pBuf))
          {
            CORE_ERR("Could not set idle menu text for maint req.", 0, 0, 0);
          }
          FREE(pBuf);
        }
        return TRUE;

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
      case AEECM_EVENT_PH_NVRUIM_CONFIG:
        /* CM event NV RUIM event */

        pMe->m_rtreConfig = event_info->event_data.ph.rtre_config;
        pMe->m_rtreControl = event_info->event_data.ph.rtre_control;

        /* Check if both PINs verified */
        if (((pMe->m_dwSecurityMask & COREAPP_PIN1_CODE_MASK) != COREAPP_PIN1_CODE_MASK) ||
            ((pMe->m_dwSecurityMask & COREAPP_NV_CODE_MASK) != COREAPP_NV_CODE_MASK)
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            || ((pMe->m_dwSecurityMask & COREAPP_PIN1_SLOT2_CODE_MASK) != COREAPP_PIN1_SLOT2_CODE_MASK)
#endif
            )
        {
          ValidateCardsAndChangeState(pMe,FALSE);
        }
        else
        {
          ISHELL_PostEventEx(pMe->a.m_pIShell, EVTFLG_ASYNC, AEECLSID_CORE_APP,
                    EVT_USER, EVT_SUBSCRIPTION_CHANGED,0L);
        }
        return TRUE;
#endif /*#ifdef FEATURE_UIM_RUN_TIME_ENABLE*/

      case AEECM_EVENT_PH_IN_USE_STATE:       /* In use state was changed */
        if (pMe->m_pIAnn == NULL){
          return FALSE;
        }

        if (event_info->event_data.ph.is_in_use) {
          UpdateBacklight(pMe);
          IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_IN_USE);
        }
        else {
          IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_INUSE, ANNUN_STATE_OFF);
        }
        return TRUE;

      case AEECM_EVENT_PH_INFO_AVAIL:         /* Phone information is now available */
        CORE_MSG("Coreapp: Got AEECM_EVENT_PH_INFO_AVAIL",0,0,0);
        /* Start provisioning only once */
        if (!pMe->m_bProvisioned)
          InitAfterPhInfo(pMe, event_info->event_data.ph.oprt_mode);
        return TRUE;

      case AEECM_EVENT_PH_OPRT_MODE:  /* Operating mode was changed */
        /* if current dlg == TC Mode, send the events to that dialog first */
        CORE_MSG("Coreapp: Got AEECM_EVENT_PH_OPRT_MODE, mode %d",  event_info->event_data.ph.oprt_mode,0,0);
        pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
        wDlgID = 0; /* initialize to zero */
        if (pIDialog != NULL)
        {
          wDlgID = IDIALOG_GetID(pIDialog);
        }
        if (wDlgID == IDD_TC_TEST_MODE) {
          CoreApp_InvokeDlgEventHandler(wDlgID, pMe, EVT_NOTIFY, wParam, dwParam);
        }
        /* continue processing of the event, e.g., update banner etc
            this is done regardless of whether we are in the Test Mode
            dialog or not */
        switch (event_info->event_data.ph.oprt_mode)
        {
        case AEECM_OPRT_MODE_FTM:
          return CoreApp_ProcessFTMMode(pMe);
        case AEECM_OPRT_MODE_OFFLINE:
          return CoreApp_ProcessOffLine(pMe);
        case AEECM_OPRT_MODE_ONLINE:
          /* if we just went online, check subscription */
          CoreApp_ProcessSubscriptionStatus(pMe);
          
          (void) GetBanner(pMe);
          if(pMe->m_bLPM )
          {
            pMe->m_bLPM = FALSE;
            EndLPMMode(pMe, FALSE);
          }
          return TRUE;
        case AEECM_OPRT_MODE_LPM:
          // Update annunciator
          if(pMe->m_bLPM)
          {
            if (! CreateLPMDlg(pMe))
            {
              CORE_ERR("Could not create LPM dialog.", 0, 0, 0);
            }
            return TRUE;
          }
          ISHELL_LoadResString (pMe->a.m_pIShell,
                    gpszResourceFile,
                    IDS_GAME_MODE,
                    psBannerDisplay,
                    BANNER_BUFFER_SIZE*sizeof(AECHAR));
          if (pMe->m_pIAnn != NULL) {
            IANNUNCIATOR_SetFieldEx(pMe->m_pIAnn, ANNUN_FIELD_BANNER, psBannerDisplay);
          }
          return TRUE;
#if (defined(FEATURE_WCDMA)|| defined(FEATURE_GSM))
#ifdef FEATURE_TC
        case AEECM_OPRT_MODE_NET_TEST_GW:
          /* only process this event in the main handler if the current dialog is not
              the TC Test mode dialog */
          if (wDlgID != IDD_TC_TEST_MODE) {
            /* first time receive this event, needs to create the TC Test Mode dialog */
            /* End all the dialogs */
            (void) DisplayIdleMenuDlg(pMe);
            /* Create TC Test Mode Dialog */
            ISHELL_CreateDialog(pMe->a.m_pIShell, gpszResourceFile,  IDD_TC_TEST_MODE,  NULL);
          }
          return TRUE;
#endif
#endif
        default:
          /* unhandled OPRT events */
          return FALSE;
        }

      default:
        break;
      }
      return FALSE;

  case NMASK_CM_SS:

      svc_changed = (boolean)(pMe->m_ssInfo.srv_status !=
                              event_info->event_data.ss.ss_info.srv_status);

      /* copy the SS info to the pMe structure */
      if (&event_info->event_data.ss != NULL) {
        (void) MEMCPY(&(pMe->m_ssInfo), 
                      (AEECMSSInfo*)&(event_info->event_data.ss.ss_info),
                      sizeof (AEECMSSInfo));
#ifdef FEATURE_USRSYSCLOCK
        CoreUtils_UpdatePLMNCFGI(pMe);
#endif // FEATURE_USRSYSCLOCK
      }

      CORE_ERR("Coreapp: Got ICM SS event 0x%x", event_info->event,0,0);
      switch (event_info->event)
      {
        case AEECM_EVENT_SS_SRV_CHANGED:          /* Serving system information changed */
          MSG_HIGH("Service Status = %d",pMe->m_ssInfo.srv_status,0,0);

#ifdef FEATURE_HDR
          if (HYBRID_MODE_AVAIL(pMe->m_phInfo))
          {
            if ((pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_PWR_SAVE) ||
                (pMe->m_ssInfo.hdr_srv_status == AEECM_SRV_STATUS_PWR_SAVE))
            {
              CoreTask_SetDeepSleep(TRUE);
            }
            else
            {
              CoreTask_SetDeepSleep(FALSE);
            }
          }
          else
#endif /*FEATURE_HDR*/
          {
            if (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_PWR_SAVE)
            {
              CoreTask_SetDeepSleep(TRUE);
            }
            else
            {
              CoreTask_SetDeepSleep(FALSE);
            }
          }

          switch(pMe->m_ssInfo.srv_status)
          {
          case AEECM_SRV_STATUS_LIMITED_REGIONAL:
            CORE_MSG("Received AEECM_SRV_STATUS_LIMITED_REGIONAL",0,0,0);
            //fallthrough
          case AEECM_SRV_STATUS_LIMITED:
            CORE_MSG("Received AEECM_SRV_STATUS_LIMITED",0,0,0);

            HandlePowerSaveMode(pMe, FALSE);

            /* Play beep if reqd */
            if(pMe->m_bServiceAlert && !pMe->m_phInfo.is_in_use && svc_changed)
            {
              uisnd_snd(UI_SVC_ALERT_SND, SND_SVC_ACQ);
            }
#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
#ifdef TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
            ISHELL_CancelTimer(pMe->a.m_pIShell, CoreUtil_AbortPPPDataSession, (void*) pMe);
#endif /* TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP */
#endif  /* FEATURE_HDR_SESSION_TIED_TO_PPP */

            RefreshAnnunciatorsOnServiceChange(pMe);
            //See if the time has been updated.
            DisplayTimeWithRefresh(pMe);
            break;

          case AEECM_SRV_STATUS_SRV:
            CORE_MSG("Received AEECM_SRV_STATUS_SRV",0,0,0);

            HandlePowerSaveMode(pMe, FALSE);

            /* Play beep if reqd */
            if(pMe->m_bServiceAlert && !pMe->m_phInfo.is_in_use && svc_changed)
            {
              uisnd_snd(UI_SVC_ALERT_SND, SND_SVC_ACQ);
            }

#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
#ifdef TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
            ISHELL_CancelTimer(pMe->a.m_pIShell, CoreUtil_AbortPPPDataSession, (void*) pMe);
#endif /* TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP */
#endif  /* FEATURE_HDR_SESSION_TIED_TO_PPP */

            RefreshAnnunciatorsOnServiceChange(pMe);

            if(pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_SRV)
            {
              pIDialog = ISHELL_GetActiveDialog(pMe->a.m_pIShell);
              if(pIDialog != NULL && IDIALOG_GetID(pIDialog) == IDD_SCAN_NETWORK)
              {
                ISHELL_EndDialog(pMe->a.m_pIShell);
              }
            }
            //See if the time has been updated.
            DisplayTimeWithRefresh(pMe);
            break;

          case AEECM_SRV_STATUS_NO_SRV:
            CORE_MSG("Received AEECM_SRV_STATUS_NO_SRV",0,0,0);
          case AEECM_SRV_STATUS_PWR_SAVE:
            CORE_MSG("Received AEECM_SRV_STATUS_PWR_SAVE or AEECM_SRV_STATUS_NO_SRV",0,0,0);

#ifdef FEATURE_HDR
            if (HYBRID_MODE_AVAIL(pMe->m_phInfo))
            {
              if ((pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_PWR_SAVE) &&
                  (pMe->m_ssInfo.hdr_srv_status == AEECM_SRV_STATUS_PWR_SAVE))
              {
                HandlePowerSaveMode(pMe, TRUE);
              }
            }
            else
#endif /*FEATURE_HDR*/
            {
              if (pMe->m_ssInfo.srv_status == AEECM_SRV_STATUS_PWR_SAVE)
              {
                HandlePowerSaveMode(pMe, TRUE);
              }
            }

            /* Play beep if reqd */
            if(pMe->m_bServiceAlert && 
               !pMe->m_phInfo.is_in_use && 
               svc_changed &&
               (pMe->m_ssInfo.srv_status != AEECM_SRV_STATUS_PWR_SAVE)
              )
            {
              uisnd_snd(UI_SVC_ALERT_SND, SND_SVC_LOST);
            }

#ifdef FEATURE_HDR_SESSION_TIED_TO_PPP
#ifdef TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP
            if((ds707_extif_pkt_is_dormant()) ||
              ((pMe->m_pICM != NULL) && 
               (AEECM_IS_XCALL_PRESENT(pMe->m_pICM,
                                       (AEECMCallType)(AEECM_CALL_TYPE_PS_DATA|
                                                       AEECM_CALL_TYPE_CS_DATA)))))
            {
              if((!pMe->m_ssInfo.hdr_hybrid) ||
                 (pMe->m_ssInfo.hdr_hybrid && (pMe->m_ssInfo.hdr_srv_status == AEECM_SRV_STATUS_NO_SRV)))
              {
                ISHELL_SetTimer(pMe->a.m_pIShell, HDR_ABORT_PPP_TIMER,
                                CoreUtil_AbortPPPDataSession, (void*) pMe);
              }
            }
#endif /* TEST_ONLY_FEATURE_UI_HDR_ABORT_PPP */
#endif  /* FEATURE_HDR_SESSION_TIED_TO_PPP */

            RefreshAnnunciatorsOnServiceChange(pMe);
            break;

          default:
            break;

          }

          return TRUE;

        case AEECM_EVENT_SS_RSSI:                 /* RSSI changed */
          CORE_MSG("Received AEECM_EVENT_SS_RSSI",0,0,0);
          RefreshRSSIAnnunciator(pMe, FALSE);
          return TRUE;

        case AEECM_EVENT_SS_HDR_RSSI:
          CORE_MSG("Received AEECM_EVENT_SS_HDR_RSSI",0,0,0);
#ifdef FEATURE_HDR
          if ((HYBRID_SRV_AVAIL(pMe->m_phInfo, pMe->m_ssInfo)) ||
              (SYS_MODE_HDR(pMe->m_ssInfo.sys_mode, pMe->m_ssInfo.hdr_srv_status)))
          {
            RefreshRSSIAnnunciator(pMe, FALSE);
          }
          else
          {
            MSG_HIGH("EVT_SS_HDR_RSSI received with no HDR srv", 0 , 0, 0);
          }
#endif
          return TRUE;

        case AEECM_EVENT_SS_WLAN_RSSI:
          CORE_MSG("Received AEECM_EVENT_SS_WLAN_RSSI",0,0,0);
#ifdef FEATURE_WLAN
          RefreshWLANRSSIAnnunciator(pMe);
#else
          CORE_ERR("Received bad WLAN RSSI", 0, 0, 0);
#endif
          return TRUE;

        default:
          return TRUE;
      }

    default:
      break;
  }
  return FALSE;
}

