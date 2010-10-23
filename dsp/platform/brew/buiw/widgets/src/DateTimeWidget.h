/*
  ========================================================================

  FILE:  DateTimeWidget.h
  
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
#ifndef __DATETIMEWIDGET_H__
#define __DATETIMEWIDGET_H__

#include "AEEDateTimeWidget.h"
#include "DefaultLocale.h"
#include "WidgetBase.h"
#include "Border.h"

#define FFLG_AMPM          1     // field is special am/pm display


typedef struct DtFld {
   uint16 *          pnVal;         // pointer to field dest value
   uint16            nMin;          // minimum value
   uint16            nMax;          // maximum value
   uint8             nLen;          // number of digits
   uint8             wFlags;        // flags (Delimiter, special AM/PM field)
   const AECHAR **   ppwDelim;      // ptr-to-ptr to delimiter string (or NULL if no delimiter)
} DtFld;


typedef struct DateWidget {
   WidgetBase     base;
   ModelListener  ml;
   Border         border;
   JulianType     julian;
   IBitmap *      piBmpArrows;   // up/down indicator bitmaps
   IShell *       piShell;
   IFont *        piFont;        // we use this font to draw/measure
   ModelListener  mlFont;        // model listener for styled IHFonts
   uint16         cxTile;        // dx size of arrow tile
   uint16         cyTile;        // dy size of arrow tile
   uint16         cyFont;        // font height
   uint16         cxFont;        // font width of typical char (usually '0')
   
   flg            fShowArrows : 1;  // draw the up/down arrows in each field
   flg            fDirty      : 1;  // julian struct needs to be converted/written to value model
   // date-mode flags
   flg            f2DigitYear : 1;  // show only 2 digits of the year [80-99]->19xx, [00-79]->20xx
   // time-mode flags
   flg            fTimeMode   : 1;  // we are a TimeWidget if TRUE, else DateWidget
   flg            f24HourTime : 1;  // displaying 24 hour time
   flg            fShowAMPM   : 1;  // displaying AM/PM field
   flg            fShortAMPM  : 1;  // displaying short AM/PM field as 'A' or 'P'
   flg            fJulianMode : 1;  // our model stores a julian type
   uint16         nPM;           // dest variable for DtFld -- used as boolean (TRUE means PM)
   const AECHAR * pwSpecDelim;   // special delim string used between hh:mm and AM/PM (usually space)
   
   uint8          ndxDay;        // index of day field in date mode (for changing max month days)
   int8           nField;        // current edit field, left to right: 0->1->2
   int8           nNumFields;    // number of fields (# of DtFld's used)
   uint8          nFldPad;       // padding used around fields
   uint8          nArrowPad;     // padding between field and up/down arrow
   DynRGB         rgbText;       // text color, indexed by RGBINDEX_xxx (see Border.h)
   DtFld          aFlds[4];      // array of fields
   AECHAR *       pwDelim;       // delimiter string, ex: ":" or  " : " or "/" or " / "
   AECHAR *       pwFormat;      // compact format string
   AECHAR *       pwAM;          // AM designator
   AECHAR *       pwPM;          // PM designator
   uint8          nDigit;        // count of digits entered in current field
   uint32         dwFlags;       // holder of PROP_FLAGS
} DateWidget;



int DateWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);
int TimeWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);

int JulianDateWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);
int JulianTimeWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);


#endif  // __DATETIMEWIDGET_H__

