/*===========================================================================

FILE: qalbumDlg_i.h

GENERAL DESCRIPTION
    This is a private header file only used by qalbumDlg.c.

      Copyright (c) 2004 - 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumDlg_i.h#1 $
 $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 09/18/07  SK     changed prototype of resize feature dialog functions
 05/22/07  SK     JPEG Decoder Resizing feature
 11/08/06  ZL     Replaced FEATURE_APP_QCAMERA with FEATURE_APP_QALBUM.
 05/10/05  JN     Add DlgDescribeEventHandler.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#if defined(FEATURE_APP_QALBUM)
#include "AEEStdLib.h"

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static void Qalbum_GetDirLen(const Qalbum *pMe, int *rootDirLen, int *dataDirLen);
static void Qalbum_GenerateEditedFileName(IFileMgr *pIFileMgr, char *szCurName);
static void Qalbum_SavePhoto(Qalbum *pMe);
static void Qalbum_SaveJpegImage(Qalbum *pMe, AEEMediaData *pmd);
static void Qalbum_SavePngImage(Qalbum *pMe, AEEMediaData *pmd);
static void Qalbum_GetEncodeCB(void *pUser);
#ifdef  FEATURE_JPEGD_RESIZE
static uint16 Qalbum_DlgSlideShowGetText(ITextCtl *ctrl, uint16 size);
static void Qalbum_DlgSlideShowSetText(ITextCtl *ctrl, uint16 nVal, uint16 size);
#endif //FEATURE_JPEGD_RESIZE
#endif //FEATURE_APP_QALBUM
