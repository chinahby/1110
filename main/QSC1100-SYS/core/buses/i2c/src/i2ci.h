#ifndef I2CI_H
#define I2CI_H
/*===========================================================================

              I 2 C   L I B R A R Y   I N T E R N A L   H E A D E R

DESCRIPTION
  This file contains global declarations and external references
  required by I2C library modules.

Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

   $Header: //depot/asic/msmshared/drivers/i2c/i2ci.h#13 $ $DateTime: 2007/12/28 05:08:09 $ $Author: aksingh $


when       who     what, where, why
--------   ---     -------------------------------------------------------
11/03/06   hz      Added support for dynamic speed change.
03/24/05   dle     Replaced I2C_HW_CTRL_RESET with I2C_CLK_RESET.
                   Added I2C_CLK_ENABLE and I2C_CLK_DISABLE.
07/13/05   dle     Added I2C_HW_BIT_DELAY and I2C_READ_SCL macro.
11/17/04   dle     Made I2C_CE_INIT, I2C_CHIP_ENABLE and I2C_CHIP_DISABLE
                   macros into function calls.
09/21/04   dle     Macros related to CHIP_ENABLE: changed name, changed to use
                   FPGA macros and implements for FFA builds.
08/27/04   dle     Conditioned CE with CPLD_MSM_H and T_SURF.
08/24/04   dle     Added back change history. Replace 3WIRE with
                   3WIRE_SW_CTRL with CE_CTRL and featuriz with
                   FEATURE_I2C_CHIP_ENABLE.
08/06/04   sch     Merged John Hacklander's changes to provide the same I2C
                   evironment as Jaguar.
07/12/04   rve     Added gpio_i2c_config() to disable i2c s/w control when
				   HW control is enabled.
06/08/04   sch     Runtime detection of s/w or h/w serial interface control.
11/10/03   dle     Changed FS_DIV divide value from 2 to 3.
07/03/03   dle     Restored the change to I2C_HW_CTRL_DELAY. The clk_busy_wait
                   differences is taken care of in source code.
06/26/03   dle     6250 does not support FEATURE_I2C_IO_CTRL_OVER_HW.
05/24/03   dle     Featurized (FEATURE_I2C_IO_CTRL_OVER_HW) options
                   (I2C_NOP_LAST_BYTE and I2C_CLK_START_BEFORE_RW) where there
                   is a need to switch to IO_CTRL in order to support these
                   options.
05/29/03   dle     Make use if clkrgm_i2c_clk_div_sel_val.
12/02/02   dle     Added clk_busy_wait in I2C_SETUP_HW_CTRL to remove compiler
                   warning.
10/04/02   dle     Extended I2C_BIT_HIGH to 32 bits. Used gpio_i2c_config()
                   and gpio_i2c_un_config() to switch between HW and SW
                   control. Added I2C_HW_SWITCH_DELAY. Added comment about
                   one extra wait state for I2C_HW_CTRL_DELAY.
10/02/02   dle     Add 1 wait state to I2C_HW_CTRL_DELAY. Without this extra
                   wait, 6100 I2C interface may not work all the time.
03/11/02   aw      Added support for I2C hardware controller.
04/14/00   sm      Featurized I2C driver.
02/21/00   sm      Modified for SDAC target.  Removed non ARM support.
                   Ported for MSM3100
10/22/98   dlb     Added support for MSM3/ARM.
09/18/98   sk      Added i2c_op_type to support default address I/O.
02/27/98   sk      Updated macros to use gpio id to obtain port addresses
                   and buffers.
02/09/98   sk      Incorporated code review comments.
12/15/97   sk      File created.

===========================================================================*/

#include "comdef.h"
#include "target.h"

#ifdef FEATURE_I2C
#include "clk.h"
#if defined FEATURE_I2C_CHIP_ENABLE && defined CPLD_MSM_H && defined T_SURF
#include CPLD_MSM_H
#endif /* FEATURE_I2C_CHIP_ENABLE */

/*===========================================================================

                RESOLVING MSM DEFINITION DIFFERENCES

===========================================================================*/
/* 6250 */
#ifdef HWIO_MSM_CLK_RESET_I2C_BMSK
#define I2C_SYS_RESET I2C
#endif

/* 6275 */
#ifdef HWIO_MSM_CLK_RESETA_I2C_SW_RESET_BMSK
#define MSM_CLK_RESET MSM_CLK_RESETA
#define I2C_SYS_RESET I2C_SW_RESET
#endif

/* 6500, 6280, 6550, 6800 */
#ifdef HWIO_MSM_CLK_RESETA_I2C_SYS_RESET_BMSK
#define MSM_CLK_RESET MSM_CLK_RESETA
#endif

/* Different MSM has Arm running at different speed. The faster the ARM
 * speed, the smaller the factor should be. Please adjust so that the correct
 * loop_count is calculated to achieve the needed delay. */
#ifndef I2C_ARM_CLK_FACTOR
#define I2C_ARM_CLK_FACTOR 1.0
#endif


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define I2C_BIT_LO        0       /* bit value '0' */

#define I2C_BIT_HI        0xFFFFFFFF /* used for setting SDA high and in      */
                                  /* checking for ACK from slave.          */

#define I2C_ACK           0       /* Slave pulls down SDA during SCL hi    */
#define I2C_NACK          1       /* Slave pulls up SDA during SCL hi      */
#define I2C_MAX_DATA_LEN  0x8000  /* Max data transfer len < 32K           */

/* I2C Controller Specific */
#define I2C_ADDR_BYTE     0x100   /* Specify an address byte transmission
                                     to the I2C_WRITE_DATA register        */
#define I2C_LAST_BYTE     0x200   /* A data transmission followed by a
                                     STOP Condition                        */

/* Slave address read/write bit mask definitions                           */
#define I2C_IO_MASK       0x01    /* r/w bit mask                          */
#define I2C_IO_MASK_R     0x01    /* mask for read operation               */
#define I2C_IO_MASK_W     0x00    /* mask for write operation              */

/* Delay for 4.7 us */
#define I2C_4_7_MICROSEC_WAIT clk_busy_wait(5)
#define I2C_4_MICROSEC_WAIT clk_busy_wait(4)

/* minimum time SCL must remain low while clocking a data bit              */
#define I2C_TLOW_DELAY         I2C_4_7_MICROSEC_WAIT

/* minimum time SCL must remain high while clocking a data bit             */
#define I2C_THIGH_DELAY        I2C_4_MICROSEC_WAIT

/* minimum time SCL must remain low after a stop condition                 */
#define I2C_TBUF_DELAY         I2C_4_7_MICROSEC_WAIT

/* Repeated Start condition setup time                                     */
#define I2C_TSU_STA_DELAY      I2C_4_7_MICROSEC_WAIT

/* Start condition hold time                                               */
#define I2C_THD_STA_DELAY      I2C_4_MICROSEC_WAIT

/* Stop condition setup time                                               */
#define I2C_TSU_STO_DELAY      I2C_4_MICROSEC_WAIT

/* I2C hardware controller data line state */
#define I2C_DATA_RESET_WAIT_S       ( 0x00 << 10 )
#define I2C_DATA_TX_ADDR_S          ( 0x01 << 10 )
#define I2C_DATA_TX_DATA_S          ( 0x02 << 10 )
#define I2C_DATA_HS_ADDR_S          ( 0x03 << 10 )
#define I2C_DATA_10_BIT_ADDR_S      ( 0x04 << 10 )
#define I2C_DATA_2ND_BYTE_S         ( 0x05 << 10 )
#define I2C_DATA_RX_DATA_S          ( 0x06 << 10 )

/* I2C hardware controller clock line state */
#define I2C_CLK_RESET_BUSIDLE_S     ( 0x00 << 13 )
#define I2C_CLK_NOT_MASTER_S        ( 0x01 << 13 )
#define I2C_CLK_HIGH_S              ( 0x02 << 13 )
#define I2C_CLK_LOW_S               ( 0x03 (( 13 )
#define I2C_CLK_HIGH_WAIT_S         ( 0x04 << 13 )
#define I2C_CLK_FORCED_LOW_S        ( 0x05 << 13 )
#define I2C_CLK_HS_ADDR_LOW_S       ( 0x06 << 13 )
#define I2C_CLK_DOUBLE_BUF_WAIT_S   ( 0x07 << 13 )

#define I2C_STATUS_RH__ERROR_MASK   ( HWIO_FMSK(I2C_STATUS,FAILED)        | \
                                      HWIO_FMSK(I2C_STATUS,INVALID_WRITE) | \
                                      HWIO_FMSK(I2C_STATUS,ARB_LOST)      | \
                                      HWIO_FMSK(I2C_STATUS,PACKET_NACKED) | \
                                      HWIO_FMSK(I2C_STATUS,BUS_ERROR)     )

/* I2C bus states */
typedef enum
{
  I2C_IDLE_S,                      /* I2C bus idle state                   */
  I2C_TX_S,                        /* I2C Tx data state                    */
  I2C_RX_S,                        /* I2C Rx data state                    */
  I2C_UNINIT_S,                    /* I2C Uninit state                     */
  I2C_FAIL_S                       /* I2C Failure state                    */
} i2c_state_type;

typedef enum {
	
  I2C_HW_CTRL,                  /* I2C hardware control                 */
#ifdef FEATURE_I2C_IO_CTRL
  I2C_IO_CTRL,                  /* I2C software control (IO controller) */
#endif /* FEATURE_I2C_IO_CTRL */
#ifdef FEATURE_I2C_CHIP_ENABLE
  I2C_CE_CTRL,                  /* I2C software IO control with chip enable */
#endif /* FEATURE_I2C_CHIP_ENABLE */
  I2C_CTRL_INVALID              /* Invalid Serial Bus ID */
} i2c_ctrl_type;

/* buffer type describing GPIO id and SCL/SDA masks */
typedef struct {
  i2c_ctrl_type         ctrl;       /* I2C controller                       */
  const  bio_gpio_type  gpio_id;    /* GPIO ID                              */
  const  dword          scl_mask;   /* SCL mask                             */
  const  dword          sda_mask;   /* SDA mask                             */
  i2c_state_type        state;      /* bus state                            */
} i2c_io_type;

/* I2C I/O operation types */
typedef enum {
  I2C_READ,
  I2C_WRITE
} i2c_op_type;

/* Declare simulator outp()/inp() for Unit Testing (off-target) */
#ifdef I2C_UT
extern int gpio_sim_outp(unsigned port, int val);
extern int gpio_sim_inp(unsigned port);
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

MACRO I2C_ADDR_MASK_R

DESCRIPTION
  This macro will set the r/w bit (8th-bit of the address byte) to READ

PARAMETERS
  Address byte

DEPENDENCIES
  None.

RETURN VALUE
  Address byte with the r/w bit set to READ.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_ADDR_MASK_R(addr)       ((addr & ~I2C_IO_MASK) | I2C_IO_MASK_R)


/*===========================================================================

MACRO I2C_ADDR_MASK_W

DESCRIPTION
  This macro will set the r/w bit (8th-bit of the address byte) to WRITE

PARAMETERS
  Address byte

DEPENDENCIES
  None.

RETURN VALUE
  Address byte with the r/w bit set to WRITE.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_ADDR_MASK_W(addr)       ((addr & ~I2C_IO_MASK) | I2C_IO_MASK_W)


/*===========================================================================

MACRO I2C_HS_DIV

DESCRIPTION
  This macro will calculate the high speed i2c scl clock value in

PARAMETERS
  freq - frequency for I2C SCL

DEPENDENCIES
  clk_i2c_freq_khz - i2c Clock frequency

RETURN VALUE
  i2c hardware controller's SCL frequency value.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_HS_DIV(freq)       MAX( MIN( ((clk_i2c_freq_khz / \
                                       (3 * (freq))) - 1), 7), 3)


/*===========================================================================

MACRO I2C_FS_DIV

DESCRIPTION

PARAMETERS
  freq - frequency for I2C SCL

DEPENDENCIES
  clk_i2c_freq_khz - I2C clock frequency

RETURN VALUE
  i2c hardware controller's FS mode frequency value.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_FS_DIV(freq)       MAX( MIN( ((clk_i2c_freq_khz / \
                                       (2 * (freq))) - 3), 255), 3)


/*===========================================================================

MACRO I2C_SCL_FREQ_CAL

DESCRIPTION
  This marco combine the i2c hardware controller HS and FS mode's SCL clock
  frequency value.

PARAMETERS
  freq - frequency for I2C SCL

DEPENDENCIES
  clk_regime_hclk_freq_khz - HCLK clock frequency

RETURN VALUE
  i2c hardware controller's SCL frequency value.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_SCL_FREQ_CAL(freq)   ((I2C_HS_DIV(freq) << 8) | I2C_FS_DIV(freq))


/*===========================================================================

MACRO I2C_HW_CTRL_DELAY

DESCRIPTION
  This marco calculate i2c hardware controller time for the last read/write,
  so driver can get the correct bus state of hardware controller.
  Since clock calculation may be rounded one way or the other, it is
  necessary to add one wait state to make sure we are reading bus state
  when it is valid.
  
  8-bits (data byte) + 1-bit (ACK/NACK) + 1-bit (STOP) + 1-bit (wait state)

PARAMETERS
  freq - frequency for I2C SCL in KHz.

DEPENDENCIES
  None.

RETURN VALUE
  i2c hardware controller's time for last read/write.

SIDE EFFECTS
  None.

===========================================================================*/
#define I2C_HW_CTRL_DELAY(freq)   ((11 * 1000) / freq )

/*===========================================================================

MACRO I2C_HW_SWITCH_DELAY

DESCRIPTION
  This marco calculate delay time after the GPIO port is switched back to
  i2c hardware controller. It requires at least one clock time.
  
PARAMETERS
  freq - frequency for I2C SCL in KHz.

DEPENDENCIES
  None.

RETURN VALUE
  i2c hardware controller's time for last read/write.

SIDE EFFECTS
  None.

===========================================================================*/
#define I2C_HW_SWITCH_DELAY(freq)   (2000 / freq )

/*===========================================================================

MACRO I2C_HW_BIT_DELAY

DESCRIPTION
  This marco calculate delay time for one bit transmit time.
  
PARAMETERS
  freq - frequency for I2C SCL in KHz.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
#define I2C_HW_BIT_DELAY(freq)   (1000 / freq )


/*===========================================================================

MACRO I2C_GPIO_OUT

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask, to a
  specified port.  Only the bits corresponding to the mask are actually
  affected; other bits retain their previous values.  To do this, an image is
  maintained of the previous value written to the port which is combined with
  the new value prior to writing.

PARAMETERS
  gpio_id - target GPIO id
  mask    - Mask of bits in the port to write
  val     - Value to write to the masked bits in the port

DEPENDENCIES
  If the port needs to be tri-stated to allow output, it must be
  tri-stated before the output will be effected by this macro call.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef I2C_UT
#define I2C_GPIO_OUT(gpio_id, mask, val) \
{ \
  uint32 temp; \
  word isave; \
  INTLOCK_SAV(isave);\
  temp = bio_gpio[gpio_id].out_buf; \
  temp = (temp & ~(mask)) | ((mask) & (val)); \
  (void) gpio_sim_outp( bio_gpio[gpio_id].out_addr, temp); \
  bio_gpio[gpio_id].out_buf = temp; \
  INTFREE_SAV(isave);\
}
#else
#define I2C_GPIO_OUT(gpio_id, mask, val) \
  BIO_OUT((gpio_id), (mask), (val))
#endif


/*===========================================================================

MACRO I2C_GPIO_TRISTATE

DESCRIPTION
  This macro tri-states specified bits in a port, qualified by a specified
  mask, of a specified port.  Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values.  To do this,
  an image is maintained of the previous value written to the port which is
  combined with the new value prior to writing.  Writing a bit to 1 enables
  it as an output, writing a 0 tri-states it as an input.

PARAMETERS
  gpio_id - target GPIO id
  mask    - Mask of bits in the port to tri-state enable
  val     - Value to write to the masked bits in the port to configure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef I2C_UT
#define I2C_GPIO_TRISTATE(gpio_id, mask, val) \
{ \
  uint32 temp; \
  word isave; \
  INTLOCK_SAV(isave);\
  temp = bio_gpio[gpio_id].ts_buf; \
  temp = (temp & ~(mask)) | ((mask) & (val)); \
  (void) gpio_sim_outp( bio_gpio[gpio_id].tsen_addr, temp); \
  bio_gpio[gpio_id].ts_buf = temp; \
  INTFREE_SAV(isave);\
}
#else
#define I2C_GPIO_TRISTATE(gpio_id, mask, val) \
  BIO_TRISTATE((gpio_id), (mask), (val))
#endif


/*===========================================================================

MACRO I2C_GPIO_INM

DESCRIPTION
  This macro reads a value from a specified I/O port address.

PARAMETERS
  gpio_id - target GPIO id
  mask    - Mask of bits in the port to read

DEPENDENCIES
  The port must be tristated for input.

RETURN VALUE
  Value read from the I/O register AND'd with 'mask'.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef I2C_UT
#define I2C_GPIO_INM(gpio_id, mask) \
        ((mask) & gpio_sim_inp(bio_gpio[gpio_id].in_addr))
#else
#define I2C_GPIO_INM(gpio_id, mask) BIO_INM((gpio_id), (mask))
#endif

/*===========================================================================

MACRO I2C_WRITE_SDA

DESCRIPTION
  Write the serial data line. Possible values are high or low.
  The effect of writing the SDA is state dependent. Possible effects are:
  Can lead to a start or stop condition if transitioned when clock is high.
  It can also be used to transmit a data bit to the slave with correct
  clock timing.

PARAMETERS
  bus_id - id of target I2C bus
  val    - position to set SDA in (hi or lo)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_WRITE_SDA(bus_id, val) \
  I2C_GPIO_OUT(i2c_io_info[(bus_id)].gpio_id, i2c_io_info[bus_id].sda_mask, \
                (val))


/*===========================================================================

MACRO I2C_WRITE_SCL

DESCRIPTION
  Write the serial clock line. Possible values are high or low.
  The effect of writing the SCL is state dependent. If the bus has already
  been STARTed, clock pulses can be used to transmit or read data bits to/
  from the slave. If the SCL is high, SDA transitions could cause a
  START, repeated START or a STOP condition.

PARAMETERS
  bus_id - id of target I2C bus
  val    - position to set SCL in (hi or lo)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_WRITE_SCL(bus_id, val) \
  I2C_GPIO_OUT(i2c_io_info[(bus_id)].gpio_id, i2c_io_info[(bus_id)].scl_mask,\
               (val))


/*===========================================================================

MACRO I2C_READ_SDA

DESCRIPTION
  Return the serial data line value.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  None

RETURN VALUE
  I2C_BIT_HI if the returned data value is high.
  I2C_BIT_LO if the returned data value is low.

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_READ_SDA(bus_id) \
  ((I2C_GPIO_INM(i2c_io_info[(bus_id)].gpio_id, \
                 i2c_io_info[(bus_id)].sda_mask))\
                 ? I2C_BIT_HI : I2C_BIT_LO)

#define I2C_READ_SCL(bus_id) \
  ((I2C_GPIO_INM(i2c_io_info[(bus_id)].gpio_id, \
                 i2c_io_info[(bus_id)].scl_mask))\
                 ? I2C_BIT_HI : I2C_BIT_LO)


/*===========================================================================

MACRO I2C_CLK_DATA_IO

DESCRIPTION
  Transition the serial clock line from low to high to low with
  the proper timing. When writing, this clocks out a data bit.

  The following diagram shows an example of clocking out a data bit. All
  timings shown are minumum values and the data line must be stable during
  the rising edge and high period of the clock.

  SDA            ---------------
                 |             |
                 |             |
      ------------             -----------
             Tsu,dat(.25us)
               ->| |<-
                   |
      |<---Tlow--->|<--Thigh-->|
      |   4.7us    |   4us     |
  SCL--            -------------
      |            |           |
      |            |           |
      --------------           -----

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  SCL must be low before this routine is invoked.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low.

===========================================================================*/
#define I2C_CLK_DATA_IO(bus_id) \
{\
  I2C_WRITE_SCL((bus_id), I2C_BIT_HI); \
  I2C_THIGH_DELAY; \
  I2C_WRITE_SCL((bus_id), I2C_BIT_LO); \
  I2C_TLOW_DELAY;\
}


/*===========================================================================

MACRO I2C_SDA_OUTPUT_ENA

DESCRIPTION
  Configure the serial data line for writing.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_SDA_OUTPUT_ENA(bus_id)  \
  I2C_GPIO_TRISTATE( i2c_io_info[(bus_id)].gpio_id, \
                     i2c_io_info[(bus_id)].sda_mask, (BIO_OUTPUT_ENA_V))


/*===========================================================================

MACRO I2C_SDA_TRISTATE_R

DESCRIPTION
  Configure the serial data line for reading.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_SDA_TRISTATE_R(bus_id)  \
  I2C_GPIO_TRISTATE( i2c_io_info[(bus_id)].gpio_id, \
                     i2c_io_info[(bus_id)].sda_mask, (BIO_OUTPUT_DIS_V) )


/*===========================================================================

MACRO I2C_SCL_INIT

DESCRIPTION
  Initialize the serial clock line to a known state.
  The serial clock gpio is configured as an output and left high.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  Only to be used after powerup before using the bus for I/O.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define I2C_SCL_INIT(bus_id) \
{\
  I2C_WRITE_SCL((bus_id), I2C_BIT_HI); \
  I2C_GPIO_TRISTATE(i2c_io_info[(bus_id)].gpio_id, \
                    i2c_io_info[(bus_id)].scl_mask, BIO_OUTPUT_ENA_V); \
}


/*===========================================================================

MACRO I2C_SDA_INIT

DESCRIPTION
  Set the serial data line to a known state.
  The serial data line is configured as an output and left high.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  Only to be used after powerup before using the bus for I/O.

RETURN VALUE
  None

===========================================================================*/
#define I2C_SDA_INIT(bus_id) \
{ \
  I2C_WRITE_SDA((bus_id), I2C_BIT_HI);\
  I2C_SDA_OUTPUT_ENA((bus_id)); \
}


/*===========================================================================

MACRO I2C_START_COND

DESCRIPTION
  Generate a START condition. A start condition is a data transition from
  high to low while the clock is high.

  The following diagram shows the timing for a START(S) condition.
  The rise and fall times of SCL/SDA have been ignored for purposes of
  illustration.

  SDA ------
           |
           |
           -------------- . . .
           |<-Thd,sta->|
               (S)4us
  SCL ------------------
                       |
                       |
                       -- . . .

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left low

===========================================================================*/
#define I2C_START_COND(bus_id) \
{ \
  /* The rise and fall times of SCL and SDA are much smaller than      */ \
  /* the SCL and SDA GPIO pin access times. So, they are not considered*/ \
  /* in defining the macro below.                                      */ \
  I2C_WRITE_SDA((bus_id), I2C_BIT_HI); /* raise SDA */ \
  I2C_WRITE_SCL((bus_id), I2C_BIT_HI); /* raise SCL */ \
  I2C_WRITE_SDA((bus_id), I2C_BIT_LO); /* lower SDA */ \
  I2C_THD_STA_DELAY; \
  I2C_WRITE_SCL((bus_id), I2C_BIT_LO); /* lower SCL */ \
} /* i2c_start_cond */



/*===========================================================================

MACRO I2C_REP_START_COND

DESCRIPTION
  Generate a repeated start condition. A repeated START is used to continue
  using an I2C bus without generating a STOP condition.

  A repeated START saves Tbuf(4.7us) as opposed to doing a STOP and a START.

  The following diagram shows an ordinary START followed by a repeated START
  at some arbitrary time. The repeated START(Sr) is the only valid START that
  can be generated without a preceding STOP condition. The rise and fall
  times of SCL/SDA have been ignored for purposes of illustration.

  SDA ------                    -----------------
           |                    |               |
           |                    |               |
           -------------- . . . -               --------------------
           |<-Thd,sta->|           |<-Tsu,sta-->|<-Thd,sta->|
               (S)4us                  4.7us    |   (Sr)4us
  SCL ------------------           --------------------------
                       |           |                        |
                       |           |                        |
                       -- . . .-----                        --------

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left high

===========================================================================*/
#define I2C_REP_START_COND(bus_id) \
{ \
  /* always enable SDA for output incase ERR_FATAL interrupting a read */ \
  I2C_SDA_OUTPUT_ENA((bus_id))  \
  /* always raise SDA & SCL high incase ERR_FATAL interrupting I/O op. */\
  I2C_WRITE_SDA((bus_id), I2C_BIT_HI); \
  I2C_WRITE_SCL((bus_id), I2C_BIT_HI); \
  I2C_TSU_STA_DELAY; \
  /* SDA transition while SCL is high */ \
  I2C_WRITE_SDA((bus_id), I2C_BIT_LO); \
  I2C_THD_STA_DELAY; \
  I2C_WRITE_SCL((bus_id), I2C_BIT_LO); \
  /* Default data line to high postion. */ \
  I2C_WRITE_SDA((bus_id), (I2C_BIT_HI)); \
} /* I2C_REP_START_COND_INT */


/*===========================================================================

MACRO I2C_STOP_COND

DESCRIPTION
  Generate a STOP condition.
  A stop condition is a data transition from low to high while the clock is
  high.

  The following diagram shows the timing for a STOP(P) condition.
  The rise and fall times of SCL/SDA have been ignored for purposes of
  illustration.

  SDA             -----------
                  |         |
                  |         |
     --------------         ------
      |<-Tsu,sto->|<-Tbuf-->|
      | (P)4us    |  4.7us  |
  SCL -------------------------
      |
      |
     -

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.
  SCL must be low.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left high

===========================================================================*/
#define I2C_STOP_COND(bus_id) \
{ \
  /* Data line starts in an unknown state */ \
  I2C_WRITE_SDA((bus_id), I2C_BIT_LO);  /* lower SDA */ \
  I2C_WRITE_SCL((bus_id), I2C_BIT_HI);  /* raise SCL */ \
  I2C_TSU_STO_DELAY;  \
  I2C_WRITE_SDA((bus_id), I2C_BIT_HI);  /* raise SDA */  \
  I2C_TBUF_DELAY;  \
} /* i2c_set_stop_cond */


/*===========================================================================

MACRO I2C_ACK_BYTE

DESCRIPTION
  Write the acknowledge bit to the slave to ack a byte that was just read.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left low
  SDA is configured for output

===========================================================================*/
#define I2C_ACK_BYTE(bus_id) \
{ \
    /* Transmit a '0' */ \
    I2C_WRITE_SDA((bus_id), I2C_BIT_LO); \
    /* Configure writing */ \
    I2C_SDA_OUTPUT_ENA((bus_id));  \
    /* Clock the bit out */ \
    I2C_CLK_DATA_IO((bus_id));  \
}


/*===========================================================================

MACRO I2C_NACK_BYTE

DESCRIPTION
  Write the nack bit to the slave.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left high
  SDA is configured for output

===========================================================================*/
#define I2C_NACK_BYTE(bus_id) \
{ \
    /* Transmit a '1' */ \
    I2C_WRITE_SDA((bus_id), I2C_BIT_HI); \
    /* Configure writing */ \
    I2C_SDA_OUTPUT_ENA((bus_id));  \
    /* Clock the bit out */ \
    I2C_CLK_DATA_IO((bus_id));  \
}


/*===========================================================================

MACRO I2C_NOP_BYTE

DESCRIPTION
  Run an idle clock pulse to let the slave pull SDA low after the last byte
  has been read from the slave. This is used to give the slave sufficient
  time to release the SDA line.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  SCL is left low
  SDA is left high
  SDA is configured for output

===========================================================================*/
#define I2C_NOP_BYTE(bus_id) \
{ \
    /* Run the clock */ \
    I2C_CLK_DATA_IO((bus_id));  \
    /* Configure writing */ \
    I2C_SDA_OUTPUT_ENA((bus_id));  \
}


/*===========================================================================

MACRO I2C_SWITCH_IO_CTRL 
DESCRIPTION
  Setup GPIO for software controller.

PARAMETERS
  bus_id - id of target I2C bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  SCL and SDA are configured for software control.

===========================================================================*/
#if defined FEATURE_I2C_IO_CTRL_OVER_HW || defined FEATURE_I2C_IO_CTRL || \
    defined FEATURE_I2C_CHIP_ENABLE
#define I2C_SWITCH_IO_CTRL(bus_id) \
{ \
   /* Switch to software IO control */ \
   gpio_i2c_un_config(); \
}
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW || FEATURE_I2C_IO_CTRL */


/*===========================================================================

MACRO I2C_SETUP_HW_CTRL

DESCRIPTION
  Setup GPIO and SCL frequency for hardware controller.

PARAMETERS
  bus_id - id of target I2C bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  SCL and SDA are configured for hardware control.

===========================================================================*/
#define I2C_SETUP_HW_CTRL(bus_id) \
{ \
  /* Diable SDA & SCL Output */ \
  BIO_TRISTATE(i2c_io_info[bus_id].gpio_id, \
               i2c_io_info[bus_id].scl_mask | i2c_io_info[bus_id].sda_mask, \
               BIO_OUTPUT_DIS_V); \
  /* Switch to I2C hardware controller */ \
  gpio_i2c_config(); \
  clk_busy_wait(I2C_HW_SWITCH_DELAY(fs_freq)); \
  /* Setup SCL clock frequency */ \
  HWIO_OUT(I2C_CLK_CTL, I2C_SCL_FREQ_CAL( i2c_fs_freq_khz )); \
}


/*===========================================================================

MACRO I2C_SCL_SDA_HI

DESCRIPTION
  SCL and SDA set to HI.

PARAMETERS
  bus_id - id of target I2C bus

DEPENDENCIES
  Must be in software controller mode.

RETURN VALUE
  None.

SIDE EFFECTS
  SCL is left high.
  SDA is left high.

===========================================================================*/
#define I2C_SCL_SDA_HI(bus_id) \
  I2C_GPIO_OUT(i2c_io_info[bus_id].gpio_id, \
               i2c_io_info[bus_id].scl_mask | i2c_io_info[bus_id].sda_mask, \
               BIO_OUTPUT_SET_V)


/*===========================================================================

MACRO I2C_CLK_XXX

DESCRIPTION
  clk_regime related defines based on MSM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
#ifdef T_MSM7500
#error code not present
#else /* T_MSM7500 */
	#ifdef FEATURE_DUAL_CLK_RGM
			#define I2C_CLK_RESET() clk_regime_msm_reset(CLK_RGM_I2C_M)
			#define I2C_CLK_ENABLE() clk_regime_msm_enable(CLK_RGM_I2C_M)
			#define I2C_CLK_DISABLE() clk_regime_msm_disable(CLK_RGM_I2C_M)
	#else
			#define I2C_CLK_RESET() clk_regime_reset(CLK_RGM_I2C_M)
			#define I2C_CLK_ENABLE() clk_regime_enable(CLK_RGM_I2C_M)
			#define I2C_CLK_DISABLE() clk_regime_disable(CLK_RGM_I2C_M)
	#endif
#endif  /* T_MSM7500 */

/* for ulc me have to take care of the GSBI wraps too */

#ifdef FEATURE_GSBI0_I2C
	#define I2C_CLK_RESET() clk_regime_msm_reset(CLK_RGM_I2C0_M)
    #define I2C_CLK_ENABLE() clk_regime_msm_enable(CLK_RGM_I2C0_M)
	#define I2C_CLK_DISABLE() clk_regime_msm_disable(CLK_RGM_I2C0_M)

#elif defined FEATURE_GSBI1_I2C
	#define I2C_CLK_RESET() clk_regime_msm_reset(CLK_RGM_I2C1_M)
    #define I2C_CLK_ENABLE() clk_regime_msm_enable(CLK_RGM_I2C1_M)
	#define I2C_CLK_DISABLE() clk_regime_msm_disable(CLK_RGM_I2C1_M)

#elif defined FEATURE_GSBI2_I2C

	#define I2C_CLK_RESET() clk_regime_msm_reset(CLK_RGM_I2C2_M)
    #define I2C_CLK_ENABLE() clk_regime_msm_enable(CLK_RGM_I2C2_M)
	#define I2C_CLK_DISABLE() clk_regime_msm_disable(CLK_RGM_I2C2_M)

#endif


#endif /* FEATURE_I2C */
#endif /* I2CI_H */

