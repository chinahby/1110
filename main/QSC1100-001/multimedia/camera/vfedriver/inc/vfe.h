#ifndef VFE_H
#define VFE_H

/*=============================================================================
FILE:       VFE.h

SERVICES:   Video Front End Interface

DESCRIPTION:
   This file provides necessary interfaces for the Video Front End (VFE) that
   ultimately drives the camera.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 - 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/vfedriver/main/latest/inc/vfe.h#9 $ $DateTime: 2009/02/12 03:17:10 $ $Author: c_caluru $

CAMIF Version Numbers:

ABSTRACT:

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/09   cg      Added Support AEC Multishot 1.1
12/24/08   cg      Changes for Ez Tune 1.0
11/24/18   chai    Fixed compiler errors and warnings
11/03/08   cg     Added support for Eztune 1.0, 3A 1.4 and 0x203 header
05/08/08   cg     Added Support for YCBCR 420 Snpashot for VFE05
03/11/08   cg      a) Added the support for new VFE firmware image for ULC
                              b) Added the function vfe_get_type().
01/21/08   cg      Lens Roll Off can be re-configured while VFE is in capture state in LCU(QSC6270).
11/08/07   hap     Fixing RVCT2.2 compiler warnings
10/09/07   arv     Added changes for VFE25 ABF and ASF - preview and snapshot
03/15/07   klee    Updated preview re-configuration on re-starting
03/14/07   klee    Removed unnecessary conditional statement which is always TRUE
03/12/07   klee    Modified by 3A-restructuring
02/08/07   dle     Fixed lint errors by replacing enums with defines.
11/20/06   jv      Removed static saturated color conversion matrix.
10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
08/30/06   klee    Removed orange/green emphasized CV matrix support
08/24/06   ven     Added support for Offline snapshot for msm6260
07/05/06   klee    Added 3 APIs to expose color matrix pointer
06/02/06   ven     Added support for VFE05 for 6260.
05/26/06   dle     Removed vfeSensorConfig from vfe_copy_sensor_config() now
                   the configuration tables are in vfe.c.
05/25/06   jv      Changed VFE_AsfCfgType.enable from int32 to VFE_Enable_type.
                   Updated vfe_start() to expect pointer to camsensor_static_params
                   and return camera_ret_code_type
04/21/06   jch     Changes in VFE interface for burst mode support
04/10/05   dle     Reverted featurization around ASF.
04/05/06   dle     Enable with FEATURE_CAMERA_VFE_16X16_SUPPORT.
03/24/06   sch     VFE20 Support.
03/20/06   klee    Added 6 vfe variables from camera_svcs layer as extern.
                   Renamed vfe_features_auto_detect() to be vfe_init()
                   Added new function vfe_start()
02/28/06   sch     Added enum vfe_clk_type for
                   FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK.
02/16/06   dle     Linted.
01/31/06   dle     Temporarily disabled 16x16 regions. Occasional data overflow
                   from VFE HW to DSP memory.
12/21/05   ven     Added support for dynamic selection of 4x4, 8x8,  16x16
                   vfe regions based on the current resolution of the preview.
12/14/05   ven     Added support for VFE 4X4 Statistics collection.
10/14/05   dle     Renamed VFE1X to VFE1P.
08/22/05   dle     Moved vfe_hw_feature_type and vfe_sw_feature_type to
                   camerai.h,
                   Modified vfe_init_rolloff_tables() to return boolean.
08/18/05   dle     Added vfe_sw_features. whiteBalance and rolloffCfg are
                   moved to the end.
                   VFE config functions now expect configuration pointer.
                   Added vfe_features_auto_detect.
08/01/05   dl      extern vfe_hw_features.
07/28/05   sch     - New vfe_hw_func_type.
                   - New API
                   void vfe_hw_features_auto_detect (void)
06/28/05   sch     FEATURE_CAMERA_VFE_ROLLOFF -> MSMHW_VFE_ROLLOFF
                   FEATURE_CAMERA_VFE_WB -> MSMHW_VFE_WB
06/20/05   dle     Added FEATURE_CAMERA_VFE_ROLLOFF and FEATURE_CAMERA_VFE_WB.
05/04/05   dle     Added FEATURE_CAMERA
                   Added vfe_copy_sensor_config(), vfe_copy_aec_awb_config(),
                   and vfe_copy_af_config().
03/22/05   sai     Added VFE_YCbCr_4_2_2_linepacked and VFE_YCbCr_4_2_0_linepacked input formats for offline snapshot.
03/14/05   wyh     New VFE, output2 can have LinePack output.
11/30/04   mvd     Changes for new VFE/Camcorder image 0x1008.  Adds error
                   msgs and splits stat reporting.
10/04/04   mvd     Renamed to "vfe.h"
---------------------- OLD History of vfe_if.h ------------------------------
09/16/04   sch     Fixed oder of enumerations in VFE_RawOutputType.
08/07/04   sch     Modifications to support CMD_END_OF_FRAME_ACK1,
                   CMD_END_OF_FRAME_ACK2
07/07/04   drl     Added application argument to VFE_Initialize to support
                   Camcorder.
06/24/04   mvd     Added new type "VFE_Coefficient_3Bit_Q_FactorType" to
                   correct bug in 3x3 luma structs
03/23/04   drl     Creation

===========================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include <comdef.h>
#ifdef FEATURE_CAMERA
#include "camif.h"
#include "camqdsp.h"
#include "camsensor.h"

/*============================================================================
                        INTERNAL FEATURES
============================================================================*/

/*============================================================================
                        MACROS
============================================================================*/

/*============================================================================
                        CONSTANTS
============================================================================*/
#define VFE_MAX_BUFFER_FRAGMENTS     4
#define VFE_MIN_NUM_OF_FRAMES        3
#define VFE_MAX_BUFFER_NUMBER        (VFE_MIN_NUM_OF_FRAMES * \
                                      VFE_MAX_BUFFER_FRAGMENTS)
#define VFE_MAX_NUM_OF_SNAPSHOTS     16

#if defined QDSP_MODULE_VFE05_DEFINED || defined QDSP_MODULE_VFE25_DEFINED
#define CAMERA_VFE_SLOPE_SHIFT_FACTOR 1
#endif
/*============================================================================
                        ABSTRACT DATA TYPES
============================================================================*/
typedef unsigned int VFE_PixelSizeType;

typedef enum VFE_YCbCr_InputCositingType
{
  VFE_CHROMA_NOT_COSITED,
  VFE_CHROMA_COSITED
} VFE_YCbCr_InputCositingType;

typedef enum VFE_EnableType
{
  VFE_Disable,
  VFE_Enable
} VFE_EnableType;

typedef enum VFE_Out1ImageFormatType
{
  VFE_O1YCbCr420_LinePack = 0
} VFE_Out1ImageFormatType;

typedef enum VFE_Out2ImageFormatType
{
  VFE_O2YCbCr420_MBPack = 0,
  VFE_O2YCbCr420_LinePack   = 1
} VFE_Out2ImageFormatType;


typedef enum VFE_YCbCrImageFormatType
{

  VFE_YCbCrH2V2 = 0,
  VFE_YCbCrH2V1,
 #ifdef QDSP_MODULE_VFE05_DEFINED 
  VFE_YCbCrH2V2_YUV
 #else 
  VFE_YCbCrH1V1
 #endif
 
} VFE_YCbCrImageFormatType;


typedef enum VFE_CC_ModeType
{
  VFE_CC_Mode_Snapshot =0,
  VFE_CC_Mode_Preview
}VFE_CC_ModeType;


typedef enum VFE_YCbCr_SnapshotCositingType
{
  MPEG4_H264,
  JPEG
} VFE_YCbCr_SnapshotCositingType;

typedef enum VFE_YCbCr_OutputCositingType
{
  VFE_MPEG4_H264_COSITING,
  VFE_JPEG_COSITING
} VFE_YCbCr_OutputCositingType;

typedef enum VFE_AbfForceOnType
{
  VFE_ABF_FORCE_ON_NON_BAD_PIXELS,
  VFE_ABF_FORCE_ON_ALL_PIXELS,
  VFE_NumberOf_AbfForceOnType   /* Used for count purposes only */
} VFE_AbfForceOnType;

typedef enum VFE_AsfSharpModeType
{
  VFE_ASF_SHARP_MODE_NONE,
  VFE_ASF_SHARP_MODE_SINGLE_FILTER,
  VFE_ASF_SHARP_MODE_DUAL_FILTER,
  VFE_ASF_SHARP_MODE_SMART_FILTER,
  VFE_NumberOf_AsfSharpModeType  /* Used for count purposes only */
} VFE_AsfSharpModeType;

#ifndef FEATURE_CAMERA_YCBCR_ONLY 
typedef enum VFE_ConfigOutputType
{
  VFE_PREVIEW,
  VFE_SNAPSHOT,
  VFE_NumberOf_ConfigOutputType  /* Used for count purposes only */
} VFE_ConfigOutputType;
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

typedef unsigned int VFE_int_1_4096_Type;    /* Integer range 1 .. 4096 */

typedef unsigned int VFE_int_1_256_Type;     /* Integer range 1 .. 256 */

typedef unsigned int VFE_int_1_30_Type;      /* Integer range 1 .. 30 */

typedef unsigned int VFE_int_1_16_Type;      /* Integer range 1 .. 16 */

typedef unsigned int VFE_int_1_4_Type;       /* Integer range 1 .. 4 */

#ifndef FEATURE_CAMERA_AEC_DURING_MULTISHOT
typedef unsigned int VFE_int_0_15_Type;      /* Integer range 0 .. 15 */
#else
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

typedef unsigned int VFE_int_0_3_Type;      /* Integer range 0 .. 3 */

typedef uint32 *VFE_AddressType;

typedef uint32 VFE_SkipBitMaskType;   /* Range 0 .. 2^30 - 1 (in other words: 30 bits) */

typedef unsigned int VFE_ThresholdType;      /* Range 0 .. 63 */

typedef enum VFE_Coefficient_2Bit_Q_FactorType
{
  VFE_SIGNED_Q7,
  VFE_SIGNED_Q8,
  VFE_SIGNED_Q9,
  VFE_SIGNED_Q10
} VFE_Coefficient_2Bit_Q_FactorType;

typedef enum VFE_Coefficient_3Bit_Q_FactorType
{
  VFE_SIGNED_3BIT_Q0,
  VFE_SIGNED_3BIT_Q1,
  VFE_SIGNED_3BIT_Q2,
  VFE_SIGNED_3BIT_Q3,
  VFE_SIGNED_3BIT_Q4,
  VFE_SIGNED_3BIT_Q5,
  VFE_SIGNED_3BIT_Q6,
  VFE_SIGNED_3BIT_Q7
} VFE_Coefficient_3Bit_Q_FactorType;

/* Used in rawOutputOption */
#define VFE_8_BIT_DIRECT  0
#define VFE_10_BIT_DIRECT 1

typedef enum VFE_NumberOfRegionsType
{
  VFE_8x8,
  VFE_4x4
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
  ,VFE_16x16
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT*/
} VFE_NumberOfRegionsType;
#ifndef FEATURE_CAMERA_VFE_16X16_SUPPORT
#define VFE_16x16 VFE_8x8
#endif /* nFEATURE_CAMERA_VFE_16X16_SUPPORT*/

typedef enum VFE_PixelDataWidthType
{
  VFE_8Bit,
  VFE_10Bit
} VFE_PixelDataWidthType;

#ifdef FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK
/* camclk_po -> camera sensor -> camif_pclk ->
 * -> MDDI client clock regime
 *
 * 18 :17 MDDI_CLIENT_VFE_CLK_SRC_SEL
 * 00 : hclk
 * 01 : camif_pclk
 * 10 : camclk_po
 * 11 : undefined_encoding
 */
typedef enum
{
  VFE_HCLK = 0,
  VFE_CAMIF_PCLK = 1,
  VFE_CAMCLK_PO = 2,
  VFE_UNDEF_ENCODING = 3,
  VFE_INVALID_CLK
} vfe_clk_type;
#endif /* FEATURE_CAMERA_VFE_CONF_INTERNAL_CLK */

typedef __packed struct VFE_InputConfigurationType
{
  VFE_YCbCr_InputCositingType YCbCrCositing  :  1;
  CAMIF_InputFormatType       format         :  4;
  unsigned int                reserved       :  3;
  VFE_PixelSizeType           height         : 12;
  VFE_PixelSizeType           width          : 12;
} VFE_InputConfigurationType;

typedef __packed struct VFE_OfflineInputConfigurationType
{
#ifdef QDSP_MODULE_VFE05_DEFINED
  unsigned int                reserved       :  1;
#else
  VFE_YCbCr_InputCositingType YCbCrCositing  :  1;
#endif
  CAMIF_InputFormatType       format         :  4;
  VFE_PixelDataWidthType      dataWidth      :  3;
  VFE_PixelSizeType           height         : 12;
  VFE_PixelSizeType           width          : 12;
} VFE_OfflineInputConfigurationType;

typedef __packed struct VFE_InputSnapshotConfigurationType
{
#ifdef QDSP_MODULE_VFE05_DEFINED
  unsigned int                reserved                  :  1;
#else
  VFE_YCbCr_InputCositingType YCbCrCositing             :  1;
#endif
  unsigned int                format                    :  3;
  VFE_int_0_15_Type           numContinuousShotsMinus1  :  4;
  VFE_PixelSizeType           height                    : 12;
  VFE_PixelSizeType           width                     : 12;
} VFE_InputSnapshotConfigurationType;

typedef __packed struct VFE_OutputConfigurationType
{
  VFE_YCbCr_OutputCositingType YCbCrCositing  :  1;
  VFE_Out1ImageFormatType      format         :  3;
  VFE_PixelSizeType            height         : 12;
  VFE_PixelSizeType            width          : 12;
  unsigned int                 reserved       :  4;
} VFE_OutputConfigurationType;

typedef __packed struct VFE_YCbCrOutputConfigurationType
{
  VFE_YCbCr_SnapshotCositingType cositing     :  1;
  VFE_YCbCrImageFormatType       imageFormat  :  3;
  VFE_PixelSizeType              height       : 12;
  VFE_PixelSizeType              width        : 12;
  unsigned int                   reserved     :  4;
} VFE_YCbCrOutputConfigurationType;

typedef __packed struct VFE_VideoOutput2ConfigurationType
{
  unsigned int        reserved         :  1;
  VFE_Out2ImageFormatType
                      format           :  3;
  VFE_PixelSizeType   height           : 12;
  VFE_PixelSizeType   width            : 12;
  VFE_EnableType      startBackend     :  1;
  unsigned int        reserved1        :  2;
  VFE_EnableType      output2Enable    :  1;
} VFE_VideoOutput2ConfigurationType;

typedef __packed struct VFE_SnapshotOutput2ConfigurationType
{
  VFE_PixelSizeType height          : 12;
  unsigned int      reserved1       :  4;
  VFE_PixelSizeType width           : 12;
  VFE_int_0_3_Type  fragmentsMinus1 :  2;
#ifdef QDSP_MODULE_VFE05_DEFINED
  VFE_YCbCrImageFormatType  imageFormat  :  2;
#else
  unsigned int      reserved2       :  2;
#endif
} VFE_SnapshotOutput2ConfigurationType;

typedef __packed struct VFE_OfflineOutput2ConfigurationType
{
  VFE_PixelSizeType height          : 12;
  unsigned int      reserved2       :  4;
  VFE_PixelSizeType width           : 12;
#ifdef QDSP_MODULE_VFE05_DEFINED
  VFE_int_0_3_Type  fragmentsMinus1      :  2;
  VFE_YCbCrImageFormatType  imageFormat  :  2;
#else
  VFE_int_0_15_Type fragmentsMinus1 :  4;
#endif
} VFE_OfflineOutput2ConfigurationType;

typedef __packed struct VFE_AHB_ConfigurationType
{
  VFE_int_1_16_Type   numBurstForYBuffers         :  5;
  VFE_int_1_16_Type   burstLenForYBuffers         :  5;
  VFE_int_1_256_Type  burstFactor2ForYBuffers     :  9;
  VFE_int_1_4096_Type burstFactor1ForYBuffers     : 13;

  VFE_int_1_16_Type   numBurstForCbCrBuffers      :  5;
  VFE_int_1_16_Type   burstLenForCbCrBuffers      :  5;
  VFE_int_1_256_Type  burstFactor2ForCbCrBuffers  :  9;
  VFE_int_1_4096_Type burstFactor1ForCbCrBuffers  : 13;
} VFE_AHB_ConfigurationType;

typedef __packed struct VFE_VideoOutputBufferType
{
  VFE_AddressType Y_OutputTripleBuffer0;
  VFE_AddressType CbCrOutputTripleBuffer0;
  VFE_AddressType Y_OutputTripleBuffer1;
  VFE_AddressType CbCrOutputTripleBuffer1;
  VFE_AddressType Y_OutputTripleBuffer2;
  VFE_AddressType CbCrOutputTripleBuffer2;
} VFE_VideoOutputBufferType;

typedef __packed struct VFE_GammaCorrectionType
{
  unsigned int                   reserved :31;
  VFE_EnableType                 enable   : 1;
} VFE_GammaCorrectionType;

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
typedef __packed struct VFE_BlackCorrectionType
{
  VFE_PixelSizeType lastPixel    : 12;
  unsigned int      reserved2    :  4;
  VFE_PixelSizeType firstPixel   : 12;
  unsigned int      reserved     :  3;
  VFE_EnableType    enable       :  1;

  VFE_PixelSizeType lastLine     : 12;
  unsigned int      reserved3    :  4;
  VFE_PixelSizeType firstLine    : 12;
  unsigned int      shift        :  4;
} VFE_BlackCorrectionType;
#endif /* QDSP_MODULE_VFE25_DEFINED */

typedef __packed struct VFE_VideoOutputFrameSkipPatternType
{
  VFE_SkipBitMaskType skipBitPattern  : 30;
  unsigned int        reserved        :  2;

  unsigned int        bitPatternWidth :  5;
  unsigned int        reserved1       : 27;
} VFE_VideoOutputFrameSkipPatternType;

typedef __packed struct VFE_ClipType
{
  VFE_PixelSizeType lastPixel  : 12;
  unsigned int      reserved1  :  4;
  VFE_PixelSizeType firstPixel : 12;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  unsigned int      reserved   :  4;
#endif /* QDSP_MODULE_VFE25_DEFINED */

  VFE_PixelSizeType lastLine   : 12;
  unsigned int      reserved3  :  4;
  VFE_PixelSizeType firstLine  : 12;
  unsigned int      reserved2  :  4;
} VFE_ClipType;

typedef __packed struct
{
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  unsigned int cx         : 12;
  unsigned int reserved1  :  4;
  unsigned int cy         : 12;
  unsigned int reserved2  :  3;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  unsigned int enable     :  1;
} VFE_RolloffCfgType;

typedef __packed struct
{
  unsigned int Y_G_gain  : 10; /* Q7 */
  unsigned int Cb_B_gain : 10; /* Q7 */
  unsigned int Cr_R_gain : 10; /* Q7 */
  unsigned int reserved  :  2;
} VFE_WhiteBalanceType;

#ifdef CAMERA_VFE_SLOPE_SHIFT_FACTOR
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
typedef __packed struct VFE_DefectPixelCorrectionType
{
  uint32                           : 1;
  VFE_EnableType    enable         : 1;
  VFE_ThresholdType minThreshold   : 7;
  VFE_ThresholdType maxThreshold   : 7;
  uint32                           :12;
  uint32 slope_shift_factor        : 3;
  uint32                           : 1;
} VFE_DefectPixelCorrectionType;
#endif /* QDSP_MODULE_VFE25_DEFINED */
#else
typedef __packed struct VFE_DefectPixelCorrectionType
{
  VFE_ThresholdType minThreshold   : 7;
  VFE_ThresholdType maxThreshold   : 7;
  unsigned int      reserved       :17;
  VFE_EnableType    enable         : 1;
} VFE_DefectPixelCorrectionType;
#endif /* CAMERA_VFE_SLOPE_SHIFT_FACTOR */

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

typedef __packed struct VFE_ColorCorrectionType
{
  unsigned int                 Ch2_max                : 10;
  unsigned int                 Ch1_max                : 10;
  unsigned int                 Ch0_max                : 10;
  unsigned int                 reserved               :  1;
  VFE_EnableType               enable                 :  1;

  unsigned int                 Ch2_min                : 10;
  unsigned int                 Ch1_min                : 10;
  unsigned int                 Ch0_min                : 10;
  VFE_Coefficient_2Bit_Q_FactorType coefficient_Q_Factor   :  2;

  int                          A11                    : 12;
  unsigned int                 reserved2              :  4;
  int                          A12                    : 12;
  unsigned int                 reserved1              :  4;

  int                          k0                     : 11;
  unsigned int                 reserved4              :  5;
  int                          A13                    : 12;
  unsigned int                 reserved3              :  4;

  int                          A21                    : 12;
  unsigned int                 reserved6              :  4;
  int                          A22                    : 12;
  unsigned int                 reserved5              :  4;

  int                          k1                     : 11;
  unsigned int                 reserved8              :  5;
  int                          A23                    : 12;
  unsigned int                 reserved7              :  4;

  int                          A31                    : 12;
  unsigned int                 reserved10             :  4;
  int                          A32                    : 12;
  unsigned int                 reserved9              :  4;

  int                          k2                     : 11;
  unsigned int                 reserved12             :  5;
  int                          A33                    : 12;
  unsigned int                 reserved11             :  4;
} VFE_ColorCorrectionType;

typedef __packed struct VFE_ColorConversionType
{
  unsigned int                 Ch2_max                : 8;
  unsigned int                 Ch1_max                : 8;
  unsigned int                 Ch0_max                : 8;
  unsigned int                 reserved               : 7;
  VFE_EnableType               enable                 : 1;

  unsigned int                 Ch2_min                : 8;
  unsigned int                 Ch1_min                : 8;
  unsigned int                 Ch0_min                : 8;
  unsigned int                 reserved1a             : 6;
  VFE_Coefficient_2Bit_Q_FactorType coefficient_Q_Factor   : 2;

  int                          A11                    : 12;
  unsigned int                 reserved2              :  4;
  int                          A12                    : 12;
  unsigned int                 reserved1              :  4;

  int                          k0                     : 11;
  unsigned int                 reserved4              :  5;
  int                          A13                    : 12;
  unsigned int                 reserved3              :  4;

  int                          A21                    : 12;
  unsigned int                 reserved6              :  4;
  int                          A22                    : 12;
  unsigned int                 reserved5              :  4;

  int                          k1                     : 11;
  unsigned int                 reserved8              :  5;
  int                          A23                    : 12;
  unsigned int                 reserved7              :  4;

  int                          A31                    : 12;
  unsigned int                 reserved10             :  4;
  int                          A32                    : 12;
  unsigned int                 reserved9              :  4;

  int                          k2                     : 11;
  unsigned int                 reserved12             :  5;
  int                          A33                    : 12;
  unsigned int                 reserved11             :  4;
} VFE_ColorConversionType;

typedef __packed struct VFE_3x3LumaFilterType
{
  VFE_Coefficient_3Bit_Q_FactorType coefficient_Q_Factor   : 3;
  unsigned int                 reserved               :28;
  VFE_EnableType               enable                 : 1;

  signed int A11                    : 8;
  signed int A12                    : 8;
  signed int A13                    : 8;
  signed int reserved1              : 8;

  signed int A21                    : 8;
  signed int A22                    : 8;
  signed int A23                    : 8;
  signed int reserved2              : 8;

  signed int A31                    : 8;
  signed int A32                    : 8;
  signed int A33                    : 8;
  signed int reserved3              : 8;
} VFE_3x3LumaFilterType;

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
typedef __packed struct
{
  /* High Pass Filter */
  /* ASF Configuration, Part 1 */
  int32           h11    : 8;
  int32           h12    : 8;
  int32           h13    : 8;
  int32                  : 7;
  VFE_EnableType  enable : 1;

  /* ASF Configuration, Part 2 */
  int32 h21    : 8;
  int32 h22    : 8;
  int32 h23    : 8;
  int32        : 8;

  /* ASF Configuration, Part 3 */
  int32 h31    : 8;
  int32 h32    : 8;
  int32 h33    : 8;
  int32        : 8;

  /* Low Pass Filter */
  /* ASF Configuration, Part 4 */
  int32 l11    : 8;
  int32 l12    : 8;
  int32 l13    : 8;
  int32        : 8;

  /* ASF Configuration, Part 5 */
  int32 l21    : 8;
  int32 l22    : 8;
  int32 l23    : 8;
  int32        : 8;

  /* ASF Configuration, Part 6 */
  int32 l31    : 8;
  int32 l32    : 8;
  int32 l33    : 8;
  int32        : 8;

  /* ASF Configuration, Part 7 */
  uint32 hpf_q_factor  : 4;
  uint32 lpf_q_factor  : 4;
  uint32 sobel_mask    : 1;
  uint32               : 1;

  uint32 hpf_threshold : 10;
  uint32 lpf_threshold : 10;
  uint32               : 2;
} VFE_AsfCfgType;
#endif /* QDSP_MODULE_VFE25_DEFINED */

typedef __packed struct VFE_AddressBufferType
{
  VFE_AddressType Y_Address;
  VFE_AddressType CbCr_Address;
} VFE_AddressBufferType;

typedef __packed struct VFE_VideoConfigureType
{
  VFE_InputConfigurationType             inputConfiguration;
  VFE_OutputConfigurationType            output1Configuration;
  VFE_VideoOutput2ConfigurationType      output2Configuration;
  VFE_AHB_ConfigurationType              output1_AHB_Configuration;
  VFE_AHB_ConfigurationType              output2_AHB_Configuration;
  VFE_VideoOutputBufferType              output1Buffer;
  VFE_VideoOutputBufferType              output2Buffer;
  VFE_GammaCorrectionType                gammaCorrection;
  VFE_BlackCorrectionType                blackCorrection;
  VFE_VideoOutputFrameSkipPatternType    videoOutput1FrameSkip;
  VFE_VideoOutputFrameSkipPatternType    videoOutput2FrameSkip;
  VFE_ClipType                           clip;
  VFE_DefectPixelCorrectionType          defectPixelCorrection;
  VFE_ColorCorrectionType                colorCorrection;
  VFE_ColorConversionType                colorConversion;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  VFE_3x3LumaFilterType                  f3x3LumaFilter;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  CAMIF_ConfigType                       camif_config;
  CAMIF_EFS_ConfigType                   camif_efs_config;
  CAMIF_FrameConfigType                  camif_frame_config;
  CAMIF_WindowWidthConfigType            camif_window_width_config;
  CAMIF_WindowHeightConfigType           camif_window_height_config;
  CAMIF_Subsample1ConfigType             camif_subsample1_config;
  CAMIF_Subsample2ConfigType             camif_subsample2_config;
  CAMIF_EpochIntrType                    camif_epoch_intr;
  VFE_RolloffCfgType                     rolloffCfg;
  VFE_WhiteBalanceType                   whiteBalance;
  VFE_AsfCfgType                         asf_cfg;
} VFE_VideoConfigureType;

typedef __packed struct VFE_ConfigureRawType
{
  unsigned int                     rawOutputOption           :  1;
  VFE_int_0_15_Type                numContinuousShotsMinus1  :  4;
  unsigned int                     reserved1                 : 11;
  VFE_int_0_3_Type                 numOutputFragsMinus1      :  2;
  unsigned int                     reserved                  : 14;

  VFE_int_1_16_Type                numBurstForBuffers        :  5;
  VFE_int_1_16_Type                burstLenForBuffers        :  5;
  VFE_int_1_256_Type               burstFactor2ForBuffers    :  9;
  VFE_int_1_4096_Type              burstFactor1ForBuffers    : 13;

  VFE_AddressType                  outputBufferFragment [VFE_MAX_BUFFER_NUMBER];
  CAMIF_ConfigType                 camif_config;
  CAMIF_EFS_ConfigType             camif_efs_config;
  CAMIF_FrameConfigType            camif_frame_config;
  CAMIF_WindowWidthConfigType      camif_window_width_config;
  CAMIF_WindowHeightConfigType     camif_window_height_config;
  CAMIF_Subsample1ConfigType       camif_subsample1_config;
  CAMIF_Subsample2ConfigType       camif_subsample2_config;
  CAMIF_EpochIntrType              camif_epoch_intr;
} VFE_ConfigureRawType;

typedef __packed struct VFE_ConfigureSnapshotType
{
  VFE_InputSnapshotConfigurationType   inputConfiguration;
  VFE_YCbCrOutputConfigurationType     output1Configuration;
  VFE_SnapshotOutput2ConfigurationType output2Configuration;
  VFE_AHB_ConfigurationType            output1_AHB_Configuration;
  VFE_AHB_ConfigurationType            output2_AHB_Configuration;
  VFE_AddressBufferType                outputBufferAddress [VFE_MIN_NUM_OF_FRAMES];
  VFE_AddressBufferType                outputBufferFragment [VFE_MAX_BUFFER_NUMBER];
  VFE_GammaCorrectionType              gammaCorrection;
  VFE_BlackCorrectionType              blackCorrection;
  VFE_ClipType                         clip;
  VFE_DefectPixelCorrectionType        defectPixelCorrection;
  VFE_ColorCorrectionType              colorCorrection;
  VFE_ColorConversionType              colorConversion;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  VFE_3x3LumaFilterType                f3x3LumaFilter;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  CAMIF_ConfigType                     camif_config;
  CAMIF_EFS_ConfigType                 camif_efs_config;
  CAMIF_FrameConfigType                camif_frame_config;
  CAMIF_WindowWidthConfigType          camif_window_width_config;
  CAMIF_WindowHeightConfigType         camif_window_height_config;
  CAMIF_Subsample1ConfigType           camif_subsample1_config;
  CAMIF_Subsample2ConfigType           camif_subsample2_config;
  CAMIF_EpochIntrType                  camif_epoch_intr;
  VFE_RolloffCfgType                   rolloffCfg;
  VFE_WhiteBalanceType                 whiteBalance;
  VFE_AsfCfgType                       asf_cfg;
} VFE_ConfigureSnapshotType;

typedef union
{
  VFE_VideoConfigureType     *video_config;
  VFE_ConfigureRawType       *raw_config;
  VFE_ConfigureSnapshotType  *snapshot_config;
} VFE_ConfigType;

typedef __packed struct VFE_ConfigurationOfflineType
{
  VFE_OfflineInputConfigurationType    input;
  VFE_AHB_ConfigurationType            input_AHB;
  VFE_AddressBufferType                inputBufferFragment [VFE_MAX_BUFFER_NUMBER];
  VFE_YCbCrOutputConfigurationType     output1Configuration;
  VFE_OfflineOutput2ConfigurationType  output2Configuration;
  VFE_AHB_ConfigurationType            output1_AHB;
  VFE_AHB_ConfigurationType            output2_AHB;
  VFE_AddressBufferType                output1Buffer;
  VFE_AddressBufferType                output2BufferFragment [VFE_MAX_BUFFER_NUMBER];
  VFE_GammaCorrectionType              gammaCorrection;
  VFE_BlackCorrectionType              blackCorrection;
  VFE_ClipType                         clip;
  VFE_DefectPixelCorrectionType        defectPixelCorrection;
  VFE_ColorCorrectionType              colorCorrection;
  VFE_ColorConversionType              colorConversion;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  VFE_3x3LumaFilterType                f3x3LumaFilter;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  VFE_RolloffCfgType                   rolloffCfg;
  VFE_WhiteBalanceType                 whiteBalance;
  VFE_AsfCfgType                       asf_cfg;
} VFE_ConfigurationOfflineType;

typedef __packed struct VFE_UpdateType
{
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
  VFE_VideoOutputFrameSkipPatternType output1SkipPattern;
  VFE_VideoOutputFrameSkipPatternType output2SkipPattern;
  VFE_ClipType                        clip;
  VFE_DefectPixelCorrectionType       defectPixelCorrection;
  VFE_ColorCorrectionType             colorCorrection;
  VFE_ColorConversionType             colorConversion;
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
  VFE_3x3LumaFilterType               f3x3LumaFilter;
  VFE_RolloffCfgType                  rolloffCfg;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  
  VFE_WhiteBalanceType                whiteBalance;
  VFE_AsfCfgType                      asf_cfg;
} VFE_UpdateType;

typedef __packed struct VFE_EOF_Ack1Type
{
  VFE_AddressBufferType  eofAck1;

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
} VFE_EOF_Ack1Type;

typedef __packed struct VFE_EOF_Ack2Type
{
  VFE_AddressBufferType  eofAck2 [VFE_MAX_BUFFER_FRAGMENTS];
} VFE_EOF_Ack2Type;

typedef __packed struct VFE_StatisticsEnableType
{
  VFE_EnableType     blackLevel              : 1;
  VFE_EnableType     defectPixel             : 1;
  VFE_EnableType     autoFocus               : 1;
  VFE_EnableType     exposureAndColorBalance : 1;
  VFE_EnableType     histogram               : 1;
  unsigned int       reserved                : 27;
} VFE_StatisticsEnableType;


typedef __packed struct VFE_HistogramConfigurationType
{
  unsigned int   innerRegionLastPixel  : 12;
  unsigned int   reserved2             :  4;
  unsigned int   innerRegionFirstPixel : 12;
  unsigned int   reserved1             :  4;

  unsigned int   innerRegionLastLine   : 12;
  unsigned int   reserved4             :  4;
  unsigned int   innerRegionFirstLine  : 12;
  unsigned int   reserved3             :  4;

  VFE_AddressType histogramDataLocation;
} VFE_HistogramConfigurationType;

typedef __packed struct VFE_ExposureAndColorBalanceConfigurationType
{
  unsigned int              maximumYValue      :  8;
  unsigned int              reserved2          :  8;
  unsigned int              minimumYValue      :  8;
  unsigned int              reserved           :  6;
  VFE_NumberOfRegionsType   numberOfRegions    :  2;

  int                     M4                 :  8;
  int                     M3                 :  8;
  int                     M2                 :  8;
  int                     M1                 :  8;

  int                     C2                 : 12;
  unsigned int            reserved4          :  4;
  int                     C1                 : 12;
  unsigned int            reserved3          :  4;

  int                     C4                 : 12;
  unsigned int            reserved6          :  4;
  int                     C3                 : 12;
  unsigned int            reserved5          :  4;

  VFE_AddressType           destinationAddress;
} VFE_ExposureAndColorBalanceConfigurationType;

typedef __packed struct VFE_AutoFocusConfigurationType
{
  unsigned int lastPixel  : 12;
  unsigned int reserved1  :  4;
  unsigned int firstPixel : 12;
  unsigned int reserved   :  4;

  unsigned int lastLine   : 12;
  unsigned int reserved3  :  4;
  unsigned int firstLine  : 12;
  unsigned int reserved2  :  4;

  unsigned int maximumY   :  8;
  unsigned int reserved5  :  8;
  unsigned int minimumY   :  8;
  unsigned int reserved4  :  8;
} VFE_AutoFocusConfigurationType;

typedef enum
{
  COLOR_CORRECTION_NORMAL = 0,
  COLOR_CORRECTION_LOW_LIGHT,
  COLOR_CORRECTION_MAX_NUM
} VFE_ColorCorrectionMatrixType;

typedef enum
{
  COLOR_CONVERSION_DEFAULT = 0,
  COLOR_CONVERSION_TL84,
  COLOR_CONVERSION_A,
  COLOR_CONVERSION_D65,
  COLOR_CONVERSION_SUNSET,
  COLOR_CONVERSION_SKINTONE,
  COLOR_CONVERSION_LOWLIGHT,
  COLOR_CONVERSION_MAX_NUM
} VFE_ColorConversionMatrixType;

typedef enum
{
  SPECIAL_EFFECT_MONOCHROME = 0,
  SPECIAL_EFFECT_SEPIA,
  SPECIAL_EFFECT_NEGATIVE,
  SPECIAL_EFFECT_MAX_NUM
} VFE_SpecialEffectMatrixType;

typedef enum
{
  WHITE_BALANCE_TL84 = 0,
  WHITE_BALANCE_A,
  WHITE_BALANCE_D65,
  WHITE_BALANCE_STROBE_FLASH,
  WHITE_BALANCE_LED_FLASH,
  WHITE_BALANCE_SPOTLIGHT,
  WHITE_BALANCE_CLOUDY,
  WHITE_BALANCE_MAX_NUM
} VFE_WhiteBalanceMatrixType;

/*============================================================================
                        EXTERNAL VARIABLES
============================================================================*/
#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define vfe_parse_regions_per_awb_aec() 0
#define vfe_update_awb_stats_config(bounding_box) FALSE
#define vfe_config_awb_per_chromatix(camsensorParamsPtr)
#define vfe_update_with_new_gain_to_color_correction_or_wb(wb, blackOffset, sourceCC)
#define vfe_apply_new_gain_to_color_correction_or_wb_for_snapshot(wb, blackOffset, sourceCC)
#define vfe_set_color_conversion(matrix_ptr, saturation)
#define vfe_update_color_conversion_for_wb(newCConversion, aggressiveness, saturation)
#define vfe_apply_digital_gain_per_aec(packed_cc_ptr, temp_gain)
#define vfe_config_aec_per_chromatix(camsensorParamsPtr)
#define vfe_restore_config_for_preview()
#else
/* Address where raw VFE output from DSP will be written */
extern VFE_ExposureAndColorBalanceConfigurationType vfeExpWbStatDspInput;
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
extern VFE_AutoFocusConfigurationType vfeAfStatDspInput;

/* VFE variables from camera_svcs.c */
extern VFE_VideoConfigureType        vfeVideoConfig;
extern VFE_ConfigureRawType          vfeRawConfig;
extern VFE_ConfigureSnapshotType     vfeSnapshotConfig;
extern VFE_ConfigurationOfflineType  vfeOfflineConfig;
extern VFE_ConfigType         vfeSensorConfig;
extern VFE_UpdateType               vfeUpdate;

/*============================================================================
                        EXTERNAL API DECLARATION
============================================================================*/
/*===========================================================================

FUNCTION:

  VFE_HW_FEATURES_AUTO_DETECT

DESCRIPTION:
  Determines the VFE H/W features supported based on the
  VFE version. Populates a global structure with the h/w
  features supported. Applications use this global variable
  for deciding whether to use H/W vs ARM/DSP based solutions.

CALLERS:
  camera_init ()

DEPENDENCIES:

SIDE EFFECTS  :
  Populates vfe_hw_features global variable.

USAGE:
  During Camera services initialization, this routine is called
  to determine the VFE H/W features supported. Based on this information,
  camera services decides whether to use H/W features or ARM/DSP
  implemented features.

RETURN VALUE:
  None.

SCOPE:
  Global

============================================================================*/
void vfe_hw_features_auto_detect (void);

/*===========================================================================

FUNCTION:

  VFE_INITIALIZE

DESCRIPTION:
  Downloads the VFE firmware image.

CALLERS:
  camera_process_start_preview ()
  camera_process_take_picture ()
  camera_process_offline_snapshot ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Upon entry of camera service operations.

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Initialize (CAMQDSP_MsgCBType cb);

/*===========================================================================

FUNCTION:

  VFE_CONFIGUREVIDEO

DESCRIPTION:
  - Configures VFE for video mode.
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  For continuous and real-time processing of camera sensor data.

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureVideo
(
 VFE_VideoConfigureType *config
);

/*===========================================================================

FUNCTION:

  VFE_CONFIGURERAWSNAPSHOT

DESCRIPTION:
  - Configures VFE for raw snapshot mode.
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  For obtaining the sensor data as is into memory.
  These raw images are later used for image tuning.

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureRawSnapshot
(
 VFE_ConfigureRawType *config
);

/*===========================================================================

FUNCTION:
  VFE_CONFIGURESNAPSHOT

DESCRIPTION:
  - Configures VFE for Snapshot mode
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  - The data obtained form the VFE can be encoded, ex., JPEG

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureSnapshot
(
 VFE_ConfigureSnapshotType *config
);

/*===========================================================================

FUNCTION:
  VFE_CONFIGUREOFFLINE

DESCRIPTION:

CALLERS:
  camera_config_vfe ()
  camera_config_offline_snapshot ()

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Process the image from memory, ex., Bayer Vignette Correction

SCOPE:

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureOffline
(
 VFE_ConfigurationOfflineType *config
);

/*===========================================================================

FUNCTION:

  VFE_CAPTURE

DESCRIPTION:
  Causes the VFE H/W to start capturing the camera sensor data.

CALLERS:
  camera_initiate_capture ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Before starting the VFE H/W please ensure the VFE H/W is properly
  configured.

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Capture (void);

/*===========================================================================

FUNCTION:

  VFE_IDLE

DESCRIPTION:
  Stops the VFE H/W.

CALLERS:
  camera_do_stopping ()
  camera_process_stop ()
  camera_process_start_preview ()
  camera_process_release_picture ()
  camera_terminate ()
  camera_process_qdsp_output1_msg ()
  camera_process_qdsp_output2_msg ()
  camera_process_qdsp_detect_error_msg ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Turning off the camera services.
  VFE must be stopped before turning off the pclk to avoid
  system hangs.

SCOPE:
  Global.

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Idle    (void);

/*===========================================================================

FUNCTION:

  VFE_UPDATE

DESCRIPTION:
  Updates the VFE parameters. All the parameters in the update are
  double buffered in the h/w and the new parameters take effect on the
  next frame boundary.

CALLERS:
  camera_process_qdsp_output1_msg ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Update
(
 VFE_UpdateType *config,
 const camsensor_static_params_type *camsensor_params
);

/*===========================================================================

FUNCTION:

  VFE_EOF_ACK1

DESCRIPTION:

CALLERS:
  camera_process_release_frame ()
  camera_process_qdsp_output1_msg ()
  output1enc_ack1_criteria ()

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_Ack1 (VFE_EOF_Ack1Type ack);

/*===========================================================================

FUNCTION:

  VFE_EOF_ACK2

DESCRIPTION:

CALLERS:
  output2enc_ack2_criteria ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_Ack2 (VFE_EOF_Ack2Type ack);

/*===========================================================================

FUNCTION:

  VFE_EOF_SnapshotACK2

DESCRIPTION:

CALLERS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_SnapshotAck2 (VFE_EOF_Ack2Type ack);

/*===========================================================================

FUNCTION:

  VFE_AFSTATSSTART

DESCRIPTION:

CALLERS:
  camera_process_start_focus ()
  camera_process_qdsp_output1_msg ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AFStatsStart (VFE_AutoFocusConfigurationType afStats);

/*===========================================================================

FUNCTION:

  VFE_AFSTATSSTOP

DESCRIPTION:

CALLERS:
  camera_svcs_take_picture ()
  camera_process_stop ()
  camera_process_stop_preview ()
  camera_process_qdsp_af_stats_complete_msg ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AFStatsStop  (void);

/*===========================================================================

FUNCTION:

  VFE_AE_AWB_STATS

DESCRIPTION:

CALLERS:
  camera_process_qdsp_output1_msg ()
  camera_process_qdsp_ae_awb_stats_complete_msg ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AE_AWB_Stats (VFE_ExposureAndColorBalanceConfigurationType stats);

/*===========================================================================

FUNCTION:

  VFE_HISTSTATS

DESCRIPTION:

CALLERS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_HistStats    (VFE_HistogramConfigurationType stats);

/*===========================================================================

FUNCTION:

  VFE_COPY_SENSOR_CONFIG

DESCRIPTION:
  Copy the sensor config to VFE config

CALLERS:
  camera_config_vfe ()

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/
boolean vfe_copy_sensor_config
(
  camsensor_op_mode_type        mode,
  /* VFE output dimenstion along output1 path.
   * Output1 is used for preview and thumbnail
   */
  uint16                        output1_width,
  uint16                        output1_height,
  /* VFE output dimenstion along output2 path.
   * Output2 is used for main image
   */
  uint16                        output2_width,
  uint16                        output2_height,
  const camsensor_static_params_type *camsensor_params /* Static Parameters */
);


/*===========================================================================


FUNCTION:

  VFE_GET_VERSION

DESCRIPTION:
  Returns the VFE Version 
CALLERS:
  

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/

CAMQDSP_VfeType vfe_get_type(void);

/*===========================================================================

FUNCTION:

VFE_COPY_AEC_AWB_CONFIG

DESCRIPTION:
  Copy the auto exposure and auto white balance info from
  camsensor parameter table.

CALLERS:
  cam3a_config ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/
boolean vfe_copy_aec_awb_config
(
  VFE_ExposureAndColorBalanceConfigurationType *aec_config,
  const camsensor_static_params_type                 *camsensor_params /* Static Parameters */
);

/*===========================================================================

FUNCTION:

VFE_COPY_AF_CONFIG

DESCRIPTION:
  Copy the auto focus info from camsensor parameter table.

CALLERS:
  cam3a_config ()

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global
============================================================================*/
boolean vfe_copy_af_config
(
  VFE_AutoFocusConfigurationType *af_config,
  const camsensor_static_params_type   *camsensor_params /* Static Parameters */
);

/*===========================================================================

FUNCTION:

  VFE_INIT (former VFE_FEATURES_AUTO_DETECT)

DESCRIPTION:
  Determines the VFE H/W features supported based on the
  VFE version. Populates a global structure with the h/w
  and s/w features supported. Applications use this global variable
  for deciding whether to use H/W vs ARM/DSP based solutions.

SIDE EFFECTS:
  Populates vfe_features global variable.

USAGE:
  During Camera services initialization, this routine is called
  to determine the VFE H/W features supported. Based on this information,
  camera services decides whether to use H/W features or ARM/DSP
  implemented features.

RETURN VALUE:
  None.

SCOPE:
  Global

=========================================================================== */
void vfe_init (void);

/*===========================================================================

FUNCTION      VFE_PUT_GAMMA_TABLE

DESCRIPTION
              Put the correct gamma table to the DSP.

DEPENDENCIES
  Assumes that the VFE has been put into IDLE state first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void vfe_put_gamma_table(int32 special_effect, int32 contrast, camsensor_op_mode_type op_mode);

/*===========================================================================

FUNCTION      VFE_INIT_GAMMA_TABLE

DESCRIPTION
              Determine the right gamma table size, and copy or downsize
              from device gamma tables.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
void vfe_init_gamma_table
(
  const camsensor_static_params_type *camsensor_params /* Other config params */
);

/*===========================================================================

FUNCTION:

  VFE_INIT_ROLLOFF_TABLES

DESCRIPTION:
  Initialize the rolloff table for all 3 lighting conditions and for both
  full size scale and quarter size scale.

SIDE EFFECTS:
  None

RETURN VALUE:
  TRUE if successful,
  FALSE if failed.

SCOPE:
  Local

============================================================================*/
boolean vfe_init_rolloff_tables
(
 const camsensor_static_params_type *camsensor_params
);

/*===========================================================================

FUNCTION      VFE_PUT_ROLLOFF_TABLE

DESCRIPTION
              Copy radius square rolloff table to DSP.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void vfe_put_rolloff_table(void);

/*===========================================================================

FUNCTION      VFE_START

DESCRIPTION
              Get the vfe service started.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type vfe_start
(
 const camsensor_static_params_type *camsensor_params
);

/*===========================================================================

FUNCTION      VFE_GET_COLOR_CORRECTION_MATRIX_PTR

DESCRIPTION
              Provide the pointer of color correction matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_rgb_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
camera_3x3_rgb_matrix_type* vfe_get_color_correction_matrix_ptr
(
 VFE_ColorCorrectionMatrixType mId 
);
#endif /* CAMERA_USES_CHROMATIX_0x203 */

/*===========================================================================

FUNCTION      VFE_GET_COLOR_CONVERSION_MATRIX_PTR

DESCRIPTION
              Provide the pointer of color conversion matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_3x3_ycbcr_matrix_type* vfe_get_color_conversion_matrix_ptr
(
 VFE_ColorConversionMatrixType mId
);

/*===========================================================================

FUNCTION      VFE_GET_SPECIAL_EFFECT_MATRIX_PTR

DESCRIPTION
              Provide the pointer of special effect matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_3x3_ycbcr_matrix_type* vfe_get_special_effect_matrix_ptr
(
 VFE_SpecialEffectMatrixType mId
);

/*===========================================================================

FUNCTION      VFE_GET_WHITE_BALANCE_MATRIX_PTR

DESCRIPTION
              Provide the pointer of white balance matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_1x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_1x3_matrix_type* vfe_get_white_balance_matrix_ptr
(
 VFE_WhiteBalanceMatrixType mId
);

/*===========================================================================

FUNCTION      vfe_convert_float_to_Qfactor

DESCRIPTION
              Converts a floating point number into an integer number
              with a particular Q factor desired

DEPENDENCIES
  None

INPUT
  Float input
  Qfactor of desired output (i.e. number of places to give fractional
    portion)

RETURN VALUE
  int32 converted number

SIDE EFFECTS
  None

===========================================================================*/
int32 vfe_convert_float_to_Qfactor(float dfloat, int32 qfactor);

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_PARSE_REGIONS_PER_AWB_AEC

DESCRIPTION
   Do inital parse of AEC AWB stats comming back from VFE

CALLERS:
  camera_3a_parse_and_implement_aec_awb ()

===========================================================================*/
int32 vfe_parse_regions_per_awb_aec(void);

/*===========================================================================

FUNCTION      VFE_UPDATE_AWB_STATS_CONFIG

DESCRIPTION
  set updated awb stats config in vfeExpWbStatDspInput

CALLERS:
  camera_awb_do_awb_advanced_grey_world_algo()

===========================================================================*/
boolean vfe_update_awb_stats_config(chromatix_wb_exp_stats_type bounding_box);

/*===========================================================================

FUNCTION      VFE_CONFIG_AWB_PER_CHROMATIX

DESCRIPTION
  set chromatix info

CALLERS:
  camera_awb_config()

===========================================================================*/
void vfe_config_awb_per_chromatix(camsensor_static_params_type * camsensorParamsPtr);

/*===========================================================================

FUNCTION      VFE_UPDATE_WITH_NEW_GAIN_TO_COLOR_CORRECTION_OR_WB

DESCRIPTION:
  Apply wb to either color correction matrix or white balance config.

CALLERS:
  camera_awb_apply_new_wb_gains ()
  camera_awb_update_color_correction_or_wb ()

============================================================================*/
void vfe_update_with_new_gain_to_color_correction_or_wb (
  camera_1x3_matrix_type wb,
  int16 blackOffset,
  camera_3x3_rgb_matrix_type *sourceCC
  );

/*===========================================================================

FUNCTION:  VFE_APPLY_NEW_GAIN_TO_COLOR_CORRECTION_OR_WB_FOR_SNAPSHOT

DESCRIPTION:
  applies the digital gain on the color correction matrix.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  NONE
============================================================================*/
void vfe_apply_new_gain_to_color_correction_or_wb_for_snapshot (
  camera_1x3_matrix_type wb,
  int16 blackOffset,
  camera_3x3_rgb_matrix_type *sourceCC
  );

/*===========================================================================

FUNCTION      VFE_SET_COLOR_CONVERSION

DESCRIPTION:
  Apply color conversion matrix for BSM settings.

CALLERS:
  camera_awb_set_wb_per_bsm ()

============================================================================*/
void vfe_set_color_conversion(
  camera_3x3_ycbcr_matrix_type * matrix_ptr,
  float saturation
  );

/*===========================================================================

FUNCTION      VFE_UPDATE_COLOR_CONVERSION_FOR_WB

DESCRIPTION:
  Update color converstion matrix for wb.

CALLERS:
  camera_awb_update_color_conversion_for_wb ()

============================================================================*/
void vfe_update_color_conversion_for_wb(
  camera_3x3_ycbcr_matrix_type * newCConversion,
  float aggressiveness,
  float saturation
  );

/*===========================================================================

FUNCTION      VFE_APPLY_DIGITAL_GAIN_PER_AEC

DESCRIPTION:
  Update color converstion matrix for wb.

CALLERS:
  camera_aec_apply_digital_gain_for_preview ()

============================================================================*/
void vfe_apply_digital_gain_per_aec(
  VFE_ColorCorrectionType *packed_cc_ptr,
  float temp_gain
  );

/*===========================================================================

FUNCTION      VFE_CONFIG_AEC_PER_CHROMATIX

DESCRIPTION
  set chromatix info

CALLERS:
  camera_aec_config()

===========================================================================*/
void vfe_config_aec_per_chromatix(camsensor_static_params_type * camsensorParamsPtr);

/*===========================================================================

FUNCTION      VFE_RESTORE_CONFIG_FOR_PREVIEW

DESCRIPTION
  restore wb gains and color correction matrix for preview restart

CALLERS:
  camera_restore_config_for_preview()

===========================================================================*/
void vfe_restore_config_for_preview(void);
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION    VFE_AF_CALCULATE_AF_WINDOW

DESCRIPTION
  Calculate AF window size based on VFE clipping window size.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vfe_calculate_af_window
(
  uint32 vfe_clip_width,
  uint32 vfe_clip_height
);

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  vfe_update_5x5_asf

DESCRIPTION:
  Set up 5x5 Adaptive Spatial Filter for VFE update.

CALLERS:
  camera_parms_write

RETURN VALUE:
  None

SCOPE:
  Global

============================================================================*/
void vfe_update_5x5_asf
(
  const camsensor_static_params_type *camsensor_params,
  VFE_ConfigOutputType vfe_cfg_op
);

/*===========================================================================

FUNCTION:
  vfe_update_3x3_asf

DESCRIPTION:
  Set up 5x5 Adaptive Spatial Filter for VFE update.

CALLERS:
  camera_parms_write

RETURN VALUE:
  None

SCOPE:
  Global

============================================================================*/
#ifndef QDSP_MODULE_VFE25_DEFINED

void vfe_update_3x3_asf
(
  const camsensor_static_params_type *camsensor_params
  
);
#endif /* QDSP_MODULE_VFE25_DEFINED */


#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
/*===========================================================================

FUNCTION:

  VFE_INIT_MESH_ROLLOFF_TABLES

DESCRIPTION:
  Initialize the rolloff table for all 3 lighting conditions and for both
  full size scale and quarter size scale.

SIDE EFFECTS:
  None

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
boolean vfe_init_mesh_rolloff_tables
(
 const camsensor_static_params_type *camsensor_params
);

/*===========================================================================

FUNCTION:
  VFE_CALCULATE_SHARPNESS_FACTOR

DESCRIPTION:
  Calculate the sharpness factor based on downscaling factor

CALLERS:
  camera_parms_write

RETURN VALUE:
  None

SCOPE:
  Global

============================================================================*/
float vfe_calculate_sharpness_factor(boolean is_preview,camsensor_static_params_type * camsensor_params);


#endif /* QDSP_MODULE_VFE25_DEFINED */
/*===========================================================================

FUNCTION:

  VFE_SELECT_ROLLOFF_TABLES

DESCRIPTION:
  Select Roll off tables as per illuminant

SIDE EFFECTS:
  None

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/

void vfe_select_rolloff
(
  camsensor_sensor_resolution_type resolution,
  boolean                          preview
);

#endif /* FEATURE_CAMERA */
#endif /* VFE_H */

