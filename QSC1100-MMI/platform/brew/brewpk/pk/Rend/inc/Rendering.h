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

typedef enum{
    DISPLAYDEV_MAIN = 0,
    //DISPLAYDEV_SLAVE,
    DISPLAYDEV_MAX
}DisplayDev;

typedef enum{
    REND_RANDOM = 0,
    /*REND_SHOW_LEFTRIGHT_OUT,         //显示
	REND_SHOW_LEFTRIGHT_IN,
    REND_SHOW_TOPBOTTOM_OUT,
    REND_SHOW_TOPBOTTOM_IN,
	REND_SHOW_CENTER_OUT,
    REND_SHOW_CENTER_IN,*/
    REND_MOVE_LEFT,
    REND_MOVE_RIGHT,
   // REND_MOVE_TOP,
   // REND_MOVE_BOTTOM,
    //REND_FLY_LEFT,          //滑入
	//REND_FLY_LEFT_FLEX,     //wlh 20090406 add
    //REND_FLY_RIGHT,
	//REND_FLY_RIGHT_FLEX,    //wlh 20090406 add
    REND_FLY_TOP,
    REND_FLY_TOP_FLEX,     //wlh 20090406 add
    REND_FLY_BOTTOM,
   // REND_FLY_BOTTOM_FLEX,     //wlh 20090406 add
/*    REND_FLY_LEFTTOP,
    REND_FLY_LEFTBOTTOM,
    REND_FLY_RIGHTTOP,
    REND_FLY_RIGHTBOTTOM,
    REND_FLYOUT_LEFT,       //滑出
    REND_FLYOUT_RIGHT,
    REND_FLYOUT_TOP,
    REND_FLYOUT_BOTTOM,
    REND_FLYOUT_LEFTTOP,
    REND_FLYOUT_LEFTBOTTOM,
    REND_FLYOUT_RIGHTTOP,
    REND_FLYOUT_RIGHTBOTTOM,*/
    //REND_ROTAT_LEFT,        //旋入
    //REND_ROTAT_RIGHT,
   /* REND_ROTAT_TOP,
    REND_ROTAT_BOTTOM,
    REND_ROTAT_LEFTTOP,
    REND_ROTAT_LEFTBOTTOM,
    REND_ROTAT_RIGHTTOP,
    REND_ROTAT_RIGHTBOTTOM,
    REND_ROTATOUT_LEFT,     //旋出
    REND_ROTATOUT_RIGHT,
    REND_ROTATOUT_TOP,
    REND_ROTATOUT_BOTTOM,
    REND_ROTATOUT_LEFTTOP,
    REND_ROTATOUT_LEFTBOTTOM,
    REND_ROTATOUT_RIGHTTOP,
    REND_ROTATOUT_RIGHTBOTTOM,*/
    REND_BLINDS_LEFT,       //百叶窗入
    REND_BLINDS_RIGHT,
	REND_BLINDS_IN, //左右交叉百叶进
	REND_BLINDS_OUT, //左右交叉百叶出
	/*
    REND_BLINDS_TOP,
    REND_BLINDS_BOTTOM,
    REND_BLINDS_LEFTTOP,
    REND_BLINDS_LEFTBOTTOM,
    REND_BLINDS_RIGHTTOP,
    REND_BLINDS_RIGHTBOTTOM,
    REND_BLINDSOUT_LEFT,    //百叶窗出
    REND_BLINDSOUT_RIGHT,
    REND_BLINDSOUT_TOP,
    REND_BLINDSOUT_BOTTOM,
    REND_BLINDSOUT_LEFTTOP,
    REND_BLINDSOUT_LEFTBOTTOM,
    REND_BLINDSOUT_RIGHTTOP,
    REND_BLINDSOUT_RIGHTBOTTOM,
    REND_SCROLL_LEFT,       //卷入
    REND_SCROLL_RIGHT,
    REND_SCROLL_TOP,
    REND_SCROLL_BOTTOM,
    REND_SCROLL_LEFTTOP,
    REND_SCROLL_LEFTBOTTOM,
    REND_SCROLL_RIGHTTOP,
    REND_SCROLL_RIGHTBOTTOM,
    REND_SCROLLOUT_LEFT,    //卷出
    REND_SCROLLOUT_RIGHT,
    REND_SCROLLOUT_TOP,
    REND_SCROLLOUT_BOTTOM,
    REND_SCROLLOUT_LEFTTOP,
    REND_SCROLLOUT_LEFTBOTTOM,
    REND_SCROLLOUT_RIGHTTOP,
    REND_SCROLLOUT_RIGHTBOTTOM,
    REND_ROLL_LEFT,         //立体滚动
    REND_ROLL_RIGHT,
    REND_ROLL_TOP,
    REND_ROLL_BOTTOM,
    REND_ROLL_LEFTTOP,
    REND_ROLL_LEFTBOTTOM,
    REND_ROLL_RIGHTTOP,
    REND_ROLL_RIGHTBOTTOM,
    REND_FIRE_TOP,          //火焰
    REND_FIRE_BOTTOM,
    REND_FADE_IN,           //渐变
    REND_FADE_OUT,*/
    REND_MAX
}RendType;

extern int Rendering_Initlize(DisplayDev eDev, void *pSrcBuf);
extern int Rendering_Release(DisplayDev eDev);
extern int Rendering_Update(DisplayDev eDev, int16 x, int16 y, int16 dx, int16 dy);

extern int Rendering_SetType(DisplayDev eDev, RendType eType, int16 xDst, int16 yDst, int16 yStart);
extern int Rendering_SetEnable(DisplayDev eDev, boolean bEnable);
#endif // _RENDERING_CONFIG_H 
