#ifndef RF_DEVICE_FUNCTIONS_H
#define RF_DEVICE_FUNCTIONS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     HDR MAXIMUM TRANSMIT POWER BACKOFF
                             HEADER FILE

GENERAL DESCRIPTION
  Initializations needed for applying Power Backoff for Maximum Transmit Power 
  for 1xEVDO RevA mode

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_device_functions.h#1 $
when       who     what, where, why
--------   ---    ----------------------------------------------------------
06/05/06   vm     Created 
===========================================================================*/

#include "comdef.h"
#include "rficap.h"
#include "rf.h"

#ifdef RF_HAS_HDR
#error code not present
#endif
/*boolean rft_pa_hysterisis_timer_access_probe_is_disabled(rfcom_device_enum_type device,rf_card_band_type band)*/
#ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
/*===========================================================================
FUNCTION RFT_HAS_PA_SWITCHPOINTS_DIFF_FROM_NV

DESCRIPTION
This function is called to determine if RFT device has PA 
Switchpoints difefrent from NV

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean rft_has_pa_switchpoints_diff_from_NV(void);
/*===========================================================================
FUNCTION RFT_GET_1X_PA_RISE_FALL_DIFF_VAL

DESCRIPTION
This function is called to get the Hardcoded minimum Power Backoffs 
between Rise and FALL needed for 4 PA Switchpoints.

DEPENDENCIES
  None

RETURN VALUE
 Valuses for Power backoffs in PA Switch point units
 Value of 5 corresponds to 2 dB Backoff

SIDE EFFECTS
  None

===========================================================================*/
void  rft_get_1X_pa_rise_fall_diff_val(int8* diff_val_ptr);
#endif

void rf_adjust_pa_thres_legacy( int2 offset );

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG

/*=========================================================================

FUNCTION RFT_GET_1X_ACCESS_PROBE_PA_BACKOFF

DESCRIPTION
This function is called so that RF Driver can program different 
PA hysterisis timerand PA switchpoints for Access Probes different 
from traffic.
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB

DEPENDENCIES
  None

RETURN VALUE
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB
===========================================================================*/
int2 rft_get_1X_access_probe_pa_backoff(rfcom_device_enum_type device,rf_card_band_type band);
#endif

#endif/*RF_DEVICE_FUNCTIONS_H*/

