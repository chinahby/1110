/* ========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ipl_types.h"
#ifndef FEATURE_IPL_NO_CAMERA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ipl.h"
#include "ipl_helper.h"
#include "ipl_tone.h"
#include "ipl_redeye.h"



int gIt;            // which invocation in seriesof images
int gId;            // file number
int gFileId;
int gMegaPix;


/*===========================================================================
 *
 * This function will find and correct red eyes
 *
 ==========================================================================*/
int ipl_fixRedEye
(
  ipl_image_type * in, 
  ipl_image_type * out, 
  ipl_rect_type * icrop, 
  int fn
)
{
  return (1);
}
#endif /*!FEATURE_IPL_NO_CAMERA*/

