#ifndef SRCH_AFC_H
#define SRCH_AFC_H
/*===========================================================================

             S E A R C H   A F C   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Search AFC.

    Copyright (c) 2004 - 2008
                  by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_afc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/08   adw     Added srch_afc_get_current_vco(), srch_afc_rpush_get_mask().
07/18/08   adw     Renamed afc_tcxo_off_cb_type to afc_freq_track_off_cb_type.
10/09/07   sst     Created function srch_afc_calc_hr_rot_to_ppm
08/21/07   sst     Created function srch_afc_set_rot_thresh
01/23/07   pa/rkc  Added typedef srch_afc_tcxo_off_cb_type.
01/09/07   sst     Merge from 11.02.02B branch
10/31/06   tjc     Fix afc logging for offline qpch
10/13/06   trc     Changes for new STM grammar
06/28/06   pa      Merged srch_afc_calc.c functions and removed
                   srch_afc_tcxo_off/on.
06/06/06   pa      Added support for AFC SM and removed srch_afc.c code.
10/19/05   gs      Add prototype for srch_afc_init_srl
08/11/05   gs/sst  Add prototypes for srch_afc_write_rot_accs_now and
                   srch_afc_set_mode_and_xfer.
07/14/05   sfm     Combined srch_tcxo with srch_afc
05/31/05   rng     Merged from SHDR Sandbox.
03/07/05   sst     Added functions to translate VCO<->Rot<->ppm
02/17/05   gs      Add Prototype for srch_afc_log_flush.
09/20/04   abh     Added SRCH_AFC_FREEZE_AFC_AT_VAL which freezes TRKLO at
                   value passed. srch_afc_freeze_afc_curr_state now returns
                   value at which AFC was frozen.
09/16/04   jcm     Added generalized AFC subpacket
09/16/04   jcm     Moved AFC types to srch_afc_t.h
09/13/04   sst     Added srch_afc overwrite capabilities through diag
09/07/04   sst     Added prototypes for write accs
08/23/04   rng     Added srch_afc_report_rgs().
08/18/04   gs      Add prototype for srch_afc_change_freq_range().
06/30/04   gs      Add gain modes for AFC-1 to enum, and remove constants
                   necessary for NV item.  Add rotator-only mode.
06/21/04   gs      Add functions to make sure rotator error doesn't grow too
                   large in RV-AFC mode, and change srch_afc_set_mode gain
                   type to enum.
06/01/04   gs      Change accumulator values to signed #s.
05/19/04   va      Exported AFC freeze and unfreeze functions
05/04/04   va      Created file.

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "log.h"
#include "msm_drv.h"
#include "srchi.h"
#include "srch_genlog.h"
#include "srch_rx_t.h"
#include "srch_stm.h"
#include "tcxomgr.h"
#include "srch_afc_t.h"


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

typedef struct
{
  int32  carrier_freq;                       /* CDMA channel carrier freq  */
  int32  transmit_freq;                      /* CDMA channel transmit freq */
} srch_afc_chan_freq_struct_type;

typedef void (*srch_afc_freq_track_off_cb_type)(void);

/* Enum to choose the strictness of the rot txfr thresholds */
typedef enum
{
  SRCH_AFC_THRESH_NORMAL,                    /* sleep  = .10-.33ppm,
                                                pullin = .33-.60ppm        */
  SRCH_AFC_THRESH_XO_LOOSE,                  /* sleep  = inf,
                                                pullin = 5ppm              */
  SRCH_AFC_THRESH_XO_INF,                    /* sleep  = inf,
                                                pullin = inf               */
  SRCH_AFC_THRESH_MAX,
} srch_afc_rot_thresh_type;


/*=========================================================================
      FUNCTION DECLARATIONS
==========================================================================*/

/*============================================================================

FUNCTION       SRCH_AFC_INIT

DESCRIPTION    This function initializes the AFC module

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

============================================================================*/
extern void srch_afc_init( void );

/*===========================================================================

FUNCTION       SRCH_AFC_DIAG_WRITE_ACCS

DESCRIPTION    This function writes to the VCO and ROT accumulators.  It is
               used only by DIAG and thus does not require that the caller
               be controlling the TCXO.

               DO NOT call this function from any subsystem other than DIAG.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_diag_write_accs
(
  int16 vco,                                     /* vco accum value        */
  int16 rot                                      /* rotator accum value    */
);

/*===========================================================================

FUNCTION       SRCH_AFC_WRITE_ACCS

DESCRIPTION    If 1X has control of the TCXO, this function writes the values
               specified to the rotator and VCO accumulators.  If 1X does not
               have control of the TCXO, the VCO value is converted to rotator
               units and added to the rotator value before the rotator value
               is written (the VCO accumulator is not written).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
void srch_afc_write_accs
(
  int16 vco,                                     /* vco accum value        */
  int16 rot                                      /* rotator accum value    */
);

/*===========================================================================

FUNCTION       SRCH_AFC_WRITE_ROT_ACC

DESCRIPTION    This function writes given value to the rotator accumulator.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
void srch_afc_write_rot_acc
(
  int16 rot                                      /* rotator accum value    */
);

/*===========================================================================

FUNCTION       SRCH_AFC_UPDATE_PRE_SLEEP_ACC

DESCRIPTION    Updates the pre-sleep value for the accumulators.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
void srch_afc_update_pre_sleep_accs
(
  srch_afc_vco_type vco,                        /* vco accum value        */
  srch_afc_rot_type rot                         /* rotator accum value    */
);

/*===========================================================================

FUNCTION       SRCH_AFC_LOG_FLUSH

DESCRIPTION    This function flushes the log out.  Note that this does NOT log
               a new item.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   If log mask is set, a log record is flushed via diag.

===========================================================================*/
extern void srch_afc_log_flush( void );

/*===========================================================================

FUNCTION       SRCH_AFC_WRITE_MODE_GAINS

DESCRIPTION    This function writes the mode and gains of the AFC.  It is
               used by DIAG only and thus does not require that the caller
               be controlling the TCXO.

               DO NOT call this function from any subsystem other than DIAG.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_write_mode_gains
(
  srch_afc_mode_enum_type config_mode,           /* config mode            */
  uint16                  vco_gain,              /* VCO gain               */
  uint16                  rot_gain               /* rotator gain           */
);

/*===========================================================================

FUNCTION       SRCH_AFC_SET_ROT_THRESH

DESCRIPTION    This function sets the thresholds for ROT txfr to the VCO
               (or XO trim)

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_set_rot_thresh
(
  srch_afc_rot_thresh_type thresh
);

/*===========================================================================

FUNCTION       SRCH_AFC_READ_VCO_ACCUM

DESCRIPTION    This function returns the current value of the frequency
               tracking accumulator.

DEPENDENCIES   srch_dmod_reset should have been previously called.

RETURN VALUE   The accumulator value.

SIDE EFFECTS   None.

===========================================================================*/
int16 srch_afc_read_vco_accum( void );

/*===========================================================================

FUNCTION       SRCH_AFC_GET_CURRENT_VCO

DESCRIPTION    This function gets the VCO value from tcxomgr.

DEPENDENCIES   None.

RETURN VALUE   The VCO value from tcxomgr.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_afc_vco_type srch_afc_get_current_vco( void );

/*===========================================================================

FUNCTION       SRCH_AFC_RPUSH_GET_MASK

DESCRIPTION    This function gets the rpush mask from tcxomgr.

DEPENDENCIES   None. 

RETURN VALUE   The tcxomgr rpush mask.  

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_afc_rpush_get_mask( void );

/*===========================================================================

FUNCTION       SRCH_AFC_BUILD_AFC_SUB

DESCRIPTION    This function builds the AFC sub packet should only be called
               by genlog.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_build_afc_sub
(
  srch_genlog_packet_id_type id,                 /* ID of packet to commit */
  sub_commit_func_type commit_fp                 /* commit function ptr    */
);

/*===========================================================================

FUNCTION       SRCH_AFC_LOG

DESCRIPTION    Save AFC log to a local buffer, and flush the log if buffer is
               full

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_log( void );

/*===========================================================================

FUNCTION       SRCH_AFC_CHANGE_FREQ_RANGE

DESCRIPTION    This function updates the VCO and/or Rotator gains after
               tuning to a new frequency range.  It is used by SRCH_RX only
               and thus does not require the caller to be controlling the TCXO.

               DO NOT call this function from any module other than SRCH_RX

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_change_freq_range
(
  srch_rx_band_type     band,                    /* new band class         */
  srch_rx_chan_type     chan                     /* new CDMA channel       */
);

/*===========================================================================

FUNCTION SRCH_AFC_GET_FREQ_ERROR

DESCRIPTION    Gets the VCO and ROT errors.

               Note that if we don't want to read the vco or rotator error it's
               possible to pass in NULL.

               The units of rot_acc are in 1.1719 Hz per LSB.

               vco_acc is the raw HW read of the VCO loop accumulator, and
               can be converted to Hz by multiplying by the NV_VCO_SLOPE *
               2^(-20) * carrier frequency (in MHz).

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.

===========================================================================*/
void srch_afc_get_freq_error
(
  int16 * vco,                                     /* vco accumulator      */
  int16 * rot                                      /* rotator accumulator  */
);

/*===========================================================================

FUNCTION       SRCH_AFC_REPORT_RGS

DESCRIPTION    This function reports the recent (current) good system info
               to the TCXOMGR.

ARGUMENTS      None.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   TCXOMGR RGS values updated.

===========================================================================*/
extern void srch_afc_report_rgs( void );

#ifdef FEATURE_1X_SRCH_AFC_SRL
/*============================================================================

FUNCTION SRCH_AFC_INIT_SRL

DESCRIPTION
  This function sets the power-up conditions for Slew Rate Limiting...
  specifically initalizing it to "off".

DEPENDENCIES
  MDSP clocks must be on before calling this function

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
extern void srch_afc_init_srl( void );
#endif /* FEATURE_1X_SRCH_AFC_SRL */

/*===========================================================================

FUNCTION       SRCH_AFC_CALC_CHAN_FREQ

DESCRIPTION    This function calculates the band class and channel number
               into the carrier and transmit frequencies

DEPENDENCIES   None.

RETURN VALUE   The Carrier frequency and the Transmit Frequency.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_afc_chan_freq_struct_type srch_afc_calc_chan_freq
(
  srch_rx_band_type  band,                   /* current band class         */
  srch_rx_chan_type  chan                    /* current CDMA channel       */
);

/*===========================================================================

FUNCTION       SRCH_AFC_CALC_ACC_TRANS_FACTORS

DESCRIPTION    This function calculates the translation factors between:
               VCO and ROT values, ROT and ppm values

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_afc_calc_acc_trans_factors
(
  srch_rx_band_type band,                        /* current band class     */
  srch_rx_chan_type chan                         /* current CDMA channel   */
);

/*===========================================================================

FUNCTION       SRCH_AFC_CALC_ROT_TO_VCO

DESCRIPTION    This function translates the Rotator acc error into VCO acc
               units.

DEPENDENCIES   None.

RETURN VALUE   Corresponding Rotator acc error in VCO acc units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_rot_to_vco
(
  int16 rot_acc               /* Rot Acc to translate (1.1719 Hz per unit) */
);

/*===========================================================================

FUNCTION       SRCH_AFC_CALC_VCO_TO_ROT

DESCRIPTION    This function translates the VCO acc error into Rotator acc
               units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
               called.

RETURN VALUE   Corresponding VCO acc error in Rotator acc units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_vco_to_rot
(
  int16 vco_acc              /* VCO Acc to translate (band specific units) */
);

/*===========================================================================

FUNCTION SRCH_AFC_CALC_ROT_TO_PPM

DESCRIPTION    This function translates the Rotator acc error into ppm units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
               called.

RETURN VALUE   Corresponding Rotator acc error in ppm units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_rot_to_ppm
(
  int16 rot_acc               /* Rot Acc to translate (1.1719 Hz per unit) */
);

/*===========================================================================

FUNCTION       SRCH_AFC_CALC_HR_ROT_TO_PPM

DESCRIPTION    This function translates the high resolution Rotator acc error
                into ppm units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
                called.

RETURN VALUE   Corresponding Rotator acc error in HR ppm units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_hr_rot_to_ppm
(
  int16 hr_rot_acc           /* HR Rot Acc to translate
                                (1.1719/4 Hz per unit) */
);

/*===========================================================================

FUNCTION SRCH_AFC_CALC_PPM_TO_ROT

DESCRIPTION    This function translates the Rotator acc error into ppm units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
               called.

RETURN VALUE   Corresponding ppm error in Rotator acc units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_ppm_to_rot
(
  int16 ppm_val                                  /* ppm value to translate */
);

/*===========================================================================

FUNCTION SRCH_AFC_CALC_PPM_TO_VCO

DESCRIPTION    This function translates the PPM error into VCO units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
                called.

RETURN VALUE   Corresponding ppm error in VCO acc units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_ppm_to_vco
(
  int16 ppm_val                                  /* ppm value to translate */
);

/*===========================================================================

FUNCTION SRCH_AFC_CALC_VCO_TO_PPM
DESCRIPTION    This function translates the VCO error into PPM units

DEPENDENCIES   srch_afc_calc_acc_trans_factors() must have been previously
                called.

RETURN VALUE   Corresponding VCO acc error in ppm units.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_afc_calc_vco_to_ppm
(
  int16 vco_acc                                  /* vco value to translate */
);

#endif /* SRCH_AFC_H */
