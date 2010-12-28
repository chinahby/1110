/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
       
                S A D C   -   S T E R E O   A U D I O   A D C

GENERAL DESCRIPTION
  This device driver provides a simple interface between DMSS audio routines
  and the stereo audio ADC.

REFERENCES
  

EXTERNALIZED FUNCTIONS
  sadc_init
    Initialize the I2C driver, and look for the sadc.
  sadc_enable
    OFF            - Power down the ADC IC. Require several seconds to charge
                     the VMID capacitor.
    STAND_BY       - Everything is shutdown except VMID circut, wake-up will 
                     take only a few msec. 
    STEREO_LINE_IN - Mic preamps are disabled; Auto Level Control (ALC) 
                     and Noise Gate are disabled.
    MONO_LINE_IN   - Mic preamps are disabled; Auto Level Control (ALC).
    CAMCORDER      - Mic preamps are enabled; ALC and Noise Gate are enabled.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/driver/sadc/main/latest/src/sadc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/05    hz     Added MONO_LINE_IN configuration.
01/21/05    aw     First version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_EXTERNAL_SADC
#include "msg.h"                   /* Message logging/reporting services   */
#include "err.h"                   /* Error logging/reporting services     */
#include "i2c.h"                   /* I2C driver definitions               */
#include "sadc.h"                  /* SADC data and prototypes             */
#include "sadc_wm8737l.h"          /* Wilfson Stereo ADC (WM8737L) data    */
                                   /* and register definitions             */



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Stereo DAC I2C bus */
#define SADC_I2C_BUS  I2C_BUS_HW_CTRL
#define SADC_ID       (WM8737L_CSB_LOW_DEVICE_ADDR << 1) 
#define SADC_POWER_OFF_V              (0x0000)
#define SADC_STAND_BY_V               (0x0100)

typedef struct {
  uint8  addr;
  uint16 val;
} sadc_reg_type;

/* Array used as a shadow buffer for the output registers of the SADC
** This allows individual bits to be written differently without taking
** the time to read back the current contents of the register.
** As we cannot read the SADC memory, we initialize the default values.
*/

/* Variable used to setup the i2c port to access the SDAC
*/
LOCAL i2c_rw_cmd_type sadc_rw_buf =
{
  SADC_I2C_BUS,   /* Bus ID */
  SADC_ID,              /* Slave address */
  I2C_DFLT_ADDR_DEV,    /* Options */
  NULL,           /* Buffer */
  0,              /* Register address */
  2,              /* Count */
  0               /* last index */
};

sadc_reg_type sadc_power_off_val[] = { 
  WM8737L_REG6_ADDR, SADC_POWER_OFF_V
};

sadc_reg_type sadc_standby_val[] = {
  WM8737L_REG6_ADDR, SADC_STAND_BY_V
};

sadc_reg_type sadc_stereo_line_in_val[] = {
  WM8737L_REG6_ADDR, 0xbc,     /* first power up the analog circuits */
  WM8737L_REG0_ADDR, 0xc3,     /* configure left  PGA = 0db */
  WM8737L_REG1_ADDR, 0xc3,     /* configure right PGA = 0db */
  WM8737L_REG2_ADDR, 0x0f,     /* Configure left  audio path LIN1 selected */
  WM8737L_REG3_ADDR, 0x0f,     /* Configure right audio path RIN1 selected */
  WM8737L_REG4_ADDR, 0x00,     /* Diabled 3D Enhance */
  WM8737L_REG5_ADDR, 0x00,     /* Stereo Mode, HPF enable, Normal power */
  WM8737L_REG7_ADDR, 0x42,     /* Master Mode, 16-bit audio, i2s format */
  WM8737L_REG8_ADDR, 0x00,     /* Default sampling rate at 44.1kHz */
  WM8737L_REG9_ADDR, 0x0f,     /* Right & left mic preamp bypass switch close */
  WM8737L_REG10_ADDR, 0x03,     /* Misc biases control */
  WM8737L_REG11_ADDR, 0x00,     /* noise gate disable */
  WM8737L_REG12_ADDR, 0x7c,     /* ALC off */
  WM8737L_REG13_ADDR, 0x00,     /* ALC off */
  WM8737L_REG14_ADDR, 0x32,     /* ALC off */
  WM8737L_REG6_ADDR, 0xfc    /* power up digital audio interface */
};
#define SADC_STEREO_LINE_IN_SIZE_V \
                (sizeof(sadc_stereo_line_in_val)/sizeof(sadc_reg_type))

sadc_reg_type sadc_mono_line_in_val[] = {
  WM8737L_REG6_ADDR, 0xbc, /* 0 first power up the analog circuits        */
  WM8737L_REG0_ADDR, 0xc3, /* 1 configure left  PGA = 0db                 */
  WM8737L_REG1_ADDR, 0xc3, /* 2 configure right PGA = 0db                 */
  WM8737L_REG2_ADDR, 0x0f, /* 3 Configure left  audio path LIN1 selected  */
  WM8737L_REG3_ADDR, 0x0f, /* 4 Configure right audio path RIN1 selected  */
  WM8737L_REG4_ADDR, 0x00, /* 5 Diabled 3D Enhance                        */
  WM8737L_REG5_ADDR, 0x00, /* 6 Stereo Mode, HPF enable, Normal power     */
  WM8737L_REG7_ADDR, 0x42, /* 7 Master Mode, 16-bit audio, i2s format     */
  WM8737L_REG8_ADDR, 0x00, /* 8 Default sampling rate at 44.1kHz          */
  WM8737L_REG9_ADDR, 0x0f, /* 9 Right & left mic preamp bypass switch close */
  WM8737L_REG10_ADDR,0x03, /* 10 Misc biases control                      */
  WM8737L_REG11_ADDR,0x00, /* 11 noise gate disable                       */
  WM8737L_REG12_ADDR,0x7c, /* 12 ALC off                                  */
  WM8737L_REG13_ADDR,0x00, /* 13 ALC off                                  */
  WM8737L_REG14_ADDR,0x32, /* 14 ALC off                                  */
  WM8737L_REG6_ADDR, 0xeb  /* 15 power up digital audio interface         */
};
#define SADC_MONO_LINE_IN_SIZE_V \
                (sizeof(sadc_mono_line_in_val)/sizeof(sadc_reg_type))

sadc_reg_type sadc_camcorder_val[] = {
  WM8737L_REG6_ADDR, 0xbc,     /* first power up the analog circuits */
  WM8737L_REG0_ADDR, 0xc3,     /* configure left  PGA = 0db */
  WM8737L_REG1_ADDR, 0xc3,     /* configure right PGA = 0db */
  WM8737L_REG2_ADDR, 0x0f,     /* Configure left  audio path LIN1 selected */
  WM8737L_REG3_ADDR, 0x0f,     /* Configure right audio path RIN1 selected */
  WM8737L_REG4_ADDR, 0x00,     /* Diabled 3D Enhance */
  WM8737L_REG5_ADDR, 0x00,     /* Stereo Mode, HPF enable, Normal power */
  WM8737L_REG7_ADDR, 0x42,     /* Master Mode, 16-bit audio, i2s format */
  WM8737L_REG8_ADDR, 0x00,     /* Default sampling rate at 44.1kHz */
  WM8737L_REG9_ADDR, 0x0f,     /* Right & left mic preamp bypass switch close */
  WM8737L_REG10_ADDR, 0x03,     /* Misc biases control */
  WM8737L_REG11_ADDR, 0x00,     /* noise gate disable */
  WM8737L_REG12_ADDR, 0x7c,     /* ALC off */
  WM8737L_REG13_ADDR, 0x00,     /* ALC off */
  WM8737L_REG14_ADDR, 0x32,     /* ALC off */
  WM8737L_REG6_ADDR, 0xfc    /* power up digital audio interface */
};

#define SADC_CAMCORDER_SIZE_V \
               (sizeof(sadc_camcorder_val)/sizeof(sadc_reg_type))

sadc_mode_type sadc_state = SADC_INVALID;

/* <EJECT> */
/*===========================================================================

FUNCTION sadc_write

DESCRIPTION
  Write a SADC register
    
DEPENDENCIES
  sadc_init must have been called.
  
RETURN VALUE
  status = SADC_PASS if successful.
  status = SADC_FAIL if unsuccessful.
  
SIDE EFFECTS
  None 

===========================================================================*/
sadc_status_type sadc_write( sadc_reg_type *reg )
{
  uint8 buf[2];

  if (sadc_state == SADC_INVALID) { 
    return (SADC_FAILURE);
  }

  buf[0] = (reg->addr << 1) | ((reg->val>>8) & 0x01);
  buf[1] = (reg->val & 0xFF);
  sadc_rw_buf.buf_ptr = buf;

  if (i2c_write(&sadc_rw_buf) != I2C_SUCCESS) {
    MSG_ERROR("sadc off  failed ",0,0,0);
    return (SADC_FAILURE);
  }
  return (SADC_SUCCESS);

} /* sadc_off */

/* <EJECT> */
/*===========================================================================

FUNCTION sadc_init

DESCRIPTION
  Initialize the I2C driver, and look for the sdac.

DEPENDENCIES
  None 
RETURN VALUE
  status = SADC_PASS if successful.
  status = SADC_FAIL if unsuccessful.
SIDE EFFECTS
  None 

===========================================================================*/
sadc_status_type sadc_init (void)
{
  sadc_state = SADC_OFF;

  if (sadc_write(sadc_power_off_val) != I2C_SUCCESS) {
    MSG_ERROR("Failed to detect sadc device",0,0,0);
    sadc_state = SADC_INVALID;
  }

  return (SADC_SUCCESS);
} /* sadc_init */

/*===========================================================================

FUNCTION sadc_enable

DESCRIPTION
  Enable/disable SADC.

DEPENDENCIES
  None

RETURN VALUE
  status = SADC_PASS if successful.
  status = SADC_FAIL if unsuccessful.

SIDE EFFECTS
  None

===========================================================================*/
sadc_status_type sadc_enable (sadc_mode_type mode)
{
  sadc_status_type ret_val;
  uint32           i,size;
  sadc_reg_type    *reg;

  switch (mode) {
    case SADC_OFF:
      size = 1;
      reg = sadc_power_off_val;
      break;

    case SADC_STANDBY:
      size = 1;
      reg = sadc_standby_val;
      break;

    case SADC_STEREO_LINE_IN:
      size = SADC_STEREO_LINE_IN_SIZE_V;
      reg  = sadc_stereo_line_in_val;
      break;

    case SADC_MONO_LINE_IN:
      size = SADC_MONO_LINE_IN_SIZE_V;
      reg  = sadc_mono_line_in_val;
      break;

    case SADC_CAMCORDER:
      size = SADC_CAMCORDER_SIZE_V;
      reg  = sadc_camcorder_val;
      break;

    default:
      return (SADC_FAILURE);
  }

  i = 0;
  do {
    ret_val = sadc_write(&reg[i++]);
  } while ((ret_val == SADC_SUCCESS) && (i < size));

  if (ret_val == SADC_SUCCESS) {
    sadc_state = mode;
  }

  return (ret_val);
} /* sadc_enable */

#endif /* FEATURE_EXTERNAL_SADC */
