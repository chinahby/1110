/*===========================================================================

FILE: qalbumAdv_i.h

GENERAL DESCRIPTION
    This is a private header file only used by qalbumAdv.c.

      Copyright (c) 2004 - 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qalbum/main/latest/src/qalbumAdv_i.h#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 12/21/06  RK     Remove the Filter for RGB Histogram feature in 6K targets.  
 11/28/06  NC     Filter out RGB Histogram feature from 6K targets.
 11/08/06  ZL     Replaced FEATURE_APP_QCAMERA with FEATURE_APP_QALBUM.
 10/09/06  NC     Defined new helper function and items.
 09/18/06  NC     Added Qalbum_Get2ndFile to simplify code.
 11/23/05  JN     Support for luma histogram.
 11/08/05  JN     Support for whiteboard effect.
 11/03/05  NC     Added three new helper functions.
 01/28/05  NC     Removed AdvEditIDList back to qalbum_i.h
 12/21/04  JN     Add advanced edit option into qalbum.
 ============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#if defined(FEATURE_APP_QALBUM)

#include "AEEModGen.h"   // Module interface definitions
#include "AEEStdLib.h"   // AEE stdlib services

#include "AEEMedia.h"    // AEE Multimedia Services

#include "qalbum.h"
#include "qalbum.bid"
#include "QcamDisp.h"

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean Qalbum_OnMagnify(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnNeon(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnPerspective(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnFog(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnGaussianBlur(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnUniformBlur(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnMotionBlur(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnArbRotation(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnAlphaBlend(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnOverlap(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnHorizontalComp(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnVerticalComp(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnCenterComp(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnWhiteboard(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnHistogram(Qalbum *pMe, uint16 key);
static boolean Qalbum_OnRgbHistogram(Qalbum *pMe, uint16 key);

static boolean Qalbum_DoMagnify(Qalbum *pMe);
static boolean Qalbum_Doperspective(Qalbum *pMe, IIPLLine *pLine, IIPLPerspectiveType type);
static boolean Qalbum_DoOverlap(Qalbum *pMe);
static boolean Qalbum_DoCompose(Qalbum *pMe, uint16 id, uint16 key, IIPLComposeType type);

static void    Qalbum_Get2ndFile(Qalbum *pMe, uint16 key);
/*-------------------------------------------------------------------
  Advanced Edit Options:
-------------------------------------------------------------------*/

// edit menu on select key handler function table
static const QalbumMenuKeyHandler QalbumAdvEditHandlers[] = {
  {IDS_MAGNIFY,             Qalbum_OnMagnify},
  {IDS_NEON,                Qalbum_OnNeon},
  {IDS_PERSPECTIVE,         Qalbum_OnPerspective},
  {IDS_FOG,                 Qalbum_OnFog},
  {IDS_GAUSSIAN_BLUR,       Qalbum_OnGaussianBlur},
  {IDS_UNIFORM_BLUR,        Qalbum_OnUniformBlur},
  {IDS_MOTION_BLUR,         Qalbum_OnMotionBlur},
  {IDS_ARB_ROTATION,        Qalbum_OnArbRotation},
  {IDS_ALPHA_BLEND,         Qalbum_OnAlphaBlend},
  {IDS_OVERLAP,             Qalbum_OnOverlap},
  {IDS_HORIZONTAL_COMP,     Qalbum_OnHorizontalComp},
  {IDS_VERTICAL_COMP,       Qalbum_OnVerticalComp},
  {IDS_CENTER_COMP,         Qalbum_OnCenterComp},
  {IDS_WHITEBOARD,          Qalbum_OnWhiteboard},
  {IDS_HISTOGRAM,           Qalbum_OnHistogram},
  {IDS_RGB_HISTOGRAM,       Qalbum_OnRgbHistogram}
};

#endif //FEATURE_APP_QALBUM
