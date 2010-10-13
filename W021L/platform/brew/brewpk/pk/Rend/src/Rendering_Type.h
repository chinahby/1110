#ifndef _RENDERING_TYPE_H
#define _RENDERING_TYPE_H
//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_Type.h
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

typedef struct {
	void        *pDest;
	void        *pSrc1; 
	void        *pSrc2;
	int16        wWidthSrc;
	int16        wHeightSrc;
    int          nDepth;
}RendDraw;

typedef void (*PFNRENDDRAW)(RendDraw *pMe, int Step, int MaxStep);

extern PFNRENDDRAW Rendering_GetRendDraw16(RendType eType);
#endif // _RENDERING_TYPE_H

