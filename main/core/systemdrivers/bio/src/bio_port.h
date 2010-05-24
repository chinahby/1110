#ifndef BIO_PORT_H
#define BIO_PORT_H

/*===========================================================================

                   B I T   I / O   D E C L A R A T I O N S

DESCRIPTION
  This file contains definitions to facilitate interaction with the DMSS
  GPIO and Miscellaneous Bit I/O ports.  This module is designed to provide
  appropriate buffering and control for these shared GPIO ports.

Copyright (c) 1992, 1993 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/bio.h_v   1.3   30 Nov 2001 11:03:14   dbrackma  $
  $Header: //depot/asic/qsc1100/drivers/bio/bio_port.h#1 $ $DateTime: 2007/09/19 14:17:27 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/18/07   cr      Put BIO LED output on second LED port.
01/15/07   fs      Ported for QSC6055.
06/18/05   gr      Mods for L4.
10/15/04   ts      Ported for MSM6800 from MSM6550
04/15/04   bmg     Ported for MSM6550 from MSM6500
===========================================================================*/

#include "comdef.h"     /* Definitions for ROM, etc.                       */
#include "target.h"     /* Target specific definitions                     */
#include "msm.h"


/*-------------------------------------------------------------------------*/

/* GPIO types. Use these names to define registers in bio?.h files. */
#define GPIO_REG_4    GPIO_REG_DUMMY
#define ENC_GPIO_0    GPIO_REG_DUMMY
#define ENC_GPIO_1    GPIO_REG_DUMMY
#define DMOD_GPIO_INT GPIO_REG_DUMMY
#define DEC_GPIO_0    GPIO_REG_DUMMY
#define DEC_GPIO_1    GPIO_REG_DUMMY
#define DMOD_GPIO     GPIO_REG_DUMMY

/*-------------------------------------------------------------------------*/

#include BIO_MB_H


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define BIO_CLR_LED( led )
#define BIO_SET_LED( led )
#define BIO_TOG_LED( led )


#ifndef BIO_SW1_M

#define BIO_SW1_M       0x80
#define BIO_SW2_M       0x40
#define BIO_SW3_M       0x20
#define BIO_SW4_M       0x10
#define BIO_SW5_M       0x08
#define BIO_SW6_M       0x04
#define BIO_SW7_M       0x02
#define BIO_SW8_M       0x01

#define BIO_SW9_M       0x0100
#define BIO_SW10_M      0x0200
#define BIO_SW11_M      0x0400
#define BIO_SW12_M      0x0800
#define BIO_SW13_M      0x1000
#define BIO_SW14_M      0x2000
#define BIO_SW15_M      0x4000
#define BIO_SW16_M      0x8000

#endif
#endif /* BIO_PORT_H */


