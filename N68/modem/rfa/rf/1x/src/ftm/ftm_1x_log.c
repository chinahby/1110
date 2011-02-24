/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM 1X Log

GENERAL DESCRIPTION
  This file supports FTM 1X Logging.

Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_log.c#5 $
  $DateTime: 2008/11/19 11:46:56 $
  $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/12/08   lp      Fixed more Klocwork errors
10/09/08   sr      Klocwork Fixes
08/25/08   sr      Fixed Lint errors
08/06/08   vm      Added support for new FTM Logs for QSC1100
06/13/08   vm      Added support for 16 bit PDET in QSC1100
02/26/07   vm      Merge for Common archive
01/31/06   bmg     Fixed includes based on features
11/22/06   vm      Added the support for Firmware TX AGC
07/05/06   lp      Fixed memory leak bug. When enabling FTM_LOG_1X_AGC and
                   FTM_LOG_1X_AGC_C1, FTM_LOG_HDR_AGC_C1 logs, memory is not 
                   deallocated
11/17/05   bmg     Added ftm_log_1x_agc() to generate on-demand dual chain
                   AGC logs at any point in AMSS
11/13/05   dyc     Added rf_intelliceiver.h
05/12/05   bmg     Added IntelliCeiver state to 1x and HDR logs
                   Fixed typo for temp index and remainder in 1x and HDR logs
05/10/05   bmg     Skip IntelliCeiver logs when phone is in sleep
05/09/05   bmg     Added IntelliCeiver log
03/07/05   bmg     Made log ID param of ftm_log_malloc() uint16 so log codes
                   larger than 0xFF can always be used.
03/02/05   ra      fixed bug in DORA support
02/17/05   ra      Only declare clk_regime_is_on prototype for non MSM6700 
                   or MSM6800 targets
01/24/05   ra      DORA merge.
10/12/04   jac     Mainlined ASYNC packet code.
10/12/04   ra      The finger driver was mainlined so changed logic to 
                   disable until we can synch up ftm log with finger driver
09/17/04   bmg     Featurized FTM modem log with finger driver
09/17/04   bmg     Switched HDR AGC logging to use rf_hdet_data global, now
                   that HDR supports HDET.
09/14/03   ra      Initial revision based on Jaguar log source file.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "AEEstd.h"
#include "ftm_task.h"

#ifdef FTM_HAS_LOGGING
#include "comdef.h"
#include "err.h"
#include "msm.h"
#include "msm_help.h"

#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "ctype.h"

#include "ftm_log.h"
#include "cagc.h"
#include "adc.h"
#include "therm.h"
#include "rf_intelliceiver.h"
#include "rficap.h"

#ifdef RF_HAS_MDSP_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif

#ifdef RF_HAS_GPS_CLK_CHECK
#include "clkregim.h"
#if !defined(T_MSM6700) || !defined(T_MSM6800)
extern boolean clk_regime_is_on( clk_regime_type regime); //needs to be added in clkregime
#endif
#endif

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/
extern uint16  rf_hdet_data;        /* last raw reading of HDET in CDMA mode */
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#define MAX_BUFFERS 100

ftm_log_config_type *ftm_log_array_ptr=NULL;
ftm_1x_agc_data_new_version_type* agc_data_new_version = NULL;
ftm_1x_agc_data_type *agc_data=NULL;
ftm_1x_c1_agc_data_type *agc_data_c1=NULL;
byte ftm_rf_temp_index;
byte ftm_rf_temp_remainder;
extern int2 rf_tx_pwr_limit;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
void ftm_log_printf(const char *fmt, ... );
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

void ftm_log_1x_agc_cb(int4 interval);
void ftm_log_1x_agc_c1_cb(int4 interval);
void ftm_log_intelliceiver_cb(int4 interval);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION FTM_LOG_INIT

DESCRIPTION
   Creates the ftm_log_array, and populates it with the supported entries.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_log_init()
{

  int i;

  /* Allocate and initialize log array structure */
  ftm_log_array_ptr = (ftm_log_config_type*)ftm_malloc(FTM_LOG_MAX * sizeof(ftm_log_config_type));

  if (ftm_log_array_ptr == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate ftm log memory",0,0,0);   
    return;
  }

  for(i=0;i<FTM_LOG_MAX;i++)
  {
    ftm_log_array_ptr[i].cb_struct = NULL;
    ftm_log_array_ptr[i].cb_func   = NULL;
    ftm_log_array_ptr[i].active    = FALSE;
  }

  /* Added callbacks for supported log types */
  ftm_log_array_ptr[FTM_LOG_1X_AGC].cb_func = &ftm_log_1x_agc_cb;
  ftm_log_array_ptr[FTM_LOG_1X_AGC_C1].cb_func = &ftm_log_1x_agc_c1_cb;
  
  ftm_log_array_ptr[FTM_LOG_INTELLICEIVER].cb_func = &ftm_log_intelliceiver_cb;

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
}

/*===========================================================================

FUNCTION FTM_LOG_MALLOC

DESCRIPTION
   This function allocates memory for an ftm log pkt.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
PACKED void * ftm_log_malloc (uint16 ftm_log_id, unsigned int length)
{
  log_ftm_gen_type *ftm_log_pkt = NULL;

  if (log_status( LOG_FTM_VER_2_C ))
  {
    ftm_log_pkt = (log_ftm_gen_type*) log_alloc( LOG_FTM_VER_2_C, length);


    if (ftm_log_pkt != NULL)
    {
      ftm_log_pkt->ftm_log_id = ftm_log_id;
    }
    else
    {
      FTM_MSG_ERROR("Unable to allocate ftm log memory",0,0,0);
    }
  }

  return ((void*)ftm_log_pkt);
} /* End ftm_log_malloc */

/*===========================================================================

FUNCTION FTM_LOG_GET_DATA_PTR

DESCRIPTION
   This function returns a ptr to the data portion of a given ftm log pkt.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
PACKED void * ftm_log_get_data_ptr (void *log_pkt)
{
  log_ftm_gen_type *ftm_log_pkt = NULL;

  ftm_log_pkt = (log_ftm_gen_type*) log_pkt;

  return ((void*)ftm_log_pkt->data);
} /* End ftm_log_get_data_ptr */

/*===========================================================================

FUNCTION FTM_LOG_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but
  will output logged to the dm monitor.


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_log_printf(const char *fmt, ... )
{
  char             *local_printf_buffer = NULL;
  AEEVaList        arg_ptr;
  log_ftm_gen_type *ftm_log_pkt = NULL;


  /* check to make sure we should log first */
  if (!log_status( LOG_FTM_VER_2_C ))
  {
    return;
  }

  /* Allocate memory for local buffer with extra space because we don't
     yet know how big the string will be once the string formatting is
     done
  */
  local_printf_buffer = (char*) ftm_malloc(strlen(fmt) + 100);

  /* check that we haev memory to work with */
  if (local_printf_buffer == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate memory for printf log",0,0,0);
    return;
  }

  /* first lets format the and copy the string. */
  //va_start( arg_ptr, fmt );
  AEEVA_START( arg_ptr, fmt );
  //vsprintf(local_printf_buffer, fmt, arg_ptr);
  std_vstrlprintf (local_printf_buffer, strlen (local_printf_buffer), fmt, arg_ptr);
  AEEVA_END( arg_ptr );

  /* now that we know the size, allocate the log memory we need */
  ftm_log_pkt = (log_ftm_gen_type *) ftm_log_malloc (FTM_LOG_PRINTF, FTM_LOG_HEADER_SIZE +
                                                strlen (local_printf_buffer) +
                                                1 /* null char */);

  /* check that we haev memory to work with */
  if (ftm_log_pkt == NULL)
  {
    return;
  }

  /* copy the string into the log pkt */
  memcpy((void*)ftm_log_pkt->data, (void*)local_printf_buffer, strlen (local_printf_buffer));

  /* send log */
  log_commit( (log_type*) ftm_log_pkt );

  /* clean up memeory */
  ftm_free ((void*)local_printf_buffer);

} /* End ftm_log_printf */


/*===========================================================================

FUNCTION FTM_LOG_1X_AGC_CB

DESCRIPTION
   This function reads the registers pertaining to the RX front status
   and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_1x_agc_cb(int4 interval)
{
  static int buf = 0;
  ftm_log_1x_agc_new_version_type *agc_log  = NULL;

  #ifdef RF_HAS_MDSP_TX_AGC
  byte tx_min_status;
  byte tx_max_status;
  #endif

  if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
  {
    agc_data_new_version[buf].rx_agc = (int)HWIO_INI(RX_AGCc_1X_VALUE_RD, 0) << 22 >> 22;
  }
  else
    agc_data_new_version[buf].rx_agc = -513;

  /* strobe the CAGC uP_READ_SYNC bit to capture the tx_gain_ctl data
  */
  /* Latch the AGC read register values */
  
  #ifndef RF_HAS_MDSP_TX_AGC  
  HWIO_OUT(TX_GAIN_CTL_LATCH,0);
  agc_data_new_version[buf].tx_gain_ctl  = (int)HWIO_IN(TX_GAIN_CTL_RD) << 22 >> 22;

  agc_data_new_version[buf].tx_gain_adj  = (int)HWIO_IN(TX_GAIN_ADJ_RD) << 23 >> 23;
  agc_data_new_version[buf].tx_open_loop = (int)HWIO_IN(TX_OPEN_LOOP_RD) << 22>> 22;
  agc_data_new_version[buf].tx_agc_adj   = HWIO_IN(TX_AGC_ADJ_RD);
  agc_data_new_version[buf].pa_ratchet   = HWIO_IN(RATCHET_BIT_DIS);
  #else
  agc_data_new_version[buf].tx_gain_ctl  =  (int)(CDMA_MDSP_RD_TRANSMIT_GAIN()) << 16 >> 22;
  agc_data_new_version[buf].tx_gain_adj =   (int)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) << 16 >> 23;
  agc_data_new_version[buf].tx_open_loop =  (int)(CDMA_MDSP_RD_TX_OPEN_LOOP_VAL()) << 16 >> 22 ;
  
  tx_min_status = ((~(byte)(CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS())) & 0x1);
  tx_max_status = ((~(byte)(CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS())) & 0x1);
  agc_data_new_version[buf].pa_ratchet   = ( tx_min_status << 1) & tx_max_status;
  agc_data_new_version[buf].tx_agc_adj   = (int2)(((int2)(CDMA_MDSP_GET_TX_AGC_ADJ_VAL())) >> 7) + 256;
  #endif
 
  if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
  {
    agc_data_new_version[buf].lna_state    = MSM_R_MASKED_i(RX_AGCc_LNA_RANGE_RD, DATA, 0);
  }
  else
    agc_data_new_version[buf].lna_state    = 8;

  #ifndef RF_HAS_MDSP_TX_AGC 
  agc_data_new_version[buf].pa_state     = HWIO_INM(TX_PA_RD, 0x3); /* Low two bits are PA state */
  #else
  agc_data_new_version[buf].pa_state     = (byte)(CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS() & 0x3);
  #endif
  agc_data_new_version[buf].hdet_raw     = (byte)(rf_hdet_data >> 2);
  agc_data_new_version[buf].therm_scaled = therm_read();
  agc_data_new_version[buf].therm_raw    = adc_read( ADC_THERM );
  agc_data_new_version[buf].temp_index   = ftm_rf_temp_index;
  agc_data_new_version[buf].temp_remainder = ftm_rf_temp_remainder;
  agc_data_new_version[buf].intelliceiver_state = rf_intelliceiver_get_power_mode( RF_PATH_0 );


  agc_data_new_version[buf].max_power_limit = 511 - rf_tx_pwr_limit;

  agc_data_new_version[buf].beta_prime_gain = CDMA_MDSP_RD_TX_BETAP_GAIN() >> 6;

  agc_data_new_version[buf].tx_agc_adj_32_bit = (uint32)(CDMA_MDSP_GET_TX_AGC_ADJ_VAL());

  agc_data_new_version[buf].hdet_16_bit = rf_hdet_data;

  agc_data_new_version[buf].tx_lin_vs_freq_low_gain = rf_freq_comp.i_lin_freq_comp[0];

  agc_data_new_version[buf].tx_lin_vs_freq_high_gain = rf_freq_comp.i_lin_freq_comp[1];

  agc_data_new_version[buf].tx_lim_vs_freq = rf_freq_comp.i_tx_lim_vs_freq;

  agc_data_new_version[buf].pa_switch_rise_0 = (uint8)(127 - (int8)((int)(CDMA_MDSP_READ_PA_R1_RISE()) << 16 >> 24));

  agc_data_new_version[buf].pa_switch_fall_0 = (uint8)(127 - (int8)((int)(CDMA_MDSP_READ_PA_R1_FALL()) << 16 >> 24));

  if(++buf >= ftm_log_array_ptr[FTM_LOG_1X_AGC].num_buffer)
  {
    agc_log = (ftm_log_1x_agc_new_version_type *)ftm_log_malloc (FTM_LOG_1X_AGC, sizeof (ftm_log_1x_agc_new_version_type)
                                                  + (ftm_log_array_ptr[FTM_LOG_1X_AGC].num_buffer-1)
                                                  * sizeof(ftm_1x_agc_data_new_version_type) );
    if (agc_log)
    {
      /* have to copy the internal buffer to the rxf_log packet, we cannot
         simply accumulate the data in the rxf_log packet directly since any memory
         allocated with ftm_log_malloc must be passed to log_commit soon afterward
         (where "soon" is apparently less than tens of ms) */
      memcpy((void*)&(agc_log->data), (void*)agc_data_new_version, ftm_log_array_ptr[FTM_LOG_1X_AGC].num_buffer
             * sizeof(ftm_1x_agc_data_new_version_type));
    /* send log */
    log_commit( (log_type*) agc_log );
//    FTM_MSG_HIGH("Sent RXF status log callback.",0,0,0);
    }

    buf = 0;
  }
} /* ftm_log_1x_agc_cb */

/*===========================================================================

FUNCTION FTM_LOG_1X_AGC_C1_CB

DESCRIPTION
   This function reads the registers pertaining to the second chain RX front
   status and returns them in an FTM log packet.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_1x_agc_c1_cb(int4 interval)
{
  static int buf = 0;
  ftm_log_1x_c1_agc_type  *agc_log  = NULL;
  #ifdef RF_HAS_MDSP_TX_AGC
  byte tx_min_status;
  byte tx_max_status;
  #endif

  if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
  {
    agc_data_c1[buf].rx_agc = (int)HWIO_INI(RX_AGCc_1X_VALUE_RD, 0) << 22 >> 22;
  }
  else
    agc_data_c1[buf].rx_agc = -513;
  if (clk_regime_is_on( CLK_RGM_RXFRONT1_M ))
  {
    agc_data_c1[buf].rx_agc_1 = (int)HWIO_INI(RX_AGCc_1X_VALUE_RD, 1) << 22 >> 22;
  }
  else
    agc_data_c1[buf].rx_agc_1 = -513;

  /* strobe the CAGC uP_READ_SYNC bit to capture the tx_gain_ctl data
  */
  /* Latch the AGC read register values */
 
  #ifndef RF_HAS_MDSP_TX_AGC
  HWIO_OUT(TX_GAIN_CTL_LATCH,0);
  agc_data_c1[buf].tx_gain_ctl  = (int)HWIO_IN(TX_GAIN_CTL_RD) << 22 >> 22;

  agc_data_c1[buf].tx_gain_adj  = (int)HWIO_IN(TX_GAIN_ADJ_RD) << 23 >> 23;
  agc_data_c1[buf].tx_open_loop = (int)HWIO_IN(TX_OPEN_LOOP_RD) << 22 >> 22;
  agc_data_c1[buf].tx_agc_adj   = HWIO_IN(TX_AGC_ADJ_RD);
  agc_data_c1[buf].pa_ratchet   = HWIO_IN(RATCHET_BIT_DIS);
  #else
  agc_data_c1[buf].tx_open_loop  = (int)(CDMA_MDSP_RD_TX_OPEN_LOOP_VAL()) << 16 >> 22 ;
  agc_data_c1[buf].tx_gain_adj  = (int)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) << 16 >> 23;  
  agc_data_c1[buf].tx_gain_ctl  = (int)(CDMA_MDSP_RD_TRANSMIT_GAIN()) << 16 >> 22;
  tx_min_status = ((~(byte)(CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS())) & 0x1);
  tx_max_status = ((~(byte)(CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS())) & 0x1);
  agc_data_c1[buf].pa_ratchet   = ( tx_min_status << 1) & tx_max_status;
  agc_data_c1[buf].tx_agc_adj   = (int2)(((int2)(CDMA_MDSP_GET_TX_AGC_ADJ_VAL())) >> 7) + 256;
  #endif

  if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
  {
    agc_data_c1[buf].lna_state     = MSM_R_MASKED_i(RX_AGCc_LNA_RANGE_RD, DATA, 0);
  }
  else
    agc_data_c1[buf].lna_state     = 8;
  if (clk_regime_is_on( CLK_RGM_RXFRONT1_M ))
  {
    agc_data_c1[buf].lna_state_1= MSM_R_MASKED_i(RX_AGCc_LNA_RANGE_RD, DATA, 1);
  }
  else
    agc_data_c1[buf].lna_state_1   = 8;
  #ifndef RF_HAS_MDSP_TX_AGC 
  agc_data_c1[buf].pa_state     = HWIO_INM(TX_PA_RD, 0x3); /* Low two bits are PA state */
  #else
  agc_data_c1[buf].pa_state     = (byte)(CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS() & 0x3);
  #endif  
  agc_data_c1[buf].hdet_raw     = (byte)(rf_hdet_data >> 2);
  agc_data_c1[buf].therm_scaled = therm_read();
  agc_data_c1[buf].therm_raw    = adc_read( ADC_THERM );
  agc_data_c1[buf].temp_index   = ftm_rf_temp_index;
  agc_data_c1[buf].temp_remainder = ftm_rf_temp_remainder;
  agc_data_c1[buf].intelliceiver_state = rf_intelliceiver_get_power_mode( RF_PATH_0 );

  if(++buf >= ftm_log_array_ptr[FTM_LOG_1X_AGC_C1].num_buffer)
  {
    agc_log = (ftm_log_1x_c1_agc_type *)ftm_log_malloc (FTM_LOG_1X_AGC_C1, sizeof (ftm_log_1x_c1_agc_type)
                                                  + (ftm_log_array_ptr[FTM_LOG_1X_AGC_C1].num_buffer-1)
                                                  * sizeof(ftm_1x_c1_agc_data_type) );
    if (agc_log)
    {
      /* have to copy the internal buffer to the rxf_log packet, we cannot
         simply accumulate the data in the rxf_log packet directly since any memory
         allocated with ftm_log_malloc must be passed to log_commit soon afterward
         (where "soon" is apparently less than tens of ms) */
      memcpy((void*)&(agc_log->data), (void*)agc_data_c1, ftm_log_array_ptr[FTM_LOG_1X_AGC_C1].num_buffer
             * sizeof(ftm_1x_c1_agc_data_type));
    /* send log */
    log_commit( (log_type*) agc_log );
//    FTM_MSG_HIGH("Sent RXF diversity status log callback.",0,0,0);
    }

    buf = 0;
  }
} /* ftm_log_1x_agc_c1_cb */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION FTM_LOG_INTELLICEIVER_CB

DESCRIPTION
   This function reports the IntelliCeiver state along with AGC and LNA
   state.

DEPENDENCIES
   Callback function, must be enabled with ftm_log_enable.

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_intelliceiver_cb(int4 interval)
{
  ftm_log_intelliceiver_type *intelliceiver_log;
  int rx_agc;

  if ( rf_chain_status[0].rf_mode == DB_RF_SLEEP )
  {
    return;
  }

  intelliceiver_log
    = (ftm_log_intelliceiver_type *)ftm_log_malloc( FTM_LOG_INTELLICEIVER,
                                                    sizeof (ftm_log_intelliceiver_type) );
  if (intelliceiver_log)
  {
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
    {
      rx_agc = HWIO_INI(RX_AGCc_1X_VALUE_RD, 0);
    }
    if (rx_agc > 511)
    {
      rx_agc = rx_agc - 1024;
    }

    intelliceiver_log->data.rx_agc = rx_agc;
    intelliceiver_log->data.lna_state = MSM_R_MASKED_i( RX_AGCc_LNA_RANGE_RD, DATA, 0 );
    intelliceiver_log->data.power_mode = rf_intelliceiver_get_power_mode( 0 );
    //intelliceiver_log->data.reserved = adc_read( ADC_HDET_PCS );

    log_commit( intelliceiver_log );

  } /* if (intelliceiver_log) */

}

/*===========================================================================

FUNCTION FTM_LOG_ENABLE

DESCRIPTION
   This function enables FTM logging for a particular log id.
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Allocates and initializes memory for log array structure if necessary.
   Registers callback function for given log id.

===========================================================================*/
void ftm_log_enable(ftm_log_id_type log_id, int4 interval, int num_buffer)
{
  if(!(log_id > 1 && log_id < FTM_LOG_MAX)) /* check that log id is in range */
  {
    return;
  }

  if(ftm_log_array_ptr == NULL)
  {
    ftm_log_init();
  }

  if(ftm_log_array_ptr == NULL)
  {
    FTM_MSG_ERROR("Unable to allocate ftm log memory",0,0,0);   
    return;
  }

  ftm_log_array_ptr[log_id].num_buffer = ((num_buffer > 0 && num_buffer < MAX_BUFFERS) ? num_buffer : 1);

  if(!ftm_log_array_ptr[log_id].active)
  {
    /* Enable callback for log_id */
    ftm_log_array_ptr[log_id].cb_struct = (clk_cb_type*)ftm_malloc(sizeof(clk_cb_type));
    clk_def(ftm_log_array_ptr[log_id].cb_struct);
    clk_reg(ftm_log_array_ptr[log_id].cb_struct,
            ftm_log_array_ptr[log_id].cb_func,
            interval,
            interval,
            TRUE);
    ftm_log_array_ptr[log_id].active = TRUE;

    if(log_id == FTM_LOG_1X_AGC)
    {
      /* must allocate an internal buffer to collect the data */
     // agc_data = (ftm_1x_agc_data_type *)ftm_malloc (ftm_log_array_ptr[FTM_LOG_1X_AGC].num_buffer
         //                                             * sizeof (ftm_1x_agc_data_type));
      /* check that we have memory to work with */
      /*if (agc_data == NULL)
      {
        FTM_MSG_ERROR("Unable to allocate RXF status data buffer",0,0,0);
      }*/

      /** NEW VERSION FOR DATA LOGGING **/
      /* must allocate an internal buffer to collect the data */
      agc_data_new_version = (ftm_1x_agc_data_new_version_type *)ftm_malloc (ftm_log_array_ptr[FTM_LOG_1X_AGC].num_buffer
                                                      * sizeof (ftm_1x_agc_data_new_version_type));
      /* check that we have memory to work with */
      if (agc_data_new_version == NULL)
      {
        FTM_MSG_ERROR("Unable to allocate RXF status data buffer",0,0,0);
      }
    }
    if(log_id == FTM_LOG_1X_AGC_C1)
    {
      /* must allocate an internal buffer to collect the data */
      agc_data_c1 = (ftm_1x_c1_agc_data_type *)ftm_malloc (ftm_log_array_ptr[FTM_LOG_1X_AGC_C1].num_buffer
                                                      * sizeof (ftm_1x_c1_agc_data_type));
      /* check that we have memory to work with */
      if (agc_data_c1 == NULL)
      {
        FTM_MSG_ERROR("Unable to allocate RXF status data buffer",0,0,0);
      }
    }
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  }
  else
  {
    /* log is already active, just set new parameters */
    timer_set(ftm_log_array_ptr[log_id].cb_struct, interval, interval, T_MSEC);
//    ftm_log_array_ptr[log_id].cb_struct->reload = interval;
  }

} /* End ftm_log_enable */

/*===========================================================================

FUNCTION FTM_LOG_DISABLE

DESCRIPTION
   This function disables FTM logging for a particular log id.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Deregisters the callback for the given log id.
   Deallocates the log array from the heap if no logs remain active.

===========================================================================*/
void ftm_log_disable(ftm_log_id_type log_id)
{
  int i;
  boolean remaining;

  if(ftm_log_array_ptr==NULL)
  {
    return;
  }

  if(!(log_id > 1 && log_id < FTM_LOG_MAX)) /* check that log id is in range */
  {
    return;
  }

  if(ftm_log_array_ptr[log_id].active)
  {
    clk_dereg(ftm_log_array_ptr[log_id].cb_struct);
    ftm_free(ftm_log_array_ptr[log_id].cb_struct);
    ftm_log_array_ptr[log_id].active = FALSE;

    for(i=0, remaining=FALSE; i<FTM_LOG_MAX; i++)
    {
      remaining = remaining || ftm_log_array_ptr[i].active;
    }
    if(!remaining)
    {
      /* deallocate the array if no logs remain active */
      ftm_free(ftm_log_array_ptr);
      ftm_log_array_ptr = NULL;
    }
    if(log_id == FTM_LOG_1X_AGC)
    {
      //ftm_free((void*)agc_data); /* free the internal buffer */
      ftm_free((void*)agc_data_new_version); /* free the internal buffer */
    }
    if(log_id == FTM_LOG_1X_AGC_C1)
    {
      ftm_free((void*)agc_data_c1); /* free the internal buffer */
    }
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  }

} /* End ftm_log_disable */

/*===========================================================================

FUNCTION FTM_LOG_DISPATCH

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_log_dispatch(ftm_log_req_pkt_type  *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  FTM_MSG_MED("FTM Log dispatching packet id: %d",cmd_ptr->ftm_hdr.cmd_id,0,0);

  switch(cmd_ptr->ftm_hdr.cmd_id)
  {
  case FTM_LOG_ENABLE:
    ftm_log_enable(cmd_ptr->params.log_id, cmd_ptr->params.interval,
                   cmd_ptr->params.num_buffer);
    break;
  case FTM_LOG_DISABLE:
    ftm_log_disable(cmd_ptr->params.log_id);
    break;
  default:
    FTM_MSG_ERROR("Unknown FTM Log Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
    break;
  }
  return rsp_pkt;
} /* End ftm_log_dispatch */

#ifndef T_MSM6800
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */
#endif /* T_MSM6800 */

/*===========================================================================

FUNCTION FTM_LOG_1X_AGC

DESCRIPTION
   This function logs PRX and DRX AGC status independent of the FTM
   logging callback timers.

DEPENDENCIES
   

RETURN VALUE
   None.

SIDE EFFECTS
   Generates FTM log pkt with results.

===========================================================================*/
void ftm_log_1x_agc( void )
{
  ftm_log_1x_c1_agc_type  *agc_log  = NULL;
  #ifdef RF_HAS_MDSP_TX_AGC
  byte tx_min_status;
  byte tx_max_status;
  #endif

  agc_log = (ftm_log_1x_c1_agc_type *)ftm_log_malloc( FTM_LOG_1X_AGC_C1, sizeof (ftm_log_1x_c1_agc_type) );
  if (agc_log)
  {
    if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
    {
      agc_log->data.rx_agc = (int)HWIO_INI(RX_AGCc_1X_VALUE_RD, 0) << 22 >> 22;
    }
    else
    {
      agc_log->data.rx_agc = -513;
    }
    if (clk_regime_is_on( CLK_RGM_RXFRONT1_M ))
    {
      agc_log->data.rx_agc_1 = (int)HWIO_INI(RX_AGCc_1X_VALUE_RD, 1) << 22 >> 22;
    }
    else
    {
      agc_log->data.rx_agc_1 = -513;
    }
  
    /* strobe the CAGC uP_READ_SYNC bit to capture the tx_gain_ctl data
    */
    /* Latch the AGC read register values */
    
    #ifndef RF_HAS_MDSP_TX_AGC
     HWIO_OUT(TX_GAIN_CTL_LATCH,0);
    agc_log->data.tx_gain_ctl  = (int)HWIO_IN(TX_GAIN_CTL_RD) << 22 >> 22;
  
    agc_log->data.tx_gain_adj  = (int)HWIO_IN(TX_GAIN_ADJ_RD) << 23 >> 23;
    agc_log->data.tx_open_loop = (int)HWIO_IN(TX_OPEN_LOOP_RD) << 22 >> 22;
    agc_log->data.tx_agc_adj   = HWIO_IN(TX_AGC_ADJ_RD);
    agc_log->data.pa_ratchet   = HWIO_IN(RATCHET_BIT_DIS);
    #else
    agc_log->data.tx_open_loop = (int)(CDMA_MDSP_RD_TX_OPEN_LOOP_VAL()) << 16 >> 22 ;
    agc_log->data.tx_gain_adj  = (int)(CDMA_MDSP_RD_TX_CLOSED_LOOP_VAL()) << 16 >> 23;
    agc_log->data.tx_gain_ctl  =  (int)(CDMA_MDSP_RD_TRANSMIT_GAIN()) << 16 >> 22;

    tx_min_status = ((~(byte)(CDMA_MDSP_RD_TX_MIN_LIMIT_RATCHET_STATUS())) & 0x1);
    tx_max_status = ((~(byte)(CDMA_MDSP_RD_TX_MAX_LIMIT_RATCHET_STATUS())) & 0x1);
    agc_log->data.pa_ratchet   = ( tx_min_status << 1) & tx_max_status;
    agc_log->data.tx_agc_adj   = (int2)(((int2)(CDMA_MDSP_GET_TX_AGC_ADJ_VAL())) >> 7) + 256;
    #endif
  
    if (clk_regime_is_on( CLK_RGM_RXFRONT0_M ))
    {
      agc_log->data.lna_state     = MSM_R_MASKED_i(RX_AGCc_LNA_RANGE_RD, DATA, 0);
    }
    else
    {
      agc_log->data.lna_state     = 8;
    }
    if (clk_regime_is_on( CLK_RGM_RXFRONT1_M ))
    {
      agc_log->data.lna_state_1= MSM_R_MASKED_i(RX_AGCc_LNA_RANGE_RD, DATA, 1);
    }
    else
    {
      agc_log->data.lna_state_1   = 8;
    }
    #ifndef RF_HAS_MDSP_TX_AGC 
    agc_log->data.pa_state     = HWIO_INM(TX_PA_RD, 0x3); /* Low two bits are PA state */
    #else
    agc_log->data.pa_state     =(byte)(CDMA_MDSP_RD_PA_R0_R1_PIN_STATUS()& 0x3);
    #endif  
    agc_log->data.hdet_raw     = (byte)(rf_hdet_data >> 2);
    agc_log->data.therm_scaled = therm_read();
    agc_log->data.therm_raw    = adc_read( ADC_THERM );
    agc_log->data.temp_index   = ftm_rf_temp_index;
    agc_log->data.temp_remainder = ftm_rf_temp_remainder;
    agc_log->data.intelliceiver_state = rf_intelliceiver_get_power_mode( RF_PATH_0 );
  
    /* send log */
    log_commit( (log_type*) agc_log );
  }
} /* ftm_log_1x_agc */

#endif /* FTM_HAS_LOGGING */
#endif /* FEATURE_FACTORY_TESTMODE */
