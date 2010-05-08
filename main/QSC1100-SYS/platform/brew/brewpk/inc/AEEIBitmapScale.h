#ifndef AEEIBITMAPSCALE_H
#define AEEIBITMAPSCALE_H
/*=============================================================================
      Copyright (c) 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEEIBitmap.h"
#include "AEERect.h"

// Transparency Operations
#define AEEBITMAPSCALE_OPAQUE     0
#define AEEBITMAPSCALE_ALPHA      1
#define AEEBITMAPSCALE_KEYCOLOR   2

#define AEEIID_IBitmapScale 0x0101e8bc

#define INHERIT_IBitmapScale(iname) \
   INHERIT_IQI(iname); \
   int (*Scale)(iname *p, \
                IBitmap *pSrc, const AEERect *prcSrc, \
                IBitmap *pDst, const AEERect *prcDst, \
                const AEERect *prcClip, int nTransOp)

AEEINTERFACE_DEFINE(IBitmapScale);

static __inline uint32 IBitmapScale_AddRef(IBitmapScale *me)
{
   return AEEGETPVTBL(me, IBitmapScale)->AddRef(me);
}

static __inline uint32 IBitmapScale_Release(IBitmapScale *me)
{
   return AEEGETPVTBL(me, IBitmapScale)->Release(me);
}

static __inline int IBitmapScale_QueryInterface(IBitmapScale *me, AEEIID iid, void **ppif)
{
   return AEEGETPVTBL(me, IBitmapScale)->QueryInterface(me, iid, ppif);
}

static __inline int IBitmapScale_Scale(IBitmapScale *me,
                                      IBitmap *pSrc, const AEERect *prcSrc,
                                      IBitmap *pDst, const AEERect *prcDst,
                                      const AEERect *prcClip, int nTransOp)
{
   return AEEGETPVTBL(me, IBitmapScale)->Scale(me, pSrc, prcSrc, pDst, prcDst,
                                              prcClip, nTransOp);
}

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================

Bitmap Scaling Transparency Operations

Description:  
   Specifies how transparency should be handled inside the scaling algorithm.
   These constants are passed to IBitmapScale_Scale() to govern how the alpha channel
   is treated while scaling.

   IBitmapScale must rely on the caller to specify if the source image supports
   transparency.  If it doesn't, IBitmapScale_Scale() can ignore the alpha channel
   and do the scale in less time.  If it does, IBitmapScale_Scale() has to create an
   alpha channel if needed and consider it  while combining pixels to make sure
   transparency is preserved.

Definition:
===pre>
   
   AEEBITMAPSCALE_OPAQUE: The source image does not support transparency

   AEEBITMAPSCALE_ALPHA: The source image has an alpha channel

   AEEBITMAPSCALE_KEYCOLOR: The source image has a transparent color

===/pre>

Comments:
   
   If AEEBITMAPSCALE_OPAQUE is specified, the source image is assumed to be opaque.  
   The alpha channel is ignored and the image is simply copied to the destination
   bitmap.  This mode provides the best performance.

   If AEEBITMAPSCALE_ALPHA is specified, IBitmapScale_Scale() combines the alpha channel for
   each pixel.  The result is blended into the destination bitmap.  This mode
   produces good results but is somewhat slower.

   If AEEBITMAPSCALE_KEYCOLOR is specified, IBitmapScale_Scale() builds an alpha channel
   from the source image using the transparency color specified in the image and
   combines it for each pixel.  This is somewhat slower but is necessary to
   preserve transparency.

See Also:
   IBitmapScale_Scale()

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

IBitmapScale Interface

Description:
   This interface provides methods to scale bitmaps.

==============================================================================
Function: IBitmapScale_AddRef()

This function is inherited from IQI_AddRef().

===================================================================================

Function: IBitmapScale_Release()

This function is inherited from IQI_Release().

===================================================================================

Function: IBitmapScale_QueryInterface()

This function is inherited from IQI_QueryInterface().

===================================================================================

Function: IBitmapScale_Scale()

Description:
   This function performs a scale of a section of the source bitmap and outputs
   it to the destination bitmap.

   **************       ****************************
   *pSrc        *       *pDst       ____________   *
   *            *       *          |prcClip     |  *
   * .........  *       *          |            |  *
   * .prcSrc .  *       *          |            |  *
   * .       .  *       *          |            |  *
   * .       .  *  =>   *  ........|........... |  *
   * .........  *       *  .prcDst |          . |  *
   *            *       *  .       |          . |  *
   **************       *  .       |          . |  *
                        *  .       |          . |  *
                        *  .       |          . |  *
                        *  .       |____________|  *
                        *  .                  .    *
                        *  ....................    *
                        ****************************

   The area of the source bitmap defined by prcSrc is scaled to fit into the prcDst 
   rect.  Only the part of the image that ends up in the intersection of prcDst and 
   prcClip is drawn into the destination.

Prototype:
   int IBitmapScale_Scale(
      IBitmapScale *po,
      IBitmap *pSrc,
      AEERect *prcSrc,
      IBitmap *pDst,
      AEERect *prcDst,
      AEERect *prcClip
      int nTransOp);

Parameters:
   po - the IBitmapScale interface
   pSrc - the source bitmap
   prcSrc - the part of the source bitmap we wish to scale (or NULL)
   pDst - the desination bitmap
   prcDst - the rect in which the scale the image into (or NULL)
   prcClip - clipping rect in the destination (or NULL)
   nTransOp - specifies how to handle transparency

Return Value:
   AEE_SUCCESS if functiona executed correctly.
   Error code if failed.

Comments:
   Important: The desitnation bitmap must implement IBITMAP_BltIn().

   if (prcSrc == NULL) then the entire source bitmap is scaled

   if (prcDst == NULL) then the source image is scaled into the entire
      destination bitmap.

   if (prcClip == NULL) then the entire scaled image is drawn into the
      destination bitmap.

Side Effects: 
   None

See Also:
   None

===================================================================================  */

#endif /* AEEIBITMAPSCALE_H */
