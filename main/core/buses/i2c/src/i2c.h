#ifndef I2C_H
#define I2C_H
/*===========================================================================

              I 2 C   L I B R A R Y    H E A D E R

DESCRIPTION
  This file contains declarations necessary to control I2C buses and
  perform I/O on I2C slave devices.

Copyright (c) 2004 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

   $Header: //depot/asic/msmshared/drivers/i2c/i2c.h#6 $ $DateTime: 2006/11/16 17:26:03 $ $Author: hzhong $


when       who     what, where, why
--------   ---     -------------------------------------------------------
11/03/06   hz      Added support for 
                   a. Dynamic speed change: i2c_operation_fs_set(); 
                   b. Combined mode: i2c_combined_write_read(). 
05/22/06   dl      Added a function to invert i2c_ce pin.
03/24/06   dle     Added some stuff for new compiler.
                   buf_ptr is now byte *.
08/24/04   dle     Added back change history. Replace 3WIRE with
                   3WIRE_SW_CTRL with CE_CTRL and featuriz with
                   FEATURE_I2C_CHIP_ENABLE.
08/06/04   sch     Merged John Hacklander's changes to provide the same I2C
                   evironment as Jaguar.
05/24/03   dle     Featurized (FEATURE_I2C_IO_CTRL_OVER_H) options
                   (I2C_NOP_LAST_BYTE and I2C_CLK_START_BEFORE_RW) where there
                   is a need to switch to IO_CTRL in order to support these
                   options.
05/29/03   dle     Moved I2C_HW_CTRL_SCL_FREQ here.
03/11/02   aw      Reorganized I2C driver and added support for hardware
                   controller.
04/14/00   sm      Featurized I2C driver.
02/21/00   sm      Modified for SDAC target.  Removed non ARM support.
07/01/99   sk      Fixed VBAP bus id.
04/21/99   rv      Added support for Kenny.
09/18/98   sk      Added support for Q.
05/11/98   sk      Added bus definitions for REN.
12/15/97   sk      File created.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_I2C

/*===========================================================================

                        CONSTANT DEFINITIONS

===========================================================================*/
#ifdef FEATURE_CAMERA_SP0828
#define I2C_HW_CTRL_SCL_FREQ_STANDARD  25//100  /* Standard mode, supports upto 100Kbps */ 
#else
#define I2C_HW_CTRL_SCL_FREQ_STANDARD  100  /* Standard mode, supports upto 100Kbps */ 
#endif
#define I2C_HW_CTRL_SCL_FREQ_FAST     400  /* Fast mode, supports upto 400Kbps, our 6K MSMs only
                                              support 384Kbps
                                            */ 

#define I2C_HW_CTRL_SCL_FREQ_HS      3400  /* High-speed mode, supports 3.4Mbps, currently we
                                              don't support. But let's put it here for future 
                                              scalability */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Prevents clk from being changed while I2C is active */
extern boolean i2c_active;

/* I2C status types for return values from i2c_read() and i2c_write()      */
typedef enum {
  I2C_SUCCESS,                       /* operation successful               */
  I2C_FAILURE,                       /* operation failed                   */
  I2C_INVALID_LEN,                   /* invalid length specified for I/O   */
  I2C_INVALID_BUS,                   /* invalid bus id                     */
  I2C_INVALID_ACCESS                 /* illegal access                     */
} i2c_status_type;


/* Add new entries. Need to support software IO control and sensor chip
 * enable. These IDs are used for runtime switching between hardware
 * control, software IO control and software IO with chip enable control. */
typedef enum {
  I2C_BUS_HW_CTRL = 0,              /* I2C hardware control                 */
#ifdef FEATURE_I2C_IO_CTRL
  I2C_BUS_IO_CTRL,                  /* I2C software control (IO controller) */
#endif /* FEATURE_I2C_IO_CTRL */
#ifdef FEATURE_I2C_CHIP_ENABLE
  I2C_BUS_CE_CTRL,                  /* Bus ID for 3 wire S/W controller */
#endif /* FEATURE_I2C_CHIP_ENABLE */
  I2C_BUS_INVALID                   /* Invalid Serial Bus ID */
} i2c_bus_type;

typedef byte i2c_dev_addr_type;      /* I2C device address type            */

/* slave control bit definitions - for use in options word in the I/O      */
/* command structure. */

typedef enum {
  I2C_DFLT_ADDR_DEV          = 0x0001, /* Use default address for I/O      */
  I2C_MEM_DEV                = 0x0002, /* memory device (16 bit addresses) */
  I2C_REG_DEV                = 0x0004, /* 8 bit register based device      */
  I2C_NACK_LAST_BYTE         = 0x0008, /* NACK last byte read from slave   */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
  I2C_NOP_LAST_BYTE          = 0x0010, /* Let slave release SDA after last */
                                       /* byte is read from it.            */
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */
  I2C_START_BEFORE_READ      = 0x0020, /* During a read transaction, gen.  */
                                       /* a repeated START after writing   */
                                       /* the slave addr(just before read) */
  I2C_STOP_START_BEFORE_READ = 0x0040, /* During a read transaction, gen.  */
                                       /* a STOP & START after writing     */
                                       /* the slave addr(just before read) */
#ifdef FEATURE_I2C_IO_CTRL_OVER_HW
  I2C_CLK_START_BEFORE_RW    = 0x0080  /* Generate a clock and a START     */
                                       /* before every I/O operation.      */
#endif /* FEATURE_I2C_IO_CTRL_OVER_HW */
} i2c_options_type;

/* read/write command type for i2c slave I/O operation */
typedef  struct {
  i2c_bus_type      bus_id;          /* specified I2C bus id               */
  i2c_dev_addr_type slave_addr;      /* slave address                      */
  i2c_options_type  options;         /* bit mask of slave control actions  */
  byte             *buf_ptr;         /* pointer to buffer in caller space  */
  union {
    word            offset;          /* offset in I2C device to read/write */
    byte            reg;             /* address of register in slave       */
  } addr;
  word              len;             /* count of bytes to transfer         */
  word              read_len;        /* count of read bytes used ONLY in the
                                        combined Read/Write operation. */
  void             *read_buf_ptr;    /* pointer to read buffer in caller space 
                                        used ONLY in combined R/W operation. */
  word              last_index;      /* index of last byte written         */
} i2c_rw_cmd_type;
/*~ FIELD i2c_rw_cmd_type.buf_ptr VARRAY LENGTH i2c_rw_cmd_type.len */
/*~ IF ( _DISC_ & I2C_MEM_DEV ) i2c_rw_cmd_type.addr.offset */
/*~ DEFAULT i2c_rw_cmd_type.addr.reg */
/*~ FIELD i2c_rw_cmd_type.addr DISC i2c_rw_cmd_type.options */


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION i2c_init

DESCRIPTION
  This function intializes all I2C buses to a known state - SCL high, SDA
  high. It configures SCL and SDA as output.

  A fatal error is generated if an invalid bus id is specified, or if the
  specified bus is busy.

DEPENDENCIES
  Must be used before any of the I2C buses are used for I/O.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void i2c_init ( void );


/*===========================================================================

FUNCTION i2c_read

DESCRIPTION
  This function reads data from a slave device. The user should ensure that
  the correct options have been chosen to control the read operation.

  The read is aborted if the slave does not ack control/address bytes
  written to it (before the data read starts). A read operation is always
  terminated with a STOP condition.

  A fatal error is generated if an invalid bus id is specified, or if the
  bus is an inconsistent state (i.e Uninitialized, or busy).

  The following diagram shows the bus activity during a read. The second
  START shown is to be interpreted as a repeated START and may be replaced
  by a STOP and a START by some protocols.

  ------------------------------------------------------------
  |S|       | |        | |S|       | |                   |N|S|
  |T|Control|A|Address |A|T|Control|A|      Data         |O|T|
  |A| Byte  |C|(Offset |C|A| Byte  |C|                   |A|O|
  |R|       |K| or Reg)|K|R|       |K|                   |C|P|
  |T|       | |        | |T|       | |                   |K| |
  ------------------------------------------------------------

DEPENDENCIES
  i2c_init should be invoked prior to this call.

RETURN VALUE
  I2C_SUCCESS      - read successful
  I2C_INVALID_LEN  - length of read > 32K bytes
  I2C_FAILURE      - read failed

SIDE EFFECTS
  SCL is left high
  SDA is left high

===========================================================================*/
extern i2c_status_type i2c_read
(
  i2c_rw_cmd_type   *cmd_ptr    /* pointer to I2C slave I/O command */
);
/*~ FUNCTION i2c_read */
/*~ PARAM INOUT cmd_ptr POINTER */

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

  A fatal error is generated if an invalid bus id is specified, or if the
  bus is busy.

  The following diagram shows the bus activity during a write operation.

  ------------------------------------------------
  |S|       | |        | |                   | |S|
  |T|Control|A|Address |A|      Data         |A|T|
  |A| Byte  |C|(Offset |C|                   |C|O|
  |R|       |K| or Reg)|K|                   |K|P|
  |T|       | |        | |                   | | |
  ------------------------------------------------

DEPENDENCIES
  i2c_init should be invoked prior to this call.

RETURN VALUE
  I2C_SUCCESS         - write successful
  I2C_INVALID_LEN     - length of read > 32K bytes
  I2C_FAILURE         - write failed

SIDE EFFECTS
  SCL is left high
  SDA is left high

===========================================================================*/
extern i2c_status_type i2c_write
(
  i2c_rw_cmd_type   *cmd_ptr    /* pointer to I2C slave I/O command */
);
/*~ FUNCTION i2c_write */
/*~ PARAM cmd_ptr POINTER */


#ifdef FEATURE_I2C_CHIP_ENABLE
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
void i2c_ce_invert(boolean value);
#endif /* FEATURE_I2C_CHIP_ENABLE */

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
);

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
i2c_status_type i2c_operation_fs_set(uint32 fs_khz);
#endif /* FEATURE_I2C */
#endif /* I2C_H */
