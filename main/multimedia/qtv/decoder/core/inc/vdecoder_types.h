#ifndef VDECODER_TYPES_H
#define VDECODER_TYPES_H
/* =======================================================================
                               vdecoder_types.h
DESCRIPTION
  This file defines all the types used by the QTV video decoder API.

Copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/inc/vdecoder_types.h#16 $
$DateTime: 2010/10/28 07:44:25 $
$Change: 1496645 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
06/30/09    as      Added Function level profiling feature
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "vdecoder_perf.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include "vdecoder_log.h"
#include "rex.h"
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

/*******************************************************************************
 Constant / Define Declarations
*******************************************************************************/

/* Maximum number of instances of vdec */
#define MAX_NO_OF_INSTANCES 2

/* The number of characters in fourcc codes */
#define VDEC_FOURCC_CHARS 4

/* Maximum supported decoder height and width */
#define VDEC_MAX_DECODE_WIDTH  320
#define VDEC_MAX_DECODE_HEIGHT 240

/* The number of characters required to store a fourcc code as a string */
#define VDEC_FOURCC_STRLEN ( VDEC_FOURCC_CHARS + 1 )

#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
  #define VDEC_MAX_LAYERS 4
#else
  #define VDEC_MAX_LAYERS 1
#endif

#ifdef FEATURE_H264_DECODER
  #define H264_NAL_LENGTH_SIZE 4
#endif 

//added because we removed internaldef.h file
#ifdef FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_XSCALE
#define FEATURE_QTV_DECODER_XSCALE_VIDEO
#endif /* FEATURE_VIDEO_PLAYER_INTERFACE_REV_2A_XSCALE */ 

//Max number of layers the player handles.
//I think this is actually used as max media layers,
//max video layers, and max audio layers
#if defined(FEATURE_MP4_TEMPORAL_SCALABILITY)
#define VDEC_COMMON_MAX_LAYERS 4 
#else
#define VDEC_COMMON_MAX_LAYERS 1
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */

/*******************************************************************************
  Type Declarations
*******************************************************************************/

typedef unsigned char VDEC_BYTE;

/* Generic void type used for decoder-stream identifiers.  Stream IDs
** are returned from decoder-creation functions and expected as input
** to all other API functions.*/
typedef void* VDEC_STREAM_ID;

/* Error code used to return information about failures to the
** outside world.*/
typedef uint32 VDEC_ERROR;

#define  VDEC_ERR_EVERYTHING_FINE             0    /* no problemo                          */
#define  VDEC_ERR_OPERATION_UNSUPPORTED       1    /* we can't do that at this time        */
#define  VDEC_ERR_NULL_STREAM_ID              2    /* a bogus stream ID pointer            */
#define  VDEC_ERR_NULL_POINTER                3    /* one of the data pointers is null     */
#define  VDEC_ERR_UNKNOWN_FTYP                4    /* dunno what to do with this ftyp      */
#define  VDEC_ERR_OUT_OF_MEMORY               5    /* an attempt to allocate memory failed */
#define  VDEC_ERR_INIT_TIMEOUT                6    /* timed out waiting for DSP init       */
#define  VDEC_ERR_MYSTERY_ERROR               7    /* what's broke?  Dunno!                */
#define  VDEC_ERR_LAYER_OUT_OF_BOUNDS         8    /* we were given a bogus layer number   */
#define  VDEC_ERR_NO_INPUT_AVAILABLE          9    /* trying to decode, but no input?      */
#define  VDEC_ERR_BOGUS_PARAMETER            10    /* application fed us something silly   */
#define  VDEC_ERR_BAD_BLOBTYPE               11    /* BLOB doesn't contain what we want    */
#define  VDEC_ERR_CORRUPT_BLOB               12    /* BLOB doesn't make sense              */
#define  VDEC_ERR_WRONG_DECODER_TYPE         13    /* wrong decoder type for attempted op  */
#define  VDEC_ERR_OUT_OF_BUFFERS             14    /* No frame buffers to satisfy request  */
#define  VDEC_ERR_RESOURCE_UNAVAILABLE       15    /* resource unavailable for the decoder */
#define  VDEC_ERR_INVALID_PARAMETER          16    /* parameter is not supported           */
#define  VDEC_ERR_INVALID_INPUT              17    /* invalid data fed to the decoder      */
#define  VDEC_ERR_UNSUPPORTED_DIMENSIONS     18    /* unsupported dimensions               */
#define  VDEC_ERR_INVALID_STREAM_ID          19    /* API called with Invalid Stream ID    */
#define  VDEC_ERR_UNSUPPORTED_PROFILE        20    /* Unsupported profile                  */
#define  VDEC_ERR_DSP_ERR_BASE            10000    /* Some error from DSP                  */ 


/* Status code passed to the frame-ready callback to indicate what caused
** the callback.  Right now, the following things can trigger that frame 
** callback:
** - a successful decode
** - an internal decoding error
** - a flushing of the frame-buffer queue. */
typedef uint32 VDEC_CB_STATUS;

#define  VDEC_STATUS_GOOD_FRAME         0       /* everything OK */
#define  VDEC_STATUS_DECODE_ERROR       1       /* no data in frame */
#define  VDEC_STATUS_FATAL_ERROR        2       /* fatal error, restart decoder */
#define  VDEC_STATUS_VOP_NOT_CODED      3       /* Healthy decode, non-coded VOP (so no frame data)*/
#define  VDEC_STATUS_FLUSH              4       /* no data in frame */
#if defined(FEATURE_FRE) || defined(FEATURE_H264_FRUC)
#define  VDEC_STATUS_FRE_FRAME          5 
#endif
#define  VDEC_STATUS_SUSPEND_DONE       6        /* suspend done */
#define  VDEC_STATUS_EOS_INDICATION     7        /* EOS indication */

/* Video frame dimensions are tossed around in these structures */
typedef struct
{
  unsigned short width;
  unsigned short height;
}
VDEC_DIMENSIONS;

/* Display/Crop window within the decoded video frame */
typedef struct
{
  unsigned short x1;
  unsigned short y1;
  unsigned short x2;
  unsigned short y2;
}
VDEC_RECTANGLE;

/* Color format supported by the decoder.*/
typedef uint32 VDEC_FOURCC;

/* YCrCb 4:2:0 planar format, Y followed by Cb and Cr planes */
#define VDEC_FOURCC_I420         0x30323449
/* YCrCb 4:2:0 planar format, interleaved Cb/Cr with Cb in the MSB */
#define VDEC_FOURCC_NV21         0x3231564E
/* Uncompressed RGB format */
#define VDEC_FOURCC_BI_RGB       0x00000000  


/* The color format and pixel depth together define the format of a frame.*/
typedef struct
{
  VDEC_FOURCC   fourcc;   /* The color format */
  VDEC_BYTE     bbp;      /* The pixel color depth, in bits per pixel */
}
VDEC_FRAME_FORMAT;


/* Indicates stream events. In the event of a stream status change, the 
** corresponding frame is the first frame to take on the new stream 
** properties. */
typedef uint32 VDEC_STREAM_STATUS;

#define   VDEC_STREAM_UNCHANGED  0    /* no change in stream status */
#define   VDEC_STREAM_SWITCHED   1    /* stream has switched */


/* The luma histogram for the frame, distributed over 256 bins. The 
** total number of samples equals 32 multiplied by the number of 
** macroblocks in the frame.*/
typedef struct
{
unsigned short  lumaHistogram[256];
}
VDEC_POST_FILTER_FRAME_INFO;

/* Structure containing post-filter MB information. Details on how to
**  interpret this interface is specified in Annex A: Post Filter
**  Interface Specification. */
typedef struct
{
  unsigned short  quant;
  VDEC_BYTE       mbType; 
  signed short    mvx[4];
  signed short    mvy[4];
  VDEC_BYTE       blockTypeConfig;
  VDEC_BYTE       codedBlockConfig;
}
VDEC_POST_FILTER_MB_INFO;

typedef struct
{
  unsigned long                 numConcealedMbs;
  unsigned long                 numIntraMbs; 
  uint32                        timeIncrement; 
  uint64                        derivedTimestamp;
  VDEC_STREAM_STATUS            streamStatus;
  VDEC_POST_FILTER_FRAME_INFO * pPostFilterFrameInfo;
  VDEC_POST_FILTER_MB_INFO *    pPostFilterMbInfo; 
}
VDEC_EXT_FRAME;

/* Frame types */
typedef uint32 VDEC_FRAMETYPE;

#define  VDEC_FRAMETYPE_UNKNOWN       0
#define  VDEC_FRAMETYPE_I             1
#define  VDEC_FRAMETYPE_P             2
#define  VDEC_FRAMETYPE_B             3 
#define  VDEC_FRAMETYPE_INTERPOLATED  4 

/* for backward compatibility between VDEC cwin change and RealPlayer */
#define QTV_VDEC_FRAME_USES_CWIN

/* Structure used to manage frame buffers in the decoder.*/
typedef struct
{
  VDEC_STREAM_ID    stream;   /* who owns this frame?                       */
  VDEC_BYTE *       pBuf;      /* where is it?                               */
  VDEC_DIMENSIONS   dimensions; /* height and width including any padding     */
  VDEC_RECTANGLE    cwin;     /* how big is cropping window */
  VDEC_FRAME_FORMAT format;    /* how's it formatted?                        */
  uint64     timestamp; /* timestamp for this frame, in ms since
                                  beginning of clip - set by vdec_decode
                                  when it returns the frame to the app       */
  unsigned short    layer;     /* layer ID from which this frame comes       */

  VDEC_FRAMETYPE    frameType;  /* the frame type                            */
  VDEC_EXT_FRAME    extFrame;    /* extended frame data                      */
  void*             pMetaData;   /* Pointer to metadata that is malloc'd and */ 
                                /* freed by the decoder. This pointer is    */
                                /* transported to upper layers in a         */
                                /* a VDEC_FRAME. It is freed when the frame */
                                /* buffer is released.                      */
  uint32   frameStatus;      /* frame is with UI or renderer */
}
VDEC_FRAME;

/* Structure used to manage input data.*/
typedef struct
{
  /* Number of layers in use.*/
  int     layers;
  /* The bitstream buffers.  If more than one layer, [0] is always the base.*/
  uint8  *buffer     [ VDEC_MAX_LAYERS ];
  /* IN/OUT param:
  ** Presentation timestamps for data in each layer. */
  uint64  timestamp  [ VDEC_MAX_LAYERS ];

  /* duration for the data in each layer. */
  uint32  delta      [ VDEC_MAX_LAYERS ];

  /* buffer_size must be filled by the application with the number of
  ** bytes available in each buffer.
  ** buffer_size is altered by the decoder, to indicate how many
  ** unprocessed bytes remain in each buffer.*/
  uint32  buffer_size[ VDEC_MAX_LAYERS ];

  /* IN/OUT param:
  ** IN: current position for the decoder to read.
  ** OUT: position of unread data remaining in each buffer.  */
  uint32  buffer_pos[ VDEC_MAX_LAYERS ];

  /* OUT:
  **
  ** The frameEventPending flag is set by the decoder to indicate which layers
  ** were consumed past an end-of-frame: in other words, to indicate
  ** from which layers we can expect a decode-done event soon.
  **
  ** The application should not write into this array, it is strictly
  ** for decoder output.
  */
  boolean bFrameEventPending[ VDEC_MAX_LAYERS ];

  /* userData is provided as a convenience for applications that wish
  ** to associate additional information with input buffers.  It is entirely
  ** unused by VDEC.
  */
  void* userData[ VDEC_MAX_LAYERS ];

  /* end of stream flag. Decoder sets this flag to indicate end of stream to DSP*/
  boolean eOSIndicator[ VDEC_MAX_LAYERS ];

}
VDEC_INPUT_BUFFER;



/* Structure used to provide performance statistics.
*/
typedef struct
{
  char ftyp[VDEC_FOURCC_STRLEN]; /* the kind of media we're decoding                        */

  /* Common stats to all types
  */
  uint32  nFrames; /* total frames decoded                           */

  /* Stats unique to particular types
  */
  union
  {
    /* media types compatible with 'isom' use this struct
    */
    struct
    {
      uint32   nIFrames;               /* number of I-frames decoded */
      uint32   nPFrames;               /* number of P-frames decoded */
    }
    mpeg;
  } media_dependent;
}
VDEC_STATS;

/* The BLOB is used to hide media-specific data from the application,
** who usually doesn't care about it anyway.
**
** BLOBs are allocated with enough memory to hold the buffer,
** which is an array of length 'size' beginning with 'data'.
*/
typedef struct
{
  char       fourcc[VDEC_FOURCC_STRLEN];
                             /* the fourcc code for the media type described
                             ** by the data in this BLOB. */
  int        type;           /* type enum in vdecoder_types_i.h */
  uint32     size;           /* bytes in the 'data' buffer      */
  VDEC_BYTE  data[1];        /* The opaque data buffer - this is bigger than
                             ** it looks! */
}
VDEC_BLOB;

/* The VDEC_AUDIO_CONFIG is used to provide audio information for combined
** audio/video DSP initialization.
*/
typedef unsigned long VDEC_CONCURRENCY_CONFIG;

#define  VDEC_CONCURRENT_NONE                        0
#define  VDEC_CONCURRENT_AUDIO_EVRC                  1
#define  VDEC_CONCURRENT_AUDIO_QCELP                 2
#define  VDEC_CONCURRENT_AUDIO_MIDI                  3
#define  VDEC_CONCURRENT_AUDIO_AMR                   4 
#define  VDEC_CONCURRENT_AUDIO_AAC                   5  
#define  VDEC_CONCURRENT_AUDIO_AAC_PLUS              6
#define  VDEC_CONCURRENT_AUDIO_MP3                   7
#define  VDEC_CONCURRENT_AUDIO_G723                  8
#define  VDEC_CONCURRENT_AUDIO_CONC                  9
#define  VDEC_CONCURRENT_AUDIO_REAL                 10 
#define  VDEC_CONCURRENT_AUDIO_AMR_WB               11
#define  VDEC_CONCURRENT_AUDIO_AMR_WB_PLUS          12
#define  VDEC_CONCURRENT_AUDIO_DUBBING              13
#define  VDEC_CONCURRENT_VIDEO_MPEG4_ENCODER        14 
#define  VDEC_CONCURRENT_VIDEO_H264_ENCODER         15
//audio override formats
#define  VDEC_CONCURRENT_AUD_OVER_ENC_QCELP13K      16 
#define  VDEC_CONCURRENT_AUD_OVER_ENC_AMR           17
#define  VDEC_CONCURRENT_AUD_OVER_ENC_EVRC          18

//QVP override formats
#define  VDEC_CONCURRENT_QVP_OVER_ENC_EVRC          19
#define  VDEC_CONCURRENT_QVP_OVER_ENC_EVRC1P        20
#define  VDEC_CONCURRENT_QVP_OVER_ENC_AMR           21
#define  VDEC_CONCURRENT_QVP_OVER_ENC_AMR1P         22
#define  VDEC_CONCURRENT_QVP_OVER_ENC_4GV           23
#define  VDEC_CONCURRENT_QVP_OVER_ENC_4GV1P         24
#define  VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB         25
#define  VDEC_CONCURRENT_QVP_OVER_ENC_4GVWB1P       26

#define  VDEC_CONCURRENT_AUDIO_BSAC                 27
#define  VDEC_CONCURRENT_QVP_OVER_ENC_EVRC05        28
#define  VDEC_CONCURRENT_QVP_OVER_ENC_AMR05         29

/* Enumeration of all the supported VDEC parameters.*/
typedef uint32 VDEC_PARAMETER_ID;

#define  VDEC_PARM_DIMENSIONS                      0
#define  VDEC_PARM_START_CODE_DETECTION            1
#define  VDEC_PARM_TIME_INCREMENT_RESOLUTION       2 
#define  VDEC_PARM_SIZE_OF_NAL_LENGTH_FIELD        3 
#define  VDEC_PARM_DEBLOCKER_ENABLE                4
#define  VDEC_PARM_FRE_ENABLE                      5
#define  VDEC_PARM_ENABLE_XSCALING                 6
#define  VDEC_PARM_LCDB_STRENGTH                   7
#define  VDEC_PARM_EXTERNAL_CLOCK_ENABLE           8
#define  VDEC_PARM_ENABLE_VLD_IN_DSP               9
#define  VDEC_PARM_SET_TS_MULTIPLY_FACTOR_BRAZIL   10
#define  VDEC_PARM_AUDIO_VTYPE                     11
#define  VDEC_PARM_MODE_SELECT                     12
#define  VDEC_PARM_ENABLE_VIDEO_AT_KEY_FRAME       13
#define  VDEC_PARM_ENABLE_UBM_ACCELERATION         14
#define  VDEC_PARM_QVP_ERR_CONCEAL                 15

/* Each VDEC parameter is tied to a corresponding VDEC parameter stucture,
** below.
*/

typedef struct
{
  /* QvpErrConceal: TRUE enables QVP Error concealment algorithm
  **                FALSE disables it.
*/
  boolean bEnableQvpErrConceal;
}
VDEC_QVP_ERR_CONCEAL;

typedef enum 
{
  VTYPE_NORMAL_MODE = 0,
  VTYPE_STEP_MODE
}DECODE_MODE_SELECTION;

typedef struct
{
  /* Mode Selection for decoder */
  DECODE_MODE_SELECTION mode;
}
VDEC_MODE_SELECT;

typedef struct
{
  /* sizeOfNalLengthField: the size of the NAL length field */
  uint32 sizeOfNalLengthField;
}
VDEC_SIZE_OF_NAL_LENGTH_FIELD;

typedef struct
{
  /* timeIncrementResolution: the time increment resolution. */
  uint32 timeIncrementResolution;
}
VDEC_TIME_INCREMENT_RESOLUTION;

typedef struct
{
  /* bDeblockerEnable: TRUE enables post-loop deblocking, FALSE
  **                   disables it.
*/
  boolean bDeblockerEnable;
}
VDEC_DEBLOCKER_ENABLE;


typedef struct
{
  /* bStartCodeDetectionEnabled: TRUE means detection is enabled, FALSE
  **                             means it is disabled. 
*/
  boolean bStartCodeDetectionEnable;
}
VDEC_START_CODE_DETECTION;

typedef struct
{
  /* bFreEnable: TRUE enables FRE, FALSE disables it. */
  boolean bFreEnable;
}
VDEC_FRE_ENABLE;

typedef struct
{
  /* bExternalClock: TRUE enables FRE, FALSE disables it. */
  boolean bExternalClock;
}
VDEC_EXTERNAL_CLOCK_ENABLE;

typedef struct
{
  /*0 – indicates use pretuned default values(provided by the first word following this variable).
    1 – user configures the LCDB setting with the below 7 parms*/
  boolean bLCDBThreshold;

  /* LCDB strength settings */
  uint16 LCDB_INTRA_MB_QP_THD0; 
  uint16 LCDB_INTER_MB_QP_THD_BND0;
  uint16 LCDB_INTER_MB_QP_THD_INSIDE0;
  uint16 LCDB_INTER_MB_QP_THD_BND4;
  uint16 LCDB_INTER_MB_QP_THD_INSIDE4;
  uint16 LCDB_NUM_NC_BLKS_THD;
  uint16 LCDB_QP_JUMP_NOTCODED_BLKS;
}
VDEC_LCDB_STRENGTH;

typedef struct
{
  /* bScalingEnable: TRUE enables xscaling, FALSE
  **                   disables it.
*/
  boolean bScalingEnable;
}
VDEC_XSCALING_ENABLE;

typedef struct
{
  /* TRUE enables VLD in DSP if possible, FALSE disables it.*/
  boolean bEnableVDLinDSP;
}
VDEC_ENABLE_VLD_IN_DSP;

typedef struct
{
  /* TS multiplication factor value*/
  boolean TSMultiplyFactor;
}
VDEC_SET_TS_MULTIPLY_FACTOR_BRAZIL;

typedef struct
{
/* Enable Video at Key frame*/
  boolean bEnableVideoAtKeyFrame;
}
VDEC_ENABLEVIDEO_AT_KEY_FRAME;

/*Audio vocoder types during incall*/
typedef enum
{
  VTYPE_NONE = 0,
  VTYPE_AMR,
  VTYPE_13K,
  VTYPE_EVRC,
  VTYPE_4GV
}
VDEC_AUDIO_VTYPE;

typedef struct
{
  /* vocoder type*/
  VDEC_AUDIO_VTYPE vtype;
}
VDEC_SET_AUDIO_CONC_VTYPE;

typedef struct
{
  /* Use UBM Acceleration*/
  boolean bUseUBMAcceleration;
}
VDEC_UBM_ACCELERATION;

/* Structure containing all the parameter info required for the
** vdec_get_parameter() and vdec_set_parameter() APIs.
*/
typedef union
{
  VDEC_DIMENSIONS                 dimensions;        
  VDEC_START_CODE_DETECTION       startCodeDetection;
  VDEC_TIME_INCREMENT_RESOLUTION  timeIncrementResolution;
  VDEC_SIZE_OF_NAL_LENGTH_FIELD   sizeOfNalLengthField;
  VDEC_DEBLOCKER_ENABLE           deblockerEnable;
  VDEC_FRE_ENABLE                 freEnable;
  VDEC_LCDB_STRENGTH              lcdbStrength;
  VDEC_XSCALING_ENABLE            xScalingEnable;
  VDEC_ENABLE_VLD_IN_DSP          VLDinDSP;
  VDEC_EXTERNAL_CLOCK_ENABLE      externalClockEnable;
  VDEC_SET_TS_MULTIPLY_FACTOR_BRAZIL     TSMultiplyFactor;
  VDEC_ENABLEVIDEO_AT_KEY_FRAME   VidKeyFrameEnable;
  VDEC_SET_AUDIO_CONC_VTYPE       audio_vtype;
  VDEC_MODE_SELECT                mode;
  VDEC_UBM_ACCELERATION           UBMEnable;
  VDEC_QVP_ERR_CONCEAL            QvpErrConceal;
}
VDEC_PARAMETER_DATA;



/* List of all dec instances */
typedef struct
{
  boolean bValid;
  VDEC_STREAM_ID StreamID;
  char fourCC[VDEC_FOURCC_STRLEN];
  rex_crit_sect_type cs;
}
VDEC_INSTANCE_LIST;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_QTV_VIDEO_DECODER_PROFILING
#error code not present
#endif
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


typedef void * (*VDEC_MALLOC_FN)( const VDEC_STREAM_ID   stream, 
                                  void *         pCbData,
                                  uint32         uSize );

typedef void     (*VDEC_FREE_FN)( const VDEC_STREAM_ID   stream,
                                  void *         pCbData, 
                                  void *         ptr );


/* Callback function used to return input buffers to the application.
**
** Parameters:
**
**   stream:  the stream ID of the decoder returning the buffer.
**   pInput:  the input buffer, which the decoder has either finished
**            processing, or which is being flushed (and is therefore
**            unread or only partially read).  How can we tell which it is?
**            check the read pointer in the input buffer.  Is it at the end?
**   pCbData: the client data the application passed in when it called
**            vdec_queue.
*/
typedef void (*VDEC_QUEUE_CB_FN)( const VDEC_STREAM_ID      stream,
                                  VDEC_INPUT_BUFFER * const pInput,
                                  void * const              pCbData );

/* Callback function used to return frame buffers to the application.
**
** Parameters:
**
**   stream:  the stream ID of the decoder returning the buffer.
**   status:          the reason for the callback. The pFrame parameter 
                      should only be used if the status indicates a  
                      successful decode.
**   nBytesConsumed:  the number of bytes consumed from the queued
**                    bitstream to produce the frame.
**   pQueueCbData:    the client data the application passed in when it 
**                    provided the frame buffer in the first place.
*/

typedef void (*VDEC_EVENT_CB_FN)( const VDEC_STREAM_ID  stream,
                                  VDEC_CB_STATUS        status,
                                  VDEC_FRAME * const   pFrame,
                                  uint32                nBytesConsumed,
                                  void * const          pEventCbData );

/* Callback function used to return status of vdec_scale. Called
** when the input frame size was not available when vdec_scale
** was first called.
**
** Parameters:
**
**   stream:  the stream ID of the decoder returning the scale status.
**   scaleDone:  the status whether scale command was successful
**   pCbData: the client data the application passed in when it called
**            vdec_scale.
*/
typedef void (*VDEC_SCALE_CB_FN)( const VDEC_STREAM_ID      stream,
                                  const boolean             scaleDone,
                                  void * const              pCbData );

#ifdef __cplusplus
}
#endif

#endif /* VDECODER_TYPES_H */

