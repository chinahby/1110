/*=============================================================================

FILE: pinyin.h
 
SERVICES: Definitions used for Pinyin input
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //UI/main/BREWOEM/inc/pinyin.h#1 $
$DateTime: 2002/08/30 23:48:06 $
  $Author: kjmckay $
  $Change: 57445 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/23/02   mjv     Added edit history 
 
=============================================================================*/

#ifndef __PINYIN_H
#define __PINYIN_H

#include "aee.h"

#define FONT_WIDTH 20
#define LINE_GB 40
#define SIZE_OF_COMBO 256
#define SIZE_OF_PINYIN 398
#define SIZE_OF_CANDIDATES 60

typedef struct _PY2GBTable{
    char py[7];
    AECHAR begin;
}PY2GBTable;

typedef struct _PYCheckTable
{
    char  firstchar;
    uint16  beginindex;
    uint16  endindex;
}PYCheckTable;
#endif
