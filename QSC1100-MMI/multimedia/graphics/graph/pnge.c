/* linesize(132)
** pagesize(60)                                     
** title("Dual Mode Subscriber Station")
** subtitle("PNG Encoder")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          P O R T I B L E    N E T W O R K    G R A P H I C S   (PNG)   
                      E N C O D E R    

GENERAL DESCRIPTION
  This header file contains the necessary interfaces for the
  PNG encoder.
  
EXTERNAL FUNCTIONS
  pnge_encode
  
  pnge_encoder_abort
  
Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/graph/pnge.c#27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/08   vma     Fixed compilation error for RVCT22
05/02/07   vma     Remove partially encoded file during cleanup in case of 
                   abortion;
                   Resolved lint errors
09/05/06   vma     Addition of separate chroma pointer in PNG encode spec
08/31/06   vma     Bug fix: Return PNGE_INVALID_PARM instead of FALSE in case
                   of EFS failure inside pnge_encode() function
07/07/06   vma     Check width when input is YCbCr: reject request if odd
09/30/05   kln     Limitted max encode dimensions to VGA (480x640)
09/16/05   kln     Fixed lint errors, added a check in pnge_deallocate_mem if
                   pngeEncodeType struct pointer is !NULL before dereferencing
05/11/05   kln     Initialized pointer p in pnge_encode()
05/11/05   anb     Changed MAX_IMG_HEIGHT from 1200 to 1600.
04/11/05   kln     Set to NULL all pointers after being freed
03/02/05   kln     Added check if valid string but not valid file name to 
                   check if valid file handler and use it instead
02/22/05   pdo     Changed crc32() to pnge_crc32()
02/04/05   Ram     Swapped input/output buffers for RGB case and removed
                     unnecessary callback in pnge_encode()
07/01/05   kln     Added intermediate converstion from YCbCr line packed 
                   to YCbCr IPL. Added a function to swap CbCr. Added file 
                   open functionality

07/01/04   mvd     Added "pnge" prefix to all functions, local, and extern
                   global variables.
05/12/04   mvd     Added checking & callback error returns for malloc 
                   failures at top level.  Added descriptive callback return 
                   codes.  Increased the max input image resolution to 
                   1600x1200.  Fixed illegal deallocated struct reference
                   on this line: pnge_handle->length = ..
                   Removed DEBUG_STATS, PNG_USE_ABS & DYNAMIC_CRC_TABLE
12/16/03   mvd     Moved Bayer image conversion from camera.c to this file
                   but now converting a selectable number of rows at a time
                   rather than the whole image at once (mem savings).
                   Error returns now result in proper PNGE Abort processing
                   so that the UI can recover properly.
                   Increased max resolution supported to 1280x1024 from
                   1024x768.
                   Known issue: encoding fails with portrait orientation if
                   the resolution is > QQVGA.
12/12/03   mvd     Added support for selecting a MEM or EFS device for 
                   encoded output.
12/11/03   pdo     Updated code to handle both EFS and MEM output cases
                   This version does not send back the encoded size.
12/11/03   mvd     Added capability to cap the num bytes outputted to some
                   configurable max number.  Can also return num bytes 
                   outputted.
                   Consolodated global vars into struct and passed into
                   PNGE init function.
                   Capped max image size at XGA due to mem alloc limits now
11/26/03   mvd     Removed dead code
                   Removed MATT_DEBUG code
                   Removed PNG_WRITE_WEIGHTED_FILTER_SUPPORTED
                   Removed PNG_SLOW_PAETH
                   Removed MSM_TARGET and PC_TARGET
                   Increased max image size to encode to 1280x1024
                   Corrected filter allocate/deallocate calls for filter selection
11/25/03   pdo     Added the PNG encoder engine
11/07/03   pdo     Initial version

===========================================================================*/

/* <EJECT> */


/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "pnge.h"
#include "pngglobal.h"
#include "pngdeflate.h"

#if defined(FEATURE_PNG_ENCODER)

#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "stdlib.h"             /* Standard I/O routines                   */
#include "graph.h"              /* Graphics task typedefs and prototype    */
#include "ipl.h"                /* Color conversion.                       */

/* <EJECT> */

/*===========================================================================

                      EXTERNAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define MAX_IMG_WIDTH   640
#define MAX_IMG_HEIGHT  640

uint32 pnge_num_bytes_malloced;
#if 0
   int32 pnge_num_bytes_freed;
#endif

pngeEncodeType * pnge_enc_ptr; /* Global PNG encoder structure */

LOCAL PNGE_status_enum_type  pnge_status;
LOCAL PNGE_cb_func_type      pnge_callback;
LOCAL pnge_handle_type      *pnge_handle;
LOCAL boolean                pnge_abort_encoder;
LOCAL PNGE_stateType         pnge_state;
LOCAL graph_png_request_type pnge_request;
LOCAL ipl_image_type         pnge_input_ipl_image, pnge_output_ipl_image; 
LOCAL ipl_image_type         pnge_inputTemp_ipl_image, pnge_outputTemp_ipl_image; 
LOCAL uint16                 pnge_bytes_per_pixel;
LOCAL uint8                 *YCbYCr_ptr; 

/* Hopefully this struct is temporary until we can 
   downsize and convert to RGB in one step!! */
LOCAL ipl_image_type         pnge_output_bayer_ipl_image; 

/* Local struct for passing parameters from the interface down
   to the encode engine */
typedef struct 
{
   // mem pointer to entire input image
   uint8 * inputPtr;

   // This is the max num bytes available for the encoded PNG image.
   // Either file OR buffer writes will NOT exceed this number of bytes
   // (even at the expense of not completing the encoding)
   // An error flag is returned if this limit is hit while encoding.
   uint32 maxOutputBytesToWrite;

   // Selecting either a file or memory pointer as the output type
   pngeOutputFormatType outputFormat;

   // If selected new PNG image output is written here
   fs_handle_type * outputFilePtr;

   // If selected new PNG image output is written here
   uint8 * outputPtr;

   // Width of input image in pixels
   uint32 width;
   // Height of input image in pixels
   uint32 height;
   // PNG Standard Color Type
   uint8 pngColorType;
   uint8 pngBitDepth;

   // For Bayer image width and height, because it is differnent from 
   // encoded image resolution
   uint16 bayerWidth;
   uint16 bayerHeight;

   // Stores num rows per iteration for Bayer format
   uint16 bayerNumRowsPerIter;

   // File name saved for possible later deletion of file 
   char * filename;

} pngeParmsType;

LOCAL pngeParmsType pnge_global_params;


/*===========================================================================
                      INTERNAL FUNCTION DECLARATIONS
===========================================================================*/

void pnge_store_uint_32(uint8 * buf, uint32 value);
void pnge_store_uint_16(uint8 * buf, uint16 value);
pngeEncRetCodeType pnge_write_IHDR( pngeEncodeType * pngEncPtr );
uint32 pnge_write_output( pngeEncodeType * pngEncPtr, const uint8 * data, uint32 numBytes );
pngeEncRetCodeType  pnge_write_chunks_before_PLTE( pngeEncodeType * pngEncPtr );
void pnge_write_chunk_start( pngeEncodeType * pngEncPtr, uint32 chunkSize, uint32 chunkName );
void pnge_write_chunk_data( pngeEncodeType * pngEncPtr, const uint8 * data, uint32 numBytes);
void pnge_write_chunk_end( pngeEncodeType * pngEncPtr );
pngeEncRetCodeType pnge_init_deflate( const pngeEncodeType * pngEncPtr );
pngeEncRetCodeType pnge_write_IEND( pngeEncodeType * pngEncPtr );
pngeEncRetCodeType pnge_write_IDAT( pngeEncodeType * pngEncPtr, 
                                    const uint8 * data, 
                                    uint32 numBytes );
uint8 * pnge_find_filter( const pngeEncodeType * pngEncPtr, uint8 * rowPtr );
void pnge_write_filtered_row( pngeEncodeType * pngEncPtr );
void pnge_write_final_row( pngeEncodeType * pngEncPtr );
pngeEncRetCodeType pnge_allocate_and_init_filter_scratch( pngeEncodeType * pngEncPtr );
uint32 pnge_crc32( uint32 crc, const uint8 *buf, uint32 len);
pngeEncRetCodeType pnge_mem_allocate( void ** dest, uint32 numBytes );
extern void pnge_mem_free( void * buf );
pngeEncRetCodeType pnge_deallocate_mem( pngeEncodeType * pngEncPtr );
pngeEncRetCodeType pnge_initalize_encode_struct( pngeEncodeType ** pngEncPtrPtr,
                                                 const pngeParmsType * global_params );
pngeEncRetCodeType pnge_encode_rows( pngeEncodeType * pngEncPtr, 
                                   uint32 numRowsToProcess,
                                   uint8 * inputRowsPtr          );


void pnge_encode_cleanup_callback(void);
void pnge_swap_cbcr(uint8 *cbcr_ptr, uint32 len);


/*===========================================================================

FUNCTION: PNGE_ENCODE

DESCRIPTION: This function checks for the input parameters.
             If no invalid parameter then it queues the request to the 
             GRAPH thread for PNG encode processing.
             
DEPENDENCIES
   None.

RETURN VALUE
   PNGE_SUCCESS              cmd sucessful
   PNGE_INVALID_PARM         cmd with invalid input parameter
   PNGE_INVALID_INPUT_SIZE_PARM cmd with unsupported image resolution
   
COMMENTS
   If calling from GRAPH thread, caller should exit GRAPH thread for the 
   PNG encoder to start.

SIDE EFFECTS
   None
==============================================================================*/
PNGE_returnCodeType   pnge_encode
(
  PNGE_spec_type      pnge_spec,
  pnge_handle_type   *handle,
  PNGE_cb_func_type   callback_ptr
)
{
  
  uint32 numRowsPerIteration = DEFAULT_NUM_ROWS_PER_ITER;
  uint16 filename_len;
  char *filename;
  char *p = NULL;
  fs_open_xparms_type open_parms;
  /* init the error code */
  pngeEncRetCodeType retCode = PNGENC_UNKNOWN_ERROR;
  fs_rsp_msg_type    fs_rsp_tmp;

  /* Check for input parameters */
  if (pnge_spec.input_ptr == NULL)
  {
      return PNGE_INVALID_PARM;
  }
 
  if ((pnge_spec.image_width > MAX_IMG_WIDTH) ||
      (pnge_spec.image_height > MAX_IMG_HEIGHT)) 
  {
     return PNGE_INVALID_PARM;
  }

  /* if input format is YCbCr format, input width should be even */
  if (((pnge_spec.input_data == YCrCb422LP) || (pnge_spec.input_data == YCrCb420LP) || (pnge_spec.input_data == YCbCr422)) 
      && ((pnge_spec.image_width % 2) == 1))
  {
    return PNGE_INVALID_WIDTH_YCBCR_INPUT;
  }

  pnge_callback   = callback_ptr;
  pnge_handle     = handle;
  pnge_state      = PNGE_START;
  pnge_abort_encoder = FALSE;
  pnge_status     = PNGE_ENCODE;

  /* Save all essential parameters for the encoding process */
  pnge_global_params.width = pnge_spec.image_width;
  pnge_global_params.height = pnge_spec.image_height;
  
  if (pnge_handle->device == PNGE_DEVICE_EFS)
  {
   
    /* Tell encoder that output will be placed in memory */
    pnge_global_params.outputFormat = FILE_POINTER;


    if (pnge_spec.filename[0] != 0)
    {
        filename = strchr(pnge_spec.filename, '/');
        while(filename)
        {
            p = filename + 1;
            filename = strchr(p, '/');
        }
        if (p)
        { 
            filename = p;
        }
        else
        {
            filename = pnge_spec.filename;
        }
        filename_len = (uint16)strlen(filename);
        if (filename_len > 20)
        {
            filename_len = 20;
        }

        /* Handle the case of the file already exist */
        fs_remove(pnge_spec.filename, NULL, &fs_rsp_tmp);
        open_parms.create.cleanup_option = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;

       /* Open the file with a handle */
        fs_open(pnge_spec.filename, FS_OA_CREATE, 
              &open_parms, NULL, &fs_rsp_tmp);

        /* Check if invalid file name and null file handler */
        if ((fs_rsp_tmp.open.status != FS_OKAY_S) && (pnge_handle->fs_rsp.open.handle == NULL))
        {
            pnge_encode_abort();
            MSG_ERROR("PNGENC: EFS failed. ", 0, 0, 0);
            return PNGE_INVALID_PARM;
        }
        pnge_handle->fs_rsp = fs_rsp_tmp;

        /* store the filename in pnge_spec to pnge_global_params for possible later file removal */
        if (pnge_mem_allocate( (void**)&pnge_global_params.filename, strlen(pnge_spec.filename) + 1) 
            != PNGENC_SUCCESS)
        {
          MSG_FATAL(" PNGE: failed to allocate %d bytes", strlen(pnge_spec.filename) + 1, 0, 0);
        }
        else
        {
          memcpy(pnge_global_params.filename, pnge_spec.filename, strlen(pnge_spec.filename) + 1);
        }
    }

    /* copy handle pointer to global EFS pointer */
    pnge_global_params.outputFilePtr  = &pnge_handle->fs_rsp.open.handle;

    /* Just make this a big number for now */
    pnge_global_params.maxOutputBytesToWrite = PNGE_DEFAULT_MAX_NUM_OUTPUT_BYTES; 

  }
  else 
  {
    /* Tell encoder that output will be placed in memory */
    pnge_global_params.outputFormat = MEMORY_POINTER;

    /* Pointer where output PNG Encoded data is placed 
       ONLY if output directly to memeory mode is selected */
    //pnge_global_params.outputPtr = pnge_spec.output_ptr;
    pnge_global_params.outputPtr = pnge_handle->buffer;

    /* Just make this a big number for now */
    pnge_global_params.maxOutputBytesToWrite = pnge_handle->length; 

  }



  // Setup IPL struct for converting input to RGB888 format
  // This struct will contain info for convering X number of rows
  // per convert call and for keeping a pointer to the latest
  // unconverted row. 
  pnge_input_ipl_image.imgPtr = pnge_spec.input_ptr;
  // By default, but these dy and dx get overwritten if the input is Bayer format
  pnge_input_ipl_image.dy = numRowsPerIteration; // num rows to convert at 1 time
  pnge_input_ipl_image.dx = pnge_global_params.width;

  // Output will always be RGB888 because the PNG Encoder
  // expects this
  pnge_output_ipl_image.cFormat = IPL_RGB888;
  pnge_output_ipl_image.dy = numRowsPerIteration; // num rows to convert at 1 time;
  pnge_output_ipl_image.dx = pnge_global_params.width;

  // Converts from YCbCr to RGB888 for Bayer after downsize ONLY
  pnge_output_bayer_ipl_image.cFormat = IPL_YCbCr; 
  pnge_output_bayer_ipl_image.dy = numRowsPerIteration; // num rows to convert at 1 time;
  pnge_output_bayer_ipl_image.dx = pnge_global_params.width;

  pnge_global_params.bayerNumRowsPerIter = (uint16)((float)numRowsPerIteration * (float)((float)pnge_spec.bayer_image_height / (float)pnge_global_params.height));
  /* Force num bayer input rows to be even - esencial for downsize_fast call */
  pnge_global_params.bayerNumRowsPerIter &= 0xFFFE;

  pnge_global_params.bayerWidth  = pnge_spec.bayer_image_width;
  pnge_global_params.bayerHeight = pnge_spec.bayer_image_height;

  YCbYCr_ptr = NULL;
  // Set the IPL format within the input struct and the bytes
  // per pixel depending on the input raw data format
  switch (pnge_spec.input_data)
  {
    case YCrCb420LP:
    case YCrCb422LP:
      /* Alloacte temp memory to convert from yCbCr line packed to YCbCr IPL */
      retCode = pnge_mem_allocate( (void **) &YCbYCr_ptr ,2*(pnge_global_params.width*pnge_global_params.height));
      if( retCode != PNGENC_SUCCESS )
      {
         MSG_FATAL(" PNGE: failed to allocate %d bytes", \
                   2*(pnge_global_params.width*pnge_global_params.height), 0, 0);
         pnge_status = PNGE_CB_MALLOC_FAILED_ABORT;
         pnge_encode_cleanup_callback( );
         break;
      }
      pnge_inputTemp_ipl_image.imgPtr = pnge_spec.input_ptr;
      pnge_inputTemp_ipl_image.clrPtr = pnge_spec.chroma_ptr;
      pnge_inputTemp_ipl_image.dy = pnge_global_params.height; 
      pnge_inputTemp_ipl_image.dx = pnge_global_params.width;
      
      /* Swap CbCr data according to color format */
      if (pnge_spec.input_data == YCrCb420LP)
      {
          pnge_swap_cbcr(pnge_inputTemp_ipl_image.clrPtr,
              pnge_global_params.width*pnge_global_params.height/4);
          pnge_inputTemp_ipl_image.cFormat = IPL_YCbCr420_LINE_PK;
      }
      else
      {
          pnge_swap_cbcr(pnge_inputTemp_ipl_image.clrPtr,
              pnge_global_params.width*pnge_global_params.height/2);
          pnge_inputTemp_ipl_image.cFormat = IPL_YCbCr422_LINE_PK;
      }

      pnge_outputTemp_ipl_image.imgPtr = YCbYCr_ptr;
      pnge_outputTemp_ipl_image.dy = pnge_global_params.height; 
      pnge_outputTemp_ipl_image.dx = pnge_global_params.width;
      pnge_outputTemp_ipl_image.cFormat = IPL_YCbCr;
      
      if (pnge_spec.input_data == YCrCb420LP)
          (void)ipl_convert_ycbcr420lp_to_ycbcr422(&pnge_inputTemp_ipl_image, &pnge_outputTemp_ipl_image);
      else
          (void)ipl_convert_ycbcr422lp_to_ycbcr422(&pnge_inputTemp_ipl_image, &pnge_outputTemp_ipl_image);
      
      pnge_input_ipl_image.imgPtr = YCbYCr_ptr;
     
      pnge_bytes_per_pixel = 2;
      pnge_input_ipl_image.cFormat = IPL_YCbCr;
      break;     
    
    case YCbCr422:
      pnge_bytes_per_pixel = 2;
      pnge_input_ipl_image.cFormat = IPL_YCbCr;
      break;
    case RGB565:
      pnge_bytes_per_pixel = 2;
      pnge_input_ipl_image.cFormat = IPL_RGB565;
      break;

    case RGB666:
      pnge_bytes_per_pixel = 2;    // assumption
      pnge_input_ipl_image.cFormat = IPL_RGB666;
      break;

    case RGB444:
      pnge_bytes_per_pixel = 2;    // assumption
      pnge_input_ipl_image.cFormat = IPL_RGB444;
      break;

    case BAYER_BGGR:
      pnge_bytes_per_pixel = 1;
      pnge_input_ipl_image.cFormat = IPL_BAYER_BGGR;
      /* Special conversion for Bayer needed here to down
         size the raw frame */
      pnge_input_ipl_image.dy = pnge_global_params.bayerNumRowsPerIter;
      pnge_input_ipl_image.dx = pnge_spec.bayer_image_width;

      break;

    case BAYER_GBRG:
      pnge_bytes_per_pixel = 1;
      pnge_input_ipl_image.cFormat = IPL_BAYER_GBRG;
      /* Special conversion for Bayer needed here to down
         size the raw frame */
      pnge_input_ipl_image.dy = pnge_global_params.bayerNumRowsPerIter;
      pnge_input_ipl_image.dx = pnge_spec.bayer_image_width;

      break;

    case BAYER_GRBG:
      pnge_bytes_per_pixel = 1;
      pnge_input_ipl_image.cFormat = IPL_BAYER_GRBG;
      /* Special conversion for Bayer needed here to down
         size the raw frame */
      pnge_input_ipl_image.dy = pnge_global_params.bayerNumRowsPerIter;
      pnge_input_ipl_image.dx = pnge_spec.bayer_image_width;

      break;

    case BAYER_RGGB:
      pnge_bytes_per_pixel = 1;
      pnge_input_ipl_image.cFormat = IPL_BAYER_RGGB;
      /* Special conversion for Bayer needed here to down
         size the raw frame */
      pnge_input_ipl_image.dy = pnge_global_params.bayerNumRowsPerIter;
      pnge_input_ipl_image.dx = pnge_spec.bayer_image_width;

      break;

    case RGB888:
      pnge_bytes_per_pixel = 3;
      pnge_input_ipl_image.cFormat = IPL_RGB888;
      break;

    default:
      pnge_state = PNGE_ABORT;
      MSG_FATAL(" un-supported raw data format", 0, 0, 0 );
      return PNGE_INPUT_FORMAT_NOT_SUPPORTED;

  }


  /* parameters look good, queue the request to GRAPH thread */
  /* Graph thread will carry out the encoding process */
  graph_png_encode();

  return PNGE_SUCCESS;

} /* pnge_encode */

/* <EJECT> */





/*===========================================================================

FUNCTION   PNGE_ENCODE_ABORT

DESCRIPTION
  This function should be called when for some reasons the encoding process
  need to be abort forcefully.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void pnge_encode_abort( void )
{
    pnge_abort_encoder = TRUE;
}




/*===========================================================================

Function: PNGE_IMAGE_ENCODE

Description:  This function is called by the GRAPH thread to start the
              PNG encoding.

Return Value: none
   
Side Effects: none

============================================================================*/
void   pnge_image_encode()
{

   /* init the error code */
   pngeEncRetCodeType retCode = PNGENC_UNKNOWN_ERROR;
   /* num rows to process per iteration */
   ipl_status_type ipl_ret_code;


   if (pnge_abort_encoder) 
   {
      pnge_state = PNGE_ABORT; 
   }
   /* A state machine to keep the encoding happening in multiple     */
   /* time slices. This should help in aborting the encoding as soon */
   /* as possible and avoid starving the sleep thread */

   switch (pnge_state) 
   {
   case PNGE_START:


      /* any one time setup before the actual encoding */
      pnge_state = PNGE_PROCESSING;

      /* Hopefully this struct is temporary until we can 
         downsize and convert to RGB in one step!! */
      retCode = pnge_mem_allocate( (void **) &pnge_output_bayer_ipl_image.imgPtr, \
                                  3*(pnge_global_params.width*pnge_input_ipl_image.dy) ); 
      if( retCode != PNGENC_SUCCESS )
      {
         MSG_FATAL(" PNGE: failed to allocate %d bytes", \
                   3*(pnge_global_params.width*pnge_input_ipl_image.dy), 0, 0);
         pnge_status = PNGE_CB_MALLOC_FAILED_ABORT;
         pnge_encode_cleanup_callback( );
         break;
      }

      /* Must allocate for converted output data to RGB888 */
      /* allocate enought for pnge_input_ipl_image.dy rows of data */
      retCode = pnge_mem_allocate( (void **) &pnge_output_ipl_image.imgPtr, \
                                  3*(pnge_global_params.width*pnge_input_ipl_image.dy) );
      if( retCode != PNGENC_SUCCESS )
      {
         MSG_FATAL(" PNGE: failed to allocate(2) %d bytes", \
                   3*(pnge_global_params.width*pnge_input_ipl_image.dy), 0, 0);
         pnge_status = PNGE_CB_MALLOC_FAILED_ABORT;
         pnge_encode_cleanup_callback( );
         break;
      }

      /* Output of the conversion becomes input to the PNG Encoding */
      pnge_global_params.inputPtr = pnge_output_ipl_image.imgPtr;

      pnge_global_params.pngColorType = PNG_COLOR_TYPE_RGB; /* Request RGB PNG format */
      pnge_global_params.pngBitDepth = DEFAULT_BIT_DEPTH;

      /* init parameters & write chunks before the PLTE and IDAT chunks */
      retCode = pnge_initalize_encode_struct( &pnge_enc_ptr, 
                                             &pnge_global_params );

      if( retCode != PNGENC_SUCCESS )
      {
         MSG_FATAL(" \"pnge_initialize_encode_struct\" call failed %d", retCode, 0, 0);
         pnge_status = PNGE_CB_FAILED_TO_INIT_ABORT;
         pnge_encode_cleanup_callback( );
         break;
      }

      MSG_HIGH("In \"pnge_image_encode\" Struct allocated & inited %d %d", pnge_enc_ptr->width, pnge_enc_ptr->height, 0);
      

      /* queue up a request in GRAPH thread */
      graph_png_encode();
      break;
      /* -------------------------------------------------------- */
   
   case PNGE_PROCESSING:
      /* Here is where the encoding is happening */
      /* it could be a continuous encoding of the entire image */
      /* or it could be a portion of image at a time until it's all done */

      /* When it is completely done (encoding), set the status to CB_DONE */
 

      // Input is already RGB888 - no conversion needed, just
      // memcopy
      if( pnge_input_ipl_image.cFormat == IPL_RGB888 )
      {

         /* See if we are at the end of the image by testing the num rows processed
            with the total height.   Cap the number of rows to convert/process */
         if( ((pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed)) <
                                pnge_output_ipl_image.dy )
         {
   
            pnge_input_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
            pnge_output_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
         }

         // RGB888 is 3 bytes/pixel
         memcpy( pnge_output_ipl_image.imgPtr, 
                 pnge_input_ipl_image.imgPtr, 
                 3 * pnge_global_params.width * pnge_input_ipl_image.dy );
      }

      /* A Bayer format resize is performed here ONLY if the input format is
         1) Bayer and 2) the input/output dimensions are NOT equal */
      else if( ((pnge_global_params.bayerWidth != pnge_global_params.width) ||
                (pnge_global_params.bayerHeight != pnge_global_params.height)) &&
               ((pnge_input_ipl_image.cFormat == IPL_BAYER_BGGR) ||
                (pnge_input_ipl_image.cFormat == IPL_BAYER_GBRG) ||
                (pnge_input_ipl_image.cFormat == IPL_BAYER_GRBG) ||
                (pnge_input_ipl_image.cFormat == IPL_BAYER_RGGB) ))
      {

         /* See if we are at the end of the image by testing the num rows processed
            with the total height.   Cap the number of rows to convert/process */
         if( ((pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed)) <
                                pnge_output_ipl_image.dy )
         {

            pnge_input_ipl_image.dy = ((pnge_global_params.bayerHeight) - \
                                  (((pnge_enc_ptr->numRowsProcessed) / (pnge_output_ipl_image.dy)) * \
                                   (pnge_global_params.bayerNumRowsPerIter) ) );

            pnge_output_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
            pnge_output_bayer_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
         }


         // Call downsize fast for speed
         // Pass NULL as 3rd arg (no BIP structure) so that we are just downsizing,
         // not doing image processing.  Image processing has been found to stripe
         // the output since it is done X number of rows at a time.
         // Do NOT call ipl_downsize since it relies on the entire image being 
         // proessed at the same time

         // Resize and convert from Bayer to YCbCr
         ipl_ret_code = ipl_downsize_fast(&pnge_input_ipl_image, 
                                          &pnge_output_bayer_ipl_image, 
                                          NULL );
         

         if( ipl_ret_code  != IPL_SUCCESS )
         {
            MSG_FATAL("Bayer PNG encode: ipl downsize failed %d", ipl_ret_code, 0, 0);

            /* close output file handle if applicable and deallocate memory */
            pnge_status = PNGE_CB_IPL_FAILURE_ABORT;
            pnge_encode_cleanup_callback( );
            break;
         }

         // Now convert from YCbCr to RGB888
         ipl_ret_code = ipl_convert_to_rgb888( &pnge_output_bayer_ipl_image, &pnge_output_ipl_image );

         if( ipl_ret_code  != IPL_SUCCESS )
         {
            MSG_FATAL("ipl_convert_to_rgb888() returned an error", 0, 0, 0 );
            /* close output file handle if applicable and deallocate memory */
            pnge_status = PNGE_CB_IPL_FAILURE_ABORT;
            pnge_encode_cleanup_callback( );
            break;
         }

      }
      else
      {
         /* See if we are at the end of the image by testing the num rows processed
            with the total height.   Cap the number of rows to convert/process */
         if( ((pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed)) <
                                pnge_output_ipl_image.dy )
         {
   
            pnge_input_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
            pnge_output_ipl_image.dy = (pnge_enc_ptr->height)-(pnge_enc_ptr->numRowsProcessed);
         }

         // Convert just the number of rows we will process here.
         // Do not convert the whole image
         ipl_ret_code = ipl_convert_to_rgb888( &pnge_input_ipl_image, &pnge_output_ipl_image );

         if( ipl_ret_code  != IPL_SUCCESS )
         {
            MSG_FATAL("ipl_convert_to_rgb888() returned an error", 0, 0, 0 );
            /* close output file handle if applicable and deallocate memory */
            pnge_status = PNGE_CB_IPL_FAILURE_ABORT;
            pnge_encode_cleanup_callback( );
            break;
         }


      }

      /* Advance row input pointer to next unconverted input row for
         next encoding iteration, advance by format type to take 
         into account the num bytes/pixel
         Notice the use of the .dy (height) parameter - it equals
         the number of rows we convert and process per call to this
         function and NOT the total rows of the image. */
      pnge_input_ipl_image.imgPtr += pnge_bytes_per_pixel * pnge_input_ipl_image.dx * pnge_input_ipl_image.dy;


      /* Pass global encode struct and num rows per iteration to process
         to encoder.
         Also pass the input row(s) in RGB888 format  */
      (void)pnge_encode_rows( pnge_enc_ptr, pnge_output_ipl_image.dy, pnge_output_ipl_image.imgPtr );

      // Quit when all rows have been processed
      if( pnge_enc_ptr->numRowsProcessed >= pnge_enc_ptr->height )
      {

         /* Return num bytes actually written to output (size of 
            new PNG Encoded image) */
         pnge_handle->length = pnge_enc_ptr->numOutputBytesWritten;

         pnge_status = PNGE_CB_DONE;

         /* close output file handle if applicable and deallocate memory */
         pnge_encode_cleanup_callback( );


      }
      else
      {
         /* it is not done yet so request the graph thread again */
         graph_png_encode();
      }
      break;
      /* -------------------------------------------------------- */

   case PNGE_ABORT:
      pnge_status = PNGE_CB_ABORT;

      /* close output file handle if applicable and deallocate memory */
      pnge_encode_cleanup_callback( );
      break;
      /* -------------------------------------------------------- */


   default:
      
      pnge_callback(PNGE_CB_ABORT, pnge_handle, NULL);
   }

} /* pnge_image_encode */











/*===========================================================================

                 INTERNAL FUNCTION DEFINITIONS

===========================================================================*/



/*===========================================================================

FUNCTION   PNGE_ENCODE_CLEANUP_CALLBACK

DESCRIPTION
  Called to deallocate global malloced data and structures before closing
  down encoding process.  Also calls callback with specified status message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Calls callback with one of the following status types:
  PNGE_ENCODE,
  PNGE_CB_DONE,
  PNGE_CB_ABORT,
  PNGE_INVALID_PARM_ABORT,
  PNGE_CB_INVALID_PARM_ABORT,
  PNGE_INVALID_INPUT_SIZE_PARM,
  PNGE_CB_INVALID_INPUT_SIZE_ABORT,
  PNGE_CB_MALLOC_FAILED_ABORT,
  PNGE_CB_FAILED_TO_INIT_ABORT,
  PNGE_CB_IPL_FAILURE_ABORT,
  PNGE_CB_FREE_FAILED_ABORT,
  PNGE_CB_FS_FAILED_ABORT

============================================================================*/
void pnge_encode_cleanup_callback(void)
{

   fs_rsp_msg_type     fs_resp;  /* Rsponse struct for EFS functions */
   pngeEncRetCodeType retCode = PNGENC_UNKNOWN_ERROR;

   pnge_mem_free( pnge_output_bayer_ipl_image.imgPtr );
   pnge_output_bayer_ipl_image.imgPtr = NULL;
   /* Free format convert output buffer or PNG Enc input buffer */
   pnge_mem_free( pnge_output_ipl_image.imgPtr );
   pnge_output_ipl_image.imgPtr = NULL;
   /* Free temp buffer for YCbCr line packed to YCbCr IPL conversion */
   pnge_mem_free( YCbYCr_ptr);
   YCbYCr_ptr = NULL;
   /* Deallocate dynamically requested memory */
   retCode = pnge_deallocate_mem( pnge_enc_ptr ); /* free dynamically allocated memory */
   pnge_enc_ptr = NULL;
   if( retCode != PNGENC_SUCCESS )
   {
     MSG_FATAL("pnge_deallocate_mem Failed to deallocate memory (in deflate) %d ", retCode,0,0 );
     if (pnge_status == PNGE_CB_DONE) /* then change to error status */
     {
        pnge_status = PNGE_CB_FREE_FAILED_ABORT;
     }
   }
   /* Only manipulate file ptr if we are using a file device as output */
   if( pnge_global_params.outputFormat == FILE_POINTER )
   {
      /* Close output PNG file */
      fs_close( (*pnge_global_params.outputFilePtr), NULL, &fs_resp);

      if (fs_resp.close.status != FS_OKAY_S)
      {
         MSG_FATAL("PNGE Failed to close file handle %d %d", fs_resp.close.status, pnge_status,0 );
         if (pnge_status == PNGE_CB_DONE) /* then change to error status */
         {
            pnge_status = PNGE_CB_FS_FAILED_ABORT;
         }
         else if (pnge_status == PNGE_CB_ABORT && pnge_global_params.filename)
         {
           /* remove partially encoded file if aborted */
           fs_remove(pnge_global_params.filename, NULL, &fs_resp);
         }
      }
      else
      {      
         pnge_global_params.outputFilePtr = NULL;
         if (pnge_status == PNGE_CB_ABORT && pnge_global_params.filename)
         {
           /* remove partially encoded file if aborted */
           fs_remove(pnge_global_params.filename, NULL, &fs_resp);
         }         
      }
      if (pnge_global_params.filename)
      {
         /* free allocated memory for storing the filename */
        pnge_mem_free(pnge_global_params.filename);
        pnge_global_params.filename = NULL;
      }
   }

   if (pnge_callback) 
   {
      pnge_callback(pnge_status, pnge_handle, NULL);
   }

}



/*===========================================================================

FUNCTION      
   pnge_initilize_encode_struct

DESCRIPTION   
   Initialize and allocate all memory for a PNG Encoding.  Alloc
   PNGE structure and fill in contexts with struct passed in.
   Also parse params and write the IHDR chunk (& any others before
   the IDAT chunks).
   Initializes deflate structures after allocating memory for them.

DEPENDENCIES
   None

ARGUMENTS IN
   pngeParmsType structure passed to fill out several parameters listed below
      // mem pointer to entire input image
      uint8 * inputPtr;
      // This is the max num bytes available for the encoded PNG image.
      // Either file OR buffer writes will NOT exceed this number of bytes
      // (even at the expense of not completing the encoding)
      // An error flag is returned if this limit is hit while encoding.
      int32 maxOutputBytesToWrite;
      // Selecting either a file or memory pointer as the output type
      pngeOutputFormatType outputFormat;
      // If selected new PNG image output is written here
      fs_handle_type * outputFilePtr;
      // If selected new PNG image output is written here
      uint8 * outputPtr;
      // Width of input image in pixels
      int32 width;
      // Height of input image in pixels
      int32 height;
      // PNG Standard Color Type
      uint8 pngColorType;
      uint8 pngBitDepth;

ARGUMENTS IN/OUT
   None

ARGUMENTS OUT
   pngEncPtrPtr is pointed to a new allocated structure.

RETURN VALUE
   returns PNGENC_SUCCESS if function completed without errors
   Can fail if mem allocation fails or if illegal encoding mode requested

SIDE EFFECTS
   Mallocs about 300 MB - mostly for large deflate look ahead buffers

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_initalize_encode_struct( pngeEncodeType ** pngEncPtrPtr,
                                                 const pngeParmsType * pnge_global_params_local)
{
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;
   uint32 numComponents;
   //pngDeflateType * pngDeflatePtr;
   pnge_z_stream * pngDeflatePtr;
   pngeEncodeType * pngEncPtr;


   pnge_num_bytes_malloced = 0;
   // these debug vars need to be cleared before too much happens!
#if 0
   // for lack of better place for this ..
   pnge_num_bytes_freed = 0;
#endif



   // Allocate memory for encode struct first
   if ( (retCode = pnge_mem_allocate ( (void **) pngEncPtrPtr, sizeof (pngeEncodeType))) != PNGENC_SUCCESS)
   {

      MSG_ERROR("In \"pnge_initalize_encode_struct\" Struct failed to allocate %d", retCode, 0, 0);
      return (retCode);
   }

   // Get local pointer for this function's use
   pngEncPtr = *pngEncPtrPtr;

   // ------------------------------------------------
   // Load the new structure with basic params from interface struct
   pngEncPtr->height               = pnge_global_params_local->height;
   pngEncPtr->width                = pnge_global_params_local->width;
   pngEncPtr->inputPtr             = pnge_global_params_local->inputPtr;
   pngEncPtr->outputFormat         = pnge_global_params_local->outputFormat;
   pngEncPtr->outputFilePtr        = pnge_global_params_local->outputFilePtr;
   pngEncPtr->outputPtr            = pnge_global_params_local->outputPtr;
   pngEncPtr->colorType            = pnge_global_params_local->pngColorType;
   pngEncPtr->bitDepth             = pnge_global_params_local->pngBitDepth;
   pngEncPtr->maxOutputBytesToWrite = pnge_global_params_local->maxOutputBytesToWrite;

   // Now init helpful params
   pngEncPtr->inputWorkingPtr      = pngEncPtr->inputPtr;  // will point to latest unprocessed row
   pngEncPtr->outputWorkingPtr     = pngEncPtr->outputPtr; // will point to currently empty location.
   pngEncPtr->interlaceMethod      = DEFAULT_INTERLACED;
   pngEncPtr->numRowsProcessed     = 0;          // init to zero - keep track of here
   pngEncPtr->numOutputBytesWritten= 0;          // Num bytes written to output



   if( (pngEncPtr->colorType == PNG_COLOR_TYPE_PALETTE ) ||
       (pngEncPtr->colorType == PNG_COLOR_TYPE_GRAY ))
   {
      // One one component per pixel entry here
      numComponents = 1;
   }
   else if( pngEncPtr->colorType == PNG_COLOR_TYPE_RGB )
   {
      // Components are R, G, & B
      numComponents = 3;
   }
   else if( pngEncPtr->colorType == PNG_COLOR_TYPE_RGB_ALPHA )
   {
      // Add alpha to RGB
      numComponents = 4;
   }
   else if( pngEncPtr->colorType == PNG_COLOR_TYPE_GRAY_ALPHA )
   {
      // Add alpha to grayscale
      numComponents = 2;
   }
   else
   {
      retCode = PNGENC_ENCODE_ROWS_NOT_SUPPORTED;
      MSG_ERROR(" in \"initialize_encode_struct\" color type not supported.  retCode = %d, color ", retCode,pngEncPtr->colorType,0 );
      return( retCode );
   }

   // +7 to round any images with bit depth < 1 byte
   // shift down by 3 to convert num bits to num bytes
   pngEncPtr->bytesPerRow = (( pngEncPtr->width * numComponents * pngEncPtr->bitDepth + 7) >> 3);


   if((pngEncPtr->colorType == PNG_COLOR_TYPE_PALETTE) || (pngEncPtr->bitDepth < 8) )
   {
      pngEncPtr->filterSelectionMask = PNG_FILTER_NONE;
   }
   else
   {
      // Run through default filter selection, set in pngglobal.h
      pngEncPtr->filterSelectionMask = PNG_DEFAULT_FILTER_SELECTION;
   }



   // ------------------------------------------------
   // Now write basic chunk data for PNG header, IHDR
   (void)pnge_write_chunks_before_PLTE( pngEncPtr );


   // ------------------------------------------------
   // Now allocate mem & initialize for image compression support 
   // (filter and deflate)

   // Not sure if this is needed, or if we can just pull from the 
   // input array itself .. (play it safe for now)
   // +1 for filter byte
   retCode = pnge_mem_allocate( (void **) &pngEncPtr->currRowPtr, (pngEncPtr->bytesPerRow)+1 );

   if( retCode != PNGENC_SUCCESS )
   {
      retCode = PNGENC_MEM_ALLOCATE_FAILURE;
      MSG_ERROR("pnge_initalize_encode_struct memory allocation error.  retCode = %d ", retCode,0,0 );
      return( retCode );
   }

   // From pnge_allocate_and_init_filter_scratch a.k.a pnge_write_start_row 
   // in standard LibPNG
   pngEncPtr->currRowPtr[0] = PNG_FILTER_VALUE_NONE;

   // Allocate mem for filters. 
   retCode = pnge_allocate_and_init_filter_scratch( pngEncPtr );
   if( retCode != PNGENC_SUCCESS )
   {
      retCode = PNGENC_MEM_ALLOCATE_FAILURE;
      MSG_ERROR("pnge_initalize_encode_struct memory allocation error 2. retCode = %d ", retCode,0,0 );
      return( retCode );
   }

   // Now allocate deflate structure memory
   retCode = pnge_mem_allocate( (void **) &pngDeflatePtr, sizeof( pnge_z_stream /*pngDeflateType*/ )  );

   if( retCode != PNGENC_SUCCESS )
   {
      retCode = PNGENC_MEM_ALLOCATE_FAILURE;
      MSG_ERROR("pnge_initalize_encode_struct memory allocation error 3.  retCode = %d ", retCode,0,0 );
      return( retCode );
   }
 
   // store reference to deflate struct in pngEnc struct.
   pngEncPtr->pngDeflateStructPtr = pngDeflatePtr;

   // The scratch row ptr is where deflate will write it's output directly
   // This can't necessarily be the output pointer itself but must be an
   // intermediate (scratch) buffer.
   pngEncPtr->scratchRowSize = PNG_SCRATCH_IDAT_CHUNK_SIZE;

   retCode = pnge_mem_allocate( (void **) &(pngEncPtr->scratchRowPtr), pngEncPtr->scratchRowSize );

   if( retCode != PNGENC_SUCCESS )
   {
      retCode = PNGENC_MEM_ALLOCATE_FAILURE;
      MSG_ERROR("pnge_initalize_encode_struct memory allocation error 4.  retCode = %d ", retCode,0,0 );
      return( retCode );
   }

   // init deflate struct
   pngDeflatePtr->total_out = 0;

   retCode = pnge_init_deflate( pngEncPtr );
                       
   if( retCode != PNGENC_SUCCESS )
   {
      retCode = PNGENC_MEM_ALLOCATE_FAILURE;
      MSG_ERROR("pnge_initalize_encode_struct memory allocation error 5.  retCode = %d ", retCode,0,0 );
      return( retCode );
   }

   return (retCode);
}






/*===========================================================================

FUNCTION
   pnge_encode_rows

DESCRIPTION
   Does PNG Encoding on a variable number of rows.  Can encode any number
   of rows.  pngEncPtr struct is passed for all image information.  
   First copy row to internal scratch buffer, then select the row filter,
   then pass to deflate which automatically writes output, if ready.
   Output is written an entire IDAT chunk at a time.  The size of the IDAT
   is configurable in pngglobal.h.
   Finally, if the last row is processed, then the deflate is commanded
   to flush it's output and the final IEND chunk is written in here.

DEPENDENCIES
   Must first call pnge_initilize_encode_struct() to setup the pngeEncodeType
   structure and allocate memory.

ARGUMENTS IN
   pngeEncodeType * pngEncPtr  -  Pointer to PNG Encode structure containing 
      all relevant information for encoding a PNG image.
   int32 numRowsToProcess     - number of rows to process, row width is 
      set in pnge_initilize_encode_struct().
   uint8 * inputRowsPtr       - Pointer to first by of next row to process

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
   returns PNGENC_SUCCESS if function completed without errors

SIDE EFFECTS
   Modifies malloced buffers pointed to by pngEncPtr.  Also collects output
   and may not write anything to output buffers for several rows

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_encode_rows( 
   pngeEncodeType * pngEncPtr, 
   uint32 numRowsToProcess,
   uint8 * inputRowsPtr          )
{
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;
   uint32 rowNum;

   // Use the passed in pointer as the input row, not the internal one.
   // The internal one updates automatically but is only for images
   // that are cached completely in memory (most likely not the case
   // after conversion to RGB888).
   // Only use the internal pointer if this one is passed in NULL
   if( inputRowsPtr != NULL )
   {
      pngEncPtr->inputWorkingPtr = inputRowsPtr;
   }

   // rowNum is relative to where we are starting in the input image
   for( rowNum = 0; rowNum < numRowsToProcess; rowNum++ )
   {

      // Copy input row to scratch working buffer, but skip the first byte
      // of the scratch buffer, this is for the filter type
      // Notice that this memcopy is needed because this row is kept around
      // for processing the next row!
      memcpy( &(pngEncPtr->currRowPtr[1]), pngEncPtr->inputWorkingPtr, pngEncPtr->bytesPerRow );


      // Find best filter for input row by running selected filters
      // returning the best filtered row
      pngEncPtr->filteredRowPtr = pnge_find_filter( pngEncPtr, pngEncPtr->currRowPtr );

      pnge_write_filtered_row( pngEncPtr );


      // increment input pointer to  next row
      pngEncPtr->inputWorkingPtr += pngEncPtr->bytesPerRow;

      pngEncPtr->numRowsProcessed++;
   }

   // actually checks to see if the last row was just processed by using
   // <pngEncPtr->numRowsProcessed>, if so then flush the deflate stream
   // output and end the IDAT chunk
   pnge_write_final_row( pngEncPtr );

   return( retCode );
}






/*===========================================================================

FUNCTION
   pnge_deallocate_mem

DESCRIPTION
   Frees the memory that pnge_initalize_encode_struct() allocated.

DEPENDENCIES
   None

ARGUMENTS IN
   pngeEncodeType * pngEncPtr  -  Pointer to PNG Encode structure containing 
      all relevant information for encoding a PNG image.

ARGUMENTS IN/OUT
   None

ARGUMENTS OUT
   None

RETURN VALUE
   Deflate may return an error if it failed to deallocate ..

SIDE EFFECTS
   About 300k bytes freed!

MODIFIED
   11/12/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_deallocate_mem( pngeEncodeType * pngEncPtr )
{
      
   int32 retCode;
   pngeEncRetCodeType pngEncRetCode = PNGENC_SUCCESS;

   // cleanup memory

   if (pngEncPtr == NULL)
       return pngEncRetCode;

   // deallocate row and filter scratch memory
   pnge_mem_free( pngEncPtr->currRowPtr );

   // NOTE that filtered Row Ptr was never directly allocated,
   // it just points to one of the output filters
   //pnge_mem_free( pngEncPtr->filteredRowPtr );

   pnge_mem_free( pngEncPtr->scratchRowPtr );
   pnge_mem_free( pngEncPtr->prevRowPtr );

   // Only deallocate if it was ever allocated ..
   if (pngEncPtr->filterSelectionMask & PNG_FILTER_SUB)
   {
      pnge_mem_free( pngEncPtr->subFilterPtr );
   }

   if (pngEncPtr->filterSelectionMask & PNG_FILTER_UP)
   {
      pnge_mem_free( pngEncPtr->upFilterPtr );
   }
   if (pngEncPtr->filterSelectionMask & PNG_FILTER_AVG)
   {
      pnge_mem_free( pngEncPtr->avgFilterPtr );
   }
   if (pngEncPtr->filterSelectionMask & PNG_FILTER_PAETH)
   {
      pnge_mem_free( pngEncPtr->paethFilterPtr );
   }

   // deallocate deflate hidden structure
   retCode = pnge_deflateEnd( pngEncPtr->pngDeflateStructPtr );
   if( retCode == Z_STREAM_ERROR )
   {
      pngEncRetCode = PNGENC_DEFLATE_DEALLOCATE_ERROR;
   }

   // Deallocate deflate struct itself
   pnge_mem_free( pngEncPtr->pngDeflateStructPtr );

   pnge_mem_free( pngEncPtr );

   return( pngEncRetCode );

}






/*===========================================================================

FUNCTION
   pnge_write_chunks_before_PLTE

DESCRIPTION
   Writes the first bytes before the PLTE or IDAT chunks.
   Writes the fixed 8 byte PNG header first.  Then 
   calls pnge_write_IHDR to write the IHDR.  All necessary 
   params for IHDR are in the input struct pointed to by 
   pngEncPtr

DEPENDENCIES
   None

ARGUMENTS IN
   pngeEncodeType * pngEncPtr  -  Pointer to PNG Encode structure containing 
      all relevant information for encoding a PNG image.

ARGUMENTS IN/OUT
    None

ARGUMENTS OUT
    None
 
RETURN VALUE
    None

SIDE EFFECTS
   Writes data to memory or file specified in encode struct.

MODIFIED
   11/14/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType  pnge_write_chunks_before_PLTE( pngeEncodeType * pngEncPtr )
{
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;
   uint8 png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

   MSG_HIGH("In \"pnge_write_chunks_before_PLTE\" ", 0, 0, 0);

   // Write 8 fixed bytes of the PNG signature (always starts a
   // PNG file).
   (void)pnge_write_output( pngEncPtr, png_signature, 8 );

   retCode = pnge_write_IHDR( pngEncPtr );

   if( retCode != PNGENC_SUCCESS )
   {
      return (retCode);
   }

   // Insert addional chunks before PLTE and IDAT here:


   return( retCode );
}






/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_write_IHDR( pngeEncodeType * pngEncPtr )
{
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;
   uint8 ihdrChunk[IHDR_CHUNK_SIZE];

   MSG_HIGH("In \"pnge_write_IHDR\" ", 0, 0, 0);

   if( pngEncPtr == NULL )
   {
      return( PNGENC_WRITE_IHDR_FAILURE );
   }

   /* check for width and height valid values */
   if( pngEncPtr->width == 0 || pngEncPtr->height == 0 )
   {
      MSG_ERROR("Image width or height is zero in IHDR", 0, 0, 0);
      return( PNGENC_PARAM_FORMAT_ERROR );
   }

   /* check other values */
   if( (pngEncPtr->bitDepth != 1) && 
       (pngEncPtr->bitDepth != 2) && 
       (pngEncPtr->bitDepth != 4) &&
       (pngEncPtr->bitDepth != 8) && 
       (pngEncPtr->bitDepth != 16) )
   {
      MSG_ERROR("Invalid bit depth in IHDR", 0, 0, 0);
      return( PNGENC_PARAM_FORMAT_ERROR );
   }

   if( pngEncPtr->colorType == 1 ||
       pngEncPtr->colorType == 5 || pngEncPtr->colorType > 6 )
   {
      MSG_ERROR("Invalid color type in IHDR", 0, 0, 0);
      return( PNGENC_PARAM_FORMAT_ERROR );
   }

   if( ((pngEncPtr->colorType == PNG_COLOR_TYPE_PALETTE) && (pngEncPtr->bitDepth > 8) ) ||
       ((pngEncPtr->colorType == PNG_COLOR_TYPE_RGB ||
         pngEncPtr->colorType == PNG_COLOR_TYPE_GRAY_ALPHA ||
         pngEncPtr->colorType == PNG_COLOR_TYPE_RGB_ALPHA  ) && 
        (pngEncPtr->bitDepth < 8) ) )
   {
      MSG_ERROR("Invalid color type/bit depth combination in IHDR", 0, 0, 0);
      return( PNGENC_PARAM_FORMAT_ERROR );
   }


   // pack the header information into the buffer
   // Write chunk size and name, reset CRC
   pnge_write_chunk_start( pngEncPtr, IHDR_CHUNK_SIZE, PNG_IHDR_CHUNK_NAME );

   pnge_store_uint_32(ihdrChunk, pngEncPtr->width);
   pnge_store_uint_32(ihdrChunk+4, pngEncPtr->height);

   ihdrChunk[8]  = pngEncPtr->bitDepth;
   ihdrChunk[9]  = pngEncPtr->colorType;
   ihdrChunk[10] = DEFAULT_COMPRESSION_TYPE;
   ihdrChunk[11] = DEFAULT_FILTER_METHOD;
   ihdrChunk[12] = pngEncPtr->interlaceMethod;

   // Write IHDR data & calculate CRC
   pnge_write_chunk_data( pngEncPtr, ihdrChunk, IHDR_CHUNK_SIZE);
   // Complete IHDR by writting it's CRC
   pnge_write_chunk_end( pngEncPtr );
 
   return( retCode );
}



/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_write_IDAT( pngeEncodeType * pngEncPtr, const uint8 * data, uint32 numBytes )
{

#if 0
// cross check
   uint8 startChunk[8];
   uint32 crc = 0;
#endif

   pngeEncRetCodeType retCode = PNGENC_SUCCESS;

   // Write chunk size and name, reset CRC
   pnge_write_chunk_start( pngEncPtr, (uint32) numBytes, PNG_IDAT_CHUNK_NAME );


   // Write data & calculate CRC
   pnge_write_chunk_data( pngEncPtr, data, numBytes);


   // Complete IDAT by writting it's CRC
   pnge_write_chunk_end( pngEncPtr );

#if 0
// test theory...
memset( data, 0, numBytes );
   // convert chunk size and name to byte buffer
   pnge_store_uint_32(startChunk, PNG_IDAT_CHUNK_NAME);

   // calc CRC across chunk name to start
   crc = pnge_crc32( crc, startChunk, 4);
   crc = pnge_crc32( crc, data, numBytes);

   if( crc != pngEncPtr->crc )
   {
      // We've got a problem
      MSG_FATAL("SPECIAL crc check failed! wanted: %x got %x %x ", 
                pngEncPtr->crc,
                crc,
                (uint8)data[0] );
   }

#endif

   return( retCode );


}








/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/18/03   MVD   Created

===========================================================================*/
pngeEncRetCodeType pnge_write_IEND( pngeEncodeType * pngEncPtr )
{
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;

   // Write chunk size and name, reset CRC
   pnge_write_chunk_start( pngEncPtr, IEND_CHUNK_SIZE, PNG_IEND_CHUNK_NAME );

   // Complete IEND by writting it's CRC
   pnge_write_chunk_end( pngEncPtr );
   return( retCode );


}







/*===========================================================================

FUNCTION

DESCRIPTION
   Write the data of a PNG chunk started with pnge_write_chunk_start().
   Note that multiple calls to this function are allowed, and that the
   sum of the lengths from these calls *must* add up to the total_length
   given to pnge_write_chunk_start().

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
void pnge_write_chunk_start( pngeEncodeType * pngEncPtr, uint32 chunkSize, uint32 chunkName )
{
   uint8 startChunk[8];

   // convert chunk size and name to byte buffer
   pnge_store_uint_32(startChunk, chunkSize);
   pnge_store_uint_32(startChunk+4, chunkName);

   /* write the data, and run the CRC over it */
   pngEncPtr->crc = 0;  // reset CRC for start of new chunk

   // Write the chunk num bytes (4 bytes) and 4 byte chunk name
   (void)pnge_write_output( pngEncPtr, startChunk, 8 );

   // calc CRC across chunk name to start
   pngEncPtr->crc = pnge_crc32( pngEncPtr->crc, startChunk+4, 4);

}




/*===========================================================================

FUNCTION

DESCRIPTION
   Write the data of a PNG chunk started with pnge_write_chunk_start().
   Note that multiple calls to this function are allowed, and that the
   sum of the lengths from these calls *must* add up to the total_length
   given to pnge_write_chunk_start().

DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
void pnge_write_chunk_data( pngeEncodeType * pngEncPtr, const uint8 * data, uint32 numBytes)
{
   /* write the data, and run the CRC over it */
   if (data != NULL && numBytes > 0)
   {
      pngEncPtr->crc = pnge_crc32( pngEncPtr->crc, data, numBytes);

      (void)pnge_write_output( pngEncPtr, data, numBytes );
   }
   else
   {
      MSG_ERROR("Invalid params to \"pnge_write_chunk_data\" ", 0, 0, 0);
   }
}





/*===========================================================================

FUNCTION

DESCRIPTION
   Finish a chunk started with pnge_write_chunk_start().  Write the CRC 
   as the last 4 bytes of the chunk.

DEPENDENCIES
  None 

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
void pnge_write_chunk_end( pngeEncodeType * pngEncPtr )
{
   uint8 buf[4];

   /* write the crc */
   pnge_store_uint_32(buf, pngEncPtr->crc);

   // write 4 byte CRC for this chunk
   (void)pnge_write_output( pngEncPtr, buf, 4  );
}







/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
uint32 pnge_write_output( pngeEncodeType * pngEncPtr, const uint8 * data, uint32 numBytes )
{

   uint32 numBytesWritten  = 0;

   fs_rsp_msg_type fs_rsp;

   if( (pngEncPtr->numOutputBytesWritten + numBytes) > pngEncPtr->maxOutputBytesToWrite)
   {
      // If we are about to exceed the max amount of output storage available then:

      // Cap this output write
      numBytes = (uint32)((pngEncPtr->maxOutputBytesToWrite) - (pngEncPtr->numOutputBytesWritten));

      MSG_ERROR("ERROR in pnge_write_output(). Output write truncated due to max spec %d %d %d",
                   numBytes, 
                   pngEncPtr->maxOutputBytesToWrite,
                   pngEncPtr->numOutputBytesWritten );
   }

   if( pngEncPtr->outputFormat == MEMORY_POINTER )
   {
      numBytesWritten = numBytes;
      memcpy( pngEncPtr->outputWorkingPtr, data, numBytes );

      pngEncPtr->outputWorkingPtr += numBytes; // update location to next free spot

      // Num bytes written to output, including all headers, CRC's etc...
      pngEncPtr->numOutputBytesWritten += numBytes;

   }
   else if( pngEncPtr->outputFormat == FILE_POINTER )
   {
      // NULL as 4th arg means that we are not specifying a callback - so 
      // BLOCK until write is done.
      fs_write( (*pngEncPtr->outputFilePtr), data, numBytes, NULL, &fs_rsp );

      numBytesWritten = fs_rsp.write.count;

      if( (numBytesWritten != numBytes) || (fs_rsp.write.status !=  FS_OKAY_S) )
      {
         MSG_FATAL("ERROR in pnge_write_output(). Status %d num bytes %d written %d ", 
                   fs_rsp.write.status, 
                   numBytes, 
                   numBytesWritten );
         pnge_encode_abort();
      }

      // Num bytes written to output, including all headers, CRC's etc...
      pngEncPtr->numOutputBytesWritten += numBytesWritten;
   }

   return( numBytesWritten);
}









/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/

/* This filters the row, chooses which filter to use, if it has not already
 * been specified by the application, and then writes the row out with the
 * chosen filter.
 */

// FROM LIBPNG:
// void pnge_write_find_filter(png_structp png_ptr, png_row_infop row_info)
uint8 * pnge_find_filter( const pngeEncodeType * pngEncPtr, uint8 * rowPtr )
{
   uint8 * best_row;
   uint32 mins; 
   uint32 bytesPerPixel;


   // find out how many bytes offset each pixel is
   // bytes per row div width in pixels
   bytesPerPixel = pngEncPtr->bytesPerRow / pngEncPtr->width;


   // REVISIT
   best_row = rowPtr;
   mins = PNG_MAXSUM;

   /* The prediction method we use is to find which method provides the
    * smallest value when summing the absolute values of the distances
    * from zero, using anything >= 128 as negative numbers.  This is known
    * as the "minimum sum of absolute differences" heuristic.  Other
    * heuristics are the "weighted minimum sum of absolute differences"
    * (experimental and can in theory improve compression), and the "zlib
    * predictive" method (not implemented yet), which does test compressions
    * of lines using different filter methods, and then chooses the
    * (series of) filter(s) that give minimum compressed data size (VERY
    * computationally expensive).
    *
    * GRR 980525:  consider also
    *   (1) minimum sum of absolute differences from running average (i.e.,
    *       keep running sum of non-absolute differences & count of bytes)
    *       [track dispersion, too?  restart average if dispersion too large?]
    *  (1b) minimum sum of absolute differences from sliding average, probably
    *       with window size <= deflate window (usually 32K)
    *   (2) minimum sum of squared differences from zero or running average
    *       (i.e., ~ root-mean-square approach)
    */


   /* We don't need to test the 'no filter' case if this is the only filter
    * that has been chosen, as it doesn't actually do anything to the data.
    */
   if ((pngEncPtr->filterSelectionMask & PNG_FILTER_NONE) &&
       pngEncPtr->filterSelectionMask != PNG_FILTER_NONE)
   {
      uint8 * rp;
      uint32 sum = 0;
      uint32 i;
      uint32 v;

      for (i = 0, rp = rowPtr + 1; i < pngEncPtr->bytesPerRow; i++, rp++)
      {
         v = *rp;
         sum += (v < 128) ? v : 256 - v;
      }

      mins = sum;
   }

   
   /* sub filter */
   if (pngEncPtr->filterSelectionMask == PNG_FILTER_SUB)
   /* it's the only filter so no testing is needed */
   {
      uint8 * rp, * lp, * dp;
      uint32 i;
      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->subFilterPtr + 1; i < bytesPerPixel;
          i++, rp++, dp++)
      {
         *dp = *rp;
      }
      for (lp = rowPtr + 1; i < pngEncPtr->bytesPerRow;
          i++, rp++, lp++, dp++)
      {
         *dp = (uint8)(((int)*rp - (int)*lp) & 0xff);
      }
      best_row = pngEncPtr->subFilterPtr;
   }

   else if (pngEncPtr->filterSelectionMask & PNG_FILTER_SUB)
   {
      uint8 * rp, * lp, * dp;
      uint32 sum = 0, lmins = mins;
      uint32 i;
      uint8 v;

      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->subFilterPtr + 1; i < bytesPerPixel;
          i++, rp++, dp++)
      {
         v = *dp = *rp;

         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = rowPtr + 1; i < pngEncPtr->bytesPerRow;
          i++, rp++, lp++, dp++)
      {
         v = *dp = (uint8)(((int)*rp - (int)*lp) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         mins = sum;
         best_row = pngEncPtr->subFilterPtr;
      }
   }

   /* up filter */
   if (pngEncPtr->filterSelectionMask == PNG_FILTER_UP)
   {
      uint8 * rp, * dp, * pp;
      uint32 i;

      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->upFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < pngEncPtr->bytesPerRow;
          i++, rp++, pp++, dp++)
      {
         *dp = (uint8)(((int)*rp - (int)*pp) & 0xff);
      }
      best_row = pngEncPtr->upFilterPtr;
   }

   else if (pngEncPtr->filterSelectionMask & PNG_FILTER_UP)
   {
      uint8 * rp, * dp, * pp;
      uint32 sum = 0, lmins = mins;
      uint32 i;
      uint8 v;


      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->upFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < pngEncPtr->bytesPerRow; i++)
      {
         v = *dp++ = (uint8)(((int)*rp++ - (int)*pp++) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         mins = sum;
         best_row = pngEncPtr->upFilterPtr;
      }
   }

   /* avg filter */
   if (pngEncPtr->filterSelectionMask == PNG_FILTER_AVG)
   {
      uint8 * rp, * dp, * pp, *lp;
      uint32 i;
      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->avgFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < bytesPerPixel; i++)
      {
         *dp++ = (uint8)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);
      }
      for (lp = rowPtr + 1; i < pngEncPtr->bytesPerRow; i++)
      {
         *dp++ = (uint8)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2))
                            & 0xff);
      }
      best_row = pngEncPtr->avgFilterPtr;
   }

   else if (pngEncPtr->filterSelectionMask & PNG_FILTER_AVG)
   {
      uint8 * rp, * dp, * pp, *lp;
      uint32 sum = 0, lmins = mins;
      uint32 i;
      uint8 v;

      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->avgFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < bytesPerPixel; i++)
      {
         v = *dp++ = (uint8)(((int)*rp++ - ((int)*pp++ / 2)) & 0xff);

         sum += (v < 128) ? v : 256 - v;
      }
      for (lp = rowPtr + 1; i < pngEncPtr->bytesPerRow; i++)
      {
         v = *dp++ =
             (uint8)(((int)*rp++ - (((int)*pp++ + (int)*lp++) / 2)) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         mins = sum;
         best_row = pngEncPtr->avgFilterPtr;
      }
   }

   /* Paeth filter */
   if (pngEncPtr->filterSelectionMask == PNG_FILTER_PAETH)
   {
      uint8 * rp, * dp, * pp, *cp, *lp;
      uint32 i;
      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->paethFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < bytesPerPixel; i++)
      {
         *dp++ = (uint8)(((int)*rp++ - (int)*pp++) & 0xff);
      }

      for (lp = rowPtr + 1, cp = pngEncPtr->prevRowPtr + 1; i < pngEncPtr->bytesPerRow; i++)
      {
         int a, b, c, pa, pb, pc, p;

         b = *pp++;
         c = *cp++;
         a = *lp++;

         p = b - c;
         pc = a - c;

         pa = p < 0 ? -p : p;
         pb = pc < 0 ? -pc : pc;
         pc = (p + pc) < 0 ? -(p + pc) : p + pc;

         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

         *dp++ = (uint8)(((int)*rp++ - p) & 0xff);
      }
      best_row = pngEncPtr->paethFilterPtr;
   }

   else if (pngEncPtr->filterSelectionMask & PNG_FILTER_PAETH)
   {
      uint8 * rp, * dp, * pp, *cp, *lp;
      uint32 sum = 0, lmins = mins;
      uint32 i;
      uint8 v;

      for (i = 0, rp = rowPtr + 1, dp = pngEncPtr->paethFilterPtr + 1,
           pp = pngEncPtr->prevRowPtr + 1; i < bytesPerPixel; i++)
      {
         v = *dp++ = (uint8)(((int)*rp++ - (int)*pp++) & 0xff);

         sum += (v < 128) ? v : 256 - v;
      }

      for (lp = (rowPtr + 1), cp = (pngEncPtr->prevRowPtr + 1); i < (pngEncPtr->bytesPerRow); i++)
      {
         int32 a, b, c, pa, pb, pc, p;

         b = *pp++;
         c = *cp++;
         a = *lp++;

         p = b - c;
         pc = a - c;
         pa = p < 0 ? -p : p;
         pb = pc < 0 ? -pc : pc;
         pc = (p + pc) < 0 ? -(p + pc) : p + pc;
         p = (pa <= pb && pa <=pc) ? a : (pb <= pc) ? b : c;

         v = *dp++ = (uint8)(((int32)*rp++ - p) & 0xff);

         sum += (v < 128) ? v : 256 - v;

         if (sum > lmins)  /* We are already worse, don't continue. */
            break;
      }

      if (sum < mins)
      {
         best_row = pngEncPtr->paethFilterPtr;
      }
   }

   /* Do the actual writing of the filtered row data from the chosen filter. */

   
   MSG_LOW("\nBest row filter type for row num %d is %d", pngEncPtr->numRowsProcessed, best_row[0],0 );

   return( best_row );

}




/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* Do the actual writing of a previously filtered row. */
pngeEncRetCodeType pnge_init_deflate( const pngeEncodeType * pngEncPtr )
{ 
   pngeEncRetCodeType retCode = PNGENC_SUCCESS;
   int32 zlibStrategy;
   int32 zlibLevel;
   int32 zlibMethod;
   uint32 zlibWindowBits;
   uint32 zlibMemLevel;
   pnge_z_stream * pngDeflatePtr = pngEncPtr->pngDeflateStructPtr;

   MSG_HIGH("In \"pnge_init_deflate\" (allocate %d bytes for deflate state)", sizeof(pnge_deflate_state), 0, 0);
   /* set up the zlib input buffer */


   if (pngEncPtr->filterSelectionMask != PNG_FILTER_NONE)
      zlibStrategy = Z_FILTERED;
   else
      zlibStrategy = Z_DEFAULT_STRATEGY;
  

   //if (!(pngEncPtr->flags & PNG_FLAG_ZLIB_CUSTOM_LEVEL))
   zlibLevel = Z_DEFAULT_COMPRESSION;

   //if (!(pngEncPtr->flags & PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL))
   zlibMemLevel = ZLIB_CONFIG_MEM_LEVEL;
   
   //if (!(pngEncPtr->flags & PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS))
   zlibWindowBits = 15;
   
   //if (!(pngEncPtr->flags & PNG_FLAG_ZLIB_CUSTOM_METHOD))
   zlibMethod = 8;


   (void)pnge_deflateInit2( pngDeflatePtr, 
                 zlibLevel,
                 zlibMethod, 
                 zlibWindowBits,
                 zlibMemLevel, 
                 zlibStrategy    );

   // init scratch output buf and size here  - is reset 
   // in pnge_write_filtered_row() only when it is full and 
   // ready to be written out.
   pngDeflatePtr->next_out = pngEncPtr->scratchRowPtr;
   pngDeflatePtr->avail_out = pngEncPtr->scratchRowSize;

   return( retCode );
}







/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* Do the actual writing of a previously filtered row. */
void pnge_write_filtered_row( pngeEncodeType * pngEncPtr )
{ 

   pnge_z_stream * pngDeflatePtr = pngEncPtr->pngDeflateStructPtr;


   //MSG_HIGH("In \"pnge_write_filtered_row\"; row %d of %d; filter = %d", pngEncPtr->numRowsProcessed+1,pngEncPtr->height, pngEncPtr->filteredRowPtr[0]);
   /* set up the zlib input buffer */

   pngDeflatePtr->next_in = pngEncPtr->filteredRowPtr;
   pngDeflatePtr->avail_in = pngEncPtr->bytesPerRow + 1;

   /* repeat until we have compressed all the data */
   do
   {
      int32 ret; /* return of zlib */

      // compress the data
      ret = pnge_deflate(pngDeflatePtr, Z_NO_FLUSH);

      /* check for compression errors */
      if (ret != Z_OK)
      {
         // Couldn't get this to compile
         //if( pngDeflatePtr->msg != NULL )
         //{
            //MSG_ERROR(pngDeflatePtr->msg,0,0,0);
         //}
         //else
         MSG_FATAL("pnge_write_final_row zlib error",0,0,0);

      }

      /* see if it is time to write another IDAT */
      if( pngDeflatePtr->avail_out == 0 )
      {
         // write the IDAT and reset the zlib output buffer 
         //pnge_write_output(pngEncPtr, pngEncPtr->scratchRowPtr, pngEncPtr->scratchRowSize );
         (void)pnge_write_IDAT(pngEncPtr, pngEncPtr->scratchRowPtr, pngEncPtr->scratchRowSize );
  
         // Rest output scratch pointer to begining because output
         // has been written to IDAT chunk
         pngDeflatePtr->next_out = pngEncPtr->scratchRowPtr;
         pngDeflatePtr->avail_out = pngEncPtr->scratchRowSize;
      }

      /* repeat until all data has been compressed */
   } while (pngDeflatePtr->avail_in);

   // swap the current and previous rows
   // This is quicker than a memcopy - so that the
   // current row becomes "prevRow" for the next iteration
   if (pngEncPtr->prevRowPtr != NULL)
   {
      uint8 * tptr;

      tptr = pngEncPtr->prevRowPtr;
      pngEncPtr->prevRowPtr = pngEncPtr->currRowPtr;
      pngEncPtr->currRowPtr = tptr;
   }

}







/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* Internal use only.  Called when finished processing a row of data. */
void pnge_write_final_row( pngeEncodeType * pngEncPtr )
{

   int32 ret;
   pnge_z_stream * pngDeflatePtr = pngEncPtr->pngDeflateStructPtr;

   // return if this is not the last row in the image
   if( pngEncPtr->numRowsProcessed < pngEncPtr->height )
   {
      return;
   }


   /* if we get here, we've just written the last row, so we need
      to flush the compressor */
   do
   {
      /* tell the compressor we are done */
      ret = pnge_deflate( pngDeflatePtr, Z_FINISH );
      /* check for an error */
      if (ret == Z_OK)
      {
         /* check to see if we need more room */
         if( pngDeflatePtr->avail_out == 0)
         {
            // write the IDAT and reset the zlib output buffer 
            (void)pnge_write_IDAT(pngEncPtr, pngEncPtr->scratchRowPtr, pngEncPtr->scratchRowSize );

            // Rest output scratch pointer to begining because output
            // has been written to IDAT chunk
            pngDeflatePtr->next_out = pngEncPtr->scratchRowPtr;
            pngDeflatePtr->avail_out = pngEncPtr->scratchRowSize;
         }
      }
      else if (ret != Z_STREAM_END)
      {
         //if( pngDeflatePtr->msg != NULL )
         //{
            //MSG_ERROR(pngDeflatePtr->msg,0,0,0);
         //}
         //else
         MSG_ERROR("pnge_write_final_row zlib error",0,0,0);

      }
   } while (ret != Z_STREAM_END);

   /* write any extra space */
   if (pngDeflatePtr->avail_out < pngEncPtr->scratchRowSize )
   {
      (void)pnge_write_IDAT( pngEncPtr, 
                      pngEncPtr->scratchRowPtr, 
                      (pngEncPtr->scratchRowSize - pngDeflatePtr->avail_out) );

   }

   (void)pnge_deflateReset( pngDeflatePtr );

   // write IEND to new PNG file
   (void)pnge_write_IEND( pngEncPtr );

}








/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* initializes the row writing capability of libpng */
pngeEncRetCodeType pnge_allocate_and_init_filter_scratch( pngeEncodeType * pngEncPtr )
{

   pngeEncRetCodeType retCode = PNGENC_SUCCESS;

   /* set up previous row buffer */
   // Allocate memory for PREV filter scratch row
   if ( (retCode = pnge_mem_allocate ( (void **) &pngEncPtr->prevRowPtr, pngEncPtr->bytesPerRow+1 )) != PNGENC_SUCCESS)
   {
      MSG_ERROR("In \"pnge_allocate_and_init_filter_scratch\" failed to allocate 2 %d", retCode, 0, 0);
      return (retCode);
   }

   // init row to zero
   memset(pngEncPtr->prevRowPtr, 0, (pngEncPtr->bytesPerRow+1));


   /* set up filtering buffer, if using this filter */
   if (pngEncPtr->filterSelectionMask & PNG_FILTER_SUB)
   {
      // Allocate memory for SUB filter scratch row
      if ( (retCode = pnge_mem_allocate( (void **) &pngEncPtr->subFilterPtr, pngEncPtr->bytesPerRow+1 )) != PNGENC_SUCCESS)
      {
         MSG_ERROR("In \"pnge_allocate_and_init_filter_scratch\" failed to allocate 1 %d", retCode, 0, 0);
         return (retCode);
      }
      pngEncPtr->subFilterPtr[0] = PNG_FILTER_VALUE_SUB;
   }


   if( pngEncPtr->filterSelectionMask &  PNG_FILTER_UP )
   {
      // Allocate memory for UP filter scratch row
      if ( (retCode = pnge_mem_allocate ( (void **) &pngEncPtr->upFilterPtr, pngEncPtr->bytesPerRow+1 )) != PNGENC_SUCCESS)
      {
         MSG_ERROR("In \"pnge_allocate_and_init_filter_scratch\" failed to allocate 3 %d", retCode, 0, 0);
         return (retCode);
      }
      pngEncPtr->upFilterPtr[0] = PNG_FILTER_VALUE_UP;

   }

   if( pngEncPtr->filterSelectionMask &  PNG_FILTER_AVG )
   {
      // Allocate memory for AVG filter scratch row
      if ( (retCode = pnge_mem_allocate ( (void **) &pngEncPtr->avgFilterPtr, pngEncPtr->bytesPerRow+1 )) != PNGENC_SUCCESS)
      {
         MSG_ERROR("In \"pnge_allocate_and_init_filter_scratch\" failed to allocate 4 %d", retCode, 0, 0);
         return (retCode);
      }
      pngEncPtr->avgFilterPtr[0] = PNG_FILTER_VALUE_AVG;
   }

   if( pngEncPtr->filterSelectionMask &  PNG_FILTER_PAETH )
   {
      // Allocate memory for PAETH filter scratch row
      if ( (retCode = pnge_mem_allocate ( (void **) &pngEncPtr->paethFilterPtr, pngEncPtr->bytesPerRow+1 )) != PNGENC_SUCCESS)
      {
         MSG_ERROR("In \"pnge_allocate_and_init_filter_scratch\" failed to allocate 5 %d", retCode, 0, 0);
         return (retCode);
      }
      pngEncPtr->paethFilterPtr[0] = PNG_FILTER_VALUE_PAETH;
   }

   return( retCode );
}











/*===========================================================================

FUNCTION      PNG_MEM_ALLOCATE

DESCRIPTION   Call malloc to dynamically allocate memory.


DEPENDENCIES
  None

ARGUMENTS IN
  int32 numBytes - number of bytes to allocate

ARGUMENTS IN/OUT
  void ** dest - pointer to newly allocated buffer placed here.

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  Takes memory from the heap.

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
// Insert platform spec ..
pngeEncRetCodeType pnge_mem_allocate( void ** dest, uint32 numBytes )
{


#if 0
   MSG_HIGH("In \"pnge_mem_allocate\" asking for %d bytes.  Total %d", numBytes, pnge_num_bytes_malloced, 0);
#endif

   pnge_num_bytes_malloced += numBytes;

   (*dest) = malloc( numBytes );

   if( (*dest) != NULL )
   {
      return( PNGENC_SUCCESS );
   }

   return( PNGENC_MEM_ALLOCATE_FAILURE );

}






/*===========================================================================

FUNCTION      PNG_MEM_FREE

DESCRIPTION   Calls free and sets ptr to NULL.


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  void * buf - buffer ptr to deallocate

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees memory that was dynamically allocated

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
// Insert platform spec ..
void pnge_mem_free( void * buf )
{

#if 0
   // IS THERE AN EQUIVALENT TO _MSIZE ON THE ARM??
   // _msize returns size in bytes of allocated memory
   //pnge_num_bytes_freed += _msize( buf );
      //MSG_HIGH("In \"pnge_mem_free\" freeing %d bytes; %d still allocated ", 
         //_msize( buf ), 
         //pnge_num_bytes_malloced-pnge_num_bytes_freed, 0);
#endif

   if (buf != NULL)
   {
      free( buf );
   }
}






/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* Place a 32-bit number into a buffer in PNG byte order.  We work
 * with unsigned numbers for convenience, although one supported
 * ancillary chunk uses signed (two's complement) numbers.
 */
void pnge_store_uint_32(uint8 * buf, uint32 value)
{
   buf[0] = (uint8) ((value >> 24) & 0xff);
   buf[1] = (uint8) ((value >> 16) & 0xff);
   buf[2] = (uint8) ((value >> 8) & 0xff);
   buf[3] = (uint8) (value & 0xff);
}


/*===========================================================================

FUNCTION

DESCRIPTION


DEPENDENCIES
  None

ARGUMENTS IN
  None

ARGUMENTS IN/OUT
  None

ARGUMENTS OUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

MODIFIED
  11/12/03   MVD   Created

===========================================================================*/
/* Place a 16-bit number into a buffer in PNG byte order.
 * The parameter is declared unsigned int, not png_uint_16,
 * just to avoid potential problems on pre-ANSI C compilers.
 */
void pnge_store_uint_16(uint8 * buf, uint16 value)
{
   buf[0] = (uint8) ((value >> 8) & 0xff);
   buf[1] = (uint8) (value & 0xff);
}




//////////////////////////////////////////////////////////////////////////


/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2002 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */



#if 0  /* #ifdef DYNAMIC_CRC_TABLE */

local int crc_table_empty = 1;
local uLongf crc_table[256];
local void make_crc_table OF((void));

/*
  Generate a table for a byte-wise 32-bit CRC calculation on the polynomial:
  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^32) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^32 is added mod p to the register if the bit is a one (where
  x^32 mod p is p+x^32 = x^26+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^32 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The table is simply the CRC of all possible eight bit values.  This is all
  the information needed to generate CRC's on data a byte at a time for all
  combinations of CRC register values and incoming bytes.
*/
local void make_crc_table()
{
   uLong c;
   int n, k;
   uLong poly;            /* polynomial exclusive-or pattern */
   /* terms of polynomial defining this crc (except x^32): */
   static const uint8 p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

   /* make exclusive-or pattern from polynomial (0xedb88320L) */
   poly = 0L;
   for (n = 0; n < sizeof(p)/sizeof(uint8); n++)
      poly |= 1L << (31 - p[n]);

   for (n = 0; n < 256; n++)
   {
      c = (uLong)n;
      for (k = 0; k < 8; k++)
         c = c & 1 ? poly ^ (c >> 1) : c >> 1;
      crc_table[n] = c;
   }
   crc_table_empty = 0;
}
#else
/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
static const uint32 crc_table[256] = {
   0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
   0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
   0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
   0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
   0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
   0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
   0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
   0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
   0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
   0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
   0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
   0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
   0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
   0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
   0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
   0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
   0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
   0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
   0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
   0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
   0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
   0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
   0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
   0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
   0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
   0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
   0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
   0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
   0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
   0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
   0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
   0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
   0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
   0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
   0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
   0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
   0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
   0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
   0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
   0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
   0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
   0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
   0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
   0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
   0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
   0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
   0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
   0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
   0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
   0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
   0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
   0x2d02ef8dUL
};
#endif


/* ========================================================================= */
#define DO1(buf) crc = crc_table[((uint32)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
uint32 pnge_crc32( uint32 crc, const uint8 *buf, uint32 len)
{
   if (buf == NULL) 
      return 0;
#if 0  /* #ifdef DYNAMIC_CRC_TABLE */
   if (crc_table_empty)
      make_crc_table();
#endif
   crc = crc ^ 0xffffffffUL;
   while (len >= 8)
   {
      DO8( (uint8 *) buf);
      len -= 8;
   }
   if (len) do
      {
         DO1(buf);
      } while (--len);
   return crc ^ 0xffffffffUL;
}

/*===========================================================================

FUNCTION      pnge_swap_cbcr

DESCRIPTION
              Swap in-place Cr and Cb that are in line packed format

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pnge_swap_cbcr(uint8 *cbcr_ptr, uint32 len)
{
    uint8 tmp;
    uint32 i;

    for (i=0;i<len;i++)
    {
        tmp = *cbcr_ptr;
        *cbcr_ptr = *(cbcr_ptr+1);
        *(cbcr_ptr+1) = tmp;
        cbcr_ptr+=2;
    }
}


#endif /* FEATURE_PNG_ENCODER   */

