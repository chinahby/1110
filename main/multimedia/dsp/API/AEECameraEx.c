/*===========================================================================

FILE: OEMCamera.c

SERVICES: ICameraEx interface implementation

DESCRIPTION
   This file implements BREW ICameraEx. ICameraEx controls the device camera
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

struct ICameraEx
{
    VTBL(ICameraEx)    *pvt;
    int32             m_nRefs;
    OEMCamera        *m_pOEMCamera;
};

// ModTable entry points...
 extern void    AEECameraEx_Init(IShell * ps);
extern int   AEECameraEx_New(IShell * ps, AEECLSID cls, void **ppif);

// Vtbl entry points...
static uint32  ICameraEx_AddRef(ICameraEx * pme);
static uint32  ICameraEx_Release(ICameraEx * pme);
static int     ICameraEx_QueryInterface(ICameraEx * pme, AEECLSID idReq, void ** ppo);
static int     ICameraEx_SetParm(ICameraEx * pme, int16 nParmID, int32 p1, int32 p2);
static int     ICameraEx_GetParm(ICameraEx * pme, int16 nParmID, int32 * pP1, int32 * pP2);
static int     ICameraEx_Start(ICameraEx * pme, int16 nMode, uint32 dwParam);
static int     ICameraEx_Stop(ICameraEx * pme);
static int     ICameraEx_EncodeSnapshot(ICameraEx * pme);
static int     ICameraEx_RegisterNotify(ICameraEx * pme, PFNCAMERANOTIFY pfnNotify, void * pUser);


static AEEVTBL(ICameraEx) gvtICameraEx =
{
   ICameraEx_AddRef,
   ICameraEx_Release,
   ICameraEx_QueryInterface,
   ICameraEx_RegisterNotify,
   ICameraEx_SetParm,
   ICameraEx_GetParm,
   ICameraEx_Start,
   ICameraEx_Stop,
   ICameraEx_EncodeSnapshot
};

/*===============================================================================

                     ICameraEx Function Definitions

===============================================================================*/
/*==================================================================

==================================================================*/
void AEECameraEx_Init(IShell * ps)
{
  OEMCameraEx_Init();
}

/*==================================================================

==================================================================*/
int AEECameraEx_New(IShell* ps, AEECLSID cls, void **ppif)
{
    ICameraEx *   pme;
    int         nRet = SUCCESS;
    DBGPRINTF("aaaaaaaaaaaaaaaaaaa-----AEECameraEx_New");

    // Alloc memory for the object
    pme = (ICameraEx *)AEE_NewClass((IBaseVtbl *)&gvtICameraEx, sizeof(ICameraEx));
    if (!pme)
    {
        nRet = ENOMEMORY;
    }
    else
    {
        uint16 wSize = 0;
        
        // Get Frame Bitmap        
        nRet = OEMCameraEx_New( cls, 
                             NULL, 
                              wSize, 
                             (void **)&pme->m_pOEMCamera);
        
        pme->pvt = &gvtICameraEx;
        pme->m_nRefs = 1;
        DBGPRINTF("ddddddddddddddddddd-----AEECamera_New");
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
static uint32 ICameraEx_AddRef(ICameraEx * pme)
{
    return (++(pme->m_nRefs));
}

/*==================================================================

==================================================================*/
static uint32 ICameraEx_Release(ICameraEx * pme)
{
    if(pme->m_nRefs <= 0)
    {
        return 0;
    }

    if (--pme->m_nRefs)
    {
        return pme->m_nRefs;
    }

    OEMCameraEx_Delete(pme->m_pOEMCamera);

    FREE(pme);
    return(0);
}

/*==================================================================

==================================================================*/
static int ICameraEx_QueryInterface(ICameraEx * pme, AEECLSID idReq, void ** ppo)
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
static int ICameraEx_SetParm(ICameraEx * pme, int16 nParmID, int32 p1, int32 p2)
{
    return OEMCameraEx_SetParm(pme->m_pOEMCamera, nParmID, p1, p2);
}

/*==================================================================

==================================================================*/
static int ICameraEx_GetParm(ICameraEx * pme, int16 nParmID, int32 * pP1, int32 * pP2)
{
    return OEMCameraEx_GetParm(pme->m_pOEMCamera, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int ICameraEx_Start(ICameraEx * pme, int16 nMode, uint32 dwParam)
{
    return OEMCameraEx_Start(pme->m_pOEMCamera, nMode, dwParam);
}

/*==================================================================

==================================================================*/
static int ICameraEx_Stop(ICameraEx * pme)
{
    return OEMCameraEx_Stop(pme->m_pOEMCamera);
}


/*==================================================================

==================================================================*/
static int ICameraEx_Pause(ICameraEx * pme, boolean bPause)
{
    return 0;//OEMCameraEx_Pause(pme->m_pOEMCamera, bPause);
}

/*==================================================================

==================================================================*/
static int ICameraEx_EncodeSnapshot(ICameraEx * pme)
{
    return OEMCameraEx_EncodeSnapshot(pme->m_pOEMCamera);
}

static int ICameraEx_RegisterNotify( ICameraEx         *pme, 
                                   PFNCAMERANOTIFY  pfnNotify, 
                                   void            *pUser)
{
    return SUCCESS;//OEMCameraEx_RegisterNotify(pme->m_pOEMCamera, pfnNotify, pUser);
}

