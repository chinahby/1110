#ifndef OEMCAMERAEX_H
#define OEMCAMERAEX_H
/*============================================================================
FILE:        OEMCamera.h

SERVICES:  BREW Camera Services

DESCRIPTION:
   This file is contains defines for OEMCamera.c

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEE_OEMComdef.h"
#include "AEECameraEx.h"
#include "AEEClassIDs.h"
#include "AEECamera.bid"

#define CAM_FOCUS_RECT_BASE            1                       // Base used by ICamera
#define CAM_FOCUS_RECT_USER_BASE       0x1000                  // Base used for extension

#define CAM_FOCUS_RECT_AUTO             (CAM_FOCUS_RECT_BASE)       
#define CAM_FOCUS_RECT_SPOT             (CAM_FOCUS_RECT_BASE + 1)   
#define CAM_FOCUS_RECT_AVERAGE          (CAM_FOCUS_RECT_BASE + 2)   
#define CAM_FOCUS_RECT_CENTER_WEIGHTED  (CAM_FOCUS_RECT_BASE + 3)

#define OEMCAMERA_FILE_SIZE_DISABLED  	0
#define OEMCAMERA_FILE_SIZE_MIN_VALUE 	0
#define OEMCAMERA_FILE_SIZE_MAX_VALUE 	4000

// The quality param is a percent from 1 to 100
#define OEMCAMERA_QUALITY_MIN_VALUE 	1
#define OEMCAMERA_QUALITY_MAX_VALUE 	100
// Mapping from old Low, Med, and High enums is: 50, 75 and 90
// percent quality levels respectively
#define OEMCAMERA_QUALITY_LOW       	50
#define OEMCAMERA_QUALITY_MED      	 	75
#define OEMCAMERA_QUALITY_HIGH      	90

// Camcorder uses the same parm but a different mapping
#define OEMCAMCORDER_QUALITY_MIN_VALUE 0
#define OEMCAMCORDER_QUALITY_LOW       (OEMCAMCORDER_QUALITY_MIN_VALUE)
#define OEMCAMCORDER_QUALITY_MED       (OEMCAMCORDER_QUALITY_MIN_VALUE + 1)
#define OEMCAMCORDER_QUALITY_HIGH      (OEMCAMCORDER_QUALITY_MIN_VALUE + 2)
#define OEMCAMCORDER_QUALITY_USER      (OEMCAMCORDER_QUALITY_MIN_VALUE + 3)
#define OEMCAMCORDER_QUALITY_MAX       (OEMCAMCORDER_QUALITY_MIN_VALUE + 4)
#define OEMCAMCORDER_QUALITY_DYNAMIC   (OEMCAMCORDER_QUALITY_MIN_VALUE + 5)
#define OEMCAMCORDER_QUALITY_MAX_VALUE (OEMCAMCORDER_QUALITY_DYNAMIC)


typedef struct OEMCamera
{
	void *            m_pUser;

	// Camera mode.
	int16             m_nMode;
	int16             m_nPreviewType;
	// Media data.
	char*             m_pszFile;
	int16             m_nQuality;
	int32             m_nFileSize;
    IFileMgr*         m_pFileMgr;
}OEMCamera;

//-------------------------------------------------------------------
//      Macros
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//     Type Declarations
//-------------------------------------------------------------------
/*==================================================================

  Frame IBitmap

==================================================================*/
typedef struct CameraConfigInfo
{
   uint16   wNotifyCount;
} CameraConfigInfo;
//---------------------------------------------------------------------------
//    AEE-OEM Function Declarations
//---------------------------------------------------------------------------
extern void    OEMCameraEx_Init(void);
extern int     OEMCameraEx_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph);
extern void    OEMCameraEx_Delete(OEMINSTANCE h);

extern int     OEMCameraEx_QueryInterface(OEMINSTANCE h, AEECLSID idReq, void ** ppo);
extern int     OEMCameraEx_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2);
extern int     OEMCameraEx_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2);
extern int     OEMCameraEx_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam);
extern int     OEMCameraEx_Stop(OEMINSTANCE h);
extern int     OEMCameraEx_Update(OEMINSTANCE h, uint32 dwParam);

extern int     OEMCameraEx_EncodeSnapshot(OEMINSTANCE h);


//---------------------------------------------------------------------------
//    User Mode AEE-OEM Function Declarations
//---------------------------------------------------------------------------
extern int     OEMCameraEx_GetConfigInfo(CameraConfigInfo * pInfo);
//---------------------------------------------------------------------------
// AEE Functions used in OEM Layer.
// AEECamera_Init(): OEM ModTable entry. Called, during BREW initialization,
//                   in system mode.
// AEECamera_New():  OEM ModTable entry. Called, during ISHELL_CreateInstance(),
//                   in user mode.
//---------------------------------------------------------------------------
extern void    AEECameraEx_Init(IShell * ps);
extern int     AEECameraEx_New(IShell * ps, AEECLSID cls, void **ppif);
#endif // #ifndef OEMCAMERA_H
