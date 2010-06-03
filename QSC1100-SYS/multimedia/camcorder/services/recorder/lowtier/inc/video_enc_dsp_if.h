#ifndef VIDEO_ENC_DSP_IF_H
#define VIDEO_ENC_DSP_IF_H

/*=============================================================================
FILE:       VIDEO_ENC_DSP_IF.h

SERVICES:   Video Encode DSP Interface

DESCRIPTION:
   This file provides necessary interfaces for the Video Encoder that
   ultimately drives the Video Encode Backend of the DSP.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 QUALCOMM Incorporated.
        All Rights Reserved.
        QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/lowtier/main/latest/inc/video_enc_dsp_if.h#1 $ $DateTime: 2008/05/01 11:10:49 $ $Author: karenp $

CAMIF Version Numbers:

ABSTRACT:
   
============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/06   gopi    Initial version for 6260.
04/12/05   alin    Add VideoEncDSPACDCPredictionType
09/13/05   dw      Allow different DSP images to be loaded for Raw Mode
07/25/05   dw      Sync with csim - changes to include new header file
06/01/05   jk      Passed CAMQDSP_ImageType to VideoEncDSP_Initialize for
                   concurrent DSP.
04/22/05   dw      Added raw mode support
02/22/05   wyh     Raw Encoder API changes.
02/03/05   dw      Updated CAMQDSP selection command with HW profile flag 
01/24/05   pp/jk   Made compatible with c-sim.
12/05/04   wyh     Added the VideoEncSleep command. Define 
                   VIDEO_ENC_DSP_OFFLINE_ENCODING to 0xFFFF (from -1) to 
                   remove a compile warning message. 
12/02/04   ali     Changes for transcoder
12/02/04   wyh     Backend Confic Command added requestIFrame flag.
11/29/04   drl     Added VideoEncDSP_VideoEncRawFrame 
11/03/04   jk      WINNT target specific change only.
10/29/04   pp      Added Parametric Rate Control.
10/07/04   jk      Added intraFramePeriod to _VideoEncodeUpdateType
10/05/04   mvd     Renamed #include camqdsp_if to camqdsp - to satisfy those
                   silly camera guys.
09/06/04   pp/wh   Pass pointers only to VideoEncDSP_VideoEncSelection(),
                   VideoEncDSP_VideoEncConfiguration(), VideoEncDSP_VideoEnc-
                   Start(), and VideoEncDSP_VideoEncUpdate()..
07/13/04   drl     Creation

============================================================================*/

#ifdef T_ARM
#include "camqdsp.h"
#else
#include "qdsp_sim.h"
#endif

#include "ipl.h"


#define VIDEO_ENC_DSP_TBD void

typedef enum VideoEncDSPEnableType
{
  VIDEO_ENC_DSP_DISABLE = 0,
  VIDEO_ENC_DSP_ENABLE  = 1
} VideoEncDSPEnableType;

typedef enum VideoEncDSPModeType
{
  VIDEO_ENC_DSP_MPEG4 = 0,

#ifdef FEATURE_H264_ENCODER
  VIDEO_ENC_DSP_H264  = 1,
#endif

  VIDEO_ENC_DSP_H263  = 2
} VideoEncDSPModeType;

typedef enum VideoEncDSPACDCPredictionType
{
  ACDC_PRED_ENABLE_AC_DC    = 0x1,
  ACDC_PRED_DISABLE_AC_DC   = 0x0,
  ACDC_PRED_ENABLE_DC_ONLY  = 0x2
} VideoEncDSPACDCPredictionType;

typedef enum VideoEncDSPProfileType
{
  VIDEO_ENC_DSP_PROFILE_DISABLE = 0,
  VIDEO_ENC_DSP_PROFILE_ENABLE  = 1
} VideoEncDSPProfileType;

typedef enum VideoEncDSP_AnchorPointProgrammingType
{
  VIDEO_ENC_HW_USE_REGISTERS          = 0,
  VIDEO_ENC_HW_USE_16X16_SEARCH_POINT = 1
} VideoEncDSP_AnchorPointProgrammingType;

typedef enum VideoEncDSP_Use16x16SrchMinType
{
  VIDEO_ENC_HW_SEE_SRCH_ANCR_PT             = 0,
  VIDEO_ENC_HW_USE_MINX_Y_FROM_16X16_SEARCH = 1
} VideoEncDSP_Use16x16SrchMinType;

typedef enum VideoEncDSP_SrchType
{
  VIDEO_ENC_HW_16X16MB                         = 1,
  VIDEO_ENC_HW_4_INDIVIDUAL_8X8                = 2,
  VIDEO_ENC_HW_1ST_MB_SEARCH_FOLLOWED_BY_4_8X8 = 3
} VideoEncDSP_SrchType;

typedef enum VideoEncDSP_RotationFlagType
{
  VIDEO_ENC_DSP_NO_ROTATION          = 0,
  VIDEO_ENC_DSP_CLOCKWISE_90         = 1,
  VIDEO_ENC_DSP_180                  = 2,
  VIDEO_ENC_DSP_COUNTER_CLOCKWISE_90 = 3
} VideoEncDSP_RotationFlagType;

typedef enum VideoEncDSP_RequestType
{
  VIDEO_ENC_DSP_NO_REQUEST = 0,
  VIDEO_ENC_DSP_REQUEST    = 1
} VideoEncDSP_RequestType;

typedef enum VideoEncDSP_TranscodingMode
{
  VIDEO_ENC_DSP_OFFLINE_ENCODING = 0xFFFF,
  VIDEO_ENC_DSP_REALTIME_ENCODING = 0
} VideoEncDSP_TranscodingMode;

typedef __packed struct VideoEncDSP_VideoCodecType
{
  VideoEncDSPModeType    mode               :  2;
  VideoEncDSPEnableType  roundingBitCtrl    :  1;
  uint16                 reservednew        :  5;
  uint16                 ratecontrolbit     :  1;
  uint16                 reserved           :  7;
} VideoEncDSP_VideoCodecType;

typedef __packed struct VideoEncDSP_SrchTypeAndAncrType
{
  VideoEncDSP_AnchorPointProgrammingType srch_ancr_pt      :  1;
  VideoEncDSP_Use16x16SrchMinType        use16x16_srch_min :  1;
  VideoEncDSP_SrchType                   srchType          :  2;
  uint16                                 reserved          : 12;
} VideoEncDSP_SrchTypeAndAncrType;

typedef enum {
  QCAMCORDER = 0,
  QVIDEOPHONE = 1
} VideoAppSelectType;

typedef __packed struct 
{
  VideoAppSelectType                 videoAppSelect : 1;
  uint16                             reserved       :15;
} VideoAppSelect_Type;

typedef __packed struct  VideoEncDSP_EncoderSelectionCommandType 
{
  VideoEncDSP_VideoCodecType         videoCodec;
  uint16                             chooseModeME16x16Threshold;
  uint16                             fractionPelSearchEnable;
  uint16                             maxmesearches;
} VideoEncDSP_EncoderSelectionCommandType;

/*
** the Frame_status for videEncFrameDoneReg
*/

typedef __packed struct VideoEncDSP_FrameStatusType
{
  uint16     request_not_coded         :1;  /* arm sent a request not coded     */
  uint16     image_sensor_data_corrupt :1;
  uint16     arm_late                  :1;
  uint16     dsp_late                  :1;
  uint16     size_overflow             :1;  /* packet size over external buffer */
  uint16     dsp_late_2_or_more_frames :1;  /* something wrong with DSP?        */
  uint16     reserved                  :10;
} VideoEncDSP_FrameStatusType;


typedef struct VideoEncDSP_ProfileStatusType
{
  uint16     framePktNum;
  uint16     mbAvgFetchCycles;
  uint16     mbAvgSearchCycles;
  uint16     mbAvgNumMbSearch;
  uint16     mbAvgNumBlkSearch;
} VideoEncDSP_ProfileStatusType;

typedef __packed struct VideoEncDSP_EncRotationType
{
  VideoEncDSP_RotationFlagType rotationFlag :  2;
  unsigned int                 reserved     : 14;
} VideoEncDSP_EncRotationType;


typedef __packed struct
{
  boolean               requestIntraFrame : 1;
  uint16                reserved          :15;
} VideoEncDSP_ReqIFrameType;

typedef enum videoEncInputFormatEnum
{
  VIDEO_ENC_INPUT_FORMAT_MB_BY_MB     = 0,
  VIDEO_ENC_INPUT_FORMAT_LINE_BY_LINE = 1
} videoEncInputFormatEnum;

typedef enum videoENCYCbCrFromatEnum
{
    VIDEO_ENC_INPUT_CB_CR_INTERLEAVED = 0,
    VIDEO_ENC_INPUT_YCB_CR_PLANAR     = 1
}videoENCYCbCrFromatEnum;

typedef struct
{
  videoEncInputFormatEnum           input_format: 1;
  videoENCYCbCrFromatEnum           ycbcrformat : 1;
  uint16                            reserved    :14;
} VideoEncDSP_InputFormatType;

typedef struct VideoEncDSP_EncoderConfigurationType
{
  uint16                      encodeWidth;
  uint16                      encodeHeight;
  uint16                      intraFramePeriod;
  uint16                      quantizerScaleIntra;
  uint16                      quantizerScaleInter;
  VideoEncDSP_EncRotationType videoEncRotation;
  uint16                      encVopReconBuf1Ptr_High;
  uint16                      encVopReconBuf1Ptr_Low;
  uint16                      encVopReconBuf2Ptr_High;
  uint16                      encVopReconBuf2Ptr_Low;
  uint16                      encodedFrameBuf1PtrHigh;
  uint16                      encodedFrameBuf1PtrLow;
  uint16                      encodedFrameBuf2PtrHigh;
  uint16                      encodedFrameBuf2PtrLow;
  uint16                      encodeFrameBuf1Size_High;
  uint16                      encodeFrameBuf1Size_Low;
  uint16                      encodeFrameBuf2Size_High;
  uint16                      encodeFrameBuf2Size_Low;
  VideoEncDSP_InputFormatType videoEncInputFormat;
  uint16                      maxFrameQpDownDelta;
  uint16                      maxFrameQpUpDelta;
  uint16                      Frame_rho_budget_high_word;
  uint16                      Frame_rho_budget_low_word;
  uint16                      requestIntraFrame;
  uint16                      mp4eZeroBlockLumaThreshold;
  uint16                      mp4eZeroBlockChromaThreshold;
  uint16                      lumaFilterEnable;

} VideoEncodeDSP_EncoderConfigurationType;

typedef __packed struct VideoEncodeDSP_RequestIntraFrameType
{
  VideoEncDSP_RequestType armRequestIntraFrameEncode :  1;
  unsigned int            reserved                   : 15;
} VideoEncodeDSP_RequestIntraFrameType;
         
typedef __packed struct VideoEncodeDSP_RequestNotCoded
{
  VideoEncDSP_RequestType requestNotCoded :  1;
  unsigned int            reserved        : 15;
} VideoEncodeDSP_RequestNotCoded;

typedef __packed struct  VideoEncodeDSP_EncoderStartType 
{
  uint16                      transcoderMode;
  uint16                      frameInputLumaPtrHigh;
  uint16                      frameInputLumaPtrLow;
  uint16                      frameInputChromaPtrHigh;
  uint16                      frameInputChromaPtrLow;
} VideoEncodeDSP_EncoderStartType;

typedef __packed struct VideoEncodeDSP_VideoEncodeUpdateType
{
  VideoEncodeDSP_RequestIntraFrameType requestIntraFrame;
  VideoEncodeDSP_RequestNotCoded       requestNotCoded;
  uint16                               fractionPelSearchEnable;
  uint16                               quantizerScaleInter;
  uint16                               quantizerScaleIntra;
  uint16                               intraFramePeriod;
  uint16                               Frame_rho_budget_high_word;
  uint16                               Frame_rho_budget_low_word;
} VideoEncodeDSP_VideoEncodeUpdateType;

typedef void (* VideoEncDSP_CBType) 
(
  CAMQDSP_ReturnCodeType result,    
  CAMQDSP_ResponseType  *response
);

CAMQDSP_ReturnCodeType VideoEncDSP_Initialize 
(
  CAMQDSP_MsgCBType   cb, 
  CAMQDSP_ImageType   app
);

CAMQDSP_ReturnCodeType VideoEncDSP_AudioCodecConfiguration (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_AudioConfiguration      (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_DTMFRequest             (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_AudioSleep              (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_AudioIdle               (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_AudioVocoderSelection   (VIDEO_ENC_DSP_TBD);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncSelection 
                          (VideoEncDSP_EncoderSelectionCommandType *config);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncConfiguration 
                          (VideoEncodeDSP_EncoderConfigurationType *config);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncStart 
                          (VideoEncodeDSP_EncoderStartType *config);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncUpdate 
                          (VideoEncodeDSP_VideoEncodeUpdateType *config);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncSleep(void);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncIdle  (void);

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncRawFrame (byte *frame_YAddress, 
                                                     byte *frame_CbCrAddress,
                                                     ipl_col_for_type image_type,
                                                     VideoEncDSP_CBType cb);

#ifdef FEATURE_MP4_TRANSCODER
CAMQDSP_ReturnCodeType VideoEncDSP_OfflineInitialize (CAMQDSP_MsgCBType cb)  ;
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_VIDEO_ENCODE_RAW
CAMQDSP_ReturnCodeType VideoEncDSP_RawInitialize (CAMQDSP_MsgCBType cb, 
                                                  CAMQDSP_ImageType app);
#endif /* FEATURE_VIDEO_ENCODE_RAW */


extern void VideoEncDSP_rawEncodeCb(
  CAMQDSP_ReturnCodeType result,
  CAMQDSP_ResponseType  *response
);


#endif /* VIDEO_ENC_DSP_IF_H */
