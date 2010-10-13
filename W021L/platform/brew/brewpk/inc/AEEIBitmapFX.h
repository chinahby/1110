#ifndef AEEIBITMAPFX_H
#define AEEIBITMAPFX_H
/*============================================================================
       Copyright (c) 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#include "AEEIQI.h"
#include "AEEIBitmap.h"

//Filter Types
#define AEEBITMAPFX_BLUR           0  // Low Pass filter  
#define AEEBITMAPFX_SHARP          1  // High Pass filter
#define AEEBITMAPFX_BLUR_GAUSSIAN  2  // Gaussian filter
#define AEEBITMAPFX_BLUR_UNIFORM   3  // Uniform filter

//Effect Types
#define AEEBITMAPFX_GRAYSCALE      0  // Convert image to grayscale 
#define AEEBITMAPFX_HUE            1  // Change the hue of the image
#define AEEBITMAPFX_NEGATIVE       2  // Create a negative of the image
#define AEEBITMAPFX_SATURATE       3  // Change the saturation of the image
#define AEEBITMAPFX_SEPIA          4  // Change the image to sepia
#define AEEBITMAPFX_POSTERIZE      5  // Posterize the image
#define AEEBITMAPFX_SOLARIZE       6  // Solarize the image
#define AEEBITMAPFX_FLASH          7  // Add smart flash to the image
#define AEEBITMAPFX_INTENSITY      9  // Change the intensity of the image
#define AEEBITMAPFX_FOG            10 // Add fog to the image
#define AEEBITMAPFX_NEON           11 // Change the image to neon
#define AEEBITMAPFX_WHITEBOARD     12 // Clear an image taken from whiteboard

/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/

#define AEEIID_IBitmapFX 0x0105f5ed

#define INHERIT_IBitmapFX(iname) \
   INHERIT_IQI(iname); \
   int         (*Effect)(iname * po, IBitmap* pinImg, IBitmap* poutImg, int32 nEffect, int32 nEffectValue );\
   int         (*Filter)(iname * po, IBitmap* pinImg, IBitmap* poutImg, int32 nFilter, int32 nBlurWindow);\
   int         (*MotionBlur)(iname * po, IBitmap* pinImg, IBitmap* poutImg, int32 nAngle, int32 nBlurWindow);\
   int         (*RadialBlur)(iname * po, IBitmap* pinImg, IBitmap* poutImg, int32 nLevel);\
   int         (*DrawLumaHistogram)(iname *po, IBitmap* pinImg, IBitmap* poutImg, AEERect* pRect)
   
AEEINTERFACE_DEFINE(IBitmapFX);

static __inline uint32 IBitmapFX_AddRef(IBitmapFX* pMe)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->AddRef(pMe);
}

static __inline uint32 IBitmapFX_Release(IBitmapFX *pMe)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Release(pMe);
}

static __inline int IBitmapFX_QueryInterface(IBitmapFX *pMe, AEECLSID clsid, void **pp)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->QueryInterface(pMe, clsid, pp);
}

static __inline int IBitmapFX_Effect(IBitmapFX *pMe, IBitmap* pinImg, IBitmap* poutImg, int32 neffect, int32 nEffectValue)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, neffect, nEffectValue);
}

static __inline int IBitmapFX_Filter(IBitmapFX *pMe, IBitmap* pinImg, IBitmap* poutImg, int32 nfilter, int32 nBlurWindow)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Filter(pMe, pinImg, poutImg, nfilter, nBlurWindow);
}

static __inline int IBitmapFX_MotionBlur(IBitmapFX *pMe, IBitmap* pinImg, IBitmap* poutImg, int32 nangle, int32 nBlurWindow)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->MotionBlur(pMe, pinImg, poutImg, nangle, nBlurWindow);
}

static __inline int IBitmapFX_RadialBlur(IBitmapFX *pMe, IBitmap* pinImg, IBitmap* poutImg, int32 nLevel)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->RadialBlur(pMe, pinImg, poutImg, nLevel);
}

static __inline int IBitmapFX_MakeGrayscale(IBitmapFX *pMe, IBitmap *pinImg, IBitmap  *poutImg)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_GRAYSCALE, 0);
}

static __inline int IBitmapFX_SetHue(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg, int32 nHue)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_HUE, nHue);
}

static __inline int IBitmapFX_MakeNegative(IBitmapFX *pMe, IBitmap *pinImg, IBitmap  *poutImg)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_NEGATIVE, 0);
}

static __inline int IBitmapFX_SetSaturation(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg, int32 nSaturation)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_SATURATE, nSaturation);
}

static __inline int IBitmapFX_MakeSepia(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_SEPIA, 0);
}

static __inline int IBitmapFX_SetPosterization(IBitmapFX *pMe, IBitmap *pinImg, IBitmap  *poutImg, int32 nPosterize)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_POSTERIZE, nPosterize);
}

static __inline int IBitmapFX_SetSolarization(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg, int32 nSolarize)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_SOLARIZE, nSolarize);
}

static __inline int IBitmapFX_SetIntensity(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg, int32 nIntensity)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_INTENSITY, nIntensity);
}

static __inline int IIBitmapFX_AddFlash(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_FLASH, 0);
}

static __inline int IBitmapFX_AddFog(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg, int32 nFog)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_FOG, nFog);
}

static __inline int IBitmapFX_MakeNeon(IBitmapFX *pMe, IBitmap *pinImg, IBitmap *poutImg)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->Effect(pMe, pinImg, poutImg, AEEBITMAPFX_NEON, 0);
}
static __inline int IBitmapFX_DrawLumaHistogram(IBitmapFX *pMe, IBitmap* pinImg, IBitmap* poutImg, AEERect* pRect)
{
  return AEEGETPVTBL(pMe, IBitmapFX)->DrawLumaHistogram(pMe, pinImg, poutImg, pRect);
}


/*===========================================================================
                       INTERFACES DOCUMENTATION
===========================================================================
Interface Name: IBitmapFX

Description:
The IBitmapFX interface which is used to perform the image processing interface 
of bitmaps.It exposes the image processing layer functions to a BREW 
Application.
The IBitmapFX provides a BREW application interface to image processing funtionalities
such as posterize ,solarize, magnify etc.

=============================================================================
                          CALL FUNCTIONS
=============================================================================
FUNCTION: IBitmapFX_AddRef()

DESCRIPTION:
  This function is inherited from IQI_AddRef().

SEE ALSO:
  IBitmapFX_Release()
=============================================================================
FUNCTION: IBitmapFX_Release()

DESCRIPTION:
  This function is inherited from IQI_Release().

SEE ALSO:
  IBitmapFX_AddRef()
=============================================================================
FUNCTION: IBitmapFX_QueryInterface()

DESCRIPTION:
  This function is inherited from IQI_QueryInterface().

SEE ALSO:
  IBitmapFX_AddRef() and IBitmapFX_Release()

=============================================================================
                    SPECIAL EFFECTS
=============================================================================

IBitmapFX_MakeGrayscale()

Description:
    This function converts the input image into a grayscale image
  
Prototype:
    int IBitmapFX_MakeGrayscale
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    me[in]:  Pointer to IBitmapFX Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image.

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
   
    
Comments:

    The size of the output image should be the same size as the input image. The colorScheme of the 
    output image needs to be set by the user.

See Also:
    
===============================================================================

IBitmapFX_MakeNeon()

Description:
    This function converts the input image into a neon image

Prototype:
    int IBitmapFX_MakeNeon
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    me[in]:  Pointer to IBitmapFX Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
 
Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    
Comments:

    The size of the output image should be the same size as the input image.
    The colorScheme of the output image needs to be set by the user.

  
See Also:
    None

===============================================================================

IBitmapFX_SetIntensity()

Description:
    This function scales the intensity of the input image. The user can increase or decrease the intensity
    of the image. The intensity scaling factor can range from -255 to 255

Prototype:
    int IBitmapFX_SetIntensity
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 intensity
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    intensity[in]: intensity scaling factor ranging between -255 to 255

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the intensity scaling factor is out of the permitted range
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

==============================================================================

IBitmapFX_SetSaturation()

Description:
    This function scales the saturation of the image. The user can increase or decrease the saturation
    of the image. The saturation scaling factor can range from -255 to 255

Prototype:
    int IBitmapFX_SetSaturation
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 saturation
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    saturation[in]: saturation value ranging between -255 to 255

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the saturation factor is outside the specified range [-255..255]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================

IBitmapFX_SetHue()

Description:
    This function scales the hue of the input image. The user can increase or decrease the hue factor 
    of the image. The hue scaling factor can range from -360 to 360

Prototype:
    int IBitmapFX_SetHue
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 hue
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    hue[in]: The hue factor ranging between -360 to 360

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the hue factor is outside the permitted range [-360 .. 360]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

===============================================================================

IBitmapFX_SetSolarization()

Description:
    This function changes the solarization factor of the input image. The user can increase or decrease 
    the solarization of the image. 

Prototype:
    int IBitmapFX_SetSolarization
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 solarize
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    solarize[in]: Solarization threshold value ranging from 0 to 255

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the solarization threshold value is outside the specified range [0..255]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

===============================================================================

IBitmapFX_SetPosterization()

Description:
    This function changes the posterization of the input image. The user can set the posterization
    of the image. The posterization value can range from 0 to 255

Prototype:
    int IBitmapFX_SetPosterization
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 posterize
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    posterize[in]: the posterization value ranging from 0 to 255

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the posterization factor is outside the specified range [0..255]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.
    If posterize value is 0 then it do nothing and return original image.
    
See Also:
    None

===============================================================================

IBitmapFX_MakeSepia()

Description:
    This function converts the input image into a sepia image

Prototype:
    int IBitmapFX_MakeSepia
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

===============================================================================

IBitmapFX_MakeNegative()

Description:
    This function converts the input image into a negative image

Prototype:
    int IBitmapFX_MakeNegative
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the hue factor is outside the specified range [-360..360]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

===============================================================================

IBitmapFX_AddFog()

Description:
    This function adds a foggy effect to the input image. The user specifies the level of
    fogginess, either light (0) or heavy (1).

Prototype:
    int IBitmapFX_AddFog
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 nfog
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    nfog[in]: nfog level, either light (0) or heavy (1)

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, otherwise
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
  None

===============================================================================

IBitmapFX_AddFlash()

Description:
    This function adds the flash effect to the input image. Thus, the output image
    will be one in case flash had been used while taking the input image

Prototype:
    int IBitmapFX_AddFlash
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully

    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

===============================================================================

IBitmapFX_Filter()

Description:
    This function filters the input image. There are 6 types of filters that can be applied to 
    an input image.

    1) BLUR : It'll result in the blurring of the input image
    2) SHARPEN: It'll result in the sharpening of the input image
    3) GAUSSIAN : It'll add a gaussian blur to the input image
    4) UNIFORM : It'll add a uniform blur to the input image  

    The blurring is done in incrementally. The blur window specifies the rectangular area 
    which is blurred at a time. The blur window can range from [ 3 .. 0.5 * min (input height, width)]
    It will do nothing when nfilter = 0 ,nBlurWindow =9  and nfilter = 0 ,nBlurWindow =30.

Prototype:
    int IBitmapFX_Filter
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 nfilter,
      int32 nBlurWindow
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    filter[in]: Type of filtering operation to be applied to the input image
    nBlurWindow[in]: Size of the blurring Window

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the filter type is not one of the permitted values or the blur Window 
              falls outside the range

Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

===============================================================================

IBitmapFX_MotionBlur()
 
Description:
  This function produces motion blur in any arbitrary direction. The size 
  of the blurring window must be an odd, positive integer in the range 
  3 <= size <= 1/2 * min(w, h), where w and h are the width and height of 
  the input image. The direction of motion is specified by a positive integer 
  angle measured in degrees from the horizontal.

Prototype:
    int IBitmapFX_MotionBlur
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 nBlurWindow,
      int32 nangle
    )

Parameters:
    me[in]:  Pointer to me Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    nBlurWindow[in]: blurWindow Size
    nangle[in]: Angle in which the motion blur is to be applied.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the blurWindow is not a positive odd integer.
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

===============================================================================

IBitmapFX_RadialBlur()
 
Description:
    
  This function performs radial blur. The blurring level can be set 
  to an integer between 0 and 100, inclusive. No blurring occurs 
  when the blurring level is 0.

Prototype:
    int IBitmapFX_RadialBlur
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      uint8 nLevel
    )

Parameters:
    me[in]:  Pointer to IBitmapFX Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    nLevel[in]: Level of radial blurring needed.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the blurring level is outside the permitted range
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    
===============================================================================

IBitmapFX_Effect()

Description:

  This function adds special effects to the input image. The type of effect to be 
  applied is determined by the effect type passed in to the function.


Prototype:
    int IBitmapFX_Effect
    (
      IBitmapFX * me,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 neffectType,
      int32 nEffectValue
    )

Parameters:
    me[in]:  Pointer to IBitmapFX Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    effectType[in]: int32 value indicating the type of effect to be applied
    effectValue[in]: value of the effect

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the neffectType is not one of the permitted transformations or if
             the effectValue is not one of the permitted values (dependent on effectType)
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
 IBitmapFX_MakeGrayscale()
 IBitmapFX_MakeNeon()
 IBitmapFX_MakeSepia()
 IBitmapFX_AddFog()
 IBitmapFX_AddFlash()
 IBitmapFX_SetSaturation()
 IBitmapFX_SetPosterization()
 IBitmapFX_SetSolarization()
 IBitmapFX_SetIntensity()
 IBitmapFX_SetHue()
 IBitmapFX_MakeNegative()
=============================================================================*/
#endif /* AEEIBITMAPFX_H */
