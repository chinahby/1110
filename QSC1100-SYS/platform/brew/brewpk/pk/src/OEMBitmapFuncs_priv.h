/*=====================================================================
FILE:  OEMBitmapFuncs_priv.h

SERVICES:  OEMBitmap function prototypes

DESCRIPTION: Reference IBitmap implementation function prototypes

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/


#ifndef OEMBITMAPFUNCS_PRIV_H
#define OEMBITMAPFUNCS_PRIV_H

#include "AEE.h"
#include "AEEBitmap.h"
#include "OEMDisplayDev.h"

// Implement multiple bit depth code
#if defined(AEE_SIMULATOR)
#define IMPLEMENT_MULTIDEPTH
#endif

#if defined(IMPLEMENT_MULTIDEPTH)
extern int OEMBitmap_CopyMode(IBitmap *pDst, IBitmap *pSrc);
#endif

extern int OEMBitmap1_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap1_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap1_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap2_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap2_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap2_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap8_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap8_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap8_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap12_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap12_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap12_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap16_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap16_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap16_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap18_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap18_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap18_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap24_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);
extern int OEMBitmap24_NewEx(uint16 w, uint16 h, void *pBuf, PFNREALLOC pfn, IDisplayDev *pIDispDev, IBitmap **ppIBitmap);
extern int OEMBitmap24_New_Child(IBitmap *pParent, PFNREALLOC pfn, IBitmap **ppIBitmap);

extern int OEMBitmap_SetRotation(IBitmap *pBmp, int nDegrees);

#endif /* OEMBITMAPFUNCS_PRIV_H */

