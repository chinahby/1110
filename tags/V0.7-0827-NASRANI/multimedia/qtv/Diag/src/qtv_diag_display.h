#ifndef QTVDIAGDISP_H
#define QTVDIAGDISP_H

/*=============================================================================
FILE:       qtv_diag_display.h

SERVICES:   Provides functionality to qtvdiag for displaying decoded frames. 

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/04   anandr  Initial Revision

$Header: //source/qcom/qct/multimedia/qtv/diag/main/latest/src/qtv_diag_display.h#5 $
$DateTime: 2008/05/07 09:12:55 $
$Change: 655026 $
============================================================================*/


/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
      Type Declarations
----------------------------------------------------------------------------*/

/*===========================================================================
   FUNCTION DEFINITIONS and INLINE FUNCTION DEFINITIONS
=========================================================================== */

/*===========================================================================

Function: qtvdiag_display_frame

Description: Call this function to render a video frame to the screen.

Return Value:
  None.

Comments:

Side Effects:
  Modifies the contents of the display device buffer.

============================================================================*/
void qtvdiag_display_frame (
  unsigned short *pFrameBuf,
  uint16  Height,  
  uint16  Width 
);

/*===========================================================================

Function: qtvdiag_set_display_rect

Description: Call this function to set the output video frame display location.
             The actual video will be centered within this rectangle
             clipping/padding as necessary.

Return Value:
  None.

Comments:

Side Effects:

============================================================================*/
void qtvdiag_set_display_rect (
  uint16  x,    /* starting column */
  uint16  y,    /* starting row */
  uint16  dx,   /* window width */
  uint16  dy    /* window length */
);


/*===========================================================================

Function: qtvdiag_free_frame_buffer

Description: Call this function to inform the codec that the specified frame
             buffer has been rendered or dropped and can be safely reused by the
             codec.

Return Value:
  None.

Comments:

Side Effects:

============================================================================*/
void qtvdiag_free_frame_buffer (
  void *pFrameBuf
);
#endif /* end QTVDIAGDISP_H  */
