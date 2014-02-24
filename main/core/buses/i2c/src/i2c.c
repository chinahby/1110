/* #pragma linesize(132)
** #pragma pagesize(60)
** #pragma title("Dual Mode Subscriber Station")
** #pragma subtitle("I2C Driver ")
*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      I 2 C   L I B R A R Y   R O U T I N E S

GENERAL DESCRIPTION
  This module provides the low level services required to read and write
  I2C slave devices.

EXTERNALIZED FUNCTIONS
  i2c_init
    Initialize the device

  i2c_read
    Read one or more bytes from the device

  i2c_write
    Write one or more bytes to the device

REFERENCES
  Philips Semiconductors - I2C Peripherals for Microcontrollers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  i2c_init should be called prior to using an I2C bus for I/O.

Copyright (c) 2004      by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

   $Header: //depot/asic/msmshared/drivers/i2c/i2c.c#32 $ $DateTime: 2008/09/18 13:52:20 $ $Author: psanghvi $


when       who     what, where, why
--------   ---     -------------------------------------------------------
09/18/08   ps      Fixed CR 151652. On some read transactions, the I2C 
                   controller generated one extra read cycle. While 
                   HW was busy doing this extra read cycle, if client 
                   code called I2C R/W API, it used to fail thus resetting 
                   SCL pin high and the slave device was left in an unknown state.
04/08/08   ps      Updated the i2c_wait_wr_buf_clr function to account for 
                   slower slave devices which can pull I2C clock signal low 
                   for extended period. 
04/22/08   pp      added fixes for Klocwork errors
04/18/06   hz      Fixed CR#109076: FFA does not work with external SONY 2MP 
                   or 3MP pufflegs - pull down SCL after enable (For CE only).
11/03/06   hz      Added support for 
                   a. Dynamic speed change: i2c_operation_fs_set(); 
                   b. Combined mode: i2c_combined_write_read(). 
                   c. SDA/SCL low stuck recovery i2c_low_stuck_recovery();
05/22/06   dl      Added a function to invert i2c_ce pin.
03/24/06   dle     Replaced HWIO_OUT calls with I2C_CLK_*.
                   Replaced I2C_HW_CTRL_RESET with I2C_CLK_RESET.
02/09/06   nrw     QSC6030 needs rex.h for rex_crit_sect_type.
12/08/05   dle     Instead of using new FEATURE_I2C_CLK_RESET or T_MSM6500 to
                   call clk_regime_msm_reset() or clk_regime_reset(), simply
                   let (undef)FEATURE_I2C_SKIP_CLK_RESET (or T_6500) do the job
                   of executing I2C__HW_CTRL_RESET.
                   Change to use BIO_INM, BIO_OUT. Use of inpdw() and outpdw()
                   is not context safe.
                   Corrected loop_count in i2c_wait_wr_buf_clr().
                   i2c_get_ctrller_state() is modified to do wait for
                   read_last and wait not only by time but also for the state.
                   Changed i2c_read_last() interface so the parameter is not
                   read_one_only, but whether to signal LAST_BYTE to HW.
                   Also introduced FEATURE_I2C_EXTRA_READ_CLK for devices that
                   needs to have extra read clocks. i2c_read_last() would set
                   LAST_BYTE after data byte is in, clock would stop one byte
                   after current read.
                   i2c_init() is modified so critical section is only init once.
                   In i2c_read() and i2c_write(), if I2C state is stuck in
                   other than IDLE state, re-init I2C.
Added comment      When reading only one byte, LAST_BYTE is set in
                   i2c_write_addr_read_one_only() before read so clock is
                   stopped after one read.
08/22/05   dle     Make i2c_read_last() work for Sirius.
07/22/05   dle     i2c_read_last() would set LAST_BYTE first, clock would
                   stop at the end of current read.
07/13/05   dle     Modified i2c_write_addr_read_one_only() so that we wait
                   for RX state instead of interrupt. At entry to this
                   function, it is possible the previous write data is
                   still in the shift register, so at the time of next
                   interrupt, currently written data may still in the holding
                   register. We must wait for all data to be out and I2C is in
                   RX state before signaling last byte.
                   In i2c_read(), if I2C_DFLT_ADDR_DEV, use FALSE when calling
                   i2c_read_last() indicating not reading last byte.
                   At the end of each I2C_BUS_CE_CTRL write, make sure SCL and
                   SDA are high.
07/12/05   dle     Added MSMHW_I2C_CLK_RESET, same effect as T_MSM6500.
05/31/05   dle     When read, we clocked for one extra byte. Some devices
                   cannot handle that. Fixed by using _read_one_only related
                   code.
04/06/05   sai     Corrected error in i2c_wait_wr_buf_clr due to loop_count 
                   never being zero. Fixed compilation error in i2c_write_addr.
04/01/05   lrw     Add FEATURE_DUAL_CLK_RGM parameter
01/12/05   lrw     Correct inport for GPIO reading to prevent over-write into 
                   other GPIO address for i2c_write_byte_io and i2c_read_byte_io
11/17/04   dle     Made I2C_CE_INIT, I2C_CHIP_ENABLE and I2C_CHIP_DISABLE
                   macros into function calls.
10/14/04   mvd     In i2c write, performing clk reset and path setup 
                   again to fix Camera sensor driver issue where focus
                   stepping motor does not respond.
10/12/04   dle     After writing last byte, make sure the status is idle,
                   otherwise wait.
09/30/04   dle     In i2c_setup_slave, need to update status when calling
                   i2c_write_addr.
09/21/04   dle     Renamed macros related to CHIP_ENABLE.
                   In i2c_read() or i2c_write(), if bus_id has changed, need
                   to do setup_path(). For 6500, also need to clk_regime_reset.
08/24/04   dle     Added back change history. Replace 3WIRE with
                   3WIRE_SW_CTRL with CE_CTRL and featuriz with
                   FEATURE_I2C_CHIP_ENABLE. Removed i2c_write_byte_serial
                   and modified i2c_write_byte_io for CE_CTRL.
08/06/04   sch     Merged John Hacklander's changes to provide the same I2C
                   evironment as Jaguar.
06/11/04   sch     Run time detection of serial protocol.
06/11/04   rve     3 wire serial protocol support.
05/27/04   dle     Merged in 6500 driver. 6500 I2C needs reset at init time.
02/06/03   dle     At i2c_wait_wr_buf_clr(), loop for 12 bit time only, or may
                   stuck forever.
10/20/03    sr     Modified interface to clk regime by introducing function
                   clk_regime_get_i2c_clk_khz. This eliminates clk_div_sel_val
                   and all references to hclk.
07/21/03   dle     Modified i2c_get_ctrller_state() to reduce un-necessary
                   wait. Use _freq_khz instead of _freq_mhz.
06/24/03   dle     Removed switching in and out of HW_CTRL when read or
                   _write() is called. As long as the I2C HW maintains
                   the GPIO pins at high when in-active, there is no need
                   to switch in and out.
                   Added support to setup_path when clock or divide is
                   changed.
                   Featurized (FEATURE_I2C_IO_CTRL_OVER_H) options
                   (I2C_NOP_LAST_BYTE and I2C_CLK_START_BEFORE_RW) where there
                   is a need to switch to IO_CTRL in order to support these
                   options.
05/29/03   dle     Moved I2C_HW_CTRL_SCL_FREQ to i2c.h. Make use of
                   clkrgm_i2c_clk_div_sel_val. Rearanged clk_busy_wait call.
02/18/03   dle     If HW_CTRL, then when not used, SWITCH_IO_CTRL the GPIO to
                   save power.
12/20/02   dle     Changed word to dword, outpw to putpdw, inpw to inpdw.
10/08/02   dle     Remove checking of CLK_STATE and DATA_STATE. Was used for
                   ASIC testing, should not be used by software. Software
                   had checked these states too early in time as well. The
                   designer says don't check it.
10/04/02   dle     Modified to use real fs_freq for delay calculation.
09/18/02   dle     i2c_stop takes another parameter to determing if wait
                   for read buffer full is needed. Removed i2c_stop_read.
09/17/02   dle     i2c_stop() does not wait for read buffer full.
                   Added i2c_stop_read() to be used for read function.
                   Added i2c_crit_sect.
03/21/02   aw      Modified code to only check clk and data states in
                   i2c_wait_wr_buf_clr() if not writing the last byte.
03/12/02   aw      Reset the hardware controller in i2c_init() to clean up
                   the status.
03/11/02   aw      Reorganized I2C driver and added support for hardware
                   controller.
07/20/01   sm      Replaced DEC_GPIO_0 with BIO_EEPROM_REG.
04/14/00   sm      Featurized I2C driver.
02/21/00   sm      Modified for SDAC target.  Removed non ARM support.
                   Ported for MSM3100.
07/01/99   sk      Modified definition to use only CODEC (no NV) for Kenny.
04/21/99   rv      Added support for Kenny.
10/22/98   dlb     Added support for MSM3/ARM.
09/18/98   sk      Added code to support Q. Fixed the uninitialized R/W slave
                   address bit for default address devices.
05/11/98   sk      Added REN definitions for SSS2. Added code to use current
                   address as default for I/O (user-specified option).
02/27/98   sk      Removed GPIO definitions and used register names to access
                   GPIOs.
02/09/98   sk      Incorporated code review comments.
12/15/97   sk      File created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_I2C
#include "bio.h"
#include "err.h"
#include "i2c.h"
#include "i2ci.h"
#include "rex.h"
#ifdef CLKRGM_H
#include CLKRGM_H
#endif

LOCAL uint32 clk_i2c_freq_khz = 0;
#if defined (FEATURE_VERSION_C316) || defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)|| defined(FEATURE_VERSION_K232_Y100A)
#define FEATURE_I2C_STUCK_RECOVERY
#endif
/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_I2C_CHIP_ENABLE
static boolean i2c_ce_invert_pin = FALSE;
static void i2c_chip_disable(void);
static void i2c_chip_enable(void);
static void i2c_ce_init(void);
#endif /* FEATURE_I2C_CHIP_ENABLE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Prevents clk from being changed while I2C is active */
boolean i2c_active = FALSE;
LOCAL uint32 fs_freq = I2C_HW_CTRL_SCL_FREQ_STANDARD; /* in KHz. */
LOCAL uint32 i2c_fs_freq_khz = I2C_HW_CTRL_SCL_FREQ_STANDARD; /* in KHz. */
#ifdef FEATURE_I2C_CHIP_ENABLE
LOCAL static last_byte = FALSE;
#endif /* FEATURE_I2C_CHIP_ENABLE */

/* I2C port addresses, GPIO shadow buffers and SCL/SDA masks */
LOCAL i2c_io_type i2c_io_info[] =
{

#ifdef FEATURE_GSBI0_I2C
{
    I2C_HW_CTRL,               /* I2C HW control                           */
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
}
#endif

#ifdef FEATURE_GSBI1_I2C
{
    I2C_HW_CTRL,               /* I2C HW control                           */
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
}
#endif

#ifdef FEATURE_GSBI2_I2C
{
    I2C_HW_CTRL,               /* I2C HW control                           */
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
}
#endif

#if !defined(FEATURE_GSBI0_I2C) && !defined (FEATURE_GSBI1_I2C) && !defined (FEATURE_GSBI2_I2C)
/* hanging i2cc definitions */
  {
    I2C_HW_CTRL,               /* I2C HW control                           */
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
  }
#endif
	
#ifdef FEATURE_I2C_IO_CTRL
  ,
  {
    I2C_IO_CTRL,               /* I2C SW IO control                        */
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
  }
#endif /* FEATURE_I2C_IO_CTRL */
#ifdef FEATURE_I2C_CHIP_ENABLE
  ,
  {
    I2C_CE_CTRL,
    BIO_EEPROM_REG,            /* I2C data & clock bus control register    */
    BIO_EEPROM_CLK_M,          /* I2C clock control register mask          */
    BIO_EEPROM_DATA_M,         /* I2C data control register mask           */
    I2C_UNINIT_S               /* I2C controller state                     */
  },
#endif /* FEATURE_I2C_CHIP_ENABLE */
};

LOCAL rex_crit_sect_type  i2c_crit_sect;

/* Define number of buses defined in i2c_io_info[] */
#define I2C_NUM_BUS   sizeof(i2c_io_info)/sizeof(i2c_io_type)

/* <EJECT> */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         Local Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_clock_halt

DESCRIPTION
  This function will halt or start the i2c hardware controller clock.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  MSM_CLK_HALT_RW register.

===========================================================================*/
#ifndef FEATURE_I2C_SKIP_CLK_HALT
LOCAL void i2c_clock_halt
(
  i2c_bus_type bus_id,
  boolean      halt
)
{
  if (i2c_io_info[bus_id].ctrl == I2C_HW_CTRL) {
    if (halt) {
      I2C_CLK_DISABLE();
    } else {
      I2C_CLK_ENABLE();
    }
  }
} /* i2c_clock_halt */
#endif /* FEATURE_I2C_SKIP_CLK_HALT */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_setup_path

DESCRIPTION
  This function will setup the GPIO pin for I2C hardware/software controller.

DEPENDENCIES
  i2c_io_info.ctrl

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void i2c_setup_path
(
  i2c_bus_type bus_id
)
{

  if( bus_id == I2C_BUS_INVALID )
  {
    ERR_FATAL("Invalid bus_id!",0,0,0); 
  }
  switch (i2c_io_info[bus_id].ctrl)
  {
#ifdef FEATURE_I2C_CHIP_ENABLE
    case I2C_CE_CTRL:
      I2C_SCL_SDA_HI(bus_id);          /* Initial SCL & SDA register bit   */
      I2C_SWITCH_IO_CTRL(bus_id);      /* Switch to software IO control    */
      break;
#endif /* FEATURE_I2C_CHIP_ENABLE */
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      I2C_SCL_SDA_HI(bus_id);          /* Initial SCL & SDA register bit   */
      I2C_SWITCH_IO_CTRL(bus_id);      /* Switch to software IO control    */
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      /* Need fs_freq in subsequent operations */
      fs_freq = clk_i2c_freq_khz / 2 /
        (I2C_FS_DIV(i2c_fs_freq_khz) + 3);
#ifndef FEATURE_I2C_SKIP_CLK_HALT
      i2c_clock_halt (bus_id, FALSE);  /* Activate hardware controller     */
#endif
      I2C_SCL_SDA_HI(bus_id);          /* Initial SCL & SDA register bit   */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
      I2C_SWITCH_IO_CTRL(bus_id);      /* Switch to IO control             */
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */
#if defined T_MSM6500 || !defined FEATURE_I2C_SKIP_CLK_RESET
      I2C_CLK_RESET();
#endif
      I2C_SETUP_HW_CTRL(bus_id);       /* Switch to Hardware control       */
#ifndef FEATURE_I2C_SKIP_CLK_HALT
      i2c_clock_halt (bus_id, TRUE);   /* Deactivate hardware controller   */
#endif
      break;

    default:
      ERR("Unknown controller type ",0,0,0);
  }
} /* i2c_setup_path */

/* <EJECT> */

#if defined FEATURE_I2C_IO_CTRL | defined FEATURE_I2C_CHIP_ENABLE
/*===========================================================================

FUNCTION I2C_READ_ACK

DESCRIPTION
  Read an acknowledge bit. The SDA is enabled for input before reading the
  data bit.

DEPENDENCIES
  i2c_init must be invoked prior to this call.
  SCL is expected to be low when called.

RETURN VALUE
  I2C_ACK or I2C_NACK depending on the value of the ack bit.

SIDE EFFECTS
  SDA is configured for writing.
  SCL is left low

===========================================================================*/
LOCAL byte i2c_read_ack
(
  i2c_bus_type bus_id
)
{
  byte ack_bit;                      /* storage of the ack bit */

  if( bus_id == I2C_BUS_INVALID )
  {
    ERR_FATAL("Invalid bus_id!",0,0,0); 
  }

  I2C_SDA_TRISTATE_R(bus_id);        /* Configure for reading */

  I2C_WRITE_SCL(bus_id, I2C_BIT_HI); /* Raise SCL */

  ack_bit = I2C_READ_SDA(bus_id);    /* read ack bit */

  I2C_WRITE_SCL(bus_id, I2C_BIT_LO); /* Lower SCL */

  I2C_SDA_OUTPUT_ENA(bus_id);        /* Configure for writing */

  return ack_bit;                    /* Return the ack bit */
} /* i2c_read_ack */

/* <EJECT> */
/*===========================================================================


FUNCTION I2C_WRITE_BYTE_IO

DESCRIPTION
  Write a byte and read the ACK bit out of the slave. This routine implements
  the level transmission of all the 8 data bits to the slave. All timing
  values documented are computed with respect to a 13.5 MHz clock.

  The code is in assembly as we need to implement accurate timing in us.

DEPENDENCIES
  i2c_setup_slave must be invoked prior to this call.
  SCL is expected to be low when called.
  Function entry and exit only verified for Microsoft C v60a.

RETURN VALUE
  I2C_ACK  if the slave ack's the write.
  I2C_NACK if the slave did not ack the write.

SIDE EFFECTS
  SCL is left low.
  SDA state depends on ACK or NACK from slave.
  Updates shadow buffer to have the most current value.

===========================================================================*/
LOCAL byte i2c_write_byte_io
(
  i2c_bus_type    bus_id,    /* id of target I2C bus */
  byte            val,       /* value to write */
  boolean         chip_en    /* Whether chip enable is used */
)
{
  int count;

  dword bit_value;

  for (count = 0; count < 8; count++)
    {
      uint32 sav;
      INTLOCK_SAV(sav);

      /* Determine if the data bit to be transmitted is a '1' or '0'. */
      bit_value = ((val & 0x80) != 0) ? BIO_EEPROM_DATA_M : 0;

      /*
      ** To transmit a bit to slave, raise the data bit and then run
      ** a clock cycle with timings as shown below.
      **   SDA            ---------------
      **                 |             |
      **                 |             |
      **      ------------             -----------
      **             Tsu,dat(.25us)
      **               ->| |<-
      **                   |
      **      |<---Tlow--->|<--Thigh-->|
      **      |   4.7us    |   4us     |
      **  SCL--            -------------
      **      |            |           |
      **      |            |           |
      **      --------------           -----
      */
      /* equivalent to: I2C_WRITE_SDA (bus_id, bit_value); */
      BIO_OUT( BIO_EEPROM_REG, BIO_EEPROM_DATA_M, bit_value);


      /* clk_nano_wait will wait at least 200ns.  The delay needed is
         250.  However, as each instruction takes 74, the instructions
         between will take at least 50. */
      clk_nano_wait ();

      /* raise the clock: I2C_WRITE_SCL (bus_id, I2C_BIT_HI); */
      BIO_OUT( BIO_EEPROM_REG, BIO_EEPROM_CLK_M, BIO_EEPROM_CLK_M);

      /* Wait on raising clock. */
      I2C_THIGH_DELAY;

      /* Lower the clock. I2C_WRITE_SCL (bus_id, I2C_BIT_LO); */
#ifdef FEATURE_I2C_CHIP_ENABLE
      if (!chip_en || !(count == 7) ||!last_byte)
#endif /* FEATURE_I2C_CHIP_ENABLE */
      {
        BIO_OUT( BIO_EEPROM_REG, BIO_EEPROM_CLK_M, 0);
      }

      /* Wait on low.  The delay of 4.7us could be reduced here for a
         performance increase.  The INTFREE and INTLOCK are somewhat
         expensive.  However, depending on their timing will affect
         how this code is implemented.  So, for now, guarantee our
         timing. */
      I2C_TLOW_DELAY;

      /* Go get the next bit. */
      val <<= 1;

      /* Free interrupts to allow a chance for any to happen. */

      INTFREE_SAV(sav);
    }

#ifdef FEATURE_I2C_CHIP_ENABLE
  if (chip_en)
  {
    return (I2C_ACK);
  }
  else
#endif /* FEATURE_I2C_CHIP_ENABLE */
  {
    return i2c_read_ack (bus_id);
  }
} /* i2c_write_byte_io */


/* <EJECT> */
/*===========================================================================

FUNCTION I2C_READ_BYTE_IO

DESCRIPTION
  Read a byte from the slave.

  The code is in assembly as we need to implement accurate timing in us.

DEPENDENCIES
  i2c_setup_slave must be invoked prior to this call.
  SCL is expected to be low when called.
  SDA must be configured as input
  Function entry and exit only verified for Microsoft C v60a

RETURN VALUE
  The value of the byte read.

SIDE EFFECTS
  SCL is left low.

===========================================================================*/
LOCAL byte i2c_read_byte_io
(
  i2c_bus_type    bus_id    /* id of target I2C bus */
)
{
  int result = 0;
  int count;

  for (count = 0; count < 8; count++)
    {
      uint32 sav;
      INTLOCK_SAV(sav);

      /*
      ** To read a data bit from the slave, read data bit in the high
      ** period of the clock. The required timings are shown below.
      **
      **      |<---Tlow--->|<--Thigh-->|
      **      |   4.7us    |   4us     |
      **  SCL--            -------------
      **      |            |           |
      **      |            |           |
      **      --------------           -----
      **
      */
      /* Wait initial time.  Could be shortened due to time spent in
         INTLOCK and INTFREE, but then we would depend on their
         implementation.  Should be visited if this is a performance
         issue. */
      I2C_TLOW_DELAY;

      /* Now raise the clock: I2C_WRITE_SCL (bus_id, I2C_BIT_HI); */
      BIO_OUT( BIO_EEPROM_REG, BIO_EEPROM_CLK_M, BIO_EEPROM_CLK_M);

      /* Wait the high delay time.  The best time to sample the data
         line is right before we lower the clock again. */
      I2C_THIGH_DELAY;

      /* Read in the data bit. */
      result <<= 1;
      /* if (I2C_READ_SDA (bus_id) == I2C_BIT_HI) */
      if (BIO_INM( BIO_EEPROM_REG, BIO_EEPROM_DATA_M))
      {
        result |= 1;
      }

      /* Lower the clock: I2C_WRITE_SCL (bus_id, I2C_BIT_LO); */
      BIO_OUT( BIO_EEPROM_REG, BIO_EEPROM_CLK_M, 0);

      INTFREE_SAV(sav);
    }

  return (byte) result;
} /* byte i2c_read_byte_io */
#endif /* FEATURE_I2C_IO_CTRL | defined FEATURE_I2C_CHIP_ENABLE */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_wait_wr_buf_clr

DESCRIPTION
  This function check with the wr_buffer_full bit in I2C_STATUS register.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write buffer is not full
  I2C_FAILURE - write error.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_wait_wr_buf_clr
(
  boolean last_byte
)
{
  word status;
  uint32 loop_count = 15; 
/* We wait maximum of 15 * 11  = 165 Bit time. This is beacause some slave 
   device might be slow and may pull SCL line down more than the usual 
   11 bit time period
   At 100Kbps, this would account for a maximum wait of  
   165 * 10us = 1650us or 1.65msec 
   At 400Kbps, this would account for a maximum wait of  
   165 * 2.5us = 412.5us or 0.4125msec 
*/



  /* Polling for write buffer */
  do {

    clk_busy_wait(I2C_HW_CTRL_DELAY(fs_freq));
    if (loop_count == 0)
    {
      break;
    }

    status = HWIO_IN(I2C_STATUS);

    /* ERR */
    if (status & I2C_STATUS_RH__ERROR_MASK)
    {
      return (I2C_FAILURE);
    }
    --loop_count;

  } while (status & HWIO_FMSK(I2C_STATUS,WR_BUFFER_FULL));
  if (loop_count)
  {
    return (I2C_SUCCESS);
  }
  else
  {
    return (I2C_FAILURE);
  }
} /* i2c_wait_wr_buf_clr */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_wait_rd_buf_full

DESCRIPTION
  This function check the rd_buffer_full bit in I2C_STATUS register.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - read buffer is full.
  I2C_FAILURE - read error.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_wait_rd_buf_full (void)
{
  word status;
  dword count = 0;

  do {
    status = HWIO_IN(I2C_STATUS);

    /* ERR */
    if (status & I2C_STATUS_RH__ERROR_MASK)
    {
      return (I2C_FAILURE);
    }

    /* Timeout, when a real device is attached, the loop count
     * is 586. 10000 should be save enough to filter out
     * phantom device */
    if (count++ > 10000)
    {
      return (I2C_FAILURE);
    }
  } while ((status & HWIO_FMSK(I2C_STATUS,RD_BUFFER_FULL))==0);
  return (I2C_SUCCESS);
} /* i2c_wait_rd_buf_full */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_write_byte_hw

DESCRIPTION
  This function write a byte to the i2c hardware controller and wait for the
  write buffer get clean up.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write successful.
  I2C_FAILURE - write failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_write_byte_hw
(
  word            val        /* value to write */
)
{
  i2c_status_type status;

  HWIO_OUT(I2C_WRITE_DATA, val);
  status = i2c_wait_wr_buf_clr( ((val & I2C_LAST_BYTE) != 0) );

  return (status);
} /* i2c_write_byte_hw */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_read_byte_hw

DESCRIPTION
  This function wait until the I2C hardware controlleri's read buffer is full
  and read in the data.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - read successful.
  I2C_FAILURE - read failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_read_byte_hw
(
  byte *val
)
{
  i2c_status_type status;

  status = i2c_wait_rd_buf_full();

  *val = HWIO_IN(I2C_READ_DATA);

  return (status);
} /* i2c_read_byte_hw */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_get_ctrller_state

DESCRIPTION
  This function converts I2C hardware controller's state to local state.

DEPENDENCIES
  Input parameter read_last should only be set to true for last byte read.

RETURN VALUE
  I2C_IDLE_S - controller in idle state.
  I2C_TX_S   - controller in TX_ADDR state or TX_DATA state.
  I2C_RX_S   - controller in RX_DATA state.
  I2C_FAIL_S - controller in unsupported state.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_state_type i2c_get_ctrller_state (boolean read_last)
{
  i2c_state_type state  = I2C_FAIL_S;
  uint16         data_state;

  if (read_last)
  {
    /* When running at 300kpbs, it is possible that data state
     * is still in RX_S after reading the last byte. So wait */
    /* at 100kbps, this is about 110us or 11 bit time on MSM6500. */
    uint32 loop_count = I2C_ARM_CLK_FACTOR * 7000 / fs_freq;

    data_state = HWIO_INM(I2C_STATUS, HWIO_RMSK(I2C_STATUS)) &
      HWIO_FMSK(I2C_STATUS,DATA_STATE);	
    while (data_state != I2C_DATA_RESET_WAIT_S)
    {
      if (loop_count == 0)
      {
        break;
      }
	 	  clk_busy_wait(1);
      data_state = HWIO_INM(I2C_STATUS, HWIO_RMSK(I2C_STATUS)) &
        HWIO_FMSK(I2C_STATUS,DATA_STATE);	
      loop_count--;
    }
    if (loop_count)
    {
	    state = I2C_IDLE_S;
    }
  }
  else
  {
    data_state = HWIO_INM(I2C_STATUS, HWIO_RMSK(I2C_STATUS)) &
      HWIO_FMSK(I2C_STATUS,DATA_STATE);	
    switch(data_state)
    {
      case I2C_DATA_RESET_WAIT_S:
        state = I2C_IDLE_S;
        break;
      case I2C_DATA_TX_ADDR_S:
      case I2C_DATA_TX_DATA_S:
        state = I2C_TX_S;
        break;
      case I2C_DATA_RX_DATA_S:
        state = I2C_RX_S;
        break;
      default:
        break;
    }
  }
  return (state);
} /* i2c_get_ctrller_state */

/* <EJECT> */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
/*===========================================================================

FUNCTION i2c_clk_start_before_rw

DESCRIPTION
  This function generate a clock pulse before I2C start. This is specially
  used for VBAP.

DEPENDENCIES
  i2c_io_info.ctrl

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void i2c_clk_start_before_rw
(
  i2c_bus_type bus_id
)
{
  switch( i2c_io_info[bus_id].ctrl ) {
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      I2C_CLK_DATA_IO(bus_id);             /* Generate a clock signal     */
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      I2C_SCL_SDA_HI(bus_id);              /* Setup SCL & SDA register bit*/
      I2C_SWITCH_IO_CTRL(bus_id);          /* Switch to use IO controller */
      I2C_CLK_DATA_IO(bus_id);             /* Generate a clock signal     */
      I2C_WRITE_SCL((bus_id), I2C_BIT_HI); /* Prepare for start           */
      I2C_THIGH_DELAY;                     /* Time delay                  */
#if defined T_MSM6500 || !defined FEATURE_I2C_SKIP_CLK_RESET
      I2C_CLK_RESET();
#endif
      I2C_SETUP_HW_CTRL(bus_id);           /* Switch to use HW controller */
      break;

    default:
      ERR("Unknown controller type ",0,0,0);
  }
} /* i2c_clk_start_before_rw */
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_stop

DESCRIPTION
  This function generates I2C STOP condition and reset i2c's bus state to
  IDLE.
  If wait_rd_buf_full is true then it will wait.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void i2c_stop
(
  i2c_bus_type bus_id,
  boolean wait_rd_buf_full
)
{
  i2c_state_type state = I2C_FAIL_S;

  
  if( bus_id == I2C_BUS_INVALID )
  {
    ERR_FATAL("Invalid bus_id!",0,0,0); 
  }

  switch (i2c_io_info[bus_id].ctrl) {
#ifdef FEATURE_I2C_CHIP_ENABLE
    case I2C_CE_CTRL:
      I2C_STOP_COND(bus_id);               /* Generate a STOP condition    */
      state = I2C_IDLE_S;                  /* Reset bus state to idle      */
      break;
#endif /* FEATURE_I2C_CHIP_ENABLE */
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      I2C_STOP_COND(bus_id);               /* Generate a STOP condition    */
      state = I2C_IDLE_S;                  /* Reset bus state to idle      */
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      if (wait_rd_buf_full)
        (void) i2c_wait_rd_buf_full();       /* Wait for previous cmd finish */
      I2C_WRITE_SCL(bus_id, I2C_BIT_LO);   /* lower SCL                    */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
      I2C_SWITCH_IO_CTRL(bus_id);          /* Switch to IO controller      */
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */
      I2C_STOP_COND(bus_id);               /* Generate a STOP condition    */
#if defined T_MSM6500 || !defined FEATURE_I2C_SKIP_CLK_RESET
      I2C_CLK_RESET();
#endif
      I2C_SETUP_HW_CTRL(bus_id);           /* Switch to HW controller      */
      state = I2C_IDLE_S;
      break;

    default:
      ERR("Unknown controller type",0,0,0);
  }
  i2c_io_info[bus_id].state = state;
} /* i2c_stop */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_write_addr

DESCRIPTION
  This function write the slave address to the i2c bus.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write successful.
  I2C_FAILURE - write failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_write_addr
(
  i2c_bus_type bus_id,
  byte         val
)
{
  i2c_state_type  state  = I2C_FAIL_S;
  i2c_status_type status = I2C_FAILURE;
  i2c_io_type     *bus   = &i2c_io_info[bus_id];

  switch(bus->ctrl) {
#ifdef FEATURE_I2C_CHIP_ENABLE
    case I2C_CE_CTRL:
      if (i2c_write_byte_io(bus_id, val, TRUE) == I2C_ACK) { /* Write addr byte  */
        /* State change depend on READ/WRITE */
        state = ((val & I2C_IO_MASK) == I2C_IO_MASK_R)? I2C_RX_S: I2C_TX_S;
        status = I2C_SUCCESS;
      }
      break;
#endif /* FEATURE_I2C_CHIP_ENABLE */
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:

      if (bus->state == I2C_TX_S) {           /* Bus already STARTed       */
        I2C_REP_START_COND(bus_id);
      } else if (bus->state == I2C_IDLE_S) {  /* bus not STARTed           */
        I2C_START_COND(bus_id);
      }
      if (i2c_write_byte_io(bus_id, val, FALSE) == I2C_ACK) { /* Write addr byte  */
        /* State change depend on READ/WRITE */
        state = ((val & I2C_IO_MASK) == I2C_IO_MASK_R)? I2C_RX_S: I2C_TX_S;
        status = I2C_SUCCESS;
      }
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      status = i2c_write_byte_hw( val | I2C_ADDR_BYTE);  /* Write addr byte*/
      state = i2c_get_ctrller_state(FALSE);              /* Get state      */
      break;

    default:
      ERR("Unknown controller type",0,0,0);
  }
  bus->state = state;
  return (status);
} /* i2c_write_addr */

/*===========================================================================

FUNCTION i2c_write_addr_read_one_only

DESCRIPTION
  This function write the slave address to the i2c bus.
  i2c_write_addr() may generate additional I2C clock and appears to read two
  bytes from the device, this could upset some device, hence this function.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write successful.
  I2C_FAILURE - write failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_write_addr_read_one_only
(
 byte slave_addr
)
{
  i2c_status_type status = I2C_SUCCESS;
  /* There are at most 2*9 = 18 bits in the output and shift register.
   * Add anothe 10 just in case clk_busy_wait is not precise. */
  uint32 bits_to_wait = 28;
  /* Interrupt is disabled for the following operations. Do not single
   * step through this block, or additonal I2C clock may be generated. */
  uint32 sav;
  INTLOCK_SAV(sav);

  /* write slave device address | READ */
  HWIO_OUT(I2C_WRITE_DATA, slave_addr | I2C_ADDR_BYTE);

  /* Wait for RX state then indicates last byte to read */
  while (i2c_get_ctrller_state(FALSE) != I2C_RX_S)
  {
    if (0 == bits_to_wait)
    {
      status = I2C_FAILURE;
      break;
    }
    clk_busy_wait(I2C_HW_BIT_DELAY(fs_freq));
    bits_to_wait--;
  }
  /* Indicates the last byte to read */
  if (status == I2C_SUCCESS)
  {
    status = i2c_write_byte_hw( I2C_LAST_BYTE);
  }
  INTFREE_SAV(sav);
  return (status);
} /* i2c_write_addr_read_one_only */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_write_bytes

DESCRIPTION
  This function write bytes of data to the slave.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write successful.
  I2C_FAILURE - write failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_write_bytes
(
  i2c_bus_type bus_id,
  byte        *buf,
  word         len,
  word        *r_index
)
{
  i2c_status_type status = I2C_SUCCESS;
  int             index  = 0;

  switch(i2c_io_info[bus_id].ctrl) {
#ifdef FEATURE_I2C_CHIP_ENABLE
    case I2C_CE_CTRL:
      for (index = 0; (index < len) && (status == I2C_SUCCESS);
           index++, (*r_index)++) {
        status = (i2c_status_type) i2c_write_byte_io(bus_id, buf[index], TRUE);
      }
      break;
#endif /* FEATURE_I2C_CHIP_ENABLE */
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      for (index = 0; (index < len) && (status == I2C_SUCCESS);
           index++, (*r_index)++) {
        status = (i2c_status_type) i2c_write_byte_io(bus_id, buf[index], FALSE);
      }
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      for (index = 0; (index < len) && (status == I2C_SUCCESS);
           index++, (*r_index)++) {
        status = i2c_write_byte_hw(buf[index]);
      }
      break;

    default:
      ERR("Unknown controller type",0,0,0);
      status = I2C_FAILURE;
  }
  return (status);
} /* i2c_write_bytes */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_write_last

DESCRIPTION
  This function write the write a byte to the slave and generate a stop
  signal.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - write successful.
  I2C_FAILURE - write failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_write_last
(
  i2c_bus_type bus_id,
  byte         val
)
{
  i2c_state_type  state  = I2C_FAIL_S;
  i2c_status_type status = I2C_FAILURE;
  i2c_io_type     *bus   = &i2c_io_info[bus_id];

  switch(bus->ctrl) {
#ifdef FEATURE_I2C_CHIP_ENABLE
    case I2C_CE_CTRL:
      if (i2c_write_byte_io(bus_id, val, TRUE) == I2C_ACK) {  /* Write byte      */
        state  = I2C_IDLE_S;                            /* Change state    */
        status = I2C_SUCCESS;
      }
      break;
#endif /* FEATURE_I2C_CHIP_ENABLE */

#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      if (i2c_write_byte_io(bus_id, val, FALSE) == I2C_ACK) {  /* Write byte      */
        I2C_STOP_COND(bus_id);                          /* Stop if ACK     */
        state  = I2C_IDLE_S;                            /* Change state    */
        status = I2C_SUCCESS;
      }
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      status = i2c_write_byte_hw( val | I2C_LAST_BYTE); /* Write last byte */
      state  = i2c_get_ctrller_state(FALSE);            /* Read state      */
      break;

    default:
      ERR("Unknown controller type ",0,0,0);
  }

  bus->state = state;
  return (status);
} /* i2c_write_last */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_read_bytes

DESCRIPTION
  This function read a byte from the slave.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - read successful.
  I2C_FAILURE - read failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_read_bytes
(
  i2c_bus_type bus_id,
  byte        *buf,
  word         len,
  word        *r_index
)
{
  i2c_status_type status = I2C_SUCCESS;
  word            index  = 0;

  switch(i2c_io_info[bus_id].ctrl) {
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      for (index =0; index < len; index++, (*r_index)++) {
        I2C_SDA_TRISTATE_R(bus_id);            /* Tristate SDA for input.  */
        buf[index] = i2c_read_byte_io(bus_id); /* Read byte from SDA line  */
        I2C_ACK_BYTE(bus_id);                  /* Ack the read byte.       */
      }
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
      for (index =0; (index < len) && (status == I2C_SUCCESS);
           index ++, (*r_index)++) {
        if( index == len - 1 ) /* If second last byte */
        {
          i2c_wait_rd_buf_full(); /* This is important. We need to wait until controller has received 
                                     the second last byte in Full */
          HWIO_OUT(I2C_WRITE_DATA, I2C_LAST_BYTE);    /* inform ctrl to NACK  */
        }
        status = i2c_read_byte_hw(&buf[index]); /* Read a byte from buffer */
      }
      break;

    default:
      ERR("Unknown controller type",0,0,0);
      status = I2C_FAILURE;
  }
  return (status);
} /* i2c_read_bytes */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_read_last

DESCRIPTION
  This function read the last byte from the slave and generate a stop
  signal.

DEPENDENCIES
  None.

RETURN VALUE
  I2C_SUCCESS - read successful.
  I2C_FAILURE - read failed.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL i2c_status_type i2c_read_last
(
  i2c_bus_type bus_id,
  byte         *val,
  /* Signal LAST_BYTE to I2C controller */
  boolean       last_byte
)
{
  i2c_status_type status  = I2C_FAILURE;
  i2c_state_type  state   = I2C_FAIL_S;
  i2c_io_type     *bus    = &i2c_io_info[bus_id];

  switch(bus->ctrl) {
#ifdef FEATURE_I2C_IO_CTRL
    case I2C_IO_CTRL:
      I2C_SDA_TRISTATE_R(bus_id);          /* Tristate SDA for input.      */
      *val   = i2c_read_byte_io(bus_id);   /* Read byte from SDA line      */
      I2C_NACK_BYTE(bus_id);               /* Nack the last byte           */
      I2C_STOP_COND(bus_id);               /* Generate a stop signal       */
      state  = I2C_IDLE_S;                 /* State change                 */
      status = I2C_SUCCESS;
      break;
#endif /* FEATURE_I2C_IO_CTRL */

    case I2C_HW_CTRL:
#if defined(SIRIUS_BUILD) || defined(FEATURE_I2C_EXTRA_READ_CLK)
      (void) i2c_wait_rd_buf_full();              /* Wait for read buf full*/
      if (last_byte)
      {
        HWIO_OUT(I2C_WRITE_DATA, I2C_LAST_BYTE); /* inform ctrl to NACK   */
      }
#else /* SIRIUS_BUILD || defined(FEATURE_I2C_EXTRA_READ_CLK */
      if (last_byte)
      {
        HWIO_OUT(I2C_WRITE_DATA, I2C_LAST_BYTE); /* inform ctrl to NACK   */
      }
      (void) i2c_wait_rd_buf_full();              /* Wait for read buf full*/
#endif /* SIRIUS_BUILD */
      status = i2c_read_byte_hw(val);             /* Read last byte        */
      state  = i2c_get_ctrller_state(TRUE);       /* Read in final state   */
      (void) HWIO_IN(I2C_READ_DATA);            /* Clean up the read buf */
      break;

    default:
      ERR("Unknown controller type",0,0,0);
  }
  bus->state = state;                             /* Updata bus state      */
  return (status);
} /* i2c_read_last */

/* <EJECT> */
/*===========================================================================

FUNCTION I2C_SETUP_SLAVE

DESCRIPTION
  Validate I/O command parameters. Generate a START condition, select the
  slave device and initialize the slave address to be indexed for data
  transfer.

  A fatal error is generated if an invalid bus id is specified, or if the
  bus is an inconsistent state (i.e Uninitialized, or busy).

  The following diagram shows the bus activity set up just before
  transmitting data to/from the slave. This routine returns the the last
  ACK value shown in the diagram.

              |<--opt.-->|
  ------------------------
  |S|       | |        | |
  |T|Control|A|Address |A|
  |A| Byte  |C|(Offset |C|
  |R|       |K| or Reg)|K|
  |T|       | |        | |
  ------------------------

DEPENDENCIES
  i2c_init must be invoked prior to this call.

RETURN VALUE
  I2C_ACK             - set up successful
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_ACCESS  - invalid I2C bus access when busy
  I2C_INVALID_BUS     - invalid I2C bus
  I2C_NACK            - slave NACKed during setup

SIDE EFFECTS
  SCL is left low

===========================================================================*/
LOCAL i2c_status_type i2c_setup_slave
(
  i2c_rw_cmd_type* cmd_ptr,  /* pointer to user command buffer */
  i2c_op_type      op_type   /* operation type */
)
{
  i2c_bus_type         bus_id = cmd_ptr->bus_id; /* bus id for I/O */
  byte                 slave_addr;        /* temporary to hold slave addr */
  i2c_status_type      status = I2C_INVALID_ACCESS;
  word                 len    = 0;

  if(bus_id >= I2C_NUM_BUS) {
    MSG_ERROR("Invalid bus id %u ",bus_id,0,0);
    return (I2C_INVALID_BUS);
  }

  if (cmd_ptr->len >= I2C_MAX_DATA_LEN) {
    MSG_ERROR("Invalid data size for I/O - bus %u, dev 0x%x", bus_id,
              cmd_ptr->slave_addr, 0);
    return (I2C_INVALID_LEN);
  }

  if (i2c_io_info[bus_id].state == I2C_IDLE_S) {

    if ((cmd_ptr->options & I2C_DFLT_ADDR_DEV) == I2C_DFLT_ADDR_DEV) {
#ifdef FEATURE_I2C_CHIP_ENABLE
      if (bus_id == I2C_BUS_CE_CTRL) {
        slave_addr = cmd_ptr->slave_addr;
      }
      else
#endif /* FEATURE_I2C_CHIP_ENABLE */
      if (op_type == I2C_READ) {
        slave_addr = I2C_ADDR_MASK_R(cmd_ptr->slave_addr);
      }
      else {
        slave_addr = I2C_ADDR_MASK_W(cmd_ptr->slave_addr);
      }
    } else {
      slave_addr = I2C_ADDR_MASK_W(cmd_ptr->slave_addr);
    }

    /* Special used for VBAP */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
    if (cmd_ptr->options & I2C_CLK_START_BEFORE_RW) {
      i2c_clk_start_before_rw(bus_id);
    }
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */

    if ((cmd_ptr->options & I2C_DFLT_ADDR_DEV) == I2C_DFLT_ADDR_DEV) 
    {
      if ((bus_id == I2C_BUS_HW_CTRL) && (cmd_ptr->len == 1) && (op_type == I2C_READ))
      {
        status = i2c_write_addr_read_one_only(slave_addr);
      }
      else
      {
        status = i2c_write_addr(bus_id, slave_addr);
      }        
    }
    else
    {    
      status = i2c_write_addr(bus_id, slave_addr);
      if (status == I2C_SUCCESS) {
        if (i2c_io_info[bus_id].state == I2C_TX_S) {
          if (cmd_ptr->options & I2C_MEM_DEV) {

            /* Transmit high word of offset */
            status = i2c_write_bytes(bus_id, ((byte *)&(cmd_ptr->addr.offset))+1,
                                     1, &len);

            /* Validate if slave ACKed. Transmit low word of offset */
            if (status == I2C_SUCCESS) {
              if ((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&
                  (op_type == I2C_READ)) {
                status = i2c_write_last(bus_id,
                                        WORD_LO(cmd_ptr->addr.offset));
              } else {
                status = i2c_write_bytes(bus_id,
                                         ((byte *)&(cmd_ptr->addr.offset)),
                                         1, &len);
              }
            }
          }
          else if (cmd_ptr->options & I2C_REG_DEV) {
            /* Transmit register address of slave device */
            if ((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&
                (op_type == I2C_READ)) {
              status = i2c_write_last(bus_id, cmd_ptr->addr.reg);
            } else {
              status = i2c_write_bytes(bus_id, &(cmd_ptr->addr.reg), 1, &len);
            }
          }

          /* Write slave control byte with 'r' bit set. */
          if ((op_type == I2C_READ) && (status == I2C_SUCCESS)) {

            /* Special used for VBAP */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
            if ((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&
                (cmd_ptr->options & I2C_CLK_START_BEFORE_RW)) {
              i2c_clk_start_before_rw(bus_id);
            }
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */

            if ((bus_id == I2C_BUS_HW_CTRL) && (cmd_ptr->len == 1))
            {
              status = i2c_write_addr_read_one_only(I2C_ADDR_MASK_R(cmd_ptr->slave_addr));
            }
            else
            {
              status = i2c_write_addr(bus_id, I2C_ADDR_MASK_R(cmd_ptr->slave_addr));
            }
          }
        }
      }
    }
  }
  return (status);
} /* i2c_setup_slave */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
#ifdef FEATURE_I2C_STUCK_RECOVERY
/*===========================================================================

FUNCTION: i2c_low_stuck_recovery_read_dummy

DESCRIPTION
  In case of i2c SDA stucks in low, do the dummy-byte read to bring it back
  to HIGH in order to notify the slave to reliquish the bus control.

DEPENDENCIES
  SDA/SCL stuck at low. 

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_FAILURE         - write failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is brought back to high
  SDA is brought back to high
===========================================================================*/
i2c_status_type i2c_low_stuck_recovery_read_dummy (void)
{
#if defined FEATURE_I2C_IO_CTRL_OVER_HW || \
    defined FEATURE_I2C_IO_CTRL         || \
    defined FEATURE_I2C_CHIP_ENABLE

  int count;

  /* In case of I2C bus stuck, let's switch uncofigure the GPIOs first.
   * 
   * Switch to IO control
   */
  I2C_SWITCH_IO_CTRL(bus_id);

  /* For upto 9 times, read data */
  for (count = 0; count < 10; count++)
  {
    uint32 sav;
    uint16 data;
    INTLOCK_SAV(sav);

    /*
    ** To read a data bit from the slave, read data bit in the high
    ** period of the clock. The required timings are shown below.
    **
    **      |<---Tlow--->|<--Thigh-->|
    **      |   4.7us    |   4us     |
    **  SCL--            -------------
    **      |            |           |
    **      |            |           |
    **      --------------           -----
    **
    */
    /* Lower the clock */
    BIO_OUT(BIO_EEPROM_REG, BIO_EEPROM_CLK_M, 0);

    /* Wait initial time.  Could be shortened due to time spent in
       INTLOCK and INTFREE, but then we would depend on their
       implementation.  Should be visited if this is a performance
       issue. */
    I2C_TLOW_DELAY;

    /* Now raise the clock */
    BIO_OUT(BIO_EEPROM_REG, BIO_EEPROM_CLK_M, BIO_EEPROM_CLK_M);

    /* Wait the high delay time.  The best time to sample the data
       line is right before we lower the clock again. */
    I2C_THIGH_DELAY;

    /* Read in the data bit. */
    data = BIO_INM( BIO_EEPROM_REG, BIO_EEPROM_DATA_M);

    INTFREE_SAV(sav);
  }

  return (I2C_SUCCESS);
#else 
  return (I2C_SUCCESS);
#endif
}

/*===========================================================================

FUNCTION: i2c_low_stuck_recovery

DESCRIPTION
  In case of bus stuck in low, this function will re-acquire the I2C control.

DEPENDENCIES
  SDA/SCL stuck in low.   

RETURN VALUE
  None.

SIDE EFFECTS
  SCL is brought back to high
  SDA is brought back to high
===========================================================================*/
void i2c_low_stuck_recovery (i2c_bus_type bus_id)
{
  /* Issue some dummy operation in order to pull back up SDA/SCL. */  
  I2C_STOP_COND(bus_id);
  i2c_low_stuck_recovery_read_dummy();

  /* Re-configure the GPIOs to setup the I2C interface */
  i2c_setup_path(bus_id);
  I2C_CLK_RESET();
}
#endif /* FEATURE_I2C_STUCK_RECOVERY */

/*===========================================================================

FUNCTION i2c_init

DESCRIPTION
  This function intializes the specified I2C bus to a known state - SCL high,
  SDA high. It also configures SCL and SDA as output. This routine should be
  called once for every bus that needs to be used in the target.

  A fatal error is generated if an invalid bus id is specified, or if the
  target bus is busy.

DEPENDENCIES
  Only to be used after powerup, before any of the I2C buses are used
  for I/O.

RETURN VALUE
  None.

SIDE EFFECTS
  SCL is left high
  SDA is left high

===========================================================================*/
void i2c_init ( void )
{
  static boolean first_init = TRUE;
  i2c_bus_type bus_id;

  if (first_init)
  {
    rex_init_crit_sect(&i2c_crit_sect);
    first_init = FALSE;
  }

  /* Get I2C clock frequency from clk regime */
  clk_i2c_freq_khz = clk_regime_get_i2c_clk_khz();

#if defined T_MSM6500 || !defined FEATURE_I2C_SKIP_CLK_RESET
    /* Reset I2C Clock Regime for clean start up */
    I2C_CLK_RESET();
#endif

  for (bus_id = (i2c_bus_type)0; bus_id < I2C_BUS_INVALID; ++bus_id) {

    if ((i2c_bus_type) bus_id == I2C_BUS_HW_CTRL) {
      /* Initialize the specified i2c bus */
      i2c_setup_path( (i2c_bus_type) bus_id );

#ifdef FEATURE_I2C_STUCK_RECOVERY
      if (I2C_READ_SDA(bus_id) == I2C_BIT_LO) {
        i2c_low_stuck_recovery(bus_id);
      }
#endif /* FEATURE_I2C_STUCK_RECOVERY */
    }

#ifdef FEATURE_I2C_CHIP_ENABLE
    if ((i2c_bus_type) bus_id == I2C_BUS_CE_CTRL) {
      i2c_ce_init();
    }
#endif /* FEATURE_I2C_CHIP_ENABLE */

    /* Initialize the bus to IDLE state */
    i2c_io_info[bus_id].state = I2C_IDLE_S;
  }

#ifdef FEATURE_I2C_STUCK_RECOVERY
  i2c_low_stuck_recovery_read_dummy();
#endif /* FEATURE_I2C_STUCK_RECOVERY */
} /* i2c_init */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_read

DESCRIPTION
  This function reads data from a slave device. The user should ensure that
  the correct options have been chosen to control the read operation. This
  routine validates input parameters.

  The read is aborted if the slave does not ack control/address bytes
  written to it (before the data read starts). A read operation is always
  terminated with a STOP condition.

  A fatal error is generated if an invalid bus id is specified, or if the
  bus is an inconsistent state (i.e Uninitialized, or busy).

  The following diagram shows the bus activity during a read. The second
  START shown is to be interpreted as a repeated START and may be replaced
  by a STOP and a START by some protocols.

              |<------optional------>|
  ------------------------------------------------------------
  |S|       | |        | |S|       | |                   |N|S|
  |T|Control|A|Address |A|T|Control|A|      Data         |O|T|
  |A| Byte  |C|(Offset |C|A| Byte  |C|                   |A|O|
  |R|       |K| or Reg)|K|R|       |K|                   |C|P|
  |T|       | |        | |T|       | |                   |K| |
  ------------------------------------------------------------
  The address may be 0/1/2 bytes depending on the slave. Every address byte
  written to the slave needs to be ACKed.

DEPENDENCIES
  i2c_init should be invoked prior to this call.
  The target I2C bus should not be busy.

RETURN VALUE
  I2C_SUCCESS         - read successful
  I2C_FAILURE         - read failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is left high
  SDA is left high

===========================================================================*/
i2c_status_type i2c_read
(
  i2c_rw_cmd_type   *cmd_ptr  /* pointer to user command buffer            */
)
{
  uint32          my_clk_i2c_freq_khz = 0;
  i2c_bus_type    bus_id;                          /* I2C bus id           */
  static i2c_bus_type my_bus_id = I2C_BUS_INVALID;
  word            index  = 0;                      /* loop variable        */
  i2c_status_type status = I2C_INVALID_ACCESS;     /* I2C operation status */
  byte           *buffer = NULL;                   /* Read buffer's pointer*/

  if ((cmd_ptr != NULL) && (cmd_ptr->buf_ptr != NULL)) {

    rex_enter_crit_sect(&i2c_crit_sect);
    i2c_active = TRUE;
    bus_id = cmd_ptr->bus_id;
    buffer = (byte *)cmd_ptr->buf_ptr;

    /* If I2C is stuck in other states, re-init */
    if (i2c_io_info[bus_id].state != I2C_IDLE_S)
    {
      i2c_init();
    }

    /* Re-setup path when clock or bus_id changed. */
    my_clk_i2c_freq_khz = clk_regime_get_i2c_clk_khz();
    if (clk_i2c_freq_khz != my_clk_i2c_freq_khz ||
        my_bus_id != bus_id)
    {
      clk_i2c_freq_khz = my_clk_i2c_freq_khz;
      my_bus_id = bus_id;
      /* Use setup path to configure the I2C pads */
      i2c_setup_path( bus_id );
    }

    if (i2c_io_info[bus_id].state == I2C_IDLE_S) {

#ifndef FEATURE_I2C_SKIP_CLK_HALT
      /* start the i2c hardware controller's clock */
      i2c_clock_halt( bus_id, FALSE );
#endif

      /* Set up slave for read operation */
      status = i2c_setup_slave(cmd_ptr, I2C_READ);

      /* Read data until (N-1)th byte */
      if (status == I2C_SUCCESS) {
        status = i2c_read_bytes(bus_id, &buffer[index], cmd_ptr->len - 1,
                                &index);
      }

      /* Read the last data byte */
      if (status == I2C_SUCCESS) {
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
        if (cmd_ptr->options & I2C_NOP_LAST_BYTE) {
          status = i2c_read_bytes(bus_id, &buffer[index], 1, &index);
          i2c_stop(bus_id, TRUE);
        }
        else
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */
        {
          status = i2c_read_last(bus_id, &buffer[index++], FALSE);
        }
      }

      /* Set the index of the last byte written to the slave */
      cmd_ptr->last_index = index;

      if (status != I2C_SUCCESS) {
        i2c_stop(bus_id, FALSE);
      }

#ifndef FEATURE_I2C_SKIP_CLK_HALT
      /* halt the i2c hardware controller's clock */
      i2c_clock_halt( bus_id, TRUE );
#endif
    }

    i2c_active = FALSE;
    rex_leave_crit_sect(&i2c_crit_sect);
  }
  return (status);
} /* i2c_read */

/* <EJECT> */
/*===========================================================================

FUNCTION i2c_write

DESCRIPTION
  This function writes data to a slave device. The user should ensure that
  the correct options have been chosen to control the write operation.

  The write is aborted if the slave does not ack control/address/data bytes
  written to it. The write operation is always terminated with a STOP
  condition.

  If a write is attempted with a count of zero, the slave is selected and a
  success is returned if the slave acks. Otherwise, a failure is returned.
  This is useful if one needs to test if a slave is addressable as in NVRAM
  devices that may be busy when doing internal cache-writes.

  The following diagram shows the bus activity during a write operation.

              |<--opt.-->|
  ------------------------------------------------
  |S|       | |        | |                   | |S|
  |T|Control|A|Address |A|      Data         |A|T|
  |A| Byte  |C|(Offset |C|                   |C|O|
  |R|       |K| or Reg)|K|                   |K|P|
  |T|       | |        | |                   | | |
  ------------------------------------------------
  The address may be 0/1/2 bytes depending on the slave. Every address byte
  written to the slave needs to be ACKed.

DEPENDENCIES
  i2c_init should be invoked prior to this call.
  The target I2C bus should not be busy.

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_FAILURE         - write failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is left high
  SDA is left high

===========================================================================*/
i2c_status_type i2c_write
(
  i2c_rw_cmd_type   *cmd_ptr  /* pointer to user command buffer            */
)
{
  uint32          my_clk_i2c_freq_khz = 0;
  static i2c_bus_type my_bus_id = I2C_BUS_INVALID;
  i2c_bus_type    bus_id;                       /* I2C bus id              */
  word            index   = 0;                  /* loop variable           */
  i2c_status_type status  = I2C_INVALID_ACCESS; /* I2C operation status    */
  byte            *buffer = NULL;               /* Write buffer's pointer  */

  if ((cmd_ptr != NULL) && (cmd_ptr->buf_ptr != NULL)) {

    rex_enter_crit_sect(&i2c_crit_sect);
    i2c_active = TRUE;

    bus_id = cmd_ptr->bus_id;

    /* If I2C is stuck in other states, re-init */
    if (i2c_io_info[bus_id].state != I2C_IDLE_S)
    {
      i2c_init();
    }

    buffer = (byte *)cmd_ptr->buf_ptr;

#ifdef FEATURE_I2C_CHIP_ENABLE
    last_byte = FALSE;
    if (bus_id == I2C_BUS_CE_CTRL) {
      i2c_setup_path( bus_id );
      i2c_chip_enable();
	    clk_busy_wait(50);
      I2C_WRITE_SCL(bus_id, I2C_BIT_LO);   /* lower SCL */
      my_bus_id = bus_id;
    }
    else
#endif /* FEATURE_I2C_CHIP_ENABLE */
    {
      /* Re-setup path when clock or bus_id changed. */
      my_clk_i2c_freq_khz = clk_regime_get_i2c_clk_khz();
      if (clk_i2c_freq_khz != my_clk_i2c_freq_khz ||
          my_bus_id != bus_id)
      {
        clk_i2c_freq_khz = my_clk_i2c_freq_khz;
        my_bus_id = bus_id;
        /* Use setup path to configure the I2C pads */
        i2c_setup_path( bus_id );

        /* Need to path setup again for some unknown
           reason.  This is needed for the Camera sensor to
           command the steping motor for focus. */
        i2c_setup_path( bus_id );
      }
    }

    if (i2c_io_info[bus_id].state == I2C_IDLE_S) {

#ifndef FEATURE_I2C_SKIP_CLK_HALT
      /* start the i2c hardware controller's clock */
      i2c_clock_halt( bus_id, FALSE );
#endif

      /* Set up slave for write operation. */
      status = i2c_setup_slave(cmd_ptr, I2C_WRITE);

      /* Write data until (N-1)th byte */
      if (status == I2C_SUCCESS) {
        status = i2c_write_bytes(bus_id, &buffer[index], cmd_ptr->len - 1,
                                 &index);
      }

      /* Write the last data byte */
      if (status == I2C_SUCCESS) {
#ifdef FEATURE_I2C_CHIP_ENABLE
        last_byte = TRUE;
#endif /* FEATURE_I2C_CHIP_ENABLE */
        status = i2c_write_last(bus_id, buffer[index++]);
        /* Occasionally I2C transmit got delayed and we may still in the
         * middle of transmit. Wait */
        if (i2c_io_info[bus_id].state != I2C_IDLE_S) {
          clk_busy_wait( I2C_HW_CTRL_DELAY( fs_freq ) );
          i2c_io_info[bus_id].state = I2C_IDLE_S;
        }
      }

#ifdef FEATURE_I2C_CHIP_ENABLE
      if (bus_id == I2C_BUS_CE_CTRL) {
        i2c_chip_disable();
        /* Delay then bring SCL and SDA high */
        I2C_TLOW_DELAY;
        I2C_SCL_SDA_HI(bus_id);
      }
#endif /* FEATURE_I2C_CHIP_ENABLE */

      /* Set the index of the last byte written to the slave */
      cmd_ptr->last_index = index;

      /* Error if write failed. Not fatal! */
      if (status != I2C_SUCCESS) {
        i2c_stop(bus_id, FALSE);
      }

#ifndef FEATURE_I2C_SKIP_CLK_HALT
      /* halt the i2c hardware controller's clock */
      i2c_clock_halt( bus_id, TRUE );
#endif
    }

    i2c_active = FALSE;
    rex_leave_crit_sect(&i2c_crit_sect);
  }
  return (status);
} /* i2c_write */

#ifdef FEATURE_I2C_CHIP_ENABLE
/*===========================================================================

FUNCTION I2C_CHIP_DISABLE

DESCRIPTION
  CHIP_ENABLE set to High. 

PARAMETERS

DEPENDENCIES
  Must be in software controller mode.

RETURN VALUE
  None.

SIDE EFFECTS
  XCE is left high.

===========================================================================*/
void i2c_chip_disable(void)
{
#if defined CPLD_MSM_H && defined T_SURF && !defined T_FFA
  FPGA_OUTM(FPGA_GPIO_OUT, I2C_CE, I2C_CE_HI);
  clk_busy_wait(5);
#elif defined BIO_I2C_CE_REG
  if (i2c_ce_invert_pin)
  {
    BIO_OUT(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_I2C_CE_LO_V);
  }
  else
  {
    BIO_OUT(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_I2C_CE_HI_V);
  }
#elif defined I2C_CE
  if (i2c_ce_invert_pin)
  {
    gpio_out(I2C_CE, GPIO_LOW_VALUE);
  }
  else
  {
    gpio_out(I2C_CE, GPIO_HIGH_VALUE);
  }
#endif
} /* i2c_chip_disable */
#endif /* FEATURE_I2C_CHIP_ENABLE */

#ifdef FEATURE_I2C_CHIP_ENABLE
/*===========================================================================

FUNCTION I2C_CHIP_ENABLE

DESCRIPTION
  CHIP_ENABLE set to Low. 

PARAMETERS

DEPENDENCIES
  Must be in software controller mode.

RETURN VALUE
  None.

SIDE EFFECTS
  XCE is left Low.

===========================================================================*/
void i2c_chip_enable(void)
{
#if defined CPLD_MSM_H && defined T_SURF && !defined T_FFA
  FPGA_OUTM(FPGA_GPIO_OUT, I2C_CE, I2C_CE_LO);
  clk_busy_wait(5);
#elif defined  BIO_I2C_CE_REG
  if (i2c_ce_invert_pin)
  {
    BIO_OUT(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_I2C_CE_HI_V);
  }
  else
  {
    BIO_OUT(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_I2C_CE_LO_V);
  }
#elif defined I2C_CE
  if (i2c_ce_invert_pin)
  {
    gpio_out(I2C_CE, GPIO_HIGH_VALUE);
  }
  else
  {
    gpio_out(I2C_CE, GPIO_LOW_VALUE);
  }
  
#endif
} /* i2c_chip_enable */
#endif /* FEATURE_I2C_CHIP_ENABLE */


#ifdef FEATURE_I2C_CHIP_ENABLE
/*===========================================================================

FUNCTION I2C_CE_INIT

DESCRIPTION
  Init CHIP_ENABLE

PARAMETERS

DEPENDENCIES
  Must be in software controller mode.

RETURN VALUE
  None.

SIDE EFFECTS
  XCE is left high.

===========================================================================*/
void i2c_ce_init(void)
{
#if defined CPLD_MSM_H && defined T_SURF && !defined T_FFA
  FPGA_OUTM(FPGA_GPIO_OE, I2C_CE, I2C_CE_HI);
  i2c_chip_disable();
#elif defined I2C_CE 
   gpio_tlmm_config(I2C_CE);
   i2c_chip_enable();
#elif defined BIO_I2C_CE_REG
   BIO_FUNC_SELECT(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_ALT_FUNC_DIS_V);
   BIO_TRISTATE(BIO_I2C_CE_REG, BIO_I2C_CE_M, BIO_OUTPUT_ENA_V);
   i2c_chip_enable();
#endif
} /* i2c_ce_init */

/*===========================================================================

FUNCTION I2C_CE_INVERT

DESCRIPTION
  Invert chip enable. 

PARAMETERS
   None.

DEPENDENCIES
  Must be in software controller mode.

RETURN VALUE
  None.

SIDE EFFECTS
  Call to chip enable and disable will be inverted.

===========================================================================*/
void i2c_ce_invert(boolean value)
{
  i2c_ce_invert_pin = value;
} /* i2c_ce_invert */

#endif /* FEATURE_I2C_CHIP_ENABLE */

/*===========================================================================

FUNCTION: i2c_combined_write_no_stop_condition 

DESCRIPTION
  Function is invoked by i2c_combined_write_read to fullfill the operation of
  write-without-stop condition.

DEPENDENCIES
  The target I2C bus should not be busy.

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_FAILURE         - write failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is left high
  SDA is left high
===========================================================================*/
i2c_status_type i2c_combined_write_no_stop_condition(i2c_rw_cmd_type *cmd_ptr)
{
  uint32          my_clk_i2c_freq_khz = 0;
  static i2c_bus_type my_bus_id = I2C_BUS_INVALID;
  i2c_bus_type    bus_id;                       /* I2C bus id              */
  word            index   = 0;                  /* loop variable           */
  i2c_status_type status  = I2C_INVALID_ACCESS; /* I2C operation status    */
  byte            *write_buffer = NULL;         /* Write buffer's pointer  */

  rex_enter_crit_sect(&i2c_crit_sect);
  i2c_active = TRUE;

  bus_id = cmd_ptr->bus_id;

  /* If I2C is stuck in other states, re-init */
  if (i2c_io_info[bus_id].state != I2C_IDLE_S)
  {
    i2c_init();
  }

  write_buffer = (byte *)cmd_ptr->buf_ptr;

#ifdef FEATURE_I2C_CHIP_ENABLE
  last_byte = FALSE;
  if (bus_id == I2C_BUS_CE_CTRL) {
    i2c_setup_path( bus_id );
    i2c_chip_enable();
    clk_busy_wait(50);
    I2C_WRITE_SCL(bus_id, I2C_BIT_LO);   /* lower SCL */
    my_bus_id = bus_id;
  }
  else
#endif /* FEATURE_I2C_CHIP_ENABLE */
  {
    /* Re-setup path when clock or bus_id changed. */
    my_clk_i2c_freq_khz = clk_regime_get_i2c_clk_khz();
    if (clk_i2c_freq_khz != my_clk_i2c_freq_khz ||
        my_bus_id != bus_id)
    {
      clk_i2c_freq_khz = my_clk_i2c_freq_khz;
      my_bus_id = bus_id;
      /* Use setup path to configure the I2C pads */
      i2c_setup_path( bus_id );

      /* Need to path setup again for some unknown
         reason.  This is needed for the Camera sensor to
         command the steping motor for focus. */
      i2c_setup_path( bus_id );
    }
  }

  if (i2c_io_info[bus_id].state == I2C_IDLE_S) {

    INTLOCK();

#ifndef FEATURE_I2C_SKIP_CLK_HALT
    /* start the i2c hardware controller's clock */
    i2c_clock_halt( bus_id, FALSE );
#endif

    /* Set up slave address first for write operation. */
    status = i2c_setup_slave(cmd_ptr, I2C_WRITE);

    /* Write register address(es) byte which need to be read */
    if (status == I2C_SUCCESS) {
      status = i2c_write_bytes(bus_id, &write_buffer[index], cmd_ptr->len, &index);
    }

#ifdef FEATURE_I2C_CHIP_ENABLE
    if (bus_id == I2C_BUS_CE_CTRL) {
      i2c_chip_disable();
      /* Delay then bring SCL and SDA high */
      I2C_TLOW_DELAY;
      I2C_SCL_SDA_HI(bus_id);
    }
#endif /* FEATURE_I2C_CHIP_ENABLE */

    /* Set the index of the last byte written to the slave */
    cmd_ptr->last_index = index;

    /* Error if write failed. Not fatal! */
    if (status != I2C_SUCCESS) {
      i2c_stop(bus_id, FALSE);
    }

#ifndef FEATURE_I2C_SKIP_CLK_HALT
    /* halt the i2c hardware controller's clock */
    i2c_clock_halt( bus_id, TRUE );
#endif

    INTFREE();
  }

  i2c_active = FALSE;
  rex_leave_crit_sect(&i2c_crit_sect);

  return (status);
} /* i2c_combined_write_no_stop_condition */

/*===========================================================================

FUNCTION i2c_combined_write_read

DESCRIPTION
  This function implemented I2C combined format operation. During a change
  of direction within a transfer, the START condition and the slave address
  are both repeated, but with the R/W bit reversed. If a master receiver sends
  a repeated START condition, it has previously sent a not-ACK.

  All decisions on auto-increment or decrement of previously accessed memory
  locations etc. are taken by the designer of the device.

  Each byte is followed by an acknowledgment bit as indicated by the ACK or 
  not-ACK blocks in the sequence.

  I2C-bus compatible devices must reset their bus logic on receipt of a START
  or repeated START condition such that they all anticipate the sending of a 
  slave address, even if these START conditions are not positioned according
  to the proper format.
                                                                     
  The following diagram shows the bus activity during a combined write-read
  operation.

              |<--opt.-->|
  --------------------------------------------------------------
  | |            | |        | |  |            | |        |   | |
  | |Slave     _ | |Address | |  |Slave     _ | |Data    |  _| |
  |S|Address(R/W)|A|(Offset |A|Sr|Address(R/W)|A|(n bytes|A/A|P|
  | |            | | or Reg)| |  |            | |  + ack)|   | |
  | |            | |        | |  |            | |        |   | |
  --------------------------------------------------------------
  The address may be 0/1/2 bytes depending on the slave. Every address byte
  written to the slave needs to be ACKed.

DEPENDENCIES
  The target I2C bus should not be busy.

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_FAILURE         - write failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is left high
  SDA is left high
===========================================================================*/
i2c_status_type i2c_combined_write_read
(
  i2c_rw_cmd_type   *cmd_ptr  /* pointer to user command buffer            */
)
{
  i2c_status_type status  = I2C_INVALID_ACCESS; /* I2C operation status    */

  if ((cmd_ptr != NULL) && (cmd_ptr->buf_ptr != NULL)) {

    status = i2c_combined_write_no_stop_condition(cmd_ptr); 

    if (status == I2C_SUCCESS) {
      cmd_ptr->len = cmd_ptr->read_len;
      cmd_ptr->buf_ptr = cmd_ptr->read_buf_ptr;
    
      status = i2c_read(cmd_ptr);
    }

    if (status != I2C_SUCCESS) {
      i2c_stop(cmd_ptr->bus_id, FALSE);
    }
  }

  return (status);
} /* i2c_combined_write_read */

/*===========================================================================

FUNCTION: i2c_operation_fs_set

DESCRIPTION
  This function provided the API with the capability of dynamic changing I2C
  operation speed.

  Currently, only Fast-mode and Standard-mode are supported.

DEPENDENCIES
  The target I2C bus should not be busy.

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_FAILURE         - write failed
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_INVALID_BUS     - Invlid bus id
  I2C_INVALID_ACCESS  - Illegal access

SIDE EFFECTS
  SCL is left high
  SDA is left high
===========================================================================*/
i2c_status_type i2c_operation_fs_set(uint32 fs_khz)
{
  static uint32 current_fs = I2C_HW_CTRL_SCL_FREQ_STANDARD; /* default */
  i2c_bus_type bus_id;

  if (current_fs == fs_khz) {

    MSG_HIGH("I2c operation fs: Nothing needs to set",0,0,0);
    return (I2C_SUCCESS);
  }

  if ((0 < fs_khz) && (fs_khz <= 100)) { 

    fs_freq = I2C_HW_CTRL_SCL_FREQ_STANDARD; 
    i2c_fs_freq_khz = I2C_HW_CTRL_SCL_FREQ_STANDARD; 

  } else if ((100 < fs_khz) && (fs_khz <= 400)) { 

    fs_freq = I2C_HW_CTRL_SCL_FREQ_FAST; 

    /* currently the maximum is 384KHz for fast-mode */
    i2c_fs_freq_khz = (I2C_HW_CTRL_SCL_FREQ_FAST - 16); 

	} else {

    ERR_FATAL("Invalid I2C operation mode",0,0,0);
    return (I2C_FAILURE);
	}

  current_fs = fs_khz;

  for (bus_id = 0; bus_id < I2C_BUS_INVALID; ++bus_id) {
  
    i2c_stop(bus_id, TRUE);
  }

  i2c_init();

  clk_busy_wait(200);

  return (I2C_SUCCESS);
}
#endif /* FEATURE_I2C */
