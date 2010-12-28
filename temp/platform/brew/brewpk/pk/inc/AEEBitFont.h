// AEEBitFont.h
#ifndef AEEBITFONT_H
#define AEEBITFONT_H

#include "AEEFont.h"
#include "AEE.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

// Values for pvStorage:
//
#define AEEBITFONT_COPYDATA    ((void*)0)   // copy data; don't retain pointer; don't free anything
#define AEEBITFONT_STATICDATA  ((void*)1)   // retain pointers, but don't free anything


// Static interface
//
int       AEE_EXPORTS AEEBitFont_NewFromBBF(const byte *pbyMem, int cbMem, void *pvStorage, PFNREALLOC pfnRealloc, IFont **ppIFont);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif // AEEBITFONT_H

