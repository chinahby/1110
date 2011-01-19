#ifndef SADC_H
#define SADC_H
/*===========================================================================

               S A D C   -   S T E R E O   A U D I O   A D C
       
DESCRIPTION

  This file contains all the definitions necessary to interface to the
  Wolfson WM8737L Stereo ADC device driver.

REFERENCES

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/driver/sadc/main/latest/src/sadc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/05    aw     Removed unsuppored mode.
04/08/05    hz     Added MONO_LINE_IN configuration.
01/21/05    aw     First verison.

===========================================================================*/
#include "comdef.h"    /* Definitions for byte, word, etc.     */
#include "target.h"    /* Target specific definitions          */

#ifdef FEATURE_EXTERNAL_SADC
/* <EJECT> */
/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
typedef enum {
  SADC_OFF,             /* IC is in full power down mode; wake-up process  */
                        /* will take several seconds to charge the VMID    */
                        /* capacitor                                       */
  SADC_STANDBY,         /* Everything is shutdown except VMID circuit;     */
                        /* wake-up will take only a few milliseconds.      */
                        /* This mode should be utilized when a Camcorder   */
                        /* or other Stereo Encoding App is active.         */
  SADC_STEREO_LINE_IN,  /* Mic Preamps are disabled; Auto Level Control    */
                        /* (ALC) and Noise Gate are disabled               */
  SADC_MONO_LINE_IN,    /* Mic Preamps are disabled; Auto Level Control    */
 
  SADC_CAMCORDER,       /* Mic Preamps are enabled; ALC and Noise Gate are */
                        /* enabled                                         */
  SADC_INVALID,
} sadc_mode_type;

/*  SADC Function return values
*/
typedef enum
{
  SADC_SUCCESS = 0,     /* Operation was successful */
  SADC_FAILURE          /* Write operation failed */
} sadc_status_type;

/* <EJECT> */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION sadc_init

DESCRIPTION
  Initialize the I2C driver, and look for the sdac.

DEPENDENCIES

RETURN VALUE
  status = SADC_PASS if successful.
  status = SADC_FAIL if unsuccessful.

SIDE EFFECTS
  

===========================================================================*/
extern sadc_status_type sadc_init (void);

/* <EJECT> */
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
extern sadc_status_type sadc_enable
(
  sadc_mode_type mode
);

#endif /* FEATURE_EXTERNAL_SADC */
#endif /* SADC_H */

