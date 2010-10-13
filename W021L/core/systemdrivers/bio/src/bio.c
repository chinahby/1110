/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 B A S I C   I / O   S E R V I C E S

GENERAL DESCRIPTION
  This module contains control and data to facilitate interaction with the
  GPIO and Miscellaneous Bit I/O ports.  This module is designed to provide
  appropriate buffering and control for these ports, but not to guarantee
  the behaviour or activation levels of the ports.

REFERENCES
  CDMA ASIC specification sheets, all.
  Schematic appropriate to the target application using this service.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None, all values are initialized to 0 by the C run-time environment.

Copyright (c) 1992, 1993, 1994 by QUALCOMM, Incorporated. All Rights Reserved
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated. All Rights Reserved
Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/bio/bio.c#2 $
$DateTime: 2007/12/19 09:32:16 $
$Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/07   vph     Ported for QSC11XX from QSC6055
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.     */
#include "target.h"
#include "rex.h"
#include "bio.h"                /* BIO typedef for bio_sw_type          */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   GPIO port addresses and shadow buffers
-------------------------------------------------------------------------- */
bio_gpio_access_type  bio_gpio[NUM_GPIO] =
{
  /* Output                 Input                         Tri-state          */
  { HWIO_ADDR(GPIO_OUT_0) , HWIO_ADDR(GPIO_IN_0)        , HWIO_ADDR(GPIO_OE_0) },
  { HWIO_ADDR(GPIO_OUT_1) , HWIO_ADDR(GPIO_IN_1)        , HWIO_ADDR(GPIO_OE_1) },
  { HWIO_ADDR(GPIO2_OUT_0), HWIO_ADDR(GPIO2_IN_0)       , HWIO_ADDR(GPIO2_OE_0) },
  { HWIO_ADDR(GPIO2_OUT_1), HWIO_ADDR(GPIO2_IN_1)       , HWIO_ADDR(GPIO2_OE_1) },
  { 0                     , HWIO_ADDR(MSM_KEYSENSE_GROUP_STATUS)    , 0          },
  { MSM3_DUMMY            , MSM3_DUMMY                   , MSM3_DUMMY   }
};

bio_gpio_image_type   bio_gpio_image[NUM_GPIO] =
{
  /* Output shadow    Tri-state shadow */
  { 0x0             , 0x0           },
  { 0x0             , 0x0           },
  { 0x0             , 0x0           },
  { 0x0             , 0x0           },
  { 0x0             , 0x0           },
  { 0x0             , 0x0           }
};

/* Buffer for BIO LED outputs
*/
word bio_led_buf;

/* RAM image of 'soft' DIP switches.  Init to all False.
*/
bio_sw_type bio_switches = 0;


/*===========================================================================

FUNCTION BIO_SET_SW

DESCRIPTION
  Set the 'soft' DIP switches

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bio_set_sw
(
  bio_sw_type switches
    /* Value to set the DIP switches to */
)
{
  INTLOCK();
  bio_switches = switches;
  INTFREE();
}


/*===========================================================================

FUNCTION BIO_GET_GPIO_ACCESS

DESCRIPTION
  This function returns a pointer to the bio_gpio_access_type table for
  each of the gpio registers, allowing the user to manipulate the
  IO registers directly.

  The user must update the shadow registers accordingly, using
  bio_get_gpio_image.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the bio_gpio[] array.

SIDE EFFECTS
  None

===========================================================================*/
bio_gpio_access_type *bio_get_gpio_access(word idx)
{
   return &bio_gpio[idx];
}


/*===========================================================================

FUNCTION BIO_GET_GPIO_IMAGE

DESCRIPTION
  This function returns a pointer to the bio_gpio_image_type table for
  each of the gpio registers, allowing the user to manipulate the
  shadow registers directly.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the bio_gpio_image[] array.

SIDE EFFECTS
  None

===========================================================================*/
bio_gpio_image_type *bio_get_gpio_image(word idx)
{
   return &bio_gpio_image[idx];
}


#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  OSBL_SAVE_BIO_SHARED_INFO

DESCRIPTION
 This function will save bio shadow info to shared segment

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void osbl_save_bio_shared_info(void)
{
  // TO BE COMPLETED
}
#endif
