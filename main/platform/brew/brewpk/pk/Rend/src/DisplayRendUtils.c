#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEIDisplayDevRend.h"
#include "Rendering.h"
#include "OEMConfig.h"
#include "DisplayRendUtils.h"
#include "OEMCFGI.h"

#ifndef RELEASEIF
#define RELEASEIF(p) do { if (p) { IQI_Release((IQueryInterface*)(p)); p = 0; } } while (0)
#endif

static int              DisplayRend_GetRand(unsigned char Max);
static RendType         DisplayRend_GetRendType(DisplayRendType idx, boolean bIn);
static DisplayRendType  DisplayRend_GetTypeByCfg(boolean bIn);

void DisplayRend_Enable(boolean bEnable)
{
    IDisplayDevRend *pDispDevRend = NULL;
    int nErr;
    
    nErr = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_DISPLAYDEV1,(void **)&pDispDevRend);
    if(nErr != SUCCESS)
    {
        return;
    }
    IDisplayDevRend_SetRendEnable(pDispDevRend,bEnable);
    RELEASEIF(pDispDevRend);
}

void DisplayRend_Push(void)
{
    IDisplayDevRend *pDispDevRend = NULL;
    int nErr;
    
    nErr = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_DISPLAYDEV1,(void **)&pDispDevRend);
    if(nErr != SUCCESS)
    {
        return;
    }
    IDisplayDevRend_PushScreen(pDispDevRend);
    RELEASEIF(pDispDevRend);
}

//#define REND_TEST REND_BLINDS_INTER_IN
void DisplayRend_StartEx(DisplayRendType eType, boolean bIn, int nDelay)
{
    IDisplayDevRend *pDispDevRend = NULL;
    int nErr;

    DBGPRINTF_FATAL("DisplayRend_Start");
    if(eType >= DISPLAYREND_TYPE_GROUP)
    {
        return;
    }
    
    nErr = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_DISPLAYDEV1,(void **)&pDispDevRend);
    if(nErr != SUCCESS)
    {
        return;
    }
    
#ifdef REND_TEST
    IDisplayDevRend_SetRendFPS(pDispDevRend, 1);
    IDisplayDevRend_StartRend(pDispDevRend, REND_TEST);
#else
    IDisplayDevRend_StartRend(pDispDevRend, DisplayRend_GetRendType(eType, bIn), nDelay);
#endif
    RELEASEIF(pDispDevRend);
}

void DisplayRend_Start(boolean bIn)
{
    DisplayRendType eType;
    
    eType = DisplayRend_GetTypeByCfg(bIn);
    DisplayRend_StartEx(eType, bIn, 100);
}

void DisplayRend_SetConfig(DisplayRendType eType)
{
    byte RendType = (byte)eType; 
    (void)OEM_SetConfig(CFGI_REND_STATE, (void*)&RendType, sizeof(RendType)); 
}

static int DisplayRend_GetRand(unsigned char Max)
{
    unsigned char Rand;
    GETRAND(&Rand, 1);
    return (Rand*Max)/0x100;
}

static RendType DisplayRend_GetRendType(DisplayRendType idx, boolean bIn)
{
    static const RendType DisplayRends[DISPLAYREND_TYPE_GROUP][2] = 
    {
        {REND_FADE_OUT,                 REND_FADE_IN},
        {REND_SHOW_HORZ_OUT,            REND_SHOW_HORZ_IN},
        {REND_SHOW_VERT_OUT,            REND_SHOW_VERT_IN},
        {REND_MOVE_HORZ_OUT,            REND_MOVE_HORZ_IN},
        {REND_MOVE_VERT_OUT,            REND_MOVE_VERT_IN},
        #if !defined(FEATURE_VERSION_W516) && !defined(FEATURE_VERSION_W208S)
        {REND_FLY_HORZ_OUT,             REND_FLY_HORZ_IN},   
        {REND_FLY_VERT_OUT,             REND_FLY_VERT_IN},
        #endif    
        {REND_ROTAT_HORZ_OUT,           REND_ROTAT_HORZ_IN},
        {REND_ROTAT_VERT_OUT,           REND_ROTAT_VERT_IN},
        {REND_BLINDS_VERT_OUT,          REND_BLINDS_VERT_IN},
        {REND_BLINDS_INTER_OUT,         REND_BLINDS_INTER_IN}
    };

    int idx2 = bIn?1:0;
    return DisplayRends[idx][idx2];
}

static DisplayRendType DisplayRend_GetTypeByCfg(boolean bIn)
{
    DisplayRendType         eType;
	static DisplayRendType  eOneByOne = DISPLAYREND_TYPE_FADE;
    byte                    RendType; 
    
    (void)OEM_GetConfig(CFGI_REND_STATE, (void*)&RendType, sizeof(RendType)); 
    DBGPRINTF_FATAL("CFGI_REND_STATE %d",RendType);
    switch(RendType){
    case DISPLAYREND_TYPE_RAND:
        eType = eOneByOne;
        if(bIn)
        {
            eOneByOne = DisplayRend_GetRand(DISPLAYREND_TYPE_GROUP);
        }
        break;
        
    case DISPLAYREND_TYPE_ONEBYONE:
        eType = eOneByOne;
        if(bIn)
        {
            eOneByOne++;
            if(eOneByOne >= DISPLAYREND_TYPE_GROUP)
            {
                eOneByOne = DISPLAYREND_TYPE_FADE;
            }
        }
        break;
    case DISPLAYREND_TYPE_MAX:
    {
        eType = (DisplayRendType)RendType;
        break;
    }
    default:
        if(RendType < DISPLAYREND_TYPE_GROUP)
        {
            eType = (DisplayRendType)RendType;
        }
        else
        {
            eType = DISPLAYREND_TYPE_FADE;
        }
        break;
    }
    
    return eType;
}

void DisplayRend_Done(void)
{
    IDisplayDevRend *pDispDevRend = NULL;
    int nErr;
    
    nErr = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_DISPLAYDEV1,(void **)&pDispDevRend);
    if(nErr != SUCCESS)
    {
        return;
    }
    IDisplayDevRend_DoneRend(pDispDevRend);
    RELEASEIF(pDispDevRend);
}

