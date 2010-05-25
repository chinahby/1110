/*===========================================================================

                 F T M   1 X   C A L I B R A T I O N   A P I
                              V E R S I O N   2

DESCRIPTION
  This module consolidates RF calibration and NV management, with
  extensions for IntelliCeiver enabled radios.


Copyright(c) 2004, 2005, 2006 by QUALCOMM, Incorporated. All Rights Reserved.
Copyright(c) 2007, 2008, 2009 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_cal_v2.c#5 $
$DateTime: 2009/01/12 18:56:46 $
$Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/09   vb      Changed the implementation of the function ftm_1x_cal_im2() to 
                   suit IM2 Calibration on QSC1100
12/08/08   vb      Bug fixes in ftm_1x_cal_nv_commit()
10/08/08   sr      Klocwork fixes
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
03/12/07   bmg     RTR6500 updates
03/05/07   vm      Featurize the RF Library Calls until RF library
                   is merged for Common Code 
02/26/07   vm      Merge for Common Archive
01/23/06   bmg     IntelliCeiver port for QSC60x5
09/26/06   hkk     Added support for BC15, with featurization
04/18/06   sar     Fixed compiler error in get_rfr_device_type().
03/31/06   vm      Added support for new APIs for new IM2 cal algorithm
11/30/05   bmg     Added prototype to remove warning.  Fixed type in memset.
                   Added bands 11 and 14 to NV commit.
11/17/05   bmg     Zero-init IntelliCeiver response packet data
09/20/05   bmg     Revert to I/Q = 32/32 for LPM 2 IM2 calibration
08/01/05   hkk     Added RFR6525 support
05/10/05   bmg     Use normal power mode IM2 calibration for LPM 2
05/09/05   bmg     Save IntelliCeiver cal result to NV even in the case of a
                   soft faliure.
04/22/05   bmg     Skip calibration for RFR6500 low power mode 2 unless
                   FTM_RFR6500_CALIBRATE_LPM_2 is turned on
03/31/05   bmg     Use high power mode IM2 cal for mid power mode on RFR6500
03/28/05   bmg     Store IntelliCeiver calibration to RFNV
12/03/04   bmg     Created

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_1x_cal_v2.h"
#include "rflib.h"
#include "ftm_1x_ctl.h"
#include "ftm_1x_nv.h"
#include "ftm_lib.h"
#include "rflib_share.h"
#include "rf_intelliceiver.h"

/*===========================================================================

                              DEFINITIONS

===========================================================================*/

enum
{
  FTM_1X_CAL_COMMIT_NV,
  FTM_1X_CAL_DVGA,
  FTM_1X_CAL_LNA,
  FTM_1X_CAL_IM2,
#ifdef RF_HAS_POWER_MODE_LIB
  FTM_1X_CAL_INTELLICEIVER
#endif /* RF_HAS_POWER_MODE_LIB */
};

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   status;
} ftm_1x_cal_commit_nv_rsp_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   count;
#ifdef RF_HAS_POWER_MODE_LIB
  int2    calibration[RFLIB_MAX_POWER_MODE];
#else
  int2    calibration;
#endif /* RF_HAS_POWER_MODE_LIB */
} ftm_1x_cal_dvga_rsp_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   count;
#ifdef RF_HAS_POWER_MODE_LIB
  int2    calibration[RFLIB_MAX_POWER_MODE];
#else
  int2    calibration;
#endif /* RF_HAS_POWER_MODE_LIB */
} ftm_1x_cal_lna_rsp_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  int2    suppression;
  uint1   count;
#ifdef RF_HAS_POWER_MODE_LIB
  PACKED struct {
    uint1 i;
    uint1 q;
    uint1 transconductor;
  } calibration[RFLIB_MAX_POWER_MODE];
#else
  PACKED struct {
    uint1 i;
    uint1 q;
    uint1 transconductor;
  } calibration;
#endif /* RF_HAS_POWER_MODE_LIB */
} ftm_1x_cal_im2_rsp_type;

#ifdef RF_HAS_POWER_MODE_LIB
typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   success;
  uint1   calibration[128];
} ftm_1x_cal_intelliceiver_rsp_type;
#endif /* RF_HAS_POWER_MODE_LIB */


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

void rf_update_freq_comp_items ( rf_path_enum_type path, word chan );

/*===========================================================================

FUNCTION FTM_1X_CAL_DISPATCH

DESCRIPTION
  Handles all 1x calibration APIs

DEPENDENCIES
   None.

RETURN VALUE
   FTM response structure, with the response returned by the individual
   API handler.

SIDE EFFECTS
   API handlers will update RF NV internal data structures with the
   latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_dispatch( void* req )
{
  ftm_composite_cmd_header_type* header = req;
  ftm_rsp_pkt_type response_packet = {FTM_RSP_BAD_CMD, 0, NULL, FALSE};

  switch (header->ftm_hdr.cmd_id)
  {
  case FTM_1X_CAL_COMMIT_NV:
    response_packet = ftm_1x_cal_nv_commit( (ftm_1x_cal_nv_commit_req_type*) req );
    break;

  case FTM_1X_CAL_DVGA:
    response_packet = ftm_1x_cal_dvga( (ftm_1x_cal_dvga_req_type*) req );
    break;

  case FTM_1X_CAL_LNA:
    response_packet = ftm_1x_cal_lna( (ftm_1x_cal_lna_req_type*) req );
    break;

  case FTM_1X_CAL_IM2:
    response_packet = ftm_1x_cal_im2( (ftm_1x_cal_im2_req_type*) req );
    break;
  
  #ifdef RF_HAS_POWER_MODE_LIB
  case FTM_1X_CAL_INTELLICEIVER:
    response_packet = ftm_1x_cal_intelliceiver( (ftm_1x_cal_intelliceiver_req_type*) req );
    break;
  #endif /* RF_HAS_POWER_MODE_LIB */
  
  default:
    /* Do nothing, return default bad command response */
    break;
  }

  return response_packet;
}


/*===========================================================================

FUNCTION FTM_1X_CAL_NV_COMMIT

DESCRIPTION
  Write RX calibration to NV

DEPENDENCIES
   None.

RETURN VALUE
   FTM response structure, with FTM_RSP_DO_LEGACY set.

SIDE EFFECTS
   All supported RX calibration NV items are written to NV.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_nv_commit( ftm_1x_cal_nv_commit_req_type* req )
{
  ftm_rsp_pkt_type rsp;
  ftm_1x_cal_commit_nv_rsp_type *cal_nv_commit_ptr;

  if (req->band == 0  ||
      req->band == 1  ||
      req->band == 3  ||
      req->band == 4  ||
      req->band == 5  ||
      req->band == 6  ||
      req->band == 11 ||
      req->band == 14 ||
      req->band == 15 
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
                       ||
      req->band == 128 ||
      req->band == 129 
#endif
      )
  {
    rsp = ftmdiag_create_new_pkt( sizeof(ftm_1x_cal_commit_nv_rsp_type) );
    cal_nv_commit_ptr = rsp.pkt_payload;
    cal_nv_commit_ptr->cmd = req->cmd;
    cal_nv_commit_ptr->sub_sys = req->sub_sys;
    cal_nv_commit_ptr->mode = req->mode;
    cal_nv_commit_ptr->ftm_cmd = req->ftm_cmd;
    cal_nv_commit_ptr->req_len = req->req_len;
    cal_nv_commit_ptr->rsp_len = rsp.pkt_len;

    cal_nv_commit_ptr->status = ftm_1x_write_band_nv(req->band);
  }
  else
  {
    rsp.pkt_len = 0;
    rsp.pkt_payload = 0; // initialized to satisfy KW
    rsp.delete_payload = 0;

    rsp.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_ERROR("Unsupported band (%d) in NV Commit API", req->band, 0, 0);
  }
  
  return rsp;
}


/*===========================================================================

FUNCTION FTM_1X_CAL_DVGA

DESCRIPTION
  Performes DVGA calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   A CDMA waveform with Ior equal to the passsed parameter <power>
   must be applied to the radio, on the correct band and channel.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_dvga( ftm_1x_cal_dvga_req_type* req )
{
  ftm_rsp_pkt_type rsp;
  ftm_1x_cal_dvga_rsp_type *cal_dvga_rsp_ptr;
  int chan_index;
  rfnv_digital_items_type *rfnv_ptr;
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode;
#endif /* RF_HAS_POWER_MODE_LIB */

  if ( req->path > 1 )
  {
    rsp.pkt_len = 0;
    rsp.pkt_payload = 0; // initialized to satisfy KW
    rsp.delete_payload = 0;

    FTM_MSG_ERROR("Unsupported path (%d) in DVGA cal", req->path, 0, 0);
    rsp.cmd = FTM_RSP_BAD_PARM;
    return rsp;
  }

  /* When calibrating diversity path in diversity mode, the rf_chain_status
     structure for the second path does not get updated correctly.  In
     this case, we need to use the channel from the primary path instead. */
  if ( req->path == RF_PATH_1 &&
       rf_chain_status[req->path].rf_state == RF_STATE_DIVERSITY )
  {
    chan_index = ftm_1x_add_cal_chan( req->path, rf_chain_status[0].rf_chan );
  }
  else
  {
    chan_index = ftm_1x_add_cal_chan( req->path, rf_chain_status[req->path].rf_chan );
  }

  rfnv_ptr = rf_chain_status[req->path].rf_nv_dig_items_ptr;

  rsp = ftmdiag_create_new_pkt( sizeof(ftm_1x_cal_dvga_rsp_type) );
  cal_dvga_rsp_ptr = rsp.pkt_payload;
  cal_dvga_rsp_ptr->cmd = req->cmd;
  cal_dvga_rsp_ptr->sub_sys = req->sub_sys;
  cal_dvga_rsp_ptr->mode = req->mode;
  cal_dvga_rsp_ptr->ftm_cmd = req->ftm_cmd;
  cal_dvga_rsp_ptr->req_len = req->req_len;
  cal_dvga_rsp_ptr->rsp_len = rsp.pkt_len;

#ifdef RF_HAS_POWER_MODE_LIB
  if ( rf_intelliceiver_is_enabled(req->path) )
  {
    for ( power_mode = RFLIB_NORMAL_POWER_MODE;
          power_mode <= rf_intelliceiver_get_lowest_power_mode(req->path);
          power_mode++ )
    {
      ftm_set_rf_power_mode( req->path, power_mode );
      cal_dvga_rsp_ptr->calibration[power_mode]
        = ftm_get_dvga_offset( req->path, req->power );

      if (chan_index >= NV_FREQ_TABLE_SIZ)
      {
          rsp.pkt_len = 0;
          rsp.pkt_payload = 0; // initialized to satisfy KW
          rsp.delete_payload = 0;

          FTM_MSG_ERROR("Trying to access an element that is out-of-bounds - max array size = %d",
                        NV_FREQ_TABLE_SIZ, 0, 0);
          rsp.cmd = FTM_RSP_BAD_PARM;
          return rsp;
      }
      rfnv_ptr->rfnv_vga_gain_offset_vs_freq[req->path][power_mode][chan_index]
        = cal_dvga_rsp_ptr->calibration[power_mode];
    }
    cal_dvga_rsp_ptr->count = rf_intelliceiver_get_lowest_power_mode(req->path) + 1;
  } /* if ( req->path == RF_PATH_0 ) */
  else
  {
    cal_dvga_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE]
      = ftm_get_dvga_offset( req->path, req->power );
    cal_dvga_rsp_ptr->count = 1;

    if (chan_index >= NV_FREQ_TABLE_SIZ)
    {
        rsp.pkt_len = 0;
        rsp.pkt_payload = 0; // initialized to satisfy KW
        rsp.delete_payload = 0;

        FTM_MSG_ERROR("Trying to access an element that is out-of-bounds - max array size = %d",
                      NV_FREQ_TABLE_SIZ, 0, 0);
        rsp.cmd = FTM_RSP_BAD_PARM;
        return rsp;
    }

    rfnv_ptr->rfnv_vga_gain_offset_vs_freq[req->path][RFLIB_NORMAL_POWER_MODE][chan_index]
      = cal_dvga_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE];
  } /* else if ( rf_intelliceiver_is_enabled(req->path) ) */
#else
  cal_dvga_rsp_ptr->calibration
    = ftm_get_dvga_offset( req->path, req->power );
  cal_dvga_rsp_ptr->count = 1;

  rfnv_ptr->rfnv_vga_gain_offset_vs_freq[req->path][chan_index]
    = cal_dvga_rsp_ptr->calibration;
#endif /* RF_HAS_POWER_MODE_LIB */

  /* Update the frequency comp structure so later LNA offsets will
     be computed with the correct DVGA gain when you switch between
     RF power modes */
  rf_freq_comp.last_chan_checked[req->path] = (uint16)-1;
  rf_freq_comp.last_band_checked[req->path] = (uint8)-1;

  #ifdef RF_HAS_SUB_BLOCK_SUPPORT
  rf_freq_comp.last_block_mask_checked[req->path] = 0;
  #endif
  
  rf_update_freq_comp_items( req->path, rf_chain_status[req->path].rf_chan );

  return rsp;
} /* ftm_1x_cal_dvga */


/*===========================================================================

FUNCTION FTM_1X_CAL_LNA

DESCRIPTION
  Performes LNA offset calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   A CDMA waveform with Ior equal to the passsed parameter <power>
   must be applied to the radio, on the correct band and channel.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_lna( ftm_1x_cal_lna_req_type* req )
{
  ftm_rsp_pkt_type rsp;
  ftm_1x_cal_lna_rsp_type *cal_lna_rsp_ptr;
  int chan_index;
  rfnv_digital_items_type *rfnv_ptr;
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode;
#endif /* RF_HAS_POWER_MODE_LIB */

  if (req->path > 1 || req->lna_step > 3)
  {
    rsp.pkt_len = 0;
    rsp.pkt_payload = 0; // initialized to satisfy KW
    rsp.delete_payload = 0;

    FTM_MSG_ERROR("Unsupported path (%d) or LNA step (%d) in LNA cal",
                  req->path, req->lna_step, 0);
    rsp.cmd = FTM_RSP_BAD_PARM;
    return rsp;
  }

  if ( req->path == RF_PATH_1 &&
       rf_chain_status[req->path].rf_state == RF_STATE_DIVERSITY )
  {
    chan_index = ftm_1x_add_cal_chan( req->path, rf_chain_status[0].rf_chan );
  }
  else
  {
    chan_index = ftm_1x_add_cal_chan( req->path, rf_chain_status[req->path].rf_chan );
  }
  
  if (chan_index >= NV_FREQ_TABLE_SIZ)
  {
      rsp.pkt_len = 0;
      rsp.pkt_payload = 0; // initialized to satisfy KW
      rsp.delete_payload = 0;

      FTM_MSG_ERROR("Trying to access an element that is out-of-bounds - max array size = %d",
                    NV_FREQ_TABLE_SIZ, 0, 0);
      rsp.cmd = FTM_RSP_BAD_PARM;
      return rsp;
  }

  rfnv_ptr = rf_chain_status[req->path].rf_nv_dig_items_ptr;

  rsp = ftmdiag_create_new_pkt(sizeof(ftm_1x_cal_lna_rsp_type));
  cal_lna_rsp_ptr = rsp.pkt_payload;
  cal_lna_rsp_ptr->cmd = req->cmd;
  cal_lna_rsp_ptr->sub_sys = req->sub_sys;
  cal_lna_rsp_ptr->mode = req->mode;
  cal_lna_rsp_ptr->ftm_cmd = req->ftm_cmd;
  cal_lna_rsp_ptr->req_len = req->req_len;
  cal_lna_rsp_ptr->rsp_len = rsp.pkt_len;

#ifdef RF_HAS_POWER_MODE_LIB
  if ( rf_intelliceiver_is_enabled(req->path) )
  {
    for ( power_mode = RFLIB_NORMAL_POWER_MODE;
          power_mode <= rf_intelliceiver_get_lowest_power_mode(req->path);
          power_mode++ )
    {
      ftm_set_rf_power_mode( req->path, power_mode );
      cal_lna_rsp_ptr->calibration[power_mode]
        = ftm_get_lna_offset(req->path, req->lna_step, req->power);

      switch (req->lna_step)
      {
      case 0:
        rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[req->path][power_mode][chan_index]
          = cal_lna_rsp_ptr->calibration[power_mode];
        break;

      case 1:
        rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[req->path][power_mode][chan_index]
          = cal_lna_rsp_ptr->calibration[power_mode];
        break;

      case 2:
        rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[req->path][power_mode][chan_index]
          = cal_lna_rsp_ptr->calibration[power_mode];
        break;

      case 3:
        rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[req->path][power_mode][chan_index]
          = cal_lna_rsp_ptr->calibration[power_mode];
        break;

      default:
        ERR_FATAL("Invalid LNA offset step (%d)", req->lna_step, 0, 0);
      }
    }
    cal_lna_rsp_ptr->count = rf_intelliceiver_get_lowest_power_mode(req->path) + 1;
  } /* if ( rf_intelliceiver_is_enabled(req->path) ) */
  else
  {
    cal_lna_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE]
      = ftm_get_lna_offset(req->path, req->lna_step, req->power);

    switch (req->lna_step)
    {
    case 0:
      rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[req->path][RFLIB_NORMAL_POWER_MODE][chan_index]
        = cal_lna_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE];
      break;

    case 1:
      rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[req->path][RFLIB_NORMAL_POWER_MODE][chan_index]
        = cal_lna_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE];
      break;

    case 2:
      rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[req->path][RFLIB_NORMAL_POWER_MODE][chan_index]
        = cal_lna_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE];
      break;

    case 3:
      rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[req->path][RFLIB_NORMAL_POWER_MODE][chan_index]
        = cal_lna_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE];
      break;

    default:
      ERR_FATAL("Invalid LNA offset step (%d)", req->lna_step, 0, 0);
    }
    cal_lna_rsp_ptr->count = 1;
  } /* else if ( rf_intelliceiver_is_enabled(req->path) ) */
#else
  cal_lna_rsp_ptr->calibration
    = ftm_get_lna_offset(req->path, req->lna_step, req->power);
  cal_lna_rsp_ptr->count = 1;

  switch (req->lna_step)
  {
  case 0:
    rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[req->path][chan_index]
      = cal_lna_rsp_ptr->calibration;
    break;
  
  case 1:
    rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[req->path][chan_index]
      = cal_lna_rsp_ptr->calibration;
    break;
  
  case 2:
    rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[req->path][chan_index]
      = cal_lna_rsp_ptr->calibration;
    break;
  
  case 3:
    rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[req->path][chan_index]
      = cal_lna_rsp_ptr->calibration;
    break;

  default:
    ERR_FATAL("Invalid LNA offset step (%d)", req->lna_step, 0, 0);
  }
#endif /* RF_HAS_POWER_MODE_LIB */
  
  return rsp;
} /* ftm_1x_cal_lna */


/*===========================================================================

FUNCTION FTM_1X_CAL_IM2

DESCRIPTION
  Performes IM2 calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   An IM2 calibration signal must be applied to the radio at
   the correct level and offset.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_im2( ftm_1x_cal_im2_req_type* req )
{
  ftm_rsp_pkt_type rsp;
  ftm_1x_cal_im2_rsp_type *cal_im2_rsp_ptr;
  im2_type im2_data;
  rfnv_digital_items_type *rfnv_ptr;
  boolean im2_cal_result;
  
  if (req->path > 1)
  {
    rsp.pkt_len = 0;
    rsp.pkt_payload = 0; // initialized to satisfy KW
    rsp.delete_payload = 0;

    FTM_MSG_ERROR("Unsupported path (%d) in IM2 cal", req->path, 0, 0);
    rsp.cmd = FTM_RSP_BAD_PARM;
    return rsp;
  }

  rfnv_ptr = rf_chain_status[req->path].rf_nv_dig_items_ptr;
  
  rsp = ftmdiag_create_new_pkt(sizeof(ftm_1x_cal_im2_rsp_type));
  cal_im2_rsp_ptr = rsp.pkt_payload;
  cal_im2_rsp_ptr->cmd = req->cmd;
  cal_im2_rsp_ptr->sub_sys = req->sub_sys;
  cal_im2_rsp_ptr->mode = req->mode;
  cal_im2_rsp_ptr->ftm_cmd = req->ftm_cmd;
  cal_im2_rsp_ptr->req_len = req->req_len;
  cal_im2_rsp_ptr->rsp_len = rsp.pkt_len;

#ifdef RF_HAS_POWER_MODE_LIB
  if ( rf_intelliceiver_is_enabled(req->path) )
  {
    /* Calibrate in low power mode */
    ftm_set_rf_power_mode( req->path, RFLIB_LOW_POWER_MODE_1 );

    /*
      Check the return status of get_im2_with_suppression.  If it failed to
      find a valid point, return obviously out of range data, but do it
      deterministically.
    */
#ifdef FTM_USES_NEW_IM2_SEARCH_METHOD
    im2_cal_result = ftm_get_im2_with_suppression(req->path, &im2_data);
#else
    im2_cal_result = ftm_get_legacy_im2_with_suppression(req->path, &im2_data);
#endif

    /*Return railed values if the alogrithm failed */
    if (im2_cal_result == FALSE)
    {
      im2_data.i_dac = 255;
      im2_data.q_dac = 255;
      im2_data.transconductor = 255;
      im2_data.suppression_level = -32768;
    }

    /* Only record suppression for low power mode 1*/
    cal_im2_rsp_ptr->suppression = im2_data.suppression_level;

    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].i = im2_data.i_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].q = im2_data.q_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].transconductor
      = im2_data.transconductor;

    rfnv_ptr->rfnv_im2_i_value[req->path][RFLIB_LOW_POWER_MODE_1]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].i;
    rfnv_ptr->rfnv_im2_q_value[req->path][RFLIB_LOW_POWER_MODE_1]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].q;
    rfnv_ptr->rfnv_im2_transconductor_value[req->path][RFLIB_LOW_POWER_MODE_1]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_1].transconductor;


    /* Values obtained from IM2 calibration in low power mode 1 are copied 
       to high power mode*/
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].i = im2_data.i_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].q = im2_data.q_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].transconductor
      = im2_data.transconductor;

    rfnv_ptr->rfnv_im2_i_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].i;
    rfnv_ptr->rfnv_im2_q_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].q;
    rfnv_ptr->rfnv_im2_transconductor_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].transconductor;


    /* Values obtained from IM2 calibration on low power mode 1 are copied 
       to low power mode 2*/
    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].i = im2_data.i_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].q = im2_data.q_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].transconductor
      = im2_data.transconductor;

    rfnv_ptr->rfnv_im2_i_value[req->path][RFLIB_LOW_POWER_MODE_2]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].i;
    rfnv_ptr->rfnv_im2_q_value[req->path][RFLIB_LOW_POWER_MODE_2]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].q;
    rfnv_ptr->rfnv_im2_transconductor_value[req->path][RFLIB_LOW_POWER_MODE_2]
      = cal_im2_rsp_ptr->calibration[RFLIB_LOW_POWER_MODE_2].transconductor;

    cal_im2_rsp_ptr->count = rf_intelliceiver_get_lowest_power_mode(req->path) + 1;
  } /* if ( rf_intelliceiver_is_enabled(req->path) ) */
  else
  {
    /*
      Check the return status of get_im2_with_suppression.  If it failed to
      find a valid point, return obviously out of range data, but do it
      deterministically.
    */
#ifdef FTM_USES_NEW_IM2_SEARCH_METHOD
    im2_cal_result = ftm_get_im2_with_suppression(req->path, &im2_data);
#else
    im2_cal_result = ftm_get_legacy_im2_with_suppression(req->path, &im2_data);
#endif
    if (im2_cal_result == FALSE)
    {
      im2_data.i_dac = 255;
      im2_data.q_dac = 255;
      im2_data.transconductor = 255;
      im2_data.suppression_level = -32768;
    }
    cal_im2_rsp_ptr->suppression = im2_data.suppression_level;
  
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].i = im2_data.i_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].q = im2_data.q_dac;
    cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].transconductor
      = im2_data.transconductor;
    cal_im2_rsp_ptr->count = 1;
  
    rfnv_ptr->rfnv_im2_i_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].i;
    rfnv_ptr->rfnv_im2_q_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].q;
    rfnv_ptr->rfnv_im2_transconductor_value[req->path][RFLIB_NORMAL_POWER_MODE]
      = cal_im2_rsp_ptr->calibration[RFLIB_NORMAL_POWER_MODE].transconductor;
  } /* else if ( rf_intelliceiver_is_enabled(req->path) ) */
#else

#ifdef FTM_USES_NEW_IM2_SEARCH_METHOD
    im2_cal_result = ftm_get_im2_with_suppression(req->path, &im2_data);
#else
    im2_cal_result = ftm_get_legacy_im2_with_suppression(req->path, &im2_data);
#endif
  if (im2_cal_result == FALSE)
  {
    im2_data.i_dac = 255;
    im2_data.q_dac = 255;
    im2_data.transconductor = 255;
    im2_data.suppression_level = -32768;
  }
  
  cal_im2_rsp_ptr->suppression = im2_data.suppression_level;
  cal_im2_rsp_ptr->calibration.i = im2_data.i_dac;
  cal_im2_rsp_ptr->calibration.q = im2_data.q_dac;
  cal_im2_rsp_ptr->calibration.transconductor = im2_data.transconductor;
  cal_im2_rsp_ptr->count = 1;

  rfnv_ptr->rfnv_im2_i_value[req->path] = cal_im2_rsp_ptr->calibration.i;
  rfnv_ptr->rfnv_im2_q_value[req->path] = cal_im2_rsp_ptr->calibration.q;
  rfnv_ptr->rfnv_im2_transconductor_value[req->path] = cal_im2_rsp_ptr->calibration.transconductor;
#endif /* RF_HAS_POWER_MODE_LIB */

  return rsp;
} /* ftm_1x_cal_im2 */


#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

FUNCTION FTM_1X_CAL_INTELLICEIVER

DESCRIPTION
  Performes IntelliCeiver calibration

DEPENDENCIES
  The radio must be tuned to a band and channel, and the proper chipset
  dependent IntelliCiever calibration signal applied to the antenna
  input.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_intelliceiver( ftm_1x_cal_intelliceiver_req_type* req )
{
  ftm_rsp_pkt_type rsp;
  ftm_1x_cal_intelliceiver_rsp_type *cal_intelliceiver_rsp_ptr;
  rfnv_digital_items_type *rfnv_ptr;

  rfnv_ptr = rf_chain_status[req->path].rf_nv_dig_items_ptr;
  
  if ( !rf_intelliceiver_is_enabled(req->path) )
  {
    rsp.pkt_len = 0;
    rsp.pkt_payload = 0; // initialized to satisfy KW
    rsp.delete_payload = 0;

    FTM_MSG_ERROR("Unsupported RFR on path %d in IntelliCeiver cal", req->path, 0, 0);
    rsp.cmd = FTM_RSP_BAD_PARM;
    return rsp;
  }

  rsp = ftmdiag_create_new_pkt(sizeof(ftm_1x_cal_intelliceiver_rsp_type));
  cal_intelliceiver_rsp_ptr = rsp.pkt_payload;
  cal_intelliceiver_rsp_ptr->cmd = req->cmd;
  cal_intelliceiver_rsp_ptr->sub_sys = req->sub_sys;
  cal_intelliceiver_rsp_ptr->mode = req->mode;
  cal_intelliceiver_rsp_ptr->ftm_cmd = req->ftm_cmd;
  cal_intelliceiver_rsp_ptr->req_len = req->req_len;
  cal_intelliceiver_rsp_ptr->rsp_len = rsp.pkt_len;
  memset((byte*)cal_intelliceiver_rsp_ptr->calibration, 0,
         sizeof(cal_intelliceiver_rsp_ptr->calibration));

  if ( ftm_calibrate_intelliceiver( RFCOM_TRANSCEIVER_0, req->power,
                                    cal_intelliceiver_rsp_ptr->calibration,
                                    sizeof(cal_intelliceiver_rsp_ptr->calibration) ) )
  {
    cal_intelliceiver_rsp_ptr->success = 1;
  }
  else
  {
    cal_intelliceiver_rsp_ptr->success = 0;
  }

  /* Save the IntelliCeiver cal into RFNV */
  /* Save no matter what the success value is, if it failed high
     or low the returned value will be railed at whatever is closest
     to the correct calibration point */
  memcpy( rfnv_ptr->rfnv_intelliceiver_cal[req->path],
          (uint8*)cal_intelliceiver_rsp_ptr->calibration,
          sizeof(rfnv_ptr->rfnv_intelliceiver_cal[req->path]) );

  return rsp;
} /* ftm_1x_cal_intelliceiver */
#endif /* RF_HAS_POWER_MODE_LIB */

#endif /* FEATURE_FACTORY_TESTMODE */
