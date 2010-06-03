 #ifndef RFC_QSC1100_H
#define RFC_QSC1100_H
/*===========================================================================


                 R F C _ Q S C 6 0 5 5   H E A D E R   F I L E

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the QUALCOMM RFR6500 Rx RF to Baseband Analog Converter.

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfc_qsc1100.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
04/16/08   vm      Added support for QSC1100 Bringup
11/19/07   ems     Port From Sc2X 3044
===========================================================================*/

#include "rf_cards.h"
#include "rflib.h"

/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                              FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION    QSC11X0_INIT_CARD

DESCRIPTION
  Sets up the internal structures to control QSC11X06500.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_init_card(void *);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_PARK

DESCRIPTION
  Sets the specified path to park.
  
PARAMETERS
  RF PATH to be parked.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_park(rf_path_enum_type path);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE

DESCRIPTION
  Sets the specified path to appropriate mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode(rf_path_enum_type path, rf_card_band_type band);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_CELL

DESCRIPTION
  Sets the specified path to CELL ( Lower band) mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_cell(rf_path_enum_type path,rf_card_band_type band);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_PCS

DESCRIPTION
  Sets the specified path to PCS ( Upper band) mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_pcs(rf_path_enum_type path,rf_card_band_type band);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_AWS

DESCRIPTION
  Sets the specified path to ASW (BC15) (Upper band) mode of operation.  This mode
  is equivalent to PCS (BC1) with specific GPIO settings for tri-band operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_aws(rf_path_enum_type path);

/*===========================================================================

FUNCTION    QSC11X0_SET_MODE_GPS

DESCRIPTION
  Sets the specified path to GPS ( Upper band) mode of operation.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_mode_gps(rf_path_enum_type path);


/*===========================================================================

FUNCTION    QSC11X0_SELECT_PRIMARY_VCO

DESCRIPTION
  Selects the primary vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_select_primary_vco(void);

/*===========================================================================

FUNCTION    QSC11X0_SELECT_SECONDARY_VCO

DESCRIPTION
  Selects the secondary chain vco.
  
PARAMETERS
  Path for the vco.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_select_secondary_vco(void);

/*===========================================================================

FUNCTION    QSC11X0_ENABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Enables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_enable_secondary_chain_vco(void);
  
/*===========================================================================

FUNCTION    QSC11X0_DISABLE_SECONDARY_CHAIN_VCO

DESCRIPTION
  Disables the secondary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_disable_secondary_chain_vco(void);


/*===========================================================================

FUNCTION    QSC11X0_GET_HDET_ADC_CHANNEL

DESCRIPTION
  Returns the ADC logical channel for the HDET based upon band class
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
adc_logical_channel_type rfc_qsc11x0_get_hdet_adc_channel( rf_card_band_type );

/*=========================================================================

FUNCTION QSC11X0_MANAGE_VREGS

DESCRIPTION
  Manages the RFRX1, RFRX2, RFTX and PA voltage regulators for the RF Card
  to conform to the RF Chain State.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_manage_vregs( rfc_vregs_id_type vregs,
                        pm_switch_cmd_type cmd,
                        rf_path_enum_type path );

/*=========================================================================

FUNCTION rfc_qsc11x0_USES_EXTERNAL_VCO

DESCRIPTION
  Returns true or false indicating whether the receiver should use an
  external VCO or not.

RETURN VALUE
  True for SHDR, otherwise false.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_qsc11x0_uses_external_vco( rf_path_enum_type path,
                                rfc_vco_source_type vco_source );

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*=========================================================================

FUNCTION RFC_QSC11X0_GET_HDET_COMP_VALUE

DESCRIPTION
    This function calculates and returns the compensation value for HDET
  depending on the HDET thermitor temperature and Radio Configuration.

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
int16 rfc_qsc11x0_get_hdet_comp_value(rfc_hdet_comp_cfg_type *hdet_comp_cfg);

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/*=========================================================================

FUNCTION QSC11X0_USES_INTERNAL_HDET

DESCRIPTION
  Query function for QSC60x5 chipset platforms to determine if this
  target is wired to the internal HDET for a given band, or uses
  an external HDET for that band.

  In a mixed HDET configuration this function can return different values
  depending on the band.

RETURN VALUE
  QSC60x5 is true for all bands on the Qualcomm reference platform

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean qsc11x0_uses_internal_hdet( rf_card_band_type band );
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
/*===========================================================================

FUNCTION  RFC_QSC11X0_SET_SPECTRAL_INV

DESCRIPTION
  Sets the correct spectral inversion in MSM dependent on band
  and Device being used
  
PARAMETERS
  Device currently used and Band
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_qsc11x0_set_spectral_inv(rfcom_device_enum_type device,rf_card_band_type band);
#endif/*RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE*/

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
/*=========================================================================

FUNCTION RFC_QSC60x5_SET_PA_ACCESS_PROBE_CONFIG

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
int16 rfc_qsc11x0_set_pa_access_probe_config(rfcom_device_enum_type device,rf_card_band_type band);
#endif


#ifdef RF_HAS_SUB_BLOCK_SUPPORT

/*=========================================================================

FUNCTION RFC_QSC11X0_GET_BLOCK

DESCRIPTION
This function is called on every band or channel change
Maps the Sub block for Band/Channel Combination
Returns the Sub class Mask for given Band/Channel combination to be tuned

  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the Block mask Valid for Current Channel as read from NV

SIDE EFFECTS
  None.

===========================================================================*/ 
uint32 rfc_qsc11x0_get_block(rf_card_band_type band, word rf_chan);

/*=========================================================================
FUNCTION RFC_QSC11X0_BLOCK_MASK_IS_VALID

DESCRIPTION
This function is called during RF Cal mode to check the validity of block Mask Passed 
Returns if the given Block Mask passed during RF Cal is Valid
If RF Cal's s Block Mask is such that it asks for 2 blocks which are supported on different 
RF Paths(Different duplexer and different Tx SAW) for given Band, this function will return FALSE, 
otherwise the functon returns TRUE
  
DEPENDENCIES
  None.

RETURN VALUE
 Returns TRUE if Block AMsk is Valid otherwise returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/ 
boolean rfc_qsc11x0_block_mask_is_valid(rf_card_band_type band,uint32 subclass_mask);

/*=========================================================================
FUNCTION RFC_QSC11x0_SET_BLOCK

DESCRIPTION
Configure the GPIOs for the current Band and Block combination 
  
DEPENDENCIES
  None.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/ 
void rfc_qsc11x0_set_block(rfcom_device_enum_type device,rf_card_band_type band,uint32 block_mask);

#endif /*RF_HAS_SUB_BLOCK_SUPPORT*/

#ifdef RF_HAS_INTERNAL_THERM_SUPPORT
/*=========================================================================

FUNCTION QSC11X0_USES_INTERNAL_THERM

DESCRIPTION
  Query function for QSC11x0 chipset platforms to determine if target uses
  internal Chip Thermistor for Temperature readings


RETURN VALUE
  QSC11x0 is true for all bands on the Qualcomm reference platform

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean qsc11x0_uses_internal_therm( rf_card_band_type band );
/*=========================================================================

FUNCTION QSC11X0_READ_INTERNAL_THERM

DESCRIPTION
  Reads the internal Thermistor for QSC1100

RETURN VALUE
  Returns the 12 bit HKADC Read Value for Thermistor

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
word qsc11x0_read_internal_therm(rf_card_band_type band);
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
/*=========================================================================

FUNCTION RFC_QSC11X0_SUPPORT_TEMP_BASED_DEVICE_CONFIG

DESCRIPTION
  Returns TRUE or FALSE depending upon if Device Configration
  needs to be changed based on temperature or not for QSC1100

RETURN VALUE
  TRUE or FALSE

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_qsc11x0_support_temp_based_device_config(rf_card_band_type band);
#endif
#endif /* RFC_QSC1100_H */

