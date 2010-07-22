#ifndef DISP_OPTIONS_H
#define DISP_OPTIONS_H


//============================================================================
//
//                 Display Device Driver Option Header File
//
// DESCRIPTION
//   This file defines optional information to supplement disp.h file.
//
// Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
//============================================================================

//============================================================================
//
//                      EDIT HISTORY FOR FILE
//
// This section contains comments describing changes made to this file.
// Notice that changes are listed in reverse chronological order.
//
// $PVCSPath:  L:/src/asw/MSM6050/vcs/disp_options.h_v   1.1   01 Apr 2002 15:48:50   dlee  $
// $Header: //source/qcom/qct/multimedia/display/mdp1/lcd_qsc1100/main/latest/inc/disp_options.h#1 $ $DateTime: 2008/05/16 16:10:52 $ $Author: aansari $
//
// when      who    what, where, why
// --------  -----  ----------------------------------------------------------
// 12/04/03  dle    Added support for TOUCAN QVGA.
// 04/01/02  dle    Added define checking.
// 03/19/02  dle    Initial release
//
//============================================================================

/* This implementation supports 16 bits per pixel device.
** You may change to support 8 or 18 bits per pixel. The display
** device driver spoofs the 8 or 18 BPP. It actually translate 8 or 18 BPP
** to 16 BPP when updating the device.
*/
/* Define only one of the followings: */
#ifdef FEATURE_LCD_18BPP_EMULATION
#error code not present
#else
  #define DISP_DEVICE_16BPP
#endif /*FEATURE_LCD_18BPP_EMULATION*/

#define DISP2_DEVICE_16BPP


#if !defined DISP_DEVICE_8BPP && !defined DISP_DEVICE_16BPP && !defined DISP_DEVICE_18BPP
#error !defined DISP_DEVICE_8BPP && !defined DISP_DEVICE_16BPP && !defined DISP_DEVICE_18BPP
#endif
#if defined DISP_DEVICE_8BPP && defined( DISP_DEVICE_16BPP)
#error defined( DISP_DEVICE_8BPP ) && defined( DISP_DEVICE_16BPP)
#endif
#if defined DISP_DEVICE_8BPP && defined( DISP_DEVICE_18BPP)
#error defined( DISP_DEVICE_8BPP ) && defined( DISP_DEVICE_18BPP)
#endif
#if defined DISP_DEVICE_18BPP && defined( DISP_DEVICE_16BPP)
#error defined( DISP_DEVICE_18BPP ) && defined( DISP_DEVICE_16BPP)
#endif

/* Display width and height in number of pixels */

#ifdef CUST_EDITION
#if defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204) || defined(FEATURE_PROJECT_FLEXI) || defined(FEATURE_PROJECT_M8) || defined(FEATURE_PROJECT_SMART)
	#define DISP_WIDTH       160
   	#define DISP_HEIGHT       128
#elif defined(FEATURE_PROJECT_W021) || defined(FEATURE_PROJECT_W022)
	#define DISP_WIDTH       128
	#define DISP_HEIGHT       128
#else
	#define DISP_WIDTH        240
	#define DISP_HEIGHT       320
#endif
#else
#ifdef FEATURE_TMD_20_QVGA
   #define DISP_WIDTH        240
   #define DISP_HEIGHT       320
   #define TMD20QVGA_LCD_18BPP   
#else
   #define DISP_WIDTH        176
   #define DISP_HEIGHT       220
#endif /* FEATURE_TMD_QVGA */

#endif
#endif    /* DISP_OPTIONS_H */

