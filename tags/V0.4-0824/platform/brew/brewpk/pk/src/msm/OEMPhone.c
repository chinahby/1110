/*======================================================
FILE:  OEMPhone.c

SERVICES:  OEM Phone Interface

GENERAL DESCRIPTION:
   OEM Phone Interfaces

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright (c) 1999-2005, 2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdlib.h>
#include "customer.h"
#include "cm.h"
#include "OEMFeatures.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMEvent.h"
#include "OEMCriticalSection.h"
#include "OEMTAPI.h"
#include "OEMConfig.h"
#include "OEMHeap.h"
#include "msm/OEMPhone.h"

#include "AEE_OEM.h"
#include "AEETelephone.h"
#include "AEETelephoneTypes.h"

#if defined(FEATURE_BREW_TAPI)

#ifdef FEATURE_GSM1x
#include "OEMGSM1xMessage_SVC.h"
#endif

#define CRIT_CINFO_BUFFERS    4

/*===========================================================================

                      DATA DECLARATIONS

===========================================================================*/
typedef struct _CallInfoBuf CallInfoBuf;
struct _CallInfoBuf{
   CallInfoBuf          *pNext;
   CM_CALL_INFO_S_TYPE  callInfo;
};

struct _OEMPhone
{
   uint32                        m_nRefs;
   uint32                        m_dwNotify;
   AEECallback                   m_cbEvent;

   AEECallback                   *m_pcbListener;

   ITelephone                    *pTel;

   /* crit_* data members to be protected by critsection.*/
   CallInfoBuf                   crit_callInfo[CRIT_CINFO_BUFFERS];
                                                      /* Extra buffers to capture any back-to-back event notifications from CM.    
                                                      ** buffer-0 is considered as primary buffer and is over-written if both buffers
                                                      ** are occupied and not notified.                                              
                                                      ** This apparently occurs on an Incoming call                                  
                                                      ** where the Idle-incoming-conversations transitions overwhelm the listeners */

   CallInfoBuf                   *pci_crit_ReadyBuf;
   CallInfoBuf                   *pci_crit_FreeBuf;

   cm_ph_info_s_type             crit_phInfo;
   CM_SS_INFO_S_TYPE             crit_ssInfo;

   flg                           m_bCallInfoValid:1;
   flg                           m_bPhoneInfoValid:1;
   flg                           m_bSSInfoValid:1;

   flg                           crit_bCallInfoNotify:1;
   flg                           crit_bPhoneInfoNotify:1;
   flg                           crit_bSSInfoNotify:1;
   
   flg                           crit_bCallInfoNotify1:1;
};
   

//
// Method declarations...
//
static void OEMPhone_Notify(OEMPhone * pme, uint32 dwBit,void * pData) ;
static void OEMPhone_Wakeup(OEMPhone * pme, uint32 dwBit) ;
static void OEMPhone_HandleEvent(OEMPhone * pme) ;

static void OEMPhone_CMCallCB(cm_call_event_e_type evt, const CM_CALL_INFO_S_TYPE * pCallInfo);
static void OEMPhone_CMPhoneCB(cm_ph_event_e_type evt, const cm_ph_info_s_type * pPhoneInfo);
static void OEMPhone_CMSSCB(cm_ss_event_e_type evt, const CM_SS_INFO_S_TYPE * pSSInfo);

static boolean OEMPhone_InitCallMgr(OEMPhone * pme) ;

static OEMPhone *          gpPhone = NULL;
static cm_client_id_type   gCMID = 0;
static boolean             gbCMAct = FALSE;

#ifdef FEATURE_MULTIMODE_ARCH

static cm_call_id_type        OEMPhone_GetCMCallID( OEMPhone * po );

/* Support for call id is required in new versions of CM
 MAX_CALLS        : Currently only one call is supported
 gOrigCALLID      : Call ID gven by CM when a call is originated
                    Use this to end call only when incoming Page
                    arrives in origination stat
 call_id_arr      : Array that contains current call ID
 orig_cdma_params : Structure that contains Parameters passed when originating a CDMA call
 orig_gwcs_params : Structure that contains Parameters passed when originating a GSM/WCDMA circuit switched call
 orig_gwps_params : Structure that contains Parameters passed when originating a GSM/WCDMA packet switched call
 ans_params       : Structure that contains Parameters passed when answering an incoming call
 end_params       : Structure that contains Parameters passed when 
                    ending a call                      
 */					
#define MAX_CALLS		         1
static cm_call_id_type        gOrigCALLID = 0;
static cm_call_id_type        call_id_arr[MAX_CALLS];

#if defined (FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900) 
static cm_cdma_orig_params_s_type  orig_cdma_params;
#endif 

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
static cm_gw_cs_orig_params_s_type orig_gwcs_params;
#endif /* if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

static cm_ans_params_s_type   ans_params;
static cm_end_params_s_type   end_params;
static uint8                  num_call_ids = MAX_CALLS;

#endif /*FEATURE_MULTIMODE_ARCH*/

static OEMCriticalSection gPhoneCriticalSection;

//dgilboa
CM_CALL_INFO_S_TYPE           gCallInfo;      
cm_ph_info_s_type             gPhInfo;
CM_SS_INFO_S_TYPE             gSsInfo;

/*===========================================================================
//
//
//   PHONE INTERFACE
//
//
===========================================================================*/
/*===========================================================================

===========================================================================*/
int OEMPhone_New(OEMPhone **ppif)
{
   IShell *pShell;

   *ppif = 0;

   if (gpPhone) {

      ++gpPhone->m_nRefs;

   } else {

      gpPhone = (OEMPhone *)sys_malloc( sizeof(OEMPhone) );
      if (!gpPhone) {
         return ENOMEMORY;
      }
      
      ZEROAT( gpPhone );

      gpPhone->m_nRefs = 1;

#ifdef FEATURE_GSM1x
      OEMGSM1xMessage_Init();
#endif

      CALLBACK_Init(&gpPhone->m_cbEvent, (PFNNOTIFY)OEMPhone_HandleEvent, 
                    gpPhone);

      if( !OEMPhone_InitCallMgr(gpPhone) ){

         OEMPhone_Release(gpPhone);
         return EFAILED;
      }

      // Default the RSSI threshold...
      OEMPhone_SetRSSIThreshold(gpPhone, 5);

      {
         int i;
         CallInfoBuf **pci = &gpPhone->pci_crit_FreeBuf;

         // Initialize the callinfo buffers.
         for( i = 0; i < CRIT_CINFO_BUFFERS; i++ ) {

            *pci = &gpPhone->crit_callInfo[i];
            pci = &((*pci)->pNext);
         }
      }

      
      if (( NULL == ( pShell = AEE_GetShell())) ||
          ( SUCCESS != ISHELL_CreateInstance( pShell, AEECLSID_TELEPHONE, (void **)( &gpPhone->pTel ))))
      {
         return EFAILED;
      }
   }
   
   *ppif = gpPhone;

   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
static int OEMPhone_Delete( OEMPhone *pme )
{
   if(gCMID && gbCMAct){

      FARF(TAPI,("CM De-Act"));
      cm_client_deact(gCMID);
      gbCMAct = FALSE;
   }

   AEE_CancelTimer(0, (void *)gpPhone);

   ITELEPHONE_Release( pme->pTel );

   gpPhone = NULL;
   sys_free(pme);

   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
uint32 OEMPhone_AddRef(OEMPhone * pme)
{
   return ++pme->m_nRefs;
}

/*===========================================================================

===========================================================================*/
uint32 OEMPhone_Release(OEMPhone * pme)
{

   if( --pme->m_nRefs ) 
      return pme->m_nRefs;

   OEMPhone_Delete(pme);
   return 0;
}

/*===========================================================================

===========================================================================*/
static void OEMPhone_CancelListener( AEECallback *pcb )
{
   OEMPhone *pme = (OEMPhone *)pcb->pCancelData;
   AEECallback *pPrev = (AEECallback *)pcb->pmc;

   if( pPrev ){
      pPrev->pNext = pcb->pNext;
   }
   else {
      pme->m_pcbListener = pcb->pNext;
   }
   if( pcb->pNext ){
      pcb->pNext->pmc = (void *)pPrev;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel   = 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_AddListener( OEMPhone *pme, AEECallback *pcb )
{

   pcb->pfnCancel = OEMPhone_CancelListener;
   pcb->pCancelData = (void *)pme;
   pcb->pmc = 0;

   if( pme->m_pcbListener ) {
      pme->m_pcbListener->pmc = (void *)pcb;
   }
   pcb->pNext = pme->m_pcbListener;
   pme->m_pcbListener = pcb;

   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
void OEMPhone_SetMask(OEMPhone * po, uint32 dwMask)
{
   OEMPhone * pme = (OEMPhone *)po;

   pme->m_dwNotify = dwMask;
}

/*===========================================================================

===========================================================================*/
const cm_ph_info_s_type * OEMPhone_GetPhoneInfo(OEMPhone * po)
{
   OEMPhone *pme = (OEMPhone *)po;

   // If we are not monitoring the phone info, then get the latest copy...
   if (!pme->m_bPhoneInfoValid) {
      FARF(TAPI,("CM PH Info"));
      return 0;
   }

   return (const cm_ph_info_s_type *)&gPhInfo;
}

/*===========================================================================

===========================================================================*/
const CM_CALL_INFO_S_TYPE * OEMPhone_GetCallInfo(OEMPhone * po)
{
   OEMPhone *pme = (OEMPhone *)po;

   // If we are not monitoring the call info, then get the latest copy...
   if (!pme->m_bCallInfoValid) {
      FARF(TAPI,("CM Call Info"));
      return 0;
   }

   return &gCallInfo;
}

/*===========================================================================

===========================================================================*/
const CM_SS_INFO_S_TYPE * OEMPhone_GetSystemInfo(OEMPhone * po)
{
   OEMPhone *pme = (OEMPhone *)po;

   if (!pme->m_bSSInfoValid) {
      FARF(TAPI,("CM SS Info"));
      return 0;
   }

   return &gSsInfo;
}

/*===========================================================================

===========================================================================*/
boolean OEMPhone_GetPhoneInfo2(OEMPhone * po, cm_ph_info_s_type *pPI)
{
   OEMPhone *pme = (OEMPhone *)po;

   if (!pme->m_bPhoneInfoValid) {
      return FALSE;
   }

   MEMCPY( pPI, &gPhInfo, sizeof( *pPI ) );
   return TRUE;
}

/*===========================================================================

===========================================================================*/
boolean OEMPhone_GetCallInfo2(OEMPhone * po, CM_CALL_INFO_S_TYPE *pCI)
{
   OEMPhone *pme = (OEMPhone *)po;

   if (!pme->m_bCallInfoValid) {
      return FALSE;
   }

   MEMCPY( pCI, &gCallInfo, sizeof( *pCI ) );
   return TRUE;
}

/*===========================================================================

===========================================================================*/
boolean OEMPhone_GetSystemInfo2(OEMPhone * po, CM_SS_INFO_S_TYPE *pSI)
{
   OEMPhone *pme = (OEMPhone *)po;

   if (!pme->m_bSSInfoValid) {
      return FALSE;
   }

   MEMCPY( pSI, &(gSsInfo), sizeof( *pSI ) );
   return TRUE;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_GetMinWStr(OEMPhone * po, int nNAM, AECHAR * pDest, int nSize)
{
   AEEMobileInfo  mi;

   OEM_GetConfig(CFGI_MOBILEINFO, &mi, sizeof(AEEMobileInfo));
   STRTOWSTR(mi.szMobileID, pDest, nSize);

   return WSTRLEN(pDest);
}

/*===========================================================================

===========================================================================*/
int OEMPhone_GetMinStr(OEMPhone * po, int nNAM, char * pszDest, int nSize)
{
   AEEMobileInfo  mi;
   
   if( (char *)0 == pszDest || 0 == nSize ) return 0;

   OEM_GetConfig(CFGI_MOBILEINFO, &mi, sizeof(AEEMobileInfo));
   STRLCPY(pszDest, mi.szMobileID, nSize);
   return STRLEN(pszDest);
}

/*===========================================================================

===========================================================================*/
int OEMPhone_SetRSSIThreshold(OEMPhone * po, uint8 nDelta)
{
   //OEMPhone * pme = (OEMPhone *)po;

   cm_ph_cmd_rssi_delta_threshold(NULL, NULL, gCMID, nDelta);
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_MakeCall(OEMPhone *po,cm_call_type_e_type cType,
                                        cm_srv_type_e_type nServ,
                                        word nServOpt,
                                        const byte * pNum,
                                        byte nNumLen,
                                        cm_digit_mode_e_type nDigitMode,
                                        const byte * pAlpha,
                                        byte nAlphaLen,
                                        CM_ACTIVATE_CODE_TYPE nActivateCode)
{
   boolean              bRet = FALSE;
#ifdef FEATURE_MULTIMODE_ARCH
   cm_call_id_type      cid = 0;
   cm_num_s_type        bCalledNum, bCallingNum;
   cm_orig_alpha_s_type bAlphaType;
   char                 bMIN[16];
   AEETSSInfo           *pSSInfo = NULL;
   
#endif /*FEATURE_MULTIMODE_ARCH*/

#ifdef FEATURE_MULTIMODE_ARCH

   pSSInfo = (AEETSSInfo *)sys_malloc(sizeof(AEETSSInfo));
   if(NULL == pSSInfo){
      return ENOMEMORY;
   }

   if( SUCCESS != ITELEPHONE_GetServingSystemInfo(po->pTel, pSSInfo, sizeof (AEETSSInfo))){
      return EFAILED;
   }

   // Filling in the params for orig_cdma_params
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
   
   if(AEET_SYS_MODE_CDMA == pSSInfo->sys_mode){ 
      orig_cdma_params.srv_opt           = nServOpt;
      orig_cdma_params.activate_code     = nActivateCode;
      orig_cdma_params.drs_bit           = FALSE;
      orig_cdma_params.sr_id_included    = FALSE;
      orig_cdma_params.sr_id             = 0;
      orig_cdma_params.qos_parms_incl    = FALSE;
      orig_cdma_params.qos_parms_len     = 0;
      orig_cdma_params.last_act_data_net = SYS_SYS_MODE_NO_SRV;
   }
#endif    
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))   
   if(AEET_SYS_MODE_GSM == pSSInfo->sys_mode ||   AEET_SYS_MODE_WCDMA == pSSInfo->sys_mode ){
      orig_gwcs_params.bc_repeat_indicator.present               = FALSE;
      orig_gwcs_params.bc_repeat_indicator.repeat_indication     = 0;
      orig_gwcs_params.bearer_capability_1.present               = FALSE;
      orig_gwcs_params.bearer_capability_2.present               = FALSE;
      orig_gwcs_params.llc_repeat_indicator.present              = FALSE;
      orig_gwcs_params.llc_repeat_indicator.repeat_indication    = 0;
      orig_gwcs_params.low_layer_compatibility_1.present         = FALSE;
      orig_gwcs_params.low_layer_compatibility_2.present         = FALSE;
      orig_gwcs_params.hlc_repeat_indicator.present              = FALSE;
      orig_gwcs_params.hlc_repeat_indicator.repeat_indication    = 0;
      orig_gwcs_params.high_layer_compatibility_1.present        = FALSE;
      orig_gwcs_params.high_layer_compatibility_1.not_applicable = TRUE;
      orig_gwcs_params.high_layer_compatibility_2.present        = FALSE;
      orig_gwcs_params.high_layer_compatibility_2.not_applicable = TRUE;
      orig_gwcs_params.called_party_bcd_number.present           = FALSE;
      orig_gwcs_params.called_party_subaddress.present           = FALSE;
      orig_gwcs_params.calling_party_subaddress.present          = FALSE;
      orig_gwcs_params.forward_cug_info.present                  = FALSE;
      orig_gwcs_params.cm_clir                                   = 0;
   }
#endif 

   // Populate the number information
   STRCPY( (char *) bCalledNum.buf, (const char *) pNum );
   bCalledNum.len                   = nNumLen;
   bCalledNum.digit_mode            = nDigitMode;
   bCalledNum.number_type           = CM_TON_UNKNOWN;
   bCalledNum.number_plan           = CM_NPI_UNKNOWN;
   bCalledNum.pi                    = 0;
   bCalledNum.si                    = 0;
   bCalledNum.subaddr.extension_bit = 0;
   bCalledNum.subaddr.subadd_type   = 0;
   bCalledNum.subaddr.odd_even_ind  = 0;
   bCalledNum.subaddr.num_char      = 0;
   STRCPY ( (char *) bCalledNum.subaddr.chari, "\0" );
   // See if the number is international. This function sets type of number
   // to International if it has a '+' at the beginning
   cm_num_fill(&bCalledNum, pNum, nNumLen, nDigitMode);

   bCallingNum.len                   = OEMPhone_GetMinStr(po, 0, bMIN, sizeof(bMIN));
   STRCPY ( (char *) bCallingNum.buf, bMIN );
   bCallingNum.digit_mode            = nDigitMode;
   bCallingNum.number_type           = CM_TON_UNKNOWN;
   bCallingNum.number_plan           = CM_NPI_UNKNOWN;
   bCallingNum.pi                    = 0;
   bCallingNum.si                    = 0;
   bCallingNum.subaddr.extension_bit = 0;
   bCallingNum.subaddr.subadd_type   = 0;
   bCallingNum.subaddr.odd_even_ind  = 0;
   bCallingNum.subaddr.num_char      = 0;
   STRCPY ( (char *) bCallingNum.subaddr.chari, "\0" );

	// Populate the Alpha information
   STRCPY ((char *) &(bAlphaType.buf), (const char *) pAlpha);
   bAlphaType.len = nAlphaLen;

   //  Adding CallID to cm call
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

   if(AEET_SYS_MODE_CDMA == pSSInfo->sys_mode){ 
      bRet = cm_mm_call_cmd_orig(NULL, NULL, gCMID, cType,nServ, &bCallingNum, &bCalledNum,&bAlphaType,&orig_cdma_params,NULL,NULL,&cid);
   }
#endif
   
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))   
   if(AEET_SYS_MODE_GSM == pSSInfo->sys_mode || AEET_SYS_MODE_WCDMA == pSSInfo->sys_mode ){
      bRet = cm_mm_call_cmd_orig(NULL, NULL, gCMID, cType,nServ, &bCallingNum, &bCalledNum,&bAlphaType,NULL,&orig_gwcs_params,NULL,&cid);
   }
#endif /* if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */

   gOrigCALLID = cid;
   
   if(NULL != pSSInfo){
      sys_free(pSSInfo);
      pSSInfo = NULL;
   }
#else
   bRet = cm_call_cmd_orig(NULL, NULL, gCMID, cType, nServ, nServOpt, pNum, nNumLen, nDigitMode, pAlpha, nAlphaLen, nActivateCode);
#endif /*FEATURE_MULTIMODE_ARCH*/

   return (boolean)(bRet ? 0 : EFAILED);
}

/*===========================================================================

===========================================================================*/
#ifdef FEATURE_MULTIMODE_ARCH
static cm_call_id_type OEMPhone_GetCMCallID( OEMPhone * po )
{
   cm_call_id_type i;
   AEETCallInfo     callInfo;

   for( i=0; i < CM_CALL_ID_MAX; i++ )
   {
      if (( SUCCESS == ITELEPHONE_GetCallInfo( po->pTel, i, &callInfo,  sizeof( AEETCallInfo ))) &&
          ( AEET_CALL_TYPE_VOICE == callInfo.call_type ))
      {
         break;
      }
   }
   return i;
}
#endif

/*===========================================================================

===========================================================================*/
 int OEMPhone_AnswerCall(OEMPhone * po,cm_call_type_e_type ct)
{
#ifdef FEATURE_MULTIMODE_ARCH
   cm_call_id_type cid;
   AEETCallInfo     callInfo;

   for( cid = 0; cid < CM_CALL_ID_MAX; cid++ )
   {
      if (( SUCCESS == ITELEPHONE_GetCallInfo( po->pTel, cid, &callInfo,  sizeof( AEETCallInfo ))) &&
          ( CM_CALL_STATE_INCOM == callInfo.call_state ) &&
          ( ct == callInfo.call_type ))
      {
         break;
      }
   }

   if( cid == CM_CALL_ID_MAX ) {

      return EFAILED;
   }

   //setup the cdma answer parameters
   ans_params.info_type = CM_CALL_MODE_INFO_CDMA;
   ans_params.ans_params.cdma_ans.call_type = ct;
   
   cm_mm_call_cmd_answer(NULL, NULL, gCMID, cid, &ans_params);
#else
   cm_call_cmd_answer(NULL, NULL,gCMID,ct);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_AnswerVoice(OEMPhone * po, cm_answer_voice_e_type nAnswer, dword dwDuration)
{
   cm_ph_cmd_answer_voice(NULL, NULL, gCMID, nAnswer,dwDuration);
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_EndCall(OEMPhone * po)
{
   int nErr = SUCCESS;
#ifdef FEATURE_MULTIMODE_ARCH
   const CM_CALL_INFO_S_TYPE  *pci;

   pci = OEMPhone_GetCallInfo(po);

   if( !pci ) {

      return EFAILED;
   }

   call_id_arr[0] = pci->call_id;
   end_params.call_id = call_id_arr[0];

   if( pci->mode_info.info_type == CM_CALL_MODE_INFO_CDMA 
       || pci->mode_info.info_type == CM_CALL_MODE_INFO_NONE ) {
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
      end_params.info_type                               = CM_CALL_MODE_INFO_CDMA;
      end_params.end_params.cdma_end.end_reason_included = TRUE;
      end_params.end_params.cdma_end.end_reason          = CM_CALL_END_REL_NORMAL;
#else
      nErr = EFAILED;
#endif
   }
   else if( pci->mode_info.info_type == CM_CALL_MODE_INFO_GW_CS ) {
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
      end_params.info_type                     = CM_CALL_MODE_INFO_GW_CS;
      end_params.end_params.gw_cs_end.reserved = 0;
#else
      nErr = EFAILED;
#endif
   }
   else {
      nErr = EFAILED;
   }

   cm_mm_call_cmd_end(NULL, NULL, gCMID, num_call_ids, &end_params );
#else
   cm_call_cmd_end(NULL,NULL,gCMID);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return nErr;
}

/*===========================================================================
            
===========================================================================*/
int OEMPhone_Flash(OEMPhone * po,const byte* pNum, byte nNumLen)
{
#ifdef FEATURE_MULTIMODE_ARCH
	// Support for flash type. Currently setting the 
	// value to CM_FLASH_TYPE_OTHER for other possible values look at cm_flash_type_e_type
	// in cm.h	
   cm_call_sups_params_s_type sups_type;
   cm_call_id_type cid = OEMPhone_GetCMCallID( po );
   
   if( cid == CM_CALL_ID_MAX ) {
      return EFAILED;
   }

   sups_type.call_id = cid;
   STRCPY ((char *) sups_type.sups_number.buf, (const char *) pNum );
   sups_type.sups_number.len = nNumLen;
   
   cm_mm_call_cmd_sups(NULL,NULL,gCMID,CM_CALL_SUPS_TYPE_SIMPLE_FLASH,
                       &sups_type);
#else
   cm_call_cmd_flash(NULL,NULL,gCMID, pNum, nNumLen);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_SetPrefMode(OEMPhone * po, cm_mode_pref_e_type nMode,CM_MODE_TERM_E_TYPE nModeTerm,
                                uint32 dwDuration)
{
#ifdef FEATURE_MULTIMODE_ARCH
#ifdef CM_API_SYS_SEL_PRL_PREF
  (void)cm_ph_cmd_sys_sel_pref(NULL, NULL, gCMID, nMode, nModeTerm, dwDuration,
                        CM_GW_ACQ_ORDER_PREF_NO_CHANGE, CM_BAND_PREF_NO_CHANGE,
                        CM_PRL_PREF_NO_CHANGE,
                          CM_ROAM_PREF_NO_CHANGE,CM_HYBR_PREF_NO_CHANGE,
                        CM_SRV_DOMAIN_PREF_NO_CHANGE,
                        CM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL
#ifdef CM_API_WLAN
                        , NULL
#endif
                        );

#else
   (void)cm_ph_cmd_sys_sel_pref(NULL, NULL, gCMID, nMode, nModeTerm, dwDuration,
                          CM_GW_ACQ_ORDER_PREF_NO_CHANGE,CM_BAND_PREF_NO_CHANGE,
                          CM_ROAM_PREF_NO_CHANGE,CM_HYBR_PREF_NO_CHANGE,
                        CM_SRV_DOMAIN_PREF_NO_CHANGE,
                        CM_NETWORK_SEL_MODE_PREF_NO_CHANGE, NULL
#ifdef CM_API_WLAN
                        , NULL
#endif
                        );
#endif
#else
   cm_ph_cmd_mode_pref(NULL,NULL,gCMID,nMode,nModeTerm,dwDuration);
#endif
   return 0;
}

/*===========================================================================

===========================================================================*/
//int OEMPhone_SetSysMode(OEMPhone * po,cm_sys_pref_e_type nPrefSys)
//{
//   cm_ph_cmd_sys_pref(NULL,NULL,gCMID, nPrefSys);
//   return 0;
//}

/*===========================================================================

===========================================================================*/
int OEMPhone_SetNAM(OEMPhone * po, cm_nam_e_type nNam)
{
   cm_ph_cmd_nam_sel(NULL,NULL,gCMID,nNam);
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_SetOprtMode(OEMPhone * po, CM_OPRT_MODE_E_TYPE nMode)
{
   cm_ph_cmd_oprt_mode(NULL,NULL,gCMID,nMode);
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_BurstDTMF(OEMPhone * po, uint8 nOn, uint8 nOff, const uint8 * pDigits, uint8 nLen)
{
#ifdef FEATURE_MULTIMODE_ARCH
   cm_call_id_type cid = OEMPhone_GetCMCallID( po );
   
   if( cid == CM_CALL_ID_MAX ) {
      return EFAILED;
   }
   cm_mm_inband_cmd_burst_dtmf(NULL,NULL,gCMID,cid,nOn,nOff,nLen,pDigits);
#else
   cm_inband_cmd_burst_dtmf(NULL,NULL,gCMID, nOn,nOff,nLen,pDigits);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_StartDTMF(OEMPhone * po, uint8 nDigit)
{
#ifdef FEATURE_MULTIMODE_ARCH
   cm_call_id_type cid = OEMPhone_GetCMCallID( po );
   
   if( cid == CM_CALL_ID_MAX ) {
      return EFAILED;
   }
   cm_mm_inband_cmd_start_cont_dtmf(NULL,NULL,gCMID,cid,nDigit);
#else
   cm_inband_cmd_start_cont_dtmf(NULL,NULL,gCMID, nDigit);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return 0;
}

/*===========================================================================

===========================================================================*/
int OEMPhone_StopDTMF(OEMPhone * po, uint16 nDuration)
{
#ifdef FEATURE_MULTIMODE_ARCH
   cm_call_id_type cid = OEMPhone_GetCMCallID( po );
   
   if( cid == CM_CALL_ID_MAX ) {
      return EFAILED;
   }
   cm_mm_inband_cmd_stop_cont_dtmf(NULL,NULL,gCMID,cid,nDuration);
#else
   cm_inband_cmd_stop_cont_dtmf(NULL,NULL,gCMID, nDuration);
#endif /*FEATURE_MULTIMODE_ARCH*/
   return 0;
}

/*===========================================================================

===========================================================================*/
static void OEMPhone_Notify(OEMPhone * pme, uint32 dwBit, void * pData)
{
   if (pme->m_dwNotify & dwBit) {
       AEE_Notify(AEECLSID_PHONE, dwBit, pData);
   }
}

/*===========================================================================

===========================================================================*/
static void OEMPhone_Wakeup(OEMPhone * pme,uint32 dwBit)
{
   AEE_SYS_RESUME(&pme->m_cbEvent);
}

/*===========================================================================
TRUE when in no service or OFFLINE state
===========================================================================*/
boolean OEMPhone_IsOffline( OEMPhone *pme )
{
   boolean bRet = FALSE;

#if defined( FEATURE_MULTIMODE_ARCH ) && !defined( FEATURE_CM20_INTERFACE )
   bRet = pme->m_bSSInfoValid && ( SYS_SRV_STATUS_NO_SRV == gSsInfo.srv_status );
#else
   bRet = pme->m_bPhoneInfoValid && ( CM_SRV_STATE_NO_SRV == gPhInfo.srv_state );
#endif
   return bRet;
}

/*===========================================================================

===========================================================================*/
static CallInfoBuf *GetCallInfoBuf( CallInfoBuf **ppi )
{
   CallInfoBuf *pret = *ppi;
   if( pret ) {
      *ppi = pret->pNext;
      pret->pNext = 0;
   }

   return pret;
}

/*===========================================================================

===========================================================================*/
static void AddCallInfoBuf( CallInfoBuf **pplist, CallInfoBuf *pbuf )
{
   while( *pplist ) {
      pplist = &((*pplist)->pNext);
   }
   *pplist = pbuf;
}

/*===========================================================================

===========================================================================*/
static void OEMPhone_HandleEvent(OEMPhone * pme)
{
   boolean eventPhoneInfo = FALSE, eventCallInfo = FALSE, eventSSInfo = FALSE;
   AEECallback *pcb;

   if( pme->crit_bCallInfoNotify ) {

      pme->crit_bCallInfoNotify = FALSE;
      do {
         CallInfoBuf *pci;

         eventCallInfo = FALSE;

         OEMCriticalSection_Enter(&gPhoneCriticalSection);

         pci = GetCallInfoBuf(&pme->pci_crit_ReadyBuf);

         if( pci ) {
            MEMCPY( &gCallInfo, &pci->callInfo, sizeof(CM_CALL_INFO_S_TYPE) );
            eventCallInfo = TRUE;
            pme->m_bCallInfoValid = TRUE;
            AddCallInfoBuf(&pme->pci_crit_FreeBuf, pci);
         }
         
         OEMCriticalSection_Leave(&gPhoneCriticalSection);

         if (eventCallInfo) {
            OEMPhone_Notify(pme,NMASK_PH_CALL_INFO,&gCallInfo);
         }

      } while( eventCallInfo );

   }

   OEMCriticalSection_Enter(&gPhoneCriticalSection);
   if (pme->crit_bPhoneInfoNotify) {
      MEMCPY(&gPhInfo,&gpPhone->crit_phInfo,sizeof(cm_ph_info_s_type));
      pme->crit_bPhoneInfoNotify = FALSE;
      eventPhoneInfo = TRUE;
      pme->m_bPhoneInfoValid = TRUE;
   }
   if( pme->crit_bSSInfoNotify ) {
      MEMCPY(&gSsInfo,&gpPhone->crit_ssInfo,sizeof(CM_SS_INFO_S_TYPE));
      pme->crit_bSSInfoNotify = FALSE;
      eventSSInfo = TRUE;
      pme->m_bSSInfoValid = TRUE;
   }
   OEMCriticalSection_Leave(&gPhoneCriticalSection);

   if (eventPhoneInfo) {
      OEMPhone_Notify(pme,NMASK_PH_PHONE_INFO,&gPhInfo);
   }
   
   if (eventSSInfo) {
      OEMPhone_Notify(pme,NMASK_PH_SS_INFO,&gSsInfo);
   }

   /* Call the listeners */
   pcb = pme->m_pcbListener;
   while( pcb ) {

      if( pcb->pfnNotify ) {
         pcb->pfnNotify( pcb->pNotifyData );
      }

      pcb = pcb->pNext;
   }

}

/*===========================================================================

===========================================================================*/
void OEMPhone_CMCallCB(cm_call_event_e_type evt, const CM_CALL_INFO_S_TYPE * pCallInfo)
{
   CallInfoBuf *pci;

   if(gpPhone && pCallInfo){

      OEMCriticalSection_Enter(&gPhoneCriticalSection);

      pci = GetCallInfoBuf( &gpPhone->pci_crit_FreeBuf );

      if( pci ) {

         MEMCPY( &pci->callInfo, pCallInfo,sizeof(CM_CALL_INFO_S_TYPE) );
         AddCallInfoBuf( &gpPhone->pci_crit_ReadyBuf, pci );
         gpPhone->crit_bCallInfoNotify = TRUE;
      }

      OEMCriticalSection_Leave(&gPhoneCriticalSection);

      OEMPhone_Wakeup(gpPhone,NMASK_PH_CALL_INFO);
   }
}

/*===========================================================================

===========================================================================*/
void OEMPhone_CMPhoneCB(cm_ph_event_e_type evt, const cm_ph_info_s_type * pPhoneInfo)
{
   if(gpPhone && pPhoneInfo){

      OEMCriticalSection_Enter(&gPhoneCriticalSection);

      MEMCPY(&gpPhone->crit_phInfo,pPhoneInfo,sizeof(cm_ph_info_s_type));
      gpPhone->crit_bPhoneInfoNotify = TRUE;

      OEMCriticalSection_Leave(&gPhoneCriticalSection);

      OEMPhone_Wakeup(gpPhone,NMASK_PH_PHONE_INFO);
   }
}

/*===========================================================================

===========================================================================*/
void OEMPhone_CMSSCB(cm_ss_event_e_type evt, const CM_SS_INFO_S_TYPE * pSSInfo)
{
   if(gpPhone && pSSInfo){

      OEMCriticalSection_Enter(&gPhoneCriticalSection);

      MEMCPY(&gpPhone->crit_ssInfo,pSSInfo,sizeof(CM_SS_INFO_S_TYPE));
      gpPhone->crit_bSSInfoNotify = TRUE;

      OEMCriticalSection_Leave(&gPhoneCriticalSection);

      OEMPhone_Wakeup(gpPhone,NMASK_PH_SS_INFO);
   }
}

/*===========================================================================

===========================================================================*/
static boolean OEMPhone_InitCallMgr(OEMPhone * pme)
{
   if(!gCMID){
      FARF(TAPI,("CM Init"));

#if defined(FEATURE_NEWCM)
      if(cm_client_init(CM_CLIENT_TYPE_FLOATING2,&gCMID) != CM_CLIENT_OK || !gCMID)
#else
      if(cm_client_init(&gCMID) != CM_CLIENT_OK || !gCMID)
#endif
         return FALSE;
   }

   FARF(TAPI,("CM Reg"));
#ifdef FEATURE_MULTIMODE_ARCH
   cm_mm_client_call_reg(gCMID, OEMPhone_CMCallCB, CM_CLIENT_EVENT_REG, CM_CALL_EVENT_ORIG, CM_CALL_EVENT_INFO, NULL);
   cm_client_ph_reg(gCMID, OEMPhone_CMPhoneCB, CM_CLIENT_EVENT_REG, CM_PH_EVENT_OPRT_MODE, CM_PH_EVENT_INFO_AVAIL, NULL);
   cm_mm_client_ss_reg(gCMID, OEMPhone_CMSSCB, CM_CLIENT_EVENT_REG, CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_INFO, NULL);
#else
   cm_client_call_reg(gCMID, OEMPhone_CMCallCB,  CM_CLIENT_EVENT_REG, CM_CALL_EVENT_ORIG,      CM_CALL_EVENT_INFO,     0);
   cm_client_ph_reg(gCMID,   OEMPhone_CMPhoneCB, CM_CLIENT_EVENT_REG, CM_PH_EVENT_OPRT_MODE,   CM_PH_EVENT_INFO_AVAIL, 0);
   cm_client_ss_reg(gCMID,   OEMPhone_CMSSCB,    CM_CLIENT_EVENT_REG, CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_INFO,       0);
#endif /*FEATURE_MULTIMODE_ARCH*/

#ifdef FEATURE_APPS_POWER_COLLAPSE
   cm_mm_client_ss_pwr_collapse_reg(gCMID, OEMPhone_CMSSCB, 
                                    CM_PROCESSOR_APPS_1,
                                    CM_PWR_COLLAPSE_NOTIFY_SELECTIVE_DEFAULT,
                                    CM_CLIENT_EVENT_REG,
                                    CM_SS_EVENT_SRV_CHANGED, 
                                    CM_SS_EVENT_INFO);
#endif

   if(!gbCMAct){      
      FARF(TAPI,("CM Act"));
      gbCMAct = (boolean)(cm_client_act(gCMID) == CM_CLIENT_OK);
      if(!gbCMAct){
         FARF(TAPI,("Unable to Activate AMSS Call Manager"));
         return FALSE;
      }
   }
   return TRUE;
}

/*===========================================================================

===========================================================================*/
boolean OEMPhone_IsVoiceCall(OEMPhone * po)
{
   cm_call_state_e_type    st;
   CM_CALL_INFO_S_TYPE *  pci;
   if((pci = (CM_CALL_INFO_S_TYPE *)OEMPhone_GetCallInfo(po)) != NULL){
      st = pci->call_state;
      FARF(TAPI,("CM IsVoice %s",(pci->call_type == CM_CALL_TYPE_VOICE && (st == CM_CALL_STATE_CONV || st == CM_CALL_STATE_INCOM || st == CM_CALL_STATE_ORIG) ? "YES" : "NO")));
      return pci->call_type == CM_CALL_TYPE_VOICE && (st == CM_CALL_STATE_CONV || st == CM_CALL_STATE_INCOM || st == CM_CALL_STATE_ORIG);
   }
   return FALSE;
}
   

/*===========================================================================

===========================================================================*/
boolean OEMPhone_IsDataSupported(OEMPhone * po)
{

#if defined(FEATURE_MULTIMODE_ARCH) && !defined(FEATURE_CM20_INTERFACE)
   const cm_mm_ss_info_s_type *     ss;

   // Returns TRUE if there is digital service

   ss = OEMPhone_GetSystemInfo(po);

   FARF(TAPI,("CM Digital %s",(ss && cm_is_sys_mode_digital(ss->sys_mode) ? "TRUE" : "FALSE")));

   return ss && cm_is_sys_mode_digital(ss->sys_mode) && (!OEMPhone_GetCallInfo(po) || ( (SYS_SYS_MODE_WCDMA != ss->sys_mode) && (!OEMPhone_IsVoiceCall(po))));
#else
   const cm_ph_info_s_type *     ph;

   // Returns TRUE if there is digital service

   ph = OEMPhone_GetPhoneInfo(po);

   FARF(TAPI,("CM Digital %s",(ph && ph->srv_state ? "TRUE" : "FALSE")));

   return ph && ph->srv_state == CM_SRV_STATE_DIGITAL && (!OEMPhone_GetCallInfo(po) || !OEMPhone_IsVoiceCall(po));
#endif /*FEATURE_MULTIMODE_ARCH*/
}

/*===========================================================================

===========================================================================*/
boolean OEMPhone_InConversation(OEMPhone * po)
{

   cm_call_state_e_type    st;
   CM_CALL_INFO_S_TYPE *  pci;
   if((pci = (CM_CALL_INFO_S_TYPE *)OEMPhone_GetCallInfo(po)) != NULL){
      st = pci->call_state;
      return (st == CM_CALL_STATE_CONV);
   }
   return FALSE;
}


#endif /* if defined(FEATURE_BREW_TAPI) */

