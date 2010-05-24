/*===========================================================================

        G P I O   F O R   Q S C 1 1 0 0

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the GPIO TLMM functions for the QSC1100.

REFERENCES

Copyright (c) 2001 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tlmm/gpio_1100.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/09   km      Fix for CR : 210811
10/07/09   km      Fix for CR : 212887
02/19/09   hvo     Fixed RUIM leakage current
11/10/08   hvo     Improved power consumption on GPIOs, turned off camera,
                   featurized GPIOs used buy BT, SDCC
09/07/08   hvo     Added support for Power Consumption
02/20/08   ps      Changed all BUILD_BOOT_CHAIN_OEMSBL to BUILD_BOOT_CHAIN
06/25/07   vph     Ported for QSC11XX from QSC60X5
===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "bio.h"
#include "gpio_1100.h"
#include "msm_drv.h"
#ifndef BUILD_BOOT_CHAIN
#include "err.h"
#endif

/*-----------------------------------------------------------------------------
* The starting GPIO values for determining the register for a given GPIO
-----------------------------------------------------------------------------*/
const uint8 GPIO_GROUP_START[4] =
{
  GPIO_GROUP0_START,
  GPIO_GROUP1_START,
  GPIO_GROUP2_START,
  GPIO_GROUP3_START,
};

/* A shadow of all GPIO configuration values */
GPIO_SignalType gpio_configs[GPIO_NUM_GPIOS];

/*===========================================================================

  FUNCTION GPIO_TLMM_CONFIG

  DESCRIPTION
    Configures the GPIO TLMM as per the signal value used for the given GPIO.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_tlmm_config(GPIO_SignalType gpio_signal)
{
  GPIO_PolarityType gpio_polarity;
  uint32            gpio_oe_register;
  uint32            gpio_mask;
  uint8             gpio_number;

  gpio_number = GPIO_NUMBER(gpio_signal);

  if (gpio_number >= GPIO_NUM_GPIOS)
  {
    #ifndef BUILD_BOOT_CHAIN
      ERR_FATAL("Invalid GPIO number 0x%x",gpio_number, 0, 0);
	  #endif
  }

  gpio_polarity    = GPIO_POLARITY(gpio_signal);
  gpio_oe_register = GPIO_GROUP(gpio_signal);
  gpio_mask        = 1 <<(gpio_number-GPIO_GROUP_START[gpio_oe_register]);

  INTLOCK();  
  if (gpio_oe_register < 2)
  {
    HWIO_OUT(GPIO_PAGE,gpio_number);
    HWIO_OUT(GPIO_CFG,(GPIO_SIGNAL(gpio_signal)<<2) | GPIO_PULL(gpio_signal));
  }
  else
  {
    HWIO_OUT(GPIO2_PAGE,gpio_number);
    HWIO_OUT(GPIO2_CFG,(GPIO_SIGNAL(gpio_signal)<<2) | GPIO_PULL(gpio_signal));
  }
  INTFREE();
  
  if ( gpio_polarity == GPIO_OUTPUT )
  {
    BIO_TRISTATE(gpio_oe_register, gpio_mask, gpio_mask);
  }
  else
  {
    BIO_TRISTATE(gpio_oe_register, gpio_mask, 0);
  }
  gpio_configs[gpio_number] = gpio_signal;
}

#ifdef FEATURE_I2C
/*===========================================================================
  FUNCTION GPIO_I2C_CONFIG()

  DESCRIPTION
    This function configures the tlmm configuration for the I2C GPIOs.

  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
   None
===========================================================================*/
void gpio_i2c_config(void)
{
  gpio_tlmm_config(I2C_SDA);
  gpio_tlmm_config(I2C_SCL);
}

/*===========================================================================
  FUNCTION GPIO_I2C_UN_CONFIG()

  DESCRIPTION
    This function releases the I2C GPIOs and configures them as a general
    purpose GPIOs in output mode.

  PARAMETERS

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
   None
===========================================================================*/
void gpio_i2c_un_config(void)
{
  gpio_tlmm_config(GPIO_OUTPUT_39);
  gpio_tlmm_config(GPIO_OUTPUT_40);
}
#endif /* FEATURE_I2C */

/*===========================================================================

  FUNCTION GPIO_INIT

  DESCRIPTION
    Initializes GPIO TLMM as per the TLMM configuration defined at boot time.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_init(void)
{
  uint32            gpio_number;
  uint32            gpio_mask;
  uint32            gpio_oe_register;
  uint32            gpio_oe_0_value = 0;
  uint32            gpio_oe_1_value = 0;
  uint32            gpio_oe_2_value = 0;
  uint32            gpio_oe_3_value = 0;
  GPIO_PolarityType gpio_polarity;
  GPIO_SignalType   gpio_signal;
  
  for (gpio_number=0; gpio_number < GPIO_NUM_GPIOS; gpio_number++ )
  {
    gpio_signal       = TLMM_GPIO_CONFIG[gpio_number];
    
    // Configure GPIO # 30 for PA_R1 in Pedro 2.0
    if ( gpio_number == 30 && VERSION_GET_VERSION_ID() == 1 ) {
      gpio_signal = MSM2_PA_R1;
    }

    gpio_polarity     = GPIO_POLARITY(gpio_signal);
    gpio_oe_register  = GPIO_GROUP(gpio_signal);

    if (GPIO_SIGNAL(gpio_signal) == 0 && gpio_polarity == GPIO_OUTPUT)  // pin is under traditional GPIO control
    {
      gpio_mask = 1 << (gpio_number - GPIO_GROUP_START[gpio_oe_register]);

      if (gpio_oe_register == 0)
      {
        /* Group 0: GPIO numbers from 12, 14-30 are used GPIO_XXX_0
         * GPIO_XXX_0:
         *      Bit [30:14] -- GPIO[30:14]
         *      Bit [13]    -- reserved
         *      Bit [12]    -- GPIO[12]
         *      Bit [11:0]  -- reserve
         */
        gpio_oe_0_value |= gpio_mask;
      }
      else if (gpio_oe_register == 1)
      {
        /* Group 1: GPIO numbers from 34-41, 53, 55-62 are used GPIO_XXX_1
         * GPIO_XXX_1:
         *      Bit [30:23] -- GPIO[62:55]
         *      Bit [22]    -- reserved
         *      Bit [21]    -- GPIO[53]
         *      Bit [20:10] -- reserved
         *      Bit [9:2]   -- GPIO[41:34]
         *      Bit [1:0]   -- reserved
         */
        gpio_oe_1_value |= gpio_mask;
      }
      else if (gpio_oe_register == 2)
      {
        /* Group 2: GPIO numbers from 0-11, 13, 31 are used GPIO2_XXX_0
         * GPIO2_XXX_0:
         *      Bit [31]    -- GPIO[31]
         *      Bit [30:14] -- reserved
         *      Bit [13]    -- GPIO[13]
         *      Bit [29:12] -- reserved
         *      Bit [11-0]  -- GPIO[11:0]
         */
        gpio_oe_2_value |= gpio_mask;
      }
      else if (gpio_oe_register == 3)
      {
        /* Group 3: GPIO numbers from 32-33, 42-52, 54 are used GPIO2_XXX_1
         * GPIO2_XXX_1:
         *      Bit [22]    -- GPIO[54]
         *      Bit [21]    -- reserved
         *      Bit [20:10] -- GPIO[52:42]
         *      Bit [9:2]   -- reserved
         *      Bit [1:0]   -- GPIO[33:32]
         */
        gpio_oe_3_value |= gpio_mask;
      }
    }

    if (gpio_oe_register < 2)
    {
      HWIO_OUT(GPIO_PAGE,gpio_number);
      HWIO_OUT(GPIO_CFG, (GPIO_SIGNAL(gpio_signal)<<2) | GPIO_PULL(gpio_signal));
    } 
    else
    {
      HWIO_OUT(GPIO2_PAGE,gpio_number);
      HWIO_OUT(GPIO2_CFG, (GPIO_SIGNAL(gpio_signal)<<2) | GPIO_PULL(gpio_signal));
    }
  }

  HWIO_OUT(GPIO_OE_0,gpio_oe_0_value);
  HWIO_OUT(GPIO_OE_1,gpio_oe_1_value);
  HWIO_OUT(GPIO2_OE_0,gpio_oe_2_value);
  HWIO_OUT(GPIO2_OE_1,gpio_oe_3_value);
}
   
/*===========================================================================
  FUNCTION GPIO_TLMM_SHADOW

  DESCRIPTION
    Initializes the shadow registers as per the TLMM configuration
    defined at boot time.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_tlmm_shadow(void)
{
  uint32             gpio_number;
  uint32             gpio_oe_register;
  uint32             gpio_mask;
  uint32             gpio_oe_0_value = 0;
  uint32             gpio_oe_1_value = 0;
  uint32             gpio2_oe_0_value = 0;
  uint32             gpio2_oe_1_value = 0;
  GPIO_PolarityType  gpio_polarity;
  GPIO_SignalType    gpio_signal;

  for (gpio_number = 0; gpio_number < GPIO_NUM_GPIOS; gpio_number++ )
  {
    gpio_signal = TLMM_GPIO_CONFIG[gpio_number];

    gpio_configs[gpio_number] = gpio_signal;
    gpio_polarity = GPIO_POLARITY(gpio_signal);
    gpio_oe_register = GPIO_GROUP(gpio_signal);

    if (gpio_polarity == GPIO_OUTPUT)  // pin is under traditional GPIO control
    {
      gpio_mask = 1 << (gpio_number - GPIO_GROUP_START[gpio_oe_register]);
      if (gpio_oe_register == 0)
      {
        gpio_oe_0_value |= gpio_mask;
      }
      else if (gpio_oe_register == 1)
      {
        gpio_oe_1_value |= gpio_mask;
      }
      else if (gpio_oe_register == 2)
      {
        gpio2_oe_0_value |= gpio_mask;
      }
      else if (gpio_oe_register == 3)
      {
        gpio2_oe_1_value |= gpio_mask;
      }
    }
  }

  BIO_UPDATE_TRISTATE_SHADOW(GPIO_REG_0,gpio_oe_0_value);
  BIO_UPDATE_TRISTATE_SHADOW(GPIO_REG_1,gpio_oe_1_value);
  BIO_UPDATE_TRISTATE_SHADOW(GPIO2_REG_0,gpio2_oe_0_value);
  BIO_UPDATE_TRISTATE_SHADOW(GPIO2_REG_1,gpio2_oe_1_value);
}

/*===========================================================================
  FUNCTION GPIO_OUT

  DESCRIPTION
    Outputs the given value to the corresponding GPIO register given a GPIO
    signal type. This function calculates the GPIO register and the mask
    value.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_out( GPIO_SignalType gpio_signal, GPIO_ValueType  gpio_value )
{
  uint32 gpio_out_register;
  uint32 gpio_mask;
  uint8  gpio_number;

  gpio_number = GPIO_NUMBER(gpio_signal);

  if (gpio_number >= GPIO_NUM_GPIOS)
  {
    #ifndef BUILD_BOOT_CHAIN
      ERR_FATAL("Invalid GPIO number 0x%x",gpio_number, 0, 0);
	  #endif
  }

  gpio_out_register = GPIO_GROUP(gpio_signal);

  gpio_mask = 1 << (gpio_number - GPIO_GROUP_START[gpio_out_register]);
  if (gpio_value == GPIO_LOW_VALUE)
  {
    BIO_OUT(gpio_out_register, gpio_mask, 0);
  } 
  else
  {
    BIO_OUT(gpio_out_register, gpio_mask, gpio_mask);
  }
}

/*===========================================================================
  FUNCTION GPIO_IN

  DESCRIPTION
    Reads the the corresponding GPIO register given a GPIO signal type and
    extracts the value for that particular GPIO. This function calculates the
    GPIO register and the mask value required to return the GPIO value.

  DEPENDENCIES
    None.

  RETURN VALUE
    GPIO value at the given GPIO number
===========================================================================*/
GPIO_ValueType gpio_in( GPIO_SignalType gpio_signal )
{
  uint32             gpio_in_register;
  uint32             gpio_mask;
  uint32             gpio_value;
  uint8              gpio_number;

  gpio_number = GPIO_NUMBER(gpio_signal);

  if (gpio_number >= GPIO_NUM_GPIOS)
  {
    #ifndef BUILD_BOOT_CHAIN
      ERR_FATAL("Invalid GPIO number 0x%x",gpio_number, 0, 0);
	  #endif
  }
  gpio_in_register = GPIO_GROUP(gpio_signal);

  gpio_mask        = 1 <<(gpio_number-GPIO_GROUP_START[gpio_in_register]);
  gpio_value       = inpdw(bio_gpio[gpio_in_register].in_addr) & gpio_mask;

  if (gpio_value == 0)
  {
    return GPIO_LOW_VALUE;
  } 
  else
  {
    return GPIO_HIGH_VALUE;
  }
}

/*===========================================================================

  FUNCTION GPIO_TRISTATE

  DESCRIPTION
   This function enables/ disables tri-state on specified GPIO.  Writing a value 0 
   (GPIO_TRISTATE_DISABLE) disables the tristate and writing a value 1 (GPIO_TRISTATE_ENABLE) 
   enables tristate.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_tristate( GPIO_SignalType gpio_signal, GPIO_TristateType gpio_tristate_value)
{
  uint32             gpio_oe_register;
  uint32             gpio_mask;
  uint8              gpio_number;

  gpio_number = GPIO_NUMBER(gpio_signal);
  if (gpio_number >= GPIO_NUM_GPIOS)
  {
    #ifndef BUILD_BOOT_CHAIN
      ERR_FATAL("Invalid GPIO number 0x%x",gpio_number, 0, 0);
	  #endif
  }

  gpio_oe_register = GPIO_GROUP(gpio_signal);
  gpio_mask        = 1 <<(gpio_number-GPIO_GROUP_START[gpio_oe_register]);

  if (gpio_tristate_value == GPIO_TRISTATE_DISABLE)
  {
     BIO_TRISTATE(gpio_oe_register,gpio_mask,gpio_mask);
  } 
  else
  {
     BIO_TRISTATE(gpio_oe_register,gpio_mask,0);
  }
}

#ifdef FEATURE_MANAGE_GPIOS_FOR_SLEEP
/* ******************* GPIO SLEEP CONFIGURATION ***************** */
/* Customer changes are required here for each different phone design */

/* For each GPIO that needs to be changed to a new function,
 * list the new configuration in this array.  This array is used with
 * registers GPIO_PAGE and GPIO_CFG to change the signal present on a
 * GPIO when the MSM is in low power / sleep mode.   The values listed
 * in this array can configure a pin for (input pull up, input pull down,
 * input no pull, or an alternate function).
 */

GPIO_SignalType gpio_sleep_changes[] =
{
  /* GPIO 0-4 (keypad usage) configure to output low, but their functions
   * remain the same, so no change.
   */
  /* GPIO 5 through 9, (key sense usage), its function unchange, no change */
  /* GPIO10 (backlight usage)configure to output low, its function unchange */
  /* GPIO 11 (Headset det usage), no change */
  /* GPIO 12,14,15 (UIM usage) configure to input low, it can be featurized */
  /* GPIO 13 uses to enable bluetooth, and BT takes care it, in the case of
   * no BT, it configure to input, tlmm init function already take care it.
  */
  /* GPIO 20,21,22,23(AUX PCM usage) configure to input, pull down */
  GPIO_IN(20,GROUP_GPIO_0,GPIO_PULL_DOWN), // AUX_PCM_CLK
  GPIO_IN(21,GROUP_GPIO_0,GPIO_PULL_DOWN), // AUX_PCM_SYNC
  GPIO_IN(22,GROUP_GPIO_0,GPIO_PULL_DOWN), // AUX_PCM_DIN
  GPIO_IN(23,GROUP_GPIO_0,GPIO_PULL_DOWN), // AUX_PCM_OUT

#ifdef FEATURE_DRV_SDCC
  /* GPIO 24-29 (SDCC usage), card is not in */
#if 0
  GPIO_IN(24,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_CLK
  GPIO_IN(25,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_DATA[0]
  GPIO_IN(26,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_DATA[1]
  GPIO_IN(27,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_DATA[2]
  GPIO_IN(28,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_DATA[3]
  GPIO_IN(29,GROUP_GPIO_0,GPIO_PULL_DOWN), // SDCC_CMD
#endif										   // 
#endif
  /* GPIO 30 (WDOG disable usage), configure output high, its function unchange */
  /* GPIO 31 (BOOT from ROM usage), configure from pull down to pull up */
  GPIO_IN(30,GROUP_GPIO_0,GPIO_NO_PULL),
  GPIO_IN(31,GROUP_GPIO_0,GPIO_PULL_UP),
  /* GPIO 32, not use on FFA but on SURF for FPGA, configure to input pull down
   * New FFA use it as SDCC card detection but has same configuration for sleep.
   */
//  GPIO_IN(32,GROUP_GPIO2_0,GPIO_PULL_DOWN),
//  GPIO_IN(32,GROUP_GPIO2_0,GPIO_NO_PULL), // FFA is different
  /* GPIO 33 (EBI1_A_D(24) usage), no change */
  /* GPIO 34 (EBI1 chipselect usage), configure from output to input pull down */
//  GPIO_IN(34, GROUP_GPIO_1,GPIO_PULL_DOWN), // EBI1_CS_N[2]
//  GPIO_IN(34, GROUP_GPIO_1,GPIO_PULL_UP), // EBI1_CS_N[2]
  /* GPIO 35 thru 38 (PA usage), configure from output to output low, their
   * functions unchange, so no to change.
   */
  /* GPIO 39 thru 40 (I2C usage), configure from bidirection to input no pull */
  GPIO_IN(39,GROUP_GPIO_1,GPIO_NO_PULL), // I2C_SDA
  GPIO_IN(40,GROUP_GPIO_1,GPIO_NO_PULL), // I2C_SDA
  /* GPIO 41, (CAMIF DIS usage), configure from output to output high */
  /* GPIO 42 through 52 (camera usage), no change */
  /* GPIO 53 (CAMIF CLK usage), configure from output to input pull down */
  GPIO_IN(53,GROUP_GPIO_1,GPIO_PULL_DOWN),
  /* GPIO 54 (MDP SYNC usage), no change */
  /* GPIO 55 through 62 (EBI1_A_D[x] usage), no change */
  GPIO_GENERIC_DEFAULT  /* Required, must be last array entry */
};


struct gpio_sleep_registers {
  uint32 mask;
  uint32 out;
};

/* gpio_sleep_oe[4] configures registers GPIO_OE_0,1 and GPIO2_OE_0,1
 * for low power mode.
 *
 * Set a bit in the "mask" of each pair to indicate that the
 * particular GPIO should be changed at sleep.  Set the
 * appropriate bit in the "out" of each pair to choose between
 * input(0) or output(1).
 */

#define GPIO_OE_REG_NUM  4
struct gpio_sleep_registers gpio_sleep_oe[GPIO_OE_REG_NUM] =
{
  /* GPIO_OE_0 consists GPIO[30-14, 12]
        BIT 30:14 -> GPIO 30 thru 14
        BIT 13    -> RESERVED
        BIT 12    -> GPIO 12
        BIT 11:0  -> RESERVED
	 Follow are GPIOs change during sleep: 
	 GPIO 12,14-29- no change, already configure with input/PD from gpio_sleep_changes struct
	 GPIO 30, change from output low to output high, need to enable its output
	 Mask = 0x40000000
	 Out  = 0x40000000
  */
  {0x40000000, 0x40000000},
  
  /* GPIO_OE_1 consists of GPIO[62-55,53,41-34]
        BIT 30:23 -> GPIO 62 thru 55
        BIT 22    -> RESERVED
        BIT 21    -> GPIO 53
        BIT 20:10 -> RESERVED
        BIT 9:2   -> GPIO 41 thru 34
        BIT 1:0   -> RESERVED

		GPIO34 - change from output to input w PD, no need, bit 2
	    GPIO35 - change from output to output low, bit 3
	    GPIO36 - change from output to output low, bit 4
	    GPIO37 - change from output to output low, bit 5
	    GPIO38 - change from output to output low, bit 6
	    GPIO39 - change from bidirection to input no pull, bit 7, no need
	    GPIO40 - change from bidirection to input no pull, bit 8, no need
	    GPIO41 - change from output to output high, bit 9
	    GPIO53 - change from output to input, bit 21, no need
	    GPIO55-62 - change from output to output, bit 23-30, no need

		Mask = 0x00000278
		Out  = 0x00000278
  */
#ifdef FEATURE_CAMERA
  {0x00000278, 0x00000278},
#else
  {0x00000078, 0x00000078},
#endif
  /*
   GPIO2_OE_0 consists GPIO[31,13,11-0]
        BIT 31    -> GPIO 31
        BIT 30:14 -> RESERVED
        BIT 13    -> GPIO 13
        BIT 12    -> RESERVED
        BIT 11:0  -> GPIO  11 thru 0
   Follow GPIOs change during sleep: 0-4, 10, and 31
   GPIO 0-4, 10 configure as output low in sleep mode
   GPIO 13, Bluetooth handles it
   GPIO 31 configures as pull up
   Mask=0x0000041f
   Out =0x0000041f
   */
  {0x0000041f, 0x0000041f},
  
  /* GPIO2_OE_1 consists of GPIO[54,52-42,33-32]
        BIT 22    -> GPIO 54
        BIT 21    -> RESERVED
        BIT 20:10 -> GPIO 52 thru 42
        BIT 9:2   -> RESERVED
        BIT 1:0   -> GPIO 33,32
	  Follow GPIOs change during sleep:
	  GPIO 32, change from output to input/pd, no change
	  GPIO 33, remain unchange
      Mask=0x00000000
  */
  {0x00000000, 0x00000000}
};

/* gpio_sleep_out[4] configures registers GPIO_OUT_0,1 and GPIO2_OUT_0,1
 * for low power mode.
 *
 * Set a bit in the "mask" of each pair to indicate that the
 * particular GPIO should be reconfigured at sleep.  Set the
 * appropriate bit in the "out" of each pair to choose the
 * output state (high or low) at sleep.
 */
#define GPIO_OUT_REG_NUM  4
struct gpio_sleep_registers gpio_sleep_out[GPIO_OUT_REG_NUM] =
{
  /* GPIO_OUT_0 consists GPIO[30-14, 12]
        BIT 30:14 -> GPIO 30 thru 14
        BIT 13    -> RESERVED
        BIT 12    -> GPIO 12
        BIT 11:0  -> RESERVED
	 Follow are GPIOs change during sleep: 12,14,15,16,17,19,20,21,23,24-29,30
	 GPIO12[output to input low, no need]
	 GPIO14[output to input low, no need]
	 GPIO15[bidirection to input high, no need]
	 GPIO16[output to input, no need]
	 GPIO17[input to input, no need]
	 GPIO19[output to input, no need]
	 GPIO20[output to input, no need]
	 GPIO21[output to input, no need]
	 GPIO23[output to input, no need]
	 GPIO24[output to input, no need]
	 GPIO25[bidirection to input, no need]
	 GPIO26[bidirection to input, no need]
	 GPIO27[bidirection to input, no need]
	 GPIO28[bidirection to input, no need]
	 GPIO29[bidirection to input, no need]
	 GPIO30[output low to output high]
	 Mask = 0x40000000
  */
  {0x40000000, 0x40000000},
 
  /* GPIO_OUT_1 consists of GPIO[62-55,53,41-34]
        BIT 30:23 -> GPIO 62 thru 55
        BIT 22    -> RESERVED
        BIT 21    -> GPIO 53
        BIT 20:10 -> RESERVED
        BIT 9:2   -> GPIO 41 thru 34
        BIT 1:0   -> RESERVED
	GPIO34[output to input/pd, bit 2, no need]
	GPIO35[output to output low, bit 3]
	GPIO36[output to output low, bit 4]
	GPIO37[output to output low, bit 5]
	GPIO38[output to output low, bit 6]
	GPIO39[bidirection to input no pull, bit 7, no need]
	GPIO40[bidirection to input no pull, bit 8, no need]
	GPIO41[output to output high, bit, bit 9]
	GPIO53[output to input, bit 21, no need]
	GPIO55-62[output to output, bit 23-30, no need]
	 Mask=0x00000278
  */
#ifdef FEATURE_CAMERA
  {0x00000278, 0x00000200},
#else
  {0x00000078, 0x00000000},
#endif
  /*
   GPIO2_OUT_0 consists GPIO[31,13,11-0]
        BIT 31    -> GPIO 31
        BIT 30:14 -> RESERVED
        BIT 13    -> GPIO 13
        BIT 12    -> RESERVED
        BIT 11:0  -> GPIO  11 thru 0
   Follow GPIOs change during sleep: 0-4, 10, and 31
   GPIO 0-4, 10 change from output to output low
   GPIO 5-9 no change
   GPIO 10 changes from output to output low
   GPIO 11 no change
   GPIO 13 handle by Bluetooth, when no bluetooth tlmm init handle it
   GPIO 31 changes from input to input/pu, no need
   Mask=0x0000041f
   */
  {0x0000041f, 0x00000000},

  /* GPIO2_OUT_1 consists of GPIO[54,52-42,33-32]
        BIT 22    -> GPIO 54
        BIT 21    -> RESERVED
        BIT 20:10 -> GPIO 52 thru 42
        BIT 9:2   -> RESERVED
        BIT 1:0   -> GPIO 33,32
	  GPIO32 changes from output to input/pd, no need
	  GPIO33 no change, no need
	  GPIO42-52 no change, no need]
	  GPIO54 no change
	  Mask=0x00000000
  */
  {0x00000000, 0x00000000}
};

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
void gpio_config_gpios_for_sleep( void )
{
  int io;

  GPIO_SignalType gpio_old_signal;
  uint32 gpio_number;

  /* Reconfigure the GPIO_CFG of select GPIOs */
  for(io=0; gpio_sleep_changes[io] != GPIO_GENERIC_DEFAULT; io++)
  {
    gpio_number = GPIO_NUMBER(gpio_sleep_changes[io]);

    gpio_old_signal = gpio_configs[gpio_number];
    gpio_tlmm_config(gpio_sleep_changes[io]);

    /* set gpio_configs[] to reflect the old purpose for restore after
       sleep. */
    gpio_configs[gpio_number] = gpio_old_signal;
  }

  for(io=0; io<GPIO_OE_REG_NUM; io++)
  {
    if(bio_gpio[io].tsen_addr)
    {
      /* Reconfigure the GPIO_OE_* and GPIO_OUT_* registers.  Shadow
         values of these registers are not changed so that the "awake"
         value may be restored after sleep. */

      /* Change GPIO_OE_* without updating shadow values */
      outpdw(bio_gpio[io].tsen_addr,
             (bio_gpio_image[io].ts_buf & (~gpio_sleep_oe[io].mask))
             | ( gpio_sleep_oe[io].mask & gpio_sleep_oe[io].out));

      /* Change GPIO_OUT_* without updating shadow values */
      outpdw(bio_gpio[io].out_addr,
             (bio_gpio_image[io].out_buf & (~gpio_sleep_out[io].mask)) |
             (gpio_sleep_out[io].mask & gpio_sleep_out[io].out));
    }
  }
}

/*===========================================================================

  FUNCTION GPIO_RESTORE_GPIOS_AFTER_SLEEP

  DESCRIPTION
    GPIOs are returned to their original configuration.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void gpio_restore_gpios_after_sleep( void )
{
  int io;

  uint32 gpio_number;

  /* Restore GPIO_OE_* and GPIO_OUT_* registers to their "awake"
     value which is in the bio_gpio_image shadow values. */
  for(io=0; io<4; io++)
  {
    if(bio_gpio[io].tsen_addr)
    {
      outpdw(bio_gpio[io].out_addr, (bio_gpio_image[io].out_buf));
      outpdw(bio_gpio[io].tsen_addr, (bio_gpio_image[io].ts_buf));
    }
}

  /* Restore GPIOs to their "awake" function using gpio_configs
     as the shadow value. */
  for(io=0; gpio_sleep_changes[io] != GPIO_GENERIC_DEFAULT; io++)
  {
    gpio_number = GPIO_NUMBER(gpio_sleep_changes[io]);
    gpio_tlmm_config( gpio_configs[gpio_number] );
  }
}

GPIO_SignalType gpio_uim_sleep_changes[] =
{
  GPIO_IN(12,GROUP_GPIO_0,GPIO_PULL_DOWN), // UIM_CLK
  GPIO_IN(14,GROUP_GPIO_0,GPIO_PULL_DOWN), // UIM_RESET
  GPIO_IN(15,GROUP_GPIO_0,GPIO_PULL_UP)    // UIM_DATA
};

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
void gpio_config_gpios_for_uim_sleep(void)
{
  GPIO_SignalType gpio_old_signal_data, gpio_old_signal_clk;
  GPIO_SignalType gpio_old_signal_reset;
  
  gpio_old_signal_clk   = gpio_configs[12];
  gpio_old_signal_reset = gpio_configs[14];
  gpio_old_signal_data  = gpio_configs[15];

  gpio_tlmm_config(gpio_uim_sleep_changes[0]);
  gpio_tlmm_config(gpio_uim_sleep_changes[1]);
  gpio_tlmm_config(gpio_uim_sleep_changes[2]);

  /* set gpio_configs[] to reflect the old purpose for restore after sleep. */
  gpio_configs[12] = gpio_old_signal_clk;
  gpio_configs[14] = gpio_old_signal_reset;
  gpio_configs[15] = gpio_old_signal_data;
}

/*===========================================================================

  FUNCTION gpio_restore_gpios_after_uim_sleep

  DESCRIPTION
    GPIOs are returned to their original configuration.

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/

void gpio_restore_gpios_after_uim_sleep( void )
{
    gpio_tlmm_config( gpio_configs[12] );
    gpio_tlmm_config( gpio_configs[14] );
    gpio_tlmm_config( gpio_configs[15] );
}
#endif