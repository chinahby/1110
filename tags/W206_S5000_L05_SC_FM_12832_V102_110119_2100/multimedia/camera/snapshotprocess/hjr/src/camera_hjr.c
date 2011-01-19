
/*=============================================================================
FILE:       camera_hjr.c

SERVICES:   Hand Jitter Reduction Controller Interface

DESCRIPTION:
   This file provides necessary interfaces for the Hand Jitter Reduction Controller.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 - 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/hjr/main/latest/src/camera_hjr.c#1 $ $DateTime: 2008/05/09 16:40:03 $ $Author: karenp $


ABSTRACT:
   
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/08    sm     Fixed Klocwork errors
03/12/08   chai    included stdlib file to fix cmpiler warning
01/25/08   hap     Bypass SW ABF for LCU
11/08/07   hap     Fixing RVCT2.2 compiler warnings
           smn     Added the smart registration for 2 and 3 frame HJR
02/23/07   ven     Optimized HJR by using 5x5 matrix instead of 9x9 matrix
                   Removed support for 10 bit bayer noise reduction by
                   wrapping it under CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED
10/16/06   ven     Featurized HJR
10/05/06   ven     Added support for using chromatix variables in HJR.
08/28/06   ven     Tuned the optimized bayer filter for micron sensor for LG
07/31/06   ven     Linted the optimized bayer noise reduction module
07/26/06   ven     Changed the bayer filter name and added comments 
                   to the bayer filter function
07/15/06   ven     Cleaned Lint errors related to HJR
07/13/06   ven     Moved hjr setup function to camera_aec.c
06/26/06   ven     Added support for GRGB and GBGR formats in bayer filter.
05/01/06   ven     Initial Release
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/

#include "assert.h"
#include "camera.h"
#include "camerai.h"
#include "camera_hjr.h"
#include "camera_aec.h"
#include "stdlib.h"

#ifdef FEATURE_CAMERA_HJR

/*============================================================================
                        CONSTANTS
============================================================================*/
#define TWO_BIT_MASK  0x3
#define FLAT_GAIN1 (camsensor_static_params_ptr->chromatix_parms->flat_area_noise_reduction_level)
#define FLAT_GAIN2 (16-FLAT_GAIN1)
#define TEXTURE_GAIN1 (camsensor_static_params_ptr->chromatix_parms->texture_noise_reduction_level)
#define TEXTURE_GAIN2 (16-TEXTURE_GAIN1)
#define SHIFT19 (1<<19)
#define SHIFT7  (1<<7)
#define SHIFT20 (1<<20)

/*============================================================================
                     LOCAL DATA DEFINITIONS 
============================================================================*/
typedef struct proj_t 
{
  uint32 *h1;
  uint32 *h2;
  uint32 *h3;
  uint32 *h4;
  uint32 *v1;
  uint32 *v2;
  uint32 *v3;
  uint32 *v4;
} proj_t;

typedef struct m_vector_t
{
  int16 vert[9];
  int16 hor[9];
} m_vector_t;


/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
static ipl_status_type camera_hjr_get_projections(uint8 *fr, uint32 width, uint32 height, proj_t *proj);
static ipl_status_type camera_hjr_smart_combine_frames(ipl_image_type *fr1, ipl_image_type *fr2, m_vector_t *vec, 
                                                       int32 width, int32 height, uint8 scaleQ8, ipl_col_for_type cs) ;
static int16 camera_hjr_corr1D(uint32 *sum1,uint32 *sum2, uint32 len, uint32 max_lag, uint32 *work_buffer);
static void camera_hjr_interpolate_vectorLine(int32 y0, int32 y1, int32 y2, uint32 size, int32 *InitalOffset, int32* Slope );
static void camera_hjr_interpolate_vector(int32 motion1, int32 motion2, uint32 size, int32 *vector, uint8 inv);

#ifndef QDSP_MODULE_VFE25_DEFINED
static void hjr_8_bit_bayer_noise_reduction(const uint8 *dataIn,uint8 *dataOut, uint16 width,
    uint16 height, camsensor_static_params_type* camsensor_static_params_ptr);

#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED 

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */

static void hjr_10_bit_bayerfiltering(uint16 *dataIn,uint16 *dataOut, int16 width,
              int16 height, uint16 threshold, uint16 scaleQ12,
              CAMIF_InputFormatType bayer_format);

#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */

static uint8 flat_gain1 = 0;
static uint8 flat_gain2 = 0;
static uint8 texture_gain1 = 0;
static uint8 texture_gain2 = 0;

static uint32 pix_sqr[256] = 
                      { 0x0000, 0x0001, 0x0004, 0x0009, 0x0010, 0x0019, 0x0024, 0x0031,
                        0x0040, 0x0051, 0x0064, 0x0079, 0x0090, 0x00A9, 0x00C4, 0x00E1,
                        0x0100, 0x0121, 0x0144, 0x0169, 0x0190, 0x01B9, 0x01E4, 0x0211,
                        0x0240, 0x0271, 0x02A4, 0x02D9, 0x0310, 0x0349, 0x0384, 0x03C1,
                        0x0400, 0x0441, 0x0484, 0x04C9, 0x0510, 0x0559, 0x05A4, 0x05F1,
                        0x0640, 0x0691, 0x06E4, 0x0739, 0x0790, 0x07E9, 0x0844, 0x08A1,
                        0x0900, 0x0961, 0x09C4, 0x0A29, 0x0A90, 0x0AF9, 0x0B64, 0x0BD1,
                        0x0C40, 0x0CB1, 0x0D24, 0x0D99, 0x0E10, 0x0E89, 0x0F04, 0x0F81,
                        0x1000, 0x1081, 0x1104, 0x1189, 0x1210, 0x1299, 0x1324, 0x13B1,
                        0x1440, 0x14D1, 0x1564, 0x15F9, 0x1690, 0x1729, 0x17C4, 0x1861,
                        0x1900, 0x19A1, 0x1A44, 0x1AE9, 0x1B90, 0x1C39, 0x1CE4, 0x1D91,
                        0x1E40, 0x1EF1, 0x1FA4, 0x2059, 0x2110, 0x21C9, 0x2284, 0x2341,
                        0x2400, 0x24C1, 0x2584, 0x2649, 0x2710, 0x27D9, 0x28A4, 0x2971,
                        0x2A40, 0x2B11, 0x2BE4, 0x2CB9, 0x2D90, 0x2E69, 0x2F44, 0x3021,
                        0x3100, 0x31E1, 0x32C4, 0x33A9, 0x3490, 0x3579, 0x3664, 0x3751,
                        0x3840, 0x3931, 0x3A24, 0x3B19, 0x3C10, 0x3D09, 0x3E04, 0x3F01,
                        0x4000, 0x4101, 0x4204, 0x4309, 0x4410, 0x4519, 0x4624, 0x4731,
                        0x4840, 0x4951, 0x4A64, 0x4B79, 0x4C90, 0x4DA9, 0x4EC4, 0x4FE1,
                        0x5100, 0x5221, 0x5344, 0x5469, 0x5590, 0x56B9, 0x57E4, 0x5911,
                        0x5A40, 0x5B71, 0x5CA4, 0x5DD9, 0x5F10, 0x6049, 0x6184, 0x62C1,
                        0x6400, 0x6541, 0x6684, 0x67C9, 0x6910, 0x6A59, 0x6BA4, 0x6CF1,
                        0x6E40, 0x6F91, 0x70E4, 0x7239, 0x7390, 0x74E9, 0x7644, 0x77A1,
                        0x7900, 0x7A61, 0x7BC4, 0x7D29, 0x7E90, 0x7FF9, 0x8164, 0x82D1,
                        0x8440, 0x85B1, 0x8724, 0x8899, 0x8A10, 0x8B89, 0x8D04, 0x8E81,
                        0x9000, 0x9181, 0x9304, 0x9489, 0x9610, 0x9799, 0x9924, 0x9AB1,
                        0x9C40, 0x9DD1, 0x9F64, 0xA0F9, 0xA290, 0xA429, 0xA5C4, 0xA761,
                        0xA900, 0xAAA1, 0xAC44, 0xADE9, 0xAF90, 0xB139, 0xB2E4, 0xB491,
                        0xB640, 0xB7F1, 0xB9A4, 0xBB59, 0xBD10, 0xBEC9, 0xC084, 0xC241,
                        0xC400, 0xC5C1, 0xC784, 0xC949, 0xCB10, 0xCCD9, 0xCEA4, 0xD071,
                        0xD240, 0xD411, 0xD5E4, 0xD7B9, 0xD990, 0xDB69, 0xDD44, 0xDF21,
                        0xE100, 0xE2E1, 0xE4C4, 0xE6A9, 0xE890, 0xEA79, 0xEC64, 0xEE51,
                        0xF040, 0xF231, 0xF424, 0xF619, 0xF810, 0xFA09, 0xFC04, 0xFE01};

/*============================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/
#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */

/*===========================================================================

FUNCTION      IPL_UNPACK_10_BIT_BAYER_DATA

DESCRIPTION
              UNPACKS THE 10 BIT PACKED BAYER DATA.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ipl_unpack_10_bit_bayer_data
(
  uint8 *input_data_ptr,
  uint32 input_data_size
)
{
  int32 i,j;
  uint8* bottom_input_data_ptr, *bottom_output_data_ptr;
  uint8  partial_10_bit_byte;
  uint8  two_bit_data;
  uint16 ten_bit_data;
  uint16* sixteen_bit_output_data_ptr;
  uint32 bottom_output_index;

  
  if (input_data_ptr == NULL)
  {
    return;
  }

  bottom_input_data_ptr = input_data_ptr + input_data_size - 1;
  /* Find out the number of bytes required to convert packed 10
   * bit values to unpacked 10 bit values of 2 bytes each */
  bottom_output_index = (input_data_size*6)/4 - 1;
  bottom_output_data_ptr = input_data_ptr + bottom_output_index;
  bottom_output_data_ptr--;
  /*lint -save */
  /*lint -e826 Suspicious pointer-to-pointer conversion (area too small) */
  sixteen_bit_output_data_ptr = (uint16*) bottom_output_data_ptr;
  /*lint -restore */


  for(i=(int32)input_data_size; i>0; i-=4)
  {
    partial_10_bit_byte = (uint8) (* bottom_input_data_ptr);
    bottom_input_data_ptr--;
    for (j=2; j>=0; j--)
    {
      two_bit_data = (partial_10_bit_byte & (TWO_BIT_MASK << (j*2)) ) >> (j*2) ;
      ten_bit_data = ( (uint8) (* bottom_input_data_ptr) | (two_bit_data << 8 ) );
      *sixteen_bit_output_data_ptr = ten_bit_data;
      bottom_input_data_ptr--;
      sixteen_bit_output_data_ptr--;
    }

  }

}

/*===========================================================================

FUNCTION      IPL_PACK_10_BIT_BAYER_DATA

DESCRIPTION
              PACKS THE 10 BIT DATA TO PACKED 10-BIT BAYER FORMAT.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ipl_pack_10_bit_bayer_data
(
  uint8 *input_data_ptr,
  uint32 input_data_size
)
{
  int32 i,j;
  uint8  partial_10_bit_byte;
  uint8  two_bit_data;
  uint8* eight_bit_output_data_ptr;

  if (input_data_ptr == NULL)
  {
    return;
  }

  eight_bit_output_data_ptr = input_data_ptr;

  for(i=0; i<(int32)input_data_size; i+=4)
  {
    partial_10_bit_byte = 0;
    for (j=0; j<3; j++)
    {
      *eight_bit_output_data_ptr = *input_data_ptr;
      eight_bit_output_data_ptr++;
      input_data_ptr++;
      two_bit_data = *input_data_ptr;
      /* Check for overflow of 10 bit data. If more, round it to 
       * highest 10 bit value */
      if ( two_bit_data > 3 )
      {
        two_bit_data = 3;
        *(eight_bit_output_data_ptr - 1 ) = 0xFF;
      }
      input_data_ptr++;
      two_bit_data &= TWO_BIT_MASK;
      partial_10_bit_byte = (uint8)(partial_10_bit_byte | (two_bit_data << (j*2)));
    }

    *eight_bit_output_data_ptr = partial_10_bit_byte;
    eight_bit_output_data_ptr++;
  }

}


#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */

/*===========================================================================

FUNCTION      HJR_BAYER_NOISE_REDUCTION

DESCRIPTION
              PERFORMS BAYER FILTERING ON THE UNPACKED DATA.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void hjr_bayer_noise_reduction(void *dataIn,void *dataOut, uint16 width, 
                  uint16 height, camsensor_raw_output_type bayer_pixel_size,
                  CAMIF_InputFormatType bayer_format,
                  camsensor_static_params_type* camsensor_static_params_ptr)
{
  if ( (dataIn == NULL) || (dataOut == NULL) ||
       ( (bayer_format != CAMIF_BAYER_G_B)
         && (bayer_format != CAMIF_BAYER_B_G)
         && (bayer_format != CAMIF_BAYER_G_R)
         && (bayer_format != CAMIF_BAYER_R_G)
       )
     )
  {
    return;
  }
#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */

  if ( bayer_pixel_size == CAMSENSOR_8_BIT_DIRECT )
  {
#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */
    hjr_8_bit_bayer_noise_reduction((const uint8*)dataIn, (uint8*) dataOut, (uint16)width,
                (uint16)height, camsensor_static_params_ptr);
#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */
  }
  else 
  {

    hjr_10_bit_bayerfiltering((uint16*)dataIn, (uint16*) dataOut, (int16)width,
              (int16)height, 8, 455,bayer_format);
  }  
#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */
}



static void hjr_8_bit_bayer_noise_reduction(const uint8 *dataIn,uint8 *dataOut,
    uint16 width, uint16 height, camsensor_static_params_type* camsensor_static_params_ptr)
{
  uint32 i,j; 
  uint8  tmpB;
  int16  a1,a5;   
  uint32 sum,mean;
  uint32 tmp2,tmp4;
  uint16 tr1;
  uint16 tr2;
  uint32 index;
  uint8 conv_table[17]={0,0,0,0,1,1,3,5,6,8,9,10,11,12,14,15,16};


  flat_gain1 = conv_table[(camsensor_static_params_ptr->chromatix_parms->flat_area_noise_reduction_level)];
  flat_gain2 = (16-flat_gain1);
  texture_gain1 = conv_table[(camsensor_static_params_ptr->chromatix_parms->texture_noise_reduction_level)];
  texture_gain2 = (16-texture_gain1);

  index = (cam3a_aec_state.exp_index * 19) / (cam3a_aec_state.num_exposure_table_values);

  tr1 = (uint16) (((camsensor_static_params_ptr->chromatix_parms->hjr_k_table[index]*5)+5)/9);
  tr2 = (uint16) tr1*(camsensor_static_params_ptr->chromatix_parms->texture_threshold);


  /* Blue and red pixels kernel offsets from thr pixel of interest*/
  a1 = (int16)(-2*width);
  a5 = (int16)(2*width);

  /* Copy first two rows and two samples from thrid one - no filtering*/
  for (i=0; i<2*width+2;i++)
    *dataOut++ = *dataIn++;

  /* Assumes BGGR or RGGB */ 
  for (i=0; i<height-4;i++)
  {
    for (j=0; j<width-4;)
    {
      /* Sum 5 pixel values in sum and 5 pixel squares in tmp2 */
      tmpB =  *(dataIn + a1); 
      sum = (uint32)tmpB; tmp2 = pix_sqr[tmpB];
      tmpB =  *(dataIn - 2); 
      sum += (uint32)tmpB; tmp2 += pix_sqr[tmpB];
      tmpB =  *(dataIn);
      j++;
      sum += (uint32)tmpB; tmp2 += pix_sqr[tmpB];
      tmpB =  *(dataIn + 2); 
      sum += (uint32)tmpB; tmp2 += pix_sqr[tmpB];
      tmpB =  *(dataIn + a5); 
      sum += (uint32)tmpB; tmp2 += pix_sqr[tmpB];


      /* Divide pixels sum by 5 */
      mean=(sum*819+2048)>>12;

      /* Compute variance for the kernel */
      tmp4=(uint32)((tmp2*5)-(sum*sum));

      /* Compare vs first thershold - if less then it is flat area 
         Filter more */
      if (tmp4<mean*tr1)
      {
        *dataOut++ = (uint8)(((flat_gain1*(uint16)(*dataIn++)) + (flat_gain2*(uint16)mean) + 8)>>4);
      }
      /* If not, compare vs second threshold - if less then it is texture 
         Filter less */
      else if (tmp4<tr2*mean)
      {
        *dataOut++ = (uint8)(((texture_gain1*(uint16)(*dataIn++)) + (texture_gain2*(uint16)mean) + 8)>>4);
      }
      /* If none of the above then it is flat edge 
         Do not filter */
      else
      {
        (*dataOut++) = (*dataIn++);
      }
    }
    /* Copy last two sample in row and first two samples in next one */
    for (j=0;j<4;j++)
    {
      *dataOut++ = *dataIn++;
    }
  }

  for (i=0; i<2*width-2;i++)
  {
    *dataOut++ = *dataIn++;
  }
}

#ifdef CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED

/* 10 bit bayer noise reduction is not supported. 
 * Please do not use 10-bit raw data as
 * input for the bayer noise reduction  */


/*===========================================================================

FUNCTION      HJR_10_BIT_BAYER_FILTERING

DESCRIPTION
              PERFORMS BAYER FILTERING ON THE 10 BIT DATA.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


static void hjr_10_bit_bayerfiltering(uint16 *dataIn, uint16 *dataOut, int16 width,
              int16 height, uint16 threshold, uint16 scaleQ12,
              CAMIF_InputFormatType bayer_format)
{
  int16 green[8],rb[8]; 
  uint16 i,j,k;
  int16 tmp1, tmp2;
  int16 *ref_1_ptr;
  int16 *ref_2_ptr;

  green[0] = -2*width;
  green[1] = -width - 1;
  green[2] = -width + 1;
  green[3] = -2;
  green[4] = 2;
  green[5] = width - 1;
  green[6] = width + 1;
  green[7] = 2*width;

  rb[0] = -2*width-2;
  rb[1] = -2*width;
  rb[2] = -2*width + 2;
  rb[3] = -2;
  rb[4] = 2;
  rb[5] = 2*width - 2;
  rb[6] = 2*width ;
  rb[7] = 2*width+2;

  if ((bayer_format == CAMIF_BAYER_B_G) || (bayer_format == CAMIF_BAYER_R_G))
  {
    ref_1_ptr = rb;
    ref_2_ptr = green;
  }
  else
  {
    ref_1_ptr = green;
    ref_2_ptr = rb;
  }
  
  for (i=0; i<2*width+2;i++)
    *dataOut++ = *dataIn++;
        
  for (i=0; i<height/2-2;i++)
  { 
    for (j=0;j<width/2-2; j++)
    {
      tmp2 =0;
      for (k=0;k<8;k++)
      {
        tmp1 =  *(dataIn+ref_1_ptr[k]) - *dataIn;
        if (HJR_ABS(tmp1)<threshold)
          tmp2 += tmp1;
      }
      tmp1 = (int16)(*dataIn + ((tmp2*scaleQ12+(1<<11))>>12));
      tmp1 = HJR_MIN(1024,tmp1);
      *dataOut++ =  HJR_MAX(0,tmp1);
      dataIn++;

      tmp2 =0;
      for (k=0;k<8;k++)
      {
        tmp1 =  *(dataIn+ref_2_ptr[k]) - *dataIn;
        if (HJR_ABS(tmp1)<threshold)
          tmp2 += tmp1;
      }
      tmp1 = (int16)(*dataIn + ((tmp2*scaleQ12+(1<<11))>>12));
      tmp1 = HJR_MIN(1024,tmp1);
      *dataOut++ =  HJR_MAX(0,tmp1);
      dataIn++;
    }
    for (j=0;j<4;j++)
      *dataOut++ = *dataIn++;
      
    for (j=0;j<width/2-2; j++)
    {
      tmp2 =0;
      for (k=0;k<8;k++)
      {
        tmp1 =  *(dataIn+ref_2_ptr[k]) - *dataIn;
        if (HJR_ABS(tmp1)<threshold)
          tmp2 += tmp1;
      }
      tmp1 = (int16)(*dataIn + ((tmp2*scaleQ12+(1<<11))>>12));
      tmp1 = HJR_MIN(1024,tmp1);
      *dataOut++ =  HJR_MAX(0,tmp1);
      dataIn++;
      
      tmp2 =0;
      for (k=0;k<8;k++)
      {
        tmp1 =  *(dataIn+ref_1_ptr[k]) - *dataIn;
        if (HJR_ABS(tmp1)<threshold)
          tmp2 += tmp1;
      }
      tmp1 = (int16)(*dataIn + ((tmp2*scaleQ12+(1<<11))>>12));
      tmp1 = HJR_MIN(1024,tmp1);
      *dataOut++ =  HJR_MAX(0,tmp1);
      dataIn++;
    }
     
    for (j=0;j<4;j++)
      *dataOut++ = *dataIn++;
  }

  for (i=0; i<2*width+2;i++)
    *dataOut++ = *dataIn++;

}

#endif /* CAMERA_HJR_10_BIT_BAYER_FILTER_SUPPORTED */
#endif /* ifndef QDSP_MODULE_VFE25_DEFINED */

/* The following code is used for multi-frame HJR */

/*===========================================================================

FUNCTION: CAMERA_HJR_FREE_PROJECTIONS

DESCRIPTION:
  Checks to see if any projects have been allocated and frees them if
  nessasary.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  NONE
============================================================================*/

void camera_hjr_free_projection(proj_t *proj)
{
  if ( proj->h1 != NULL )    free(proj->h1);
  if ( proj->h2 != NULL )    free(proj->h2);
  if ( proj->h3 != NULL )    free(proj->h3);
  if ( proj->h4 != NULL )    free(proj->h4);
  if ( proj->v1 != NULL )    free(proj->v1);
  if ( proj->v2 != NULL )    free(proj->v2);
  if ( proj->v3 != NULL )    free(proj->v3);
  if ( proj->v4 != NULL )    free(proj->v4);
  proj->h1 = proj->h2 = proj->h3 = proj->h4 = proj->v1 = proj->v2 = proj->v3 = proj->v4 = NULL;
}

/*===========================================================================

FUNCTION: CAMERA_HJR_ALLOCATE_PROJECTION

DESCRIPTION:
  Allocates all of the memory needed for the projects of one frame.
  If any one allocation fails, all memory if freed and IP_NO_MEMORY is returned

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  IPL_SUCCESS if successful.  IP_NO_MEMORY if the allocation fails
============================================================================*/
ipl_status_type camera_hjr_allocate_projection(proj_t *proj, uint16 width, uint16 height)
{
  proj->h1 = proj->h2 = proj->h3 = proj->h4 = proj->v1 = proj->v2 = proj->v3 = proj->v4 = NULL;

  proj->h1 = (uint32*)malloc(width*sizeof(uint32));
  proj->h2 = (uint32*)malloc(width*sizeof(uint32));
  proj->h3 = (uint32*)malloc(width*sizeof(uint32));
  proj->h4 = (uint32*)malloc(width*sizeof(uint32));
  proj->v1 = (uint32*)malloc(height*sizeof(uint32));
  proj->v2 = (uint32*)malloc(height*sizeof(uint32));
  proj->v3 = (uint32*)malloc(height*sizeof(uint32));
  proj->v4 = (uint32*)malloc(height*sizeof(uint32));

  if ( (proj->h1 == NULL) || (proj->h2 == NULL) || (proj->h3 == NULL) || (proj->h4 == NULL) ||
       (proj->v1 == NULL) || (proj->v2 == NULL) || (proj->v3 == NULL) || (proj->v4 == NULL) )
  {
    /* call free projections because checks which allocations failed */
    camera_hjr_free_projection(proj);
    return IPL_NO_MEMORY;
  }
  return IPL_SUCCESS;
}

/*===========================================================================

FUNCTION: CAMERA_HJR_SMART_REGISTER_FRAMES

DESCRIPTION:
  Entry point for the registering of two frames for HJR.  Takes
  two or three frames and automatically registers the frames to create
  one frame with reduced hand jitter

DEPENDENCIES:

SIDE EFFECTS:,

RETURN VALUE:
  IPL_SUCCESS on success.  ILP_NO_MEMORY if memory can't be allocated for the algorithm.
  IPL_FAILURE is an input value is invaled
============================================================================*/
ipl_status_type camera_hjr_smart_register_frames(ipl_image_type **in_image, 
                                                 uint16 max_corr_Xlag, uint16 max_corr_Ylag)
{
  ipl_image_type *frame1, *frame2, *frame3;
  uint16 numberFrames;
  proj_t proj1, proj2;
  m_vector_t m_vec12, m_vec13;
  uint16 max_corr_factor;
  uint32 *corr_buffer;
  uint32 height, width;
  ipl_col_for_type cs;
  ipl_status_type ipl_ret = IPL_SUCCESS;

  /* Check for valid input values */
  if ( in_image == NULL )
    return IPL_FAILURE;

  frame1 = in_image[0];
  frame2 = in_image[1];
  frame3 = in_image[2];

  if ( (frame1 == NULL) || (frame2 == NULL) )
    return IPL_FAILURE;

  /* In the two frame case, frame three is set to null */
  /* If more frames are given, they are ignored */
  if ( frame3 == NULL )
    numberFrames = 2;
  else
    numberFrames = 3;


  height = frame1->dy;
  width = frame1->dx;
  cs = frame1->cFormat;

  /* check to make sure the dimisions and format are the same */
  if ( (height != frame2->dy) || (width != frame2->dx) || (cs != frame2->cFormat) )
    return IPL_FAILURE;

  if ( (numberFrames == 3)&& ((height != frame3->dy) || (width != frame3->dx) || (cs != frame3->cFormat)) )
    return IPL_FAILURE;


  if ( (cs != IPL_YCrCb420_LINE_PK) && (cs != IPL_YCrCb422_LINE_PK) )
    return IPL_FAILURE;

  /* allocate the work buffer for corrolating the motion vecotors
  This is done to eliminate multiple malloc/frees */
  max_corr_factor = max_corr_Xlag > max_corr_Ylag ? max_corr_Xlag : max_corr_Ylag;
  corr_buffer = (uint32*)malloc((2*max_corr_factor+1)*sizeof(uint32));
  if ( corr_buffer == NULL )
  {
    return IPL_NO_MEMORY;
  }

  /* Allocate memory for the projections */
  if ( camera_hjr_allocate_projection(&proj1,width,height) != IPL_SUCCESS )
  {
    free(corr_buffer);
    camera_hjr_free_projection(&proj1);
    return IPL_NO_MEMORY;
  }
  if ( camera_hjr_allocate_projection(&proj2,width,height) != IPL_SUCCESS )
  {
    free(corr_buffer);
    camera_hjr_free_projection(&proj1);
    camera_hjr_free_projection(&proj2);
    return IPL_NO_MEMORY;
  }

  /* caculate the projections for frame 1 and 2 */
  if((camera_hjr_get_projections(frame1->imgPtr, width , height, &proj1) == IPL_SUCCESS) &&
     (camera_hjr_get_projections(frame2->imgPtr, width , height, &proj2) == IPL_SUCCESS))
  {
      /* Calculate the motion vectors from the projections */
      m_vec12.hor[0]  = camera_hjr_corr1D(proj1.h1, proj2.h1, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[1]  = camera_hjr_corr1D(proj1.h1+width/4, proj2.h1+width/4, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[2]  = camera_hjr_corr1D(proj1.h1+width/2, proj2.h1+width/2, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[3]  = camera_hjr_corr1D(proj1.h2, proj2.h2, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[4]  = camera_hjr_corr1D(proj1.h2+width/4, proj2.h2+width/4, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[5]  = camera_hjr_corr1D(proj1.h2+width/2, proj2.h2+width/2, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[6]  = camera_hjr_corr1D(proj1.h3, proj2.h3, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[7]  = camera_hjr_corr1D(proj1.h3+width/4, proj2.h3+width/4, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.hor[8]  = camera_hjr_corr1D(proj1.h3+width/2, proj2.h3+width/2, width/2, max_corr_Xlag, corr_buffer);
      m_vec12.vert[0] = camera_hjr_corr1D(proj1.v1, proj2.v1, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[3] = camera_hjr_corr1D(proj1.v1+height/4, proj2.v1+height/4, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[6] = camera_hjr_corr1D(proj1.v1+height/2, proj2.v1+height/2, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[1] = camera_hjr_corr1D(proj1.v2, proj2.v2, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[4] = camera_hjr_corr1D(proj1.v2+height/4, proj2.v2+height/4, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[7] = camera_hjr_corr1D(proj1.v2+height/2, proj2.v2+height/2, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[2] = camera_hjr_corr1D(proj1.v3, proj2.v3, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[5] = camera_hjr_corr1D(proj1.v3+height/4, proj2.v3+height/4, height/2, max_corr_Ylag, corr_buffer);
      m_vec12.vert[8] = camera_hjr_corr1D(proj1.v3+height/2, proj2.v3+height/2, height/2, max_corr_Ylag, corr_buffer);
    
      /* combine frames 1 and 2 */
      if ( (ipl_ret = camera_hjr_smart_combine_frames(frame1, frame2, &m_vec12,width, height, 128, cs)) == IPL_SUCCESS )
      {
        if ( numberFrames >2 )
        {
          /* In the three frame case, combine frame 1 and 3 */
          /* reuse the memory allocated for frame 2 projections for frame 3 */
          /* Frame 1 projections would not have changed */
          camera_hjr_get_projections(frame3->imgPtr, width , height, &proj2); 
          /* caculate the motion vecotors between frame 1 and 3 */
          m_vec13.hor[2]  = camera_hjr_corr1D(proj1.h1, proj2.h1, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[1]  = camera_hjr_corr1D(proj1.h1+width/4, proj2.h1+width/4, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[0]  = camera_hjr_corr1D(proj1.h1+width/2, proj2.h1+width/2, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[5]  = camera_hjr_corr1D(proj1.h2, proj2.h2, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[4]  = camera_hjr_corr1D(proj1.h2+width/4, proj2.h2+width/4, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[3]  = camera_hjr_corr1D(proj1.h2+width/2, proj2.h2+width/2, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[8]  = camera_hjr_corr1D(proj1.h3, proj2.h3, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[7]  = camera_hjr_corr1D(proj1.h3+width/4, proj2.h3+width/4, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.hor[6]  = camera_hjr_corr1D(proj1.h3+width/2, proj2.h3+width/2, width/2, max_corr_Xlag, corr_buffer);
          m_vec13.vert[2] = camera_hjr_corr1D(proj1.v1, proj2.v1, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[5] = camera_hjr_corr1D(proj1.v1+height/4, proj2.v1+height/4, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[8] = camera_hjr_corr1D(proj1.v1+height/2, proj2.v1+height/2, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[1] = camera_hjr_corr1D(proj1.v2, proj2.v2, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[4] = camera_hjr_corr1D(proj1.v2+height/4, proj2.v2+height/4, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[7] = camera_hjr_corr1D(proj1.v2+height/2, proj2.v2+height/2, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[0] = camera_hjr_corr1D(proj1.v3, proj2.v3, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[3] = camera_hjr_corr1D(proj1.v3+height/4, proj2.v3+height/4, height/2, max_corr_Ylag, corr_buffer);
          m_vec13.vert[6] = camera_hjr_corr1D(proj1.v3+height/2, proj2.v3+height/2, height/2, max_corr_Ylag, corr_buffer);
          /* combine the frames betwee frame 1 and 3.  Frame 3's is using a smaller factor */
          if((ipl_ret = camera_hjr_smart_combine_frames(frame1, frame3, &m_vec13,width, height, 85, cs) ) != IPL_SUCCESS )
          {
            ipl_ret = IPL_NO_MEMORY;
          }
        }
      }else{
        ipl_ret = IPL_NO_MEMORY;
      }
    }else{
      ipl_ret = IPL_NO_MEMORY;
    }
  /* Free the allocated memory */
  free(corr_buffer);

  camera_hjr_free_projection(&proj1);
  camera_hjr_free_projection(&proj2);

  return ipl_ret;
}


/*===========================================================================

FUNCTION: CAMERA_HJR_GET_PROJECTIONS

DESCRIPTION:
  Creates the projects from an input frame.  

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
============================================================================*/
ipl_status_type camera_hjr_get_projections(uint8 *fr, uint32 width , uint32 height, proj_t *proj)
{
  uint32 i,j, h4,w4;
  uint32 *temp1,*temp2,*temp3,*temp4,*temph;
  uint32 wstop;
  uint8 *pixel;

  if(fr == NULL)
    return IPL_FAILURE;

  /* caculate the quadrents */
  h4=height/4;
  w4=width/4;

  pixel = fr;

  /* Initialize the projections to zero */
  memset(proj->h1,0,width*sizeof(proj->h1));
  memset(proj->h2,0,width*sizeof(proj->h2));
  memset(proj->h3,0,width*sizeof(proj->h3));
  memset(proj->h4,0,width*sizeof(proj->h4));

  memset(proj->v1,0,height*sizeof(proj->v1));
  memset(proj->v2,0,height*sizeof(proj->v2));
  memset(proj->v3,0,height*sizeof(proj->v3));
  memset(proj->v4,0,height*sizeof(proj->v4));

  /* Temp pointers to the vertical projects */
  temp1 = proj->v1;
  temp2 = proj->v2;
  temp3 = proj->v3;
  temp4 = proj->v4;
  /* go through the first quadrent */
  for ( i=0;i<h4;i++ )
  {
    temph = proj->h1;
    for ( j=0;j<w4;j++ )
    {   /* calulate for the first fourth incrementing ponters */
      *temph++ += *fr;
      *temp1 += *fr++;
    }
    wstop = w4+w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp2 += *pixel++;
    }
    wstop += w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp3 += *pixel++;
    }
    for ( ;j<width;j++ )
    {
      *temph++ += *pixel;
      *temp4 += *pixel++;
    }
    /* increment the temp vertical pointers */
    temp1++; temp2++; temp3++; temp4++;
  }

  for ( i=h4;i<2*h4;i++ )
  {
    temph = proj->h2;
    for ( j=0;j<w4;j++ )
    {
      *temph++ += *pixel;
      *temp1 += *pixel++;
    }
    wstop = w4+w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp2 += *pixel++;
    }
    wstop += w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp3 += *pixel++;
    }
    for ( ;j<width;j++ )
    {
      *temph++ += *pixel;
      *temp4 += *pixel++;
    }
    /* increment the temp vertical pointers */
    temp1++; temp2++; temp3++; temp4++;
  }

  for ( i=2*h4;i<3*h4;i++ )
  {
    temph = proj->h3;
    for ( j=0;j<w4;j++ )
    {
      *temph++ += *pixel;
      *temp1 += *pixel++;
    }
    wstop = w4 + w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp2 += *pixel++;
    }
    wstop += w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp3 += *pixel++;
    }
    for ( ;j<width;j++ )
    {
      *temph++ += *pixel;
      *temp4 += *pixel++;
    }
    /* increment the temp vertical pointers */
    temp1++; temp2++; temp3++; temp4++;
  }

  for ( i=3*h4;i<height;i++ )
  {
    temph = proj->h4;
    for ( j=0;j<w4;j++ )
    {
      *temph++ += *pixel;
      *temp1 += *pixel++;
    }
    wstop = w4 + w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp2 += *pixel++;
    }
    wstop += w4;
    for ( ;j<wstop;j++ )
    {
      *temph++ += *pixel;
      *temp3 += *pixel++;
    }
    for ( ;j<width;j++ )
    {
      *temph++ += *pixel;
      *temp4 += *pixel++;
    }
    /* increment the temp vertical pointers */
    temp1++; temp2++; temp3++; temp4++;
  }

  temp1 = proj->h1;
  temp2 = proj->h2;
  temp3 = proj->h3;
  temp4 = proj->h4;

  for ( i=0;i<width;i++ )
  {
    *temp1++ += *temp2;
    *temp2++ += *temp3;
    *temp3++ += *temp4;
  }
  temp1 = proj->v1;
  temp2 = proj->v2;
  temp3 = proj->v3;
  temp4 = proj->v4;
  for ( i=0;i<height;i++ )
  {
    *temp1++ +=  *temp2;
    *temp2++ += *temp3;
    *temp3++ += *temp4;
  }

  return IPL_SUCCESS;
}


/*===========================================================================

FUNCTION: CORR1D

DESCRIPTION:
  Creates the correlation between two projections for a specific dimension

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
Correlation value
============================================================================*/
int16 camera_hjr_corr1D(uint32 *sum1,uint32 *sum2, uint32 len, uint32 max_lag, uint32 *work_buffer)
{
  uint8 scale;
  uint32 N,i,j, ind1, ind2;
  int32 max_ind;
  int32 tmp32;
  uint32 *ptr1;
  uint32 *ptr2;
  uint32 *corr;
  uint32 tmp;

  corr = work_buffer;

  if ( len < 256 )
    scale = 8;
  else if ( len < 512 )
    scale = 9;
  else if ( len < 1024 )
    scale = 10;
  else if ( len < 2048 )
    scale = 11;
  else
    scale = 12;

  N = len-max_lag;
  ind1 = 0; 
  ind2 = max_lag;

  for ( i=0;i<2*max_lag+1;i++ )
  {
    ptr1=sum1+ind1;
    ptr2=sum2+ind2;
    tmp = 0;
    for ( j=0;j<N;j++ )
    {
      tmp32 = (int32)(*ptr1++)-(int32)(*ptr2++);
      //tmp = tmp + (JR_ABS(tmp32));
      tmp32=tmp32>>4;
      tmp += (tmp32*tmp32)>>scale;
    }
    corr[i] = tmp;

    if ( ind1<ind2 )
    {
      ind2 -= 1;
    } else
    {
      ind1 += 1;
    }
  }

  tmp = corr[0];
  max_ind =0;

  for ( i=1;i< 2*max_lag+1;i++ )
  {
    if ( tmp > corr[i] )
    {
      tmp = corr[i];
      max_ind = i;
    }
  }
  return(int16)(max_lag - max_ind);
}


/*===========================================================================

FUNCTION: CAMERA_HJR_INTERPOLATE_VECTOR

DESCRIPTION:
  Interpolates the motion vectors for each line of the image.  

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
============================================================================*/
void camera_hjr_interpolate_vector(int32 motion1, int32 motion2, uint32 size, int32 *vector, uint8 inv)
{
  uint32 i;
  register int32 diff,tmp;

  /* caculate the difference between two rows of the frame */
  diff = motion2-motion1;
  if ( diff>0 )
    diff=2*diff/size;
  else
    diff=-((-2*diff)/(int32)size);

  /* loop through caculating the motion for each row */
  tmp = motion1;
  if ( !inv )
  {
    for ( i=0;i<size; i++ )
    {
      *vector++ = tmp;
      tmp += diff;
    }
  } else
  {
    vector = vector+size-1;
    for ( i=0;i<size; i++ )
    {
      *vector-- = tmp;
      tmp += diff;
    }
  }
}

/*===========================================================================

FUNCTION: SMART_COMBINE_FRAMES

DESCRIPTION:
  Takes the registration data between two frames and combines the two frames.
  The first frame is overwritten by the new combined frames.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  IPL_SUCCESS for combining the frames.  IPL_NO_MEMORY if a memory allocation
  error occures.
============================================================================*/

ipl_status_type camera_hjr_smart_combine_frames(ipl_image_type *frame1, ipl_image_type *frame2, m_vector_t *vec, 
                                                int32 width, int32 height, uint8 scaleQ8, ipl_col_for_type cs) 
{
  uint16 scaleQ8_inv;
  uint16 tmp;  /* used in the caculation of the new pixel data */
  uint8 *chr1;  /* Croma for the first frame */
  uint8 *chr2;  /* Croma for the second frame */
  uint8 *fr1;   /* Y data for the first frame */
  uint8 *fr2;   /* Y data for the second frame */
  int32 i,j;   /* Counters for looping through the data */
  int32 I,J;    /* Current offset for the vertical and horizontal directions */
  int32 *p1_h;  
  int32 *p2_h;
  int32 *p3_h;
  int32 *p1_v;
  int32 *p2_v;
  int32 *p3_v;
  uint8 *TempFr1,*TempFr2;  /* Temp pointers to the pixel data currently being processed */
  int32 maxshiftI,maxshiftJ,nextbreak,stopwidth, inextbreak,jnextbreak;
  int32 hnoff,vnoff;
  int32 iOrgImage,iCromaFactor;  /* For caculating Croma, we need the orginal offset for the line */
  int32 HorInitalOffset, HorSlope,VerInitalOffset,VerSlope; 
  int32 OrgWidth = width;

  scaleQ8_inv = (1<<8)-scaleQ8;
  fr1 = frame1->imgPtr;
  fr2 = frame2->imgPtr;
  TempFr1 = fr1; /* Just initialization so the compiler won't complain */
  TempFr2 = fr2; /* Just initialization so the compiler won't complain */

  /* Allocate interperlated vectors */
  p1_h=(int32*)malloc(height*sizeof(uint32));
  p2_h=(int32*)malloc(height*sizeof(uint32));
  p3_h=(int32*)malloc(height*sizeof(uint32));
  p1_v=(int32*)malloc(height*sizeof(uint32));
  p2_v=(int32*)malloc(height*sizeof(uint32));
  p3_v=(int32*)malloc(height*sizeof(uint32));

  /* Make sure we allocated the buffers */
  if ( (p1_h == NULL) || (p2_h == NULL) || (p3_h == NULL) || (p1_v == NULL) || (p2_v == NULL) || (p3_v == NULL))
  {
    /* allocation failed, free sucessful allocations */
    if ( p1_h != NULL ) free(p1_h);
    if ( p2_h != NULL ) free(p2_h);
    if ( p3_h != NULL ) free(p3_h);
    if ( p1_v != NULL ) free(p1_v);
    if ( p2_v != NULL ) free(p2_v);
    if ( p3_v != NULL ) free(p3_v);
    return IPL_NO_MEMORY;
  }

  /* Interpolate from the correlation vectors for three columns of the image */
  camera_hjr_interpolate_vector(vec->hor[3]<<20, vec->hor[0]<<20, height/2, p1_h, 1);
  camera_hjr_interpolate_vector(vec->hor[3]<<20, vec->hor[6]<<20, height/2, p1_h+height/2, 0);
  camera_hjr_interpolate_vector(vec->vert[3]<<20, vec->vert[0]<<20, height/2, p1_v, 1);
  camera_hjr_interpolate_vector(vec->vert[3]<<20, vec->vert[6]<<20, height/2, p1_v+height/2, 0);

  camera_hjr_interpolate_vector(vec->hor[4]<<20, vec->hor[1]<<20, height/2, p2_h, 1);
  camera_hjr_interpolate_vector(vec->hor[4]<<20, vec->hor[7]<<20, height/2, p2_h+height/2, 0);
  camera_hjr_interpolate_vector(vec->vert[4]<<20, vec->vert[1]<<20, height/2, p2_v, 1);
  camera_hjr_interpolate_vector(vec->vert[4]<<20, vec->vert[7]<<20, height/2, p2_v+height/2, 0);

  camera_hjr_interpolate_vector(vec->hor[5]<<20, vec->hor[2]<<20, height/2, p3_h, 1);
  camera_hjr_interpolate_vector(vec->hor[5]<<20, vec->hor[8]<<20, height/2, p3_h+height/2, 0);
  camera_hjr_interpolate_vector(vec->vert[5]<<20, vec->vert[2]<<20, height/2, p3_v, 1);
  camera_hjr_interpolate_vector(vec->vert[5]<<20, vec->vert[8]<<20, height/2, p3_v+height/2, 0);

  for ( i=0;i<height;i++ )
  {
    /* calculate the Initial offset and slope for the horizontal and vertical shifts between the images */
    camera_hjr_interpolate_vectorLine(p1_v[i], p2_v[i], p3_v[i],width, &VerInitalOffset, &VerSlope);
    camera_hjr_interpolate_vectorLine(p1_h[i], p2_h[i], p3_h[i],width, &HorInitalOffset, &HorSlope);
    I=(VerInitalOffset+SHIFT19)>>20; /* initial offset */
    J=(HorInitalOffset+SHIFT19)>>20; /* initial offset */

    if ( VerSlope == 0 )
    { /* if the slope is zero, then the caculated values will not change on this row */
      inextbreak = width;
      vnoff = width;
      maxshiftI = I;
    }
    else
    {
      inextbreak = SHIFT20/VerSlope;  /* The length of a segment that the vertical offset will not change */
      if ( inextbreak < 0 )  inextbreak = -inextbreak; /* Get the absolute value */
      vnoff = (VerInitalOffset%SHIFT20)/VerSlope;   /* Caculate the inital segment this value holds true for */
      if ( vnoff < 0 ) vnoff = -vnoff;  /* Get the absolute value */
      maxshiftI = I + ((VerSlope*width)>>20);  /* Calculate the maximum vertical offset for this row */
    }
     /* same caculations as above, but for the horizontal offsets */
    if ( HorSlope == 0 )
    {
      jnextbreak = width;
      hnoff = width;
      maxshiftJ = J;
    } else
    {
      jnextbreak = SHIFT20/HorSlope;
      if ( jnextbreak < 0 )  jnextbreak = -jnextbreak;
      hnoff = (HorInitalOffset%SHIFT20)/HorSlope;
      if ( hnoff < 0 ) hnoff = -hnoff;
      maxshiftJ = J + ((HorSlope*width)>>20);
    }

    /* first let see if this line, will ever overlap */
    if ( (((i + I) >= 0) || ((i + I + maxshiftI) >= 0)) &&
         (((i + I) < height) || ((i + I + maxshiftI) < height)) )
    {
      /* Initialze the values */
      if ( J < 0 )
      {
        /* If the inital hor shift is negitave, then we can start that number of pixels into the row */
        j = -J;
        stopwidth = width - maxshiftJ + J + 1;  /* caculate how many pixels neede for horizontal shifts */
      } else
      {
        j = 0;
        stopwidth = width - maxshiftJ;
      }
      stopwidth = width < stopwidth ? width : stopwidth; /* Get the overall effect lenght of the row */
      TempFr1  = &fr1[i*width+j];  /* Get the location of the pixel in frame 1 */
      while ( j<stopwidth )
      {
        nextbreak = vnoff < hnoff ? vnoff : hnoff; /* min change of pixel */
        if ( j < nextbreak )
        { /* catch a special case that the start of the line is after the first break */
          if ( (I+i >= 0) && (I+i < height) )
          {
            nextbreak = nextbreak < stopwidth ? nextbreak : stopwidth;
            TempFr2 = &fr2[(I+i)*width+j+J];
            if ( scaleQ8 == 128 )
            {
              /* This is evenly merge the images, so use the faster shifts */
              for ( ;j < nextbreak;j++ )
              {
                tmp=(uint16)((*TempFr1) + (uint16)(*TempFr2++));
                *TempFr1++ = (uint8)(tmp>>1);
              }
            } else
            {
              /* Generic case, need to multiply by the scaling factor */
              for ( ;j < nextbreak;j++ )
              {
                tmp=(uint16)((*TempFr1*scaleQ8_inv) + (uint16)(*TempFr2++*scaleQ8));
                *TempFr1++ = (uint8)((tmp+SHIFT7)>>8);
              }
            }
          }
          else
          {
            /* Move the frames forward so the loop will continue */
            TempFr1 += nextbreak - j;
            TempFr2 += nextbreak - j;
            j = nextbreak;
          }
        }
        /* caculate the break */
        /* If both cases are true we will update vertical then loop back around
           effectively dowing nothing and update the horizontal variables */
        if ( nextbreak == vnoff )
        {  /* we are at a vertical break it the offset */
          vnoff += inextbreak;
          if ( VerSlope > 0 )
            I++;
          else
            I--;
        } 
        else
        { /* we are at the horizontal break in the offset */
          hnoff += jnextbreak;
          if ( HorSlope > 0 )
            J++;
          else
            J--;
        }
        if ( (i+I < 0) || (i+I > height) ) /* end of the usful part of the line */
          break; /* just out of the while loop */

      }
    }
  }

  chr1 = frame1->clrPtr;
  chr2 = frame2->clrPtr;
  iCromaFactor = 0;

  if ( cs == IPL_YCrCb420_LINE_PK )
  {
    height = height/2;
    iCromaFactor = 1;
    width = width/2;

  } else if ( cs == IPL_YCrCb422_LINE_PK )
  {
    width = width/2;
  }

  /* Chroma Registration */

  for ( i=0;i<height;i++ )
  {
    iOrgImage = i<<iCromaFactor;
    camera_hjr_interpolate_vectorLine(p1_v[iOrgImage], p2_v[iOrgImage], p3_v[iOrgImage],OrgWidth, &VerInitalOffset, &VerSlope);
    camera_hjr_interpolate_vectorLine(p1_h[iOrgImage], p2_h[iOrgImage], p3_h[iOrgImage],OrgWidth, &HorInitalOffset, &HorSlope);
    I=((VerInitalOffset>>iCromaFactor)+SHIFT19)>>20; /* initial offset */
    J=((HorInitalOffset>>1)+SHIFT19)>>20; /* initial offset */

    if ( VerSlope == 0 )
    {
      inextbreak = width;
      vnoff = width;
      maxshiftI = I;
    } else
    {
      inextbreak = (SHIFT20/(VerSlope<<iCromaFactor));
      if ( inextbreak < 0 )  inextbreak = -inextbreak;
      vnoff = ((VerInitalOffset>>1)%SHIFT20)/(VerSlope<<1);
      if ( vnoff < 0 ) vnoff = -vnoff;
      maxshiftI = I + (((VerSlope<<iCromaFactor)*width)>>20);
    }
    if ( HorSlope == 0 )
    {
      jnextbreak = width;
      hnoff = width;
      maxshiftJ = J;
    } else
    {
      jnextbreak = SHIFT20/(HorSlope<<iCromaFactor);
      if ( jnextbreak < 0 )  jnextbreak = -jnextbreak;
      hnoff = ((HorInitalOffset>>iCromaFactor)%SHIFT20)/(HorSlope<<iCromaFactor);
      if ( hnoff < 0 ) hnoff = -hnoff;
      maxshiftJ = J + (((HorSlope<<iCromaFactor)*width)>>20);
    }

    /* first let see if this line, will ever overlap */
    if ( (((i + I) >= 0) || ((i + I + maxshiftI) >= 0)) &&
         (((i + I) < height) || ((i + I + maxshiftI) < height)) )
    {
      if ( J < 0 )
      {
        j = -J;
        stopwidth = width - maxshiftJ + J + 1;
      } else
      {
        j = 0;
        stopwidth = width - maxshiftJ;
      }
      stopwidth = width < stopwidth ? width : stopwidth;
      TempFr1 = &chr1[(i*width+j)<<1];
      while ( j<stopwidth )
      {
        nextbreak = vnoff < hnoff ? vnoff : hnoff; /* min change of pixel */
        if ( j < nextbreak )
        { /* catch a special case that the start of the line is after the first break */
          if ( (i+I >= 0) && (i+I < height) )
          {
            nextbreak = nextbreak < stopwidth ? nextbreak : stopwidth;
            if ( j < nextbreak )
            { /* catch a special case that the start of the line is after the first break */
              TempFr2 = &chr2[((I+i)*width+j+J)<<1];
              if ( scaleQ8 == 128 )
              {
                for ( ;j < nextbreak;j++ )
                {
                  tmp=(uint16)((*TempFr1) + (uint16)(*TempFr2++));
                  *TempFr1++ = (uint8)(tmp>>1);
                  tmp=(uint16)((*TempFr1) + (uint16)(*TempFr2++));
                  *TempFr1++ = (uint8)(tmp>>1);
                }
              } else
              {
                for ( ;j < nextbreak;j++ )
                {
                  tmp=(uint16)((*TempFr1*scaleQ8_inv) + (uint16)(*TempFr2++*scaleQ8));
                  *TempFr1++ = (uint8)((tmp+SHIFT7)>>8);
                  tmp=(uint16)((*TempFr1*scaleQ8_inv) + (uint16)(*TempFr2++*scaleQ8));
                  *TempFr1++ = (uint8)((tmp+SHIFT7)>>8);
                }
              }
            } else
            {
              TempFr1 += (nextbreak - j)<<1;
              TempFr2 += (nextbreak - j)<<1;
              j = nextbreak;
            }
          }
        }
        /* caculate the break */
        if ( nextbreak == vnoff )
        {
          vnoff += inextbreak;
          if ( VerSlope > 0 )
            I++;
          else
            I--;
        } else
        {
          hnoff += jnextbreak;
          if ( HorSlope > 0 )
            J++;
          else
            J--;
        }
        if ( (i+I < 0) || (i+I > height) ) /* end of the usful part of the line */
          break; /* just out of the while loop */
      }
    }
  }
  /* Just free the temp vectors.  They have already been checked for NULL */
  free(p1_h);
  free(p2_h);
  free(p3_h);   
  free(p1_v);
  free(p2_v);
  free(p3_v);
  return IPL_SUCCESS;
}

/*===========================================================================

FUNCTION: CAMERA_HJR_INTERPOLATE_VECTORLINE

DESCRIPTION:
  Calulates the inital offset and slope for a line given the 
  motion vec

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
============================================================================*/
void camera_hjr_interpolate_vectorLine(int32 motion0, int32 motion1, int32 motion2, uint32 size, int32 *InitalOffset, int32* Slope)
{
  *InitalOffset = ((4*motion0+motion1-2*motion2))/3;
  *Slope = ((motion2-motion0)*2)/(int32)size;
}
#endif /* FEATURE_CAMERA_HJR */
