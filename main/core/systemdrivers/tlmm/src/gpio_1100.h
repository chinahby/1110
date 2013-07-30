#ifndef GPIO_1100_H
#define GPIO_1100_H
/*===========================================================================

        G P I O   D E F I N I T I O N S   F O R   Q S C 1 1 0 0

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the GPIO TLMM functions for the QSC1100.

REFERENCES

Copyright (c) 2008  by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tlmm/gpio_1100.h#17 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/09   kma     Provided PULL_UP, NO_PULL, PULL_DOWN ALT Functions for UIM Pins.
                   UIM S/W will take care of setting the PULL accordingly.
02/19/09   hvo     Fixed RUIM leakage current
09/07/08   hvo     Added support for Power Consumption
06/25/07   vph     Ported for QSC1100 from QSC60X5.
===========================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"

/* ----------------------------------------------------------------------------
* GPIO Group
------------------------------------------------------------------------------*/
typedef enum
{
  GROUP_GPIO_0 = 0,
  GROUP_GPIO_1,
  GROUP_GPIO2_0,
  GROUP_GPIO2_1
} GPIO_GroupType;

/* ----------------------------------------------------------------------------
* The starting GPIO number for each GPIO group
------------------------------------------------------------------------------*/
#define GPIO_GROUP0_START       0
#define GPIO_GROUP1_START       32
#define GPIO_GROUP2_START       0 
#define GPIO_GROUP3_START       32
#define GPIO_NUM_GPIOS          63

/* ----------------------------------------------------------------------------
* The enumeration for GPIO tristate enable type.
------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_TRISTATE_DISABLE  = 0,
  GPIO_TRISTATE_ENABLE = 1
} GPIO_TristateType;

/* ----------------------------------------------------------------------------
* The enumeration for GPIO value(high or low) type.
------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_LOW_VALUE  = 0,
  GPIO_HIGH_VALUE = 1
} GPIO_ValueType;

/* ----------------------------------------------------------------------------
* The enumeration for GPIO pull-up,pull-down type.
------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_NO_PULL	  = 0,
  GPIO_PULL_DOWN  = 0x1,
  GPIO_PULL_UP    = 0x3
} GPIO_PullupPulldownType;

/* ----------------------------------------------------------------------------
* The enumeration for GPIO polarity type.
------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_INVALID_POLARITY = 0,
  GPIO_INPUT ,
  GPIO_OUTPUT,
  GPIO_ALTERNATE
} GPIO_PolarityType;

/* ----------------------------------------------------------------------------
* GPIO Function Select
------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_PRI_FUNC = 0,
  GPIO_ALT_FUNC_1,
  GPIO_ALT_FUNC_2,
  GPIO_ALT_FUNC_3,
  GPIO_ALT_FUNC_4,
  GPIO_ALT_FUNC_5
} GPIO_AltFuncType;

/* ----------------------------------------------------------------------------
* The masks used for accessing various fields from the GPIO signal value.
------------------------------------------------------------------------------*/
#define GPIO_SIGNAL_MASK       0x0000000f
#define GPIO_SIGNAL_POSITION   0
#define GPIO_REGISTER_MASK     0x000000f0
#define GPIO_REGISTER_POSITION 4
#define GPIO_NUMBER_MASK       0x0000ff00
#define GPIO_NUMBER_POSITION   8
#define GPIO_POLARITY_MASK     0x00030000
#define GPIO_POLARITY_POSITION 16
#define GPIO_PULL_MASK         0x000C0000
#define GPIO_PULL_POSITION     18

/* ----------------------------------------------------------------------------
* Signal enum decoders
------------------------------------------------------------------------------*/
#define GPIO_CONFIG(number,group,select,polarity,pull)						  \
            ((((number) << GPIO_NUMBER_POSITION) & GPIO_NUMBER_MASK)          \
    		   | (((group) << GPIO_REGISTER_POSITION) & GPIO_REGISTER_MASK)       \
           | (((select) << GPIO_SIGNAL_POSITION) & GPIO_SIGNAL_MASK)          \
           | (((polarity) << GPIO_POLARITY_POSITION) & GPIO_POLARITY_MASK) \
           | (((pull) << GPIO_PULL_POSITION) & GPIO_PULL_MASK))
#define GPIO_NUMBER(gpio_signal) ((gpio_signal & GPIO_NUMBER_MASK)            \
                                     >> GPIO_NUMBER_POSITION)
#define GPIO_GROUP(gpio_signal) ((gpio_signal & GPIO_REGISTER_MASK)           \
                                     >> GPIO_REGISTER_POSITION)
#define GPIO_SIGNAL(gpio_signal) ((gpio_signal & GPIO_SIGNAL_MASK)            \
                                     >> GPIO_SIGNAL_POSITION)
#define GPIO_POLARITY(gpio_signal) ((gpio_signal & GPIO_POLARITY_MASK)        \
                                     >> GPIO_POLARITY_POSITION)
#define GPIO_PULL(gpio_signal) ((gpio_signal & GPIO_PULL_MASK)                \
                                     >> GPIO_PULL_POSITION)

#define GPIO_OUT(number,group)                                                \
          (GPIO_CONFIG((number), (group), 0, GPIO_OUTPUT, GPIO_NO_PULL))
#define GPIO_OUP(number,group,pull)                                           \
          (GPIO_CONFIG((number), (group), 0, GPIO_OUTPUT, (pull)))
#define GPIO_IN(number,group,pull)                                            \
          (GPIO_CONFIG((number), (group), 0, GPIO_INPUT, (pull)))
#define GPIO_ALT(number,group,alternate,pull)                                 \
          (GPIO_CONFIG((number), (group), (alternate), GPIO_ALTERNATE, (pull)))

/*
** This enum contains the QSC1100 Software Manual names
** of the GPIO pins and their alternate functions (except for
** the GPIO_OUTPUT_* and GPIO_INPUT_*).  Do not change this enum, if you
** are adding a new ALIAS for a GPIO, #define the symbol at the end
** of the file to the name in this enum.
*/

typedef enum
{
  /* GPIO 0 */
  GPIO_OUTPUT_0         = GPIO_OUT(0,GROUP_GPIO2_0),
  GPIO_INPUT_0          = GPIO_IN (0,GROUP_GPIO2_0,GPIO_PULL_UP),
  KYPD_9                = GPIO_OUP(0,GROUP_GPIO2_0, GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_0 but with PULL_UP */
  DBG_BUS_OUT_0         = GPIO_ALT(0,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_IN_0          = GPIO_ALT(0,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),

  /* GPIO 1 */
  GPIO_OUTPUT_1         = GPIO_OUT(1,GROUP_GPIO2_0),
  GPIO_INPUT_1          = GPIO_IN (1,GROUP_GPIO2_0,GPIO_PULL_UP),
  KYPD_11               = GPIO_OUP(1,GROUP_GPIO2_0,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_1 but with PULL_UP */
  DBG_BUS_OUT_1         = GPIO_ALT(1,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_IN_1          = GPIO_ALT(1,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),

  /* GPIO 2 */
  GPIO_OUTPUT_2         = GPIO_OUT(2,GROUP_GPIO2_0),
  GPIO_INPUT_2          = GPIO_IN (2,GROUP_GPIO2_0,GPIO_PULL_UP),
  KYPD_15               = GPIO_OUP(2,GROUP_GPIO2_0,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_2 but with PULL_UP */
  DBG_BUS_OUT_2         = GPIO_ALT(2,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_IN_2          = GPIO_ALT(2,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),

  /* GPIO 3 */
  GPIO_OUTPUT_3         = GPIO_OUT(3,GROUP_GPIO2_0),
  GPIO_INPUT_3          = GPIO_IN (3,GROUP_GPIO2_0,GPIO_PULL_UP),
  KYPD_17               = GPIO_OUP(3,GROUP_GPIO2_0,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_3 but with PULL_UP */
  PM_CLK_0              = GPIO_ALT(3,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_3         = GPIO_ALT(3,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_BUS_IN_3          = GPIO_ALT(3,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),

  /* GPIO 4 */
  GPIO_OUTPUT_4         = GPIO_OUT(4,GROUP_GPIO2_0),
  GPIO_INPUT_4          = GPIO_IN (4,GROUP_GPIO2_0,GPIO_PULL_UP),
  KYPD_MEMO             = GPIO_OUP(4,GROUP_GPIO2_0,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_4 but with PULL_UP */
  PM_CLK_1              = GPIO_ALT(4,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_4         = GPIO_ALT(4,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_BUS_IN_4          = GPIO_ALT(4,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),

  /* GPIO 5 */
  GPIO_OUTPUT_5         = GPIO_OUT(5,GROUP_GPIO2_0),
  GPIO_INPUT_5          = GPIO_IN (5,GROUP_GPIO2_0,GPIO_PULL_UP),
  KEYSENSE_4_N          = GPIO_IN (5,GROUP_GPIO2_0,GPIO_PULL_UP),
  PM_CLK_2              = GPIO_ALT(5,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  MODEM_DBG_CLK_ALT     = GPIO_ALT(5,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_CLK_ALT           = GPIO_ALT(5,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),
  PS_HOLD               = GPIO_ALT(5,GROUP_GPIO2_0,GPIO_ALT_FUNC_4,GPIO_PULL_UP),

  /* GPIO 6 */
  GPIO_OUTPUT_6         = GPIO_OUT(6,GROUP_GPIO2_0),
  GPIO_INPUT_6          = GPIO_IN (6,GROUP_GPIO2_0,GPIO_PULL_UP),
  KEYSENSE_3_N          = GPIO_IN (6,GROUP_GPIO2_0,GPIO_PULL_UP),
  PM_CLK_3              = GPIO_ALT(6,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_SSBI_MS           = GPIO_ALT(6,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),

  /* GPIO 7 */
  GPIO_OUTPUT_7         = GPIO_OUT(7,GROUP_GPIO2_0),
  GPIO_INPUT_7          = GPIO_IN (7,GROUP_GPIO2_0,GPIO_PULL_UP),
  KEYSENSE_2_N          = GPIO_IN (7,GROUP_GPIO2_0,GPIO_PULL_UP),
  PM_CLK_4              = GPIO_ALT(7,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_AD_SSBI_RX        = GPIO_ALT(7,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  SSBI_RX_OE            = GPIO_ALT(7,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),

  /* GPIO 8 */
  GPIO_OUTPUT_8         = GPIO_OUT(8,GROUP_GPIO2_0),
  GPIO_INPUT_8          = GPIO_IN (8,GROUP_GPIO2_0,GPIO_PULL_UP),
  KEYSENSE_1_N          = GPIO_IN (8,GROUP_GPIO2_0,GPIO_PULL_UP),
  EXT_CLK2              = GPIO_ALT(8,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_AD_SSBI_TX        = GPIO_ALT(8,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  SSBI_TX_OE            = GPIO_ALT(8,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),

  /* GPIO 9 */
  GPIO_OUTPUT_9         = GPIO_OUT(9,GROUP_GPIO2_0),
  GPIO_INPUT_9          = GPIO_IN (9,GROUP_GPIO2_0,GPIO_PULL_UP),
  KEYSENSE_0_N          = GPIO_IN (9,GROUP_GPIO2_0,GPIO_PULL_UP),
  EXT_SLEEP_CLK         = GPIO_ALT(9,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_PM_SSBI           = GPIO_ALT(9,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  PM_SSBI_OBS           = GPIO_ALT(9,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_UP),
  PM_SSBI_OE            = GPIO_ALT(9,GROUP_GPIO2_0,GPIO_ALT_FUNC_4,GPIO_PULL_UP),

  /* GPIO 10 */
  GPIO_OUTPUT_10        = GPIO_OUT(10,GROUP_GPIO2_0),
  GPIO_INPUT_10         = GPIO_IN (10,GROUP_GPIO2_0,GPIO_PULL_DOWN),
  BACKLIGHT_EN          = GPIO_OUT(10,GROUP_GPIO2_0),                                 // Need to check
  SD_PWR_EN_N           = GPIO_OUT(10,GROUP_GPIO2_0),                                 // Need to check
  GP_PDM                = GPIO_ALT(10,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  DBG_BUS_OUT_5         = GPIO_ALT(10,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_5          = GPIO_ALT(10,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  #ifdef FEATURE_VERSION_K202
  GPIO_10               = GPIO_OUP(10,GROUP_GPIO2_0,GPIO_PULL_DOWN),
  #endif

  /* GPIO 11 */
  GPIO_OUTPUT_11        = GPIO_OUT(11,GROUP_GPIO2_0),
  GPIO_INPUT_11         = GPIO_IN (11,GROUP_GPIO2_0,GPIO_PULL_UP),
  HEADSET_DET_N         = GPIO_IN (11,GROUP_GPIO2_0,GPIO_PULL_UP),

  /* GPIO 12 */
  GPIO_OUTPUT_12        = GPIO_OUT(12,GROUP_GPIO_0),
  GPIO_INPUT_12         = GPIO_IN (12,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI0_0               = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  UIM_CLK               = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  UIM_CLK_PD            = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  UIM_CLK_PU            = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  UIM_CLK_NP            = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_NO_PULL),
  USB_SE0_VM            = GPIO_ALT(12,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),

  /* GPIO 13 */
  GPIO_OUTPUT_13        = GPIO_OUT(13,GROUP_GPIO2_0),
  GPIO_INPUT_13         = GPIO_IN (13,GROUP_GPIO2_0,GPIO_PULL_DOWN),
  GSBI0_1               = GPIO_ALT(13,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  BT_PWR_EN					= GPIO_OUT(13,GROUP_GPIO2_0),												// Output, No Pull
  EXT_CLK1              = GPIO_ALT(13,GROUP_GPIO2_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  GPIO_TX_AGC_ADJ       = GPIO_ALT(13,GROUP_GPIO2_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  USB_OE_INT_N          = GPIO_ALT(13,GROUP_GPIO2_0,GPIO_ALT_FUNC_4,GPIO_PULL_DOWN),

  /* GPIO 14 */
  GPIO_OUTPUT_14        = GPIO_OUT(14,GROUP_GPIO_0),
  GPIO_INPUT_14         = GPIO_IN (14,GROUP_GPIO_0,GPIO_PULL_DOWN),
  UIM_RESET             = GPIO_OUT(14,GROUP_GPIO_0),
  UIM_RESET_PD          = GPIO_ALT(14,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  UIM_RESET_PU          = GPIO_ALT(14,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  UIM_RESET_NP          = GPIO_ALT(14,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_NO_PULL),
  GSBI0_2               = GPIO_ALT(14,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  USB_DAT_VP            = GPIO_ALT(14,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),

  /* GPIO 15 */
  GPIO_OUTPUT_15        = GPIO_OUT(15,GROUP_GPIO_0),
  GPIO_INPUT_15         = GPIO_IN (15,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI0_3               = GPIO_ALT(15,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  UIM_DATA              = GPIO_ALT(15,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  UIM_DATA_PD           = GPIO_ALT(15,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  UIM_DATA_PU           = GPIO_ALT(15,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),      /* Wrap in GSBI */
  UIM_DATA_NP           = GPIO_ALT(15,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_NO_PULL),      /* Wrap in GSBI */

  /* GPIO 16 */
  GPIO_OUTPUT_16        = GPIO_OUT(16,GROUP_GPIO_0),
  GPIO_INPUT_16         = GPIO_IN (16,GROUP_GPIO_0,GPIO_PULL_UP),
  GSBI1_0               = GPIO_ALT(16,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  SPI_CS_N              = GPIO_ALT(16,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),    /* Wrap in GSBI */
  UART1_RFR_N           = GPIO_ALT(16,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),    /* Wrap in GSBI */

  /* GPIO 17 */
  GPIO_OUTPUT_17        = GPIO_OUT(17,GROUP_GPIO_0),
  GPIO_INPUT_17         = GPIO_IN (17,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI1_1               = GPIO_ALT(17,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  SPI_CLK               = GPIO_ALT(17,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  UART1_CTS_N           = GPIO_ALT(17,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */

  /* GPIO 18 */
  GPIO_OUTPUT_18        = GPIO_OUT(18,GROUP_GPIO_0),
  GPIO_INPUT_18         = GPIO_IN (18,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI1_2               = GPIO_ALT(18,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  SPI_DATA_I            = GPIO_ALT(18,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  UART1_RX_DAT          = GPIO_ALT(18,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */

  /* GPIO 19 */
  GPIO_OUTPUT_19        = GPIO_OUT(19,GROUP_GPIO_0),
  GPIO_INPUT_19         = GPIO_IN (19,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI1_3               = GPIO_ALT(19,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  SPI_DATA_O            = GPIO_ALT(19,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  UART1_TX_DAT          = GPIO_ALT(19,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */

  /* GPIO 20 */
  GPIO_OUTPUT_20        = GPIO_OUT(20,GROUP_GPIO_0),
  GPIO_INPUT_20         = GPIO_IN (20,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI2_0               = GPIO_ALT(20,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  AUX_PCM_CLK           = GPIO_ALT(20,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  DBG_BUS_OUT_6         = GPIO_ALT(20,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_6          = GPIO_ALT(20,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),

  /* GPIO 21 */
  GPIO_OUTPUT_21        = GPIO_OUT(21,GROUP_GPIO_0),
  GPIO_INPUT_21         = GPIO_IN (21,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI2_1               = GPIO_ALT(21,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  AUX_PCM_SYNC          = GPIO_ALT(21,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */

  /* GPIO 22 */
  GPIO_OUTPUT_22        = GPIO_OUT(22,GROUP_GPIO_0),
  GPIO_INPUT_22         = GPIO_IN (22,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI2_2               = GPIO_ALT(22,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  AUX_PCM_DIN           = GPIO_ALT(22,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  DBG_BUS_OUT_8         = GPIO_ALT(22,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_8          = GPIO_ALT(22,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),

  /* GPIO 23 */
  GPIO_OUTPUT_23        = GPIO_OUT(23,GROUP_GPIO_0),
  GPIO_INPUT_23         = GPIO_IN (23,GROUP_GPIO_0,GPIO_PULL_DOWN),
  GSBI2_3               = GPIO_ALT(23,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  AUX_PCM_DOUT          = GPIO_ALT(23,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),    /* Wrap in GSBI */
  DBG_BUS_OUT_9         = GPIO_ALT(23,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_9          = GPIO_ALT(23,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),

  /* GPIO 24 */
  GPIO_OUTPUT_24        = GPIO_OUT(24,GROUP_GPIO_0),
  GPIO_INPUT_24         = GPIO_IN (24,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_CLK              = GPIO_ALT(24,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_10        = GPIO_ALT(24,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_10         = GPIO_ALT(24,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  GPIO_SDCC_CLK         = GPIO_OUP(24,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 25 */
  GPIO_OUTPUT_25        = GPIO_OUT(25,GROUP_GPIO_0),
  GPIO_INPUT_25         = GPIO_IN (25,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_DAT_0            = GPIO_ALT(25,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  UART_TX_DATA          = GPIO_ALT(25,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_OUT_11        = GPIO_ALT(25,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  DBG_BUS_IN_11         = GPIO_ALT(25,GROUP_GPIO_0,GPIO_ALT_FUNC_4,GPIO_PULL_DOWN),
  GPIO_SDCC_DAT_0_OUT   = GPIO_OUP(25,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_SDCC_DAT_0_IN    = GPIO_IN (25,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 26 */
  GPIO_OUTPUT_26        = GPIO_OUT(26,GROUP_GPIO_0),
  GPIO_INPUT_26         = GPIO_IN (26,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_DATOUT_1         = GPIO_ALT(26,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_12        = GPIO_ALT(26,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_12         = GPIO_ALT(26,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  GPIO_SDCC_DATOUT_1_OUT= GPIO_OUP(26,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_SDCC_DATOUT_1_IN = GPIO_IN (26,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 27 */
  GPIO_OUTPUT_27        = GPIO_OUT(27,GROUP_GPIO_0),
  GPIO_INPUT_27         = GPIO_IN (27,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_DATOUT_2         = GPIO_ALT(27,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  UART_RX_DATA          = GPIO_ALT(27,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_OUT_13        = GPIO_ALT(27,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  DBG_BUS_IN_13         = GPIO_ALT(27,GROUP_GPIO_0,GPIO_ALT_FUNC_4,GPIO_PULL_DOWN),
  GPIO_SDCC_DATOUT_2_OUT= GPIO_OUP(27,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_SDCC_DATOUT_2_IN = GPIO_IN (27,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 28 */
  GPIO_OUTPUT_28        = GPIO_OUT(28,GROUP_GPIO_0),
  GPIO_INPUT_28         = GPIO_IN (28,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_DATOUT_3         = GPIO_ALT(28,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_14        = GPIO_ALT(28,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_14         = GPIO_ALT(28,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  GPIO_SDCC_DATOUT_3_OUT= GPIO_OUP(28,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_SDCC_DATOUT_3_IN = GPIO_IN (28,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 29 */
  GPIO_OUTPUT_29        = GPIO_OUT(29,GROUP_GPIO_0),
  GPIO_INPUT_29         = GPIO_IN (29,GROUP_GPIO_0,GPIO_PULL_DOWN),
  SDCC_CMD              = GPIO_ALT(29,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  DBG_BUS_OUT_15        = GPIO_ALT(29,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  DBG_BUS_IN_15         = GPIO_ALT(29,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  GPIO_SDCC_CMD_OUT     = GPIO_OUP(29,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_SDCC_CMD_IN      = GPIO_IN (29,GROUP_GPIO_0,GPIO_PULL_UP),

  /* GPIO 30 */
  #ifdef FEATURE_LCD_TOUCH_ENABLE
  GPIO_OUTPUT_30        = GPIO_OUP(30,GROUP_GPIO_0,GPIO_PULL_UP),
  GPIO_INPUT_30         = GPIO_IN (30,GROUP_GPIO_0,GPIO_PULL_DOWN),
  #else
  GPIO_OUTPUT_30        = GPIO_OUT(30,GROUP_GPIO_0),
  GPIO_INPUT_30         = GPIO_IN (30,GROUP_GPIO_0,GPIO_NO_PULL),
  #endif
  BAND_SEL2             = GPIO_ALT(30,GROUP_GPIO_0,GPIO_PRI_FUNC,GPIO_PULL_DOWN),
  WDOG_DISABLE          = GPIO_ALT(30,GROUP_GPIO_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  XO_EN_N               = GPIO_ALT(30,GROUP_GPIO_0,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  ARM_IRQ               = GPIO_ALT(30,GROUP_GPIO_0,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  ARM_FIQ               = GPIO_ALT(30,GROUP_GPIO_0,GPIO_ALT_FUNC_4,GPIO_PULL_DOWN),
  MSM2_PA_R1            = GPIO_ALT(30,GROUP_GPIO_0,GPIO_ALT_FUNC_4,GPIO_PULL_DOWN),

  /* GPIO 31 */
  #ifdef FEATURE_LCD_TOUCH_ENABLE
  GPIO_OUTPUT_31        = GPIO_OUP(31,GROUP_GPIO2_0,GPIO_PULL_DOWN),
  #else
  GPIO_OUTPUT_31        = GPIO_OUT(31,GROUP_GPIO2_0),
  #endif
  GPIO_INPUT_31         = GPIO_IN (31,GROUP_GPIO2_0,GPIO_PULL_UP),
  FM_INT                = GPIO_ALT(31,GROUP_GPIO2_0,GPIO_PRI_FUNC,GPIO_PULL_DOWN),
  BOOT_FROM_ROM         = GPIO_ALT(31,GROUP_GPIO2_0,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
//#if defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204) 
#if defined (FEATURE_SIM_SEL_GPIO_31_PULL_DOWN)  
#ifdef FEATURE_LCD_TOUCH_ENABLE
  SIM_SEL               = GPIO_OUP(31,GROUP_GPIO2_0,GPIO_PULL_DOWN),
#else
  SIM_SEL               = GPIO_OUP(31,GROUP_GPIO2_0,GPIO_PULL_DOWN),
#endif
#else
#ifdef FEATURE_LCD_TOUCH_ENABLE
  SIM_SEL               = GPIO_OUP(31,GROUP_GPIO2_0,GPIO_PULL_UP),
#endif
#endif

  
  #ifdef FEATURE_LCD_TOUCH_ENABLE
  GPIO_OUTPUT_32        = GPIO_OUP(32,GROUP_GPIO2_1,GPIO_PULL_UP),
  #else
  GPIO_OUTPUT_32        = GPIO_OUT(32,GROUP_GPIO2_1),
  #endif
  GPIO_INPUT_32         = GPIO_IN (32,GROUP_GPIO2_1,GPIO_PULL_UP),
  LCD_FLAG              = GPIO_OUT(32,GROUP_GPIO2_1),
  FAILED_BOOT           = GPIO_ALT(32,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),


 /*  GPIO 33 */
 #if defined (FEATURE_LCD_TOUCH_ENABLE)&&defined(FEATURE_GPIO_LAMP_EN_OUTPUT_33)
	GPIO_OUTPUT_33		  = GPIO_OUP(33,GROUP_GPIO2_1,GPIO_PULL_DOWN),
 #else
	GPIO_OUTPUT_33		  = GPIO_OUT(33,GROUP_GPIO2_1),
 #endif

  GPIO_INPUT_33         = GPIO_IN (33,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  EBI1_A_D_24           = GPIO_ALT(33,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  DBG_CLK               = GPIO_ALT(33,GROUP_GPIO2_1,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),
  MODEM_DBG_CLK         = GPIO_ALT(33,GROUP_GPIO2_1,GPIO_ALT_FUNC_3,GPIO_PULL_DOWN),
  KYPD_EX1              = GPIO_OUP(33,GROUP_GPIO2_1,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_41 but with PULL_UP */


  /* GPIO 34 */
  GPIO_OUTPUT_34        = GPIO_OUT(34,GROUP_GPIO_1),
  GPIO_INPUT_34         = GPIO_IN (34,GROUP_GPIO_1,GPIO_PULL_UP),
  EBI1_CS_N_2           = GPIO_ALT(34,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  KYPD_EX2              = GPIO_OUP(34,GROUP_GPIO_1,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_42 but with PULL_UP */
//#if defined(FEATURE_PROJECT_W021) || defined(FEATURE_PROJECT_W022) || defined(FEATURE_PROJECT_W021_128x160) || defined(FEATURE_PROJECT_W021_176X220) || defined(FEATURE_PROJECT_W021_240X320) || defined(FEATURE_PROJECT_W021_220X176) || defined(FEATURE_PROJECT_W021_320X240)
#if defined (FEATURE_SIM_SEL_GPIO_34_PULL_DOWN)
  SIM_SEL               = GPIO_OUP(34,GROUP_GPIO_1,GPIO_PULL_DOWN),
#endif

  /* GPIO 35 */
  GPIO_OUTPUT_35        = GPIO_OUT(35,GROUP_GPIO_1),
  GPIO_INPUT_35         = GPIO_IN (35,GROUP_GPIO_1,GPIO_PULL_DOWN),
  PA_ON_0               = GPIO_ALT(35,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 36 */
  #ifdef FEATURE_LCD_TOUCH_ENABLE
  GPIO_OUTPUT_36        = GPIO_OUP(36,GROUP_GPIO_1,GPIO_PULL_DOWN),
  #else
  GPIO_OUTPUT_36        = GPIO_OUT(36,GROUP_GPIO_1),
  #endif
  GPIO_INPUT_36         = GPIO_IN (36,GROUP_GPIO_1,GPIO_PULL_DOWN),
  PA_ON_1               = GPIO_ALT(36,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 37 */
  GPIO_OUTPUT_37        = GPIO_OUT(37,GROUP_GPIO_1),
  GPIO_INPUT_37         = GPIO_IN (37,GROUP_GPIO_1,GPIO_PULL_DOWN),
  PA_R0                 = GPIO_ALT(37,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 38 */
  GPIO_OUTPUT_38        = GPIO_OUT(38,GROUP_GPIO_1),
  GPIO_INPUT_38         = GPIO_IN (38,GROUP_GPIO_1,GPIO_PULL_DOWN),
  PA_R1                 = GPIO_ALT(38,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  PA_ON_2               = GPIO_ALT(38,GROUP_GPIO_1,GPIO_ALT_FUNC_2,GPIO_PULL_DOWN),

  /* GPIO 39 */
  GPIO_OUTPUT_39        = GPIO_OUT(39,GROUP_GPIO_1),
  GPIO_INPUT_39         = GPIO_IN (39,GROUP_GPIO_1,GPIO_PULL_UP),
  I2C_SDA               = GPIO_ALT(39,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  SPI_CS3_N             = GPIO_ALT(39,GROUP_GPIO_1,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_BUS_OUT_16        = GPIO_ALT(39,GROUP_GPIO_1,GPIO_ALT_FUNC_3,GPIO_PULL_UP),
  DBG_BUS_IN_16         = GPIO_ALT(39,GROUP_GPIO_1,GPIO_ALT_FUNC_4,GPIO_PULL_UP),

  /* GPIO 40 */
  GPIO_OUTPUT_40        = GPIO_OUT(40,GROUP_GPIO_1),
  GPIO_INPUT_40         = GPIO_IN (40,GROUP_GPIO_1,GPIO_PULL_UP),
  I2C_SCL               = GPIO_ALT(40,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_UP),
  SPI_CS2_N             = GPIO_ALT(40,GROUP_GPIO_1,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_BUS_OUT_17        = GPIO_ALT(40,GROUP_GPIO_1,GPIO_ALT_FUNC_3,GPIO_PULL_UP),
  DBG_BUS_IN_17         = GPIO_ALT(40,GROUP_GPIO_1,GPIO_ALT_FUNC_4,GPIO_PULL_UP),

  /* GPIO 41 */
#ifdef FEATURE_CAMERA
  GPIO_OUTPUT_41        = GPIO_OUP(41,GROUP_GPIO_1,GPIO_PULL_UP),
#else
  GPIO_OUTPUT_41        = GPIO_OUT(41,GROUP_GPIO_1),
#endif
  GPIO_INPUT_41         = GPIO_IN (41,GROUP_GPIO_1,GPIO_PULL_UP),
  CAMIF_DISABLE         = GPIO_OUT(41,GROUP_GPIO_1),
  EBI1_CS_N_3           = GPIO_ALT(41,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_UP),

  /* GPIO 42 */
  GPIO_OUTPUT_42        = GPIO_OUT(42,GROUP_GPIO2_1),
  GPIO_INPUT_42         = GPIO_IN (42,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_0          = GPIO_ALT(42,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  DBG_BUS_OUT_7         = GPIO_ALT(42,GROUP_GPIO2_1,GPIO_ALT_FUNC_2,GPIO_PULL_UP),
  DBG_BUS_IN_7          = GPIO_ALT(42,GROUP_GPIO2_1,GPIO_ALT_FUNC_3,GPIO_PULL_UP),

  /* GPIO 43 */
  GPIO_OUTPUT_43        = GPIO_OUT(43,GROUP_GPIO2_1),
  GPIO_INPUT_43         = GPIO_IN (43,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_1          = GPIO_ALT(43,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 44 */
  GPIO_OUTPUT_44        = GPIO_OUT(44,GROUP_GPIO2_1),
  GPIO_INPUT_44         = GPIO_IN (44,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_2          = GPIO_ALT(44,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 45 */
  GPIO_OUTPUT_45        = GPIO_OUT(45,GROUP_GPIO2_1),
  GPIO_INPUT_45         = GPIO_IN (45,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_3          = GPIO_ALT(45,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 46 */
  GPIO_OUTPUT_46        = GPIO_OUT(46,GROUP_GPIO2_1),
  GPIO_INPUT_46         = GPIO_IN (46,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_4          = GPIO_ALT(46,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 47 */
  GPIO_OUTPUT_47        = GPIO_OUT(47,GROUP_GPIO2_1),
  GPIO_INPUT_47         = GPIO_IN (47,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_5          = GPIO_ALT(47,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 48 */
  GPIO_OUTPUT_48        = GPIO_OUT(48,GROUP_GPIO2_1),
  GPIO_INPUT_48         = GPIO_IN (48,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_6          = GPIO_ALT(48,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 49 */
  GPIO_OUTPUT_49        = GPIO_OUT(49,GROUP_GPIO2_1),
  GPIO_INPUT_49         = GPIO_IN (49,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_DATA_7          = GPIO_ALT(49,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 50 */
  GPIO_OUTPUT_50        = GPIO_OUT(50,GROUP_GPIO2_1),
  GPIO_INPUT_50         = GPIO_IN (50,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_PCLK            = GPIO_ALT(50,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 51 */
  GPIO_OUTPUT_51        = GPIO_OUT(51,GROUP_GPIO2_1),
  GPIO_INPUT_51         = GPIO_IN (51,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_HSYNC           = GPIO_ALT(51,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 52 */
  GPIO_OUTPUT_52        = GPIO_OUT(52,GROUP_GPIO2_1),
  GPIO_INPUT_52         = GPIO_IN (52,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  CAMIF_VSYNC           = GPIO_ALT(52,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 53 */
  GPIO_OUTPUT_53        = GPIO_OUT(53,GROUP_GPIO_1),
  GPIO_INPUT_53         = GPIO_IN (53,GROUP_GPIO_1,GPIO_PULL_DOWN),
  CAMIF_MCLK            = GPIO_ALT(53,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  /* GPIO 54 */
  GPIO_OUTPUT_54        = GPIO_OUT(54,GROUP_GPIO2_1),
  GPIO_INPUT_54         = GPIO_IN (54,GROUP_GPIO2_1,GPIO_PULL_DOWN),
  MDP_VSYNC_P           = GPIO_ALT(54,GROUP_GPIO2_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  KYPD_EX3              = GPIO_OUP(54,GROUP_GPIO2_1,GPIO_PULL_UP),/* <----- same as GPIO_OUTPUT_44 but with PULL_UP */

  /* GPIO 55 */
  GPIO_OUTPUT_55        = GPIO_OUT(55,GROUP_GPIO_1),
  GPIO_INPUT_55         = GPIO_IN (55,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_16           = GPIO_ALT(55,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 56 */
  GPIO_OUTPUT_56        = GPIO_OUT(56,GROUP_GPIO_1),
  GPIO_INPUT_56         = GPIO_IN (56,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_17           = GPIO_ALT(56,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 57 */
  GPIO_OUTPUT_57        = GPIO_OUT(57,GROUP_GPIO_1),
  GPIO_INPUT_57         = GPIO_IN (57,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_18           = GPIO_ALT(57,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 58 */
  GPIO_OUTPUT_58        = GPIO_OUT(58,GROUP_GPIO_1),
  GPIO_INPUT_58         = GPIO_IN (58,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_19           = GPIO_ALT(58,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 59 */
  GPIO_OUTPUT_59        = GPIO_OUT(59,GROUP_GPIO_1),
  GPIO_INPUT_59         = GPIO_IN (59,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_20           = GPIO_ALT(59,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 60 */
  GPIO_OUTPUT_60        = GPIO_OUT(60,GROUP_GPIO_1),
  GPIO_INPUT_60         = GPIO_IN (60,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_21           = GPIO_ALT(60,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 61 */
  GPIO_OUTPUT_61        = GPIO_OUT(61,GROUP_GPIO_1),
  GPIO_INPUT_61         = GPIO_IN (61,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_22           = GPIO_ALT(61,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),
  
  /* GPIO 62 */
  GPIO_OUTPUT_62        = GPIO_OUT(62,GROUP_GPIO_1),
  GPIO_INPUT_62         = GPIO_IN (62,GROUP_GPIO_1,GPIO_PULL_DOWN),
  EBI1_A_D_23           = GPIO_ALT(62,GROUP_GPIO_1,GPIO_ALT_FUNC_1,GPIO_PULL_DOWN),

  GPIO_GENERIC_DEFAULT   = GPIO_CONFIG(0xFF,0,0,GPIO_INPUT,GPIO_NO_PULL)

} GPIO_SignalType;

/* ----------------------------------------------------------------------------
* The externalized functions
------------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION gpio_init()

  DESCRIPTION
        This function initializes the GPIOs for the TLMM configuration
        specified at boot time.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack

===========================================================================*/
void gpio_init(void);

/*===========================================================================
  FUNCTION gpio_tlmm_config()

  DESCRIPTION
        This function configures the tlmm configuration for the specified
        GPIO. Only the specified GPIO is configured for the specified
        functionality.

  PARAMETERS
    TLMM signal name for the given GPIO

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
   None
===========================================================================*/
void gpio_tlmm_config(GPIO_SignalType gpio_signal);

#ifdef FEATURE_I2C
/*===========================================================================
  FUNCTION gpio_i2c_config()

  DESCRIPTION
        This function configures the tlmm configuration for the I2C
        GPIOs.

  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
   None
===========================================================================*/
void gpio_i2c_config(void);

/*===========================================================================
  FUNCTION gpio_i2c_un_config()

  DESCRIPTION
    This function releases the I2C GPIOs and configures them as a general
    purpose GPIOs.

  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
   None
===========================================================================*/
void gpio_i2c_un_config(void);
#endif /* FEATURE_I2C */

/*===========================================================================
  FUNCTION gpio_tlmm_shadow()

  DESCRIPTION
        This function initializes the shadow registers that needs to be
    initialized to enable several gpios to a specific mode.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    It uses the stack

===========================================================================*/
void gpio_tlmm_shadow(void);

/*===========================================================================
  FUNCTION gpio_in()

  DESCRIPTION
        This function returns the value read from the  GPIO pin mapped
        to the given signal name.

  PARAMETERS
    TLMM signal name for the given GPIO

  DEPENDENCIES
    None.

  RETURN VALUE
    Value read fromthe GPIO pin. The value is either 0 or 1

  SIDE EFFECTS
   None
===========================================================================*/
GPIO_ValueType gpio_in( GPIO_SignalType gpio_signal);

/*===========================================================================
  FUNCTION gpio_out()

  DESCRIPTION
        This function writes the specified value to the  GPIO pin mapped
        to the given signal name.

  PARAMETERS
    TLMM signal name for the given GPIO

  DEPENDENCIES
    Value to be written to the GPIO pin

  RETURN VALUE
    None
  SIDE EFFECTS
   None
===========================================================================*/
void gpio_out
(
    GPIO_SignalType gpio_signal,
    GPIO_ValueType  gpio_value
);

/*===========================================================================
  FUNCTION gpio_tristate()

  DESCRIPTION
        This function enables/ disables gpio pin's tristate.  (legacy terminology from BIO)

  PARAMETERS
    TLMM signal name for the given GPIO

  DEPENDENCIES
    gpio_tristate_value (GPIO_TRISTATE_ENABLE configures the pin to o/p and 
    GPIO_TRISTATE_DISABLE configures the pin to be input

  RETURN VALUE
    None
  SIDE EFFECTS
   None
===========================================================================*/
void gpio_tristate
(
    GPIO_SignalType gpio_signal,
    GPIO_TristateType  gpio_tristate_value
);

#ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
/*===========================================================================

  FUNCTION GPIO_CONFIG_GPIOS_FOR_SLEEP

  DESCRIPTION
    GPIOs are reconfigured to their low power settings in preparation for
    turning off TCXO and putting the phone in low power mode.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_config_gpios_for_sleep( void );

/*===========================================================================

  FUNCTION GPIO_RESTORE_GPIOS_AFTER_SLEEP

  DESCRIPTION
    GPIOs are returned to their original configuration.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_restore_gpios_after_sleep( void );

/*===========================================================================

  FUNCTION gpio_config_gpios_for_uim_sleep

  DESCRIPTION
    GPIOs are reconfigured to their low power settings in preparation for
    turning off TCXO and putting the phone in low power mode.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/

void gpio_config_gpios_for_uim_sleep(void);

/*===========================================================================

  FUNCTION gpio_restore_gpios_after_uim_sleep

  DESCRIPTION
    GPIOs are returned to their original configuration.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/

void gpio_restore_gpios_after_uim_sleep( void );
#endif

/* NEED TO REMOVE */
#define GPS_SWITCHING 0
#define TCXO_EN       0

#define GPIO_CAMIF_EN_ON_V       0
#define GPIO_CAMIF_EN_OFF_V      1

#ifdef FEATURE_DSP

#define	ATV_POWER_PIN			(GPIO_OUTPUT_25)
#define	ATV_RESET_PIN			(GPIO_OUTPUT_24)

#define AIT701_RESET			(GPIO_OUTPUT_53)
#define AIT701_BYPASS			(GPIO_OUTPUT_51)

#endif

#define CAMIF_EN_N  GPIO_OUTPUT_41
#define CAMCLK_PO  CAMIF_MCLK
#define CAMCLK_PO_OFF GPIO_INPUT_53

#define CAMSENSOR1_POWER_PIN GPIO_OUTPUT_34
#define CAMSENSOR2_POWER_PIN GPIO_OUTPUT_41

#define CAMIF_PCLK_DISABLE       GPIO_INPUT_50
#define CAMIF_HSYNC_OUT_DISABLE  GPIO_INPUT_51
#define CAMIF_VSYNC_OUT_DISABLE  GPIO_INPUT_52
#define CAMIF_DATA_0_DISABLE     GPIO_INPUT_42
#define CAMIF_DATA_1_DISABLE     GPIO_INPUT_43
#define CAMIF_DATA_2_DISABLE     GPIO_INPUT_44
#define CAMIF_DATA_3_DISABLE     GPIO_INPUT_45
#define CAMIF_DATA_4_DISABLE     GPIO_INPUT_46
#define CAMIF_DATA_5_DISABLE     GPIO_INPUT_47
#define CAMIF_DATA_6_DISABLE     GPIO_INPUT_48
#define CAMIF_DATA_7_DISABLE     GPIO_INPUT_49

#define CAMIF_HSYNC_OUT          CAMIF_HSYNC
#define CAMIF_VSYNC_OUT          CAMIF_VSYNC


#endif /* GPIO_1100_H */
