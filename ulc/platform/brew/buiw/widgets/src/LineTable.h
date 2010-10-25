/*
  ========================================================================

  FILE:  LineTable.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Line table object for widgets that wrap text.
  methods

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __LINETABLE_H__
#define __LINETABLE_H__


typedef uint16 TLPos;


typedef struct {
   TLPos *     paPos;         // array of line positions (indexes)
   int         cbAlloc;       // number of bytes allocated (in paPos)
   int         cbUsed;        // number of bytes used (in paPos)
   uint16      nLines;        // number of lines in line table
} TextLineTbl;


static __inline int TextLineTbl_GetNumLines(TextLineTbl *me) {
   return me->nLines;
}


int   TextLineTbl_New         (TextLineTbl **ppo);
void  TextLineTbl_Ctor        (TextLineTbl *me);
void  TextLineTbl_Dtor        (TextLineTbl *me);

void  TextLineTbl_Reset       (TextLineTbl *me);

int   TextLineTbl_WrapFill    (TextLineTbl *me, IFont *piFont, const AECHAR *pwText, 
                               int cchText, int nUpdateFromLine, int cxWrapWidth, 
                               boolean bNoBreakOnSpace);

int   TextLineTbl_GetLine     (TextLineTbl *me, uint16 nLine, TLPos *pposOffset, int *pnLen);

int   TextLineTbl_GetLineFromPos (TextLineTbl *me, TLPos pos);


#endif   //__LINETABLE_H__

