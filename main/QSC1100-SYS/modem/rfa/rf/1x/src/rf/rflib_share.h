#ifndef RFLIB_SHARE_H
#define RFLIB_SHARE_H

/*===========================================================================

                      RF Library Header File

DESCRIPTION
  Contains the interface definitions for the functions which are exported
  out of the Library.


Copyright (c) 2004, 2005, 2006      by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rflib_share.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/07   bmg/dyc C++ compatibility fixes, tweaked SC2x enums
10/17/06   ycl     Modifications for initial MSM7600 build.
06/13/06   rmd     For the DGZRF6500, added support for the RTR6500 Synth.
04/26/06   et      GSM QUADBAND support
01/13/06   lp      Added support for SSBI
01/04/06   dyc     Lint updates.
12/01/05   zg      Naming clean up for ZRF6185.
09/15/05   dyc     Added RF_PATH_MAX to rf_path_enum_type.
09/12/05   dbc     Added support for using external or internal VCO for RF 
                   chipsets that can support either 
07/11/05   ans     Added initial support for RFR6155
07/05/05   hkk/    Added support for RFR6525
           bmg     
05/04/05   ans     Moved rflib_power_mode_type from rflib.h to rflib_share.h 
                   so that pre-MSM6500 targets can use MP library.
04/18/05   bmg     Updated rfr_I_Q_dac_set API for IntelliCeiver support
12/17/04   dbc     Created.

===========================================================================*/

#include "comdef.h"
#include "db.h"
#include "bio.h"
#include "rfifreq.h"
#include "rfcom.h"


/* type for cofiguring A/B switch in RFT chip */
typedef enum {
  RFLIB_SELECT_A,
  RFLIB_SELECT_B
} rflib_a_b_select_type;


typedef enum{
  GPIO_CLOCK_SIG,
  GPIO_DATA_SIG,
  GPIO_LATCH_SIG
}
gpio_sig_type;

typedef enum {
  RFLIB_NORMAL_POWER_MODE,
  RFLIB_LOW_POWER_MODE_1,
  RFLIB_LOW_POWER_MODE_2,
  RFLIB_MAX_POWER_MODE
} rflib_power_mode_type;

#ifdef __cplusplus
extern "C"
{
#endif
extern boolean rfr_I_Q_dac_set(rf_path_enum_type, byte*, byte*, byte*);

/*===========================================================================

FUNCTION rflib_set_sbi_info_rxf

DESCRIPTION
  Sets up the SBI Info Reg for ZIF mode of operation.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_sbi_info_rxf(rf_path_enum_type path, db_rf_mode_type rf_mode);

/*===========================================================================

FUNCTION  RFLIB_SET_PATH_0_DEVICES

DESCRIPTION
  This function is called by the upper layer to configure the devices on
  RF path 0.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean : TRUE - devices were successfully configured
            FALSE - devices were not configured 

SIDE EFFECTS
  None.
===========================================================================*/
boolean rflib_set_path_0_devices( const rf_path_0_config_type *config );

/*=========================================================================

FUNCTION    rflib_serial_device_gpio_config

DESCRIPTION
  This functions configures the selected GPIOs to be used as the aux sbi bus

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rflib_serial_device_gpio_config( rf_path_enum_type path, gpio_sig_type signal, 
                                      bio_gpio_type reg, dword mask);

/*=========================================================================

FUNCTION rflib_get_band_min_max_chan

DESCRIPTION
  This function fills in a structure (passed by caller as a pointer) with
  the minimum/maximum channels supported by the chipset driver for the 
  specified band.  If the band is not supported the minimum and maximum 
  channel is set to 0 and FALSE is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  boolean:  TRUE if band is supported
            FALSE if band is not supported  

SIDE EFFECTS
  None.

===========================================================================*/
boolean rflib_get_band_min_max_chans( rf_path_enum_type path, rf_card_band_type band, 
                                        rflib_min_max_chan_type *chptr );

/*===========================================================================

FUNCTION  RFLIB_SET_BAND_SEL_SWITCH

DESCRIPTION
  This function sets select the position of the A/B switch in the RFT (if 
  supported)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rflib_set_band_sel_switch( rf_path_enum_type path, rflib_a_b_select_type switch_pos);



/*=========================================================================

FUNCTION rftlib_is_band_chan_supported

DESCRIPTION
  This function returns a boolean indicating whether the specified channel
  is supported by the chipset driver in the specified band.  If the band 
  or channel within the band is not supported FALSE is returned; otherwise 
  TRUE is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  boolean:  TRUE   if band/channel is supported by the current devices 
                   configured in the library
            FALSE  if band/channel is not supported by the current devices 
                   configured in the library  

SIDE EFFECTS
  None.

===========================================================================*/
boolean rflib_is_band_chan_supported( rf_path_enum_type path, rf_card_band_type band, 
                                      word channel );
#ifdef __cplusplus
}
#endif


#endif /* RFLIB_SHARE_H */
