#ifndef RFCOM_H
#define RFCOM_H

/*===========================================================================


      R F  D r i v e r  C o m m o n  E x p o r t e d  I n t e r f a c e
      
                            H e a d e r  F i l e

DESCRIPTION
  This file contains all exported declarations and definitions that are
  common across multiple layers of the RF driver.  It breaks the ideally
  layered driver structure, in which the upper layers are only dependent
  on the lower layers of the driver.  Although maintenance is simplified
  as it is not necessary to duplicate definitions at multiple layers of
  the driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 1999 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 - 2009  by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfcom.h#3 $ 
  $DateTime: 2009/02/10 17:16:47 $ 
  $Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/09   vb      Added RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA configurations for QSC1100
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
12/06/07   ans     Changed sequence of devices being initialized. Sequence has a dependency 
                   on OLTA parameters tables.
111/15/07  ems     Support for QSC1100
11/07/07   jfc     Added new type rfcom_rx_switchpoint_type.
10/26/07   rsr     Added support for PLATFORM_F_SMPS
09/28/07   adk     Updated RF_HW_QRF7600 to 33.
09/27/07   jfc     Change PLATFORM_H to PLATFORM_TRF6285
09/10/07   et      added PLATFORM_H for TRF6285
08/13/07   jfc/lcl Added HOT and COLD regions for GSM LNA gain range
08/02/07   avi     Added suport for QRF7600 (multimode 1x/WCDMA card)
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
06/01/07   aak     Removal of JIDZRF card support
05/21/07   ycl     Remove Platform G definition.
05/07/07   rmd     Added support for the TRF6285.
04/27/07   aak     Removed support for ZRF and JIZRF
04/27/07   hkk/ak  Reordered rf_card_band_type enums
04/13/07   ycl     Replace platform G with hw U2.
03/31/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
03/14/07   dyc     Updated QSC60X5 with DUAL or TRI band support.
03/09/07   bmg     Added RTR6500 support.
03/04/07   dyc     Added QSC60x5 support.
02/27/07   jfc     Moved FEATURE_RFM_IOCTL from rfcom.h to custrf.h. Fixed bug
                    in meta comments
02/15/07   ycl     Added support for U2.
02/15/07   sar     Added support for DZRF6285.
02/08/07   adk     Added RFT device RTR6285_1X
02/05/07   ycl     Merge to 7200.
01/23/07   lcl     Added platform F support.
01/24/07   jfc     Added RF_CAP_LDO_HAS_EXTRA_DELAY to rfcom_rf_cap_type
12/15/06   jfc     Moved structure definition rf_antenna_diff_type to rfcom.h
11/07/06   ra      Added RFCOM_GPS_MODE
10/31/06   ycl     Added for WDZRF 7600 card.
10/17/06   ycl     Modifications for initial MSM7600 build.
05/27/05   sar     Added extern definition for rf_init_state_3 for DUAL mode.
04/07/04   sar     Added extern definitions for state variables accessed in rfm_init().
12/08/03   sar     Allow RFCOM_RECEIVER_DUAL to have it's own value.
11/20/03   sar     Added state RFCOM_RECEIVER_DUAL to accomodate transitions
                   between Diversity and Dual states.
09/17/03   sar     Moved definition for rf_path_state_type to this file.
08/05/03   dbc     Added RFCOM_RECEIVER_DIV enum
12/13/02   sd      Removed temporary GSM synth tuning types (MON and PWR) for
                   non CDMA1X builds.
10/10/02   sar     Added RFCOM_1X_MODE to enum rfcom_mode_enum_type to support
                   cdma 1X mode.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "sys.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Defines devices supported by RF driver.  Note: This enum definition
  must match the corresponding  API layer device enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFCOM_TRANSCEIVER_0,             /* Primary Radio transceiver */
  RFCOM_RECEIVER_1,                /* Secondary Receiver        */
  RFCOM_RECEIVER_DIV,              /* Recieve Diversity         */ 
  RFCOM_RECEIVER_DUAL,             /* Independent Rx Chains     */
  RFCOM_MAX_DEVICES
} rfcom_device_enum_type;

/*---------------------------------------------------------------------------
  Defines different operating modes and frequency bands supported by
  RF driver.  Note: This enum definition must match the corresponding
  API layer mode enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFCOM_PARKED_MODE,     /* Parked/sleep mode */
  RFCOM_WCDMA_MODE,      /* WCDMA */
  RFCOM_GSM_MODE,        /* GSM */
  RFCOM_1X_MODE,         /* CDMA 1X Mode */
  RFCOM_GPS_MODE,        /* GPS Mode */
  RFCOM_NUM_MODES        /* Number of RF operating modes. */
} rfcom_mode_enum_type;

/*---------------------------------------------------------------------------
  Defines whether Rx, Tx, or both synthesizers should be tuned by RF
  driver.  Note: This enum definition must match the corresponding API
  layer enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFCOM_TUNE_TX,                /* Tune Tx synthesizer only */
  RFCOM_TUNE_RX,                /* Tune Rx synthesizer only */
  RFCOM_TUNE_TX_RX,             /* Tune both Tx and Rx synthesizors */
#ifdef FEATURE_CDMA1X           /* This needs to be removed from 1X also. */
  RFCOM_TUNE_GSM_MON_RX,        /* Temporary: Tune GSM Rx monitor until
                                   fixed in GSM driver. */
  RFCOM_TUNE_GSM_PWR_RX,        /* Temporary: Tune GSM Rx power until
                                   fixed in GSM driver. */
#endif /* FEATURE_CDMA1X */ 
  RFCOM_NUM_TUNES
} rfcom_synth_tune_enum_type;

/*---------------------------------------------------------------------------
  Defines data structure used for storing captured loop values and
  restoring previously captured loop values.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 tbd1;
  uint8 tbd2;
  uint16 tbd3;
} rfcom_loop_data_type;

/*---------------------------------------------------------------------------
  Generic event types passed into call back handler when call back
  handler is called during RF hardware warmup or setting times.  This
  allows call back handlers to be called at multiple stages during RF
  hardware warmup and settling times, with each call to the call back
  handler signifying a specific event.  This provides finer timing
  granularity to allow the caller to perform various actions while
  waiting for various stages of RF warmup and settling time to complete.
  Note: This enum definition must match the corresponding API layer enum
  type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFCOM_TIME_DELAY_EXPIRED,       /* RF time delay has completely expired */
  RFCOM_NUM_CB_EVENTS
} rfcom_cb_event_enum_type;


/*---------------------------------------------------------------------------
  Defines path configuration states.
---------------------------------------------------------------------------*/

typedef enum {
  RF_DISABLED_ST,
  RF_R0_ONLY_ST,
  RF_R1_ONLY_ST,
  RF_DIVERSITY_ST,
  RF_DUAL_ST
} rf_path_state_type;

extern rf_path_state_type rf_path_state;
extern int rf_init_state, rf_init_state_1, rf_init_state_2, rf_init_state_3;
extern boolean rfm_initialized;

/*---------------------------------------------------------------------------
  Generic call back handler type definition for RF driver.
---------------------------------------------------------------------------*/
typedef void (*rfcom_cb_handler_type)( rfcom_cb_event_enum_type, void* );




/*---------------------------------------------------------------------------
  Defines different operating modes and frequency bands supported by
  RF driver.  Note: This enum definition must match the corresponding
  RF common enum type definition.
---------------------------------------------------------------------------*/



typedef enum
{
  RFM_PARKED_MODE = RFCOM_PARKED_MODE, /* Parked/sleep mode */
  RFM_IMT_MODE    = RFCOM_WCDMA_MODE,  /* WCDMA */
  RFM_EGSM_MODE   = RFCOM_GSM_MODE,    /* GSM */
  RFM_1X_MODE     = RFCOM_1X_MODE,     /* 1X Mode */
  RFM_GPS_MODE    = RFCOM_GPS_MODE,    /* GPS Mode */
  RFM_NUM_MODES   = RFCOM_NUM_MODES    /* Number of RF operating modes. */
} rfm_mode_enum_type;


/*---------------------------------------------------------------------------
  Defines whether Rx, Tx, or both synthesizers should be tuned by RF
  driver.  Note: This enum definition must match the corresponding RF
  common enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFM_TUNE_TX = RFCOM_TUNE_TX,         /* Tune Tx synthesizer only */
  RFM_TUNE_RX = RFCOM_TUNE_RX,         /* Tune Rx synthesizer only */
  RFM_TUNE_TX_RX = RFCOM_TUNE_TX_RX,   /* Tune both Tx and Rx synthesizors */
#ifdef FEATURE_CDMA1X                  /* This needs to be removed from 1X. */
  RFM_TUNE_GSM_MON_RX = RFCOM_TUNE_GSM_MON_RX,
                                       /* Temporary: Tune GSM Rx monitor until
                                          fixed in GSM driver. */
  RFM_TUNE_GSM_PWR_RX = RFCOM_TUNE_GSM_PWR_RX,
                                       /* Temporary: Tune GSM Rx power until
                                          fixed in GSM driver. */
#endif /* FEATURE_CDMA1X */   
  RFM_NUM_TUNES = RFCOM_NUM_TUNES
} rfm_synth_tune_enum_type;


/*---------------------------------------------------------------------------
  Defines data structure used for storing captured loop values and
  restoring previously captured loop values.
---------------------------------------------------------------------------*/
typedef rfcom_loop_data_type rfm_loop_data_type;

/*---------------------------------------------------------------------------
  Generic event types passed into call back handler when call back
  handler is called during RF hardware warmup or setting times.  This
  allows call back handlers to be called at multiple stages during RF
  hardware warmup and settling times, with each call to the call back
  handler signifying a specific event.  This provides finer timing
  granularity to allow the caller to perform various actions while
  waiting for various stages of RF warmup and settling time to complete.
  Note: This enum definition must match the corresponding RF common enum
  type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFM_TIME_DELAY_EXPIRED = RFCOM_TIME_DELAY_EXPIRED,  /* RF time delay has
                                                         completely expired */
  RFM_NUM_CB_EVENTS
} rfm_cb_event_enum_type;

/*---------------------------------------------------------------------------
  Generic call back handler type definition for RF driver.
---------------------------------------------------------------------------*/
typedef void (*rfm_cb_handler_type)( rfm_cb_event_enum_type, void* );



/* RF band */
#if 0
typedef enum {
   RFI_CDMA_CELL_BAND = 0, /* CDMA 800 MHz */
   RFI_CDMA_PCS_BAND,      /* CDMA 1900 MHz */
   RFI_GPS_BAND,           /* 1575.42 MHz */
   RFI_IMT_BAND,           /* 1920-1980 Mhz UL and 2110-2170 Mhz DL */
   RFI_EGSM_BAND,          /* 880 - 915 Mhz UL and 925 - 960 Mhz DL */
   RFI_DCS_BAND,           /* 1710 - 1785 Mhz UL and 1805 - 1880 Mhz DL */
   RFI_AMPS_BAND,          /* analog cellular */
   RFI_NO_BAND_SEL         /* No band selected */
} rfi_band_type;
#endif

typedef enum {
  RFI_NO_BAND_SEL     = SYS_BAND_CLASS_NONE,
  
  RFI_CDMA_CELL_BAND  = SYS_BAND_BC0, 
  RFI_CDMA_PCS_BAND   = SYS_BAND_BC1,
  //RFI_CDMA_BC2_BAND   = SYS_BAND_BC2,
  RFI_CDMA_BC3_BAND   = SYS_BAND_BC3,
  RFI_CDMA_BC4_BAND   = SYS_BAND_BC4,
  RFI_CDMA_BC5_BAND   = SYS_BAND_BC5,
  RFI_CDMA_BC6_BAND   = SYS_BAND_BC6,
  RFI_CDMA_BC7_BAND   = SYS_BAND_BC7,
  RFI_CDMA_BC8_BAND   = SYS_BAND_BC8,
  RFI_CDMA_BC9_BAND   = SYS_BAND_BC9,
  RFI_CDMA_BC10_BAND  = SYS_BAND_BC10,
  RFI_CDMA_BC11_BAND  = SYS_BAND_BC11,
  RFI_CDMA_BC12_BAND  = SYS_BAND_BC12,
  //RFI_CDMA_BC13_BAND  = SYS_BAND_BC13,
  RFI_CDMA_BC14_BAND  = SYS_BAND_BC14,
  
  RFI_IMT_BAND        = SYS_BAND_WCDMA_I_IMT_2000,    
  RFI_WCDMA_1900_BAND = SYS_BAND_WCDMA_II_PCS_1900,
  RFI_WCDMA_BC3_BAND  = SYS_BAND_WCDMA_III_1700,
  RFI_WCDMA_BC4_BAND  = SYS_BAND_WCDMA_IV_1700,
  #ifdef FEATURE_WCDMA_850
#error code not present
  #else
  RFI_WCDMA_800_BAND  = SYS_BAND_WCDMA_VI_800,
  #endif
  RFI_WCDMA_BC8_BAND  = SYS_BAND_WCDMA_VIII_900,
  RFI_WCDMA_BC9_BAND  = SYS_BAND_WCDMA_IX_1700,
  
  RFI_EGSM_BAND       = SYS_BAND_GSM_EGSM_900,
  RFI_DCS_BAND        = SYS_BAND_GSM_DCS_1800,
  RFI_GSM_850_BAND    = SYS_BAND_GSM_850,
  RFI_GSM_1900_BAND   = SYS_BAND_GSM_PCS_1900,
  
  RFI_GPS_BAND
} rfi_band_type;

typedef enum {
  RF_BC0_BAND = 0,                  /*    US Cellular (800MHz )*/
  RF_BC1_BAND = 1,                  /*    US PCS (1900MHz)     */
  RF_BC2_BAND = 2,                  /*    JTACS                */
  RF_BC3_BAND = 3,                  /*    JCDMA                */
  RF_BC4_BAND = 4,                  /*    Korean PCS           */
  RF_BC5_BAND = 5,                  /*    NMT (450MHz)         */
  RF_BC6_BAND = 6,                  /*    IMT (2.1GHz)         */
  RF_BC7_BAND = 7,
  RF_BC8_BAND = 8,
  RF_BC9_BAND = 9,
  RF_BC10_BAND = 10,
  RF_BC11_BAND = 11,
  RF_BC12_BAND = 12,
  RF_BC13_BAND = 13,
  RF_BC14_BAND = 14,
  RF_BC15_BAND = 15,

  RF_GPS_BAND,                      /*    1785MHz              */
  RF_FM_BAND,                       /*    ANALOG BAND          */


  RF_BCI_BAND,                      /* WCDMA IMT 2100     */
  RF_BCII_BAND,                     /* WCDMA PCS 1900     */
  RF_BCIII_BAND,                    /* WCDMA 1700         */
  RF_BCIV_BAND,                     /* WCDMA 1700-2100    */
  RF_BCV_BAND,                      /* WCDMA 850          */
  RF_BCVI_BAND,                     /* WCDMA 800          */
  RF_BCVIII_BAND,                   /* WCDMA 900          */
  RF_BCIX_BAND,                     /* WCDMA 1700         */
  
  RF_EGSM_BAND,                     /* GSM 900             */
  RF_DCS_BAND,                      /* GSM 1800            */
  RF_GSM_850_BAND,                  /* GSM 850             */
  RF_GSM_1900_BAND,                 /* GSM 1900            */

 

  RF_MAX_BAND
} rf_card_band_type;

typedef enum {
  RF_BAND_BLOCK_0 = 0,                  /*    US Cellular (800MHz )*/
  RF_BAND_BLOCK_1 = 1,                  /*    US PCS (1900MHz)     */
  RF_BAND_MAX_BLOCKS
} rf_card_blocks_type;



///////////////////////////////////////////////////////////////////////////////



/*---------------------------------------------------------------------------
  Generic call back handler type definition for RF driver.
---------------------------------------------------------------------------*/
////typedef void (*rfcom_cb_handler_type)( rfcom_cb_event_enum_type, void* );



/* RAT system type.
 */
typedef enum
{
  RFCOM_RGS_NONE=0,
  RFCOM_RGS_CDMA_1X,
  RFCOM_RGS_CDMA_HDR,
  RFCOM_RGS_WCDMA,
  RFCOM_RGS_GSM,
  RFCOM_RGS_MAX=0xFF
} rfcom_afc_rgs_type_enum_type;

/* Recent good system data type.
 */
typedef struct
{
  rfcom_afc_rgs_type_enum_type rgs_type;
  int16 vco;
  int16 rot;
  uint8 temp;
  uint32 time;
} rfcom_afc_rgs_data_type;

/* Specify if the data retrieved is a default value, an exact table entry,
 * or an interpolated value from the table.
 */
typedef enum
{
  RFCOM_AFC_DATA_EXACT=0,
  RFCOM_AFC_DATA_INTERPOLATED,
  RFCOM_AFC_DATA_DEFAULT
} rfcom_afc_data_indicator_type;

/* Data type to specify nv items. Multiple items are specified by ORing
 * each mask together.
 */
typedef enum
{
  RFCOM_AFC_NV_VCO_TABLE_M   = 0x01,
  RFCOM_AFC_NV_RGS_DATA_M    = 0x02,
  RFCOM_AFC_NV_ALL_M         = 0xFF
} rfcom_afc_nv_item_mask_type;


/* RF Max Tx Power Limit, Control Channel */
typedef enum 
{
  RFCOM_TXPLIM_RACH = 0,    /* RACH */
  RFCOM_TXPLIM_DCH          /* DCH */
} rfcom_txplim_channel_type;

typedef enum
{
  RFCOM_SERVINGCELL_RXAGC,
  RFCOM_INGAP_RXAGC
} rfcom_rxagc_type;

typedef enum
{
  RFCOM_RXAGC_IN_DBM,
  RFCOM_RXAGC_IN_DB12
} rfcom_rxagc_unit_type;

/* Type to indicate whether to set Antenna select signals as GPIO or GRFC */
typedef enum 
{
  RFCOM_ANT_SEL_GPIO_MODE = 0,
  RFCOM_ANT_SEL_GRFC_MODE
} rfcom_ant_sel_mode_type;

typedef enum 
{
  RFCOM_BAND_IMT,
  RFCOM_BAND_1900,
  RFCOM_BAND_BC3,
  RFCOM_BAND_BC4,
  RFCOM_BAND_800,
  RFCOM_BAND_BC8,
  RFCOM_BAND_BC9,
  RFCOM_NUM_WCDMA_BANDS
} rfcom_wcdma_band_type;

typedef enum
{
  RFCOM_BAND_GSM850,
  RFCOM_BAND_GSM900,
  RFCOM_BAND_GSM1800,
  RFCOM_BAND_GSM1900,
  RFCOM_NUM_GSM_BANDS
} rfcom_gsm_band_type;

typedef enum
{
  DBG_SCR_TX_AGC,
  DBG_SCR_TX_AGC_IN_DBM,
  DBG_SCR_RX_AGC,
  DBG_SCR_GSM_RX_LEVEL,
  DBG_SCR_LNA_STATE,
  DBG_SCR_PA_RANGE,
  DBG_SCR_HDET,
  DBG_SCR_RSSI,
  DBG_SCR_BAND_WCDMA,
  DBG_SCR_BAND_GSM,
  DBG_SCR_CHANNEL,
  DBG_SCR_SUPPORTED_HW,
  DBG_SCR_NV_BAND
} rfcom_dbg_scr_param_type;

typedef union
{
  uint16 dbg_scr_tx_agc;
  int16  dbg_scr_tx_agc_in_dbm;
  int32  dbg_scr_rx_agc;
  int8   dbg_scr_gsm_rx_level;
  uint8  dbg_scr_lna_state;
  uint8  dbg_scr_pa_range;
  uint8  dbg_scr_hdet;
  int32  dbg_scr_rssi;
  uint16 dbg_scr_channel;
  uint8  dbg_scr_supported_hw;
  uint8  dbg_scr_nv_band;
} rfcom_dbg_scr_param_data_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE DBG_SCR_TX_AGC
    rfcom_dbg_scr_param_data_type.dbg_scr_tx_agc        */
/*~ CASE DBG_SCR_TX_AGC_IN_DBM
    rfcom_dbg_scr_param_data_type.dbg_scr_tx_agc_in_dbm */
/*~ CASE DBG_SCR_RX_AGC
    rfcom_dbg_scr_param_data_type.dbg_scr_rx_agc        */
/*~ CASE DBG_SCR_GSM_RX_LEVEL
    rfcom_dbg_scr_param_data_type.dbg_scr_gsm_rx_level  */
/*~ CASE DBG_SCR_LNA_STATE
    rfcom_dbg_scr_param_data_type.dbg_scr_lna_state     */
/*~ CASE DBG_SCR_PA_RANGE
    rfcom_dbg_scr_param_data_type.dbg_scr_pa_range      */
/*~ CASE DBG_SCR_HDET
    rfcom_dbg_scr_param_data_type.dbg_scr_hdet          */
/*~ CASE DBG_SCR_RSSI
    rfcom_dbg_scr_param_data_type.dbg_scr_rssi          */
/*~ CASE DBG_SCR_CHANNEL
    rfcom_dbg_scr_param_data_type.dbg_scr_channel       */
/*~ CASE DBG_SCR_SUPPORTED_HW
    rfcom_dbg_scr_param_data_type.dbg_scr_supported_hw  */
/*~ CASE DBG_SCR_NV_BAND
    rfcom_dbg_scr_param_data_type.dbg_scr_nv_band       */
/*~ DEFAULT rfcom_dbg_scr_param_data_type.void          */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef enum
{
  RFCOM_IOCTL_GET_DBG_SCR_TX_AGC          = DBG_SCR_TX_AGC,
  RFCOM_IOCTL_GET_DBG_SCR_TX_AGC_IN_DBM   = DBG_SCR_TX_AGC_IN_DBM,
  RFCOM_IOCTL_GET_DBG_SCR_RX_AGC          = DBG_SCR_RX_AGC,
  RFCOM_IOCTL_GET_DBG_SCR_GSM_RX_LEVEL    = DBG_SCR_GSM_RX_LEVEL,
  RFCOM_IOCTL_GET_DBG_SCR_LNA_STATE       = DBG_SCR_LNA_STATE,
  RFCOM_IOCTL_GET_DBG_SCR_PA_RANGE        = DBG_SCR_PA_RANGE,
  RFCOM_IOCTL_GET_DBG_SCR_HDET            = DBG_SCR_HDET,
  RFCOM_IOCTL_GET_DBG_SCR_RSSI            = DBG_SCR_RSSI,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_WCDMA      = DBG_SCR_BAND_WCDMA,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_GSM        = DBG_SCR_BAND_GSM,
  RFCOM_IOCTL_GET_DBG_SCR_CHANNEL         = DBG_SCR_CHANNEL,
  RFCOM_IOCTL_GET_DBG_SCR_SUPPORTED_HW    = DBG_SCR_SUPPORTED_HW,
  RFCOM_IOCTL_GET_DBG_SCR_NV_BAND         = DBG_SCR_NV_BAND,
  RFCOM_IOCTL_GET_DBG_SCR_RX_AGC_IN_DBM,
  RFCOM_IOCTL_GET_DBG_SCR_RATCHET_STATE,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_1X,
  RFCOM_IOCTL_GET_DBG_SCR_INTELLICEIVER_POWER_MODE,
  RFCOM_IOCTL_GET_DBG_SCR_TX_ADJ,
  RFCOM_IOCTL_GET_SLEEP_STATE,
  //RFCOM_IOCTL_SET_*
  RFCOM_IOCTL_MAX
} rfcom_ioctl_param_type;

typedef union
{
  uint8   uint8_field;
  uint16  uint16_field;
  uint32  uint32_field;
  uint64  uint64_field;
} rfcom_ioctl_pointer_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE 1
    rfcom_ioctl_pointer_type.uint8_field        */
/*~ CASE 2
    rfcom_ioctl_pointer_type.uint16_field       */
/*~ CASE 4
    rfcom_ioctl_pointer_type.uint32_field       */
/*~ CASE 8
    rfcom_ioctl_pointer_type.uint64_field       */
/*~ DEFAULT rfcom_ioctl_pointer_type.void       */
#endif /* FEATURE_HTORPC_METACOMMENTS */


typedef enum
{
  RFCOM_IOCTL_NO_ERROR          = 0,
  RFCOM_IOCTL_INVALID_REQUEST   = 1,
  RFCOM_IOCTL_INVALID_DEVICE    = 2,
  RFCOM_IOCTL_NULL_BUFFER       = 3,
  RFCOM_IOCTL_INVALID_LENGTH    = 4,
  RFCOM_IOCTL_UNKNOWN_ERROR     = 5
} rfcom_ioctl_return_type;


#ifdef FEATURE_WCDMA
#error code not present
#else
 typedef sint15                    dBx16_T;   
#endif //FEATURE_WCDMA

typedef enum
{
  RF_RX_BURST,
  RF_TX_BURST,
  RF_MON_BURST,
  RF_PWR_BURST,
  RF_WCDMA_RF_ON_BURST,
  RF_WCDMA_RF_OFF_BURST,
  RF_MAX_BURST_TYPES
} rf_burst_type;

/* Enum used to indicate when rx power is within limits of current LNA range. */
#define RFCOM_HOT_DELTA                       (4*16)  /* 4dB delta */
#define RFCOM_COLD_DELTA                      (4*16)  /* 4dB delta */

typedef enum
{
  RF_IN_RANGE,
  RF_SATURATED,
  RF_NOISY,
  RF_HOT,
  RF_COLD
} rf_measurement_T;

/* LNA gain ranges.*/
typedef enum
{
  GAIN_RANGE_1=1,
  GAIN_RANGE_2,
  GAIN_RANGE_3,
  GAIN_RANGE_4,
  GAIN_RANGE_5
} rf_gain_range_T;

typedef enum 
{ 
  RF_GSM_RX_SWITCHPOINT_1_2, 
  RF_GSM_RX_SWITCHPOINT_2_3, 
  RF_GSM_RX_SWITCHPOINT_3_4, 
  RF_GSM_RX_SWITCHPOINT_4_5, 
  RF_GSM_RX_SWITCHPOINT_MAX 
} rfcom_rx_switchpoint_type; 
 
typedef enum
{
  RFGSM_BAND_GSM850=SYS_BAND_GSM_850,      /* GSM  850  */
  RFGSM_BAND_GSM900=SYS_BAND_GSM_EGSM_900, /* GSM  900  */
  RFGSM_BAND_GSM1800=SYS_BAND_GSM_DCS_1800,/* GSM  1800 */
  RFGSM_BAND_GSM1900=SYS_BAND_GSM_PCS_1900,/* GSM  1900 */
  RF_MAX_BAND_TYPES,
  RFGSM_BAND_NONE = 0xFF
} rfgsm_band_type;

typedef struct
{
  dBx16_T         valx16;
  dBx16_T         range_high_limitx16; /* Rx pwr > this level is saturated */
  dBx16_T         range_low_limitx16;  /* Rx pwr < this level is noisy     */
  rf_gain_range_T range;            /* Needed for logging purposes only */
} rf_gain_T;

#if 0
typedef enum
{
  RF_INIT_SUCCESS,
  RF_INIT_HW_FAIL,
  RF_INIT_STATUS_MAX
} rf_init_status_type;
#endif

typedef enum
{
  RF_INIT_SUCCESS,
  RF_INIT_HW_FAIL,
  RF_INIT_INVALID_RFCAL,
  RF_INIT_INVALID_MODE,
  RF_INIT_STATUS_MAX
} rf_init_status_type;

typedef struct
{
  int16 caseA;
  int16 caseB;
  int16 caseC;
  int16 caseD;
} rf_max_tx_pwr_limit_type;

typedef enum
{ 
  RF_PATH_0,    /* Primary Rx path   */
  RF_PATH_1,    /* Rx diversity path */
  RF_PATH_NUM,
  RF_PATH_MAX=RF_PATH_NUM
} rf_path_enum_type;


//////////////////////////////RF_SHARE_H//////////////////////////////////
#if 1 //TODO
/* supported RFR devices */                
typedef enum
{
   RFR6125,
   RFR6000,
   RFR6175,
   RFR6135,
   RFR6500,
   RFR6500_DRX,
   RFR6525,
   RFR6525_DRX,
   RFR61X5,
   QSC60X5_RX,
   RTR6500_RX,
   #if defined (RF_HAS_ALTERNATE_GPS_ASIC)
#error code not present
   #endif /* RF_HAS_ALTERNATE_GPS_ASIC */
   QSC11X0_RX,
   NO_RFR
} rfr_device_enum;


/* supported RFT devices */
typedef enum {
  RFT6100,
  RFT6120,
  RFT6102,
  RFT6170,
  RFT6150,
  RTR6300,
  RTR6350,
  RTR6275_1X, //To do WDZRF 7600
  RTR6285_1X, //RF_TODO_7600_ADK
  QSC60X5_TX,
  RTR6500_TX,
  QSC11X0_TX,
  NO_RFT
} rft_device_enum;


/* supported synth devices */                
typedef enum
{
   LMX2433,
   RFT6100_SYNTH,
   RFT6120_SYNTH,
   RFT6170_SYNTH,
   RFT6150_SYNTH,
   RTR6300_SYNTH,
   RTR6275_1X_SYNTH,  //TODO_WDZRF7600
   #if defined (RF_HAS_ALTERNATE_GPS_ASIC)
#error code not present
   #endif /* RF_HAS_ALTERNATE_GPS_ASIC */
   RTR6285_1X_SYNTH,
   RFR6135_SYNTH,
   RFR6500_SYNTH,
   RFR6500_DRX_SYNTH,
   RFR6525_SYNTH,
   RFR6525_DRX_SYNTH,
   RFR61X5_SYNTH,
   RFR61X5_GPS_SYNTH,
   RTR6350_SYNTH,
   QSC60X5_RX0_SYNTH,
   QSC60X5_RX1_SYNTH,
   QSC60X5_TX_SYNTH,
   RTR6500_RX0_SYNTH,
   RTR6500_RX1_SYNTH,
   RTR6500_TX_SYNTH,
   QSC11X0_RX0_SYNTH,
   QSC11X0_TX_SYNTH,
   USER_DEFINED_SYNTH,
   NO_SYNTH
} synth_device_enum;


/* supported serial devices types */
typedef enum {
  AUX_SBI,
  SBI,
  LMX_STUFF_DATA,
  SSBI
} rf_serial_device_enum;


typedef enum {
    VCO_POSITIVE_POL,
    VCO_NEGATIVE_POL
} vco_polarity_type;


/* PLL paths for RF chain 1 */
typedef enum {
  RFT_PLL,
  EXT_SYNTH
} pll_path_type_enum_type;

typedef enum {
  GPS_MODE_TUNEAWAY,
  GPS_MODE_SGPS
} gps_mode_type;

/* Structure used to configure RF Path 0 devices */
typedef struct rf_path_0_config
{
    rfr_device_enum rfrdev;
    rft_device_enum rftdev;

    synth_device_enum rxpllsynthdev;
    synth_device_enum txpllsynthdev;
    synth_device_enum gpspllsynthdev;
    rf_serial_device_enum rft_serial_device_type;
    rf_serial_device_enum rfr_serial_device_type;

    byte rfr_sbibus;
    byte rft_sbibus;

    boolean cdma_vco_is_external;
    vco_polarity_type cdma_external_vco_polarity;

    gps_mode_type gps_mode;

} rf_path_0_config_type;


/* Structure used to configure RF Path 1 devices */
typedef struct rf_path_1_config
{
    rfr_device_enum rfrdev;
    synth_device_enum synthdev;
    #if defined (RF_HAS_ALTERNATE_GPS_ASIC)
#error code not present
    #endif /* RF_HAS_ALTERNATE_GPS_ASIC */
    rf_serial_device_enum rfr_serial_device_type;
    rf_serial_device_enum synth_serial_device_type;
    
    byte rfr_sbibus;

    boolean cdma_vco_is_external;
    vco_polarity_type cdma_external_vco_polarity;

    gps_mode_type gps_mode;

} rf_path_1_config_type;
#endif


#define  RF_PATHS_TABLE_SIZ        2


typedef enum
{
  HDET_DM_NOT_REQUIRED,
  HDET_DM_CLK_ENABLED,
  HDET_DM_CLK_DISABLED
} rfcom_dm_for_hdet_type;

typedef enum
{
  RF_CAP_DM_FOR_HDET,
  RF_CAP_LDO_HAS_EXTRA_DELAY,
  RF_CAP_TECH_DEPENDENT_LNA_SWITCH
} rfcom_rf_cap_type;

typedef enum
{
  RFCOM_REL99_MODE,
  RFCOM_HS_MODE
}rfcom_tech_type;

/* -------------------------------------------------------
** The RF Card Id used in the target
** Note: The Id needs to be sequential
** ------------------------------------------------------- */
typedef enum {
  /* 1x Only */
  RF_HW_UNDEFINED   = (uint8) 0,
  RF_HW_UNDEFINED1  = (uint8) 1,
  RF_HW_UNDEFINED2  = (uint8) 2,
  RF_HW_CZRF        = (uint8) 3,
  RF_HW_KPRF        = (uint8) 4,
  RF_HW_GZRF        = (uint8) 5,
  RF_HW_FZRF        = (uint8) 6,
  RF_HW_DZRF        = (uint8) 7,
  RF_HW_UNDEFINED3  = (uint8) 8,
  RF_HW_JTDZRF      = (uint8) 9,
  RF_HW_DZRF_SSBI   = (uint8) 10,
  RF_HW_JTDZRF_SSBI = (uint8) 11,
  RF_HW_ZRF6185     = (uint8) 12,
  RF_HW_GWZRF6250   = (uint8) 13,
  RF_HW_DGZRF6500   = (uint8) 14,
  RF_HW_DZRFBC14    = (uint8) 15,

  RF_HW_D450        = (uint8) 16,

  /* Shared Targets */
  RF_HW_WDZRF       = (uint8) 17,

  RF_HW_QSC60X5_DUAL_BAND = (uint8) 18,
  RF_HW_QSC60X5_TRI_BAND  = (uint8) 19,

  /* platform F */
  RF_HW_PLATFORM_F_US  = (uint8) 20,  /* US variant */
  RF_HW_PLATFORM_F_EU  = (uint8) 21,  /* EU variant */
  RF_HW_PLATFORM_F_JP  = (uint8) 22,  /* JP variant */

  RF_HW_U2       = (uint8) 23,
  RF_HW_DZRF6285 = RF_HW_U2,    /* 1x naming convention */

  RF_HW_TRF6500        = (uint8) 24,

  RF_HW_TRF6285        = (uint8) 25,
  RF_HW_PLATFORM_TRF6285 = RF_HW_TRF6285,  /* 25 */

  RF_HW_QRF7600        = (uint8) 33,   /* Will have associated UMTS equiavalent for MM RF card*/

  RF_HW_PLATFORM_F_US_SMPS = (uint8) 34,
  RF_HW_PLATFORM_F_EU_SMPS = (uint8) 35,
  RF_HW_PLATFORM_F_JP_SMPS = (uint8) 36,
  RF_HW_QSC11X0_DUAL_BAND = (uint8) 37,
  RF_HW_QSC11X0_TRI_BAND  = (uint8) 38,


  RF_HW_DEFAULT        = (uint8) 50,   /* Default card for 1x*/   

  RF_HW_QSC11X0_TRI_BAND_BC5_TRI_STATE_PA = (uint8) 70,

  /* Put this before UMTS legacy targets to preserve compatibility
    with 1x code */
  RF_HW_MAX,

  /* UMTS Only--Legacy Targets */
  RF_HW_3U                = 203,  /* 3U            */
  RF_HW_3U_LITE           = 204,  /* 3U Lite       */
  RF_HW_3U_BC3            = 205,  /* 3U BC3        */
  RF_HW_PLATFORM_B        = 210,  /* Platform B    */
  RF_HW_PLATFORM_B_LITE   = 211,  /* Platform B Lite */
  RF_HW_PLATFORM_E_EU     = 212,  /* Platform E EU - BC3(RTR), BC1(RFR), BC8(RFR) */
  RF_HW_PLATFORM_E_JP     = 213,  /* Platform E JP - BC9(RTR), BC1(RFR), BC5/6(RFR) */
  RF_HW_PLATFORM_E_US     = 214,  /* Platform E US - BC4(RTR), BC2(RFR), BC5/6(RFR) */
  RF_HW_PLATFORM_D        = 215,  /* placeholder */
  RF_HW_PLATFORM_D_JAPAN  = 216,  /* W2100 PRx/DRx(RFR6525), Tx RTR6275
                                          W800  PRx/DRx(RFR6525), Tx RTR6275
                                          W1700 RTR6275 */
  RF_HW_PLATFORM_D_US     = 217,  /* W1900 PRx/DRx(RFR6500), Tx RTR6275 
                                          W800  PRx/DRx(RFR6500), Tx RTR6275 
                                          W2100 RTR6275 */
  RF_HW_PLATFORM_D_EU     = 218,  /* EU variant */

  RF_HW_PLATFORM_E_JP_LITE = 219,
  RF_HW_PLATFORM_E_LITE_BC9 = 220,
  RF_HW_PLATFORM_E_US_LITE = 221,
  RF_HW_PLATFORM_E_US_BC1 = 222,

  /* The follow symbols are depecated. Use the RF_HW_ enums */
  RF_TARGET_NONE              = RF_HW_UNDEFINED,
  RF_TARGET_OFFSET            = 200, /* This is the difference between the new
                                        RF_HW_CONFIG enums and old RF_CONFIG enums
                                        for UMTS targets */
  RF_TARGET_3U                = RF_HW_3U,
  RF_TARGET_3U_LITE           = RF_HW_3U_LITE,
  RF_TARGET_3U_BC3            = RF_HW_3U_BC3,
  RF_TARGET_PLATFORM_B        = RF_HW_PLATFORM_B,
  RF_TARGET_PLATFORM_B_LITE   = RF_HW_PLATFORM_B_LITE,
  RF_TARGET_PLATFORM_E_EU     = RF_HW_PLATFORM_E_EU,
  RF_TARGET_PLATFORM_E_JP     = RF_HW_PLATFORM_E_JP,
  RF_TARGET_PLATFORM_E_US     = RF_HW_PLATFORM_E_US,
  RF_TARGET_PLATFORM_D        = RF_HW_PLATFORM_D,
  RF_TARGET_PLATFORM_D_JAPAN  = RF_HW_PLATFORM_D_JAPAN,
  RF_TARGET_PLATFORM_D_US     = RF_HW_PLATFORM_D_US,
  RF_TARGET_PLATFORM_D_EU     = RF_HW_PLATFORM_D_EU,
  RF_TARGET_PLATFORM_E_JP_LITE = RF_HW_PLATFORM_E_JP_LITE,
  RF_TARGET_PLATFORM_E_LITE_BC9 = RF_HW_PLATFORM_E_LITE_BC9,
  RF_TARGET_PLATFORM_E_US_LITE = RF_HW_PLATFORM_E_US_LITE,
  RF_TARGET_PLATFORM_E_US_BC1 = RF_HW_PLATFORM_E_US_BC1,
  RF_TARGET_PLATFORM_F_US     = RF_HW_PLATFORM_F_US,
  RF_TARGET_PLATFORM_F_EU     = RF_HW_PLATFORM_F_EU,
  RF_TARGET_PLATFORM_F_JP     = RF_HW_PLATFORM_F_JP,
  RF_TARGET_PLATFORM_TRF6285  = RF_HW_PLATFORM_TRF6285,
  RF_TARGET_PLATFORM_F_US_SMPS   = RF_HW_PLATFORM_F_US_SMPS,
  RF_TARGET_PLATFORM_F_EU_SMPS   = RF_HW_PLATFORM_F_EU_SMPS,
  RF_TARGET_PLATFORM_F_JP_SMPS   = RF_HW_PLATFORM_F_JP_SMPS 

} rf_hw_type;

typedef enum 
{
  RF_MM_CMD_SHUTDOWN = RFCOM_PARKED_MODE,
  RF_MM_CMD_CDMA     = RFCOM_1X_MODE,
  RF_MM_CMD_WCDMA    = RFCOM_WCDMA_MODE,
  RF_MM_CMD_NOP      = RFCOM_NUM_MODES,
} rf_multimode_handover_cmd_type;

typedef enum
{
  RF_ANT_QLTY_UNKNOWN,
  RF_ANT_QLTY_IDENTICAL,
  RF_ANT_QLTY_BETTER,
  RF_ANT_QLTY_WORSE
} rf_antenna_diff_type;


#endif  /* RFCOM_H */

