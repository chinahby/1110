#ifndef _HSU_ANALOG_AUDIO_H_
#define _HSU_ANALOG_AUDIO_H_

/*==============================================================================

Analog Audio over High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API for switching between Analog Audio USB and USB normal mode

EXTERNALIZED FUNCTIONS
hsu_analog_audio_init
hsu_analog_audio_config

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_analog_audio_init

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_analog_audio.h
when     who  what, where, why
-------- ---  ---------------------------------------------------------
03/30/08 tli   First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" 
#include "hsu_common.h"

/*==============================================================================
Typedefs
==============================================================================*/
/* This type defines the modes each core can be in */
typedef enum{
  HSU_AUDIO_MODE_STEREO_SPK = 0,      /* Stereo speakers */
  HSU_AUDIO_MODE_STEREO_SPK_MIC = 1,  /* Stereo speakers + mic*/
  HSU_AUDIO_MODE_MONO_SPK = 2,        /* Mono speakers */
  HSU_AUDIO_MODE_MONO_SPK_MIC = 3,    /* Mono speakeres + mic*/
  HSU_AUDIO_MODE_USB = 4,             /* Usb normal mode - default val. */
  HSU_AUDIO_MODE_INVALID = 5
} hsu_analog_audio_mode_type ;

/* The callback to be called on completing the Analog Audio configuration.
   It will return the result of the configuration process */
typedef void (*hsu_analog_audio_conf_cb_type)(boolean result);

/* This type describes the parameter passed to hsu_analog_audio_config() */
typedef struct{
  hsu_analog_audio_mode_type new_mode;        /* The mode to switch to */
  hsu_analog_audio_conf_cb_type completion_cb; /* The callback to be called
                                                 on completing the configuration*/
} hsu_analog_audio_conf_param_type ; 
/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_analog_audio_config

DESCRIPTION
Switches between different Audio modes and regular USB mode.
In case of switching to Analog Audio mode (from USB) the USB core is stopped, 
put D+/D- signals into high-impedance mode.
Receives parameter of type hsu_analog_audio_conf_param_type.
NOTE: The function is not reentrant! 

DEPENDENCIES
hsu_analog_audio_init() must be called prior to this function
Should be called from AL task context

RETURN VALUE
None

SIDE EFFECTS
===========================================================================*/
void hsu_analog_audio_config(void *param);

/*===========================================================================

FUNCTION hsu_analog_audio_init

DESCRIPTION
Inits internal module data structures

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_analog_audio_init(void);

/*===========================================================================

FUNCTION hsu_analog_audio_get_curr_mode

DESCRIPTION
Returns the current mode the USB core is in.

DEPENDENCIES
None

RETURN VALUE
Current mode the USB core is in.

SIDE EFFECTS
None

===========================================================================*/
hsu_analog_audio_mode_type hsu_analog_audio_get_curr_mode(void);

#endif /* _HSU_ANALOG_AUDIO_H_ */
