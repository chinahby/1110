/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
       
                S D A C   -   S T E R E O   A U D I O   D A C

GENERAL DESCRIPTION
  This device driver provides a simple interface between DMSS audio routines
  and the DAC-3550A stereo audio DAC.

REFERENCES
  MCN 256-36086-0000 "DAC 3550A - Stereo Audio DAC".  Micronas Intermetall.
    July 23, 1999.

EXTERNALIZED FUNCTIONS
  sdac_init
    Initialize the I2C driver, and look for the sdac.
  sdac_write
    Writes a value to the specified register.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000,2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/driver/sdac/main/latest/src/sdac.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/02    st     Modified code to support the SDAC enable line.
03/11/02    aw     Remove i2c_init() from sdac_init() and unnecessary options.
                   Rename I2C bus id.
04/13/00    sm     Added sdac_enable function.  Default init to sdac disabled.
02/21/00    sm     First version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_EXTERNAL_SDAC

#include "msg.h"       /* Message logging/reporting services   */
#include "err.h"       /* Error logging/reporting services     */
#include "i2c.h"       /* I2C driver definitions               */
#include "sdac.h"      /* SDAC data and prototypes           */

#ifdef FEATURE_ENABLE_SDAC
#error code not present
#endif /* FEATURE_ENABLE_SDAC */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Stereo DAC I2C bus */
#define SDAC_I2C_BUS  I2C_BUS_HW_CTRL

/* This flag is TRUE when the SDAC is detected. */
LOCAL boolean sdac_detected = FALSE ;  

/* This flag is TRUE when the sdac is being initialized */ 
LOCAL boolean sdac_initializing = FALSE;

/* Array used as a shadow buffer for the output registers of the SDAC
** This allows individual bits to be written differently without taking
** the time to read back the current contents of the register.
** As we cannot read the SDAC memory, we initialize the default values.
*/
typedef struct {
  byte size;
  byte data[SDAC_MAX_REG_SIZE];
} sdac_image_type;

LOCAL sdac_image_type sdac_image[ SDAC_IMAGE_SIZE ] = {
  {SDAC_SAMPLE_RATE_CTL_S,
    { SDAC_SR_DEFAULT_V, 0 }},
  {SDAC_AVOL_CTL_S,
    { WORD_HI(SDAC_AVOL_DEFAULT_V), WORD_LO(SDAC_AVOL_DEFAULT_V) }},
  {SDAC_GLOBAL_CFG_CTL_S,
    { SDAC_GCFG_DEFAULT_V, 0 }}
};

/* Variable used to setup the i2c port to access the SDAC
*/
LOCAL i2c_rw_cmd_type sdac_rw_buf =
{
  SDAC_I2C_BUS,   /* Bus ID */
  0,              /* Slave address */
  I2C_REG_DEV,    /* Options */
  NULL,           /* Buffer */
  0,              /* Register address */
  0,              /* Count */
  0               /* last index */
};

/* <EJECT> */
/*===========================================================================

FUNCTION sdac_write

DESCRIPTION
  Write a SDAC register
    
DEPENDENCIES
  sdac_init must have been called.
  
RETURN VALUE
  status = SDAC_PASS if successful.
  status = SDAC_FAIL if unsuccessful.
  
SIDE EFFECTS
  None 

===========================================================================*/
sdac_status_type sdac_write(
  byte reg,                       /* Register to write */
  byte *mask,                     /* Mask for register */
  byte *val                       /* Value to write */
)
{
  int i;
  byte reg_idx = reg - 1;

  if (!sdac_detected && !sdac_initializing) {
    return (SDAC_FAIL);
  }

  /* Check valid reg (also checks if reg was 0).
  */
  if (reg_idx > SDAC_IMAGE_SIZE-1) {
    ERR("Register address out of range",0,0,0);
    return (SDAC_FAIL);
  }

  /* Update image
  */
  for(i = 0; i < sdac_image[ reg_idx ].size; i++) {
    sdac_image[ reg_idx ].data[ i ] =
      (sdac_image[ reg_idx ].data[ i ] & ~mask[ i ]) | (val[ i ] & mask[ i ]);
  }

  sdac_rw_buf.slave_addr = SDAC_WRITE_ADDR; 
  sdac_rw_buf.buf_ptr    = sdac_image[ reg_idx ].data;
  sdac_rw_buf.addr.reg   = reg;
  sdac_rw_buf.len        = sdac_image[ reg_idx ].size;
  if (i2c_write(&sdac_rw_buf) != I2C_SUCCESS) {
    MSG_HIGH("Write failed, reg 0x%x", reg, 0, 0);
    return (SDAC_FAIL);
  }

  return (SDAC_PASS);

} /* sdac_write */
/* <EJECT> */
/*===========================================================================

FUNCTION sdac_init

DESCRIPTION
  Initialize the I2C driver, and look for the sdac.

DEPENDENCIES
  None 
RETURN VALUE
  status = SDAC_PASS if successful.
  status = SDAC_FAIL if unsuccessful.
SIDE EFFECTS
  None 

===========================================================================*/
sdac_status_type sdac_init (void)
{
  int  i;                              /* Count                              */
  byte mask_all[] = {
    0xff, 0xff
  };

  sdac_initializing = TRUE;          /* Allows sdac_write() access       */

#ifdef FEATURE_ENABLE_SDAC
#error code not present
#endif /* FEATURE_ENABLE_SDAC */

  /* Now write the default power-up state of the device, and save it in the
  ** the image.
  */
  if (!sdac_detected) {
    for (i = 0; i < SDAC_IMAGE_SIZE; i++) {
      if (sdac_write((byte) i+1, mask_all, sdac_image[i].data) != SDAC_PASS)
      {
        MSG_HIGH("Write failed, reg 0x%x", i+1, 0, 0);
        sdac_initializing = FALSE;   /* Disallow writes on SDAC regs     */
        return (SDAC_FAIL);
      } 
    }
  }
  sdac_detected = TRUE;              /* SDAC has a normal power up state.*/
  sdac_initializing = FALSE;         /* Allow writes on SDAC regs        */
  MSG_HIGH("SDAC initialized",0,0,0);
  return (SDAC_PASS);
} /* sdac_init */

/*===========================================================================

FUNCTION sdac_enable

DESCRIPTION
  Enable/disable SDAC.

DEPENDENCIES
  None

RETURN VALUE
  status = SDAC_PASS if successful.
  status = SDAC_FAIL if unsuccessful.

SIDE EFFECTS
  None

===========================================================================*/
sdac_status_type sdac_enable (boolean enable)
{
  sdac_status_type ret_val;
  byte tbyte;
  byte mask = SDAC_GCFG_DAC_SEL_M;
  word mask_vol = SDAC_AVOL_LEFT_M | SDAC_AVOL_RIGHT_M;
  byte vol[2];

  if(enable) {
    tbyte = SDAC_GCFG_DAC_ON_V;
    vol[0] = SDAC_AVOL_0_V;
    vol[1] = SDAC_AVOL_0_V;
    ret_val = sdac_write(SDAC_GLOBAL_CFG_CTL, &mask, &tbyte);
    if(ret_val == SDAC_PASS) {
      ret_val = sdac_write(SDAC_AVOL_CTL, (byte *)&mask_vol, vol);
    }
  } else {
    vol[0] = SDAC_AVOL_MUTE_V;
    vol[1] = SDAC_AVOL_MUTE_V;
    tbyte = SDAC_GCFG_DAC_OFF_V;
    ret_val = sdac_write(SDAC_AVOL_CTL, (byte *)&mask_vol, vol);
    if(ret_val == SDAC_PASS) {
      ret_val = sdac_write(SDAC_GLOBAL_CFG_CTL, &mask, &tbyte);
    }
  }
  return(ret_val);

} /* sdac_enable */

/*===========================================================================

FUNCTION sdac_set_sample_rate

DESCRIPTION
  Sets sample rate on SDAC.

DEPENDENCIES
  None

RETURN VALUE
  status = SDAC_PASS if successful.
  status = SDAC_FAIL if unsuccessful.

SIDE EFFECTS
  None

===========================================================================*/
sdac_status_type sdac_set_sample_rate (byte sample_rate)
{
  byte mask = SDAC_SR_SAMP_RATE_M;

  return(sdac_write(SDAC_SAMPLE_RATE_CTL, &mask, &sample_rate));
} /* sdac_set_sample_rate */
#endif /* FEATURE_EXTERNAL_SDAC */
