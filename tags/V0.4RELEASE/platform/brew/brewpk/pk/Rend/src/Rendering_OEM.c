//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_OEM.c
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
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "OEMDisplayDev.h"
#include "Rendering_Config.h"
#include "Rendering_OEM.h"

extern void Rendering_TimerCB(void *pUser);

int Rendering_StartTimer(int ms, void *pUser)
{
    return AEE_SetSysTimer(ms, Rendering_TimerCB, pUser);;
}

int Rendering_CancelTemer(void *pUser)
{
    return AEE_CancelTimer(Rendering_TimerCB, pUser);
}

int Rendering_GetRand(unsigned char Max)
{
    unsigned char Rand;
    GETRAND(&Rand, 1);
    return Rand%Max;
}

boolean Rendering_DevAvaild(void)
{
	AEEDeviceInfo Info;	
	AEE_GetDeviceInfo(&Info);
#if (REND_PLATFORMID != 0)
	if(Info.dwPlatformID != REND_PLATFORMID)
	{
		return FALSE;
	}
#endif
//wlh mod for 3D test
//	if(Info.cxScreen != REND_SCREEN_WIDTH || Info.cyScreen != REND_SCREEN_HEIGHT)
//	{
//		return FALSE;
//	}
	return TRUE;
}

extern IBitmap *gpDevBitmap;
extern IDisplayDev *gpDevDisp;
extern int OEMDisplayDev_Update(IDisplayDev *pMe, IBitmap *pbmSrc, AEERect *prc);

void Rendering_UpdateDev(void *pBmpBuf)
{
	void *pBack = ((IDIB *)gpDevBitmap)->pBmp;
	AEERect rc={0, 0, REND_SCREEN_WIDTH, REND_SCREEN_HEIGHT};
	DBGPRINTF("Rendering_UpdateDev");
	((IDIB *)gpDevBitmap)->pBmp = (byte *)pBmpBuf;
	OEMDisplayDev_Update(gpDevDisp,gpDevBitmap,&rc);
	((IDIB *)gpDevBitmap)->pBmp = (byte *)pBack;
}

void *Rendering_Memset(void *buffer, int c, int count)
{
	return MEMSET(buffer,c,count);
}
void *Rendering_Memcpy(void *dest, void *src, int count)
{
	return MEMCPY(dest, src, count);
}

void *Rendering_Memset32(void *buffer, int c, int count)
{
    uint32 *p = (uint32 *)buffer;
    int countmod;
    count = count>>2;
    countmod = count&0x7;
    count = count>>3;
    while(count--)
    {
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
    }
    
    while(countmod--)
    {
        *p++ = c;
    }
	return buffer;
}

void *Rendering_Malloc(int count)//wlh 20090413
{
	return MALLOC(count);
}
void Rendering_Free(void *buffer)//wlh 20090413
{
	FREE(buffer);
}