#ifndef  CAMERA_TUNE_UI_FOR_BREW_H
#define  CAMERA_TUNE_UI_FOR_BREW_H
/////////////////////////////////////////////////////////////////////////////
//
//  C A M E R A   T U N E   U I   F O R   B R E W
//
//  FILE:         camera_tune_ui_for_brew.h
//
//  COPYRIGHT:    Copyright (c) 2005 by QUALCOMM, Incorporated.
//                All Rights Reserved.
//
//  DESCRIPTION:  When FEATURE_CAMERA_MOBICAT is enabled in custcamera.h,
//                apps/qcamera/qcamera.c
//                #includes this file to augment the BREW UI's behavior.
//
//                The 2 key, from Preview mode, toggles the on-screen top-banner
//                between Qcamera (a BREW app) and runtime tuning use.
//
//                If runtime tuning is visible, the top banner displays
//                current settings for a current subsystem.
//                The left-right direction keys rotate through registered
//                subsystems, and up-down direction keys alter values.
//
//  HISTORY:      (in reverse chronological order)
//
// $Header: //source/qcom/qct/multimedia/camera/6k/qmobicat/main/latest/inc/camera_tune_ui_for_brew.h#1 $
// $DateTime: 2008/05/09 16:40:03 $
// $Author: karenp $ 
//
//  when      who    what, where, why
//  --------  -----  ---------------------------------------------------------
//  2006-01-30 jj    Abstracts BREW symbols from camera_tune.c 
//
/////////////////////////////////////////////////////////////////////////////
//
//  For an OEM UI to access MobiCaT (Mobile Camera (runtime) Tuning),
//  the native UI's key-press symbols need to be mapped to the generic
//  keypress symbols of the camera_tune (i.e., MobiCaT) code.
//
/////////////////////////////////////////////////////////////////////////////


#include <AEEVCodes.h>      /**  I.e., apps/brew_3.1/sdk/inc/AEEVCodes.h  **/


#define CAMERA_TUNE_KEY_0       (uint16)( AVK_0 )
#define CAMERA_TUNE_KEY_1       (uint16)( AVK_1 )
#define CAMERA_TUNE_KEY_2       (uint16)( AVK_2 )
#define CAMERA_TUNE_KEY_3       (uint16)( AVK_3 )
#define CAMERA_TUNE_KEY_4       (uint16)( AVK_4 )
#define CAMERA_TUNE_KEY_5       (uint16)( AVK_5 )
#define CAMERA_TUNE_KEY_6       (uint16)( AVK_6 )
#define CAMERA_TUNE_KEY_7       (uint16)( AVK_7 )
#define CAMERA_TUNE_KEY_8       (uint16)( AVK_8 )
#define CAMERA_TUNE_KEY_9       (uint16)( AVK_9 )
#define CAMERA_TUNE_KEY_CLR     (uint16)( AVK_CLR )
#define CAMERA_TUNE_KEY_DOWN    (uint16)( AVK_DOWN )
#define CAMERA_TUNE_KEY_LEFT    (uint16)( AVK_LEFT )
#define CAMERA_TUNE_KEY_POUND   (uint16)( AVK_POUND )
#define CAMERA_TUNE_KEY_RIGHT   (uint16)( AVK_RIGHT )
#define CAMERA_TUNE_KEY_SELECT  (uint16)( AVK_SELECT )
#define CAMERA_TUNE_KEY_STAR    (uint16)( AVK_STAR )
#define CAMERA_TUNE_KEY_UP      (uint16)( AVK_UP )
#define CAMERA_TUNE_KEY_I       (uint16)( AVK_INFO )
#define CAMERA_TUNE_KEY_PTT     (uint16)( AVK_PTT )


/////////////////////////////////////////////////////
#endif  //  CAMERA_TUNE_UI_FOR_BREW_H
