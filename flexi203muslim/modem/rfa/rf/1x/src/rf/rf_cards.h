 #ifndef RF_CARDS_H
#define RF_CARDS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 R F   C A R D S   H E A D E R   F I L E  

GENERAL DESCRIPTION
    This file contains the declarations for the supported RF hw boards/cards.  


Copyright (c) 2004, 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007             by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_cards.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/08   vm      Added support for HDET Waveform Compensation
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
11/06/07   vm      Added support for getting SCH gain from Mux
10/04/07   Vish    Added support for initializing card specific RFConfig RX LNA
                   database.
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
07/16/07   hkk     Tweaks for DO Rev0 compesation
07/12/07   vm      Added new functional interface for rf cards to fix
                   the spectral inversion dependency on device
06/03/07   hkk     1x, DO and temp waveform compensation
03/31/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
03/30/07   bmg     Added rfc_uses_internal_hdet() function for RTR6500 and
                     other chips that have built-in HDET
03/14/07   rmd     Added Radio Configuration and temperature compensation for
                   HDET.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
12/08/06   jfc     Separate 1x and UMTS defs of RF_PA_DVS_VOLTAGE
10/31/06   ycl     Include the correct rficap file.
10/17/06   ycl     Modifications for initial 7600 build.
06/12/06   bmg     Added PA Thermistor VREG control to rfc_vregs_id_type enum.
06/12/06   hkk     Added support for DZRFBC14
06/06/06   hkk     Removed Support for ZRF6185.
03/30/06   sar     Added support for ZRF6185.
01/13/06   lp      Added SSBI support for DZRF6500/JTDZRF6500
11/16/05   sar     Changed type for rf_subclass_type from enum to uint32.
10/20/05   sar     Changed type for rfc_subclass_mask_tbl from word to uint32.
09/15/05   dyc     Updated rf_card_type with rfc_supports_opt_wu
07/05/05   hkk/    Added support for RFR6525
           bmg     
05/12/05   sar     Added support for Band Subclass and Antenna Attributes.
02/14/05   bmg     Added function rfc_uses_external_vco().
01/13/05   sar     Added function rfc_manage_vregs().
10/21/04   bmg     Added initial DZRF6500 support
10/20/04   sar     Removed default values for card specific nv items.
10/14/04   bmg     Added power mode support.
09/30/04   sar     Added definition for rfc_nv_type.
09/14/04   dbc     Added a new function pointer to rf_card_type structure
                   for getting the HDET ADC channel based upon band
                   class (rfc_get_hdet_adc_channel_type)
08/19/04   dbc     Added RF_HW_FZRF to rf_hw_type to represent CDMA 450 RF 
                   card (ZRF617X)
07/26/04   sar     Initial Revision. 
===========================================================================*/
#include "comdef.h"
#include "rflib.h"
#include "bio.h"
#include "tlmm.h"
#include "rficap.h"
#include "rfifreq.h"
#include "adc.h"
#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
#include "enc.h"
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#if defined(FEATURE_PMIC)
  #include "pm.h"
#endif

/* ------------------------------------------------------------------- */
/*                     C E L L  S E T T I N G S                        */
/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/*                     C E L L  S E T T I N G S                        */
/* ------------------------------------------------------------------- */
#ifdef RF_HAS_CELL_ONLY
    #define BSP_RX_SPECTRAL_INVERSION_CELL      1       /* normal RX       */
    #define BSP_TX_SPECTRAL_INVERSION_CELL      1       /* normal TX       */
#else
    #define BSP_RX_SPECTRAL_INVERSION_CELL      0       /* normal RX       */
    #define BSP_TX_SPECTRAL_INVERSION_CELL      0       /* normal TX       */
#endif

/* ------------------------------------------------------------------- */
/*           R E F E R E N C E   O S C I L L A T O R (TCXO)            */
/* ------------------------------------------------------------------- */
#define BSP_OSCILLATOR_IN   BSP_TCXO_19P2_MHZ             /* 19.2 MHz  */

/* time in usec for RF to settle after rf_tune_to_chan() returns */
#define RF_TUNE_TO_CHAN_TIME_USEC ((word)1600)
/* time in usec for a band change in rf_digital_band_select() */
#define RF_BAND_CHANGE_TIME_USEC  ((word)4000)
/* ------------------------------------------------------------------- */
/*              V O L T A G E   C O N S T A N T S                      */
/* ------------------------------------------------------------------- */
/* Voltage in mV for the PA dynamic voltage swiching voltage regulator */
#define RF_PA_DVS_VOLTAGE_1X  1750 /* mV */  //TODO

/* Microseconds to wait for RF VREG power up
*/
#define RF_VREG_RX_PWR_WAIT        200

/* Microseconds to wait for SYNTH VREG power up
*/
#define RF_VREG_SYNTH_PWR_WAIT     225


#ifdef FEATURE_RF_PA_DVS

#define RF_PM_TRANSMIT_VREGS (PM_VREG_RFTX_M | PM_VREG_PA_M )
                
#else

#define RF_PM_TRANSMIT_VREGS PM_VREG_RFTX_M

#define RF_MAX_ANTS 4

#endif /* FEATURE_RF_PA_DVS */

/*=========================================================================*/

/* Identifies the various groups RFC managed Voltage Regulators */
typedef enum {
    RFC_RX_VREGS,
    RFC_RX_DIV_VREGS,
    RFC_TX_VREGS,
    RFC_GPS_VREGS,
    RFC_PA_THERM_VREGS
}
rfc_vregs_id_type;

/* Defines the states of the RFC managed Voltage Regulators */
typedef enum {
    RFC_VREG_OFF,
    RFC_VREG_ON
}
rfc_vregs_cmd_type;

/* Different operating modes that may require special VCO configurations */
typedef enum {
  RFC_DIV_VCO,
  RFC_DUAL_RX_VCO,
  RFC_GPS_VCO
}
rfc_vco_source_type;

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
/* This enum is used to enable/disable HDET compensation
   completely or parcially (either temperature or radio 
   configuration (RC)). */
typedef enum
{
  RF_HDET_COMP_ENABLE,          /* Enable HDET Compensation. */
                                /* Both RC and TEMP */
  RF_HDET_COMP_DISABLE,         /* Disable HDET Compensation. */
                                /* Both RC and TEMP */
  RF_HDET_COMP_TEMP_ENA,        /* Only enable HDET Temperature */
                                /* Compensation. */
  RF_HDET_COMP_RC_ENA,          /* Only enable HDET RC */
                                /* Compensation. */
  RF_HDET_COMP_RRI_ENA,         /* Only enable HDET RRI */
                                /* Compensation. */
  RF_HDET_COMP_INVALID
}rf_hdet_comp_cmd_type;


/* Structure definition used to pass the parameters necessary to determine
   HDET temperature depending on the current temperature and radio 
   configuration. */
typedef struct
{
 uint8                  rc;           /* Reverse linK Fundamental Channel 
                                         Radio Configuration. */
 uint8                  r_sch_rate;   /* Reverse linK Supplemental Channel 
                                         data rate. */
 uint8                  r_fch_rate;

 boolean                dcch_on;      /* If Reverse linK DCCH (Dedicated 
                                         control Channel) is on/off. */
 boolean                r_fch_on;     /* If Reverse linK fundamental Channel 
                                         is on. */
 boolean                r_sch_on;     /* If Reverse linK supplemental channel
                                         is on.*/
 boolean                update_temp;  /* If it is time to update/read the HDET 
                                         ADC temperature value. */
 word                   r_sch_gain;    /* SCH Gain*/
 rf_card_band_type      band;         /* Current band Class. */
 rf_hdet_comp_cmd_type  cmd;          /* Enable or disable HDET compensation.*/
 int16                  test_temp;    /* used to test HDET temperature 
                                         compensation. */
}rfc_hdet_comp_cfg_type;


/* Structure definition used to pass the parameters for waveform compensation for
   HDR */
typedef struct
{
 uint8 					subtype;		/* Current subtype.  0 or 1 for Rev 0, 3 for Rev A. */

 uint8 					ps_index;       /* Packet size index (also known as RRI). 0-5 in Rev 0, 
											 0-12 in Rev A */

 boolean 				packet_is_lolat; /* Indicates that the current subframe of Rev A is using boosted LoLat 
										  parameters. Only valid for Rev A. */

 uint16 				t2p;             /* Traffic-to-pilot ratio of data channel. In 1/4 dB (Q2) units. */

 boolean                update_temp;  /* If it is time to update/read the HDET 
                                         ADC temperature value. */
 rf_card_band_type      band;         /* Current band Class. */
 rf_hdet_comp_cmd_type  cmd;          /* Enable or disable HDET compensation.*/
 int16                  test_temp;    /* used to test HDET temperature 
                                         compensation. */
}rfc_hdet_comp_cfg_for_hdr_type;

#endif /*RF_HAS_HDET_COMP_FOR_RC_AND_TEMP*/

/* -------------------------------------------------------
** Band Subclass Bit Id 
** ------------------------------------------------------- */
#define RF_SC_0 ((uint32)1<<0)
#define RF_SC_1 ((uint32)1<<1)
#define RF_SC_2 ((uint32)1<<2)
#define RF_SC_3 ((uint32)1<<3)
#define RF_SC_4 ((uint32)1<<4)
#define RF_SC_5 ((uint32)1<<5)
#define RF_SC_6 ((uint32)1<<6)
#define RF_SC_7 ((uint32)1<<7)
#define RF_SC_8 ((uint32)1<<8)
#define RF_SC_9 ((uint32)1<<9)
#define RF_SC_10 ((uint32)1<<10)
#define RF_SC_11 ((uint32)1<<11)
#define RF_SC_12 ((uint32)1<<12)
#define RF_SC_13 ((uint32)1<<13)
#define RF_SC_14 ((uint32)1<<14)
#define RF_SC_15 ((uint32)1<<15)

typedef uint32 rf_subclass_type;

/* rfc_init_card_type */
typedef void (*rfc_init_card_type)(void *);

/* rfc_set_mode_park_type */
typedef void (*rfc_set_mode_park_type)(rf_path_enum_type);

/* rfc_set_mode_type */
typedef void (*rfc_set_mode_type)(rf_path_enum_type,rf_card_band_type);

/* rfc_select_vco */
typedef void (*rfc_select_primary_vco_type)(void);

/* rfc_select_vco */
typedef void (*rfc_select_secondary_vco_type)(void);

/* rfc_rft_band_A_select_type */
typedef void (*rfc_rft_band_A_select_type)(void);

/* rfc_rft_band_B_select_type */
typedef void (*rfc_rft_band_B_select_type)(void);
  
/* rfc_enable_primary_chain_vco */
typedef void (*rfc_enable_primary_chain_vco_type)(void);
  
/* rfc_enable_secondary_chain_vco */
typedef void (*rfc_enable_secondary_chain_vco_type)(void);
  
/* rfc_disable_primary_chain_vco */
typedef void (*rfc_disable_primary_chain_vco_type)(void);
  
/*  rfc_disable_secondary_chain_vco */
typedef void (*rfc_disable_secondary_chain_vco_type)(void);

/*  rfc_get_hdet_adc_channel */
typedef adc_logical_channel_type (*rfc_get_hdet_adc_channel_type)( rf_card_band_type );

/* rfc_manage_vregs */
typedef void (*rfc_manage_vregs_type)(rfc_vregs_id_type, pm_switch_cmd_type, rf_path_enum_type);

/* rfc_select_diversity */
typedef boolean (*rfc_uses_external_vco_type)(rf_path_enum_type, rfc_vco_source_type);

#if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
typedef int16 (*rfc_get_hdet_comp_value_type)(rfc_hdet_comp_cfg_type *hdet_comp_cfg);
typedef int16 (*rfc_get_hdet_comp_value_for_hdr_type)(rfc_hdet_comp_cfg_for_hdr_type *hdet_comp_cfg_for_hdr);
#endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */

#ifdef RF_HAS_INTERNAL_HDET_SUPPORT
/* rfc_uses_internal_hdet */
typedef boolean (*rfc_uses_internal_hdet_type)( rf_card_band_type );
#endif /* RF_HAS_INTERNAL_HDET_SUPPORT */

#ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
typedef void (*rfc_uses_device_info_for_spectral_inv_type)( rfcom_device_enum_type device,rf_card_band_type band);
#endif

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
typedef int16 (*rfc_set_pa_access_probe_config_type)(rfcom_device_enum_type device,rf_card_band_type band); 
#endif

#ifdef RF_HAS_RFCONFIG_RX_LNA
typedef void (*rfc_init_rfconfig_rx_lna_type) (void);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
typedef uint32 (*rfc_get_block_type)(rf_card_band_type band, word rf_chan);
typedef boolean (*rfc_block_mask_is_valid_type)(rf_card_band_type band,uint32 subclass_mask);
typedef void (*rfc_set_block_type)(rfcom_device_enum_type device,rf_card_band_type band,uint32 subclass_mask);
#endif

#ifdef RF_HAS_INTERNAL_THERM_SUPPORT
typedef boolean (*rfc_uses_internal_therm_type)(rf_card_band_type band);   
typedef word (*rfc_read_internal_therm_type)(rf_card_band_type band);   
#endif

#ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
typedef boolean (*rfc_support_temp_based_device_config_type)(rf_card_band_type band);     
#endif
/* Antenna tunability */
typedef struct rf_ant_attrib
{
    int ant_id;
    boolean tunnable[RF_MAX_BAND];
}rf_ant_attrib_type;


/* RF Card Structure */
typedef struct rfcard
{
    
    /* RFC Data */
    rf_hw_type                              rf_hw_id;
    rf_card_band_type                       rfc_default_band;
    word                                    rfc_tune_to_chan_time_usec;
    word                                    rfc_band_change_time_usec;
    boolean                                 rfc_supports_opt_wu;
#if defined (RF_HAS_ALTERNATE_GPS_ASIC)
#error code not present
#endif /* RF_HAS_ALTERNATE_GPS_ASIC */
    /* RFC Functions */
    rfc_init_card_type                      rfc_init_card;
    rfc_set_mode_park_type                  rfc_set_mode_park;
    rfc_set_mode_type                       rfc_set_mode;
    rfc_select_primary_vco_type             rfc_select_primary_vco;
    rfc_select_secondary_vco_type           rfc_select_secondary_vco;
    rfc_rft_band_A_select_type              rfc_rft_band_A_select;
    rfc_rft_band_B_select_type              rfc_rft_band_B_select;
    rfc_enable_primary_chain_vco_type       rfc_enable_primary_chain_vco;
    rfc_enable_secondary_chain_vco_type     rfc_enable_secondary_chain_vco;
    rfc_disable_primary_chain_vco_type      rfc_disable_primary_chain_vco;
    rfc_disable_secondary_chain_vco_type    rfc_disable_secondary_chain_vco;
    rfc_get_hdet_adc_channel_type           rfc_get_hdet_adc_channel; 
    rfc_manage_vregs_type                   rfc_manage_vregs;
    rfc_uses_external_vco_type              rfc_uses_external_vco;
    #if defined(RF_HAS_HDET_COMP_FOR_RC_AND_TEMP)
    rfc_get_hdet_comp_value_type            rfc_get_hdet_comp_value;
    rfc_get_hdet_comp_value_for_hdr_type    rfc_get_hdet_comp_value_for_hdr;
    #endif /* RF_HAS_HDET_COMP_FOR_RC_AND_TEMP */
    #ifdef RF_HAS_INTERNAL_HDET_SUPPORT
    rfc_uses_internal_hdet_type             rfc_uses_internal_hdet;
    #endif /* RF_HAS_INTERNAL_HDET_SUPPORT */
    #ifdef RF_HAS_SPECTRAL_INV_BASED_ON_DEVICE
    rfc_uses_device_info_for_spectral_inv_type  rfc_uses_device_info_for_spectral_inv;
    #endif
    #ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
    rfc_set_pa_access_probe_config_type  rfc_set_pa_access_probe_config;
    #endif
    #ifdef RF_HAS_RFCONFIG_RX_LNA
    rfc_init_rfconfig_rx_lna_type           rfc_init_rfconfig_rx_lna;
    #endif /* RF_HAS_RFCONFIG_RX_LNA */
    /* Misc Attributes */
    uint8                                   rfc_max_ants;
    uint32                                  rfc_subclass_mask_tbl[RF_MAX_BAND];
    rf_ant_attrib_type                      rfc_ant_attrib[RF_MAX_ANTS];

    #ifdef RF_HAS_SUB_BLOCK_SUPPORT
    rfc_get_block_type rfc_get_block;
    rfc_block_mask_is_valid_type rfc_block_mask_is_valid;
    rfc_set_block_type rfc_set_block;
    #endif
    
    #ifdef RF_HAS_INTERNAL_THERM_SUPPORT
    rfc_uses_internal_therm_type  rfc_uses_internal_therm;
    rfc_read_internal_therm_type  rfc_read_internal_therm;   
    #endif

    #ifdef RF_HAS_TEMP_BASED_DEVICE_CONFIG
    rfc_support_temp_based_device_config_type  rfc_support_temp_based_device_config;     
    #endif

}rf_card_type;


#endif /* RF_CARDS_H */

