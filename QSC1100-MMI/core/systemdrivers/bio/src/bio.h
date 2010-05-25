#ifndef BIO_H
#define BIO_H

/*===========================================================================

                   B I T   I / O   D E C L A R A T I O N S

DESCRIPTION
  This file contains definitions to facilitate interaction with the DMSS
  GPIO and Miscellaneous Bit I/O ports.  This module is designed to provide
  appropriate buffering and control for these shared GPIO ports.

Copyright (c) 1992, 1993 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/bio/bio.h#2 $ 
  $DateTime: 2007/12/19 09:32:16 $ 
  $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/07   vph     Ported for QSC11XX from QSC6055
03/22/07   fs      Removed BIO_NOT_PORT related remappings of BIO macros
01/18/07   cr      Put BIO LED output on second LED port.
01/15/07   fs      Ported for QSC6055.
06/18/05   gr      Mods for L4.
10/15/04   ts      Ported for MSM6800 from MSM6550
04/15/04   bmg     Ported for MSM6550 from MSM6500
===========================================================================*/
#include "comdef.h"     /* Definitions for ROM, etc.                       */
#include "target.h"     /* Target specific definitions                     */
#include "msm.h"
#include "bio_port.h"

/*-------------------------------------------------------------------------*/
/* GPIO types. Use these names to define registers in bio?.h files.        */
typedef enum {
  GPIO_REG_0 = 0,       /* GPIO 12:29 */
  GPIO_REG_1,           /* GPIO 34:55 */
  GPIO2_REG_0,          /* GPIO2  0:11, 30, 31 */
  GPIO2_REG_1,          /* GPIO2 32, 33 */
  GPIO_KEYSENSE,        /* GPIO keysense read register */
  GPIO_REG_DUMMY,       /* Defines away GPIO IO that does not exist */
  NUM_GPIO             /* # GPIOs registers in enum */
} bio_gpio_type;

/*-------------------------------------------------------------------------*/
#include BIO_MB_H


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* GPIO access info */
typedef const struct bio_gpio_access_s {
  const  dword   out_addr;    /* GPIO output address */
  const  dword   in_addr;     /* GPIO input address */
  const  dword   tsen_addr;   /* GPIO tristate control port address */
} bio_gpio_access_type;

/* GPIO shadow buffer image */
typedef struct {
  dword          out_buf;     /* GPIO OUT  port shadow buffer */
  dword          ts_buf;      /* GPIO TSEN port shadow buffer */
} bio_gpio_image_type;

extern bio_gpio_access_type   bio_gpio[];

extern bio_gpio_image_type    bio_gpio_image[NUM_GPIO];
extern bio_gpio_access_type  *bio_get_gpio_access ( word idx );
extern bio_gpio_image_type   *bio_get_gpio_image ( word idx );


/*===========================================================================
MACRO BIO_OUT

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to a
  specified port.  Only the bits corresponding to the mask are actually
  affected; other bits retain their previous values.  To do this, an image is
  maintained of the previous value written to the port which is combined with
  the new value prior to writing.

PARAMETERS
  io    Basic I/O port defined by this header
  mask  Mask of bits in the port to write
  val   Value to write to the masked bits in the port

DEPENDENCIES
  If the port needs to be tri-stated to allow output, it must be
  tri-stated before the output will be effected by this macro call.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_OUT(io, mask, val) \
  INTLOCK();\
  bio_gpio_image[io].out_buf = (bio_gpio_image[io].out_buf & ~(mask)) \
                                    | ((mask) & (val)); \
  outpdw(bio_gpio[io].out_addr, bio_gpio_image[io].out_buf); \
  INTFREE();


/*===========================================================================
MACRO BIO_TRISTATE

DESCRIPTION
  This macro tri-states specified bits in a port, qualified by a specified
  mask, of a specified port.  Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values.  To do this,
  an image is maintained of the previous value written to the port which is
  combined with the new value prior to writing.  Writing a bit to 1 enables
  it as an output, writing a 0 enables it as an input.

PARAMETERS
  io    Basic I/O port defined by this header
  mask  Mask of bits in the port to tri-state enable
  val   Value to write to the masked bits in the port to tri-state enable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_TRISTATE(io, mask, val) \
  INTLOCK();\
  bio_gpio_image[io].ts_buf = (bio_gpio_image[io].ts_buf & ~(mask)) \
                                  | ((mask) & (val)); \
  outpdw(bio_gpio[io].tsen_addr, bio_gpio_image[io].ts_buf); \
  INTFREE();

/*===========================================================================
MACRO BIO_UPDATE_TRISTATE_SHADOW

DESCRIPTION
  This macro updates the shadow registers for the specified tristate bits
  Only the bits corresponding to the mask are actually affected; other bits
  retain their previous values.

  PARAMETERS
  io    Basic I/O port defined by this header
  mask  Mask of bits in the port to tri-state enable
  val   Value to write to the masked bits in the port to tri-state enable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_UPDATE_TRISTATE_SHADOW(io, val) \
  INTLOCK();\
  bio_gpio_image[io].ts_buf =  (val); \
  INTFREE();

/* Enable/Disable a GPIO as an output.  GPIO's can always be read as inputs.
*/
#define BIO_OUTPUT_ENA_V    0xffffffff
#define BIO_OUTPUT_DIS_V    0x00000000

#define BIO_OUTPUT_SET_V    0xffffffff
#define BIO_OUTPUT_CLR_V    0x00000000



/*===========================================================================
MACRO BIO_UPDATE_OUTBUFFER_SHADOW

DESCRIPTION
  This macro updates the shadow registers for the specified output bits
  Only the bits corresponding to the mask are actually affected; other bits
  retain their previous values.

PARAMETERS
  io    Basic I/O port defined by this header
  mask  Mask of bits in the port to write
  val   Value to write to the masked bits in the port

DEPENDENCIES
  If the port needs to be tri-stated to allow output, it must be
  tri-stated before the output will be effected by this macro call.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_UPDATE_OUTBUFFER_SHADOW(io, val) \
  INTLOCK();\
  bio_gpio_image[io].out_buf =  (val); \
  INTFREE();


/*===========================================================================
MACRO BIO_INM

DESCRIPTION
  This macro reads a value from a specified Basic I/O port address and then
  applies a specified mask to the value.  Bits not corresponding to the mask
  are returned as zero.

PARAMETERS
  io    Basic I/O port defined by this header
  mask  Mask of bits in the port to read

DEPENDENCIES
  Note that we are always performing 32 bit reads.  This is dangerous if
  the register being read is less than 32 bits.  We are protected, however,
  by the mask passed in.  If the mask is incorrect, however, and is too wide
  for the register, the value returned is UNDEFINED.

RETURN VALUE
  Value read from the I/O register AND'd with 'mask'.

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_INM(io, mask)     ((mask) &  inpdw(bio_gpio[io].in_addr))


/*===========================================================================
MACRO BIO_GET_SW

DESCRIPTION
  Get the current value of the specified switches.

PARAMETERS
  switches   Mask of switches to report the value of, use the BIO_SWn_M
             values defined above.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*  Note: when the switch is in the "on" position, it reads back as logic 0.
**  so we invert it here to get logical 1's instead.
*/
#define BIO_GET_SW(switches)    ( bio_switches & (switches) )

typedef word bio_sw_type;
extern bio_sw_type bio_switches;


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
extern void bio_set_sw
(
  bio_sw_type switches
    /* Value to set the DIP switches to */
);

/*===========================================================================
MACRO BIO_IN

DESCRIPTION
  This macro reads a value from a specified GPIO input register address.

PARAMETERS
  io    GPIO register address table index

DEPENDENCIES
  Address activated registers will not activate if CHIPX8 is off.

RETURN VALUE
  32-bit read from the GPIO input register.

SIDE EFFECTS
  None

===========================================================================*/
#define BIO_IN(io) (inpdw(bio_gpio[io].in_addr))

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
void osbl_save_bio_shared_info(void);
#endif /* BUILD_BOOT_CHAIN */

#endif /* BIO_H */
