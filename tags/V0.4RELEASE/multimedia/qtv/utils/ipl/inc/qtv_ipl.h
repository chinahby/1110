#ifndef _QTV_IPL_API_H_
#define _QTV_IPL_API_H_
/*=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is
  regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

$Header: //source/qcom/qct/multimedia/qtv/utils/ipl/main/latest/inc/qtv_ipl.h#7 $
$DateTime: 2008/05/08 13:55:54 $
$Change: 656417 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "ipl.h"                /* for data types from the IPL */
#include "qtv_msg.h"            /* for QTV error messages */

/*--------------------------------------------------------------------------
          Global Constants/Macros
--------------------------------------------------------------------------*/

#define QTV_IPL_CONVERT_YCBCR420TORGB_HALFWORD( lumaa1, lumaa2, lumaa3, lumaa4, cb, cr,  r, out, out2, out3, out4,  rTable, gTable, bTable ) \
  (lumaa4) -= (lumaa3); \
  (lumaa3) -= (lumaa2); \
  (lumaa2) -= (lumaa1); \
  (r) = (lumaa1) + qtv_Cr2RTable[(cr)]; \
  if ( (r) > 0 ) \
  { \
    out = rTable[ (r) ]; \
  } \
  else \
  { \
    out = 0; \
  } \
  (r) += (lumaa2); \
  if ( (r) > 0 ) \
  { \
    out2 = rTable[ (r) ]; \
  } \
  else \
  { \
    out2 = 0; \
  } \
  (r) += (lumaa3); \
  if ( (r) > 0 ) \
  { \
    out3 = rTable[ (r) ]; \
  } \
  else \
  { \
    out3 = 0; \
  } \
  (r) += (lumaa4); \
  if ( (r) > 0 ) \
  { \
    out4 = rTable[ (r) ]; \
  } \
  else \
  { \
    out4 = 0; \
  } \
  (r) = (lumaa1) - qtv_Cb2GTable[(cb)] - qtv_Cr2GTable[(cr)]; \
  if ( (r) > 0 ) \
  { \
    out += gTable[(r)]; \
  } \
  (r) += (lumaa2); \
  if ( (r) > 0 ) \
  { \
    out2 += gTable[(r)]; \
  } \
  (r) += (lumaa3); \
  if ( (r) > 0 ) \
  { \
    out3 += gTable[(r)]; \
  } \
  (r) += (lumaa4); \
  if ( (r) > 0 ) \
  { \
    out4 += gTable[(r)]; \
  } \
  (r) = (lumaa1) + qtv_Cb2BTable[(cb)]; \
  if ( (r) > 0 ) \
  { \
    out += bTable[(r)]; \
  } \
  (r) += (lumaa2); \
  if ( (r) > 0 ) \
  { \
    out2 += bTable[(r)]; \
  } \
  (r) += (lumaa3); \
  if ( (r) > 0 ) \
  { \
    out3 += bTable[(r)]; \
  } \
  (r) += (lumaa4); \
  if ( (r) > 0 ) \
  { \
    out4 += bTable[(r)]; \
  }

/*--------------------------------------------------------------------------
                     GLOBAL DECLARATIONS
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
    Table of coefficients for RGB to YCbCr convertion
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
  * lookup table which simplifies the conversion from YCBR to RGB565
  * these tables store computationally intensive intermediate
  * functions. Look at ipl_init_lookup_tables for more .
  * Selection of these smaller tables and co-locating them
  * is to aid better cache performance.
--------------------------------------------------------------------------*/
extern int16 qtv_Cr2RTable[0xff + 1];
extern int16 qtv_Cr2GTable[0xff + 1];
extern int16 qtv_Cb2GTable[0xff + 1];
extern int16 qtv_Cb2BTable[0xff + 1];

/*--------------------------------------------------------------------------
    shifted and quantized values for RGB565/444/666
    encoded images
--------------------------------------------------------------------------*/
extern uint16 qtv_ipl_r5xx[512];
extern uint16 qtv_ipl_gx6x[340];
extern uint16 qtv_ipl_bxx5[];

/*--------------------------------------------------------------------------
        Multiplication table.
--------------------------------------------------------------------------*/
extern int16 *qtv_multab3by4;

/*==========================================================================

FUNCTION QTV_IPL_CONVERTCROPANDROT90_YCBC420TORGB

DESCRIPTION
  This function rotates crops and color convert a YUV420 based
  frame into RGB444/565/666 format. Since RGB666 has a differtent packing
  altogether this function calls another helper function in that case.
  The degree of rotation is 90 degree.

  Crop with rotation: In this case the orientation of cropped image is
  assumed with respect to output.So the effect will be rotate the image
  and then do a cropping for the specifed parameters.

DEPENDENCIES
  None



ARGUMENTS IN
  i_img_ptr points to the input image
  crop is a structure informing ipl how to crop
  rotate is the rotation to do


ARGUMENTS OUT
  o_img_ptr points to the output image

RETURN VALUE
  IPL_SUCCESS   is operation was succesful
  IPL_FAILURE   otherwise

SIDE EFFECTS
  Modifies the output image buffer

==========================================================================*/
extern ipl_status_type qtv_ipl_convertCropAndRot90_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
);

/*==========================================================================

FUNCTION QTV_IPL_CONVERTCROPANDROT180_YCBC420TORGB

DESCRIPTION
  This function rotates crops and color convert a YUV420 based
  frame into RGB444/565/666 format. Since RGB666 has a differtent packing
  altogether this function calls another helper function in that case.

DEPENDENCIES
  None



ARGUMENTS IN
  i_img_ptr points to the input image
  i_frame_ptr points to the frame image
  crop is a structure informing ipl how to crop
  rotate is the rotation to do
  transparentValue is the 16 bit transparent pixel value


ARGUMENTS OUT
  o_img_ptr points to the output image

RETURN VALUE
  IPL_SUCCESS   is operation was succesful
  IPL_FAILURE   otherwise

SIDE EFFECTS
  Modifies the output image buffer

==========================================================================*/
extern ipl_status_type qtv_ipl_convertCropAndRot180_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
);

/*==========================================================================

FUNCTION QTV_IPL_CONVERTCROPANDROT270_YCBC420TORGB

DESCRIPTION
  This function rotates crops and color convert a YUV420 based
  frame into RGB444/565/666 format. Since RGB666 has a differtent packing
  altogether this function calls another helper function in that case.
  The degree of rotation is 270 degree.

  Crop with rotation: In this case the orientation of cropped image is
  assumed with respect to output.So the effect will be rotate the image
  and then do a cropping for the specifed parameters.

DEPENDENCIES
  None



ARGUMENTS IN
  i_img_ptr points to the input image
  i_frame_ptr points to the frame image
  crop is a structure informing ipl how to crop
  rotate is the rotation to do
  transparentValue is the 16 bit transparent pixel value


ARGUMENTS OUT
  o_img_ptr points to the output image

RETURN VALUE
  IPL_SUCCESS   is operation was succesful
  IPL_FAILURE   otherwise

SIDE EFFECTS
  Modifies the output image buffer

==========================================================================*/
extern ipl_status_type qtv_ipl_convertCropAndRot270_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
);

/*==========================================================================

FUNCTION    QTV_IPL_CONVERTANDCROP_YCBCR420TORGB()

DESCRIPTION
  This function will handle cropping, color converting of
  YCbCr420 based images to different RGB flavors. This function is more
  optimalif you are not cropping . This function is specifically designed
  for the favorable input condition where - there is no framing going on. So
  transparent value check is not needed. This function calls for help when
  the output is RGB666 .This because translation very different and output
  is32 bit value.

DEPENDENCIES
  None

ARGUMENTS IN
  ipl_image_type* input_img_ptr        Points to the input image
  ipl_rect_type* crop                  Cropping parameter

ARGUMENTS OUT
  ipl_image_type* output_img_ptr       Points to the output image

RETURN VALUE
  Status

SIDE EFFECTS
  Alters the output image buffer with the cropped/converted frame.

==========================================================================*/
extern ipl_status_type qtv_ipl_convertAndCrop_YCbCr420ToRGB
(
  ipl_image_type* input_img_ptr,        /* Points to the input image    */
  ipl_image_type* output_img_ptr,       /* Points to the output image   */
  ipl_rect_type* crop                   /* Crop config                  */
);

#endif /* end of QTV_IPL_API_H */
