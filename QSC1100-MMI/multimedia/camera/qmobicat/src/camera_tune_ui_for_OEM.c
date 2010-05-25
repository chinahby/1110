////////////////////////////////////////////////////////
#ifdef   FEATURE_CAMERA_MOBICAT
////////////////////////////////////////////////////////
/*****************************************************************************

  C A M E R A   T U N E   U I   F O R   O E M

  FILE:         camera_tune_ui_for_oem.c

  COPYRIGHT:    Copyright (c) 2005 by QUALCOMM, Incorporated.
                All Rights Reserved.

  DESCRIPTION:  When FEATURE_CAMERA_MOBICAT is enabled in custcamera.h,
                an OEM's LCD UI application may
                #includes this file to augment the OEM's UI's behavior.

                The 2 key, from Preview mode, may toggle the on-screen top-banner
                between the OEM's UI and runtime tuning use.

                If runtime tuning is visible, the top banner displays
                current settings for a current subsystem.
                The left-right direction keys rotate through registered
                subsystems, and up-down direction keys alter values.

  HISTORY:      (in reverse chronological order)

 $Header: //source/qcom/qct/multimedia/camera/6k/qmobicat/main/latest/src/camera_tune_ui_for_OEM.c#1 $
 $DateTime: 2008/05/09 16:40:03 $
 $Author: karenp $ 

  when      who    what, where, why
  --------  -----  ---------------------------------------------------------
  2006-02-10 jj    Skeletal file for OEMs to fill out for their own LCD UIs.

*****************************************************************************/

#include "camera_tune.h"

ANY DECLARATIONS REQUIRED OR DEFINITIONS OF VARIABLES GO HERE


//////////////////////////////////////////////////////
//  Toggle which on-screen menu is visible, that of //
//  the app, or that of MobiCaT (camera_tune).      //
//                                                  //
//  This is needed for the BREW demo app "Qcamera"; //
//  OEMs may not need to do this for their apps.    //
//////////////////////////////////////////////////////


void  camera_tune_lcd_ui_toggle_visibility( void )
{
  FILL THIS OUT WITH OEM-UI-AWARE CODE
}


///////////////////////////////////////////////////////
//  This is called by camera_tune.c so needs to      //
//  handle details for the OEM's desired UI system.  //
///////////////////////////////////////////////////////


void  camera_tune_lcd_ui_print( char * string )
{
  FILL THIS OUT WITH OEM-UI-AWARE CODE
}


////////////////////////////////////////////////////////
#endif //FEATURE_CAMERA_MOBICAT
////////////////////////////////////////////////////////
