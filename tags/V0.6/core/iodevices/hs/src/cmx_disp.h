#ifndef CMX_DISP_H
#define CMX_DISP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Implements the GAPI to display CMX

GENERAL DESCRIPTION
    This file contains the implementation of the GAPI interface
    to display CMX on the display hardware of Zeus.

EXTERNALIZED FUNCTIONS
   cmx_disp_pic_control()
   cmx_disp_saf_header()
   cmx_disp_pic_data()

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $PVCSPath:  L:/src/asw/MSM6050/vcs/cmx_disp.h_v   1.2   21 Mar 2002 20:52:04   sarnold  $
 $Header: //depot/asic/msmshared/services/hs/cmx_disp.h#3 $ $DateTime: 2002/03/21 20:52:04 $ $Author: sarnold $

 * 
 *    Rev 1.0   14 Aug 2001 11:36:26   karthick
 * Initial revision.
 * 
 *    Rev 1.0   05 Apr 2001 18:29:42   karthick
 * Initial revision.
============================================================================*/

#include "target.h"
#include "customer.h"

#include "comdef.h"

typedef enum {
  CMX_DISP_AF_POS_LT,          /* Left and/or top justify     */
  CMX_DISP_AF_POS_RB,          /* Right and/or bottom justify */
  CMX_DISP_AF_POS_CENTER,      /* Center justify              */
  CMX_DISP_AF_POS_PERCENT,     /* Use percentage for offset   */
  CMX_DISP_AF_POS_BIG = 0xffffff
} cmx_disp_af_pos_enum_type;

typedef enum {
  CMX_DISP_AF_PIC_STATUS_INIT,     /* Init parameters for new picture         */
  CMX_DISP_AF_PIC_STATUS_CLEAR,    /* Clear picture display                   */
  CMX_DISP_AF_PIC_STATUS_ENABLE,   /* Enable picture display                  */
  CMX_DISP_AF_PIC_STATUS_DISABLE,  /* Disable Picture display                 */
  CMX_DISP_AF_PIC_STATUS_ERROR,/* Error occured while processing picture data */
  CMX_DISP_AF_PIC_STATUS_BIG = 0xffffff
} cmx_disp_af_pic_status_enum_type;

typedef enum {
  CMX_DISP_AF_PIC_DATA_APPEND, /* Append picture data to current data         */
  CMX_DISP_AF_PIC_DATA_DONE,   /* Final block of picture data for current pic */
  CMX_DISP_AF_PIC_DATA_BIG = 0xffffff
} cmx_disp_af_pic_data_enum_type;

typedef enum {
  CMX_DISP_AF_PIC_PNG,        /* Data is in decoded PNG format (PNM format) */
  CMX_DISP_AF_PIC_JPEG,       /* Data is in decoded JPEG format (PNM format)*/
  CMX_DISP_AF_PIC_BMP,        /* Data is in BMP format                      */
  CMX_DISP_AF_PIC_UNKNOWN,    /* Data is of unknown format                  */
  CMX_DISP_AF_PIC_BIG = 0xffffff
} cmx_disp_af_pic_enum_type;

/* A point is expressed as (x,y), where x increases toward the right and 
   y increases toward the bottom */
typedef struct
{
  uint32 x;    /* x coordinate */
  uint32 y;    /* y coordinate */
} cmx_disp_point_type;

/* The size data type is used to specify dimension and location information
   (in pixels) */
typedef struct
{
  uint32 width;      /* Number of pixels in the horizontal direction */
  uint32 height;     /* Number of pixels in the vertical direction */
} cmx_disp_rect_size_type;

extern void cmx_disp_pic_control (
  cmx_disp_af_pic_status_enum_type  pic_ctl,
  cmx_disp_af_pos_enum_type         x_pos,
  cmx_disp_af_pos_enum_type         y_pos,
  uint8                             x_percent,
  uint8                             y_percent,
  cmx_disp_point_type              *start_position,
  cmx_disp_rect_size_type          *pic_size
);

extern void cmx_disp_saf_header (
  uint8            *header,
  uint32          num_bytes
);

extern void cmx_disp_pic_data (
  boolean                           ispng,
  cmx_disp_af_pic_data_enum_type    pic_data,
  cmx_disp_af_pic_enum_type         pic_format,
  uint32                            num_bytes,
  uint8                            *buffer
);

#endif    /* CMX_DISP_H */

