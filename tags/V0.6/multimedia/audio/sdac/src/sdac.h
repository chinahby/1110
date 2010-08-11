#ifndef SDAC_H
#define SDAC_H
/*===========================================================================

               S D A C   -   S T E R E O   A U D I O   D A C
       
DESCRIPTION

  This file contains all the definitions necessary to interface to the
  Micronas DAC-3550A Device Driver.

REFERENCES
  MCN 256-36086-0000 "DAC 3550A - Stereo Audio DAC".  Micronas Intermetall.
    July 23, 1999.

Copyright (c) 2000,2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/driver/sdac/main/latest/src/sdac.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/01    st     Modified SDAC_SR_DEFAULT_V to use SDAC_SR_DELAY_1_V.
04/13/00    sm     Added sdac_enable function.  Default init to sdac disabled.
02/21/00    sm     Initial version.

===========================================================================*/

#include "processor.h" /* CPU Specific header file             */
#include "comdef.h"    /* Definitions for byte, word, etc.     */
#include "target.h"    /* Target specific definitions          */

#ifdef FEATURE_EXTERNAL_SDAC
/* <EJECT> */
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
/*  SDAC Addresses
**  The DAC 3550A only allows write functions.  We are unable to read.
*/
#define SDAC_WRITE_ADDR         0x9A /* Write mode device select address   */

/*  SDAC Register definitions
*/
#define SDAC_MAX_REG_SIZE          2 /* Largest register is 2 bytes        */

/*  Sample Rate Control Register
*/
#define SDAC_SAMPLE_RATE_CTL    0x01
#define SDAC_SAMPLE_RATE_CTL_S     1 /* Sample rate control reg is 1 byte. */

#define SDAC_SR_RESERVED_M      0xE0 /* Reserved bits.                     */
#define SDAC_SR_RESERVED_V      0x00 /* Reserved bits are zeroed.          */

#define SDAC_SR_LR_SEL_M        0x10 /* Select Left/Right channel format.  */
#define SDAC_SR_LR_SEL_L_V      0x00 /* WSI at 0 selects left channel.     */
#define SDAC_SR_LR_SEL_R_V      0x10 /* WSI at 0 selects right channel.    */

#define SDAC_SR_DELAY_M         0x08 /* Receive channel control mask       */
#define SDAC_SR_DELAY_NONE_V    0x00 /* Select no delay enabled            */
#define SDAC_SR_DELAY_1_V       0x08 /* Receive channel muted              */

#define SDAC_SR_SAMP_RATE_M     0x07 /* Sample rate selection              */
#define SDAC_SR_SAMP_RATE_48_V  0x00 /* Select sample rates from 32-48 kHz.*/
#define SDAC_SR_SAMP_RATE_32_V  0x01 /* Select sample rates from 26-32 kHz.*/
#define SDAC_SR_SAMP_RATE_24_V  0x02 /* Select sample rates from 20-26 kHz.*/
#define SDAC_SR_SAMP_RATE_16_V  0x03 /* Select sample rates from 14-20 kHz.*/
#define SDAC_SR_SAMP_RATE_12_V  0x04 /* Select sample rates from 10-14 kHz.*/
#define SDAC_SR_SAMP_RATE_08_V  0x05 /* Select sample rates from 08-10 kHz.*/
#define SDAC_SR_SAMP_RATE_AU_V  0x06 /* Autoselect rate.                   */

/* Default value for software initialization */
#define SDAC_SR_DEFAULT_M ( SDAC_SR_RESERVED_M \
                          | SDAC_SR_LR_SEL_M   \
                          | SDAC_SR_DELAY_M    \
                          | SDAC_SR_SAMP_RATE_M )

#define SDAC_SR_DEFAULT_V ( SDAC_SR_RESERVED_V   \
                          | SDAC_SR_LR_SEL_L_V   \
                          | SDAC_SR_DELAY_1_V \
                          | SDAC_SR_SAMP_RATE_48_V )


/* Analog Volume Register
*/
#define SDAC_AVOL_CTL           0x02
#define SDAC_AVOL_CTL_S            2   /* Analog volume reg is 2 bytes.    */

#define SDAC_AVOL_RESERVED_M    0x80C0 /* Reserved bits.                   */
#define SDAC_AVOL_RESERVED_V    0x0000 /* Reserved bits are zeroed.        */

#define SDAC_AVOL_DEEMPHASIS_M  0x4000 /* Deemphasis flag.                 */
#define SDAC_AVOL_DEEMPH_OFF_V  0x0000 /* Deemphasis off.                  */
#define SDAC_AVOL_DEEMPH_ON_V   0x4000 /* Deemphasis on.                   */

#define SDAC_AVOL_LEFT_M        0x3F00 /* Analog volume, left channel.     */
#define SDAC_AVOL_RIGHT_M       0x003F /* Analog volume, right channel.    */

/* Table of analog volumes.  For use with both left & right channels. */
#define SDAC_AVOL_MUTE_V        0x00   /* MUTE */
#define SDAC_AVOL_180_V         0x38   /*  18.0 dB */
#define SDAC_AVOL_165_V         0x37
#define SDAC_AVOL_150_V         0x36
#define SDAC_AVOL_135_V         0x35
#define SDAC_AVOL_120_V         0x34
#define SDAC_AVOL_105_V         0x33
#define SDAC_AVOL_90_V          0x32
#define SDAC_AVOL_75_V          0x31
#define SDAC_AVOL_60_V          0x30
#define SDAC_AVOL_45_V          0x2F
#define SDAC_AVOL_30_V          0x2E
#define SDAC_AVOL_15_V          0x2D   /*   1.5 dB */
#define SDAC_AVOL_0_V           0x2C   /*     0 dB */
#define SDAC_AVOL_N15_V         0x2B   /*  -1.5 dB */
#define SDAC_AVOL_N30_V         0x2A
#define SDAC_AVOL_N45_V         0x29
#define SDAC_AVOL_N60_V         0x28
#define SDAC_AVOL_N75_V         0x27
#define SDAC_AVOL_N90_V         0x26
#define SDAC_AVOL_N105_V        0x25
#define SDAC_AVOL_N120_V        0x24
#define SDAC_AVOL_N135_V        0x23
#define SDAC_AVOL_N150_V        0x22
#define SDAC_AVOL_N165_V        0x21
#define SDAC_AVOL_N180_V        0x20
#define SDAC_AVOL_N195_V        0x1F
#define SDAC_AVOL_N210_V        0x1E
#define SDAC_AVOL_N225_V        0x1D
#define SDAC_AVOL_N240_V        0x1C
#define SDAC_AVOL_N255_V        0x1B
#define SDAC_AVOL_N270_V        0x1A
#define SDAC_AVOL_N285_V        0x19
#define SDAC_AVOL_N300_V        0x18
#define SDAC_AVOL_N315_V        0x17
#define SDAC_AVOL_N330_V        0x16
#define SDAC_AVOL_N345_V        0x15
#define SDAC_AVOL_N360_V        0x14
#define SDAC_AVOL_N375_V        0x13
#define SDAC_AVOL_N390_V        0x12
#define SDAC_AVOL_N405_V        0x11
#define SDAC_AVOL_N420_V        0x10
#define SDAC_AVOL_N435_V        0x0F
#define SDAC_AVOL_N450_V        0x0E
#define SDAC_AVOL_N465_V        0x0D
#define SDAC_AVOL_N480_V        0x0C
#define SDAC_AVOL_N495_V        0x0B
#define SDAC_AVOL_N510_V        0x0A
#define SDAC_AVOL_N525_V        0x09
#define SDAC_AVOL_N540_V        0x08
#define SDAC_AVOL_N570_V        0x07
#define SDAC_AVOL_N600_V        0x06
#define SDAC_AVOL_N630_V        0x05
#define SDAC_AVOL_N660_V        0x04
#define SDAC_AVOL_N690_V        0x03
#define SDAC_AVOL_N720_V        0x02
#define SDAC_AVOL_N750_V        0x01   /* -75.0 dB */

/* Default value for software initialization */
#define SDAC_AVOL_DEFAULT_M ( SDAC_AVOL_RESERVED_M   \
                            | SDAC_AVOL_DEEMPHASIS_M \
                            | SDAC_AVOL_LEFT_M       \
                            | SDAC_AVOL_RIGHT_M )

#define SDAC_AVOL_DEFAULT_V ( SDAC_AVOL_RESERVED_V    \
                            | SDAC_AVOL_DEEMPH_OFF_V  \
                            | LEFT_VOL(SDAC_AVOL_0_V) \
                            | RIGHT_VOL(SDAC_AVOL_0_V) )


/* Global configuration register
*/
#define SDAC_GLOBAL_CFG_CTL     0x03
#define SDAC_GLOBAL_CFG_CTL_S      1 /* Global config reg is 1 byte.       */

#define SDAC_GCFG_RESERVED_M    0x80 /* Reserved bits.                     */
#define SDAC_GCFG_RESERVED_V    0x00 /* Reserved bits are zeroed.          */

#define SDAC_GCFG_SEL_V_M       0x40 /* Voltage select.                    */
#define SDAC_GCFG_SEL_3V_V      0x00 /* Select 3 volt operation.           */
#define SDAC_GCFG_SEL_5V_V      0x40 /* Select 5 volt operation.           */

#define SDAC_GCFG_PWR_MODE_M    0x20 /* Power mode select.                 */
#define SDAC_GCFG_PWR_NORMAL_V  0x00 /* Normal power mode.                 */
#define SDAC_GCFG_PWR_LOW_V     0x20 /* Low power mode.                    */

#define SDAC_GCFG_AUX2_SEL_M    0x10 /* Select aux 2 input pins.           */
#define SDAC_GCFG_AUX2_OFF_V    0x00 /* Aux 2 input off.                   */
#define SDAC_GCFG_AUX2_ON_V     0x10 /* Aux 2 input on.                    */

#define SDAC_GCFG_AUX1_SEL_M    0x08 /* Select aux 1 input pins.           */
#define SDAC_GCFG_AUX1_OFF_V    0x00 /* Aux 1 input off.                   */
#define SDAC_GCFG_AUX1_ON_V     0x08 /* Aux 1 input on.                    */

#define SDAC_GCFG_DAC_SEL_M     0x04 /* Select DAC operation.              */
#define SDAC_GCFG_DAC_OFF_V     0x00 /* DAC off.                           */
#define SDAC_GCFG_DAC_ON_V      0x04 /* DAC on.                            */

#define SDAC_GCFG_AUX_CHN_M     0x02 /* Select aux input channels.         */
#define SDAC_GCFG_AUX_STEREO_V  0x00 /* Aux input as stereo.               */
#define SDAC_GCFG_AUX_MONO_V    0x02 /* Aux input as mono.                 */

#define SDAC_GCFG_IRPA_M        0x01 /* Invert right power amplifier select*/
#define SDAC_GCFG_IRPA_OFF_V    0x00 /* Don't invert RPA.                  */
#define SDAC_GCFG_IRPA_ON_V     0x01 /* Invert RPA.                        */

/* Default value for software initialization */
#define SDAC_GCFG_DEFAULT_M ( SDAC_GCFG_RESERVED_M \
                            | SDAC_GCFG_PWR_MODE_M \
                            | SDAC_GCFG_SEL_V_M    \
                            | SDAC_GCFG_AUX2_SEL_M \
                            | SDAC_GCFG_AUX1_SEL_M \
                            | SDAC_GCFG_DAC_SEL_M  \
                            | SDAC_GCFG_AUX_CHN_M  \
                            | SDAC_GCFG_IRPA_M )

#define SDAC_GCFG_DEFAULT_V ( SDAC_GCFG_RESERVED_V   \
                            | SDAC_GCFG_PWR_NORMAL_V \
                            | SDAC_GCFG_SEL_3V_V     \
                            | SDAC_GCFG_AUX2_OFF_V   \
                            | SDAC_GCFG_AUX1_OFF_V   \
                            | SDAC_GCFG_DAC_OFF_V    \
                            | SDAC_GCFG_AUX_STEREO_V \
                            | SDAC_GCFG_IRPA_OFF_V )


/* Default Powerup values for SDAC Registers as per document.
** For reference only!
*/
#define SDAC_DEF_SR_CTL_V       0x00   /* Sample rate control register value */
#define SDAC_DEF_AVOL_CTL_V     0x2C2C /* Analog volume ctl register val     */
#define SDAC_DEF_GCFG_CTL_V     0x04   /* Global config register value       */


/* <EJECT> */
/*  SDAC Function return values
*/
typedef enum
{
  SDAC_PASS = 0,
  /* Operation was successful */
  SDAC_FAIL
  /* Write operation failed */
} sdac_status_type;

/* <EJECT> */
/* The number of registers in the SDAC
** 
*/
#define SDAC_IMAGE_SIZE         3

/* <EJECT> */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define LEFT_VOL(x) ((x) << 8)           /* Make LEFT channel volume       */
#define RIGHT_VOL(x) (x)                 /* Make RIGHT channel volume      */

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION sdac_init

DESCRIPTION
  Initialize the I2C driver, and look for the sdac.

DEPENDENCIES

RETURN VALUE
  status = SDAC_PASS if successful.
  status = SDAC_FAIL if unsuccessful.

SIDE EFFECTS
  

===========================================================================*/
extern sdac_status_type sdac_init (void);

/* <EJECT> */
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
extern sdac_status_type sdac_enable (boolean enable);

/* <EJECT> */
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
extern sdac_status_type sdac_set_sample_rate (byte sample_rate);

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
extern sdac_status_type sdac_write(
  byte reg,                       /* Register to write */
  byte *mask,                     /* Mask for register */
  byte *val                       /* Value to write */
);
#endif /* FEATURE_EXTERNAL_SDAC */
#endif /* SDAC_H */

