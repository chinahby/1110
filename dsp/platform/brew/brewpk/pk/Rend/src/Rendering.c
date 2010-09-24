//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering.c
//Author:       AnyLook
//Version:      1.0
//Date:      2009/01/09
//Description:
//    Rendering Src file
//Others: 
//Function List: 
//<Name>        <desc>
//Simple        
//History: 
//<author>      <time>      <version >  <desc>
// AnyLook    2009/01/09       1.0       build this moudle  
//**************************************************************
#include "Rendering.h"
#include "Rendering_OEM.h"
#include "Rendering_Type.h"
#include "AEEStdLib.h"

#define MSPF(fps)   (1000/fps)

typedef struct _RendDev{
    byte           *m_pBackBuf;
	byte           *m_pDestBuf;
	byte           *m_pSrcBuf;
    int             m_pSrcSize;
    int             m_dxSrc;
    int             m_dySrc;
    int             m_nPitch;
    int             m_nDepth;
    int             m_nScheme;
    RendType        m_eType;
    int             m_fps;
    int             m_iCurrStep;
    int             m_iMaxStep;
    PFNRENDDRAW     m_pfnRendDraw;
    boolean         m_isPlaying;
	boolean         m_isEnable;
    boolean         m_bPushed;
	boolean         m_bInited;
}RendDev;

int Rendering_Init(RENDHANDLE *hhDev)
{
    RendDev *pMe = (RendDev *)Rendering_Malloc(sizeof(RendDev));
	if(!pMe)
	{
		return 1;
	}
    
    pMe->m_pBackBuf     = NULL;
    pMe->m_pDestBuf     = NULL;
    pMe->m_pSrcBuf      = NULL;
    pMe->m_fps          = REND_FPS_DEFAULT;
    pMe->m_iCurrStep    = 0;
    pMe->m_iMaxStep     = REND_STEP_MAX;
    pMe->m_isPlaying    = FALSE;
	pMe->m_isEnable     = FALSE;
	pMe->m_pfnRendDraw  = NULL;
    pMe->m_bPushed      = FALSE;
    if(hhDev)
    {
        *hhDev = (RENDHANDLE)pMe;
    }
    else
    {
        Rendering_Done((RENDHANDLE)pMe);
        return 1;
    }
    return 0;
}

int Rendering_Done(RENDHANDLE hDev)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(pMe->m_pBackBuf)
    {
        Rendering_Free(pMe->m_pBackBuf);
        pMe->m_pBackBuf = NULL;
    }

    if(pMe->m_pDestBuf)
    {
        Rendering_Free(pMe->m_pDestBuf);
        pMe->m_pDestBuf = NULL;
    }
    
    Rendering_Free(pMe);
    return 0;
}

int Rendering_SetType(RENDHANDLE hDev, RendType eType)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(!Rendering_IsEnable(hDev))
    {
        return 1;
    }
    
    pMe->m_eType        = eType;
    return 0;
}

int Rendering_SetFPS(RENDHANDLE hDev, int nFPS)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(nFPS == 0)
    {
        nFPS = REND_FPS_DEFAULT;
    }
    pMe->m_fps= nFPS;
    return 0;
}

int Rendering_SetEnable(RENDHANDLE hDev, boolean bEnable)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(!bEnable)
    {
        (void)Rendering_Stop(hDev);
    }
    pMe->m_isEnable = bEnable;
	return 0;
}

int Rendering_SetSrcScreen(RENDHANDLE hDev, byte *pData, int nSize, int dx, int dy, int nPitch, int nDepth, int nScheme)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(!Rendering_IsEnable(hDev))
    {
        return 1;
    }

    if(nSize > pMe->m_pSrcSize)
    {
        if(pMe->m_pBackBuf)
        {
            Rendering_Free(pMe->m_pBackBuf);
            pMe->m_pBackBuf = NULL;
        }

        if(pMe->m_pDestBuf)
        {
            Rendering_Free(pMe->m_pDestBuf);
            pMe->m_pDestBuf = NULL;
        }
    }
    
    pMe->m_pSrcBuf  = pData;
    pMe->m_pSrcSize = nSize;
    pMe->m_dxSrc    = dx;
    pMe->m_dySrc    = dy;
    pMe->m_nPitch   = nPitch;
    pMe->m_nDepth   = nDepth;
    pMe->m_nScheme  = nScheme;

    if(!pMe->m_pBackBuf)
    {
        pMe->m_pBackBuf = Rendering_Malloc(nSize);
    }

    if(!pMe->m_pDestBuf)
    {
        pMe->m_pDestBuf = Rendering_Malloc(nSize);
    }
    return 0;
}

int Rendering_PushScreen(RENDHANDLE hDev)
{
    RendDev *pMe = (RendDev *)hDev;
	void *pDest = pMe->m_pBackBuf;
    void *pSrc  = pMe->m_pSrcBuf;

    if(!pMe)
    {
        return 1;
    }
    
    if(!Rendering_IsEnable(hDev) || pMe->m_pBackBuf == NULL)
    {
        return 1;
    }
    pMe->m_bPushed      = TRUE;
    (void)REND_MEMCPY32(pDest, pSrc, pMe->m_pSrcSize);
    return 0;
}

int Rendering_Start(RENDHANDLE hDev, int nDelay)
{
    RendDev *pMe = (RendDev *)hDev;
	RendType currType;
    
    if(!pMe)
    {
        return 1;
    }
    
	if(!Rendering_IsEnable(hDev)||!pMe->m_bPushed)
    {
        return 1;
    }
    
    if(pMe->m_isPlaying)
    {
        return 1;
    }
    
    pMe->m_iCurrStep = 0;
    currType = pMe->m_eType;
    
    pMe->m_pfnRendDraw = Rendering_GetRendDraw16(currType);
    if(pMe->m_pfnRendDraw && pMe->m_pBackBuf && pMe->m_pDestBuf)
    {
	    pMe->m_isPlaying = TRUE;
		Rendering_StartTimer(MSPF(pMe->m_fps)+nDelay, pMe);
    }
    return 0;
}

int Rendering_Stop(RENDHANDLE hDev)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
    if(!Rendering_IsEnable(hDev))
    {
        return 1;
    }
    
    if(pMe->m_isPlaying)
    {
        Rendering_CancelTemer(pMe);
		Rendering_UpdateDev(pMe->m_pSrcBuf, 
                            pMe->m_pSrcSize, 
                            pMe->m_dxSrc, 
                            pMe->m_dySrc, 
                            pMe->m_nPitch,
                            pMe->m_nDepth,
                            pMe->m_nScheme);
        pMe->m_isPlaying    = FALSE;
        pMe->m_pfnRendDraw  = NULL;
        pMe->m_bPushed      = FALSE;
    }
    return 0;
}

boolean Rendering_IsPlaying(RENDHANDLE hDev)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
	return pMe->m_isPlaying;
}

boolean Rendering_IsEnable(RENDHANDLE hDev)
{
    RendDev *pMe = (RendDev *)hDev;
    
    if(!pMe)
    {
        return 1;
    }
    
	return pMe->m_isEnable && Rendering_DevAvaild();
}

void Rendering_TimerCB(void *pUser)
{
    RendDev *pMe = (RendDev *)pUser;
    
    pMe->m_iCurrStep++;
    if(pMe->m_iCurrStep > pMe->m_iMaxStep)
    {
		Rendering_Stop((RENDHANDLE)pMe);
        return;
    }
    
    if(pMe->m_pfnRendDraw)
    {
		RendDraw myRendDraw;
        
		Rendering_StartTimer(MSPF(pMe->m_fps), pUser);
		
		myRendDraw.pDest      = pMe->m_pDestBuf;
		myRendDraw.pSrc1      = pMe->m_pSrcBuf;
		myRendDraw.pSrc2      = pMe->m_pBackBuf;
		myRendDraw.wWidthSrc  = pMe->m_dxSrc;
		myRendDraw.wHeightSrc = pMe->m_dySrc;
		myRendDraw.nDepth     = pMe->m_nDepth;
        
        pMe->m_pfnRendDraw(&myRendDraw,
                            pMe->m_iCurrStep,
                            pMe->m_iMaxStep
                           );
        
		Rendering_UpdateDev(pMe->m_pDestBuf, 
                            pMe->m_pSrcSize, 
                            pMe->m_dxSrc, 
                            pMe->m_dySrc, 
                            pMe->m_nPitch,
                            pMe->m_nDepth,
                            pMe->m_nScheme);
    }
    else
    {
        Rendering_Stop((RENDHANDLE)pMe);
    }
}
