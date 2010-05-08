/*****************************************************************************

            C A M E R A   S V C S   D I A G

            This defines the C-side of RPC involving
            Perl+PC and AMSS+Phone.
            The incipient effort relied heavily on
            ftm_camera.{c,h}, and may reuse some of
            those files' code.


         Copyright (c) 2005 by QUALCOMM INCORPORATED.
                       All Rights Reserved.

                    EDIT HISTORY FOR FILE
              (in reverse chronological order)
1336

  $Header: //source/qcom/qct/multimedia/camera/6k/diag/main/latest/src/camera_svcs_diag.c#3 $
  $DateTime: 2009/02/25 02:29:46 $ 
  $Author: c_caluru $
 
  This #1 = //depot/asic/msm6550/services/camera/test/camera_svcs_diag.c#28 .
  This was formerly under msmshared, version 7, until June 28 2005.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/09  cg      Fixed certain issues with EzTune1.0 ( Dog Timeout while sending all the parameters , Gamma Table read and write issue through chromatix,
                  Preview becomes reddish while trying to send all the paramters , Enabling/Disabling for the VFE blocks) 
04/04/08   kd      Added support for mobicat preview over Chromatix for PC
04/02/08   hap     Fixed Klocwork errors
01/23/08   nshree  The fix for the CR#131723 by returning the correct response to 
                   the DIAG after completing the camera stop operation.
12/18/07   kdiv    Added support for YCbCr Jpegd DSP output
11/08/07   hap     Fixing RVCT2.2 compiler warnings
08/03/07   vma     Report to Jpeg Unit Test Unit the output dimension
06/14/07   jpg     Fixed compiler warnings if FEATURE_CAMERA_MOBICAT not defined.
05/29/07   jpg     Handlers for new data exchange protocol for PC QMobiCaT
05/22/07   vma     Added Jpeg Decoder Resize feature
03/27/07   vma     Replaced banned strncpy API with standard API strlcpy
02/26/07   vma     Removed custcamera.h to properly include features only from
                   customer.h
02/26/07   vma     Fixed typo CAMERA_FEATURE_LPM -> FEATURE_CAMERA_LPM
02/07/07   vma     Wrapped LPM related addition inside FEATURE_CAMERA_LPM
11/17/06   vma     LPM unit testing
09/15/06   vma     Removed checking of encode quality (not limited to 50,75,90)
06/20/06   vma     Added target.h and customer.h include to fix compilation error
06/12/06   vma     Added FEATURE_CAMERA
06/01/06   vma     Made jpegd output format, and arm/dsp based programmable
05/29/06   vma     Added support for transferring images to/from RAM directly
05/24/06   vma     Cleaned up the code by removing diag event reporting calls
05/19/06   vma     Added more programmable parammeters for encoding
                   Added query routine for test script to query status/stats
11/10/05   jj      Replaced in subsidiary directory by request.
07/22/05   jj      Rewriting comments to QCT standard form...
07/05/05   jj      deriving from services/diag/toolsdiag.c per
                   Saritha Alluri's tip.  Holds commented versions
                   of code from ftm_camera.c and from our camera svcs
                   extensions to ftm_camera.c.
*****************************************************************************/

#include "target.h"
#include "customer.h"
#ifdef FEATURE_CAMERA

char * camera_svcs_diag_revision = __DATE__ ;  //  C preprocessor defines that

#ifdef   FEATURE_CAMERA_SVCS_DIAG
//lint --e{715}  Unreferenced params in callbacks of an established API.
//lint --e{818}  Const pointer possibility broke compilation.
//lint --e{754}  Struct member not referenced?  Look where it's passed..
//lint --e{550}  Parameter from established API not referenced.

#include "fs_public.h"
#include "camera.h"
#include "stdlib.h"
#include "jpeg.h"
#include "jpegd.h"
#include "jpege.h"
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "camera_svcs.h"
#include "camera_svcs_diag.h"
#include "lpm.h"
#ifdef CAMERA_USES_LIBSTD
#include "AEEstd.h"
#endif /* CAMERA_USES_LIBSTD */

#ifdef  FEATURE_CAMERA_MOBICAT
#include "camera_tune.h"
#include <stdio.h>
#ifdef FEATURE_CAMERA_MOBICAT_PREVIEW
#include "mobicat_preview.h"
#endif //FEATURE_CAMERA_MOBICAT_PREVIEW
#endif


/*****************************************************************************
  DECLARATIONS ( including those by #define )

  We start the opcode numbering at 1000 because there
  are around 100 opcodes allocated in early 2003 by an
  incipient, then aborted, effort, under this DIAG subsystem id.
*****************************************************************************/


#define DIAG_SUBSYS_CAMERA_SVCS  ((int)(DIAG_SUBSYS_QCAMERA))


#define  PING                            (1000)
#define  JPEG_ENC_ABORT                  (1001)
#define  JPEG_DEC_ABORT                  (1002)
#define  JPEG_ENC_EFS_TO_EFS             (1003)
#define  JPEG_DEC_EFS_TO_EFS             (1004)
#define  EFS_TO_RAM_TO_EFS               (1005)
#define  IPL_ENABLE_SOLARIZE             (1006)
#define  IPL_DEFAULT_GAMMA               (1007)
#define  CAMERA_START                    (1008)
#define  CAMERA_STOP                     (1009)
#define  CAMERA_PREVIEW_START            (1010)
#define  CAMERA_PREVIEW_STOP             (1011)
#define  CAMERA_TUNE_CONFIG_FILE_READ    (1012)
#define  CAMERA_TUNE_CONFIG_FILE_WRITE   (1013)
#define  CAMERA_TUNE_GET_LIST            (1014)
#define  CAMERA_TUNE_GET_PARMS           (1015)
#define  CAMERA_TUNE_SET_PARMS           (1016)
#define  JPEG_TEST_QUERY                     (1017)
#define  JPEG_LOAD_FILE_TO_RAM           (1018)
#define  JPEG_GET_FILE_FROM_RAM          (1019)
#define  LPM_START                       (1020)
#define  CAMERA_TUNE_MISC_COMMAND        (1021)
#define  CAMERA_SET_ZOOM_LEVEL           (1022)
#define  CAMERA_MOBICAT_PREVIEW_ACK      (1023)
#define  CAMERA_SET_DIMENSIONS           (1024)
#define  CAMERA_MOBICAT_PREVIEW_START    (1025)
#define  CAMERA_MOBICAT_PREVIEW_STOP     (1026)
//#define  CAMERA_MOBICAT_PREVIEW_BENCHMARK (1027)

typedef PACKED void * (diag_opcode) ( PACKED void * , uint16 );

diag_opcode  camera_svcs_diag_ping                  ;
diag_opcode  camera_svcs_diag_jpeg_encoder_abort    ;
diag_opcode  camera_svcs_diag_jpeg_decoder_abort    ;
diag_opcode  camera_svcs_diag_jpeg_encode           ;
diag_opcode  camera_svcs_diag_jpeg_decode           ;
diag_opcode  camera_svcs_diag_efs_to_ram_to_efs     ;
diag_opcode  camera_svcs_diag_ipl_enable_solarize   ;
diag_opcode  camera_svcs_diag_ipl_default_gamma     ;
diag_opcode  camera_svcs_diag_camera_start          ;
diag_opcode  camera_svcs_diag_camera_stop           ;
diag_opcode  camera_svcs_diag_camera_preview_start  ;
diag_opcode  camera_svcs_diag_camera_preview_stop   ;
//  diag_opcode  camera_svcs_diag_camera_tune_set_value ;  //  stalled
//  diag_opcode  camera_svcs_diag_camera_tune_get_value ;  //  stalled
//  diag_opcode  camera_svcs_diag_camera_tune_describe_registry ;  // stalled
diag_opcode  camera_svcs_diag_camera_tune_config_file_read    ;
diag_opcode  camera_svcs_diag_camera_tune_config_file_write   ;
diag_opcode  camera_svcs_diag_camera_tune_get_list ;
diag_opcode  camera_svcs_diag_camera_tune_get_parms  ;
diag_opcode  camera_svcs_diag_camera_tune_set_parms  ;
diag_opcode  camera_svcs_diag_camera_tune_misc_command  ;
diag_opcode  camera_svcs_diag_jpeg_test_query                     ;
diag_opcode  camera_svcs_diag_jpeg_load_file_to_ram               ;
diag_opcode  camera_svcs_diag_jpeg_get_file_from_ram              ;
#ifdef FEATURE_CAMERA_LPM 
#error code not present
#endif 

diag_opcode  camera_svcs_diag_camera_set_zoom       ;
diag_opcode  camera_svcs_diag_mobicat_preview_ack   ;
diag_opcode  camera_svcs_diag_camera_set_dimensions ;
diag_opcode  camera_svcs_diag_mobicat_preview_start ;
diag_opcode  camera_svcs_diag_mobicat_preview_stop  ; 
//diag_opcode  camera_svcs_diag_mobicat_preview_benchmark ;

#define ENCODETHISFILE   "/camera_test/jpeg_encoder"
#define DECODETHISFILE   "/camera_test/jpeg_decoder"
#define RAMFILESINK      "/camera_test/ramfilesink"

/* The allocated RAM to hold encoded/decoded file: 10Mbytes */
#define READ_FROM_EFS            0
#define READ_FROM_RAM            1
#define WRITE_TO_EFS             0
#define WRITE_TO_RAM             1

#define CAMERA_TUNE_PC_QMOBICAT_TEMP_FILE_NAME  "saved.ini"


/*****************************************************************************
  DEFINITIONS

  (data and functions...)

  This table of callbacks is registered by camera_svcs_diag_init().
*****************************************************************************/


static const diagpkt_user_table_entry_type 
             camera_svcs_diag_func_table [] =
{
  {                  PING ,
                     PING ,
    camera_svcs_diag_ping 
  },

  {                  JPEG_ENC_ABORT ,
                     JPEG_ENC_ABORT ,
    camera_svcs_diag_jpeg_encoder_abort
  },

  {                  JPEG_DEC_ABORT ,
                     JPEG_DEC_ABORT ,
    camera_svcs_diag_jpeg_decoder_abort
  },

  {                  JPEG_ENC_EFS_TO_EFS ,
                     JPEG_ENC_EFS_TO_EFS ,
    camera_svcs_diag_jpeg_encode
  },

  {                  JPEG_DEC_EFS_TO_EFS ,
                     JPEG_DEC_EFS_TO_EFS ,
    camera_svcs_diag_jpeg_decode
  },

  {                  EFS_TO_RAM_TO_EFS ,
                     EFS_TO_RAM_TO_EFS ,
    camera_svcs_diag_efs_to_ram_to_efs
  },

  {                  IPL_ENABLE_SOLARIZE ,
                     IPL_ENABLE_SOLARIZE ,
    camera_svcs_diag_ipl_enable_solarize
  },

  {                  IPL_DEFAULT_GAMMA ,
                     IPL_DEFAULT_GAMMA ,
    camera_svcs_diag_ipl_default_gamma
  },

  {                  CAMERA_START ,
                     CAMERA_START ,
    camera_svcs_diag_camera_start
  },

  {                  CAMERA_STOP ,
                     CAMERA_STOP ,
    camera_svcs_diag_camera_stop
  },

  {                  CAMERA_PREVIEW_START,
                     CAMERA_PREVIEW_START,
    camera_svcs_diag_camera_preview_start
  },

  {                  CAMERA_PREVIEW_STOP ,
                     CAMERA_PREVIEW_STOP ,
    camera_svcs_diag_camera_preview_stop
  },

  {                  CAMERA_TUNE_CONFIG_FILE_READ ,
                     CAMERA_TUNE_CONFIG_FILE_READ ,
    camera_svcs_diag_camera_tune_config_file_read
  },

  {                  CAMERA_TUNE_CONFIG_FILE_WRITE ,
                     CAMERA_TUNE_CONFIG_FILE_WRITE ,
    camera_svcs_diag_camera_tune_config_file_write
  },

  {                  CAMERA_TUNE_GET_LIST ,
                     CAMERA_TUNE_GET_LIST ,
    camera_svcs_diag_camera_tune_get_list
  },

  {                  CAMERA_TUNE_GET_PARMS ,
                     CAMERA_TUNE_GET_PARMS ,
    camera_svcs_diag_camera_tune_get_parms
  },

  {                  JPEG_TEST_QUERY ,
                     JPEG_TEST_QUERY ,
    camera_svcs_diag_jpeg_test_query
  },

  {                  JPEG_LOAD_FILE_TO_RAM ,
                     JPEG_LOAD_FILE_TO_RAM ,
    camera_svcs_diag_jpeg_load_file_to_ram
  },

  {                  JPEG_GET_FILE_FROM_RAM ,
                     JPEG_GET_FILE_FROM_RAM ,
    camera_svcs_diag_jpeg_get_file_from_ram
  },
#ifdef FEATURE_CAMERA_LPM 
#error code not present
#endif 

  {                  CAMERA_TUNE_SET_PARMS ,
                     CAMERA_TUNE_SET_PARMS ,
    camera_svcs_diag_camera_tune_set_parms
  },

  {                  CAMERA_TUNE_MISC_COMMAND ,
                     CAMERA_TUNE_MISC_COMMAND ,
    camera_svcs_diag_camera_tune_misc_command
  },

  {
                     CAMERA_SET_ZOOM_LEVEL,
                     CAMERA_SET_ZOOM_LEVEL,
    camera_svcs_diag_camera_set_zoom
  },

  {                  
                     CAMERA_MOBICAT_PREVIEW_ACK,
                     CAMERA_MOBICAT_PREVIEW_ACK,
    camera_svcs_diag_mobicat_preview_ack
  },

  {
                     CAMERA_SET_DIMENSIONS,
                     CAMERA_SET_DIMENSIONS,
    camera_svcs_diag_camera_set_dimensions
  },
  
  {
                     CAMERA_MOBICAT_PREVIEW_START,
                     CAMERA_MOBICAT_PREVIEW_START,
    camera_svcs_diag_mobicat_preview_start
  },
  
  
  {
                     CAMERA_MOBICAT_PREVIEW_STOP,
                     CAMERA_MOBICAT_PREVIEW_STOP,
    camera_svcs_diag_mobicat_preview_stop
  },
/*
  {
                     CAMERA_MOBICAT_PREVIEW_BENCHMARK,
                     CAMERA_MOBICAT_PREVIEW_BENCHMARK,
    camera_svcs_diag_mobicat_preview_benchmark
  },
  */
};


/** Loading a file, in EFS, to memory requires a buffer. **/

static  void *  buffer_needing_liberation = NULL;


JPEGENC_encodeSpecType   encodersettings  ;
exif_info_type           jpege_test_exif_info;

jpeg_decode_input_type   decodersettings  ;
jpeg_decode_output_type  decoderresults   ;

static uint8             jpege_in_progress ;
static uint8             jpegd_in_progress ;
static uint32            jpege_time_started;
static uint32            jpege_encode_time ;
static uint32            jpegd_time_started;
static uint32            jpegd_decode_time ;
static uint8*            input_file_in_ram ;
static uint32            input_file_size   ;
static uint8*            output_file_in_ram;
static uint32            output_file_size  ;
static uint32            encode_data_dst   ;
static uint32            decode_data_dst   ;
static uint32            output_malloc_size;
static uint32            jpegd_output_format;
static uint16            jpegd_output_width;
static uint16            jpegd_output_height;
static uint32            dump_ram_to_efs_time;
static uint32            dump_ram_to_efs_in_progress;
static fs_open_xparms_type    open_parms;
static fs_rsp_msg_type        fs_rsp;
static fs_handle_type         fs_handle;
static uint8                  file_close_pending;

#ifdef FEATURE_CAMERA_LPM 
#error code not present
#endif /* FEATURE_CAMERA_LPM */ 

/*****************************************************************************
  This routine needs to be called from camera_svcs_init() or similar.
*****************************************************************************/


void camera_svcs_diag_init ()
{
  static boolean camera_svcs_diag_registered = FALSE;

  if (!camera_svcs_diag_registered)
  {
    camera_svcs_diag_registered = TRUE;
    DIAGPKT_DISPATCH_TABLE_REGISTER ( DIAG_SUBSYS_CAMERA_SVCS ,
                                      camera_svcs_diag_func_table
                                    );
  }
}


/*****************************************************************************
* 
*   ipl_enable_solarize
* 
*   tells IPL to enable the "solarize" special effect.
* 
*****************************************************************************/


typedef PACKED struct { diagpkt_subsys_header_type  unused; }
        IPL_ENABLE_SOLARIZE_RESPONSE;


PACKED void *  camera_svcs_diag_ipl_enable_solarize ( PACKED void * request , uint16 length )
{
  PACKED void * response;

  (void)camera_enable_solarize();

  response = (PACKED void *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , IPL_ENABLE_SOLARIZE
                                 , sizeof( IPL_ENABLE_SOLARIZE_RESPONSE )
                                 );
  return response;
}


/*****************************************************************************
* 
*   ipl_default_gamma                
* 
*   tells IPL to return to a normal gamma table.
* 
*****************************************************************************/


typedef PACKED struct { diagpkt_subsys_header_type  unused; }
        IPL_DEFAULT_GAMMA_RESPONSE;


PACKED void *  camera_svcs_diag_ipl_default_gamma ( PACKED void * request , uint16 length )
{
  PACKED void * response;

  (void)camera_default_gamma   ();

  response = (PACKED void *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , IPL_DEFAULT_GAMMA
                                 , sizeof( IPL_DEFAULT_GAMMA_RESPONSE )
                                 );
  return response;
}


/*****************************************************************************
* 
*   jpeg_enc_abort
* 
*   tells the jpeg encoder to abort.
* 
*****************************************************************************/


typedef PACKED struct { diagpkt_subsys_header_type  unused; }
               JPEG_ENC_ABORT_RESPONSE;


PACKED void *  camera_svcs_diag_jpeg_encoder_abort  ( PACKED void *  request , uint16 length )
{
  PACKED void * response;

  jpeg_encoder_abort( 0 );  //  has return type void
 
  response = (PACKED void *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_ENC_ABORT
                                 , sizeof( JPEG_ENC_ABORT_RESPONSE )
                                 );
  return response;
}


/*****************************************************************************
* 
*   jpeg_dec_abort
* 
*   tells the jpeg decoder to abort.
* 
*****************************************************************************/


typedef PACKED struct { diagpkt_subsys_header_type  unused; }
               JPEG_DEC_ABORT_RESPONSE;


PACKED void *  camera_svcs_diag_jpeg_decoder_abort  ( PACKED void * request , uint16 length )
{
  PACKED void * response;

//jpeg_decode_image_abort();  //  has return type void  //  ARM decoder abort, that is...
  jpegd_decoder_abort( 0 );   //  has return type void  //  DSP decoder abort, that is...
 
  response = (PACKED void *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_DEC_ABORT
                                 , sizeof( JPEG_DEC_ABORT_RESPONSE )
                                 );
  return response;
}


/****************************************************************************/


int efs_to_memory (
                    char           *  filename      //  inbound  argument
                  , unsigned char **  fileinmemory  //  outbound return value
                  , unsigned int   *  size          //  outbound return value
                  )
{
  struct fs_stat  fileinfo;
  int             r;                    // return value
  int             f;                    // file descriptor (or handle)

  //
  // read the file's size.
  //

  r = efs_stat( filename , &fileinfo );
  if (r != 0)                           //  then stat failed
     return r;
  *size = fileinfo.st_size ;

  //
  // now we've retrieved the file's size and stored it in *size.
  // so, we'll malloc memory in which to store it.
  //
  if (buffer_needing_liberation != NULL)
     {
       free( buffer_needing_liberation );
       buffer_needing_liberation = NULL;
     }
  *fileinmemory = (unsigned char *)( malloc(*size) );
  buffer_needing_liberation = *fileinmemory;
  if (*fileinmemory == NULL)
     return 1;                          //  malloc failed

  //
  // now we have memory in which to store the *size bytes of file filename.
  // so, we need to open the file and read the *size bytes of it in.
  //

  f = efs_open( filename , O_RDONLY );
  if (f == (-1))
     return 2;                          //  open failed

  //
  // open worked, so read it in..
  //

  {
    //  don't side-effect the in-out parameters to this function
    //  who belong to the caller, not this, the called.

    unsigned int    s = *size;
    unsigned char * w = *fileinmemory;

    r = efs_read( f , w , s );
    if (r == (-1))
    {
      return r;  //  i.e., (-1).
    }

    while ((unsigned int)r < s)
    {
      w += r;
      s -= (unsigned int)r;
      r = efs_read( f , w , s );
      if (r == (-1))
      {
        return r;  //  i.e., (-1).
      }
    }

    if ((unsigned int)r != s)                       //  read didn't read everything...thus failed.
    {
      (void)efs_close(f);
      return 3;
    }
  }

  //
  // everything worked, so return 0 as the success code...and the in_out arguments
  // hold the file's address and size.
  //
  // NOTE:  THIS MALLOCED MEMORY...THE CALLER IS RESPONSIBLE FOR RELEASING IT LATER.
  //

  (void)efs_close(f);
  return 0;   // NOTE:  this malloced memory...caller responsible for releasing it later.
}


/****************************************************************************/


int memory_to_efs (
                    char          *  filename      //  inbound argument
                  , unsigned char *  fileinmemory  //  inbound argument
                  , unsigned int     size          //  inbound argument
                  )
{
  int  r,  //  return value
       f;  //  file descriptor (or handle)

  //
  // open the file into which to write the size # of unsigned chars from memory.
  //

  f = efs_open( filename , O_CREAT | O_TRUNC | O_WRONLY );
  if (f == (-1))
     return 2;                          //  open failed

  //
  // open worked, so write the unsigned chars from memory to the file
  //

  r = efs_write( f , (void *)fileinmemory , size );
  if (r == (-1))
  {
    return r;
  }

  while ((unsigned int)r < size)
  {
    fileinmemory += r;
    size         -= (unsigned)r;
    r = efs_write( f , (void *)fileinmemory , size );
    if (r == (-1))
    {
      return r;
    }
  }

  if ((unsigned int)r != size)     //  read didn't read everything...thus failed.
  {
    (void)efs_close (f);
    return 3;
  }

  //
  // open and write worked with little effort,
  // close the file and return "success".
  //

  r = efs_close(f);
  return r;
}


/*****************************************************************************
* 
*   jpeg_test_encoder_cb
* 
*   is used by the jpeg services to report results...
*   the example provided by the JPEG guys relied on BREW, which I and
*   this test suite can NOT, so this function is a no-op.
* 
*****************************************************************************/


void camera_svcs_diag_jpeg_test_encoder_cb ( JPEGENC_CBrtnType * rtnValues )
{
  MSG_HIGH ("camera_jpeg_encode_callback", 0, 0, 0);   

  if (rtnValues != NULL)
  {
    switch (rtnValues->status)
    {
      case JPEGENC_IMG_DONE:
        jpege_encode_time = timetick_get_ms() - jpege_time_started;
        if (encode_data_dst == WRITE_TO_RAM)
        {
          output_file_size = rtnValues->size;
        }
        jpege_in_progress = 0;
        MSG_HIGH( "JPEG ENCODER CB:  DONE", 0,0,0 );
        break;
      case JPEGENC_IMG_ABORT:
        jpege_in_progress = 0;
        MSG_HIGH( "JPEG ENCODER CB:  ABORT", 0,0,0 );
        break;

      default:
        MSG_HIGH( "JPEG ENCODER CB:  default", 0,0,0 );
        break;
     }
  }
}


/*****************************************************************************
* 
*   jpeg_enc_efs_to_efs
* 
*   tells the jpeg encoder to encode a raw file from efs to a jpeg in efs.
* 
*****************************************************************************/

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         uint32                       width    ;
                         uint32                       height   ;
                         uint32                       encoding ;  //  0 = H2V2 ; 1 = H2V1
                         uint32                       quality  ;
                         uint32                       restart  ;
                         uint32                       rotation ;  //  0 = JPEGENC_NO_ROT;
                                                                  //  1 = JPEGENC_CCLK90
                                                                  //  2 = JPEGENC_CCLK180
                                                                  //  3 = JPEGENC_CCLK270
                         uint32                       data_src ;  
                         uint32                       data_dst ;  
                      }  JPEG_ENC_EFS_TO_EFS_REQUEST  ;

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  JPEG_ENC_EFS_TO_EFS_RESPONSE ;


PACKED void *  camera_svcs_diag_jpeg_encode ( PACKED void * request, uint16 length )
{
  JPEG_ENC_EFS_TO_EFS_RESPONSE *  response        ;
  int                             status          ;
  unsigned char *                 rawfileinmemory ;
  unsigned int                    rawfilesize     ;
  uint32                          width    = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->width    ;
  uint32                          height   = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->height   ;
  uint32                          encoding = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->encoding ;
  uint32                          quality  = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->quality  ;
  uint32                          rotation = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->rotation ;
  uint32                          restart  = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->restart  ;
  uint32                          data_src = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->data_src ;

  encode_data_dst                          = ((JPEG_ENC_EFS_TO_EFS_REQUEST*)request)->data_dst ; 
  response = (JPEG_ENC_EFS_TO_EFS_RESPONSE *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_ENC_EFS_TO_EFS
                                 , sizeof( JPEG_ENC_EFS_TO_EFS_RESPONSE )
                                 );
  if(response == NULL)
  {
    return response;
  }

  if (jpege_in_progress || jpegd_in_progress)
  {
    response->result = 1; 
    MSG_HIGH ( "Requesting JPEG encode while another session is in progress. Decline request" , 0,0,0) ;
    return response;
  }
  // validate parameter passed in
  if (((encoding != 0) && (encoding != 1)) || (rotation > 3))
  {
    response->result = 1;
    MSG_HIGH ( "Invalid parameter passed in to encode", 0,0,0);
    return response;
  }
  if (data_src == READ_FROM_RAM && input_file_in_ram == NULL) /* read from ram */
  {
    response->result = 1;
    MSG_HIGH ( "Load image to memory first", 0,0,0);
    return response;
  }
  if (encode_data_dst == WRITE_TO_RAM && output_file_in_ram == NULL) /* write to ram */
  {
    response->result = 1;
    MSG_HIGH ( "Allocate space to hold encoded image", 0,0,0);
    return response;
  }
  if (data_src == READ_FROM_EFS) /* read from efs */
  {
    if (input_file_in_ram != NULL)
    {
      free(input_file_in_ram);
      input_file_in_ram = NULL;
    }
    status = efs_to_memory(ENCODETHISFILE ".raw", &rawfileinmemory, &rawfilesize);
    if ( status )
    {
      response->result = (char)status;  
      MSG_HIGH( "efs_to_memory failure %d" , status , 0 , 0 );
      return response;
    }
  }
  else
  {
    rawfileinmemory = input_file_in_ram;
  }

  jpege_in_progress = 1;
  exif_init_info( &jpege_test_exif_info );

  /* This encode spec is global; it does not go away. */
  encodersettings.ClientId                   = 12;  //  copied from camera_svcs, could use 0 ?
  encodersettings.CallBack                   = camera_svcs_diag_jpeg_test_encoder_cb;
  /* write to efs */
  if (encode_data_dst == WRITE_TO_EFS)
  {
    encodersettings.Dest.device              = JPEGENC_EFS;
#ifdef CAMERA_USES_LIBSTD
    (void)std_strlcpy(encodersettings.Dest.handle.efs.filename, (ENCODETHISFILE ".jpg"), FS_FILENAME_MAX_LENGTH_P);
#else /* CAMERA_USES_LIBSTD */
    (void)strncpy(encodersettings.Dest.handle.efs.filename, (ENCODETHISFILE ".jpg"), FS_FILENAME_MAX_LENGTH_P);
#endif /* CAMERA_USES_LIBSTD */
  }
  else /* write to ram */
  {
    encodersettings.Dest.device              = JPEGENC_MEM;
    encodersettings.Dest.handle.mem.buffer   = output_file_in_ram;
    encodersettings.Dest.handle.mem.length   = output_malloc_size;
  }
  encodersettings.Rotation                   = (JPEGENC_OrientType)rotation ;
  encodersettings.Main.Quality               = (uint16) quality ;
  encodersettings.Main.Width                 = (uint16) width  ;
  encodersettings.Main.Height                = (uint16) height ;
  encodersettings.Main.Restart               = (uint16) restart ;
  encodersettings.Main.Subsample             = ( (encoding == 0) ? (JPEGENC_H2V2) : (JPEGENC_H2V1) );
  encodersettings.Main.FragCnt               = 1 ;
  encodersettings.Main.Fragmnt[0].Height     = (uint32)height ;
  encodersettings.Main.Fragmnt[0].Luma_ptr   = rawfileinmemory                ;
  encodersettings.Main.Fragmnt[0].Chroma_ptr = rawfileinmemory + width*height ;
  encodersettings.Main.OrigWidth             = (uint16) width  ;
  encodersettings.Main.OrigHeight            = (uint16) height ;
  encodersettings.HasThumbnail               = FALSE ;
  
  //status = jpege_encode_jfxx( &encodersettings );
  jpege_time_started = timetick_get_ms();
  status = (int)jpege_encode_exif( &encodersettings , &jpege_test_exif_info );

  response->result = (char) status;
  return response;
}


/*****************************************************************************
* 
*   jpeg_test_decoder_cb
* 
*   is used by the jpeg services to report results...
*   the example provided by the JPEG guys relied on BREW, which I and
*   this test suite can NOT, so this function is a no-op.
* 
*****************************************************************************/


void camera_svcs_diag_jpeg_test_decoder_cb (
                            jpeg_cb_status            status
                          , jpeg_decode_output_type * output_ptr 
                          )
{
  unsigned int w = output_ptr->image_width  ,
               h = output_ptr->image_height ;
  
  switch( status )
  {
    case JPEG_DECODE_DONE:
    {
      int errorcode = 0;
      jpegd_decode_time = timetick_get_ms() - jpegd_time_started;            
      if (decode_data_dst == WRITE_TO_EFS)
      {      

        errorcode = memory_to_efs(
                                  DECODETHISFILE ".raw"
                                , output_ptr->dstPtr  //  decodersettings.display_ptr
                                , output_file_size
                               );
        if ( errorcode )
        {
          MSG_HIGH( "jpeg_test_decoder_cb : memory_to_efs failure %d..." , errorcode , 0 , 0 );
        } 
        else
        {
          MSG_HIGH( "jpeg_test_decoder_cb : memory_to_efs OK" , 0 , 0 , 0 );
        }
        free( output_ptr->dstPtr );
      }
      else /* WRITE_TO_MEM */
      {
        if (output_file_in_ram != NULL)
        {
          free(output_file_in_ram);
        }
        output_file_in_ram = output_ptr->dstPtr;
      }
    }
    jpegd_in_progress = 0;
    break;

    case  JPEG_DECODE_ABORT:
    {
      jpegd_in_progress = 0;
      MSG_HIGH( "jpeg_test_decoder_cb : ABORT, though why?" , 0 , 0 , 0 );
    }
    break;

    case JPEG_DECODE_GET_BUF:
    {
      MSG_HIGH( "jpeg_test_decoder_cb : GET_BUF..." , 0 , 0 , 0 );
      if (jpegd_output_format == (uint32)JPEG_RGB888)
      {
        output_file_size = w * h * 3;
      }
      else if (jpegd_output_format == (uint32)JPEG_RGB565 || jpegd_output_format == (uint32)JPEG_YCbCr422lp
                            || jpegd_output_format == (uint32)JPEG_YCrCb422lp)
      {
        output_file_size = w * h * 2;
      }
      else 
      {
        output_file_size = w * h * 3 / 2;
      }
      output_ptr->dstPtr = malloc( output_file_size ); //  did malloc fail ?
      jpegd_output_width = (uint16)w;
      jpegd_output_height = (uint16)h;

    //decodersettings.display_ptr    = malloc(  w *  h * 2 ); //  did malloc fail ?
    //decodersettings.disp_buf_size  =       (  w *  h * 2 );
    }
    break;

    default:
    {
      MSG_HIGH( "jpeg_test_decoder_cb : status unknown = %d", status, 0, 0 );
    }
  }
}


/*****************************************************************************
* 
*   jpeg_dec_efs_to_efs
* 
*   tells the jpeg decoder to decode a jpeg from efs to a raw in efs.
* 
*****************************************************************************/

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         uint32                      data_src ;  
                         uint32                      data_dst ;  
                         uint32                      output_format;
                         uint32                      use_arm_based;
                         uint32                      resize_width;
                         uint32                      resize_height;
                         uint32                      aspect_ratio_flag;
                         uint32                      resize_flag;
                      }  JPEG_DECODER_REQUEST  ;

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  JPEG_DECODER_RESPONSE ; 


/****************************************************************************/


PACKED void *  camera_svcs_diag_jpeg_decode ( PACKED void * request , uint16 length )
{
  JPEG_DECODER_RESPONSE        *  response;
  int                             status;
  unsigned char *                 jpegfileinmemory;
  unsigned int                    jpegfilesize;
  uint32                          data_src = ((JPEG_DECODER_REQUEST *)request)->data_src;
  boolean                         use_dsp = (((JPEG_DECODER_REQUEST *)request)->use_arm_based == 1) ? FALSE : TRUE;

  response = (JPEG_DECODER_RESPONSE *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_DEC_EFS_TO_EFS
                                 , sizeof( JPEG_DECODER_RESPONSE )
                                 );

  if(response == NULL)
  {
    return response;
  }

  jpegd_output_format = ((JPEG_DECODER_REQUEST *)request)->output_format;

  if (jpegd_in_progress || jpege_in_progress)
  {
    MSG_HIGH ( "Requesting JPEG decode while another session is in progress. Decline request" , 0,0,0) ;
    response->result = 1; 
    return response;
  }
  if (data_src == READ_FROM_RAM && input_file_in_ram == NULL)
  {
    MSG_HIGH ( "Load jpeg to memory first", 0, 0, 0 );
    response->result = 1;
    return response;
  }
  if (jpegd_output_format >= (uint32)JPEG_NUM_FORMAT)
  {
    MSG_HIGH ( "Unknown output format", 0, 0, 0 );
    response->result = 1;
    return response;
  }
  if (data_src == READ_FROM_EFS)
  {
    if (input_file_in_ram != NULL)
    {
      free(input_file_in_ram);
      input_file_in_ram = NULL;
    }
    status = efs_to_memory(DECODETHISFILE ".jpg", &jpegfileinmemory, &jpegfilesize);
    if ( status )
    {
      response->result = (char)status; 
      MSG_HIGH( "efs_to_memory failure %d" , status , 0 , 0 );
      return response;
    }
  }
  else
  {
    jpegfileinmemory = input_file_in_ram;
    jpegfilesize = input_file_size;
  }
  decode_data_dst = ((JPEG_DECODER_REQUEST *)request)->data_dst;
  if (output_file_in_ram != NULL)
  {
    free(output_file_in_ram);
    output_file_in_ram = NULL;
  }

  jpegd_in_progress = 1;  
  decodersettings.image_data_ptr = jpegfileinmemory ;
  decodersettings.file_size      = jpegfilesize     ;
  decodersettings.thumbnail      = FALSE ;
  decodersettings.output_format  = (jpeg_format_enum_type)jpegd_output_format;
#ifdef FEATURE_JPEGD_RESIZE
  decodersettings.resizeImageHeight = ((JPEG_DECODER_REQUEST *)request)->resize_height;
  decodersettings.resizeImageWidth = ((JPEG_DECODER_REQUEST *)request)->resize_width;
  decodersettings.aspectRatioFlag = ((JPEG_DECODER_REQUEST *)request)->aspect_ratio_flag ? TRUE : FALSE;
  decodersettings.jpegdResizeFlag = ((JPEG_DECODER_REQUEST *)request)->resize_flag ? TRUE : FALSE;
#endif

  jpegd_time_started = timetick_get_ms();
  jpeg_decode_thin_layer( &decodersettings
                        , camera_svcs_diag_jpeg_test_decoder_cb
                        , &decoderresults
                        , use_dsp  //  use the DSP?
                        );

  // Now save the raw results to disk;
  // do so in callback,
  // because it's not there until the jpeg
  // routines call the callback.

  response->result = 0;
  return response;
}


/*****************************************************************************
*  
*  dump_ram_to_efs_callback
*
*  This routine is called back when fs_write is done writing the 
*  output_file_in_ram to the EFS.
*
*****************************************************************************/

void dump_ram_to_efs_callback(fs_rsp_msg_type *rsp_msg)
{
  dump_ram_to_efs_time = timetick_get_ms() - dump_ram_to_efs_time;
  dump_ram_to_efs_in_progress = 0;
  if (rsp_msg->write.count != output_file_size || rsp_msg->write.status != FS_OKAY_S)
  {
    MSG_ERROR("Error writing RAM file to EFS", 0, 0, 0);
    return;
  }
  file_close_pending = 1;
}

/*****************************************************************************
* 
*   jpeg_test_query
* 
*   service any query from diag regarding the jpeg unit test session
* 
*****************************************************************************/

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         uint32                       query_type ;
                      }  JPEG_TEST_QUERY_REQUEST  ;

typedef PACKED struct { 
                         diagpkt_subsys_header_type  unused; 
                         uint32                      param;
                         uint32                      param2;
                      }  JPEG_TEST_QUERY_RESPONSE  ;

enum QUERY_TYPE_T
{
  JPEGE_IN_PROGRESS = 0,
  JPEGD_IN_PROGRESS,
  JPEGE_ENCODE_TIME,
  JPEGD_DECODE_TIME,
  DUMP_RAM_FILE_TO_EFS_START,
  DUMP_RAM_FILE_TO_EFS_IN_PROGRESS,
  DUMP_RAM_FILE_TO_EFS_TIME,
  LPM_IN_PROGRESS,
  JPEGD_OUTPUT_DIMENSION,
  NUM_QUERY_TYPE
};

PACKED void *  camera_svcs_diag_jpeg_test_query  ( PACKED void *  request , uint16 length )
{
  JPEG_TEST_QUERY_RESPONSE* response;
  uint32 query_type = ((JPEG_TEST_QUERY_REQUEST*)request)->query_type;

  response = (JPEG_TEST_QUERY_RESPONSE*)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_TEST_QUERY
                                 , sizeof( JPEG_TEST_QUERY_RESPONSE )
                                 );
  switch (query_type)
  {
  case JPEGE_IN_PROGRESS:
    response->param = (uint32)jpege_in_progress;
    break;
  case JPEGD_IN_PROGRESS:
    response->param = (uint32)jpegd_in_progress;
    break;
  case JPEGE_ENCODE_TIME:
    response->param = jpege_encode_time;
    break;
  case JPEGD_DECODE_TIME:
    response->param = jpegd_decode_time;
    break;
  case DUMP_RAM_FILE_TO_EFS_START:
    fs_remove(RAMFILESINK, NULL, &fs_rsp);
    open_parms.create.cleanup_option = FS_OC_CLOSE;
    open_parms.create.buffering_option = FS_OB_PROHIBIT;
    fs_open(RAMFILESINK, FS_OA_CREATE, &open_parms, NULL, &fs_rsp);
    if (fs_rsp.open.status != FS_OKAY_S)
    {
      response->param = 1;
      break;
    }
    response->param = 0;
    fs_handle = fs_rsp.open.handle;
    file_close_pending = 0;
    dump_ram_to_efs_in_progress = 1;
    dump_ram_to_efs_time = timetick_get_ms();
    fs_write(fs_handle, output_file_in_ram, output_file_size, dump_ram_to_efs_callback, &fs_rsp);
    break;
  case DUMP_RAM_FILE_TO_EFS_IN_PROGRESS:
    response->param = (uint32)dump_ram_to_efs_in_progress;
    if (file_close_pending)
    {
      file_close_pending = 0;
      fs_close(fs_handle, NULL, &fs_rsp);
    }
    break;
  case DUMP_RAM_FILE_TO_EFS_TIME:
    response->param = dump_ram_to_efs_time;
    break;
  case JPEGD_OUTPUT_DIMENSION:
    response->param = jpegd_output_width;
    response->param2 = jpegd_output_height;
    break;
#ifdef FEATURE_CAMERA_LPM
#error code not present
#endif
  default:
    break;
  }

  return response;
}

/*****************************************************************************
* 
*   camera_svcs_diag_jpeg_load_file_to_ram
* 
*   load bitstream data from diag pkts to RAM
* 
*****************************************************************************/
typedef struct {
                         diagpkt_subsys_header_type  header ;
                         uint32                      operation; /* 0 = load, 1 = malloc, 2 = free */
                         uint32                      offset;
                         uint32                      length;
                         uint8                       data;
                      }  JPEG_LOAD_FILE_REQUEST  ;

typedef PACKED struct { 
                         diagpkt_subsys_header_type  unused; 
                         uint32                      result;
                      }  JPEG_LOAD_FILE_RESPONSE  ;

PACKED void *  camera_svcs_diag_jpeg_load_file_to_ram  ( PACKED void *  request , uint16 length )
{
  JPEG_LOAD_FILE_RESPONSE* response;
  JPEG_LOAD_FILE_REQUEST*  resq = (JPEG_LOAD_FILE_REQUEST*)request;
 
  response = (JPEG_LOAD_FILE_RESPONSE*)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_LOAD_FILE_TO_RAM
                                 , sizeof( JPEG_LOAD_FILE_RESPONSE )
                                 );

  switch (resq->operation)
  {
  case 0: /* load data */
    if (resq->offset + resq->length <= input_file_size)
    {
      memcpy(input_file_in_ram + resq->offset, &(resq->data), resq->length);
    }
    break;
  case 1: /* malloc */
    if (input_file_in_ram != NULL)
    {
      free(input_file_in_ram);
    }
    input_file_in_ram = (uint8 *)malloc(resq->length);
    input_file_size = resq->length;
    break;
  case 2: /* free */
    if (input_file_in_ram != NULL)
    {
      free(input_file_in_ram);
      input_file_in_ram = NULL;
      input_file_size = 0;
    }
    break;
  default:
    break;
  }
  return response;
}

/*****************************************************************************
* 
*   camera_svcs_diag_jpeg_get_file_from_ram
* 
*   load bitstream data from diag pkts to RAM
* 
*****************************************************************************/

typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         uint32                      operation ; // 0 = get data, 1 = malloc, 2 = free, 3 = get data size
                         uint32                      offset;
                         uint32                      length;
                      }  JPEG_GET_FILE_REQUEST  ;

typedef struct { 
                    diagpkt_subsys_header_type  unused; 
                    uint32                      data_length;
                    uint8                       data;
                 }  JPEG_GET_FILE_RESPONSE  ;

PACKED void *  camera_svcs_diag_jpeg_get_file_from_ram  ( PACKED void *  request , uint16 length )
{
  JPEG_GET_FILE_RESPONSE* response = NULL;
  JPEG_GET_FILE_REQUEST*  resq = (JPEG_GET_FILE_REQUEST*)request;

  switch (resq->operation)
  {
  case 0: /* get data */
    if (output_file_in_ram != NULL && resq->offset + resq->length <= output_file_size)
    {
      response = (JPEG_GET_FILE_RESPONSE*)
           diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                               , JPEG_GET_FILE_FROM_RAM
                               , sizeof( JPEG_GET_FILE_RESPONSE ) + resq->length - 1
                               );
      if(response == NULL)
      {
        return response;
      }

      memcpy(&(response->data), output_file_in_ram + resq->offset, resq->length);
    }
    break;
  case 1: /* malloc */
    response = (JPEG_GET_FILE_RESPONSE*)
           diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                               , JPEG_GET_FILE_FROM_RAM
                               , sizeof( JPEG_GET_FILE_RESPONSE )
                               );
    if (output_file_in_ram != NULL)
    {
      free(output_file_in_ram);
    }
    output_file_in_ram = (uint8 *)malloc(resq->length);
    output_malloc_size = resq->length;
    break;
  case 2: /* free */
    response = (JPEG_GET_FILE_RESPONSE*)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , JPEG_GET_FILE_FROM_RAM
                                 , sizeof( JPEG_GET_FILE_RESPONSE )
                                 );
    if (output_file_in_ram != NULL)
    {
      free(output_file_in_ram);
      output_file_in_ram = NULL;
    }
    break;
  case 3: /* get data size */
    response = (JPEG_GET_FILE_RESPONSE*)
           diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                               , JPEG_GET_FILE_FROM_RAM
                               , sizeof( JPEG_GET_FILE_RESPONSE )
                               );
    if(response == NULL)
      {
        return response;
      }                           
    response->data_length = output_file_size;
    break;
  default:
    response = (JPEG_GET_FILE_RESPONSE*)
       diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                           , JPEG_GET_FILE_FROM_RAM
                           , sizeof( JPEG_GET_FILE_RESPONSE )
                           );
    break;
  }
  return response;
}

/*****************************************************************************
* 
*   efs_to_ram_to_efs
* 
*   tests whether i'm using EFS routines correctly:  load a file then rewrite it.
* 
*****************************************************************************/


typedef PACKED struct { 
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  EFS_TO_RAM_TO_EFS_RESPONSE;


/****************************************************************************/


PACKED void *  camera_svcs_diag_efs_to_ram_to_efs ( PACKED void * request , uint16 length )
{
  EFS_TO_RAM_TO_EFS_RESPONSE *  response;
  int                           status;
  unsigned char *               fileinmemory;
  unsigned int                  filesize;
  static char * temp_file_name  = "efs_test_file";
  static char * temp_file_name2 = "efs_test_file-cloned";

  response = (EFS_TO_RAM_TO_EFS_RESPONSE *)
             diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                 , EFS_TO_RAM_TO_EFS
                                 , sizeof( EFS_TO_RAM_TO_EFS_RESPONSE )
                                 );

  if(response == NULL)
  {
    return response;
  }

  status = efs_to_memory(
                           temp_file_name
                         , &fileinmemory
                         , &filesize
                        );

  if ( status )
  {
    response->result = (char)status;  //  non-0, so uh oh.
    return response;
  }

  // now save the file to disk with filename altered

  status = memory_to_efs(
                           temp_file_name2
                         , fileinmemory
                         , filesize
                        );
  
  if ( status )
  {
    response->result = (char)status;  //  non-0, so uh oh.
    return response;
  }

  response->result =  0;
  return response;
}


/*****************************************************************************
* 
*   ping
* 
*   tests whether there's any connectivity.
* 
*****************************************************************************/


typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  PING_RESPONSE;


PACKED void *  camera_svcs_diag_ping ( PACKED void * request , uint16 length )
{
  PING_RESPONSE * response = (PING_RESPONSE *)
                  diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                                      , PING
                                      , sizeof( PING_RESPONSE )
                                      );
  if(response == NULL)
  {
    return response;
  }

  response -> result = 1;
  return  response;
}


/*****************************************************************************
*   camera_svcs_diag_callback   is used by the various async camera calls.
*****************************************************************************/


void  camera_svcs_diag_callback ( 
                                  camera_cb_type    cb,
                                  const void      * client_data,
                                  camera_func_type  func,
                                  int32             parm4
                                )
{       
  switch( func )
  {
    case CAMERA_FUNC_START:
      break;
    case CAMERA_FUNC_STOP:
      break;
    case CAMERA_FUNC_START_PREVIEW:
      {
        MSG_HIGH ("camera_svcs_diag_callback :  START_PREVIEW", 0, 0, 0);   
      //if( cb == CAMERA_RSP_CB_SUCCESS )
      //{
      ////MSG_LOW( "func=CAMERA_FUNC_START_PREVIEW, camera_cb=CAMERA_RSP_CB_SUCCESS",0,0,0 );
      ////ftm_sig_rex_set(FTM_SIG_WAIT_SIG);
      //}
      //else if (cb == CAMERA_EVT_CB_FRAME) 
      //{
      ////ret_code = camera_release_frame(  );
      ////MSG_LOW( "func=CAMERA_FUNC_START_PREVIEW, camera_release_frame= %d",ret_code,0,0 );
      //}
      //else if( cb == CAMERA_EXIT_CB_FAILED )
      //{
      ////MSG_LOW( "func=CAMERA_FUNC_START_PREVIEW, camera_cb=CAMERA_EXIT_CB_FAILED",0,0,0 );
      ////ftm_sig_rex_set(FTM_SIG_WAIT_SIG);
      //}
      }
      break;
    default:
        MSG_HIGH ("camera_svcs_diag_callback :  DEFAULT", 0, 0, 0);   
      break;
  }
}


/*****************************************************************************
*   camera_svcs_diag_camera_start  turns on the camera subsystem.
*****************************************************************************/


typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  CAMERA_START_STOP_RESPONSE;


/****************************************************************************/


PACKED void *  camera_svcs_diag_camera_start ( PACKED void * request , 
                                               uint16        length 
                                             )
{
  CAMERA_START_STOP_RESPONSE * response = (CAMERA_START_STOP_RESPONSE *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                        , CAMERA_START
                        , sizeof( CAMERA_START_STOP_RESPONSE )
                        );

  if(response == NULL)
  {
    return response;
  }

  response -> result = (char) camera_start( camera_svcs_diag_callback, NULL );
  return  response;
}


/*****************************************************************************
*   camera_svcs_diag_camera_stop   turns off the camera subsystem.
*****************************************************************************/


PACKED void *  camera_svcs_diag_camera_stop  ( PACKED void * request , 
                                               uint16        length 
                                             )
{
  CAMERA_START_STOP_RESPONSE * response = (CAMERA_START_STOP_RESPONSE *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                        , CAMERA_STOP
                        , sizeof( CAMERA_START_STOP_RESPONSE )
                        );
  if(response == NULL)
  {
    return response;
  }

  response -> result = (char) camera_stop( camera_svcs_diag_callback , NULL );
  return  response;
}


/*****************************************************************************
*   camera_svcs_diag_camera_preview_start  starts reading from sensor.
*****************************************************************************/


typedef PACKED struct {
                         diagpkt_subsys_header_type  header ;
                         char                        result ;
                      }  CAMERA_PREVIEW_RESPONSE ;


/****************************************************************************/


PACKED void *  camera_svcs_diag_camera_preview_start ( PACKED void * request
                                                     , uint16        length 
                                                     )
{
  CAMERA_PREVIEW_RESPONSE * response = (CAMERA_PREVIEW_RESPONSE *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                        , CAMERA_PREVIEW_START
                        , sizeof( CAMERA_PREVIEW_RESPONSE )
                        );
  if(response == NULL)
  {
    return response;
  }
  response -> result = (char) camera_start_preview( camera_svcs_diag_callback , NULL );
  return  response;
}


/*****************************************************************************
*   camera_svcs_diag_camera_preview_stop   stops  reading from sensor.
*****************************************************************************/


PACKED void *  camera_svcs_diag_camera_preview_stop  ( PACKED void * request
                                                     , uint16        length 
                                                     )
{
  CAMERA_PREVIEW_RESPONSE * response = (CAMERA_PREVIEW_RESPONSE *)
    diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
                        , CAMERA_PREVIEW_STOP
                        , sizeof( CAMERA_PREVIEW_RESPONSE )
                        );
  if(response == NULL)
  {
    return response;
  }
  response -> result = (char) camera_stop_preview();
  return  response;
}


//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_*  structures...
//  *****************************************************************************/
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header   ;
//                           int16                       variable ;
//                           byte                        four_bytes [4];
//                        }  CHROMATIX_LIVE_SET_REQUEST  ;
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header ;
//                           char                        result ;
//                        }  CHROMATIX_LIVE_SET_RESPONSE ;
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header   ;
//                           int16                       variable ;
//                        }  CHROMATIX_LIVE_GET_REQUEST  ;
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header ;
//                           byte                        four_bytes [4];
//                           char                        result ;
//                        }  CHROMATIX_LIVE_GET_RESPONSE ;
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_set_integer
//  *****************************************************************************/
//  
//  
//  PACKED void *  camera_svcs_diag_chromatix_live_set_integer
//                 ( 
//                   PACKED void * request
//                 , uint16        length 
//                 )
//  {
//    CHROMATIX_LIVE_SET_RESPONSE * response = NULL ;
//  
//    int                           i        =    0 ;
//    int16                         variable =    0 ;
//    int32                         value    =    0 ;
//  
//    variable = ((CHROMATIX_LIVE_SET_REQUEST*)request)->variable;
//    value    = 0;
//    for (i = 0 ; i<4 ; ++i)
//    {
//      value =   (256 * value)
//              + ((CHROMATIX_LIVE_SET_REQUEST*)request)->four_bytes[i];
//    }
//  
//    //
//    //  Now we should have reconstituted an integer from the 4 bytes.
//    //  So, we have the variable's 16bit id, and its new value.
//    //  Seek the variable in the registry and assign its new value.
//    //
//  
//    response = (CHROMATIX_LIVE_SET_RESPONSE*)
//               diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
//                                   , CHROMATIX_LIVE_SET_INTEGER
//                                   , sizeof( CHROMATIX_LIVE_SET_RESPONSE )
//                                   );
//    response -> result = 1;
//  
//    return  (PACKED void *)response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_set_float
//  *****************************************************************************/
//  
//  
//  PACKED void *  camera_svcs_diag_chromatix_live_set_float
//                 ( 
//                   PACKED void * request
//                 , uint16        length 
//                 )
//  {
//    CHROMATIX_LIVE_SET_RESPONSE * response = NULL ;
//  
//    int16                         variable =    0 ;
//    float                         value    =    0 ;
//  
//    variable = ((CHROMATIX_LIVE_SET_REQUEST*)request)->variable;
//    value    = 0.0;
//  
//    //
//    //  Now we should have reconstituted a float from the 4 bytes.
//    //  So, we have the variable's 16bit id, and its new value.
//    //  Seek the variable in the registry and assign its new value.
//    //
//  
//    response = (CHROMATIX_LIVE_SET_RESPONSE*)
//               diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
//                                   , CHROMATIX_LIVE_SET_FLOAT
//                                   , sizeof( CHROMATIX_LIVE_SET_RESPONSE )
//                                   );
//    response -> result = 1;
//  
//    return  (PACKED void *)response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_get_integer
//  *****************************************************************************/
//  
//  
//  PACKED void *  camera_svcs_diag_chromatix_live_get_integer
//                 ( 
//                   PACKED void * request
//                 , uint16        length 
//                 )
//  {
//    CHROMATIX_LIVE_GET_RESPONSE * response = NULL ;
//  
//  //int16 variable = ((CHROMATIX_LIVE_GET_REQUEST*) request)->variable;
//    int32 value    = 0;
//  
//    //
//    //  Retrieve the real int32 value from the registry.
//    //
//  
//    response = (CHROMATIX_LIVE_GET_RESPONSE*)
//               diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
//                                   , CHROMATIX_LIVE_GET_INTEGER
//                                   , sizeof( CHROMATIX_LIVE_GET_RESPONSE )
//                                   );
//    *( (int32 *)(&(response -> four_bytes[0])) ) = value;
//    response -> result = 1;
//  
//    return response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_get_float
//  *****************************************************************************/
//  
//  
//  PACKED void *  camera_svcs_diag_chromatix_live_get_float
//                 ( 
//                   PACKED void * request
//                 , uint16        length 
//                 )
//  {
//    CHROMATIX_LIVE_GET_RESPONSE * response = NULL ;
//  
//  //int16 variable = ((CHROMATIX_LIVE_GET_REQUEST*) request)->variable;
//    float value    = 0.0;
//  
//    //
//    //  Retrieve the real float value from the registry.
//    //
//  
//    response = (CHROMATIX_LIVE_GET_RESPONSE*)
//               diagpkt_subsys_alloc( DIAG_SUBSYS_CAMERA_SVCS
//                                   , CHROMATIX_LIVE_GET_FLOAT
//                                   , sizeof( CHROMATIX_LIVE_GET_RESPONSE )
//                                   );
//    *( (float *)(&(response -> four_bytes[0])) ) = value;
//    response -> result = 1;
//  
//    return response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_whos_there  structures...
//  *****************************************************************************/
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header   ;
//                        }  CHROMATIX_LIVE_WHOS_THERE_REQUEST  ;
//  
//  
//  typedef PACKED struct {
//                           diagpkt_subsys_header_type  header ;
//                           char                        result ;
//                           int16                       whos_there [1];
//                        }  CHROMATIX_LIVE_WHOS_THERE_RESPONSE ;
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_chromatix_live_whos_there
//  *****************************************************************************/
//  
//  
//  PACKED void *  camera_svcs_diag_chromatix_live_whos_there
//                 ( 
//                   PACKED void * request
//                 , uint16        length 
//                 )
//  {
//    CHROMATIX_LIVE_WHOS_THERE_RESPONSE * response = NULL ;
//  
//    //
//    //  Query the registry to see how many objects are
//    //  registered, and construct an array of int16
//    //  hold one int16 which is the # N of registered
//    //  objects, followed by the N objects' int16 ids
//    //  from the registry.
//    //  Return that array of N+1 objects.
//    //
//    //  When does the thing get cleaned up, though?
//    //  Hmmm...
//    //
//  
//    response = (CHROMATIX_LIVE_WHOS_THERE_RESPONSE*)
//               diagpkt_subsys_alloc(
//                                     DIAG_SUBSYS_CAMERA_SVCS
//                                   , CHROMATIX_LIVE_WHOS_THERE
//                                   , sizeof( CHROMATIX_LIVE_WHOS_THERE_RESPONSE )
//                                   );
//  //*( (float *)(&(response -> four_bytes[0])) ) = value;
//    response -> result = 1;
//  
//    return response;
//  }


//  /*****************************************************************************
//  *   camera_svcs_diag_camera_tune_set_value
//  *****************************************************************************/
//  
//  
//  typedef PACKED struct
//  {
//    diagpkt_subsys_header_type  header;
//    int16                       unique_id;
//    char                        value_string[ 128 ];
//  }
//    CAMERA_TUNE_SET_VALUE_REQUEST  ;
//  
//  
//  typedef PACKED struct
//  {
//    char result;
//  }
//    CAMERA_TUNE_SET_VALUE_RESPONSE ;
//  
//  
//  PACKED void *  camera_svcs_diag_camera_tune_set_value (
//                                                          PACKED void * packet
//                                                        , uint16        length
//                                                        )
//  {
//    CAMERA_TUNE_SET_VALUE_REQUEST  * request           ;
//    CAMERA_TUNE_SET_VALUE_RESPONSE * response          ;
//  
//    request  = (CAMERA_TUNE_SET_VALUE_REQUEST  *)packet;
//    response = (CAMERA_TUNE_SET_VALUE_RESPONSE *)
//      diagpkt_subsys_alloc(
//                            DIAG_SUBSYS_CAMERA_SVCS , CAMERA_TUNE_SET_VALUE
//                          , sizeof( CAMERA_TUNE_SET_VALUE_RESPONSE )
//                          );
//  
//    if (length != sizeof(CAMERA_TUNE_SET_VALUE_REQUEST))
//    {
//      response -> result = 0;  //  we didn't receive a full request packet.
//    }
//    else
//    {
//      int16   unique_id    =   request->unique_id        ;
//      char  * value_string = &(request->value_string[0]) ;
//  
//      camera_tune_set_value( unique_id, value_string );
//      response -> result = 1;
//    }
//    return  (PACKED void *)response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_camera_tune_get_value
//  *****************************************************************************/
//  
//  
//  typedef PACKED struct
//  {
//    diagpkt_subsys_header_type  header    ;
//    int16                       unique_id ;
//  }
//    CAMERA_TUNE_GET_VALUE_REQUEST  ;
//  
//  
//  typedef PACKED struct
//  {
//    char result;
//    char assignment_string[ 128 ];
//  }
//    CAMERA_TUNE_GET_VALUE_RESPONSE ;
//  
//  
//  PACKED void *  camera_svcs_diag_camera_tune_get_value (
//                                                          PACKED void * packet
//                                                        , uint16        length
//                                                        )
//  {
//    CAMERA_TUNE_GET_VALUE_REQUEST  * request  ;
//    CAMERA_TUNE_GET_VALUE_RESPONSE * response ;
//  
//    request  = (CAMERA_TUNE_GET_VALUE_REQUEST  *)packet;
//    response = (CAMERA_TUNE_GET_VALUE_RESPONSE *)
//      diagpkt_subsys_alloc(
//                            DIAG_SUBSYS_CAMERA_SVCS , CAMERA_TUNE_GET_VALUE ,
//                            sizeof( CAMERA_TUNE_GET_VALUE_RESPONSE )
//                          );
//  
//    if (length != sizeof(CAMERA_TUNE_GET_VALUE_REQUEST))
//    {
//      //
//      //  We didn't receive a full request packet.
//      //
//  
//      response -> assignment_string[0] = '\0';
//      response -> result               = 0;
//    }
//    else
//    {
//      //
//      //  Do we leak memory here...need to free(value_string) after strncpy?
//      //
//  
//      char  * value_string ;
//  
//      value_string = camera_tune_get_value( request->unique_id );
//      strncpy( &(response->assignment_string[0]) , value_string , 128 );
//      response -> result = 1;
//    }
//    return  (PACKED void *)response;
//  }
//  
//  
//  /*****************************************************************************
//  *   camera_svcs_diag_camera_tune_describe_registry
//  *****************************************************************************/
//  
//  
//  typedef PACKED struct
//  {
//    diagpkt_subsys_header_type  header;
//  }
//    CAMERA_TUNE_DESCRIBE_REGISTRY_REQUEST  ;
//  
//  
//  typedef PACKED struct
//  {
//    diagpkt_subsys_header_type  header;  //  Lint8 will complain naively...needed by an extern function.
//    char                        result;
//    int16                       description_length  ;
//    int16                       description_version ;
//  }
//    CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE ;
//  
//  
//  //  static int16  dud_registry_description_response[2] = {2,0} ;
//  
//  
//  PACKED void *  camera_svcs_diag_camera_tune_describe_registry
//                 (
//                   PACKED void * packet
//                 , uint16        length
//                 )
//  {
//    CAMERA_TUNE_DESCRIBE_REGISTRY_REQUEST  * request  ;
//    CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE * response ;
//  
//    request = (CAMERA_TUNE_DESCRIBE_REGISTRY_REQUEST *)packet;
//  
//    if (length != sizeof(CAMERA_TUNE_DESCRIBE_REGISTRY_REQUEST))
//    {
//      //
//      //  We didn't receive a full request packet.
//      //
//  
//      response = (CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE *)
//        diagpkt_subsys_alloc(
//                              DIAG_SUBSYS_CAMERA_SVCS , CAMERA_TUNE_DESCRIBE_REGISTRY ,
//                              sizeof( CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE )
//                            );
//      response -> result = 0;
//      response -> description_length  = 2;
//      response -> description_version = 0;
//    }
//    else
//    {
//      //
//      //  Do we leak memory here...need to free(value_string) after strncpy?
//      //
//  
//      int16  * description ;
//  
//      description = camera_tune_describe_registry();
//      response = (CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE *)
//        diagpkt_subsys_alloc(
//                              DIAG_SUBSYS_CAMERA_SVCS , CAMERA_TUNE_DESCRIBE_REGISTRY ,
//                              sizeof( CAMERA_TUNE_DESCRIBE_REGISTRY_RESPONSE )
//                              +
//                              (description[0] - 2)
//                            );
//  //  for ( ; ; )
//  //  {
//  //    // copy over the int16s man
//  //  }
//      response -> result = 1;
//    }
//    return  (PACKED void *)response;
//  }
//  
//  
//////////////////////////////////////////////////////////////////////////////


typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
} CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_REQUEST ;


/////////////////////////////////////////////////////


typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
  char                        result;
} CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE ;


/////////////////////////////////////////////////////


PACKED void *  camera_svcs_diag_camera_tune_config_file_read
               (
                  PACKED void * packet
               ,  uint16        length
               )
{
  CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE * response ;
  static  char  file_name[ 256 ];

  response = (CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE *)
    diagpkt_subsys_alloc(
                          DIAG_SUBSYS_CAMERA_SVCS ,
                          CAMERA_TUNE_CONFIG_FILE_READ ,
                          sizeof(CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE)
                        );
  if (response != NULL)
  {
    response -> result = 0;  //  Pessimistic, I know.
  }

  if (length == sizeof(CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_REQUEST))
  {
    //
    //  Call MobiCaT to tell it to READ the current config file.
    //

    file_name[0] = '\0';

    #ifdef  FEATURE_CAMERA_MOBICAT
    (void)snprintf( file_name , 256 , "%s" ,
                    CAMERA_TUNE_PC_QMOBICAT_TEMP_FILE_NAME
                  );

    (void)camera_tune_read_dot_ini_file ( file_name );
    #endif//FEATURE_CAMERA_MOBICAT
    if (response != NULL)
    {
      response -> result = 1;
    }
  }

  return  (PACKED void *)response;
}


/////////////////////////////////////////////////////


PACKED void *  camera_svcs_diag_camera_tune_config_file_write
               (
                  PACKED void * packet
               ,  uint16        length
               )
{
//  CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_REQUEST  * request  ;
  CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE * response = NULL ;

  #ifdef  FEATURE_CAMERA_MOBICAT

  static  char  file_name[ 256 ];

//  request =  (CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_REQUEST*)packet;
  response = (CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE *)
    diagpkt_subsys_alloc(
                          DIAG_SUBSYS_CAMERA_SVCS ,
                          CAMERA_TUNE_CONFIG_FILE_WRITE ,
                          sizeof(CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_RESPONSE)
                        );
  if (response != NULL)
  {
    response -> result = 0;  //  Pessimistic, I know.
  }

  if (length == sizeof(CAMERA_TUNE_CONFIG_FILE_READ_OR_WRITE_REQUEST))
  {
    //
    //  Call MobiCaT to tell it to WRITE the current config file.
    //

    file_name[0] = '\0';
    (void)snprintf( file_name , 256 , "%s" ,
                    CAMERA_TUNE_PC_QMOBICAT_TEMP_FILE_NAME
                  );

    (void)
    camera_tune_write_dot_ini_to_filename( file_name );


    //
    //  Here we just hope it worked.  This should be revised in a later
    //  implementation of MobiCaT, not this late 2005 initial commercial seed.
    //

    if (response != NULL)
    {
      response -> result = 1;
    }
  }

  #endif//FEATURE_CAMERA_MOBICAT

  return  (PACKED void *)response;
}


//////////////////////////////////////////////////////////////////////////////


// This structure is used to send or request parameter updates.
// See camera_tune.c for format of data.
typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
  byte                        data [1529];  // Was 2048 but discovered anything > 1529 fails.
} CAMERA_TUNE_PARM_REQUEST ;

// This structure is used to return parameter info
// See camera_tune.c for format of data.
typedef PACKED struct 
{
  diagpkt_subsys_header_type  header ;
  byte                        data [2048] ;
} CAMERA_TUNE_PARM_RESPONSE ; 

// This structure is used to respond with a status
// See camera_tune.c for meaning of status.
typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
  byte                        status;
} CAMERA_TUNE_STATUS_RESPONSE ;

// This structure is used to request a list of parameter info.
// See camera_tune.c for meaning of num_to_start.
typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
  uint16                      num_to_start;
} CAMERA_TUNE_LIST_REQUEST ;

// This structure is used to send or request small basic data.
// See camera_tune.c for format of data.
typedef PACKED struct
{
  diagpkt_subsys_header_type  header;
  byte                        data [128];
} CAMERA_TUNE_MISC_REQUEST ;

// This structure is used to return small basic data
// See camera_tune.c for format of data.
typedef PACKED struct 
{
  diagpkt_subsys_header_type  header ;
  byte                        data [128] ;
} CAMERA_TUNE_MISC_RESPONSE ; 


/////////////////////////////////////////
// camera_svcs_diag_camera_tune_get_list
// For requesting a list of tunable parameters
//
// request:  CAMERA_TUNE_LIST_REQUEST
// response: CAMERA_TUNE_PARM_RESPONSE
// 
// See camera_tune_get_list in camera_tune.c for full description
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_tune_get_list
               (
                  PACKED void * request
               ,  uint16        length
               )
{
  CAMERA_TUNE_PARM_RESPONSE * response = NULL;

  MSG_HIGH( "$$$ : MOBICAT got get_list diag" , 0 , 0 , 0 );  
  #ifdef  FEATURE_CAMERA_MOBICAT
  response = (CAMERA_TUNE_PARM_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_TUNE_GET_LIST ,
                                   sizeof( CAMERA_TUNE_PARM_RESPONSE ));
  if (response != NULL)
  {
    (void)camera_tune_get_list(((CAMERA_TUNE_LIST_REQUEST *) request)->num_to_start, 
                               (byte*)(response->data), 
                               sizeof(response->data));
  }
  #endif //FEATURE_CAMERA_MOBICAT
  
  return response;
}


/////////////////////////////////////////
// camera_svcs_diag_camera_tune_get_parms
// For getting updated values of parameters
//
// request:  CAMERA_TUNE_PARM_REQUEST
// response: CAMERA_TUNE_PARM_RESPONSE
// 
// See camera_tune_get_parms in camera_tune.c for full description
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_tune_get_parms
               (
                  PACKED void * request
               ,  uint16        length
               )
{
  CAMERA_TUNE_PARM_RESPONSE * response = NULL;

  MSG_HIGH( "$$$ : MOBICAT got get_parms diag" , 0 , 0 , 0 );  
  #ifdef  FEATURE_CAMERA_MOBICAT
  response = (CAMERA_TUNE_PARM_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_TUNE_GET_PARMS ,
                                   sizeof( CAMERA_TUNE_PARM_RESPONSE ));
  if (response != NULL)
  {
    (void)camera_tune_get_parms((byte*)(((CAMERA_TUNE_PARM_REQUEST *)request)->data),
                                sizeof(((CAMERA_TUNE_PARM_REQUEST *)request)->data),
                                (byte*)(response->data), 
                                sizeof(response->data));
  }
  #endif //FEATURE_CAMERA_MOBICAT

  return response;
}


/////////////////////////////////////////
// camera_svcs_diag_camera_tune_set_parms
// For setting updated values of parameters
//
// request:  CAMERA_TUNE_PARM_REQUEST
// response: CAMERA_TUNE_STATUS_RESPONSE
// 
// See camera_tune_set_parms in camera_tune.c for full description
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_tune_set_parms
               (
                  PACKED void * request
               ,  uint16        length
               )
{
  CAMERA_TUNE_STATUS_RESPONSE * response = NULL;

  MSG_HIGH( "$$$ : MOBICAT got set_parms diag" , 0 , 0 , 0 );  
  #ifdef  FEATURE_CAMERA_MOBICAT
  response = (CAMERA_TUNE_STATUS_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_TUNE_SET_PARMS ,
                                   sizeof( CAMERA_TUNE_STATUS_RESPONSE ));
  if (response != NULL)
  {
    (void)camera_tune_set_parms((byte*)(((CAMERA_TUNE_PARM_REQUEST *)request)->data),
                                sizeof(((CAMERA_TUNE_PARM_REQUEST *)request)->data),
                                (byte*)&response->status);
	(void)rex_sleep(10);
  }
  #endif //FEATURE_CAMERA_MOBICAT

  return response;
}


/////////////////////////////////////////
// camera_svcs_diag_camera_tune_misc_command
// For sending miscellaneous commands
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
// See camera_tune_misc_command in camera_tune.c for full description
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_tune_misc_command
               (
                  PACKED void * request
               ,  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  MSG_HIGH( "$$$ : MOBICAT got misc_command diag" , 0 , 0 , 0 );  
  #ifdef  FEATURE_CAMERA_MOBICAT
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_TUNE_MISC_COMMAND ,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE ));
  if (response != NULL)
  {
    (void)camera_tune_misc_command((byte*)(((CAMERA_TUNE_MISC_REQUEST *)request)->data),
                                   sizeof(((CAMERA_TUNE_MISC_REQUEST *)request)->data),
                                   (byte*)(response->data), 
                                   sizeof(response->data));
  }
  #endif //FEATURE_CAMERA_MOBICAT

  return response;
}

/////////////////////////////////////////
// camera_svcs_diag_camera_set_zoom
// Zooms in or out the camera preview
// 
// request:  CAMERA_TUNE_PARM_REQUEST
// response: CAMERA_TUNE_PARM_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_set_zoom
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_SET_ZOOM_LEVEL ,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));
  if (response != NULL)
  {
    response->data[0] = 
        (byte)(camera_set_parm(CAMERA_PARM_ZOOM,
               *(uint32*)((CAMERA_TUNE_PARM_REQUEST*)request)->data,
               NULL,
               NULL) == (int)CAMERA_SUCCESS);
  }
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}



////////////////////////////////////////
// camera_svcs_diag_camera_set_dimensions
// Set camera preview dimensions and restart the USB preview
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_camera_set_dimensions
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_SET_DIMENSIONS ,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));

  if (response != NULL)
  {
    CAMERA_TUNE_MISC_REQUEST* req = (CAMERA_TUNE_MISC_REQUEST*)request;
    uint16 width  = *(uint16*)req->data;
    uint16 height = *(uint16*)(req->data + sizeof(uint16));

    mpp_end();
    camera_stop_preview();
    camera_set_dimensions(
      width,  //TOOD: These first two parameters might not be necessary
      height,
      width,
      height,
      NULL,
      NULL
    );
    camera_svcs_start_mobicat_preview(mpp_get_mode(), NULL);
    camera_start_preview(camera_svcs_diag_callback , NULL);
  }
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}


////////////////////////////////////////
// camera_svcs_diag_mobicat_preview_start
// Start the PC preview over USB
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_mobicat_preview_start
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_MOBICAT_PREVIEW_START,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));
  if (response != NULL)
  {
    uint32 mode = *(uint32*)((CAMERA_TUNE_MISC_REQUEST*)request)->data;
    response->data[0] = (byte)camera_svcs_start_mobicat_preview(mode, NULL);
  }
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}


/*
////////////////////////////////////////
// camera_svcs_diag_mobicat_preview_benchmark
// Provides a benchmark to test the USB link speed 
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_mobicat_preview_benchmark
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_MOBICAT_PREVIEW_BENCHMARK,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));
  mpp_benchmark(
    *(uint32*)((CAMERA_TUNE_PARM_REQUEST*)request)->data
    );
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}
*/

////////////////////////////////////////
// camera_svcs_diag_mobicat_preview_ack
// Send an ACK message to the PC over USB
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_mobicat_preview_ack
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_MOBICAT_PREVIEW_ACK,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));
  if (response != NULL)
  {
    response->data[0] = (byte)mpp_send_ack();
  }
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}


////////////////////////////////////////
// camera_svcs_diag_mobicat_preview_stop
// Stop the PC preview over USB
// 
// request:  CAMERA_TUNE_MISC_REQUEST
// response: CAMERA_TUNE_MISC_RESPONSE
// 
/////////////////////////////////////////
PACKED void *  camera_svcs_diag_mobicat_preview_stop
               (
                  PACKED void * request,
                  uint16        length
               )
{
  CAMERA_TUNE_MISC_RESPONSE * response = NULL;

  #ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
  response = (CAMERA_TUNE_MISC_RESPONSE *) 
             diagpkt_subsys_alloc (DIAG_SUBSYS_CAMERA_SVCS ,
                                   CAMERA_MOBICAT_PREVIEW_STOP,
                                   sizeof( CAMERA_TUNE_MISC_RESPONSE));
  mpp_end();
  #endif //FEATURE_CAMERA_MOBICAT_PREVIEW

  return response;
}


    

//////////////////////////////////////////////////////////////////////////////

#ifdef FEATURE_CAMERA_LPM 
#error code not present
#endif /* FEATURE_CAMERA_LPM */

#endif /* FEATURE_CAMERA_SVCS_DIAG */
#endif /* FEATURE_CAMERA */

