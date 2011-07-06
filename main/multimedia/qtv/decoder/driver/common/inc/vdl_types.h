#ifndef VDL_TYPES_H
#define VDL_TYPES_H
/* =======================================================================
                               DLTypes.h
DESCRIPTION
  This file has the data structures that will not only be exposed outside VDL (along
  with VDL-API) but will also be used within VDL
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/driver/common/rel/2.0/inc/vdl_types.h#5 $
$DateTime: 2010/02/02 00:16:07 $ 
$Change: 1161143 $

========================================================================== */

/*==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "queue.h"
#include "comdef.h"


/*==========================================================================

                       DATA DECLARATIONS

========================================================================== */

typedef enum
{
  VDL_ERR_NONE = 0,   
  VDL_ERR_NULL_STREAM_ID,   
  VDL_ERR_INVALID_STREAM_ID,
  VDL_ERR_INIT_FAILURE,      
  VDL_ERR_INVALID_STATE,      
  VDL_ERR_OUT_OF_MEMORY,
  VDL_ERR_TERMINATE_FAILURE,
  VDL_ERR_MYSTERY

}VDL_ERROR;

typedef struct
{
  uint16 PacketID;
  uint16 Num_MB_Concealed;
  uint16 Num_Intra_MB;
  uint16 Frame_Decode_Time_High;
  uint16 Frame_Decode_Time_Low;
  uint16 EndofStatsMarker;
} VDL_Frame_Stats_Pkt_Type;     /* Stats packet structure */

typedef struct
{
  q_link_type   link;
  VDL_Frame_Stats_Pkt_Type *pStats_pkt;
}VDL_FrameStats_Pkt_Type;

typedef struct
{
  q_link_type   link;             /* buffer queue link    */
  uint16        BufferID;         /* buffer identification number */
  uint16        SliceSeqNum; /* Queue counter comon between slice and PP buffers*/
  uint16        *pNextMacroBlock; /* pointer to the next macro block to send */

  uint16        CodecType;        

  uint16        NumMacroBlocks;   /* number of macro blocks in this buffer */
  uint16        NumMacroBlocksToSend;
  uint16        fFirstMB;         /* indicates first MacroBlock in a frame */
  uint16        fLastMB;          /* indicates last MacroBlock in a frame */
  void          *pSliceData;      /* slice data */
#ifdef FEATURE_L4_WRITEBACK_CACHE
#error code not present
#endif
  uint16        SubframePktSeqNum;/* indicates subframe packet sequence number */
  uint32        SliceDataSize;    /* indicates size of subframe in bytes */
  uint32        SliceBufferSize;  /* indicates size of slice buffer allocated */
  boolean       fAllocateYUVBuf;  /* if true, lower levels should allocate a new */
                                  /* video frame to hold output from this slice */
  boolean isFlushMarker;          /* flush marker for processing flush */
  boolean isSubFrameDone;
  VDL_Frame_Stats_Pkt_Type *pFrameStatsInPtr; /*Frame Statictics holder*/
  VDL_FrameStats_Pkt_Type *pFrameStatsOutPtr; /*Frame Statictics ptr.filed used by DL*/
  boolean isStatsFromL2;           /*used by DL to figure from where the stats cam from*/
} VDL_Slice_Pkt_Type; /* slice packet structure */

typedef struct
{
  q_link_type   link;             /* buffer queue link    */
  uint16        BufferID;         /* buffer identification number */
  uint16        SliceSeqNum; /* Queue counter comon between slice and PP buffers*/
  uint16        CodecType; 
  void          *PP_Output_ptr;  /*PP output address info*/
  uint16        postproc_info_1; /*Load Sharing Details*/
  uint32        LoadSharingPktSize; /*Load Sharing pakcet size*/
  void          *LoadSharingPktAddr; /*Load sharing data pkt addr*/
  uint16        pp_Param_0; /*This is a post-processing command specific input */
  uint16        pp_Param_1; /*This is a post-processing command specific input */
  uint16        pp_Param_2; /*This is a post-processing command specific input */
  uint16        pp_Param_3; /*This is a post-processing command specific input */
  boolean isFlushMarker;    /* flush marker for processing flush */
  boolean isPPDone;         /*Like SubframeDone */
} VDL_PP_cmd_Info_Type; //PP Cammand information paramaters*/

typedef enum
{
  VDL_FRAME_FLUSHED          =-1, /* frame flushed */
  VDL_DECODE_NO_ERROR        = 0, /* no frame error*/
  VDL_DECODE_FRAME_ERROR     = 1, /* frame error */    
  VDL_FATAL_ERROR            = 2, /* fatal error, restart decoder */    
  VDL_DECODE_FRAME_NOT_CODED = 3, /* the frame is a not coded frame */
  VDL_OUT_OF_MEMORY          = 4, /* out of memory */
  VDL_DECODE_FRAME_FRE       = 5,      
  VDL_SUSPEND_NO_ERROR       = 6,
  VDL_EOS_FRAME = 7,         /*EOS frame used only for VLD in DSP case for now*/
  VDL_FRAME_FRE_FAILED = 8   /*used in VLD in DSP interface only*/
} VDL_Decode_Return_Type; /* return codes used in the decoder callback function*/


typedef enum
{
  VDL_AUDIO_NONE = 0,
  VDL_AUDIO_EVRC8K,
  VDL_AUDIO_QCELP13K,
  VDL_AUDIO_AMR,
  VDL_AUDIO_AAC,
  VDL_AUDIO_BSAC,
  VDL_AUDIO_AAC_PLUS,
  VDL_AUDIO_MP3,
  VDL_AUDIO_WMA,
  VDL_AUDIO_CONC,
  VDL_AUDIO_REAL,
  VDL_AUDIO_AMR_WB,
  VDL_AUDIO_AMR_WB_PLUS,
  VDL_VIDEO_MPEG4_ENCODER,
  VDL_VIDEO_H264_ENCODER,
  VDL_AUD_OVER_ENC_QCELP13K,
  VDL_AUD_OVER_ENC_AMR,
  VDL_AUD_OVER_ENC_EVRC,
  VDL_QVP_OVER_ENC_EVRC,
  VDL_QVP_OVER_ENC_EVRC05,
  VDL_QVP_OVER_ENC_EVRC1P,
  VDL_QVP_OVER_ENC_AMR,
  VDL_QVP_OVER_ENC_AMR05,
  VDL_QVP_OVER_ENC_AMR1P,
  VDL_QVP_OVER_ENC_4GV,
  VDL_QVP_OVER_ENC_4GV1P,
  VDL_QVP_OVER_ENC_4GVWB,
  VDL_QVP_OVER_ENC_4GVWB1P,
  VDL_AUDIO_CONC_13K,
  VDL_AUDIO_CONC_EVRC,
  VDL_AUDIO_CONC_AMR,
  VDL_AUDIO_CONC_4GV,
  VDL_AUDIO_MAX
} VDL_Concurrency_Type;

typedef enum
{
  VDL_VIDEO_NONE = 0,
  VDL_VIDEO_MPEG4,
  VDL_VIDEO_H263,
  VDL_VIDEO_H264,
  VDL_VIDEO_H263P3,
  VDL_VIDEO_WMV,
  VDL_VIDEO_REAL,
  VDL_VIDEO_OSCAR,
  VDL_VIDEO_MAX
} VDL_Video_Type;

typedef enum
{
  VDL_PROFILE_SIMPLE = 0,
  VDL_PROFILE_MAIN,
  VDL_PROFILE_EXTENDED,
  VDL_PROFILE_MAX
}VDL_Profile_Type;

typedef enum
{
  VDL_CODEC_MPEG4 = 0,     
  VDL_CODEC_H263_P0,  
  VDL_CODEC_H264,      
  VDL_CODEC_H263_P3,  
  VDL_CODEC_RV9,     
  VDL_CODEC_WMV9,      
  VDL_CODEC_FRUC = 0x7,
  VDL_CODEC_H264_VLD = 0x8
}VDL_Codec_Type;

typedef struct
{
  uint16   VideoConfiguration;  /* bpp, dc prediction flag */
  uint16   XDimension;          /* output video frame width */
  uint16   YDimension;          /* output video frame length */
  uint16   Operations;          /* indicates tasks that QDSP performs */
  uint16   ImageScale;          /* image scaling specification */
  uint16   ImageRotation;       /* rotation image +- 90 degrees */
  uint16   DeblockType;         /* deblock filter type */
} VDL_Decode_Info_Type;

typedef struct
{
  /* REV C/C1 specific parameters */
  void **pYUVBuf ;           /* pointer to YUV buffer */
  void *pFilteredYUVBuf;    /* pointer to Filtered buffer */
  void **pOutputYUVBuf ;     /* pointer to Output buffer */
  uint16 *pPQuantTable;      /* pointer to the P quantization table */
  uint16* *pIQuantTable;     /* pointer to the I quantization table */
  void *pPaletteIndex;
  VDL_Decode_Info_Type DecodeInfo;
  /* VLD RTOS specific parameters */
  boolean vld_dsp_flag;
  boolean bSVHMode;
}VDL_CONFIG_PARAMS;

typedef void (* VDL_Decoder_Cb_Type)     /* decoder callback function definition*/
(
VDL_Decode_Return_Type ReturnCode,    /* return status code */
void *pDecodeStats,  /* return decode frame statistics */
void               *pUserData      /* user data */
);

typedef enum
{
  VDL_INTERFACE_NONE = 0,
  VDL_INTERFACE_ARM,
  VDL_INTERFACE_REVC_DMA,
  VDL_INTERFACE_REVC1_DMA,
  VDL_INTERFACE_REV2A_DMA,
  VDL_INTERFACE_REV2A_DME,
  VDL_INTERFACE_RTOS,
  VDL_INTERFACE_RTOS_VLD_DSP,
  VDL_INTERFACE_MAX
}VDL_Interface_Type;



#endif

