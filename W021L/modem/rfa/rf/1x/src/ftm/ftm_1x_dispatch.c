/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM 1x Dispatch

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007, 2008       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_dispatch.c#8 $ 
  $DateTime: 2008/11/19 11:46:56 $ 
  $Author: lesp $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/08   sr      Klocwork fixes
10/12/08   vm      Changed the name to FTM_SET_BLOCK_MASK
09/30/08   vb/vm   Added Support for Band Class Sub Block Implementation
06/11/08   sl      Added support for UINT16 HDET under new API FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2=144
                   and under feature RF_HAS_HDET_UINT16.
04/18/08   vb      Added support for reading Beta Prime value and setting the Primary Chain to Sleep
03/12/08   vm      Added Support for supporting Single chain on QSC1100
05/02/07   anr	   Enabled the calling of ftm_set_tx_sweep_step_size()
03/13/07   bmg     HDET read from RF driver instead of directly from ADC
                   channel
03/31/06   vm      Added support for new APIs for new IM2 cal algorithm
31/01/06   vm      Added a code to return a negative  non valid value for C/N
                   if the RF chain is sleeping
12/05/06   hkk     Fixes for compile warnings 
10/25/05   vm      Used the function ftm_synth_is_locked() to get the 
                   synth lock status
03/20/05   bmg     Added ftm_set_rf_power_mode API
10/31/04   ra      Added support for: ftm_get_im2_with_suppression,
                   ftm_get_all_hdet_from_tx_sweep_cal
10/12/04   jac     Mainlined ASYNC packet code.
07/06/04   bmg     Merged all library headers into ftm_lib.h
05/27/04   bmg     Added closed-loop HDET tracking API
05/18/04   rsr     Renamed FEATURE_FTM_ASYNCH_PKT to FTM_HAS_ASYNCH_PKT
05/18/04   rsr     ftm_pkt_type has changed so updated usage of ftm_pkt_type
				           to remove cmd_ptr
03/09/04   ra      renamed FEATURE_FTM_BYPASS_NV_CAL_POINTS to FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
12/29/03   bmg     Moved C/N averaging into library function.
11/21/03   bmg     Extended second chain call API to support extra modes
11/14/03   bmg     Added second chain call test API to FTM command set
11/12/03   bmg     Second chain call test mode API
11/03/03   ra      Leave Tx alone if setting mode in OFS
10/20/03   bmg     Added HDR RF mode support for tuning to channel
10/08/03   bmg     Added experimental feature for passed in AGC calibration
09/12/03   bmg     Removed RF driver DB dependencies, added ftm_get_CtoN API
09/10/03   bmg     Removed adc_read cast which breaks with logical ADC channels
09/08/03   wd      Added Bad command response pkt for uknown commands.
09/08/03   ra      Renamed FTM_GET_HDET subcommand id to FTM_GET_HDET_FROM_TX_SWEEP_CAL.
                   The new name reflects the binding of the stored HDET values taken during
                   TX CAL SWEEP. If the user wishes to read HDET, the FTM_GET_ADC_VAL api
                   should be used  
09/05/03   bmg     Removed unused FM code.
08/27/03   wd      Added support for variable length response pkt.
07/29/03   bmg     Removed hard-coded RF device values
07/22/03   bmg     Added calling support for multi-chain RF driver
                   Converted test_* names to ftm_* names
07/09/03   bmg     Merged in Couger 3030->4040 changes:
  06/19/03 wd      Removed obsolete wait when doing FTM_GET_FM_IQ.
  06/06/03 wd      Removed unnecessaries includes.
  04/25/03 wd      Added ftm_lna_range_override() API to enable/disable LNA 
                   range override. And change enums from TEST_ to FTM_.
  04/25/03 wd      Fixed featurization problem.
  04/07/03 wd      Removed Redundant call to set_tx during set_chan.
  04/03/03 wd      Added check for TX state and react as needed.
06/04/03   bmg     Added RF mode setting again, more IM2 support
05/28/03   bmg     Removed setting mode functionality for now
05/27/03   bmg     Re-enabled IM2, added tracking of lna_range state
03/06/03   wd      Move setup code from zif lib to here to be able to remove
                   featurization from zif lib.
02/17/03   wd      Include ftm_im2.h instead of ftm_im2_lib.h
02/07/03   wd      Rename file from ftm_rf_1x_dispatch.c -> ftm_1x_dispatch.c
02-07-03   wd      Changed calls to iqmismatch and get_im2, moved to 
                   ftm_im2_lib.c, and clean up.
12/11/02   ra      mainlined CDMA1X/AMPS 
10-04-02   ra      Created.
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "comdef.h"
#include "target.h"
#include "ftm_task.h"
#include "ftm_1x_ctl.h"
#include "ftm_1x_dispatch.h"
#include "ftm_lib.h"
#include "ftm_cmdi.h"
#include "pm.h"
#include "ring.h"
#include "db.h"
#include "adc.h"
#include "synth.h"
/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/
extern rf_chain_state_type rf_chain_status[];

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

/* global var to track state of transmitter */
LOCAL boolean tx_state = FALSE;

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION FTM_1X_DISPATCH

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
ftm_rsp_pkt_type ftm_1x_dispatch(rfcom_device_enum_type device, ftm_pkt_type  * ftm)
{
  im2_type im2_value;
  word rf_channel;
  int temp_cton, i;
  rf_chain_state_type *rf_chain_ptr;
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  synth_pll_for_synth_check pll_device = RX_PLL;

#ifdef FEATURE_MULTI_RX_CHAIN
  FTM_MSG_MED("FTM 1X dispatching packet id: %d, device %d, path %d",ftm->rf_params.ftm_rf_cmd,device,rfcom_to_path[0]);
#else
  FTM_MSG_MED("FTM 1X dispatching packet id: %d, device %d, path %d",ftm->rf_params.ftm_rf_cmd,0,0);
#endif

  /* ----------------------------------------
  ** Figure out what mode we are currently in
  ** ---------------------------------------- */
  /* point to chain status structure for the device */
  rf_chain_ptr = &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  #ifndef FEATURE_MULTI_RX_CHAIN
  if(device != RFCOM_TRANSCEIVER_0)
  {
    ERR_FATAL("Second chain is not supported", 0, 0, 0);
    return rsp_pkt;
  }
  #endif

  switch (ftm->rf_params.ftm_rf_cmd)
  {
  
  case FTM_SET_MODE:
    if(device != RFCOM_RECEIVER_1)
	{
      if (tx_state)
      {
        FTM_MSG_HIGH("TX is ON while entering set mode, turning TX OFF.",0,0,0);
        ftm_set_tx(device, 0);        
        tx_state = FALSE;
      }
	}
    ftm_set_rf_mode(device, ftm->rf_params.ftm_rf_factory_data.mode);
    FTM_MSG_MED("FTM_SET_MODE: %d", ftm->rf_params.ftm_rf_factory_data.mode, 0, 0);
    break;

  case FTM_SET_PDM:
    ftm_set_pdm((ftm_pdm_id_type) ftm->rf_params.ftm_rf_factory_data.id_val.id,
                 ftm->rf_params.ftm_rf_factory_data.id_val.val);

    FTM_MSG_MED("FTM_SET_PDM: %d, %d", ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;

  case FTM_SET_TX_ON:
    ftm_set_tx(device, 1);       
    tx_state = TRUE;
    FTM_MSG_MED("FTM_SET_TX_ON", 0, 0, 0);
    break;

  case FTM_SET_TX_OFF:
    ftm_set_tx(device, 0);        
    tx_state = FALSE;
    FTM_MSG_MED("FTM_SET_TX_OFF", 0, 0, 0);
    break;

  case FTM_SET_CHAN:
    rf_channel = ftm->rf_params.ftm_rf_factory_data.chan;
    ftm_tune_to_chan(device, rf_channel);
    FTM_MSG_MED("FTM_SET_CHAN: %d", rf_channel, 0, 0);
    break;

  case FTM_SET_CDMA_CW_WAVEFORM:
    ftm_set_cdma_cw(ftm->rf_params.ftm_rf_factory_data.on_off);        
    FTM_MSG_MED("FTM_SET_CDMA_CW_WAVEFORM: %d", ftm->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  case FTM_SET_TX_WAVEFORM:
    ftm_set_tx_waveform(ftm->rf_params.ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform,
                        ftm->rf_params.ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform_attrib);

    FTM_MSG_MED("FTM_SET_TX_WAVEFORM: %d %d",ftm->rf_params.ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform,
                 ftm->rf_params.ftm_rf_factory_data.ftm_tx_waveform_data.ftm_tx_waveform_attrib,0);
    break;

  case FTM_GET_CAGC_RX_AGC:
    i = 0;
    if (rf_chain_ptr->rf_mode != DB_RF_ANALOG)
      ftm->rf_params.ftm_rf_factory_data.gen_w = i = ftm_get_rf_cdma_rx_agc(device);
    FTM_MSG_MED("FTM_GET_CAGC_RX_AGC: %d", i, 0, 0);
    break;

  case FTM_SET_PA_RANGE:
    ftm_set_pa_r1_r0((byte) ftm->rf_params.ftm_rf_factory_data.range);
    FTM_MSG_MED("FTM_SET_PA_RANGE: %d", ftm->rf_params.ftm_rf_factory_data.range, 0, 0);
    break;

  case FTM_SET_LNA_RANGE: // done       
#ifdef FEATURE_MULTI_RX_CHAIN
    rf_set_cdma_lna_decision( device, ftm->rf_params.ftm_rf_factory_data.range);
#else /* FEATURE_MULTI_RX_CHAIN */
    rf_set_cdma_lna_decision( RFCOM_TRANSCEIVER_0,ftm->rf_params.ftm_rf_factory_data.range);
#endif /* else FEATURE_MULTI_RX_CHAIN */
    FTM_MSG_MED("FTM_SET_LNA_RANGE: %d", ftm->rf_params.ftm_rf_factory_data.range, 0, 0);
    break;

  case FTM_GET_ADC_VAL:
    if ( ftm->rf_params.ftm_rf_factory_data.id_val.id == ADC_HDET_CELL)
    {
       ftm->rf_params.ftm_rf_factory_data.id_val.val =       
         (word)rf_read_hdet( rf_chain_ptr->rf_curr_cdma_band );
    }
    else
    {
       ftm->rf_params.ftm_rf_factory_data.id_val.val =       
         (word)adc_read(ftm->rf_params.ftm_rf_factory_data.id_val.id);
    }
    FTM_MSG_MED("FTM_GET_ADC_VAL: %d = %d", ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;

  case FTM_GET_SYNTH_STATE:
    if (rf_chain_ptr->rf_mode == DB_RF_GPS)
    {
      pll_device = GPS_PLL;
    }
    else if (rf_chain_ptr->rf_mode == DB_RF_SLEEP)
    {
      FTM_MSG_ERROR( "Cannot check for synth lock in sleep mode: %d", rf_chain_ptr->rf_mode,0,0 );
    }
    else
    {
      pll_device = RX_PLL;
    }

    if (ftm_synth_is_locked( device,pll_device ) == TRUE)
    {
      ftm->rf_params.ftm_rf_factory_data.on_off = ON;
    }
    else
    {
      ftm->rf_params.ftm_rf_factory_data.on_off = OFF;
    }

    FTM_MSG_MED("FTM_GET_SYNTH_STATE: %d", ftm->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  case FTM_SET_LNA_OFFSET:
    ftm_set_lna_offset(device,
                       (byte) ftm->rf_params.ftm_rf_factory_data.id_val.id,
                       ftm->rf_params.ftm_rf_factory_data.id_val.val);      
    FTM_MSG_MED("FTM_SET_LNA_OFFSET: %d = %d", ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;

  case FTM_SET_DVGA_OFFSET:
    ftm_Set_Dvga_Gain_Offset(device,
                             ftm->rf_params.ftm_rf_factory_data.gen_w);      
    FTM_MSG_MED("FTM_SET_DVGA_OFFSET: %d", ftm->rf_params.ftm_rf_factory_data.gen_w, 0, 0);
    break;

  case FTM_GET_CDMA_IM2:  
#ifdef FTM_IM2_DSP
    ftm_get_im2(&im2_value);
#else
    #ifdef FTM_USES_NEW_IM2_SEARCH_METHOD
        ftm_get_im2(device, &im2_value);
    #else
        ftm_get_legacy_im2(device, &im2_value);
    #endif
#endif
    ftm->rf_params.ftm_rf_factory_data.byte_struct.byte0 = im2_value.i_dac;
    ftm->rf_params.ftm_rf_factory_data.byte_struct.byte1 = im2_value.q_dac;
    ftm->rf_params.ftm_rf_factory_data.byte_struct.byte2 = im2_value.transconductor;
    FTM_MSG_MED("FTM_GET_CDMA_IM2: %d %d %d", im2_value.i_dac, im2_value.q_dac, im2_value.transconductor);
    break;

  case FTM_TX_SWEEP_CAL:
    ftm_tx_sweep_cal(ftm->rf_params.ftm_rf_factory_data.on_off); 
    FTM_MSG_MED("FTM_TX_SWEEP_CAL: %d", ftm->rf_params.ftm_rf_factory_data.on_off, 0, 0);
    break;

  case FTM_GET_DVGA_OFFSET:
    ftm->rf_params.ftm_rf_factory_data.gen_w = 
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      ftm_get_dvga_offset(device, ftm->rf_params.ftm_rf_factory_data.gen_w);
#else
      ftm_get_dvga_offset(device);
#endif
    FTM_MSG_MED("FTM_GET_DVGA_OFFSET: %d", ftm->rf_params.ftm_rf_factory_data.gen_w, 0, 0);
    break;

  case FTM_GET_LNA_OFFSET:
    i = ftm->rf_params.ftm_rf_factory_data.id_val.val;
    ftm->rf_params.ftm_rf_factory_data.id_val.val = 
#ifdef FEATURE_FTM_BYPASS_LNA_NV_CAL_POINTS
      ftm_get_lna_offset(device, ftm->rf_params.ftm_rf_factory_data.id_val.id, ftm->rf_params.ftm_rf_factory_data.id_val.val);
#else
      ftm_get_lna_offset(device, ftm->rf_params.ftm_rf_factory_data.id_val.id);
#endif
    FTM_MSG_MED("FTM_GET_LNA_OFFSET: %d = %d (%d)", ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val,
                i);
    break;

  case FTM_GET_HDET_FROM_TX_SWEEP_CAL:

    ftm->rf_params.ftm_rf_factory_data.id_val.val = 
    ftm_get_hdet_from_tx_cal_sweep(ftm->rf_params.ftm_rf_factory_data.id_val.id);
    FTM_MSG_MED("FTM_GET_HDET_FROM_TX_SWEEP_CAL: %d = %d", ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;
  
  case FTM_SET_SECONDARY_CHAIN:
    ftm_set_secondary_chain(ftm->rf_params.ftm_rf_factory_data.second_chain);
    FTM_MSG_MED("FTM_SET_SECONDARY_CHAIN: %d", ftm->rf_params.ftm_rf_factory_data.second_chain, 0, 0);
    break;
  
  case FTM_GET_CTON:
   if( rf_chain_status[ rfcom_to_path[device] ].rf_mode == DB_RF_SLEEP )
   {
     temp_cton = -999;
     ftm->rf_params.ftm_rf_factory_data.gen_d = (dword)(temp_cton);
   }
   else
   {
     temp_cton = ftm_get_CtoN(device);
     ftm->rf_params.ftm_rf_factory_data.gen_d = (dword)(temp_cton);
     FTM_MSG_MED("FTM_GET_CTON: %d", ftm->rf_params.ftm_rf_factory_data.gen_d, 0, 0);
   }
   break;

  case FTM_SECOND_CHAIN_TEST_CALL:
    ftm_rx_on_second_chain(ftm->rf_params.ftm_rf_factory_data.gen_b);
    FTM_MSG_MED("FTM_SECOND_CHAIN_TEST_CALL: %d", ftm->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    break;

  case FTM_SET_HDET_TRACKING:
    ftm_set_hdet_tracking(ftm->rf_params.ftm_rf_factory_data.id_val.id,
                          ftm->rf_params.ftm_rf_factory_data.id_val.val);
    FTM_MSG_MED("FTM_SET_HDET_TRACKING: %d %d",
                ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);
    break;
  case FTM_CONFIGURE_TX_SWEEP_CAL:        
        ftm_configure_tx_sweep_cal(ftm->rf_params.ftm_rf_factory_data.id_val.id,
                                   (byte)ftm->rf_params.ftm_rf_factory_data.id_val.val);      
        
        FTM_MSG_MED("FTM_CONFIGURE_TX_SWEEP_CAL: %d %d",
                ftm->rf_params.ftm_rf_factory_data.id_val.id,
                ftm->rf_params.ftm_rf_factory_data.id_val.val, 0);

    break;

  case FTM_GET_CDMA_IM2_WITH_SUPPRESSION:
#ifdef FTM_USES_NEW_IM2_SEARCH_METHOD
    (void)ftm_get_im2_with_suppression(device, &im2_value);
#else
    (void)ftm_get_legacy_im2_with_suppression(device, &im2_value);
#endif        	  
    ftm->rf_params.ftm_rf_factory_data.mix_struct.byte0 = im2_value.i_dac;
    ftm->rf_params.ftm_rf_factory_data.mix_struct.byte1 = im2_value.q_dac;
    ftm->rf_params.ftm_rf_factory_data.mix_struct.byte2 = im2_value.transconductor;
    ftm->rf_params.ftm_rf_factory_data.mix_struct.intv = im2_value.suppression_level;
	break;

  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2:

    FTM_MSG_MED("FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL_V2:",0,0,0);

    for(i=0;i<FTM_NUM_HDET_TO_RETURN;i++)
    {    
      ftm->rf_params.ftm_rf_factory_data.hdet_array_16_bit[i] = ftm_get_hdet_from_tx_cal_sweep(i);
    }
    break;

  case FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL:

    FTM_MSG_MED("FTM_GET_ALL_HDET_FROM_TX_SWEEP_CAL:",0,0,0);

    for(i=0;i<FTM_NUM_HDET_TO_RETURN;i++)
    {    
      ftm->rf_params.ftm_rf_factory_data.hdet_array[i] = ftm_get_hdet_from_tx_cal_sweep(i);
    }
    break;

#ifdef RF_HAS_POWER_MODE_LIB
  case FTM_SET_RF_POWER_MODE:
    FTM_MSG_MED("FTM_SET_RF_POWER_MODE: %d",
                ftm->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
    ftm_set_rf_power_mode(device, ftm->rf_params.ftm_rf_factory_data.gen_b);
    break;
#endif /* RF_HAS_POWER_MODE_LIB */

  case FTM_SET_TX_SWEEP_STEP_SIZE:
	ftm_set_tx_sweep_step_size(ftm->rf_params.ftm_rf_factory_data.gen_b);
	break;

  case FTM_SET_BETA_PRIME_SCALING:
	ftm_set_beta_prime_scaling(ftm->rf_params.ftm_rf_factory_data.id_val.val,ftm->rf_params.ftm_rf_factory_data.id_val.id);
    FTM_MSG_MED("FTM_SET_BETA_PRIME_SCALING implemented with Resolution: %d & Scaled value: %d", ftm->rf_params.ftm_rf_factory_data.id_val.val, ftm->rf_params.ftm_rf_factory_data.id_val.id, 0);
	break;

  case FTM_SET_RF_PRIMARY_CHAIN_STATE:
	ftm_set_rf_primary_chain_state((ftm_primary_chain_state)ftm->rf_params.ftm_rf_factory_data.gen_b);      
    FTM_MSG_MED("FTM_SET_RF_PRIMARY_CHAIN_STATE: %d", ftm->rf_params.ftm_rf_factory_data.gen_b, 0, 0);
	break;

#ifdef RF_HAS_SUB_BLOCK_SUPPORT
    case FTM_SET_BLOCK_MASK:
      ftm->rf_params.ftm_rf_factory_data.on_off = ftm_set_block_mask_override(device,ftm->rf_params.ftm_rf_factory_data.ftm_block_info.index,ftm->rf_params.ftm_rf_factory_data.ftm_block_info.mask);
      if(ftm->rf_params.ftm_rf_factory_data.on_off == TRUE)
      {
        FTM_MSG_MED("Success in setting the Block Mask Passed, Index:%d, Block Mask:%d",ftm->rf_params.ftm_rf_factory_data.ftm_block_info.index,ftm->rf_params.ftm_rf_factory_data.ftm_block_info.mask,0);
      }
      else
      {
        FTM_MSG_MED("Failure in setting the Block Mask Passed, Index:%d, Block Mask:%d",ftm->rf_params.ftm_rf_factory_data.ftm_block_info.index,ftm->rf_params.ftm_rf_factory_data.ftm_block_info.mask,0);
      }
      break;
#endif /* RF_HAS_SUB_BLOCK_SUPPORT */

  default:
    rsp_pkt.pkt_len = 0;
    rsp_pkt.pkt_payload = 0; // initialized to satisfy KW
    rsp_pkt.delete_payload = 0;

    FTM_MSG_ERROR("Unknown FTM 1X RF Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  }

  return rsp_pkt;
}   /* end ftm_1x_dispatch */
#endif /*FEATURE_FACTORY_TESTMODE */
