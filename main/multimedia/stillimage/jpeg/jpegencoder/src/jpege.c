/* linesize(132)
** pagesize(60)                                     
** title("Dual Mode Subscriber Station")
** subtitle("JPEG Encoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                      E N C O D E R  

GENERAL DESCRIPTION
  This module contains functions to encode and decode Joint Photographers  
  Expert Group (JPEG) image files for MSM6550.

EXTERNALIZED FUNCTIONS
  jpege_encode_exif
    This function is called to start the JPEG decoder
  jpege_encode_jfxx
  
  jpege_encode_file_size_exif
    This function is called to start the JPEG encoder using file
    size control rather than quality settings
  jpege_encode_file_size_jfxx
    This function is called to start the JPEG encoder using file
    size control rather than quality settings
  jpeg_get_image_specs
    This function is called to extract the header information from the
    JPEG image
  jpeg_abort_decode
    This function is called to end the current JPEG decoder session 
  jpeg_encode
    This function is called to start the JPEG encode 
  jpeg_abort_encode
    This function is called to end the current JPEG encoder session
  jpeg_set_buf
    This function is called by client to provide data memory for JPEG
    encoder and decoder.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2002-2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegencoder/main/latest/src/jpege.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/10  chai  fixed klock work errors
08/21/09   sv      Added EFS APIs by removing deprecated FS APIs.
01/05/09   kdiv    Make sure to abort the QDSP current session before termination (while the 
                      request comes from QDSP layers)
12/22/08   kdiv    Reverting the CL#770749
11/24/08   sigu    Fixed an issue for not waiting for IDLE response from JPEGD DSP.
02/15/08   kdiv    Fix CR 136640:Memory leak for exif_init_info in jpege_encode_jfxx
                   changes ported from 7k
12/20/07   yy      Fixed issue with encoding from Qalbum with non-multiple of 16 in dimension,
                   where padding is needed.
11/28/07   kgelli  Fixed Abort encoding in Qalbum by using clear key or end key.
11/27/07   yy      Fixed the issue with FS_write when flash is almost full.
                   Added check for data size being written to efs.
09/17/07   yy      Fixed memory leak inside file size control code, corner case
                   for when file size control failed.
08/16/07   yy      Fixed the divided by zero error in file size control code
07/13/07   yy      Fixed the malloc issue in file size control code
05/23/07   arv     Fixed the mobicat header issue in jpeg encoding
02/14/07   arv     Modified string libraries in accordance with security 
                   standards
09/14/06   vma     Fixed bug when encode abort is sent before DSP is downloaded
09/13/06   dl      Fixed JPEG encoding fail due to the overflow of 
                   acSizeLuma variable.
07/14/06   vma     Fixed bug in reporting encoded filesize in MEM mode
                   include thumbnail portion
06/07/06   vma     Fixed bug in reporting encoded filesize in Mem mode
05/02/06   dl      Added clockwise rotation support.
04/04/06   dl      Added API for user to request thumbnail width and height.
03/23/06   vma     Replaced all mallocs and frees to JPEG_MALLOC's and JPEG_FREE's
03/17/05   dl      Use cache or cache lines based on supported cache.
01/20/06   dl      Check the last byte of encode stream for padded data.
01/05/06   dl      Handle APP1 and APP7 size limitation gracefully.
10/31/05   kln     Disposed of code review comments
10/03/05   kln     Added a version for mobiCat marker
09/29/05   kln     Fixed lint erors
08/30/05   kln     Added a function to compute MCU padded image dimensions
08/17/05   kln     Fixed a problem related to image padding
07/29/05   kln     Exposed mobicat stream pointer and length in bytes through
                   jpege_mobicat_insert_info() API
07/18/05   kln     Added API to build byte stream of sensor parameters structure
                   and insert it in JPEG stream
05/16/05   kln     Added functionality related to enhancing file size control
                   by looking ahead in teh image to be encoded
05/10/05   kln     Increased exif buffer size from 1200 to 1500 to accomodate 
                   potential user comment 
05/05/05   pdo     Replaced global cache disable with mmu...disable_lines()
04/28/05   kln     Fixed bug related to memset clearing filesize structure
03/22/05   kln     Added a check to remove JPEG encoded file if abort command  
03/22/05   arv     Featurizing the JPEG Encoder - FEATURE_JPEG_ENCODER_REV2
03/18/05   kln     Added support for > 2 passes for file size control
03/02/05   kln     Added support for mem mode file size control  with first pass 
                   being over the allocated buffer
02/28/05   kln     Added optimal support for H2V1 with odd rotation - capture 
                   in H1V1 and then subsample  to get odd rotated H2V1.
02/18/05   kln     Added function to vertically upsample CrCb 2D array
                   Modified file size control when scaling up, down , and when 
                   capping the quality
02/02/05   kln     Corrected lint errors and cleaned up file size control code
01/25/05   kln     Added additional checks for handling FS related problems 
                   when JPEG encode
                   Resolved file size control issues related to proper statistics
                   gathering.
                   Added file size related exits with proper status
                   Modified file size structure to handle arbitrary number of passes
01/24/05   pdo     Returned buffer pointer for the DEVICE_MEM mode encoding
01/10/05   mvd     Added support for File Size Control.
12/07/04   kln     Added function to upsample horizontally,
                   added H1V2 support
10/22/04   pdo     Changed JFIF interface and enabled parameters for 
                    file-size control.
10/19/04   pdo     Added support for JFIF encoder
10/01/04   pdo     Added debug messages, removed extra from DSP and 
                    removed extra JPEGENC_CBrtnType from API.
09/01/04   pdo     Added restart marker and thumbnail image
07/09/04   pdo     Initial version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */
#include "jpege.h"
#include "jpegi.h"
#include "jpegqdsp.h"
#include "stdlib.h"
#include "msg.h"                /* Message logging/reporting services      */
#include "graph.h"              /* Graphics task typedefs and prototype    */
#include "jpeg_malloc.h"
#include "fs_public.h"
#ifdef JPEG_USES_LIBSTD
#include "AEEstd.h"
#endif /* JPEG_USES_LIBSTD */

#ifdef FEATURE_NEW_SLEEP_API
#include "sleep.h"
#endif

#define jpeg_file_id MALLOC_JPEGE

#ifdef FEATURE_JPEG_ENCODER_REV2
#define OUTPUT_BUF_SIZE  8192   /* Pair of Ping Pong buffers of this size  */
#define JPEG_EXIF_HEADER_SIZE 2000   /* Enough to hold GPS and PIM data    */
#define JPEG_JFIF_HEADER_SIZE  600   
#define JPEGE_AVER_CHROMA(x1,x2) ((uint8)(((uint16)(x1)+(uint16)(x2)+1)>>1))
#define SCALE_FACTOR_50_PER_Q8 (1*256)
#define SCALE_FACTOR_1_PER_Q8  (int16)(0.02*256)
#define JPEGE_TURN_OFF_MAX_SIZE 0x7FFF

/* Set max allowed encoding passes */
#define JPEGE_FILE_SIZE_MAX_PASSES 2

/* Lowest and highest scale factors allowed for file size ctrl
   1% and 90% respectively.
   Apply this formula: scale factor = 50/(100-quality%)
   if quality is > 50 else scale factor = quality%/50         
   The given bellow are the scale factors to achieve 
   a spefified quality                                       */
#define ONE_PERCENT_SCALE_FACTOR          0.02
#define NINTY_EIGHT_PERCENT_SCALE_FACTOR 25.0
#define FIFTY_PERCENT_SCALE_FACTOR_Q8    256

/* Estimate of number of bits per 8x8 block for EOB
   ** This assumes default Huffman tables are used **
   2 bits for chrominance and 4 for luminance.
   For H2V2 (420) encoding:
   so: 4* num blocks + 2* (1/2 num blocks) = 5 bits/blk
   For H2V1 (422) or H1V2 encoding:
   so: 4* num blocks + 2* num blocks = 6 bits/blk */
#define END_OF_BLOCK_BITS_PER_8X8_420 5
#define END_OF_BLOCK_BITS_PER_8X8_422 6

/* Place an upper limit on the change in the scale factor from pass to
   pass by caping the K multiplication factor.  This is needed
   because of corner cases with very simple image subjects that can
   result in huge changes in K
   This 2 allows for (at most) a 25% increase in JPEG the quality 
   percent */
#define MAX_K_FACTOR_VALUE_Q8   (256*2)

/* File Size control enhancement related defines */
/*************************************************/
/* AC reference point scale  */
#define JPEGE_AC_SCALE 41

/* Golden curve coefficients */
#define JPEGE_FILE_SIZE_SQUARE_COEFF_Q8 (int32)(256*0.6869)
#define JPEGE_FILE_SIZE_LIN_COEFF_Q8    (int32)(256*1.5825)

/* Used to modify estimated file size curve */
#define JPEGE_FILE_SIZE_LOW_THRESHOLD   44
#define JPEGE_FILE_SIZE_HIGH_THRESHOLD  60

/* The polynomial coefficients to map from 
   entropy to bit rate (for DC and AC   */
#define JPEGE_FILE_SIZE_DC_1ST          497
#define JPEGE_FILE_SIZE_DC_2ND          (-84)
#define JPEGE_FILE_SIZE_DC_3RD          14
#define JPEGE_FILE_SIZE_AC_1ST          157
#define JPEGE_FILE_SIZE_AC_2ND          (-67)
#define JPEGE_FILE_SIZE_AC_3RD          10

/* Cap down AC entropy and size          */
#define JPEGE_FILE_SIZE_MIN_AC_ENTROPY  858
#define JPEGE_FILE_SIZE_MIN_AC_SIZE     11

/* Cap up file size correction based on 
   file size estimation                  */ 
#define JPEGE_FILE_SIZE_MAX_LIN_COEF_Q8 352
#define JPEGE_FILE_SIZE_MAX_SQ_COEF_Q8  512

/* Used to correct for picture taken from close distance */
#define JPEGE_FILE_SIZE_SC_LUMACHROMA   1920
#define JPEGE_FILE_SIZE_MAX_SC_LUMACHROMA 512

/* Default DC scale (first pass) */
#define SCALE_FACTOR_DC_ENH_Q8          768
#define JPEGE_MIN_NUM_BYTES_FOR_AC      500
#define JPEGE_MAX_FILE_SIZE_QUALITY_DEFAULT 90

/* Mid probability test for artificial images */ 
#define JPEGE_MID_PROB                  0.15

/* Max index to compute processed image histogram */
#define MAX_INDEX                       500
#define INDEX(i,j,J)                    ((i)*(J)+(j))

/* When scaling JPEG quant tables, some options exist
   in regard to the DC component quantization values */
typedef enum
{
  DO_NOT_SCALE_DC,                /* Keep quant. values as defaults   */
  SCALE_GREATER_QUALITY_ONLY_DC,  /* Scale down for greater qual only */
  SCALE_ALL_DC                    /* Scale up or down                 */
} dcScalingOptionsType;

typedef struct
{
  uint8  *buf_ptr;
  uint32  buf_size;
} storeHeaderType;

/* Enum states for the header emittion */
typedef enum
{
  UP_TO_THUMBNAIL,                /* Keep quant. values as defaults   */
  AFTER_THUMBNAIL,                /* Scale down for greater qual only */
  FULL_HEADER                     /* Scale up or down                 */
} emitHeaderType;

/* Enum state for header type */
typedef enum
{
  EXIF_JPEG_HEADER,               /* With EXIF JPEG header   */
  JFIF_JPEG_HEADER                /* Basic JFIF header       */
} JPEGENC_headerType;

/* Enum for the buffer holders */
typedef enum
{
  BUFFER_1,                       /* Use buffer1 to hold DSP info   */
  BUFFER_2                        /* Use buffer2 to hold DSP info   */
} bufferType;

/* Aggregate information regarding the encoding session */
struct
{
  /* Destination dimension */
  storeHeaderType       memData;         /* The header buffer - temporary */
  JPEGENC_handleType    dst_format;      /* Destination format */
  int                   file_descriptor; /* variable for EFS operations*/
  char *                filename;        /* File name */
  uint16                filename_len;    /* File name length */
  uint32                exif_data_size;  /* Destination buffer size */
  byte *                exif_buf;        /* Destination buffer */
  uint32                exif_buf_size;
  emitHeaderType        thumbnail;       /* header emit state */
} jpege_info;

/* ----------- Types and Variables for JPEG File Size Control --------------*/
/* ================= OEM AJUSTMENT PARAMETERS ============================= */
typedef struct
{
  /* Adjustment on file size target to account for algorithm error
     in prediction.  Number between 1 and 100.  May not be > 90. */
  uint16 targetScaleDownPercent;

  /* Computed as: (Pass 1 actual file size/max target file size)*100
     These are thresholds at varying resulting qualities.  Note that
     and Pass 1 image quality decreases, we raise the ratio threshold
     to make it more likely to encode Pass2 (reason is because for
     lower quality it is more urgent to increase quality by
     re-encoding */
  uint16 fsThresholds[10];
} fileSizeCfgParamsType;

fileSizeCfgParamsType fileSizeCfgParams =
{
  /* Error rate for two pass encoding is currently 10% */
  90,

  /* The following are in units of percent and represent
     thresholds or minimum acceptable sizes of the actual Pass 1
     encoding to the max target file size */
  {
    80,   /* for 0-10  percent quality */
    80,   /* for 11-20 percent quality */
    70,   /* for 21-30 percent quality */
    60,   /* for 31-40 percent quality */
    50,   /* for 41-50 percent quality */
    40,   /* for 51-60 percent quality */
    30,   /* for 61-70 percent quality */
    30,   /* for 71-80 percent quality */
    30,   /* for 81-90 percent quality */
    30    /* for 91-100 percent quality */
  }
};

/* File size enhancment related variables */
static boolean file_size_enhanced = TRUE;
static uint16 linCoeffAdjustQ8 = 0xFF;
static uint16 squareCoeffAdjustQ8 = 0xFF;
static uint16 goldCurveRateAt80PercentHighQ8 = 34;
static uint16 goldCurveRateAt80PercentLowQ8 = 26;

static int16 *dcPtrLuma = NULL;
static int16 *dcPtrCb = NULL;
static int16 *dcPtrCr = NULL;
static int16 *dcPtr1Luma = NULL;
static int16 *dcPtr1Cb = NULL;
static int16 *dcPtr1Cr = NULL;
static int16 *dcPtr2Luma = NULL;
static int16 *dcPtr2Cb = NULL;
static int16 *dcPtr2Cr = NULL;

static uint16 *jpegeFileSizeEstiBufPtr = NULL;



static uint16 chromaWidth,chromaHeight;
static uint16 subsample; 
static uint16 acSize ;

#ifdef CACHE_POLICY_WRITE_BACK
static uint8 *original_thumb_luma_ptr = 0x0;
static uint8 *original_thumb_chroma_ptr = 0x0;
#endif /*CACHE_POLICY_WRITE_BACK*/

/* log2 table in Q8 */
static uint16 jpegLogTable[]=
{
  0, 256, 406, 512, 594, 662, 719, 768, 812, 850, 886, 918, 947, 975,
  1000, 1024, 1046, 1068, 1087, 1106, 1124, 1142, 1158, 1174, 1189, 1203,
  1217, 1231, 1244, 1256, 1268, 1280, 1291, 1302, 1313, 1324, 1334, 1343, 1353,
  1362, 1372, 1380, 1389, 1398, 1406, 1414, 1422, 1430, 1437, 1445, 1452, 1459,
  1466, 1473, 1480, 1487, 1493, 1500, 1506, 1512, 1518, 1524, 1530};

#ifdef FEATURE_CAMERA_MOBICAT
#define JPEGE_MOBICAT_STATIC_SIZE 8
static uint16 mcSizes[] = {sizeof(byte), sizeof(byte), sizeof(uint16), sizeof(int16),
  sizeof(uint32), sizeof(int32),sizeof(float), sizeof(double)};
static byte *mobicatStreamPtr = NULL;
static uint32 mobicatStreamSize = 0;
#endif 

typedef enum
{
  FSIZE_CTRL_INACTIVE = 0,
  PRE_PASS1_ENCODING,  /* Looking to init params for Pass 1  */
  PASS1_ENCODING,      /* Estimate file size & encode pass 1 */
  PASS2_ENCODING,      /* Acutal file size feedback & encode pass 2 */
  PASS_X_SUCCESSFUL,   /* Result of pass x encode came in at or under
                        target num bytes */
  /* FS error in reading from it */
  FAILURE_FS,
  /* Can't scale down output any more as we are already at 1% qual. */
  FAILURE_1_PERCENT,
  FAILURE_OUTPUT_OVER, /* Output of last pass is over requested output size */
  FAILURE_JPEG_ENCODE  /* JPEG will not accept this encode */
} fsRateCtrlStateType;

typedef struct
{
  uint32  resultPercent;  /* actual size / target size                  */
  uint32  fileSize;       /* Actual size (from the DSP)                 */
  uint32  estNumBytesHeader; 
  /* Number of bytes for JFIF/EXIF header + 
     quantization and Huffman tables            */
  uint32  actualNumBytesHeader; 
  /* Number of bytes for JFIF/EXIF header + 
     quantization and Huffman tables            */
  uint32  estNumBytesDC; 
  uint32  actualNumBytesDC;   
  int32   estNumBytesAC;
  uint32  actualNumBytesAC;
  uint32  estACBitsPerPixQ8; 
  /* Num bits per pixel for AC component (Q8)  */
  uint32  actualACBitsPerPixQ8; 
  /* Num bits per pixel for AC component (Q8)   */
  uint16  scaleFactorQ8;  /* Pass 1 quant table scale factor            */
  uint16  qualityPercent; /* pass1ScaleFactor in quality percent form   */
  uint32  multFactorKQ8;
}  jpegComponentSizeType;

typedef struct
{
  fsRateCtrlStateType state;     /* State of the file size rate control algo   */
  uint32  maxSnapshotFileSize;   /* Target file size (not scaled down)         */
  uint32  scaledFileSize;        /* Scaled down target file size               */
  uint16  passX;                 /* Iteration count index                      */
  uint16  maxQuality;            /* Max quaily to use (if applicable)          */
  uint16  maxScaleFactorQ8;      /* Max scale factor (derived from max quality */
  uint32  origScaleFactorQ8;
  uint32  num8x8Blocks;          /* Number of 8x8 blocks                       */
  uint32  numBytesEOB;           /* Number EOB (for luma and chroma)           */
  /* Pass x performance results                 */
  jpegComponentSizeType passXArr[JPEGE_FILE_SIZE_MAX_PASSES];
} fileSizeCtrlType;

/* Main struct for encapsulating JPEG file size rate control */
fileSizeCtrlType fileSizeCtrl;

/* Struct for calling JPEG within file size ctrl, make global here */
JPEGENC_encodeSpecType  globalEncodeSpec;   /* encode specification   */
exif_info_type         *glocalExifDataPtr;
JPEGENC_CBType          globalCallback;

static void jpeg_encoder_terminate
( void);
static void jpege_process_callback
( int32 clientId, 
  int32 clientData);
static void jpege_apply_scale_to_quant_tbls
( dcScalingOptionsType dcScale,
  double scaleFactor,
  uint16 * luma_dqt_table_ptr,
  uint16 * chroma_dqt_table_ptr );
void jpege_image_pad
( uint8 *image, 
  uint8 *image_new,
  uint16 height, 
  uint16 width,
  uint16 heightNew, 
  uint16 widthNew);
static JPEGENCReturnCodeType jpege_encode_file_size_common
( JPEGENC_encodeSpecType  *encodeInfo,   /* encode specification   */
  /* One of the following two will be NULL */
  exif_info_type *exifDataPtr,
  uint32 defaultHeaderSize );
static void jpege_file_size_callback
( JPEGENC_CBrtnType  *clientData);    /* client's useful data     */
static boolean jpege_estimate_file_size
( uint32 num8x8Blocks,
  JPEGENC_formatType subsample,
  jpegComponentSizeType * estimate );
static uint32 jpege_estimate_dc_component
( uint16 scaleFactorQ8,
  uint32 num8x8Blocks,
  JPEGENC_formatType subsample );
static void jpege_calculate_scale_factor
( uint32 fileSizeAC,
  uint32 num8x8Blocks,
  uint16 *scaleFactorQ8,
  uint16 *qualityPercent,
  uint32 *ACBitsPerPixQ8 );
static int16 jpege_log2Q8(uint16 x);
static int16 jpege_sum_8x8_luma
( uint8 * image, 
  uint16 width);
static int16 jpege_sum_8x8_chroma
( uint8 * image, 
  uint16 width);
static boolean jpege_init_file_size_enh
( JPEGENC_encodeSpecType spec);
static void jpege_exit_file_size_enh
( void);
static boolean jpege_estimate_ac_size_enh
( JPEGENC_encodeSpecType spec, 
  uint16 *ac_size);
static uint32 jpege_estimate_dc_size_enh
( JPEGENC_encodeSpecType spec, 
  uint16 estimateScaleFactorQ8);
static uint32 jpege_update_dc_size_enh
( JPEGENC_encodeSpecType spec, 
  uint16 estimateScaleFactorQ8);
void jpege_vertical_subsample
( uint8 *image, 
  uint16 height, 
  uint16 width);
/* Current client ID */
static  int32                   jpeg_session_id =0;
static  boolean                 jpeg_encoding = FALSE;
        boolean                 jpeg_encode_abort  = FALSE;
static  boolean                 jpeg_encode_failed = FALSE;
static  boolean                 jpege_in_shutdown_process = FALSE;

static  graph_context_req_type  context_data;
static  JPEGENC_encodeSpecType  localEncodeInfo;   /* encode specification   */
static  JPEGENC_CBrtnType       localReturnData;   /* return data for client */
static  JPEGENC_CBType          localClientCB_func = NULL;
static  exif_info_type          *exif_data_ptr;
static  JPEGE_OutBufCfgType     outputBuf[2];
#ifdef CACHE_POLICY_WRITE_BACK
static  JPEGE_OutBufCfgType     outputBuf_malloc[2];
#endif //CACHE_POLICY_WRITE_BACK
static  JPEGENC_headerType      jpegHeader;
static  bufferType              last_buffer_holder;
static  JPEGE_outputMsgBufType  buffer1, buffer2;
static  exif_info_type          jfifHdrData;

/* For dynamic quantization table generation          */
/* These two arrays will hold scaled DQT table values for JPEG encoding */
static uint16   jpeg_luma_dqt_variable[64];
static uint16   jpeg_chroma_dqt_variable[64];

static uint8    *padded_image = NULL;

static uint8    *main_luma_ptr = NULL;
static uint8    *main_chroma_ptr = NULL;
static uint8    *thumbnail_luma_ptr = NULL;
static uint8    *thumbnail_chroma_ptr = NULL;



static boolean  in_file_size_ctrl = FALSE;

#ifdef FEATURE_NEW_SLEEP_API
extern sleep_okts_handle jpeg_sleep_okts_handle;
#endif /* FEATURE_NEW_SLEEP_API */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

=============================================================================*/ 

/*lint -save -e834 -e818 possible declare const, op follows by op */

/*===========================================================================

Function: JPEGE_PARAM_CHECK

Description: This function will check the input parameters passed by encodeInfo
             and will subsample thumbnail and main image if coming as H1V1 
             (H2V1 with odd rotation)

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: localEncodeInfo, jpege_info

==============================================================================*/

JPEGENCReturnCodeType   jpege_param_check(JPEGENC_encodeSpecType  *encodeInfo)
{
  uint16 cnt;

  /* If initiated during another encoding session */
  if (jpeg_encoding)
  {
    MSG_HIGH("JPEGENC: encoder is in progress - new encoding not accepted",0 ,0, 0);
    return(JPEGENC_BUSY_STATE);
  }

  /* Check input pointers */
  if (!encodeInfo)
  {
    MSG_ERROR("JPEGENC: Bad encoder input parameters detected", 0, 0, 0);
    return(JPEGENC_INVALID_PTR);
  }

  if (!encodeInfo->Main.FragCnt)
  {
    MSG_ERROR("JPEGENC: Bad encoder input parameters detected", 0, 0, 0);
    return(JPEGENC_INVALID_PTR);
  }

  for (cnt=0; cnt<encodeInfo->Main.FragCnt; cnt++)
  {
    if (!(encodeInfo->Main.Fragmnt[cnt].Height)||
        !(encodeInfo->Main.Fragmnt[cnt].Luma_ptr)||
        !(encodeInfo->Main.Fragmnt[cnt].Chroma_ptr))
    {
      MSG_ERROR("JPEGENC: Bad fragment parameters detected %d %d %d",
                encodeInfo->Main.Fragmnt[cnt].Height , 
                encodeInfo->Main.Fragmnt[cnt].Luma_ptr,
                encodeInfo->Main.Fragmnt[cnt].Chroma_ptr);
      return(JPEGENC_BAD_DATA);
    }
  }

  if ((!encodeInfo->CallBack) || (!encodeInfo->Main.Width) ||
      (!encodeInfo->Main.Height) || (!encodeInfo->Main.FragCnt))
  {
    MSG_ERROR("JPEGENC: Bad encode parameters detected", 0, 0, 0);
    return(JPEGENC_BAD_DATA);
  }
  
  switch (encodeInfo->Rotation)
  {
    case JPEGENC_CLK90:
      encodeInfo->Rotation = JPEGENC_CCLK270;
      break;
    
    case JPEGENC_CLK180:
      encodeInfo->Rotation = JPEGENC_CCLK180;
      break;
    
    case JPEGENC_CLK270:
      encodeInfo->Rotation = JPEGENC_CCLK90;
      break;

    default:
      break;
  }
  
  if (encodeInfo->HasThumbnail)
  {
    if ((!encodeInfo->Thumbnail.Width) || (!encodeInfo->Thumbnail.Height) ||
        (encodeInfo->Thumbnail.Quality > 100) || (!encodeInfo->Thumbnail.Quality))
    {
      MSG_ERROR("JPEGENC: Bad thumbnail parameters detected", 0, 0, 0);
      return(JPEGENC_BAD_DATA);
    }

    if (!(encodeInfo->Thumbnail.Data.Height)||
        !(encodeInfo->Thumbnail.Data.Luma_ptr)||
        !(encodeInfo->Thumbnail.Data.Chroma_ptr))
    {
      MSG_ERROR("JPEGENC: Bad Thumbnail fragment parameters detected %d %d %d",
                encodeInfo->Thumbnail.Data.Height , 
                encodeInfo->Thumbnail.Data.Luma_ptr,
                encodeInfo->Thumbnail.Data.Chroma_ptr);
      return(JPEGENC_BAD_DATA);
    }
    else
    {
      jpege_info.thumbnail = UP_TO_THUMBNAIL;

      /* If H2V1 with odd rotation, subsample verticaly */
      if ((encodeInfo->Main.Subsample == JPEGENC_H2V1) &&
          ((encodeInfo->Rotation == JPEGENC_CCLK90) || (encodeInfo->Rotation == JPEGENC_CCLK270)))
      {
        jpege_vertical_subsample(encodeInfo->Thumbnail.Data.Chroma_ptr, encodeInfo->Thumbnail.Height,encodeInfo->Thumbnail.Width);
      }
    }
  }
  else
  {
    jpege_info.thumbnail = FULL_HEADER;
  }

  /* If H2V1 with odd rotation, subsample verticaly */
  if ((encodeInfo->Main.Subsample == JPEGENC_H2V1) &&
      ((encodeInfo->Rotation == JPEGENC_CCLK90) || (encodeInfo->Rotation == JPEGENC_CCLK270)))
  {
    jpege_vertical_subsample(encodeInfo->Main.Fragmnt[0].Chroma_ptr, encodeInfo->Main.Height,encodeInfo->Main.Width);
  }
  return(JPEGENC_SUCCESS);
}

/*===========================================================================

Function: JPEGE_ENCODE_EXIF

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: jpeg_session_id, jpeg_encoding, jpeg_encode_abort,
              jpeg_encode_failed, jpegHeader, localEncodeInfo, 
              exif_data_ptr, localClientCB_func, context_data 

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_exif(JPEGENC_encodeSpecType  *encodeInfo,   
                                          exif_info_type *exifDataPtr)
{
  JPEGENCReturnCodeType  rntStatus;

  rntStatus = jpege_param_check(encodeInfo);

  if (rntStatus != JPEGENC_SUCCESS)
  {
    return(rntStatus);
  }

  if (!exifDataPtr)
  {
    return(JPEGENC_INVALID_PTR);
  }

  jpeg_session_id++;
  jpeg_encoding = TRUE;
  jpeg_encode_abort      = FALSE;
  jpeg_encode_failed     = FALSE;
  jpegHeader    = EXIF_JPEG_HEADER;

  /* Save the pointers of the original input buffers.
     In the case where padding is needed, jpege layer will malloc buffers,
	 then use it for encoding. After encoding is done, jpege layer will free 
	 the buffers.
	 In callback function, the original buffer pointers need to be passed back,
	 for upper layer to free.  Hence here save the pointers.
   */
  main_luma_ptr = encodeInfo->Main.Fragmnt[0].Luma_ptr;
  main_chroma_ptr = encodeInfo->Main.Fragmnt[0].Chroma_ptr;
  thumbnail_luma_ptr = encodeInfo->Thumbnail.Data.Luma_ptr;
  thumbnail_chroma_ptr = encodeInfo->Thumbnail.Data.Chroma_ptr;

  /* Copy the client data for the async. processing */
  localEncodeInfo = *encodeInfo;
  localReturnData.encodeSpec = &localEncodeInfo;
  exif_data_ptr = exifDataPtr; 
  localClientCB_func = localEncodeInfo.CallBack;

  /* Queue the encoding command on tthe GRAPH task */
  context_data.function_ptr = jpege_process_callback;
  context_data.client_ID = jpeg_session_id;
  context_data.client_data = (int16)JPEGENC_DOWNLOAD;
  graph_context_req( context_data );
  return(rntStatus);
}

/*===========================================================================

Function: JPEGE_ENCODE_JFXX

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with JFIF extension (Thumbnail) embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: jpeg_session_id, jpeg_encoding, jpeg_encode_abort,
              jpeg_encode_failed, jpegHeader, localEncodeInfo, 
              jfifHdrData, localClientCB_func, context_data 
==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_jfxx(JPEGENC_encodeSpecType  *encodeInfo)
{
  JPEGENCReturnCodeType  rntStatus;

  rntStatus = jpege_param_check(encodeInfo);

  if (rntStatus != JPEGENC_SUCCESS)
  {
    return(rntStatus);
  }

  jpeg_session_id++;
  jpeg_encoding = TRUE;
  jpeg_encode_abort = FALSE;
  jpeg_encode_failed = FALSE;
  jpegHeader = JFIF_JPEG_HEADER;

  exif_init_info(&jfifHdrData);

  main_luma_ptr = encodeInfo->Main.Fragmnt[0].Luma_ptr;
  main_chroma_ptr = encodeInfo->Main.Fragmnt[0].Chroma_ptr;
  thumbnail_luma_ptr = encodeInfo->Thumbnail.Data.Luma_ptr;
  thumbnail_chroma_ptr = encodeInfo->Thumbnail.Data.Chroma_ptr;


  /* Copy the client data for the async. processing */
  localEncodeInfo = *encodeInfo;
  localReturnData.encodeSpec = &localEncodeInfo;
  exif_data_ptr = &jfifHdrData; 
  localClientCB_func = localEncodeInfo.CallBack;

  /* Queue the encoding command on tthe GRAPH task */
  context_data.function_ptr = jpege_process_callback;
  context_data.client_ID = jpeg_session_id;
  context_data.client_data = (int16)JPEGENC_DOWNLOAD;
  graph_context_req( context_data );
  return(rntStatus);
}

/*===========================================================================

Function: JPEGE_ENCODE_FILE_SIZE_EXIF

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.
             This function also performs file size control encoding on the
               image such that the passed in target file size is achieved through
               (potentially) several encoding passes.
             If an encoding quality is provided with this function for the main
               image, then file size control will encode to that quality as a
               maximum (however, the minimum could drop well below this).
               A main image encode quality of zero is ignored. 
             Encoding quailty is ignored in the thumbnail.

Return Value:
   JPEGENC_SUCCESS                 // cmd sucessful 
   JPEGENC_INVALID_PTR             // Invalid callback function
   JPEGENC_BUSY_STATE 
   JPEGENC_BAD_DATA 
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input

Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_file_size_exif (JPEGENC_encodeSpecType  *encodeInfo,                                                  
                                                     exif_info_type *exifDataPtr)
{
  JPEGENCReturnCodeType  rtnStatus= JPEGENC_SUCCESS;

  rtnStatus = jpege_encode_file_size_common(encodeInfo,   /* encode specification   */
                                            exifDataPtr,
                                            JPEG_EXIF_HEADER_SIZE );
  return(rtnStatus);
} 

/*===========================================================================

Function: JPEGE_ENCODE_FILE_SIZE_JFXX

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.
             This function also performs file size control encoding on the
               image such that the passed in target file size is achieved through
               (potentially) several encoding passes.
             If an encoding quality is provided with this function for the main
               image, then file size control will encode to that quality as a
               maximum (however, the minimum could drop well below this).
               A main image encode quality of zero is ignored. 
             Encoding quailty is ignored in the thumbnail.

Return Value:
   JPEGENC_SUCCESS                 // cmd sucessful 
   JPEGENC_INVALID_PTR             // Invalid callback function
   JPEGENC_BUSY_STATE 
   JPEGENC_BAD_DATA 
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input

Comments: None

Side Effects: None

==============================================================================*/
JPEGENCReturnCodeType   jpege_encode_file_size_jfxx(JPEGENC_encodeSpecType  *encodeInfo)
{
  JPEGENCReturnCodeType  rtnStatus = JPEGENC_SUCCESS;

  rtnStatus  = jpege_encode_file_size_common(encodeInfo,    /* encode specification   */
                                             NULL,          /* Nothing for JFIF */
                                             JPEG_JFIF_HEADER_SIZE );
  return(rtnStatus);
} 

/*===========================================================================

Function: JPEGE_ENCODE_FILE_SIZE_COMMON

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.
             This function also performs file size control encoding on the
               image such that the passed in target file size is achieved through
               (potentially) several encoding passes.
             If an encoding quality is provided with this function for the main
               image, then file size control will encode to that quality as a
               maximum (however, the minimum could drop well below this).
               A main image encode quality of zero is ignored. 
             Encoding quailty is ignored in the thumbnail.

Return Value:
   JPEGENC_SUCCESS                 // cmd sucessful 
   JPEGENC_INVALID_PTR             // Invalid callback function
   JPEGENC_BUSY_STATE 
   JPEGENC_BAD_DATA 
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input
   
Comments: 
             Launches encoding task.  Changes users callback to it's own.
             Runs potentially several encoding passes 
             (up to a compiled maximum limit)

Side Effects:
             localReturnData, localClientCB_func, linCoeffAdjustQ8, 
             squareCoeffAdjustQ8, fileSizeCtrl             
==============================================================================*/
static JPEGENCReturnCodeType jpege_encode_file_size_common (JPEGENC_encodeSpecType  *encodeInfo,
                                                            exif_info_type *exifDataPtr,
                                                            uint32 defaultHeaderSize)
{
  JPEGENCReturnCodeType  rtnStatus = JPEGENC_SUCCESS;
  uint32 totalPixels;
  localReturnData.encodeSpec = encodeInfo;


  main_luma_ptr = encodeInfo->Main.Fragmnt[0].Luma_ptr;
  main_chroma_ptr = encodeInfo->Main.Fragmnt[0].Chroma_ptr;
  thumbnail_luma_ptr = encodeInfo->Thumbnail.Data.Luma_ptr;
  thumbnail_chroma_ptr = encodeInfo->Thumbnail.Data.Chroma_ptr;

  if ((encodeInfo->Dest.device == JPEGENC_MEM) && 
      (encodeInfo->Dest.handle.mem.length < (1024*encodeInfo->TargetEncodedSize)))
  {
    localReturnData.clientId = encodeInfo->ClientId;
    localReturnData.status = JPEGENC_FILE_SIZE_FAIL;
    localClientCB_func = encodeInfo->CallBack; 

    if (localClientCB_func)
    {
	  /* restore the pointers to the caller,  so original buffers are freed.  */
	  localReturnData.encodeSpec->Main.Fragmnt[0].Luma_ptr = main_luma_ptr;
	  localReturnData.encodeSpec->Main.Fragmnt[0].Chroma_ptr = main_chroma_ptr;
	  localReturnData.encodeSpec->Thumbnail.Data.Luma_ptr=thumbnail_luma_ptr;
	  localReturnData.encodeSpec->Thumbnail.Data.Chroma_ptr=thumbnail_chroma_ptr;

      localClientCB_func(&localReturnData);
    }
    return(JPEGENC_ALLOCATED_MEM_TOO_LOW);
  }



  linCoeffAdjustQ8 = 0xFF;
  squareCoeffAdjustQ8 = 0xFF;
  fileSizeCtrl.state = PASS1_ENCODING;

  /* Clear struct out initially */
  memset( &fileSizeCtrl, 0, sizeof(fileSizeCtrlType));

  /* Convert kbytes to num bytes by multiplying by 1024 */
  fileSizeCtrl.maxSnapshotFileSize = 1024*encodeInfo->TargetEncodedSize;
  fileSizeCtrl.passX = 0;    /* Init pass to zero */

  /* If zero then this quality percent is set to the default, otherwise this 
     quality becomes the max encoding quailty, regardless of how much additional
     room we have for a greater quality image */
  if (encodeInfo->Main.Quality)
  {
    fileSizeCtrl.maxQuality = (uint32)encodeInfo->Main.Quality; 
  }
  else
  {
    fileSizeCtrl.maxQuality = JPEGE_MAX_FILE_SIZE_QUALITY_DEFAULT;
  }

  /* Convert max quality percent to scale factor.
   Apply this formula: scale factor = 50/(100-quality%)
   if quality is > 50 else scale factor = quality%/50         */
  if (fileSizeCtrl.maxQuality > 50)
  {
    fileSizeCtrl.maxScaleFactorQ8 = 
    (uint16) (256.0 * (50.0/(100.0-(float)fileSizeCtrl.maxQuality))); /* Q8 number */
  }
  else
  {
    fileSizeCtrl.maxScaleFactorQ8 = 
    (uint16) (256.0 * ((float)fileSizeCtrl.maxQuality/50.0)); /* Q8 number */
  }

  /* Reduce the target file size by X% that represents the error
     in this file size control algorithm (overshoot error)    */        
  fileSizeCtrl.scaledFileSize = 
  (uint32) ((fileSizeCtrl.maxSnapshotFileSize * fileSizeCfgParams.targetScaleDownPercent) / 100);

  totalPixels = encodeInfo->Main.Width * encodeInfo->Main.Height;

  fileSizeCtrl.passXArr[0].estNumBytesHeader = defaultHeaderSize;

  /* Add thumbnail into calculation */
  if (encodeInfo->HasThumbnail == TRUE)
  {
    totalPixels += encodeInfo->Thumbnail.Width * encodeInfo->Thumbnail.Height;
  }

  /* Calculate the number of 8x8 blocks as a straight grid;
     do not take into account 420 or 422.  8 * 8 = 64.  64=2^6 so 
     we can shift right by 6. */
  fileSizeCtrl.num8x8Blocks = totalPixels>>6;

  /* Round up to be a multiple of 8 (blocks) */
  fileSizeCtrl.num8x8Blocks = (fileSizeCtrl.num8x8Blocks + 7)&(uint32)0xFFFFFFF8;

  /* Save calling task callback */
  globalCallback = encodeInfo->CallBack;

  /* Save EXIF data ptr */
  glocalExifDataPtr = exifDataPtr;

  /* This encode spec is global, it does not go away */
  globalEncodeSpec = *encodeInfo;

  /* Use local callback to catch JPEG after it is done encoding */
  globalEncodeSpec.CallBack = jpege_file_size_callback;

  rtnStatus = (JPEGENCReturnCodeType)jpege_estimate_file_size( fileSizeCtrl.num8x8Blocks,
                                                               encodeInfo->Main.Subsample,
                                                               &fileSizeCtrl.passXArr[0]);
  if (!rtnStatus)
  {
    /* If error due to small file size callback with
       JPEGENC_FILE_SIZE_FAIL status                */
    localReturnData.clientId = encodeInfo->ClientId;
    localReturnData.status = JPEGENC_FILE_SIZE_FAIL;
    localClientCB_func = encodeInfo->CallBack; 
    if (localClientCB_func)
    {
		/* restore the pointers to the caller,  so original buffers are freed.  */
		localReturnData.encodeSpec->Main.Fragmnt[0].Luma_ptr = main_luma_ptr;
		localReturnData.encodeSpec->Main.Fragmnt[0].Chroma_ptr = main_chroma_ptr;
		localReturnData.encodeSpec->Thumbnail.Data.Luma_ptr=thumbnail_luma_ptr;
		localReturnData.encodeSpec->Thumbnail.Data.Chroma_ptr=thumbnail_chroma_ptr;

		localClientCB_func(&localReturnData);
    }
    return(rtnStatus);
  }

  globalEncodeSpec.Main.Quality   = fileSizeCtrl.passXArr[0].qualityPercent;
  globalEncodeSpec.Thumbnail.Quality   = fileSizeCtrl.passXArr[0].qualityPercent;
  in_file_size_ctrl = TRUE;

  if (exifDataPtr != NULL)
  {  /* Then treat as EXIF */
    rtnStatus = jpege_encode_exif( &globalEncodeSpec, glocalExifDataPtr );
  }
  else
  { /* must be JFIF */
    rtnStatus = jpege_encode_jfxx( &globalEncodeSpec );
  }
  return(rtnStatus);
} /* jpege_encode_file_size_common */

/*===========================================================================

Function: JPEGE_FILE_SIZE_CALLBACK

Description: The callback is called upon completion of file size controled JPEG
             encoding. The output info is assesssed and based on customer
             criteria another JPEG encoding is scheduled

Return Value:
typedef enum
{
   JPEGENC_SUCCESS       = 0,      // cmd sucessful
   JPEGENC_INVALID_PTR,            // Invalid callback function
   JPEGENC_BUSY_STATE,
   JPEGENC_BAD_DATA,
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input
} JPEGENCReturnCodeType;

Comments: None

Side Effects: in_file_size_ctrl, fileSizeCtrl
==============================================================================*/
static void jpege_file_size_callback (JPEGENC_CBrtnType  *clientData)
{
  jpegComponentSizeType * passInfo;
  jpegComponentSizeType * nextPassInfo;
  int32 qual, index;
  JPEGENCReturnCodeType  rtnStatus= JPEGENC_SUCCESS;
  uint32 interpolatedBPPQ8, newScaleQ8 = 1;
  uint32 squareCoeffQ8;
  uint32 linearCoeffQ8;

  in_file_size_ctrl = FALSE;
  passInfo = &(fileSizeCtrl.passXArr[fileSizeCtrl.passX]);

  switch (clientData->status)
  {
    case JPEGENC_DSP_SUCCESS:
    case JPEGENC_IMG_DONE:
      break;
    case JPEGENC_DSP_FAIL:
    case JPEGENC_DSP_BAD_CMD:
    case JPEGENC_IMG_ABORT:
    case JPEGENC_IMG_FAIL:
    default:
      MSG_HIGH("ERROR File Size callback code: %d", clientData->status, 0, 0);
      /* Callback to camera services with JPEG error code */
      if (globalCallback)
      {
        globalCallback( clientData );
      }
      return;
  }

  clientData->status = JPEGENC_IMG_DONE;

  /* Set actual size of this encoding (incl thumbnail and header) */
  passInfo->fileSize = clientData->size;

  /* How we fared.  Less than 100 means pass1 met the limit, over did not.*/
  passInfo->resultPercent = (uint32) ((100.0 * ((float)passInfo->fileSize / 
                                                (float) fileSizeCtrl.maxSnapshotFileSize))+0.5);
  /* actual bit count for DC, used by filesize control */
  passInfo->actualNumBytesDC = (clientData->dcBitCnt/8);

  passInfo->actualNumBytesHeader = clientData->header_size;

  /* How many bytes did the AC components actually consume */
  passInfo->actualNumBytesAC = passInfo->fileSize - fileSizeCtrl.numBytesEOB - 
                               passInfo->actualNumBytesDC - passInfo->actualNumBytesHeader;

  /* AC bit per pixel Q8 = 256(8 * fileSizeAC / (num 8x8 blocks*64)) */
  passInfo->actualACBitsPerPixQ8 = (((32 * passInfo->actualNumBytesAC)/fileSizeCtrl.num8x8Blocks)+1);

  /* See if we pass on this run */
  if (fileSizeCtrl.maxSnapshotFileSize >= passInfo->fileSize)
  {
    /* starting with quality ranges 0-10 and incrementing by 10
       percent, run Pass 2 if the Pass 1 file size is a certain
       gradient less than desired off the target max file size */
    for (index = 0, qual = 10; qual <= 100; index++, qual += 10)
    {
      /* If are passes are up - then accept an undershoot */
      if (JPEGE_FILE_SIZE_MAX_PASSES <= (1+fileSizeCtrl.passX))
      {
        /* Accept an undershoot image (quailty too low) because this is
           the last pass we can perform */
        fileSizeCtrl.state = PASS_X_SUCCESSFUL;
        break;  /* out of FOR loop */
      }

      if (qual >= passInfo->qualityPercent)
      {
        /* Decide whether or not to commit to pass 2 encode */
        if (passInfo->resultPercent >= fileSizeCfgParams.fsThresholds[index])
        {
          /* Image came within acceptable range of requested file size, neither
             over size or too far under, accept this pass */
          fileSizeCtrl.state = PASS_X_SUCCESSFUL;
          break;  /* out of FOR loop */
        }
      }
    }
  }

  /* Now either encode again or callback to caller function if done */
  /* Iterate to the next free pass */
  fileSizeCtrl.passX++;

  if (fileSizeCtrl.state == PASS_X_SUCCESSFUL)
  {
    if (globalCallback)
    {
      globalCallback( clientData );
    }
  }
  else if (JPEGE_FILE_SIZE_MAX_PASSES <= fileSizeCtrl.passX)
  {
    MSG_ERROR("Error: Still over target size after %d passes", fileSizeCtrl.passX, 0, 0);
    fileSizeCtrl.state = FAILURE_OUTPUT_OVER;
    clientData->status = JPEGENC_FILE_SIZE_FAIL;

    if (globalCallback)
    {
      globalCallback( clientData );
    }
  }
  else
  { /* Run another pass */
    /* Compute paremeters for next pass                       */
    nextPassInfo = &(fileSizeCtrl.passXArr[fileSizeCtrl.passX]);

    /* Actual file size is greater than the target size */
    if (passInfo->estACBitsPerPixQ8 < passInfo->actualACBitsPerPixQ8)
    {
      /* First make sure there is room to scale down */
      if (passInfo->scaleFactorQ8 <= SCALE_FACTOR_1_PER_Q8)
      {
        /* Scale factor for previous pass is already lowest, so do NOT
           attempt another pass, just return here */
        fileSizeCtrl.state = FAILURE_1_PERCENT;
        MSG_ERROR("Error: Still over target size with lowest scale factor", 0, 0, 0);
        fileSizeCtrl.state = FAILURE_OUTPUT_OVER;
        clientData->status = JPEGENC_FILE_SIZE_FAIL;
        if (globalCallback)
        {
          globalCallback( clientData );
        }
        return;
      }

      /* definitions: new = new Bits Per Pixel to base next pass encoding on;
         cur = BPP result of Pass 1 frame; est = BPP estimate before Pass 1.
         new = cur + (cur-est)/4 */
      interpolatedBPPQ8 =(uint32)(passInfo->estACBitsPerPixQ8 + 
                                  (((passInfo->actualACBitsPerPixQ8 - 
                                     passInfo->estACBitsPerPixQ8)*2)/3));

      /* Now apply new BPP from above to get a mult factor (K) for the
         orignial scale factor */
      squareCoeffQ8 = interpolatedBPPQ8 * interpolatedBPPQ8; /* Q8 * Q8 = Q16 */
      squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
      squareCoeffQ8 *= JPEGE_FILE_SIZE_SQUARE_COEFF_Q8; /* Q8 * Q8 = Q16 */
      squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
      squareCoeffQ8 = ((squareCoeffQ8 *(uint32)squareCoeffAdjustQ8 +(1<<7))>>8);     

      linearCoeffQ8 = JPEGE_FILE_SIZE_LIN_COEFF_Q8 * interpolatedBPPQ8;
      linearCoeffQ8 = linearCoeffQ8 >>8;  /* Convert back to Q8 */
      linearCoeffQ8 = ((linearCoeffQ8 *(uint32)linCoeffAdjustQ8 +(1<<7))>>8);

      newScaleQ8 = squareCoeffQ8 + linearCoeffQ8;
    }
    /* UNDERSHOOT */
    else
    {
      /* We undershot by too much, adjust quality up and encode again */
      /* First make sure there is room to scale up */
      if (passInfo->scaleFactorQ8 >= fileSizeCtrl.maxScaleFactorQ8)
      {
        /* Scale factor for current pass is already highest, so do NOT
           attempt another pass, just return here */
        /* This time we are successful */
        fileSizeCtrl.state = PASS_X_SUCCESSFUL;

        if (globalCallback)
        {
          globalCallback( clientData );
        }

        return;
      }
      else
      {
        /* Compute new scale factor to use in below K factor calc */
        /* Q8 * Q8 = Q16 */
        squareCoeffQ8 = (passInfo->actualACBitsPerPixQ8 * passInfo->actualACBitsPerPixQ8); 
        squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
        squareCoeffQ8 *= JPEGE_FILE_SIZE_SQUARE_COEFF_Q8; /* Q8 * Q8 = Q16 */
        squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
        squareCoeffQ8 = ((squareCoeffQ8 *(uint32)squareCoeffAdjustQ8 +(1<<7))>>8);

        linearCoeffQ8 = JPEGE_FILE_SIZE_LIN_COEFF_Q8 * passInfo->actualACBitsPerPixQ8;
        linearCoeffQ8 = linearCoeffQ8 >> 8;  /* Convert back to Q8 */
        linearCoeffQ8 = ((linearCoeffQ8 *(uint32)linCoeffAdjustQ8 +(1<<7))>>8);

        newScaleQ8 = squareCoeffQ8 + linearCoeffQ8;

        /* Limit scale */ 
        newScaleQ8 = newScaleQ8 + (uint32)((passInfo->scaleFactorQ8 - newScaleQ8)>>2);
      } /* end if there is room to scale up */
    } /* end if we undershot the previous pass */

    /* Based on the new scale factor, adjust the old one here
       either up or down */
    /* Q8 = Q16 / Q8 */

    if (newScaleQ8==0) 
	{ 
	   newScaleQ8=1; /* to avoid divide by zero error, floor the variable to default value */
	}

    nextPassInfo->multFactorKQ8 =  (256*fileSizeCtrl.origScaleFactorQ8) / newScaleQ8;

    if (passInfo->multFactorKQ8 > MAX_K_FACTOR_VALUE_Q8)
    {
      MSG_HIGH("Capping K Factor to max acceptable (was %d)", passInfo->multFactorKQ8, 0, 0);
      passInfo->multFactorKQ8 = MAX_K_FACTOR_VALUE_Q8;
    }

    /* Now "scale" the estimated scale factor in the current pass by K
       to form the new scale factor for the next pass (Convert to Q8 from Q16)*/
    nextPassInfo->scaleFactorQ8 = (uint16)((passInfo->scaleFactorQ8 * nextPassInfo->multFactorKQ8)>>8);

    /* Convert from Q8 scale to quality percent */
    if (nextPassInfo->scaleFactorQ8 > SCALE_FACTOR_50_PER_Q8)
    {
      /* for > 50% quality */
      nextPassInfo->qualityPercent = 100 - ((int16) ((50*256)/(nextPassInfo->scaleFactorQ8)));
    }
    else
    {
      /* for <= 50% quality */
      nextPassInfo->qualityPercent = (uint16)((50 * (nextPassInfo->scaleFactorQ8)) >>8);
    }

    /* Other parameters are already set from the first pass */
    /* Just adjust the qualty percent and encode again      */
    globalEncodeSpec.Main.Quality        = nextPassInfo->qualityPercent;
    globalEncodeSpec.Thumbnail.Quality   = nextPassInfo->qualityPercent;

    /* All leftover goes to AC component now */
    nextPassInfo->estNumBytesAC = (int32)(fileSizeCtrl.scaledFileSize - 
                                          fileSizeCtrl.numBytesEOB - 
                                          passInfo->estNumBytesDC - 
                                          passInfo->estNumBytesHeader);
    nextPassInfo->estACBitsPerPixQ8 = (32*fileSizeCtrl.num8x8Blocks)/fileSizeCtrl.num8x8Blocks;

    if (fileSizeCtrl.passX == 1)
    {
      fileSizeCtrl.state = PASS2_ENCODING;
    }

    in_file_size_ctrl = TRUE;  

    if (jpegHeader == EXIF_JPEG_HEADER)
    {
      rtnStatus = jpege_encode_exif( &globalEncodeSpec, glocalExifDataPtr );
    }
    else
    {
      rtnStatus = jpege_encode_jfxx( &globalEncodeSpec );
    }

    if (rtnStatus != JPEGENC_SUCCESS)
    {
      MSG_ERROR("Error: subsequent passes failed to start", fileSizeCtrl.passX, 0, 0);
      fileSizeCtrl.state = FAILURE_JPEG_ENCODE;
      if (globalCallback)
      {
        clientData->status = JPEGENC_IMG_FAIL;
        globalCallback( clientData );
      }
    }
  }
}

/*===========================================================================

Function: JPEGE_ESTIMATE_FILE_SIZE

Description:
   Provide estimates for the following given file size
   (file size minus header size minus built-in-error)
       1) estimate of num bytes for end of block data
       2) estimate of num bytes for DC components
       3) estimate of num bytes for AC components
     -Estimate takes into account the maximum quality desired

Return Value:
typedef enum
{
   JPEGENC_SUCCESS       = 0,      // cmd sucessful
   JPEGENC_INVALID_PTR,            // Invalid callback function
   JPEGENC_BUSY_STATE,
   JPEGENC_BAD_DATA,
   JPEGENC_INVALID_INPUT_PARM      // Encoding was given invaild input
} JPEGENCReturnCodeType;

Comments: None

Side Effects: fileSizeCtrl, linCoeffAdjustQ8, squareCoeffAdjustQ8

==============================================================================*/
static boolean jpege_estimate_file_size(uint32 num8x8Blocks,
                                        JPEGENC_formatType subsample_local,
                                        jpegComponentSizeType * estimate )
{
  uint32 DCtemp;
  int32 dcDifference;
  uint32 tmp;

  /* Calculate num End of Block bytes for all blocks */
  if (subsample_local == JPEGENC_H2V2)
  {
    fileSizeCtrl.numBytesEOB = (num8x8Blocks * END_OF_BLOCK_BITS_PER_8X8_420)/8;
  }
  else if ((subsample_local == JPEGENC_H2V1) ||
           (subsample_local == JPEGENC_H1V2))
  {
    fileSizeCtrl.numBytesEOB = (num8x8Blocks * END_OF_BLOCK_BITS_PER_8X8_422)/8;
  }

  /* Estimate DC based on fifty % scale factor */
  if (!file_size_enhanced)
  {
    estimate->estNumBytesDC = jpege_estimate_dc_component(FIFTY_PERCENT_SCALE_FACTOR_Q8,
                                                          num8x8Blocks,
                                                          subsample_local);
  }
  else
  {
    /* If using file size enhancement */
    /* Reserve memory and subsample image */
    if (!jpege_init_file_size_enh(globalEncodeSpec))
    {
      jpege_exit_file_size_enh();
      MSG_ERROR("JPEGENC: malloc falied in file size", 0, 0, 0); 
      return(FALSE);
    }

    /* Estimate image complexity */
    if (jpege_estimate_ac_size_enh(globalEncodeSpec, &acSize))
    {
      /* Choose reference point baced on image complexity */
      if (acSize < JPEGE_FILE_SIZE_LOW_THRESHOLD)
      {
        linCoeffAdjustQ8 =(int16)( (goldCurveRateAt80PercentHighQ8<<8)/acSize);
        squareCoeffAdjustQ8 = (int16)((linCoeffAdjustQ8 * linCoeffAdjustQ8)>>8);
      }
      else if (acSize < JPEGE_FILE_SIZE_HIGH_THRESHOLD)
      {
        tmp = (acSize - JPEGE_FILE_SIZE_LOW_THRESHOLD)/2;
        linCoeffAdjustQ8 =(int16)( ((goldCurveRateAt80PercentHighQ8-tmp)<<8)/acSize);
        squareCoeffAdjustQ8 = (int16)((linCoeffAdjustQ8 *linCoeffAdjustQ8)>>8);
      }
      else
      {
        linCoeffAdjustQ8 =(int16)( ((int32)goldCurveRateAt80PercentLowQ8<<8)/acSize);
        squareCoeffAdjustQ8 = (int16)(((int32)linCoeffAdjustQ8 * (int32)linCoeffAdjustQ8)>>8);
      }

      /* Limit up coefficients */
      if (linCoeffAdjustQ8 > JPEGE_FILE_SIZE_MAX_LIN_COEF_Q8)
      {
        linCoeffAdjustQ8 = JPEGE_FILE_SIZE_MAX_LIN_COEF_Q8;
      }

      if (squareCoeffAdjustQ8 > JPEGE_FILE_SIZE_MAX_SQ_COEF_Q8)
      {
        squareCoeffAdjustQ8 = JPEGE_FILE_SIZE_MAX_SQ_COEF_Q8;
      }
    }

    /* Estimate DC size */
    estimate->estNumBytesDC = jpege_estimate_dc_size_enh(globalEncodeSpec, SCALE_FACTOR_DC_ENH_Q8);
  }

  /* All leftover goes to AC component now */
  estimate->estNumBytesAC = (int32)(fileSizeCtrl.scaledFileSize - 
                                    fileSizeCtrl.numBytesEOB - 
                                    estimate->estNumBytesDC - 
                                    estimate->estNumBytesHeader);

  if (estimate->estNumBytesAC <= 0)
  {
    MSG_ERROR("JPEGENC: requested file size too small %d", estimate->estNumBytesAC, 0, 0); 

	/* free memory if use this option to do estimation */
    if (file_size_enhanced) {
		jpege_exit_file_size_enh();
	}

    return(FALSE);
  }

  /* Compute scale factor and quality percent from num bytes available
     for AC */
  jpege_calculate_scale_factor((uint32)estimate->estNumBytesAC, 
                               num8x8Blocks,
                               &estimate->scaleFactorQ8,
                               &estimate->qualityPercent,
                               &estimate->estACBitsPerPixQ8 );

  /* Estimate DC based on calculated scale factor */
  if (!file_size_enhanced)
  {
    DCtemp = jpege_estimate_dc_component(estimate->scaleFactorQ8, /* Q8 number */
                                         num8x8Blocks,
                                         subsample_local );
  }
  else
  {
    DCtemp = jpege_update_dc_size_enh(globalEncodeSpec, estimate->scaleFactorQ8);

  }

  dcDifference = (int32)(estimate->estNumBytesDC - DCtemp);
  if (dcDifference  < 0)
    dcDifference = -dcDifference;

  /* if >= 1% difference; then recalc AC scale factor */
  if ((fileSizeCtrl.scaledFileSize* 0.01) < dcDifference)
  {
    /* Save new DC and recalc scale factor for 1 iteration (only) */
    estimate->estNumBytesDC = DCtemp;

    /* All leftover goes to AC component now */
    estimate->estNumBytesAC = (int32)(fileSizeCtrl.scaledFileSize - 
                                      fileSizeCtrl.numBytesEOB - 
                                      estimate->estNumBytesDC - 
                                      estimate->estNumBytesHeader);

    if (estimate->estNumBytesAC <= 0)
    {
      MSG_ERROR("JPEGENC: requested file size too small %d", estimate->estNumBytesAC, 0, 0); 
	  /* free memory if use this option to do estimation */
	  if (file_size_enhanced) {
		  jpege_exit_file_size_enh();
	  }

      return(FALSE);
    }

    /* Compute scale factor and quality percent from num bytes available
       for AC */
    jpege_calculate_scale_factor((uint32)estimate->estNumBytesAC, 
                                 num8x8Blocks,
                                 &estimate->scaleFactorQ8,
                                 &estimate->qualityPercent,
                                 &estimate->estACBitsPerPixQ8 );
  }

  /* Free file size enhancement memory */
  if (file_size_enhanced)
  {
    jpege_exit_file_size_enh();
  }
  return(TRUE);
}

/*===========================================================================

Function: JPEGE_ESTIMATE_DC_COMPONENT

Description:
  Estimate DC component based on scale factor

Return Value:
  Estimate num bytes DC value

Comments: None

Side Effects: None

==============================================================================*/
static uint32 jpege_estimate_dc_component(uint16 scaleFactorQ8,
                                          uint32 num8x8Blocks,
                                          JPEGENC_formatType subsample_local )
{
  uint32 numDCBits8x8_Q8;
  uint32 numDCBits8x8Format_Q8;
  uint32 numDCBytes;

  /* Round scale factor up for simplified calc below */
  numDCBits8x8_Q8 = (256*4) + (5*scaleFactorQ8) - ((scaleFactorQ8*scaleFactorQ8)/(3*256)); 

  if (subsample_local == JPEGENC_H2V2)
  {
    /* Mult by 1.5 for account for H2V2 subsampling */
    numDCBits8x8Format_Q8 = numDCBits8x8_Q8 + (numDCBits8x8_Q8>>1); 

  }
  else
  {
    /* Mult by 2.0 for account for H2V2 subsampling */
    numDCBits8x8Format_Q8 = 2*numDCBits8x8_Q8;
  }

  /* Shift by 11 converts from Q8 to Q0 and from bits to bytes */
  numDCBytes = (numDCBits8x8Format_Q8 * num8x8Blocks) >> 11;

  return(numDCBytes);
}

/*===========================================================================

Function: JPEGE_CALCULATE_SCALE_FACTOR

Description:
  Compute scale factor from num bytes available for image.

Return Value:
  scaleFactor in Q8 format
  qualityPercent 

Comments: None

Side Effects: None

==============================================================================*/
static void jpege_calculate_scale_factor(uint32 fileSizeAC, uint32 num8x8Blocks, 
                                         uint16 *scaleFactorQ8, uint16 *qualityPercent,
                                         uint32 *ACBitsPerPixQ8 )
{
  uint32 squareCoeffQ8;
  uint32 linearCoeffQ8;

  /* Check to make sure max file size in kbytes is 
     a realistic value given the image encoding size */
  if (fileSizeAC < JPEGE_MIN_NUM_BYTES_FOR_AC)
  {

    /* The requested file size is unrealistic, but there is nothing we can do
       proceed with encoding and do our best here */
    MSG_HIGH("Requested File Size too small %d %d", fileSizeAC, JPEGE_MIN_NUM_BYTES_FOR_AC, 0);
  }

  /* AC bit per pixel Q8 = 256(8 * fileSizeAC / (num 8x8 blocks*64)) */
  *ACBitsPerPixQ8 = 32 * (fileSizeAC/num8x8Blocks);


  /* Now estimate the scaling factor to be applied to each quantization table
     by: scale factor = 0.6869(estAcBitsPerPixel)^2 + 1.5825(estAcBitsPerPixel) */

  squareCoeffQ8 = (*ACBitsPerPixQ8 * (*ACBitsPerPixQ8)); /* Q8 * Q8 = Q16 */
  squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
  squareCoeffQ8 *= JPEGE_FILE_SIZE_SQUARE_COEFF_Q8; /* Q8 * Q8 = Q16 */
  squareCoeffQ8 = squareCoeffQ8 >> 8;  /* Convert back to Q8 */
  squareCoeffQ8 = ((squareCoeffQ8 *squareCoeffAdjustQ8 +(1<<7))>>8);

  linearCoeffQ8 = JPEGE_FILE_SIZE_LIN_COEFF_Q8 * (*ACBitsPerPixQ8);
  linearCoeffQ8 = linearCoeffQ8 >> 8;  /* Convert back to Q8 */
  linearCoeffQ8 = ((linearCoeffQ8 *linCoeffAdjustQ8 +(1<<7))>>8);

  *scaleFactorQ8 = (uint16)(squareCoeffQ8 + linearCoeffQ8);

  fileSizeCtrl.origScaleFactorQ8 = *scaleFactorQ8;

  if (*scaleFactorQ8 > fileSizeCtrl.maxScaleFactorQ8)
  {
    *scaleFactorQ8 = fileSizeCtrl.maxScaleFactorQ8;
  }

  /* Convert from Q8 scale to quality percent */
  if (*scaleFactorQ8 > SCALE_FACTOR_50_PER_Q8)
  {
    /* for > 50% quality */
    *qualityPercent = 100 - ((int16) ((50*256)/(*scaleFactorQ8)));
  }
  else
  {
    /* for <= 50% quality */
    *qualityPercent = (uint16)((50 * (*scaleFactorQ8)) >>8);
  }
}

/*===========================================================================

Function: JPEGE_HANDLE_DSP_CB

Description: This function will encode the YCbCr (MDP) into a JFIF compliance
               format with EXIF (version 2.2) data embedded into the bit stream.
             This is an asynchronous function but caller will receive 
               a return status for this function.
             Upon completion of the encoding, a callback
               will be invoked to notify the caller the status of the encoding.

Return Value:
   JPEGENC_SUCCESS              cmd sucessful
   JPEGENC_INVALID_CB           cmd contains a NULL cb pointer
   JPEGENC_INVALID_STATE        cmd issued during invalid state
   
Comments: None

Side Effects: context_data

==============================================================================*/
void jpege_handle_dsp_CB (JPEGQDSPMsgType  *pMsgInfo)
{
  /* Base on the information from the DSP, set the encoder state */
  if (pMsgInfo)
  {
    switch (pMsgInfo->Code)
    {
      case  JPEGE_IMAGE_READY:
        /* DSP image is ready.   */
        context_data.client_data = (int16)JPEGENC_ENCODE_IMG;
        break;
      case  JPEGE_OUTPUT_PRODUCED:

        if (last_buffer_holder == BUFFER_2)
        {
          /* Use BUFFER1 to hold DSP buffer info */
          buffer1 = pMsgInfo->Msg.Buf;
          last_buffer_holder = BUFFER_1;
          context_data.client_data = (int16)JPEGENC_READ_BUFFER1;
        }
        else
        { /* last_buffer_holder == BUFFER_1 */
          /* Use BUFFER2 to hold DSP buffer info */
          buffer2 = pMsgInfo->Msg.Buf;
          last_buffer_holder = BUFFER_2;
          context_data.client_data = (int16)JPEGENC_READ_BUFFER2;
        }
        break;
      case  JPEGE_IDLE_COMPLETE:
        context_data.client_data =  (int16)JPEGENC_IDLE;
        break;
      case  JPEGE_ILLEGAL_CMD:
        /* Not sure what we can recover from here other than log a msg */
        context_data.client_data =  (int16)JPEGENC_OPER_MAX;
        MSG_ERROR("JPEGENC: DSP rejected command", 0, 0, 0); 
        break;
      default:
        context_data.client_data =  (int16)JPEGENC_OPER_MAX;
        MSG_ERROR("JPEGENC: DSP is lost", 0, 0, 0); 
    }
    /* Queue the encoding command on the GRAPH task */
    context_data.function_ptr = jpege_process_callback;
    context_data.client_ID = jpeg_session_id;
    graph_context_req( context_data );
  }
}

/*===========================================================================
Function: JPEGE_SETUP_ENCODE_CMD

Description:  This function prepares the encoding information. Setups working
              variables and allocates memory for the header.
              
Return Value: None
   
Comments: This function knows when the entire header information is ready and it 
          will write the content in the temporary buffer to the destination
          accordingly and transition the state to encode the main image.

Side Effects:
============================================================================*/
void jpege_setup_encode_cmd(JPEGENC_encodeSpecType *pInfo, JPEGE_encodeCmdType *pCmd)
{
  uint16 cnt;
  uint8 * thumb_cached_luma_buff = 0x0;
  uint8 * thumb_cached_chroma_buff = 0x0;

  if (!pInfo || !pCmd)
  {
    MSG_HIGH("JPEGENC: Null pointer encounter", 0, 0, 0);
    return;
  }

  /* Get the ping pong buffers for the encoder */
  pCmd->Out[0] = outputBuf[0];
  pCmd->Out[1] = outputBuf[1];

  /* assign the quantization table location */
  pCmd->LumaDQTPtr   = jpeg_luma_dqt_variable;
  pCmd->ChromaDQTPtr = jpeg_chroma_dqt_variable;

  pCmd->Enc.Reserved1 = 0;
  pCmd->Enc.Reserved2 = 0;

  pCmd->Img.Reserved  = 0;
  pCmd->Fragmnt[0].Reserved  = 0;

  /* Initialize the bufer designation for buffer 1 is first one to be used */
  last_buffer_holder = BUFFER_2;

  if (pInfo->HasThumbnail)
  {
    /* Setup to encode the thunbnail image */
    pCmd->Enc.Subsample  = (uint16)pInfo->Thumbnail.Subsample;/* JPEG encoding format               */
    /* 0 - H2V2                           */
    /* 1 - H2V1 (0 & 180 rotation only)   */
    /* 2 - H1V2 (90 & 270 rotation only)  */
    /* 3 - Not used                       */
    /* If H2V1 with odd rotation pass to DSP as H1V2 */

    if ((pInfo->Thumbnail.Subsample == JPEGENC_H2V1) &&
        ((pInfo->Rotation == JPEGENC_CCLK90) || (pInfo->Rotation == JPEGENC_CCLK270)))
      pCmd->Enc.Subsample = (uint16)JPEGENC_H1V2;

    pCmd->Enc.Rotation = (uint16)pInfo->Rotation;
    /* 0 -  no rotation                   */
    /* 1 -  90 degrees CClockwise rotation*/
    /* 2 - 180 degrees CClockwise rotation*/
    /* 3 - 270 degrees CClockwise rotation*/
    pCmd->Enc.Fragments = 1; /* Number of fragments                */
    pCmd->Enc.Restart   = pInfo->Thumbnail.Restart; /* Restart interval count */

    pCmd->Img.Width = pInfo->Thumbnail.Width;
#ifdef CACHE_POLICY_WRITE_BACK
    //Malloc and update Luma buff with 32 bit aligned address
    thumb_cached_luma_buff  = JPEG_MALLOC(pInfo->Thumbnail.Width*pInfo->Thumbnail.Height + CACHE_LINE_SIZE);
    //preserve the original ptr for freeing it later
    original_thumb_luma_ptr = thumb_cached_luma_buff;
    //allign 32 bit now.
    thumb_cached_luma_buff  = (uint8*)((((uint32)thumb_cached_luma_buff+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
    //Malloc and update Chroma buff with 32 bit aligned address
	if (pInfo->Thumbnail.Subsample == JPEGENC_H2V2)
	{
	    thumb_cached_chroma_buff  = JPEG_MALLOC((pInfo->Thumbnail.Width*pInfo->Thumbnail.Height)/2 + CACHE_LINE_SIZE);
	}
	else 
	{
	    thumb_cached_chroma_buff  = JPEG_MALLOC((pInfo->Thumbnail.Width*pInfo->Thumbnail.Height) + CACHE_LINE_SIZE);
	}
   	//preserve the original ptr for freeing it later
    original_thumb_chroma_ptr = thumb_cached_chroma_buff;
    //Allign to 32 now
    thumb_cached_chroma_buff  = (uint8*)((((uint32)thumb_cached_chroma_buff+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);

    if (thumb_cached_luma_buff && thumb_cached_chroma_buff)
    {
        //Copy the contents to the new alligned buffer
        memcpy (thumb_cached_luma_buff,pInfo->Thumbnail.Data.Luma_ptr, pInfo->Thumbnail.Width*pInfo->Thumbnail.Height);
        memcpy (thumb_cached_chroma_buff,pInfo->Thumbnail.Data.Chroma_ptr, pInfo->Thumbnail.Width*pInfo->Thumbnail.Height);
    
        //Update the original luma and chroma address with the cached/alligned ones
        pInfo->Thumbnail.Data.Luma_ptr = thumb_cached_luma_buff;
        pInfo->Thumbnail.Data.Chroma_ptr = thumb_cached_chroma_buff;
    
        //Flush the cache for thumbnail before encoding 
        JPEG_FLUSH_DATA_CACHE ((uint32*)pInfo->Thumbnail.Data.Luma_ptr,pInfo->Thumbnail.Width*pInfo->Thumbnail.Height);
        
        if (pInfo->Thumbnail.Subsample == JPEGENC_H2V2)
        {
              JPEG_FLUSH_DATA_CACHE ((uint32*)pInfo->Thumbnail.Data.Chroma_ptr,pInfo->Thumbnail.Width*pInfo->Thumbnail.Height/2);
        }
        else 
        {
            //Hope that it is only H2V1 or H1V2, not H1V1 case 
            //TO DO address H1V1 case too!
            JPEG_FLUSH_DATA_CACHE ((uint32*)pInfo->Thumbnail.Data.Chroma_ptr,pInfo->Thumbnail.Width*pInfo->Thumbnail.Height);        
        }
    }
    else 
    {
        MSG_ERROR ("jpege_setup_encode_cmd : Malloc failed",0,0,0);    
    }
#endif /*CACHE_POLICY_WRITE_BACK*/ 
    pCmd->Fragmnt[0] = pInfo->Thumbnail.Data; 
  }
  else
  {
    /* Setup to encode the thunbnail image */
    pCmd->Enc.Subsample  = (uint16)pInfo->Main.Subsample;/* JPEG encoding format               */
    /* 0 - H2V2                           */
    /* 1 - H2V1 (0 & 180 rotation only)   */
    /* 2 - H1V2 (90 & 270 rotation only)  */
    /* 3 - Not used                       */
    /* If H2V1 with odd rotation pass to DSP as H1V2 */
    if ((pInfo->Main.Subsample == JPEGENC_H2V1) &&
        ((pInfo->Rotation == JPEGENC_CCLK90) || (pInfo->Rotation == JPEGENC_CCLK270)))
      pCmd->Enc.Subsample = (uint16)JPEGENC_H1V2;

    pCmd->Enc.Rotation = (uint16)pInfo->Rotation;
    /* 0 -  no rotation                   */
    /* 1 -  90 degrees CClockwise rotation*/
    /* 2 - 180 degrees CClockwise rotation*/
    /* 3 - 270 degrees CClockwise rotation*/
    pCmd->Enc.Fragments = pInfo->Main.FragCnt; /* Number of fragments    */
    pCmd->Enc.Restart   = pInfo->Main.Restart; /* Restart interval count */

    pCmd->Img.Width = pInfo->Main.Width;
    for (cnt=0; cnt < pCmd->Enc.Fragments; cnt++)
    {
      pCmd->Fragmnt[cnt] = pInfo->Main.Fragmnt[cnt];           
    }
  }
}

/*===========================================================================

Function: JPEGE_INIT

Description:  This function prepares the encoding information. Setups working
              variables and allocates memory for the header.
              
Return Value: None
   
Comments: This function knows when the entire header information is ready and it 
          will write the content in the temporary buffer to the destination
          accordingly and transition the state to encode the main image.

Side Effects:  jpege_info

============================================================================*/
static boolean jpege_init()
{
  char *p = NULL;
  uint16 chroma_height, chroma_width, cnt;
  uint16 luma_height_padded, luma_width_padded;
  uint16 chroma_height_padded, chroma_width_padded;
  uint32 padded_size;
  uint8 *luma_ptr;
  uint8 *chroma_ptr;
  boolean isH2V2;
    
  isH2V2 = (localEncodeInfo.Main.Subsample == JPEGENC_H2V2);
  jpege_get_dimensions(&luma_width_padded, &luma_height_padded, localEncodeInfo.Main.Width, 
                       localEncodeInfo.Main.Height, isH2V2, localEncodeInfo.Rotation,
                       JPEGE_TURN_OFF_MAX_SIZE, JPEGE_TURN_OFF_MAX_SIZE);

  if ((localEncodeInfo.Main.Width != luma_width_padded) || (localEncodeInfo.Main.Height != luma_height_padded))
  {
    padded_size = luma_width_padded * luma_height_padded * 2;
    padded_image = JPEG_MALLOC(padded_size);

    if (!padded_image)
    {
      MSG_ERROR("JPEGENC: Malloc failed for padding ", 0, 0, 0);
      return(FALSE);
    }

    luma_ptr = padded_image;
    chroma_ptr = padded_image+luma_width_padded * luma_height_padded;


    jpege_image_pad(localEncodeInfo.Main.Fragmnt[0].Luma_ptr, luma_ptr, localEncodeInfo.Main.Height,
                    localEncodeInfo.Main.Width,luma_height_padded, luma_width_padded);

    if (localEncodeInfo.Main.Subsample == JPEGENC_H2V2)
    {
      chroma_width = localEncodeInfo.Main.Width;
      chroma_height = localEncodeInfo.Main.Height/2;
      chroma_width_padded = luma_width_padded ;
      chroma_height_padded = luma_height_padded/2;

    }
    else
    {
      /* It is OK to shif right since dimensions are much less that uint16 */
      /*lint -save -e734 */ 
      chroma_width = localEncodeInfo.Main.Width*2; 
      chroma_height = localEncodeInfo.Main.Height/2;
      chroma_width_padded = luma_width_padded*2;
      chroma_height_padded = luma_height_padded/2;
      /*lint -restore */
    }

    jpege_image_pad(localEncodeInfo.Main.Fragmnt[0].Chroma_ptr, chroma_ptr, chroma_height,
                    chroma_width, chroma_height_padded, chroma_width_padded);
    localEncodeInfo.Main.Fragmnt[0].Luma_ptr = luma_ptr;
    localEncodeInfo.Main.Fragmnt[0].Chroma_ptr = chroma_ptr;
    localEncodeInfo.Main.Width = luma_width_padded;
    localEncodeInfo.Main.Height = luma_height_padded;

    for (cnt=0; cnt<localEncodeInfo.Main.FragCnt; cnt++)
    {
      localEncodeInfo.Main.Fragmnt[cnt].Height = luma_height_padded;
    }
  }

  /* if user does not request width and height, use input width and height */
  if ((localEncodeInfo.Main.OrigWidth == 0) ||
      (localEncodeInfo.Main.OrigHeight == 0))
  {
    localEncodeInfo.Main.OrigWidth = localEncodeInfo.Main.Width;
    localEncodeInfo.Main.OrigHeight = localEncodeInfo.Main.Height;
  }

  if ((localEncodeInfo.Thumbnail.OrigWidth == 0) ||
      (localEncodeInfo.Thumbnail.OrigHeight == 0))
  {
    localEncodeInfo.Thumbnail.OrigWidth = localEncodeInfo.Thumbnail.Width;
    localEncodeInfo.Thumbnail.OrigHeight = localEncodeInfo.Thumbnail.Height;
  }

  jpege_info.dst_format = localEncodeInfo.Dest;

  if (jpege_info.dst_format.device == JPEGENC_EFS)
  {
    jpege_info.filename = strchr(localEncodeInfo.Dest.handle.efs.filename, '/');

    while (jpege_info.filename)
    {
      p = jpege_info.filename + 1;
      jpege_info.filename = strchr(p, '/');
    }

    if (p)
    {
      jpege_info.filename = p;
    }
    else
    {
      jpege_info.filename = localEncodeInfo.Dest.handle.efs.filename;
    }

    jpege_info.filename_len = (uint16)strlen(jpege_info.filename);

    if (jpege_info.filename_len > FS_FILENAME_MAX_LENGTH_P)
    {
      jpege_info.filename_len = FS_FILENAME_MAX_LENGTH_P;
    }

    /* Handle the case of the file already exist */
    efs_unlink(localEncodeInfo.Dest.handle.efs.filename);

    jpege_info.file_descriptor = efs_open(localEncodeInfo.Dest.handle.efs.filename, O_CREAT | O_TRUNC | O_RDWR);
    if (jpege_info.file_descriptor == (-1))
    {
      jpeg_encode_failed = TRUE;
      MSG_ERROR("JPEGENC: EFS open failed. ", 0, 0, 0);
      return(FALSE);
    }
  }

  /* Now that the destination device is setup, setup header buffer */
  if (jpege_info.memData.buf_ptr == NULL)
  {
    jpege_info.memData.buf_size = JPEG_EXIF_HEADER_SIZE + 
#ifdef FEATURE_CAMERA_MOBICAT           
                                  mobicatStreamSize +
#endif
                                  (2*localEncodeInfo.Thumbnail.Width * localEncodeInfo.Thumbnail.Height);

    jpege_info.memData.buf_ptr = JPEG_MALLOC(jpege_info.memData.buf_size);
    if (jpege_info.memData.buf_ptr)
    {
      /* Use this as a working pointer */
      jpege_info.exif_buf = jpege_info.memData.buf_ptr;
      jpege_info.exif_buf_size = jpege_info.memData.buf_size;
      jpege_info.exif_data_size = 0;
    }
    else
    {
      jpeg_encode_failed = TRUE;
      MSG_ERROR("JPEGENC: out of memory", 0, 0, 0);
      return(FALSE);
    }
  }
  else
  {
    MSG_HIGH("JPEGENC: invalid header buffer memory situation", 0, 0, 0);
    return(FALSE);
  }

  return(TRUE);
}

/*===========================================================================

Function: JPEGE_PREPARE_HEADER

Description:  Writes the exif information to the temporary buffer before writing
              out to the destination.
              
Return Value: None
   
Comments: This function knows when the entire header information is ready and it 
          will write the content in the temporary buffer to the destination
          accordingly and transition the state to encode the main image.

Side Effects:

============================================================================*/
boolean jpege_prepare_header(exif_info_type *pExifData, JPEGENC_OrientType rotation)
{
  uint16 quality, i;
  exif_emit_type     emit_phase;
  exif_header_type   header;
  boolean  ret_val = TRUE; 
  double scaleFactor;
  byte *temp_exif_buffer = NULL;
  uint32 temp_exif_buffer_data_size = 0;
  int write_count=0;

  quality = localEncodeInfo.Main.Quality;
  /* Used in EFS & MEM modes for File Size Control Algorithm */    
  if (jpege_info.thumbnail == UP_TO_THUMBNAIL)
  { /* also check for exif_emit_state? */
    /* emit up to thumbnail for the header */
    if (jpegHeader == EXIF_JPEG_HEADER)
    {
      emit_phase = EXIF_EMIT_UPTO_THUMBNAIL;
    }
    else
    {
      emit_phase = JFXX_EMIT_UPTO_THUMBNAIL;
    }
    quality = localEncodeInfo.Thumbnail.Quality; 

    /* set compress subsample and restart marker */
    header.subsample = (exif_subsample_type)localEncodeInfo.Thumbnail.Subsample;
    exif_set_restart(pExifData, localEncodeInfo.Thumbnail.Restart);
  }
  else if (jpege_info.thumbnail == AFTER_THUMBNAIL)
  {
    /* emit the portion after the thumbnail but before the main image */
    if (jpegHeader == EXIF_JPEG_HEADER)
    {
      emit_phase = EXIF_EMIT_AFTER_THUMBNAIL;
    }
    else
    {
      emit_phase = JFXX_EMIT_AFTER_THUMBNAIL;
    }

    /* set compress subsample and restart marker */
    header.subsample = (exif_subsample_type)localEncodeInfo.Main.Subsample;
    exif_set_restart(pExifData, localEncodeInfo.Main.Restart);
  }
  else
  {
    /* No thumbnail required. Emit header up to before the main image */
    if (jpegHeader == EXIF_JPEG_HEADER)
    {
      emit_phase = EXIF_EMIT_ALL;
    }
    else
    {
      emit_phase = JFXX_EMIT_ALL;
    }

    /* set compress subsample and restart marker */
    header.subsample = (exif_subsample_type)localEncodeInfo.Main.Subsample;
    exif_set_restart(pExifData, localEncodeInfo.Main.Restart);
  }
  /*  must be in range 1 to 100             */
  /* Turn quality percent into scale factor */ 
  if (quality > 50)
  {
    scaleFactor = 50.0 / (float)(100-quality);
  }
  else
  { /* must be between 1 - 50 */
    scaleFactor = (float) quality / 50.0;
  }

  /* Now apply scale factor to quant tables uniformally */
  jpege_apply_scale_to_quant_tbls(SCALE_ALL_DC,
                                  scaleFactor,
                                  jpeg_luma_dqt_variable,
                                  jpeg_chroma_dqt_variable );
  header.type = emit_phase;
  header.color_space= EXIF_YCbCr;
  header.quality_percent = (int16)localEncodeInfo.Main.Quality;   /* Quality in percent */
  header.buf_ptr = jpege_info.memData.buf_ptr;     /* buffer head        */
  header.length  = jpege_info.memData.buf_size;    /* buffer size        */
  header.luma_dqt_table   = jpeg_luma_dqt_variable;/* Generated Quant table */
  header.chroma_dqt_table = jpeg_chroma_dqt_variable;/* Generated Quant table */

  if ((rotation == JPEGENC_CCLK90) || (rotation == JPEGENC_CCLK270))
  {
    /* Swap the Height and Width */
    header.main_width = localEncodeInfo.Main.OrigHeight;
    header.main_height= localEncodeInfo.Main.OrigWidth;
    header.tn_width   = localEncodeInfo.Thumbnail.OrigHeight;
    header.tn_height  = localEncodeInfo.Thumbnail.OrigWidth;
  }
  else
  {
    header.main_width = localEncodeInfo.Main.OrigWidth;
    header.main_height= localEncodeInfo.Main.OrigHeight; 
    header.tn_width   = localEncodeInfo.Thumbnail.OrigWidth;
    header.tn_height  = localEncodeInfo.Thumbnail.OrigHeight;
  }

  ret_val = exif_emit_header( pExifData, &header);
  localReturnData.header_size = pExifData->buf_index_save;  

  if (!ret_val)
  {
    jpeg_encode_failed = TRUE;
    MSG_ERROR("JPEGENC: Error in file header. Encode abort", 0, 0, 0);  
  }
  else
  {
    /* Keep the data size to know how many to write to the destination */
    jpege_info.exif_data_size = pExifData->buf_index;

    /* convert Q tables to Q16 multiplication for the DSP */
    for (i=0; i<64; i++)
    {
      jpeg_luma_dqt_variable[i]   = 
      (uint16) ((double)0xFFFF/(double)jpeg_luma_dqt_variable[i]);
      jpeg_chroma_dqt_variable[i] = 
      (uint16) ((double) 0xFFFF/(double)jpeg_chroma_dqt_variable[i]);
    }
  }

  if (jpege_info.thumbnail != UP_TO_THUMBNAIL)
  {
    /* Write exif in temporary bufer to destination */
    /* cumulative size of output */
    localReturnData.size = jpege_info.exif_data_size; 

    if (jpege_info.dst_format.device == JPEGENC_MEM)
    {
      if ((jpege_info.dst_format.handle.mem.length >= jpege_info.exif_data_size) &&
          (jpege_info.dst_format.handle.mem.buffer != NULL))
      {
        /* Output is buffer, copy to client buffer */
        memcpy(jpege_info.dst_format.handle.mem.buffer, 
               jpege_info.exif_buf, jpege_info.exif_data_size);
        jpege_info.dst_format.handle.mem.buffer += jpege_info.exif_data_size;
        jpege_info.dst_format.handle.mem.length -= jpege_info.exif_data_size;
      }
      else
      {
        /* If in file size control just accumulate statistics from DSP */
        if (!in_file_size_ctrl)
        {
          MSG_HIGH("JPEGENC: insufficient destination buffer size", 0, 0, 0);
          /* Nothing was written to buffer */
          localReturnData.size = 0;
          ret_val = FALSE;
        }
      }
    }
    else
    { /* dst_format.device == EFS */
      /* Write data to the EFS file */
      write_count = efs_write(jpege_info.file_descriptor, jpege_info.exif_buf, 
               jpege_info.exif_data_size);
      temp_exif_buffer = jpege_info.exif_buf;
      temp_exif_buffer_data_size = jpege_info.exif_data_size;

      if (write_count == (-1))
      {
        MSG_HIGH("JPEGENC: Error writing to efs", 0, 0, 0);
        ret_val = FALSE;
      }
      else
      {  
        //efs_write() some times writes fewer bytes than requested so below while loop is to handle that.	  
        while ((unsigned int)write_count < temp_exif_buffer_data_size)
        {
          temp_exif_buffer += write_count;
          temp_exif_buffer_data_size -= (unsigned)write_count;
          write_count = efs_write( jpege_info.file_descriptor, temp_exif_buffer , temp_exif_buffer_data_size );
          if (write_count == (-1))
          {
            MSG_HIGH("JPEGENC: Error writing to efs", 0, 0, 0);
            ret_val = FALSE;
            break;
          }
        }
      }
    }

    if (jpege_info.memData.buf_ptr)
    {
      JPEG_FREE(jpege_info.memData.buf_ptr);
      jpege_info.memData.buf_ptr = NULL;
    }
    /* All header info. has been complete. Only the main image left */
    jpege_info.thumbnail = FULL_HEADER;
  }
  return(ret_val); 
}

/*===========================================================================

Function: JPEGE_WRITE_THUMBNAIL_IMG

Description:  Writes the encoded bit-stream to the thumbnail image location
              in the temporary buffer.
              
Return Value: None
   
Comments: This function knows when the thumbnail encoding is done and it 
          will transition the state to encode the main image. An idle command
          is issued to signify the end of thumbnail encoding and the starting 
          of the main image encoding.

Side Effects:localReturnData, jpege_info

============================================================================*/
void jpege_write_thumbnail_img(JPEGE_outputMsgBufType buffer)
{
  JPEGE_OutBufCfgType  output_consummed;
  JPEGQDSPReturnCodeType retCode = JPEGQDSP_SUCCESS;

  if ((buffer.Ptr != outputBuf[0].Ptr) && (buffer.Ptr != outputBuf[1].Ptr))
  {
    MSG_ERROR("JPEGENC: buffer pointer not original", 0, 0, 0);
    return;
  }
  MSG_LOW("JPEGENC: TN image writing to efs", 0, 0, 0);

  /* Disable the mmu cache before reading the data generated by DSP */
  JPEG_INVALIDATE_DATA_CACHE (buffer.Ptr, OUTPUT_BUF_SIZE);  /*lint !e826 */

  if ((buffer.Status) && (*(buffer.Ptr + buffer.Size -1) == 0))
  {
    buffer.Size--;
  }

  /* cumulative size of output */
  localReturnData.size += buffer.Size;

  /* Check to make sure no data overflow before copy data */
  if ((jpege_info.exif_data_size + buffer.Size) < jpege_info.exif_buf_size)
  {
    memcpy(jpege_info.exif_buf+jpege_info.exif_data_size, buffer.Ptr, buffer.Size);
    jpege_info.exif_data_size += buffer.Size;

    /* Buf_index is where the next exif data should be written */
    exif_data_ptr->buf_index += buffer.Size;
  }
  else
  {
    MSG_HIGH("JPEGENC: insufficient buffer size", 0, 0, 0);
  }

  if (buffer.Status)
  {
    /* Done with thumbnail encoding */
    /* thumbnail size is already included in the buf_index of exif */

    if (exif_data_ptr->buf_index > EXIF_MAX_APP1_LENGTH)
    {
      localReturnData.dcBitCnt = 0;
    }
    else
    {
      localReturnData.dcBitCnt = buffer.DC_count;
    }

    if (jpegHeader   == EXIF_JPEG_HEADER)
    {
      if (exif_update_thumbnail(exif_data_ptr, 0) == FALSE)
      {
        MSG_HIGH("JPEGENC: Update thumbnail size failed", 0, 0, 0);
      }
    }
    else
    {
      if (jfif_update_thumbnail(exif_data_ptr, 0) == FALSE)
      {
        MSG_HIGH("JPEGENC: Update thumbnail size failed", 0, 0, 0);
      }
    }

    /* Update exif emit state and send idle command */
    jpege_info.thumbnail = AFTER_THUMBNAIL;

    /* This command is a way of confirming the DSP state */
    /* and get the proper transition for the main image encoding */
    retCode = jpeg_qdsp_command(JPEGE_IDLE, NULL);
  }
  else
  {
    /* Notify DSP with an output consummed command */ 
    output_consummed.Ptr = buffer.Ptr;
    output_consummed.Size = OUTPUT_BUF_SIZE;
    retCode = jpeg_qdsp_command(JPEGE_OUTPUT_CONSUMED, &output_consummed);
  }

  if (retCode != JPEGQDSP_SUCCESS)
  {
    jpeg_encode_failed = TRUE;
    jpege_process_callback( jpeg_session_id, (int16)JPEGENC_DONE );
  }
}

/*===========================================================================

Function: JPEGE_WRITE_MAIN_IMG

Description:  Writes the encoded bit-stream to the main image location
              according to the destination specification.
              
Return Value: None
   
Comments: This function knows when the entire encoding is done and it 
          will set the state accordingly, free the memory, disable the dsp 
          and notify the client.

Side Effects: localReturnData, jpege_info

============================================================================*/
void jpege_write_main_img(JPEGE_outputMsgBufType buffer)
{
  JPEGE_OutBufCfgType  output_consummed;
  JPEGQDSPReturnCodeType retCode = JPEGQDSP_SUCCESS;
  uint8 *temp_buffer_ptr = NULL;
  uint32 temp_buffer_size = 0;
  int write_count=0;

  if ((buffer.Ptr != outputBuf[0].Ptr) && (buffer.Ptr != outputBuf[1].Ptr))
  {
    MSG_ERROR("JPEGENC: buffer pointer not original", 0, 0, 0);
    return;
  }
  MSG_LOW("JPEGENC: main image writing to efs", 0, 0, 0);

  /* Disable the mmu cache before reading the data generated by DSP */
  JPEG_INVALIDATE_DATA_CACHE (buffer.Ptr, OUTPUT_BUF_SIZE);  /*lint !e826 */

  if ((buffer.Status) && (*(buffer.Ptr + buffer.Size -1) == 0))
  {
    buffer.Size--;
  }

  /* cumulative size of output */
  localReturnData.size += buffer.Size;

  /* Write the additional encoded stream to the destination */
  if (jpege_info.dst_format.device == JPEGENC_MEM)
  {
    if (jpege_info.dst_format.handle.mem.length >= buffer.Size)
    {
      /* Output is buffer, copy to user buffer */
      memcpy(jpege_info.dst_format.handle.mem.buffer, buffer.Ptr, buffer.Size);
      jpege_info.dst_format.handle.mem.buffer += buffer.Size;
      jpege_info.dst_format.handle.mem.length -= buffer.Size;
    }
    else
    {
      /* If in file size control just accumulate statistics from DSP */
      if (!in_file_size_ctrl)
      {
        MSG_HIGH("JPEGENC: insufficient destination buffer size", 0, 0, 0);

        /* Subtract last buffer which is not written */
        localReturnData.size -= buffer.Size;
        jpeg_encode_failed = TRUE;
      }
    }
  }
  else
  { /* dst_format.device == EFS */
    /* Write data to the EFS file */
    temp_buffer_ptr = buffer.Ptr;
    temp_buffer_size = buffer.Size;
    write_count = efs_write(jpege_info.file_descriptor, temp_buffer_ptr, 
             temp_buffer_size);

    if (write_count == (-1))
    {
      MSG_HIGH("JPEGENC: Error writing to efs", 0, 0, 0);
      jpeg_encode_failed = TRUE;
    }
    else
    {
      //efs_write() some times writes fewer bytes than requested so below while loop is to handle that.	  
      while ((unsigned int)write_count < temp_buffer_size)
      {
        temp_buffer_ptr += write_count;
        temp_buffer_size -= (unsigned)write_count;
        write_count = efs_write( jpege_info.file_descriptor, temp_buffer_ptr , temp_buffer_size );
        if (write_count == (-1))
        {
          MSG_HIGH("JPEGENC: Error writing to efs", 0, 0, 0);
          jpeg_encode_failed = TRUE;
          break;
        }
      }
    }
  }

  if (buffer.Status)
  { /* Done with main image encoding */
    /* Done with image encoding. Read the DC bit count */
    localReturnData.dcBitCnt += buffer.DC_count;

    /* Prepare the destination data */
    if (jpege_info.dst_format.device == JPEGENC_MEM)
    {
      localReturnData.mode   = JPEGENC_MEM;
      localReturnData.outPtr = localEncodeInfo.Dest.handle.mem.buffer;
    }
    else
    { /* dst_format.device == EFS */
      localReturnData.mode   = JPEGENC_EFS; 
      localReturnData.outPtr = NULL;
    }

    /* Call the state transition routine to handle the callback */
    jpege_process_callback( jpeg_session_id, (int16)JPEGENC_DONE );

  } /* if (buffer.Status */
  else
  {
    /* Notify DSP with an output consummed command */ 
    output_consummed.Ptr = buffer.Ptr;
    output_consummed.Size = OUTPUT_BUF_SIZE;
    retCode = jpeg_qdsp_command(JPEGE_OUTPUT_CONSUMED, &output_consummed);
  }

  if (retCode != JPEGQDSP_SUCCESS)
  {
    jpeg_encode_failed = TRUE;
    jpege_process_callback( jpeg_session_id, (int16)JPEGENC_DONE );
  }
}

/*===========================================================================

Function: JPEGE_PROCESS_CALLBACK

Description:  This function executes in the GRAPH task and it behaves as a
              state machine for the JPEG encoder.
              
Return Value: None
   
Comments:  

Side Effects: jpeg_encode_abort. jpeg_encode_failed, jpege_in_shutdown_process.
              localReturnData, 
============================================================================*/
static void jpege_process_callback(int32 clientId, int32 clientData)
{
  JPEGENC_operationType  operation;
  JPEGE_encodeCmdType    encodeCmdData;
  JPEGQDSPReturnCodeType retCode = JPEGQDSP_SUCCESS;
  int16  i;

  MSG_LOW("JPEGENC: in callback function %d %d %d", 0, 0, clientData);

  if (clientId != jpeg_session_id)
  {
    MSG_HIGH("JPEGENC: invalid callback session ID",0,0,0);
    return;
  }

  operation = (JPEGENC_operationType) clientData;

  /* Abort is external and failure is from within */
  if (jpeg_encode_abort || jpeg_encode_failed)
  {
    jpege_in_shutdown_process = TRUE;
    if (jpeg_encode_abort)
    {
      localReturnData.status = JPEGENC_IMG_ABORT;
      MSG_HIGH("JPEGENC: abort jpeg encoder", 0, 0, 0);
    }
    else
    {
      localReturnData.status = JPEGENC_IMG_FAIL;
      MSG_HIGH("JPEGENC: jpeg encoder failed", 0, 0, 0);
    }
    jpeg_encode_abort = 0; 
    jpeg_encode_failed = 0;
    /* if abort is issued before DSP is downloaded */
    if (operation == JPEGENC_DOWNLOAD)
    {
      operation = JPEGENC_IDLE;
    }
    /* Command the DSP to IDLE state and ready to terminate the encoder */ 
    else 
    {
      (void)jpeg_qdsp_command(JPEGE_IDLE, NULL);
      return;   
    }
  }

  switch (operation)
  {
    case JPEGENC_DOWNLOAD:
      /* Download the DSP image */
      (void)jpeg_qdsp_initialize(jpege_handle_dsp_CB, JPEG_ENCODER_DSP);

#ifdef FEATURE_NEW_SLEEP_API
      sleep_negate_okts(jpeg_sleep_okts_handle);
#endif
      /* allocate the ping pong buffers here */
      for (i=0; i<2; i++)
      {
#ifdef CACHE_POLICY_WRITE_BACK
        if ((outputBuf_malloc[i].Ptr = JPEG_MALLOC( OUTPUT_BUF_SIZE+CACHE_LINE_SIZE )) != NULL)
        {
          outputBuf[i].Ptr = (uint8*)((((uint32)outputBuf_malloc[i].Ptr+CACHE_LINE_SIZE-1)/CACHE_LINE_SIZE)*CACHE_LINE_SIZE);
#else
        if ((outputBuf[i].Ptr = JPEG_MALLOC( OUTPUT_BUF_SIZE )) != NULL)
        {
#endif
          outputBuf[i].Size = OUTPUT_BUF_SIZE;
        }
        else
        {
          MSG_ERROR("JPEGENC: out of memory", 0, 0, 0);
          jpeg_encode_failed = TRUE;
          return;
        }
      }
      localReturnData.size =0;
      localReturnData.dcBitCnt =0;
      if (!jpege_init())
      {
        jpeg_encode_failed = TRUE;
        return;
      }

      break;
    case JPEGENC_ENCODE_IMG:
      /* Image just downloaded. Generates the file header first */
      if (!jpege_prepare_header(exif_data_ptr, localEncodeInfo.Rotation ))
      {
        jpeg_encode_failed = TRUE;

        /* Queue the main image encoding command on the GRAPH task */
        context_data.client_data = (int16)JPEGENC_ENCODE_IMG;
        context_data.function_ptr= jpege_process_callback;
        context_data.client_ID   = jpeg_session_id;
        graph_context_req( context_data );
        return;
      }

      jpege_setup_encode_cmd(&localEncodeInfo, &encodeCmdData);
      retCode = jpeg_qdsp_command( JPEGE_ENCODE_CMD, &encodeCmdData); 

      if (retCode != JPEGQDSP_SUCCESS)
      {
        localReturnData.clientId = localEncodeInfo.ClientId;
        jpeg_encoder_terminate();

        if (localClientCB_func)
        {
			/* restore the pointers to the caller,  so original buffers are freed.  */
			localReturnData.encodeSpec->Main.Fragmnt[0].Luma_ptr = main_luma_ptr;
			localReturnData.encodeSpec->Main.Fragmnt[0].Chroma_ptr = main_chroma_ptr;
			localReturnData.encodeSpec->Thumbnail.Data.Luma_ptr=thumbnail_luma_ptr;
			localReturnData.encodeSpec->Thumbnail.Data.Chroma_ptr=thumbnail_chroma_ptr;

			localClientCB_func(&localReturnData);
        }
      }

      break;
    case JPEGENC_READ_BUFFER1:
      if (jpege_info.thumbnail != FULL_HEADER)
      {
        /* Write data at the thunbnail space */
        jpege_write_thumbnail_img(buffer1);
      }
      else
      { /* Data for the main image */
        jpege_write_main_img(buffer1);
      }
      break;

    case JPEGENC_READ_BUFFER2:
      if (jpege_info.thumbnail != FULL_HEADER)
      {
        /* Write data at the thunbnail space */
        jpege_write_thumbnail_img(buffer2);
      }
      else
      { /* Data for the main image */
        jpege_write_main_img(buffer2);
      }
      break;

    case JPEGENC_IDLE:
      if (jpege_in_shutdown_process)
      {
        localReturnData.clientId = localEncodeInfo.ClientId;
        jpeg_encoder_terminate();

        if (jpege_info.dst_format.device == JPEGENC_EFS)
        {
          efs_close(jpege_info.file_descriptor);
          /* If abort command or JPEG failed before writing to file       
             remove file */
          efs_unlink(localEncodeInfo.Dest.handle.efs.filename);
        }

        if (localClientCB_func)
        {
			    /* restore the pointers to the caller,  so original buffers are freed.  */
			    localReturnData.encodeSpec->Main.Fragmnt[0].Luma_ptr = main_luma_ptr;
			    localReturnData.encodeSpec->Main.Fragmnt[0].Chroma_ptr = main_chroma_ptr;
			    localReturnData.encodeSpec->Thumbnail.Data.Luma_ptr=thumbnail_luma_ptr;
			    localReturnData.encodeSpec->Thumbnail.Data.Chroma_ptr=thumbnail_chroma_ptr;

			    localClientCB_func(&localReturnData);
        }
        else
        {
          MSG_HIGH("JPEGENC: Null function pointer", 0, 0, 0);
        }
        return;
      }
      /* Use this state to transition from thumbnail image to main image */
      if (jpege_info.thumbnail == AFTER_THUMBNAIL)
      {
        /* Queue the main image encoding command on the GRAPH task */
        context_data.client_data = (int16)JPEGENC_ENCODE_IMG;
        context_data.function_ptr= jpege_process_callback;
        context_data.client_ID   = jpeg_session_id;
        graph_context_req( context_data );
        localEncodeInfo.HasThumbnail = FALSE;
      }
      break;
    case JPEGENC_DONE:

      /* Send encode done status to client */
      localReturnData.clientId = localEncodeInfo.ClientId;
      localReturnData.status = JPEGENC_IMG_DONE;

      if (jpege_info.dst_format.device == JPEGENC_EFS)
      {
        efs_close(jpege_info.file_descriptor);
      }

      /* Clean up the jpeg encoder space */
      jpeg_encoder_terminate();
      if (localClientCB_func)
      {
		    /* restore the pointers to the caller,  so original buffers are freed.  */
		    localReturnData.encodeSpec->Main.Fragmnt[0].Luma_ptr = main_luma_ptr;
		    localReturnData.encodeSpec->Main.Fragmnt[0].Chroma_ptr = main_chroma_ptr;
		    localReturnData.encodeSpec->Thumbnail.Data.Luma_ptr=thumbnail_luma_ptr;
		    localReturnData.encodeSpec->Thumbnail.Data.Chroma_ptr=thumbnail_chroma_ptr;

		    localClientCB_func(&localReturnData);
      }
      else
      {
        MSG_HIGH("JPEGENC: Null function pointer", 0, 0, 0);
      }

      break;
    default:
      MSG_HIGH("JPEGENC: unrecognize encoding operation",0,0,0);
  }

  if (retCode != JPEGQDSP_SUCCESS)
  {
    jpeg_encode_failed = TRUE;
    jpege_process_callback( jpeg_session_id, (int16)JPEGENC_DONE );
  }
}

/*===========================================================================

FUNCTION      JPEGE_APPLY_SCALE_TO_QUANT_TBLS

DESCRIPTION
  Builds both luminance and chrominance JPEG quantization tables
  based a floating point scale factor (divisor of the table values)
  Note that the DC component may be optionally scaled with the
  AC components (DC component is index 0 of both tables).
  Note the following formulas to translate from "percent quality
  (a number from 1 to 99) to the floating point "scaleFactor"
  which this function accepts as input:

  if(quality_percent <= 50)
    scaleFactor = (quality_percent / 50)
  else if(quality_percent > 50)
    scaleFactor = 50 / (100 - quality_percent)

INPUTS
  dcScalingOptionsType dcScale - flag on how to scale the DC component
    DO_NOT_SCALE_DC,    -Keep quant. values as defaults
    SCALE_GREATER_QUALITY_ONLY_DC, -Scale down for greater qual only
    SCALE_ALL_DC        -Scale up or down

  double scaleFactor - Value used to directly divide the Quant tables
    (scale) for a given quality.

  quantization_type * luma_dqt_table_ptr - pointer to luma output table

  quantization_type * chroma_dqt_table_ptr - pointer to chroma output table

DEPENDENCIES
  On Fixed input arrays : exif_luma_dqt_low & exif_chroma_dqt_low

RETURN VALUE

SIDE EFFECTS
  Replace 64 elements of data per quant table pointer passed in.

===========================================================================*/
static void jpege_apply_scale_to_quant_tbls (dcScalingOptionsType dcScale,
                                             double scaleFactor,
                                             uint16 * luma_dqt_table_ptr,
                                             uint16 * chroma_dqt_table_ptr )
{
  int16 i;

  /* No reason to go lower in quality than this */
  /* 0.02 corresponds to 1% quality!! */
  if (scaleFactor < ONE_PERCENT_SCALE_FACTOR)
    scaleFactor = ONE_PERCENT_SCALE_FACTOR;

  /* No reason to go higher in quality than this */
  /* 50 corresponds to 99% quality!! */
  /* 5 corresponds to 90% quality!! */
  /* Apply this formula to convert from % quality to scale factor:
     scale factor = 50/(100-quality%) if quality is > 50
     else scale factor = quality%/50         */
  /* Found empirically that 99% quality doesn't show up correctly
     but 98 and below is OK, so use 98 max */
  if (scaleFactor > NINTY_EIGHT_PERCENT_SCALE_FACTOR)
  {
    scaleFactor = NINTY_EIGHT_PERCENT_SCALE_FACTOR;
  }

  /* Special processing for the DC component (index zero of table) */
  /* below default quality threshold (currently 50%)   */

  /* First, assume (dcScale == DO_NOT_SCALE_DC) and keep quant. values as defaults */
  /* Copy default quant params that correspond to 50% quality */
  luma_dqt_table_ptr[0] = exif_luma_dqt_low[0];
  chroma_dqt_table_ptr[0] = exif_chroma_dqt_low[0];

  /* Scale down for greater qual only */
  if (dcScale == SCALE_GREATER_QUALITY_ONLY_DC)
  {
    if (scaleFactor > 1)
    {
      /* Only scale DC up */
      luma_dqt_table_ptr[0] = (uint16) (((double)exif_luma_dqt_low[0] / scaleFactor)+0.5);
      chroma_dqt_table_ptr[0] = (uint16) (((double)exif_chroma_dqt_low[0] / scaleFactor)+0.5);
    }
  }
  else if (dcScale == SCALE_ALL_DC)
  {      /* Scale up or down */
    /* Scale up or down */
    luma_dqt_table_ptr[0] = (uint16) (((double)exif_luma_dqt_low[0] / scaleFactor)+0.5);
    chroma_dqt_table_ptr[0] = (uint16) (((double)exif_chroma_dqt_low[0] / scaleFactor)+0.5);
  }

  /* Cap the upper and lower bounds of the resulting DC value */
  if (luma_dqt_table_ptr[0] < 1)
    luma_dqt_table_ptr[0]   = 1;
  if (chroma_dqt_table_ptr[0] < 1)
    chroma_dqt_table_ptr[0] = 1;

  if (luma_dqt_table_ptr[0] > 255)
    luma_dqt_table_ptr[0]   = 255;
  if (chroma_dqt_table_ptr[0] > 255)
    chroma_dqt_table_ptr[0] = 255;

  /* Scale all AC compoent quant entries */
  for (i = 1; i < 64; i++)
  {

    /* Compute new luma or chroma based on input percent */
    /* and on the 50% table (low)                        */
    /* Add 0.5 after the divide to round up fractional divides to be
       more conservative.  */
    luma_dqt_table_ptr[i] = (uint16) (((double)exif_luma_dqt_low[i] / scaleFactor)+0.5);
    chroma_dqt_table_ptr[i] = (uint16) (((double)exif_chroma_dqt_low[i] / scaleFactor)+0.5);

    /* Cap the upper and lower bounds of the resulting table */
    if (luma_dqt_table_ptr[i] < 1)
    {
      luma_dqt_table_ptr[i]   = 1;
    }

    if (chroma_dqt_table_ptr[i] < 1)
    {
      chroma_dqt_table_ptr[i] = 1;
    }

    if (luma_dqt_table_ptr[i] > 255)
    {
      luma_dqt_table_ptr[i]   = 255;
    }

    if (chroma_dqt_table_ptr[i] > 255)
    {
      chroma_dqt_table_ptr[i] = 255;
    }
  }
}

/*===========================================================================

Function: JPEGE_ENCODER_TERMINATE

Description:  This function disables the DSP, free all memory and reset the 
              operation states.
              
Return Value: None
   
Comments:  

Side Effects: jpeg_encoding, jpeg_encode_abort,jpeg_encode_failed,  
              jpege_in_shutdown_process, jpege_info, padded_image
============================================================================*/
static void jpeg_encoder_terminate()
{
  int i;

  jpeg_encoding = FALSE;
  jpeg_encode_abort  = FALSE;
  jpeg_encode_failed = FALSE;
  jpege_in_shutdown_process = FALSE;

  jpege_terminate();
#ifdef CACHE_POLICY_WRITE_BACK
  JPEG_FREE(original_thumb_luma_ptr);
  original_thumb_luma_ptr = NULL;
  JPEG_FREE(original_thumb_chroma_ptr);
  original_thumb_chroma_ptr = NULL;		
#endif

  for (i=0; i<2; i++)
  {
#ifdef CACHE_POLICY_WRITE_BACK
    outputBuf[i].Ptr = outputBuf_malloc[i].Ptr;
#endif
    if (outputBuf[i].Ptr)
    {
      JPEG_FREE(outputBuf[i].Ptr);
      outputBuf[i].Ptr = NULL;
    }
  }

  if (jpege_info.memData.buf_ptr)
  {
    JPEG_FREE(jpege_info.memData.buf_ptr);
    jpege_info.memData.buf_ptr = NULL;
  }

  /* If padding was performed free dst buffer */
  if (padded_image)
  {
    JPEG_FREE(padded_image);
    padded_image = NULL;
  }

  if (exif_data_ptr == &jfifHdrData)
  {
    exif_terminate(&jfifHdrData);
  }

#ifdef FEATURE_CAMERA_MOBICAT         
  if (mobicatStreamPtr)
  {
    /* Clear stream pointer and size */
    JPEG_FREE(mobicatStreamPtr);
    mobicatStreamPtr = NULL;
    mobicatStreamSize = 0;
  }
#endif

#ifdef FEATURE_NEW_SLEEP_API
  sleep_assert_okts(jpeg_sleep_okts_handle);
#endif
}

/*===========================================================================

Function: JPEGE_ABORT

Description:  This function is used to abort the on going encoding session.
              
Return Value:
   JPEGENC_SUCCESS      - cmd sucessful
   JPEGENC_BAD_ID       - Bad client ID for this command
   
Comments: If JPEGENC_SUCCESS, caller should wait for the callback from the
           encoding session to properly abort the session. 

Side Effects: jpeg_encode_abort

============================================================================*/
void  jpeg_encoder_abort
(
uint16      ClientId                 /* Use the same client ID for the   */
)                                     /*  encoding session                */
{
  jpeg_encode_abort = TRUE;
  /*lint -save -e715*/
}
/*lint -restore */

/*===========================================================================

Function: JPEGE_HORIZONTAL_UPSAMPLE

Description:  This function is upsamples 2D CrCb  array in horizontal dimension
              by linear interpolation
              
Return Value:
   NOne
   
Comments: Upsampling is done in-place (the array should be twice the size of  
           original data
Side Effects:

============================================================================*/
void jpege_horizontal_upsample(uint8 *image, uint16 height, uint16 width)
{
  uint16 i,j;
  uint8 *ptr_orig_im;
  uint8 *ptr_upsample_im;

  /* Place pointer at the end of input buffer */
  ptr_orig_im=image+height*width*2-1;

  /* Place pointer at the end of output buffer */
  ptr_upsample_im=image+height*width*4-1;
  for (i=0;i<height;i++)
  {
    /* End of line just duplicate chroma */
    *ptr_upsample_im--=*ptr_orig_im; // Copy Cr
    *ptr_upsample_im--=*(ptr_orig_im-1); // Copy Cb
    *ptr_upsample_im--=*ptr_orig_im; // Duplicate Cr
    *ptr_upsample_im--=*(ptr_orig_im-1); // Duplicate Cb

    for (j=0;j<width-1;j++)
    {
      /* Filter Cr - use adjacent Cr samples */
      *ptr_upsample_im-- = JPEGE_AVER_CHROMA(*ptr_orig_im,*(ptr_orig_im-2));
      /* Filter Cb - use adjacent Cb samples */
      *ptr_upsample_im-- = JPEGE_AVER_CHROMA(*(ptr_orig_im-1),*(ptr_orig_im-3));

      /* Copy Cr and Cb */
      *ptr_upsample_im-- = *(ptr_orig_im-2);
      *ptr_upsample_im-- = *(ptr_orig_im-3);
      ptr_orig_im-=2;
    }
    /* Move input pointer to the end of next line */
    ptr_orig_im-=2;
  }
}

/*===========================================================================

Function: JPEGE_IMAGE_PAD

Description:  The function padd image with sizes height x width to
               image height_new x width_new (if at least one of the
               new dimensions is bigger)

Return Value:
   None

Comments:
Side Effects:

============================================================================*/

void jpege_image_pad (uint8 *image, uint8 *image_new, uint16 height,
                      uint16 width, uint16 heightNew, uint16 widthNew)
{
  uint16 i,j;

  /* Check if at least one of the new dimensions is greater */
  if ((height >= heightNew) && (width>=widthNew))
  {
    return;
  }

  /* Padding the width (pitch) */
  for (i=0; i<height;i++)
  {
    for (j=0; j<width;j++)
    {
      *image_new++ = *image++; // Copy to new image
    }
    image--;                     // Get last element of the row
    for (j=0; j<widthNew-width;j++)
    {
      *image_new++ = *image;   // Fill the rest with last element
    }    
    image++;
  }

  /* Padding the height */
  for (i=0; i<heightNew-height; i++)
  {
    image -= width;
    for (j=0; j<width ;j++)
    {
      *image_new++ = *image++;  // Repeat last line
    }
    image--;
    for (j=0; j<widthNew-width ;j++)
    {
      *image_new++ = *image;    // Extend with last element
    }
    image++;                      // of last line
  }
}

/*===========================================================================

Function: JPEGE_VERTICAL_UPSAMPLE

Description:  This function is upsamples 2D CrCb array in vertical dimension
              by linear interpolation
              
Return Value:
   NOne
   
Comments: Upsampling is done in-place (the array should be twice the size of  
           original data
Side Effects:

============================================================================*/
void jpege_vertical_upsample(uint8 *image, uint16 height, uint16 width)
{
  uint16 i,j;
  uint8 *ptr_orig_im;
  uint8 *ptrPrevRow;
  uint8 *ptrNextRow;
  uint8 *ptr_upsample_im;

  /* Place pointer at the end of input buffer */
  ptr_orig_im = image+height*width*2-1;

  /* Place pointer at last pixel */
  ptr_upsample_im=image+height*width*4-1;

  /* Copy last two rows */
  for (i=0;i<2*width;i++)
  {
    *ptr_upsample_im-- = *ptr_orig_im--;
  }

  ptr_orig_im += 2*width;

  for (i=0;i<2*width;i++)
  {
    *ptr_upsample_im-- = *ptr_orig_im--;
  }

  ptr_upsample_im -= 2*width;

  /* Copy the even rows (no change) */
  for (i=0;i<height-1;i++)
  {
    for (j=0;j<2*width;j++)
    {
      *ptr_upsample_im-- = *ptr_orig_im--;
    }
    ptr_upsample_im -= 2*width;
  }

  ptrPrevRow = image;
  ptrNextRow = ptrPrevRow + 4*width;
  ptr_upsample_im = image + 2*width;

  /* Fill the odd rows (average) */
  for (i=0;i<height-1;i++)
  {
    for (j=0;j<2*width;j++)
    {
      *ptr_upsample_im++ = JPEGE_AVER_CHROMA(*ptrPrevRow++,*ptrNextRow++);
    }
    ptr_upsample_im += 2*width;
    ptrPrevRow += 2*width;
    ptrNextRow += 2*width;
  }      
}

/*===========================================================================

Function: JPEGE_VERTICAL_SUBSAMPLE

Description:  This function subsamples 2D CrCb array in vertical dimension
              by averaging
              
Return Value:
   NOne
   
Comments: Subsampling is done in-place 
Side Effects:

============================================================================*/
void jpege_vertical_subsample(uint8 *image, uint16 height, uint16 width)
{
  uint8 *prevLinePtr;
  uint8 *nextLinePtr;
  uint8 *subsampledPtr;
  uint16 i,j;

  prevLinePtr = image;
  nextLinePtr = image + 2*width;
  subsampledPtr = image;

  for (i=0; i<height/2; i++)
  {
    for (j=0;j<2*width; j++)
    {
      /* Average prevLine and nextline */
      *subsampledPtr++ = (uint8)(((uint16)(*prevLinePtr++) + (uint16)(*nextLinePtr++))>>1);
    }
    prevLinePtr += 2*width;
    nextLinePtr += 2*width;
  }
}


/*===========================================================================

Function: JPEGE_LOG2Q8

Description:  This function computes log2(x) using table look up. The result is 
              returned in Q8
              
Return Value:
   log2(x) in Q8
   
Comments:  
Side Effects:

============================================================================*/
static  int16 jpege_log2Q8(uint16 x)
{
  int8 scale=0;
  uint16 index=0;

  if (x==0)
  {
    return(0);
  }

  index = x;

  /* Shift x(index) till less than 63 (within table) */
  while (index>63)
  {
    scale++;
    index = index >>1;
  }
  /* Shift x with rounding  */
  index = (x+(uint16)(1<<(scale-1)))>>scale;

  /* Compensate for the shift and scale */
  /*lint -save -e734 scale much less than uint8 */

 if (index != NULL)
{
  return(jpegLogTable[index-1]+scale*(1<<8));
}
else
{
   return(0);
}
  /*lint -restore */ 
}

/*===========================================================================

Function: JPEGE_GET_DC_LUMA

Description:  This function perform 8x8 summing per MCU for luma part
              
Return Value:
   NOne
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_get_dc_luma(uint8 *image, int16 *dc, uint16 height, uint16 width)
{
  uint16 i,j;
  uint16 blockHeight,blockWidth;

  blockHeight = height/8;
  blockWidth = width/8;

  for (i=0; i< blockHeight;i++)
  {
    for (j=0; j< blockWidth;j++)
    {
      dc[INDEX(i,j,blockWidth)] = jpege_sum_8x8_luma(image+INDEX(i*8,j*8,blockWidth*8),width);
    }
  }
}

/*===========================================================================

Function: JPEGE_GET_DC_CHROMA

Description:  This function perform 8x8 summing per MCU for chroma part
              
Return Value:
   NOne
   
Comments:  Interleaved format is expected for chroma
Side Effects:

============================================================================*/
static void jpege_get_dc_chroma(uint8 *image, int16 *dc, uint16 height, uint16 width)
{
  uint16 i,j;
  uint16 blockHeight,blockWidth;

  blockHeight = height/8;
  blockWidth = width/8;

  for (i=0; i< blockHeight;i++)
  {
    for (j=0; j< blockWidth;j++)
    {
      dc[INDEX(i,j,blockWidth)] = 
      jpege_sum_8x8_chroma(image+INDEX(i*8,j*16,blockWidth*16),width);
    }
  }
}

/*===========================================================================

Function: JPEGE_DC_ENTROPY

Description:  This function computes the histogram and the entropy 
              
Return Value:
   DC entropy in Q8
   
Comments:  
Side Effects:

============================================================================*/
static  int16 jpege_dc_entropy(int16 *image, uint16 height, uint16 width, 
                               uint16 scale, uint16* midRange)
{
  int16 index;
  uint16 i,j;
  uint16 *hist;
  int32 entropy=0;
  int32 tmp32;


  hist = jpegeFileSizeEstiBufPtr;

  if (scale == 0)
  {
    scale = 1;
  }

  //hist = (uint16 *)JPEG_MALLOC(2*(MAX_INDEX+1)*sizeof(uint16));

  //if (hist == NULL)
  //  return(0);

  /* Zero histogram bins */
  for (i=0;i<2*MAX_INDEX;i++)
    hist[i]=0;

  /* Compute histogram */
  for (i=0; i< height;i++)
  {
    for (j=0; j< width;j++)
    {
      index = *image++;
      tmp32 = (int32)index * (int32)scale +(1<<7);
      index = (int16)(tmp32/256);
      index = index > -MAX_INDEX ? index : -MAX_INDEX;
      index = index < MAX_INDEX ? index :MAX_INDEX;
      index += MAX_INDEX;
      hist[index]++;
    }
  }

  /* Compute the probability of the 3 center bins */ 
  *midRange = hist[MAX_INDEX-1] + hist[MAX_INDEX] + hist[MAX_INDEX+1];

  /* Compute entropy */
  for (i=0;i<2*MAX_INDEX;i++)
  {
    if (hist[i]>0)
    {
      entropy += hist[i]*jpege_log2Q8(hist[i]);
    }
  }

  /*lint -save -e795 -e734 posible division by 0 - height and width >0 */
  entropy=(int32)(entropy/(height*width) - jpege_log2Q8(height*width));
  /*lint -restore */

  //JPEG_FREE(hist);
  return(int16)entropy;
}

/*===========================================================================

Function: JPEGE_SUM_8X8_LUMA

Description:  This function computes 8x8 sum for luma
              
Return Value:
      The zero mean sum of 8x8 block 
   
Comments:  
Side Effects:

============================================================================*/
static int16 jpege_sum_8x8_luma(uint8 * image, uint16 width)
{
  uint16 i,j;
  int16 sum=0;

  for (i=0; i<8;i++)
  {
    for (j=0;j<8;j++)
    {
      sum = sum + (int16)image[INDEX(i,j,width)]-128;
    }
  }
  return(sum);
}

/*===========================================================================

Function: JPEGE_SUM_8X8_CHROMA

Description:  This function computes 8x8 sum for chroma
              
Return Value:
   The zero mean sum of 8x8 block 
   
Comments:  
Side Effects:

============================================================================*/
static int16 jpege_sum_8x8_chroma(uint8 * image, uint16 width)
{
  uint16 i,j;
  int16 sum=0;

  for (i=0; i<8;i++)
  {
    for (j=0;j<8;j++)
    {
      sum = sum + (int16)image[INDEX(i,2*j,2*width)]-128;
    }
  }
  return(sum);
}

/*===========================================================================

Function: JPEGE_DC_HFILTER_LUMA

Description:  This function performs horizontal DC filter as
              y(n,m) = x(n,m)- x(n-1,m)  
              
Return Value:
   None
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_dc_Hfilter_luma(int16 *in, int16 *out, uint16 width, uint16 height)
{
  uint16 i,j;

  for (i=0;i<height;i++)
  {
    *out++ = *in++ ; /* Copy first element */
    for (j=0;j<width-1;j++)
    {
      *out++ = in[1]-in[0];
      in++;
    }
  }
}

/*===========================================================================

Function: JPEGE_DC_VFILTER_LUMA

Description:  This function performs vertical DC filter as
              y(n,m) = x(n,m)- x(n,m-1)  
              
Return Value:
   NOne
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_dc_Vfilter_luma(int16 *in, int16 *out, uint16 width, uint16 height)
{
  uint16 i,j;
  int16 *inTmp;
  int16 *outTmp;

  for (i=0;i<width;i++)
  {
    inTmp = in + i;
    outTmp = out + i;
    *outTmp = *inTmp ; /* Copy first element */
    outTmp += width;
    inTmp += width;

    for (j=0;j<height-1;j++)
    {
      *outTmp = inTmp[width]-inTmp[0];
      outTmp += width;
      inTmp += width;
    }
  }
}

/*===========================================================================

Function: JPEGE_AC_HFILTER_LUMA

Description:  This function performs horizontal 4 tap filter
              with taps  -1 2 -2 1  
              
Return Value:
   NOne
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_ac_Hfilter_luma(int16 *in, int16 *out, uint16 width, uint16 height)
{
  uint16 i,j;

  for (i=0;i<height;i++)
  {
    /*lint -save -e734 save since in is positive and correlated */
    *out++ = in[0]/4; /* Copy first element */
    *out++ = (-2*(int32)in[0]+(int32)in[1])/4; 
    *out++ = (2*(int32)in[0]-2*(int32)in[1]+(int32)in[2])/4; 
    in +=3;
    for (j=0;j<width-6;j++)
    {
      *out++ = (-(int32)in[0]+2*(int32)in[1]-2*(int32)in[2]+(int32)in[3])/4; 
      in++;
    }
    *out++ = (-(int32)in[0]+2*(int32)in[1]-2*(int32)in[2])/4; 
    *out++ = (-(int32)in[1]+2*(int32)in[2])/4; 
    *out++ = -in[3]/4; /* Copy first element */
    /*lint -restore */
  }
}

/*===========================================================================

Function: JPEGE_AC_VFILTER_LUMA

Description:  This function performs vertical 4 tap filter
              with taps  -1 2 -2 1  
              
Return Value:
   NOne
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_ac_Vfilter_luma(int16 *in, int16 *out, uint16 width, uint16 height)
{
  uint16 i,j;
  int16 *inTmp;
  int16 *outTmp;   

  for (i=0;i<width;i++)
  {
    inTmp = in + i;
    outTmp = out + i;
    /*lint -save -e734 save since in is positive and correlated */
    outTmp[0] = inTmp[0]/4; /* Copy first element */
    outTmp[width] = (-2*(int32)inTmp[0]+inTmp[width])/4; 
    outTmp[2*width] = (2*(int32)inTmp[0]-2*(int32)inTmp[width]+(int32)inTmp[2*width])/4; 
    inTmp += (3*width);
    outTmp += (3*width);
    for (j=0;j<height-6;j++)
    {
      outTmp[0] = 
      (-(int32)inTmp[0]+2*(int32)inTmp[width]-2*(int32)inTmp[2*width]+(int32)inTmp[3*width])/4; 
      inTmp += width;
      outTmp += width;
    }
    outTmp[2*width] = (-(int32)inTmp[0]+2*(int32)inTmp[width]-2*(int32)inTmp[2*width])/4; 
    outTmp[width] = (-(int32)inTmp[width]+2*(int32)inTmp[2*width])/4; 
    outTmp[0] = inTmp[2*width]/4;
    /*lint -restore */
  }
}

/*===========================================================================

Function: JPEGE_GETDC_FROM_ENTROPY

Description:  This function computes DC size from entropy using 2nd order 
              polynomial. The coefficients are in Q10 
              The DC size is returned in Q8
              
Return Value:
   DC size in Q8
   
Comments:  
Side Effects:

============================================================================*/
static uint16 jpege_getDC_from_entropy (int16 entropyQ8)
{
  int32 first,second,third;
  int32 dc;

  first = (int32)JPEGE_FILE_SIZE_DC_1ST; /* In Q10 */
  second = (int32)entropyQ8 * (int32)JPEGE_FILE_SIZE_DC_2ND; /* In Q18 */ 
  third = 
  (((int32)entropyQ8 *(int32)entropyQ8+(1<<5))/64) * (int32)JPEGE_FILE_SIZE_DC_3RD; /* In Q20 */
  dc = first*1024 + second*4 + third; /* In Q20 */ 
  return(uint16)((dc+2048)/4096); 
}

/*===========================================================================

Function: JPEGE_GETAC_FROM_ENTROPY

Description:  This function computes AC size from entropy using 2nd order 
              polynom. The coefficients are in Q10 
              The AC size is returned in Q8
              
Return Value:
   AC size in Q8
   
Comments:  
Side Effects:

============================================================================*/
static uint16 jpege_getAC_from_entropy (int16 entropyQ8)
{
  int32 first,second,third;
  int32 dc;

  /* Limit down the result */
  if (entropyQ8 < JPEGE_FILE_SIZE_MIN_AC_ENTROPY)
  {
    return(JPEGE_FILE_SIZE_MIN_AC_SIZE);
  }
  first = (int32)JPEGE_FILE_SIZE_AC_1ST; /* In Q10 */
  second = (int32)entropyQ8 * (int32)JPEGE_FILE_SIZE_AC_2ND; /* In Q18 */ 
  third = 
  (((int32)entropyQ8 *(int32)entropyQ8+(1<<5))/64) * (int32)JPEGE_FILE_SIZE_AC_3RD; /* In Q20 */
  dc = first*1024 + second*4 + third; /* In Q20 */ 
  return(uint16)((dc+2048)/4096); 
}

/*===========================================================================

Function: JPEGE_INIT_FILE_SIZE_ENH

Description:  This function reserves the memory needed for file size
              enhancement and subsamples luma and chroma  
              
Return Value:
   TRUE if malloc succeeded
   FALSE if malloc fail
   
Comments:  
Side Effects:

============================================================================*/
static boolean jpege_init_file_size_enh(JPEGENC_encodeSpecType spec)
{

  if (spec.Main.Subsample == JPEGENC_H2V2)
  {
    chromaWidth = spec.Main.Width/2;
    chromaHeight = spec.Main.Height/2;
    subsample = 4;
  }
  else
  {
    if ((spec.Rotation == JPEGENC_CCLK90) || (spec.Rotation == JPEGENC_CCLK270))
    {
      chromaWidth = spec.Main.Width;
      chromaHeight = spec.Main.Height/2;
    }
    else
    {
      chromaWidth = spec.Main.Width/2;
      chromaHeight = spec.Main.Height;
    }
    subsample = 2;
  }

  /* Reserve needed memory */
  dcPtrLuma = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtrLuma)
  {
    return(FALSE);
  }

  dcPtr1Luma = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr1Luma)
  {
    return(FALSE);
  }
  dcPtr2Luma = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr2Luma)
  {
    return(FALSE);
  }

  dcPtrCb = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtrCb)
  {
    return(FALSE);
  }
  dcPtr1Cb = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr1Cb)
  {
    return(FALSE);
  }
  dcPtr2Cb = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr2Cb)
  {
    return(FALSE);
  }

  dcPtrCr = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtrCr)
  {
    return(FALSE);
  }
  dcPtr1Cr = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr1Cr)
  {
    return(FALSE);
  }
  dcPtr2Cr = JPEG_MALLOC(spec.Main.Height*spec.Main.Width/64*2);
  if (!dcPtr2Cr)
  {
    return(FALSE);
  }

  jpegeFileSizeEstiBufPtr = (uint16 *)JPEG_MALLOC(2*(MAX_INDEX+1)*sizeof(uint16));
  if (!jpegeFileSizeEstiBufPtr)
  {
    return(FALSE);
  }

  /* Subsample luma and chroma */
  jpege_get_dc_luma(spec.Main.Fragmnt[0].Luma_ptr, dcPtrLuma, spec.Main.Height, spec.Main.Width);
  jpege_get_dc_chroma(spec.Main.Fragmnt[0].Chroma_ptr, dcPtrCr, chromaHeight, chromaWidth);
  jpege_get_dc_chroma(spec.Main.Fragmnt[0].Chroma_ptr+1, dcPtrCb, chromaHeight, chromaWidth);
  return(TRUE);
}


/*===========================================================================

Function: JPEGE_EXIT_FILE_SIZE_ENH

Description:  This function frees the memory used by file size
              enhancement  
              
Return Value:
   NOne
   
Comments:  
Side Effects:

============================================================================*/
static void jpege_exit_file_size_enh(void)
{
  if (dcPtrLuma)
  {
    JPEG_FREE(dcPtrLuma);
    dcPtrLuma = NULL;
  }

  if (dcPtr1Luma)
  {
    JPEG_FREE(dcPtr1Luma);
    dcPtr1Luma = NULL;
  }

  if (dcPtr2Luma)
  {
    JPEG_FREE(dcPtr2Luma);
    dcPtr2Luma = NULL;
  }

  if (dcPtrCr)
  {
    JPEG_FREE(dcPtrCr);
    dcPtrCr = NULL;
  }

  if (dcPtr1Cr)
  {
    JPEG_FREE(dcPtr1Cr);
    dcPtr1Cr = NULL;
  }

  if (dcPtr2Cr)
  {
    JPEG_FREE(dcPtr2Cr);
    dcPtr2Cr = NULL;
  }

  if (dcPtrCb)
  {
    JPEG_FREE(dcPtrCb);
    dcPtrCb = NULL;
  }

  if (dcPtr1Cb)
  {
    JPEG_FREE(dcPtr1Cb);
    dcPtr1Cb = NULL;
  }

  if (dcPtr2Cb)
  {
    JPEG_FREE(dcPtr2Cb);
    dcPtr2Cb = NULL;
  }

  if (jpegeFileSizeEstiBufPtr)
  {
    JPEG_FREE(jpegeFileSizeEstiBufPtr);
    jpegeFileSizeEstiBufPtr = NULL;
  }


}

/*===========================================================================

Function: JPEGE_ESTIMATE_AC_SIZE_ENH

Description:  This function estimates the AC complexity by performing
              4x4 high pass filering, histogram computaion, entropy 
              compuattion , and AC complexity mapping
              
Return Value:
   TRUE if complexity estimator is usable
   FALSE if complexity estimator is not usable
   
Comments:  
Side Effects:

============================================================================*/
static boolean jpege_estimate_ac_size_enh(JPEGENC_encodeSpecType spec, uint16 *ac_size)
{
  int16  acEntLuma, acEntCr, acEntCb;
  uint16 acSizeLuma, acSizeCr, acSizeCb;
  uint16 tmp, midRange;

  /* 2D high pass filetr for luma and chroma */
  jpege_ac_Vfilter_luma(dcPtrLuma, dcPtr1Luma, spec.Main.Width/8, spec.Main.Height/8);
  jpege_ac_Hfilter_luma(dcPtr1Luma, dcPtr2Luma, spec.Main.Width/8, spec.Main.Height/8);
  jpege_ac_Vfilter_luma(dcPtrCr, dcPtr1Cr,  chromaWidth/8, chromaHeight/8);
  jpege_ac_Hfilter_luma(dcPtr1Cr, dcPtr2Cr,  chromaWidth/8, chromaHeight/8);
  jpege_ac_Vfilter_luma(dcPtrCb, dcPtr1Cb,  chromaWidth/8, chromaHeight/8);
  jpege_ac_Hfilter_luma(dcPtr1Cb, dcPtr2Cb,  chromaWidth/8, chromaHeight/8);

  /* Get AC per pixel and middle 3 bins sum */
  acEntLuma = jpege_dc_entropy(dcPtr2Luma, spec.Main.Height/8, spec.Main.Width/8, 
                               JPEGE_AC_SCALE, &tmp);
  midRange = tmp;
  acSizeLuma = jpege_getAC_from_entropy(-acEntLuma);
  acEntCr = jpege_dc_entropy(dcPtr2Cr, chromaHeight/8, chromaWidth/8,JPEGE_AC_SCALE, &tmp);

  /*lint -save -e734 subsample and tmp very small */ 
  midRange += (int32)(tmp*subsample);
  acSizeCr = jpege_getAC_from_entropy(-acEntCr);
  acEntCb = jpege_dc_entropy(dcPtr2Cb, chromaHeight/8, chromaWidth/8,JPEGE_AC_SCALE, &tmp);
  midRange += (int32)(tmp*subsample);
  /*lint -restore*/

  acSizeCb = jpege_getAC_from_entropy(-acEntCb);

  /* Special case which hapen when picture is taken from very close distance 
     Adjust AC complexity                                                   */
  if ((2*acSizeLuma) < (acSizeCr + acSizeCb))
  {
    tmp = (uint16) (((acSizeCr+acSizeCb-2*acSizeLuma)*JPEGE_FILE_SIZE_SC_LUMACHROMA)/acSizeLuma);
    if (tmp >JPEGE_FILE_SIZE_MAX_SC_LUMACHROMA)
      tmp = JPEGE_FILE_SIZE_MAX_SC_LUMACHROMA;
    acSizeLuma = (uint16)(acSizeLuma * ((1 << 9) + tmp));
    acSizeLuma = (acSizeLuma + (1 <<8 )) / 512;
  }
  *ac_size = acSizeLuma;

  /* If middle 3 bins sum is bellow limit do not use enhancement */
  return(midRange > JPEGE_MID_PROB*spec.Main.Height*spec.Main.Width/64);
}


/*===========================================================================

Function: JPEGE_ESTIMATE_DC_SIZE_ENH

Description:  This function filters DC and estimate size  
              
Return Value:
   DC size in Q8
   
Comments:  
Side Effects:

============================================================================*/
static uint32 jpege_estimate_dc_size_enh(JPEGENC_encodeSpecType spec, uint16 estimateScaleFactorQ8)
{
  if ((spec.Rotation == JPEGENC_CCLK90) || (spec.Rotation == JPEGENC_CCLK270))
  {
    jpege_dc_Vfilter_luma(dcPtrLuma, dcPtr1Luma, spec.Main.Width/8, spec.Main.Height/8);
    jpege_dc_Vfilter_luma(dcPtrCr, dcPtr1Cr, chromaWidth/8, chromaHeight/8);
    jpege_dc_Vfilter_luma(dcPtrCb, dcPtr1Cb, chromaWidth/8, chromaHeight/8);
  }
  else
  {
    jpege_dc_Hfilter_luma(dcPtrLuma, dcPtr1Luma, spec.Main.Width/8, spec.Main.Height/8);
    jpege_dc_Hfilter_luma(dcPtrCr, dcPtr1Cr, chromaWidth/8, chromaHeight/8);
    jpege_dc_Hfilter_luma(dcPtrCb, dcPtr1Cb, chromaWidth/8, chromaHeight/8);
  }
  return(jpege_update_dc_size_enh(spec, estimateScaleFactorQ8)); 
}

/*===========================================================================

Function: JPEGE_UPDATE_DC_SIZE_ENH

Description:  This function computes DC entropy and estimates DC size 
              
Return Value:
   DC size in Q8
   
Comments:  
Side Effects:

============================================================================*/
static uint32 jpege_update_dc_size_enh(JPEGENC_encodeSpecType spec, uint16 estimateScaleFactorQ8)
{
  int16 dcEntLuma, dcEntCr, dcEntCb;
  uint16 dcSize1Luma, dcSize1Cr, dcSize1Cb,tmp;
  uint32 dcSizeNLuma, dcSizeNCr, dcSizeNCb;


  dcEntLuma = jpege_dc_entropy(dcPtr1Luma, spec.Main.Height/8, spec.Main.Width/8,
                               (uint16)(estimateScaleFactorQ8/exif_luma_dqt_low[0]), &tmp);
  dcSize1Luma = jpege_getDC_from_entropy(-dcEntLuma);

  dcEntCr = jpege_dc_entropy(dcPtr1Cr, chromaHeight/8, chromaWidth/8,
                             (uint16)(estimateScaleFactorQ8/exif_chroma_dqt_low[0]), &tmp);
  dcSize1Cr = jpege_getDC_from_entropy(-dcEntCr);

  dcEntCb = jpege_dc_entropy(dcPtr1Cb, chromaHeight/8, chromaWidth/8,
                             (uint16)(estimateScaleFactorQ8/exif_chroma_dqt_low[0]), &tmp);
  dcSize1Cb = jpege_getDC_from_entropy(-dcEntCb);
  dcSizeNLuma = dcSize1Luma * (spec.Main.Height/8)*(spec.Main.Width/8);
  dcSizeNCr = dcSize1Cr * (chromaHeight/8)*(chromaWidth/8);
  dcSizeNCb = dcSize1Cb * (chromaHeight/8)*(chromaWidth/8);
  return((dcSizeNLuma>>8) + (dcSizeNCr>>8) + (dcSizeNCb>>8)); 
}

/*=========================================================================== 

Function: JPEGE_GET_DIMENSIONS

Description:  This function computes JPEG dimensions (for MCU padding) 
              subject to max limits              
Return Value:
              None  
Comments:     Round up or down till x8(16) based on image color subsampling
              and sensor dimensions 
Side Effects:

============================================================================*/
void jpege_get_dimensions(uint16 *widthOut, uint16 *heightOut, uint16 origWidth, uint16 origHeight,
                          boolean isH2V2, JPEGENC_OrientType rotation, 
                          uint16 maxWidth, uint16 maxHeight)
{
  uint16 widthUp,heightUp, widthDown, heightDown;

  if (isH2V2)
  {
    widthUp = (origWidth+0xF)&0xFFF0;
    heightUp = (origHeight+0xF)&0xFFF0;
    widthDown = maxWidth&0xFFF0;
    heightDown = maxHeight& 0xFFF0;
  }
  else
  {
    if ((rotation == JPEGENC_CCLK90) || (rotation == JPEGENC_CCLK270))
    {
      widthUp = (origWidth+0x7)&0xFFF8;
      heightUp = (origHeight+0xF)&0xFFF0;
      widthDown = maxWidth&0xFFF8;
      heightDown = maxHeight&0xFFF0;
    }
    else
    {
      widthUp = (origWidth+0xF)&0xFFF0;
      heightUp = (origHeight+0x7)&0xFFF8;
      widthDown = maxWidth&0xFFF0;
      heightDown = maxHeight&0xFFF8;
    }
  }

  *heightOut = heightUp;
  *widthOut = widthUp;

  /* If sensor can not provide those dimensions round down */
  if (heightUp > maxHeight)
  {
    *heightOut = heightDown;
  }
  if (widthUp > maxWidth)
  {
    *widthOut = widthDown;
  }

}

#ifdef FEATURE_CAMERA_MOBICAT
/*===========================================================================

Function: JPEGE_MOBICAT_INSERT_INFO

Description:  This function transforms mobiCat parameters structure to
              a bytes stream and mark it to get included in APP7 marker
              
Return Value:
              Size of mobicat stream.
   
Comments:  Data is writen in little endian format 
Side Effects:

============================================================================*/
uint32 jpege_mobicat_insert_info(JPEGENC_mobiCat_type * elemArray, uint16 len)
{
  uint16 i;
  uint32 size;
  byte  *tmpPtr;

  /* Compute total length for mobiCat stream */
  size = JPEGE_MOBICAT_STATIC_SIZE * len;
  for (i=0; i<len; i++)
  {
    size += strlen(elemArray[i].elemName);
    size += (mcSizes[elemArray[i].type] * elemArray[i].width * elemArray[i].height);        
  }

  mobicatStreamSize = size;
  mobicatStreamPtr = (byte*)JPEG_MALLOC(size);
  if (mobicatStreamPtr == NULL)
  {
    MSG_ERROR("JPEGENC: out of memory", 0, 0, 0);
    jpeg_encode_failed = TRUE;
    return(0);
  }
  tmpPtr = mobicatStreamPtr;

  /* Fill byte stream with sensor attribute data */
  for (i=0; i<len; i++)
  {
    /* Write element length */
    size = mcSizes[elemArray[i].type];
    memcpy(tmpPtr, &size, sizeof(uint16));
    tmpPtr += sizeof(uint16);

    /* Write name length */
    *tmpPtr++ = (uint8) strlen(elemArray[i].elemName);

    /* Write name */
    memcpy(tmpPtr, elemArray[i].elemName,  (uint8) strlen(elemArray[i].elemName));
    tmpPtr += strlen(elemArray[i].elemName);

    /* Write type */
    memcpy(tmpPtr, &elemArray[i].type, sizeof(uint8));
    tmpPtr += sizeof(uint8);

    /* Write array width */
    memcpy(tmpPtr, &elemArray[i].width, sizeof(uint16));
    tmpPtr += sizeof(uint16);

    /* Write array height */
    memcpy(tmpPtr, &elemArray[i].height, sizeof(uint16));
    tmpPtr += sizeof(uint16);       

    /* Write data in little endian format */
    size = elemArray[i].height*elemArray[i].width*mcSizes[elemArray[i].type];
    memcpy(tmpPtr, elemArray[i].data, size);
    tmpPtr += size;
  }
  return(mobicatStreamSize);
}

/*===========================================================================

Function: JPEGE_EMIT_APP7

Description:  This function inserts sensor parameters byte stream in JPEG header
Return Value:
              TRUE (succes) , FALSE(failure)
   
Comments:  Global stream pointer and length are cleared after writing 
Side Effects:

============================================================================*/
boolean jpege_emit_app7(exif_info_type *info)
{
  byte app7_header[256];
  uint32 size;
  const char * header_string = "Qualcomm Camera Attributes";
  uint8 header_length = (uint8) strlen(header_string);

  if (!mobicatStreamPtr) return(FALSE);

  /* Compute marker size, 2 byte for size, 1 byte for strlen */
  size = 2 + 1 + header_length + mobicatStreamSize;

  /* Truncate overflow data */
  if (size > EXIF_MAX_APP7_LENGTH)
  {
    size = EXIF_MAX_APP7_LENGTH;
    mobicatStreamSize = size - (2 + 1 + header_length);
  }

  /* 2 byte for maker */
  if (info->buf_index + size + 2 >= info->buf_len) return(FALSE);

  /* Emit marker signature */
  app7_header[0] = 0xFF;
  app7_header[1] = 0xE7;

  /* Emit marker size in big endian */
  app7_header[2] = (byte)(size >>8);
  app7_header[3] = (byte)(size & 0xFF);

  app7_header[4]= (uint8) strlen(header_string);

  /* Emit magic words */
   memcpy( (char*)(app7_header+5) , header_string, header_length); 

  /* Copy to JPEG AP7 header */
  size = strlen( header_string ) + 5;

  memcpy(info->buf_ptr+info->buf_index, app7_header, size);
  info->buf_index += size;

  /* Copy to JPEG parameter data */
  memcpy(info->buf_ptr+info->buf_index, mobicatStreamPtr, mobicatStreamSize);
  info->buf_index += mobicatStreamSize;
  return(TRUE);
}

#endif /* FEATURE_CAMERA_MOBICAT */

/*lint -restore */

#endif /* FEATURE_JPEG_ENCODER_REV2 */

