#ifndef AEEMEDIASVG_H
#define AEEMEDIASVG_H

/*============================================================================
FILE:	AEEMediaSVG.h

SERVICES:  BREW SVG Services

DESCRIPTION:
   This file defines IMediaSVG interface that provides specialized services
   for Scalable Vector Graphics (SVG) media.
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEMedia.h"
#include "AEEBitmap.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//
// Status codes
//
// Status code is returned via registered callback function to 
// indicate event status and to return data to client.
// AEEMediaCmdNotify::nStatus in the callback function contains the following
// status codes.
//
#define MM_STATUS_SVG_BASE             0x1000                  // Base used by IMediaSVG
#define MM_STATUS_SVG_DOM_BASE         0x4000                  // Base used by SVG DOM
#define MM_STATUS_SVG_USER_BASE        0x8000                  // Base for extension

#define MM_STATUS_SVG_PREPARE_DONE         (MM_STATUS_SVG_BASE)       // [Play] SVG has been decoded and can begin rendering frames
#define MM_STATUS_SVG_URL                  (MM_STATUS_SVG_BASE + 1)   // [Play] The SVG content specifies a URL should be opened due to user action; pCmdData = URL

//
// Command codes
//
// Command code is returned via registered callback function to 
// indicate event type and to return data to client.
// AEEMediaCmdNotify::nCmd in the callback function contains the following
// status codes.
//
#define MM_CMD_SVG_BASE             0x1000   // Base used by IMediaSVG
#define MM_CMD_SVG_USER_BASE        0x8000   // Base for derived class

//
// SVG Control Parms
//
// Set/Get IMediaSVG parms using IMEDIA_SetMediaParm()/IMEDIA_GetMediaParm().
// Setting/Getting SVG parms may result in async callback events. If
// this happens, then function will return MM_PENDING status.
//
// IMPORTANT NOTE: 
//    For documentation of these parameters, please refer to 
//    "SVG Control Parameters (MM_PARM_SVG_XXX)" section under 
//    documentation (scroll down a few pages).
//
#define MM_PARM_SVG_BASE            0x1000                   // Base used by IMediaSVG
#define MM_PARM_SVG_DOM_BASE        0x4000                   // Base used by SVG DOM
#define MM_PARM_SVG_USER_BASE       0x8000                   // Base for extension

#define MM_PARM_SVG_DPI                       (MM_PARM_SVG_BASE)      // [Set] Dots per inch
#define MM_PARM_SVG_DIR                       (MM_PARM_SVG_BASE + 1)  // [Set] Directory to use for external file requests.
                                                                      // [Get] Pass buffer size as second parm
#define MM_PARM_SVG_GENERATE_FRAME            (MM_PARM_SVG_BASE + 2)  // [Set] Requests a frame to be generated in client-controlled mode
#define MM_PARM_SVG_FRAMEDELAY                (MM_PARM_SVG_BASE + 3)  // [Set] Frame delay in automatic frame generation mode
#define MM_PARM_SVG_BACKGROUND_COLOR          (MM_PARM_SVG_BASE + 4)  // [Set] Background color for rendered frames
#define MM_PARM_SVG_INPUT                     (MM_PARM_SVG_BASE + 5)  // [Set] Delivers user input to the SVG decoder
#define MM_PARM_SVG_CLIENT_FRAME_GENERATION   (MM_PARM_SVG_BASE + 6)  // [Set] Enables/Disables client controlled frame generation mode

//
// Color Schemes
//
#define MM_SVG_COLOR_SCHEME_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_COLOR_SCHEME_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_COLOR_SCHEME_NONE         (MM_SVG_COLOR_SCHEME_BASE + 1)       // Unknown Color Scheme
#define MM_SVG_COLOR_SCHEME_RGB565       (MM_SVG_COLOR_SCHEME_BASE + 2)   // RGB 565 format, 16 bits/pixel
#define MM_SVG_COLOR_SCHEME_0RGB888      (MM_SVG_COLOR_SCHEME_BASE + 4)   // RGB 888 format, 32 bits/pixel

//
// User events (interactivity)
//
#define MM_SVG_EVENT_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_EVENT_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_EVENT_KEY              (MM_SVG_EVENT_BASE)       // Key Pressed
#define MM_SVG_EVENT_FOCUS            (MM_SVG_EVENT_BASE + 1)   // Text Input gets Focus
#define MM_SVG_EVENT_POINTER_CLICK    (MM_SVG_EVENT_BASE + 2)   // Mouse Click
#define MM_SVG_EVENT_POINTER_DOWN     (MM_SVG_EVENT_BASE + 3)   // Mouse Down
#define MM_SVG_EVENT_POINTER_MOVE     (MM_SVG_EVENT_BASE + 4)   // Mouse Move
#define MM_SVG_EVENT_POINTER_OVER     (MM_SVG_EVENT_BASE + 5)   // Mouse Over
#define MM_SVG_EVENT_POINTER_UP       (MM_SVG_EVENT_BASE + 6)   // Mouse Up
#define MM_SVG_EVENT_POINTER_LEAVE    (MM_SVG_EVENT_BASE + 7)   // Mouse Leave
#define MM_SVG_EVENT_ROTATE           (MM_SVG_EVENT_BASE + 8)   // Rotate Event
#define MM_SVG_EVENT_PAN              (MM_SVG_EVENT_BASE + 9)   // Pan Event
#define MM_SVG_EVENT_ZOOM             (MM_SVG_EVENT_BASE + 10)  // Zoom Event
#define MM_SVG_EVENT_RESET_FRAME      (MM_SVG_EVENT_BASE + 11)  // Resets the current frame to original view, undoing prior zoom and pan events

//
// Virtual keys
//
#define MM_SVG_VK_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_VK_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_VK_UNDEFINED           (MM_SVG_VK_BASE)
#define MM_SVG_VK_RIGHT_ALT           (MM_SVG_VK_BASE + 1)
#define MM_SVG_VK_LEFT_ALT            (MM_SVG_VK_BASE + 2)
#define MM_SVG_VK_LEFT_CONTROL        (MM_SVG_VK_BASE + 3)
#define MM_SVG_VK_RIGHT_CONTROL       (MM_SVG_VK_BASE + 4)
#define MM_SVG_VK_LEFT_SHIFT          (MM_SVG_VK_BASE + 5)
#define MM_SVG_VK_RIGHT_SHIFT         (MM_SVG_VK_BASE + 6)
#define MM_SVG_VK_LEFT_META           (MM_SVG_VK_BASE + 7)
#define MM_SVG_VK_RIGHT_META          (MM_SVG_VK_BASE + 8)
#define MM_SVG_VK_CAPS_LOCK           (MM_SVG_VK_BASE + 9)
#define MM_SVG_VK_DELETE              (MM_SVG_VK_BASE + 10)
#define MM_SVG_VK_END                 (MM_SVG_VK_BASE + 11)
#define MM_SVG_VK_ENTER               (MM_SVG_VK_BASE + 12)
#define MM_SVG_VK_ESCAPE              (MM_SVG_VK_BASE + 13)
#define MM_SVG_VK_HOME                (MM_SVG_VK_BASE + 14)
#define MM_SVG_VK_INSERT              (MM_SVG_VK_BASE + 15)
#define MM_SVG_VK_NUM_LOCK            (MM_SVG_VK_BASE + 16)
#define MM_SVG_VK_PAUSE               (MM_SVG_VK_BASE + 17)
#define MM_SVG_VK_PRINTSCREEN         (MM_SVG_VK_BASE + 18)
#define MM_SVG_VK_SCROLL_LOCK         (MM_SVG_VK_BASE + 19)
#define MM_SVG_VK_LEFT                (MM_SVG_VK_BASE + 20)
#define MM_SVG_VK_RIGHT               (MM_SVG_VK_BASE + 21)
#define MM_SVG_VK_UP                  (MM_SVG_VK_BASE + 22)
#define MM_SVG_VK_DOWN                (MM_SVG_VK_BASE + 23)
#define MM_SVG_VK_PAGE_DOWN           (MM_SVG_VK_BASE + 24)
#define MM_SVG_VK_PAGE_UP             (MM_SVG_VK_BASE + 25)
#define MM_SVG_VK_F1                  (MM_SVG_VK_BASE + 26)
#define MM_SVG_VK_F2                  (MM_SVG_VK_BASE + 27)
#define MM_SVG_VK_F3                  (MM_SVG_VK_BASE + 28)
#define MM_SVG_VK_F4                  (MM_SVG_VK_BASE + 29)
#define MM_SVG_VK_F5                  (MM_SVG_VK_BASE + 30)
#define MM_SVG_VK_F6                  (MM_SVG_VK_BASE + 31)
#define MM_SVG_VK_F7                  (MM_SVG_VK_BASE + 32)
#define MM_SVG_VK_F8                  (MM_SVG_VK_BASE + 33)
#define MM_SVG_VK_F9                  (MM_SVG_VK_BASE + 34)
#define MM_SVG_VK_F10                 (MM_SVG_VK_BASE + 35)
#define MM_SVG_VK_F11                 (MM_SVG_VK_BASE + 36)
#define MM_SVG_VK_F12                 (MM_SVG_VK_BASE + 37)
#define MM_SVG_VK_F13                 (MM_SVG_VK_BASE + 38)
#define MM_SVG_VK_F14                 (MM_SVG_VK_BASE + 39)
#define MM_SVG_VK_F15                 (MM_SVG_VK_BASE + 40)
#define MM_SVG_VK_F16                 (MM_SVG_VK_BASE + 41)
#define MM_SVG_VK_F17                 (MM_SVG_VK_BASE + 42)
#define MM_SVG_VK_F18                 (MM_SVG_VK_BASE + 43)
#define MM_SVG_VK_F19                 (MM_SVG_VK_BASE + 44)
#define MM_SVG_VK_F20                 (MM_SVG_VK_BASE + 45)
#define MM_SVG_VK_F21                 (MM_SVG_VK_BASE + 46)
#define MM_SVG_VK_F22                 (MM_SVG_VK_BASE + 47)
#define MM_SVG_VK_F23                 (MM_SVG_VK_BASE + 48)
#define MM_SVG_VK_F24                 (MM_SVG_VK_BASE + 49)

//
// Key modifiers for key events
//
#define MM_SVG_KEY_MOD_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_KEY_MOD_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_KEY_MOD_NONE           (MM_SVG_KEY_MOD_BASE)
#define MM_SVG_KEY_MOD_RIGHT_ALT      (MM_SVG_KEY_MOD_BASE + 1)
#define MM_SVG_KEY_MOD_LEFT_ALT       (MM_SVG_KEY_MOD_BASE + 2)
#define MM_SVG_KEY_MOD_LEFT_CONTROL   (MM_SVG_KEY_MOD_BASE + 4)
#define MM_SVG_KEY_MOD_RIGHT_CONTROL  (MM_SVG_KEY_MOD_BASE + 8)
#define MM_SVG_KEY_MOD_LEFT_SHIFT     (MM_SVG_KEY_MOD_BASE + 16)
#define MM_SVG_KEY_MOD_RIGHT_SHIFT    (MM_SVG_KEY_MOD_BASE + 32)
#define MM_SVG_KEY_MOD_LEFT_META      (MM_SVG_KEY_MOD_BASE + 64)
#define MM_SVG_KEY_MOD_RIGHT_META     (MM_SVG_KEY_MOD_BASE + 128)

//
// Focus event types
//
#define MM_SVG_FOCUS_EVENT_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_FOCUS_ENENT_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_FOCUS_EVENT_NONE       (MM_SVG_FOCUS_EVENT_BASE)       // No Event
#define MM_SVG_FOCUS_EVENT_BEGIN      (MM_SVG_FOCUS_EVENT_BASE + 1)   // Sets Focus to first visible focusable element
#define MM_SVG_FOCUS_EVENT_NEXT       (MM_SVG_FOCUS_EVENT_BASE + 2)   // Sets Focus to next visible focusable element
#define MM_SVG_FOCUS_EVENT_PREVIOUS   (MM_SVG_FOCUS_EVENT_BASE + 3)   // Sets Focus to previous visible focusable element
#define MM_SVG_FOCUS_EVENT_ACTIVATE   (MM_SVG_FOCUS_EVENT_BASE + 4)   // Activates Focus to the element with focus, might generate URL and/or file requests
#define MM_SVG_FOCUS_EVENT_END        (MM_SVG_FOCUS_EVENT_BASE + 5)   // Unsets focus to the frame

//
// Scroll modes for pan events
//
#define MM_SVG_SCROLL_MODE_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_SCROLL_MODE_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_SCROLL_MODE_NORMAL           (MM_SVG_SCROLL_MODE_BASE)
#define MM_SVG_SCROLL_MODE_FAST             (MM_SVG_SCROLL_MODE_BASE + 1)

//
// Centering modes for zoom events
//
#define MM_SVG_CENTER_MODE_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_CENTER_MODE_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_CENTER_MODE_UNCENTERED           (MM_SVG_CENTER_MODE_BASE)
#define MM_SVG_CENTER_MODE_CENTERED             (MM_SVG_CENTER_MODE_BASE + 1)

//
// SVG Media spec flags
//
#define MM_SVG_SPEC_FEATURE_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_SPEC_FEATURE_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_SPEC_FEATURE_NONE            (MM_SVG_SPEC_FEATURE_BASE)       // No flags
#define MM_SVG_SPEC_FEATURE_ANIMATED        (MM_SVG_SPEC_FEATURE_BASE + 1)   // SVG media is animated
#define MM_SVG_SPEC_FEATURE_ZOOMPANEVENTS   (MM_SVG_SPEC_FEATURE_BASE + 2)   // SVG media supports zoom and pan events
#define MM_SVG_SPEC_FEATURE_TEXTONLY        (MM_SVG_SPEC_FEATURE_BASE + 4)   // SVG media supports text-only mode

//
// Phase for event targets
//
#define MM_SVG_EVTTARGET_PHASE_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_EVTTARGET_PHASE_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_EVTTARGET_PHASE_DEFAULT           (MM_SVG_EVTTARGET_PHASE_BASE)
#define MM_SVG_EVTTARGET_PHASE_CAPTURE           (MM_SVG_EVTTARGET_PHASE_BASE + 1)

//
// Properties for events
//
#define MM_SVG_EVENT_PROPERTIES_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_EVENT_PROPERTIES_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_EVENT_PROPERTIES_NONE             (MM_SVG_EVTTARGET_PHASE_BASE)
#define MM_SVG_EVENT_PROPERTIES_BUBBLE           (MM_SVG_EVTTARGET_PHASE_BASE + 1)  //Event is a bubbling event
#define MM_SVG_EVENT_PROPERTIES_CANCEL           (MM_SVG_EVTTARGET_PHASE_BASE + 2)  //Event can have its default action prevented

//
// Key event specification structure
//
typedef struct SVGKeyData
{
   uint16  nKey;         // [Mandatory] ASCII code for key pressed
   int16   nVirtualKey;  // [Mandatory] Virtual Key pressed; one of MM_SVG_VK_XXX
   int16   nModifier;    // [Mandatory] Key Modifiers used; combination of MM_SVG_KEY_MOD_XXX
} SVGKeyData;

//
// Pointer event specification structure
//
typedef struct SVGPointerData
{
   int16 x;              // [Mandatory] x-coordinate of pointer position
   int16 y;              // [Mandatory] y-coordinate of pointer position
   int16 nKeyModifier;   // [Mandatory] Key Modifiers; combination of MM_SVG_KEY_MOD_XXX
} SVGPointerData;

//
// Rotate event specification structure
//
typedef struct SVGRotateData
{
   uint32 cx;       // [Mandatory] x-coordinate of center of rotation
   uint32 cy;       // [Mandatory] y-coordinate of center of rotation
   int16  nAngle;   // [Mandatory] angle of rotation
} SVGRotateData;

//
// Zoom event specification structure
//
typedef struct SVGZoomData
{
   int16 x;               // [Mandatory] x-coordinate of zoom position
   int16 y;               // [Mandatory] y-coordinate of zoom position
   uint32  nZoomFactor;   // [Mandatory] Zoom factor in 16.16 fixed point
   flg flgCentered;       // [Mandatory] MM_SVG_CENTER_MODE_UNCENTERED, or MM_SVG_CENTER_MODE_CENTERED to recenter in view after zoom
} SVGZoomData;

//
// Pan event specification structure
//
typedef struct SVGPanData
{
   int16 x;                // [Mandatory] x-coordinate for panning
   int16 y;                // [Mandatory] y-coordinate for panning
   flg flgScrollMode;      // [Mandatory] MM_SVG_SCROLL_MODE_NORMAL for normal scrolling, MM_SVG_SCROLL_MODE_FAST for fast scrolling
} SVGPanData;

//
// SVG media spec
//
typedef struct AEEMediaSVGSpec
{
   uint16  flgFeatures;         // Flags indicating features (MM_SVG_SPEC_FEATURE_XXX) supported by the media
   int16   nColorScheme;        // Color scheme used; one of MM_SVG_COLOR_SCHEME_XXX
   char    szTitle[128];        // Title, if defined, of the SVG file. String is NULL terminated
   uint32  nFrameWidth;         // Width of frames
   uint32  nFrameHeight;        // Height of frames
   uint32  nStructSize;         // Size of this structure       
} AEEMediaSVGSpec;

typedef struct IMediaSVG IMediaSVG;


//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//
// Commonly used APIs to set/get IMedia parm
//
// Initialization:
#define IMEDIASVG_SetDPI(p, v)                          IMEDIA_SetMediaParm(p, MM_PARM_SVG_DPI,                       (int32)(v),       0)
#define IMEDIASVG_SetDir(p, sz)                         IMEDIA_SetMediaParm(p, MM_PARM_SVG_DIR,                       (int32)(sz),      0)
#define IMEDIASVG_EnableClientFrameGeneration(p, b)     IMEDIA_SetMediaParm(p, MM_PARM_SVG_CLIENT_FRAME_GENERATION,   (int32)(b),       0)
#define IMEDIASVG_SetFrameDelay(p, v)                   IMEDIA_SetMediaParm(p, MM_PARM_SVG_FRAMEDELAY,                (int32)(v),       0)
#define IMEDIASVG_SetBackgroundColor(p, v)              IMEDIA_SetMediaParm(p, MM_PARM_SVG_BACKGROUND_COLOR,          (int32)(v),       0)

// User interactivity
#define IMEDIASVG_Key(p, pd)                            IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_KEY,           (int32)(pd))
#define IMEDIASVG_Focus(p, v)                           IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_FOCUS,         (int32)(v))
#define IMEDIASVG_PointerClick(p, pd)                   IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_CLICK, (int32)(pd))
#define IMEDIASVG_PointerDown(p, pd)                    IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_DOWN,  (int32)(pd))
#define IMEDIASVG_PointerMove(p, pd)                    IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_MOVE,  (int32)(pd))
#define IMEDIASVG_PointerOver(p, pd)                    IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_OVER,  (int32)(pd))
#define IMEDIASVG_PointerUp(p, pd)                      IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_UP,    (int32)(pd))
#define IMEDIASVG_PointerLeave(p, pd)                   IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_POINTER_LEAVE, (int32)(pd))
#define IMEDIASVG_Rotate(p, pd)                         IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_ROTATE,        (int32)(pd))
#define IMEDIASVG_Pan(p, pd)                            IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_PAN,           (int32)(pd))
#define IMEDIASVG_Zoom(p, pd)                           IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_ZOOM,          (int32)(pd))
#define IMEDIASVG_ResetFrame(p)                         IMEDIA_SetMediaParm(p, MM_PARM_SVG_INPUT,        MM_SVG_EVENT_RESET_FRAME,   0)

// Frame generation
#define IMEDIASVG_GenerateFrame(p, v)                   IMEDIA_SetMediaParm(p, MM_PARM_SVG_GENERATE_FRAME,   (int32)(v), 0)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
SVGKeyData

Description: 
This structure contains information for processing Key events during SVG
playback.

Definition:
   typedef struct SVGKeyData
   {
      uint16  nKey;
      int16   nVirtualKey;
      int16   nModifier;
   } SVGKeyData;

Members:
   nKey:        [Mandatory] ASCII code for key pressed
   
   nVirtualKey: [Mandatory] Virtual Key pressed; one of the following:
   MM_SVG_VK_UNDEFINED
   MM_SVG_VK_RIGHT_ALT
   MM_SVG_VK_LEFT_ALT
   MM_SVG_VK_LEFT_CONTROL
   MM_SVG_VK_RIGHT_CONTROL
   MM_SVG_VK_LEFT_SHIFT
   MM_SVG_VK_RIGHT_SHIFT
   MM_SVG_VK_LEFT_META
   MM_SVG_VK_RIGHT_META
   MM_SVG_VK_CAPS_LOCK
   MM_SVG_VK_DELETE
   MM_SVG_VK_END
   MM_SVG_VK_ENTER
   MM_SVG_VK_ESCAPE
   MM_SVG_VK_HOME
   MM_SVG_VK_INSERT
   MM_SVG_VK_NUM_LOCK
   MM_SVG_VK_PAUSE
   MM_SVG_VK_PRINTSCREEN
   MM_SVG_VK_SCROLL_LOCK
   MM_SVG_VK_LEFT
   MM_SVG_VK_RIGHT
   MM_SVG_VK_UP
   MM_SVG_VK_DOWN
   MM_SVG_VK_PAGE_DOWN
   MM_SVG_VK_PAGE_UP
   MM_SVG_VK_F1
   MM_SVG_VK_F2
   MM_SVG_VK_F3
   MM_SVG_VK_F4
   MM_SVG_VK_F5
   MM_SVG_VK_F6
   MM_SVG_VK_F7
   MM_SVG_VK_F8
   MM_SVG_VK_F9
   MM_SVG_VK_F10
   MM_SVG_VK_F11
   MM_SVG_VK_F12
   MM_SVG_VK_F13
   MM_SVG_VK_F14
   MM_SVG_VK_F15
   MM_SVG_VK_F16
   MM_SVG_VK_F17
   MM_SVG_VK_F18
   MM_SVG_VK_F19
   MM_SVG_VK_F20
   MM_SVG_VK_F21
   MM_SVG_VK_F22
   MM_SVG_VK_F23
   MM_SVG_VK_F24
   
   nModifier:   [Mandatory] Key Modifiers used; combination of the following:
   MM_SVG_KEY_MOD_NONE
   MM_SVG_KEY_MOD_RIGHT_ALT
   MM_SVG_KEY_MOD_LEFT_ALT
   MM_SVG_KEY_MOD_RIGHT_CONTROL
   MM_SVG_KEY_MOD_RIGHT_CONTROL
   MM_SVG_KEY_MOD_LEFT_SHIFT
   MM_SVG_KEY_MOD_RIGHT_SHIFT
   MM_SVG_KEY_MOD_LEFT_META
   MM_SVG_KEY_MOD_RIGHT_META

Comments:
   None.

See Also:
   IMEDIASVG_Key,
   IMEDIA_Play()

=============================================================================

SVGPointerData

Description: 
This structure contains information for processing Pointer events during SVG
playback.

Definition:
   typedef struct SVGPointerData
   {
      int16 x;
      int16 y;
      int16 nKeyModifier;
   } SVGPointerData;

Members:
   x:            [Mandatory] x-coordinate of pointer position
   y:            [Mandatory] y-coordinate of pointer position
   nKeyModifier: [Mandatory] Key Modifiers; combination of MM_SVG_KEY_MOD_XXX (see SVGKeyData)

Comments:
   None.

See Also:
   IMEDIASVG_PointerClick(),
   IMEDIASVG_PointerDown(),
   IMEDIASVG_PointerMove(),
   IMEDIASVG_PointerOver(),   
   IMEDIASVG_PointerUp(),
   IMEDIASVG_PointerLeave(),   
   IMEDIA_Play()

=============================================================================

SVGRotateData

Description: 
This structure contains information for processing Rotate events during SVG
playback.

Definition:
   typedef struct SVGRotateData
   {
      uint32 cx;
      uint32 cy;
      int16  nAngle;
   } SVGRotateData;

Members:
   cx:     [Mandatory] x-coordinate of center of rotation
   cy:     [Mandatory] y-coordinate of center of rotation
   nAngle: [Mandatory] angle of rotation

Comments:
   None.

See Also:
   IMEDIASVG_Rotate(),
   IMEDIA_Play()

=============================================================================

SVGZoomData

Description: 
This structure contains information for processing Zoom events during SVG
playback.

Definition:
   typedef struct SVGZoomData
   {
      int16 x;
      int16 y;
      uint32  nZoomFactor;
      flg flgCentered;
   } SVGZoomData;

Members:
   x:             [Mandatory] x-coordinate of zoom position
   y:             [Mandatory] y-coordinate of zoom position
   nZoomFactor:   [Mandatory] Zoom factor in 16.16 fixed point (see comments)
   flgCentered:   [Mandatory] MM_SVG_CENTER_MODE_UNCENTERED, or MM_SVG_CENTER_MODE_CENTERED to recenter in view after zoom

Comments:
   nZoomFactor is always positive and operates as a percentage.

   For example: 

   Zooming in by 2: specify 0.5 (equivalent to 50% of the view).
   To get the 16.16 fixed point value, multiply by 65536 to get 32768.
    
   Zooming out by 2: specify 2.0 (equivalent to 200% of the view)
   To get the 16.16 fixed point value, multiply by 65536 to get 131072.

See Also:
   IMEDIASVG_Zoom(),
   IMEDIA_Play()

=============================================================================

SVGPanData

Description: 
This structure contains information for processing Pan events during SVG
playback.

Definition:
   typedef struct SVGPanData
   {
      int16 x;
      int16 y;
      flg flgScrollMode;
   } SVGPanData;

Members:
   x:             [Mandatory] x-coordinate for panning
   y:             [Mandatory] y-coordinate for panning
   flgScrollMode: [Mandatory] MM_SVG_SCROLL_MODE_NORMAL for normal scrolling, MM_SVG_SCROLL_MODE_FAST for fast scrolling

Comments:
   None.

See Also:
   IMEDIASVG_Pan(),
   IMEDIA_Play()

=============================================================================

AEEMediaSVGSpec

Description: 
This structure contains information about the SVG file being played back.
It is sent via the registered callback function with status
MM_STATUS_MEDIA_SPEC.

Definition:
   typedef struct AEEMediaSVGSpec
   {
      uint16  flgFeatures;
      int16   nColorScheme;
      char    szTitle[128];
      uint32  nFrameWidth;
      uint32  nFrameHeight;
      uint32  nStructSize;
   } AEEMediaSVGSpec;

Members:
   flgFeatures:      Features supported by the media. Combination of the following:
   MM_SVG_SPEC_FEATURE_NONE:           No features supported
   MM_SVG_SPEC_FEATURE_ANIMATED:       Media is an animation. If this flag is not set then media is a single image.
   MM_SVG_SPEC_FEATURE_ZOOMPANEVENTS:  Media supports zoom and pan events
   MM_SVG_SPEC_FEATURE_TEXTONLY:       Media supports text-only mode
   
   nColorScheme:     Color scheme used; one of the following:
   MM_SVG_COLOR_SCHEME_NONE:   Unknown Color Scheme
   MM_SVG_COLOR_SCHEME_RGB565: RGB 565 format, 16 bits/pixel

   szTitle:          Title, if defined, of the SVG file. String is NULL terminated

   nFrameWidth:      Width of frames
   
   nFrameHeight:     Height of frames
   
   nStructSize:      Size of this structure
   
Comments:
   None.
   
See Also:
   AEEMediaCmdNotify,
   IMEDIA_Play()

=============================================================================

SVG Control Parameters

Description:
These parameters(MM_PARM_SVG_XXX) allow setting/getting of SVG parameters. They are used in
IMEDIA_SetMediaParm() and IMEDIA_GetMediaParm() APIs.
===pre>
MM_PARM_SVG_DPI
Set:  Sets the Dots Per Inch before decoding. This is done in Ready mode before calling
      IMEDIA_PLAY()
      p1 = uint16 value  

MM_PARM_SVG_DIR
Set:  Sets the directory for external file references before decoding. This is done in Ready mode before calling
      IMEDIA_PLAY()
      p1 = char *. NULL terminated string.  
Get:  Retrieves the directory for external file references.
      p1 = buffer to copy into
      p2 = buffer size of p1

MM_PARM_SVG_GENERATE_FRAME
Set:  Requests a frame to be generated during playback.
      p1 = uint32 value. Frame time in ms.

MM_PARM_SVG_CLIENT_FRAME_GENERATION
Set:  Sets the control mode before decoding. This is done in Ready mode before calling
      IMEDIA_PLAY()
      p1 = boolean. TRUE => Client-controlled mode.

MM_PARM_SVG_FRAMEDELAY
Set:  Sets the frame delay in automatic frame generation mode before decoding. This is done in Ready mode before calling
      IMEDIA_PLAY()
      p1 = uint32 value. Delay in ms.

MM_PARM_SVG_BACKGROUND_COLOR
Set:  Sets the frame background color before decoding. This is done in Ready mode before calling
      IMEDIA_PLAY()
      p1 = RGBVAL  

MM_PARM_SVG_INPUT
Set:  Sends user input during playback.
      p1 = MM_SVG_EVENT_XXX
      p2 = Event data.  Depends on event.  


===/pre>
=============================================================================

    SVG Command codes

Description:
    Command code is returned via registered callback function to indicate event
type and to return data to client. AEEMediaCmdNotify::nCmd in the 
callback function contains the following command codes.

Definition:
    MM_CMD_SVG_BASE:  Base used by IMediaSVG
    MM_CMD_SVG_USER_BASE:  Base for derived class

Members:
    None

Comments:
    None

See Also:
    None

=============================================================================

    SVG Status codes

Description:
    Status code is returned via registered callback function to indicate event status
and to return data to client. AEEMediaCmdNotify::nStatus sent via callback function
contains the following status codes.

Definition:
   MM_STATUS_SVG_BASE:  Base used by IMediaSVG
   MM_STATUS_SVG_USER_BASE:  Base for extension
   MM_STATUS_SVG_PREPARE_DONE:  [Play] SVG has been decoded and can begin rendering frames
   MM_STATUS_SVG_URL:           [Play] The SVG content specifies a URL should be opened due to user action; pCmdData = URL
   
Members:
   None

Comments:
   None

See Also:
   None

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

IMediaSVG Interface

Description: 
   IMediaSVG interface provides specialized methods to control SVG playback.
   An application can get the specification for the current SVG media by
   handling MM_STATUS_MEDIA_SPEC in the registered callback after calling
   IMEDIA_Play(). The data for this status will be a pointer to AEEMediaSVGSpec.

===H2>
   Event Notification:
===/H2>
===p>
 
   IMedia asynchronously notifies all the events to client app via the
   callback function. The app must register a callback notification function
   using IMEDIA_RegisterNotify().
===H2>
   Frame generation:
===/H2>
===p>
   If automatic frame generation mode is enabled (the default), IMedia renders frames based on the frame delay specified.
===p>
   If client-controlled mode is enabled, the application must call IMEDIASVG_GenerateFrame() to request each frame to be rendered.
===H2>
   Display:
===/H2>
===p>
   If frame callbacks are not enabled (the default), IMediaSVG displays the rendered frames.
===p>
   If frame callbacks are enabled, IMedia dispatches the rendered frames via the user-registered callback 
   function with status MM_STATUS_FRAME.
   In the callback, the application needs to call IMEDIA_GetFrame() to get the frame
   represented by IBitmap and it is the application's responsibility to display these frames onto the screen or other destination.
===H2> 
   Sample Code:
===/H2>

===pre>
   The following code snippet plays an SVG file.

      static int CApp_PlaySVG(CApp * pme)
      {
         int   nErr;
         AEEMediaData md;

         // Create IMedia instance.
         nErr = ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_MEDIASVG, (void **)&pme->m_pIMedia);
         if (nErr)
            return nErr;
            
         // Open the file
         md.clsData = MMD_FILE_NAME;
         md.pData = "myfile.svg";
         md.dwSize = 0;
         nErr = IMEDIA_SetMediaData(pme->m_pIMedia, &md); 
         if (nErr)
            return nErr;
            
         // Register callback notification function.
         nErr = IMEDIA_RegisterNotify(pme->m_pIMedia, CApp_MediaNotify, pme);
         if (nErr)
            return nErr;
         
         // Set the frame background color to black
         IMEDIASVG_SetBackgroundColor(pme->m_pIMedia, RGB_BLACK);
         
         // We want to play parts of the file out of order, 
         // so set to client-controlled frame generation mode 
         // to control how playback happens
         IMEDIASVG_EnableClientFrameGeneration(pme->m_pIMedia, TRUE);
         
         // We will draw the frames instead of letting BREW draw them
         IMEDIA_EnableFrameCallback(pme->m_pIMedia, TRUE);
         
         nErr = IMEDIA_Play(pme->m_pIMedia);
         if (nErr)
            return nErr;
            
         return SUCCESS;
      }

      static void CApp_MediaNotify(void * pUser, AEEMediaCmdNotify * pn)
      {
         CApp *   pme = (CApp *)pUser;

         if (!pme || !pn)
            return;

         switch (pn->nStatus)
         {
            case MM_STATUS_START:
               // Now in Play mode...
               break;

            case MM_STATUS_PREPARE_DONE:
               // SVG file data has been decoded. Frames can now be rendered

               // Request a frame be generated from 5000 ms into the file
               pme->nFrameTime = 5000
               IMEDIASVG_GenerateFrame(pme->m_pIMedia, pme->nFrameTime);
               break;

            case MM_STATUS_FRAME:
            {
               IBitmap *      pFrame;
               AEEBitmapInfo  bi;
               
               //
               // IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
               //
               if (SUCCESS != IMEDIA_GetFrame(pme->m_pIMedia, &pFrame))
                  return;

               // Get the bitmap info...this can be saved in app global structure.
               IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

               // Display the frame at (0, 0) location of the screen
               IDISPLAY_BitBlt(pme->m_pIDisplay, 0, 0, bi.cx, bi.cy, pFrame, 0, 0, AEE_RO_COPY);
               IDISPLAY_Update(pme->m_pIDisplay);
               IBITMAP_Release(pFrame);
               
               // Generate the next frame we want to display
               pme->nFrameTime -= 1000;
               if (pme->nFrameTime >= 0)
               {
                  IMEDIASVG_GenerateFrame(pme->m_pIMedia, pme->nFrameTime);
               }
               else
               {
                  IMEDIA_Stop(pme->m_pIMedia);
               }
               
               break;
            }

            case MM_STATUS_DONE:
               // IMEDIA_Stop() was called and playback stopped.
               break;

            case MM_STATUS_ABORT:
               // Playback got aborted.
               break;
         }
      }
===/pre>
===H2>
State Machine:
===/H2>
===p>
Please see IMedia.
===p>
The following header file is required:~
AEEMediaSVG.h
=============================================================================

IMEDIASVG_EnableClientFrameGeneration()

Description:
    This function allows the application to control frame generation. 


Prototype:
    int IMEDIASVG_EnableClientFrameGeneration
    (
    IMedia * pIMedia, 
    boolean bEnable 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    bEnable:  [in]:  TRUE/FALSE => Enable/Disable client control of frame generation

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    By default frames are automatically generated from the beginning of the
    media to the end of the media based on a specified delay. An application
    can call this function before starting playback to enable manual control
    of the generation of frames by the application. In client-controlled mode,
    the application must request generation of each frame it wants. This allows
    parts of the media to be played out of order. In this mode, each frame the
    application wants to create is generated by calling IMEDIASVG_GenerateFrame().

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_Focus()

Description:
    This function sends a focus event to the SVG decoder. 


Prototype:
    int IMEDIASVG_Focus
    (
    IMedia * pIMedia, 
    int16 nFocusEvent 
    )

Parameters:
    pIMedia:  [in]  Pointer to IMedia Interface.
    
    nFocusEvent:  [in]  Can have one of the following values -
    
	===pre>
	MM_SVG_FOCUS_EVENT_NONE:     No Event
    MM_SVG_FOCUS_EVENT_BEGIN:    Sets Focus to first visible focusable element
    MM_SVG_FOCUS_EVENT_NEXT:     Sets Focus to next visible focusable element
    MM_SVG_FOCUS_EVENT_PREVIOUS: Sets Focus to previous visible focusable element
    MM_SVG_FOCUS_EVENT_ACTIVATE: Activates Focus to the element with focus, might generate URL and/or file requests
    MM_SVG_FOCUS_EVENT_END:      Unsets focus to the frame
	===/pre>

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_FOCUS.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_GenerateFrame()

Description:
    This function requests a frame to be generated in client-controlled mode. 


Prototype:
    int IMEDIASVG_GenerateFrame
    (
    IMedia * pIMedia, 
    uint32 nFrameTime 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    nFrameTime:  [in]:  Time in ms.

Return Value:
    SUCCESS: Successful. Operation completed.
    MM_PENDING: Result returned via the registered callback
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_Key()

Description:
    This function sends a key event to the SVG decoder. 


Prototype:
    int IMEDIASVG_Key
    (
    IMedia * pIMedia, 
    SVGKeyData * pKeyData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pKeyData:  [in]:  Key data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_KEY.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_Pan()

Description:
    This function sends a pan event to the SVG decoder. 


Prototype:
    int IMEDIASVG_Pan
    (
    IMedia * pIMedia, 
    SVGPanData * pPanData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPanData:  [in]:  Pan data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_PAN.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerClick()

Description:
    This function sends a pointer click event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerClick
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_CLICK.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerDown()

Description:
    This function sends a pointer down event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerDown
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_DOWN.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerLeave()

Description:
    This function sends a pointer leave event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerLeave
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_LEAVE.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerMove()

Description:
    This function sends a pointer move event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerMove
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_MOVE.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerOver()

Description:
    This function sends a pointer over event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerOver
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_OVER.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_PointerUp()

Description:
    This function sends a pointer up event to the SVG decoder. 


Prototype:
    int IMEDIASVG_PointerUp
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_POINTER_UP.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_ResetFrame()

Description:
    This function resets the current frame to original view, undoing prior
    zoom and pan events


Prototype:
    int IMEDIASVG_ResetFrame
    (
    IMedia * pIMedia, 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_Rotate()

Description:
    This function sends a rotate event to the SVG decoder. 


Prototype:
    int IMEDIASVG_Rotate
    (
    IMedia * pIMedia, 
    SVGRotateData * pRotateData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pRotateData:  [in]:  Rotate data.

Return Value:
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_ROTATE.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_SetBackgroundColor()

Description:
    This function sets the frame background color. 


Prototype:
    int IMEDIASVG_SetBackgroundColor
    (
    IMedia * pIMedia, 
    RGBVAL rgbColor 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    rgbColor:  [in]:  Background color

Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    By default frames are rendered on a white background.
    
Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_SetDir()

Description:
    This function sets the default directory for external files. 


Prototype:
    int IMEDIASVG_SetDir
    (
    IMedia * pIMedia, 
    char * szDir 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    szDir:  [in]:  Directory (NULL terminated).
    
Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    SVG content may contain external file references. If such a reference
    does not contain a directory path, the SVG decoder will use the path
    set by this function. By default this is the same directory path where 
    the SVG file exists.
    
Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_SetDPI()

Description:
    This function sets the frame dots per inch (DPI). 


Prototype:
    int IMEDIASVG_SetDPI
    (
    IMedia * pIMedia, 
    uint16 nDPI 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    nDPI:  [in]:  Dots per inch.
    
Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    By default frames are rendered with DPI of 72.
    
Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_SetFrameDelay()

Description:
    This function sets the frame delay for autmatic rendering mode. 


Prototype:
    int IMEDIASVG_SetFrameDelay
    (
    IMedia * pIMedia, 
    uint32 nFrameDelay 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    nFrameDelay:  [in]:  Frame delay in ms.
    
Return Value:
    SUCCESS: Successful. Operation completed.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    By default frames are generated as often as possible (with a
    delay of 0). Increasing the delay between frames will cause 
    fewer frames to be generated over the time of the SVG playback, 
    creating choppier animation. 
    
Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

=============================================================================

IMEDIASVG_Zoom()

Description:
    This function sends a zoom event to the SVG decoder. 


Prototype:
    int IMEDIASVG_Zoom
    (
    IMedia * pIMedia, 
    SVGZoomData * pZoomData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pZoomData:  [in]:  Zoom data.

Return Value:
    SUCCESS: Successful. Operation completed.
    MM_PENDING: Result returned via the registered callback
                with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                and pCmdData is MM_SVG_EVENT_ZOOM.
    EBADPARM: Bad parm
    ENOMEMORY: Not enough memory
    EBADSTATE: Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED: General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMEDIA_SetMediaParm()

============================================================================= */

#endif // AEEMEDIASVG_H
