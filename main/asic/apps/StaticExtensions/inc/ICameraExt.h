#ifndef ICAMERAEXT_H
#define ICAMERAEXT_H

/*============================================================================
FILE:   ICameraExt.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file defines extra ICamera interfaces that is not in current AEECamera.h.
   The items should be removed if they are picked up by the AEECamera.h.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright  2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/ICameraExt.h#5 $ $DateTime: 2006/07/25 16:06:05 $ $Author: williamh $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 07/21/06  wyn    Camcorder special effects control. 
 07/14/06  NC     Added Night option for BSM.
 07/10/06  NC     Updated some definitions. 
 06/07/06  NC     Initial design of the file
===============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEECamera.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//Port extra definitions for 3.1.4.
//IMPOTANT NOTE: Remove CAM_PARM_FRAME_TIMESTAMP definition if it is not defined 
//in AEECamera.h or here.
#ifndef CAM_PARM_FRAME_TIMESTAMP // This is BREW 3.1.4 with no new 3.1.5 stuff

//
//
//
//
//
// UUID Atom Info
//

#endif //ifndef CAM_PARM_FRAME_TIMESTAMP

//Port extra definitions for 3.1.4 and 3.1.5
#define CAM_PARM_ENCODESNAPSHOT_EX      (CAM_PARM_QCOM_RESERVED_BEGIN + 1)
#define CAM_PARM_FRAME_EX               (CAM_PARM_QCOM_RESERVED_BEGIN + 2)
#define CAM_PARM_TRANSITION             (CAM_PARM_QCOM_RESERVED_BEGIN+3)

#define CAM_PARM_BESTSHOT               (CAM_PARM_CTL_BASE + 130)// [Set/Get] Best shot mode
#define CAM_PARM_BRACKETING             (CAM_PARM_CTL_BASE + 131)// [Set/Get] Bracketing mode
#define CAM_PARM_DIGITAL_ZOOM_ENABLE    (CAM_PARM_CTL_BASE + 132)// [Set/Get] Enable/Disable digital zoom
#define CAM_PARM_AUTO_FOCUS_REGION_LIST (CAM_PARM_CTL_BASE + 133)// [Get] Auto focus regions
#define CAM_PARM_SHUTTER_CALLBACK       (CAM_PARM_CTL_BASE + 134)// [Get] Shutter open/close state and time stamp
#define CAM_PARM_HISTOGRAM              (CAM_PARM_CTL_BASE + 135)// [Get] Preview histogram

//
//
//CAM_PARM_BRACKETING settings
//
#define CAM_BRACKETING_BASE         0
#define CAM_BRACKETING_OFF          (CAM_BRACKETING_BASE)            // Turn off bracketing mode
#define CAM_BRACKETING_AE           (CAM_BRACKETING_BASE + 1)        // Bracketing for auto exposure
#define CAM_BRACKETING_AF           (CAM_BRACKETING_BASE + 2)        // Bracketing for auto focus
#define CAM_BRACKETING_AWB          (CAM_BRACKETING_BASE + 3)        // Bracketing for auto whitebalance
//
//Histogram reqire mode
#define CAM_HISTOGRAM_LUMA          0
#define CAM_HISTOGRAM_RED           1
#define CAM_HISTOGRAM_GREEN         2
#define CAM_HISTOGRAM_BLUE          3

// CAM_PARM_TRANSITION settings
#define CAM_TRANSITION_CURTAIN      (CAM_FADE_BASE + 4) // horizon curtains
#define CAM_TRANSITION_ALPHA        (CAM_FADE_BASE + 5) // alpha transition
#define CAM_TRANSITION_OFF          (CAM_FADE_BASE + 6) // transition off

typedef struct CameraSnapshotMulti
{
  int      nCount;     // Number of snapshots
  uint32   dwInterval; // Suggested interval between snapshots. Zero means burst mode.
} CameraSnapshotMulti;

typedef struct CameraShutterStatus
{
  boolean  bOpen;
  uint64   ullTimeStampMS;
} CameraShutterStatus;

//
// psm points to CameraSnapshotMulti. If it is NULL, it defaults to ICAMERA_RecordSnapshot()
// Callback sequence:
// When app calls ICAMERA_RecordSnapshotEx(),
//    - A callback is received for each frame. CAM_STATUS_FRAME, AEECameraNotify::*pData contains an int (the frame index).
//    - {CAM_STATUS_DONE, CAM_CMD_START, CAM_MODE_SNAPSHOT, *pData = int (Total # of snapshots)
//    - {CAM_STATUS_DONE, CAM_CMD_ENCODESNAPSHOT, *pData = int (index of encoded snapshot)
//
static __inline int ICAMERA_RecordSnapshotEx(ICamera *me, CameraSnapshotMulti *psm)
{
   return AEEGETPVTBL(me,ICamera)->Start(me, CAM_MODE_SNAPSHOT, (uint32)psm);
}

static __inline int ICAMERA_GetFrameEx(ICamera *me, int nIndex, IBitmap **ppFrame)
{
   return AEEGETPVTBL(me,ICamera)->GetParm(me, CAM_PARM_FRAME_EX, (int32 *)nIndex, (int32 *)ppFrame);
}

static __inline int ICAMERA_EncodeSnapshotEx(ICamera *me, int nIndex)
{
   return AEEGETPVTBL(me,ICamera)->SetParm(me, CAM_PARM_ENCODESNAPSHOT_EX, (int32)nIndex, 0);
}


#endif // AEECAMERAEXT_H
