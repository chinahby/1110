/*=============================================================================
FILE: AEEMenuCtlEx.h

SERVICES: BREW MenuCtlEx Services
 
GENERAL DESCRIPTION: This file defines extended settings for IMenuCtl interface

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

===========================================================================*/
#ifndef AEEMENUCTLEX_H
#define AEEMENUCTLEX_H

#include "AEEMenu.h"


#define MP_EX                             0xFFFF0000  // extended menu properties
#define MP_EX_SINGLE_CLICK_LAUNCH         0xFFFF0001
#define MP_EX_ITEM_HEIGHT                 0xFFFF0002


#define ODA_USER_BASE                     10000
#define ODA_INFO_SB                       (ODA_USER_BASE + 1)
#define ODA_INFO_ITEM                     (ODA_USER_BASE + 2)

typedef struct
{
  uint16      nSize;
  boolean     bDisable; 
  uint16      wWidth;
  RGBVAL      cFrame;
  RGBVAL      cScrollbar;
  RGBVAL      cThumb;
  IImage     *pImageThumb;
  IImage     *pImageScrollbar;
  const char *pszResImage;
  uint16      wImageThumb;
  uint16      wImageScrollbar;
} AEEScrollBarInfo;

typedef struct
{
  uint16      nSize;
  RGBVAL      cText;
  RGBVAL      cTextSel;
  IImage     *pImage;
  IImage     *pImageSelect;
  const char *pszResImage;
  uint16      wImage;
  uint16      wImageSelect;
} AEEMenuExItemInfo;

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================

MP_EX_SINGLE_CLICK_LAUNCH: enable/disable single click launch

============================================================================*/
#endif //AEEMENUCTLEX_H
