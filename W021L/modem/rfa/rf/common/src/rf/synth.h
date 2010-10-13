#ifndef SYNTH_H
#define SYNTH_H
/*=================================================================================

                                 EXTERNAL SYNTHESIZER PROGRAMMING

GENERAL DESCRIPTION
  This file contains delcarations to support the LMX External PLL chips

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007             by QUALCOMM, Incorporated.  All Rights Reserved.
===================================================================================*/


/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/synth.h#1 $
  $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------------
03/13/07   jfc     Removed extra parens around feature name
03/12/07   jfc     Wrapped functions according to CDMA1x or WCDMA
10/31/06   ycl     Change return type rf_synth_gps_IF_type to unit8 for 
                   rfm_get_gps_IF_config() function.
10/17/06   ycl     Modifications for initial MSM7600 build.
03/03/06   zg      Constant name change.
12/21/05   et      LINT fixes
11/14/05   zg      Added new gps IF item for ZRF6185.
10/25/05   vm	   Added function synth_is_locked() to get the PLL lock
                   status for synth device.
04/18/05   zg      Added synth_get_gps_if_config.
01/30/05   dyc     synth_rftx_pll_load() comment update.
11/08/04   ans     Added synth_use_external_vco() function for using external VCO 
                   with synth device, when synthesizer is in RFR.
04/14/04   sar     Updated functions to support Enhanced Bands Type.
04/12/04   dbc     Changed synth_rfrx_pll_enable() to take db_rf_mode_type
02/06/04   dbc     Changes for ZIF library architecture update
09/05/03   dbc     Changed prototype of synth_set_synth_device()
08/11/03   dbc     Added new API functions to enable/disable PLLs
08/05/03   dbc     Added synth_rfrx_pll_load() and synth_rftx_pll_load()
08/04/03   dbc     Created

===================================================================================*/



/*=================================================================================

                            INCLUDE FILES FOR MODULE

===================================================================================*/
#include "rficap.h"
#include "rfcom.h"

#ifdef RF_HAS_FEATURE_CDMA1X   //TODO
#include "rflib.h"

/* Defination for GPS IF type */
typedef enum {
  RF_GPS_IF_ZERO,           /* IF = 0  */
  RF_GPS_IF_1P80K,          /* IF = 1.80 kHz */
  RF_GPS_IF_0P531K,         /* IF = 0.531 kHz */
  RF_GPS_IF_N_9P47K,        /* IF = -9.47 kHz */
  RF_GPS_IF_TYPE_MAX
} rf_synth_gps_IF_type;

typedef enum {
  RX_PLL,    
  TX_PLL,
  GPS_PLL
} synth_pll_for_synth_check;



/*=========================================================================

FUNCTION SYNTH_SET_SYNTH_PATH_1_DEVICE

DESCRIPTION
 This function configures the external synth device 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean synth_set_path_1_synth_device( const rf_path_1_config_type *); 

/*=========================================================================

FUNCTION SYNTH_SET_PATH_0_SYNTH_DEVICES

DESCRIPTION
 This function configures the synth devices on path 0

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean synth_set_path_0_synth_devices( const rf_path_0_config_type *path_0_config );

/*===========================================================================
FUNCTION: SYNTH_INIT

DESCRIPTION
  This function initializes an external synthesizer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void synth_init( rf_path_enum_type path );

/*=========================================================================

FUNCTION SYNTH_IS_LOCKED

DESCRIPTION
   This API function is called for checking the status of synth lock for specific 
   synth device.The synth device is selected on the basis of the device type passed
   as a parameter to the function and another parameter which determines whether 
   synth lock is to be checked for Rx_PLL,TX_PLL or GPS_PLL.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.
  Returns TRUE if synth is locked else return FALSE


SIDE EFFECTS
  None.


===========================================================================*/
boolean synth_is_locked( rfcom_device_enum_type device,synth_pll_for_synth_check pll_device );

/*=========================================================================

FUNCTION SYNTH_RFRX_PLL_BUFFER_ENABLE

DESCRIPTION
  API function called to enable the PLL buffer in the RFR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfrx_pll_buffer_enable( rfcom_device_enum_type device );

/*=========================================================================

FUNCTION SYNTH_RFRX_PLL_DISABLE

DESCRIPTION
  API function called to disable the RX PLL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rfrx_pll_disable( rfcom_device_enum_type device );

/*=========================================================================

FUNCTION SYNTH_RFTX_PLL_ENABLE

DESCRIPTION
  API function called to enable the TX PLL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rftx_pll_enable( rfcom_device_enum_type device );

/*=========================================================================

FUNCTION SYNTH_RFTX_PLL_DISABLE

DESCRIPTION
  API function called to disable the TX PLL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rftx_pll_disable( rfcom_device_enum_type device );

/*==============================================================================

FUNCTION SYNTH_USE_EXTERNAL_VCO

DESCRIPTION
  This function is called to configure the SYNTH DEVICE to use internal or external
  VCO

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void synth_use_external_vco( rf_path_enum_type path, boolean select_external_vco );

/*==============================================================================

FUNCTION SYNTH_GET_GPS_IF_CONFIG

DESCRIPTION
  This function is called to return GPS IF configuration so that SRCH can offset
  the corresponding offset by the rotator.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  Gps Synth IF type.

===========================================================================*/
//To do 7600
uint8 synth_get_gps_if_config( rfcom_device_enum_type device );

/*===========================================================================

DESCRIPTION SYNTH_SET_MODE
  This function sets the mode of the external synthesizer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void synth_set_mode( rf_path_enum_type path,rf_card_band_type band, db_rf_mode_type rf_mode );

/*=========================================================================

FUNCTION SYNTH_RFRX_PLL_LOAD

DESCRIPTION
  API function called to load the RX PLL for either RF_PATH_0 or RF_PATH_1

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rfrx_pll_load( rfcom_device_enum_type device,rf_card_band_type band, db_rf_mode_type rf_mode, dword channel );

/*=========================================================================

FUNCTION SYNTH_RFRX_PLL_ENABLE

DESCRIPTION
  API function called to enable the RX PLL

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rfrx_pll_enable( rfcom_device_enum_type device, db_rf_mode_type rf_mode);

/*=========================================================================

FUNCTION SYNTH_RFTX_PLL_LOAD

DESCRIPTION
  API function called to the TX PLL (RF_PATH_0 only)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void synth_rftx_pll_load(rfcom_device_enum_type device,rf_card_band_type band, db_rf_mode_type rf_mode, dword channel );

/*=========================================================================

FUNCTION CUST_SYNTH_LOAD_RFRX_COARSE_TUNE_VAL

DESCRIPTION
 This function is only needed for RFRs on RF_PATH_1 that have an internal 
 CDMA VCO that requires tuning.  The RFR will load the proper VCO coarse 
 tune code for N value passed in as a parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cust_synth_load_rfrx_coarse_tune_val( rf_card_band_type, dword );

#endif //RF_HAS_FEATURE_CDMA1X



////TODO//////////////////////////////////////////////////////////////////////

#ifdef RF_HAS_FEATURE_WCDMA
#error code not present
#endif /* RF_HAS_FEATURE_WCDMA */


#endif   /* SYNTH_H */

