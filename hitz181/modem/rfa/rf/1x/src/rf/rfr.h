#ifndef RFR_H
#define RFR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 RADIO FREQUENCY RECEIVER DRIVER API HEADER FILE

GENERAL DESCRIPTION


Copyright (c) 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfr.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
09/19/08   sl      Added rfr_adjust_lo_pwc_dac_codes.
09/14/08   vm      Added support for correct Sleep Enable Sequence
04/27/07   bmg     Added rfr_blank API for GPS blanking
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
12/21/05   et      LINT fixes
10/30/05   dyc     Added rfr_is_valid() API
04/18/05   bmg     Updated rfr_I_Q_dac_set API for IntelliCeiver support
03/31/05   bmg     Removed IntelliCeiver featurization from library
12/09/04   bmg     Expanded VCO coarse tune 2 to support 16 frequency bins
11/08/04   bmg     Added extended coarse tuning parameters support
11/08/04   ans     Moved API for rfr_set_pre_lna_on_off() to rfzifapi.c.
10/29/04   ans     Added function rfr_set_pre_lna_on_off.
10/28/04   ans     Removed function rfr_load_filter_coarse_tune_val().
06/22/04   sar     Added definition for save_coarse_tune_tbl_type. Changed the 
                   prototype for rfr_do_vco_coarse_tuning() to return TRUE if
                   the table will be saved in NV, FALSE otherwise.
05/11/04   sar     Updated functions to support Enhanced Bands Type.
05/03/04   jac     Added load functions for bb filter val and iq resistor val.
04/09/04   jac     Added rfr_do_tunable_resistor_calibration prototype for RFR6135
02/06/04   dbc     Changes for ZIF library architecture update
09/05/03   dbc     Changed prototype of rfr_set_rfr_device()
05/01/03   dbc     Changed prototype of rfr_do_filter_coarse_tune()
04/03/03   dbc     Added new API function rfr_do_filter_coarse_tune()
11/11/02   dbc     Added new API function for VCO coarse tuning
09/04/02   dbc     removed legacy code
08/28/02   et      removed 5600 code
08/26/02   dbc     Remove usage of cai_**
06/17/02   et      renamed rfl_set_pll_mode to rfr_set_lna_mode
05/30/02   dbc     More cleanup.
05/23/02   et      added rfl_set_pll_mode() for support for JCDMA in the LNA
05/20/02   dbc     Cleanup.
04/05/02   dbc     Added prototype for rfr_pll_enable().
03/21/02   aks     Changed interface to fm slotting functions.
03/08/02   dbc     Breakdown of rfr_init(..) into rfr_init() and 
                   rfr_set_mode()
11/13/01    et     added a couple of prototypes
11/12/01    et     changed function names and added rfr6000 support
09/11/01   aks     Added module defines, header comment.
07/20/01   et      initial revision.  this is the header code that is generic
                   for any rfr chip

===========================================================================*/

#include "comdef.h"
#include "db.h"
#include "rficap.h"
#include "rfifreq.h"
#include "rflib.h"
#include "rfi.h"


typedef enum
{
    SAVE_FILTER_COARSE_TUNE_VAL,
    DO_NOT_SAVE_FILTER_COARSE_TUNE_VAL
} save_filter_coarse_tune_val_type;


typedef enum
{
    SAVE_COARSE_TUNE_TBL,
    DO_NOT_SAVE_COARSE_TUNE_TBL
} save_coarse_tune_tbl_type;

typedef struct
{
  uint16 table_length;
  byte *coarse_tune_table;
  byte *coarse_tune_2;
} vco_coarse_tune_params_type;

/*===================================================================================

FUNCTION RFR_SET_RFR_DEVICE

DESCRIPTION
  This function is a generic function which gets called to initialize which
  ever RFR chip is being used.
  
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfr_set_rfr_device( int devicenum, const rf_path_0_config_type*, const rf_path_1_config_type* ); 
                            


/*=========================================================================

FUNCTION rfr_init

DESCRIPTION
  This function resets the RFR and RFL to their default register settings. 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_init( rf_path_enum_type );


/*========================================================================

FUNCTION rfr_set_mode 

DESCRIPTION
  This function must be called to program the RFR and RFL to the proper
  mode (PCS, CELL, AMPS, GPS)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_set_mode( rf_path_enum_type, rf_card_band_type band, db_rf_mode_type rf_mode );

/*=========================================================================

FUNCTION rfr_sleep_disable

DESCRIPTION
  This function wakes up the RFR and RFL
  

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_sleep_disable( rf_path_enum_type );


/*==========================================================================

FUNCTION rfr_idle_disable

DESCRIPTION
  This function takes the RFR and RFL out of idle mode


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_idle_disable( rf_path_enum_type );


/*=========================================================================

FUNCTION rfr_idle_enable

DESCRIPTION
  This function puts the RFR and RFL into idle mode

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_idle_enable( rf_path_enum_type );


/*=========================================================================

FUNCTION rfr_pll_enable

DESCRIPTION
  This function enables the pll buffer in the RFR
  

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_pll_enable( rf_path_enum_type );


/*=========================================================================

FUNCTION rfr_sleep_enable

DESCRIPTION
  This function puts the RFR and RFL to sleep
  

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_sleep_enable( rf_path_enum_type );


/*=========================================================================

FUNCTION rfr_fm_slotting_enable

DESCRIPTION
  This function puts the RFR in FM slotting mode


DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_fm_slotting_enable( rf_path_enum_type );



/*=========================================================================

FUNCTION rfr_fm_slotting_disable

DESCRIPTION
  This function takes the RFR out of fm slotting mode and back into fm 
  non-slotting mode

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_fm_slotting_disable( rf_path_enum_type );



/*==============================================================================

FUNCTION RFR_DO_VCO_COARSE_TUNING

DESCRIPTION
  This function is called to perform coarse tuning of the UHF VCO (if supported)

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the table will be saved in NV, FALSE otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/
save_coarse_tune_tbl_type rfr_do_vco_coarse_tuning( rf_path_enum_type, vco_coarse_tune_params_type* );


/*=========================================================================

FUNCTION RFR_LOAD_VCO_COARSE_TUNE_TABLE

DESCRIPTION
  This function loads the coarse tune values for the internal VCO from the
  input array table_values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_load_vco_coarse_tune_table( rf_path_enum_type, vco_coarse_tune_params_type* );

/*=========================================================================

FUNCTION rfr_set_curr_band

DESCRIPTION
  This function sets the curr band for the sp[ecified path rfr device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void rfr_set_curr_band( rf_path_enum_type path, rf_card_band_type band);


/*=========================================================================

FUNCTION rfr_get_curr_band

DESCRIPTION
  This function gets the curr band for the sp[ecified path rfr device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
rf_card_band_type rfr_get_curr_band( rf_path_enum_type path);

/*==============================================================================

FUNCTION RFR_DO_FILTER_COARSE_TUNE

DESCRIPTION
  This function is called to perform the filter coarse tuning.

DEPENDENCIES
  None

RETURN VALUE
  save_filter_coarse_tune_val_type: 
  SAVE_FILTER_COARSE_TUNE_VAL : tells the caller that the value obtained is
                                good and it should be saved.  The value is 
                                contained in ctval
  DO_NOT_SAVE_FILTER_COARSE_TUNE_VAL : tells the caller that the value 
                               generated should not be saved.
  
SIDE EFFECTS
  None.

===========================================================================*/
save_filter_coarse_tune_val_type rfr_do_filter_coarse_tune( rf_path_enum_type, byte * );

/*==============================================================================

FUNCTION rfr_I_Q_dac_set

DESCRIPTION
  This function programs the I and Q channel dac.
  and the Inut transconductor value

DEPENDENCIES

RETURN VALUE
  boolean-  FALSE if the values passed in where > 6 bits
            TRUE  if the values were withing the 6 bit limit.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfr_I_Q_dac_set(rf_path_enum_type, byte*, byte*, byte*);



/*==============================================================================

FUNCTION RFR_USE_EXTERNAL_VCO

DESCRIPTION
  This function is called to configure the RFR to use internal or external
  VCO

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfr_use_external_vco(  rf_path_enum_type, boolean );

/*==============================================================================

FUNCTION RFR_DO_TUNABLE_RESISTOR_CALIBRATION

DESCRIPTION
  This function is called to perform coarse tuning of the UHF VCO (if supported)

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfr_do_tunable_resistor_calibration( rf_path_enum_type, byte* );
/*==============================================================================

FUNCTION RFR_LOAD_TUNABLE_RESISTOR_CALIBRATION_VAL

DESCRIPTION
  This function is called to load coarse the filter coarse tuning value
  from nv.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfr_load_tunable_resistor_calibration_val( rf_path_enum_type, byte );

/*==============================================================================

FUNCTION RFR_BLANK

DESCRIPTION
  This function will enable or disable analog blanking.
  While the RF is blanked the baseband output will be disconnected from the
  antenna.  This is usually done by disabling the LNA or the LNA and mixer,
  however the implementation is chip dependant.

DEPENDENCIES
  Depending on the RF chipset the blanking state may not be maintained
  across other API function calls.  The expected use case for this API
  is brief blanking during transient events within the coarse of steady
  state reception.

RETURN VALUE
  None
  
SIDE EFFECTS
  Baseband output will drop to the noise floor.

===========================================================================*/
void rfr_blank( rf_path_enum_type path, boolean blank_enable );

/*==============================================================================

FUNCTION RFR_IS_VALID

DESCRIPTION
  This function is called to determine if the RF device is assessible and able to 
  communicate via SBI.

DEPENDENCIES
  rfr_init() must have been called prior to using this API.

RETURN VALUE
  boolean:  TRUE if SBI communication is good
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rfr_is_valid(rf_path_enum_type);

/*===========================================================================

FUNCTION  rfr_enable_sleep_on_off_sequence     

DESCRIPTION
    This function is called to enable Specific SBI Sequence when 
    RF Goes to Sleep
  
DEPENDENCIES
    None

RETURN VALUE
    None

Parameters Passed

SIDE EFFECTS


===========================================================================*/
void rfr_enable_sleep_on_off_sequence(rf_path_enum_type path , boolean sleep_enable);

/*===========================================================================

FUNCTION rfr_adjust_lo_pwc_dac_codes                

DESCRIPTION
    Performs PWC DAC codes updates based on temperature.
  
DEPENDENCIES
    None

RETURN VALUE
    None

Parameters Passed
    rf_path_enum_type path = Input. The current rf path.

SIDE EFFECTS
    See description.

===========================================================================*/
void rfr_adjust_lo_pwc_dac_codes( rf_path_enum_type path );

/*===========================================================================

FUNCTION RFR_SET_INITIAL_TEMP_BASED_DEVICE_CONFIG              

DESCRIPTION
    Performs the temperature based Device Updates
  
DEPENDENCIES
    None

RETURN VALUE
    None

Parameters Passed
    rf_path_enum_type path = Input. The current rf path.

SIDE EFFECTS
    
===========================================================================*/
void rfr_set_initial_temp_based_device_config(rf_path_enum_type path,rf_card_band_type band);

#endif /* RFR_H */

