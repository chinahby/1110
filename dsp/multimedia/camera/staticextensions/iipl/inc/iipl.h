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

#define AEEIID_IIPL    	0x0102eae0

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef uint32 IIPLColorType;
typedef uint32 IIPLPerspectiveType;
typedef uint32 IIPLRotateType;
typedef uint32 IIPLReflectType;
typedef uint32 IIPLFilterType;
typedef uint32 IIPLTransformType;
typedef uint32 IIPLComposeType;
typedef uint32 IIPLEffectType;
typedef uint32 IIPLQuality;

//Supports following color types. 
#define IIPL_COLOR_BASE                   0x00000000            // base used by IIPL

#define IIPL_NO_COLOR                     (IIPL_COLOR_BASE)     
#define IIPL_YCBCR                        (IIPL_COLOR_BASE + 1) // YCbCr pixel color format 4:2:2
#define IIPL_YCBCR_LINE420                (IIPL_COLOR_BASE + 2) // YCbCr 4:2:0 Line Packed format
#define IIPL_YCBCR_LINE422                (IIPL_COLOR_BASE + 3) // YCbCr 4:2:2 Line Packed format
#define IIPL_YCBCR_LINE444                (IIPL_COLOR_BASE + 4) // YCbCr 4:4:4 Line Packed format
#define IIPL_RGB565                       (IIPL_COLOR_BASE + 5) // RGB565 format (2 bytes per pixel)
#define IIPL_RGB888                       (IIPL_COLOR_BASE + 6) // RGB888 format (3 bytes per pixel)
#define IIPL_RGB666                       (IIPL_COLOR_BASE + 7) // RGB666 format
#define IIPL_RGB444                       (IIPL_COLOR_BASE + 8) // RGB444 format
#define IIPL_MAX_COLOR_VAL                (IIPL_COLOR_BASE + 9) 

// Supports following effect types
#define IIPL_PERSPECTIVE_BASE             0x00000000                  // base used by IIPL

#define IIPL_PERSPECTIVE_UP               (IIPL_PERSPECTIVE_BASE)     // Perspective Trapezoid with the smaller edge upwards
#define IIPL_PERSPECTIVE_DOWN             (IIPL_PERSPECTIVE_BASE + 1) // Perspective Trapezoid with the smaller edge downwards
#define IIPL_PERSPECTIVE_LEFT             (IIPL_PERSPECTIVE_BASE + 2) // Perspective Trapezoid with the smaller edge to the left
#define IIPL_PERSPECTIVE_RIGHT            (IIPL_PERSPECTIVE_BASE + 3) // Perspective Trapezoid with the smaller edge to the right
#define IIPL_MAX_PERSPECTIVE_VAL          (IIPL_PERSPECTIVE_BASE + 4) 

//The degrees of rotation
#define IIPL_ROTATE_BASE                  0x00000000             // base used by IIPL

#define IIPL_NO_ROTATE                    (IIPL_ROTATE_BASE)     // No rotation 
#define IIPL_ROTATE_90                    (IIPL_ROTATE_BASE + 1) // Rotate by 90 degrees
#define IIPL_ROTATE_180                   (IIPL_ROTATE_BASE + 2) // Rotate by 180 degrees
#define IIPL_ROTATE_270                   (IIPL_ROTATE_BASE + 3) // Rotate by 270 degrees
#define IIPL_MAX_ROTATE_VAL               (IIPL_ROTATE_BASE + 4) 

//Image can be reflected by changing X-Axis and  Y-Axies.
#define IIPL_REFLECT_BASE                 0x00000000              // base used by IIPL

#define IIPL_NO_REFLECT                   (IIPL_REFLECT_BASE)     // Don't reflect 
#define IIPL_REFLECT_X                    (IIPL_REFLECT_BASE + 1) // Reflect along the X-Axis
#define IIPL_REFLECT_Y                    (IIPL_REFLECT_BASE + 2) // Reflect along the Y-Axis
#define IIPL_MAX_REFLECT_VAL              (IIPL_REFLECT_BASE + 3) 

//Supports different Filter Types
#define IIPL_BLUR_BASE                    0x00000000           // base used by IIPL

#define IIPL_BLUR                         (IIPL_BLUR_BASE)     // Low Pass filter  
#define IIPL_SHARP                        (IIPL_BLUR_BASE + 1) // High Pass filter
#define IIPL_BLUR_GAUSSIAN                (IIPL_BLUR_BASE + 2) // Gaussian filter
#define IIPL_BLUR_UNIFORM                 (IIPL_BLUR_BASE + 3) // Uniform filter
#define IIPL_MAX_FILTER_VAL               (IIPL_BLUR_BASE + 4) 

//different transformation types.
#define IIPL_TRANSFORM_BASE               0x00000000                // base used by IIPL

#define IIPL_REFLECT                      (IIPL_TRANSFORM_BASE)     // Reflect the image about an axis  
#define IIPL_ROTATE                       (IIPL_TRANSFORM_BASE + 1) // Rotate the image 
#define IIPL_MAX_TRANSFORM_TYPE           (IIPL_TRANSFORM_BASE + 2) 

//Compose type for combine two images
#define IIPL_COMPOSE_BASE                 0x00000000              // base used by IIPL

#define IIPL_BLEND                        (IIPL_COMPOSE_BASE)     // Blend between two images
#define IIPL_CENTER                       (IIPL_COMPOSE_BASE + 1) // Add an image in the center of the other
#define IIPL_FILMSTRIP                    (IIPL_COMPOSE_BASE + 2) // Add the beginning of an image to the end of the first.
#define IIPL_HORIZONTAL                   (IIPL_COMPOSE_BASE + 3) // Add the left/right of an image to the left/right of the other image
#define IIPL_VERTICAL                     (IIPL_COMPOSE_BASE + 4) // Add one image to the top/bottom of the other image
#define IIPL_MAX_COMPOSE_VAL              (IIPL_COMPOSE_BASE + 5) 

//Effect Type
#define IIPL_EFFECT_BASE                  0x00000000            // base used by IIPL

#define IIPL_GRAYSCALE                    (IIPL_COLOR_BASE)     // Convert image to grayscale 
#define IIPL_HUE                          (IIPL_COLOR_BASE + 1) // Change the hue of the image
#define IIPL_NEGATIVE                     (IIPL_COLOR_BASE + 2) // Create a negative of the image
#define IIPL_SATURATE                     (IIPL_COLOR_BASE + 3) // Change the saturation of the image
#define IIPL_SEPIA                        (IIPL_COLOR_BASE + 4) // Change the image to sepia
#define IIPL_POSTERIZE                    (IIPL_COLOR_BASE + 5) // Posterize the image
#define IIPL_SOLARIZE                     (IIPL_COLOR_BASE + 6) // Solarize the image
#define IIPL_FLASH                        (IIPL_COLOR_BASE + 7) // Add smart flash to the image
#define IIPL_INTENSITY                    (IIPL_COLOR_BASE + 9) // Change the intensity of the image
#define IIPL_FOG                          (IIPL_COLOR_BASE + 10) // Add fog to the image
#define IIPL_NEON                         (IIPL_COLOR_BASE + 11) // Change the image to neon
#define IIPL_WHITEBOARD                   (IIPL_COLOR_BASE + 12) // Clear an image taken from whiteboard
#define IIPL_MAX_EFFECT_VAL               (IIPL_COLOR_BASE + 13) 

// Quality of result
#define IIPL_QUALITY_LOW                  0 // fastest code, lowest quality
#define IIPL_QUALITY_MEDIUM               1 // fast code, good quality
#define IIPL_QUALITY_HIGH                 2 // slow code, great quality
#define IIPL_QUALITY_MAX                  3 // slowest code, but highest quality

/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/

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
   int         (*RemoveRedEye)(iname *po, IBitmap* i_image, IBitmap* o_image, AEERect rcCut, AEERect rcPaste);\
   int         (*BottomUp)(iname *po, IBitmap *i_image);\
   int         (*CropResizeRot)(iname * po, IBitmap* i_image, IBitmap* o_image, \
                                AEERect *rcIn, AEERect *rcOut, IIPLRotateType rot, IIPLQuality quality);\
   int         (*ImageEdit)(iname * po, IBitmap* i_image, IBitmap* o_image, uint16 ProcID, uint32 p1, uint32 p2 )                             

AEEINTERFACE(IIPL)
{
  INHERIT_IIpl(IIpl);
};

/*-------------------------------------------------------------------
      API Definitions
-------------------------------------------------------------------*/
static __inline uint32 IIPL_AddRef(IIpl *pIIpl)
{
  return AEEGETPVTBL(pIIpl, IIPL)->AddRef(pIIpl);
}

static __inline uint32 IIPL_Release(IIpl *pIIpl)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Release(pIIpl);
}

static __inline int IIPL_QueryInterface(IIpl *pIIpl, AEECLSID clsid, void **pp)
{
  return AEEGETPVTBL(pIIpl, IIPL)->QueryInterface(pIIpl, clsid, pp);
}

static __inline int IIPL_CopyPaste(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, AEERect* rcCut, AEERect* rcPaste)
{
  return AEEGETPVTBL(pIIpl, IIPL)->CopyPaste(pIIpl, i_image, o_image, rcCut, rcPaste);
}

static __inline int IIPL_RotateFrame(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,IIPLRotateType p1)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RotateFrame(pIIpl, i_image, o_image, p1);
}

static __inline int IIPL_ArbitRotate(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, int16 angle, uint16 fillerPixel)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ArbitRotate(pIIpl, i_image, o_image, angle, fillerPixel);
}

static __inline int IIPL_Filter(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, IIPLFilterType filter, uint32 blurWindow)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Filter(pIIpl, i_image, o_image, filter, blurWindow);
}

static __inline int IIPL_ConvertFrame(IIpl *pIIpl, IBitmap* i_frame, IBitmap* o_frame)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ConvertFrame(pIIpl, i_frame, o_frame);
}

static __inline int IIPL_ComposeCanvas(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,
                                       IIPLIconInfo* icon_list, uint32 listSize)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ComposeCanvas(pIIpl, i_image, o_image,
                                                 icon_list, listSize);
}

static __inline int IIPL_RotateCropAddFrame(IIpl *pIIpl, IBitmap* i_image, IBitmap* i_frame, IBitmap* o_image,
                                            IIPLIconInfo* icon_list, uint32 listSize,
                                            AEERect* cropRect, IIPLRotateType rotate)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RotateCropAddFrame(pIIpl, i_image, i_frame, o_image,
                                                      icon_list, listSize, cropRect, rotate);
}

static __inline int IIPL_ChangeSize(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ChangeSize(pIIpl, i_image, o_image);
}

static __inline int IIPL_ViewPerspective(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,
                                         IIPLLine* line, uint16 fillerPixel, IIPLPerspectiveType orientation)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ViewPerspective(pIIpl, i_image, o_image, line, fillerPixel, orientation);
}

static __inline int IIPL_ConvertColor(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ConvertColor(pIIpl, i_image, o_image);
}

static __inline int IIPL_Magnify(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, IIPLCircle* circle)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Magnify(pIIpl, i_image, o_image, circle);
}

static __inline int IIPL_MotionBlur(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, uint32 angle, uint32 blurWindow)
{
  return AEEGETPVTBL(pIIpl, IIPL)->MotionBlur(pIIpl, i_image, o_image, angle, blurWindow);
}

static __inline int IIPL_RadialBlur(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, uint32 level)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RadialBlur(pIIpl, i_image, o_image, level);
}

static __inline int IIPL_Overlap(IIpl *pIIpl, IBitmap* i_image, IBitmap* i_image2, IBitmap* o_image,AEERect* rcOverlap)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Overlap(pIIpl, i_image, i_image2, o_image, rcOverlap);
}

static __inline int IIPL_Warp(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, IIPLGrid* grid, boolean gridLoc)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Warp(pIIpl, i_image, o_image, grid, gridLoc);
}

static __inline int IIPL_Transform(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, IIPLTransformType xform, uint32 xFormValue)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Transform(pIIpl, i_image, o_image, xform, xFormValue);
}

static __inline int IIPL_Reflect(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, IIPLReflectType reflect)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Transform(pIIpl, i_image, o_image, IIPL_REFLECT, reflect);
}

static __inline int IIPL_Rotate(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, IIPLRotateType rotate)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Transform(pIIpl, i_image, o_image, IIPL_ROTATE, rotate);
}

static __inline int IIPL_Effect(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,IIPLEffectType effect, int32 effectValue)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, effect, effectValue);
}

static __inline int IIPL_MakeGrayscale(IIpl *pIIpl, IBitmap *i_image, IBitmap  *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_GRAYSCALE, 0);
}

static __inline int IIPL_SetHue(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, int32 hue)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_HUE, hue);
}

static __inline int IIPL_MakeNegative(IIpl *pIIpl, IBitmap *i_image, IBitmap  *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_NEGATIVE, 0);
}

static __inline int IIPL_SetSaturation(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, int32 saturation)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_SATURATE, saturation);
}

static __inline int IIPL_MakeSepia(IIpl	*pIIpl, IBitmap *i_image, IBitmap *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_SEPIA, 0);
}

static __inline int IIPL_SetPosterization(IIpl *pIIpl, IBitmap *i_image, IBitmap  *o_image, int32 posterize)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_POSTERIZE, posterize);
}

static __inline int IIPL_SetSolarization(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, uint32 solarize)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_SOLARIZE, solarize);
}

static __inline int IIPL_SetIntensity(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, int32 intensity)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_INTENSITY, intensity);
}

static __inline int IIPL_AddFlash(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_FLASH, 0);
}

static __inline int IIPL_AddFog(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, uint32 fog)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_FOG, fog);
}

static __inline int IIPL_MakeNeon(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Effect(pIIpl, i_image, o_image, IIPL_NEON, 0);
}

static __inline int IIPL_Compose(IIpl *pIIpl, IBitmap *i_image,IBitmap *i_image2,IBitmap *o_image,
                                 uint32 p1, int32 boundary)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image,
                                           p1, boundary);
}

static __inline int IIPL_Blend(IIpl *pIIpl, IBitmap *i_image, IBitmap *i_image2, IBitmap *o_image, int32 alpha)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image, IIPL_BLEND,alpha);
}

static __inline int IIPL_Center(IIpl *pIIpl, IBitmap *i_image, IBitmap *i_image2, IBitmap *o_image, uint32 width)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image, IIPL_CENTER,width);
}

static __inline int IIPL_FilmStrip(IIpl *pIIpl, IBitmap *i_image, IBitmap *i_image2, IBitmap *o_image, uint32 boundary)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image, IIPL_FILMSTRIP, boundary);
}

static __inline int IIPL_AddHorizontal(IIpl *pIIpl, IBitmap *i_image, IBitmap *i_image2, IBitmap *o_image, uint32 boundary)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image, IIPL_HORIZONTAL, boundary);
}

static __inline int IIPL_AddVertical(IIpl *pIIpl, IBitmap *i_image, IBitmap *i_image2, IBitmap *o_image, uint32 height)
{
  return AEEGETPVTBL(pIIpl, IIPL)->Compose(pIIpl, i_image, i_image2, o_image, IIPL_VERTICAL, height);
}

static __inline int IIPL_CutPaste(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,AEERect* rcCut, AEERect* rcPaste, uint16 fillerPixel)
{
  return AEEGETPVTBL(pIIpl, IIPL)->CutPaste(pIIpl, i_image, o_image, rcCut, rcPaste, fillerPixel);
}

static __inline int IIPL_AddFrame(IIpl *pIIpl, IBitmap *i_image, IBitmap* i_frame, IBitmap *o_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RotateCropAddFrame(pIIpl, i_image, i_frame, o_image, NULL,NULL,NULL,0);
}

static __inline int IIPL_Crop(IIpl *pIIpl, IBitmap *i_image, IBitmap *o_image, AEERect* crop)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RotateCropAddFrame(pIIpl, i_image, NULL, o_image, NULL,NULL,crop,0);
}

static __inline int IIPL_DrawHistogram(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, AEERect* pRect)
{
  return AEEGETPVTBL(pIIpl, IIPL)->DrawHistogram(pIIpl, i_image, o_image, pRect);
}

static __inline int IIPL_DrawRgbHistogram(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, AEERect* pRect)
{
  return AEEGETPVTBL(pIIpl, IIPL)->DrawRgbHistogram(pIIpl, i_image, o_image, pRect);
}

static __inline int IIPL_RemoveRedEye(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, AEERect rcCut, AEERect rcPaste)
{
  return AEEGETPVTBL(pIIpl, IIPL)->RemoveRedEye(pIIpl, i_image, o_image, rcCut, rcPaste);
}

static __inline int IIPL_BottomUp(IIpl *pIIpl, IBitmap* i_image)
{
  return AEEGETPVTBL(pIIpl, IIPL)->BottomUp(pIIpl, i_image);
}

static __inline int IIPL_CropResizeRot(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image,
                                       AEERect *rcIn, AEERect *rcOut, IIPLRotateType rot, IIPLQuality quality)
{
  return AEEGETPVTBL(pIIpl, IIPL)->CropResizeRot(pIIpl, i_image, o_image,
                                                 rcIn, rcOut, rot, quality);
}

static __inline int IIPL_ImageEdit(IIpl *pIIpl, IBitmap* i_image, IBitmap* o_image, uint16 ProcID, uint32 p1, uint32 p2)
{
  return AEEGETPVTBL(pIIpl, IIPL)->ImageEdit(pIIpl, i_image, o_image, ProcID, p1, p2);
}

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
    of the image. The posterization value can range from 0 to 255

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
    This function adds a foggy effect to the input image. The user specifies the level of
    fogginess, either light (0) or heavy (1).

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
    fog[in]: fog level, either light (0) or heavy (1)

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM, otherwise
    
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
    It will do nothing when filter = 0 ,blurWindow =9  and filter = 0 ,blurWindow =30.

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
    When the rotation is 90 or 270 then the input dx has to be equal to output
    dy and the output dx has to be equal to input dy. 

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
    When the rotation is 90 or 270 then the input dx has to be equal to output
    dy and the output dx has to be equal to input dy. 

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

/*=============================================================================

IIPL_BottomUp()
 
Description:
    This function is used to convert original top_down bitmap to bottom_up bitmap 
    before performing any other transform operation.

  Prototype:
    int IIPL_BottomUp
    (
      IIpl *pIIpl,
      IBitmap *inImage,
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.

Return Value:
    Returns AEEError codes

=============================================================================*/

/*=============================================================================

IIPL_CropResizeRot()

Description:
    This function is used for cropping, resizing, and rotating, or any
    combination of those.

    There is a quality param that can be specified to determine the quality
    of the resulting image, and how fast the operation can be performed
    (higher quality -> lower speed)

Prototype:
    int IIPL_CropResizeRot
    (
      IIpl *pIIpl,
      IBitmap *inImage,
      IBitmap *outImage,
      AEERect *rcIn,
      AEERect *rcOut,
      IIPLRotateType rot,
      IIPLQuality quality,
    )

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image

    rcIn[in]: cropping rectangle of input image: can be NULL, no cropping
    rcOut[in]: cropping rectangle of output image: can be NULL, no cropping
    rot[in]: rotation

    quality[in]: Quality of resulting image:
                 IIPL_QUALITY_LOW,            // fastest code, lowest quality
                 IIPL_QUALITY_MEDIUM,         // fast code, good quality
                 IIPL_QUALITY_HIGH,           // slow code, great quality
                 IIPL_QUALITY_MAX             // slowest code, but highest quality

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully

    
Comments:
    The colorScheme of the output image needs to be set by the user.

See Also:

=============================================================================*/


/*============================================================================

IIPL_ImageEdit()

Description:
    This function is used for editing the input image. The type of pocess to be 
    applied is determined by the ProcID type passed in to the function.
    The different edit types are indicated by ProcID.

Prototype:
      int IIPL_ImageEdit
      (
        IIPL *pIIpl,
        IBitmap *inImage,
        IBitmap *outImage,
        uint16 ProcID,
        uint32 p1, 
        uint32 p2
      )   

Parameters:
    pIIpl[in]:  Pointer to IIpl Interface.
    inImage[in]: Pointer to the input image.
    outImage[in/out]: Pointer to the output image
    ProcID[in]: The ID to identify the edit feature
    p1[in]:     This is a input variable or an address to the input structure for the edit feature.
    p2[in]:     This is a the size of p1 buffer or the second input variable.

Return Value:
    Returns AEEError codes

    AEE_SUCCESS, if the function completed successfully
    EBADPARM: if the ProcID is not one of the permitted transformations or if
              the p1 is not one of the permitted values (dependent on ProcID)
    
Comments:
    
See Also:
==============================================================================*/
#endif /* IIPL_H */
