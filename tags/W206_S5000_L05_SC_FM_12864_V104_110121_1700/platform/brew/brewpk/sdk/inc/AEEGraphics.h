#ifndef  AEEGRAPHICS_H
#define  AEEGRAPHICS_H
/*=================================================================================
FILE:       AEEGraphics.h

SERVICES:   2D graphics interfaces.

DESCRIPTION:   This file provides definitions for the 2D Graphics interfaces made
available by the AEE to the application developers.  This is a standard header file
that must be included by all applications using IGraphics modules of the AEE.

PUBLIC CLASSES:   Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEDisp.h"
#include "AEEBitmap.h"

typedef struct _IGraphics  IGraphics;

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// Flags used for handling clipping region
// 0x01 is reserved for internal use.
#define  AEE_GRAPHICS_NONE    0x00
#define  AEE_GRAPHICS_FRAME   0x02
#define  AEE_GRAPHICS_CLEAR   0x04
#define  AEE_GRAPHICS_FILL    0x08

// Types for clipping shapes.
// Only closed 2D shapes can be clip-shapes
enum {
   CLIPPING_NONE, // No clipping shape is specified, the window is used
   CLIPPING_RECT,
   CLIPPING_CIRCLE,
   CLIPPING_ELLIPSE,
   CLIPPING_PIE,
   CLIPPING_TRIANGLE,
   CLIPPING_POLYGON
};
#ifdef WIN32
typedef unsigned AEEClipShape;
#else
typedef int8 AEEClipShape;
#endif


// Enum for algorithm choice
enum {
   ALGORITHM_ACCURATE,  // accurate and slow algorithm
   ALGORITHM_FAST       // fast but inaccurate algorithm
};
#ifdef WIN32
typedef unsigned AEEAlgorithmHint;
#else
typedef int8 AEEAlgorithmHint;
#endif



// Enum for Stroke Styles
enum {
   STROKE_SOLID,
   STROKE_DOTTED
};
#ifdef WIN32
typedef unsigned AEEStrokeStyle;
#else
typedef int8 AEEStrokeStyle;
#endif


// Circle
typedef struct _circle {
   int16 cx, cy;     // Center of the circle
   int16 r;          // Radius of the circle
} AEECircle;



// Ellipse (axis-aligned only)
typedef struct _ellipse {
   int16 cx, cy;     // center of ellipse
   int16 wx;         // semimajor/minor axis along x-axis
   int16 wy;         // semimajor/minor axis along y-axis
                     // which is major or minor depends on the relative values of wx and wy
} AEEEllipse;



// Arc
// Comment: Make sure that the the first 3 fields are identical to AEECircle,
// because sometimes we may convert an arc to circle using explicit type casting.
typedef struct _arc {
   int16 cx, cy;     // Center of the reference circle
   int16 r;          // Radius of the reference circle
   int16 startAngle; // In degrees from 0 to 359
   int16 arcAngle;   // In degrees from 0 360
} AEEArc;






// Pie
typedef struct _pie {
   int16 cx, cy;     // Center of the reference circle
   int16 r;          // Radius of the reference circle
   int16 startAngle; // In degrees
   int16 arcAngle;   // In degrees
} AEEPie;



typedef struct _triangle {
   int16 x0, y0;     // First point
   int16 x1, y1;     // Second point
   int16 x2, y2;     // Third point
} AEETriangle;



typedef struct _polygon {
   int16    len;      // Number of vertices
   AEEPoint *points;  // Array of vertices
} AEEPolygon;



typedef struct _polyline {
   int16    len;     // Number of vertices
   AEEPoint *points; // Array of points
} AEEPolyline;



// 2D shapes for clipping
typedef struct _clipshape{
   AEEClipShape   type; // Type of the shape for clip area
   union {
      AEERect        rect;
      AEECircle      circle;
      AEEEllipse     ellipse;
      AEEPie         pie;
      AEETriangle    triangle;
      AEEPolygon     polygon;
   } shape;
} AEEClip;



// Paint mode
enum {
   AEE_PAINT_COPY,         // New content overwrites the old
   AEE_PAINT_XOR        // New "xor" old
};
#ifdef WIN32
typedef unsigned AEEPaintMode;
#else
typedef int8 AEEPaintMode;
#endif



// Line
typedef struct _line {
   int16 sx, sy;     // starting point
   int16 ex, ey;     // ending point
} AEELine;




//====================================================================================
// FUNCTION DEFINITIONS and INLINE FUNCTION DEFINITIONS
//====================================================================================


//************************************************************************************
//
// IGraphics Interface
//
//************************************************************************************

QINTERFACE(IGraphics)
{
   DECLARE_IBASE(IGraphics)

   // Rendering attributes setting and query operations

   RGBVAL            (*SetBackground)(IGraphics *po, uint8 r, uint8 g, uint8 b);
   void              (*GetBackground)(IGraphics *po, uint8 *r, uint8 *g, uint8 *b);

   RGBVAL            (*SetColor)(IGraphics *po, uint8 r, uint8 g, uint8 b, uint8 alpha);
   void              (*GetColor)(IGraphics *po, uint8 *r, uint8 *g, uint8 *b, uint8 *alpha);

   boolean           (*SetFillMode)(IGraphics *po, boolean fFill);
   boolean           (*GetFillMode)(IGraphics *po);

   RGBVAL            (*SetFillColor)(IGraphics *po, uint8 r, uint8 g, uint8 b, uint8 alpha);
   void              (*GetFillColor)(IGraphics *po, uint8 *pRed, uint8 *pGreen, uint8 *pBlue, uint8 *pAlpha);

   uint8             (*SetPointSize)(IGraphics *po, uint8 u8Size);
   uint8             (*GetPointSize)(IGraphics *po);

   boolean           (*SetClip)(IGraphics *po, AEEClip *pShape, uint8 nFlag);
   boolean           (*GetClip)(IGraphics *po, AEEClip *pShape);

   boolean           (*SetViewport)(IGraphics *po, AEERect *pRect, uint8 nFlag);
   boolean           (*GetViewport)(IGraphics *po, AEERect *pRect, boolean *framed);
   void              (*ClearViewport)(IGraphics *po);

   AEEPaintMode      (*SetPaintMode)(IGraphics *po, AEEPaintMode mode);
   AEEPaintMode      (*GetPaintMode)(IGraphics *po);


   uint8             (*GetColorDepth)(IGraphics *po);

   // Shape operations

   int            (*DrawPoint)(IGraphics *po, AEEPoint *pPoint);
   int            (*DrawLine)(IGraphics *po, AEELine *pLine);
   int            (*DrawRect)(IGraphics *po, AEERect *pRect);
   int            (*DrawCircle)(IGraphics *po, AEECircle *pCircle);
   int            (*DrawArc)(IGraphics *po, AEEArc *pArc);
   int            (*DrawPie)(IGraphics *po, AEEPie *pPie);
   int            (*DrawEllipse)(IGraphics *po, AEEEllipse *pEllipse);                                                                              // ellipse only
   int            (*DrawTriangle)(IGraphics *po, AEETriangle *pTriangle);
   int            (*DrawPolygon)(IGraphics *po, AEEPolygon *pPolygon);
   int            (*DrawPolyline)(IGraphics *po, AEEPolyline *pPolyline);

   // Frame buffer operations
   int            (*ClearRect)(IGraphics *po, AEERect *pRect);
   boolean        (*EnableDoubleBuffer)(IGraphics *po, boolean flag);
   void           (*Update)(IGraphics *po);

   // Transformations

   // Translate the origin of the current coordiate system by (x, y).
   // By default, the origin is at (0, 0), which is at the lower left corner of the window.

   void           (*Translate)(IGraphics *po, int16 x, int16 y);
   void           (*Pan)(IGraphics *po, int16 cx, int16 cy);
   void           (*StretchBlt)(IGraphics* po, int16 destX, int16 destY,
                   int16 destW, int16 destH, const void * pbmSource,
                   AEERasterOp dwRopCode);

   void              (*SetAlgorithmHint)(IGraphics *po, AEEAlgorithmHint hint);
   AEEAlgorithmHint  (*GetAlgorithmHint)(IGraphics *po);

   int            (*SetDestination)(IGraphics *po, IBitmap *pDst);
   IBitmap*       (*GetDestination)(IGraphics *po);

   AEEStrokeStyle (*SetStrokeStyle)(IGraphics *po, AEEStrokeStyle strokeStyle);
   AEEStrokeStyle (*GetStrokeStyle)(IGraphics *po);

   int            (*DrawEllipticalArc)(IGraphics *po, AEERect *pRect, int16 startAngle, int16 arcAngle );
   int            (*DrawRoundRectangle)(IGraphics *po, AEERect *pRect, int16 arcWidth, int16 arcHeight );
};


// C Access Macros

#define IGRAPHICS_AddRef(p)                     GET_PVTBL(p, IGraphics)->AddRef(p)
#define IGRAPHICS_Release(p)                    GET_PVTBL(p, IGraphics)->Release(p)
#define IGRAPHICS_SetBackground(p, r, g, b)     GET_PVTBL(p, IGraphics)->SetBackground(p, r, g, b)
#define IGRAPHICS_SetColor(p, r, g, b, a)       GET_PVTBL(p, IGraphics)->SetColor(p, r, g, b, a)
#define IGRAPHICS_SetFillMode(p, m)             GET_PVTBL(p, IGraphics)->SetFillMode(p, m)
#define IGRAPHICS_SetFillColor(p, r, g, b, a)   GET_PVTBL(p, IGraphics)->SetFillColor(p, r, g, b, a)
#define IGRAPHICS_SetPointSize(p, s)            GET_PVTBL(p, IGraphics)->SetPointSize(p, s)
#define IGRAPHICS_SetClip(p, s, f)              GET_PVTBL(p, IGraphics)->SetClip(p, s, f)
#define IGRAPHICS_SetViewport(p, r, f)          GET_PVTBL(p, IGraphics)->SetViewport(p, r, f)
#define IGRAPHICS_SetPaintMode(p, m)            GET_PVTBL(p, IGraphics)->SetPaintMode(p, m)
#define IGRAPHICS_GetBackground(p, r, g, b)     GET_PVTBL(p, IGraphics)->GetBackground(p, r, g, b)
#define IGRAPHICS_GetColorDepth(p)              GET_PVTBL(p, IGraphics)->GetColorDepth(p)
#define IGRAPHICS_GetColor(p, r, g, b, a)       GET_PVTBL(p, IGraphics)->GetColor(p, r, g, b, a)
#define IGRAPHICS_GetFillMode(p)                GET_PVTBL(p, IGraphics)->GetFillMode(p)
#define IGRAPHICS_GetFillColor(p, r, g, b, a)   GET_PVTBL(p, IGraphics)->GetFillColor(p, r, g, b, a)
#define IGRAPHICS_GetPointSize(p)               GET_PVTBL(p, IGraphics)->GetPointSize(p)
#define IGRAPHICS_GetClip(p, s)                 GET_PVTBL(p, IGraphics)->GetClip(p, s)
#define IGRAPHICS_GetViewport(p, r, f)          GET_PVTBL(p, IGraphics)->GetViewport(p, r, f)
#define IGRAPHICS_GetPaintMode(p)               GET_PVTBL(p, IGraphics)->GetPaintMode(p)
#define IGRAPHICS_DrawPoint(p, point)           GET_PVTBL(p, IGraphics)->DrawPoint(p, point)
#define IGRAPHICS_DrawLine(p, ln)               GET_PVTBL(p, IGraphics)->DrawLine(p, ln)
#define IGRAPHICS_DrawRect(p, r)                GET_PVTBL(p, IGraphics)->DrawRect(p, r)
#define IGRAPHICS_DrawCircle(p, cir)            GET_PVTBL(p, IGraphics)->DrawCircle(p, cir)
#define IGRAPHICS_DrawArc(p, arc)               GET_PVTBL(p, IGraphics)->DrawArc(p, arc)
#define IGRAPHICS_DrawPie(p, pie)               GET_PVTBL(p, IGraphics)->DrawPie(p, pie)
#define IGRAPHICS_DrawEllipse(p, e)             GET_PVTBL(p, IGraphics)->DrawEllipse(p, e)
#define IGRAPHICS_DrawTriangle(p, t)            GET_PVTBL(p, IGraphics)->DrawTriangle(p, t)
#define IGRAPHICS_DrawPolygon(p, pgon)          GET_PVTBL(p, IGraphics)->DrawPolygon(p, pgon)
#define IGRAPHICS_DrawPolyline(p, pln)          GET_PVTBL(p, IGraphics)->DrawPolyline(p, pln)
#define IGRAPHICS_ClearViewport(p)              GET_PVTBL(p, IGraphics)->ClearViewport(p)
#define IGRAPHICS_ClearRect(p, r)               GET_PVTBL(p, IGraphics)->ClearRect(p, r)


#define IGRAPHICS_EnableDoubleBuffer(p, f)      GET_PVTBL(p, IGraphics)->EnableDoubleBuffer(p, f)


#define IGRAPHICS_Update(p)                     GET_PVTBL(p, IGraphics)->Update(p)
#define IGRAPHICS_Translate(p, x, y)            GET_PVTBL(p, IGraphics)->Translate(p, x, y)
#define IGRAPHICS_Pan(p, cx, cy)                GET_PVTBL(p, IGraphics)->Pan(p, cx, cy)
#define IGRAPHICS_StretchBlt(p, x, y, x1, y1, pbm, c)       GET_PVTBL(p, IGraphics)->StretchBlt(p, x,y,x1,y1,pbm,c)
#define IGRAPHICS_SetAlgorithmHint(p, h)        GET_PVTBL(p, IGraphics)->SetAlgorithmHint(p, h)
#define IGRAPHICS_GetAlgorithmHint(p)           GET_PVTBL(p, IGraphics)->GetAlgorithmHint(p)

#define IGRAPHICS_SetDestination(p, b)          GET_PVTBL(p, IGraphics)->SetDestination(p, b)
#define IGRAPHICS_GetDestination(p)             GET_PVTBL(p, IGraphics)->GetDestination(p)

#define IGRAPHICS_SetStrokeStyle(p, ss)         GET_PVTBL(p, IGraphics)->SetStrokeStyle(p, ss)
#define IGRAPHICS_GetStrokeStyle(p)             GET_PVTBL(p, IGraphics)->GetStrokeStyle(p)

#define IGRAPHICS_DrawEllipticalArc(p, r, s, a)    GET_PVTBL(p, IGraphics)->DrawEllipticalArc(p, r, s, a)
#define IGRAPHICS_DrawRoundRectangle(p, r, aw, ah) GET_PVTBL(p, IGraphics)->DrawRoundRectangle(p, r, aw, ah)


#endif   // AEEGRAPHICS_H

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================

IGRAPHICS Flags

Description:
The bitmap flags to control the behavior of a number of IGraphics functions.

Definition:

AEE_GRAPHICS_NONE:   Default to no flags
AEE_GRAPHICS_FRAME:  if this flag is set, the frame of the geometric primitive is drawn
AEE_GRAPHICS_CLEAR:  If this flag is set, the interior is cleared to the background color
AEE_GRAPHICS_FILL:   If this flag is set, the interior is filled.

Members:
None

Comments:
None



See Also:
IGRAPHICS_SetClip()

================================================================================

AEEAlgorithmHint

Description:
The ENUM specifies which algorithm the application is to use for some
drawing operations.

Definition:
typedef enum {
   ALGORITHM_ACCURATE,
   ALGORITHM_FAST
} AEEAlgorithmHint;

Members:

ALGORITHM_ACCURATE : The implementation is accurate for all kinds of input.  But it might be slow.~
ALGORITHM_FAST:      The implementation is fast but it may not be accurate for some rare input.

Comments:
In the release, the algorithm hint is only effective for IGRAPHICS_DrawPolygon.
The default algorithm hint is ALGORITHM_FAST.  For most applications, this is sufficient.  The
result is accurate for most cases.

See Also:
   IGRAPHICS_DrawPolygon()

=============================================================================

AEEStrokeStyle

Description
This ENUM specifies what stroke style the application is to use when
drawing lines or the outline of shapes.

Definition:
typedef enum {
   STROKE_SOLID,
   STROKE_DOTTED
} AEEStrokeStyle;

Members:

STROKE_SOLID : Lines and outlines are drawn using a solid line.~
STROKE_DOTTED : Lines and outlines are drawn using a dotted line.

Comments:
   None

See Also:
   IGRAPHICS_GetStrokeStyle()
   IGRAPHICS_SetstrokeStyle()

================================================================================

AEEClipShape

Description:
This ENUM specifies shape types for clipping region.

Definition:
typedef enum {
   CLIPPING_NONE,
   CLIPPING_RECT,
   CLIPPING_CIRCLE,
   CLIPPING_ELLIPSE
   CLIPPING_PIE,
   CLIPPING_TRIANGLE,
   CLIPPING_POLYGON
} AEEClipShape;

Members:

CLIPPING_NONE : No clipping shape is specified, the display window is used as default
CLIPPING_RECT : A rectangular shape
CLIPPING_CIRCLE : A circular shape
CLIPPING_ELLIPSE : A ellipse
CLIPPING_PIE : A circular pie
CLIPPING_TRIANGLE : A triangular shape
CLIPPING_POLYGON : An arbitrary polygon


Comments:
The default clipping shape is CLIPPING_RECT and the display window is the default
clipping region. The program can change the clipping region by calling IGRAPHICS_SetClip().

See Also:
AEEClip
AEERect
AEECircle
AEEEllipse
AEEPie
AEETriangle
AEEPolygon
IGRAPHICS_SetClip()

==============================================================================

AEELine

Description:
This structure defines the line segment data type.

Definition:
typedef struct _line
{
   int16 sx, sy;
   int16 ex, ey;
} AEELine;

Members:

sx : X coordinate of the starting point of the line segment
sy : Y coordinate of the starting point of the line segment
ex : X coordinate of the ending point of the line segment
ey : Y coordinate of the ending point of the line segment


Comments:
Mathematically, a line is infinitely long. So AEELine defines a line segment from
(sx, sy) to (ex, ey), instead of a mathematical line. The line segment includes
both end points.

See Also:
IGRAPHICS_DrawLine()

=============================================================================

AEECircle

Description:
This structure defines the circle data type.

Definition:
typedef struct _circle
{
   int16 cx, cy;
   int16 r;
} AEECircle;

Members:

cx : X coordinate of the circle's center
cy : Y coordinate of the circle's center
r : Radius of the circle, in number of pixels

Comments:
None

See Also:
IGRAPHICS_DrawCircle()
IGRAPHICS_Translate()

==============================================================================

AEEEllipse

Description:
This structure defines the ellipse data type.

Definition:
typedef struct _ellipse
{
   int16 cx, cy;
   int16 wx;
   int16 wy;
} AEEEllipse;

Members:

cx : X coordinate of the ellipse's center
cy : Y coordinate of the ellipse's center
wx : Semi-axis length of the ellipse along x-axis
wy : Semi-axis length of the ellipse along y-axis


Comments:
All ellipses are axis-aligned. However, the major axis can be either aligned with the 
x-axis or y-axis.

See Also:
IGRAPHICS_DrawEllipse()
IGRAPHICS_Translate()

==============================================================================

AEEArc

Description:
This structure defines the circular arc data type.

Definition:
typedef struct _arc
{
   int16 cx, cy;
   int16 r;
   int16 startAngle;
   int16 arcAngle;
} AEEArc;

Members:

cx : X coordinate of the center of the reference circle
cy : Y coordinate of the center of the reference circle
r : Radius of the reference circle
startAngle : The angle, in degrees, from which the circular arc begins
arcAngle : The angle of the circular arc, in degrees


Comments:
The first three fields are identical to that of AEECircle . This makes it convenient
to explicitly cast an AEEArc structure to AEECircle when needed.

See Also:
AEECircle
IGRAPHICS_DrawArc()
IGRAPHICS_Translate()

==============================================================================

AEEPie

Description:
This structure defines the circular pie data type.

Definition:
typedef struct _pie
{
   int16 cx, cy;
   int16 r;
   int16 startAngle;
   int16 arcAngle;
} AEEPie;

Members:

cx : X coordinate of the center of the reference circle
cy : Y coordinate of the center of the reference circle
r : Radius of the reference circle
startAngle : The angle, in degrees, from which the circular arc begins
arcAngle : The angle of the circular arc, in degrees


Comments:
The parameters in this data structure are identical to that of AEEArc . This makes
it convenient to explicitly cast an AEEPie type to AEEArc if necessary.

See Also:
AEEArc
IGRAPHICS_DrawPie(), IGRAPHICS_Translate()

==============================================================================

AEETriangle

Description:
This structure defines the triangle data type.

Definition:
typedef struct _triangle {
   int16 x0, y0;
   int16 x1, y1;
   int16 x2, y2;
} AEETriangle;

Members:

x0 : X coordinate of the first vertex of the triangle.
y0 : Y coordinate of the first vertex.
x1 : X coordinate of the second vertex.
y1 : Y coordinate of the second vertex.
x2 : X coordinate of the third vertex.
y2 : Y coordinate of the third vertex.


Comments:
None

See Also:
IGRAPHICS_DrawTriangle()
IGRAPHICS_Translate()

==============================================================================

AEEPolygon

Description:
This structure defines the polygon data type.

Definition:
typedef struct _polygon
{
   int16 len;
   AEEPoint * points;
} AEEPolygon;

Members:
len : Number of vertices of the polygon
points : Array of vertices of the polygon

Comments:
None

See Also:
   AEEPoint
   IGRAPHICS_DrawPolygon()
   IGRAPHICS_Translate()

=============================================================================

AEEPolyline

Description:
This structure defines the polyline data type. Polyline is a sequence of connected
line segments.

Definition:
typedef struct _polyline
{
   int16 len;
   AEEPoint * points;
} AEEPolyline;

Members:
len : Number of points in the polyline
points : Array of points in the polyline

Comments:
This data type has exactly the same structure as AEEPolygon . This enables convenient
casting between AEEPolyline and AEEPolygon .

See Also:
   AEEPoint
   IGRAPHICS_DrawPolyline()
   IGRAPHICS_Translate()

==============================================================================

AEEClip

Description:
This structure defines the dimensions of a clipping shape that is used to restrict
the region in which an IGraphics drawing operations takes effect.

Definition:
typedef struct _clipshape
{
   AEEClipShape type;
   union {
      AEERect rect;
      AEECircle circle;
      AEEEllipse ellipse;
      AEEPie pie;
      AEETriangle triangle;
   } shape;
} AEEClip;

Members:

type : Type of clipping shape
shape : Union of clipping shape
shape.rect : Rectangular clipping shape
shape.circle : Circular clipping shape
shape.ellipse : Ellipse as the clipping shape
shape.pie : Circular pie as the clipping shape
shape.triangle : Triangular clipping shape


Comments:
Shape is a UNION type. Only one of the shapes is effective at any moment. The program
has to check the type and then access shape for the corresponding clipping shape.

See Also:
AEEClipShape

==============================================================================

AEEPaintMode

Description:
This ENUM specifies the raster operation types for drawing.

Definition:
typedef enum {
   AEE_PAINT_COPY,
   AEE_PAINT_XOR
} AEEPaintMode;

Members:

AEE_PAINT_COPY : Copy raster operation
AEE_PAINT_XOR : XOR raster operation

Comments:
When the paint mode is set to AEE_PAINT_COPY, new content shall overwrite the old
content in the display buffer. When the paint mode is set to AEE_PAINT_XOR, the
"xor-ed" result of the new content and the old is written into the display buffer.
IGraphics's paint mode is set to AEE_PAINT_COPY as default. The program can change
the paint mode by calling IGRAPHICS_SetPaintMode() .

See Also:
IGRAPHICS_SetPaintMode()

==============================================================================
   INTERFACES DOCUMENTATION
==============================================================================

IGraphics Interface

Description:
   This interface provides the 2D graphics services. It provides services
such as: 
~
drawing geometric primitives,~
seting colors,~
seting pen size,~
others.~
*

The following header file is required:~
AEEGraphics.h
==============================================================================

Function: IGRAPHICS_AddRef()

Description:
	This function is inherited from IBASE_AddRef().
	
==============================================================================

Function: IGRAPHICS_Release()

Description:
	This function is inherited from IBASE_Release().
==============================================================================

Function: IGRAPHICS_SetBackground()

Description:
   This function sets the RGB values of the current background color.
   This color will remain in effect until it is set to different values.

Prototype:
   RGBVAL IGRAPHICS_SetBackground(IGraphics *pIGraphics, uint8 r, uint8 g, uint8 b);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   r:          Red component of the RGB value.
   g:          Green component of the RGB value.
   b:          Blue component of the RGB value.

Return Value:
   The updated RGB value for the current background color.

Comments:
   The program shall not show the new background color until
   IGRAPHICS_ClearRect is called.

   The value of each color component (red, green and blue) should be
   a positive value between 0 and 255.  Any integer values beyond this
   range will be converted to a value within the range from 0 to 255.
   However, the conversion itself is machine-dependent.  The user is
   encouraged to use values within the range to have predictable color
   effect.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetBackground()

Description:
   This function queries the RGB values of the current background color.

Prototype:
   void IGRAPHICS_GetBackground(IGraphics *po, uint8 *r, uint8 *g, uint8 *b);

Parameters:
   po:   Pointer to the IGraphics interface object.
   r:    Pointer to the red component of the RGB color.
   g:    Pointer to the green component of the RGB color.
   b:    Pointer to the blue component of the RGB color.

Return Value:
   None.

Comments:
   The default background color is white.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_SetColor()

Description:
   This function sets the current foreground color's RGBA value. All subsequent graphics objects
   will be drawn in this color until this function is called again for different
   RGBA values.

Prototype:
   RGBVAL IGRAPHICS_SetColor(IGraphics *pIGraphics, uint8 r, uint8 g, uint8 b, uint8 alpha);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   r:          Red component of the RGBA value.
   g:          Green component of the RGBA value.
   b:          Blue component of the RGBA value.
   alpha:      Alpha value.

Return Value:
   The updated RGBA value for foreground.

Comments:
   In the current version, "alpha" value is just a placeholder that has no
   effect on the foreground color.  Eventually this value will be used to support
   transparent color and color blending.

   The default foreground color is black.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetColor()

Description:
   This function gets the RGBA value for the current foreground color.

Prototype:
   void IGRAPHICS_GetColor(IGraphics *po, uint8 *r, uint8 *g, uint8 *b, uint8 *alpha);

Parameters:
   po:      Pointer to the IGraphics interface object.
   r:       Pointer to the red component of the RGBA value.
   g:       Pointer to the green component of the RGBA value.
   b:       Pointer to the blue component of the RGBA value.
   alpha:   Alpha value.

Return Value:
   None

Comments:
   "alpha" is just a place holder in the current version.  It has no effect
   on the foreground color.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_SetFillMode()

Description:
   This function turns on the fill-mode for all subsequent closed geometric
   primitives by passing in TRUE.  It turns off the fill-mode by passing in
   FALSE.

Prototype:
   boolean IGRAPHICS_SetFillMode(IGraphics *pIGraphics, boolean fFill)

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   fFill:      Boolean value: TRUE to turn on the fill mode, FALSE to turn
               off the fill-mode.

Return Value:
   The updated value for the current fill-mode. ~ 
   TRUE: for on 
   FALSE: for off.

Comments:
   If an invalid value (other than TRUE or FALSE) is passed as input
   parameter, the fill mode is set to FALSE by default.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetFillMode()

Description:
   This function queries the current fill-mode.  TRUE for fill and FALSE for
   not-fill.

Prototype:
   boolean IGRAPHICS_GetFillMode(IGraphics *pIGraphics);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.

Return Value:
   It returns a boolean value:~ 
   TRUE: for fill 
   FALSE: for not-fill.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================
Function: IGRAPHICS_SetFillColor()

Description:
   This function sets the fill color for all subsequent drawings of closed
   geometric primitives.

Prototype:
   RGBVAL IGRAPHICS_SetFillColor(IGraphics *pIGraphics, uint8 r, uint8 g,
                                 uint8 b, uint8 alpha);


Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   r:          Red component of the RGBA value.
   g:          Green component of the RGBA value.
   b:          Blue component of the RGBA value.
   alpha:      Alpha value.

Return Value:
   The updated RGBA value of the current fill color.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetFillColor()

Description:
   This function gets the RGBA value for the current fill color.

Prototype:
   void IGRAPHICS_GetFillColor(IGraphics *po, uint8 *pRed, uint8 *pGreen,
                               uint8 *pBlue, uint8 *pAlpha)

Parameters:
   po:      Pointer to the IGraphics interface object.
   pRed:    Red component of the RGBA value.
   pGreen:  Green component of the RGBA value.
   pBlue:   Blue component of the RGBA value.
   pAlpha:  Alpha value.

Return Value:
   None

Comments:
   "pAlpha" is just a place holder in the current version.  The value in "*pAlpha"
   has no effect on the fill color.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_SetPointSize()

Description:
   This function sets the size (width) of a point in terms of number of pixels.

Prototype:
   uint8 IGRAPHICS_SetPointSize(IGraphics *pIGraphics, uint8 u8Size);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   u8Size:     Size (width) of a point in terms of number of pixels.

Return Value:
   The updated value of the point size.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetPointSize()

Description:
   This function gets the current point size in terms of number of pixels.

Prototype:
   uint8 IGRAPHICS_GetPointSize(IGraphics *pIGraphics)

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.

Return Value:
   The current point size.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_SetClip()

Description:
   This function sets the clipping area.  Graphics contents will be displayed
   on the screen only if they are within the clipping area.  If the pointer pShape
   is NULL, it resets the clipping region the window.

Prototype:
   boolean IGRAPHICS_SetClip(IGraphics *pIGraphics, AEEClip *pShape, uint8 nFlag);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pShape:     Pointer to the clipping shape.
   nFlag:      Bitmap flags.

Return Value:
   TRUE: if the clipping area is set successfully, 
   FALSE: otherwise.

Comments:
   If AEE_GRAPHICS_FRAME is set for nFlag, it draws the frame using the current
   foreground color and the paint mode, while setting the clipping region.
   If AEE_GRAPHICS_CLEAR is set for nFlag, it clear the interior of of the clipping
   region while setting it.
   If AEE_GRAPHICS_FILL is set for nFlag, it fills the interior using the current
   fill color and the current paint mode, while setting the clipping region.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetClip()

Description:
   This function gets the current clipping area, specified by a closed geometric
   primitive.

Prototype:
   boolean IGRAPHICS_GetClip(IGraphics *pIGraphics, AEEClip *pShape);

Parameters:
   pIGraphics:    Pointer to the IGraphics interface object.
   pShape:        Pointer the current clipping shape.

Return Value:
   TRUE: if the query is successful, 
   FALSE: otherwise.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_SetViewport()

Description:
   A world-coordinate area selected for display is called a window.  An area on a
   display device to which a window is mapped is called a viewport.
   This function sets the viewport (a rectangular area) for drawing.  The default
   viewport is the screen without a frame  If pRect is NULL, this function resets
   the viewport to the default and it also resets the clipping region to the default
   viewport.
~
===img src="../../images/transformation.gif"> 
*


Prototype:
   boolean IGRAPHICS_SetViewport(IGraphics *pIGraphics, AEERect *pRect, uint8 nFlag);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pRect:      Pointer to the rectangular shape, which specifies the area
               for display.
   nFlag:      Bitmap flags for different options.

Return Value:
   A boolean value:~ 
   TRUE: for success
   FALSE: for failure.

Comments:
   It returns TRUE only if the displayable area is nonempty and the viewport
   is completely contained in the physical screen.  Otherwise, it returns FALSE.

   When it returns FALSE for any reason, the previous window settings are retained.
~
   If AEE_GRAPHICS_FRAME is set, it draws the frame. The minimum value for dx, dy is 3 pixel.~
   If AEE_GRAPHICS_NONE is set, the minimum value for dx, dy is 1 pixel.~
   If AEE_GRAPHICS_CLEAR is set, it clears the viewport to the background color.~
*

   While the input of other IGraphics functions are in the world-coordinate system,
   the input of this function is in screen-coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetViewport()

Description:
   This function gets the current viewport (a rectangular area in the screen
   coordinate frame) for drawing.

Prototype:
   boolean IGRAPHICS_GetViewport(IGraphics *pIGraphics, AEERect *pRect, boolean *framed);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pRect:      Pointer to the rectangular shape that specifies the current
               viewport area for drawing.
   framed:     TRUE if the viewport is framed, FALSE otherwise.

Return Value:
   A boolean value:~ 
   TRUE: for success
   FALSE: for failure.

Comments:
   The output is in screen coordinate system.~
===img src="../../images/transformation.gif"> 
Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None
==============================================================================

Function: IGRAPHICS_SetPaintMode()

Description:
   This function sets the paint mode. In AEE_PAINT_COPY mode, the new
   graphics content overwrites the previous one.  In AEE_PAINT_XOR mode,
   the color will be determined by the XOR binary operation of the
   old and new colors.

   Given an arbitrary background and an arbitrary new foreground color,
   XOR the same foreground color against the background with recover the
   background.

Prototype:
   AEEPaintMode IGRAPHICS_SetPaintMode(IGraphics *pIGraphics, AEEPaintMode mode);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   mode:       The enum value for paint mode.

Return Value:
   The update enum value for paint mode.

Comments:
   If an invalid integer (enum) value is passed as the input "mode"
   parameter, the paint mode is set to AEE_PAINT_COPY by default.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetPaintMode()

Description:
   This function gets the current paint mode.

Prototype:
   AEEPaintMode IGRAPHICS_GetPaintMode(IGraphics *pIGraphics);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.

Return Value:
   The enum value of the current paint mode.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_GetColorDepth()

Description:
   This function gets the color depth of the device.

Prototype:
   short IGRAPHICS_GetColorDepth(IGraphics *pIGraphics);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.

Return Value:
   The color depth.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawPoint()

Description:
   This function draw a point with the current point size and foreground color.

Prototype:
   int IGRAPHICS_DrawPoint(IGraphics *pGraphics, AEEPoint *pPoint);

Parameters:
   pGraphics:  Pointer to the IGraphics interface object.
   pPoint:     Pointer to the point to be drawn.

Return Value:

   An error code is returned to indicate the status of this transaction:~
   SUCCESS: if the pointer is successfully drawn; 
   EBADPARM: is one of the input parameters is invalid.
   
Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawLine()

Description:
   This function draws a line segment, given the starting point and ending
   point.  The
   color of the line the is the current foreground color.

Prototype:
   int IGRAPHICS_DrawLine(IGraphics *pGraphics, AEELine *pLine);

Parameters:
   pGraphics:     Pointer to the IGraphics interface object.
   pLine:         Pointer to the line to be drawn.

Return Value:
   An error code that indicates the status of this transaction:~
   EBADPARM:if one of the parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawRect()

Description:
   This function draws an axis-aligned rectangle.
   The color of the outline is the current foreground
   color.  If the fill-mode is turned on, the interior will be filled with the
   current fill color.

Prototype:
   int IGRAPHICS_DrawRect(IGraphics *pIGraphics, AEERect *pRect);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pRect:      Pointer to the rectangle to be drawn.

Return Value:
   The error code indicating the status of this transaction:~ 
   EBADPARM: if one of the input parameters is invalid; 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawCircle()

Description:
   This function draws a circle.
   The color of the outline is the current foreground color.  If the
   fill-mode is turned on, the interior of the circle will be filled with the
   current fill color.

Prototype:
   int IGRAPHICS_DrawCircle(IGraphics *pIGraphics, AEECircle *pCircle);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pCircle:    Pointer to the circle to be drawn.

Return Value:
   The error code that indicates the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESSL: otherwise.

Comments:
   The input circle is in the window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawArc()

Description:
   This function draws a circle arc. The color of the arc is the current
   foreground color.

Prototype:
   int IGRAPHICS_DrawArc(IGraphics *po, AEEArc *pArc);

Parameters:
   po:      Pointer to the IGraphics interface object.
   pArc:    Pointer to the arc to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawPie()

Description:
   This function draws a circular pie.
   The color of the outline is the current foreground color.  If
   fill-mode is turned on, the interior of the pie shall be filled with the
   current fill color.

Prototype:
   int IGRAPHICS_DrawPie(IGraphics *po, AEEPie *pPie);

Parameters:
   po:      Pointer to the IGraphics interface object.
   pPie:    Pointer to the pie to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.
   Fill mode is not supported in this version.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawEllipse()

Description:
   This function draws an ellipse.
   The color of the outline is the current foreground color. If
   the fill-mode is turned on, its interior shall be filled with the current
   fill-color.

Prototype:
   int IGRAPHICS_DrawEllipse(IGraphics *pIGraphics, AEEEllipse *pEllipse)

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pEllipse:   Pointer to the ellipse to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawTriangle()

Description:
   This function draws a triangle.
   The color of the outline is the current foreground color. If
   the fill-mode is turned on, its interior shall be filled with the current
   fill-color.

Prototype:
   int IGRAPHICS_DrawTriangle(IGraphics *pIGraphics, AEETriangle *pTriangle);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pTriangle:  Pointer to the triangle to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawPolygon()

Description:
   This function draws a polygon.
   The color of the outline is the current foreground color. If
   the fill-mode is turned on, its interior shall be filled with the current
   fill-color.

Prototype:
   int IGRAPHICS_DrawPolygon(IGraphics *pIGraphics, AEEPolygon *pPolygon);

Parameters:
   pIGraphics:    Pointer to the IGraphics interface pointer.
   pPolygon:      Pointer to the polygon to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

   There are two implementation for this drawing operation.  An application can
   choose the implementation by calling IGRAPHICS_SetAlgorithmHint. By default,
   ALGORITHM_FAST is set.  ALGORITHM_ACCURATE guarantees accurate display for each pixel
   for all input polygons, but the execution speed is slow.  ALGORITHM_FAST gives much
   faster execution and is sufficiently accurate for most polygons.

   Even if ALGORITHM_ACCURATE is selected, IGRAPHICS_DrawPolygon() will automatically
   use the fast implementation if one of the following conditions is true:
   1. The paint mode (set by IGRAPHICS_SetPaintMode())is AEE_PAINT_COPY.
   2. The frame color (set by IGRAPHICS_SetColor()) and the fill color (set by
      IGRAPHICS_SetFillColor()) are identical.

   In other words, the only case when the "accurate and slow" implementation will be used is
   if all the following conditions are true:
   1. ALGORITHM_ACCURATE is selected.
   2. The frame color is different from the fill color.
   3. The paint mode is AEE_PAINT_XOR.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_DrawPolyline()

Description:
   This function draws a polyline.
   The color of the outline is the current foreground color.

Prototype:
   int IGRAPHICS_DrawPolyline(IGraphics *pIGraphics, AEEPolyline *pPolyline);

Parameters:
   pIGraphics: Pointer to the IGraphics interface object.
   pPolyline:   Pointer to the polyline to be drawn.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   The input is in window coordinate system.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_ClearRect()

Description:
   This function clears a rectangular area of the window by filling it with the
   background color.

Prototype:
   int IGRAPHICS_ClearRect(IGraphics *po, AEERect *pRect);

Parameters:
   po:      Pointer to IGraphics interface pointer.
   pRect:   Pointer to the rectangle that specifies the area to cleared.

Return Value:
   The error code indicating the status of this transaction: 
   EBADPARM: if one of the input parameters is invalid, 
   SUCCESS: otherwise.

Comments:
   IGRAPHICS_SetBackground has no effect until IGRAPHICS_ClearRect is called.

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_EnableDoubleBuffer()

Description:
   This function turns on double buffering by passing in TRUE and turns it off
   by passing in FALSE.

Prototype:
   boolean IGRAPHICS_EnableDoubleBuffer(IGraphics *po, boolean flag);

Parameters:
   po:      Pointer to IGraphics interface object.
   flag:    A boolean value: TRUE to enable double buffering, FALSE to disable
            double buffering.

Return Value:
   It returns 
   TRUE: if the transaction is successful, 
   FALSE: otherwise. ~ 
   If double buffering is not supported by the system, it will return FALSE when the client
   tries to turn on the double buffering.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_Update()

Description:
   If double-buffering is supported and enabled, it swaps the two buffers and
   pushes the offscreen buffer to the screen. If the double-buffering is not
   supported or it is not enabled, this function has no effect.

Prototype:
   void IGRAPHICS_Update(IGraphics *po);

Parameters:
   po:      Pointer to the IGraphics interface object.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_Translate()

Description:
   This function translates the window's origin in the world-coordinate system.
   Namely, a  different area of the world will be displayed. The
   units are in number of pixels.  The window's origin is always its upper left
   corner, which is the default at the origin of the world-coordinate system.
   As a result of the translation, all geometric primitives will be translated by the
   same amount, but in the opposite direction in the viewport. Refer to
   IGRAPHICS_SetViewport for the definition of "window" and "viewport."

Prototype:
   void IGRAPHICS_Translate(IGraphics *po, int16 x, int16 y);

Parameters:
   po:      Pointer to the IGraphics interface object.
   x:       The window's origin will be translated by x pixels along X-axis.
   y:       The window's origin will be translated by y pixels along Y-axis.

Return Value:
   None

Comments:
   Conceptually, it helps to imagine the "window" as the camera.  This camera can be
   translated such that it points to different area of the world.  The world is the scene
   a programmer has designed.~
===img src="../../images/transformation.gif"> 

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==============================================================================

Function: IGRAPHICS_Pan()

Description:
   This function re-centers the window in the world-coordinate system.  In some
   sense, (cx, cy) are the world coordinates at which the camera is pointed.

Prototype:
   void IGRAPHICS_Pan(IGraphics *po, int16 cx, int16 cy);

Parameters:
   po:      Pointer to the IGraphics interface object.
   cx:      X coordinate of the new center in world coordinate frame.
   cy:      Y coordinate of the new center in world coordinate frame.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

============================================================================

Function: IGRAPHICS_ClearViewport()

Description:
   This function clears the current viewport to the background color.

Prototype:
   void IGRAPHICS_ClearViewport(IGraphics *po);

Parameters:
   po:      Pointer to the IGraphics interface object.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.0

See Also:
   None

==========================================================================

Function: IGRAPHICS_StretchBlt()

Description:
   This function stretch-blt an input bitmap to a specified position and
   width/height.

Prototype:
   void IGRAPHICS_StretchBlt(IGraphics* po, int16 destX, int16 destY,
                        int16 destW, int16 destH, const void * pbmSource,
                        AEERasterOp dwRopCode)

Parameters:
   po:      Pointer to the IGraphics interface object.
   destX:   Specifies the x-coordinates of the upper left corner of the
            destination rectangular area.
   destY:   Specifies the y-coordinates of the upper left corner of the
            destination rectangular area.
   destW:   Destination width.
   destH:   Destination height.
   pbmSource:  Pointer to a Windows BMP structure containing the source
            bitmap.  The data beening pointed is without the AEE header.
            The data should start from BITMAPFILEHEADER. The BMP should be
            noncompressed.
   dwRopCode:  Specifies the Raster operation that must be used
            while doing the bit-block transfer.

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

============================================================================

Function: IGRAPHICS_SetAlgorithmHint()

Description:
   This function sets the algorithm hint for some drawing operations.

Prototype:
   void IGRAPHICS_SetAlgorithmHint(IGraphics *po, AEEAlgorithmHint hint);

Parameters:
   po:      Pointer to the IGraphics interface object
   hint:    Algorithm hint

Return Value:
   None

Comments:
   The algorithm hint is only effective for IGRAPHICS_DrawPolygon.
   By default, the algorithm hint is set to ALGORITHM_FAST.

Side Effects:
   The algorithm hint will be set to the input parameter "hint."

Version:
   Introduced BREW Client 1.1

See Also:
   AEEAlgorithmHint
   IGRAPHICS_GetAlgorithmHint()
   IGRAPHICS_DrawPolygon()

============================================================================

Function: IGRAPHICS_GetAlgorithmHint()

Description:
   This function queries the current algorithm hint.

Prototype:
   AEEAlgorithmHint  IGRAPHICS_GetAlgorithmHint(IGraphics *po);

Parameters:
   po:      Pointer to the IGraphics interface object

Return Value:
   The current algortihm hint.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEEAlgorithmHint
   IGRAPHICS_SetAlgorithmHint()
   IGRAPHICS_DrawPolygon()

==============================================================================
Function: IGRAPHICS_SetDestination()

Description:
   This function sets the destination bitmap to which IGRAPHICS functions
   will render.

Prototype:
   int IGRAPHICS_SetDestination(IGraphics *po, IBitmap *pDst)

Parameters:
   po   [in]: Pointer to IGRAPHICS interface object.
   pDst [in]: IBitmap interface pointer that will be used for rendering,
              or NULL to set to screen bitmap.

Return Value:
   SUCCESS: if successful
   EFAILED: if unable to set destination

Comments:
   When IGraphics is instantiated, the default bitmap is the screen bitmap.
   Use this function will NULL for pDst to restore to default.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

============================================================================
Function: IGRAPHICS_GetDestination()

Description:
   This function gets the destination bitmap to which IGRAPHICS functions
   will render.

Prototype:
   IBitmap* IGRAPHICS_GetDestination(IGraphics *po);

Parameters:
   po [in]: Pointer to IGRAPHICS interface object.

Return Value:
   Pointer to the current destination bitmap.

Comments:
   If this function ever returns NULL, it is an error.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

==============================================================================
Function: IGRAPHICS_SetStrokeStyle()

Description:
   This function sets the Stroke Style that the IGRAPHICS draw functions
   will use.

Prototype:
   AEEStrokeStyle IGRAPHICS_SetstrokeStyle(IGraphics *po, AEEStrokeStyle strokeStyle)

Parameters:
   po          [in]: Pointer to IGRAPHICS interface object.
   strokeStyle [in]: Stroke style to be used for drawing.

Return Value:
   Returns the previous stroke style.

Comments:
   When IGraphics is instantiated, the default stroke style is STROKE_SOLID.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

============================================================================
Function: IGRAPHICS_GetStrokeStyle()

Description:
   This function gets the Stroke Style that the IGRAPHICS draw functions
   will use.

Prototype:
   AEEStrokeStyle IGRAPHICS_GetStrokeStyle(IGraphics *po)

Parameters:
   po           [in]: Pointer to IGRAPHICS interface object.

Return Value:
   Returns the current stroke style.

Comments:
   None.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

============================================================================
Function: IGRAPHICS_DrawEllipticalArc()

Description:
   This function draws an elliptical arc.
   The color of the outline is the current foreground color.  If the
   fill-mode is turned on, the interior of the arc will be filled with the
   current fill color.
   The start angle begins at the postive x axis and goes counter clockwise.

Prototype:
   int IGRAPHICS_DrawEllipticalArc(IGraphics *po,
                                   AEERect *pRect,
                                   int16 startAngle, int16 arcAngle )

Parameters:
   po           [in]: Pointer to IGRAPHICS interface object.
   pRect        [in]: Pointer to the rectangle containing the ellipse
   startAngle   [in]: start angle of the arc in degrees, 0 is positive
                      x-axis increasing angles go counterclockwise.
   arcAngle     [in]: size of the arc in degrees

Return Value:
   Error code that indicates the status of the operation:
      EBADPARM: if one of the parameters is invalid,
      SUCCESS: otherwise

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

============================================================================
Function: IGRAPHICS_DrawRoundRectangle()

Description:
   This function draws a rectangle with rounded corners.
   The color of the outline is the current foreground color.  If the
   fill-mode is turned on, the interior of the rectangle will be filled
   with the current fill color.


Prototype:
   int IGRAPHICS_DrawRoundRectangle(IGraphics *po,
                                    AEERect *pRect,
                                    int16 arcWidth, int16 arcHeight )

Parameters:
   po           [in]: Pointer to IGRAPHICS interface object.
   pRect        [in]: Pointer to the rectangle to be drawn
   arcWidth     [in]: width of ellipse used for corner arc
   arcHeight    [in]: height of ellipse used for corner arc

Return Value:
   Error code that indicates the status of the operation:
      EBADPARM: if one of the parameters is invalid,
      SUCCESS: otherwise

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   None

==========================================================================*/

