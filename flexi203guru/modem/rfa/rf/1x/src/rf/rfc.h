#ifndef RFC_H
#define RFC_H
/* EJECT   */
/*===========================================================================

                      RF Card Driver Header File

DESCRIPTION
  This header file contains all the definitions necessary in order to
  interface with the RF Card Driver.  This interface is exclusive for the RF
  unit to manipulate the RF card (hardware).

Copyright (c) 1997, 1998, 1999 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

 
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfc.h#4 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/08   vm      Added the Support for Temperature based Device
                   Configration change
10/12/08   vm      Added support for PDET Waveform Compensation
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
11/28/07   ems     Enclosed rfc_get_hdet_comp_value_for_hdr under RF_HAS_HDR
10/04/07   Vish    Added support for initializing card specific RFConfig RX LNA
                   database using rfc_init_rfconfig_rx_lna().
08/13/07   cv      Moving the definition of the rfc_hdet_therm_comp_values_type 
                   strcuture from card files to this common file.
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
06/03/07   hkk     1x, DO and temp waveform compensation
03/30/07   bmg     Added rfc_uses_internal_hdet() function for RTR6500 and
                     other chips that have built-in HDET
02/19/07   rmd     Added Radio Configuration and temperature compensation for
                   HDET.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
10/04/05   sar     Changed return type for rfc_get_sub_class() to uint32.
09/29/05   sar     Updated comments for rfc_get_sub_class.
07/04/05   dyc     Added rfc_get_sub_class() prototype.
05/12/05   sar     Added APIs to query Band Subclass and Antenna Attributes.
01/13/05   sar     Added function rfc_manage_vregs().
10/08/04   sar     Removed function rfc_copy_nv_item().
09/30/04   sar     Added function rfc_copy_nv_item(). Changed rfc_card_mode to 2-item array to manage status
                   of two individual chains.
08/16/04   dyc     Removed rfc_init_card_analog()
08/11/04   sar     Added prototypes and definitions for rfc restructuring.
07/11/03   sar     Added support for multipath chipset drivers.
10/08/02   dbc     Removed rfc_get_uhf_freq() prototype
09/04/02   dbc     Removed unused legacy code. Cleanup.
07/12/02   dyc     Remove RF_HIGH_POWER_R0R1_THRESHOLD definition. Replaced 
                   as nv item.
                   (merged from rf.01.00.13)
03/20/02   sar     Merged from MSM. Featurized RFC_TUNE_CHAN_WAIT to 5ms
02/28/02   et      mainlined GPS by eliminating non-GPS version of 
                   enum (rfc_card_mode_type)
01/24/02   aks     Add definition for Candidate Frequency Search tune time.
04/26/01   tn      reduced RFC_TUNE_CHAN_WAIT from 40msec to 10msec.
10/18/00   fas     Copied entirely from PPG:
                   L:/src/asw/MSM3300/VCS/rfc.h_v   1.10   28 Aug 2000 16:51:00   dbrackma
10/06/00   dsb     Removed TG== code.
08/28/00   dsb     Moved RF MODE macros to rfigpio.h.
08/04/00   db      Defined SURF3300 compatable RF Mode Pin macros to facilitate RF mode changes.
06/22/00   dd      PP 3.0 merge. Removed #define's for obsolete targets.
04/26/00   dd      Added prototype for rfc_init_card_gps() + various gps related definitions.
12/09/99   ymc     Renamed rf_cdma_band_type as rf_card_band_type;
                   Renamed RF_CDMA_BAND_800MHZ as RF_800_BAND;
                   Renamed RF_CDMA_BAND_1900MHZ as RF_PCS_BAND;
                   Externalized functions rfc_get_uhf_freq and rfc_get_pcs_band;
                   Externalized macros RFC_GET_CARD_MODE, RFC_GET_CARD_BAND and
                   IS_RF_CARD_INITIALIZED;
                   Moved rfc_pcs_band_type declaration from rfcs.c to rfc.h;
                   Declared rfc_card_mode_type.                  
10/26/99   ymc     Modified to support new rfcs.c file.
01/06/99   ychan   Modified for targets NGP, SURF and Q-1900.
01/06/99   ychan   Updated Copyright date.
03/24/98   kss     Adjusted RFC_THERM_60_DEG_C for the Module.
02/18/98   jjn     Added rfc_do_lna_call_back and rfc_do_125ms_isr to
                   facilitate work-arounds that are specific to WLL targets.
02/09/98   thh     Added support for the Q-1900 & Q-800.
                   Externalize rfc_retune_for_rxtx() and rfc_retune_for_rx()
                   in RF_CAP_RXTX_RETUNE scope.
02/03/98   kss     Partitioned Module RF to conform to new RF architecture
11/17/97   jjn     Partitioned ISS2 RF to conform to the new RF architecture
08/14/97   thh     Initial version.  Originally this was part of the "big"
                   RF driver.

===========================================================================*/

 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rficap.h"
#include "comdef.h"
#include "rfifreq.h"
#include "rfcom.h"
#include "rf_cards.h"
#include "db.h"         /* Database services  */

/*===========================================================================

                            PUBLIC DEFINITIONS

===========================================================================*/

#define RFC_CFS_TUNE_WAIT (5+CLK_MS_PER_TICK)

/* "Tuneable" tune time for Candidate Frequency channel changes.
    This will eventually want to be just a constant. */

#define RFC_TUNE_CHAN_WAIT  (10+CLK_MS_PER_TICK)
  /* wait 10 ms after rfc_tune_to_chan()  */

/* time in usec for RF to settle after rf_tune_to_chan() returns */
#define RF_TUNE_TO_CHAN_TIME_USEC ((word)1600)

/* -----------------------------------------------------------------------
** With the thermistor scaled +85C (0) to -30C (255), this is the value
** which corresponds to +60C.  This value cannot be computed, as the
** curve is not linear and is not a known function.  This number is
** determined by examining a quantity of thermistor data taken from
** a "representative sample" of phones.
**
** Step 1) Get the thermistor ADC reading for +60C by looking at the
**         data.
**
** Step 2) Scale this over the thermistor min/max range.
**
**     (value - therm_min) * THERM_SCALED_MAX / (therm_max - therm_min)
** ----------------------------------------------------------------------- */

#define RFC_THERM_60_DEG_C       57

/* -------------------------------------------------------------------------
** FM power level at which to switch between the high & low power HDET
** ------------------------------------------------------------------------- */

#define RF_HDET_HIGH_RANGE_THRESHOLD    5


/* -------------------------------------------------------------------------

                            DATA TYPES

---------------------------------------------------------------------------- */
#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)

/* Structure type used to store HDET Radio Configuration 
   Compensation values. */
typedef struct
{
  int16 r_sch_and_r_fch[ENC_NUM_SCH_RATES];
  int16 r_sch_and_dccch[ENC_NUM_SCH_RATES];
  int16 r_fch_only[ENC_8TH_RATE+1];
  int16 dcch_only;
}rfc_hdet_rc3_comp_values_type;

typedef struct
{
  int16 temp;
  int16 bc_comp[2];  /* This table only supports BC0 (index = 0) and
                        BC1 (index = 1). */
}rfc_hdet_therm_comp_values_type;


#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

/* Definitions for high and low PCS band */
typedef enum {RFC_PCS_HI, RFC_PCS_LO}
rfc_pcs_band_type;


/* Defines RF card modes:

    RFC_CARD_MODE_CDMA for CDMA cellular mode,
    RFC_CARD_MODE_PCS  for PCS mode,
    RFC_CARD_MODE_ANALOG for Analog cellular mode,
    RFC_CARF_MODE_GPS (PPG), and
    RFC_CARD_MODE_NONE for unspecified mode meaning that the RF card has not
                        yet been initialized.

*/
typedef enum {RFC_CARD_MODE_CDMA, RFC_CARD_MODE_PCS, RFC_CARD_MODE_ANALOG,
              RFC_CARD_MODE_GPS, RFC_CARD_MODE_NONE} 
rfc_card_mode_type;



/* -------------------------------------------------------------------------

                              LOCAL DATA

---------------------------------------------------------------------------- */

/* TRUE if rfc_init_card() has been called and the function returned successfully.
*/
extern boolean rfc_is_card_init;

/* Current mode in the RF card : RFC_CARD_MODE_CDMA, RFC_CARD_MODE_PCS or 
   RFC_CARD_MODE_ANALOG
   If it has not been configured into any mode, rfc_card_mode will hold the default
   value of RFC_CARD_MODE_NONE.
*/
extern rfc_card_mode_type rfc_card_mode[];

/* Current RF card band */
extern rf_card_band_type rfc_card_band;

/* RF Card in the system */
extern rf_card_type rf_card;

/*===========================================================================

MACRO IS_RF_CARD_INITIALIZED()

DESCRIPTION
  This macro is basically rfc_is_card_init which indicates whether
  rfc_init_card() has been called to initialize the RF card successfully.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  rfc_is_card_init

SIDE EFFECTS
  None

===========================================================================*/

#define IS_RF_CARD_INITIALIZED()  (rfc_is_card_init)

/*===========================================================================

                           FUNCTION PROTORYPES

===========================================================================*/

/*===========================================================================

FUNCTION RFC_INIT_CARD_CDMA                         EXTERNAL TO RF UNIT ONLY       

DESCRIPTION
  Initialize the RF card and its synthesizer for CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfc_init_card_cdma( rfcom_device_enum_type device );

 
/*===========================================================================

FUNCTION RFC_INIT_CARD_PCS                         EXTERNAL TO RF UNIT ONLY

DESCRIPTION
  Initialize the RF card and its synthesizer for PCS mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfc_init_card_pcs( rfcom_device_enum_type device );


#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */


/*===========================================================================

FUNCTION RFC_TUNE_SYNTH                           EXTERNAL TO RF UNIT ONLY

DESCRIPTION
  This function programs the synthesizer to tune to a specific channel.  The
  frequency of the channel is dependent on the current mode of the RF card.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern void rfc_tune_synth
(
  rfcom_device_enum_type device,
  word chan       /* Channel to tune to */
);

 
/*===========================================================================

FUNCTION RFC_INIT_CARD                            EXTERNAL TO RF UNIT ONLY

DESCRIPTION
  This function initializes the synthesizer to the current band.  It also
  performs other necessary intializations for the RF card.  And the it test-
  tunes the synthesizer to a low and high channel of the appropriate band.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the RF card could not be initialized.
  TRUE if the RF card was properly initialized.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean rfc_init_card
(
  rfcom_device_enum_type device,
  rf_card_band_type band
);

 
/*===========================================================================

FUNCTION RFC_CHAN_OFFSET                          EXTERNAL TO RF UNIT ONLY

DESCRIPTION
  Function to calculate offset from the channel with the lowest frequency.

DEPENDENCIES
  None

RETURN VALUE
  word value 0..831 representing the channel offset

SIDE EFFECTS
  None

===========================================================================*/
extern word rfc_chan_offset
(
  word channel          /* input channel number */
);
/*===========================================================================

FUNCTION    RFC_SET_CARD

DESCRIPTION
  Sets up the internal function pointers to control the specified RF HW Card.
  
PARAMETERS
  The rf hw type to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_set_card(rf_hw_type rf_hw);

/*===========================================================================

FUNCTION    RFC_SET_MODE_PARK

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
void rfc_set_mode_park(rf_path_enum_type path);

/*===========================================================================

FUNCTION    RFC_SET_MODE

DESCRIPTION
  Sets the specified path to the mode of operation based on band setting.
  
PARAMETERS
  Path to be set.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_set_mode(rf_path_enum_type path, rf_card_band_type band);

/*===========================================================================

FUNCTION    RFC_SELECT_PRIMARY

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
void rfc_select_primary_vco(void);

/*===========================================================================

FUNCTION    RFC_SELECT_SECONDARY

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
void rfc_select_secondary_vco(void);

/*===========================================================================

FUNCTION    RFC_RFT_BAND_A_SELECT

DESCRIPTION
  Selects PCS Band A for RFT.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_rft_band_A_select(void);

/*===========================================================================

FUNCTION    RFC_RFT_BAND_B_SELECT

DESCRIPTION
  Selects PCS Band B for RFT.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_rft_band_B_select(void);

/*===========================================================================

FUNCTION    RFC_ENABLE_PRIMARY_CHAIN_VCO

DESCRIPTION
  Enables the primary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_enable_primary_chain_vco(void);
  
/*===========================================================================

FUNCTION    RFC_ENABLE_SECONDARY_CHAIN_VCO

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
void rfc_enable_secondary_chain_vco(void);
  
/*===========================================================================

FUNCTION    RFC_DISABLE_PRIMARY_CHAIN_VCO

DESCRIPTION
  Disables the primary chain's vco.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_disable_primary_chain_vco(void);
  
/*===========================================================================

FUNCTION    RFC_DISABLE_SECONDARY_CHAIN_VCO

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
void rfc_disable_secondary_chain_vco(void);

/*=========================================================================

FUNCTION RFC_MANAGE_VREGS

DESCRIPTION
  Manages the RFRX1, RFRX2, RFTX and PA voltage regulators for the RF Card
  to conform to the RF Chain State.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_manage_vregs(rfc_vregs_id_type, pm_switch_cmd_type, rf_path_enum_type);

/*===========================================================================

FUNCTION RFC_IS_SUB_CLASS_SUPPORTED

DESCRIPTION
  This function takes band and subclass as parameters and returns a boolean,
  TRUE if the subclass is supported otherwise a FALSE is returned. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if the subclass is supported otherwise a FALSE is returned.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rfc_is_sub_class_supported( rf_card_band_type band, rf_subclass_type subclass );

/*===========================================================================

FUNCTION RFC_GET_SUB_CLASS

DESCRIPTION
  This function takes band parameter and and returns an associated subclass
  bitmask. For bands that do not have subclasses, LSB of the mask is set to 1.

DEPENDENCIES
  None

RETURN VALUE
  A word containing bitmask set to indicate the supported subclasses with the
  following mapping:
  
    
    BIT  :  SUBCLASS 
   -------------------
     0         0   
     1         1   
     2         2   
     3         3   
     4         4   
     5         5   
     6         6   
     7         7   
     8         8   
     9         9   
    10        10  
    11        11  
    12        12  
    13        13  
    14        14  
    15        15  
  ----------------

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 rfc_get_sub_class( rf_card_band_type band );

/*===========================================================================

FUNCTION RFC_GET_NUM_ANTENNA

DESCRIPTION
  This function does not take a parameter and returns an unsigned int representing
  the no of rf antennas supported. 

DEPENDENCIES
  None

RETURN VALUE
  Returns an unsigned int representing the no of rf antennas supported.

SIDE EFFECTS
  None
  
===========================================================================*/
uint8 rfc_get_num_antenna(void);

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/*=========================================================================

FUNCTION rfc_get_hdet_comp_value

DESCRIPTION
    This function calculates and returns the compensation value for HDET
  depending on the HDET thermitor temperature and Radio Configuration.

RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
int16 rfc_get_hdet_comp_value(rfc_hdet_comp_cfg_type *hdet_comp_config);

/*=========================================================================

FUNCTION rfc_get_hdet_comp_value_for_hdr

DESCRIPTION
  This function calculates and returns the waveform compensation value
  for HDR
   
RETURN VALUE
  HDET Compensation in 100 of a dBm uints.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

/*===========================================================================

FUNCTION RFC_IS_TUNABLE_ON_ANTENNA

DESCRIPTION
  This function takes an index to the antenna array and band and returns a boolean TRUE
  if the antenna is tunable, FALSE otherwise. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if the specified antenna is tunable, FALSE otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rfc_is_tunable_on_antenna( int antenna_index, rf_card_band_type band );

/*=========================================================================

FUNCTION RFC_GET_HDET_ADC_CHANNEL

DESCRIPTION
  This function returns the band-dependent ADC channel for HDET on the
  current RF card.

RETURN VALUE
  adc_logical_channel_type - the channel to pass to adc_read() in order
                             to get the HDET reading

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
adc_logical_channel_type rfc_get_hdet_adc_channel( rf_card_band_type band );

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/*=========================================================================

FUNCTION RFC_USES_INTERNAL_HDET

DESCRIPTION
  This function returns true if the RF card is wired to use the internal
  RF chipset power detector for a given band.  If the card uses an
  external power detector the function returns false.

RETURN VALUE
  boolean - true if the target uses internal HDET for this band

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_uses_internal_hdet( rf_card_band_type band );
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
/*=========================================================================
FUNCTION RFC_SET_PA_ACCESS_PROBE_CONFIG

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


SIDE EFFECTS
  None
===========================================================================*/
int16 rfc_set_pa_access_probe_config(rfcom_device_enum_type device,rf_card_band_type band);
#endif


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*=========================================================================
FUNCTION   RFC_INIT_RFCONFIG_RX_LNA

DESCRIPTION
   This function is used for initializing RFConfig database containing Rx LNA
   switchpoints info. Card specific implementation may initialize using Rx
   chipset specific default values, and possibly also with RF NV items
   that contain LNA switchpoint information.

   This function serves as a hook for RF driver to refresh Rx LNA RFConfig
   database whenever the driver loads RF NV items from QCN file.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void rfc_init_rfconfig_rx_lna(void);
#endif /* RF_HAS_RFCONFIG_RX_LNA */


/*=========================================================================

FUNCTION rfc_empty_func

DESCRIPTION
  Default (empty) function.  To be assigned to function pointers when the
  function pointer needs to point to a function that does nothing 
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rfc_empty_func( void* param1, ... );

/*=========================================================================

FUNCTION rfc_empty_func_return_0

DESCRIPTION
  Default (empty) function.  To be assigned to function pointers when the
  function pointer needs to point to a function that does nothing except 
  return  0.
  

DEPENDENCIES
  None.

RETURN VALUE
  0

SIDE EFFECTS
  None.

===========================================================================*/ 
int16 rfc_empty_func_return_0( void* param1, ... );


#ifdef RF_HAS_SUB_BLOCK_SUPPORT

/*=========================================================================

FUNCTION RFC_GET_BLOCK

DESCRIPTION
This function is called on every band or channel change
Maps the Sub block for Band/Channel Combination
Returns the Sub class Mask for given Band/Channel combination to be tuned
If Block Mask is forced through FTM Mode or in RF CAL it returns the forced block Mask

  
DEPENDENCIES
  None.

RETURN VALUE
  Returns the Block mask Valid for Current Channel as read from NV

SIDE EFFECTS
  None.

===========================================================================*/ 
uint32 rfc_get_block(rf_card_band_type band, word rf_chan);

/*=========================================================================
FUNCTION RFC_BLOCK_MASK_IS_VALID

DESCRIPTION
This function is called during RF Cal mode to check the validity of block Mask Passed 
Returns if the given Block Mask passed during RF Cal is Valid
If RF Cal's s Block Mask is such that it asks for 2 blocks which are supported on different 
RF Paths(Different duplexer and different Tx SAW) for given Band,this function will return FALSE, 
otherwise the functon returns TRUE
  
DEPENDENCIES
  None.

RETURN VALUE
 Returns TRUE if Block AMsk is Valid otherwise returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/ 
boolean rfc_block_mask_is_valid(rf_card_band_type band,uint32 subclass_mask);

/*=========================================================================
FUNCTION RFC_SET_BLOCK

DESCRIPTION
Configure the GPIOs for the current Band and Block combination 
  
DEPENDENCIES
  None.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/ 
void rfc_set_block(rfcom_device_enum_type device,rf_card_band_type band,uint32 subclass_mask);

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

#ifdef RF_HAS_INTERNAL_THERM_SUPPORT
/*=========================================================================

FUNCTION RFC_USES_INTERNAL_THERM

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
boolean rfc_uses_internal_therm( rf_card_band_type band );

/*=========================================================================

FUNCTION RFC_READ_INTERNAL_THERM

DESCRIPTION
  Reads the internal Thermistor for QSC1100

RETURN VALUE
  Returns the 12 bit HKADC Read Value for Thermistor

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
word rfc_read_internal_therm(rf_card_band_type band);
#endif
#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
/*=========================================================================

FUNCTION RFC_SUPPORT_TEMP_BASED_DEVICE_CONFIG

DESCRIPTION
  Returns TRUE or FALSE depending upon if Device Configration
  needs to be changed based on temperature or not

RETURN VALUE
  TRUE or FALSE

PARAMETERS
  band - the band under consideration

SIDE EFFECTS
  None.

===========================================================================*/
boolean rfc_support_temp_based_device_config(rf_card_band_type band);
#endif
#endif /* RFC_H */

