#ifndef MDP_FRAME_H_
#define MDP_FRAME_H_
/*============================================================================

                   MDP Frame Service Header File

  DESCRIPTION
  This file provides the API and data type for the MDP Frame Service.
  The Frame Service implements dynamic memory allocation for display buffers.
  
  API:
     mdp_frame_type application frame-buffer data-type 
     MDP_BUFFER_TEST_OPTION option-argument data-type for mdp_check_frames()
  
     mdp_create_primary_frame   create a frame for the primary display
     mdp_create_secondary_frame create a frame for the secondary display
     mdp_create_largest_frame   create a frame to accomodate largest display
     mdp_free_frame             frees/deletes the provided frame
     mdp_check_frame            check given frame for buffer overflow
     mdp_check_frames           runs frame-check for all registered frames
     
     SCR_BYTE_SIZE macro to compute size of display

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/inc/mdp_buffers.h#2 $ 
  $DateTime: 2008/11/06 22:45:00 $ 
  $Author: gauravs $

when      who        what, where, why
--------  ---------  ----------------------------------------------------------
11/07/08  gs             Add frame dimension macros, moved from mdp_buffers.c 
07/11/07  sbernard   Creation
============================================================================*/

/*============================================================================
                            INCLUDE FILES
============================================================================*/
#include "comdef.h"
#include "queue.h"
#include "disp.h"

/*=============================================================================
                        PRIVATE MACRO DEFINITION
=============================================================================*/

/* QCIF resolution */
#define QCIF_WIDTH 176
#define QCIF_HEIGHT 220

/* QVGA resolution */
#define QVGA_WIDTH 240
#define QVGA_HEIGHT 320

/* Max allowed display resolution */
#define MAX_WB_WIDTH QVGA_HEIGHT
#define MAX_WB_HEIGHT QVGA_HEIGHT

/*=============================================================================
                              DATA TYPE
=============================================================================*/

/* Frame buffer data-structure */
typedef struct mdp_frame_struct 
{
  uint8* buffer;     /* pointer to screen buffer */
  uint16 pxWidth;    /* screen width in pixel */
  uint16 pxHeight;   /* screen width in pixel */
  uint32 byteSize;   /* buffer size in bytes */
  disp_bpp_type bpp; /* screen byte-per-pixel */

  /*== private data ==*/
  void* pParent;     /* ptr to parent frame-service data structure */
} mdp_frame_type;

/* Option arguments for mdp_check_frames() */
typedef enum mdp_buffer_test_option_struct 
{
  ROUND_ROBIN,
  ALL
} MDP_BUFFER_TEST_OPTION;

/*=============================================================================
                            MACRO DEFINTION
=============================================================================*/

/* Computes the byte size of a screen for a given BPP format
** Note: arguments MUST be integers! */
#define SCR_BYTE_SIZE(w,h,bpp) (((w*h*bpp)/8)+	(((w*h*bpp)%8)?1:0))

/*=============================================================================
                                  API
=============================================================================*/

//==>> API <<=============================================================
//  mdp_create_primary_frame
//------------------------------------------------------------------------
// Description: returns a frame of a given bpp for the primary display.
//
// Arguments:
// - bpp:  bit-per-pixel format
//
// Return:
// - ptr to mdp frame
//
// Dependencies:
//
//========================================================================
mdp_frame_type* mdp_create_primary_frame(disp_bpp_type bpp);

//==>> API <<=============================================================
//  mdp_create_secondary_frame
//------------------------------------------------------------------------
// Description: returns a frame of a given bpp for the secondary display.
//
// Arguments:
// - bpp:  bit-per-pixel format
//
// Return:
// - ptr to mdp frame
//
// Dependencies:
//
//========================================================================
mdp_frame_type* mdp_create_secondary_frame(disp_bpp_type bpp);

//==>> API <<=============================================================
//  mdp_create_largest_frame
//------------------------------------------------------------------------
// Description: returns a frame of a given bpp for to accomodate either
// primary or secondary display.
//
// Arguments:
// - bpp:  bit-per-pixel format
//
// Return:
// - ptr to mdp frame
//
// Dependencies:
//
//========================================================================
mdp_frame_type* mdp_create_largest_frame(disp_bpp_type bpp);

//==>> API <<=============================================================
//  mdp_free_frame
//------------------------------------------------------------------------
// Description: releases an existing mdp frame.
//
// Arguments: 
// - ptr. to mdp frame
//
// Return: none
//
// Dependencies:
//
//========================================================================
void mdp_free_frame( mdp_frame_type* pFrame );

//==>> API <<=============================================================
//  mdp_check_frames
//------------------------------------------------------------------------
// Description: performs sanity tests on ALL allocated mdp frames.
//
//  -NOTE: Currently, only buffer overflow is implemented.
//
// Arguments: 
//- opt: test option (only ALL implemented)
//
// Return:
// - boolean: TRUE if successful, FALSE if test failed.
//
// Dependencies:
//
//========================================================================
boolean mdp_check_frames( MDP_BUFFER_TEST_OPTION opt );

//==>> API <<=============================================================
//  mdp_check_frame
//------------------------------------------------------------------------
// Description: performs sanity tests on a given mdp format.
//
// Arguments:
// - ptr. to mdp frame
//
// Return:
// - boolean: TRUE if successful, FALSE if test failed.
//
// Dependencies:
//
//========================================================================
boolean mdp_check_frame( mdp_frame_type* pFrame );

#endif /*MDP_FRAME_H_*/

