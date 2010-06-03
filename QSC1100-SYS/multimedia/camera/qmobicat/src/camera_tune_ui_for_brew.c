#include "custcamera.h"

/*****************************************************************************

  C A M E R A   T U N E   U I   F O R   B R E W

  FILE:         camera_tune_ui_for_brew.c

  COPYRIGHT:    Copyright (c) 2005 by QUALCOMM, Incorporated.
                All Rights Reserved.

  DESCRIPTION:  When FEATURE_CAMERA_MOBICAT is enabled in custcamera.h,
                apps/qcamera/qcamera.c
                #includes this file to augment the BREW UI's behavior.

                The 2 key, from Preview mode, toggles the on-screen top-banner
                between Qcamera (a BREW app) and runtime tuning use.

                If runtime tuning is visible, the top banner displays
                current settings for a current subsystem.
                The left-right direction keys rotate through registered
                subsystems, and up-down direction keys alter values.

  HISTORY:      (in reverse chronological order)

 $Header: //source/qcom/qct/multimedia/camera/6k/qmobicat/main/latest/src/camera_tune_ui_for_brew.c#3 $
 This was //depot/asic/msm6550/services/camera/mobicat/camera_tune_brew.c#2.
 This was //depot/asic/msm6550/services/camera/mobicat/camera_tune_brew.h#9.
 This WAS //depot/asic/msm6550/services/camera/camera_tune_mobile.h#28
 $DateTime: 2008/10/22 01:39:52 $
 $Author: shilpam $ 

  when      who    what, where, why
  --------  -----  ---------------------------------------------------------
  2008-10-22 rk   Fixed the Mobicat Crash.
  2008-04-01 rk   Fixed the compiler warning if FEATURE_CAMERA_MOBICAT are not defined.
  2007-11-28 rk   Removed the feature FEATURE_APP_QCAMERA for fix the compiler error.
  2007-10-26 smv   To fix issues with debugging qcamera symbols.
  2007-05-07 jpg   Added camera_tune_return_control_to_app for returning
                   control to camera app. Old way of calling 
                   Qcamera_ToggleMenuDisplay() doesn't work on targets 
                   without MDP support.
  2007-03-19 jpg   Added gray visibility background (white renders as transparent)
  2006-11-22 ry    Wrapped Qcamera_ToggleMenuDisplay() under FEATURE_OVERLAY to resolve a Link error
  2006-01-26 jj    Code review improvements, simplifications.
  2006-01-23 jj    Massive overhaul to simplify for OEMs.
  2005-11-03 jj    Renamed camera_tune_utils.* to camera_tune.* ...
  2005-09-06 jj    Renamed to camera_tune_brew.h since it's a BREW plugin.
  2005-08-02 jj    Clarified header comments.
  2005-08-02 jj    Pulling share-able structures to camera_tune_utils.h .
  2005-07-29 jj    Moved all content from camera_tune_mobile.c to here.
  2005-07-22 jj    Rewriting comments to QCT standards.
  2005-06-30 jj    Had been under msmshared.
  2005-05-06 jj    Added the standard headers, honoring coding conventions.

*****************************************************************************/


#include "camera_tune.h"
#include "qcamera.h"
#include "wstring.h"

#ifdef  FEATURE_CAMERA_MOBICAT
//////////////////////////////////////////////////////
//  Having global variables precludes threadsafety. //
//////////////////////////////////////////////////////

QCam    * camera_tune_the_host_brew_app      = NULL  ;
boolean   camera_tune_was_enabled_at_startup = FALSE ;
boolean   camera_tune_return_control_to_app  = FALSE ;

//////////////////////////////////////////////////////
//  Toggle which on-screen menu is visible, that of //
//  the app, or that of MobiCaT (camera_tune).      //
//                                                  //
//  This is needed for the BREW demo app "Qcamera"; //
//  OEMs may not need to do this for their apps.    //
//////////////////////////////////////////////////////


void  camera_tune_lcd_ui_toggle_visibility( void )
{
  camera_tune_return_control_to_app = TRUE;
}


///////////////////////////////////////////////////////
//  This is called by camera_tune.c so needs to      //
//  handle details for the OEM's desired UI system.  //
///////////////////////////////////////////////////////


#define CAMERA_TUNE_BREW_CLIP_REGION_X_ORIGIN    (0)
#define CAMERA_TUNE_BREW_CLIP_REGION_Y_ORIGIN    (0)
#define CAMERA_TUNE_BREW_CLIP_REGION_WIDTH     (240)
#define CAMERA_TUNE_BREW_CLIP_REGION_HEIGHT     (20)
#define CAMERA_TUNE_BREW_STRING_WIDTH          (128)


void  camera_tune_lcd_ui_print( char * string )
{
  AEERect  rectangle;
  AECHAR   camera_tune_wide_chars[ CAMERA_TUNE_BREW_STRING_WIDTH ] ;
  RGBVAL   oldColor;
  static   int fontheight = 0;

  if (fontheight == 0)
  {
    // The call below returns either EFAILED or the font height.
    fontheight = IDISPLAY_GetFontMetrics(camera_tune_the_host_brew_app->a.m_pIDisplay,
                                         AEE_FONT_BOLD,
                                         NULL,
                                         NULL);
    if (fontheight == EFAILED)
    {
      fontheight = CAMERA_TUNE_BREW_CLIP_REGION_HEIGHT;
    }
    else
    {
      // BREW sometimes underreports the height needed.
      // Let's add some so we don't cut anything off.
      fontheight += 5;
    }
  }

  SETAEERECT ( &rectangle, 
               CAMERA_TUNE_BREW_CLIP_REGION_X_ORIGIN,
               CAMERA_TUNE_BREW_CLIP_REGION_Y_ORIGIN,
               CAMERA_TUNE_BREW_CLIP_REGION_WIDTH   ,
               fontheight
             );

  strtowstr ( string ,                       //  Transcribe from ascii..
              camera_tune_wide_chars ,       //  .. to wide chars for BREW.
              CAMERA_TUNE_BREW_STRING_WIDTH  //  sizeof(camera_tune_wide_chars) ?
            );

  oldColor = IDISPLAY_SetColor(camera_tune_the_host_brew_app->a.m_pIDisplay,
                               CLR_USER_BACKGROUND, 
                               MAKE_RGB(192, 192, 192));

  IDISPLAY_DrawText( camera_tune_the_host_brew_app->a.m_pIDisplay,
                     AEE_FONT_BOLD , 
                     camera_tune_wide_chars , -1 ,
                     0, 1,
                     &rectangle ,  //  Draw within there.
                     IDF_ALIGN_LEFT | IDF_RECT_FILL
                   );
    
  IDISPLAY_UpdateEx(camera_tune_the_host_brew_app->a.m_pIDisplay, FALSE);

  (void) IDISPLAY_SetColor(camera_tune_the_host_brew_app->a.m_pIDisplay,
                           CLR_USER_BACKGROUND, 
                           oldColor);
}


////////////////////////////////////////////////////////
#endif  //  FEATURE_CAMERA_MOBICAT
