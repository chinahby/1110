#ifndef MP4BUF_H
#define MP4BUF_H
/*=============================================================================
            mp4buf.h

DESCRIPTION:
   This file provides necessary buffer structures for MPEG 4 video.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

============================================================================*/
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/transform/mp4_dsp/rel/2.0/inc/mp4buf.h#3 $
$DateTime: 2010/05/19 05:37:12 $
$Change: 1304658 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include <comdef.h>


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*
** Define common video resolutions
*/
#define MP4_SQCIF_WIDTH           128
#define MP4_SQCIF_HEIGHT           96
#define MP4_QCIF_WIDTH            176
#define MP4_QCIF_HEIGHT           144
#define MP4_CIF_WIDTH             352
#define MP4_CIF_HEIGHT            288
#define MP4_ONEEIGHTH_VGA_WIDTH   240
#define MP4_ONEEIGHTH_VGA_HEIGHT  160
#define MP4_ONEQUARTER_VGA_WIDTH  320
#define MP4_ONEQUARTER_VGA_HEIGHT 240
#define MP4_WQVGA_WIDTH           400
#define MP4_VGA_WIDTH             640
#define MP4_VGA_HEIGHT            480
#define MP4_WVGA_WIDTH            800 
#define MP4_WVGA_HEIGHT           480
#define MP4_240_WIDTH             240
#define MP4_192_HEIGHT            192


/* DSP only has restriction on image width since it is used for image decoding
 * prediction algoritioms.  Maximum DSP width is currently 320.  Only other
 * restriction is the total buffer size or resolution. 
 */
#ifdef T_MSM7500
#error code not present
#else
  #ifdef FEATURE_DISABLE_QVGA_DECODE
    #define MP4_MAX_DSP_WIDTH         MP4_QCIF_WIDTH
  #elif defined (FEATURE_QTV_VGA_ENABLE)
    #define MP4_MAX_DSP_WIDTH         MP4_VGA_WIDTH
  #elif defined (FEATURE_QTV_WQVGA_ENABLE)
#error code not present
  #else
    #define MP4_MAX_DSP_WIDTH         MP4_ONEQUARTER_VGA_WIDTH
  #endif  /* FEATURE_DISABLE_QVGA_DECODE */
#endif /* T_MSM7500 */

#define MP4_SQCIF_RESOLUTION    (MP4_SQCIF_WIDTH * MP4_SQCIF_HEIGHT)
#define MP4_QCIF_RESOLUTION     (MP4_QCIF_WIDTH  * MP4_QCIF_HEIGHT)
#define MP4_CIF_RESOLUTION      (MP4_CIF_WIDTH   * MP4_CIF_HEIGHT)
#define MP4_ONEEIGHTH_VGA_RESOLUTION    (MP4_ONEEIGHTH_VGA_WIDTH * MP4_ONEEIGHTH_VGA_HEIGHT)
#define MP4_ONEQUARTER_VGA_RESOLUTION   (MP4_ONEQUARTER_VGA_WIDTH * MP4_ONEQUARTER_VGA_HEIGHT)

/* RGB bits/pixel */
#define MP4_RED_BPP             5
#define MP4_GREEN_BPP           6
#define MP4_BLUE_BPP            5

/* Maximum supported width and height */
#ifdef T_MSM7500
#error code not present
#else
  #ifdef FEATURE_DISABLE_QVGA_DECODE
    #define MP4_MAX_DECODE_WIDTH    MP4_QCIF_WIDTH
    #define MP4_MAX_DECODE_HEIGHT   MP4_QCIF_HEIGHT
  #elif defined (FEATURE_QTV_VGA_ENABLE)
    #define MP4_MAX_DECODE_WIDTH    MP4_VGA_WIDTH
    #define MP4_MAX_DECODE_HEIGHT   MP4_VGA_HEIGHT
  #elif defined (FEATURE_QTV_WQVGA_ENABLE)
#error code not present
  #else
    #define MP4_MAX_DECODE_WIDTH    MP4_ONEQUARTER_VGA_WIDTH
    #define MP4_MAX_DECODE_HEIGHT   MP4_ONEQUARTER_VGA_HEIGHT
  #endif  /* FEATURE_DISABLE_QVGA_DECODE */  
#endif /* T_MSM7500 */

#define MP4_RGB_BITS_PER_PIXEL  16
#define MP4_YUV_BITS_PER_PIXEL  12

/*
** Define common color order formats used in the mp4 buffer structure
*/
#define RGB_LENDIAN_ORDER        (0x0001)   /* little endian RGB color order */
#define RGB_BENDIAN_ORDER        (0x0000)   /* big endian RGB color order */

/*
** The following structure describes the buffer format for
** the device dependent bitmap formatted output video frame buffers.
*/
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef struct
{
  /*
  ** Video window or clipping window information
  **
  ** Note: Width and Height parameters specify the size of the output buffers
  ** passed to the video codec.  That is, the video codec shall clip the output
  ** frame to this windows size as specified by the FrameOrientation vol_setup
  ** parameter.
  ** (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT) < (Width*Height)
  */
  uint16   Width;      /* num output window/frame columns */
  uint16   Height;     /* num output window/frame rows */

  /*
  ** Video frame buffer color base information
  ** Note: current only 8bpp & 16bpp color depths with little endian
  ** ordering are supported.
  */
  uint16   RedBpp;        /* Number of bits for red color */
  uint16   GreenBpp;      /* Number of bits for green color */
  uint16   BlueBpp;       /* Number of bits for blue color */
  uint16   ColorOrder;    /* Defines how the output rgb buffer color */
  /* bits are ordered including the endianess */

#if !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK)
  /*
  ** Output RGB video frame buffers
  **
  ** RGB buffer size = Width*Height*(RedBpp+GreenBpp+BlueBpp)/8 (bytes)
  **    where: (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT) < (Width*Height)
  **    Note: data is always organized contiguously in these buffers.
  **          These buffers MUST be 32-bit aligned.
  **          These buffers must NOT cross a 24-bit boundary.
  */
  uint16 numRGB_buffers;
  void** pRGBBuf;
#endif /* !defined (FEATURE_QTV_MDP) || defined (PLATFORM_LTK) */

#if defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || \
    defined FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1
  /*
  ** Output YUV video frame buffers
  **
  ** YUV buffer size = Width*Height*12bpp/8 (bytes)
  **    where: (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT) < (Width*Height)
  **    Note: data is always organized contiguously in these buffers.
  **          These buffers MUST be 32-bit aligned.
  **          These buffers must NOT cross a 24-bit boundary.
  */
  uint16 numOutputYUV_buffers;
  void** pOutputYUVBuf;
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A || 
          FEATURE_VIDEO_PLAYER_INTERFACE_REV_C1 */

  /*
  ** Output YUV video frame buffers
  **
  ** YUV buffer size = Width*Height*12bpp/8 (bytes)
  **    where: (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT) < (Width*Height)
  **    Note: data is always organized contiguously in these buffers.
  **          These buffers MUST be 32-bit aligned.
  **          These buffers must NOT cross a 24-bit boundary.
  **
  ** Number of YUV buffers is dynamic, and determined at runtime by
  ** AllocateOutputBuffers()...there will probably be null pointers here!
  */
  uint16 numYUVBuffers;
  void** pYUVBuf;

#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_C
  /*
  ** Filtered YUV buffer (output of the deblocker)
  **
  **    where: (MP4_MAX_DECODE_WIDTH*MP4_MAX_DECODE_HEIGHT) < (Width*Height)
  **    Note: data is always organized contiguously in these buffers.
  **          These buffers MUST be 32-bit aligned.
  **          These buffers must NOT cross a 24-bit boundary.  
  */
  void     *pFilteredYUVBuf;
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_C */

#if defined (PLATFORM_LTK) && defined (FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A)
#error code not present
#endif /* PLATFORM_LTK && FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A */

#ifdef FEATURE_PALETTIZED_DISPLAY
  /*
  ** Palette index table
  **    This table is output display device dependent and informs the QDSP
  **    how to map its 12-bit color output to 8bpp for those devices.
  */
  uint8    PaletteIndex[4096];  /* mapping of 12bpp to 8bpp */
#endif
  uint16   OutputWidth;      /* num output window/frame columns */
  uint16   OutputHeight;     /* num output window/frame rows */
} MP4BufType;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* =======================================================================
**                        Class Declarations
** ======================================================================= */

#endif

