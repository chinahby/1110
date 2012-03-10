/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEEGraphics.c

SERVICES:  AEE Graphics Interfaces

GENERAL DESCRIPTION:  Provides support for IGraphics services
in AEE.  These include:

- Circles
- Arcs
- Triangles
- Polygons, etc


PUBLIC CLASSES AND STATIC FUNCTIONS: IGraphics

Copyright ?1999-2002 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

/*===================================================================================

            INCLUDES AND VARIABLE DEFINITIONS

===================================================================================*/



/*--------------------------------

      Defines

--------------------------------*/




/*--------------------------------

      Include Files

--------------------------------*/

#include "AEE_OEM.h"
#include "AEE.h"
#include "OEMDisp.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "AEEGraphics_priv.h"
#include "BMPServices_priv.h"
#include "AEEDispatch_priv.h"
#include <limits.h>

#ifdef _MSC_VER 
#pragma pack(push,1)
#pragma check_stack(on)
#endif



/*===============================================================================

                     PRIVATE DATA DECLARATIONS

===============================================================================*/

static short g_usEndianTest = 1;

#define BIG_ENDIAN() ((char)g_usEndianTest != *((char *)&g_usEndianTest))

#define TRY(exception, func) \
   if (SUCCESS != (exception = func)) {\
      goto exception##bail; \
   }

#define THROW(exception, errno) \
   exception = errno; \
   goto exception##bail;

#define CATCH(exception) exception##bail: if (exception != SUCCESS)


// Flags used for handling geometric primitives
// The first flags are defined in AEEGraphics.h
//#define  AEE_GRAPHICS_NONE    0x00
//#define  AEE_GRAPHICS_FRAME   0x01
//#define  AEE_GRAPHICS_CLEAR   0x02

// The next 2 flags are only used internally so that we don't
// have to write seperate functions for drawing and clipping.
//#define  AEE_GRAPHICS_FILL    0x04
#define  AEE_GRAPHICS_CLIP    0x01
#define  AEE_GRAPHICS_ARC     0x10

// Window area for display. All values are in
// screen coordinates
typedef struct _winrect {
   int16 xMin, xMax, yMin, yMax;
} WinRect;


// Interval is a pair of integers that specifies the
// leftend and the rightend of the interval.
typedef struct {
   int16 xMin;
   int16 xMax;
} Interval;

// All clipping data are in the original screen coordinate system
typedef struct _clipInfo {
   WinRect  rect;          // Bounding box of the clipping shape for quick rejection test
   Interval *intervals;    // List of intervals for clipping, one for each scanning line
} ClipInfo;

typedef struct _edge_accurate {
   struct _edge_accurate   *next;
   int16                   yUpper, yLower;
   int16                   xUpper;        // This field is added for sorting!!!!
   int16                   xIntersectMin, xIntersectMax; // To support XOR, we need to keep track
                                                // both ends of the intersection.  Otherwise
                                                // a line with slope less than one may not have
                                                // the right color with XOR mode.
   int16                   dx, dy, count; // integer implementation of the inverse slope
   int16                   abs_2dx;       // abosolute value of dx times 2
   int16                   dy2;           // dy times 2
} EdgeAccurate;


typedef struct _horizontalEdge {
   struct _horizontalEdge  *next;
   int16                   y;
   int16                   xMin, xMax;
} HorizontalEdge;

typedef struct _edge_fast {
   struct   _edge_fast *next;
   int16    yUpper;
   int16    xIntersect;
   int16    dx, dy, count;  // integer implementation of the inverse slope
   int16    abs_2dx;    // abosolute value of dx times 2
   int16    dy2;        // dy times 2
} EdgeFast;

typedef struct _fraction {
   int e;   // enumerator
   int d;   // denominator
} FractionType;

/*===========================================================================

Internal data structures to support drawing ellipses and round rectangles

===========================================================================*/
typedef struct {
   int16 x;                     /* point on ellipse */
   int16 y;
   int16 xi;                    /* xi,y is inside ellipse  */

   int16 last_x;                /* previous point on ellipse */
   int16 last_y;
   int16 last_xi;
} AEEEllipsePoint;

typedef struct {
   AEEPoint p;
   AEEPoint a;
   AEEPoint d;
   int16 quadrant;
   int16 count;

   AEEEllipsePoint reg;         /* regular point */
   AEEEllipsePoint mir;         /* mirror point */

   // Cached values for ELLIPSE_STEP
   boolean q13;
} AEEEllipseInfo;

typedef struct {
  int32 r1;
  int32 ax2;
  int32 ay2;
  int32 ax2pydy;
  int32 ay2pxdx;
  int32 xdx;
  int32 xdx_offset;
  int32 ydy;
  int32 ydy_offset;
} AEEStepInfo32;

typedef struct {
  int64 r1;
  int64 ax2;
  int64 ay2;
  int64 ax2pydy;
  int64 ay2pxdx;
  int64 xdx;
  int64 xdx_offset;
  int64 ydy;
  int64 ydy_offset;
} AEEStepInfo64;

typedef struct {
   boolean incX;                /* if true, inc X, otherwise inc Y */
   int16 increment;             /* increment to apply to X or Y */
   int16 step;                  /* fractional increment for other dim */
   int16 accum;                 /* accumulation of fractional increments */

   int16 x;                     /* most recent point on the line */
   int16 y;

   // min and max x values for a particular y
   int16 left_x;
   int16 right_x;
} AEEEllipseLineInfo;

typedef struct {
  int16 y;                      /* current line */
  int16 ym;                     /* current mirror line */
  int16 left_x, right_x;        /* left and right x values for left side
                                   of ellipse */
  int16 left_xm, right_xm;      /* left and right x values for right side
                                   of ellipse */
} AEEEllipseArcInfo;

/*===============================================================================

                     PUBLIC DATA DECLARATIONS

===============================================================================*/

#ifdef _MSC_VER 
#pragma pack(pop) 
#endif

OBJECT(QGraphics)
{
   DECLARE_VTBL(IGraphics)

   // Private object data
   long           m_nRefs;       // reference count
   uint16         m_cx;          // width of the physical screen
   uint16         m_cy;          // height of the physical screen

   // handles to other AEE services
   IShell         *m_pIShell;
   IDisplay       *m_pIDisp;     // handle to the display interface
   IBitmap        *m_pbmDst;     // handle to destination bitmap

   // information for the graphics state machine, 
   // such as the color, buffer, font, point size, etc..

   NativeColor    m_bg;        // background color
   NativeColor    m_fg;        // foreground color
   NativeColor    m_fl;        // fill color

   // We have to keep a copy of the original RGB color values because NativeColor cannot
   // be uniquely mapped back to RGBVAL.
   RGBVAL            m_bgRGB;
   RGBVAL            m_fgRGB;
   RGBVAL            m_flRGB;

   AEERasterOp       m_ro;             // raster operation
   AEEAlgorithmHint  m_hint;           // which implementation to pick
   AEEPoint          m_windowOrigin;   // origin of the window in the world coordinate system
   AEEPoint          m_viewportOrigin; // origin of the viewport in the screen coordinate system
   AEEPoint          m_translate;      // net translation from world to the screen
   WinRect           m_viewport;       // display window area
   AEEClip           m_clip;           // clipping shape, just a record
   ClipInfo          m_clipInfo;       // used by non-rectangular clipping
                                       // internal clipping info

   AEEStrokeStyle    m_strokeStyle;    // which stroke style to use when drawing
   boolean           m_clrBufEnabled;  // TRUE for double buffer, FALSE for single buffer
   boolean           m_framed;         // is the window framed?
   boolean           m_fill;           // fill mode -- TRUE for fill and FALSE for non-fill
   uint8             m_pointSize;      // point size
   uint8             m_clrDepth;    // color depth
};


/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

// TODO - change this to be scaled by 2^15 instead of 10^4 (even 2^14 would
// have more resolution and better performance.

// Since we have no floating point support, we use the following precomputed table
// for sin(x).  This table stores sin(x) for integral degrees in [0, 45].  It's 
// accurate upto it's third digit after the decimal point.
// The value stores in the table means x/10000.  For example, 7071 really means 
// 0.7071.
const int16 SIN[91] = {
   0, 175, 349, 523, 698, 872, 1045, 1219, 1392, 1564, 1736, 
   1908, 2079, 2250, 2419, 2588, 2756, 2924, 3090, 3256, 3420, 
   3584, 3746, 3907, 4067, 4226, 4384, 4540, 4695, 4848, 5000, 
   5150, 5299, 5446, 5592, 5736, 5878, 6018, 6157, 6293, 6428, 
   6561, 6691, 6820, 6947, 7071, 7193, 7314, 7431, 7547, 7660, 
   7771, 7880, 7986, 8090, 8192, 8290, 8387, 8480, 8572, 8660, 
   8746, 8829, 8910, 8988, 9063, 9135, 9205, 9272, 9336, 9397, 
   9455, 9511, 9563, 9613, 9659, 9703, 9744, 9781, 9816, 9848, 
   9877, 9903, 9925, 9945, 9962, 9976, 9986, 9994, 9998, 10000      
};

#define SIN_ACCURACY_INVERSE        10000
#define HALF_SIN_ACCURACY_INVERSE   5000
#define ceiling(x,y) ((x+y-1)/y)



// 
// Global constants to support dotted drawing
//
// For drawing dotted lines, a variable, usually called nDotPhase keeps track
// of when a pixel should be drawn or not drawn. The algorithms are careful
// that the dot phase is continuous at verticies so that the dot pattern is
// continuous.
//
// In some cases end points of lines or arcs are switched to make drawing
// easier, in these cases, the dot phase must be reveresed and translated so
// that the dot pattern is still continuous. The reversal operation is
// performed by indexing in to the DotPhaseMap; the translation operation is
// performed by doing an add modulo the repeat count.
//
// There are macros defined in the macros section below to help with dot phase
// manipulations.
//
// We picked a 2-on 2-off pattern so that the % operations will be shifts for
// performance.
// For a 3-on 3-off pattern use the following values:
// DOT_ON_COUNT     3
// DOT_REPEAT_COUNT 6
// DotPhaseMap =    { 2, 1, 0, 5, 4, 3 }
//

#define DOT_ON_COUNT        2
#define DOT_REPEAT_COUNT    4

static const int16 DotPhaseMap[DOT_REPEAT_COUNT] = { 1, 0, 3, 2 };

typedef enum {
   ARC,
   PIE, 
   CAP
} arcType;


// enum constants used in Sutherland-Hodgeman polygon clipping
typedef enum {LEFT, RIGHT, BOTTOM, TOP} RectEdge;


/*============================================================================
   Implementations of the functions in the IGraphics VTBL
============================================================================*/
static uint32           QGraphics_AddRef(IGraphics *po);
static uint32           QGraphics_Release(IGraphics *po);
static RGBVAL           QGraphics_SetBackground(IGraphics *po, uint8 r, uint8 g, uint8 b);
static void             QGraphics_GetBackground(IGraphics *po, uint8 *r, uint8 *g, uint8 *b);
static RGBVAL           QGraphics_SetColor(IGraphics *po, uint8 r, uint8 g, uint8 b, uint8 alpha);
static void             QGraphics_GetColor(IGraphics *po, uint8 *r, uint8 *g, uint8 *b, uint8 *alpha);
static boolean          QGraphics_SetFillMode(IGraphics *po, boolean fFill);
static boolean          QGraphics_GetFillMode(IGraphics *po);
static RGBVAL           QGraphics_SetFillColor(IGraphics *po, uint8 r, uint8 g, uint8 b, uint8 alpha);
static void             QGraphics_GetFillColor(IGraphics *po, uint8 *pRed, uint8 *pGreen, uint8 *pBlue, uint8 *pAlpha);
static uint8            QGraphics_SetPointSize(IGraphics *po, uint8 u8Size);
static uint8            QGraphics_GetPointSize(IGraphics *po);
static boolean          QGraphics_SetClip(IGraphics *po, AEEClip *pShape, uint8 nFlag);
static boolean          QGraphics_GetClip(IGraphics *po, AEEClip *pShape);
static boolean          QGraphics_SetViewport(IGraphics *po, AEERect *pRect, uint8 nFlag);
static boolean          QGraphics_GetViewport(IGraphics *po, AEERect *pRect, boolean *framed);
static void             QGraphics_ClearViewport(IGraphics *po);
static AEEPaintMode     QGraphics_SetPaintMode(IGraphics *po, AEEPaintMode mode);
static AEEPaintMode     QGraphics_GetPaintMode(IGraphics *po);
static uint8            QGraphics_GetColorDepth(IGraphics *po);
static int              QGraphics_DrawPoint(IGraphics *po, AEEPoint *pPoint);
static int              QGraphics_DrawLine(IGraphics *po, AEELine *pLine);
static int              QGraphics_DrawRect(IGraphics *po, AEERect *pRect);
static int              QGraphics_DrawCircle(IGraphics *po, AEECircle *pCircle);
static int              QGraphics_DrawArc(IGraphics *po, AEEArc *pArc);
static int              QGraphics_DrawPie(IGraphics *po, AEEPie *pPie);
static int              QGraphics_DrawEllipse(IGraphics *po, AEEEllipse *pEllipse);                      // ellipse only
static int              QGraphics_DrawTriangle(IGraphics *po, AEETriangle *pTriangle);
static int              QGraphics_DrawPolygon(IGraphics *po, AEEPolygon *pPolygon);
static int              QGraphics_DrawPolyline(IGraphics *po, AEEPolyline *pPolyline);
//static void             QGraphics_ReleaseColorBuffer(IGraphics *po, AEEColorBuffer *buf);
static int              QGraphics_ClearRect(IGraphics *po, AEERect *pRect);
//static void             QGraphics_CopyArea(IGraphics *po, AEERect *pRect, int16 dx, int16 dy);
static boolean          QGraphics_EnableDoubleBuffer(IGraphics *po, boolean flag);
//static void             QGraphics_ReadColorBuffer(IGraphics *po, AEERect *pRect, AEEColorBuffer *buf);
static void             QGraphics_Update(IGraphics *po);
static void             QGraphics_Translate(IGraphics *po, int16 x, int16 y);
static void             QGraphics_Pan(IGraphics *po, int16 cx, int16 cy);
static void             QGraphics_StretchBlt(IGraphics* po, int16 destX, int16 destY,
                                             int16 destW, int16 destH, const void * pbmSource,
                                             AEERasterOp dwRopCode);
static void             QGraphics_SetAlgorithmHint(IGraphics *po, AEEAlgorithmHint hint);
static AEEAlgorithmHint QGraphics_GetAlgorithmHint(IGraphics *po);
static int              QGraphics_SetDestination(IGraphics *po, IBitmap *pDst);
static IBitmap*         QGraphics_GetDestination(IGraphics *po);

static AEEStrokeStyle   QGraphics_SetStrokeStyle(IGraphics *po, AEEStrokeStyle strokeStyle);
static AEEStrokeStyle   QGraphics_GetStrokeStyle(IGraphics *po);

static int              QGraphics_DrawEllipticalArc(IGraphics *po, AEERect *pRect, int16 startAngle, int16 arcAngle );
static int              QGraphics_DrawRoundRectangle(IGraphics *po, AEERect *pRect, int16 arcWidth, int16 arcHeight );




/*============================================================================
   Other static functions
============================================================================*/

static void resetViewport(QGraphics *pMe);
static boolean clipVScanline(QGraphics *pMe, int x, int16 yMinIn, int16 yMaxIn, int16 *yMinOut, int16 *yMaxOut);
static void insertEdgeAccurate(EdgeAccurate *list, EdgeAccurate *edge);
static boolean withinYBound(QGraphics *pMe, int y);
static void updateInterval(QGraphics *pMe, int y, int xMin, int xMax);
static boolean clipTest(int p, int q, FractionType *u1, FractionType *u2);
static void insertEdgeFast(EdgeFast * list, EdgeFast * edge);

static const VTBL(IGraphics) gGraphicsFuncs =  {QGraphics_AddRef,
                                                QGraphics_Release,
                                                QGraphics_SetBackground,
                                                QGraphics_GetBackground,
                                                QGraphics_SetColor,
                                                QGraphics_GetColor,
                                                QGraphics_SetFillMode,
                                                QGraphics_GetFillMode,
                                                QGraphics_SetFillColor,
                                                QGraphics_GetFillColor,
                                                QGraphics_SetPointSize,  
                                                QGraphics_GetPointSize,
                                                QGraphics_SetClip,
                                                QGraphics_GetClip,
                                                QGraphics_SetViewport,
                                                QGraphics_GetViewport,
                                                QGraphics_ClearViewport,
                                                QGraphics_SetPaintMode,
                                                QGraphics_GetPaintMode,
                                                QGraphics_GetColorDepth,
                                                QGraphics_DrawPoint,
                                                QGraphics_DrawLine,
                                                QGraphics_DrawRect,
                                                QGraphics_DrawCircle,
                                                QGraphics_DrawArc,
                                                QGraphics_DrawPie,
                                                QGraphics_DrawEllipse,
                                                QGraphics_DrawTriangle,
                                                QGraphics_DrawPolygon,
                                                QGraphics_DrawPolyline,
//                                                QGraphics_ReleaseColorBuffer,
                                                QGraphics_ClearRect,
//                                                QGraphics_CopyArea,
                                                QGraphics_EnableDoubleBuffer,
//                                                QGraphics_ReadColorBuffer,
                                                QGraphics_Update,
                                                QGraphics_Translate,
                                                QGraphics_Pan,
                                                QGraphics_StretchBlt,                                              
                                                QGraphics_SetAlgorithmHint,
                                                QGraphics_GetAlgorithmHint,
                                                QGraphics_SetDestination,
                                                QGraphics_GetDestination,
                                                QGraphics_SetStrokeStyle,
                                                QGraphics_GetStrokeStyle,
                                                QGraphics_DrawEllipticalArc,
                                                QGraphics_DrawRoundRectangle
};



//===========================================================================
//    Local functions
//===========================================================================
static boolean intervalIntersection(int16 xMin1, int16 xMax1, // interval 1
                                    int16 xMin2, int16 xMax2, // interval 2
                                    int16 *xMin, int16 *xMax);   // result





// It returns TRUE if the two intervals has a non-empty intersection and 
// stores the union in [xMin, xMax].  Otherwise it returns FALSE.
static boolean intervalUnion(int16 xMin1, int16 xMax1, // interval 1
                             int16 xMin2, int16 xMax2, // interval 2
                             int16 *xMin, int16 *xMax);   // result


// It returns TRUE if [xMin1, xMax1] - [xMin2, xMax2] is non-empty and stores
// the difference in [xMin, xMax].  Otherwise it returns FALSE.
// Note: The result of the DIFF might be two disjoint intervals, when the second
// interval is completely contained in the first one.
// For convenience in terms of interface, we always return two intervals as the
// result.  One might be empty if its min is greater than it max.
// When an empty interval may causes problem, it's recommended to check the
// returned value.
static boolean intervalDiff(int16 xMin1, int16 xMax1, // interval 1
                            int16 xMin2, int16 xMax2, // interval 2
                            int16 *int1xMin, int16 *int1xMax,     // result: interval 1
                            int16 *int2xMin, int16 *int2xMax);     // result: interal 2

static boolean valid(QGraphics *pMe, int x, int y);
//static void setRectClip(QGraphics *pMe, AEERect *pRect, uint8 nFlag);
static void resetClip(QGraphics *pMe);
static void initShapeClipInfo(QGraphics *pMe, AEERect *pRect, uint8 nFlag);
// clipping
static boolean clipLine(QGraphics *pMe, AEELine *original, AEELine *clipped);
static boolean clipHScanline(QGraphics *pMe, int16 y, int16 xMinIn, int16 xMaxIn, int16 *xMinOut, int16 *xMaxOut);
// Drawing
static void drawHScanline(QGraphics *pMe, int16 y, int16 xMin, int16 xMax, NativeColor color, AEERasterOp ro );
static void drawHScanline_Dotted(QGraphics * pMe, int16 y, int16 xMinIn, 
                                 int16 xMaxIn, NativeColor color, 
                                 AEERasterOp ro, boolean bDotted,
                                 int * piDotPhase);
static void drawLineVertical(QGraphics *pMe, int16 x, int16 startY, int16 endY,
                             NativeColor color, 
                             AEERasterOp ro,
                             boolean bNeedTest);

static void drawLinePositiveSlopeLessThanOne(QGraphics *pMe, int16 sStart, int16 yStart, int16 xEnd, int16 dx, int16 dy, boolean bClipped);
static void drawLinePositiveSlopeGreaterThanOne(QGraphics *pMe, int16 sStart, int16 yStart, int16 yEnd, int16 dx, int16 dy, boolean bClipped);
static void drawLineNegativeSlopeGreaterThanMinusOne(QGraphics *pMe, int16 sStart, int16 yStart, int16 xEnd, int16 dx, int16 dy, boolean bClipped);
static void drawLineNegativeSlopeLessThanMinusOne(QGraphics *pMe, int16 sStart, int16 yStart, int16 yEnd, int16 dx, int16 dy, boolean bClipped);
static void processAndDrawLine(QGraphics *pMe, const AEELine *pLine);
static void drawLine(QGraphics *pMe, AEELine *pLine, boolean bClipped);

static void drawDottedLineVertical(QGraphics *pMe, int16 x, int16 startY, int16 endY,
                                   NativeColor color, 
                                   AEERasterOp ro,
                                   boolean bNeedTest,
                                   int nDotPhase);
static void drawDottedLineHorizontal(QGraphics *pMe,
                                     int16 y, int16 startX, int16 endX, 
                                     NativeColor color,
                                     AEERasterOp ro,
                                     int nDotPhase);
static void drawDottedLinePositiveSlopeLessThanOne(QGraphics *pMe,
                                                   int16 sStart, int16 yStart, int16 xEnd,
                                                   int16 dx, int16 dy,
                                                   AEERasterOp ro,
                                                   boolean bClipped, int nDotPhase);
static void drawDottedLinePositiveSlopeGreaterThanOne(QGraphics *pMe,
                                                      int16 sStart, int16 yStart, int16 yEnd,
                                                      int16 dx, int16 dy,
                                                      AEERasterOp ro,
                                                      boolean bClipped, int nDotPhase);
static void drawDottedLineNegativeSlopeGreaterThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                           int16 yStart, int16 xEnd, 
                                                           int16 dx, int16 dy,
                                                           AEERasterOp ro,
                                                           boolean bClipped, int nDotPhase);
static void drawDottedLineNegativeSlopeLessThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                        int16 yStart, 
                                                        int16 yEnd, int16 dx, int16 dy, 
                                                        AEERasterOp ro,
                                                        boolean bClipped, int nDotPhase);
static int processAndDrawDottedLine(QGraphics *pMe, const AEELine *pLine, AEERasterOp ro, int nDotPhase);
static void drawDottedLine(QGraphics *pMe, AEELine *pLine, AEERasterOp ro, boolean bClipped, int nDotPhase);

// The parameter list of this rect intersection function might be very misleading.
// Unfortunately in this file, I always intersect a WinRect with an AEERect.  In order
// to save a few lines of code (such as translating one type of rect to another), I
// wrote this ugly function: first parameter is a WinRect and the other two are
// AEERect.  It's ugly, but it helps to keep the code a little short.
static boolean intersectRect(WinRect *first, AEERect *second, AEERect *intersection);


// Geometric primitive drawing functions
static void drawRect(QGraphics *pMe, AEERect *pRect, uint8 nFlag);
static void drawDottedRect(QGraphics *pMe, AEERect *pRect,
                           AEERasterOp ro);
static void frameRect(QGraphics *pMe, AEERect *pRect, boolean needTest, 
                      NativeColor,
                      AEERasterOp ro);
static void fillRect(QGraphics *pMe, AEERect *pRect, boolean needTest, 
                     NativeColor color,
                     AEERasterOp ro);

// Ellipse helper functions
static void reduceArc(int16 startAngle, int16 arcAngle, int16 *reducedStart, int16 *reducedArc);
static void mapAngleToLine( AEERect *pRect, int16 angle,
                            AEEEllipseLineInfo *pLine );
static void line_step( AEEEllipseLineInfo *pLine, AEERect *pRect );
static void drawEllipseLine( QGraphics *pMe, AEERect *pRect,
                             boolean drawFrame, boolean fillInterior,
                             boolean clip,
                             NativeColor frameColor,
                             NativeColor interiorColor,
                             AEERasterOp ro );
static void drawEllipseFillAndClip( QGraphics *pMe,
                                    int16 y1, int16 y2, int16 xm, int16 x,
                                    boolean fillInterior, boolean clip,
                                    NativeColor interiorColor,
                                    AEERasterOp ro );
static int drawEllipsePoints( QGraphics *pMe, boolean bDotted, int iDotPhase,
                               int16 x1, int16 y1,
                               int16 x2, int16 y2,
                               NativeColor color, AEERasterOp ro );
static int drawEllipseWithFrame( QGraphics *pMe, AEERect *pRect,
                                  boolean fillInterior, boolean clip,
                                  NativeColor frameColor,
                                  NativeColor interiorColor,
                                  AEERasterOp ro,
                                  int16 offset_x, int16 offset_y );
static int drawEllipseWithoutFrame( QGraphics *pMe, AEERect *pRect,
                                     boolean fillInterior, boolean clip,
                                     NativeColor interiorColor,
                                     AEERasterOp ro,
                                     int16 offset_x, int16 offset_y );
static void drawEllipticalArcLine( QGraphics *pMe, AEERect *pRect,
                                   boolean drawFrame, boolean fillInterior,
                                   boolean clip,
                                   NativeColor frameColor,
                                   NativeColor interiorColor,
                                   AEERasterOp ro,
                                   int16 startAngle, int16 endAngle );
static void drawEllipticalArcRow( QGraphics *pMe, int16 line,
                                   int16 left_xm, int16 right_xm,
                                   int16 left_x,  int16 right_x,
                                   boolean drawFrame, boolean fillInterior,
                                   NativeColor frameColor,
                                   NativeColor interiorColor,
                                   AEERasterOp ro, boolean bDotted, 
                                   int * piArcDotPhase, int * piPieDotPhase );
static void drawEllipticalArcDoubleRow( QGraphics *pMe, int16 line,
                                        int16 left_xm, int16 right_xm,
                                        int16 left_xmid, int16 right_xmid,
                                        int16 left_x,  int16 right_x,
                                        boolean drawFrame,
                                        boolean fillInterior,
                                        NativeColor frameColor,
                                        NativeColor interiorColor,
                                        AEERasterOp ro, boolean bDotted,
                                        int * piArcDotPhase, int * piPieDotPhase );
static int drawEllipticalArcCase1( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);
static int drawEllipticalArcCase2( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);
static int drawEllipticalArcCase3( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);
static int drawEllipticalArcCase4( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);
static int drawEllipticalArcCase5( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);
static int drawEllipticalArcCase6( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle);

static int drawEllipse(QGraphics *pMe, AEERect *pRect, uint8 nFlag);
static int drawEllipticalArc(QGraphics *pMe, AEERect *pRect, int16 startAngle, int16 arcAngle, uint8 nFlag);
static int drawRoundRectangle(QGraphics *pMe, AEERect *pRect, int16 arcWidth, int16 arcHeight, uint8 nFlag);

static int fillPolygonAccurate(QGraphics *pMe, AEEPolygon *pPolygon, int bClipped);
static int fillPolygonFast(QGraphics *pMe, AEEPolygon *pPolygon, int bClipped);
static void drawPolyline(QGraphics *pMe, int len, const AEEPoint *pts, boolean bClose);

static boolean isHorizontal(AEEPolygon *pPolygon);
static void mergeHorizontalSegments(AEEPolygon *pPolygon, AEELine *pLine);

// Get value from buffer
static uint32 GetValue(byte* inBuffer, int16 i, int16 j, int16 depth,
					 int16 inputLineBytes);
// Write a value to buffer
static void WriteOut(byte* outBuffer, int16 i,int16 j, uint32 value, int16 depth,
				  int16 outLineBytes);
// transformation to screen coordinates
static void transformLineToScreen(QGraphics *pMe, AEELine *out, AEELine *in);
static void transformRectToScreen(QGraphics *pMe, AEERect *out, AEERect *in);
static int transformPolylineToScreen(QGraphics *pMe, AEEPolyline *out, AEEPolyline *in);

// convert from Shape to Rect
static void convertCircleToRect(AEERect *pRect, AEECircle *pCircle);
static void convertEllipseToRect(AEERect *pRect, AEEEllipse *pEllipse);

// preprocessing
static void preprocessLine(AEELine *out, const AEELine *in);
static void preprocessDottedLine(AEELine *out, const AEELine *in,
                                 int nDotPhase, int *nStartPhase, int *nEndPhase);
//static void transformTriangleToScreen(QGraphics *pMe, AEETriangle *out, AEETriangle *in);
static void preprocessPolygon(AEEPolygon *pPolygon, int *yMin, int *yMax);



//RSD - previously no prototypes...
static void insertHorizontalEdge(HorizontalEdge *list, HorizontalEdge *edge);
static void deleteHorizontalEdges(HorizontalEdge *list);
static boolean intersectHorizontalEdge(HorizontalEdge *list, int16 scan, 
                                       int16 xMin, int16 xMax,
                                       int16 *intersectionMin, int16 *intersectionMax);

static int yPrev(int current, int len, AEEPoint *points);
static int yNext(int current, int len, AEEPoint *points);
static void makeEdgeRecAccurate(AEEPoint lower, AEEPoint upper, int yComp,
                                int yMin, EdgeAccurate *edge,  
                                EdgeAccurate *edges[]);
static void makeEdgeRecFast(AEEPoint lower, AEEPoint upper, int yComp, 
                            int yMin, EdgeFast * edge, EdgeFast * edges[]) 
                           ;
static int buildEdgeListAccurate(int len, AEEPoint points[], int yMin,
                                 EdgeAccurate *edges[], HorizontalEdge *horizontalEdgeList, 
                                 EdgeAccurate *pEdges);
static void buildEdgeListFast(int cnt, AEEPoint *pts, EdgeFast *edges[], int yMin, EdgeFast *pEdges);
static void buildActiveListAccurate(int scan, int yMin, 
                            EdgeAccurate *active, EdgeAccurate *edges[]);
static void buildActiveListFast(int scan, int yMin, EdgeFast * active, EdgeFast * edges[]);
static boolean intervalIntersection(int16 xMin1, int16 xMax1,
                                    int16 xMin2, int16 xMax2,
                                    int16 *xMin, int16 *xMax);
static boolean intervalUnion(int16 xMin1, int16 xMax1,
                             int16 xMin2, int16 xMax2,
                             int16 *xMin, int16 *xMax);
static boolean intervalDiff(int16 xMin1, int16 xMax1,
                            int16 xMin2, int16 xMax2,
                            int16 *int1xMin, int16 *int1xMax,
                            int16 *int2xMin, int16 *int2xMax);
static void fillScanAccurate(QGraphics *pMe, int16 scan, EdgeAccurate *active, HorizontalEdge *horizontalEdges, int bClipped);
static void fillScanFast(QGraphics *pMe, int16 scan, EdgeFast * active, int bClipped);
static void updateHorizontalEdgeList(HorizontalEdge *list, int scan);
static void updateXIntersections(int scan, EdgeAccurate *active);
static void updateActiveListAccurate(int scan, EdgeAccurate *active);
static void updateActiveListFast(int scan, EdgeFast *active);
static void resortActiveListAccurate(EdgeAccurate *active);
static void resortActiveListFast(EdgeFast *active);

// local functions called by sutherland hodgeman polygon clipping
static int inside (AEEPoint p, int b, AEEPoint wMin, AEEPoint wMax);
static int cross (AEEPoint p1, AEEPoint p2, int b, AEEPoint wMin, AEEPoint wMax);
static AEEPoint intersect (AEEPoint p1, AEEPoint p2, int b, AEEPoint wMin, AEEPoint wMax);
static void clipPoint(AEEPoint p, int bMarked[], int b, 
                      AEEPoint wMin, AEEPoint wMax,
                      AEEPoint *pOut, int *cnt, AEEPoint first[], AEEPoint *s);
static void closeClip(AEEPoint wMin, AEEPoint wMax, AEEPoint *pOut,
                      int *cnt, AEEPoint first[], int bMarked[], AEEPoint *s);
static int clipPolygon(AEEPoint wMin, AEEPoint wMax, int n, 
                       const AEEPoint *pIn, AEEPoint *pOut);


//===========================================================================
// MACRO DEFINITIONS
//===========================================================================
#ifndef MIN
   #define MIN(x, y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
   #define MAX(x, y)       (((x) > (y)) ? (x) : (y))
#endif

#ifndef SQRUARE
   #define SQUARE(x)       ((x) * (x))
#endif



// Determines whether 2 integers have the same sign
// Assuming 2's compliment in both Windows and the phone.
// For one's compliment number system, 0 could cause some problems.
// This is more efficient than  x * y > 0
// This macro is "positively" biased for zero.  Namely zero is 
// considered positive.
#define SIGNEQ(x, y) (((x) ^ (y)) >= 0)

// Macros to support dotted drawing (see the comments near the global constants
// definitions for dotted drawing).
#define DOT_PHASE_ON(phase)         ((phase) < DOT_ON_COUNT)
#define DOT_PHASE_INC(phase)        (((phase) >= (DOT_REPEAT_COUNT - 1)) ? 0 : (phase) + 1)
#define DOT_PHASE_ADD(phase, value) (((phase) + (value)) % DOT_REPEAT_COUNT)
#define DOT_PHASE_SUB(phase, value) \
        DOT_PHASE_ADD(phase, (DOT_REPEAT_COUNT - (value) % DOT_REPEAT_COUNT))

static void resetViewport(QGraphics *pMe)
{
   // By default, the viewport is just the screen.
   pMe->m_framed = FALSE;
   pMe->m_viewport.xMin = pMe->m_viewport.yMin = 0;
   pMe->m_viewport.xMax = pMe->m_cx - 1;
   pMe->m_viewport.yMax = pMe->m_cy - 1;

   pMe->m_viewportOrigin.x = pMe->m_viewportOrigin.y = 0;

   pMe->m_clip.type = CLIPPING_RECT;  // default clipping is a rectangle
   pMe->m_clip.shape.rect.x = pMe->m_clip.shape.rect.y = 0;
   pMe->m_clip.shape.rect.dx = pMe->m_cx;
   pMe->m_clip.shape.rect.dy = pMe->m_cy;
   pMe->m_clipInfo.rect = pMe->m_viewport; // identical to the window

}

/*===========================================================================

EXTERNAL - Called from OEM static table.  Supports creation of Graphics classes.

===========================================================================*/
int AEEGraphics_New(IShell *pIShell, AEECLSID clsid, void **ppobj)
{
   QGraphics *pMe;
   int       nErr;

   /* mallocs, inits the vtable, addrefs me */
   pMe = (QGraphics *)AEE_NewClass((IBaseVtbl *)&gGraphicsFuncs, 
                                   sizeof(QGraphics));
   
   if ((QGraphics *)0 != pMe) {
      
      nErr = ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY, 
                                   (void **)&(pMe->m_pIDisp));

      if (SUCCESS == nErr) {

         // Initialize handles to other AEE services
         pMe->m_pIShell = pIShell;
         ISHELL_AddRef(pIShell);

         // initializes the private data
         pMe->m_bgRGB = RGB_WHITE;
         pMe->m_fgRGB = RGB_BLACK;
         pMe->m_flRGB = RGB_BLACK;

         pMe->m_fill = FALSE;   // default to non-fill
         pMe->m_pointSize = 1;  // default to 1 pixel
         pMe->m_ro = AEE_RO_COPY;   // default to over-writing
         pMe->m_hint = ALGORITHM_FAST; // default to the simple and fast implementation
         pMe->m_strokeStyle = STROKE_SOLID;

         nErr = QGraphics_SetDestination((IGraphics *)pMe,0);
      }
   } else {
      nErr = ENOMEMORY;
   }

   if (SUCCESS != nErr) {
      if ((QGraphics *)0 != pMe) {
         QGraphics_Release((IGraphics *)pMe);
         pMe = 0;
      }
   }

   *ppobj = pMe;
   
   return nErr;
}



/*===========================================================================

Interface Method - Increments reference Count

===========================================================================*/
static uint32 QGraphics_AddRef(IGraphics *pIGraphics)
{
   return (++((QGraphics *)pIGraphics)->m_nRefs);
}



/*===========================================================================

Interface Method - Decrements reference Count. When count goes to zero, does cleanup

===========================================================================*/
static uint32 QGraphics_Release(IGraphics *pIGraphics)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;

   if (--pMe->m_nRefs != 0)
      return (pMe->m_nRefs);

   if (pMe->m_pbmDst) {
      IBITMAP_Release(pMe->m_pbmDst);
   }
   if (pMe->m_pIDisp) {
      IDISPLAY_Release(pMe->m_pIDisp);
   }

   if (pMe->m_pIShell) {
      ISHELL_Release(pMe->m_pIShell);
   }

   FREE(pMe->m_clipInfo.intervals);
   FREE(pMe);
   return (0);
}

/*===========================================================================

Interface Method - Implements IGRAPHICS_StretchBlt

===========================================================================*/

static void QGraphics_StretchBlt(IGraphics* po, int16 destX, int16 destY,
                                 int16 destW, int16 destH, const void * pbmSource,
                                 AEERasterOp dwRopCode)
						
{
   QGraphics *pMe = (QGraphics *)po;
   uint32 inValue,dataOffset, outSize;
   int16 SIZEOF_BITMAPFILEHEADER, 
      depth,inputW, inputH, outW, outH,inputLineBytes,
      outLineBytes, ratioX, i,j,k,i2,j2;
   byte *inBuffer, *outBuffer, *outBmp;
   AEE_WinBMPFileHdr* pbmfh, *pbmfh2; 
   AEE_WinBMPInfoHdr* pbmi, *pbmi2; 
   AEEImageInfo imageInfo;
   boolean bVal;
   void* native;

   if( pbmSource == NULL ) {
      return;
   }

   if(dwRopCode >= AEE_RO_TOTAL || dwRopCode < 0) {
      dwRopCode = AEE_RO_COPY;
   }

   SIZEOF_BITMAPFILEHEADER = 14;

   //input headers
   pbmfh = (AEE_WinBMPFileHdr*) ((byte*)pbmSource +2);
   dataOffset = LETOHL(pbmfh->dwOffset);
   pbmi = (AEE_WinBMPInfoHdr*)((byte*) pbmSource + SIZEOF_BITMAPFILEHEADER);

   // We don't handle compressed bmp
   if(LETOHL(pbmi->dwBICompress) != 0)
      return;

   // common parameters
   depth = LETOHS(pbmi->wBitsPerPix);

   // inputs
   inputW = (int16)(LETOHL(pbmi->dwBIWidth));
   inputH = (int16)(LETOHL(pbmi->dwBIHeight));
   inputLineBytes = ((inputW * depth + 31)/32) *4;
   dataOffset = LETOHL(pbmfh->dwOffset);
   inBuffer = ((byte*) pbmSource) + dataOffset;

   // output
   outW = destW;
   outH = destH;
   outLineBytes = ((outW * depth + 31)/32) *4;

   // create output  bmp
   outSize = dataOffset + outLineBytes * outH;
   outBmp = (byte*) MALLOC(outSize);
   if(! outBmp) return;

   MEMSET(outBmp,0,outSize);
   MEMCPY(outBmp, pbmSource, dataOffset);
   outBuffer = (byte*) outBmp + dataOffset;
	
   // modify output header
   pbmfh2 = (AEE_WinBMPFileHdr*) (outBmp +2);      
   pbmi2 = (AEE_WinBMPInfoHdr*)((byte*) outBmp + SIZEOF_BITMAPFILEHEADER);
	
   pbmfh2->dwFileSize = outSize;
   pbmi2->dwBIWidth = outW;
   pbmi2->dwBIHeight = outH; 
   pbmi2->dwBISize = outLineBytes * outH;
   HTOLEL_D(pbmfh2->dwFileSize);
   HTOLEL_D(pbmi2->dwBIWidth);
   HTOLEL_D(pbmi2->dwBIHeight);
   HTOLEL_D(pbmi2->dwBISize);

   // Map input line: (i2,j2) -> (i,j)
   ratioX = ceiling(outW,inputW);
   for(j2 =0; j2< outH-1; j2++) {
	
      j = j2 *(inputH-1) /(outH-1);
		
      for( i2 =0; i2<outW-1; i2++) { // 1 less than the pixels per row			
         i = i2*(inputW-1)/(outW-1);
         inValue = GetValue(inBuffer, i,j, depth, inputLineBytes);
         // Flush right-ward input values
				
         for(k =0; k<ratioX && i2+k <= outW-1; k++) {
            WriteOut(outBuffer, (int16)(i2+k),j2, inValue, depth, outLineBytes);
         }


      }
		
      // Last pixel in a row
      i2 = outW -1;
	  i = outW != 1 ? i2*(inputW-1)/(outW-1) : 0;
      inValue = GetValue(inBuffer, i,j, depth,inputLineBytes);
      WriteOut(outBuffer, i2,j2, inValue, depth, outLineBytes);
   }

   // last row
   j2 = outH -1;
   j = inputH-1;
   for( i2=0; i2<outW-1; i2++) { // 1 less than the pixels per row			
      i = i2*(inputW-1)/(outW-1);
      inValue = GetValue(inBuffer, i,j, depth, inputLineBytes);
      // Flush right-ward input values
				
      for(k =0; k<ratioX && i2+k <= outW-1; k++) {
         WriteOut(outBuffer, (int16)(i2+k),j2, inValue, depth, outLineBytes);
      }


   }
	
   // Last pixel in a row
   i2 = outW - 1;
   i = outW != 1 ? i2*(inputW-1)/(outW-1) : 0;
   inValue = GetValue(inBuffer, i,j, depth,inputLineBytes);
   WriteOut(outBuffer, i2,j2, inValue, depth, outLineBytes);

   native = CONVERTBMP (outBmp, &imageInfo, &bVal);

   IDISPLAY_BitBlt(pMe->m_pIDisp, destX, destY, destW, destH, native,
                   0, 0, dwRopCode);

   FREE(outBmp);
   if (bVal) {//free only if realloc was done
      SYSFREE ( native);
   }

}

/*===========================================================================

Interface Method - Implements IGRAPHICS_SetBackground

===========================================================================*/
static RGBVAL QGraphics_SetBackground(IGraphics *pIGraphics, uint8 r, uint8 g, uint8 b)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;
   pMe->m_bgRGB = MAKE_RGB(r, g, b);
   pMe->m_bg = IBITMAP_RGBToNative(pMe->m_pbmDst, pMe->m_bgRGB);
   return pMe->m_bgRGB;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetBackground

===========================================================================*/

static void QGraphics_GetBackground(IGraphics *po, uint8 *r, uint8 *g, uint8 *b)
{
   QGraphics *pMe = (QGraphics *)po;
   RGBVAL color = pMe->m_bgRGB;

   *r = (uint8)(color >> 8);
   *g = (uint8)(color >> 16);
   *b = (uint8)(color >> 24);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_SetColor

===========================================================================*/

static RGBVAL QGraphics_SetColor(IGraphics *pIGraphics, uint8 r, uint8 g, uint8 b, uint8 alpha)
{
   // WARNING: Alpha value is ignored for now.  It will be supported in later versions.
   QGraphics *pMe = (QGraphics *) pIGraphics;
   pMe->m_fgRGB = MAKE_RGB(r, g, b);
   pMe->m_fg = IBITMAP_RGBToNative(pMe->m_pbmDst, pMe->m_fgRGB);
   return pMe->m_fgRGB;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetColor

===========================================================================*/
static void QGraphics_GetColor(IGraphics *po, uint8 *r, uint8 *g, uint8 *b, uint8 *alpha)
{
   QGraphics *pMe = (QGraphics *)po;
   RGBVAL color = pMe->m_fgRGB;

   *alpha = (uint8) color;
   *r = (uint8) (color >> 8);
   *g = (uint8) (color >> 16);
   *b = (uint8) (color >> 24);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_SetFillMode

===========================================================================*/
static boolean QGraphics_SetFillMode(IGraphics *pIGraphics, boolean fFill)
{
   ((QGraphics *)pIGraphics)->m_fill = FALSE;   // default to FALSE

   if (fFill == TRUE)
      ((QGraphics *)pIGraphics)->m_fill = TRUE;

   return ((QGraphics *)pIGraphics)->m_fill;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetFillMode

===========================================================================*/
static boolean QGraphics_GetFillMode(IGraphics *pIGraphics)
{
   return (((QGraphics *)pIGraphics)->m_fill);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_SetFillColor

===========================================================================*/
static RGBVAL QGraphics_SetFillColor(IGraphics *pIGraphics, uint8 r, uint8 g, uint8 b, uint8 alpha)
{
   // WARNING: Alpha value is ignored for now.  It will be suppored in the next version.
   QGraphics *pMe = (QGraphics *)pIGraphics;
   pMe->m_flRGB = MAKE_RGB(r, g, b);
   pMe->m_fl = IBITMAP_RGBToNative(pMe->m_pbmDst, pMe->m_flRGB);
   return pMe->m_flRGB;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetFillColor

===========================================================================*/
static void QGraphics_GetFillColor(IGraphics *po, uint8 *pRed, uint8 *pGreen, uint8 *pBlue, uint8 *pAlpha)
{
   QGraphics *pMe = (QGraphics *)po;
   RGBVAL color = pMe->m_flRGB;

   *pAlpha = (uint8) color;
   *pRed = (uint8) (color >> 8);
   *pGreen = (uint8) (color >> 16);
   *pBlue = (uint8) (color >> 24);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_SetPointSize

===========================================================================*/

static uint8 QGraphics_SetPointSize(IGraphics *pIGraphics, uint8 u8Size)
{
   return (((QGraphics *)pIGraphics)->m_pointSize = u8Size);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetPointSize

===========================================================================*/
static uint8 QGraphics_GetPointSize(IGraphics *pIGraphics)
{
   return (((QGraphics *)pIGraphics)->m_pointSize);
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_SetClip

===========================================================================*/
static boolean QGraphics_SetClip(IGraphics *pIGraphics, AEEClip *pShape, uint8 nFlag)
{
   int exception = SUCCESS;
   QGraphics *pMe = (QGraphics *)pIGraphics;
   
   // NULL for resetting the clipping region to the entire window
   if (!pShape) {
      resetClip(pMe);

      pMe->m_clip.shape.rect.x  = pMe->m_clipInfo.rect.xMin;
      pMe->m_clip.shape.rect.y  = pMe->m_clipInfo.rect.yMin;
      pMe->m_clip.shape.rect.dx = pMe->m_clipInfo.rect.xMax - pMe->m_clipInfo.rect.xMin + 1;
      pMe->m_clip.shape.rect.dy = pMe->m_clipInfo.rect.yMax - pMe->m_clipInfo.rect.yMin + 1;
   }
   else {
      AEERect calcRect;
      AEERect screenRect;
      
      if ((pShape->type == CLIPPING_RECT) || 
          (pShape->type == CLIPPING_CIRCLE) ||
          (pShape->type == CLIPPING_ELLIPSE)) {

         MEMCPY(&(pMe->m_clip), pShape, sizeof(AEEClip));   // Keep a record
         
         // Resetting the clipping region to the window first so that
         // we can construct the new clipping region.
         resetClip(pMe);
      }
      else {
         THROW(exception, EFAILED);
      }
      
      switch (pShape->type) {
      case CLIPPING_RECT:
         transformRectToScreen(pMe, &screenRect, &(pShape->shape.rect));
         drawRect(pMe, &screenRect, (uint8) (nFlag | AEE_GRAPHICS_CLIP));
         pMe->m_clip.type = CLIPPING_RECT;
         break;
         
      case CLIPPING_CIRCLE:
         if (pShape->shape.circle.r < 0) {
            THROW(exception, EBADPARM);
         }

         convertCircleToRect(&calcRect, &(pShape->shape.circle));
         transformRectToScreen(pMe, &screenRect, &calcRect);
         initShapeClipInfo(pMe, &screenRect, nFlag);
         TRY(exception, drawEllipse(pMe, &screenRect, (uint8) (nFlag | AEE_GRAPHICS_CLIP)));
         pMe->m_clip.type = CLIPPING_CIRCLE;
         
         break;
         
      case CLIPPING_ELLIPSE:
         if (pShape->shape.ellipse.wx < 0 || pShape->shape.ellipse.wy < 0) {
            THROW(exception, EBADPARM);
         }
         
         convertEllipseToRect(&calcRect, &(pShape->shape.ellipse));
         transformRectToScreen(pMe, &screenRect, &calcRect);
         initShapeClipInfo(pMe, &screenRect, nFlag);
         TRY(exception, drawEllipse(pMe, &screenRect, (uint8) (nFlag | AEE_GRAPHICS_CLIP)));
         pMe->m_clip.type = CLIPPING_ELLIPSE;
         
         break;
         
         // Default to the window
      default:
         // This line should never be executed.
         THROW(exception, EFAILED);
      }
   }
   CATCH(exception) {
      return FALSE;
   }
   return TRUE;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetClip

===========================================================================*/
static boolean QGraphics_GetClip(IGraphics *pIGraphics, AEEClip *pShape)
{
   if (!pShape) {
      return FALSE;
   }

   MEMCPY(pShape, &(((QGraphics *)pIGraphics)->m_clip), sizeof(AEEClip));
   return TRUE;
}


/*===========================================================================

Interface Method - Implements IGRAPHICS_SetViewport

===========================================================================*/
static boolean QGraphics_SetViewport(IGraphics *pIGraphics, AEERect *prc, uint8 nFlag)
{
   QGraphics *pMe = (QGraphics *) pIGraphics;
   
   if ((AEERect *)0 != prc) {
      WinRect window;

      if (
          /* if the rectangle's smaller than 3x3 */
          (((prc->dx < 3) || (prc->dy < 3)) && 
           /* AND */
           /* we need a frame, fail */
           (nFlag & AEE_GRAPHICS_FRAME)) ||

          
          /* OR */
          /* if the rectangle's smaller than 1x1, fail */
          (prc->dx < 1) || (prc->dy < 1)) {
         return FALSE;
      }

      window.xMax = prc->x + prc->dx - 1;
      window.yMax = prc->y + prc->dy - 1;
      window.xMin = prc->x;
      window.yMin = prc->y;
      
      // The new window has to be completely contained in the screen.
      if ((window.xMin < 0) || 
          (window.yMin < 0) || 
          (window.xMax >= pMe->m_cx) || (window.yMax >= pMe->m_cy)) {
         return FALSE;
      }
      
      pMe->m_viewport = window;

      pMe->m_framed = (nFlag & AEE_GRAPHICS_FRAME) ? TRUE : FALSE;

      if (pMe->m_framed) {
         
         frameRect(pMe, prc, FALSE, pMe->m_fg, AEE_RO_COPY);
         
         window.xMin++;
         window.yMin++;
         window.xMax--;
         window.yMax--;
      }

      // Set the origin (0, 0) to the upper left corner of the the viewport.
      pMe->m_viewportOrigin.x = window.xMin;
      pMe->m_viewportOrigin.y = window.yMin;
      
      pMe->m_translate.x = pMe->m_viewportOrigin.x - pMe->m_windowOrigin.x;
      pMe->m_translate.y = pMe->m_viewportOrigin.y - pMe->m_windowOrigin.y;
      
      // Keep a copy of the new clipping rectangle
      pMe->m_clip.shape.rect.x = window.xMin;
      pMe->m_clip.shape.rect.y = window.yMin;
      pMe->m_clip.shape.rect.dx = window.xMax - window.xMin + 1;
      pMe->m_clip.shape.rect.dy = window.yMax - window.yMin + 1;
      
      // Set the default clipping region, which is same as the window.
      // This won't change until SetClip is called.
      pMe->m_clip.type = CLIPPING_RECT;
      pMe->m_clipInfo.rect = window;
      
      // clear the window
      if (nFlag & AEE_GRAPHICS_CLEAR) {
         fillRect(pMe, &pMe->m_clip.shape.rect, FALSE, pMe->m_bg, AEE_RO_COPY);
      }
   } else { /* prc is NULL */
      // Reset the window to the screen with no frame
      resetViewport(pMe);
      pMe->m_translate.x = -pMe->m_windowOrigin.x;
      pMe->m_translate.y = -pMe->m_windowOrigin.y;
   }
   
   return TRUE;
}



/*===========================================================================

Interface Method - Implements IGRAPHICS_GetViewport

===========================================================================*/
static boolean QGraphics_GetViewport(IGraphics *pIGraphics, AEERect *pRect, boolean *framed)
{
   if (!pRect) {
      return FALSE;
   } else {
      QGraphics *pMe = (QGraphics *)pIGraphics;
      pRect->x = pMe->m_viewport.xMin;
      pRect->y = pMe->m_viewport.yMin;
      pRect->dx = pMe->m_viewport.xMax - pMe->m_viewport.xMin + 1;
      pRect->dy = pMe->m_viewport.yMax - pMe->m_viewport.yMin + 1;

      *framed = pMe->m_framed;

      return TRUE;
   }
}


/*===========================================================================

Interface Method - Implements IGRAPHICS_ClearViewport

===========================================================================*/
static void QGraphics_ClearViewport(IGraphics *pIGraphics)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;
   AEERect rect;

   rect.x = pMe->m_viewport.xMin;
   rect.y = pMe->m_viewport.yMin;
   rect.dx = pMe->m_viewport.xMax - pMe->m_viewport.xMin + 1;
   rect.dy = pMe->m_viewport.yMax - pMe->m_viewport.yMin + 1;

   if (pMe->m_framed) {
      rect.x++;
      rect.y++;
      rect.dx -=2;
      rect.dy -=2;
   }

   fillRect(pMe, &rect, FALSE, pMe->m_bg, AEE_RO_COPY);
}



/*===========================================================================

Interface Method - Implements IGraphics_SetPaintMode

===========================================================================*/
static AEEPaintMode QGraphics_SetPaintMode(IGraphics *pIGraphics, AEEPaintMode mode)
{
   QGraphics *pMe = (QGraphics *) pIGraphics;

   // Since we are not using AEE_RO_XXXXX, AEEPaintMode enum is
   // redundant.  However, following code has been kept to ensure
   // backward compatibility with BREW 1.x
   if (mode == AEE_PAINT_XOR) {
      pMe->m_ro = AEE_RO_XOR;
      return AEE_PAINT_XOR;
   }
   else {    // default mode
      pMe->m_ro = AEE_RO_COPY;
	  return AEE_PAINT_COPY;
   }
}



/*===========================================================================

Interface Method - Implements IGraphics_GetPaintMode

===========================================================================*/
static AEEPaintMode QGraphics_GetPaintMode(IGraphics *pIGraphics)
{
   QGraphics *pMe = (QGraphics *) pIGraphics;

   if (pMe->m_ro == AEE_RO_XOR)
      return AEE_PAINT_XOR;
   else
      return AEE_PAINT_COPY;
}


/*===========================================================================

Interface Method - Implements IGraphics_SetAlgorithmHint

===========================================================================*/
static void QGraphics_SetAlgorithmHint(IGraphics *pIGraphics, AEEAlgorithmHint hint)
{
   QGraphics *pMe = (QGraphics *) pIGraphics;

   if (hint == ALGORITHM_ACCURATE)
      pMe->m_hint = ALGORITHM_ACCURATE;
   else
      pMe->m_hint = ALGORITHM_FAST; // default
}


/*===========================================================================

Interface Method - Implements IGraphics_GetAlgorithmHint

===========================================================================*/
static AEEAlgorithmHint QGraphics_GetAlgorithmHint(IGraphics *pIGraphics)
{
   QGraphics *pMe = (QGraphics *) pIGraphics;
   return pMe->m_hint;
}




/*===========================================================================

Interface Method - Implements IGraphics_GetColorDepth

===========================================================================*/

static uint8 QGraphics_GetColorDepth(IGraphics *pIGraphics)
{
   return (((QGraphics *)pIGraphics)->m_clrDepth);
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawPoint

===========================================================================*/
static int QGraphics_DrawPoint(IGraphics *pGraphics, AEEPoint *pPoint)
{
   QGraphics *pMe = (QGraphics *)pGraphics;

   if (!pPoint)
      return EBADPARM; 
   else {
      int16 x0 = pPoint->x + pMe->m_translate.x;  // screen coordinate
      int16 y0 = pPoint->y + pMe->m_translate.y;  // screen coordinate
      int16 x = x0, y = y0;
      int16 i, j;

      for (i = 0; i < pMe->m_pointSize; i++) {
         for (j = 0; j < pMe->m_pointSize; j++) {
            if (valid(pMe, x, y))
               IBITMAP_DrawPixel(pMe->m_pbmDst, x, y, pMe->m_fg, pMe->m_ro);
            y++;
         }
         y = y0;
         x++;
      }
   }

   return SUCCESS;
}


/*===========================================================================

Interface Method - Implements IGraphics_DrawLine.

COMMENTS:   The input pLine is in window coordinates.  It should be transformed
to screen coordinates before clipping and drawing. This "philosophy" applies to 
every "draw" function.

WARNING: Point size is assumed to be 1. Will have to modify the code to support 
point size other than 1.

===========================================================================*/
static int QGraphics_DrawLine(IGraphics *pGraphics, AEELine *pLine)
{
   if (!pLine)
      return EBADPARM;
   else {
      QGraphics *pMe = (QGraphics *)pGraphics;
      AEELine screenLine;

      // Transform the line in window coordinates to screen coordinates
      transformLineToScreen(pMe, &screenLine, pLine);
      if( pMe->m_strokeStyle == STROKE_SOLID )
         processAndDrawLine(pMe, &screenLine);
      else
         processAndDrawDottedLine(pMe, &screenLine, pMe->m_ro, 0);
   }

   return SUCCESS;
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawRect

===========================================================================*/
static int QGraphics_DrawRect(IGraphics *pIGraphics, AEERect *pRect)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;
   AEERect screenRect;
      
   if (!pRect || pRect->dx < 0 || pRect->dy < 0 )
      return EBADPARM;

   if( pRect->dx != 0 && pRect->dy != 0 ) {
      // Transform the rectangle from window coordinates to screen coordinates
      transformRectToScreen(pMe, &screenRect, pRect);

      if (pMe->m_fill) {
         if (pMe->m_fg != pMe->m_fl || pMe->m_strokeStyle == STROKE_DOTTED)
            drawRect(pMe, &screenRect, AEE_GRAPHICS_FILL | AEE_GRAPHICS_FRAME);
         else
            drawRect(pMe, &screenRect, AEE_GRAPHICS_FILL);
      }
      else
         drawRect(pMe, &screenRect, AEE_GRAPHICS_FRAME);
   }

   return SUCCESS;
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawCircle

===========================================================================*/
static int QGraphics_DrawCircle(IGraphics *po, AEECircle *pCircle)
{
   AEERect calcRect;

   if (!pCircle)
      return EBADPARM;
   if (pCircle->r == 0)
      return SUCCESS;   // empty circle

   // Transform the circle to the screen coordinates
   convertCircleToRect(&calcRect, pCircle);
   return QGraphics_DrawEllipticalArc( po, &calcRect, 0, 360 );
}

/*===========================================================================

Interface Method - Implements IGraphics_DrawArc

===========================================================================*/
static int QGraphics_DrawArc(IGraphics *po, AEEArc *pArc)
{
   AEERect calcRect;

   if (!pArc)
      return EBADPARM;
   if(pArc->r < 0)
      return EBADPARM;
   if(pArc->r == 0 || pArc->arcAngle == 0)
      return SUCCESS;

   // Transform the circle to the screen coordinates
   convertCircleToRect(&calcRect, (AEECircle *) pArc);
   return QGraphics_DrawEllipticalArc( po, &calcRect,
                                       pArc->startAngle, pArc->arcAngle );
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawPie

===========================================================================*/
static int QGraphics_DrawPie(IGraphics *po, AEEPie *pPie)
{
   int exception = SUCCESS;
   QGraphics *pMe = (QGraphics *) po;
   AEERect calcRect;
   AEERect screenRect;
   int16   startAngle;
   int16   arcAngle;

   if (!pPie){
      THROW(exception, EBADPARM);
   }
   if(pPie->r < 0) {
      THROW(exception, EBADPARM);
   }
   if(pPie->r == 0 || pPie->arcAngle == 0) {
      THROW(exception, SUCCESS);
   }

   // Transform the circle to the screen coordinates
   convertCircleToRect(&calcRect, (AEECircle *) pPie);
   transformRectToScreen(pMe, &screenRect, &calcRect);

   reduceArc(pPie->startAngle, pPie->arcAngle, &startAngle, &arcAngle);

   if( arcAngle == 360 ) {
      TRY(exception, QGraphics_DrawEllipticalArc(po, &screenRect, startAngle, arcAngle));
   }
   else {
      if (pMe->m_fill) {
         if (pMe->m_fg != pMe->m_fl || pMe->m_strokeStyle == STROKE_DOTTED) {
            TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                              AEE_GRAPHICS_FILL | AEE_GRAPHICS_FRAME));
         }
         else {
            TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                              AEE_GRAPHICS_FILL));
         }
      }
      else {
         TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                           AEE_GRAPHICS_FRAME ));
      }
   }
   CATCH(exception);
   return exception;
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawEllipse

===========================================================================*/
static int QGraphics_DrawEllipse(IGraphics *po, AEEEllipse *pEllipse)
{
   AEERect calcRect;

   if (!pEllipse)
      return(EBADPARM);

   if (pEllipse->wx < 0 || pEllipse->wy < 0) // invalid ellipse
      return(EBADPARM);

   if (pEllipse->wx == 0 || pEllipse->wy == 0)
      return(SUCCESS);   // empty ellipse

   // Transform the ellipse to the screen coordinates
   convertEllipseToRect(&calcRect, pEllipse);
   return QGraphics_DrawEllipticalArc( po, &calcRect, 0, 360 );
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawTriangle

===========================================================================*/
static int QGraphics_DrawTriangle(IGraphics *pIGraphics, AEETriangle *pTriangle)
{
   AEEPoint aeepoint[3];

   if (!pTriangle)
      return EBADPARM;
   else {
      // Comments: The following code can be replaced by more efficient code.
      // In this version, we simply convert a triangle to a polygon and call
      // DrawPolygon.  
      AEEPolygon polygon;
      int ret;
      
      polygon.len = 3;
      polygon.points = aeepoint;

      if (!(polygon.points))
         return ENOMEMORY;

      MEMCPY(polygon.points, pTriangle, sizeof(AEETriangle));

      ret = QGraphics_DrawPolygon(pIGraphics, &polygon);

      return ret;
   }
}

/*===========================================================================

Interface Method - Implements IGraphics_DrawPolygon

===========================================================================*/
static int QGraphics_DrawPolygon(IGraphics *pIGraphics, AEEPolygon *pPolygon)
{
   int retVal = SUCCESS;
   QGraphics *pMe = (QGraphics *) pIGraphics;
   AEEPolygon screenPolygon;
   int bClipped = FALSE;
   AEEPolygon clippedPolygon;
   AEEPoint wMin, wMax;
   AEEPoint *buf, *pts;
   int i, tx, ty;


   // Allocate memory on Stack for polygon of length upto 4 vertices.  
   // If the polygon has m number of vertices, we need 3m units of memory: m for
   // transformation from the world coordinate system to the screen, and 2m for
   // clipping.  In this case the number of vertices is 4.  So we need  4*3 unit 
   // of memory.

   AEEPoint aeepoint[12];

   if (!pPolygon || !(pPolygon->points) || (pPolygon->len < 3))
      return EBADPARM;

   // Memory allocation is expensive.  It's more efficient for us to allocate
   // enough memory than allocating them as needed.  
   // If the polygon has m number of vertices, we need 3m units of memory: m for
   // transformation from the world coordinate system to the screen, and 2m for
   // clipping.
   screenPolygon.len = pPolygon->len;

   // Allocate memory on the Stack if the length of the Polygon is less than or equal to 4.

   if(screenPolygon.len <=4)
   {
       buf = aeepoint;
   }
   else
   {
     if ((buf = (AEEPoint *)MALLOC(sizeof(AEEPoint) * screenPolygon.len * 3)) == NULL)
        return ENOMEMORY;
   }

   screenPolygon.points = buf;
   
   // Use some local variable to avoid multiple deferencing.
   tx = pMe->m_translate.x, ty = pMe->m_translate.y;
   pts = pPolygon->points;

   // Translation to screen
   for (i = 0; i < screenPolygon.len; i++) {
      screenPolygon.points[i].x = pts[i].x + tx;
      screenPolygon.points[i].y = pts[i].y + ty;
   }

   // Before calling "fillPolygon", we have to handle degenerate
   // polygons.
   if (isHorizontal(&screenPolygon)) {
      AEELine hline;

      mergeHorizontalSegments(&screenPolygon, &hline);
      if( pMe->m_strokeStyle == STROKE_SOLID )
         drawHScanline(pMe, hline.sy, hline.sx, hline.ex, pMe->m_fg, pMe->m_ro);
      else
         processAndDrawDottedLine( pMe, &hline, pMe->m_ro, 0 );

      // Call FREE on the memory if it is allocated on Heap i.e. if the length of the 
      // polygon is greater than 4.

      if(screenPolygon.len > 4)
       FREE(screenPolygon.points);
      return SUCCESS;
   }

   // It's more efficient if we clip the polygon before passing it for drawing.
   // It usually reduces the number of vertices, which is the dominant factor
   // in the processing time.
   // If the clipping region is rectangle, it's even better because no "scan line"
   // clipping is needed.  The drawing function can just each scan line without further
   // clipping.

   if(screenPolygon.len > 4)
    clippedPolygon.points = buf + screenPolygon.len;
   else
    clippedPolygon.points = buf + 4;  // Since we are allocating memory for length 4 if
                                     // the length of the Polygon is less than or equal to 4.

   // Now the real work starts.

   wMin.x = pMe->m_clipInfo.rect.xMin;
   wMin.y = pMe->m_clipInfo.rect.yMin;
   wMax.x = pMe->m_clipInfo.rect.xMax;
   wMax.y = pMe->m_clipInfo.rect.yMax;

   clippedPolygon.len = clipPolygon(wMin, wMax, screenPolygon.len, 
                                    screenPolygon.points, clippedPolygon.points);

   if (!clippedPolygon.len) { // empty polygon


     // Call FREE on the memory if it is allocated on Heap i.e. if the length of the 
     // polygon is greater than 4.

      if(screenPolygon.len > 4)
        FREE(buf);
      return SUCCESS;
   }

   // If the clipping shape is rectangle, no futher scanline clipping is necesary.
   if (pMe->m_clip.type == CLIPPING_RECT) {
      bClipped = TRUE;
   }

   if (!(pMe->m_fill))  // draw frame only
      drawPolyline(pMe, screenPolygon.len, screenPolygon.points, TRUE);
   // We do have to fill it
   else if ((pMe->m_hint == ALGORITHM_FAST) || 
            (pMe->m_fl == pMe->m_fg) ||
            (pMe->m_ro == AEE_RO_COPY)) {
      retVal = fillPolygonFast(pMe, &clippedPolygon, bClipped);
      if ((retVal == SUCCESS) && (pMe->m_flRGB != pMe->m_fgRGB))
         drawPolyline(pMe, screenPolygon.len, screenPolygon.points, TRUE);
   }
   // Now we hit the worst case:
   // fillPolygonAccurate will be called only if all the following conditions are met
   // 1. ALGORITHM_FAST is not seleted;
   // 2. XOR is the raster op
   // 3. frame color is different from the fill color

   // If any of the above condition is violated, IGRAPHICS will automatically switch
   // to fillPolyFast even if ALGORITHM_ACCURATE is seleted.
   else 
      retVal = fillPolygonAccurate(pMe, &clippedPolygon, bClipped);


   // Call FREE on the memory if it is allocated on Heap i.e. if the length of the 
   // polygon is greater than 4.

   if(screenPolygon.len > 4)
     FREE(buf);
   return retVal;
}



/*===========================================================================

Interface Method - Implements IGraphics_DrawPolyline

===========================================================================*/
static int QGraphics_DrawPolyline(IGraphics *pIGraphics, AEEPolyline *pPolyline)
{
   if (!pPolyline || !(pPolyline->points) || (pPolyline->len < 2))
      return EBADPARM;
   else {
      int err = SUCCESS;
      QGraphics *pMe = (QGraphics *) pIGraphics;
      AEEPolyline screenPolyline;

      if (SUCCESS != (err = transformPolylineToScreen(pMe, &screenPolyline, pPolyline)))
         return err;

      drawPolyline(pMe, screenPolyline.len, screenPolyline.points, FALSE);
      FREE(screenPolyline.points);
      return SUCCESS;
   }
}



/*===========================================================================

Interface Method - Implements IGraphics_ClearRect

===========================================================================*/
static int QGraphics_ClearRect(IGraphics *pIGraphics, AEERect *pRect)
{
   if (!pRect || pRect->dx < 0 || pRect->dy < 0 )
      return EBADPARM;

   if( pRect->dx != 0 && pRect->dy != 0 )
      drawRect((QGraphics *)pIGraphics, pRect, AEE_GRAPHICS_CLEAR);

   return SUCCESS;
}


/*===========================================================================

Interface Method - Implements IGraphics_EnableDoubleBuffer

===========================================================================*/
static boolean QGraphics_EnableDoubleBuffer(IGraphics *pIGraphics, boolean flag)
{
   // The offscreen buffer is always there.
   return flag;
}

/*===========================================================================

Interface Method - Implements IGraphics_Update

===========================================================================*/
static void QGraphics_Update(IGraphics *pIGraphics)
{ 
   QGraphics *pMe = (QGraphics *)pIGraphics;

   IDISPLAY_Update(pMe->m_pIDisp);
}



/*===========================================================================

Interface Method - Implements IGraphics_Translate

===========================================================================*/
static void QGraphics_Translate(IGraphics *pIGraphics, int16 x, int16 y)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;

   // Translate the window's origin in the world coordinate system.
   // It's equivalent to translating the camera.

   pMe->m_windowOrigin.x += x;
   pMe->m_windowOrigin.y += y;

   pMe->m_translate.x = pMe->m_viewportOrigin.x - pMe->m_windowOrigin.x;
   pMe->m_translate.y = pMe->m_viewportOrigin.y - pMe->m_windowOrigin.y;
}



/*===========================================================================

Interface Method - Implements IGraphics_Pan

===========================================================================*/
static void QGraphics_Pan(IGraphics *pIGraphics, int16 cx, int16 cy)
{
   QGraphics *pMe = (QGraphics *)pIGraphics;

   pMe->m_windowOrigin.x = cx - (pMe->m_viewport.xMax - pMe->m_viewport.xMin + 1) / 2;
   pMe->m_windowOrigin.y = cy - (pMe->m_viewport.yMax - pMe->m_viewport.yMin + 1) / 2;

   pMe->m_translate.x = pMe->m_viewportOrigin.x - pMe->m_windowOrigin.x;
   pMe->m_translate.y = pMe->m_viewportOrigin.y - pMe->m_windowOrigin.y;
}



/*===========================================================================

Interface Method - Implements IGraphics_SetDestination

===========================================================================*/
static int QGraphics_SetDestination(IGraphics *po, IBitmap *pDst)
{
   QGraphics     *pMe = (QGraphics *)po;
   AEEBitmapInfo  bi;
   int            nErr;

   if ((IBitmap *)0 == pDst) {
      // Set to screen bitmap
      nErr = IDISPLAY_GetDeviceBitmap(pMe->m_pIDisp, &pDst);
      if (SUCCESS != nErr) {
         return nErr;
      }
   } else {
      IBITMAP_AddRef(pDst);
   }

   // get screen size, and color depth
   nErr = IBITMAP_GetInfo(pDst, &bi, sizeof(bi));
   if (SUCCESS != nErr) {
      IBITMAP_Release(pDst);
      return nErr;
   }

   // Install the new destination bitmap
   if (pMe->m_pbmDst) {
      IBITMAP_Release(pMe->m_pbmDst);
      pMe->m_pbmDst = 0;
   }
   pMe->m_pbmDst = pDst;

   pMe->m_cx         = (uint16)bi.cx;
   pMe->m_cy         = (uint16)bi.cy;
   pMe->m_clrDepth   = (uint8)bi.nDepth;

   // Reset the native colors, since the bitmap type may have changed.
   pMe->m_fg = IBITMAP_RGBToNative(pDst, pMe->m_fgRGB);
   pMe->m_bg = IBITMAP_RGBToNative(pDst, pMe->m_bgRGB);
   pMe->m_fl = IBITMAP_RGBToNative(pDst, pMe->m_flRGB);

   QGraphics_SetViewport((IGraphics *)pMe,0,0);
   QGraphics_SetClip((IGraphics *)pMe,0,0);
   QGraphics_Translate((IGraphics *)pMe,
                       (int16)-pMe->m_windowOrigin.x,
                       (int16)-pMe->m_windowOrigin.y);

   FREEIF(pMe->m_clipInfo.intervals);

   /* Allocate the memory for intervals, must be as high as dest bitmap */
   pMe->m_clipInfo.intervals = (Interval *)MALLOC(bi.cy * sizeof(Interval));
   
   if (bi.cy && (Interval *)0 == pMe->m_clipInfo.intervals) {
      nErr = ENOMEMORY;
   }
   
   return nErr;
}


/*===========================================================================

Interface Method - Implements IGraphics_GetDestination

===========================================================================*/
static IBitmap* QGraphics_GetDestination(IGraphics *po)
{
   IBitmap *pOldDst = ((QGraphics*) po)->m_pbmDst;
   IBITMAP_AddRef(pOldDst);
   return pOldDst;
}


/*===========================================================================

Interface Method - Implements IGraphics_SetStrokeStyle

===========================================================================*/
static AEEStrokeStyle QGraphics_SetStrokeStyle(IGraphics *po, AEEStrokeStyle strokeStyle)
{
   AEEStrokeStyle ssPrev;

   ssPrev = ((QGraphics*) po)->m_strokeStyle;
   ((QGraphics*) po)->m_strokeStyle = strokeStyle;

   return ssPrev;
}


/*===========================================================================

Interface Method - Implements IGraphics_GetStrokeStyle

===========================================================================*/
static AEEStrokeStyle QGraphics_GetStrokeStyle(IGraphics *po)
{
   return ((QGraphics*) po)->m_strokeStyle;
}


/*===========================================================================

Interface Method - Implements IGraphics_DrawEllipticalArc

===========================================================================*/
static int QGraphics_DrawEllipticalArc(IGraphics *po, AEERect *pRect,
                                       int16 startAngle, int16 arcAngle )
{
   int exception = SUCCESS;
   QGraphics *pMe = (QGraphics *) po;
   AEERect screenRect;

   if( pRect == NULL || pRect->dx < 0 || pRect->dy < 0 ) {
      THROW(exception, EBADPARM);
   }
   if( pRect->dx == 0 || pRect->dy == 0 || arcAngle == 0 ) {
      THROW(exception, SUCCESS);
   }

   transformRectToScreen(pMe, &screenRect, pRect);

   reduceArc(startAngle, arcAngle, &startAngle, &arcAngle);

   if( arcAngle == 360 ) {
      if (pMe->m_fill) {
         if (pMe->m_fg != pMe->m_fl || pMe->m_strokeStyle == STROKE_DOTTED) {
            TRY(exception, drawEllipse(pMe, &screenRect,
                        AEE_GRAPHICS_FILL | AEE_GRAPHICS_FRAME));
         } else {
            TRY(exception, drawEllipse(pMe, &screenRect, AEE_GRAPHICS_FILL));
         }
      }
      else {
         TRY(exception, drawEllipse(pMe, &screenRect, AEE_GRAPHICS_FRAME));
      }
   }
   else {
      if (pMe->m_fill) {
         if (pMe->m_fg != pMe->m_fl || pMe->m_strokeStyle == STROKE_DOTTED) {
            TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                              AEE_GRAPHICS_FILL | AEE_GRAPHICS_FRAME));
         } else {
            TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                              AEE_GRAPHICS_FILL));
         }
      }
      else {
         TRY(exception, drawEllipticalArc(pMe, &screenRect, startAngle, arcAngle,
                           AEE_GRAPHICS_FRAME | AEE_GRAPHICS_ARC));
      }
   }
   CATCH(exception);
   return exception;
}


/*===========================================================================

Interface Method - Implements IGraphics_DrawRoundRectangle

===========================================================================*/
static int QGraphics_DrawRoundRectangle(IGraphics *po, AEERect *pRect,
                                         int16 arcWidth, int16 arcHeight )
{
   int exception = SUCCESS;
   QGraphics *pMe = (QGraphics *) po;
   AEERect screenRect;

   if( pRect == NULL ||
       pRect->dx < 0 || pRect->dy < 0 ||
       arcWidth < 0  || arcHeight < 0 ||
       arcWidth > pRect->dx || arcHeight > pRect->dy ) {
      THROW(exception, EBADPARM);
   }

   if( pRect->dx == 0 || pRect->dy == 0 ) {
      THROW(exception, SUCCESS);
   }

   if( arcWidth <= 2 || arcHeight <= 2 ) {
      // Rectangle not rounded - just call regular draw rectangle.
      return QGraphics_DrawRect( po, pRect );
   }

   transformRectToScreen( pMe, &screenRect, pRect );

   if (pMe->m_fill) {
      if (pMe->m_fg != pMe->m_fl || pMe->m_strokeStyle == STROKE_DOTTED) { 
         TRY(exception, drawRoundRectangle(pMe, &screenRect, arcWidth, arcHeight,
                            AEE_GRAPHICS_FILL | AEE_GRAPHICS_FRAME));
      } else {
         TRY(exception, drawRoundRectangle(pMe, &screenRect, arcWidth, arcHeight,
                            AEE_GRAPHICS_FILL));
      }
   }
   else {
      TRY(exception, drawRoundRectangle(pMe, &screenRect, arcWidth, arcHeight,
                         AEE_GRAPHICS_FRAME));
   }

   CATCH(exception);
   return exception;
}


/*===========================================================================

  Helper Function: resetClip()

===========================================================================*/
// Reset the clipping region to the entire display window (window is not necesarily
// identical to the screen).
static void resetClip(QGraphics *pMe)
{
   pMe->m_clip.type = CLIPPING_RECT;
   pMe->m_clipInfo.rect = pMe->m_viewport;
   if (pMe->m_framed) {
      pMe->m_clipInfo.rect.xMin++;
      pMe->m_clipInfo.rect.yMin++;
      pMe->m_clipInfo.rect.xMax--;
      pMe->m_clipInfo.rect.yMax--;
   }
}


/*===========================================================================

  Helper Function: initShapeClipInfo()

===========================================================================*/
static void initShapeClipInfo(QGraphics *pMe, AEERect *pRect, uint8 nFlag)
{
   Interval *intervals = pMe->m_clipInfo.intervals;
   int xMin, xMax, yMin, yMax; 
   int i;

   yMin = pMe->m_clipInfo.rect.yMin = MIN(pMe->m_cy-1,MAX(pRect->y, pMe->m_clipInfo.rect.yMin));

   yMax = pMe->m_clipInfo.rect.yMax = MAX(0,MIN(pRect->y + pRect->dy - 1, pMe->m_clipInfo.rect.yMax));

   xMin = pMe->m_clipInfo.rect.xMin = MIN(pMe->m_cx-1,MAX(pRect->x, pMe->m_clipInfo.rect.xMin));
   xMax = pMe->m_clipInfo.rect.xMax = MAX(0,MIN(pRect->x + pRect->dx - 1, pMe->m_clipInfo.rect.xMax));

   // We can leave the rest of the array garbage, because
   // we'll never use them.
   for (i = yMin; i <= yMax; i++) {
      intervals[i].xMin = xMin;
      intervals[i].xMax = xMax;
   }

   // Make the first and last intervals invalid if necesary
   // Note when the left end if greater than the right, it's an empty
   // interval
   if (nFlag & AEE_GRAPHICS_FRAME) {

      // only invalidate the intervals if they're on the edge of the region,
      // not if they're on the edge of the screen
      
      if (yMin != 0 || yMin == pRect->y) {
         intervals[yMin].xMin = 1;
         intervals[yMin].xMax = 0;
      }

      if (yMax != pMe->m_cy-1 ||
          yMax == pRect->y + pRect->dy - 1) {
         intervals[yMax].xMin = 1;
         intervals[yMax].xMax = 0;
      }

   }
}


/*===========================================================================

  Helper Function: Checks if the y is within the bounding box along Y-axis

===========================================================================*/
static boolean withinYBound(QGraphics *pMe, int y)
{
   if (y < pMe->m_clipInfo.rect.yMin)
      return FALSE;
   
   if (y > pMe->m_clipInfo.rect.yMax)
      return FALSE;

   return TRUE;
}


/*===========================================================================

  Helper Function: updateInterval()

===========================================================================*/
static void updateInterval(QGraphics *pMe, int y, int xMin, int xMax)
{
   Interval *intervals = pMe->m_clipInfo.intervals;

   if (withinYBound(pMe, y)) {
      if (xMin > intervals[y].xMin)
         intervals[y].xMin = xMin;

      if (xMax < intervals[y].xMax)
         intervals[y].xMax = xMax;
   }
}


/*===========================================================================

  Helper Function: Checks if (x, y) in screen (device) coordinates

===========================================================================*/
static boolean valid(QGraphics *pMe, int x, int y)
{
   switch (pMe->m_clip.type) {
// CLIPPING_NONE should never happen as an internal state!!!!
//      case CLIPPING_NONE:

      case CLIPPING_CIRCLE: 
      case CLIPPING_ELLIPSE:
         if ((y >= pMe->m_clipInfo.rect.yMin) &&
             (y <= pMe->m_clipInfo.rect.yMax) &&
             (x >= pMe->m_clipInfo.intervals[y].xMin) &&
             (x <= pMe->m_clipInfo.intervals[y].xMax))
             return TRUE;

         break;

      // rectangle is the default clipping region
      default:
         if ((x >= pMe->m_clipInfo.rect.xMin) && (x <= pMe->m_clipInfo.rect.xMax) &&
             (y >= pMe->m_clipInfo.rect.yMin) && (y <= pMe->m_clipInfo.rect.yMax))
            return TRUE;
   }

   return FALSE;
}


/*===========================================================================

  Helper Function: preprocessLine()

 In order to have consistent drawing of a line, we preprocess
 a line so that its starting point (sx, sy) is always to the left
 of its ending point (ex, ey), for non-vertical lines.
 If a line is vertical, then the preprocessing makes sure that sy <= ey.

 However we do not want to change the original line structure.

===========================================================================*/
static void preprocessLine(AEELine *out, const AEELine *in)
{
   if ((in->ex < in->sx) || (in->ex == in->sx && in->ey < in->sy)) {
      out->ex = in->sx;
      out->ey = in->sy;
      out->sx = in->ex;
      out->sy = in->ey;
   }
   else
      *out = *in;
}


/*===========================================================================

  Helper Function: preprocessDottedLine()

 In order to have consistent drawing of a line, we preprocess
 a line so that its starting point (sx, sy) is always to the left
 of its ending point (ex, ey), for non-vertical lines.
 If a line is vertical, then the preprocessing makes sure that sy <= ey.

 However we do not want to change the original line structure.

===========================================================================*/
static void preprocessDottedLine(AEELine *out, const AEELine *in,
                                 int nDotPhase, int *nStartPhase, int *nEndPhase)
{
   int16 dx;
   int16 dy;
   int d;

   // Adjust phase using the max difference
   dx = ABS(in->sx - in->ex);
   dy = ABS(in->sy - in->ey);
   d = (int) MAX( dx, dy );

   if ((in->ex < in->sx) || (in->ex == in->sx && in->ey < in->sy)) {
      out->ex = in->sx;
      out->ey = in->sy;
      out->sx = in->ex;
      out->sy = in->ey;

      *nStartPhase = DOT_PHASE_SUB(DotPhaseMap[nDotPhase], d);
      *nEndPhase   = DOT_PHASE_INC(DotPhaseMap[*nStartPhase]);
   }
   else {
      *out = *in;
      *nStartPhase = nDotPhase;
      *nEndPhase = DOT_PHASE_ADD( nDotPhase, d + 1);
   }
}


/*===========================================================================

  Helper Function: WriteOut()

===========================================================================*/
static void WriteOut(byte* outBuffer, int16 i,int16 j, uint32 value, 
                     int16 depth,int16 outLineBytes)
{
   uint32 shiftedIn;	
   PACKED uint32* pointer;
   uint32 rawValue, mod, maskseed, mask0, mask1,
      mask0011000;

   uint32 byteoffset = j*outLineBytes + i*depth/8;
   outBuffer += byteoffset;
   pointer = (uint32*) outBuffer;
   rawValue = *pointer;
   mod = i*depth%8;
   maskseed = (1 << depth) -1; //11
	
   if(BIG_ENDIAN()){ // big endian
		
      mask1 = maskseed << (32 - mod - depth); // 001100
      mask1 = ~mask1; // 110011
      rawValue = rawValue & mask1;
      shiftedIn = value << (32 - mod - depth);
      rawValue = rawValue | shiftedIn;
   }
   else { // little endian  qualcomm
      if(depth >= 8) {
         mask0 = ~maskseed;
         rawValue = rawValue & mask0;	
         rawValue = rawValue | value;
			
      }
      else {
         mask0011000 = maskseed << (8-mod-depth);
         mask0 = ~mask0011000; // 110011
         rawValue = rawValue & mask0;
         shiftedIn = value <<  (8-mod-depth) ;
         rawValue = rawValue | shiftedIn;
			
      }
   }
	
	
   *pointer = rawValue;
}

/*===========================================================================

  Helper Function: GetValue()

===========================================================================*/
uint32 GetValue(byte* inBuffer, int16 i,int16 j, int16 depth,
                int16 inputLineBytes)
{

   PACKED uint32* pointer;
   uint32 rawValue, mod, maskseed,mask0011000;

   uint32 byteoffset = j*inputLineBytes + i*depth/8;
   inBuffer += byteoffset;
   pointer = (uint32*) inBuffer;

   rawValue = *pointer;
   mod = i*depth%8;
   maskseed = (1 << depth)-1; // 0011	
	
   if(BIG_ENDIAN()){ // big endian
		
      rawValue = rawValue<<mod;
      rawValue = rawValue >> (32 - depth);
		
		
   }
   else { // little endian
      if(depth >= 8) {			
         rawValue = rawValue & maskseed;		
      }
      else {
         mask0011000 = maskseed << (8-mod-depth);
         rawValue = rawValue & mask0011000;
         rawValue = rawValue >> (8-mod-depth);
      }
   }
   return rawValue;
}


/*===========================================================================

  Helper Function: clipVScanline()

// This function can only clip a vertical scan line against a rectangle.
// If the clipping region is not rectangular, the program should still call
// this function to clip the vertical line against the bounding box.  This
// will improve the efficency by eliminating some unnecesary testing on the
// pixel level.
// In sumary, the best we can do it to clip a vertical line to the bounding
// box of the clipping region.
===========================================================================*/
static boolean clipVScanline(QGraphics *pMe, int x,
                             int16 yMinIn, int16 yMaxIn,
                             int16 *yMinOut, int16 *yMaxOut)
{
   if ((x < pMe->m_clipInfo.rect.xMin) || (x > pMe->m_clipInfo.rect.xMax))
      return FALSE;
   
   *yMinOut = MAX(pMe->m_clipInfo.rect.yMin, yMinIn);
   *yMaxOut = MIN(pMe->m_clipInfo.rect.yMax, yMaxIn);

   if (*yMinOut > *yMaxOut)
      return FALSE;

   return TRUE;
}

/*===========================================================================

  Helper Function: clipHScanline()

  Changes to this function should also be made in drawHScanline.

===========================================================================*/
static boolean clipHScanline(QGraphics *pMe, int16 y,
                             int16 xMinIn, int16 xMaxIn, 
                             int16 *xMinOut, int16 *xMaxOut)
{
   int xMin, xMax;

   // quick rejection
   if ((y < pMe->m_clipInfo.rect.yMin) || (y > pMe->m_clipInfo.rect.yMax))
      return FALSE;

   switch (pMe->m_clip.type) {
   case CLIPPING_CIRCLE:
   case CLIPPING_ELLIPSE:
      xMin = pMe->m_clipInfo.intervals[y].xMin;
      xMax = pMe->m_clipInfo.intervals[y].xMax;
      break;

   // All other cases:
   default:
      xMin = pMe->m_clipInfo.rect.xMin;
      xMax = pMe->m_clipInfo.rect.xMax;
      break;
   }

   *xMinOut = MAX(xMin, xMinIn);
   *xMaxOut = MIN(xMax, xMaxIn);

   if (*xMinOut > *xMaxOut)
      return FALSE;

   return TRUE;
}




// The following code is the Liang-Barsky line clipping algorithm 
// modified for integers only.  It is carefully modified such that
// little efficiency is lost.  On a processor with slow floating
// point operation, the modification may even improve the performance.

//#define ROUND(a) ((int)(a+0.5))
#define fractionGreater(x, y) \
SIGNEQ((x).d, (y).d) ? \
   (((x).e * (y).d) > ((x).d * (y).e)) : \
   (((x).e * (y).d) < ((x).d * (y).e))



/*===========================================================================

  Helper Function: clipTest()

===========================================================================*/
static boolean clipTest(int p, int q, FractionType *u1, FractionType *u2)
{
   FractionType r;
   boolean retVal = TRUE;
   
   if (p < 0) {
      r.e = q, r.d = p;
      if (fractionGreater(r, *u2))
         retVal = FALSE;
      else if (fractionGreater(r, *u1))
         *u1 = r;
   }
   else if (p > 0) {
      r.e = q, r.d = p;
      
      if (fractionGreater(*u1, r))
         retVal = FALSE;
      else if(fractionGreater(*u2, r))
         *u2 = r;
   }
   else if (q < 0)
      /* Line is outside clipping edge */
      retVal = FALSE;
   
   return (retVal);
}


/*===========================================================================

  Helper Function: clipLine()
 Clipping a general line using "window"
 It returns TRUE if part of the line is in the window,
 FALSE otherwise.
===========================================================================*/
static boolean clipLine(QGraphics *pMe, AEELine *original, AEELine *clipped)
{
   int winLeft = pMe->m_clipInfo.rect.xMin;
   int winRight = pMe->m_clipInfo.rect.xMax;
   int winTop = pMe->m_clipInfo.rect.yMin;
   int winBottom = pMe->m_clipInfo.rect.yMax;

   // dereferencing for better efficiency
   int p1x = original->sx, p1y = original->sy;   // starting point
   int p2x = original->ex, p2y = original->ey;   // ending point

   FractionType u1 = {0, 1}, u2 = {1, 1}; //u1 = 0.0, u2 = 1.0;
   int dx = p2x - p1x, dy;
   
   if (clipTest (-dx, p1x - winLeft, &u1, &u2)) 
      if (clipTest (dx, winRight - p1x, &u1, &u2)) {
         dy = p2y - p1y;
         if (clipTest (-dy, p1y - winTop, &u1, &u2))
            if (clipTest (dy, winBottom - p1y, &u1, &u2)) {
               if (((u2.d > 0) && (u2.d > u2.e)) || ((u2.d < 0) && (u2.d < u2.e))) {
                  int x_numer = 2 * u2.e * dx;
                  int y_numer = 2 * u2.e * dy;

                  x_numer += SIGNEQ( x_numer, u2.d ) ? u2.d : -u2.d;
                  y_numer += SIGNEQ( y_numer, u2.d ) ? u2.d : -u2.d;

                  p2x = p1x + x_numer / (2 * u2.d);
                  p2y = p1y + y_numer / (2 * u2.d);
               }

               if (SIGNEQ(u2.e, u2.d)) {
                  int x_numer = 2 * u1.e * dx;
                  int y_numer = 2 * u1.e * dy;

                  x_numer += SIGNEQ( x_numer, u1.d ) ? u1.d : -u1.d;
                  y_numer += SIGNEQ( y_numer, u1.d ) ? u1.d : -u1.d;

                  p1x += x_numer / (2 * u1.d);
                  p1y += y_numer / (2 * u1.d);
               }

               // At least part of the line is visible in the window
               clipped->sx = p1x;
               clipped->sy = p1y;
               clipped->ex = p2x;
               clipped->ey = p2y;

               return TRUE;
            }
      }

   return FALSE;
}

// End of Line Clipping 


/*===========================================================================

  Helper Function: drawHScanline

  Clips and then draws a horizontal scan line replaces commonly used code:

  int16 xMin, xMax;
  if( clipHScanline( PME, LINE, XMIN, XMAX, &xMin, &xMax ) )
     IBITMAP_DrawHScanline( PBM, LINE, xMin, xMax, COLOR, RO );
  
  Changes to the clipping in this function may also need to be made in
  clipHScanline.
===========================================================================*/
static void drawHScanline(QGraphics *pMe, int16 y,
                          int16 xMinIn, int16 xMaxIn,
                          NativeColor color, AEERasterOp ro )
{
   int xMin, xMax;

   // quick rejection
   if ((y < pMe->m_clipInfo.rect.yMin) || (y > pMe->m_clipInfo.rect.yMax))
      return;

   switch (pMe->m_clip.type) {
   case CLIPPING_CIRCLE:
   case CLIPPING_ELLIPSE:
      xMin = pMe->m_clipInfo.intervals[y].xMin;
      xMax = pMe->m_clipInfo.intervals[y].xMax;
      break;

   // All other cases:
   default:
      xMin = pMe->m_clipInfo.rect.xMin;
      xMax = pMe->m_clipInfo.rect.xMax;
      break;
   }

   if( xMin < xMinIn )
      xMin = xMinIn;

   if( xMax > xMaxIn )
      xMax = xMaxIn;

   if (xMin > xMax)
      return;

   IBITMAP_DrawHScanline( pMe->m_pbmDst, y, xMin, xMax, color, ro );
}

/*===========================================================================

  Helper Function: drawHScanline_Dotted

  Clips and then draws a horizontal scan line like drawHScanline, but
  also takes DOTTED into account

  Changes to the clipping in this function may also need to be made in
  clipHScanline.
===========================================================================*/
static void drawHScanline_Dotted(QGraphics * pMe, int16 y, int16 xMinIn, 
                                 int16 xMaxIn, NativeColor color, 
                                 AEERasterOp ro, boolean bDotted,
                                 int * piDotPhase)
{
   int16     x;
   int       xMin, xMax;
   IBitmap * pbm = pMe->m_pbmDst;

   // quick rejection
   if ((y < pMe->m_clipInfo.rect.yMin) || (y > pMe->m_clipInfo.rect.yMax))
      return;

   switch (pMe->m_clip.type) {
   case CLIPPING_CIRCLE:
   case CLIPPING_ELLIPSE:
      xMin = pMe->m_clipInfo.intervals[y].xMin;
      xMax = pMe->m_clipInfo.intervals[y].xMax;
      break;

   // All other cases:
   default:
      xMin = pMe->m_clipInfo.rect.xMin;
      xMax = pMe->m_clipInfo.rect.xMax;
      break;
   }

   if( xMin < xMinIn )
      xMin = xMinIn;

   if( xMax > xMaxIn )
      xMax = xMaxIn;

   if (xMin > xMax)
      return;

   if ( ! bDotted)
   {
     IBITMAP_DrawHScanline( pMe->m_pbmDst, y, xMin, xMax, color, ro );
   }
   else     // must take Dotted into account
   {
     for ( x=xMin;  x<=xMax; x++)  // For all the pixels in the Arc
     {
       if (DOT_PHASE_ON(*piDotPhase))
       { // If we need to draw this pixel
         IBITMAP_DrawPixel( pbm, x, y, color, ro );
       }
       *piDotPhase = DOT_PHASE_INC(*piDotPhase);
     }
   }
}

// All the function with name drawLineXXXXXX assumes that
// the line has been clipped correctly if "bClipped" is TRUE, 
// and (x, y) are screen coordinates.
// If "bClipped" is FALSE, we have to test each point before drawing.


// Reviewer of the code may question the duplication of code.  This is
// very unfortunate.  In theory, we should clip every line before calling
// the line drawing functions.  Unfortunately there is no efficient way to 
// intersect a line with a circle or ellipse, without FLOAT/DOUBLE data 
// type.  So when we drawing a line against a circular clipping region,
// we have to test the validity of each point.   We don't need such a test
// if we are only clipping against a rectangular region.  But we don't want
// to check the flag "bClipped" before setting each pixel.  So we have to 
// duplicate the code for that.

/*===========================================================================

  Helper Function: drawLineVertical()

 Assuming startY < endY
===========================================================================*/
static void drawLineVertical(QGraphics *pMe, int16 x, int16 startY, int16 endY, 
                             NativeColor color,
                             AEERasterOp ro,
                             boolean bNeedTest)
{
   int16 y;

   // Avoid repeated dereferencing
   IBitmap *pbm = pMe->m_pbmDst;

   if (bNeedTest) {
      for (y = startY; y <= endY; y++) {
         if (valid(pMe, x, y)) {
            IBITMAP_DrawPixel(pbm, x, y, color, ro);
         }
      }
   } else {
      AEERect rc;
      rc.x = x;
      rc.dx = 1;
      rc.y = startY;
      rc.dy = endY - startY + 1;
      IBITMAP_FillRect(pbm, &rc, color, ro);
   }
}


/*===========================================================================

  Helper Function: drawLinePositiveSlopeLessThanOne()

===========================================================================*/
static void drawLinePositiveSlopeLessThanOne(QGraphics *pMe, 
                                            int16 xStart, int16 yStart, 
                                            int16 xEnd, int16 dx, int16 dy,
                                            boolean bClipped)
{
   int16 pyx = 2 * dy - dx;
   int16 twoDy = 2 * dy;
   int16 twoDyDx = 2 * (dy - dx);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   AEERasterOp ro = pMe->m_ro;
   
   // draw starting point
   if (bClipped || valid(pMe, x, y))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);

   if (bClipped) {
      while (x < xEnd) {
         x++;
         if (pyx < 0)
            pyx += twoDy;
         else {
            y++;
            pyx += twoDyDx;
         }
  
         IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
   else {   // Duplicating code for better efficiency. We don't want to check "bClipped" in the "while" loop
      while (x < xEnd) {
         x++;
         if (pyx < 0)
            pyx += twoDy;
         else {
            y++;
            pyx += twoDyDx;
         }
         
         if (valid(pMe, x, y))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
}


/*===========================================================================

  Helper Function: drawLinePositiveSlopeGreaterThanOne()

===========================================================================*/
static void drawLinePositiveSlopeGreaterThanOne(QGraphics *pMe, 
                                               int16 xStart, int16 yStart, int16 yEnd, 
                                               int16 dx, int16 dy,
                                               boolean bClipped)
{
   int16 pxy = 2 * dx - dy;
   int16 twoDx = 2 * dx;
   int16 twoDxDy = 2 * (dx - dy);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   AEERasterOp ro = pMe->m_ro;
   
   // draw starting point
   if (bClipped || valid(pMe, x, y))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   
   if (bClipped) {
      while (y < yEnd) {
         y++;
         if (pxy < 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }
  
         IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
   else {
      while (y < yEnd) {
         y++;
         if (pxy < 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }
      
         if (valid(pMe, x, y))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
}


/*===========================================================================

  Helper Function: drawLineNegativeSlopeGreaterThanMinusOne()

===========================================================================*/
static void drawLineNegativeSlopeGreaterThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                     int16 yStart, int16 xEnd, 
                                                     int16 dx, int16 dy, boolean bClipped)
{
   int16 pyx = 2 * dy + dx;
   int16 twoDy = 2 * dy;
   int16 twoDyDx = 2 * (dy + dx);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   AEERasterOp ro = pMe->m_ro;
   
   // draw starting point
   if (bClipped || valid(pMe, x, y))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   
   if (bClipped) {
      while (x < xEnd) {
         x++;
         if (pyx > 0)
            pyx += twoDy;
         else {
            y--;
            pyx += twoDyDx;
         }
         
         IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
   else {
      while (x < xEnd) {
         x++;
         if (pyx > 0)
            pyx += twoDy;
         else {
            y--;
            pyx += twoDyDx;
         }
         
         if (valid(pMe, x, y))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
}


/*===========================================================================

  Helper Function: drawLineNegativeSlopeLessThanMinusOne()

===========================================================================*/
static void drawLineNegativeSlopeLessThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                  int16 yStart, 
                                                  int16 yEnd, int16 dx, int16 dy, 
                                                  boolean bClipped)
{
   int16 pxy = 2 * dx + dy;
   int16 twoDx = 2 * dx;
   int16 twoDxDy = 2 * (dx + dy);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   AEERasterOp ro = pMe->m_ro;
   
   // draw starting point
   if (bClipped || valid(pMe, x, y))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   
   if (bClipped) {
      while (y < yEnd) {
         y++;
         if (pxy > 0)
            pxy += twoDx;
         else {
            x--;
            pxy += twoDxDy;
         }

         IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
   else {
      while (y < yEnd) {
         y++;
         if (pxy > 0)
            pxy += twoDx;
         else {
            x--;
            pxy += twoDxDy;
         }

         if (valid(pMe, x, y))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
      }
   }
}


/*===========================================================================

  Helper Function: drawLine()
 "screenLine" is in screen/device coordinates
===========================================================================*/
static void drawLine(QGraphics *pMe, AEELine *pLine, boolean bClipped)
{
   AEELine line = *pLine;  // dereferencing for better efficiency

   // draw vertical line
   if (line.sx == line.ex)
         drawLineVertical(pMe, line.sx, line.sy, line.ey, pMe->m_fg, pMe->m_ro, (boolean)!bClipped);
   // draw horizontal line
   else if (line.sy == line.ey) {
      if (bClipped)
         IBITMAP_DrawHScanline(pMe->m_pbmDst, line.sy, line.sx, line.ex, pMe->m_fg, pMe->m_ro);
      else
         drawHScanline(pMe, line.sy, line.sx, line.ex, pMe->m_fg, pMe->m_ro);
   }
   else {   // general line
      int16 dx = line.ex - line.sx;
      int16 dy = line.ey - line.sy;

      // Due to preprocessing, dx is always positive if it ever reaches this point
      if (dy > 0) { // positive slope
         if (dx > dy) // slope is between 0 and 1, increment along X
            drawLinePositiveSlopeLessThanOne(pMe, line.sx, line.sy, line.ex, dx, dy, bClipped);
         else   // slope is greater than 1, increment along Y
            drawLinePositiveSlopeGreaterThanOne(pMe, line.sx, line.sy, line.ey, dx, dy, bClipped);
      }
      else {   // negative slope
         if (ABS(dy) < dx)  // slope is between -1 and 0
            // Increment along X while decrementing along Y
            drawLineNegativeSlopeGreaterThanMinusOne(pMe, line.sx, line.sy, line.ex, dx, dy, bClipped);
         else  // slope is between negative infinity and -1
            // Increment along Y while decrement along X.
            drawLineNegativeSlopeLessThanMinusOne(pMe, line.ex, line.ey, line.sy, (int16)-dx, (int16)-dy, bClipped);
      }
   }
}



/*===========================================================================

  Helper Function: processAndDrawLine()
===========================================================================*/
static void processAndDrawLine(QGraphics *pMe, const AEELine *pLine)
{
   AEELine niceLine, clippedLine;

   // Proprocess the line such that the starting point and ending point
   // are consistent with the algorithm. 
   preprocessLine(&niceLine, pLine);
   
   if ((pMe->m_clip.type == CLIPPING_RECT) && clipLine(pMe, &niceLine, &clippedLine)) {
      preprocessLine(&niceLine, &clippedLine);
      drawLine(pMe, &niceLine, TRUE);
   } else {
      drawLine(pMe, &niceLine, FALSE);      
   }
}



/*===========================================================================

  Helper Function: drawDottedLineVertical()

 Assuming startY < endY
===========================================================================*/
static void drawDottedLineVertical(QGraphics *pMe,
                                   int16 x, int16 startY, int16 endY,
                                   NativeColor color, 
                                   AEERasterOp ro,
                                   boolean bNeedTest,
                                   int nDotPhase)
{
   int16 y;

   // Avoid repeated dereferencing
   IBitmap *pbm = pMe->m_pbmDst;

   if (bNeedTest) {
      for (y = startY; y <= endY; y++) {
         if (valid(pMe, x, y) && DOT_PHASE_ON(nDotPhase)) {
            IBITMAP_DrawPixel(pbm, x, y, color, ro);
         }
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   } else {
      for (y = startY; y <= endY; y++) {
         if (DOT_PHASE_ON(nDotPhase)) {
            IBITMAP_DrawPixel(pbm, x, y, color, ro);
         }
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
}


/*===========================================================================

  Helper Function: drawDottedLineHorizontal()

 Assuming startX < endX
===========================================================================*/
static void drawDottedLineHorizontal(QGraphics *pMe,
                                    int16 y, int16 startX, int16 endX, 
                                    NativeColor color,
                                    AEERasterOp ro,
                                    int nDotPhase)
{
   int16 x;
   
   // Avoid repeated dereferencing
   IBitmap *pbm = pMe->m_pbmDst;

   for (x = startX; x <= endX; x++ ) {
      if (DOT_PHASE_ON(nDotPhase)) {
         IBITMAP_DrawPixel(pbm, x, y, color, ro);
      }
      nDotPhase = DOT_PHASE_INC(nDotPhase);
   }
}


/*===========================================================================

  Helper Function: drawDottedLinePositiveSlopeLessThanOne()

===========================================================================*/
static void drawDottedLinePositiveSlopeLessThanOne(QGraphics *pMe, 
                                                   int16 xStart, int16 yStart, 
                                                   int16 xEnd, int16 dx, int16 dy,
                                                   AEERasterOp ro,
                                                   boolean bClipped, int nDotPhase)
{
   int16 pyx = 2 * dy - dx;
   int16 twoDy = 2 * dy;
   int16 twoDyDx = 2 * (dy - dx);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   
   // draw starting point
   if ((bClipped || valid(pMe, x, y)) && DOT_PHASE_ON(nDotPhase))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   nDotPhase = DOT_PHASE_INC(nDotPhase);

   if (bClipped) {
      while (x < xEnd) {
         x++;
         if (pyx < 0)
            pyx += twoDy;
         else {
            y++;
            pyx += twoDyDx;
         }
  
         if( DOT_PHASE_ON(nDotPhase) )
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
   else {   // Duplicating code for better efficiency. We don't want to check "bClipped" in the "while" loop
      while (x < xEnd) {
         x++;
         if (pyx < 0)
            pyx += twoDy;
         else {
            y++;
            pyx += twoDyDx;
         }
         
         if (valid(pMe, x, y) && DOT_PHASE_ON(nDotPhase))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
}


/*===========================================================================

  Helper Function: drawDottedLinePositiveSlopeGreaterThanOne()

===========================================================================*/
static void drawDottedLinePositiveSlopeGreaterThanOne(QGraphics *pMe, 
                                                      int16 xStart, int16 yStart, int16 yEnd, 
                                                      int16 dx, int16 dy,
                                                      AEERasterOp ro,
                                                      boolean bClipped, int nDotPhase)
{
   int16 pxy = 2 * dx - dy;
   int16 twoDx = 2 * dx;
   int16 twoDxDy = 2 * (dx - dy);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   
   // draw starting point
   if ((bClipped || valid(pMe, x, y)) && DOT_PHASE_ON(nDotPhase))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   nDotPhase = DOT_PHASE_INC(nDotPhase);

   if (bClipped) {
      while (y < yEnd) {
         y++;
         if (pxy < 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }
  
         if( DOT_PHASE_ON(nDotPhase) )
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
   else {
      while (y < yEnd) {
         y++;
         if (pxy < 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }
      
         if (valid(pMe, x, y) && DOT_PHASE_ON(nDotPhase))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
}


/*===========================================================================

  Helper Function: drawDottedLineNegativeSlopeGreaterThanMinusOne()

===========================================================================*/
static void drawDottedLineNegativeSlopeGreaterThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                           int16 yStart, int16 xEnd, 
                                                           int16 dx, int16 dy,
                                                           AEERasterOp ro,
                                                           boolean bClipped, int nDotPhase)
{
   int16 pyx = 2 * dy + dx;
   int16 twoDy = 2 * dy;
   int16 twoDyDx = 2 * (dy + dx);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   
   // draw starting point
   if ((bClipped || valid(pMe, x, y)) && DOT_PHASE_ON(nDotPhase))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   nDotPhase = DOT_PHASE_INC(nDotPhase);
   
   if (bClipped) {
      while (x < xEnd) {
         x++;
         if (pyx > 0)
            pyx += twoDy;
         else {
            y--;
            pyx += twoDyDx;
         }
         
         if( DOT_PHASE_ON(nDotPhase) )
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
   else {
      while (x < xEnd) {
         x++;
         if (pyx > 0)
            pyx += twoDy;
         else {
            y--;
            pyx += twoDyDx;
         }
         
         if (valid(pMe, x, y) && DOT_PHASE_ON(nDotPhase))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
}


/*===========================================================================

  Helper Function: drawDottedLineNegativeSlopeLessThanMinusOne()

===========================================================================*/
static void drawDottedLineNegativeSlopeLessThanMinusOne(QGraphics *pMe, int16 xStart, 
                                                        int16 yStart, 
                                                        int16 yEnd, int16 dx, int16 dy, 
                                                        AEERasterOp ro,
                                                        boolean bClipped, int nDotPhase)
{
   int16 pxy = 2 * dx + dy;
   int16 twoDx = 2 * dx;
   int16 twoDxDy = 2 * (dx + dy);
   int16 x = xStart, y = yStart;

   // Avoid repeated dereferencing of the same pointer in the "while" loop.  
   IBitmap *pbmDst = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   
   // draw starting point
   if ((bClipped || valid(pMe, x, y)) && DOT_PHASE_ON(nDotPhase))
      IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
   nDotPhase = DOT_PHASE_INC(nDotPhase);
   
   if (bClipped) {
      while (y > yEnd) {
         y--;
         if (pxy <= 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }

         if( DOT_PHASE_ON(nDotPhase) )
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
   else {
      while (y > yEnd) {
         y--;
         if (pxy <= 0)
            pxy += twoDx;
         else {
            x++;
            pxy += twoDxDy;
         }

         if (valid(pMe, x, y) && DOT_PHASE_ON(nDotPhase))
            IBITMAP_DrawPixel(pbmDst, x, y, fg, ro);
         nDotPhase = DOT_PHASE_INC(nDotPhase);
      }
   }
}


/*===========================================================================

  Helper Function: drawDottedLine()
 "screenLine" is in screen/device coordinates
===========================================================================*/
static void drawDottedLine(QGraphics *pMe, AEELine *pLine, AEERasterOp ro, boolean bClipped, int nDotPhase)
{
   AEELine line = *pLine;  // dereferencing for better efficiency

   // draw vertical line
   if (line.sx == line.ex)
         drawDottedLineVertical(pMe, line.sx, line.sy, line.ey,
                                pMe->m_fg, ro,
                                (boolean)!bClipped, nDotPhase);
   // draw horizontal line
   else if (line.sy == line.ey) {
      if (bClipped) {
         drawDottedLineHorizontal(pMe, line.sy, line.sx, line.ex,
                                  pMe->m_fg, ro, nDotPhase);
      }
      else {
         int16 xMin, xMax;
         if (clipHScanline(pMe, line.sy, line.sx, line.ex, &xMin, &xMax)) {
            nDotPhase = DOT_PHASE_ADD( nDotPhase, xMin - line.sx );
            drawDottedLineHorizontal(pMe, line.sy, xMin, xMax,
                                     pMe->m_fg, ro, nDotPhase);
         }
      }
   }
   else {   // general line
      int16 dx = line.ex - line.sx;
      int16 dy = line.ey - line.sy;

      // Due to preprocessing, dx is always positive if it ever reaches this point
      if (dy > 0) { // positive slope
         if (dx > dy) // slope is between 0 and 1, increment along X
            drawDottedLinePositiveSlopeLessThanOne(pMe, line.sx, line.sy, line.ex, dx, dy, ro, bClipped, nDotPhase);
         else   // slope is greater than 1, increment along Y
            drawDottedLinePositiveSlopeGreaterThanOne(pMe, line.sx, line.sy, line.ey, dx, dy, ro, bClipped, nDotPhase);
      }
      else {   // negative slope
         if (ABS(dy) < dx)  // slope is between -1 and 0
            // Increment along X while decrementing along Y
            drawDottedLineNegativeSlopeGreaterThanMinusOne(pMe, line.sx, line.sy, line.ex, dx, dy, ro, bClipped, nDotPhase);
         else  // slope is between negative infinity and -1
            // Increment along Y while decrement along X.
            drawDottedLineNegativeSlopeLessThanMinusOne(pMe, line.sx, line.sy, line.ey, (int16)dx, (int16)dy, ro, bClipped, nDotPhase);
      }
   }
}



/*===========================================================================

  Helper Function: processAndDrawDottedLine()
===========================================================================*/
static int processAndDrawDottedLine(QGraphics *pMe, const AEELine *pLine,
                                    AEERasterOp ro, int nDotPhase)
{
   AEELine niceLine, clippedLine;
   int nStartPhase;
   int nEndPhase;
   int d;

   // Proprocess the line such that the starting point and ending point
   // are consistent with the algorithm. 
   preprocessDottedLine(&niceLine, pLine, nDotPhase, &nStartPhase, &nEndPhase);
   
   if ( (pMe->m_clip.type == CLIPPING_RECT) &&
        clipLine(pMe, &niceLine, &clippedLine) ) {
      d = MAX( ABS(niceLine.sx - clippedLine.sx),
               ABS(niceLine.sy - clippedLine.sy) );
      nStartPhase = DOT_PHASE_ADD( nStartPhase, d );
      drawDottedLine(pMe, &clippedLine, ro, TRUE, nStartPhase);
   }
   else
      drawDottedLine(pMe, &niceLine, ro, FALSE, nStartPhase);

   return nEndPhase;
}




////////////////////////////////////////////////
//    POLYLINE
////////////////////////////////////////////////

/*===========================================================================

  Helper Function: drawPolyline()
===========================================================================*/
static void drawPolyline(QGraphics *pMe, int len, const AEEPoint *pts, boolean bClose)
{
   int i;
   AEELine segment;
   int nDotPhase = 0;
   
   if( pMe->m_strokeStyle == STROKE_SOLID ) {
      for (i = 1; i < len; i++) {
         segment.sx = pts[i-1].x;
         segment.sy = pts[i-1].y;
         segment.ex = pts[i].x;
         segment.ey = pts[i].y;
      
         processAndDrawLine(pMe, &segment);
      }

      // Do we want to close the "loop"
      if (bClose) {
         segment.sx = pts[len-1].x;
         segment.sy = pts[len-1].y;
         segment.ex = pts[0].x;
         segment.ey = pts[0].y;

         processAndDrawLine(pMe, &segment);
      }
   }
   else {
      for (i = 1; i < len; i++) {
         segment.sx = pts[i-1].x;
         segment.sy = pts[i-1].y;
         segment.ex = pts[i].x;
         segment.ey = pts[i].y;
      
         nDotPhase = processAndDrawDottedLine(pMe, &segment, pMe->m_ro,
                                              nDotPhase);
         // back phase by 1 because end point is drawn again
         nDotPhase = DOT_PHASE_ADD(nDotPhase, DOT_REPEAT_COUNT - 1);
      }

      // Do we want to close the "loop"
      if (bClose) {
         segment.sx = pts[len-1].x;
         segment.sy = pts[len-1].y;
         segment.ex = pts[0].x;
         segment.ey = pts[0].y;

         processAndDrawDottedLine(pMe, &segment, pMe->m_ro, nDotPhase);
      }
   }
}




///////////////////////////////////////////////////////////////////////////////////
//    RECTANGLE
///////////////////////////////////////////////////////////////////////////////////


/*===========================================================================

  Helper Function: intersectRect()

 It returns TRUE if the "first" does intersect with the "second".
 Otherwise it returns FALSE. When it returns false, the data in "intersection"
 is not valid: either the width (dx) or the height (dy) of the rectangle is
 negative.
===========================================================================*/
static boolean intersectRect(WinRect *first, AEERect *second, AEERect *intersection)
{
   if (!first || !second || !intersection)
      return FALSE;
   else {
      int16 secondRight = second->x + second->dx - 1;
      int16 secondBottom = second->y + second->dy - 1;
      int16 intersectionRight = MIN(first->xMax, secondRight);     // lower right
      int16 intersectionBottom = MIN(first->yMax, secondBottom);  // corner of the intersection
      intersection->x = MAX(first->xMin, second->x);  // Upper left
      intersection->y = MAX(first->yMin, second->y);  // corner of the intersection
      intersection->dx = intersectionRight - intersection->x + 1;

      if (intersection->dx < 0)
         return FALSE;

      intersection->dy = intersectionBottom - intersection->y + 1;
      if (intersection->dy < 0)
         return FALSE;
   }

   return TRUE;
}


/*===========================================================================

  Helper Function: drawDottedRect()

 pRect is already in screen coorindates.
 This function is called only when AEE_GRAPHICS_FRAME is set and the stroke
 style is dotted.
 We do have to clip each side seperately instead of clipping the entire
 rectangle.
===========================================================================*/
static void drawDottedRect(QGraphics *pMe, AEERect *pRect, AEERasterOp ro)
{
   int16 left = pRect->x;
   int16 right = left + pRect->dx - 1;
   int16 top = pRect->y;
   int16 bottom = top + pRect->dy - 1;
   AEELine segment;
   int nDotPhase = 0;

   // The performance of this routine can be improved by open coding the
   // functionality of processAndDrawDottedLine, preprocessDottedLine, and
   // drawDottedLine.
   // This would be a good trade off of code space vs performance.

   // Top line
   segment.sx = left;
   segment.sy = top;
   segment.ex = right;
   segment.ey = top;
   nDotPhase = processAndDrawDottedLine(pMe, &segment, ro, nDotPhase );

   // Right line
   segment.sx = right;
   segment.sy = top + 1;
   segment.ex = right;
   segment.ey = bottom;
   nDotPhase = processAndDrawDottedLine(pMe, &segment, ro, nDotPhase );

   // Bottom line
   segment.sx = right - 1;
   segment.sy = bottom;
   segment.ex = left;
   segment.ey = bottom;
   nDotPhase = processAndDrawDottedLine(pMe, &segment, ro, nDotPhase );

   // Left line
   if( bottom - 1 > top + 1 ) {
      segment.sx = left;
      segment.sy = bottom - 1;
      segment.ex = left;
      segment.ey = top + 1;
      nDotPhase = processAndDrawDottedLine(pMe, &segment, ro, nDotPhase );
   }
}


/*===========================================================================

  Helper Function: frameRect()

 pRect is already in screen coorindates.
 This function is called only when AEE_GRAPHICS_FRAME is set.
 We do have to clip each side seperately instead of clipping the entire
 rectangle.
===========================================================================*/
static void frameRect(QGraphics *pMe, AEERect *pRect, boolean needTest,
                      NativeColor color,
                      AEERasterOp ro)
{
   int16 yMin, yMax;
   int16 left = pRect->x;
   int16 right = left + pRect->dx - 1;
   int16 top = pRect->y;
   int16 bottom = top + pRect->dy - 1;

   drawHScanline(pMe, top, left, right, color, ro);
   if( top != bottom ) {
      drawHScanline(pMe, bottom, left, right, color, ro);
   }

   // It's a little messy for the two vertical edges due to possible non-rectangular
   // clipping region.

   top++;
   bottom--;

   // If "clipped == TRUE", the line is already clipped so that we can call
   // DrawVScanline. Otherwise, we call drawLineVertical, which tests each
   // pixel before calling DrawPixel.
   if (clipVScanline(pMe, left, top, bottom, &yMin, &yMax))
      drawLineVertical(pMe, left, yMin, yMax, color, ro, needTest);

   if( left != right ) {
      if (clipVScanline(pMe, right, top, bottom, &yMin, &yMax))
         drawLineVertical(pMe, right, yMin, yMax, color, ro, needTest);
   }
}


/*===========================================================================

  Helper Function: fillRect()

 pRect is in screen coordinates and already correctly clipped.
 Assuming that the input are valid
===========================================================================*/
static void fillRect(QGraphics *pMe, AEERect *pRect, boolean needTest, 
                     NativeColor color,
                     AEERasterOp ro)
{
   int16 endX, endY, startX, startY, y;
   Interval *intervals = pMe->m_clipInfo.intervals;

   endX = pRect->x + pRect->dx - 1;
   endY = pRect->y + pRect->dy - 1;
   startX = pRect->x;
   startY = pRect->y;

   // Fill the interior
   if (needTest) { // test each scanline before drawing
      int16 xMin, xMax;
      for (y = startY; y <= endY; y++) {
         xMin = MAX(intervals[y].xMin, startX);
         xMax = MIN(intervals[y].xMax, endX);
         IBITMAP_DrawHScanline(pMe->m_pbmDst, y, xMin, xMax, color, ro);
      }
   }
   else    // drawing without testing
      IBITMAP_FillRect(pMe->m_pbmDst, pRect, color, ro);
}


/*===========================================================================

  Helper Function: drawRect()

===========================================================================*/
static void drawRect(QGraphics *pMe, AEERect *pRect, uint8 nFlag)
{
   AEERasterOp ro = pMe->m_ro;   // deferencing for better efficiency
   NativeColor interiorColor = pMe->m_fl;
   boolean drawFrame    = (nFlag & AEE_GRAPHICS_FRAME) != 0;
   boolean fillInterior = (nFlag & AEE_GRAPHICS_FILL) != 0;
   boolean clip         = (nFlag & AEE_GRAPHICS_CLIP) != 0;
   AEERect interior = *pRect; // make a copy to avoid multiple dereferencing
   AEERect clippedRect;
   boolean needTest = FALSE; // default to FALSE: be optimistic about life :)

   // If AEE_GRAPHICS_FRAME is set, the interior = rect - frame.
   // Otherwise interio = rect.
   if (drawFrame) {
      interior.x ++;
      interior.y ++;
      interior.dx -= 2;
      interior.dy -= 2;
   }

   // Set the interior color and avoid doing the binary AND repeatedly.
   if (nFlag & AEE_GRAPHICS_CLEAR) {
      interiorColor = pMe->m_bg;
      fillInterior = TRUE;
      ro = AEE_RO_COPY;
   }

   if (pMe->m_clip.type != CLIPPING_RECT)
      needTest = TRUE;  // have to test each pixel before calling DrawPixel

   if (drawFrame) {   // draw the frame
      if( pMe->m_strokeStyle == STROKE_DOTTED ) {
         drawDottedRect(pMe, pRect, ro);
      }
      else {
         frameRect(pMe, pRect, needTest, pMe->m_fg, ro);
      }
   }

   if (fillInterior || clip) {
      if (intersectRect(&(pMe->m_clipInfo.rect), &interior, &clippedRect)) {
         if (fillInterior)
            fillRect(pMe, &clippedRect, needTest, interiorColor, ro);

         if (clip) {
            pMe->m_clipInfo.rect.xMin = clippedRect.x;
            pMe->m_clipInfo.rect.yMin = clippedRect.y;
            pMe->m_clipInfo.rect.xMax = clippedRect.x + clippedRect.dx - 1;
            pMe->m_clipInfo.rect.yMax = clippedRect.y + clippedRect.dy - 1;
            pMe->m_clip.shape.rect = clippedRect;
         }
      }
   }
}


////////////////////////////////////////////////////////////////////////////////////////
//    ELLIPSE (Circle, Arc, Pie, Ellipse and Elliptical Arc
////////////////////////////////////////////////////////////////////////////////////////
/*===========================================================================
  Helper Function: reduceArc()

===========================================================================*/
static void reduceArc(int16 startAngle, int16 arcAngle, int16 *reducedStart, int16 *reducedArc)
{
   // Make 0 < arcAngle <= 360
   if( arcAngle > 360 || arcAngle <= -360 ) {
      arcAngle = 360;
   }
   else if( arcAngle < 0 ) {
      startAngle += arcAngle;
      arcAngle = -arcAngle;
   }

   // Make 0 <= startAngle < 360
   if( startAngle < 0 ) {
      // This may leave startAngle == 360, so must always do second test
      startAngle += (1 - startAngle/360)*360;
   }
   if( startAngle >= 360 ) {
      startAngle = startAngle % 360;
   }

   *reducedStart = startAngle;
   *reducedArc   = arcAngle;
}


/*===========================================================================
  Helper Function: mapAngleToLine()

  Convert angle to line through center point of rectangle.

  O degrees points in the positive x direction and increasing angle goes
  counter clockwise on the display (which means from the positive x axis
  towards the negative y axis).

  The angle is scaled such that a 45 degree line will go from the origin
  to the top right corner of the rectangle.
===========================================================================*/
static void mapAngleToLine( AEERect *pRect, int16 angle,
                            AEEEllipseLineInfo *pLine )
{
   int32 delta_x;
   int32 delta_y;

   // Compute delta_x and delta_y
   if (angle >= 270) {  // 270 <= angle <= 359
      delta_x = SIN[angle - 270];
      delta_y = SIN[360 - angle];
   }
   else if (angle >= 180) { // 180 <= angle < 270
      delta_x = -SIN[270 - angle];
      delta_y = SIN[angle - 180];
   }
   else if (angle > 90) {  // 90 < angle < 180
      delta_x = -SIN[angle - 90];
      delta_y = -SIN[180 - angle];
   }
   else {   // 0 <= angle < 90
      delta_x = SIN[90 - angle];
      delta_y = -SIN[angle];
   }

   // Adjust delta_x or delta_y based on rectangle
   // This can only reduce the size of delta_...
   if( pRect->dx > pRect->dy ) {
      delta_y = delta_y * pRect->dy / pRect->dx;
   }
   else if( pRect->dx < pRect->dy ) {
      delta_x = delta_x * pRect->dx / pRect->dy;
   }

   // Determine which variable changes slowest and the step size for that
   // variable. The calculations here can not overflow 32-bits and the result
   // can not overflow 16-bits.
   if( ABS(delta_x) > ABS(delta_y) ) {
      pLine->incX = TRUE;
      pLine->step = (int16) (SIN_ACCURACY_INVERSE * delta_y / ABS(delta_x));
      pLine->increment = (delta_x < 0) ? -1 : 1;
   }
   else {
      pLine->incX = FALSE;
      pLine->step = (int16) (SIN_ACCURACY_INVERSE * delta_x / ABS(delta_y));
      pLine->increment = (delta_y < 0) ? -1 : 1;
   }

   pLine->accum = 0;

   pLine->x  = (pRect->x + pRect->x + pRect->dx) / 2;
   pLine->y  = (pRect->y + pRect->y + pRect->dy) / 2;
}

/*===========================================================================
  Helper Function: line_step()

  Move along line until the y value changes keeping track of the first x
  value and last x value associated with the previous y value.

  left_x is the left most value of x associated with the previous y value
  right_x is the right most value of x associated with the previous y value
===========================================================================*/
static void line_step( AEEEllipseLineInfo *pLine, AEERect *pRect )
{
   if( pLine->incX ) {
      // Increment x case - keep incrementing x until y changes
      if( pLine->increment > 0 )
         pLine->left_x = pLine->x;
      else
         pLine->right_x = pLine->x;

      while( pRect->x <= pLine->x &&
             pLine->x <= pRect->x + pRect->dx ) {
         pLine->x  += pLine->increment;
         pLine->accum += pLine->step;

         if( pLine->accum > HALF_SIN_ACCURACY_INVERSE ) {
            pLine->y++;
            pLine->accum -= SIN_ACCURACY_INVERSE;
            break;
         }
         else if( pLine->accum < - HALF_SIN_ACCURACY_INVERSE ) {
            pLine->y--;
            pLine->accum += SIN_ACCURACY_INVERSE;
            break;
         }
      }

      if( pLine->increment > 0 )
         pLine->right_x = pLine->x - 1;
      else
         pLine->left_x = pLine->x + 1;
   }
   else {
      // Increment y case - no loop since y always changes
      pLine->left_x = pLine->right_x = pLine->x;
      pLine->y  += pLine->increment;
      pLine->accum += pLine->step;
      if( pLine->accum > HALF_SIN_ACCURACY_INVERSE ) {
         pLine->x++;
         pLine->accum -= SIN_ACCURACY_INVERSE;
      }
      else if( pLine->accum < - HALF_SIN_ACCURACY_INVERSE ) {
         pLine->x--;
         pLine->accum += SIN_ACCURACY_INVERSE;
      }
   }
}

/*===========================================================================
  Helper Function: AEEStepInfo32To64()
===========================================================================*/
/*
* this funciton may be used if it becomes nessessary to recover from a
* int32 overflow in ellipse_step_int32
*
*static void AEEStepInfo32To64(AEEStepInfo64* pOut, AEEStepInfo32* pIn) {
*  pOut->r1 = pIn->r1;            
*  pOut->ax2 = pIn->ax2;
*  pOut->ay2 = pIn->ay2;
*  pOut->ax2pydy = pIn->ax2pydy;
*  pOut->ay2pxdx = pIn->ay2pxdx;
*  pOut->xdx = pIn->xdx;
*  pOut->xdx_offset = pIn->xdx;
*  pOut->ydy = pIn->ydy;
*  pOut->ydy_offset = pIn->ydy_offset;
*}
*/
#ifdef _MSC_VER
#  define MAX_int64 (int64)0x7FFFFFFFFFFFFFFF
#  define MIN_int64 (int64)0x8000000000000000
#else
#  define MAX_int64 (int64)LLONG_MAX 
#  define MIN_int64 (int64)LLONG_MIN
#endif
#define MAX_int32 (int32)INT_MAX
#define MIN_int32 (int32)INT_MIN
#ifdef _MSC_VER
#  define MAX_RADIUS_int64 (int64)(0x1<<30)
#else
#  define MAX_RADIUS_int64 (int64)(0x1ll<<30)
#endif
#define MAX_RADIUS_int32 (int32)(801050)

#define ON_ADD_OVERFLOW_THROW(inttype, exception, xx,yy) \
   if((((xx) > ((MAX_##inttype) - (yy))) && ((yy) > 0)) ||  \
      (((xx) < ((MIN_##inttype) - (yy))) && ((yy) < 0)) ) { \
      THROW(exception, EBADPARM); \
   }

#define ON_LSHIFT_OVERFLOW_THROW(inttype, exception, xx,nn) \
   if((xx) > ((MAX_##inttype))>>(nn) && (xx) > 0 || \
      (xx) < ((MIN_##inttype))>>(nn) && (xx) < 0 ) { \
      THROW(exception, EBADPARM); \
   }

/*===========================================================================
  Helper Function: SETUP_ELLIPSE_QUADRANT()
===========================================================================*/
#define SETUP_ELLIPSE_QUADRANT(pRect, quad, offset_x, offset_y, eInfo, pStep)\
{\
  static const int16 sine[] = {999,1,-1,-1,1};\
  static const int16 cosine[] = {999,-1,-1,1,1};\
\
  (eInfo)->quadrant = (quad);\
  (eInfo)->a.x = (pRect)->dx;\
  (eInfo)->a.y = (pRect)->dy;\
  (pStep)->ax2 = (eInfo)->a.x*(eInfo)->a.x;\
  (pStep)->ay2 = (eInfo)->a.y*(eInfo)->a.y;\
\
  /* quadrants are numbered as follows (on a +Y => down screen):
                   1     2     3     4
     clockwise     LR    LL    UL    UR
     CCW           UR    UL    LL    LP
  */\
\
  (eInfo)->q13 = ((quad) == 1 || (quad) == 3);\
  (eInfo)->d.x = 2*cosine[(quad)];\
  (eInfo)->d.y = 2*sine[(quad)];\
  (eInfo)->p.x = (eInfo)->a.x*sine[(quad)];\
  (eInfo)->p.y = (eInfo)->a.y*-cosine[(quad)];\
  if ((eInfo)->q13) {\
    (eInfo)->p.y = ((eInfo)->a.y & 1) ? ((eInfo)->d.y>>1) : 0; \
  } else {\
    (eInfo)->p.x = ((eInfo)->a.x & 1) ? ((eInfo)->d.x>>1) : 0; \
  }\
\
  /* NOTE that these calculations overflow 32 bit integers at
     diameters of approx. 1024: 1024^2 * 1024 * 2  = 2^31.
     anatolyy: check was added at the start of the macro
  */\
  (pStep)->ay2pxdx = (pStep)->ay2*(eInfo)->p.x*(eInfo)->d.x;\
  (pStep)->ax2pydy = (pStep)->ax2*(eInfo)->p.y*(eInfo)->d.y;\
\
  if ((eInfo)->q13) {\
    (pStep)->r1 = ((eInfo)->p.y == 0) ? 0 : -(pStep)->ax2;\
  } else {\
    (pStep)->r1 = ((eInfo)->p.x == 0) ? 0 : -(pStep)->ay2;\
  }\
  (pStep)->r1 -= (pStep)->ay2 + (pStep)->ay2pxdx +((pStep)->ax2 << 2) +\
     ((pStep)->ax2pydy <<1 );\
    \
  /* Draw the ellipse until we're close to the axis */\
  (eInfo)->count = (eInfo)->q13 ? ABS((eInfo)->p.x) : ABS((eInfo)->p.y);\
\
  /* Set cache values */\
  (pStep)->xdx = ((pRect)->x << 1) + (pRect)->dx;\
  (pStep)->xdx_offset = (pStep)->xdx + ((offset_x) << 1);\
  (pStep)->ydy = ((pRect)->y << 1) + (pRect)->dy;\
  (pStep)->ydy_offset = (pStep)->ydy + ((offset_y) << 1);\
\
  if( (eInfo)->q13) {\
     /*TODO: verify that casting is appropriate */\
     (eInfo)->reg.x = (int16)(((pStep)->xdx_offset + (eInfo)->p.x) >> 1);\
     (eInfo)->mir.x = (int16)(((pStep)->xdx - (eInfo)->p.x) >> 1);\
  }\
  else {\
     /*TODO: verify that casting is appropriate */\
     (eInfo)->reg.x = (int16)(((pStep)->xdx + (eInfo)->p.x) >> 1);\
     (eInfo)->mir.x = (int16)(((pStep)->xdx_offset - (eInfo)->p.x) >> 1);\
  }\
\
  (eInfo)->reg.xi = (eInfo)->reg.x - 1;\
  (eInfo)->mir.xi = (eInfo)->mir.x + 1;\
\
  /*TODO: verify that casting is appropriate */\
  (eInfo)->reg.y = (int16)(((pStep)->ydy_offset + (eInfo)->p.y) >> 1);\
  (eInfo)->mir.y = (int16)(((pStep)->ydy - (eInfo)->p.y) >> 1);\
\
  (eInfo)->reg.last_x  = (eInfo)->reg.x;\
  (eInfo)->reg.last_xi = (eInfo)->reg.xi;\
  (eInfo)->mir.last_x  = (eInfo)->mir.x;\
  (eInfo)->mir.last_xi = (eInfo)->mir.xi;\
  (eInfo)->reg.last_y  = (eInfo)->reg.y;\
  (eInfo)->mir.last_y  = (eInfo)->mir.y;\
\
}

static int setup_ellipse_quadrant_int32(AEERect *pRect, int16 quadrant,
                                   int16 offset_x, int16 offset_y,
                                   AEEEllipseInfo *eInfo, AEEStepInfo32* pStep) {
   SETUP_ELLIPSE_QUADRANT(
         pRect, 
         quadrant, 
         offset_x, 
         offset_y, 
         eInfo,
         pStep);
   return SUCCESS;
}

static int setup_ellipse_quadrant_int64(AEERect *pRect, int16 quadrant,
                                   int16 offset_x, int16 offset_y,
                                   AEEEllipseInfo *eInfo, AEEStepInfo64* pStep) {
  if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int64) {
     return EBADPARM; 
  } 
   SETUP_ELLIPSE_QUADRANT(
         pRect, 
         quadrant, 
         offset_x, 
         offset_y, 
         eInfo,
         pStep);
   return SUCCESS;
}
/*===========================================================================
  Helper Function: ELLIPSE_STEP()
===========================================================================*/
#define ellipse_step_int32 ellipse_step_int32

static int ellipse_step_int32(AEEStepInfo32* pStep, AEEEllipseInfo *eInfo)
{
  int32 r2,r3,r4;

  /* 
  * these operations overflow for relatively small radius sizes  
  * setup_quadrant is supposed to indicate if int64 is nessessary
  * to enable runtime int32 overflow recovery this function needs
  * to be changed so pStep and eInfo is updated after all the 
  * overflow checks pass.
  */
  r2 = pStep->r1 + (pStep->ax2pydy + (pStep->ax2<<1) + pStep->ax2) -
     (pStep->ay2pxdx + (pStep->ay2<<1) + pStep->ay2);

  r3 = r2 - ((pStep->ax2pydy<<1) + (pStep->ax2<<3));
  r4 = pStep->r1 - ((pStep->ay2pxdx<<1) + (pStep->ay2<<3));

  /* Step in X? */
  if ((eInfo->q13 ^ (pStep->r1 > 0)) || (eInfo->q13 ^ (r3 > 0))) {
    eInfo->p.x += eInfo->d.x;
    pStep->r1 -= (pStep->ay2pxdx<<1)+(pStep->ay2<<3);
    pStep->ay2pxdx += pStep->ay2<<2;
    if (eInfo->q13) eInfo->count -= 2;
  }
  
  /* Step in Y? */
  if ((!eInfo->q13 ^ (r2 > 0)) || (!eInfo->q13 ^ (r4 > 0))) {
    eInfo->p.y += eInfo->d.y;
    pStep->r1 -= (pStep->ax2pydy<<1)+((pStep->ax2+pStep->ax2+pStep->ax2)<<2);
    pStep->ax2pydy += pStep->ax2<<2;
    if (!eInfo->q13) eInfo->count -= 2;
  }

  eInfo->reg.last_x  = eInfo->reg.x;
  eInfo->reg.last_xi = eInfo->reg.xi;
  eInfo->mir.last_x  = eInfo->mir.x;
  eInfo->mir.last_xi = eInfo->mir.xi;
  eInfo->reg.last_y  = eInfo->reg.y;
  eInfo->mir.last_y  = eInfo->mir.y;

  if( eInfo->q13 ) {
     //TODO: verify that casting is appropriate
     eInfo->reg.x = (int16)((pStep->xdx_offset + eInfo->p.x) >> 1);
     eInfo->mir.x = (int16)((pStep->xdx - eInfo->p.x) >> 1);
  }
  else {
     //TODO: verify that casting is appropriate
     eInfo->reg.x = (int16)((pStep->xdx + eInfo->p.x) >> 1);
     eInfo->mir.x = (int16)((pStep->xdx_offset - eInfo->p.x) >> 1);
  }

  eInfo->reg.xi = eInfo->reg.x - 1;
  eInfo->mir.xi = eInfo->mir.x + 1;

  //TODO: verify that casting is appropriate
  eInfo->reg.y = (int16)((pStep->ydy_offset + eInfo->p.y) >> 1);
  eInfo->mir.y = (int16)((pStep->ydy - eInfo->p.y) >> 1);
  return SUCCESS;
}

#define ellipse_step_int64 ellipse_step_int64

int ellipse_step_int64(AEEStepInfo64* pStep, AEEEllipseInfo* eInfo) 
{ 
  int exception = SUCCESS;
  int64 r2,r3,r4,tmp; 
  int64 r1      = (pStep)->r1; 
  int64 ax2     = (pStep)->ax2; 
  int64 ay2     = (pStep)->ay2; 
  int64 ax2pydy = (pStep)->ax2pydy; 
  int64 ay2pxdx = (pStep)->ay2pxdx; 
  
  int16 count   = (eInfo)->count; 
  boolean q13   = (eInfo)->q13; 
  int16 px      = (eInfo)->p.x; 
  int16 py      = (eInfo)->p.y; 
  
  r2 = r1; 
  ON_ADD_OVERFLOW_THROW(int64, exception, ax2pydy, -ay2pxdx); 
  tmp = ax2pydy - ay2pxdx; 
  ON_ADD_OVERFLOW_THROW(int64, exception, r2, tmp); 
  r2 += tmp; 
  
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ax2, 1); 
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ay2, 1); 
  ON_ADD_OVERFLOW_THROW(int64, exception, ax2<<1, -(ay2<<1)) 
  tmp = ((ax2<<1) - (ay2<<1)); 
  
  ON_ADD_OVERFLOW_THROW(int64, exception, r2, tmp); 
  r2 += tmp; 
  
  ON_ADD_OVERFLOW_THROW(int64, exception, ax2, -ay2); 
  tmp = ax2 - ay2; 
  
  ON_ADD_OVERFLOW_THROW(int64, exception, r2, tmp); 
  r2 += tmp; 
  
  r3 = r2; 
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ax2pydy, 1) 
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ax2, 3) 
  ON_ADD_OVERFLOW_THROW(int64, exception, ax2pydy<<1, ax2<<3) 
  tmp = (ax2pydy<<1) + (ax2<<3); 
  
  ON_ADD_OVERFLOW_THROW(int64, exception, r3, -tmp); 
  r3 = r3 - tmp; 
  
  r4 = r1;
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ay2pxdx, 1); 
  ON_LSHIFT_OVERFLOW_THROW(int64, exception, ay2, 3); 
  ON_ADD_OVERFLOW_THROW(int64, exception, ay2pxdx<<1, ay2<<3); 
  tmp = (ay2pxdx<<1) + (ay2<<3);
  
  ON_ADD_OVERFLOW_THROW(int64, exception, r4, -tmp); 
  r4 = r4 - tmp;

  /* Step in X? */
  if ((q13 ^ (r1 > 0)) || (q13 ^ (r3 > 0))) {
    ON_ADD_OVERFLOW_THROW(int64, exception, px, (eInfo)->d.x) 
    px += (eInfo)->d.x;

    tmp = (ay2pxdx<<1)+(ay2<<3);
    ON_ADD_OVERFLOW_THROW(int64, exception, r1, -tmp); 
    r1 -= tmp;

    ON_ADD_OVERFLOW_THROW(int64, exception, ay2pxdx, ay2<<2); 
    ay2pxdx += ay2<<2;

    if (q13) count -= 2;
  }
  
  /* Step in Y? */
  if ((!q13 ^ (r2 > 0)) || (!q13 ^ (r4 > 0))) {
    ON_ADD_OVERFLOW_THROW(int64, exception, py, (eInfo)->d.y); 
    py += (eInfo)->d.y;
    
    ON_ADD_OVERFLOW_THROW(int64, exception, ax2, ax2); 
    tmp = ax2 + ax2;

    ON_ADD_OVERFLOW_THROW(int64, exception, tmp, ax2); 
    tmp += ax2;

    ON_LSHIFT_OVERFLOW_THROW(int64, exception, tmp, 2); 
    ON_ADD_OVERFLOW_THROW(int64, exception, ax2pydy<<1, tmp<<2); 
    tmp = (ax2pydy<<1) + (tmp<<2);

    ON_ADD_OVERFLOW_THROW(int64, exception, r1, -tmp); 
    r1 -= tmp;

    ON_ADD_OVERFLOW_THROW(int64, exception, ax2pydy, ax2<<2); 
    ax2pydy += ax2<<2;

    if (!q13) count -= 2;
  }
  /*its very important that (eInfo) is updated after all the
  overflow checks, otherwise the algorithm cant recover by
  using a larger type*/ 
  (pStep)->r1 = r1;
  (pStep)->ax2pydy = ax2pydy;
  (pStep)->ay2pxdx = ay2pxdx;

  (eInfo)->p.y = py;
  (eInfo)->p.x = px;
  (eInfo)->count = count;
  (eInfo)->reg.last_x  = (eInfo)->reg.x;
  (eInfo)->reg.last_xi = (eInfo)->reg.xi;
  (eInfo)->mir.last_x  = (eInfo)->mir.x;
  (eInfo)->mir.last_xi = (eInfo)->mir.xi;
  (eInfo)->reg.last_y  = (eInfo)->reg.y;
  (eInfo)->mir.last_y  = (eInfo)->mir.y;

  if( q13 ) {
     /*TODO: verify that casting is appropriate*/
     (eInfo)->reg.x = (int16)(((pStep)->xdx_offset + (eInfo)->p.x) >> 1);
     (eInfo)->mir.x = (int16)(((pStep)->xdx - (eInfo)->p.x) >> 1);
  }
  else {
     /*TODO: verify that casting is appropriate*/
     (eInfo)->reg.x = (int16)(((pStep)->xdx + (eInfo)->p.x) >> 1);
     (eInfo)->mir.x = (int16)(((pStep)->xdx_offset - (eInfo)->p.x) >> 1);
  }

  (eInfo)->reg.xi = (eInfo)->reg.x - 1;
  (eInfo)->mir.xi = (eInfo)->mir.x + 1;

  /*TODO: verify that casting is appropriate*/
  (eInfo)->reg.y = (int16)(((pStep)->ydy_offset + (eInfo)->p.y) >> 1);
  (eInfo)->mir.y = (int16)(((pStep)->ydy - (eInfo)->p.y) >> 1);
  CATCH(exception);
  return exception;
}

/*===========================================================================
  Helper Function: ELLIPSE_ARC_STEP()

  Step ellipse until y changes or done
===========================================================================*/
static int ellipse_arc_step_int32(
      AEEStepInfo32* pStep, 
      AEEEllipseInfo *eInfo, 
      AEEEllipseArcInfo *arcInfo 
      ) {
   arcInfo->y = eInfo->reg.y;
   arcInfo->ym = eInfo->mir.y;

   arcInfo->right_x = eInfo->reg.x;
   arcInfo->left_xm = eInfo->mir.x;

   while( eInfo->count >= 2 && arcInfo->y == eInfo->reg.y) {
      ellipse_step_int32((pStep), eInfo);
   }

   arcInfo->left_x = eInfo->reg.last_x;
   arcInfo->right_xm = eInfo->mir.last_x;
   return SUCCESS;
}

static int ellipse_arc_step_int64(
      AEEStepInfo64* pStep, 
      AEEEllipseInfo *eInfo, 
      AEEEllipseArcInfo *arcInfo 
      ) {
   int exception = SUCCESS;
   arcInfo->y = eInfo->reg.y;
   arcInfo->ym = eInfo->mir.y;

   arcInfo->right_x = eInfo->reg.x;
   arcInfo->left_xm = eInfo->mir.x;

   while( eInfo->count >= 2 && arcInfo->y == eInfo->reg.y) {
      TRY(exception, ellipse_step_int64((pStep), eInfo));
   }

   arcInfo->left_x = eInfo->reg.last_x;
   arcInfo->right_xm = eInfo->mir.last_x;
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipseLine()

  Handle special case of very thin ellipses - either width or height less
  than 3 pixels.
===========================================================================*/
static void drawEllipseLine( QGraphics *pMe, AEERect *pRect,
                             boolean drawFrame, boolean fillInterior,
                             boolean clip,
                             NativeColor frameColor,
                             NativeColor interiorColor,
                             AEERasterOp ro )
{
   int16       xClipMin;   
   int16       yClipMin;
   int16       xClipMax;
   int16       yClipMax;
   int16       i;
   NativeColor color;

   xClipMin = pRect->x;
   xClipMax = xClipMin + pRect->dx - 1;

   yClipMin = pRect->y;
   yClipMax = yClipMin + pRect->dy;
    
   if (drawFrame) {
      color = frameColor;
   } else {
      color = interiorColor;
   }
  
   if (drawFrame || fillInterior) {
      for (i = yClipMin; i < yClipMax; i++) {
         drawHScanline(pMe, i, xClipMin, xClipMax, color, ro);
      }
   }
  
   if (clip) {
      if (drawFrame) {
         xClipMin = 1;
         xClipMax = 0;
      }
      for (i = yClipMin; i < yClipMax; i++) {      
         updateInterval( pMe, i, xClipMin, xClipMax );
      }
   }
}

/*===========================================================================
  Helper Function: drawEllipseFillAndClip()
===========================================================================*/
static void drawEllipseFillAndClip( QGraphics *pMe,
                                    int16 y1, int16 y2, int16 xm, int16 x,
                                    boolean fillInterior, boolean clip,
                                    NativeColor interiorColor,
                                    AEERasterOp ro )
{
   if( fillInterior )
      drawHScanline(pMe, y1, xm, x, interiorColor, ro);

   if( clip )
      updateInterval( pMe, y1, xm, x );

   if( y1 != y2 ) {
      if( fillInterior )
         drawHScanline(pMe, y2, xm, x, interiorColor, ro);

      if( clip )
         updateInterval( pMe, y2, xm, x );
   }
}

/*===========================================================================
  Helper Function: drawEllipsePoints()
===========================================================================*/
static int drawEllipsePoints( QGraphics *pMe, boolean bDotted, int iDotPhase,
                               int16 x1, int16 y1,
                               int16 x2, int16 y2,
                               NativeColor color, AEERasterOp ro )
{
   IBitmap *pbm = pMe->m_pbmDst;

   if( ! bDotted || (bDotted && DOT_PHASE_ON(iDotPhase)) ) {
      if( valid( pMe, x1, y1 ) )
         IBITMAP_DrawPixel(pbm, x1, y1, color, ro);
      if( valid( pMe, x2, y2 ) )
         IBITMAP_DrawPixel(pbm, x2, y2, color, ro);
   }

   return DOT_PHASE_INC(iDotPhase);
}

/*===========================================================================
  Helper Function: drawEllipseWithFrame()

  Draws an ellipse (or round rectangle) with a Frame:
      the frame may or may not be dotted;
      the ellipse may or may not be filled; and
      the clipping region may or may not be set.
  If clipping region is set, it is set to the interior of the ellipse.
===========================================================================*/
static int drawEllipseWithFrame( QGraphics *pMe, AEERect *pRect,
                                  boolean fillInterior, boolean clip,
                                  NativeColor frameColor,
                                  NativeColor interiorColor,
                                  AEERasterOp ro,
                                  int16 offset_x, int16 offset_y )
{
   int exception = SUCCESS;
   int16 quadrant;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   boolean bDotted = pMe->m_strokeStyle == STROKE_DOTTED;
   int iDotPhase = 0;
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   for (quadrant = 1; quadrant <= 2; quadrant++) {
      if(FALSE == need64) {
         setup_ellipse_quadrant_int32(pRect, quadrant, offset_x, offset_y, &eInfo, &oStep32);
      } else {
         TRY(exception, setup_ellipse_quadrant_int64(pRect, quadrant, offset_x, offset_y, &eInfo, &oStep64));
      }

      if( quadrant == 1 && offset_y != 0) {
         int16 y;

         if( eInfo.mir.y + offset_y == eInfo.reg.y )
            iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                           eInfo.mir.x, eInfo.reg.y,
                                           eInfo.reg.x, eInfo.mir.y,
                                           frameColor, ro );
         else
            iDotPhase = DOT_PHASE_INC(iDotPhase);

         for( y = eInfo.mir.y + 1; y < eInfo.reg.y; y++ ) {
            iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                           eInfo.mir.x, y,
                                           eInfo.reg.x, y,
                                           frameColor, ro );
            drawEllipseFillAndClip( pMe, y, y, eInfo.mir.xi, eInfo.reg.xi,
                                    fillInterior, clip,
                                    interiorColor, ro );
         }
      }

      while (eInfo.count >= 2) {
         iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                        eInfo.reg.x, eInfo.reg.y,
                                        eInfo.mir.x, eInfo.mir.y,
                                        frameColor, ro );

         // We draw a fill line and set clip interval only if we have done
         // a Y step. Towards the bottom of the ellipse, we might make
         // several X steps per scanline.
         if( eInfo.reg.last_y != eInfo.reg.y && (fillInterior || clip)) {
            if( quadrant == 1 ) {
               drawEllipseFillAndClip( pMe, eInfo.reg.last_y, eInfo.mir.last_y,
                                       eInfo.mir.last_xi, eInfo.reg.last_xi,
                                       fillInterior, clip,
                                       interiorColor, ro );
            }
            else if( eInfo.mir.x == eInfo.reg.x && clip ) {
               updateInterval( pMe, eInfo.reg.y, 1, 0 );
               updateInterval( pMe, eInfo.mir.y, 1, 0 );
            }
         }
         if(FALSE == need64) {
            //should always succeed
            ellipse_step_int32(&oStep32, &eInfo);
         } else {
            TRY(exception, ellipse_step_int64(&oStep64, &eInfo));
         }
      }

      // We draw a fill line and set clip interval only if we have done
      // a Y step. Towards the bottom of the ellipse, we might make
      // several X steps per scanline.
      if( eInfo.reg.last_y != eInfo.reg.y && (fillInterior || clip)) {
         if( quadrant == 1 ) {
            drawEllipseFillAndClip( pMe, eInfo.reg.last_y, eInfo.mir.last_y,
                                    eInfo.mir.last_xi, eInfo.reg.last_xi,
                                    fillInterior, clip,
                                    interiorColor, ro );
         }
         else if( eInfo.mir.x == eInfo.reg.x && clip ) {
            updateInterval( pMe, eInfo.reg.y, 1, 0 );
            updateInterval( pMe, eInfo.mir.y, 1, 0 );
         }
      }

      /* Fill in the pixels near the axis */
      if (eInfo.q13) {
         int16 i;

         if (eInfo.p.x != 0) {
            for (;;) {
               iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                              eInfo.reg.x, eInfo.reg.y,
                                              eInfo.mir.x, eInfo.mir.y,
                                              frameColor, ro );

               if (eInfo.a.y == ABS(eInfo.p.y)) break;

               if( eInfo.reg.x == eInfo.mir.x ) {
                  // No interior, ellipse is single pixel wide
                  if( clip ) {
                     updateInterval( pMe, eInfo.reg.y, 1, 0);
                     updateInterval( pMe, eInfo.mir.y, 1, 0);
                  }
               }
               else {
                  drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                          eInfo.mir.xi, eInfo.reg.xi,
                                          fillInterior, clip,
                                          interiorColor, ro );
               }                                          

               eInfo.reg.last_y = eInfo.reg.y;
               eInfo.p.y += eInfo.d.y;
               eInfo.reg.y  += eInfo.d.y >> 1;
               eInfo.mir.y -= eInfo.d.y >> 1;
            }

            if( clip ) {
               updateInterval( pMe, eInfo.reg.y, 1, 0);
               updateInterval( pMe, eInfo.mir.y, 1, 0);
            }

            eInfo.reg.x--;
            eInfo.mir.x++;
         }

         for( i = 0; i < offset_x; i++ ) {
            iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                           eInfo.reg.x, eInfo.reg.y,
                                           eInfo.mir.x, eInfo.mir.y,
                                           frameColor, ro );
            eInfo.reg.x--;
            eInfo.mir.x++;
         }
      }
      else {
         if (eInfo.p.y != 0) {
            for (;;) {
               iDotPhase = drawEllipsePoints( pMe, bDotted, iDotPhase,
                                              eInfo.reg.x, eInfo.reg.y,
                                              eInfo.mir.x, eInfo.mir.y,
                                              frameColor, ro );
               if (eInfo.a.x == ABS(eInfo.p.x)) break;
               eInfo.p.x += eInfo.d.x;
               eInfo.reg.x  += eInfo.d.x >> 1;
               eInfo.mir.x -= eInfo.d.x >> 1;
            }
         }
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipseWithoutFrame()

  Draws an ellipse (or round rectangle) without a frame:
      the ellipse may or may not be filled; and
      the clipping region may or may not be set.
  If the ellipse is filled, the frame and the interior are drawn together.
  If clipping region is set, it is set to the frame of the ellipse.
===========================================================================*/
static int drawEllipseWithoutFrame( QGraphics *pMe, AEERect *pRect,
                                     boolean fillInterior, boolean clip,
                                     NativeColor interiorColor,
                                     AEERasterOp ro,
                                     int16 offset_x, int16 offset_y )
{
   int exception = SUCCESS;
   int16 last_q1_y = -1;

   int16 quadrant;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   for (quadrant = 1; quadrant <= 2; quadrant++) {
      if(FALSE == need64) {
         setup_ellipse_quadrant_int32(pRect, quadrant, offset_x, offset_y, &eInfo, &oStep32);
      } else {
         TRY(exception, setup_ellipse_quadrant_int64(pRect, quadrant, offset_x, offset_y, &eInfo, &oStep64));
      }


      if( quadrant == 1 ) {
         int16 y;

         for( y = eInfo.mir.y; y <= eInfo.reg.y; y++ ) {
            drawEllipseFillAndClip( pMe, y, y, eInfo.mir.x, eInfo.reg.x,
                                    fillInterior, clip,
                                    interiorColor, ro );
         }
         last_q1_y = eInfo.reg.y;
      }
      else if( eInfo.mir.x == eInfo.reg.x && eInfo.reg.y != last_q1_y ) {
         drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                 eInfo.mir.x, eInfo.reg.x,
                                 fillInterior, clip,
                                 interiorColor, ro );
      }

      while (eInfo.count >= 2) {
         // We draw a fill line and set clip interval only if we have done
         // a Y step. Towards the bottom of the ellipse, we might make
         // several X steps per scanline.
         if( eInfo.reg.last_y != eInfo.reg.y ) {
            if( quadrant == 1 ) {
               drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                       eInfo.mir.x, eInfo.reg.x,
                                       fillInterior, clip,
                                       interiorColor, ro );
               last_q1_y = eInfo.reg.y;
            }
            else if( eInfo.mir.x == eInfo.reg.x ) {
               drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                       eInfo.mir.x, eInfo.reg.x,
                                       fillInterior, clip,
                                       interiorColor, ro );
            }
         }

         if(FALSE == need64) {
            //should always succeed
            ellipse_step_int32(&oStep32, &eInfo);
         } else {
            TRY(exception, ellipse_step_int64(&oStep64, &eInfo));
         }
      }

      // We draw a fill line and set clip interval only if we have done
      // a Y step. Towards the bottom of the ellipse, we might make
      // several X steps per scanline.
      if( eInfo.reg.last_y != eInfo.reg.y && (fillInterior || clip)) {
         if( quadrant == 1 ) {
            if( eInfo.mir.x != eInfo.reg.x ) {
               drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                       eInfo.mir.x, eInfo.reg.x,
                                       fillInterior, clip,
                                       interiorColor, ro );
            last_q1_y = eInfo.reg.y;
            }
         }
         else if( eInfo.mir.x == eInfo.reg.x ) {
            drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                    eInfo.mir.x, eInfo.reg.x,
                                    fillInterior, clip,
                                    interiorColor, ro );
         }
      }

      /* Fill in the pixels near the axis */
      if (eInfo.q13) {
         if (eInfo.p.x != 0) {
            for (;;) {
               if (eInfo.a.y == ABS(eInfo.p.y)) break;
               eInfo.reg.last_y = eInfo.reg.y;
               eInfo.p.y += eInfo.d.y;
               eInfo.reg.y  += eInfo.d.y >> 1;
               eInfo.mir.y -= eInfo.d.y >> 1;
               if( quadrant == 1 ) {
                  drawEllipseFillAndClip( pMe, eInfo.reg.y, eInfo.mir.y,
                                          eInfo.mir.x, eInfo.reg.x,
                                          fillInterior, clip,
                                          interiorColor, ro );
                  last_q1_y = eInfo.reg.y;
               }
            }
         }
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipse()

  Main helper function for drawing ellipses - looks at the flags and ellipse
  size and calls the appropriate draw routine.
===========================================================================*/
static int drawEllipse(QGraphics *pMe, AEERect *pRect, uint8 nFlag)
{
   int exception = SUCCESS;
   NativeColor frameColor    = pMe->m_fg;
   NativeColor interiorColor = pMe->m_fl;
   boolean drawFrame    = (nFlag & AEE_GRAPHICS_FRAME) != 0;
   boolean fillInterior = (nFlag & AEE_GRAPHICS_FILL) != 0;
   boolean clip         = (nFlag & AEE_GRAPHICS_CLIP) != 0;
   AEERasterOp ro = pMe->m_ro;

   if( nFlag & AEE_GRAPHICS_CLEAR ) {
      interiorColor = pMe->m_bg;
      fillInterior = TRUE;
      ro = AEE_RO_COPY;
   }

   if( pRect->dx <= 2 || pRect->dy <= 2 ) {
      drawEllipseLine( pMe, pRect, drawFrame, fillInterior, clip,
                       frameColor, interiorColor, ro );
      return exception;
   }

   pRect->dx--;
   pRect->dy--;

   if( drawFrame ) {
      TRY(exception, drawEllipseWithFrame( pMe, pRect, fillInterior, clip,
                            frameColor, interiorColor, ro, 0, 0 ));
   }
   else {
      TRY(exception, drawEllipseWithoutFrame( pMe, pRect, fillInterior, clip,
                               interiorColor, ro, 0, 0 ));
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcLine()

  Handle special case of very thin elliptical arcs - either width or height
  less than 3 pixels.
===========================================================================*/
static void drawEllipticalArcLine( QGraphics *pMe, AEERect *pRect,
                                   boolean drawFrame, boolean fillInterior,
                                   boolean clip,
                                   NativeColor frameColor,
                                   NativeColor interiorColor,
                                   AEERasterOp ro,
                                   int16 startAngle, int16 endAngle )
{
   AEERect actRect;

   actRect = *pRect;

   // Handle special caes of ellipse that is less than three pixels wide
   if( actRect.dx <= 2 ) {
      // vertical ellipse
      if( startAngle < 180 && endAngle <= 180 && endAngle > startAngle )
         actRect.dy = actRect.dy/2; // just the top half

      if( startAngle >= 180 && (endAngle > startAngle || endAngle == 0) ) {
         // just the bottom half
         actRect.dy = actRect.dy/2;
         actRect.y += actRect.dy;
      }         
   }
   else {
      // horizontal ellipse
      if( (endAngle <= 90 && (startAngle >= 270 || startAngle < endAngle)) ||
          (startAngle >= 270 && endAngle > startAngle) ) {
         // just the right half
         actRect.dx = actRect.dx/2;
         actRect.x += actRect.dx;
      }

      if( (startAngle >= 90 && endAngle <= 270 && endAngle > startAngle ) )
         actRect.dx = actRect.dx/2; // just the left half
   }

   drawEllipseLine( pMe, &actRect, drawFrame, fillInterior, clip,
                    frameColor, interiorColor, ro );
}

/*===========================================================================
  Helper Function: drawEllipticalArcRow()

  Draws a row of an elliptical arc region:
      may or may not draw the frame; and
      may or may not fill the interior; and 
      may or may not have a dotted frame.

  left_xm and right_xm are the x values for the left side of the region.
  left_x and right_x are the x values for the right side of the region.

  If the right side is entirely to the left of the left side, nothing is drawn.

===========================================================================*/
static void drawEllipticalArcRow( QGraphics *pMe, int16 line,
                                   int16 left_xm, int16 right_xm,
                                   int16 left_x,  int16 right_x,
                                   boolean drawFrame, boolean fillInterior,
                                   NativeColor frameColor,
                                   NativeColor interiorColor,
                                   AEERasterOp ro, boolean bDotted, 
                                   int * piArcDotPhase, int * piPieDotPhase)
{
   if( drawFrame ) {
      if( right_xm < left_x ) {
         // there is a gap between the left and right sides of the ellipse
         drawHScanline_Dotted(pMe, line, left_xm, right_xm, frameColor, ro,
                              bDotted, piArcDotPhase);
         drawHScanline_Dotted(pMe, line, left_x, right_x, frameColor, ro,
                              bDotted, piPieDotPhase);

         if( fillInterior )
            drawHScanline( pMe, line, (int16)(right_xm+1), (int16)(left_x-1),
                           interiorColor, ro );
      }
      else if( left_xm <= right_x ) {
         // there is some frame left to draw
         drawHScanline_Dotted(pMe, line, left_xm, right_x, frameColor, ro,
                              bDotted, piArcDotPhase);
      }
   }
   else {
      // the whole thing is interior - draw as much as possible
      if( fillInterior )
         drawHScanline( pMe, line, left_xm, right_x, interiorColor, ro );
   }
}

/*===========================================================================
  Helper Function: drawEllipticalArcDoubleRow()

  Draws a row of an elliptical arc with two regions with a common edge:
      may or may not draw the frame; and
      may or may not fill the interior.
      may or may not have a dotted frame.

  left_xm and right_xm are the x values for the left side of the region.
  left_xmid and right_xmid are the x values for the common (middle) edge
  left_x and right_x are the x values for the right side of the region.
===========================================================================*/
static void drawEllipticalArcDoubleRow( QGraphics *pMe, int16 line,
                                        int16 left_xm, int16 right_xm,
                                        int16 left_xmid, int16 right_xmid,
                                        int16 left_x,  int16 right_x,
                                        boolean drawFrame,
                                        boolean fillInterior,
                                        NativeColor frameColor,
                                        NativeColor interiorColor,
                                        AEERasterOp ro, boolean bDotted,
                                        int * piArcDotPhase, int * piPieDotPhase)
{
   if( drawFrame ) {
      if( right_xm < left_xmid ) {
         // There's a gap at the left, so draw left side and fill
         drawHScanline_Dotted(pMe, line, left_xm, right_xm, frameColor, ro,
                              bDotted, piArcDotPhase);
         if( fillInterior )
            drawHScanline( pMe, line, (int16)(right_xm+1), (int16)(left_xmid-1),
                           interiorColor, ro );
      }
      else {
         // There's no gap at the left, so start drawing at the left edge
         left_xmid = left_xm;
         right_xmid = MAX( right_xm, right_xmid );
      }

      if( right_xmid < left_x ) {
         // There's a gap at the right, so draw right side and fill
         drawHScanline_Dotted(pMe, line, left_x, right_x, frameColor, ro,
                              bDotted, piArcDotPhase);
         if( fillInterior )
            drawHScanline( pMe, line, (int16)(right_xmid+1), (int16)(left_x-1),
                           interiorColor, ro );
      }
      else {
         // There's no gap at the right, so end drawing at the right edge
         left_xmid = MIN( left_x, left_xmid );
         right_xmid = right_x;
      }

      // Draw middle segment
      drawHScanline_Dotted(pMe, line, left_xmid, right_xmid, frameColor, ro,
                           bDotted, piPieDotPhase);
   }
   else {
      if( fillInterior )
         drawHScanline( pMe, line, left_xm, right_x, interiorColor, ro );
   }
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase1()

  startAngle < 180 && endAngle < startAngle
===========================================================================*/
static int drawEllipticalArcCase1( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));
   
   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }
   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.y != eInfo.reg.y ) {
         if( arcInfo.y != arcInfo.ym ) {
            // Draw bottom half
            drawEllipticalArcRow( pMe, arcInfo.y,
                                  arcInfo.left_xm, arcInfo.right_xm,
                                  arcInfo.left_x, arcInfo.right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }

         if( drawArc ) {
            // Draw top left
            if(arcInfo.left_xm <= lStart.right_x) {
               right_x = MIN( arcInfo.right_xm, lStart.right_x ); 
               drawHScanline_Dotted(pMe, arcInfo.ym, arcInfo.left_xm, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            else if(arcInfo.right_xm >= lEnd.left_x) {
               left_x = MAX(lEnd.left_x, arcInfo.left_xm);
               drawHScanline_Dotted(pMe, arcInfo.ym, left_x, arcInfo.right_xm,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            // Draw top right
            if( arcInfo.right_x >= lEnd.left_x ) {
               left_x = MAX( arcInfo.left_x, lEnd.left_x);
               drawHScanline_Dotted(pMe, arcInfo.ym, left_x, arcInfo.right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            else if( arcInfo.left_x <= lStart.right_x ) {
               right_x = MIN( arcInfo.right_x, lStart.right_x);
               drawHScanline_Dotted(pMe, arcInfo.ym, arcInfo.left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            if( lEnd.left_x <= lStart.right_x &&
                lEnd.left_x <= arcInfo.right_x &&
                lStart.right_x >= arcInfo.left_xm ) {
               // Portions of the lines overlap, merge into a single segment
               left_x = MIN( lEnd.left_x, lStart.left_x );
               right_x = MAX( lEnd.right_x, lStart.right_x );
               drawEllipticalArcDoubleRow( pMe, arcInfo.ym,
                                           arcInfo.left_xm, arcInfo.right_xm,
                                           left_x, right_x,
                                           arcInfo.left_x, arcInfo.right_x,
                                           drawFrame, fillInterior,
                                           frameColor, interiorColor, ro,
                                           bDotted, &iArcDotPhase, &iPieDotPhase );
            }
            else {
               // Lines do not overlap, draw individually
               // Draw top left
               left_x = MIN( lStart.left_x, arcInfo.left_x );
               right_x =  MIN( lStart.right_x, arcInfo.right_x );
               drawEllipticalArcRow( pMe, arcInfo.ym,
                                     arcInfo.left_xm, arcInfo.right_xm,
                                     left_x, right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
               // Draw top right
               left_x = MAX( lEnd.left_x, arcInfo.left_xm );
               right_x = MAX( lEnd.right_x, arcInfo.right_xm );
               drawEllipticalArcRow( pMe, arcInfo.ym,
                                     left_x, right_x,
                                     arcInfo.left_x, arcInfo.right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      right_x = MIN( lStart.right_x, arcInfo.right_x );
      left_x = MAX( lEnd.left_x, arcInfo.left_xm );

      if( drawArc ) {
         if( arcInfo.left_xm <= right_x )
            drawHScanline_Dotted(pMe, arcInfo.ym, arcInfo.left_xm, right_x,
                                 frameColor, ro, bDotted, &iArcDotPhase);
         if( left_x <= arcInfo.right_x )
            drawHScanline_Dotted(pMe, arcInfo.ym, left_x, arcInfo.right_x,
                                 frameColor, ro, bDotted, &iArcDotPhase);
      }
      else {
         if( arcInfo.left_xm <= right_x ) {
            drawEllipticalArcRow( pMe, arcInfo.ym,
                                  arcInfo.left_xm, right_x,
                                  arcInfo.left_xm, right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }
         if( left_x <= arcInfo.right_x ) {
            drawEllipticalArcRow( pMe, arcInfo.ym,
                                  left_x, arcInfo.right_x,
                                  left_x, arcInfo.right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }
      }

      if( arcInfo.y != arcInfo.ym ) {
         // Draw bottom half
         drawEllipticalArcRow( pMe, arcInfo.y,
                               arcInfo.left_xm, arcInfo.right_x,
                               arcInfo.left_xm, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }
      // Draw bottom half
      drawEllipticalArcRow( pMe, arcInfo.y,
                            arcInfo.left_xm, arcInfo.right_xm,
                            arcInfo.left_x, arcInfo.right_x,
                            drawFrame, fillInterior,
                            frameColor, interiorColor, ro,
                            bDotted, &iArcDotPhase, &iPieDotPhase );
      if( arcInfo.left_xm <= lStart.right_x ) {
         right_x = MIN( arcInfo.right_x, lStart.right_x);
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
      else if( arcInfo.right_x >= lEnd.left_x ) {
         left_x = MAX( arcInfo.left_xm, lEnd.left_x );
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase2()

  startAngle < 180 && startAngle < endAngle <= 180
===========================================================================*/
static int drawEllipticalArcCase2( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }
      if( arcInfo.y != eInfo.reg.y ) {
         if( drawArc ) {
            if( arcInfo.right_xm >= lEnd.left_x &&
                arcInfo.left_xm <= lStart.right_x ) {
               left_x = MAX( arcInfo.left_xm, lEnd.left_x );
               right_x = MIN( arcInfo.right_xm, lStart.right_x );
               drawHScanline_Dotted(pMe, arcInfo.ym, left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            if( arcInfo.left_x <= lStart.right_x &&
                arcInfo.right_x >= lEnd.left_x ) {
               left_x = MAX( arcInfo.left_x, lEnd.left_x );
               right_x = MIN( arcInfo.right_x, lStart.right_x );
               drawHScanline_Dotted(pMe, arcInfo.ym, left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            drawEllipticalArcRow( pMe, arcInfo.ym,
                                  (int16) MAX( arcInfo.left_xm, lEnd.left_x ),
                                  (int16) MAX( arcInfo.right_xm, lEnd.right_x ),
                                  (int16) MIN( lStart.left_x, arcInfo.left_x ),
                                  (int16) MIN( lStart.right_x, arcInfo.right_x ),
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      arcInfo.left_x = MAX( arcInfo.left_xm, lEnd.left_x );
      arcInfo.right_x = MIN( lStart.right_x, arcInfo.right_x );
      if( arcInfo.left_x <= arcInfo.right_x ) 
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               arcInfo.left_x, arcInfo.right_x,
                               arcInfo.left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }
      if( arcInfo.left_xm <= lStart.right_x &&
          arcInfo.right_x >= lEnd.left_x ) {
         left_x  = MAX( arcInfo.left_xm, lEnd.left_x );
         right_x = MIN( arcInfo.right_x, lStart.right_x);
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               left_x, right_x,
                               left_x, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase3()

  startAngle < 180 && endAngle > 180
===========================================================================*/
static int drawEllipticalArcCase3( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.y != eInfo.reg.y ) {
         if( drawArc ) {
            if( arcInfo.y != arcInfo.ym) {
               if(arcInfo.left_xm <= lStart.right_x) {
                  right_x = MIN( arcInfo.right_xm, lStart.right_x );
                  drawHScanline_Dotted(pMe, arcInfo.ym, arcInfo.left_xm, right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
               if(arcInfo.left_x <= lStart.right_x) {
                  right_x = MIN( arcInfo.right_x, lStart.right_x );
                  drawHScanline_Dotted(pMe, arcInfo.ym, arcInfo.left_x, right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }

               if( arcInfo.left_xm <= lEnd.right_x ) {
                  right_x = MIN( arcInfo.right_xm, lEnd.right_x );
                  drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_xm, right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
               if( arcInfo.left_x <= lEnd.right_x ) {
                  right_x = MIN( arcInfo.right_x, lEnd.right_x );
                  drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_x, right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
            }
            else {
               if(arcInfo.left_x <= lStart.right_x) {
                  right_x = MIN( arcInfo.right_x, lStart.right_x );
                  drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_x, right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }

               drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_xm, arcInfo.right_xm,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            if( arcInfo.y != arcInfo.ym ) {
               // Draw top half
               drawEllipticalArcRow( pMe, arcInfo.ym,
                                     arcInfo.left_xm, arcInfo.right_xm,
                                     (int16) MIN( arcInfo.left_x, lStart.left_x ),
                                     (int16) MIN( arcInfo.right_x, lStart.right_x ),
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
               // Draw bottom half
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     arcInfo.left_xm, arcInfo.right_xm,
                                     (int16) MIN( arcInfo.left_x, lEnd.left_x ),
                                     (int16) MIN( arcInfo.right_x, lEnd.right_x ),
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
            else {
               // The portions of the lines are co-linear, and have the
               // center point in common, so just draw one segment which
               // combines both lines
               left_x  = MIN( lEnd.left_x, lStart.left_x );
               right_x = MAX( lEnd.right_x, lStart.right_x );
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     arcInfo.left_xm, arcInfo.right_xm,
                                     (int16) MIN( arcInfo.left_x, left_x ),
                                     (int16) MIN( arcInfo.right_x, right_x ),
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      right_x = MIN( arcInfo.right_x, lStart.right_x );
      if( arcInfo.left_xm <= right_x ) {
         // Draw top half
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }

      right_x = MIN( arcInfo.right_x, lEnd.right_x );
      if( arcInfo.left_xm <= right_x ) {
         // Draw bottom half
         drawEllipticalArcRow( pMe, arcInfo.y,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.left_xm <= lStart.right_x ) {
         right_x = MIN( arcInfo.right_x, lStart.right_x);
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
      if( arcInfo.left_xm <= lEnd.right_x ) {
         right_x = MIN( arcInfo.right_x, lEnd.right_x );
         drawEllipticalArcRow( pMe, arcInfo.y,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase4()

  startAngle >= 180 && endAngle < 180
===========================================================================*/
static int drawEllipticalArcCase4( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.y != eInfo.reg.y ) {
         if( drawArc ) {
            if( arcInfo.y != arcInfo.ym ) {
               if( arcInfo.right_x >= lEnd.left_x ) {
                  left_x = MAX( arcInfo.left_x, lEnd.left_x);
                  drawHScanline_Dotted(pMe, arcInfo.ym, left_x, arcInfo.right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
               if( arcInfo.right_xm >= lEnd.left_x ) {
                  left_x = MAX( arcInfo.left_xm, lEnd.left_x);
                  drawHScanline_Dotted(pMe, arcInfo.ym, left_x, arcInfo.right_xm,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
               if( arcInfo.right_x >= lStart.left_x ) {
                  left_x = MAX( arcInfo.left_x, lStart.left_x);
                  drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_x,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
               if( arcInfo.right_xm >= lStart.left_x ) {
                  left_x = MAX( arcInfo.left_xm, lStart.left_x);
                  drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_xm,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }
            }
            else {
               if( arcInfo.right_xm >= lEnd.left_x ) {
                  left_x = MAX( arcInfo.left_xm, lEnd.left_x);
                  drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_xm,
                                       frameColor, ro, bDotted, &iArcDotPhase);
               }

               drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_x, arcInfo.right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            if( arcInfo.y != arcInfo.ym ) {
               // Draw top half
               drawEllipticalArcRow( pMe, arcInfo.ym,
                                     (int16) MAX( arcInfo.left_xm, lEnd.left_x ),
                                     (int16) MAX( arcInfo.right_xm, lEnd.right_x ),
                                     arcInfo.left_x, arcInfo.right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
               // Draw bottom half
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     (int16) MAX( arcInfo.left_xm, lStart.left_x ),
                                     (int16) MAX( arcInfo.right_xm, lStart.right_x ),
                                     arcInfo.left_x, arcInfo.right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
            else {
               // The portions of the lines are co-linear, and have the
               // center point in common, so just draw one segment which
               // combines both lines
               left_x  = MIN( lEnd.left_x, lStart.left_x );
               right_x = MAX( lEnd.right_x, lStart.right_x );
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     (int16) MIN( arcInfo.left_x, left_x ),
                                     (int16) MIN( arcInfo.right_x, right_x ),
                                     arcInfo.left_x, arcInfo.right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      left_x = MAX( arcInfo.left_xm, lEnd.left_x );
      if( left_x <= arcInfo.right_x ) {
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }

      left_x = MAX( arcInfo.left_xm, lStart.left_x );
      if( left_x <= arcInfo.right_x ) {
         drawEllipticalArcRow( pMe, arcInfo.y,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.right_x >= lStart.left_x ) {
         left_x = MAX( arcInfo.left_xm, lStart.left_x);
         drawEllipticalArcRow( pMe, arcInfo.y,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
      if( arcInfo.right_x >= lEnd.left_x ) {
         left_x = MAX( arcInfo.left_xm, lEnd.left_x );
         drawEllipticalArcRow( pMe, arcInfo.ym,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase5()

  startAngle >= 180 && 180 <= endAngle < startAngle
===========================================================================*/
static int drawEllipticalArcCase5( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.y != eInfo.reg.y ) {
         // Draw top half
         if( arcInfo.y != arcInfo.ym ) {
            drawEllipticalArcRow( pMe, arcInfo.ym,
                                  arcInfo.left_xm, arcInfo.right_xm,
                                  arcInfo.left_x, arcInfo.right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }

         if( drawArc ) {
            // Draw Bottom left
            if( arcInfo.left_xm <= lEnd.right_x ) {
               right_x = MIN( arcInfo.right_xm, lEnd.right_x );
               drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_xm, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            if( arcInfo.right_xm >= lStart.left_x ) {
               left_x = MAX( lStart.left_x, arcInfo.left_xm );
               drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_xm,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }

            // Draw top right
            if( arcInfo.right_x >= lStart.left_x ) {
               left_x = MAX( arcInfo.left_x, lStart.left_x );
               drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            if( arcInfo.left_x <= lEnd.right_x ) {
               right_x = MIN( arcInfo.right_x, lEnd.right_x );
               drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            if( lStart.left_x <= lEnd.right_x &&
                lStart.left_x <= arcInfo.right_x &&
                lEnd.right_x >= arcInfo.left_xm ) {
               // Portions of the lines overlap, merge into a single segment
               drawEllipticalArcDoubleRow( pMe, arcInfo.y,
                                           arcInfo.left_xm, arcInfo.right_xm,
                                           (int16) MIN( lEnd.left_x, lStart.left_x ),
                                           (int16) MAX( lEnd.right_x, lStart.right_x ),
                                           arcInfo.left_x, arcInfo.right_x,
                                           drawFrame, fillInterior,
                                           frameColor, interiorColor, ro,
                                           bDotted, &iArcDotPhase, &iPieDotPhase );
            }
            else {
               // Draw bottom right
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     (int16) MAX( lStart.left_x, arcInfo.left_xm ),
                                     (int16 )MAX( lStart.right_x, arcInfo.right_xm ),
                                     arcInfo.left_x, arcInfo.right_x,
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
               // Draw bottom left
               drawEllipticalArcRow( pMe, arcInfo.y,
                                     arcInfo.left_xm, arcInfo.right_xm,
                                     (int16) MIN( lEnd.left_x, arcInfo.left_x ),
                                     (int16) MIN( lEnd.right_x, arcInfo.right_x ),
                                     drawFrame, fillInterior,
                                     frameColor, interiorColor, ro,
                                     bDotted, &iArcDotPhase, &iPieDotPhase );
            }
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      if( arcInfo.y != arcInfo.ym ) {
         drawEllipticalArcRow( pMe, arcInfo.ym,
                                arcInfo.left_xm, arcInfo.right_x,
                                arcInfo.left_xm, arcInfo.right_x,
                                drawFrame, fillInterior,
                                frameColor, interiorColor, ro,
                                bDotted, &iArcDotPhase, &iPieDotPhase );
      }

      left_x = MAX( lStart.left_x, arcInfo.left_xm );
      right_x = MIN( lEnd.right_x, arcInfo.right_x );
      if( drawArc ) {
         if(arcInfo.left_xm <= right_x )
            drawHScanline_Dotted(pMe, arcInfo.y, arcInfo.left_xm, right_x,
                                 frameColor, ro, bDotted, &iArcDotPhase);
         if( left_x <= arcInfo.right_x )
            drawHScanline_Dotted(pMe, arcInfo.y, left_x, arcInfo.right_x,
                                 frameColor, ro, bDotted, &iArcDotPhase);
      }
      else {
         if( left_x <= arcInfo.right_x ) {
            drawEllipticalArcRow( pMe, arcInfo.y,
                                  left_x, arcInfo.right_x,
                                  left_x, arcInfo.right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }

         if( arcInfo.left_xm <= right_x ) {
            drawEllipticalArcRow( pMe, arcInfo.y,
                                  arcInfo.left_xm, right_x,
                                  arcInfo.left_xm, right_x,
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }
      }
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      // Draw top half
      drawEllipticalArcRow( pMe, arcInfo.ym,
                            arcInfo.left_xm, arcInfo.right_xm,
                            arcInfo.left_x, arcInfo.right_x,
                            drawFrame, fillInterior,
                            frameColor, interiorColor, ro,
                            bDotted, &iArcDotPhase, &iPieDotPhase );
      if( arcInfo.left_xm <= lEnd.right_x ) {
         right_x = MIN( arcInfo.right_x, lEnd.right_x);
         drawEllipticalArcRow( pMe, arcInfo.y,
                               arcInfo.left_xm, right_x,
                               arcInfo.left_xm, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
      else if( arcInfo.right_x >= lStart.left_x ) {
         left_x = MAX( arcInfo.left_xm, lStart.left_x );
         drawEllipticalArcRow( pMe, arcInfo.y,
                               left_x, arcInfo.right_x,
                               left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllipticalArcCase6()

  startAngle >= 180 && endAngle > startAngle
===========================================================================*/
static int drawEllipticalArcCase6( QGraphics *pMe, AEERect *pRect,
                                    boolean drawFrame, boolean drawArc,
                                    boolean fillInterior,
                                    NativeColor frameColor,
                                    NativeColor interiorColor,
                                    AEERasterOp ro,
                                    int16 startAngle, int16 endAngle)
{
   int exception = SUCCESS;
   AEEEllipseInfo eInfo;
   AEEStepInfo32 oStep32;
   AEEStepInfo64 oStep64;
   boolean need64 = FALSE;
   AEEEllipseLineInfo lStart;
   AEEEllipseLineInfo lEnd;
   AEEEllipseArcInfo arcInfo;
   int16 left_x;
   int16 right_x;
   int16 last_y;
   int   iArcDotPhase = 0;
   int   iPieDotPhase = 0;
   boolean bDotted = (pMe->m_strokeStyle == STROKE_DOTTED);
   MEMSET(&eInfo,0,sizeof(eInfo));

   if((pRect)->dx * (pRect)->dx + (pRect)->dy * (pRect)->dy > MAX_RADIUS_int32) {
      need64 = TRUE;
   } 
   mapAngleToLine( pRect, startAngle, &lStart );
   mapAngleToLine( pRect, endAngle, &lEnd );
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 1, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 1, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 ) {
      line_step( &lStart, pRect );
      line_step( &lEnd, pRect );
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.y != eInfo.reg.y ) {
         if( drawArc ) {
            if( arcInfo.right_xm >= lStart.left_x &&
                arcInfo.left_xm <= lEnd.right_x ) {
               left_x = MAX( arcInfo.left_xm, lStart.left_x );
               right_x = MIN( arcInfo.right_xm, lEnd.right_x );
               drawHScanline_Dotted(pMe, arcInfo.y, left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
            if( arcInfo.left_x <= lEnd.right_x &&
                arcInfo.right_x >= lStart.left_x ) {
               left_x = MAX( arcInfo.left_x, lStart.left_x );
               right_x = MIN( arcInfo.right_x, lEnd.right_x );
               drawHScanline_Dotted(pMe, arcInfo.y, left_x, right_x,
                                    frameColor, ro, bDotted, &iArcDotPhase);
            }
         }
         else {
            drawEllipticalArcRow( pMe, arcInfo.y,
                                  (int16) MAX( arcInfo.left_xm, lStart.left_x ),
                                  (int16) MAX( arcInfo.right_xm, lStart.right_x ),
                                  (int16) MIN( arcInfo.left_x, lEnd.left_x ),
                                  (int16) MIN( arcInfo.right_x, lEnd.right_x ),
                                  drawFrame, fillInterior,
                                  frameColor, interiorColor, ro,
                                  bDotted, &iArcDotPhase, &iPieDotPhase );
         }
      }
   }

   if( arcInfo.y == eInfo.reg.y || eInfo.p.x != 0 ) {
      if( arcInfo.y != eInfo.reg.y ) {
         arcInfo.y++;
         arcInfo.ym--;

         line_step( &lStart, pRect );
         line_step( &lEnd, pRect );

         arcInfo.left_xm = arcInfo.right_xm = eInfo.mir.x;
         arcInfo.left_x  = arcInfo.right_x  = eInfo.reg.x;
      }

      arcInfo.left_x = MAX( arcInfo.left_xm, lStart.left_x );
      arcInfo.right_x = MIN( arcInfo.right_x, lEnd.right_x );
      if( arcInfo.left_x <= arcInfo.right_x ) {
         drawEllipticalArcRow( pMe, arcInfo.y,
                               arcInfo.left_x, arcInfo.right_x,
                               arcInfo.left_x, arcInfo.right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }

   last_y = arcInfo.y;
   if(FALSE == need64) {
      setup_ellipse_quadrant_int32(pRect, 2, 0, 0, &eInfo, &oStep32);
   } else {
      TRY(exception, setup_ellipse_quadrant_int64(pRect, 2, 0, 0, &eInfo, &oStep64));
   }


   while( eInfo.count >= 2 && eInfo.reg.y > last_y ) {
      if(FALSE == need64) {
         ellipse_arc_step_int32(&oStep32, &eInfo, &arcInfo);
      } else {
         TRY(exception, ellipse_arc_step_int64(&oStep64, &eInfo, &arcInfo));
      }

      if( arcInfo.left_xm <= lEnd.right_x &&
          arcInfo.right_x >= lStart.left_x ) {
         left_x  = MAX( arcInfo.left_xm, lStart.left_x );
         right_x = MIN( arcInfo.right_x, lEnd.right_x);
         drawEllipticalArcRow( pMe, arcInfo.y,
                               left_x, right_x,
                               left_x, right_x,
                               drawFrame, fillInterior,
                               frameColor, interiorColor, ro,
                               bDotted, &iArcDotPhase, &iPieDotPhase );
      }
   }
   CATCH(exception);
   return exception;
}

/*===========================================================================
  Helper Function: drawEllippticalArc()

  Main helper function for drawing elliptical arcs - looks at the flags, ellipse
  size and angles and calls the appropriate drawing routine.

  Expects 0 <= startAngle < 360 and 0 < endAngle < 360.
===========================================================================*/
static int drawEllipticalArc(QGraphics *pMe, AEERect *pRect,
                              int16 startAngle, int16 arcAngle, uint8 nFlag)
{
   int exception = SUCCESS;
   NativeColor frameColor    = pMe->m_fg;
   NativeColor interiorColor = pMe->m_fl;
   boolean drawFrame    = (nFlag & AEE_GRAPHICS_FRAME) != 0;
   boolean drawArc      = (nFlag & AEE_GRAPHICS_ARC) != 0;
   boolean fillInterior = (nFlag & AEE_GRAPHICS_FILL) != 0;
   boolean clip         = (nFlag & AEE_GRAPHICS_CLIP) != 0;
   AEERasterOp ro = pMe->m_ro;
   int16 endAngle;

   if( nFlag & AEE_GRAPHICS_CLEAR ) {
      interiorColor = pMe->m_bg;
      fillInterior = TRUE;
      ro = AEE_RO_COPY;
   }

   endAngle = startAngle + arcAngle;
   if( endAngle >= 360 ) {
      endAngle -= 360;
   }

   if( pRect->dx <= 2 || pRect->dy <= 2 ) {
      drawEllipticalArcLine( pMe, pRect, drawFrame, fillInterior, clip,
                             frameColor, interiorColor, ro,
                             startAngle, endAngle );
      THROW(exception, SUCCESS);
   }

   pRect->dx--;
   pRect->dy--;

   if( startAngle < 180 ) {
      if( endAngle < startAngle ) {
         TRY(exception, drawEllipticalArcCase1( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      } else if( endAngle <= 180 ) {
         TRY(exception, drawEllipticalArcCase2( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      } else {
         TRY(exception, drawEllipticalArcCase3( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      }
   }
   else {
      if( endAngle < 180 ) {
         TRY(exception, drawEllipticalArcCase4( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      } else if( endAngle < startAngle ) {
         TRY(exception, drawEllipticalArcCase5( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      } else {
         TRY(exception, drawEllipticalArcCase6( pMe, pRect, drawFrame, drawArc, fillInterior,
                                      frameColor, interiorColor, ro,
                                      startAngle, endAngle ));
      }
   }
   CATCH(exception);
   return exception;
}


/*===========================================================================
  Helper Function: drawRoundRectangle()

  Main helper function for drawing round rectangles - looks at the flags,
  ellipse size and rectangle size and calls the appropriate draw routine

  Expects:
      arcWidth > 2 && arcHeight > 2
      pRect.dx >= arcWidth && pRect.dy >= arcHeight
===========================================================================*/
static int drawRoundRectangle(QGraphics *pMe, AEERect *pRect,
                               int16 arcWidth, int16 arcHeight, uint8 nFlag)
{
   int exception = SUCCESS;
   NativeColor frameColor    = pMe->m_fg;
   NativeColor interiorColor = pMe->m_fl;
   boolean drawFrame    = (nFlag & AEE_GRAPHICS_FRAME) != 0;
   boolean fillInterior = (nFlag & AEE_GRAPHICS_FILL) != 0;
   boolean clip         = (nFlag & AEE_GRAPHICS_CLIP) != 0;
   AEERasterOp ro = pMe->m_ro;

   AEERect eRect;               /* ellipse rectangle */

   if( nFlag & AEE_GRAPHICS_CLEAR ) {
      interiorColor = pMe->m_bg;
      fillInterior = TRUE;
      ro = AEE_RO_COPY;
   }

   if( pRect->dx <= 2 || pRect->dy <= 2 ) {
      drawEllipseLine( pMe, pRect, drawFrame, fillInterior, clip,
                       frameColor, interiorColor, ro );
      THROW(exception, SUCCESS);
   }

   eRect.x = pRect->x;
   eRect.y = pRect->y;
   eRect.dx = arcWidth - 1;
   eRect.dy = arcHeight - 1;

   if( drawFrame ) {
      TRY(exception, drawEllipseWithFrame( pMe, &eRect, fillInterior, clip,
                            frameColor, interiorColor, ro,
                            (int16) (pRect->dx - eRect.dx - 1),
                            (int16) (pRect->dy - eRect.dy - 1) ));
   } else {
      TRY(exception, drawEllipseWithoutFrame( pMe, &eRect, fillInterior, clip,
                               interiorColor, ro,
                               (int16) (pRect->dx - eRect.dx - 1),
                               (int16) (pRect->dy - eRect.dy - 1) ));
   }
   CATCH(exception);
   return exception;
}



////////////////////////////////////////////////////////////////////////////////////////
//    Local functions that transform geometric primitives for window coordinates
//    to screen coordinates.
////////////////////////////////////////////////////////////////////////////////////////

/*===========================================================================
  Helper Function: transformLineToScreen()

===========================================================================*/
static void transformLineToScreen(QGraphics *pMe, AEELine *out, AEELine *in)
{
   if (pMe && in && out) {
      *out = *in;
      out->sx += pMe->m_translate.x;
      out->sy += pMe->m_translate.y;
      out->ex += pMe->m_translate.x;
      out->ey += pMe->m_translate.y;
   }
}


/*===========================================================================
  Helper Function: transformRectToScreen()

===========================================================================*/
static void transformRectToScreen(QGraphics *pMe, AEERect *out, AEERect *in)
{
   if (pMe && in && out) {
      *out = *in;
      out->x += pMe->m_translate.x;
      out->y += pMe->m_translate.y;
   }
}


/*===========================================================================
  Helper Function: transformPolylineToScreen()

===========================================================================*/
static int transformPolylineToScreen(QGraphics *pMe, AEEPolyline *out, AEEPolyline *in)
{
   if (pMe && in && out) {
      int i;
      int len = in->len;   // dereferencing for better speed
      AEEPoint *pPoint;    // pointer to AEEPoint

      if ((out->points = (AEEPoint *)MALLOC(sizeof(AEEPoint) * len)) == NULL)
         return ENOMEMORY;

      MEMCPY(out->points, in->points, sizeof(AEEPoint) * len);

      out->len = len;
      pPoint = out->points;   // dereferencing for better speed

      for (i = 0; i < len; i++) {
         pPoint->x += pMe->m_translate.x;
         pPoint->y += pMe->m_translate.y;
         pPoint++;   // more efficient dereferencing
      }
   }

   return SUCCESS;   // success
}



/*===========================================================================
  Helper Function: ConvertCircleToRect()

===========================================================================*/
static void convertCircleToRect(AEERect *pRect, AEECircle *pCircle)
{
   pRect->x = pCircle->cx - pCircle->r;
   pRect->y = pCircle->cy - pCircle->r;

   if( pCircle->r == 0 ) {
      pRect->dx = pRect->dy = 0;
   }
   else {
      pRect->dx = pRect->dy = 2 * pCircle->r + 1;
   }
}



/*===========================================================================
  Helper Function: convertEllipseToRect()

===========================================================================*/
static void convertEllipseToRect(AEERect *pRect, AEEEllipse *pEllipse)
{
   pRect->x = pEllipse->cx - pEllipse->wx;
   pRect->y = pEllipse->cy - pEllipse->wy;

   if( pEllipse->wx == 0 || pEllipse->wy == 0 ) {
      pRect->dx = pRect->dy = 0;
   }
   else {
      pRect->dx = 2 * pEllipse->wx + 1;
      pRect->dy = 2 * pEllipse->wy + 1;
   }
}



//////////////////////////////////////////////////////////////
//    CODE TO FILL ARBITRARY POLYGON
//////////////////////////////////////////////////////////////


// The following code follows the algorithm presented in "Computer Graphics",
// Donald Hearn and M. Pauline Baker.
// However the algorithms had to be modified to handle degeneracy. 
// Also a severe bug in the original algorithm is fixed: the original algorithm
// crashes when horizontal line is present in a self-intersection polygons.


// It's computationally expensive to handle all degeneracies. It's also computationally
// expensive to implement XOR such that every pixel has the right color.  In most cases,
// errors at one or 2 pixels should be acceptable visually.  Also the inaccuracy only shows
// up with with XOR mode.
// To balance accuracy and speed, we provide two algorithms: accurate and fast.  The applicatin
// programmers are encouraged to use the "fast" algorithm.  A user should not need the "accurate" 
// algorithm unless, the input polygon has self-intersections and the raster op is set to XOR.

// "Accurate" and "fast" algorithm share some of the code.  Those code will be listed
// first in this file.

// COMMON code for both "accurate" and "fast" implementations.

static void preprocessPolygon(AEEPolygon *pPolygon, int *yMin, int *yMax)
{
   int i;
   AEEPoint *p = pPolygon->points;

   *yMin = *yMax = p[0].y;

   for (i = 1; i < pPolygon->len; i++) {
      if (p[i].y > *yMax)
         *yMax = p[i].y;

      if (p[i].y < *yMin)
         *yMin = p[i].y;
   }
}


/*===========================================================================
  Helper Function: yNext()
 Given an index, return the y-coordinate of next vertex that is not
 on the same horizontal line.
===========================================================================*/
static int yNext(int current, int len, AEEPoint *points)
{
   int next;
   
   if (current == (len - 1))
      next = 0;
   else
      next = current + 1;

   while (points[current].y == points[next].y)
      if (next == (len - 1))
         next = 0;
      else
         next++;

   return (points[next].y);
}



/*===========================================================================
  Helper Function: yPrev()
 Given an index, return the y-coordinate of previous vertex that is
 not on the same horizontal line
===========================================================================*/
static int yPrev(int current, int len, AEEPoint *points)
{
   int prev;

   if (current == 0)
      prev = len - 1;
   else
      prev = current - 1;

   while (points[current].y == points[prev].y)
      if (prev == 0)
         prev = len - 1;
      else 
         prev--;

   return (points[prev].y);
}


/*===========================================================================
  Helper Function: insertHorizontalEdge()
 Insert horizontal edges into list in order of increasing y coordinates
===========================================================================*/
static void insertHorizontalEdge(HorizontalEdge *list, HorizontalEdge *edge)
{
   HorizontalEdge *p, *q = list;
   
   p = q->next;

   while(p)
      if (edge->y < p->y)
         p = NULL;
      else {
         q = p;
         p = p->next;
      }

   edge->next = q->next;
   q->next = edge;
}

/*===========================================================================
  Helper Function: deleteHorizontalEdges()
===========================================================================*/
static void deleteHorizontalEdges(HorizontalEdge *list)
{
   HorizontalEdge *p, *q = list;

   p = q->next;

   while (p) {
      q->next = p->next;
      FREE(p);
      p = q->next;
   }
}



// Topological Properties:
// There is a very important topological properties that we have to keep in
// mind while dealing with horizontal edges.  Luckily, each scanline interval
// can intersect at most 1 horizontal edge.  Otherwise the polygon code would be
// much messier.
// 
// This function returns TRUE if it finds a horizontal edge that intersects with
// the interval [xMin, xMax] and stores the intersection in [intersectionMin, intersectionMax].
// Otherwise it retures FALSE. When it returns FALSE, [intersectionMin, intersectionMax]
// no valid data.
static boolean intersectHorizontalEdge(HorizontalEdge *list, int16 scan, 
                                       int16 xMin, int16 xMax,
                                       int16 *intersectionMin, int16 *intersectionMax)
{
   HorizontalEdge *q = list;
   HorizontalEdge *p = q->next;

   while (p) {
      if (p->y > scan)
         return FALSE;
      else if ((p->y == scan) && intervalIntersection(p->xMin, p->xMax, xMin, xMax, 
                                                      intersectionMin, intersectionMax))
         return TRUE;

      p = p->next;
   }

   return FALSE;
}


/*===========================================================================
  Helper Function: insertEdgeAccurate()
 Inserts edge into list in order of increasing xIntersect field.
===========================================================================*/
static void insertEdgeAccurate(EdgeAccurate *list, EdgeAccurate *edge)
{
   EdgeAccurate *p, *q = list;
   
   p = q->next;

   while (p)
      if (edge->xIntersectMin < p->xIntersectMin)
         p = NULL;
      else if ((edge->xIntersectMin == p->xIntersectMin) &&
               (edge->xUpper < p->xUpper))
         p = NULL;
      else {
         q = p;
         p = p->next;
      }

   edge->next = q->next;
   q->next = edge;
}


/*===========================================================================
  Helper Function: makeEdgeRecAccurate()
 Store the inverse slope for each edge.  Adjust
 and store upper-y coordinate for edges that are the lower member
 of a monotically increasing or decreasing pair of edges.
===========================================================================*/
static void makeEdgeRecAccurate(AEEPoint lower, AEEPoint upper, int yComp,
                                int yMin, EdgeAccurate *edge, EdgeAccurate *edges[])
{
   edge->dx = upper.x - lower.x; // might be negative
   edge->abs_2dx = ABS(edge->dx) + ABS(edge->dx);
   edge->dy = upper.y - lower.y; // always positive
   edge->count = 0; // used to round xIntersect to the nearest integer.
   edge->yLower = lower.y;
   edge->xUpper = upper.x; // needed for sorting

   edge->xIntersectMin = edge->xIntersectMax = lower.x;

   if (upper.y < yComp) {
      edge->yUpper = upper.y - 1;
      (edge->dy)--;  // edge->dy might be 0 after this step
   }
   else if (upper.y > yComp) {
      edge->yUpper = upper.y;  
   }

   edge->dy2 = edge->dy + edge->dy;
   insertEdgeAccurate(edges[lower.y - yMin], edge);
}


/*===========================================================================
  Helper Function: buildEdgeListAccurate()
===========================================================================*/
static int buildEdgeListAccurate(int len, AEEPoint points[], int yMin,
                                 EdgeAccurate *edges[], HorizontalEdge *horizontalEdgeList, 
                                 EdgeAccurate *pEdges)
{
   EdgeAccurate  *edge;
   AEEPoint tail, head;
   int i;
   int prev = len - 1;
   int count = 0;
   
   tail.x = points[prev].x; 
   tail.y = points[prev].y;

   for (i = 0; i < len; i++) {
      head = points[i];

      if (tail.y != head.y) {  // nonhorizontal lines only
         edge = pEdges + count++;

         if (tail.y < head.y)  // up-going edge
            makeEdgeRecAccurate(tail, head, yNext(i, len, points), yMin, edge, edges);
         else                 // down-going edge
            makeEdgeRecAccurate(head, tail, yPrev(prev, len, points), yMin, edge, edges);
      }
      else {   // insert horizontal edges for the horizontal-edge list
         HorizontalEdge *hEdge;
         if ((hEdge = (HorizontalEdge *) MALLOC(sizeof(HorizontalEdge))) == NULL) {
            deleteHorizontalEdges(horizontalEdgeList);
            return ENOMEMORY;
         }

         hEdge->y = head.y;
         hEdge->xMin = MIN(head.x, tail.x);
         hEdge->xMax = MAX(head.x, tail.x);

         insertHorizontalEdge(horizontalEdgeList, hEdge);
      }
      
      prev = i;
      tail = head;
   }

   return SUCCESS;
}



/*===========================================================================
  Helper Function: buildActiveListAccurate()
===========================================================================*/
static void buildActiveListAccurate(int scan, int yMin, 
                            EdgeAccurate *active, EdgeAccurate *edges[])
{
   EdgeAccurate *p, *q;
   
   p = edges[scan-yMin]->next;

   while (p) {
      q = p->next;
      insertEdgeAccurate(active, p);
      p = q;
   }
}



// All intervals in the following 3 function are inclusive.  Namely the
// endpoints are included.
// One key assumption for the following 3 functions is: The input intervals
// are non-empty.  Namely xMin[1, 2] <= xMax[1. 2]

/*===========================================================================
  Helper Function: intervalIntersection()
 It returns TRUE if the two intervals has a non-empty intersection and stores
 the intersection in [xMin, xMax]. Otherwise it returns FALSE and the result is
 simple the two input intervals.
===========================================================================*/
static boolean intervalIntersection(int16 xMin1, int16 xMax1, // interval 1
                                    int16 xMin2, int16 xMax2, // interval 2
                                    int16 *xMin, int16 *xMax)   // result
{
   *xMin = MAX(xMin1, xMin2);
   *xMax = MIN(xMax1, xMax2);

   if (*xMin > *xMax)
      return FALSE;  // empty intersection

   return TRUE;
}


/*===========================================================================
  Helper Function: intervalUnion()
 It return TRUE if the two intervals has a non-empty intersection and 
 stores the union in [xMin, xMax].  Otherwise it returns FALSE.
===========================================================================*/
static boolean intervalUnion(int16 xMin1, int16 xMax1, // interval 1
                             int16 xMin2, int16 xMax2, // interval 2
                             int16 *xMin, int16 *xMax)   // result
{
   if (!intervalIntersection(xMin1, xMax1, xMin2, xMax2, xMin, xMax))
      return FALSE;  // empty intersection

   // Compute the union
   *xMin = MIN(xMin1, xMin2);
   *xMax = MAX(xMax1, xMax2);
   return TRUE;
}


/*===========================================================================
  Helper Function: intervalDiff()

// It returns TRUE if [xMin1, xMax1] - [xMin2, xMax2] is non-empty and stores
// the difference in [xMin, xMax].  Otherwise it returns FALSE.
// Note: The result of the DIFF might be two disjoint intervals, when the second
// interval is completely contained in the first one.
// For convenience in terms of interface, we always return two intervals as the
// result.  One might be empty if its min is greater than it max.
// When an empty interval may causes problem, it's recommended to check the
// returned value.
===========================================================================*/
static boolean intervalDiff(int16 xMin1, int16 xMax1, // interval 1
                            int16 xMin2, int16 xMax2, // interval 2
                            int16 *int1xMin, int16 *int1xMax,     // result: interval 1
                            int16 *int2xMin, int16 *int2xMax)     // result: interal 2
{
   int16 xMin, xMax;

   if (!intervalIntersection(xMin1, xMax1, xMin2, xMax2, &xMin, &xMax)) { // DIFF = [xMin1, xMax1]
      // Put result in the first interval
      *int1xMin = xMin1;
      *int1xMax = xMax1;

      // Make the second empty
      *int2xMin = xMax1;
      *int2xMax = xMin1 - 1;   // just incase xMin == xMax1
   }
   else {   // [xMin, xMax] is non-empty
      // interval1 = [xMin1, xMin - 1]
      *int1xMax = xMin - 1;
      *int1xMin = xMin1;
      // interval2 = [xMax + 1, xMax1]
      *int2xMin = xMax + 1;
      *int2xMax = xMax1;

      if ((*int1xMin > *int1xMax) && (*int2xMin > *int2xMax))  // both empty
         return FALSE;  // empty difference
   }

   return TRUE;
}


/*===========================================================================
  Helper Function: fillScanAccurate()

===========================================================================*/
static void fillScanAccurate(QGraphics *pMe, int16 scan, EdgeAccurate *active, HorizontalEdge *horizontalEdges,
                             int bClipped)
{
   EdgeAccurate *p1, *p2;
   HorizontalEdge *h;
   int16 xMin, xMax;
   int16 xMin1, xMax1;  // first interval in the diff
   int16 xMin2, xMax2;  // second interal in the diff

   // Avoid repeated dereferencing.  It's something that the compiler's optimization option
   // may have taken care of.  But we should not rely on that, especially when our code
   // may have to run on different platforms, compiled by different compilers.
   IBitmap *pbm = pMe->m_pbmDst;
   NativeColor fg = pMe->m_fg;
   NativeColor fl = pMe->m_fl;
   AEERasterOp ro = pMe->m_ro;

   p1 = active->next;

   while(p1) {
      p2 = p1->next;

      if (intersectHorizontalEdge(horizontalEdges, scan, p1->xIntersectMin, 
                                  p2->xIntersectMax, &xMin, &xMax)) {
         // Fill the interior portion not occupied by the horizontal edge
         if (pMe->m_fill && intervalDiff((int16) (p1->xIntersectMax+1), (int16) (p2->xIntersectMin - 1),
            xMin, xMax, &xMin1, &xMax1, &xMin2, &xMax2)) {
            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, xMin1, xMax1, fl, ro);
            else
               drawHScanline(pMe, scan, xMin1, xMax1, fl, ro);

            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, xMin2, xMax2, fl, ro);
            else
               drawHScanline(pMe, scan, xMin2, xMax2, fl, ro);
         }

         // Draw the frame
         if (intervalUnion(p1->xIntersectMin, p1->xIntersectMax, 
            p2->xIntersectMin, p2->xIntersectMax, &xMin1, &xMax1)) { 
            // p1 and p2 intersect and the y coordiate of this intersection
            // equals to "scan".
            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, xMin1, xMax1, fg, ro);
            else
               drawHScanline(pMe, scan, xMin1, xMax1, fg, ro);
         }
         else {   // p1 and p2 doesn't intersect on this scanline
            if (intervalDiff(p1->xIntersectMin, p1->xIntersectMax, xMin, xMax,
               &xMin1, &xMax1, &xMin2, &xMax2)) {
               if (bClipped)
                  IBITMAP_DrawHScanline(pbm, scan, xMin1, xMax1, fg, ro);
               else
                  drawHScanline(pMe, scan, xMin1, xMax1, fg, ro);

               if (bClipped)
                  IBITMAP_DrawHScanline(pbm, scan, xMin2, xMax2, fg, ro);
               else
                  drawHScanline(pMe, scan, xMin2, xMax2, fg, ro);
            }

            if (intervalDiff(p2->xIntersectMin, p2->xIntersectMax, xMin, xMax,
               &xMin1, &xMax1, &xMin2, &xMax2)) {
               if (bClipped)
                  IBITMAP_DrawHScanline(pbm, scan, xMin1, xMax1, fg, ro);
               else
                  drawHScanline(pMe, scan, xMin1, xMax1, fg, ro);

               if (bClipped)
                  IBITMAP_DrawHScanline(pbm, scan, xMin2, xMax2, fg, ro);
               else
                  drawHScanline(pMe, scan, xMin2, xMax2, fg, ro);
            }
         }
      }
      else {   // no issues with horizontal edges
         // fill the interior
         if (pMe->m_fill) {
            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, (int16)(p1->xIntersectMax + 1), (int16)(p2->xIntersectMin - 1), fl, ro);
            else
               drawHScanline(pMe, scan,
                             (int16)(p1->xIntersectMax + 1),
                             (int16)(p2->xIntersectMin - 1), fl, ro);
         }

         // draw the frame

         // If p1 and p2 has an intersection with "scan" being its y-coordinate,
         // we need to avoid drawing the same point twice.  Otherwise there will be
         // a "tiny little" hole at the intersection, when XOR is the raster op.
         if (p1->xIntersectMax < p2->xIntersectMin) { // no intersection
            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, p1->xIntersectMin, p1->xIntersectMax, fg, ro);
            else
               drawHScanline(pMe, scan, p1->xIntersectMin, p1->xIntersectMax,
                             fg, ro);
                 
            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, p2->xIntersectMin, p2->xIntersectMax, fg, ro);
            else
               drawHScanline(pMe, scan, p2->xIntersectMin, p2->xIntersectMax,
                             fg, ro);
         }
         else {   // Intersection!!!!!! Draw the union
            int16 xMax = MAX(p1->xIntersectMax, p2->xIntersectMax);

            if (bClipped)
               IBITMAP_DrawHScanline(pbm, scan, (int16)MIN(p1->xIntersectMin, p2->xIntersectMin), xMax, fg, ro);
            else
               drawHScanline(pMe, scan,
                             (int16)MIN(p1->xIntersectMin, p2->xIntersectMin),
                             xMax, fg, ro);
         }
      }

      p1 = p2->next;
   }


   // Draw the horizontal edge with the same y-coordinate
   h = horizontalEdges->next;

   while (h) {
      if (h->y != scan)
         break;

      // replace this piece of code with line clipping
      if (bClipped)
         IBITMAP_DrawHScanline(pbm, scan, h->xMin, h->xMax, fg, ro);
      else
         drawHScanline(pMe, scan, h->xMin, h->xMax, fg, ro);

      h = h->next;
   }
}


/*===========================================================================
  Helper Function: updateHorizontalEdgeList()

===========================================================================*/
static void updateHorizontalEdgeList(HorizontalEdge *list, int scan)
{
   HorizontalEdge *q = list;
   HorizontalEdge *p = q->next;

   while (p) {
      if (p->y <= scan) {
         q->next = p->next;
         FREE(p);
         p = q->next;
      }
      else
         break;
   }
}



/*===========================================================================
  Helper Function: updateXIntersections()
 Delete completed edges. Update 'xIntersect' field for others.
 "scan" is the y coordinate.
===========================================================================*/
static void updateXIntersections(int scan, EdgeAccurate *active)
{
   EdgeAccurate *p = active->next;
   int xOld = 0;   // to get rid of a compilation warning
   
   while (p) {
      if (scan > p->yUpper) {
         p = p->next;
      }
      else {
         // update the xIntersection from the previous scan line
         if (p->dx > 0)
            xOld = p->xIntersectMax;
         else if (p->dx < 0)
            xOld = p->xIntersectMin;
         
         if (!(p->dy2)) {  // p->dy2 is now zero due to shortening
            if (p->dx > 0)
               p->xIntersectMax = p->xUpper;
            else if (p->dx < 0)
               p->xIntersectMin = p->xUpper;
         }
         else {
            while(p->count >= p->dy) {
               p->count -= p->dy2;
            
               if (p->dx == 0)   // do nothing for a vertical edge
                  break;
               else if (p->dx > 0)
                  (p->xIntersectMax)++;
               else   // (p->dx < 0)
                  (p->xIntersectMin)--;
            }
         }

         if ((p->dx > 0) && (p->xIntersectMax > xOld))
            p->xIntersectMin = xOld + 1;
         else if ((p->dx < 0) && (p->xIntersectMin < xOld))
            p->xIntersectMax = xOld - 1;

         p->count += p->abs_2dx;         
         p = p->next;

      }
   }
}


/*===========================================================================
  Helper Function: updateActiveListAccurate()

===========================================================================*/
static void updateActiveListAccurate(int scan, EdgeAccurate *active)
{
   EdgeAccurate *q = active, *p = active->next;

   while (p)
      if (scan >= p->yUpper) {
         p = p->next;
         q->next = q->next->next;
      }
      else {
         q = p;
         p = p->next;
      }
}



/*===========================================================================
  Helper Function: resortActiveListAccurate()

===========================================================================*/
static void resortActiveListAccurate(EdgeAccurate *active)
{  
   EdgeAccurate *q, *p = active->next;

   p = active->next;
   active->next = NULL;
   while(p) {
      q = p->next;
      insertEdgeAccurate(active, p);
      p = q;
   }
}


/*===========================================================================
  Helper Function: fillPolygonAccurate()

  This function assumes that the degeneracy has been taken care of before
 this function is called. There are two topological degeneracy:
 CASE I:  A single point.
 CASE II: The polygon consists of only a set of horizontal lines.
===========================================================================*/
static int fillPolygonAccurate(QGraphics *pMe, AEEPolygon *pPolygon, int bClipped)
{
   EdgeAccurate **edges;
   EdgeAccurate *active = NULL;
   int16 i, scan;
   int len = pPolygon->len;
   AEEPoint *points = pPolygon->points;
   int polygonHeight, yMin, yMax;
   HorizontalEdge *horizontalEdges;
   EdgeAccurate *pEdges;

   preprocessPolygon(pPolygon, &yMin, &yMax);
   polygonHeight = yMax - yMin + 1;
  
   // For efficiency, we would like to avoid the multiple calls of mallocing small
   // pieces of memory.
   // Here the memory map for this buffer:
   // polygonHeight * sizeof(Edge*) followed by
   // polygonHeight * sizeof(Edge) followed by
   // pPolygon->len * sizeof(Edge) followed by
   // 1 sizeof(Edge) followed by
   // 1 sizeof(HorizontalEdge)
   if ((edges = (EdgeAccurate **)MALLOC(sizeof(EdgeAccurate*) * polygonHeight + sizeof(EdgeAccurate) * (polygonHeight + pPolygon->len + 1) + sizeof(HorizontalEdge))) == NULL)
      return ENOMEMORY;
   
   pEdges = (EdgeAccurate*) (edges + polygonHeight);

   for (i = 0; i < polygonHeight; i++) {
      edges[i] = pEdges+i;
      edges[i]->next = NULL;
   }

   pEdges += polygonHeight;
   active = pEdges + pPolygon->len;
   horizontalEdges = (HorizontalEdge *) (active + 1);

   if (buildEdgeListAccurate(len, points, yMin, edges, horizontalEdges, pEdges) == SUCCESS){
      // We only need to scan the polygon, not the entire window.
      for (scan = yMin; scan <= yMax; scan++) {
         buildActiveListAccurate(scan, yMin, active, edges);
         
         if (active->next) {
            updateXIntersections(scan, active);
            resortActiveListAccurate(active);
            fillScanAccurate(pMe, scan, active, horizontalEdges, bClipped);
            updateActiveListAccurate(scan, active);
            updateHorizontalEdgeList(horizontalEdges, scan);
         }
      }
   }
   else 
      deleteHorizontalEdges(horizontalEdges);

   FREE(edges);
   return SUCCESS;
}



/*===========================================================================
  Helper Function: isHorizontal()

 Assumption: pPolygon has at least two points.
===========================================================================*/
static boolean isHorizontal(AEEPolygon *pPolygon)
{
   int len = pPolygon->len;
   AEEPoint *points = pPolygon->points;
   int y = points[0].y;
   int i;

   for (i = 1; i < len; i++)
      if (y != points[i].y)
         return FALSE;

   return TRUE;
}


/*===========================================================================
  Helper Function: mergeHorizontalSegments()

 This function takes as input a polygon which has been verified as a set of 
 horizontal lines only. It merges them into one single horizontal line.
===========================================================================*/
static void mergeHorizontalSegments(AEEPolygon *pPolygon, AEELine *pLine)
{
   int minX, maxX;
   int len = pPolygon->len;
   int i;
   AEEPoint *points = pPolygon->points;

   minX = maxX = points[0].x;

   for (i = 1; i < len; i++) {
      if (points[i].x < minX)
         minX = points[i].x;
      else
         maxX = points[i].x;
   }

   pLine->sx = minX;
   pLine->ex = maxX;
   pLine->sy = pLine->ey = points[0].y;
}



/////////////////////////////////////////////////
//    Code for "fast" polygon implementation
/////////////////////////////////////////////////

/*===========================================================================
  Helper Function: insertEdgeFast()

Inserts edge into list in order of increasing xIntersect field. 
===========================================================================*/
static void insertEdgeFast(EdgeFast * list, EdgeFast * edge)
{
   EdgeFast * p, * q = list;
   
   p = q->next;
   while (p != NULL) {
      if (edge->xIntersect < p->xIntersect)
         p = NULL;
      else {
         q = p;
         p = p->next;
      }
   }
   edge->next = q->next;
   q->next = edge;
}


/*===========================================================================
  Helper Function: makeEdgeRecFast()

Store lower-y coordinate and inverse slope for each edge.  Adjust
and store upper-y coordinate for edges that are the lower member
of a monotically increasing or decreasing pair of edges 
===========================================================================*/
static void makeEdgeRecFast(AEEPoint lower, AEEPoint upper, int yComp, int yMin,
                            EdgeFast * edge, EdgeFast * edges[])
{
   edge->dx = upper.x - lower.x; // might be negative
   edge->abs_2dx = ABS(edge->dx) + ABS(edge->dx);
   edge->dy = upper.y - lower.y; // always non-negative
   edge->dy2 = edge->dy + edge->dy;
   edge->count = 0;              // used to round xIntersect to the nearest integer.

   edge->xIntersect = lower.x;
   if (upper.y < yComp)
      edge->yUpper = upper.y - 1;
   else if (upper.y > yComp)
      edge->yUpper = upper.y;
   else {   // horizontal edge
      edge->yUpper = upper.y - 1;
   }

   insertEdgeFast(edges[lower.y - yMin], edge);
}


/*===========================================================================
  Helper Function: buildEdgeListFast()

===========================================================================*/
static void buildEdgeListFast(int cnt, AEEPoint *pts, EdgeFast *edges[], int yMin, EdgeFast *pEdges)
{
   EdgeFast *edge;
   AEEPoint v1, v2;
   int i, prev = cnt - 1;
   int count = 0;
   
   v1.x = pts[prev].x; 
   v1.y = pts[prev].y;

   for (i=0; i<cnt; i++) {
      v2 = pts[i];
      if (v1.y != v2.y) {                /* nonhorizontal line */
         edge = pEdges + count++;

         if (v1.y < v2.y)                 /* up-going edge      */
            makeEdgeRecFast(v1, v2, yNext(i, cnt, pts), yMin, edge, edges);
         else                             /* down-going edge    */
            makeEdgeRecFast(v2, v1, yPrev(prev, cnt, pts), yMin, edge, edges);
      }

      prev = i;
      v1 = v2;
   }
}



/*===========================================================================
  Helper Function: buildActiveListFast()

===========================================================================*/
static void buildActiveListFast(int scan, int yMin, EdgeFast * active, EdgeFast * edges[])
{
   EdgeFast *p, *q;
   
   p = edges[scan - yMin]->next;
   while (p) {
      q = p->next;
      insertEdgeFast(active, p);
      p = q;
   }
}


/*===========================================================================
  Helper Function: fillScanFast()

===========================================================================*/
static void fillScanFast(QGraphics *pMe, int16 scan, EdgeFast * active, int bClipped)
{
   EdgeFast *p1, *p2;
   
   p1 = active->next;
   while (p1) {
      p2 = p1->next;

      if (bClipped)    // no scan line clipping is required
         IBITMAP_DrawHScanline(pMe->m_pbmDst, scan, p1->xIntersect, p2->xIntersect, pMe->m_fl, pMe->m_ro);
      else
         drawHScanline(pMe, scan, p1->xIntersect, p2->xIntersect,
                       pMe->m_fl, pMe->m_ro);
      p1 = p2->next;
   }
}


/*===========================================================================
  Helper Function: updateActiveListFast()
 Delete completed edges. Update 'xIntersect' field for others
===========================================================================*/
static void updateActiveListFast(int scan, EdgeFast *active)
{
   EdgeFast *q = active, *p = active->next;
   
   while (p) 
      if (scan >= p->yUpper) {
         p = p->next;
         q->next = q->next->next;
      }
      else {
         p->count += p->abs_2dx;
         while (p->count >= p->dy) {
            p->count -= p->dy2;

            if (p->dx == 0)   // do nothing for a vertical edge
               break;
            else if (p->dx > 0)
               (p->xIntersect)++;
            else // (p->dx < 0)
               (p->xIntersect)--;
         }
         q = p;
         p = p->next;
      }
}


/*===========================================================================
  Helper Function: resortActiveListFast()
 
===========================================================================*/
static void resortActiveListFast(EdgeFast *active)
{
   EdgeFast *q, *p = active->next;
   
   active->next = NULL;
   while (p) {
      q = p->next;
      insertEdgeFast(active, p);
      p = q;
   }
}


/*===========================================================================
  Helper Function: fillPolygonFast()
 The current implementation assume that input polygon fits the window.
 It rejects any polygon that is larger than the current window. Nothing 
 would be drawn given such a polygon. 
===========================================================================*/
static int fillPolygonFast(QGraphics *pMe, AEEPolygon *pPolygon, int bClipped)
{
   EdgeFast **edges;
   EdgeFast *active, *pEdges;
   int i;
   int16 scan;
   AEEPoint *pts = pPolygon->points;
   int yMin, yMax, polygonHeight;

   preprocessPolygon(pPolygon, &yMin, &yMax);
   polygonHeight = yMax - yMin + 1;

   // we should only need one malloc for all
   // That's where we get nearly 10 fold speed optimization
   edges = (EdgeFast**)MALLOC(sizeof(EdgeFast*) * polygonHeight + (polygonHeight + pPolygon->len + 1) * sizeof(EdgeFast));
   if (!edges)
      return ENOMEMORY;

   pEdges = (EdgeFast*) (edges + polygonHeight);
   
   for (i= 0; i < polygonHeight; i++) {
      edges[i] = pEdges + i;
      edges[i]->next = NULL;
   }

   pEdges += polygonHeight;
   buildEdgeListFast(pPolygon->len, pts, edges, yMin, pEdges);

   active = pEdges + pPolygon->len;
   
   for (scan=yMin; scan<=yMax; scan++) {
      buildActiveListFast(scan, yMin, active, edges);
      if (active->next) {
         fillScanFast(pMe, scan, active, bClipped);
         updateActiveListFast(scan, active);
         resortActiveListFast(active);
      }
   }
   
   // Free edge records that have been MALLOC'ed ... 
   FREE(edges);

   return SUCCESS;
}



/////////////////////////////////////////////////////////////////////
//    SUTHERLAND-HODGEMAN POLYGON CLIPPING
/////////////////////////////////////////////////////////////////////


#define N_EDGE 4  // a rectangular window only has 4 sides

/*===========================================================================
  Helper Function: inside()
 Whether the point "p" is inside a given "edge"
===========================================================================*/
int inside(AEEPoint p, int b, AEEPoint wMin, AEEPoint wMax)
{
   switch (b) {  
   case LEFT:   if (p.x < wMin.x) return (FALSE); break;
   case RIGHT:  if (p.x > wMax.x) return (FALSE); break;
   case BOTTOM: if (p.y < wMin.y) return (FALSE); break;
   case TOP:    if (p.y > wMax.y) return (FALSE); break;
   }
   return (TRUE);
}


/*===========================================================================
  Helper Function: cross()
 Whether the edge from "p1" to "p2" crosses edge "b".
===========================================================================*/
static int cross(AEEPoint p1, AEEPoint p2, int b, 
                 AEEPoint wMin, AEEPoint wMax)
{
   if (inside(p1, b, wMin, wMax) == inside (p2, b, wMin, wMax))
      return (FALSE);
   else 
      return (TRUE);
}


/*===========================================================================
  Helper Function: intersect()
 compute the intersection
===========================================================================*/
static AEEPoint intersect(AEEPoint p1, AEEPoint p2, int b, AEEPoint wMin, AEEPoint wMax)
{
   AEEPoint pt;
   FractionType m;

   // For efficiency, the next two lines should be moved into each "case" block.  
   // However this makes the code fat and ugly.  So we keep the little piece of
   // inefficient code here.  Namely we compute "m" regardless whether it's necesary.
   m.e = p1.y - p2.y;
   m.d = p1.x - p2.x;
   
   // This function should only be called after "cross" has returned TRUE.
   // Therefore it's safe not to check whether the line is vertical for the 
   // first 2 cases in the switch statement. 
   switch (b) {
   case LEFT:
      // m.d won't be zero, otherwise "cross" would have returned FALSE
      pt.x = wMin.x;
      pt.y = (p1.y == p2.y) ? p2.y : p2.y + ((wMin.x - p2.x) * m.e + m.d/2) / m.d;
      break;

   case RIGHT:
      // m.d won't be zero, otherwise "cross" would have returned FALSE
      pt.x = wMax.x;
      pt.y = (p1.y == p2.y) ? p2.y : p2.y + ((wMax.x - p2.x) * m.e + m.d/2) / m.d;
      break;

   case BOTTOM:
      // m.e won't be zero, otherwise "cross" would have returned FALSE
      pt.y = wMin.y;
      // The conditional statement here is not for robustness, because m.e
      // cannot be zero.  It's for efficiency instead, because a comparison
      // is cheaper than "1 mul, 2 div, and 2 plus".
      pt.x = (p1.x == p2.x) ? p2.x : p2.x + ((wMin.y - p2.y) * m.d + m.e/2) / m.e;
      break;

   case TOP:
      // m.e won't be zero, otherwise "cross" would have returned FALSE
      pt.y = wMax.y;
      pt.x = (p1.x == p2.x) ? p2.x : p2.x + ((wMax.y - p2.y) * m.d + m.e/2) / m.e;
      break;
   }
   return (pt); 
}


/*===========================================================================
  Helper Function: clipPoint()
===========================================================================*/
static void clipPoint(AEEPoint p, int bMarked[], int b, 
                      AEEPoint wMin, AEEPoint wMax,
                      AEEPoint *pOut, int *cnt, AEEPoint first[], AEEPoint *last)
{
   AEEPoint pt;
   
   /* If no previous point exists for this edge, save this point. */
   if (!bMarked[b]) {
      bMarked[b] = 1;
      first[b] = p;
   }
   else
   /* Previous point exists.  If 'p' and previous point cross edge,
   find intersection.  Clip against next boundary, if any.  If
   no more edges, add intersection to output list. */
   if (cross(p, last[b], b, wMin, wMax)) {
      pt = intersect(p, last[b], b, wMin, wMax);
      if (b < TOP)
         clipPoint(pt, bMarked, b+1, wMin, wMax, pOut, cnt, first, last);
      else {
         pOut[*cnt] = pt;  
         (*cnt)++;
      }
   }
   
   last[b] = p;           /* Save 'p' as most recent point for this edge */
   
   /* For all, if point is 'inside' proceed to next clip edge, if any */
   if (inside(p, b, wMin, wMax)) {
      if (b < TOP)
         clipPoint(p, bMarked, b+1, wMin, wMax, pOut, cnt, first, last);
      else {
         pOut[*cnt] = p;  
         (*cnt)++;
      }
   }
}


/*===========================================================================
  Helper Function: closeClip()
===========================================================================*/
static void closeClip(AEEPoint wMin, AEEPoint wMax, AEEPoint *pOut,
                      int *cnt, AEEPoint first[], int bMarked[], AEEPoint *last)
{
   AEEPoint p;
   int b; // boundary of the rectangle
   
   for (b = LEFT; b <= TOP; b++) {
      if (cross(last[b], first[b], b, wMin, wMax)) {
         p = intersect(last[b], first[b], b, wMin, wMax);
         if (b < TOP)
            clipPoint(p, bMarked, b+1, wMin, wMax, pOut, cnt, first, last);
         else {
            pOut[*cnt] = p;  
            (*cnt)++;
         }
      }
   }
}


/*===========================================================================
  Helper Function: clipPolygon()
===========================================================================*/
static int clipPolygon(AEEPoint wMin, AEEPoint wMax, int n, 
                       const AEEPoint *pIn, AEEPoint *pOut)
{
   /* 'first' holds pointer to first point processed against a clip
   edge.  's' holds most recent point processed against an edge */
   AEEPoint first[N_EDGE], last[N_EDGE];
   int bMarked[N_EDGE];
   int i, cnt = 0;

   ZEROAT(&bMarked);
   ZEROAT(&first);

   for (i = 0; i < n; i++) 
      clipPoint(pIn[i], bMarked, LEFT, wMin, wMax, pOut, &cnt, first, last);
   closeClip(wMin, wMax, pOut, &cnt, first, bMarked, last);

   return cnt;
}


/////////////////////////////////////////////////////////////////////
//    SUTHERLAND-HODGEMAN POLYGON CLIPPING ALGORITHM END
/////////////////////////////////////////////////////////////////////


#include "AEEModTable.h"
#include "AEEGraphics_BREW.bid"

extern const AEEStaticClass gascAEEGraphics[] = {
   {AEECLSID_GRAPHICS_BREW, ASCF_UPGRADE, 0, 0, AEEGraphics_New},
   NULL
};
