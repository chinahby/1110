#ifndef  AEEIDISPLAYDEVREND_H
#define  AEEIDISPLAYDEVREND_H
/*=============================================================================

FILE:          AEEIDisplayDevRend.h

SERVICES:      IDisplayDevRend interface

DESCRIPTION:   Low level display functions.

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

        Copyright ?2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIQI.h"
#include "AEEIBitmap.h"
#include "AEERect.h"
#include "OEMDisplayDev.h"
#include "Rendering.h"

#define INHERIT_IDisplayDevRend(iname) \
    INHERIT_IDisplayDev(iname); \
    int  (*SetRendEnable)(iname *po, boolean bEnable, int fps); \
    int  (*PushScreenEx)(iname *po, IBitmap *pbmSrc); \
    int  (*StartRend)(iname *po, RendType eType, int nDelay); \
    int  (*StopRend)(iname *po); \
    int  (*DoneRend)(iname *po); \
    boolean (*IsRendEnable)(iname *po); \
    boolean (*IsRendPlaying)(iname *po)

AEEINTERFACE_DEFINE(IDisplayDevRend);

// Access macros for IDisplayDevRend
static __inline uint32 IDisplayDevRendRend_AddRef(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->AddRef(p);
}
static __inline uint32 IDisplayDevRend_Release(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->Release(p);
}
static __inline int IDisplayDevRend_QueryInterface(IDisplayDevRend *p, AEEIID i, void **p2)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->QueryInterface(p,i,p2);
}
static __inline int IDisplayDevRend_Update(IDisplayDevRend *p, IBitmap *b, AEERect *r)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->Update(p,b,r);
}
static __inline int IDisplayDevRend_SetRendEnableEx(IDisplayDevRend *p, boolean b, int fps)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->SetRendEnable(p,b,fps);
}
static __inline int IDisplayDevRend_SetRendEnable(IDisplayDevRend *p, boolean b)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->SetRendEnable(p,b,0);
}
static __inline int IDisplayDevRend_PushScreenEx(IDisplayDevRend *p, IBitmap *b)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->PushScreenEx(p,b);
}
static __inline int IDisplayDevRend_PushScreen(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->PushScreenEx(p,NULL);
}
static __inline int IDisplayDevRend_StartRend(IDisplayDevRend *p, RendType e, int nDelay)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->StartRend(p,e,nDelay);
}
static __inline int IDisplayDevRend_StopRend(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->StopRend(p);
}
static __inline int IDisplayDevRend_DoneRend(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->DoneRend(p);
}
static __inline boolean IDisplayDevRend_IsRendEnable(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->IsRendEnable(p);
}
static __inline boolean IDisplayDevRend_IsRendPlaying(IDisplayDevRend *p)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->IsRendPlaying(p);
}
static __inline int IDisplayDevRend_SetRendFPS(IDisplayDevRend *p, int fps)
{
   return AEEGETPVTBL(p, IDisplayDevRend)->SetRendEnable(p,IDisplayDevRend_IsRendEnable(p),fps);
}

#endif /* AEEIDISPLAYDEVREND_H */

