#ifndef AEEIMEDIASVG_H
#define AEEIMEDIASVG_H

/*============================================================================
FILE:	AEEIMediaSVG.h

SERVICES:  BREW SVG Services

DESCRIPTION:
   This file defines IMediaSVG interface that provides specialized services
   for Scalable Vector Graphics (SVG) media.
   
PUBLIC CLASSES:  IMediaSVG

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

=============================================================================
        Copyright (c) 1999-2008 QUALCOMM Incorporated.
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

#include "AEEIMedia.h"
#include "AEEIBitmap.h"

#define AEEIID_IMediaSVG                      0x0106e202

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
#define MM_STATUS_SVG_URI_RESOURCE         (MM_STATUS_SVG_BASE + 2)   // [Play] The SVG content contains a URI referencing a needed image; pCmdData = SVGURIResourceRequestData*
#define MM_STATUS_SVG_URI_DATA             (MM_STATUS_SVG_BASE + 3)   // [Play] Notification that URI data was successfully loaded; pCmdData = SVGURIResourceDataAck*

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
// Set/Get IMediaSVG parms using IMedia_SetMediaParm()/IMedia_GetMediaParm().
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
#define MM_PARM_SVG_PROGRESSIVE_RENDER        (MM_PARM_SVG_BASE + 7)  // [Set] Enables/Disables progressive rendering
#define MM_PARM_SVG_AUTO_STOP                 (MM_PARM_SVG_BASE + 8)  // [Set] Enables/Disables auto stop mode

// SVG DOM operations
#define MM_PARM_SVG_DOM_EMPTY_DOC             (MM_PARM_SVG_BASE + 9)  // [Set] Creates and empty document    
#define MM_PARM_SVG_TRANSPARENCY              (MM_PARM_SVG_BASE + 10) // [Set] Const alpha transparency value
#define MM_PARM_SVG_ENABLE_TRANSPARENCY       (MM_PARM_SVG_BASE + 11) // [Set] Enables/Disables alpha transparency
#define MM_PARM_SVG_URIRESOURCE_HANDLED       (MM_PARM_SVG_BASE + 12) // [Set] Enables/Disables client URI image handling  
#define MM_PARM_SVG_URIRESOURCE_DATA          (MM_PARM_SVG_BASE + 13) // [Set] Attaches an image to document
#define MM_PARM_SVG_VERSION                   (MM_PARM_SVG_BASE + 14) // [Get] Gets the supported version of SVG Tiny

// SVG Viewport
#define MM_PARM_SVG_DEFAULT_CANVAS            (MM_PARM_SVG_BASE + 15)  // DefaultCanvas
#define MM_PARM_SVG_VIEWPORT                  (MM_PARM_SVG_BASE + 16)  // Viewport

// SVG Document verify
#define MM_SVG_DOCUMENT_VERIFY                (MM_PARM_SVG_BASE + 17)   // Document verify

// SVG Focus Info Attribute
#define MM_PARM_FOCUS_INFO                    (MM_PARM_SVG_BASE + 18)   // Focus Info

// SVG version
#define SVGT_VERSION_11  1   // [deprecated] implementation supports SVG Tiny 1.1 functionality
#define SVGT_VERSION_12  2   // implementation supports SVG Tiny 1.2 functionality

//
// Color Schemes
//
#define MM_SVG_COLOR_SCHEME_BASE         0                     // Base used by IMediaSVG
#define MM_SVG_COLOR_SCHEME_USER_BASE    0x1000                // Base used for extension

#define MM_SVG_COLOR_SCHEME_NONE         (MM_SVG_COLOR_SCHEME_BASE + 1)       // Unknown Color Scheme
#define MM_SVG_COLOR_SCHEME_RGB565       (MM_SVG_COLOR_SCHEME_BASE + 2)   // RGB 565 format, 16 bits/pixel
#define MM_SVG_COLOR_SCHEME_RGB888       (MM_SVG_COLOR_SCHEME_BASE + 3)   // RGB 888 format, 24 bits/pixel
#define MM_SVG_COLOR_SCHEME_0RGB888      (MM_SVG_COLOR_SCHEME_BASE + 4)   // RGB 888 format, 32 bits/pixel
#define MM_SVG_COLOR_SCHEME_ARGB8888     (MM_SVG_COLOR_SCHEME_BASE + 5)   // Alpha + RGB 888 format, 32 bits/pixel
#define MM_SVG_COLOR_SCHEME_RGBA8888     (MM_SVG_COLOR_SCHEME_BASE + 8)   // RGBA 8888 format, 32 bits/pixel

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
#define MM_SVG_EVENT_TEXT_INPUT       (MM_SVG_EVENT_BASE + 12)  // Text input from device key or keyboard
#define MM_SVG_EVENT_KEY_DOWN         (MM_SVG_EVENT_BASE + 13)  // Key was pressed down
#define MM_SVG_EVENT_KEY_UP           (MM_SVG_EVENT_BASE + 14)  // Pressed key was released

#define MM_SVG_EVENT_FOCUS_IN         (MM_SVG_EVENT_BASE + 15)  // DOMFocusIn
#define MM_SVG_EVENT_FOCUS_OUT        (MM_SVG_EVENT_BASE + 16)  // DOMFocusOut
#define MM_SVG_EVENT_DOM_ACTIVATE     (MM_SVG_EVENT_BASE + 17)  // DOMActivate

#define MM_SVG_EVENT_MOUSE            (MM_SVG_EVENT_BASE + 18)  // Mouse Event
#define MM_SVG_EVENT_TEXT             (MM_SVG_EVENT_BASE + 19)  // Text Event
#define MM_SVG_EVENT_KEYBOARD         (MM_SVG_EVENT_BASE + 20)  // Keyboard Event
#define MM_SVG_EVENT_CONNECTION       (MM_SVG_EVENT_BASE + 21)  // Connection Event
#define MM_SVG_EVENT_TIME             (MM_SVG_EVENT_BASE + 22)  // Time Event
#define MM_SVG_EVENT_UI               (MM_SVG_EVENT_BASE + 23)  // UI Event
#define MM_SVG_EVENT_WHEEL            (MM_SVG_EVENT_BASE + 24)  // Wheel Event
#define MM_SVG_EVENT_PROGRESS         (MM_SVG_EVENT_BASE + 25)  // Progress Event

#define MM_SVG_RESOURCE_BASE           0
#define MM_SVG_RESOURCE_FS            (MM_SVG_RESOURCE_BASE + 1)  // External resource in file system
#define MM_SVG_RESOURCE_URL           (MM_SVG_RESOURCE_BASE + 2)  // External resource in URL

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

// DEPRECATED
#define MM_SVG_FOCUS_EVENT_NORTH      (MM_SVG_FOCUS_EVENT_BASE + 6)   // Move to Focusable element at North 
#define MM_SVG_FOCUS_EVENT_NORTH_EAST (MM_SVG_FOCUS_EVENT_BASE + 7)   // Move to Focusable element at North-East 
#define MM_SVG_FOCUS_EVENT_EAST       (MM_SVG_FOCUS_EVENT_BASE + 8)   // Move to Focusable element at East 
#define MM_SVG_FOCUS_EVENT_SOUTH_EAST (MM_SVG_FOCUS_EVENT_BASE + 9)   // Move to Focusable element at South-East 
#define MM_SVG_FOCUS_EVENT_SOUTH      (MM_SVG_FOCUS_EVENT_BASE + 10)  // Move to Focusable element at South 
#define MM_SVG_FOCUS_EVENT_SOUTH_WEST (MM_SVG_FOCUS_EVENT_BASE + 11)  // Move to Focusable element at South-West 
#define MM_SVG_FOCUS_EVENT_WEST       (MM_SVG_FOCUS_EVENT_BASE + 12)  // Move to Focusable element at West 
#define MM_SVG_FOCUS_EVENT_NORTH_WEST (MM_SVG_FOCUS_EVENT_BASE + 13)  // Move to Focusable element at North-West 
// end DEPRECATED

#define MM_SVG_FOCUS_EVENT_UP         (MM_SVG_FOCUS_EVENT_BASE + 14)  // Move to Focusable element in the upward direction.
#define MM_SVG_FOCUS_EVENT_UP_RIGHT   (MM_SVG_FOCUS_EVENT_BASE + 15)  // Move to Focusable element in the upward-rightward direction.
#define MM_SVG_FOCUS_EVENT_RIGHT      (MM_SVG_FOCUS_EVENT_BASE + 16)  // Move to Focusable element in the rightward direction.
#define MM_SVG_FOCUS_EVENT_DOWN_RIGHT (MM_SVG_FOCUS_EVENT_BASE + 17)  // Move to Focusable element in the downward-rightward direction.
#define MM_SVG_FOCUS_EVENT_DOWN       (MM_SVG_FOCUS_EVENT_BASE + 18)  // Move to Focusable element in the downward direction.
#define MM_SVG_FOCUS_EVENT_DOWN_LEFT  (MM_SVG_FOCUS_EVENT_BASE + 19)  // Move to Focusable element in the downward-leftward direction.
#define MM_SVG_FOCUS_EVENT_LEFT       (MM_SVG_FOCUS_EVENT_BASE + 20)  // Move to Focusable element in the leftward direction.
#define MM_SVG_FOCUS_EVENT_UP_LEFT    (MM_SVG_FOCUS_EVENT_BASE + 21)  // Move to Focusable element in the upward-leftward direction.


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

#define MM_SVG_SPEC_FEATURE_SEEKABLE        (MM_SVG_SPEC_FEATURE_BASE + 8)    // [deprecated] SVG media allows seek operations

#define MM_SVG_SPEC_FEATURE_AUTOSTOP        (MM_SVG_SPEC_FEATURE_BASE + 16)     // SVG media supports AutoStop
#define MM_SVG_SPEC_FEATURE_USEREVENTS      (MM_SVG_SPEC_FEATURE_BASE + 32)     // SVG media supports User events for Interactivity

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
// Structure containing flags indicating if the content is well form
//
#define MM_SVG_DOC_VERIFY_BASE                  0x0       // No flags
#define MM_SVG_DOC_VERIFY_NO_ERROR              (MM_SVG_DOC_VERIFY_BASE)       // No flags
#define MM_SVG_DOC_VERIFY_UNKNOWN_OPCODE        (MM_SVG_DOC_VERIFY_BASE + 1)   // an element contains unknown attributes.or properties
#define MM_SVG_DOC_VERIFY_MISSING_REQ_ATTRI     (MM_SVG_DOC_VERIFY_BASE + 2)   // an element is missing a required attribute
#define MM_SVG_DOC_VERIFY_CIR_REF               (MM_SVG_DOC_VERIFY_BASE + 4)   // the document contians a circular reference
#define MM_SVG_DOC_VERIFY_LOCAL_IMAGE_REF       (MM_SVG_DOC_VERIFY_BASE + 8)   // the document contains a local image reference.
#define MM_SVG_DOC_VERIFY_DUP_ID                (MM_SVG_DOC_VERIFY_BASE + 16)  // the document contains elements with a duplicate ID.
#define MM_SVG_DOC_VERIFY_ANIMATION_ERROR       (MM_SVG_DOC_VERIFY_BASE + 32)  // the document contains an error in animation
#define MM_SVG_DOC_VERIFY_UNSUPPORED_VAL        (MM_SVG_DOC_VERIFY_BASE + 64)  // the document contains an element with an unsupported value in XML 1.0 specifiction

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
   unsigned flgCentered;       // [Mandatory] MM_SVG_CENTER_MODE_UNCENTERED, or MM_SVG_CENTER_MODE_CENTERED to recenter in view after zoom
} SVGZoomData;

//
// Pan event specification structure
//
typedef struct SVGPanData
{
   int16 x;                // [Mandatory] x-coordinate for panning
   int16 y;                // [Mandatory] y-coordinate for panning
   unsigned flgScrollMode;      // [Mandatory] MM_SVG_SCROLL_MODE_NORMAL for normal scrolling, MM_SVG_SCROLL_MODE_FAST for fast scrolling
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

//
// Structure containing a request for external data from a URI
//
typedef struct SVGURIResourceRequestData
{
   char    *pURL;               // URL of requested image data
   boolean  bMoreRequests;      // TRUE if more requests will be coming
} SVGURIResourceRequestData;

//
// Structure for sending in data fetched from a URL
//
typedef struct SVGURIResourceData
{
   char   *pURL;                // [Mandatory] URL sent in MM_STATUS_SVG_URI_RESOURCE status callback
   uint32 dwBufferSize;         // [Mandatory] Number of bytes in pBuffer
   uint8  pBuffer[1];           // [Mandatory] Buffer containing entire image data
} SVGURIResourceData;

//
// Structure containing acknowledgement that external URI data was successfully loaded
//
typedef struct SVGURIResourceDataAck
{
   char   *pURL;                // URL sent in MM_STATUS_SVG_URI_RESOURCE status callback
   boolean bLoaded;             // TRUE if data was successfully loaded
} SVGURIResourceDataAck;

typedef struct SVGDocumentVerify
{
   uint16  flgDocumentVerify;
} SVGDocumentVerify;

typedef struct _SVGRectRegion
{
   int32  x1;
   int32  y1;
   int32  x2;
   int32  y2;
} SVGRectRegion;

typedef struct _SVGFocusInfo
{
   SVGRectRegion  boundingBox;  /* Out: Bounding box in Screen coordinates */
   boolean  bHighlightFocus;    /* Out: True if focusHighlight attribute of element evaluates to <auto> */
} SVGFocusInfo;


/*===========================================================================
                    CLASS DEFINITIONS
===========================================================================*/
#define INHERIT_IMediaSVG(iname) \
   INHERIT_IMedia(iname)

AEEINTERFACE_DEFINE(IMediaSVG);

/*-------------------------------------------------------------------
      API Definitions
-------------------------------------------------------------------*/

// IMedia accessors
static __inline uint32 IMediaSVG_AddRef(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->AddRef(p);
}
static __inline uint32 IMediaSVG_Release(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Release(p);
}
static __inline int IMediaSVG_QueryInterface(IMediaSVG *p, AEEIID i, void **p2)
{
   return AEEGETPVTBL(p, IMediaSVG)->QueryInterface(p, i, p2);
}
static __inline int IMediaSVG_RegisterNotify(IMediaSVG *p, PFNMEDIANOTIFY f, void *pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->RegisterNotify(p, f, pd);
}

static __inline int IMediaSVG_SetMediaParm(IMediaSVG *p, int c, int32 p1, int32 p2)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, c, p1, p2);
}
static __inline int IMediaSVG_GetMediaParm(IMediaSVG *p, int c, int32 *pp1, int32 *pp2)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetMediaParm(p, c, pp1, pp2);
}

static __inline int IMediaSVG_Play(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Play(p);
}
static __inline int IMediaSVG_Record(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Record(p);
}
static __inline int IMediaSVG_Stop(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Stop(p);
}

static __inline int IMediaSVG_Seek(IMediaSVG *p, AEEMediaSeek s, int32 t)
{
   return AEEGETPVTBL(p, IMediaSVG)->Seek(p, s, t);
}
static __inline int IMediaSVG_Rewind(IMediaSVG *p, int32 t)
{
   return AEEGETPVTBL(p, IMediaSVG)->Seek(p, MM_SEEK_CURRENT, (t)*(-1));
}
static __inline int IMediaSVG_FastForward(IMediaSVG *p, int32 t)
{
   return AEEGETPVTBL(p, IMediaSVG)->Seek(p, MM_SEEK_CURRENT, t);
}
static __inline int IMediaSVG_SeekFrame(IMediaSVG *p, AEEMediaSeek s, int32 t)
{
   return AEEGETPVTBL(p, IMediaSVG)->Seek(p, MM_SEEK_MODE_FRAME | s, t);
}

static __inline int IMediaSVG_Pause(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Pause(p);
}
static __inline int IMediaSVG_Resume(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->Resume(p);
}

static __inline int IMediaSVG_GetTotalTime(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetTotalTime(p);
}

static __inline int IMediaSVG_GetState(IMediaSVG *p, boolean *pb)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetState(p, pb);
}

// IMediaSVG functions

// Commonly used APIs to set/get IMedia parm
//
// Initialization:
static __inline int IMediaSVG_SetDPI(IMediaSVG *p, uint16 v)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_DPI, (int32)v, 0);
}

static __inline int IMediaSVG_SetDir(IMediaSVG *p, char* sz)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_DIR, (int32)sz, 0);
}

static __inline int IMediaSVG_EnableClientFrameGeneration(IMediaSVG *p, boolean b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_CLIENT_FRAME_GENERATION, (int32)b, 0);
}

static __inline int IMediaSVG_SetFrameDelay(IMediaSVG *p, uint32 v)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_FRAMEDELAY, (int32)v, 0);
}

static __inline int IMediaSVG_SetBackgroundColor(IMediaSVG *p, RGBVAL v)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_BACKGROUND_COLOR, (int32)v, 0);
}

// User interactivity

static __inline int IMediaSVG_Key(IMediaSVG *p, SVGKeyData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_KEY, (int32)pd);
}

static __inline int IMediaSVG_Focus(IMediaSVG *p, int16 v)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_FOCUS, (int32)v);
}

static __inline int IMediaSVG_PointerClick(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_CLICK, (int32)pd);
}

static __inline int IMediaSVG_PointerDown(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_DOWN, (int32)pd);
}

static __inline int IMediaSVG_PointerMove(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_MOVE, (int32)pd);
}

static __inline int IMediaSVG_PointerOver(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_OVER, (int32)pd);
}

static __inline int IMediaSVG_PointerUp(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_UP, (int32)pd);
}

static __inline int IMediaSVG_PointerLeave(IMediaSVG *p, SVGPointerData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_POINTER_LEAVE, (int32)pd);
}

static __inline int IMediaSVG_Rotate(IMediaSVG *p, SVGRotateData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_ROTATE, (int32)pd);
}

static __inline int IMediaSVG_Pan(IMediaSVG *p, SVGPanData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_PAN, (int32)pd);
}

static __inline int IMediaSVG_Zoom(IMediaSVG *p, SVGZoomData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_ZOOM, (int32)pd);
}

static __inline int IMediaSVG_TextInput(IMediaSVG *p, SVGKeyData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_TEXT_INPUT, (int32)pd);
}

static __inline int IMediaSVG_KeyDown(IMediaSVG *p, SVGKeyData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_KEY_DOWN, (int32)pd);
}

static __inline int IMediaSVG_KeyUp(IMediaSVG *p, SVGKeyData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_KEY_UP, (int32)pd);
}

static __inline int IMediaSVG_ResetFrame(IMediaSVG *p)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_INPUT, MM_SVG_EVENT_RESET_FRAME, 0);
}

// Frame generation

static __inline int IMediaSVG_GenerateFrame(IMediaSVG *p, uint32 v)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_GENERATE_FRAME, (int32)v, 0);
}

static __inline int IMediaSVG_EnableProgressiveRender(IMediaSVG *p, boolean b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_PROGRESSIVE_RENDER, (int32)b, 0);
}

static __inline int IMediaSVG_EnableAutoStop(IMediaSVG *p, boolean b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_AUTO_STOP, (int32)b, 0);
}

// Linked URL Images

static __inline int IMediaSVG_EnableClientURIResourceHandling(IMediaSVG *p, boolean b, int16 r)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_URIRESOURCE_HANDLED, (int32)b, (int16)r);
}

static __inline int IMediaSVG_URIResourceData(IMediaSVG *p, SVGURIResourceData* pd)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_URIRESOURCE_DATA, (int32)pd, 0);
}

// Empty document

static __inline int IMediaSVG_SetDOMEmptyDoc(IMediaSVG *p, boolean b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_DOM_EMPTY_DOC, (int32)b, 0);
}

// Transparency

static __inline int IMediaSVG_EnableTransparency(IMediaSVG *p, boolean b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_ENABLE_TRANSPARENCY, (int32)b, 0);
}

static __inline int IMediaSVG_SetTransparency(IMediaSVG *p, uint32 b)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_TRANSPARENCY, (uint32)b, 0);
}

// Version
static __inline int IMediaSVG_GetVersion(IMediaSVG *p, int32* pn)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetMediaParm(p, MM_PARM_SVG_VERSION, pn, NULL);
}

//Viewport functions
static __inline int IMediaSVG_SetDefaultCanvas(IMediaSVG *p, AEERect* r)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_DEFAULT_CANVAS, (int32)r, 0);
}

static __inline int IMediaSVG_SetViewport(IMediaSVG *p, AEERect* r)
{
   return AEEGETPVTBL(p, IMediaSVG)->SetMediaParm(p, MM_PARM_SVG_VIEWPORT, (int32)r, 0);
}

static __inline int IMediaSVG_GetViewport(IMediaSVG *p, AEERect* r)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetMediaParm(p, MM_PARM_SVG_VIEWPORT, (int32*)r, 0);
}

//Document verification
static __inline int IMediaSVG_DocumentVerify(IMediaSVG *p, SVGDocumentVerify* d)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetMediaParm(p, MM_SVG_DOCUMENT_VERIFY, (int32*)d, 0);
}

//Get Focus Info
static __inline int IMediaSVG_GetFocusInfo(IMediaSVG *p, SVGFocusInfo * d)
{
   return AEEGETPVTBL(p, IMediaSVG)->GetMediaParm(p, MM_PARM_FOCUS_INFO, (int32*)d, 0);
}


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
   IMediaSVG_Key,
   IMedia_Play()

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
   IMediaSVG_PointerClick(),
   IMediaSVG_PointerDown(),
   IMediaSVG_PointerMove(),
   IMediaSVG_PointerOver(),   
   IMediaSVG_PointerUp(),
   IMediaSVG_PointerLeave(),   
   IMedia_Play()

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
   IMediaSVG_Rotate(),
   IMedia_Play()

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
      unsigned flgCentered;
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
   IMediaSVG_Zoom(),
   IMedia_Play()

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
      unsigned flgScrollMode;
   } SVGPanData;

Members:
   x:             [Mandatory] x-coordinate for panning
   y:             [Mandatory] y-coordinate for panning
   flgScrollMode: [Mandatory] MM_SVG_SCROLL_MODE_NORMAL for normal scrolling, MM_SVG_SCROLL_MODE_FAST for fast scrolling

Comments:
   None.

See Also:
   IMediaSVG_Pan(),
   IMedia_Play()

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
   IMedia_Play()

=============================================================================

SVG Control Parameters

Description:
These parameters(MM_PARM_SVG_XXX) allow setting/getting of SVG parameters. They are used in
IMedia_SetMediaParm() and IMedia_GetMediaParm() APIs.
===pre>
MM_PARM_SVG_DPI
Set:  Sets the Dots Per Inch before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = uint16 value  

MM_PARM_SVG_DIR
Set:  Sets the directory for external file references before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = char *. NULL terminated string.  
Get:  Retrieves the directory for external file references.
      p1 = buffer to copy into
      p2 = buffer size of p1

MM_PARM_SVG_GENERATE_FRAME
Set:  Requests a frame to be generated during playback.
      p1 = uint32 value. Frame time in ms.

MM_PARM_SVG_CLIENT_FRAME_GENERATION
Set:  Sets the control mode before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = boolean. TRUE => Client-controlled mode.

MM_PARM_SVG_FRAMEDELAY
Set:  Sets the frame delay in automatic frame generation mode before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = uint32 value. Delay in ms.

MM_PARM_SVG_BACKGROUND_COLOR
Set:  Sets the frame background color before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
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
   IMedia_Play(). The data for this status will be a pointer to AEEMediaSVGSpec.

===H2>
   Event Notification:
===/H2>
===p>
 
   IMedia asynchronously notifies all the events to client app via the
   callback function. The app must register a callback notification function
   using IMedia_RegisterNotify().
===H2>
   Frame generation:
===/H2>
===p>
   If automatic frame generation mode is enabled (the default), IMedia renders frames based on the frame delay specified.
===p>
   If client-controlled mode is enabled, the application must call IMediaSVG_GenerateFrame() to request each frame to be rendered.
===H2>
   Display:
===/H2>
===p>
   If frame callbacks are not enabled (the default), IMediaSVG displays the rendered frames.
===p>
   If frame callbacks are enabled, IMedia dispatches the rendered frames via the user-registered callback 
   function with status MM_STATUS_FRAME.
   In the callback, the application needs to call IMedia_GetFrame() to get the frame
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
         nErr = IMedia_SetMediaData(pme->m_pIMedia, &md); 
         if (nErr)
            return nErr;
            
         // Register callback notification function.
         nErr = IMedia_RegisterNotify(pme->m_pIMedia, CApp_MediaNotify, pme);
         if (nErr)
            return nErr;
         
         // Set the frame background color to black
         IMediaSVG_SetBackgroundColor(pme->m_pIMedia, RGB_BLACK);
         
         // We want to play parts of the file out of order, 
         // so set to client-controlled frame generation mode 
         // to control how playback happens
         IMediaSVG_EnableClientFrameGeneration(pme->m_pIMedia, TRUE);
         
         // We will draw the frames instead of letting BREW draw them
         IMedia_EnableFrameCallback(pme->m_pIMedia, TRUE);
         
         nErr = IMedia_Play(pme->m_pIMedia);
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
               IMediaSVG_GenerateFrame(pme->m_pIMedia, pme->nFrameTime);
               break;

            case MM_STATUS_FRAME:
            {
               IBitmap *      pFrame;
               AEEBitmapInfo  bi;
               
               //
               // IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
               //
               if (SUCCESS != IMedia_GetFrame(pme->m_pIMedia, &pFrame))
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
                  IMediaSVG_GenerateFrame(pme->m_pIMedia, pme->nFrameTime);
               }
               else
               {
                  IMedia_Stop(pme->m_pIMedia);
               }
               
               break;
            }

            case MM_STATUS_DONE:
               // IMedia_Stop() was called and playback stopped.
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
AEEIMediaSVG.h
=============================================================================

IMediaSVG_EnableClientFrameGeneration()

Description:
    This function allows the application to control frame generation. 


Prototype:
    int IMediaSVG_EnableClientFrameGeneration
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
    application wants to create is generated by calling IMediaSVG_GenerateFrame().

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_Focus()

Description:
    This function sends a focus event to the SVG decoder. 


Prototype:
    int IMediaSVG_Focus
    (
    IMedia * pIMedia, 
    int16 nFocusEvent 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    
    nFocusEvent:  [in]:  Can have one of the following values:
    MM_SVG_FOCUS_EVENT_NONE:     No Event
    MM_SVG_FOCUS_EVENT_BEGIN:    Sets Focus to first visible focusable element
    MM_SVG_FOCUS_EVENT_NEXT:     Sets Focus to next visible focusable element
    MM_SVG_FOCUS_EVENT_PREVIOUS: Sets Focus to previous visible focusable element
    MM_SVG_FOCUS_EVENT_ACTIVATE: Activates Focus to the element with focus, might generate URL and/or file requests
    MM_SVG_FOCUS_EVENT_END:      Unsets focus to the frame

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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_GenerateFrame()

Description:
    This function requests a frame to be generated in client-controlled mode. 


Prototype:
    int IMediaSVG_GenerateFrame
    (
    IMedia * pIMedia, 
    uint32 nFrameTime 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    nFrameTime:  [in]:  Time in ms into the file content.

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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_Key()

Description:
    This function sends a key event to the SVG decoder. 


Prototype:
    int IMediaSVG_Key
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_Pan()

Description:
    This function sends a pan event to the SVG decoder. 


Prototype:
    int IMediaSVG_Pan
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerClick()

Description:
    This function sends a pointer click event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerClick
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerDown()

Description:
    This function sends a pointer down event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerDown
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerLeave()

Description:
    This function sends a pointer leave event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerLeave
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerMove()

Description:
    This function sends a pointer move event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerMove
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerOver()

Description:
    This function sends a pointer over event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerOver
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_PointerUp()

Description:
    This function sends a pointer up event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerUp
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_ResetFrame()

Description:
    This function resets the current frame to original view, undoing prior
    zoom and pan events


Prototype:
    int IMediaSVG_ResetFrame
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_Rotate()

Description:
    This function sends a rotate event to the SVG decoder. 


Prototype:
    int IMediaSVG_Rotate
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_SetBackgroundColor()

Description:
    This function sets the frame background color. 


Prototype:
    int IMediaSVG_SetBackgroundColor
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
    
    The background color has no effect if IMediaSVG_SetTransparency() is called with alpha != 0

    
Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_SetDir()

Description:
    This function sets the default directory for external files. 


Prototype:
    int IMediaSVG_SetDir
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_SetDPI()

Description:
    This function sets the frame dots per inch (DPI). 


Prototype:
    int IMediaSVG_SetDPI
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_SetFrameDelay()

Description:
    This function sets the frame delay for autmatic rendering mode. 


Prototype:
    int IMediaSVG_SetFrameDelay
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
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_Zoom()

Description:
    This function sends a zoom event to the SVG decoder. 


Prototype:
    int IMediaSVG_Zoom
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
    IMedia_SetMediaParm()

============================================================================= */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

SVGDocumentVerify

Description: 
This structure contains information for document verify

Definition:
   typedef struct SVGDocumentVerify
   {
      uint16  flgDocumentVerify;
   } SVGDocumentVerify;

Members:
   flgDocumentVerify: document verified information  Combination of the following:

   MM_SVG_DOC_VERIFY_NO_ERROR              No error
   MM_SVG_DOC_VERIFY_UNKNOWN_OPCODE        An element contains unknown attributes.or properties
   MM_SVG_DOC_VERIFY_MISSING_REQ_ATTRI     An element is missing a required attribute
   MM_SVG_DOC_VERIFY_CIR_REF               The document contians a circular reference
   MM_SVG_DOC_VERIFY_LOCAL_IMAGE_REF       The document contains a local image reference.
   MM_SVG_DOC_VERIFY_DUP_ID                The document contains elements with a duplicate ID.
   MM_SVG_DOC_VERIFY_ANIMATION_ERROR       The document contains an error in animation
   MM_SVG_DOC_VERIFY_UNSUPPORED_VAL        The document contains an element with an unsupported value in XML 1.0 specifiction

=============================================================================

// DEPRECATED
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

   nVirtualKey: [deprecated] Virtual Key pressed; one of the following:
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
   
   nModifier:   [deprecated] Key Modifiers used; combination of the following:
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
   IMediaSVG_Key,
   IMedia_Play()

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
   nKeyModifier: [deprecated] Key Modifiers; combination of MM_SVG_KEY_MOD_XXX (see SVGKeyData)

Comments:
   None.

See Also:
   IMediaSVG_PointerClick(),
   IMediaSVG_PointerDown(),
   IMediaSVG_PointerMove(),
   IMediaSVG_PointerOver(),   
   IMediaSVG_PointerUp(),
   IMediaSVG_PointerLeave(),   
   IMedia_Play()

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
   MM_SVG_SPEC_FEATURE_ANIMATED:       [deprecated] Media is an animation. If this flag is not set then media is a single image.
   MM_SVG_SPEC_FEATURE_ZOOMPANEVENTS:  Media supports zoom and pan events
   MM_SVG_SPEC_FEATURE_TEXTONLY:       Media supports text-only mode
   MM_SVG_SPEC_FEATURE_AUTOSTOP:       Playback stops at end of content
   MM_SVG_SPEC_FEATURE_SEEKABLE:       Media allows seek operations

   nColorScheme:     Color scheme used; one of the following:
   MM_SVG_COLOR_SCHEME_NONE:   Unknown Color Scheme
   MM_SVG_COLOR_SCHEME_RGB565: RGB 565 format, 16 bits/pixel
   MM_SVG_COLOR_SCHEME_RGBA8888:  RGBA 8888 format, 32 bits/pixel

   szTitle:          Title, if defined, of the SVG file. String is NULL terminated
   nFrameWidth:      Width of frames  
   nFrameHeight:     Height of frames  
   nStructSize:      Size of this structure
   
Comments:
   None.
   
See Also:
   AEEMediaCmdNotify,
   IMedia_Play()

=============================================================================
// end DEPRECATED

SVGURIResourceRequestData

Description: 
This structure is sent to the notification callback with MM_STATUS_SVG_URI_RESOURCE.

Definition:
   typedef struct SVGURIResourceRequestData
   {
      char *pURL;                              
      boolean bMoreRequests;         
   } SVGURIResourceRequestData;

Members:
   pURL:           URL of requested image data
   bMoreRequests:  TRUE if more requests will be coming
   
Comments:
   None.
   
See Also:
   IMediaSVG_URIResourceData()

=============================================================================

SVGURIResourceData

Description: 
This structure is used to send fetched URL data to the SVG decoder.

Definition:
   typedef struct SVGURIResourceData
   {
      char   *pURL;                  // URL sent in MM_STATUS_SVG_URI_RESOURCE status callback
      uint32  dwBufferSize; 
      uint8   pBuffer[1];                             
   } SVGURIResourceData;

Members:
   pURL:          [Mandatory] URL sent in MM_STATUS_SVG_URI_RESOURCE status callback
   dwBufferSize:  [Mandatory] Number of bytes in pBuffer
   pBuffer:       [Mandatory] Buffer containing entire image data
 
Comments:
   None.
   
See Also:
   IMediaSVG_URIResourceData()

=============================================================================

SVGURIResourceDataAck

Description: 
This structure is sent to the notification callback with MM_STATUS_SVG_URL_DATA
to notify whether URL data was successfully loaded.

Definition:
   typedef struct SVGURIResourceDataAck
   {
      char   *pURL;       // URL sent in MM_STATUS_SVG_URI_RESOURCE status callback
      boolean bLoaded;         
   } SVGURIResourceDataAck;

Members:
   pURL:           URL of requested image data
   bLoaded:        TRUE if data was successfully loaded
   
Comments:
   None.
   
See Also:
   IMediaSVG_URIResourceData()

=============================================================================

SVG Control Parameters

MM_PARM_SVG_PROGRESSIVE_RENDER
Set:  Sets whether progressive rendering mode before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = boolean. TRUE => Progressive rending occurs.

MM_PARM_SVG_AUTO_STOP
Set:  Sets end of playback behavior before decoding. This is done in Ready mode before calling
      IMedia_PLAY()
      p1 = boolean. TRUE => Playback ends when end of content is reached.
                    FALSE=> Media remains in Play state and user interaction is possible.
   

MM_PARM_SVG_URIRESOURCE_HANDLED
Set:  Sets whether the client will fetch URL images. If not, the images are skipped.
      This is done in Ready mode before calling IMedia_PLAY()
      p1 = boolean. TRUE => Client will fetch URL images.

MM_PARM_SVG_URIRESOURCE_DATA
Set:  Sends externally linked URL Image data to the decoder.
      This is done in response to MM_STATUS_SVG_URI_RESOURCE.
      p1 = Image data.
      p2 = Size of image data.

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

IMediaSVG Interface

  This interface also supports IMediaSVGDOM. To obtain IMediaSVGDOM 
use IMediaSVG_QueryInterface() with AEEIID_MEDIASVGDOM.

The following header file is required:~
AEEMediaSVGDOM.h

=============================================================================
IMediaSVG_GetFocusInfo

Description:
    This function returns the focus information of the currently focussed
    element.

Prototype:
    int IMediaSVG_GetFocusInfo
    (
    IMediaSVG * pIMedia,
    SVGFocusInfo * pInfo;
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMediaSVG Interface.
    pInfo:    [out]: Focus information

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EFAILED:      General failure

Comments:
    Application can call this API immediately after IMediaSVG_Focus() to 
    get to know the focus information of the currently focussed element.

See Also:
    None

=============================================================================
IMediaSVG_DocumentVerify

Description:
    This function returns information for application if the content is 
    well formed.

Prototype:
    int IMediaSVG_DocumentVerify
    (
    IMedia * pIMedia,
    SVGDocumentVerify* pVerify;
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pVerify:   [out]: Document verify information

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EFAILED:      General failure

Comments:
    Application can be called any time after SVG content is parsed.
    DOM operation may change the content and introduce new not well 
    form attributes.

See Also:
    None

=============================================================================

IMediaSVG_EnableAutoStop()

Description:
    This function allows the application to control whether playback is
    stopped when the end of the content is reached. 


Prototype:
    int IMediaSVG_EnableAutoStop
    (
    IMedia * pIMedia, 
    boolean bEnable 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    bEnable:  [in]:  TRUE/FALSE => Enable/Disable auto stop

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    By default auto stop is set to TRUE and media will stop playing and
    return to Ready state when the end of the content is reached. If auto
    stop is set to FALSE, then when the end of the content is reached the
    media remains in Play state, giving an opportunity for new frames to
    be generated in response to user input. This is useful to allow users
    to zoom and pan on an image with only one frame, such as a map. 
    
See Also:
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_EnableClientFrameGeneration()

    To display graphics that have been manipulated via the AEEMediaSVGDOM unterface,
    client frame generation must be enabled via IMediaSVG_EnableClientFrameGeneration.

=============================================================================

IMediaSVG_EnableProgressiveRender()

Description:
    This function allows the media to be progressively rendered. 


Prototype:
    int IMediaSVG_EnableProgressiveRender
    (
    IMedia * pIMedia, 
    boolean bEnable 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    bEnable:  [in]:  TRUE/FALSE => Enable/Disable progressive rendering

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    By default progressive rendering is set to FALSE and no rendering is performed
    until all requests with externalResourcesRequired=true are available. 
    If progressive rendering is TRUE then the externalResourcesRequired attribute 
    is ignored and SVG content will start displaying without required requests. 
    Once the requests are available, they will be displayed along with the rest of
    the SVG content

See Also:
    IMedia_SetMediaParm()

=============================================================================
// DEPRECATED
IMediaSVG_Focus()

Description:
    This function sends a focus event to the SVG decoder. 


Prototype:
    int IMediaSVG_Focus
    (
    IMedia * pIMedia, 
    int16 nFocusEvent 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    
    nFocusEvent:  [in]:  Can have one of the following values:
    MM_SVG_FOCUS_EVENT_NONE:      No Event
    MM_SVG_FOCUS_EVENT_BEGIN:     [deprecated] Sets Focus to first visible focusable element
    MM_SVG_FOCUS_EVENT_NEXT:      Sets Focus to next visible focusable element
    MM_SVG_FOCUS_EVENT_PREVIOUS:  Sets Focus to previous visible focusable element
    MM_SVG_FOCUS_EVENT_ACTIVATE:  Activates Focus to the element with focus, might generate URL and/or file requests
    MM_SVG_FOCUS_EVENT_END:       [deprecated] Unsets focus to the frame
    MM_SVG_FOCUS_EVENT_NORTH      [deprecated] Move to Focusable element at North 
    MM_SVG_FOCUS_EVENT_NORTH_EAST [deprecated] Move to Focusable element at North-East 
    MM_SVG_FOCUS_EVENT_EAST       [deprecated] Move to Focusable element at East 
    MM_SVG_FOCUS_EVENT_SOUTH_EAST [deprecated] Move to Focusable element at South-East 
    MM_SVG_FOCUS_EVENT_SOUTH      [deprecated] Move to Focusable element at South 
    MM_SVG_FOCUS_EVENT_SOUTH_WEST [deprecated] Move to Focusable element at South-West 
    MM_SVG_FOCUS_EVENTWEST        [deprecated] Move to Focusable element at West 
    MM_SVG_FOCUS_NORTH_WEST       [deprecated] Move to Focusable element at North-West 
    MM_SVG_FOCUS_EVENT_UP           Move to Focusable element in the upward direction.
    MM_SVG_FOCUS_EVENT_UP_RIGHT     Move to Focusable element in the upward-rightward direction
    MM_SVG_FOCUS_EVENT_RIGHT        Move to Focusable element in the rightward direction.
    MM_SVG_FOCUS_EVENT_DOWN_RIGHT   Move to Focusable element in the downward-rightward directi
    MM_SVG_FOCUS_EVENT_DOWN         Move to Focusable element in the downward direction.
    MM_SVG_FOCUS_EVENT_DOWN_LEFT    Move to Focusable element in the downward-leftward directio
    MM_SVG_FOCUS_EVENT_LEFT         Move to Focusable element in the leftward direction.
    MM_SVG_FOCUS_EVENT_UP_LEFT      Move to Focusable element in the upward-leftward direction.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_FOCUS.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

[deprecated] IMediaSVG_Key()

Description:
    This function sends a key event to the SVG decoder. 


Prototype:
    int IMediaSVG_Key
    (
    IMedia * pIMedia, 
    SVGKeyData * pKeyData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pKeyData:  [in]:  Key data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_KEY.
    EBADPARM:     Bad parm
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================
// end DEPRECATED

IMediaSVG_KeyDown()

Description:
    This function sends a device key down event to the SVG decoder. 


Prototype:
    int IMediaSVG_KeyDown
    (
    IMedia * pIMedia, 
    SVGKeyData * pKeyData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pKeyData:  [in]:  Key data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_KEY.
    EBADPARM:     Bad parm
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_KeyUp()

Description:
    This function sends a device key up event to the SVG decoder. 


Prototype:
    int IMediaSVG_KeyUp
    (
    IMedia * pIMedia, 
    SVGKeyData * pKeyData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pKeyData:  [in]:  Key data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_KEY.
    EBADPARM:     Bad parm
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

// DEPRECATED
IMediaSVG_PointerLeave()

Description:
    This function sends a pointer leave event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerLeave
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_POINTER_LEAVE.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

[deprecated] IMediaSVG_PointerOver()

Description:
    This function sends a pointer over event to the SVG decoder. 


Prototype:
    int IMediaSVG_PointerOver
    (
    IMedia * pIMedia, 
    SVGPointerData * pPointerData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pPointerData:  [in]:  Pointer data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_POINTER_OVER.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================
// end DEPRECATED

IMediaSVG_SetDOMEmptyDoc()

Description:
    This function allows creation of an empty DOM document. 

Prototype:
    int IMediaSVG_SetDOMEmptyDoc
    (
    IMedia * pIMedia, 
    boolean bEnable 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    bEnable:  [in]:  TRUE/FALSE => Enable/Disable creation of empty DOM document

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    Traditionally IMedia and its derivatives require the application to
    indicate a media source by calling IMedia_SetMediaData(). This function
    sets up the necessary internal framework for IMedia given that there is
    no pre-existing media source for an empty DOM document.
    
    To create and display SVG content starting with an empty DOM document
    an application must follow these steps:
      - Call ISHELL_CreateInstance() with AEECLSID = AEECLSID_MEDIASVG
      - Call IMediaSVG_SetDOMEmptyDoc() with bEnable = TRUE
      - Call IMediaSVG_EnableClientFrameGeneration() with bEnable = TRUE
      - Call IMedia_Play()
      - Build the content using IMediaSVG_DOM()
      - Call IMediaSVG_GenerateFrame() to display the content

See Also:
    IMedia_SetMediaParm()
    IMediaSVG_DOM()
    IMediaSVG_EnableClientFrameGeneration()
    IMediaSVG_GenerateFrame()

=============================================================================

IMediaSVG_EnableClientURIResourceHandling()

Description:
    This function tells the SVG decoder that the application will fetch 
    externally linked resources (such as images) from a URL or file system 
    and send the data back to the decoder so that it may be displayed with 
    the rest of the content.

Prototype:
    int IMediaSVG_EnableClientURIResourceHandling
    (
    IMedia * pIMedia,
    boolean bEnable,
    int16   nResourceLocation
    )

Parameters:
    pIMedia:           [in]:  Pointer to IMedia Interface.
    bEnable:           [in]:  TRUE/FALSE => Enable/Disable notification to fetch 
                              resource referenced by URL or file system.
    nResourceLocation: [in]:  MM_SVG_RESOURCE_FS
                              MM_SVG_RESOURCE_URL

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    SVG content can contain external resource (such as images) from URL links 
    or file system meant to be displayed with the rest of the content. An 
    application must fetch the image and send the data to the decoder so that 
    it can be displayed appropriately. By default URL resource handling is disabled 
    and URL links to images will be ignored by the SVG decoder.  On the other 
    hand, the external resource in the file system is fetched by the SVG decoder
    by default.

See Also:
    IMedia_SetMediaParm()
    IMediaSVG_URIResourceData()

=============================================================================

IMediaSVG_TextInput()

Description:
    This function sends a key character event to the SVG decoder. 


Prototype:
    int IMediaSVG_TextInput
    (
    IMedia * pIMedia, 
    SVGKeyData * pKeyData 
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pKeyData:  [in]:  Key data.

Return Value:
    MM_PENDING:   Result returned via the registered callback
                  with AEEMediaCmdNotify where nCmd is MM_CMD_SETMEDIAPARM, 
                  nSubCmd is MM_PARM_SVG_INPUT, nStatus is MM_STATUS_DONE or MM_STATUS_ABORT, 
                  and pCmdData is MM_SVG_EVENT_KEY.
    EBADPARM:     Bad parm
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    None.

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IMedia_SetMediaParm()

=============================================================================

IMediaSVG_URIResourceData()

Description:
    This function sends externally linked URL image data to the decoder so
    that it can be displayed in context. 

Prototype:
    int IMediaSVG_URIResourceData
    (
    IMedia * pIMedia, 
    SVGURIResourceData * pImageData
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    pImageData:  [in]:  Pointer to structure describing image data retrieved from the URL.

Return Value:
    SUCCESS: Successful. Operation completed.
    MM_PENDING:   Result returned via the registered callback
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    After calling this function the registered notification callback function
    will be called with status MM_STATUS_SVG_URL_DATA. The data paswsed with 
    this status will be SVGURIResourceDataAck*, which tells the application
    whether the URL data was successfully loaded.

See Also:
    IMedia_SetMediaParm()
    IMediaSVG_EnableClientURIResourceHandling()

=============================================================================

Description:

    This function sets the transparency in the SVG context with the supplied alpha value.

Prototype:
    int IMediaSVG_SetTransparency
    (
    IMedia  *pIMedia, 
    uint32   alpa
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    alpha:    [in]:  the constant alpha transparency value to be used for rendering.

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    Alpha value must be a 16.16 fixed point number in the range [0.0, 1.0]. The source 
    pixels are always combined with destination pixels using the Source Over Destination
    rule [Porter-Duff]. In this context, the Source Over Destination rule has the following
    properties: a fully opaque pixel in the source must replace the destination pixel,
    a fully transparent pixel in the source must leave the destination pixel unchanged,
    and a semitransparent pixel in the source must be alpha blended with the destination pixel
    using the supplied value. The default alpha value is 1.0 (fully opaque), whennot specified.

See Also:
    IMediaSVG_EnableTransparency()

============================================================================= 

Description:

    This function enables or disables transparency processing for the SVG media.

Prototype:
    int IMediaSVG_EnableTransparency
    (
    IMedia *pIMedia, 
    boolean bEnable
    )

Parameters:
    pIMedia:  [in]:  Pointer to IMedia Interface.
    bEnable:  [in]:  TRUE/FALSE => Enable/Disable notification to apply alpha blending to final color

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    ENOMEMORY:    Not enough memory
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Parm not supported by the class
    EFAILED:      General failure

Comments:
    When bEnable is FALSE, the color components R, G, B, and A will be returned.
    When bEnable is TRUE, the returned color component will be R*A, G*A, and B*A.
    In this case, if the user specifies a constant alpha value using IMediaSVG_SetTransparency(), 
    the final color components of R, G, B will be multiplied with this user specified alpha value.
    The default value of bEnable is TRUE.

See Also:
    IMedia_SetMediaParm()
    IMediaSVG_SetTransparency()

=============================================================================

Description:

     This functions returns the version of the underlying SVG engine implementation.

Prototype:
    int IMediaSVG_GetVersion
    (
    IMedia *pIMedia, 
    int32  *pnVersion
    )

Parameters:
    pIMedia:    [in]:  Pointer to IMedia Interface.
    pnVersion: [out]:  Pointer to integer

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EUNSUPPORTED: Operation not supported by the class
    EFAILED:      General failure

Comments:
    When the implementation returns SVGT_VERSION_12, then [deprecated] Tiny 1.1
    functionality may or may not produce the same result as when the implementation 
    returns SVGT_VERSION_11.

See Also:
    None

=============================================================================

Description:

    This function will set the default viewport for content that doesn’t specify a viewport
    or specifies it in percentage terms (ie. 50% of the default viewport).
    In these cases, the viewport and frame size will be the width and height specified by this function.

Prototype:
    int IMediaSVG_SetDefaultCanvas
    (
    IMedia  *pIMedia, 
    AEERect *pRect
    )

Parameters:
    pIMedia:    [in]:  Pointer to IMedia Interface.
    pRect:      [in]:  Pointer to default canvas dimensions

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Operation not supported by the class
    EFAILED:      General failure

Comments:
    pRect->x == pRect->y and must equal 0.

    When content does specify a viewport in pixel terms, the frames returned will be the size of this viewport
    which is specified in the content, and the parameters from IMediaSVG_SetDefaultCanvas will be ignored.
    The maximum frame size though will be the size of the display. 

    If IMediaSVG_SetDefaultCanvas isn’t called, then the default viewport is assumed to be the size of the display. 

See Also:
    IMediaSVG_SetViewport()
    IMediaSVG_GetViewport()

=============================================================================

Description:

   This function sets a new viewport size and makes the SVG content fit within this viewport. 

Prototype:
    int IMediaSVG_SetViewport
    (
    IMedia  *pIMedia, 
    AEERect *pRect
    )

Parameters:
    pIMedia:    [in]:  Pointer to IMedia Interface.
    pRect:      [in]:  Pointer to viewport dimensions

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Operation not supported by the class
    EFAILED:      General failure

Comments:
    pRect->x == pRect->y and must equal 0.

See Also:
    IMediaSVG_GetViewport()
    IMediaSVG_SetDefaultCanvas()

=============================================================================

Description:

    This function returns the current viewport.

Prototype:
    int IMediaSVG_GetViewport
    (
    IMedia  *pIMedia, 
    AEERect *pRect
    )

Parameters:
    pIMedia:    [in]:  Pointer to IMedia Interface.
    pRect:      [in]:  Pointer to viewport dimensions

Return Value:
    SUCCESS:      Successful. Operation completed.
    EBADPARM:     Bad parameter value.
    EBADSTATE:    Cannot set parm in the current state
    EUNSUPPORTED: Operation not supported by the class
    EFAILED:      General failure

Comments:
    pRect->x == pRect->y == 0.

See Also:
    IMediaSVG_SetViewport()
    IMediaSVG_SetDefaultCanvas()

============================================================================= */

#endif // AEEIMEDIASVG_H
