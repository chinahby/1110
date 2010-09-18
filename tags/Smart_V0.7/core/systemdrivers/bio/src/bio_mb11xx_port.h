#ifndef BIO_MB11XX_PORT_H
#define BIO_MB11XX_PORT_H
/*===========================================================================

              S U R F 6 8 0 0   B I O   H E A D E R    F I L E

DESCRIPTION
  This file contains definitions of the bits of the SURF6800 main board for
  the BIO services.  These bits are the control lines which can be read
  or written using the BIO services.

  Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/bio/bio_mb11xx_port.h#2 $
$DateTime: 2007/12/28 05:23:00 $
$Author: aksingh $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/07    cr     Port TCXO_EN.
10/14/04   ts      Ported for MSM6800 from MSM6550
09/02/04   jkl     Updated FFA settings.
08/30/04   jkl     Updated HEADSET detection.
08/17/04   trc     Added Slingshot GPIO support
08/11/04   jkl     Clean up code.
04/15/04   bmg     Ported for MSM6550 from MSM6500
===========================================================================*/
#ifndef _ARM_ASM_
 #include "tlmm.h"
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO REGISTER 0
---------------------------------------------------------------------------*/


#ifdef FEATURE_GSBI0_I2C
#define BIO_EEPROM_REG        GPIO_REG_0  /* GPIO 12,13 :GSBI0             */
#define BIO_EEPROM_DATA_M     0x00001000  /* Muxed  : I2C_SDA              */
#define BIO_EEPROM_CLK_M      0x00002000  /* Muxed  : I2C_SCL              */
#endif

#ifdef FEATURE_GSBI1_I2C
#define BIO_EEPROM_REG        GPIO_REG_0  /* GPIO 16/17 :GSBI1             */
#define BIO_EEPROM_DATA_M     0x00010000  /* Muxed  : I2C_SDA              */
#define BIO_EEPROM_CLK_M      0x00020000  /* Muxed  : I2C_SCL              */
#endif

#ifdef FEATURE_GSBI2_I2C
#define BIO_EEPROM_REG        GPIO_REG_0  /* GPIO 20/21:GSBI2              */
#define BIO_EEPROM_DATA_M     0x00100000  /* Muxed  : I2C_SDA              */
#define BIO_EEPROM_CLK_M      0x00200000  /* Muxed  : I2C_SCL              */
#endif

/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO REGISTER 1
---------------------------------------------------------------------------*/

#if !defined FEATURE_GSBI0_I2C && !defined FEATURE_GSBI1_I2C &&  !defined FEATURE_GSBI2_I2C

/* This is the fourth hanging i2cc outside GSBI wrap block */

#define BIO_EEPROM_REG        GPIO_REG_1  /* GPIO 39,40: Hanging I2CC      */
#define BIO_EEPROM_DATA_M     0x00000080  /* Muxed  : I2C_SDA              */
#define BIO_EEPROM_CLK_M      0x00000100  /* Muxed  : I2C_SCL              */
#endif


/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO REGISTER 2
---------------------------------------------------------------------------*/
#define BIO_TCXO_EN_REG       GPIO_REG_2  /* GPIO 25:                      */
#define BIO_TCXO_EN_M         0x02000000  /* Muxed  : GP_PDM_2             */
#define BIO_TCXO_EN_HI_V      0x02000000  /* Muxed  : DBG_BUS_OUT_15       */
#define BIO_TCXO_EN_LO_V      0x00000000  /* Muxed  : DBG_BUS_IN_24        */
                                          /* Muxed  : TCXO_EN_N            */

/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO REGISTER 3
---------------------------------------------------------------------------*/




/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR GPIO_KEYSENSE
---------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------
                 BIT I/O MASKS FOR EXTENDED GPIO
---------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------
                          Special cases

In certain TLMM configurations, some code modules require GPIOs to be
available (even though they won't be used) in order for the build to
compile.  As such, we provide dummy GPIO's here, even though we understand
that they won't be (and should not be) used.
---------------------------------------------------------------------------*/
/* Call LED pin. This is no longer used */

#define BIO_CALL_LED_REG      GPIO_REG_DUMMY
#define BIO_CALL_LED_M        0x00000000      /* GPIO 45: Call LED             */
#define BIO_CALL_LED_ON_V     0x00000000
#define BIO_CALL_LED_OFF_V    0x00000000

#define BIO_DCD_REG                   GPIO_REG_DUMMY
#define BIO_DCD_M                     0x0001
#define BIO_DCD_ON_V                  0x0000
#define BIO_DCD_OFF_V                 0x0001

#define BIO_RI_REG                    GPIO_REG_DUMMY
#define BIO_RI_O_25_M                 0x0000
#define BIO_RI_ON_V                   0x0000
#define BIO_RI_OFF_V                  0x0000

#define BIO_DTR_REG                   GPIO_REG_DUMMY
#define BIO_DTR_M                     0x0020    
#define BIO_DTR_ON_V                  0x0020    
#define BIO_DTR_OFF_V                 0x0000
#define BIO_DTR_GPIO_INT              44        

#define BIO_USB_SUSPEND_REG           GPIO_REG_0
#define BIO_USB_SUSPEND_M             0x00020000
#define BIO_USB_SUSPEND_ON_V          0x00020000
#define BIO_USB_SUSPEND_OFF_V         0x00000000

#define BIO_PS_HOLD_REG               GPIO_REG_DUMMY
#define BIO_PS_HOLD_M                 0x0008
#define BIO_PS_HOLD_ON_V              0x0008
#define BIO_PS_HOLD_OFF_V             0x0000

#endif /* BIO_MB11XX_PORT_H */
