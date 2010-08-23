#ifndef RF_MUX_SELECT_H
#define RF_MUX_SELECT_H

/*===========================================================================

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rf_mux_select.h#1 $
$Date: 2008/05/14 $
$Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/08   vb      Prototype for RF_SETUP_MUXES
===========================================================================*/

#include ADC_CHANNELS_H

/*===========================================================================
FUNCTION RF_SETUP_MUXES

DESCRIPTION
This function would make the proper MUX selection (on Napoleon) so that HKADC 
can read the requested RF-ADC logical channels.

This function should be called before a RF logical channel can be read by HKADC
      
DEPENDENCIES
None

  
RETURN VALUE
None

Parameters Passed
 

SIDE EFFECTS
 None
===========================================================================*/

void rf_setup_muxes( const adc_logical_channel_type rf_logical_chans);

#endif /* End of RF_MUX_SELECT_H */


