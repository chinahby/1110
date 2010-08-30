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
#define REND_PLATFORMID    0
#define REND_DEFAULT_CLR   0
#define REND_FPS_DEFAULT   25
#define REND_STEP_MAX      10

// squrt(w*w+h*h)
#define REND_Z_MAX         400

#define REND_ROUND2EVEN(n) (n&(~0x1))
#define REND_MEMCPY16      Rendering_Memcpy16
#define REND_MEMCPY32      Rendering_Memcpy32
#define REND_MEMSET16      Rendering_Memset16
#define REND_MEMSET32      Rendering_Memset32

extern void *Rendering_Memcpy16(void *dest, void *src, int size);
extern void *Rendering_Memcpy32(void *dest, void *src, int size);
extern void *Rendering_Memset16(void *buffer, int c, int size);
extern void *Rendering_Memset32(void *buffer, int c, int size);


#endif // RENDERING_CONFIG_H 

