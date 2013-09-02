/*===========================================================================
                              OEMSVC.c

  OEM REFERENCE SUPERVISOR MODE FUNCTIONS SUPPORTING
  USER MODE APPLICATION CONTROL ROUTINES

  This file contains a reference implementation of Supervisor mode services that
  OEMs need to provide in order to support BREW. Most functions defined here are
  called from OEM porting layers such as OEMAppFuncs, OEMNotify, etc. The functions
  specified in this file are not all mandatory. The OEM can add, delete, or modify
  functions in this file to support the functions in OEMAppFuncs and OEMNotify.

        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "target.h"
#include "customer.h"
#if !defined(AEE_SIMULATOR)
#ifndef WIN32
#include "disp.h"
#include "bit.h"
#endif
#include "ui.h"
#ifndef WIN32
#include "voc.h"
#endif
#endif //!defined(AEE_SIMULATOR)
#include "OEMFeatures.h"
#if defined( FEATURE_APP_MANAGER) || ( defined( FEATURE_QSC6010_TEST) && !defined( FEATURE_BREW_SMS) && defined( T_QSC6010))
#include "AEE_OEMFile.h"
#endif
#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif //def FEATURE_MMGSDI

#ifdef CUST_EDITION
#include "OEMHeap.h"
#endif
#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "OEMDisp.h"

#ifdef FEATURE_GSM1x
#include "OEMGSM1xMessage_SVC.h"

#ifndef FEATURE_GSM1x_WMS
extern nv_stat_enum_type uasms_nv_read_wait(nv_items_enum_type item,nv_item_type *data_ptr);
extern rex_tcb_type uasms_tcb;

#else

extern nv_stat_enum_type wms_nv_read_wait(nv_items_enum_type item,nv_item_type *data_ptr);
extern rex_tcb_type wms_tcb;
#endif/*FEATURE_GSM1x_WMS*/

#endif/*FEATURE_GSM1x*/

#include "AEEComdef.h"

#if defined(OEMAPPFUNCS)
#ifdef FEATURE_MANGO_UI
#include "brewPort.bid"
#endif
#include "OEMConfig.h"
#include "OEMNotify.h"
#include "AEE_OEMDispatch.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEPhone.h"
#include "AEEFile.h"
#include "AEERUIM.h"

#ifdef FEATURE_UI_CORE
#ifndef USES_MMI
#include "CoreApp.BID"
#endif
#include "uixsnd.h"
#endif /* FEATURE_UI_CORE */
#include "OEMCFGI.h"
#include "AEEDownload.h"
#include "nv.h"
#include "nvi.h"
#ifndef USES_MMI
#include "uiutils.h"
#include "ui_base.h"
#else
#include "OEMNV.h"
#include "AEE_OEM.h"
#include "OEMClassIDs.h"
#endif
#include "mobile.h"

#ifdef FEATURE_UIM_RUIM
#include "rex.h"
#include "uim.h"
#include "nvruimi.h"

extern dword uim_get_esn_me(void);
#endif //def FEATURE_UIM_RUIM

#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
#endif //def FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING

#include "OEMSMS.h"
#include "AEESMS.h"

#include "AEEMutualAuth.h"   // Required for MA CAPAB Flags

#include "AEECLSID_BACKLIGHT.bid"

#ifdef CUST_EDITION	  
#include "OEMSVC.h"
#endif /*CUST_EDITION*/

#include "OEMSecSysClock.h"

#if defined(FEATURE_UIONE_HDK)
#include"PhoneApp.bid"
#include "TestApp_SafeMode.h"
#include "TESTAPP_SAFEMODE.BID"
#endif // FEATURE_UIONE_HDK

#ifdef FEATURE_ICARDSESSION
#include "AEECardSession.h"
#include "AEECARDSESSION.bid"
#endif //FEATURE_ICARDSESSION

#if defined(FEATURE_UIM_RUIM)
#define CARD_ID_SIZE    ((word)(sizeof(((nv_item_type *) 0)-> esn.esn )))

void OEM_IssueRUIMCmd(uim_cmd_type *uim_cmd_ptr);
static void UIM_Report(uim_rpt_type *report);

#endif //defined(FEATURE_UIM_RUIM)

int OEM_GetBrewConfig(AEEConfigItem id, void *buffer, int len);
#define BREW_CONFIG_FILE        "fs:/sys/brewconfig.dat"
#define BREW_CONFIG_DELIMITER   '\n'

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#include "pbmlib.h"
#endif
#define DEFAULT_MDN "NOT-AVAILABLE"

#define DEFAULT_SUBSCRIBERID "0000000000000000000000000000000"

#define CFGI_OEMAUTH_RUIM_CARDID  (CFGI_MAX + 121)

#define OEMAUTH_CHALLENGE_CAP	"OEMAuthCap="
#define OEMAUTH_CHALLENGE_CAP_LEN	11

#define OEM_AUTH_RESPLEN	"OEMAuthRespMaxLen="
#define OEM_AUTH_RESPLEN_LEN	18

#define OEMAUTH_RUIM_CARD_ID	"OEMAuthCardId="
#define OEMAUTH_RUIM_CARD_ID_LEN	14

#ifdef FEATURE_MMGSDI
typedef struct{
  boolean success;
  int32   len;
  uint8  global_data[GSDI_MAX_DATA_BLOCK_LEN];
}sim_access_data_type;

sim_access_data_type sim_read_response;
#endif /* FEATURE_MMGSDI */

#define LAST_VALID_TIMEINFO_FILE "fs:/sys/lastvalidinfo.dat"

#ifdef FEATURE_ICARDSESSION
typedef struct _FileAttr
{
   AEECardSessionReadTpStatus *pReadData;
   AEECardSessionReadRecStatus *pReadRecData;
   AEECardSessionFileAttribStatus *pFileStatus;
   AEECardSessionCmdStatus *pRegisterStatus;
   boolean bTransp;//Whether transparent file or linear fixed
   OEMProvisionInfo *pProvInfo;
   AEECallback *pcbSVCCallback;
   int nSize;//size provided by user
   uint32 dwConfigItem;//config item 
   boolean bRead; //Whether for Read or Write
   ICardSession *pICardSession;
} FileAttr;

#define MMS_ICP_PROV_INFO "fs:/sys/mmsicp.dat"
#define MMS_CONFIG_PROV_INFO "fs:/sys/mmsconfig.dat"
#define MMS_UP_PROV_INFO "fs:/sys/mmsup.dat"
#define BROWSER_CP_PROV_INFO "fs:/sys/browsercp.dat"
#define JAVA_JDL_PROV_INFO "fs:/sys/javajdl.dat"


#define MMS_UP_RECORD_LEN 47
#define MMS_UP_MAX_NUM_RECORDS 2

#ifndef RELEASEIF
#define RELEASEIF(p)          if (p) { IBASE_Release((IBase*)(void*)(p)); (p) = NULL; }
#endif //RELEASEIF
#ifndef FREEIF
#define FREEIF(p)                 if (p) { FREE((void*)p); (p) = 0; }
#endif //FREEIF

#endif //FEATURE_ICARDSESSION
/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/
#ifdef FEATURE_APP_SVCPRG
int OEM_GetCachedConfig(AEEConfigItem i, void * pBuff, int nSize);
int OEM_SetCachedConfig(AEEConfigItem i, void * pBuff, int nSize);
int OEM_GetPlatformID(uint32 *pPlatformID);
#endif  //def FEATURE_APP_SVCPRG

#ifdef FEATURE_ICARDSESSION
void OEM_SVCFileAttrCb(FileAttr *pAttr);
void OEM_SVCReadDataCb(FileAttr *pAttr);
void OEM_SVC_EFS_GetData(FileAttr *pAttr);
#endif //FEATURE_ICARDSESSION

typedef struct _NVMap
{
   AEEConfigItem        cfgi;
   nv_items_enum_type   nvi;
} NVMap;
#ifdef CUST_EDITION	 
// 解决慢卡插入后，启动 Applet 慢的问题
typedef struct _RUIM_NV_Cache
{
    // CFGI_CARDID
    flg         isCARDIDValid:1;    // 值为1时结构包含对应项有效数据
    // NV_DIR_NUMBER_I
    flg         isDirNumValid:1;    // 值为1时结构包含对应项有效数据
    // NV_CURR_NAM_I
    flg         isCurNamValid:1;    // 值为1时结构包含对应项有效数据
    
    int         nCardIDLen;
    byte        *CardIDbuf;
    
    nv_mob_dir_data_type           mobdir_number;
} RUIM_NV_Cache;

static RUIM_NV_Cache        gUIMNVCache;

static int DeviceMp4State = DEVICE_MP4_STATE_OFF;
static int DeviceCameraState = DEVICE_CAMERA_STATE_OFF;
#endif /*CUST_EDITION*/
#if defined(FEATURE_UIM_RUIM)
#if defined(FEATURE_MMGSDI ) && !defined ( FEATURE_MULTIPROCESSOR )
static uim_cmd_type        gUimCmd;
#endif /* ndef FEATURE_MMGSDI */
static uim_rpt_type        gCallBack;
#endif //defined(FEATURE_UIM_RUIM)

void GetMobileInfo(AEEMobileInfo * pmi);
static int  GetSoftwareVersion(byte *pVer, int *pnVerLen);
extern uint32 GetLngCode(void);
extern int SetLngCode(uint32 dwLngCode);

int GetAccess(boolean *pBool);
int GetTraffic(boolean *pBool);
int GetPayloadLength(uint32 *pInt);
int SetAccess(boolean *pBool);
int SetTraffic(boolean *pBool);
int SetPayloadLength(uint32 *pInt);
int SetSubscriberID(byte *pBuff, int nSize);

extern int OEMDisp_GetDispInfo(AEEDeviceInfo* pi);

#define DOWNLOAD_INFO_FILE "fs:/sys/downloadinfo.dat"
#define USER_CLOCK_OFFSET_FILE "fs:/sys/userclkoffset.dat"

#if defined(FEATURE_GSM1x)
/*===========================================================================
FUNCTION gsm1x_get_nv

DESCRIPTION
  Get an item from the nonvolatile memory.  This function can be called from
  one of the following tasks :UI,WMS and uasms.
  For each task the appropriate function is called.
RETURN VALUE
  The NV return code.

DEPENDENCIES
===========================================================================*/
nv_stat_enum_type gsm1x_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
   rex_tcb_type *tcb_ptr = rex_self();
   if (tcb_ptr == &ui_tcb)
   {
      return ui_get_nv(item,data_ptr);
   }

#ifndef FEATURE_GSM1x_WMS

   if (tcb_ptr == &uasms_tcb)
   {
       return uasms_nv_read_wait(item,data_ptr);
   }

#else

   if (tcb_ptr == &wms_tcb)
   {
      return wms_nv_read_wait(item,data_ptr);
   }

#endif/*FEATURE_GSM1x_WMS*/

   /*Called from a illegal task.*/
   MSG_FATAL( "gsm1x_get_nv called from a wrong task context  0x%x", tcb_ptr, 0, 0 );
   return NV_FAIL_S;
}

#if defined(TSG_NV_ACCESS_IMPLEMENTATION)
#define GSM1X_NV_WRITE_FUNC(__item_id,__item_ptr)  OEMNV_Put(__item_id,__item_ptr)
#define GSM1X_NV_READ_FUNC(__item_id,__item_ptr)   OEMNV_Get(__item_id,__item_ptr)
#else
#define GSM1X_NV_WRITE_FUNC(__item_id,__item_ptr)  ui_put_nv(__item_id,__item_ptr)
/*write is always from a UI context */

#define GSM1X_NV_READ_FUNC(__item_id,__item_ptr)   gsm1x_get_nv(__item_id,__item_ptr)

#endif /* TSG_NV_ACCESS_IMPLEMENTATION */

void OEM_GetGSM1xPRL( AEEGSM1xPRLInfo *PRLInfoPtr );
void OEM_SetGSM1xPRL( AEEGSM1xPRLInfo *PRLInfoPtr );

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
void OEM_GetGSM1xRTREConfig( AEEGSM1xRTREConfig *pRTREConfig );
void OEM_SetGSM1xRTREConfig( AEEGSM1xRTREConfig *pRTREConfig );
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

void OEM_GetGSM1xIdentityParams( AEEGSM1xIdentityParams *pParams );
void OEM_SetGSM1xIdentityParams( AEEGSM1xIdentityParams *pParams );
void OEM_SetGSM1xSIDNIDParams( AEEGSM1xSIDNIDParams *pParams );
void OEM_GetGSM1xSIDNIDParams( AEEGSM1xSIDNIDParams *pParams );

#endif /* FEATURE_GSM1x */
/* translate from min to digit - '0' is 10 */
static char mintable[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

#if defined(FEATURE_UIM_RUIM)
/*===========================================================================

Function: UIM_Report

===========================================================================*/
static void UIM_Report(uim_rpt_type *report)
{
#ifndef WIN32
   MEMCPY(&gCallBack, report, sizeof(uim_rpt_type));
   rex_set_sigs( &ui_tcb, UI_RUIM_SIG);
#endif
}
#endif //(FEATURE_UIM_RUIM)

#ifdef FEATURE_MMGSDI
void OEM_SVCGsdiReadCb(gsdi_cnf_T * sim_cnf)
{
  // Save Data somewhere
  if ( sim_cnf->read_cnf.message_header.gsdi_status == GSDI_SUCCESS )
  {
    sim_read_response.success = TRUE;
    memcpy(sim_read_response.global_data,
           sim_cnf->read_cnf.data,
           sim_cnf->read_cnf.returned_data_len);
    sim_read_response.len = sim_cnf->read_cnf.returned_data_len;
  }
  else
  {
    sim_read_response.success = FALSE;
    sim_read_response.len = 0;
  }
  rex_set_sigs(&ui_tcb,UI_RUIM_SIG);
}
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_ICARDSESSION
/*==================================================================
Function: OEM_SVCRegisterReadCb

Description: Callback function for Register API

Prototype:
   void OEM_SVCRegisterReadCb(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVCRegisterReadCb(FileAttr *pAttr)
{
   FileAttr *psFileAttr = (FileAttr*)pAttr;
   int i = psFileAttr->dwConfigItem;
   uint32 dwFileName = 0;
   int nResult = EFAILED;
   AEECardSessionCmdStatus *pRegisterStatus = (AEECardSessionCmdStatus*)
                                               psFileAttr->pRegisterStatus;
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   CALLBACK_Cancel(psFileAttr->pcbSVCCallback);   
   if(SUCCESS == pRegisterStatus->nCmdStatus){
      AEECallback  *pcbReadData = psFileAttr->pcbSVCCallback; 
      AEECardSessionReadTpStatus *psReadData = NULL;
      AEECardSessionReadRecStatus *psReadRecData = NULL;
      FREEIF(pRegisterStatus);      
      CALLBACK_Init(pcbReadData, (PFNNOTIFY)OEM_SVCReadDataCb,
                          (void*)(psFileAttr));
      //psFileAttr->pcbSVCCallback = (AEECallback*)pcbReadData;

      switch(i){
               case CFGI_MMS_CONFIG:
                  {
                     dwFileName = AEECARDSESSION_CDMA_MMS_CONFIG;
                     break;
                  }
               case CFGI_MMS_ICP:
                  {
                     dwFileName = AEECARDSESSION_CDMA_MMS_ICP;
                     break;
                  }
               case CFGI_MMS_UP:
                  {
                     dwFileName = AEECARDSESSION_CDMA_MMS_UP;
                     break;
                  }
               case CFGI_BROWSER_CP:
                  {
                     dwFileName = AEECARDSESSION_CDMA_BROWSER_CP;
                     break;
                  }
               case CFGI_JAVA_JDL:
                  {
                     dwFileName = AEECARDSESSION_CDMA_JAVA_DURL;
                     break;
                  }
               
               default:
                  {
                     RELEASEIF(psFileAttr->pICardSession);
                     FREEIF(pcbReadData);
                     FREEIF(psFileAttr);
                     pProvInfo->nStatus = EFAILED;
                     pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
                     return ;
                  }

      }
      if(CFGI_MMS_UP != i){
         psReadData= (AEECardSessionReadTpStatus *)MALLOC(
                             sizeof(AEECardSessionReadTpStatus));
         if(NULL == psReadData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         psReadData->pReadData = (AEECardSessionData*)MALLOC(sizeof(AEECardSessionData));
         if(NULL == psReadData->pReadData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(psReadData);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         psReadData->pReadData->pData = (uint8*)MALLOC(psFileAttr->nSize);
         psReadData->pReadData->nLen = psFileAttr->nSize;
         if(NULL == psReadData->pReadData->pData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(psReadData->pReadData);
            FREEIF(psReadData);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         psFileAttr->pReadData = psReadData;
         psFileAttr->bTransp = TRUE;
         nResult = ICARDSESSION_ReadTransparent(psFileAttr->pICardSession,
                                                dwFileName,
                                                0,
                                                psFileAttr->nSize,
                                                psReadData,
                                                pcbReadData);
      }else{
         
         psReadRecData= (AEECardSessionReadRecStatus *)MALLOC(
                           sizeof(AEECardSessionReadRecStatus));
         if(NULL == psReadRecData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         
         psReadRecData->pReadData = (AEECardSessionData*)MALLOC(sizeof(AEECardSessionData));
         if(NULL == psReadRecData->pReadData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(psReadRecData);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         
         psReadRecData->pReadData->pData = (uint8*)MALLOC(psFileAttr->nSize);
         psReadRecData->pReadData->nLen = psFileAttr->nSize;
         if(NULL == psReadRecData->pReadData->pData){
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(psReadRecData->pReadData);
            FREEIF(psReadRecData);
            FREEIF(pcbReadData);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = ENOMEMORY;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }
         psFileAttr->pReadRecData = psReadRecData;
         psFileAttr->bTransp = FALSE;

         nResult = ICARDSESSION_ReadRecord(psFileAttr->pICardSession,
                                          dwFileName,
                                          pProvInfo->dwRecordNumber,
                                          psFileAttr->nSize,
                                          psReadRecData,
                                          pcbReadData);
      }
      if(SUCCESS != nResult){
         RELEASEIF(psFileAttr->pICardSession);
         FREEIF(psReadRecData);
         FREEIF(pcbReadData);
         FREEIF(psFileAttr);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData); 
         return;
       
      }
      pProvInfo->nStatus = nResult;

   }
   else {
      FREEIF(pRegisterStatus);
      RELEASEIF(psFileAttr->pICardSession);
      OEM_SVC_EFS_GetData(psFileAttr);
   }
}
/*==================================================================
Function: OEM_SVCRegisterWriteCb

Description: Callback function for Register API

Prototype:
   void OEM_SVCRegisterWriteCb(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVCRegisterWriteCb(FileAttr *pAttr)
{
   FileAttr *psFileAttr = (FileAttr*)pAttr;
   int i = psFileAttr->dwConfigItem;
   int nResult = EFAILED;
   AEECallback *pcbGetAttr = psFileAttr->pcbSVCCallback;
   AEECardSessionCmdStatus *pRegisterStatus = (AEECardSessionCmdStatus*)
                                               psFileAttr->pRegisterStatus;
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   CALLBACK_Cancel(psFileAttr->pcbSVCCallback);
   //FREEIF(psFileAttr->pcbSVCCallback); 
   if(SUCCESS == pRegisterStatus->nCmdStatus){
      AEECardSessionFilePathType sFilePath = {0};
       
      AEECardSessionFileAttribStatus *pCmdStatus = NULL;
      CALLBACK_Init(pcbGetAttr, (PFNNOTIFY)OEM_SVCFileAttrCb,
                          (void*)(psFileAttr));
      pCmdStatus= (AEECardSessionFileAttribStatus *)MALLOC(
                                             sizeof(AEECardSessionFileAttribStatus));
      if(NULL == pCmdStatus){
         FREEIF(pcbGetAttr);
         FREEIF(psFileAttr);
         pProvInfo->nStatus = ENOMEMORY;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
         return;
      }

      psFileAttr->pFileStatus = pCmdStatus;
      sFilePath.dwFileIndication = AEECARDSESSION_EF_FILE_NAME;
      switch(i)
      {
      case CFGI_MMS_CONFIG:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_CONFIG;
            break;
         }
      case CFGI_MMS_ICP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_ICP;
            break;
         }      
      case CFGI_MMS_UP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_UP;
            break;
         }
      case CFGI_BROWSER_CP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_BROWSER_CP;
            break;
         }
      case CFGI_JAVA_JDL:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_JAVA_DURL;
            break;
         }      
      default:
         {
            FREEIF(pCmdStatus);
            FREEIF(pcbGetAttr);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = EFAILED;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
            return ;
         }

      }

      nResult = ICARDSESSION_GetFileAttr(psFileAttr->pICardSession, 
                                         sFilePath,
                                         (AEECardSessionFileAttribStatus*)pCmdStatus,
                                         pcbGetAttr);
      if(SUCCESS != nResult){
         FREEIF(pcbGetAttr);
         RELEASEIF(psFileAttr->pICardSession);
         FREEIF(psFileAttr);
         FREEIF(pCmdStatus);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
         return ;         
         
      }
      pProvInfo->nStatus = nResult;      

   }else{
      FREEIF(pcbGetAttr);
      RELEASEIF(psFileAttr->pICardSession);
      FREEIF(psFileAttr);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
      return ;   
      
   }
   
}
/*==================================================================
Function: OEM_SVCRegisterLenCb

Description: Callback function for Register API

Prototype:
   void OEM_SVCRegisterLenCb(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVCRegisterLenCb(FileAttr *pAttr)
{
   FileAttr *psFileAttr = (FileAttr*)pAttr;
   int i = psFileAttr->dwConfigItem;
   int nResult = EFAILED;
   AEECallback  *pcbGetAttr = psFileAttr->pcbSVCCallback;
   AEECardSessionCmdStatus *pRegisterStatus = (AEECardSessionCmdStatus*)
                                               psFileAttr->pRegisterStatus;
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   CALLBACK_Cancel(psFileAttr->pcbSVCCallback);    
   if(SUCCESS == pRegisterStatus->nCmdStatus){
      AEECardSessionFilePathType sFilePath = {0};
      AEECardSessionFileAttribStatus *pCmdStatus = NULL;
      
      FREEIF(pRegisterStatus);
      pCmdStatus= (AEECardSessionFileAttribStatus *)MALLOC(
                                             sizeof(AEECardSessionFileAttribStatus));
      if(NULL == pCmdStatus){
         pProvInfo->nStatus = ENOMEMORY;         
         RELEASEIF(psFileAttr->pICardSession);
         FREEIF(pcbGetAttr);
         FREEIF(psFileAttr);
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
         return;
      }      
      pProvInfo->dwMemType = 0;
      psFileAttr->pProvInfo = pProvInfo;
      psFileAttr->pFileStatus = pCmdStatus; 
      psFileAttr->bRead = TRUE;                  
      CALLBACK_Init(pcbGetAttr, (PFNNOTIFY)OEM_SVCFileAttrCb,
                    (void*)(psFileAttr));            

      sFilePath.dwFileIndication = AEECARDSESSION_EF_FILE_NAME;
      switch(i){
      case CFGI_MMS_CONFIG_LEN:
      case CFGI_MMS_CONFIG:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_CONFIG;
            break;
         }
      case CFGI_MMS_ICP_LEN:
      case CFGI_MMS_ICP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_ICP;
            break;
         }
      case CFGI_MMS_UP_ATTRIBUTES:
      case CFGI_MMS_UP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_MMS_UP;
            break;
         }
      case CFGI_BROWSER_CP_LEN:
      case CFGI_BROWSER_CP:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_BROWSER_CP;
            break;
         }
      case CFGI_JAVA_JDL_LEN:
      case CFGI_JAVA_JDL:
         {
            sFilePath.dwFileName = AEECARDSESSION_CDMA_JAVA_DURL;
            break;
         }      
      default:
         {
            FREEIF(pCmdStatus);
            RELEASEIF(psFileAttr->pICardSession);
            FREEIF(pcbGetAttr);
            FREEIF(psFileAttr);
            pProvInfo->nStatus = EFAILED;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
            return ;
         }

      }
      nResult = ICARDSESSION_GetFileAttr( psFileAttr->pICardSession, 
                                          sFilePath,
                                          (AEECardSessionFileAttribStatus*)pCmdStatus,
                                          pcbGetAttr); 
      if(SUCCESS != nResult){
         FREEIF(pcbGetAttr);
         RELEASEIF(psFileAttr->pICardSession);
         FREEIF(psFileAttr);
         FREEIF(pCmdStatus);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
         return;
       
      }
      pProvInfo->nStatus = nResult;


   }else {
      FREEIF(pRegisterStatus);
      RELEASEIF(psFileAttr->pICardSession);
      FREEIF(psFileAttr);
      FREEIF(pcbGetAttr);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
   }
}
/*==================================================================
Function: OEM_SVC_EFS_Provision_Info

Description: Function to return EFS related file attributes 

Prototype:
   void OEM_SVC_EFS_Provision_Info(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVC_EFS_Provision_Info(FileAttr *pAttr)
{

   FileAttr *psFileAttr = (FileAttr*)pAttr;   
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   IFileMgr* pFileMgr = NULL;               
   IFile* pFile = NULL;
   int nResult = EFAILED;
   char *pszFileName = NULL;
   AEEFileInfo fileinfo = {0};  
   IShell * pIShell = AEE_GetShell();
   nResult = ISHELL_CreateInstance( pIShell,
                                    AEECLSID_FILEMGR,
                                    (void**)&pFileMgr);

   if(SUCCESS != nResult){           
      FREEIF(psFileAttr);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      return;
   }
   switch(psFileAttr->dwConfigItem)
   {                  
   case CFGI_MMS_CONFIG_LEN:
      {
         pszFileName = MMS_CONFIG_PROV_INFO;
         break;
      }
   case CFGI_MMS_ICP_LEN:
      {
         pszFileName = MMS_ICP_PROV_INFO;
         break;
      }
   case CFGI_MMS_UP_ATTRIBUTES:
      {
         pszFileName = MMS_UP_PROV_INFO;
         break;
      }
   case CFGI_BROWSER_CP_LEN:
      {
         pszFileName = BROWSER_CP_PROV_INFO;
         break;
      }
   case CFGI_JAVA_JDL_LEN:
      {
         pszFileName = JAVA_JDL_PROV_INFO;
         break;
      }      
   default:
      {                  
         FREEIF(psFileAttr);
         RELEASEIF(pFileMgr);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
         return;
      }

   }

   pFile = IFILEMGR_OpenFile(pFileMgr,
                             pszFileName,
                             _OFM_READ);
   if(!pFile){ 
      pProvInfo->dwMemType = 0;                             
      FREEIF(psFileAttr);
      RELEASEIF(pFileMgr);
      RELEASEIF(pFile);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      return;
   }
   nResult = IFILE_GetInfo(pFile, &fileinfo);   
   if(SUCCESS != nResult){
      FREEIF(psFileAttr);
      RELEASEIF(pFileMgr);
      RELEASEIF(pFile);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      return;
   }
   pProvInfo->dwMemType = MEMORY_TYPE_EFS;
   if(CFGI_MMS_UP_ATTRIBUTES != psFileAttr->dwConfigItem){
      *(uint32*)pProvInfo->pData = fileinfo.dwSize;
      pProvInfo->nStatus = SUCCESS;
   }else{
      *(uint32*)pProvInfo->pData = MMS_UP_RECORD_LEN;
      pProvInfo->dwRecordNumber = (fileinfo.dwSize)/MMS_UP_RECORD_LEN;
      pProvInfo->nStatus = SUCCESS;

   }                          
   FREEIF(psFileAttr);
   RELEASEIF(pFileMgr);
   RELEASEIF(pFile);               
   pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      
}
/*==================================================================
Function: OEM_SVC_EFS_GetData

Description: Function to return EFS related raw data  

Prototype:
   void OEM_SVC_EFS_GetData(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVC_EFS_GetData(FileAttr *pAttr)
{
   FileAttr *psFileAttr = (FileAttr*)pAttr;   
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   IFileMgr* pFileMgr;               
   IFile* pFile;
   char *pszFileName = NULL;  
   int nResult = EFAILED;
   IShell * pIShell = AEE_GetShell();

   nResult = ISHELL_CreateInstance(pIShell,
                                    AEECLSID_FILEMGR,
                                    (void**)&pFileMgr);

   if(SUCCESS != nResult){      
      FREEIF(psFileAttr);  
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      return;
   }
   switch(psFileAttr->dwConfigItem)
   {                  
   case CFGI_MMS_CONFIG:
      {
         pszFileName = MMS_CONFIG_PROV_INFO;
         break;
      }
   case CFGI_MMS_ICP:
      {
         pszFileName = MMS_ICP_PROV_INFO;
         break;
      }
   case CFGI_MMS_UP:
      {
         pszFileName = MMS_UP_PROV_INFO;
         break;
      }
   case CFGI_BROWSER_CP:
      {
         pszFileName = BROWSER_CP_PROV_INFO;
         break;
      }
   case CFGI_JAVA_JDL:
      {
         pszFileName = JAVA_JDL_PROV_INFO;
         break;
      }   
   default:
      {                        
         RELEASEIF(pFileMgr);         
         FREEIF(psFileAttr);  
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
         return ;
      }

   }

   pFile = IFILEMGR_OpenFile(pFileMgr,
                             pszFileName,
                             _OFM_READ);
   if(!pFile){                   
      pProvInfo->dwMemType = 0;
      RELEASEIF(pFileMgr);      
      FREEIF(psFileAttr);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
      return ;                  
   }
   if(CFGI_MMS_UP != psFileAttr->dwConfigItem){
      if(IFILE_Read(pFile,
                    (void*)pProvInfo->pData,
                    psFileAttr->nSize) == psFileAttr->nSize)
      {                 
         RELEASEIF(pFileMgr);
         RELEASEIF(pFile);
         FREEIF(psFileAttr);
         pProvInfo->dwMemType = MEMORY_TYPE_EFS;
         pProvInfo->nStatus = SUCCESS;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
         return;
      }
      else
      {   
         RELEASEIF(pFileMgr);
         RELEASEIF(pFile);                  
         FREEIF(psFileAttr);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
         return;                  
      }
   }else{
      if((2  >= pProvInfo->dwRecordNumber) && (0 < pProvInfo->dwRecordNumber )){
         nResult = IFILE_Seek(pFile, _SEEK_START, (pProvInfo->dwRecordNumber - 1) * MMS_UP_RECORD_LEN); 
         if(SUCCESS != nResult){
            RELEASEIF(pFileMgr);
            RELEASEIF(pFile);
            FREEIF(psFileAttr);
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
            return;
         }
         if(IFILE_Read(pFile,(void*)pProvInfo->pData, psFileAttr->nSize) == psFileAttr->nSize){
               RELEASEIF(pFileMgr);
               RELEASEIF(pFile);
               FREEIF(psFileAttr);
               pProvInfo->dwMemType = MEMORY_TYPE_EFS;
               pProvInfo->nStatus = SUCCESS;
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return;

         }
         else
         {   
            RELEASEIF(pFileMgr);
            RELEASEIF(pFile);                  
            FREEIF(psFileAttr);  
            pProvInfo->nStatus = EFAILED;
            pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);
            return;                  
         }
      }

   }
            
}
/*==================================================================
Function: OEM_SVC_WriteData

Description: Function to write to EFS file

Prototype:
   void OEM_SVC_WriteData(FileAttr *pAttr)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVC_WriteData(FileAttr *pAttr)
{
   IFileMgr* pFileMgr = NULL;  
   FileAttr *psFileAttr = (FileAttr*)pAttr;   
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   IFile* pFile = NULL;
   char *pszFileName = NULL;
   int nResult = EFAILED;
   IShell * pIShell = AEE_GetShell();                       
   nResult = ISHELL_CreateInstance(pIShell,
                                    AEECLSID_FILEMGR,
                                    (void**)&pFileMgr);

   if(SUCCESS != nResult){
      FREEIF(psFileAttr);
      pProvInfo->nStatus = EFAILED;
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
      return;
   }
   switch(psFileAttr->dwConfigItem)
   {                  
   case CFGI_MMS_CONFIG:
      {
         pszFileName = MMS_CONFIG_PROV_INFO;
         break;
      }
   case CFGI_MMS_ICP:
      {
         pszFileName = MMS_ICP_PROV_INFO;
         break;
      }
   case CFGI_MMS_UP:
      {
         pszFileName = MMS_UP_PROV_INFO;
         break;
      }
   case CFGI_BROWSER_CP:
      {
         pszFileName = BROWSER_CP_PROV_INFO;
         break;
      }
   case CFGI_JAVA_JDL:
      {
         pszFileName = JAVA_JDL_PROV_INFO;
         break;
      }   
   default:
      {                        
         RELEASEIF(pFileMgr);
         FREEIF(psFileAttr);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
         return;
      }

   } 
   pFile = IFILEMGR_OpenFile( pFileMgr,
                              pszFileName,
                              _OFM_READWRITE);
   if(!pFile) {                  
      pFile = IFILEMGR_OpenFile(pFileMgr,
                                 pszFileName,
                                 _OFM_CREATE);
   }
   if(pFile) {
      if(CFGI_MMS_UP != psFileAttr->dwConfigItem){
         if(IFILE_Write(pFile,
                        (void*)pProvInfo->pData,
                        psFileAttr->nSize) == psFileAttr->nSize){
               RELEASEIF(pFile);
               RELEASEIF(pFileMgr);
               pProvInfo->nStatus = SUCCESS;
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return ;
            } else {     
               pProvInfo->nStatus = EFAILED;
               RELEASEIF(pFile);
               //IFILEMGR_Remove(pFileMgr, pszFileName);
               RELEASEIF(pFileMgr);
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return;
            }
      }else{
         if(((2  >= pProvInfo->dwRecordNumber) && (0 < pProvInfo->dwRecordNumber ))
             || (MMS_UP_RECORD_LEN >= psFileAttr->nSize)){
            nResult = IFILE_Seek(pFile, _SEEK_START, (pProvInfo->dwRecordNumber - 1) * MMS_UP_RECORD_LEN); 
            if(SUCCESS != nResult){
               RELEASEIF(pFileMgr);
               RELEASEIF(pFile);
               FREEIF(psFileAttr);
               pProvInfo->nStatus = EFAILED;
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return;
            }
            if(IFILE_Write(pFile,(void*)pProvInfo->pData, psFileAttr->nSize) == psFileAttr->nSize){
               RELEASEIF(pFile);
               RELEASEIF(pFileMgr);
               FREEIF(psFileAttr);
               pProvInfo->dwMemType = MEMORY_TYPE_EFS;
               pProvInfo->nStatus = SUCCESS;
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return;
            }else{
               pProvInfo->nStatus = EFAILED;
               RELEASEIF(pFileMgr);
               RELEASEIF(pFile);
               FREEIF(psFileAttr);
               pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
               return;
            }
         }
      }
   }
   pProvInfo->nStatus = EFAILED;
   RELEASEIF(pFileMgr);
   pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);      
   return;
}
/*==================================================================
Function: OEM_SVCFileAttrCb

Description: Callback function 

Prototype:
   void OEM_SVCFileAttrCb(FileAttr *pAttr);

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVCFileAttrCb(FileAttr *pAttr)
{
   FileAttr *pFileAttrib = (FileAttr*)pAttr;   
   AEECardSessionFileAttribStatus *pFileStatus = 
                          (AEECardSessionFileAttribStatus*)pFileAttrib->pFileStatus;
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;   
   CALLBACK_Cancel(pFileAttrib->pcbSVCCallback);
   FREEIF(pFileAttrib->pcbSVCCallback);    
   if(pFileAttrib->bRead){
      if(SUCCESS == pFileStatus->nCmdStatus){
         //The file is in RUIM
         pProvInfo->dwMemType = MEMORY_TYPE_RUIM;      
         if(CFGI_MMS_UP_ATTRIBUTES != pFileAttrib->dwConfigItem){
            *(uint32*)pProvInfo->pData = pFileStatus->dwFileSize;
         }else{
            *(uint32*)pProvInfo->pData = pFileStatus->file.sLinearFileStatus.dwRecordLen;
            pProvInfo->dwRecordNumber = pFileStatus->file.sLinearFileStatus.dwNumberRecords;
         }
         FREEIF(pFileStatus);               
         RELEASEIF(pFileAttrib->pICardSession);
         FREEIF(pFileAttrib);        
            
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);            
      }else{
         //The file can be in EFS 
         FREEIF(pFileStatus);
         RELEASEIF(pFileAttrib->pICardSession);
         OEM_SVC_EFS_Provision_Info(pAttr);
      }
   }else{
      RELEASEIF(pFileAttrib->pICardSession);
      if(SUCCESS != pFileStatus->nCmdStatus){
         //File to be written is present on EFS
         OEM_SVC_WriteData(pFileAttrib);
      }else{
         FREEIF(pFileStatus);               
         RELEASEIF(pFileAttrib->pICardSession);
         FREEIF(pFileAttrib);
         pProvInfo->nStatus = EFAILED;
         pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData);

      }

   }

                                                
}
/*==================================================================
Function: OEM_SVCReadDataCb

Description: Read data callback function

Prototype:
   void OEM_SVCReadDataCb(FileAttr *pAttr);

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SVCReadDataCb(FileAttr *pAttr)
{
   FileAttr *pFileAttrib = (FileAttr*)pAttr;
   int i = pFileAttrib->dwConfigItem;
   int nResult = EFAILED;
   
   AEECardSessionReadTpStatus *pReadStatus = 
                          (AEECardSessionReadTpStatus*)pFileAttrib->pReadData;
   AEECardSessionReadRecStatus *pReadRecStatus = 
                          (AEECardSessionReadRecStatus*)pFileAttrib->pReadRecData;
   OEMProvisionInfo *pProvInfo = (OEMProvisionInfo *)pAttr->pProvInfo;
   CALLBACK_Cancel(pFileAttrib->pcbSVCCallback);
   FREEIF(pFileAttrib->pcbSVCCallback);
   RELEASEIF(pFileAttrib->pICardSession);
   if(CFGI_MMS_UP != i){
      nResult = pReadStatus->nCmdStatus;
   }else{
      nResult = pReadRecStatus->nCmdStatus; 
   }
   
   //If command status is success, file is present on RUIM
   if(SUCCESS == nResult){
      
      pProvInfo->dwMemType = MEMORY_TYPE_RUIM;

      if(pAttr->bTransp){
         (void)MEMCPY((void*)pProvInfo->pData, 
                (void*)pReadStatus->pReadData->pData, 
                pFileAttrib->nSize);
         FREEIF(pReadStatus->pReadData->pData);
         FREEIF(pReadStatus->pReadData);          
         FREEIF(pReadStatus);
         FREEIF(pFileAttrib->pcbSVCCallback);
         FREEIF(pFileAttrib);

      }else{
         (void)MEMCPY((void*)pProvInfo->pData, 
                (void*)pReadRecStatus->pReadData->pData, 
                pFileAttrib->nSize);
         FREEIF(pReadRecStatus->pReadData->pData);
         FREEIF(pReadRecStatus->pReadData);          
         FREEIF(pReadRecStatus);
         FREEIF(pFileAttrib->pcbSVCCallback);
         FREEIF(pFileAttrib);
      }
      pProvInfo->pcbUserCallback->pfnNotify(pProvInfo->pcbUserCallback->pNotifyData); 
      return;
   }else{
      if(pAttr->bTransp){
         FREEIF(pReadStatus->pReadData->pData); 
         FREEIF(pReadStatus->pReadData);
         FREEIF(pReadStatus);
      }else{
         FREEIF(pReadRecStatus->pReadData->pData); 
         FREEIF(pReadRecStatus->pReadData);
         FREEIF(pReadRecStatus);
      }
      OEM_SVC_EFS_GetData(pFileAttrib);
      return;

   }

}

#endif

/*==================================================================
Function: OEM_SVCGetConfig

Description: Retrieves configuration information from the OEM

Prototype:
   int  OEM_SVCGetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_SVCGetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   IShell *pShell = AEE_GetShell();
#ifdef FEATURE_OEMOMH
    if (i == CFGI_DOWNLOAD || i == CFGI_SUBSCRIBERID ||
    i == CFGI_SUBSCRIBERID_LEN)
    {
        if (OEMOMH_GetConfig(i, pBuff, nSize) == SUCCESS)
            return SUCCESS;
    }
#endif   
   //
   // NOTE: This code should be replaced by code that stores/retrieves values in
   // NV-RAM.  You will need to add NV items for each of the items referenced here.
   //
//#ifdef FEATURE_APP_SVCPRG
   /* found a match in our cache */
   if (OEM_GetCachedConfig(i, pBuff, nSize) == 0)
     return 0;
//#endif  //def FEATURE_APP_SVCPRG

   // Make sure we have a valid buffer
   if(!pBuff)
      return EBADPARM;


    switch (i)
    {

   case CFGI_USER_CLOCK_OFFSET:
      {
         IFileMgr* pFileMgr;
         int32 *pdwOffset = (int32 *)pBuff;
         PACONTEXT pacLast;
         int nErr;

         if(nSize != sizeof(int32))
            return(EBADPARM);

         MEMSET(pdwOffset, 0, nSize);

         pacLast = AEE_EnterAppContext(NULL);
         nErr = ISHELL_CreateInstance(pShell,
                               AEECLSID_FILEMGR,
                                     (void**)&pFileMgr);
         if(SUCCESS == nErr)
         {
            IFile* pFile;
            boolean bWriteFile = FALSE;
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                    USER_CLOCK_OFFSET_FILE,
                                      _OFM_READ);
            if(pFile)
            {
               /* Read from File */
               if(IFILE_Read(pFile,
                                 (void*)pdwOffset,
                             sizeof(int32)) == sizeof(int32))
               {
                  /* Successful Read. */
                  IFILE_Release(pFile);
                  }
               else
               {
                  /* Read failed for some reason. Destroy the file and re-write. */
                  IFILE_Release(pFile);
                  IFILEMGR_Remove(pFileMgr, USER_CLOCK_OFFSET_FILE);
                  bWriteFile = TRUE;
               }
            }
            else
            {
               bWriteFile = TRUE;
            }

            if(bWriteFile)
            {

               /* No file there yet. Write out defaults. */
               pFile = IFILEMGR_OpenFile(pFileMgr,
                                       USER_CLOCK_OFFSET_FILE,
                                         _OFM_CREATE);
               if(pFile)
               {
                  /* File opened successfully. Write the data */
                  if(IFILE_Write(pFile,
                                 pdwOffset,
                                 sizeof(int32)) == sizeof(int32))
                  {
                     /* Successful write. Close the file and return.*/
                     IFILE_Release(pFile);
                     }
                  else
                  {
                     /* Something went wrong. Destroy the file. */
                     IFILE_Release(pFile);
                     IFILEMGR_Remove(pFileMgr, USER_CLOCK_OFFSET_FILE);
                     IFILEMGR_Release(pFileMgr);
                     AEE_LeaveAppContext(pacLast);
                     return EFAILED;
                  }
               }
               else
               {
                  IFILEMGR_Release(pFileMgr);
                  AEE_LeaveAppContext(pacLast);
                  return EFAILED;
               }
            }
            IFILEMGR_Release(pFileMgr);
         }
         AEE_LeaveAppContext(pacLast);

         return SUCCESS;
      }

#if defined(FEATURE_GSM1x)
      case CFGI_GSM1X_PRL:
         {
           AEEGSM1xPRLInfo *PRLInfoPtr = (AEEGSM1xPRLInfo *)pBuff;
           if( PRLInfoPtr->maxSizeBytes < sizeof(nv_ram_roaming_list_type) ){
               return EFAILED;
           }
           OEM_GetGSM1xPRL( PRLInfoPtr );
         }
         return SUCCESS;

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
      case CFGI_GSM1X_RTRE_CONFIG:
         {
           AEEGSM1xRTREConfig *RTREPtr = (AEEGSM1xRTREConfig *)pBuff;
           OEM_GetGSM1xRTREConfig( RTREPtr );
         }
         return SUCCESS;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      case CFGI_GSM1X_IDENTITY_PARAMS:
         {
           AEEGSM1xIdentityParams *IdentityParamsPtr = (AEEGSM1xIdentityParams *)pBuff;
           OEM_GetGSM1xIdentityParams( IdentityParamsPtr );
         }
         return SUCCESS;

      case CFGI_GSM1X_SID_NID_PARAMS:
         {
           AEEGSM1xSIDNIDParams *SIDNIDParamsPtr = (AEEGSM1xSIDNIDParams *)pBuff;
           OEM_GetGSM1xSIDNIDParams( SIDNIDParamsPtr );
         }
      return SUCCESS;
#endif /* FEATURE_GSM1x */

   //
   // DNS Addresses - Used by the HTTP layer, etc.
   //

      case CFGI_DNS_IP1:
         if( nSize != sizeof( uint32 ) ) {
            return EBADPARM;
         }
         {
            nv_item_type nvItem;
            if(NV_DONE_S == ui_get_nv(NV_PRIMARY_DNS_I, &nvItem))
         {
               *(uint32*)pBuff = HTONL(nvItem.primary_dns);
               return SUCCESS;
            }
            else
               return EFAILED;
         }

      case CFGI_DNS_IP2:
         if( nSize != sizeof( uint32 ) ) {
            return EBADPARM;
         }
         {
            nv_item_type nvItem;
            if(NV_DONE_S == ui_get_nv(NV_SECONDARY_DNS_I, &nvItem))
            {
               *(uint32*)pBuff = HTONL(nvItem.secondary_dns);
               return SUCCESS;
            }
            else
               return EFAILED;
         }

   //
   // IPv6 DNS Addresses
   //

#ifdef FEATURE_DATA_PS_IPV6
      case CFGI_IPV6_DNS_IP1:
         if( nSize != sizeof( INAddr6 ) ) {
            return EBADPARM;
         }
         {
            nv_item_type nvItem;
            if(NV_DONE_S == ui_get_nv(NV_IPV6_PRIMARY_DNS_I, &nvItem))
            {
               MEMCPY((byte*)pBuff, (byte*)&nvItem.ipv6_primary_dns, sizeof(INAddr6));
               return SUCCESS;
            }
            else
               return EFAILED;
         }

      case CFGI_IPV6_DNS_IP2:
         if( nSize != sizeof( INAddr6 ) ) {
            return EBADPARM;
         }
         {
            nv_item_type nvItem;
            if(NV_DONE_S == ui_get_nv(NV_IPV6_SECONDARY_DNS_I, &nvItem))
            {
               MEMCPY((byte*)pBuff, (byte*)&nvItem.ipv6_secondary_dns, sizeof(INAddr6));
               return SUCCESS;
            }
            else
               return EFAILED;
         }
#endif //FEATURE_DATA_PS_IPV6

      case CFGI_MOBILE_DIR_NUM_LEN:
         {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
            int *             pLen = pBuff;
            boolean           bFound = FALSE;
            pbm_return_type   pbmRet;
            uint16            rec_id;
            pbm_field_s_type *fields_buf = NULL;
            int               fields_size;
            int               nItemCount, i;


            pbmRet = pbm_enum_rec_init(PBM_MSISDN, PBM_CAT_SIM_MSISDN, PBM_FIELD_NONE, 
                                        NULL, 0, 0);

            if(pbmRet == PBM_SUCCESS)
            {
               pbmRet = pbm_enum_next_rec_id(&rec_id);
               if (pbmRet == PBM_SUCCESS)
               {
                  //read the actual record
                  fields_size = pbm_calculate_fields_size_from_id(rec_id);
                  fields_buf = MALLOC(fields_size);

                  if (fields_buf == NULL)
                  {
                     return ENOMEMORY;
                  }
            
                  pbmRet = pbm_record_read(rec_id, NULL, &nItemCount, (uint8*)fields_buf, fields_size);
                  if (pbmRet == PBM_SUCCESS)
                  {
                     for(i=0; i < nItemCount;i++) 
                     {
                        // If AEEDB_FT_STRING or AEEDB_FT_PHONE, need to convert to AECHAR.  
                        if (fields_buf[i].field_type == PBM_FT_PHONE) 
                        {
                           //Found it
                           bFound = TRUE;
                           *pLen = fields_buf[i].data_len;
                           break;
                        }
                     }
                  }

                  FREE(fields_buf);
               }
            }

            if(!bFound)
            {
            *pLen = STRLEN(DEFAULT_MDN) + 1;
            }

            return SUCCESS;

#else
            nv_item_type      nvi;
            int *             pLen = pBuff;

            if( !pLen || nSize != sizeof(int) ){
               return EBADPARM;
            }
            if( ui_get_nv(NV_CURR_NAM_I, &nvi) != NV_DONE_S ){
               return EFAILED;
            }else{
              nvi.dir_number.nam = nvi.curr_nam;
              if( ui_get_nv(NV_DIR_NUMBER_I, &nvi) != NV_DONE_S){
               return EFAILED;
                }else{
                   int nLen = STRLEN((char*)&nvi.dir_number.dir_number) + 1;
                   *pLen = MIN(nLen, sizeof(nvi.dir_number.dir_number) + 1);
                }
            }
#endif //defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
         }
         return AEE_SUCCESS;

#ifndef FEATURE_UIONE_HDK
        case CFGI_KB_AUTOREPEAT:
            {
                OEMKBAutoRepeat* pautoRepeat = (OEMKBAutoRepeat*)pBuff;
                pautoRepeat->dwStart = 0;
                pautoRepeat->dwRate  = 0;
                //pautoRepeat->dwStart = KB_AUTOREPEAT_START;
                //pautoRepeat->dwRate = KB_AUTOREPEAT_RATE;
            }
            return AEE_SUCCESS;
#endif // !FEATURE_UIONE_HDK

      case CFGI_MOBILE_DIR_NUM:
      {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
            boolean           bFound = FALSE;
            pbm_return_type   pbmRet;
            uint16            rec_id;
            pbm_field_s_type *fields_buf = NULL;
            int               fields_size;
            int               nItemCount, i;

            pbmRet = pbm_enum_rec_init(PBM_MSISDN, PBM_CAT_SIM_MSISDN, PBM_FIELD_NONE, 
                                        NULL, 0, 0);

            if(pbmRet == PBM_SUCCESS)
            {
               pbmRet = pbm_enum_next_rec_id(&rec_id);
               if (pbmRet == PBM_SUCCESS)
               {
                  //read the actual record
                  fields_size = pbm_calculate_fields_size_from_id(rec_id);
                  fields_buf = MALLOC(fields_size);
            
                  if (fields_buf == NULL)
                  {
                     return ENOMEMORY;
                  }

                  pbmRet = pbm_record_read(rec_id, NULL, &nItemCount, (uint8*)fields_buf, fields_size);
                  if (pbmRet == PBM_SUCCESS)
                  {
                     for(i=0; i < nItemCount;i++) 
                     {
                        // If PBM_FT_PHONE we found a number.  
                        if (fields_buf[i].field_type == PBM_FT_PHONE) 
                        {
                           //Found it
                           if(nSize == fields_buf[i].data_len)
                           {
                              bFound = TRUE;
                              MEMCPY(pBuff, pbm_get_field_data_ptr(fields_buf,i), nSize);
                              break;
                           }
                        }
                     }
                  }
                  FREE(fields_buf);
               }
            }
            
            if(!bFound)
            {
               if(nSize == STRLEN(DEFAULT_MDN) + 1)
               {
                  MEMCPY(pBuff, DEFAULT_MDN, nSize);
               }
               else
               {
            return EBADPARM;
         }
            }
            return SUCCESS;

#else
         nv_item_type      nvi;
         char *            szBuff = (char *)pBuff;

         if( !szBuff || !nSize ){
            return EBADPARM;
         }
         if( ui_get_nv(NV_CURR_NAM_I, &nvi) != NV_DONE_S ){
            return EFAILED;
         }else{
          //  ui_get_nv(NV_CURR_NAM_I, &nvi);
            // cbCurrentNam = pMobileInfo->nCurrNAM = nvi.curr_nam;
           nvi.dir_number.nam = nvi.curr_nam;
           if( ui_get_nv(NV_DIR_NUMBER_I, &nvi) != NV_DONE_S){
            return EFAILED;
             }else{
               STRLCPY(szBuff, (char *)nvi.dir_number.dir_number, MIN(nSize, sizeof(nvi.dir_number.dir_number)+1));
             }
         }
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
      }
      return AEE_SUCCESS;


   //
   // MA Capability Flags - specific to the carrier/handset.
   // See AEEMutualAuth.h for full documentation for setting MA Capability flags.
   //

   case CFGI_BREWMA_CAPABILITIES:

	  if( nSize != sizeof( uint32 ) ) {
		 return EBADPARM;
	  }

	  // Bit 0 (LSB) is set dynamically on the handset by getting the serving
	  // system info from ITelephone
	  // 0x00000001 MA_CLIENT_CAP_MODE_NO_TIME_SYNC Dynamic - from ITelephone

	  // Bit 1 & 2 are ALWAYS hard coded by BREW, and ALWAYS set ON.
	  // Any values set by the OEM for bits 1 & 2 will be overridden.

	  // 0x00000002 MA_CLIENT_CAP_SUPPORTS_NN	      Hard coded ON by BREW
	  // 0x00000004 MA_CLIENT_CAP_SUPPORTS_TT	      Hard coded ON by BREW
	  //
	  // The remaining defined bits should be set by the OEM.
	  // As a basic default, we assume a battery backed RTC - all other caps
	  // are off by default or are presently unused.
	  // 0x00000008	MA_CLIENT_CAP_BATTERY_BACKED_RTC	OEM Config Item

	  (*(uint32*)pBuff) = (MA_CLIENT_CAP_BATTERY_BACKED_RTC);

	  return(SUCCESS);



                

   //
   // Mobile ID information - generally, this is the MIN
   //
      case CFGI_MOBILEINFO:
        if (nSize != sizeof(AEEMobileInfo))
           return(EBADPARM);
           GetMobileInfo((AEEMobileInfo *) pBuff);

         return SUCCESS;

   //
   // AutoStart Application - This app will be started by BREW after AEE_Init
   // completes and the next time AEE_Dispatch is called.
   //

      case CFGI_AUTOSTART:
         {
         AEECLSID * pc = (AEECLSID *)pBuff;

         if(nSize != sizeof(AEECLSID))
            return(EBADPARM);
#ifdef FEATURE_UI_CORE
#ifdef FEATURE_UIONE_HDK
         if (TestApp_SafeMode_OEM_IsAutoStartApp()) {
            *pc = AEECLSID_TESTAPP_SAFEMODE;
         } else {
         *pc = AEECLSID_PHONEAPP;   //phoneapp
         }
#else // ! FEATURE_UIONE_HDK
         //*pc = AEECLSID_CORE_STARTAPP;
         *pc = AEECLSID_CORE_APP;
#endif //FEATURE_UIONE_HDK
#else
         *pc = 0;      
#endif /* FEATURE_UI_CORE */

#ifdef FEATURE_MANGO_UI
         *pc = AEECLSID_BREWPORT;
#endif
         return SUCCESS;
         }

      case CFGI_FIRMWARE_ID_LEN:
         if(nSize != sizeof(int))
            return EBADPARM;
         return GetSoftwareVersion(NULL, (int *)pBuff);

      case CFGI_FIRMWARE_ID:
      {
         int nVerLen;
         if(GetSoftwareVersion(NULL, &nVerLen))
            return EUNSUPPORTED;
         if(nSize < nVerLen)
            return EBADPARM;
         return GetSoftwareVersion((byte *)pBuff, &nVerLen);
      }

      case CFGI_SYSMEM_SIZE:
      {
         /* Default SYSMEM Size is 4kB. Let's use larger size due
            to large number of UI Apps. */
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = 8192;
         return AEE_SUCCESS;
      }

#if defined (FEATURE_UIONE_HDK) || defined (FEATURE_EXT_USB_P1)
      /* Bump up dispatch time to prevent brew throttling
         during high speed SD writes using IFILE_Write() */
      case CFGI_MAX_DISPATCH_TIME:
      {
         if(nSize < sizeof(uint32))
            return EBADPARM;
#if defined (FEATURE_UIONE_HDK)
         *(uint32*)pBuff = 3000;
#else
         *(uint32*)pBuff = 450;
#endif
         return AEE_SUCCESS;
      }
#else
      case CFGI_MAX_DISPATCH_TIME:
        return EUNSUPPORTED;
#endif  /* FEATURE_EXT_USB_P1 || FEATURE_UIONE_HDK */

      case CFGI_BUSY_CURSOR_OFFSET:
      case CFGI_DOWNLOAD_BUFFER:
      case CFGI_HTTP_BUFFER:
      case CFGI_NET_CONNTIMEOUT:
      case CFGI_MIN_IDLE_TIME:
      case CFGI_SLEEP_TIMER_RESOLUTION:
      case CFGI_DOWNLOAD_FS_INFO:
      case CFGI_SCREEN_SAVER:
      case CFGI_FILE_CACHE_INFO:
      case CFGI_GPSONE_TRANSPORT:
      case CFGI_GPSONE_SVRIP:
      case CFGI_GPSONE_SVRPORT:
         return EUNSUPPORTED;

        case CFGI_CLOSE_KEYS:
            if ((nSize != sizeof(OEMCloseKeys)) || (pBuff == NULL))
            {
                return EBADPARM;
            }
            
            {
                OEMCloseKeys *pOCK = (OEMCloseKeys *)pBuff;
                pOCK->wCloseApp = AVK_CLR;
                pOCK->evtCloseApp = EVT_KEY;
                pOCK->wCloseAllApps = AVK_END;
                pOCK->evtCloseAllApps = EVT_KEY;
            }
            return SUCCESS;
        
      case CFGI_OEMAUTH_CHALLENGE_CAP:
      {
         if(nSize < sizeof(boolean))
		   	return EBADPARM;

         OEM_GetBrewConfig(i, pBuff, nSize);
         return SUCCESS;
	   }

	   case CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN:
	   case CFGI_OEMAUTH_RUIM_CARDID:
	   {
         if(nSize < sizeof(uint32))
            return EBADPARM;

         OEM_GetBrewConfig(i, pBuff, nSize);
         return SUCCESS;
	   }
   //
   // Default file system limits for apps.
   //

      case CFGI_MODULE_FSLIMIT:
      {
         MIFFSLimit *   pfsLimit = (MIFFSLimit *)pBuff;

         if (nSize < sizeof(MIFFSLimit))
            return EBADPARM;

#if !defined(AEE_SIMULATOR)
         //
         // NOTE: OEMs can enforce their (per module) defaults here...
         //
         pfsLimit->wMaxFiles = MAX_FILES_UNLIMITED;
         pfsLimit->dwMaxSpace = MAX_SPACE_UNLIMITED;
#else
#error Simulator CFGI_MODULE_FSLIMIT code is located in a different file
#endif // !defined(AEE_SIMULATOR)

         return SUCCESS;
      }

   //
   // Card ID (RUIM/SIM) support
   //
#if defined (FEATURE_UIM_RUIM)
      case CFGI_CARDID_LEN:
      case CFGI_CARDID:
      {
         int   nErr = SUCCESS;
         byte *buffer;

         buffer = MALLOC(CARD_ID_SIZE);
         if(!buffer){
           return(EFAILED);
         }
#ifdef FEATURE_MMGSDI
   #ifdef FEATURE_MULTIPROCESSOR
         gsdi_sim_read(UIM_CDMA_RUIM_ID,
                       GSDI_NO_INDEX,
                       0,
                       NULL,
                       0,
                       NULL,
                       CARD_ID_SIZE,
                       0,
                       NULL,
                       0,
                       OEM_SVCGsdiReadCb);

         ui_wait(UI_RUIM_SIG);
         if(!sim_read_response.success)
         {
           return(EFAILED);
         }

         if(i == CFGI_CARDID_LEN){
            if(nSize == sizeof(int)){
               *((int*)pBuff) = CARD_ID_SIZE;
            }else{
               nErr = EBADPARM;
            }
         }else{
            nSize = MIN(nSize, CARD_ID_SIZE);
            MEMCPY(pBuff, &(sim_read_response.global_data[1]), nSize);
         }
         return(nErr);
#else

         // Prepare to read the id EF.
         gUimCmd.access_uim.hdr.command            = UIM_ACCESS_F;
         gUimCmd.access_uim.hdr.cmd_hdr.task_ptr   = NULL;
         gUimCmd.access_uim.hdr.cmd_hdr.sigs       = 0;
         gUimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;
         gUimCmd.access_uim.hdr.options            = UIM_OPTION_ALWAYS_RPT;
         gUimCmd.access_uim.hdr.protocol           = UIM_CDMA;
         gUimCmd.access_uim.hdr.rpt_function       = UIM_Report;

         gUimCmd.access_uim.item      = UIM_CDMA_RUIM_ID;
         gUimCmd.access_uim.access    = UIM_READ_F;
         gUimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;
         gUimCmd.access_uim.num_bytes = CARD_ID_SIZE;
         gUimCmd.access_uim.offset    = 1;
         gUimCmd.access_uim.data_ptr  = buffer;

         // From nvruim_access():  Access an EF, do not signal any task, use no
         // signal, no done queue, use a callback, always report status.

         // Send the command to the R-UIM:
         (void) rex_clr_sigs(&ui_tcb, UI_RUIM_SIG);
         uim_cmd(&gUimCmd);
         ui_wait(UI_RUIM_SIG);

         if ((gCallBack.rpt_type   != UIM_ACCESS_R) ||
            (gCallBack.rpt_status != UIM_PASS)) {
            FREE(buffer);
            return(EFAILED);
         }

         if(i == CFGI_CARDID_LEN){
            if(nSize == sizeof(int)){
               *((int*)pBuff) = gUimCmd.access_uim.num_bytes_rsp;
            }else{
               nErr = EBADPARM;
            }
         }else{
            nSize = MIN(nSize, gUimCmd.access_uim.num_bytes_rsp);
            MEMCPY(pBuff, buffer, nSize);
         }
         FREE(buffer);
         return(nErr);
   #endif /* !FEATURE_MULTIPROCESSOR */
#endif /* !FEATURE_MMGSDI */
      }
#endif // defined (FEATURE_UIM_RUIM)

      case CFGI_DEBUG_KEY:
         return EUNSUPPORTED;

      case CFGI_ALLOW_3G_2G_FAILOVER:
         if(nSize != sizeof(boolean))
            return EBADPARM;
         // Allow failover
         *(boolean*)pBuff = TRUE;
         return SUCCESS;

#ifdef FEATURE_UIONE_HDK
      case CFGI_SAFEMODE_TIMER:
         if(nSize != sizeof(int32)) return EBADPARM;
         return TestApp_SafeMode_OEM_GetSafeModeTimer((int32*)pBuff);

      case CFGI_SAFEMODE_STARTMODE:
         if(nSize != sizeof(int32)) return EBADPARM;
         return TestApp_SafeMode_OEM_GetSafeModeStartMode((int*)pBuff);
#else

#if defined(FEATURE_BREW_DEV)
      // short safe mode timer during development
      case CFGI_SAFEMODE_TIMER:
         if(nSize != sizeof(int32))
            return EBADPARM;
         *(int32*)pBuff = 5;
         return SUCCESS;
#endif // defined(FEATURE_BREW_DEV)

#endif 

      case CFGI_SMS_EMAIL_DEST:
         if(nSize < (STRLEN("6245") + 1))
            return EBADPARM;
         STRLCPY((char*)pBuff, "6245", STRLEN("6245"));
         return SUCCESS;

      case CFGI_SMS_EMAIL_DEST_LENGTH:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = STRLEN("6245") + 1;
         return SUCCESS;

      // Maximum payload length in terms of number of bytes
     case CFGI_SMS_MAX_PAYLOAD_LENGTH:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         return GetPayloadLength((uint32*)pBuff);

      // Default encoding for MO SMS
      case CFGI_SMS_DEFAULT_MO_ENC:
         if(nSize < sizeof(AEESMSEncType))
            return EBADPARM;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
         *(AEESMSEncType*)pBuff = AEESMS_ENC_UNICODE;
#else
         *(AEESMSEncType*)pBuff = AEESMS_ENC_ASCII;
#endif
         return SUCCESS;

      // Count of encoding types available for MO SMS
      case CFGI_SMS_MO_ENC_TYPES_COUNT:
         if(nSize < sizeof(uint32))
            return EBADPARM;
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
         *(uint32*)pBuff = 3;
#else
         *(uint32*)pBuff = 5;
#endif
         return SUCCESS;

      // Encoding types available for MO SMS
      case CFGI_SMS_MO_ENC_TYPES:
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
         if(nSize < sizeof(uint32)*3)
            return EBADPARM;
         *(uint32*)pBuff = AEESMS_ENC_UNICODE;
         *(((uint32*)pBuff) + 1) = AEESMS_ENC_OCTET;
         *(((uint32*)pBuff) + 2) = AEESMS_ENC_GSM_7_BIT_DEFAULT;
#else
         if(nSize < sizeof(uint32)*5)
            return EBADPARM;
         *(uint32*)pBuff = AEESMS_ENC_ASCII;
         *(((uint32*)pBuff) + 1) = AEESMS_ENC_LATIN;
         *(((uint32*)pBuff) + 2) = AEESMS_ENC_UNICODE;
         *(((uint32*)pBuff) + 3) = AEESMS_ENC_OCTET;
         *(((uint32*)pBuff) + 4) = AEESMS_ENC_GSM_7_BIT_DEFAULT;
#endif
         return SUCCESS;

      case CFGI_BREWDIR_SMS_TELESERVICE:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = 4098;
        return SUCCESS;

      // If ON_ACCESS is set, the SMS message is sent on the access channel
      // If ON_TRAFFIC is set, it is sent on the traffic channel
      // If both are set, it will attempt the access channel, and use the
      // traffic channel if the message is too large.
      case CFGI_SMS_MO_ON_ACCESS_CHANNEL:
         if(nSize < sizeof(boolean))
            return EBADPARM;
         return GetAccess((boolean*)pBuff);

      case CFGI_SMS_MO_ON_TRAFFIC_CHANNEL:
        if(nSize < sizeof(boolean))
            return EBADPARM;
         return GetTraffic((boolean*)pBuff);

      case CFGI_LNG:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = GetLngCode();
         return SUCCESS;

      case CFGI_SMS_GW_DOMAIN_PREF:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         return OEMSMS_GetDomainPref((int*)pBuff);

      case CFGI_SMS_VERIFY_BREW_HANDLER:
         if (nSize < sizeof(boolean)) {
            return EBADPARM;
         }
         *((boolean *)pBuff) = TRUE;         
         return SUCCESS;

      case CFGI_BACKLIGHT_CLASSIDS_LIST:
         if(nSize < sizeof(uint32)*2)
            return EBADPARM;
         *(uint32*)pBuff = AEECLSID_BACKLIGHT;
         *(((uint32*)pBuff) + 1) = 0;
         return SUCCESS;

      case CFGI_GPSONE_LOCK:
         {
            nv_item_type nvItem;
            if(nSize < sizeof(dword))
               return EBADPARM;
            if( ui_get_nv(NV_GPS1_LOCK_I, &nvItem) != NV_DONE_S ){
               return EFAILED;
            }else{
               *(dword*)pBuff = nvItem.gps1_lock;
               return SUCCESS;
            }
         }

      // Type boolean, if TRUE, OEM app Context notifications sent via
      // OEM_Notify, else not.
      case CFGI_OEM_NOTIFY_APP_CTXT:
         if(nSize < sizeof(boolean))
         {
            return EBADPARM;
         }
         *(boolean*)pBuff = FALSE;
         return SUCCESS;

      // TYpe boolean. TRUE if pending ACKS to be flushed after first data app's launch 
      case CFGI_LAZY_ACK:
         if(nSize < sizeof(boolean))
            return EBADPARM;
         *(boolean*)pBuff = TRUE;
         return SUCCESS;
      
      case CFGI_LAST_VALID_TIME:
      {
         IFileMgr* pFileMgr;
         JulianType Date;
         LastValidCfg *pConfig = (LastValidCfg *)pBuff;
         uint64 secs=0;
         PACONTEXT pacLast;
         int nErr;

	     AEECLSID callerID = AEE_GetAppContextCls(AEE_GetAppContext());

	     if(nSize != sizeof(LastValidCfg))
            return(EBADPARM);

         MEMSET(pConfig, 0, nSize);


         // Default time in micro-seconds is set to Apr 14th, 2005 - 09:30:30
         Date.wYear    = 2005;
         Date.wMonth   = 4;
         Date.wDay     = 14;
         Date.wHour    = 9;
         Date.wMinute  = 30;
         Date.wSecond  = 30;
         Date.wWeekDay = 3;

         secs = (uint64) JULIANTOSECONDS(&Date);
	pConfig->qwTime = secs * 1000 * 1000; // in microseconds

	pConfig->OwnerCls = callerID;

         pacLast = AEE_EnterAppContext(NULL);
         nErr = ISHELL_CreateInstance(pShell,
                                     AEECLSID_FILEMGR,
                                     (void**)&pFileMgr);
         if(SUCCESS == nErr)
         {
            IFile* pFile;
            boolean bWriteFile = FALSE;
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                      LAST_VALID_TIMEINFO_FILE,
                                      _OFM_READ);
            if(pFile)
            {
               /* Read from File */
               if(IFILE_Read(pFile,
                             (void*)pConfig,
                             sizeof(LastValidCfg)) == sizeof(LastValidCfg))
               {
                  /* Successful Read. */
                  IFILE_Release(pFile);
               }
               else
               {
                  /* Read failed for some reason. Destroy the file and re-write. */
                  IFILE_Release(pFile);
                  IFILEMGR_Remove(pFileMgr, LAST_VALID_TIMEINFO_FILE);
                  bWriteFile = TRUE;
               }
            }
            else
            {
               bWriteFile = TRUE;
            }

			if(bWriteFile)
            {

               /* No file there yet. Write out defaults. */
               pFile = IFILEMGR_OpenFile(pFileMgr,
                                         LAST_VALID_TIMEINFO_FILE,
                                         _OFM_CREATE);
               if(pFile)
               {
                  /* File opened successfully. Write the data */
                  if(IFILE_Write(pFile,
                                 pConfig,
                                 sizeof(LastValidCfg)) == sizeof(LastValidCfg))
                  {
                     /* Successful write. Close the file and return.*/
                     IFILE_Release(pFile);
                  }
                  else
                  {
                     /* Something went wrong. Destroy the file. */
                     IFILE_Release(pFile);
                     IFILEMGR_Remove(pFileMgr, LAST_VALID_TIMEINFO_FILE);
		     IFILEMGR_Release(pFileMgr);
                     AEE_LeaveAppContext(pacLast);
                     return EFAILED;
                  }
               }
               else
               {
                  IFILEMGR_Release(pFileMgr);
                  AEE_LeaveAppContext(pacLast);
                  return EFAILED;
               }
            }
            IFILEMGR_Release(pFileMgr);
         }
         AEE_LeaveAppContext(pacLast);

         return nErr;
      }

      case CFGI_REQUIREPINENTRY:
      {
         nv_item_type nvItem;
         int nRet;

         if(nSize != sizeof(boolean))
         {
            return EBADPARM;
         }

         nRet = ui_get_nv(NV_AUTO_LOCK_I, &nvItem);
         if(NV_DONE_S == nRet)
         {
            *(boolean *)pBuff = nvItem.auto_lock;
            return SUCCESS;
         }
         else if (NV_NOTACTIVE_S == nRet)
         {
            //If the NV item is not active,
            //then set pBuff to the default value (false)
            *(boolean *)pBuff = 0;
            return SUCCESS;
         }
         return EFAILED;
      }

#ifdef FEATURE_MFLO
   case CFGI_MFLO_PROXY:
   {
      nv_item_type nvItem;
      int i;
      byte* pBBuff = (byte *)pBuff;
      MEMSET(pBBuff, 0, nSize);

      if(nSize > MF_PROXY_MAX_LEN*sizeof(byte))
      {
         return EBADPARM;
      }
      if(NV_DONE_S == ui_get_nv(NV_MF_PROXY_I, &nvItem))
      {
         for (i = 0; i < nSize; i++) //if null termination is  before nSize , it will still be fine.
         {
            pBBuff[i] = nvItem.mf_proxy[i];
         }
         return SUCCESS;
      }
      return EFAILED;
   }
   break;

   case  CFGI_MFLO_PROXY_LEN :
   {
      nv_item_type      nvi;
      int *             pLen = pBuff;
      int i =0;

      if( !pLen || nSize != sizeof(int) )
      {
         return EBADPARM;
      }
      if( ui_get_nv(NV_MF_PROXY_I, &nvi) != NV_DONE_S )
      {
         return EFAILED;
      }
      else
      {
         for (i = 0; i < MF_PROXY_MAX_LEN; i++) //instead of strlen for safety
         {
         if(nvi.mf_proxy[i]==0)
            break;
         }
      *pLen = i;
      return SUCCESS;
      }
   }

   break;
#endif

#ifdef FEATURE_ICARDSESSION
   case CFGI_MMS_CONFIG:
      case CFGI_MMS_ICP:      
      case CFGI_BROWSER_CP:
      case CFGI_JAVA_JDL:
      case CFGI_MMS_UP:      
         {
            int nResult = EFAILED;            
            AEECallback  *pcbReadData = NULL;            
            AEECardSessionCmdStatus *pRegisterCmdStatus = NULL;
            OEMProvisionInfo *pProvInfo = (OEMProvisionInfo*)pBuff;
            ICardSession *pICardSession = NULL;
            FileAttr *psFileAttr = NULL;
            IShell * pIShell = AEE_GetShell();
            if(NULL == pIShell || NULL == pProvInfo ||
               NULL == pProvInfo->pData || NULL == pProvInfo->pcbUserCallback){
               return EFAILED;
            }
                                    
            pcbReadData= (AEECallback *)MALLOC(
                                       sizeof(AEECallback));
            if(NULL == pcbReadData){               
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            psFileAttr= (FileAttr *)MALLOC(
                                       sizeof(FileAttr));
            if(NULL == psFileAttr){               
               FREEIF(pcbReadData);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            pRegisterCmdStatus= (AEECardSessionCmdStatus *)MALLOC(
                                       sizeof(AEECardSessionCmdStatus));
            if(NULL == pRegisterCmdStatus){               
               FREEIF(pcbReadData);
               FREEIF(psFileAttr);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            psFileAttr->nSize = nSize;
            pProvInfo->dwMemType = 0;
            psFileAttr->dwConfigItem = i;
            psFileAttr->pcbSVCCallback = (AEECallback*)pcbReadData; 
            CALLBACK_Init(pcbReadData, (PFNNOTIFY)OEM_SVCRegisterReadCb,
                          (void*)(psFileAttr));
            psFileAttr->pProvInfo = pProvInfo;
            psFileAttr->pRegisterStatus = pRegisterCmdStatus; 
            
            nResult = ISHELL_CreateInstance(pIShell, 
                                            AEECLSID_CARDSESSION, 
                                            (void **)(&pICardSession));
            if(SUCCESS != nResult){
               FREEIF(pcbReadData);
               FREEIF(psFileAttr);
               FREEIF(pRegisterCmdStatus);
               pProvInfo->nStatus = EFAILED;
               return EFAILED;
            } 
            psFileAttr->pICardSession = pICardSession;
            nResult = ICARDSESSION_Register(pICardSession,
                                            pRegisterCmdStatus,
                                            pcbReadData);
                                            
            if(SUCCESS != nResult){
               FREEIF(pcbReadData);
               FREEIF(psFileAttr);
               FREEIF(pRegisterCmdStatus);
               RELEASEIF(pICardSession);
            }
            
            pProvInfo->nStatus = nResult;
            return nResult;            
                        
         }
      
      case CFGI_MMS_CONFIG_LEN:
      case CFGI_MMS_ICP_LEN:      
      case CFGI_BROWSER_CP_LEN:
      case CFGI_JAVA_JDL_LEN: 
      case CFGI_MMS_UP_ATTRIBUTES:       
         {
            int nResult = EFAILED;
            ICardSession *pICardSession = NULL;
            IShell * pIShell = AEE_GetShell();
            AEECallback  *pcbGetAttr = NULL;
            OEMProvisionInfo *pProvInfo = (OEMProvisionInfo*)pBuff;            
            AEECardSessionCmdStatus *pRegisterCmdStatus = NULL;
            FileAttr *psFileAttr = NULL;
            
            if(NULL == pIShell || NULL == pProvInfo || 
               NULL == pProvInfo->pData || NULL == pProvInfo->pcbUserCallback){
               return EFAILED;
            }

            *(uint32*)pProvInfo->pData = 0;
            

            pcbGetAttr= (AEECallback *)MALLOC(
                                       sizeof(AEECallback));
            if(NULL == pcbGetAttr){
               pProvInfo->nStatus = EFAILED;               
               return ENOMEMORY;
            }
            psFileAttr= (FileAttr *)MALLOC(
                                       sizeof(FileAttr));
            if(NULL == psFileAttr){               
               FREEIF(pcbGetAttr);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            pRegisterCmdStatus= (AEECardSessionCmdStatus *)MALLOC(
                                       sizeof(AEECardSessionCmdStatus));
            if(NULL == pRegisterCmdStatus){               
               FREEIF(pcbGetAttr);
               FREEIF(psFileAttr);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            pProvInfo->dwMemType = 0;
            psFileAttr->pProvInfo = pProvInfo;
            psFileAttr->pRegisterStatus = pRegisterCmdStatus; 
            psFileAttr->bRead = TRUE;
            psFileAttr->pcbSVCCallback = (AEECallback*)pcbGetAttr;
            psFileAttr->dwConfigItem = i;            
            CALLBACK_Init(pcbGetAttr, (PFNNOTIFY)OEM_SVCRegisterLenCb,
                          (void*)(psFileAttr));            
                        
            nResult = ISHELL_CreateInstance(pIShell, 
                                            AEECLSID_CARDSESSION, 
                                            (void **)(&pICardSession));
            if(SUCCESS != nResult){               
               FREEIF(pcbGetAttr);
               FREEIF(psFileAttr);
               FREEIF(pRegisterCmdStatus);
               pProvInfo->nStatus = EFAILED;
               return EFAILED;
            } 
            psFileAttr->pICardSession = pICardSession;            
            nResult = ICARDSESSION_Register(pICardSession,
                                            pRegisterCmdStatus,
                                            pcbGetAttr);
                                            
            if(SUCCESS != nResult){
               FREEIF(pcbGetAttr);
               FREEIF(psFileAttr);
               FREEIF(pRegisterCmdStatus);
               RELEASEIF(pICardSession);
            }
            pProvInfo->nStatus = nResult;
            return nResult;                                                
         }
#endif //FEATURE_ICARDSESSION

      default:
         return(EUNSUPPORTED);
   }
   return(EBADPARM);
}

/*==================================================================
Function: OEM_SVCSetConfig

Description: Sets configuration information from the OEM

Prototype:
   int  OEM_SVCSetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_SVCSetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   IShell *pShell = AEE_GetShell();
   if(!pBuff)
      return(EBADPARM);

//#ifdef FEATURE_APP_SVCPRG
   /* Try to save it our cache */
   if (OEM_SetCachedConfig(i, pBuff, nSize) == 0)
     return 0;
//#endif //FEATURE_APP_SVCPRG

   switch(i) {
      case CFGI_USER_CLOCK_OFFSET:
      {
         IFileMgr* pFileMgr;
         PACONTEXT pacLast;
         int nErr;

         if(nSize!=sizeof(int32))
            return EBADPARM;

         pacLast = AEE_EnterAppContext(NULL);
         nErr = ISHELL_CreateInstance(pShell,
                                     AEECLSID_FILEMGR,
                                     (void**)&pFileMgr);
         if(SUCCESS == nErr)
         {
            IFile* pFile;
            /* Attempt to open for READWRITE first. */
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                       USER_CLOCK_OFFSET_FILE,
                                       _OFM_READWRITE);
            if(!pFile)
            {
               /* Maybe the file doesn't exist yet. Create it. */
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                    USER_CLOCK_OFFSET_FILE,
                                          _OFM_CREATE);
            }
            if(pFile)
            {
               if(IFILE_Write(pFile,
                                  pBuff,
                                  sizeof(int32)) == sizeof(int32))
               {
                  IFILE_Release(pFile);
                  IFILEMGR_Release(pFileMgr);
                  AEE_LeaveAppContext(pacLast);
                  return SUCCESS;
               }
               else
               {
                  /* Some problem while writing to the file. Destroy it. */
                     IFILE_Release(pFile);
                  IFILEMGR_Remove(pFileMgr, USER_CLOCK_OFFSET_FILE);
               }
            }
            IFILEMGR_Release(pFileMgr);
         }
         AEE_LeaveAppContext(pacLast);

         /* If we get here, there was a problem. */
         return nErr;
      }

#if defined(FEATURE_GSM1x)
      case CFGI_GSM1X_PRL:
         {
           AEEGSM1xPRLInfo *PRLInfoPtr = (AEEGSM1xPRLInfo *)pBuff;
           OEM_SetGSM1xPRL( PRLInfoPtr );
         }
         return SUCCESS;

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
      case CFGI_GSM1X_RTRE_CONFIG:
         {
           AEEGSM1xRTREConfig *RTREPtr = (AEEGSM1xRTREConfig *)pBuff;
           OEM_SetGSM1xRTREConfig( RTREPtr );
         }
         return SUCCESS;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      case CFGI_GSM1X_IDENTITY_PARAMS:
         {
           AEEGSM1xIdentityParams *IdentityParamsPtr = (AEEGSM1xIdentityParams *)pBuff;
           OEM_SetGSM1xIdentityParams( IdentityParamsPtr );
         }
         return SUCCESS;

      case CFGI_GSM1X_SID_NID_PARAMS:
         {
           AEEGSM1xSIDNIDParams *SIDNIDParamsPtr = (AEEGSM1xSIDNIDParams *)pBuff;
           OEM_SetGSM1xSIDNIDParams( SIDNIDParamsPtr );
         }
         return SUCCESS;
#endif /* FEATURE_GSM1x */
#if 0 
      case CFGI_DOWNLOAD:
      {
         IFileMgr* pFileMgr;
         if(nSize!=sizeof(AEEDownloadInfo))
            return EBADPARM;

         if(ISHELL_CreateInstance(pShell,
                                    AEECLSID_FILEMGR,
                                    (void**)&pFileMgr) == SUCCESS)
         {
            IFile* pFile;
            /* Attempt to open for READWRITE first. */
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                       DOWNLOAD_INFO_FILE,
                                       _OFM_READWRITE);
            if(!pFile)
            {
               /* Maybe the file doesn't exist yet. Create it. */
               pFile = IFILEMGR_OpenFile(pFileMgr,
                                          DOWNLOAD_INFO_FILE,
                                          _OFM_CREATE);
            }
            if(pFile)
            {
               if(IFILE_Write(pFile,
                              pBuff,
                              sizeof(AEEDownloadInfo)) == sizeof(AEEDownloadInfo))
               {
                  IFILE_Release(pFile);
                  IFILEMGR_Release(pFileMgr);
                  return SUCCESS;
               }
               else
               {
                  /* Some problem while writing to the file. Destroy it. */
                  IFILE_Release(pFile);
                  IFILEMGR_Remove(pFileMgr, DOWNLOAD_INFO_FILE);
               }
            }
            IFILEMGR_Release(pFileMgr);
         }
         /* If we get here, there was a problem. */
         return EFAILED;
      }
         break;
#endif
      case CFGI_SUBSCRIBERID:
         return SetSubscriberID((byte *)pBuff, nSize);

      case CFGI_LNG:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         return SetLngCode(*(uint32*)pBuff);

      case CFGI_SMS_GW_DOMAIN_PREF:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         return OEMSMS_SetDomainPref(*(int*)pBuff);

      case CFGI_LAST_VALID_TIME:
      {
         IFileMgr* pFileMgr;
         PACONTEXT pacLast;
         int nErr;

         if(nSize!=sizeof(LastValidCfg))
            return EBADPARM;

         pacLast = AEE_EnterAppContext(NULL);
         nErr = ISHELL_CreateInstance(pShell,
                                         AEECLSID_FILEMGR,
                                         (void**)&pFileMgr);
         if(SUCCESS == nErr)
         {
            IFile* pFile;
            /* Attempt to open for READWRITE first. */
            pFile = IFILEMGR_OpenFile(pFileMgr,
                                       LAST_VALID_TIMEINFO_FILE,
                                       _OFM_READWRITE);
            if(!pFile)
            {
               /* Maybe the file doesn't exist yet. Create it. */
               pFile = IFILEMGR_OpenFile(pFileMgr,
                                          LAST_VALID_TIMEINFO_FILE,
                                          _OFM_CREATE);
            }
            if(pFile)
            {
               if(IFILE_Write(pFile,
                              pBuff,
                              sizeof(LastValidCfg)) == sizeof(LastValidCfg))
               {
                  IFILE_Release(pFile);
                  IFILEMGR_Release(pFileMgr);
                  AEE_LeaveAppContext(pacLast);
                  return SUCCESS;
               }
               else
               {
                  /* Some problem while writing to the file. Destroy it. */
                  IFILE_Release(pFile);
                  IFILEMGR_Remove(pFileMgr, LAST_VALID_TIMEINFO_FILE);
               }
            }
            IFILEMGR_Release(pFileMgr);
         }
         /* If we get here, there was a problem. */
         AEE_LeaveAppContext(pacLast);

         return EFAILED;
      }
   case CFGI_SMS_MAX_PAYLOAD_LENGTH:
     {
       return SetPayloadLength((uint32*)pBuff);
     }
   case CFGI_SMS_MO_ON_ACCESS_CHANNEL:
     {
       return SetAccess((boolean*)pBuff);
     }
   case CFGI_SMS_MO_ON_TRAFFIC_CHANNEL:
     {
       return SetTraffic((boolean*)pBuff);
     }

   case CFGI_GPSONE_LOCK:
      {
         nv_item_type nvItem;
         if(nSize < sizeof(dword))
            return EBADPARM;

         nvItem.gps1_lock = *(dword*)pBuff;
         if( ui_put_nv(NV_GPS1_LOCK_I, &nvItem) != NV_DONE_S )
         {
            return EFAILED;
         }
         return SUCCESS;
      }
#ifdef FEATURE_ICARDSESSION
      case CFGI_MMS_CONFIG:
      case CFGI_MMS_ICP:      
      case CFGI_BROWSER_CP:
      case CFGI_JAVA_JDL:
      case CFGI_MMS_UP:      
         {
            int nResult = EFAILED;
            ICardSession *pICardSession = NULL;
            AEECardSessionCmdStatus *pCmdStatus = NULL;
            IShell * pIShell = AEE_GetShell();
            AEECallback  *pcbGetAttr = NULL;
            OEMProvisionInfo *pProvInfo = (OEMProvisionInfo*)pBuff;           
            FileAttr *psFileAttr = NULL;
            
            if(NULL == pIShell || NULL == pProvInfo ||
               NULL == pProvInfo->pData || NULL == pProvInfo->pcbUserCallback){

               return EFAILED;
            }
            
            pCmdStatus= (AEECardSessionCmdStatus *)MALLOC(
                                       sizeof(AEECardSessionCmdStatus));
            if(NULL == pCmdStatus){
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            pcbGetAttr= (AEECallback *)MALLOC(
                                       sizeof(AEECallback));
            if(NULL == pcbGetAttr){
               FREEIF(pCmdStatus);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            psFileAttr= (FileAttr *)MALLOC(
                                       sizeof(FileAttr));
            if(NULL == psFileAttr){
               FREEIF(pCmdStatus);
               FREEIF(pcbGetAttr);
               pProvInfo->nStatus = EFAILED;
               return ENOMEMORY;
            }
            pProvInfo->dwMemType = 0;
            psFileAttr->pProvInfo = pProvInfo;
            psFileAttr->nSize = nSize;
            psFileAttr->bRead = FALSE;
            psFileAttr->pRegisterStatus = pCmdStatus;                       
            psFileAttr->pcbSVCCallback = (AEECallback*)pcbGetAttr;
            psFileAttr->dwConfigItem = i;            
            CALLBACK_Init(pcbGetAttr, (PFNNOTIFY)OEM_SVCRegisterWriteCb,
                          (void*)(psFileAttr));                                   
            
            nResult = ISHELL_CreateInstance(pIShell, 
                                            AEECLSID_CARDSESSION, 
                                            (void **)(&pICardSession));
            if(SUCCESS != nResult){
               FREEIF(pCmdStatus);
               FREEIF(pcbGetAttr);
               FREEIF(psFileAttr);
               pProvInfo->nStatus = EFAILED;
               return EFAILED;
            } 
            psFileAttr->pICardSession = pICardSession;           
            nResult = ICARDSESSION_Register(pICardSession,
                                            pCmdStatus,
                                            pcbGetAttr);
            if(SUCCESS != nResult){
               FREEIF(pCmdStatus);
               FREEIF(pcbGetAttr);
               FREEIF(psFileAttr);
               RELEASEIF(pICardSession);
            }
            pProvInfo->nStatus = nResult;
            return nResult;                                                
         }
#endif //FEATURE_ICARDSESSION

      default:
         break;
   }

   return(EUNSUPPORTED);
}


boolean OEM_SVCIsVocCapable(voc_capability_type vocCap)
{
return voc_capability_supported(vocCap);

}

int OEM_GetBrewConfig(AEEConfigItem id, void *buffer, int len)
{
   char       *pDelimiter;
   ACONTEXT   *pacRestore;
   char       *pBuf  = NULL;
   char       *pRead = NULL;
   uint32      bytesLeft;
   uint32      bytesRead = 0, nRead;
   int         configLen;
   AEEFileInfo fileinfo;
   IFileMgr   *piFileMgr = NULL;
   IFile      *piFile = NULL;
   IShell     *piShell = AEE_GetShell();

   if ((buffer == NULL) || (len <= 0) || (piShell == NULL))
      return EFAILED;

   MEMSET(buffer, 0, len);

   pacRestore = AEE_EnterAppContext(NULL);
   ISHELL_CreateInstance(piShell, AEECLSID_FILEMGR, (void**)&piFileMgr);
   if (piFileMgr) { 
      if ((piFile = IFILEMGR_OpenFile(piFileMgr, BREW_CONFIG_FILE, _OFM_READ)) != NULL) { 
         IFILE_GetInfo(piFile, &fileinfo); 

         pRead = (char *)MALLOC(fileinfo.dwSize); 
         if (pRead != NULL) { 
            bytesLeft = fileinfo.dwSize; 
            while (bytesLeft > 0) { 
               nRead = IFILE_Read(piFile, (void *)(pRead + bytesRead), bytesLeft); 

               if (nRead == 0) { 
                  FREE(pRead);
                  IFILE_Release(piFile);
                  IFILEMGR_Release(piFileMgr);
                  AEE_LeaveAppContext(pacRestore);
                  return EFAILED;
               }

               bytesRead += nRead;
               bytesLeft -= nRead;
            }

            switch (id) {

               case CFGI_OEMAUTH_CHALLENGE_CAP:
                  if((pBuf = STRISTR(pRead, OEMAUTH_CHALLENGE_CAP)) != NULL) {
                     pBuf += OEMAUTH_CHALLENGE_CAP_LEN;
                     *((boolean*)buffer) = (boolean)ATOI(pBuf);
                     pBuf = NULL;
                  }
                  break;


               case CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN:
                  if((pBuf = STRISTR(pRead, OEM_AUTH_RESPLEN)) != NULL) {
                     pBuf += OEM_AUTH_RESPLEN_LEN;
                     *((uint32*)buffer) = (uint32)ATOI(pBuf);
                     pBuf = NULL;
                  }
                  break;


               case CFGI_OEMAUTH_RUIM_CARDID:
                  if((pBuf = STRISTR(pRead, OEMAUTH_RUIM_CARD_ID)) != NULL) {
                     pBuf += OEMAUTH_RUIM_CARD_ID_LEN;
                     *((uint32*)buffer) = (uint32)ATOI(pBuf);
                     pBuf = NULL;
                  }
                  break;


            }

            if (pBuf) {
               MEMSET(buffer, 0, len);
               pDelimiter = STRCHREND(pBuf, BREW_CONFIG_DELIMITER);
               pDelimiter--;  // To skip the line feed
               if ((pDelimiter - pBuf) > len) {
                  configLen = len;
               } else {
                  configLen = pDelimiter - pBuf;
               }
               MEMCPY(buffer, pBuf, configLen);
            }
         }
      }
   }

   if (pRead != NULL)
      FREE(pRead);
   if (piFile != NULL)
      IFILE_Release(piFile);
   if (piFileMgr != NULL)
      IFILEMGR_Release(piFileMgr);

   AEE_LeaveAppContext(pacRestore);
   return SUCCESS;
}

/*==================================================================
Function: OEM_SimpleBeep

Description:
   This function plays a standard OEM beep tone and vibration given
the duration, and returns TRUE if successful.

Prototype:
   boolean OEM_SimpleBeep(BeepType nBeepType, boolean bLoud);

Parameters:
   nBeepType: beep type which can be one of the following:
      - BEEP_OFF: stop playback of the current beep or vibration
      - BEEP_ALERT: alert beep tone
      - BEEP_REMINDER: reminder beep tone
      - BEEP_MSG: message beep tone
      - BEEP_ERROR: error beep tone
      - BEEP_VIBRATE_ALERT: alert vibration
      - BEEP_VIBRATE_REMIND: reminder vibration
   bLoud: boolean flag that sets the playback volume high or low

Return Value:
   TRUE if successfuly played or stopped the tone or vibration;
   FALSE otherwise/unsupported.

Comments:
   None

Side Effects:
   None

See Also:
   None
==================================================================*/
boolean OEM_SVCSimpleBeep(BeepType nBeepType, boolean bLoud)
{
   switch( nBeepType )
   {
   case BEEP_OFF:
      // Turn current BEEP sound off
#ifndef WIN32
      uisnd_snd_stop();
#endif
      return TRUE;
   case BEEP_ALERT:
      // Use Alert Beep sound
#ifndef WIN32
      uisnd_snd(UI_MSG_SND, SND_ALERT);
#endif
      return TRUE;
   case BEEP_REMINDER:
      // Use Reminder Beep sound
#ifndef WIN32
      uisnd_tone(UI_MSG_SND, SND_WARN, 2000);
#endif
      return TRUE;
   case BEEP_MSG:
      // Use New Message Beep Sound
#ifndef WIN32
      uisnd_tone(UI_MSG_SND, SND_MSG_WAITING, 2000);
#endif
	  return TRUE;
   case BEEP_ERROR:
      // Use Errpor Beep sound
#ifndef WIN32
      uisnd_tone(UI_MSG_SND, SND_ERR, 2000);
#endif
      return TRUE;
   case BEEP_VIBRATE_ALERT:
      // Use Alert Sound
#ifndef WIN32
      uisnd_vibrate(1000,NULL,NULL);
#endif
      return TRUE;
   case BEEP_VIBRATE_REMIND:
      // Use Vibrate Reminder
#ifndef WIN32
      uisnd_vibrate(1000,NULL,NULL);
#endif
      return TRUE;
   default:
      break;
   }
   return FALSE;
}


/*==============================================================
Function: OEM_SVCdisp_get_info

Description: Gets Display info. from DMSS

==============================================================*/
int  OEM_SVCdisp_get_info(disp_info_type *pInfo)
{
   disp_info_type dispInfo;

   if(!pInfo)
      return 1;
#ifndef WIN32
   dispInfo = disp_get_info();
#endif//WIN32  
   MEMCPY( pInfo, &dispInfo, sizeof(dispInfo));
   return 0;
}

#ifdef FEATURE_SECONDARY_DISPLAY
/*==============================================================
Function: OEM_SVCdisp_get_info2

Description: Gets Display info. for secondary display from DMSS

==============================================================*/
int  OEM_SVCdisp_get_info2(disp_info_type *pInfo)
{
   disp_info_type dispInfo;

   if(!pInfo)
      return 1;
#ifndef WIN32
   dispInfo = disp_get_info2();
#endif
//lint -save -e611 -e740   Suppress complaint about suspicious cast
   MEMCPY(pInfo, &dispInfo, sizeof(dispInfo));
//lint -restore
   return 0;
}
#endif // FEATURE_SECONDARY_DISPLAY


#if 0
/*==============================================================
Function: OEM_GetDefaultNetInfo


 Fill the dial, username and password strings for the default
 data network.

==============================================================*/
static void OEM_GetDefaultNetInfo(DataNetworkInfo * pi)
{
   STRCPY(pi->szDial,"#777");
}

/*==============================================================
Function: OEM_GetDataNetInfo


 Fill the dial, username and password strings for the default
 data network.

==============================================================*/
static void OEM_GetDataNetInfo(DataNetworkInfo * pi)
{
   STRCPY(pi->szDial,"#777");
}

/*==============================================================
Function: OEM_GetDownloadNetInfo


 Fill the dial, username and password strings for the default
 data network.

==============================================================*/
static void OEM_GetDownloadNetInfo(DataNetworkInfo * pi)
{
   STRCPY(pi->szDial,"#777");
}
#endif //0


/*===========================================================================
            Local Functions
==========================================================================*/
#if defined(T_MSM6250)
/*===========================================================================
FUNCTION SetImsiFromSim

DESCRIPTION
  Read Info from SIM card and set IMSI
===========================================================================*/
static void SetImsiFromSim (
  char* imsiText       // destination text
) {

  gsdi_returns_T              status;
  uim_items_enum_type         simFileNameImsi = 0;
  gsdi_card_mode_capability_T cardMode;
  int                         dummy;
  byte                        imsi[GSDI_IMSI_LEN];
  int                         i, j;

  if (gsdi_get_card_mode(&cardMode) != GSDI_SUCCESS) {
    ERR("SIM not initialized or invalid", 0, 0, 0);
    *imsiText = '\0';
    return;
  }
  if (cardMode == GSDI_CARD_GSM) {
    simFileNameImsi = UIM_GSM_IMSI;
  } else if (cardMode == GSDI_CARD_USIM) {
    simFileNameImsi = UIM_USIM_IMSI;
  } else {
    ERR("Invalid sim card", 0, 0, 0);
    *imsiText = '\0';
    return;
  }

  status = gsdi_sim_read(simFileNameImsi, GSDI_NO_INDEX, 0, NULL, 0,
                         (byte*)&imsi[0], GSDI_IMSI_LEN, 0, &dummy, 0, NULL);
  if (status != GSDI_SUCCESS) {
    ERR("sim read fails, status = %d", status, 0, 0);
    *imsiText = '\0';
    return;
  }

  j=0;
  *imsiText++ = ((imsi[1] & 0xF0) >> 4) + '0';
  for (i=2; i<imsi[0]; i++) {
    *imsiText++ = (imsi[i] & 0x0F) + '0';
    *imsiText++ = ((imsi[i] & 0xF0) >> 4) + '0';
  }

  if ((imsi[1] & 0x0F) == 0x9) { // even # digit
    *imsiText++ = (imsi[i] & 0x0F) + '0';
    *imsiText++ = ((imsi[i] & 0xF0) >> 4) + '0';
  } else {                       // odd # digit
    *imsiText++ = (imsi[i] & 0x0F) + '0';
  }

  // NULL terminate the string
  *imsiText = (char)0;
}
#endif //defined(T_MSM6250)


 void GetMobileInfo(AEEMobileInfo * pMobileInfo)
{
  nv_item_type nvi;  // buffer to read NV
  uint8 curr_nam;    // current NAM
  char* txt;         // destination text
  word temp;         // working buffer
  dword value;       // to store value read from nv
#ifdef CUST_EDITION	   
  if (pMobileInfo != NULL)
  {
      MEMSET(pMobileInfo, 0, sizeof(AEEMobileInfo));
  }
#endif /*CUST_EDITION*/
#if defined(FEATURE_GSM1x)
  pMobileInfo->dwESN = 0;
#else

#ifdef FEATURE_UIM_RUIM
   nvi.esn.esn = uim_get_esn_me();
#else
  ui_get_nv(NV_ESN_I, &nvi);
#endif /* FEATURE_UIM_RUIM */

  pMobileInfo->dwESN = nvi.esn.esn;
#endif /* defined(FEATURE_GSM1x) */

  ui_get_nv(NV_CURR_NAM_I, &nvi);
  pMobileInfo->nCurrNAM = nvi.curr_nam;
  curr_nam = nvi.curr_nam;

  // szMobileID's composed of the following
  // - mcc: mobile country code (3 digits)
  // - mnc: mobile network code (2 digits)
  // - min2: area code (3 digits)
  // - min1: phone number (7 digits)

  // set up destination text buffer
  txt = pMobileInfo->szMobileID;

#if defined(T_MSM6250)
// get IMSI from NV or SIM card?
   ui_get_nv(NV_RTRE_CONFIG_I, &nvi);
   if (nvi.rtre_config != NV_RTRE_CONFIG_NV_ONLY) {
     SetImsiFromSim(txt);
     return;
      }
#endif /* defined(T_MSM6250) */

  // read Mobile Contry Code
  nvi.imsi_mcc.nam = curr_nam;
  ui_get_nv( NV_IMSI_MCC_I, &nvi );
  value = nvi.imsi_mcc.imsi_mcc;
  *txt++ = mintable[(value/100) %10];
  value %= 100;
  *txt++ = mintable[value /10];
  *txt++ = mintable[value %10];

  // read Mobile Network Code
  nvi.imsi_11_12.nam = curr_nam;
  ui_get_nv( NV_IMSI_11_12_I, &nvi );
  value = nvi.imsi_11_12.imsi_11_12;
  *txt++ = mintable[(value/10) %10];
  *txt++ = mintable[value %10];


  // read MIN2
#ifdef CUST_EDITION	
  nvi.min2.nam = curr_nam;
#endif
  ui_get_nv(NV_MIN2_I, &nvi);
  value = nvi.min2.min2[1];
  *txt++ = mintable[ (value/100) %10];
  value %= 100;
  *txt++ = mintable[ value/10 ];
  *txt++ = mintable[ value%10 ];
  
  // read MIN1
#ifdef CUST_EDITION	
  nvi.min1.nam = curr_nam;
#endif
  ui_get_nv(NV_MIN1_I, &nvi);
  value = nvi.min1.min1[1];
  DBGPRINTF("******NV_MIN1_I=%d******", nvi.min1.min1[1]);   temp = (word) (value>>14 );
  *txt++ = mintable[ (temp/100) %10];
  temp %= 100;
  *txt++ = mintable[ temp/10 ];
  *txt++ = mintable[ temp%10 ];
  value &= 0x3FFFL;                /* get bottom 14 bits */
  /* next digit is top 4 bits */
  temp = (word) (( value >> 10 ) & 0xF );
  *txt++ = (char) ( ( ( temp == 10 ) ? 0 : temp ) + '0' );
  temp = (word) ( value & 0x3FF ); /* get bottom 10 bits */
  *txt++ = mintable[ (temp/100) %10];
  temp %= 100;
  *txt++ = mintable[ temp/10 ];
  *txt++ = mintable[ temp%10 ];

  // NULL terminate the string
  *txt = (char)0;
}  /* GetMobileInfo */

static int GetSoftwareVersion(byte *pVer, int *pnVerLen)
{
   uint32 strSize;

   if(!pnVerLen)
      return EFAILED;

   strSize = STRLEN(mob_sw_rev)+1;

   if(!pVer)
   {
      *pnVerLen = strSize;
      return SUCCESS;
   }

   MEMSET(pVer, '\0', *pnVerLen);
   MEMCPY(pVer, mob_sw_rev, (*pnVerLen - 1));

   return SUCCESS;
}



#if defined(FEATURE_GSM1x)
/*==================================================================
Function: OEM_GetGSM1xPRL

Description: Retrieves PRL from the specified NAM.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_GetGSM1xPRL
(
  AEEGSM1xPRLInfo *PRLInfoPtr
)
{
  nv_ram_roaming_list_type   *nvBufForPRL;
  nv_item_type               nvBufForPRLEnabled;

  byte                       *NAMPtr;


  /* Get the roaming list.*/
  nvBufForPRL = (nv_ram_roaming_list_type*) (PRLInfoPtr->pRoamingListStruct);
  (void) GSM1X_NV_READ_FUNC( NV_ROAMING_LIST_683_I, (nv_item_type*)nvBufForPRL );

  /* Get PRL enabled field.*/
  NAMPtr = (byte*) (PRLInfoPtr->pRoamingListStruct);

  nvBufForPRLEnabled.prl_enabled.nam = *NAMPtr;

  (void) GSM1X_NV_READ_FUNC( NV_PRL_ENABLED_I, &nvBufForPRLEnabled );

  PRLInfoPtr->enabled = nvBufForPRLEnabled.prl_enabled.enabled;

}

/*==================================================================
Function: OEM_SetGSM1xPRL

Description: Save the specified PRL to the specified NAM.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SetGSM1xPRL
(
  AEEGSM1xPRLInfo *PRLInfoPtr
)
{
  nv_ram_roaming_list_type   *nvBufForPRL;
  nv_item_type               nvBufForPRLEnabled;

  byte                       *NAMPtr;


  /* Set PRL. */
  nvBufForPRL = (nv_ram_roaming_list_type*) (PRLInfoPtr->pRoamingListStruct);

  (void) GSM1X_NV_WRITE_FUNC( NV_ROAMING_LIST_683_I, (nv_item_type*)nvBufForPRL );

  /* Set PRL enabled field.*/
  NAMPtr = (byte*) (PRLInfoPtr->pRoamingListStruct);

  nvBufForPRLEnabled.prl_enabled.nam     = *NAMPtr;
  nvBufForPRLEnabled.prl_enabled.enabled =  PRLInfoPtr->enabled;
  (void) GSM1X_NV_WRITE_FUNC( NV_PRL_ENABLED_I, &nvBufForPRLEnabled );
}

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)

/*==================================================================
Function: OEM_GetGSM1xRTREConfig

Description: Retrieves RTRE configuration from NV.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_GetGSM1xRTREConfig
(
  AEEGSM1xRTREConfig *pRTREConfig
)
{
  nv_item_type  nv_buf;

  (void) GSM1X_NV_READ_FUNC( NV_RTRE_CONFIG_I, &nv_buf );

  if(NULL != pRTREConfig){
     *pRTREConfig =  nv_buf.rtre_config;
  }
}

/*==================================================================
Function: OEM_SetGSM1xRTREConfig

Description: Update RTRE configuration in NV.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SetGSM1xRTREConfig
(
  AEEGSM1xRTREConfig *pRTREConfig
)
{
  nv_item_type  nv_buf;

  nv_buf.rtre_config = *pRTREConfig;

  (void) GSM1X_NV_WRITE_FUNC( NV_RTRE_CONFIG_I, &nv_buf );
}


#endif /* FEATURE_UIM_RUN_TIME_ENABLE */


/*==================================================================
Function: OEM_GetGSM1xIdentityParams

Description: Retrieves GSM1x identity - related parameters from
             the specified NAM in NV.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_GetGSM1xIdentityParams
(
  AEEGSM1xIdentityParams *pParams
)
{
  nv_item_type  nv_buf;

  /* CDMA IMSI */
  nv_buf.min1.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_MIN1_I, &nv_buf );
  DBGPRINTF("******NV_MIN1_I=%d******", nv_buf.min1.min1[1]);   pParams->imsi_s1[0] = nv_buf.min1.min1[0];
  pParams->imsi_s1[1] = nv_buf.min1.min1[1];

  nv_buf.min2.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_MIN2_I, &nv_buf );
  pParams->imsi_s2[0] = nv_buf.min2.min2[0];
  pParams->imsi_s2[1] = nv_buf.min2.min2[1];

  nv_buf.imsi_mcc.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_MCC_I, &nv_buf );
  pParams->mcc = nv_buf.imsi_mcc.imsi_mcc;

  nv_buf.imsi_11_12.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_11_12_I, &nv_buf );
  pParams->imsi_11_12 = nv_buf.imsi_11_12.imsi_11_12;

  nv_buf.imsi_addr_num.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_ADDR_NUM_I, &nv_buf );
  pParams->addr_num = nv_buf.imsi_addr_num.num;


  /* True IMSI */
  nv_buf.min1.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_T_S1_I, &nv_buf );
  pParams->t_imsi_s1[0] = nv_buf.imsi_t_s1.min1[0];
  pParams->t_imsi_s1[1] = nv_buf.imsi_t_s1.min1[1];

  nv_buf.min2.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_T_S2_I, &nv_buf );
  pParams->t_imsi_s2[0] = nv_buf.imsi_t_s2.min2[0];
  pParams->t_imsi_s2[1] = nv_buf.imsi_t_s2.min2[1];

  nv_buf.imsi_t_mcc.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_T_MCC_I, &nv_buf );
  pParams->t_mcc = nv_buf.imsi_t_mcc.imsi_mcc;

  nv_buf.imsi_t_11_12.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_T_11_12_I, &nv_buf );
  pParams->t_imsi_11_12 = nv_buf.imsi_t_11_12.imsi_11_12;

  nv_buf.imsi_t_addr_num.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_IMSI_T_ADDR_NUM_I, &nv_buf );
  pParams->addr_num = nv_buf.imsi_t_addr_num.num;

  /* ACCOLC */
  nv_buf.accolc.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_ACCOLC_I, &nv_buf );

  #if 1
     pParams->accolc[0] = nv_buf.accolc.ACCOLCpClass[0];
     pParams->accolc[1] = nv_buf.accolc.ACCOLCpClass[1];
  #else
     pParams->accolc[0] = nv_buf.accolc.class[0];
     pParams->accolc[1] = nv_buf.accolc.class[1];
  #endif //1

  /* MDN cellular*/
  nv_buf.dir_number.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_DIR_NUMBER_I, &nv_buf );
  memcpy((char*)pParams->msisdn, (char*)nv_buf.dir_number.dir_number, sizeof(pParams->msisdn) );

  /* MDN PCS */
   nv_buf.mob_dir_number.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_DIR_NUMBER_PCS_I, &nv_buf );
  pParams->num_digits = nv_buf.mob_dir_number.n_digits;
  memcpy((char*)pParams->digitn, (char*)nv_buf.mob_dir_number.digitn, sizeof(pParams->digitn) );

}



/*==================================================================
Function: OEM_SetGSM1xIdentityParams

Description: Retrieves GSM1x identity - related parameters from
             the specified NAM in NV.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SetGSM1xIdentityParams
(
  AEEGSM1xIdentityParams *pParams
)
{
  nv_item_type  nv_buf;

  /* CDMA IMSI */
  nv_buf.min1.nam = pParams->nam;
  nv_buf.min1.min1[0] = pParams->imsi_s1[0];
  nv_buf.min1.min1[1] = pParams->imsi_s1[1];
  (void) GSM1X_NV_WRITE_FUNC( NV_MIN1_I, &nv_buf );
  DBGPRINTF("******NV_MIN1_I=%d******", pParams->imsi_s1[1]); 
  nv_buf.min2.nam = pParams->nam;
  nv_buf.min2.min2[0] = pParams->imsi_s2[0];
  nv_buf.min2.min2[1] = pParams->imsi_s2[1];
  (void) GSM1X_NV_WRITE_FUNC( NV_MIN2_I, &nv_buf );

  nv_buf.imsi_mcc.nam = pParams->nam;
  nv_buf.imsi_mcc.imsi_mcc = pParams->mcc;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_MCC_I, &nv_buf );

  nv_buf.imsi_11_12.nam = pParams->nam;
  nv_buf.imsi_11_12.imsi_11_12 = pParams->imsi_11_12;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_11_12_I, &nv_buf );


  nv_buf.imsi_addr_num.nam = pParams->nam;
  nv_buf.imsi_addr_num.num = pParams->addr_num;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_ADDR_NUM_I, &nv_buf );



  /* True IMSI */
  nv_buf.min1.nam = pParams->nam;
  nv_buf.imsi_t_s1.min1[0] = pParams->t_imsi_s1[0];
  nv_buf.imsi_t_s1.min1[1] = pParams->t_imsi_s1[1];
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_T_S1_I, &nv_buf );

  nv_buf.min2.nam = pParams->nam;
  nv_buf.imsi_t_s2.min2[0] = pParams->t_imsi_s2[0];
  nv_buf.imsi_t_s2.min2[1] = pParams->t_imsi_s2[1];
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_T_S2_I, &nv_buf );

  nv_buf.imsi_t_mcc.nam = pParams->nam;
  nv_buf.imsi_t_mcc.imsi_mcc = pParams->t_mcc;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_T_MCC_I, &nv_buf );

  nv_buf.imsi_t_11_12.nam = pParams->nam;
  nv_buf.imsi_t_11_12.imsi_11_12 = pParams->t_imsi_11_12;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_T_11_12_I, &nv_buf );

  nv_buf.imsi_t_addr_num.nam = pParams->nam;
  nv_buf.imsi_t_addr_num.num = pParams->addr_num;
  (void) GSM1X_NV_WRITE_FUNC( NV_IMSI_T_ADDR_NUM_I, &nv_buf );

  /* ACCOLC */
  nv_buf.accolc.nam = pParams->nam;
  #if 1
     nv_buf.accolc.ACCOLCpClass[0] = pParams->accolc[0];
     nv_buf.accolc.ACCOLCpClass[1] = pParams->accolc[1];
  #else
     nv_buf.accolc.class[0] = pParams->accolc[0];
     nv_buf.accolc.class[1] = pParams->accolc[1];
  #endif //1
  (void) GSM1X_NV_WRITE_FUNC( NV_ACCOLC_I, &nv_buf );


  /* MDN cellular*/
  nv_buf.dir_number.nam = pParams->nam;
  memcpy((char*)nv_buf.dir_number.dir_number, (char*)pParams->msisdn, sizeof(nv_buf.dir_number.dir_number) );
  (void) GSM1X_NV_WRITE_FUNC( NV_DIR_NUMBER_I, &nv_buf );
    /* MDN PCS */
   nv_buf.mob_dir_number.nam = pParams->nam;
   nv_buf.mob_dir_number.n_digits = pParams->num_digits;
   memcpy((char*)nv_buf.mob_dir_number.digitn, (char*)pParams->digitn,  sizeof(pParams->digitn) );
   (void) GSM1X_NV_WRITE_FUNC( NV_DIR_NUMBER_PCS_I, &nv_buf );
}

/*==================================================================
Function: OEM_SetGSM1xSIDNIDParams
Description: Set the specified home and locked SID,NID pairs in the
             specified NAM.
Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_SetGSM1xSIDNIDParams
(
  AEEGSM1xSIDNIDParams *pParams
)
{
  word          pairIndex;
  nv_item_type  nv_buf;

  memset( (byte*)&nv_buf, 0, sizeof(nv_buf));
  nv_buf.home_sid_nid.nam = pParams->nam;
  for( pairIndex = 0; pairIndex < MIN(pParams->homePairCnt, NV_MAX_HOME_SID_NID); pairIndex++ ){
     nv_buf.home_sid_nid.pair[pairIndex].sid = pParams->pHomeSID[pairIndex];
     nv_buf.home_sid_nid.pair[pairIndex].nid = pParams->pHomeNID[pairIndex];
  }
  (void) GSM1X_NV_WRITE_FUNC( NV_HOME_SID_NID_I, &nv_buf );


  memset( (byte*)&nv_buf, 0, sizeof(nv_buf));
  nv_buf.sid_nid_lock.nam = pParams->nam;
  for( pairIndex = 0; pairIndex < MIN(pParams->lockedPairCnt, NV_MAX_SID_NID_LOCK); pairIndex++ ){
     nv_buf.sid_nid_lock.pair[pairIndex].sid = pParams->pLockedSID[pairIndex];
     nv_buf.sid_nid_lock.pair[pairIndex].nid = pParams->pLockedNID[pairIndex];
  }
  (void) GSM1X_NV_WRITE_FUNC( NV_SID_NID_LOCK_I, &nv_buf );
}


/*==================================================================
Function: OEM_GetGSM1xSIDNIDParams
Description: Retrieves the Home and Locked SID,NID pairs from the
            specified NAM.

Return Value:  N/A
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_GetGSM1xSIDNIDParams
(
  AEEGSM1xSIDNIDParams *pParams
)
{
  uint16          pairIndex;
  nv_item_type    nv_buf;
  uint16          homeCnt;
  uint16          lockCnt;

  nv_buf.home_sid_nid.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_HOME_SID_NID_I, &nv_buf );
  for( pairIndex = 0, homeCnt = 0; pairIndex < MIN(pParams->homePairCnt, NV_MAX_HOME_SID_NID); pairIndex++, homeCnt++ ){
     pParams->pHomeSID[pairIndex] = nv_buf.home_sid_nid.pair[pairIndex].sid;
     pParams->pHomeNID[pairIndex] = nv_buf.home_sid_nid.pair[pairIndex].nid;
  }

  pParams->homePairCnt = homeCnt;

  nv_buf.sid_nid_lock.nam = pParams->nam;
  (void) GSM1X_NV_READ_FUNC( NV_SID_NID_LOCK_I, &nv_buf );
  for( pairIndex = 0, lockCnt = 0; pairIndex < MIN(pParams->lockedPairCnt, NV_MAX_SID_NID_LOCK); pairIndex++, lockCnt++ ){
     pParams->pLockedSID[pairIndex] = nv_buf.sid_nid_lock.pair[pairIndex].sid;
     pParams->pLockedNID[pairIndex] = nv_buf.sid_nid_lock.pair[pairIndex].nid;
  }

  pParams->lockedPairCnt = lockCnt;
}

#endif /* FEATURE_GSM1x*/

int GetPayloadLength(uint32 *pInt)
{
  nv_item_type nvi;  // buffer to read NV
  uint32 retval;

  // pointer must be valid
  if(!pInt)
    return EBADPARM;

  retval = ui_get_nv(NV_SMS_MAX_PAYLOAD_LENGTH_I, &nvi);
  if(retval == NV_DONE_S)
    {
      *pInt = nvi.sms_max_payload_length;
      return SUCCESS;
    }
  else if(retval == NV_NOTACTIVE_S)
    {
      *pInt = OEMNV_SMS_MAX_MSG_PAYLOAD;
      return SetPayloadLength(pInt);
    }
  else
    {
      return EFAILED;
    }
}

int GetTraffic(boolean *pBool)
{
  nv_item_type nvi;  // buffer to read NV
  uint32 retval;

  // pointer must be valid
  if(!pBool)
    return EBADPARM;

  retval = ui_get_nv(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi);
  if(retval == NV_DONE_S)
    {
      *pBool = nvi.sms_mo_on_traffic_channel;
    }
  else if(retval == NV_NOTACTIVE_S)
    {
      *pBool = TRUE;
      return SetTraffic(pBool);
    }
  else
    {
      return EFAILED;
    }

  // If theres a bad value stored, default to true
  if(*pBool != 0 && *pBool != 1)
    {
      *pBool = TRUE;
      return SetTraffic(pBool);
    }

  return SUCCESS;
}

int GetAccess(boolean *pBool)
{
  nv_item_type nvi;  // buffer to read NV
  uint32 retval;

  // pointer must be valid
  if(!pBool)
    return EBADPARM;

  retval = ui_get_nv(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi);
  if(retval == NV_DONE_S)
    {
      *pBool = nvi.sms_mo_on_access_channel;
    }
  else if(retval == NV_NOTACTIVE_S)
    {
      *pBool = TRUE;
      return SetAccess(pBool);
    }
  else
    {
      return EFAILED;
    }

  // If theres a bad value stored, default to true
  if(*pBool != 0 && *pBool != 1)
    {
      *pBool = TRUE;
      return SetAccess(pBool);
    }

  return SUCCESS;
}

int SetTraffic(boolean *pBool)
{
  boolean data;
  nv_item_type nvi;  // buffer to read NV

  // pointer must be valid
  if(!pBool)
    return EBADPARM;

  // Boolean must be 0 or 1
  if(*pBool != 0 && *pBool != 1)
    return EBADPARM;

  // Enforce that both traffic and access cannot be off
  if(*pBool == FALSE)
    {
      if(NV_DONE_S != GetAccess(&data))
        return EFAILED;
      if(data == FALSE)
        return EBADPARM;
    }

  nvi.sms_mo_on_traffic_channel = *pBool;
  return ui_put_nv(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi);
}

int SetAccess(boolean *pBool)
{
  boolean data;
  nv_item_type nvi;  // buffer to read NV

  // Pointer must be valid
  if(!pBool)
    return EBADPARM;

  // Boolean must be 0 or 1
  if(*pBool != 0 && *pBool != 1)
    return EBADPARM;

  // Enforce that both traffic and access cannot be off
  if(*pBool == FALSE)
    {
      if(NV_DONE_S != GetTraffic(&data))
        return EFAILED;
      if(data == FALSE)
        return EBADPARM;
    }

  nvi.sms_mo_on_access_channel = *pBool;
  return ui_put_nv(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi);
}

int SetPayloadLength(uint32 *pInt)
{
  nv_item_type nvi;  // buffer to read NV

  // Pointer must be valid
  if(!pInt)
    return EBADPARM;

  nvi.sms_max_payload_length = *pInt;
  return ui_put_nv(NV_SMS_MAX_PAYLOAD_LENGTH_I, &nvi);
}

int SetSubscriberID(byte *pBuff, int nSize)
{
   nv_item_type nvi; //buffer to read NV
   int i;
   int numBytesToCopy;

   if (nSize >= NV_BREW_SID_SIZ)
   {
      numBytesToCopy = NV_BREW_SID_SIZ;
   }
   else
   {
      numBytesToCopy = nSize;
   }

   // Copy subscriber ID
   for (i = 0; i < numBytesToCopy; i++)
   {
      nvi.brew_subscriber_id[i] = pBuff[i];
   }

   // Pad the rest with the null char
   for (; i < NV_BREW_SID_SIZ; i++)
   {
      nvi.brew_subscriber_id[i] = '\0';
   }

   return ui_put_nv(NV_BREW_SUBSCRIBER_ID_I, &nvi);
}

#endif //defined(OEMAPPFUNCS)


#ifdef CUST_EDITION	 
// Macro to trim some letter.
#define TRIM_LEFT_CHAR(psz, ch)	do { while(*(psz) && *(psz)==(ch)) (psz)++; } while(0)

// inline to copy string within some size.
__inline int str_cpy_within_buf_size(char *pDest, int cbDest, char *pSrc)
{
	int cb = cbDest;

	while(--cb > 0)	{
		if('\0' == *pSrc)
			break;
		*pDest++ = *pSrc++;		
	}
	*pDest = 0;

	return (cbDest - (cb + 1));
}

// Function to read nv. Using OEMNV_Get can not read some ppp item as NV_PPP_PASSWORD_I. 
nv_stat_enum_type OEMNV_GetPPPItem
(
  nv_items_enum_type  item,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
#ifndef WIN32
  nv_cmd_type nvi;           /* nv command buffer */
  
  nvi.tcb_ptr = rex_self();          /* notify this task when done */
  nvi.sigs = UI_NV_SIG;
  nvi.done_q_ptr = NULL;             /* command goes on no queue when done */

  nvi.item = item;                 /* item to read */
  nvi.cmd = NV_READ_F;

  /* Set up NV so that it will read the data into the correct location */
  nvi.data_ptr = data_ptr;

  /* Call NV and wait for a response */
  (void)rex_clr_sigs(rex_self(), UI_NV_SIG);
#ifndef WIN32
  nv_cmd( &nvi );
#endif
  rex_wait( UI_NV_SIG );

  if( nvi.status != NV_DONE_S &&
      nvi.status != NV_NOTACTIVE_S )
  {
    ERR( "NV Read Failed Item %d Code %d", nvi.item, nvi.status, 0 );
  }  
  return( nvi.status );
#else
	return NV_STAT_ENUM_PAD;
#endif
} /* OEMNV_GetPPPItem() */

/*
** Set the PPP auth acount info to nv.
*/
int OEM_SetPppAccounts(PppAccounts *pAccount, DataSvcType dsType)
{
	nv_item_type nvi, nvi2;
	uint8 index;
	
	if(NULL == pAccount)
		return EBADPARM;
	
    switch(dsType)
    {
	case DS_BREW_TYPE:
	case DS_WAP20_TYPE:
	default:
		{
#ifdef FEATURE_DS_MULTIPLE_PROFILES // use CHAP but PAP,
			
			// Get  the PPP NAI ID to NV FOR ACTIVE PROFILE.   
			if(NV_DONE_S != OEMNV_GetPPPItem(NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nvi))   {
				DBGPRINTF( "NV read fails for active profile index");
				
				/* If the active profile index is not active, 	we should put it as 0 but not return EFAILED.*/
#ifndef WIN32
				nvi.ds_sip_active_profile_index = 0;

				if(OEMNV_Put(NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nvi) != NV_DONE_S)   {
					DBGPRINTF(";put NV_DS_SIP_ACTIVE_PROFILE_INDEX_I as 0 failed");
					return EFAILED;
				}
#endif
			}
#ifndef WIN32			
			index = nvi.ds_sip_active_profile_index;
		
			//nvi.ds_sip_nai_info.nai_length = (byte)STRLEN(pAccount->user_id_info);
			//STRCPY((char*)nvi.ds_sip_nai_info.nai, pAccount->user_id_info);
			nvi.ds_sip_nai_info.nai_length = str_cpy_within_buf_size((char*)nvi.ds_sip_nai_info.nai, 
															sizeof(nvi.ds_sip_nai_info.nai), 
															pAccount->user_id_info);
			
			//nvi.ds_sip_ppp_ss_info.ss_length = (byte)STRLEN(pAccount->passwd_info);
			//STRCPY((char*)nvi2.ds_sip_ppp_ss_info.ss, pAccount->passwd_info);
			nvi2.ds_sip_ppp_ss_info.ss_length = str_cpy_within_buf_size((char*)nvi2.ds_sip_ppp_ss_info.ss, 
															sizeof(nvi2.ds_sip_ppp_ss_info.ss),
															pAccount->passwd_info);
#endif				
#else // FEATURE_DS_MULTIPLE_PROFILES
				
			//nvi.pap_user_id.user_id_len = (byte)STRLEN(pAccount->user_id_info);
			//STRCPY((char*)nvi.pap_user_id.user_id, pAccount->user_id_info);
			nvi.pap_user_id.user_id_len = str_cpy_within_buf_size((char*)nvi.pap_user_id.user_id,
															sizeof(nvi.pap_user_id.user_id),
															pAccount->user_id_info);
			
			//nvi2.pap_password.password_len = (byte)STRLEN(pAccount->passwd_info);
			//STRCPY((char*)nvi2.pap_password.password, pAccount->passwd_info);          
			nvi2.pap_password.password_len = str_cpy_within_buf_size((char*)nvi2.pap_password.password,
															sizeof(nvi2.pap_password.password),
															pAccount->passwd_info);
			
#endif   // FEATURE_DS_MULTIPLE_PROFILES
		}
		break;	   
    } // switch( dsType) 
	
	
#ifdef FEATURE_DS_MULTIPLE_PROFILES
#ifndef WIN32	
	nvi.ds_sip_nai_info.index = index;   
	if(OEMNV_Put(NV_DS_SIP_NAI_INFO_I, &nvi) != NV_DONE_S)
		DBGPRINTF(";put NV_DS_SIP_NAI_INFO_I failed");
	
	nvi2.ds_sip_ppp_ss_info.index = index;   
	if( OEMNV_Put(NV_DS_SIP_PPP_SS_INFO_I, &nvi2) != NV_DONE_S) 
		DBGPRINTF(";put NV_DS_SIP_PPP_SS_INFO_I failed");
#endif	
#else // FEATURE_DS_MULTIPLE_PROFILES
	
	if(OEMNV_Put(NV_PPP_USER_ID_I, &nvi) != NV_DONE_S)
		DBGPRINTF(";put NV_PPP_USER_ID_I failed");
	
	if( OEMNV_Put(NV_PPP_PASSWORD_I, &nvi2) != NV_DONE_S) 
		DBGPRINTF(";put NV_PPP_PASSWORD_I failed");
	
#endif   // FEATURE_DS_MULTIPLE_PROFILES
	
	return SUCCESS;
} //OEM_SetPppAccounts

/*
** Get the PPP auth acount info from nv.
*/
int OEM_GetPppAccounts(PppAccounts *pAccount, DataSvcType dsType)
{
   char *pszUserId, *pszPassWord;
   nv_item_type  nvi, nvi2;
   uint8 index;
	//MSG_FATAL("OEM_GetPppAccounts......",0,0,0);
   if(NULL == pAccount)
      return EBADPARM;
    
#ifdef FEATURE_DS_MULTIPLE_PROFILES
   // Get  the PPP NAI ID to NV FOR ACTIVE PROFILE.   
   //MSG_FATAL("OEM_GetPppAccounts......0000000000000",0,0,0);
#ifndef WIN32
   if(NV_DONE_S != OEMNV_Get(NV_DS_SIP_ACTIVE_PROFILE_INDEX_I, &nvi ))   {
      DBGPRINTF( "NV read fails for active profile index");
      return EFAILED;
   }   
   //MSG_FATAL("OEM_GetPppAccounts...........NV_DS_SIP_NAI_INFO_I",0,0,0);
   index = nvi.ds_sip_active_profile_index;

   nvi.ds_sip_nai_info.index = index;  
   if(OEMNV_GetPPPItem(NV_DS_SIP_NAI_INFO_I, &nvi) != NV_DONE_S)
      DBGPRINTF(";get NV_DS_SIP_NAI_INFO_I failed");
   
   nvi2.ds_sip_ppp_ss_info.index = index;  
   if( OEMNV_GetPPPItem(NV_DS_SIP_PPP_SS_INFO_I, &nvi2) != NV_DONE_S) 
      DBGPRINTF(";get NV_DS_SIP_PPP_SS_INFO_I failed");  
#endif   
#else // FEATURE_DS_MULTIPLE_PROFILES
  // MSG_FATAL("OEM_GetPppAccounts...........NV_PPP_USER_ID_I",0,0,0);
   if(OEMNV_GetPPPItem(NV_PPP_USER_ID_I, &nvi) != NV_DONE_S)
      DBGPRINTF(";get NV_PPP_USER_ID_I failed");
   
   if(OEMNV_GetPPPItem(NV_PPP_PASSWORD_I, &nvi2) != NV_DONE_S) 
      DBGPRINTF(";get NV_PPP_PASSWORD_I failed");   
#endif // FEATURE_DS_MULTIPLE_PROFILES

	//MSG_FATAL("OEM_GetPppAccounts......11111111111",0,0,0);
		
    switch(dsType)
    {
        case DS_BREW_TYPE:
        case DS_WAP20_TYPE:
        default:
#ifdef FEATURE_DS_MULTIPLE_PROFILES
#ifndef WIN32
            pszUserId = (char *)nvi.ds_sip_nai_info.nai;
            TRIM_LEFT_CHAR(pszUserId, '0');

            pszPassWord = (char *)nvi2.ds_sip_ppp_ss_info.ss;
            TRIM_LEFT_CHAR(pszPassWord, '0');
#endif
#else // FEATURE_DS_SIP_MULTIPLE_PROFILE
#ifndef WIN32
            pszUserId = (char *)nvi.pap_user_id.user_id;
            TRIM_LEFT_CHAR(pszUserId, '0');

            pszPassWord = (char *)nvi2.pap_password.password;
            TRIM_LEFT_CHAR(pszPassWord, '0');
#endif
#endif // FEATURE_DS_MULTIPLE_PROFILES

            str_cpy_within_buf_size(pAccount->user_id_info, PPP_MAX_USER_ID_LEN, pszUserId);
            str_cpy_within_buf_size(pAccount->passwd_info, PPP_MAX_PASSWD_LEN, pszPassWord);		
    } // switch( dsType)

    return SUCCESS;
} //OEM_GetPppAccounts


// 解决慢卡插入后，启动 Applet 慢的问题
/*==============================================================================
函数:
    OEMSVC_UpdateRUIMNVCacheReq

说明:
    函数通过变更 RUIM_NV_Cache 结构中标记�?�使用到 RUIM_NV_Cache 的函数根据这个标
    记确定是否读�? RUIM �?

参数:
    nItem [in]: NV 项�??

返回�?:
    none

备注:
        
==============================================================================*/
void OEMSVC_UpdateRUIMNVCacheReq(nv_items_enum_type nItem)
{
    switch (nItem)
    {
        case CFGI_CARDID:
        case CFGI_CARDID_LEN:
            gUIMNVCache.isCARDIDValid = 0;
            if (gUIMNVCache.CardIDbuf != NULL)
            {
                sys_free(gUIMNVCache.CardIDbuf);
                gUIMNVCache.CardIDbuf = NULL;
            }
            gUIMNVCache.nCardIDLen = 0;
            break;
            
        case NV_CURR_NAM_I:
            gUIMNVCache.isDirNumValid = 0;
            gUIMNVCache.isCurNamValid = 0;
            break;
            
        case NV_DIR_NUMBER_PCS_I:
            gUIMNVCache.isDirNumValid = 0;
            break;
            
        default:
            break;
    }
}

void SetDeviceState(DeviceType dt,DeviceStateType dst)
{   
    switch(dt)
    {
        case DEVICE_TYPE_MP4:
             if(dst==DEVICE_MP4_STATE_ON)
             {
                 DeviceMp4State=DEVICE_MP4_STATE_ON;
             }
             else if(dst==DEVICE_MP4_STATE_OFF)
             {   DeviceMp4State=DEVICE_MP4_STATE_OFF;
                
             }
             break;

        case DEVICE_TYPE_CAMERA:
            if(dst==DEVICE_CAMERA_STATE_ON)
            {
                DeviceCameraState=DEVICE_CAMERA_STATE_ON;
            }
            else if(dst==DEVICE_CAMERA_STATE_OFF)
            {
                DeviceCameraState=DEVICE_CAMERA_STATE_OFF;
            }

            break;

        default:
            break;
    }
}

int GetDeviceState(DeviceType dt)
{   
    int state = 0;
    
    switch(dt)
    {
        case DEVICE_TYPE_MP4:
            state = DeviceMp4State;
            break;

        case DEVICE_TYPE_CAMERA:
            state = DeviceCameraState;
            break;

        default:
            break;
    }

    return  state;
}

#endif /*CUST_EDITION*/
#ifdef FEATURE_NET_LOCK
boolean OEM_IsNetLock(void)
{		
//add by wenyu ,����V3137 ��ȡSPN���Ƿ���"mts"���ж�
	#ifdef FEATURE_VERSION_V3C137
	extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
	int i;
	for(i=0;i<STRLEN(charsvc_p_name);i++)
		{
			DBGPRINTF("charsvc_p_name=%0x",charsvc_p_name[i]);
	}
	if(STRISTR(charsvc_p_name,"mts"))
	{	
		return FALSE;
	}	
	return TRUE;
	#else
	nv_item_type nvi;  // buffer to read NV
    uint8 curr_nam;    // current NAM
    dword mcc;       // to store value read from nv
    dword mnc;
    dword sid;
    nv_item_type nvilock;  // buffer to read NV
    int i;
    word original_mcc,original_mnc,final_mcc,final_mnc;
  //  MSG_FATAL("OEM_IsNetLock",0,0,0);

    if (SUCCESS != OEM_ReadESN(&nvi.esn.esn))
    {
        return FALSE;
    }
    
    if(nvi.esn.esn == 0)
    {
        return FALSE;
    }
  //  MSG_FATAL("nvilock.enabled_net_lock.b_lock = %d",nvilock.enabled_net_lock.b_lock,0,0);
    if(NV_DONE_S != ui_get_nv(NV_NET_LOCK_I, &nvilock))
    {
        return FALSE;
    }
//    MSG_FATAL("nvilock.enabled_net_lock.b_lock = %d",nvilock.enabled_net_lock.b_lock,0,0);
    if(!nvilock.enabled_net_lock.b_lock)
    {
        return FALSE;
    }
    
    ui_get_nv(NV_CURR_NAM_I, &nvi);
    curr_nam = nvi.curr_nam;

    nvi.imsi_mcc.nam = curr_nam;
    ui_get_nv( NV_IMSI_MCC_I, &nvi );
    original_mcc = nvi.imsi_mcc.imsi_mcc;
    
    // read Mobile Network Code
    nvi.imsi_11_12.nam = curr_nam;
    ui_get_nv( NV_IMSI_11_12_I, &nvi );
    original_mnc = nvi.imsi_11_12.imsi_11_12;
    
    nvi.sid_nid.nam = curr_nam;
    ui_get_nv( NV_SID_NID_I, &nvi );
    sid = nvi.sid_nid.pair[NV_CDMA_MIN_INDEX][0].sid;

    MSG_FATAL("original_mcc = %d original_mnc=%d",original_mcc,original_mnc,0);	
    final_mcc = original_mcc+ 111;
    final_mnc = original_mnc + 11; 

    //ERR("BS mcc(%d), BS mnc(%d)",cur_bs_ptr->esp.mcc,cur_bs_ptr->esp.imsi_11_12,0);

    if ( final_mcc % 10 == 0 )
    {
       final_mcc -= 10;
    }
    if ( (final_mcc/10) % 10 == 0 )
    {
       final_mcc -= 100;
    }
    if ( final_mcc >= 1000 )
    {
       final_mcc -= 1000; 
    } 

    if ( final_mnc % 10 == 0 )
    {
       final_mnc -= 10;
    }
    if ( final_mnc >= 100 )
    {
       final_mnc -= 100;
    } 
    
    MSG_FATAL("final_mcc = %d final_mnc=%d sid=%d",final_mcc,final_mnc,sid);	  

    for(i=0;i<nvilock.enabled_net_lock.b_lock;i++)
    {    
        MSG_FATAL("phone mcc = %d mnc=%d sid=%d",nvilock.enabled_net_lock.mcc[i],nvilock.enabled_net_lock.mnc[i],nvilock.enabled_net_lock.sid[i]);	
        if( (final_mcc == nvilock.enabled_net_lock.mcc[i] || nvilock.enabled_net_lock.mcc[i] == 65535)
          &&(final_mnc == nvilock.enabled_net_lock.mnc[i] || nvilock.enabled_net_lock.mnc[i] == 65535)
          &&(sid ==0 || sid == nvilock.enabled_net_lock.sid[i] || nvilock.enabled_net_lock.sid[i] == 65535)
          )
        {
        
            MSG_FATAL("return false",0,0,0); 
            return FALSE;
        }
    }
    return TRUE;
	#endif
}
#endif

