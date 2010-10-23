/*
  ========================================================================

  FILE:  DateTimeWidget.c
  
  SERVICES:  Widgets providing display and entry of date/time

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEEFont.h"
#include "AEEHFontOutline.h"
#include "DateTimeWidget.h"
#include "AEEDisplayCanvas.h"
#include "AEEWProperties.h"
#include "ModelBase.h"
#include "modres.h"
#include "wutil.h"
#include "widget_res.h"

#define ME_FROM_WIDGET   DateWidget *me = (DateWidget*)po

/* 
   Date/Time pattern specs
   
   ___ Allowable format patterns for date ___
   
   d | dd | ddd | dddd              day, (two digits)
   M | MM | MMM | MMMM              month (two digits)
   y | yy | yyy                     year (two digits)
   Y | yyyy                         year (four digits)
   
   
   ___ Allowable format patterns for time ___
   
   h | hh                           hour, (1-12, two digits)
   H | HH                           hour, (1-24, two digits)
   m | mm                           minute, (two digits)
   s | ss                           seconds, (two digits)
   t                                AM/PM designator (single char)
   T | tt                           AM/PM designator (two chars) 


   (all other chars in format string are ignored)
*/

static const AECHAR gwszDefTimePattern[] = {'h','m','T', 0};
static const AECHAR gwszDefDatePattern[] = {'M','d','Y', 0};

static const AECHAR gwszDateDelim[] = {'/', 0};
static const AECHAR gwszTimeDelim[] = {':', 0};
static const AECHAR gwszSpecDelim[] = {' ', 0};

static const AECHAR gwszAM[]        = {'A','M', 0};
static const AECHAR gwszPM[]        = {'P','M', 0};


#define ISDAYCHAR(c)    ((c) == (AECHAR)'d')
#define ISMONCHAR(c)    ((c) == (AECHAR)'M')
#define ISYEARCHAR(c)   ((c) == (AECHAR)'y' || (c) == (AECHAR)'Y')
#define ISHOURCHAR(c)   ((c) == (AECHAR)'h' || (c) == (AECHAR)'H')
#define ISMINCHAR(c)    ((c) == (AECHAR)'m')
#define ISSECCHAR(c)    ((c) == (AECHAR)'s')
#define ISAMPMCHAR(c)   ((c) == (AECHAR)'t' || (c) == (AECHAR)'T')

#ifndef IFONT_DRAW_TRANSPARENT
#define IFONT_DRAW_TRANSPARENT     IDF_TEXT_TRANSPARENT
#endif


#define YEAR_FLD_MIN             1980
#define YEAR_FLD_MAX             2115
#define JULIAN_YEAR_FLD_MIN      0
#define JULIAN_YEAR_FLD_MAX      9999


#define FLDPAD    1

#define MASK_NOALPHA 0xFFFFFF00

#define RES_FILE     "widgets.mif"

#define IS_LEAP(y)   ((((y)%4)==0) && (((y)%100) || (((y)%400)==0)))

/////////////////////////////////////////////////////////////////////////////
// forward decls

static void DateWidget_SetValue(DateWidget *me);
static void DateWidget_ToggleAMPMValue(DateWidget *me);



/////////////////////////////////////////////////////////////////////////////
// DtFld access methods

static __inline void DtFld_Ctor(DtFld *me, uint16 *pnVal, uint16 nMin, uint16 nMax, 
                                uint8 nLen, uint8 wFlags) {
   me->pnVal      = pnVal;
   me->nMin       = nMin;
   me->nMax       = nMax;
   me->nLen       = nLen;
   me->wFlags     = wFlags;
}


static __inline uint16 DtFld_GetVal(DtFld *me) {
   return *me->pnVal;
}


/////////////////////////////////////////////////////////////////////////////
// misc utilities

static __inline IFont_MeasureStringEx(IFont *me, const AECHAR *pw, int cchLen) {
   int cchFits, nPixels = 0;
   IFONT_MeasureText(me, pw, cchLen, IFONT_MAXWIDTH, &cchFits, &nPixels);
   return nPixels;
}

static __inline IFont_MeasureString(IFont *me, const AECHAR *pw) {
   return IFont_MeasureStringEx(me, pw, WSTRLEN(pw));
}

// Notify through view model of field change
static __inline void DateWidget_NotifyFieldChange(DateWidget *me, int nNewField, boolean bDir) {
   if (me->base.piViewModel) {
      ModelEvent mev;
      mev.evCode  = EVT_MDL_FIELDCHANGE;
      mev.dwParam = (uint32)MAKELONG(nNewField, (int)bDir);
      IMODEL_Notify(me->base.piViewModel, &mev);
   }
}

/////////////////////////////////////////////////////////////////////////////

// fix up a JulianType structure and make sure it's
// date and time range is valid.  Use BREW's JulianType
// conversion routines for this
static boolean FixJulianDate(JulianType *pjt)
{
   JulianType old = *pjt;
   uint16 wYear;
   uint32 dwSecs;
   
   // adjust year so we get a valid reference time
   wYear = pjt->wYear;
   if (IS_LEAP(pjt->wYear)) {
      pjt->wYear = 2000;
   } else {
      pjt->wYear = 2001;
   }

   // convert to seconds and back.  This will ensure
   // all the date and time fields are set to something
   // within range.
   dwSecs = JULIANTOSECONDS(pjt);
   GETJULIANDATE(dwSecs, pjt);

   //restore the year
   pjt->wYear = wYear;

   // return TRUE if something changed
   return (0 != MEMCMP(&old, pjt, sizeof(JulianType)));
}

// fix up certain members of the JulianType struct for 
// display (12-hour time and 2-digit year)
static void DateWidget_UseValue(DateWidget *me)
{
   if (me->fJulianMode == 0) {
      // Non-Julian Date mode.  Validate our internal date
      uint32 dwSecs = JULIANTOSECONDS(&me->julian);
      GETJULIANDATE(dwSecs, &me->julian);
   }

   // time widget -- fix hour field for 12-hour time
   if (me->fTimeMode) {
      me->nPM = (me->julian.wHour >= 12);
   
      if (!me->f24HourTime) {
         if (me->julian.wHour == 0) {
            me->julian.wHour = 12;
         } else if (me->julian.wHour > 12) {
            me->julian.wHour -= 12;
         }
      }
   
   // date widget -- fix year field for 2-digit year
   } else if (me->f2DigitYear) {
      me->julian.wYear %= 100;
   }
}

// update the value model with the contents of the internal
// JulianType struct -- fix up JulianType members, as needed, 
// before conversion to seconds
static void DateWidget_SetValue(DateWidget *me)
{
   if (me->fDirty) {
      uint32 secs;

      // need special handling for 12 hour time, since JulianType
      // uses 24 hour time
      if (!me->f24HourTime) {

         if (me->julian.wHour == 12) {
            if (!me->nPM) {
               me->julian.wHour = 0;
            }
         } else if (me->nPM && me->julian.wHour < 12) {
            me->julian.wHour += 12;
         } else if (!me->nPM && me->julian.wHour > 12) {
            me->julian.wHour -= 12;
         }

      }

      // handle 2-digit years
      if (!me->fTimeMode && me->f2DigitYear) {
         // values in range 80-99 are evaluated as 1980-1999
         if (me->julian.wYear >= 80 && me->julian.wYear <= 99) {
            me->julian.wYear += 1900;
         // all other values go to 20xx
         } else {
            me->julian.wYear += 2000;
         }
      }

      if (me->fJulianMode) {
         IWIDGET_SetJulianDateTime((IWidget *)me, &me->julian);
      } else {
         secs = JULIANTOSECONDS(&me->julian);
         IVALUEMODEL_SetValue((IValueModel *)me->base.piModel, (void*)secs, 1, 0);
      }
      me->fDirty = 0;
   }
}


static void DateWidget_ToggleAMPMValue(DateWidget *me)
{
   if (me->f24HourTime) {
      if (!me->nPM && me->julian.wHour == 12) {
         me->julian.wHour = 0;
      } else if (me->nPM) {
         me->julian.wHour += 12;
      } else {
         me->julian.wHour -= 12;
      }
   }
}

static void DateWidget_UpdateAndUseInternalDate(DateWidget *me)
{
   if (me->base.piModel) {
      if (me->fJulianMode) {
         me->julian = *((JulianType *)(IVALUEMODEL_GetValue((IValueModel *)me->base.piModel, NULL)));
         if (FixJulianDate(&me->julian)) {
            IWIDGET_SetJulianDateTime((IWidget *)me, &me->julian);
            return;  // IWIDGET_SetJulianDateTime() calls DateWidget_UseValue().  We're done!
         }
      } else {
         uint32 secs = (uint32)IVALUEMODEL_GetValue((IValueModel *)me->base.piModel, NULL);
         GETJULIANDATE(secs, &me->julian);
      }
      DateWidget_UseValue(me);
   }
}

static void DateWidget_ModelListener(DateWidget *me, ModelEvent *pEvent) 
{
   if (pEvent->evCode == EVT_MDL_VALUE) {
      DateWidget_UpdateAndUseInternalDate(me);
      WidgetBase_InvalidateContent((IWidget*)me);
   }
}


static void DateWidget_FontRecalc(DateWidget *me)
{
   AEEFontInfo fi;
   static const AECHAR wch = (AECHAR)'0';

   // get the width of a typical numeric digit in this font 
   me->cxFont = IFont_MeasureStringEx(me->piFont, &wch, 1);
   
   // get the font height
   IFONT_GetInfo(me->piFont, &fi, sizeof(fi));
   me->cyFont = fi.nAscent + fi.nDescent;
}


static void DateWidget_FontChanged(DateWidget *me, ModelEvent *pEvent)
{
   IHFont *piHFont;

   if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
      // we're an IHFont and the size of our characters may have changed
      DateWidget_FontRecalc(me);
      RELEASEIF(piHFont);
   }
   WidgetBase_InvalidateContent((IWidget*)me);
}


static void DateWidget_UseFont(DateWidget *me, IFont *piFont)
{
   IModel *piModel = 0;

   LISTENER_Cancel(&me->mlFont);

   if (piFont) {    

      // recalculate
      DateWidget_FontRecalc(me);

      // if font supports IModel, attach a listener for IHFonts
      if (SUCCESS == IFONT_QueryInterface(me->piFont, AEEIID_MODEL, (void**)&piModel)) {
         IMODEL_AddListenerEx(piModel, &me->mlFont, (PFNLISTENER)DateWidget_FontChanged, me);
         IMODEL_Release(piModel);
      }
      WidgetBase_InvalidateContent((IWidget*)me);
   }
}


static void DateWidget_SetFont(DateWidget *me, IFont *piFont)
{
   if (piFont) {
     
      RELEASEIF(me->piFont);
      me->piFont = piFont;
      IFONT_AddRef(me->piFont);

      DateWidget_UseFont(me, piFont);
   }
}


static __inline void DateWidget_CalcFieldRect(DateWidget *me, int nField, AEERect *prc) {

   if (me->aFlds[nField].wFlags & FFLG_AMPM) {
      int cxA = IFont_MeasureStringEx(me->piFont, me->pwAM, (me->fShortAMPM ? 1 : WSTRLEN(me->pwAM)));
      int cxP = IFont_MeasureStringEx(me->piFont, me->pwPM, (me->fShortAMPM ? 1 : WSTRLEN(me->pwPM)));
      SETAEERECT(prc, 0, 0, MAX(cxA,cxP) + (me->nFldPad*2), me->cyFont + (me->nFldPad*2));
   
   } else {
      SETAEERECT(prc, 0, 0, 
                 (me->aFlds[nField].nLen * me->cxFont) + (me->nFldPad*2), 
                 me->cyFont + (me->nFldPad*2));
   }
}


/*
   --------------------------------------------------------
   Checks if field value is within Min and Max values. 
   
   Allows rollover from min->max and max->min if bRollover 
   flags is on, otherwise, simply constrains value to be
   within min and max
   --------------------------------------------------------
*/
static void DateWidget_CheckField(DateWidget *me, DtFld *pFld, boolean bRollover)
{
   int16 fldval = *(pFld->pnVal);

   if (fldval < pFld->nMin) {
      fldval = bRollover ? pFld->nMax : pFld->nMin;
   }
   if (fldval > pFld->nMax) {
      fldval = bRollover ? pFld->nMin : pFld->nMax;
   }

   *(pFld->pnVal) = fldval;
}



/*
   --------------------------------------------------------
   Checks date value to ensure that it is a valid date. 
   Knows about days per month and leap years.
   --------------------------------------------------------
*/
static void DateWidget_ValidateDate(DateWidget *me)
{
   // normalized array of month days [0]=not used, [1]=Jan, [12]=Dec
   static const uint8 aNormMonths[] = {
      0,31,28,31,30,31,30,31,31,30,31,30,31
   };
   
   // (leap year) 
   // normalized array of month days [0]=not used, [1]=Jan, [12]=Dec
   static const uint8 aLeapMonths[] = {
      0,31,29,31,30,31,30,31,31,30,31,30,31
   };

   uint16 nMaxDay;

   if (IS_LEAP(me->julian.wYear)) {
      nMaxDay = aLeapMonths[me->julian.wMonth];
   } else {
      nMaxDay = aNormMonths[me->julian.wMonth];
   }

   // fix up the maxium value in the day field
   me->aFlds[me->ndxDay].nMax = nMaxDay;
   // and check the value against it
   DateWidget_CheckField(me, &me->aFlds[me->ndxDay], 0);
}


static void DateWidget_EnterField(DateWidget *me, uint16 wParam)
{
   // find field
   DtFld *pFld = me->aFlds + me->nField;

   uint16 fldval = *(pFld->pnVal);
   uint16 keyval = wParam - AVK_0;

   // special case for AM/PM field
   if (wParam == AVK_SELECT) {
      if (pFld->wFlags & FFLG_AMPM) {
         *(pFld->pnVal) = !fldval;
         me->fDirty = 1;
         // update the value model if on-field-change is enabled
         if (me->dwFlags & DTWF_UPDATE_ONFIELDCHANGE) {
            DateWidget_ToggleAMPMValue(me);
            DateWidget_SetValue(me);
         }
         WidgetBase_InvalidateContent((IWidget*)me);
      }
      return;
   }

   // handle normal numeric keys
   if (me->nDigit) {
      fldval *= 10;
   } else {
      fldval = 0;
   }

   me->nDigit++;

   fldval += keyval;
   *(pFld->pnVal) = fldval;
   me->fDirty = 1;

   // if field is full or no more digits allowed, force move
   // to next field
   if (me->nDigit > pFld->nLen-1 || (fldval * 10) > pFld->nMax) {

      // special case for year field
      if (pFld->pnVal == &me->julian.wYear) {
         // use current year if value is too small
         if (fldval < pFld->nMin) {
            JulianType jt;
            GETJULIANDATE(0, &jt);
            *pFld->pnVal = jt.wYear;
         }
      }

      DateWidget_CheckField(me, pFld, 0);
   
      // move to next field to the right
      me->nField = MIN(me->nField+1, me->nNumFields-1);
      if (!me->fTimeMode) {
         DateWidget_ValidateDate(me);
      }

      // update the value model if on-field-change is enabled
      if (me->dwFlags & DTWF_UPDATE_ONFIELDCHANGE) {
         DateWidget_SetValue(me);
      }
      
      // send field change notify through viewmodel
      DateWidget_NotifyFieldChange(me, me->nField, 1);

      me->nDigit = 0;
   }

   WidgetBase_InvalidateContent((IWidget*)me);
}


static void DateWidget_IncrementCurrentField(DateWidget *me, boolean bInc)
{
   // find field
   DtFld *pFld = me->aFlds + me->nField;

   uint16 fldval = *(pFld->pnVal);

   // special case for AM/PM field
   if (pFld->wFlags & FFLG_AMPM) {
      *(pFld->pnVal) = !fldval;
      me->fDirty = 1;
      // update the value model if on-field-change is enabled
      if (me->dwFlags & DTWF_UPDATE_ONFIELDCHANGE) {
         DateWidget_ToggleAMPMValue(me);
         DateWidget_SetValue(me);
      }
      WidgetBase_InvalidateContent((IWidget*)me);
   
   } else {

      *(pFld->pnVal) = fldval + (bInc ? 1 : -1);
      me->fDirty = 1;
   
      DateWidget_CheckField(me, pFld, 1);
      if (!me->fTimeMode) {
         DateWidget_ValidateDate(me);
      }
   
      // update the value model if on-field-change is enabled
      if (me->dwFlags & DTWF_UPDATE_ONFIELDCHANGE) {
         DateWidget_SetValue(me);
      }
   
      WidgetBase_InvalidateContent((IWidget*)me);
   }
}


static __inline CountSame(const AECHAR *pw) {
   const AECHAR *pe;
   for (pe=pw; *pw == *pe; pe++);
   return (pe-pw);
}


static __inline int MemAppendWChar(AECHAR **pp, int *pcb, int *pcbUsed, AECHAR wch) {
   return MemAppend((void**)pp, pcb, pcbUsed, &wch, sizeof(AECHAR));
}


// build a compressed format string from a potentially complex one
static int DateWidget_ParseDateFormat(DateWidget *me, const AECHAR *pwFormat)
{
   AECHAR *pwOut = 0;
   const AECHAR *p = 0;
   int cb = 0;
   int cbUsed = 0;
   int nErr = 0;
   boolean bMon = 0;
   boolean bDay = 0;
   boolean bYear = 0;

   // alloc at least 3 chars 
   nErr = MemGrow((void**)&pwOut, &cb, cbUsed, 4 * sizeof(AECHAR));

   for (p=pwFormat; !nErr && *p; p++) {

      if (ISMONCHAR(*p)) {
         if (!bMon) {
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)'M');
            bMon = 1;
         }

      } else if (ISDAYCHAR(*p)) {
         if (!bDay) {
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)'d');
            bDay = 1;
         }

      } else if (ISYEARCHAR(*p)) {
         if (!bYear) {
            AECHAR wch = (AECHAR)'y';
            if (4 == CountSame(p) || *p == (AECHAR)'Y') {
               wch = (AECHAR)'Y';
            }
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, wch);
            bYear = 1;
         }
      }
   }

   // must have at least one format char to be valid
   if (!bMon && !bDay && !bYear) {
      nErr = EFAILED;
   }

   // append terminator
   if (!nErr) {
      nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)0);
   }

   if (!nErr) {
      FREEIF(me->pwFormat);
      me->pwFormat = pwOut;
   } else {
      FREEIF(pwOut);
   }

   return nErr;
}


// build a compressed format string from a potentially complex one
static int DateWidget_ParseTimeFormat(DateWidget *me, const AECHAR *pwFormat)
{
   AECHAR *pwOut = 0;
   const AECHAR *p = 0;
   int cb = 0;
   int cbUsed = 0;
   int nErr = 0;
   boolean bHour = 0;
   boolean bMin = 0;
   boolean bSec = 0;
   boolean bAMPM = 0;

   // alloc at least 4 chars 
   nErr = MemGrow((void**)&pwOut, &cb, cbUsed, 4 * sizeof(AECHAR));

   for (p=pwFormat; !nErr && *p; p++) {

      if (ISHOURCHAR(*p)) {
         if (!bHour) {
            me->f24HourTime = 0;
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, *p);
            bHour = 1;
         }

      } else if (ISMINCHAR(*p)) {
         if (!bMin) {
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)'m');
            bMin = 1;
         }

      } else if (ISSECCHAR(*p)) {
         if (!bSec) {
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)'s');
            bSec = 1;
         }
      
      } else if (ISAMPMCHAR(*p)) {
         if (!bAMPM) {
            AECHAR wch = (AECHAR)'t';
            if (1 < CountSame(p) || *p == (AECHAR)'T') {
               wch = (AECHAR)'T';
            }
            nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, wch);
            bAMPM = 1;
         }
      }
   }

   // must have at least one format char to be valid
   if (!nErr && !bHour && !bMin && !bSec) {
      nErr = EFAILED;
   }

   // append terminator
   if (!nErr) {
      nErr = MemAppendWChar(&pwOut, &cb, &cbUsed, (AECHAR)0);
   }

   if (!nErr) {
      FREEIF(me->pwFormat);
      me->pwFormat = pwOut;
   } else {
      FREEIF(pwOut);
   }

   return nErr;
}


static void DateWidget_InitTimeFields(DateWidget *me)
{
   const AECHAR *pw = me->pwFormat;
   
   me->nNumFields = 0;
   me->fShortAMPM = 0;
   
   for (; *pw; pw++) {

      if (ISHOURCHAR(*pw)) {

         uint16 nMaxHour = 12;
         uint16 nMinHour = 1;
         if (*pw == (AECHAR)'H') {
            nMinHour = 0;
            nMaxHour = 23;
            me->f24HourTime = 1;
         }
         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wHour, nMinHour, nMaxHour, 2, 0);
      
      } else if (ISMINCHAR(*pw)) {

         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wMinute, 0, 59, 2, 0);

      } else if (ISSECCHAR(*pw)) {

         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wSecond, 0, 59, 2, 0);
      
      } else if (ISAMPMCHAR(*pw)) {

         if (*pw == (AECHAR)'t') {
            me->fShortAMPM = 1;
         }
         if (me->nNumFields) {
            me->pwSpecDelim = gwszSpecDelim;
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwSpecDelim;
         }
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->nPM, 0, 1, 2, FFLG_AMPM);
         me->fShowAMPM = 1;
      }
   }

   // update and use model's time value
   DateWidget_UpdateAndUseInternalDate(me);
}


static void DateWidget_InitDateFields(DateWidget *me)
{
   const AECHAR *pw = me->pwFormat;
   
   me->nNumFields = 0;

   for (; *pw; pw++) {

      if (ISMONCHAR(*pw)) {
         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wMonth, 1, 12, 2, 0);
      
      } else if (ISDAYCHAR(*pw)) {
         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }
         me->ndxDay = me->nNumFields;
         DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wDay, 1, 31, 2, 0);
      
      } else if (ISYEARCHAR(*pw)) {

         if (me->nNumFields) {
            me->aFlds[me->nNumFields-1].ppwDelim = (const AECHAR **)&me->pwDelim;
         }

         if (*pw == (AECHAR)'y') {
            me->f2DigitYear = 1;
            DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wYear, 
                       0, 99, 2, 0);
         } else {
            me->f2DigitYear = 0;
            if (me->fJulianMode) {
               // Julian Date Widget supports wider year range, 0 -> 9999
               DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wYear, 
                          JULIAN_YEAR_FLD_MIN, JULIAN_YEAR_FLD_MAX, 4, 0);
            } else {
               // Date Widget supports CDMA time, year 1980 -> 2115
               DtFld_Ctor(&me->aFlds[me->nNumFields++], &me->julian.wYear, 
                          YEAR_FLD_MIN, YEAR_FLD_MAX, 4, 0);
            }
         }
      }
   }

   // update and use model's date value
   DateWidget_UpdateAndUseInternalDate(me);
}


static int DateWidget_BuildPaddedDelimiter(DateWidget *me, const AECHAR *pw)
{
   FREEIF(me->pwDelim);

   // if delim string starts with whitespace, don't do extra padding
   if (W_ISWHITESPACE(*pw)) {
      me->pwDelim = WSTRDUP(pw);

   } else {

      const AECHAR wszFmt[] = {' ','%','s',' ', 0};
      int cbAlloc = WSTRSIZE(pw) + sizeof(AECHAR) * 2;
   
      // set the default delimiter
      me->pwDelim = MALLOC(cbAlloc);
      if (me->pwDelim) {
         WSPRINTF(me->pwDelim, cbAlloc, wszFmt, pw);
      }
   }

   return me->pwDelim ? SUCCESS : ENOMEMORY;
}


static void DateWidget_MeasureContent(DateWidget *me, WExtent *pwe) 
{
   int cx = 0;
   int nFld = 0;

   for (; nFld < me->nNumFields; nFld++) {
      const AECHAR *pwDelim = me->aFlds[nFld].ppwDelim ? *(me->aFlds[nFld].ppwDelim) : 0;
      AEERect rc;
      // draw field as string
      DateWidget_CalcFieldRect(me, nFld, &rc);
      pwe->height = rc.dy;
      cx += rc.dx;
      if (pwDelim) {
         cx += IFont_MeasureString(me->piFont, pwDelim);
      }
   }

   pwe->width = cx;
}

static void DateWidget_SetDelimiterFromLocale(DateWidget *me, ILocale *piLocale)
{
   const AECHAR *pw = 0;

   // set up fields
   if (me->fTimeMode) {

      if (piLocale) {
         ILOCALE_GetTimeSeparator(piLocale, &pw);
      }
      if (!pw) {
         pw = gwszTimeDelim;
      }
      // set the default delimiter
      DateWidget_BuildPaddedDelimiter(me, pw);

   } else {

      if (piLocale) {
         ILOCALE_GetDateSeparator(piLocale, &pw);
      }
      if (!pw) {
         pw = gwszDateDelim;
      }
      // set the default delimiter
      DateWidget_BuildPaddedDelimiter(me, pw);
   }
}


static void DateWidget_UseLocale(DateWidget *me, ILocale *piLocale)
{
   DateWidget_SetDelimiterFromLocale(me, piLocale);

   if (piLocale) {

      // set up fields
      if (me->fTimeMode) {

         const AECHAR *pw = 0;

         // get AM string
         FREEIF(me->pwAM);
         ILOCALE_GetAMDesignator(piLocale, &pw);
         if (!pw) {
            pw = gwszAM;
         }
         me->pwAM = WSTRDUP(pw);
         pw = 0;

         // get PM string
         FREEIF(me->pwPM);
         ILOCALE_GetPMDesignator(piLocale, &pw);
         if (!pw) {
            pw = gwszPM;
         }
         me->pwPM = WSTRDUP(pw);
         pw = 0;

         // get short time pattern from ILocale
         ILOCALE_GetShortTimePattern(piLocale, &pw);
         if (!pw) {
            pw = gwszDefTimePattern;
         }
         DateWidget_ParseTimeFormat(me, pw);

         // initialize the time fields
         DateWidget_InitTimeFields(me);

      } else {

         const AECHAR *pw = 0;

         // get short time pattern from ILocale
         ILOCALE_GetShortDatePattern(piLocale, &pw);
         if (!pw) {
            pw = gwszDefDatePattern;
         }
         DateWidget_ParseDateFormat(me, pw);

         // initialize the date fields
         DateWidget_InitDateFields(me);
      }

   }
}


static int DateWidget_LoadResBitmap(DateWidget *me, uint16 idRes, IBitmap **ppiBmp)
{
   *ppiBmp = WidgetLib_LoadResBitmap(me->piShell, RES_FILE, idRes);
   return (!*ppiBmp) ? ERESOURCENOTFOUND : SUCCESS;
}

static void DateWidget_PrepareArrowStrip(DateWidget *me)
{
   AEEBitmapInfo abi;

   if (me->piBmpArrows) {

      IBITMAP_SetTransparencyColor(me->piBmpArrows, 
                                   IBITMAP_RGBToNative(me->piBmpArrows, MAKE_RGBA(255,0,255,0)));
      IBITMAP_GetInfo(me->piBmpArrows, &abi, sizeof(abi));
      me->cxTile  = (uint16)abi.cx / 2;
      me->cyTile  = (uint16)abi.cy;
   }
}

static boolean DateWidget_HaveArrows(DateWidget *me)
{
   if (me->piBmpArrows) {
      return TRUE;
   }

   if (SUCCESS != DateWidget_LoadResBitmap(me, IDB_VSBARROWS, &me->piBmpArrows)) {
      return FALSE;
   }

   DateWidget_PrepareArrowStrip(me);

   return TRUE;
}


static void DateWidget_DrawArrow(DateWidget *me, IBitmap *piBmp, 
                                 const AEERect *prcClip, int nTile, int x, int y)
{
   AEERect rc, rcClip, rcSrc;

   // create the destination rectangle
   SETAEERECT(&rc, x, y, me->cxTile, me->cyTile);

   // intersect with the destination rectangle
   IntersectRect(&rcClip, &rc, prcClip);

   // normalize to bitmap dimension by subtracting x,y
   rcSrc = rcClip;
   rcSrc.x -= x;
   rcSrc.y -= y;

   rcSrc.x += (me->cxTile * nTile);

   // do a bltIn into the IDisplay's bitmap
   IBITMAP_BltIn(piBmp, rcClip.x, rcClip.y, rcSrc.dx, rcSrc.dy, 
                 me->piBmpArrows, rcSrc.x, rcSrc.y, 
                 AEE_RO_TRANSPARENT);
}


//int Bitmap_ReplaceColor(IBitmap *me, RGBVAL rgbOld, RGBVAL rgbNew)
//{
//   int nErr = 0;
//   IDIB *piDib = 0;
//
//   // is our bitmap a DIB?
//   nErr = IBITMAP_QueryInterface(me, AEEIID_DIB, (void**)&piDib);
//   if (!nErr && piDib && piDib->pRGB && piDib->cntRGB) {
//      NativeColor nc = IBITMAP_RGBToNative(me, rgbOld);
//      if (nc < piDib->cntRGB) {
//         piDib->pRGB[nc] = NTOHL(rgbNew);
//      }
//      IDIB_FlushPalette(piDib);
//   }
//
//   RELEASEIF(piDib);
//   return nErr;
//}

static void DateWidget_FieldChanged(DateWidget *me, boolean bMoveFwd)
{
   me->nDigit = 0;

   if (!me->fTimeMode) {
      DateWidget_ValidateDate(me);
   }
   // update the value model if on-field-change is enabled
   if (me->dwFlags & DTWF_UPDATE_ONFIELDCHANGE) {
      DateWidget_SetValue(me);
   }
   // send field change notify through viewmodel
   DateWidget_NotifyFieldChange(me, me->nField, bMoveFwd);
   WidgetBase_InvalidateContent((IWidget*)me);
}

static void DateWidget_FieldMove(DateWidget *me, boolean bMoveFwd)
{
   DateWidget_CheckField(me, me->aFlds + me->nField, 0);
   me->nField += (bMoveFwd ? 1 : -1);

   if (me->nField < 0) {
      me->nField = me->nNumFields-1;
   } else if (me->nField > me->nNumFields-1) {
      me->nField = 0;
   }

   DateWidget_FieldChanged(me, bMoveFwd);
}

static boolean DateWidget_SetField(DateWidget *me, int dwField)
{
   // is this an invalid field?
   if ((dwField < 0) || (dwField > me->nNumFields-1)) {
      return FALSE;
   }

   DateWidget_CheckField(me, me->aFlds + me->nField, 0);
   me->nField = dwField;

   DateWidget_FieldChanged(me, TRUE);

   return (TRUE);
}

static boolean DateWidget_HandleEvent(IWidget *po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ME_FROM_WIDGET;

   if (Border_HandleEvent(&me->border, evt, wParam, dwParam)) {
      return TRUE;
   }

   switch (evt) {
      
      case EVT_WDG_SETFOCUS:
         // update the value model when losing focus and if field has been changed
         if (!wParam) {
            DateWidget_CheckField(me, me->aFlds + me->nField, 0);
            me->nDigit = 0;
            if (!me->fTimeMode) {
               DateWidget_ValidateDate(me);
            }
            DateWidget_SetValue(me);
            WidgetBase_InvalidateContent(po);
         }
         return TRUE;

      case EVT_KEY:
         switch (wParam) {
            case AVK_LEFT:
            case AVK_RIGHT:
               DateWidget_FieldMove(me, (boolean)(wParam == AVK_RIGHT));
               return TRUE;

            case AVK_UP:
            case AVK_DOWN:
               if (me->dwFlags & DTWF_USEUPDOWNKEYS) {
                  me->nDigit = 0;
                  DateWidget_IncrementCurrentField(me, (boolean)(wParam == AVK_UP));
                  return TRUE;
               }
               break;

            default:
               if (wParam >= AVK_0 && wParam <= AVK_9 ||
                   wParam == AVK_SELECT) {
                  DateWidget_EnterField(me, wParam);
                  return TRUE;
               }
               break;
         }
         break;   // EVT_KEY


      case EVT_WDG_SETPROPERTY:

         switch (wParam) {
            
            case PROP_FONT_CLASS: {
               IFont *piFont = 0;
               if (SUCCESS == ISHELL_CreateInstance(me->piShell, (AEECLSID)dwParam, (void**)&piFont)) {
                  DateWidget_SetFont(me, piFont);
               }
               RELEASEIF(piFont);
               return TRUE;
            } 

            case PROP_FONT:
               DateWidget_SetFont(me, (IFont*)dwParam);
               return TRUE;

            case PROP_FONT_OUTLINECOLOR: {
               IHFont *piHFont;
               if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
                  IHFONT_SetFontOutlineColor(piHFont, (RGBVAL)dwParam);
                  RELEASEIF(piHFont);
                  DateWidget_UseFont(me, me->piFont);
                  return TRUE;
               } else {
                  break;
               }
            }

            case PROP_FONT_OUTLINEWIDTH: {
               IHFont *piHFont;
               if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
                  IHFONT_SetFontOutlineWidth(piHFont, dwParam);
                  RELEASEIF(piHFont);
                  DateWidget_UseFont(me, me->piFont);
                  return TRUE;
               } else {
                  break;
               }  
            }

            case PROP_FLAGS:
               me->dwFlags = dwParam;
               me->fShowArrows = ( (me->dwFlags & DTWF_USEUPDOWNKEYS) && 
                                   (me->dwFlags & DTWF_SHOWUPDOWNARROWS) );
               return TRUE;

            case PROP_LOCALE: 
               DateWidget_UseLocale(me, (ILocale*)dwParam);
               WidgetBase_InvalidateContent(po);
               return TRUE;
            
            case PROP_FORMATSTRING:
               if (me->fTimeMode) {
                  if (SUCCESS == DateWidget_ParseTimeFormat(me, (AECHAR*)dwParam)) {
                     // initialize the date fields
                     DateWidget_InitTimeFields(me);
                     return TRUE;
                  }
               } else {
                  if (SUCCESS == DateWidget_ParseDateFormat(me, (AECHAR*)dwParam)) {
                     // initialize the date fields
                     DateWidget_InitDateFields(me);
                     return TRUE;
                  }
               }
               break;

            case PROP_DELIMITER:
               FREEIF(me->pwDelim);
               // if delimiter string supplied, set it
               if (dwParam) {
                  me->pwDelim = WSTRDUP((AECHAR*)dwParam);
               
               // otherwise, revert to default delimiter from locale
               } else {
                  ILocale *piLocale = 0;
                  ISHELL_CreateInstance(me->piShell, AEECLSID_CURRENTLOCALE, (void**)&piLocale);
                  // call this method even if piLocale is NULL
                  DateWidget_SetDelimiterFromLocale(me, piLocale);
                  RELEASEIF(piLocale);
               }
               return TRUE;


            case PROP_FIELDPAD:
               me->nFldPad = MIN((uint8)dwParam, 255);
               WidgetBase_InvalidateContent(po);
               return TRUE;

            case PROP_ARROWPAD:
               me->nArrowPad = MIN((uint8)dwParam, 255);
               WidgetBase_InvalidateContent(po);
               return TRUE;

            case PROP_FGCOLOR:
               DynRGB_Collapse(&me->rgbText, (RGBVAL)dwParam);
               DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, RGB_WHITE);
               WidgetBase_InvalidateContent(po);
               return TRUE;

            case PROP_SACTIVE_FGCOLOR:
               DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, (RGBVAL)dwParam);
               return TRUE;

            case PROP_IMAGESTRIP:
               RELEASEIF(me->piBmpArrows);
               me->piBmpArrows = (IBitmap*)dwParam;
               if (me->piBmpArrows) {
                  IBITMAP_AddRef(me->piBmpArrows);
                  DateWidget_PrepareArrowStrip(me);
               }
               WidgetBase_InvalidateContent(po);
               return TRUE;

            case PROP_FOCUSINDEX:
               if (DateWidget_SetField(me, (int)dwParam)) {
                  WidgetBase_InvalidateContent(po);
                  return TRUE;
               } else {
                  break;
               }
               
         }
         break;   // EVT_WDG_SETPROPERTY


      case EVT_WDG_GETPROPERTY:

         switch (wParam) {
         case PROP_FONT:
               *(IFont **)dwParam = me->piFont;
               ADDREFIF(me->piFont);
               return TRUE;
         
            case PROP_FONT_OUTLINECOLOR: {
               IHFont *piHFont;
               if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
                  IHFONT_GetFontOutlineColor(piHFont, (RGBVAL*)dwParam);
                  RELEASEIF(piHFont);
                  return TRUE;
               } else {
                  break;
               }
            }

            case PROP_FONT_OUTLINEWIDTH: {
               IHFont *piHFont;
               if (SUCCESS == IFONT_TO_IHFONT(me->piFont, &piHFont)) {
                  IHFONT_GetFontOutlineWidth(piHFont, (int*)dwParam);
                  RELEASEIF(piHFont);
                  return TRUE;
               } else {
                  break;
               }
            }

            case PROP_FLAGS:
               *(uint32*)dwParam = me->dwFlags;
               return TRUE;

            case PROP_FIELDPAD:
               *(uint8*)dwParam = me->nFldPad;
               return TRUE;

            case PROP_ARROWPAD:
               *(uint8*)dwParam = me->nArrowPad;
               return TRUE;

            case PROP_FGCOLOR:
               DynRGB_Get(&me->rgbText, RGBINDEX_ACTIVE, (uint32*)dwParam);
               return TRUE;

            case PROP_SACTIVE_FGCOLOR:
               DynRGB_Get(&me->rgbText, RGBINDEX_SACTIVE, (uint32*)dwParam);
               return TRUE;

            case PROP_FOCUSINDEX:
               *(int*)dwParam = me->nField;
               return TRUE;

            case PROP_JULIANVALUEMODEL:
               *(boolean*)dwParam = (me->fJulianMode != 0) ? TRUE : FALSE;
               return TRUE;
         }
         break;
   }

   return WidgetBase_DefHandleEvent(po, evt, wParam, dwParam);
}


/////////////////////////////////////////////////////////////////////////////
// drawing

static int DateWidget_DrawField(DateWidget *me, IBitmap *piBmp, const AEERect *prcClip, 
                                int nField, int x, int y, boolean bSelected)
{
   AECHAR wszBuf[12];
   int cx = 0;
   int cchRemaining = 12;
   DtFld *pFld = me->aFlds + nField;
   AEERect rc;

   int nPrePad = (me->nDigit == 0 || me->nField != nField) ? pFld->nLen : 0;

   // special case, display AM or PM string in field
   if (pFld->wFlags & FFLG_AMPM) {
      boolean bPM = (boolean) *(pFld->pnVal);
      WSTRLCPY(wszBuf, bPM ? me->pwPM : me->pwAM, ARRAYSIZE(wszBuf));
      // if short format, just display 1st character
      if (me->fShortAMPM) {
         wszBuf[1] = 0; // truncate after 1st char
      }
   } else {
      // normal field, convert value to string
      WWRITELONGEX(wszBuf, (long)*(pFld->pnVal), nPrePad, &cchRemaining);
   }

   DateWidget_CalcFieldRect(me, nField, &rc);
   cx = rc.dx;
   rc.x += x;
   rc.y += y;

   // draw selection rectangle
   if (bSelected && me->border.bActive) {
      RGBVAL rgb;
      AEERect rcFill;
      NativeColor ncSel;

      Border_GetBGColor(&me->border, RGBINDEX_SACTIVE, &rgb);
      ncSel = IBITMAP_RGBToNative(piBmp, rgb);

      IntersectRect(&rcFill, &rc, prcClip);
      IBITMAP_FillRect(piBmp, &rcFill, ncSel, AEE_RO_COPY);

      // ensure resource image is loaded, then draw arrows
      if (me->fShowArrows && DateWidget_HaveArrows(me)) {

//         Bitmap_ReplaceColor(me->piBmpArrows, RGB_BLACK, rgb);

         // draw the arrows
         DateWidget_DrawArrow(me, piBmp, prcClip, 0, 
                              rcFill.x + (rcFill.dx-me->cxTile)/2,
                              rcFill.y - me->cyTile - me->nArrowPad);
         DateWidget_DrawArrow(me, piBmp, prcClip, 1, 
                              rcFill.x + (rcFill.dx-me->cxTile)/2,
                              rcFill.y + rcFill.dy + me->nArrowPad);

//         Bitmap_ReplaceColor(me->piBmpArrows, rgb, RGB_BLACK);
      }
   }

   rc.y += me->nFldPad;

   // draw field text
   {
      const AECHAR *pwDelim = pFld->ppwDelim ? *pFld->ppwDelim : 0;

      int cxDigits;
      RGBVAL rgb;
      NativeColor ncFore;
      int ndxFG = (me->border.bActive) ? RGBINDEX_ACTIVE : RGBINDEX_INACTIVE;
      int ndxFGDelim = ndxFG;
      if (bSelected) {
         ndxFG += RGBINDEX_SINACTIVE;
      }

      DynRGB_Get(&me->rgbText, ndxFG, &rgb);
      rgb &= MASK_NOALPHA;
      ncFore = IBITMAP_RGBToNative(piBmp, rgb);

      cxDigits = IFont_MeasureString(me->piFont, wszBuf);

      IFONT_DrawText(me->piFont, piBmp, rc.x + cx - me->nFldPad - cxDigits, rc.y, wszBuf, WSTRLEN(wszBuf), 
                     ncFore, 0, prcClip, IFONT_DRAW_TRANSPARENT);

      // draw delimiter following the field
      if (pwDelim) {
         DynRGB_Get(&me->rgbText, ndxFGDelim, &rgb);
         rgb &= MASK_NOALPHA;
         ncFore = IBITMAP_RGBToNative(piBmp, rgb);
         IFONT_DrawText(me->piFont, piBmp, rc.x + cx, rc.y, pwDelim, WSTRLEN(pwDelim), 
                        ncFore, 0, prcClip, IFONT_DRAW_TRANSPARENT);
         cx += IFont_MeasureString(me->piFont, pwDelim);
      }
   }
   
   return cx;
}


/////////////////////////////////////////////////////////////////////////////
// IWidget methods

void DateWidget_Draw(IWidget *po, ICanvas *piCanvas, int x, int y)
{
   ME_FROM_WIDGET;
   IDisplay *piDisplay = 0;
   IBitmap *piBmp = 0;

   Border_Draw(&me->border, piCanvas, x, y);
   Border_AdjustDisplayClipRect(&me->border, piCanvas, x, y);

   if (SUCCESS == ICANVAS_GetDisplay(piCanvas, &piDisplay)) {

      AEERect rcClip;

      int xd = x + me->border.rcClient.x;
      int yd = y + me->border.rcClient.y;
      
      if (me->fShowArrows) {
         yd += me->cyTile + me->nArrowPad;
      }

      IDISPLAY_GetClipRect(piDisplay, &rcClip);

      piBmp = IDISPLAY_GetDestination(piDisplay);

      if (piBmp) {
         int nFld;
         for (nFld=0; nFld < me->nNumFields; nFld++) {
            // draw field as string
            xd += DateWidget_DrawField(me, piBmp, &rcClip, nFld, xd, yd, (boolean)(nFld == me->nField));
         }
      }
   }

   RELEASEIF(piBmp);
   RELEASEIF(piDisplay);
}


int DateWidget_SetModel(IWidget *po, IModel *piModel)
{
   ME_FROM_WIDGET;
   IValueModel *pivm = 0;
   int nErr = IMODEL_QueryInterface(piModel, AEEIID_VALUEMODEL, (void **)&pivm);

   if (!nErr) {
      WidgetBase_SetModel(po, piModel);
      IMODEL_AddListenerEx(piModel, &me->ml, (PFNLISTENER)DateWidget_ModelListener, me);
      WidgetBase_InvalidateContent(po);
   }

   RELEASEIF(pivm);
   return nErr;
}


void DateWidget_GetPreferredExtent(IWidget *po, WExtent *pweOut)
{
   ME_FROM_WIDGET;

   WExtent weClient;
   SETWEXTENT(&weClient, 40, me->cyFont + (me->nFldPad*2));   // some defaults

   if (me->piFont) {
      DateWidget_MeasureContent(me, &weClient);
      if (me->fShowArrows && DateWidget_HaveArrows(me)) {
         weClient.height += ((me->cyTile+me->nArrowPad) * 2);
      }
   }

   Border_CalcPreferredExtent(&me->border, pweOut, &weClient);
}


void DateWidget_SetExtent(IWidget *po, WExtent *pe)
{
   ME_FROM_WIDGET;
   WidgetBase_SetExtent((IWidget*)&me->base, pe);
   Border_CalcClientRect(&me->border);
}


boolean DateWidget_IntersectOpaque(IWidget *po, AEERect *prcOut, const AEERect *prcIn)
{
   ME_FROM_WIDGET;
   return Border_IntersectOpaque(&me->border, prcOut, prcIn);
}


/////////////////////////////////////////////////////////////////////////////
// Widget creation and cleanup

static void DateWidget_Dtor(DateWidget *me)
{
   LISTENER_Cancel(&me->ml);
   RELEASEIF(me->piShell);
   LISTENER_Cancel(&me->mlFont);
   RELEASEIF(me->piFont);
   RELEASEIF(me->piBmpArrows);
   FREEIF(me->pwDelim);
   FREEIF(me->pwFormat);
   FREEIF(me->pwAM);
   FREEIF(me->pwPM);
   Border_Dtor(&me->border);
   DynRGB_Dtor(&me->rgbText);
   WidgetBase_Dtor(&me->base);
}


static void DateWidget_Delete(DateWidget *me)
{
   DateWidget_Dtor(me);
   FREE(me);
}


static uint32 DateWidget_Release(IWidget *po)
{
   ME_FROM_WIDGET;

   if (me->base.nRefs == 1) {
      DateWidget_Dtor(me);
   }      

   return WidgetBase_Release(po);
}


static int DateWidget_Construct(DateWidget *me, AEEVTBL(IWidget) *pvt, IShell *piShell,
                                IModule *piModule, boolean bTimeMode, boolean bJulianMode,
                                PFNHANDLER pfnDefHandler)
{
   IModel *piModel = 0;
   ILocale *piLocale = 0;
   IFont *piFont = 0;
   int nErr = 0;

   me->fTimeMode = bTimeMode;
   me->fJulianMode = bJulianMode;

   WidgetBase_Ctor(&me->base, pvt, piModule, DEFHANDLER(DateWidget_HandleEvent));
   
#define VTFUNC(name) pvt->name = DateWidget_##name
   // vtable initialization
   VTFUNC( Release );
   VTFUNC( GetPreferredExtent );
   VTFUNC( SetExtent );
   VTFUNC( Draw );
   VTFUNC( IntersectOpaque );
   VTFUNC( SetModel );

#undef VTFUNC

   DynRGB_Ctor(&me->rgbText, RGB_BLACK);
   DynRGB_Set(&me->rgbText, RGBINDEX_SACTIVE, RGB_WHITE);

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->nFldPad = 1;
   me->nArrowPad = 1;

   if (!nErr) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CURRENTLOCALE, (void**)&piLocale);
   }

   if (!nErr) {
      DateWidget_UseLocale(me, piLocale);

      Border_Ctor(&me->border, (PFNINVALIDATE)WidgetBase_Invalidate, me, &me->base.extent, TRUE, &WBASE(me)->piViewModel);
      Border_SetBorderColor(&me->border, 0, RGBA_BLACK);
      Border_SetBGColor(&me->border, 0, RGBA_NONE);
      
      nErr = ISHELL_CreateFont(piShell, AEECLSID_FONTSYSNORMAL, &piFont);
   }

   if (!nErr) {
      DateWidget_SetFont(me, piFont);
      nErr = ValueModel_New((IValueModel**)&piModel, piModule);
   }

   if (!nErr) {
      DateWidget_SetModel((IWidget*)me, piModel);

      GETJULIANDATE(0, &me->julian);
      DateWidget_UseValue(me);
      me->fDirty = 1;
      DateWidget_SetValue(me);
   }

   RELEASEIF(piLocale);
   RELEASEIF(piModel);
   RELEASEIF(piFont);

   return nErr;
}


static int DateTimeWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule, boolean bTimeMode, boolean bJulianMode)
{
   int nErr = 0;
   DateWidget *me = MALLOCREC_VTBL(DateWidget, IWidget);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   nErr = DateWidget_Construct(me, GETVTBL(me, IWidget), piShell, piModule, bTimeMode, bJulianMode, 0);

   if (!nErr) {
      *ppo = (IWidget*)me;
   } else {
      DateWidget_Delete(me);
   }

   return nErr;
}


int DateWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   return DateTimeWidget_New(ppo, piShell, piModule, 0, 0);
}                                                     


int TimeWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   return DateTimeWidget_New(ppo, piShell, piModule, 1, 0);
}

int JulianDateWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   return DateTimeWidget_New(ppo, piShell, piModule, 0, 1);
}                                                     


int JulianTimeWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule)
{
   return DateTimeWidget_New(ppo, piShell, piModule, 1, 1);
}



