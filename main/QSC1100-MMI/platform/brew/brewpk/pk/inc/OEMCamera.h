#ifndef OEMCAMERA_H
#define OEMCAMERA_H
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
#include "AEECamera.h"
#include "AEEClassIDs.h"
#include "AEECamera.bid"

#define CAM_FOCUS_RECT_BASE            1                       // Base used by ICamera
#define CAM_FOCUS_RECT_USER_BASE       0x1000                  // Base used for extension

#define CAM_FOCUS_RECT_AUTO             (CAM_FOCUS_RECT_BASE)       
#define CAM_FOCUS_RECT_SPOT             (CAM_FOCUS_RECT_BASE + 1)   
#define CAM_FOCUS_RECT_AVERAGE          (CAM_FOCUS_RECT_BASE + 2)   
#define CAM_FOCUS_RECT_CENTER_WEIGHTED  (CAM_FOCUS_RECT_BASE + 3)

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

/* This stuff is really in AEECamera.c. It doesn't really belong here, but it
   is for now. */
typedef struct CameraCallback
{
   AEECallback       cb;
   AEECameraNotify   camNotify;
   flg               bInUse:1;
} CameraCallback;

#ifdef VC0848_CAM_DBG
struct ICamera
{
   const AEEVTBL(ICamera) *m_pvt;

   AEECallback       m_cbSysObj;

   IShell *          m_pIShell;
   uint32            m_nRefs;
   OEMINSTANCE       m_hInstance;

   ACONTEXT *        m_pac;
   AEECallback       m_cbExit;

   // User registered callback info.
   PFNCAMERANOTIFY   m_pfnNotify;
   void *            m_pUser;

   CameraCallback *  m_pcbList;
   uint16            m_wCBCount;

   // Media data.
   AEEMediaData      m_md;
   char *            m_pszMIME;

   // Captured frame
   IBitmap *         m_pFrame;
   uint16            m_wFrameSize;
};
#endif // VC0848_CAM_DBG
//---------------------------------------------------------------------------
//    AEE-OEM Function Declarations
//---------------------------------------------------------------------------
extern void    OEMCamera_Init(void);
extern int     OEMCamera_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph);
extern void    OEMCamera_Delete(OEMINSTANCE h);

extern int     OEMCamera_QueryInterface(OEMINSTANCE h, AEECLSID idReq, void ** ppo);
extern int     OEMCamera_RegisterNotify(OEMINSTANCE h, PFNCAMERANOTIFY pfnNotify, void * pUser);
extern int     OEMCamera_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2);
extern int     OEMCamera_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2);
extern int     OEMCamera_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam);
extern int     OEMCamera_Stop(OEMINSTANCE h);
extern int     OEMCamera_Pause(OEMINSTANCE h, boolean bPause);
extern int     OEMCamera_EncodeSnapshot(OEMINSTANCE h);

extern void    OEMCamera_ReleaseFrame(OEMINSTANCE h);
extern int     OEMCamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                                  void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop);

//---------------------------------------------------------------------------
//    User Mode AEE-OEM Function Declarations
//---------------------------------------------------------------------------
extern int     OEMCamera_GetConfigInfo(CameraConfigInfo * pInfo);
extern int     OEMCamera_FrameBitmapNew(IBitmap ** ppFrame, uint16 * pwSize);
extern boolean OEMCamera_GetFrameTrigger(IBitmap * pFrame);
#ifdef FEATURE_CAMERA7500
extern int     OEMCamera_GetMultipleFocusRect(int32* pRc, AEERect **pInfo);
#endif
//---------------------------------------------------------------------------
// AEE Functions used in OEM Layer.
// AEECamera_Init(): OEM ModTable entry. Called, during BREW initialization,
//                   in system mode.
// AEECamera_New():  OEM ModTable entry. Called, during ISHELL_CreateInstance(),
//                   in user mode.
//---------------------------------------------------------------------------
extern void    AEECamera_Init(IShell * ps);
extern int     AEECamera_New(IShell * ps, AEECLSID cls, void **ppif);

// Calls OEMCamera_FrameBlt() in system mode.
extern int     AEECamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                                  void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop);

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

CameraConfigInfo

Description:
   This structure specifies AEECamera the number of pending callback
   events to be buffered before delivering to the app.

Definition:
   typedef struct CameraConfigInfo
   {
      uint16   wNotifyCount;
   } CameraConfigInfo;

Members:
   wNotifyCount: number of pending callback events to be buffered

Comments:
   By default, AEE buffers five events.

See Also:
   OEMCamera_GetConfigInfo()

=======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
OEMCamera Interface

   ICamera implementation in AEE layer uses the interface defined in
   this file to perform all the ICamera operations.

=============================================================================

OEMCamera_Init()

Description:
   This function allows OEMs to perform one-time initialization for the camera
   when BREW starts up.

Prototype:
   void OEMCamera_Init(void);

Parameters:
   None.

Return Value:
   None

Comments:
   None

See Also:
   None
=============================================================================

OEMCamera_New()

Description:
   This function creates the OEM camera object that is refered by AEE ICamera
   object.

Prototype:
   int OEMCamera_New(AEECLSID cls, IBitmap * pFrame, uint16 wSize, OEMINSTANCE * ph);

Parameters:
   cls: Class ID
   pFrame: IBitmap representing the camera frame
   wSize: Size, in bytes, of the camera frame
   ph [out]: Pointer to the instance handle to allocated to OEM camera object

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   pFrame must be saved and used by OEM camera object

See Also:

=============================================================================

OEMCamera_Delete()

Description:
   This function deletes the camera object in OEM layer.

Prototype:
   void OEMCamera_Delete(OEMINSTANCE h);

Parameters:
   h: Instance handle

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   OEM camera object must be deleted.

See Also:
   OEMCamera_Delete()

=============================================================================

OEMCamera_QueryInterface()

Description:
    This function can be used to
     -    Get a pointer to an interface or data based on the input class ID
     -    Query an extended version of the ICamera-derived class
     -    Support version compatibility

Prototype:
    int OEMCAMERA_QueryInterface
    (
    OEMCamera h, 
    AEECLSID clsReq, 
    void ** ppo
    )

Parameters:
      h [in]: instance handle
    clsReq:  [in]:  A globally unique id to identify the entity (interface or
data) that we are trying to query.
    ppo:  [out]:  Pointer to the interface or data that we want to retrieve. If
the value passed back is NULL, the interface or data that we query are not available.

Return Value:
    SUCCESS: on success, 
    Otherwise: an error code.

Comments:
    If ppo is back a NULL pointer, the interface or data that we query is not available.

Side Effects:
    If an interface is retrieved, then this function increments its reference count.

    If a data structure is retrieved, then a pointer to the internal structure is
given and user should not free it.

Version:
   Introduced BREW Client 3.1.4

See Also:
    None

=============================================================================
OEMCamera_RegisterNotify()

Description:
   This function registers the AEE ICamera callback that will be called when
   OEM layer camera object needs to send an event.

Prototype:
   int OEMCamera_RegisterNotify(OEMINSTANCE h, PFNCAMERANOTIFY pfnNotify, void * pUser);

Parameters:
   h: instance handle
   pfnNotify: callback notification function
   pUser: user data

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   None

See Also:
   None

=============================================================================

OEMCamera_SetParm()

Description:
   This function sets the camera control parameters.

Prototype:
   int OEMCamera_SetParm(OEMINSTANCE h, int16 nParmID, int32 p1, int32 p2);

Parameters:
   h: instance handle
   nParmID: CAM_PARM_XXX
   p1: Depends on the parameter.
   p2: Depends on the parameter.

Return Value:
   SUCCESS:    successful. Operation is completed.
   CAM_PENDING: result returned via the registered callback
   EBADPARM:   bad parm
   ENOMEMORY: not enough memory
   EBADSTATE:  Cannot set the parm in the current state.
   EUNSUPPORTED: parm not supported by the class

Comments:
    See the "Camera Control Parameters" data structure section in ICamera documentation.

See Also:
   None

=============================================================================

OEMCamera_GetParm()

Description:
   This function gets the camera control parameters.

Prototype:
   int OEMCamera_GetParm(OEMINSTANCE h, int16 nParmID, int32 * pP1, int32 * pP2);

Parameters:
   h: instance handle
   nParmID: CAM_PARM_XXX
   pP1: Depends on the parameter.
   pP2: Depends on parameter.

Return Value:
   SUCCESS:    successful. The operation is completed.
   CAM_PENDING: value returned via the registered callback
   EBADPARM:   bad parm
   ENOMEMORY: not enough memory
   EBADSTATE:  Cannot get parm in the current state.
   EUNSUPPORTED: Parm not supported by the class

Comments:
    See the "Camera Control Parameters" data structure section in ICamera documentation.

See Also:
   None

=============================================================================

OEMCamera_GetParm()

Description:
   This function starts the camera operation in preview/snapshot/movie mode.

Prototype:
   int OEMCamera_Start(OEMINSTANCE h, int16 nMode, uint32 dwParam);

Parameters:
   h: instance handle
   nMode:  CAM_MODE_PREVIEW/CAM_MODE_SNAPSHOT/CAM_MODE_MOVIE
   dwParam: reserved

Return Value:
   SUCCESS: command accepted
   EFAILED: general failure
   EBADPARM: Bad parm is passed.
   ENOMEMORY: not enough memory
   EBADSTATE: Start cannot be done in the current state.

Comments:
   In the callback, AEECameraNotify, nCmd = CAM_CMD_START and nSubCmd = nMode.

See Also:
   OEMCamera_Stop()
   OEMCamera_Pause()

=============================================================================

OEMCamera_Stop()

Description:
   This function stops the current camera operation and puts it in Ready state.

Prototype:
   int OEMCamera_Stop(OEMINSTANCE h);

Parameters:
   h: instance handle

Return Value:
   SUCCESS: command accepted
   EFAILED: general failure
   ENOMEMORY: not enough memory
   EBADSTATE: Stop cannot be done in current state.

Comments:
   This function results in CAM_STATUS_DONE status callback.
   In the callback, AEECameraNotify, nCmd = CAM_CMD_START and nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_MOVIE.

See Also:
   None
=============================================================================

OEMCamera_Pause()

Description:
   This function pauses/resumes the camera operation. In preview and record modes,
   the frame callbacks are paused/resumed. In record mode, the encoding is also paused/resumed.

Prototype:
   int OEMCamera_Pause(OEMINSTANCE h, boolean bPause);

Parameters:
   h: instance handle
   bPause: TRUE/FALSE => Pause/Resume

Return Value:
   SUCCESS: command accepted
   EFAILED: general failure
   ENOMEMORY: got enough memory
   EBADSTATE: Resume cannot be done in the current state.

Comments:
   This API does not apply to Snapshot mode.

   This function results in a CAM_STATUS_RESUME status callback.
   In the callback, AEECameraNotify, nCmd = CAM_CMD_START and nSubCmd = CAM_MODE_PREVIEW/CAM_MODE_MOVIE.

See Also:
   OEMCamera_Start()

=============================================================================

OEMCamera_EncodeSnapshot()

Description:
   This function encodes the latest raw snapshot frame.

Prototype:
   int OEMCamera_EncodeSnapshot(OEMINSTANCE h);

Parameters:
   h: instance handle

Return Value:
   SUCCESS if successful
   Error code if failure

Comments:
   This function results in {CAM_CMD_ENCODESNAPSHOT, CAM_STATUS_DONE} callback after the
   snapshot is encoded.

   This API can be called in any mode. It may abort the current active operation like preview, movie,
   encode snapshot, and so forth. It may also be used to encode any frame, if supported.

See Also:
   OEMCamera_Start()
=============================================================================

OEMCamera_ReleaseFrame()

Description:
   This function informs OEM layer to release the current frame.

Prototype:
   void OEMCamera_ReleaseFrame(OEMINSTANCE h);

Parameters:
   h: instance handle

Return Value:
   None

Comments:
   This function is called after the frame callback is processed by
   the application.

See Also:
   None
=============================================================================

OEMCamera_FrameBlt()

Description:
   This function blts the camera frame to the specified destination.

Prototype:
   int OEMCamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                          void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop);

Parameters:
   pDst: destination object
   dwDstSize: size, in bytes, of destination object
   xDst: x pos of destination
   yDst: y pos of destination
   cxDst: size of destination along x
   cyDst: size of destination along y
   pSrc: source object
   dwSrcSize: size, in bytes, of the source object
   xSrc: x pos of the source
   ySrc: y pos of the source
   rop: raster operation

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   This function is called in response to AEECamera_FrameBlt().

See Also:
   AEECamera_FrameBlt().
=============================================================================

OEMCamera_GetConfigInfo()

Description:
   This function is called to by AEE ICamera object to retrieve the
   initial config info.

Prototype:
   int OEMCamera_GetConfigInfo(CameraConfigInfo * pInfo);

Parameters:
   pInfo: camera config info used by AEE ICamera object

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   None.

See Also:
   None.
=============================================================================

OEMCamera_FrameBitmapNew()

Description:
   This function is called by AEE ICamera object to create a new frame bitmap.

Prototype:
   int OEMCamera_FrameBitmapNew(IBitmap ** ppFrame, uint16 * pwSize);

Parameters:
   ppFrame [out]: camera frame
   pwSize [out]: size, in bytes, of the frame

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   None

See Also:
   OEMCamera_GetFrameTrigger()
=============================================================================

OEMCamera_GetFrameTrigger()

Description:
   This function is called from AEECamera within ICAMERA_GetFrame() API. OEM can do 
   custom processing on the frame to get it ready for the caller of ICAMERA_GetFrame().

Prototype:
   boolean OEMCamera_GetFrameTrigger(IBitmap * pFrame);

Parameters:
   pFrame [in]: camera frame

Return Value:
   TRUE if OEM does custom processing
   FALSE if this function is no-op

Comments:
   None

See Also:
   OEMCamera_FrameBitmapNew()
===============================================================================

OEMCamera_GetParm()

Description:
   OEM Mod table entry for ICamera create instance.

Prototype:
   int AEECamera_New(IShell * ps, AEECLSID cls, void **ppif);

Parameters:
   ps: Pointer to IShell
   cls: Class ID
   ppif [out]: Pointer to created object

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   None.

See Also:
   None.

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
AEECamera Interface

=======================================================================

AEECamera_Init()

Description:
   OEM Mod table entry for ICamera initialization.

Prototype:
   void AEECamera_Init(IShell * ps);

Parameters:
   ps: pointer to IShell

Return Value:
   None.

Comments:
   None

See Also:
  OEMCamera Interface Documentation
=========================================================================

AEECamera_FrameBlt()

Description:
   This function blts the camera frame to the specified destination.

Prototype:
   int AEECamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                          void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop);

Parameters:
   pDst: destination object
   dwDstSize: size, in bytes, of the destination object
   xDst: x pos of the destination
   yDst: y pos of the destination
   cxDst: size of the destination along x
   cyDst: size of the destination along y
   pSrc: source object
   dwSrcSize: size, in bytes, of the source object
   xSrc: x pos of the source
   ySrc: y pos of the source
   rop: Raster operation

Return Value:
   SUCCESS if successful
   Error code if error

Comments:
   None

See Also:
   None
==============================================================
AEECamera


See the ICamera Interface in the BREW API Reference.


=============================================================================*/
#endif // #ifndef OEMCAMERA_H
