/*===========================================================================

        I M A G E  P R O C E S S I N G   L I B R A R Y    F I L E

DESCRIPTION
  Th1s file contains the implementation of skin tone detecton IPL APIs.

REFERENCES
  IPL ISOD: XX-XXXXX-XX

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/* <EJECT> */
/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "ipl_types.h"
#ifndef FEATURE_IPL_NO_CAMERA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ipl_tone.h"
#include "ipl_compose.h"
#include "ipl_helper.h"
#include "ipl.h"

// Turn off some lint warnings
/*lint -save -e508, extra extern is okay */

#define IPL_TONE_DEBUG    0
#define IPL_TONE_DEBUG_2  0
#define IPL_TONE_DEBUG_3  0
#define IPL_TONE_DEBUG_4  0

/*===========================================================================
                      GLOBALS
===========================================================================*/

// return value of our various functions
int gipl_count[TONE_MAX_REGIONS][TONE_MAX_REGIONS]; // % skin for block
ipl_rect_type gipl_regions[TONE_MAX_REGIONS];       // list of regions
                                                    // .x must be < 10000
int gipl_maxReg;          // total number of region found 
int gipl_bestReg;         // index of region most like face





// these are temporary variables used by the inline funtions.
uint8 gipl_tluma, gipl_tcb, gipl_tcr, gipl_subCr;

// the return value of th function is put in gipl_match. 1 means it is
// skin,red,or whatever color, and 0 means not.
uint8 gipl_match;

// local globals
int ipl_count2[TONE_MAX_REGIONS][TONE_MAX_REGIONS];
int tMinx, tMiny, tMaxx, tMaxy;


/*===========================================================================
                      TONE TABLES
===========================================================================*/

uint8 sTable_v1_flash[1][1][1];
uint8 sTable_v1_mixed[1][1][1];
uint8 sTable_v2_mixed[1][1][1];
uint8 rTable[1][1][1];



/*
 *
 * Draw gipl_count's (blocks) or gipl_regions (regions) contents
 *
 */
API_EXTERN ipl_status_type ipl_draw_skin
(
  ipl_image_type* input_img_ptr,     // input image  
  ipl_image_type* out_img_ptr,       // if NULL, will draw over input image 
  ipl_rect_type* crop,    // what area to draw, must be that used when skin
                          // content was being computed by ipl_find_skin
  int dicex,              // must match dicex, dicey used when finding skin
  int dicey,
  int mode                // 2 means draw blocks, 3 means draw regions 
)
{
  return IPL_SUCCESS;
}



/* <EJECT> */
/*===========================================================================

FUNCTION ipl_find_skin

DESCRIPTION

  This function will return the location of skin blocks in an image
  that have a good chance of being a face

  It is imperative that the input image have gone through the entire
  color pipeline including gamma and AWB before we try to detect skin.

  The input should be YCrCb 4:2:0 line pack or frame pack
  The output should be YCrCb 4:2:0 line pack or frame pack

DEPENDENCIES

  None

ARGUMENTS IN

  input_img_ptr   points to the input image

  crop       area of image to look for skin

  dicex      number of blocks in x-direction 
  dicey      number of blocks in y-direction 

  thresh     threshold used to determin if a block has enough skin

  regionalize should we return regions of skin blocks

  color      what color (skin) to look for


RETURN VALUE
  IPL_SUCCESS   is operation was succesful
  IPL_FAILURE   otherwise

SIDE EFFECTS

  gipl_count[dicex][dicey] will hold the % of skin pixels in dicex * dicey 
  blocks. E.g. if dicex and dicey are 4 and 4, then gipl_count[0][0] through
  gipl_coutn[4][4] will have meaningful values.

===========================================================================*/
API_EXTERN ipl_status_type ipl_find_skin
(
  ipl_image_type* input_img_ptr,      
  ipl_rect_type* crop,
  int dicex,
  int dicey,
  int thresh,
  int regionalize,
  int color 
)
{
  return IPL_SUCCESS;
}
#endif /*!FEATURE_IPL_NO_CAMERA*/


/*lint -restore */
