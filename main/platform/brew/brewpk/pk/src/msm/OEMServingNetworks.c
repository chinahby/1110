/*=============================================================================

FILE: OEMServingNetworks.c

SERVICES: OEM Serving Networks Functions

GENERAL DESCRIPTION:
   A reference implementation of the OEMServingNetworks functions declared in
   IOEMServingNetworkst.h that are used in the implementation of IServingNetworks.

PUBLIC CLASSES AND STATIC FUNCTIONS:

   IOEServingNetworks

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None.

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"
#include "AEECriticalSection.h"

#include "AEEISignalCBFactory.h"
#include "AEESignalCBFactory.bid"
#include "OEMTelephone.h"

#include "err.h" 

#include "OEMServingNetworks.h"
#include "AEETelephone.h"  

#include "cm.h"


#define FARF_OEMSN_ERROR   1
#define FARF_OEMSN_HIGH    1
#define FARF_OEMSN_MED     1
#define FARF_OEMSN_LOW     1

#ifdef MSG_ERROR 
   #undef MSG_ERROR
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif


#ifdef FARF
 #undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 


#define MSG_ERROR( sf, a, b, c )    FARF(OEMSN_ERROR, (sf, a, b, c))
#define MSG_HIGH( sf, a, b, c )     FARF(OEMSN_ERROR, ("*****"sf, a, b, c))
#define MSG_MED( sf, a, b, c )      FARF(OEMSN_MED, (sf, a, b, c))
#define MSG_LOW( sf, a, b, c )      FARF(OEMSN_LOW, (sf, a, b, c))

// All available networks information/description
typedef struct OEMSNNwDesc {
   AEETNetworkID     wlanNwid;
   AEETWLANDesc      wlanDesc;
   boolean           bWlanValid;
   
   AEETNetworkID     hdrNwid; 
   AEETHDRDesc       hdrDesc;
   boolean           bHdrValid;
   
   AEETNetworkID     cdmaNwid;   
   AEETCDMADesc      cdmaDesc;
   boolean           bCdmaValid;
   
   AEETNetworkID     gwNwid;   
   AEETGWDesc        gwDesc;
   boolean           bGwValid;
}OEMSNNwDesc;


struct _OEMServingNetworks {
   uint32                               dwRef;
   IShell                              *piShell;
   
   ISignalBus                          *piSignalBus;
   uint8                                currMaxNwid;  
   /* used to allocate network id*/
   OEMSNNwDesc                          nwDesc; 
    
   ISignalCtl                          *piSignalCtlSS;
   ISignal                             *piSignalSS;
   AEECallback                          cbOnExit;   
   
   OEMTelephone                        *pOemTelephone;
};


static OEMServingNetworks *gpOEMSN = NULL;

static boolean gbOEMSNInit = FALSE;


extern int OEMTelephone_GetSSDetails(OEMTelephone *pme, cm_mm_ss_info_s_type *pSSInfo,
                                     int nSize, cm_ss_event_e_type *pSSEvt);

extern int OEMTelephone_DeregisterSSEvts(OEMTelephone *pme);

extern int OEMTelephone_RegisterSSEvts(OEMTelephone *pme, ISignal *piSignal);


static void OEMServingNetworks_CopyNwInfo(OEMServingNetworks *pMe,
                                                const cm_mm_ss_info_s_type *src);
static void OEMServingNetworks_UpdateGwNw(const cm_mm_ss_info_s_type *src,
                                         AEETGWDesc *pgw_info );

static void OEMServingNetworks_UpdateCdmaNw(const cm_mm_ss_info_s_type *src,
                                           AEETCDMADesc *pcdma_info );
static AEETNetworkID OEMServingNetworks_GetNwId(OEMServingNetworks *pMe);

#ifdef CM_API_WLAN 
static void OEMServingNetworks_UpdateWlanNw(const cm_mm_ss_info_s_type *src, 
                                           AEETWLANDesc *pwlan_info );
#endif
static void OEMServingNetworks_UpdateCdmaNw(const cm_mm_ss_info_s_type *src,
                                           AEETCDMADesc *pcdma_info );

static void OEMServingNetworks_UpdateHdrNw(const cm_mm_ss_info_s_type *src,
                                          AEETHDRDesc *phdr_info );

/* ========================================================================
   ======================================================================== */



/* ========================================================================
   ======================================================================== */
static void OEMServingNetworks_HandleSSEvent(void *po)
{
   OEMServingNetworks* pSN = NULL;
   boolean bNotify = TRUE;
   cm_mm_ss_info_s_type ssInfo;
   cm_ss_event_e_type cm_ss_event;
   int nResult;
   
   
   MSG_HIGH("OEMServingNetworks_HandleSSEvent", 0, 0, 0);

   pSN =  (OEMServingNetworks*)po;
   if(NULL == pSN){
      return;
   }
   
   nResult = OEMTelephone_GetSSDetails(pSN->pOemTelephone, &ssInfo,
                             sizeof(cm_mm_ss_info_s_type), &cm_ss_event);
   if(SUCCESS != nResult) {
      return;
   }
      
   // updating all serving network information
   switch(cm_ss_event){
      case CM_SS_EVENT_INFO:
      case CM_SS_EVENT_SRV_CHANGED:
         MSG_HIGH("OEMServingNetworks CM_SS_EVENT_SRV_CHANGED", 0, 0, 0);
         OEMServingNetworks_CopyNwInfo(pSN, &ssInfo);
         break;
      case CM_SS_EVENT_RSSI:
         switch(ssInfo.sys_mode){
            case SYS_SYS_MODE_CDMA:
               pSN->nwDesc.cdmaDesc.usRssi = ssInfo.rssi;
               break;
            case SYS_SYS_MODE_GSM:
            case SYS_SYS_MODE_WCDMA:
            case SYS_SYS_MODE_GW:
               pSN->nwDesc.gwDesc.usRssi = ssInfo.rssi;
               break;         
            case SYS_SYS_MODE_HDR:
               pSN->nwDesc.hdrDesc.usRssi = ssInfo.rssi;
               break;       
         }
#ifdef FEATURE_HDR_HYBRID
      case CM_SS_EVENT_HDR_RSSI:
            if(ssInfo.hdr_hybrid ){
               pSN->nwDesc.hdrDesc.usRssi = ssInfo.hdr_rssi;
            }
          break;
#endif

#ifdef CM_API_WLAN                
      case CM_SS_EVENT_WLAN_RSSI:
            if(ssInfo.hdr_hybrid ){
               pSN->nwDesc.hdrDesc.usRssi = ssInfo.hdr_rssi;
            }
#endif

          break;
      default:
          bNotify = FALSE;
          break;
   }
     
   if(bNotify){
      ISignalBus_Strobe(pSN->piSignalBus); 
   }
   
   if(NULL != pSN->piSignalCtlSS) {
      ISignalCtl_Enable(pSN->piSignalCtlSS);
   }
   
}
   

/* ========================================================================
   ======================================================================== */
static void OEMServingNetworks_CopyNwInfo(OEMServingNetworks *pMe,
                                                const cm_mm_ss_info_s_type *src)
{
  /* Updating CDMA network information */ 
   MSG_HIGH("OEMServingNetworks_CopyNwInfo", 0, 0, 0);
                  
   if( SYS_SYS_MODE_CDMA == src->sys_mode ){
      if( !pMe->nwDesc.cdmaNwid ){
         pMe->nwDesc.cdmaNwid = OEMServingNetworks_GetNwId(pMe);
      }

      OEMServingNetworks_UpdateCdmaNw(src, &pMe->nwDesc.cdmaDesc );
      pMe->nwDesc.bCdmaValid = TRUE;
   }
   else{
      pMe->nwDesc.bCdmaValid = FALSE;   
   }

  /* Updating GSM/UMTS network information */
   if( ( SYS_SYS_MODE_GSM == src->sys_mode ) || (SYS_SYS_MODE_WCDMA == src->sys_mode )
         || (SYS_SYS_MODE_GW == src->sys_mode ) ) {
      if( ! pMe->nwDesc.gwNwid ){
         pMe->nwDesc.gwNwid = OEMServingNetworks_GetNwId(pMe);
      }

      OEMServingNetworks_UpdateGwNw(src, &pMe->nwDesc.gwDesc );
      pMe->nwDesc.bGwValid = TRUE;
   }
   else{
      pMe->nwDesc.bGwValid = FALSE;   
   }
   
  /* Updating WLAN network information */
#ifdef CM_API_WLAN
   if( !pMe->nwDesc.wlanNwid ){
      pMe->nwDesc.wlanNwid = OEMServingNetworks_GetNwId(pMe);
   }

   OEMServingNetworks_UpdateWlanNw(src, &pMe->nwDesc.wlanDesc );
   pMe->nwDesc.bWlanValid = TRUE;
#else 
   pMe->nwDesc.bWlanValid = FALSE;  
#endif

  /* Updating HDR network information */
   if(AEET_SYS_MODE_HDR == src->sys_mode || src->hdr_hybrid ){
      if( !pMe->nwDesc.hdrNwid ){
            pMe->nwDesc.hdrNwid = OEMServingNetworks_GetNwId(pMe);
         }
      OEMServingNetworks_UpdateHdrNw(src, &pMe->nwDesc.hdrDesc); 
      pMe->nwDesc.bHdrValid = TRUE;
   }
   else {
      pMe->nwDesc.bHdrValid = FALSE; 
   }

}




/* ========================================================================
   ======================================================================== */
/* Allocate network id from 1 - 255 */
static AEETNetworkID OEMServingNetworks_GetNwId(OEMServingNetworks *pMe)
{
   MSG_HIGH("OEMServingNetworks_GetNwId", 0, 0, 0);
   do{
     pMe->currMaxNwid++;
     // check for pre allocated nwid values, so no nwid assigned to two networks
     if(  !( ( pMe->currMaxNwid == pMe->nwDesc.wlanNwid) ||
             ( pMe->currMaxNwid == pMe->nwDesc.hdrNwid ) || 
             ( pMe->currMaxNwid == pMe->nwDesc.gwNwid  ) ||
             ( pMe->currMaxNwid == pMe->nwDesc.cdmaNwid) ) )  {
        break;
     }
     
     if(pMe->currMaxNwid >= 255 ){
        pMe->currMaxNwid = 0;
     }
   }while(1);


   return ((AEETNetworkID)(pMe->currMaxNwid));
}


/*=============================================================================
FUNCTION: OEMServingNetworks_UpdateWlanNw

DESCRIPTION:  
   Update WLAN network information

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
#ifdef CM_API_WLAN 
static void OEMServingNetworks_UpdateWlanNw(const cm_mm_ss_info_s_type *src, 
                                           AEETWLANDesc *pwlan_info )
{
  
   int size = 0;
   pwlan_info->sysMode = AEET_SYS_MODE_WLAN;
   pwlan_info->srvStatus = (AEETSrvStatus)src->wlan_srv_status;
   MSG_HIGH("OEMServingNetworks_UpdateWlanNw", 0, 0, 0);
    
#ifdef CM_API_EMERGENCY_NUM_LIST   
   pwlan_info->bandClass = (AEETSysBandClass)src->active_band;
#endif 
   size = MIN(sizeof(AEETWLANSysID),sizeof(sys_sys_id_u_type)); 
   (void) MEMCPY(&pwlan_info->ssid,&src->sys_id.id, size);
   
  pwlan_info->chan = (AEETChanType)src->wlan_bss_info.chan;
  pwlan_info->bssType = (AEETWlanBssType) src->wlan_bss_info.bss_type;
  pwlan_info->bssid = (AEETWlanBssid)src->wlan_bss_info.bssid;
  pwlan_info->usRssi = src->wlan_bss_info.rssi;
  pwlan_info->tech = (AEETWlanTech) src->wlan_bss_info.tech;
    
}
#endif

/*=============================================================================
FUNCTION: OEMServingNetworks_UpdateCdmaNw

DESCRIPTION:  
 Update CDMA network information 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMServingNetworks_UpdateCdmaNw(const cm_mm_ss_info_s_type *src,
                                           AEETCDMADesc *pcdma_info )
{
   MSG_HIGH("OEMServingNetworks_UpdateCdmaNw", 0, 0, 0);

   pcdma_info->sysMode = AEET_SYS_MODE_CDMA;
   pcdma_info->srvStatus =  (AEETSrvStatus)src->srv_status;
   pcdma_info->srvDomain = (AEETSrvDomain)src->srv_domain;
   pcdma_info->srvCapability = (AEETSrvCapability)src->srv_capability;
   pcdma_info->roamStatus = (AEETRoamStatus)src->roam_status;
   pcdma_info->usRssi = src->rssi;
   
   MEMCPY(&pcdma_info->is95, &src->sys_id.id.is95, sizeof(AEETIS95SysID) ) ;
   pcdma_info->bCcsSupported = src->mode_info.cdma_info.ccs_supported ;
   pcdma_info->bRegistered = src->mode_info.cdma_info.is_registered;
   pcdma_info->bandClass = (AEETSysBandClass)src->mode_info.cdma_info.band_class;
   pcdma_info->nUzId = src->mode_info.cdma_info.uz_id;
   (void) MEMCPY(&pcdma_info->uzName, &src->mode_info.cdma_info.uz_name,
                    sizeof(AEETUZName));
   pcdma_info->blockSystem =(AEETSysBlocks) src->mode_info.cdma_info.block_or_system;
   pcdma_info->usPacketZone = src->mode_info.cdma_info.packet_zone;
   pcdma_info->ucBsPRev = src->mode_info.cdma_info.bs_p_rev;
   pcdma_info->ucPRevInUse = src->mode_info.cdma_info.p_rev_in_use;
}

/*=============================================================================
  =============================================================================*/
/* copy of cm_util_ussd_unpack modified to use const byte* packed_data */
static uint32 OEMServingNetworks_prv_util_ussd_unpack(
    byte* str, 
    const byte* packed_data, 
    uint32 num_bytes
)
{
  uint32 stridx = 0;
  uint32 pckidx = 0;
  byte prev = 0;
  byte curr = 0;
  byte shift;

  while(pckidx < num_bytes)
  {
    shift = (byte)(stridx & 0x07);
    curr = packed_data[pckidx++];
    
    /* A 7-bit character can be split at the most between two bytes of packed
    ** data.
    */
    str[stridx++] = ( (curr << shift) | (prev >> (8-shift)) ) & 0x7F;
    
    /* Special case where the whole of the next 7-bit character fits inside
    ** the current byte of packed data.
    */
    if(shift == 6)
    {
      /* If the next 7-bit character is a CR (0x0D) and it is the last
      ** character, then it indicates a padding character. Drop it.
      */

      if((curr >> 1) == 0x0D && pckidx == num_bytes)
      {
        break;
      }
      str[stridx++] = curr >> 1;
    }

    prev = curr;
  }
  
  return stridx;
} /* cm_util_ussd_unpack() */


/*=============================================================================
  =============================================================================*/
/* return number of unpacked characters */
static __inline
uint32 OEMServingNetworks_Unpack7bitChars(byte *pDest, uint32 uDest, 
                                    const byte *pSrc, uint32 uSrc)
{
   uint32 uRet = (uSrc * 8)/7;

   uRet = MIN(uDest, uRet);

   OEMServingNetworks_prv_util_ussd_unpack(pDest, pSrc, (7 * (uRet >> 3)) + (uRet & 0x07));

   return uRet;
}

/*=============================================================================
  =============================================================================*/
static __inline
void OEMServingNetworks_CopyInNWName(AEETNWName *dest, 
                               const sys_network_full_name_s_type *src)
{
   dest->coding_scheme = (AEETNWCodingType)src->coding_scheme;
   dest->add_country_initials = src->add_country_initials;

   if (AEET_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET 
       == dest->coding_scheme) {
      dest->length = (byte)OEMServingNetworks_Unpack7bitChars(dest->name, AEET_MAX_NW_NAME_CHARS,
                                                  src->name, src->length);
   }
   else {
      unsigned octets = MIN(src->length, AEET_MAX_NW_NAME_CHARS);
      dest->length = (byte)(octets >>1);
      (void) MEMCPY(dest->name, src->name, octets);
   }
}


/*=============================================================================
FUNCTION: OEMServingNetworks_CopyInMMInfo

DESCRIPTION:  
  Copy in Mobility Mangement Informtion

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMServingNetworks_CopyInMMInfo(AEETMMInfo *mmInfo, 
                               const sys_mm_information_s_type *mm_info)
{

  mmInfo->plmn_avail                = mm_info->plmn_avail;
  mmInfo->full_name_avail           = mm_info->full_name_avail;
  mmInfo->short_name_avail          = mm_info->short_name_avail;
  mmInfo->time_and_time_zone_avail  = mm_info->univ_time_and_time_zone_avail;
  mmInfo->time_zone_avail           = mm_info->time_zone_avail;
  mmInfo->lsa_identity_avail        = mm_info->lsa_identity_avail;

  if (mm_info->plmn_avail)
    (void) MEMCPY(&mmInfo->plmn, &mm_info->plmn, sizeof(AEETPLMNID));

  if (mm_info->full_name_avail){
     OEMServingNetworks_CopyInNWName(&mmInfo->full_name, &mm_info->full_name);
  }
  
  if (mm_info->short_name_avail){
      OEMServingNetworks_CopyInNWName(&mmInfo->short_name, 
                         (sys_network_full_name_s_type*)&mm_info->short_name);
   }

  if (mm_info->univ_time_and_time_zone_avail) {
    JulianType julian_time;

    julian_time.wYear   = 2000+mm_info->univ_time_and_time_zone.year; /* GT 2000 */
    julian_time.wMonth  = mm_info->univ_time_and_time_zone.month;
    julian_time.wDay    = mm_info->univ_time_and_time_zone.day;
    julian_time.wHour   = mm_info->univ_time_and_time_zone.hour;
    julian_time.wMinute = mm_info->univ_time_and_time_zone.minute;
    julian_time.wSecond = mm_info->univ_time_and_time_zone.second;
    julian_time.wWeekDay = 0;

    /* Get the week day populated */
    GETJULIANDATE(JULIANTOSECONDS(&julian_time), &julian_time);
    (void) MEMCPY(&mmInfo->time_and_time_zone.time, &julian_time,
                  sizeof(JulianType));
    mmInfo->time_and_time_zone.time_zone = (AEETTimeZone)
                        mm_info->univ_time_and_time_zone.time_zone;
  }

  if (mm_info->daylight_saving_adj_avail) {
    mmInfo->daylight_saving_adj = (int8)mm_info->daylight_saving_adj;
  }
  else {
     mmInfo->daylight_saving_adj = 0;
  }

  if (mm_info->time_zone_avail)
    mmInfo->time_zone = (AEETTimeZone) mm_info->time_zone;

  if (mm_info->lsa_identity_avail) {
    mmInfo->lsa_identity.type = (AEETLSAIdentityType)
                                 mm_info->lsa_identity.type;
    (void) MEMCPY(mmInfo->lsa_identity.identity,
                  mm_info->lsa_identity.identity, 3);
  }
  
}

/*=============================================================================
FUNCTION: OEMServingNetworks_UpdateGwNw

DESCRIPTION:  
   Update GSM/UMTS network information

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMServingNetworks_UpdateGwNw(const cm_mm_ss_info_s_type *src,
                                         AEETGWDesc *pgw_info )
{
   MSG_HIGH("OEMServingNetworks_UpdateGwNw", 0, 0, 0);

   pgw_info->sysMode = (AEETSysMode)src->sys_mode;
   pgw_info->srvStatus =  (AEETSrvStatus)src->srv_status;
   pgw_info->srvDomain = (AEETSrvDomain)src->srv_domain;
   pgw_info->srvCapability = (AEETSrvCapability)src->srv_capability;
   pgw_info->roamStatus = (AEETRoamStatus)src->roam_status;
   pgw_info->usRssi = src->rssi;
   
   (void*)MEMMOVE(&pgw_info->plmn, &src->sys_id.id.plmn, sizeof(AEETPLMNID)) ;
   pgw_info->simState = (AEETSIMState) src->mode_info.gw_info.sim_state;
   pgw_info->bPlmnForbidden = src->mode_info.gw_info.plmn_forbidden;
   
   OEMServingNetworks_CopyInMMInfo(&pgw_info->mmInformation,
                        &src->mode_info.gw_info.mm_information);
#ifdef CM_API_CELL_SUPP_SERVICES
   pgw_info->bCcsSupported = (src->cell_srv_ind.dtm_supp == SYS_DTM_SUPPORT_AVAIL);
#endif   

}

/*=============================================================================
FUNCTION: OEMServingNetworks_UpdateHdrNw

DESCRIPTION:  
   Update HDR(EVDO) network information

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMServingNetworks_UpdateHdrNw(const cm_mm_ss_info_s_type *src,
                                          AEETHDRDesc *phdr_info )
{
   MSG_HIGH("OEMServingNetworks_UpdateHdrNw", 0, 0, 0);

   phdr_info->sysMode = AEET_SYS_MODE_HDR;

   if(AEET_SYS_MODE_HDR == src->sys_mode ){
      phdr_info->srvStatus =  (AEETSrvStatus)src->srv_status;
      phdr_info->srvDomain = (AEETSrvDomain)src->srv_domain;
      phdr_info->srvCapability = (AEETSrvCapability)src->srv_capability; 
      phdr_info->usRssi = src->rssi;
      phdr_info->roamStatus = (AEETRoamStatus)src->roam_status;
      (void*)MEMMOVE(phdr_info->ucaIs856, &src->sys_id.id.is856, sizeof(phdr_info->ucaIs856) ) ;
      // Will be used when EVDO rev A and rev B will be available
   }
   else{
#ifdef FEATURE_HDR
      phdr_info->srvStatus =  (AEETSrvStatus)src->hdr_srv_status;
      phdr_info->usRssi = src->hdr_rssi;
      phdr_info->roamStatus = (AEETRoamStatus)src->hdr_roam_status;
      (void*)MEMMOVE(phdr_info->ucaIs856, &src->hdr_sys_id.id.is856, sizeof(phdr_info->ucaIs856) ) ;      
#else
      phdr_info->roamStatus = AEET_ROAM_STATUS_OFF;
      phdr_info->srvStatus = AEET_SRV_STATUS_NO_SRV;   
#endif   
   }
   
}



/* ========================================================================
   ======================================================================== */
int 
OEMServingNetworks_GetSrvNetworks(OEMServingNetworks* pme, AEETNwInfo *pNwInfo, 
                                  int nSize, int *pAvailableNw)
{
   int nAvail = 0;
   int arrSize = nSize / sizeof(AEETNwInfo);
   
   
   if( NULL == pme || NULL == pAvailableNw || (0 == arrSize && NULL != pNwInfo)){
      return AEE_EBADPARM;
   }
   
   /* Get the number of available networks */
   if( pme->nwDesc.bCdmaValid && 
       pme->nwDesc.cdmaDesc.srvStatus > AEET_SRV_STATUS_NO_SRV ){
      
      if(arrSize > 0 && NULL != pNwInfo ){
         pNwInfo->nwid = pme->nwDesc.cdmaNwid;
         pNwInfo->sysMode = pme->nwDesc.cdmaDesc.sysMode;
         pNwInfo->srvStatus = pme->nwDesc.cdmaDesc.srvStatus;
         arrSize--;
         pNwInfo++;         
      }
      nAvail++;
   }
   if( pme->nwDesc.bGwValid && 
       pme->nwDesc.gwDesc.srvStatus > AEET_SRV_STATUS_NO_SRV ){

      if(arrSize > 0 && NULL != pNwInfo ){
         pNwInfo->nwid = pme->nwDesc.gwNwid;
         pNwInfo->sysMode = pme->nwDesc.gwDesc.sysMode;
         pNwInfo->srvStatus = pme->nwDesc.gwDesc.srvStatus;
         arrSize--;
         pNwInfo++;         
      }
      nAvail++;
   }

   if( pme->nwDesc.bHdrValid && 
       pme->nwDesc.hdrDesc.srvStatus >  AEET_SRV_STATUS_NO_SRV ){

      if(arrSize > 0 && NULL != pNwInfo){
         pNwInfo->nwid = pme->nwDesc.hdrNwid;
         pNwInfo->sysMode = pme->nwDesc.hdrDesc.sysMode;
         pNwInfo->srvStatus = pme->nwDesc.hdrDesc.srvStatus;
         arrSize--;
         pNwInfo++;         
      }
      nAvail++;
   }
   
   if( pme->nwDesc.bWlanValid && 
       pme->nwDesc.wlanDesc.srvStatus >  AEET_SRV_STATUS_NO_SRV ){

      if(arrSize > 0 && NULL != pNwInfo ){
         pNwInfo->nwid = pme->nwDesc.wlanNwid;
         pNwInfo->sysMode = pme->nwDesc.wlanDesc.sysMode;
         pNwInfo->srvStatus = pme->nwDesc.wlanDesc.srvStatus;
         arrSize--;
         pNwInfo++;
      }
      nAvail++;
   }
   
   *pAvailableNw = nAvail;


   return AEE_SUCCESS;  
   
}



/* ========================================================================
   ======================================================================== */
int 
OEMServingNetworks_GetNwInfo(OEMServingNetworks* pme, AEETNetworkID nwid,
                             char *pBuff, int buffLen )
{
   int nResult = AEE_SUCCESS;
   
   if(NULL == pBuff || buffLen <= 0 || nwid == 0 ){
      return EBADPARM;
   }
   
   /* copy the AEETXXXDesc for given nwid in pBuff */
   if(pme->nwDesc.wlanNwid == nwid && pme->nwDesc.bWlanValid){
      (void ) MEMMOVE((void*)pBuff, (void*)&pme->nwDesc.wlanDesc, 
                                   MIN(buffLen , sizeof(AEETWLANDesc)));
   }
   else if(pme->nwDesc.hdrNwid == nwid && pme->nwDesc.bHdrValid ){
      (void ) MEMMOVE((void*)pBuff, (void*)&pme->nwDesc.hdrDesc, 
                                   MIN(buffLen , sizeof(AEETHDRDesc)));
   }
   else if(pme->nwDesc.gwNwid == nwid && pme->nwDesc.bGwValid){
      (void ) MEMMOVE((void*)pBuff, (void*)&pme->nwDesc.gwDesc, 
                                   MIN(buffLen , sizeof(AEETGWDesc)));
   }
   else if(pme->nwDesc.cdmaNwid == nwid && pme->nwDesc.bCdmaValid){
      (void ) MEMMOVE((void*)pBuff, (void*)&pme->nwDesc.cdmaDesc, 
                                   MIN(buffLen , sizeof(AEETCDMADesc)));
   }
   else{
      nResult = EBADPARM;
   }   
    
   return nResult;   
}



/* ========================================================================
   ======================================================================== */
int 
OEMServingNetworks_RegisterSignal(OEMServingNetworks* pme, ISignal *piSignal)
{
   if(NULL == piSignal){
      return EBADPARM;
   }

   return (ISignalBus_Add(pme->piSignalBus, piSignal));
}


/* ========================================================================
   ======================================================================== */
static int OEMServingNetworks_CtorZ(OEMServingNetworks* pme, IShell *pIShell)
{
   int nErr;
   ISignalCBFactory * piSigCBFactory = NULL;
   cm_mm_ss_info_s_type ssInfo;
   cm_ss_event_e_type ssEvt;
   PACONTEXT pacLast;
   
   pme->piShell = pIShell;
   ISHELL_AddRef(pme->piShell);
   
   DBGPRINTF("OEMServingNetworks_CtorZ");
   
   pacLast = AEE_EnterAppContext(NULL);

   nErr = ISHELL_CreateInstance(pme->piShell , AEECLSID_SignalBus, (void **)&pme->piSignalBus);
   if (AEE_SUCCESS != nErr) {
      goto bail;
   }
   
   nErr = ISHELL_CreateInstance(pme->piShell, AEECLSID_SignalCBFactory, (void**) &piSigCBFactory);
   if(SUCCESS != nErr){
      goto bail;   
   }
   
   nErr = ISignalCBFactory_CreateSignal(piSigCBFactory, OEMServingNetworks_HandleSSEvent, pme,
                                          &pme->piSignalSS , &pme->piSignalCtlSS); 
   if(SUCCESS != nErr){
      goto bail;   
   }
   
   // Initialize AMSS Call manage
   nErr = OEMTelephone_Open(&pme->pOemTelephone);
   if(SUCCESS != nErr){
      goto bail;
   }
   AEE_LeaveAppContext(pacLast);
   
   OEMTelephone_RegisterSSEvts(pme->pOemTelephone, pme->piSignalSS);
   
   nErr = OEMTelephone_GetSSDetails(pme->pOemTelephone, &ssInfo, sizeof(ssInfo), &ssEvt); 
   if(SUCCESS != nErr){
      OEMTelephone_DeregisterSSEvts(pme->pOemTelephone);
      goto bail;
   }
   
   OEMServingNetworks_CopyNwInfo(pme, &ssInfo);
       
   IQI_RELEASEIF(piSigCBFactory);
   return nErr;
   
bail:
   AEE_LeaveAppContext(pacLast);
   IQI_RELEASEIF(pme->piSignalBus);
   IQI_RELEASEIF(pme->piSignalSS);
   IQI_RELEASEIF(pme->piSignalCtlSS);
   return nErr;
}


/* ========================================================================
   ======================================================================== */
static void OEMServingNetworks_Delete(OEMServingNetworks *pme)
{
   if(NULL == pme){
      return;
   }

   CALLBACK_Cancel(&pme->cbOnExit);
   
   if(NULL != pme->pOemTelephone){
      OEMTelephone_DeregisterSSEvts(pme->pOemTelephone);
   
      OEMTelephone_Close(pme->pOemTelephone);
      pme->pOemTelephone = NULL;
   }
     
   if(NULL != pme->piSignalBus) {   
      ISignalBus_Clear(pme->piSignalBus);
   }
   
   IQI_RELEASEIF(pme->piSignalBus);
   if(NULL != pme->piSignalCtlSS) {
      ISignalCtl_Detach(pme->piSignalCtlSS);
      IQI_RELEASEIF(pme->piSignalCtlSS);
   }
   
   IQI_RELEASEIF(pme->piSignalSS);
   IQI_RELEASEIF(pme->piShell);

   gbOEMSNInit = FALSE;
   
   if(gpOEMSN == pme){
      gpOEMSN = NULL;
   }
   
   sys_free(pme);
}


/* ========================================================================
   ======================================================================== */
void OEMServingNetworks_Close(OEMServingNetworks *pme)
{
   if(0 == --pme->dwRef){
      OEMServingNetworks_Delete(pme);
   }
}

/* ========================================================================
   ======================================================================== */
int OEMServingNetworks_Open(OEMServingNetworks **ppObject)
{
   int nErr = SUCCESS;
   OEMServingNetworks *pme = gpOEMSN;   
   
   DBGPRINTF("OEMServingNetworks_Open");
   
   if (NULL != pme) {
      pme->dwRef++;
      *ppObject = pme;
      return SUCCESS; 
   }

   pme = (OEMServingNetworks*)sys_malloc( sizeof(OEMServingNetworks) );
   if (NULL == pme) {
      return ENOMEMORY;
   }
   
   nErr = OEMServingNetworks_CtorZ(pme, AEE_GetShell());
   if(SUCCESS != nErr){
      goto bail;
   }
   pme->dwRef++;

   gpOEMSN = *ppObject = pme;
   gbOEMSNInit = TRUE;
   
   CALLBACK_Init( &pme->cbOnExit, OEMServingNetworks_Delete, pme ); 
   AEE_RegisterSystemCallback( &pme->cbOnExit, AEE_SCB_AEE_EXIT, 0);

 bail:

   if (SUCCESS != nErr) {
      sys_free(pme);
      OEMServingNetworks_Close(pme);
   }
   return nErr;
}
