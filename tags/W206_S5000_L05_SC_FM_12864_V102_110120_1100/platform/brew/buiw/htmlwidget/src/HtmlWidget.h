/*
  ========================================================================

  FILE:  HtmlWidget.h
  
  SERVICES:  

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __HTMLWIDGET_H__
#define __HTMLWIDGET_H__

#include "AEEHtmlWidget.h"
#include "WidgetBase.h"
#include "Border.h"
#include "AEETextModel.h"
#include "dox.h"
#include "doxparse.h"


typedef struct HtmlWidget {

   WidgetBase     base;
   ModelListener  ml;
   Border         border;
   IShell *       piShell;
   IFont *        piFont;        // we use this font to draw/measure

   Dox            dox;
   DoxParse       doxparse;

   AEECallback    cbkPeek;
   AEECallback    cbkUpdate;

   flg            isbusy:1;

} HtmlWidget;


int HtmlWidget_New(IWidget **ppo, IShell *piShell, IModule *piModule);



#endif //__HTMLWIDGET_H__
