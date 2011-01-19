/*
  ========================================================================

  FILE:           samplewidgets_util.h

  SERVICES:       Private declarations and definitions of utility 
                  functions used by the sample widgets.  Used internally 
                  by the SampleWidget module only.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __SAMPLEWIDGETS_UTIL_H__
#define __SAMPLEWIDGETS_UTIL_H__

#include "AEEDisp.h"
#include "AEEStdLib.h"

// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt)     MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)) )
#endif

// get pointer to vtbl from object allocated w/ MALLOCREC_VTBL
#ifndef GETVTBL
#define GETVTBL(po,vtt)             ((AEEVTBL(vtt)*)(void *)((po)+1))
#endif

// RELEASEIF
#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**) (void *) &p)

static __inline void RELEASEPPIF(IBase **p) {
   if (*p) {
      IBASE_Release(*p);
      *p = 0;
   }
}
#endif

//CALLBACK_Timer
#ifndef CALLBACK_Timer
#define CALLBACK_Timer(pcb,pfn,pv,ps,n) \
   do { CALLBACK_Init((pcb), (pfn), (pv)); ISHELL_SetTimerEx((ps),(n),(pcb)); } while (0)
#endif

// Calculate the intersection of two rectangles
boolean IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2);

// Subtract one rect from another, if result is rectangular
boolean SubtractRect(AEERect *prcResult, const AEERect *prcBase, const AEERect *prcSub);

// Mono DIB functions
void InitMonoDIBPalette(IDIB *piDIB, RGBVAL rgb);
int CreateMonoDIB(IDisplay *piDisplay, IDIB **ppiDIB, uint16 width, uint16 height, boolean bAllocData);

#endif // __SAMPLEWIDGETS_UTIL_H__