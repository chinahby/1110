/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         C D M A   T R A N S M I T   T R A F F I C   M O D U L E

GENERAL DESCRIPTION
  Items associated with the traffic channel

EXTERNALIZED FUNCTIONS
  txc_rlgc_trans_cmd - The RLGC transaction callback
  txc_ho_trans_cmd - The HO transaction callback
  txc_traffic_init - Initialization section for traffic channel
  txc_rev_sch_trans_cmd - The reverse SCH transaction callback
  txc_traffic_isr - Interrupt handler for the traffic channel
  txc_traffic - Process traffic channel state

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 1991 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/txctraffic.c#7 $ $DateTime: 2009/01/30 15:03:26 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/09   jtm     Disable TX in action frame -20ms when performing HHO to a 
                   different frame offset.
08/13/08   jtm     Fixed Klockwork Errors. 
07/24/08   jtm     Removed left over FEATURE_IS2000_REL_C_DV code and changed
                   TXC_UPR_BNDS_CHK to TXC_UPPER_BOUNDS_CHECK for clarity.
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV and FEATURE_IS2000_REL_D_DV.
                   Reduced debug string sizes for ULC effort. 
                   Removed FEATURE_MAX_PWR_PROTECTION_DEBUG. Added debug strings
                   for monitoring commands.
02/08/08   vlc     Removed interim step up in power after HHO for high RCs.
11/06/07   vlc     Removed parameters to ds707_scrm_max_allowable_sch_rate()
                   for older target with no data-on-apps.
10/15/07   vlc     Removed an ASSERT which caused a lint warning.
08/08/07   vlc/vm  Change the function name from rfm_set_pa_hyst_timer to
                   rfm_set_pa_access_probe_config().Also change the
                   feature name to FEATURE_PA_ACCESS_PROBE_CONFIG 
08/03/07   vlc/vm  In function txc_traffic_init(), added call to function 
                   rfm_set_pa_hyst_timer() to notify RF that subsequent call 
                   to rfm_enable_tx_pwr() is for traffic state so RF can set 
                   its timer to the appropriate value (under feature
                   FEATURE_PA_DIFF_BETWEEN_ACC_AND_TRAFFIC.)
05/04/07   vlc     Added debug information for HHO TX power settings.
04/02/07   vlc     Added new PCG define for bin0 ENC ISRs specifically for loop
                   back calls.  Moved default PCG to 1 for bin0 ENC ISRs for 
                   all other calls.
02/23/07   vlc     Fixed compiler warnings/errors when SCH, Rel A, and P2 
                   features are turned off.
01/22/07   trc     Parameterize the PCG for bin0 and bin3 ENC ISRs
01/09/07   vlc     Merged the following change to main line from SC2X branch.
                   Mainly, adding support for FEATURE_MDSP_SUPPORTS_TX_AGC.
           vm      Added the support for Firmware TX 
12/07/06   vlc     Moved code to turn off TX for HHO action to action time 
                   frame (was done in the frame previous to action time frame.)
11/22/06   vlc     Fixed RVCT compiler warnings
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
10/26/06   vlc     Changed TXC time line for traffic state.  Moved 1st ENC ISR
                   to bin0, PCG1.  Moved 2nd ENC ISR to bin3, PCG2.  Removed 
                   code which sends SCH frame in task context.  Added new build
                   states to handle late frames or overflow frames.
10/10/06   adk/vlc Updated the function "txc_traffic()" (case TXC_FTM_SCH_F):
                   Called "enc_sch_rc()" to correctly process SCH.
                   Called "txc_update_chan_config_to_rf()" to implement
                   "FEATURE_TX_POWER_BACK_OFF."
09/19/06   vlc     Fixed lint errors.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
07/24/06   vlc     Movd prim_num_bits to txcmux.c since it's only used there.
07/14/06   vlc     Merged the following change from MSM7500 orphan archive.
           tmr     Added parameters to ds707_scrm_max_allowable_sch_rate() call
                   to reduce RPC calls for 7500 data-on-apps.
06/09/06   trc     Remove T_MSM6700 featurization
05/03/06   vlc     In function txc_tx_on_off_trans_cmd(), swapped order of
                   programming tx gain adjust register and turning on PA to
                   eliminate spike after HHO.
                   In txc_tx_frame_proc(), move a "{" out of an #ifdef pair
                   so the brackets will now match up in the entire function.
04/17/06   vlc     Fixed sign in ASSERT statements.
03/20/06   vlc     In function txc_tx_frame_proc_send_data3g(), if a NULL pointer
                   is detected in data received from RLP layer, recover by dropping
                   the frame via an ASSERT rather than an ERR_FATAL.
                   Added important Medium level debug messages.
12/05/05   rkc     Check if rev_fr_ptr->fr_item_ptr for NULL in
                   txc_log_rlp3() and txc_log_fdch_rlp3().
09/08/05   ejv     Use new srch_mux interface calls.
08/02/05   rkc     Fixed position of closing "}" to be inside
                   #ifdef FEATURE_IS2000_R_SCH in function txc_tx_frame_proc_build3g().
07/25/05   rkc     Fixed lint errors.
05/09/95   vlc     Added check for NULL ptr from data buffers given by RLP to
                   transmit.
04/29/05   vlc     Added 4GV support under FEATURE_VOC_4GV.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
03/14/05   vlc     Fixed compiler warnings for MSM6700.
01/26/05   vlc     Changed #include ds.h (obsolete) to ds707.h.
01/17/05   ljl     Updated for Rel D interface changes by CP.
12/09/04   jyw     removed dec_init_td_flsb_dv if DV is already enabled
12/09/04   jyw     Refeaterization to avoid the compiling error for nonDV
12/03/04   bn      Correct name change in mDSP DV APP
12/03/04   ljl     Updated txc_switch_trans_cmd() to store target and serving
                   sector information.
11/08/04   sr      Added mDSP call to delay start of PDCCH by 1 slot in
                   txc_f_dv_params_trans_cmd()
10/20/04   ljl     Featurized the function call of rpc_program_rev_d_gain_ratios().
10/19/04   ljl     Added featurization, used the correct slot for 10ms tick,
                   and disabled reverse power control during PLT.
10/12/04   ljl     Added to call enc_write_data_done() after R-SCH data buffer has
                   been written to encoder ram for TDSO.
10/04/04   ljl     Added featurization for muxdvlog_update_gating_rate().
09/27/04   ljl     Modified to follow Rev D call flow for PLT.
09/14/04   jrp     Update calls to DV_INFO header.
09/10/04   ljl     Processed TXC_10MS_TICK_SIG.
09/08/04   sr      Disabled DV Application in the firmware when ending a call
                   as a workaround for high PER on subsequent DV calls.
09/01/04   jrp     Added parameter for DTX of R-REQCH when writing gain ratios.
09/01/04   sr      Disabled DV channels in mDSP in TXC_EXIT_F and TXC_CDMA_F
                   Added mDSP message handler for PDCH Traffic Msg
09/01/04   jrp     Added back pre-6700 interfaces.
09/01/04   sr      Added proper features around DV header files
08/30/04   jrp     Added missing call to muxmdsp_dv_ctrl_init().  Also, set both
                   serving and target parameters in f_dv_parms action.
08/25/04   jyw     Modified the rxcpdch_init interface to include the bit shift control
08/25/04   jyw     Put 2-bit shift for the DMSS dec ob control
08/25/04   jyw     Put the dec ob control for valid CRC only
08/17/04   jrp     Added programming of gain ratios in PLT mode.
08/16/04   sr      Made a new function to enable DV application. This waits for
                   an acknowledgement from mDSP services that signals DV app is
                   ready before proceeding.
                   Fixed txc_switch_trans_cmd().
08/16/04   ljl     Rel D reverse channels PLT.
08/12/04   sr      Registered action handlers for F_DV_PARAMS and SWITCH
                   actions. Set stale timeout interrupt to 8
08/02/04   jyw     Fixed the long code mask in the txc_f_dv_params_trans;
07/30/04   vlc     Added support for rel C TDSO.
07/29/04   jyw     Updated the action interface of txc_f_dv_params_trans_cmd to include
                   plt_pdch_cfg
07/28/04   jrp     Fixed compilation errors.
07/26/04   jyw     Added rxcpdch_init and rxcpdcch_init in the TXC_F_DV_PARAMS_TRANS_CMD
                   handler;
07/26/04   jyw     In TXC_F_DV_PARAMS_TRANS_CMD action handler, added function call
                   dec_init_td_flsb_dv(); to initialize the turbo decoder and
                   for link symbol buffer.
07/21/04   bn      Call encoder write done for SCH PLT
07/19/04   sr      Renamed MCTRANS_TXC_SWITCH_ACTION to MCTRANS_SWITCH_ACTION
06/29/04   jrp     Gain pass-through, and modulator interrupt changes.
06/24/04   jyw/sr  Fixed the way Devo mdsp application is enabled plus misc
                   cleanup for DV
06/16/04   sr      Merged in changes from 6500 5004 build
06/14/04   sr/va   Added HHO cleanup under FEATURE_HHO_ENHANCEMENTS
06/01/04   ljl     Added DV PLT support.
05/21/04   jyw     Introduce new RF PA backoff interface
05/12/04   vlc     Added MCTRANS_MAX_ACTION under feature FEATURE_TRANS_MANAGER.
05/07/04   vlc     Modified RLP logging extensively to decouple MUX processing
                   and RLP processing, moved RLP processing to RLP module.
                   Added MCTRANS_MAX_ACTION.
05/04/04   jyw     remove the featurization around the txc_f_dv_params_trans_cmd
                   and txc_switch_trans_cmd
05/04/04   jyw     include txc_f_dv_params_trans_cmd and txc_switch_trans_cmd
                   to FEATURE_TRANS_MANAGER
04/26/04   vlc     Implemented new way of handling RXTX confirmation of all
                   signalling messages for voice calls/data calls/svd calls.
04/26/04   sr      Set gain adjust first before turning on PA in TXC_TX_ON_F
04/20/04   jrp     Fixed lint warnings.
03/31/04   sr      Added call to enc_user_pn_mask() in TXC_RETURNED_F
03/16/04   jw      Filled the txc_lcm_trans_cmd as the action handler for the
                   NMCTRANS_LCM_ACTION in the new transaction engine;
03/16/04   jw      Added txc_lcm_trans_cmd() function for the new action
                   MCTRANS_LCM_ACTION;
03/16/04   jw      Featurized the lcm processing part in the function
                   txc_ho_trans_cmd();
03/16/04   sr      Made TEMP_HACKS_FOR_DV depend on T_MSM6700
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/05/04   sr/bkm  Moved clearing of frame offset from MC to TXC_EXIT_F and
                   TXC_CDMA_F
03/05/04   bn      For MSM6700, need to tell the SCH encoder when done
03/03/04   sr      Changes for DV plus included changes to MSMSHARED tip
02/03/04   sab     Moved call to txc_build_dcch_frame if not Release B.
02/03/04   sab     Featurized confirmation_type changes under Release B.
12/02/03   vlc     Removed txc_tx_frame_proc_build_data3g() function inside
                   #if 0.
11/21/03   jrp     Added call to muxmdsp_dv_cell_switch to write mDSP parameters.
11/04/03   sr      Explicitly compared band class returned by
                   mc_get_band_class() against CAI_BAND_CELL/PCS
10/28/03   bn      Checked the for NULL pointer from RLP, before writing to encoder
10/21/03   bn      Fixed compilation warning errors when using RVCT compiler
10/15/03   sb      If in a voice call, and not on DCCH, make sure the confirmation
                   is always delayed by a frame, to enable a rate change.
09/11/03   lcc/vlc In txc_stop_tx, only zero out all digital gains when we are turning
                   off the transmitter for HHO and not in the case of fade.  Otherwise
                   when we come out of fade and turn on the transmitter, the gains will
                   be incorrect.
09/09/03   dna     Merged porting changes for MSM6500
09/08/03   vlc     In function txc_tx_frame_proc_send_data3g(), added check for
                   NULL ptr before calling ENC_SCH_DATA_OUT macro.
09/05/03   vlc     Removed shared txc_so.rate_set from txc_log_fdch_rlp3().
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
08/06/03   sab     Use confirmation_type in payload of TXC_MSG_F to determine
                   whether TX should signal RXTX when the msg was built, or sent.
07/25/03   junz    Added FDCH RLP3 frame logging
06/27/03   bn      Corrected the RLP logging's time stamp logic when in VP2 and P3
06/24/03   jrp     Changed calls to srch_set_tx_rate_adj() to call instead
                   rpc_set_tx_rate_adj() so that TX_RATE_ADJ is written in the
                   appropriate units - either 1/12 or 1/10dB.
05/28/03   vlc     Disable SCH when getting TXC_CDMA_F or TXC_EXIT_F from MC.
05/21/03   vlc     Allow pri + sec on DCCH to teardown unsupported call scenario
                   gracefully rather than error fatal.
05/14/03   vlc     In function txc_tx_proccess_sch(), set service type to Primary
                   or Secondary for 1x SCH MUX PDU 1 and MUX PDU 2.
04/25/03   bn      Added logics to support RLP loging in VP2 and P3
04/10/03   vlc     For Rel A, moved TXC ISR to PCG 1 in traffic state.
03/28/03   vlc     Added logic to support different transmit schemes for
                   different types of RLP signalling frames in an SVD call.
03/07/03   vlc     Reverted changes for SVD done on Release 0 code which caused
                   signalling msgs to be transmitted on the incorrect fundicated
                   channel in some cases.
03/04/03   bkm     Added support for REV_FCH_GATING event.
01/31/03   bkm     Shortened R-SCH F3 debug message for readability.
01/31/03   bn      Re-did P2 featurizations fixes for JCDMA builds for sr/wn
01/30/03   bn/sy   Used the less-DSM-size-dependency logic to check for chained
                   DSM items. Also generic name for function to get DSM items
01/23/03   sr/wn   P2 featurizations fixes for JCDMA builds
01/10/03   bn      Used the new encode byte accessing function to simplify
                   the logic handling MUXPDU5 and MUXPDU3 frame processing
01/07/03   jrp     Changed sch_fr_sent initialization to FALSE to prevent
                   inaccurate error message: RLP frames not built in time.
12/05/02   sn      Fixed the compilation error when undefined feature P2
12/02/02   sy      Made changes to support E0 at the end of the turbo mux pdu5
                   payload.
11/25/02   bn      Support 32x, make sure the max number of MUXPDU is no more than 8
11/26/02   dbc     Added support for rf_notify_sch_data_rate() for dynamic PA
                   switch point adjustment according RSCH data rate. Code
                   wrapped in FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
10/31/02   bn      Fixed the wrongly increasing RDCCH MUX counters
10/16/02   jrp     Moved pcbit data write from Tx txc_tx_frame_proc() to a new
                   function to be called from Rx context.
10/15/02   dlb     Added new power control logging to combine power control
                   and frame type logging.
10/14/02   bn      Using the correct feature R_SCH in PLT
10/14/02   ak, vlc Modified code to work with MultiMode Data client on MSM6100.
10/08/02   jrp     Added featurization to fix compiler warnings.
09/30/02   js, bn  Fixed the incorrected Reverse MUX PDU5 frame size in RLP logging
09/24/02   bn      Fixed the incorrected length of the FILL MUX PDU5
09/19/02   snn     Made changes to let DCCH handle BLANK rates and IDLE bits
                   correctly during transmission.
09/19/02   bgc     Merged feature changes for MSM6000 for IS2000_SCH and P2.
09/09/02   bn      Added logic to update RSCH-MUXPDU5 Mux-statitic counters
09/09/02   cml     Featurized definition of rlp_to_mux_table
                   Wrapped TXC_TX_POST_RLP_FCH_PROCESS with FEATURE_DS_RPL3
09/04/02   bn,hrk  In Rel.A when FEATURE_IS2000_REL_A_SVD is turned off,
                   prim_dcch_data_ptr and prim_dcch_rate were not initialized.
08/27/02   hrk     Fixed frame type log reporting in Rel.A when both R-DCCH and
                   R-FCH are assgined.
08/20/02   bn      Added new SCRM rate logic
08/20/02   dbc     Remove usage of global rf_tx_pwr_limit; instead use RF unit
                   API function rf_get_cdma_tx_pwr_limit(). When
                   FEATURE_NV_HAS_MIN_RX_RSSI is defined use rf_get_min_tx_pwr_in_dB()
                   to get the minimum TX power
08/14/02   bn      Initialize the enc_fch_gain and enc_dcch_gain correctly when
                   R-FCH or R-DCCH is not included
08/13/02   hrk     Intialized txtc.tx_rate_adj and txtc.offset_pwr in
                   txc_pilot_init() to take care of problem during handoffs
                   between high and low RCs. Fixed featurization errors.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
08/01/02   bn      Fixed compilation error when feature REL_A is off
08/01/02   jrp     Added featurization for FPC modes 4,5,6.  Changed FPC_MODE
                   enums to be more descriptive due to SVD.  Uses QIB and EIB
                   flags set by RXC without clearing.  RXC now sets and clears
                   these flags.
07/25/02   hrk,bn  Fixed featurization errors.
07/24/02   hrk     Mainlined following changes from branch off rev 1.137.1.11
           bn      Updated REV MUXPDU5 logic to support turbo code
           bn      Added support for REV MUXPDU5
           hrk     Added support for VP2.
07/15/02   bgc     Added T_MSM_5100 featurization for setting of txc_xxx_gain variables.
07/11/02   bn      Put logics to support freezing the setpoint when TX is off
07/09/02   lcc     When turning off transmitter, resets tx_rate_adjust to 0.
06/25/02   HQ      Added FEATURE_CDSMS.
06/18/02   hrk     RL_GAIN_ADJ was not factored correctly.
06/18/02   jrp     Added support for FFPC Modes 4,5,6.
06/11/02   bgc     Merged MSM6000 changes to handle rpc logging.  Added
                   featurization to turn Data off.
06/06/02   sr      Applied earlier fix for back-to-back bursts to the case of
                   data as a secondary service
06/05/02   hrk     Fixed merge error for MSM5100.
                   TX_RATE_ADJ incorrectly programmed in txc_traffic_isr().
06/04/02   lcc     1. Do not set encoder gains when transmitter is off.
                   2. When turning off transmitter, set encoder gains to 0 for
                      MSM5100.
06/03/02   lcc     Corrected featurization for P2 in txc_tx_frame_proc.
05/31/02   lcc     1. Corrected stopping of HHO preambles with DCCH.
                   2. Corrected preamble gain level for MSM5105.
05/22/02   bn      Added new logics to support VP2 RPC
05/15/02   sr      If RLP finishes building frames late, i.e. after the 15ms
                   mark, send the frames right away. This fixes problems with
                   RLP resets during back to back burst scenarios.
05/14/02   snn     Replaced enum types for BLANK rates in case of Secondary service.
05/12/02   ro      Use FEATURE_IS2000_R_SCH around sch code in txc_tx_frame_proc
05/08/02   bn      Added logic to handle out of action buffer case
04/19/02   az      Added support for new voice service option SMV (SO 56)
04/27/02   lcc     Handles the case when SO is set back to CAI_SO_NULL or removed
                   on traffic channel.  Happens when SCR contains no SO.
03/27/02   bn      Fixed Non-Rel A builds complilation error caused by secondary
                   service option and fixed a possible mis-alignment error in RDCH log
03/26/02   snn     For Non-Rel A builds, fixed use of Service type and made it
                   DSRLP_PRIMARY_SRVC always.
03/26/02   hrk     Added SVD support for Markov, SMS SOs.
03/25/02   bn      Fixed compilation errors causing by secondary service option
03/12/02   snn     Added changes for 1x SCH for Secondary service
03/06/02   sy      Made corrections for RLP logging for secondary service.
03/04/02   bn      Set the DCCH mux category of DTX frame type correctly for RS1 or RS2
03/04/02   bgc     Changed features to IS2000_R_SCH where appropriate.
02/25/02   hjr     Fixed featurization of FEATURE_IS2000_SCH with
                   FEATURE_IS2000_DS_RS2 on.
02/28/02   hrk,fs  Replaced PILOT_FCH_9600 with PILOT_FCH
02/27/02   hrk     Removed FEATURE_IS2000_REL_A_SVD around rlp_to_mux_table.
02/27/02   bn,hrk  Added SVD support.
           sy      Added RLP logging for Secondary traffic.
02/14/02   vlc     In function txc_tx_frame_proc(), removed code which
                   decrements txtc.num_preambles and exits gating mode when
                   it becomes zero.
02/13/02   bn      For MSM5100 PLT, removed writing of data to encoder in the task context
                   as it is now done in the ISR
02/11/02   hrk     For MSM5100, TX_RATE_ADJ is programmed in the last 5ms bin.
                   Setting R-FCH, R-DCCH, R-SCH digital gain to 0 during preambles.
02/01/02   snn     Changed r_sch_blank_pdu_hdr to uint32 from byte.
01/14/02   bgc     Now using correct bit shifts for TX_AGC_ADJ and tx open
                   loop.
01/22/02   bn      Added support for a counter of consecutive DTX frames
01/16/02   ak/sr   Cleared dtx_duration_included and txc_dtx_off if reverse
                   dtx duration is set to infinite. This solves the problem
                   where if the reverse dtx duration went from a finite value
                   to infinite the mobile was still following the old value.
01/15/02   hrk     Changed RF_ENABLE_PWR_WAIT to RF_ENABLE_TX_PWR_WAIT when using
                   FEATURE_RF_WAIT_CHANGES.
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
12/17/01   vlc     Removed load long code mask modification (temporary) due to
                   short test time of released builds.
12/17/01   vlc     Re-did last two revisions of this file to correct a check in
                   error (stripped version was checked in by mistake.)
12/17/01   tc      Load long code mask in txc_ho_trans_cmd if load is included.
12/13/01   vlc     In function txc_tx_frame_proc (), added check for transmitting
                   valid data frames before transmitting spare bits (TDSO support
                   only.)
12/10/01   hrk     Renamed txc_erasure_flag to txc_eib_flag. Puncturing 0x0000 if
                   a good F-FCH/F-DCCH frame is received, and 0xFFFF if a ERASURE/
                   DTX frame is received on F-FCH/F-DCCH.
12/06/01   bn      Changed the order of enable-DCCH-disable-FCH encoder to
                   disable-FCH-enable-DCCH encoder so it works in any HW platform
12/05/01   lcc     Moved encoder ISR for non-IRAM builds from bin 0 to bin 0 +
                   1 PCG to make sure that it happens after the system time frame
                   boundary.
12/02/01   ak      Fixed blanking PDU's for turbo.
11/28/01   fc      Added the support for the update of DTX duration of R-SCH
                   in progress by ESCAM containing only F-SCH assignment.
11/28/01   snn     Updated Logging RS2 data.
11/12/01   bn      Added the logic to include FCH 8th rate gating into RDCH log record
11/09/01   lcc     Removed clearing of txc_so.stored_pri when RXC_RELEASE_F is
                   received since potentially the service option can be reactivated
                   again and the call not released.
11/07/01   bn      Used MSG_HIGH to out put the expiration of SCH DTX counter
10/30/01   bn      Added support for Release A RDCH logging
10/26/01   tc      Input FCH data to modulator at ISR for MSM5100.
                   Signal RLP build frame at BIN 0 for MSM5100 non-IRAM build.
10/22/01   snn     In mixed rate sets, 1x muxPDU for SCH and FCH were made
                   different.
10/15/01   kk      Fixed R-DCCH DTX problem.
10/05/01   lcc     1. Added handling of MCTRANS_NULL_ACTION in txc_service_trans_cmd.
                   2. Added critical section protection in txc_add_action.
10/03/01   bn      Fixed RPC SCH/PICH ratio DTX all the time in STDSO or FTDSO
09/26/01   bn      Fixed the mismatched time between the RSCH rpc and frame type log
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP instance.
                   Changed to DSRLP_FIXED_SR_ID to DSRLP_FIXED_SR_ID_INDEX
09/14/01   bn      Fixed P2 featurization
09/11/01   bgc     Fixed problems with frame type in reverse frame logging.
                   Fixed R_SCH0 RC in reverse frame type logging.
09/05/01   lcc     Removed incorrect featurization around 1/8th rate gating.
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/22/01   bn      Support DCCH in FTM mode
08/14/01   sy      Added support for DCCH on RS2
08/13/01   bn,hrk  Fixed :
                   1. incorrect turn around constant during R-SCH transition in RPC logs.
                   2. multiple channel adjustment gain for FCH was delayed by 1 frame in
                      RPC logs.
08/03/01   bkm     Fixed P2 featurization in txc_tx_frame_proc_build_data3g().
07/23/01   snn     Code cleanup for allowing DCCH calls.
07/12/01   snn     Added support for Mixed Rates
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/28/01   vlc     Use r_sch_rc in txtc instead of rev_sch_rc in txc_so to
                   to remove duplication (PLT change).
06/21/01   lcc     In txc_scr_trans_cmd, when DCCH is not included, added disabling of
                   DCCH.
06/18/01   lcc     1. Disables frame type logging when sending preambles.
                   2. Added enabling of FCH when FCH is included in txc_scr_action
                      and txc_scr_trans_cmd.
06/15/01   day     Mainlined FEATURE_RLGC, FEATURE_FFPC, FEATURE_DATA_TIMELINE_ADVANCE,
                   FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR, FEATURE_FINGER_POLL
06/14/01   hrk     Corrected comprison of txc_rev_fr_block.num_frames from 1 to
                   SCH_BUF_START to determine presence of R-SCH frame in P2 mode.
06/13/01   hrk     Added Max pwr protection on DCCH for TDSO.
06/13/01   sy      Added DSRLP_RATE_INACTIVE enum to support P2 Mode
                   to show which FCH/DCCH is inactive.It behaves exactly
                   like DSRLP_RATE_BLANK.
06/08/01   hrk     Updated Max pwr protection for SCH in a DCCH+SCH configuration.
06/07/01   kk      Added Mixed Rate Support to TDSO interface.
06/05/01   hrk     uncommented code to set TX_RATE_ADJ in PLT/FTM mode.
06/06/01   lcc     1. Changed the way DTX works such that pilot gating (control hold)
                      will work for MSM5100 as well.
                   2. Recovered changes for making S-TDSO work on DCCH.
06/04/01   bgc     Added #ifdefs to turn off data services.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   jq      Wrapped some code in FEATURE_IS95B_MAHHO.
05/31/01   hrk     PCS value was used for computation of TX_GAIN_ADJ_FLOOR for
                   Cellular target.
05/30/01   kk      Fixed hardcoded RS1 number for 1x Reverse link.
05/24/01   fas     Add hooks for rf_pa_backoff.c
05/22/01   kk      Added missing EIB bit shift for RS2.
05/18/01   kk      RS2 TDSO support.
05/15/01   tc      Supported MSM5100 DCCH channel gain for P2 feature.
05/11/01   fc      Merged the following:
           lcc     Added the support of DCCH STDSO statistics logging.
05/11/01   vlc     Merged in datapump changes for PLT (ks.)
05/10/01   lcc     Corrected the problem of control hold gating rate not stored.
05/08/01   sr      Software work around for MSM5100 5ms issue
                   Turned off max power protection for baseband builds
                   Removed txc_rev_fr_block declaration in txc_tx_frame_proc()
05/03/01   tc      Supported reverse link DCCH PLT.
05/02/01   vlc     Changed FCH default tx pattern in txc_traffic_init() (for PLT
                   only.)
04/26/01   vlc     Changed name of a few constants to accomodate new SCH RC4
                   development on MSM5100.  Added SCH RC4 support.
04/24/01   jq      Fixed an error in frame logging
04/20/01   sr      Merged in from MSM5100 archive
04/19/01   tc      Fixed incorrect dsch checking in txc_log_tc_msg()
                   Fixed unnecessary check for dcch_included in txc_tc_isr()
04/19/01   tc      Merged with MSM_MUX1X.01.00.01
           lcc     Corrected re-enabling of FCH after sending PCG preambles to
                   be done only if FCH is included.
           fc      Added support of events report: Transmitter disabled and
                   Transmitter enabled.
                   Renamed mcc_report_event_msg_txed to
                   mclog_report_event_msg_txed.
           hrk     added support to disable Max Pwr protection on R-SCH from QXDM.
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc,hrk Added support for FEATURE_IS2000_P2
03/27/01   vlc     Merged up to rev 1.27 of MSM archive.
03/29/01   ak      If RLP produces a blank, handle it by sending 1/8th rate
                   NULL (if no signalling).
           lcc     1. Corrected tranmission of preambles in PCG units so that it won't
                      be repeated for "n" number of frames.
                   2. Ensures transmission of HHO preambles is done before sending
                      signalling messages (HCM).
           hrk     Mods to detect baseband conditions so that max pwr protection
                   can be dynamically disabled.
           hrk     Mods to FEATURE_MAX_PWR_PROTECTION.
           lcc     Merged in support for SO 17 (CAI_SO_VOICE_13K_IS733).
           hrk     Changed prototype for txc_rpctrl_log_frame().
           hrk     Mods to FEATURE_MAX_PWR_PROTECTION.
           hrk     Reverse Power Control info is logged immediately after frame is
                   constructed.
           fc      Added support of events report: Message transmitted.
03/27/01   snn     Added void in txc_calc_r_sch_throttle() to fix compiler err.
03/23/01   sr      Added support for 32X (PLT)
03/08/01   lh      Fix for compile errors.
03/08/01   sr      Removed divide-by-2 of ok_num_mux_pdus
03/07/01   va      Added support for populating p_rev_in_use in txtc_type
                   during handoffs and HHO returns.
           lh      Added Release A SAR parameter settings.
03/02/01   tc      Merged MSM_MUX1X.00.00.15.
02/22/01   ak      RLP logging no longer in IRAM.  Separated out 3G data code
                   into separate functions to save IRAM.
02/20/01   hrk     When TX_PWR hits minimum, TX_GAIN_ADJ is not allowed to go
                   further down.
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime switch.
02/16/01   hrk     Flushing RPC log at the end of call.
02/05/01   ak      Correct blanking of rest of SDU when Turbo or Conv.  Now
                   report correct bps for FCH in RLP logging (1/2 rate, 1/4
                   rate, etc).
01/31/01   tc      Fixed FEATURE_IS2000_TDSO.
01/30/01   tc      Merged GPS support.
01/18/01   lcc     Added initialization of txc_hho_preamble_pcg_units to FALSE
                   in txc_traffic_init.
01/11/01   tc      Merged MSM_MUX1X.00.00.06.
01/08/00   ak      Fixes for R-SCH throttling, to account for asynchronous
12/20/00   tc      Fixed PLT traffic data pattern.
12/27/00   lcc     Added turning off of 1/8 rate gating on return from handoff
                   failure if gating is not enabled.
12/13/00   hrk     Enhancements for Max Power Protection.
11/27/00   tc      Fixed PLT data rate for fundamental and supplemental channel.
11/23/00   tc      Fixed MSM5100 PLT traffic data.
11/13/00   tc      Defined txc_pch_gain, txc_fch_gain, txc_sch_gain at txccommon.
11/09/00   tc      Defined feature T_MSM_5100.
11/09/00   tc      Supported MSM5100 modulator driver interface.
12/01/00   ak      New function to do R-SCH throttling calculations, to
                   help account for case where only F-SCH changes.
11/29/000  ak      Updated PARM ID to reflect changed name in CAI.h
11/22/00   sr      Fixed problem with 1x SCH in TDSO
11/15/00   bgc     Fixed merge error with duplicate code.
11/13/00   bgc     (merge) Seperated the FCH and SCH tdso blocks while
                   packing Reverse TDSO frames. Also fixed the fr_time
                   during FCH tdso frames.
11/10/00   ak      Fixed compiler bug resulting from missed change in prev.
11/07/00   ttl/ak  Changes to throttle R-SCH in data calls when assigned SCH
                   is too high for CPU.
11/12/00   sr      (merge) Added support for Full TDSO
11/12/00   bgc     (merge) Fixed warnings and errors in factory test mode
                   for RSCH.
11/12/00   sr      (merge) Fixed problem with SCH spare bits not being set
                   to 0 forSTDSO calls
11/10/00   jc      This file was part of txc.c. See txccommon.c for full
                   change history

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <memory.h>
#include "comdef.h"
#include "customer.h"
#include "qw.h"
#include "queue.h"
#include "rex.h"
#include "enc.h"
#include "task.h"
#include "assert.h"
#include "voc.h"
#include "tx.h"
#include "txc.h"
#include "err.h"
#include "crc.h"
#include "bit.h"
#include "log.h"
#include "cai.h"
#include "srch_mux.h"
#include "ran.h"
#include "mar.h"
#include "msg.h"
#include "ts.h"
#include "rfm.h"
#include "ulpn.h"
#include "rxc.h"
#include "loopback.h"
#include "rfnv.h"
#include "mux.h"

#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
#include "rf_mdsp_tx_agc.h"
#endif

#if defined(FEATURE_DATA_MM)
  #include "ds707.h"
  #include "ds707_scrm.h"
  #if  defined (FEATURE_IS95B_MDR)
    #include "mdrrlp.h"
  #else
    #include "rlp.h"
  #endif /* FEATURE_IS95B_MDR */
#elif defined(FEATURE_DS)
  #include "ds707.h"
  #if  defined (FEATURE_IS95B_MDR)
    #include "mdrrlp.h"           /* MDR RLP routines */
  #else
    #include "rlp.h"
  #endif /* FEATURE_IS95B_MDR */
#endif /* FEATURE_DATA_MM */

#include "msm_drv.h"

#ifdef FEATURE_PLT
#error code not present
#endif

#include "ts.h"
#include "mctrans.h"
#include "mccdma.h"
#include "parm.h"
#include "ffpc.h"
#include "rlgc.h"

#ifdef FEATURE_DS_RLP3
  #include "dsrlp.h"
#endif

#include "txccommon.h"
#include "txcmux.h"
#include "txctraffic.h"
#include "txcaccess.h"
#include "txcso.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "test_mc.h"
#endif
#include "event.h"
#include "mclog.h"
/* for Release A logging */
#ifdef FEATURE_IS2000_REL_A
#include "mux_logtypes.h"
#endif /* FEATURE_IS2000_REL_A */
#include "rxctraffic.h"

#include "mdsp.h"

#include "txcmodint.h"
#include "muxmdsp.h"

#ifdef FEATURE_RLP_LOGGING
#include "dsrlp_stats.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define FCH_BUF       0   // Index to buffer for FCH in RLP data buffer
#ifdef FEATURE_IS2000_P2
  #define DCCH_BUF      1 // Index to buffer for DCCH in RLP data buffer
  #define SCH_BUF_START 2 // Index to 1st buffer for SCH in RLP data buffer
#else
  #define SCH_BUF_START 1 // Index to 1st buffer for SCH in RLP data buffer
#endif // FEATURE_IS2000_P2

byte *prim_data_ptr;           /* Pointer to primary data */
cai_data_rate_type prim_rate;  /* Primary data rate */

#ifdef FEATURE_IS2000_P2
byte *prim_dcch_data_ptr = NULL;   /* Pointer to DCCH primary data */
cai_data_rate_type prim_dcch_rate; /* Primary DCCH data rate */
#endif

#ifdef FEATURE_IS2000_REL_A_SVD
byte *sec_fch_data_ptr  = NULL;    /* Pointer to FCH secondary data */
word sec_fch_num_bits = 0;         /* Num of bits in FCH secondary frame */

#ifdef FEATURE_IS2000_P2
byte *sec_dcch_data_ptr  = NULL;   /* Pointer to DCCH secondary data */
word sec_dcch_num_bits = 0;        /* Num of bits in DCCH secondary frame */
#endif /* FEATURE_IS2000_P2 */
#endif // FEATURE_IS2000_REL_A_SVD

#ifdef FEATURE_DS_RLP3
  static dsrlp_rev_frame_block_type txc_rev_fr_block;  /* Set of all RLP frames to be tx*/
#endif /*FEATURE_DS_RLP3 */


#ifdef FEATURE_FACTORY_TESTMODE
  /* Holding buffer for TX data on the reverse FCH*/
  txc_tc_frame_type  txc_ftm_rfch_data;
  enc_rate_type txc_ftm_rfch_rate;
#ifdef FEATURE_IS2000_P2
  /* Support FEATURE_FTM_DCCH */
  /* Holding buffer for TX data on the reverse DCCH*/
  txc_tc_frame_type  txc_ftm_rdcch_data;
  enc_rate_type txc_ftm_rdcch_rate;
  boolean txc_ftm_dtx_dcch = FALSE;
  /* End of FEATURE_FTM_DCCH */
#endif
#ifdef FEATURE_IS2000_R_SCH
  /* Holding buffer for TX data on the reverse SCH*/
  txc_tc_supp_frame_type  txc_ftm_rsch_data;
  enc_sch_rate_type txc_ftm_rsch_rate;
#endif /*FEATURE_IS2000_R_SCH*/
#endif

  /* Reverse power control log accumulation buffer */
  extern word rxc_rpc_history;        /* Decision history for RPC. */

  /* AGC values for logging */
  extern byte log_rx_agc;
  extern byte log_tx_pwr;
  extern byte log_tx_gain_adj;

  /* Accumulation buffer for LOG_REV_PCTRL_C log packet */
  log_rev_pctrl_type           txc_log_rpctrl_buf = {0};

  /* Sub-record pointer for LOG_REV_PCTRL_C log packet */
  log_rev_pctrl_subrecord_type *txc_log_rpctrl_sr_ptr = NULL;

  /* Reverse frame types log accumulation buffer */
  /* Accumulation buffer */
  log_rev_frame_types_type           txc_log_frm_types_buf = {0};

  /* Sub-record pointer */
  log_rev_frame_types_subrecord_type *txc_log_frm_types_sr_ptr = NULL;

#ifndef FEATURE_HHO_ENHANCEMENTS
/* Handoff Action Time information */
boolean txc_use_action_time;
  /* TRUE --- explicit action time;
   * FALSE --- implicit action time.
   */

boolean txc_second_frm_sig_enabled = FALSE;
  /* When "txc_second_frm_sig_enabled" is TRUE, TXC will report
   * "MCC_TXC_FRAME_SIG" to MC at the next "TXC_INT_SIG" after
   * TXC sends the first "MCC_TXC_FRAME_SIG" so that MC can tune
   * to different frequency after the transmitter is turned off
   */
#endif /* ! FEATURE_HHO_ENHANCEMENTS */

#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */

   boolean txc_connect_priso = FALSE;
#ifdef FEATURE_IS2000_REL_A_SVD
   boolean txc_connect_secso = FALSE;
#endif // FEATURE_IS2000_REL_A_SVD

   //
   // These are initialized in case of HHO and extra_params is TRUE
   // in the HO cmd. They are only used when the HO cmd is followed
   // by a HO_OK cmd
   //
   byte txc_stored_nom_pwr = 0;
   byte txc_stored_num_preambles = 0;
   boolean txc_hho_preamble_pcg_units = FALSE;
#ifndef FEATURE_HHO_ENHANCEMENTS
   boolean txc_mcc_frame_sig = FALSE; // used to let mc know when tx is off
#endif /* ! FEATURE_HHO_ENHANCEMENTS */
#ifdef FEATURE_IS2000_R_SCH
   #define  MAX_R_SCH_BLANK_PDU  92  /* maximum PDU5 body size, CONV, 32X, RS1 */
    /*-----------------------------------------------------------------------
      Buffer used to write zero's to the ENC when doing blank mux pdu's
      (where the first byte is 0xE0.  This is not sized to include the
      0xE0, as in turbo, you don't necessarily use it.  So this covers
      the size of the PDU, minus the first byte
    -----------------------------------------------------------------------*/
  LOCAL const byte r_sch_blank_buffer[MAX_R_SCH_BLANK_PDU] =
                  {
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0
                  };
  /*------------------------------------------------------------------------
    For MSM5100 data to the encoder is written as 4 bytes at a time using
    ENC_SCH_DATA_OUT
    Since we use ENC_SCH_DATA_OUT to write the data into encoder from
    r_sch_blank_pdu_hdr, r_sch_blank_pdu_hdr should also be 4 byte aligned.
    If it is not 4 bytes long, then only byte 1 will be valid and bytes 2,
    3 & 4 are invalid. To make all the 4 bytes valid, we make it as unit32.
  ------------------------------------------------------------------------*/
  LOCAL const byte r_sch_blank_pdu_hdr[4] = {0xE0,0,0,0};

  LOCAL byte               txc_sch_spare_bits[29] =
                                 {
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0
                                 };

  /*
  ** The IS2000 standard specifies that the "spare bits" remaining after
  ** the maximum number of PDUs for the given rate have been used up to form
  ** the SDU shall be fille with 0's. Since turbo coded frames do not have LTU
  ** CRCs, this number is a function of the supplemental channel data rate and
  ** the code type. Convolutional = 0, Turbo = 1.
  */
  LOCAL byte               txc_num_sch_spare_bits[ENC_NUM_SCH_RATES][2] =
                                 {
                                   {0, 0},          /* 1500   */
                                   {0, 0},          /* 2700   */
                                   {0, 0},          /* 4800   */
                                   {0, 0},          /* 9600   */
                                   {8, 8},          /* 19200  */
                                   {8, 40},         /* 38400  */
                                   {40, 104},       /* 76800  */
                                   {104, 232}       /* 153600 */
#ifdef FEATURE_32X_SCH_DATA_RATE
                                   /* Having 0's for 307.2kbps entry is right
                                    * because the array is only meant for PDU type
                                    * 3.  307.2kbps only uses PDU type 5 and uses
                                    * a separate array. */
                                   ,{0, 0}          /* 307.2 Kbps */
#endif /* FEATURE_32X_SCH_DATA_RATE */
                                 };

#ifdef FEATURE_IS2000_REL_A

  /* The spare bit for MUXPDU 5. This number is a function of the supplemental channel
  ** data rate and the code type. Convolutional = 0, Turbo = 1. The spare bit should be
  ** byte alignment. The only exception in this case for 1x, the spare bit for RS1 is 4
  ** and RS2 is 3, but we will around it to 8 bits
  */
  LOCAL byte               txc_muxpdu5_nsch_sbits[ENC_NUM_SCH_RATES][2] =
                                 {
                                   {0, 0},
                                   {0, 0},
                                   {0, 0},
                                   {8, 8},
                                   {0, 0},
                                   {8, 0},
                                   {8, 0},
                                   {40,0},
                                   {40,0}
                                 };

  /* The maximum frame size of MUXPDU 5, rate set 1. This number is a function of the
  ** supplemental channel data rate and the code type. Convolutional = 0, Turbo = 1.
  */
  LOCAL word               txc_muxpdu5_rs1_maxsize[ENC_NUM_SCH_RATES][2] =
                                 {
                                   {0, 0},             /* 1500   */
                                   {0, 0},             /* 2700   */
                                   {0, 0},             /* 4800   */
                                   {20, 20},           /* 9600   */
                                   {44, 44},           /* 19200  */
                                   {43, 92},           /* 38400  */
                                   {44, 188},          /* 76800  */
                                   {44,380},           /* 153600 */
                                   {92,764}            /* 307200 */
                                 };
  /* The maximum frame size of MUXPDU 5, rate set 2. This number is a function of the
  ** supplemental channel data rate and the code type. Convolutional = 0, Turbo = 1.
  */
  LOCAL word               txc_muxpdu5_rs2_maxsize[ENC_NUM_SCH_RATES][2] =
                                 {
                                   {0, 0},             /* 1800   */
                                   {0, 0},             /* 3600   */
                                   {0, 0},             /* 7200   */
                                   {32, 32},           /* 14400  */
                                   {68, 68},           /* 28800  */
                                   {67, 140},          /* 57600  */
                                   {68, 284},          /* 115200 */
                                   {68,572},           /* 230400 */
                                   {140,1148}          /* 460800 */
                                 };
  /* The maximum frame size of MUXPDU 5 for TDSO in bits, for rate set 1. This number is a function of the
  ** supplemental channel data rate and the code type. Convolutional = 0, Turbo = 1.
  */
  LOCAL word                txc_tdso_muxpdu5_rs1[TDSO_SCH_RATE_32X+1][ 2 ] =
                                 {
                                   {166, 166},           /* 9600   */
                                   {354, 354},           /* 19200  */
                                   {346, 738},           /* 38400  */
                                   {354,1506},           /* 76800  */
                                   {354,3042},           /* 153600 */
                                   {738,6114}            /* 307200 */
                                 };

  /* The maximum frame size of MUXPDU 5 for TDSO in bits, for rate set 2. This number is a function of the
  ** supplemental channel data rate and the code type. Convolutional = 0, Turbo = 1.
  */
  LOCAL word                txc_tdso_muxpdu5_rs2[TDSO_SCH_RATE_32X+1][ 2 ] =
                                 {
                                   {261,  261},           /* 14400  */
                                   {546,  546},           /* 28800  */
                                   {538, 1122},           /* 57600  */
                                   {546, 2274},           /* 115200 */
                                   {546, 4578},           /* 230400 */
                                   {1122,9186}            /* 460800 */
                                 };

#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_IS2000_R_SCH */

  #ifdef FEATURE_PLT
#error code not present
   #endif // FEATURE_PLT

   #ifdef FEATURE_MAX_PWR_PROTECTION
     int16  tx_extra_gain = 0;
#ifdef FEATURE_IS2000_R_SCH
boolean txc_max_pwr_limited_dtx_sch = FALSE;
boolean txc_sch_max_pwr_protection_enabled = TRUE;
#endif // FEATURE_IS2000_R_SCH


   #endif /* FEATURE_MAX_PWR_PROTECTION */

word  txc_rlp_null_data = 0xFFFF;

/* number of lost vocoder packets */
word txc_lost_voc_pkts = 0;  /*lint -e552 used for logging */

LOCAL enc_rate_type curr_fch_frame_rate;  /*lint -e552 used for logging */
LOCAL enc_rate_type next_fch_frame_rate;
LOCAL log_sch_rate_enum_type curr_sch0_frame_rate; /*lint -e552 used for logging */
LOCAL log_sch_rate_enum_type next_sch0_frame_rate;
LOCAL txc_sig_frame_type next_fch_sig_type;
LOCAL txc_sig_frame_type curr_fch_sig_type; /*lint -e552 used for logging */

/* Specific variables for the Traffic Channel */
txtc_type txtc;

/* States to turn off TX at the right time in HHO cases only.  Turning off TX
** has to be done in expensive interrupt context to meet spec.  It first started
** out in task context which still failed spec, so as the final step was moved
** to interrupt context.  The turning off action is now guaranteed to happen within
** the time range allowed by spec.
**
** Default state:  Do nothing 
** Armed state:  when action time frame arrives, turn off TX 
*/
typedef enum
{
  TXC_TURN_OFF_TX_DEFAULT_S = 0,
  TXC_TURN_OFF_TX_ARMED_S = 1

} txc_turn_off_tx_for_hho_state_type;

/* TXC will command the transmitter to be shut down at the beginning of
** the action time frame, not earlier in action time frame - 20ms, which is
** when MC gives TXC the HHO action.
*/
txc_turn_off_tx_for_hho_state_type txc_turn_off_tx_for_hho_state = 
                                     TXC_TURN_OFF_TX_DEFAULT_S;

boolean txc_eib_flag;
#ifdef FEATURE_IS2000_REL_A
boolean txc_qib_flag;
boolean txc_sch_eib_flag;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_R_SCH
/*---------------------------------------------------------------------------
  Indicates if the R-SCH should be checked for throttling.  It is a word
  instead of a boolean, because some of the R-SCH params change a little
  bit asynchronously.  By setting it to some value like 3, then in 3
  frame times, the R-SCH should hopefully be okay.  And the idea is that
  the phone won't crash due to processor overload in 3 frame times.
---------------------------------------------------------------------------*/
#define TXC_SCH_CHANGED_DELAY   2
word txc_sch_changed = 0;
#endif

typedef void (*txc_transaction_processor_type) (mctrans_action_packet_type *);
  /* function prototype for transaction callback function */

#ifdef FEATURE_DS_RLP3
/*-------------------------------------------------------------------------
  This table gives the number of bits corresponding to each RLP rate.
  This is needed because, RLP gives the rate at which it has built a
  frame and Mux will then use that rate to lookup for bit count in this
  table.
------------------------------------------------------------------  */

const word rlp_to_mux_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                           [DSRLP_MUX_PDU_3] =
{
  171, 266,     168, 262,       /* Rate 1 -   full rate           */
    0,   0,     152, 242,       /* Rate 7/8 - secondary only      */
    0,   0,     128, 208,       /* Rate 3/4 - secondary only      */
   80, 124,      88, 138,       /* Rate 1/2 - half rate           */
    0,   0,       0, 121,       /* Rate 7/16- secondary only      */
    0,   0,       0, 101,       /* Rate 3/8 - secondary only      */
   40,  54,       0,  67,       /* Rate 1/4 - quarter rate        */
    0,   0,       0,  52,       /* Rate 3/16- secondary only      */
   16,  20,       0,  32,       /* Rate 1/8 - eigth rate          */
    0,   0,       0,  20,       /* Rate 1/16- secondary only      */
 };
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_IS2000_REL_A_SVD
const word dsrlp_mask_table[DSRLP_RATE_ERASURE_SCH][DSRLP_GENERIC_SRVC]
                          [ENC_RATE_14400+1] =
{
/* FCH Rate 1 */
/* Primary */
  DSRLP_RATE_1_MASK|DSRLP_RATE_1_2_MASK|DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK,  DSRLP_RATE_1_MASK|DSRLP_RATE_1_2_MASK|DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK,

/* Secondary */
  DSRLP_RATE_1_MASK, DSRLP_RATE_1_MASK|DSRLP_RATE_7_16_MASK|DSRLP_RATE_3_16_MASK|DSRLP_RATE_1_16_MASK ,

/* FCH Rate 7/8 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK|DSRLP_RATE_7_8_MASK, DSRLP_RATE_BLANK_MASK|DSRLP_RATE_7_8_MASK|DSRLP_RATE_3_8_MASK|DSRLP_RATE_1_8_MASK,

/* FCH Rate 3/4 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK|DSRLP_RATE_3_4_MASK, DSRLP_RATE_BLANK_MASK|DSRLP_RATE_3_4_MASK|DSRLP_RATE_1_4_MASK,

/* FCH Rate 1/2 */
  DSRLP_RATE_1_2_MASK|DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK, DSRLP_RATE_1_2_MASK|DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK,
  DSRLP_RATE_BLANK_MASK|DSRLP_RATE_1_2_MASK,  DSRLP_RATE_BLANK_MASK|DSRLP_RATE_1_2_MASK,

/* FCH Rate 7/16 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_7_16_MASK,

/* FCH Rate 3/8 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_3_8_MASK,

/* FCH Rate 1/4 */
  DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK, DSRLP_RATE_1_4_MASK|DSRLP_RATE_1_8_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_1_4_MASK,

/* FCH Rate 3/16 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_3_16_MASK,

/* FCH Rate 1/8 */
  DSRLP_RATE_1_8_MASK, DSRLP_RATE_1_8_MASK,
  DSRLP_RATE_BLANK_MASK,DSRLP_RATE_1_8_MASK,

/* FCH Rate 1/16 */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_1_16_MASK,

/* FCH Erasure */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,

/* FCH Blank Rate */
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK,
  DSRLP_RATE_BLANK_MASK, DSRLP_RATE_BLANK_MASK
};
#endif /* defined FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_LOG_PC_INFO_2
/*---------------------------------------------------------------------------
                 REVERSE POWER CONTROL INFO LOG TYPE
---------------------------------------------------------------------------*/
/* One channel:    (240bytes/12bytes)*20ms/frame) --> 0.40 sec per log
** Two channels:   (240bytes/16bytes)*20ms/frame) --> 0.30 sec per log
** Three channels: (240bytes/20bytes)*20ms/frame) --> 0.24 sec per log
*/
#define TXC_MAX_RPC_INFO_FRAME_BUF_SIZE 240

/* Possible values for txc_max_pwr_limited_dtx_sch
*/
typedef enum
{
  TXC_NOT_APPLICABLE = 0,
  TXC_NO             = 1,
  TXC_YES            = 2

} txc_na_no_yes_type;

typedef enum
{
  TXC_FCH  = 0,
  TXC_SCH0 = 1,
  TXC_SCH1 = 2,
  TXC_DCCH = 3
} txc_ch_type_type;

typedef struct
{
  uint8 ch_type  : 2;  /* Uses txc_ch_type_type                   */
  uint8 fch_type : 2;  /* Uses log_fch_type_enum_type (FCH only)  */
  uint8 ch_rc    : 4;  /* Uses cai_radio_config_type              */
  uint8 ch_rate;       /* Uses channel dependent type:            */
                       /*    R-FCH  uses log_r_fch_rate_enum_type */
                       /*    R-SCH0 uses log_sch_rate_enum_type   */
                       /*    R-SCH1 uses log_sch_rate_enum_type   */
                       /*    R-DCCH uses DCCH Frame Category      */
  int16 tx_pwr;        /* TX power in 0.125 dB units              */
} txc_channel_info;

/* Reverse Power Control frame record.  This structure defines each
** frame entry in the sub-record.
*/
typedef struct
{
  uint16 dec_history; /* Power control decision history */
  uint8  rx_agc;       /* RX AGC vals            */
  uint8  tx_pwr;       /* TX CAGC vals           */
  uint8  tx_gain_adj;  /* Gain adjust pctrl vals */
  uint8  txc_max_pwr_limited_dtx_sch;  /* Is max pwr protection in effect */
  uint8  cdma_tx_pwr_limit;            /* TX Power limit                  */
  uint8  ch_count;     /* Channel count          */

  /* Variable length array of channel values, up to 4 per frame. */
  /* ch_type field indicates which channel an entry is for.      */
  txc_channel_info ch_info[4];

} txc_rpc_info_log_frame_type;


/* Reverse Power Control Log Record
*/
typedef struct
{
  log_hdr_type hdr;   /* All logs must have a header */

  uint8  frame_offset;          /* Frame offset in 1.25 ms units.    */
  uint8  band_class;            /* Band Class                        */
  uint16 so;                    /* Service Option                    */
  uint8  pilot_gating_rate;     /* Pilot gating rate                 */
  uint8  step_size;             /* Power control step size           */
  uint8  frame_cnt;             /* Count of sub-records              */
  uint8  reserved;

  /* Variable length buffer of txc_rpc_info_log_frame_type frame records */
  uint8 frame_buf[TXC_MAX_RPC_INFO_FRAME_BUF_SIZE];

} txc_rpc_info_log_type;

/* Accumulation buffer for LOG_RPC_INFO_C log packet*/
LOCAL txc_rpc_info_log_type  txc_rpc_info_log_buf;

/* Temp storage of log packet length */
LOCAL int txc_rpc_info_log_length = 0;

/* Index into frame_buf of start of next frame */
#define TXC_RPC_INFO_LOG_OFFSET(cur_buf_len) \
  (cur_buf_len - FPOS(txc_rpc_info_log_type, frame_buf))

/* Threshold to determine when the buf is full and the log should be sent. */
#define TXC_RPC_INFO_BUF_THRESHOLD \
  (sizeof(txc_rpc_info_log_type) - sizeof(txc_rpc_info_log_frame_type))

#endif /* FEATURE_LOG_PC_INFO_2 */

/* The following states are the build states in every frame.  Frame building
** starts at the 1st ENC ISR of the frame.  Most of the lengthy build process 
** for RLP service options is done in task context following the 1st ENC ISR.
** By the 2nd ISR of the same frame, if the frame is ready, it is sent in ISR
** context.  If the frame is not ready, the ISR will send a null frame on the
** FCH (if assigned) and DTX the DCCH and SCH.  In the following frame, the
** 1st ENC ISR will not start a new build frame process.  The 2nd ENC ISR will
** send the late frame (built in the previous frame.) 
**
** This mechanism is used to handle situation where TXC is starved so the 
** build frame process finishes late in the current frame or overflows to
** the next frame.  By not allowing late frames to propagate, TXC should 
** recover after dropping 1 frame.
*/

typedef enum
{
  TXC_BUILD_FRAME_MIN = 0,       /* range checking */
  TXC_BUILD_FRAME_DONE = TXC_BUILD_FRAME_MIN,  /* default state */        
  TXC_BUILD_FRAME_IN_PROGRESS,   /* frame is being built */
  TXC_BUILD_FRAME_LATE,          /* building frame takes too long */
  TXC_BUILD_FRAME_BUILT,         /* frame is built, ready to send */
  TXC_BUILD_FRAME_MAX = TXC_BUILD_FRAME_BUILT  /* range checking */

} txc_build_frame_enum_type;

LOCAL txc_build_frame_enum_type txc_build_frame_state = 
                                        TXC_BUILD_FRAME_DONE;

/*===========================================================================

                 FUNCTION PROTOTYPE OF LOCAL FUNCTIONS

===========================================================================*/

#if defined (FEATURE_IS95B_MAHHO) && !defined(FEATURE_HHO_ENHANCEMENTS)
static void txc_scr_action( txc_scr_info_type scr_info );
#endif /* FEATURE_IS95B_MAHHO && !FEATURE_HHO_ENHANCEMENTS */

void txc_register_action_handlers_notifier_interrupt_func(void);
void (*txc_action_time_notifier_func_ptr)(mctrans_server_token_type, qword);

#ifdef FEATURE_IS2000_REL_A_SVD
#ifdef FEATURE_IS2000_R_SCH
void txc_max_pwr_prot (void);
void txc_tx_process_sch (dsrlp_rev_frame_block_type *);
#endif /* FEATURE_IS2000_R_SCH */

void txc_tx_post_rlp_fch_process (dsrlp_rev_frame_block_type *);

#ifdef FEATURE_IS2000_P2
void txc_tx_post_rlp_dcch_process (dsrlp_rev_frame_block_type *);
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_IS2000_REL_A_SVD*/

#ifdef FEATURE_IS2000_P2
void txc_build_dcch_frame (void);
void txc_dtx_dcch_frame (void);
#endif /* FEATURE_IS2000_P2 */

void txc_rpctrl_log_frame (byte);

#ifdef FEATURE_LOG_PC_INFO_2
txc_rpc_info_log_frame_type *txc_log_rpc_info_frame_init (void);
void                         txc_log_rpc_info            (byte);
#endif /* FEATURE_LOG_PC_INFO_2 */

void txc_stop_tx( boolean hho );

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_LOG_TC_MSG

DESCRIPTION
  This function logs messages sent on the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void  txc_log_tc_msg
(
  txc_dsch_type     dsch,     /* Signalling channel carrying the message */
  txc_msg_buf_type *msg_ptr   /* pointer to sent message */
)
{
  log_code_type log_code = dsch == TXC_REV_FCH ? LOG_R_TC_MSG_C : LOG_REV_DCCH_MSG_C;

#if !defined (FEATURE_DIAG_V7_LOG_IFACE)
  log_r_tc_msg_type *log_ptr;

  log_ptr = (log_r_tc_msg_type *) log_alloc( log_code, msg_ptr->msg[0]+sizeof(log_hdr_type));

  if (log_ptr != NULL)
  {
    /* copy message into buffer */
    (void) memcpy( (void *) log_ptr->msg,
                   (void *) msg_ptr->msg,
                   msg_ptr->msg[0] );

    log_commit((log_type *) log_ptr);
  }
#else
  txc_log_msg_type *log_ptr;   /* pointer to buffer to log msg in */

  /* get a buffer off of the txc_log_msg_q to log the received message */
  if( ( log_ptr = (txc_log_msg_type *) q_get(&txc_log_msg_q) ) == NULL )
  {
     MSG_MED( "No buf from msg_log_q", 0, 0, 0 );
  }
  else
  {
    /* set up log buffer */
    log_ptr->entry.tc_msg.hdr.len =
      msg_ptr->msg[0] + sizeof( log_hdr_type );

    log_ptr->entry.tc_msg.hdr.code = log_code;  /* set log type */
    (void) ts_get( (unsigned long *)(log_ptr->entry.tc_msg.hdr.ts ));           /* set timestamp */

    /* Copy message into buffer */
    (void) memcpy( (void *) log_ptr->entry.tc_msg.msg,
                   (void *) msg_ptr->msg,
                   msg_ptr->msg[0] );

    log_put( &log_ptr->desc );
  }
#endif /* !defined (FEATURE_DIAG_V7_LOG_IFACE) */

  /* Event report: Message transmitted */
  if ( dsch == TXC_REV_FCH )
  {
    /* R-FCH */
    mclog_report_event_msg_txed(CAI_R_FCH, msg_ptr->msg);
  }
  else
  {
    /* R-DCCH */
    mclog_report_event_msg_txed(CAI_R_DCCH, msg_ptr->msg);
  }

} /* txc_log_tc_msg */
/*===========================================================================

FUNCTION TXC_PILOT_INIT

DESCRIPTION
  Initialize reverse pilot properly based on RC.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void txc_pilot_init()
{
  if (
      #ifdef FEATURE_IS2000_P2
      txc_so.rev_dcch.included ||
      #endif // FEATURE_IS2000_P2
      txc_so.rev_fch.rc >= CAI_RC_3 )
  {
    // JRP - need not have FCH or DCCH.  Could be just Rev C/D channels
    MSG_HIGH( "En rev pilot",0 ,0, 0 );
    enc_is95c_enable( TRUE ); // IS-95C
    enc_pch_enable( TRUE );

    /* Intialize offset_pwr, so that if MS switches from low RCs to high RCs
     * tx_rate_adj is properly calculated
     */
    txtc.offset_pwr = 12 * 85 / 10;  // 8.5 * 12;
  }
  else
  {
    MSG_HIGH( "Dis rev pilot",0 ,0, 0 );
    enc_is95c_enable( FALSE ); // IS-95A
    TXC_SET_PCH_GAIN( 0 );
    TXC_SET_FCH_GAIN( 0 );
    enc_pch_enable( FALSE );

    /* Intialize tx_rate_adj and offset_pwr to 0, so that if MS switches from
     * high RCs to low RCs tx_rate_adj doesn't get set to the high RC value
     */
    txtc.tx_rate_adj = 0;
    txtc.offset_pwr  = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_TRAFFIC_INIT

DESCRIPTION
  Initialization section for traffic channel.

DEPENDENCIES
  None
RETURN VALUE
  None.
SIDE EFFECTS
  None
===========================================================================*/
void txc_traffic_init( void )
{

#ifdef FEATURE_FACTORY_TESTMODE
  int i;
#endif

#ifdef FEATURE_IS2000_REL_A
  int sr_index;
#endif /* FEATURE_IS2000_REL_A */

  txtc.power = txac.last_pwr;    /* set last access probe power level */

  /* Initialize default PCG for bin0 ENC ISRs, which is the earliest PCG to 
  ** start frame building.  Need to do this before registering 
  ** function txc_traffic_isr().
  */
  txc_slot0_isr_pcg = TXC_SLOT0_ISR_PCG_NUM;

  /* Should clear state before initializing action time processing.
  */
  txc_turn_off_tx_for_hho_state = TXC_TURN_OFF_TX_DEFAULT_S; 
  txc_register_action_handlers_notifier_interrupt_func();

  txc_build_frame_state = TXC_BUILD_FRAME_DONE; /* default setting */ 

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

  /* Convert to units of 1/2 dB and add rl_gain_adj value
  ** Note that txtc.power is sign inverted.
  */
  txtc.power -= (byte) (2 * txtc.rl_gain_adj);

  (void)memcpy( txtc.frame.data, zeros, 24 );   /* copy zeros into frame */


  /* set long code mask for traffic channel */
  txc_int_cmd.cmd = INT_TC_F;
  qw_equ( txc_int_cmd.parm.tc.mask,  txc_tx.mask );
  DO_INT_CMD();

  /* Set Traffic Channel offsets. */
  txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
  (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                     txc_traffic_isr, 
                                     ENC_STATUS_INTERRUPT_SLOT_0, 
                                     TXMOD_INT_MUX_CLT);
  (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,
                                     txc_traffic_isr,
                                     ENC_STATUS_INTERRUPT_SLOT_3, 
                                     TXMOD_INT_MUX_CLT);

  txtc.frame.sig = NORM;          /* no data for interrupt handler */

  txc_pilot_init();
   
  #ifdef FEATURE_PA_ACCESS_PROBE_CONFIG
  /* Notify RF that we are entering traffic state.  Need to do this before 
  ** calling rfm_enable_tx_pwr() for traffic state, so RF can set its pa
  ** hysterysis timer appropriately.  When leaving traffic state, RF will reset its
  ** timer when function rfm_tx_disable() is called so it's not necessary
  ** for TXC to call this function with a FALSE parameter.
  */
  rfm_set_pa_access_probe_config(RFCOM_TRANSCEIVER_0,TRUE);
  #endif //FEATURE_PA_ACCESS_PROBE_CONFIG


  /* -------------------------------------------
  ** Power up and program the VHF TX synthesizer
  ** ------------------------------------------- */
  rfm_enable_tx_pwr(RFCOM_TRANSCEIVER_0);

#ifdef FEATURE_RF_WAIT_CHANGES
#error code not present
#else
  (void) rex_set_timer( &txc_rf_timer, RF_ENABLE_PWR_WAIT );
#endif // FEATURE_RF_WAIT_CHANGES

  (void) txc_wait( TXC_RF_TIMER_SIG );
  rfm_tx_enable(RFCOM_TRANSCEIVER_0);

  enc_tx_enable (TRUE);           /* make sure power amp is on */
  txc_tx.tx_on = TRUE;            /* transmitter is on */
  rxc_set_tx_in_traffic_on(TRUE);

  /* Events report: Transmitter enabled */
  event_report(EVENT_TRANSMITTER_ENABLED);

  txc_tx.tx_blank = TRUE;         /* expect a blank first */

  srch_mux_set_tx_adj( txtc.power );  /* set power to last access probe level */

#ifndef FEATURE_IS2000_REL_A
  /* default to no primary or secondary service option */
  txc_so.pri = CAI_SO_NULL;
  txc_so.stored_pri = CAI_SO_NULL;
  txc_so.sec = CAI_SO_NULL;
#else
  /* default to no primary or secondary service option and no physical mapping */
  txc_so.pri.so = CAI_SO_NULL;
  txc_so.sec.so = CAI_SO_NULL;
  txc_so.pri.phy_channels = 0;
  txc_so.sec.phy_channels = 0;
  /* set to defautl no service option for each reference ID */
  for (sr_index = 0; sr_index < SR_ID_MAX; sr_index++)
  {
    txc_srid_ref[sr_index].so = CAI_SO_NULL;
    txc_srid_ref[sr_index].traffic_type = CAI_NUL_TRAF;
    txc_srid_ref[sr_index].phy_channels = 0;

  }
  txc_so.signaling_phy_channel = 0;

  prim_rate          = CAI_BLANK_RATE;
  prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
  prim_dcch_rate     = CAI_NULL_RATE;
  prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */

  sec_fch_num_bits   = 0;
  sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
  sec_dcch_num_bits  = 0;
  sec_dcch_data_ptr  = NULL;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
  /* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control
  ** frames when RLP is the secondary SO in a SVD call.  The purpose of the
  ** implementation is to improve voice quality in a SVD call.
  */
  txc_rlp_sig_priority.prev_fr_val = DSRLP_TX_NONE;
  txc_rlp_sig_priority.curr_fr_val = DSRLP_TX_NONE;
  txc_rlp_sig_priority.delay_fr_cnt = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */

#endif /* FEATURE_IS2000_REL_A */

  txtc.preambles = TRUE;          /* send preambles when we first get here */
  /* Preambles always in frames at TCI */
  txc_hho_preamble_pcg_units = FALSE;

  #ifdef FEATURE_IS2000_CHS
  // Initialize to no pilot gating
  txtc.gating_rate = CAI_GATING_RATE_NONE;
  #endif // FEATURE_IS2000_CHS

  //------------------------------------------------------
  // For FTM this parameter is initialized during TXC_TC_F
  //------------------------------------------------------
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_mode == DMSS_MODE)
  {
    txtc.num_preambles = 0xFFFF;
  }
#else
  txtc.num_preambles = 0xFFFF;
#endif // FEATURE_FACTORY_TESTMODE

  txtc.nulls = FALSE;             /* not sending null data yet */
  txtc.last_rate = ENC_8TH_RATE;   /* close as we get to nothing sent yet */

  /* initialize number of lost logged vocoder packets */
  txc_lost_voc_pkts = 0;

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  txc_log_mux_buf.count = 0;
#else
  if( txc_mux1_ptr ) {             /* if left over buffer - put on q */
    q_put( &txc_log_mux1_q, &txc_mux1_ptr->desc.link );
    txc_mux1_ptr = NULL;
  }
#endif

  txc_tx.active_msg = FALSE;
  txc_tx.delay = TXC_NO_CONF;              /* no confirmation delay */

#ifdef FEATURE_IS2000_R_SCH
  txtc.pot_num_mux_pdus = 0;
  txtc.ok_num_mux_pdus  = 0;

  txtc.dtx_duration_included = FALSE;
#endif /* FEATURE_IS2000_R_SCH */

  /* We are now ready to transmit */
  txtc.ready_to_transmit = TRUE;

#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef T_IO_CARD
#error code not present
#else
  if (BIO_GET_SW( BIO_SW11_M ))
  {
    /* If dipswitch 11 is ON, MAX PWR PROTECTION on R-SCH is disabled
    */
    txc_sch_max_pwr_protection_enabled = FALSE;
  }
  else
  {
    txc_sch_max_pwr_protection_enabled = TRUE;
  }
#endif /* T_IO_CARD */
#endif // FEATURE_MAX_PWR_PROTECTION
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_mode == FTM_MODE)
  {
         for( i = 0; i < ENC_14400_FULL_RATE_SIZE; i++ )
         {
#ifdef FEATURE_PLT
#error code not present
#else
           txtc.frame.data[ i ] = 0xAA;
#endif
         }
#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_32X_SCH_DATA_RATE
         for( i = 0; i < ENC_SCH_RC3_32X_SIZE; i++ )
#elif defined (FEATURE_DS_IS2000_RS2)
         for( i = 0; i < ENC_SCH_RC4_16X_SIZE; i++ )
#else
         for( i = 0; i < ENC_SCH_RC3_16X_SIZE; i++ )
#endif /* FEATURE_32X_SCH_DATA_RATE */
         {
           txtc.supp_frame.data[ i ] = 0x5A;
         }
#endif /*FEATURE_IS2000_R_SCH*/
  }
#endif /* FEATURE_FACTORY_TESTMODE */

#if defined (FEATURE_FACTORY_TESTMODE) || defined (FEATURE_PLT)

  enc_fch_enable ( FALSE );
#ifdef FEATURE_IS2000_P2
  enc_dcch_enable ( FALSE );
  if (txc_so.rev_dcch.included)
  {
    enc_dcch_enable ( TRUE );
  }
  if (txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
  {
    enc_fch_enable ( TRUE );
  }
#endif /* FTM || PLT */


  // LJL
  // Acording to interface document, if in DV mode, TXC waits for Switch before
  // setting non-zero gains. Program gains including pilot to 0
  // Can this guarantee that?

#ifdef FEATURE_PLT
#error code not present
#endif

} /* txc_traffic_init */

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_TRAFFIC_SIGNEXT

DESCRIPTION
  Process traffic channel next signaling frame type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void txc_traffic_signext ( void )
{
  /* now figure out if next frame will be dim and burst or blank and burst */
  if( txc_tx.active_msg ) {
    if( txc_tx.buf.blank )
    {
      if( txc_tx.buf.pos < txc_tx.buf.len )
      {                                      /* Not done with message? */
        txtc.frame.sig = BLANK;           /* blank and burst next */
      }
    }
    else
    {
      if( txc_tx.buf.pos < txc_tx.buf.len ) {  /* Not done with message? */
        txtc.frame.sig = DIM;              /* dim and burst next */
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_VOC_XCHG

DESCRIPTION
  Vocoder service routine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_voc_xchg (void)
{
  /*
   * This function is called from the rxc_tc_isr, which assumes that whenever
   * we receive a frame, we also transmit one.  But this is not true when we
   * a listening to the forward link before we transmit the preambles.  So
   * before transmitting, check to make sure first.
  */
  if (!txtc.ready_to_transmit)
  {
    return;
  }

  /* In the new frame processing, TXC_PRI returns only the voice data if
     this feature is defined. If the service option is data then txc_pri
     does nothing. */

  /* get the vocoder data */
  txc_pri ();

}/* txc_voc_xchg */

#ifdef FEATURE_IS2000_REL_A
/*         Variables for RDCH Release A logging         */
static uint32 raw_rdch_log_buffer[ FRDCH_LOG_BUFFER_SIZE/4  + 1] = {0};
static log_puma_rdch_information_type * txc_rdch_log_buffer =
  (log_puma_rdch_information_type *) raw_rdch_log_buffer; //lint !e740 Unusual pointer cast
static rdch_sub_record_type *txc_rdch_sub_record_ptr = NULL;
static rev_sch_information_type *rsch_frame_info = NULL;
static int txc_rdch_log_buff_offset = 0;
static int txc_rdch_log_sub_record_offset = 0;
static word txc_rdch_curr_sch_mux_opt = 0;

/*===========================================================================

FUNCTION TXC_RDCH_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated RDCH frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RDCH frame information log may be sent.

===========================================================================*/
void txc_rdch_frame_info_send_log(void)
{
  if (txc_rdch_log_buffer->hdr.len > 0)
  {
    /* Will send log if submitted. */
    (void) log_submit((log_type *) txc_rdch_log_buffer);

    /* Flag to init start of log */
    txc_rdch_log_buffer->hdr.len = 0;
  }

} /* txc_rdch_frame_info_send_log */

/*===========================================================================

FUNCTION TXC_RDCH_LOG_FRAME_INFO_INIT

DESCRIPTION
  This function initializes the RDCH frame info log subrecord, as well as the
  pointer to it and the entry for this frame, in the frame types log buffer.
  If necessary, the log is sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to frame log entry.

SIDE EFFECTS
  A RDCH frame info. log may be sent.

===========================================================================*/
rdch_frame_type  * txc_rdch_log_frame_info_init(void)
{
  rdch_frame_type  *rdch_frame_ptr = NULL;
  channels_info_type  rdch_chan_set_mask;

  /* If logging is enabled. */
  if (log_status(LOG_RDCH_FRAME_INFO_C))
  {
    /* If we reached accumulation threshold, send it.
    the threshold = the buffer size - the maximum of byte per record in the
    worst case analysic */
    if (txc_rdch_log_buffer->hdr.len >= FRDCH_LOG_BUFFER_THRESHOLD)
    {
      txc_rdch_frame_info_send_log();
    }
    /* check if there is a need to re-initialize */
    if (txc_rdch_log_buffer->hdr.len == 0)
    {
      txc_rdch_log_buffer->hdr.len = (word) FPOS(log_puma_rdch_information_type,reference_buf);
      txc_rdch_log_buffer->hdr.code = LOG_RDCH_FRAME_INFO_C;
      /* First frame, log time stamp. */
      ts_get((unsigned long *) txc_rdch_log_buffer->hdr.ts);

      /*  Fill in beginning of record   */
      txc_rdch_log_buffer->frame_offset = txc_frame_offset;
      txc_rdch_log_buffer->num_sub_records = 0;

      /* reset all the control variables */
      txc_rdch_log_buff_offset = 0;
      txc_rdch_sub_record_ptr = NULL;

    }
    else
    {
      /* If a new sub-record is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      /* clear the rdch mask */
      rdch_chan_set_mask.mask = 0;

      /* set the the mask based on the active RDCH channels */
#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_dcch.included)
      {
        rdch_chan_set_mask.individual_channels.dcch = TRUE;
      }

      if (txc_so.rev_fch.included)
#endif
      {
        rdch_chan_set_mask.individual_channels.fch = TRUE;
      }

      if (txtc.sch_on)
      {
        rdch_chan_set_mask.individual_channels.sch0 = TRUE;
      }
      /* SCCH channel, no scch on the reverse channel, set to zero */
      rdch_chan_set_mask.individual_channels.scch = 0;

      /* PICH channel */
      rdch_chan_set_mask.individual_channels.rpich = txtc.gating_rate != CAI_GATING_RATE_NONE;

      /* start a new sub-record if there is a new channel been added or
      the SCH MUX option has been changed */
      if(rdch_chan_set_mask.mask != txc_rdch_sub_record_ptr->assigned_channels.mask)
      {
        /* We no longer have a valid sub-record. */
        txc_rdch_sub_record_ptr = NULL;
      }
      else
      {
        if((txtc.sch_on) &&
           (txc_rdch_curr_sch_mux_opt != txc_so.rev_sch_mux_option))
        {
          /* We no longer have a valid sub-record. */
          txc_rdch_sub_record_ptr = NULL;
        }

      }

    }
    /* Need to init a new subrecord? */
    if (txc_rdch_sub_record_ptr == NULL)
    {
      txc_rdch_sub_record_ptr =
            (rdch_sub_record_type *) &txc_rdch_log_buffer->reference_buf[txc_rdch_log_buff_offset];
      /* Increment sub-record count. */
      txc_rdch_log_buffer->num_sub_records++;
      /* reset the number of frame counter */
      txc_rdch_sub_record_ptr->num_frames = 0;

      /* reset the rdch log sub-record offset */
      txc_rdch_log_sub_record_offset = 0;

      /* reset the channel mask */
      txc_rdch_sub_record_ptr->assigned_channels.mask = 0;

      /********Initialize the channel assignment and RC for each RDCHs ************/

      /* check for PICH */
      txc_rdch_sub_record_ptr->assigned_channels.individual_channels.rpich
                         = txtc.gating_rate != CAI_GATING_RATE_NONE;

      /* check for FCH */
#ifdef FEATURE_IS2000_P2
      if (txc_so.rev_fch.included)
#endif
      {
        txc_rdch_sub_record_ptr->assigned_channels.individual_channels.fch
                                   = TRUE;
        /* include FCH RC */
        txc_rdch_sub_record_ptr->reference_buf[txc_rdch_log_sub_record_offset++]
                                   = txc_so.rev_fch.rc;
        /* include GCH 8th gating */
        txc_rdch_sub_record_ptr->reference_buf[txc_rdch_log_sub_record_offset++]
                                   = txtc.fch_8th_rate_gating;
      }
#ifdef FEATURE_IS2000_P2
      /* check for DCCH */
      if (txc_so.rev_dcch.included)
      {
        txc_rdch_sub_record_ptr->assigned_channels.individual_channels.dcch
                                   = TRUE;
        txc_rdch_sub_record_ptr->reference_buf[txc_rdch_log_sub_record_offset++]
                                   = txc_so.rev_dcch.rc;
      }
#endif
      /* check for SCH and initialize RSCH data*/
      if (txtc.sch_on)
      {
        /* set the channel bit mask */
        txc_rdch_sub_record_ptr->assigned_channels.individual_channels.sch0
                                   = TRUE;
        rsch_frame_info = (rev_sch_information_type *)
                 &txc_rdch_sub_record_ptr->reference_buf[txc_rdch_log_sub_record_offset];

        /* RSCH RC */
        rsch_frame_info->rev_sch_rc = txtc.r_sch_rc;

        /* SCH MUX OPTION */
        if (txc_so.rev_sch_mux_option != 0)
        {
          /* RSCH0 MUX option LSB */
          rsch_frame_info->rev_sch0_mux_option_lo = txc_so.rev_sch_mux_option & 0xFF;
          /* RSCH0 MUX option MSB */
          rsch_frame_info->rev_sch0_mux_option_hi = (txc_so.rev_sch_mux_option >> 8) & 0xFF;

          /* store the MUX option for later comparasion */
          txc_rdch_curr_sch_mux_opt = txc_so.rev_sch_mux_option;
        }
        else
        {
          /* RSCH0 MUX option MSB to zero*/
          rsch_frame_info->rev_sch0_mux_option_hi = 0;
          /* RSCH0 MUX option LSB to zero*/
          rsch_frame_info->rev_sch0_mux_option_lo = 0;
        }
        /* RSCH frame length, 20 ms, 40 ms or 80 ms. For now we are only support 20 ms frame*/
        rsch_frame_info->sch_frame_length = 0;

        /* RSCH0 number of bits */
        rsch_frame_info->sch0_num_bits = txtc.supp_rate;

        /* number of MUX PDU per SCH frame */
        rsch_frame_info->num_mux_pdu_per_sch_frame = txtc.pot_num_mux_pdus;

        /* update the sub-record offset */
        txc_rdch_log_sub_record_offset += (int) sizeof(rev_sch_information_type);
      }
      /* NO SCCH in the reverse channel, Initialize the number SCCH channels to zero */
      txc_rdch_sub_record_ptr->assigned_channels.individual_channels.scch = 0;

      /* Update packet length */
      txc_rdch_log_buffer->hdr.len += (word)
          (FPOS(rdch_sub_record_type,reference_buf) + txc_rdch_log_sub_record_offset); //lint !e713 !e737 FPOS
      /*Update the buffer pointer offset */
      txc_rdch_log_buff_offset += (txc_rdch_log_sub_record_offset
                                    + (FPOS(rdch_sub_record_type,reference_buf)));//lint !e713 !e737 FPOS
    }
    /* Increment frame count */
    txc_rdch_sub_record_ptr->num_frames++;

    /* Initialize the RDCH frame record pointer */
    rdch_frame_ptr = (rdch_frame_type *)
                          &txc_rdch_sub_record_ptr->reference_buf[txc_rdch_log_sub_record_offset];
  }
  else
  {
    txc_rdch_log_buffer->hdr.len = 0; /* Flag that this record has stopped */
  }
  return rdch_frame_ptr;
}

/*=========================================================================
FUNCTION TXC_RDCH_LOG_FRAME_INFO

DESCRIPTION
  This function populates RDCH frame information logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void txc_rdch_log_frame_info (void)
{
  int rdch_logframe_offset = 0;
  int index;
  rdch_frame_type  *rdch_frame_record_ptr = NULL;
  r_sch_mux_pdu_information_type   *rsch_mux_pdu_info_ptr = NULL;

  /* Call the initialized function to init the rdch frame pointer */
  rdch_frame_record_ptr = txc_rdch_log_frame_info_init();

  if (rdch_frame_record_ptr != NULL)
  {

    /* If FCH is assigned, then including FCH format */
    if (txc_rdch_sub_record_ptr->assigned_channels.individual_channels.fch)
    {
      rdch_frame_record_ptr->reference_buf[rdch_logframe_offset++] = txtc.rev_mux;
    }
#ifdef FEATURE_IS2000_P2
    /* If DCCH is assigned, then including DCCH format */
    if (txc_rdch_sub_record_ptr->assigned_channels.individual_channels.dcch)
    {
      /* See structure definition for explaination of reference_buf */
      rdch_frame_record_ptr->reference_buf[rdch_logframe_offset++] =
         txtc.dcch_frame.frame_cat;  //lint !e661 Possible access of out-of-bounds pointer
    }
#endif
    /* If SCH is assigned, then including SCH format */
    if (txc_rdch_sub_record_ptr->assigned_channels.individual_channels.sch0)
    {
      boolean   sch_format_xmit = FALSE;

#ifdef FEATURE_IS2000_REL_A
      if (((( txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G ) ||
            ( txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G )) &&
           ( txc_rev_fr_block.num_frames > SCH_BUF_START))
#ifdef FEATURE_IS2000_TDSO
          || ((( txc_so.pri.so == CAI_SO_TDSO ) ||
               ( txc_so.pri.so == CAI_SO_FULL_TDSO ) ||
               ( txc_so.sec.so == CAI_SO_TDSO ) ||
               ( txc_so.sec.so == CAI_SO_FULL_TDSO )) &&
               ( txc_rev_fr_block.num_frames > 0))
#endif
         )
#else
      if ((( txc_so.pri == CAI_SO_PPP_PKT_DATA_3G ) &&
           ( txc_rev_fr_block.num_frames > SCH_BUF_START) )
#ifdef FEATURE_IS2000_TDSO
          || ((( txc_so.pri == CAI_SO_TDSO ) ||
               ( txc_so.pri == CAI_SO_FULL_TDSO )) &&
               ( txc_rev_fr_block.num_frames > 0))
#endif
         )
#endif /* FEATURE_IS2000_REL_A */
      {
        sch_format_xmit = TRUE;
      }
      /* Including the SCH frame format */
      /* See structure definition for explaination of reference_buf */
      rdch_frame_record_ptr->reference_buf[rdch_logframe_offset++]
                               = sch_format_xmit;  //lint !e661 !e662 Possible access of out-of-bounds pointer
      /* if the SCH_FORMAT indicated that this frame was transmitted,
      include NUM_MUX_PDU_PER_SCH_FRAME instance of SR_ID in the record*/
      if (sch_format_xmit)
      {
        /* Including FSCH MUX PDUs for each FSCH frame */
        for (index = 0; index < rsch_frame_info->num_mux_pdu_per_sch_frame; index++)
        {
          /* intitialize the FSCH MUX PDU pointer */
          /* See structure definition for explaination of reference_buf */
          rsch_mux_pdu_info_ptr = (r_sch_mux_pdu_information_type *)
                        &rdch_frame_record_ptr->reference_buf[rdch_logframe_offset++];   //lint !e661 !e662 Possible access of out-of-bounds pointer

          rsch_mux_pdu_info_ptr->sr_id = txc_rev_fr_block.sr_id; //lint !e661 !e662 Possible access of out-of-bounds pointer
        }
      }
    }
    /* Updated the total lenght of the record header */
    txc_rdch_log_buffer->hdr.len += (word) rdch_logframe_offset;
    /* update the offset variables */
    txc_rdch_log_sub_record_offset += rdch_logframe_offset;
    txc_rdch_log_buff_offset += rdch_logframe_offset;
  }
} /* end of txc_rdch_log_frame_info */

#else

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_FRAME_TYPES_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse frame types log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A rev frame types log may be sent.

===========================================================================*/
void txc_frame_types_send_log(void)
{
  if (txc_log_frm_types_buf.hdr.len != 0)
  {
    /* Will send log if enabled. */
    log_submit((log_type *) &txc_log_frm_types_buf);

    /* Flag to init start of log */
    txc_log_frm_types_buf.hdr.len = 0;
  }

} /* txc_frame_types_send_log */


/* <EJECT> */
/*===========================================================================

FUNCTION TXC_FRAME_TYPES_LOG_INIT

DESCRIPTION
  This function initializes the frame types log subrecord, as well as the
  pointer to it, in the frame types log buffer.  If necessary, the log is
  sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A rev frame types log may be sent.

===========================================================================*/
static boolean txc_sch_log_frame_type_data = FALSE;

log_rev_frame_types_frm_type * txc_log_frame_type_frm_init(void)
{
  log_rev_frame_types_frm_type *frm_ptr = NULL;
  log_ded_rev_chan_set_type chan_set_mask;

  /* If logging is enabled. */
  if (log_status(LOG_REV_FRAME_TYPES_C)
#ifdef FEATURE_LOG_PC_INFO_2_EXCLUSIVE
      /* The REV_FRAME_TYPES log message is a subset of the RPC_INFO log */
      /* message, so only log it if RPC_INFO is not being logged.   */
      && !log_status(LOG_RPC_INFO_C)
#endif /* FEATURE_LOG_PC_INFO_2_EXCLUSIVE */
     )
  {
    /* If we reached accumulation threshold, send it. */
    if (txc_log_frm_types_buf.hdr.len >= LOG_REV_FRAME_TYPES_BUF_THRESHOLD)
    {
      txc_frame_types_send_log();
    }

    if (txc_log_frm_types_buf.hdr.len == 0) /* Need to re-init */
    {
      txc_log_frm_types_buf.hdr.len = FPOS(log_rev_frame_types_type, var_len_buf);

      txc_log_frm_types_buf.hdr.code = LOG_REV_FRAME_TYPES_C;

      /* First frame, log time stamp. */
      (void) ts_get((unsigned long *) txc_log_frm_types_buf.hdr.ts);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Frame offset in 1.25 ms units */
      txc_log_frm_types_buf.frame_offset = txc_frame_offset;
#ifndef FEATURE_IS2000_REL_A
      /* Service Option */
      txc_log_frm_types_buf.so = txc_so.pri;
#else
      txc_log_frm_types_buf.so = txc_so.pri.so;
#endif /* FEATURE_IS2000_REL_A */

      /* FFPC Primary Channel */
      #ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_fch.included )
      #endif  // FEATURE_IS2000_P2
      {
        txc_log_frm_types_buf.rev_ch_rc.r_fch_rc = txc_so.rev_fch.rc;
      }
      #ifdef FEATURE_IS2000_P2
      else
      {
        txc_log_frm_types_buf.rev_ch_rc.r_fch_rc = 0;
      }

      if ( txc_so.rev_dcch.included )
      {
        txc_log_frm_types_buf.rev_ch_rc.r_dcch_rc = txc_so.rev_dcch.rc;
      }
      else
      #endif  // FEATURE_IS2000_P2
      {
        txc_log_frm_types_buf.rev_ch_rc.r_dcch_rc = 0;
      }

#ifdef FEATURE_IS2000_R_SCH
      if ( txtc.sch_on )
      {
        txc_log_frm_types_buf.rev_ch_rc.r_sch0_rc = txtc.r_sch_rc;
      }
      else
#endif /* FEATURE_IS2000_R_SCH */
      {
        txc_log_frm_types_buf.rev_ch_rc.r_sch0_rc = 0;
      }

      txc_log_frm_types_buf.rev_ch_rc.r_sch1_rc = 0; /* not supported */

      /* We no longer have a valid sub-record. */
      txc_log_frm_types_buf.sr_cnt = 0;
      txc_log_frm_types_sr_ptr     = NULL;

    }
    else
    {
      /* If a new subrecord is needed, i.e. the channel set has changed,
      ** check for the condition here.
      */
      chan_set_mask.mask = 0;

      chan_set_mask.chan_set.r_fch  = FALSE;
#ifdef FEATURE_IS2000_P2
      chan_set_mask.chan_set.r_dcch = FALSE;
#endif  /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_dcch.included )
      {
        chan_set_mask.chan_set.r_dcch = TRUE;
      }

      if ( txc_so.rev_fch.included )
#endif  /* FEATURE_IS2000_P2 */
      {
        chan_set_mask.chan_set.r_fch  = TRUE;
      }

      chan_set_mask.chan_set.r_sch0 =
               (txc_sch_log_frame_type_data) ? TRUE : FALSE;

      if(chan_set_mask.mask != txc_log_frm_types_sr_ptr->ch_set_mask.mask)
      {
        /* We no longer have a valid sub-record. */
        txc_log_frm_types_sr_ptr = NULL;
      }
#ifdef FEATURE_IS2000_R_SCH
      if ( txtc.sch_on )
      {
        txc_log_frm_types_buf.rev_ch_rc.r_sch0_rc = txtc.r_sch_rc;
      }
#endif
    }

    /* Need to init a new subrecord? */
    if (txc_log_frm_types_sr_ptr == NULL)
    {
      /* Init sub-record pointer by finding the offset into the variable length buf.
      ** TXC_FRM_TYPES_LOG_OFFSET is calculated from the length of the log packet.
      */
      txc_log_frm_types_sr_ptr =
        (log_rev_frame_types_subrecord_type *)
          &txc_log_frm_types_buf.var_len_buf.buf_ref[TXC_FRM_TYPES_LOG_OFFSET];

      /* Increment sub-record count. */
      txc_log_frm_types_buf.sr_cnt++;

      /* Init all bits in mask, and len_per_frame */
      txc_log_frm_types_sr_ptr->ch_set_mask.mask = 0;
      txc_log_frm_types_sr_ptr->len_per_frame    = 0;

#ifdef FEATURE_IS2000_P2
      txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_dcch = FALSE;
#endif  /* FEATURE_IS2000_P2 */
      txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_fch  = FALSE;

#ifdef FEATURE_IS2000_P2
      if ( txc_so.rev_dcch.included )
      {
        txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_dcch = TRUE;

        /* Increase length per frame value. */
        txc_log_frm_types_sr_ptr->len_per_frame +=
          sizeof(log_rev_frame_types_frm_type);
      }

      if ( txc_so.rev_fch.included )
#endif  /* FEATURE_IS2000_P2 */
      {
        txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_fch  = TRUE;

        /* Increase length per frame value. */
        txc_log_frm_types_sr_ptr->len_per_frame +=
          sizeof(log_rev_frame_types_frm_type);
      }

      if (txc_sch_log_frame_type_data)
      {
        /* R-SCH0 is active */
        txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_sch0 = 1;

        /* Increase length per frame value. */
        txc_log_frm_types_sr_ptr->len_per_frame +=
          sizeof(log_rev_frame_types_frm_type);

      }
      /* Init frame count */
      txc_log_frm_types_sr_ptr->frame_cnt = 0;

      /* Update packet length */
      txc_log_frm_types_buf.hdr.len +=
        FPOS(log_rev_frame_types_subrecord_type, var_len_buf);

    }

    /* Increment frame count */
    txc_log_frm_types_sr_ptr->frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** TXC_FRM_TYPES_LOG_OFFSET is calculated from the length of the log.
    */
    frm_ptr = (log_rev_frame_types_frm_type *)
        &txc_log_frm_types_buf.var_len_buf.buf_ref[TXC_FRM_TYPES_LOG_OFFSET];
  }
  else /* Log code not enabled */
  {
    txc_log_frm_types_buf.hdr.len = 0; /* Flag that this record has stopped */
  }
  /* reset the flag for the last TX frame of the call */
  txc_sch_log_frame_type_data = txtc.sch_on;
  return frm_ptr;

} /* txc_log_frame_type_frm_init */

/* <EJECT> */
/*=========================================================================
FUNCTION TXC_LOG_FRAME_TYPE

DESCRIPTION
  This function populates frame type logs.

DEPENDENCIE
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void
txc_log_frame_type (void)
{
  log_rev_frame_types_frm_type *frm_ptr;

  frm_ptr = txc_log_frame_type_frm_init();

  if (frm_ptr != NULL)
  {
    /* Start recording per channel data */
    if (txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_fch)
    {
      frm_ptr->r_fch.rate = curr_fch_frame_rate;
      frm_ptr->r_fch.type = curr_fch_sig_type;

      frm_ptr++; /* Increase sizeof 1 frame entry */
    }

    if (txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_sch0)
    {
      frm_ptr->r_sch0 = curr_sch0_frame_rate;

      frm_ptr++; /* Increase sizeof 1 frame entry */
    }

    #ifdef FEATURE_IS2000_P2
    if (txc_log_frm_types_sr_ptr->ch_set_mask.chan_set.r_dcch)
    {
      frm_ptr->r_dcch = txtc.dcch_frame.frame_cat;

      frm_ptr++; /* Increase sizeof 1 frame entry */
    }
    #endif /* FEATURE_IS2000_P2 */

    /* TODO: Add other channels here! */

    /* end per channel data */

    /* Update packet length */
    txc_log_frm_types_buf.hdr.len += txc_log_frm_types_sr_ptr->len_per_frame;

  } /* If logging enabled */
} /* txc_log_frame_type */

#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION       TXC_CALC_R_SCH_THROTTLE

DESCRIPTION    This figures out the R-SCH throttling parameters.

DEPENDENCIES   None

SIDE EFFECTS   None

RETURN VALUE   None
===========================================================================*/
void txc_calc_r_sch_throttle(void)
{
  boolean             rxc_sch_data_is_valid;
  dec_rc_type         sch_rc;
  dec_sch_rate_type   dec_sch_rate;
  boolean             double_size;

  if (txtc.sch_on == TRUE)
  {
    /*-----------------------------------------------------------------------
      The number of PDU's to build is analogous to the "X".  So 1X has one
      PDU, 2X has two PDU's.  The key is that it assumes single-size PDU's.
    -----------------------------------------------------------------------*/
    txtc.pot_num_mux_pdus = 1 << (txtc.supp_rate - ENC_SCH_FULL_RATE);
#ifdef FEATURE_32X_SCH_DATA_RATE
       /* for 32X, we want to limit the number of potential PDU as 16X case */
       if (txtc.supp_rate == ENC_SCH_32X_RATE)
       {
         txtc.pot_num_mux_pdus = txtc.pot_num_mux_pdus >> 1;
       }
#endif /* FEATURE_32X_SCH_DATA_RATE */
#ifdef FEATURE_IS2000_REL_A
    if ((txtc.double_sized == TRUE) || ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
                                             && (txtc.supp_rate > ENC_SCH_FULL_RATE)))
#else
    if (txtc.double_sized == TRUE)
#endif /* FEATURE_IS2000_REL_A */
    {
      /*-----------------------------------------------------------------------
        If using double-sized PDU's, then the number of PDU's is split
        in half.
      -----------------------------------------------------------------------*/
      txtc.pot_num_mux_pdus = txtc.pot_num_mux_pdus >> 1;
    }

#ifdef FEATURE_IS2000_TDSO
    /*-----------------------------------------------------------------------
      Don't throttle for TDSO.  Do this by setting the ok number of pdu's
      equal to the pot(ential) number of mux pdus.
    -----------------------------------------------------------------------*/
#ifndef FEATURE_IS2000_REL_A
    if ((txc_so.pri == CAI_SO_TDSO) ||
        (txc_so.pri == CAI_SO_FULL_TDSO))
#else
    if ((txc_so.pri.so == CAI_SO_TDSO) ||
        (txc_so.pri.so == CAI_SO_FULL_TDSO))
#endif /* FEATURE_IS2000_REL_A */
    {
      txtc.ok_num_mux_pdus  = txtc.pot_num_mux_pdus;
    }
    else
#endif /* FEATURE_IS2000_TDSO */
    {
      rxc_sch_data_is_valid = rxc_get_sch_info(&sch_rc, 
                                           &dec_sch_rate, 
                                           &double_size);
#ifdef FEATURE_DATA_MM
#ifdef FEATURE_PRE_DV_RLP_INTERFACE
      txtc.ok_num_mux_pdus  = ds707_scrm_max_allowable_sch_rate();
#else
  /* The parameters are introduced to minimize the RPC for 7500 */
      ds707_scrm_max_allowable_sch_rate(
                                                     rxc_sch_data_is_valid,
                                                     sch_rc,
                                                     dec_sch_rate,
                                                     double_size,
                                                     rxc_sup_dec_int_enable);
#endif /* FEATURE_PRE_DV_RLP_INTERFACE */
#else
#ifdef FEATURE_PRE_DV_RLP_INTERFACE
      txtc.ok_num_mux_pdus  = ds_scrm_max_allowable_sch_rate();
#else
      txtc.ok_num_mux_pdus  = ds_scrm_max_allowable_sch_rate(
                                                     rxc_sch_data_is_valid,
                                                     sch_rc,
                                                     dec_sch_rate,
                                                     double_size,
                                                     rxc_sup_dec_int_enable);
#endif /* FEATURE_PRE_DV_RLP_INTERFACE */
#endif
    }

    txtc.ok_num_mux_pdus = MIN(txtc.pot_num_mux_pdus, txtc.ok_num_mux_pdus);

    if (txtc.ok_num_mux_pdus < txtc.pot_num_mux_pdus)
    {
      MSG_HIGH("R-SCH self-throttle pdus: %d -> %d",
                txtc.pot_num_mux_pdus,txtc.ok_num_mux_pdus,0);
    }
    else
    {
      MSG_HIGH("R-SCH not throttled pdus: %d",
                txtc.pot_num_mux_pdus,0,0);
    }
  }
  else /* no supps assigned or DTX */
  {
    txtc.pot_num_mux_pdus = 0;
    txtc.ok_num_mux_pdus  = 0;
  }
}
#endif /*FEATURE_IS2000_R_SCH*/

#ifdef FEATURE_RLP_LOGGING
/*===========================================================================
FUNCTION     TXC_LOG_RLP3

DESCRIPTION  This function logs RLP 3 frames ready for xmit.

DEPENDENCIES Assumes frames are already built.  Also assumes log mask is
             turned on.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_log_rlp3
(
  dsrlp_rev_frame_block_type *rev_fr_block_ptr   /* RLP frames to be tx'ed */
)
{
  dsrlp_rev_frame_type *rev_fr_ptr;               /* Ptr to 1 RLP tx frame */
  log_rlp_pkt_type *rlp_frame_log_ptr;
  word              log_id;
  word              loop_cntr;
  byte              i;
#ifdef FEATURE_IS2000_R_SCH
  word              log_sch_frame_bits;
  word              log_sch_phy_chan_rate;
#endif /* FEATURE_IS2000_R_SCH */
  word              chan_rate;
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
  word txc_rlp_skip_log_count = 0;
  boolean           rlp_begin_new_subrecord = TRUE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_IS2000_R_SCH
  /* Generalize for MUX_PDU != 3 or 1 */
  if (txtc.supp_rate == ENC_SCH_FULL_RATE)
  {
#ifdef FEATURE_DS_IS2000_RS2
     if (txtc.r_sch_rc == CAI_RC_4)
     {
        log_sch_phy_chan_rate = 144;
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
        {
          /* (total number of bytes for MUX PDU5 + 1(mux header)) * 8 bits/byte) - 6 bits( 6 bits header) */
          log_sch_frame_bits = ((txc_muxpdu5_rs2_maxsize[txtc.supp_rate ][txtc.supp_turbo]) + 1)*8 - 6;//lint !e734 Loss of precision (assignment) (19 bits to 16 bits)
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */
         log_sch_frame_bits = 266;
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* FEATURE_IS2000_REL_A */
     }
     else
#endif /* FEATURE_DS_IS2000_RS2 */
     {
        log_sch_phy_chan_rate = 96;
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
        {
          /* (total number of bytes for MUX PDU5 + 1(mux header)) * 8 bits/byte) - 6 bits( 6 bits header) */
          log_sch_frame_bits = ((txc_muxpdu5_rs1_maxsize[txtc.supp_rate ][txtc.supp_turbo]) + 1)*8 - 6;//lint !e734 Loss of precision (assignment) (19 bits to 16 bits)
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */
         log_sch_frame_bits = 171;
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* FEATURE_IS2000_REL_A */
     }
  }
  else
  {
    /* since in MUX PDU5, we don't have the concept of double sided, so txtc.double_sized is
     * always set to FALSE, we only handle MUX PDU5 in the else part
     */
    if (txtc.double_sized == TRUE)
    {
#ifdef FEATURE_DS_IS2000_RS2
     /*-------------------------------------------------------------------
       For double sized PDU a data block contains 538 bits if it is RS2
       and 346 bits if it is RS1.
      --------------------------------------------------------------------*/
     if (txtc.r_sch_rc == CAI_RC_4)
     {
        log_sch_frame_bits = 538;
     }
     else
#endif /* FEATURE_DS_IS2000_RS2 */
     {
      log_sch_frame_bits = 346;
     }
    }
    else
    {
#ifdef FEATURE_DS_IS2000_RS2
     /*------------------------------------------------------------------
       For single sized PDU a data block contains 266 bits if it is RS2
       and 170 bits if it is RS1.
      --------------------------------------------------------------------*/
     if (txtc.r_sch_rc == CAI_RC_4)
     {
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
        {
          /* (total number of bytes for MUX PDU5 + 1(mux header)) * 8 bits/byte) - 6 bits( 6 bits header) */
          log_sch_frame_bits = ((txc_muxpdu5_rs2_maxsize[txtc.supp_rate ][txtc.supp_turbo]) + 1)*8 - 6;//lint !e734 Loss of precision (assignment) (19 bits to 16 bits)
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */
         log_sch_frame_bits = 266; /* (560 - 6*2 (header) - 160) / 2  */
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* FEATURE_IS2000_REL_A */
     }
     else
#endif /* FEATURE_DS_IS2000_RS2 */
     {
#ifdef FEATURE_IS2000_REL_A
       if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
       {
          /* (total number of bytes for MUX PDU5 + 1(mux header)) * 8 bits/byte) - 6 bits( 6 bits header) */
          log_sch_frame_bits = ((txc_muxpdu5_rs1_maxsize[txtc.supp_rate ][txtc.supp_turbo]) + 1)*8 - 6;//lint !e734 Loss of precision (assignment) (19 bits to 16 bits)
       }
       else
       {
#endif /* FEATURE_IS2000_REL_A */
          log_sch_frame_bits = 170;
#ifdef FEATURE_IS2000_REL_A
       }
#endif /* FEATURE_IS2000_REL_A */
     }
    }

    if (txtc.r_sch_rc == CAI_RC_4)
    {
        log_sch_phy_chan_rate = (1 << (txtc.supp_rate - ENC_SCH_FULL_RATE)) * 144;
    }
    else
    {
        log_sch_phy_chan_rate = (1 << (txtc.supp_rate - ENC_SCH_FULL_RATE)) * 96;
    }

  }
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
  /*-----------------------------------------------------------------------
    The zero'th element is the FCH.  pot_num_mux_pdus is the number of
    rlp frames sent on SCH, so add 1 to include FCH.
  -----------------------------------------------------------------------*/
  loop_cntr = txtc.pot_num_mux_pdus + SCH_BUF_START;
#else
  /*-----------------------------------------------------------------------
    FCH only
  -----------------------------------------------------------------------*/
  loop_cntr = SCH_BUF_START;
#endif /* FEATURE_IS2000_R_SCH */
  for( i = 0; i < loop_cntr; i++)
  {
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
    /* in case of P3 or VP2, we should not log FCH when it carries voice only */
    if (((i == FCH_BUF) && ((!txc_so.rev_fch.included) ||
         ((!DS_ISDATA (txc_so.pri.so)) && (sec_fch_num_bits == 0))))
        || ((!txc_so.rev_dcch.included) && (i == DCCH_BUF) ) )
    {
      txc_rlp_skip_log_count = txc_rlp_skip_log_count + 1;
      continue;
    }
#else
    #ifdef FEATURE_IS2000_P2
    /* in case of P3 or VP2, we should not log FCH when it carries voice only */
    if ( ((!txc_so.rev_fch.included) && (i == FCH_BUF)) ||
         ((!txc_so.rev_dcch.included) && (i == DCCH_BUF) ) )
    {
      continue;
    }
    #endif // FEATURE_IS2000_P2
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
    rlp_frame_log_ptr = dsrlp_log_tx_init_frame();
    if (rlp_frame_log_ptr == NULL)
    {
      break;
    }
    else
    {
      log_id = dsrlp_log_id_table[i];
      /*-------------------------------------------------------------------
        Access the frame immediately.  Use it whenever possible, to be
        generic across FCH & SCH logging.
      -------------------------------------------------------------------*/
      rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[i]);
      dsrlp_log_tx_rlp_frm_buf.service_ID = rev_fr_block_ptr->sr_id;

      rlp_frame_log_ptr->mux_chan.phy_chan = log_id;


      rlp_frame_log_ptr->mux_chan.mux_pdu = rev_fr_ptr->mux_pdu_type;

#ifdef FEATURE_IS2000_R_SCH
      /*-------------------------------------------------------------------
        Messy part.  phy chan rate has to be calc'd separately for FCH &
        SCH.
      -------------------------------------------------------------------*/
      if ( i < SCH_BUF_START )
#endif /* FEATURE_IS2000_R_SCH */
      {
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
        if (rlp_begin_new_subrecord)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
        {
          if (rlp_frame_log_ptr == &dsrlp_log_tx_rlp_frm_buf.var_len_buf.pkt[0])
          {
            /* This is the existing frame, not the "next" frame. */
            rlp_frame_log_ptr->time_rate.time_delta = 0;
          }
          else
          {
            rlp_frame_log_ptr->time_rate.time_delta = 1;
          }
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
          rlp_begin_new_subrecord = FALSE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
        }
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
        else
        {
          /* if both DCCH and FCH are included, and when the starting the
          new log, both must have the same time as the time stamp in the header*/

          rlp_frame_log_ptr->time_rate.time_delta = 0;
        }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */

        /*-----------------------------------------------------------------
         Get the physical channel rate for the given dsrlp rate.
        -----------------------------------------------------------------*/
#ifdef FEATURE_IS2000_P2
        if (i == DCCH_BUF)
        {
          chan_rate = (ENC_RC_TO_RATE_SET( txc_so.rev_dcch.rc ) == ENC_RATE_14400) ? 144 : 96;
        }
        else
#endif // FEATURE_IS2000_P2
        {
          chan_rate = (ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ) == ENC_RATE_14400) ? 144 : 96;
        }
        if(rev_fr_block_ptr->rev_arr->srvc_type == DSRLP_PRIMARY_SRVC)
        {
          switch (rev_fr_ptr->fr_rate)
          {
             /* Full Rate */
            case DSRLP_RATE_1:
              break;

             /* Half Rate */
            case DSRLP_RATE_1_2:
              chan_rate = chan_rate >> 1;
              break;

             /* Quater Rate */
            case DSRLP_RATE_1_4:
              chan_rate = chan_rate >> 2;
              break;

               /* Eighth Rate */
            case DSRLP_RATE_1_8:
              chan_rate = chan_rate >> 3;
              break;

             /* Zero Rate */
            case DSRLP_RATE_BLANK:
              chan_rate = 0;
              break;

            default:
              ERR("Bad Pri cat %d",rev_fr_ptr->fr_rate,0,0);
              break;
          }
        }
        else if(rev_fr_block_ptr->rev_arr->srvc_type == DSRLP_SECONDARY_SRVC)
        {

          switch (rev_fr_ptr->fr_rate)
          {
            /* Full Rate */
            case DSRLP_RATE_1:
            case DSRLP_RATE_7_8:
            case DSRLP_RATE_3_4:
            case DSRLP_RATE_1_2:
              break;

             /* Half Rate */
            case DSRLP_RATE_7_16:
            case DSRLP_RATE_3_8:
            case DSRLP_RATE_1_4:
              chan_rate = chan_rate >> 1;
              break;

             /* Quarter Rate */
            case DSRLP_RATE_3_16:
            case DSRLP_RATE_1_8:
              chan_rate = chan_rate >> 2;
              break;

             /* Eighth Rate */
            case DSRLP_RATE_1_16:
              chan_rate = chan_rate >> 3;
              break;

             /* Zero Rate */
            case DSRLP_RATE_BLANK:
              chan_rate = 0;
              break;

            default:
              ERR("Bad Sec Cat %d",rev_fr_ptr->fr_rate,0,0);
              break;
          }
        }

        else
        {
          ERR("Bad Serv Cat %d",rev_fr_block_ptr->rev_arr->srvc_type,0,0);
        }
        rlp_frame_log_ptr->time_rate.phy_chan_rate = chan_rate;
        rlp_frame_log_ptr->frame_len = ((rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK)?
                                         0:rlp_to_mux_table[rev_fr_ptr->fr_rate]
                                         [rev_fr_block_ptr->rev_arr->srvc_type]
                                         [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, 
                                                           DSRLP_MUX_PDU_3)]);
        rlp_frame_log_ptr->payload_len.head = ((rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK)?
                                                0:(rlp_frame_log_ptr->frame_len + 7) / 8);
      }
#ifdef FEATURE_IS2000_R_SCH
      else
      {
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
        /*-----------------------------------------------------------------
          All else is SCH.  Delta is always same as FCH.
        -----------------------------------------------------------------*/
        /* check if there is no RLP log for FCH or DCCH */
        if (txc_rlp_skip_log_count == 2)
        {
          if (i == (DCCH_BUF + 1))
          {
            if (rlp_frame_log_ptr ==
                  &dsrlp_log_tx_rlp_frm_buf.var_len_buf.pkt[0])
            {
              /* same time as the header */
              rlp_frame_log_ptr->time_rate.time_delta    = 0;
            }
            else
            {
              /* derive the time from the previous frame */
              rlp_frame_log_ptr->time_rate.time_delta    = 1;
            }
          }
          else
          {
            /* same time as the previous SCH log */
            rlp_frame_log_ptr->time_rate.time_delta    = 0;
          }
        }
        else
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
        {
          /* same time as the FCH or DCCH rlp log record */
          rlp_frame_log_ptr->time_rate.time_delta = 0;
        }
        rlp_frame_log_ptr->time_rate.phy_chan_rate = log_sch_phy_chan_rate;
        rlp_frame_log_ptr->frame_len = log_sch_frame_bits;
        rlp_frame_log_ptr->payload_len.head = DSRLP_SCH_PAYLOAD_LOG_LEN;
      }
#endif /* FEATURE_IS2000_R_SCH */

      rlp_frame_log_ptr->payload_len.tail = 0; /*           No tail!     */

      if ( (i < rev_fr_block_ptr->num_frames) &&
           (rev_fr_ptr->fr_item_ptr != NULL) )
      {
        dsrlp_log_tx_rlp_frm_buf.hdr.len += rlp_frame_log_ptr->payload_len.head;

        /* Copy frame data as head payload. */
        memcpy((void*) rlp_frame_log_ptr->payload,
               rev_fr_ptr->fr_item_ptr->data_ptr,
               rlp_frame_log_ptr->payload_len.head);
      }
      else
      {
        rlp_frame_log_ptr->frame_len = 0;
        rlp_frame_log_ptr->payload_len.head = 0;
        rlp_frame_log_ptr->payload_len.tail = 0;
      }
    }
  } /* for */
} /* txc_log_rlp3() */

/*===========================================================================
FUNCTION     TXC_LOG_FDCH_RLP3

DESCRIPTION  This function logs fundicated RLP 3 frames ready for xmit.

DEPENDENCIES Assumes frames are already built.  Also assumes log mask is
             turned on.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_log_fdch_rlp3
(
  dsrlp_rev_frame_block_type *rev_fr_block_ptr   /* RLP frames to be tx'ed */
)
{
  dsrlp_rev_frame_type *rev_fr_ptr;               /* Ptr to 1 RLP tx frame */
  log_rlp_pkt_type *rlp_frame_log_ptr;
  word              log_id;
  word              loop_cntr;
  byte              i;
  word              chan_rate;
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
  word txc_rlp_skip_log_count = 0;
  boolean           rlp_begin_new_subrecord = TRUE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*-----------------------------------------------------------------------
    FCH only
  -----------------------------------------------------------------------*/
  loop_cntr = SCH_BUF_START;

  for( i = 0; i < loop_cntr; i++)
  {
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
    /* in case of P3 or VP2, we should not log FCH when it carries voice only */
    if (((i == FCH_BUF) && ((!txc_so.rev_fch.included) ||
         ((!DS_ISDATA (txc_so.pri.so)) && (sec_fch_num_bits == 0))))
        || ((!txc_so.rev_dcch.included) && (i == DCCH_BUF) ) )
    {
      txc_rlp_skip_log_count = txc_rlp_skip_log_count + 1;
      continue;
    }
#else
    #ifdef FEATURE_IS2000_P2
    /* in case of P3 or VP2, we should not log FCH when it carries voice only */
    if ( ((!txc_so.rev_fch.included) && (i == FCH_BUF)) ||
         ((!txc_so.rev_dcch.included) && (i == DCCH_BUF) ) )
    {
      continue;
    }
    #endif // FEATURE_IS2000_P2
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
    rlp_frame_log_ptr = dsrlp_log_tx_init_frame();
    if (rlp_frame_log_ptr == NULL)
    {
      break;
    }
    else
    {
      log_id = dsrlp_log_id_table[i];
      /*-------------------------------------------------------------------
        Access the frame immediately.  Use it whenever possible, to be
        generic across FCH & SCH logging.
      -------------------------------------------------------------------*/
      rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[i]);
      dsrlp_log_tx_rlp_frm_buf.service_ID = rev_fr_block_ptr->sr_id;
      rlp_frame_log_ptr->mux_chan.phy_chan = log_id;
      rlp_frame_log_ptr->mux_chan.mux_pdu = rev_fr_ptr->mux_pdu_type;

      {
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
        if (rlp_begin_new_subrecord)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
        {
          if (rlp_frame_log_ptr ==
              &dsrlp_log_tx_rlp_frm_buf.var_len_buf.pkt[0])
          {
            /* This is the existing frame, not the "next" frame. */
            rlp_frame_log_ptr->time_rate.time_delta = 0;
          }
          else
          {
            rlp_frame_log_ptr->time_rate.time_delta = 1;
          }
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
          rlp_begin_new_subrecord = FALSE;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */
        }
#if defined(FEATURE_IS2000_P2) && defined(FEATURE_IS2000_REL_A)
        else
        {
          /* if both DCCH and FCH are included, and when the starting the
          new log, both must have the same time as the time stamp in the header*/

          rlp_frame_log_ptr->time_rate.time_delta = 0;
        }
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A */

        /*-----------------------------------------------------------------
         Get the physical channel rate for the given dsrlp rate.
        -----------------------------------------------------------------*/

#ifdef FEATURE_IS2000_P2
        if (i == DCCH_BUF)
        {
          chan_rate = (ENC_RC_TO_RATE_SET( txc_so.rev_dcch.rc ) == ENC_RATE_14400) ? 144 : 96;
        }
        else
#endif // FEATURE_IS2000_P2
        {
          chan_rate = (ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ) == ENC_RATE_14400) ? 144 : 96;
        }
        if(rev_fr_block_ptr->rev_arr->srvc_type == DSRLP_PRIMARY_SRVC)
        {
          switch (rev_fr_ptr->fr_rate)
          {
             /* Full Rate */
            case DSRLP_RATE_1:
              break;

             /* Half Rate */
            case DSRLP_RATE_1_2:
              chan_rate = chan_rate >> 1;
              break;

             /* Quater Rate */
            case DSRLP_RATE_1_4:
              chan_rate = chan_rate >> 2;
              break;

               /* Eighth Rate */
            case DSRLP_RATE_1_8:
              chan_rate = chan_rate >> 3;
              break;

             /* Zero Rate */
            case DSRLP_RATE_BLANK:
              chan_rate = 0;
              break;

            default:
              ERR("Bad Pri cat %d",rev_fr_ptr->fr_rate,0,0);
              break;
          }
        }
        else if(rev_fr_block_ptr->rev_arr->srvc_type == DSRLP_SECONDARY_SRVC)
        {

          switch (rev_fr_ptr->fr_rate)
          {
            /* Full Rate */
            case DSRLP_RATE_1:
            case DSRLP_RATE_7_8:
            case DSRLP_RATE_3_4:
            case DSRLP_RATE_1_2:
              break;

             /* Half Rate */
            case DSRLP_RATE_7_16:
            case DSRLP_RATE_3_8:
            case DSRLP_RATE_1_4:
              chan_rate = chan_rate >> 1;
              break;

             /* Quarter Rate */
            case DSRLP_RATE_3_16:
            case DSRLP_RATE_1_8:
              chan_rate = chan_rate >> 2;
              break;

             /* Eighth Rate */
            case DSRLP_RATE_1_16:
              chan_rate = chan_rate >> 3;
              break;

             /* Zero Rate */
            case DSRLP_RATE_BLANK:
              chan_rate = 0;
              break;

            default:
              ERR("Bad Sec cat %d",rev_fr_ptr->fr_rate,0,0);
              break;
          }
        }

        else
        {
          ERR("Bad Serv Cat %d",rev_fr_block_ptr->rev_arr->srvc_type,0,0);
        }
        rlp_frame_log_ptr->time_rate.phy_chan_rate = chan_rate;
        rlp_frame_log_ptr->frame_len = ((rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK)?
                                         0:rlp_to_mux_table[rev_fr_ptr->fr_rate]
                                         [rev_fr_block_ptr->rev_arr->srvc_type]
                                         [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, 
                                                           DSRLP_MUX_PDU_3)]);

        rlp_frame_log_ptr->payload_len.head = ((rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK)?
                                                0:(rlp_frame_log_ptr->frame_len + 7) / 8);
      }


      rlp_frame_log_ptr->payload_len.tail = 0; /*           No tail!     */

      if ( (i < rev_fr_block_ptr->num_frames) &&
           (rev_fr_ptr->fr_item_ptr != NULL) )
      {
        dsrlp_log_tx_rlp_frm_buf.hdr.len
              += rlp_frame_log_ptr->payload_len.head;

        /* Copy frame data as head payload. */
        memcpy((void*) rlp_frame_log_ptr->payload,
               rev_fr_ptr->fr_item_ptr->data_ptr,
               rlp_frame_log_ptr->payload_len.head);
      }
      else
      {
        rlp_frame_log_ptr->frame_len = 0;
        rlp_frame_log_ptr->payload_len.head = 0;
        rlp_frame_log_ptr->payload_len.tail = 0;
      }
    }
  } /* for */
} /* txc_log_fdch_rlp3() */

#endif /* FEATURE_RLP_LOGGING */

#ifdef FEATURE_DS_RLP3
/*===========================================================================
FUNCTION     TXC_TX_FRAME_SEND_DATA3G

DESCRIPTION  This function sends the RLP 3 frames out the SCH.

DEPENDENCIES Assumes some parameters stuff is done beforehand.  Also assumes
             the SCH is correctly config'd.  Don't call this if doing DTX.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_frame_proc_send_data3g
(
  dsrlp_rev_frame_block_type *rev_fr_block_ptr   /* RLP frames to be tx'ed */
)
{
  dsrlp_rev_frame_type *rev_fr_ptr;               /* Ptr to 1 RLP tx frame */
  byte i;                                         /* loop ctr              */

#ifdef FEATURE_IS2000_R_SCH
  byte num_sch_spare_bits;
#endif /* FEATURE_IS2000_R_SCH */

#if( defined( FEATURE_IS2000_R_SCH ) )
    word r_sch_blank_buffer_size;            /* num blank bytes out   */
#endif // FEATURE_IS2000_R_SCH
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_IS2000_R_SCH
  if ((txtc.sch_on == TRUE) && (rev_fr_block_ptr->num_frames > SCH_BUF_START))
  {
#ifdef FEATURE_IS2000_REL_A
    if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
    {
      switch (txtc.supp_rate)
      {
        case ENC_SCH_8TH_RATE:
        case ENC_SCH_QTR_RATE:
        case ENC_SCH_HALF_RATE:
          break;
        case ENC_SCH_FULL_RATE:
          (void) parm_inc(CAI_RSCH0_1_ID, 1);
          break;
        case ENC_SCH_2X_RATE:
          (void) parm_inc(CAI_RSCH0_2_ID, 1);
          break;
        case ENC_SCH_4X_RATE:
          (void) parm_inc(CAI_RSCH0_3_ID, 1);
          break;
        case ENC_SCH_8X_RATE:
          (void) parm_inc(CAI_RSCH0_4_ID, 1);
          break;
        case ENC_SCH_16X_RATE:
          (void) parm_inc(CAI_RSCH0_5_ID, 1);
          break;
        case ENC_SCH_32X_RATE:
          (void) parm_inc(CAI_RSCH0_6_ID, 1);
          break;
        default:
          ERR_FATAL("Bad rate %d",txtc.supp_rate,0,0);
          break;
      }
    }
    else
#endif /* FEATURE_IS2000_REL_A */
    {
      switch (txtc.supp_rate)
      {
        case ENC_SCH_8TH_RATE:
        case ENC_SCH_QTR_RATE:
        case ENC_SCH_HALF_RATE:
          break;
        case ENC_SCH_FULL_RATE:
          (void) parm_inc(CAI_SCH0_REV_1X_ID, 1);
          break;
        case ENC_SCH_2X_RATE:
          (void) parm_inc(CAI_SCH0_REV_2X_ID, 1);
          break;
        case ENC_SCH_4X_RATE:
          (void) parm_inc(CAI_SCH0_REV_4X_ID, 1);
          break;
        case ENC_SCH_8X_RATE:
          (void) parm_inc(CAI_SCH0_REV_8X_ID, 1);
          break;
        case ENC_SCH_16X_RATE:
          (void) parm_inc(CAI_SCH0_REV_16X_ID, 1);
          break;
#ifdef FEATURE_IS2000_REL_A
        case ENC_SCH_32X_RATE:
          (void) parm_inc(CAI_SCH0_REV_32X_ID, 1);
          break;
#endif /* FEATURE_IS2000_REL_A */
        default:
          ERR_FATAL("Bad rate %d",txtc.supp_rate,0,0);
          break;
      }
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  /*-----------------------------------------------------------------------
    Go ahead and log the data before sending it out.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_RLP_LOGGING
  if (log_status(LOG_RLP_TX_FRAMES_C))
  {
    dsrlp_log_tx_rlp_frm_buf.hdr.code = LOG_RLP_TX_FRAMES_C;
    txc_log_rlp3(rev_fr_block_ptr);
  }
  else if(log_status(LOG_RLP_TX_FDCH_FRAMES_C))
  {
    dsrlp_log_tx_rlp_frm_buf.hdr.code = LOG_RLP_TX_FDCH_FRAMES_C;
    txc_log_fdch_rlp3(rev_fr_block_ptr);
  }

#endif /* FEATURE_RLP_LOGGING */


#ifdef FEATURE_IS2000_R_SCH
  /*-----------------------------------------------------------------------
    Only increment when not turbo and > 2x, as those use LTU's.  Also
    account for double/single-sized PDU's.
  -----------------------------------------------------------------------*/
  if ((!txtc.supp_turbo) &&
      (txtc.supp_rate > ENC_SCH_2X_RATE) &&
      (rev_fr_block_ptr->num_frames > SCH_BUF_START)
     )
  {
#ifdef FEATURE_IS2000_REL_A
    if(txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
    {
      (void) parm_inc(CAI_RSCH0_LTU_ID, txtc.pot_num_mux_pdus);
    }
    else
#endif /* FEATURE_IS2000_REL_A */
    {
      if ( txtc.double_sized == TRUE)
      {
        (void) parm_inc(CAI_SCH0_REV_LTU_ID, txtc.pot_num_mux_pdus);
      }
      else
      {
        (void) parm_inc(CAI_SCH0_REV_LTU_ID, txtc.pot_num_mux_pdus/2);
      }
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  for( i=0; i < rev_fr_block_ptr->num_frames; i++)
  {
    rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[i]);

#ifdef FEATURE_IS2000_R_SCH
    /* Assuming 1 MuxPDU fits completely in a dsm item */
    if( i >= SCH_BUF_START)
    {
      if ((txtc.supp_rate == ENC_SCH_FULL_RATE)
#ifdef FEATURE_IS2000_REL_A
        && (txtc.mux_pdu_type != CAI_MUX_PDU_TYPE_5 )
#endif /* FEATURE_IS2000_REL_A */
         )
      {
          /*
             for 1x, need to have Mixed mode bit pre-appended
             Note that only for 1x rates we have to test for Primary and
             Secondary rates on SCH.
          */
#ifdef FEATURE_DS_IS2000_RS2
          if (txtc.r_sch_rc == CAI_RC_4)
          {
#ifdef FEATURE_IS2000_REL_A
            if (CAI_SO_PPP_PKT_DATA_3G == txc_so.pri.so)
#else
            if (CAI_SO_PPP_PKT_DATA_3G == txc_so.pri)
#endif // FEATURE_IS2000_REL_A
            {
                txtc.supp_frame.data[0] = 0x00;
                b_copy( rev_fr_ptr->fr_item_ptr->data_ptr, 0,
                        txtc.supp_frame.data, 2,
                        MUX2_FULL_PRI_LEN );
            }
#ifdef FEATURE_IS2000_REL_A
            else if (CAI_SO_PPP_PKT_DATA_3G == txc_so.sec.so)
#else
            else if (CAI_SO_PPP_PKT_DATA_3G == txc_so.sec)
#endif // FEATURE_IS2000_REL_A
            {
                /*-----------------------------------------------------------
                  For MuxPDU2 we have to add header in first 6 bits. The
                  header is comprised of:
                  EIB  |  Mixed Mode bit | Frame Mode
                   0         1                 0111
                 Hence make the first 6 bits as 0101 11xx i.e. 0x5C

                 Copy the next 262 bits of data into the transmit array
                 after 6 bits of header.

                 Since, "used field" is used later, we need to increment it
                 by 1 because adding 6 bits crosses the byte boundry.
                 Note that incrementing used field will not cross beyond the
                 DSM boundries, because max is only 33 bytes.
                -----------------------------------------------------------*/
                txtc.supp_frame.data[0] = 0x5C;
                b_copy( rev_fr_ptr->fr_item_ptr->data_ptr, 0,
                        txtc.supp_frame.data, 6,
                        MUX2_FULL_SEC_LEN );
                rev_fr_ptr->fr_item_ptr->used+=1;
            }
            else
            {
#ifdef FEATURE_IS2000_REL_A
                MSG_ERROR("Bad Srv typ for data %d",txc_so.sec.so,0,0);
#else
                MSG_ERROR("Bad Srv typ for data %d",txc_so.sec,0,0);
#endif // FEATURE_IS2000_REL_A

            }
          }
          else
#endif /* FEATURE_DS_IS2000_RS2 */
              {
#ifdef FEATURE_IS2000_REL_A
                  if (CAI_SO_PPP_PKT_DATA_3G == txc_so.pri.so)
#else
                  if (CAI_SO_PPP_PKT_DATA_3G == txc_so.pri)
#endif // FEATURE_IS2000_REL_A
                  {
                      txtc.supp_frame.data[0] = 0x00;
                      b_copy( rev_fr_ptr->fr_item_ptr->data_ptr, 0,
                              txtc.supp_frame.data, 1,
                              MUX1_FULL_PRI_LEN );
                  }
#ifdef FEATURE_IS2000_REL_A
                  else if (CAI_SO_PPP_PKT_DATA_3G == txc_so.sec.so)
#else
                  else if (CAI_SO_PPP_PKT_DATA_3G == txc_so.sec)
#endif // FEATURE_IS2000_REL_A
                  {
                      /*-----------------------------------------------------------
                        For MuxPDU1 we have to add header in first 4 bits. The
                        header is comprised of:
                         Mixed Mode bit | Traffic Type | Traffic Mode
                         1                 1             11
                       Hence make the first 4 bits as 0xF0

                       Copy the next 168 bits of data into the transmit array.

                       Since, used field is used later, we need to increment it
                       by 1 because adding 4 bits crosses the byte boundry.
                      -----------------------------------------------------------*/
                      txtc.supp_frame.data[0] = 0xF0;
                      b_copy( rev_fr_ptr->fr_item_ptr->data_ptr, 0,
                              txtc.supp_frame.data, 4,
                              MUX1_FULL_SEC_LEN);
                      rev_fr_ptr->fr_item_ptr->used+=1;
                  }
                  else
                  {
#ifdef FEATURE_IS2000_REL_A
                      MSG_ERROR("Bad Srv typ for data %d",txc_so.sec.so,0,0);
#else
                      MSG_ERROR("Bad Srv typ for data %d",txc_so.sec,0,0);
#endif // FEATURE_IS2000_REL_A
                  }
              }
        ENC_SCH_DATA_OUT
        (
           txtc.supp_frame.data,
           rev_fr_ptr->fr_item_ptr->used,
           txtc.double_sized
        );
      }
      else
      {
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
        {

          if (
              (rev_fr_ptr->fr_item_ptr != NULL) &&
              (rev_fr_ptr->fr_item_ptr->data_ptr != NULL)
             )
          {
            /* if the used field is equal the max_frame_size field + 1, then
                there is only one DSM item per PDU. If there are diferent, then we
                know that there are multiple DSM items chained together*/
            if (rev_fr_ptr->fr_item_ptr->used == (rev_fr_ptr->max_frame_size + 1))
            {
                enc_sch_muxpdu5_data_out(rev_fr_ptr->fr_item_ptr->data_ptr,
                                         rev_fr_ptr->fr_item_ptr->used);
            }
            else  /* there could be more than 1 DSM item per DPU */
            {
              dsm_item_type *temp_current_dsm_prt;
              dsm_item_type *temp_next_dsm_prt;
              temp_current_dsm_prt = rev_fr_ptr->fr_item_ptr;

              /*--------------------------------------------------------------------------
               Add changes for adding E0 at end of the data
               Obtain Length of the RLP frame using Length Indicator.
               If (Length Indicator == 00)
                  No need to do anything.
               else if (length indicator is 01 or 10)
                  Add E0 at the end
               else
                  Should never happen. Print message and fail an ASSERT for now

               {
                   Add E0 at the end of the packet.
                   If packet does not have any more items, then grab a free SMALL DSM pkt.
                   If no free DSM packet exists, its okay, send something(its usually 0s)
               }
               Code in else path, can actually be done by existing dsm API, pushdown_tail;
               Made the length 1, though its a 4 byte word, because we are interested only
               in E0.Probably not too elegant, but saves extra definitions. If anytime in
               future you change the endianness this may be another piece to be visited
               The total data that can be sent is based on (rev_fr_ptr->max_frame_size+1).
               We add 1, because that's the header for SR_ID.
              --------------------------------------------------------------------------*/

              if (txtc.supp_turbo)
              {
                /* for turbo, there is one PDU per frame, thus use this special
                logic to insert the NULL PDU header after the last used data byte*/
                dsrlp_insert_pdu5null_hdr_turbo(temp_current_dsm_prt);
              }
              temp_next_dsm_prt = dsrlp_get_next_dsm_item_chain(temp_current_dsm_prt);
              while (temp_next_dsm_prt != NULL)
              {
                /* write the current dsm data into the encoder */
                enc_sch_muxpdu5_data_out
                (
                  temp_current_dsm_prt->data_ptr,
                  temp_current_dsm_prt->used
                );

                /* update the current pointer to the next */
                temp_current_dsm_prt = temp_next_dsm_prt;
                /* get the next pointer */
                temp_next_dsm_prt = dsrlp_get_next_dsm_item_chain(temp_current_dsm_prt);
              }

              /* the last dsm item will be write into the encoder by the following logic */
              /* initialize the data pointer and the number of byte */
              if (temp_current_dsm_prt->data_ptr != NULL)
              {
                enc_sch_muxpdu5_data_out
                (
                  temp_current_dsm_prt->data_ptr,
                  temp_current_dsm_prt->used
                );
              }
              else
              {
                ASSERT (temp_current_dsm_prt->data_ptr != NULL);
              }
            }
          }
          else
          {
            ASSERT (rev_fr_ptr->fr_item_ptr != NULL);
            ASSERT (rev_fr_ptr->fr_item_ptr->data_ptr != NULL);
          }
        }
        else
#endif /* FEATURE_IS2000_REL_A */
        {
          if (
              (rev_fr_ptr->fr_item_ptr != NULL) &&
              (rev_fr_ptr->fr_item_ptr->data_ptr != NULL)
             )
          {
            ENC_SCH_DATA_OUT
            (
              rev_fr_ptr->fr_item_ptr->data_ptr,
              rev_fr_ptr->fr_item_ptr->used,
              txtc.double_sized
            );
          }
          else
          {
            ASSERT (rev_fr_ptr->fr_item_ptr != NULL);
            ASSERT (rev_fr_ptr->fr_item_ptr->data_ptr != NULL);
          }
        }
      }
    }
#endif /* FEATURE_IS2000_R_SCH */
    if( rev_fr_ptr->free_dsm_pkt == TRUE)
    {
      /* this will free all the DSM items in the chain */
      dsm_free_packet( &(rev_fr_ptr->fr_item_ptr));
    }
  } /* for loop */

#ifdef FEATURE_IS2000_R_SCH
 /* If there is room left, fill the R-SCH with blank PDUs */
  if( (txtc.sch_on == TRUE) &&
      (rev_fr_block_ptr->num_frames > SCH_BUF_START) &&
      (rev_fr_block_ptr->num_frames < txtc.pot_num_mux_pdus+SCH_BUF_START) )
  {
    /*---------------------------------------------------------------------
      Note that the size is decremented by one, since the one-byte header
      is written separately.
    ---------------------------------------------------------------------*/
    if((ENC_RC_TO_RATE_SET(txtc.r_sch_rc)) == ENC_RATE_14400)
    {
#ifdef FEATURE_IS2000_REL_A
      if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
      {
        r_sch_blank_buffer_size = txc_muxpdu5_rs2_maxsize[txtc.supp_rate ][txtc.supp_turbo];
      }
      else
      {
#endif /* FEATURE_IS2000_REL_A */
        r_sch_blank_buffer_size =( (txtc.double_sized) ? (68 - 1) : (34 - 1));
#ifdef FEATURE_IS2000_REL_A
      }
#endif /* FEATURE_IS2000_REL_A */
    }
    else if ((ENC_RC_TO_RATE_SET(txtc.r_sch_rc)) == ENC_RATE_9600)
    {
#ifdef FEATURE_IS2000_REL_A
      if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
      {
        r_sch_blank_buffer_size = txc_muxpdu5_rs1_maxsize[txtc.supp_rate ][txtc.supp_turbo];
      }
      else
      {
#endif /* FEATURE_IS2000_REL_A */
        r_sch_blank_buffer_size =( (txtc.double_sized) ? (44 - 1) : (22 - 1));
#ifdef FEATURE_IS2000_REL_A
      }
#endif /* FEATURE_IS2000_REL_A */
    }
    else
    {
      /* Default to Rate Set 1 blank buffer size */
      r_sch_blank_buffer_size =( (txtc.double_sized) ? (44 - 1) : (22 - 1));

      ERR( "Bad Rate Set: Fix it", 0, 0, 0 );
      // ASSERT(0); Use for debug
    }

    /*---------------------------------------------------------------------
      The first blank PDU has the 0x0E header.  Later PDU's may or may not
      have this header, depending on if Turbo or convolutional encoding
    ---------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A
    /* for MUXPDU 5, as for the current implementation turbo will have only 1 MUX PDU,
     * thus RLP will take care all of the NULL filling. For PDU5 and turbo code,
     * the following logic should not run at all. In the future, if there are more than
     * one PDU5 per frame for turbo, then this logic is needed.
     */
    if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
    {
      /* write the NULL PDU header first */
      enc_sch_muxpdu5_data_out((void *)r_sch_blank_pdu_hdr,4);
      enc_sch_muxpdu5_data_out((void *)r_sch_blank_buffer,r_sch_blank_buffer_size - 3);
    }
    else
#endif /* FEATURE_IS2000_REL_A */
    {
      ENC_SCH_DATA_OUT((void *)(r_sch_blank_pdu_hdr),4, txtc.double_sized);
      ENC_SCH_DATA_OUT
      (
        (void *)r_sch_blank_buffer,
        (word)(r_sch_blank_buffer_size - 3),
        txtc.double_sized
      );
    }
    /*---------------------------------------------------------------------
      Have already written one blank mux pdu.  So make sure don't write
      too many more (notice "i = 1" in for... conditional).  In the for...
      loop, write the one-byte header, and then zero out rest of pdu.
    ---------------------------------------------------------------------*/
    for (i = 1;
         i < (txtc.pot_num_mux_pdus + SCH_BUF_START - rev_fr_block_ptr->num_frames);
         i++)
    {
      if (txtc.supp_turbo == TRUE)
      {
        /*-----------------------------------------------------------------
          Only the first PDU has a 0x0E header.  Rest are all zero's
          For this current implementation, this logic only apply for PDU3
        -----------------------------------------------------------------*/
        ENC_SCH_DATA_OUT((void *)r_sch_blank_buffer,4, txtc.double_sized);
      }
      else
      {
        /*-----------------------------------------------------------------
          Every PDU has the 0x0E header.
        -----------------------------------------------------------------*/
          ENC_SCH_DATA_OUT((void *)(r_sch_blank_pdu_hdr),4, txtc.double_sized);
      }
#ifdef FEATURE_IS2000_REL_A
      if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
      {
        enc_sch_muxpdu5_data_out((void *)r_sch_blank_buffer,r_sch_blank_buffer_size - 3);
      }
      else
#endif /* FEATURE_IS2000_REL_A */
      {
        ENC_SCH_DATA_OUT
        (
          (void *)r_sch_blank_buffer,
          (word) (r_sch_blank_buffer_size -3),
          txtc.double_sized
        );
      }
    } /* for */
    (void) parm_xtra_inc(CAI_XTRA_SCH0_REV_BLANK_PDU_ID,
                  (txtc.pot_num_mux_pdus + SCH_BUF_START - rev_fr_block_ptr->num_frames)
                 );
  } /* if blank frames need to be added */

  /*-----------------------------------------------------------------------
    The IS2000 standard specifies that the "spare bits", remaining after
    the maximum number of PDUs for the given rate have been used up to form
    the SDU, shall be filled with 0's. Since turbo coded frames do not have
    LTU CRCs, this number is a function of the supplemental channel data
    rate and the code type. Convolutional = 0, Turbo = 1.
  -----------------------------------------------------------------------*/
  if ((txtc.sch_on == TRUE) && (rev_fr_block_ptr->num_frames > SCH_BUF_START))
  {
#ifdef FEATURE_IS2000_REL_A
    if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
    {
      num_sch_spare_bits = (txc_muxpdu5_nsch_sbits[ txtc.supp_rate ][ txtc.supp_turbo ])/8;
    }
    else
#endif /* FEATURE_IS2000_REL_A */
    {
      num_sch_spare_bits = (txc_num_sch_spare_bits[ txtc.supp_rate ][ txtc.supp_turbo ])/8;
    }

    ENC_SCH_DATA_OUT
    (
       txc_sch_spare_bits,
       num_sch_spare_bits,
       txtc.double_sized
    );
#ifdef FEATURE_IS2000_REL_A
    if( (!txtc.double_sized) && ( txtc.mux_pdu_type != CAI_MUX_PDU_TYPE_5))
#else
    if( !txtc.double_sized )
#endif /* FEATURE_IS2000_REL_A */
    {
      ENC_SCH_DATA_OUT
      (
        txc_sch_spare_bits,
        2,
        FALSE
      );
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  rev_fr_block_ptr->num_frames = 0;
  enc_write_data_done();
} /* txc_tx_frame_proc_send_data3g() */
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_DS_RLP3
#ifdef FEATURE_IS2000_REL_A_SVD

/*===========================================================================
FUNCTION     TXC_TX_FRAME_PROC_BUILD_DATA3G

DESCRIPTION  This function builds the RLP 3 frames.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_frame_proc_build_data3g
(
  dsrlp_rev_frame_block_type *rev_fr_block_ptr   /* RLP frames to be tx'ed */
)
{
  rev_fr_block_ptr->num_frames = 0;
  /*-------------------------------------------------------------------------
   filling in frame information for frame building. (FCH/DCCH).
   Ask RLP to build frames to transmit using the new RLP interfaces.
  -------------------------------------------------------------------------*/
  rev_fr_block_ptr->sr_id = DSRLP_FIXED_SR_ID_INDEX;
  rev_fr_block_ptr->max_frames = SCH_BUF_START;

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
     See if throttling is required on R-SCH
  -------------------------------------------------------------------------*/
  if (txc_sch_changed > 0)
  {
    txc_sch_changed--;
    if (txc_sch_changed == 0)
    {
      txc_calc_r_sch_throttle();
    }
  }

#ifdef FEATURE_IS2000_REL_A
  /* for MUXPDU5, for simple the implementation, in the case turbo, we use only 1 MUXPDU */
  if ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5) && (txtc.supp_turbo))
  {
    if (txtc.ok_num_mux_pdus > 0)
    {
      txtc.ok_num_mux_pdus = 1;
      txtc.pot_num_mux_pdus = 1;
    }
  }
#endif /* FEATURE_IS2000_REL_A */
  /*-------------------------------------------------------------------------
    DTX expired, should not send data on SCH
  -------------------------------------------------------------------------*/
  if (txtc.txc_dtx_off)
  {
    txtc.txc_dtx_off = FALSE;
    if (txtc.sch_on)
    {
      /* turns off sch_on to prevent the data preparation on SCH */
      txtc.sch_on = FALSE;

#ifdef FEATURE_TX_POWER_BACK_OFF
      /* Channel configuration has changed, we need to run the RF PA backoff algorithm
      to back off the power if necessary */
      txc_update_chan_config_to_rf(TRUE);
#endif /* FEATURE_TX_POWER_BACK_OFF */

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
      rfm_notify_sch_data_rate(
          RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
      rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  /*-------------------------------------------------------------------------
    FCH, DCCH frame information already processed.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_IS2000_R_SCH
  /* do max power protection
  */
  txc_max_pwr_prot();

  /*-------------------------------------------------------------------------
    filling in frame information for frame building. (SCH)
  -------------------------------------------------------------------------*/

  txc_tx_process_sch (rev_fr_block_ptr);
#endif /* FEATURE_IS2000_R_SCH */

  dsrlp_build_tx_frames(rev_fr_block_ptr);

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
     DTX duration checking is activated
  -------------------------------------------------------------------------*/
  if (txtc.dtx_duration_included )
  {
    if ((txtc.pot_num_mux_pdus > 0) &&
        (rev_fr_block_ptr->num_frames <= SCH_BUF_START))
    {
      /*---------------------------------------------------------------------
        all SCH RLP frames are empty in the data block
      ---------------------------------------------------------------------*/
      if (txtc.dtx_duration > 0)
      {
        txtc.dtx_duration --;
      }
      else
      {
        /*-------------------------------------------------------------------
          DTX expired, release SCH by sending SCRM to BS
          and disable the DTX duration checking.
        -------------------------------------------------------------------*/
        MSG_HIGH("DTX exp, releasing SCH", 0, 0, 0);
        mcc_set_rsch_dtx_sig();   /* Ask MC to release the SCH */
        txtc.dtx_duration_included = FALSE;
        txtc.dtx_duration_org = 0;
        txtc.txc_dtx_off = TRUE;
      }
    }
    else if (rev_fr_block_ptr->num_frames > SCH_BUF_START)
    {
      /*---------------------------------------------------------------------
        The data TX resumed before the DTX duration expired.
        Reset dtx_duration to the dtx_duration_org
      ---------------------------------------------------------------------*/
      txtc.dtx_duration = txtc.dtx_duration_org;
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

/**********************************************************************************************/
 /* Post Processing */
/**********************************************************************************************/

  /* process FCH
  */
  txc_tx_post_rlp_fch_process (rev_fr_block_ptr);

#ifdef FEATURE_IS2000_P2
  /* process DCCH
  */
  txc_tx_post_rlp_dcch_process (rev_fr_block_ptr);
#endif /* FEATURE_IS2000_P2 */

} /* txc_tx_frame_proc_build_data3g() */

#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION     TXC_MAX_PWR_PROT

DESCRIPTION  This function does processing for Maximum Power Protection.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_max_pwr_prot(void)
{
#ifdef FEATURE_MAX_PWR_PROTECTION
   int16  gain_req_full_rate;
#endif // FEATURE_MAX_PWR_PROTECTION

  txc_max_pwr_limited_dtx_sch = FALSE;

  if (!txtc.sch_on || !txc_sch_max_pwr_protection_enabled)
  {
    return;
  }

#ifdef FEATURE_IS2000_P2
  if (txc_so.rev_fch.included && txc_so.rev_dcch.included)
  {
    gain_req_full_rate = -txc_fch_dcch_sch_gain_tab [ENC_FULL_RATE]
                                         [DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain;
    if (tx_extra_gain <= gain_req_full_rate)
    {
      txc_max_pwr_limited_dtx_sch = TRUE;
      gain_req_full_rate = -txc_fch_dcch_sch_gain_tab [ENC_FULL_RATE]
                                         [DCCH_ACTIVE_SCH_DTX].pch_adj_gain;
    }
  }
  else if (txc_so.rev_dcch.included)
  {
    gain_req_full_rate =
          -txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain;

    if (tx_extra_gain <= gain_req_full_rate)
    {
      txc_max_pwr_limited_dtx_sch = TRUE;
    }
  }
  else if (txc_so.rev_fch.included)
#endif  /* FEATURE_IS2000_P2 */
  {
    gain_req_full_rate = -txc_sch_pch_adj_gain_tab [ENC_FULL_RATE];
    if (tx_extra_gain <= gain_req_full_rate)
    {
      txc_max_pwr_limited_dtx_sch = TRUE;
      gain_req_full_rate = -txc_dtx_pch_adj_gain_tab [ENC_FULL_RATE];
    }
  }
} /* txc_max_pwr_prot */
#endif // FEATURE_IS2000_R_SCH
#endif // FEATURE_MAX_PWR_PROTECTION

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION     TXC_TX_PROCCESS_SCH

DESCRIPTION  This function sets up RLP information needed to build a R-SCH frame.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_process_sch
(
 dsrlp_rev_frame_block_type *rev_fr_block_ptr
)
{
  dsrlp_rev_frame_type *rev_fr_ptr;             /* Ptr to 1 RLP tx frame */
  int i;

  enc_rate_set_type sch_rate_set = ENC_RATE_9600;

  if (txtc.sch_on
#ifdef FEATURE_MAX_PWR_PROTECTION
      &&
      (!txc_max_pwr_limited_dtx_sch)
#endif // FEATURE_MAX_PWR_PROTECTION
  )
  {
    rev_fr_block_ptr->max_frames += txtc.ok_num_mux_pdus;
    /*---------------------------------------------------------------------
      Time to find out SCH rate sets
    ---------------------------------------------------------------------*/
    sch_rate_set = ENC_RC_TO_RATE_SET(txtc.r_sch_rc);

    for( i = 0; i < txtc.pot_num_mux_pdus; i++)
    {
        /*-------------------------------------------------------------------
        Loop through all potential frames, as this will make logging
        easier.  But only pass up to RLP the "ok" number of frames.
      -------------------------------------------------------------------*/
      rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[i+SCH_BUF_START]);

      rev_fr_ptr->fr_rate_mask = DSRLP_RATE_1_MASK;

      if (sch_rate_set == ENC_RATE_9600)
      {
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
        {
          rev_fr_ptr->max_frame_size  = txc_muxpdu5_rs1_maxsize[txtc.supp_rate ][txtc.supp_turbo];
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */

          rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_1;
          rev_fr_ptr->max_frame_size  = ( (txtc.double_sized) ? 43 : 21);

          /* Added check for MUX PDU 1 and 2 1x SCH so RLP will put RLP frame type bit
          ** in correct position, not applicable to MUX PDU 5.
          */
          if (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G)
          {
            rev_fr_ptr->srvc_type = DSRLP_SECONDARY_SRVC;
          }
          else
          {
            rev_fr_ptr->srvc_type = DSRLP_PRIMARY_SRVC;
          }
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* #ifdef FEATURE_IS2000_REL_A */

      }
      else if(sch_rate_set == ENC_RATE_14400)
      {
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
        {
          rev_fr_ptr->max_frame_size  = txc_muxpdu5_rs2_maxsize[txtc.supp_rate ][txtc.supp_turbo];
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */

          rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_2;
          rev_fr_ptr->max_frame_size  = ( (txtc.double_sized) ? 67 : 33);

          /* Added check for MUX PDU 1 and 2 1x SCH so RLP will put RLP frame type bit
          ** in correct position, not applicable to MUX PDU 5.
          */
          if (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G)
          {
            rev_fr_ptr->srvc_type = DSRLP_SECONDARY_SRVC;
          }
          else
          {
            rev_fr_ptr->srvc_type = DSRLP_PRIMARY_SRVC;
          }
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* #ifdef FEATURE_IS2000_REL_A */

      }
      else
      {
         ERR("Got Bad Rate Set info",0,0,0);
          // ASSERT(0); Use only for debugging
      }

       /*
       Sarithay : Do the check for 1x . Talk to Nagesh/Harsha about the functionality.
       */
#ifdef FEATURE_IS2000_REL_A
      if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
      {
        rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_5;
      }
      else
      {
#endif /* FEATURE_IS2000_REL_A */
        if (txtc.supp_rate == ENC_SCH_FULL_RATE)
        {
           rev_fr_ptr->max_frame_size  =
              dsrlp_len_table[DSRLP_RATE_1]
                             [rev_fr_ptr->srvc_type]
                             [rev_fr_ptr->mux_pdu_type];
        }
        else
        {
          rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_3;

          //rev_fr_ptr->srvc_type    = DSRLP_GENERIC_SRVC;

        }
#ifdef FEATURE_IS2000_REL_A
      }
#endif /* FEATURE_IS2000_REL_A */
      if (i < txtc.ok_num_mux_pdus)
      {
        rev_fr_ptr->fr_cat_mask  = DSRLP_CAT_DATA_MASK;
      }
      else
      {
       /*-----------------------------------------------------------------
         Reverse throttling taking place... don't build these
         frames.
       -----------------------------------------------------------------*/
       rev_fr_ptr->fr_cat_mask = DSRLP_CAT_BLANK_MASK;
      }
    } /* for */
  } /* sch is on */
}
#endif /*FEATURE_IS2000_R_SCH*/

#else

/*===========================================================================
FUNCTION     TXC_TX_FRAME_PROC_BUILD_DATA3G

DESCRIPTION  This function builds the RLP 3 frames.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_frame_proc_build_data3g
(
  dsrlp_rev_frame_block_type *txc_rev_fr_block   /* RLP frames to be tx'ed */
)
{
  dsrlp_rev_frame_type *rev_fr_ptr;             /* Ptr to 1 RLP tx frame */
#if defined(FEATURE_IS2000_R_SCH)
  byte i;                                       /* loop ctr              */
#endif

#if defined(FEATURE_MAX_PWR_PROTECTION) && defined(FEATURE_IS2000_R_SCH)
  int16  gain_req_full_rate;

#ifdef FEATURE_MAX_PWR_PROTECTION_RLP
#error code not present
#endif // FEATURE_MAX_PWR_PROTECTION_RLP
#endif // FEATURE_MAX_PWR_PROTECTION && FEATURE_IS2000_R_SCH

#ifdef FEATURE_IS2000_P2
  // Remember if we need to send msg on DCCH
  boolean msg_on_dcch = txc_so.rev_dcch.included && txc_tx.active_msg &&
                        (txtc.cur_dsch == TXC_REV_DCCH);
  boolean msg_on_fch = txc_so.rev_fch.included && txc_tx.active_msg &&
                        (txtc.cur_dsch == TXC_REV_FCH);
  #ifdef FEATURE_IS2000_CHS
  boolean control_hold = txtc.gating_rate != CAI_GATING_RATE_NONE;
  #endif // FEATURE_IS2000_CHS
#endif // FEATURE_IS2000_P2

  /*
     For generality program mux_pdu to be PDU1. Note that we use only one
     variable for building mux_pdu for fch/dcch/sch, since they are used
     at mutually-xclusive times.
  */
  dsrlp_mux_pdu_enum_type mux_pdu = DSRLP_MUX_PDU_1;

  /*
   The rate sets for different channels. Note that, we need separate variables
   for each channel because, we support Mixed rate sets.
   Initialize them to RS1 as these are default values.
  */
  enc_rate_set_type fundich_rate_set = ENC_RATE_9600;
#ifdef FEATURE_IS2000_SCH
  enc_rate_set_type sch_rate_set = ENC_RATE_9600;
#endif /*FEATURE_IS2000_SCH*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  txc_rev_fr_block->num_frames = 0;

  /*-------------------------------------------------------------------------
    Read the rate sets of all the channels. We use them only if the
    channels are in use.
    if FCH is included then fundicated channel rate set is that of FCH
    else, it is for DCCH.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_P2
  if (txc_so.rev_fch.included)
  {
#endif
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc);
#ifdef FEATURE_IS2000_P2
  }
  else
  {
    fundich_rate_set = ENC_RC_TO_RATE_SET(txc_so.rev_dcch.rc);
  }
#endif

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
     See if throttling is required on R-SCH
  -------------------------------------------------------------------------*/
  if (txc_sch_changed > 0)
  {
    txc_sch_changed--;
    if (txc_sch_changed == 0)
    {
      txc_calc_r_sch_throttle();
    }
  }
#ifdef FEATURE_IS2000_REL_A
  /* for MUXPDU5, for simple the implementation, in the case turbo, we use only 1 MUXPDU */
  if ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5) && (txtc.supp_turbo))
  {
    if (txtc.ok_num_mux_pdus > 0)
    {
      txtc.ok_num_mux_pdus = 1;
      txtc.pot_num_mux_pdus = 1;
    }
  }
#endif /* FEATURE_IS2000_REL_A */
  /*-------------------------------------------------------------------------
    DTX expired, should not send data on SCH
  -------------------------------------------------------------------------*/
  if (txtc.txc_dtx_off)
  {
    txtc.txc_dtx_off = FALSE;
    if (txtc.sch_on)
    {
      /* turns off sch_on to prevent the data preparation on SCH */
      txtc.sch_on = FALSE;

#ifdef FEATURE_TX_POWER_BACK_OFF
      /* Channel configuration has changed, we need to run the RF PA backoff algorithm
      to back off the power if necessary */
      txc_update_chan_config_to_rf(TRUE);
#endif /* FEATURE_TX_POWER_BACK_OFF */

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
      rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
      rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  /*-------------------------------------------------------------------------
   filling in frame information for frame building. (FCH/DCCH).
   Ask RLP to build frames to transmit using the new RLP interfaces.
  -------------------------------------------------------------------------*/
  /* NOTE: Although the variable below is named txc_rev_fr_blk->sr_id, it is
  actually the RLP instance (or index in rscb_array) for this service.
  Currently, only one RLP instance is supported. In the future, when multiple
  RLP instances are supported, change this to use the correct one*/
  txc_rev_fr_block->sr_id = DSRLP_FIXED_SR_ID_INDEX;
  txc_rev_fr_block->max_frames = SCH_BUF_START;
  rev_fr_ptr = &(txc_rev_fr_block->rev_arr[FCH_BUF]);

  /*-------------------------------------------------------------------------
    Now read the type of mux PDU to be used for the FCH/DCCH.
  -------------------------------------------------------------------------*/
  if(fundich_rate_set == ENC_RATE_9600)
  {
    mux_pdu = DSRLP_MUX_PDU_1;
    MSG_LOW("Making RS1 FCH Fr",0,0,0);
  }
  else if (fundich_rate_set == ENC_RATE_14400)
  {
    mux_pdu = DSRLP_MUX_PDU_2;
    MSG_LOW("Making RS2 FCH Fr",0,0,0);
  }
  else
  {
    ERR("Bad Rate set: Fix it",0,0,0);
    //ASSERT(0); Use for debugging
  }


  #ifdef FEATURE_IS2000_P2
  if ( txc_so.rev_fch.included )
  #endif // FEATURE_IS2000_P2
  {
#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH
      txc_max_pwr_limited_dtx_sch = FALSE;

    if (txtc.sch_on && txc_sch_max_pwr_protection_enabled)
    {
          gain_req_full_rate = -txc_sch_pch_adj_gain_tab [ENC_FULL_RATE];
          if (tx_extra_gain <= gain_req_full_rate)
          {
            txc_max_pwr_limited_dtx_sch = TRUE;
            gain_req_full_rate = -txc_dtx_pch_adj_gain_tab [ENC_FULL_RATE];
          }

#ifdef FEATURE_MAX_PWR_PROTECTION_RLP
#error code not present
#endif // FEATURE_MAX_PWR_PROTECTION_RLP
        }
#endif // FEATURE_IS2000_R_SCH

#ifdef FEATURE_MAX_PWR_PROTECTION_RLP
#error code not present
#endif // FEATURE_MAX_PWR_PROTECTION_RLP
#endif // FEATURE_MAX_PWR_PROTECTION

        rev_fr_ptr->srvc_type    = DSRLP_PRIMARY_SRVC;

        /*-------------------------------------------------------------------
          We are writing FCH Frame parameters. So use the FCH rate set
          information
        -------------------------------------------------------------------*/
        if (fundich_rate_set == ENC_RATE_9600)
        {
           rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_1;
           MSG_LOW("Making RS1 FCH Fr",0,0,0);
        }
        else if (fundich_rate_set == ENC_RATE_14400)
        {
            rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_2;
            MSG_LOW("Making RS2 FCH Fr",0,0,0);
        }
        else
        {
           ERR("Bad Rate Set TYPE. Fix it",0,0,0);
           // ASSERT(0); Use for debugging
        }


        rev_fr_ptr->fr_cat_mask  = DSRLP_CAT_DATA_MASK |
                                   DSRLP_CAT_IDLE_MASK |
                                   DSRLP_CAT_CTL_MASK;

    if (
         #ifdef FEATURE_IS2000_P2
         !msg_on_fch
         #else
         !txc_tx.active_msg
         #endif // FEATURE_IS2000_P2
       )
    {
#ifndef FEATURE_MAX_PWR_PROTECTION_RLP
      rev_fr_ptr->max_frame_size = dsrlp_len_table[ DSRLP_RATE_1]
                                                  [ DSRLP_PRIMARY_SRVC]
                                                  [ mux_pdu];

      rev_fr_ptr->fr_rate_mask = DSRLP_RATE_1_MASK   |
                                 DSRLP_RATE_1_2_MASK |
                                 DSRLP_RATE_1_4_MASK |
                                 DSRLP_RATE_1_8_MASK;
#endif // FEATURE_MAX_PWR_PROTECTION_RLP
        }
        else
        {
#ifdef FEATURE_MAX_PWR_PROTECTION_RLP
#error code not present
#endif // FEATURE_MAX_PWR_PROTECTION_RLP
  {
    rev_fr_ptr->fr_rate_mask  = DSRLP_RATE_BLANK_MASK;
    rev_fr_ptr->fr_cat_mask   = DSRLP_CAT_BLANK_MASK;
    rev_fr_ptr->fr_rate       = DSRLP_RATE_BLANK;
          }
        }
  }
  #ifdef FEATURE_IS2000_P2
  else
  {
    rev_fr_ptr->fr_rate_mask  = DSRLP_RATE_BLANK_MASK;
    rev_fr_ptr->fr_cat_mask   = DSRLP_CAT_BLANK_MASK;
    rev_fr_ptr->fr_rate       = DSRLP_RATE_INACTIVE;
    txc_data_rate = prim_rate = CAI_BLANK_RATE;
  }

  // Now do DCCH
  rev_fr_ptr = &txc_rev_fr_block->rev_arr[DCCH_BUF];
  if ( txc_so.rev_dcch.included )
  {
#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH
    txc_max_pwr_limited_dtx_sch = FALSE;
    if (txtc.sch_on && txc_sch_max_pwr_protection_enabled)
    {
      gain_req_full_rate =
            -txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain;

      if (tx_extra_gain <= gain_req_full_rate)
      {
        txc_max_pwr_limited_dtx_sch = TRUE;
      }
    }
#endif // FEATURE_IS2000_R_SCH
#endif // FEATURE_MAX_PWR_PROTECTION

    if (fundich_rate_set == ENC_RATE_9600)
    {
      rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_1;
      MSG_LOW("Making RS1 DCCH Fr",0,0,0);
    }
    else if (fundich_rate_set == ENC_RATE_14400)
    {
      rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_2;
      MSG_LOW("Making RS2 DCCH Fr",0,0,0);
    }
    else
    {
      ERR("Bad Rate Set TYPE. Fix it",0,0,0);
    }

    rev_fr_ptr->max_frame_size  =
                          dsrlp_len_table[ DSRLP_RATE_1]
                                         [ DSRLP_PRIMARY_SRVC]
                                         [ rev_fr_ptr->mux_pdu_type];

    rev_fr_ptr->fr_rate_mask = DSRLP_RATE_1_MASK;
    rev_fr_ptr->srvc_type     = DSRLP_PRIMARY_SRVC;
    txc_data_rate = prim_rate = CAI_FULL_RATE;
    if (
         #ifdef FEATURE_IS2000_P2
         msg_on_dcch
           #ifdef FEATURE_IS2000_CHS
           // No data allowed when in control hold
           || control_hold
           #endif // FEATURE_IS2000_CHS
         #else
         txc_tx.active_msg
         #endif // FEATURE_IS2000_P2
       )
    {
      // Need to revisit when simultaneous FCH and DCCH are supported
      rev_fr_ptr->fr_rate_mask  = DSRLP_RATE_BLANK_MASK;
      rev_fr_ptr->fr_cat_mask   = DSRLP_CAT_BLANK_MASK;
      rev_fr_ptr->fr_rate       = DSRLP_RATE_BLANK;
      txc_data_rate = prim_rate = CAI_BLANK_RATE;
    }
    else
    {
      rev_fr_ptr->fr_cat_mask  = DSRLP_CAT_DATA_MASK |
                                 DSRLP_CAT_IDLE_MASK |
                                 DSRLP_CAT_CTL_MASK;
    }
  }
  else
  {
    rev_fr_ptr->fr_rate_mask  = DSRLP_RATE_BLANK_MASK;
    rev_fr_ptr->fr_cat_mask   = DSRLP_CAT_BLANK_MASK;
    rev_fr_ptr->fr_rate       = DSRLP_RATE_INACTIVE;
    txc_data_rate = prim_rate = CAI_BLANK_RATE;
  }
  #endif // FEATURE_IS2000_P2

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
    filling in frame information for frame building. (SCH)
  -------------------------------------------------------------------------*/
  if (txtc.sch_on
#ifdef FEATURE_MAX_PWR_PROTECTION
              &&
      (!txc_max_pwr_limited_dtx_sch)
#endif // FEATURE_MAX_PWR_PROTECTION
     )
    {
      txc_rev_fr_block->max_frames += txtc.ok_num_mux_pdus;
      /*---------------------------------------------------------------------
        Time to find out SCH rate sets
      ---------------------------------------------------------------------*/
      sch_rate_set = ENC_RC_TO_RATE_SET(txtc.r_sch_rc);

      for( i = 0; i < txtc.pot_num_mux_pdus; i++)
      {
        /*-------------------------------------------------------------------
          Loop through all potential frames, as this will make logging
          easier.  But only pass up to RLP the "ok" number of frames.
        -------------------------------------------------------------------*/
        rev_fr_ptr = &(txc_rev_fr_block->rev_arr[i+SCH_BUF_START]);


        /*-------------------------------------------------------------------
          Check the rate set of the SCH (note this can be different from
          FCH). Update the muxPDU type. Note that this will be useful
          only for 1x case or MUXPDU 5 case. For Non-1x and Non-MUXPDU 5,
          we use MUXPDU3 but for 1x, we use either 1 or 2 .
        -------------------------------------------------------------------*/
        if (sch_rate_set == ENC_RATE_9600)
        {
#ifdef FEATURE_IS2000_REL_A
          if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
          {
            rev_fr_ptr->max_frame_size  = txc_muxpdu5_rs1_maxsize[txtc.supp_rate ][txtc.supp_turbo];
          }
          else
          {
#endif /* FEATURE_IS2000_REL_A */
            rev_fr_ptr->max_frame_size  = ( (txtc.double_sized) ? 43 : 21);
            mux_pdu = DSRLP_MUX_PDU_1;
            MSG_LOW("Making RS1 SCH Fr",0,0,0);
#ifdef FEATURE_IS2000_REL_A
          }
#endif /* #ifdef FEATURE_IS2000_REL_A */
        }
        else if(sch_rate_set == ENC_RATE_14400)
        {
#ifdef FEATURE_IS2000_REL_A
          if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
          {
            rev_fr_ptr->max_frame_size  = txc_muxpdu5_rs2_maxsize[txtc.supp_rate ][txtc.supp_turbo];
          }
          else
          {
#endif /* FEATURE_IS2000_REL_A */
           rev_fr_ptr->max_frame_size  = ( (txtc.double_sized) ? 67 : 33);
           mux_pdu = DSRLP_MUX_PDU_2;
           MSG_LOW("Making RS2 SCH Fr",0,0,0);
#ifdef FEATURE_IS2000_REL_A
          }
#endif /* #ifdef FEATURE_IS2000_REL_A */
        }
        else
        {
           ERR("Got Bad Rate Set info",0,0,0);
            // ASSERT(0); Use only for debugging
        }

        rev_fr_ptr->fr_rate_mask = DSRLP_RATE_1_MASK;

        rev_fr_ptr->srvc_type    = DSRLP_GENERIC_SRVC;

#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
        {
          rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_5;
          if (i < txtc.ok_num_mux_pdus)
          {
            rev_fr_ptr->fr_cat_mask  = DSRLP_CAT_DATA_MASK;
          }
          else
          {
            /*-----------------------------------------------------------------
              Reverse throttling taking place... don't build these
              frames.
            -----------------------------------------------------------------*/
            rev_fr_ptr->fr_cat_mask = DSRLP_CAT_BLANK_MASK;
          }
        }
        else
        {
#endif /* FEATURE_IS2000_REL_A */
          /*-------------------------------------------------------------------
            Since SCH frame now, uses only PDU3, hard code it.
          -------------------------------------------------------------------*/
          rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_3;

          if (i < txtc.ok_num_mux_pdus)
          {
            rev_fr_ptr->fr_cat_mask  = DSRLP_CAT_DATA_MASK;
          }
          else
          {
            /*-----------------------------------------------------------------
              Reverse throttling taking place... don't build these
              frames.
            -----------------------------------------------------------------*/
            rev_fr_ptr->fr_cat_mask = DSRLP_CAT_BLANK_MASK;
          }

          if (txtc.supp_rate == ENC_SCH_FULL_RATE)
          {
            /*-----------------------------------------------------------------
              For 1x SCH, use MUX_PDU1 or MUX_PDU2, per standard
            -----------------------------------------------------------------*/
            rev_fr_ptr->max_frame_size  =
                dsrlp_len_table[ DSRLP_RATE_1]
                               [ DSRLP_PRIMARY_SRVC]
                               [ mux_pdu];
            rev_fr_ptr->mux_pdu_type = mux_pdu;
            rev_fr_ptr->srvc_type    = DSRLP_PRIMARY_SRVC;
          }
#ifdef FEATURE_IS2000_REL_A
        }
#endif /* FEATURE_IS2000_REL_A */
      } /* for */
  } /* sch is on */
#endif /* FEATURE_IS2000_R_SCH */

  dsrlp_build_tx_frames(txc_rev_fr_block);

#ifdef FEATURE_IS2000_R_SCH
  /*-------------------------------------------------------------------------
     DTX duration checking is activated
  -------------------------------------------------------------------------*/
  if (txtc.dtx_duration_included )
  {
    if ((txtc.pot_num_mux_pdus > 0) &&
        (txc_rev_fr_block->num_frames <= SCH_BUF_START))
    {
      /*---------------------------------------------------------------------
        all SCH RLP frames are empty in the data block
      ---------------------------------------------------------------------*/
      if (txtc.dtx_duration > 0)
      {
        /* Detect the R-SCH transition from TX to DTX, then reset the
        continuous RSCH counter in the MUX statistic*/
        if (txtc.dtx_duration == txtc.dtx_duration_org)
        {
          /* second input = 0xFF will reset the corresponding parameter */
          parm_xtra_inc(CAI_XTRA_SCH0_REV_SDU_CON_DTX_ID, 0xFF);
        }
        txtc.dtx_duration --;
        /* increasing the continuous RSCH counter in the MUX statistic by 1 */
        parm_xtra_inc(CAI_XTRA_SCH0_REV_SDU_CON_DTX_ID, 1);
      }
      else
      {
        /*-------------------------------------------------------------------
          DTX expired, release SCH by sending SCRM to BS
          and disable the DTX duration checking.
        -------------------------------------------------------------------*/
        MSG_HIGH("DTX exp, DTX cnt = %d fr", txtc.dtx_duration_org, 0, 0);
        MSG_HIGH("Rel SCH", 0, 0, 0);
        mcc_set_rsch_dtx_sig();   /* Ask MC to release the SCH */
        txtc.dtx_duration_included = FALSE;
        txtc.dtx_duration_org = 0;
        txtc.txc_dtx_off = TRUE;
      }
    }
    else if (txc_rev_fr_block->num_frames > SCH_BUF_START)
    {
      /*---------------------------------------------------------------------
        The data TX resumed before the DTX duration expired.
        Reset dtx_duration to the dtx_duration_org
      ---------------------------------------------------------------------*/
      txtc.dtx_duration = txtc.dtx_duration_org;
    }
  }
#endif /* FEATURE_IS2000_R_SCH */

  if (txc_rev_fr_block->num_frames
      #ifdef FEATURE_IS2000_P2
      && txc_so.rev_fch.included
      #endif
      )
  {
    /* based on rate from RLP, set fr. rate*/
    rev_fr_ptr = &(txc_rev_fr_block->rev_arr[FCH_BUF]);
    switch ( rev_fr_ptr->fr_rate)
    {
       case DSRLP_RATE_1:
         txc_data_rate = prim_rate = CAI_FULL_RATE;
         prim_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
         break;

       case DSRLP_RATE_1_2:
         txc_data_rate = prim_rate = CAI_HALF_RATE;
         prim_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
         break;

       case DSRLP_RATE_1_4:
         txc_data_rate = prim_rate = CAI_QUARTER_RATE;
         prim_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
         break;

       case DSRLP_RATE_1_8:
         txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
         prim_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
         break;

       case DSRLP_RATE_BLANK:
         if (
             #ifdef FEATURE_IS2000_P2
             msg_on_fch
             #else
             txc_tx.active_msg
             #endif // FEATURE_IS2000_P2
             )
         {
         txc_data_rate = prim_rate = CAI_BLANK_RATE;
         }
         else
         {
           ERR("No Sgnling, but RLP 3 Tx Blank Rate FCH", 0, 0, 0 );
           txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
           prim_data_ptr = (byte *) &txc_rlp_null_data;
         }
         break;

       default:
         ERR("Bad Rate req by RLP 3 TX %d", rev_fr_ptr->fr_rate, 0, 0 );
         txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
         prim_data_ptr = (byte *) &txc_rlp_null_data;
         break;
    }
  } /* if num_frames > 0 */
  #ifdef FEATURE_IS2000_P2
  else if ( txc_rev_fr_block->num_frames && txc_so.rev_dcch.included )
  {
    /*-----------------------------------------------------------------------
      Let's see if Data has anything to send.Use following logic to decide
      DCCH Frame_Type can be either FULL or BLANK

      Frame_Type   Idle_Frame_Bit         ACtion
      BLANK         TRUE                  Donot Send anything
      BLANK         FALSE                 Donot Send Anything
      FULL          TRUE                  Umm.. Probably good to DTX, because
                                          its anyway Idle frame. In future
                                          decide, if anything else needs to
                                          be done
      FULL          FALSE                 Send the data
    -----------------------------------------------------------------------*/
    if ( txc_rev_fr_block->rev_arr[DCCH_BUF].idle_frame )
    {
      // Force this to BLANK if we DTX.  It's used in RLP logging later.
      txc_rev_fr_block->rev_arr[DCCH_BUF].fr_rate = DSRLP_RATE_BLANK;

      prim_dcch_rate     = CAI_NULL_RATE;
      prim_dcch_data_ptr = NULL;
    }
    else
    {
//    ASSERT (txc_rev_fr_block->rev_arr[DCCH_BUF].fr_rate != DSRLP_RATE_BLANK);

      // DCCH can only be either full rate or DTX.
      if (txc_rev_fr_block->rev_arr[DCCH_BUF].fr_rate == DSRLP_RATE_BLANK)
      {
        prim_dcch_rate     = CAI_NULL_RATE;
        prim_dcch_data_ptr = NULL;
      }
      else
      {
        prim_dcch_rate     = CAI_FULL_RATE;
        prim_dcch_data_ptr = txc_rev_fr_block->rev_arr[DCCH_BUF].fr_item_ptr->data_ptr;
      }
    }
  }
  #endif // FEATURE_IS2000_P2
  else
  {
    /*-----------------------------------------------------------------------
      Most likely here because call has ended, and RLP has been terminated
      but txc still running.  Cover this case.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_P2
    if (msg_on_fch)
    {
      txc_data_rate = prim_rate = CAI_BLANK_RATE;
    }
    else if (msg_on_dcch)
    {
      txc_data_rate = prim_dcch_rate = CAI_NULL_RATE;
    }
    else
    {
      if (txc_so.rev_fch.included)
      {
        /* No L3/RLP msg, send 1/8 rate NULL frame on FCH
        */
        txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
        prim_data_ptr = (byte *) &txc_rlp_null_data;
      }
      else
      {
        /* No L3/RLP msg, DTX DCCH
        */
        prim_dcch_rate     = CAI_NULL_RATE;
        prim_dcch_data_ptr = NULL;
      }
    }
#else
    if (txc_tx.active_msg)
    {
      txc_data_rate = prim_rate = CAI_BLANK_RATE;
    }
    else
    {
      txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
      prim_data_ptr = (byte *) &txc_rlp_null_data;
    }
#endif // FEATURE_IS2000_P2
  }
} /* txc_tx_frame_proc_build_data3g() */

#endif /* FEATURE_IS2000_REL_A_SVD */
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================
FUNCTION     TXC_TX_PROCESS_FUNDICH_DATA3G

DESCRIPTION  This function populates the RLP parameters required to build a
             data frame on the fundicated channel.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void
txc_tx_process_fundich_data3g
(
 word                 phy_channel,
 dsrlp_rate_enum_type max_rlp_rate,
 dsrlp_srvc_enum_type srvc_type
)
{
  dsrlp_rev_frame_type   *rev_fr_ptr;             /* Ptr to 1 RLP tx frame */
  cai_radio_config_type  chan_rc;

  if ((phy_channel != MCTRANS_PHY_CHAN_FCH) &&
      (phy_channel != MCTRANS_PHY_CHAN_DCCH))
  {
    ERR_FATAL("Bad phy ch", 0, 0, 0);
  }

  if (phy_channel == MCTRANS_PHY_CHAN_FCH)
  {
    rev_fr_ptr = &(txc_rev_fr_block.rev_arr[FCH_BUF]);
    chan_rc = txc_so.rev_fch.rc;
  }
#ifdef FEATURE_IS2000_P2
  else
  {
    rev_fr_ptr = &(txc_rev_fr_block.rev_arr[DCCH_BUF]);
    chan_rc = txc_so.rev_dcch.rc;
  }
#endif /* FEATURE_IS2000_P2 */

  if (max_rlp_rate == DSRLP_RATE_BLANK)
  {
    rev_fr_ptr->data_on_ch = FALSE;
  }
  else
  {
    rev_fr_ptr->data_on_ch = TRUE;
  }

  rev_fr_ptr->max_frame_rate = (uint16) max_rlp_rate;
  rev_fr_ptr->srvc_type      = srvc_type;

  if (ENC_RC_TO_RATE_SET(chan_rc) == ENC_RATE_9600)
  {
    rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_1;
  }
  else if (ENC_RC_TO_RATE_SET(chan_rc) == ENC_RATE_14400)
  {
    rev_fr_ptr->mux_pdu_type = DSRLP_MUX_PDU_2;
  }


  if (rev_fr_ptr->max_frame_rate == DSRLP_RATE_BLANK)
  {
    rev_fr_ptr->max_frame_size =  0;
    rev_fr_ptr->fr_cat_mask    = DSRLP_CAT_BLANK_MASK;
  }
  else
  {
    rev_fr_ptr->max_frame_size = dsrlp_len_table[rev_fr_ptr->max_frame_rate]
                          [rev_fr_ptr->srvc_type]
                          [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, DSRLP_MUX_PDU_3)];

    rev_fr_ptr->fr_cat_mask    = DSRLP_CAT_DATA_MASK |
                                 DSRLP_CAT_IDLE_MASK |
                                 DSRLP_CAT_CTL_MASK;
  }

#ifdef FEATURE_IS2000_P2
  if (phy_channel == MCTRANS_PHY_CHAN_DCCH)
  {
    rev_fr_ptr->fr_rate_mask = (rev_fr_ptr->max_frame_rate == DSRLP_RATE_1)
                               ? DSRLP_RATE_1_MASK
                               : DSRLP_RATE_BLANK_MASK;
  }
  else
#endif /* FEATURE_IS2000_P2 */
  {
    rev_fr_ptr->fr_rate_mask = dsrlp_mask_table[rev_fr_ptr->max_frame_rate]
                          [rev_fr_ptr->srvc_type]
                          [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, DSRLP_MUX_PDU_3)];
  }
}
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_DS_RLP3
/*===========================================================================
FUNCTION     TXC_TX_POST_RLP_FCH_PROCESS

DESCRIPTION  This function initializes the MUX R-FCH TX information like
             frame rate and frame buffer ptrs.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_post_rlp_fch_process
(
 dsrlp_rev_frame_block_type *rev_fr_block_ptr
)
{
  dsrlp_rev_frame_type *rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[FCH_BUF]);

  if (rev_fr_ptr->data_on_ch)
  {
    if (rev_fr_block_ptr->num_frames > 0)
    {
      if (rev_fr_ptr->fr_rate != DSRLP_RATE_BLANK)
      {
         ASSERT (rev_fr_ptr->fr_item_ptr != NULL);
      }

      if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
      {
        if (rev_fr_ptr->fr_item_ptr)
        {
          /* set prim_rate, prim_data_ptr
          */
          prim_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
        }

        switch (rev_fr_ptr->fr_rate)
        {
          case DSRLP_RATE_1:
            txc_data_rate = prim_rate = CAI_FULL_RATE;
            break;

          case DSRLP_RATE_1_2:
            txc_data_rate = prim_rate = CAI_HALF_RATE;
            break;

          case DSRLP_RATE_1_4:
            txc_data_rate = prim_rate = CAI_QUARTER_RATE;
            break;

          case DSRLP_RATE_1_8:
            txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
            break;

          case DSRLP_RATE_BLANK:
            if (rev_fr_ptr->max_frame_rate == DSRLP_RATE_BLANK)
            {
              /* signaling msg was pending, hence RLP was BLANK'd
              */
              prim_data_ptr = NULL;
              txc_data_rate = prim_rate = CAI_BLANK_RATE;
            }
            else
            {
              /* RLP had nothing to send
              */
              ERR("No Sgnling, but RLP 3 Tx Blank Rate FCH", 0, 0, 0 );
              txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
              prim_data_ptr = (byte *) &txc_rlp_null_data;
            }
            break;

         default:
            ERR("Bad Rate rtrnd by RLP 3 TX %d", rev_fr_ptr->fr_rate, 0, 0 );
            txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
            prim_data_ptr = (byte *) &txc_rlp_null_data;
            break;
        }
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
      {
        /* set sec_data_rate and sec_num_bits
        */
        if ((rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK) ||
            (rev_fr_ptr->fr_rate == DSRLP_RATE_INACTIVE))
        {
          sec_fch_data_ptr = NULL;
          sec_fch_num_bits = 0;
        }
        else
        {
          sec_fch_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
          sec_fch_num_bits = rlp_to_mux_table[ rev_fr_ptr->fr_rate ]
                [ DSRLP_SECONDARY_SRVC ]
                [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, DSRLP_MUX_PDU_3)];

          if ((sec_fch_num_bits == 0) || (sec_fch_data_ptr == NULL))
          {
            sec_fch_data_ptr = NULL;
            sec_fch_num_bits = 0;
            ERR_FATAL("SVD, RLP rtrnd MUX 0 bits",0,0,0);
          }
        }
      }
#endif /* FEATURE_IS2000_REL_A_SVD */
      else
      {
        ERR_FATAL("%d: Bad Traf type on FCH",rev_fr_ptr->srvc_type,0,0);
      }
    } /* if (txc_rev_fr_block->num_frames > 0) */
    else
    {
      /* RLP built nothing.
      */
      if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
      {
        if (rev_fr_ptr->max_frame_rate == DSRLP_RATE_BLANK)
        {
          /* signaling msg was pending, hence RLP was BLANK'd
          */
          prim_data_ptr = NULL;
          txc_data_rate = prim_rate = CAI_BLANK_RATE;
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
        }
        else
        {
          /* RLP had nothing to send
          */
          ERR("No Sgnling, but RLP 3 Tx Blank Rate FCH", 0, 0, 0 );
          txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
          prim_data_ptr = (byte *) &txc_rlp_null_data;
        }
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
      {
        sec_fch_data_ptr    = NULL;
        sec_fch_num_bits    = 0;
        rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
      }
#endif /* FEATURE_IS2000_REL_A_SVD */
    }
  }
  else
  {
//  ASSERT((txc_rev_fr_block->num_frames == 0) &&
//         (rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK));

    rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;

    if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
    {
      prim_data_ptr = NULL;
      prim_rate     = CAI_BLANK_RATE;
    }
#ifdef FEATURE_IS2000_REL_A_SVD
    else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
    {
      sec_fch_data_ptr = NULL;
      sec_fch_num_bits = 0;
    }
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
}
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_IS2000_P2
/*===========================================================================
FUNCTION     TXC_TX_POST_RLP_DCCH_PROCESS

DESCRIPTION  This function initializes the MUX R-DCCH TX information like
             frame rate and frame buffer ptrs.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void txc_tx_post_rlp_dcch_process
(
 dsrlp_rev_frame_block_type *rev_fr_block_ptr
)
{
  dsrlp_rev_frame_type *rev_fr_ptr = &(rev_fr_block_ptr->rev_arr[DCCH_BUF]);

  if (rev_fr_ptr->data_on_ch)
  {

    if (rev_fr_block_ptr->num_frames > 0)
    {
      if (rev_fr_ptr->fr_rate != DSRLP_RATE_BLANK)
      {
         ASSERT (rev_fr_ptr->fr_item_ptr != NULL);
      }

      if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
      {
        if (rev_fr_ptr->idle_frame ||
            (rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK))
        {
          prim_dcch_rate     = CAI_NULL_RATE;
          prim_dcch_data_ptr = NULL;

          /* Force this to BLANK if we DTX.  It's used in RLP logging later.
          */
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
        }
        else if (rev_fr_ptr->fr_rate == DSRLP_RATE_1)
        {
          /* DCCH can only be either full rate or DTX.
          */
          prim_dcch_rate     = CAI_FULL_RATE;
          prim_dcch_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;

          if (prim_dcch_data_ptr == NULL)
          {
            prim_dcch_rate = CAI_NULL_RATE;

            /* Force this to BLANK if we DTX.  It's used in RLP logging later.
            */
            rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
            ERR_FATAL("SVD, DCCH data ptr is NULL", 0, 0, 0);
          }
        }
        else
        {
          prim_dcch_rate      = CAI_NULL_RATE;
          prim_dcch_data_ptr  = NULL;
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
          ERR_FATAL("SVD, Bad rate %d", rev_fr_ptr->fr_rate, 0, 0);
        }
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
      {
        if (rev_fr_ptr->idle_frame ||
            (rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK))
        {
          sec_dcch_num_bits   = 0;
          sec_dcch_data_ptr   = NULL;

          /* Force this to BLANK if we DTX.  It's used in RLP logging later.
          */
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
        }
        else if (rev_fr_ptr->fr_rate == DSRLP_RATE_1)
        {
          /* DCCH can only be either full rate or DTX.
          */
          sec_dcch_data_ptr = rev_fr_ptr->fr_item_ptr->data_ptr;
          sec_dcch_num_bits = rlp_to_mux_table[ rev_fr_ptr->fr_rate ]
                     [ DSRLP_SECONDARY_SRVC ]
                     [TXC_UPPER_BOUNDS_CHECK(rev_fr_ptr->mux_pdu_type, DSRLP_MUX_PDU_3)];

          if ((sec_dcch_data_ptr == NULL) || (sec_dcch_num_bits == 0))
          {
            // REMOVE AFTER DEBUG
            sec_dcch_data_ptr = NULL;
            sec_dcch_num_bits = 0;
            ERR_FATAL("SVD, DCCH data ptr is NULL", 0, 0, 0);
          }
        }
        else
        {
          sec_dcch_data_ptr = NULL;
          sec_dcch_num_bits = 0;
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
          ERR_FATAL("SVD, Bad rate %d", rev_fr_ptr->fr_rate, 0, 0);
        }
      }
#endif /* FEATURE_IS2000_REL_A_SVD */
      else
      {
        ERR_FATAL("%d: Bad Traf type on FCH",rev_fr_ptr->srvc_type,0,0);
      }
    }
    else
    {
      /* rev_fr_block_ptr->num_frames == 0
      */
      if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
      {
        prim_dcch_rate     = CAI_NULL_RATE;
        prim_dcch_data_ptr = NULL;
      }
#ifdef FEATURE_IS2000_REL_A_SVD
      else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
      {
        sec_dcch_num_bits = 0;
        sec_dcch_data_ptr = NULL;
      }
#endif /* FEATURE_IS2000_REL_A_SVD */

      /* Force this to BLANK if we DTX.  It's used in RLP logging later.
      */
      rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
    }
  }
  else
  {
//  ASSERT((rev_fr_block_ptr->num_frames == 0) &&
//         (rev_fr_ptr->fr_rate == DSRLP_RATE_BLANK));

    rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;

    if (rev_fr_ptr->srvc_type == DSRLP_PRIMARY_SRVC)
    {
      prim_dcch_data_ptr = NULL;
      prim_dcch_rate     = CAI_BLANK_RATE;
    }
#ifdef FEATURE_IS2000_REL_A_SVD
    else if (rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC)
    {
      sec_dcch_data_ptr = NULL;
      sec_dcch_num_bits = 0;
    }
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
}
#endif /* FEATURE_IS2000_P2 */

/*===========================================================================

FUNCTION TXC_TX_FRAME_PROC

DESCRIPTION
   Note that this function is created by splitting the txc_voc_xchg into
   two parts. This part does the RLP frame building in case of data services
   service option and gets signalling from the CP. The real mulitplexing is
   done here.

   This is called from txc_traffic so as to build the TX frame in the background
   and not to load the TX ISR.

DEPENDENCIES
   This function was created on 24th March 1999. [AJ and GB]

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

/* build the frame in the background when the txc_voc_xchg signals */
void txc_tx_frame_proc(void)
{

/* This is new frame building in the background. If feature for minimizing the
** ISR loading is defined then we do not call RLP in the txc_pri which executes in
** TXC interrupt context but is called in this function which will be called from the
** TXC_TRAFFIC task. Thus RLP TX runs in background reducing the ISR loading.
*/
#ifdef FEATURE_IS2000_REL_A
  /* flush the RDCH buffer */
  static boolean flush_rdch_buffer = FALSE;
#endif /* End of FEATURE_IS2000_REL_A */

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_TX_ADJ_WINDDOWN
  int8   tx_winddown_gain_adj;
  int16  tx_winddown_gain_adj_floor;
  int8   tx_winddown_min_pwr;
  int16  tx_winddown_open_loop;
#endif // FEATURE_TX_ADJ_WINDDOWN

#ifdef FEATURE_DS_RLP3
#if defined(FEATURE_IS2000_R_SCH)
  byte i;                                       /* loop ctr              */
#endif
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_IS2000_REL_A
  boolean so_is_voice = (
                            (txc_so.pri.so == CAI_SO_VOICE_IS96A)
                         || (txc_so.pri.so == CAI_SO_VOICE_EVRC)
#ifdef FEATURE_SO_VOICE_SMV
                         || (txc_so.pri.so == CAI_SO_VOICE_SMV)
#endif
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
                         || (txc_so.pri.so == CAI_SO_VOICE_13K_IS733)
                         || (txc_so.pri.so == CAI_SO_VOICE_13K)
#endif
#ifdef FEATURE_VOC_4GV
                         || (txc_so.pri.so == CAI_SO_VOICE_4GV_NB)
                         || (txc_so.pri.so == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */
                        );
#else
  boolean so_is_voice = (
                            (txc_so.pri == CAI_SO_VOICE_IS96A)
                         || (txc_so.pri == CAI_SO_VOICE_EVRC)
#ifdef FEATURE_SO_VOICE_SMV
                         || (txc_so.pri == CAI_SO_VOICE_SMV)
#endif
#ifndef FEATURE_NO_13K_VOICE_SUPPORT
                         || (txc_so.pri == CAI_SO_VOICE_13K_IS733)
                         || (txc_so.pri == CAI_SO_VOICE_13K)
#endif
#ifdef FEATURE_VOC_4GV
                         || (txc_so.pri == CAI_SO_VOICE_4GV_NB)
                         || (txc_so.pri == CAI_SO_VOICE_4GV_WB)
#endif /* FEATURE_VOC_4GV */
                        );
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_R_SCH
  int16  tx_open_loop, tx_gain_adj, tx_pwr_limit;
#endif /* FEATURE_IS2000_SCH */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Store the frame rate for logging.  curr is the one being
  * encoded now.  next is the one that will be sent in 20ms. */
  curr_sch0_frame_rate = next_sch0_frame_rate;

#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef FEATURE_IS2000_R_SCH

   MSM_LOCK();

   /* Read in 10-bit TX_OPEN_LOOP value */
   /* Read in 9-bit TX_GAIN_ADJ value, units -1/4 dB */
   tx_open_loop = rfm_get_tx_open_loop(RFCOM_TRANSCEIVER_0);
   tx_gain_adj = rfm_get_tx_gain_adjust(RFCOM_TRANSCEIVER_0);

   MSM_UNLOCK();

   /* convert open loop to -1/12 dB units */
   tx_open_loop *= -1;

   /* change TX_GAIN_ADJ value to -1/12 dB units */
   tx_gain_adj *= -3;

   /* find out how much extra gain is available */
   tx_pwr_limit = (int16) rfm_get_cdma_tx_pwr_limit(RFCOM_TRANSCEIVER_0);
   tx_extra_gain = -(511 - tx_pwr_limit) - tx_open_loop - tx_gain_adj;

   /* add the difference between turnaround constants for RC1,RC2 and RC3,RC4 -
   ** since this gets added currently to the RATE_ADJ register in MSM.
   */
   tx_extra_gain += txtc.offset_pwr;

#endif // FEATURE_IS2000_R_SCH
#endif /* FEATURE_MAX_PWR_PROTECTION */

  if ( !txtc.preambles )
  {
#ifdef FEATURE_DS
  /* DS_ISDATA is a macro containing several IFs.
  */
#ifndef FEATURE_IS2000_REL_A
    if ( DS_ISDATA (txc_so.pri) )
#else
    if ( DS_ISDATA (txc_so.pri.so) )
#endif /* FEATURE_IS2000_REL_A */
    {
      /* Fill up the buffer with data, returns rate */
      if (txc_tx.tx_on)
      {
#ifndef FEATURE_IS2000_REL_A
        if( txc_so.pri != CAI_SO_PPP_PKT_DATA_3G)
#else
        if( txc_so.pri.so != CAI_SO_PPP_PKT_DATA_3G)
#endif /* FEATURE_IS2000_REL_A */
        {
          /* Can send data on the FCH. Figure out the rate. */
          if (!txc_tx.active_msg)
          {
            switch (rlp_tx_get_next_frame( &prim_data_ptr, TRUE, RLP_FULL_RATE ))
            {
              case RLP_EIGHTH_RATE:
                txc_data_rate = prim_rate = CAI_EIGHTH_RATE;
                break;

              case RLP_HALF_RATE:
                txc_data_rate = prim_rate = CAI_HALF_RATE;
                break;

              case RLP_FULL_RATE:
                txc_data_rate = prim_rate = CAI_FULL_RATE;
                break;

              case RLP_QUARTER_RATE:
                /* Not 3G call, should only be for FCH. */
                if( ENC_RC_TO_RATE_SET( txc_so.rev_fch.rc ) == ENC_RATE_14400)
                {
                  txc_data_rate = prim_rate = CAI_QUARTER_RATE;
                  break;
                }
                /* else fall thru for ERR_FATAL */

              default:
                ERR_FATAL("Bad Rate req by RLP TX", 0, 0, 0 );
            }
          }
          else
          {
            txc_data_rate = prim_rate = CAI_BLANK_RATE;
          }
        }/* if not SO 33 */
#ifdef FEATURE_DS_RLP3
        else
        {
          txc_tx_frame_proc_build_data3g(&txc_rev_fr_block);
        }
#endif /* FEATURE_DS_RLP3 */
      } /*txc_tx.tx_on == TRUE */
      else
      {
        txc_data_rate = prim_rate = CAI_BLANK_RATE;
      }
    }/* if DS_ISDATA */

#ifdef FEATURE_IS2000_REL_A_SVD
    else if (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G)
    {
      if (txc_tx.tx_on)
      {
        /* process Secondary data
        */
        txc_tx_frame_proc_build_data3g (&txc_rev_fr_block);
      }
      else
      {
        /* txc_tx.tx_on == FALSE
        */
#ifdef FEATURE_IS2000_P2
        if (txc_so.rev_dcch.included &&
                 (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
        {
          sec_dcch_num_bits = 0;
          sec_dcch_data_ptr = NULL;
        }

        if (txc_so.rev_fch.included &&
            (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_FCH))
#endif /* FEATURE_IS2000_P2 */
        {
          sec_fch_num_bits = 0;
          sec_fch_data_ptr = NULL;
        }
      }
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

#endif /* FEATURE_DS */

#if( defined( FEATURE_IS2000_TDSO ) && defined( FEATURE_IS2000_R_SCH ) )
  //
  // TDSO SCH part
  //
#ifndef FEATURE_IS2000_REL_A
    else if(( txc_so.pri == CAI_SO_TDSO ) ||
            (txc_so.pri == CAI_SO_FULL_TDSO ))
#else
    else if(( txc_so.pri.so == CAI_SO_TDSO ) ||
            (txc_so.pri.so == CAI_SO_FULL_TDSO ))
#endif /*FEATURE_IS2000_REL_A*/
    {
#ifdef FEATURE_MAX_PWR_PROTECTION
      txc_max_pwr_limited_dtx_sch = FALSE;
      if (txtc.sch_on && txc_sch_max_pwr_protection_enabled)
      {
#ifdef FEATURE_IS2000_P2
        if (txc_so.rev_fch.included)
#endif // FEATURE_IS2000_P2
        {
          if (tx_extra_gain <= -txc_sch_pch_adj_gain_tab [ENC_FULL_RATE])
          {
            txc_max_pwr_limited_dtx_sch = TRUE;
          }
        }

#ifdef FEATURE_IS2000_P2
        // Now do DCCH
        if (txc_so.rev_dcch.included &&
           (tx_extra_gain <=
              -txc_dcch_sch_gain_tab [DCCH_ACTIVE_SCH_ACTIVE].pch_adj_gain))
        {
          txc_max_pwr_limited_dtx_sch = TRUE;
        }
#endif // FEATURE_IS2000_P2
      }
#endif // FEATURE_MAX_PWR_PROTECTION

      if (txtc.sch_on
#ifdef FEATURE_MAX_PWR_PROTECTION
          &&
          !txc_max_pwr_limited_dtx_sch
#endif // FEATURE_MAX_PWR_PROTECTION
      )
      {
#ifdef FEATURE_DS_IS2000_RS2
        if (txtc.r_sch_rc == CAI_RC_4)
        {
           sch_tdso_frame_block.sup_fr_mux = TDSO_RATE_SET_2;
        }
        else
#endif /* FEATURE_DS_IS2000_RS2 */
        {
           sch_tdso_frame_block.sup_fr_mux = TDSO_RATE_SET_1;
        }
        sch_tdso_frame_block.fr_time = qw_lo( txc_tx.frame_cnt );
#ifdef FEATURE_IS2000_REL_A
        /* if turbo code, use only one muxpdu 5 for the whole frame */
        if (txtc.supp_turbo && (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5))
        {
          txtc.pot_num_mux_pdus = 1;
        }
#endif /* FEATURE_IS2000_REL_A */

        for( i = 0; i < txtc.pot_num_mux_pdus; i++ )
        {
          sch_tdso_frame_block.frame_type.rev_arr[ i ].sch_rate =
             (tdso_sch_rate_type) (txtc.supp_rate - 3);

          sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_channel =
             TDSO_SCH0;

        #ifdef FEATURE_IS2000_REL_A
          if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5 )
          {
            sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type = TDSO_MUX_PDU_5;
            if (sch_tdso_frame_block.sup_fr_mux == TDSO_RATE_SET_1)
            {
              sch_tdso_frame_block.frame_type.rev_arr[ i ].max_frame_size =
                txc_tdso_muxpdu5_rs1[sch_tdso_frame_block.frame_type.rev_arr[ i ].sch_rate][txtc.supp_turbo];
            }
            else
            {
              sch_tdso_frame_block.frame_type.rev_arr[ i ].max_frame_size =
                txc_tdso_muxpdu5_rs2[sch_tdso_frame_block.frame_type.rev_arr[ i ].sch_rate][txtc.supp_turbo];
            }
          }
          else
          {
            if( txtc.double_sized )
            {
              sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate =
                                                TDSO_RATE_2; // double sized, rate set 1
            }
            else
            {
              sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate =
                                                TDSO_RATE_1; // single sized, rate set 1
            }

            // sch_tdso_frame_block.fr_time = ???;

            sch_tdso_frame_block.frame_type.rev_arr[ i ].max_frame_size =
                    tdso_data_len[ sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate ]
                                 [ sch_tdso_frame_block.sup_fr_mux ];

            if( txtc.supp_rate > ENC_SCH_FULL_RATE )
            {
              sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type = TDSO_MUX_PDU_3;
            }
            else
            {
      #ifdef FEATURE_DS_IS2000_RS2
              if (txtc.r_sch_rc == CAI_RC_4)
              {
                sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type =
                             TDSO_MUX_PDU_2;
              }
              else
      #endif /* FEATURE_DS_IS2000_RS2 */
              {
                sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type =
                              TDSO_MUX_PDU_1;
              }
            }

          }
      #else

          if( txtc.double_sized )
          {
            sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate =
               TDSO_RATE_2; // double sized, rate set 1
          }
          else
          {
            sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate =
               TDSO_RATE_1; // single sized, rate set 1
          }

          // sch_tdso_frame_block.fr_time = ???;

          sch_tdso_frame_block.frame_type.rev_arr[ i ].max_frame_size =
             tdso_data_len[ sch_tdso_frame_block.frame_type.rev_arr[ i ].fr_rate ]
                          [ sch_tdso_frame_block.sup_fr_mux ];

          if( txtc.supp_rate > ENC_SCH_FULL_RATE )
          {
            sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type =
               TDSO_MUX_PDU_3;
          }
          else
          {
#ifdef FEATURE_DS_IS2000_RS2
            if (txtc.r_sch_rc == CAI_RC_4)
            {
               sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type =
                  TDSO_MUX_PDU_2;
            }
            else
#endif /* FEATURE_DS_IS2000_RS2 */
            {
               sch_tdso_frame_block.frame_type.rev_arr[ i ].mux_pdu_type =
                  TDSO_MUX_PDU_1;
            }
          }
#endif /* FEATURE_IS2000_REL_A */
          sch_tdso_frame_block.frame_type.rev_arr[ i ].srvc_type =
             TDSO_PRIMARY_SRVC;
        }
        sch_tdso_frame_block.num_frames = txtc.pot_num_mux_pdus;
        if (tdso_build_tx_frames(REV_LINK, &sch_tdso_frame_block ) != TDSO_VALID)
        {
          MSG_ERROR("TDSO Build Failed",0,0,0);
        }

        txc_rev_fr_block.num_frames = sch_tdso_frame_block.num_frames;
      }
      else
      {
        sch_tdso_frame_block.num_frames = 0;
        txc_rev_fr_block.num_frames = 0;
      }
    } /* if TDSO */
#endif // FEATURE_IS2000_TDSO && ...R_SCH
  } // of if (!txtc.preambles)
  else
  {
    // Set sig to NORM for preambles
    txtc.frame.sig = NORM;
  }

  /* build the actual frame buffer to be sent */
#ifdef FEATURE_FACTORY_TESTMODE

  if(ftm_mode == FTM_MODE)
  {
    txc_tx.active_msg = FALSE;
    txc_tx.delay = TXC_NO_CONF;              /* no confirmation delay */
    txc_eib_flag = FALSE;
#ifdef FEATURE_IS2000_REL_A
    txc_sch_eib_flag = FALSE;
    txc_qib_flag = FALSE;
#endif /* FEATURE_IS2000_REL_A */
    if( txtc.num_preambles > 0 )
    {
      txtc.preambles = TRUE;
      --txtc.num_preambles;
    }
    else
    {
      txtc.preambles = FALSE;
#ifdef FEATURE_PLT
#error code not present
#endif

        txtc.rate = txc_ftm_rfch_rate;

#ifdef FEATURE_IS2000_R_SCH
        txtc.supp_rate = txc_ftm_rsch_rate;
#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
        rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,txtc.sch_on, txtc.supp_rate, txc_so.rev_dcch.included);
#else
        rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,txtc.sch_on, txtc.supp_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */
#endif /*FEATURE_IS2000_R_SCH*/

#ifndef FEATURE_PLT
        /* Copy tx_pattern or rx loopback data */
#ifdef FEATURE_IS2000_P2
        /* Support FEATURE_FTM_DCCH */
        if (txc_so.rev_dcch.included)
        {
          txtc.rate = txc_ftm_rdcch_rate;
          (void)memcpy ( txtc.frame.data, txc_ftm_rdcch_data.data,
                         ENC_14400_FULL_RATE_SIZE );
        }
    /* End of FEATURE_FTM_DCCH */
        if (txc_so.rev_fch.included)
#endif
        {
          (void)memcpy ( txtc.frame.data, txc_ftm_rfch_data.data,
                         ENC_14400_FULL_RATE_SIZE );
        }

#ifdef FEATURE_IS2000_R_SCH
        (void)memcpy ( txtc.supp_frame.data, txc_ftm_rsch_data.data,
#ifdef FEATURE_32X_SCH_DATA_RATE
                   ENC_SCH_RC3_32X_SIZE );
#else
                   ENC_SCH_RC3_16X_SIZE );
#endif /* FEATURE_32X_SCH_DATA_RATE */
#endif /*FEATURE_IS2000_R_SCH*/

#endif

        //
        // The data rates are set in txc_traffic_init()
        //
        if(
           #ifdef FEATURE_IS2000_P2
           txc_so.rev_dcch.included ||
           #endif // FEATURE_IS2000_P2
           txc_so.rev_fch.rc >= CAI_RC_3 )
        {
#if defined(FEATURE_PLT)
#error code not present
#endif // FEATURE_PLT

          enc_pcbit_test_data( 0xffff );

#ifdef FEATURE_IS2000_R_SCH
          if( txtc.sch_on )
          {
            txc_pch_gain =
               txc_pch_fch_sch_gain_tab[TXC_UPPER_BOUNDS_CHECK(
                                  txtc.supp_rate, ENC_NUM_SCH_RATES)].pch_gain;

            if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
            {
               TXC_SET_PCH_GAIN( txc_pch_gain );
            }
            {
               int16  pch_adj_gain;
               pch_adj_gain =
                   rpc_get_pch_adj_gain
                   (
                     PCH_GAIN_TRUNCATE_TO_8BITS(txc_pch_fch_sch_gain_tab[
                          TXC_UPPER_BOUNDS_CHECK(txtc.supp_rate, ENC_NUM_SCH_RATES)].pch_gain)
                   );
               txtc.tx_rate_adj = pch_adj_gain + txtc.offset_pwr;
               rpc_set_tx_rate_adj (txtc.tx_rate_adj);
            }
          }
          else
#endif /*FEATURE_IS2000_R_SCH*/
          {
            txc_pch_gain =
               txc_pch_fch_gain_tab[ txtc.rate ].pch_gain;

            if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
            {
               TXC_SET_PCH_GAIN( txc_pch_gain );
            }
            txtc.tx_rate_adj = txc_pch_adj_gain_tab [txtc.rate] +
                               txtc.offset_pwr;
            rpc_set_tx_rate_adj (txtc.tx_rate_adj);
          }

        }
        else
        {
          TXC_SET_PCH_GAIN( 0 );
          txtc.tx_rate_adj = txtc.offset_pwr;
          rpc_set_tx_rate_adj (txtc.tx_rate_adj);
        }

        if( txc_so.rev_fch.rc >= CAI_RC_3 )
        {
#ifdef FEATURE_IS2000_R_SCH
          if( txtc.sch_on )
          {
             txc_fch_gain =
                txc_pch_fch_sch_gain_tab[TXC_UPPER_BOUNDS_CHECK(
                                  txtc.supp_rate, ENC_NUM_SCH_RATES)].fch_gain;

             if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
             {
                TXC_SET_FCH_GAIN( txc_fch_gain );
             }
          }
          else
#endif /* FEATURE_IS2000_R_SCH */
          {
             txc_fch_gain =
                txc_pch_fch_gain_tab[ txtc.rate ].fch_gain;

             if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
             {
                TXC_SET_FCH_GAIN( txc_fch_gain );
             }
          }
        }
        else
        {
           TXC_SET_FCH_GAIN( TXC_IS95A_FCH_GAIN );
        } /* end if (txc_so.rev_fch.rc >= ENC_FCH_RC_3) */

#ifdef FEATURE_PLT /* Support FEATURE_FTM_DCCH */
#error code not present
#else
#ifdef FEATURE_IS2000_P2

        if ( txc_so.rev_dcch.included == TRUE )
        {
          txtc.dcch_frame.dtx = FALSE;
          if (txc_ftm_dtx_dcch == TRUE)
          {
            txtc.dcch_frame.dtx = TRUE;
            txc_ftm_dtx_dcch = FALSE;
          }
          if( txtc.sch_on )
          {
            if (txtc.dcch_frame.dtx)
            {
              /* set the gain to zero if DTX */
              txc_dcch_gain = 0;
            }
            else
            {
              txc_dcch_gain =
                txc_pch_fch_sch_gain_tab[TXC_UPPER_BOUNDS_CHECK(
                          txtc.supp_rate, ENC_NUM_SCH_RATES)].fch_gain;
            }

           if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
            {
             TXC_SET_DCCH_GAIN( txc_dcch_gain );
            }

          }
          else
          {
            if (txtc.dcch_frame.dtx)
            {
              /* set the gain to zero if DTX */
              txc_dcch_gain = 0;
            }
            else
            {
              txc_dcch_gain =
                txc_pch_fch_gain_tab[ txtc.rate ].fch_gain;
            }

           if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
            {
             TXC_SET_DCCH_GAIN( txc_dcch_gain );
            }
          }
        }
#endif
        /* End of FEATURE_FTM_DCCH */
#endif /* FEATURE_PLT */


#ifdef FEATURE_IS2000_R_SCH
        if( txtc.sch_on )
        {
          txc_sch_gain =
             txc_pch_fch_sch_gain_tab[TXC_UPPER_BOUNDS_CHECK(
                          txtc.supp_rate, ENC_NUM_SCH_RATES)].sch_gain;

          if ( TXC_LAST_ENC_INT_BIN == ENC_STATUS_INTERRUPT_SLOT_3 )
          {
             TXC_SET_SCH_GAIN( txc_sch_gain );
          }
        }
        else
#endif /* FEATURE_IS2000_R_SCH */
        {
          TXC_SET_SCH_GAIN( 0 );
        }
#ifdef FEATURE_PLT
#error code not present
#endif
    }
  }
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

  if(ftm_mode == DMSS_MODE)
  {
#endif /* FEATURE_FACTORY_TESTMODE */

#ifdef FEATURE_IS2000_P2
    if ( txc_so.rev_dcch.included )
    {
      txc_build_dcch_frame ();
    }

    if ( txc_so.rev_fch.included )
#endif // FEATURE_IS2000_P2
    {
      txc_sig_frame_type  signext = txtc.frame.sig;

      /* fill txtc frame (fundamental) with zeros */
      (void)memcpy( txtc.frame.data, zeros, ENC_14400_FULL_RATE_SIZE );

#ifdef FEATURE_IS2000_REL_A_SVD
      /*
      ** If R-FCH is not the signaling channel,
      ** build only normal traffic frame on FCH
      ** regardless of whether a signaling msg is pending or not.
      */
      if (txtc.cur_dsch != TXC_REV_FCH)
      {
        signext = NORM;
      }
      else
#endif /* FEATURE_IS2000_REL_A_SVD */

      /* In hard handoff case, txtc.frame.sig == NORM may be overwritten by
       * the call to txc_traffic_signext() above since HCM has been scheduled
       * and is pending. If signext is not set to NORM here when preamble
       * is in progress, then the preamble duration will exceed duration given
       * by standard.
       */
      if (txtc.preambles == TRUE)
      {
        signext = NORM;
      }

      /* build the actual frame buffer (fundamental) to be sent */
      txc_build_frame (signext);
    }

#ifdef FEATURE_FACTORY_TESTMODE
  }
#endif /* FEATURE_FACTORY_TESTMODE */

  /* Store the fch frame rate (full/half/...) and signalling
   * type (dim/blank/norm) for logging.  curr is the one being
   * encoded now.  next is the one that will be sent in 20ms.
   * next_fch_sig_type is set by txc_build_frame().
   */

  curr_fch_frame_rate = next_fch_frame_rate;
  next_fch_frame_rate = txtc.rate;

  /* Log frame type if we are not sending preambles */
  if ( !txtc.preambles )
  {
#ifdef FEATURE_IS2000_REL_A
    if (log_status(LOG_RDCH_FRAME_INFO_C))
    {
      txc_rdch_log_frame_info();
      flush_rdch_buffer = TRUE;
    }
    else if (flush_rdch_buffer)
    {
      /* flush the buffer */
      txc_rdch_frame_info_send_log();
      flush_rdch_buffer = FALSE;
    }
#else
    txc_log_frame_type();
#endif /* FEATURE_IS2000_REL_A */
  }

  /* Since curr_fch_sig_type has been logged, now update it to its value
     for the next time txc_log_frame_type() will be called */
  curr_fch_sig_type = next_fch_sig_type;


  if (txc_tx.active_msg)   /* Currently working on a signalling message */
  {
    if (txc_tx.buf.pos >= txc_tx.buf.len)   /* Done with message? */
    {
      txtc.frame.sig = NORM;        /* No message data this time */
      txc_tx.active_msg = FALSE;    /* No signalling message for now */

      /* Log message
      */
      #ifdef FEATURE_IS2000_P2
      txc_log_tc_msg( txtc.cur_dsch, &txc_tx.buf );
      #else
      txc_log_tc_msg( TXC_REV_FCH, &txc_tx.buf );
      #endif

      /* Check if it is necessary to delay confirmation to MC by 1 frame.
      ** This delay is used to stagger back to back signalling messages to
      ** allow voice to have 1 full rate frame before another signalling
      ** message is sent.  In case of Release B, RXTX wants the confirmation
      ** when the message has been sent, not when it is built.
      */
      if (
           (
            (so_is_voice)
#ifdef FEATURE_IS2000_P2
            && (txtc.cur_dsch == TXC_REV_FCH)
#endif // FEATURE_IS2000_P2
           )
#ifdef FEATURE_IS2000_REL_B
           || (txc_tx.confirmation_type == CONFIRM_MSG_SENT)
#endif /* FEATURE_IS2000_REL_B */
         )
      {
        txc_tx.delay = TXC_DELAY_CONF;
      }
      else
      {
        txc_tx.delay = TXC_SEND_CONF;
      } /* end if (SO_IS_VOICE... */
    }
    else /* else of if (txc_tx.buf.pos... */
    {
      /* work out whether we need a blank or dim frame next */
      txc_traffic_signext();
    }
  }
  else /* else of if (txc_tx.active_msg) */
  {
    txtc.frame.sig = NORM;        /* No message data this time */
  } /* end if (txc_tx.active_msg) */

  /* Check if we need to send signalling confirmation back to MC.
  */
  if (txc_tx.delay != TXC_NO_CONF)
  {
    if (txc_tx.delay == TXC_DELAY_CONF)
    {
      /* Delay confirmation till the next frame.
      */
      txc_tx.delay = TXC_SEND_CONF;
    }
    else
    {
      /* Send confirmation this frame.
      */
      txc_tx.delay = TXC_NO_CONF;
      MSG_HIGH ("Tx sig msg conf,active_msg=%d,pos=%d,len=%d",
               txc_tx.active_msg,
               txc_tx.buf.pos,
               txc_tx.buf.len
             );

      /* Send confirmation and dispose of command buffer.
      */
      txc_cmd_done( txc_tx.buf.cmd_ptr, TXC_DONE_S );
    }
  } /* end if (txc_tx.delay != TXC_NO_CONF) */

  /*
  ** Set Rate set 2 forward power control erasure reporting bit
  */
  if (
       #ifdef FEATURE_IS2000_P2
       txc_so.rev_fch.included &&
       #endif  // FEATURE_IS2000_P2
       txc_so.rev_fch.rc == CAI_RC_2 &&
       txc_eib_flag )
  {
    txtc.frame.data[0] |= 0x80;
    txc_eib_flag = FALSE;
  }


  /* Send data to encoder, not generating a CRC if we are currently
  ** transmitting traffic channel preamble frames.
  */
#ifdef FEATURE_FACTORY_TESTMODE
  if(ftm_mode == FTM_MODE)
  {
    if( txtc.preambles )
    {
#ifdef FEATURE_IS2000_P2
           /* Support FEATURE_FTM_DCCH */
           if (txc_so.rev_dcch.included)
           {
                 enc_dcch_data_out( zeros, FALSE, ENC_FULL_RATE );
                 MSG_HIGH( "Tx Preambl fr", 0, 0, 0);

           }
           /* End of FEATURE_FTM_DCCH */
           if(txc_so.rev_fch.included)
#endif /* FEATURE_IS2000_P2 */
           {
                 enc_fch_data_out( zeros, FALSE, ENC_FULL_RATE );
                 MSG_HIGH( "Tx Preambl fr", 0, 0, 0);

           }
    }
    else
    {
#if defined(FEATURE_PLT) && !defined(FEATURE_NO_SCH_HW)
#error code not present
#endif /* FEATURE_PLT */
#ifdef FEATURE_IS2000_R_SCH
       if( txtc.sch_on )
       {
          enc_sch_data_out
          (
             txtc.supp_frame.data,
             TRUE,                     // generate CRC or not
             txtc.supp_rate,
             txtc.supp_turbo,          // Turbo or not
             txtc.supp_ltu_size        // LTU size; 0 means no LTU
          );
          enc_write_data_done();
       }
#endif /* FEATURE_IS2000_R_SCH */
    }
  }


  /* Set RL channel gains.
  */
  if(ftm_mode == DMSS_MODE)
  {
#endif /* FEATURE_FACTORY_TESTMODE */
    if ( txc_tx.tx_on )
    {

  /* Set gains properly when sending preambles for RC 3 or higher
     Note: only PiCH transmitted on preambles */
      if (txtc.preambles && (
          #ifdef FEATURE_IS2000_P2
          txc_so.rev_dcch.included ||
          #endif // FEATURE_IS2000_P2
          (txc_so.rev_fch.rc >= CAI_RC_3)) )
      {
        /* During preambles set R-FCH, R-DCCH, R-SCH digital gain = 0
        */
        txc_pch_gain  = RPC_TOT_DIG_PWR_255<<7;
        txc_fch_gain  = 0;
        txc_dcch_gain = 0;
        txc_sch_gain  = 0;

        txtc.tx_rate_adj = txtc.offset_pwr;
      }
      else
      {
        if (
           #ifdef FEATURE_IS2000_P2
           txc_so.rev_dcch.included ||
           txc_so.rev_fch.included &&
           #endif  // FEATURE_IS2000_P2
           txc_so.rev_fch.rc >= CAI_RC_3 )
        {
#ifdef FEATURE_IS2000_R_SCH
          if ( txtc.sch_on )
          {
            if ( (
#ifndef FEATURE_IS2000_REL_A
                 ( txc_so.pri == CAI_SO_PPP_PKT_DATA_3G ) &&
#else
                 ( ( txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G ) ||
                   ( txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G ) ) &&
#endif /* FEATURE_IS2000_REL_A */
                 ( txc_rev_fr_block.num_frames > SCH_BUF_START ) )
#ifdef FEATURE_IS2000_TDSO
                 ||
                 (
#ifndef FEATURE_IS2000_REL_A
                 ( ( txc_so.pri == CAI_SO_TDSO ) ||
                   ( txc_so.pri == CAI_SO_FULL_TDSO ) ) &&
#else
                 ( ( txc_so.pri.so == CAI_SO_TDSO ) ||
                   ( txc_so.sec.so == CAI_SO_TDSO ) ||
                   ( txc_so.pri.so == CAI_SO_FULL_TDSO ) ||
                   ( txc_so.sec.so == CAI_SO_FULL_TDSO ) ) &&
#endif /* FEATURE_IS2000_REL_A */
                 ( txc_rev_fr_block.num_frames > 0 ) )
#endif
               )
            {
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
              if (txc_so.rev_dcch.included && txc_so.rev_fch.included)
              {
                txc_fch_dcch_sch_gain_type *gain_ptr;
                 /* FCH and SCH are active and DCCH is DTXed */
                 if (txtc.dcch_frame.dtx)
                 {
                   gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_DTX_SCH_ACTIVE];
                 }
                 else /* FCH, DCCH and SCH are active */
                 {
                   gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_ACTIVE_SCH_ACTIVE];
                 }
                 txc_pch_gain = gain_ptr->gain.pch_gain;
                 txc_fch_gain = gain_ptr->gain.fch_gain;
                 txc_dcch_gain = gain_ptr->gain.dcch_gain;
                 txc_sch_gain = gain_ptr->gain.sch_gain;
                 txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;
              }
              else if (txc_so.rev_dcch.included)
#else
              if (txc_so.rev_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
              {
                txc_dcch_sch_gain_type *gain_ptr;

                if (txtc.dcch_frame.dtx)
                {
                  gain_ptr = &txc_dcch_sch_gain_tab[DCCH_DTX_SCH_ACTIVE];
                }
                else
                {
                  gain_ptr = &txc_dcch_sch_gain_tab[DCCH_ACTIVE_SCH_ACTIVE];
                }

                txc_pch_gain = gain_ptr->gain.pch_gain;
                txc_fch_gain = gain_ptr->gain.fch_gain;
                txc_dcch_gain = gain_ptr->gain.dcch_gain;
                txc_sch_gain = gain_ptr->gain.sch_gain;
                txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;

              }
              else
#endif /* FEATURE_IS2000_P2 */
              {
                txc_pch_gain = txc_gain_tab[ txtc.rate ].pch_gain;
                txc_fch_gain = txc_gain_tab[ txtc.rate ].fch_gain;
                txc_sch_gain = txc_gain_tab[ txtc.rate ].sch_gain;
#ifdef FEATURE_IS2000_P2
                txc_dcch_gain = txc_gain_tab[ txtc.rate ].dcch_gain;
#endif
                txtc.tx_rate_adj = txc_sch_pch_adj_gain_tab[ txtc.rate ] +
                                   txtc.offset_pwr;

              }
            }
            else
            {
                  /* R-SCH is DTX */
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
              if (txc_so.rev_dcch.included && txc_so.rev_fch.included)
              {
                 txc_fch_dcch_sch_gain_type *gain_ptr;
                 /* FCH is active. SCH and DCCH are DTXed */
                 if (txtc.dcch_frame.dtx)
                 {
                   gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_DTX_SCH_DTX];
                 }
                 else /* FCH, DCCH are active and SCH is DTXed */
                 {
                   gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_ACTIVE_SCH_DTX];
                 }
                 txc_pch_gain = gain_ptr->gain.pch_gain;
                 txc_fch_gain = gain_ptr->gain.fch_gain;
                 txc_dcch_gain = gain_ptr->gain.dcch_gain;
                 txc_sch_gain = gain_ptr->gain.sch_gain;
                 txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;

              }
              else if (txc_so.rev_dcch.included)
#else
              if (txc_so.rev_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
              {
                txc_dcch_sch_gain_type *gain_ptr;

                if (txtc.dcch_frame.dtx)
                {
                  gain_ptr = &txc_dcch_sch_gain_tab[DCCH_DTX_SCH_DTX];
                }
                else
                {
                  gain_ptr = &txc_dcch_sch_gain_tab[DCCH_ACTIVE_SCH_DTX];
                }

                txc_pch_gain = gain_ptr->gain.pch_gain;
                txc_dcch_gain = gain_ptr->gain.dcch_gain;
                txc_sch_gain = gain_ptr->gain.sch_gain;
                txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;

              }
              else
#endif /* FEATURE_IS2000_P2 */
              {
                 txc_pch_gain = txc_dtx_gain_tab[ txtc.rate ].pch_gain;
                 txc_fch_gain = txc_dtx_gain_tab[ txtc.rate ].fch_gain;
                 txc_sch_gain = 0;
                 txtc.tx_rate_adj = txc_dtx_pch_adj_gain_tab[ txtc.rate ] +
                                    txtc.offset_pwr;

              }
            }
          }
          else
#endif /* FEATURE_IS2000_R_SCH */
          {
#ifdef FEATURE_IS2000_P2
#ifdef FEATURE_IS2000_REL_A_SVD
            if (txc_so.rev_dcch.included && txc_so.rev_fch.included)
            {
               txc_fch_dcch_sch_gain_type *gain_ptr;
               /* FCH is active, DCCH is DTXed and SCH is off*/
               if (txtc.dcch_frame.dtx)
               {
                 gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_DTX_SCH_OFF];
               }
               else /* FCH, DCCH are active and SCH is OFF*/
               {
                 gain_ptr = &txc_fch_dcch_sch_gain_tab[txtc.rate][DCCH_ACTIVE_SCH_OFF];
               }
               txc_pch_gain = gain_ptr->gain.pch_gain;
               txc_dcch_gain = gain_ptr->gain.dcch_gain;
               txc_fch_gain = gain_ptr->gain.fch_gain;
               txc_sch_gain = gain_ptr->gain.sch_gain;
               txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;
            }
            else if (txc_so.rev_dcch.included)
#else
            if (txc_so.rev_dcch.included)
#endif /* FEATURE_IS2000_REL_A_SVD */
            {
              txc_dcch_sch_gain_type *gain_ptr;

              if (txtc.dcch_frame.dtx)
              {
                gain_ptr = &txc_dcch_sch_gain_tab[DCCH_DTX_SCH_OFF];
              }
              else
              {
                gain_ptr = &txc_dcch_sch_gain_tab[DCCH_ACTIVE_SCH_OFF];
              }

              txc_pch_gain = gain_ptr->gain.pch_gain;
              txc_dcch_gain = gain_ptr->gain.dcch_gain;
              txc_sch_gain = gain_ptr->gain.sch_gain;
              txtc.tx_rate_adj = gain_ptr->pch_adj_gain + txtc.offset_pwr;

            }
            else
#endif /* FEATURE_IS2000_P2 */
            {
              txc_pch_gain = txc_pch_fch_gain_tab[ txtc.rate ].pch_gain;
              txc_fch_gain = txc_pch_fch_gain_tab[ txtc.rate ].fch_gain;
              txc_sch_gain = 0;
              txtc.tx_rate_adj = txc_pch_adj_gain_tab[ txtc.rate ] +
                                 txtc.offset_pwr;
            }
          } // else part of if ( txtc.sch_on )
        } // of if ( txc_so.rev_dcch.included || ...
        else
        {
          /*
          ** For RC1 and RC2, fundamental channel gain must be 110.
          */
          txc_pch_gain = 0;
          txc_fch_gain = TXC_IS95A_FCH_GAIN;

          TXC_SET_PCH_GAIN( 0 );
          TXC_SET_FCH_GAIN( TXC_IS95A_FCH_GAIN );
          txtc.tx_rate_adj = txtc.offset_pwr;
          rpc_set_tx_rate_adj (txtc.tx_rate_adj);
        }
      }
    } /* of if ( txc_tx.tx_on ... */

    /* Log power control values for the frame currently transmitted.
    ** This function makes use of txc_rev_fr_block.num_frames to determine
    ** if the NEXT SCH frame is DTX or not.
    */
#if  defined (FEATURE_IS95B_MDR)
    txc_rpctrl_log_frame (txc_rev_fr_block.num_frames);
#ifdef FEATURE_LOG_PC_INFO_2
    txc_log_rpc_info (txc_rev_fr_block.num_frames);
#endif /* FEATURE_LOG_PC_INFO_2 */
#else
    txc_rpctrl_log_frame (0);
#ifdef FEATURE_LOG_PC_INFO_2
    txc_log_rpc_info (0);
#endif /* FEATURE_LOG_PC_INFO_2 */
#endif

#ifdef FEATURE_TX_ADJ_WINDDOWN
    MSM_LOCK();
    /* Read in 10-bit TX_OPEN_LOOP value */

   tx_winddown_open_loop = rfm_get_tx_open_loop(RFCOM_TRANSCEIVER_0);

    /* Read in 8-bit TX_GAIN_ADJ value, units -1/2 dB */
    tx_winddown_gain_adj = (int8) srch_mux_get_tx_adj ();
    MSM_UNLOCK();

    /* convert to 1/2 dB units */
    tx_winddown_open_loop /= -6;

    /* change TX_GAIN_ADJ value to 1/2 dB units */
    tx_winddown_gain_adj *= -1;
    if (tx_winddown_gain_adj == -128)
    {
      tx_winddown_gain_adj = 126;
    }

    if (mc_get_band_class() == CAI_BAND_PCS)
    {
      /* Band Class PCS */
#if defined( FEATURE_NV_HAS_MIN_RX_RSSI )
       tx_winddown_min_pwr = (int8) rfm_get_min_tx_pwr_in_dB(RFCOM_TRANSCEIVER_0) - 3;
#else
       tx_winddown_min_pwr = -55;
#endif
      tx_winddown_gain_adj_floor = tx_winddown_min_pwr * 2 -
                                   (tx_winddown_open_loop - 25 ) +
                                   (txtc.tx_rate_adj / 6) ;
    }
    else
    {
      /* Band Class Cellular */
#if defined( FEATURE_NV_HAS_MIN_RX_RSSI )
       tx_winddown_min_pwr = (int8) rfm_get_min_tx_pwr_in_dB(RFCOM_TRANSCEIVER_0);
#else
       tx_winddown_min_pwr = -52;
#endif
      tx_winddown_gain_adj_floor = tx_winddown_min_pwr * 2 -
                                   (tx_winddown_open_loop - 19 ) +
                                   (txtc.tx_rate_adj / 6) ;
    }

    if (tx_winddown_gain_adj < tx_winddown_gain_adj_floor)
    {
      /* TX_GAIN_ADJ is below the theoretical floor.
      ** Overwrite with floor value
      */
      srch_mux_set_tx_adj ((byte)(-tx_winddown_gain_adj_floor));
    }
#endif // FEATURE_TX_ADJ_WINDDOWN


    //
    // Must program one of MOD_MISC_CTL or TX_PATN register with the
    // correct mode or pattern depending on data rate if we are in FCH
    // 8TH rate gating mode. Hardware does not automatically do this.
    // We program both registers here just to be sure.
    //
    if( txtc.fch_8th_rate_gating )
    {
      if( txtc.rate == ENC_8TH_RATE )
      {
        enc_gating_mode( ENC_FCH_8TH_RATE_GATING_MODE );
        enc_is95c_tx_patn( (word) ENC_FCH_8TH_RATE_GATING_PATTERN );
      }
      else
      {
        //
        // Only one of the 3 gating "modes" (PILOT_GATING, FCH_8TH_RATE and
        // HHO_PREAMBLE can be on at any given time, so it is OK to clear all
        // gating modes if we were in FCH_8TH_RATE "mode" and we are not sending
        // 8th rate frames
        //
        enc_gating_mode( ENC_NO_GATING_MODE );
        enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
      }
    }

#ifdef FEATURE_IS2000_R_SCH
    if ((txtc.sch_on == TRUE) &&
        (
#ifndef FEATURE_IS2000_REL_A
         (txc_so.pri == CAI_SO_PPP_PKT_DATA_3G) &&
#else
         ((txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G) ||
          (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G)) &&
#endif /* FEATURE_IS2000_REL_A */
          (txc_rev_fr_block.num_frames > SCH_BUF_START)
        )
#ifdef FEATURE_IS2000_TDSO
        ||
        (
#ifndef FEATURE_IS2000_REL_A
         ((txc_so.pri == CAI_SO_TDSO) ||
          (txc_so.pri == CAI_SO_FULL_TDSO)) &&
#else
         ((txc_so.pri.so == CAI_SO_TDSO) ||
          (txc_so.sec.so == CAI_SO_TDSO) ||
          (txc_so.pri.so == CAI_SO_FULL_TDSO) ||
          (txc_so.sec.so == CAI_SO_FULL_TDSO)) &&
#endif /* FEATURE_IS2000_REL_A */
         (txc_rev_fr_block.num_frames > 0)
        )
#endif
       )
    {
      enc_sch_data_out_cfg  //does this need to be done every frame?
      (
        txtc.supp_rate,
        txtc.supp_turbo,
        txtc.supp_ltu_size,
        TRUE
      );

      /* Store the frame rate for logging. Next is the one that will be sent in 20ms. */
      next_sch0_frame_rate = LOG_SCH_FULL_RATE;

    }
    else
    {
      if (txtc.sch_on == TRUE)
      {
        (void) parm_xtra_inc(CAI_XTRA_SCH0_REV_SDU_DTX_ID, 1);
      }
      enc_sch_enable( FALSE);

      /* Store the frame rate for logging. Next is the one that will be sent in 20ms. */
      next_sch0_frame_rate = LOG_SCH_BLANK_FRAME;

    }
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_TDSO

#ifndef FEATURE_IS2000_REL_A
    if((txc_so.pri == CAI_SO_TDSO ) ||
            (txc_so.pri == CAI_SO_FULL_TDSO ))
#else
    if((txc_so.pri.so == CAI_SO_TDSO ) ||
          (txc_so.pri.so == CAI_SO_FULL_TDSO ))
#endif /* FEATURE_IS2000_REL_A */
    {

#ifdef FEATURE_IS2000_R_SCH
      if ((txtc.sch_on == TRUE) && (txc_rev_fr_block.num_frames > 0))
      {
#ifdef FEATURE_IS2000_REL_A
        if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
        {
          switch (txtc.supp_rate)
          {
            case ENC_SCH_8TH_RATE:
            case ENC_SCH_QTR_RATE:
            case ENC_SCH_HALF_RATE:
              break;
            case ENC_SCH_FULL_RATE:
              (void) parm_inc(CAI_RSCH0_1_ID, 1);
              break;
            case ENC_SCH_2X_RATE:
              (void) parm_inc(CAI_RSCH0_2_ID, 1);
              break;
            case ENC_SCH_4X_RATE:
              (void) parm_inc(CAI_RSCH0_3_ID, 1);
              break;
            case ENC_SCH_8X_RATE:
              (void) parm_inc(CAI_RSCH0_4_ID, 1);
              break;
            case ENC_SCH_16X_RATE:
              (void) parm_inc(CAI_RSCH0_5_ID, 1);
              break;
            case ENC_SCH_32X_RATE:
              (void) parm_inc(CAI_RSCH0_6_ID, 1);
              break;
            default:
              ERR_FATAL("Bad SCH rate (%d)",txtc.supp_rate,0,0);
              break;
          }
        }
        else
#endif /* FEATURE_IS2000_REL_A */
        {
          switch (txtc.supp_rate)
          {
            case ENC_SCH_8TH_RATE:
            case ENC_SCH_QTR_RATE:
            case ENC_SCH_HALF_RATE:
              break;
            case ENC_SCH_FULL_RATE:
              (void) parm_inc(CAI_SCH0_REV_1X_ID, 1);
              break;
            case ENC_SCH_2X_RATE:
              (void) parm_inc(CAI_SCH0_REV_2X_ID, 1);
              break;
            case ENC_SCH_4X_RATE:
              (void) parm_inc(CAI_SCH0_REV_4X_ID, 1);
              break;
            case ENC_SCH_8X_RATE:
              (void) parm_inc(CAI_SCH0_REV_8X_ID, 1);
              break;
            case ENC_SCH_16X_RATE:
              (void) parm_inc(CAI_SCH0_REV_16X_ID, 1);
              break;
#ifdef FEATURE_IS2000_REL_A
            case ENC_SCH_32X_RATE:
              (void) parm_inc(CAI_SCH0_REV_32X_ID, 1);
              break;
#endif /* FEATURE_IS2000_REL_A */
            default:
              ERR_FATAL("Bad SCH rate (%d)",txtc.supp_rate,0,0);
              break;
          }
        }
      }
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
      for( i=0; i< txc_rev_fr_block.num_frames; i++)
      {
        // rev_frptr =  &sch_tdso_frame_block.frame_type.rev_arr[i].fr_ptr[ i ];
        /* Assuming 1 MuxPDU fits completely in a dsm item */
        if ((!txtc.supp_turbo) && (txtc.supp_rate > ENC_SCH_2X_RATE))
        {
          /*-----------------------------------------------------------------
            Only increment when not turbo and > 2x, as those use LTU's
          -----------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A
          if (txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
          {
            (void) parm_inc(CAI_RSCH0_LTU_ID, 1);
          }
          else
#endif /* FEATURE_IS2000_REL_A */
          {
            (void) parm_inc(CAI_SCH0_REV_LTU_ID, 1);
          }
        }
#ifdef FEATURE_IS2000_REL_A
        if ((txtc.supp_rate == ENC_SCH_FULL_RATE) && (txtc.mux_pdu_type != CAI_MUX_PDU_TYPE_5))
#else
        if (txtc.supp_rate == ENC_SCH_FULL_RATE)
#endif /* FEATURE_IS2000_REL_A */
        {
          /* for 1x, need to have Mixed mode bit pre-appended */
          txtc.supp_frame.data[0] = 0x00;
#ifdef FEATURE_DS_IS2000_RS2
          if (txtc.r_sch_rc == CAI_RC_4)
          {

            b_copy( sch_tdso_frame_block.frame_type.rev_arr[i].fr_item_ptr, 0,
                    txtc.supp_frame.data, 2,
                    MUX2_FULL_PRI_LEN );
          }
          else
#endif /* FEATURE_DS_IS2000_RS2 */
          {
            b_copy( sch_tdso_frame_block.frame_type.rev_arr[i].fr_item_ptr, 0,
                    txtc.supp_frame.data, 1,
                    CAI_FULL_RATE_BITS );
          }
          ENC_SCH_DATA_OUT
          (
             txtc.supp_frame.data,
             34,
             TRUE
          );
        }
        else
        {
          word bits_to_copy;
          word total_byte_to_copy;

          /* Prefix the Mux PDU type 3 header */
          txtc.supp_frame.data[0] = 0x20;
#ifdef FEATURE_IS2000_REL_A
          if ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5) )
          {
            if (txtc.r_sch_rc == CAI_RC_4)
            {
              bits_to_copy = txc_tdso_muxpdu5_rs2[txtc.supp_rate -3][txtc.supp_turbo];
            }
            else
            {
              bits_to_copy = txc_tdso_muxpdu5_rs1[txtc.supp_rate -3][txtc.supp_turbo];
            }
          }
          else
          {
#ifdef FEATURE_DS_IS2000_RS2
            if (txtc.r_sch_rc == CAI_RC_4)
            {
                bits_to_copy = txtc.double_sized ? 538 : 265;
            }
            else
#endif /* FEATURE_DS_IS2000_RS2 */
            {
                bits_to_copy = txtc.double_sized ? 346 : 170;
            }
          }
#else
#ifdef FEATURE_DS_IS2000_RS2
          if (txtc.r_sch_rc == CAI_RC_4)
          {
              bits_to_copy = txtc.double_sized ? 538 : 265;
          }
          else
#endif /* FEATURE_DS_IS2000_RS2 */
          {
              bits_to_copy = txtc.double_sized ? 346 : 170;
          }
#endif /* FEATURE_IS2000_REL_A */

          total_byte_to_copy = ((bits_to_copy+6)/8);

          b_copy( sch_tdso_frame_block.frame_type.rev_arr[i].fr_item_ptr,0,
                  txtc.supp_frame.data,6 ,bits_to_copy );

#ifdef FEATURE_IS2000_REL_A
          if ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5))
          {
            enc_sch_muxpdu5_data_out(txtc.supp_frame.data,total_byte_to_copy);
          }
          else
#endif /*  FEATURE_IS2000_REL_A */
          {
            ENC_SCH_DATA_OUT
            (
              txtc.supp_frame.data,
              total_byte_to_copy ,
              txtc.double_sized
            );
          }
        }
      } /* for loop */

      /*
      ** The IS2000 standard specifies that the "spare bits", remaining after
      ** the maximum number of PDUs for the given rate have been used up to form
      ** the SDU, shall be filled with 0's. Since turbo coded frames do not have
      ** LTU CRCs, this number is a function of the supplemental channel data
      ** rate and the code type. Convolutional = 0, Turbo = 1.
      */
      /* note that, for TDSO MUXPDU 5, there is no spare bit needs to be filled */
#ifdef FEATURE_IS2000_REL_A
      if ((txtc.sch_on == TRUE) && (txc_rev_fr_block.num_frames > 0)
         && (txtc.mux_pdu_type != CAI_MUX_PDU_TYPE_5))
#else
      if ((txtc.sch_on == TRUE) && (txc_rev_fr_block.num_frames > 0))
#endif /* FEATURE_IS2000_REL_A */
      {
        ENC_SCH_DATA_OUT
        (
          txc_sch_spare_bits,
          (txc_num_sch_spare_bits[ txtc.supp_rate ][ txtc.supp_turbo ])/8,
          txtc.double_sized
        );

        /* flush with 16 bits for single sized PDUs */
        if( !txtc.double_sized )
        {
          ENC_SCH_DATA_OUT
          (
            txc_sch_spare_bits,
            2,
            FALSE
          );
        }
      }
      enc_write_data_done();
#endif /* FEATURE_IS2000_R_SCH */
    }
#endif // FEATURE_IS2000_TDSO
#ifdef FEATURE_FACTORY_TESTMODE
  }
#endif // FEATURE_FACTORY_TESTMODE

  if( enc_status() & ENC_STATUS_WRITE_ERROR_M ) /* stomped on encoder data */
  {
    ERR
    (
       "Stomped on encoder data: int %d, status %d",
       txc_tx.int_cnt,
       enc_status(),
       0
    );
  }
} /* txc_tx_frame_proc() */

#ifdef FEATURE_IS2000_P2
/*===========================================================================
FUNCTION     TXC_BUILD_DCCH_FRAME

DESCRIPTION  This function builds a frame to be transmitted on R-DCCH.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void
txc_build_dcch_frame (void)
{
  boolean msg_on_dcch = txc_so.rev_dcch.included &&
                      (txtc.cur_dsch == TXC_REV_DCCH) &&
                      txc_tx.active_msg;

#ifdef FEATURE_IS2000_CHS
  boolean control_hold = txtc.gating_rate != CAI_GATING_RATE_NONE;
#endif // FEATURE_IS2000_CHS

  txtc.dcch_frame.dtx = FALSE;

  /* When sending preambles, or NULLs and no signaling message to transmit,
  ** DCCH can be DTX'd.
  */
  if ( txtc.preambles || ((txtc.nulls
       #ifdef FEATURE_IS2000_CHS
       || control_hold
       #endif
       ) && !msg_on_dcch) )
  {
    txtc.dcch_frame.dtx = TRUE;
    if ( txtc.preambles && txc_hho_preamble_pcg_units )
    {
      // We want num_preambles 1's as the pilot gating pattern
      enc_is95c_tx_patn (PREAMBLES_TO_GATING_PATN( txtc.num_preambles ));
      enc_gating_mode( ENC_HHO_PREAMBLE_MODE );
    }

    if ( !txtc.preambles )
    {
      /* DTX DCCH frame
      */
      txc_dtx_dcch_frame();
    }
  }
  // See if signalling frame is requested
  else if ( msg_on_dcch )
  {
    byte tmpCat;

    /* Send a signalling frame
    */
    (void)memcpy( txtc.dcch_frame.data, zeros, ENC_14400_FULL_RATE_SIZE );

    /* Build the signalling frame buffer and increment frame category counter
    */
    tmpCat = build_blank_frame(TXC_REV_DCCH);
    (void) parm_inc( tmpCat, 1 );
    txtc.dcch_frame.frame_cat = tmpCat;
  }
#ifdef FEATURE_IS2000_REL_A_SVD
  //
  // REMOVE else if{...} after DEBUG
  //
  else if ((txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH) &&
           (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH))
  {
    MSG_ERROR ("Pri + Sec not supp on DCCH", 0,0,0);
  }
#endif // FEATURE_IS2000_REL_A_SVD
  /* no signalling frame on DCCH.  See if RLP has anything
  */
  else if (
           ((prim_dcch_rate == CAI_NULL_RATE) || (prim_dcch_rate == CAI_BLANK_RATE))
#ifdef FEATURE_IS2000_REL_A_SVD
           && (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
#endif // FEATURE_IS2000_REL_A_SVD
          )
  {
    /* Force this to BLANK if DCCH is DTX'd.  Used in RLP logging later.
    */
    txc_rev_fr_block.rev_arr[DCCH_BUF].fr_rate = DSRLP_RATE_BLANK;

    /* DTX DCCH frame
    */
    txc_dtx_dcch_frame();
  }
#ifdef FEATURE_IS2000_REL_A_SVD
  else if (txc_so.pri.phy_channels & MCTRANS_PHY_CHAN_DCCH)
#else
  else
#endif
  {
    //
    // REMOVE if{...} after DEBUG
    //
    if (!prim_dcch_data_ptr)
    {
      /* already checked that prim_dcch_rate was not BLANK/NULL
      ** why is prim_dcch_ptr == NULL??
      */
      ERR_FATAL("Prim_dcch_ptr is NULL", 0,0,0);
    }

    txtc.dcch_frame.data[0] = 0;
    if ( txc_so.rev_dcch.rc == CAI_RC_3 )
    {
      /* RS 1 based
      */
      b_copy( prim_dcch_data_ptr, 0, txtc.dcch_frame.data, 1, CAI_FULL_RATE_BITS );
      (void) parm_inc(CAI_MUX1_REV_DCCH_1_ID, 1); // Increment primary traffic only counter
      txtc.dcch_frame.frame_cat = CAI_MUX1_REV_DCCH_1_ID;
    }
    else
    {
      /* Must be RS 2
      */
      b_copy( prim_dcch_data_ptr, 0, txtc.dcch_frame.data, 2, MUX2_FULL_PRI_LEN );
      (void) parm_inc(CAI_MUX2_REV_DCCH_1_ID, 1); // Increment primary traffic only counter
      txtc.dcch_frame.frame_cat = CAI_MUX2_REV_DCCH_1_ID;
    }
  }
#ifdef FEATURE_IS2000_REL_A_SVD
  /* signaling already sent. If RLP built nothing, DTX.
  */
  else if ((txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH) &&
           (sec_dcch_num_bits == 0))
  {
    //
    // REMOVE if{...} after DEBUG
    //
    if (sec_dcch_data_ptr)
    {
      /* RLP build nothing why is sec_dcch_ptr != NULL??
      */
      ERR_FATAL("Sec_dcch_ptr is not NULL", 0,0,0);
    }

    /* Force this to BLANK if we DTX.  It's used in RLP logging later.
    */
    txc_rev_fr_block.rev_arr[DCCH_BUF].fr_rate = DSRLP_RATE_BLANK;

    /* DTX DCCH frame
    */
    txc_dtx_dcch_frame();
  }
  else if (txc_so.sec.phy_channels & MCTRANS_PHY_CHAN_DCCH)
  {
    //
    // REMOVE if{...} after DEBUG
    //
    if (!sec_dcch_data_ptr)
    {
      /* already handled the case when sec_dcch_num_bits == 0
      ** why is sec_dcch_ptr == NULL??
      */
      ERR_FATAL("Sec_dcch_ptr is NULL", 0,0,0);
    }

    if ( txc_so.rev_dcch.rc == CAI_RC_3 )
    {
      /* RS 1 based
      */
      txtc.dcch_frame.data[0] = 0xF0; // 1111 xxxx
      b_copy (sec_dcch_data_ptr, 0, txtc.dcch_frame.data, 4, sec_dcch_num_bits);
      (void) parm_inc(CAI_MUX1_REV_DCCH_14_ID, 1); // Increment second traffic only counter
      txtc.dcch_frame.frame_cat = CAI_MUX1_REV_DCCH_14_ID;
    }
    else
    {
      /* Must be RS 2
      */
      txtc.dcch_frame.data[0] = 0x5C; // X101 11xx
      b_copy (sec_dcch_data_ptr, 0, txtc.dcch_frame.data, 6, sec_dcch_num_bits);
      (void) parm_inc(CAI_MUX2_REV_DCCH_9_ID, 1); // Increment secondary traffic only counter
      txtc.dcch_frame.frame_cat = CAI_MUX2_REV_DCCH_9_ID;
    }
  }
  else
  {
    /* If DCCH is used ONLY as Signaling channel and
    ** no if in the current no signaling msg is pending, DTX DCCH frame.
    */
    txc_dtx_dcch_frame();
  }
#endif // FEATURE_IS2000_REL_A_SVD

  /* Determine if encoder needs to be disabled to perform DTX.
  ** Normally DTX is done by setting DCCH gain to 0.
  ** Pilot gating is performed by HW only if encoder is disabled.
  */
  if ( txtc.gating_rate != CAI_GATING_RATE_NONE && txtc.dcch_frame.dtx )
  {
    enc_dcch_enable( FALSE );
  }
  else
  {
    enc_dcch_enable( TRUE );
  }
}

/*===========================================================================
FUNCTION     TXC_DTX_DCCH_FRAME

DESCRIPTION  This function does processing when a R-DCCH frame is to be DTX'd.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void
txc_dtx_dcch_frame (void)
{
  txtc.dcch_frame.dtx = TRUE;

  if ( txc_so.rev_dcch.rc == CAI_RC_3 )
  {
    /* RS 1 */
    txtc.dcch_frame.frame_cat = CAI_MUX1_REV_DCCH_15_ID;
  }
  else
  {
    /* RS 2 */
    txtc.dcch_frame.frame_cat = CAI_MUX2_REV_DCCH_27_ID;
  }

  /* Increment DTX counter
  */
  (void) parm_inc(txtc.dcch_frame.frame_cat, 1);
}

#endif /* FEATURE_IS2000_P2 */

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_TRAFFIC_ISR

DESCRIPTION
  Interrupt handler for the traffic channel.  Merge the vocoder and
  signaling data and give them to the encoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
qword qw_frame_off;

void txc_traffic_isr
(
  uint32 arg
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qword ts; /* Used in action time processing, in 20 ms frame units */
  word temp_pch_gain = 0; 
  word temp_fch_gain = 0; 
  word temp_dcch_gain = 0;
  word temp_sch_gain = 0;
  int16 temp_pch_adj_gain = 0;
  byte null_fch_data [3];  /* Store 8th rate NULL frame for both rate sets */

   txtc.last_encoder_interrupt_bin = arg;

  if ( txtc.last_encoder_interrupt_bin == ENC_STATUS_INTERRUPT_SLOT_3 )
  {
    /* This is the 2nd interrupt in a frame.  Use this interrupt to set
    ** gain values and send data.
    */
    switch (txc_build_frame_state)
    {
      /* Frame building for current frame is finished or TXC is starting
      ** from default state. 
      */
      case TXC_BUILD_FRAME_BUILT:
      case TXC_BUILD_FRAME_DONE:
      {
        /* Reset frame building status */
        txc_build_frame_state = TXC_BUILD_FRAME_DONE;
        
        /* Program digital gains with values calculated when the frame was 
        ** built.
        */
      if (
#ifdef FEATURE_IS2000_P2
           ( txc_so.rev_fch.included ) && 
#endif /* FEATURE_IS2000_P2 */
           ( txc_so.rev_fch.rc < CAI_RC_3)
         )
      {
         TXC_SET_PCH_GAIN( 0 );
         TXC_SET_FCH_GAIN( TXC_IS95A_FCH_GAIN );
         TXC_SET_SCH_GAIN( 0 );
         TXC_SET_DCCH_GAIN( 0 );
         rpc_set_tx_rate_adj ( 0 );
      }
      else
      {
         TXC_SET_PCH_GAIN( txc_pch_gain );
         TXC_SET_FCH_GAIN( txc_fch_gain );
         TXC_SET_SCH_GAIN( txc_sch_gain );
         TXC_SET_DCCH_GAIN( txc_dcch_gain );
         rpc_set_tx_rate_adj (txtc.tx_rate_adj);
      }

        /* If DCCH is assigned, output current DCCH frame data to ENC. */
#ifdef FEATURE_IS2000_P2
    if ( txc_so.rev_dcch.included && !txtc.dcch_frame.dtx )
    {
      // Send data out, always generate CRC and always full rate for DCCH.
      // Note: this is called even when sending preambles, but when
      // sending preambles, DCCH is not enabled
      enc_dcch_data_out( txtc.dcch_frame.data, TRUE, ENC_FULL_RATE);
    }

        /* If FCH is assigned, output current FCH frame data to ENC. */
    if ( txc_so.rev_fch.included )
#endif
    {
      enc_fch_data_out( txtc.frame.data, (boolean) !txtc.preambles, txtc.rate);
    }

        /* If SCH is assigned, output current SCH frame data to ENC. */
#if( defined( FEATURE_DS_RLP3 ) && defined( FEATURE_IS2000_R_SCH ) )
#ifndef FEATURE_IS2000_REL_A
    if( txc_so.pri == CAI_SO_PPP_PKT_DATA_3G)
#else
    if( (txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G) ||
        (txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G) )
#endif /* FEATURE_IS2000_REL_A */
    {
        txc_tx_frame_proc_send_data3g(&txc_rev_fr_block);
        }
#endif /* FEATURE_IS2000_R_SCH && FEATURE_DS_RLP3 */
        break;
      }

      case TXC_BUILD_FRAME_IN_PROGRESS:
      case TXC_BUILD_FRAME_LATE:
      {
        /* Frame building for current frame is not yet complete.  This is
        ** unexpected but TXC must handle this situation.  Set build frame
        ** status to LATE.
        */
        txc_build_frame_state = TXC_BUILD_FRAME_LATE;
        
        /* Program digital gains but do not use the gain values calculated for 
        ** the current frame since the current frame is late.  Always DTX
        ** DTX DCCH and SCH by setting the channel digital gains to 0. 
        */
        temp_sch_gain = 0;
        temp_dcch_gain = 0;

        if ( 
#ifdef FEATURE_IS2000_P2
            ( txc_so.rev_fch.included ) && 
#endif /* FEATURE_IS2000_P2 */
            ( txc_so.rev_fch.rc < CAI_RC_3) 
           )
        {
          /* Set digital gains and TX rate adjust to IS95A/B configuration. */   
          temp_pch_gain = 0;
          temp_fch_gain = TXC_IS95A_FCH_GAIN;
          temp_pch_adj_gain = 0;
        }
        else
        {
          /* Set digital gains and TX rate adjust to IS95C configuration, 
          ** depending on channel assignment.
          */   
#ifdef FEATURE_IS2000_P2
          if ( txc_so.rev_fch.included )
#endif /* FEATURE_IS2000_P2 */
          {
            /* Set digital gains and TX rate adjust for 8th rate FCH frame. */
            temp_pch_gain = txc_pch_fch_gain_tab[ENC_8TH_RATE].pch_gain;
            temp_fch_gain = txc_pch_fch_gain_tab[ENC_8TH_RATE].fch_gain;
          }
#ifdef FEATURE_IS2000_P2
          else
          {
            /* FCH is not assigned and the rest of the channel configuration
            ** can be DTX'ed, so total digital gain is given to PCH.  The 15
            ** bit resolution of PCH gain requires the left shift by 7.
            */
            temp_pch_gain = RPC_TOT_DIG_PWR_255 << 7;
            temp_fch_gain = 0;
          }
#endif /* FEATURE_IS2000_P2 */

          /* Calculate TX_RATE_ADJUST. */
          temp_pch_adj_gain = 
            rpc_get_pch_adj_gain(PCH_GAIN_TRUNCATE_TO_8BITS(temp_pch_gain)) 
              + txtc.offset_pwr;
        }

        /* Output digital gains and TX_RATE_ADJUST. */
        TXC_SET_PCH_GAIN (temp_pch_gain);
        TXC_SET_FCH_GAIN (temp_fch_gain);
        TXC_SET_SCH_GAIN (temp_sch_gain);
        TXC_SET_DCCH_GAIN (temp_dcch_gain);
        rpc_set_tx_rate_adj (temp_pch_adj_gain);

#ifdef FEATURE_IS2000_P2
        /* If FCH is assigned, output null FCH frame data to ENC. */
        if ( txc_so.rev_fch.included )
#endif /* FEATURE_IS2000_P2 */
        {
          if( ENC_RC_TO_RATE_SET(txc_so.rev_fch.rc) == ENC_RATE_14400)
          {
            /* Put null primary traffic (20 bits) into an 1800 bps frame.
            ** Make sure to account for the Erasure bit and MM bit.
            */
            null_fch_data[0] = 0x3F;
            null_fch_data[1] = 0xFF;
            null_fch_data[2] = 0xFC;
          }
          else
          {
            /* Put null primary traffic (16 bits) into an 1200 bps frame.
            ** Rate set 1 does not have an Erasure bit or a MM bit.
            */
            null_fch_data[0] = 0xFF;
            null_fch_data[1] = 0xFF;
          }
          enc_fch_data_out (null_fch_data, TRUE, ENC_8TH_RATE);
        
        } /* end if ( txc_so.rev_fch.included ) */
        break;
      }

      default:
        /* Only get here if the state setting is out of range due to error.
        ** Reset it to the default setting.
        */
        txc_build_frame_state = TXC_BUILD_FRAME_DONE;
        break;

    } /* end switch (txc_build_frame_state) */

    return;
  }

  /* Get here if this is the 1st interrupt in a frame.  This interrupt is
  ** the main TXC interrupt which kicks off frame processing among other
  ** things.
  */

  /* If the Turn off TX for HHO mechanism has been armed, it means
  ** right now is the action time frame to turn off TX for the HHO.
  ** So turn off TX and clear the TX HHO mechanism.
  */
  if (txc_turn_off_tx_for_hho_state == TXC_TURN_OFF_TX_ARMED_S)
  {
    /* Turn off TX at action time frame */
    txc_stop_tx(TRUE); /* HHO flag set to TRUE */
    txc_turn_off_tx_for_hho_state = TXC_TURN_OFF_TX_DEFAULT_S; 
  }

  /* Every frame, give the current time to the transaction
  ** engine.  If there is an action for that frame, the
  ** transaction engine will set TXC_ACTION_TIME_SIG
  ** within the interrupt context.
  **
  ** Use function pointer to call the transaction engine.
  ** This function must execute or TXC will not process any
  ** action.
  **
  ** Also, note that TXC action time handling is being set
  ** to take place in Bin 0, PCG 1 of the frame (for Data calls).
  */
  ts_get_20ms_frame_offset_time( ts );
  if (txc_action_time_notifier_func_ptr != NULL)
  {
    /* Reset the signal. */
    (void) rex_clr_sigs (&tx_tcb, TXC_ACTION_TIME_SIG );
    (*txc_action_time_notifier_func_ptr)(MCTRANS_SERVER_TXC, ts);
  }

  /* So here, check for the signal.  If it is set, call the transaction
  ** engine dispatcher to handle the action.  Action handlers have been
  ** registered to the transaction engine at power up.
  **
  ** Note that there is a plan to move the action time handling to task
  ** context in the future.
  */
  if ((rex_get_sigs(&tx_tcb)) & TXC_ACTION_TIME_SIG)
  {
    mctrans_process_transactions(MCTRANS_SERVER_TXC);
    /* Reset the signal. */
    (void) rex_clr_sigs (&tx_tcb, TXC_ACTION_TIME_SIG );
  }

  /* Start frame processing for the current frame, only if the previous
  ** frame has been sent.  If the previous frame is late, do not build a
  ** new frame so TXC can catch up.  Late frames are the result of TXC being
  ** starved so the build frame process spans more than 1 frame.  It is 
  ** desirable to skip building new frames until TXC catches up, rather than 
  ** sending a continuous group of late frames which BS may not received.  
  ** This is consider error recovery for TXC, not normal operation.
  */
  if (txc_build_frame_state == TXC_BUILD_FRAME_DONE) 
  {
    txc_voc_xchg();                /* Reads vocoder encoder bits */
  }

  txc_tx.int_cnt++;                   /* count tx interrupts */

  /* count "after_fo_handoff_count_isrs" number of encoder interrupt for frame
     recalulation from system time after frame offset handoff. After those isrs,
     just increment frame cnt */

  if( after_fo_handoff_count_isrs > 0 )
  {
      after_fo_handoff_count_isrs--;

      /* Derive frame from timestamp>>22 */
      ts_get ( txc_tx.frame_cnt );

      /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
      ** then divide the 1.25 ms units by 16 to get 20ms units.
      */
      (void) qw_div_by_power_of_2( txc_tx.frame_cnt, txc_tx.frame_cnt, 16 );
      qw_set ( qw_frame_off, 0L, txc_frame_offset);
      qw_sub( txc_tx.frame_cnt, txc_tx.frame_cnt, qw_frame_off );
      (void) qw_div_by_power_of_2( txc_tx.frame_cnt, txc_tx.frame_cnt, 4 );

  }

  /* Increment frame count for correct TX frame count */
  qw_inc( txc_tx.frame_cnt, 1L );

  /* Signal tx task to begin operation. */
  (void)rex_set_sigs ( &tx_tcb, TXC_INT_SIG );

  // For a RC3/4 hard handoff, after sending the preambles, we need to enable the
  // FCH again.
  if( txtc.preambles && txtc.num_preambles == 0 && (
      #ifdef FEATURE_IS2000_P2
      txc_so.rev_dcch.included ||
      #endif // FEATURE_IS2000_P2
      ( txc_so.rev_fch.rc >= CAI_RC_3)) )
  {
    txtc.preambles = FALSE;
    #ifdef FEATURE_IS2000_P2
    if ( txc_so.rev_fch.included )
    #endif // FEATURE_IS2000_P2
    {
      enc_fch_enable( TRUE );
    }
    enc_gating_mode( ENC_NO_GATING_MODE );
    enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
    MSG_HIGH ("Stop PCG preamble", 0, 0, 0);
  }
} /* txc_traffic_isr */
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_STOP_TX

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_stop_tx( boolean hho )
{
  MSG_HIGH( "Stop TXC transmitting at %lx,hho is %d",
            txc_get_time_in_1_25ms_unit(), hho, 0 );

  rfm_tx_stop (RFCOM_TRANSCEIVER_0);           /* Make sure power amp is off */

  /* Zero out all digital gains when turning transmitter off for HHO */
  if ((hho) && (txc_so.rev_fch.rc < CAI_RC_3))
  {
    txc_pch_gain  = 0;
    txc_fch_gain  = 0;
    txc_dcch_gain = 0;
    txc_sch_gain  = 0;
    TXC_SET_PCH_GAIN  (0);
    TXC_SET_FCH_GAIN  (0);
    TXC_SET_DCCH_GAIN (0);
    TXC_SET_SCH_GAIN  (0);
  }


  /* Reset rate adjust to not include any adjustment */
  rpc_set_tx_rate_adj (txtc.offset_pwr);
  txc_tx.tx_on = FALSE;
  /* set up the rxc flag so that ffpc will not update the setpoint */
  rxc_set_tx_in_traffic_on(FALSE);

  /* Events report: Transmitter disabled */
  event_report(EVENT_TRANSMITTER_DISABLED);

  if( txc_tx.active_msg )
  {
    /* we were in the middle of the message - send it again */
    txc_tx.buf.pos = 0;
  }
  txtc.power = srch_mux_get_tx_adj(); /* save power */
  MSG_HIGH( "SavedTxPwr=%d,OffsetPwr=%d",
            txtc.power, txtc.offset_pwr, 0 );

  /* End of call, stop RLP logging */
#if defined(FEATURE_RLP_LOGGING)
  dsrlp_log_tx_send_frames();
#endif
} /* txc_stop_tx */

#ifndef FEATURE_HHO_ENHANCEMENTS
/* <EJECT> */
/*===========================================================================

FUNCTION TXC_SEND_MC_FRAME_SIG

DESCRIPTION
  This function does action time check if txc_action_time_chk_enabled.
  When the action time is up (within the frame before System Action Time),
  it sends MCC_TXC_FRAME_SIG to MC.

  The funtion also signal MC with MCC_TXC_FRAME_SIG when
  txc_second_frm_sig_enabled, so that MC can tune to TF after Xmitter
  is turned off.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

LOCAL void txc_send_mc_frame_sig( void )
{
  qword temp_time;
  word phase;
    /* used for 20ms phase in 4*20ms period or  */
    /* 80ms phase in 64*80ms period             */

  if (txc_action_time_chk_enabled
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */
     )
  { /* First MCC_TXC_FRAME_SIG */
    if ( txc_use_action_time )
    { /* Explicit action time */
      /* Need to check if the action time is up. If yes, send MC
       * MCC_TXC_FRAME_SIG.
       */
      /* Calculate the 20ms phase in 4*20ms. Afterwords, temp_time
       * holds the time in 80ms unit.
       */
      phase = qw_div_by_power_of_2( temp_time, txc_tx.frame_cnt, 2 );

      if ( phase == 0 )
      { /* Now it is at an 80ms boundary */

        /* Calculate the 80ms phase in 64*80ms period */
        phase = qw_div_by_power_of_2( temp_time, temp_time, 6 );

        if ( phase == txc_action_time )
        {
          /* Shut down the transmitter
          ** Note: Shutting down the transmitter here guarantees that the
          ** shutdown will occur 8.75 ms before the next frame boundary.
          ** Shutting down the transmitter before the next frame boundary is
          ** important because the hardware shutdown takes place only at the
          ** frame boundary.  If the next frame boundary is missed, the mobile
          ** will be transmitting while tuning for an entire frame.  If MC
          ** were to initiate the shutdown, 8.75 ms might seem a bit tight
          ** for MC to get scheduled to run, receive the signal from TXC, and
          ** to send a command back to TXC for the shutdown
          */
          if ( txc_action_time_stop_tx_enabled )
          {
            txc_stop_tx( FALSE );    /* This is not HHO, set flag to false */
          }

          /* Action Time is up. Signal MC  */
          (void) rex_set_sigs( &mc_tcb, MCC_TXC_FRAME_SIG);

          txc_action_time_chk_enabled = FALSE;
        } /* End of "phase == txc_action_time" */
      }  /* End of 80ms boundary processing */
    }  /* End of Explicit Action Time Processing */
    else
    { /* Implicit Action Time */

      if ( txc_action_time_stop_tx_enabled )
      {
        txc_stop_tx( FALSE );    /* This is not HHO, set flag to false */
      }

      /* Signal MC immediately for Action Time */
      (void) rex_set_sigs( &mc_tcb, MCC_TXC_FRAME_SIG);

      txc_action_time_chk_enabled = FALSE;
    }
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */
  } /* End of txc_action_time_chk_enabled */
  else if (txc_second_frm_sig_enabled
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */
          )
  { /* Second MCC_TXC_FRAME_SIG */

     /* Signal MC that it is the frame after System Action Time */
    (void) rex_set_sigs( &mc_tcb, MCC_TXC_FRAME_SIG);

    /* Disable txc_second_frm_sig_enabled, so no more frame sig will
     * be sent to MC until next time txc_action_time_chk_enabled is TRUE
     */
    txc_second_frm_sig_enabled = FALSE;
#ifdef FEATURE_GPS_MODE
#error code not present
#endif /* FEATURE_GPS_MODE */
  }  /* End of txc_second_frm_sig_enabled */
} /* txc_send_mc_frame_sig */
#endif /* ! FEATURE_HHO_ENHANCEMENTS */

/* <EJECT> */
/*=========================================================================
FUNCTION TXC_RPCTRL_LOG_FRAME

DESCRIPTION
  This function populates FFPC frame logs.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
txc_rpctrl_log_frame
(
 byte    num_frames
)
{
  unsigned int idx; /* Setpoint array index */
  log_rev_pctrl_frame_type *frm_ptr;
  txc_chan_status_type sch_chan_status  = TXC_SCH_DTX;
  txc_chan_status_type dcch_chan_status = TXC_DCCH_DTX;
  static int16 curr_frame_fch_to_pich = 0,  next_frame_fch_to_pich = 0;
  static int16 curr_frame_dcch_to_pich = 0, next_frame_dcch_to_pich = 0;
  static int16 curr_frame_sch0_to_pich = 0, next_frame_sch0_to_pich = 0;

  frm_ptr = txc_rpctrl_log_frame_init();

  if (frm_ptr != NULL)
  {
    /* Record history */
    frm_ptr->dec_history = rxc_rpc_history;

    /* Record AGC vals */
    frm_ptr->rx_agc = log_rx_agc;
    frm_ptr->tx_pwr = log_tx_pwr;
    frm_ptr->tx_gain_adj = log_tx_gain_adj;

#ifdef FEATURE_IS2000_R_SCH
    if (
        (
#ifndef FEATURE_IS2000_REL_A
         ( txc_so.pri == CAI_SO_PPP_PKT_DATA_3G ) &&
#else
         (( txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G ) ||
          ( txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G )) &&
#endif /* FEATURE_IS2000_REL_A */
         ( num_frames > SCH_BUF_START)
        )
#ifdef FEATURE_IS2000_TDSO
        ||
        (
#ifndef FEATURE_IS2000_REL_A
          (( txc_so.pri == CAI_SO_TDSO ) ||
           ( txc_so.pri == CAI_SO_FULL_TDSO )) &&
#else
          (( txc_so.pri.so == CAI_SO_TDSO ) ||
           ( txc_so.sec.so == CAI_SO_TDSO ) ||
           ( txc_so.pri.so == CAI_SO_FULL_TDSO ) ||
           ( txc_so.sec.so == CAI_SO_FULL_TDSO )) &&
#endif /* FEATURE_IS2000_REL_A */
          ( num_frames > 0 )
        )
#endif
      )
    {
      sch_chan_status = TXC_SCH_ACTIVE;
    }
#endif // FEATURE_IS2000_R_SCH

#ifdef FEATURE_IS2000_P2
    if (txc_so.rev_dcch.included && !txtc.dcch_frame.dtx)
    {
      dcch_chan_status = TXC_DCCH_ACTIVE;
    }
#endif // FEATURE_IS2000_P2

    /* note the FCH_TO_PICH, SCH0_TO_PICH ratios for
     * the frame being currently transmitted.
     */
    curr_frame_fch_to_pich  = next_frame_fch_to_pich;
    curr_frame_dcch_to_pich = next_frame_dcch_to_pich;
    curr_frame_sch0_to_pich = next_frame_sch0_to_pich;

    /* get the FCH_TO_PICH, SCH0_TO_PICH, DCCH_TO_PICH ratios for
     * the frame being built.
     */
    rpc_get_relative_gains (&next_frame_fch_to_pich,
                            &next_frame_dcch_to_pich,
                            &next_frame_sch0_to_pich,
                            txtc.rate, sch_chan_status, dcch_chan_status);

    /* Start recording per channel data */
    idx = 0; /* Initialize setpoint array index */

    if (txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_fch == 1)
    {
      frm_ptr->tx_ch_pwr[idx++].r_fch_pich = curr_frame_fch_to_pich;
    }

    if (txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_sch0 == 1)
    {
      frm_ptr->tx_ch_pwr[idx++].r_sch0_pich = curr_frame_sch0_to_pich;
    }

    if (txc_log_rpctrl_sr_ptr->ch_set_mask.chan_set.r_dcch == 1)
    {
      frm_ptr->tx_ch_pwr[idx++].r_dcch_pich = curr_frame_dcch_to_pich;
    }

    /* TODO: Add other channels here! */

    /* end per channel data */

    /* Update packet length */
    txc_log_rpctrl_buf.hdr.len += txc_log_rpctrl_sr_ptr->len_per_frame;

  } /* If logging enabled */
} /* txc_rpctrl_log_frame */


/* <EJECT> */
#ifdef FEATURE_LOG_PC_INFO_2
/*===========================================================================

FUNCTION TXC_RPC_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated LOG_RPC_INFO_C log packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A LOG_RPC_INFO_C log may be sent.

===========================================================================*/
void txc_rpc_info_send_log(void)
{
  if (txc_rpc_info_log_length != 0)
  {
    /* Will send log if enabled. */
    log_set_length (&txc_rpc_info_log_buf, txc_rpc_info_log_length);
    log_submit((log_type *) &txc_rpc_info_log_buf);

    /* Flag to init start of log */
    txc_rpc_info_log_length = 0;
  }

} /* txc_rpc_info_send_log */


/*===========================================================================

FUNCTION TXC_LOG_RPC_INFO_FRAME_INIT

DESCRIPTION
  This function initializes the LOG_RPC_INFO_C log subrecord, as well as the
  pointer to it and the frame entry, in the log buffer.  If necessary, the log is
  sent and a new one is generated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A LOG_RPC_INFO_C log may be sent.

===========================================================================*/
txc_rpc_info_log_frame_type * txc_log_rpc_info_frame_init(void)
{
  txc_rpc_info_log_frame_type  *frm_ptr = NULL;

  /* If logging is enabled. */
  if (log_status(LOG_RPC_INFO_C))
  {
    /* If we reached accumulation threshold, send it. */
    if (txc_rpc_info_log_length >= TXC_RPC_INFO_BUF_THRESHOLD)
    {
      txc_rpc_info_send_log();
    }

    if (txc_rpc_info_log_length == 0) /* Need to re-init */
    {
      txc_rpc_info_log_length = FPOS(txc_rpc_info_log_type, frame_buf);

      /* Set header info (except length which is set later */
      log_set_code      (&txc_rpc_info_log_buf, LOG_RPC_INFO_C);
      log_set_timestamp (&txc_rpc_info_log_buf);

      /*------------------------------
        Fill in beginning of record
      ------------------------------*/
      /* Frame offset specified by base station */
      txc_rpc_info_log_buf.frame_offset = txc_frame_offset;

      txc_rpc_info_log_buf.band_class = cdma.band_class; /* From MC */

#ifndef FEATURE_IS2000_REL_A
      /* Service Option */
      txc_rpc_info_log_buf.so = txc_so.pri;
#else
      txc_rpc_info_log_buf.so = txc_so.pri.so;
#endif /* FEATURE_IS2000_REL_A */

      /* Pilot gating rate not used until the control hold state is supported */
      txc_rpc_info_log_buf.pilot_gating_rate = 0;

      /* Power Control step size */
      txc_rpc_info_log_buf.step_size = txtc.pwr_ctrl_step_size;

      /* We no longer have a valid sub-record. */
      txc_rpc_info_log_buf.frame_cnt = 0;

    }

    /* Increment frame count */
    txc_rpc_info_log_buf.frame_cnt++;

    /* Init frame pointer by finding the offset into the variable length buf.
    ** TXC_RPC_INFO_LOG_OFFSET is calculated from the length of the log packet.
    */
    frm_ptr = (txc_rpc_info_log_frame_type *)
              &txc_rpc_info_log_buf.frame_buf
                      [TXC_RPC_INFO_LOG_OFFSET(txc_rpc_info_log_length)];

  }
  else
  {
    txc_rpc_info_log_length = 0; /* Flag that this record has stopped */
  }

  return frm_ptr;

} /* txc_log_rpc_info_frame_init */


/*=========================================================================
FUNCTION TXC_LOG_RPC_INFO

DESCRIPTION
  This function populates LOG_RPC_INFO_C frame logs.

DEPENDENCIE
  None.

RETURN VALUE

SIDE EFFECTS
  None.
=========================================================================*/
void
txc_log_rpc_info
(
 byte num_frames
)
{
  txc_rpc_info_log_frame_type *frm_ptr;
  int                          ch_index;
  txc_chan_status_type         sch_chan_status  = TXC_SCH_DTX;
  txc_chan_status_type         dcch_chan_status = TXC_DCCH_DTX;

  /* The global variables for these values have already been updated */
  /* for the next frame, so we need to use saved values from the     */
  /* previous frame.                                                 */
  static int16 curr_frame_fch_to_pich = 0,  next_frame_fch_to_pich = 0;
  static int16 curr_frame_dcch_to_pich = 0, next_frame_dcch_to_pich = 0;
  static int16 curr_frame_sch0_to_pich = 0, next_frame_sch0_to_pich = 0;
  static boolean curr_max_pwr_limited_dtx_sch = FALSE;
  static boolean curr_sch_on = FALSE;

  frm_ptr = txc_log_rpc_info_frame_init();

  if (frm_ptr != NULL)
  {
    /* Record history */
    frm_ptr->dec_history = rxc_rpc_history;

    /* Record AGC vals */
    frm_ptr->rx_agc = log_rx_agc;
    frm_ptr->tx_pwr = log_tx_pwr;
    frm_ptr->tx_gain_adj = log_tx_gain_adj;

    /* Record SCH power limiting state */
#ifdef FEATURE_IS2000_R_SCH
    if (txtc.sch_on)
    {
      if (curr_max_pwr_limited_dtx_sch)
      {
        frm_ptr->txc_max_pwr_limited_dtx_sch = TXC_YES;
      }
      else
      {
        frm_ptr->txc_max_pwr_limited_dtx_sch = TXC_NO;
      }
    }
    else
    {
      frm_ptr->txc_max_pwr_limited_dtx_sch = TXC_NOT_APPLICABLE;
    }
    /* Update for next frame */
    curr_max_pwr_limited_dtx_sch = txc_max_pwr_limited_dtx_sch;
#else
    frm_ptr->txc_max_pwr_limited_dtx_sch = TXC_NOT_APPLICABLE;
#endif
    frm_ptr->cdma_tx_pwr_limit = (uint8)(rfm_get_cdma_tx_pwr_limit(RFCOM_TRANSCEIVER_0) >> 2);

    /* Determine state of SCH0 (Active or DTX) */
#ifdef FEATURE_IS2000_R_SCH
    if (
        (
#ifndef FEATURE_IS2000_REL_A
         ( txc_so.pri == CAI_SO_PPP_PKT_DATA_3G ) &&
#else
         (( txc_so.pri.so == CAI_SO_PPP_PKT_DATA_3G ) ||
          ( txc_so.sec.so == CAI_SO_PPP_PKT_DATA_3G )) &&
#endif /* FEATURE_IS2000_REL_A */
         ( num_frames > SCH_BUF_START)
        )
#ifdef FEATURE_IS2000_TDSO
        ||
        (
#ifndef FEATURE_IS2000_REL_A
          (( txc_so.pri == CAI_SO_TDSO ) ||
           ( txc_so.pri == CAI_SO_FULL_TDSO )) &&
#else
          (( txc_so.pri.so == CAI_SO_TDSO ) ||
           ( txc_so.sec.so == CAI_SO_TDSO ) ||
           ( txc_so.pri.so == CAI_SO_FULL_TDSO ) ||
           ( txc_so.sec.so == CAI_SO_FULL_TDSO )) &&
#endif /* FEATURE_IS2000_REL_A */
          ( num_frames > 0 )
        )
#endif
      )
    {
      sch_chan_status = TXC_SCH_ACTIVE;
    }
#endif // FEATURE_IS2000_R_SCH

    /* Determine state of DCCH (Active or DTX) */
#ifdef FEATURE_IS2000_P2
    if (txc_so.rev_dcch.included && !txtc.dcch_frame.dtx)
    {
      dcch_chan_status = TXC_DCCH_ACTIVE;
    }
#endif // FEATURE_IS2000_P2

    /* note the FCH_TO_PICH, SCH0_TO_PICH ratios for
     * the frame being currently transmitted.
     */
    curr_frame_fch_to_pich  = next_frame_fch_to_pich;
    curr_frame_dcch_to_pich = next_frame_dcch_to_pich;
    curr_frame_sch0_to_pich = next_frame_sch0_to_pich;

    /* get the FCH_TO_PICH, SCH0_TO_PICH, DCCH_TO_PICH ratios for
     * the frame being built.
     */
    rpc_get_relative_gains (&next_frame_fch_to_pich,
                            &next_frame_dcch_to_pich,
                            &next_frame_sch0_to_pich,
                            txtc.rate, sch_chan_status, dcch_chan_status);

    /* Start recording per channel data */
    ch_index = 0;

#ifdef FEATURE_IS2000_P2
    if (txc_so.rev_fch.included)
#endif
    {
      frm_ptr->ch_info[ch_index].tx_pwr   = curr_frame_fch_to_pich;
      frm_ptr->ch_info[ch_index].ch_rate  = curr_fch_frame_rate;
      frm_ptr->ch_info[ch_index].ch_type  = TXC_FCH;
      frm_ptr->ch_info[ch_index].fch_type = curr_fch_sig_type;
      frm_ptr->ch_info[ch_index].ch_rc    = txc_so.rev_fch.rc;
      ch_index++;
    }

#ifdef FEATURE_IS2000_R_SCH
    if ( curr_sch_on )
    {
      frm_ptr->ch_info[ch_index].tx_pwr   = curr_frame_sch0_to_pich;
      frm_ptr->ch_info[ch_index].ch_rate  = curr_sch0_frame_rate;
      frm_ptr->ch_info[ch_index].ch_type  = TXC_SCH0;
      frm_ptr->ch_info[ch_index].ch_rc    = txtc.r_sch_rc;
      ch_index++;
    }
    /* Update flag for next frame */
    curr_sch_on = txtc.sch_on;
#endif

#ifdef FEATURE_IS2000_P2
    if ( txc_so.rev_dcch.included )
    {
      frm_ptr->ch_info[ch_index].tx_pwr   = curr_frame_dcch_to_pich;
      frm_ptr->ch_info[ch_index].ch_rate  = txtc.dcch_frame.frame_cat;
      frm_ptr->ch_info[ch_index].ch_type  = TXC_DCCH;
      frm_ptr->ch_info[ch_index].ch_rc    = txc_so.rev_dcch.rc;
      ch_index++;
    }
#endif

    /* TODO: Add other channels here as they become supported! */

    /* Record the number of channels for this frame */
    frm_ptr->ch_count = ch_index;

    /* end per channel data */

    /* Update packet length */
    txc_rpc_info_log_length += FPOS (txc_rpc_info_log_frame_type, ch_info) +
                               (ch_index * sizeof (txc_channel_info));

  } /* If logging enabled */
} /* txc_log_rpc_info */

#endif /* FEATURE_LOG_PC_INFO_2 */

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_TRAFFIC

DESCRIPTION
  Process traffic channel state

DEPENDENCIES
  None
RETURN VALUE
  Next state.
SIDE EFFECTS
  None
===========================================================================*/
txc_state_type txc_traffic
(
  txc_state_type state                  /* txc_state - TXC_TRAFFIC_S */
)
{
  txc_cmd_type           *cmd_ptr;             /* command from queue */
  rex_sigs_type          sigs;                 /* task signals */
  word                   len;                  /* calculate message length */
  word                   flag_var;             /* save flags var*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void)rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

  /* We have an interrupt handler for the traffic channel to take care of */
  /* feeding the actual data to the encoder                              */
  /* now, act on commands on the tx command queue */

  do {

    sigs = rex_get_sigs( &tx_tcb ); /* Get the current signals */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* handle interrupt  */

    if( sigs & TXC_INT_SIG )
    {
      (void)rex_clr_sigs( &tx_tcb, TXC_INT_SIG );

#ifndef FEATURE_HHO_ENHANCEMENTS
      /* Send MC "MCC_TXC_FRAME_SIG" if needed */
      txc_send_mc_frame_sig();

      //
      // This signal is sent one frame interrupt after the one in
      // which the HHO action took place to ensure that the transmitter
      // is really off
      //
      if( txc_mcc_frame_sig )
      {
         (void) rex_set_sigs( &mc_tcb, MCC_TXC_FRAME_SIG);
         txc_mcc_frame_sig = FALSE;
      }
#endif /* ! FEATURE_HHO_ENHANCEMENTS */

#if defined(FEATURE_IS95B_MAHHO) || defined(FEATURE_GPS_MODE)
      txc_next_int_occurred = TRUE;
#endif /* FEATURE_IS95B_MAHHO || FEATURE_GPS_MODE */

      /* Start frame processing for the current frame, only if the previous
      ** frame has been sent.  If the previous frame is late, do not build a
      ** new frame so TXC can catch up.  Late frames are the result of TXC being
      ** starved so the build frame process spans more than 1 frame.  It is 
      ** desirable to skip building new frames until TXC catches up, rather than 
      ** sending a continuous group of late frames which BS may not received.  
      ** This is consider error recovery for TXC, not normal operation.
      */
      if (txc_build_frame_state == TXC_BUILD_FRAME_DONE) 
      {
        txc_build_frame_state = TXC_BUILD_FRAME_IN_PROGRESS;

#ifdef FEATURE_IS2000_REL_A_SVD
      /* process secondary traffic type
      */
      if ( !txtc.preambles )
      {
        txc_sec ();
      }
#endif // FEATURE_IS2000_REL_A_SVD

      /* When the conv enc ISR occurs, it sets TXC_INT_SIG signal and depending on
      ** this signal the tx frame processing starts in the background. If minimize
      ** ISR loading feature is defined then TXC_PRI which runs in ISR context does
      ** only vocoder reading and keeps the voice in a buffer. That voice is muxed
      ** with signalling and written to the convolutional encoder in this function.
      **/

      txc_tx_frame_proc ();

        /* Finish frame processing for transmit task.  As the result, a RL frame is
        ** now ready for transmission.  If the ENC ISR already occured, it's too late
        ** to send data this frame.
        ** In both cases (late or normal), set the state to BUILT.
        */
        if (txc_build_frame_state == TXC_BUILD_FRAME_LATE) 
        {
          MSG_HIGH ("Saw late fr @ end of TX tsk",0,0,0);
        }
        else if (txc_build_frame_state != TXC_BUILD_FRAME_IN_PROGRESS) 
        {
          MSG_ERROR ("Unxpctd build st=%d @ end of TX tsk",txc_build_frame_state,0,0);
        }
        txc_build_frame_state = TXC_BUILD_FRAME_BUILT;
      }
      else if (txc_build_frame_state == TXC_BUILD_FRAME_BUILT) 
      {
        MSG_HIGH ("Prev fr done late, Tx in this fr",0,0,0);
      }
      else
      {
        MSG_ERROR ("Unxpctd build st=%d @ start of TX tsk",txc_build_frame_state,0,0);
      }
    } /* handle interrupt */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    (void) rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG ); /* clear queued signal */

    /* get items off the command queue */
    while ( (cmd_ptr = (txc_cmd_type*) q_get( &txc_cmd_q )) != NULL )
    {

      /* clear the command queued signal - if there is another one we will */
      /* get it on the next interrupt */
      (void)rex_clr_sigs( &tx_tcb, TXC_CMD_Q_SIG );

      /* interpret the command from the queue */
      switch ( cmd_ptr->hdr.command )
      {

        case TXC_MSG_F:            /* send a traffic message */
          MSG_MED ("Got TXC_MSG_F",0,0,0);

          if( !txc_tx.active_msg ) /* get a free buffer */
          {
            txc_tx.buf.cmd_ptr = cmd_ptr; /* save command info */

#ifdef FEATURE_DS
#ifndef FEATURE_IS2000_REL_A
            if ( (txc_so.pri == CAI_SO_NULL) ||
                 DS_ISDATA(txc_so.pri)) /* All data SOs */
#else
            if ( (txc_so.pri.so == CAI_SO_NULL) ||
                 DS_ISDATA(txc_so.pri.so)) /* All data SOs */
#endif /* FEATURE_IS2000_REL_A */
#else
#ifndef FEATURE_IS2000_REL_A
            if (txc_so.pri == CAI_SO_NULL)
#else
            if (txc_so.pri.so == CAI_SO_NULL)
#endif /* FEATURE_IS2000_REL_A */
#endif
            {
              txc_tx.buf.blank = TRUE;   /* blank and burst */
            }
            else {
              txc_tx.buf.blank = FALSE;  /* dim and burst */
            }

            if( txtc.preambles || txtc.nulls ) {
              /* if preambles or null, force B&B */
              txc_tx.buf.blank = TRUE;
            }

            txc_tx.buf.pos = 0;         /* start at the beginning */
            len = cmd_ptr->msg.len;   /* get length  */

            /* Perform a sanity check on the length of the message */
            /* Not that the len is in the units of bits            */
            ASSERT(len <= (CAI_REV_TC_MSG_SIZE  - (CAI_TC_CRC_SIZE + CAI_TC_LGTH_SIZE)));

#ifdef FEATURE_TC_EXTENDED_MSG_LENGTH
#error code not present
#endif /* FEATURE_TC_EXTENDED_MSG_LENGTH */
            {
              //The old way, unchanged since IS_95A.
              txc_tx.buf.msg[ (len>>3 ) + 1 ] = 0; /* pad end with 0's */

              /* copy the actual message */
              b_copy( cmd_ptr->msg.msg_ptr, 0,       /* source */
                &txc_tx.buf.msg[1], 0, len);   /* destination,
                                                  leave room for len */

              len = len+16+8;
              if( len%8 ) {         /* pad length out to byte boundary */
                len = (len & ~7 ) + 8;
              }

              /* convert to bytes */
              txc_tx.buf.msg[0] = ( byte )( len>>3 );
            }

            txc_tx.buf.len = len;

            b_packw(
              crc_16_calc( txc_tx.buf.msg, len-16 ),  /* calculate CRC */
              txc_tx.buf.msg, len-16, 16              /* add to message */
            );
            txtc.dim_delay = 3;
            INTLOCK_SAV (flag_var);
            txc_tx.active_msg = TRUE;
            txc_traffic_signext ();

#ifdef FEATURE_IS2000_REL_B
            /* Grant the confirmation_type requested by rxtx */
            txc_tx.confirmation_type = cmd_ptr->msg.confirmation_type;
#endif /* FEATURE_IS2000_REL_B */

            INTFREE_SAV (flag_var);

          }
          else {
            /* couldn't get a free buffer for the message! */
            txc_cmd_done( cmd_ptr, TXC_ERR_S ); /* dispose of command buffer */
            ERR( "0 tx bufs", 0, 0, 0 );
          }
          break;

        case TXC_NULLS_F:          /* Transmit null traffic data */
          MSG_MED ("Got TXC_NULLS_F",0,0,0);

          txtc.nulls = TRUE;
          txtc.preambles = FALSE;
          //
          // preambles done; turn FCH back on
          //
          #ifdef FEATURE_IS2000_P2
          if( txc_so.rev_fch.included )
          #endif // FEATURE_IS2000_P2
          {
            enc_fch_enable( TRUE );
          }
          txc_cmd_done( cmd_ptr, TXC_DONE_S ); /* dispose of command buffer */
          break;

#ifndef FEATURE_HHO_ENHANCEMENTS
        case TXC_ON_TX_F:          /* Turn transmit power on */
          MSG_MED ("Got TXC_ON_TX_F",0,0,0);

#ifdef FEATURE_TX_POWER_BACK_OFF
          /* Channel configuration has changed, we need to run the RF PA backoff algorithm
          to back off the power if necessary */
          txc_update_chan_config_to_rf(TRUE);
#endif /* FEATURE_TX_POWER_BACK_OFF */
          /* switched the order so PA is turned on AFTER setting GAIN_ADJ to
           ** saved value */
          srch_mux_set_tx_adj( txtc.power );  /* power from access */
          rfm_tx_start(RFCOM_TRANSCEIVER_0);          /* Make sure power amp is on */
          txc_tx.tx_on = TRUE;
          /* set up the rxc flag so that ffpc will update the setpoint */
          rxc_set_tx_in_traffic_on(TRUE);

          /* Events report: Transmitter enabled */
          event_report(EVENT_TRANSMITTER_ENABLED);

          txc_tx.tx_blank = TRUE;      /* expect one blank frame */
          txc_cmd_done( cmd_ptr, TXC_DONE_S ); /* dispose of command buffer */
          break;

        case TXC_OFF_TX_F:         /* Turn transmit power off */
          MSG_MED ("Got TXC_OFF_TX_F",0,0,0);
          txc_stop_tx( FALSE );    /* This is not HHO, set flag to false */
          txc_cmd_done( cmd_ptr, TXC_DONE_S ); /* dispose of command buffer */
          break;
#endif

        case TXC_SO_F:
          MSG_MED ("Got TXC_SO_F",0,0,0);
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;

        case TXC_EXIT_F:
          MSG_MED ("Got TXC_EXIT_F",0,0,0);

          /* ----------------------------------------------------
          ** Turn off the SCH again in case a SCH transaction start
          ** arrives after we process TXC_RELEASE_F.
          ** ---------------------------------------------------- */
          txtc.sch_on = FALSE;

          /* set interrupt handler to null and turn off TX clocks */
          txc_exit();
          enc_set_frame_off(0,0);
          txc_set_frame_offset(0);
          txc_cmd_done( cmd_ptr, TXC_DONE_S ); /* dispose of command buffer */
          state = TXC_EXIT_S;
          break;

        case TXC_CDMA_F:
          MSG_MED ("Got TXC_CDMA_F",0,0,0);

          /* --------------------------------------------------
          ** Transition back to CDMA init state without exiting
          ** txc_subtask()
          ** -------------------------------------------------- */

          /* ----------------------------------------------------
          ** Turn off the SCH again in case a SCH transaction start
          ** arrives after we process TXC_RELEASE_F.
          ** ---------------------------------------------------- */
          txtc.sch_on = FALSE;

          /* ----------------------------------------------------
          ** Set interrupt handler to null and turn off TX clocks
          ** ---------------------------------------------------- */
          txc_exit();
          enc_set_frame_off(0,0);
          txc_set_frame_offset(0);
          /* -------------------------
          ** Dispose of command buffer
          ** ------------------------- */
          txc_cmd_done( cmd_ptr, TXC_DONE_S );

          /* ---------------------------
          ** Initialize encoder for CDMA
          ** --------------------------- */
          txc_cdma_enc_setup();

          /* ------------------------
          ** Switch back to CDMA mode
          ** ------------------------ */
          state = TXC_CDMA_S;
          break;

        case TXC_TC_F:
          MSG_MED ("Got TXC_TC_F",0,0,0);

#ifdef FEATURE_IS2000_P2
          // Default to FCH for signalling
          txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;

          if (cmd_ptr->tc.rev_dcch.included)
          {
            txc_so.rev_dcch.included = cmd_ptr->tc.rev_dcch.included;

            // Before Logical-to-physical mapping is support, just do the default
            // dsch_map specified in IS2000
            if (!cmd_ptr->tc.rev_fch.included)
            {
              txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
            }
            txc_so.rev_dcch.rc = cmd_ptr->tc.rev_dcch.rc;
            enc_dcch_rc( (enc_fch_radio_config_type ) txc_so.rev_dcch.rc );
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
          }
#ifndef FEATURE_PLT
          else
          {
            txc_so.rev_dcch.included = FALSE;
            enc_dcch_enable (FALSE);
          }
#endif /* ! FEATURE_PLT */

          if (cmd_ptr->tc.rev_fch.included)
#endif  /* FEATURE_IS2000_P2 */
          {
#ifdef FEATURE_IS2000_P2
            txc_so.rev_fch.included = cmd_ptr->tc.rev_fch.included;
#endif  /* FEATURE_IS2000_P2 */

            txc_so.rev_fch.rc = cmd_ptr->tc.rev_fch.rc;
            enc_fch_rc((enc_fch_radio_config_type) txc_so.rev_fch.rc );
#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */
          }
#ifndef FEATURE_PLT
#ifdef FEATURE_IS2000_P2
          else
#endif  /* FEATURE_IS2000_P2 */
          {
#ifdef FEATURE_IS2000_P2
            txc_so.rev_fch.included = FALSE;
#endif  /* FEATURE_IS2000_P2 */
            enc_fch_enable (FALSE);
          }
#endif /* ! FEATURE_PLT */

#ifdef FEATURE_PLT
#error code not present
#endif /* FEATURE_PLT */

#ifdef FEATURE_IS2000_REL_A_SVD
          /* Choose either FCH or DCCH as signaling channel.
          ** FCH given priority over DCCH.
          */
#ifdef FEATURE_IS2000_P2
          if (txc_so.rev_dcch.included &&
                   (cmd_ptr->tc.signaling_phy_channel & MCTRANS_PHY_CHAN_DCCH))
          {
            txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
          }

          if (txc_so.rev_fch.included)
#endif  /* FEATURE_IS2000_P2 */
          {
            if (cmd_ptr->tc.signaling_phy_channel & MCTRANS_PHY_CHAN_FCH)
            {
              txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;
            }
          }
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined (FEATURE_PLT)
#error code not present
#endif


          /* Ignore the rest of the cmd since we are in TC state already */

          if (cmd_ptr->tc.tcb_ptr != NULL)
          {
            /* -------------------------------------------------------
            ** Notify requesting task that transition to traffic state
            ** is complete
            ** ------------------------------------------------------- */
            (void) rex_set_sigs( cmd_ptr->tc.tcb_ptr, cmd_ptr->tc.sigs );
          }

          /* Dispose of command buffer */
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_IS2000_R_SCH
        case TXC_FTM_SCH_F:
          MSG_MED ("Got TXC_FTM_SCH_F",0,0,0);

         txtc.sch_on = cmd_ptr->ftm_sch.sch_on;
         if( cmd_ptr->ftm_sch.sch_on )
         {
           byte *tx_pat_ptr;
           int i;

           /* Added the following lines for RC4 R-SCH support. */
           txtc.r_sch_rc = cmd_ptr->ftm_sch.radio_cfg;
           enc_sch_rc ((enc_sch_radio_config_type) txtc.r_sch_rc );

#ifdef FEATURE_DS_IS2000_RS2
           if ((txtc.r_sch_rc == CAI_RC_4) &&
               (cmd_ptr->ftm_sch.sch_turbo == TRUE) &&
               (cmd_ptr->ftm_sch.sch_rate >= ENC_SCH_16X_RATE))
           {
              ERR ("R-SCH config not supp", 0, 0, 0);
           }
#endif
           txtc.supp_rate = cmd_ptr->ftm_sch.sch_rate;
           txc_ftm_rsch_rate = cmd_ptr->ftm_sch.sch_rate;

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
           rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txc_ftm_rsch_rate, txc_so.rev_dcch.included);
#else
           rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txc_ftm_rsch_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

           txtc.supp_turbo = cmd_ptr->ftm_sch.sch_turbo;
           txtc.supp_ltu_size = 0;

           txtc.sch_loopback = FALSE; /* Initially off */

           if (!txtc.sch_loopback)
           {
             tx_pat_ptr = (byte *)&cmd_ptr->ftm_sch.tx_pattern;
#ifdef FEATURE_32X_SCH_DATA_RATE
             for( i = 0; i < ENC_SCH_RC3_32X_SIZE; i++ )
#elif defined (FEATURE_DS_IS2000_RS2)
             for( i = 0; i < ENC_SCH_RC4_16X_SIZE; i++ )
#else
             for( i = 0; i < ENC_SCH_RC3_16X_SIZE; i++ )
#endif /* FEATURE_32X_SCH_DATA_RATE */
             {
               txc_ftm_rsch_data.data[i] = tx_pat_ptr[i%4];
             }
           }
#ifdef FEATURE_TX_POWER_BACK_OFF
           /* Channel configuration has changed, we need to run the RF PA backoff algorithm
           ** to back off the power if necessary.
           */
           txc_update_chan_config_to_rf(TRUE);
#endif
         }
#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
         else
         {
#ifdef FEATURE_IS2000_P2
             rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
             rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, FALSE);
#endif
         }
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

         /* Dispose of command buffer */
         txc_cmd_done( cmd_ptr, TXC_DONE_S );
         break;
#endif /*FEATURE_IS2000_R_SCH*/

        case TXC_FTM_FCH_LOOPBACK:
          MSG_MED ("Got TXC_FTM_FCH_LOOPBACK",0,0,0);
          txtc.fch_loopback = cmd_ptr->loopback.loopback_on;
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;
#ifdef FEATURE_IS2000_P2
        /* Support FEATURE_FTM_DCCH */
        case TXC_FTM_DCCH_LOOPBACK:
          MSG_MED ("Got TXC_FTM_DCCH_LOOPBACK",0,0,0);
          txtc.dcch_loopback = cmd_ptr->loopback.loopback_on;
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;
        /* End of FEATURE_FTM_DCCH */
#endif

        case TXC_FTM_SCH_LOOPBACK:
          MSG_MED ("Got TXC_FTM_SCH_LOOPBACK",0,0,0);
          txtc.sch_loopback = cmd_ptr->loopback.loopback_on;
          txc_cmd_done( cmd_ptr, TXC_DONE_S );
          break;
#endif // FEATURE_FACTORY_TESTMODE

      case TXC_CONNECT_SO_F:
        MSG_MED ("Got TXC_CONNECT_SO_F",0,0,0);

#ifndef FEATURE_IS2000_REL_A
         txc_connect_priso = TRUE;

         if( txc_so.pri != txc_so.stored_pri )
         {
            txc_so.pri = txc_so.stored_pri;
            if
            (
              (txc_so.pri == CAI_SO_LOOPBACK) ||
              (txc_so.pri == CAI_SO_LOOPBACK_13K) ||
              (txc_so.pri == CAI_SO_LOOPBACK_SO55)
            )
            {
              /* For loop back calls, we want to immediately Tx the frame that
              ** we just Rx'ed.  The slot0 interrupt needs to happen late
              ** enough into the frame to ensure that the Rx frame data has 
              ** been extracted and made available to the Tx subsystem.  
              ** Currently, the loopback calls are the only calls requiring a 
              ** slight delay in building the frame, it uses PCG 3.  
              ** All the other service options use PCG 1 for the slot0 
              **interrupt.
              */
              txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
              txc_slot0_isr_pcg = TXC_SLOT0_ISR_LB_PCG_NUM;
              (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg, 
                                                 txc_traffic_isr, 
                                                 ENC_STATUS_INTERRUPT_SLOT_0, 
                                                 TXMOD_INT_MUX_CLT);
              (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM, 
                                                 txc_traffic_isr, 
                                                 ENC_STATUS_INTERRUPT_SLOT_3, 
                                                 TXMOD_INT_MUX_CLT);
            }
            txtc.preambles = FALSE;
            //
            // preambles done; turn FCH back on
            //
            #ifdef FEATURE_IS2000_P2
            if ( txc_so.rev_fch.included )
            #endif // FEATURE_IS2000_P2
            {
              enc_fch_enable( TRUE );
            }
            txtc.nulls = FALSE;
         }
#else
         /* Check for the primary traffic type */

         if (txc_srid_ref[cmd_ptr->conn_so.sr_id].traffic_type == CAI_PRI_TRAF)
         {
           txc_connect_priso = TRUE;
           if ((txc_so.pri.so != txc_srid_ref[cmd_ptr->conn_so.sr_id].so) ||
               (txc_so.pri.phy_channels !=
                  txc_srid_ref[cmd_ptr->conn_so.sr_id].phy_channels))
           {
             txc_so.pri.so = txc_srid_ref[cmd_ptr->conn_so.sr_id].so;
             txc_so.pri.phy_channels = txc_srid_ref[cmd_ptr->conn_so.sr_id].phy_channels;

             if
             (
               (txc_so.pri.so == CAI_SO_LOOPBACK) ||
               (txc_so.pri.so == CAI_SO_LOOPBACK_13K) ||
               (txc_so.pri.so == CAI_SO_LOOPBACK_SO55)
             )
             {
               /* For loop back calls, we want to immediately Tx the frame that
               ** we just Rx'ed.  The slot0 interrupt needs to happen late
               ** enough into the frame to ensure that the Rx frame data has 
               ** been extracted and made available to the Tx subsystem.  
               ** Currently, the loopback calls are the only calls requiring a 
               ** slight delay in building the frame, it uses PCG 3.  
               ** All the other service options use PCG 1 for the slot0 
               **interrupt.
               */
               txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
               txc_slot0_isr_pcg = TXC_SLOT0_ISR_LB_PCG_NUM;
               (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg, 
                                                  txc_traffic_isr, 
                                                  ENC_STATUS_INTERRUPT_SLOT_0, 
                                                  TXMOD_INT_MUX_CLT);
               (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM, 
                                                  txc_traffic_isr, 
                                                  ENC_STATUS_INTERRUPT_SLOT_3, 
                                                  TXMOD_INT_MUX_CLT);
             }
             txtc.preambles = FALSE;
             //
             // preambles done; turn FCH back on
             //
             #ifdef FEATURE_IS2000_P2
             if ( txc_so.rev_fch.included )
             #endif // FEATURE_IS2000_P2
             {
               enc_fch_enable( TRUE );
             }
             txtc.nulls = FALSE;
           }
         }
#ifdef FEATURE_IS2000_REL_A_SVD
         /* Check for the secondary traffic type */
         if (txc_srid_ref[cmd_ptr->conn_so.sr_id].traffic_type == CAI_SEC_TRAF)
         {
           txc_connect_secso = TRUE;
           if ((txc_so.sec.so != txc_srid_ref[cmd_ptr->conn_so.sr_id].so) ||
               (txc_so.sec.phy_channels !=
                  txc_srid_ref[cmd_ptr->conn_so.sr_id].phy_channels))
           {
             txc_so.sec.so = txc_srid_ref[cmd_ptr->conn_so.sr_id].so;
             txc_so.sec.phy_channels =
                  txc_srid_ref[cmd_ptr->conn_so.sr_id].phy_channels;
             txtc.preambles = FALSE;
             //
             // preambles done; turn FCH back on
             //
             #ifdef FEATURE_IS2000_P2
             if ( txc_so.rev_fch.included )
             #endif // FEATURE_IS2000_P2
             {
               enc_fch_enable( TRUE );
             }
             txtc.nulls = FALSE;
           }

         }
#endif /* end of FEATURE_IS2000_REL_A_SVD */

#endif  /* FEATURE_IS2000_REL_A */

         /* Dispose of command buffer */
         txc_cmd_done( cmd_ptr, TXC_DONE_S );

         break;

#ifdef FEATURE_IS2000_REL_A_SVD
         /* new case in SVD feature */
        case TXC_RELEASE_SO_F:
          MSG_MED ("Got TXC_RELEASE_SO_F",0,0,0);

         /*look into the service option reference data, check for release primary
         service */
         if (txc_srid_ref[cmd_ptr->release_so.sr_id].traffic_type == CAI_PRI_TRAF)
         {
           txc_connect_priso = FALSE;
           txc_so.pri.so = CAI_SO_NULL;
           txc_so.pri.phy_channels = 0;  /*none of the channel has been assigned */

           prim_rate          = CAI_BLANK_RATE;
           prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
           prim_dcch_rate     = CAI_NULL_RATE;
           prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
           MSG_MED ("Rel Pri SO", 0, 0, 0);
         }
         /* look into the service option reference data, check for
         release secondary service */
         if (txc_srid_ref[cmd_ptr->release_so.sr_id].traffic_type == CAI_SEC_TRAF)
         {
           txc_connect_secso = FALSE;
           txc_so.sec.so = CAI_SO_NULL;
           txc_so.sec.phy_channels = 0; /* none of the channel has been assigned */

           sec_fch_num_bits   = 0;
           sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
           sec_dcch_num_bits  = 0;
           sec_dcch_data_ptr  = NULL;
#endif /* FEATURE_IS2000_P2 */

           MSG_MED ("Rel Sec SO", 0, 0, 0);

          /* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control
          ** frames when RLP is the secondary SO in a SVD call.  The purpose of the
          ** implementation is to improve voice quality in a SVD call.
          */
          txc_rlp_sig_priority.prev_fr_val = DSRLP_TX_NONE;
          txc_rlp_sig_priority.curr_fr_val = DSRLP_TX_NONE;
          txc_rlp_sig_priority.delay_fr_cnt = 0;

         }
         /* Dispose of command buffer */
         txc_cmd_done( cmd_ptr, TXC_DONE_S );

         break;

#endif /* FEATURE_IS2000_REL_A_SVD */

        case TXC_RELEASE_F:
          MSG_MED ("Got TXC_RELEASE_F",0,0,0);

#ifndef FEATURE_IS2000_REL_A
        txc_connect_priso = FALSE;
        //
        // Do all the other stuff that was being done under TXC_SO_F
        // with SO_NULL
        //
        txc_so.pri = CAI_SO_NULL;
#else
        txc_connect_priso = FALSE;
        txc_so.pri.so = CAI_SO_NULL;
        txc_so.pri.phy_channels = 0;
        txc_so.signaling_phy_channel = 0;

        prim_rate          = CAI_BLANK_RATE;
        prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
        prim_dcch_rate     = CAI_NULL_RATE;
        prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
        txc_connect_secso = FALSE;
        txc_so.sec.so = CAI_SO_NULL;
        txc_so.sec.phy_channels = 0;

        sec_fch_num_bits   = 0;
        sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
        sec_dcch_num_bits  = 0;
        sec_dcch_data_ptr  = NULL;
#endif /* FEATURE_IS2000_P2 */

        /* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control
        ** frames when RLP is the secondary SO in a SVD call.  The purpose of the
        ** implementation is to improve voice quality in a SVD call.
        */
        txc_rlp_sig_priority.prev_fr_val = DSRLP_TX_NONE;
        txc_rlp_sig_priority.curr_fr_val = DSRLP_TX_NONE;
        txc_rlp_sig_priority.delay_fr_cnt = 0;

        MSG_MED ("Rel Pri & Sec SO", 0, 0, 0);
#endif

#endif /* FEATURE_IS2000_REL_A */
        txtc.preambles = FALSE;
        txtc.nulls = TRUE;
#ifdef FEATURE_FACTORY_TESTMODE
        if(ftm_mode == FTM_MODE)
        {
          enc_set_frame_off( 0, 0 );
          txc_stop_tx(FALSE);  // Not HHO, set flag to false
        }
#endif // FEATURE_FACTORY_TESTMODE

        txtc.sch_on = FALSE;

#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
        rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
        rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, FALSE);
#endif /* FEATURE_IS2000_P2 */
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

        /* Call is releasing, reset DTX duration checking */
        txtc.txc_dtx_off = FALSE;
        txtc.dtx_duration_included = FALSE;
#endif // FEATURE_IS2000_R_SCH

        /* Dispose of command buffer */
        txc_cmd_done( cmd_ptr, TXC_DONE_S );

        /* End of call.  Terminate RLP frames log. */
#if defined(FEATURE_RLP_LOGGING)
        dsrlp_log_tx_send_frames();
#endif

        if (log_status(LOG_REV_PCTRL_C))
        {
          txc_rpctrl_send_log();
        }
#ifdef FEATURE_LOG_PC_INFO_2
        if (log_status(LOG_RPC_INFO_C))
        {
          txc_rpc_info_send_log();
        }
#endif /* FEATURE_LOG_PC_INFO_2 */
        break;

        case TXC_HHO_SUCCESS_F:
          MSG_MED ("Got TXC_HHO_SUCCESS_F",0,0,0);

        txc_hho( txc_stored_num_preambles, txc_stored_nom_pwr );

        /* Dispose of command buffer */
        txc_cmd_done( cmd_ptr, TXC_DONE_S );

        break;

#ifndef FEATURE_HHO_ENHANCEMENTS
#ifdef FEATURE_IS95B_MAHHO
      case TXC_RETURNED_F:
        MSG_MED ("Got TXC_RETURNED_F",0,0,0);

        if( cmd_ptr->ret.scr_incl )
        {
           txc_scr_action( cmd_ptr->ret.scr_info );
        }

        if( cmd_ptr->ret.lcm_change_ind )
        {
           qw_equ( txc_tx.mask, cmd_ptr->ret.lcm );
          enc_user_pn_mask( txc_tx.mask ); /* load user PN mask  */
        }

#ifdef FEATURE_IS2000_REL_A
        txtc.p_rev_in_use = cmd_ptr->ret.p_rev_in_use;
#endif

        if
        (
           cmd_ptr->ret.frame_offset_incl &&
           cmd_ptr->ret.frame_offset != txc_frame_offset
        )
        {
           txc_frame_offset_cmd.frame_offset = cmd_ptr->ret.frame_offset;
           /* what about the action frame? */
           txc_frame_offset = txc_frame_offset_cmd.frame_offset;
           rxc_set_frame_offset( txc_frame_offset_cmd.frame_offset );

           enc_set_frame_off( txc_frame_offset, txc_frame_offset );
        }

        if( cmd_ptr->ret.pwr_ctrl_step_incl )
        {
           rfm_tx_gain_adj_step_size( RFCOM_TRANSCEIVER_0,cmd_ptr->ret.pwr_ctrl_step );
        }

#ifdef FEATURE_FACTORY_TESTMODE
        if(ftm_mode == DMSS_MODE)
#endif
        {
          if( cmd_ptr->ret.rev_fch_gating_on )
          {
             txtc.fch_8th_rate_gating = TRUE;
             MSG_HIGH( "FCH 1/8R gating on", 0, 0, 0 );
          }
          else
          {

                // 1/8 rate gating off, to turn it off in encoder
                enc_gating_mode( ENC_NO_GATING_MODE );
                enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
             txtc.fch_8th_rate_gating = FALSE;
             MSG_HIGH( "FCH NOT 1/8R gated", 0, 0, 0 );
          }

          /* R-FCH gating event has a 1 byte boolean parameter */
          event_report_payload(EVENT_REV_FCH_GATING_IN_USE,
                               sizeof(byte),
                               (void *) &txtc.fch_8th_rate_gating);

          rpc_fch_gating_cmd (cmd_ptr->ret.rev_fch_gating_on);
        }

        /* Dispose of command buffer */
        txc_cmd_done( cmd_ptr, TXC_DONE_S );

        break;
#endif /* MAHHO */
#endif /* FEATURE_HHO_ENHANCEMENTS */

#ifdef FEATURE_PLT
#error code not present
#endif // FEATURE_PLT

        default:                /* bad command */
          ERR( "Bad TC cmd %d", cmd_ptr->hdr.command, 0, 0 );
          /* dispose of command buffer */
          txc_cmd_done( cmd_ptr, TXC_BADCMD_S );
          break;

      }  /* switch */
    } /* Item off of queue */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* wait for command on the queue or an interrupt */
    if( state == TXC_TRAFFIC_S ) {
      sigs = txc_wait( TXC_CMD_Q_SIG | TXC_INT_SIG | TXC_10MS_TICK_SIG);
    }

  } while ( state == TXC_TRAFFIC_S ); /* until done with this state */

  txc_clr_msgs();                  /* clear any pending messages to send */

  txc_data_rate = CAI_BLANK_RATE;

  return (state);

} /* end txc_traffic() */

#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================
FUNCTION       TXC_INFORM_SCH_CHNG

DESCRIPTION    Called by data when the SCH config has changed.  Sets a flag
               which tells txc to calculate the throttling params.  Done
               this way since some vars in txc and rxc (used to calc the
               throttling numbers) don't change immediately.

DEPENDENCIES   None

SIDE EFFECTS   None

RETURN VALUE   None
===========================================================================*/
void txc_inform_sch_chng()
{
  txc_sch_changed = TXC_SCH_CHANGED_DELAY;
}
#endif /*FEATURE_IS2000_R_SCH*/

#ifdef FEATURE_IS2000_R_SCH
/*========================================================================
FUNCTION TXC_REV_SCH_TRANS_CMD

DESCRIPTION
   The reverse SCH transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_rev_sch_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{

   if( pkt_ptr != NULL )
   {
     switch ( pkt_ptr->action.txc_r_sch.mode )
     {
       case MCTRANS_R_SCH_START:
       {
         txtc.sch_on = TRUE;
         txtc.r_sch_rc = pkt_ptr->action.txc_r_sch.rc;

         enc_sch_rc ((enc_sch_radio_config_type) txtc.r_sch_rc);
         txtc.supp_rate = (enc_sch_rate_type)
                             ( pkt_ptr->action.txc_r_sch.sch_rate + 3 );

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
         rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,TRUE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
         rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,TRUE, txtc.supp_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

         txtc.supp_turbo = pkt_ptr->action.txc_r_sch.turbo_code;
         txtc.double_sized = pkt_ptr->action.txc_r_sch.double_size_pdu;
         if (( !txtc.supp_turbo ) && (txtc.supp_rate > ENC_SCH_2X_RATE))
         {
           /*----------------------------------------------------------------
              true for any rate greater than 2x, as an LTU is defined as
              the size of a double-size mux pdu + mux header + 16 bit
              CRC
              size = mux hdr + data payload + ltu crc
           ----------------------------------------------------------------*/
#ifndef FEATURE_IS2000_REL_A
#ifdef FEATURE_DS_IS2000_RS2
           if (txtc.r_sch_rc == CAI_RC_4)
           {
             txtc.supp_ltu_size = 6 + 538 + 16;
           }
           else
#endif /* FEATURE_DS_IS2000_RS2 */
           {
             txtc.supp_ltu_size = 6 + 346 + 16;
           }
#else
           txtc.supp_ltu_size = pkt_ptr->action.txc_r_sch.ltu_len;
#endif /* FEATURE_IS2000_REL_A */
         }
         else
         {
           /*----------------------------------------------------------------
              For 1x and 2x, there is no LTU used, as per standard.
              Therefore, for our purposes, the ltu_size is zero
           ----------------------------------------------------------------*/
           txtc.supp_ltu_size = 0;
         }

#ifdef FEATURE_IS2000_REL_A
         txtc.mux_pdu_type = pkt_ptr->action.txc_r_sch.mux_pdu_type;
         txtc.num_ltu = pkt_ptr->action.txc_r_sch.num_ltu;
#endif /* FEATURE_IS2000_REL_A */

         /*------------------------------------------------------------------
           Set the number of PDU's to be built to the max.  DS will call
           a function so that any throttling that is required will take
           place.
         ------------------------------------------------------------------*/
         txtc.pot_num_mux_pdus = 1 << (txtc.supp_rate - ENC_SCH_FULL_RATE);
         /* no double side consept in for MUXPDU 5, but each LTU contains one PDU,
         ** so it seems just like double side in the case MUXPDU 3
         */
#ifdef FEATURE_32X_SCH_DATA_RATE
         /* for 32X, we want to limit the number of potential PDU as 16X case */
         if (txtc.supp_rate == ENC_SCH_32X_RATE)
         {
           txtc.pot_num_mux_pdus = txtc.pot_num_mux_pdus >> 1;
         }
#endif /* FEATURE_32X_SCH_DATA_RATE */
#ifdef FEATURE_IS2000_REL_A
         if ((txtc.double_sized == TRUE) || ((txtc.mux_pdu_type == CAI_MUX_PDU_TYPE_5)
                                             && (txtc.supp_rate > ENC_SCH_FULL_RATE)))
#else
         if (txtc.double_sized == TRUE)
#endif /* FEATURE_IS2000_REL_A */
         {
           txtc.pot_num_mux_pdus = txtc.pot_num_mux_pdus >> 1;
         }
         txtc.ok_num_mux_pdus  = txtc.pot_num_mux_pdus;

         /*------------------------------------------------------------------
           Get the DTX duration.
         ------------------------------------------------------------------*/
         txtc.dtx_duration_org = pkt_ptr->
                                    action.txc_r_sch.rev_sch_dtx_duration;

         /*------------------------------------------------------------------
            DTX duration is not set to infinite by the BS. Turns on the DTX
            duration checking.  A DTX duration of zero means immediately
            send the SCRM, while 1 means send the SCRM after the DTX period.
         ------------------------------------------------------------------*/
         if (txtc.dtx_duration_org != 0x0F)
         {
           txtc.dtx_duration_included = TRUE;
           txtc.dtx_duration = txtc.dtx_duration_org;
         }
         else
         {
           txtc.dtx_duration_included = FALSE;
           txtc.txc_dtx_off = FALSE;
         }

         MSG_HIGH("Start RSCH rate=%d, Turbo=%d #PDUs=%d", txtc.supp_rate,
                    txtc.supp_turbo, txtc.pot_num_mux_pdus);
         break;
       } // of case MCTRANS_R_SCH_START

       case MCTRANS_R_SCH_STOP:
       {
         // Shutdown SCH processing
         txtc.sch_on = FALSE;

#ifdef FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST
#ifdef FEATURE_IS2000_P2
         rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, txc_so.rev_dcch.included);
#else
         rfm_notify_sch_data_rate(RFCOM_TRANSCEIVER_0,FALSE, txtc.supp_rate, FALSE);
#endif
#endif /* FEATURE_P1_DYNAMIC_PA_SWITCH_POINT_ADJUST */

         /* Reset DTX duration checking */
         txtc.txc_dtx_off = FALSE;
         txtc.dtx_duration_included = FALSE;
         break;
       } // of case MCTRANS_R_SCH_STOP

       case MCTRANS_R_SCH_DTX:
       {
         // If SCH processing is on, update DTX duration
         if (txtc.sch_on)
         {
           if (!txtc.dtx_duration_included)
           {
             /*-----------------------------------------------------------------
                DTX duration is not set to infinite by the BS. Turns on the DTX
                duration checking.
             ------------------------------------------------------------------*/
             if (pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration != 0x0F)
             {
               txtc.dtx_duration_included = TRUE;
               txtc.dtx_duration =
                 pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration;
             }
           }
           else
           {
             if (pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration == 0x0F)
             {
               /*-------------------------------------------------------------
                  DTX duration is set to infinite by the BS. Turns off the DTX
                  duration checking.
               ---------------------------------------------------------------*/
               txtc.txc_dtx_off = FALSE;
               txtc.dtx_duration_included = FALSE;
             }
             else if ((txtc.dtx_duration_org - txtc.dtx_duration) >=
                      pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration)
             {
               /*-------------
                  DTX expired.
               ---------------*/
               txtc.dtx_duration = 0;
             }
             else
             {
               /*--------------------
                  Update DTX duration
               ----------------------*/
               txtc.dtx_duration =
                pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration -
                txtc.dtx_duration_org + txtc.dtx_duration;
             }
           } // end if (!txtc.dtx_duration_included)

           /*-----------------------
             Save the DTX duration.
           -------------------------*/
           txtc.dtx_duration_org =
             pkt_ptr->action.txc_r_sch.rev_sch_dtx_duration;
         }
         break;
       } // of case MCTRANS_R_SCH_DTX

     } //lint !e744 : No default needed for this switch
#ifdef FEATURE_TX_POWER_BACK_OFF
     /* Channel configuration has changed, we need to run the RF PA backoff algorithm
     to back off the power if necessary */
     txc_update_chan_config_to_rf(TRUE);
#endif /* FEATURE_TX_POWER_BACK_OFF */

   }

} // txc_rev_sch_trans_cmd
#endif // FEATURE_IS2000_R_SCH

/*========================================================================
FUNCTION TXC_RLGC_TRANS_CMD

DESCRIPTION
   The RLGC transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_rlgc_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
   if( pkt_ptr != NULL )
   {
     /* handle Power Control Step */
     if( pkt_ptr->action.rlgc.pwr_cntl_step.included )
     {
        txtc.pwr_ctrl_step_size = pkt_ptr->action.rlgc.pwr_cntl_step.value;
        rfm_tx_gain_adj_step_size(RFCOM_TRANSCEIVER_0, pkt_ptr->action.rlgc.pwr_cntl_step.value );
     }

     rpc_trans_cmd( pkt_ptr );
   }
}
#ifdef FEATURE_PLCM_ENHANCEMENTS
/*========================================================================
FUNCTION TXC_LCM_TRANS_CMD

DESCRIPTION
   The LCM transaction callback
   LCM means long code mask

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_lcm_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
   if( pkt_ptr != NULL )
   {
      /*load the new long code mask */
      qw_equ(txc_tx.mask, pkt_ptr->action.txc_lcm.long_code_mask);
      enc_user_pn_mask(txc_tx.mask);
   }
   else
   {
      MSG_ERROR( "Null act pkt", 0, 0, 0 );
   }
}
#endif //FEATURE_PLCM_ENHANCEMENTS


/*========================================================================
FUNCTION TXC_HO_TRANS_CMD

DESCRIPTION
   The HO transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_ho_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
   if( pkt_ptr != NULL )
   {
#ifdef FEATURE_IS2000_REL_A
      // For release A, switch P_REV right here and build the next
      // frame with new P_REV.
      txtc.p_rev_in_use = pkt_ptr->action.txc_ho.p_rev_in_use;
#endif /* FEATURE_IS2000_REL_A */

#ifndef FEATURE_PLCM_ENHANCEMENTS
      if( pkt_ptr->action.txc_ho.lcm.included )
      {
         qw_equ( txc_tx.mask, pkt_ptr->action.txc_ho.lcm.value );

         enc_user_pn_mask( txc_tx.mask ); /* load user PN mask  */
      }
#endif /* FEATURE_PLCM_ENHANCEMENTS */

      if( pkt_ptr->action.txc_ho.hho_ind != TRUE )
      {
        if ( pkt_ptr->action.txc_ho.nom_pwr.included )
        {
          //
          // nom_pwr ought not to change for a soft handoff, but still
          // recalculate using the "new" nom_pwr
          //
          byte power;
          /* ------------------------------------------------------------
          ** nom_pwr is already sign extended and, if necessary, adjusted
          ** according to nom_pwr_ext.
          ** ------------------------------------------------------------ */
          power = (byte) pkt_ptr->action.txc_ho.nom_pwr.value;

          /* Invert to turn power adjustment into tx gain adjustment */
          /* power is 5 bit two's complement.  After negation, it is stored
           * as two's complement, but the type is unsigned. The sign is not
           * lost because the value of this variable only uses the lower
           * five bits. */
          power = -(power);//lint !e732 Loss of sign (assignment) (int to unsigned char)

          /* Convert to units of 1/2 dB */
          /* power is never more than 2^5 here. After multiplication, 2^6 */
          power = power * 2;//lint !e734 Loss of precision (assignment) (9 bits to 8 bits)

          /* subtract from current gain adjust in negative 2's complement */
          txtc.power = txtc.power + power;
        }
      }
      else // This is a hard handoff
      {
         MSG_HIGH("HHOtrans,FrOff %d to %d,SavedTxPwr=%d",
                  txc_frame_offset,pkt_ptr->action.txc_ho.frame_offset, txtc.power);

         #ifndef FEATURE_HHO_ENHANCEMENTS
         /* Send MCC_TXC_FRAME_SIG after one more frame */
         txc_mcc_frame_sig = TRUE;
         #endif

         if( pkt_ptr->action.txc_ho.frame_offset != txc_frame_offset )
         {
            /* This is a special corner case where changing the frame offset
               during a HHO to a new band can cause us to not stop TX before 
               RF is retuned. Stop TX now in action time frame - 20ms*/
            txc_stop_tx(TRUE); /* HHO flag set to TRUE */

            txc_frame_offset_cmd.frame_offset =
               pkt_ptr->action.txc_ho.frame_offset;

            /* Modify frame offset */            
            txc_frame_offset = txc_frame_offset_cmd.frame_offset;           
            rxc_set_frame_offset( txc_frame_offset_cmd.frame_offset );
            enc_set_frame_off( txc_frame_offset, txc_frame_offset );
         }
         else
         {
           /* This is action time frame -20ms, do not turn off TX too early, wait until next
           ** frame, which is action time frame.
           */
           txc_turn_off_tx_for_hho_state = TXC_TURN_OFF_TX_ARMED_S; 
         }


         if( pkt_ptr->action.txc_ho.nom_pwr.included )
         {
           txc_stored_nom_pwr = pkt_ptr->action.txc_ho.nom_pwr.value;
           MSG_HIGH( "HHOtrans,NomPwr=%d,NumPre=%d", 
                     txc_stored_nom_pwr,txc_stored_num_preambles,0);
         }
         txc_stored_num_preambles = pkt_ptr->action.txc_ho.num_preamble.value;
         if( pkt_ptr->action.txc_ho.num_preamble.unit == MCTRANS_PREAMBLE_PCG )
         {
            txc_hho_preamble_pcg_units = TRUE;
         }
         else
         {
            txc_hho_preamble_pcg_units = FALSE;
         }

      }
      /* For a return transaction, HO action can carry frame offset
         but the hho_ind value will be set to 0 */
      if( pkt_ptr->action.txc_ho.frame_offset != txc_frame_offset )
      {
         txc_frame_offset_cmd.frame_offset =
           pkt_ptr->action.txc_ho.frame_offset;
         /* what about the action frame? */
         txc_frame_offset = txc_frame_offset_cmd.frame_offset;
         rxc_set_frame_offset( txc_frame_offset_cmd.frame_offset );
         enc_set_frame_off( txc_frame_offset, txc_frame_offset );
      }
   }
}

/*========================================================================
FUNCTION TXC_SCR_TRANS_CMD

DESCRIPTION
   The SCR transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_scr_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
#ifndef FEATURE_IS2000_REL_A
  boolean so_connected;
#endif /* !FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
  word index;
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_P2
  // Default dsch to FCH
  txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;

  // Note the order of checking DCCH and then FCH below cannot be reversed since MSM5000/5105
  // shares the same hardware between DCCH and FCH
  if (pkt_ptr->action.txc_scr.dcch.included)
  {
    txc_so.rev_dcch.included = pkt_ptr->action.txc_scr.dcch.included;

    // Before Logical-to-physical mapping is support, just do the default
    // dsch_map specified in IS2000
    if (!pkt_ptr->action.txc_scr.fch.included)
    {
      txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
    }
    txc_so.rev_dcch.rc = pkt_ptr->action.txc_scr.dcch.rc;
    enc_dcch_rc( (enc_fch_radio_config_type)txc_so.rev_dcch.rc );
    enc_dcch_enable (TRUE);
  }
  else
  {
    txc_so.rev_dcch.included = FALSE;
    enc_dcch_enable (FALSE);
  }
#endif  /* FEATURE_IS2000_P2 */

  if (pkt_ptr->action.txc_scr.fch.included)
  {
#ifdef FEATURE_IS2000_P2
    txc_so.rev_fch.included = pkt_ptr->action.txc_scr.fch.included;
#endif  /* FEATURE_IS2000_P2 */

    txc_so.rev_fch.rc = (cai_radio_config_type) pkt_ptr->action.txc_scr.fch.rc;
    enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
    enc_fch_enable( TRUE );
  }
  else
  {
#ifdef FEATURE_IS2000_P2
    txc_so.rev_fch.included = FALSE;
#endif  /* FEATURE_IS2000_P2 */
    enc_fch_enable (FALSE);
  }

#ifdef FEATURE_IS2000_REL_A_SVD
  /* Choose either FCH or DCCH as signaling channel.
  ** FCH given priority over DCCH.
  */
#ifdef FEATURE_IS2000_P2
  if (txc_so.rev_dcch.included &&
      (pkt_ptr->action.txc_scr.signaling_phy_channel & MCTRANS_PHY_CHAN_DCCH))
  {
    txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
  }

  if (txc_so.rev_fch.included)
#endif  /* FEATURE_IS2000_P2 */
  {
    if (pkt_ptr->action.txc_scr.signaling_phy_channel & MCTRANS_PHY_CHAN_FCH)
    {
      txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;
    }
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_P2
  if (!txc_so.rev_fch.included)
  {
    enc_fch_enable( FALSE );
  }
#endif  // FEATURE_IS2000_P2

#ifdef FEATURE_IS2000_REL_A
  /* copy the SCH MUX OPTION from the upper layer to the MUX for RDCH logging*/
  if (pkt_ptr->action.txc_scr.rev_sch0_mux_option.included)
  {
    txc_so.rev_sch_mux_option = pkt_ptr->action.txc_scr.rev_sch0_mux_option.value;
  }
  else
  {
    txc_so.rev_sch_mux_option = 0;
  }
#endif /* FEATURE_IS2000_REL_A */
  // Just in case we are changing between new and old RCs, make sure rev pilot
  // is turned on/off properly
  txc_pilot_init();
#ifndef FEATURE_IS2000_REL_A
  if
  (
    (pkt_ptr->action.txc_scr.pri_so.included) &&
    (pkt_ptr->action.txc_scr.pri_so.value != txc_so.pri)
  )
  {
    if( txc_connect_priso )
    {
       so_connected = TRUE;
    }
    else
    {
      so_connected = FALSE;
      if( pkt_ptr->action.txc_scr.pri_so.connect_so )
      {
         txc_connect_priso = TRUE;
         so_connected = TRUE;
      }
      else
      {
         txc_so.stored_pri = pkt_ptr->action.txc_scr.pri_so.value;
      }
    }

    if( so_connected )
    {
      txc_so.pri = txc_so.stored_pri =
         pkt_ptr->action.txc_scr.pri_so.value;

      if                                                                 
      (                                                                  
        (txc_so.pri == CAI_SO_LOOPBACK) ||                               
        (txc_so.pri == CAI_SO_LOOPBACK_13K) ||                           
        (txc_so.pri == CAI_SO_LOOPBACK_SO55)                             
      )                                                                  
      {                                                                  
        /* For loop back calls, we want to immediately Tx the frame that 
        ** we just Rx'ed.  The slot0 interrupt needs to happen late      
        ** enough into the frame to ensure that the Rx frame data has    
        ** been extracted and made available to the Tx subsystem.        
        ** Currently, the loopback calls are the only calls requiring a  
        ** slight delay in building the frame, it uses PCG 3.            
        ** All the other service options use PCG 1 for the slot0         
        **interrupt.                                                     
        */                                                               
        txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);                    
        txc_slot0_isr_pcg = TXC_SLOT0_ISR_LB_PCG_NUM;                    
        (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,            
                                           txc_traffic_isr,              
                                           ENC_STATUS_INTERRUPT_SLOT_0,  
                                           TXMOD_INT_MUX_CLT);           
        (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,        
                                           txc_traffic_isr,              
                                           ENC_STATUS_INTERRUPT_SLOT_3,  
                                           TXMOD_INT_MUX_CLT);           
      }                                                                  

      txtc.preambles = FALSE;
      //
      // preambles done; turn FCH back on
      //
      #ifdef FEATURE_IS2000_P2
      if( txc_so.rev_fch.included )
      #endif  // FEATURE_IS2000_P2
      {
        enc_fch_enable( TRUE );
      }
      // If connecting CAI_SO_NULL (removing SO effectively), then we should
      // just send nulls.
      txtc.nulls = (txc_so.pri == CAI_SO_NULL);
    }
  }
#else
  /*check if there is new service */
  if (pkt_ptr->action.txc_scr.num_of_so_recs > 0)
  {

    for(index = 0; index < pkt_ptr->action.txc_scr.num_of_so_recs; index++)
    {
      /* Check for new PRIMARY service option transaction */
      if (pkt_ptr->action.txc_scr.so_cfgs[index].traffic_type == CAI_PRI_TRAF)
      {
        if ((pkt_ptr->action.txc_scr.so_cfgs[index].so != txc_so.pri.so) ||
            (txc_so.pri.phy_channels !=
                      pkt_ptr->action.txc_scr.so_cfgs[index].phy_channel))
        {
          if( pkt_ptr->action.txc_scr.so_cfgs[index].connect_so )
          {
            txc_connect_priso = TRUE;
          }

          if( txc_connect_priso )
          {
            txc_so.pri.so = pkt_ptr->action.txc_scr.so_cfgs[index].so;
            txc_so.pri.phy_channels   =
                      pkt_ptr->action.txc_scr.so_cfgs[index].phy_channel;

            if                                                                 
            (                                                                  
              (txc_so.pri.so == CAI_SO_LOOPBACK) ||                            
              (txc_so.pri.so == CAI_SO_LOOPBACK_13K) ||                        
              (txc_so.pri.so == CAI_SO_LOOPBACK_SO55)                          
            )                                                                  
            {                                                                  
              /* For loop back calls, we want to immediately Tx the frame that 
              ** we just Rx'ed.  The slot0 interrupt needs to happen late      
              ** enough into the frame to ensure that the Rx frame data has    
              ** been extracted and made available to the Tx subsystem.        
              ** Currently, the loopback calls are the only calls requiring a  
              ** slight delay in building the frame, it uses PCG 3.            
              ** All the other service options use PCG 1 for the slot0         
              **interrupt.                                                     
              */                                                               
              txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);                    
              txc_slot0_isr_pcg = TXC_SLOT0_ISR_LB_PCG_NUM;                    
              (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,            
                                                 txc_traffic_isr,              
                                                 ENC_STATUS_INTERRUPT_SLOT_0,  
                                                 TXMOD_INT_MUX_CLT);           
              (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,        
                                                 txc_traffic_isr,              
                                                 ENC_STATUS_INTERRUPT_SLOT_3,  
                                                 TXMOD_INT_MUX_CLT);           
            }                                                                  

            txtc.preambles = FALSE;
            //
            // preambles done; turn FCH back on
            //
            #ifdef FEATURE_IS2000_P2
            if( txc_so.rev_fch.included )
            #endif  // FEATURE_IS2000_P2
            {
              enc_fch_enable( TRUE );
            }
            txtc.nulls = FALSE;
          }
        }
#ifdef FEATURE_IS2000_REL_A_SVD
        /* if PRIMARY SERVICE OPTION is the only service, then reset the secondary SO */
        if (pkt_ptr->action.txc_scr.num_of_so_recs == 1)
        {
          txc_connect_secso = FALSE;
          if (txc_so.sec.so != CAI_SO_NULL)
          {
            MSG_MED ("Rel Sec SO", 0, 0, 0);
          }
          txc_so.sec.so = CAI_SO_NULL;
          txc_so.sec.phy_channels = 0;

          sec_fch_num_bits   = 0;
          sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
          sec_dcch_num_bits  = 0;
          sec_dcch_data_ptr  = NULL;
#endif
        }
#endif /* FEATURE_IS2000_REL_A_SVD */
      } /* end of PRIMARY SERVICE OPTION */

#ifdef FEATURE_IS2000_REL_A_SVD
      /* Check for new SECONDARY service option transaction */

      if (pkt_ptr->action.txc_scr.so_cfgs[index].traffic_type == CAI_SEC_TRAF)
      {
        if ((pkt_ptr->action.txc_scr.so_cfgs[index].so != txc_so.sec.so) ||
            (txc_so.sec.phy_channels !=
                      pkt_ptr->action.txc_scr.so_cfgs[index].phy_channel))
        {
          if( pkt_ptr->action.txc_scr.so_cfgs[index].connect_so )
          {
            txc_connect_secso = TRUE;
          }

          if( txc_connect_secso )
          {
            txc_so.sec.so = pkt_ptr->action.txc_scr.so_cfgs[index].so;
            txc_so.sec.phy_channels   =
                      pkt_ptr->action.txc_scr.so_cfgs[index].phy_channel;
            txtc.preambles = FALSE;
            //
            // preambles done; turn FCH back on
            //
            #ifdef FEATURE_IS2000_P2
            if( txc_so.rev_fch.included )
            #endif  // FEATURE_IS2000_P2
            {
              enc_fch_enable( TRUE );
            }
            txtc.nulls = FALSE;
          }
        }
        /* if SECONDARY SERVICE OPTION is the only service, then reset the primary SO */
        if (pkt_ptr->action.txc_scr.num_of_so_recs == 1)
        {
          txc_connect_priso = FALSE;
          if (txc_so.pri.so != CAI_SO_NULL)
          {
            MSG_MED ("Rel Pri SO", 0, 0, 0);
          }
          txc_so.pri.so = CAI_SO_NULL;
          txc_so.pri.phy_channels = 0;

          prim_rate          = CAI_BLANK_RATE;
          prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
          prim_dcch_rate     = CAI_NULL_RATE;
          prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
        }

      } /* end of SECONDARY SERVICE OPTION */
#endif /* FEATURE_IS2000_REL_A_SVD */

      /* copy the tx service option reference */
      txc_srid_ref[pkt_ptr->action.txc_scr.so_cfgs[index].sr_id].so =
                                       pkt_ptr->action.txc_scr.so_cfgs[index].so;
      txc_srid_ref[pkt_ptr->action.txc_scr.so_cfgs[index].sr_id].phy_channels
                                       = pkt_ptr->action.txc_scr.so_cfgs[index].phy_channel;
      txc_srid_ref[pkt_ptr->action.txc_scr.so_cfgs[index].sr_id].traffic_type
                                       = pkt_ptr->action.txc_scr.so_cfgs[index].traffic_type;

    } /*end of FOR */

  } /* end of number of service bigger than zero */
  else
  {
    // If there is no SO to connect, then we should just send nulls.
    txtc.nulls = TRUE;
  }

  /* copy the signaling physical channels */
  txc_so.signaling_phy_channel = pkt_ptr->action.txc_scr.signaling_phy_channel;

#endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_TX_POWER_BACK_OFF
  // Channel configuration has changed, we need to run the RF PA backoff algorithm
  // to back off the power if necessary
  txc_update_chan_config_to_rf(TRUE);
  #endif // FEATURE_TX_POWER_BACK_OFF
}

/*========================================================================
FUNCTION TXC_GATING_TRANS_CMD

DESCRIPTION
   The FCH 8TH rate gating transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_gating_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
  if( pkt_ptr != NULL )
  {
    if( pkt_ptr->action.txc_gating.rev_fch_gating_on )
    {
      txtc.fch_8th_rate_gating = TRUE;
      MSG_HIGH( "FCH 1/8R gating on", 0, 0, 0 );
    }
    else
    {
      txtc.fch_8th_rate_gating = FALSE;
      enc_gating_mode( ENC_NO_GATING_MODE );
      enc_is95c_tx_patn( (word) ENC_NO_GATING_PATTERN );
      MSG_HIGH( "FCH NOT 1/8R gated", 0, 0, 0 );
    }

    /* R-FCH gating event has a 1 byte boolean parameter */
    event_report_payload(EVENT_REV_FCH_GATING_IN_USE,
                         sizeof(byte),
                         (void *) &txtc.fch_8th_rate_gating);

    rpc_fch_gating_cmd (pkt_ptr->action.txc_gating.rev_fch_gating_on);
  }
  else
  {
    MSG_ERROR( "Null act pkt", 0, 0, 0 );
  }
}

#ifdef FEATURE_IS2000_CHS
/*========================================================================
FUNCTION TXC_CONTROL_HOLD_TRANS_CMD

DESCRIPTION
   Processor for the control hold action.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_control_hold_trans_cmd
(
  mctrans_action_packet_type *pkt_ptr
)
{
  enc_gating_pattern_type gating_pattern;

  if ( pkt_ptr == NULL )
  {
    // If nothing to do, just return
    return;
  }

  // Default to no gating
  gating_pattern = ENC_NO_GATING_PATTERN;

  // Remember gating rate
  txtc.gating_rate = pkt_ptr->action.txc_ctrl_hld_gating.gating_rate;

  if ( txtc.gating_rate == CAI_GATING_RATE_QUARTER )
  {
    gating_pattern = ENC_QTR_RATE_GATING_PATTERN;
  }
  else if (txtc.gating_rate == CAI_GATING_RATE_HALF )
  {
    gating_pattern = ENC_HALF_RATE_GATING_PATTERN;
  }

  // Now turn on/off gating
  enc_gating_mode( ENC_PILOT_GATING_MODE );
  enc_is95c_tx_patn( gating_pattern );

  if ( txtc.gating_rate != CAI_GATING_RATE_NONE )
  {
    MSG_HIGH( "Pilot gating ON:%d", txtc.gating_rate, 0, 0 );
  }
  else
  {
    MSG_HIGH( "Pilot gating OFF", 0, 0, 0 );
  }
}
#endif // FEATURE_IS2000_CHS

#ifdef FEATURE_HHO_ENHANCEMENTS
/*========================================================================
FUNCTION TXC_TX_ON_OFF_TRANS_CMD

DESCRIPTION
   The TX on/off call back

DEPENDENCIES
  None                              

SIDE EFFECTS
  None

RETURN VALUE
  None

========================================================================*/
void txc_tx_on_off_trans_cmd( mctrans_action_packet_type *pkt_ptr )
{
  if (pkt_ptr->action.txc_tx_on_off.tx_on)
  {
    MSG_HIGH("TX on,SavedTxPwr=%d",txtc.power,0,0);
    /* Set power level before turning on TX.
    */
    srch_mux_set_tx_adj( txtc.power );  /* power from access */

    /* Turn on PA
    */
    rfm_tx_start(RFCOM_TRANSCEIVER_0);
    txc_tx.tx_on = TRUE;
    /* set up the rxc flag so that ffpc will update the setpoint */
    rxc_set_tx_in_traffic_on(TRUE);
#ifdef FEATURE_VOIP_1X_HANDOFF
    txtc.ready_to_transmit = TRUE;
#endif /* FEATURE_VOIP_1X_HANDOFF */
    /* Events report: Transmitter enabled */
    event_report(EVENT_TRANSMITTER_ENABLED);

    txc_tx.tx_blank = TRUE;      /* expect one blank frame */

  }
  else
  {
    MSG_HIGH("TX off,not HHO",0,0,0);
    txc_stop_tx( FALSE );    /* This is not HHO, set flag to false */
  }
}
#endif /* FEATURE_HHO_ENHANCEMENTS */


#if defined (FEATURE_IS95B_MAHHO) && !defined(FEATURE_HHO_ENHANCEMENTS)
/*===========================================================================

FUNCTION TXC_SCR_ACTION

DESCRIPTION
  Performs a service option change "action"

DEPENDENCIES
  None.
RETURN VALUE
  Never returns.
SIDE EFFECTS
  None.
===========================================================================*/
static void txc_scr_action( txc_scr_info_type scr_info )
{
  boolean so_connected;

#ifdef FEATURE_IS2000_REL_A
  int index;
#endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_P2
  // Default dsch to to FCH
  txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;

  // Note the order of checking DCCH and then FCH below cannot be reversed since MSM5000/5105
  // shares the same hardware between DCCH and FCH
  if (scr_info.rev_dcch.included)
  {
    txc_so.rev_dcch.included = scr_info.rev_dcch.included;

    // Before Logical-to-physical mapping is support, just do the default
    // dsch_map specified in IS2000
    if (!scr_info.rev_fch.included)
    {
      txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
    }
    txc_so.rev_dcch.rc = scr_info.rev_dcch.rc;
    enc_dcch_rc( (enc_fch_radio_config_type) txc_so.rev_dcch.rc );
    enc_dcch_enable( TRUE );

  }
  else
  {
    txc_so.rev_dcch.included = FALSE;
    enc_dcch_enable (FALSE);
  }
#endif  /* FEATURE_IS2000_P2 */

  if (scr_info.rev_fch.included)
  {
#ifdef FEATURE_IS2000_P2
    txc_so.rev_fch.included = scr_info.rev_fch.included;
#endif  /* FEATURE_IS2000_P2 */

    txc_so.rev_fch.rc = (cai_radio_config_type) scr_info.rev_fch.rc;
    enc_fch_rc( (enc_fch_radio_config_type) txc_so.rev_fch.rc );
    enc_fch_enable( TRUE );
  }
  else
  {
#ifdef FEATURE_IS2000_P2
    txc_so.rev_fch.included = FALSE;
#endif  /* FEATURE_IS2000_P2 */
    enc_fch_enable (FALSE);
  }

#ifdef FEATURE_IS2000_REL_A_SVD
  /* Choose either FCH or DCCH as signaling channel.
  ** FCH given priority over DCCH.
  */
#ifdef FEATURE_IS2000_P2
  if (txc_so.rev_dcch.included &&
      (scr_info.signaling_phy_channel & MCTRANS_PHY_CHAN_DCCH))
  {
    txtc.dsch_map = txtc.cur_dsch = TXC_REV_DCCH;
  }

  if (txc_so.rev_fch.included)
#endif  /* FEATURE_IS2000_P2 */
  {
    if (scr_info.signaling_phy_channel & MCTRANS_PHY_CHAN_FCH)
    {
      txtc.dsch_map = txtc.cur_dsch = TXC_REV_FCH;
    }
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_P2
  if (!txc_so.rev_fch.included)
  {
    enc_fch_enable( FALSE );
  }
#endif  // FEATURE_IS2000_P2

  // Just in case we are changing between new and old RCs, make sure rev pilot
  // is turned on/off properly
  txc_pilot_init();
#ifndef FEATURE_IS2000_REL_A
  if
  (
    (scr_info.change_pri) &&
    (scr_info.pri != txc_so.pri)
  )
  {
    if( txc_connect_priso )
    {
      so_connected = TRUE;
    }
    else
    {
      so_connected = FALSE;
      if( scr_info.connect_so )
      {
         txc_connect_priso = TRUE;
         so_connected = TRUE;
      }
      else
      {
         txc_so.stored_pri = scr_info.pri;
      }
    }

    if( so_connected )
    {
      txc_so.pri = txc_so.stored_pri = scr_info.pri;
      txtc.preambles = FALSE;
      //
      // preambles done; turn FCH back on
      //
      #ifdef FEATURE_IS2000_P2
      if( txc_so.rev_fch.included )
      #endif  // FEATURE_IS2000_P2
      {
        enc_fch_enable( TRUE );
      }
      // If connecting CAI_SO_NULL (removing SO effectively), then we should
      // just send nulls.
      txtc.nulls = (txc_so.pri == CAI_SO_NULL);
    }
  }
#else
  if (scr_info.num_of_so_recs > 0) /* check if there are new so record */
  {
    for (index = 0; index < scr_info.num_of_so_recs; index++)
    {
      /* Check for new PRIMARY service option transaction */
      if (scr_info.so_cfgs[index].traffic_type == CAI_PRI_TRAF)
      {
        if(scr_info.so_cfgs[index].so != txc_so.pri.so)
        {
          if( txc_connect_priso )
          {
            so_connected = TRUE;
          }
          else
          {
            so_connected = FALSE;
            if( scr_info.so_cfgs[index].connect_so )
            {
              txc_connect_priso = TRUE;
              so_connected = TRUE;
            }
          }
          if( so_connected )
          {
            txc_so.pri.so = scr_info.so_cfgs[index].so ;
            txc_so.pri.phy_channels = scr_info.so_cfgs[index].phy_channel;
            txtc.preambles = FALSE;
            //
            // preambles done; turn FCH back on
            //
            #ifdef FEATURE_IS2000_P2
            if( txc_so.rev_fch.included )
            #endif  // FEATURE_IS2000_P2
            {
              enc_fch_enable( TRUE );
            }
            txtc.nulls = FALSE;
          }
          /* srore the service option based on the service reference */
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].so = scr_info.so_cfgs[index].so;
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].phy_channels = scr_info.so_cfgs[index].phy_channel;
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].traffic_type = CAI_PRI_TRAF;
        }
#ifdef FEATURE_IS2000_REL_A_SVD
        /* if PRIMARY SERVICE OPTION is the only service, then reset the secondary SO */
        if (scr_info.num_of_so_recs == 1)
        {
          txc_connect_secso = FALSE;
          if (txc_so.sec.so != CAI_SO_NULL)
          {
            MSG_MED ("Rel Sec SO", 0, 0, 0);
          }
          txc_so.sec.so = CAI_SO_NULL;
          txc_so.sec.phy_channels = 0;

          sec_fch_num_bits   = 0;
          sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
          sec_dcch_num_bits  = 0;
          sec_dcch_data_ptr  = NULL;
#endif /* FEATURE_IS2000_P2 */
        }
#endif /* FEATURE_IS2000_REL_A_SVD */

      }
#ifdef FEATURE_IS2000_REL_A_SVD
      /* check for change request for secondary service option */
      so_connected = FALSE;
      /* Check for new SECONDARY service option transaction */
      if (scr_info.so_cfgs[index].traffic_type == CAI_SEC_TRAF)
      {
        if(scr_info.so_cfgs[index].so != txc_so.sec.so)
        {
          if( txc_connect_secso )
          {
            so_connected = TRUE;
          }
          else
          {
            so_connected = FALSE;
            if( scr_info.so_cfgs[index].connect_so )
            {
              txc_connect_secso = TRUE;
              so_connected = TRUE;
            }
          }
          if( so_connected )
          {
            txc_so.sec.so = scr_info.so_cfgs[index].so ;
            txc_so.sec.phy_channels = scr_info.so_cfgs[index].phy_channel;
            txtc.preambles = FALSE;
            //
            // preambles done; turn FCH back on
            //
            #ifdef FEATURE_IS2000_P2
            if( txc_so.rev_fch.included )
            #endif  // FEATURE_IS2000_P2
            {
              enc_fch_enable( TRUE );
            }
            txtc.nulls = FALSE;
          }
          /* srore the service option based on the service reference */
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].so = scr_info.so_cfgs[index].so;
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].phy_channels = scr_info.so_cfgs[index].phy_channel;
          txc_srid_ref[scr_info.so_cfgs[index].sr_id].traffic_type = CAI_SEC_TRAF;
        }
        /* if SECONDARY SERVICE OPTION is the only service, then reset the primary SO */
        if (scr_info.num_of_so_recs == 1)
        {
          txc_connect_priso = FALSE;
          if (txc_so.pri.so != CAI_SO_NULL)
          {
            MSG_MED ("Rel Pri SO", 0, 0, 0);
          }
          txc_so.pri.so = CAI_SO_NULL;
          txc_so.pri.phy_channels = 0;

          prim_rate          = CAI_BLANK_RATE;
          prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
          prim_dcch_rate     = CAI_NULL_RATE;
          prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */
        }
      }
#endif /* FEATURE_IS2000_REL_A_SVD */
    }
  }
  else
  {
    // If there is no SO to connect, then we should just send nulls.
    txtc.nulls = TRUE;
  }
#endif /* FEATURE_IS2000_REL_A*/

  #ifdef FEATURE_TX_POWER_BACK_OFF
  // Channel configuration has changed, we need to run the RF PA backoff algorithm
  // to back off the power if necessary
  txc_update_chan_config_to_rf(TRUE);
  #endif // FEATURE_TX_POWER_BACK_OFF
} // TXC_SCR_ACTION
#endif /* FEATURE_IS95B_MAHHO && !FEATURE_HHO_ENHANCEMENTS */

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_HHO

DESCRIPTION
  This function is called by MC on a hard handoff boundary to set the
  number of preamble frames to be transmitted and the new nominal power.

DEPENDENCIES
 Relies on the fact that TX_OFF_TX_F has already been sent and TXC is now
 waiting for TX_ON_TX_F.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_hho(
word num_preambles,
word nom_pwr
)
{
  byte power;
  /* ------------------------------------------------------------
  ** nom_pwr is already sign extended and, if necessary, adjusted
  ** according to nom_pwr_ext.
  ** ------------------------------------------------------------ */
  power = (byte) nom_pwr;

  /* Invert to turn power adjustment into tx gain adjustment */
  /* power is 5 bit two's complement.  After negation, it is stored
   * as two's complement, but the type is unsigned. The sign is not
   * lost because the value of this variable only uses the lower
   * five bits. */
  power = -(power); //lint !e732 Loss of sign (assignment) (int to unsigned char)

  /* Convert to units of 1/2 dB */
  /* power is never more than 2^5 here. After multiplication, 2^6 */
  power = power * 2;//lint !e734 Loss of precision (assignment) (9 bits to 8 bits)

  /* subtract from current gain adjust in negative 2's complement */
  txtc.power = txtc.power + power;
  MSG_HIGH ("HHOsuccess,NomPwr=%d,NumPre=%d,TxPwr=%d", 
            nom_pwr,num_preambles,txtc.power);

  if (num_preambles > 0)
  {
    txtc.num_preambles = num_preambles;
    txtc.preambles = TRUE;
  }
} /* txc_hho */


/* <EJECT> */
/*===========================================================================

FUNCTION TXC_WRITE_PCBIT_DATA

DESCRIPTION
  This function is called by RXC to write the PCBIT data.  It must be called
  from RXC context because of timing constraints.  The power control data
  must be written so that it is transmitted two frames after the frame it
  is giving quality information about.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_write_pcbit_data()
{
  word pcbit_data;

  if (
       (
         (txc_so.fpc_mode == FPC_MODE_PRI_EIB)
#ifdef FEATURE_IS2000_REL_A
         || (txc_so.fpc_mode == FPC_MODE_PRI_QIB)
         || (txc_so.fpc_mode == FPC_MODE_PRI_QIB_SEC_EIB)
         || (txc_so.fpc_mode == FPC_MODE_PRI_400Hz_SEC_EIB)
#endif /* FEATURE_IS2000_REL_A */
       ) &&
       // FPC_MODE = EIB implies FCH is active, no need to check below
       ((
         #ifdef FEATURE_IS2000_P2
         txc_so.rev_fch.included &&
         #endif  // FEATURE_IS2000_P2
         (txc_so.rev_fch.rc >= CAI_RC_3))
         #ifdef FEATURE_IS2000_P2
         || txc_so.rev_dcch.included
         #endif  // FEATURE_IS2000_P2
       )
     )
  {
     /*
     ** PC Bit was enabled in MOD_MISC_CTL when
     ** txc_so.fpc_mode was set.
     */

    pcbit_data = ffpc_calculate_pcbit_data
    (
      txc_eib_flag
#ifdef FEATURE_IS2000_REL_A
      ,txc_sch_eib_flag
      ,txc_qib_flag
#endif /* FEATURE_IS2000_REL_A */
    );

    enc_pcbit_test_data (pcbit_data);

  }
}

/*===========================================================================

FUNCTION txc_set_action_time_signal

DESCRIPTION This function sets the TXC_ACTION_TIME_SIG for TXC.  This signal
  indicates that there is an TXC action in this frame which needs to be processed.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void txc_set_action_time_signal(void)
{
  MSG_HIGH("TXC set act time sgnl",0,0,0);
  (void) rex_set_sigs( &tx_tcb, TXC_ACTION_TIME_SIG );
}

/*===========================================================================

FUNCTION txc_custom_interrupt_reg_func

DESCRIPTION This function is called to obtain a pointer to MC's transaction
            manager's function which handles TXC action time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void txc_custom_interrupt_reg_func(
  void (* ticker_func)(mctrans_server_token_type, qword)
)
{
  /* Store ptr to action time function and call it, if it's not NULL,
  ** every frame to process action time.
  */
  //MSG_HIGH("Before INIT txc_at_notifier_func_ptr %d",
  //       txc_action_time_notifier_func_ptr,0,0);

  txc_action_time_notifier_func_ptr = ticker_func;

  //MSG_HIGH("After INIT txc_at_notifier_func_ptr %d",
  //         txc_action_time_notifier_func_ptr,0,0);
} /* end txc_custom_interrupt_reg_func */

/*lint -save -e785 OK to have too few initializers due to ifdef's */
/* Structure to support centralized transaction manager.  This struct contains
** call back functions to set action time signal and to register the timer tick
** used in processing TXC actions, as well as the functions which the MC will
** call to process TXC actions.
*/
mctrans_function_pointer_initializer_type txc_func_pointer_initializers =
{
  txc_set_action_time_signal,
  txc_custom_interrupt_reg_func,
  {
    { MCTRANS_FFPC_ACTION,            NULL},
    { MCTRANS_RLGC_ACTION,            txc_rlgc_trans_cmd},
    { MCTRANS_F_SCH_ACTION,           NULL},

    #ifdef FEATURE_IS2000_R_SCH
    { MCTRANS_R_SCH_ACTION,           txc_rev_sch_trans_cmd},
    #else
    { MCTRANS_R_SCH_ACTION,           NULL},
    #endif

    { MCTRANS_HO_ACTION,              txc_ho_trans_cmd},
    { MCTRANS_SCR_ACTION,             txc_scr_trans_cmd},
    { MCTRANS_CFS_NSET_ACTION,        NULL},
    { MCTRANS_CFS_CTRL_ACTION,        NULL},
    { MCTRANS_GATING_ACTION,          txc_gating_trans_cmd},

    #ifdef FEATURE_IS2000_CHS
    { MCTRANS_CTRL_HLD_GATING_ACTION, txc_control_hold_trans_cmd},
    #else
    { MCTRANS_CTRL_HLD_GATING_ACTION, NULL},
    #endif

    { MCTRANS_ASET_ACTION,            NULL},

    #ifdef FEATURE_PLCM_ENHANCEMENTS
    { MCTRANS_LCM_ACTION,             txc_lcm_trans_cmd},
    #else
    { MCTRANS_LCM_ACTION,             NULL},
    #endif /* FEATURE_PLCM_ENHANCEMENTS */
    #ifdef FEATURE_HHO_ENHANCEMENTS
    { MCTRANS_TX_ON_OFF_ACTION,             txc_tx_on_off_trans_cmd},
    #else
    { MCTRANS_TX_ON_OFF_ACTION,             NULL},
    #endif
    { MCTRANS_NULL_ACTION,            NULL},
    { MCTRANS_MAX_ACTION,             NULL}
  }
};
/*lint -restore */

/*===========================================================================

FUNCTION txc_register_action_handlers_notifier_interrupt_func

DESCRIPTION This function is called at power up to register the following items
            with MC's transaction manager:
      - call back function to set TXC action time signal
      - call back function to register TXC timer tick
            - call back functions which MC will call to process TXC actions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void txc_register_action_handlers_notifier_interrupt_func(void)
{
  MSG_HIGH("reg_act_handlers called",0,0,0);
  mctrans_register_action_notifier_handler_interrupt_reg_func(
                      MCTRANS_SERVER_TXC,
                      txc_func_pointer_initializers);
}

#if defined(FEATURE_VOIP_1X_HANDOFF) || defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
/*===========================================================================

FUNCTION TXC_TT_TC_INIT

DESCRIPTION
  When transition from TXC_TT_S to TXC_TRAFFIC_S, this function will be called
  to initialize the encoder to be ready for traffic.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_tt_tc_init (boolean turn_on_tx)
{

#ifdef FEATURE_IS2000_REL_A
  int sr_index;
#endif /* FEATURE_IS2000_REL_A */

  txtc.power = txac.last_pwr;    /* set last access probe power level */

  txc_register_action_handlers_notifier_interrupt_func();
  /* Convert to units of 1/2 dB and add rl_gain_adj value
  ** Note that txtc.power is sign inverted.
  */
  txtc.power -= (byte) (2 * txtc.rl_gain_adj);

  (void)memcpy( txtc.frame.data, zeros, 24 );   /* copy zeros into frame */

  txtc.frame.sig = NORM;          /* no data for interrupt handler */

  txc_pilot_init();

  /* base on the input, whether MUX should setup for a HHO or a SHO.
  For HHO, all transmitter should be off. For the SHO, the transmitter
  should be on */
  enc_tx_enable (turn_on_tx);     
  txc_tx.tx_on = turn_on_tx;  
  rxc_set_tx_in_traffic_on(turn_on_tx);
  txtc.ready_to_transmit = turn_on_tx;

  if (turn_on_tx)
  {
    /* Events report: Transmitter enabled */
    event_report(EVENT_TRANSMITTER_ENABLED);
  }

  txc_tx.tx_blank = TRUE;         /* expect a blank first */

  srch_mux_set_tx_adj( txtc.power );  /* set power to last access probe level */

#ifndef FEATURE_IS2000_REL_A
  /* default to no primary or secondary service option */
  txc_so.pri = CAI_SO_NULL;
  txc_so.stored_pri = CAI_SO_NULL;
  txc_so.sec = CAI_SO_NULL;
#else
  /* default to no primary or secondary service option and no physical mapping */
  txc_so.pri.so = CAI_SO_NULL;
  txc_so.sec.so = CAI_SO_NULL;
  txc_so.pri.phy_channels = 0;
  txc_so.sec.phy_channels = 0;
  /* set to defautl no service option for each reference ID */
  for (sr_index = 0; sr_index < SR_ID_MAX; sr_index++)
  {
    txc_srid_ref[sr_index].so = CAI_SO_NULL;
    txc_srid_ref[sr_index].traffic_type = CAI_NUL_TRAF;
    txc_srid_ref[sr_index].phy_channels = 0; 

  }
  txc_so.signaling_phy_channel = 0;

  prim_rate          = CAI_BLANK_RATE;
  prim_data_ptr      = NULL;
#ifdef FEATURE_IS2000_P2
  prim_dcch_rate     = CAI_NULL_RATE;
  prim_dcch_data_ptr = NULL;
#endif /* FEATURE_IS2000_P2 */

  sec_fch_num_bits   = 0;
  sec_fch_data_ptr   = NULL;
#ifdef FEATURE_IS2000_P2
  sec_dcch_num_bits  = 0;
  sec_dcch_data_ptr  = NULL;
#endif /* FEATURE_IS2000_P2 */

#ifdef FEATURE_IS2000_REL_A_SVD
  /* txc_rlp_sig_priority is used to keep track of priorities in sending RLP control 
  ** frames when RLP is the secondary SO in a SVD call.  The purpose of the
  ** implementation is to improve voice quality in a SVD call.
  */
  txc_rlp_sig_priority.prev_fr_val = DSRLP_TX_NONE;
  txc_rlp_sig_priority.curr_fr_val = DSRLP_TX_NONE;
  txc_rlp_sig_priority.delay_fr_cnt = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */

#endif /* FEATURE_IS2000_REL_A */

  txtc.preambles = TRUE;          /* send preambles when we first get here */
  /* Preambles always in frames at TCI */
  txc_hho_preamble_pcg_units = FALSE;

  #ifdef FEATURE_IS2000_CHS
  // Initialize to no pilot gating
  txtc.gating_rate = CAI_GATING_RATE_NONE;  
  #endif // FEATURE_IS2000_CHS

  txtc.num_preambles = 0xFFFF;
  txtc.nulls = FALSE;             /* not sending null data yet */
  txtc.last_rate = ENC_8TH_RATE;   /* close as we get to nothing sent yet */

  /* initialize number of lost logged vocoder packets */
  txc_lost_voc_pkts = 0;

#if !defined(FEATURE_DIAG_V7_LOG_IFACE)
  txc_log_mux_buf.count = 0;
#else
  if( txc_mux1_ptr ) {             /* if left over buffer - put on q */
    q_put( &txc_log_mux1_q, &txc_mux1_ptr->desc.link );
    txc_mux1_ptr = NULL;
  }
#endif

  txc_tx.active_msg = FALSE;
  txc_tx.delay = TXC_NO_CONF;              /* no confirmation delay */

#ifdef FEATURE_IS2000_R_SCH
  txtc.pot_num_mux_pdus = 0;
  txtc.ok_num_mux_pdus  = 0;

  txtc.dtx_duration_included = FALSE;
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_MAX_PWR_PROTECTION
#ifdef T_IO_CARD
#error code not present
#else
  if (BIO_GET_SW( BIO_SW11_M ))
  {
    /* If dipswitch 11 is ON, MAX PWR PROTECTION on R-SCH is disabled
    */
    txc_sch_max_pwr_protection_enabled = FALSE;
  }
  else
  {
    txc_sch_max_pwr_protection_enabled = TRUE;
  }
#endif /* T_IO_CARD */
#endif // FEATURE_MAX_PWR_PROTECTION
#endif /* FEATURE_IS2000_R_SCH */

  /* --------------------------
  ** Go to Traffic Channel mode
  ** -------------------------- */
  enc_set_mode( ENC_TC_MODE );
  enc_user_pn_mask( txc_tx.mask );

  /* Set the counter so that TXC ISR calculates the frame number for
  first NO_OF_ISRS_AFTER_FO isrs. */
  after_fo_handoff_count_isrs = NO_OF_ISRS_AFTER_FO;

  /* Initialize default PCG for bin0 ENC ISRs, which is the earliest PCG to 
  ** start frame building.  Need to do this before registering 
  ** function txc_traffic_isr() because this time transfer functionality does
  ** not go through normal txc traffic start up, i.e. it does not call 
  ** txc_traffic_init() where txc_slot0_isr_pcg is set.
  */
  txc_slot0_isr_pcg = TXC_SLOT0_ISR_PCG_NUM;

  txcmod_int_deregister_all(TXMOD_INT_MUX_CLT);
  (void) txcmod_int_regis_for_pcgint(txc_slot0_isr_pcg,
                                     txc_traffic_isr,
                                     ENC_STATUS_INTERRUPT_SLOT_0,
                                     TXMOD_INT_MUX_CLT);
  (void) txcmod_int_regis_for_pcgint(TXC_SLOT3_ISR_PCG_NUM,
                                     txc_traffic_isr,
                                     ENC_STATUS_INTERRUPT_SLOT_3, 
                                     TXMOD_INT_MUX_CLT);

  if (txtc.tcb_ptr != NULL)
  {
    /* Notify requesting task TXC is entering traffic state */
    (void) rex_set_sigs( txtc.tcb_ptr, txtc.sigs );
    txtc.tcb_ptr = NULL;
  }
} /* txc_tt_tc_init */
#endif /* FEATURE_VOIP_1X_HANDOFF and FEATURE_UMTS_1X_HANDOVER_1XMSM */

