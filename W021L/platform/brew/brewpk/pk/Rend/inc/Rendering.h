#ifndef _RENDERING_CONFIG_H
#define _RENDERING_CONFIG_H
//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering.h
//Author:       AnyLook
//Version:      1.0
//Date:         2008/07/31
//Description:
//   Rendering Include file
//Others: 
//Function List: 
//<Name>        <desc>
//Simple        
//History: 
//<author>      <time>      <version >  <desc>
// AnyLook    2009/01/09       1.0       build this moudle  
//**************************************************************
#include "Rendering_Config.h"
#include "Rendering_Typedef.h"

typedef void * RENDHANDLE;

typedef enum{
    REND_SHOW_HORZ_OUT = 0,     // 水平显示
    REND_SHOW_HORZ_IN,
    REND_SHOW_VERT_OUT,         // 垂直显示
    REND_SHOW_VERT_IN,
    REND_FADE_OUT,              // 全屏渐变
    REND_FADE_IN,
    REND_MOVE_HORZ_OUT,         // 水平移动
    REND_MOVE_HORZ_IN,
    REND_MOVE_VERT_OUT,         // 垂直移动
    REND_MOVE_VERT_IN,
    REND_FLY_HORZ_OUT,          // 水平滑入
    REND_FLY_HORZ_IN,
    REND_FLY_VERT_OUT,          // 垂直滑入
    REND_FLY_VERT_IN,
    REND_ROTAT_HORZ_OUT,        // 水平旋转
    REND_ROTAT_HORZ_IN,
    REND_ROTAT_VERT_OUT,        // 垂直旋转
    REND_ROTAT_VERT_IN,
    REND_BLINDS_VERT_OUT,       // 垂直百叶窗
    REND_BLINDS_VERT_IN,
    REND_BLINDS_INTER_OUT,      // 交叉百叶窗
    REND_BLINDS_INTER_IN,
    REND_MAX
}RendType;

extern int     Rendering_Init(RENDHANDLE *hhDev);
extern int     Rendering_Done(RENDHANDLE hDev);
extern int     Rendering_SetSrcScreen(RENDHANDLE hDev, byte *pData, int nSize, int dx, int dy, int nPitch, int nDepth, int nScheme);
extern int     Rendering_PushScreen(RENDHANDLE hDev);
extern int     Rendering_Start(RENDHANDLE hDev, int nDelay);
extern int     Rendering_SetType(RENDHANDLE hDev, RendType eType);
extern int     Rendering_SetFPS(RENDHANDLE hDev, int nFPS);
extern int     Rendering_Stop(RENDHANDLE hDev);
extern int     Rendering_SetEnable(RENDHANDLE hDev, boolean bEnable);
extern boolean Rendering_IsEnable(RENDHANDLE hDev);
extern boolean Rendering_IsPlaying(RENDHANDLE hDev);
#endif // _RENDERING_CONFIG_H 

