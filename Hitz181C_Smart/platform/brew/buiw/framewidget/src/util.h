/*
  ========================================================================

  FILE: util.h
  
  SERVICES: 

  GENERAL DESCRIPTION:  


  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __UTIL_H__
#define __UTIL_H__

#include "AEE.h"

boolean IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2);


// Release helper
#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**)(void*)&p)
static __inline void RELEASEPPIF(IBase **p) 
{ if (*p) { IBASE_Release(*p); *p = 0; } }
#endif


// AddRef helper
#ifndef ADDREFIF
#define ADDREFIF(p)  do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif


// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt)     MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)) )
#endif

// get pointer to vtbl from object allocated w/ MALLOCREC_VTBL
#ifndef GETVTBL
#define GETVTBL(po,vtt)             ((AEEVTBL(vtt)*)(void *)((po)+1))
#endif

#ifndef SETAEERECT
#define SETAEERECT(prc,l,t,w,h)   (prc)->x=(int16)(l),(prc)->y=(int16)(t),(prc)->dx=(int16)(w),(prc)->dy=(int16)(h)
#endif



#endif //__UTIL_H__
