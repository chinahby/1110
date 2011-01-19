#ifndef OEMCOMMONDISP_H
#define OEMCOMMONDISP_H

#include "AEE.h"
#include "AEEDisp.h"

/*******************************
    OEM Display device Color Depth
********************************/

#define DISP_DEVICE_16BPP

/*******************************
    OEM Display Context 
********************************/

/*--- Graphics Context ---*/

typedef union
{
  struct {
    byte blue;
    byte green;
    byte red;
  } RGB;
  dword entry;
} OEMPaletteEntry;

/* Pen Type defines the pen style (pattern), and the width and color in 
   drawing graphics primitives. The pen is used for drawing lines and the
   outlines of filled shapes. The pen color is also used for drawing text. 
   The default color has all three components (red,green,and blue) set to 255
 */

typedef enum
{
   PEN_STYLE_NULL     = 0,    /* No border     */
   PEN_STYLE_SOLID    = 1,    /* Solid border  */
   PEN_STYLE_DASHED   = 2,    /* Dashed border */
   PEN_STYLE_DOTTED   = 3,    /* Dotted border */
   PEN_STYLE_MAX
} OEMPenStyle;


typedef enum
{
   PEN_SIZE_1        = 0,          /* 1 pixel border  */
   PEN_SIZE_2        = 1,          /* 2 pixels border */
   PEN_SIZE_MAX
} OEMPenSize;

typedef struct
{
   OEMPenStyle   style;   /* Pen Style */
   OEMPenSize    size;    /* Pen Size  */
   uint32        color;   /* Pen Color */
} OEMPen;


/* Brush defines the fill and background color to fill a closed primitive,
   such as a rectangle. */
typedef enum
{
   BRUSH_NULL      = 0,  /* No color inside the closed primitive */
   BRUSH_SOLID     = 1,  /* Entire primitive filled with a specific color */
   BRUSH_STYLE_MAX
} OEMBrushStyle;

typedef struct
{
   OEMBrushStyle style;   /* Brush Style */
   uint32        color;   /* Brush color */
} OEMBrush;


/*--- Text Context ---*/

/* Text justification type defines a justification setting for text. 
   The horizontal justification can be left, right, center,
   and the vertical justification can be top, bottom, baseline */
typedef enum
{
   JUSTIFY_LEFT     = 0,   /* Left alignment        */
   JUSTIFY_RIGHT    = 1,   /* Right alignment       */
   JUSTIFY_CENTER   = 2,   /* Horizontally centered */
   JUSTIFY_HORZ_MAX
} OEMTextHJust;

/* Vertical justification type */
typedef enum
{
   JUSTIFY_TOP        = 0,  /* Top justification                        */
   JUSTIFY_BOTTOM     = 1,  /* Bottom justification                     */
   JUSTIFY_CENTERLINE = 2,  /* Baseline (center) vertical justification */
   JUSTIFY_VERT_MAX
} OEMTextVJust;


typedef struct
{
   OEMTextHJust  HorzJustify;
   OEMTextVJust  VertJustify;
} OEMTextJust;

/* Font/character data types used by Console Font Translator tool */
typedef struct
{
   unsigned short value;
   unsigned long width;    /* Width of the character in pixels */
   long offset_leading;    /* Leading horizontal offset of the 
                              character in pixels ** may be negative */
   long offset_trailing;   /* Trailing horizontal offset of the 
                              character in pixels ** may be negative*/
   unsigned char *data;    
}OEMFontChar;

/* Font type defines the font used in displaying text . */
typedef struct
{
   long ascent;                 /* The font ascent (units above the base line)
                                   ** Usually the height of the letter 'A' or 'H' */
   long descent;                /* The font descent (units below the base line)
                                   ** Usually the overhang of the letter 'g' */
   unsigned long (*get_char_width)(unsigned short character); /* Function returns the width 
                                                                 of the specified character */
   long (*get_char_offset_leading)(unsigned short character); /* Function returns the leading
                                                                 horizontal offset in pixels 
                                                                 of the character. */
   long (*get_char_offset_trailing)(unsigned short character);/* Function returns the trailing
                                                                 horizontal offset in pixels 
                                                                 of the character. */
   const unsigned char *(*get_char_data)(unsigned short character, unsigned long *image_width);
   /* Function returns the block of data used to represent the character.
   ** The pointer returned must be byte aligned, and the data must be 
   ** formatted as follows:
   ** One bit representing a single pixel of the character from left to right 
   ** for the width of the character, followed by (image_width - char width),
   ** bits of padding from the top row of the image through the bottom row of the image
   ** Zero (0) represents a background color bit
   ** One (1) represents a foreground color bit
   ** This data block must contain at least (get_char_width x get_char_height) bits
   ** Used for drawing purposes.
   */
  /* This is the ative color table */
  RGBVAL             clrs[MAX_ACTIVE_COLOR + 1];
} OEMFontInfo;

/* IDs for built-in fonts */
typedef enum font_id_struct
{
   FONT_CURRENT,                                  /* Current process font */
   FONT_SYSTEM,                                   /* Default system font */
   FONT_MSM_SYSTEM,                               /* MSM system font.*/
   FONT_JAPANESE,                                 /* Japanese font */
   FONT_COUNT                                     /* Number of supported fonts */
} OEMFontId;

/* Transparency type specifies the background mix. It is used with text 
   and non-solid pen types. The background mode can either be transparent,
   i.e. the background remains unchanged, or it is filled with the current
   background color as specified by the brush before the non-solid pen or
   text is drawn.
*/
typedef enum
{
   TRANSPARENCY_OPAQUE      = 0, /* Background is filled with current 
                                    background color as determined by 
                                    the brush color, before the text 
                                    or non-solid pen is drawn  */
   TRANSPARENCY_TRANSPARENT = 1, /* Background is unchanged */
   TRANSPARENCY_MAX
} OEMTransparency;

/* Polygon filling rule type specifies which of the two rules are
   used to fill a polygon: an even-odd rule or non-zero winding rule
*/
typedef enum
{
   POLYFILL_EVEN_ODD = 0, /* Even-odd (or Aleternate) polygon filling rule */
   POLYFILL_WINDING  = 1, /* Non-zero winding polygon filling rule */
   POLYFILL_RULE_MAX
} OEMFillRule;

/* A point is expressed as (x,y), where x increases toward the right and 
   y increases toward the bottom */
typedef struct
{
   uint32 x;    /* x coordinate */
   uint32 y;    /* y coordinate */
} OEMPoint;

/* The rectangle is expressed by the two-dimensional coordinate of the
   pixel at the top-left corner of the rectangle to draw and the pixel
   at the bottom-right corner of the rectangle to draw. */
typedef struct
{
   OEMPoint  ulhc;  /* Upper left hand corner of the rectangle  */
   OEMPoint  lrhc;  /* Lower right hand corner of the rectangle */
} OEMRect;


/********************************************************************
    GRAPHIC CONTEXT:
    ----------------
        This structure containing all the attributes needed by the 
        graphics drawing functions, whether or not the drawing is done to
        the physical display device or onto a destination image. 
*********************************************************************/
typedef struct
{
   OEMPen           pen;              /* default is PEN_SIZE_1 (width) and 
                                         PEN_STYLE_SOLID (style) and default pen
                                         color is black */
   OEMBrush         brush;            /* default is BRUSH_NULL */
   OEMFontInfo      *pFont;           /* default ID is FONT_SYSTEM */
   OEMTextJust      text_just;        /* default is JUSTIFY_LEFT for horizontal 
                                         justification, and JUSTIFY_TOP for 
                                         vertical justification */
   OEMTransparency  transparency;     /* background mix mode, the default 
                                         mode is TRANSPARENCY_OPAQUE, i.e. the 
                                         brush color is used for background */
   OEMFillRule      polyfill_rule;    /* default is POLYFILL_EVEN_ODD */
   OEMPoint         current_position; /* default is (0,0) */
   OEMPoint         dev_disp_offset;  /* offset from the device display origin 
                                         (screen) that will be used for drawing */
   AEERasterOp      raster_op;        /* Specifies pixel logic, used by IMG only. 
                                         Default is ROP_COPY */
} OEMGraphicsContext;


/*********************************************
    DISPLAY-DEVICE SPECIFIC INFORMATION
**********************************************/    

/* Definition of the possible Bits Per Pixel color depths */
typedef enum 
{
   OEMDISP_8BPP,  /* 8 bits per pixel, or one byte per word16  */
   OEMDISP_16BPP  /* 16 bits per pixel, or 2 bytes per word16 */
} OEMBPP;

/* Definition of the possible pixel orderings within a byte for the Display */
typedef enum
{
   PIX_LITTLE_ENDIAN_BYTES = 0,    /* Leftmost pixel is at bit 0 */
   PIX_BIG_ENDIAN_BYTES = 1        /* Leftmost pixel is at bit 7 */
} OEMPixelOrder;

typedef struct
{
   uint16          width;
   uint16          height;
   OEMBPP          bpp;
   boolean         palette_support;
   boolean         contrast_support;
   byte            contrast_min;
   byte            contrast_max;
   byte            contrast_default;
   boolean         backlight_support;
   byte            backlight_min;
   byte            backlight_max;
   byte            backlight_default;
}  OEMDispInfo;

typedef struct
{
   uint32 width;      /* Number of pixels in the horizontal direction */
   uint32 height;     /* Number of pixels in the vertical direction */
} OEMRectSize;


/***************************************
    COLOR CONTEXT
****************************************/

/* the collor ordering conforms to RGBVAL as defined in AEEBitmap */
typedef union
{
   struct
   {
      uint8   alpha;
      uint8   red;
      uint8   green;
      uint8   blue;
   }RGB;
   uint32  RGB_word32;
} OEMRGB;
#ifdef DISP_DEVICE_16BPP
/* Definition of a 32-bit word containing a 16-bit filler and the
   RGB bits in the least significant 16 bits, as GGGBBBBBRRRRRGGG */
typedef struct
{
   uint16   filler;
   uint16   color16;
} OEMNativeColor16;
#else
/* Definition of a 32-bit word containing a 24-bit filler and the
   RGB bits in the least significant 8 bits */
typedef struct
{
   uint16   filler1;
   uint8    filler2;
   uint8    color8;
} OEMNativeColor8;
#endif

#ifdef DISP_DEVICE_16BPP
// Macro to translate given 8-bit values for R, G, and B to a R5G6B5 16-bit value
#define TOUCAN_RGB_TO_NATIVE(r, g, b)         \
  ( (roundTo5[r] << 11) |  \
    (roundTo6[g] <<  5) |  \
    (roundTo5[b]      )         )

// JTL: Check this out -- conflicts with OEMBitmap16.h
// Macro to translate given a 16-bit RGB (5,6,5) to 32-bit RGB
#define TOUCAN_NATIVE_TO_RGB(clr16)                 \
  ( ( (clr16 & 0xF800)       ) | \
    ( (clr16 & 0x07E0) << 13 ) | \
    ( (clr16 & 0x001F) << 27 )   )

#elif defined DISP_DEVICE_8BPP
// Macro to translate given 8-bit values for R, G, and B to a B2G3R3 8-bit value
#define TOUCAN_RGB_TO_NATIVE(r, g, b)        \
  ( (roundTo3[r]     ) |  \
    (roundTo3[g] << 3) |  \
    (roundTo2[b] << 6)         )

// Macro to translate given a 8-bit RGB (B2,G3,R3) to 32-bit RGB
#define TOUCAN_NATIVE_TO_RGB(clr8)               \
  ( ( (clr8 & 0x07) << 13 ) | \
    ( (clr8 & 0x38) << 18 ) | \
    ( (clr8 & 0xC0) << 24 )   )

#endif /* DISP_DEVICE_16BPP */

/**********************************************
   VARIABLES
***********************************************/

extern const unsigned long int system_palette[];
extern uint16 gPalette8To16Tbl[256];
#ifdef DISP_DEVICE_16BPP
extern uint8  roundTo5[256];
extern uint8  roundTo6[256];
#elif defined DISP_DEVICE_8BPP
extern uint8  roundTo2[256];
extern uint8  roundTo3[256];
#endif /* DISP_DEVICE_8BPP */

/**********************************************
   PROTOTYPES
***********************************************/
// Prototypes for OEMFont.c 
extern int OEMFont_GetSystemFont(AEEFont nFont, IFont **ppif);
extern int OEMFont_SetSystemFont(OEMFontId FontId, OEMFontInfo *pFontInfo);

#ifdef DISP_DEVICE_16BPP
  typedef word OEMPixel;
  #define OEM_PIXEL_SIZE 2
  #define OEM_COLOR_DEPTH 16
#elif defined DISP_DEVICE_8BPP /* DISP_DEVICE_8BPP */
  typedef byte OEMPixel;
  #define OEM_PIXEL_SIZE 1
  #define OEM_COLOR_DEPTH 8
#endif /* DISP_DEVICE_16BPP */
#endif // OEMCOMMONDISP_H
