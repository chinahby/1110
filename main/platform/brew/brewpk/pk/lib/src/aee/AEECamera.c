/*===========================================================================

FILE: OEMCamera.c

SERVICES: ICamera interface implementation

DESCRIPTION
   This file implements BREW ICamera. ICamera controls the device camera
   and allows recording of snapshot or movie.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_CAMERA)
#include "AEE.h"
#include "AEEBitmap.h"
#include "AEE_OEM.h"
#include "AEECamera.h"
#include "OEMCamera.h"
#include "OEMClassIDs.h"
#include "OEMCameraDMSS.h"

#include "aeestdlib.h"
#include "oemcamera.h"

#ifndef VC0848_CAM_DBG
struct ICamera
{
    VTBL(ICamera)    *pvt;
    int32             m_nRefs;
    OEMCamera        *m_pOEMCamera;
    IDIB             *m_pFrame;
};
#else
struct ICamera
{
   const AEEVTBL(ICamera) *pvt; //*m_pvt;

   AEECallback       m_cbSysObj;

   IShell *          m_pIShell;
   uint32            m_nRefs;
   OEMINSTANCE       m_hInstance;
   OEMCamera        *m_pOEMCamera;
   IDIB             *m_pFrame;

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
   //IBitmap *         m_pFrame;
   uint16            m_wFrameSize;
};

#endif 

// ModTable entry points...
extern void    AEECamera_Init(IShell * ps);
extern int     AEECamera_New(IShell * ps, AEECLSID cls, void **ppif);
extern int     AEECamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                                  void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop);
// Vtbl entry points...
static uint32  ICamera_AddRef(ICamera * pme);
static uint32  ICamera_Release(ICamera * pme);
static int     ICamera_QueryInterface(ICamera * pme, AEECLSID idReq, void ** ppo);
static int     ICamera_RegisterNotify(ICamera * pme, PFNCAMERANOTIFY pfnNotify, void * pUser);
static int     ICamera_SetParm(ICamera * pme, int16 nParmID, int32 p1, int32 p2);
static int     ICamera_GetParm(ICamera * pme, int16 nParmID, int32 * pP1, int32 * pP2);
static int     ICamera_Start(ICamera * pme, int16 nMode, uint32 dwParam);
static int     ICamera_Stop(ICamera * pme);
static int     ICamera_Pause(ICamera * pme, boolean bPause);
static int     ICamera_GetFrame(ICamera * po, IBitmap ** ppFrame);
static int     ICamera_EncodeSnapshot(ICamera * pme);

static AEEVTBL(ICamera) gvtICamera =
{
   ICamera_AddRef,
   ICamera_Release,
   ICamera_QueryInterface,
   ICamera_RegisterNotify,
   ICamera_SetParm,
   ICamera_GetParm,
   ICamera_Start,
   ICamera_Stop,
   ICamera_Pause,
   ICamera_GetFrame,
   ICamera_EncodeSnapshot
};

/*===============================================================================

                     ICamera Function Definitions

===============================================================================*/
/*==================================================================

==================================================================*/
void AEECamera_Init(IShell * ps)
{
   OEMCamera_Init();
}

/*==================================================================

==================================================================*/
int AEECamera_New(IShell * ps, AEECLSID cls, void **ppif)
{
    ICamera *   pme;
    int         nRet = SUCCESS;

    // Alloc memory for the object
    pme = (ICamera *)AEE_NewClass((IBaseVtbl *)&gvtICamera, sizeof(ICamera));
    if (!pme)
    {
        nRet = ENOMEMORY;
    }
    else
    {
        uint16 wSize;
        
        // Get Frame Bitmap
        nRet = OEMCamera_FrameBitmapNew((IBitmap **)&pme->m_pFrame, &wSize);
        if(nRet != SUCCESS)
        {
            goto Done;
        }
        
        nRet = OEMCamera_New( cls, 
                             (IBitmap *)pme->m_pFrame , 
                              wSize, 
                             (void **)&pme->m_pOEMCamera);
        if(nRet != SUCCESS)
        {
            (void)IBITMAP_Release((IBitmap *)pme->m_pFrame);
            goto Done;
        }
        
        pme->pvt = &gvtICamera;
        pme->m_nRefs = 1;
    }

    if (ppif)
    {
        *ppif = pme;
    }
    return nRet;

Done:
    if (ppif)
    {
        *ppif = NULL;
    }
    FREE(pme);
    return nRet;
}

/*==================================================================

==================================================================*/
static uint32 ICamera_AddRef(ICamera * pme)
{
    return (++(pme->m_nRefs));
}

/*==================================================================

==================================================================*/
static uint32 ICamera_Release(ICamera * pme)
{
    if(pme->m_nRefs <= 0)
    {
        return 0;
    }

    if (--pme->m_nRefs)
    {
        return pme->m_nRefs;
    }
    // Delete OEM object
    OEMCamera_Delete(pme->m_pOEMCamera);
    
    if (pme->m_pFrame)
    {
        IBITMAP_Release((IBitmap *)pme->m_pFrame);
    }

    FREE(pme);
    return(0);
}

/*==================================================================

==================================================================*/
static int ICamera_QueryInterface(ICamera * pme, AEECLSID idReq, void ** ppo)
{
    if (!ppo)
    {
        return EBADPARM;
    }

    if (NULL != IQI_SELF(pme, idReq, ppo, AEECLSID_CAMERA))
    {
        IQI_AddRef(*ppo);
        return SUCCESS;
    }
    else
        *ppo = NULL;

    // AddRef is not done...
    
    return *ppo ? SUCCESS : ECLASSNOTSUPPORT;
}

/*==================================================================

==================================================================*/
static int ICamera_RegisterNotify( ICamera         *pme, 
                                   PFNCAMERANOTIFY  pfnNotify, 
                                   void            *pUser)
{
#ifdef FEATURE_DSP
return SUCCESS;//OEMCameraEx_RegisterNotify(pme->m_pOEMCamera, pfnNotify, pUser);
#else
    return OEMCamera_RegisterNotify(pme->m_pOEMCamera, pfnNotify, pUser);
#endif
}

/*==================================================================

==================================================================*/
static int ICamera_SetParm(ICamera * pme, int16 nParmID, int32 p1, int32 p2)
{
    return OEMCamera_SetParm(pme->m_pOEMCamera, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int ICamera_GetParm(ICamera * pme, int16 nParmID, int32 * pP1, int32 * pP2)
{
    return OEMCamera_GetParm(pme->m_pOEMCamera, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int ICamera_Start(ICamera * pme, int16 nMode, uint32 dwParam)
{
    return OEMCamera_Start(pme->m_pOEMCamera, nMode, dwParam);
}

/*==================================================================

==================================================================*/
static int ICamera_Stop(ICamera * pme)
{
    return OEMCamera_Stop(pme->m_pOEMCamera);
}


/*==================================================================

==================================================================*/
static int ICamera_Pause(ICamera * pme, boolean bPause)
{
    return OEMCamera_Pause(pme->m_pOEMCamera, bPause);
}

/*==================================================================

==================================================================*/
static int ICamera_EncodeSnapshot(ICamera * pme)
{
    return OEMCamera_EncodeSnapshot(pme->m_pOEMCamera);
}

/*==================================================================

==================================================================*/
static int ICamera_GetFrame(ICamera * pme, IBitmap ** ppFrame)
{
    if (!ppFrame)
    {
        return EBADPARM;
    }
    
    if (!pme->m_pFrame || !pme->m_pFrame->pBmp)
    {
        *ppFrame = NULL;
        return EFAILED;
    }

    IBITMAP_AddRef((IBitmap *)pme->m_pFrame);
    *ppFrame = (IBitmap *)pme->m_pFrame;

    return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEECamera_FrameBlt(void * pDst, uint32 dwDstSize, int xDst, int yDst, int cxDst, int cyDst,
                       void * pSrc, uint32 dwSrcSize, int xSrc, int ySrc, AEERasterOp rop)
{
    return OEMCamera_FrameBlt(pDst, dwDstSize, xDst, yDst, cxDst, cyDst,
                              pSrc, dwSrcSize, xSrc, ySrc, rop);
}

#endif //FEATURE_BREW_CAMERA
