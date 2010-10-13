#ifndef JPEGI_H
#define JPEGI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)
               I N T E R N A L   H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains the internal definitions for the Joint
  Photographers Expert Group (JPEG) decoder and encoder.


Copyright(c) 2002-2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/common/main/latest/inc/jpegi.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/08   kdiv    ULC Write back cache changes
08/13/07   arv     Added scalable jpeg decoder under FEATURE_JPEGD_RESIZE  
06/25/07   arv     Added support for MediaFlo service icons
04/06/06   arv     Added SOS invalid table selector error status message
03/17/05   dl      Use cache or cache lines based on supported cache.
03/13/06   vma     Fixed all Lint errors
01/24/06   arv     Added JPEGD_END_OF_IMAGE to status messages
11/28/05   arv     Fixed Lint errors - Phase 1
09/16/05   pdo     Included JPEGD_INITIALIZESCAN for jpeg internal reference 
08/24/05   arv     Restructured the ARM based JPEG Decoder
06/30/05   dl      Added jpeg_decode_status_type and output ptr.
11/30/04   pdo     Increased MAX_WIDTH, MAX_HEIGHT and the related params. 
05/17/04   pdo     Added JPEG_FORMAT, JPEG_LENGTH and other constants for
                   the jpeg_extract_thumbnail  
04/19/04   pdo     Increased input buffer for dynamic allocation
03/22/04   st      Added JPEG_BUFFER_RECEIVED and JPEG_INIT_READ_DONE states.
02/10/04   pdo     Increased image heigh for the portrait of 1.3MP
09/30/03   pdo     Support image filesize up to 1280x1024, provided enough
                   heap memory is available.Removed qdsp_reset() to avoid 
				   un-necessary DSP image swap.
				   Needed new JPEG decoder DSP image.
06/01/03   pdo     Redesigned JPEG decoder to support Progressive mode
11/29/02   sa      Removed various MCU definitions previously required by
                   decoder.
10/10/02   dle     Modified WIDTH to 176 and HEIGHT to 144.
04/17/02   sa      Set both MAX_IMAGE_WIDTH and _HEIGHT to 160.  Also redefined
                   MAX_IO_BUF_1_SIZE.  Removed unused defines.
03/24/02   sa      Corrected MAX_TOTAL_MCU_RGB to reflect MCU width of 8 and
                   not 16.
03/22/02   sa      Added #defines for additional parameter checks in decoder
03/22/02   sa      Initial version

===========================================================================*/
#include "target.h"             /* target specific definitions             */
#include "comdef.h"             /* customer specific definitions           */
#include "cmx.h"                /* CMX specific definitions                */
#include "qdsp.h"

#ifdef CACHE_POLICY_WRITE_BACK
#ifndef JPEG_DATA_CACHE_LINES
#define JPEG_DATA_CACHE_LINES
#endif //JPEG_DATA_CACHE_LINES
#define CACHE_LINE_SIZE         32
#endif //CACHE_POLICY_WRITE_BACK

#ifdef JPEG_DATA_CACHE_LINES
  #include "cache_mmu.h"
#else
  extern void mmu_invalidate_data_cache(void);
#endif


#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)

#ifndef max
#define max(a,b) (((a)>(b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif

/* Image width and height can be set up to 640x480 if RAM available        */
#define MAX_IMAGE_WIDTH       176  /* max width can be set up to 640       */
#define MAX_IMAGE_HEIGHT      144  /* max height can be set up to 480      */
#define MAX_MCU_HEIGHT        16   /* Max for RGB/YUV. DO NOT CHANGE       */
#define MAX_MCU_WIDTH         16   /* Max for RGB/YUV. DO NOT CHANGE       */
#define MAX_COMP_PER_PIXEL    3    /* components per pixel.  DO NOT CHANGE */
#define MAX_MCU_PER_DSP_WRITE 4    /* DO NOT CHANGE. Can be no more than 4 */
#define MAX_IO_BUF_1_SIZE     (MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * MAX_COMP_PER_PIXEL)
#define MAX_IO_BUF_2_SIZE     5000 /*based on JPEG input/output DSP buffers*/
#define MAX_MCU_BUF_SIZE      (MAX_MCU_PER_DSP_WRITE * MAX_MCU_HEIGHT * MAX_MCU_WIDTH * MAX_COMP_PER_PIXEL)
#define MAX_JPEG_BUFFER_SIZE  (MAX_IO_BUF_1_SIZE + MAX_IO_BUF_2_SIZE + MAX_MCU_BUF_SIZE)

/* various states used to initialize JPEG encoder and decoder              */
typedef enum
{
  JPEG_STATE_INIT,
  JPEG_QDSP_READY,
  JPEG_WRITE_DATA,
  JPEG_INIT_READ_DONE,
  JPEG_CHECK_HEADER,
  JPEG_BUFFER_RECEIVED,
  JPEG_DECODE_SCAN,
  JPEG_IMAGE_DECODE_DONE,
} jpeg_state_type;

typedef enum
{
  DECODE_PARTIAL_IMAGE_DONE = -1,
  DECODE_ENTIRE_IMAGE_DONE = 1,
} jpeg_decode_status_type;

#ifdef JPEG_DATA_CACHE_LINES
  #define JPEG_INVALIDATE_DATA_CACHE(x,y) INVALIDATE_DATA_CACHE(x,y)
  #define JPEG_FLUSH_DATA_CACHE(x,y)      FLUSH_DATA_CACHE(x,y)
#else
  #define JPEG_INVALIDATE_DATA_CACHE(x,y) mmu_invalidate_data_cache()
#endif

#ifdef FEATURE_JPEG_DECODER
//#define JPEG_DECODE_CMD       0x0500
#endif

#ifdef FEATURE_JPEG_ENCODER
//#define JPEG_ENCODE_CMD       0x0600
#endif

//------------------------------------------------------------------------------
#define MAX_DATA_BYTES       10240
//------------------------------------------------------------------------------
// May need to be adjusted if support for other colorspaces/sampling factors is added
#define JPGD_MAXBLOCKSPERMCU 10
//------------------------------------------------------------------------------
#define JPGD_MAXHUFFTABLES   8
#define JPGD_MAXQUANTTABLES  4
#define JPGD_MAXCOMPONENTS   4
#define JPGD_MAXCOMPSINSCAN  4
//------------------------------------------------------------------------------

#define JPGD_MAXBLOCKSPERROW 1200

//#define JPGD_MAXBLOCKSPERCOMP 320
//------------------------------------------------------------------------------
// Max. allocated blocks
#define JPGD_MAXBLOCKS    100
#define JPGD_BLOCKSIZE     64
//------------------------------------------------------------------------------
// JPEG MAX Image Dimensions
#define JPGD_MAX_HEIGHT 3800
#define JPGD_MAX_WIDTH  3800

// JPEG MIN Scaled Width
#define JPGD_MIN_SCALED_WIDTH   32
#define JPGD_MIN_SCALED_HEIGHT  32

//------------------------------------------------------------------------------

// Support for MediaFlo Service icons - max support for wqvga
#ifdef JPEGD_MFLO_SVC_ICON_SUPPORT
#error code not present
#endif

/* JPEG specific errors */

typedef enum {
JPEGD_DECODING_OK                    =    1,
JPEGD_DECODING_INIT                    =    0,
JPEGD_DECODING_ERROR                  =  -100,
JPEGD_NO_ARITHMETIC_DECODING_SUPPORT  =  -101, 
JPEGD_NOT_SUFFICIENT_MEMORY           =  -102, 
JPEGD_INVALID_WIDTH                   =  -103,
JPEGD_INVALID_HEIGHT                  =  -104,
JPEGD_UNSUPPORTED_MARKER              =  -105,
JPEGD_UNEXPECTED_MARKER               =  -106,
JPEGD_INVALID_SOI_MARKER              =  -107,
JPEGD_INVALID_SOF_LENGTH              =  -108,
JPEGD_UNSUPPORTED_PRECISION_IN_SOF    =  -109,
JPEGD_INAVLID_LENGTH_IN_SOS           =  -110,
JPEGD_INVALID_COMPID_IN_SOS           =  -111, 
JPEGD_INVALID_COMPONENTS_IN_SOS       =  -112,
JPEGD_INVALID_SPECTRAL_IN_SOS         =  -113,
JPEGD_INVALID_SUCC_APPROX_IN_SOS      =  -114, 
JPEGD_INVALID_DHT_COUNT               =  -115,
JPEGD_INVALID_DHT_INDEX               =  -116,
JPEGD_INVALID_DHT_MARKER              =  -117,
JPEGD_INVALID_DQT_MARKER              =  -118, 
JPEGD_INVALID_DQT_TABLE               =  -119,  
JPEGD_INVALID_DQT_LENGTH              =  -120,
JPEGD_INVALID_PRECISION               =  -121,
JPEGD_INVALID_VARIABLE_MARKER         =  -122, 
JPEGD_UNSUPPORTED_COLORSPACE          =  -123,
JPEGD_INVALID_NUMBER_OF_COMPONENTS    =  -124,
JPEGD_INVALID_DRI_MARKER              =  -125,
JPEGD_INVALID_DRI_LENGTH              =  -126,
JPEGD_UNSUPPORTED_SAMPLING_FACTOR     =  -127,
JPEGD_UNSUPPORTED_SOF_MARKER          =  -128,
JPEGD_INVALID_SOF_MARKER              =  -129,
JPEGD_INVALID_DHT_TABLE               =  -130,
JPEGD_NOT_JPEG_IMAGE                  =  -131,
JPEGD_INVALID_RESTART_MARKER          =  -132,
JPEGD_ERROR_INTERPRETING_MARKER       =  -133,
JPEGD_INVALID_MCU_ROW_SIZE            =  -134,
JPEGD_INVALID_MCU_COL_SIZE            =  -135,
JPEGD_INVALID_DHT_MARKER_LENGTH       =  -136,
JPEGD_INVALID_HUFFMAN_CODE            =  -137,
JPEGD_INVALID_SAMPLING_FACTOR         =  -138,
JPEGD_ERROR_CALCULATING_MCU           =  -139,
JPEGD_INVALID_FILE_SIZE              =  -140, 
JPEGD_END_OF_IMAGE                    =  -141,
JPEGD_INVALID_TABLE_SELECTOR_IN_SOS   =  -142
} JPEGD_STATUS_MSG;
//------------------------------------------------------------------------------
#define JPGD_GRAYSCALE 1
#define JPGD_YH1V1     2
#define JPGD_YH2V1     3
#define JPGD_YH1V2     4
#define JPGD_YH2V2     5

#define OUTPUT_PER_PIXEL  3
#define JPEG_FORMAT  0x0201
#define JPEG_LENGTH  0x0202
#define ENTRY_SIZE   12

//------------------------------------------------------------------------------
typedef enum
{
   JPGD_FAILED,
   JPGD_DONE,
   JPGD_OKAY
} jpeg_return_type;

//------------------------------------------------------------------------------
typedef enum
{
  M_SOF0  = 0xC0,
  M_SOF1  = 0xC1,
  M_SOF2  = 0xC2,
  M_SOF3  = 0xC3,

  M_SOF5  = 0xC5,
  M_SOF6  = 0xC6,
  M_SOF7  = 0xC7,

  M_JPG   = 0xC8,
  M_SOF9  = 0xC9,
  M_SOF10 = 0xCA,
  M_SOF11 = 0xCB,

  M_SOF13 = 0xCD,
  M_SOF14 = 0xCE,
  M_SOF15 = 0xCF,

  M_DHT   = 0xC4,

  M_DAC   = 0xCC,

  M_RST0  = 0xD0,
  M_RST1  = 0xD1,
  M_RST2  = 0xD2,
  M_RST3  = 0xD3,
  M_RST4  = 0xD4,
  M_RST5  = 0xD5,
  M_RST6  = 0xD6,
  M_RST7  = 0xD7,

  M_SOI   = 0xD8,
  M_EOI   = 0xD9,
  M_SOS   = 0xDA,
  M_DQT   = 0xDB,
  M_DNL   = 0xDC,
  M_DRI   = 0xDD,
  M_DHP   = 0xDE,
  M_EXP   = 0xDF,

  M_APP0  = 0xE0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xEF,

  M_JPG0  = 0xF0,
  M_JPG13 = 0xFD,
  M_COM   = 0xFE,
  M_JPGM  = 0xFF,

  M_TEM   = 0x01,

  M_ERROR = 0x100
} JPEG_MARKER;
//------------------------------------------------------------------------------
#define RST0 0xD0
//------------------------------------------------------------------------------


typedef struct
{
  uint32  look_up[256];
  uint8   code_size[256];
  uint32  tree[512];
} Phuff_tables_t;


/*---------------------BEGIN IDCT---------------------------------------------*/
#define CONST_BITS  13
#define PASS1_BITS  2
#define SCALEDONE ((int32) 1)
/*----------------------------------------------------------------------------*/
#define FIX_0_298631336  ((int32)  2446)        /* FIX(0.298631336) */
#define FIX_0_390180644  ((int32)  3196)        /* FIX(0.390180644) */
#define FIX_0_541196100  ((int32)  4433)        /* FIX(0.541196100) */
#define FIX_0_765366865  ((int32)  6270)        /* FIX(0.765366865) */
#define FIX_0_899976223  ((int32)  7373)        /* FIX(0.899976223) */
#define FIX_1_175875602  ((int32)  9633)        /* FIX(1.175875602) */
#define FIX_1_501321110  ((int32)  12299)       /* FIX(1.501321110) */
#define FIX_1_847759065  ((int32)  15137)       /* FIX(1.847759065) */
#define FIX_1_961570560  ((int32)  16069)       /* FIX(1.961570560) */
#define FIX_2_053119869  ((int32)  16819)       /* FIX(2.053119869) */
#define FIX_2_562915447  ((int32)  20995)       /* FIX(2.562915447) */
#define FIX_3_072711026  ((int32)  25172)       /* FIX(3.072711026) */
/*----------------------------------------------------------------------------*/
#define DESCALE(x,n)  (((x) + (SCALEDONE << ((n)-1))) >> n)
/*----------------------------------------------------------------------------*/
#define MULTIPLY(var,cnst)  ((var) * (cnst))
/*---------------------END IDCT-----------------------------------------------*/
#define QUANT_DESC int16
#define DATAUNIT_DESC int16

/* internal definition for AC and DC component decoding */
typedef void (*dataUnit_decode_func_ptr_type) (
int32 component_id,
int32 dataUnit_x,
int32 dataUnit_y
);

/* Definition for component data structure */
typedef struct
{
   uint32 comp_h_samp;
   uint32 comp_v_samp;
   int32  comp_quant;
   int32  comp_dc_tab;
   int32  comp_ac_tab;
   uint32 dataUnit_num_x;
   uint32 dataUnit_num_y;
   uint32 dataUnit_x_mcu;
   uint32 dataUnit_y_mcu;
   uint32 coeff_size;
   uint32 last_dc_val;
   DATAUNIT_DESC  *seg_ptr;
} comp_data_type;



/* Definition for DSP configuration data structure */
typedef struct
{
   uint16   *RGBBuf1Ptr;        /* Output buffer1 pointer for display */
   uint16   *RGBBuf2Ptr;        /* Output buffer2 pointer for display */
   int16    *coefBuf1PtrSet[3]; /* pointers to DCT coefficients   */
   int16    *coefBuf2PtrSet[3]; /* pointers to DCT coefficients   */
   uint16   *lumaQuantTab;      /* Luma table address             */
   uint16   *chromaQuantTab;    /* Chroma table address           */
   uint16    imageWidth;        /* image width in multiple of 16  */
   uint16    imageHeight;       /* image height in multiple of 16 */
   uint16    outFormat;         /* out format RGB888 or RGB565    */
   uint16    samplingFactor;         /* number of color components     */
} JPG_PD_config_struct;

/* Definition for DSP command data */
typedef struct
{
   uint16    inputBufIndex;     /* Which input coefficient buffer to use */
   uint16    startMCU;          /* Starting number of MCU          */
   uint16    MCU_count;         /* number of MCUs in this session  */
   uint16    samplingFactor;    /* either H1V1, H1V2, H2V1 or H2V2 */
   uint8    *output1_ptr;        /* Output buffer1 pointer for display */
   uint8    *output2_ptr;        /* Output buffer2 pointer for display */
} JPG_PD_command_struct;

/*
** Define common CAMERA QDSP return code enumeration
*/
typedef enum
{
   JPGPD_SUCCESS          = 0, /* cmd sucessful */

   /* Download done return codes */
   JPGPD_QDSP_DOWNLD_FAIL,     /* JPEG image download failed */
   JPGPD_INVALID_STATE,
   JPGPD_QDSP_BUSY,

   /* QDSP command error return codes */
   JPGPD_INVALID_CMD,          /* invalid QDSP command specified */
   JPGPD_INVALID_PARM,         /* invalid QDSP command parameter */
   JPGPD_MAX_RETURN_CODES
} JPGPD_returnCodeType;

/*
** Define different type of commands
*/
typedef enum
{
   JPGPD_SLEEP_CMD   = 2,
   JPGPD_IDLE_CMD    = 7,     /* Command the DSP into IDLE state */
   JPGPD_CONFIG_CMD  = 0x500,
   JPGPD_DECODE_CMD  = 0x501, /* JPEG decode command  */
} JPGPDCommandType;

/*===========================================================================
Function: JPEGD_INITIALIZESCAN

Description:  Initialize the Decoder Scan

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects: see jpeg.c module
============================================================================*/
JPEGD_STATUS_MSG jpegd_initializeScan(void);

#endif /* defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER) */
#endif /* JPEGI_H              */

