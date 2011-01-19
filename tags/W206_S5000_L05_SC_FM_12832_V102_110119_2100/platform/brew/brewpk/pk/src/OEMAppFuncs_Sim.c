/*===========================================================================

  FILE: OEMAppFuncs_Sim.c
  
  SERVICES: Windows implementation of the OEM Functions
   
  DESCRIPTION
     This file provides the windows implementation of some of the functions
     in OEMAppFuncs.c. These functions are also used by the BREW Simulator. 
     This file is shipped with the Porting Kit. OEMs will be able to modify these 
     functions, re-build the BREW.DLL and test them using the BREW Simulator.
      
  PUBLIC CLASSES:  
        N/A
        
  INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
         
           
  Copyright (c) 2000 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

#include <windows.h>
#include <tchar.h>
#include "sim_dns.h"
#include "OEMSafeMode.h"
#include "OEMConfig.h"
#include "OEMFS.h"
#include "OEMSecSysClock.h"
#include "OEMVersion.h"
#include "AEE_OEM.h"
#include "AEE.h"
#include "AEEDownload.h"
#include "AEEStdLib.h"
#include "AEEComdef.h"
#include "AEEClassIDs.h"
#include "AEEFile.h"
#include "AEESMS.h"
#include "AEENetworkTypes.h"
#include "AEEBacklight.h"
#include "AEEMutualAuth.h"  // for MA CLSID - for MA CAPs Get & Set
#include "AEECLSID_SIM_CONFIGEXTENDER.BID"

#ifdef FEATURE_COMMERCIAL_SIMULATOR
void AEESIM_GetDownloadInfoFromConfig(char* pszServer, uint32 *dwCarrierID, uint32 *dwPlatformID);
#endif

#define AEECLSID_OEM_DL_CONFIG     (AEECLSID_SAMPLE_APP + 23)

#define AEECLSID_LAST_VALID_CFG    AEECLSID_OEM_APP
#define AEECLSID_USR_CLK_OFF       (AEECLSID_OEM_APP + 1)

#define OEM_LAST_VALID_VER   (1)
#define OEM_USR_CLK_OFF_VER  (1)

#define SIM_CHIP_ID  "Simulator"

#define SIM_SUBSCRIBER_ID "1234567890"

#define OEM_MAX_ESN_STRING_SIZE  20    // ESN string in Hex or Decimal, or "Not supported"

#define OEM_MAX_IMEI_STRING_SIZE 16    // IMEI string (decimal value)

#define OEM_MAX_MEID_STRING_SIZE 18    // MEID string (hex value)

#define OEM_DL_CFG_VER  (10)

#define OEM_MA_CAPAB_VER  (1)

static void GetDownloadFSAvail(DLItemSize * pi);
static void GetDownloadFSAvailEx(AEEFSFreeSpace * pfs);
static void GetMobileInfo(AEEMobileInfo * pmi);

extern boolean TAPI_CompressNumber(char * pszSrc);

extern int GetDisplayInfo(AVSType avs, AEEBitmapInfo *pInfo, int nSize);

extern void GetDownloadHeader(char* pBuff, int* pnSize);

extern boolean AEESIM_GetMobileInfo(char* pszESN, int nESNSize);
extern boolean AEESIM_GetIMEI(char *pszIMEI, int nIMEISize);
extern boolean AEESIM_GetMEID(char *pszMEID, int nMEIDSize);
extern boolean AEESIM_GetSubscriberID(char* pBuff);
extern void   AEESIM_GetDownloadConfig(char* pszServer, uint32 *dwCarrierID, uint32 *dwPlatformID);
extern void   AEESIM_GetFSLimit(MIFFSLimit * pfsLimit);
extern uint8  AEESIM_GetSoftkeyCount();
extern uint32 AEESIM_GetDeviceID(void);
extern int    AEESIM_GetDisplayInfo(AVSType avs, AEEBitmapInfo *pInfo, int *pnSize);
extern uint32 AEESIM_GetHeapSize(void);
extern uint16 AEESIM_GetEnc(void);
extern const RGBVAL *AEESIM_GetSysColors(AVSType avsCode);
extern uint32 AEESIM_GetLang(void);
extern void   AEESIM_GetKeySupported(KeySupportType *keySupport);
extern boolean AEESIM_GetAutoStartApp(AEECLSID *pcls);
extern boolean AEESIM_GetAutoStartArgs(char *pBuff, int nSize);
extern BOOL AEESIM_GetEFSMax(uint64 * pqwMaxSpace, uint32 * pdwMaxFiles);
extern boolean AEESIM_IsGSMNetwork(void);
extern boolean AEESIM_IsMIDISupported();

extern boolean OEMSound_IsVibratorON(void);

extern boolean AEESIM_GetFlipClose(BOOL * pbFlipClose);

//Class ID of Idle App.Launcher will attempt to start this app as an idle app
#define AEECLSID_IDLEAPP_BID     0x01010ec1

static gbCFGI_AUTOSTART = TRUE;
static AECHAR * gpCFGI_AUTOSTARTARGS = NULL;

static AEEGPSOneLockInfo gCFGI_GPSONE_LOCK = GPSONE_LOCK_NONE;

extern RGBVAL gSysRGBs[];

static int GetFirmwareVersion(byte *pVer, int *pnVerLen);

#define BREW_CONFIG_FILE        "fs:/sys/brewconfig.dat"

#define BREW_THEMENAME          "ThemeName="
#define BREW_THEMENAME_LEN      10
#define BREW_SUBLCDPROPS        "SubLcdProps="
#define BREW_SUBLCDPROPS_LEN    12
#define BREW_ANNUNPROPS         "AnnunProps="
#define BREW_ANNUNPROPS_LEN     11
#define BREW_AUTOSTARTAPP       "AutoStartApp="
#define BREW_AUTOSTARTAPP_LEN   13
#define BREW_CONFIG_DELIMITER   '\n'

#define BREW_FCACHEDEF          "CacheDef="
#define BREW_FCACHEDEF_LEN      9
#define BREW_FCACHEMIN          "CacheMin="
#define BREW_FCACHEMIN_LEN      9
#define BREW_FCACHEMAX          "CacheMax="
#define BREW_FCACHEMAX_LEN      9

#define BREW_THEMES_INI         "ThemesIni="
#define BREW_THEMES_INI_LEN     10
#define BREW_WALLPAPERS_INI     "WallpapersIni="
#define BREW_WALLPAPERS_INI_LEN 14
#define BREW_RINGTONES_INI      "RingtonesIni="
#define BREW_RINGTONES_INI_LEN  13

#define OEM_AUTH_CR_CAP          "OEMAuthCap="
#define OEM_AUTH_CR_CAP_LEN      11
#define OEM_AUTH_RESPLEN         "OEMAuthRespMaxLen="
#define OEM_AUTH_RESPLEN_LEN     18
#define OEM_AUTH_RUIM_CARDID	   "OEMAuthCardId="
#define OEM_AUTH_RUIM_CARDID_LEN	14

#define CFGI_OEMAUTH_RUIM_CARDID  (CFGI_MAX + 121)

static int  OEM_GetBrewConfig(AEEConfigItem id, void *buffer, int len);
static int  OEM_SetBrewConfig(AEEConfigItem id, void *pBuffer, int len);

#ifdef CFGI_RESERVED_1
#undef CFGI_RESERVED_1
#endif

#ifdef CFGI_DISABLE_MA 
#undef CFGI_DISABLE_MA 
#endif

#define   CFGI_RESERVED_1  72 
#define   CFGI_DISABLE_MA  CFGI_RESERVED_1                    


uint32 gdwDisableMA = 0;  // Leave MA ON, by default

static IConfig *spIConfigExtender = NULL;
static boolean sbConfigExtenderCI = FALSE;
static void CustomOEMConfig_ReleaseConfigExtender(void *unused) 
{
   sbConfigExtenderCI = FALSE;
   if (spIConfigExtender != NULL) {
      (void) ICONFIG_Release(spIConfigExtender);
      spIConfigExtender = NULL;
   }
}

/*=============================================================================
FUNCTION: CustomOEMConfig_GetConfigExtender

DESCRIPTION:  Returns the IConfig extenter interface 

PARAMETERS:
   None

RETURN VALUE:
   IConfig *:  This pointer does not contain a reference.  No release required

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static IConfig *CustomOEMConfig_GetConfigExtender(void) 
{
   if (FALSE == sbConfigExtenderCI && TRUE == AEE_IsInitialized()) { 
      IShell *pIShell = AEE_GetShell();

      if (pIShell != NULL) {
         ACONTEXT *pCtxOld;
         static boolean bReentered = FALSE;

         // Create the extender in the system context to
         // avoid app-switching issues
         pCtxOld = AEE_EnterAppContext(NULL);

         // Protect against reentering this function, this can be caused by 
         // ISHELL_CreateInstance since this function may call OEM_GetDeviceInfo.
         if (!bReentered) {
            static AEECallback sBREWShutDownCallback;
            bReentered = TRUE;

            (void)ISHELL_CreateInstance(pIShell, AEECLSID_SIM_CONFIGEXTENDER, 
                                       (void **) &spIConfigExtender);
            sbConfigExtenderCI = TRUE;
            CALLBACK_Init(&sBREWShutDownCallback, CustomOEMConfig_ReleaseConfigExtender, NULL); 
            AEE_RegisterSystemCallback(&sBREWShutDownCallback, AEE_SCB_AEE_EXIT, 0);

            bReentered = FALSE;
         }

         // Restore the original app context.
         AEE_LeaveAppContext(pCtxOld);
      }
   }
   return spIConfigExtender; // No addref
}
    
static int OEM_GetPrefs(AEECLSID cls, uint16 wVer, void * pCfg, uint16 nSize)
{
   IShell * pIShell = AEE_GetShell();
   return ISHELL_GetPrefs(pIShell, cls, wVer, pCfg, nSize);
}

static int OEM_SetPrefs(AEECLSID cls, uint16 wVer, void * pCfg, uint16 nSize)
{
   IShell * pIShell = AEE_GetShell();
   return ISHELL_SetPrefs(pIShell, cls, wVer, pCfg, nSize);
}

int OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   int nErr = EUNSUPPORTED;

   /* First try to delegate the IConfig item to the extender */
   {
      IConfig *pIConfigExtender = CustomOEMConfig_GetConfigExtender();
      if (pIConfigExtender != NULL) {
         int nErr = ICONFIG_GetItem(pIConfigExtender, i, pBuff, nSize);
         pIConfigExtender = NULL;  // No release required, spIConfigExtender is released on BREW shutdown
         if (EUNSUPPORTED != nErr) {
            return nErr;
         }
      }
   }

   //did not handle GetConfig via pIConfigExtender, hence proceed to the usual handling
   switch(i){
#if   defined(_AEEWIN)
   // The real CE device definitely exhibits a totally system performance than the PC
   // need to twiddle these numbers a bit.
      // In fact, the setting below should be appliable to Simulator as well, but in case, we
      // limit the effect
   case CFGI_MAX_DISPATCH_TIME:
       // this is the default the system would set to if this request was not handled
      *(uint32 *)pBuff = 0xffffffff;
      return 0;
   case CFGI_MIN_IDLE_TIME:
       // this is the default the system would set to if this request was not handled
      *(uint32 *)pBuff = 1;
      return 0;
#endif      
#if 0
   // Note: Added this code to better emulate the size limits on downloading apps.  

      case CFGI_CACHED_RESOURCES:
         {
         OEMResCacheInfo   *  pri = (OEMResCacheInfo *)pBuff;
         uint32               dwTotal;

         if(pri){
            GETRAMFREE(&dwTotal,NULL);
            if(dwTotal < (250 * 1024)){
               pri->nCachedFiles = 12;
               break;
            }
            pri->nCachedFiles = 24;
            return(0);
         }
         break;
         }
#endif
      
      case CFGI_DISABLE_MA:
      {
         if( nSize != sizeof( uint32 ) ) {
            return EBADPARM;
         }
   
         (*(uint32*)pBuff) = gdwDisableMA; 
         return(SUCCESS);
      }

      case CFGI_BREWMA_CAPABILITIES:

         if( nSize != sizeof( uint32 ) ) {
            return EBADPARM;
         }

         if(SUCCESS != OEM_GetPrefs(AEECLSID_IMUTUALAUTH, OEM_MA_CAPAB_VER, pBuff, (uint16)nSize)) {
            // Bit 0 (LSB) is set dynamically on the handset by getting the serving
            // system info from ITelephone - but for the simulator we allow setting 
            // via Config Item. This is purely to allow sensible testing, since otherwise
            // the ITelephone interface would always return NO SERVICE.
            // 0x00000001	MA_CLIENT_CAP_MODE_NO_TIME_SYNC Dynamic - from ITelephone

            // Bit 1 & 2 are ALWAYS hard coded by BREW, and ALWAYS set ON.
            
            // 0x00000002	MA_CLIENT_CAP_SUPPORTS_NN	      Hard coded ON by BREW  
            // 0x00000004	MA_CLIENT_CAP_SUPPORTS_TT	      Hard coded ON by BREW	  
            //
            // For Simulator, we assume a battery backed RTC - all other caps are off.
            // 0x00000008	MA_CLIENT_CAP_BATTERY_BACKED_RTC	OEM Config Item	  

            (*(uint32*)pBuff) = (MA_CLIENT_CAP_BATTERY_BACKED_RTC|!MA_CLIENT_CAP_MODE_NO_TIME_SYNC); 
         }
         return(SUCCESS);

      case CFGI_LAST_VALID_TIME:
      {
         JulianType Date;
         uint64 secs=0;

         if(nSize != sizeof(LastValidCfg)) {
            return(EBADPARM);
         }
        
         if(SUCCESS != OEM_GetPrefs(AEECLSID_LAST_VALID_CFG, OEM_LAST_VALID_VER, pBuff, (uint16)nSize)) {
             // Default time in micro-seconds is set to Apr 14th, 2005 - 09:30:30
            Date.wYear    = 2005;
            Date.wMonth   = 4;
            Date.wDay     = 14; 
            Date.wHour    = 9;
            Date.wMinute  = 30;
            Date.wSecond  = 30;
            Date.wWeekDay = 3;
            secs = (uint64) JULIANTOSECONDS(&Date);
	 
            (*(LastValidCfg *)pBuff).qwTime = secs * 1000 * 1000; // in microseconds 
            (*(LastValidCfg *)pBuff).OwnerCls = AEE_GetAppContextCls(AEE_GetAppContext());
         }
         return SUCCESS;	   
      }

      case CFGI_USER_CLOCK_OFFSET:
      {
         if(nSize != sizeof(int32)) {
            return(EBADPARM);
         }

         if(SUCCESS != OEM_GetPrefs(AEECLSID_USR_CLK_OFF, OEM_USR_CLK_OFF_VER, pBuff, (uint16)nSize)) {
            (*(int32 *)pBuff) = 0;  
         }
         return SUCCESS;
      }

      case CFGI_FILE_CACHE_INFO:
         {
         OEMFileCacheInfo *   pi = (OEMFileCacheInfo *)pBuff;
         uint32               dwTotal;

         if(pi){
            GETRAMFREE(&dwTotal,NULL);
            if(dwTotal < (250 * 1024))
               break;
            if(dwTotal < (500 * 1024))
               pi->nCacheMax = 100 * 1024;
            else
               pi->nCacheMax = 150 * 1024;
            return(0);
         }
         break;
         }

#if defined(AEE_SIMULATOR)
      case CFGI_DOWNLOAD_FS_INFO:
         {
         DLItemSize * pi = (DLItemSize *)pBuff;

         if(!pi || nSize < sizeof(DLItemSize))
            return(EBADPARM);

         GetDownloadFSAvail(pi);
         return(0);
         }
      
      case CFGI_DOWNLOAD_FS_INFOEX:
         {
            AEEFSFreeSpace * pfs = (AEEFSFreeSpace *)pBuff;

            if(!pfs || nSize < sizeof(AEEFSFreeSpace)) {
               return EBADPARM;
            }
            GetDownloadFSAvailEx(pfs);
            return SUCCESS;
         }

      case CFGI_AUTOSTARTARGS:
         {
            if (gpCFGI_AUTOSTARTARGS)
            {
               char *pArgs = (char *)pBuff;
               WSTRTOSTR(gpCFGI_AUTOSTARTARGS, pArgs, nSize);            
            }
            else
               return !AEESIM_GetAutoStartArgs(pBuff, nSize);
               
            return(0);
         }
#endif

     case CFGI_SYSMEM_SIZE:
         if(pBuff && nSize == sizeof(uint32)){
            *((uint32 *)pBuff) = 2048;
            return(0);
         }  
         return(EBADPARM);
         
     case CFGI_MOBILEINFO:
         if(!pBuff || nSize != sizeof(AEEMobileInfo))
            return(EBADPARM);
         GetMobileInfo((AEEMobileInfo *)pBuff);
         return(0);


      case CFGI_AUTOSTART:
         {
         AEECLSID * pcls = (AEECLSID *)pBuff;

         if(pcls && gbCFGI_AUTOSTART)
         {
#if defined(AEE_SIMULATOR) && !defined(_AEEWIN)
            if (!AEESIM_GetAutoStartApp(pcls))
            {
               // could not read devicepack or invalid entry in devicepack,
               // use default
               *pcls = AEECLSID_APPMANAGER;
            }
#else
            *pcls = AEECLSID_APPMANAGER;
#endif
         }

         return(0);
         }

   case CFGI_DNS_IP1:
   case CFGI_DNS_IP2:
      {
         INAddr aa[2];
         int nWhich = (i == CFGI_DNS_IP1) ? 0 : 1;
         
         /* try to retrieve first 2 name servers */
         if ((nWhich + 1) > GetSysDNS(aa, ARR_SIZE(aa))) {
            break;
         }
         
         MEMMOVE(pBuff, &aa[nWhich], min(nSize, sizeof(INAddr)));
         return 0;
      }

   //
   // IPv6 DNS Addresses
   //

   case CFGI_IPV6_DNS_IP1:
      return EUNSUPPORTED;
      
   case CFGI_IPV6_DNS_IP2:
      return EUNSUPPORTED;

   case CFGI_DOWNLOAD:
      {
      AEEDownloadInfo * pdi = (AEEDownloadInfo *)pBuff;

      if(!pBuff || nSize != sizeof(AEEDownloadInfo))
         return(EBADPARM);

      MEMSET((void *)pdi, 0, nSize);
#if FEATURE_COMMERCIAL_SIMULATOR
      AEESIM_GetDownloadInfoFromConfig(pdi->szServer, &(pdi->dwCarrierID), &(pdi->dwPlatformID));
      if(strlen(pdi->szServer) !=0)
      {
      // Specify Auth Policy for Simulator to use
      pdi->nAuth = APOLICY_NONE;
//         pdi->nAuth = APOLICY_SID;
//         pdi->nAuth = APOLICY_TEXT;
//         pdi->nAuth = APOLICY_NUM;
      pdi->wFlags = DIF_TEST_ALLOWED | DIF_MIN_FOR_SID;
      pdi->nPolicy = PPOLICY_BREW_OR_CARRIER;
      return SUCCESS;// assume values are filled up for commercial simulator
      }
#endif
      if(!OEM_GetPrefs(AEECLSID_OEM_DL_CONFIG, OEM_DL_CFG_VER, pBuff, (uint16)nSize))
         return(SUCCESS);
   
      // The following values correspond to values 
      // which can be used with PEK OATDownload module for
      // test purposes: 
      // 1. pdi->szServer = ADS Server
      // 2. pdi->dwCarrierID = Carrier ID
      // 3. pdi->dwPlatformID = Platform ID.
      // You may change these values in 
      // order to use the BREW Simulator with other
      // ADS servers, carrier IDs or platform IDs.
      // The values can be changed either in the Devicepack files or
      // the default values in the AEESIM_GetDownloadConfig function in 
      // ...sdkdev/src/qemu/qemu2.cpp

      AEESIM_GetDownloadConfig(pdi->szServer, &(pdi->dwCarrierID), &(pdi->dwPlatformID));

      // Specify Auth Policy for Simulator to use
      pdi->nAuth = APOLICY_NONE;
//         pdi->nAuth = APOLICY_SID;
//         pdi->nAuth = APOLICY_TEXT;
//         pdi->nAuth = APOLICY_NUM;
      pdi->wFlags = DIF_TEST_ALLOWED | DIF_MIN_FOR_SID;
      pdi->nPolicy = PPOLICY_BREW_OR_CARRIER;
      return(0);
      }

   case CFGI_SUBSCRIBERID:
      // Fetch the info from the device loaded in Simulator, or else use default
      if (!AEESIM_GetSubscriberID((char *)pBuff))
         STRLCPY((char *)pBuff, SIM_SUBSCRIBER_ID, nSize);   // default
      return(0);

   case CFGI_MODULE_FSLIMIT:
   {
      MIFFSLimit *   pfsLimit = (MIFFSLimit *)pBuff;

      if (!pBuff || (size_t)nSize < sizeof(MIFFSLimit))
         return EBADPARM;

      // Fetch the info from the device loaded in Simulator
      AEESIM_GetFSLimit(pfsLimit);

      return 0;
   }

   case CFGI_APP_KEY_1:
   {
      // Mechanism for the BREW Simulator to specify CFGI_APP_KEY_1.  
      // For information on using this with a device, please refer
      // to documentation for OEM_GetConfig().
      OEMConfigData* pConfigData = (OEMConfigData*)pBuff;
      uint32 dwSize;
      int nErr = EFAILED;
      void* fp;

#define APPKEY_FILENAME AEEFS_SYS_DIR"priv/appkey1.dat"

      if (OEMFS_GetAttributes(APPKEY_FILENAME, 0,0,&dwSize) == SUCCESS)
      {
         if (dwSize > pConfigData->nSize)
         {
            pConfigData->nSize = dwSize;
            nErr = ENEEDMORE;
         }
         else
         {
            nErr = OEMFS_Open(APPKEY_FILENAME, _OFM_READ, &fp);
            if (SUCCESS == nErr) {

               int32 nRead;

               nRead = OEMFS_Read(fp, pConfigData->pData, pConfigData->nSize,
                                  &nErr);

               if (nRead >= 0 && nRead <= (int32)pConfigData->nSize) {
                  nErr = SUCCESS;
               }

               pConfigData->nSize = (uint32)nRead;
               OEMFS_Close(fp);
            }
         }
      }

      return nErr;
   }

#if defined(FEATURE_BREW_DEV) || defined(AEE_SIMULATOR)
      // short safe mode timer during development and emulation
      case CFGI_SAFEMODE_TIMER:
         if(nSize != sizeof(int32))
            return EBADPARM;
         *(int32*)pBuff = 5;
         return SUCCESS;
#endif // defined(FEATURE_BREW_DEV) || defined(AEE_SIMULATOR)

      case CFGI_MOBILE_DIR_NUM_LEN:
         if(nSize != sizeof(int))
            return EBADPARM;
         *(int *)pBuff = STRLEN(SIM_SUBSCRIBER_ID) + 1;
         return SUCCESS;

      case CFGI_MOBILE_DIR_NUM:
         if(nSize < (int) STRLEN(SIM_SUBSCRIBER_ID))
            return EBADPARM;
         STRLCPY((char *)pBuff, SIM_SUBSCRIBER_ID, nSize); 
         return SUCCESS;

      case CFGI_FIRMWARE_ID_LEN:
         if(nSize != sizeof(int))
            return EBADPARM;
         return GetFirmwareVersion(NULL, (int *)pBuff);

      case CFGI_FIRMWARE_ID:
      {
         int nVerLen;
         if(GetFirmwareVersion(NULL, (int *)&nVerLen))
            return EUNSUPPORTED;
         if(nSize < nVerLen)
            return EBADPARM;
         return GetFirmwareVersion((byte *)pBuff, &nVerLen); 
      }      
     
      case CFGI_SMS_GW_SCADDRESS:           
         if(nSize < (int)(STRLEN(SIM_SUBSCRIBER_ID) + 1))
            return EBADPARM;
         STRNCPY((char*)pBuff, SIM_SUBSCRIBER_ID, STRLEN(SIM_SUBSCRIBER_ID));
         return SUCCESS;
         
      case CFGI_SMS_GW_SCADDRESS_LENGTH:           
         if(nSize < sizeof(uint32))
            return EBADPARM;         
         *(uint32*)pBuff = STRLEN(SIM_SUBSCRIBER_ID) + 1;
         return SUCCESS;
         
      // Maximum payload length in terms of number of bytes
     case CFGI_SMS_MAX_PAYLOAD_LENGTH:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = 140;
         return SUCCESS;       
         
      // Default encoding for MO SMS
      case CFGI_SMS_DEFAULT_MO_ENC:
         if(nSize < sizeof(AEESMSEncType))
            return EBADPARM;

	 if (AEESIM_IsGSMNetwork() == TRUE)
	 {
	    *(AEESMSEncType*)pBuff = AEESMS_ENC_UNICODE;
	 }
	 else
	 {
            *(AEESMSEncType*)pBuff = AEESMS_ENC_ASCII;
	 }

         return SUCCESS;
         
      // Count of encoding types available for MO SMS
      case CFGI_SMS_MO_ENC_TYPES_COUNT:  
         if(nSize < sizeof(uint32))
            return EBADPARM;

	 if (AEESIM_IsGSMNetwork() == TRUE)
	 {
	    *(uint32*)pBuff = 3;
	 }
	 else
	 {
	    *(uint32*)pBuff = 5;
	 }

         return SUCCESS;
         
      // Encoding types available for MO SMS
      case CFGI_SMS_MO_ENC_TYPES:
	 if (AEESIM_IsGSMNetwork() == TRUE)
	 {
	    if(nSize < sizeof(uint32)*3)
	    {
               return EBADPARM;
	    }

	    *(uint32*)  pBuff       = AEESMS_ENC_UNICODE;
	    *(((uint32*)pBuff) + 1) = AEESMS_ENC_OCTET;
	    *(((uint32*)pBuff) + 2) = AEESMS_ENC_GSM_7_BIT_DEFAULT;
	 }
	 else
	 {
	    if(nSize < sizeof(uint32)*5)
	    {
	       return EBADPARM;
	    }
 
            *(uint32*)  pBuff       = AEESMS_ENC_ASCII;
            *(((uint32*)pBuff) + 1) = AEESMS_ENC_LATIN;
            *(((uint32*)pBuff) + 2) = AEESMS_ENC_UNICODE;
            *(((uint32*)pBuff) + 3) = AEESMS_ENC_OCTET;
	    *(((uint32*)pBuff) + 4) = AEESMS_ENC_GSM_7_BIT_DEFAULT;
	 }

         return SUCCESS;

      // boolean, if TRUE, SMS message is sent on access channel
      case CFGI_SMS_MO_ON_ACCESS_CHANNEL:
         if(nSize < sizeof(boolean))
            return EBADPARM;
         *(boolean*)pBuff = TRUE;
         return SUCCESS;

      // boolean, if TRUE, SMS message is sent on traffic channel. If both of
      // CFGI_SMS_MO_ON_ACCESS_CHANNEL and CFGI_SMS_MO_ON_TRAFFIC_CHANNEL
      // are TRUE, SMS message is first sent on access channel and on being
      // too large for access channel is sent on traffic channel
      case CFGI_SMS_MO_ON_TRAFFIC_CHANNEL:
         if(nSize < sizeof(boolean))
            return EBADPARM;
         *(boolean*)pBuff = TRUE;
         return SUCCESS;

      case CFGI_LNG:
         if(nSize < sizeof(uint32))
            return EBADPARM;
         *(uint32*)pBuff = AEESIM_GetLang();
         return SUCCESS;

      case CFGI_SMS_VERIFY_BREW_HANDLER:
         if (nSize < sizeof(boolean)) {
            return EBADPARM;
         }
         *((boolean *)pBuff) = TRUE;         
         return SUCCESS;

         
      case CFGI_BACKLIGHT_CLASSIDS_LIST:
         if(nSize < sizeof(uint32)*5)
            return EBADPARM;
         *(uint32*)pBuff = AEECLSID_BACKLIGHT_DISPLAY1;
         *(((uint32*)pBuff) + 1) = AEECLSID_BACKLIGHT_DISPLAY2;
         *(((uint32*)pBuff) + 2) = AEECLSID_BACKLIGHT_DISPLAY3;
         *(((uint32*)pBuff) + 3) = AEECLSID_BACKLIGHT_DISPLAY4;
         *(((uint32*)pBuff) + 4) = 0;
         return SUCCESS;

      case CFGI_GPSONE_LOCK:
         if(nSize < sizeof(AEEGPSOneLockInfo)) {
            return EBADPARM;
         }
         *(AEEGPSOneLockInfo *)pBuff = gCFGI_GPSONE_LOCK;
         return SUCCESS;

         
      case CFGI_LAZY_ACK:
         if(nSize < sizeof(boolean))
            return EBADPARM;
         *(boolean*)pBuff = TRUE;
         return SUCCESS;

      // Type boolean, if TRUE, OEM app Context notifications sent via 
      // OEM_Notify, else not.
      case CFGI_OEM_NOTIFY_APP_CTXT:
         if(nSize < sizeof(boolean))
         {
            return EBADPARM;
         }   
         *(boolean*)pBuff = FALSE;
         return SUCCESS;

      case CFGI_THEMENAME_LEN:
      case CFGI_THEMES_INI_LEN:
      case CFGI_WALLPAPERS_INI_LEN:
      case CFGI_RINGTONES_INI_LEN:
      {
         char buffer[128];
         AEEConfigItem pair = 0;
         if(nSize < sizeof(uint32))
            return EBADPARM;
         if (i == CFGI_THEMENAME_LEN)           pair = CFGI_THEMENAME;
         else if (i == CFGI_THEMES_INI_LEN)     pair = CFGI_THEMES_INI;
         else if (i == CFGI_WALLPAPERS_INI_LEN) pair = CFGI_WALLPAPERS_INI;
         else if (i == CFGI_RINGTONES_INI_LEN)  pair = CFGI_RINGTONES_INI;
         else return EUNSUPPORTED;
         OEM_GetBrewConfig(pair, buffer, sizeof(buffer));
         *(uint32*)pBuff = STRLEN(buffer);
         return SUCCESS;
      }

      case CFGI_THEMENAME:
      case CFGI_SUBLCD_PROPS:
      case CFGI_ANNUN_PROPS:
      case CFGI_THEMES_INI:
      case CFGI_WALLPAPERS_INI:
      case CFGI_RINGTONES_INI:
         OEM_GetBrewConfig(i, pBuff, nSize);
         return SUCCESS;

      
      case CFGI_OEMAUTH_CHALLENGE_CAP:
      {
         char buffer[32];
         OEM_GetBrewConfig(i, buffer, nSize);
         *(uint32*)pBuff = STRTOUL(buffer, NULL, 10); //decimal
         return SUCCESS;
      }

      case CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN:
      case CFGI_OEMAUTH_RUIM_CARDID:
      {
         char buffer[32];
         OEM_GetBrewConfig(i, buffer, sizeof(buffer));
         *(uint32*)pBuff = STRTOUL(buffer, NULL, 10); //decimal
         return SUCCESS;
      }

      default:
         return(EUNSUPPORTED);
   }
   return(EBADPARM);
}



/*==================================================================
Function: OEM_SetConfig

Description: Sets configuration information from the OEM

Prototype:
   int  OEM_SetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_SetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   int nErr = EUNSUPPORTED;

   /* First try to delegate the IConfig item to the extender */
   {
      IConfig *pIConfigExtender = CustomOEMConfig_GetConfigExtender();
      if (pIConfigExtender != NULL) {
         int nErr = ICONFIG_SetItem(pIConfigExtender, i, pBuff, nSize);
         pIConfigExtender = NULL;  // No release required, spIConfigExtender is released on BREW shutdown
         if (EUNSUPPORTED != nErr) {
            return nErr;
         }
      }
   }

   //did not handle SetConfig via pIConfigExtender, hence proceed to the usual handling
   // 
   // NOTE: This code should be replaced by code that stores/retrieves values in
   // NV-RAM.  You will need to add NV items for each of the items referenced here.
   //

   switch(i) {
      case CFGI_DOWNLOAD:
         nErr = OEM_SetPrefs(AEECLSID_OEM_DL_CONFIG, OEM_DL_CFG_VER, pBuff, (uint16)nSize);
         break;

      case CFGI_BREWMA_CAPABILITIES:
         nErr = OEM_SetPrefs(AEECLSID_IMUTUALAUTH, OEM_MA_CAPAB_VER, pBuff, (uint16)nSize);
         break;

      case CFGI_DISABLE_MA:
      {
         if( nSize != sizeof( uint32 ) ) {
            nErr = EBADPARM;
         }
         else
         {
            gdwDisableMA = *(uint32*)pBuff; 
            nErr = SUCCESS;
         }
      }
      break;

      case CFGI_LAST_VALID_TIME:
      {
         if(nSize != sizeof(LastValidCfg)) {
            nErr = EBADPARM;
         }
         else {
            nErr = OEM_SetPrefs(AEECLSID_LAST_VALID_CFG, OEM_LAST_VALID_VER, pBuff, (uint16)nSize);
         }
      }
      break;

      case CFGI_USER_CLOCK_OFFSET:
      {
         if(nSize != sizeof(int32)) {
            nErr = EBADPARM;
         }
         else {
            nErr = OEM_SetPrefs(AEECLSID_USR_CLK_OFF, OEM_USR_CLK_OFF_VER, pBuff, (uint16)nSize);
         }
      }
      break;

      case CFGI_THEMENAME:
      case CFGI_ANNUN_PROPS:
      case CFGI_THEMES_INI:
      case CFGI_WALLPAPERS_INI:
      case CFGI_RINGTONES_INI:
      {
         nErr = OEM_SetBrewConfig(i, pBuff, nSize);
      }
      break;

      case CFGI_GPSONE_LOCK:
         if(nSize < sizeof(AEEGPSOneLockInfo))
            return EBADPARM;
         gCFGI_GPSONE_LOCK = *(AEEGPSOneLockInfo*)pBuff;
         return SUCCESS;

      default:
         break;
   }
         
   return nErr;
}

static void GetMobileInfo(AEEMobileInfo * pmi)
{
   char pszESN[OEM_MAX_ESN_STRING_SIZE];

   if(!pmi)
      return;

   MEMSET(pmi, 0, sizeof(AEEMobileInfo));

   // read the ESN from the device pack, if not found, use default
   if ( AEESIM_GetMobileInfo(pszESN, OEM_MAX_ESN_STRING_SIZE) )
   {
      // See if the value is decimal or Hex
      if ( STRSTR(pszESN, "0x") ) // Hex
      {
         pmi->dwESN = STRTOUL(pszESN, NULL, 16);
      }
      else  // decimal
         pmi->dwESN = STRTOUL(pszESN, NULL, 10);
   }
   else  // not found in device pack, use default
      pmi->dwESN = 0xbabababa;

   pmi->nCurrNAM = 100;

   STRLCPY(pmi->szMobileID, SIM_SUBSCRIBER_ID, sizeof(pmi->szMobileID));
}


static int OEM_ReadESN(uint32 *pESN)
{
   char pszESN[OEM_MAX_ESN_STRING_SIZE];

   // read the ESN from the device pack, if not found, use default
   if ( AEESIM_GetMobileInfo(pszESN, OEM_MAX_ESN_STRING_SIZE) )
   {
      // See if the value is decimal or Hex
      if ( STRSTR(pszESN, "0x") ) // Hex
      {
        *pESN = STRTOUL(pszESN, NULL, 16);
      }
      else  // decimal
         *pESN = STRTOUL(pszESN, NULL, 10);
   }
   else  // not found in device pack, use default
      *pESN = 0xbabababa;
   return SUCCESS;
}

static int OEM_ReadIMEI(byte *pIMEI)
{
   char pszIMEI[OEM_MAX_IMEI_STRING_SIZE];
   int i;

   // read the IMEI from the device pack, if not found, use default
   if ( AEESIM_GetIMEI(pszIMEI, OEM_MAX_IMEI_STRING_SIZE) )
   {
      // pad with zeros, if too short
      STRLCAT(pszIMEI, "000000000000000", OEM_MAX_IMEI_STRING_SIZE);
      pIMEI[0] = 8;
      pIMEI[1] = ((pszIMEI[0]-'0') << 4) + 0xa;
      for (i = 2; i <= 8; i++) {
         pIMEI[i] = ((pszIMEI[(i-1)*2-1] - '0')) +
                    ((pszIMEI[(i-1)*2] - '0') << 4);
      }

      return SUCCESS;
   }
   return EUNSUPPORTED;
}

static int OEM_ReadMEID(uint64 *pMEID)
{
   char aMEID[OEM_MAX_MEID_STRING_SIZE];
   char * pszMEID = aMEID;

   // read the MEID from the device pack, if not found, use default
   if ( AEESIM_GetMEID(pszMEID, OEM_MAX_MEID_STRING_SIZE) )
   {
      // See if the value has '0x' prefix
      if ( STRSTR(pszMEID, "0x") ) // Hex
      {
         pszMEID+=2;
      }
      STRLCAT(pszMEID, "000000000000000", OEM_MAX_MEID_STRING_SIZE);
      // assume little endian...
      pszMEID[14] = 0;
      (((uint32* )pMEID)[0]) = STRTOUL(&pszMEID[6], NULL, 16);
      pszMEID[6] = 0;
      (((uint32* )pMEID)[1]) = STRTOUL(pszMEID, NULL, 16);

      return SUCCESS;
   }
   return EUNSUPPORTED;
}

#define MEID_SIZE (15) // 14 digits + eos. doesn't include checksum

static int OEM_ReadMEIDS(byte *  meid_ptr)
{
   int index = 0;
   uint64 meid;
   byte asc;

   if (SUCCESS != OEM_ReadMEID(&meid)) {
      return -1;
   }

   for (index = 0; index < MEID_SIZE-1; index++) {
      asc = (byte) (meid >> ((MEID_SIZE-2 - index)*4)) & 0xf;
      meid_ptr[index] = (asc <= 9)?asc+'0' : asc-10+'a';
   }
   meid_ptr[MEID_SIZE-1] = 0;

   return MEID_SIZE;
}

/*======================================================================= 
Function: OEM_GetHWIDList()

Description: 
   This method returns either the Primary hardware id or the entire
   hardware id list.  In all cases, the first item is always the primary hardware
   id and should not change on a device.

Prototype:

   int   OEM_GetHWIDList(byte *pList, int *pwLength, boolean bPrimary)

Parameters:
   pList: pointer to space allocated for HWIDList
   pwLength: pointer to length.  On input this points to the size of pList.
             On output it contains the number of bytes required for the list.
   bPrimary: If true, return primary id only.

Return Value: 
   SUCCESS: If successfully created list.
   EUNSUPPORTED: no hardware IDs found.  BREW relies on a valid hardware id,
            so this could be fatal.

Comments:  
   None.

Side Effects: 
   None

See Also: 
   AEE_DEVICEITEM_HWID
   AEE_DEVICEITEM_HWIDList

=======================================================================*/
static int OEM_GetHWIDList(byte * pList, int * pwLength, boolean bPrimary)
{
   int listLength = 0;
   int len = 0;
   AEEHWIDType *pHWID = (AEEHWIDType *)pList;
   boolean bSpace = TRUE;     // keep order consistent - if one doesn't fit,
                              //  following items are skipped also

   if (pHWID && *pwLength >= sizeof(pHWID->uLength)) {
      // mark the initial item as zero length in case nothing else fits
      pHWID->uLength = 0;
   }
   if (!bPrimary) {
      listLength += sizeof(pHWID->uLength);  // for list, always allow minimum space for end of list marker
   }
   // add items onto HW List in this priority:
   // MEID, IMEI, ESN, pESN
   {
      uint64 meid;
      if (SUCCESS == OEM_ReadMEID(&meid)) {
         // length calculation is equivalent to structure_size - 4 + (sizeof(meid)/4)*4
         len = sizeof(AEEHWIDType) + ((sizeof(meid)-1) & 0xFFFC);
         listLength += len;
         // big enough?
         if (pList && *pwLength >= listLength) {
            pHWID->uLength       = len;
            pHWID->bHWIDLength   = sizeof(meid);
            pHWID->bHWIDType      = AEE_HWID_MEID;
            memcpy( (byte *) (pHWID->HWID), (byte *)&meid, sizeof(meid));

            pList += pHWID->uLength;
            pHWID = (AEEHWIDType *) pList;
         }
         else {
            bSpace = FALSE;
         }
         if (bPrimary) {
            *pwLength = listLength;
            return SUCCESS;
         }
      }
   }

   {
      byte imei[9];
      uint32 imei_bcd_len;
      if (SUCCESS == OEM_ReadIMEI(imei)) {
      	/* The first byte of NV ue_imei is the length of IMEI BCDs */
         imei_bcd_len = imei[0];
         len = sizeof(AEEHWIDType) + ((imei_bcd_len-1) & 0xFFFC);
         listLength += len;
         if (pList && *pwLength >= listLength && bSpace) {
            pHWID->uLength       = len;
            pHWID->bHWIDLength   = (byte) imei_bcd_len;
            pHWID->bHWIDType      = AEE_HWID_IMEI;
            memcpy( (byte *) (pHWID->HWID), (byte *)&imei[1], imei_bcd_len);
            pList += len;
            pHWID = (AEEHWIDType *) pList;
   
         }
         else {
            bSpace = FALSE;
         }
         if (bPrimary) {
            *pwLength = listLength;
            return SUCCESS;
         }
      }
   
   }

   {
      uint32 esn_me;
      if (SUCCESS == OEM_ReadESN(&esn_me) && esn_me != 0) {
         len = sizeof(AEEHWIDType) + ((sizeof(esn_me)-1) & 0xFFFC);
         listLength += len;
         // big enough?
         if (pList && *pwLength >= listLength && bSpace) {
            pHWID->uLength       = len;
            pHWID->bHWIDLength   = sizeof(esn_me);
            if (0x80000000L == (esn_me & 0xff000000L)) {
               pHWID->bHWIDType = AEE_HWID_PESN;
            }
            else {
               pHWID->bHWIDType = AEE_HWID_ESN;
            }
            memcpy( (byte *) (pHWID->HWID), (byte *)&esn_me, sizeof(esn_me));
            pList += len;
            pHWID = (AEEHWIDType *) pList;
         }
         if (bPrimary) {
            *pwLength = listLength;
            return SUCCESS;
         }
      }
   }
   listLength += sizeof(AEEHWIDType) - sizeof(pHWID->uLength) - sizeof (pHWID->HWID[0]);
   if (pList && *pwLength >= sizeof(pHWID->uLength)) {
      // room for uLength was allocated up front
      pHWID->uLength       = 0;
      if (*pwLength >= listLength) {
         pHWID->bHWIDLength   = 0;
         pHWID->bHWIDType = AEE_HWID_NONE;
      }
   }

   *pwLength = listLength;

   // found at least 1 id?
   if (listLength > (sizeof(AEEHWIDType) - sizeof(pHWID->HWID[0])))
      return SUCCESS;
   else
      return(EUNSUPPORTED);
}

void OEM_GetDeviceInfo(AEEDeviceInfo * pi)
{
   AEEDownloadInfo sDownloadInfo;
   AEEBitmapInfo bi;
   int nSize = sizeof(bi);

   if(!pi) 
      return;
   
   AEESIM_GetDisplayInfo(AVS_SCREEN_0, &bi, &nSize);
   
   memset(pi,0, sizeof(AEEDeviceInfo));
   pi->cxScreen = (uint16)bi.cx;
   pi->cyScreen = (uint16)bi.cy;
   pi->cxScrollBar = 4;
   pi->nColorDepth = (uint16)bi.nDepth;
   
   pi->dwRAM = AEESIM_GetHeapSize();
   
   pi->bPosLoc = 1;
   pi->dwPromptProps = CP_3D_BORDER|ST_MIDDLETEXT|ST_UNDERLINE;
   pi->wKeyCloseApp     = AVK_CLR;
   pi->wKeyCloseAllApps = AVK_END;
   
   // Set Language Support...
   {
      uint32 dwLang;
      if (SUCCESS == OEM_GetConfig(CFGI_LNG, &dwLang, sizeof(dwLang))) {
         pi->dwLang = dwLang;
      } else {
         pi->dwLang = AEESIM_GetLang();
      }
   }
   pi->wEncoding = AEESIM_GetEnc();

   pi->wMaxPath  = (uint16)OEMFS_GetMaxPathLength();
   
#if defined(FEATURE_UI_FLIP_PHONE)
   pi->bFlip            = TRUE;
   pi->bAltDisplay      = FALSE;
   pi->cxAltScreen      = 0;
   pi->cyAltScreen      = 0;
#endif
   
#if defined(FEATURE_VIBRATOR)
   pi->bVibrator = TRUE;
#endif
   
#if defined(FEATURE_SPEAKER_PHONE)
   pi->bExtSpeaker = TRUE;
#endif
   
#if defined(FEATURE_PHONE_VR)
   pi->bVR = TRUE;
#endif
     
   if (SUCCESS == OEM_GetConfig(CFGI_DOWNLOAD, &sDownloadInfo, sizeof(AEEDownloadInfo))) {
      pi->dwPlatformID = sDownloadInfo.dwPlatformID;
   }

   pi->bMIDI = AEESIM_IsMIDISupported();
}


/*===========================================================================

===========================================================================*/
int   OEM_GetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int *pnSize)
{
   switch (nItem) {
      case AEE_DEVICEITEM_CHIP_ID:
      {
         char szStr[32];

         if(!pnSize)
            return(EBADPARM);

         if(pBuff && *pnSize) {
            STRLCPY(szStr,"Simulator", sizeof(szStr));
            STRTOWSTR(szStr,(AECHAR *)pBuff,*pnSize);
         }
         *pnSize = (STRLEN("Simulator")+1)*2;
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_MOBILE_ID:
      {
         AEEMobileInfo mi;

         MEMSET(&mi,0,sizeof(AEEMobileInfo));

         if(!pnSize)
            return(EBADPARM);

         if(!OEM_GetConfig(CFGI_MOBILEINFO,&mi,sizeof(AEEMobileInfo))) {
            TAPI_CompressNumber(mi.szMobileID);
            if(pBuff && *pnSize > 0)
               STRLCPY(pBuff,mi.szMobileID,*pnSize);
            *pnSize = STRLEN(mi.szMobileID)+1;
            return(SUCCESS);
         }
         else
            return(EUNSUPPORTED);
      }

      case AEE_DEVICEITEM_IMEI:
      {
         char pszIMEI[OEM_MAX_IMEI_STRING_SIZE];

         if ( AEESIM_GetIMEI(pszIMEI, OEM_MAX_IMEI_STRING_SIZE) )
         {
            if(pBuff && *pnSize > 0)
               STRLCPY(pBuff,pszIMEI,*pnSize);
            *pnSize = STRLEN(pszIMEI)+1;
            return SUCCESS;
         }
         return EUNSUPPORTED;
      }

      case AEE_DEVICEITEM_MEIDS:
      {
         char pszMEID[MEID_SIZE];
         int size = OEM_ReadMEIDS(pszMEID);

         if (size <= 0) {
            return EUNSUPPORTED;
         }
         if (!pnSize) {
            return(EBADPARM);
         }
         if (pBuff && *pnSize >= MEID_SIZE) {
            STRNCPY((char*)pBuff, (char*)pszMEID, *pnSize);
         }
         *pnSize = MEID_SIZE;
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_HWID:
      if (!pnSize) {
         return(EBADPARM);
      }
      return OEM_GetHWIDList(pBuff, pnSize, TRUE);
      
      case AEE_DEVICEITEM_HWIDLIST:
      if (!pnSize) {
         return(EBADPARM);
      }
      return OEM_GetHWIDList(pBuff, pnSize, FALSE);
   
      case AEE_DEVICEITEM_CARRIER_ID:
      {
         AEEDownloadInfo di;

         MEMSET(&di,0,sizeof(AEEDownloadInfo));

         if(!pnSize)
            return(EBADPARM);

         if(!OEM_GetConfig(CFGI_DOWNLOAD,&di,sizeof(AEEDownloadInfo))) {
            if(pBuff && (*pnSize == sizeof(di.dwCarrierID)))
               *(uint32 *)pBuff = di.dwCarrierID;
            *pnSize = sizeof(di.dwCarrierID);
            return(SUCCESS);
         }
         else
            return(EUNSUPPORTED);
      }

      case AEE_DEVICEITEM_SYS_COLORS_DISP1:
      case AEE_DEVICEITEM_SYS_COLORS_DISP2:
      case AEE_DEVICEITEM_SYS_COLORS_DISP3:
      case AEE_DEVICEITEM_SYS_COLORS_DISP4:
      {
         const RGBVAL *pColors = AEESIM_GetSysColors(nItem - AEE_DEVICEITEM_SYS_COLORS_DISP1 + AVS_SCREEN_0);
         int nSize;

         if (NULL == pColors) {
            return EUNSUPPORTED;
         }

         if(!pnSize)
            return(EBADPARM);

         nSize = MIN(*pnSize, CLR_SYS_LAST * sizeof(RGBVAL));
         *pnSize = CLR_SYS_LAST * sizeof(RGBVAL);

         if (NULL == pBuff) {
            return SUCCESS;
         }

         MEMCPY(pBuff, pColors, nSize);

         return SUCCESS;
      }

      case AEE_DEVICEITEM_DISPINFO1:
      case AEE_DEVICEITEM_DISPINFO2:
      case AEE_DEVICEITEM_DISPINFO3:
      case AEE_DEVICEITEM_DISPINFO4:
         return AEESIM_GetDisplayInfo(nItem - AEE_DEVICEITEM_DISPINFO1 + AVS_SCREEN_0, pBuff, pnSize);

      case AEE_DEVICEITEM_SOFTKEY_COUNT:
      {
         if (!pnSize)
            return EBADPARM;

         if (pBuff && *pnSize >= sizeof(uint8))
            *((uint8 *)pBuff) = AEESIM_GetSoftkeyCount(); 
         *pnSize = sizeof(uint8);
         return SUCCESS;
      }

      case AEE_DEVICESTATE_FLIP_OPEN:
      {
         BOOL bFlipClose;
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         if (AEESIM_GetFlipClose(&bFlipClose))
         {
            if (bFlipClose) //flip is closed, AEE_DEVICESTATE_FLIP_OPEN should indicate FALSE (device in flip closed state)
            {
               *((boolean *)pBuff) = FALSE;
            }
            else
            {
               *((boolean *)pBuff) = TRUE;
            }
         }
         else
         {
            *((boolean *)pBuff) = FALSE;
         }
         
         *pnSize = sizeof(boolean);

         return(SUCCESS);
      }

      case AEE_DEVICESTATE_HEADPHONE_ON:
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = FALSE;
         *pnSize = sizeof(boolean);

         return(SUCCESS);
      }

      case AEE_DEVICESTATE_KEYGUARD_ON:
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = FALSE;
         *pnSize = sizeof(boolean);

         return(SUCCESS);
      }

      case AEE_DEVICESTATE_SPEAKER_ON:
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = FALSE;
         *pnSize = sizeof(boolean);

         return(SUCCESS);
      }

      case AEE_DEVICESTATE_VIBRATOR_ON:
      {
         if (!pnSize) {
            return(EBADPARM);
         }

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEMSound_IsVibratorON();
         *pnSize = sizeof(boolean);

         return(SUCCESS);
      }

      case AEE_DEVICEITEM_MANNER_MODE:
      {
         if (!pnSize)
            return EBADPARM;

         if (!pBuff || *pnSize < sizeof(uint32))
         {
            *pnSize = sizeof(uint32);
            return(SUCCESS);
         }
         *((uint32 *)pBuff) = (uint32)AEE_MANNER_MODE_NORMAL;
         *pnSize = sizeof(uint32);
         return SUCCESS;
      }

   case AEE_DEVICEITEM_KEY_SUPPORT:
      if (!pnSize)
         return EBADPARM;

      if (pBuff && *pnSize >= sizeof(KeySupportType))
         AEESIM_GetKeySupported((KeySupportType*)pBuff); 
      *pnSize = sizeof(KeySupportType);
      return SUCCESS;

   case AEE_DEVICEITEM_DOWNLOAD_HEADER:
      if (!pnSize)
         return EBADPARM;

      GetDownloadHeader((char *)pBuff, pnSize);

      return SUCCESS;
   
   case AEE_DEVICEITEM_ICCID:
      if (!pnSize) {
         return(EBADPARM);
      }

      if (!pBuff || *pnSize < (int)(STRLEN("Simulator") + 1)) {
         *pnSize = STRLEN("Simulator") + 1;
      } else {
         STRNCPY((char*)pBuff, "Simulator", *pnSize);
         *pnSize = (STRLEN("Simulator") + 1);
      }
      return(SUCCESS);

   case AEE_DEVICEITEM_POSDET_PRIVACY_ALERT:
   {
      if (!pnSize) {
         return(EBADPARM);
      }

      if(!pBuff || *pnSize < sizeof(boolean)) {
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }
      
#ifdef FEATURE_BREW_PRIVACY_PROMPT
      *((boolean *)pBuff) = TRUE;
#else
      *((boolean *)pBuff) = FALSE;
#endif /* FEATURE_BREW_PRIVACY_PROMPT */

      *pnSize = sizeof(boolean);

      return(SUCCESS);
   }

   case AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY:
   {   

      if (!pnSize) {
         return(EBADPARM);
      }
      
      if(!pBuff || *pnSize < sizeof(boolean)) {
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

      return OEM_GetConfig(CFGI_GPSONE_LOCK, pBuff, *pnSize);

   }

   case AEE_DEVICEITEM_FSSPACE_CARD0:
   case AEE_DEVICEITEM_FSSPACE_ROOT:
   {
      int nErr;
      uint64 qwTotalSpace;
      uint64 qwFreeSpace;

      if (!pnSize) {
         return(EBADPARM);
      }

      if(!pBuff || *pnSize < sizeof(AEEFSFreeSpace)) {
         *pnSize = sizeof(AEEFSFreeSpace);
         return(AEE_SUCCESS);
      }

      if (AEE_DEVICEITEM_FSSPACE_CARD0 == nItem) {
         nErr = OEMFS_StatVFS(AEEFS_CARD0_DIR, &qwTotalSpace, &qwFreeSpace);
      }
      else if (AEE_DEVICEITEM_FSSPACE_ROOT == nItem) {
         nErr = OEMFS_StatVFS(AEEFS_ROOT_DIR, &qwTotalSpace, &qwFreeSpace);
      }
      else {
         return EBADPARM;
      }
      
      if (SUCCESS == nErr) {
         AEEFSFreeSpace* pFSFreeSpace = (AEEFSFreeSpace*) pBuff;
         pFSFreeSpace->qwTotalSpace = qwTotalSpace;
         pFSFreeSpace->qwFreeSpace  = qwFreeSpace;
      }

      return nErr;
   }

   default:
      return(EUNSUPPORTED);
   }
}


/*===========================================================================

===========================================================================*/
static uint64 GetDirSpace(const char * pszDir)
{
   IFileMgr *  pfm;
   uint64      qwTotal = 0;
   FileInfo    fi;
   IShell * pIShell = AEE_GetShell();

   ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)(&pfm));
   if(pfm){
      if(!IFILEMGR_EnumInit(pfm,pszDir,FALSE)){
         while(IFILEMGR_EnumNext(pfm,&fi)){
            if(!(fi.attrib & AEE_FA_CONST))
               qwTotal += fi.dwSize;
         }
      }                  
      IFILEMGR_Release(pfm);
   }
   return(qwTotal);
}

/*===========================================================================

===========================================================================*/
static void GetDownloadFSAvail(DLItemSize * pi)
{
   AEEFSFreeSpace fs;

   GetDownloadFSAvailEx(&fs);   
   
   // Cap our return values at MAX_UINT32 to prevent wrapping
   pi->dwFSSize = (fs.qwTotalSpace < MAX_UINT32)? (uint32)fs.qwTotalSpace : MAX_UINT32;
   pi->dwFSAvail = (fs.qwFreeSpace < MAX_UINT32)? (uint32)fs.qwFreeSpace : MAX_UINT32;
}

/*===========================================================================

===========================================================================*/
static void GetDownloadFSAvailEx(AEEFSFreeSpace * pfs)
{
   IFileMgr *  pfm;
   uint64      qwTotal = 0;
   FileInfo    fi;
   char *      psz;
   IShell * pIShell = AEE_GetShell();

#define DOWNLOAD_FS_SPACE     (1024l * 3000l)
   pfs->qwTotalSpace = DOWNLOAD_FS_SPACE;

   AEESIM_GetEFSMax(&pfs->qwTotalSpace, NULL);

   ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)(&pfm));
   if(pfm){
      if(!IFILEMGR_EnumInit(pfm,AEEFS_MOD_DIR,TRUE)){
         while(IFILEMGR_EnumNext(pfm,&fi)){
            psz = STRRCHR(fi.szName,'/');
            if(psz){
               psz++;
               if(STRTOUL(psz,NULL,10))
                  qwTotal += GetDirSpace(fi.szName);
            }
         }
      }                  
      IFILEMGR_Release(pfm);
   }

   if(qwTotal > pfs->qwTotalSpace)
      pfs->qwFreeSpace = 0;
   else
      pfs->qwFreeSpace = pfs->qwTotalSpace - qwTotal;
}


//Simulator Specific Functions. These are used on the BREW Simulator only. 
//These are provided here so that carriers can test the IDle App. Valid 
//values must be returned for the TimeOut and the IdleAppClass.  Then, 
//when the timeout expires, first a EVT_BUSY event is sent to the currently 
//running app. If that app returns FALSE, then the ScreenSaver is started. 
//When any key is pressed, control goes back to the currently running app.


//Return Idle App Timeout value in milliseconds
uint32 OEM_GetIdleAppTimeout()
{
   //Number of millisecons to wait before launching IdleApp. 
   //If you are using IdleApp in the Simulator, a non-zero value 
   // must be returned.
   
   // return(2000);
   
   return(0);
}

uint32 OEM_GetIdleAppCls()
{
   //If you are using IdleApp in the Simulator, please return
   //a valid classID of IdleApp to work
   
   //return(AEECLSID_IDLEAPP_BID);
   
   return(0);
}

void EnableCFGI_AUTOSTART(boolean bAutoStart)
{
   gbCFGI_AUTOSTART = bAutoStart;
}

boolean OEM_IsClsOKInSafeMode(uint32 clsid)
{
   // always need to be able to start the core app
   if (AEECLSID_APPMANAGER == clsid) {
      return TRUE;
   }

   return FALSE;
}

void SetCFGI_AUTOSTARTARGS(AECHAR * pArgs)
{
   gpCFGI_AUTOSTARTARGS = pArgs;
}

static int GetFirmwareVersion(byte *pVer, int *pnVerLen)
{
   char            szFirmware[64];
#if   !defined(_WIN32_WCE)
   OSVERSIONINFOEX osvi;
   BOOL            bOsVersionInfoEx;

   if(!pnVerLen)
      return EFAILED;

   MEMSET(&osvi, 0, sizeof(OSVERSIONINFOEX));
   MEMSET(szFirmware, '\0', 64);


   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         SNPRINTF(szFirmware, 64, "1.2.3.4");
   }
#else
   OSVERSIONINFO osvi;
   
   osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
   if (! GetVersionEx (&osvi) ) 
      SNPRINTF(szFirmware, 64, "1.2.3.4");
#endif   
   SNPRINTF(szFirmware, 64, "%d.%d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);

   if(!pVer)
      *pnVerLen = STRLEN(szFirmware) + 1;
   else
      STRLCPY(pVer, szFirmware, *pnVerLen);

   return SUCCESS;
}

/*==============================================================
Function: OEM_GetUpdateVersion

Description: Returns the update version information.

==============================================================*/
int OEM_GetUpdateVersion(char *pszVersion, int *pnLen)
{
   int nUpdateVerLen = 0;
   FILE * fptr = NULL;
   char strLine[200]; 
   char * pStart = NULL;
   char seps[] = "\""; 
   TCHAR szPathName [_MAX_PATH];
   TCHAR szCurDir [_MAX_PATH];
   boolean bCurDirChanged = FALSE;
   PTSTR pszSlash;

   if(0 == pnLen)
   {
      return EFAILED;
   }

   // Get the directory in which <BREWSimulator.exe> is present
   if (0 == GetModuleFileName( 0, szPathName, MAX_PATH ))
   {
      goto fail;
   }
    pszSlash = _tcsrchr( szPathName, _T('\\') );
    if (!pszSlash)
    {
       goto fail;
    }
    else
    {
       //remove the "\BREWSimulator.exe" from the path.
       _tcscpy( pszSlash, _T("\0"));
    }

   //remember the current directory
   if (0 == GetCurrentDirectory(_MAX_PATH,szCurDir))
   {
      goto fail;
   }
   if (FALSE == SetCurrentDirectory(szPathName))
   {
      goto fail;
   }
   bCurDirChanged = TRUE;

   //NOTE: use this path below, if you are stepping in Visual Studio debugger
   //fptr = fopen("..\\..\\..\\sdk\\inc\\BREWVersion.h","r");
   fptr = fopen("..\\..\\sdk\\inc\\BREWVersion.h","r");
   if (NULL == fptr)
   {
      goto fail;
      
   }

   while (NULL != fgets(strLine,200,fptr))
   {
      if (NULL != STRSTR(strLine,"BREW_UPDATE_VERSION"))
      {
         //found BREW_UPDATE_VERSION, now get the update string
         //if we did not find a string that is "" at least, we have an error
         pStart = strtok(strLine,seps);
         if (NULL == pStart)
         {
            goto fail;
         }
         pStart = strtok(NULL,seps);
         if ((NULL == pStart))
         {
            goto fail;
         }
         if (0x0A == *pStart) //handle line feed, fgets will return line feed for empty string ""
         {
            nUpdateVerLen = 0;
         }
         else
         {
            nUpdateVerLen = STRLEN(pStart);
         }
        
         if (nUpdateVerLen >= 200 ) //local stack buffer limit check - to account for null termination, we do >=200
         {
            goto fail;
         }
         //found BREW_UPDATE_VERSION, so break
         break;
      }
   }
  
   if (0 == pszVersion)
   {
      *pnLen = nUpdateVerLen; // not include null termination.
      goto success;
   }
   
   if(*pnLen < nUpdateVerLen)
   {
      *pnLen = nUpdateVerLen; // not include null termination.
      *pszVersion = 0;
      goto fail;
   }
   STRNCPY(pszVersion,pStart,nUpdateVerLen);
   *(pszVersion+nUpdateVerLen) = '\0'; //null terminating the string
   *pnLen = nUpdateVerLen;  // not include null termination.
   goto success;

success:
   if (fptr)
   {
      fclose(fptr);
      fptr = NULL;
   }
   if (bCurDirChanged)
   {
      if (0 == SetCurrentDirectory(szCurDir))
      {
         return EFAILED;
      }
   }
   return SUCCESS;

fail:
   if (fptr)
   {
      fclose(fptr);
      fptr = NULL;
   }
   if (bCurDirChanged)
   {
      SetCurrentDirectory(szCurDir);
   }
   return EFAILED;
}


// Support for persisting config items to a file
// Copied from pkdev/surf/src/msm/OEMSVC.c

static int OEM_GetBrewConfig(AEEConfigItem id, void *buffer, int len)
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
   // Set the default values
   switch (id) {
      case CFGI_THEMENAME:
         STRLCPY((char *)buffer, "trig.bar", len);
         break;
      case CFGI_SUBLCD_PROPS:
         // FGCOLOR,RGB,BGCOLOR,RGB,OUTLINECOLOR,RGB,OUTLINEWIDTH,RGB
         STRLCPY((char *)buffer, "320,255,304,0,791,255,790,1", len);
         break;
      case CFGI_ANNUN_PROPS:
         // FGCOLOR,RGB,BGCOLOR,RGB
         STRLCPY((char *)buffer, "320,255,304,16777215", len);
         break;
      case CFGI_THEMES_INI:
          STRLCPY((char *)buffer, "themes.ini", len);
          break;
      case CFGI_WALLPAPERS_INI:
          STRLCPY((char *)buffer, "wallpapers.ini", len);
          break;
      case CFGI_RINGTONES_INI:
          STRLCPY((char *)buffer, "ringtones.ini", len);
          break;
      default:
          *((char*) buffer) = 0;
          break;
   }

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
               case CFGI_THEMENAME:
                  if ((pBuf = STRISTR(pRead, BREW_THEMENAME)) != NULL) {
                     pBuf += BREW_THEMENAME_LEN;
                  }
                  break;
               case CFGI_SUBLCD_PROPS:
                  if ((pBuf = STRISTR(pRead, BREW_SUBLCDPROPS)) != NULL) {
                     pBuf += BREW_SUBLCDPROPS_LEN;
                  }
                  break;
               case CFGI_ANNUN_PROPS:
                  if ((pBuf = STRISTR(pRead, BREW_ANNUNPROPS)) != NULL) {
                     pBuf += BREW_ANNUNPROPS_LEN;
                  }
                  break;
               case CFGI_THEMES_INI:
                  if ((pBuf = STRISTR(pRead, BREW_THEMES_INI)) != NULL) {
                     pBuf += BREW_THEMES_INI_LEN;
                  }
                  break;
               case CFGI_WALLPAPERS_INI:
                  if ((pBuf = STRISTR(pRead, BREW_WALLPAPERS_INI)) != NULL) {
                     pBuf += BREW_WALLPAPERS_INI_LEN;
                  }
                  break;
               case CFGI_RINGTONES_INI:
                  if ((pBuf = STRISTR(pRead, BREW_RINGTONES_INI)) != NULL) {
                     pBuf += BREW_RINGTONES_INI_LEN;
                  }
                  break;
               case CFGI_OEMAUTH_CHALLENGE_CAP:
                  if ((pBuf = STRISTR(pRead, OEM_AUTH_CR_CAP)) != NULL) {
                     pBuf += OEM_AUTH_CR_CAP_LEN;
                  }
                  break;
               case CFGI_OEMAUTH_CHALLENGE_RESPONSE_LEN:
                  if ((pBuf = STRISTR(pRead, OEM_AUTH_RESPLEN)) != NULL) {
                     pBuf += OEM_AUTH_RESPLEN_LEN;
                  }
                  break;

               case CFGI_OEMAUTH_RUIM_CARDID:
                  if((pBuf = STRISTR(pRead, OEM_AUTH_RUIM_CARDID)) != NULL) {
                     pBuf += OEM_AUTH_RUIM_CARDID_LEN;
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

static int OEM_SetBrewConfig(AEEConfigItem id, void *pBuffer, int len)
{
   char       *pDelimiter;
   ACONTEXT   *pacRestore;
   char       *pBuf  = NULL;
   char       *pRead = NULL;
   uint32      bytesLeft;
   uint32      bytesRead = 0, nRead;
   AEEFileInfo fileinfo;
   IFileMgr   *piFileMgr = NULL;
   IFile      *piFile = NULL;
   char        buffer[256];
   int         nErr = EFAILED;
   IShell     *piShell = AEE_GetShell();

   if ((pBuffer == NULL) || (len <= 0) || (piShell == NULL))
      return EBADPARM;

   switch (id) {
      case CFGI_THEMENAME:
         SNPRINTF(buffer, sizeof(buffer), "%s%s\r\n", BREW_THEMENAME, (char*)pBuffer);
         break;
      case CFGI_SUBLCD_PROPS:
      case CFGI_ANNUN_PROPS:
      {
         int  count = 0;
         char szProp[100];
         if (id == CFGI_SUBLCD_PROPS) {
            SNPRINTF(buffer, sizeof(buffer), "%s", BREW_SUBLCDPROPS);
         } else {
            SNPRINTF(buffer, sizeof(buffer), "%s", BREW_ANNUNPROPS);
         }
         len /= sizeof(int);
         if (len >= 2) {
            SNPRINTF(szProp, sizeof(szProp), "%lu,%lu", ((int*)pBuffer)[count], ((int*)pBuffer)[count+1]);
            STRCAT(buffer, szProp);
            count += 2;
         }
         while (count+1 < len) {
            SNPRINTF(szProp, sizeof(szProp), ",%lu,%lu", ((int*)pBuffer)[count], ((int*)pBuffer)[count+1]);
            STRCAT(buffer, szProp);
            count += 2;
         }
         STRCAT(buffer, "\r\n");
         break;
      }
      case CFGI_THEMES_INI:
         SNPRINTF(buffer, sizeof(buffer), "%s%s\r\n", BREW_THEMES_INI, (char*)pBuffer);
         break;
      case CFGI_WALLPAPERS_INI:
         SNPRINTF(buffer, sizeof(buffer), "%s%s\r\n", BREW_WALLPAPERS_INI, (char*)pBuffer);
         break;
      case CFGI_RINGTONES_INI:
         SNPRINTF(buffer, sizeof(buffer), "%s%s\r\n", BREW_RINGTONES_INI, (char*)pBuffer);
         break;
   }

   pacRestore = AEE_EnterAppContext(NULL);
   ISHELL_CreateInstance(piShell, AEECLSID_FILEMGR, (void**)&piFileMgr);
   if (piFileMgr) {
      MEMSET(&fileinfo, 0, sizeof(fileinfo));
      piFile = IFILEMGR_OpenFile(piFileMgr, BREW_CONFIG_FILE, _OFM_READWRITE);

      if (piFile) {
         IFILE_GetInfo(piFile, &fileinfo);
      } else {
         piFile = IFILEMGR_OpenFile(piFileMgr, BREW_CONFIG_FILE, _OFM_CREATE);
      }

      if (piFile) {
         nErr = SUCCESS;
         if (fileinfo.dwSize) {
            pRead = (char *)MALLOC(fileinfo.dwSize); 
            if (pRead != NULL) { 
               bytesLeft = fileinfo.dwSize; 

               while (bytesLeft > 0) { 
                  nRead = IFILE_Read(piFile, (void *)(pRead + bytesRead), bytesLeft); 

                  if (nRead == 0) { 
                     break;
                  }

                  bytesRead += nRead;
                  bytesLeft -= nRead;
               }
            }

            IFILE_Truncate(piFile, 0);
            switch (id) {
               case CFGI_THEMENAME:
                  pBuf = STRISTR(pRead, BREW_THEMENAME);
                  break;
               case CFGI_SUBLCD_PROPS:
                  pBuf = STRISTR(pRead, BREW_SUBLCDPROPS);
                  break;
               case CFGI_ANNUN_PROPS:
                  pBuf = STRISTR(pRead, BREW_ANNUNPROPS);
                  break;
               case CFGI_THEMES_INI:
                  pBuf = STRISTR(pRead, BREW_THEMES_INI);
                  break;
               case CFGI_WALLPAPERS_INI:
                  pBuf = STRISTR(pRead, BREW_WALLPAPERS_INI);
                  break;
               case CFGI_RINGTONES_INI:
                  pBuf = STRISTR(pRead, BREW_RINGTONES_INI);
                  break;
            }

            if (pBuf) {
               IFILE_Write(piFile, pRead, pBuf - pRead);
               IFILE_Write(piFile, buffer, STRLEN(buffer));
               pDelimiter = STRCHREND(pBuf, BREW_CONFIG_DELIMITER);
               if (pDelimiter) {
                  IFILE_Write(piFile, pDelimiter + 1, (pRead + fileinfo.dwSize) - (pDelimiter + 1));
               } 
            } else {
               IFILE_Write(piFile, pRead, fileinfo.dwSize);
               IFILE_Write(piFile, buffer, STRLEN(buffer));
            }
         } else {
            IFILE_Write(piFile, buffer, STRLEN(buffer));
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
   return nErr;
}
