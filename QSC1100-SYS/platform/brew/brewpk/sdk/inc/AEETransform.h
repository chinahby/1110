#ifndef  AEETRANSFORM_H
#define  AEETRANSFORM_H
/*=================================================================================
FILE:          AEETransform.h

SERVICES:      Bitmap transformation

DESCRIPTION:   This file provides definitions for the ITransform interface made
available by the AEE to the application developers.  This is an extension to
IBitmap, obtained using IBITMAP_QueryInterface.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2002-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEBitmap.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// constants for unTransform field of TransformBltSimple
#define TRANSFORM_FLIP_X         (0x0004)

#define TRANSFORM_ROTATE_90      (0x0001)
#define TRANSFORM_ROTATE_180     (0x0002)
#define TRANSFORM_ROTATE_270     (0x0003)

#define TRANSFORM_SCALE_2        (0x0008)
#define TRANSFORM_SCALE_4        (0x0010)
#define TRANSFORM_SCALE_8        (0x0018)
#define TRANSFORM_SCALE_EIGHTH   (0x0028)
#define TRANSFORM_SCALE_QUARTER  (0x0030)
#define TRANSFORM_SCALE_HALF     (0x0038)

// constants for unComposite field of TransformBltSimple and TransformBltComplex
#define COMPOSITE_KEYCOLOR       (0x00)
#define COMPOSITE_OPAQUE         (0xFF)

typedef struct {
   int16 A;  // A, B, C, and D are fixed point values with an 8-bit integer part
   int16 B;  // and an 8-bit fractional part.
   int16 C;
   int16 D;
} AEETransformMatrix;

typedef struct ITransform ITransform;



//=================================================================================
// FUNCTION DEFINITIONS and INLINE FUNCTION DEFINITIONS 
//=================================================================================


//*********************************************************************************
//
// ITransform Interface
//
//*********************************************************************************

AEEINTERFACE(ITransform)
{
   INHERIT_IQueryInterface(ITransform);

   int   (*TransformBltSimple)   (ITransform *po, int xDst, int yDst,
                                  IBitmap *pSrc, int xSrc, int ySrc,
                                  unsigned dxSrc, unsigned dySrc,
                                  uint16 unTransform, uint8 unComposite);

   int   (*TransformBltComplex)  (ITransform *po, int xDst, int yDst,
                                  IBitmap *pSrc, int xSrc, int ySrc,
                                  unsigned dxSrc, unsigned dySrc,
                                  const AEETransformMatrix *pMatrixTransform,
                                  uint8 unComposite);
};


// Access macros for IBitmap
#define  ITRANSFORM_AddRef(p)                AEEGETPVTBL(p,ITransform)->AddRef(p)
#define  ITRANSFORM_Release(p)               AEEGETPVTBL(p,ITransform)->Release(p)
#define  ITRANSFORM_QueryInterface(p,i,p2)   AEEGETPVTBL(p,ITransform)->QueryInterface(p,i,p2)
#define  ITRANSFORM_TransformBltSimple(p,xd,yd,s,x,y,cx,cy,t,c)\
                                             AEEGETPVTBL(p,ITransform)->TransformBltSimple(p,xd,yd,s,x,y,cx,cy,t,c)
#define  ITRANSFORM_TransformBltComplex(p,xd,yd,s,x,y,cx,cy,m,c)\
                                             AEEGETPVTBL(p,ITransform)->TransformBltComplex(p,xd,yd,s,x,y,cx,cy,m,c)



/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
AEEMatrixTransform

Description:
   This struct describes a 2X2 matrix used for doing complex transformations.

Definition:
   typedef struct {
      int16 A;
      int16 B;
      int16 C;
      int16 D;
   } AEETransformMatrix;

Members:
   A, B, C, D: Fixed point values with and 8-bit integer part and an 8-bit
               fractional part.  (For example, 2.5 would be represented as
               2.5 * 256 or 640.)  This makes up a 2x2 matrix as follows:
===pre>

               [ A B
                 C D ]
===/pre>

Comments:
   Some example transforms:
===pre>

   Scale by 2.5:  A = 640, B = 0,
                  C = 0,   D = 640.

   Rotate:        A = 256 * cos(angle),   B = 256 * sin(angle),
                  C = 256 * -sin(angle),  D = 256 * cos(angle).
===/pre>

   Note: The above values have already been converted to fixed point.

See Also:
   ITRANSFORM_TransformBltComplex()

===================================================================================
Transform properties

Description:
   These properties are used for various paramters of ITransform functions.

Definition:
   Flags for unTransform parameter of ITRANSFORM_TransformBltSimple:
   TRANSFORM_FLIP_X           Flip over x axis.
   TRANSFORM_ROTATE_90        Rotate 90 degrees counter-clockwise.
   TRANSFORM_ROTATE_180       Rotate 180 degrees counter-clockwise.
   TRANSFORM_ROTATE_270       Rotate 270 degrees counter-clockwise.
   TRANSFORM_SCALE_2          Scale by a factor of 2.
   TRANSFORM_SCALE_4          Scale by a factor of 4.
   TRANSFORM_SCALE_8          Scale by a factor of 8.
   TRANSFORM_SCALE_EIGHTH     Scale by a factor of 1/8.
   TRANSFORM_SCALE_QUARTER    Scale by a factor of 1/4.
   TRANSFORM_SCALE_HALF       Scale by a factor of 1/2.

   Values for unComposite parameter of ITRANSFORM_TransformBltSimple and
   ITRANSFORM_TransformBltComplex:
   COMPOSITE_KEYCOLOR         Do transparent blit.  This means that pixels that
                              the transparency color of the source bitmap will not
                              be drawn.
   COMPOSITE_OPAQUE           Do no (opaque) blit.

   All other composite values are reserved for future use.  An implementation should
   currently treat all undefined composite values as COMPOSITE_OPAQUE.

Comments:
   Note: All transformation use the source bitmap's center as the origin.

See Also:
   ITRANSFORM_TransformBltSimple(), ITRANSFORM_TransformBltComplex(),
   IBITMAP_SetTransparencyColor()

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================
ITransform Interface

Description:
   ITransform, like IBitmap, provides functions for accessing a bitmap.  This
   interface provides functions for doing blits with transformations.  It supports
   two kinds of transformation simple (with ITRANSFORM_TransformBltSimple()) and
   arbitrary affine transforms (with ITRANSFORM_TransformBltComplex()).

   To obtain an ITransform interface, you must already have a pointer to another
   interface of an existing bitmap object, such as an IBitmap interface.  You then
   need to call the object's IQI_QueryInterface() method with AEECLSID_TRANSFORM.
   This will give you the object's ITransform interface.

   Not all bitmap objects are required to export the ITransform interface.
   Device independent bitmaps (DIBs) do not support ITransform.  Device bitmaps
   and device compatible bitmaps are required to support it.


==============================================================================
Function: ITRANSFORM_AddRef()

Description:
This function is inherited from IQI_AddRef(). 


Version:
   Introduced BREW Client 2.0

See Also
   None

==============================================================================
Function: ITRANSFORM_Release()

Description:
This function is inherited from IQI_Release().

Version:
   Introduced BREW Client 2.0

See Also
   None

==============================================================================
Function: ITRANSFORM_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().

Version:
   Introduced BREW Client 2.0

See Also
   None

==============================================================================
Function: ITRANSFORM_TransformBltSimple()

Description:
   This functions blits a one bitmap to another, applying a set of simple,
   predefined transformations.

Prototype:
   int ITRANSFORM_TransformBltSimple(ITransform *po, int xDst, int yDst,
                                     IBitmap *pSrc, int xSrc, int ySrc,
                                     unsigned dxSrc, unsigned dySrc,
                                     uint16 unTransform, uint8 unComposite);

Parameters:
   po              [in]: Pointer to ITransform interface.

   xDst, yDst      [in]: Coordinate in destination where upper left corner
                         of source will be drawn.  NOTE: This assumes that
                         that no transform is being applied.  The location is
                         really specified by the center of the source area,
                         which is drawn at (xDst + dxSrc / 2, yDst + dySrc / 2).

   pSrc            [in]: Source bitmap.

   xSrc, ySrc      [in]: Upper left corner of source bitmap to be blitted to
                         destination.

   dxSrc, dySrc    [in]: Width and height of source bitmap area to be blitted.

   unTransform     [in]: Set of flags that specify transformation to perform.
                         See Transform properties.

   unComposite     [in]: See Transform properties.

Return Value:
   SUCCESS: if successful.
   EUNSUPPORTED: if blit is not supported.

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   Transform properties

==============================================================================
Function: ITRANSFORM_TransformBltComplex()

Description:

Prototype:
   int ITRANSFORM_TransformBltComplex(ITransform *po, int xDst, int yDst,
                                      IBitmap *pSrc, int xSrc, int ySrc,
                                      unsigned dxSrc, unsigned dySrc,
                                      const AEETransformMatrix *pMatrixTransform,
                                      uint8 unComposite);

Parameters:
   po              [in]: Pointer to ITransform interface.

   xDst, yDst      [in]: Coordinate in destination where upper left corner
                         of source will be drawn.  NOTE: This assumes that
                         that no transform is being applied.  The location is
                         really specified by the center of the source area,
                         which is drawn at (xDst + dxSrc / 2, yDst + dySrc / 2).

   pSrc            [in]: Source bitmap.

   xSrc, ySrc      [in]: Upper left corner of source bitmap to be blitted to
                         destination.

   dxSrc, dySrc    [in]: Width and height of source bitmap area to be blitted.

   pMatrixTransform[in]: Pointer to AEETranformMatrix structure that
                         specifies the transformation to be used.

   unComposite     [in]: See Transform properties.

Return Value:
   SUCCESS: if successful.
   EUNSUPPORTED: if blit is not supported.

Comments:
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEMatrixTransform, Transform properties

=================================================================================*/



#endif /* AEETRANSFORM_H */

