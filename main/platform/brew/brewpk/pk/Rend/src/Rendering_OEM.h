#ifndef _RENDERING_OEM_H
#define _RENDERING_OEM_H
//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_OEM.h
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

extern int Rendering_StartTimer(int ms, void *pUser);
extern int Rendering_CancelTemer(void *pUser);
extern int Rendering_GetRand(unsigned char Max);
extern void Rendering_UpdateDev(void *pBmpBuf);
extern boolean Rendering_DevAvaild(void);
#endif // _RENDERING_OEM_H 
