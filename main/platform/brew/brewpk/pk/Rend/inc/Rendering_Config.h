#ifndef RENDERING_CONFIG_H
#define RENDERING_CONFIG_H
//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_Config.h
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

#define REND_SCREEN_WIDTH  176//wlh ÆÁÄ»¿í¶È
#define REND_SCREEN_HEIGHT 220//wlh ÆÁÄ»¸ß¶È
#define REND_BPP           2
#define REND_BUF_SIZE      (REND_SCREEN_WIDTH*REND_SCREEN_HEIGHT*REND_BPP)
#define REND_PLATFORMID    0
#define REND_DEFAULT_CLR   0

// squrt(w*w+h*h)
#define REND_Z_MAX         400

#define REND_MEMSET        Rendering_Memset
#define REND_MEMSET32      Rendering_Memset32
#define REND_MEMCPY        Rendering_Memcpy
#define REND_MALLOC        Rendering_Malloc//wlh 20090413
#define REND_FREE          Rendering_Free//wlh 20090413
extern void *Rendering_Memset(void *buffer, int c, int count);
extern void *Rendering_Memset32(void *buffer, int c, int count);
extern void *Rendering_Memcpy(void *dest, void *src, int count);
extern void *Rendering_Malloc(int count);//wlh 20090413
extern void  Rendering_Free(void *buffer);//wlh 20090413
#endif // RENDERING_CONFIG_H 

