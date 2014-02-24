/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEDate.c

SERVICES:  AEE Date Control Interface

GENERAL DESCRIPTION:  Provides support for IDateCtl services
in AEE.

PUBLIC CLASSES AND STATIC FUNCTIONS: IDateCtl

Copyright  1999-2002,2007 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE_OEM.h"
#include "AEEStdLib.h"
#ifndef CUST_EDITION
#include "AEEDate_priv.h"
#endif
#include "AEEControls_res.h"
#ifdef CUST_EDITION
#include "Appscommon.h"
#include "appscommonimages.brh"
#include "AEEDate.h"
#endif
#include "AEEPointerHelpers.h"
#include "err.h"
/*===========================================================================

DEFINITIONS

===========================================================================*/
#define DRAW_GIGNUM

#define WRAP(val,mod)      ((val)+(mod))%(mod)
#define IS_LEAP(y)         ((((y)%4)==0) && (((y)%100) || (((y)%400)==0)))
#define DATE_BUFFER_MIN    (12)
#define TITLEHEIGHT        (pme->m_nFontTitleHeight)
#define LINEHEIGHT         (pme->m_nFontLineHeight)

#define MAX_WEEK_ROWS      (6)
#define DAYS_PER_WEEK      (7)

#define FLDPAD             (4)
#define IMAGE_WIDTH        (4)

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

#ifdef CUST_EDITION	
/* 背景色*/
#define DATECTL_BG_COLOR                      (RGB_BLACK)
/* 平日背景填充色*/
#define DATECTL_NORMALDAY_BG_COLOR  (MAKE_RGB(0x70, 0x70, 0x70))
/* 节假日背景填充色*/
#define DATECTL_SPECIALDAY_BG_COLOR  (RGB_WHITE)
/* 平日字体颜色*/
#define DATECTL_NORMALDAY_COLOR        (RGB_WHITE)
#define DATECTL_UNDELINE_COLOR            (RGB_WHITE)

#define DATECTL_SPECIALDAY_COLOR        (MAKE_RGB(0xD4, 0x7C, 0x0A))
#define DATECTL_TODAY_FRAME_COLOR     (MAKE_RGB(0x78, 0xF0, 0x78))

#define DATECTL_FONT_HEIGHT       10    //12
#endif /*CUST_EDITION*/

extern int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags);

/*===========================================================================


Class Definitions

===========================================================================*/

OBJECT(DateCtl)
{
   DECLARE_VTBL(IDateCtl)

   // private object data
   long           m_nRefs;          // reference count
   AEERect        m_rc;             // control rectangle
   
   IShell *       m_pIShell;        // shell interface
   IDisplay *     m_pIDisplay;      // display interface
   IImage *       m_pFont;
#ifdef CUST_EDITION	   
#ifdef FEATURE_CALENDAR_USE_STYLE
   IImage *       m_pBgImage;
#endif
#endif /*CUST_EDITION*/  
   int            m_nFontTitleHeight;
   int            m_nFontLineHeight;
   AEERect        m_rcGrid;

   AECHAR *       m_pTitle;
   AEEFont        m_fntTitle;
   AEEFont        m_fntText;

   int            m_nStartDayOfWeek;
   int            m_nDayOfWeek;
   int            m_nYear;         // e.g. 1999, 2000
   int            m_nMonth;        // range: 1 - 12
   int            m_nDay;          // range: 1 - 31
   long           m_lJulianDay;    // 
#ifdef CUST_EDITION	
   long           m_Today;          
   long           m_StartJulDate;
   long           m_EndJulDate;
   
   boolean        m_ActRange;
   boolean        m_bDrawDateText;
#endif /*CUST_EDITION*/   
   int            m_nCurrentField;
   int            m_nNumFields;

   int            m_nYearFldWidth;
   int            m_nMonFldWidth;
   int            m_nDayFldWidth;

   uint32         m_dwMDayMask;
   uint32         m_dwProps;
   uint16         m_nCmdId;
   int            m_nHitData; // -1 means nothing
   
   flg            m_bActive : 1;    // control is active flag
   flg            m_bMonthView : 1; // control is a month-view control
   flg            m_bSendCmd : 1;
   flg            m_bMonthChanged:1;
   flg            m_bValidDate:1;
};

//---------------------------------------------------------------------
// Function Prototypes
//---------------------------------------------------------------------

static uint32      IDateCtl_AddRef(IDateCtl * po);
static uint32      IDateCtl_Release(IDateCtl * po);
static boolean     IDateCtl_HandleEvent(IDateCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean     IDateCtl_Redraw(IDateCtl * po);
static void        IDateCtl_SetActive(IDateCtl * po, boolean bActive);
static boolean     IDateCtl_IsActive(IDateCtl * po);
static void        IDateCtl_SetRect(IDateCtl * po, const AEERect * prc);
static void        IDateCtl_GetRect(IDateCtl * po, AEERect * prc);
static void        IDateCtl_SetProperties(IDateCtl * po, uint32 nProperties);
static uint32      IDateCtl_GetProperties(IDateCtl * po);
static void        IDateCtl_Reset(IDateCtl * po);
static boolean     IDateCtl_SetJulianDay(IDateCtl * po, int32 lJulDate);
static int32       IDateCtl_GetJulianDay(IDateCtl * po);
static boolean     IDateCtl_SetTitle(IDateCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText);
static boolean     IDateCtl_SetDate(IDateCtl * po, int nYear, int nMonth, int nDay);
static boolean     IDateCtl_GetDate(IDateCtl * po, int * pnYear, int * pnMonth, int * pnDay);
static uint16      IDateCtl_GetDayOfWeek(IDateCtl * po);
static void        IDateCtl_EnableCommand(IDateCtl * po, boolean bEnable, uint16 nCmdId);
static void        IDateCtl_SetActiveDayMask(IDateCtl * po, uint32 dwMask);
static AECHAR *    IDateCtl_GetDayString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars);
static AECHAR *    IDateCtl_GetMonthString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars);
static boolean     IDateCtl_GetDateString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars, uint32 dwDateFormat);
static void        IDateCtl_SetFont(IDateCtl * po, AEEFont fntText, AEEFont fntTitle);
static boolean     IDateCtl_GetFont(IDateCtl * po, AEEFont *pfntText, AEEFont *pfntTitle);
static void        IDateCtl_SizeToFit(IDateCtl * po, AEERect *prc);
#ifdef CUST_EDITION	
static void        IDateCtl_SetToday(IDateCtl * po, int32 lJulDate);
static boolean     IDateCtl_SetDateRange(IDateCtl * po, int32 StartJulDate, int32 EndJulDate);
static void        IDateCtl_EnableDateRange(IDateCtl * po, boolean ActRange);

static boolean     IDateCtl_SetJulianDayEx(IDateCtl * po, int32 lJulDate);
#endif /*CUST_EDITION*/
static void        DateCtl_DisplayDateText(DateCtl * pme);
static void        DateCtl_SetGridRect(DateCtl * pme);
static void        DateCtl_Digit(DateCtl * pme, int nDigit, int x, int y);
static void        DateCtl_DrawDay(DateCtl * pme, int nDay, AEERect * prc);
static void        DateCtl_DrawDayEx(DateCtl * pme, int nDay, AEERect * prc);

#ifdef CUST_EDITION	
static boolean     DateCtl_IsToday(IDateCtl * po, int nday);
#endif /*CUST_EDITION*/
static void        DateCtl_DisplayMonthDays(DateCtl * pme, int nStartDOW, int nMonLen);
static boolean     DateCtl_DrawMonthView(DateCtl * pme);
static void        DateCtl_GetGridRect(DateCtl * pme, AEERect * prc, int nGridIndex);
static int         DateCtl_GetGridRectFromPoint(DateCtl * pme, AEERect * prc, int16 xPos, int16 yPos);
static void        DateCtl_HighlightGridRect(DateCtl * pme, int nGridIndex);

static boolean     DateCtl_DrawDate(DateCtl * pme);
static void        DateCtl_IncDateFld(DateCtl * pme, int nInc);
static int         DateCtl_CalcCurrentField(DateCtl * pme, AEERect* prc, int16 wXPos, int16 wYPos);
static boolean     DateCtl_GetDayString(DateCtl * pme, int nDay, AECHAR * pBuff, int nSize);
static boolean     DateCtl_GetMonthString(DateCtl * pme, int nMonth, AECHAR * pBuff, int nSize);

static AECHAR *    WriteChar(AECHAR * psz, AECHAR ch, int * pnRemaining);
static boolean     JDayToDate(long jday, int *pYear, int *pMonth, int *pDay);
static long        DateToJDay(int nYear, int nMonth, int nDay);
static int         GetMonthLength(int nMonth, int nYear);
static int         GetDayOfWeek(long lJulianDay);
static AECHAR *    SpitDate(AECHAR * psz, int * pVal, int * pPad, int nRemaining,AECHAR chDelim);
static void        SetTextSizeCache(DateCtl * pme);
#ifdef CUST_EDITION	
#if defined( FEATURE_JEWISH_CALENDAR)
static void        IDateCtl_GetJewishDate(IDateCtl* po, JewishType* pjewish);
#endif
static void drawImage( DateCtl *pMe, char *resFile, int16 resId, int x, int y);
#endif /*CUST_EDITION*/
#define HighlightDay(p,day) \
   DateCtl_HighlightGridRect(p,(day)+((p)->m_nStartDayOfWeek)-1)

#define ClearScreen(p) \
   IDISPLAY_EraseRect((p)->m_pIDisplay,&((p)->m_rc))

#define ClearDateText(p) \
   IDISPLAY_EraseRgn((p)->m_pIDisplay, 0, 0, (p)->m_rc.dx, (p)->m_nFontLineHeight)

//---------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------

static const VTBL(IDateCtl) gQDateCtlFuncs = {
   IDateCtl_AddRef,
   IDateCtl_Release,
   IDateCtl_HandleEvent,
   IDateCtl_Redraw,
   IDateCtl_SetActive,
   IDateCtl_IsActive,
   IDateCtl_SetRect,
   IDateCtl_GetRect,
   IDateCtl_SetProperties,
   IDateCtl_GetProperties,
   IDateCtl_Reset,
   IDateCtl_SetTitle,
   IDateCtl_SetJulianDay,
   IDateCtl_GetJulianDay,
   IDateCtl_SetDate,
   IDateCtl_GetDate,
   IDateCtl_GetDayOfWeek,
   IDateCtl_GetDayString,
   IDateCtl_GetMonthString,
   IDateCtl_GetDateString,
   IDateCtl_EnableCommand,
   IDateCtl_SetActiveDayMask,
   IDateCtl_SetFont,
   IDateCtl_GetFont,
   IDateCtl_SizeToFit
#ifdef CUST_EDITION	   
   ,IDateCtl_SetToday,
   IDateCtl_SetDateRange,
   IDateCtl_EnableDateRange,

#if defined( FEATURE_JEWISH_CALENDAR)
	IDateCtl_GetJewishDate
#endif
#endif /*CUST_EDITION*/
};

static const int16 gMonthIDs[] = {
      AEE_IDS_JANUARY, AEE_IDS_FEBRUARY, AEE_IDS_MARCH, AEE_IDS_APRIL, AEE_IDS_MAY, AEE_IDS_JUNE, 
      AEE_IDS_JULY, AEE_IDS_AUGUST, AEE_IDS_SEPTEMBER, AEE_IDS_OCTOBER, AEE_IDS_NOVEMBER, AEE_IDS_DECEMBER
};

static const int16 gDayIDs[] = { 
      AEE_IDS_SUNDAY, AEE_IDS_MONDAY, AEE_IDS_TUESDAY, AEE_IDS_WEDNESDAY, AEE_IDS_THURSDAY, AEE_IDS_FRIDAY, AEE_IDS_SATURDAY
};


/*===========================================================================

EXTERNAL - Called from AEEControls.c.  Supports creation of date classes.

===========================================================================*/
int DateCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj)
{
   DateCtl *      pme = NULL;
   int            i;
   IBitmap *      pbmDst;
   AEEBitmapInfo  bi;

   // allocate memory for object

   if(!pIShell || !ppobj)
      return(EBADPARM);

   *ppobj = NULL;

  // allocate memory for object

   if((pme = (DateCtl *)AEE_NewClass((IBaseVtbl *)&gQDateCtlFuncs, sizeof(DateCtl))) == NULL)
      return(ENOMEMORY);

   // Increment the reference count on the shell...

   ISHELL_AddRef(pIShell);

   // initialize object data

   pme->m_pIShell       = pIShell;
   pme->m_nRefs         = 1;
   pme->m_nNumFields    = 3;     // may be configurable later
   pme->m_nCurrentField = -1;
   pme->m_bMonthView    = (cls == AEECLSID_DATEPICKCTL); // needed by
                                                         // SetTextSizeCache
#ifdef CUST_EDITION	
   pme->m_ActRange  =  FALSE;
#endif /*CUST_EDITION*/
   // create IDisplay interface

   i = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
   if(!pme->m_pIDisplay){
      IDateCtl_Release((IDateCtl *)pme);
      return(i);
   }

   // get display metrics
   pbmDst = IDISPLAY_GetDestination(pme->m_pIDisplay);
   if (!pbmDst) {
      IDateCtl_Release((IDateCtl *)pme);
      return EFAILED;
   }
   i = IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   if (SUCCESS != i) {
      IDateCtl_Release((IDateCtl *)pme);
      return i;
   }
   SETAEERECT(&(pme->m_rc), 0, 0, bi.cx, bi.cy);

   // initialize default fonts

#if defined(FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_K212_ND)
    pme->m_fntTitle = AEE_FONT_ITALIC;  //AEE_FONT_SMALL;
    pme->m_fntText  = AEE_FONT_ITALIC;  //AEE_FONT_SMALL;
#else
    pme->m_fntTitle = AEE_FONT_BOLD;
    pme->m_fntText  = AEE_FONT_NORMAL;
#endif

   pme->m_nHitData = -1;
   // Set default date string
   if(cls == AEECLSID_DATEPICKCTL){
      pme->m_dwProps = DFMT_DD_MON_YY;
      DateCtl_SetGridRect(pme);
   }else{
      pme->m_dwProps = DFMT_MON_DD_YYYY;
   }
   // set text size fields
   SetTextSizeCache(pme);

   // load bitmap for tiny digits 0-9

   pme->m_pFont = ISHELL_LoadResImage(pme->m_pIShell, AEECONTROLS_RES_FILE, AEE_IDB_SMALLFONTS);
   if(pme->m_pFont)
      IIMAGE_SetParm(pme->m_pFont, IPARM_CXFRAME, IMAGE_WIDTH, 0);
#ifdef CUST_EDITION	   
#ifdef FEATURE_CALENDAR_USE_STYLE
   pme->m_pBgImage = ISHELL_LoadResImage(pme->m_pIShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modi by yangdecai
#endif
#endif /*CUST_EDITION*/
   *ppobj = (IDateCtl*)pme;

   return(0);
}


/*=====================================================================

Public Method - Increments the reference count on the date class object.

======================================================================*/
static uint32 IDateCtl_AddRef(IDateCtl * po)
{
   return(++((DateCtl*)po)->m_nRefs);
}

/*=====================================================================

Public Method - Decrements the reference count.  When the count reaches
0, the class is freed.

======================================================================*/
static uint32 IDateCtl_Release(IDateCtl * po)
{
   DateCtl * pme = (DateCtl*)po;

   if(pme->m_nRefs){
      if (--pme->m_nRefs)
         return(pme->m_nRefs);

      aee_releaseobj((void **)&pme->m_pFont);
#ifdef CUST_EDITION		  
#ifdef FEATURE_CALENDAR_USE_STYLE
      if(pme->m_pBgImage != NULL)
      {
        IImage_Release(pme->m_pBgImage);
        pme->m_pBgImage = NULL;
      }
#endif
#endif /*CUST_EDITION*/
      aee_freeobj((void **)&pme->m_pTitle);
      aee_releaseobj((void **)&pme->m_pIDisplay);
      aee_releaseobj((void **)&pme->m_pIShell);
      FREE(pme);
   }
   return(0);
}

/*=====================================================================

Public Method - Event handler for the date control.  See AEEDate.h

======================================================================*/
static boolean IDateCtl_HandleEvent(IDateCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   DateCtl * pme = (DateCtl*)po;

   if (!pme->m_bActive)
      return(FALSE);     // did not handle the event

   switch (eCode) {
   case EVT_POINTER_STALE_MOVE:
   case EVT_POINTER_MOVE:
      break;  // Quickly do not handle

   case EVT_POINTER_DOWN:
   case EVT_POINTER_UP:
      {
         AEERect rct;
         int16    wXPos = (int16)AEE_POINTER_GET_X((const char *)dwParam);
         int16    wYPos = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
         boolean  bRet  = FALSE;

         if( !pme->m_bValidDate ){
            if( PT_IN_RECT(wXPos, wYPos, pme->m_rc) && eCode == EVT_POINTER_DOWN ){
               bRet = TRUE; // Handled by specification, down, in rect and active
            }
            return bRet;
         }
         if (pme->m_bMonthView){
            int nHitData = DateCtl_GetGridRectFromPoint(pme, &rct, wXPos,wYPos);
            if (-1 != nHitData) {
               if( eCode == EVT_POINTER_UP){
                  if( nHitData == pme->m_nHitData ){
                     int nYear, nMonth, nDay;

                     JDayToDate(IDateCtl_GetJulianDay(po), &nYear, &nMonth, &nDay);
                     if( nHitData >= pme->m_nStartDayOfWeek ){
                        nDay = ((nHitData+1) - pme->m_nStartDayOfWeek);
                        if( nDay <= GetMonthLength(pme->m_nMonth, pme->m_nYear) ){
                           IDateCtl_SetJulianDay(po, DateToJDay(nYear, nMonth, nDay));
                           IDateCtl_Redraw(po);
                           bRet = TRUE; // Handled Pen Up.
                        }
                     }
                  }
               } else { // pointer up
                  if( nHitData != -1 ){
                     if (pme->m_bSendCmd) { 
                        if (PT_IN_RECT(wXPos, wYPos, rct) &&
                           (2 == AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam))) {
                           return ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)pme->m_nCmdId, (uint32)po);
                        }
                     }  //if (pme->m_bSendCmd)
                     bRet = TRUE; // Handled Pen Down.
                  } //if( nHitData != -1 )
               } //else { // pointer up
            } //if (-1 != nHitData)
            pme->m_nHitData = nHitData;
            return bRet;
         }else{
            int nHitData = DateCtl_CalcCurrentField(pme, &rct, (int16)AEE_POINTER_GET_X((const char *)dwParam), (int16)AEE_POINTER_GET_Y((const char *)dwParam));
            if (-1 != nHitData) {
               if( eCode == EVT_POINTER_UP){
                  if( nHitData == pme->m_nHitData ){
                     pme->m_nCurrentField = nHitData;
                     IDateCtl_Redraw(po);
                     bRet = TRUE; // Handled Pen Up.
                  }
               } else { // pointer up
                  if( nHitData != -1 ){
                     if (pme->m_bSendCmd) { 
                        if (PT_IN_RECT(wXPos, wYPos, rct) && 
                           (2 == AEE_POINTER_GET_CLICKCOUNT((const char *)dwParam))) {
                           return ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)pme->m_nCmdId, (uint32)po);
                        }
                     }  //if (pme->m_bSendCmd)
                     bRet = TRUE; // Handled Pen Down.
                  } //if( nHitData != -1 )
               } //else { // pointer up
            } //if (-1 != nHitData)
            pme->m_nHitData = nHitData;
            return bRet;
         }
      }

      case EVT_KEY: {
         
         switch (wParam) {
#ifdef CUST_EDITION	    
#if defined( AEE_SIMULATOR)
            case AVK_SELECT:
#else
            case AVK_INFO:
#endif
#else
			case AVK_SELECT:
#endif /*CUST_EDITION*/
               if (pme->m_bSendCmd) 
                  return ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND, (uint16)pme->m_nCmdId, (uint32)po);
               break;

            case AVK_LEFT:
               if (pme->m_bMonthView)
                  IDateCtl_SetJulianDay(po, pme->m_lJulianDay-1);
               else{
                  pme->m_nCurrentField = (pme->m_nCurrentField + pme->m_nNumFields - 1) % pme->m_nNumFields;
                  if(pme->m_nCurrentField == pme->m_nNumFields - 1)
                     ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)0, (uint32)po);
               }
               IDateCtl_Redraw(po);
               return(TRUE);

            case AVK_RIGHT:
               if (pme->m_bMonthView)
                  IDateCtl_SetJulianDay(po, pme->m_lJulianDay+1);
               else{
                  pme->m_nCurrentField = (pme->m_nCurrentField + 1) % pme->m_nNumFields;
                  if(pme->m_nCurrentField == 0)
                     ISHELL_HandleEvent(pme->m_pIShell, EVT_CTL_TAB, (uint16)1, (uint32)po);
               }
               IDateCtl_Redraw(po);
               return(TRUE);

            case AVK_UP:
				if(pme->m_nYear>=1980)
				{
               		if (pme->m_bMonthView)
                  	IDateCtl_SetJulianDay(po, pme->m_lJulianDay-7);
              		 else 
                  	DateCtl_IncDateFld(pme, 1);
				}
               IDateCtl_Redraw(po);
               return(TRUE);

            case AVK_DOWN:
               if (pme->m_bMonthView)
                  IDateCtl_SetJulianDay(po, pme->m_lJulianDay+7);
               else
                  DateCtl_IncDateFld(pme, -1);
               IDateCtl_Redraw(po);
               return(TRUE);
         }

      } break;

  }

  return(FALSE);   // did not handle the event
}

/*=====================================================================

Public Method - Redraws the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_Redraw(IDateCtl * po)
{
   DateCtl * pme = (DateCtl *)po;

   if (pme->m_bMonthView){
      if(pme->m_bMonthChanged && pme->m_bSendCmd){
         ISHELL_HandleEvent(pme->m_pIShell, EVT_COMMAND,(uint16)-1, (uint32)po);
         pme->m_bMonthChanged = FALSE;
      }
      return DateCtl_DrawMonthView(pme);
   }
   return DateCtl_DrawDate(pme);
}

/*=====================================================================

Public Method - Sets the date control into/out of edit mode. See AEEDate.h

======================================================================*/
static void IDateCtl_SetActive(IDateCtl * po, boolean bActive)
{
   DateCtl * pme = (DateCtl *)po;

   if (pme->m_bActive != bActive) {
      pme->m_bActive = bActive;
      pme->m_nCurrentField = (bActive ? 0 : -1);
      IDateCtl_Redraw(po);
   }
}

/*=====================================================================

Public Method - Returns TRUE if the date control is in edit mode. See AEEDate.h

======================================================================*/
static boolean IDateCtl_IsActive(IDateCtl * po)
{
	return (((DateCtl*)po)->m_bActive == 0 ? FALSE : TRUE);
}

/*=====================================================================

Public Method - Sets the rectangle for the date control. See AEEDate.h

======================================================================*/
static void IDateCtl_SetRect(IDateCtl * po, const AEERect * prc)
{
   DateCtl * pme = (DateCtl *)po;

   if(prc){
      pme->m_rc = *prc;
      if(pme->m_bMonthView)
         DateCtl_SetGridRect((DateCtl *)po);
      else{

   // 11/14/01 - Insure minimum height can fit the date...

         if(prc->dy < pme->m_nFontLineHeight)
            pme->m_rc.dy = pme->m_nFontLineHeight;
      }

   }
}

/*=====================================================================

Public Method - Returns the rectangle for the date control. See AEEDate.h

======================================================================*/
static void IDateCtl_GetRect(IDateCtl * po, AEERect * prc)
{
   if(prc)
      *prc = ((DateCtl*)po)->m_rc;
}

/*=====================================================================

Public Method - Sets the flags/properties for the date control. See AEEDate.h

======================================================================*/
static void IDateCtl_SetProperties(IDateCtl * po, uint32 nProperties)
{
   DateCtl * pme = (DateCtl*)po;
   uint32 dwDispMask = pme->m_dwProps & DFMT_FORMAT_MASK;
   
   pme->m_dwProps = nProperties;
   // If they change the property, this will need to recalc.
   if((nProperties & DFMT_FORMAT_MASK) != dwDispMask)
      SetTextSizeCache(pme);
}
   
/*=====================================================================

Public Method - Returns the flags for the date control. See AEEDate.h

======================================================================*/
static uint32 IDateCtl_GetProperties(IDateCtl * po)
{
   DateCtl * pme = (DateCtl*)po;

   return(pme->m_dwProps);
}

/*=====================================================================

Public Method - Resets the contents and edit mode of the date control. See AEEDate.h

======================================================================*/
static void IDateCtl_Reset(IDateCtl * po)
{
   DateCtl * pme = (DateCtl*)po;

   aee_freeobj((void **)&pme->m_pTitle);

   pme->m_bActive = 0;
   pme->m_nCurrentField = -1;
   pme->m_dwMDayMask = 0;
   pme->m_nHitData   = -1;
}

/*=====================================================================

Public Method - Resets the title of the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_SetTitle(IDateCtl * po, const char * pszResFile, uint16 wResID, AECHAR * pText)
{
   DateCtl * pme = (DateCtl*)po;
   AECHAR  szBuff[32];

   aee_freeobj((void **)&pme->m_pTitle);
  
   if (!pText && pszResFile &&
       ISHELL_LoadResString(pme->m_pIShell, pszResFile, wResID, szBuff, sizeof(szBuff))) 
      pText = szBuff;

   if (pText) 
      pme->m_pTitle = WSTRDUP(pText);

   return (pme->m_pTitle != NULL);
}

/*=====================================================================

Public Method - Sets the Julian date for the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_SetJulianDay(IDateCtl * po, int32 lJulDate)
{
   int nYear = 0, nMonth = 0, nDay = 0;

   DateCtl * pme = (DateCtl*)po;

  // convert to yyyy / mm / dd
   nYear = nMonth = nDay = 0;

   pme->m_bValidDate = JDayToDate(lJulDate, &nYear, &nMonth, &nDay);

   if(pme->m_bValidDate){

  // save the julian day

      pme->m_lJulianDay = lJulDate;
#ifdef CUST_EDITION	
      if(pme->m_ActRange) 
      { 
         if((int32)pme->m_StartJulDate > lJulDate)
         {
            pme->m_lJulianDay = pme->m_StartJulDate;
         }

         if((int32)pme->m_EndJulDate < lJulDate )
         {
             pme->m_lJulianDay = pme->m_EndJulDate;
         }         

		 if( lJulDate != pme->m_lJulianDay)
		 {
			 lJulDate = pme->m_lJulianDay;
			 JDayToDate( lJulDate, &nYear, &nMonth, &nDay);
		 }
      }
#endif /*CUST_EDITION*/
  // get and save the day-of-week for the 1st of the month

      pme->m_nStartDayOfWeek = GetDayOfWeek(lJulDate - nDay + 1);

  // get and save day-of-week  (0=Sun ... 6=Sat)

      pme->m_nDayOfWeek = GetDayOfWeek(lJulDate);

      pme->m_bMonthChanged = (nMonth != pme->m_nMonth ? TRUE : FALSE);

   }
   else{
      pme->m_nStartDayOfWeek = 0;
      pme->m_nDayOfWeek = 0;
      pme->m_lJulianDay = 0;
   }

  // save year, month, day

   pme->m_nYear  = nYear;
   pme->m_nMonth = nMonth;
   pme->m_nDay   = nDay;
#ifdef CUST_EDITION	
   if( pme->m_bValidDate)
   {
	   ISHELL_PostEvent( pme->m_pIShell, ISHELL_ActiveApplet( pme->m_pIShell), EVT_COMMAND,(uint16)-2, (uint32)po);
   }
#endif /*CUST_EDITION*/
   return(pme->m_bValidDate);
}
#ifdef CUST_EDITION	
/*=====================================================================

======================================================================*/
static boolean IDateCtl_SetJulianDayEx(IDateCtl * po, int32 lJulDate)
{
   int nYear = 0, nMonth = 0, nDay = 0;

   DateCtl * pme = (DateCtl*)po;

  // convert to yyyy / mm / dd
   nYear = nMonth = nDay = 0;

   pme->m_bValidDate = JDayToDate(lJulDate, &nYear, &nMonth, &nDay);

   if(pme->m_bValidDate){

  // save the julian day

      pme->m_lJulianDay = lJulDate;

  // get and save the day-of-week for the 1st of the month

      pme->m_nStartDayOfWeek = GetDayOfWeek(lJulDate - nDay + 1);

  // get and save day-of-week  (0=Sun ... 6=Sat)

      pme->m_nDayOfWeek = GetDayOfWeek(lJulDate);

   }
   else{
      pme->m_nStartDayOfWeek = 0;
      pme->m_nDayOfWeek = 0;
      pme->m_lJulianDay = 0;
   }

  // save year, month, day

   pme->m_nYear  = nYear;
   pme->m_nMonth = nMonth;
   pme->m_nDay   = nDay;

   return(pme->m_bValidDate);
}
#endif /*CUST_EDITION*/
/*=====================================================================

Public Method - Returns the julian date of the date control. See AEEDate.h

======================================================================*/
static int32 IDateCtl_GetJulianDay(IDateCtl * po)
{
   return ((DateCtl*)po)->m_lJulianDay;
}

/*=====================================================================

Public Method - Sets the date for the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_SetDate(IDateCtl * po, int nYear, int nMonth, int nDay)
{
   int32 lJulDay = DateToJDay(nYear, nMonth, nDay);

   IDateCtl_SetJulianDay(po, lJulDay);

   if(lJulDay  == 0 && (nYear != 1969 || nMonth != 12 || nDay != 28))
	  return(FALSE);
   return(TRUE);
}

/*=====================================================================

Public Method - Returns the date for the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_GetDate(IDateCtl * po, int * pnYear, int * pnMonth, int * pnDay)
{
   DateCtl * pme = (DateCtl*)po;

   if (pnYear)
      *pnYear = pme->m_nYear;

   if (pnMonth)
      *pnMonth = pme->m_nMonth;

   if (pnDay)
      *pnDay = pme->m_nDay;

   return(TRUE);
}

/*=====================================================================

Public Method - Returns the week day of the date control. See AEEDate.h

======================================================================*/
static uint16 IDateCtl_GetDayOfWeek(IDateCtl * po)
{
   return ((DateCtl*)po)->m_nDayOfWeek;
}

/*=====================================================================

Public Method - Returns the day string for the date control. See AEEDate.h

======================================================================*/
static AECHAR * IDateCtl_GetDayString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars)
{
   DateCtl * pme = (DateCtl*)po;
   int nChars = 0;

   if (DateCtl_GetDayString(pme, pme->m_nDayOfWeek, pBuffer, nMaxSize))
      nChars = WSTRLEN(pBuffer);

   if (pnChars)
      *pnChars = nChars;

   return pBuffer + nChars + 1;
}

/*=====================================================================

Public Method - Returns the month string for the date control. See AEEDate.h

======================================================================*/
static AECHAR * IDateCtl_GetMonthString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars)
{
   DateCtl * pme = (DateCtl*)po;
   int nChars = 0;

   if (DateCtl_GetMonthString(pme, pme->m_nMonth, pBuffer, nMaxSize))
      nChars = WSTRLEN(pBuffer);

   if (pnChars)
      *pnChars = nChars;

   return pBuffer + nChars + 1;
}

/*=====================================================================

Public Method - Returns the date string for the date control. See AEEDate.h

======================================================================*/
static boolean IDateCtl_GetDateString(IDateCtl * po, AECHAR * pBuffer, int nMaxSize, int * pnChars, uint32 dwDateFormat)
{
   DateCtl *   pme = (DateCtl*)po;
   AECHAR *    psz = pBuffer;
   uint32      dwFormat = (dwDateFormat & DFMT_FORMAT_MASK);
   int         nRemaining = nMaxSize/sizeof(AECHAR);
   int         nYear = pme->m_nYear;
   int         nWritten = 0;
   int         pad[3];
   int         val[3];

   if(!pBuffer)
	   return(FALSE);

   if (pnChars)
      *pnChars = 0;
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_IC241A_MMX)||defined(FEATURE_VERSION_K232_Y100A)
	if(dwFormat == DFMT_DD_MM_YYYY)
	{
		dwFormat = DFMT_DD_MON_YY;
	}
	else if(dwFormat == DFMT_MM_DD_YYYY) 
	{
		dwFormat = DFMT_MON_DD_YY;
	}
#endif
#ifdef FEATURE_VERSION_K212_ND
	dwFormat=DFMT_YYYY_MM_DD;
#endif
   switch(dwFormat){
      case DFMT_DD_MONTH_YYYY:
      case DFMT_DD_MON_YYYY:
      case DFMT_DD_MON_YY:
      
         psz = WWRITELONGEX(psz, pme->m_nDay, 0, &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);
         psz = WriteChar(psz, ' ', &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);
      
         if (dwFormat == DFMT_DD_MONTH_YYYY) {
            psz = IDateCtl_GetMonthString(po, psz, nRemaining*sizeof(AECHAR), &nWritten);
	        psz--;
   	   }
         else {
            IDateCtl_GetMonthString(po, psz, nRemaining*sizeof(AECHAR), &nWritten);
            psz[ABBREV_LEN] = 0;
            nWritten = ABBREV_LEN;
            psz += ABBREV_LEN;
         }
         nRemaining -= nWritten;
         if (nRemaining <= 0)
            return(FALSE);

         psz = WriteChar(psz, ' ', &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);

         if (dwFormat == DFMT_DD_MON_YY) {
            nYear %= 100;
            psz = WriteChar(psz, '\'', &nRemaining);
            if (nRemaining <= 0)
               return(FALSE);
         }
         psz = WWRITELONGEX(psz, nYear, 2, &nRemaining);
         break;

      case DFMT_MONTH_DD_YYYY:
      case DFMT_MON_DD_YYYY:  
      case DFMT_MON_DD_YY:
         if (dwFormat == DFMT_MONTH_DD_YYYY){
            psz = IDateCtl_GetMonthString(po, psz, nRemaining*sizeof(AECHAR), &nWritten);
            psz--;
         }
         else{
            IDateCtl_GetMonthString(po, psz, nRemaining*sizeof(AECHAR), &nWritten);
            nWritten = ABBREV_LEN;
            psz[ABBREV_LEN] = 0;
            psz += ABBREV_LEN;
         }
      
         nWritten--;

         nRemaining -= nWritten;
         if (nRemaining <= 0)
            return(FALSE);

         psz = WriteChar(psz, ' ', &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);

         psz = WWRITELONGEX(psz, pme->m_nDay, 0, &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);

         psz = WriteChar(psz, ',', &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);

         psz = WriteChar(psz, ' ', &nRemaining);
         if (nRemaining <= 0)
            return(FALSE);
      
         if (dwFormat == DFMT_MON_DD_YY) {
            nYear %= 100;
            psz = WriteChar(psz, '\'', &nRemaining);
            if (nRemaining <= 0)
               return(FALSE);
         }

         psz = WWRITELONGEX(psz, nYear, 2, &nRemaining);
         break;

      case DFMT_INT_YYYY_MM_DD:
      case DFMT_YYYY_MM_DD:
         pad[0] = 2;
         pad[1] = pad[2] = 0;
         val[0] = nYear;
         val[1] = pme->m_nMonth;
         val[2] = pme->m_nDay;
         psz = SpitDate(psz, val, pad, nRemaining,(AECHAR)(dwFormat == DFMT_YYYY_MM_DD ? '/' : '.'));
         if(!psz)
            return(FALSE);
         break;

      case DFMT_MM_DD_YYYY:
         pad[0] = pad[1] = 0;
         pad[2] = 2;
         val[0] = pme->m_nMonth;
         val[1] = pme->m_nDay;
         val[2] = nYear;
         psz = SpitDate(psz, val, pad, nRemaining,(AECHAR)'/');
         if(!psz)
            return(FALSE);
         break;

      case DFMT_DD_MM_YYYY:
         pad[0] = pad[1] = 0;
         pad[2] = 2;
         val[0] = pme->m_nDay;
         val[1] = pme->m_nMonth;
         val[2] = nYear;
         psz = SpitDate(psz, val, pad, nRemaining,(AECHAR)'/');
         if(!psz)
            return(FALSE);
         break;

      default:
         return(FALSE);
   }

   if (pnChars)
      *pnChars = WSTRLEN(pBuffer);
   
   return(TRUE);
}

/*===================================================================== 
Function: IDateCtl_SetFont()

Description: 
   This method allows the caller to set the title and text fonts.

Prototype:

   void IDateCtl_SetFont(IDateCtl * pIDateCtl, AEEFont fntText, AEEFont fntTitle);

Parameters:
   pIDateCtl : Pointer to the IDateCtl Interface object
   fntText: Font to use for the text
            If this parameter is set to NULL then the default font of
            AEE_FONT_NORMAL will be assumed.
   fntTitle: Font to use for the title
             If this parameter is set to NULL then the default font of
             AEE_FONT_BOLD will be assumed.

Return Value: 
   None

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=====================================================================*/ 
static void IDateCtl_SetFont(IDateCtl * po, AEEFont fntText, AEEFont fntTitle)
{
   DateCtl * pme = (DateCtl*)po;

   if(!fntTitle)
      fntTitle = AEE_FONT_BOLD;
   if(!fntText)
      fntText  = AEE_FONT_NORMAL;

   if (!BETWEEN(fntText,AEE_FONT_NORMAL,AEE_FONT_TOTAL) ||
       !BETWEEN(fntTitle,AEE_FONT_NORMAL,AEE_FONT_TOTAL)) {
      return;
   }

   pme->m_fntTitle = fntTitle;
   pme->m_fntText  = fntText;

   // set text size fields
   SetTextSizeCache(pme);
}

/*===================================================================== 

Function: IDATECTL_GetFont()

Description: 
   This method allows the caller to get the title and text fonts.

Prototype:

   boolean IDATECTL_GetFont(IDateCtl * pIDateCtl, AEEFont *pfntText, AEEFont *pfntTitle);

Parameters:
   pIDateCtl : Pointer to the IDateCtl Interface object
   pfntText: pointer to location to set to text font
   pfntTitle: pointer to location to set to title font

Return Value: 
    TRUE : If success
    FALSE : If unsuccessful

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=====================================================================*/ 
static boolean IDateCtl_GetFont(IDateCtl * po, AEEFont *pfntText, AEEFont *pfntTitle)
{
   DateCtl * pme = (DateCtl*)po;

   if( pfntText == NULL || pfntTitle == NULL ) {
      return FALSE;
   }

   *pfntTitle = pme->m_fntTitle;
   *pfntText  = pme->m_fntText;

   return TRUE;
}

/*===================================================================== 

Function: IDATECTL_SizeToFit()

Description: 
   This method allows the caller to size the rect to the date text

Prototype:

   void IDATECTL_SizeToFit(IDateCtl * pIDateCtl, AEERect *prc);

Parameters:
   pIDateCtl : Pointer to the IDateCtl Interface object
   prc: pointer to location to set to the fit rect

Return Value: 
   None

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=====================================================================*/ 
static void IDateCtl_SizeToFit(IDateCtl * po, AEERect *prc)
{
   DateCtl * pme = (DateCtl*)po;
   int       nWidth;
   int       nHeight;

   if( prc == NULL)
      return;

   nHeight = LINEHEIGHT + LINEHEIGHT; // Space for Day and Date

   if (pme->m_pTitle != NULL) // If we have a title
   {
      nHeight += TITLEHEIGHT; // Add space to display the title
   }

   nWidth = pme->m_nMonFldWidth + pme->m_nDayFldWidth 
          + pme->m_nYearFldWidth + 4;

   if (nWidth < prc->dx)
      prc->dx = nWidth;

   if (nHeight < prc->dy)
      prc->dy = nHeight;

   IDateCtl_SetRect(po, prc);
}

/*=====================================================================

Public Method - Enables/Disables the EVT_COMMAND for the date control. See AEEDate.h

======================================================================*/
static void IDateCtl_EnableCommand(IDateCtl * po, boolean bEnable, uint16 nCmdId)
{
   DateCtl * pme = (DateCtl*)po;
   pme->m_bSendCmd = (bEnable != 0);
   if (bEnable)
      pme->m_nCmdId = nCmdId;
}

/*=====================================================================

Public Method - Sets the active day mask (0-31) for the month. See AEEDate.h

======================================================================*/
static void IDateCtl_SetActiveDayMask(IDateCtl * po, uint32 dwMask)
{
   DateCtl * pme = (DateCtl*)po;

   pme->m_dwMDayMask = dwMask;
}

/*=====================================================================





Local Methods





======================================================================*/


/*==================================================================

Local Method - This function is used to get name string for the given month. This 
function reads name string from the resource control file specified 
by AEECONTROLS_RES_FILE.

==================================================================*/
static boolean DateCtl_GetMonthString(DateCtl * pme, int nMonth, AECHAR * pBuff, int nSize)
{  
   if(pBuff && nMonth >= 1 && nMonth <= 12)
      return(ISHELL_LoadResString(pme->m_pIShell, AEECONTROLS_RES_FILE, gMonthIDs[nMonth-1], pBuff, nSize) != 0);

   return(FALSE);
}

/*==================================================================

Local Method - This function is used to get name string for the given day of the 
week. This function reads name string from the resource control 
file specified by AEECONTROLS_RES_FILE.

==================================================================*/
static boolean DateCtl_GetDayString(DateCtl * pme, int nDay, AECHAR * pBuff, int nSize)
{
   if(nDay >= 0 && nDay <= 6) 
      return(ISHELL_LoadResString(pme->m_pIShell, AEECONTROLS_RES_FILE, gDayIDs[nDay], pBuff, nSize) != 0);

   return(FALSE);
}

/*==================================================================

Local Method - This function is used to draw current date on the phone screen. 
This clears the phone screen, draws phone title, draws day string 
at the next line and then draws date string on the next line in 
18 Jul, 2000 format. Depending upon the selection, day, month or 
year part is highlighted by drawing in reverse video.

==================================================================*/
static boolean DateCtl_DrawDate(DateCtl * pme)
{
   int         x = pme->m_rc.x;
   int         y = pme->m_rc.y;
   int         n;
   int         nChars = 0, nLines;
   AECHAR      sz[80];
   AECHAR *    psz = NULL;
   boolean     bHighlight,bCentered;
   AEERect     rc;
   uint32      dwFormat;

   if(pme->m_rc.dy <= 0 || pme->m_rc.dx <= 0)
      return(FALSE);

   ClearScreen(pme);

   if(!pme->m_bValidDate){
      IDISPLAY_Update(pme->m_pIDisplay);
      return(TRUE);
   }
   // Set the clip rect, so they can calculate this beneath us.
   MEMCPY(&rc, &pme->m_rc, sizeof(AEERect));
   IDISPLAY_SetClipRect(pme->m_pIDisplay, &rc);
   
   // Calculate the number of lines we can display...
   // Only show the title IF the number of lines allows...

   if (pme->m_rc.dy >= TITLEHEIGHT + LINEHEIGHT && pme->m_pTitle) {
      IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntTitle, pme->m_pTitle, -1, x, y+2,NULL,IDF_RECT_NONE);    
      y += TITLEHEIGHT;
      nLines = (pme->m_rc.dy - TITLEHEIGHT) / LINEHEIGHT;
   }
   else {
      nLines = pme->m_rc.dy / LINEHEIGHT;
   }

   bCentered = (boolean)(pme->m_dwProps & DP_CENTERED ? TRUE : FALSE);

   // Determine centered y coordinate...

   if(bCentered && nLines){
      n = LINEHEIGHT * nLines;
      if(n < pme->m_rc.dy)
         y += (pme->m_rc.dy - n) / 2;
   }

   // Only show the Day string if the number of lines is > 1

   if(nLines > 1){
      DateCtl_GetDayString(pme, pme->m_nDayOfWeek, sz, sizeof(sz));
      SETAEERECT(&rc, x, y, pme->m_rc.dx, LINEHEIGHT);      
      IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1, x, y+1, &rc,IDF_RECT_FILL|(bCentered ? IDF_ALIGN_CENTER : 0));     
      y += LINEHEIGHT;
   }

   // Determine the centered x coordinate...

   if(bCentered){
      n = (pme->m_nMonFldWidth + pme->m_nDayFldWidth + pme->m_nYearFldWidth + 4);
      if(n < pme->m_rc.dx)
         x += (pme->m_rc.dx - n) / 2;
   }

   // Always show the date...

   // display the month field

   bHighlight = (pme->m_nCurrentField == 0);

   // Change made by dpark 
   // Without cleaning the sz, if month is set to 0 the day field is still in the sz and gets displayed

   *sz = (AECHAR)0;
   
   dwFormat = pme->m_dwProps & DFMT_FORMAT_MASK;
   switch(dwFormat){
   case DFMT_MONTH_DD_YYYY:
   case DFMT_MON_DD_YYYY:
   case DFMT_MON_DD_YY:
   case DFMT_MM_DD_YYYY:
      {
         int      nSep = 0;    // Re-use for field separation widths depending on mode
         AECHAR   szSep[2];// Re-use for field separation chars depending on mode
         int      xCpy; //copy x traverser
         // For calculating the particle of the full field width, just the selection area.
         int      nMonWidth, nDayWidth, nYearWidth, nPad;
         // See if it is a Mon or a xx type and format accordingly
         if(dwFormat != DFMT_MM_DD_YYYY){
            IDateCtl_GetMonthString((IDateCtl*)pme, sz, sizeof(sz), &nChars);
            if(dwFormat == DFMT_MON_DD_YYYY || dwFormat == DFMT_MON_DD_YY){
               sz[ABBREV_LEN] = (AECHAR)0;
            }
         }else{
            WWRITELONG(sz, pme->m_nMonth);
         }
         // Get Width of this field and draw
         nPad = (FLDPAD/2);         
         nMonWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,pme->m_fntText, (const AECHAR *)sz) + nPad;
                
         SETAEERECT(&rc, x, y, nMonWidth, LINEHEIGHT);
         
         IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1,x+nPad, y+2, &rc,(bHighlight ? IDF_RECT_FILL|IDF_TEXT_INVERTED : IDF_RECT_FILL));
         xCpy = x;
         // Offset and draw field separator if needed
         if(dwFormat == DFMT_MM_DD_YYYY){
            *szSep = (AECHAR)'/';
            *(szSep+1) = (AECHAR)0;
            x += nMonWidth;
            
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);            
            SETAEERECT(&rc, x, y, nSep, LINEHEIGHT);           
            IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, szSep, -1,x, y+2, &rc,IDF_RECT_FILL);            
         }
         x = xCpy + pme->m_nMonFldWidth;
         // display the day field

         bHighlight = (pme->m_nCurrentField == 1);
         psz = WWRITELONG(sz, pme->m_nDay);         
         nDayWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,pme->m_fntText, (const AECHAR *)sz) + nPad;        
         SETAEERECT(&rc, x, y, nDayWidth, LINEHEIGHT);        
         IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1, x+nPad, y+2, &rc, (bHighlight ? IDF_RECT_FILL|IDF_TEXT_INVERTED : IDF_RECT_FILL));
         xCpy = x;
         x += nDayWidth;
         if(dwFormat != DFMT_MM_DD_YYYY){
            *szSep = (AECHAR)',';
            *(szSep+1) = (AECHAR)0;            
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);           
         }
         SETAEERECT(&rc, x, y, nSep, LINEHEIGHT);        
         IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, szSep, -1,x, y+2, &rc,IDF_RECT_FILL);         
         x = xCpy + pme->m_nDayFldWidth;
         // display the year field

         bHighlight = (pme->m_nCurrentField == 2);
         psz = WWRITELONG(sz, pme->m_nYear);
         if(dwFormat != DFMT_MON_DD_YY){
            psz = sz;
         }else{
            *szSep = (AECHAR)'\'';
            *(szSep+1) = (AECHAR)0;            
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);           
            SETAEERECT(&rc, x, y, nSep+(FLDPAD/2), LINEHEIGHT);           
            IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, szSep, -1,x+nPad, y+2, &rc,IDF_RECT_FILL);
            nPad = 0;
            x+= nSep + nPad + 1;
            psz -=2;
         }
         
         nYearWidth = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)psz) + FLDPAD;         
         SETAEERECT(&rc, x, y, nYearWidth, LINEHEIGHT);         
         IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, psz, -1, x+nPad, y+2, &rc,(bHighlight ? IDF_RECT_FILL|IDF_TEXT_INVERTED : IDF_RECT_FILL));
     }
      break;
   case DFMT_DD_MONTH_YYYY:
   case DFMT_DD_MON_YYYY:
   case DFMT_DD_MON_YY:
   case DFMT_DD_MM_YYYY:
      break;
   case DFMT_YYYY_MM_DD:
   case DFMT_INT_YYYY_MM_DD:
      break;
   }

   // update the screen

   IDISPLAY_Update(pme->m_pIDisplay);
   IDISPLAY_SetClipRect(pme->m_pIDisplay, NULL);

   return(TRUE);
}

/*==================================================================

Local Method - This function is used to calculate current field portion
Return: -1 == Nothing!
==================================================================*/
static int DateCtl_CalcCurrentField(DateCtl * pme, AEERect* prc, int16 wXPos, int16 wYPos)
{
   int         x = pme->m_rc.x;
   int         y = pme->m_rc.y;
   int         n;
   int         nChars = 0, nLines;
   AECHAR      sz[80];
   AECHAR *    psz = NULL;
   boolean     bCentered;
   uint32      dwFormat;

   if( !PT_IN_RECT(wXPos, wYPos, pme->m_rc) ){
      return -1;
   }
   
   // Calculate the number of lines we can display...
   // See if Title will be shown and increment
   if (pme->m_rc.dy >= TITLEHEIGHT + LINEHEIGHT && pme->m_pTitle) {
      y += TITLEHEIGHT;
      nLines = (pme->m_rc.dy - TITLEHEIGHT) / LINEHEIGHT;
   }
   else {
      nLines = pme->m_rc.dy / LINEHEIGHT;
   }

   bCentered = (boolean)(pme->m_dwProps & DP_CENTERED ? TRUE : FALSE);
   // Determine centered y coordinate...
   if(bCentered && nLines){
      n = LINEHEIGHT * nLines;
      if(n < pme->m_rc.dy)
         y += (pme->m_rc.dy - n) / 2;
   }
   if(nLines > 1){
      DateCtl_GetDayString(pme, pme->m_nDayOfWeek, sz, sizeof(sz));
      y += LINEHEIGHT;
      if( wYPos < y || wYPos > y + LINEHEIGHT ){
         return -1;
      }
   }

   // Determine the centered x coordinate...
   if(bCentered){
      n = (pme->m_nMonFldWidth + pme->m_nDayFldWidth + pme->m_nYearFldWidth + 4);
      if(n < pme->m_rc.dx)
         x += (pme->m_rc.dx - n) / 2;
      if( wXPos < x || wXPos > (x + n) ){
         return -1;
      }
   }

   // Without cleaning the sz, if month is set to 0 the day field is still in the sz
   *sz = (AECHAR)0;
   
   dwFormat = pme->m_dwProps & DFMT_FORMAT_MASK;
   switch(dwFormat){
   case DFMT_MONTH_DD_YYYY:
   case DFMT_MON_DD_YYYY:
   case DFMT_MON_DD_YY:
   case DFMT_MM_DD_YYYY:
      {
         int      nSep = 0;    // Re-use for field separation widths depending on mode
         AECHAR   szSep[2];// Re-use for field separation chars depending on mode
         int      xCpy; //copy x traverser
         // For calculating the particle of the full field width, just the selection area.
         int      nMonWidth, nDayWidth, nYearWidth, nPad;
         // See if it is a Mon or a xx type and format accordingly
         if(dwFormat != DFMT_MM_DD_YYYY){
            IDateCtl_GetMonthString((IDateCtl*)pme, sz, sizeof(sz), &nChars);
            if(dwFormat == DFMT_MON_DD_YYYY || dwFormat == DFMT_MON_DD_YY){
               sz[ABBREV_LEN] = (AECHAR)0;
            }
         }else{
            WWRITELONG(sz, pme->m_nMonth);
         }
         // Get Width of this field and draw
         nPad = (FLDPAD/2);
         
         nMonWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,pme->m_fntText, (const AECHAR *)sz) + nPad;        
         SETAEERECT(prc, x, y, nMonWidth, LINEHEIGHT);
         if( PT_IN_RECT(wXPos, wYPos, *prc) ){
            return 0;
         }
         xCpy = x;
         // Offset and draw field separator if needed
         if(dwFormat == DFMT_MM_DD_YYYY){
            *szSep = (AECHAR)'/';
            *(szSep+1) = (AECHAR)0;
            x += nMonWidth;            
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);            
            SETAEERECT(prc, x, y, nSep, LINEHEIGHT);
         }
         x = xCpy + pme->m_nMonFldWidth;
         // display the day field

         psz = WWRITELONG(sz, pme->m_nDay);
         nDayWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,pme->m_fntText, (const AECHAR *)sz) + nPad;         
         SETAEERECT(prc, x, y, nDayWidth, LINEHEIGHT);
         if( PT_IN_RECT(wXPos, wYPos, *prc) ){
            return 1;
         }
         xCpy = x;
         x += nDayWidth;
         if(dwFormat != DFMT_MM_DD_YYYY){
            *szSep = (AECHAR)',';
            *(szSep+1) = (AECHAR)0;            
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);           
         }
         SETAEERECT(prc, x, y, nSep, LINEHEIGHT);
         x = xCpy + pme->m_nDayFldWidth;
         // display the year field

         psz = WWRITELONG(sz, pme->m_nYear);
         if(dwFormat != DFMT_MON_DD_YY){
            psz = sz;
         }else{
            *szSep = (AECHAR)'\'';
            *(szSep+1) = (AECHAR)0;
            nSep = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)szSep);          
            SETAEERECT(prc, x, y, nSep+(FLDPAD/2), LINEHEIGHT);
            nPad = 0;
            x+= nSep + nPad + 1;
            psz -=2;
         }         
         nYearWidth = IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, (const AECHAR *)psz) + FLDPAD;        
         SETAEERECT(prc, x, y, nYearWidth, LINEHEIGHT);
         if( PT_IN_RECT(wXPos, wYPos, *prc) ){
            return 2;
         }
      }
      break;
   case DFMT_DD_MONTH_YYYY:
   case DFMT_DD_MON_YYYY:
   case DFMT_DD_MON_YY:
   case DFMT_DD_MM_YYYY:
      break;
   case DFMT_YYYY_MM_DD:
   case DFMT_INT_YYYY_MM_DD:
      break;
   }

   return -1;
}

/*==================================================================

Local Method - Days of a month are displayed on phone screen in rows of seven days. 
This function is used to set grid rect corresponding to the given 
grid index.

==================================================================*/
static void DateCtl_SetGridRect(DateCtl * pme)
{
   pme->m_rcGrid.x = pme->m_rc.x;
#ifdef FEATURE_CALENDAR_USE_STYLE
   pme->m_rcGrid.y = pme->m_rc.y + pme->m_nFontTitleHeight + 5;
#else
   pme->m_rcGrid.y = pme->m_rc.y + pme->m_nFontTitleHeight + 2;
#endif
   pme->m_rcGrid.dy = (pme->m_rc.dy - pme->m_rcGrid.y) * 10 / MAX_WEEK_ROWS; // multiply by 10 for rounding
   pme->m_rcGrid.dy = (pme->m_rcGrid.dy + 5) / 10;   // rounding
   pme->m_rcGrid.dx = (pme->m_rc.dx * 10 / DAYS_PER_WEEK); // multiply by 10 for rounding
   pme->m_rcGrid.dx = (pme->m_rcGrid.dx + 5) / 10;   // rounding

#if defined(FEATURE_VERSION_EC99) || defined (FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_K212_ND)
   pme->m_rcGrid.y -= 5;
   pme->m_rcGrid.dy -= 3;
#endif
}

/*==================================================================

Local Method - This function displays date on the phone screen. This clears the 
previous date, writes abbreviated day of the week in top left 
corner of phone screen and writes date string in the top right 
corner of phone screen.
==================================================================*/
#ifdef CUST_EDITION	
static void DateCtl_DisplayDateText(DateCtl * pme)
{
   AECHAR   sz[90];
#ifdef FEATURE_CALENDAR_USE_STYLE
   AEERect  rectTitle;
#endif
   RGBVAL nOldFontColor;

   
#ifdef FEATURE_CALENDAR_USE_STYLE
   SETAEERECT(&rectTitle, pme->m_rc.x+4, pme->m_rc.y+3, pme->m_rc.dx-7, pme->m_nFontLineHeight);
#ifndef FEATURE_VERSION_EC99
#ifndef FEATURE_VERSION_K212_20D
#ifndef FEATURE_VERSION_K212_ND
#ifndef FEATURE_VERSION_IN50A
   drawImage(pme, AEE_APPSCOMMONRES_IMAGESFILE, IDI_DATE_BAR, pme->m_rc.x, pme->m_rc.y);
#endif
#endif   
#endif
#endif
#endif
    nOldFontColor = IDISPLAY_SetColor( pme->m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);
   
   // display the day-of-week string in the top left of screen

   if (DateCtl_GetDayString(pme, pme->m_nDayOfWeek, sz, sizeof(sz)))
#ifdef FEATURE_CALENDAR_USE_STYLE     
      IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz,ABBREV_LEN,0, rectTitle.y, &rectTitle,IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);      
#else      
      IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz,ABBREV_LEN,pme->m_rc.x, pme->m_rc.y+1, NULL,IDF_TEXT_TRANSPARENT);      
#endif
   // display the date string in the top right of screen
   // DateCtl_FormatDate(pme, pme->m_nYear, pme->m_nMonth, pme->m_nDay, sz, 3);


{
    int nChars,nWidth;
    IDateCtl_GetDateString((IDateCtl*)pme, sz, sizeof(sz), &nChars, pme->m_dwProps);    
    nWidth = IDISPLAY_MeasureTextEx(pme->m_pIDisplay, pme->m_fntText, sz, nChars, 70, NULL);   
#ifdef FEATURE_CALENDAR_USE_STYLE   
    IDISPLAY_DrawText(pme->m_pIDisplay, 
                     pme->m_fntText, 
                     sz, 
                     -1,
                     0, 
                     rectTitle.y, 
                     &rectTitle,
                     IDF_ALIGN_RIGHT | IDF_TEXT_TRANSPARENT);   
#else
    IDISPLAY_DrawText(pme->m_pIDisplay, 
                     pme->m_fntText, 
                     sz, 
                     -1,
                     0, 
                     pme->m_rc.y+1, 
                     NULL,
                     IDF_ALIGN_RIGHT | IDF_TEXT_TRANSPARENT);    
#endif
}

    (void)IDISPLAY_SetColor( pme->m_pIDisplay, CLR_USER_TEXT, nOldFontColor/*RGB_BLACK*/);
}
#else
/*==================================================================

Local Method - This function displays date on the phone screen. This clears the 
previous date, writes abbreviated day of the week in top left 
corner of phone screen and writes date string in the top right 
corner of phone screen.

==================================================================*/
static void DateCtl_DisplayDateText(DateCtl * pme)
{
   int16    nWidth;
   AECHAR   sz[90];
   int      nChars;

   // clear date text

   ClearDateText(pme);
   
   // display the day-of-week string in the top left of screen

   if (DateCtl_GetDayString(pme, pme->m_nDayOfWeek, sz, sizeof(sz)))
   {
    IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz,ABBREV_LEN,pme->m_rc.x, pme->m_rc.y+1, NULL,0);   
   }
   
   // display the date string in the top right of screen
   // DateCtl_FormatDate(pme, pme->m_nYear, pme->m_nMonth, pme->m_nDay, sz, 3);

   IDateCtl_GetDateString((IDateCtl*)pme, sz, sizeof(sz), &nChars, pme->m_dwProps);  
   nWidth = IDISPLAY_MeasureTextEx(pme->m_pIDisplay, pme->m_fntText, sz, nChars, 70, NULL);
   IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1,pme->m_rc.dx-1-nWidth, pme->m_rc.y+1, NULL,0);  
}
#endif /*CUST_EDITION*/
/*==================================================================

Local Method - Days of a month are displayed on phone screen in rows of seven days. 
This function is used to get grid rect corresponding to the given 
grid index.

==================================================================*/
static void DateCtl_GetGridRect(DateCtl * pme, AEERect * prc, int nGridIndex) 
{
   int x, y;

   x = pme->m_rcGrid.x + ((nGridIndex % DAYS_PER_WEEK) * pme->m_rcGrid.dx);
   y = pme->m_rcGrid.y + ((nGridIndex / DAYS_PER_WEEK) * pme->m_rcGrid.dy);
      
#if defined (FEATURE_CALENDAR_USE_STYLE) && defined (CUST_EDITION)
#ifdef FEATURE_VERSION_K202
   SETAEERECT(prc, x+1, y+5, pme->m_rcGrid.dx-2, pme->m_rcGrid.dy-4);
#else
   MSG_FATAL("x+1=%d,y+1=%d",x+1,y+3,0);
   #ifdef FEATURE_VERSION_K212_ND
   SETAEERECT(prc, x+1, y+9, pme->m_rcGrid.dx-2, pme->m_rcGrid.dy-1);
   #else
   SETAEERECT(prc, x+1, y+1, pme->m_rcGrid.dx-2, pme->m_rcGrid.dy-4);
   #endif
   #endif
#else
   #ifdef FEATURE_VERSION_K212_HUALU
   SETAEERECT(prc, x+1, y+1, pme->m_rcGrid.dx-2, pme->m_rcGrid.dy-4);
   #else
   SETAEERECT(prc, x, y, pme->m_rcGrid.dx, pme->m_rcGrid.dy);
   #endif
#endif
}

/*==================================================================

Local Method - Days of a month are displayed on phone screen in rows of seven days. 
This function is used to get grid index and grid rect corresponding to the given point.
Return: -1 not in Grid!

==================================================================*/
static int DateCtl_GetGridRectFromPoint(DateCtl * pme, AEERect * prc, int16 xPos, int16 yPos) 
{
   int nGridIndex;
   int x, y;

   if( PT_IN_RECT(xPos, yPos, pme->m_rc) ){
      x = (xPos - pme->m_rc.x)/pme->m_rcGrid.dx;
      if( (xPos - pme->m_rc.x) >= (pme->m_rcGrid.dx*DAYS_PER_WEEK) ){
         x--;
      }
      y = (yPos - pme->m_rc.y)/pme->m_rcGrid.dy;
      if( y ){
         y--;
      }
      nGridIndex = (y*DAYS_PER_WEEK)+x;
      DateCtl_GetGridRect(pme, prc, nGridIndex);
      return nGridIndex;
   }
   return -1;
}

/*==================================================================

Local Method - This method is used to high light grid specified by the given index.

==================================================================*/
static void DateCtl_HighlightGridRect(DateCtl * pme, int nGridIndex)
{
  AEERect rc;

  DateCtl_GetGridRect(pme, &rc, nGridIndex);

  IDISPLAY_InvertRect(pme->m_pIDisplay, &rc);
}

/*==================================================================

Local Method - This method is used to display a digit on phone screen. This copies 
the specified digit from the font bitmap at the specified location 
on phone screen

==================================================================*/
static void DateCtl_Digit(DateCtl * pme, int nDigit, int x, int y)
{
   if (pme->m_pFont && nDigit >= 0 && nDigit <= 9)
      IIMAGE_DrawFrame(pme->m_pFont,nDigit, x, y);
   //	MSG_FATAL("x====%d,y=====%d",x,y,0);
}

/*==================================================================

Local Method - This function is called to draw specified day on phone screen.

==================================================================*/
static void DateCtl_DrawDay(DateCtl * pme, int nDay, AEERect * prc)
{
  int msd, lsd;
  int x, y;

  y = prc->y + ((prc->dy - IMAGE_WIDTH) / 2);
	
  if (nDay >= 1 && nDay <= 31) {

   // See if the day has been 'ticked'...

    if(pme->m_dwMDayMask & 1 << (nDay - 1)){
      AEERect rc;

      SETAEERECT(&rc, prc->x, prc->y, 1, 1);
	  MSG_FATAL("prc->x=====%d,prc->y======%d",prc->x,prc->y,0);
      IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_BLACK);
   }

    msd = nDay / 10;
    lsd = nDay % 10;
    if (msd > 0) {
      int extra = (msd != 1 && lsd != 1);
      x = prc->x + 1;
      DateCtl_Digit(pme, msd, x, y);
	  
      DateCtl_Digit(pme, lsd, x+4+extra, y);
    }
    else {
      x = prc->x + 3;
      DateCtl_Digit(pme, lsd, x, y);
    }
  }
}

/*==================================================================

Local Method - This function is used to display days of the month. days are 
displayed in rows of 7 days starting with sunday e.g. July 2000, 
thats starts with a saturday  will be displayed as
                      1
    2  3  4  5  6  7  8
    9 10 11 12 13 14 15
   16 17 18 19 20 21 22
   23 24 25 26 27 28 29
   30 31

though as per the present implementation of
DateCtl_DrawDay, single digit days will not align
with the last digit of double digit days. They will
fall in middle of first and last digit of double
digit days.

==================================================================*/
static void DateCtl_DisplayMonthDays(DateCtl * pme, int nStartDOW, int nMonLen)
{
   int      i;
   AEERect  rc;

   for (i = 0; i < nMonLen; i++) { 
      DateCtl_GetGridRect(pme, &rc, i+nStartDOW);
      //DateCtl_DrawDay(pme, i+1, &rc);
      #ifdef CUST_EDITION   //modi by yangdecai 2010.06.15
      DateCtl_DrawDayEx(pme, i+1, &rc);
      #else
      DateCtl_DrawDay(pme, i+1, &rc);
      #endif
   }
}
#ifdef CUST_EDITION	
/*==================================================================

Local Method - This method draws the month view on phone screen. This clears the 
phone screen, calls DateCtl_DisplayDateText to display date text 
in first line of the screen, draws a separator, draws matrix 
displaying month of the days, highlights current day and update 
phone screen.

==================================================================*/
static boolean DateCtl_DrawMonthView(DateCtl * pme)
{
    AEERect rc = pme->m_rc;
    ERR("DateCtl_DrawMonthView:1111111:%d",pme->m_bValidDate,0,0);
    if (pme->m_bValidDate)
    {

#ifdef FEATURE_CALENDAR_USE_STYLE
        IDISPLAY_FillRect(pme->m_pIDisplay, &rc, DATECTL_BG_COLOR);
        Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pBgImage, DATECTL_BG_COLOR, &rc, rc.x, rc.x);
#else
        IDISPLAY_FillRect(pme->m_pIDisplay, &rc, RGB_BLACK);
#endif

        // display date text
        DateCtl_DisplayDateText(pme);

        // draw line separator
        {
#ifdef FEATURE_FUNCS_THEME
#if !defined(FEATURE_CALENDAR_USE_STYLE)
            Theme_Param_type    themeParms      = {0};
            AEERect             rect            = {0};

            Appscom_GetThemeParameters( &themeParms);
            SETAEERECT( &rect, 0, pme->m_nFontTitleHeight, rc.dx, 1);
            IDISPLAY_FillRect( pme->m_pIDisplay, &rect, themeParms.themeColor);
#endif
#endif
        }

        // display the array of days for this month
        DateCtl_DisplayMonthDays(pme, pme->m_nStartDayOfWeek, GetMonthLength(pme->m_nMonth, pme->m_nYear));
    }

   IDISPLAY_Update(pme->m_pIDisplay);

   return(TRUE);
}
#else
/*==================================================================

Local Method - This method draws the month view on phone screen. This clears the 
phone screen, calls DateCtl_DisplayDateText to display date text 
in first line of the screen, draws a separator, draws matrix 
displaying month of the days, highlights current day and update 
phone screen.

==================================================================*/
static boolean DateCtl_DrawMonthView(DateCtl * pme)
{
   ERR("DateCtl_DrawMonthView:22222:%d",pme->m_bValidDate,0,0);	
   ClearScreen(pme);
   
   if(pme->m_bValidDate){

   // display date text

      DateCtl_DisplayDateText(pme);
   
   // draw line separator   
        
      IDISPLAY_DrawHLine(pme->m_pIDisplay, pme->m_rc.x, pme->m_rc.y+pme->m_nFontTitleHeight, pme->m_rc.dx);
   
   // display the array of days for this month
      DateCtl_DisplayMonthDays(pme, pme->m_nStartDayOfWeek, GetMonthLength(pme->m_nMonth, pme->m_nYear));
   
   // highlight the current day

      HighlightDay(pme, pme->m_nDay);
   
   // update the screen

   }

   IDISPLAY_Update(pme->m_pIDisplay);

   return(TRUE);
}
#endif /*CUST_EDITION*/
/*=====================================================================

Local Method - Increments the edit field in the direction specified.

======================================================================*/
static void DateCtl_IncDateFld(DateCtl * pme, int nInc)
{
   int nMonLen;

   if (pme->m_nCurrentField == 0) 
      pme->m_nMonth = WRAP(pme->m_nMonth+nInc-1, 12) + 1;
   else{
      if (pme->m_nCurrentField == 2) 
         pme->m_nYear = pme->m_nYear + nInc;
   }

   // always calculate month length for year

   nMonLen = GetMonthLength(pme->m_nMonth, pme->m_nYear);

   if (pme->m_nCurrentField == 1)
      pme->m_nDay = WRAP(pme->m_nDay+nInc-1, nMonLen) + 1;
   else
      pme->m_nDay = CONSTRAIN(pme->m_nDay, 1, nMonLen);

   IDateCtl_SetDate((IDateCtl*)pme, pme->m_nYear, pme->m_nMonth, pme->m_nDay);
}

/*=====================================================================



Utility Functions


======================================================================*/


/*==================================================================

Local Method - This function is used to put given character at the end of the given 
string provided pnRemaining is not NULL and contains non zero value.

==================================================================*/
static AECHAR * WriteChar(AECHAR * psz, AECHAR ch, int * pnRemaining)
{
   if (pnRemaining && (*pnRemaining > 0)) {
      if (*pnRemaining > 1) {
         *(psz++) = ch;
         (*pnRemaining)--;
      }
      *psz = 0;
   }
   return psz;
}

/*==================================================================

Local Method - This function is used to calculate day of the week for the 
specified julian day.

==================================================================*/
static int GetDayOfWeek(long lJulianDay)
{
  int dow = lJulianDay % 7;
  if (dow < 0)
    dow += 7;
  return dow;
}

/*==================================================================

Local Method - This function is used to calculate number of days in the specified 
month of the specified year.

==================================================================*/
static int GetMonthLength(int nMonth, int nYear)
{
  // handle february special case

  if (nMonth == 2)
    return (IS_LEAP(nYear) ? 29 : 28);

  // If we shift the year to start on Mar1, then
  // there is a repeating 5-month pattern of 
  // {31,30,31,30,31} {31,30,31,30,31} {31,<sp>

  nMonth = (nMonth > 2) ? nMonth-3 : nMonth+9;

  return ((nMonth % 5) & 1) ? 30 : 31;
}

/*==================================================================

Local Method - This function is used to calculate julian day for a specified date.

==================================================================*/
static long DateToJDay(int nYear, int nMonth, int nDay)
{
  // check for invalid dates
  if (nYear < 1 || 
      nMonth <= 0 || nMonth > 12 ||
      nDay <= 0 || nDay > 31)
    return 0;
  
  // Adjust the start of the year to be March 1
  if (nMonth > 2)
    nMonth -= 3;
  else {
    nMonth += 9;
    nYear--;
  }
  return ((nYear / 100) * DAYS_PER_400_YEARS) / 4
          + ((nYear % 100) * DAYS_PER_4_YEARS) / 4
          + (nMonth * DAYS_PER_5_MONTHS + 2) / 5
          + nDay
          - JDAY_OFFSET;
}

/*==================================================================

Local Method - This function is used to calculate date of a julian day

==================================================================*/
static boolean JDayToDate(long jday, int *pYear, int *pMonth, int *pDay)
{
  long      lTemp;
  int       nCentury;
  int       nYear;
  int       nMonth;
  int       nDay;
  int       nDayOfYear;    // (1 <= nDayOfYear <= 366
  
  if(jday < 0 || jday >= ((uint32)0xffffffff - (JDAY_OFFSET * 4)) / 4)
      return(FALSE);

   lTemp = (jday + JDAY_OFFSET) * 4 - 1;
   nCentury = lTemp / DAYS_PER_400_YEARS;

  // Calculate the year and day of year
   lTemp = ((lTemp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
   nYear = (nCentury * 100) + (lTemp / DAYS_PER_4_YEARS);
   
   if(nYear > 9999)
      return(FALSE);

   nDayOfYear = (lTemp % DAYS_PER_4_YEARS) / 4 + 1;
  
  // Calculate the month and day of month
   lTemp = nDayOfYear * 5 - 3;                   // shift to Mar based years
   nMonth = lTemp / DAYS_PER_5_MONTHS;
   nDay = (lTemp % DAYS_PER_5_MONTHS) / 5 + 1;
  
  // Convert back to Jan based years
   if (nMonth < 10)
      nMonth += 3;
   else {
      nMonth -= 9;
      nYear++;
   }
   *pYear = nYear;
   *pMonth = nMonth;
   *pDay = nDay;

   return(TRUE);
}

/*=====================================================================

Local Method - Writes the date to the buffer.

======================================================================*/
static AECHAR * SpitDate(AECHAR * psz, int * pVal, int * pPad, int nRemaining,AECHAR chDelim)
{
   int   i;

   for(i = 0; i < 3; i++, pVal++, pPad++){

      psz = WWRITELONGEX(psz, *pVal, *pPad, &nRemaining);

      if(nRemaining <= 0)
         return(NULL);

      if(i < 2){
         psz = WriteChar(psz, chDelim , &nRemaining);
         if (nRemaining <= 0)
           return(NULL);
      }
   }
   return(psz);
}      

/*=====================================================================

Local Method - Sets the text size fields when the font has been set.

======================================================================*/
static void SetTextSizeCache(DateCtl * pme)
{
   int    i;
   int16  width;
   AECHAR sz[80];
   AECHAR szExtra[2]; // extra formatting chars that affect field widths
   uint32 dwFormat = pme->m_dwProps & DFMT_FORMAT_MASK;

   // Set title height
   pme->m_nFontTitleHeight =
      IDISPLAY_GetFontMetrics(pme->m_pIDisplay, pme->m_fntTitle, NULL, NULL);  
   
   // Set line height  
   pme->m_nFontLineHeight =
      IDISPLAY_GetFontMetrics(pme->m_pIDisplay, pme->m_fntText, NULL, NULL);
         
   if (!pme->m_bMonthView) {
      // get width of widest month string
      pme->m_nMonFldWidth = 0;
      if(dwFormat != DFMT_MM_DD_YYYY && dwFormat != DFMT_DD_MM_YYYY
         && dwFormat != DFMT_YYYY_MM_DD && dwFormat != DFMT_INT_YYYY_MM_DD){
         for (i=1; i <= 12; i++) {
            DateCtl_GetMonthString(pme, i, sz, sizeof(sz));
            if(dwFormat != DFMT_DD_MONTH_YYYY && dwFormat != DFMT_MONTH_DD_YYYY)
               sz[ABBREV_LEN] = 0;

            width = IDISPLAY_MeasureText(pme->m_pIDisplay,
                                         pme->m_fntText, (const AECHAR *)sz);           
            if (pme->m_nMonFldWidth < width)
               pme->m_nMonFldWidth = width;
         }
      }else{
         // 10 will be the widest month
         *sz = '1';
         *(sz+1) = '0';
         *(sz+2) = (AECHAR)0;

         pme->m_nMonFldWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,
                                         pme->m_fntText, (const AECHAR *)sz);       
      }
      pme->m_nMonFldWidth += FLDPAD;
      if(dwFormat == DFMT_MM_DD_YYYY || dwFormat == DFMT_DD_MM_YYYY
         || dwFormat == DFMT_YYYY_MM_DD || dwFormat == DFMT_INT_YYYY_MM_DD){
         *szExtra = (AECHAR)(dwFormat == DFMT_INT_YYYY_MM_DD ? '.' : '/');
         *(szExtra+1) = (AECHAR)0;

         pme->m_nMonFldWidth += IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, szExtra);       
      }

      // get width of "30" which should be the widest date
      STRTOWSTR("30,",sz, sizeof(sz));
      pme->m_nDayFldWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,
                                                 pme->m_fntText,
                                                 (const AECHAR *)sz) + FLDPAD;      
      if(dwFormat == DFMT_MONTH_DD_YYYY || dwFormat == DFMT_MON_DD_YYYY
         || dwFormat == DFMT_MON_DD_YY || dwFormat == DFMT_MM_DD_YYYY
         || dwFormat == DFMT_DD_MM_YYYY){
         if(dwFormat == DFMT_DD_MM_YYYY || dwFormat == DFMT_MM_DD_YYYY)
            *szExtra = (AECHAR)'/';
         else
            *szExtra = (AECHAR)',';
         *(szExtra+1) = (AECHAR)0;
         pme->m_nDayFldWidth += IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, szExtra);
      }

      // get width of "2000" which should be the widest year
      STRTOWSTR("2000",sz, sizeof(sz));
      if(dwFormat == DFMT_DD_MON_YY || dwFormat == DFMT_MON_DD_YY)
         sz[2] = (AECHAR)0;
      pme->m_nYearFldWidth = IDISPLAY_MeasureText(pme->m_pIDisplay,
                                                  pme->m_fntText,
                                                  (const AECHAR *)sz) + FLDPAD;     
      if(dwFormat == DFMT_DD_MON_YY || dwFormat == DFMT_MON_DD_YY 
         || dwFormat == DFMT_YYYY_MM_DD || dwFormat == DFMT_INT_YYYY_MM_DD){
            if(dwFormat == DFMT_YYYY_MM_DD)
               *szExtra = (AECHAR)'/';
            else if(dwFormat == DFMT_INT_YYYY_MM_DD)
               *szExtra = (AECHAR)'.';
            else
               *szExtra = (AECHAR)'\'';
            *(szExtra+1) = (AECHAR)0;
            pme->m_nYearFldWidth += IDISPLAY_MeasureText(pme->m_pIDisplay, pme->m_fntText, szExtra);          
      }
   }
}
#ifdef CUST_EDITION	
/*==============================================================================
函数: 
       DateCtl_DrawDayEx
       
说明: 
       画日子数字到方格上去
       
参数: 
       pme [in]：指向TimeCtl接口t对象的指针。
       
返回值:
       无
       
备注:
       
==============================================================================*/
static void drawImage( DateCtl *pMe, char *resFile, int16 resId, int x, int y)
{
    IImage *image = ISHELL_LoadResImage( pMe->m_pIShell, resFile, resId);
    if( image != NULL)
    {
        IIMAGE_Draw( image, x, y);
        IIMAGE_Release( image);
    }
}

static void DateCtl_DrawDayEx(DateCtl * pme, int nDay, AEERect * prc)
{
    AECHAR sz[10];
    AEEFont lsave;
    RGBVAL preclr;
    
    MEMSET(sz, 0, sizeof(sz));
    lsave = pme->m_fntText;
    if (nDay >= 1 && nDay <= 31)
    {
#ifdef FEATURE_FUNCS_THEME    
        Theme_Param_type theme;
#endif
#ifdef FEATURE_CALENDAR_USE_STYLE
        RGBVAL nTextBgColor;
#endif

#ifdef FEATURE_FUNCS_THEME    
        Appscom_GetThemeParameters(&theme);
#endif
        if( ((pme->m_nStartDayOfWeek + nDay-1) % 7 == 0) ||
            ((pme->m_nStartDayOfWeek + nDay-1) % 7 == 6)
        )
        {
            preclr = DATECTL_SPECIALDAY_COLOR;//0x9900;
#ifdef FEATURE_CALENDAR_USE_STYLE
            nTextBgColor = DATECTL_SPECIALDAY_BG_COLOR;
#endif
        }
        else
        {
#ifdef FEATURE_FUNCS_THEME    
            preclr = theme.textColor;
#else
            preclr = DATECTL_NORMALDAY_COLOR;
#endif
#ifdef FEATURE_CALENDAR_USE_STYLE
            nTextBgColor = DATECTL_NORMALDAY_BG_COLOR;
#endif
        }
        preclr = IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, preclr);

        (void)WWRITELONG(sz, nDay); //copy the longe number to string.

        {
#ifdef FEATURE_CALENDAR_USE_STYLE
            uint32 properties = IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_ALIGN_CENTER;
#else
            uint32 properties = IDF_TEXT_TRANSPARENT|IDF_ALIGN_MIDDLE|IDF_RECT_FRAME;

            if (pme->m_dwMDayMask & 1 << (nDay - 1))
            {
                properties |= IDF_ALIGN_RIGHT;
            }
            else
            {
                properties |= IDF_ALIGN_CENTER;
            }
#endif
            
#ifdef FEATURE_CALENDAR_USE_STYLE
            IDISPLAY_FillRect(pme->m_pIDisplay, prc, nTextBgColor);
            Appscommon_ResetBackground(pme->m_pIDisplay, pme->m_pBgImage, nTextBgColor, prc, pme->m_rc.x, pme->m_rc.y);
#endif

            if (DateCtl_IsToday((IDateCtl*)pme, nDay))
            {
                RGBVAL oldFrameColor;
#ifdef FEATURE_CALENDAR_USE_STYLE
                AEERect rectText;

                SETAEERECT(&rectText, prc->x-1, prc->y-1, prc->dx+2,prc->dy+2);
                oldFrameColor = IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_FRAME, DATECTL_TODAY_FRAME_COLOR);
                properties |= IDF_RECT_FRAME;
                (void)IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1, 0, 0, &rectText, properties);               
#else
                oldFrameColor = IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_FRAME, 0xff00);
                (void)IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1, 0, 0, prc, properties);
#endif


                IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_FRAME, oldFrameColor);
            }
            else
            {
                (void)IDISPLAY_DrawText(pme->m_pIDisplay, pme->m_fntText, sz, -1, 0, 0, prc, properties);               
            }
            MSG_FATAL("****pyg*** pme->m_nDay =%d nDay =%d",pme->m_nDay,nDay,0);
            if (nDay == pme->m_nDay)
            {
                AEERect rc = { 0 };
                
                SETAEERECT( &rc, prc->x, prc->y + prc->dy, prc->dx, 1);   //modi by yangdecai 2010.06.15
                 MSG_FATAL("****pyg*** prc->x =%d prc->y + prc->dy =%d prc->dx =%d",prc->x,prc->y + prc->dy,prc->dx);
                IDISPLAY_FillRect(pme->m_pIDisplay, &rc, DATECTL_UNDELINE_COLOR);
            }
        }
        IDISPLAY_SetColor(pme->m_pIDisplay, CLR_USER_TEXT, preclr);

        // See if the day has been 'ticked'...
        if (pme->m_dwMDayMask & 1 << (nDay - 1))
        {
            int x = prc->x;
            int y = prc->y;
            if( DateCtl_IsToday((IDateCtl*)pme, nDay))
            {
                x ++;
                y ++;
            }

            drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDI_ALARM, x, y);
        }

        pme->m_fntText = lsave;

  }
}

static boolean DateCtl_IsToday(IDateCtl * po, int nday)
{
     int nYear1, nMonth1, nDay1;
     int nYear2, nMonth2, nDay2;
     int32 savelJulDate;
     
     DateCtl *pme = (DateCtl*) po;

     boolean revalue = FALSE;
     
     savelJulDate = pme->m_lJulianDay;

     IDateCtl_GetDate(po, &nYear1, &nMonth1, &nDay1);
     
     IDateCtl_SetJulianDayEx(po, pme->m_Today); 
     
     IDateCtl_GetDate(po, &nYear2, &nMonth2, &nDay2);

     if((nYear1==nYear2)&&(nMonth1==nMonth2))
     {
         if(nday == nDay2)
         {
            revalue = TRUE;
         }
     }
    
     pme->m_lJulianDay = savelJulDate;
     IDateCtl_SetJulianDayEx(po, pme->m_lJulianDay);
     return revalue;
}

static void IDateCtl_SetToday(IDateCtl * po, int32 lJulDate)
{
     DateCtl * pme = (DateCtl*)po;
     pme->m_Today = lJulDate;
     return;
}

static boolean IDateCtl_SetDateRange(IDateCtl * po, int32 StartJulDate, int32 EndJulDate)
{
   int nYear = 0, nMonth = 0, nDay = 0;
   boolean bStart = FALSE, bEnd = FALSE;
  
   DateCtl * pme = (DateCtl*)po;


   nYear = nMonth = nDay = 0;

   bStart = JDayToDate(StartJulDate, &nYear, &nMonth, &nDay);

   bEnd = JDayToDate(EndJulDate, &nYear, &nMonth, &nDay);

   if(bStart && bEnd)
   {
      pme->m_StartJulDate = StartJulDate;

      pme->m_EndJulDate = EndJulDate;
      
      return TRUE;
   }

   return FALSE; 
}

static void IDateCtl_EnableDateRange(IDateCtl * po, boolean ActRange)
{
    DateCtl * pme = (DateCtl*)po;
    pme->m_ActRange = ActRange;
    return;
}


//-----------------------------------------------------------------
#if defined( FEATURE_JEWISH_CALENDAR)

#define JEWISH_EPOCH 347995.5
#define GREGORIAN_EPOCH 1721425.5

static double jewish_to_jd( int year, int month, int day);

static boolean jewish_leap( int year)
{

	return ( ((year * 7) + 1) % 19) < 7;
}

static int jewish_year_months( int year)
{

	return jewish_leap( year) ? 13 : 12;
}

static int jewish_elapsed_days( int year)
{

	int months  = 0;
	int days    = 0;
	int parts   = 0;

	months = FFLOOR( ((235 * year) - 234) / 19);
	parts  = 12084 + ( 13753 * months);
	days   = ( months * 29) + FFLOOR( parts / 25920);

	if( ( (3 * ( days + 1)) % 7) < 3)
	{
		days ++;
	}
	return days;
}

static int jewish_delayed_days( int year)
{

	int last       = jewish_elapsed_days( year - 1);
	int present    = jewish_elapsed_days( year);
	int next       = jewish_elapsed_days( year + 1);

	return ((next - present) == 356) ? 2 :
			(((present - last) == 382) ? 1 : 0);
}

static int jewish_year_days( int year)
{

	return FFLOOR( jewish_to_jd( year + 1, 7, 1) - jewish_to_jd( year, 7, 1));
}

static int jewish_month_days( int year, int month)
{

	if( month == 2  ||
		month == 4  ||
		month == 6  ||
		month == 10 ||
		month == 13 ||
		( month == 12 && !jewish_leap(year)) ||
		( month == 8 && ( jewish_year_days( year) % 10) != 5) ||
		( month == 9 && ( jewish_year_days(year) % 10) == 3)
	)
	{
		return 29;
	}

	return 30;
}

static int jewish_to_days( int year, int month, int day)
{

	int i           = 0;
	int yearMonths  = jewish_year_months( year);
	int days        = jewish_elapsed_days( year) + jewish_delayed_days( year) + day + 1;

	if( month < 7)
	{

		for( i = 7; i <= yearMonths; i ++)
		{
			days += jewish_month_days( year, i);
		}

		for( i = 1; i < month; i ++)
		{
			days += jewish_month_days( year, i);
		}
	}
	else
	{

		for( i = 7; i < month; i ++)
		{
			days += jewish_month_days( year, i);
		}
	}

	return days;
}

static double jewish_to_jd( int year, int month, int day)
{

	double jd = JEWISH_EPOCH + jewish_to_days( year, month, day);
	return jd;
}

static void jd_to_jewish( double jd, int* pYear, int* pMonth, int* pDay)
{

	int year    = 0;
	int month   = 0;
	int day     = 0;
	int i       = 0;
	int count   = 0;
	int first   = 0;

	jd     = FFLOOR( jd) + 0.5;
	count  = FFLOOR( ((jd - JEWISH_EPOCH) * 98496.0) / 35975351.0);
	year   = count - 1;

	DBGPRINTF( ";-----------------");
	DBGPRINTF( ";jd_to_jewish");
	DBGPRINTF( ";count = %d, year = %d", count, year);
	for( i = count; jd >= jewish_to_jd( i, 7, 1); i ++)
	{
		year ++;
	}
	DBGPRINTF( ";result year = %d, loop %d to get result year", year, (year - count + 1));

	first = ( jd < jewish_to_jd( year, 1, 1)) ? 7 : 1;
	month = first;
	DBGPRINTF( ";month = %d", month);
	for( i = first; jd > jewish_to_jd( year, i, jewish_month_days( year, i)); i ++)
	{
		month++;
	}
	DBGPRINTF( ";result month = %d, loop %d to get result month", month, ( month - first));

	day = ( jd - jewish_to_jd( year, month, 1)) + 1;
	DBGPRINTF( ";result day = %d", day);
	DBGPRINTF( ";-----------------");

	*pYear     = year;
	*pMonth    = month;
	*pDay      = day;
}

static double date_to_jd( int year, int month, int day)
{
	return ( GREGORIAN_EPOCH - 1)             +
		   ( 365 * ( year - 1))               +
		   FFLOOR( (year - 1) / 4)            +
		   (-FFLOOR( (year - 1) / 100))       +
		   FFLOOR( (year - 1) / 400)          +
		   FFLOOR( (((367 * month) - 362) / 12) + ((month <= 2) ? 0 : ( IS_LEAP(year) ? -1 : -2)) + day);
}

static int get_holiday( int month, int day, int weekday, int yeardays, int julianYear)
{
	int holiday = 0;

	static int holidays_table[][30] =
	{

		{   // Nisan
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 15, 32, 16, 16, 16, 16,
			28, 29, 0, 0, 0, 24, 24, 24, 0, 0
		},
		{   // Iyar
			0, 17, 17, 17, 17, 17, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 18, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 26, 0, 0
		},
		{   // Sivan
			0, 0, 0, 0, 19, 20, 30, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Tamuz
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 21, 21, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 36, 36
		},
		{   // Av
			0, 0, 0, 0, 0, 0, 0, 0, 22, 22,
			0, 0, 0, 0, 23, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Elul
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Tishrey
			1, 2, 3, 3, 0, 0, 0, 0, 0, 4,
			0, 0, 0, 0, 5, 31, 6, 6, 6, 6,
			7, 27, 8, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Heshvan
			0, 0, 0, 0, 0, 0, 0, 0, 0, 35,
			35, 35, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Kislev
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 9, 9, 9, 9, 9, 9
		},
		{   // Tevet
			9, 9, 9, 0, 0, 0, 0, 0, 0, 10,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Shvat
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 11, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 33
		},
		{   // Adar
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			12, 0, 12, 13, 14, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Adar 1
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		},
		{   // Adar 2
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			12, 0, 12, 13, 14, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}
	};

	if (month < 1 || month > 14 || day < 1 || day > 31)
	{
		return 0;
	}

	holiday = holidays_table[month - 1][day - 1];
	DBGPRINTF( ";---------------get_holiday");
	DBGPRINTF( ";month = %d, day = %d, holiday = %d", month, day, holiday);

	// if tzom on sat delay one day, yom cipur on sat
	if( (holiday == 3) && ( weekday == 0 || ( day == 4 && weekday != 1)))
	{
		holiday = 0;
	}

	// 17 of Tanuz on sat
	if( (holiday == 21) && ( weekday == 0 || ( day == 18 && weekday != 1)))
	{
		holiday = 0;
	}

	// 9 of Av on sat
	if( (holiday == 22) && ( weekday == 0 || ( day == 10 && weekday != 1)))
	{
		holiday = 0;
	}

	// Hanukah in a long year
	if( (holiday == 9) && (yeardays % 10 != 3) && (day == 3))
	{
		holiday = 0;
	}

	// if tanit ester on sat mov to Thu
	if( (holiday == 12) && ( weekday == 0 || (day == 11 && weekday != 5)))
	{
		holiday = 0;
	}

	// yom yerushalym after 68
	if( holiday == 26 && julianYear < 1968)
	{
		holiday = 0;
	}

	// yom ha azmaot and yom ha zicaron
	if (holiday == 17)
	{
		if( julianYear < 1948)
		{
			holiday = 0;
		}
		else if( julianYear < 2004)
		{
			if( ( (day == 3) && (weekday == 5)) ||
				( (day == 4) && (weekday == 5)) ||
				( (day == 5) && (weekday != 6 && weekday != 0))
			)
			{
				holiday = 17;
			}
			else if( ( (day == 2) && (weekday == 4))  ||
					 ( (day == 3) && (weekday == 4))  ||
					 ( (day == 4) && (weekday != 5 && weekday != 6))
			)
			{
				holiday = 25;
			}
			else
			{
				holiday = 0;
			}
		}
		else
		{
			if( ( (day == 3) && (weekday == 5)) ||
				( (day == 4) && (weekday == 5)) ||
				( (day == 5) && (weekday != 6 && weekday != 0 && weekday != 2)) ||
				( (day == 6) && (weekday == 3))
			)
			{
				holiday = 17;
			}
			else if( ( (day == 2) && (weekday == 4)) ||
					 ( (day == 3) && (weekday == 4)) ||
					 ( (day == 4) && (weekday != 5 && weekday != 6 && weekday != 1)) ||
					 ( (day == 5) && (weekday == 2))
			)
			{
				holiday = 25;
			}
			else
			{
				holiday = 0;
			}
		}
	}

	// yom ha shoaa, on years after 1958
	if( holiday == 24)
	{
		if( julianYear < 1958)
		{
			holiday = 0;
		}
		else if( ( (day == 26) && (weekday != 5)) ||
				 ( (day == 27) && (weekday == 6 || weekday == 1)) ||
				 ( (day == 28) && (weekday != 2))
		)
		{
			holiday = 0;
		}
	}

	// Rabin day, on years after 1997
	if( holiday == 35)
	{
		if (julianYear < 1997)
		{
			holiday = 0;
		}
		else if( ( (day == 10 || day == 11) && (weekday != 5)) ||
				 ( (day == 12) && (weekday == 6 || weekday == 0))
		)
		{
			holiday = 0;
		}
	}

	// Zhabotinsky day, on years after 2005
	if (holiday == 36)
	{
		if (julianYear < 2005)
		{
			holiday = 0;
		}
		else if( ( (day == 29) && (weekday == 0)) ||
				 ( (day == 30) && (weekday != 1))
		)
		{
			holiday = 0;
		}
	}

	return holiday;
}

static void IDateCtl_GetJewishDate( IDateCtl * po, JewishType* pjewish)
{

	DateCtl*   pme     = (DateCtl*)po;
	JulianType julian  = {0};
	int        year    = 0;
	int        month   = 0;
	int        day     = 0;
	int        weekday = 0;


	GETJULIANDATE( GETTIMESECONDS(), &julian);
	if( julian.wHour >= 18)
	{
		IDateCtl_SetJulianDay( po, pme->m_lJulianDay+1);
	}

	jd_to_jewish( date_to_jd( pme->m_nYear, pme->m_nMonth, pme->m_nDay), &year, &month, &day);
	weekday = jewish_to_days( year, month, day) % 7;
	pjewish->year      = year;
	pjewish->month     = month;
	pjewish->day       = day;
	pjewish->weekday   = weekday;
	pjewish->leap      = jewish_leap( year);
	pjewish->holiday   = get_holiday( (pjewish->leap && month >= 12)?(month + 1):month, day, weekday, jewish_year_days( year), pme->m_nYear);

	if( julian.wHour >= 18)
	{
		IDateCtl_SetJulianDay( po, pme->m_lJulianDay-1);
	}

	DBGPRINTF( ";-------------------------------------");
	DBGPRINTF( ";IDateCtl_GetJewishDate");
	DBGPRINTF( ";jd, %d.%02d.%02d", pme->m_nYear, pme->m_nMonth, pme->m_nDay);
	DBGPRINTF( ";jewish, %d.%02d.%02d", year, month, day);
	if( pjewish->leap)
	{
		DBGPRINTF( ";is leap jewish year");
	}
}
#endif
#endif /*CUST_EDITION*/

