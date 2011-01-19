
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                      E N C O D E R  AND  D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to encode and decode Joint Photographers  
  Expert Group (JPEG) image files.

EXTERNALIZED FUNCTIONS
  jpeg_decode
    This function is called to start the JPEG decoder
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

Copyright(c) 2002-2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/src/jpeg.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/09   RK      Fixed issue in the main image dimension parsing
02/28/09   kdiv    Vote for sleep assert and negate; to avoid TCXO shutdown during jpegd.
11/05/08   kdiv    Extract main image width and height in header parsing
09/09/08   kdiv    Extract thumbnail width and height in Exif header parsing
09/02/08   kdiv    ULC Write back cache changes
07/19/08   sigu    Removed the depreciated function sprintf with std_strlprintf
02/19/08   kdiv    Fixed issues with YCbCr Jpeg decoder o/p with odd dimension images
12/18/07   kdiv    Added support for YCbCr Jpegd DSP output and YCrCb final output
12/04/07   kdiv    Fixed issue in scalable JPEG decoder.
11/23/07   saur    Fixed critical lint warning
11/08/07   hap     Fixing RVCT2.2 compiler warnings
08/13/06   arv     Added support for non x16 support for JPEGD_RESIZE with arm decoding
06/25/06   arv     Added support for MediaFlo service icons
06/04/07   arv     Fixed an issue related to resizing of images using ARM based decoding
04/23/07   arv     Added support for JPEG sequential images with two SOS - 
                   one having Y components and another scan having interleaved CbCr 
                   components
                   Limited to decoding only Y component in sequential image
02/27/07   arv     Fixed the issue related to invalidation of data cache for the 
                   Input stream buffer to DSP when the NULL pointer/zero bytes 
                   are passed
02/14/07   arv     Modified string libraries in accordance with security 
                   standards
01/31/06   vma     Resolve lint issues (take out most suppressions in idct routine)
12/14/06   vma     Abort decoding if jpeg_init() fails to allocate buffer
11/28/06   vma     Fixed compilation error without RESIZE feature defined
11/12/06   arv     Added resize feature for JPEG Decoder
07/11/06   arv     Added jpeg_cancel_decode function to make sure that decoder
                   goes to IDLE state when the app cancels the decoding session
                   Fixes the graph overflow buffer issue
06/16/06   arv     Fixed the issue related to image having no quantization tables
                   Fixed the issue related to incorrect passing of quantization 
                   table to pre-6550 firmware
                   Fixed the issue with color conversion for images having height 
                   less than 16
06/07/06   dl      Checked for NULL pointer before sending callback.
06/06/06   vma     Made decode output format programmable in jpeg_decode_thin_layer
05/24/06   arv     Added a new FEATURE_JPEG_ARM_STANDALONE_DECODER to enable
                   standalone ARM based decoding
03/23/06   vma     Replaced all mallocs and frees to JPEG_MALLOC's and JPEG_FREE's
03/20/06   arv     Featurized camera and included ipl.h
03/17/06   dl      Use cache or cache lines based on supported cache.
03/15/06   vma     Added #ifndef FEATURE_JPEG_DECODER_REV2 around declaration of
                   DSP isr and callback so it would compile for pre-6550 targets
03/13/06   vma     Fixed all Lint errors
                   Check for grayscale because DSP does not support
12/27/05   an      Fixed the issue related to RGB565 color conversion for progressive
                   JPEG images
12/22/05   arv     Fixed the issue where the input bistream ptr is incorrectly pointing 
                   to the second byte after end of SOS Header which results in passing 
                   incorrect input bitstream to DSP decoder.Aligned the bitstream pointer 
                   based on the bitsRemaining parameter.
11/28/05   arv     Fixed Lint errors - Phase 1
11/07/05   arv     Fixed the compilation issue related to ThinUI builds by inserting 
                   FEATURE_CAMERA 
11/03/05   arv     Made changes to jpeg_decode_in _progress flag inorder to accomodate
                   the changes in OEMJPEG layer
10/12/05   arv     Changed the JPEG DSP decoder function name to represent the REV2 platform
09/30/05   arv     Made changes to jpeg_decode to avoid memory leak during jpeg_decode_abort and 
                   at the same time starting another decoding session
08/24/05   dl      Implement error recovery using restart marker.
08/24/05   arv     Restructured the ARM based JPEG Decoder
08/05/05   arv     Fixed the issue of not decoding pictures having TEM marker.
                   Fixed the issue with decoder being reset in case of
                   simultaneous decoding sessions using ARM and DSP
                   Reverted back the mmu_invalidate_data_cache changes for 
                   pre-6550.
                   Fixed the issue of memory leaking when very high resolution
                   pics don't get decoded
08/04/05   pdo     Removed memory allocation within extract thumbnail to support
                   L4 vitual memory scheme. Extract thumbnail interface has changed.
06/30/05   dl      Used ping pong buffer to hold DCT coefficients and
                   decode one row at a time for efficient memory consumption.
                   Also improve latency by using parallel processing between
                   arm and dsp.
06/08/05   pdo     Removed compiling warnings about stand alone encoding
05/18/05   arv     Fixed the issue where the JPEG images are distorted in CMX
                   using RGB888 format
05/05/05   arv     Made changes to mmu_invalidate_data_cache in such a way
                   that it doesn't invalidate cache globally
04/27/05   arv     Fixed the issue where jpeg decoder doesn't compile without 
                   pre-6550 DSP image 
04/08/05   pdo     Fixed RGB888 output format using in CMX
04/05/05   arv     Fixed the issue with the YCbCr output decoder for DSP
03/22/05   arv     Enabled YCbCr 422 and 420 LinePacked Output for JPEG ARM Decoder
03/22/05   arv     Featurized the JPEG Decoder - FEATURE_JPEG_DECODER_REV2
02/04/05   arv     Modified the decoder so as to identify the huffman and 
                   quantization table numbers from SOS and SOF respectively.
                   Set the ARM_Based flag accordingly for H1V1 and H1V2 support.
                   Fix for single pixel images.
01/13/05   pdo     Ensured proper decoder is used based on request and
                     the encoded subsample; Use JPEG_DECODER_REV2 label
01/11/05   arv     Changed the JPEG_DECODE_THIN_LAYER and JPEG_THIN_INTERFACE
                   to make it completely functional
12/08/04   arv     Added the FEATURE_JPEG_DECODER, FEATURE_JPEG_ENCODER and T_MSM6550 
                   at the appropriate places. 
12/02/04   pdo     Added support to DSP based 6550 JPEG decoder
                   DSP based 6550 only support H2V1 and H2V2 sequential.
12/01/04   dle     Added support for FEATURE_CAMERA_ENCODE_PROPERTIES.
11/04/04   arv     Changed JPEG_ARM_BASED_DECODE and JPEG_DECODE API for
                   proper playback of PMD files.
10/27/04   arv     Fixed divide by zero error with YUV 411 images
09/29/04   arv     Changed CLKRGM_NON_CDMA_APPS to CLKRGM_JPEG_APPS
08/03/04   arv     Removed EOI checking for sequential decode.
07/09/04   mvd     For the JPEG stand alone encoder, fix a bug where the
                   camera init was called twice.  Also passing back the
                   parm4 in the callback function that represents the num
                   byte of output for MEM mode encoding.
06/18/04   pdo     Added checks to take care the jpeg decoder re-entry case
                   Abort on going seesion and start new JPEG decoding session
                   Updated code for RVCT compiler
05/17/04   pdo     Provided a thin layer to support QTV/JPEG decoding.
                   Added jpeg_extract_thumbnail from EXIF 
05/03/04   pdo     Added sync/async option for DSP version decoder
04/28/04   mvd     Added handling for new CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED
                   in camera func CAMERA_FUNC_ENCODE_PICTURE mode only.
                   This is returned from Camera layer when the encoding 
                   completed but output encoded image exceeds user requested 
                   file size
04/19/04   pdo     changed jpeg_decode() and jpeg_ARM_based_decode API with
                   new parameters. Input buffer is dynamically allocated. 
                   Provided needed interfaces to synchronize the async. 
                   data request. 
                   Added clk_unexpected_enable() to ensure ARM not sleep
                   when get interrupt from DSP.
04/15/04   mvd     Revised the stand-alone JPEG encoder to call into
                   camera services differently (i.e. not relying on the 
                   sensor).  Supports following input formats: YCbCr,
                   RGB888, and RGB444
03/24/04    st     Modified code to use client buffer, if available, to return
                   initial header to client for a decode operation; modified
                   code to verify state in QDSP callback function; modified
                   JPEG decode state machine to ensure initial data read
                   succeeds before enabling DSP; modified decode state machine
                   to have asynchronous get buffer command.
03/23/04   mvd     Revised stand-alone JPEG Encoder to call into camera 
                   services layer.  Provides encode and abort funtionality.
02/24/04   pdo     New capability: ARM based JPEG decoding, not using DSP.
02/18/04   pdo     Handled DSP illegal SLEEP cmd and terminated DSP before
                   sending ABORT/FAILURE callback to client.
                   Featurized the clk regime voting
02/12/04   pdo     Supported for FEATURE_QDSP_USES_MODULES
                   Added clk regime voting at start/end of decoding.
                   More adversarial cases detection
02/06/04   pdo     Added more adversarial cases detection and stopped
                   searching beyond the actual file size.
01/23/04   pdo     Handled redundant QDSP_EV_IMAGE event and checked for 
                   failure in memory allocation.
12/04/03   pdo     Added supports for RGB565 output from DSP.
                   and replaced OEM_Malloc and OEM_Free with malloc and free
09/30/03   pdo     Support image filesize up to 1280x1024, provided enough
                   heap memory is available. Removed qdsp_reset() to avoid 
                   un-necessary DSP image swap.
                   Needed new JPEG decoder DSP image.
09/18/03   pdo     Fixed the Restart marker detection in the sequential decoding
08/08/03   pdo     Detected adverserial conditions in JPEG file to meet OATViewer 
                   test for the BREW. This should gracefully exit the JPEG
                   decoder when encounter bad situations or conditions. 
07/17/03   pdo     Called qdsp_reset() to ensure DSP image got download again.
                   Added return code for SOI check for adversarial cases.
06/20/03   pdo     Used proper Enum in qdsp_allow_reset function.
06/01/03   pdo     Redesigned JPEG decoder to support Progressive mode
                   This new decoder requires Progressive DSP image.
04/10/03    sa     Moved qdsp_allow_reset() after sleep command in
                   jpeg_disable_qdsp().  Image specification callback
                   is issued before requesting output buffer from client
                   in decoder.
04/10/03    sa     Fixed problem with JPEG driver accessing DSP after JPEG 
                   image has been disabled.
11/29/02    sa     Changed decoder to have client provide image output buffer
                   via callback.
09/05/02    sa     Removed check of num_cols_dup in encoder function 
                   jpeg_enc_create_mcu() that would allow an invalid 
                   col_width_size value of 0 for certain size images. 
08/30/02    an     Fixed problem with previously queued commands being 
                   executed after encoder/decoder had been disabled.  
                   This also fixed a bug where DSP memory was being accessed 
                   after disabling the DSP. 
07/17/02    sa     Added support for 16-bit 5-6-5 output from decoder.
04/17/02    sa     Modified code so that the encoder requires no additional 
                   raw image data reads. Also changed check of col_width_size.
04/02/02    sa     Added header for grayscale output in decoder.  Modified 
                   mask for QDSP address to support all current address 
                   mappings.  Cleaned up and optimized encoder code.
03/24/02    sa     Added check in decoder to make sure number of words free
                   in input buffer does not exceed actual buffer length.
03/22/02    sa     Removed callback in encoder and decoder ISR.  Also put
                   in additional parameter checks and cleaned up code.
03/22/02    sa     Initial version

===========================================================================*/
//#define JPEG_TIMING_TEST    /* Enabled suring timing test */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <stdio.h>              /* Standard I/O routines                   */
#include <stdlib.h>
#include "jpeg.h"
#include "jpegi.h"
#include "jpeg_mrk.h"
#include "jpeg_huf.h"
#include "ipl.h"
#include "jpeg_malloc.h"
#define jpeg_file_id MALLOC_JPEG

#ifdef FEATURE_JPEG_DECODER_REV2
#include "jpegd.h"              /* JPEG decoder for 6550 HW accelerated    */
#endif

#include "qdsp.h"
#include "qdspext.h"            /* QDSP access function definitions        */
#include "graph.h"              /* Graphics task typedefs and prototype    */
#ifdef FEATURE_CAMERA
#include "camera.h"             /* Included for calls to camera layer      */
#endif
#include "clk.h"                
#include "exif.h"        
#ifdef JPEG_USES_LIBSTD
#include "AEEstd.h"
#endif /* JPEG_USES_LIBSTD */

#ifdef FEATURE_NEW_SLEEP_API
#include "sleep.h"
#endif

#ifdef JPEG_TIMING_TEST
#include "timetest.h"
LOCAL qword          start_time, end_time;
LOCAL char           *time_elapsed_ptr;
char *qmark_timing ( qword , qword );
#endif 

#ifdef FEATURE_QDSP_USES_MODULES
  qdsp_isr_func_type dsp_isr_table[QDSP_ISR_MAX] = {NULL, NULL};
#endif /* FEATURE_QDSP_USES_MODULES */

#ifdef FEATURE_NEW_SLEEP_API
sleep_okts_handle jpeg_sleep_okts_handle;
#endif /* FEATURE_NEW_SLEEP_API */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

//------------------------------------------------------------------------------


LOCAL int32 const ZAG[64] =
{
   0,  1,  8, 16,  9,  2,  3, 10,
  17, 24, 32, 25, 18, 11,  4,  5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13,  6,  7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63,
};


const int32 AAN_SCALE_BITS = 14;                       /* Variables used for IDCT Conversion */
const int32 IFAST_SCALE_BITS = 2; 


uint16   jpegd_imageWidth;
uint16   jpegd_imageHeight;



uint8    *in_dataPtr;
uint8    *in_data_buf;


uint32   bitPack;                                        /* Buffer to unpack the JPEG stream */
uint32   byteOffset;                                     
int32    bitsRemaining;

uint32    jpegd_fileSize;
uint32    total_byte_read;

uint32    half_buffer_select;
uint32    data_byte_remain;
                               
  
int16    jpegd_quant[JPGD_MAXQUANTTABLES][64];  /* Quantization Table with JPGD_MAXQUANTTABLES = 4 */

uint32   numberOfComponentsInFrame;
uint32   numberOfComponentsInScan;

int32    startOfSpectralSel; 
int32    endOfSpectralSel;
int32    succApproxBitHigh;
int32    succApproxBitLow;
int32    EOBrun;
uint32   last_dc_val[JPGD_MAXCOMPONENTS];

uint8    frameCompID[JPGD_MAXCOMPONENTS];     
uint32   frameVSamplingFactor[JPGD_MAXCOMPONENTS];   /* Component horizontal sampling factor */
uint32   frameHSamplingFactor[JPGD_MAXCOMPONENTS];     /* Component vertical sampling factor */

uint32   frameQuantTableSelector[JPGD_MAXCOMPONENTS];   /* Frame Quantization Table Selector */
uint32   scanCompSelector[JPGD_MAXCOMPSINSCAN];
uint32   scanDCTableSelector[JPGD_MAXCOMPONENTS];
uint32   scanACTableSelector[JPGD_MAXCOMPONENTS];

uint16   restart_interval;
uint16   restarts_left;
uint32   mcu_to_skip;
uint32   total_coeff_size = 0;

boolean  prog_flag;
boolean  isSyncCb;




LOCAL    uint8   *imageOut;
LOCAL    uint8   *rgbPtr;
LOCAL    uint8   *localRGBPtr;
LOCAL    uint8   *localrgbptr;
LOCAL    uint8   *localYCbCrPtr;
LOCAL    uint8   *jpegdBufPtr1;
LOCAL    uint8   *jpeg_output_buf_ptr = NULL;
LOCAL    uint8   *current_output_ptr;

LOCAL    uint8   *jpegdCurDestYCbCrPtr; 
LOCAL    uint8   *jpegdChromaCurDestYCbCrPtr;

LOCAL    uint8   next_restart_num;

LOCAL    uint32  *jpeg_id = 0;                                       /* JPEG session counter */
LOCAL    uint32  file_position;                        /* Current position within input file */

LOCAL    uint8   *zerothRowScan; 
LOCAL    uint8   *firstRowScan; 
LOCAL    uint8   *lastDestPtr;

#ifdef FEATURE_JPEGD_RESIZE
static  uint32   jpegd_resizeCol = 0;
static  uint32   jpegd_resizeColCum = 0;

static uint32  jpegd_localImageHeight;
static uint32  jpegd_pixRow;
static uint16   jpegd_imageWidthEven;
int32 colSizeToScale;
#endif

LOCAL    uint32  bytesPerPixel;                                             /* Bits per pixel */
LOCAL    uint32  bytes_per_line;
LOCAL    uint32  linesDecoded;                               /* Lines Decoded in the picture */
LOCAL    uint32  dummyBits;
LOCAL    uint32  hMCUBlock[JPGD_MAXCOMPONENTS];
LOCAL    uint32  vMCUBlock[JPGD_MAXCOMPONENTS];

LOCAL    uint32  maxMCUrowSize;                                            /* MCU's row size */
LOCAL    uint32  maxMCUcolSize;                                            /* MCU's col size */
LOCAL    uint32  dataUnitsperMCU;
LOCAL    uint32  maxDataUnitsperRow;
LOCAL    uint32  mcusperRow, mcusperCol;
LOCAL    uint32  maxMCUsperRow;
LOCAL    uint32  maxMCUsperCol;
LOCAL    int32   error_code;

LOCAL    uint32  mcu_rows_decoded;
LOCAL    uint32  total_rows_decoded;
LOCAL    uint32  mcu_rows_per_decode;
LOCAL    uint32  decode_y_size;
LOCAL    uint32  lines_to_decode;


LOCAL    uint32  maxDataUnitsperMCU;
LOCAL    uint32  mcuBlk[JPGD_MAXBLOCKSPERMCU];
LOCAL    uint32  totalLinesRemaining;                     /* Total number of lines remaining */
LOCAL    int32   mcuLinesRemaining;                   /* Total number lines remaining in MCU */
LOCAL    int32   dataUnit_max_zag_set[JPGD_MAXBLOCKSPERROW];
LOCAL    int32   scan_display;

LOCAL    int32   g_max_file_size;
LOCAL    void    *jpeg_enc_inst_handle_ptr;
LOCAL    uint8   *jpeg_mcu_ptr;                      /* Pointer to MCU input or output data   */
LOCAL    uint8   *jpeg_io_buf2_ptr;                 /* Pointer to one of the two I/O buffers */
LOCAL    uint8   *jpeg_io_buf1_ptr;                 /* Pointer to one of the two I/O buffers */

LOCAL    int32   jpege_mem_mode_output_size;                /* Num output bytes in mem mode  */
LOCAL    char    tstring[25];

LOCAL    boolean   jpeg_set_buffer_flag = FALSE;              /* JPEG internal buffers set   */
LOCAL    boolean   jpeg_encode_abort_flag;                 /* JPEG encode abort status flag  */
LOCAL    boolean   jpeg_encode_in_progress;                /* JPEG encode in progress flag   */
 
LOCAL    boolean   readyFlag;
LOCAL    boolean   ARM_based;


LOCAL    boolean   DSP_decode=FALSE;
LOCAL    boolean   jpeg_image_specs_flag = FALSE;         /* JPEG file specification flag    */
         boolean   jpeg_decode_in_progress = FALSE;               /* JPEG decode in progress flag    */

LOCAL    boolean   jpeg_decode_abort_flag = FALSE;
LOCAL    boolean   jpeg_cancel_decode_flag = FALSE;
LOCAL    boolean   mem_allocated = FALSE;
LOCAL    boolean   image_done = FALSE;

static  boolean                  jpegdResizeFlag = FALSE;

#ifdef FEATURE_JPEGD_RESIZE
/* Flag to check if JPEG Image can be resizable based on aspect ratio calculation */
static  boolean                  isResizable = FALSE;
static  boolean                  resizeNow = FALSE;
#endif

jpeg_cmx_spec_type    jpeg_spec;

LOCAL    jpeg_dsp_req_type       jpeg_dsp_request;
LOCAL    exif_info_type          JPEG_exif_info;

LOCAL    jpege_status_enum_type  jpeg_enc_status;
LOCAL    jpege_cb_func_type      jpeg_enc_cb;
LOCAL    jpeg_cmx_get_data_type  jpeg_get_data;                      /* Buffer data from CMX */
LOCAL    jpeg_cmx_data_type      cmx_data;                                  /* JPEG CMX Data */


LOCAL    DATAUNIT_DESC           tempDataUnit[64];
LOCAL    jpeg_state_type         jpeg_state;                   /* JPEG decoder state machine */


LOCAL    jpeg_decode_input_type     client_input_info;          /* JPEG Thin Interface Layer */
LOCAL    jpeg_decode_output_type  * output_info_ptr;
LOCAL    jpeg_cb_func_type          client_cb_func;

LOCAL    ipl_image_type    jpegdIPLIn;
LOCAL    ipl_image_type    jpegdIPLOut;



MCU_info_type          mcu_info;
JPEGD_STATUS_MSG       jpegd_statusMsg;
jpeg_decodeInfo_type      localjpegdInfo;
JPEG_subsample_type subsampletype, progSubSampleType;

void                   *jpeg_cmx_handle;                                 /* JPEG CMX handler */
jpeg_cmx_cb_func_type  jpeg_cmx_cb = NULL;                  /* Callback for JPEG CMX handler */

jpeg_huff_tbl          read_huff;
jpeg_derived_tbl       dc_derived_table[4];
jpeg_derived_tbl       ac_derived_table[4];


comp_data_type           *comp_data;
LOCAL    uint8           current_comp_index;
LOCAL    comp_data_type  comp_buffer_1[JPGD_MAXCOMPONENTS];
LOCAL    comp_data_type  comp_buffer_2[JPGD_MAXCOMPONENTS];

#ifdef FEATURE_JPEG_DECODER_REV2
int16                    quant_6550[2][64];
JPEGDEC_decodeSpecType   jpegd2_info;
JPEGDEC_bufferInfoType   jpegd2_buffer;
uint8*                   jpegd_dspBstrmPtr;
uint32                   jpegd_totalBytesRead;
extern  boolean          jpeg_decode_abort;
#endif

/* The following variables will be removed automatically when IPL color conversion is added */
LOCAL int32   crr[256];
LOCAL int32   cbb[256];
LOCAL int32   padd;
LOCAL int32   crg[256];
LOCAL int32   cbg[256];
boolean isDQTTablePresent;

/***********************************************************************/
/*                 JPEG Decoder Functions Prototype                    */
/***********************************************************************/
void jpeg_init(void);

void idct(DATAUNIT_DESC *data, 
          uint8 *Pdst_ptr);



void jpeg_decode_image(void *cb_data );


jpeg_return_type jpegd_colorConvert_RGB(void);
jpeg_return_type jpegd_colorConvert_YCbCr(void);
#ifdef FEATURE_JPEGD_RESIZE
jpeg_return_type jpegd_colorConvert_resize_common(void);
#endif

#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
LOCAL void jpegd_q2a_isr( void);
LOCAL void jpegd_q2a_callback(qdsp_event_data_type *);
#endif


#ifdef FEATURE_JPEG_DECODER

#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
LOCAL void jpegd_terminate( void);
#endif

#ifdef FEATURE_JPEG_ENCODER
LOCAL void jpeg_CameraLayerCB(camera_cb_type cb, const void *client_data, camera_func_type func, int32 parm4);
#endif

#ifdef FEATURE_CAMERA

#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
camera_ret_code_type camera_encode_picture_stand_alone(
               camera_frame_type *,                /* Uncompressed Input stats */
               camera_handle_type *,               /* Services requested       */
               camera_encode_properties_type *,    /* Services requested       */
               camera_cb_f_type,                   /* Local callback           */
               void * );                           /* client data here-to pass back via callback */
#else /* FEATURE_CAMERA_ENCODE_PROPERTIES */
camera_ret_code_type camera_encode_picture_stand_alone(
               int32,                              /* JPEG max file size       */
               camera_frame_type *,                /* Uncompressed Input stats */
               camera_handle_type *,               /* Services requested       */
               camera_cb_f_type,                   /* Local callback           */
               void * );                           /* client data here-to pass back via callback */
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

camera_ret_code_type camera_stop_stand_alone(
               camera_cb_f_type,                   /* no callback           */
               void * );                           /* client data here-to pass back via callback */

#endif /* FEATURE_CAMERA */




/***********************************************************************/
/*                    JPEG Decoder Operational States                  */ 
/***********************************************************************/
typedef enum
{
   UNKNOWN_STATE  = 0,          /* undefine state */
   RESET_STATE,                 /* image is being downloaded */
   INIT_STATE,                  /* download complete, ready for command. */
   CONFIG_STATE,
   DECODE_STATE
} JPGPD_stateEnumType;

JPGPD_stateEnumType JPEG_dsp_state = UNKNOWN_STATE;

#endif /* FEATURE_JPEG_DECODER */

#ifdef FEATURE_JPEG_ENCODER
/* Encoder Variables */

LOCAL camera_frame_type       global_camera_frame;
LOCAL camera_handle_type      global_camera_handle;

#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
LOCAL camera_encode_properties_type global_encode_properties;
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

#endif


#if defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)
/*===========================================================================

FUNCTION jpeg_register

DESCRIPTION
  This function initializes everything needed for JPEG at the time of bootup.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void jpeg_register(void)
{
#ifdef FEATURE_JPEG_DECODER
#ifdef FEATURE_NEW_SLEEP_API
  jpeg_sleep_okts_handle = sleep_register("JPEG", TRUE);
#endif /* FEATURE_NEW_SLEEP_API */
#endif /* FEATURE_JPEG_DECODER */

}
#endif /* FEATURE_JPEG_DECODER || FEATURE_JPEG_ENCODER */


#ifdef FEATURE_JPEG_DECODER
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER //arvind


/*===========================================================================

Function: JPEGD_INITIALIZE

Description: This function initializes the progressive JPEG decoder.
             This is an asynchronous function. Caller of this routine
              will receive a return status.
              Upon successful downloading of the QDSP code, a callback
              will be invoked to notify the caller. This may take few hundred
              miliseconds.

Return Value:
   JPGPD_SUCCESS              cmd sucessful
   JPGPD_INVALID_CB           init cmd contains a NULL cb pointer
   JPGPD_INVALID_STATE        init cmd issued during invalid state
   JPGPD_QDSP_DWNLD_FAIL      qdsp JPEG accelerator download failed

Comments: This function must be called prior to calling any other function
          or issuing any qdsp commands.

Side Effects: Resets the QDSP and loads the JPEG accelerator image.

==============================================================================*/
JPGPD_returnCodeType   jpegd_initialize ()
{

#ifdef FEATURE_QDSP_USES_MODULES
  dsp_isr_table[QDSP_ISR_INT0] = jpegd_q2a_isr;
#endif /* FEATURE_QDSP_USES_MODULES */

  /* QDSP image is being downloaded */
  JPEG_dsp_state = RESET_STATE;

  qdsp_disable( QDSP_APP_IGG );

  /*  register the camqdsp ISR and status callback functions           */
#ifdef FEATURE_QDSP_USES_MODULES
  qdsp_register_app( QDSP_APP_IGG,
                     dsp_isr_table,
                     jpegd_q2a_callback,
                     QDSP_EV_COMMAND | QDSP_EV_MODULE
                   );
  /* enable the JPEG accelerator application image                     */
  qdsp_reset_and_enable( QDSP_APP_IGG, QDSP_MODULE_JPEGD);
#else
  qdsp_register( QDSP_APP_IGG,
                 jpegd_q2a_isr,
                 QDSP_ISR_INT0,
                 jpegd_q2a_callback,
                 QDSP_EV_COMMAND | QDSP_EV_MODULE | QDSP_EV_IMAGE
               );
  /* enable the JPEG accelerator application image                                  */
  qdsp_enable( QDSP_APP_IGG, QDSP_IMAGE_JPEGD);
#endif /* FEATURE_QDSP_USES_MODULES */

  return JPGPD_SUCCESS;
} /* jpegd_initialize */



/*===========================================================================

Function: JPEGD_COMMAND

Description:  This function is used to command the QDSP firmware.
              It is only valid after the QDSP download is successful.

Return Value:
   JPG_PD_SUCCESS      - cmd sucessful
   JPG_PD_INVALID_CMD  - invalid QDSP command specified
   JPG_PD_INVALID_PARM - invalid QDSP command parameter

Comments: See JPGPDCommandType for command options

Side Effects:

============================================================================*/
JPGPD_returnCodeType   jpegd_command
(
 JPGPDCommandType        DSPCommandCode,/* type of command                */
 void                    *pCmd          /* pointer to DSP cmd buffer      */
 /* NULL if cmd = IDLE             */
 )
{
  uint16  TempReg;
  JPGPD_returnCodeType  ReturnStatus = JPGPD_SUCCESS;
  JPG_PD_config_struct  *pConfig_data;
  JPG_PD_command_struct *pDecode_data;
  
  switch (DSPCommandCode)
  {
    case JPGPD_CONFIG_CMD:
     if (JPEG_dsp_state != INIT_STATE)
     {
      return JPGPD_INVALID_STATE;
     }
    
     /* Set up necessary parameters to configure the decoder */
     TempReg = qdsp_read(QDSP_jpegDecCmdReg);
     
     if ( TempReg )
     {
      MSG_HIGH("JPGPD: Command register was not 0", 0, 0, 0);
     }
    
     if (pCmd)
     {
      pConfig_data = (JPG_PD_config_struct *)pCmd;
      if ((pConfig_data->RGBBuf1Ptr==NULL) ||(pConfig_data->RGBBuf2Ptr==NULL))
      {
        MSG_ERROR("Invalid address detected", 0 , 0, 0);
        return JPGPD_INVALID_PARM;
      }
      
      if ((pConfig_data->coefBuf1PtrSet[0] == NULL) ||
        (pConfig_data->coefBuf2PtrSet[0] == NULL) )
      {
        MSG_ERROR("Invalid address detected", 0 , 0, 0);
        return JPGPD_INVALID_PARM;
      }
      
      if ( (pConfig_data->lumaQuantTab == NULL) ||
        (pConfig_data->chromaQuantTab == NULL) )
      {
        MSG_ERROR("Invalid address detected", 0 , 0, 0);
        return JPGPD_INVALID_PARM;
      }
      else
      {
        /* Copy the quantization tables to the QDSP */
        qdsp_block_write(QDSP_jpegdLumaQuantTab, 0,
            pConfig_data->lumaQuantTab, 64, FALSE);
        
        qdsp_block_write(QDSP_jpegdChromaQuantTab, 0,
            pConfig_data->chromaQuantTab, 64, FALSE);
        
        qdsp_write(QDSP_jpegDecCmdReg, JPGPD_CONFIG_CMD);
        qdsp_write(QDSP_jpegDecCmdBuf, pConfig_data->outFormat);
        qdsp_write(QDSP_jpegDecCmdBuf+1, pConfig_data->samplingFactor);
        qdsp_write(QDSP_jpegDecCmdBuf+2, pConfig_data->imageWidth);
        qdsp_write(QDSP_jpegDecCmdBuf+3, pConfig_data->imageHeight);
        
        *QDSP_jpegdRGBBuf1Ptr = (uint32) pConfig_data->RGBBuf1Ptr;
        *QDSP_jpegdRGBBuf2Ptr = (uint32) pConfig_data->RGBBuf2Ptr;
        
        *QDSP_jpegdDctCoefBuf1PtrSet = (uint32) pConfig_data->coefBuf1PtrSet[0];
        *(QDSP_jpegdDctCoefBuf1PtrSet+1) = (uint32) pConfig_data->coefBuf1PtrSet[1];
        *(QDSP_jpegdDctCoefBuf1PtrSet+2) = (uint32) pConfig_data->coefBuf1PtrSet[2];
        
        *QDSP_jpegdDctCoefBuf2PtrSet = (uint32) pConfig_data->coefBuf2PtrSet[0];
        *(QDSP_jpegdDctCoefBuf2PtrSet+1) = (uint32) pConfig_data->coefBuf2PtrSet[1];
        *(QDSP_jpegdDctCoefBuf2PtrSet+2) = (uint32) pConfig_data->coefBuf2PtrSet[2];
        
        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
    }
    else
    {
      MSG_ERROR("Null pointer encouter", 0 , 0, 0);
      ReturnStatus = JPGPD_INVALID_PARM;
    }
    break;
    
    case JPGPD_DECODE_CMD:                 /* Start decode the image  */
      if (JPEG_dsp_state != CONFIG_STATE)
      {
        return JPGPD_INVALID_STATE;
      }
    
      TempReg = qdsp_read(QDSP_jpegDecCmdReg);
    
      if ( TempReg )
      {
        MSG_HIGH("JPGPD: Command register was not 0", 0, 0, 0);
        return  JPGPD_QDSP_BUSY;
      }
    
      if (pCmd)
      {
        pDecode_data = (JPG_PD_command_struct *) pCmd;
      
        /* set output pointer to the begining of RGB buffer */
        *QDSP_jpegdRGBBuf1Ptr = (uint32) pDecode_data->output1_ptr;
        *QDSP_jpegdRGBBuf2Ptr = (uint32) pDecode_data->output2_ptr;
      
        /* Set decode command ID */
        qdsp_write(QDSP_jpegDecCmdReg, JPGPD_DECODE_CMD);
        qdsp_write(QDSP_jpegDecCmdBuf, pDecode_data->inputBufIndex);
        qdsp_write(QDSP_jpegDecCmdBuf+1, pDecode_data->startMCU);
        qdsp_write(QDSP_jpegDecCmdBuf+2, pDecode_data->MCU_count);
        qdsp_write(QDSP_jpegDecCmdBuf+3, (pDecode_data->samplingFactor + 1));
      
        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
      else
      {
        MSG_ERROR("Null pointer encouter", 0 , 0, 0);
        ReturnStatus = JPGPD_INVALID_PARM;
      }
    
      break;
    
    case JPGPD_IDLE_CMD:                /* Stop the QDSP from decoding */
      /* Check register to make sure DSP is ready for new command */
      TempReg = qdsp_read(QDSP_jpegDecCmdReg);
      if ( TempReg )
      {
        MSG_HIGH("JPGPD: Command register was not 0", 0, 0, 0);
        return  JPGPD_QDSP_BUSY;
      }
      else
      {
        /* Set idle command ID */
        qdsp_write(QDSP_jpegDecCmdReg,JPGPD_IDLE_CMD);
        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
    
      break;
    
    case JPGPD_SLEEP_CMD:             /* Put the QDSP to SLEEP */
    
      /* Check register to make sure DSP is ready for new command */
      TempReg = qdsp_read(QDSP_jpegDecCmdReg);
      if ( TempReg )
      {
        MSG_HIGH("JPGPD: Command register was not 0", 0, 0, 0);
        return  JPGPD_QDSP_BUSY;
      }
      else
      {
        /* Set idle command ID */
        qdsp_write(QDSP_jpegDecCmdReg,JPGPD_SLEEP_CMD);
        /* Assert ARM to DSP interrupt */
        qdsp_up_to_dsp_irq();
      }
      break;
    
    default:
      ReturnStatus = JPGPD_INVALID_CMD;
      MSG_ERROR("Un-recognized command", 0, 0, 0);
  }
  return ReturnStatus;
  
} /* jpegd_command */

/*===========================================================================

FUNCTION JPEGD_Q2A_ISR

DESCRIPTION

  Depends on the current stage of operation, this function notifies ARM the
  state of QDSP, including decoder ready message, decoder config ready message
  and decoder done message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpegd_q2a_isr( void)
{
  int16            semaphore;
  jpeg_decode_status_type  jpeg_decode_status = 0;

  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */
#ifdef FEATURE_REX_PREEMPT_INTS
    clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  /* Check DSP decoder states */
  if ((semaphore = qdsp_read(QDSP_jpegdStateReg)) != 0)
  {
    switch (semaphore)
    {
       case -1:/* Decoder enters INIT state */
 
         /* JPEG DSP image is ready to accept command */
         /* No action is required here since download callback */
         /* already setup necessary condition and state */
         break;

       case 1 :/* Decoder enters CONFIG state */
         JPEG_dsp_state = CONFIG_STATE;
           //Set a local state or start decode command
         jpeg_dsp_request.function_ptr =
                         (jpeg_cmx_command_cb_func_type) jpeg_decode_image;
         jpeg_dsp_request.client_data = (void *) jpeg_id;

         if (jpeg_cmx_cb != NULL)
         {
             jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
         }
         jpeg_state = JPEG_DECODE_SCAN;
         break;

       case 2 :
         JPEG_dsp_state = DECODE_STATE;
         MSG_LOW("DSP is in decode state",0,0,0);

         break;

       default:
         MSG_ERROR ("DSP decoder is in unknown state",0,0,0);

    }
    qdsp_write(QDSP_jpegdStateReg, 0);

  }

  /* Check for image decoding complete
  ** Function to process request.
  */
  jpeg_decode_status = qdsp_read(QDSP_jpegDecDoneReg);

  if ((jpeg_decode_status == DECODE_ENTIRE_IMAGE_DONE ) || 
      (jpeg_decode_status == DECODE_PARTIAL_IMAGE_DONE))
  {
    JPEG_dsp_state = CONFIG_STATE;

    #ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
    /* transition DSP to config state */
    jpegd_command(JPGPD_IDLE_CMD, NULL);
    #endif

    /* Setup to write data to QDSP                                          */
    jpeg_dsp_request.function_ptr =
                         (jpeg_cmx_command_cb_func_type) jpeg_decode_image;
    jpeg_dsp_request.client_data = (void *) jpeg_id;

    if (jpeg_cmx_cb != NULL)
    {
      jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
    }

    /* for progressive, we decode until image_done flag is true */
    if (prog_flag == TRUE)
    {
       if (image_done == TRUE)
       {
         jpeg_state = JPEG_IMAGE_DECODE_DONE;
       }
       else
       {
         jpeg_state = JPEG_DECODE_SCAN;
         total_rows_decoded = 0;         
         mcu_rows_decoded = 0;
       }
    }
    /* for sequencial, we use msg from dsp to determine image done*/
    else if (jpeg_decode_status == DECODE_ENTIRE_IMAGE_DONE)
    {
      jpeg_state = JPEG_IMAGE_DECODE_DONE;
    }
    else
    {
      jpeg_state = JPEG_DECODE_SCAN;
    }

    qdsp_write(QDSP_jpegDecDoneReg, 0);
  }

} /* jpegd_q2a_isr */
/*===========================================================================

FUNCTION JPEGD_Q2A_CALLBACK

DESCRIPTION
  This function will be called after the qdsp_enable function has
  completed.  This function is registered as a call back using the
  qdsp_register function.

DEPENDENCIES
  Must be registered with qdsp_register

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void jpegd_q2a_callback
(
 qdsp_event_data_type *event_ptr
)
{
  switch (event_ptr->event)
  {
    case QDSP_EV_COMMAND:
      MSG_MED("Received QDSP command event (jpegd)",0,0,0);
      break;

#ifdef FEATURE_QDSP_USES_MODULES
    case QDSP_EV_MODULE:
      MSG_MED("Received QDSP module event (jpegd)",0,0,0);

      if (event_ptr->mod.mod_status == QDSP_MOD_READY)
      {
        if(event_ptr->mod.module == QDSP_MODULE_JPEGD)
        {
          if (JPEG_dsp_state == RESET_STATE)
          {
            MSG_MED( "QDSP jpeg decoder image READY event", 0, 0, 0 );
            JPEG_dsp_state = INIT_STATE;

            /* setup to execute the jpeg decoder with different state */
            if (jpeg_cmx_cb != NULL)
            {
              jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
            }
            jpeg_state = JPEG_CHECK_HEADER;

          }
        }
        else
        {
          MSG_HIGH( "JPEGD: unrecognized image %d ready",0,0,0);
        }
      }
      else
      {
        if( jpeg_decode_in_progress == TRUE )
        {
        /* make sure to set the disable flag accordingly */
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
        jpegd_terminate();
#endif
        }
        qdsp_disable_event_rsp( QDSP_APP_IGG );
      }
     break;

#else

    case QDSP_EV_MODULE:
      MSG_MED("Received QDSP module event (jpegd)",0,0,0);
      break;

    case QDSP_EV_IMAGE:
      if (event_ptr->img.image_status == QDSP_IMAGE_READY)
      {
        if(event_ptr->img.image == QDSP_IMAGE_JPEGD)
        {
          if (JPEG_dsp_state == RESET_STATE)
          {

#ifdef JPEG_TIMING_TEST
             outpw(TIMETEST_PORT, 0xf0);
#endif
             MSG_MED( "QDSP jpeg decoder image READY event", 0, 0, 0 );
             JPEG_dsp_state = INIT_STATE;

             /* setup to execute the jpeg decoder with different state */
             jpeg_dsp_request.function_ptr =
                            (jpeg_cmx_command_cb_func_type) jpeg_decode_image;
             jpeg_dsp_request.client_data = (void *) jpeg_id;

             if (jpeg_cmx_cb != NULL)
             {
                jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
             }
             jpeg_state = JPEG_CHECK_HEADER;
          }
        }
        else
        {
          MSG_HIGH( "JPEGD: unrecognized image %d ready",
                    event_ptr->img.image, 0, 0 );
        }
      }
      else if (event_ptr->img.image_status == QDSP_IMAGE_RESET )
      {
        if (event_ptr->img.image == QDSP_IMAGE_JPEGD )
        {
          if (jpeg_decode_in_progress == TRUE )
          {
            /* make sure to set the disable flag accordinly */
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
            jpegd_terminate();
#endif
          }
          MSG_MED("MM allow reset %d", event_ptr->img.image,0,0);
          qdsp_allow_reset( QDSP_APP_IGG );
        }
        else
        {
          qdsp_allow_reset( QDSP_APP_IGG );
        }
      }
      break;
#endif /* FEATURE_QDSP_USES_MODULES */
    default:
      MSG_ERROR("Illegal QDSP Event type (jpegd)",0,0,0);
      break;
  }
} /* jpegd_q2a_callback */

 
/*===========================================================================

FUNCTION JPEGD_TERMINATE

DESCRIPTION

  This operation terminates the DSP decoder from operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

LOCAL void jpegd_terminate( void)
{
   jpeg_decode_in_progress = FALSE;
   if (!ARM_based)
   {
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
      if (JPEG_dsp_state == CONFIG_STATE || JPEG_dsp_state == DECODE_STATE)
      {
        jpegd_command(JPGPD_SLEEP_CMD, NULL);
      }
#endif
      qdsp_disable(QDSP_APP_IGG);
   }
}

#endif


/*===========================================================================

FUNCTION JPEGD_DSP_SUPPORT

DESCRIPTION
   Check for 6550 DSP support for certain picture formats 
   Grayscale Images, Progressive Images, Sequential Images 
   of H1V1, H1V2 format, Sequential images with two SOS 
   having Y and Cb,Cr respectively can be supported only
   through ARM based decoder.
   

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean jpegd_dsp_support()
{
    /* Make sure the new 6550 DSP support these picture formats */
    if (prog_flag || 
        (frameHSamplingFactor[0]  == 1) || /* does not support H1 */
        (numberOfComponentsInFrame == 1))  /* does not support grayscale */
    {
        return FALSE;
    }

#ifdef JPEGD_MFLO_SVC_ICON_SUPPORT
#error code not present
#endif
    
    return TRUE;
}


/*===========================================================================

Function: JPEGD_PARSEMARKERS

Description:  Parses the SOF, SOS, DQT, DHT, DRI - JPEG Markers
              
Return Value: Marker/JPEGD_STATUSMSG

Comments:

Side Effects:

============================================================================*/
int32 jpegd_parseMarkers(void)
{
  int32 c;

  for (;;)
  {
    if (total_byte_read > (jpegd_fileSize+4))
    {
      jpegd_statusMsg = JPEGD_INVALID_FILE_SIZE;
      MSG_ERROR ("JPEG Decoder: Inavlid File Size", 0, 0, 0);
      return (int32)jpegd_statusMsg;
    }
    
    c = jpegd_readNextMarker();

    switch (c)
    {
      case M_SOF0:
      case M_SOF1:
      case M_SOF2:
      case M_SOF3:
      case M_SOF5:
      case M_SOF6:
      case M_SOF7:
        //      case M_JPG:
      case M_SOF9:
      case M_SOF10:
      case M_SOF11:
      case M_SOF13:
      case M_SOF14:
      case M_SOF15:
      case M_SOI:
      case M_EOI:
      case M_SOS:
        {
          return c;
        }
      case M_DHT:
        {
          if (jpeg_read_dht(read_huff.bits, read_huff.huffval) == FALSE)
          { 
             jpegd_statusMsg = JPEGD_INVALID_DHT_MARKER; 
             MSG_ERROR ("JPEG Decoder: Inavlid DHT Marker", 0, 0, 0);
             return (int32)jpegd_statusMsg;
          }
          break;
        }
      case M_DAC:
        {
          jpegd_statusMsg = JPEGD_NO_ARITHMETIC_DECODING_SUPPORT; 
          MSG_ERROR ("JPEG Decoder: Arithmetic coding not supported", 0, 0, 0);
          return (int32)jpegd_statusMsg;
        }
      case M_DQT:
        {
          if (jpegd_interpretDQTMarker() != JPEGD_DECODING_OK)
          {
            jpegd_statusMsg = JPEGD_INVALID_DQT_TABLE; 
            MSG_ERROR ("JPEG Decoder: Invalid DQT Marker", 0, 0, 0);
            return (int32)jpegd_statusMsg;
          }
          break;
        }
      case M_DRI:
        {
          if (jpegd_readDRIMarker() != JPEGD_DECODING_OK)
          {
            jpegd_statusMsg = JPEGD_INVALID_DRI_MARKER; 
            MSG_ERROR ("JPEG Decoder: Invalid DRI Marker", 0, 0, 0);
            return (int32)jpegd_statusMsg;
          }
          break;
        }
        //case M_APP0:  /* no need to read the JFIF marker */

      case M_JPG:
        {
          jpegd_statusMsg = JPEGD_UNSUPPORTED_MARKER; 
          MSG_ERROR ("JPEG Decoder: Unsupported Marker", 0, 0, 0);
          return (int32)jpegd_statusMsg;
        }
      default:    
        {
          if ((jpegd_skipMarker()) != JPEGD_DECODING_OK)
          {
            jpegd_statusMsg = JPEGD_UNSUPPORTED_MARKER; 
            MSG_ERROR ("JPEG Decoder: Unsupported Marker", 0, 0, 0);
            return (int32)jpegd_statusMsg;
          }
          break;
        }
    }
  }
}


/*===========================================================================

Function:  jpeg_init

Description:  Reset everything to default state.

Return Value:  None

Comments: 

Side Effects:
============================================================================*/
void jpeg_init()
{
   
  EOBrun = 0;

  linesDecoded = 0;
  byteOffset = 0;

  mcu_rows_decoded = 0;
  total_rows_decoded = 0;
  mcu_rows_per_decode = 1;
  
  readyFlag = FALSE;
  jpegd_imageWidth = jpegd_imageHeight = 0;
  decode_y_size = 0;
  memset(jpegd_quant, 0, sizeof(jpegd_quant));

  error_code = 0;
  
  comp_data = NULL;
 
  total_byte_read = 0;
  file_position = 0;
  half_buffer_select = 1;

  DSP_decode = FALSE;
  prog_flag = FALSE;
 
  mem_allocated = FALSE;
  
  image_done = FALSE;
  scan_display = 0;

  mcu_info.subsample = JPEG_H2V2;

  numberOfComponentsInFrame = 0;

  memset(frameHSamplingFactor, 0, sizeof(frameHSamplingFactor));
  memset(frameVSamplingFactor, 0, sizeof(frameVSamplingFactor));
  memset(frameQuantTableSelector, 0, sizeof(frameQuantTableSelector));
  memset(frameCompID, 0, sizeof(frameCompID));
  memset(hMCUBlock, 0, sizeof(hMCUBlock));
  memset(vMCUBlock, 0, sizeof(vMCUBlock));

  numberOfComponentsInScan  = 0;
  memset(scanCompSelector, 0, sizeof(scanCompSelector));
  memset(scanDCTableSelector, 0, sizeof(scanDCTableSelector));
  memset(scanACTableSelector, 0, sizeof(scanACTableSelector));

  maxMCUrowSize = 8;
  maxMCUcolSize = 8;

  dataUnitsperMCU = 0;
  maxDataUnitsperRow = 0;
  mcusperRow = 0;
  mcusperCol = 0;

  memset(mcuBlk, 0, sizeof(mcuBlk));

  totalLinesRemaining = 0;
  mcuLinesRemaining = 0;

  bytesPerPixel = 0;
   
  restart_interval = 0;
  restarts_left    = 0;
  next_restart_num = 0;
  mcu_to_skip      = 0;   

  maxMCUsperRow = 0;
  maxDataUnitsperMCU = 4;
  mcu_info.blk_count = 4;
  mcu_info.in_row = 0;
  mcu_info.in_col = 0;
  
  memset(last_dc_val, 0, sizeof(last_dc_val));
  
  in_data_buf = (uint8 *) JPEG_MALLOC(MAX_DATA_BYTES*2);
  in_dataPtr = in_data_buf;
  jpegd_statusMsg = JPEGD_DECODING_INIT;
  isDQTTablePresent = FALSE;

#ifdef FEATURE_JPEG_DECODER_REV2
  jpegd_totalBytesRead = 0;
  jpegd_dspBstrmPtr = in_data_buf;
#endif
  
  jpegdResizeFlag = FALSE;

#ifdef FEATURE_JPEGD_RESIZE
  jpegd_resizeCol    = 0;
  jpegd_resizeColCum = 0;
  isResizable = TRUE;
  resizeNow   = FALSE;
  jpegd_pixRow = 0;
#endif


  jpeg_huff_init();

  memset(comp_buffer_1, 0, sizeof(comp_buffer_1));
  memset(comp_buffer_2, 0, sizeof(comp_buffer_2));
  
  memset(dc_derived_table, 0, sizeof(dc_derived_table));
  memset(ac_derived_table, 0, sizeof(ac_derived_table));

}

/*===========================================================================

Function: JPEGD_CALCULATEPIXELASPECTRATIO

Description:  Calculates the aspect ratio
              
Return Value: None
   
Comments: This function calculates the aspect ratio and recomputes the resize
          dimensions if required

Side Effects:

============================================================================*/

#ifdef FEATURE_JPEGD_RESIZE
boolean jpegd_calculatePixelAspectRatio()
{
  uint32 resizeImageHeight;
  uint32 resizeImageWidth;
   
      
  resizeImageHeight = (localjpegdInfo.resizeImageWidth * localjpegdInfo.height ) / (localjpegdInfo.width);

  if (resizeImageHeight > localjpegdInfo.resizeImageHeight)
  {
    resizeImageWidth = (localjpegdInfo.resizeImageHeight * localjpegdInfo.width ) / (localjpegdInfo.height);
    resizeImageHeight = localjpegdInfo.resizeImageHeight;
  }
  else
  {
    resizeImageWidth = localjpegdInfo.resizeImageWidth;
  }

  /* If width and height are odd, make it even to work for YCbCr data */
  localjpegdInfo.resizeImageWidth = (resizeImageWidth & 1)? (resizeImageWidth + 1):resizeImageWidth;
  localjpegdInfo.resizeImageHeight = (resizeImageHeight & 1)? (resizeImageHeight + 1):resizeImageHeight;
 
  if ((localjpegdInfo.resizeImageWidth < JPGD_MIN_SCALED_WIDTH ) || (localjpegdInfo.resizeImageHeight < JPGD_MIN_SCALED_HEIGHT))
  {
    MSG_HIGH("JPEGDSPDEC: Invalid resize dimensions, reverting to original dimensions",0,0,0);
    return FALSE;
  }

  
  return TRUE;
}
#endif


/*===========================================================================
Function: JPEGD_PROCESSRESTART

Description:  Process Restart Marker

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/
boolean jpegd_processRestart(void)
{
  uint8 current_marker = 0;
  uint8 next_marker = 0;
  uint8 markers_skiped = 0;
  uint8 c = 0;
  
  /* Reset each component's DC prediction values. */
  memset(last_dc_val, 0, numberOfComponentsInFrame * sizeof(uint32));

  restarts_left = restart_interval;

  do
  {
    do
    {
      c = fetchByte();
    } while ((c != 0xFF)&&( total_byte_read < (jpegd_fileSize + 4)));

    if (c == 0xFF)
    {
      current_marker = fetchByte();
    }
  } while (((current_marker < (uint8)M_RST0) || (current_marker > (uint8)M_RST7)) &&
           ( total_byte_read < (jpegd_fileSize + 4)));

  /* we have reached end of file without encountering restart marker */
  if ( total_byte_read >= (jpegd_fileSize + 4))
  {
    mcu_to_skip = 0xFFFFFFFF;
    return FALSE;
  }

  next_marker = (uint8)M_RST0 + next_restart_num;
  markers_skiped = (8 + current_marker - next_marker) & 0x7;
  mcu_to_skip = restart_interval * markers_skiped;
  next_restart_num = (next_restart_num + 1 + markers_skiped) & 0x7;

  
  bitsRemaining = 32;
  (void)fetchBits(16,TRUE);
  (void)fetchBits(16,TRUE);
  
  return TRUE;
}




/*===========================================================================
Function: JPEGD_LOADDECODENEXTROW

Description: Final Stage of JPEG Decoding - IDCT

Return Value: NONE

Comments: 

Side Effects:
============================================================================*/

void jpegd_loadDecodeNextRow(void)
{
  uint32 i;
  DATAUNIT_DESC *p;
  QUANT_DESC *q;
  uint8 *Pdst_ptr;
  uint32 mcu_row, mcu_dataUnit;
  uint32 component_id;
  uint32 dataUnit_x_mcu_ofs = 0, dataUnit_y_mcu_ofs = 0;
  
  lastDestPtr = imageOut;
  
  
  for (i=0; i<numberOfComponentsInScan ; i++)
  {
	  comp_data[i].dataUnit_x_mcu = 0;
  }

  for (mcu_row = 0; mcu_row < mcusperRow; mcu_row++)
  {
    
    for (mcu_dataUnit = 0; mcu_dataUnit < dataUnitsperMCU; mcu_dataUnit++)
    {
      component_id = mcuBlk[mcu_dataUnit];
      
	    p = (DATAUNIT_DESC *) (comp_data[component_id].seg_ptr + (comp_data[component_id].dataUnit_x_mcu 
		                  + dataUnit_x_mcu_ofs) * 64 + (comp_data[component_id].dataUnit_y_mcu 
						  + dataUnit_y_mcu_ofs) * (64 * comp_data[component_id].dataUnit_num_x));
      
	    Pdst_ptr = lastDestPtr;
	    lastDestPtr = Pdst_ptr + 64;
  
      q = jpegd_quant[frameQuantTableSelector[component_id]];

      for (i = 0; i < 64; i++)
      {
        tempDataUnit[ZAG[i]] = (DATAUNIT_DESC)(p[i]*q[i]);
      }
      
      idct(tempDataUnit, Pdst_ptr);
      
      if (numberOfComponentsInScan  == 1)
        comp_data[component_id].dataUnit_x_mcu++;
      else
      {
        if (++dataUnit_x_mcu_ofs == frameHSamplingFactor[component_id])
        {
          dataUnit_x_mcu_ofs = 0;

          if (++dataUnit_y_mcu_ofs == frameVSamplingFactor[component_id])
          {
            dataUnit_y_mcu_ofs = 0;

            comp_data[component_id].dataUnit_x_mcu += frameHSamplingFactor[component_id];
          }
        }
      } 

    }
  }

  if (numberOfComponentsInScan  == 1)
    comp_data[scanCompSelector[0]].dataUnit_y_mcu++;
  else
  {
    for (i = 0; i < numberOfComponentsInScan ; i++)
    {
      component_id = scanCompSelector[i];

      comp_data[component_id].dataUnit_y_mcu += frameVSamplingFactor[component_id];
    }
  }
}

/*===========================================================================
Function: JPEGD_DECODESEQUENTIAL

Description:  JPEG Sequential Decoding

Return Value: JPGD_OKAY/JPGD_FAILED

Comments: 

Side Effects:
============================================================================*/
jpeg_return_type jpegd_decodeSequential(MCU_info_type *mcuPtr)
{

  if ((jpegd_statusMsg) || (!readyFlag))
    return (JPGD_FAILED);
 
  if (jpeg_dec_mcu_row(1, mcuPtr->per_row) == FALSE)
    return (JPGD_FAILED);

  mcuPtr->in_row = 0;
  mcuPtr->in_col++;

  totalLinesRemaining-=maxMCUcolSize;

  return (JPGD_OKAY);
}



/*===========================================================================
Function: JPEGD_CALCULATEMCU

Description:  Calculate MCU BLK

Return Value: JPEGD_STATUS_MSG

Comments: 

Side Effects:
============================================================================*/
JPEGD_STATUS_MSG jpegd_calculateMCU(void)
{
  uint32 component_num, component_id;
  uint32 max_h_samp = 0, max_v_samp = 0;
  uint32 num_dataUnits;
  
  for (component_id = 0; component_id < numberOfComponentsInFrame; component_id++)
  {
    if (frameHSamplingFactor[component_id] > max_h_samp)
      max_h_samp = frameHSamplingFactor[component_id];
    
    if (frameVSamplingFactor[component_id] > max_v_samp)
      max_v_samp = frameVSamplingFactor[component_id];
  }
  
  if((max_h_samp == 0) || (max_v_samp == 0))
  {
    jpegd_statusMsg = JPEGD_INVALID_SAMPLING_FACTOR; 
    MSG_ERROR ("JPEG Decoder: Invalid Sampling Factor", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  
  for (component_id = 0; component_id < numberOfComponentsInFrame; component_id++)
  {
    hMCUBlock[component_id] = ((((jpegd_imageWidth * frameHSamplingFactor[component_id]) + (max_h_samp - 1)) / max_h_samp) + 7) / 8;
    vMCUBlock[component_id] = ((((jpegd_imageHeight * frameVSamplingFactor[component_id]) + (max_v_samp - 1)) / max_v_samp) + 7) / 8;
  }
  
  if (numberOfComponentsInScan  == 1)
  {
    mcusperRow = hMCUBlock[scanCompSelector[0]];
    mcusperCol = vMCUBlock[scanCompSelector[0]];
    mcuBlk[0] = scanCompSelector[0];
    dataUnitsperMCU = 1;
    
    if ((!prog_flag) && (numberOfComponentsInFrame == 3))
      mcu_info.subsample = JPEGD_GRAY;
  }
  else
  {
    mcusperRow = (((jpegd_imageWidth + 7) / 8) + (max_h_samp - 1)) / max_h_samp;
    mcusperCol = (((jpegd_imageHeight + 7) / 8) + (max_v_samp - 1)) / max_v_samp;
    dataUnitsperMCU = 0;
    
    for (component_num = 0; component_num < numberOfComponentsInScan; component_num++)
    {
      component_id = scanCompSelector[component_num];
      
      num_dataUnits = frameHSamplingFactor[component_id] * frameVSamplingFactor[component_id];
      
      while (num_dataUnits--)
        mcuBlk[dataUnitsperMCU++] = component_id;
    }
  }
  
  if (mcu_info.subsample != JPEGD_GRAY)
  {
    if (numberOfComponentsInScan > 1)  
    {
      mcu_info.subsample = subsampletype;
    }
    else
    {
      mcu_info.subsample = JPEG_H1V1;
    }
  }
  
  
  mcu_info.blk_count = dataUnitsperMCU;
  mcu_info.per_row = mcusperRow;
  mcu_info.per_col = mcusperCol;
  return JPEGD_DECODING_OK;
}
  

/*===========================================================================
Function: JPEGD_INITIALIZESCAN

Description:  Initialize the Decoder Scan

Return Value: JPEGD_STATUS_MSG/JPEGD_DECODING_OK

Comments: 

Side Effects:
============================================================================*/

JPEGD_STATUS_MSG jpegd_initializeScan(void)
{
  JPEGD_STATUS_MSG       jpegd_statusMsgTmp;   
 
  jpegd_statusMsgTmp = jpegd_interpretSOSMarker();

  if (jpegd_statusMsgTmp  == JPEGD_END_OF_IMAGE)
  {
    jpegd_statusMsgTmp = JPEGD_END_OF_IMAGE; 
    MSG_LOW ("JPEG Decoder: End of Image", 0, 0, 0);
    return jpegd_statusMsgTmp;
  }

  if (jpegd_statusMsgTmp  != JPEGD_DECODING_OK)
  {
    jpegd_statusMsg = JPEGD_ERROR_INTERPRETING_MARKER; 
    MSG_ERROR ("JPEG Decoder: Error Interpreting SOS Marker", 0, 0, 0);
    return jpegd_statusMsg;
  }

  if (jpegd_calculateMCU() != JPEGD_DECODING_OK)
  {
    jpegd_statusMsg = JPEGD_ERROR_CALCULATING_MCU;
    MSG_ERROR ("JPEG Decoder: Error in Calculating MCU", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  if(jpeg_check_huff_tables(numberOfComponentsInScan) != JPEGD_DECODING_OK)
  {
    jpegd_statusMsg = JPEGD_INVALID_DHT_TABLE;
    MSG_ERROR ("JPEG Decoder: Error in Huffman Table during initialize scan", 0, 0, 0);
    return jpegd_statusMsg;
  }


  memset(last_dc_val, 0, numberOfComponentsInFrame * sizeof(uint32));

  if (restart_interval)
  {
    restarts_left = restart_interval;
    next_restart_num = 0;
  }

  jpegd_correctBuffer();

  return JPEGD_DECODING_OK ;
}



/*===========================================================================
Function: JPEGD_INITIALIZEFRAME

Description:  Initialize the Decoder Frame

Return Value: NONE

Comments:  JPEGD_STATUS_MSG/JPEGD_DECODING_OK

Side Effects:
============================================================================*/

JPEGD_STATUS_MSG jpegd_initializeFrame(MCU_info_type *mcu_infoPtr)
{
  uint32  i; 
  uint32  complete_mcus_per_col = 0;
  uint32  mcu_rows_require_to_allocate = 0;

  total_coeff_size = 0;
  
  if (numberOfComponentsInFrame == 1)
  {
    subsampletype  = JPEGD_GRAY;
    maxDataUnitsperMCU = 1;
    mcu_infoPtr->blk_count = 1;
    bytesPerPixel = 1;
    
  }
  else if (numberOfComponentsInFrame == 3)
  {
    if ( ((frameHSamplingFactor[1] != 1) || (frameVSamplingFactor[1] != 1)) ||
      ((frameHSamplingFactor[2] != 1) || (frameVSamplingFactor[2] != 1)) )
    {
      jpegd_statusMsg = JPEGD_UNSUPPORTED_SAMPLING_FACTOR;
      MSG_ERROR ("JPEG Decoder: Unsupported Sampling Factor", 0, 0, 0);
      return jpegd_statusMsg;
    }
    
    if ((frameHSamplingFactor[0] == 1) && (frameVSamplingFactor[0] == 1))
    {
      subsampletype = JPEG_H1V1;
      maxDataUnitsperMCU = 3;
      mcu_infoPtr->blk_count = 3;
      
      
    }
    else if ((frameHSamplingFactor[0] == 2) && (frameVSamplingFactor[0] == 1))
    {
      subsampletype = JPEG_H2V1;
      maxMCUrowSize     = 16;
      
    }
    else if ((frameHSamplingFactor[0] == 1) && (frameVSamplingFactor[0] == 2))
    {
       subsampletype = JPEG_H1V2;
       maxMCUcolSize     = 16;
       
    }
    else if ((frameHSamplingFactor[0] == 2) && (frameVSamplingFactor[0] == 2))
    {
      subsampletype = JPEG_H2V2;
      maxDataUnitsperMCU = 6;
      mcu_infoPtr->blk_count = 6;
      maxMCUrowSize     = 16;
      maxMCUcolSize     = 16;
      
    }
    else
    {
      jpegd_statusMsg = JPEGD_UNSUPPORTED_SAMPLING_FACTOR;
      MSG_ERROR ("JPEG Decoder: Unsupported Sampling Factor", 0, 0, 0);
      return jpegd_statusMsg;
    }
  }
  else
  {
    jpegd_statusMsg = JPEGD_UNSUPPORTED_COLORSPACE;
    MSG_ERROR ("JPEG Decoder: Unsupported ColorSpace", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  mcu_infoPtr->subsample = subsampletype;
  progSubSampleType = subsampletype;
  
  maxMCUsperRow = (jpegd_imageWidth + (maxMCUrowSize - 1)) / maxMCUrowSize;
  mcu_infoPtr->per_row = maxMCUsperRow;
  maxMCUsperCol = (jpegd_imageHeight + (maxMCUcolSize - 1)) / maxMCUcolSize;
  mcu_infoPtr->per_col = maxMCUsperCol;
  
  complete_mcus_per_col = jpegd_imageHeight / maxMCUcolSize;
  
  /* for progressive, we use only one buffer to decode */
  if ((prog_flag == TRUE) || (mcu_infoPtr->per_col < 2) || 
    (mcu_rows_per_decode >= (mcu_infoPtr->per_col - 1)))
  {
    mcu_rows_per_decode = mcu_infoPtr->per_col;
    decode_y_size = jpegd_imageHeight;
    mcu_rows_require_to_allocate = mcu_rows_per_decode;
  }
  /* no padding require, decode two row at the end */
  else if (mcu_infoPtr->per_col == complete_mcus_per_col)
  {
    mcu_rows_require_to_allocate = mcu_rows_per_decode + 1;
    decode_y_size = maxMCUcolSize * mcu_rows_require_to_allocate;
  }
  /* padding require, decode one row + padding at the end */
  else
  {
    mcu_rows_require_to_allocate = mcu_rows_per_decode + 1;
    decode_y_size = (maxMCUcolSize * mcu_rows_per_decode) + (jpegd_imageHeight % maxMCUcolSize);
  }
  
  if ((localjpegdInfo.outFormat == JPEGD_RGB565) ||
    (localjpegdInfo.outFormat == JPEGD_YCbCr422lp) ||
    (localjpegdInfo.outFormat == JPEGD_YCbCr420lp) ||
    (localjpegdInfo.outFormat == JPEGD_YCrCb422lp) ||
    (localjpegdInfo.outFormat == JPEGD_YCrCb420lp))
  {
    bytesPerPixel = 2;
  }
  else if (localjpegdInfo.outFormat == JPEGD_RGB888)
  {
    bytesPerPixel = 3;
  }
  
  if (localjpegdInfo.outFormat == JPEGD_YCbCr422lp)
    jpegdIPLOut.cFormat = IPL_YCbCr422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCbCr420lp)
    jpegdIPLOut.cFormat = IPL_YCbCr420_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb422lp)
    jpegdIPLOut.cFormat = IPL_YCrCb422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb420lp)
    jpegdIPLOut.cFormat = IPL_YCrCb420_LINE_PK;
  
  
  bytes_per_line = jpegd_imageWidth * bytesPerPixel;
  
  maxDataUnitsperRow = maxMCUsperRow * maxDataUnitsperMCU;
  
  for (i = 0; i <numberOfComponentsInFrame ; i++)
  {
    
    comp_buffer_1[i].comp_h_samp    = frameHSamplingFactor[i];
    comp_buffer_1[i].comp_v_samp    = frameVSamplingFactor[i];
    comp_buffer_1[i].dataUnit_num_x = mcu_infoPtr->per_row * frameHSamplingFactor[i];
    comp_buffer_1[i].dataUnit_num_y = mcu_infoPtr->per_col * frameVSamplingFactor[i];
    comp_buffer_1[i].coeff_size = comp_buffer_1[i].dataUnit_num_x * mcu_rows_require_to_allocate * 
    frameVSamplingFactor[i] * JPGD_BLOCKSIZE;
    
    comp_buffer_1[i].seg_ptr = (DATAUNIT_DESC *) JPEG_MALLOC(comp_buffer_1[i].coeff_size *  sizeof(DATAUNIT_DESC));
    
    if (comp_buffer_1[i].seg_ptr == NULL) 
    {
      jpeg_decode_abort_flag = TRUE;
      jpegd_statusMsg = JPEGD_NOT_SUFFICIENT_MEMORY;
      MSG_ERROR ("JPEG Decoder: Not sufficient memory", 0, 0, 0);
      return jpegd_statusMsg;
    }
    
    mem_allocated = TRUE;
    
    memset(comp_buffer_1[i].seg_ptr, 0, comp_buffer_1[i].coeff_size*sizeof(DATAUNIT_DESC));

    /* determine if we need to allocate buffer2 */       
    if (mcu_rows_per_decode < mcu_infoPtr->per_col)
    {
      comp_buffer_2[i].comp_h_samp    = frameHSamplingFactor[i];
      comp_buffer_2[i].comp_v_samp    = frameVSamplingFactor[i];
      comp_buffer_2[i].dataUnit_num_x = mcu_infoPtr->per_row * frameHSamplingFactor[i];
      comp_buffer_2[i].dataUnit_num_y = mcu_infoPtr->per_col * frameVSamplingFactor[i];
      
      comp_buffer_2[i].coeff_size = comp_buffer_2[i].dataUnit_num_x * mcu_rows_require_to_allocate * 
                                                          frameVSamplingFactor[i] * JPGD_BLOCKSIZE;
      
      comp_buffer_2[i].seg_ptr = (DATAUNIT_DESC *) JPEG_MALLOC(comp_buffer_2[i].coeff_size*sizeof(DATAUNIT_DESC));
      
      if (comp_buffer_2[i].seg_ptr == NULL) 
      {
        jpeg_decode_abort_flag = TRUE;
        jpegd_statusMsg = JPEGD_NOT_SUFFICIENT_MEMORY;
        MSG_ERROR ("JPEG Decoder: Not sufficient memory", 0, 0, 0);
        return jpegd_statusMsg;
      }
      memset(comp_buffer_2[i].seg_ptr, 0, comp_buffer_2[i].coeff_size*sizeof(DATAUNIT_DESC));
    }
    total_coeff_size += comp_buffer_1[i].dataUnit_num_x * frameVSamplingFactor[i];
  }
  
  /* set current pointer to buffer1 */
  comp_data = comp_buffer_1;
  current_comp_index = 0;
  
  if (ARM_based)
  {
      if(mcu_info.subsample == JPEG_H1V1 || mcu_info.subsample == JPEG_H2V1)
      {
          /* This is to perform 16:x downscaling instead of 8:x in case of H1V1 and H2V1*/ 
          imageOut = (uint8 *)JPEG_MALLOC(total_coeff_size * JPGD_BLOCKSIZE*2);
      }
      else
      {
          imageOut = (uint8 *)JPEG_MALLOC(total_coeff_size * JPGD_BLOCKSIZE);
      }
      

    if (imageOut == NULL) 
    {
      jpegd_statusMsg = JPEGD_NOT_SUFFICIENT_MEMORY;
      MSG_ERROR ("JPEG Decoder: Not sufficient memory", 0, 0, 0);
      return jpegd_statusMsg;
    }
  }
  
  
  if (maxDataUnitsperRow > JPGD_MAXBLOCKSPERROW)
  {
    jpegd_statusMsg =  JPEGD_DECODING_ERROR;
    MSG_ERROR ("JPEG Decoder: JPEG Decoding error", 0, 0, 0);
    return jpegd_statusMsg;
  }
  
  for (i = 0; i < maxDataUnitsperRow; i++)
    dataUnit_max_zag_set[i] = 64;
  
  totalLinesRemaining = jpegd_imageHeight;
  
  mcuLinesRemaining = 0;
  
  
  return JPEGD_DECODING_OK ;
}
/*===========================================================================

Function: JPEG_DECODE_SCAN

Description:  This function decode the next scan 

Return Value:  None

Comments: JPGD_OKAY/JPGD_FAILED

Side Effects:
============================================================================*/
jpeg_return_type jpeg_decode_scan()
{
  uint32    cnt;
  //static   scan_display = 0;
  
  if (prog_flag)
  {
    if (!jpeg_decode_prog_scan(mcu_info,numberOfComponentsInScan, mcuBlk) || 
		    (++scan_display == 19))
    {
      image_done = TRUE;
      
      numberOfComponentsInScan  = numberOfComponentsInFrame;
      for (cnt = 0; cnt < numberOfComponentsInFrame; cnt++)
      {
        scanCompSelector[cnt] = cnt;
        comp_data[cnt].dataUnit_x_mcu = 0;
        comp_data[cnt].dataUnit_y_mcu = 0;
      }
      
      if (jpegd_calculateMCU() != JPEGD_DECODING_OK)
      {
        jpegd_statusMsg = JPEGD_ERROR_CALCULATING_MCU;
        MSG_ERROR ("JPEG Decoder: Error in Calculating MCU", 0, 0, 0);
        return JPGD_FAILED;
      }
    }
    if ( image_done )
    {
      total_rows_decoded = mcu_info.per_col;
      
    }
    
  }
  else
  {
    if (jpegd_decodeSequential(&mcu_info) != JPGD_OKAY)
    {
      MSG_ERROR ("JPEG Decoder Failed",0,0,0) ;
      return JPGD_FAILED;
    }
    linesDecoded++;
    mcu_rows_decoded++;
    total_rows_decoded++;
    
    if (total_rows_decoded == (mcu_info.per_col - 1))
    {
      mcu_rows_per_decode = mcu_rows_decoded;
    }
    else 
    {
      mcu_rows_per_decode = mcu_rows_decoded;
      for (cnt = 0; cnt < numberOfComponentsInFrame; cnt++)
        comp_data[cnt].dataUnit_y_mcu = 0;
      mcu_info.in_col = 0;
    } 
    
  }
  
  return JPGD_OKAY;
}

/*===========================================================================

Function: RELEASE_MEMORY

Description:  Release all dynamically allocated memory blocks

Return Value:  None

Comments: 

Side Effects:
============================================================================*/

void release_memory(void)
{
  uint32 i;
  
  if (in_data_buf)
  {
    JPEG_FREE(in_data_buf);
    in_data_buf = NULL;
  }
  else
  {
    jpeg_decode_in_progress = FALSE;
    return;
  }
  
  if (mem_allocated)
  {
    for (i=0; i<numberOfComponentsInFrame; i++) 
    {
      if (comp_buffer_1[i].seg_ptr) 
      {
        JPEG_FREE(comp_buffer_1[i].seg_ptr);
        comp_buffer_1[i].seg_ptr = NULL;
      }
      if (comp_buffer_2[i].seg_ptr) 
      {
        JPEG_FREE(comp_buffer_2[i].seg_ptr);
        comp_buffer_2[i].seg_ptr = NULL;
      }
    }
    
    if (ARM_based)
    {
      JPEG_FREE(imageOut); imageOut = NULL;
      JPEG_FREE(zerothRowScan); zerothRowScan = NULL;
      JPEG_FREE(firstRowScan); firstRowScan = NULL; 

    }
  }

  jpeg_decode_in_progress = FALSE;

#ifdef JPEG_TIMING_TEST
   outpw(TIMETEST_PORT, 0x0);
#endif   

#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
   jpegd_terminate();
#endif

  ARM_based = FALSE;
   
#if defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

   /* Cancel the request to operate in hyper mode */
  clk_regime_deregister_for_cpu_resource(CLKRGM_JPEG_APPS);

#endif

#ifdef FEATURE_NEW_SLEEP_API
     sleep_assert_okts(jpeg_sleep_okts_handle);
#endif

  MSG_LOW("Released all allocated memory in JPEGD", 0, 0, 0);
}


/*===========================================================================

Function:  JPEG_DECODE_IMAGE

Description:  This method keeps the states for the JPEG decoding.
              Base on the state, it determines what to do for next time it
              got the CPU time.

Return Value:  None

Comments: 

Side Effects:
============================================================================*/
void jpeg_decode_image(void *cb_data )
{
  uint8 i;
  jpeg_cmx_buf_req_type cmx_buf_req;
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
  JPG_PD_config_struct  configData;
  JPG_PD_command_struct decodeData;
#endif

  /* if not from the current decode session, skip it */
  if (cb_data != jpeg_id)
     return;

  /* if abort flag set then abort decoder                                  */
  if ( jpeg_decode_abort_flag )
  {
    release_memory();

    if ( jpeg_cmx_cb != NULL )
    {
      jpeg_cmx_cb (JPEG_CMX_ABORT, jpeg_cmx_handle, NULL);
    }
    
    return;
  }

  /* JPEG decoder state machine   */
  MSG_HIGH ("JPEG decode image, state = %d", jpeg_state, 0, 0);
  switch ( jpeg_state )
  {
      case JPEG_INIT_READ_DONE:
        if (!ARM_based)
        {
#ifdef JPEG_TIMING_TEST
          outpw(TIMETEST_PORT, 0x70);
#endif

#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
           jpegd_initialize();     /* down load the DSP code */
#endif
        }
        break;

      case JPEG_STATE_INIT:

        jpeg_init();         /* initialize JPEG decoder variables */
        /* Set up a callback request */
        jpeg_dsp_request.function_ptr =
                           (jpeg_cmx_command_cb_func_type) jpeg_decode_image;
        jpeg_dsp_request.client_data = (void *) jpeg_id;
           
        /* if jpeg_init() fails to allocate memory, abort the decoding */
        if (in_dataPtr == NULL) 
        {
          jpeg_decode_abort_flag = TRUE;
          if (jpeg_cmx_cb != NULL)
          {
            jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
          }
          break;
        }

        image_done = FALSE;

        /* Get size of file to be decoded */
        if (jpeg_cmx_cb != NULL)
        {
           jpeg_cmx_cb(JPEG_CMX_GET_FILE_SIZE, jpeg_cmx_handle, &jpegd_fileSize);
        }
        data_byte_remain = jpegd_fileSize;

        if (jpegd_fileSize > (2 * MAX_DATA_BYTES))
        {
           jpeg_get_data.num_bytes = (2 * MAX_DATA_BYTES);
           file_position += (2 * MAX_DATA_BYTES);
           data_byte_remain -= (2 * MAX_DATA_BYTES);
        }
        else
        {
           jpeg_get_data.num_bytes = jpegd_fileSize;
           file_position += jpegd_fileSize;
           data_byte_remain = 0;
        }

        /* setup to read JPEG header data                                  */
        jpeg_get_data.buffer = in_data_buf;
        jpeg_get_data.position = 0;

#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
        if (!ARM_based) 
        {
          jpeg_state = JPEG_INIT_READ_DONE;  /* if DSP mode, next is DSP download */
        }
        else
#endif
        {
          jpeg_state = JPEG_CHECK_HEADER;
        }
        jpeg_get_data.callback_ptr = jpeg_decode_image;
        jpeg_get_data.client_data  = (void *) jpeg_id;
        
        if (jpeg_cmx_cb != NULL)
        {
          jpeg_cmx_cb(JPEG_CMX_GET_DATA, jpeg_cmx_handle, &jpeg_get_data);
        }
        
        break;
        
      case JPEG_CHECK_HEADER:
        
        bitsRemaining = 32;
        dummyBits = fetchBits(16,FALSE);
        dummyBits = fetchBits(16,FALSE);


        if (jpegd_findSOFMarker() != JPEGD_DECODING_OK)
        {
          release_memory();
          if (jpeg_cmx_cb != NULL)
          {
            jpeg_cmx_cb(JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
          }
          
          jpeg_decode_abort_flag = TRUE;
          jpeg_state = JPEG_STATE_INIT;
          
          MSG_ERROR ("JPEG decoder error in locating SOF marker",0,0,0) ;
          return;
        }
        
        /* should always pass file specifications back to the client         */
        if (jpeg_cmx_cb != NULL)
        {
           jpeg_spec.width            = jpegd_imageWidth;
           jpeg_spec.height           = jpegd_imageHeight;
           jpeg_spec.num_components   = numberOfComponentsInFrame;

#ifdef FEATURE_JPEGD_RESIZE
           localjpegdInfo.width = jpegd_imageWidth;
           localjpegdInfo.height = jpegd_imageHeight;
           jpegd_localImageHeight = jpegd_imageHeight;

           if(localjpegdInfo.jpegdResizeFlag)
           {
             if (!(localjpegdInfo.resizeImageWidth) || !(localjpegdInfo.resizeImageHeight))
             {
               MSG_HIGH("JPEGDSPDEC: Image Width or Height cannot be zero, reverting to original dimensions",0,0,0);
               isResizable = FALSE;
             }

             if (((localjpegdInfo.resizeImageWidth %2) != 0 ) || ((localjpegdInfo.resizeImageHeight % 2) != 0 ))
             {
               MSG_HIGH("JPEGDSPDEC: No support for odd resize resolution, reverting to original dimensions",0,0,0);
               isResizable = FALSE;
             }
             if ((localjpegdInfo.resizeImageWidth > localjpegdInfo.width ) || (localjpegdInfo.resizeImageHeight >localjpegdInfo.height))
             {
               MSG_HIGH("JPEGDSPDEC: No support for upscaling, reverting to original dimensions",0,0,0);
               isResizable = FALSE;
             }

             if(localjpegdInfo.aspectRatioFlag && isResizable)
               isResizable = jpegd_calculatePixelAspectRatio();
             
             if (!(jpegd_dsp_support()) || (ARM_based))
             {
              if (jpegd_calculateMCU() != JPEGD_DECODING_OK)
              {
                MSG_ERROR ("JPEG Decoder: Error in Calculating MCU", 0, 0, 0);
                isResizable = FALSE;
              }
               
               if ((mcu_info.subsample == JPEG_H2V2) || (mcu_info.subsample == JPEG_H1V2))
               {
                 maxMCUcolSize = 16;
               }

               if ((localjpegdInfo.resizeImageHeight / (jpegd_localImageHeight / (float)maxMCUcolSize)) < 0.5 )
               {
                 MSG_HIGH("JPEGDSPDEC: Invalid resize dimensions, reverting to original dimensions",0,0,0);
                 isResizable = FALSE;
               }
             }

             if (isResizable)
             {
               localjpegdInfo.width = localjpegdInfo.resizeImageWidth;
               localjpegdInfo.height = localjpegdInfo.resizeImageHeight;
               jpegd_imageWidthEven  = (jpegd_imageWidth & 1) ? (jpegd_imageWidth + 1):jpegd_imageWidth;
               jpegdResizeFlag = TRUE;
             }
           }
#endif

           if (numberOfComponentsInFrame == 1)
           {
             jpeg_spec.color_space = JPEG_GRAY;
             if (!jpegdResizeFlag)
             {
#ifdef JPEG_USES_LIBSTD
               (void)std_strlprintf(tstring, sizeof(tstring), "P5\n%d %d\n255\n\0", (int)jpegd_imageWidth, 
                           (int)jpegd_imageHeight ); 
#else
               (void)sprintf(tstring, "P5\n%d %d\n255\n\0", (int)jpegd_imageWidth, 
                           (int)jpegd_imageHeight);
#endif
             }
             else
             {
#ifdef FEATURE_JPEGD_RESIZE  
#ifdef JPEG_USES_LIBSTD
               (void)std_strlprintf(tstring, sizeof(tstring), "P5\n%d %d\n255\n\0", (int)localjpegdInfo.width, 
                           (int)localjpegdInfo.height);
#else
               (void)sprintf(tstring, "P5\n%d %d\n255\n\0", (int)localjpegdInfo.width, 
                           (int)localjpegdInfo.height);
#endif
#endif
             }
           }
           else
           { /* need to change when YUV output is supported                   */
             jpeg_spec.color_space = JPEG_RGB;
             if (!jpegdResizeFlag)
             {
#ifdef JPEG_USES_LIBSTD
               (void)std_strlprintf(tstring, sizeof(tstring), "P6\n%d %d\n255\n\0", (int)jpegd_imageWidth, 
                           (int)jpegd_imageHeight);
#else
               (void)sprintf(tstring, "P6\n%d %d\n255\n\0", (int)jpegd_imageWidth, 
                           (int)jpegd_imageHeight);
#endif
             }
             else
             {
#ifdef FEATURE_JPEGD_RESIZE              
#ifdef JPEG_USES_LIBSTD
               (void)std_strlprintf(tstring, sizeof(tstring), "P5\n%d %d\n255\n\0", (int)localjpegdInfo.width, 
                           (int)localjpegdInfo.height);
#else
               (void)sprintf(tstring, "P5\n%d %d\n255\n\0", (int)localjpegdInfo.width, 
                           (int)localjpegdInfo.height);
#endif
#endif
             }
           }

#ifdef FEATURE_JPEGD_RESIZE                         
           jpeg_spec.width = localjpegdInfo.width;
           jpeg_spec.height = localjpegdInfo.height;
#endif
           jpeg_spec.sample_factor    = (cmx_jpeg_sample_type)mcu_info.subsample;

           //width and height should be even if o/p is not RGB565
           if( (localjpegdInfo.outFormat != JPEGD_RGB565) && !jpegdResizeFlag)
           {
                jpeg_spec.width  =  ((jpeg_spec.width % 2) == 0)? jpeg_spec.width: (jpeg_spec.width - 1);
                jpeg_spec.height =  ((jpeg_spec.height % 2) == 0)? jpeg_spec.height: (jpeg_spec.height - 1);
           }
           jpeg_cmx_cb (JPEG_CMX_SPEC, jpeg_cmx_handle, (void *) &jpeg_spec);
        }

        if (jpeg_image_specs_flag)
        {
           jpeg_image_specs_flag = FALSE;
           return;
        }

#ifdef FEATURE_JPEG_DECODER_REV2
        if (jpegd_dsp_support() && (!ARM_based))
        {
            int count;
            /* setup the necessary data and pointers */
            /* call the jpegd_6550 */
            if (jpegd_initializeScan() != JPEGD_DECODING_OK)
            {
              jpeg_decode_abort_flag = TRUE;
              MSG_ERROR ("JPEG Decoder: JPEG Decoding Failed", 0, 0, 0);
              
              if ( jpeg_cmx_cb != NULL )
              {
               jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
              }

             MSG_ERROR ("JPEG Decoder: Scan Initialization Failed", 0, 0, 0);
             return;
            }

            jpegd2_info.callBack = jpeg_cmx_cb;
            jpegd2_info.client_handle = jpeg_cmx_handle;
            jpegd2_info.width   = jpegd_imageWidth;
            jpegd2_info.height  = jpegd_imageHeight;
            jpegd2_info.restart = restart_interval;

            if( (localjpegdInfo.outFormat==JPEGD_YCbCr422lp) ||
                (localjpegdInfo.outFormat==JPEGD_YCbCr420lp) )
            {
                jpegd2_info.chromaType  = JPEGDEC_CbCr;
            }
            else
            {
                jpegd2_info.chromaType  = JPEGDEC_CrCb;
            }
            
            if (frameVSamplingFactor[0] == 1)
            {
               jpegd2_info.subsample = JPEGDEC_H2V1;
            }
            else
            {
               jpegd2_info.subsample = JPEGDEC_H2V2;
            }
            
            jpegd2_info.filesize   = jpegd_fileSize;
            jpegd2_info.bytes_read = jpegd_fileSize - data_byte_remain;
             
            /* transform into row major order */
            for (count=0; count<64; count++)
            {
               quant_6550[0][ZAG[count]] = jpegd_quant[frameQuantTableSelector[0]][count];
               quant_6550[1][ZAG[count]] = jpegd_quant[frameQuantTableSelector[1]][count];
            }

            jpegd2_info.q_table[0] = quant_6550[0];
            jpegd2_info.q_table[1] = quant_6550[1];
            
            jpegd2_info.h_codeCount[0] =  dc_derived_table[0].pub->bits;
            jpegd2_info.h_codeCount[1] =  dc_derived_table[1].pub->bits;
            jpegd2_info.h_codeCount[2] =  ac_derived_table[0].pub->bits;
            jpegd2_info.h_codeCount[3] =  ac_derived_table[1].pub->bits;
            jpegd2_info.h_dcCodeValue[0] = dc_derived_table[0].pub->huffval;
            jpegd2_info.h_dcCodeValue[1] =   dc_derived_table[1].pub->huffval;
            jpegd2_info.h_acCodeValue[0] =   ac_derived_table[0].pub->huffval;
            jpegd2_info.h_acCodeValue[1] =  ac_derived_table[1].pub->huffval;

            in_dataPtr = jpegd_dspBstrmPtr;
            total_byte_read = jpegd_totalBytesRead;

            jpegd2_buffer.bufPtr    = in_data_buf;
            jpegd2_buffer.currentPtr= in_dataPtr;
            jpegd2_buffer.bufSize   = 2*MAX_DATA_BYTES;

            /* Amount of data left in buffer is the different between bytes has */
            /* read from and the amount of bytes has been consummed             */
            jpegd2_buffer.dataSize  = jpegd2_info.bytes_read - total_byte_read;
              
            if(jpegd_dsp_dec_rev2((uint32) jpeg_cmx_handle, &jpegd2_info, &jpegd2_buffer) != JPEGDEC_SUCCESS)
            {
              jpeg_decode_abort_flag = TRUE;
              MSG_ERROR ("JPEG Decoder: JPEG Decoding Failed", 0, 0, 0);
              if (jpeg_cmx_cb != NULL)
              {
               jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
              }
            }
           return;
        }
        else
        {
          ARM_based = TRUE;
        }
#endif

        if (jpegd_initializeFrame(&mcu_info) != JPEGD_DECODING_OK)
        {
          jpeg_decode_abort_flag = TRUE;
        }
        
        /* This callback is needed by the Brew Media */
        if (jpeg_cmx_cb != NULL)
        {
          cmx_data.num_bytes = strlen(tstring);
          if (jpeg_output_buf_ptr != NULL) 
          {
            memcpy(jpeg_output_buf_ptr, (uint8 *) tstring, cmx_data.num_bytes);
            cmx_data.buffer     = jpeg_output_buf_ptr;
            jpeg_output_buf_ptr = NULL;
          } 
          else 
          {
            cmx_data.buffer = (uint8 *) tstring;
          }

          jpeg_cmx_cb(JPEG_CMX_DATA, jpeg_cmx_handle, (void *) &cmx_data);
         
          cmx_buf_req.callback_ptr = jpeg_decode_image;
          cmx_buf_req.client_data  = (void *) jpeg_id;
          cmx_buf_req.num_bytes = maxMCUsperRow*maxMCUsperCol*maxMCUrowSize*
                                  maxMCUcolSize*bytesPerPixel;
#ifdef FEATURE_JPEGD_RESIZE          
          if (jpegdResizeFlag)
          {
            cmx_buf_req.num_bytes = localjpegdInfo.width * localjpegdInfo.height * bytesPerPixel;
            MSG_HIGH ("JPEG Decoder: Utilizing ARM Decoding and ARM Scaler", 0, 0, 0);
            MSG_HIGH ("JPEG Decoder: Original Image Dimensions Width: %d Height: %d", jpegd_imageWidth, jpegd_imageHeight, 0);   
            MSG_HIGH ("JPEG Decoder: Expected scaled Dimensions Width: %d Height: %d", localjpegdInfo.resizeImageWidth, localjpegdInfo.resizeImageHeight, 0);   
          }
#endif

          cmx_buf_req.buf_handle = &localRGBPtr;
          localrgbptr = localRGBPtr;
          localRGBPtr         = NULL;
          jpeg_cmx_cb(JPEG_CMX_GET_BUF, jpeg_cmx_handle,
                      (void *) &cmx_buf_req);
        }
        
        jpeg_state = JPEG_BUFFER_RECEIVED;
        break;

      case JPEG_BUFFER_RECEIVED:
        if (localRGBPtr == NULL)
        {
          release_memory();
          if ( jpeg_cmx_cb != NULL )
          {
             jpeg_cmx_cb (JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
          }
           
          MSG_ERROR ("Output buffer is NULL. Decoder aborted", 0, 0, 0);
          return;
        }

        jpegdCurDestYCbCrPtr = localRGBPtr;

        jpegdChromaCurDestYCbCrPtr = jpegdCurDestYCbCrPtr + jpeg_spec.width * jpeg_spec.height;
        current_output_ptr = localRGBPtr;

        
     
        if (jpegd_initializeScan() != JPEGD_DECODING_OK)
        {
          release_memory();
          if (jpeg_cmx_cb != NULL)
          {
            jpeg_cmx_cb(JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
          }
          
          jpeg_decode_abort_flag = TRUE;
          jpeg_state = JPEG_STATE_INIT;
          
          MSG_ERROR ("JPEG decoder error in initiliazing SOS marker",0,0,0) ;
          return;
        }

        readyFlag = TRUE;

        if (ARM_based)
        {
            jpeg_state = JPEG_DECODE_SCAN;
        }
        else
        {
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
           /* Only one output buffer so both have the same buffer pointer */
           configData.RGBBuf1Ptr     = (uint16 *) localRGBPtr; //cmx_data.buffer;
           configData.RGBBuf2Ptr     = (uint16 *) localRGBPtr; //cmx_data.buffer;

           configData.coefBuf1PtrSet[0] = comp_buffer_1[0].seg_ptr;
           configData.coefBuf1PtrSet[1] = comp_buffer_1[1].seg_ptr;
           configData.coefBuf1PtrSet[2] = comp_buffer_1[2].seg_ptr;

           /* check to see if we use ping pong buffer */
           if (comp_buffer_2[0].seg_ptr != NULL)
           {
             configData.coefBuf2PtrSet[0] = comp_buffer_2[0].seg_ptr;
             configData.coefBuf2PtrSet[1] = comp_buffer_2[1].seg_ptr;
             configData.coefBuf2PtrSet[2] = comp_buffer_2[2].seg_ptr;
           }
           else
           {
             configData.coefBuf2PtrSet[0] = comp_buffer_1[0].seg_ptr;
             configData.coefBuf2PtrSet[1] = comp_buffer_1[1].seg_ptr;
             configData.coefBuf2PtrSet[2] = comp_buffer_1[2].seg_ptr;
           }

           configData.lumaQuantTab   = (uint16 *)jpegd_quant[frameQuantTableSelector[0]];
           configData.chromaQuantTab = (uint16 *)jpegd_quant[frameQuantTableSelector[1]];
           configData.imageWidth     = jpegd_imageWidth;
           configData.imageHeight    = (uint16)decode_y_size;

           if (localjpegdInfo.outFormat == JPEGD_RGB565)
              configData.outFormat      = 3;  /* for RGB565 */
           else
              configData.outFormat      = 1;  /* for RGB24 */
        
           configData.samplingFactor      = (uint16)numberOfComponentsInFrame;

           /* At this point, there are sufficient data to config. the DSP decoder */
           jpegd_command(JPGPD_CONFIG_CMD, &configData);
#endif
           break;
        }

      /*lint -fallthrough */ 
      case JPEG_DECODE_SCAN:
        /* huffman decode the first row and the last row */
        if ((total_rows_decoded == 0) ||
            (total_rows_decoded == (mcu_info.per_col - 1)))
        {
          if (jpeg_decode_scan() != JPGD_OKAY)
          {
            release_memory(); 
            jpeg_state = JPEG_STATE_INIT;

            if (jpeg_cmx_cb != NULL)
            {
             jpeg_cmx_cb(JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
            }
          }
          else
          {
            jpeg_state = JPEG_DECODE_SCAN;
            if (jpeg_cmx_cb != NULL)
            {
               jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
            }
          }
          break;
        }
        else if (ARM_based)
        {
           /* the last IDCT decoding session */
           if (total_rows_decoded == (mcu_info.per_col))
           {
              lines_to_decode = decode_y_size;
           }
           else
           {
              lines_to_decode = mcu_rows_per_decode * maxMCUcolSize;
           }
           if ((localjpegdInfo.outFormat == JPEGD_RGB565) || 
              (localjpegdInfo.outFormat == JPEGD_RGB888)) 
           {
             if (jpegdResizeFlag)
             {
#ifdef FEATURE_JPEGD_RESIZE
               (void)jpegd_colorConvert_resize_common();
#endif
             }
             else
             {
               (void)jpegd_colorConvert_RGB();
             }

           }
           else if ((localjpegdInfo.outFormat == JPEGD_YCbCr422lp) ||
                    (localjpegdInfo.outFormat == JPEGD_YCbCr420lp) ||
                    (localjpegdInfo.outFormat == JPEGD_YCrCb422lp) ||
                   (localjpegdInfo.outFormat == JPEGD_YCrCb420lp))
           {
             if(jpegdResizeFlag)
             {
#ifdef FEATURE_JPEGD_RESIZE
               (void)jpegd_colorConvert_resize_common(); 
#endif
             }
             else
             {
               (void)jpegd_colorConvert_YCbCr();
             }
           }
           /* reset pointer to the begining of the buffer */
           for (i = 0; i < numberOfComponentsInFrame; i++)
           {
             comp_data[i].dataUnit_y_mcu = 0;
           }
        }
        else
        {
#ifndef  FEATURE_JPEG_ARM_STANDALONE_DECODER
          decodeData.inputBufIndex = current_comp_index;
          decodeData.startMCU  = 0;
          decodeData.MCU_count = (uint16)(maxMCUsperRow *  mcu_rows_per_decode);
          decodeData.samplingFactor = (int32) mcu_info.subsample;
          decodeData.output1_ptr = current_output_ptr;
          decodeData.output2_ptr = current_output_ptr;

          jpegd_command(JPGPD_DECODE_CMD, &decodeData);
#endif
        }

        /* increment output pointer and swap buffer if we have more rows to decode */   
        if ( total_rows_decoded < mcu_info.per_col )
        {
          current_output_ptr += jpegd_imageWidth * maxMCUcolSize * bytesPerPixel;             

          mcu_rows_decoded = 0;
          DSP_decode = FALSE;

          if (current_comp_index == 0)
          {
            current_comp_index = 1;
            comp_data = comp_buffer_2;
          }
          else
          {
            current_comp_index = 0;
            comp_data = comp_buffer_1;
          }

          /* reset the buffer holding DCT coefficients */
          for (i = 0; i < numberOfComponentsInFrame ; i++)
          {
            memset(comp_data[i].seg_ptr, 0, comp_data[i].coeff_size * sizeof(DATAUNIT_DESC));
          }

          /* huffman decode the next row */
          if (jpeg_decode_scan() != JPGD_OKAY)
          {
            release_memory(); 
            jpeg_state = JPEG_STATE_INIT;
            if (jpeg_cmx_cb != NULL)
            {
               jpeg_cmx_cb(JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
            }            
          }
        }
        
        /* for dsp based, we use isr to trigger the next decode state */ 
        if (ARM_based == TRUE)
        {
          if (total_rows_decoded == mcu_info.per_col)
          {
            jpeg_state = JPEG_IMAGE_DECODE_DONE;
          }
          else
          {
            jpeg_state = JPEG_DECODE_SCAN;
          }

          if (jpeg_cmx_cb != NULL)
          {
             jpeg_cmx_cb(JPEG_DSP_REQUEST, jpeg_cmx_handle, &jpeg_dsp_request);
          }          
        }
       

        break;

      case JPEG_IMAGE_DECODE_DONE:

        JPEG_FLUSH_DATA_CACHE ((uint32*)localRGBPtr,(jpegd_imageWidth * jpegd_imageHeight * bytesPerPixel));

        release_memory();

        MSG_HIGH("JPEG Decoding Done",0,0,0);
        
        if (jpeg_cmx_cb != NULL)
        {
          jpeg_cmx_cb(JPEG_CMX_DONE, jpeg_cmx_handle, NULL);
        }
        
        break;

      default:
        release_memory();
        if (jpeg_cmx_cb != NULL)
        {
          jpeg_cmx_cb(JPEG_CMX_FAILURE, jpeg_cmx_handle, NULL);
        }
        
        MSG_ERROR ("Invalid JPEG state",0,0,0);

    }

} /*lint !e818 cb_data could be declared as pointing to const 
   * but it would involve changing the interface to outside */

/*===========================================================================

FUNCTION   JPEG_DECODE

DESCRIPTION
  This function gets called by client to initiate the JPEG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void jpeg_decode
(
  void                      *handle,
  jpeg_cmx_cb_func_type     callback_ptr,
  jpeg_decodeInfo_type      jpegdInfo
)
{

#ifdef JPEG_TIMING_TEST
  outpw(TIMETEST_PORT, 0xf0);
#endif

  if (handle)
  {
    jpeg_cmx_handle = handle;
    jpeg_cmx_cb     = callback_ptr;
  }
  
  if(jpeg_cancel_decode_flag == TRUE)
  {
    jpeg_cancel_decode_flag = FALSE;

    if ( jpeg_cmx_cb != NULL )
    {
      jpeg_cmx_cb (JPEG_CMX_ABORT, jpeg_cmx_handle, NULL);
    }
    return;
  }
  
  /* if a decode or get specs is in progress then abort it                 */
  if (jpeg_decode_in_progress == TRUE)
  {
    release_memory();
    if ( jpeg_cmx_cb != NULL )
    {
      jpeg_cmx_cb (JPEG_CMX_ABORT, jpeg_cmx_handle, NULL);
    }
    /* A callback was not issued since it will cause the abortion of */
    /* the new decoding session */
    MSG_HIGH("Abort the decoding image, start new session", 0, 0, 0);
  }

  jpeg_id++;

  if (handle)
  {
     isSyncCb  = jpegdInfo.isSync;
     
      
     ARM_based = jpegdInfo.arm_based;
     jpeg_decode_in_progress = TRUE;        /* set jpeg decode in progress */

     jpeg_decode_abort_flag = FALSE;        /* reset abort flag            */
     jpeg_image_specs_flag = FALSE;         /* reset specs flag            */

     jpeg_state = JPEG_STATE_INIT;          /* set JPEG to init state      */

#if defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

     /* Request to operate in hyper mode */
     clk_regime_register_for_cpu_resource(CLKRGM_JPEG_APPS);

#endif

#ifdef FEATURE_NEW_SLEEP_API
     sleep_negate_okts(jpeg_sleep_okts_handle);
#endif

     localjpegdInfo = jpegdInfo;
     


#ifdef FEATURE_JPEG_DECODER_REV2
    jpegd2_info.outFormat = (JPEGDEC_outFormatType)jpegdInfo.outFormat;

#ifdef FEATURE_JPEGD_RESIZE
    jpegd2_info.resizeImageWidth = localjpegdInfo.resizeImageWidth;
    jpegd2_info.resizeImageHeight = localjpegdInfo.resizeImageHeight;
    jpegd2_info.aspectRatioFlag = localjpegdInfo.aspectRatioFlag; 
    jpegd2_info.jpegdResizeFlag = localjpegdInfo.jpegdResizeFlag; 
#endif

#endif

     MSG_HIGH ("Starting JPEG Decoding", 0, 0, 0);

     jpeg_decode_image((void *) jpeg_id);   /* call JPEG decoder           */

  }

}

/*===========================================================================

FUNCTION   JPEG_ARM_BASED_DECODE

DESCRIPTION
  This function gets called by client to initiate the JPEG decoder without
  using the DSP resource.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void jpeg_ARM_based_decode
(
  void                      *handle,
  jpeg_cmx_cb_func_type     callback_ptr,
  jpeg_decodeInfo_type      jpegdInfo
)
{
    isSyncCb  = jpegdInfo.isSync; 
    graph_jpeg_decode(handle, callback_ptr, jpegdInfo);
}

/*===========================================================================

FUNCTION   JPEG_GET_IMAGE_SPECS

DESCRIPTION
  This function reads the header data from a JPEG image and passes back
  image specifications to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void jpeg_get_image_specs
(
void                          *handle,
jpeg_cmx_cb_func_type         callback_ptr
)
{
  /* if a decode or get specs is in progress then abort it                 */
  if ((jpeg_decode_in_progress == TRUE) && (jpeg_decode_abort_flag == FALSE))
  {
    MSG_ERROR("JPEGD is in progress, not accept get image specs", 0, 0, 0);
    return;
  }

  jpeg_id++;

  jpeg_decode_in_progress = TRUE;           /* set jpeg decode in progress */

  jpeg_decode_abort_flag = FALSE;           /* reset abort flag            */
  jpeg_image_specs_flag = TRUE;             /* set flag to get specs       */

  jpeg_cmx_handle = handle;
  jpeg_cmx_cb = callback_ptr;

  jpeg_state = JPEG_STATE_INIT;             /* set JPEG to init state      */

  MSG_HIGH ("Starting JPEG Decoding to get spec.", 0, 0, 0);

  jpeg_decode_image((void *) jpeg_id);      /* call JPEG decoder           */
}


/*===========================================================================

FUNCTION   JPEG_DECODE_CONTINUE

DESCRIPTION This function is needed for CMX compatible for previous decoder
  This function gets called when decode continues from QDSP image ready signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void jpeg_decode_continue()
{
  MSG_HIGH(" CONTINUE !!!", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_DECODE_IMAGE_ABORT

DESCRIPTION
  This function is called when the JPEG decode image has been aborted because
  of a QDSP image reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void jpeg_decode_image_abort()
{
  /* Set abort flag so decoder can abort it at the proper state */
   jpeg_decode_abort_flag = TRUE;
   jpeg_cancel_decode_flag = FALSE;
   MSG_HIGH("jpeg decode image abort", 0, 0, 0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_CANCEL_DECODE

DESCRIPTION

  Cancel JPEG decoding

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void jpeg_cancel_decode()
 {
  jpeg_cancel_decode_flag = TRUE;
  jpeg_decode_abort_flag = TRUE;
  
#ifdef FEATURE_JPEG_DECODER_REV2
  jpeg_decode_abort = TRUE;
#endif
 }

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_ABORT_DECODE

DESCRIPTION
  Abort function for the JPEG decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void jpeg_abort_decode ()
{
  jpeg_decode_abort_flag = TRUE;               /* set decode abort flag    */
  MSG_HIGH("jpeg abort decode", 0, 0, 0);
}



/*===========================================================================

FUNCTION   IDCT

DESCRIPTION
  Perform invert DCT to the 8x8 block
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e704 Operations in this function rely on right-shifting of signed values to work */
void idct(DATAUNIT_DESC *data, uint8 *Pdst_ptr)
{
  int32 tmp0, tmp1, tmp2, tmp3;
  int32 tmp10, tmp11, tmp12, tmp13;
  int32 z1, z2, z3, z4, z5;
  register DATAUNIT_DESC *dataptr;
  int32 rowctr;
  int16 dcval;
  int32 i;

  dataptr = data;

  if (dataptr == NULL)
   return;

  for (rowctr = 8-1; rowctr >= 0; rowctr--)
  {
    if ((dataptr[1] | dataptr[2] | dataptr[3] | dataptr[4] |
         dataptr[5] | dataptr[6] | dataptr[7]) == 0)
    {
      dcval = (int16) ((uint16)dataptr[0] << PASS1_BITS);

      dataptr[0] = dcval;
      dataptr[1] = dcval;
      dataptr[2] = dcval;
      dataptr[3] = dcval;
      dataptr[4] = dcval;
      dataptr[5] = dcval;
      dataptr[6] = dcval;
      dataptr[7] = dcval;

      dataptr += 8;       /* advance pointer to next row */
      continue;
    }

    z2 = (int32) dataptr[2];
    z3 = (int32) dataptr[6];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
    tmp2 = z1 + MULTIPLY(z3, - FIX_1_847759065);
    tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);

    tmp0 = (int32)((uint32)((int32) dataptr[0] + (int32) dataptr[4]) << CONST_BITS);
    tmp1 = (int32)((uint32)((int32) dataptr[0] - (int32) dataptr[4]) << CONST_BITS);

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    tmp0 = (int32) dataptr[7];
    tmp1 = (int32) dataptr[5];
    tmp2 = (int32) dataptr[3];
    tmp3 = (int32) dataptr[1];

    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602);

    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);
    z1 = MULTIPLY(z1, - FIX_0_899976223);
    z2 = MULTIPLY(z2, - FIX_2_562915447);
    z3 = MULTIPLY(z3, - FIX_1_961570560);
    z4 = MULTIPLY(z4, - FIX_0_390180644);

    z3 += z5;
    z4 += z5;

    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;

    dataptr[0] = (int16) DESCALE(tmp10 + tmp3, (CONST_BITS-PASS1_BITS));
    dataptr[7] = (int16) DESCALE(tmp10 - tmp3, (CONST_BITS-PASS1_BITS));
    dataptr[1] = (int16) DESCALE(tmp11 + tmp2, (CONST_BITS-PASS1_BITS));
    dataptr[6] = (int16) DESCALE(tmp11 - tmp2, (CONST_BITS-PASS1_BITS));
    dataptr[2] = (int16) DESCALE(tmp12 + tmp1, (CONST_BITS-PASS1_BITS));
    dataptr[5] = (int16) DESCALE(tmp12 - tmp1, (CONST_BITS-PASS1_BITS));
    dataptr[3] = (int16) DESCALE(tmp13 + tmp0, (CONST_BITS-PASS1_BITS));
    dataptr[4] = (int16) DESCALE(tmp13 - tmp0, (CONST_BITS-PASS1_BITS));

    dataptr += 8;
  }

  dataptr = data;
  for (rowctr = 8-1; rowctr >= 0; rowctr--)
  {

    if ((dataptr[8*1] | dataptr[8*2] | dataptr[8*3] |
         dataptr[8*4] | dataptr[8*5] | dataptr[8*6] |
         dataptr[8*7]) == 0)
    {
      dcval = (int16) DESCALE((int32) dataptr[0], (PASS1_BITS+3));

      if ((dcval += 128) < 0)
        dcval = 0;
      else if (dcval > 255)
        dcval = 255;

      Pdst_ptr[8*0] = (uint8)dcval;
      Pdst_ptr[8*1] = (uint8)dcval;
      Pdst_ptr[8*2] = (uint8)dcval;
      Pdst_ptr[8*3] = (uint8)dcval;
      Pdst_ptr[8*4] = (uint8)dcval;
      Pdst_ptr[8*5] = (uint8)dcval;
      Pdst_ptr[8*6] = (uint8)dcval;
      Pdst_ptr[8*7] = (uint8)dcval;

      dataptr++;
      Pdst_ptr++;
      continue;
    }

    z2 = (int32) dataptr[8*2];
    z3 = (int32) dataptr[8*6];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);
    tmp2 = z1 + MULTIPLY(z3, - FIX_1_847759065);
    tmp3 = z1 + MULTIPLY(z2, FIX_0_765366865);

    tmp0 = (int32)((uint32)((int32) dataptr[8*0] + (int32) dataptr[8*4]) << CONST_BITS);
    tmp1 = (int32)((uint32)((int32) dataptr[8*0] - (int32) dataptr[8*4]) << CONST_BITS);

    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    tmp0 = (int32) dataptr[8*7];
    tmp1 = (int32) dataptr[8*5];
    tmp2 = (int32) dataptr[8*3];
    tmp3 = (int32) dataptr[8*1];

    z1 = tmp0 + tmp3;
    z2 = tmp1 + tmp2;
    z3 = tmp0 + tmp2;
    z4 = tmp1 + tmp3;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602);

    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);
    z1 = MULTIPLY(z1, - FIX_0_899976223);
    z2 = MULTIPLY(z2, - FIX_2_562915447);
    z3 = MULTIPLY(z3, - FIX_1_961570560);
    z4 = MULTIPLY(z4, - FIX_0_390180644);

    z3 += z5;
    z4 += z5;

    tmp0 += z1 + z3;
    tmp1 += z2 + z4;
    tmp2 += z2 + z3;
    tmp3 += z1 + z4;

    i = (int32) DESCALE(tmp10 + tmp3, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*0] = clamp(i);

    i = (int32) DESCALE(tmp10 - tmp3, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*7] = clamp(i);

    i = (int32) DESCALE(tmp11 + tmp2, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*1] = clamp(i);

    i = (int32) DESCALE(tmp11 - tmp2, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*6] = clamp(i);

    i = (int32) DESCALE(tmp12 + tmp1, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*2] = clamp(i);

    i = (int32) DESCALE(tmp12 - tmp1, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*5] = clamp(i);

    i = (int32) DESCALE(tmp13 + tmp0, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*3] = clamp(i);

    i = (int32) DESCALE(tmp13 - tmp0, (CONST_BITS+PASS1_BITS+3)) + 128;
    Pdst_ptr[8*4] = clamp(i);

    dataptr++;
    Pdst_ptr++;
  }
}
/*lint -restore*/


/*===========================================================================

FUNCTION   JPEGD_COLORCONVERT_RGB()

DESCRIPTION
  Color Conversion from YCbCr MCU Blocks to RGB 565/888
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RGB565 Ideal for image display on LCD,but results in image 
  quality getting degraded if done further processing.

===========================================================================*/

jpeg_return_type jpegd_colorConvert_RGB()
{
  int32 lines_dec = 0;
  static ipl_rect_type copy_area, paste_area;
  boolean isLastRow = FALSE, lastRow = FALSE;
  copy_area.x = 0;
  copy_area.y = 0;
  
  copy_area.dx = jpegd_imageWidth;
  copy_area.dy = lines_to_decode;
  
  paste_area.x = 0;
  paste_area.y = 0;
  
  paste_area.dx = jpegd_imageWidth;
  paste_area.dy = lines_to_decode;
  
  
  jpegdIPLIn.dy = lines_to_decode;

  jpegdIPLOut.dy = lines_to_decode;
  jpegdIPLOut.dx = jpegd_imageWidth;
  
  if (localjpegdInfo.outFormat == JPEGD_RGB888)
  {
    jpegdIPLOut.cFormat = IPL_RGB888;
  }
  else
  {
    jpegdIPLOut.cFormat = IPL_RGB565;
  }
  
  lines_dec = (int32)lines_to_decode;

  if (jpegd_imageHeight < 16)
  {
     if( (mcu_info.subsample == JPEG_H2V2) || (mcu_info.subsample == JPEG_H1V2))
       jpegdIPLIn.dy = (((uint32)lines_dec + 15) / 16) * 16;
     else
       jpegdIPLIn.dy = (((uint32)lines_dec + 7) / 8) * 8;  
  }
 
  while( lines_dec > 0)
  {
    if(isLastRow)
    {
      copy_area.dy = (uint32)lines_dec;
      paste_area.dy = (uint32)lines_dec;
      
      if( (mcu_info.subsample == JPEG_H2V2) || (mcu_info.subsample == JPEG_H1V2))
        jpegdIPLIn.dy = (((uint32)lines_dec + 15) / 16) * 16;
      else
        jpegdIPLIn.dy = (((uint32)lines_dec + 7) / 8) * 8;  

      jpegdIPLOut.dy = (uint32)lines_dec;
      lastRow = TRUE;
     }
    
    if ((uint32)lines_dec > maxMCUcolSize)
    {
      copy_area.dy = maxMCUcolSize;
      paste_area.dy = maxMCUcolSize;
      jpegdIPLIn.dy = maxMCUcolSize;
      jpegdIPLOut.dy = maxMCUcolSize;
    }
    
    
    jpegd_loadDecodeNextRow();
    
    
    jpegdIPLIn.imgPtr = imageOut;
    jpegdIPLOut.imgPtr = localRGBPtr;
    
    switch (mcu_info.subsample)
    {
      
     case JPEG_H2V2:
      
      jpegdIPLIn.cFormat = IPL_H2V2MCU_CbCr;
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      

      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V2MCU Block to RGB565", 0, 0, 0);
      }
      
      break;
      
    case JPEG_H2V1:
      
      jpegdIPLIn.cFormat = IPL_H2V1MCU_CbCr;
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      
     
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V1MCU Block to RGB565", 0, 0, 0);
      }
      
      break;
      
      
    case JPEG_H1V2:

      jpegdIPLIn.cFormat = IPL_H1V2MCU_CbCr;
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V2MCU Block to RGB565", 0, 0, 0);
      }
      
      break;
      
    case JPEG_H1V1:
      
      jpegdIPLIn.cFormat = IPL_H1V1MCU_CbCr;
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V1MCU Block to RGB565", 0, 0, 0);
      }
      
      break;
      
    case JPEGD_GRAY:

      jpegdIPLIn.cFormat = IPL_MCU_GRAY;
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      

      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrMCU Block to GrayScale", 0, 0, 0);
      }
      
      break;
      
    default:
      
      break;
    }
    
    if (localjpegdInfo.outFormat == JPEGD_RGB888)
    {
      if(!lastRow)
        localRGBPtr += jpegd_imageWidth * maxMCUcolSize * 3;
    }
    else
    {
      if(!lastRow)
        localRGBPtr += jpegd_imageWidth * maxMCUcolSize * 2;
    }
    
    lines_dec = lines_dec - (int32)maxMCUcolSize;
    
    if( lines_dec  < (int32)maxMCUcolSize )
    {
      isLastRow = TRUE;
    } 
  }
    
    
  isLastRow = FALSE;
  lastRow = FALSE;
  return JPGD_OKAY;        
}



/*===========================================================================

FUNCTION   JPEGD_COLORCONVERT_YCBCR

DESCRIPTION
  Color conversion from YCbCr MCU Block to YCbCr 422 LP or YCbCr 420 LP
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
jpeg_return_type jpegd_colorConvert_YCbCr()
{
  int32 lines_dec = 0;
  static ipl_rect_type copy_area, paste_area;
  boolean isLastRow = FALSE;
  copy_area.x = 0;
  copy_area.y = 0;
  
  copy_area.dx = jpeg_spec.width;
  copy_area.dy = lines_to_decode;
  
  paste_area.x = 0;
  paste_area.y = 0;
  
  paste_area.dx = jpeg_spec.width;
  paste_area.dy = lines_to_decode;
  
  
  jpegdIPLIn.dy = lines_to_decode;
  
  
  jpegdIPLOut.dx = jpeg_spec.width;
  jpegdIPLOut.dy = lines_to_decode;
  
  lines_dec = (int32)lines_to_decode;
   
  
  if (localjpegdInfo.outFormat == JPEGD_YCbCr422lp)
    jpegdIPLOut.cFormat = IPL_YCbCr422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCbCr420lp)
    jpegdIPLOut.cFormat = IPL_YCbCr420_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb422lp)
    jpegdIPLOut.cFormat = IPL_YCrCb422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb420lp)
    jpegdIPLOut.cFormat = IPL_YCrCb420_LINE_PK;

  
  while( lines_dec > 0)
  {
    if(isLastRow)
    {
      lines_dec = ((lines_dec % 2) == 0)? lines_dec: (lines_dec - 1);
      copy_area.dy = (uint32)lines_dec;
      paste_area.dy = (uint32)lines_dec;
      jpegdIPLIn.dy = (uint32)lines_dec;
      jpegdIPLOut.dy = (uint32)lines_dec;
      isLastRow = FALSE;
    }
    
    if ((uint32)lines_dec > maxMCUcolSize)
    {
      copy_area.dy = maxMCUcolSize;
      paste_area.dy = maxMCUcolSize;
      jpegdIPLIn.dy = maxMCUcolSize;
      jpegdIPLOut.dy = maxMCUcolSize;
      
      isLastRow = TRUE;
    }
    
    jpegd_loadDecodeNextRow();
    
    jpegdIPLIn.imgPtr = imageOut;
    jpegdIPLOut.imgPtr = jpegdCurDestYCbCrPtr;
    jpegdIPLOut.clrPtr  =  jpegdChromaCurDestYCbCrPtr;
    
    
    switch (mcu_info.subsample)
    {
      
    case JPEG_H2V2:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      jpegdIPLIn.cFormat = IPL_H2V2MCU_CbCr;
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V2MCU Block to YCbCrLp", 0, 0, 0);
      } 
      
      break;
      
    case JPEG_H2V1:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      jpegdIPLIn.cFormat = IPL_H2V1MCU_CbCr;
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V1MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
      
    case JPEG_H1V2:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_H1V2MCU_CbCr;
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V2MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
    case JPEG_H1V1:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_H1V1MCU_CbCr;
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V1MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
    case JPEGD_GRAY:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_MCU_GRAY;
      
      if ((ipl_copy_and_paste(&jpegdIPLIn, &jpegdIPLOut,&copy_area, &paste_area))!= IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V1MCU Block to GrayScale", 0, 0, 0);
      }
      
      break;
      
    default:
      break;
    } 
    
    jpegdCurDestYCbCrPtr = jpegdCurDestYCbCrPtr + jpeg_spec.width * maxMCUcolSize;
    
    if ( (localjpegdInfo.outFormat == JPEGD_YCbCr422lp) || (localjpegdInfo.outFormat == JPEGD_YCrCb422lp) )
      jpegdChromaCurDestYCbCrPtr = jpegdChromaCurDestYCbCrPtr + jpeg_spec.width * maxMCUcolSize;
    else if( (localjpegdInfo.outFormat == JPEGD_YCbCr420lp) || (localjpegdInfo.outFormat == JPEGD_YCrCb420lp) )
      jpegdChromaCurDestYCbCrPtr = jpegdChromaCurDestYCbCrPtr + (jpeg_spec.width * maxMCUcolSize)/2;
    else
      MSG_ERROR("JPEGD: JPEG output format not set correctly ", 0, 0, 0);
    
    lines_dec = lines_dec - (int32)maxMCUcolSize;
  }
  return JPGD_OKAY;        
}


/*===========================================================================

Function: jpegd_parse_thumbnail_dimensions

Description: This function parses the thumbnail header to extract the width and height

Return Value:
              None
Comments: 

Side Effects: 

==============================================================================*/
void jpegd_parse_thumbnail_dimensions (exif_info_type *info)
{
  
  int32 m1;
  in_dataPtr      = info->thumbnail_stream;
  total_byte_read = 0;
  jpegd_fileSize  = info->thumbnail_stream_length;

  byteOffset    = 0;
  bitPack       = 0;
  bitsRemaining = 0;

  for(;;)
  {
    if (( m1 = jpegd_parseMarkers() ) < 0)
    {
      jpegd_statusMsg = JPEGD_ERROR_INTERPRETING_MARKER ;
      MSG_ERROR ("JPEG Decoder: Error Interpreting Marker", 0, 0, 0);
      return;
    }

    if(m1 == M_SOF0)
    {
      if ((jpegd_interpretSOFMarker()) != JPEGD_DECODING_OK)
      {
        jpegd_statusMsg = JPEGD_INVALID_SOF_MARKER ;

      }
      info->thumbnail_height  = jpegd_imageHeight;
      info->thumbnail_width   = jpegd_imageWidth;
      return;
    }

  }
}

/*===========================================================================

Function: jpegd_parse_main_image_dimensions

Description: This function parses the main image width and height

Return Value:
              None
Comments: 

Side Effects: 

==============================================================================*/
void jpegd_parse_main_image_dimensions (exif_info_type *info)
{
  
  int32 m1;

  if(info->thumbnail_present)
  {
    in_dataPtr      = info->thumbnail_stream + info->thumbnail_stream_length;
  }
  else
  {
    in_dataPtr      = info->pointer;
  }
  total_byte_read = 0;
  jpegd_fileSize  = info->stream_length - (in_dataPtr - info->stream);
  
  byteOffset    = 0;
  bitPack       = 0;
  bitsRemaining = 0;

  for(;;)
  {
    if (( m1 = jpegd_parseMarkers() ) < 0)
    {
      jpegd_statusMsg = JPEGD_ERROR_INTERPRETING_MARKER ;
      MSG_ERROR ("JPEG Decoder: Error Interpreting Marker", 0, 0, 0);
      return;
    }

    if ((m1 == M_SOF0) || (m1 == M_SOF2))
    {
      if ((jpegd_interpretSOFMarker()) != JPEGD_DECODING_OK)
      {
        jpegd_statusMsg = JPEGD_INVALID_SOF_MARKER ;

      }
      info->image_width    = jpegd_imageWidth;
      info->image_height   = jpegd_imageHeight;
      return;
    }

  }
}

#ifdef FEATURE_JPEGD_RESIZE
/*===========================================================================

FUNCTION   JPEGD_COLORCONVERT_RESIZE_COMMON

DESCRIPTION
  Color conversion and resize from YCbCr MCU Block to YCbCr 422 LP or YCbCr 420 LP
  or RGB565 or RGB888
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
jpeg_return_type jpegd_colorConvert_resize_common()
{
  int32 lines_dec = 0;
  static ipl_rect_type cropIn;
  uint8  *imageOutBckup;
  
  boolean isLastRow = FALSE;
  boolean lastRow = FALSE;
    
  jpegdIPLOut.dx = jpegd_imageWidth;
  jpegdIPLOut.dy = lines_to_decode;
  
  cropIn.x = 0;
  cropIn.dx = jpegd_imageWidthEven;
  cropIn.y  = 0;
  cropIn.dy = lines_to_decode;

  lines_dec = (int32)lines_to_decode;
   
  if(localjpegdInfo.outFormat == JPEGD_RGB565)
    jpegdIPLOut.cFormat = IPL_RGB565;
  else if (localjpegdInfo.outFormat == JPEGD_YCbCr422lp)
    jpegdIPLOut.cFormat = IPL_YCbCr422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCbCr420lp)
    jpegdIPLOut.cFormat = IPL_YCbCr420_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_RGB888)
    jpegdIPLOut.cFormat = IPL_RGB888;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb422lp)
    jpegdIPLOut.cFormat = IPL_YCrCb422_LINE_PK;
  else if(localjpegdInfo.outFormat == JPEGD_YCrCb420lp)
    jpegdIPLOut.cFormat = IPL_YCrCb420_LINE_PK;
   

  jpegdIPLIn.dy  =  maxMCUcolSize;

  while( lines_dec > 0)
  {
    if (resizeNow == FALSE)
    {
        jpegd_loadDecodeNextRow();

        if(lines_dec < maxMCUcolSize)
        {
            colSizeToScale = lines_dec;
        }
        else
        {
            colSizeToScale = maxMCUcolSize;
        }
        lines_dec      = lines_dec - maxMCUcolSize;

        if( (mcu_info.subsample == JPEG_H1V1 || mcu_info.subsample == JPEG_H2V1))
        {
            /*This condition indicates last slice of image*/
            if ((lines_dec <= 0) && ((total_rows_decoded*maxMCUcolSize) < jpegd_localImageHeight) && !prog_flag)
            {
                resizeNow = TRUE;
                return JPGD_OKAY;
            }
        }
    }

    if( (mcu_info.subsample == JPEG_H1V1 || mcu_info.subsample == JPEG_H2V1) && lines_dec > 0)
    {
        imageOutBckup = imageOut;
        imageOut     += total_coeff_size * JPGD_BLOCKSIZE;
    
        jpegd_loadDecodeNextRow();
        
        resizeNow   = FALSE;
        imageOut    = imageOutBckup;

        if(lines_dec < maxMCUcolSize)
        {
            colSizeToScale += lines_dec;
        }
        else
        {
            colSizeToScale  = maxMCUcolSize  * 2;
        }
        lines_dec      = lines_dec - maxMCUcolSize;
        jpegdIPLIn.dy  =  maxMCUcolSize*2;
    }

    if (localjpegdInfo.outFormat == JPEGD_YCbCr420lp)
    { 
      jpegd_resizeCol = (int)((((localjpegdInfo.resizeImageHeight - jpegd_resizeColCum) * colSizeToScale) / ((float)(jpegd_localImageHeight - jpegd_pixRow))) + 0.5);
      jpegd_resizeCol = ((jpegd_resizeCol % 2) == 0)? jpegd_resizeCol: (jpegd_resizeCol - 1);
    }
    else
    {
      jpegd_resizeCol = (int)((((localjpegdInfo.resizeImageHeight - jpegd_resizeColCum) * colSizeToScale) / ((float)(jpegd_localImageHeight- jpegd_pixRow))) + 0.5); 
    }

    if(mcu_info.subsample == JPEG_H1V1 || mcu_info.subsample == JPEG_H2V1)
    {
        jpegd_pixRow    = jpegd_pixRow    + maxMCUcolSize;
    }

    jpegdIPLOut.dx = localjpegdInfo.resizeImageWidth;
    jpegdIPLOut.dy = jpegd_resizeCol;
    cropIn.dy      = colSizeToScale;   

    if(isLastRow)
    {
      jpegdIPLOut.dy =  localjpegdInfo.resizeImageHeight - jpegd_resizeColCum;
      isLastRow = FALSE;
      lastRow = TRUE;
    }
  
    jpegd_resizeColCum =  jpegd_resizeColCum +  jpegd_resizeCol;
    jpegd_pixRow = jpegd_pixRow + maxMCUcolSize;
    
    jpegdIPLIn.imgPtr = imageOut;
     
    if ((localjpegdInfo.outFormat == JPEGD_RGB565) || (localjpegdInfo.outFormat == JPEGD_RGB888))
    {
      jpegdIPLOut.imgPtr = localRGBPtr;
    }
    else
    {
      jpegdIPLOut.imgPtr = jpegdCurDestYCbCrPtr;
      jpegdIPLOut.clrPtr =  jpegdChromaCurDestYCbCrPtr;
    }
        
    switch (mcu_info.subsample)
    {
      
    case JPEG_H2V2:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      jpegdIPLIn.cFormat = IPL_H2V2MCU_CbCr;
      
      if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )

      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V2MCU Block to YCbCrLp", 0, 0, 0);
      } 
      
      break;
      
    case JPEG_H2V1:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 15) / 16) * 16;
      jpegdIPLIn.cFormat = IPL_H2V1MCU_CbCr;
      
      if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH2V1MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
      
    case JPEG_H1V2:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_H1V2MCU_CbCr;
      
      if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V2MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
    case JPEG_H1V1:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_H1V1MCU_CbCr;
      
      if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V1MCU Block to YCbCrLp", 0, 0, 0);
      }
      
      break;
      
    case JPEGD_GRAY:
      
      jpegdIPLIn.dx = ((jpegd_imageWidth + 7) / 8) * 8;
      jpegdIPLIn.cFormat = IPL_MCU_GRAY;
      
      if (ipl_crop_resize_rot(&jpegdIPLIn, &jpegdIPLOut, &cropIn, NULL, IPL_NOROT, IPL_QUALITY_MAX) != IPL_SUCCESS )
      {
        MSG_ERROR("JPEGD: Failed to convert YCbCrH1V1MCU Block to GrayScale", 0, 0, 0);
      }
      
      break;
      
    default:
      MSG_ERROR("JPEGD: Unkown Subsampling Format", 0, 0, 0);
      break;
    } 
    
    
    if (localjpegdInfo.outFormat == JPEGD_RGB565)
    {
      if(!lastRow)
        localRGBPtr += localjpegdInfo.width * jpegd_resizeCol * 2;
    }
    else if ( (localjpegdInfo.outFormat == JPEGD_YCbCr422lp) || (localjpegdInfo.outFormat == JPEGD_YCrCb422lp) )
    {
      jpegdCurDestYCbCrPtr = jpegdCurDestYCbCrPtr + localjpegdInfo.width * jpegd_resizeCol;
      jpegdChromaCurDestYCbCrPtr = jpegdChromaCurDestYCbCrPtr + localjpegdInfo.width * jpegd_resizeCol;
    }
    else if( (localjpegdInfo.outFormat == JPEGD_YCbCr420lp) || (localjpegdInfo.outFormat == JPEGD_YCrCb420lp) )
    {
      jpegdCurDestYCbCrPtr = jpegdCurDestYCbCrPtr + localjpegdInfo.width * jpegd_resizeCol;
      jpegdChromaCurDestYCbCrPtr = jpegdChromaCurDestYCbCrPtr + (localjpegdInfo.width * jpegd_resizeCol)/2;
    }
    else if (localjpegdInfo.outFormat == JPEGD_RGB888)
    {
      if(!lastRow)
        localRGBPtr += localjpegdInfo.width * jpegd_resizeCol * 3;
    }
      
    if( lines_dec  < (int32)maxMCUcolSize )
    {
      isLastRow = TRUE;
    }
  }

  isLastRow = FALSE;
  lastRow = FALSE;
  return JPGD_OKAY;        
}
#endif


/*===========================================================================

FUNCTION: JPEG_THIN_INTERFACE

DESCRIPTION
   Callback function called by the JPEG decoder to provide status, info

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void jpeg_thin_interface(jpeg_cmx_status_enum_type command,
                                void                      *handle,
                                void                      *decoder_data ) 
{
  
   switch (command)
   {
      case JPEG_CMX_GET_FILE_SIZE:
      {
         uint32 *temp = (uint32 *) decoder_data;
         *temp = client_input_info.file_size;
      }
      break;
      
      case  JPEG_CMX_GET_BUF:
      {
         jpeg_cmx_buf_req_type *data = (jpeg_cmx_buf_req_type *)decoder_data;

         
         if (client_input_info.display_ptr != NULL) 
         {  
           *(data->buf_handle) = client_input_info.display_ptr;
         }
         else
         {
           client_cb_func(JPEG_DECODE_GET_BUF, output_info_ptr);

           if (output_info_ptr->dstPtr)
           {
             *(data->buf_handle) = output_info_ptr->dstPtr; 
           }
         }
         
         if (data->callback_ptr != NULL)    
         {
           data->callback_ptr(data->client_data);
         }
      }
      break;
      
      case  JPEG_CMX_GET_DATA:
      {
         jpeg_cmx_get_data_type *data = (jpeg_cmx_get_data_type *) decoder_data;

         memcpy((void *) data->buffer, 
                (void *) (client_input_info.image_data_ptr + data->position),
                (MIN(data->position + data->num_bytes, client_input_info.file_size) - data->position)
               );
         if (data->callback_ptr != NULL)    //Changes
         {
           data->callback_ptr(data->client_data);
         }
      }
      break;

      case  JPEG_CMX_SPEC:  
      {
         // Just read the parameters back
         jpeg_cmx_spec_type *buf = (jpeg_cmx_spec_type*) decoder_data;
              
         output_info_ptr->image_width  = (uint16)buf->width;
         output_info_ptr->image_height = (uint16)buf->height;

#ifdef FEATURE_JPEGD_RESIZE     
         if (jpegdResizeFlag)    
         {
           output_info_ptr->image_width  = localjpegdInfo.width;
           output_info_ptr->image_height = localjpegdInfo.height;
         }    
#endif         
         if (client_input_info.display_ptr != NULL)
         {
           /* Check for RGB output buffer size  */ 
           if (localjpegdInfo.outFormat == JPEGD_RGB565)
           {
             if (client_input_info.disp_buf_size < buf->width*buf->height*2)
             {
               jpeg_decode_abort_flag = TRUE;
             }
           }
           else  /* must be RGB24 output format */
           {
             if (client_input_info.disp_buf_size < buf->width*buf->height*3)
             {
               jpeg_decode_abort_flag = TRUE;
             }
           }
         }
      }

     
      break;    
      
      case JPEG_CMX_DONE:
      case JPEG_CMX_ABORT:
      case JPEG_CMX_FAILURE:
      case JPEG_CMX_INVALID:
      {
      
           if (client_cb_func != NULL)
           {
             if (command == JPEG_CMX_ABORT || command == JPEG_CMX_FAILURE || 
                 command == JPEG_CMX_INVALID)
             {
                client_cb_func(JPEG_DECODE_ABORT, output_info_ptr);
             }
             else
             {
                client_cb_func(JPEG_DECODE_DONE, output_info_ptr);
             }
           }
      }
      break;  
      
      default:
           MSG_LOW("Received but not process state", command,0,0);
         
      }

}/*lint !e715 !e818: handle could be declared as pointing to const 
  * but it would involve changing the interface to outside */

/*===========================================================================

FUNCTION JPEG_DECODE_THIN_LAYER

DESCRIPTION
  This function performs as a thin interface to both JPEG ARM based 
  and DSP based decoder.
  
DEPENDENCIES
  None

RETURN VALUE
  jpeg_decode_output_type structure

SIDE EFFECTS
  None.

===========================================================================*/
void jpeg_decode_thin_layer
(
const jpeg_decode_input_type      *in_decode_ptr,
jpeg_cb_func_type           callback_ptr,
jpeg_decode_output_type     *out_decode_ptr,   /* inform client about image*/
boolean                     dsp_engine         /* True, use DSP decoder */
)
{
   jpeg_decodeInfo_type      jpegdInfo;
   client_input_info  = *in_decode_ptr;
   output_info_ptr    = out_decode_ptr;
   client_cb_func     = callback_ptr;
#ifdef FEATURE_JPEGD_RESIZE   
   jpegdInfo.resizeImageWidth = in_decode_ptr->resizeImageWidth;
   jpegdInfo.resizeImageHeight = in_decode_ptr->resizeImageHeight;
   jpegdInfo.aspectRatioFlag = in_decode_ptr->aspectRatioFlag;
   jpegdInfo.jpegdResizeFlag = in_decode_ptr->jpegdResizeFlag;
#endif

   if (client_input_info.thumbnail)
   {
     if (jpeg_exifread(&JPEG_exif_info, client_input_info.image_data_ptr, client_input_info.file_size))
     {
       if (JPEG_exif_info.thumbnail_present)
       {
         client_input_info.image_data_ptr = JPEG_exif_info.thumbnail_stream;
         client_input_info.file_size = JPEG_exif_info.thumbnail_stream_length;
       }
     }
   }
   if (dsp_engine)
   {
     jpegdInfo.isSync = TRUE;
     jpegdInfo.outFormat = (jpeg_outputFormat_type)client_input_info.output_format;
     jpegdInfo.arm_based = FALSE; 
     graph_jpeg_decode(&client_input_info, jpeg_thin_interface, jpegdInfo);
   }
   else
   {
     jpegdInfo.isSync = TRUE;
     jpegdInfo.outFormat = (jpeg_outputFormat_type)client_input_info.output_format;
     jpegdInfo.arm_based = TRUE;
     jpeg_ARM_based_decode(&client_input_info, jpeg_thin_interface, jpegdInfo);
   }

}


/*===========================================================================
FUNCTION  jpeg_get_exif_pointer

DESCRIPTION
  Get the 4 bytes pointer at the current byte stream 

DEPENDENCIES
  None

RETURN VALUE
  The pointer value

SIDE EFFECTS
  None
===========================================================================*/

uint32 jpeg_get_exif_pointer(const uint8 * streamPtr, boolean big_endian)
{
   uint32 value;
    
    /* Depend on the format of the input data */
    if (big_endian)
    {
       value = *streamPtr++;
       value = value + (((uint32)(*streamPtr++)) << 8);
       value = value + (((uint32)(*streamPtr++)) << 16);
       value = value + (((uint32)(*streamPtr++)) << 24);
    }
    else
    {
       value = *streamPtr++;
       value = (value << 8) + *streamPtr++;
       value = (value << 8) + *streamPtr++;
       value = (value << 8) + *streamPtr++;
    }
  
    return value;
}

/*===========================================================================
FUNCTION  jpeg_get_exif_entry

DESCRIPTION
  Get the entry number at the current byte stream 

DEPENDENCIES
  None

RETURN VALUE
  Entry number

SIDE EFFECTS
  None
===========================================================================*/
uint16 jpeg_get_exif_entry(const uint8 * streamPtr, boolean big_endian)
{
    uint16 value;
   
    if (big_endian)
    {
       value = *streamPtr++;
       value = (uint16)(value + (((uint16)(*streamPtr++)) << 8));
    }
    else
    {
       value = *streamPtr++;
       value = (uint16)((value << 8) + *streamPtr++);
    }
    return value;
}

/*===========================================================================
FUNCTION  jpeg_extract_thumbnail

DESCRIPTION
  Extract the thumbnail image from the EXIF JPEG image.
  
  This function allocates the output buffer. It is the caller's
  responsibility to free the outPtr when data have been consumed.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE:  succeed with thumbnail extracted
            FALSE: No thumbnail or un-recognize file

SIDE EFFECTS
  None
===========================================================================*/
boolean  jpeg_extract_thumbnail( uint8 *bitStreamPtr, uint8 *outPtr,
                                               uint32 *out_buf_size)
{
  uint8  thischar, lastchar, *inPtr;
  uint8  *exif_hdr_ptr, *thumbnailPtr=NULL;
  uint32 i, length = 0;
  uint16 entries, tag;
  uint32 offset, thumbnail_size=0;
  boolean big_endian;

  inPtr = bitStreamPtr;
  
    
  lastchar = *inPtr++;
  thischar = *inPtr++;

  /* Expect SOI marker for JPEG file */

  if ((lastchar != 0xFF) || (thischar != (uint8)M_SOI))
  {
     return FALSE;
  }
  else
  {
      /* write the SOI to the file */
      lastchar = *inPtr++;
      thischar = *inPtr++;

      /* Make sure this is a EXIF JPEG file */
    if ((lastchar != 0xFF) || (thischar != (uint8)M_APP1))
      {
         return FALSE;
      }
      else
      {  
         /* write APP0 to output */
         length = *inPtr++;
         length = (length << 8) + *inPtr++;
        
         /* Check for a valid thumbail in the Exif */
         if (length == 0)
            return FALSE; 

         /* search for Oth IFD */
         exif_hdr_ptr =  inPtr + 6;
       
         if ((*exif_hdr_ptr == 0x4D) && (*(exif_hdr_ptr+1) == 0x4D))
         {
            big_endian = FALSE;
         }
         else
         {
            big_endian = TRUE;
         }
         
         /* set pointer to offset of next IFD  */
         inPtr = exif_hdr_ptr + 4;
         offset = jpeg_get_exif_pointer(inPtr, big_endian);

         /* Get entry count for the Oth IFD */
         inPtr = exif_hdr_ptr + offset;

         if (inPtr > (exif_hdr_ptr + length))
            return FALSE; 

         entries = jpeg_get_exif_entry(inPtr, big_endian);
         
         /* Skip Oth IFD and look for 1st IFD */
         inPtr = inPtr + entries * ENTRY_SIZE +2;
         offset = jpeg_get_exif_pointer(inPtr, big_endian);

         if (!offset) return FALSE;
         
         /* Get entry count for the 1st IFD */
         inPtr = exif_hdr_ptr + offset;

         if (inPtr > (exif_hdr_ptr + length))
            return FALSE; 

         entries = jpeg_get_exif_entry(inPtr, big_endian);
       
         /* Search for the thumbnail data and thumbnail size tags */
         inPtr = inPtr + 2;
         for (i=entries; i>0; i--)
         {
           tag = jpeg_get_exif_entry(inPtr, big_endian);
           switch (tag)
           {
             case 0x0201:
               inPtr += 8;
               offset = jpeg_get_exif_pointer(inPtr, big_endian);
               /* this is the offset for the thumbnail */
               thumbnailPtr = exif_hdr_ptr + offset; 
               inPtr += 4;
               break;
             case 0x0202:
               inPtr += 8;
               /* this is the size of the thumbnail */
               thumbnail_size = jpeg_get_exif_pointer(inPtr, big_endian);
               inPtr += 4;
               break;
             default:
               inPtr += 12;
           }
         }

         if(!thumbnailPtr)
           return FALSE;

         if (thumbnailPtr > (exif_hdr_ptr + length))
         {
             return FALSE;
         }
         else /* A valid thumbnail is located */
         {
             if (*out_buf_size >= thumbnail_size)
             {
                *out_buf_size = thumbnail_size;
                memcpy(outPtr, thumbnailPtr, thumbnail_size);
             }
             else
                return FALSE;
         }
      } 
  }

  return TRUE;
}

#endif /* FEATURE_JPEG_DECODER */


#ifdef FEATURE_JPEG_ENCODER


/*===========================================================================

FUNCTION   JPEGE_ENCODE

DESCRIPTION
  This function gets called by client to perform the JPEG encode by 
  calling into the lower Camera layer.  The Camera layer takes care of all
  DSP interaction and invokes a callback up to this layer.

  Note that this function will not create the EFS path to an output file
  if it doesn't already exist!


ARGUEMENTS
  jpege__encode_type                   jpege_params,
    -Input & output specification

  void                                *handle,
    -Serves no purpose except to pass back via callback to CMX task
     for that tasks bookkeeping.

  jpege_cb_func_type                  callback_ptr
    -callback handle to alert caller of final encode status

DEPENDENCIES
  None

RETURN VALUE
  Through callback, if the callback pointer is valid, otherwise this 
  function does nothing.

  Through return code:
  JPEGE_SUCCESS                     :Lower processing layer has been successfully 
                                     queued with JPEG encode command
  JPEGE_CB_FUNC_NOT_SPECIFIED       :Callback FN not provided
  JPEGE_INVALID_PARM
  JPEGE_INPUT_FORMAT_NOT_SUPPORTED

SIDE EFFECTS
  

===========================================================================*/
jpege_return_code_type jpege_encode_fn
(
  jpege_encode_type       encoding_spec,
  void  *                 handle,
  jpege_cb_func_type      callback_ptr
)
{
  camera_ret_code_type camera_ret;

  /* Must have a non-null callback */
  jpeg_enc_cb = callback_ptr;

  jpeg_enc_inst_handle_ptr = handle;

  if(jpeg_enc_cb == NULL)
  {
    return JPEGE_CB_FUNC_NOT_SPECIFIED;
  }

  /* Translate params from jpege_type to camera_frame_type 
     before making camera layer call */
  /* BAYER is not supported */
  switch( encoding_spec.format )
  {

     case JPEGE_YCBCR:
        global_camera_frame.format = CAMERA_YCBCR;
        break;

#if 0
// not supported yet
     case JPEGE_RGB565:
        global_camera_frame.format = CAMERA_RGB565;
        break;

     case JPEGE_RGB666:
        global_camera_frame.format = CAMERA_RGB666;
        break;
#endif

     case JPEGE_RGB444:
        global_camera_frame.format = CAMERA_RGB444;
        break;

     case JPEGE_RGB888:
        global_camera_frame.format = CAMERA_RGB888;
        break;

     default:
        return JPEGE_INVALID_PARM;

  }


  global_camera_frame.dx = (uint16)encoding_spec.dx;
  global_camera_frame.dy = (uint16)encoding_spec.dy;
  global_camera_frame.rotation = 0;   /* No rotation */

  /*  Set input buffer to entire image */
  global_camera_frame.buffer = encoding_spec.in_data;

  g_max_file_size = encoding_spec.max_file_size;         /* JPEG max file size       */
  /* Translate params from jpege_type to camera_handle_type 
     before making camera layer call */
  if (encoding_spec.device == JPEGE_DEVICE_MEM)
  {
    global_camera_handle.device = CAMERA_DEVICE_MEM;
    global_camera_handle.mem.device = (camera_device_type)(encoding_spec.device);
    /* JPEG encoding here.                  */
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
    global_camera_handle.mem.format = CAMERA_JPEG;
    global_camera_handle.mem.quality = encoding_spec.quality;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */
    global_camera_handle.mem.buffer = encoding_spec.device_spec.mem.buffer;
    global_camera_handle.mem.length = encoding_spec.device_spec.mem.length;


  }
  else if (encoding_spec.device == JPEGE_DEVICE_EFS)
  {
    global_camera_handle.device = CAMERA_DEVICE_EFS;
    global_camera_handle.efs.device = (camera_device_type)(encoding_spec.device);
    /* JPEG encoding here.                  */
#ifndef FEATURE_CAMERA_ENCODE_PROPERTIES
    global_camera_handle.efs.format = CAMERA_JPEG;
    global_camera_handle.efs.quality = encoding_spec.quality;
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES */

#ifdef JPEG_USES_LIBSTD
    (void)std_strlcpy(global_camera_handle.efs.filename, encoding_spec.device_spec.efs.filename, FS_FILENAME_MAX_LENGTH_P);
#else
    (void)strcpy(global_camera_handle.efs.filename, encoding_spec.device_spec.efs.filename);
#endif

    

  }
  else
  {
     return JPEGE_INVALID_PARM;
  }
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
  global_encode_properties.format = CAMERA_JPEG;
  global_encode_properties.quality = encoding_spec.quality;
  global_encode_properties.file_size = g_max_file_size;
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

  jpeg_enc_status = JPEGE_ENCODE;

#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
  camera_ret = camera_encode_picture_stand_alone(
               &global_camera_frame,         /* Uncompressed Input stats */
               &global_camera_handle,        /* Services requested       */
               &global_encode_properties,    /* Services requested       */
               jpeg_CameraLayerCB,    /* Local callback           */
               jpeg_enc_inst_handle_ptr ); /* client data here - to pass back via callback */
#else /* FEATURE_CAMERA_ENCODE_PROPERTIES */
  camera_ret = camera_encode_picture_stand_alone(
               g_max_file_size,              /* JPEG max file size       */
               &global_camera_frame,         /* Uncompressed Input stats */
               &global_camera_handle,        /* Services requested       */
               jpeg_CameraLayerCB,    /* Local callback           */
               jpeg_enc_inst_handle_ptr ); /* client data here - to pass back via callback */
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

  if (camera_ret != CAMERA_SUCCESS)
  {
     return JPEGE_CAMERA_LAYER_CALL_FAILED;
  }

  return JPEGE_SUCCESS;

}



/*===========================================================================

FUNCTION   JPEG_ABORT_ENCODE

DESCRIPTION
  Abort function for the JPEG encoder.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void jpeg_abort_encode ()
{
  //camera_ret_code_type camera_ret;

  jpeg_encode_abort_flag = TRUE;               /* set encode abort flag    */
  /* stop encoding: queues request to run in graph context */
  camera_release_picture(); /*lint !e534 intentionally ignoring returned value */
}





/*===========================================================================

FUNCTION   JPEG_CAMERALAYERCB

DESCRIPTION
  Translate the returns from the Camera layer into generic JPEG stand-
  alone codes. 

DEPENDENCIES
  None

RETURN VALUE
  One of the following through callback:
  JPEGE_ENCODE
  JPEGE_CB_DONE
  JPEGE_CB_ABORT
  JPEGE_CB_ERROR
  JPEGE_CB_FILE_SIZE_EXCEEDED

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void jpeg_CameraLayerCB
(
  camera_cb_type cb, 
  const void *client_data, 
  camera_func_type func, 
  int32 parm4
)
{

  if (jpeg_enc_cb == NULL)
  {
    return;
  }

  // Common status handling...
  switch (cb)
  {
    case CAMERA_EXIT_CB_DSP_IDLE:    // Fall through...
        camera_stop_stand_alone(
             NULL,    /* no callback           */
             (void *)client_data ); /* client data here - to pass back via callback */
       jpeg_enc_cb(JPEGE_CB_ERROR, (void *)client_data, NULL);
       /* if (camera_ret != CAMERA_SUCCESS) */
       break;

    case CAMERA_EXIT_CB_DSP_ABORT:   // Fall through...
    case CAMERA_EXIT_CB_ABORT:       // Fall through...
       camera_stop_stand_alone(
             NULL,    /* no callback           */
             (void *)client_data ); /* client data here - to pass back via callback */
       jpeg_enc_cb(JPEGE_CB_ABORT, (void *)client_data, NULL);
       break;

    case CAMERA_EXIT_CB_ERROR:
       /* Failed to do JPEG encoding, alert caller */
       /* Just pass client data pointer through to give caller
         proper context */
       camera_stop_stand_alone(
             NULL,    /* no callback           */
             (void *)client_data ); /* client data here - to pass back via callback */
       jpeg_enc_cb(JPEGE_CB_ERROR, (void *)client_data, NULL);
       break;

    case CAMERA_EVT_CB_FRAME:
       break;

    case CAMERA_EXIT_CB_DONE:
       /* JPEG encoding completed sucessfully */
       /* Just pass client data pointer through to give caller
         proper context */
       camera_stop_stand_alone(
             NULL,    /* no callback           */
             (void *)client_data ); /* client data here - to pass back via callback */

       /* Parm4 == num bytes of output in MEM mode */
       jpege_mem_mode_output_size = parm4;
       jpeg_enc_cb(JPEGE_CB_DONE, (void *)client_data, &jpege_mem_mode_output_size);
       break;

    case CAMERA_EXIT_CB_FILE_SIZE_EXCEEDED:
       /* JPEG encoding completed, but output exceeded specified file size */
       /* Just pass client data pointer through to give caller
         proper context */
       camera_stop_stand_alone(
             NULL,    /* no callback           */
             (void *)client_data ); /* client data here - to pass back via callback */

       /* Parm4 == num bytes of output in MEM mode */
       jpege_mem_mode_output_size = parm4;
       jpeg_enc_cb(JPEGE_CB_FILE_SIZE_EXCEEDED, (void *)client_data, &jpege_mem_mode_output_size);
       break;

    case CAMERA_RSP_CB_SUCCESS:
       /* Function is accepted         */
       /* Do nothing interms of calling app here */
       break;
  default:
    break;
  
  }

} /*lint !e550 !e715 */

#endif /* FEATURE_JPEG_ENCODER */

/* <EJECT> */
/*===========================================================================

FUNCTION   JPEG_SET_BUF

DESCRIPTION
  Function for setting a buffer ptr to buffer passed in by client.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 jpeg_set_buf
(
  uint8   *client_buffer,                         /* client buffer         */
  uint32  client_buffer_size,                     /* size of client buffer */
  uint8   *output_buffer
)
{

  /* if client_buffer_size is less than JPEG needs then exit               */
  if (client_buffer_size < MAX_JPEG_BUFFER_SIZE)
  {
    jpeg_set_buffer_flag = FALSE;           /* set buffer flag             */
    MSG_ERROR("Buffer allocated for JPEG is too small", 0, 0, 0);
    return (1);
  }

  jpeg_io_buf1_ptr = client_buffer;    /* set JPEG input ptr to client buf */
  jpeg_io_buf2_ptr = client_buffer + MAX_IO_BUF_1_SIZE;  /* set output ptr */
  jpeg_mcu_ptr = jpeg_io_buf2_ptr +  MAX_IO_BUF_2_SIZE;  /* set mcu ptr    */

  jpeg_output_buf_ptr = output_buffer;

  jpeg_set_buffer_flag = TRUE;               /* set buffer flag            */

  return (0);
}

#ifdef JPEG_TIMING_TEST
/* Procedure to calculate elapsed time */
char *qmark_timing ( qword start, qword finish )
{
   int32  run_time_l = 0;
   qword run_time;
   char mark_str[25];

   qw_sub (run_time, finish, start);

   if (qw_hi(run_time)==0)       /*most of cases */
      run_time_l = qw_lo(run_time);

   std_strlprintf(mark_str,sizeof(mark_str)," %d [ms]   ", (int)run_time_l);

   return(mark_str);
}
#endif

#endif /* defined(FEATURE_JPEG_DECODER) || defined(FEATURE_JPEG_ENCODER)   */

#if 0 /* start of commented out JFIF converion function */

#define M_SOI        0xD8
#define M_APP0       0xE0
#define M_APP1       0xE1
#define JPEG_FORMAT  0x0201
#define JPEG_LENGTH  0x0202
#define ENTRY_SIZE   12
#define JFXX_SIZE    12

uint8 jfif_extension[] = {0x4a, 0x46,  0x58, 0x58, 0, 0x10};

/*===========================================================================
FUNCTION  exif_get_pointer

DESCRIPTION
  Get the 4 bytes pointer at the current byte stream 

DEPENDENCIES
  None

RETURN VALUE
  The pointer value

SIDE EFFECTS
  None
===========================================================================*/

uint32 exif_get_pointer(uint8 * streamPtr, boolean big_endian)
{
  uint32 value;
    
    /* Depend on the format of the input data */
    if (big_endian)
    {
       value = *((uint32 *) streamPtr);
    }
    else
    {
       value = *streamPtr++;
       value = (value << 8) + *streamPtr++;
       value = (value << 8) + *streamPtr++;
       value = (value << 8) + *streamPtr++;
    }
  
    return value;
}

/*===========================================================================
FUNCTION  exif_get_entry

DESCRIPTION
  Get the entry number at the current byte stream 

DEPENDENCIES
  None

RETURN VALUE
  Entry number

SIDE EFFECTS
  None
===========================================================================*/
uint32 exif_get_entry(uint8 * streamPtr, boolean big_endian)
{
  uint32 value;
   
    if (big_endian)
    {
       value = *((uint16 *) streamPtr);
    }
    else
    {
       value = *streamPtr++;
       value = (value << 8) + *streamPtr++;
    }
    return value;
}

/*===========================================================================
FUNCTION  jpeg_convert_eThumb2jThumb

DESCRIPTION
  Search for Thumbnail in EXIF JPEG file and extract it to build a JPEG file
  with JFIF extension. 
  
DEPENDENCIES
  This conversion function expects input JPEG file that contains EXIF with 
       JPEG encoded only thumbnail. 
  This conversion function expects input JPEG file generated by the QCT's 
       Camera app. JPEG images from sources other Camera app. may work

RETURN VALUE
  boolean - TRUE:  conversion succeed
            FALSE: conversion failed

SIDE EFFECTS
  None
===========================================================================*/
boolean  jpeg_convert_eThumb2jThumb( uint8 *inPtr, uint32 exif_size, 
                                     uint8 *outPtr, uint32 *jfif_size)
{
  uint8  thischar, lastchar, *savePtr;
  uint8  *exif_hdr_ptr, *thumbnailPtr=NULL;
  uint32 i, length = 0;
  uint16 entries, tag;
  uint32 offset, thumbnail_size=0;
  boolean big_endian;

  savePtr = inPtr;
  *jfif_size = 0;
    
  lastchar = *inPtr++;
  thischar = *inPtr++;

  /* Expect SOI marker for JPEG file */

  if ((lastchar != 0xFF) || (thischar != M_SOI))
  {
     return FALSE;
  }
  else
  {
      /* write the SOI to the file */
      *outPtr++ = 0xFF;
      *outPtr++ = 0xD8;
      lastchar = *inPtr++;
      thischar = *inPtr++;

      /* Make sure this is a EXIF JPEG file */
      if ((lastchar != 0xFF) || (thischar != M_APP1))
      {
         return FALSE;
      }
      else
      {  
         /* write APP0 to output */
         *outPtr++ = 0xFF;
         *outPtr++ = M_APP0;
         length = *inPtr++;
         length = (length << 8) + *inPtr++;
        
         /* Check for a valid thumbail in the Exif */
         if (length == 0)
            return FALSE; 

         /* search for Oth IFD */
         exif_hdr_ptr =  inPtr + 6;
       
         if ((*exif_hdr_ptr == 0x4D) && (*(exif_hdr_ptr+1) == 0x4D))
         {
            big_endian = FALSE;
         }
         else
         {
            big_endian = TRUE;
         }
         
         /* set pointer to offset of next IFD  */
         inPtr = exif_hdr_ptr + 4;
         offset = exif_get_pointer(inPtr, big_endian);

         /* Get entry count for the Oth IFD */
         inPtr = exif_hdr_ptr + offset;

         if (inPtr > (exif_hdr_ptr + length))
            return FALSE; 

         entries = exif_get_entry(inPtr, big_endian);
         
         /* Skip Oth IFD and look for 1st IFD */
         inPtr = inPtr + entries * ENTRY_SIZE +2;
         offset = exif_get_pointer(inPtr, big_endian);

         if (!offset) return FALSE;
         
         /* Get entry count for the 1st IFD */
         inPtr = exif_hdr_ptr + offset;

         if (inPtr > (exif_hdr_ptr + length))
            return FALSE; 

         entries = exif_get_entry(inPtr, big_endian);
       
         /* Search for the thumbnail data and thumbnail size tags */
         inPtr = inPtr + 2;
         for (i=entries; i>0; i--)
         {
             tag = exif_get_entry(inPtr, big_endian);
             switch (tag)
             {
             case 0x0201:
                 inPtr += 8;
                 offset = exif_get_pointer(inPtr, big_endian);
                 /* this is the offset for the thumbnail */
                 thumbnailPtr = exif_hdr_ptr + offset; 
                 inPtr += 4;
                 break;
             case 0x0202:
                 inPtr += 8;
                 /* this is the size of the thumbnail */
                 thumbnail_size = exif_get_pointer(inPtr, big_endian);
                 inPtr += 4;
                 break;
             default:
                 inPtr += 12;
              }
          }

          if (thumbnailPtr > (exif_hdr_ptr + length))
         {
             return FALSE;
         }
         else /* thumbnail is located */
         {
             thumbnail_size += 8;
             *outPtr++= thumbnail_size >> 8;
             *outPtr++= thumbnail_size & 0xFF;
          
             memcpy(outPtr, &jfif_extension, sizeof(jfif_extension));
             outPtr += sizeof(jfif_extension);
             memcpy(outPtr, thumbnailPtr,(exif_size - (thumbnailPtr-savePtr)));
         }
         
         /* Update the size of the newly compose JFIF image */ 
         *jfif_size = (exif_size - (thumbnailPtr-savePtr)+JFXX_SIZE);
      } 
  
  }

  return TRUE;
}

#endif /* end of commented out JFIF converion function */




