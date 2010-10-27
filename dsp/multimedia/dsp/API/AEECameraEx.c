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

#include "AEE.h"
#include "AEE_OEM.h"
#include "AEECameraEx.h"
#include "OEMCameraEx.h"
#include "OEMClassIDs.h"

#include "aeestdlib.h"

struct ICamera
{
    VTBL(ICamera)    *pvt;
    int32             m_nRefs;
    OEMCamera        *m_pOEMCamera;
};

// ModTable entry points...
extern void    AEECamera_Init(IShell * ps);
extern int     AEECamera_New(IShell * ps, AEECLSID cls, void **ppif);
// Vtbl entry points...
static uint32  ICamera_AddRef(ICamera * pme);
static uint32  ICamera_Release(ICamera * pme);
static int     ICamera_QueryInterface(ICamera * pme, AEECLSID idReq, void ** ppo);
static int     ICamera_SetParm(ICamera * pme, int16 nParmID, int32 p1, int32 p2);
static int     ICamera_GetParm(ICamera * pme, int16 nParmID, int32 * pP1, int32 * pP2);
static int     ICamera_Start(ICamera * pme, int16 nMode, uint32 dwParam);
static int     ICamera_Stop(ICamera * pme);
static int     ICamera_EncodeSnapshot(ICamera * pme);
static int     ICamera_RegisterNotify(ICamera * pme, PFNCAMERANOTIFY pfnNotify, void * pUser);


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
    pme = (ICamera *)AEE_OEM_NEWCLASS((IBaseVtbl *)&gvtICamera, sizeof(ICamera));
    if (!pme)
    {
        nRet = ENOMEMORY;
    }
    else
    {
        uint16 wSize;
        
        // Get Frame Bitmap        
        nRet = OEMCamera_New( cls, 
                             NULL, 
                              wSize, 
                             (void **)&pme->m_pOEMCamera);
        
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

static int ICamera_RegisterNotify( ICamera         *pme, 
                                   PFNCAMERANOTIFY  pfnNotify, 
                                   void            *pUser)
{
    return SUCCESS;//OEMCamera_RegisterNotify(pme->m_pOEMCamera, pfnNotify, pUser);
}

