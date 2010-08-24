/*===========================================================================

              qtv_ipl_rotation_convert.c

DESCRIPTION
  This file contains the the functions for rotation and color conversion.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Some of the tables need initialization before use.

  Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is
  regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

$Header: //source/qcom/qct/multimedia/qtv/utils/ipl/main/latest/src/qtv_ipl_rotation_convert.c#7 $
$DateTime: 2008/05/08 13:55:54 $
$Change: 656417 $
===========================================================================*/
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_ipl.h"

LOCAL boolean qtv_ipl_initialized = FALSE;

LOCAL int32 qtv_ipl_roundFloatVal(float valToRound);

/*===========================================================================

FUNCTION QTV_IPL_INIT()

DESCRIPTION
This function will create intermediate values which are used in YCB to RGB
565 conversion. The functions are self explanatory and the initiliazation is
is to populate the values before its used.

DEPENDENCIES
None

ARGUMENTS IN
None

RETURN VALUE
Status - this not used right now. But it is kept to account for possible
errors when we move this to heap.

SIDE EFFECTS
None

===========================================================================*/
LOCAL ipl_status_type   qtv_ipl_init
(
  void
)
{
  int32 i = 0;
  float Cr2R = 1.57489;
  float Cb2G = 0.18744;
  float Cr2G = 0.46826;
  float Cb2B = 1.8554;
  float  origVal;
/*------------------------------------------------------------------------*/


  /*------------------------------------------------------------------------
      If already intialized bail out returning success

  ------------------------------------------------------------------------*/
  if(qtv_ipl_initialized)
  {
    return( IPL_SUCCESS );
  }

  /*------------------------------------------------------------------------

     Explanation for the magic numbers used in the YCbCr to RGB conversion.

      Here is the matrix represenation for the transformation of YCbCr to
      RGB values.

     | R  |       | 1     Cb2R    Cr2R  |     | Y  |          | Cb2R+Cr2R |
     | G  |   =   | 1     Cb2G    Cr2G  |  x  | Cb | - 128 X  | Cb2G+Cr2G |
     | B  |       | 1     Cb2B    Cr2b  |     | Cr |          | Cb2B+Cr2b |


      where X denotes matrix multiplication. These values are
      derived from the coeffeicents used in the earlier IPL.

    The value of Cb2R = 4.88828 * 10 exp -4 so this is totally igonord
                        given the range and quantization on these values.
    The value of Cr2B = 7.3242 * 10 exp -4  so this is totally igonord
                        given the range and quantization on these values.

   The last values are condensed into appropriate constants.
  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
   this creates thse function for values 0 - 255 for cr and cb.
   The output values are kept in 32 bit values
   -----------------------------------------------------------------------*/
  for ( i = 0; i <= 0xff; i++ )
  {

    /*----------------------------------------------------------------------
    take the conversion routine for the RGB values
    ----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
         conversion for R value

         The value -201.586 is = 128 * (Cr2R + Cb2G)
     ---------------------------------------------------------------------*/
    /*lint –e736 */
    origVal =   ( (float) (i) * Cr2R )  - 201.586;
    /*lint +e736 */

    qtv_Cr2RTable[i] = (int16) qtv_ipl_roundFloatVal( origVal);

    /*----------------------------------------------------------------------
       conversion for Cr to G

       The value -83.9296 = 128 * (Cr2g + Cb2G)
   -----------------------------------------------------------------------*/
    origVal = ( (float) (i) * Cr2G )  - 83.9296;
    qtv_Cr2GTable[i] = (int16) qtv_ipl_roundFloatVal( origVal);

    /*----------------------------------------------------------------------
        conversion for Cb2G
    ----------------------------------------------------------------------*/
    origVal = (float) (i) * Cb2G;
    qtv_Cb2GTable[i] = (int16) qtv_ipl_roundFloatVal( origVal);

    /*----------------------------------------------------------------------
         conversion for Cb to B

         The value -237.491 = 128 ( Cr2B + Cb2B)
    ----------------------------------------------------------------------*/
    origVal = ( (float) (i) * Cb2B )  - 237.491;
    qtv_Cb2BTable[i]=  (int16) qtv_ipl_roundFloatVal( origVal); 

  } /* end of loop i */


  /*------------------------------------------------------------------------
      Create multiplication for 3 / 4.
  ------------------------------------------------------------------------*/
  for( i = - 255; i <= 255; i++ )
  {
    qtv_multab3by4[i] = ( i   * 3  + 2) / 4;
  } /* end of for loop */

  /*------------------------------------------------------------------------
     flag the module initialization
  ------------------------------------------------------------------------*/
  qtv_ipl_initialized = TRUE;

  return( IPL_SUCCESS );

} /* end of function qtv_ipl_init */

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
ipl_status_type qtv_ipl_convertCropAndRot90_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
)
{
  register unsigned char* inputImgPtr;
  register uint16* outputImgPtr;
  register uint32 out, out2, out3, out4;
  register uint8 cb,cr;
  register int32 lumaa1, lumaa2, lumaa3, lumaa4;
  register int32 r;
  register uint16 *rTable;
  register uint16 *gTable;
  register uint16 *bTable;
  register uint32 input_row_size;
  uint32 rowSize;
  uint8 *cb_ptr;
  uint32 rowInc = 0, destInc, cr_offset, src_index;
  uint32 row, col;
  uint32 render_col_size;
  uint32 render_row_size;
/*------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
      Call the init routine. Mulitiple initializations does not hurt.
  ------------------------------------------------------------------------*/
  if ( qtv_ipl_init() != IPL_SUCCESS )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot90_YCbC420ToRGB:: /\
            Could not initialize Qtv_IPL lookup tables");
    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      NULL check the bare minimum parameters required
   -----------------------------------------------------------------------*/
  if ( !crop || !i_img_ptr || !o_img_ptr )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL crop/input/output ptr");
    return( IPL_FAILURE );
  }
  else
  {
    /*----------------------------------------------------------------------
        See if images have actual buffers
     ---------------------------------------------------------------------*/

    if ( !i_img_ptr->imgPtr || !o_img_ptr->imgPtr )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL input/output image ptr"); 
      return( IPL_FAILURE );
    }
  }

  /*------------------------------------------------------------------------
          Make sure cropping parameters are ok
  -----------------------------------------------------------------------*/
  if ( ( (crop->x + crop->dx) > o_img_ptr->dx ) ||
    ( (crop->y + crop->dy) > o_img_ptr->dy ) )
  {
    /*----------------------------------------------------------------------
             Invalid dimensions set
     ---------------------------------------------------------------------*/
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->x = %d, /\
                   crop->dx= %d, o_img_ptr->dx = %d", crop->x,
                   crop->dx, o_img_ptr->dx);

    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->y = %d, /\
                   crop->dy= %d, o_img_ptr->dy = %d", crop->y,
                   crop->dy, o_img_ptr->dy);

    return(IPL_FAILURE);
  }

  /*------------------------------------------------------------------------
    Warn the user and bail out if the cropped O/P have more pixels than
    the actual no of input pixels.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dy < crop->dx) || (i_img_ptr->dx < crop->dy ) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect crop setting  crop->dx = %d, /\
                 i_img_ptr->dx = %d", crop->dx,
                 i_img_ptr->dx);

    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect crop setting crop->dy = %d, /\
                 i_img_ptr->dy = %d", crop->dy,
                 i_img_ptr->dy);

    return( IPL_FAILURE );
  }

  /*-----------------------------------------------------------------------
    For the kind of packing used in YUV420 array we need even no of cols
    for sure. For cases involoving rotation rows and cols will change and
    it is best to check for even rows and cols before proceeding.
    Supporting these will involve more complicated routines.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dx & 0x1) || (i_img_ptr->dy & 0x1) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot90_YCbC420ToRGB failed i_img_ptr->dx = %x, /\
           i_img_ptr->dy  = %x", (uint32) i_img_ptr->dx,
           (uint32)i_img_ptr->dy);

    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
    Current scheme will truncate it to lower even. Otherwise the
    alogorithms will become unnecessarily complex.
  -----------------------------------------------------------------------*/
  if( (crop->dx & 0x1) || (crop->dy & 0x1) )
  {
    /*----------------------------------------------------------------------
      Warn the user strongly
    ----------------------------------------------------------------------*/
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Proceeding with /\
           less than acceptable cropping params crop->dx = %x, /\
           crop->dy  = %x will like them both even ", (uint32) crop->dx,
           (uint32)crop->dy);

  }


  /*------------------------------------------------------------------------
      If crop coloumns are more than what the actual row size of input
      image return error
  ------------------------------------------------------------------------*/
  if ( i_img_ptr->dy < crop->dx )
  {
    /*----------------------------------------------------------------------
               Invalid settings
    ----------------------------------------------------------------------*/
    return(IPL_FAILURE);
  }


  inputImgPtr = i_img_ptr->imgPtr;
  outputImgPtr = (uint16 *) o_img_ptr->imgPtr;
  input_row_size = i_img_ptr->dx;
  rowSize = o_img_ptr->dx;
  render_col_size = (i_img_ptr->dy < crop->dx) ? i_img_ptr->dy : crop->dx;
  render_row_size = (i_img_ptr->dx < crop->dy) ? i_img_ptr->dx : crop->dy;

  /*------------------------------------------------------------------------
      In this scheme odd no of cols in a row will not work.
      I am truncating it to nearest lower even
  ------------------------------------------------------------------------*/
  render_row_size &=   (~0x1);
  render_col_size &=   (~0x1);


  /*--------------------------------------------------------------------
        initialize the conversion with RGB 565 packing table
  --------------------------------------------------------------------*/
  rTable = &(qtv_ipl_r5xx[0]);
  gTable = &(qtv_ipl_gx6x[0] );
  bTable = &(qtv_ipl_bxx5[0] );

  /*------------------------------------------------------------------------
    90 degree rotation needs starting the image from the same side  corner
    where the image starts and wroking upwards. Same with a 2 col offset
    and so on.

     inputImgPtr
     |
     |
     ^__________________________
     |                         |
   ^ |                         |
   | |                         |
   | |                         |
   | |_________________________|
     ^
     | ----> Go this way for rows
     |
   Offset it into the array to start
   picking Y values from here backwards on the same col
   as shown in the arrow leftside

   When offsetted by src_index ( = end of frame - row size )
      the inputImgPtr takes us to this location.
  ------------------------------------------------------------------------*/
  src_index =  i_img_ptr->dx * ( i_img_ptr->dy - 1);

  /*------------------------------------------------------------------------
      set the starting position of input image pointer properly offset
  ------------------------------------------------------------------------*/
  inputImgPtr  += src_index;

  /*------------------------------------------------------------------------
      Since we swapped rows for cols at the end of every row iteration
      we need to offset the o/p for the unrendered pixels. We need to
      subtract cropped colomn size. Addition here is byte addressed.
  ------------------------------------------------------------------------*/
  destInc = (o_img_ptr->dx - crop->dx) * 2;

  /*------------------------------------------------------------------------
      rowInc is slightly misleading. we are talking wrt output vector
      lets say. I am subtracting no of cols in o/p from no of rows
      in input. All because of 90 degree rotation. We go 2 row at a time.
  ------------------------------------------------------------------------*/
  rowInc = (i_img_ptr->dy - crop->dx );

  /*------------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

  |   (dx*dy) no of Y values = cr_offset )   |1/4 Yvals      |1/4 Yvals    |
  |------------------------------------------|---------------|-------------|
  |------------------------------------------|---------------|-------------|
        Y values                                  Cb Vals       Cr Vals
        src_index is actually one row off interms of cb and cr plane. This
        is because every odd follows the previous cb\cr vector. So to offset
        correctly into CR plane we need subtract one more row from
        src_index which accounts for (src_index - i_img_ptr->dx) in the
        following calculation.
        _____________________________________________
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |------> Cb/Cr plane offset row before last  |
        |____________________________________________|
        ^
        |
        |
       Y plane offset
  ------------------------------------------------------------------------*/
  cb_ptr = i_img_ptr->imgPtr +
                   ( i_img_ptr->dx * i_img_ptr->dy  ) +
                   ( (src_index - i_img_ptr->dx)  >> 2);

  /*------------------------------------------------------------------------
      This assumes even no of coloumns. Need to discuss
  ------------------------------------------------------------------------*/
  cr_offset = ( i_img_ptr->dx * i_img_ptr->dy  ) >> 2 /* /4 */;

  /*------------------------------------------------------------------------
    Offset into output buffer to do relocation; half ward addressed
    addition
  ------------------------------------------------------------------------*/
  outputImgPtr += crop->x + o_img_ptr->dx*crop->y;

  /*------------------------------------------------------------------------
     We process 2 cols at a time in this encoding. So we need half the
     number of iterations. Please note that row and col iterations are
     swapped.
  ------------------------------------------------------------------------*/
  for ( row = render_row_size /2 ; row; row-- )
  {
    /*----------------------------------------------------------------------
        We process 2 rows at a time we need half the number of coloumns
        iteratios.
    ----------------------------------------------------------------------*/
    for ( col = render_col_size / 2; col; col-- )
    {

      /*--------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

          |   (dx*dy) of Y values )        |1/4 Yvals|1/4 Yvals |
          |--------------------------------|---------|----------|
          |--------------------------------|---------|----------|
                Y values                     Cb Vals    Cr Vals
      --------------------------------------------------------------------*/


      /*--------------------------------------------------------------------
          Take Luma of all four pixels. The Lumma Packing is as shown
          below

          |Y2|Y4|             (this processing)   |RGB1|RGB2|
                ---Cb, Cr  ------------------->   |RGB3|RGB4|
          |Y1|Y3|

          As we can see above this small 2 by 2 vector already 90 degree
          transposed by this one assignment. Cb and Cr remains the same.
          One less worry.
      --------------------------------------------------------------------*/
      lumaa1 = *( inputImgPtr );
      lumaa3 = *( inputImgPtr + 1 );

      /*--------------------------------------------------------------------
            Go back by one row
      --------------------------------------------------------------------*/
      inputImgPtr -= input_row_size;
      lumaa2 = *( inputImgPtr );
      lumaa4 = *( inputImgPtr + 1 );

      /*--------------------------------------------------------------------
            Go back by one row so that next iteration works perfect
      --------------------------------------------------------------------*/
      inputImgPtr -= input_row_size;


      /*--------------------------------------------------------------------
          Cr is same for all the 4  Yavals
      --------------------------------------------------------------------*/
      cb = *(cb_ptr );

      /*--------------------------------------------------------------------
          Get the Cr for all 4 Y vals
      --------------------------------------------------------------------*/
      cr = *(cb_ptr + cr_offset);

      /*--------------------------------------------------------------------
            We need to decrease the cb by row size /2 to get the next 4
            pixel cb/cr pair
      --------------------------------------------------------------------*/
      cb_ptr -= input_row_size >> 1;

      /*--------------------------------------------------------------------
          The following is macro which replces a repetitive
          block of code which takes 4 lummas and a Cr/Cb combination
          to convert it to a RGB flavor with out and out 2 in two words.
          cannot be used for RGB666. because 2 pixels cant fit in one
          word
                                              (out)
          Y | Y                       RGB444/565 | RGB444/565
                   + CB/CR ----->
          Y | Y                       RGB444/565 | RGB444/565
                                             (out2)
      --------------------------------------------------------------------*/
      QTV_IPL_CONVERT_YCBCR420TORGB_HALFWORD( lumaa1, lumaa2, lumaa3,
                                              lumaa4, cb, cr,  r, out,
                                              out2, out3, out4,
                                              rTable, gTable, bTable );

      *outputImgPtr = (uint16) out;
      *(outputImgPtr + 1) = (uint16) out2;
      *(outputImgPtr++ + rowSize) = (uint16) out3;
      *(outputImgPtr++ + rowSize) = (uint16) out4;
    }

    /*----------------------------------------------------------------------
         Need to add  number of coulumns * 2 /4 --> rowSize /2
         which is same as rowSize >> 1; destInc accounts for the
         size mismatch b/w cropped image and output frame size
    ----------------------------------------------------------------------*/
    outputImgPtr =  (uint16 *) ( (uint8 *) outputImgPtr +
                      (rowSize << 1) + destInc );


    /*----------------------------------------------------------------------
        reinitialize the input and cb pointer to start on next coloumn
        last row

        Yn    | Yn + 1  | Yn + 4 | Yn + 5
        Yn+ 2 | Yn+3    | Yn + 6 | Yn + 7

        By doing this we are getting Yn + 7 which will be lumaa1 for
        our first pixel of our next iteration. Last +2 takes us from
        Yn+2 to Yn+7

        src_index +  input_row_size + 2 -
                   (rowInc * input_row_size) -  accounts for rows not
                                                rendered for cropping
    ----------------------------------------------------------------------*/
    inputImgPtr += src_index /* take to the earlier start */ +
                  2 /* advnce to next */ -
              ( ( (rowInc - 1) * input_row_size ) );/* account for cropping */

    cb_ptr += ((src_index + input_row_size) >> 2) + 1 -
              ( ( rowInc * input_row_size ) >> 2 );
  }

  return( IPL_SUCCESS );

} /* end of function qtv_ipl_convertCropAndRot90_YCbC420ToRGB */

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
ipl_status_type qtv_ipl_convertCropAndRot180_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
)
{
  register unsigned char* inputImgPtr;
  register uint16* outputImgPtr;
  register uint32 out, out2, out3, out4;
  register uint8 cb,cr;
  register int32 lumaa1, lumaa2, lumaa3, lumaa4;
  register int32 r;
  register uint16 *rTable;
  register uint16 *gTable;
  register uint16 *bTable;
  register uint32 input_row_size;
  uint32 rowSize;
  uint8 *cr_ptr;
  uint32 rowInc=0, dest_index, destInc, cb_offset;
  uint32 row, col;
/*------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
      Call the init routine. Mulitiple initializations does not hurt.
  ------------------------------------------------------------------------*/
  if ( qtv_ipl_init() != IPL_SUCCESS )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot180_YCbC420ToRGB:: /\
            Could not initialize Qtv_IPL lookup tables");
    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      NULL check the bare minimum parameters required
   -----------------------------------------------------------------------*/
  if ( !crop || !i_img_ptr || !o_img_ptr )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL crop/input/output ptr");
    return( IPL_FAILURE );
  }
  else
  {
    /*----------------------------------------------------------------------
        See if images have actual buffers
     ---------------------------------------------------------------------*/

    if ( !i_img_ptr->imgPtr || !o_img_ptr->imgPtr )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL input/output image ptr"); 
      return( IPL_FAILURE );
    }
  }

  /*------------------------------------------------------------------------
          Make sure cropping parameters are ok
  -----------------------------------------------------------------------*/
  if ( ( (crop->x + crop->dx) > o_img_ptr->dx ) ||
    ( (crop->y + crop->dy) > o_img_ptr->dy ) )
  {
    /*----------------------------------------------------------------------
             Invalid dimensions set
     ---------------------------------------------------------------------*/
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->x = %d, /\
                   crop->dx= %d, o_img_ptr->dx = %d", crop->x,
                   crop->dx, o_img_ptr->dx);

    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->y = %d, /\
                   crop->dy= %d, o_img_ptr->dy = %d", crop->y,
                   crop->dy, o_img_ptr->dy);

    return(IPL_FAILURE);
  }

  /*------------------------------------------------------------------------
    Warn the user and bail out if the cropped O/P have more pixels than
    the actual no of input pixels.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dx < crop->dx) || (i_img_ptr->dy < crop->dy ) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect  crop setting  crop->dx = %d, /\
                   i_img_ptr->dx = %d", crop->dx,
                   i_img_ptr->dx);

    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect crop setting crop->dy = %d, /\
                 i_img_ptr->dy = %d", crop->dy,
                 i_img_ptr->dy);
    return( IPL_FAILURE );
  }

  /*-----------------------------------------------------------------------
      For the kind of packing used in YUV420 array we need even no of cols
      for sure. For cases involoving rotation rows and cols will change and
      it is best to check for even rows and cols before proceeding.
      Supporting these will involve more complicated routines.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dx & 0x1) || (i_img_ptr->dy & 0x1) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot180_YCbC420ToRGB failed i_img_ptr->dx = %x, /\
               i_img_ptr->dy  = %x", (uint32) i_img_ptr->dx,
               (uint32)i_img_ptr->dy);

    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      Current scheme will truncate it to lower even. Otherwise the
      alogorithms will become unnecessarily complex.
  -----------------------------------------------------------------------*/
  if( (crop->dx & 0x1) || (crop->dy & 0x1) )
  {
    /*----------------------------------------------------------------------
          Warn the user strongly
    ----------------------------------------------------------------------*/
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Proceeding with /\
               less than acceptable cropping params crop->dx = %x, /\
               crop->dy  = %x will like them both even ", (uint32) crop->dx,
               (uint32)crop->dy);

  }

  inputImgPtr = i_img_ptr->imgPtr;
  outputImgPtr = (uint16 *) o_img_ptr->imgPtr;
  input_row_size = i_img_ptr->dx;
  rowSize = o_img_ptr->dx;

  /*------------------------------------------------------------------------
      180 degree rotation needs starting the image from the extreme corner
      so lets add the whole frame size into the input image ptr and then
      starts decrementing rows

      inputImgPtr
      |
      |
      ^__________________________
      |                         |
      |                         |
      |                         |
      |                         |
      |_______________________|_|
                              |
                              |
                               Offset it into the array to start
                               picking Y values from here backwards
  ------------------------------------------------------------------------*/
  inputImgPtr += i_img_ptr->dx * i_img_ptr->dy  - 1;



  /*------------------------------------------------------------------------
     In the output RGB565/444 array each pixel is 2 bytes and so
     we offset into the buffer the same way
  ------------------------------------------------------------------------*/
  dest_index = (crop->x + o_img_ptr->dx * crop->y) << 1 /* * 2 */;

  /*--------------------------------------------------------------------
        initialize the conversion with RGB 565 packing table
  --------------------------------------------------------------------*/
  rTable = &(qtv_ipl_r5xx[0]);
  gTable = &(qtv_ipl_gx6x[0] );
  bTable = &(qtv_ipl_bxx5[0] );

  outputImgPtr = (uint16 *) ((uint32)outputImgPtr +  dest_index);

  /*------------------------------------------------------------------------
      Destination should increment 2 bytes per pixel for RGB565/444
      encoding.
  ------------------------------------------------------------------------*/
  destInc = ( o_img_ptr->dx - crop->dx) * 2;

  /*------------------------------------------------------------------------
      In this case the Y values are a byte array covering the entier image.

                    crop->dx
              <---------------------->

      cropped area
      delta  in dx
      ______|___________________________
      |________| ______________________|
      |        |                       |
      |        |                       |
      |        |                       |
      |        |                       |
      |        |                       |
      |________|_______________________|
      <-------------------------------->
            input_img->dx
      We are going backwards thats why this is decerementing
  ------------------------------------------------------------------------*/
  rowInc = (i_img_ptr->dx - crop->dx );


  /*------------------------------------------------------------------------
          cr_ptr with respect to  YCbCr420 packing

  |   (dx*dy) no of Y values = cr_offset )   |1/4 Yvals      |1/4 Yvals    |
  |------------------------------------------|---------------|-------------|
  |------------------------------------------|---------------|-------------|
        Y values                                  Cb Vals    Cr Vals       |
                                                                           |
                                                                           ^
                                                                       cr_ptr
                                                                      is here
  ------------------------------------------------------------------------*/
  cr_ptr = i_img_ptr->imgPtr +
         ( i_img_ptr->dx * i_img_ptr->dy  ) /* y array */ +
       (  ( i_img_ptr->dx * i_img_ptr->dy  ) >> 1 )/* cb array * 2 */
        - 1 /* to fetch the last element */;

  /*------------------------------------------------------------------------
      This assumes even no of coloumns. Need to discuss
  ------------------------------------------------------------------------*/
  cb_offset =  (i_img_ptr->dx * i_img_ptr->dy  ) >> 2;


  /*----------------------------------------------------------------------
      We process 2 rows at a time in this encoding. So we need half the
      number of iterations
  ----------------------------------------------------------------------*/
  for ( row = crop->dy/2; row; row-- )
  {
    /*--------------------------------------------------------------------
        We process 2 cols at a time we need half the number of coloumns
        iteratios.
    --------------------------------------------------------------------*/
    for ( col = crop->dx/2; col; col-- )
    {

      /*------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

          |   (dx*dy) of Y values )        |1/4 Yvals|1/4 Yvals |
          |--------------------------------|---------|----------|
          |--------------------------------|---------|----------|
                Y values                     Cb Vals    Cr Vals
      ------------------------------------------------------------------*/


      /*------------------------------------------------------------------
          Take Luma of all four pixels. The Lumma Packing is as shown
          below

          |Y4|Y3|             (this processing)   |RGB1|RGB2|
                ---Cb, Cr  ------------------->   |RGB3|RGB4|
          |Y2|Y1|
      ------------------------------------------------------------------*/
      lumaa1 = *inputImgPtr;  /* corresponds to Y1 in the pic */
      lumaa2 = *(inputImgPtr - 1);  /* corresponds to Y2 in the pic */
      lumaa3 = *(inputImgPtr-- - input_row_size);
      lumaa4 = *(inputImgPtr-- - input_row_size);

      /*------------------------------------------------------------------
          Cr is same for all the 4  Yavals
      ------------------------------------------------------------------*/
      cr = *(cr_ptr);

      /*------------------------------------------------------------------
          Get the Cr for all 4 Y vals
      ------------------------------------------------------------------*/
      cb = *(cr_ptr - cb_offset);

      /*------------------------------------------------------------------
          We got both CB and Cr for this iteration we decrement cr_ptr.
          If you look at the pics you can see that they both (cb anc cr)
          move in tandem
      ------------------------------------------------------------------*/
      --cr_ptr;

      /*------------------------------------------------------------------
          The following is macro which replces a repetitive
          block of code which takes 4 lummas and a Cr/Cb combination
          to convert it to a RGB flavor with out and out 2 in two words.
          cannot be used for RGB666. because 2 pixels cant fit in one
          word
                                              (out)
          Y | Y                       RGB444/565 | RGB444/565
                   + CB/CR ----->
          Y | Y                       RGB444/565 | RGB444/565
                                             (out2)
      ------------------------------------------------------------------*/
      QTV_IPL_CONVERT_YCBCR420TORGB_HALFWORD( lumaa1, lumaa2, lumaa3,
                                              lumaa4, cb, cr,  r, out,
                                              out2, out3, out4,
                                              rTable, gTable, bTable );

      *outputImgPtr = (uint16) out;
      *(outputImgPtr + 1) = (uint16) out2;
      *(outputImgPtr++ + rowSize) = (uint16) out3;
      *(outputImgPtr++ + rowSize) = (uint16) out4;
    }

    /*--------------------------------------------------------------------
        Need to add  number of coulumns * 2 /4 --> rowSize /2
        which is same as rowSize >> 1;
    --------------------------------------------------------------------*/
    outputImgPtr =  (uint16 *) ( (uint8 *) outputImgPtr +
                      (rowSize << 1)  + destInc ) ;

    /*--------------------------------------------------------------------
      |<------- input_row_size  ----->|
      _______________________________
      |                              |
      |                              |
      |                              |
      |                              |
      |_____________________________X|-------> Where this operation
      |______________________________|         will take us
      |________|_____________________|
               |
               |
               ^
               inputImgPtr
     |-- rowInc-|
    --------------------------------------------------------------------*/
    inputImgPtr -= (uint32) (rowInc + input_row_size);

    /*--------------------------------------------------------------------
      Remember each four pixels has same CB/CR combination. So we need
      to add number of pixels offset divided by 4

      YY
      -----------------maps to one cr/cb --> cr/cb
      YY

    --------------------------------------------------------------------*/
    cr_ptr -= (rowInc) >> 1 /* divided by 2 */;

  }

  return( IPL_SUCCESS );

} /* end of function qtv_ipl_convertCropAndRot180_YCbC420ToRGB */

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
ipl_status_type qtv_ipl_convertCropAndRot270_YCbC420ToRGB
(
  ipl_image_type* i_img_ptr,    /* Input Image Pointer      */
  ipl_image_type* o_img_ptr,    /* Output Image Pointer      */
  ipl_rect_type* crop          /* cropping params */
)
{
  register unsigned char* inputImgPtr;
  register uint16* outputImgPtr;
  register uint32 out, out2, out3, out4;
  register uint8 cb,cr;
  register int32 lumaa1, lumaa2, lumaa3, lumaa4;
  register int32 r;
  register uint16 *rTable;
  register uint16 *gTable;
  register uint16 *bTable;
  register uint32 input_row_size;
  uint32 rowSize;
  uint8 *cb_ptr;
  uint32 destInc, cr_offset, src_index, cb_offset;
  uint32 row, col;
  uint32 render_col_size;
  uint32 render_row_size;
/*------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
      Call the init routine. Mulitiple initializations does not hurt.
  ------------------------------------------------------------------------*/
  if ( qtv_ipl_init() != IPL_SUCCESS )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot270_YCbC420ToRGB:: /\
            Could not initialize Qtv_IPL lookup tables");
    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      NULL check the bare minimum parameters required
   -----------------------------------------------------------------------*/
  if ( !crop || !i_img_ptr || !o_img_ptr )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL crop/input/output ptr");
    return( IPL_FAILURE );
  }
  else
  {
    /*----------------------------------------------------------------------
        See if images have actual buffers
     ---------------------------------------------------------------------*/

    if ( !i_img_ptr->imgPtr || !o_img_ptr->imgPtr )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL input/output image ptr"); 
      return( IPL_FAILURE );
    }
  }

  /*------------------------------------------------------------------------
          Make sure cropping parameters are ok
  -----------------------------------------------------------------------*/
  if ( ( (crop->x + crop->dx) > o_img_ptr->dx ) ||
    ( (crop->y + crop->dy) > o_img_ptr->dy ) )
  {
    /*----------------------------------------------------------------------
             Invalid dimensions set
     ---------------------------------------------------------------------*/
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->x = %d, /\
                   crop->dx= %d, o_img_ptr->dx = %d", crop->x,
                   crop->dx, o_img_ptr->dx);

    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->y = %d, /\
                   crop->dy= %d, o_img_ptr->dy = %d", crop->y,
                   crop->dy, o_img_ptr->dy);

    return(IPL_FAILURE);
  }

  /*------------------------------------------------------------------------
      Warn the user and bail out if the cropped O/P have more pixels than
      the actual no of input pixels.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dy < crop->dx) || (i_img_ptr->dx < crop->dy ) )
  {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                      perfect crop setting  crop->dx = %d, /\
                     i_img_ptr->dx = %d", crop->dx,
                     i_img_ptr->dx);

      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                      perfect crop setting crop->dy = %d, /\
                     i_img_ptr->dy = %d", crop->dy,
                     i_img_ptr->dy);

      return( IPL_FAILURE );
  }

  /*-----------------------------------------------------------------------
    For the kind of packing used in YUV420 array we need even no of cols
    for sure. For cases involoving rotation rows and cols will change and
    it is best to check for even rows and cols before proceeding.
    Supporting these will involve more complicated routines.
  ------------------------------------------------------------------------*/
  if( (i_img_ptr->dx & 0x1) || (i_img_ptr->dy & 0x1) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertCropAndRot270_YCbC420ToRGB failed i_img_ptr->dx = %x, /\
           i_img_ptr->dy  = %x", (uint32) i_img_ptr->dx,
           (uint32)i_img_ptr->dy);

    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
    Current scheme will truncate it to lower even. Otherwise the
    alogorithms will become unnecessarily complex.
  -----------------------------------------------------------------------*/
  if( (crop->dx & 0x1) || (crop->dy & 0x1) )
  {
    /*----------------------------------------------------------------------
      Warn the user strongly
    ----------------------------------------------------------------------*/
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Proceeding with /\
           less than acceptable cropping params crop->dx = %x, /\
           crop->dy  = %x will like them both even ", (uint32) crop->dx,
           (uint32)crop->dy);

  }

  /*------------------------------------------------------------------------
      If crop coloumns are more than what the actual row size of input
      image return error
  ------------------------------------------------------------------------*/
  if ( i_img_ptr->dy < crop->dx )
  {
    /*----------------------------------------------------------------------
               Invalid settings
    ----------------------------------------------------------------------*/
    return(IPL_FAILURE);
  }


  inputImgPtr = i_img_ptr->imgPtr;
  outputImgPtr = (uint16 *) o_img_ptr->imgPtr;
  input_row_size = i_img_ptr->dx;
  rowSize = o_img_ptr->dx;
  render_col_size = (i_img_ptr->dy < crop->dx) ? i_img_ptr->dy : crop->dx;
  render_row_size = (i_img_ptr->dx < crop->dy) ? i_img_ptr->dx : crop->dy;

  /*------------------------------------------------------------------------
      In this scheme odd no of cols in a row will not work.
      I am truncating it to nearest lower even
  ------------------------------------------------------------------------*/
  render_row_size &=   (~0x1);
  render_col_size &=   (~0x1);


  /*--------------------------------------------------------------------
        initialize the conversion with RGB 565 packing table
  --------------------------------------------------------------------*/
  rTable = &(qtv_ipl_r5xx[0]);
  gTable = &(qtv_ipl_gx6x[0] );
  bTable = &(qtv_ipl_bxx5[0] );

  /*------------------------------------------------------------------------
    270 degree rotation needs starting the image from the same side right
    corner from where the image starts and wroking downwards.
    - with a 2 col offset and so on.

      inputImgPtr
     |
     |
     ^__________________________
     |                         |  |
     |                         |  |
     |                         |  |
     |                         |  v  col increment
     |_________________________|
        <-----------------------
         row increment
  ------------------------------------------------------------------------*/
  src_index =  i_img_ptr->dx;

  /*------------------------------------------------------------------------
      set the starting position of input image pointer properly offset
  ------------------------------------------------------------------------*/
  inputImgPtr  += src_index - 1;

  /*------------------------------------------------------------------------
      Since we swapped rows for cols at the end of every row iteration
      we need to offset the o/p for the unrendered pixels. We need to
      subtract cropped colomn size. Addition here is byte addressed.
  ------------------------------------------------------------------------*/
  destInc = (o_img_ptr->dx - crop->dx) * 2;


  /*------------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

  |   (dx*dy) no of Y values = cr_offset )   |   1/4 Yvals   |1/4 Yvals    |
  |------------------------------------------|---------------|-------------|
  |------------------------------------------|---------------|-------------|
        Y values                                  Cb Vals       Cr Vals
        src_index is actually one row off interms of cb and cr plane. This
        is because every odd follows the previous cb\cr vector.
        _____________________________________________
        |                                  <--------||
        |                               Yplane/Cb    |
        |                               offset start |
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |                                            |
        |____________________________________________|

  ------------------------------------------------------------------------*/
  cb_offset = ( i_img_ptr->dx * i_img_ptr->dy  );
  cb_ptr = i_img_ptr->imgPtr +
                    + cb_offset +
                   ( src_index  >> 1) - 1 /* end of prev row */;

  /*------------------------------------------------------------------------
      This assumes even no of coloumns. Need to discuss
  ------------------------------------------------------------------------*/
  cr_offset = ( i_img_ptr->dx * i_img_ptr->dy  ) >> 2 /* /4 */;

  /*------------------------------------------------------------------------
    Offset into output buffer to do relocation; half ward addressed
    addition
  ------------------------------------------------------------------------*/
  outputImgPtr += crop->x + o_img_ptr->dx*crop->y;

  /*------------------------------------------------------------------------
     We process 2 cols at a time in this encoding. So we need half the
     number of iterations. Please note that row and col iterations are
     swapped.
  ------------------------------------------------------------------------*/
  for ( row = render_row_size /2 ; row; row-- )
  {
    /*----------------------------------------------------------------------
        We process 2 rows at a time we need half the number of coloumns
        iteratios.
    ----------------------------------------------------------------------*/
    for ( col = render_col_size / 2; col; col-- )
    {

      /*--------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

          |   (dx*dy) of Y values )        |1/4 Yvals|1/4 Yvals |
          |--------------------------------|---------|----------|
          |--------------------------------|---------|----------|
                Y values                     Cb Vals    Cr Vals
      --------------------------------------------------------------------*/


      /*--------------------------------------------------------------------
          Take Luma of all four pixels. Luma Extracion we do on the
          following lines of code assumes the following figure. Just by
          assuming the following geometry we get the 270 degree rotatin.
          You can see the tiny square changed to 90  degree
          anticlockwise now (  same 270 ).

          |Y3|Y1|             (this processing)   |RGB1|RGB2|
                ---Cb, Cr  ------------------->   |RGB3|RGB4|
          |Y4|Y2|

          As we can see above this small 2 by 2 vector already -90 degree
          transposed by this one assignment. Cb and Cr remains the same.
          One less worry.
      --------------------------------------------------------------------*/
      lumaa1 = *( inputImgPtr );
      lumaa3 = *( inputImgPtr - 1 );

      /*--------------------------------------------------------------------
            Go back by one row
      --------------------------------------------------------------------*/
      inputImgPtr += input_row_size;
      lumaa2 = *( inputImgPtr );
      lumaa4 = *( inputImgPtr - 1 );

      /*--------------------------------------------------------------------
            Go back by one row so that next iteration works perfect
      --------------------------------------------------------------------*/
      inputImgPtr += input_row_size;


      /*--------------------------------------------------------------------
          Cr is same for all the 4  Yavals
      --------------------------------------------------------------------*/
      cb = *(cb_ptr );

      /*--------------------------------------------------------------------
          Get the Cr for all 4 Y vals
      --------------------------------------------------------------------*/
      cr = *(cb_ptr + cr_offset);

      /*--------------------------------------------------------------------
            We need to decrease the cb by row size /2 to get the next 4
            pixel cb/cr pair
      --------------------------------------------------------------------*/
      cb_ptr += input_row_size >> 1;

      /*--------------------------------------------------------------------
          The following is macro which replces a repetitive
          block of code which takes 4 lummas and a Cr/Cb combination
          to convert it to a RGB flavor with out and out 2 out3 and out4
          in four half words.
                                              (out)
          Y | Y                       RGB444/565 | RGB444/565
                   + CB/CR ----->
          Y | Y                       RGB444/565 | RGB444/565
                                             (out2)
      --------------------------------------------------------------------*/
      QTV_IPL_CONVERT_YCBCR420TORGB_HALFWORD( lumaa1, lumaa2, lumaa3,
                                              lumaa4, cb, cr,  r, out,
                                              out2, out3, out4,
                                              rTable, gTable, bTable );

      *outputImgPtr = (uint16) out;
      *(outputImgPtr + 1) = (uint16) out2;
      *(outputImgPtr++ + rowSize) = (uint16) out3;
      *(outputImgPtr++ + rowSize) = (uint16) out4;

    } /* end of col loop */

    /*----------------------------------------------------------------------
         Need to add  number of coulumns * 2 /4 --> rowSize /2
         which is same as rowSize >> 1; destInc accounts for the
         size mismatch b/w cropped image and output frame size
     ---------------------------------------------------------------------*/
    outputImgPtr =  (uint16 *) ( (uint8 *) outputImgPtr +
                      (rowSize << 1) + destInc );


    /*----------------------------------------------------------------------
        reinitialize the input and cb pointer to start on next coloumn
        last row

        Yn    | Yn + 1  | Yn + 4 | Yn + 5
        Yn+ 2 | Yn+3    | Yn + 6 | Yn + 8

        If we started our first iteration on Yn + 5 doing this we
        are getting Yn + 1.

      Since we are always on the first row offsetted by two than the
      previous one.
    ----------------------------------------------------------------------*/
    src_index -= 2;
    inputImgPtr = i_img_ptr->imgPtr + src_index  - 1;

    cb_ptr = i_img_ptr->imgPtr + cb_offset +
                   ( src_index  >> 1) - 1 /* end of prev row */;


  } /* end of row loop */

  return( IPL_SUCCESS );

} /* end of function qtv_ipl_convertCropAndRot270_YCbC420ToRGB */

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
ipl_status_type qtv_ipl_convertAndCrop_YCbCr420ToRGB
(
  ipl_image_type* input_img_ptr,        /* Points to the input image    */
  ipl_image_type* output_img_ptr,       /* Points to the output image   */
  ipl_rect_type* crop                   /* Crop config                  */
)
{
  register unsigned char* inputImgPtr;
  register uint16* outputImgPtr;
  register uint32 out, out2, out3, out4;
  register uint8 cb,cr;
  register int32 lumaa1, lumaa2, lumaa3, lumaa4;
  register int32 r;
  register uint16 *rTable;
  register uint16 *gTable;
  register uint16 *bTable;
  uint32 input_row_size;
  uint32 rowSize;
  uint8 *cb_ptr;
  uint32 rowInc=0,dest_index,destInc;
  uint32 row, col, cr_offset;
/*------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
      Call the init routine. Mulitiple initializations does not hurt.
  ------------------------------------------------------------------------*/
  if ( qtv_ipl_init() != IPL_SUCCESS )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertAndCrop_YCbCr420ToRGB:: /\
            Could not initialize Qtv_IPL lookup tables");
    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      NULL check the bare minimum parameters required
   -----------------------------------------------------------------------*/
  if ( !crop || !input_img_ptr || !output_img_ptr )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL crop/input/output ptr");
    return( IPL_FAILURE );
  }
  else
  {
    /*----------------------------------------------------------------------
        See if images have actual buffers
     ---------------------------------------------------------------------*/

    if ( !input_img_ptr->imgPtr || !output_img_ptr->imgPtr )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL input/output image ptr"); 
      return( IPL_FAILURE );
    }
  }

  /*------------------------------------------------------------------------
          Make sure cropping parameters are ok
  -----------------------------------------------------------------------*/
  if ( ( (crop->x + crop->dx) > output_img_ptr->dx ) ||
    ( (crop->y + crop->dy) > output_img_ptr->dy ) )
  {
    /*----------------------------------------------------------------------
             Invalid dimensions set
     ---------------------------------------------------------------------*/
    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->x = %d, /\
                   crop->dx= %d, output_img_ptr->dx = %d", crop->x,
                   crop->dx, output_img_ptr->dx);

    QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid crop setting /\
                   crop->y = %d, /\
                   crop->dy= %d, output_img_ptr->dy = %d", crop->y,
                   crop->dy, output_img_ptr->dy);

    return(IPL_FAILURE);
  }

  /*------------------------------------------------------------------------
      Warn the user and bail out if the cropped O/P have more pixels than
      the actual no of input pixels.
  ------------------------------------------------------------------------*/
  if( (input_img_ptr->dx < crop->dx) || (input_img_ptr->dy < crop->dy ) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect  crop setting  crop->dx = %d, /\
                   input_img_ptr->dx = %d", crop->dx,
                   input_img_ptr->dx);

    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Not proceeding with less than /\
                  perfect crop setting crop->dy = %d, /\
                 input_img_ptr->dy = %d", crop->dy,
                 input_img_ptr->dy);
    return( IPL_FAILURE );
  }

  /*-----------------------------------------------------------------------
      For the kind of packing used in YUV420 array we need even no of cols
      for sure. For cases involoving rotation rows and cols will change and
      it is best to check for even rows and cols before proceeding.
      Supporting these will involve more complicated routines.
  ------------------------------------------------------------------------*/
  if( (input_img_ptr->dx & 0x1) || (input_img_ptr->dy & 0x1) )
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "qtv_ipl_convertAndCrop_YCbCr420ToRGB failed input_img_ptr->dx = %x, /\
               input_img_ptr->dy  = %x", (uint32) input_img_ptr->dx,
               (uint32)input_img_ptr->dy);

    return( IPL_FAILURE );
  }

  /*------------------------------------------------------------------------
      Current scheme will truncate it to lower even. Otherwise the
      alogorithms will become unnecessarily complex.
  -----------------------------------------------------------------------*/
  if( (crop->dx & 0x1) || (crop->dy & 0x1) )
  {
    /*----------------------------------------------------------------------
          Warn the user strongly
    ----------------------------------------------------------------------*/
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Proceeding with /\
               less than acceptable cropping params crop->dx = %x, /\
               crop->dy  = %x will like them both even ", (uint32) crop->dx,
               (uint32)crop->dy);

  }

  if ( input_img_ptr->dx < crop->dx )
  {
    /*--------------------------------------------------------------------
         Invalid settings
    --------------------------------------------------------------------*/
    return(IPL_FAILURE);
  }

  /* initialize parms after null checks */
  inputImgPtr = input_img_ptr->imgPtr;
  outputImgPtr = (uint16*) output_img_ptr->imgPtr;
  input_row_size = input_img_ptr->dx;
  rowSize = output_img_ptr->dx;

  /*------------------------------------------------------------------------
     In the output RGB565/444 array each pixel is 2 bytes and so
     we offset into the buffer the same way
  ------------------------------------------------------------------------*/
  dest_index = (crop->x + output_img_ptr->dx * crop->y) << 1 /* * 2 */;

  /*--------------------------------------------------------------------
        initialize the conversion with RGB 565 packing table
  --------------------------------------------------------------------*/
  rTable = &(qtv_ipl_r5xx[0]);
  gTable = &(qtv_ipl_gx6x[0] );
  bTable = &(qtv_ipl_bxx5[0] );

  /*------------------------------------------------------------------------
      Calculate the offset before we start stuffing in the O/P buffer
  ------------------------------------------------------------------------*/
  outputImgPtr = (uint16 *) ((uint32)outputImgPtr +  (uint32) dest_index);

  /*------------------------------------------------------------------
      Destination should increment 2 bytes per pixel for RGB565/444
      encoding.
  ------------------------------------------------------------------*/
  destInc = ( output_img_ptr->dx - crop->dx )  * 2;

  /*------------------------------------------------------------------------
      In this case the Y values are a byte array covering the entire image.

            input_img->dx
      <--------------------------------->
      _________________________________
      |_______________________|________|---------> rowInc = delta in dx
      |                       |        |
      |                       |        |
      |                       |        |
      |_______________________|________|
       <---------------------->
          crop->dx

       at the end of each row we need to avance the cropped pixels rowInc
  ------------------------------------------------------------------------*/
  rowInc = (input_img_ptr->dx - crop->dx );


  /*------------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

  |   (dx*dy) no of Y values = cr_offset )   |1/4 Yvals      |1/4 Yvals    |
  |------------------------------------------|---------------|-------------|
  |------------------------------------------|---------------|-------------|
              Y values                          Cb Vals          Cr Vals

        Following calculation takes us to the beginning of the Cb Vals
  ------------------------------------------------------------------------*/
  cb_ptr = input_img_ptr->imgPtr +
           ( input_img_ptr->dx * input_img_ptr->dy  );

  /*------------------------------------------------------------------------
      Takes us to the offset for Cb Vals
  ------------------------------------------------------------------------*/
  cr_offset = ( input_img_ptr->dx * input_img_ptr->dy  ) >> 2 /* /4 */;


  /*------------------------------------------------------------------------
      We process 2 rows at a time in this encoding. So we need half the
      number of iterations
  ------------------------------------------------------------------------*/
  for ( row = crop->dy/2; row; row-- )
  {
    /*----------------------------------------------------------------------
        We process 2 cols at a time we need half the number of coloumns
        iteratios.
    ----------------------------------------------------------------------*/
    for ( col = crop->dx/2; col; col-- )
    {

      /*--------------------------------------------------------------------
          The format of the YCbCr420 packing as follows

          |   (dx*dy) of Y values )        |1/4 Yvals|1/4 Yvals |
          |--------------------------------|---------|----------|
          |--------------------------------|---------|----------|
                Y values                     Cb Vals    Cr Vals
      --------------------------------------------------------------------*/


      /*--------------------------------------------------------------------
          Take Luma of all four pixels. The Lumma Packing is as shown
          below

          |Y1|Y2|             (this processing)   |RGB1|RGB2|
                ---Cb, Cr  ------------------->   |RGB3|RGB4|
          |Y3|Y4|
      --------------------------------------------------------------------*/
      lumaa1 = *inputImgPtr;  /* corresponds to Y1 in the pic */
      lumaa2 = *(inputImgPtr + 1);  /* corresponds to Y2 in the pic */

      /*--------------------------------------------------------------------
            Following line gets you Y3 and then base increments on
            inputImgPtr .. Hopefully
      --------------------------------------------------------------------*/
      lumaa3 = *(inputImgPtr++ + input_row_size);
      lumaa4 = *( inputImgPtr++ + input_row_size);

      /*--------------------------------------------------------------------
          Cb is for all the 4  Yavals
      --------------------------------------------------------------------*/
      cb = *(cb_ptr);

      /*--------------------------------------------------------------------
          Get the Cr for all 4 Y vals
      --------------------------------------------------------------------*/
      cr = *(cb_ptr + cr_offset);

      /*--------------------------------------------------------------------
          We got both CB and Cr for this iteration we offset cb_ptr.
          If you look at the pics you can see that they both move in
          tandem
      --------------------------------------------------------------------*/
      ++cb_ptr;


      /*--------------------------------------------------------------------
        Take the delta from third to fourth. This is for faster
        computation as you see later. We are doing this in the reverse order
        because we will loose the original delta if do in the forward
        order
      --------------------------------------------------------------------*/
      lumaa4 -= lumaa3;

      /*--------------------------------------------------------------------
        Take the delta from second to third. This is for faster
        computation as you see later
      --------------------------------------------------------------------*/
      lumaa3 -= lumaa2;

      /*--------------------------------------------------------------------
        Take the delta from first to second. This is for faster
        computation as you see later
      --------------------------------------------------------------------*/
      lumaa2 -= lumaa1;


      /*--------------------------------------------------------------------
        Each of these comparisons add and compares are designed to force
        the compiler to do conditional execution
      --------------------------------------------------------------------*/
      r = lumaa1 + qtv_Cr2RTable[cr];
      /*--------------------------------------------------------------------
          If  r is -ve reinitialize out = 0 and start adding stuff as
          you need it. No need for an extra check for the second pixel
      --------------------------------------------------------------------*/
      if ( r > 0 )
      {
        out = rTable[ r ];
      }
      else
      {
        out = 0;
      }

      /*--------------------------------------------------------------------
          Add the delta in Luma2 to calculate the next R value
      --------------------------------------------------------------------*/
      r += lumaa2;

      if ( r > 0 )
      {
        out2 = rTable[ r ];
      }
      else
      {
        out2 = 0;
      }

      /*--------------------------------------------------------------------
          Add the delta b/w in 2 and 3 Luma
      --------------------------------------------------------------------*/
      r += lumaa3;



      if ( r > 0 )
      {
        out3 = rTable[ r ];
      }
      else
      {
        out3 = 0;
      }

      /*--------------------------------------------------------------------
          Add the delta b/w 3 and 4 in Luma to get next R4
      --------------------------------------------------------------------*/
      r += lumaa4;

      if ( r > 0 )
      {
        out4 = rTable[ r ];
      }
      else
      {
        out4 = 0;
      }



      r = lumaa1 - qtv_Cb2GTable[cb] - qtv_Cr2GTable[cr];

      if ( r > 0 )
      {
        out += gTable[r];
      }

      r += lumaa2;

      if ( r > 0 )
      {

        out2 += gTable[r];
      }

      r += lumaa3;

      if ( r > 0 )
      {

        out3 += gTable[r];
      }

      r += lumaa4;

      if ( r > 0 )
      {
        out4 += gTable[r];
      }


      r = lumaa1 + qtv_Cb2BTable[cb];
      /*--------------------------------------------------------------------
         If we have -ve or zero value we dont need to do anthing as the
         translation or effect of that component on the whole pixel
         value isbound to be zero no matter what encoding RGB flavor we
         are using.
      --------------------------------------------------------------------*/
      if ( r > 0 )
      {
        out += bTable[r];
      }

      r += lumaa2;

      if ( r > 0 )
      {
        out2 += bTable[r];
      }

      r += lumaa3;

      if ( r > 0 )
      {
        out3 += bTable[r];
      }

      r += lumaa4;

      if ( r > 0 )
      {
        out4 += bTable[r];
      }

      *outputImgPtr = (uint16) out;
      *(outputImgPtr + 1) = (uint16) out2;
      *(outputImgPtr++ + rowSize) = (uint16) out3;
      *(outputImgPtr++ + rowSize) = (uint16) out4;
    }

    /*----------------------------------------------------------------------
        Need to add  number of coulumns * 2 /4 --> rowSize /2
        which is same as rowSize >> 1;
    ----------------------------------------------------------------------*/

    outputImgPtr =  (uint16 *) ( (uint8 *) outputImgPtr +
                      (rowSize << 1)  + destInc ) ;

    inputImgPtr += (uint32) (rowInc  + input_row_size);

    /*----------------------------------------------------------------------
      Remember each four pixels has same CB/CR combination. So we need
      to add number of pixels offset divided by 4

      YY
      -----------------maps to one cr/cb --> cr/cb
      YY

    ----------------------------------------------------------------------*/
    cb_ptr += (rowInc) >> 1 /* divided by 2 */;

  }

  return( IPL_SUCCESS );

} /* end of function qtv_ipl_convertAndCrop_YCbCr420ToRGB */

/*--------------------------------------------------------------------------                         |
                          LOCAL UNEXPOSED ROUTINES
--------------------------------------------------------------------------*/

int32 qtv_ipl_roundFloatVal
(
  float valToRound
)
{
  int32 trunc;

  trunc = (int32) valToRound;

  /*------------------------------------------------------------------------
    This assumes that the rouding  setup on call to the function is
    truncation.
  ------------------------------------------------------------------------*/
  if ( valToRound > 0 )
  {
    valToRound +=  0.5;
    trunc = (int32)valToRound;
  }
  else if ( valToRound < 0 )
  {
    valToRound -=  0.5;
    trunc = (int32)valToRound;
  }

  return( trunc );

} /* end of qtv_ipl_roundFloatVal */

