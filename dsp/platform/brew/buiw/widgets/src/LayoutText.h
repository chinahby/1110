/*
  ========================================================================

  FILE:  LayoutText.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Layout table object for widgets that wrap text.
  methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __LAYOUTTEXT_H__
#define __LAYOUTTEXT_H__

//#include "AEEFont.h"
// #include "AEEBitmap.h"  // define RGBVAL
typedef struct  {
   IFont* piFont;
   RGBVAL rgbForeground;
   RGBVAL rgbBackground;
} LayoutTextStyleInfo;

typedef struct _LayoutText LayoutText;

#define LAYOUTTEXT_MIN_TEXT_RANGE ((int)0)
#define LAYOUTTEXT_MAX_TEXT_RANGE ((int)-1)


int   LayoutText_New          (LayoutText** pme, IShell* piShell);
int   LayoutText_Delete       (LayoutText* me);

int   LayoutText_SetLineBreak (LayoutText* me, boolean bMultiLine, boolean bBreakOnWord);
int   LayoutText_SetEllipsis  (LayoutText* me, boolean bDrawEllipsis);
int   LayoutText_SetLineGap   (LayoutText* me, int     nLineGap);
int   LayoutText_SetBreak     (LayoutText* me, boolean bBreakOnSpace);
int   LayoutText_SetTextAlignment (LayoutText* me, uint32 dwAlignment);


int   LayoutText_SetWidth          (LayoutText* me, int width);
int   LayoutText_GetHeight         (LayoutText* me, int* height);
int   LayoutText_GetLineCount      (LayoutText* me, int* count);
int   LayoutText_GetScrollPosition (LayoutText* me, int offset, int* x);

int   LayoutText_DeleteText   (LayoutText* me, int start, int end);
int   LayoutText_AddText      (LayoutText* me, LayoutTextStyleInfo* pStyle, const AECHAR *pwText, int length, int insertPosition );

int   LayoutText_Draw         (LayoutText* me, IDisplay* piDisplay, int x, int y, AEERect* prcClip, uint32 nFlags, AEERect* prcEllipsis, RGBVAL rbg );

int   LayoutText_GetCharacterLocation (LayoutText* me, int nCharacter, AEERect* prcChar);

int   LayoutText_GetCaretInfo    (LayoutText* me, int index, int* xCaret, int* yCaret);
int   LayoutText_GetCaretScroll  (LayoutText* me, int nCaret, int xScrollPos, int* pxScrollPos);

int   LayoutText_GetLineFromIndex  (LayoutText* me, int index, int* pLine);
int   LayoutText_GetLineStartIndex (LayoutText* me, int line,  int* pIndex);



#endif   //__LINETABLE_H__

