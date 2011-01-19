/*===========================================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  BrewAppMgrUtils.c

SERVICES:  This files has common stuff between AppMgr and MobileShop
   applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 2000-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "AEE.h"
#include "AEEClassIDs.h"
#include "AEEDate.h"
#include "AEESecurity.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEFlip.h"
#include "AEECLSID_FLIP.bid"
#include "BrewAppMgrUtils.h"

#ifdef USE_MENUCTL_EX
#include "aeemenuctlex.bid"
#include "AEEMenuCtlEx.h"
#endif //USE_MENUCTL_EX

#ifdef FEATURE_HAPTIC_SUPPORT
#include "AEESound.h"			
#endif

#if defined(AEE_STATIC)
#include "AEE_OEM.h"
#endif


#define FARF_UTILS     1
#define FARF(x, p)

//===========================================================================
// To compile aeemenuctl statically into project
//   add all .h and .c files from ..\aeemenuctlex into project
//   define AEEMENUCLT_STATIC here and also in ..\aeemenuctlex\aeemenuctlex.c
//===========================================================================
//#define AEEMENUCLT_STATIC

#if defined(USE_MENUCTL_EX) && defined(AEEMENUCLT_STATIC)
int MenuCtlClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod,  void **ppObj) ;
#endif //defined(USE_MENUCTL_EX) && defined(AEEMENUCLT_STATIC)

static void GetString(char* pszString, int*  pnSize);
#ifdef USE_MENUCTL_EX
static void MenuDrawItemCB(void * pvContext, AEEODCBData* is);
#endif //USE_MENUCTL_EX
/*===========================================================================

Returns pszIn + 'ch' + pszAdd. Expects pszIn on heap and pszAdd on stack.

===========================================================================*/
AECHAR * AppendWStr(AECHAR * pszIn, AECHAR * pszAdd,AECHAR ch)
{
   AECHAR * pszBuff;
   AECHAR * pszDest;
   int      i = 0;

   if(pszIn)
      i = WSTRLEN(pszIn) + 1;

   i += (pszAdd ? WSTRLEN(pszAdd) : 0) + 1;

   pszBuff = pszDest = (AECHAR *)MALLOC(i * sizeof(AECHAR));

   if(pszDest){
      if(pszIn){
         WSTRCPY(pszDest, pszIn);
         pszDest += WSTRLEN(pszIn);
         *(pszDest++) = ch;
         FREE(pszIn);
      }
      if (pszAdd)
         WSTRCPY(pszDest,pszAdd);
   }
   return(pszBuff);
}

/*===========================================================================

Gives date string corresponding to dwDate seconds since CDMA Reference time
in pszDest.

============================================================================*/
boolean GetDateString(IShell * ps, uint32 dwDate, AECHAR * pszDest, int nSize)
{
   JulianType     j;
   IDateCtl *     pd;
   AEEDeviceInfo  di;
 
   CreateInstance(ps,AEECLSID_DATECTL,(void **)&pd);

   *pszDest = 0;

   if(!pd)
      return(FALSE);

   ISHELL_GetJulianDate(ps, dwDate, &j);
   IDATECTL_SetDate(pd, j.wYear, j.wMonth, j.wDay);

   ISHELL_GetDeviceInfo(ps,&di);
   IDATECTL_GetDateString(pd,pszDest,nSize,NULL,(uint32)(di.dwLang == LNG_ENGLISH ? DFMT_DD_MON_YY : DFMT_YYYY_MM_DD));
   IDATECTL_Release(pd);
   return(TRUE);
}

/*===========================================================================

Returns specified dwSize as 10K, 5MB, 5.4MB ((10K), (5MB), (5.4MB) respectively
if bWrap == TRUE) in specified buffer.

dwDestLen is the num of AECHAR in the pszDest buffer

===========================================================================*/
void FormatMem(uint64 qwSize, AECHAR * pszDest, uint32 dwDestLen, boolean bWrap)
{
   char     szText[32];
   char *   psz;
   uint16   nGB,nMB,nK;

   psz = szText;
   if(bWrap){
      STRCPY(szText," (");
      psz += STRLEN(szText);
   }
   else
      *psz = 0;

   nGB = (uint16)(qwSize / ONE_GB);
   nMB = (uint16)((qwSize - ((uint64)nGB * ONE_GB)) / ONE_MB);
   nK = (uint16)((qwSize - ((uint64)nMB * ONE_MB)) / ONE_K);

   //
   // From 0Kb up to 999Kb, we print "0K" to "999K"
   // From 1000Kb up to 999.99Mb, we print "0.97Mb" to "999.99Mb"
   // From 1000Mb up, we print "0.97Gb" up
   //
   // We never print ".00" though, if the value would be 1.00, we just print "1"
   //
   if(nGB || (nMB > 999))
      SPRINTF(psz,(const char *)(((nMB * 100) / ONE_K) ? "%d.%02.2dGB" : "%dGB"), nGB,((nMB * 100) / ONE_K));
   else if(nMB || (nK > 999))
      SPRINTF(psz,(const char *)(((nK * 100) / ONE_K) ? "%d.%02.2dMB" : "%dMB"), nMB,((nK * 100) / ONE_K));
   else 
      SPRINTF(psz,"%dK", nK);

   if(bWrap)
      STRCAT(psz,")");

   (void)MEMSET(pszDest, 0, dwDestLen * sizeof(AECHAR));
   STRTOWSTR(szText,pszDest, dwDestLen * sizeof(AECHAR));
}

/*===========================================================================

Frees memory associated with specified pointer

===========================================================================*/
void FreePtr(void ** ppObj)
{
   if(ppObj && *ppObj){
      FREE(*ppObj);
      *ppObj = NULL;
   }
}

/*===========================================================================

Frees memory associated with specified interface

===========================================================================*/
void FreeObj(void ** ppObj)
{
   if (ppObj && *ppObj) {
      IBASE_Release((IBase *)*ppObj);
      *ppObj = NULL;
   }
}

/*===========================================================================

If bShowInvalid == TRUE, copies expiration string in specified buffer and
returns TRUE. If bShowInvalid = FALSE, copies expiration string in the
specified buffer and returns TRUE for the items that haven't expired yet.
Otherwise returns FALSE.

===========================================================================*/
boolean GetExpireString(IShell* pIShell, const char* pResFile, uint32 dwNow, AEELicenseType lt, uint32 dwValue, AECHAR * pszDest, int nSize, boolean bShowInvalid, boolean bAdjust)
{
   uint16   wIDFormat;
   AECHAR   szFormat[32];
   uint32   dwExpire = 0;

   // Get current time if not specified
   if(!dwNow)
   {
#ifdef GETUTCSECONDS
            dwNow = GETUTCSECONDS();
#else
            dwNow = GETTIMESECONDS();
#endif
   }

   // Load appropriate string according to dwValue and license type
   if(dwValue == BV_UNLIMITED)
      wIDFormat = IDS_EXP_UNLIMITED; // Unlimited Use
   else{
      switch(lt){
         case LT_USES:
            if(!dwValue){
               if(!bShowInvalid)
                  return(FALSE);    // FALSE if no uses left and bShowInvalid == FALSE
               wIDFormat = IDS_EXP_EXPIRED;  // No uses left
            }
            else
               // if bShowInvalid == TRUE; %d Uses Remaining, Otherwise %d Uses
               wIDFormat = (uint16)(bShowInvalid ? IDS_MT_USES_LEFT : IDS_EXP_USES);
            break;

         case LT_DAYS:
         case LT_DATE:
            wIDFormat = IDS_MT_EXP_DATE; // Expires on %s

            if(bAdjust && lt == LT_DAYS)
               dwExpire = dwNow + (dwValue * SECS_PER_DAY);
            else
               dwExpire = dwValue;

            if(dwNow >= dwExpire){
               if(!bShowInvalid)
                  return(FALSE);    // FALSE if already expired and bShowInvalid == FALSE
               wIDFormat = IDS_EXP_EXPIREDON;   // Expired on %s
            }
            else
               wIDFormat = IDS_MT_EXP_DATE;        // Expires on %s
            break;

         case LT_MINUTES_OF_USE:
            wIDFormat = IDS_MT_MIN_REMAINING;   // %d Minutes Remaining
            break;

         default:
            if(!bShowInvalid)
               return(FALSE);    // FALSE if bShowInvalid == FALSE
            wIDFormat = IDS_EXP_UNLIMITED;   // Unlimited Use
      }
   }

   // Load aforementioned string
   ISHELL_LoadResString(pIShell, pResFile, wIDFormat, szFormat, sizeof(szFormat));

   if(dwValue != BV_UNLIMITED && (lt == LT_DATE || lt == LT_DAYS)){
      AECHAR      szDate[32];
      
      // Get date string corresponding to the expiration time
      if(!GetDateString(pIShell, dwExpire, szDate, sizeof(szDate)))
         return(FALSE);

      WSPRINTF(pszDest, nSize, szFormat, szDate); // Copy expiration string
   }
   else
      WSPRINTF(pszDest,nSize, szFormat, dwValue); // Copy expiration string

   return(TRUE);
}

/*===========================================================================

Returns names of the unhidden applications.

===========================================================================*/
AECHAR * LoadAppNames(IShell * pIShell, AEEAppInfo * pai, int nCount)
{
   AECHAR   szName[32];
   AECHAR * pText = NULL;

   while(nCount--){
      if(!(pai->wFlags & AFLAG_HIDDEN)){
         if(ISHELL_LoadResString(pIShell, pai->pszMIF,APPR_NAME(*pai),szName,sizeof(szName)))
            pText = AppendWStr(pText, szName, (AECHAR)',');
      }
      pai++;
   }
   return(pText);
}

//===========================================================================
//
// DESCRIPTION
//    Fetches App Name associated with the specified application manager item.
//
// PARAMETERS
//    pIShell: IShell *
//    pi:      AppMgr Item
//
// Return Value: App Name
//===========================================================================
/*
AECHAR * GetAppName(IShell* pIShell, AppMgrItem* pi)
{
   AEEAppInfo ai;
   AEECLSID cls = pi->cls;

   if (!pi)
      return NULL;

   if (*pi->wszName)
      return pi->wszName;

   if (cls && ISHELL_QueryClass(pIShell, cls, &ai))
      ISHELL_LoadResString(pIShell, ai.pszMIF, APPR_NAME(ai), pi->wszName, sizeof(pi->wszName));      

   if(!*pi->wszName)
      ISHELL_LoadResString(pIShell, AEECONTROLS_RES_FILE, AEE_IDS_UNKNOWN, pi->wszName, sizeof(pi->wszName));

   return pi->wszName;
}
*/
/*===========================================================================

Draw histogram according to the download %

===========================================================================*/
void DrawHist(IDisplay * pd, AEERect * prc, uint16 wPct)
{
   AEERect  rc = *prc;

   if(wPct > 100)
      wPct = 100;

   // Draw Frame
   IDISPLAY_DrawFrame(pd, &rc, AEE_FT_INDENT, CLR_SYS_SCROLLBAR);
   rc.dx = (rc.dx * wPct) / 100;

   // Fill rect according to % download complete
   IDISPLAY_FillRect(pd, &rc, CLR_SYS_SCROLLBAR_FILL);
}


/*===========================================================================

Loads a format string from the resource file. Uses this and specified string
to return formatted string.

===========================================================================*/
AECHAR * FormatString(IShell * ps, const char * pszRes, uint16 wFormat, AECHAR * psz)
{            
   AECHAR   * pszText;
   AECHAR   * pszFormat;

   pszText = (AECHAR *)MALLOC(MAX_STR_SIZE + WSTRSIZE(psz));
   if(pszText){
      pszFormat = (AECHAR *)MALLOC(MAX_STR_SIZE);
      if(pszFormat){
         // Load format string from the resource file
         if(ISHELL_LoadResString(ps, pszRes, wFormat, pszFormat,MAX_STR_SIZE)){
            // Use format string and specified string to construct formatted string
            WSPRINTF(pszText,MAX_STR_SIZE,pszFormat,psz);
            FREE(pszFormat);
            return(pszText);
         }
         FREE(pszFormat);
      }
      FREE(pszText);
   }
   return(NULL);
}

/*===========================================================================

Loads a format string from the resource file. Uses this and specified strings
to return formatted string.

===========================================================================*/
AECHAR * FormatStringEx(IShell * ps, const char * pszRes, uint16 wFormat, AECHAR * psz1, AECHAR * psz2)
{            
   AECHAR   * pszText;
   AECHAR   * pszFormat;

   pszText = (AECHAR *)MALLOC(MAX_STR_SIZE + WSTRSIZE(psz1) + WSTRSIZE(psz2));
   if(pszText){
      pszFormat = (AECHAR *)MALLOC(MAX_STR_SIZE);
      if(pszFormat){
         // Load format string from the resource file
         if(ISHELL_LoadResString(ps, pszRes, wFormat, pszFormat,MAX_STR_SIZE)){
            // Use format string and specified string to construct formatted string
            WSPRINTF(pszText,MAX_STR_SIZE,pszFormat,psz1,psz2);
            FREE(pszFormat);
            return(pszText);
         }
         FREE(pszFormat);
      }
      FREE(pszText);
   }
   return(NULL);
}

//===========================================================================
//DESCRIPTION
//    Sets style and color of the menu
//
//PARAMETERS
//    pMenu : IMenuCtl pointer
//    nMenuCls : Menu Class
//    nColorDepth : Device Color Depth
//
//Return Value:
//    none
//    
//===========================================================================
void SetMenuStyleAndColor(IMenuCtl* pMenu, AEECLSID nMenuCls, uint16 nColorDepth, boolean isTouch)
{
   AEEItemStyle   sel,normal;
   AEEMenuColors  col;

   if (OEM_UNDERLINE_MENU_TITLE)
   {
      uint32 dwProps;
      dwProps = IMENUCTL_GetProperties(pMenu);

#ifdef MP_ALLOW_DUPE_IDS
      IMENUCTL_SetProperties(pMenu, dwProps|MP_UNDERLINE_TITLE|MP_ALLOW_DUPE_IDS);
#else
      IMENUCTL_SetProperties(pMenu, dwProps|MP_UNDERLINE_TITLE);
#endif // MP_ALLOW_DUPE_IDS
   }

#ifdef USE_MENUCTL_EX
   if (AEECLSID_MENUCTLEX == nMenuCls || AEECLSID_SOFTKEYCTLEX == nMenuCls)
   {
      // enabling/disabling ownerdraw will triger menuctl read scrollbar width
      if(SUCCESS != IMENUCTL_SetOwnerDrawCB(pMenu, (PFNOWNERDRAWCB)&MenuDrawItemCB, NULL, ODF_SB_OD|ODA_DRAW_ITEM))
      {
         DBGPRINTF_ERROR("MC: failed to set owner draw.");
      }

      if(SUCCESS != IMENUCTL_SetOwnerDrawCB(pMenu, (PFNOWNERDRAWCB)NULL, NULL, 0))
      {
         DBGPRINTF_ERROR("MC: failed to set owner draw.");
      }
   }
#endif //USE_MENUCTL_EX

   // If it is a touch contorl we need to always modify some things
   if (isTouch)
   {
      AEEItemStyle Sel,Normal;
      AEEMenuColors rMenuColors;

      switch (nMenuCls)
      {
#ifdef USE_MENUCTL_EX
         case AEECLSID_MENUCTLEX:
#endif //USE_MENUCTL_EX
         case AEECLSID_MENUCTL:
            Sel.ft = AEE_FT_BOX ;
            Sel.xOffset = TOUCHMENU_ITEM_OFFSET;
            Sel.yOffset = TOUCHMENU_ITEM_OFFSET;
            Sel.roImage = AEE_RO_TRANSPARENT;

            Normal.ft = AEE_FT_LOWERED;
            Normal.xOffset = TOUCHMENU_ITEM_OFFSET;
            Normal.yOffset = TOUCHMENU_ITEM_OFFSET;
            Normal.roImage = AEE_RO_TRANSPARENT;

            IMENUCTL_SetStyle(pMenu, &Normal, &Sel);

            rMenuColors.wMask = MC_BACK | MC_TEXT | MC_SEL_BACK | MC_SEL_TEXT | MC_FRAME | MC_TITLE;
            rMenuColors.cBack = TOUCHMENU_BACKGROUND;
            rMenuColors.cText = RGB_BLACK;
            rMenuColors.cSelBack = TOUCHMENU_SELECT_BACKGROUND;
            rMenuColors.cSelText = RGB_BLACK;
            rMenuColors.cFrame = TOUCHMENU_ITEMFRAME;
            rMenuColors.cTitle = TOUCHSCREEN_BACKGROUND;

            IMENUCTL_SetColors(pMenu, &rMenuColors );
            break;

#ifdef USE_MENUCTL_EX
         case AEECLSID_SOFTKEYCTLEX:
#endif //USE_MENUCTL_EX
         case AEECLSID_SOFTKEYCTL:
         {
            //set the softmenu properties
            Sel.ft = AEE_FT_BOX ;
            Sel.xOffset = TOUCHMENU_ITEM_OFFSET;
            Sel.yOffset = TOUCHMENU_ITEM_OFFSET;
            Sel.roImage = AEE_RO_TRANSPARENT;

            Normal.ft = AEE_FT_RAISED;
            Normal.xOffset = TOUCHMENU_ITEM_OFFSET;
            Normal.yOffset = TOUCHMENU_ITEM_OFFSET;
            Normal.roImage = AEE_RO_TRANSPARENT;

            IMENUCTL_SetStyle(pMenu, &Normal, &Sel);

            rMenuColors.wMask = MC_BACK | MC_TEXT | MC_SEL_BACK | MC_SEL_TEXT; // | MC_FRAME | MC_TITLE;
            rMenuColors.cBack = TOUCHMENU_BACKGROUND;
            rMenuColors.cText = RGB_BLACK;
            rMenuColors.cSelBack = TOUCHMENU_SELECT_BACKGROUND;
            rMenuColors.cSelText = RGB_BLACK;
            //rMenuColors.cFrame = TOUCHMENU_ITEMFRAME;
            //rMenuColors.cTitle = TOUCHSCREEN_BACKGROUND;

            IMENUCTL_SetColors(pMenu, &rMenuColors );
         }

         default:
            break;
      }
   }

   // OEM_USE_DEFAULT_COLOR_STYLE is TRUE, return without making any changes
   // in color and style
   if (OEM_USE_DEFAULT_COLOR_STYLE)
      return;

   // Set Color Mask
   col.wMask = MENU_COLOR_MASK;

   // Set style and color according to the color depth
   if (nColorDepth > 2)
   {
      // Normal Item
      normal.ft = AEE_FT_NONE;
      normal.xOffset = 0;
      normal.yOffset = 0;
      normal.roImage = (AEE_RO_COPY|AEE_RO_TRANSPARENT);

      // Selected Item
      sel.ft = AEE_FT_RAISED;
      sel.xOffset = 0;
      sel.yOffset = 0;
      sel.roImage = AEE_RO_TRANSPARENT;

      switch (nMenuCls)
      {
#ifdef USE_MENUCTL_EX
        case AEECLSID_MENUCTLEX:
#endif //USE_MENUCTL_EX
        case AEECLSID_MENUCTL:

            // Set Menu Colors
            col.cBack =             COLOR_MENU_BACKGROUND;
            col.cSelBack =          COLOR_MENU_SELECT_BACKGROUND;
            col.cSelText =          COLOR_MENU_SELECT_TEXT;
            break;

#ifdef USE_MENUCTL_EX     	
        case AEECLSID_SOFTKEYCTLEX:
#endif //USE_MENUCTL_EX
        case AEECLSID_SOFTKEYCTL:

            // Set Softkey/IConView Colors
            col.cBack =             COLOR_SK_BACKGROUND;
            col.cSelBack =          COLOR_SK_SELECT_BACKGROUND;
            col.cSelText =          COLOR_SK_SELECT_TEXT;
            break;

#ifdef USE_MENUCTL_EX     	
        case AEECLSID_ICONVIEWCTLEX:
#endif //USE_MENUCTL_EX
        case AEECLSID_ICONVIEWCTL:
   
           // Set Softkey/IConView Colors
            col.cBack =             COLOR_ICON_BACKGROUND;
            col.cSelBack =          COLOR_ICON_SELECT_BACKGROUND;
            col.cSelText =          COLOR_ICON_SELECT_TEXT;
            break;
      }
   }
   else
   {
      // Normal Item
      normal.ft = AEE_FT_NONE;
      normal.xOffset = 0;
      normal.yOffset = 0;
      normal.roImage = AEE_RO_COPY;

      // Selected Item
      sel.ft = AEE_FT_NONE;
      sel.xOffset = 0;
      sel.yOffset = 0;
      sel.roImage = AEE_RO_NOT;

      switch (nMenuCls)
      {
#ifdef USE_MENUCTL_EX
         case AEECLSID_MENUCTLEX:
         case AEECLSID_SOFTKEYCTLEX:
         case AEECLSID_ICONVIEWCTLEX:  
#endif //USE_MENUCTL_EX
         case AEECLSID_MENUCTL:
         case AEECLSID_SOFTKEYCTL:
         case AEECLSID_ICONVIEWCTL:
            // Same colors for menu/softkey/icon view
            col.cBack =             MENU_BACKGROUND;
            col.cSelBack =          MENU_SELECT_BACKGROUND;
            col.cSelText =          MENU_SELECT_TEXT;
            break;
      }
   }

   // Set Menu Style
   IMENUCTL_SetStyle(pMenu, &normal, &sel);

   // Set Menu Colors
   IMENUCTL_SetColors(pMenu, &col);
}

//===========================================================================
// 
// DESCRIPTION
//    Gets item ID corresponding to the specified class ID and fills in the
//    [DL2]AppModInfo* for the item, if provided
//
// PARAMETERS
//    pDownload: pointer to IDownload interface
//    cls: class ID
//
// Return Value:
//    None
//
//===========================================================================
#ifdef USE_IDL2
DLITEMID GetItemIDFromClassID(IDownload2* pIDownload2, AEECLSID cls, DL2AppModInfo** ppai)
#else
DLITEMID GetItemIDFromClassID(IDownload* pIDownload, AEECLSID cls, AppModInfo** ppai)
#endif
{
   uint32* pIDs;
   uint32* pdw;
   uint32 dw;
#ifdef USE_IDL2
   DL2AppModInfo* pai;
   IDownload* pIDownload = NULL;
#else
   AppModInfo* pai;
#endif

#ifdef USE_IDL2
   if (NULL == pIDownload2)
   {
      return 0;
   }

   if (IDOWNLOAD2_QueryInterface(pIDownload2, AEECLSID_DOWNLOAD, 
                                 (void**)&pIDownload) != AEE_SUCCESS)
   {
      return 0;
	}
#endif

   if (NULL == pIDownload)
   {
      return 0;
   }

   pIDs = IDOWNLOAD_GetAppIDList(pIDownload);
   if(pIDs)
   {
      for(pdw = pIDs; (dw = *pdw) != 0; pdw++)
      {
         //Its not a valid app (with valid license), no point checking for the license
         if (dw >= FAKE_MODULE_ID)
            continue;
#ifdef USE_IDL2
         pai = IDOWNLOAD2_GetModInfo(pIDownload2, dw);
#else
         pai = IDOWNLOAD_GetModInfo(pIDownload, dw);
#endif
         if (pai)
         {
            uint16 index;
            for (index = 0; index < pai->nApps; index++)
            {
               if (pai->apps[index].cls == cls)
               {
#ifdef USE_IDL2
                  FreeObj((void**)&pIDownload);
#endif
                  if (NULL != ppai)
                  {
                     *ppai = pai;
                  }
                  return (dw);
               }
            }
         }
      }
   }
#ifdef USE_IDL2
   FreeObj((void**)&pIDownload);
#endif
   return 0;
}


//===========================================================================
// 
// DESCRIPTION
//   Locks an App.  This function is a wrapper around IDOWNLOAD_Lock()
//   currently IDOWNLOAD_Lock() does not return success/failure like
//   the documentaion states, so this function makes checks after
//   calling to verify the states
//
// PARAMETERS
//    pDownload[2]: pointer to IDownload[2] interface
//    id: DLITEMID
//    bLock: whether to lock or unlock
//
// Return Value:
//    Success/Failure
//
//===========================================================================
#ifdef USE_IDL2
boolean LockApp(IDownload2* pIDownload2, DLITEMID id, boolean bLock)
#else
boolean LockApp(IDownload* pIDownload, DLITEMID id, boolean bLock)
#endif
{
   boolean bRet = FALSE;
#ifdef USE_IDL2
   DL2AppModInfo* pai;
   IDownload* pIDownload = NULL;
#else
   AppModInfo* pai;
#endif

#ifdef USE_IDL2
   if (NULL == pIDownload2)
   {
      return FALSE;
   }

   if (IDOWNLOAD2_QueryInterface(pIDownload2, AEECLSID_DOWNLOAD, 
                                 (void**)&pIDownload) != AEE_SUCCESS)
   {
      return FALSE;
	}
#endif

   if (NULL == pIDownload)
   {
      return FALSE;
   }

   // Lock the App
   IDOWNLOAD_Lock(pIDownload, id, bLock);

   // Now verify the state of the app
#ifdef USE_IDL2
   pai = IDOWNLOAD2_GetModInfo(pIDownload2, id);
#else
   pai = IDOWNLOAD_GetModInfo(pIDownload, id);
#endif

   if (NULL != pai)
   {
      if (bLock == pai->bLocked)
      {
         bRet = TRUE;
      }
   }

#ifdef USE_IDL2
   FreeObj((void**)&pIDownload);
#endif

   return bRet;
}


char* AllocSecretString(
   IShell*  piShell,
   boolean  bUsePrevWindow)
{
   int      nErr              = SUCCESS;
   IHash*   piHash            = NULL;
   uint32   uTimeWindow       = 0;
   int      nTimeWindowSize   = 0;
   char     szTimeWindow[11]  = {0};
   int      nEsnSize          = 0;
   uint32   uEsn              = 0;
   int      nMobileIdSize     = 0;
   char*    pszMobileId       = NULL;
   int      nStringSize       = 0;
   char     szString[14]      = {0};
   byte     dummyBuffer[1]    = {0};
   int      nHashResultSize   = 0;
   byte*    pHashResult       = NULL;
   int      nSecretStringSize = 0;
   char*    pszSecretString   = NULL;
   int      i                 = 0;

   // Check for invalid parameters.
   if (piShell == NULL) {
      nErr = EBADPARM;
   }

   if (nErr == SUCCESS) {
      nErr = CreateInstance(piShell, AEECLSID_MD5, (void**)&piHash);
   }

   if (nErr == SUCCESS) {
      uTimeWindow = GETUPTIMEMS() >> 15; // Window length is 32.768 seconds.
      if (bUsePrevWindow && (uTimeWindow > 0)) {
         --uTimeWindow;
      }
      SNPRINTF(szTimeWindow, sizeof(szTimeWindow), "%u", uTimeWindow);
      nTimeWindowSize = STRLEN(szTimeWindow) + 1;
   }

   if (nErr == SUCCESS) {
      nEsnSize = sizeof(uEsn);
      nErr = ISHELL_GetDeviceInfoEx(piShell, AEE_DEVICEITEM_ESN, &uEsn, &nEsnSize);
   }

   if (nErr == SUCCESS) {
      if (nEsnSize != sizeof(uEsn)) {
         nErr = EFAILED;
      }
   }

   if (nErr == SUCCESS) {
      nMobileIdSize = 0;
      nErr = ISHELL_GetDeviceInfoEx(piShell, AEE_DEVICEITEM_MOBILE_ID, NULL, &nMobileIdSize);
   }

   if (nErr == SUCCESS) {
      pszMobileId = MALLOC(nMobileIdSize);
      if (pszMobileId == NULL) {
         nErr = ENOMEMORY;
      }
   }

   if (nErr == SUCCESS) {
      nErr = ISHELL_GetDeviceInfoEx(piShell, AEE_DEVICEITEM_MOBILE_ID, pszMobileId, &nMobileIdSize);
   }

   if (nErr == SUCCESS) {
      nStringSize = sizeof(szString);
      GetString(szString, &nStringSize);
   }

   if (nErr == SUCCESS) {
      IHASH_Update(piHash, (byte*)szTimeWindow, nTimeWindowSize);
      IHASH_Update(piHash, (byte*)&uEsn, nEsnSize);
      IHASH_Update(piHash, (byte*)pszMobileId, nMobileIdSize);
      IHASH_Update(piHash, (byte*)szString, nStringSize);
      nHashResultSize = sizeof(dummyBuffer);
      nErr = IHASH_GetResult(piHash, dummyBuffer, &nHashResultSize);
      if (nErr == AEE_HASH_MORE_DATA) {
         nErr = SUCCESS;
      }
   }

   if (nErr == SUCCESS) {
      pHashResult = MALLOC(nHashResultSize);
      if (pHashResult == NULL) {
         nErr = ENOMEMORY;
      }
   }

   if (nErr == SUCCESS) {
      nErr = IHASH_GetResult(piHash, pHashResult, &nHashResultSize);
   }

   if (nErr == SUCCESS) {
      nSecretStringSize = (2 * nHashResultSize) + 1;
      pszSecretString = MALLOC(nSecretStringSize);
      if (pszSecretString == NULL) {
         nErr = ENOMEMORY;
      }
   }

   if (nErr == SUCCESS) {
      for (i = 0; i < nHashResultSize; ++i) {
         SNPRINTF(pszSecretString + (2 * i), 2 + 1, "%2.2x", pHashResult[i]);
      }
   }

   if (piHash != NULL) {
      (void)IHASH_Release(piHash);
      piHash = NULL;
   }

   FREEIF(pszMobileId);
   FREEIF(pHashResult);

   if (nErr != SUCCESS) {
      FREEIF(pszSecretString);
   }

   return pszSecretString;
}

//===========================================================================
//DESCRIPTION
//    concats secret string on to the end of the given command string
//
//PARAMETERS
//    piShell     : IShell pointer
//    pszCommand  : the command pointer, which has been alloced
//
//Return Value:
//    boolean true on success
//    
//===========================================================================
boolean AppendSecretString(
   IShell*  piShell,
   char * pszCommand)
{
   char * pszSecretString = NULL;

   // check args
   if (!piShell || !pszCommand ) {
      return FALSE;
   }

   // Make sure there is plenty of room to append
   if (!REALLOC(pszCommand, MAX_STR_SIZE)) {
      return FALSE;
   }

   // Get the secret string 
   pszSecretString = AllocSecretString(piShell, FALSE);

   // Make sure we have a secret string and room to append.
   // The +1 is for the comma seperator
   if (pszSecretString && 
       (STRLEN(pszSecretString) + STRLEN(pszCommand) + 1) < MAX_STR_SIZE) {
      char * pszEnd = NULL;
      
      // Add a comma to the end of the command:
      pszEnd = pszCommand + STRLEN(pszCommand);
      *pszEnd = CMSHOP_CMD_COMMA;
      pszEnd++;
      *pszEnd = '\0';
      // Concatenated the secret string to the command
      STRCAT(pszCommand, pszSecretString);
      FREEIF(pszSecretString);
      return TRUE;
   }
   else {
      FREEIF(pszSecretString);
      return FALSE;
   }
}

//===========================================================================
//DESCRIPTION
//    checks received secret string to see if it is valid.
//
//PARAMETERS
//    piShell     : IShell pointer
//    pszReceivedSecretString : Received string pointer
//
//Return Value:
//    boolean true on success
//    
//===========================================================================
boolean CheckSecretString(
   IShell* piShell, 
   char* pszReceivedSecretString)
{
   char* pszSecretString = NULL;

   // check params
   if (!pszReceivedSecretString || !piShell) {
      return FALSE;
   }
   
   // get valid secret string.  If they don't match use previous window and check again:
   pszSecretString = AllocSecretString(piShell, FALSE);
   if ( !pszSecretString || (STRCMP(pszReceivedSecretString, pszSecretString) != 0)) {
      FREEIF(pszSecretString);
      pszSecretString = AllocSecretString(piShell, TRUE);
      if (!pszSecretString || (STRCMP(pszReceivedSecretString, pszSecretString) != 0)) {
         FREEIF(pszSecretString);
         return FALSE;
      }
   }
   FREEIF(pszSecretString);
   return TRUE;
}

static void GetString(
   char* pszString,
   int*  pnSize)
{
   char* psz1              = "chick";
   char* psz2              = "turkey"; // not used, included as a deception
   char* psz3              = "svccfs";
   int   nMaxStringLength  = *pnSize - 1;
   char* pChar             = NULL;
   int   nPos              = 0;

   pszString[nMaxStringLength] = '\0';
   // reference psz2 so it will not be compiled out and further the deception
   STRNCPY(pszString, psz2, nMaxStringLength);
   // copy what will be "rubber". Copy right over the deception
   STRNCPY(pszString, psz3, nMaxStringLength);
   // subtract 1 from each character to make rubber out of svccfs
   for (pChar = pszString; *pChar != '\0'; ++pChar) {
     --(*pChar);
   }
   // add the "check"
   nPos = STRLEN(pszString);
   STRNCPY(pszString + nPos, psz1, nMaxStringLength - nPos);
   // Finally put the "en" on
   nPos = STRLEN(pszString);
   STRNCPY(pszString + nPos, "e", nMaxStringLength - nPos);
   nPos = STRLEN(pszString);
   STRNCPY(pszString + nPos, "n", nMaxStringLength - nPos);
   // return the size
   *pnSize = STRLEN(pszString) + 1;
}

// Ascertain the current display is touch enabled or not - to create brew controls or VZW controls
boolean IsDisplayTouchEnabled(AEECLSID checkDisplayID, uint16 touchField) 
{
  int displayIndex;
  // find out to which AEECLSID_DISPLAYn, the current dispaly is mapping to.
  // BREW supports 4 displays
  switch (checkDisplayID)
   {
      case AEECLSID_DISPLAY1:
         FARF(UTILS, ("Check enabled: display 1"));
         displayIndex = 1;
         break;
      case AEECLSID_DISPLAY2:
         FARF(UTILS, ("Check enabled: display 2"));
         displayIndex = 2;
         break;
      case AEECLSID_DISPLAY3:
         FARF(UTILS, ("Check enabled: display 3"));
         displayIndex = 3;
         break;
      case AEECLSID_DISPLAY4:
         FARF(UTILS, ("Check enabled: display 4"));
         displayIndex = 4;
         break;
      default:
         FARF(UTILS, ("Invalid display index"));
         return FALSE;
   }
  //known the current display id, find out whether the display is touch enabled or not
  return IS_DISPLAY_TOUCH(displayIndex, touchField);
}










#ifdef FEATURE_OEMCFG

static char *OEMCfg_GetLine(OEMCfg *pme, IFile* pIFile)
{
   boolean blastingEOL = FALSE;
   char lineBuf[MAX_STR_CHARS];
   int lineBufPos = 0;

   if (pme->m_pCfgFileBuf == NULL)
      pme->m_pCfgFileBuf = (char *) MALLOC(MAX_STR_CHARS);

   while ((pme->m_iCfgFileBufPos < pme->m_nCfgBytes) || ((pme->m_nCfgBytes = IFILE_Read(pIFile, (void *) pme->m_pCfgFileBuf, MAX_STR_CHARS)) > 0))
   {
      // If we just read a new block of data, reset curPos.
      if (pme->m_iCfgFileBufPos >= pme->m_nCfgBytes)
         pme->m_iCfgFileBufPos = 0;
      // Start at current buffer location and read forward until we see a CR and/or LF
      if ((pme->m_pCfgFileBuf[pme->m_iCfgFileBufPos] == '\n') || (pme->m_pCfgFileBuf[pme->m_iCfgFileBufPos] == '\r'))
      {
         blastingEOL = TRUE;
         pme->m_iCfgFileBufPos++;
      }
      // Finished blasting the EOL or the line is too long
      else if ((blastingEOL) || (lineBufPos >= (MAX_STR_CHARS - 1)))
      {
         // We have successfully blasted the EOL and are ready to return this line
         lineBuf[(lineBufPos < MAX_STR_CHARS) ? lineBufPos : (MAX_STR_CHARS - 1)] = 0;
         return STRDUP(lineBuf);
      }
      // Middle of the line somewhere, copy it.
      else
      {
         lineBuf[lineBufPos++] = pme->m_pCfgFileBuf[pme->m_iCfgFileBufPos++];
      }
   }
   // Once we hit the EOF, return whatever is leftover in the buffer.  If there is nothing, return NULL.
   if (lineBufPos > 0)
   {
      lineBuf[lineBufPos] = 0;
      return STRDUP(lineBuf);
   }
   // EOF and nothing left to return.
   FreePtr((void **) &pme->m_pCfgFileBuf);
   return NULL;
}

boolean OEMCfg_NextNVPair(OEMCfg *pme, char** ppszName, char **ppszValue)
{
   boolean done = FALSE;
   char quoted = 0;
   char *line;
   char *name;
   char *value;

   *ppszName = NULL;
   *ppszValue = NULL;
   while ((!done) && ((line = OEMCfg_GetLine(pme, pme->m_pIFile)) != NULL))
   {

      char *tmpLine;
      char *valueMunger;
      char *valueEnd;

      name = NULL;
      value = NULL;
      valueMunger = NULL;
      valueEnd = NULL;
      //
      // Trim leading and trailing spaces
      //
      for (tmpLine = line; *tmpLine != 0; ++tmpLine);    // Find the end
      for (--tmpLine; (tmpLine > line) && ((*tmpLine == ' ') || (*tmpLine == '\t')); --tmpLine) *tmpLine = 0; // Nuke trailing spaces/tabs
      for (tmpLine = line; ((*tmpLine == ' ') || (*tmpLine == '\t')); ++tmpLine);      // Advance past leading spaces/tabs
      //
      // At this point, tmpLine points to first non-space and is trimmed.
      //
      while (tmpLine && (*tmpLine != 0))
      {
         if (((*tmpLine == '#') || (*tmpLine == ';')) && (!quoted))        // '#' and ';' indicate rest of line is a comment
         {
            tmpLine = NULL;
            continue;
         }
         //
         // The line begins with a name.
         //
         if (!name)
         {
            name = tmpLine;
         }
         //
         // We're still processing the name. Do some small amount of checking for invalid stuff.
         //
         if (!value)
         {
            //
            // Skip leading spaces before the name
            //
            if ((*name == ' ') || (*name == '\t'))
            {
               name++;
               tmpLine++;
            }
            //
            // Names cannot contain spaces but our error reporting capabilities are severely limited.
            // So if we come to a space in the name, we're just going to end the name at that point
            // and ignore the other characters after the space.
            //
            else if ((*tmpLine == ' ') || (*tmpLine == '\t'))
            {
               *tmpLine++ = 0;
            }
            //
            // When we hit an '=' character, start collecting the value.
            //
            else if (*tmpLine == '=')
            {
               *tmpLine++ = 0;         // NULL terminate the name
               value = tmpLine;        // Grab a pointer to the start of the value
               valueMunger = value;    // and set up the value builder pointer.
            }
            else
               tmpLine++;           // Middle of the name just accept whatever it is.
         }
         //
         // Handle the various types of special value characters
         //
         else
         {
            //
            // Check for leading and trailing spaces in the value and skip them
            //
            if (((*tmpLine == ' ') || (*tmpLine =='\t')) && (!quoted))
            {
               //
               // Leading spaces
               //
               if (valueMunger == value)
               {
                  tmpLine++;
                  continue;
               }
               //
               // Trailing spaces
               //
               else
               {
                  if (valueEnd == NULL)
                     valueEnd = valueMunger;
               }
            }
            else
               valueEnd = NULL;
            //
            // Look for quoted strings in the value
            //
            if ((*tmpLine == '"') || (*tmpLine == '\''))
            {
               if (*tmpLine == quoted)
               {
                  //
                  // We are currently looking at a potential closure of the previously opened quoted string. If the
                  // character used to open the quoted string appears twice, then it should appear once in the resulting
                  // string and the quotation should not be closed.
                  //
                  if (*(tmpLine + 1) == quoted)
                  {
                     *valueMunger++ = quoted;
                     valueEnd = NULL;              // Whenever we add a character to the value, we reset valueEnd
                     tmpLine += 2;
                     continue;
                  }
                  else
                  {
                     tmpLine++;     // Skip pact the close quote
                     quoted = 0;    // Cancel quoted string processing
                     continue;
                  }
               }
               if (!quoted)
               {
                  quoted = *tmpLine++;
                  continue;
               }
            }
            //
            // Check for a backslash character which quotes just the next character.
            //
            if (*tmpLine =='\\')
            {
               tmpLine++;     // Skip the backslash and fall through to default case
            }
            //
            // Handle this character literally
            //
            if (*tmpLine != 0)
               *valueMunger++ = *tmpLine++;
         }
      }
      //
      // If we have a name, then we're done regardless of whether we have a value or not.
      //
      if (name && (*name != 0))
      {
         done = TRUE;
         *ppszName = STRDUP(name);
         if (value && (*value != 0))
         {
            if (valueEnd != NULL)
               *valueEnd = 0;                // Nuke trailing spaces
            *valueMunger = 0;                // Null terminate the value we're building.
            *ppszValue = STRDUP(value);
         }
      }
      FreePtr((void **)&line);
   }
   return done;
}

#endif

#ifdef FEATURE_INTERACT_WITH_SA_APP
boolean WriteAppListLine(IFile* file, AECHAR* appName, AEECLSID clsid, boolean hasShortcut, uint32 numLaunches, uint32 lastLaunch)
{
   boolean done = FALSE;
   uint32 numCharsWritten = 0;
   uint32 outputLineLen = 37;  // 4 for the seperators, 1 for the boolean, 2 for the newline, 10 for the clsid and 0x,
                              // 10 each for the numLaunches and lastLaunch
   char* outBuf = NULL;

   if (NULL == appName)
   {
      FARF(UTILS, ("No app name passed into WriteAppListLine"));
      return FALSE;
   }

   outputLineLen += WSTRLEN(appName);

   outBuf = (char*) MALLOC((outputLineLen + 1) * sizeof(char));

   if (NULL == outBuf)
   {
      FARF(UTILS, ("Not Enough memory to create output string"));
      return done;
   }

   SNPRINTF(outBuf, outputLineLen + 1, "%S|0x%X|%d|%d|%d\r\n", appName, clsid, hasShortcut?1:0, numLaunches, lastLaunch);

   outputLineLen = STRLEN(outBuf);


   numCharsWritten = IFILE_Write(file, outBuf, outputLineLen);

   if (numCharsWritten != outputLineLen)
   {
      // We have a problem.  Truncate the file so that we do not have a corrupt file
      // hanging around.
      //IFILE_Truncate(file, 0);
   }

   FreePtr((void**) &outBuf);

   return (numCharsWritten == outputLineLen);
}

boolean WriteAppList(IFileMgr* pIFileMgr)
{
   IFile* pIFile = NULL;
   AECHAR className[MAX_STR_CHARS];
   AEECLSID clsid = 0x44448888;

   pIFile = IFILEMGR_OpenFile(pIFileMgr, APPLIST_FILE, _OFM_READWRITE);   
   if (NULL == pIFile)
   {
      pIFile = IFILEMGR_OpenFile(pIFileMgr, APPLIST_FILE, _OFM_CREATE);
      if (NULL == pIFile)
      {
         FARF(UTILS, ("Unable to crete AppList file %s, result %d", APPLIST_FILE, IFILEMGR_GetLastError(pIFileMgr)));
         return FALSE;
      }
   }

   FARF(UTILS, ("Our current IFILEMGR last error is %d", IFILEMGR_GetLastError(pIFileMgr)));

   STRTOWSTR("Sample App Title", className, MAX_STR_CHARS * sizeof (AECHAR));

   if (!WriteAppListLine(pIFile, className, clsid, 1, 0, 0))
   {
      FARF(UTILS, ("Unable to Write to AppList %d", IFILEMGR_GetLastError(pIFileMgr)));
   }


   STRTOWSTR("Some Other App Name", className, MAX_STR_CHARS * sizeof (AECHAR));
   clsid = 0xABAB5F6F;


   if (!WriteAppListLine(pIFile, className, clsid, 0, 15, 333555889))
   {
      FARF(UTILS, ("Unable to Write to AppList %d", IFILEMGR_GetLastError(pIFileMgr)));
   }

   IFILE_Release(pIFile);

   return TRUE;
}


static uint32 ReadNextApp(AppListFile* pme, AEECLSID* clsid, boolean* hasShortcut, uint32* numLaunches, uint32* lastLaunch)
{
   boolean done = FALSE;
   char *line;
   char *name;
   char *szClsId;
   char *szHasShortcut;
   char *szNumLaunches;
   char *szLastLaunch;

   if ((NULL == clsid) || (NULL == hasShortcut) || (NULL == numLaunches) || (NULL == lastLaunch))
   {
      return EBADPARM;
   }

   while ((!done) && ((line = OEMCfg_GetLine(pme, pme->m_pIFile)) != NULL))
   {
      char *tmpLine;

      name = NULL;
      szClsId = NULL;
      szHasShortcut = NULL;
      szNumLaunches = NULL;
      szLastLaunch = NULL;

      //
      // Trim leading and trailing spaces
      //
      for (tmpLine = line; *tmpLine != 0; ++tmpLine);    // Find the end
      for (--tmpLine; (tmpLine > line) && ((*tmpLine == ' ') || (*tmpLine == '\t')); --tmpLine) *tmpLine = 0; // Nuke trailing spaces/tabs
      for (tmpLine = line; ((*tmpLine == ' ') || (*tmpLine == '\t')); ++tmpLine);      // Advance past leading spaces/tabs
      //
      // At this point, tmpLine points to first non-space and is trimmed.
      //
      while (tmpLine && (*tmpLine != 0))
      {
         if ((*tmpLine == '#') || (*tmpLine == ';'))        // '#' and ';' indicate rest of line is a comment
         {
            tmpLine = NULL;
            continue;
         }
         //
         // The line begins with a name.
         //
         if (!name)
         {
            name = tmpLine;
         }
         //
         // We're still processing the name. Do some small amount of checking for invalid stuff.
         //
         if (!szClsId)
         {
            //
            // Skip leading spaces before the name
            //
            if ((*name == ' ') || (*name == '\t'))
            {
               name++;
               tmpLine++;
            }
            //
            // When we hit an '|' character, start collecting the classId.
            //
            else if (*tmpLine == '|')
            {
               *tmpLine++ = 0;         // NULL terminate the name
               szClsId = tmpLine;        // Grab a pointer to the start of the value
            }
            else
            {
               tmpLine++;           // Middle of the name just accept whatever it is.
            }
         }
         //
         // Handle the various types of special value characters
         //
         else if (!szHasShortcut)
         {
            //
            // Check for leading spaces in the value and skip them
            //
            if ((*szClsId == ' ') || (*szClsId =='\t'))
            {
               //
               // Leading spaces
               //
               szClsId++;
               tmpLine++;
            }
            //
            // When we hit an '|' character, start collecting the hasShortcut.
            //
            else if (*tmpLine == '|')
            {
               *tmpLine++ = 0;         // NULL terminate the name
               szHasShortcut = tmpLine;        // Grab a pointer to the start of the hasShortcut
            }
            else
            {
               tmpLine++;           // Middle of the clsid just accept whatever it is.
            }  
         }
         else if (!szNumLaunches)
         {
            //
            // Check for leading spaces in the value and skip them
            //
            if ((*szHasShortcut == ' ') || (*szHasShortcut =='\t'))
            {
               //
               // Leading spaces
               //
               szHasShortcut++;
               tmpLine++;
            }
            //
            // When we hit an '|' character, start collecting the hasShortcut.
            //
            else if (*tmpLine == '|')
            {
               *tmpLine++ = 0;         // NULL terminate the name
               szNumLaunches = tmpLine;        // Grab a pointer to the start of the hasShortcut
            }
            else
            {
               tmpLine++;           // Middle of the clsid just accept whatever it is.
            }  
         }
         else if (!szLastLaunch)
         {
            //
            // Check for leading spaces in the value and skip them
            //
            if ((*szNumLaunches == ' ') || (*szNumLaunches =='\t'))
            {
               //
               // Leading spaces
               //
               szNumLaunches++;
               tmpLine++;
            }
            //
            // When we hit an '|' character, start collecting the hasShortcut.
            //
            else if (*tmpLine == '|')
            {
               *tmpLine++ = 0;         // NULL terminate the name
               szLastLaunch = tmpLine;        // Grab a pointer to the start of the hasShortcut
            }
            else
            {
               tmpLine++;           // Middle of the clsid just accept whatever it is.
            }  
         }
         else
         {
            //
            // Check for leading spaces in the value and skip them
            //
            if ((*szLastLaunch == ' ') || (*szLastLaunch =='\t'))
            {
               //
               // Leading spaces
               //

               szLastLaunch++;
               tmpLine++;
            }
            else
            {
               tmpLine++;          // Middle of the hasShortcut just accept whatever it is.
            }
         }
      }
      //
      // If we have a classid, then we're done regardless of whether we have a shortcut or not.
      //
      if (szClsId && (*szClsId != 0))
      {
         done = TRUE;
         *clsid = STRTOUL(szClsId, NULL, 16);
         if (szHasShortcut && (*szHasShortcut != 0))
         {
            *hasShortcut = (boolean) STRTOUL(szHasShortcut, NULL, 10);
         }
         else 
         {
            *hasShortcut = FALSE;
         }
         if (szNumLaunches && (*szNumLaunches != 0))
         {
            *numLaunches = STRTOUL(szNumLaunches, NULL, 10);
         }
         else 
         {
            *numLaunches = 0;
         }
         if (szLastLaunch && (*szLastLaunch != 0))
         {
            *lastLaunch = STRTOUL(szLastLaunch, NULL, 10);
         }
         else 
         {
            *lastLaunch = 0;
         }
      }

      FreePtr((void **)&line);
   }


   return done?SUCCESS:EFILEEOF;
}

boolean DoesAppHaveShortcut(IFileMgr* pIFileMgr, AEECLSID clsid)
{
   AppListFile appListFile;
   AEECLSID currentClsid;
   uint32 launches = 0;
   uint32 whenLastLaunch = 0;
   boolean hasShortcut = FALSE;

   appListFile.m_pCfgFileBuf = NULL;
   appListFile.m_iCfgFileBufPos = 0;
   appListFile.m_nCfgBytes = 0;

   appListFile.m_pIFile = IFILEMGR_OpenFile(pIFileMgr, APPLIST_FILE, _OFM_READ);

   if (NULL != appListFile.m_pIFile)
   {
      // the app list file exists, so parse it and see if we can find the clsid specified
      // and if it has a shortcut
      while (SUCCESS == ReadNextApp(&appListFile, &currentClsid, &hasShortcut, &launches, &whenLastLaunch))
      {
         if (currentClsid == clsid)
         {
            break;
         }
         hasShortcut = FALSE;
      }

   }


   FreeObj((void**)&(appListFile.m_pIFile));
   if (appListFile.m_pCfgFileBuf != NULL)
      FreePtr((void **) &(appListFile.m_pCfgFileBuf));

   return hasShortcut;
}

boolean GetAppProperties(IFileMgr* pIFileMgr, AEECLSID clsid, boolean* hasShortcut,
                         uint32* numLaunches, uint32* lastLaunch)
{
   AppListFile appListFile;
   AEECLSID currentClsid;
   boolean shortcut = FALSE;
   uint32 launches = 0;
   uint32 whenLastLaunch = 0;
   boolean bFound = FALSE;

   if ((NULL == hasShortcut) || (NULL == numLaunches) || (NULL == lastLaunch))
   {
      return FALSE;
   }

   appListFile.m_pCfgFileBuf = NULL;
   appListFile.m_iCfgFileBufPos = 0;
   appListFile.m_nCfgBytes = 0;

   appListFile.m_pIFile = IFILEMGR_OpenFile(pIFileMgr, APPLIST_FILE, _OFM_READ);

   if (NULL != appListFile.m_pIFile)
   {
      // the app list file exists, so parse it and see if we can find the clsid specified
      // and if it has a shortcut
      while (SUCCESS == ReadNextApp(&appListFile, &currentClsid, &shortcut, &launches, &whenLastLaunch))
      {
         if (currentClsid == clsid)
         {
            *hasShortcut = shortcut;
            *numLaunches = launches;
            *lastLaunch = whenLastLaunch;
            bFound = TRUE;
            break;
         }
         shortcut = FALSE;
         launches = 0;
         whenLastLaunch = 0;
      }

   }


   FreeObj((void**)&(appListFile.m_pIFile));
   if (appListFile.m_pCfgFileBuf != NULL)
      FreePtr((void **) &(appListFile.m_pCfgFileBuf));

   return bFound;
}
#endif




// Deep copy download item information.  Some pointer items are not
// copied simply because MobileShop doesn't use them.

DLItem* CopyDLItem(DLItem* pItem)
{
   int index = 0;
   DLItem* pCopy = NULL;

   FARF(UTILS, ("CopyDLItem()"));

   if (NULL == pItem)
   {
      FARF(UTILS, ("NULL item pointer in CopyDLItem()"));
      return pCopy;
   }
   if (DLI_CATEGORY == pItem->t)
   {
      FARF(UTILS, ("Attempt to copy category item in CopyDLItem()"));
      return pCopy;
   }

   pCopy = (DLItem*) MALLOC(sizeof(DLItem));
   if (NULL == pCopy)
   {
      FARF(UTILS, ("Failed to malloc DLItem"));
      return pCopy;
   }
   MEMSET(pCopy, 0, sizeof(DLItem));

   pCopy->t = pItem->t;
   pCopy->idParent = pItem->idParent;
   pCopy->iID = pItem->iID;
   pCopy->nIcon = pItem->nIcon;

   if (NULL != pItem->pszName)
   {
      pCopy->pszName = WSTRDUP(pItem->pszName);
      if (NULL == pCopy->pszName)
      {
         FARF(UTILS, ("Failed to WSTRDUP pszName"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   if (NULL != pItem->pszDescription)
   {
      pCopy->pszDescription = WSTRDUP(pItem->pszDescription);
      if (NULL == pCopy->pszDescription)
      {
         FARF(UTILS, ("Failed to WSTRDUP pszDescription"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   // currentLicense
   pCopy->currentLicense.pt = pItem->currentLicense.pt;
   pCopy->currentLicense.lt = pItem->currentLicense.lt;
   pCopy->currentLicense.dwExpire = pItem->currentLicense.dwExpire;

   // DemoOption
   pCopy->DemoOption.lt = pItem->DemoOption.lt;
   pCopy->DemoOption.Price.dwHandle = pItem->DemoOption.Price.dwHandle;
   pCopy->DemoOption.Price.dwValue = pItem->DemoOption.Price.dwValue;
   if (NULL != pItem->DemoOption.Price.pszPrice)
   {
      pCopy->DemoOption.Price.pszPrice =
         WSTRDUP(pItem->DemoOption.Price.pszPrice);
      if (NULL == pCopy->DemoOption.Price.pszPrice)
      {
         FARF(UTILS, ("Failed to WSTRDUP DemoOption.Price.pszPrice"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   // SubscriptionOption
   pCopy->SubscriptionOption.lt = pItem->SubscriptionOption.lt;
   pCopy->SubscriptionOption.Price.dwHandle =
      pItem->SubscriptionOption.Price.dwHandle;
   pCopy->SubscriptionOption.Price.dwValue =
      pItem->SubscriptionOption.Price.dwValue;
   if (NULL != pItem->SubscriptionOption.Price.pszPrice)
   {
      pCopy->SubscriptionOption.Price.pszPrice =
         WSTRDUP(pItem->SubscriptionOption.Price.pszPrice);
      if (NULL == pCopy->SubscriptionOption.Price.pszPrice)
      {
         FARF(UTILS, ("Failed to WSTRDUP SubscriptionOption.Price.pszPrice"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   // UpgradeOption
   pCopy->UpgradeOption.lt = pItem->UpgradeOption.lt;
   pCopy->UpgradeOption.Price.dwHandle = pItem->UpgradeOption.Price.dwHandle;
   pCopy->UpgradeOption.Price.dwValue = pItem->UpgradeOption.Price.dwValue;
   if (NULL != pItem->UpgradeOption.Price.pszPrice)
   {
      pCopy->UpgradeOption.Price.pszPrice =
         WSTRDUP(pItem->UpgradeOption.Price.pszPrice);
      if (NULL == pCopy->UpgradeOption.Price.pszPrice)
      {
         FARF(UTILS, ("Failed to WSTRDUP UpgradeOption.Price.pszPrice"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   // PurchaseOption
   pCopy->PurchaseOption.lt = pItem->PurchaseOption.lt;
   pCopy->PurchaseOption.nPrices = pItem->PurchaseOption.nPrices;
   for (index = 0; index < pCopy->PurchaseOption.nPrices; index++)
   {
      pCopy->PurchaseOption.pPrices[index].dwHandle =
         pItem->PurchaseOption.pPrices[index].dwHandle;
      pCopy->PurchaseOption.pPrices[index].dwValue =
         pItem->PurchaseOption.pPrices[index].dwValue;
      if (NULL != pItem->PurchaseOption.pPrices[index].pszPrice)
      {
         pCopy->PurchaseOption.pPrices[index].pszPrice =
            WSTRDUP(pItem->PurchaseOption.pPrices[index].pszPrice);
         if (NULL == pCopy->PurchaseOption.pPrices[index].pszPrice)
         {
            FARF(UTILS, ("Failed to WSTRDUP PurchaseOption.Price[n].pszPrice"));
            FreeDLItem(pCopy);
            pCopy = NULL;
            return pCopy;
         }
      }
   }

   // AutoInstallOption
   pCopy->AutoInstallOption.lt = pItem->AutoInstallOption.lt;
   pCopy->AutoInstallOption.Price.dwHandle =
      pItem->AutoInstallOption.Price.dwHandle;
   pCopy->AutoInstallOption.Price.dwValue =
      pItem->AutoInstallOption.Price.dwValue;
   if (NULL != pItem->AutoInstallOption.Price.pszPrice)
   {
      pCopy->AutoInstallOption.Price.pszPrice =
         WSTRDUP(pItem->AutoInstallOption.Price.pszPrice);
      if (NULL == pCopy->AutoInstallOption.Price.pszPrice)
      {
         FARF(UTILS, ("Failed to WSTRDUP AutoInstallOption.Price.pszPrice"));
         FreeDLItem(pCopy);
         pCopy = NULL;
         return pCopy;
      }
   }

   pCopy->bEULA = pItem->bEULA;
   pCopy->nPackages = pItem->nPackages;
   // We don't use pPkgList so don't bother copying
   pCopy->dwRAMRequired = pItem->dwRAMRequired;
   pCopy->dwFlags = pItem->dwFlags;
   // We don't use pszUses or pszActionType

   return pCopy;
}






// Deep free download item information.  Some pointer members are not
// deep copied by CopyDLItem, in which case we expect those pointers to
// be NULL here.
//
void FreeDLItem(DLItem* pItem)
{
   int index;

   FARF(UTILS, ("FreeDLItem()"));

   if (NULL == pItem)
   {
      FARF(UTILS, ("NULL item pointer in FreeDLItem()"));
      return;
   }
   if (DLI_CATEGORY == pItem->t)
   {
      FARF(UTILS, ("Attempt to free category item in FreeDLItem()"));
      return;
   }

   if (NULL != pItem->pszName)
   {
      FREE(pItem->pszName);
      pItem->pszName = NULL;
   }

   if (NULL != pItem->pszDescription)
   {
      FREE(pItem->pszDescription);
      pItem->pszDescription = NULL;
   }

   // DemoOption price string
   if (NULL != pItem->DemoOption.Price.pszPrice)
   {
      FREE(pItem->DemoOption.Price.pszPrice);
   }

   // SubscriptionOption price string
   if (NULL != pItem->SubscriptionOption.Price.pszPrice)
   {
      FREE(pItem->SubscriptionOption.Price.pszPrice);
   }

   // UpgradeOption price string
   if (NULL != pItem->UpgradeOption.Price.pszPrice)
   {
      FREE(pItem->UpgradeOption.Price.pszPrice);
   }
   
   // PurchaseOption price strings
   for (index = 0; index < pItem->PurchaseOption.nPrices; index++)
   {
      if (NULL != pItem->PurchaseOption.pPrices[index].pszPrice)
      {
         FREE(pItem->PurchaseOption.pPrices[index].pszPrice);
      }
   }

   // AutoInstallOption price string
   if (NULL != pItem->AutoInstallOption.Price.pszPrice)
   {
      FREE(pItem->AutoInstallOption.Price.pszPrice);
   }

   // We don't use any of these so they should be NULL
   if (NULL != pItem->pPkgList)
   {
      FARF(UTILS, ("Unexpected non-NULL pPkgList in FreeDLItem()"));
   }
   if (NULL != pItem->pszUses)
   {
      FARF(UTILS, ("Unexpected non-NULL pszUses in FreeDLItem()"));
   }
   if (NULL != pItem->pszActionType)
   {
      FARF(UTILS, ("Unexpected non-NULL pszActionType in FreeDLItem()"));
   }

   FREE(pItem);

   return;
}





// Deep copy Price information. This is needed so that if we suspend
// and then resume we don't need to try to re-create the price info
// sepcifically the string representing the actual price
Price* CopyPrice(Price* pPrice)
{
   Price* pCopy = NULL;

   FARF(UTILS, ("CopyPrice()"));

   if (NULL == pPrice)
   {
      FARF(UTILS, ("CopyPrice passed a NULL pointer"));
      return pCopy;
   }

   pCopy = (Price*) MALLOC(sizeof(Price));

   if (NULL == pCopy)
   {
      FARF(UTILS, ("Failed to malloc Price"));
      return pCopy;
   }

   MEMSET(pCopy, '0', sizeof(Price));

   pCopy->dwHandle = pPrice->dwHandle;
   pCopy->dwValue = pPrice->dwValue;
   if (NULL != pPrice->pszPrice)
   {
      pCopy->pszPrice = WSTRDUP(pPrice->pszPrice);
      if (NULL == pCopy->pszPrice)
      {
         FARF(UTILS, ("Failed to STRDUP pszPrice"));
         FreePrice(&pCopy);
         return pCopy;
      }
   }

   return pCopy;
}

// Deep free our copy of a Price.  
//
void FreePrice(Price** ppPrice)
{
   FARF(UTILS, ("FreePrice"));

   if (NULL == ppPrice || NULL == *ppPrice)
   {
      FARF(UTILS, ("NULL price pointer in FreePrice"));
      return;
   }

   if (NULL != (*ppPrice)->pszPrice)
   {
      FREE((*ppPrice)->pszPrice);
      (*ppPrice)->pszPrice = NULL;
   }

   FREE(*ppPrice);
   *ppPrice = NULL;

   return;
}

// Copy a null terminated int16 Array.  If 
// the src array is NULL, the return is NULL, also if there 
// is not enough memory, the return is NULL
uint16* CopyUInt16Array(uint16* src)
{
   uint16* dest = NULL;
   if (src)
   {
      size_t numItems = 0;

      // Calculate number of items
      while(*(src + numItems))
      {
         numItems++;
      }
      if (numItems > 0)
      {
         dest = (uint16*)MALLOC((numItems + 1) * sizeof(uint16));
         if (NULL != dest)
         {
            (void)MEMCPY(dest, src, numItems * sizeof(uint16));
            dest[numItems] = 0;
         }
      }
   }
   return dest;
}





//===========================================================================
//
// DESCRIPTION
//    Determines whether BAM is allowed to run on the specified display
//    based on the OEMCFG information available (or its default).  If the
//    specified display ID is APP_MGR_CURRENT_DISPLAY the check is made on
//    the current display (see code for how the current display is
//    determined).

//
// PARAMETERS
//
//    pIShell - pointer to ISHELL interface that can be used to
//    instantiate an IFlip.
//
//    bHasMultipleDisplays - a flag indicating whether the device we're
//    running on has multiple displays, normally based on a compile time
//    configuration item
//
//    wEnabledDisplays - a bit field indicating which displays are
//    enabled on this device, normally taken from an OEMCfg item.
//
//    clsDisplay - display ID of the display to be checked, or
//    APPMGR_CURRENT_DISPLAY to indicate that the current display
//    should be checked.  The only acceptable values other than
//    APPMGR_CURRENT_DISPLAY are:
//
//    AEECLSID_DISPLAY1
//    AEECLSID_DISPLAY2
//    AEECLSID_DISPLAY3
//    AEECLSID_DISPLAY4
//
//
//===========================================================================
boolean DisplayEnabled(IShell* pIShell, boolean bHasMultipleDisplays,
   uint16 wEnabledDisplays, AEECLSID clsDisplay)
{
   int displayIndex = 0;
   boolean result = FALSE;

   // Default to display 1
   AEECLSID checkDisplayID = AEECLSID_DISPLAY1;

   // If APPMGR_CURRENT_DISPLAY is specified use the current display's ID.
   // Stick with the default if unable to determine the current display
   if (APPMGR_CURRENT_DISPLAY == clsDisplay)
   {
#ifdef FEATURE_HAS_MULTIPLE_DISPLAYS
      if (TRUE == bHasMultipleDisplays)
      {
         // Use the IFlip interface to determine the current display ID.
         // If we can't use IFlip for any reason fall back to the default.
         IFlip* pIFlip = NULL;
         int tempResult = CreateInstance(pIShell,
            AEECLSID_FLIP, (void **)&pIFlip);
         if (SUCCESS == tempResult)
         {
            int32 numDisplays = 0;
            tempResult = IFLIP_GetDisplayList(pIFlip, NULL, &numDisplays);
            if (SUCCESS == tempResult)
            {
               if (0 < numDisplays)
               {
                  AEECLSID* idList = (AEECLSID*)
                     MALLOC(sizeof(AEECLSID) * numDisplays);
                  if (NULL != idList)
                  {
                     tempResult = IFLIP_GetDisplayList(pIFlip, (void*)idList,
                        &numDisplays);
                     if (SUCCESS == tempResult)
                     {
                        FARF(UTILS, ("DisplayEnabled() using IFlip value"));
                        checkDisplayID = idList[0];
                     }
                     else
                     {
                        FARF(UTILS, ("Second GetDisplayList() failed"));
                     }
                     FreePtr((void**) &idList);
                  }
                  else
                  {
                     FARF(UTILS, ("Unable to allocate space for display list"));
                  }
               }
            }
            else
            {
               FARF(UTILS, ("First GetDisplayList() failed"));
            }
            FreeObj((void**)& pIFlip);
         }
         else
         {
            FARF(UTILS, ("Unable to create IFlip"));
         }
      }
      else
      {
         // if there aren't multiple displays stick with the default
      }
#else
   // If the multiple display feature isn't supported we assume that
   // there is one display and use the default.
#endif
   }
   else
   {
      // Use the specified display ID
      checkDisplayID = clsDisplay;
   }

   // At this point we should be looking at a well known display ID.
   // Map it to a value we can use in the OEMCFG bit twiddling macros.
   switch (checkDisplayID)
   {
      case AEECLSID_DISPLAY1:
         FARF(UTILS, ("Check enabled: display 1"));
         displayIndex = 1;
         break;
      case AEECLSID_DISPLAY2:
         FARF(UTILS, ("Check enabled: display 2"));
         displayIndex = 2;
         break;
      case AEECLSID_DISPLAY3:
         FARF(UTILS, ("Check enabled: display 3"));
         displayIndex = 3;
         break;
      case AEECLSID_DISPLAY4:
         FARF(UTILS, ("Check enabled: display 4"));
         displayIndex = 4;
         break;
      default:
         FARF(UTILS, ("Invalid display index"));
         return result;
   }

   // Ultimately the result is based on OEM config info...
   result = IS_BAM_ENABLED_DISPLAY(displayIndex, wEnabledDisplays);
   return result;
}


///////////////////////////////////////////////////////////////////////////
// GetKeyPresValues:  This takes in the list of values to map key presses
//    to for displays that don't have the necessary keys.
//    listOfKeys looks like 1234,1234,1234,1234
//    where the first value is the int value of the AVK_ value that we should
//    treat as the up key, the second, as the down key, the third as the select
//    key and the 4th as the clr key
///////////////////////////////////////////////////////////////////////////
boolean GetKeyPressValues(const char* listOfKeys, size_t strLen, uint16* upKey, uint16* downKey, uint16* selectKey, uint16* clrKey)
{
   const char* currentPosition = NULL;
   char* walker = NULL;
   uint16 upValue = AVK_UP;
   uint16 downValue = AVK_DOWN;
   uint16 selectValue = AVK_SELECT;
   uint16 clrValue = AVK_CLR;

   if ((NULL == listOfKeys) || (NULL == upKey) || (NULL == downKey) || (NULL == selectKey) || (NULL == clrKey))
   {
      return FALSE;
   }

   currentPosition = listOfKeys;

   // Strip off any leading chars that we don't expect.
   while ((('0' > *currentPosition) || ('9' < *currentPosition)) && (',' != *currentPosition) 
          && ('\0' != *currentPosition) &&((size_t)(currentPosition - listOfKeys) < strLen))
   {
      ++currentPosition;
   }
   upValue = (uint16)STRTOUL(currentPosition, &walker, 10);

   // Ignore any white space or junk.  Look for the ,
   while (',' != *walker && ((size_t)(walker - listOfKeys) < strLen))
   {
      ++walker;
   }
   currentPosition = walker + 1;

   // Strip off any leading chars that we don't expect.
   while ((('0' > *currentPosition) || ('9' < *currentPosition)) && (',' != *currentPosition) 
          && ('\0' != *currentPosition) &&((size_t)(currentPosition - listOfKeys) < strLen))
   {
      ++currentPosition;
   }
   if ((size_t)(currentPosition - listOfKeys) < strLen)
   {
      downValue = (uint16)STRTOUL(currentPosition, &walker, 10);
   }
   
   // Ignore any white space or junk.  Look for the ,
   while (',' != *walker && ((size_t)(walker - listOfKeys) < strLen))
   {
      ++walker;
   }
   currentPosition = walker + 1;

   // Strip off any leading chars that we don't expect.
   while ((('0' > *currentPosition) || ('9' < *currentPosition)) && (',' != *currentPosition) 
          && ('\0' != *currentPosition) &&((size_t)(currentPosition - listOfKeys) < strLen))
   {
      ++currentPosition;
   }
   if ((size_t)(currentPosition - listOfKeys) < strLen)
   {
      selectValue = (uint16)STRTOUL(currentPosition, &walker, 10);
   }

   // Ignore any white space or junk.  Look for the ,
   while (',' != *walker && ((size_t)(walker - listOfKeys) < strLen))
   {
      ++walker;
   }
   currentPosition = walker + 1;

   // Strip off any leading chars that we don't expect.
   while ((('0' > *currentPosition) || ('9' < *currentPosition)) && (',' != *currentPosition) 
          && ('\0' != *currentPosition) &&((size_t)(currentPosition - listOfKeys) < strLen))
   {
      ++currentPosition;
   }
   if ((size_t)(currentPosition - listOfKeys) < strLen)
   {
      clrValue = (uint16)STRTOUL(currentPosition, &walker, 10);
   }


   // Assign the values that we got.  If we got invalid numbers
   // use the default for that type of key
   if ((upValue <= AVK_FIRST) || (upValue >= AVK_LAST))
   {
      *upKey = AVK_UP;
   }
   else
   {
      *upKey = upValue;
   }
  
   if ((downValue <= AVK_FIRST) || (downValue >= AVK_LAST))
   {
      *downKey = AVK_DOWN;
   }
   else
   {
      *downKey = downValue;
   }
   
   if ((selectValue <= AVK_FIRST) || (selectValue >= AVK_LAST))
   {
      *selectKey = AVK_SELECT;
   }
   else
   {
      *selectKey = selectValue;
   }
  
   if ((clrValue <= AVK_FIRST) || (clrValue >= AVK_LAST))
   {
      *clrKey = AVK_CLR;
   }
   else
   {
      *clrKey = clrValue;
   }
   
   return TRUE;

}

#ifdef USE_MENUCTL_EX
void MenuDrawItemCB(void * pvContext, AEEODCBData* is)
{
  switch (is->nAction)
  {
    case ODA_INFO_SB:
      {
        AEEScrollBarInfo *pinfo;
        pinfo = (AEEScrollBarInfo*)is->nActionInfo;
 
        pinfo->wWidth = SCROLLBAR_WIDTH; //14;

        //pinfo->cScrollbar = MAKE_RGB(0, 0, 255);
        //pinfo->cThumb = MAKE_RGB(255, 0, 0);

        //pinfo->pszResImage = BREWAPPMGRLN_RES_FILE;
        //pinfo->wImageThumb = IDB_SCROLLBAR_THUMB;
        //pinfo->wImageScrollbar = IDB_SCROLLBAR;
      }
      break;
    case ODA_MEASURE_SB:
      // sets scroll bar width
     // is->rc.dx = SCROLLBAR_WIDTH;
      break;
    case ODA_INFO_ITEM:
      {
        AEEMenuExItemInfo* pinfo;
        pinfo = (AEEMenuExItemInfo*)is->nActionInfo;

        pinfo->cText = MAKE_RGB(0,0,0);
        pinfo->cTextSel = MAKE_RGB(255,255,255);
        //pinfo->pszResImage = BREWAPPMGRLN_RES_FILE;
        //pinfo->wImageSelect = IDB_MENUITEM_SEL;
        //pinfo->wImage = IDB_MENUITEM_BACK;
      }
      break;
    default:
      break;
  }
}
#endif//USE_MENUCTL_EX

AEECLSID GetMenuCtlClassID(boolean bTouchEnabled)
{
  AEECLSID clsID = AEECLSID_MENUCTL;

#ifdef USE_MENUCTL_EX
  clsID = bTouchEnabled ? AEECLSID_MENUCTLEX : AEECLSID_MENUCTL;
#endif // USE_MENUCTL_EX

  return clsID;
}

AEECLSID GetIconViewClassID(boolean bTouchEnabled)
{
  AEECLSID clsID = AEECLSID_ICONVIEWCTL;

#ifdef USE_MENUCTL_EX
  clsID = bTouchEnabled ? AEECLSID_ICONVIEWCTLEX : AEECLSID_ICONVIEWCTL;
#endif // USE_MENUCTL_EX

  return clsID;
}

AEECLSID GetSoftKeyClassID(boolean bTouchEnabled)
{
  AEECLSID clsID = AEECLSID_SOFTKEYCTL;

#ifdef USE_MENUCTL_EX
  clsID = bTouchEnabled ? AEECLSID_SOFTKEYCTLEX : AEECLSID_SOFTKEYCTL;
#endif // USE_MENUCTL_EX

  return clsID;
}

// this function will create an object instance based on the class ID
int CreateInstance(IShell * pIShell, AEECLSID clsID, void ** ppobj)
{
  int nRet;

#if defined(USE_MENUCTL_EX) && defined(AEEMENUCLT_STATIC)
   if(clsID == AEECLSID_SOFTKEYCTLEX ||
      clsID == AEECLSID_LISTCTLEX ||
      clsID == AEECLSID_MENUCTLEX ||
      clsID == AEECLSID_ICONVIEWCTLEX)
   {
      nRet = MenuCtlClsCreateInstance(clsID, pIShell, NULL,  ppobj);
   }
   else
#endif //defined(USE_MENUCTL_EX) && defined(AEEMENUCLT_STATIC)
   {
     nRet = ISHELL_CreateInstance(pIShell, clsID, ppobj);
   }

   return nRet;
}

// return TRUE if pt is in/on rc, otherwise return FALSE
boolean IsInRect(int16 wXp, int16 wYp, AEERect* pRect)
{
	if(pRect && ((wXp >= pRect->x) && (wXp <= (pRect->x + pRect->dx)))
		&& ( (wYp >= pRect->y) && (wYp <= (pRect->y + pRect->dy))))
  {
		return TRUE;
  }
	return FALSE;
}
// return TRUE if menu is active and the pt is in the menu rc
boolean IsInMenuCtl(int16 wXp, int16 wYp, IMenuCtl* pMenu)
{
  if (pMenu) // && IMENUCTL_IsActive(pMenu))
  {
    AEERect rcMenu;

    if(IMENUCTL_GetItemCount(pMenu) > 0)
    {
      MEMSET(&rcMenu, 0, sizeof(AEERect));
      IMENUCTL_GetRect(pMenu, &rcMenu);
      return IsInRect(wXp, wYp, &rcMenu);
    }
  }

  return FALSE;
}

boolean IsInStaticCtl(int16 wXp, int16 wYp, IStatic* pStatic)
{
   if (pStatic)
   {
      AEERect rcStatic;

      MEMSET(&rcStatic, 0, sizeof(AEERect));
      ISTATIC_GetRect(pStatic, &rcStatic);
      return IsInRect(wXp, wYp, &rcStatic);
   }

   return FALSE;
}

// this function returns rectangle around all visiable menu items
boolean GetListViewMenuRect(IMenuCtl* pMenu, AEERect* prc)
{
  int i, y;
  int cnt;
  int nFirst = -1;
  AEERect rcItem, rcMenu;

  if(NULL == pMenu || NULL == prc)
  {
    return FALSE;
  }

  if(0 == (cnt = IMENUCTL_GetItemCount(pMenu)))
  {
    return FALSE;
  }

  IMENUCTL_GetRect(pMenu, &rcMenu);
  // calculate the last visable item rectangle
  y = 0;
  for(i=0; i < cnt; i++)
  {
    MEMSET(&rcItem, 0,  sizeof(AEERect));
    IMENUCTL_GetItemRect(pMenu, i, &rcItem);
    if(nFirst >= 0 && ((rcItem.y + rcItem.dy) > (rcMenu.y + rcMenu.dy) || ((rcItem.y + rcItem.dy) == 0)))
    {
      break;
    }
    if(nFirst < 0 && (rcItem.y + rcItem.dy) > 0)
    {
      // first visiable item
      nFirst = i;
    }
    y = rcItem.y + rcItem.dy;
  }

  if(nFirst >= 0)
  {
    IMENUCTL_GetItemRect(pMenu, nFirst, &rcItem);
 
    prc->x = rcMenu.x;
    prc->dx = rcMenu.dx;
    prc->y = rcMenu.y;
    prc->dy = y - prc->y;

    return TRUE;
  }

  return FALSE;
}

// given an instance of IFlip, return to the user the new display classid
// that we should use.
boolean GetDisplayToUse(IFlip *pIFlip, AEECLSID* newDisplayId)
{
   int32    dwNumDisplays = 0;
   AEECLSID *pClsList = NULL;
   boolean  bRet = FALSE;
   if (NULL == pIFlip || NULL == newDisplayId)
   {
      return FALSE;
   }

   if ((SUCCESS == IFLIP_GetDisplayList(pIFlip, NULL, &dwNumDisplays)) && (0 != dwNumDisplays))
   {
      pClsList = (AEECLSID*) MALLOC(sizeof(AEECLSID) * dwNumDisplays);

      if (NULL == pClsList)
      {
         FARF(UTILS, ("Out of memory"));
         return FALSE;
      }

      if (SUCCESS == IFLIP_GetDisplayList(pIFlip, (void*)pClsList, &dwNumDisplays))
      {

         FARF(UTILS, ("IFlip told me to use %d", pClsList[0]));
         *newDisplayId = pClsList[0];
         bRet = TRUE;
      }
   }

   FreePtr((void**)&pClsList);
   return bRet;
}

#ifdef FEATURE_HAPTIC_SUPPORT
void PlyHapticResponse(ISound* iSoundPtr,AEESoundToneData toneData )
{
   // Validate the toneData; toneData.eTone and toneData.wDuration are read from oemcfg.txt.
   // OEM's would disable the haptic feature by removing the "HapticTone" and "HapticDuration" or by assigning value 0 to both:
   
   // toneData.eTone ==0 or toneData.wDuration == 0, indicates OEM's have disabled the Haptic feature or invlid entry for haptic parameters
   // In both cases return, without calling ISound_PlayTone
   if(toneData.eTone == 0 || toneData.wDuration == 0)
   {
      FARF(UTILS, ("Unable to play sound : Invalid toneData"));
      return;
   }
   if(iSoundPtr)
   {
      ISound_PlayTone(iSoundPtr, toneData);
   }
   else
   {
      FARF(UTILS, ("Unable to play sound : iSoundPtr is NULL"));
   }
}
#endif


