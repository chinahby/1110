/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                          
                    F T M  C O M M O N  D I S P A T C H

GENERAL DESCRIPTION
  This is the FTM Common dispatch file which contains the dispatch function
  for commands common to both 1x and UMTS.

Copyright (c) 2005, 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_common_dispatch.c#4 $ 
$DateTime: 2008/11/19 11:46:56 $ 
$Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/08   sr      Klocwork fixes 
03/21/08   sl      Added FTM_DO_INTERNAL_DEVICE_CAL for 1x.
10/13/07   anr	   Supprot for FTM_SET_CALIBRATION_STATE 
10/03/07   bmg     Protect 1x only builds from UMTS features
08/01/07    kg     featurization changes for EDGE only support
07/23/07   anr	   XO Cal APIs append their own headers. 
07/15/07   dyc     Added rf_init_rf_nv_items() prototype.
05/16/07   anr     Added FTM_DO_XO_DC_CAL and FTM_DO_XO_FT_CURVE_CAL 
01/19/07   sar     Added call to rf_init_rf_nv_items() in ftm_common_dispatch();
09/21/06   sar     Adding case for INACTIVE items fo rfnv_retrieve_nv_items.
09/21/06   sar     Added switch statement to properly handle the return value from
                   rfnv_retrieve_nv_items() in ftm_common_dispatch.
04/19/06   ka      Updated Tx-Rx API for diversity/lpm changes.
02/07/06   ka      Added FTM_DO_INTERNAL_DEVICE_CAL.
12/05/06   hkk     Fixes for compile warnings 
11-17-05   ka      Added FTM_LOAD_RF_NV.
08-19-05   ka      Created
===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_common_dispatch.h"
#include "ftm_task.h"
#include "rfnv.h"
#include "ftm_sys_ctl.h"


#ifdef FTM_HAS_WCDMA
#error code not present
#endif /*FTM_HAS_WCMDA*/

#ifdef FTM_HAS_1X
#include "ftm_1x_ctl.h"
#include "rfnv.h"
#endif

#ifdef FTM_HAS_GSM
#error code not present
#endif /* FTM_HAS_GSM */


typedef PACKED struct {
  uint16 rx_chain;
  uint8  num_freqs;
  uint8  num_steps;
  uint16 step_length;
  uint8  *data;
} ftm_tx_rx_freq_cal_sweep_param_type;

extern void rf_init_rf_nv_items( void );

/*===========================================================================

FUNCTION FTM_COMMON_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_dispatch(ftm_common_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_composite_cmd_header_type  rsp_headers;
  /* local_buffer and extra_data_len allow return data size to depend on a 
     request parameter. Unused for now */
  byte                           *local_buffer  = NULL;
  uint16                         extra_data_len = 0; 
  ftm_common_result_type   result;
#if defined(FTM_HAS_WCDMA) || defined(FTM_HAS_1X)
  ftm_tx_rx_freq_cal_sweep_param_type *sweep_param_ptr;
  byte *data_ptr;
#endif /*FTM_HAS_WCDMA || FTM_HAS_1X*/

#ifdef FTM_HAS_1X
  rfnv_status_type status;
#endif /*FTM_HAS_1X*/
  /* Initialize response packet size to length of the common headers */
  /* the size of the return parameters will be added in the dispatch cases below */
  rsp_headers.ftm_hdr.cmd_rsp_pkt_size = sizeof(ftm_composite_cmd_header_type);


  switch (cmd_ptr->ftm_hdr.cmd_id)
  {
#if defined(FTM_HAS_WCDMA) || defined(FTM_HAS_1X)
  case FTM_TX_RX_FREQ_CAL_SWEEP: 

    sweep_param_ptr = (ftm_tx_rx_freq_cal_sweep_param_type*)&(cmd_ptr->ftm_extended_params);
    data_ptr = (byte*)&(sweep_param_ptr->data);

    result.error_code = (uint16)ftm_do_tx_rx_freq_cal_sweep(
    /* desired rx chain */
    sweep_param_ptr->rx_chain,
    /* number of frequencies */
    sweep_param_ptr->num_freqs,
    /* number of steps per frequency */
    sweep_param_ptr->num_steps,
    /* step length in sleep clock cycles */
    sweep_param_ptr->step_length, 
    /* list of frequencies */
    (uint16*)data_ptr,
    /* TX PDM list */
    (uint16*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16)),
    /* primary chain expected agc value list */
    (int16*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16) + 
             sweep_param_ptr->num_steps*sizeof(uint16)),
    /* PA range list */
    (uint8*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16) + 
             2*sweep_param_ptr->num_steps*sizeof(uint16)),
    /* read HDET flag list */
    (uint8*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16) + 
             2*sweep_param_ptr->num_steps*sizeof(uint16) + 
             sweep_param_ptr->num_steps*sizeof(uint8)),
    /* RX action list */
    (uint8*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16) + 
             2*sweep_param_ptr->num_steps*sizeof(uint16) + 
             2*sweep_param_ptr->num_steps*sizeof(uint8)),
    /* LPM list */
    (uint8*)(data_ptr + sweep_param_ptr->num_freqs*sizeof(uint16) + 
             2*sweep_param_ptr->num_steps*sizeof(uint16) + 
             3*sweep_param_ptr->num_steps*sizeof(uint8)));

    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
    break;
#endif /* defined(FTM_HAS_WCDMA) || defined(FTM_HAS_1X) */

  case FTM_LOAD_RF_NV:
    #ifndef FTM_HAS_1X
    result.error_code = (uint16)rfnv_retrieve_nv_items(rex_self(), FTM_SIG_WAIT_SIG, ftm_sig_rex_wait_cb);
    #else /* FTM_HAS_1X */
    status = rfnv_retrieve_nv_items(rex_self(), FTM_SIG_WAIT_SIG, ftm_sig_rex_wait_cb);
    rf_init_rf_nv_items();
    /* Translate status enum to boolean result.error_code value */
    switch (status) {
    case RFNV_READ_SUCCESS:
    case RFNV_WRITE_SUCCESS:
    case RFNV_WRITE_FAILED:
    case RFNV_INACTIVE_ITEM:
      result.error_code = 0;
      break;
    default:
      result.error_code = 1;
      break;
    }
    #endif /* FTM_HAS_1X */
    #ifdef FTM_HAS_WCDMA
#error code not present
    #else
       #ifdef FTM_HAS_GSM
#error code not present
       #endif /* FTM_HAS_GSM */
    #endif /*FTM_HAS_WCDMA*/
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
    break;


  case FTM_DO_INTERNAL_DEVICE_CAL:
    /* Error Code of 0 means no errors to report */
    result.error_code = ftm_do_internal_device_calibration();
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
    break;


#if defined (FTM_HAS_XO_FACTORY_CAL) 

  case FTM_DO_XO_DC_CAL:
    rsp_pkt =  ftm_do_xo_dc_cal((ftm_xo_cal_request_type*) cmd_ptr);
	rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_xo_cal_response_type);
	break;

  case FTM_DO_XO_FT_CURVE_CAL:
	rsp_pkt =  ftm_do_xo_ft_curve_cal((ftm_xo_cal_request_type*) cmd_ptr);
	rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_xo_cal_response_type);
	break;
#endif	  

  case FTM_SET_CALIBRATION_STATE:
     {
        int ftm_cal_state = (int)(cmd_ptr->ftm_extended_params) ;
        ftm_set_calibration_state(  (ftm_cal_state_type)ftm_cal_state  );
     }
    result.error_code = 0;
	rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
	break;

  default:

    result.error_code = FTM_FAILURE;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_common_result_type);
    break;
  
  }

  if ((!(cmd_ptr->ftm_hdr.cmd_id == FTM_DO_XO_DC_CAL )) && (!(cmd_ptr->ftm_hdr.cmd_id == FTM_DO_XO_FT_CURVE_CAL )))
  {


	  /* Create response packet */
	  /* Copy common header information from the request to the response */
	  rsp_headers.diag_hdr             = cmd_ptr->diag_hdr;
	  rsp_headers.ftm_hdr.cmd_id       = cmd_ptr->ftm_hdr.cmd_id;
	  rsp_headers.ftm_hdr.cmd_data_len = cmd_ptr->ftm_hdr.cmd_data_len; 

  /* At this point the response packet size equals the size of the headers plus
     the size of the fixed return parameters. If the response contains extra data
     whose size is determined by a parameter in the request, then we add it here. */
	  rsp_pkt = ftmdiag_create_new_pkt(rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len);
	
	  if (rsp_pkt.pkt_payload != NULL)
	  {
		/* copy header information */
		memcpy((void*)rsp_pkt.pkt_payload, 
			   (void*)&rsp_headers, 
			   sizeof(ftm_composite_cmd_header_type));
	
		/* copy fixed return parameters */
		memcpy((void*)((byte*)rsp_pkt.pkt_payload + sizeof(ftm_composite_cmd_header_type)), 
			   (void*)&result,
			   (rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type)));
	
		if (extra_data_len > 0)
		{
		  /* copy extra variable length return data if present */
		  memcpy((void*)((byte*)rsp_pkt.pkt_payload + rsp_headers.ftm_hdr.cmd_rsp_pkt_size), 
				 local_buffer, 
				 extra_data_len);
		}
	  }
	  else
	  {
		FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %d", 
					  cmd_ptr->ftm_hdr.cmd_id, 0, 0);
	  }
	  if (local_buffer != NULL)
	  {
		ftm_free(local_buffer);
	  }
	
	
  }
  return rsp_pkt;
} /* end ftm_common_dispatch */

#endif /* FEATURE_FACTORY_TESTMODE */
