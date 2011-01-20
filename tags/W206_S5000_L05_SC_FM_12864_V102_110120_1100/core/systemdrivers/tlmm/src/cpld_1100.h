#ifndef FPGA_1100_H
#define FPGA_1100_H
/*===========================================================================

               SURF QSC1100   F P G A   H E A D E R    F I L E

DESCRIPTION
  This file contains definitions of the FPGA functions of the SURF QSC1100
  main board.

  Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/qsc1100/drivers/tlmm/cpld_1100.h#1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/07   vph     Created from QSC60x5.  Temporary port for compiling.
                   Need to clean up. 

===========================================================================*/

#include "target.h"
#include "msm.h"

#ifdef T_SURF


/* --------------------------------------------------------------------------
            Register/mask definitions for FPGA registers on SURF
---------------------------------------------------------------------------*/

/* Notifies other SW modules that the debug LED port is available */
#define FPGA_BASE                                          GP1_BASE

/* Configuration registers - show the state of the
   DIP switches on the main board */

#define FPGA_GPIO_IN                                       (FPGA_BASE + 0x30006)
#define FPGA_GPIO_IN__UI_JS_WEST_IN_MASK                         0x0001
#define FPGA_GPIO_IN__UI_JS_EAST_IN_MASK                         0x0002
#define FPGA_GPIO_IN__UI_JS_SOUTH_IN_MASK                        0x0004
#define FPGA_GPIO_IN__UI_JS_NORTH_IN_MASK                        0x0008
#define FPGA_GPIO_IN__UI_JS_CENTER_IN_MASK                       0x0010
#define FPGA_GPIO_IN__TS_CLK_IN_MASK                             0x0020
#define FPGA_GPIO_IN__TS_CS_N_IN_MASK                            0x0040
#define FPGA_GPIO_IN__TS_DOUT_IN_MASK                            0x0080
#define FPGA_GPIO_IN__TS_DIN_IN_MASK                             0x0100
#define FPGA_GPIO_IN__TS_IRQ_IN_MASK                             0x0200
#define FPGA_GPIO_IN__KYPD_EXPAND_MASK                           0x0400
#define FPGA_GPIO_IN__FLIP_OPEN_MASK                             0x0800
#define FPGA_GPIO_IN__GP_SW1_MASK                                0x1000
#define FPGA_GPIO_IN__PWR_ON_SW_MASK                             0x2000


#define FPGA_GPIO_OUT                                      (FPGA_BASE + 0x30008)
#define FPGA_GPIO_OUT__UI_JS_WEST_OUT_MASK                       0x0001
#define FPGA_GPIO_OUT__UI_JS_WEST_OUT_LO_VAL                     0x0000
#define FPGA_GPIO_OUT__UI_JS_WEST_OUT_HI_VAL                     0x0001

#define FPGA_GPIO_OUT__UI_JS_EAST_OUT_MASK                       0x0002
#define FPGA_GPIO_OUT__UI_JS_EAST_OUT_LO_VAL                     0x0000
#define FPGA_GPIO_OUT__UI_JS_EAST_OUT_HI_VAL                     0x0002

#define FPGA_GPIO_OUT__UI_JS_SOUTH_OUT_MASK                      0x0004
#define FPGA_GPIO_OUT__UI_JS_SOUTH_OUT_LO_VAL                    0x0000
#define FPGA_GPIO_OUT__UI_JS_SOUTH_OUT_HI_VAL                    0x0004

#define FPGA_GPIO_OUT__UI_JS_NORTH_OUT_MASK                      0x0008
#define FPGA_GPIO_OUT__UI_JS_NORTH_OUT_LO_VAL                    0x0000
#define FPGA_GPIO_OUT__UI_JS_NORTH_OUT_HI_VAL                    0x0008

#define FPGA_GPIO_OUT__UI_JS_CENTER_OUT_MASK                     0x0010
#define FPGA_GPIO_OUT__UI_JS_CENTER_OUT_LO_VAL                   0x0000
#define FPGA_GPIO_OUT__UI_JS_CENTER_OUT_HI_VAL                   0x0010

#define FPGA_GPIO_OUT__TS_CLK_OUT_MASK                           0x0020
#define FPGA_GPIO_OUT__TS_CLK_OUT_LO_VAL                         0x0000
#define FPGA_GPIO_OUT__TS_CLK_OUT_HI_VAL                         0x0020

#define FPGA_GPIO_OUT__TS_CS_N_OUT_MASK                          0x0040
#define FPGA_GPIO_OUT__TS_CS_N_OUT_LO_VAL                        0x0000
#define FPGA_GPIO_OUT__TS_CS_N_OUT_HI_VAL                        0x0040

#define FPGA_GPIO_OUT__TS_DOUT_OUT_MASK                          0x0080
#define FPGA_GPIO_OUT__TS_DOUT_OUT_LO_VAL                        0x0000
#define FPGA_GPIO_OUT__TS_DOUT_OUT_HI_VAL                        0x0080

#define FPGA_GPIO_OUT__TS_DIN_OUT_MASK                           0x0100
#define FPGA_GPIO_OUT__TS_DIN_OUT_LO_VAL                         0x0000
#define FPGA_GPIO_OUT__TS_DIN_OUT_HI_VAL                         0x0100

#define FPGA_GPIO_OUT__TS_IRQ_OUT_MASK                           0x0200
#define FPGA_GPIO_OUT__TS_IRQ_OUT_LO_VAL                         0x0000
#define FPGA_GPIO_OUT__TS_IRQ_OUT_HI_VAL                         0x0200

#define FPGA_GPIO_OUT__KYPD_EXPAND_MASK                          0x0400
#define FPGA_GPIO_OUT__KYPD_EXPAND_LO_VAL                        0x0000
#define FPGA_GPIO_OUT__KYPD_EXPAND_HI_VAL                        0x0400

#define FPGA_GPIO_OUT__I2C_CE_MASK                               0x8000
#define FPGA_GPIO_OUT__I2C_CE_LO_VAL                             0x0000
#define FPGA_GPIO_OUT__I2C_CE_HI_VAL                             0x8000

#define FPGA_GPIO_OE                                       (FPGA_BASE + 0x3000A)

#define FPGA_GPIO_OE__I2C_CE_MASK                                0x8000
#define FPGA_GPIO_OE__I2C_CE_LO_VAL                              0x0000
#define FPGA_GPIO_OE__I2C_CE_HI_VAL                              0x8000

/* --------------------------------------------------------------------------
                               Macros
---------------------------------------------------------------------------*/

/* Reads a FPGA bit to ensure it is set (1) */
#define FPGA_IS_EN(reg, mask) \
          (reg##__##mask == ( inpw(reg) & reg##__##mask ))

/* Reads a FPGA register bit mask */
#define FPGA_INM(reg, mask) \
          ( inpw(reg) & reg##__##mask##_MASK )

/* Writes a FPGA register bit mask */
#define FPGA_OUTM(reg, mask, val) \
          INTLOCK(); \
          outpw(reg, (inpw(reg) & ~(reg##__##mask##_MASK)) \
                | (reg##__##mask##_MASK & reg##__##val##_VAL )); \
          INTFREE();

/* Example
     return (FPGA_INM(FPGA_GPIO_IN, PWR_ON_SW) == 0);   to read
     FPGA_OUTM(FPGA_GPIO_OE, I2C_CE, I2C_CE_HI);        to write 1
     FPGA_OUTM(FPGA_GPIO_OE, I2C_CE, I2C_CE_LO);        to write 0
     
     
*/     
                                   
#endif /* T_SURF */
#endif /* FPGA_1100_H */

