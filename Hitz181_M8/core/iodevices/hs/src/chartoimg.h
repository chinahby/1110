#ifndef CHARTOIMG_H
#define CHARTOIMG_H

/*============================================================================

                 Interface to the Bitmap Display Driver

 DESCRIPTION
   This file contains the definitions needed for the converting character
   operations to bitmap operations rendered on the bitmap display.

 Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $PVCSPath:  L:/src/asw/MSM6050/vcs/chartoimg.h_v   1.0   13 Mar 2002 07:39:38   dlee  $
 $Header: //depot/asic/msmshared/services/hs/chartoimg.h#1 $ $DateTime: 2002/03/13 07:39:36 $ $Author: dlee $

 * 
 *    Rev 1.1   05 Apr 2001 17:10:48   karthick
 * Fixed log
 * 
 *    Rev 1.0   05 Apr 2001 16:58:12   karthick
 * Initial revision.
============================================================================*/

#include "target.h"
#include "customer.h"

#ifdef FEATURE_BITMAP
#include "comdef.h"    /* Contains REX basic types definitions      */

//============================================================================
//                        DATA DECLARATIONS
//============================================================================

/*--- Type Definitions --*/

//============================================================================
//                        EXPORTED FUNCTIONS
//============================================================================

/*===========================================================================

FUNCTION       CHARTOIMG_INIT

DESCRIPTION
               This function initialises and clears the entire display screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_init(void);


/*===========================================================================

FUNCTION       CHARTTOBMP_UPDATE_SCREEN

DESCRIPTION
               This function updates the screen portion of the display
               with the input character buffer. It basically draws
               the text on the screen portion.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_update_screen(
  /* A pointer to a buffer containing the 4 lines of characters */
  byte *screen_ptr
);


/*===========================================================================

FUNCTION       CHARTOIMG_UPDATE_ANNUNCIATORS

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void chartoimg_update_annunciators(
  /* A pointer to the annunciators to set */
  word *annunciator_ptr, 
  /* A pointer to the RSSI level to set */
  byte *rssi_ptr,
  /* A pointer to the battery level to set */
  byte *batt_ptr
);
#endif /* FEATURE_BITMAP */
#endif    /* CHARTOIMG_H */
