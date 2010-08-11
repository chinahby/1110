#ifndef CAMERA_HJR_H
#define CAMERA_HJR_H
#ifdef FEATURE_CAMERA_HJR

/*=============================================================================
FILE:       camera_hjr.h

SERVICES:   Hand Jitter Reduction Control & Bayer Function Interface.

DESCRIPTION:
   This file provides necessary interfaces for Hand Jitter controller.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 - 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/hjr/main/latest/inc/camera_hjr.h#1 $ $DateTime: 2008/05/09 16:40:03 $ $Author: karenp $


ABSTRACT:
   
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/08   hap     Bypass SW ABF for LCU
07/25/07   smn     Added defines needed for 2-3 three frame smart registration
02/26/07   ven     Removed support for 10 bit bayer noise reduction by wrapping it
                   under CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED
10/16/06   ven     Featurized HJR
10/05/06   ven     Added support for using chromatix variables in HJR.
08/31/06   ven     removed the old bayer filter tuning parameters from 
                   function signature
07/26/06   ven     Changed the bayer filter name and added comments 
                   to the bayer filter function
07/15/06   ven     Cleaned Lint errors related to HJR
07/13/06   ven     Moved hjr setup function to camera_aec.c
06/26/06   ven     Added support for GRGB and GBGR formats in bayer filter.
05/01/06   ven     Initial release
===========================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "camif.h"
#include "camsensor.h"

/*============================================================================
                        INTERNAL FEATURES
============================================================================*/

/*============================================================================
                        MACROS
============================================================================*/
#define IS_UINT7(a) ((a)<=127)

#define HJR_ABS(x) (x>0 ? x : -x)
#define HJR_MIN(x,y) (x<y ? x : y)
#define HJR_MAX(x,y) (x>y ? x : y)


/*============================================================================
                        CONSTANTS
============================================================================*/
#define HJR_MAX_FRAMES_SUPPORTED 3
#define HJR_MULTIFRAME_SEARCH_X_DIRECTION 50
#define HJR_MULTIFRAME_SEARCH_Y_DIRECTION 50
#define HJR_DEFAULT_GAIN_FACTOR 4
#define HJR_MULTI_FRAME_SEARCH_FACTOR 0.025

/*============================================================================
                        ABSTRACT DATA TYPES
============================================================================*/
typedef struct 
{
    /* Input values */
    uint8 current_luma;
    uint32 current_line_count;
    float  current_sensor_gain;
    uint16 luma_target;
    uint32 max_line_count;
    float  max_sensor_gain;
    uint16 maxNumFrames;
    int32  current_frame_rate; /* Q8 Number */
    int32  max_frame_rate; /* Q8 Number */
   

    /* Output Values */
    uint32 new_line_count;
    float  new_sensor_gain;
    int32  new_dig_gain; /* Q7 number */
    uint16 new_numberFrames;
    int32  new_frame_rate; /* Q8 Number */
    
    uint16 meetTargetQ10;
    uint16 digGainQ10;
    uint16 gain_factor;
} HJR_struct;

/*============================================================================
                        EXTERNAL API DECLARATION
============================================================================*/
#ifndef QDSP_MODULE_VFE25_DEFINED
#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */
void ipl_unpack_10_bit_bayer_data
(
  uint8 *input_data_ptr,
  uint32 input_data_size
);

void ipl_pack_10_bit_bayer_data
(
  uint8 *input_data_ptr,
  uint32 input_data_size
);

#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */

void hjr_bayer_noise_reduction(void *dataIn,void *dataOut, uint16 width, 
                  uint16 height, camsensor_raw_output_type bayer_pixel_size,
                  CAMIF_InputFormatType bayer_format, 
                  camsensor_static_params_type* camsensor_static_params_ptr );
#endif /* ifndef QDSP_MODULE_VFE25_DEFINED */

ipl_status_type camera_hjr_smart_register_frames(ipl_image_type **in_image, uint16 max_corr_Xlag, uint16 max_corr_Ylag);

#endif /* FEATURE_CAMERA_HJR */
#endif /* CAMERA_HJR_H */

