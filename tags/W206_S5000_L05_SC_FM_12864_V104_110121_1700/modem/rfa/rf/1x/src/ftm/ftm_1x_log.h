#ifndef FTM_1X_LOG_H
#define FTM_1X_LOG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM 1X Log

GENERAL DESCRIPTION
  This is the header file for FTM 1X Logging.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_log.h#2 $ 
  $DateTime: 2008/08/06 18:48:37 $ 
  $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/08   vm      Added support for new FTM logs for QSC1100
05/18/05   bmg     Added IntelliCeiver state to 1x and HDR logs
05/09/05   bmg     Added IntelliCeiver log
09/14/03   ra      Initial revision based on Jaguar log source file.	
===========================================================================*/

#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"

#ifdef FTM_HAS_LOGGING
/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "log_codes.h"
#include "log_dmss.h"
#include "ftm_log.h"



/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/
typedef PACKED struct
{
  log_hdr_type hdr;             /* Log header */
  uint16       ftm_log_id;      /* FTM log id */
} ftm_log_gen_type;

typedef PACKED struct
{
  int16             rx_agc;
  int16             tx_gain_ctl;
  int16             tx_gain_adj;
  int16             tx_open_loop;
  int16             tx_agc_adj;
  byte              pa_ratchet;
  byte              lna_state;
  byte              pa_state;
  byte              hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_index;
  byte              temp_remainder;
  byte              intelliceiver_state;
} ftm_agc_data_type;
typedef PACKED struct
{
  int16             rx_agc;
  int16             tx_gain_ctl;
  int16             tx_gain_adj;
  int16             tx_open_loop;
  int16             tx_agc_adj;
  byte              pa_ratchet;
  byte              lna_state;
  byte              pa_state;
  byte              hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_index;
  byte              temp_remainder;
  byte              intelliceiver_state;
  int16             max_power_limit;
  int16             beta_prime_gain;
  uint32            tx_agc_adj_32_bit;
  uint16            hdet_16_bit;  
  int16             tx_lin_vs_freq_low_gain;
  int16             tx_lin_vs_freq_high_gain;
  int16             tx_lim_vs_freq;
  uint8             pa_switch_rise_0;
  uint8             pa_switch_fall_0;
} ftm_agc_data_new_version_type;

typedef PACKED struct
{
  ftm_log_gen_type    header;
  ftm_agc_data_type data;
} ftm_log_agc_type;

typedef PACKED struct
{
  ftm_log_gen_type    header;
  ftm_agc_data_new_version_type data;
} ftm_log_agc_new_version_type;

typedef PACKED struct
{
  int16             rx_agc;
  int16             tx_gain_ctl;
  int16             tx_gain_adj;
  int16             tx_open_loop;
  int16             tx_agc_adj;
  byte              pa_ratchet;
  byte              lna_state;
  byte              pa_state;
  byte              hdet_raw;
  uint16            therm_raw;
  byte              therm_scaled;
  byte              temp_index;
  byte              temp_remainder;
  int16	            rx_agc_1;
  byte              lna_state_1;
  byte              intelliceiver_state;
} ftm_c1_agc_data_type;
typedef PACKED struct
{
  ftm_log_gen_type    header;
  ftm_c1_agc_data_type data;
} ftm_log_c1_agc_type;

typedef PACKED struct
{
  int16  rx_agc;
  byte   lna_state;
  byte   power_mode;
  uint32 reserved;
} ftm_intelliceiver_data_type;
typedef PACKED struct
{
  ftm_log_gen_type            header;
  ftm_intelliceiver_data_type data;
} ftm_log_intelliceiver_type;

typedef ftm_c1_agc_data_type ftm_1x_c1_agc_data_type;
typedef ftm_c1_agc_data_type ftm_hdr_c1_agc_data_type;
typedef ftm_agc_data_type ftm_1x_agc_data_type;
typedef ftm_agc_data_new_version_type ftm_1x_agc_data_new_version_type;
typedef ftm_agc_data_type ftm_hdr_agc_data_type;
typedef ftm_log_agc_type ftm_log_1x_agc_type;
typedef ftm_log_agc_new_version_type ftm_log_1x_agc_new_version_type;
typedef ftm_log_agc_type ftm_log_hdr_agc_type;
typedef ftm_log_c1_agc_type ftm_log_1x_c1_agc_type;
typedef ftm_log_c1_agc_type ftm_log_hdr_c1_agc_type;
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */


#ifdef FEATURE_FTM_HWTC
#error code not present
#endif

/*===========================================================================

                          FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif


#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_1X_LOG_H */

