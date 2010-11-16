#ifdef FEATURE_QTV_XSCALE_VIDEO
#ifndef VPPXSCALAR_H
#define VPPXSCALAR_H
/* =======================================================================
                               vppXScalar.h
DESCRIPTION
  This header file contains the function definitions for the
  QTV Video Post Processing xScalar API.

Copyrighted by QUALCOMM Incorporated;
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/decoder/transform/common/rel/2.0/inc/vppxscalar.h#1 $
$DateTime: 2008/11/03 04:38:22 $
$Change: 775446 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define CMD_XSCALE_FRAME 0x0F00

/*
** Define a structure for the xScalar which is embedded into the Framehdr
*/
typedef struct
{
  uint16 cmdScaleFrame;          /* DSP scale cmd identifier            */
  uint16 FrameInfo;              /* user defined frame info             */
  uint16 xPhaseStepInt;          /* Horizontal phase step int part      */
  uint16 xPhaseStepFraction;     /* Horizontal phase step fraction part */
  uint16 yPhaseStepInt;          /* Vertical phase step int part        */
  uint16 yPhaseStepFraction;     /* Vertical phase step fraction part   */
  uint16 lumChromaScaling;       /* Used to specify luma and chroma scaling methods */

  uint16 xInputDim;              /* Input X Dimension in pixels        */
  uint16 yInputDim;              /* Input Y Dimension in pixels        */

  uint16 xOutputDim;             /* Output X Dimension in pixels       */
  uint16 yOutputDim;             /* Output Y Dimension in pixels       */

  uint16 sourceYUVBuf0Hi;        /* High 16 bits...                     */
  uint16 sourceYUVBuf0Lo;        /* Low 16 bits...                      */

  uint16 targetYUVBuf0Hi;        /* High 16 bits...                     */
  uint16 targetYUVBuf0Lo;        /* Low 16 bits...                      */

  uint16 xPolyphaseTapsHi;       /* High 16 bits of the table for horizontal taps */
  uint16 xPolyphaseTapsLo;       /* Low 16 bits of the table for horizontal taps  */

  uint16 yPolyphaseTapsHi;       /* High 16 bits of the table for vertical taps */
  uint16 yPolyphaseTapsLo;       /* Low 16 bits of the table for vertical taps */

} VPP_XScalarCmdType;

/* Error structure used to return information about scaling failures to the
** client.
*/
typedef enum
{
  VPP_XSCALE_ERR_EVERYTHING_FINE = 0,        /* no problemo                                                */
  VPP_XSCALE_ERR_SCALING_NOT_ACTIVE,         /* Xscaling not activated, so dont scale frames               */
  VPP_XSCALE_ERR_UNSUPPORTED_SCALING_FACTOR, /* we can't we can't support the req scaling(up/down factor   */
  VPP_XSCALE_ERR_NULL_POINTER,               /* one of the data pointers is null                           */
  VPP_XSCALE_ERR_NO_LUMA_NO_CHROMA_SCALING,  /* the user did not request either luma or chroma 
                                         ** scaling although Xcaling was activated                     */
  VPP_XSCALE_ERR_INVALID_BUF_SIZE,           /* the input buffer is small to hold the xscale cmd           */
  VPP_XSCALE_ERR_INVALID_SCALING_DIMENSIONS,  /* Invalid scaling dimensions specified*/
  VPP_XSCALE_ERR_NO_SCALING_NEEDED             /* input/output has same dimesions so xscalar turned off*/
}
VPP_XSCALE_ERROR;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION
  vpp_xScale_frames

DESCRIPTION
  Used to turn the xScalar on/off.

PARAMETERS
  boolean bDoScaling
    Used to turn on/off the xScalar to scale vide frames. Only when its TRUE 
    the other parameters are interpreted.
    
  uint16  xDecDim
    Specifies the mod 16 decoder frame width

  uint16  yDecDim
    Specifies the mod 16 decoder frame height
    
  uint16  xInputDim
    Specifies the video frame input dimension, its input width
    
  uint16  yInputDim
    Specifies the video frame input dimension, its input height
    
  uint16  xOutputDim
    Specifies the video frame output dimension, its output width
    
  uint16  yOutputDim.
    Specifies the video frame output dimension, its output width

RETURN VALUE
  Throws an error if the scaling factor is not supported.

========================================================================== */
VPP_XSCALE_ERROR vpp_xScale_frames( boolean bDoScaling,
                                    uint16  xDecDim,
                                    uint16  yDecDim,
                                    uint16  xInputDim,
                                    uint16  yInputDim,
                                    uint16  xOutputDim,
                                    uint16  yOutputDim );

/* ======================================================================
FUNCTION
  vpp_xScalar_is_active

DESCRIPTION
  Used to check if the xScalar is on/off.

PARAMETERS
  None

RETURN VALUE
  Return ture if xScale was turned on by a previous call to 
  vpp_xScale_frames.

========================================================================== */
boolean vpp_xScalar_is_active(void);


/* ======================================================================
FUNCTION
  vpp_xScalar_pack_cmd

DESCRIPTION
  This function is used to construct the xScalar command body as per
  the arm-dsp spec.

PARAMETERS
  void*    scaleBuf
    Target buffer to pack the xScalar command
     
  uint32   bufSize
    Buffer size fo the target buffer
    
  unsigned char * srvYuvBuf
    Source address of the decoded frame
  
  unsigned char * dstYuvBuf
    Target address of the buffer to hold the xScaled frame
    
  boolean  scaleLuma
    Used to specify if luma should be scaled for this frame
    
  boolean  scaleChroma
    Used to specify if chroma should be scaled for this frame
    
  uint16   usrdata
    Any user specific data to be put in the xScalar cmd
    
  uint16*  xOutputDim
    Returns the output width of the xScaled frame. We need to do this
    bookkeeping since when the frame is returned from the dsp there is 
    no way do know what is the size of the returned xScale frame.
    
  uint16*  yOutputDim
    Returns the output height of the xScaled frame.

RETURN VALUE
  Returns an err if the input parameters fail validation.

DEPENDENCY
  vpp_xScale_frames and this function should be called from within the
  same task context.
========================================================================== */
VPP_XSCALE_ERROR vpp_xScalar_pack_cmd( void*    scaleBuf, 
                                       uint32   bufSize,
                                       unsigned char * srvYuvBuf,
                                       unsigned char * dstYuvBuf,
                                       boolean  scaleLuma,
                                       boolean  scaleChroma,
                                       uint16   usrdata,
                                       uint16*  xOutputDim,
                                       uint16*  yOutputDim );

/* ======================================================================
FUNCTION
  vpp_xScalar_adjust_scaled_dim

DESCRIPTION
  Temp workaround until DSP supports non-mod 16 clip dimensions for xScalar.
  Lets say we are trying to scale 170x144 -> 320x240. To get this scaled output
  we need to tell DSP to scale 176x144 -> (322x170)/176x240 and then crop the extra
  scaled pixels.

PARAMETERS
  uint16 reqOutputDim
    Original requested scaled dimension
    
  uint32 SrcDim
    Non-mod 16 src clip dimension
    
  uint32* DecodedDim
    Mod-16 padded src clip dimension
    
  uint32* ScaledDim
    This represents the new scaled dimension
    
  VPP_XScalarCmdType* xScalarBuffer
    xScalar cmd body required to reflect the change.

  boolean xDimension
    ture => we are adjusting the width
    false => we are adjusting the height

RETURN VALUE
  Returns an err if required.

DEPENDENCY
  None.
========================================================================== */
VPP_XSCALE_ERROR vpp_xScalar_adjust_scaled_dim( uint16 reqOutputDim,
                                                uint32 SrcDim,
                                                uint32* DecodedDim,
                                                uint32* ScaledDim,
                                                VPP_XScalarCmdType* xScalarBuffer,
                                                boolean xDimension );


#endif //VPPXSCALAR_H
#endif
