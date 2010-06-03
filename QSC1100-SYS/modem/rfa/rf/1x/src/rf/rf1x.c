/*===========================================================================

                      RF Unit Header File

DESCRIPTION
  This header file contains all the definitions necessary in order to
  interface with the RF Unit.  The RF unit provides interface calls
  for accessing the analog common card and PCN applique synthesizers
  and other miscellaneous items.

Copyright (c) 2003, 2004, 2005     by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2006, 2007           by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf1x.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   vm      Added Support for supporting Single chain on QSC1100
12/11/07   ems     Support for QSC1100
12/04/07   vm      Support for HDR/1X ACESS Probe and PA Switchpoint backoff
08/10/07   vm      Added the support for programming different PA switchpoints
                   and PA hysterisis timer during Access Probes different from 
                   traffic
08/05/03   vm      Added Interface API for programming of PA Hysterisis
                   timer by Encoder
03/15/07   sar     Remove rfm_get_hdet function from the file.
03/13/07   sar     Added rfm_get_hdet function.
03/01/07   sar     Removed compile warnings.
01/23/07   ra      Move rfm_trigger_gps_bbc_update() to rfm file
01/11/07   ra      Added support for rfm_trigger_gps_bbc_update()
12/18/06   jfc     Moved rfm_is_sleeping to RFM layer
12/16/06   jfc     Moved antenna API to RFM layer
11/14/06   ra      continue cleanup for CGPS
11/07/06   ra      start cleanup for CGPS
10/31/06   ycl     Change return type for rf_synth_gps_IF_type to unit8 for
                   rfm_get_gps_IF_config() function.
09/11/06   kss     Modified rfm_hdr_start_agc_acq() call (param change).
08/16/06   kss     Modified call to rfm_hdr_start_agc_acq() (new arg. added). 
08/01/06   mt      Modified rfm_hdr_start_agc_acq() function. 
06/20/06   ra      Added rfm_hdr_reva_notify_tx_pwr_backoff()
04/18/06   bmg     Integrated MS-based GPS API into mainline
03/07/06   hxw     Added RFM_GPS_READ_AGC_DC_OFFSET() to read AGC and DC I/Q offset.
02/07/06   bmg     Added rfm_prepare_to_sleep() API for DC offset mangement
                   problems when disabling RTC but leaving RF enabled
01/19/06   sar     Removed featurization from call to rf_is_sleeping().
01/02/06   dyc     Lint errors updates.
12/14/05   sar     Eliminated lint errors.
11/30/05   sar     Removed featurization from prototype rf_is_sleeping().
                   Added functions rf_freeze/unfreeze_cagc().
11/29/05   hkk     Removed FEATURE_SRCH_PCG for fixing resetting during MSB/MSA session.
11/08/05   sar     Added functions rfm_hdr_log_rx_tx_agc and rfm_hdr_log_tx_agc.
10/14/05   sar     Created file and transferred implementations from rf1x.h.
                   Keeping comment history from the rf1x.h for reference.
10/12/05   sar     Eliminated unused definitions and functions.
10/07/05   bmg     Added RF API rfm_has_high_sinr() for HDR modem init
10/05/05   sar     Added definition for struct rf_carrier_freq.
10/05/05   sar     Added functions rfm_get_tx_carrier_freq() & 
                   rfm_get_rx_carrier_freq.
09/28/05   dyc     Wrap rfm_init_digital_band_chan() with RF_HAS_WU_OPTIMIZED
09/16/05   dyc     Added return value to rfm_init_digital_band_chan()
08/19/05   ra      Added rfm_bypass_rtc_ctl_path_on_off
05/19/05   sar     Eliminated functions that are not referenced by client apps. 
05/08/05   sar     Added APIs to query Band Subclass and Antenna Attributes.
04/26/05   SAR     Moved function rfm_get_band_mask() to rfm.h.
04/18/05   zg      Added new API rfm_get_gps_IF_config().
11/04/04   dbc     Moved PA Backoff types form rf.h to here.  Added rfm versions 
                   of TX PA Backoff module interface functions
10/31/04   dyc     Added rfm_get_unused_synth_lock_time() and rfm_get_max_synth_lock_time()
09/20/04   dyc/ab  rfm_pre_tune_to_gps() freq error parameter
08/10/04   sar     Updating rfm_get_band_chan() to call rf_get_band_chan_temp()
                   to support both chains.
08/06/04   sar     Temporarily forcing rfm_get_band_chan() to return 
                   chain 0 band and chan only.
08/04/04   sar     Added macro for rfm_get_band_chan().
07/13/04   ic      Fixed RCVT 2.1 compiler warnings ('statement unreachable')
07/02/04   sar     fixed return type in function rfm_get_path_cdma_rx_agc_full_scale().
06/28/04   dyc     Remove diversity check for rfm_tune_band_chan()
02/23/04   kss     Use RF_HAS_HDR instead of FEATURE_HDR.
02/11/04   dyc     Updated type for rf_init_delay()
12/17/03   dyc     Allow device param to be passed to rf_complete_sleep_warmup() in
                   rfm_complete_sleep_warmup().
12/01/03   sar     Relocated extern definitions from inside the functions to
                   the top of the file and made other minor changes to eliminate
                   warnings.
10/31/03   dyc     Update rfm_stay_asleep_tcxo_pdm_on() to use rf_set_stay_alseep()
10/20/03   dyc     Remove FEATURE_HWTC
10/17/03   zg      Fixed path checking in GPS pre/post functions.
10/17/03   sar     Added macro for rfm_get_path_cdma_rx_agc_full_scale().
10/01/03   dyc     Remove HDR related ifdefs around rfm_is_sleeping()
09/19/03   sar     Changed rfm_get_frq_trk_acc() function to return path 0
                   value for all devices.
09/17/03   dyc     Added rfm_complete_sleep_warmup()
09/17/03   sar     Modified gps pretune and post tune functions to correctly
                   handle the different path states.
09/13/03   dyc     Added rfm_get_tx_warmup() API
09/12/03   sar     Added device parameter to rf driver functions in :
                   rfm_stay_asleep_tcxo_pdm_on(), rfm_post_cdma_reacq(),
                   rfm_cdma_do_temp_comp(), rfm_get_lna_gain_state(),
                   rfm_set_pa_offsets(), rfm_log_ext_agc() and
                   rfm_is_band_chan().
09/02/03   sar     Deleted API macros not referenced by client applications.
                   Added case statements in function macros that do not utilize
                   device parameter to envoke rf driver call only when device is
                   primary transceiver. Changed rfm_get_cdma_rx_agc() to call
                   rf driver with device parameter.
08/26/03   sar     passing the added parameter of rfcom_device_enum_type in
                   rf_notify_sch_data_rate().
07/11/03   sar     Added definition for rfm_is_sleeping().
07/11/03   sar     Added support for multipath chipset drivers.
02/14/03   sar     Added definitions for rfm_init_digital_band_chan() and
                   rfm_store_1x_sleep_params();
02/13/03   sar     Added definitions for rf_cagc_dynamic_range_type and
                   rf_cagc_pwr_rails_type
02/12/03   sar     Moved common 1x definitions from rf.h

===========================================================================*/
#include "rf.h"
#include "rf1x.h"

#ifdef RF_HAS_GPS
#error code not present
#endif

#ifdef RF_HAS_EGPS
#error code not present
#endif /* RF_HAS_GPS */

#ifdef RF_HAS_CGPS
#error code not present
#endif

#ifdef FEATURE_MULTIMODE_ARCH
/*===========================================================================

FUNCTION RFM_COMPLETE_WARMUP

DESCRIPTION
  This function completes warm up the RF circuitry.  PDM I/Q DC offset
  and TRK_LO_ADJ loops should have settled to their pre-sleep
  values.  When switched back to closed loop CDMA mode, there should
  not be a large transition.

DEPENDENCIES
  rf_init_digital() or rf_init_digital_band_chan() must have been called earlier.
  CDMA mode enabled and the synthesizers should be in lock.
  Called after wakeup and when the real time counter is running.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_complete_warmup(rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        rf_complete_warmup();
        break;
    default:
        break;
    }
}

/*===========================================================================

FUNCTION RFM_COMPLETE_SLEEP_WARMUP

DESCRIPTION
  This function completes warm up the RF circuitry after sleep wakeup.
  This is used when there is a requirement to separate the enabling of
  the dc and rxagc acq loops to after when the rtc is running (i.e. cx8
  wakeup int).  This is necessary because the rxf/rxagc block uses the
  rtc for its internal timings.

  Typically called from srch as last RF call during wakeup processing.

DEPENDENCIES
  Must be called after rf_init_digital().
  The real time counter must be running.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_complete_sleep_warmup(rfcom_device_enum_type device)
{
    rf_complete_sleep_warmup( device );
}
/*===========================================================================

FUNCTION RFM_TX_ENABLE

DESCRIPTION
  This function programs the VHF transmit synthesizer for analog
  or digital (CDMA or PCN) mode of operation.

  Once rfapi_tx_enable has been called, a delay of RF_TUNE_CHAN_WAIT
  milliseconds must be performed by the application to allow the
  VHF transmit synthesizer to settle.  After the VHF transmit sythesizer
  has settled, only then should the PA be enabled.

DEPENDENCIES
  database item RF_MODE must be set
  Procedure rf_enable_tx_pwr() must have been called before calling
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  rfm_tx_enable(rfcom_device_enum_type device)
{
    switch (device) 
    {
      case RFCOM_TRANSCEIVER_0:
      case RFCOM_RECEIVER_1:
      default:
        rf_tx_enable(RFCOM_TRANSCEIVER_0);
      break;
    }
}
/*===========================================================================

FUNCTION RFM_TX_DISABLE

DESCRIPTION
  This function turns off the VHF transmit synthesizer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  rfm_tx_disable(rfcom_device_enum_type device)
{
    switch (device) 
    {
      case RFCOM_TRANSCEIVER_0:
      case RFCOM_RECEIVER_1:
      default:
        rf_tx_disable(RFCOM_TRANSCEIVER_0);
      break;
    }
 }
/*===========================================================================

FUNCTION RFM_ENABLE_TX_PWR

DESCRIPTION
  This function turns on the transmit portion of the RF hardware.

  Once rf_enable_tx_pwr() has been called, a delay of RF_ENABLE_PWR_WAIT
  milliseconds must be performed by the application to allow the RF
  hardware to settle.  After the RF hardware has settled, only then should
  the rf_tx_enable() function be called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_enable_tx_pwr(rfcom_device_enum_type device)
{
    switch (device) 
    {
      case RFCOM_TRANSCEIVER_0:
      case RFCOM_RECEIVER_1:
      default:
        rf_enable_tx_pwr(RFCOM_TRANSCEIVER_0);
      break;
    }

}
#if defined(RF_HAS_PA_ACCESS_PROBE_CONFIG)
/*===========================================================================

FUNCTION RFM_SET_PA_ACCESS_PROBE_CONFIG

DESCRIPTION
This function is called by Mux Layer to infrorm the RF Driver
that its going to be in traffic state.
This is done so that RF Driver can program different PA hysterisis timer
and PA switchpoints for Access Probes different from traffic.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rfm_set_pa_access_probe_config(rfcom_device_enum_type device ,boolean is_traffic_state)
{
  rf_set_pa_access_probe_config(device,is_traffic_state);
}

#ifdef RF_HAS_HDR
#error code not present
#endif
#endif
/*===========================================================================

FUNCTION RFM_GET_CDMA_RX_AGC

DESCRIPTION
  This function returns the CDMA receive AGC setting in 8 bit two's
  complement format.

  WITH Rx LINEARIZER SETUP FOR -106dBm..-20.75dBm, the Rx signal strength is:

    Rx strength in dBm = -63.248 + (( rf_get_cdma_rx_agc() * 334 ) / 1000 )

    That's -0.334dB per count referenced to -63.248dBm.  Since this returns
    int1, this is a numeric Rx signal strength from -106dBm to -20.83dBm.

DEPENDENCIES
  None

RETURN VALUE
  int1 - 8 bit two's complement CDMA Rx AGC value

SIDE EFFECTS
  None

===========================================================================*/
int1 rfm_get_cdma_rx_agc(rfcom_device_enum_type device)
{
    return rf_get_path_cdma_rx_agc(device);
}
/*===========================================================================

FUNCTION RFM_GET_CDMA_TX_PWR_LIMIT

DESCRIPTION
  This function returns the CDMA Tx power limit setting in 10 bit binary
  format.  Since the hardware register is not directly readable, this
  functions returns a copy of the last setting performed by the RF unit.
  The returned value is interpreted as follows:

    Returned value               0........1023
    Tx power limit          -52.25........+31dBm
    Tx power limit register   +511........-512

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx power limit value

SIDE EFFECTS
  None

===========================================================================*/
word rfm_get_cdma_tx_pwr_limit(rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_cdma_tx_pwr_limit();
    default:
        break;
    }
    return 0;
}
/*===========================================================================

FUNCTION RFM_POST_CDMA_REACQ                             EXTERNALIZED FUNCTION

DESCRIPTION
  Perform RF initialization required following CDMA pilot re-acquisition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_post_cdma_reacq(   rfcom_device_enum_type device )
{
    rf_post_cdma_reacq(device);
}
/*===========================================================================

FUNCTION RFM_CDMA_TEMP_COMP_INIT                             INTERNAL FUNCTION

DESCRIPTION
  This function simply saves the address of the timer block.  This address
  is passed by a task.  There is an understanding between this task and
  us (RF driver) that when this timer block expires, the task will call
  the function rf_cdma_temp_comp_start so that the RF driver may start the
  process of adjusting the linearizers based on the temperature.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates temp_cal_timer_blk

===========================================================================*/
#ifdef RF_HAS_MULTI_RX_CHAIN 
void rfm_cdma_temp_comp_init(rfcom_device_enum_type device, rex_timer_type *timer_ptr )
{/*lint !e578 */
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        rf_cdma_temp_comp_init(timer_ptr );
        break;
    default:
        break;
    }
}
#else
void rfm_cdma_temp_comp_init(rex_timer_type *timer_ptr )
{
   rf_cdma_temp_comp_init(timer_ptr );
}
#endif
/*===========================================================================

FUNCTION RFM_CDMA_DO_TEMP_COMP                               INTERNAL FUNCTION

DESCRIPTION
  This function does 2 things.  The first thing it does is to reset the
  timer so that it will get called again.  Then it will start the process
  of updating the temperature and adjusting the linearizers.  This process
  can only be stopped by clearing the timer with the function rex_clr_timer().

DEPENDENCIES
  This function must be called from a task, so that it remains interruptable.
  temp_comp_timer_ptr must have been initialized and setup.

RETURN VALUE
  None

SIDE EFFECTS
  This function will set the timer, so that in will get executed again.

===========================================================================*/
#ifdef RF_HAS_MULTI_RX_CHAIN 
void rfm_cdma_do_temp_comp(rfcom_device_enum_type device)
{
    rf_cdma_do_temp_comp(device);
}
#else
void rfm_cdma_do_temp_comp()
{
    rf_cdma_do_temp_comp(RFCOM_TRANSCEIVER_0);
}
#endif
/*===========================================================================

FUNCTION RFM_TX_START                                    EXTERNALIZED FUNCTION

DESCRIPTION
  This function re-enables the PA and Tx periodic power adjustments that were
  disabled by the function rf_tx_stop().  This function will update
  rf_tx_status to RF_TX_IS_ON.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_tx_start(rfcom_device_enum_type device)
{
   switch (device) 
   {
     case RFCOM_TRANSCEIVER_0:
     case RFCOM_RECEIVER_1:
     default:
       rf_tx_start(RFCOM_TRANSCEIVER_0);
     break;
   }
}
/*===========================================================================

FUNCTION RFM_TX_STOP                                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function disables the PA and Tx periodic power adjustments while
  remaining in CDMA RxTx state.  This function will update rf_tx_status
  to RF_TX_IS_OFF.

DEPENDENCIES
  rf_tx_start() must be called before exiting CDMA RxTx state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_tx_stop(rfcom_device_enum_type device)
{
    switch (device) 
    {
      case RFCOM_TRANSCEIVER_0:
      case RFCOM_RECEIVER_1:
      default:
        rf_tx_stop(RFCOM_TRANSCEIVER_0);
      break;
    }

}
/*===========================================================================

FUNCTION RFM_DIGITAL_BAND_SELECT                         EXTERNALIZED FUNCTION

DESCRIPTION
  Initialises the rf card for the current band(CDMA/PCS). This function has
  has to be called each time whenever there is a band change. (from PCS to
  Cellular or vice versa).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the data base and rf_curr_cdma_band with the current
  band setting.

===========================================================================*/
void rfm_digital_band_select(rfcom_device_enum_type device, byte rf_band )
{

    rf_digital_band_select(device, rf_band );
}
#ifdef RF_HAS_EGPS
#error code not present
#endif /* RF_HAS_EGPS */
/*===========================================================================

FUNCTION RFM_TX_GAIN_ADJ_STEP_SIZE                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the power control step size for each up/down
  bit. The step size can be 1, 0.5 or 0.25 dB.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_tx_gain_adj_step_size( rfcom_device_enum_type device, byte step_size )
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        rf_tx_gain_adj_step_size( step_size );
        break;
    default:
        break;
    }
}
#ifdef FEATURE_SUBPCG_PA_WARMUP_DELAY
/*===========================================================================

FUNCTION RFM_GET_PA_WARMUP                      EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated PA WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
word rfm_get_pa_warmup( rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_pa_warmup();
    default:
        break;
    }
    return 0;
}

#endif /* FEATURE_SUBPCG_PA_WARMUP_DELAY */


/*===========================================================================

FUNCTION RFM_GET_CDMA_TX_AGC

DESCRIPTION
  This function returns the CDMA Tx AGC setting in 10 bit binary
  format.

  WITH Tx LINEARIZER SETUP FOR -52.25dBm..+33dBm, the Tx signal strength is:

    Tx strength in dBm = -52.25 + (( rf_get_cdma_rx_agc() * 0833 ) / 10000 )

    That's +0.085dB per count referenced to -52.25dBm.  Since this returns
    word, this is a numeric Tx signal strength from -52.25dBm to +32.97dBm.

DEPENDENCIES
  None

RETURN VALUE
  word = 10 bit Tx AGC value

SIDE EFFECTS
  None

===========================================================================*/
word rfm_get_cdma_tx_agc(rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_cdma_tx_agc();
    default:
        break;
    }
    return 0;
}
/*===========================================================================

FUNCTION RF_GET_MIN_TX_PWR_IN_DB                            EXTERNAL FUNCTION

DESCRIPTION
  Returns the minimum TX power rail for CDMA cellular (in dBm)

DEPENDENCIES
  Should be called after NV is read

RETURN VALUE
  int2

SIDE EFFECTS
  None

===========================================================================*/
int2 rfm_get_min_tx_pwr_in_dB( rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_min_tx_pwr_in_dB();
    default:
        break;
    }
    return 0;
}


/*===========================================================================

FUNCTION RF_GET_TX_OPEN_LOOP                               EXTERNAL FUNCTION

DESCRIPTION
  Returns the 10 bit sign extended TX Open Loop Accumulator value

DEPENDENCIES
  Should be called within an MSM_LOCK(), MSM_UNLOCK() pair

RETURN VALUE
  int16

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_tx_open_loop( rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_tx_open_loop();
    default:
        break;
    }
    return 0;
}

/*===========================================================================

FUNCTION RF_GET_TX_GAIN_ADJUST                              EXTERNAL FUNCTION

DESCRIPTION
  Returns the 9 bit sign extended TX Gain Adjust register value

DEPENDENCIES
  Should be called within an MSM_LOCK(), MSM_UNLOCK() pair

RETURN VALUE
  int16

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_tx_gain_adjust( rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_tx_gain_adjust();
    default:
        break;
    }
    return 0;
}

/*===========================================================================

FUNCTION RFM_GET_TX_WARMUP                      EXTERNALIZED FUNCTION

DESCRIPTION    This function returns the new calculated TX WARMUP value
               based on the NV item in chips units.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
word rfm_get_tx_warmup( rfcom_device_enum_type device)
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        return rf_get_tx_warmup();
    default:
        break;
    }
    return 0;
}

/*===========================================================================

FUNCTION RFM_TUNE_BAND_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function tunes the UHF Tx/Rx synthesizer to the specified channel in
  the specified band. If the band and channel have not changed since the
  last time the rf was tuned nothing is done.  If the band has not changed
  only the channel is tuned. If the band has changed both a band change
  and channel tune will be performed. The amount of time to wait before
  RF is settled is returned.

DEPENDENCIES
  rf_config, rf_pa_range_step_cal, and rf_cdma_tx_comp_vs_freq[] must
  have been loaded from NV storage
  rf_state must be setup

RETURN VALUE
   word : time to wait for RF to settle

SIDE EFFECTS
  Updates database item DB_CHANNEL to the new channel
  Updates rf_curr_cdma_band
  Updates rf_freq_index

===========================================================================*/
int16 rfm_tune_band_chan
(
    rfcom_device_enum_type device,
    rf_card_band_type band,
    word chan
)
{
    return (int16)rf_tune_band_chan(device,band,chan);
}
/*===========================================================================

FUNCTION RFM_INIT_DIGITAL_BAND_CHAN                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function is meant to be called at CDMA slotted sleep wakeup.  Caller
  may request initialization to different band and channel than the last band
  and channel used.  Changes between PCS and Cellular digital modes are only
  supported.  rf_mode (DB_RF_MODE) must be DB_RF_SLEEP upon entering this
  function. RF state must be RF_STATE_CDMA_RX.

DEPENDENCIES
  1) rf_state must be = RF_STATE_CDMA_RX
  2) Database item DB_RF_MODE must be = DB_RF_SLEEP
  3) Procedure rf_enable_digital_rx_pwr() must have been called before calling
     this procedure.
  4) rf_complete_warmup() must be called when the warm up timer expires to
     finish digital initialization.

RETURN VALUE
  When RF_HAS_WU_OPTIMIZED defined.
     uint32 representing the usec wait value for RxAGC settling.  Value of 0
            represents AGC needs to be started by calling rf_complete_sleep_warmup().

  When RF_HAS_WU_OPTIMIZED undefined.
     void

SIDE EFFECTS
  Updates RF local variable rf_mode and database item DB_RF_MODE to
  either DB_RF_CDMA or DB_RF_PCN
  RF local variable rf_chan and database item DB_RF_CHAN may be updated if
  there is a channel change

===========================================================================*/
#ifdef RF_HAS_WU_OPTIMIZED
#error code not present
#else
 void   rfm_init_digital_band_chan
#endif /* RF_HAS_WU_OPTIMIZED */
(
    rfcom_device_enum_type device,
    rf_card_band_type band,
    word chan
)
{
#ifdef RF_HAS_WU_OPTIMIZED
#error code not present
#else
   (void)  rf_init_digital_band_chan(device,band,chan);
#endif /* RF_HAS_WU_OPTIMIZED */
}
#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
/*===========================================================================

FUNCTION       RFM_NOTIFY_SCH_DATA_RATE

DESCRIPTION    This API function is called to notify the RF driver when
               the reverse supplemental channel will be on/off in the next
               frame.  R-SCH data rate is also passed.

DEPENDENCIES   rf_nv_dig_items_ptr must have been set up

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void rfm_notify_sch_data_rate
(
    rfcom_device_enum_type device,
    boolean sch_is_on,
    enc_sch_rate_type sch_rate,
    boolean dcch_is_on
)

{
    rf_notify_sch_data_rate(device,sch_is_on,sch_rate,dcch_is_on);
}
#endif  /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

#if defined(T_MSM6300)
#error code not present
#endif /* T_MSM6300 */

/*===========================================================================

FUNCTION RFM_SET_MDSP_CDMA_SPECTRAL_INVERSION            EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets the CDMA spectral inversion bit in the MDSP for targets
  that support it.

DEPENDENCIES
  DSP must be downloaded and running.  Cannot be called while a search is in
  progress

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_set_mdsp_cdma_spectral_inversion( rfcom_device_enum_type device )
{
    switch (device) {
    case RFCOM_TRANSCEIVER_0:
        rf_set_mdsp_cdma_spectral_inversion();
        break;
    default:
        break;
    }

}

#ifdef RF_HAS_HDR
#error code not present
#endif /* RF_HAS_HDR */
/*===========================================================================

FUNCTION RFM_GET_BAND_CHAN                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the current settings of the RF driver internal
  variables rf_curr_cdma_band and rf_chan

DEPENDENCIES
  Should not be called until after digital mode is entered for the first
  time.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rfm_get_band_chan
(
    rfcom_device_enum_type device, 
    rf_card_band_type *band, 
    word *chan
)
{
    rf_get_band_chan_temp(device,band,chan);
}
/*===========================================================================

FUNCTION RFM_GET_GPS_IF_CONFIG                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns one of the possible GPS IF configuration. 
  
DEPENDENCIES
  None

RETURN VALUE
  GPS IF configuration type

SIDE EFFECTS
  None
  
===========================================================================*/
//To do 7600
//rf_synth_gps_IF_type rfm_get_gps_IF_config( rfcom_device_enum_type device )
uint8 rfm_get_gps_IF_config( rfcom_device_enum_type device )
{
    return synth_get_gps_if_config( device ); 
}

/*===========================================================================

FUNCTION RFM_GET_SUB_CLASS

DESCRIPTION
  This function takes band parameter and and returns an associated subclass
  bitmask.

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
uint32 rfm_get_sub_class
(
    rfcom_device_enum_type device, 
    rf_card_band_type band
)
{
    switch(device)
    {
    case RFCOM_TRANSCEIVER_0:
    case RFCOM_RECEIVER_1:
    default:
        return rf_get_sub_class( band );
    }
}

#ifdef RF_HAS_NO_RTC_DEPENDENCY
/*===========================================================================

FUNCTION RFM_BYPASS_RTC_CTL_ON_OFF

DESCRIPTION
  Control to enable or disable the Bypass of RTC Synch.
  This function will control a specified rx path. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfm_bypass_rtc_ctl_path_on_off( rfcom_device_enum_type device, boolean rf_bypass_rtc_on_off )
{
  rf_bypass_rtc_ctl_path_on_off( device , rf_bypass_rtc_on_off);
}

#endif /* RF_HAS_NO_RTC_DEPENDENCY */
/*===========================================================================

FUNCTION RFM_HAS_HIGH_SINR

DESCRIPTION
  Returns TRUE if the current chipset configuration can exhibit high
  SINR during conducted diversity testing.
    
DEPENDENCIES
  None
     
RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
  
===========================================================================*/
boolean rfm_has_high_sinr( void )
{
  return rf_has_high_sinr();
}

/*=========================================================================

FUNCTION RFM_GET_TX_CARRIER_FREQ

DESCRIPTION
  This function returns the Tx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  Rx carrier frequency in KHz for valid band and channel.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rfm_get_tx_carrier_freq( rf_card_band_type band, word chan )
{
    return rf_get_tx_carrier_freq( band, chan );
}

/*=========================================================================

FUNCTION RFM_GET_RX_CARRIER_FREQ

DESCRIPTION
  This function returns the Rx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None


RETURN VALUE
  Tx carrier frequency in KHz for valid band and channel.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rfm_get_rx_carrier_freq( rf_card_band_type band, word chan )
{
    return rf_get_rx_carrier_freq( band, chan );
}
/*===========================================================================

FUNCTION RFM_INIT_DIGITAL

DESCRIPTION
  Programs the RF RX synthesizers unit for digital mode of operation
  (CDMA or PCN).  This procedure should be called every time digital
  mode is entered.

DEPENDENCIES
  database item RF_MODE must be set
  The MSM1 core AGC hardware should have been disabled first
  Procedure rfapi_enable_digital_rx_pwr() must have been called before calling
  this procedure.

RETURN VALUE
  None

SIDE EFFECTS
  The database item DB_RF_MODE is set to DB_RF_PCN or DB_RF_CDMA
  depending on the current hardware configuration.

===========================================================================*/
void rfm_init_digital(rfcom_device_enum_type device)
{
    rf_init_digital(device);
}

#ifdef FEATURE_TX_POWER_BACK_OFF
/*===========================================================================

FUNCTION RFM_GET_TX_PWR_LIMIT_ADJ                            EXTERNAL FUNCTION

DESCRIPTION
  API function used by MUX1X to query what the TX power limit (and amount
  of adjustment due to PA Backoff) would be for a particular reverse link
  channel configuration
  
DEPENDENCIES
  None

RETURN VALUE
  rf_txc_pwr_limit_type

SIDE EFFECTS
  None

===========================================================================*/
void rfm_get_tx_pwr_limit_adj
(  
    rf_txc_chan_config_type *chan_config,
    rf_txc_pwr_limit_type *tx_pwr_limit_adj
)
{
    rf_get_tx_pwr_limit_adj( chan_config, tx_pwr_limit_adj );
}

#endif /* FEATURE_TX_POWER_BACK_OFF */

/*===========================================================================

FUNCTION rfm_get_nv_band

DESCRIPTION
  This function returns the NV band ID representing which NV table
  is currently being used.

DEPENDENCIES
  None

RETURN VALUE
  NV band id.

SIDE EFFECTS
  None
  
===========================================================================*/
byte rfm_get_nv_band( rfcom_device_enum_type device )
{
    return rf_get_nv_band( device );
}

#ifdef FEATURE_TX_POWER_BACK_OFF
/*===========================================================================

FUNCTION RFM_UPDATE_TX_PWR_LIMIT                            EXTERNAL FUNCTION

DESCRIPTION
  API function used by MUX1X to notify RF that the reverse link channel
  configuration is changing
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  TX Power limit may be updated

===========================================================================*/
void rfm_update_tx_pwr_limit( rf_txc_chan_config_type *chan_config)
{
    rf_update_tx_pwr_limit( chan_config );
}

/*===========================================================================

FUNCTION RFM_RESET_PA_BACKOFF_MODULE                        EXTERNAL FUNCTION

DESCRIPTION
  API function used to reset the PA Backoff module
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  TX power limit may be adjusted

===========================================================================*/
void rfm_reset_pa_backoff_module( uint16 rf_reset_acc_val )
{
    rf_reset_pa_backoff_module( rf_reset_acc_val );
}

#endif /* FEATURE_TX_POWER_BACK_OFF */

/*===========================================================================

FUNCTION RFM_GET_CAGC_LINEARIZER_RANGE                               EXTERNAL

DESCRIPTION
  This function gets the current setting of the linearizer range of the 
  CAGC section (RX & TX) 
    

DEPENDENCIES
  Should not be called until after NV is read and 
  rf_init_cagc_linearizer_range() is called

RETURN VALUE
  rf_cagc_dynamic_range_type : CAGC linearizer range setting

SIDE EFFECTS
  None
===========================================================================*/
rf_cagc_dynamic_range_type rfm_get_cagc_linearizer_range( void )
{
    return rf_get_cagc_linearizer_range();
}

/*===========================================================================

FUNCTION RFM_GET_CDMA_TX_AGC_IN_DBM                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the CDMA TX AGC in dBm.  Resolution is .1 dB

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_cdma_tx_agc_in_dBm( void )
{
    return rf_get_cdma_tx_agc_in_dBm();
}

/*===========================================================================

FUNCTION RFM_GET_PATH_CDMA_RX_AGC_IN_DBM                     EXTERNAL FUNCTION

DESCRIPTION
  This function returns the CDMA Rx RSSI in dBm for a specific Rx device.  
  Resolution is .1 dB

DEPENDENCIES
  None

RETURN VALUE
  Signed Rx power in dBm

SIDE EFFECTS
  None

===========================================================================*/
int16 rfm_get_path_cdma_rx_agc_in_dBm( rfcom_device_enum_type device )
{
    return rf_get_path_cdma_rx_agc_in_dBm( device );
}

/*===========================================================================

FUNCTION RFM_GET_TX_GAIN_ADJ                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the current value of the Tx gain adjust register.
  If extended linearizers were used, the value is converted from the
  extended linearizer range to the expected 85 dB range (giving 1/2 dB
  resolution).

DEPENDENCIES
  None.

RETURN VALUE
  int1 - Tx gain adjust register value (8 bits @ 1/2 dB resolution).

SIDE EFFECTS
  None.

===========================================================================*/
int1 rfm_get_tx_gain_adj( void )
{
    return rf_get_tx_gain_adj();
}

/*============================================================================

FUNCTION RFM_GET_LNA_GAIN_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an enum representing the current LNA state.

DEPENDENCIES
  None

RETURN VALUE
  lna_gain_state_type

SIDE EFFECTS
  None

=============================================================================*/
lna_gain_state_type rfm_get_lna_gain_state( void )
{
    return rf_get_lna_gain_state();
}

/*============================================================================

FUNCTION RFM_GET_PA_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an byte representing the two bits for PA state.

DEPENDENCIES
  None

RETURN VALUE
  pa state

SIDE EFFECTS
  None

=============================================================================*/
byte rfm_get_pa_state( void )
{
    return rf_get_pa_state();
}

/*============================================================================

FUNCTION RFM_GET_RATCHET_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an byte representing the two bits for RATCHET state.

DEPENDENCIES
  None

RETURN VALUE
  ratchet state

SIDE EFFECTS
  None

=============================================================================*/
byte rfm_get_ratchet_state( void )
{
    return rf_get_ratchet_state();
}

/*===========================================================================

FUNCTION RFM_GET_SUPPORTED_HW                           INTERNAL FUNCTION

DESCRIPTION
  Returns the ID of the RF Card, from the NV Item 
  NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
  rf_hw_type value containing the Id of the RF card being used with this target.

SIDE EFFECTS
  None
===========================================================================*/
rf_hw_type rfm_get_supported_hw(void)
{
    return rf_get_supported_hw();
}

/*============================================================================

FUNCTION RFM_GET_PATH_LNA_GAIN_STATE                                       EXTERNAL

DESCRIPTION
  This function will return an enum representing the current LNA state.

DEPENDENCIES
  None

RETURN VALUE
  lna_gain_state_type

SIDE EFFECTS
  None

=============================================================================*/
lna_gain_state_type rfm_get_path_lna_gain_state( rfcom_device_enum_type device )
{
    return rf_get_path_lna_gain_state(device);
}

/*===========================================================================

FUNCTION RFM_FREEZE_CAGC                                 INTERNAL FUNCTION

DESCRIPTION
This function is called before writing the RAS RAM linearizer tables. This will
a eliminate any errors in the TX_AGC_ADJ and any Rx power gain resulting from
LNA gain offset during the loading of the RAS RAM linearizer tables.

DEPENDENCIES
  This function is called during a INTLOCK_SAV call.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void rfm_freeze_cagc ( void )
{
    rf_freeze_cagc();
}

/*===========================================================================

FUNCTION RFM_UNFREEZE_CAGC                                 INTERNAL FUNCTION

DESCRIPTION
This function is called after writing the RAS RAM linearizer tables. The
function will unfreeze the CAGC (RX AND TX) back to normal operation. It
is called once the RAS RAM linearizer tables are written.


DEPENDENCIES
    This function is called during a INTLOCK_SAV call.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void rfm_unfreeze_cagc ( void )
{
    rf_unfreeze_cagc();
}

/*============================================================================

FUNCTION RFM_PREPARE_TO_SLEEP                                         EXTERNAL

DESCRIPTION
  This function is called by 1x and 1xEV-DO clients when the radio is no
  longer actively needed but before sleep.  The function will prepare for
  sleep before the RTC is disabled, which can cause the RX front AGC and
  DC loops to break.  This function insures that DC offsets are saved at
  a point where they are known to be good.

  This function is safe to call if the radio will not actually go to sleep.

  Only MSM state is saved in this function, RF chipset state is unaffected.

DEPENDENCIES
  On MSMs that have an RX front dependance on the RTC, this function must
  be called before the RTC is disabled if the radio is not already disabled.

RETURN VALUE
  None

SIDE EFFECTS
  DC offsets are saved.

=============================================================================*/
void rfm_prepare_to_sleep( rfcom_device_enum_type device )
{
  rxf_save_accum_data( rfcom_to_path[ device ] );

  /* If other processing is added here, insure that this function is still
     safe to call if the radio is re-used before it goes to sleep */
}

#ifdef RF_HAS_GPS
#error code not present
#endif /* RF_HAS_GPS */
#ifdef RF_HAS_HDR_REVA_TX_POWER_BACK_OFF
#error code not present
#endif /* RF_HAS_HDR_REVA_TX_POWER_BACK_OFF */

#endif /* FEATURE_MULTIMODE_ARCH */

