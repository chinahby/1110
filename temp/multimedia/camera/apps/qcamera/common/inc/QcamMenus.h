#ifndef QCAMMENUS_H
#define QCAMMENUS_H
/*===========================================================================

FILE: QcamMenus.h

GENERAL DESCRIPTION
    This is the common header file contains definitions for QcamMenus
    source file.

    Copyright (c) 2004 - 2006 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/inc/QcamMenus.h#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/22/06  ZL     Removed softkey menu bar icons at Qcam settings screen.
 10/31/06  NC     Updated QcamMenus_GetItemDataPerStep type
 01/05/06  NC     Added header and revision history
=============================================================================== */
/*===============================================================================
INCLUDES AND DEFINITIONS
=============================================================================== */
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif
#include "qcamera.h"
#include "qcamera.brh"
#include "QcamDef.h"
#ifdef FEATURE_APP_QCAMERA
//List menu type
typedef enum
{
  MENU_Icon,
  MENU_Radio,
  MENU_Check
}ListMenuType;

//Setting menu mapping:
// build menu function definition
typedef void (*PFNMENUBUILD) (QCam *pMe);

// execute menu function definition
typedef void (*PFNMENUEXEC) (QCam *pMe);

// event handler sanity check definition
typedef struct _CSettingMenuHandler
{
  uint16       MenuID;
  PFNMENUBUILD MenuBuildFunc; //On select menu icon to build menu function
} CSettingMenuHandler;

// event handler sanity check definition
typedef struct _CExecMenuHandler
{
  QCamMenuType dwActiveMenu;
  PFNMENUEXEC MenuExecFunc; //On select menu item to execute menu function
} CExecMenuHandler;

//public functions

boolean QcamMenus_DoSettingsMenu(QCam *pMe, int item);
void QcamMenus_ExecuteActiveMenu(QCam *pMe);
void QcamMenus_BuildGPSMenu(QCam *pMe);
void QcamMenus_BuildSettingsMenu(QCam *pMe);
void QcamMenus_BuildSoundMenu(QCam *pMe);
void QcamMenus_BuildTakePhotoMenu(QCam *pMe);
void QcamMenus_UpdateDeleteAllMenu(QCam *pMe, int item);
boolean QcamMenus_ValueInDataList(uint32 val, uint16 valId);
#ifdef FEATURE_CAMERA7500
uint16  QcamMenus_GetItemDataPerStep(uint16 itemId, int step, uint32 *pSelData);
#endif //FEATURE_CAMERA7500
#endif //FEATURE_APP_QCAMERA
#endif //QCAMMENUS_H
