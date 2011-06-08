#include "AEE.h"
#include "AEEClassIDs.h"
#include "AEEDate.h"
#include "AEESecurity.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AppMgrUtils.h"
#include "AppMgrSettings.h"

/*===========================================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AppMgrUtils.c

SERVICES:  This files has common stuff between AppMgr and MobileShop
   applets.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

static void GetString(
   char* pszString,
   int*  pnSize);

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
 
   ISHELL_CreateInstance(ps,AEECLSID_DATECTL,(void **)&pd);

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

===========================================================================*/
void FormatMem(uint32 dwSize, AECHAR * pszDest,boolean bWrap)
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

   nGB = (uint16)(dwSize / ONE_GB);
   nMB = (uint16)((dwSize - ((uint32)nGB * ONE_GB)) / ONE_MB);
   nK = (uint16)((dwSize - ((uint32)nMB * ONE_MB)) / ONE_K);

   if(nGB)
      SPRINTF(psz,(const char *)(nMB ? "%d.%02.2dGB" : "%dGB"), nGB,(nMB / 10));
   else if(nMB)
      SPRINTF(psz,(const char *)(nK ? "%d.%02.2dMB" : "%dMB"), nMB,(nK / 10));
   else 
      SPRINTF(psz,"%dK",nK);

   if(bWrap)
      STRCAT(psz,")");

   STR_TO_WSTR(szText,pszDest,10 * sizeof(AECHAR));
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
      IBASE_Release(((IBase *)(*ppObj)));
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
   DBGPRINTF("GetExpireString %d %d %d",dwNow,dwValue,lt);
   // Get current time if not specified
   if(!dwNow)
      dwNow = GET_SECONDS();

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
            DBGPRINTF("GetExpireString %d",bAdjust);
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
//		Sets style and color of the menu
//
//PARAMETERS
//		pMenu : IMenuCtl pointer
//    nMenuCls : Menu Class
//    nColorDepth : Device Color Depth
//
//Return Value:
//    none
//		
//===========================================================================
void SetMenuStyleAndColor(IMenuCtl* pMenu, AEECLSID nMenuCls, uint16 nColorDepth)
{
#if !OEM_USE_DEFAULT_COLOR_STYLE
	AEEItemStyle   sel,normal;
	AEEMenuColors  col;
#endif

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

   // OEM_USE_DEFAULT_COLOR_STYLE is TRUE, return without making any changes
   // in color and style
   if (OEM_USE_DEFAULT_COLOR_STYLE)
      return;

#if !OEM_USE_DEFAULT_COLOR_STYLE
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
         case AEECLSID_MENUCTL:
            // Set Menu Colors
	         col.cBack =             COLOR_MENU_BACKGROUND;
	         col.cSelBack =          COLOR_MENU_SELECT_BACKGROUND;
	         col.cSelText =          COLOR_MENU_SELECT_TEXT;
            break;

         case AEECLSID_SOFTKEYCTL:
	         // Set Softkey/IConView Colors
	         col.cBack =             COLOR_SK_BACKGROUND;
	         col.cSelBack =          COLOR_SK_SELECT_BACKGROUND;
	         col.cSelText =          COLOR_SK_SELECT_TEXT;
            break;

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
#endif
}

//===========================================================================
// 
// DESCRIPTION
//		Gets item ID corresponding to the specified class ID
//
// PARAMETERS
//		pIDownload: pointer to IDownload interface
//    cls: class ID
//
// Return Value:
//		None
//
//===========================================================================
DLITEMID GetItemIDFromClassID(IDownload * pIDownload, AEECLSID cls)
{
   uint32* pIDs;
   uint32* pdw;
   uint32 dw;
   AppModInfo* pai;

   pIDs = IDOWNLOAD_GetAppIDList(pIDownload);
   if(pIDs)
   {
      for(pdw = pIDs; (dw = *pdw) != 0; pdw++)
      {
         //Its not a valid app (with valid license), no point checking for the license
         if (dw >= FAKE_MODULE_ID)
            continue;

         pai = IDOWNLOAD_GetModInfo(pIDownload, dw);
         if (pai)
         {
            uint16 index;
            for (index = 0; index < pai->nApps; index++)
            {
               if (pai->apps[index].cls == cls)
                  return (dw);
            }
         }
      }
   }
   return 0;
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
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_MD5, (void**)&piHash);
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
