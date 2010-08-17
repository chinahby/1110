/*==============================================================================
Analog Audio over High Speed USB

GENERAL DESCRIPTION
Contains the API switching between Analog Audio USB and USB normal mode

EXTERNALIZED FUNCTIONS
hsu_analog_audio_init
hsu_analog_audio_activated
hsu_analog_audio_config

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_analog_audio_init

Copyright (c) 2008, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_analog_audio.c

when      who     what, where, why
--------  ---     ------------------------------------------------------
01/04/08  tli     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_analog_audio.h"
#include "os_common.h"
#include "os_port.h"
#include "hsu_mdm_apis.h"
#include "hsu_phy_utils.h"

#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_stack_utils.h"

#ifndef FEATURE_HSU_TEST
#include <msm.h>
#endif
#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#endif /* FEATURE_HS_USB_HAL */

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/
typedef struct {
  /* These fields holds data about the mode the USB core is currently in*/
  hsu_analog_audio_mode_type core_mode ;
  boolean initialized ;
}hsu_analog_audio_data_type;

/*==============================================================================
Variables
==============================================================================*/
static hsu_analog_audio_data_type hsu_analog_audio_data;
/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
static boolean hsu_analog_audio_stop(void);
static boolean hsu_analog_audio_start
(
 hsu_analog_audio_mode_type new_mode
);
static boolean hsu_analog_audio_config_phy_to_mode
(
 hsu_analog_audio_mode_type new_mode
);
/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

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
void hsu_analog_audio_init(void)
{
  hsu_analog_audio_data.core_mode = HSU_AUDIO_MODE_INVALID;
  hsu_analog_audio_data.initialized = TRUE;
}

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
hsu_analog_audio_mode_type hsu_analog_audio_get_curr_mode(void)
{
  return hsu_analog_audio_data.core_mode;
}

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
TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
void hsu_analog_audio_config(void *param)
{
  hsu_analog_audio_conf_param_type *configuration_param = NULL;
  boolean res = TRUE;

  HSU_MSG_HIGH("*_config: enter",0,0,0);

  configuration_param = (hsu_analog_audio_conf_param_type*)param;
  if (!configuration_param)
  {
    HSU_MSG_ERROR("*_config: Invalid parameter",0,0,0);
    return;
  }

  if (configuration_param->new_mode >= HSU_AUDIO_MODE_INVALID)
  {
    HSU_MSG_ERROR("*_config: Invalid mode[%d].",configuration_param->new_mode, 0, 0);
    res = FALSE ;
  }

  if (!hsu_analog_audio_data.initialized)
  {
    HSU_MSG_ERROR("*_config: Analog Audio was not initialized!",0,0,0);
    res = FALSE;
  }

  if (hsu_analog_audio_data.core_mode == configuration_param->new_mode)
  {
    HSU_MSG_LOW("*_config: USB Core is already in mode [%d]",
      hsu_analog_audio_data.core_mode,0,0);
  }
  else
  {
    switch(hsu_analog_audio_data.core_mode) 
    {
    case HSU_AUDIO_MODE_STEREO_SPK:
    case HSU_AUDIO_MODE_STEREO_SPK_MIC:
    case HSU_AUDIO_MODE_MONO_SPK:
    case HSU_AUDIO_MODE_MONO_SPK_MIC:
      if (configuration_param->new_mode == HSU_AUDIO_MODE_USB)
      { /*We are now in one of the analog modes and need to switch back to USB*/
        res = hsu_analog_audio_stop();
      }
      else /* We are already in one of the analog modes so the USB core is
              stopped. All we need to do is re-configure the PHY */
      {
        res = hsu_analog_audio_config_phy_to_mode(configuration_param->new_mode);
      }
      break;
    case HSU_AUDIO_MODE_USB:
      res = hsu_analog_audio_start(configuration_param->new_mode);
      break;
    case HSU_AUDIO_MODE_INVALID:
      /* This is the initiation: If the new mode is USB it means that the
        USB stack is already up and no action is required */
      if (configuration_param->new_mode == HSU_AUDIO_MODE_USB)
      {
        hsu_analog_audio_data.core_mode = configuration_param->new_mode;
      }
      else
      { /* Start one of the analog audio modes */
        res = hsu_analog_audio_start(configuration_param->new_mode);
      }
      break;
    default:
      HSU_MSG_ERROR("*_config: Invalid mode[%d].",hsu_analog_audio_data.core_mode, 0, 0);
      res = FALSE;
    }
  }

  if (configuration_param->completion_cb)
  {
    configuration_param->completion_cb(res);
  }
  HSU_MSG_HIGH("*_config: exit",0,0,0);
}
/*===========================================================================

FUNCTION hsu_analog_audio_config_phy_to_mode

DESCRIPTION
Configures the PHY to the given Analog mode

DEPENDENCIES
None

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
static boolean hsu_analog_audio_config_phy_to_mode
(
 hsu_analog_audio_mode_type new_mode
)
{
  int audr_en_val = 0;
  int audl_en_val = 0;
  int spkr_lrshort_val = 0 ;
  int mic_en_val = 0;
  int mic_id_sel_val = 1;

  switch(new_mode) {
  case HSU_AUDIO_MODE_STEREO_SPK:
    /* Left speaker D-, Right speaker D+ :
    AUDR_EN = 1
    AUDL_EN = 1
    SPKR_LRSHORT = 0 */
    audr_en_val = 1;
    audl_en_val = 1;
    spkr_lrshort_val = 0 ;
    mic_en_val = 0;
    break;
  case HSU_AUDIO_MODE_STEREO_SPK_MIC:
    /* Left speaker D-, Right speaker D+, Mic ID :
    AUDR_EN = 1
    AUDL_EN = 1
    SPKR_LRSHORT = 0
    MIC_EN = 1
    MIC_ID_SEL = 1 */
    audr_en_val = 1;
    audl_en_val = 1;
    spkr_lrshort_val = 0 ;
    mic_en_val = 1;
    break;
  case HSU_AUDIO_MODE_MONO_SPK:
    /* Left speaker D-, Right speaker D+ :
    AUDR_EN = 1
    AUDL_EN = 1
    SPKR_LRSHORT = 1
    MIC_EN = 0
    MIC_ID_SEL = 0 */
    audr_en_val = 1;
    audl_en_val = 1;
    spkr_lrshort_val = 1 ;
    mic_en_val = 0;
    break;
  case HSU_AUDIO_MODE_MONO_SPK_MIC:
    /* Speaker D-, Mic D+ :
    AUDR_EN = 0
    AUDL_EN = 1
    SPKR_LRSHORT = 1
    MIC_EN = 1
    MIC_ID_SEL = 0 */
    audr_en_val = 0;
    audl_en_val = 1;
    spkr_lrshort_val = 1 ;
    mic_en_val = 1;
    break;
  case HSU_AUDIO_MODE_USB:
    /* :
    AUDR_EN = 0
    AUDL_EN = 0
    SPKR_LRSHORT = 0
    MIC_EN = 0
    MIC_ID_SEL = 0 */
    audr_en_val = 0;
    audl_en_val = 0;
    spkr_lrshort_val = 0 ;
    mic_en_val = 0;
    break;
  default:
    HSU_MSG_ERROR("*_config_phy_to_mode: Invalid mode[%d].",new_mode, 0, 0);
    return FALSE;
  }
  HWIO_OUTF(ULPI_PHY_CONFIG,AUDR_EN, audr_en_val);
  HWIO_OUTF(ULPI_PHY_CONFIG,AUDL_EN, audl_en_val);
  HWIO_OUTF(ULPI_PHY_CONFIG,SPKR_LRSHORT,spkr_lrshort_val);
  HWIO_OUTF(ULPI_PHY_CONFIG,MIC_EN, mic_en_val);
  HWIO_OUTF(ULPI_PHY_CONFIG,MIC_ID_SEL, mic_id_sel_val);

  hsu_analog_audio_data.core_mode = new_mode;
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_analog_audio_start

DESCRIPTION
This function stops the usb core, puts the USB PHY in LPM and configures
the PHY to the selected analog mode

DEPENDENCIES
None

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
===========================================================================*/
static boolean hsu_analog_audio_start
(
 hsu_analog_audio_mode_type new_mode
 )
{
  uint8 phy_reg_value;
  /* cores_data[core_index] can't be HSU_AUDIO_MODE_INVALID.
     We checked it in hsu_analog_audio_config() before branching here */
  if (hsu_analog_audio_data.core_mode != HSU_AUDIO_MODE_USB)
  {
    HSU_MSG_HIGH("*_start: Analog Audio is already active for this core",0,0,0);
    return TRUE;
  }
  /* cannot start Analog audio while stack is in forced_disable mode */
  if (hsu_conf_sel_stack_utils_is_forced_disable() == TRUE)
  {
    HSU_MSG_HIGH("*_start: USB stack is in forced_disable mode",0,0,0);
    return FALSE;
  }

  /* disable usb stack */
  if (hsu_conf_sel_stack_utils_disable() == FALSE)
  {
    return FALSE;
  }

  /* Additional PHY configurations that are needed to set PHY in Analog 
     Audio mode: */
  /* Clear bit #7 (gpanaiosel) of ULPI register 0x30 */
  if (!hsu_os_bus_read_from_phy(0x30,&phy_reg_value))
  {
    HSU_MSG_ERROR("*_start: Couldn't read from PHY (reg=0x30)",0,0,0);
    return FALSE;
  }
  phy_reg_value &= 0x7F;
  if (!hsu_os_bus_write_to_phy(0x30,phy_reg_value))
  {
    HSU_MSG_ERROR("*_start: Couldn't write to PHY (reg=0x30)",0,0,0);
    return FALSE;
  }
  /* Set ULPI register 0x0A to 0 */
  if (!hsu_os_bus_write_to_phy(0x0A,0))
  {
    HSU_MSG_ERROR("*_start: Couldn't write to PHY (reg=0x0A)",0,0,0);
    return FALSE;
  }
  /* Set ULPI 0x34 register to 0x3 */
  if (!hsu_os_bus_write_to_phy(0x34,0x3))
  {
    HSU_MSG_ERROR("*_start: Couldn't write to PHY (reg=0x34)",0,0,0);
    return FALSE;
  }
  /* Set ULPI 0x4 register to 0x4D  */
  if (!hsu_os_bus_write_to_phy(0x4,0x4D))
  {
    HSU_MSG_ERROR("*_start: Couldn't write to PHY (reg=0x4)",0,0,0);
    return FALSE;
  }

  hsu_os_bus_enter_lpm(HS_USB_CORE_INDEX, FALSE, TRUE);
  return hsu_analog_audio_config_phy_to_mode(new_mode);
}


/*===========================================================================

FUNCTION hsu_analog_audio_stop

DESCRIPTION
This function starts the USB stack, activates the core and reconnects the PHY

DEPENDENCIES
None

RETURN VALUE
TRUE/FALSE

SIDE EFFECTS
===========================================================================*/

static boolean hsu_analog_audio_stop()
{
  if ((hsu_analog_audio_data.core_mode == HSU_AUDIO_MODE_USB) ||
     (hsu_analog_audio_data.core_mode == HSU_AUDIO_MODE_INVALID))
  {
    HSU_MSG_HIGH("*_stop: Analog Audio is NOT active for this core",0,0,0);
    return TRUE;
  }
  /* Enable the USB stack and changing to HSU_AUDIO_MODE_USB */
  hsu_analog_audio_config_phy_to_mode(HSU_AUDIO_MODE_USB);
  if (hsu_conf_sel_stack_utils_enable() == FALSE)
  {
    return FALSE;
  }

  return TRUE;
}



