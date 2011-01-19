#ifdef  FEATURE_CAMERA_MOBICAT
#ifndef  CAMERA_TUNE_UI_FOR_OEM_H
#define  CAMERA_TUNE_UI_FOR_OEM_H
/////////////////////////////////////////////////////////////////////////////
//
//  C A M E R A   T U N E   U I   F O R   O E M
//
//  FILE:         camera_tune_ui_for_oem.h
//
//  COPYRIGHT:    Copyright (c) 2005 by QUALCOMM, Incorporated.
//                All Rights Reserved.
//
//  DESCRIPTION:  When FEATURE_CAMERA_MOBICAT is enabled in custcamera.h,
//                an OEM's LCD UI application may 
//                #includes this file to augment the UI's behavior.
//
//                The 2 key, from Preview mode, may toggle the on-screen top-banner
//                between the OEM's UI and runtime tuning use.
//
//                If runtime tuning is visible, the top banner displays
//                current settings for a current subsystem.
//                The left-right direction keys rotate through registered
//                subsystems, and up-down direction keys alter values.
//
//  HISTORY:      (in reverse chronological order)
//
// $Header: //source/qcom/qct/multimedia/camera/6k/qmobicat/main/latest/inc/camera_tune_ui_for_OEM.h#1 $
// $DateTime: 2008/05/09 16:40:03 $
// $Author: karenp $ 
//
//  when      who    what, where, why
//  --------  -----  ---------------------------------------------------------
//  2006-02-10 jj    Skeletal file for OEMs to fill out for their own LCD UIs.
//
/////////////////////////////////////////////////////////////////////////////
//
//  For an OEM UI to access MobiCaT (Mobile Camera (runtime) Tuning),
//  the native UI's key-press symbols need to be mapped to the generic
//  keypress symbols of the camera_tune (i.e., MobiCaT) code.
//
//  The OEM's UI application may then #include camera_tune_ui_for_OEM.c
//  just as apps/qcamera/qcamera.c #includes camera_tune_ui_for_brew.c
//  and the OEM should change the #include of camera_tune_ui_for_brew.h
//  in camera_tune.c to #include this (corrected, by the OEM) file.
//
/////////////////////////////////////////////////////////////////////////////


#define CAMERA_TUNE_KEY_0       (uint16)( )
#define CAMERA_TUNE_KEY_1       (uint16)( )
#define CAMERA_TUNE_KEY_2       (uint16)( )
#define CAMERA_TUNE_KEY_3       (uint16)( )
#define CAMERA_TUNE_KEY_4       (uint16)( )
#define CAMERA_TUNE_KEY_5       (uint16)( )
#define CAMERA_TUNE_KEY_6       (uint16)( )
#define CAMERA_TUNE_KEY_7       (uint16)( )
#define CAMERA_TUNE_KEY_8       (uint16)( )
#define CAMERA_TUNE_KEY_9       (uint16)( )
#define CAMERA_TUNE_KEY_CLR     (uint16)( )
#define CAMERA_TUNE_KEY_DOWN    (uint16)( )
#define CAMERA_TUNE_KEY_LEFT    (uint16)( )
#define CAMERA_TUNE_KEY_POUND   (uint16)( )
#define CAMERA_TUNE_KEY_RIGHT   (uint16)( )
#define CAMERA_TUNE_KEY_SELECT  (uint16)( )
#define CAMERA_TUNE_KEY_STAR    (uint16)( )
#define CAMERA_TUNE_KEY_UP      (uint16)( )
#define CAMERA_TUNE_KEY_I       (uint16)( )
#define CAMERA_TUNE_KEY_PTT     (uint16)( )


/////////////////////////////////////////////////////
#endif  //  CAMERA_TUNE_UI_FOR_OEM_H
#endif  //  FEATURE_CAMERA_MOBICAT
