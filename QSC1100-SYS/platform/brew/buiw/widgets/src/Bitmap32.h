/*
  ========================================================================

  FILE:  Bitmap32.h
  
  SERVICES:  

  GENERAL DESCRIPTION: Bitmap32 implements a 32bpp alpha-capable bitmap


  ========================================================================
  ========================================================================
    
               Copyright © 2006-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef _BITMAP32_H_
#define _BITMAP32_H_

#include "AEE.h"
#include "AEEDisp.h"
#include "AEEShell.h"
#include "AEEStdLib.h"


// Define the new colour scheme
#define IDIB_COLORSCHEME_8888 32



typedef struct Bitmap32 Bitmap32;

// This is just for type safe casting of a Bitmap32* to an IBitmap*.
static __inline IBitmap *BITMAP32_TO_IBITMAP(const Bitmap32 *pBitmap32)
{
   return (IBitmap*)(void*)pBitmap32;
}

struct Bitmap32 {
   // IDIB overlays IBitmap.
   //
   // The vtable (pvt) pointer of m_dib is used as the vtable pointer for the
   // IBitmap instance.
   IDIB        m_dib;
   uint32      m_uRefs;

   // area of bitmap that is dirty (needs to be updated to screen)
   int         m_xDirty;
   int         m_yDirty;
   int         m_x2Dirty;
   int         m_y2Dirty;
};



/**
   Construct a 32bpp bitmap with a specific width and height

   @param w - width of bitmap
   @param h - height of bitmap
   @param pBuf - use for the bitmap buffer if non-null (otherwise memory will be allocated)
                 the Bitmap32 takes ownership of this memory if provided, and deallocates by calling FREE
   @param ppIBitmap - on return contains pointer to bitmap

   @return SUCCESS or errorcode
   */
extern int Bitmap32_New(uint16 w, uint16 h, void *pBuf, IBitmap **ppIBitmap);

/**
   Blit from this bitmap class to a desination buffer, with a global transparency value 
   (which applies in addition to per-pixel alpha values

   This method can be used instead of IBITMAP_BltOut.

   @param opacity - applies as well as any individual pixel alpha values.
   */
int Bitmap32_BltOutTransparent(IBitmap* pSrc, int aDstX, int aDstY, int aSrcW, int aSrcH, IBitmap* pDst, int aSrcX, int aSrcY, int opacity);


#endif // _BITMAP32_H_
