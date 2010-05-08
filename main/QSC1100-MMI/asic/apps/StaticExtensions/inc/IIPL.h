#ifndef IIPL_H
#define IIPL_H

/*============================================================================
FILE:	IIPL.h

SERVICES:  BREW IPL Interface

DESCRIPTION:
   This file defines the IIPL interface which is used to perform the image processing 
   on the bitmaps. It exposes the image processing layer functions to a BREW Application.
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

============================================================================*/
/*==============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEEBitmap.h"

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

#define AEEIID_IPL    	0x0102eae0

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

enum
{
    IIPL_NO_COLOR               = 0x00000000,
    IIPL_YCBCR                  = 0x00000001,   // YCbCr pixel color format 4:2:2
    IIPL_YCBCR_LINE420          = 0x00000002,   // YCbCr 4:2:0 Line Packed format
    IIPL_YCBCR_LINE422          = 0x00000003,   // YCbCr 4:2:2 Line Packed format
    IIPL_YCBCR_LINE444          = 0x00000004,   // YCbCr 4:4:4 Line Packed format
    IIPL_RGB565                 = 0x00000005,   // RGB565 format (2 bytes per pixel)
    IIPL_RGB888                 = 0x00000006,   // RGB888 format (3 bytes per pixel)
    IIPL_RGB666                 = 0x00000007,   // RGB666 format
    IIPL_RGB444                 = 0x00000008,   // RGB444 format
    IIPL_MAX_COLOR_VAL          = 0x00000009
};
typedef uint32 IIPLColorType;

enum
{
    IIPL_PERSPECTIVE_UP         = 0x00000000,   // Perspective Trapezoid with the smaller edge upwards
    IIPL_PERSPECTIVE_DOWN       = 0x00000001,   // Perspective Trapezoid with the smaller edge downwards
    IIPL_PERSPECTIVE_LEFT       = 0x00000002,   // Perspective Trapezoid with the smaller edge to the left
    IIPL_PERSPECTIVE_RIGHT      = 0x00000003,   // Perspective Trapezoid with the smaller edge to the right
    IIPL_MAX_PERSPECTIVE_VAL    = 0x00000004
};
typedef uint32 IIPLPerspectiveType;

enum
{
    IIPL_NO_ROTATE              = 0x00000000,   // No rotation    
    IIPL_ROTATE_90	        = 0x00000001,   // Rotate by 90 degrees
    IIPL_ROTATE_180	        = 0x00000002,   // Rotate by 180 degrees          
    IIPL_ROTATE_270	        = 0x00000003,   // Rotate by 270 degrees
    IIPL_MAX_ROTATE_VAL         = 0x00000004
};
typedef uint32 IIPLRotateType;

enum
{
    IIPL_NO_REFLECT             = 0x00000000,   // Don't reflect    
    IIPL_REFLECT_X	        = 0x00000001,   // Reflect along the X-Axis
    IIPL_REFLECT_Y	        = 0x00000002,   // Reflect along the Y-Axis
    IIPL_MAX_REFLECT_VAL        = 0x00000003
};
typedef uint32 IIPLReflectType;

enum
{
    IIPL_BLUR                   = 0x00000000,   // Low Pass filter   
    IIPL_SHARP	                = 0x00000001,   // High Pass filter
    IIPL_BLUR_GAUSSIAN	        = 0x00000002,   // Gaussian filter
    IIPL_BLUR_UNIFORM	        = 0x00000003,   // Uniform filter
    IIPL_MAX_FILTER_VAL         = 0x00000004
};
typedef uint32 IIPLFilterType;

enum
{
    IIPL_REFLECT                = 0x00000000,   // Reflect the image about an axis
    IIPL_ROTATE                 = 0x00000001,   // Rotate the image 
    IIPL_MAX_TRANSFORM_TYPE     = 0x00000002
};
typedef uint32 IIPLTransformType;

enum
{
    IIPL_BLEND                  = 0x00000000,   // Blend between two images
    IIPL_CENTER                 = 0x00000001,   // Add an image in the center of the other
    IIPL_FILMSTRIP              = 0x00000002,   // Add the beginning of an image to the end of the first.
    IIPL_HORIZONTAL             = 0x00000003,   // Add the left/right of an image to the left/right of the other image
    IIPL_VERTICAL               = 0x00000004,   // Add one image to the top/bottom of the other image
    IIPL_MAX_COMPOSE_VAL        = 0x00000005
};
typedef uint32 IIPLComposeType;

enum
{
    IIPL_GRAYSCALE              = 0x00000000,     // Convert image to grayscale
    IIPL_HUE                    = 0x00000001,     // Change the hue of the image
    IIPL_NEGATIVE               = 0x00000002,     // Create a negative of the image
    IIPL_SATURATE               = 0x00000003,     // Change the saturation of the image
    IIPL_SEPIA                  = 0x00000004,     // Change the image to sepia
    IIPL_POSTERIZE              = 0x00000005,     // Posterize the image
    IIPL_SOLARIZE               = 0x00000006,     // Solarize the image
    IIPL_FLASH                  = 0x00000007,     // Add smart flash to the image
    IIPL_INTENSITY              = 0x00000008,     // Change the intensity of the image
    IIPL_FOG                    = 0x00000009,     // Add fog to the image
    IIPL_NEON                   = 0x0000000A,     // Change the image to neon.
    IIPL_WHITEBOARD             = 0x0000000B,     // Clear an image taken from whiteboard
    IIPL_MAX_EFFECT_VAL         = 0x0000000C
};
typedef uint32 IIPLEffectType;

// This structure is used to provide the icons and the location on the screen at which they need to be placed.
typedef struct IIPLIconInfo
{
  IBitmap* iconBmp;         // The bitmap of the icon 
  AEERect  iconRect;        // The location of the icon on the image
}IIPLIconInfo;

// This structure is used to represent a straight line.
typedef struct IIPLLine
{
 int16 sX;                 // X Co-ordinate of the starting point.
 int16 sY;                 // Y Co-ordinate of the starting point.
 int16 eX;                 // X Co-ordinate of the ending point.
 int16 eY;                 // Y Co-orindate of the ending point.
}IIPLLine;

// This structure is used to represent a cirlce
typedef struct IIPLCircle
{
  int16 cX;                // X Co-ordinate of the center
  int16 cY;                // Y Co-ordinate of the center
  uint16 radius;           // Radius of the circle
}IIPLCircle;


// This structure is used to specify the grid that needs to be passed in for the warp effect
typedef struct IIPLGrid
{
  uint16* rowElements;    // Points to the array containing the list of the beginning Y-cordinates of each row.
  uint16* columnElement;  // Points to the array containing the list of the beginning X-cordinates of each column.
  uint16 numElements;     // Specifies the number of rows and columns in the Grid
}IIPLGrid;


//*****************************************************************************
//
//    Some Sample Grids for Warp Effects 
//
//    1) To achieve the effect of stretching the top of an image
//        {
//          11,
//          0,5,15,40,60,70,80,85,90,95,100
//          0,10,20,30,40,50,60,70,80,90,100
//       };
//      
//      2) To achieve the effect of pinching the center of the picture
//      {
//        11,
//        0,15,30,40,46,50,66,70,80,90,100
//        0,10,20,30,40,50,60,70,80,90,100
//      };
//
//
//
//
//*****************************************************************************



//*****************************************************************************
//
// IIpl Interface
//
//*****************************************************************************
typedef struct IIpl IIpl;

#define INHERIT_IIpl(iname) \
   INHERIT_IQueryInterface(iname); \
   int         (*Transform)(iname * po, IBitmap* i_image, IBitmap* o_image, IIPLTransformType xform, uint32 xFormValue); \
   int         (*ConvertColor)(iname * po, IBitmap* i_image, IBitmap* o_image); \
   int         (*Effect)(iname * po, IBitmap* i_image, IBitmap* o_image,IIPLEffectType effect, int32 effectValue );\
   int         (*Compose)(iname * po, IBitmap* i_image, IBitmap* i_image2, IBitmap* o_image, IIPLComposeType p1, int32 boundary);\
   int         (*CopyPaste)(iname * po, IBitmap* i_image, IBitmap* o_image, AEERect* rcCut, AEERect* rcPaste);\
   int         (*RotateFrame)(iname * po, IBitmap* i_image, IBitmap* o_image,IIPLRotateType p1);\
   int         (*ArbitRotate)(iname * po, IBitmap* i_image, IBitmap* o_image,int16 angle, uint16 fillerPixel);\
   int         (*ConvertFrame)(iname * po, IBitmap* i_frame, IBitmap* o_frame);\
   int         (*Filter)(iname * po, IBitmap* i_image, IBitmap* o_image,IIPLFilterType filter, uint32 blurWindow);\
   int         (*ComposeCanvas)(iname * po, IBitmap* i_image, IBitmap* o_image,IIPLIconInfo* icon_list, uint32 listSize);\
   int         (*RotateCropAddFrame)(iname * po, IBitmap* i_image, IBitmap* i_frame, IBitmap* o_image, \
                                     IIPLIconInfo* icon_list, uint32 listSize, \
                                     AEERect* cropRect, IIPLRotateType rotate);\
   int         (*ChangeSize)(iname * po, IBitmap* i_image, IBitmap* o_image);\
   int         (*ViewPerspective)(iname * po, IBitmap* i_image, IBitmap* o_image,\
                                   IIPLLine* line, uint16 fillerPixel, IIPLPerspectiveType orientation);\
   int         (*Magnify)(iname * po, IBitmap* i_image, IBitmap* o_image, IIPLCircle* circle);\
   int         (*MotionBlur)(iname * po, IBitmap* i_image, IBitmap* o_image, uint32 angle, uint32 blurWindow);\
   int         (*RadialBlur)(iname * po, IBitmap* i_image, IBitmap* o_image, uint32 level);\
   int         (*Overlap)(iname * po, IBitmap* i_image, IBitmap* i_image2, IBitmap* o_image,AEERect* rcOverlap);\
   int         (*Warp)(iname * po, IBitmap* i_image, IBitmap* o_image, IIPLGrid* grid, boolean gridLoc);\
   int         (*CutPaste)(iname * po, IBitmap* i_image, IBitmap* o_image,AEERect* rcCut, AEERect* rcPaste, uint16 fillerPixel);\
   int         (*DrawHistogram)(iname *po, IBitmap* i_image, IBitmap* o_image, AEERect* pRect);\
   int         (*DrawRgbHistogram)(iname *po, IBitmap* i_image, IBitmap* o_image, AEERect* pRect);\
   int         (*RemoveRedEye)(iname *po, IBitmap* i_image, IBitmap* o_image, AEERect rcCut, AEERect rcPaste)

AEEINTERFACE(IIpl)
{
  INHERIT_IIpl(IIpl);
};


#define IIPL_AddRef(p)                                          AEEGETPVTBL((p), IIpl)->AddRef((p))
#define IIPL_Release(p)                                         AEEGETPVTBL((p), IIpl)->Release((p))
#define IIPL_QueryInterface(p, clsid, pp)                       AEEGETPVTBL((p), IIpl)->QueryInterface((p), clsid, pp)


#define IIPL_CopyPaste(p,i1,o1,p1,p2)                       AEEGETPVTBL((p), IIpl)->CopyPaste((p),i1,o1,p1,p2) 
#define IIPL_RotateFrame(p,i1,o1,p1)                        AEEGETPVTBL((p), IIpl)->RotateFrame((p),i1,o1,p1) 
#define IIPL_ArbitRotate(p,i1,o1,p1,p2)                     AEEGETPVTBL((p), IIpl)->ArbitRotate((p),i1,o1,p1,p2) 
#define IIPL_Filter(p,i1,o1,p1,p2)                          AEEGETPVTBL((p), IIpl)->Filter((p),i1,o1,p1,p2)
#define IIPL_ConvertFrame(p,i1,o1)                          AEEGETPVTBL((p), IIpl)->ConvertFrame((p),i1,o1)
#define IIPL_ComposeCanvas(p,i1,o1,p1,p2)                   AEEGETPVTBL((p), IIpl)->ComposeCanvas((p),i1,o1,p1,p2)
#define IIPL_RotateCropAddFrame(p,i1,i2,o1,p1,p2,p3,p4)     AEEGETPVTBL((p), IIpl)->RotateCropAddFrame((p),i1,i2,o1,p1,p2,p3,p4)
#define IIPL_ChangeSize(p,i1,o1)                            AEEGETPVTBL((p), IIpl)->ChangeSize((p),i1,o1)
#define IIPL_ViewPerspective(p,i1,o1,p1,p2,p3)              AEEGETPVTBL((p), IIpl)->ViewPerspective((p),i1,o1,p1,p2,p3)
#define IIPL_ConvertColor(p,i1,o1)                          AEEGETPVTBL((p), IIpl)->ConvertColor((p),i1,o1)
#define IIPL_Magnify(p,i1,o1,p1)                            AEEGETPVTBL((p), IIpl)->Magnify((p),i1,o1,p1)
#define IIPL_MotionBlur(p,i1,o1,p1,p2)                      AEEGETPVTBL((p), IIpl)->MotionBlur((p),i1,o1,p1,p2)
#define IIPL_RadialBlur(p,i1,o1,p1)                         AEEGETPVTBL((p), IIpl)->RadialBlur((p),i1,o1,p1)
#define IIPL_Overlap(p,i1,i2,o1,p1)                         AEEGETPVTBL((p), IIpl)->Overlap((p),i1,i2,o1,p1)
#define IIPL_Warp(p,i1,o1,p1,p2)                            AEEGETPVTBL((p), IIpl)->Warp((p),i1,o1,p1,p2)

#define IIPL_Transform(p,i1,o1,p1,p2)                       AEEGETPVTBL((p), IIpl)->Transform((p),i1,o1,p1,p2)
#define IIPL_Reflect(p,i1,o1,p2)                            AEEGETPVTBL((p), IIpl)->Transform((p),i1,o1,IIPL_REFLECT,p2)
#define IIPL_Rotate(p,i1,o1,p2)                             AEEGETPVTBL((p), IIpl)->Transform((p),i1,o1,IIPL_ROTATE,p2)

#define IIPL_Effect(p,i1,o1,p1,p2)                          AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,p1,p2)
#define IIPL_MakeGrayscale(p,i1,o1)                         AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_GRAYSCALE,0) 
#define IIPL_SetHue(p,i1,o1,p2)                             AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_HUE,p2)
#define IIPL_MakeNegative(p,i1,o1)                          AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_NEGATIVE,0)
#define IIPL_SetSaturation(p,i1,o1,p2)                      AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_SATURATE,p2)
#define IIPL_MakeSepia(p,i1,o1)                             AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_SEPIA,0)
#define IIPL_SetPosterization(p,i1,o1,p2)                   AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_POSTERIZE,p2)
#define IIPL_SetSolarization(p,i1,o1,p2)                    AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_SOLARIZE,p2)
#define IIPL_SetIntensity(p,i1,o1,p2)                       AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_INTENSITY,p2)
#define IIPL_AddFlash(p,i1,o1)                              AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_FLASH,0)
#define IIPL_AddFog(p,i1,o1,p2)                             AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_FOG,p2)
#define IIPL_MakeNeon(p,i1,o1)                              AEEGETPVTBL((p), IIpl)->Effect((p),i1,o1,IIPL_NEON,0)


#define IIPL_Compose(p,i1,i2,o1,p1,p2)                      AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,p1,p2)
#define IIPL_Blend(p,i1,i2,o1,p2)                           AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,IIPL_BLEND,p2)
#define IIPL_Center(p,i1,i2,o1,p2)                          AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,IIPL_CENTER,p2)
#define IIPL_FilmStrip(p,i1,i2,o1,p2)                       AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,IIPL_FILMSTRIP,p2)
#define IIPL_AddHorizontal(p,i1,i2,o1,p2)                   AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,IIPL_HORIZONTAL,p2)
#define IIPL_AddVertical(p,i1,i2,o1,p2)                     AEEGETPVTBL((p), IIpl)->Compose((p),i1,i2,o1,IIPL_VERTICAL,p2)

#define IIPL_CutPaste(p,i1,o1,p1,p2,p3)                     AEEGETPVTBL((p), IIpl)->CutPaste((p),i1,o1,p1,p2,p3) 
#define IIPL_AddFrame(p,i1,i2,o1)                           AEEGETPVTBL((p), IIpl)->RotateCropAddFrame((p),i1,i2,o1,NULL,NULL,NULL,0)
#define IIPL_Crop(p,i1,o1,p1)                               AEEGETPVTBL((p), IIpl)->RotateCropAddFrame((p),i1,NULL,o1,NULL,NULL,p1,0)
#define IIPL_DrawHistogram(p,i1,o1,p1)                      AEEGETPVTBL((p), IIpl)->DrawHistogram((p),i1,o1,p1)
#define IIPL_DrawRgbHistogram(p,i1,o1,p1)                   AEEGETPVTBL((p), IIpl)->DrawRgbHistogram((p),i1,o1,p1)
#define IIPL_RemoveRedEye(p,i1,o1,p1,p2)                    AEEGETPVTBL((p), IIpl)->RemoveRedEye((p),i1,o1,p1,p2) 

/*===========================================================================

                       INTERFACES DOCUMENTATION

===========================================================================*/

/*---------------------------------------------------------------------------
                          CALL FUNCTIONS
---------------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: IIPL_AddRef

DESCRIPTION:
  Increments the reference count for the IIPL object.  This enables
  the IIPL object to determine when the last reference is removed and
  cleanup the object's resources.

PROTOTYPE:
  uint32 IIPL_AddRef(IIpl *pMe);


PARAMETERS:
  IIpl *pMe:            Pointer to the IIPL interface

RETURN VALUE:
  Returns the incremented reference count.  (Returns zero if the
  object does not exist.)

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IIPL_Release

=============================================================================*/

/*=============================================================================
FUNCTION: IIPL_Release

DESCRIPTION:
  Decrements the reference count for the IIPL object.  When no more
  references exist, it releases all of the object's resources.

PROTOTYPE:
  uint32 IIPL_Release(IIpl *pMe);

PARAMETERS:
  IIpl *pMe:            Pointer to the IIPL interface

RETURN VALUE:
  Returns the decremented reference count for the object.  If the object
  does not exist, or if the call releases the last reference, the return
  value is zero.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
  IIPL_AddRef

=============================================================================*/

/*=============================================================================
  
                    SPECIAL EFFECTS

=============================================================================*/

/*=============================================================================
IIPL_MakeGrayscale()

Description:
    This function converts the input image into a grayscale image
  
Prototype:
    int IIPL_MakeGrayscale
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image.

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
   
    
Comments:

    The size of the output image should be the same size as the input image. The colorScheme of the 
    output image needs to be set by the user.

See Also:
    

=============================================================================*/

/*=============================================================================
IIPL_MakeNeon()

Description:
    This function converts the input image into a neon image

Prototype:
    int IIPL_MakeNeon
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/
/*=============================================================================
IIPL_SetIntensity()

Description:
    This function scales the intensity of the input image. The user can increase or decrease the intensity
    of the image. The intensity scaling factor can range from -255 to 255

Prototype:
    int IIPL_SetIntensity
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 intensity
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/

/*=============================================================================

IIPL_SetSaturation()

Description:
    This function scales the saturation of the image. The user can increase or decrease the saturation
    of the image. The saturation scaling factor can range from -255 to 255

Prototype:
    int IIPL_SetSaturation
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 saturation
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/

/*=============================================================================

IIPL_SetHue()

Description:
    This function scales the hue of the input image. The user can increase or decrease the hue factor 
    of the image. The hue scaling factor can range from -360 to 360

Prototype:
    int IIPL_SetHue
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 hue
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/

/*=============================================================================
IIPL_SetSolarization()

Description:
    This function changes the solarization factor of the input image. The user can increase or decrease 
    the solarization of the image. 

Prototype:
    int IIPL_SetSolarization
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      uint32 solarize
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/
/*=============================================================================

IIPL_SetPosterization()

Description:
    This function changes the posterization of the input image. The user can set the posterization
    of the image. The posterization value can range from 1 to 255

Prototype:
    int IIPL_SetPosterization
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 posterize
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    posterize[in]: the posterization value ranging from 1 to 255

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the posterization factor is outside the specified range [1..255]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/

/*=============================================================================

IIPL_MakeSepia()

Description:
    This function converts the input image into a sepia image

Prototype:
    int IIPL_MakeSepia
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/
/*=============================================================================


IIPL_MakeNegative()

Description:
    This function converts the input image into a negative image

Prototype:
    int IIPL_MakeNegative
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/

/*=============================================================================

IIPL_AddFog()

Description:
    This function adds a foggy effect to the input image. The level of fogginess can
    range from 0..255

Prototype:
    int IIPL_AddFog
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      uint32 fog
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    fog[in]: level of fogginess to be added. 

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the fog level isn't in the specified range [0 .. 255]
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
  None

=============================================================================*/

/*=============================================================================


IIPL_AddFlash()

Description:
    This function adds the flash effect to the input image. Thus, the output image
    will be one in case flash had been used while taking the input image

Prototype:
    int IIPL_AddFlash
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
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

=============================================================================*/
/*=============================================================================

IIPL_Magnify()

Description:
    This function magnifies a circular area of the input image. This area will appear
    as being viewed under a magnifying glass.

Prototype:
    int IIPL_Magnify
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLCircle* circle
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    circle[in]: Pointer to the IIPLCircle type specifying the circular area to magnify

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully

    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.


See Also:
    IIPLCircle

=============================================================================*/

/*=============================================================================

IIPL_Rotate()

Description:
    This function rotates the input image. There are 4 degrees of rotation as specified by IIPLRotateType.
    1) Zero degrees - IIPL_NO_ROTATE
    2) 90 degrees - IIPL_ROTATE_90
    3) 180 degrees - IIPL_ROTATE_180
    4) 270 degrees - IIPL_ROTATE_270


Prototype:
    int IIPL_Rotate
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLRotateType rotate
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    rotate[in]: Rotation type

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the rotate value provided is not one of the permitted values
    
Comments:

    The size of the output image should be appropriate. 
    For e.g. if rotating a 160*120 image, the output image should be of size 120*160.
    
    The colorScheme of the output image needs to be set by the user.

See Also:
    IIPLRotateType

=============================================================================*/
/*=============================================================================

IIPL_Reflect()

Description:
    This function creates a reflection effect on the input image. The input image can be reflected 
    about the X-Axis or it could be reflected about the Y-axis

    'reflect' can take one of three values : IIPL_REFLECT_NONE, IIPL_REFLECT_X, IIPL_REFLECT_Y

Prototype:
    int IIPL_Reflect
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLReflectType reflect
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    reflect[in]: reflection parameter

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the reflect value provided is not one of the permitted values
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    IIPLReflectType

=============================================================================*/

/*=============================================================================

IIPL_ChangeSize()

Description:
    This function is used to change the size of the input image. 

    The scaling of the image can be arbitrary, i.e. we can increase the width and decrease the height or vice-versa

Prototype:
    int IIPL_ChangeSize
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully

    
Comments:
    The colorScheme of the output image needs to be set by the user.


See Also:
    None
=============================================================================*/

/*=============================================================================

IIPL_Filter()

Description:
    This function filters the input image. There are 6 types of filters that can be applied to 
    an input image.

    1) BLUR : It'll result in the blurring of the input image
    2) SHARPEN: It'll result in the sharpening of the input image
    3) GAUSSIAN : It'll add a gaussian blur to the input image
    4) UNIFORM : It'll add a uniform blur to the input image  

    The blurring is done in incrementally. The blur window specifies the rectangular area 
    which is blurred at a time. The blur window can range from [ 3 .. 0.5 * min (input height, width)]

Prototype:
    int IIPL_Filter
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLFilterType filter,
      uint32 blurWindow
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    filter[in]: Type of filtering operation to be applied to the input image
    blurWindow[in]: Size of the blurring Window

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the filter type is not one of the permitted values or the blur Window 
              falls outside the range

Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    IIPLFilterType

=============================================================================*/
/*=============================================================================

IIPL_Blend()

Description:
  This function performs alpha blending of the two input images. The user specifies the alpha 
  blending factor for the first input image. The blending factor for the second input is (100 - alpha)%. 

  Alpha must be between 0 and 100, inclusive.
  
Prototype:
    int IIPL_Blend
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      int32 alpha
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    alpha[in]: alpha blend factor applicable to the first image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the alpha factor is not in the permitter range [0..100]

Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/
/*=============================================================================

IIPL_Center()

Description:
  This function creates an output image which is a combination of two images 
  by taking the center part of the first image and the left and right parts of 
  the second image. The width indicates the width of the center region taken from 
  the first image.
  Examples:
    (1) if width = 0, output = second image only;
    (2) if width = input width, output = first image only;
    (3) if width = 1/2 of input width, output contains center half of 
        first image.
  
Prototype:
    int IIPL_Center
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      uint32 width
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    width[in]: width of the first input image to be put in the center of the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the width value lies outside the permitted range [0 .. input image width]

Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.
    
See Also:
    None

=============================================================================*/
/*=============================================================================

IIPL_FilmStrip()

Description:
  This function combines the end part of the first image with the 
  beginning part of the second image to simulate the effect of a 
  moving film strip. The boundary indicates where the second image 
  starts.
  Examples:
    (1) if boundary = 0, output = second image only;
    (2) if boundary = input width, output = first image only.

  Input and output images must have the same dimensions. 
  Input and output images must have the same color format, which can 
  be RGB565, YCbCr 4:2:2, or YCbCr 4:2:0 line packed.


Prototype:
    int IIPL_FilmStrip
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      uint32 boundary
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    boundary[in]: boundary of the first input image where the second input image is attached
                  create the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the boundary is not within the permitted range [0 .. input image width]

Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/
/*=============================================================================

IIPL_AddHorizontal()

Description:
  This function combines two images by taking the left part of the first 
  image and the right part of the second image. The boundary indicates where
  the second image starts.  
  Examples:
    (1) if boundary = 0, output = second image only;
    (2) if boundary = input width, output = first image only.

  Input and output images must have the same dimensions.
  Input and output images must have the same color format, which 
  can be RGB565, YCbCr 4:2:2, or YCbCr 4:2:0 line packed.


Prototype:
    int IIPL_Horizontal
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      uint32 boundary
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    boundary[in]: boundary of the first image where the second image is attached

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the boundary is not within the permitted range [0 .. input image width]


Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/
/*=============================================================================

IIPL_AddVertical()

Description:
  This function combines two images by taking the top part of the first image 
  and the bottom part of the second image. The boundary indicates where the 
  second image starts.  
  Examples:
    (1) if boundary = 0, output = second image only;
    (2) if boundary = input height, output = first image only.
  
  Input and output images must have the same dimensions.
  Input and output images must have the same color format, which can be 
  RGB565, YCbCr 4:2:2, or YCbCr 4:2:0 line packed.


Prototype:
    int IIPL_AddVertical
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      uint32 height
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    height[in]: the height boundary at which the second input image is attached to the first

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the boundary is not within the permitted range [0 .. input image height]

    
Comments:

    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/
/*=============================================================================

IIPL_Overlap()

Description:
  This function overlaps one image on top of another. The overlap region may 
  be placed anywhere as long as it fits within the boundaries of the first 
  input image. If the overlap region is smaller than the second input image, 
  the second image will be downsized. The output image must have the same 
  dimensions as the first input image. 

Prototype:
    int IIPL_Overlap
    (
      IIpl * pIIpl,
      IBitmap* inImage1,
      IBitmap* inImage2,
      IBitmap* outImage,
      AEERect* rect
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage1[in]: Pointer to the first input image 
    inImage2[in]: Pointer to the second input image
    outImage[in/out]: Pointer to the output image
    rect[in]: Pointer to the rectangular area indicating the overlap region of the first image to
              be filled with the second image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the overlap region is not within the input image area
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/

/*=============================================================================


IIPL_ConvertFrame()

Description:
  This function will do frame color conversion. It ensures that the 
  transparent pixel value is unique after color conversion.
  
  Input frame and output frame have the same color format and same dimensions

Prototype:
    int IIPL_ConvertFrame
    (
      IIpl * pIIpl,
      IBitmap* inFrame,
      IBitmap* outFrame
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inFrame[in]: Pointer to the input frame
    outFrame[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    
Comments:
    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None

=============================================================================*/

/*=============================================================================

IIPL_ConvertColor()

Description:
  This function will perform color conversion of the image. The user can specify the 
  desired color format of the output image and this function will return an output image
  with that color format to the user.
  
Prototype:
    int IIPL_ConvertColor
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    
Comments:
    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    

=============================================================================*/

/*=============================================================================


IIPL_ViewPerspective()

Description:
  This function creates a perspective image shaped like a trapezoid.  
  The user specifies 2 input parameters: (1) the line defining the top 
  of the trapezoid, (2) a pixel value to fill in blank output regions 
  outside of the trapezoid (3) orientation of the trapezoid

============================================================================
  !! Note !! :   The line is strictly horizontal or strictly vertical (i.e. 
  either the X co-ordinate or the Y co-ordinate of the lines are the same)
============================================================================

  The width of the line must be at least 1 pixel and no more than the width 
  of the input image. A long top width corresponds to a vanishing 
  point that is far away from the viewer or camera. A short top width 
  indicates a vanishing point near the camera. The filler pixel value 
  must be given in a format compatible with the output color format. For 
  instance, black is 0 for RGB565 and 0x8010 for YCbCr. White is 0xFFFF 
  for RGB565 and 0x80EB for YCbCr. Notice that for YCbCr, the chroma 
  value (Cb or Cr) comes before the luma value, so white is 0x80EB 
  instead of 0xEB80. The trapezoid’s height and bottom width are assumed 
  to be the input height and width for "up" and "down" orientations. 
  For "left" and "right" orientations, the trapezoid's height and bottom 
  width are the input width and height, respectively.

  For any orientation, the top width is defined as the smaller 
  of the two parallel edges of the trapezoid.

  Orientation can take one of four values: 
  1) IIPL_PERSPECTIVE_UP
  2) IIPL_PERSPECTIVE_DOWN
  3) IIPL_PERSPECTIVE_LEFT
  4) IIPL_PERSPECTIVE_RIGHT

  (0) up:
         ----
        /    \
       /      \
       --------
  (1) down:
       --------
       \      /
        \    /
         ----
  (2) left:
        __ --- |
      |        |
      | __     |
           --- |
  (3) right:
      | --- __
      |        |
      |     __ |
      | ---
  
Prototype:
    int IIPL_ViewPerspective
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLLine* line,
      uint16 fillerPixel,
      IIPLPerspectiveType orientation
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    line[in]: Pointer to the IIPLLine info structure which specifies the straight line indicating 
              the top of trapezoid
    fillerPixel[in]: unit16 value specifying the color value to be used to fill the blank region
    orientation[in]: orientation of the trapezoid

Return Value:
    Returns AEEErrorCodes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the line passed as input isn't strictly horizontal or strictly vertical
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    IIPLLine

=============================================================================*/

/*=============================================================================

IIPL_CopyPaste()
 
Description:
  This function performs copy-and-paste. A region is copied from the input 
  image and pasted onto a region in the output image. Input and output 
  image sizes can be different. If the input "copy" region is strictly 
  larger than the output "paste" region, the input region will be downsized 
  to fit the output region. If the images are RGB565, an input "copy" region 
  can also be upsized if it is strictly smaller than the output "paste" 
  region.

  
Prototype:
    int IIPL_CopyPaste
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      AEERect* cutRect,
      AEERect* pasteRect
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    cutRect[in]: Pointer to the rectangular area to be copied from the input image
    pasteRect[in]: Pointer to the rectangular area on the output image where the copied image is to be pasted

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the paste area falls outside the output image dimensions
    
Comments:
    The colorScheme of the output image needs to be set by the user.

See Also:
    AEERect

=============================================================================*/
/*=============================================================================

IIPL_CutPaste()
 
Description:
  This function performs cut-and-paste. A region is cut from the input 
  image and pasted onto a region in the output image. Input and output 
  image sizes can be different. If the input "cut" region is strictly 
  larger than the output "paste" region, the input region will be downsized 
  to fit the output region. If the images are RGB565, an input "cut" region 
  can also be upsized if it is strictly smaller than the output "paste" 
  region.

  This will also affect the input image and the piece that was cut out
  will be replaced by the fillerPixel passed in by the user.
  
Prototype:
    int IIPL_CutPaste
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      AEERect* cutRect,
      AEERect* pasteRect,
      uint16 fillerPixel
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    cutRect[in]: Pointer to the rectangular area to be cut from the input image
    pasteRect[in]: Pointer to the rectangular area on the output image where the cut image is to be pasted
    fillerPixel[in]: uint16 value representing the color that shall be used to replace the cut piece from the input

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the paste area falls outside the output image dimensions
    
Comments:
    The colorScheme of the output image needs to be set by the user.

See Also:
    AEERect

=============================================================================*/

/*=============================================================================


IIPL_ArbitRotate()
 
Description:
  This function performs arbitrary rotation. The user must specify 
  the angle of rotation in degrees and the pixel value used to fill 
  in blank regions. Any part of the input that is rotated outside
  image boundaries is cut off. Only rotations of 0, 90, 180, and 270 
  degrees do not cut off any part of the input image. Negative angles 
  produce clockwise rotation. Positive angles produce counterclockwise 
  rotation. Angles can be any integer between positive and negative 
  infinity.
  
  
Prototype:
    int IIPL_ArbitRotate
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      int32 rotate,
      uint16 fillColor
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    rotate[in]: The angle, in degrees, by which the input image is to be rotated
    fillColor[in]: The color to be used to fill in the blank regions after the arbitrary rotation

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    None
=============================================================================*/

/*=============================================================================
IIPL_RotateFrame()
 
Description:
  This function performs 90 degree clockwise rotation of frames with special 
  processing for transparent pixels. Four types of rotation are supported: 
  0 degrees, 90 degrees, 180 degrees, and 270 degrees.
  
  Input and output frame images must be in RGB565.
  
Prototype:
    int IIPL_RotateFrame
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLRotateType rotation
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    rotation[in]: IIPLRotateType value specifying the angle of rotation (0, 90, 180 or 270)

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the rotation value passed in is not one of the four permitted values.
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.


See Also:
    IIPLRotateType

=============================================================================*/
/*=============================================================================

IIPL_ComposeCanvas()
 
Description:
  This function iterates through a list of icons and composes a frame
  from the list. 

  Note:
  1) iconList can be NULL if there are no icons to be added. listSize is ignored
     if iconList is NULL.
  
Prototype:
    int IIPL_ComposeCanvas
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLIconInfo* iconList,
      uint32 listSize
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    iconList[in]: Pointer to the first element in the IIPLIconInfo array
    listSize[in]: size of the IIPLIconType array

Return Value:
    Returns AEEError values

    AEE_SUCCESS: if the operation is successful
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.


See Also:
    IIPLIconType

=============================================================================*/
/*=============================================================================
IIPL_RotateCropAddFrame()
 
Description:
  This function performs real-time clockwise rotation and addition of a frame. 
  Four types of rotations are supported: 0 degrees, 90 degrees, 180 degrees,
  and 270 degrees. It will also

  Note:

  1) IconList is an optional parameter. It can be NULL if no icons need to be added. Also,
     listSize parameter will be ignored if IconList is NULL.

  2) The cropping rectangle should not lie outside the output image. 

  3) The input Frame can be NULL if there are no frames to be added.
    
  
Prototype:
    int IIPL_RotateCropAddFrame
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* inFrame,
      IBitmap* outImage,
      IIPLIconInfo* iconList,
      uint32 listSize,
      AEERect* crop,
      IIPLRotateType rotation
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    inFrame[in]: Pointer to the input frame.
    outImage[in/out]: Pointer to the output image
    iconList[in]: Pointer to the first element in the IIPLIconInfo array
    listSize[in]: size of the iconList passed 
    crop[in]: Pointer to AEERect indicating the cropping rectangle
    rotation[in]: IIPLRotateType value specifying the angle of rotation (0, 90, 180 or 270)

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the cropping rectangle lies outside the output image dimensions
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    AEERect
    
    IIPLRotateType
    IIPLIconInfo

=============================================================================*/

/*=============================================================================
IIPL_Crop()
 
Description:

   This function crops the input image as specified by the cropping rectangle 
   and writes the cropped part to the output image

  Note:

  2) The cropping rectangle should not lie outside the output image. 

Prototype:
    int IIPL_RotateCropAddFrame
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      AEERect* crop
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    crop[in]: Pointer to AEERect indicating the cropping rectangle

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the cropping rectangle lies outside the output image dimensions
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    AEERect
    

=============================================================================*/

/*=============================================================================
IIPL_AddFrame()
 
Description:
  This function performs addition of a frame. 

  The frame has to be in a RGB format and the same size as the input image. 

  
Prototype:
    int IIPL_RotateCropAddFrame
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* inFrame,
      IBitmap* outImage,
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    inFrame[in]: Pointer to the input frame.
    outImage[in/out]: Pointer to the output image

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    

=============================================================================*/

/*=============================================================================
IIPL_MotionBlur()
 
Description:
  This function produces motion blur in any arbitrary direction. The size 
  of the blurring window must be an odd, positive integer in the range 
  3 <= size <= 1/2 * min(w, h), where w and h are the width and height of 
  the input image. The direction of motion is specified by a positive integer 
  angle measured in degrees from the horizontal.

Prototype:
    int IIPL_MotionBlur
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      uint32 blurWindow,
      uint32 angle
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    blurWindow[in]: blurWindow Size
    angle[in]: Angle in which the motion blur is to be applied.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the blurWindow is not a positive odd integer.
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    AEERect
    
    IIPLRotateType
    IIPLIconType

=============================================================================*/
/*=============================================================================
IIPL_RadialBlur()
 
Description:
    
  This function performs radial blur. The blurring level can be set 
  to an integer between 0 and 100, inclusive. No blurring occurs 
  when the blurring level is 0.

Prototype:
    int IIPL_RadialBlur
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      uint8 level
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    level[in]: Level of radial blurring needed.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the blurring level is outside the permitted range
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
    

=============================================================================*/
/*=============================================================================
IIPL_Warp()
 
Description:
    
  This function performs a warp on the input image. The warping effect is decided 
  by the grid that is passed in. "gridLoc" decides whether the supplied grid is applicable
  to the input image or the output image.

  The effect is achieved as follows: 

  1)  If gridLoc is set to FALSE, 
      The input image is assumed to have been set on a perfect checkerboard grid with rows and 
      columns of equal width and the output image is assumed to be set on the grid that is passed
      in, which could have rows and columns of different widths.

  2)  If gridLoc is set to TRUE,
      The input image is assumed to have been set on the grid passed in, with unequal rows and columns 
      and the output image is assumed to be set on the perfect checkerboard grid with rows and columns 
      of equal widths.
  
  Each block in the input grid is mapped to the corresponding grid in the output grid and the warp
  effect is achieved.

  So, with gridLoc set to TRUE, the grid would have the opposite effect than with gridLoc set to FALSE.

Prototype:
    int IIPL_Warp
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLGrid* grid,
      boolean gridLoc
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    grid[in]: Pointer to the IIPLGrid specifying the warp
    gridLoc[in]: boolean value which determines the location of the grid passed in.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
IIPLGrid
    

=============================================================================*/

/*=============================================================================
IIPL_Transform()

Description:

  This function performs the transformation of the input image. The type of transformation
  to be applied is determined by the transformation type passed in to the function.

  The different transformation types are indicated by IIPLTransformType

Prototype:
    int IIPL_Transform
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLTransformType xFormType,
      uint32 xFormValue
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    xFormType[in]: IIPLTransformType value indicating the type of transformation to be applied
    xFormValue[in]: value of the transformation depending on the IIPLTransformType

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the xFormType is not one of the permitted transformations or if
             the xFormValue is not one of the permitted values (dependent on xFormatype)
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
  IIPLTransformType
  IIPLReflectType
  IIPLRotateType
  IIPL_Reflect()
  IIPL_Rotate()

=============================================================================*/
/*=============================================================================

IIPL_Effect()

Description:

  This function adds special effects to the input image. The type of effect to be 
  applied is determined by the effect type passed in to the function.

  The different effect types are indicated by IIPLEffectType


Prototype:
    int IIPL_Effect
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      IIPLEffectType effectType,
      int32 effectValue
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    effectType[in]: IIPLEffectType value indicating the type of effect to be applied
    effectValue[in]: value of the effect

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the effectType is not one of the permitted transformations or if
             the effectValue is not one of the permitted values (dependent on effectType)
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
  IIPLEffectType
  IIPL_MakeGrayscale()
  IIPL_MakeNeon()
  IIPL_MakeSepia()
  IIPL_AddFog()
  IIPL_AddFlash()
  IIPL_SetSaturation()
  IIPL_SetPosterization()
  IIPL_SetSolarization()
  IIPL_SetIntensity()
  IIPL_SetHue()
  IIPL_MakeNegative()

=============================================================================*/

/*=============================================================================
IIPL_Compose()

Description:

  This function composes an output image by combining two input images. There are
  several composition functions that can be used to combine two images. 

  The different composition methods are indicated by IIPLComposeType


Prototype:
    int IIPL_Compose
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* inImage2
      IBitmap* outImage,
      IIPLComposeType composeType,
      int32 composeValue
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    inImage2[in]: Pointer to the second input image.
    outImage[in/out]: Pointer to the output image
    composeType[in]: IIPLComposeType value indicating the type of compostition to be used
    composeValue[in]: value of the compose

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if the composeType is not one of the permitted transformations or if
             the composeType is not one of the permitted values (dependent on composeType)
    
Comments:

    The size of the output image should be the same size as the input image
    The colorScheme of the output image needs to be set by the user.

See Also:
  IIPLComposeType
  IIPL_Blend()
  IIPL_Center()
  IIPL_Vertical()
  IIPL_Horizontal()
  IIPL_FilmStrip()

=============================================================================*/

/*=============================================================================

--------------------------------------------------------------------------------

                  IIPL Types and Values

--------------------------------------------------------------------------------

IIPLTransformType:

    It indicates the different types of transformations that can be applied to the 
    input image. It is passed in to IIPL_Transform as 'p1' to indicate what transformation is to be applied
    to the input image. 'p2' indicates the value of the transformation to be applied

    1) IIPL_REFLECT: Apply reflection transformation to the input image
       p2 : IIPLReflectType variable 

    2) IIPL_ROTATE: Apply Rotation operation to the input image
       p2: IIPLRotateType variable

IIPLEffectType:
    It indicates the different types of special effects that can be applied to the 
    input image. It is passed in to IIPL_Effect as 'p1' to indicate what special effect 
    is to be applied to the input image. 'p2' indicates the value of the special effect 
    to be applied.

    1) IIPL_GRAYSCALE: Add grayscale effect to the input image
        p2 : Not Used.

    2) IIPL_HUE : Set the hue scaling factor of the input image
        p2: Hue scaling factor. Can Range from -360 to 360
    
    3) IIPL_NEGATIVE : Converts the input image to a negative image
        p2: Not Used

    4) IIPL_SATURATION : Set the saturation scaling factor of the input image 
        p2: Saturation Scaling factor. Can Range from 0 to 255
    
    5) IIPL_SEPIA : Converts the input image into a sepia image   
        p2: Not Used
    
    6) IIPL_POSTERIZE : Set the posterization factor of the input image             
        p2: Posterization scaling factor. Ranges from 0 to 255
    
    7) IIPL_SOLARIZE : Set the solarization scaling factor of the input image              
        p2: Solarization scaling factor. Ranges from 0 to 255
    
    8) IIPL_FLASH: Adds flash effect to the input image                 
        p2: Not Used
    
    9) IIPL_INTENSITY : Set the intensity scaling factor of the input image                          
        p2: Intensity scaling factor. Ranges from 0 to 255
    
    10) IIPL_FOG : Adds fog to the input image                  
        p2: Level of fogging to be applies. Ranges from 0 .. 255
    
    11) IIPL_NEON : Converts the input image into a neon image                 
        p2: Not Used

IIPLComposeType:
    It indicates the different types of compostition methods that can be applied to the 
    two input images. It is passed in to IIPL_ComposeType as 'p1' to indicate what special 
    composition method is to be applied to the input images. 'p2' indicates the additional
    information needed by the composition method and it is dependent on the type of 
    composition method used.

    1) IIPL_BLEND : Peform blending of the two input images
       p2: the alpha blending factor. Ranges from  0 .. 100

    2) IIPL_CENTER: Place the second image at the center of the first image
       p2: the width of the second image to be placed at the center of the first

    3) IIPL_VERTICAL: Place the second image on top of the first
       p2: The height on the output image at which the first image ends and second image begins.

    4) IIPL_HORIZONTAL: Place the second image to the right of the first
       p2: The width at which the first image ends and the second image begins.

    5) IIPL_FILMSTRIP: Attach the beginning of the second image to the end of the first
       p2: The width of the first image after which the second image is added.

=============================================================================*/

/*=============================================================================

IIPL_RemoveRedEye()
 
Description:
    This function performs red eye reduction. A region(rcCut) from the input image 
    is searched for the red eye. Once the red eye is removed, the modified region(rcPaste)
    is pasted to the output image. Input and output image sizes can be different.
    The cut/paste region is a relative location between the input/output image 
    and the full image.
  
Prototype:
    int IIPL_RemoveRedEye
    (
      IIpl * pIIpl,
      IBitmap* inImage,
      IBitmap* outImage,
      AEERect  rcCut,
      AEERect  rcPaste,
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    rcCut[in]: The rectangular area with the red eye to be modified from the input image
    rcPaste[in]: The rectangular area on the output image where the modified image is to be pasted

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the paste area falls outside the output image dimensions
    
=============================================================================*/
#endif /* IIPL_H */
