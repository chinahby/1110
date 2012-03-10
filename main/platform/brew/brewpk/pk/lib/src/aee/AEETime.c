/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEETime.c

SERVICES:  AEE Time Control (digital) Interface

GENERAL DESCRIPTION:  Provides support for ITimeCtl services
in AEE.  These include:

- Clock
- Stopwatch

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 1999-2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*=========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE_OEM.h"
#include "AEEStdLib.h"
//#include "AEETime_priv.h"
#include "AEEPointerHelpers.h"

#include "AEETime.h"
#include "OEMFeatures.h"
#include "Appscommon.h"
/*===========================================================================

DEFINITIONS

===========================================================================*/

#define AMPM_IDX                 (6)
#define MINUTE_IDX               (3)
#define SECONDS_IDX              (6)
#define IGNORE1_IDX              (2)
#define IGNORE2_IDX              (5)

#define COUNTDOWN_TIMER_MSECS    (500l)
#define STOPWATCH_TIMER_MSECS    (20l)
#define MSECS_PER_SEC            (1000l)
#define SECS_PER_HOUR            (60l * 60l)
#define SECS_PER_DAY             (SECS_PER_HOUR * 24l)
#define MSECS_PER_DAY            (SECS_PER_DAY * 1000l)
#define SNOOZE_MSECS             (15l * 60l * 1000l)  // 15 minutes...
#define MAX_TIME_STRING          (16)

#define TIMECTL_BG_COLOR      (APPSCOMMON_BG_COLOR)
/*===========================================================================

Class Definitions

===========================================================================*/

OBJECT(TimeCtl)
{
   DECLARE_VTBL(ITimeCtl)

   long           m_nRefs;          

   flg            m_bActive:1;      
   flg            m_bSendCmd:1;
   flg            m_bErase:1;
   flg            m_bSmallDisplay:1;
   flg            m_bValid:1;

   uint16         m_nCmdId;
   IShell *       m_pShell;         
   IDisplay *     m_pDisplay;       
   int32          m_dwTime;
   uint16         m_nSelect;        
   AEERect        m_rc;             
   AEECLSID       m_cls;
   uint32         m_dwProps;
   uint16         m_wMinInc;

   AEEFont        m_fntNum;
   AEEFont        m_fntLetter;

   int16          m_nFontNumWidth;
   int16          m_nFontNumHeight;
   int16          m_nFontLetterWidthDigit;
   int16          m_nFontLetterWidthChar;
   int16          m_nFontLetterHeight;

   int            m_nHitData; // -1 means nothing

   AECHAR         m_szLast[MAX_TIME_STRING + 1];

   uint32         m_dwOemProperties;
   int            m_nEditingDigit;
   RGBVAL     m_nBgColor;
};

#define IsAlarmClock(x)    ((x)->m_cls == AEECLSID_CLOCKCTL)
#define IsCountDown(x)     ((x)->m_cls == AEECLSID_COUNTDOWNCTL)
#define IsStopWatch(x)     ((x)->m_cls == AEECLSID_STOPWATCHCTL)

#define TimeCtl_SetErase(pme)  (pme->m_bErase = TRUE)

#define PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

//
// Local function prototypes
//

static uint32  ITimeCtl_AddRef(ITimeCtl * po);
static uint32  ITimeCtl_Release(ITimeCtl * po);
static boolean ITimeCtl_HandleEvent(ITimeCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static boolean ITimeCtl_Redraw(ITimeCtl * po);
static void    ITimeCtl_SetActive(ITimeCtl * po, boolean bActive);
static boolean ITimeCtl_IsActive(ITimeCtl * po);
static void    ITimeCtl_SetRect(ITimeCtl * po, const AEERect * prc);
static void    ITimeCtl_GetRect(ITimeCtl * po, AEERect * prc);
static boolean ITimeCtl_SetTime(ITimeCtl * po, int32 dwMSecsTOD, boolean bIncUpdate);
static int32   ITimeCtl_GetTime(ITimeCtl * po);
static void    ITimeCtl_SetFont(ITimeCtl * po, AEEFont fntNum, AEEFont fntLetter);
static boolean ITimeCtl_GetFont(ITimeCtl * po, AEEFont *pfntNum, AEEFont *pfntLetter);
static void    ITimeCtl_SizeToFit(ITimeCtl * po, AEERect *prc);
static void    ITimeCtl_SetProperties(ITimeCtl * po, uint32 dwProp);
static uint32  ITimeCtl_GetProperties(ITimeCtl * po);
static void    ITimeCtl_Reset(ITimeCtl * po);
static void    ITimeCtl_EnableCommand(ITimeCtl * po, boolean bEnable, uint16 nCmdID);
static void    ITimeCtl_GetTimeString(ITimeCtl * po, uint32 dwSecs, AECHAR * pDest, int nSize, uint16 wFlags);
static void    ITimeCtl_SetIncrement(ITimeCtl * po, uint16 wMins);
static void    ITimeCtl_SetEditField(ITimeCtl * po, ITField field);

static void    TimeCtl_Redraw(TimeCtl * pme, boolean bSave);
static int     TimeCtl_TimeText(TimeCtl * pme, AECHAR * pBuff,int x,boolean bDraw);
static uint32  TimeCtl_HMToMSECS(int16 wHours, int16 wMins, int16 wSecs, boolean bPM, boolean bConvert);
static int16   TimeCtl_BumpMins(int16 nMins, int16 *pnHours, boolean * pbPM, int nDir);
static boolean TimeCtl_BumpHours(int16 * pnHours, boolean * pbPM, int nDir);
static int16   TimeCtl_BumpCDTime(int16 nVal, int nDir,int16 nMax);
static void    TimeCtl_SetTextSizeCache(TimeCtl * pme);
static int     TimeCtl_FieldHit(TimeCtl * pme, int16 wXPos, int16 wYPos);

static void    ITimeCtl_SetOemProperties(ITimeCtl * po, uint32 dwProp);

static void    ITimeCtl_SetBgColor(ITimeCtl * po, RGBVAL nBgColor);
//---------------------------------------------------------------------
// Global Data Definitions
//---------------------------------------------------------------------

static const VTBL(ITimeCtl) gTimeFuncs = {         
    ITimeCtl_AddRef,                                
    ITimeCtl_Release,
    ITimeCtl_HandleEvent,
    ITimeCtl_Redraw,
    ITimeCtl_SetActive,
    ITimeCtl_IsActive,
    ITimeCtl_SetRect,
    ITimeCtl_GetRect,
    ITimeCtl_SetProperties,
    ITimeCtl_GetProperties,
    ITimeCtl_Reset,
    ITimeCtl_SetTime,
    ITimeCtl_GetTime,
    ITimeCtl_EnableCommand,
    ITimeCtl_GetTimeString,
    ITimeCtl_SetIncrement,
    ITimeCtl_SetEditField,
    ITimeCtl_SetFont,
    ITimeCtl_GetFont,
    ITimeCtl_SizeToFit,
    ITimeCtl_SetOemProperties
    ,ITimeCtl_SetBgColor
};

/*===========================================================================

EXTERNAL - Called from OEM static table.  Supports creation of time classes.

===========================================================================*/
int TimeCtl_New(IShell * ps,AEECLSID cls, void ** ppobj)
{
   TimeCtl *         pme = NULL;
   IDisplay *        pd;
   int               nErr;
   IBitmap *         pbmDst;
   AEEBitmapInfo     bi;

   if(!ps || !ppobj)
      return(EBADPARM);

   *ppobj = NULL;

  // allocate memory for object

   if((pme = (TimeCtl *)AEE_NewClass((IBaseVtbl *)&gTimeFuncs, sizeof(TimeCtl))) == NULL)
      return(ENOMEMORY);

   // Increment the reference count on the shell...

   ISHELL_AddRef(ps);

   // initialize object data

   pme->m_cls        = cls;
   pme->m_pShell     = ps;
   pme->m_wMinInc    = 1;
   pme->m_nHitData   = -1;

   if(cls == AEECLSID_CLOCKCTL)
      pme->m_dwProps = TP_NO_SECONDS;

  // create IDisplay interface

   nErr = ISHELL_CreateInstance(pme->m_pShell, AEECLSID_DISPLAY, (void **)&pd);
   if(!pd)
      goto Error;
   pme->m_pDisplay = pd;

   // get display metrics
   pbmDst = IDISPLAY_GetDestination(pme->m_pDisplay);
   if (!pbmDst) {
      nErr = EFAILED;
      goto Error;
   }
   nErr = IBITMAP_GetInfo(pbmDst, &bi, sizeof(bi));
   IBITMAP_Release(pbmDst);
   if (SUCCESS != nErr)
      goto Error;
   pme->m_rc.dx = (int16)bi.cx;
   pme->m_rc.dy = (int16)bi.cy;

   // initialize default fonts
   pme->m_fntNum = AEE_FONT_LARGE;
   pme->m_fntLetter = AEE_FONT_BOLD;
   pme->m_nBgColor = TIMECTL_BG_COLOR;

   // set text size fields
   TimeCtl_SetTextSizeCache( pme );

   *ppobj = (ITimeCtl*)pme;

   return(0);

Error:
   ITimeCtl_Release((ITimeCtl *)pme);
   return(nErr);
}

/*=====================================================================

Public Method - Increments the reference count on the menu class object.

======================================================================*/
static uint32 ITimeCtl_AddRef(ITimeCtl * po)
{
   return(++((TimeCtl*)po)->m_nRefs);
}

/*=====================================================================

Public Method - Decrements the reference count.  If count reaches 0, frees
the object.

======================================================================*/
static uint32 ITimeCtl_Release(ITimeCtl * po)
{
   TimeCtl * pme = (TimeCtl*)po;

   if(pme->m_nRefs){
      if(--pme->m_nRefs) 
         return(pme->m_nRefs);

      aee_releaseobj((void **)&pme->m_pDisplay);
      aee_releaseobj((void **)&pme->m_pShell);

      FREE(pme);
   }
   return(0);
}

/*=====================================================================

Public Method - Event handler for time control. 

======================================================================*/
#define FEATURE_EDIT_SINGLE_DIGIT   1

static boolean ITimeCtl_HandleEvent(ITimeCtl * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   TimeCtl *  pme = (TimeCtl*)po;
   int         nlrDir = 3,nDir = 1,nIdx, nMaxIdx;

   if (!pme->m_bActive)
      return(FALSE);     // did not handle the event

   if( eCode == EVT_KEY ){
      switch(wParam){

         case AVK_SELECT:
            if (pme->m_bSendCmd) 
               return(ISHELL_HandleEvent(pme->m_pShell, EVT_COMMAND, (uint16)pme->m_nCmdId, (uint32)po));
            break;

       // CHANGING FIELD VALUES...

         case AVK_DOWN:
            nDir = -1;

      // Fall Thru!!!

         case AVK_UP:
            if( IsCountDown(pme) && ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE))
            {
                return FALSE;
            }
            else
            {
            int16          nHours, nMins, nSecs,nHSecs;
            boolean        bPM;
            CtlValChange   v;
            int            nSel = pme->m_nSelect;

            aee_MSECSToHM(pme->m_dwTime, &nHours, &nMins, &nSecs, &nHSecs, (IsAlarmClock(pme) ? &bPM : NULL));
            if(nSel < IGNORE1_IDX)
			{
               if(IsCountDown(pme))
               {
               	nHours = TimeCtl_BumpCDTime(nHours, nDir, 99);
               }
               else
               {
               	TimeCtl_BumpHours(&nHours, (IsAlarmClock(pme) ? &bPM : NULL), nDir);
               }
            }
            else{
               if(nSel < IGNORE2_IDX){
                  if(nDir > 0)
                     nDir = pme->m_wMinInc;
                  else
                     nDir = -pme->m_wMinInc;
                  if(IsCountDown(pme))
                     nMins = TimeCtl_BumpCDTime(nMins, nDir, (int16)(60 - pme->m_wMinInc));
                  else
                     nMins = TimeCtl_BumpMins(nMins, &nHours, (IsAlarmClock(pme) ? &bPM : NULL), nDir);
               }
               else{
                  if(IsCountDown(pme))
                     nSecs = TimeCtl_BumpCDTime(nSecs, nDir,59);
                  else
                     bPM = !bPM;
               }
            }
      
         // See if the specified value is valid...

            v.dwParam = TimeCtl_HMToMSECS(nHours, nMins, nSecs, bPM,(boolean)(IsAlarmClock(pme) ? TRUE : FALSE));
            v.pc = (IControl *)pme;
            v.bValid = TRUE;
            if(ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_CHANGING, 0, (uint32)&v)){
               if(!v.bValid)
                  break;
            }
            pme->m_dwTime = v.dwParam;
         
            ITimeCtl_Redraw(po);
            return(TRUE);
            }

         // CHANGING FIELD FOCUS...

         case AVK_LEFT:
            nlrDir = -3;

        // Fall Thru!!!

          case AVK_RIGHT:
            nIdx = pme->m_nSelect;

#if FEATURE_EDIT_SINGLE_DIGIT
            if( IsCountDown(pme) &&
                ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE) &&
                pme->m_nEditingDigit % 2 != wParam - AVK_LEFT
            )
            {
                pme->m_nEditingDigit += nlrDir / 3;
            }
            else
#endif
            {
                nIdx += nlrDir;

            nMaxIdx = AMPM_IDX;

            switch(pme->m_cls){

               case AEECLSID_COUNTDOWNCTL:
                  if(pme->m_dwProps & TP_NO_SECONDS)
                     nMaxIdx = MINUTE_IDX;
                  else
                     nMaxIdx = SECONDS_IDX;
                  break;

               case AEECLSID_CLOCKCTL:
                  if(pme->m_dwProps & TP_NOEDIT_AMPM)
                     nMaxIdx = MINUTE_IDX;
                  break;
            }
            if(nIdx < 0){
#if !defined( AEE_SIMULATOR)
                   if(!ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)0, (uint32)po))
#endif
                  nIdx = nMaxIdx;
            }
            else{
               if(nIdx > nMaxIdx){
#if !defined( AEE_SIMULATOR)
                     if(!ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_TAB, (uint16)1, (uint32)po))
#endif
                     nIdx = 0;
               }
            }
            pme->m_nSelect = nIdx;
#if FEATURE_EDIT_SINGLE_DIGIT
                pme->m_nEditingDigit = pme->m_nSelect / 3 * 2 + ( wParam == AVK_LEFT ?  1 : 0);
#else
                pme->m_nEditingDigit = pme->m_nSelect / 3 * 2;
#endif
            }
            ITimeCtl_Redraw(po);
            return(TRUE);
     }
    if( IsCountDown( pme) &&
        ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE) &&
        wParam >= AVK_0 &&
        wParam <= AVK_9
    )
    {

        int16   timeHM[4]   = {0};
        int16   timePart[2] = {0};
        int     editField   = pme->m_nSelect / 3;
        int     digit       = wParam - AVK_0;

        int     hourLowerLimit  = 0;
        int     hourUpperLimit  = 23;
        boolean bPM             = FALSE;
        boolean ampmFormat      = pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_12_FORMAT;

        if( ampmFormat)
        {
            hourLowerLimit  = 1;
            hourUpperLimit  = 12;
        }

        if( ( pme->m_nEditingDigit < (editField * 2)) || ( pme->m_nEditingDigit > (editField * 2 + 1)))
        {
            pme->m_nEditingDigit = editField * 2;
        }

        aee_MSECSToHM(pme->m_dwTime, &timeHM[0], &timeHM[1], &timeHM[2], &timeHM[3], ampmFormat?&bPM:0);

        timePart[1] = timeHM[editField] % 10;
        timePart[0] = timeHM[editField] / 10;

        timePart[pme->m_nEditingDigit % 2] = digit;
        if( pme->m_nEditingDigit == 0 &&
            ( timePart[0] * 10 + timePart[1]) > hourUpperLimit
        )
        {
            timePart[1] = hourUpperLimit % 10;
        }
        if( pme->m_nEditingDigit == 0 &&
            ( timePart[0] * 10 + timePart[1]) < hourLowerLimit
        )
        {
            timePart[1] = hourLowerLimit % 10;
        }
        timeHM[editField] = timePart[0] * 10 + timePart[1];
        if( timeHM[0] >= hourLowerLimit && timeHM[0] <= hourUpperLimit && timeHM[1] < 60 && timeHM[2] < 100)
        {
            CtlValChange    v           = {0};
            int             maxField    = ( pme->m_dwProps & TP_NO_SECONDS) ? 3 : 6;

            v.dwParam   = TimeCtl_HMToMSECS( timeHM[0], timeHM[1], timeHM[2], bPM, ampmFormat);
            v.pc        = (IControl *)pme;
            v.bValid    = TRUE;
            if( ISHELL_HandleEvent(pme->m_pShell, EVT_CTL_CHANGING, 0, (uint32)&v))
            {
                if( !v.bValid)
                {
                    return FALSE;
                }
            }

            pme->m_dwTime = v.dwParam;

            if( pme->m_nEditingDigit % 2 == 1)
            {
                pme->m_nSelect += 3;
            }

            if( pme->m_nSelect > maxField)
            {
                pme->m_nSelect = maxField;
            }
            else
            {
                pme->m_nEditingDigit ++;
            }

            ITimeCtl_Redraw(po);
            return(TRUE);
        }

        return FALSE;
    }

    if( IsCountDown( pme) &&
        ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_12_FORMAT) &&
#if defined( AEE_SIMULATOR)
        wParam == AVK_STAR
#else
        (wParam == AVK_INFO || wParam == AVK_STAR)
#endif
    )
    {
        pme->m_dwTime += 12 * 60 * 60 * 1000;
        pme->m_dwTime %= 24 * 60 * 60 * 1000;
        ITimeCtl_Redraw(po);
        return TRUE;
     }
  }
  if( eCode == EVT_POINTER_STALE_MOVE || eCode == EVT_POINTER_MOVE ){
     // No dragging!
     return FALSE;
  }
  if(eCode == EVT_POINTER_DOWN || eCode == EVT_POINTER_UP){
      int16       wXHit    = (int16)AEE_POINTER_GET_X((const char *)dwParam);
      int16       wYHit    = (int16)AEE_POINTER_GET_Y((const char *)dwParam);
      int         nHitData = TimeCtl_FieldHit(pme, wXHit, wYHit);

      if( eCode == EVT_POINTER_UP && nHitData != -1 ){
         if( nHitData == pme->m_nHitData ){
            // Alarm clocks can't edit beyond the minute index if the noedit ampm is set
            if( !IsAlarmClock(pme) || 
               (IsAlarmClock(pme) && !(pme->m_dwProps & TP_NOEDIT_AMPM)) ||
               (IsAlarmClock(pme) && (pme->m_dwProps & TP_NOEDIT_AMPM) && nHitData <= MINUTE_IDX) ){
               pme->m_nSelect = nHitData;
               ITimeCtl_Redraw(po);
               return TRUE; // Handled Pen Up.
            }
         }
         pme->m_nHitData = -1;
      }else{
         pme->m_nHitData = nHitData;
         if( nHitData != -1 || PT_IN_RECT(wXHit, wYHit, pme->m_rc) ){
            return TRUE; // Handled Pen Down.
         }
      }
  }
  return(FALSE);   // did not handle the event
}

/*=====================================================================

Public Method - Redraws time control.  See AEETime.h

======================================================================*/
static boolean ITimeCtl_Redraw(ITimeCtl * po)
{
   TimeCtl    *  pme = (TimeCtl *)po;

   MEMSET(pme->m_szLast, 0, sizeof(pme->m_szLast));
   TimeCtl_Redraw(pme, FALSE);
   return(TRUE);
}


/*=====================================================================

Public Method - Sets the time control into edit/read-only mode.  See AEETime.h

======================================================================*/
static void ITimeCtl_SetActive(ITimeCtl * po, boolean bActive)
{
   TimeCtl * pme = (TimeCtl *)po;

   if(!IsStopWatch(pme)){
      pme->m_bActive = bActive;
      TimeCtl_SetErase(pme);
      if(!bActive)
      {
         pme->m_nSelect = 0;
         pme->m_nEditingDigit = 0;
      }
      if(pme->m_dwProps & TP_AUTOREDRAW)
         TimeCtl_Redraw(pme,FALSE);
   }
}

/*=====================================================================

Public Method - Returns TRUE if the time control is in edit mode.  See AEETime.h

======================================================================*/
static boolean ITimeCtl_IsActive(ITimeCtl * po)
{
   return(((TimeCtl *)po)->m_bActive == 0 ? FALSE : TRUE);
} 

/*=====================================================================

Public Method - Sets the rectangle for the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_SetRect(ITimeCtl * po, const AEERect * prc)
{
   TimeCtl * pme = (TimeCtl *)po;

   if(prc){
      pme->m_rc = *prc;

   // 11/14/01 - CR37083 Force minimum height to the size of the small character set...

      if(prc->dy < pme->m_nFontLetterHeight)
         pme->m_rc.dy = pme->m_nFontLetterHeight;
   }
      
}

/*=====================================================================

Public Method - Returns the rectangle for the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_GetRect(ITimeCtl * po, AEERect * prc)
{
   if(prc)
      *prc = ((TimeCtl *)po)->m_rc;
}

/*=====================================================================

Public Method - Sets the flags/properties for the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_SetProperties(ITimeCtl * po, uint32 dwProp)
{
   TimeCtl * pme = (TimeCtl *)po;

   if(pme->m_cls == AEECLSID_CLOCKCTL)
      dwProp |= TP_NO_SECONDS;
   pme->m_dwProps = dwProp;
   TimeCtl_SetErase(pme);
}

/*=====================================================================

Public Method - Returns the the flags/properties for the time control.  See AEETime.h

======================================================================*/
static uint32 ITimeCtl_GetProperties(ITimeCtl * po)
{
   return(((TimeCtl *)po)->m_dwProps);
}

/*=====================================================================

Public Method - Resets the contents of the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_Reset(ITimeCtl * po)
{
   TimeCtl * pme = (TimeCtl *)po;

   pme->m_bActive = FALSE;
   pme->m_nSelect = 0;
   pme->m_nEditingDigit = 0;
   pme->m_nHitData = -1;
   TimeCtl_SetErase(pme);
}

/*=====================================================================

Public Method - Returns the time string.  See AEETime.h

======================================================================*/
static void ITimeCtl_GetTimeString(ITimeCtl * po, uint32 dwSecs, AECHAR * pDest, int nSize, uint16 wFlags)
{
   TimeCtl    *  pme = (TimeCtl *)po;
   char           szTemp[16];
   int16          wHour, wMins,wSecs,wHSecs;
   boolean        bPM;

   aee_MSECSToHM(dwSecs, &wHour, &wMins, &wSecs, &wHSecs, ((wFlags & GTS_AMPM) ? &bPM : NULL));

   if((wFlags & GTS_MSECS) && !wHour)
      SPRINTF(szTemp,"%02d",wMins);
   else
      SPRINTF(szTemp,((pme->m_bActive || (wFlags & GTS_SECS)) ? "%02d:%02d" : "%d:%02d"),wHour,wMins);

   // need to show sec when GTS_MSECS, otherwise the unit is not right.   
   if(wFlags & GTS_SECS || wFlags & GTS_MSECS)
      SPRINTF(szTemp + STRLEN(szTemp),":%02d", wSecs);

   if(wFlags & GTS_MSECS) 
   { 
	  SPRINTF(szTemp + STRLEN(szTemp),".%02d",wHSecs);
   }

   if(wFlags & GTS_AMPM)
      STRCAT(szTemp,(bPM ? " PM" : " AM"));

   STRTOWSTR(szTemp, pDest, nSize);
}

/*=====================================================================

Public Method - Sets the increment value for the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_SetIncrement(ITimeCtl * po, uint16 wMins)
{
   ((TimeCtl *)po)->m_wMinInc = wMins;
}

/*===================================================================== 
Function: ITimeCtl_SetFont()

Description: 
   This method allows the caller to set the title and text fonts.

Prototype:

   void ITimeCtl_SetFont(ITimeCtl * pITimeCtl, AEEFont fntNum, AEEFont fntLetter);

Parameters:
   pITimeCtl : Pointer to the ITimeCtl Interface object
   fntNum: Font to use for the numbers in the time
           If this parameter is set to NULL then default font of
           AEE_FONT_LARGE will be assumed
   fntLetter: Font to use for the non-number portions of the time (AM/PM)
           If this parameter is set to NULL then default font of
           AEE_FONT_BOLD will be assumed

Return Value: 
   None

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=====================================================================*/ 
static void ITimeCtl_SetFont(ITimeCtl * po, AEEFont fntNum, AEEFont fntLetter)
{
   TimeCtl * pme = (TimeCtl*)po;

   if(!fntNum)
      fntNum = AEE_FONT_LARGE;
   if(!fntLetter)
      fntLetter = AEE_FONT_BOLD;

   if (!BETWEEN(fntNum,AEE_FONT_NORMAL,AEE_FONT_TOTAL) ||
       !BETWEEN(fntLetter,AEE_FONT_NORMAL,AEE_FONT_TOTAL)) {
      return;
   }

   pme->m_fntNum = fntNum;
   pme->m_fntLetter = fntLetter;

   // set text size fields
   TimeCtl_SetTextSizeCache( pme );
}

/*===================================================================== 

Function: ITimeCTL_GetFont()

Description: 
   This method allows the caller to get the time number and letter fonts.

Prototype:

   boolean ITimeCTL_GetFont(ITimeCtl * pITimeCtl, AEEFont *pfntNum, AEEFont *pfntLetter);

Parameters:
   pITimeCtl : Pointer to the ITimeCtl Interface object
   pfntNum: pointer to location to set to number font
   pfntLetter: pointer to location to set to letter (AM/PM) font

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
static boolean ITimeCtl_GetFont(ITimeCtl * po, AEEFont *pfntNum, AEEFont *pfntLetter)
{
   TimeCtl * pme = (TimeCtl*)po;

   if( pfntNum == NULL || pfntLetter == NULL ) {
      return FALSE;
   }

   *pfntNum = pme->m_fntNum;
   *pfntLetter  = pme->m_fntLetter;

   return TRUE;
}

/*===================================================================== 

Function: ITimeCTL_SizeToFit()

Description: 
   This method allows the caller to size the rect to fit the time text.

Prototype:

   void ITimeCTL_SizeToFit(ITimeCtl * pITimeCtl, AEERect *prc);

Parameters:
   pITimeCtl : Pointer to the ITimeCtl Interface object
   prc: pointer to location to set to bounding rect

Return Value: 
    none

Comments:  
   None

Side Effects: 
   None

See Also: 
   None

=====================================================================*/ 
static void ITimeCtl_SizeToFit(ITimeCtl * po, AEERect *prc)
{
   TimeCtl *  pme = (TimeCtl*)po;
   AECHAR     szBuff[16];
   int        nWidth, nHeight;
   uint16     wFlags = 0;

   if( prc == NULL)
      return;

   if(!(pme->m_dwProps & TP_NO_SECONDS))
      wFlags |= GTS_SECS;

   switch(pme->m_cls){

      case AEECLSID_CLOCKCTL:
        //ERR("pme->m_dwProps = %x",pme->m_dwProps,0,0);
        if(!(pme->m_dwProps & TP_24_FORMAT))
        {
            wFlags |= GTS_AMPM;
        }
        break;

      case AEECLSID_STOPWATCHCTL:
         if(!(pme->m_dwProps & TP_NO_MSECONDS))
            wFlags |= GTS_MSECS;
         break;

      case AEECLSID_COUNTDOWNCTL:
          if( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_12_FORMAT)
          {
              wFlags |= GTS_AMPM;
          }
          break;
   }

   ITimeCtl_GetTimeString((ITimeCtl *)pme, pme->m_dwTime, szBuff, sizeof(szBuff), wFlags);
   nWidth = TimeCtl_TimeText(pme, szBuff, -1,FALSE);

   if (pme->m_nFontNumHeight  > pme->m_nFontLetterHeight)
   {
      nHeight = pme->m_nFontNumHeight;
   }  
   else
   {
      nHeight = pme->m_nFontLetterHeight;
   }

   if(nWidth < prc->dx)
      prc->dx = nWidth;
   if(nHeight < prc->dy)
      prc->dy = nHeight;

   ITimeCtl_SetRect(po, prc);
}

/*=====================================================================

Public Method - Sets the active edit field.  See AEETime.h

======================================================================*/
static void ITimeCtl_SetEditField(ITimeCtl * po, ITField field)
{
   ((TimeCtl *)po)->m_nSelect = (uint16)field * 3;

     ((TimeCtl *)po)->m_nEditingDigit = field * 2;
}

/*=====================================================================

Public Method - Sets the time for the time control.  See AEETime.h

======================================================================*/
static boolean ITimeCtl_SetTime(ITimeCtl * po, int32 tod, boolean bIncUpdate)
{
   TimeCtl * pme = (TimeCtl *)po;

   pme->m_bValid = FALSE;

   if(tod >= 0){

      if(IsAlarmClock(pme) && tod > (SECS_PER_HOUR * 1000 * 24))
         return(FALSE);

      if(!bIncUpdate && pme->m_szLast[0])
         MEMSET(pme->m_szLast, 0, sizeof(pme->m_szLast));

      pme->m_bValid = TRUE;
      pme->m_dwTime = tod;
      TimeCtl_Redraw(pme, bIncUpdate);
      return(TRUE);
   }
   return(FALSE);
}

/*=====================================================================

Public Method - Returns the time from the time control.  See AEETime.h

======================================================================*/
static int32 ITimeCtl_GetTime(ITimeCtl * po)
{
   return(((TimeCtl *)po)->m_dwTime);
}

/*=====================================================================

Public Method - Enables the EVT_COMMAND for the time control.  See AEETime.h

======================================================================*/
static void ITimeCtl_EnableCommand(ITimeCtl * po, boolean bEnable, uint16 nCmdId)
{
   TimeCtl * pMe = (TimeCtl*)po;

   pMe->m_bSendCmd = (bEnable != 0);

   if (bEnable)
      pMe->m_nCmdId = nCmdId;
}


/*=====================================================================




  LOCAL METHODS



======================================================================*/
/*=====================================================================

Local Method - Fills the input string with the time.  Also calculates the size.

======================================================================*/
static int TimeCtl_TimeText(TimeCtl * pme, AECHAR * pBuff,int x,boolean bDraw)
{
   IDisplay    *  pd = pme->m_pDisplay;
   boolean        bAlready = FALSE, bAllSmall;
   int16          nTotalWidth, nFontNumWidth;
   int16          nFontLetterWidthDigit, nFontLetterWidthChar;
   AECHAR  *      pch,ch,*pchLast;
   int            nFontNumHeight, nFontLetterHeight,i, nHiIdx, ySmall, yLarge, yColon;
   AEEFont        fnt;
   AEERect        rc;
   uint32         dwFlags = 0;
   RGBVAL       nOldFontColor;
   int nResult = AEE_SUCCESS;

   nHiIdx = (pme->m_bActive ? pme->m_nSelect : -1);

   nTotalWidth = 0;

   // Y for small characters is up from bottom of y for large characters...

   nFontLetterHeight = pme->m_nFontLetterHeight;
   nFontNumHeight = pme->m_nFontNumHeight;

   nFontLetterWidthDigit = pme->m_nFontLetterWidthDigit;
   nFontLetterWidthChar  = pme->m_nFontLetterWidthChar;
   nFontNumWidth         = pme->m_nFontNumWidth;

   if (nFontNumHeight > pme->m_rc.dy || (bDraw && (x < 0)))
   {
      bAllSmall = TRUE;
      if (x < 0)
      {
      	x = 0;
	  }
      ySmall = yColon = yLarge = pme->m_rc.y;
      nFontNumHeight = nFontLetterHeight;
   } 
   else
   {
      bAllSmall = FALSE;

      yLarge = pme->m_rc.y + ((pme->m_rc.dy - nFontNumHeight) / 2);

   // Set normal (AM/PM, etc.) up from the bottom of the large numbers and 
   // colons in the middle...

      ySmall = (yLarge + nFontNumHeight) - nFontLetterHeight;
      yColon = yLarge + ((nFontNumHeight - nFontLetterHeight) / 2);
   }

   // Erase flag may have been set directly or by transition from small to large text, etc.
#ifndef CUST_EDITION   /*²¿·ÖÓ¦ÓÃ³öÏÖ°×ÆÁµÄÔ­Òò*/
   IDisplay_ClearScreen(pd);
#endif 
   if (bAllSmall != pme->m_bSmallDisplay)
   {
      pme->m_bErase = TRUE;
      pme->m_bSmallDisplay = bAllSmall;
   }

   if (IsCountDown( pme) && ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE) && !pme->m_bActive)
   {
       pme->m_bErase = TRUE;
   }

   if (pme->m_bErase && bDraw)
   {
	  // Reset the last time displayed...
      MEMSET(pme->m_szLast, 0, sizeof(pme->m_szLast));
      IDISPLAY_FillRect(pd, &pme->m_rc, CLR_USER_BACKGROUND);
      pme->m_bErase = FALSE;
   }
    if ( IsStopWatch( pme))
    {    	
        if(pme->m_dwOemProperties & TP_OEM_CUSTOM_BG_COLOR)
        {
            Appscommon_ResetBackgroundEx(pd, &pme->m_rc, FALSE);
        }
        else
        {
            IDISPLAY_FillRect(pd, &pme->m_rc, pme->m_nBgColor/*RGB_BLACK*/);
        }
    }

   // Draw or measure each character...

   rc.x = x;

   for(i = 0, pchLast = pme->m_szLast, pch = pBuff; (ch = *pch) != 0; pch++, i++, pchLast++)
   {
     nResult = EFAILED;

      if(!bAllSmall &&  (ch <= (AECHAR)'9' && ch >= (AECHAR)'0'))
	  {
         fnt = pme->m_fntNum;

         rc.y = yLarge;
         rc.dx = nFontNumWidth;
         rc.dy = nFontNumHeight;
      }
      else
	  {
         if(ch == (AECHAR)'.')
         {
         	bAllSmall = TRUE;
         }
         fnt = pme->m_fntLetter;
         rc.y = (ch == (AECHAR)':' ? yColon : ySmall);
         rc.dy = nFontLetterHeight;
         // If ch is part of AM or PM use char width, othewise use digit width
         rc.dx = ((AECHAR)'A' <= ch && ch <= (AECHAR)'Z') ? nFontLetterWidthChar :  nFontLetterWidthDigit;
         
      }


      // Only draw when we need to!!!
      if (bDraw && (ch != *pchLast || pme->m_bActive))
	  {
         dwFlags = 0;		 
         // Not Left Justified so must CENTER
         if(!(pme->m_dwProps & TP_LEFT_JUSTIFY))
         {
         	dwFlags |= IDF_ALIGN_CENTER;
         }		 	

#if FEATURE_EDIT_SINGLE_DIGIT
            if( IsCountDown( pme) && ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE))
            {            	
                if( ( i - pme->m_nSelect / 3) == pme->m_nEditingDigit)
                {                	
                    dwFlags |= IDF_TEXT_INVERTED;
                }                
            }
            else
#endif
            if (nHiIdx >= 0 && nHiIdx <= i)
            {            	
                dwFlags |= IDF_TEXT_INVERTED;
                if (bAlready)
                {
                	nHiIdx = -1;
                }
                else
                {
                	bAlready = TRUE;
                }
            }            

		   if ( IsStopWatch( pme))
            {            	            	
                nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, RGB_WHITE);
                dwFlags |= IDF_TEXT_TRANSPARENT;
            }

            else if( IsCountDown( pme) && ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_EDITABLE))
            { 
				if( !pme->m_bActive)
				{	
                    nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, 0x84848400);
                	dwFlags &= ~IDF_TEXT_INVERTED;
                	fnt = AEE_FONT_BOLD;
				}
				else if( fnt == AEE_FONT_BOLD)
				{	
					IDISPLAY_FillRect( pd, &rc, CLR_USER_BACKGROUND);
				}	
				//Add By zzg 2010_07_07
				else
				{
					nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, 0x84848400);	
				}
				//Add End
            }
            /* must get the former font color and then set it back, or some interface might display the wrong color!*/
            else
            {               	
                nOldFontColor = IDISPLAY_SetColor(pd, CLR_USER_TEXT, RGB_BLACK);
            }
			
         	IDISPLAY_DrawText(pd,fnt,pch,1,rc.x,rc.y,&rc,dwFlags);
      }

      if(dwFlags & IDF_ALIGN_CENTER)
	  {

        int nWidth = 0, nFits = 0;
        nWidth = IDisplay_MeasureTextEx(pd, fnt, pch, 1, rc.dx, &nFits);

        /* handling ALIGH_CENTER case */
        nWidth += (rc.dx - nWidth)/2;

        nTotalWidth += nWidth;
        rc.x += nWidth;
        nResult = SUCCESS;
      }

      if(SUCCESS != nResult)
	  {
        nTotalWidth += rc.dx;
        rc.x += rc.dx;
      }
   }
   return(nTotalWidth);
}

/*=====================================================================

Local Method - Redraws the time control.

======================================================================*/
static void TimeCtl_Redraw(TimeCtl * pme, boolean bSave)
{
   AECHAR     szBuff[16];
   int            nWidth;
   int            x = pme->m_rc.x;
   uint16         wFlags;
   uint32         dwProps = pme->m_dwProps;

   if(!pme->m_bValid){
      IDISPLAY_EraseRect(pme->m_pDisplay, &pme->m_rc);
      IDISPLAY_Update(pme->m_pDisplay);    // Post a deferred update event...
      return;
   }
   
   wFlags = 0;

   if(!(dwProps & TP_NO_SECONDS))
      wFlags |= GTS_SECS;

   switch(pme->m_cls){

      case AEECLSID_CLOCKCTL:
        //ERR("pme->m_dwProps1 = %x",pme->m_dwProps,0,0);
        if(!(pme->m_dwProps & TP_24_FORMAT))
        {
             wFlags |= GTS_AMPM;
        }
        break;

      case AEECLSID_STOPWATCHCTL:
         if(!(dwProps & TP_NO_MSECONDS))
            wFlags |= GTS_MSECS;
         break;

      case AEECLSID_COUNTDOWNCTL:
          if( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_12_FORMAT)
          {
              wFlags |= GTS_AMPM;
          }
          break;
   }

   ITimeCtl_GetTimeString((ITimeCtl *)pme, pme->m_dwTime, szBuff, sizeof(szBuff), wFlags);

   nWidth = TimeCtl_TimeText(pme, szBuff, -1,FALSE);

   if(!(dwProps & TP_LEFT_JUSTIFY)) // Not Left Justified so CENTER text
      x += ((pme->m_rc.dx - nWidth) / 2);

   TimeCtl_TimeText(pme, szBuff, x,TRUE);

   // Save the last buffer.  We only draw what we need...

   if(bSave)
      WSTRNCOPYN(pme->m_szLast, MAX_TIME_STRING, szBuff, -1);

   IDISPLAY_Update(pme->m_pDisplay);    // Post a deferred update event...
}

/*==================================================================
Local Method - This function takes HH, MM and SS of HH:MM:SS time and returns 
number of milli seconds expired between 00:00:00 and HH:MM:SS. The 
behavior of this function is dependent upon bConvert flag. If 
bConvert flag is true, HH:MM:SS is treated as time shown by a alarm 
clock. Otherwise this is treated as time of stop watch or countdown 
device.

==================================================================*/
static uint32 TimeCtl_HMToMSECS(int16 wHours, int16 wMins, int16 wSecs, boolean bPM, boolean bConvert)
{
   if(bConvert){
      if(!bPM){
         if(wHours == 12)
            wHours = 0;
      }
      else{
         if(wHours < 12)
            wHours += 12;
      }
   }
   return(((wHours * SECS_PER_HOUR) + (wMins * 60) + wSecs) * MSECS_PER_SEC);
}

/*==================================================================

Local Method - This function is used to increase/decrease hour for HH:MM:SS 
according to nDir

==================================================================*/
static boolean TimeCtl_BumpHours(int16 * pnHours, boolean * pbPM, int nDir)
{
   int16 nHours = *pnHours, nInc;

   nInc = (nDir > 0 ? 1 : -1);

   nHours += nInc;

   // If we dropped the number of hours below 0, blow it off...

   if(nHours < 0) {
      return(FALSE);
   }

   if(nHours > 12)
      nHours = 1;
   else{
      if(nHours < 1)
         nHours = 12;
      else
         if(pbPM && 
            ((nHours == 11 && nInc < 0) || (nHours == 12 && nInc > 0)))
            *pbPM = !*pbPM;
   }
   *pnHours = nHours;
   return(TRUE);
}

/*==================================================================

Local Method - This function is used to increase/decrease minutes for HH:MM:SS 
   according to nDir

==================================================================*/
static int16 TimeCtl_BumpMins(int16 nMins, int16 *pnHours, boolean * pbPM, int nDir)
{
   nMins += nDir;

   if(nMins > 59){
      TimeCtl_BumpHours(pnHours, pbPM, nDir);
      return(0);
   }
   if(nMins < 0){
      TimeCtl_BumpHours(pnHours,pbPM,nDir);
      return(60 + nDir);
   }
   return(nMins);
}

/*===========================================================================

Local Method - This is helper function used to calculate new value of countdown timer 
corresponding to the specified value, specified change and maximum limit.

===========================================================================*/
static int16 TimeCtl_BumpCDTime(int16 nVal, int nDir, int16 nMax)
{
   nVal += nDir;
   if(nVal > nMax)
      nVal = 0;
   else{
      if(nVal < 0)
         nVal = nMax;
   }
   return(nVal);
}

/*===========================================================================

Local Method - Sets the text size fields when the font has been set.

===========================================================================*/
static void TimeCtl_SetTextSizeCache(TimeCtl * pme)
{
   AECHAR            szBuff[2];

   pme->m_nFontNumHeight    = IDISPLAY_GetFontMetrics(pme->m_pDisplay,
                                                      pme->m_fntNum,
                                                      NULL, NULL);
   pme->m_nFontLetterHeight = IDISPLAY_GetFontMetrics(pme->m_pDisplay,
                                                      pme->m_fntLetter,
                                                      NULL, NULL);

   szBuff[0] = (AECHAR)'0';
   szBuff[1] = 0;

   pme->m_nFontNumWidth = IDISPLAY_MeasureText(pme->m_pDisplay,
                                               pme->m_fntNum,
                                               (const AECHAR *) szBuff);
   pme->m_nFontLetterWidthDigit =
      1 + IDISPLAY_MeasureText(pme->m_pDisplay,
                               pme->m_fntLetter,
                               (const AECHAR *) szBuff);

   // Use 'M' because it's wider than 'A' and 'P'
   szBuff[0] = (AECHAR)'M';
   pme->m_nFontLetterWidthChar =
      1 + IDISPLAY_MeasureText(pme->m_pDisplay,
                               pme->m_fntLetter,
                               (const AECHAR *) szBuff);
}
/*===========================================================================

Local Method - Finds out which field has been hit by pen event (-1 is no field)

===========================================================================*/
static int TimeCtl_FieldHit(TimeCtl * pme, int16 wXPos, int16 wYPos)
{
   if(PT_IN_RECT(wXPos, wYPos, pme->m_rc)){
      uint16      wFlags = 0;
      boolean     bAllSmall;
      AECHAR *    pch,ch,*pchLast,szwBuff[16];
      int         i,x=0,ySmall, yLarge, yColon;
      AEERect     rc;

      if(!(pme->m_dwProps & TP_NO_SECONDS))
         wFlags |= GTS_SECS;

      if(pme->m_cls == AEECLSID_CLOCKCTL){
         wFlags |= GTS_AMPM;
      }

      if( IsCountDown( pme) && ( pme->m_dwOemProperties & TP_OEM_COUNTDOWNCTL_12_FORMAT))
      {
          wFlags |= GTS_AMPM;
      }
      ITimeCtl_GetTimeString((ITimeCtl *)pme, pme->m_dwTime, szwBuff, sizeof(szwBuff), wFlags);

      // Not Left Justified so CENTER text
      if(!(pme->m_dwProps & TP_LEFT_JUSTIFY)){
         x += ((pme->m_rc.dx - (TimeCtl_TimeText(pme, szwBuff, -1,FALSE))) / 2);
      }

      if(pme->m_nFontNumHeight > pme->m_rc.dy || (x < 0)){
         bAllSmall = TRUE;
         if(x < 0)
            x = 0;
         ySmall = yColon = yLarge = pme->m_rc.y;
      }else{
         bAllSmall = FALSE;
         yLarge = pme->m_rc.y + ((pme->m_rc.dy - pme->m_nFontNumHeight) / 2);

         // Set normal (AM/PM, etc.) up from the bottom of the large numbers and 
         // colons in the middle...
         ySmall = (yLarge + pme->m_nFontNumHeight) - pme->m_nFontLetterHeight;
         yColon = yLarge + ((pme->m_nFontNumHeight - pme->m_nFontLetterHeight) / 2);
      }

      rc.x = x;
      for(i = 0, pchLast = pme->m_szLast, pch = szwBuff; (ch = *pch) != 0; pch++, i++, pchLast++){

         if(!bAllSmall &&  (ch <= (AECHAR)'9' && ch >= (AECHAR)'0')){

            rc.y = yLarge;
            rc.dx = pme->m_nFontNumWidth;
            rc.dy = pme->m_nFontNumHeight;
         }else{
            if(ch == (AECHAR)'.')
               bAllSmall = TRUE;
            rc.y = (ch == (AECHAR)':' ? yColon : ySmall);
            rc.dy = pme->m_nFontLetterHeight;
            // If ch is part of AM or PM use char width, othewise use digit width
            rc.dx = ((AECHAR)'A' <= ch && ch <= (AECHAR)'Z') ?
               pme->m_nFontLetterWidthChar :  pme->m_nFontLetterWidthDigit;
         }
         if( PT_IN_RECT(wXPos, wYPos, rc) && ((i+1)%0x3) ){
            return ((i/3)*3);
         }
         rc.x += rc.dx;
         if( rc.x > wXPos ){
            break;
         }
      }
   }
   return -1;
}

static void ITimeCtl_SetOemProperties(ITimeCtl * po, uint32 dwProp)
{
   TimeCtl * pme = (TimeCtl *)po;

   pme->m_dwOemProperties = dwProp;
}
static void    ITimeCtl_SetBgColor(ITimeCtl * po, RGBVAL nBgColor)
{
    TimeCtl * pme = (TimeCtl *)po;

    if(NULL != pme)
    {
        pme->m_nBgColor = nBgColor;
    }
}
