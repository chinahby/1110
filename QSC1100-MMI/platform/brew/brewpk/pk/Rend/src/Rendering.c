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

#include "OEMCFGI.h"//wlh 20090405 add 
#include "AEEStdLib.h"

#define REND_TYPE_TEST REND_SHOW_LEFTRIGHT_OUT//REND_ROTAT_LEFTTOP
#define FPS_DEFAULT 10
#define STEP_MAX    10
#define BUF_MAX     2
#define MSPF(fps)   (100/fps)


typedef struct _RendDev{
    byte        m_pBackBuf[REND_BUF_SIZE];
	byte        m_pDestBuf[REND_BUF_SIZE];
	byte       *m_pSrcBuf;
    RendType    m_eType;
    DisplayDev  m_eDev;
    int         m_fps;
    int         m_iCurrStep;
    int         m_iMaxStep;
    boolean     m_isDrawing;
	boolean     m_isEnable;
	boolean     m_isRun;//wlh 20090409 add
    PFNRENDDRAW m_pfnRendDraw;
	FlexDirection         m_iFlexDirection;//wlh 20090406 add for flex direction
}RendDev;

static RendDev g_RendDev[DISPLAYDEV_MAX];

///////////////////////////////////////////////////////////////////////////////////////////////////
static int Rendering_PushScreen(DisplayDev eDev);
static int Rendering_UpdateScreen(DisplayDev eDev, int16 x, int16 y, int16 dx, int16 dy);

static int Rendering_Start(DisplayDev eDev);
static int Rendering_Stop(DisplayDev eDev);

static boolean Rendering_IsEnable(DisplayDev eDev);
static boolean Rendering_IsPlaying(DisplayDev eDev);
///////////////////////////////////////////////////////////////////////////////////////////////////

int Rendering_Initlize(DisplayDev eDev, void *pSrcBuf)
{
    DBGPRINTF("Rendering_Initlize");
    g_RendDev[eDev].m_pSrcBuf      = (byte *)pSrcBuf;
    g_RendDev[eDev].m_eType        = REND_RANDOM;
    g_RendDev[eDev].m_fps          = FPS_DEFAULT;
    g_RendDev[eDev].m_iCurrStep    = 0;
    g_RendDev[eDev].m_iMaxStep     = STEP_MAX;
    g_RendDev[eDev].m_isDrawing    = FALSE;
	g_RendDev[eDev].m_isEnable     = FALSE;
    g_RendDev[eDev].m_eDev         = eDev;
	g_RendDev[eDev].m_pfnRendDraw  = NULL;
    return 0;
}

int Rendering_Release(DisplayDev eDev)
{
    return 0;
}

int Rendering_SetType(DisplayDev eDev, RendType eType, int16 xDst, int16 yDst, int16 yStart)
{
    g_RendDev[eDev].m_eType        = eType;
    return 0;
}

int Rendering_SetEnable(DisplayDev eDev, boolean bEnable)
{
	g_RendDev[eDev].m_isEnable = bEnable;
	return 0;
}
int Rendering_Update(DisplayDev eDev, int16 x, int16 y, int16 dx, int16 dy)
{
	boolean bFullScreen = (x==0 && y==0 && dx==REND_SCREEN_WIDTH && dy==REND_SCREEN_HEIGHT);
	if(bFullScreen)
	{
		if(!Rendering_IsPlaying(eDev))
		{
			if(Rendering_IsEnable(eDev))
			{
			    DBGPRINTF("Rendering_Update 1");
				return Rendering_Start(eDev);
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(!Rendering_IsPlaying(eDev))
		{
			(void)Rendering_UpdateScreen(eDev, x, y, dx, dy);
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int Rendering_UpdateEx(void)
{
/*
	Rendering_PushScreen(DISPLAYDEV_MAIN);
	if(!Rendering_IsPlaying(DISPLAYDEV_MAIN))
	{
		if(Rendering_IsEnable(DISPLAYDEV_MAIN))
		{
		    DBGPRINTF("Rendering_UpdateEx 1");
			return Rendering_Start(DISPLAYDEV_MAIN);
		}
	}
	else
	{
		return 0;
	}

	return 1;*/
}
int Rendering_PushScreen(DisplayDev eDev)
{
    void *pDest = g_RendDev[eDev].m_pBackBuf;
    void *pSrc  = g_RendDev[eDev].m_pSrcBuf;
    (void)REND_MEMCPY(pDest, pSrc, REND_BUF_SIZE);
    return 0;
}


int Rendering_UpdateScreen(DisplayDev eDev, int16 x, int16 y, int16 dx, int16 dy)
{
	byte *pDest = (byte *)g_RendDev[eDev].m_pBackBuf;
    byte *pSrc  = (byte *)g_RendDev[eDev].m_pSrcBuf;
	int   i,nSize,nPitch;

	nPitch = REND_SCREEN_WIDTH*REND_BPP;
	pDest += y*nPitch + x*REND_BPP;
	pSrc  += y*nPitch + x*REND_BPP;
	nSize  = dx*REND_BPP;
	
	for(i=0;i<dy;i++)
	{
		REND_MEMCPY(pDest, pSrc, nSize);
		pDest += nPitch;
		pSrc  += nPitch;
	}
    return 0;
}

int Rendering_Start(DisplayDev eDev)
{
    int RendType;

	//wlh 20090405 add start
	static byte nOldRendState;
    static uint32 nOldCFGRendType;
	(void)OEM_GetConfig(CFGI_DEFAULT_REND, (void*)&nOldCFGRendType, sizeof(nOldCFGRendType));
    (void)OEM_GetConfig(CFGI_REND_STATE, (void*)&nOldRendState, sizeof(nOldRendState));
    DBGPRINTF("Rendering_Start %d %d",nOldRendState,nOldCFGRendType);
	if(nOldRendState == 0)
	{
		return 0;
	}
	else if(nOldRendState == 1)
	{
		g_RendDev[eDev].m_eType = nOldCFGRendType;
	}
	else if(nOldRendState == 2)
	{
		g_RendDev[eDev].m_eType = REND_RANDOM;
	}
	//wlh 20090405 add end

    g_RendDev[eDev].m_iCurrStep = 0;
    if(g_RendDev[eDev].m_eType == REND_RANDOM)
    {
        RendType = Rendering_GetRand(REND_MAX);
    }
    else
    {
        RendType = g_RendDev[eDev].m_eType;
    }
#ifdef REND_TYPE_TEST
    // Test **************
    //RendType = REND_MOVE_LEFT;
#endif
    g_RendDev[eDev].m_pfnRendDraw = Rendering_GetRendDraw16(RendType);
	g_RendDev[eDev].m_iFlexDirection = Rendering_GetRend16FlexDirect(RendType);//wlh 20090406 add
    if(g_RendDev[eDev].m_pfnRendDraw)
    {
		RendDraw myRendDraw;

	    g_RendDev[eDev].m_isDrawing = TRUE;
	    DBGPRINTF("Rendering_StartTimer %d %d",MSPF(g_RendDev[eDev].m_fps),eDev);
		Rendering_StartTimer(MSPF(g_RendDev[eDev].m_fps), &g_RendDev[eDev]);
		myRendDraw.pDest      = g_RendDev[eDev].m_pDestBuf;
		myRendDraw.pSrc1      = g_RendDev[eDev].m_pSrcBuf;
		myRendDraw.pSrc2      = g_RendDev[eDev].m_pBackBuf;
		myRendDraw.wWidthSrc  = REND_SCREEN_WIDTH;
		myRendDraw.wHeightSrc = REND_SCREEN_HEIGHT;

		g_RendDev[eDev].m_pfnRendDraw(&myRendDraw,
                                       g_RendDev[eDev].m_iCurrStep,
                                       g_RendDev[eDev].m_iMaxStep
                                      );
		Rendering_UpdateDev(g_RendDev[eDev].m_pDestBuf);
    }
    return 0;
}

static boolean Rendering_IsPlaying(DisplayDev eDev)
{
	return g_RendDev[eDev].m_isDrawing;
}
boolean Rendering_IsPlayingEx(DisplayDev eDev)
{
	return g_RendDev[eDev].m_isDrawing;
}
static boolean Rendering_IsEnable(DisplayDev eDev)
{
	return g_RendDev[eDev].m_isEnable && Rendering_DevAvaild();
}

void Rendering_TimerCB(void *pUser)
{
    RendDev *pMe = (RendDev *)pUser;
    DBGPRINTF("Rendering_TimerCB %d %d 0x%x",pMe->m_iCurrStep,pMe->m_iMaxStep,pMe->m_pfnRendDraw);
    pMe->m_iCurrStep++;
    if(pMe->m_iCurrStep >= pMe->m_iMaxStep)
    {
		//wlh 20090406 add start
		//if((pMe->m_eType == REND_FLY_LEFT_FLEX) || (pMe->m_eType == REND_FLY_RIGHT_FLEX) || (pMe->m_eType == REND_FLY_TOP_FLEX) || (pMe->m_eType == REND_FLY_BOTTOM_FLEX) )
		if(pMe->m_eType == REND_FLY_TOP_FLEX)
		{
			if(pMe->m_pfnRendDraw != Rendering_GetRendDraw16Flex())
			{
				pMe->m_pfnRendDraw = Rendering_GetRendDraw16Flex();
				pMe->m_iCurrStep = 0;
				Rendering_StartTimer(MSPF(pMe->m_fps), pUser);
				return; 
			}
		}
		//wlh 20090406 add end
		Rendering_PushScreen(pMe->m_eDev);
		Rendering_UpdateDev(pMe->m_pSrcBuf);
		pMe->m_isDrawing = FALSE;
		
        return;
    }
    
    if(pMe->m_pfnRendDraw)
    {
		RendDraw myRendDraw;
        
		Rendering_StartTimer(MSPF(pMe->m_fps), pUser);
		
		myRendDraw.pDest      = pMe->m_pDestBuf;
		myRendDraw.pSrc1      = pMe->m_pSrcBuf;
		myRendDraw.pSrc2      = pMe->m_pBackBuf;
		myRendDraw.wWidthSrc  = REND_SCREEN_WIDTH;
		myRendDraw.wHeightSrc = REND_SCREEN_HEIGHT;
		myRendDraw.iFlexDirection = pMe->m_iFlexDirection;//wlh 20090406 add
		
        pMe->m_pfnRendDraw(&myRendDraw,
                            pMe->m_iCurrStep,
                            pMe->m_iMaxStep
                           );
		if(pMe->m_iCurrStep != 1)
		Rendering_UpdateDev(pMe->m_pDestBuf);
    }
}
