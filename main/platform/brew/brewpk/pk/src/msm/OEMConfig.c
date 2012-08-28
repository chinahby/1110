/*===========================================================================
                              OEMConfig.c

  OEM REFERENCE APPLICATION CONTROL ROUTINES

  This file contains a reference implementation of a standard AEE application
  control interfaces.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "OEMFeatures.h"
#include "comdef.h"
#include "AEECLSID_SIM_CONFIGEXTENDER.BID"

#include "target.h"
#include "customer.h"
#include "OEMNV.h"

#include "OEMDisp.h"
#include "OEMFS.h"
#ifdef FEATURE_DRV_SDCC
#include "sdcc_api.h"
#endif //FEATURE_DRV_SDCC

// Include pdapi.h for CGPS
#ifdef FEATURE_CGPS
#include "pdapi.h"
#elif defined( FEATURE_GPSONE ) || defined( FEATURE_PDAPI )
#include "pdsm.h"
#endif // FEATURE_CGPS

#include "voc.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#include "ghdi_exp.h"
#endif

#if   defined(_AEEWIN)
#   define  PLATFORM_NAME  "AEEWIN"
#else
#   define  PLATFORM_NAME  "Simulator"
#endif

#if defined(OEMCONFIG)

#include "OEMConfig.h"
#ifdef FEATURE_UIONE_HDK
#include "OEMDeviceInfoEx_impl.h"
#endif // FEATURE_UIONE_HDK
#include "OEMSVC.h"
#include "AEE_OEM.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "AEEDownload.h"
#include "AEEDeviceNotifier.h"
#include "ui.h"
#ifdef USES_MMI
#include "mobile.h"
#endif

#include "nv.h"
#include "nvi.h"

#ifdef FEATURE_UIM_RUIM
#include "nvruimi.h"
extern nvruim_esn_usage_type nvruim_esn_usage;
#endif

#include "AEEGROUPID_USBHMS.bid"

#include "msg.h"
#if (defined(T_MSM6550) && defined(T_SURF)) || defined(T_FFA6550_SS) || (defined(T_MSM6280) && defined(T_SURF))
#include "hs.h"
#endif /* (defined(T_MSM6550) && defined(T_SURF)) || defined(T_FFA6550_SS)  || (defined(T_MSM6280) && defined(T_SURF)) */

#ifdef FEATURE_QAUDIOFX
#include "DEVICEITEM_AUD3D_MAX_ENVIRON.BID"
#endif

extern int OEMDisp_GetDispInfo(AEEDeviceInfo * pi);


extern int OEMDisp_GetDispInfo(AEEDeviceInfo * pi);

uint32 GetLngCode(void);
int SetLngCode(uint32 dwLngCode);
static uint32 CachedLngCode = 0;

static int GetNVItem(nv_items_enum_type item, nv_item_type *data_ptr);

extern boolean OEMSound_IsVibratorON(void);

// color table, returned from OEM_GetDeviceInfoEx (AEE_DEVICEITEM_SYS_COLORS_DISP1)
#define RGB_LIGHT_GREY  (MAKE_RGB(0xC0, 0xC0, 0xC0))
#define RGB_DARK_GREY   (MAKE_RGB(0x80, 0x80, 0x80))
static const RGBVAL gSysRGBs[CLR_SYS_LAST] = {
   0,
   RGB_BLACK,     // CLR_USER_TEXT
   RGB_WHITE,     // CLR_USER_BACKGROUND
   RGB_BLACK,     // CLR_USER_LINE
   RGB_WHITE,     // CLR_SYS_TITLE
   RGB_BLACK,     // CLR_SYS_TITLE_TEXT
   RGB_LIGHT_GREY,// CLR_SYS_ITEM
   RGB_BLACK,     // CLR_SYS_ITEM_TEXT
   RGB_LIGHT_GREY,// CLR_SYS_ITEM_SEL
   RGB_BLACK,     // CLR_SYS_ITEM_SEL_TEXT
   RGB_WHITE,     // CLR_SYS_WIN
   RGB_WHITE,     // CLR_SYS_FRAME_HI
   RGB_BLACK,     // CLR_SYS_FRAME_LO
   RGB_LIGHT_GREY,// CLR_SYS_LT_SHADOW
   RGB_DARK_GREY, // CLR_SYS_DK_SHADOW
   RGB_WHITE,     // CLR_SYS_SCROLLBAR
   RGB_BLACK      // CLR_SYS_SCROLLBAR_FILL
};


/* gCameraClassIDs is a list of Class IDs to be returned for
   OEM_GetDeviceInfoEx(AEE_DEVICEITEM_CAMERA_INFO) */
static const uint32 gCameraClassIDs[] =
{
#ifdef FEATURE_ICAMERA
   AEECLSID_CAMERA,
#endif
   NULL
};

// Callback to be invoked on change in the value of a configuration item
static AEECallback * gpcbOnItemUpdate;

static IConfig *spIConfigExtender = NULL;
static void CustomOEMConfig_ReleaseConfigExtender(void *unused)
{
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
   IShell *pIShell = NULL;
   ACONTEXT *pCtxOld = NULL;

   if (spIConfigExtender != NULL) {
      return spIConfigExtender; // No addref
   }
   if (AEE_IsInitialized()) {
      pIShell = AEE_GetShell();
      if (pIShell != NULL) {
         static boolean bReentered = FALSE;
         // Create the extender in the system context to
         // avoid app-switching issues
         pCtxOld = AEE_EnterAppContext(NULL);
         // Protect against reentering this function, this can be caused by
         //   ISHELL_CreateInstance since this function may call OEM_GetDeviceInfo.
         if (!bReentered) {
            bReentered = TRUE;
            if (SUCCESS == ISHELL_CreateInstance(pIShell, AEECLSID_SIM_CONFIGEXTENDER, (void **) &spIConfigExtender)) {
               static AEECallback sBREWShutDownCallback;
               CALLBACK_Init(&sBREWShutDownCallback, CustomOEMConfig_ReleaseConfigExtender, NULL);
               AEE_RegisterSystemCallback(&sBREWShutDownCallback, AEE_SCB_AEE_EXIT, 0);
            }
            bReentered = FALSE;
         }
         // Restore the original app context.
         AEE_LeaveAppContext(pCtxOld);
      }
   }
   return spIConfigExtender; // No addref
}
/*==================================================================
Function: OEM_GetConfig

Description: Retrieves configuration information from the OEM

Prototype:
   int  OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
int OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize)
{
   IConfig *pIConfigExtender = NULL;
   int nErr;

   /* First try to delegate the IConfig item to the extender */
   pIConfigExtender = CustomOEMConfig_GetConfigExtender();
   if (pIConfigExtender != NULL) {
      nErr = ICONFIG_GetItem(pIConfigExtender, i, pBuff, nSize);
      pIConfigExtender = NULL;  // No release required
      if (EUNSUPPORTED != nErr) {
         return nErr;
      }
   }
   return OEM_SVCGetConfig(i, pBuff, nSize);
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
   IConfig *pIConfigExtender = NULL;
   int nErr = EUNSUPPORTED;

   /* First try to delegate the IConfig item to the extender */
   pIConfigExtender = CustomOEMConfig_GetConfigExtender();
   if (pIConfigExtender != NULL) {
      nErr = ICONFIG_SetItem(pIConfigExtender, i, pBuff, nSize);
      pIConfigExtender = NULL;  // No release required
   }
   if (EUNSUPPORTED == nErr) {
      nErr = OEM_SVCSetConfig(i, pBuff, nSize);
   }

   if (SUCCESS == nErr)
   {
      IShell * pIShell = AEE_GetShell();
      ISHELL_Notify(pIShell, AEECLSID_DEVICENOTIFIER, NMASK_DEVICENOTIFIER_SET_CONFIG, (void*)i);

      if (gpcbOnItemUpdate)
      {
         *(uint32*)gpcbOnItemUpdate->pReserved = i;
         AEE_SYS_RESUME(gpcbOnItemUpdate);
      }
   }

   return nErr;
}

/*==================================================================
Function: OEM_GetDeviceInfo

Description: Retrieves the device information from the OEM

Prototype:
   void  OEM_GetDeviceInfo(AEEDeviceInfo * pi, RGBVAL * pSysColors, int nRGB);

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
#ifdef FEATURE_DYNAMIC_OEM_HEAP
extern uint32 guHeapBytesLen;
#endif
void  OEM_GetDeviceInfo(AEEDeviceInfo * pi)
{
   if(pi){
      AEEDownloadInfo sDownloadInfo;

      MEMSET(pi,0, sizeof(AEEDeviceInfo));

      if (SUCCESS == OEMDisp_GetDispInfo(pi))   /* get info from display device */
      {
      pi->cxScrollBar      = 3;
      pi->dwPromptProps    = CP_3D_BORDER|ST_MIDDLETEXT|ST_UNDERLINE;
#ifdef FEATURE_SECONDARY_DISPLAY
         pi->bAltDisplay      = TRUE;
#else
      pi->bAltDisplay      = FALSE;
      pi->cxAltScreen      = 0;
      pi->cyAltScreen      = 0;
#endif
      pi->wKeyCloseApp     = AVK_CLR;
      pi->wKeyCloseAllApps = AVK_END;

#if defined(AEE_SIMULATOR)
      pi->dwPlatformID     = DEFAULT_PLATFORM_ID;
#else
      pi->wStructSize = sizeof(AEEDeviceInfo);
      if (SUCCESS == OEM_GetConfig(CFGI_DOWNLOAD, (void *)&sDownloadInfo, sizeof(AEEDownloadInfo))) {
         pi->dwPlatformID = sDownloadInfo.dwPlatformID;
      }
      else {
         pi->dwPlatformID     = 600;
      }
      pi->wEncoding = AEE_ENC_UNICODE;
#if defined(T_MSM7500)
         pi->dwSleepDefer = 500;
#else
         pi->dwSleepDefer = 15000;
#endif
#endif
      // Set Language Support...
      pi->dwLang = GetLngCode();
      pi->wMaxPath = (uint16)OEMFS_GetMaxPathLength();

#ifdef FEATURE_DYNAMIC_OEM_HEAP
      pi->dwRAM = guHeapBytesLen;
#else
      pi->dwRAM = OEM_STATIC_HEAP_SIZE;
#endif

#if defined(FEATURE_AUDIO_FORMAT)
      pi->bMIDI = TRUE;
#endif
#if !defined(FEATURE_PEKTEST)
#if defined(FEATURE_CMX)
      pi->bCMX = TRUE;
#endif
#endif
#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
      pi->bPosLoc = TRUE;
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
   }
      else
      {
         // If we couldn't get display info, log an error
         MSG_HIGH("Could not obtain display info.", 0, 0, 0);
      }
   }
}

/*===========================================================================
Function: GetNVItem

Description:
  Get an item from the nonvolatile memory.  Wait until item fetched.


Prototype:
   int GetNVItem(nv_items_enum_type item, nv_item_type *data_ptr);

Parameter(s):
   item - the nv item type.
   data_ptr - pointer to storage area for item.

Return Value:  The NV return code.
Comments:      None
Side Effects:  None
See Also:      None

===========================================================================*/
static int GetNVItem(nv_items_enum_type item, nv_item_type *data_ptr)
{
   nv_cmd_type  cmd;                /* nv command buffer */
   rex_sigs_type sigs;              /* hold signals from rex_wait() */

   cmd.tcb_ptr = rex_self();
   cmd.done_q_ptr = NULL;
   cmd.sigs = UI_NV_SIG;
   cmd.item = item;
   cmd.cmd  = NV_READ_F;
   cmd.data_ptr = data_ptr;

   (void) rex_clr_sigs( rex_self(), UI_NV_SIG );
   nv_cmd( &cmd );

   for( ;; )
   {                     /* until signal back from NV */
     sigs = rex_wait(UI_NV_SIG);

     /* The NV is done.  Exit this routine. */
     if( sigs & UI_NV_SIG )
     {
        return cmd.status;
     }
   }
}

int OEM_ReadESN(uint32 *pESN)
{
   nv_stat_enum_type status;
#ifdef FEATURE_UIM_RUIM
   nvruim_esn_usage_type nv_esn_type;

   // save current state (a global variable)
   nv_esn_type = nvruim_get_esn_usage();
   nvruim_set_esn_usage(NV_RUIM_USE_ESN);
#endif /* FEATURE_UIM_RUIM */

   status = GetNVItem(NV_ESN_I, (nv_item_type *)pESN);

#ifdef FEATURE_UIM_RUIM
   nvruim_set_esn_usage(nv_esn_type);
#endif /* FEATURE_UIM_RUIM */

   // shouldn't fail, but just in case...
   if ( NV_DONE_S != status) {
      *pESN = 0;
      return EFAILED;
   }
   else
      return SUCCESS;

}  /* OEM_ReadESN */

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
#define IMEI_SIZE ((NV_UE_IMEI_SIZE-1)*2)

static int OEM_ReadIMEIS(byte *  imei_ptr)
{
   ghdi_status_T status;
   byte imei_bcd_len = 0;
   int bcd_index = 0;
   int ascii_index = 0;
   byte asc_1 = 0;
   byte asc_2 = 0;
   byte imei[IMEI_SIZE];

   /* This is a valid IMEI */
   memset(imei, 0, IMEI_SIZE);

   status = ghdi_read_imei(imei_ptr);
   if (status != GHDI_SUCCESS)
      return -1;

   /* The first byte of NV ue_imei is the length of IMEI BCDs */
   imei_bcd_len = imei_ptr[0];

   if (imei_bcd_len == 0)
   {
      return 0;
   }

   if( imei_bcd_len < NV_UE_IMEI_SIZE )
   {
      int checkSum = 0;
      int checkDigit = 0;

      /*
      ** THIS IS A VALID IMEI, SO CONVERT IT TO ASCII
      ** The 1st byte lower nybble == 0x0A, upper nybble is
      ** 1st digit of ID.  Will convert it all anyway & then
      ** drop 1st character later.
      */
      for( bcd_index = 1; bcd_index <= (int) imei_bcd_len; bcd_index++ )
      {
         asc_1 = (imei_ptr[bcd_index] & 0x0F);
         asc_2 = (imei_ptr[bcd_index] & 0xF0) >> 4;
         imei[ascii_index++] = (asc_1 <= 9)?asc_1+'0'
                                                 :asc_1-10+'a';
         imei[ascii_index++] = (asc_2 <= 9)?asc_2+'0'
                                                 :asc_2-10+'a';

         ////
         // Calculate the check-sum used to calculate the check-digit
         // Digits are numbered starting with the least significant digit
         if (bcd_index != 1) // The 1st char is not the imei so don't use it
         {
            // For the odd digits,double their value and add their digits together
            int doubledDigit = 2 * asc_1;

            //checkSum += (doubledDigit / 10) + (doubledDigit % 10);
            if (doubledDigit >= 10) {
               checkSum += 1 + (doubledDigit - 10); // Example '13' becomes 1 + 3
            } else {
               checkSum += doubledDigit;
            }
         }
         // For the even digits, add their values into the check-sum
         checkSum += asc_2;
      }

      ////
      // Calculate the check-digit
      // If the checkSum ends in zero, then zero is the check-digit
      if ((checkSum % 10) == 0)
      {
         checkDigit = 0;
      }
      // Otherwise, subtract checkSum from the next highest multiple of 10
      else
      {
         checkDigit = 10 - (checkSum % 10);
      }

      // The last digit of the IMEI is the check-digit
      imei[IMEI_SIZE-1] = checkDigit + '0';

      if(ascii_index > 2)
      {
         memcpy( imei_ptr, imei + 1, (uint32) (ascii_index - 1) );
      }

      return (ascii_index - 1);
   } //  if( imei_bcd_len < NV_UE_IME...
   else
   {
      return 0;
   }
} //OEM_ReadIMEI
#endif

#if defined(FEATURE_MEID_SUPPORT)

int OEM_ReadMEID(uint64 *meid)
{
   nv_stat_enum_type status;

   status = GetNVItem(NV_MEID_I, (nv_item_type *)meid);
   // not available?
   if ( NV_DONE_S != status)
      return EFAILED;
   else
      return SUCCESS;

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
      asc = (meid >> ((MEID_SIZE-2 - index)*4)) & 0xf;
      meid_ptr[index] = (asc <= 9) ? asc+'0' : asc-10+'a';
   }
   meid_ptr[MEID_SIZE-1] = 0;

   return MEID_SIZE;
}
#endif

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
#if defined(FEATURE_MEID_SUPPORT)
   {
      uint64 meid;
      void *vptr;
      if (SUCCESS == OEM_ReadMEID(&meid)) {
         // length calculation is equivalent to structure_size - 4 + (sizeof(meid)/4)*4
         len = sizeof(AEEHWIDType) + ((sizeof(meid)-1) & 0xFFFC);
         listLength += len;
         // big enough?
         if (pList && *pwLength >= listLength) {
            pHWID->uLength       = len;
            pHWID->bHWIDLength   = sizeof(meid);
            pHWID->bHWIDType      = AEE_HWID_MEID;
//lint -save -e419
            *(&vptr) = (void *)(pHWID->HWID);
            memcpy( (byte *)vptr, (byte *)&meid, sizeof(meid));
//lint -restore

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
#endif

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
   {
      byte imei[NV_UE_IMEI_SIZE];
      uint32 imei_bcd_len;
      if (GHDI_SUCCESS == ghdi_read_imei(imei)) {
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
#endif

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

/*===========================================================================

===========================================================================*/
static boolean OEM_CompressNumber(char * pszSrc)
{
   char *   pszDest;
   char     ch;
   int      i;

   // Compress out any garbage...

   for(i = 0, pszDest = pszSrc; (ch = *pszSrc) != 0; pszSrc++){
      if((ch == '*' || ch == '#') || (ch >= '0' && ch <= '9')){
         *(pszDest++) = ch;
         i++;
      }
   }
   *pszDest = 0;

   return (boolean)(i > 0);
}

/*=======================================================================
Function: OEM_GetDeviceInfoEx()

Description:
   This method is used to get specific information about the device. This function takes
   an ID that specifies what information is needed. The buffer contains the corresponding
   information on return.

Prototype:

   int   OEM_GetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int *pnSize);

Parameters:
   nItem: Specifies the Item whose info is needed. Please see documentation of
   AEEDeviceItem for the supported Device Items.
   pBuff: Contains the corresponding information on return
   pnSize: Contains the size of pBuff. On return, it contains the size filled. This parameter may
   be NULL for certain device Items.

Return Value:
   SUCCESS: If successful
   EBADPARM: if bad parameters are passed in
   EUNSUPPORTED: If this item is not supported
   EFAILED: An unspecified error occurred

Comments:
   None

Side Effects:
   None

See Also:
   AEEDeviceItem

=======================================================================*/
int   OEM_GetDeviceInfoEx(AEEDeviceItem nItem, void *pBuff, int *pnSize)
{
#ifdef FEATURE_UIONE_HDK
   if (SUCCESS == OEM_GetDeviceInfoEx_Extra(nItem,pBuff,pnSize)) {
      return SUCCESS;
   }
#endif // FEATURE_UIONE_HDK
   switch(nItem)
   {
   case AEE_DEVICEITEM_CHIP_ID:
      {
#if defined(AEE_SIMULATOR)
         char szStr[32];

         if(!pnSize)
            return(EBADPARM);

         if(pBuff && *pnSize) {
            STRCPY(szStr,PLATFORM_NAME);
            STRTOWSTR(szStr,(AECHAR*)pBuff,*pnSize);
         }
         *pnSize = (STRLEN(PLATFORM_NAME)+1)*2;
         return(SUCCESS);
#else
         char szStr[32];

         if(!pnSize)
            return(EBADPARM);

         if(pBuff && *pnSize) {
#if defined T_MSM7500
            #define CHIP_ID "MSM7500"
#elif defined T_QSC1110
            #define CHIP_ID "QSC1110"
#elif defined T_QSC1100
            #define CHIP_ID "QSC1100"
#elif defined T_MSM6280
            #define CHIP_ID "MSM6280"
#elif defined T_MSM6250
            #define CHIP_ID "MSM6250"
#elif defined T_MSM6550
            #define CHIP_ID "MSM6550"
#elif defined T_MSM6500
            #define CHIP_ID "MSM6500"
#elif defined T_MSM6100
            #define CHIP_ID "MSM6100"
#elif defined T_MSM6800
            #define CHIP_ID "MSM6800"
#elif defined T_MSM6275
            #define CHIP_ID "MSM6275"
#elif defined T_QSC6010
            #define CHIP_ID "QSC6010"
#elif defined T_QSC6020
            #define CHIP_ID "MSM6020"
#elif defined T_QSC6030
            #define CHIP_ID "MSM6030"
#endif
            STRCPY(szStr,CHIP_ID);
            STRTOWSTR(szStr,pBuff,*pnSize);
         }
         *pnSize = (STRLEN(CHIP_ID)+1)*2;
         return(SUCCESS);
#endif // defined(AEE_SIMULATOR)
      }

      case AEE_DEVICEITEM_MOBILE_ID:
      {
         AEEMobileInfo mi;

         MEMSET(&mi,0,sizeof(AEEMobileInfo));

         if(!pnSize)
            return(EBADPARM);

         if(!OEM_GetConfig(CFGI_MOBILEINFO,&mi,sizeof(AEEMobileInfo))) {
            OEM_CompressNumber(mi.szMobileID);
            if(pBuff && *pnSize > 0)
               STRLCPY(pBuff,mi.szMobileID,*pnSize);
            *pnSize = STRLEN(mi.szMobileID)+1;
            return(SUCCESS);
         }
         else
            return(EUNSUPPORTED);
      }
      break;

      case AEE_DEVICEITEM_SOFTKEY_COUNT:
      {
         if (!pnSize)
            return(EBADPARM);

         if (!pBuff || *pnSize < sizeof(uint8)) {
            *pnSize = sizeof(uint8);
            return(SUCCESS);
         }

         *((uint8 *)pBuff) = 0;
         return(SUCCESS);
      }


      case AEE_DEVICEITEM_CARRIER_ID:
      {
         AEEDownloadInfo di;

         MEMSET((void *)&di,0,sizeof(AEEDownloadInfo));

         if(!pnSize)
            return(EBADPARM);

         if(!OEM_GetConfig(CFGI_DOWNLOAD,(void*)&di,sizeof(AEEDownloadInfo))) {
            if(pBuff && (*pnSize == sizeof(di.dwCarrierID)))
               *(uint32 *)pBuff = di.dwCarrierID;
            *pnSize = sizeof(di.dwCarrierID);
            return(SUCCESS);
         }
         else
            return(EUNSUPPORTED);
      }

      case AEE_DEVICEITEM_AMR_VOCODER_SUPPORT:      // Is AMR (Adaptive Multi-Rate) Vocoder Supported ?
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEM_SVCIsVocCapable(VOC_CAP_AMR);
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_EVRC_VOCODER_SUPPORT:      // Is EVRC (Enhanced Variable Rate Codec) Vocoder Supported ?
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEM_SVCIsVocCapable(VOC_CAP_IS127); /* EVRC 8K supported?? */
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_IS96_VOCODER_SUPPORT:      // Is QCELP (Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEM_SVCIsVocCapable(VOC_CAP_IS96); /* QCELP 13K supported?? */
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_IS96A_VOCODER_SUPPORT:      // Is QCELP-IS96 (8K)(Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEM_SVCIsVocCapable(VOC_CAP_IS96A ); /* QCELP IS-96A (8K) supported?? */
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

      case AEE_DEVICEITEM_IS733_VOCODER_SUPPORT:      // Is QCELP (Qualcomm Code Excited Linear Predictive Coding) Vocoder Supported ?
      {
         if(!pnSize)
            return(EBADPARM);

         if(!pBuff || *pnSize < sizeof(boolean)) {
            *pnSize = sizeof(boolean);
            return(SUCCESS);
         }

         *((boolean *)pBuff) = OEM_SVCIsVocCapable(VOC_CAP_IS733); /* QCELP 13K supported?? */
         *pnSize = sizeof(boolean);
         return(SUCCESS);
      }

   case AEE_DEVICEITEM_SYS_COLORS_DISP1:
#ifdef FEATURE_SECONDARY_DISPLAY
   case AEE_DEVICEITEM_SYS_COLORS_DISP2:
#endif
      {
         int nSize;

         if(!pnSize)
            return(EBADPARM);

         nSize = (int)MIN((uint32)*pnSize, CLR_SYS_LAST * sizeof(RGBVAL));
         *pnSize = CLR_SYS_LAST * sizeof(RGBVAL);

         if (NULL == pBuff) {
            return SUCCESS;
         }

//lint -save -e611 -e740 Suppress "suspicious pointer cast" errors
         MEMCPY(pBuff, gSysRGBs, (uint32)nSize);
//lint +e611 +e740

         return SUCCESS;
      }

   case AEE_DEVICEITEM_DISPINFO1:
      {
         AEEBitmapInfo  bi;
         AEEDeviceInfo  di;
         int            nSize;

         if(!pnSize)
            return(EBADPARM);

         nSize = MIN(*pnSize, (int)sizeof(AEEBitmapInfo));
         *pnSize = sizeof(AEEBitmapInfo);

         if (!pBuff) {
            return(SUCCESS);
         }

         if (SUCCESS != OEMDisp_GetDispInfo(&di))
         {
            MSG_HIGH("Could not obtain display info for %d query.",
                     nItem, 0, 0);
            return EFAILED;
         }
         else
         {
           bi.cx = di.cxScreen;
           bi.cy = di.cyScreen;
           bi.nDepth = di.nColorDepth;
         }

//lint -save -e611 -e740  Suppress complaint about pointer casts
         MEMCPY(pBuff, &bi, (uint32)nSize);
//lint -restore

         return SUCCESS;
      }

#ifdef FEATURE_SECONDARY_DISPLAY
   case AEE_DEVICEITEM_DISPINFO2:
      {
         AEEBitmapInfo  bi;
         AEEDeviceInfo  di;
         int            nSize;

         if(!pnSize)
            return(EBADPARM);

         nSize = MIN(*pnSize, (int)sizeof(AEEBitmapInfo));
         *pnSize = sizeof(AEEBitmapInfo);

         if (NULL == pBuff) {
            return(SUCCESS);
         }

         if (SUCCESS != OEMDisp_GetDispInfo(&di))
         {
            MSG_HIGH("Could not obtain display info for %d query.",
                     nItem, 0, 0);
            return EFAILED;
         }
         else
         {
           bi.cx = di.cxAltScreen;
           bi.cy = di.cyAltScreen;
           bi.nDepth = di.nColorDepth;
         }

//lint -save -e611 -e740  Suppress complaint about pointer casts
         MEMCPY(pBuff, &bi, (uint32)nSize);
//lint -restore

         return SUCCESS;
      }
#endif // FEATURE_SECONDARY_DISPLAY

   case AEE_DEVICEITEM_MANNER_MODE:
   {
      if (!pnSize)
         return EBADPARM;

      if (!pBuff || *pnSize < sizeof(uint32))
      {
         *pnSize = sizeof(uint32);
         return(SUCCESS);
      }

      // NOTE to OEMs: OEMs need to set their "global"
      // manner mode flag here in terms of AEE_MANNER_MODE_XXX.
      *((uint32 *)pBuff) = (uint32)AEE_MANNER_MODE_NORMAL;
      *pnSize = sizeof(uint32);
      return SUCCESS;
   }

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   case AEE_DEVICEITEM_IMEI:
   {
      byte szIMEI[IMEI_SIZE];
      int size = OEM_ReadIMEIS(szIMEI);
      szIMEI[IMEI_SIZE-1] = 0;
      if (size <= 0) {
         return EUNSUPPORTED;
      }
      if (!pnSize) {
         return(EBADPARM);
      }
      if (pBuff && *pnSize >= IMEI_SIZE) {
         STRNCPY((char*)pBuff, (char*)szIMEI, *pnSize);
      }
      *pnSize = IMEI_SIZE;
      return(SUCCESS);
   }
#endif

#if defined(FEATURE_MEID_SUPPORT)
   case AEE_DEVICEITEM_MEIDS:
   {
      byte szMEID[MEID_SIZE];
      int size = OEM_ReadMEIDS(szMEID);
      szMEID[MEID_SIZE-1] = 0;
      if (size <= 0) {
         return EUNSUPPORTED;
      }
      if (!pnSize) {
         return(EBADPARM);
      }
      if (pBuff && *pnSize >= MEID_SIZE) {
         STRNCPY((char*)pBuff, (char*)szMEID, *pnSize);
      }
      *pnSize = MEID_SIZE;
      return(SUCCESS);
   }
#endif

   case AEE_DEVICEITEM_HWID:
      if (!pnSize) {
         return(EBADPARM);
      }
      return OEM_GetHWIDList((byte *)pBuff, pnSize, TRUE);
      break;

   case AEE_DEVICEITEM_HWIDLIST:
      if (!pnSize) {
         return(EBADPARM);
      }
      return OEM_GetHWIDList((byte *)pBuff, pnSize, FALSE);
      break;
      
   case AEE_DEVICEITEM_MANUFACTURER_NAME:
      if (!pnSize) 
      {
         return(EBADPARM);
      }
      if (!pBuff || *pnSize< (STRLEN("WATERWORLD INC.")+1)*sizeof(AECHAR)) 
      {
         *pnSize = (STRLEN("WATERWORLD INC.")+1)*sizeof(AECHAR);
         return(EBUFFERTOOSMALL);
      }
      
      (void)STRTOWSTR("WATERWORLD INC.", (AECHAR *)pBuff, *pnSize);
      return SUCCESS;
   
   case AEE_DEVICEITEM_MODEL_NAME:
      if (!pnSize) 
      {
         return(EBADPARM);
      }
      if (!pBuff || *pnSize< (STRLEN(ver_modelname)+1)*sizeof(AECHAR)) 
      {
         *pnSize = (STRLEN(ver_modelname)+1)*sizeof(AECHAR);
         return(EBUFFERTOOSMALL);
      }
      
      (void)STRTOWSTR(ver_modelname, (AECHAR *)pBuff, *pnSize);
      return SUCCESS;
   
   case AEE_DEVICEITEM_FRIENDLY_NAME:
      if (!pnSize) 
      {
         return(EBADPARM);
      }
      if (!pBuff || *pnSize< (STRLEN(OEMNV_DEFAULT_BANNER)+1)*sizeof(AECHAR)) 
      {
         *pnSize = (STRLEN(OEMNV_DEFAULT_BANNER)+1)*sizeof(AECHAR);
         return(EBUFFERTOOSMALL);
      }
      
      (void)STRTOWSTR(OEMNV_DEFAULT_BANNER, (AECHAR *)pBuff, *pnSize);
      return SUCCESS;
   
   case AEE_DEVICEITEM_MDN:
      {
         AECHAR  wstrBuf[OEMNV_PHONENUMBER_MAXLEN]={0,};
         
         if (!pnSize)
         {
            return(EBADPARM);
         }
         
         if (OEM_GetConfig(CFGI_PHONE_NUMBER, wstrBuf, sizeof(wstrBuf)) != SUCCESS)
         {
            return(EFAILED);
         }
         
         if (!pBuff || *pnSize< WSTRSIZE(wstrBuf)) 
         {
            *pnSize = WSTRSIZE(wstrBuf);
            return(EBUFFERTOOSMALL);
         }
          
         (void)WSTRCPY((AECHAR *)pBuff, wstrBuf);
      }
      return SUCCESS;
   
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
   case AEE_DEVICEITEM_ICCID:
   {
      byte szICCID[32];

      if (!pnSize) {
         return(EBADPARM);
      }

      return(EUNSUPPORTED);// Currently not supported

      MEMSET(szICCID, 0, sizeof(szICCID));

      if (!pBuff || *pnSize < (int)(STRLEN((char*)szICCID) + 1)) {
         *pnSize = STRLEN((char*)szICCID) + 1;
      } else {
         STRNCPY((char*)pBuff, (char*)szICCID, *pnSize);
         *pnSize = (STRLEN((char*)szICCID) + 1);
      }
      return(SUCCESS);
   }
#endif

   case AEE_DEVICEITEM_CAMERA_INFO:
   {
      if(!pnSize)
      {
         return EBADPARM;
      }

      if(!pBuff || *pnSize < sizeof(gCameraClassIDs))
      {
         *pnSize = sizeof(gCameraClassIDs);
      }
      else
      {
         MEMCPY(pBuff, gCameraClassIDs, sizeof(gCameraClassIDs));
         *pnSize = sizeof(gCameraClassIDs);
      }
      return AEE_SUCCESS;
   }

   case AEE_DEVICEITEM_KEY_SUPPORT:
   {
      KeySupportType* pKeySupport;

      if (!pnSize)
         return EBADPARM;

      if (!pBuff || *pnSize < sizeof(KeySupportType))
      {
         *pnSize = sizeof(KeySupportType);
         return(SUCCESS);
      }

      pKeySupport = (KeySupportType*)pBuff;
      switch(pKeySupport->key)
      {
         case AVK_0:
         case AVK_1:
         case AVK_2:
         case AVK_3:
         case AVK_4:
         case AVK_5:
         case AVK_6:
         case AVK_7:
         case AVK_8:
         case AVK_9:
         case AVK_STAR:
         case AVK_POUND:
         case AVK_SEND:
         case AVK_END:
         case AVK_CLR:
         case AVK_SELECT:
         case AVK_UP:
         case AVK_DOWN:
         case AVK_LEFT:
         case AVK_RIGHT:
            pKeySupport->supported = TRUE;
            break;
         default:
            pKeySupport->supported = FALSE;
            break;
      }
      return SUCCESS;
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

#if (defined(T_MSM6550) && defined(T_SURF)) || defined(T_FFA6550_SS) || (defined(T_MSM6280) && defined(T_SURF))
   case AEE_DEVICESTATE_FLIP_OPEN:
   {
      hs_key_type    flipState;           // Clamshell is open/closed
      boolean        bIsFlipOpen;
      int            nSize;               // Bytes to copy

      if(!pnSize)
      {
         return(EBADPARM);
      }

      nSize = MIN(*pnSize, (int)sizeof(boolean));
      *pnSize = sizeof(boolean);

      if (!pBuff) {
         return(SUCCESS);
      }

      flipState = hs_query_clamshell_state();
      bIsFlipOpen = (HS_FLIP_OPEN_K == flipState);
//lint -save -e611 -e740  Suppress complaint about pointer casts
      MEMCPY(pBuff, &bIsFlipOpen, (uint32)nSize);
//lint -restore
      *pnSize = sizeof(boolean);
      return SUCCESS;
   }
#endif // (defined(T_MSM6550) && defined(T_SURF)) || defined(T_FFA6550_SS)

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

#if defined(FEATURE_DRV_SDCC) || defined(FEATURE_MMC)
   case AEE_DEVICEITEM_CARD0_INFO:
   {
      DRV_GEOMETRY_DESC dgd;
      AEECardInfo * pci = (AEECardInfo*)pBuff;
      size_t nSerialSize = 0;
      size_t nModelSize = 0;
      uint32 nErr = SUCCESS;
      char   SerialNumber[32];

      // pnSize must be a valid pointer
      if (!pnSize) {
         return(EBADPARM);
      }

      // If pBuff is NULL, set size, return SUCCESS,
      if (pBuff == NULL) {
         *pnSize = sizeof(AEECardInfo);
         return(SUCCESS);
      }

	  // Return the required size if input buffer is not sufficient
      if (!pBuff || *pnSize < sizeof(AEECardInfo)) {
         *pnSize = sizeof(AEECardInfo);
         return(EBUFFERTOOSMALL);
      }

	  // Read the card info
#ifdef FEATURE_DRV_SDCC
      if(!sdcc_read_serial(0, &dgd)) {
#else
      if(!mmc_read_serial(0, &dgd)) {
#endif //FEATURE_DRV_SDCC
         return(EFAILED);
      }

     // Copy the serial number if its a valid buffer
      if (pci->pszSerialNum && 0 < pci->nSerialNumSize) {

		  nSerialSize = SPRINTF(pci->pszSerialNum, "%d", dgd.serialNum);
                  pci->nSerialNumSize = STRLEN((char*)pci->pszSerialNum) + 1;
      }

      // If the char buffer is invalid or too small, return the required size
      if (!pci->pszSerialNum || (nSerialSize >= pci->nSerialNumSize)) {
		  SPRINTF(SerialNumber, "%d", dgd.serialNum);
          pci->nSerialNumSize = STRLEN((char*)SerialNumber) + 1;
          nErr = EBUFFERTOOSMALL;
      }
	  // Copy the model number if its a valid buffer
      if (pci->pszModelNum && 0 < pci->nModelNumSize) {
          nModelSize = STRLCPY(pci->pszModelNum, (char*)dgd.modelNum, pci->nModelNumSize);
                  pci->nModelNumSize = STRLEN((char*)dgd.modelNum) + 1;
      }

      // If the char buffer is invalid or too small, return the required size
      if (!pci->pszModelNum || (nModelSize >= pci->nModelNumSize)) {
          pci->nModelNumSize = STRLEN((char*)dgd.modelNum) + 1;
          nErr = EBUFFERTOOSMALL;
      }

      //if the size passed in is too large, adjust the size.
      if(*pnSize > sizeof(AEECardInfo))
      {
         *pnSize = sizeof(AEECardInfo);
      }
      return(nErr);
   }
#endif // defined(FEATURE_DRV_SDCC) || defined(FEATURE_MMC)

   #if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
   case AEE_DEVICEITEM_POSDET_EMERGENCY_ONLY:
   {
      pdsm_gps_lock_e_type gpsLock;

      MEMSET(&gpsLock,0,sizeof(pdsm_gps_lock_e_type));

      if (!pnSize) {
         return(EBADPARM);
      }

      if(!pBuff || *pnSize < sizeof(boolean)) {
         *pnSize = sizeof(boolean);
         return(AEE_SUCCESS);
      }

      if(!OEM_GetConfig(CFGI_GPSONE_LOCK, &gpsLock, sizeof(pdsm_gps_lock_e_type))) {

         if ((gpsLock == PDSM_GPS_LOCK_MI) ||
            (gpsLock == PDSM_GPS_LOCK_MT) ||
            (gpsLock == PDSM_GPS_LOCK_NONE) ){
            *((boolean *)pBuff) = FALSE;
         }
         else if ( (gpsLock == PDSM_GPS_LOCK_ALL ) ) {
            *((boolean *)pBuff) = TRUE;
         }
         else {
            return (EFAILED);
         }

         *pnSize = sizeof(boolean);
         return(AEE_SUCCESS);
      }
      else {
         return(EUNSUPPORTED);
      }
   }
   #endif // FEATURE_GPSONE || FEATURE_PDAPI

#ifdef FEATURE_QAUDIOFX
   case AEECLSID_DEVICEITEM_AUD3D_MAX_ENVIRON:
   {
      if (!pnSize)
         return(EBADPARM);

      if (!pBuff || *pnSize < sizeof(uint8)) {
         *pnSize = sizeof(uint8);
         return(SUCCESS);
      }

      *((uint8 *)pBuff) = 1;
      return(SUCCESS);
   }
#endif /* FEATURE_QAUDIOFX */
   case AEE_DEVICEITEM_FSSPACE_CARD0:
   case AEE_DEVICEITEM_FSSPACE_ROOT:
   case AEE_DEVICEITEM_FSSPACE_USBHMS1:
   case AEE_DEVICEITEM_FSSPACE_USBHMS2:
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
      else if (AEE_DEVICEITEM_FSSPACE_USBHMS1 == nItem) {
         char*   pszFS = 0;
         int     nLen  = 0;

         nErr = AEE_ResolvePath(AEEFS_USB_HOST_MASS_STORAGE1, NULL, &nLen);
         if (SUCCESS != nErr) {
            return nErr;
         }

         pszFS = MALLOC(nLen);
         if (0 == pszFS) {
            return ENOMEMORY;
         }

         nErr = AEE_ResolvePath(AEEFS_USB_HOST_MASS_STORAGE1, pszFS, &nLen);
         if (SUCCESS != nErr) {
            FREE(pszFS);
            return nErr;
         }

         nErr = OEMFS_StatVFS(pszFS, &qwTotalSpace, &qwFreeSpace);
         FREE(pszFS);
      }
      else if (AEE_DEVICEITEM_FSSPACE_USBHMS2 == nItem) {
         char*   pszFS = 0;
         int     nLen  = 0;

         nErr = AEE_ResolvePath(AEEFS_USB_HOST_MASS_STORAGE2, NULL, &nLen);
         if (SUCCESS != nErr) {
            return nErr;
         }

         pszFS = MALLOC(nLen);
         if (0 == pszFS) {
            return ENOMEMORY;
         }

         nErr = AEE_ResolvePath(AEEFS_USB_HOST_MASS_STORAGE2, pszFS, &nLen);
         if (SUCCESS != nErr) {
            FREE(pszFS);
            return nErr;
         }

         nErr = OEMFS_StatVFS(pszFS, &qwTotalSpace, &qwFreeSpace);
         FREE(pszFS);
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



/*==================================================================

==================================================================*/
#if 0
uint32 GetLngCode(void)
{
   if (0 == CachedLngCode)
   {
      IShell * shell_ptr = 0;
      CachedLngCode = LNG_ENGLISH;

      shell_ptr = AEE_GetShell();

      if (shell_ptr)
      {
         IFileMgr * pfm = 0;
         ISHELL_CreateInstance(shell_ptr, AEECLSID_FILEMGR, (void **) (&pfm));
         if (pfm)
         {
            IFile * pf = 0;
            pf = IFILEMGR_OpenFile(pfm, AEEFS_SYS_DIR"lang.dat", _OFM_READ);
            if (pf)
            {
               char lan_str[11];
               int32 count = IFILE_Read(pf, lan_str, 11);
               if (count)
               {
                  char * endptr;
                  unsigned long val = STRTOUL(lan_str, &endptr, 16);
                  if (val)
                  {
                     CachedLngCode = val;
                  }
               }
               IFILE_Release(pf);
            }
            IFILEMGR_Release(pfm);
         }
      }
   }

   return CachedLngCode;
}

int SetLngCode(uint32 dwLngCode)
                  {
   int nErr = EFAILED;
   IShell * shell_ptr = 0;

   CachedLngCode = dwLngCode;
   shell_ptr = AEE_GetShell();

   if (shell_ptr)
   {
      IFileMgr * pfm = 0;
      ISHELL_CreateInstance(shell_ptr, AEECLSID_FILEMGR, (void **) (&pfm));
      if (pfm)
      {
         IFile * pf = 0;
         if (IFILEMGR_Test(pfm, AEEFS_SYS_DIR"lang.dat") == SUCCESS)
         {
            pf = IFILEMGR_OpenFile(pfm, AEEFS_SYS_DIR"lang.dat", _OFM_READWRITE);
                  }
         else
         {
            pf = IFILEMGR_OpenFile(pfm, AEEFS_SYS_DIR"lang.dat", _OFM_CREATE);
               }
         if (pf)
         {
            char lan_str[11];
            MEMSET(lan_str, 0, sizeof(lan_str));
            SNPRINTF(lan_str, sizeof(lan_str), "0x%.8x", dwLngCode);
            if (IFILE_Write(pf, lan_str, 11))
            {
               // Notify the world that the language has changed
               (void ) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
               nErr = SUCCESS;
               }
               IFILE_Release(pf);
            }
         else
         {
            nErr = IFILEMGR_GetLastError(pfm);
            DBGPRINTF("SetLngCode : open file error = %d", nErr);
         }
            IFILEMGR_Release(pfm);
         }
      }

   return nErr;
}

#else
static uint32 g_usLngCode = 0; // FOR PEK TEST ONLY
uint32 GetLngCode(void)
{
    nv_language_enum_type language=0;
    uint32 LngCode = LNG_ENGLISH;

    if(g_usLngCode != 0)
    {
        LngCode = g_usLngCode;
        g_usLngCode = 0;
        return LngCode;
    }
    
    OEM_SVCGetConfig(CFGI_LANGUAGE_SELECTION, &language, sizeof(language));

#ifdef WIN32//wlh ÁÙÊ±ÐÞ¸ÄÎªÖÐÎÄ
	language = NV_LANGUAGE_CHINESE;
#endif//WIN32

    switch (language)
    {
#ifdef FEATURE_LANG_SPANISH  
        case NV_LANGUAGE_SPANISH:
            LngCode = LNG_SPANISH;
            break;
#endif //FEATURE_LANG_SPANISH           

#ifdef FEATURE_LANG_FRENCH  
        case NV_LANGUAGE_FRENCH:
            LngCode = LNG_FRENCH;
            break;
#endif //FEATURE_LANG_FRENCH  

#ifdef FEATURE_LANG_PORTUGUESE
         case NV_LANGUAGE_PORTUGUESE:
            LngCode = LNG_PORTUGUESE;
            break;   
#endif //FEATURE_LANG_PORTUGUESE            

#ifdef FEATURE_LANG_TCHINESE
        case NV_LANGUAGE_TCHINESE:
            LngCode = LNG_TCHINESE;
            break;
#endif //FEATURE_LANG_TCHINESE           

#ifdef FEATURE_LANG_CHINESE
        case NV_LANGUAGE_CHINESE:
            LngCode = LNG_SCHINESE;
            break;
#endif //FEATURE_LANG_CHINESE            

#ifdef FEATURE_LANG_JAPANESE
         case NV_LANGUAGE_JAPANESE:
            LngCode = LNG_JAPANESE;
            break; 
#endif //FEATURE_LANG_JAPANESE            

#ifdef FEATURE_LANG_KOREAN
        case NV_LANGUAGE_KOREAN:
            LngCode = LNG_KOREAN;
            break;
#endif //FEATURE_LANG_KOREAN            

#ifdef FEATURE_LANG_HEBREW
        case NV_LANGUAGE_HEBREW:
            LngCode = LNG_HEBREW;
            break;
#endif //FEATURE_LANG_HEBREW            

#ifdef FEATURE_LANG_ARABIC
        case NV_LANGUAGE_ARABIC:
            LngCode = LNG_ARABIC;
            break;
#endif //FEATURE_LANG_ARABIC

#ifdef FEATURE_LANG_THAI
         case NV_LANGUAGE_THAI:
            LngCode = LNG_THAI;
            break;  
#endif //FEATURE_LANG_THAI            

#ifdef FEATURE_LANG_ITALIAN
        case NV_LANGUAGE_ITALIAN:
            LngCode = LNG_ITALIAN;
            break;
#endif //FEATURE_LANG_ITALIAN           

#ifdef FEATURE_LANG_INDONESIAN
        case NV_LANGUAGE_INDONESIAN:
            LngCode = LNG_INDONESIAN;
            break;
#endif 	//FEATURE_LANG_INDONESIAN     
#ifdef FEATURE_LANG_MYANMAR    //add by yangdecai 2010-1224
		case NV_LANGUAGE_MYANMAR:
		LngCode = LNG_MYANMAR;
        break;
#endif

#ifdef FEATURE_LANG_HINDI
         case NV_LANGUAGE_HINDI:
            LngCode = LNG_HINDI;
            break;     
#endif //FEATURE_LANG_HINDI

#ifdef FEATURE_LANG_RUSSIAN
         case NV_LANGUAGE_RUSSIAN:
            LngCode = LNG_RUSSIAN;
            break;     
#endif //FEATURE_LANG_RUSSIAN


#ifdef FEATURE_LANG_VIETNAMESE
         case NV_LANGUAGE_VIETNAMESE:
            LngCode = LNG_VIETNAMESE;
            break; 
#endif //FEATURE_LANG_VIETNAMESE            
           
#ifdef FEATURE_LANG_ENGLISH
        case NV_LANGUAGE_ENGLISH:
#endif //FEATURE_LANG_VIETNAMESE            
        default:
            LngCode = LNG_ENGLISH;
            break;
    }
    return LngCode;    
}

int SetLngCode(uint32 dwLngCode)
{
    nv_language_enum_type language=NV_LANGUAGE_ENGLISH;
    g_usLngCode = 0;
    switch (dwLngCode)
    {
#ifdef FEATURE_LANG_SPANISH    
        case LNG_SPANISH:
            language = NV_LANGUAGE_SPANISH;            
            break;
#endif //FEATURE_LANG_SPANISH            

#ifdef FEATURE_LANG_FRENCH  
        case LNG_FRENCH:
            language = NV_LANGUAGE_FRENCH;                
            break;
#endif //FEATURE_LANG_FRENCH              

#ifdef FEATURE_LANG_PORTUGUESE 
         case LNG_PORTUGUESE:
            language = NV_LANGUAGE_PORTUGUESE;                
            break;    
#endif //FEATURE_LANG_PORTUGUESE 

#ifdef FEATURE_LANG_TCHINESE
        case LNG_TCHINESE:
            language = NV_LANGUAGE_TCHINESE;                
            break;
#endif //FEATURE_LANG_TCHINESE

#ifdef FEATURE_LANG_CHINESE
        case LNG_SCHINESE:
            language = NV_LANGUAGE_CHINESE;                
            break;
#endif //FEATURE_LANG_CHINESE

#ifdef FEATURE_LANG_JAPANESE
         case LNG_JAPANESE:
            language = NV_LANGUAGE_JAPANESE;                
            break;  
#endif //FEATURE_LANG_JAPANESE

#ifdef FEATURE_LANG_KOREAN
        case LNG_KOREAN:
            language = NV_LANGUAGE_KOREAN;                
            break;
#endif //FEATURE_LANG_KOREAN            

#ifdef FEATURE_LANG_HEBREW
        case LNG_HEBREW:
            language = NV_LANGUAGE_HEBREW;                
            break;
#endif //FEATURE_LANG_HEBREW

#ifdef FEATURE_LANG_ARABIC
        case LNG_ARABIC:
            language = NV_LANGUAGE_ARABIC;                
            break;
#endif //FEATURE_LANG_ARABIC

#ifdef FEATURE_LANG_THAI
         case LNG_THAI:
            language = NV_LANGUAGE_THAI;                
            break;    
#endif //FEATURE_LANG_THAI

#ifdef FEATURE_LANG_ITALIAN
        case LNG_ITALIAN:
            language = NV_LANGUAGE_ITALIAN;   
            break;
#endif //FEATURE_LANG_ITALIAN

#ifdef FEATURE_LANG_INDONESIAN
        case LNG_INDONESIAN:
            language = NV_LANGUAGE_INDONESIAN;                
            break;
#endif //FEATURE_LANG_INDONESIAN
#ifdef FEATURE_LANG_MYANMAR    //add by yangdecai 2010-1224
		case LNG_MYANMAR:
		language = NV_LANGUAGE_MYANMAR;
        break;
#endif

#ifdef FEATURE_LANG_HINDI
        case LNG_HINDI:
            language = NV_LANGUAGE_HINDI;               
            break; 
#endif //FEATURE_LANG_HINDI            

#ifdef FEATURE_LANG_VIETNAMESE
         case LNG_VIETNAMESE:
            language = NV_LANGUAGE_VIETNAMESE;                
            break; 
#endif //FEATURE_LANG_VIETNAMESE            

#ifdef FEATURE_LANG_ENGLISH          
        case LNG_ENGLISH:
            language = NV_LANGUAGE_ENGLISH;
            break;
#endif //FEATURE_LANG_ENGLISH            
        default:
            language = NV_LANGUAGE_ENGLISH;  
            g_usLngCode = dwLngCode;
            break;
    }

    OEM_SVCSetConfig(CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
    return SUCCESS;
}

#endif

/*===========================================================================
Cancels Config item listener callback
===========================================================================*/
static void OEMConfig_CancelItemListener(AEECallback *pcb)
{
   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMConfig_CancelItemListener)
      return;

   if (pcb == gpcbOnItemUpdate) {
      gpcbOnItemUpdate = 0;
   }

   pcb->pCancelData = 0;
   pcb->pfnCancel = 0;
}

/*===========================================================================
Called by IConfig to register callback to be invoked on change in a config
item value
===========================================================================*/
void OEM_OnConfigItemUpdate(AEECallback *pcb, uint32 * pdwUpdatedItem)
{
   // Make sure that its not there in any other list
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = OEMConfig_CancelItemListener;
   pcb->pReserved = pdwUpdatedItem;
   gpcbOnItemUpdate = pcb;
}

#endif
