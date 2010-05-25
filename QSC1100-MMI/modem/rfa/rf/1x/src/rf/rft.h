 #ifndef RFT_H
#define RFT_H

/*=============================================================================

                      R F T    H E A D E R   F I L E

DESCRIPTION
  This file contains declarations and definitions to support interaction 
  with the QUALCOMM RFT Tx Analog Baseband-RF Converter.

Copyright (c) 1999, 2000  by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001, 2002  by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007        by QUALCOMM, Incorporated.  All Rights Reserved.
===============================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rft.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
10/12/08   vm      Added support for Different Access Probes PA Switchpoints
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
03/28/08   vm      Added the support for Tx AGC interface with MDSP for QSC1100
12/02/07   ans     Added API to return bands supported by TX chip. For now API supports
                   RTR6500 and QSC60X5.
11/25/07   ans     Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX as they should be
                   outside library.
11/13/07   ans     Modified TX comp data structure.
11/09/07   vm      Added the support for Programmable Overdrive delay
11/09/07   ans     Modified TX compensation APIs to take band as parameter
                   and removed da_bias. Compensation values are required to be 
                   independent of bias.
11/05/07   ans     Moved RF_GET_TX_LIN_MIN and RF_GET_TX_LIN_MAX to RFT file.
11/03/07   ans     Added APIs for TX compensation Algorithm.
11/03/07   ans     Updated APIs for setting TX compensation registers with Bias setting
                   as a parameter.
                   Added typedefs for TX compensation.
07/12/07   vm      Added the API for supporting Tx and PA warmup times
                   different from legacy
04/30/07   vm      Added support for New Tx Cal for RF chips with 
                   Tx Compensation registers in RFT
04/27/07   bmg     Added rft_set_tx_comp API for SC2x/RTR6500 Tx calibration
03/30/07   bmg     Added rft_get_hdet_therm() API and cleaned up comments
                     The new APIs are defined for all targets now, and have
                     reasonable defaults.
03/14/07   ans/dyc Added hdet and thermistor related API (QSC60x5)
03/09/07   bmg     Added rft_get_hdet API for RTR6500/chips with integrated
                   HDET
03/07/07   adk     Updated the function  rft_get_tx_agc_pdm_write_reg_address()
02/07/07   adk/vm  Added support for firmware TX AGC
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
12/21/05   et      LINT fixes
10/30/05   dyc     Added rft_is_valid() API
02/10/04   sar     Updated functions to support Enhanced Bands Type.
02/06/04   dbc     Changes for ZIF library architecture update
09/05/03   dbc     Chnaged prototype of rft_set_rft_device()
07/15/03   dbc     Removed gpio_sig_type and rft_aux_sbi_gpio_config()
07/10/03   dbc     Updates due to change in rf_serial_device_type 
04/14/03   dbc     Added rft_set_rft_chip() and associated enum rft_chip_type
03/13/03   dbc     Added rft_rfrx_pll_load() and rft_rftx_pll_load()
11/27/02   et      removed function rft_set_aux_sbi_flag
11/14/02   et      wrapped rtr6300 related function in RF_HAS_RTR6300
11/12/02   et      added rft_is_vco_external()
10/30/02   dbc     Wrapped A/B selection functionality in RFT_PCS_SUPPORTS_BAND_SELECTION
10/18/02   dbc     Added rft_is_band_chan_supported() and 
                   and rft_get_band_min_max_chans().  
                   Removed rft_get_out_of_range().
09/11/02    et     fixed function comment
09/10/02    et     added function rft_get_out_of_range
09/04/02   dbc     remove legacy code
08/28/02    et     removed 5600 code
08/26/02   dbc     Added rft_a_b_select_type enum and rft_set_band_sel_switch()
                   prototype for configuring the A/B switch in the RFT chip                  
05/30/02   dbc     Cleanup.
05/21/02   dbc     Added function descriptions 
05/10/02   et      added intermediate functions to connect to rft6100 functions
04/15/02   dbc     Added rft_idle_enable(), rft_idle_disable(), rft_rx_pll_enable()
                   and rft_rx_pll_disable().
03/25/02   aks     Removed more unused macros, just have exported prototypes.
03/21/02   aks     Removed unused macros.
03/08/02   dbc     Breakdown of rft_init() inot rft_init() and rft_set_mode()
11/15/01    et     changed uhf_freq to channel_or_uhf_freq
11/12/01    et     added rft6100 support
10/05/01    et     changed rf_card_band_type to rf_curr_mode in most places
10/02/01    et     removed all of the 3100 or 5600 chipset specific definitions
07/20/01    et     added API changes so that all RFT chips will have their
           own files.
06/28/01    et     added some SHIFTS and MASKS #defines that are used
           by both rft3100 and rft5600
               included rft_5600.h
05/30/01    et     added functions rft_tx_if_pll_load, rft_rx_if_pll_load, 
           rft_rf_pll_load, and rft5600_stuff_data
           added/updated macros to support the RFT5600
           added register definitions for all the RX and RF registers 
           for the RFT5600
02/28/01    tn      changed prototype for rft_pll_load.
10/18/00   fas     Copied entirely from PPG:
                   L:/src/asw/MSM3300/VCS/rft.h_v   1.3   28 Aug 2000 16:57:18   dbrackma
08/28/00   dsb     Added BSP support for RFT programming.
08/04/00   jc      Added PLL programming interface via BSP.
05/17/00   ymc     Modified register definitions according to RFT3100 device
                   specification x2;
                   Added 3 function prototypes: rft_init, rft_enable and rft_disable.
10/05/99   rv      Added fn to program MSTIC for Trimode.
05/19/99   ymc     Created.

===============================================================================*/


#include "comdef.h"
#include "rfifreq.h"
#include "rficap.h"
#include "db.h"
#include "rfr.h"
#include "rflib.h"


/*===========================================================================

                             CONSTANT DEFINITIONS

===========================================================================*/

/* Number of uint16 items in the rft_set_tx_comp parameter */

#define NUM_OF_TX_COMP_BINS 32

typedef enum 
{
    RF_DA_LOW_BIAS = 0, 
    RF_DA_HIGH_BIAS = 1,
    RF_MAX_DA_BIAS
} rft_da_bias_type;

#ifdef RF_HAS_PDM_VS_POWER_TX_CAL
typedef struct
{
  int16 value[NUM_OF_TX_COMP_BINS]; /* Unit of correction registers in the chip */
  int16 pwr_offset[NUM_OF_TX_COMP_BINS]; /* Unit of 1/100th dB */
  boolean is_calc[NUM_OF_TX_COMP_BINS];
  int16 lin_index_used[NUM_OF_TX_COMP_BINS];
} rf_tx_comp_bin_type;
#endif

/* ------------------------------------------------------------------- */
/* PROTOTYPES                                                          */
/* ------------------------------------------------------------------- */

/*=================================================================================

FUNCTION    RFT_SET_RFT_DEVICE

DESCRIPTION
  Configures the RFT device in the library

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rft_set_rft_device( int, const rf_path_0_config_type*, const rf_path_1_config_type* );



/*=================================================================================

FUNCTION    RFT_IDLE_ENABLE

DESCRIPTION
  Puts the rft into idle mode.

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rft_idle_enable( rf_path_enum_type );


/*=========================================================================

FUNCTION    RFT_IDLE_DISABLE

DESCRIPTION
  Takes the rft out of idle mode. Required for RXTX mode. 
 
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
void rft_idle_disable( rf_path_enum_type );


/*=========================================================================

FUNCTION RFT_INIT

DESCRIPTION
  This function is a generic function which gets called to initialize the 
  rft chip.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rft_init( rf_path_enum_type );


/*=========================================================================

FUNCTION RFT_SET_MODE

DESCRIPTION
  This function is a generic function which gets called to set the rft 
  mode (PCS, CELL, AMPS, GPS)

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rft_set_mode( rf_path_enum_type, rf_card_band_type band, db_rf_mode_type mode );


/*=========================================================================

FUNCTION rft_is_vco_external

DESCRIPTION
  just calls the lower layer function, which computes whether the VCO
  is internal or external.

DEPENDENCIES

RETURN VALUE
  boolean- if the VCO is external or internal

SIDE EFFECTS
  None.

===========================================================================*/
boolean rft_is_vco_external(void);


/*==============================================================================

FUNCTION RFT_IS_VALID

DESCRIPTION
  This function is called to determine if the RF device is accessible and able to 
  communicate via SBI.

DEPENDENCIES
  rft_init() must have been called prior to using this API.

RETURN VALUE
  boolean:  TRUE if SBI communication is good
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean rft_is_valid( rf_path_enum_type );

#ifdef RF_HAS_MDSP_TX_AGC

#ifdef RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE
/*==============================================================================

FUNCTION RFT_SET_TX_AGC_ADJ_VAL

DESCRIPTION
  This function sets the 32 bit PDM Value for MDSP for given PA Range.
  It reads the PA_R_MAP to determine the corresponding PA gain state
  so that corresponding 32 bit Tx AGC value can be set

Parameters:
 The index in the default Look up table for a given PA State.
 This default look up table is the origional table provided by RF Systems

DEPENDENCIES
  TX AGC LUT must have been populated.
  PA_R_MAP for a given Band Class must have been configured

RETURN VALUE
  None
  Just programs the 32 bit Tx AGC for given PA Range based on PA_R_MAP
  for given Band Class
  
SIDE EFFECTS
  None.

===========================================================================*/
void rft_set_tx_agc_adj_val(uint16 mdsp_pdm_index, byte pa_r1_r0);
/*==============================================================================

FUNCTION RFT_GET_TX_AGC_PDM_WRITE_REG_ADDRESS

DESCRIPTION
  This function returns the register addresses at which MDSP sends
  TX AGC packets to RFT. It also returns the RFT SSBI Port at which MDSP
  writes teh Tx AGC Packets

  This function also returns the SBI/SSBI port value of the RFT:

DEPENDENCIES
  None

RETURN VALUE
  
  Return value:   0,    1,     4,     5,     6,       7
  SBI/SSBI port: SBI0, SBI1, SSBI0, SSBI1, SSBI2, PMIC SSBI
  
SIDE EFFECTS
  None.

===========================================================================*/
void rft_get_tx_agc_pdm_write_reg_address (uint16 *ssbi_port, uint16 *reg1, uint16 *reg2,uint16 *reg3, uint16 *reg4);
/*==============================================================================

FUNCTION RFT_GET_TX_AGC_ADJ_32_BIT

DESCRIPTION
  This function returns the 32 bit PDM Value for MDSP for given PA state.

Parameters:
 The index in the default Look up table for a given PA State.
 This default look up table is the origional table provided by RF Systems

DEPENDENCIES
  TX AGC LUT must have been populated.

RETURN VALUE
  32 bit Tx AGC Value

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rft_get_tx_agc_adj_32_bit(byte pa_state,uint16 mdsp_pdm_index, rf_card_band_type band);
#else
/*==============================================================================

FUNCTION RFT_GET_TX_AGC_PDM_WRITE_REG_ADDRESS

DESCRIPTION
  This function returns the register addresses at which MDSP sends
  TX AGC packets to RFT. Since the TX AGC PDM value is 9 bits,
  only the upper 8 bits are written to these registers.
  (1) If the LSB is 0, then the upper 8 bits are written to *reg1.
  (2) If the LSB is 1, then the upper 8 bits are written to *reg2.

  This function also returns the SBI/SSBI port value of the RFT:
  Return value:   0,    1,     4,     5,     6,       7
  SBI/SSBI port: SBI0, SBI1, SSBI0, SSBI1, SSBI2, PMIC SSBI

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
void rft_get_tx_agc_pdm_write_reg_address (uint16 *ssbi_port, uint16 *reg1, uint16 *reg2);
#endif/*RF_SUPPORTS_32_BIT_TX_AGC_INTERFACE*/
#endif  /* RF_HAS_MDSP_TX_AGC */


/*=========================================================================

FUNCTION    RFT_GET_HDET

DESCRIPTION
  For RF transmit ICs that have internal HDET reading capability, returns
  the HDET reading.

DEPENDENCIES
  RFT should be actively transmitting. 

RETURN VALUE
  HDET reading

SIDE EFFECTS
  The internal HDET circuit of the RTR may be disabled at the time this
  function is called.  If it is, the RF driver will enable the HDET
  circuit, take the reading, and disable the circuit again.  This will
  briefly increase RTR current consumption.

===========================================================================*/
#ifdef RF_HAS_HDET_UINT16
uint16 rft_get_hdet( rf_path_enum_type path );
#else
byte rft_get_hdet( rf_path_enum_type path );
#endif

/*=========================================================================

FUNCTION    RFT_INTERNAL_HDET_ENABLE

DESCRIPTION
  Turns on the internal HDET circuit inside the transmitter.

DEPENDENCIES
  This function should do a busy wait, if there is some settling 
  time associated with it. 

RETURN VALUE
  boolean - true for success, false for a failure

SIDE EFFECTS
  Transmitter current will increase

===========================================================================*/
boolean rft_internal_hdet_enable( rf_path_enum_type path );


/*=========================================================================

FUNCTION    RFT_INTERNAL_HDET_DISABLE

DESCRIPTION
  Turns off the internal HDET circuit inside the transmitter.

DEPENDENCIES
  This function should do a busy wait, if there is some settling 
  time associated with it.

RETURN VALUE
  boolean - true for success, false for a failure

SIDE EFFECTS
  Transmitter current will decrease

===========================================================================*/
boolean rft_internal_hdet_disable( rf_path_enum_type path );


/*=========================================================================

FUNCTION    RFT_GET_HDET_THERM

DESCRIPTION
  For RF transmit ICs that have internal HDET thermistor reading capability,
  returns the thermistor reading.

DEPENDENCIES
  RFT should be actively transmitting. 

RETURN VALUE
  HDET thermistor reading

SIDE EFFECTS
  The internal HDET thermistor circuit of the RTR may be disabled at the
  time this function is called.  If it is, the RF driver will enable the
  HDET circuit, take the reading, and disable the circuit again.  This will
  briefly increase RTR current consumption.

===========================================================================*/
byte rft_get_hdet_therm( rf_path_enum_type path );


/*=========================================================================

FUNCTION    RFT_INTERNAL_HDET_THERM_ENABLE

DESCRIPTION
  Turns on the internal thermistor circuit for HDET inside the transmitter.

DEPENDENCIES
  This function should do a busy wait, if there is some settling 
  time associated with it. 

RETURN VALUE
  boolean - true for success, false for a failure

SIDE EFFECTS
  Transmitter current will increase

===========================================================================*/
boolean rft_internal_hdet_therm_enable( rf_path_enum_type path );


/*=========================================================================

FUNCTION    RFT_INTERNAL_HDET_THERM_DISABLE

DESCRIPTION
  Turns off the internal thermistor circuit for HDET inside the transmitter.

DEPENDENCIES
  This function should do a busy wait, if there is some settling 
  time associated with it. 

RETURN VALUE
  boolean - true for success, false for a failure

SIDE EFFECTS
  Transmitter current will decrease

===========================================================================*/
boolean rft_internal_hdet_therm_disable( rf_path_enum_type path );


/*=========================================================================

FUNCTION RFT_SET_TX_COMP

DESCRIPTION
  Loads the Tx compensation calibration for the current programmed band.

DEPENDENCIES
  The rft_set_mode API must be called before loading the compensation
  values.

RETURN VALUE
  None.

SIDE EFFECTS
  If the chipset supports Tx compensation the master linearizer will no longer
  be correct without corresponding compensation.

===========================================================================*/
void rft_set_tx_comp( rf_path_enum_type path, const int16 comp_values[NUM_OF_TX_COMP_BINS], rf_card_band_type band );

/*=========================================================================

FUNCTION RFT_GET_INTERNAL_OVERFLOW

DESCRIPTION
  This function returns if the Tx compensation bin has hit internal chip overflow.

DEPENDENCIES
  The rft_set_mode API must be called before calling the function to make sure that 
  chip is set to right band.

RETURN VALUE
  Zero if no internal overflow.
  If internal overflow is reached it will return a value stating how much we have 
  gone above the overflow limit.

SIDE EFFECTS
  
===========================================================================*/
int rft_get_internal_overflow(rf_path_enum_type path, int bin, int correction, rf_card_band_type band);

/*=========================================================================

FUNCTION RFT_GET_WIDTH_OVERFLOW

DESCRIPTION
  This function returns if the Tx compensation bin has hit width overflow.
  Width overflow means if correction register has reached maximum value that can be set.

DEPENDENCIES
  None.

RETURN VALUE
  Zero if no width overflow.
  If width overflow is reached it will return a value stating how much we have 
  gone above the overflow limit.

SIDE EFFECTS
  
===========================================================================*/
int rft_get_width_overflow(rf_path_enum_type path, int bin, int correction, rf_card_band_type band);

/*=========================================================================

FUNCTION RFT_IF_TX_COMP_REGS_ARE_SUPPORTED

DESCRIPTION
  This function determines if TX compensation registers exist for particular RFT chip.
  Also returns the Maximum number of Bins supported for compensation

DEPENDENCIES
  

RETURN VALUE
 Boolean depending upon whether TX compensation registers are supported or not.

SIDE EFFECTS
  
===========================================================================*/
boolean rft_if_tx_comp_regs_are_supported(byte* bins);
/*==============================================================================

FUNCTION RFT_GET_HW_RFT_DEVICE

DESCRIPTION
  This function returns the RFT device type.

DEPENDENCIES
  None

RETURN VALUE
  RFT device enum.
  
SIDE EFFECTS
  None.

===========================================================================*/
rft_device_enum rft_get_hw_rft_device(void);

/*=========================================================================

FUNCTION RFT_GET_TX_AGC_ODRIVE_DELAY

DESCRIPTION
 This function returns the Tx PDM Overderive Strength

DEPENDENCIES
  

RETURN VALUE
 

SIDE EFFECTS
  
===========================================================================*/
uint32 rft_get_tx_agc_odrive_delay(void);

#ifdef RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY
/*=========================================================================

FUNCTION RFT_GET_TX_WARMUP_TIMER_VAL

DESCRIPTION
 This function returns teh Tx Warmup timer Value in HW uints of
 153.6* tx_warmup_time(ms) + 64

DEPENDENCIES
  

RETURN VALUE
 Tx Warmup time in HW uints of
 153.6* tx_warmup_time(ms) + 64

SIDE EFFECTS
  
===========================================================================*/
word rft_get_tx_warmup_timer_val(void);
/*=========================================================================

FUNCTION RFT_GET_PA_WARMUP_TIMER_VAL

DESCRIPTION
 This function returns teh Tx Warmup timer Value in HW uints of
 153.6* pa_warmup_time(ms) + 64

DEPENDENCIES
  

RETURN VALUE
 Tx Warmup time in HW uints of
 153.6* pa_warmup_time(ms) + 64

SIDE EFFECTS
  
===========================================================================*/
word rft_get_pa_warmup_timer_val(void);
#endif /*RF_HAS_PA_AND_TX_WARMUP_TIMES_DIFF_FROM_LEGACY*/

/*=========================================================================

FUNCTION rft_is_band_supported

DESCRIPTION
  This function returns a boolean indicating whether the specified band
  is supported by the chipset driver.  If the band is not supported 
  FALSE is returned; otherwise TRUE is returned.

DEPENDENCIES
  Currently implemented only for RTR6500 and QSC60X5.
   

RETURN VALUE
  boolean:  TRUE   if band is supported by the current devices 
                   configured in the library
            FALSE  if band is not supported by the current devices 
                   configured in the library  

SIDE EFFECTS
  None.

===========================================================================*/
boolean rft_is_band_supported( rf_path_enum_type path, rf_card_band_type band);

/*=========================================================================

FUNCTION    RFT_CONFIG_DEVICE_FOR_TEMP

DESCRIPTION
  This Function Configures the RF Device for the temperature change

DEPENDENCIES
  Temperature from Thermistor should be known beforehand

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
void rft_config_device_for_temp( rf_path_enum_type path,rf_card_band_type band,word temperature );
#endif  /* RFT_H */



