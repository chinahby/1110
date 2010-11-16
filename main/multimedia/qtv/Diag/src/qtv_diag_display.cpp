/*=============================================================================
FILE:       qtvdiagdisp.c

SERVICES:   This file contains helper functions that will be used by QTVDIAG
  in order to bypass BREW and directly render frames to the display. 

PUBLIC CLASSES:   Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  There are no initialization and sequencing requirements for the routines in
  this file

        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/qtv/diag/main/latest/src/qtv_diag_display.cpp#5 $
$DateTime: 2008/05/07 09:12:55 $
$Change: 655026 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/04   anr     (anandr) Initial Revision

============================================================================*/

/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"

#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE

#include "disp.h"               /* Display driver routines           */

#include "qtv_diag_display.h"         /* external video rendering functions */
#include "qtv_msg.h"

#ifdef FEATURE_QTV_MDP
extern "C" {
  #include "mdp_drv_api.h"
}
#endif /* FEATURE_QTV_MDP */
/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/
/* Output window, source image and destination display structure */
typedef struct
{
  disp_info_type  my_disp_info;   /* local display information */
  
  /* Output window location and size */
  uint16    disp_x;   /* display window starting column */
  uint16    disp_y;   /* display window starting row */
  uint16    disp_dx;  /* display window width   */
  uint16    disp_dy;  /* display window height */

  /* Source image location and size */
  uint16     src_x;   /* source image starting col */
  uint16     src_y;   /* source image starting row */
  uint16     src_dx;  /* source image width */
  uint16     src_dy;  /* source image height*/

  /* Destination location and size */
  uint16     dest_x;  /* destination starting col */
  uint16     dest_y;  /* destination starting row */
  uint16     dest_dx; /* destination width  */
  uint16     dest_dy; /* destination hieght */

} qtvdiag_display_type;

static qtvdiag_display_type qtvdiag_display;


#define OFFSET_Y_SIZE  35
#define OFFSET_X  0
#define OFFSET_Y  40

/*===========================================================================

Function: qtvdiag_calc_display_rect

Description: This function recalculates the values in qtvdiag_display after its
             fields have been changed.  In particular, the following fields are
             recalculated:

             qtvdiag_display.src_x
             qtvdiag_display.src_y
             qtvdiag_display.dest_x
             qtvdiag_display.dest_y
             qtvdiag_display.dest_dx
             qtvdiag_display.dest_dy

Return Value:
  None.

Comments:  If any fields other than the above are modified, this function MUST
           be called before qtvdiag_display_frame is called.

Side Effects:

============================================================================*/
static void qtvdiag_calc_display_rect(void)
{
  /*
  **  Calculate the source clipping area and destination location
  */

  /*
  **  If the output window is wider than or equal to the source image:
  **    - the starting source column is the beginning of the image
  **    - the destination column is the window starting column plus half the
  **      window & source size difference.
  **    - the copy width is merely the source width
  */
  if (qtvdiag_display.disp_dx >= qtvdiag_display.src_dx)
  {
    qtvdiag_display.src_x = 0;
    qtvdiag_display.dest_x  = qtvdiag_display.disp_x +
                          (qtvdiag_display.disp_dx - qtvdiag_display.src_dx) / 2;
    qtvdiag_display.dest_dx = qtvdiag_display.src_dx;
  }

  /*
  **  else the output window is narrower than the source image:
  **    - the starting source column is half the window & src size difference
  **    - the destination column is merely the windows starting column.
  **    - the copy width is merely the window width
  */
  else
  {
    qtvdiag_display.src_x = (qtvdiag_display.src_dx - qtvdiag_display.disp_dx) / 2;
    qtvdiag_display.dest_x  = qtvdiag_display.disp_x;
    qtvdiag_display.dest_dx = qtvdiag_display.disp_dx;
  }

  /*
  **  If the output window is longer than or equal to the source image:
  **    - the starting source row is the beginning of the image
  **    - the destination row is the window starting row plus half the
  **      window & source size difference.
  **    - the copy height is merely the source height
  */
  if (qtvdiag_display.disp_dy >= qtvdiag_display.src_dy)
  {
    qtvdiag_display.src_y = 0;
    qtvdiag_display.dest_y  = qtvdiag_display.disp_y +
                        ((qtvdiag_display.disp_dy - qtvdiag_display.src_dy) / 2);
    qtvdiag_display.dest_dy = qtvdiag_display.src_dy;
  }

  /*
  **  else the output window is shorter than the source image:
  **    - the starting source row is half the window & src size difference
  **    - the destination column is merely the windows starting column.
  **    - the copy width is merely the window height
  */
  else
  {
    qtvdiag_display.src_y = (qtvdiag_display.src_dy - qtvdiag_display.disp_dy) / 2;
    qtvdiag_display.dest_y  = qtvdiag_display.disp_y;
    qtvdiag_display.dest_dy = qtvdiag_display.disp_dy;
  }

  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"dest x=%d  y=%d", \
       qtvdiag_display.dest_x, qtvdiag_display.dest_y);
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"dest size Dx=%d  Dy=%d", \
       qtvdiag_display.dest_dy, qtvdiag_display.dest_dx);
} /* end qtvdiag_calc_display_rect */

/*===========================================================================
Function: qtvdiag_display_frame

Description:  Call this function to render the decoded frame at the appropriate
  time.

Return Value:
   void

Comments:

Side Effects:

============================================================================*/
void qtvdiag_display_frame ( 
  unsigned short *pFrameBuf,
  uint16  Height,  
  uint16  Width )
{

  /* Record the clip size */
  unsigned short buf[12*10] = {0};
  int i = 0;
  for(i=0;i<120;i++)
  {
  	buf[i] = 0xf800;
  }
  qtvdiag_display.src_dx  = Width;
  qtvdiag_display.src_dy  = Height;

  /* Setup the output video frame display information */
  qtvdiag_display.my_disp_info = disp_get_info();

  	QTV_MSG7(QTVDIAG_GENERAL,"qtvdiag_display : %d,%d,%d,%d,%d,%d,%d",
		qtvdiag_display.src_dx,
		       qtvdiag_display.src_y,   
               qtvdiag_display.src_x, 
               qtvdiag_display.dest_dy, 
               qtvdiag_display.dest_dx, 
               qtvdiag_display.dest_y,  
               qtvdiag_display.dest_x );

  qtvdiag_set_display_rect( OFFSET_X, 
                            OFFSET_Y, 
                            qtvdiag_display.my_disp_info.disp_width - (2*OFFSET_X), 
                            qtvdiag_display.my_disp_info.disp_height - OFFSET_Y - OFFSET_Y_SIZE);

#ifdef FEATURE_QTV_MDP
  MDPIMG mdpImg;

  mdpImg.dest = PRIMARY_LCD_TYPE;     
  mdpImg.imgType = MDP_YCBCR420;
  mdpImg.bmy_addr = (uint32 *)pFrameBuf;
  mdpImg.cbcr_addr = (uint32 *)&pFrameBuf[qtvdiag_display.src_dx * qtvdiag_display.src_dy/2]; 
  mdpImg.width = qtvdiag_display.src_dx;
  mdpImg.cwin.x1 = 0;
  mdpImg.cwin.x2 = qtvdiag_display.src_dx-1;
  mdpImg.cwin.y1 = 0;
  mdpImg.cwin.y2 = qtvdiag_display.src_dy-1;
  mdpImg.lcd_x = qtvdiag_display.dest_x;
  mdpImg.lcd_y = qtvdiag_display.dest_y;
  mdpImg.mdpOp = MDPOP_NOP;
  mdpImg.layer = MDP_FGL;

  (void)mdp_disp_update(&mdpImg);

#else /* Non-MDP */
  pFrameBuf = buf;
	QTV_MSG7(QTVDIAG_GENERAL,"qtvdiag_display : %d,%d,%d,%d,%d,%d,%d",
		qtvdiag_display.src_dx,
		       qtvdiag_display.src_y,   
               qtvdiag_display.src_x, 
               qtvdiag_display.dest_dy, 
               qtvdiag_display.dest_dx, 
               qtvdiag_display.dest_y,  
               qtvdiag_display.dest_x );
	
  disp_update( pFrameBuf,  
               qtvdiag_display.src_dx, 
               qtvdiag_display.src_y,   
               qtvdiag_display.src_x, 
               qtvdiag_display.dest_dy, 
               qtvdiag_display.dest_dx, 
               qtvdiag_display.dest_y,  
               qtvdiag_display.dest_x );
#endif /* FEATURE_QTV_MDP */

} /* end qtvdiag_display_frame */

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
)
{
  /* Record the output display window parameters  */
  qtvdiag_display.disp_x = x;
  qtvdiag_display.disp_y = y;
  qtvdiag_display.disp_dx = dx;
  qtvdiag_display.disp_dy = dy;

  /* Calculate the display rectangle */
  qtvdiag_calc_display_rect();
}

#endif /* FEATURE_DISABLE_QTV_DIAG_IFACE */
