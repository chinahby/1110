#ifndef MCCDMA_H
#define MCCDMA_H
/*===========================================================================

    M A I N   C O N T R O L   T A S K   C D M A    S U B - S Y S T E M
                       H E A D E R    F I L E

DESCRIPTION
  This file data and function declarations necessary for the CDMA Sub-system
  of the Main Control task.

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccdma.h_v   1.53   02 Oct 2002 13:49:34   sbrelsfo  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccdma.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/29/09   pk      Added MEID_ME to CDMA structure for FEATURE_HWID_SUPPORT
06/26/08   bb      Changes for FEATURE_8K_STUFF_TO_BE_PURGED feature
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
12/14/07   an      Added Passport Mode
12/13/07   sb      Check ddtm only in Idle state, not System Access.
07/27/07   pg      Added mccdma_get_cur_bs_info() to pass current base station
                   info to upper layer.
06/14/07   an      Added support for White Grey Black Card Operation
05/14/07   pg      Do not let srch do QPCH after a failed reacq. 
03/09/07   pg      removed dependency on WMS and make sure it only depends on 
                   CM for SMS.
02/23/07   sb      Added support for operational mode feature.
02/03/07   fc      Corrected the ovhd rxed macros.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/27/06   fh      AKA/AES: changed CRC-8 calculation algorithm to handle more 
                            general cases.
10/19/06   fc      Code review changes.
09/30/06   fc      Featurization changes.
09/30/06   fh      AKA/AES: moved the 8-bit CRC calculation from mccaes.c into
                            mccsup.c
09/19/06   fh      AKA/AES: added sa_msg_tx_msg to the data structure of
                    cdma_ms_type so that Security Mode Request Message can
                    be sent in CDMA_MSG sub state.
09/14/06   fh      AKA/AES: added a new field to aes_status to carry the
                            crypto-sync for AES encryption/decryption use.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/17/06   fc      Optimize the implementation of pseudo bs.
07/13/06   fh      AKA/AES: Cleaned up AES capability helper functions.
07/11/06   fc      Updated function prototype for mcc_send_cm_srv_sys_info.
07/07/06   fc      Moved member with size dependent on JCDMA to the end of
                   cdma_ms_type so as to make cp libary code common to JCDMA 
                   and non-JCDMA build.
07/06/06   fc      Renamed ABORT_RF_RESOURCE_LOST to ABORT_SRCH_REQUEST_SYS_RESTART.
06/29/06   fc      Obsolete offline_cmd from cdma structure.
06/23/06   pg      Added support for RPC.
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the 
                   memory by the size of MAX PRL size and use SD PRL for 
                   uploading.
06/19/06   fh      Added two function prototypes for mux files to encrypt/decrypt
                   voice service frames. Note that these two function prototypes
                   along with the two functions in mccdma.c are not under feature
                   FEATURE_IS2000_REL_A_AES on purpose. The function body has 
                   the appropriate features to process the encryption/decryption.
06/15/06   fc      Added definition for pseudo pilot pn.
06/12/06   fc      Added support for service status order.
06/12/06   fc      Added mob_qos in cdma_ms_type.
06/09/06   fc      Moved all gsrm/egsrm related functions back to mccidl.c.
                   Fixed the data corruption issue of sharing a common data 
                   structure for gsrm/egsrm and srdm processing. 
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
06/02/06   fc      Cleanup umts interface.
06/01/06   an      Added the bypass registration indicator changes
05/31/06   pg      Changed function mcc_inform_sd_redirection() interface.
                   Release D EGSRM code cleanup.
05/30/06   fc      Added support of MC command processing in slotted mode.
05/30/06   fc      Removed obsolete element end_by_data from cdma_ms_type.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/19/06   fc      Added enum mcc_tranceiver_request_status_type. 
05/18/06   fc      Added mcc_tx_int_msg and mcc_rx_int_msg.
05/17/06   fh      Merged in new development of AKA/AES features
04/26/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurixation.
04/12/06   pg      Added support for release D EGSRM.
03/14/06   sb      Added fix for KDDI related to high persistence.
03/01/06   fh      Added AES Support
02/16/06   dna     Avoid HDR doing extra PCCC negotiation.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
02/07/06   fc      Updated function prototypes.
02/13/06   an      Added ECCLM Cross Band Hashing Support
01/20/06   fc      Merged the following :
           yll     Added support for AKA.
01/06/06   fc      Cleanup on OOS changes.
11/03/05   an      Moved MCC_CHK_PTR macro definition to mc.h from mccdma.h
10/28/05   an      Made band class/subclass code review changes
10/19/05   sb      Removed MAX_BANDS_IN_GROUP.
10/15/05   fc      Cleanup NCC featurization.
10/14/05   an      Added Band Class/Subclass Reporting Feature.
07/26/05   fc      Fixed lint errors.
07/25/05   fc      Changes for Rel D Addendum f-csh messages.
07/22/05   va      Removed is_jcdma_emergency flag
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed rxc.h to rxcmc.h
                     changed txc.h to txcmc.h                     
07/08/05   fh      Added new definitions for SHDR pahse II
                     added boolean transceiver_request_pending into
                      the data structure cdma_ms_type
                     removed function mcc_send_srch_access_cmd
                     added the following two functions:
                       mcc_wait_transceiver_resource
                       mcc_request_transceiver_resource
07/18/05   fc      Removed multiple definition of meid to fix compile error.
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/20/05   pg      Removed FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Renamed FEATURE_JUMP to FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Added support for early jump to hyperspace under FEATURE_JUMP.
04/27/05   az      Submitted changes for CR 62361. This also requires 
                   corresponding changes in NV. Changed the name of a #define
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone, 
                    auth and rand during AHO/APHO.
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/28/05   fc      Added support for DCA.
03/25/05   az      Changed the name from MCC_USE_DEFAULT_SCI to MCC_NULL_SCI.
03/25/05   az      Created a #define for initial value of last_reg_sci.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated 
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/14/05   an       Added special dup detection function for ecam
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/09/05   va      Merged following from REL_B_C archive:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/10/05   va      Merged following from REL_B_C archive:
12/16/04   an        Added hashed_cdmach to cdma_bs_type
12/06/04   yll       Added interface change for apiOne.
12/22/04   pg      Moved mcc_flush_rxtx_q() to mccsup.c.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
09/27/04   va      Merge from REL B/C archive
07/23/04   yll      Mainlined the FEATURE_INT_ROAMING.
07/21/04   sj       Added support for umts to 1x handover.
07/12/04   pg       Added is_cdma_subs_avail to cdma_ms_type to indicate
                    current CDMA/AMPS/HDR subscription availability status.
09/21/04   az      Changed feature name from FEATURE_REL_D_NEG_SCI to 
                   FEATURE_IS2000_REL_D_NEG_SCI
09/16/04   fc      Added support for reverse DV.
08/17/04   az      Rel D Negative SCI changes
08/09/04   fc      Service negotiation changes for Release C and D.
08/09/04   bkm     Converted T50 and T51 timers to callback timers.
08/03/04   ht      Added Rel D MEID LAC support
06/16/04   va      Added tc_tx_on and tc_tx_to_be_on variables
06/16/04   fc      Bundle Aset update with channel config update.
                   Rel C addendum changes on ECAM.
06/14/04   fc      Renamed sr_id_included to is_sr_id_valid.
06/08/04   sb      Fixed featurization of long_code struct.
05/28/04   bkm     Merged from IS2000_REL_B_C:
05/17/04   sb      Typedef'd off_time structure.
05/17/04   va      Merged following from MSMSHARED:
03/11/04   yll      Added support for KDDI requirement to ignore async data
                  and async fax 1x pages during HDR connected state.
03/05/04   bkm      Lint cleanup.
04/09/04   sb      Typedef'd long_code structure
03/36/04   bkm     Merged from IS2000_REL_B_C
03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
03/19/04   va      Merged following from MSMSHARED
02/27/04  jrp     Added ABORT_UNSUPPORTED_PARAMETER to access_result_type for
               case of unsupported REACH parameters.
02/24/04  RC      Added support for FEATURE_INT_ROAMING
02/27/04  bkm     Moved typedef for access_result_type from mccsa.c to this file.
                  Added extern def of calc_psist(). These are now used in mccreg.c.
03/12/04   fc      Added the definition of MCC_PRI_BCCH_BCN.
02/09/04   va      Consolidated TC to Idle related info in a separate
                   data structure
02/05/05   bkm     Merged from IS2000_REL_B_C
01/09/04   yll     Fixed a compile error when FEATURE_BROADCAST_SMS is 
                     undefined.
02/05/04   bkm     Removed obsolete macro ALLOCATED_CDMA_CHN that always 
                   returned true.  Many MSMs ago it was decided that L3 
                   should not verify channels.
01/05/04   bkm     Added boolean tc_tx_on to cdma_ms_type.
11/13/03   yll     Added support for JCDMA emergency calls.
10/30/03   az      Added function declaration mcc_is_current_band_class_in_group
                   for FEATURE_ENHANCED_BAND_CLASS
10/23/03   va      Added New fields for TC to Idle in cdma_ms_type 
10/23/03   yll     Added support for updating HDR PANID.
10/23/03   bkm     Added types for enhanced Idle and Access HO events.
10/15/03   is      Added stale_info in mcc_pd_bs_info_type for
                   FEATURE_GPSONE_BS_INFO
10/03/03   yll     Added mccidl_is_fast_raho().
10/03/03   bkm     Using explicit values to define enum values in
                   mccdma_page_match_result_type to potentially allow
                   for bit-mask like operations.
09/11/03   yll     Added FEATURE_ACP to analog code.
08/25/03   anr     added changes to support setting base_lat and base_long
                   in BSINFO struct.
09/04/03   fc      Added ruim_id_avail to CDMA structure.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/08/03   fc      Renamed caii_sec_mode_msg_type to caii_sec_mode_type.
08/06/03   sb      Added support for CDMA Off Time Reporting.
07/30/03   az      Added support to return overflow size for MO SDB
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
05/27/03   az      Removed imsi_hash_key from QPCH featurization. This
                   to fix the incorrect reporting of Hash key during 
                   RAHO.
04/14/03   va      Changed data structure for TC to idle transition.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
03/21/03   fc      Removed qpch_bi_supported and qpch_power_level_bcast from
                   mcc_complete_sys_parms_type.
01/23/03   yll     Added mcc_sdb_supported function.
12/12/02   hxw     Added l2_time_out in mcc_pd_layer2_type.
11/27/02   ph      Fixed compile error.
11/25/02   va      Merged gpsOne force dormancy changes MSM archive.
11/18/02   va      Added prototype for mcc_send_orig_fail_retry_delay
11/14/02   sb      Featurized rand parameters under FEATURE_AUTH.
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/24/02   yll     Changed the mcc_bc_match prototype.
10/17/02   sb      Added access function for slot_cycle_index_p.
10/02/02   sb      Renamed is_silent_redial, silent_redial_count to orig_reason, orig_count.
10/01/02   az      Mainlined FEATURE_IS2000.
09/30/02   sb      Mainlined FEATURE_SD20.
09/04/02   AP      Included wmsbc.h for FEATURE_BROADCAST_SMS and
                   FEATURE_CDSMS support.
09/02/02   fc      Added macro NLM_MSG_IS_RECVD.
08/30/02   ts      Added support for Run Time R-UIM Enable.
08/13/02   HQ      Added FEATURE_CDSMS.
08/08/02   fc      Added function prototype mcc_update_acc_msg_seq.
08/01/02   fc      Changed mcc_parse_page function prototype.
                   Added mcc_increment_pag_3_or_fccch_3 function prototype.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
06/27/02   fc      Changed UPM function names.
06/21/02   ph      changed cmmc.h to cmxll.h
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/12/02   yll     Added mccidl_assert_sleep_ok() prototype.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed the broadcast SMS function names
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/12/02   lh      Added support for subaddress in Orig_C.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
04/01/02   fc      Added function prototype mcc_is_upd_ovhd_msg_okay.
03/28/02   sb      Added mcc_rand_parms_type.
03/13/02   fc      RelA addendum 2 changes
03/09/02   lh      Merged from common archive.
02/13/02   ts      Include uim.h when FEATURE_UIM_RUIM is defined.  This allows
                   FEATURE_AUTH to be not defined with FEATURE_UIM_RUIM.
02/07/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive
02/06/02   sh      Added hybird operation support
01/31/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
02/07/02   lh      Added parameter to process_intercept function.
01/22/02   va      Fixed some errros for code to compile with REL_A turned off
01/14/02   va      Removed is_cur_bs_cs_supported function
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/13/01   va      Merged in from SVD branch.
11/30/01   va      Made "cs_supported" part of cdma_ms_type struct
08/28/01   lh      Support for concurrent service.
08/23/01   ph      Update to tc_so_change() for SVD.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
10/02/01   kk      Qos Parameters Added.
09/06/01   fc      Changed the function prototype of mcc_parse_page.
                   Removed the function prototypes is_in_access_ho_list and
                   is_access_ho_allowed.
                   Removed extern definitions sleep_slot_mask, bc_slot_cycle,
                   mccdma_bc_init and mccdma_bc_page.
08/22/01   sb      Changed encryption fields in mccdma_chnasn_am0_type and
                   mccdma_pc_ord_type.
08/16/01   kk      Merge from common archive.
                   - Defined a variable for RUIM.
08/08/01   yll     Merged in the following from the MSM archive:
                   Added esn_me and ruim_id to the cdma structure.
                   Rid unused mcc_ds_buf.
07/16/01   fc      Moved cai_fccch_rate_type to cai.h.
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
06/11/01   fc      Added extern definition for mc_update_last_cdma_entry
                   (SD20 only).
06/06/01   lcc     Changed retry_type in retry_delay_type to cai_retry_order_type
                   and changed prototype of update_retry_delay to use
                   cai_retry_order_type.
05/29/01   fc      Added mcc_redir_info_type definition (SD20 only).
05/25/01   ts      Added an interface to the new UIM server.
06/14/01   va      Changed substate enums to reflect the new TC substates and
                   Call Control instance substates.
05/17/01   ph      Merged in from MSM_CP.05.00.76
04/17/01   va      Added nghbr_pilot_rec_type in the neighbor list.
                  Fixed bug in GNLM_MSG_IS_RECVD and GNLM_MSG_NOT_RECVD macro.
04/12/01   fc      Moved all events logging function data types / prototypes
                  to mclog.h and mclog.c.
04/10/01   lcc     Added some macros and changed macros RATE_SET_OF_FWD_RC and
                  RATE_SET_OF_REV_RC.
04/18/01   va      Changed "pilot_rec_type" name  in nl to "nghbr_pilot_rec_type"
03/28/01   va      Merged the folowing from common archive.
03/06/01 fc      Added support for events report: Message received, Message
                   transmitted, Timer expired, Counter threshold and Call
                   processing state changes.
03/07/01   ph      changed prototype of mcc_info_rec() to reflect new compute_independent_
                   msg_len() in mccsa.c
03/02/01   ph      Merged in from MSM_CP.05.00.55
02/16/01   fc      Added support for system determination 2.0. All changes
                  are featurized under FEATURE_SD20.
02/16/01   sh      added support for MC_END_F ack
01/24/01   ych     Merged T-53/JCDMA features for SK.
01/16/01   jq      Moved/renamed mccsch_retry_delay_type, mccsch_retry_delay to
                   here.
12/14/00   cah     Remove a uasmsi.h inclusion for FEATURE_SMS.
12/13/00   cah     Replaced obsolete smsi.h inclusion with uasmsi.h.
11/28/00   ck      Externalised the definition for RUIM Access buffer.
                   Added ruim_ota_features to the cdma structure.
02/27/01   kk      added new origination parameters to cdma_ms_type
02/26/01   ph      removed func mcc_check_ext_encryption_fields_is_okay() (rel A)
01/10/01   va      Added support for storing RELA overhead messages/info in the
                   BS info data structure.
01/10/01   ph      1. Added Encryption, SR3 fields to mccdma_chnasn_am0_type.
                   2. Added Encryption fields to mccdma_pc_ord_type.
                   3. Added Security Mode Command Message to mccdma_matched_msg_type.
                   4. Added prototype for mcc_check_ext_encryption_fields_is_okay().
11/13/00   lcc     Added function prototype for mcc_update_bs_p_rev.
11/01/00   lcc     Added drs member in cdms_ms_type to support DRS bit in ORM.
09/11/00   yll     Added support for Position Determination DBM.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   jq      Added mcc_set_rsch_dtx_sig ()
08/18/00   fc      Added retry_type and retry_delay to mccdma_pc_ord_type.
07/21/00   fc      Used enumeration type for FPC mode.
07/17/00   jq      added end_reason to cdma_ms_type.
07/14/00   ks      defined t_slotted in cdma_ms_type. Defined ecl_rx, egsd_rx
                   in cdma_msg_type. Define ecl, egsd in cdma_bs_type. These
                   fields support new PC messages and Slotted timer.
07/14/00   va      Added pwr_cntl_delay and rev_fch_gating_on in cdma_ms_type.
07/06/00   yll     Added the interface to provide the MSID and registered BS
                   info, include: band class, channel, bs ID, SID and NID.
06/26/00   sh      added AEHO_OVHD_MSG_OK
06/14/00   ks      Added band_class parameter to cdma_rf_init function.
06/04/00   jq      Changed parameter list of mcc_parse_page() and
                   mcc_detect_dup_bcast()
05/26/00   jq      Added slot_mask related defines and processing functions.
                   Fixed MAX_BC_PER_PAGE_MSG.
04/25/00   sh      Increase MC_MAX_BASES to 10
04/11/00   lcc     Added data structure and members to support QPCH.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added Broadcast SMS feature support.
03/10/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ht      Add mcsyspr_set_last_cdma function prototype.
           ram     Fixed a typo in my last check-in which caused problems
                   in the SUPPORTED_BANDS, SUPPORTED_CDMA_BANDS, and
                   SUPPORTED_OP_MODES macros for trimode targets.
           ck      Added band class to the cdma_bs_type and also the to
                   the mcc_entry_init prototype.
           ram     Extended the Macros SUPPORTED_BANDS, SUPPORTED_CDMA_BANDS,
                   SUPPORTED_OP_MODES to include Trimode and Dual CDMA band
                   Targets.
01/26/00   va      Merged in changes from GV40401
                   Removed mcc_handoff_timer.
                   Modified SUPPORTED_OP_MODES to allow op codes 0 and 1 for
                   IS-95 MS in cellular or PCS band.
12/20/99   va      Added new data struct to store power control info temporarily.
                   Added fpc_subchan_gain to cdma_ms_type.
11/15/99   va      IS2000 cHANGES FOR VOICE.
                   Support for QOF.
                   mccdma_chnasn_type expanded for support of enhanced ECAM.
                   New mcros added for RC_CLASS() determination.
                   mcc_info_rec prototype changed.
10/28/99   lcc     Moved PLT related code to plt_mc.c.  Also added extern
                   function declarations for some functions used in plt_mc.c
08/24/99   fc      Removed nset_cnt from plt_rpt_ovhd_rec_type because SRCH
                   will provide the count of pilots in Pilot Strength
                   Measurement report.
08/21/99   ks      Modified plt_rpt_ovhd_rec_type and othrpt_pilot_info to
                   support pilot reporting for already attempted pilots.
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Fixed a merge problem in which a #endif for FEATURE_ACP
                   was misplaced
07/28/99   lh      CM and two-way SMS support added.
07/28/99   lh      CM support added.
07/22/99   fc      Changes in Pilot Reporting to support Access Probe Handoff.
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Added a new rex time for IS95B hard handoff
06/24/99   ks      Fixed a typo in AHO_OVHD_MSG_OK macro
06/22/99   kmp     Moved the definitions for IMSI_S1_ZERO, IMSI_S2_ZERO,
                   IMSI_MCC_ZERO and IMSI_11_12_ZERO to this file from
                   mccdma.c since these values are also used in mccidl.c in
                   function proc_true_imsi() when FEATURE_IS95B_TRUE_IMSI is
                   defined.
06/17/99   kmp     Merged in the following IS-95B changes.
06/13/99   fc      Changed the data structure of plt_rpt_ovhd_rec_type,
                   aho_pilot_info_type and othrpt_pilot_info.
06/07/99   ks      Added roam_indi field in mccdma_pc_ord_type structure.
06/04/99   jq      Added cdma_sa_orig_ministate_type for Release on Access
                   Channel feature
06/04/99   sh      changed data type from byte to int1 for add_intercept and
                   drop_intercept in mcc_ho_cb_type
05/26/99   fc      Added the support for closed loop power control step size
                   in the cdma_ms_type structure.
05/24/99   doj     Added p_rev_in_use to cdma_ms_type (for FEATURE_IS95B).
                   This was already done in the IS-95B branch is required
                   for all builds not just FEATURE_IS95B.
05/12/99   ks      Added Macros to check if AMPS is supported for a band class.
05/10/99   kmp     Merged the following IS-95B related changes and changed
                   T_SMS, T_ACP and T_AUTH to FEATURE_SMS, FEATURE_ACP,
                   and FEATURE_AUTH respectively.
           ks      Added AHO_OVHD_MSG_OK macro to validate overhead messages
                   during Access Handoff or Access probe Handoff.
05/06/99   kmp     Merged IS-95B changes below and changed T_SMS, T_ACP and
                   T_AUTH to FEATURE_SMS, FEATURE_ACP, and FEATURE_AUTH
                   respectively.
           kmp     Modified the mcc_pc_frames array to be 3D by adding
                   ADDR_TYPE checking for duplicate page messages for IS-95B.
           ks      Addded an external function is_in_access_ho_list.
           ks      Added an external function is_access_ho_allowed.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           fc      Put in changes based on code review input.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           fc      Put in changes based on code review input.
           jq      Modified some naming due to code review
           fc      Fixed lint error.
           ks      Removed CDMA_IDLE_AHO from cdma_idle_state_type enum.
           kmp     changed type of cdma.ho_state to cdma_ho_type from a word
                   Added include for mccsrch.h for pc_srch_rpt function proto.
           fc      Modified comments. Renamed plt_rpt_bookeep structure to
                   plt_rpt_ovhd_rec and pilot_rpt_bookeep structure to
                   pilot_rpt_ovhd_rec.
           jq      Added declaration for pc_srch_rpt, so lint is happy
           jq      Modified mccdma_chnasn_type, and channel assignment
                   related structures to accomodate both CAM and ECAM
           lh      Authentication using IMSI_M.
           fc      Defined ACCESS_HO_LIST, OTHER_REPORTED_LIST and
                   PLT_RPT_BOOKEEP.
           fc      Added PLT_RPT_BKP, ACC_HO_LIST and OTHER_RPTED_LIST data
                   types for pilot reporting. Added ACCESS_ATTEMPTED field in
                   CDMA_NGHBR_SET_TYPE to support pilot reporting. Defined
                   NO_PREV_PILOT. Defined ACC_MSG_NUM_PLT_ALLOWED() and
                   ACC_MSG_PLT_RPT_NEEDED() macros.
           ks      Removed CDMA_HO_STATE from main state enumerations.
           sh      Added mcc_send_srch_parms function.
           ks      Defined a new enumeration for various handoffs.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kmp     Added Phased Deployment support, added PACA cancel substate
                   to the cdma_sa_state_type enum.
           sh      added IS95B soft handoff parameters
           ks      Added a new macro to check status of Nghbr List messages.
           ks      Added IS95B overhead message processing fields. Added fields
                   ho_sa_state and ho_state fields for Access Handoff. Added
                   cdma_prev_bs_info data type for holding previous BS info.
           lh      p_rev_in_use is added to cdma_ms_type structure.
           lh      IS-95B True IMSI implementation added.
06/21/99   lh      Two-way SMS and CM support.
06/01/99   sh      added mcc_check_alert_signal_info()
04/25/99   abh     Added prototype for mcc_init_ovhd_info, which was introduced to
                   move the initilisation of bs_info array at the start of CDMA Acquisition
                   process, rather than after getting Sync Message.
04/07/99   abh     Changes for the new Queue Interface.
03/18/99   ck      Added imsi_t variables, auth_data and spc_change_enabled
                   to cdma structure.
03/18/99   ck      Do not define mcc_voc_selftest if FEATURE_VOCODER_MANAGER.
                   Merged the change from SPSW rev 1.85 by ro
03/09/99   ram     Externalised mcc_mdr_duration_timer for IS95B MDR
02/19/99   ram     Externalised mcc_scam_start_timer for IS95B MDR
01/27/99   ck      mcc_pref_voice_so is ifdefed under FEATURE_EVRC_SO_MGMT
12/08/98   dna     Fixed some compile warnings (merged from ASW 1.11)
10/09/98   ck      Externalised mcc_pref_voice_so
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
08/25/98   ck      Added functions mcc_queue_dh_rpt() and mcc_dh_cmd() to
                   support the dh task.
08/24/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
08/20/98   ck      Removed mcc_voc_capability definition, no longer needed.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/09/98   lcc     Fixed problem with illegal characters after #endif statement
07/02/98   pms     Added element "end_by_data" to cdma_ms_type struct.
07/02/98   pms     Added prototype for process_ds_call_failed().
06/25/98   kis     Added digit_mode and ASCII related fields in cdma_ms_type.
05/21/98   abh     MAX_SID_NID changed to MAX_HOME_SID_NID for SSPR-800
02/18/98   dna     Added SUPPORTED_CDMA_BANDS and BIT_MASK macro to allow
                   System Determination to run-time check for supported band.
01/27/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/27/98   ych     Featurized mcc_idle_timeout and defined a constant
                   MAX_DWORD_VAL. Also added customer.h to the include file
                   list
08/17/97   ych     Changed the MCC_VALID_OVHD_TIMEOUT timer value to 30000
                   from 10000.
07/15/97   dna     Moved SUPPORTED_BANDS and SUPPORTED_OP_MODES here.
06/16/97   dna     New func. sets hold_orig and timer when returning to idle
05/20/97   ck      Added support for LPM.
04/18/97   dna     Changed some #ifdefs to make class 1 IMSI its own feature
01/25/97   dna     More flexible reject of status request.
12/13/96   jca     Added new fields to mccdma_chnasn_am2_type.
10/18/96   dna     Added TEMP_NL_SEQ
09/13/96   dna     Added cdma.otasp_call flag.
09/11/96   jca     Changed mcc_xlate_dtmf() to mcc_encode_dtmf().  Added new
                   function mcc_decode_dtmf().
08/08/96   dna     Add Service Programming Code to cdma. structure for OTASP.
07/10/96   day     Add mcc_pc_pg_frames array for layer2 duplicate detection.
                   Add input parameter to identify page/non-page messages
                   in mcc_detect_duplicate.
06/25/96   dna     For FEATURE_OTASP, added mob_dir to cdma structure.
06/10/96   jpf     Removed BS tmsi_zone from cdma_ms_type.
05/28/96   dna     Added imsi_addr_num for class 1 IMSI support.
05/24/96   jjw     Gemini & ISS2 Data Services release.
05/15/96   dna     Changed auth_on to auth_mode and encryption_enabled to
                   encrypt_mode.
05/13/96   jca     Added band_class and block_or_sys to cdma_ms_type.
04/15/96   jca     Changed cdma_dtmf_type to accomodate queued Burst DTMFs.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/13/96   day     Added J-STD-008 PCS message modifications.  Using FEATURE_JSTD008
                   ifdef.
02/08/96   jca     Put ()s around macros to make Lint happy.
12/08/95   bns     Removed flag for ISS2 forced registration
12/06/95   bns     Added flag for ISS2 forced registration
11/29/95   jca     Deleted sid_acq field from cdma_ms_type.
11/14/95   jca     Removed support for configurable service option 1 def.
10/05/95   dna     Added cdma.authr_valid so zero could be valid RAND value
08/18/95   dna     Changed mcc_pc_frames array, added assign mode '101'
08/17/95   jca     Added function mcc_process_info_records().
08/10/95   rdb     Added declarations to support enhanced service messaging.
07/26/95   jca     Added an_chan_type field to mccdma_chnasn_am3_type.
07/20/95   jca     Added support for Subscriber Access Control.
07/13/95   rdb     Added types for assign mode 4 of chan assign message
                   Added serv_neg to cdma structure.
06/25/95   dy      Added CDMA SMS data structures and functions to support
                   transmission of SMS messages.
06/09/95   jca     Changed mcc_wait() interface to support new queue sigs.
06/07/95   dna     Added Authentication Definitions
11/14/94   dna     Added headers for all externalized functions.
                   Added page_chan_s to cdma_ms_type.
08/09/94   jca     Deleted function mcc_get_voc_version and other misc mods.
05/12/94   gb      Added new test commands.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Modified for IS-95 Phase 2 release.
12/22/93   jca     Modified for IS-95 Phase 1 release.
03/24/93   gb      Removed debug t_tdrop variable.
07/24/92   jai     Changed externs for several procedures for modified calling
                   parameters.
06/26/92   jai     Created file for DMSS.
12/20/90   jai     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "qw.h"
#include "mc.h"
#include "mcc.h"
#include "mccrxtx.h"
#include "rxtx.h"
#include "nv.h"
#include "srchmc.h"
#include "cmxll.h"
#include "db.h"
#include "caii.h"
#include "caix.h"
#include "voc.h"

#ifdef FEATURE_CDSMS
#include "wmsmsg.h"

#ifdef FEATURE_BROADCAST_SMS
#include "wmsbc.h"
#endif /* FEATURE_BROADCAST_SMS */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_AUTH
#include "auth.h"
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_DS
#include "ds707_extif.h"
#endif /* FEATURE_DS */

#ifdef FEATURE_OTASP
#include "otaspi.h"
#endif /* FEATURE_OTASP */

#ifdef FEATURE_DH
#include "dh.h"
#endif /* FEATURE_DH */

#include "mccsrch.h"
#include "mccqpch.h"

#include "rxcmc.h"
#include "txcmc.h"
#include "sd.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Main states */
#define  CDMA_ENTER            0x0000
#define  CDMA_INIT             0x0100
#define  CDMA_IDLE             0x0200
#define  CDMA_SYS_ACC          0x0300
#define  CDMA_TC               0x0400
#define  CDMA_EXIT             0x0500
#define  CDMA_CC               0x0600
#define  CDMA_VOIP_HO          0x0700

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumerations for substates                                              */

/* State codes are separated by 256 and substate codes are consecutive.
   No substates are needed for the CDMA_ENTER or CDMA_EXIT states.         */

/* ---------------------------------
** Substates of Initialization State
** --------------------------------- */
typedef enum
{
  CDMA_PIL_ACQ = CDMA_INIT,  /* Pilot Channel Acquisition */
  CDMA_SYNC_ACQ,             /* Sync Channel Acquisition (part of IS-95
                                Sync Channel Acquisition substate) */
  CDMA_SYNC_MSG,             /* Process Sync channel Message (part of
                                IS-95 Sync Channel Acquistion substate) */
  CDMA_JUMP,                 /* Jump to Hyperspace (IS-95 Timing Change
                                Substate) */
  CDMA_UNJUMP                /* Jump back from Hyperspace */

} cdma_init_state_type;

/* -----------------------
** Substates of Idle State
** ----------------------- */
typedef enum
{
  CDMA_IDLE_INIT = CDMA_IDLE,    /* Initial entry into idle */
  CDMA_IDLE_MSG_PENDING,         /* Had to send ACK and leave msg pending.
                                    System Access finished, process msg now */
  CDMA_UNSLOTTED,                /* Idle not operating in slotted mode */
  CDMA_SLOTTED,                  /* Idle operating in slotted mode */
  CDMA_OVHD                      /* System Access Update Overhead info */

} cdma_idle_state_type;

/* --------------------------------
** Substates of System Access State
** -------------------------------- */
typedef enum
{
  CDMA_UPDATE_OVH = CDMA_SYS_ACC, /* Update Overhead Information */
  CDMA_ORIG,                      /* Mobile Station Origination Attempt */
  CDMA_PAGE_RES,                  /* Page Response */
  CDMA_ORD_RES,                   /* Mobile Station Order/Message Response */
  CDMA_REG,                       /* Registration Access */
  CDMA_MSG,                       /* Message transmission */
  CDMA_PACA_CAN                   /* PACA Cancel Substate */

} cdma_sa_state_type;

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/* Substates of Origination state. With Release Order on AC feature, the
 * mobile could be waiting for Lay2 ACK of Release Order instead of CAM/ECAM
 * in the Origination state. */
typedef enum
{
  CDMA_ORIG_NORM,   /* Origination state while not in release process */
  CDMA_ORIG_RELS    /* Origination state while in release process */

} cdma_sa_orig_ministate_type;

typedef enum 
{
  NOT_ABORT,
  ABORT_PSIST,
  ABORT_MSG_TOO_LONG,
  ABORT_NO_ACC_PARA,

  #ifdef FEATURE_IS2000_REL_A_CC
  ABORT_NO_EA_PARA,
  ABORT_NO_ACCEPTED_RATE_WORD,
  ABORT_UNSUPPORTED_PARAMETER,
  #endif /* FEATURE_IS2000_REL_A_CC */

  ABORT_XLATE_ERROR,
  ABORT_HO_LIST,

  #ifdef FEATURE_IS2000_REL_A
  ABORT_ACCT_BLOCK,
  #endif /* FEATURE_IS2000_REL_A */

  ABORT_SRCH_REQUEST_SYS_RESTART,
  ABORT_NUM_REASONS

} access_result_type;

/* ----------------------------------
** Substates of Traffic Channel State
** ---------------------------------- */
typedef enum
{
  CDMA_TCI = CDMA_TC,            /* Traffic Channel Initialization */
  CDMA_TRAFFIC,                  /* Traffic State */
  CDMA_RELEASE                   /* Release */

} cdma_tc_state_type;

/* ----------------------------------
** Substates of a Call Control Instance in Traffic Channel State
** These used to be part of the TC state, With Release A and
** concurrent services , we can have multiple call control instances
** and hence each of them can be in one of the following states
** ---------------------------------- */
typedef enum
{
  CDMA_CC_WFO = CDMA_CC,  /* Waiting For Order */
  CDMA_CC_WFA,            /* Waiting for Mobile Station Answer */
  CDMA_CC_CONV,           /* Conversation */
  CDMA_CC_RELEASE         /* Release */

} cdma_cc_state_type;

/* ----------------------------------
** Substates of VOIP Handoff State
** ---------------------------------- */
typedef enum
{
  CDMA_WAIT_FOR_SRDM = CDMA_VOIP_HO,   /* Wait for an SRDM from CM */
  CDMA_SRCH_GET_TIMING,                /* Wait for SRCH to acquire the timing.  
                                          Send the Origination Message. */
  CDMA_WAIT_FOR_UHDM,                  /* Wait for a UHDM to be received from bs */
  CDMA_PREPARE_FOR_TCH,                /* Prepare to go to the Traffic Channel */
  CDMA_TC_INIT,                        /* Traffic Channel Initialization */
  CDMA_WAIT_FOR_RESOURCES              /* Wait for CM to end the VOIP call and
                                          tell the lower layers to go to TCH */
} cdma_voip_ho_state_type;

/* ----------------------------------
** Powerdown registration types
** ---------------------------------- */

typedef enum
{
  CDMA_NORMAL,
  CDMA_LPM,
  CDMA_NAM_CHANGE,
  CDMA_SHUTDOWN

} cdma_powerdown_reg_type;

/* ------------------------------
** IMSI types
** ------------------------------ */
typedef enum
{
  CDMA_IMSI_M,
  CDMA_IMSI_T

} cdma_imsi_type;

/* ----------------------------------
** Various Mobile Handoff types
** ---------------------------------- */
typedef enum
{
  CDMA_IDLE_HO,
  CDMA_AEHO,
  CDMA_AHO,
  CDMA_APHO,
  CDMA_RAHO,       /* For event reporting only */
  CDMA_FAST_RAHO   /* For event reporting only */

} cdma_ho_type;

// This new struct was introduced for FEATURE_IS2000
// All code channels are now replaced by this struct
// For bw compatbility the QOF mask Id can be set to 0
typedef struct
{
  word code_chan;
  byte qof_mask_id;

} cdma_walsh_fn_type;

/* Use an internal data type to refer to Fields related to
   CHIND data struct */
typedef caii_ext_chnasn_pilot_info_type mccdma_pilot_info_type;

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

/* Status of transceiver request */
typedef enum
{
  MCC_TRANSCEIVER_RESOURCE_GRANTED, /* Granted RF resource */
  MCC_TRANSCEIVER_REQUEST_TIMEOUT,  /* Timeout on RF resource request */
  MCC_TRANSCEIVER_RESOURCE_LOST     /* Lost RF resource */

} mcc_transceiver_request_status_type;

/* -----------------------------
** Message received channel type
** ----------------------------- */
typedef struct
{
  word                   cdmach;
  cai_rx_channel_id_type chan_type;
  byte                   chan_id;
} cdma_msg_rxed_chan_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Miscellaneous definintions                                              */

/* Pseudo Pilot PN for other technology -> 1x handover. Pilot PN is 9-bit, 
   0xffff will not collide with a valid pilot PN value.*/ 
#define PSEUDO_PILOT_PN 0xffff

/* Maximum number of groups */
#define BAND_INDEX_NA 0xFF     /* Band Index Not Applicable */

#define MAX_BAND_GROUPS 2

#define PRI_PAGECH 1
  /* The primary paging channel (IS-95 section 7.1.3.4) */

#ifdef FEATURE_IS2000_REL_A_CC
#define PRI_FCCCH 1
  /* Designated arbitrary default value for forward common control channel */
#endif /* FEATURE_IS2000_REL_A_CC */

#define CHANGE_PAGE_CHAN_WAIT 80
  /* Time (in ms) after searcher is told to change paging channels
     at which we can be sure the new mask is in effect */

#define INVALID_TMSI_CODE 0xFFFFFFFFUL
  /* Value used to indicate invalid TMSI code */

/* Designated arbitrary default values for MCC and IMSI_11_12              */
#define IMSI_S1_ZERO 0xF9EBE7L
#define IMSI_S2_ZERO 999
#define IMSI_MCC_ZERO 999
#define IMSI_11_12_ZERO 99

#define IMSI_CLASS_0_ADDR_NUM 0xFF
#define IMSI_CLASS_1_ADDR_NUM_MAX 7

#define CLASS_1_IMSI(a)  \
((((byte) (a)) <= IMSI_CLASS_1_ADDR_NUM_MAX) ? TRUE : FALSE)
  /* cdma.imsi_addr_num (type byte) should be passed as "a". */

#define MCC_VALID_OVHD_TIMEOUT 30000
  /* Maximum time to obtain updated overhead messages arriving
     on the Paging Channel while operating in CDMA_IDLE_INIT state */

#ifdef FEATURE_IS2000_REL_D_NEG_SCI
#error code not present
#else
#define MCC_MIN_SCI    0
#endif

#define MCC_MAX_SCI    7
#define MCC_NULL_SCI  0x7f
#define MCC_DEFAULT_SCI 1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Constants for origination message translation                           */

#define MCC_DIAL_0    0xA
#define MCC_DIAL_STAR 0xB
#define MCC_DIAL_PND  0xC
#define MCC_DIAL_NULL 0x0

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                     BAND CLASS MASKS                                    */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*                   OPERATING MODE MASKS                                  */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#define J_STD_008_OP_MODE_MASK (1 << CAI_J_STD_008_OP_MODE) /* ANSI JSTD008 */
#define IS95_CDMA_OP_MODE_MASK (1 << CAI_IS95_CDMA_OP_MODE) /* IS-95 CDMA   */
#define IS95_ANAL_OP_MODE_MASK (1 << CAI_IS95_ANAL_OP_MODE) /* IS-95 analog */

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Specify which Band Class(es) and Operating Mode(s) is (are) supported   */
/* depending on which features are being compiled in.                      */
/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#ifdef FEATURE_ACP
#error code not present
#else
#define SUPPORTED_OP_MODES (IS95_CDMA_OP_MODE_MASK | J_STD_008_OP_MODE_MASK)          
#endif

/* Macros to check if the AMPS is supported for a given band class */
#define IS_AMPS_MODE_SUPPORTED (SUPPORTED_OP_MODES & IS95_ANAL_OP_MODE_MASK)

/*---------------MACRO for FEATURE_IS2000-----------------------*/
#define RC_CLASS(rc)  ( ((byte)rc > (byte)CAI_RC_2)? 2:1)
#define RATE_SET_OF_FWD_RC(rc) \
       ( (((byte)rc==(byte)CAI_RC_1)||((byte)rc==(byte)CAI_RC_3)||((byte)rc==4))? 0:1)
#define RATE_SET_OF_REV_RC(rc) \
       ( (((byte)rc==(byte)CAI_RC_1)||((byte)rc==(byte)CAI_RC_3))? 0:1)
#define RATE_SET_TO_MUX(rate_set) (rate_set+1)
#define MUX_TO_RATE_SET(mux) (mux-1)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Checks if the Band group is in the specified range
*/

#define MCC_BAND_GROUP_IN_RANGE(gp)  \
    ((gp > MCC_MIN_GROUP) && (gp < MCC_MAX_GROUP))

/* Band Groups. 
*/
typedef enum
{
  MCC_MIN_GROUP = -1,

  MCC_CELLULAR_GROUP,
  MCC_PCS_GROUP,

  MCC_MAX_GROUP

} cdma_band_class_group_type;

/* Band class table row entry
*/
typedef struct
{
  cdma_band_class_group_type band_group;
  uint8 num_bands;
  uint8 band_class[CAI_MAX_BAND_CLASSES];
} mcc_band_group_type;

/* Type which contains dialed digits                                       */

typedef struct
{
  byte digits[CM_MAX_DIALED_DIGITS];
    /* Dialed digits */
  byte num_digits;
    /* Number of digits in digits array */
  byte digits_sent;
    /* Indicator of how many digits were sent in the Origination Message
       or need to be sent during the Conversation state */
} cdma_dial_type;

#ifdef FEATURE_PASSPORT_MODE
/* CDMA message types */
typedef enum
{
  CDMA_MSG_TYPE_NONE = 0,
  CDMA_MSG_TYPE_REG = (1 << 0),
  CDMA_MSG_TYPE_NORMAL_ORM = (1 << 1),
  CDMA_MSG_TYPE_OTASP_ORM = (1 << 2),
  CDMA_MSG_TYPE_EMERG_ORM = (1 << 3),
  CDMA_MSG_TYPE_PAGE = (1 << 4),
  CDMA_MSG_TYPE_DBM = (1 << 5),
  CDMA_ALL = 0xFF
} cdma_msg_type;

/* Mask to store messages to be blocked */
extern byte cdma_block_action_mask;
#endif /* FEATURE_PASSPORT_MODE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* CHANNEL ASSIGNMENT and EXTENDED CHANNEL ASSIGNMENT MESSAGE (see IS-95B
 * Section 7.7.2.3.2.8/21). CAM and ECAM share this internal data structure
 * so the processing of both messages is the same.  ECAM is basically a super
 * set of CAM, with assign_mode 100 and 101 of CAM mapped to assign_mode 00
 * and 01 of ECAM.*/

/* CAM assign_mode 000 and 100, ECAM assign_mode 00 and 100*/
typedef struct
{
  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  boolean freq_incl;            /* Frequency included indicator */
  byte default_config;          /* Default configuration */
  boolean bypass_alert_answer;  /* Bypass indicator */
  byte num_pilots;              /* Number of pilot records following */
                                /* Please note that in IS95-B std, this field is the
                                 * actual pilot number - 1, but here, num_pilot is
                                 * the actual number of pilot to be compatible with
                                 * CAM implementation */
  byte granted_mode;            /* Granted mode */
  word cdma_freq;               /* Frequency assignment (only present if
                                   freq_incl field is set to 1) */
  byte band_class;              /* Band class */
  byte frame_offset;            /* Frame offset */
  byte encrypt_mode;            /* Message encryption mode */

  #ifdef FEATURE_IS2000_REL_A
  byte d_sig_encrypt_mode;      /* Signaling Encryption Mode */
  byte key_size;                /* Key Size */
  boolean c_sig_encrypt_mode_incl; /* Common Channel Encrypt Mode */
  byte c_sig_encrypt_mode;      /* Common Channel Encrypt Mode */
  boolean x3fl_1xrl_incl;       /* SR3 center freq included */
  byte x1rl_freq_offset;        /* SR3 center freq */
  #endif /* FEATURE_IS2000_REL_A */

  /* Active Set Pilot record information */
  //The following pilot rec is used becos it is a superset of all other pilot rec types
  mccdma_pilot_info_type pilot_rec[CAI_EXT_CHNASN_AM4_MAX];
  cai_radio_config_type for_rc; //RC of the forward CH
  cai_radio_config_type rev_rc; // RC of the reverse CH
  cai_chind_type ch_ind;
  byte fpc_subchan_gain; //FPC subchan gain for FFPC
  byte rl_gain_adj; // Not used for the time being, phase 2 stuff 4-bit 2's compliment
  byte fpc_fch_init_setpt; // Init setpt for FCH for FFPC
  byte fpc_fch_fer; // Target FER for puter loop forward power control
  byte fpc_fch_min_setpt; // This and the following parameter given the setpt range
  byte fpc_fch_max_setpt;
  byte fpc_dcch_init_setpt; // Init setpt for DCCH  for FFPC
  byte fpc_dcch_fer; // Target FER for puter loop forward power control
  byte fpc_dcch_min_setpt; // This and the following parameter given the setpt range
  byte fpc_dcch_max_setpt;
  boolean fpc_pri_chan; // Set to "0" i.e FCH till DCCH is supported

  #ifdef FEATURE_IS2000_REL_A
  boolean x3_fch_info_incl;
  caii_ext_ch_3x_fch_type fch_info[CAI_EXT_CHNASN_AM4_MAX];
  boolean x3_dcch_info_incl;
  caii_ext_ch_3x_dcch_type dcch_info[CAI_EXT_CHNASN_AM4_MAX];
  #endif /* FEATURE_IS2000_REL_A */

  boolean rev_fch_gating_on; //Indicates if 8th rate gating is on or off */
  boolean rev_pwr_cntl_delay_incl; /* Is Rev pwr cntl delay incl */
  byte rev_pwr_cntl_delay; /* Rev pwr Cntl delay */

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)  
  boolean plcm_type_incl;    /* Public Long Code Mask type included indicator */
  byte plcm_type;            /* Public Long Code Mask type indicator */
  qword plcm_39;             /* 39 LSB bits of Public Long Code Mask */
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

  #ifdef FEATURE_IS2000_REL_C
  byte sr_id_restore;        /* Service reference identifier to be restored */
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)  
  boolean msg_int_info_incl; /* Message integrity information included
                                indicator */
  boolean change_keys;       /* Change key indicator */
  boolean use_uak;           /* Use UAK indicator */
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

  #ifdef FEATURE_IS2000_REL_C
  byte ext_ch_ind;                     /* Extended channel indicator */

  boolean full_ci_feedback_ind;        /* Full C/I feedback rate indicator */
  byte rev_cqich_frame_offset;         /* Reverse channle quality indicator channel
                                          frame offset */
  byte rev_cqich_reps;                 /* Reverse channel quality indicator channel
                                          repetition factor */
  byte rev_ackch_reps;                 /* Reverse acknowledgment channel repetition
                                          factor */
  byte for_pdch_rc;                    /* Forward packet data channel radio
                                          configuration */
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #ifdef FEATURE_IS2000_REL_C
  boolean pdch_group_ind_incl;         /* Packet data channel indicator
                                          inclusion flag */
  boolean for_pdch_parms_incl;         /* Indicator of the inclusion of forward
                                          packet data channel configuration
                                          fields */
  boolean for_pdch_rlgain_incl;        /* F-PDCH parameters related to reverse
                                          link adjustment gains included 
                                          indicator */
  byte rlgain_ackch_pilot;             /* R-ACKCH to pilot adjustment gain */
  byte rlgain_cqich_pilot;             /* R-CQICH to pilot adjustment gain */
  byte num_soft_switching_frames;      /* Number of frames for R-CQICH soft
                                          switching */
  byte num_softer_switching_frames;    /* Number of frames for R-CQICH softer
                                          switching */
  byte num_soft_switching_slots;       /* Number of slots per frame for R-CQICH soft
                                          switching */
  byte num_softer_switching_slots;     /* Number of slots per frame for R-CQICH
                                          softer switching */
  boolean chm_switching_params_incl;   /* Control hold mode fields inclusion
                                          indicator */
  byte num_soft_switching_frames_chm;  /* Number of frames for R-CQICH soft
                                          switching while in control hold */
  byte num_softer_switching_frames_chm;/* Number of frames for R-CQICH
                                          softer switching while in control
                                          hold */
  byte num_soft_switching_slots_chm;   /* Number of slots per frame for
                                          R-CQICH soft switching while in
                                          control hold */
  byte num_softer_switching_slots_chm; /* Number of slots per frame for
                                          R-CQICH softer swicthing while in
                                          control hold */
  byte pdch_soft_switching_delay;      /* F-PDCH soft switching delay */
  byte pdch_softer_switching_delay;    /* F-PDCH softer switching delay */
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

} mccdma_chnasn_am0_type;

/* CAM assign_mode 001 and 101, ECAM assign_mode 01 */
typedef struct
{
  boolean respond;       /* Respond on new Access Channel indicator */
  boolean freq_incl;     /* Frequency included indicator */
  byte band_class;       /* Band class */
  word cdma_freq;        /* Frequency assignment (only present if
                            freq_incl field is set to 1) */
  word num_pilots;       /* Number of entries in pilot_pn array */
                         /* Please note that in IS95-B std, this field is the
                          * actual pilot number - 1, but here, num_pilot is
                          * the actual number of pilot to be compatible with
                          * CAM implementation */
  word pilot_pn[CAI_EXT_CHNASN_AM1_MAX]; /* Pilot PN sequence offset index */

} mccdma_chnasn_am1_type;

/* CAM assign_mode 010, ECAM assign_mode 10 */
typedef struct
{
  boolean respond;        /* Respond on analog control channel indicator */
  byte analog_sys;        /* System indicator */
  boolean use_analog_sys; /* Use analog system indicator */
  byte band_class;        /* Band class */

} mccdma_chnasn_am2_type;

/* CAM assign_mode 011, ECAM assign_mode 11 */
typedef struct
{
  word sid;              /* System identification of the analog system */
  byte vmac;             /* Voice mobile station attenuation code */
  word analog_chan;      /* Voice channel number */
  byte scc;              /* SAT color code */
  boolean mem;           /* Message encryption mode indiator */
  byte an_chan_type;     /* Analog voice channel type */
  byte band_class;       /* Band class */

} mccdma_chnasn_am3_type;

/* Union of Channel Assignment messages */
typedef union
{
  mccdma_chnasn_am0_type am0;
  mccdma_chnasn_am1_type am1;
  mccdma_chnasn_am2_type am2;
  mccdma_chnasn_am3_type am3;

} mccdma_ca_ord_type;

/* Channel Assignment message type */
typedef struct
{

  byte msg_type;     /* Message type */
  caii_pc_hdr_type hdr;  /* Paging Channel common header fields */
  byte assign_mode;      /* Assignment mode */
  mccdma_ca_ord_type mode; /* Union for different assign modes */

} mccdma_chnasn_type;

/* This data struct is used for temporarily storing forward pwr ctrl info
   till the action time is reached, so that it can be updated in
   the non negotiable SCR */
typedef struct
{
  byte trans_num;             /* Trans num of this cmd given to lower layers */
  byte pwr_cntl_step;         /* Pwr Control Step */
  boolean fpc_incl;           /* Is FPC values included */
  cai_fpc_mode_type fpc_mode; /* FPC mode */
  boolean fpc_pri_chan;       /* FPC primary channel */
  boolean fpc_olpc_fch_incl;  /* Is FCH outer loop params included */
  byte fpc_fch_fer;           /* FCH target FER */
  byte fpc_fch_min_setpt;     /* FCH min setpt */
  byte fpc_fch_max_setpt;     /* FCH max setpt */
  boolean fpc_olpc_dcch_incl; /* Is DCCH outer loop parameters included */
  byte fpc_dcch_fer;          /* DCCH target FER */
  byte fpc_dcch_min_setpt;    /* DCCH min setpt */
  byte fpc_dcch_max_setpt;    /* DCCH max set point */

} mccdma_pwr_ctrl_info_type;

#ifdef FEATURE_IS2000_REL_B
typedef struct
{
  /* The following variables are needed to support the CDMA Off Time
     Reporting feature. */
  boolean cdma_off_time_supported;  /* Off Time Report Message supported */
  word cdma_off_time_threshold;     /* Off Time Report threshold (in ms) */
  word srch_off_time_estimate;      /* SRCH's estimate of the search time */
  qword cdma_off_time_timestamp;    /* Time at which the off-time starts */
  boolean sent_off_time_message;    /* Indicates whether an Off Time Msg was sent */
} mccdma_off_time_type;

/* This struct contains info that may be needed for TC to Idle transition */
typedef struct
{
  boolean release_to_idle_ind; /* Indicates if Rel to Idle is allowed */
  /* Std added this field only in REL C but internally it is useful 
   * even if REL C is not defined */
  caix_chan_type idle_ovhd_chan; /* Ovhd channel MS was idling on*/
  uint8 idle_band_class;         /* Band class in the idle state */
  uint16 idle_cdmach;            /* CDMA FREQ when in the Idle state */
  uint16 idle_sid;               /* SID when in the idle state */
  uint16 idle_nid;               /* NID when in the idle state */
  uint8  idle_bs_p_rev;          /* P_REV of the BS when in the idle state */
  boolean idle_td_supported;
  /* Note : Other info like PCH and BCCH info is also needed for this but they are
     already stored in the "cdma" struct and hence not under this feature */

} mccdma_stored_tc_to_idle_info_type;
#endif

#ifdef FEATURE_IS2000_REL_C
typedef struct
{
  boolean   info_valid;
  uint8     release_type;
  uint16    sid;
  uint16    nid;
  boolean   freq_incl;
  uint16    cdma_freq;
  uint8     band_class;
  uint8     page_ch;
  uint8     prat;
  uint8     sr1_bcch_code_chan_non_td;
  boolean   sr1_crat_non_td;
  uint8     sr1_brat_non_td;
  uint8     sr1_td_mode;
  uint8     sr1_bcch_code_chan_td;
  boolean   sr1_crat_td;
  uint8     sr1_brat_td;
  boolean   num_pilots_d2i_incl;
  uint8     num_pilots_d2i;
  uint16    pilot_pn[CAI_MAX_NUM_D2I_PILOTS];

} mccdma_rxed_tc_to_idle_info_type;
#endif

/* ----------------------------------------------------------
** Private_lcm_active indicates whether the mobile is using   
** the public long code mask or the private long code mask.  
** If the mobile is using public, public_lcm_type indicates
** which public type it is using.
** Bs_plcm_39 holds the plcm_39 sent by the base station if
** the mobile is using the base station-specified plcm.
** Note that we do not store the long code mask itself, we 
** generate it as needed.
** ---------------------------------------------------------- */
typedef struct
{
  cai_plcm_type public_lcm_type;

  #if defined(FEATURE_IS2000_REL_C) || defined(FEATURE_MEID_SUPPORT)
  qword bs_plcm_39;
  #endif /* FEATURE_IS2000_REL_C || FEATURE_MEID_SUPPORT */

  #ifdef FEATURE_AUTH
  boolean private_lcm_active;
  #endif /* FEATURE_AUTH */

} mccdma_long_code_type;

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif  /* FEATURE_IS2000_REL_C_AKA */

#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains Mobile Station specific variables                   */

/* ----------------------------------------------
** Many of these fields are to store CDMA Numeric
** Information defined in IS-95A section 1.1.2.2
** ---------------------------------------------- */
typedef struct
{
  word curr_state;     /* Current state of CDMA state machine */
  byte curr_nam;       /* Current NAM */
  boolean serv_neg;    /* TRUE  => service negotiation  */
                       /*  FALSE => service option negotiation */
  word so_req;         /* Service Option of outstanding request */
  word so_cur_pri;     /* Current Primary service option */
  word so_cur_sec;     /* Current Secondary sevice option */
  dword esn;           /* Electronic serial number */
  uint16 cdmach;       /* Current CDMA Channel number */
  mcc_entry_type *entry_ptr;
    /* Pointer to mode entry information received from main MC */

  /* Following are IMSI_O items */
  dword imsi_s1;       /* First part of Operational IMSI (IMSI_O_S1) */
  word imsi_s2;        /* Second part of Operational IMSI (IMSI_O_S2) */
  qword imsi_s;        /* IMSI_S1 and IMSI_S2 converted to a qword imsi_s */
  byte imsi_11_12;     /* Mobile Network Code (MNC) for IMSI_O address */
  word mcc;            /* Mobile Country Code (MCC) for IMSI_O address */
  byte imsi_addr_num;  /* Number of digits in NMSI - 4, or class 0 IMSI flag */

  /* Following are IMSI_M items.  */
  dword imsi_m_s1;       /* First part of MIN based IMSI (IMSI_M_S1) */
  word imsi_m_s2;        /* Second part of MIN based IMSI (IMSI_M_S2) */
  qword imsi_m_s;        /* IMSI_M_S1 and IMSI_M_S2 converted to a qword */
  byte imsi_m_11_12;     /* Mobile Network Code (MNC) for IMSI_M address */
  word imsi_m_mcc;       /* Mobile Country Code (MCC) for IMSI_M address */
  byte imsi_m_addr_num;  /* Number of digits in NMSI - 4,or class 0 IMSI flag*/

  /* Following are IMSI_T items.  */
  dword imsi_t_s1;       /* First part of True IMSI (IMSI_T_S1) */
  word imsi_t_s2;        /* Second part of True IMSI (IMSI_T_S2) */
  qword imsi_t_s;        /* IMSI_T_S1 and IMSI_T_S2 converted to a qword */
  byte imsi_t_11_12;     /* Mobile Network Code (MNC) for IMSI_T address */
  word imsi_t_mcc;       /* Mobile Country Code (MCC) for IMSI_T address */
  byte imsi_t_addr_num;  /* Number of digits in NMSI - 4,or class 0 IMSI flag*/

  cdma_imsi_type imsi_type; /* indicator of the type of IMSI_O */

  boolean mob_term_home;
    /* Indicator of whether phone is configured to receive calls when using
       a home SID,NID pair */
  boolean mob_term_for_sid;
    /* Indicator of whether phone is configured to receive calls as a
       foreign SID roamer */
  boolean mob_term_for_nid;
    /* Indicator of whether phone is configured to receive calls as a
       foreign NID roamer */
  byte accolc;         /* Access overload class */

  #ifdef FEATURE_SSPR_ENHANCEMENTS
  nv_sid_nid_pair_type pair[NV_MAX_HOME_SID_NID]; /* SID,NID pairs for MIN */
  #else
  nv_sid_nid_pair_type pair[NV_MAX_SID_NID];      /* SID,NID pairs for MIN */
  #endif

  dword imsi_hash_key;/* MIN based hash key */
  byte mob_cai_rev;   /* Protocol revision number of the mobile station */
  byte mob_model;     /* Model number of the mobile station */
  word mob_firm_rev;  /* Firmware revision number for mobile station */
  byte scm;           /* Station class mark */
  word code_chan;
    /* Code channel for channel assignment or CDMA-to-CDMA handoff */

  #ifdef FEATURE_IS2000_REL_A_CC
  byte td_mode;
    /* SR1 TD mode (OTD or STS) */
  byte td_power_level;
    /* SR1 TD transmit power level */
  byte bcch_td;
    /* SR1 TD CDMA Broadcast Control Channel walsh */
  byte bcch_non_td;
    /* SR1 Non-TD CDMA Broadcast Control Channel walsh */
  cai_bcch_rate_type brat_td;
    /* SR1 TD Broadcast Control Channel data rate */
  cai_bcch_rate_type brat_non_td;
    /* SR1 Non-TD Broadcast Control Channel data rate */
  cai_code_rate_type bcch_code_rate_td;
    /* SR1 TD Broadcast Control Channel code rate */
  cai_code_rate_type bcch_code_rate_non_td;
    /* SR1 Non-TD Broadcast Control Channel code rate */
  byte bcch;
    /* Current CDMA Broadcast Control Channel walsh */
  cai_bcch_rate_type brat;
    /* Current Broadcast Control channel data rate */
  cai_code_rate_type bcch_code_rate;
    /* Current Broadcast Control channel code rate */
  byte fccch_id;
    /* Current CDMA Forward Control Channel number */
  byte num_fccch_s;
    /* Number of Forward Common Control Channels on current CDMA channel */
  #endif /* FEATURE_IS2000_REL_A_CC*/

  byte pagech;
    /* Current CDMA Paging Channel number */
  byte page_chan_s;
    /* Number of Paging Channels on current CDMA channel */
  cai_pch_rate_type prat;
    /* Current CDMA Paging channel data rate */
  int8 slot_cycle_index_p;
    /* SLOT_CYCLE_INDEXp - the slot cycle index prefered by user */

  #ifdef FEATURE_MODIFIED_SCI
#error code not present
  #endif /* FEATURE_MODIFIED_SCI */

  uint16 idle_pgslot;
  /* Hashed Paging Channel slot (0-2047).  Used on the Traffic Channel. */
  
  int8 last_sci_reg;
    /* The last SCI value that MS registered with the BS */  
  int8 curr_sci_pref;
    /* The current slot cycle index prefered by upper layer */

  mccdma_chnasn_type chnasn;
    /* Channel assignment message received during the Idle state or System 
       Access state and retained for use in TC state*/
  boolean call_orig;
    /* Indicator of whether the call in progress is mobile originated (TRUE)
       or mobile terminated (FALSE) */
  boolean rel_orig;
    /* Indicator of whether the user initiated the call release (TRUE)
       or the release was initiated by the base station (FALSE) */
  byte reg_type;
    /* Type of registration required -- this field must be set when the
       next state is CDMA_REG - the codes are defined in the IS-95 spec */
  cdma_powerdown_reg_type powerdown_reg_type;
    /* type of powerdown registration required- whether from powerdown
       or from lpm */
  word sa_tc_state;
    /* State that should be entered after Update Overhead Information substate */
  cdma_ho_type ho_state;
    /* Keeps track of various Handoff states */
  word ho_sa_tc_state;
    /* This holds the state that should be entered after Update Overhead Information
       substate during Access Entry handoff */
  byte sa_resp_msg;
    /* Message to be sent, during order/message response substate, in
       response to a received message */
  caii_ac_ord_type sa_resp_ord;
    /* Specific order to transmit when the sa_resp_msg is an Order Message */

  boolean digit_mode;      /* Identifies between DTMF and ASCII call type */
  byte number_type;        /* Number type for an ASCII mode */
  byte number_plan;        /* Number plan for an ASCII mode */
  cdma_dial_type dial;     /* Structure containing digits dialed by user */
  byte remaining_digits;   /* Number of digits that didn't fit in origination */
  byte frame_offset;       /* Current Traffic Channel frame offset, in units of 1.25 msec */
  byte hdm_seq;            /* Handoff Direction Message sequence number */

  boolean vp_desired;      /* Indicates if the user wants voice privacy.  This 
                              is the value of the pm bit in a page response or 
                              origination */
  byte encrypt_mode;       /* Indicates if message encryption is on */
  byte auth_mode;          /* Indicates if authentication data sent on access */
  byte band_class;         /* CDMA band class (cellular or PCS) */
  nv_block_or_sys_enum_type block_or_sys; /* Cellular serv-sys (A or B) or
                                             PCS frequency block (A-F)   */
  #ifdef FEATURE_AUTH
  dword rand;          /* Value of RAND used in last computation of AUTHR for 
                          registrations */
  boolean authr_valid; /* Signifies whether the authr value can be used */
  dword authr;         /* Value of AUTHR used is registration messages */
  dword randu;         /* Random value received in Auth Challenge Order */
  #endif /* FEATURE_AUTH */

  caii_tmsi_data_type assigning_tmsi;
  byte tmsi_code_len;
  boolean tmsi_full_timer_set;

  #ifdef FEATURE_OTASP
  otaspi_mob_dir_data_type mob_dir; /* Mobile Directory Number */

  #ifdef FEATURE_UIM_RUIM
  byte spc_status;
  #endif /* FEATURE_UIM_RUIM */

  #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte spc[NV_SEC_CODE_SIZE]; /* Service Programming Code */
  #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  otasp_call_type otasp_call; /* Indicates service provisioning type */

  #ifdef FEATURE_UIM_RUIM
    /* OTA capability features supported by the card */
  ruim_otasp_features_type ruim_ota_features;
  #endif /* FEATURE_UIM_RUIM */

  #ifdef FEATURE_OTASP_OTAPA
  boolean otapa_enabled; /* Indicate if OTAPA is enabled for current NAM */
  boolean nam_lock;      /* Indicates if the current NAM is protected by SPASM */
  #endif /* FEATURE_OTASP_OTAPA */

  #endif /* FEATURE_OTASP */

  dword auth_data;            /* auth data for a mobile originated call */
  boolean spc_change_enabled; /* indicates if user has enabled the spc change */
  byte p_rev_in_use;          /* P_REV_IN_USE */
  byte pwr_cntl_step;         /* Closed loop power control step size */
  byte fpc_subchan_gain;      /* FPC subchan gain */
  byte end_reason;            /* reason for ending a call */
  word t_slotted;             /* Slotted timer value */
  boolean  rev_fch_gating_on; /* Rev FCH 8th-rate gating mode on indicator */
  byte  rev_pwr_cntl_delay;   /* Rev pwr ctrl delay */
  boolean drs;                /* DRS bit in Origination message */
  byte call_id;               /* Temporary storage for call_id */

  #ifdef FEATURE_IS2000_REL_A
  boolean orig_reason;            /* Indicates if silent redial due to L2 failure. */
  byte orig_count;      
    /* If orig_reason is 1: Number of silent redials due to L2 failure; 
       Else, Number of silent redials for the desired service option that have
       failed since the last successful connection (not implemented). */
  boolean is_packet_data;         /* indicates if packet data origination */
  boolean is_dormant_packet_data; /* indicates if dormant packet data */
  boolean is_emergency;           /* if this is an emergency call */
  boolean cs_supported;           /* Indicates if Concurrent Services is supported by BS */
  boolean mob_qos;                /* Permission indicator for Qos in Origination Message*/
  boolean qos_parms_incl;         /* Qos Parameters indicator */
  byte qos_parms_len;
  byte qos_parms[CAI_ORIG_QOS_LEN_MAX];
  boolean is_sr_id_valid;         /* Indicates if SR_ID is valid */
  byte sr_id;                     /* SR_ID for the 1st call */
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_A_BSID
  boolean pilot_info_req_supported; /* Pilot information request supported indicator */
  #endif /* FEATURE_IS2000_REL_A_BSID */


  #if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
  byte mo_dbm_type;  /* mobile orig'ed data burst msg type */
  void *dbm_buf_ptr; /* pointer to the MO DBM data */
  boolean dbm_wait_for_l2_ack;
          /* Indicates if MC is waiting for layer 2 ack for an MO DBM request */
          /* It is set to TRUE if it is waiting for L2 ack                    */
          /* It is set to FALSE if it is *not* waiting for L2 ack.            */
          /* At a time, MC can wait for either SMS, PD or SDB L2 ack only.     */
  #endif /* FEATURE_CDSMS || FEATURE_DBM */

  #ifdef FEATURE_UIM_RUIM
  dword esn_me;             /* ESN_ME */

  #if defined(FEATURE_MEID_SUPPORT) && defined(FEATURE_HWID_SUPPORT)
  qword meid_me;            /* MEID_ME */
  #endif /* FEATURE_MEID_SUPPORT && FEATURE_HWID_SUPPORT*/
  
  boolean ruim_id_avail;    /* Indicates if R-UIM ID is available */
  dword ruim_id;            /* R-UIM ID */
  #endif /* FEATURE_UIM_RUIM */

  #if (defined(FEATURE_DDTM_CNTL) && defined(FEATURE_HDR))
#error code not present
  #endif /* (defined(FEATURE_DDTM_CNTL) && defined(FEATURE_HDR)) */

  boolean is_cdma_subs_avail; /* current CDMA/AMPS/HDR subscription
                                 availability status */

  boolean transceiver_request_pending;

  /* Set/Reset ONLY by ITSPM, UHDM. Used ONLY in traffic state. 
     It indicates whether BS supports SDB on traffic channel.  */
  boolean sdb_supported; 

  mccdma_long_code_type long_code;

  boolean tc_tx_on;       /* Current status of the transmitter  on TC*/
  boolean tc_tx_to_be_on; /* Indicates the new state of the transmitter */

  #ifdef FEATURE_MEID_SUPPORT
  qword meid;             /* Mobile Equipment Identification Number */
  unsigned long  meidhash[5];     /* MEID SHA-1 Hash value */
  #endif /* FEATURE_MEID_SUPPORT */

  #ifdef FEATURE_IS2000_REL_B
  /* The following variables are needed to support the REL_B TC to Idle 
    transition */
  mccdma_stored_tc_to_idle_info_type tc_to_idle_info;
  /* The following variables are needed to support the CDMA Off Time
     Reporting feature. */
  mccdma_off_time_type off_time_info;
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C 
  boolean chm_supported; /* Indicates if 1x control hold is supported by BS*/
  boolean for_pdch_supported; /* Indicates if DV is supported by BS */
  mccdma_rxed_tc_to_idle_info_type rxed_tc_to_idle_info; /* Indicates Directo to Idle
                                  info, if received */                  
  boolean use_sync_id; /* Indicates if BS supports SYNC ID while in traffic */
  #endif /* FEATURE_IS2000_REL_C */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
  #endif /* FEATURE_IS2000_REL_C_AKA */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

  #ifdef FEATURE_IS2000_REL_A_AES
#error code not present
  #endif /* FEATURE_IS2000_REL_A_AES */

  byte sa_msg_tx_msg; /* this is the message type to be sent in CDMA_MSG */

  /* ---------------------------------------------------------------------------
  ** The size of below members varies with whether JCDMA is supported or not. 
  ** Important !!! Please do not add new member that has no size dependency on 
  ** JCDMA after them. 
  ** --------------------------------------------------------------------------- */
  mc_calling_party_number_type  calling_party_number;  /* Calling party number record */
  mc_subaddress_type            calling_party_subaddr; /* Calling party subaddress record */
  mc_subaddress_type            called_party_subaddr;  /* Called Party subaddress record */

} cdma_ms_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure to hold the intermediate channel list on receiving the ECCLM */
typedef struct
{
  byte cdma_band;         /* CDMA band */ 
  word cdma_freq;         /* CDMA frequency */
  byte cdma_freq_weight;  /* CDMA frequency weight */
  byte ecclm_band_index;  /* Index to ECCLM num_band loop in ECCLM */
  byte ecclm_freq_index;  /* Index to ECCLM num_freq/num_add_freq */
} cdma_int_chn_list_type; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains message sequence numbers and message receipt flags  */

typedef struct
{
  qword msg_time;
    /* Time of receipt of most recently received message for this base
       station */
  boolean sy_rx;
    /* indicator of whether valid Sync channel information is in the
       sy structure */
  boolean sp_rx;
    /* System Parameter Message received */
  boolean nl_rx;
    /* Neighbor List Message received */
  boolean cl_rx;
    /* CDMA Channel List Message received */
  boolean gnl_rx;
    /* Extended CDMA Channel List Message Received */
  boolean ecl_rx;

    /* Extended Global Service Redirection Message Received */
  boolean egsd_rx;
  boolean ap_rx;
    /* Access Parameter Message received */
  boolean esp_rx;
    /* Extended System Parameters Message received */
  boolean gsd_rx;
    /* Global Service Redirection Message received */

  #ifdef FEATURE_IS2000_REL_A_CC
  boolean a41spm_rx;
  /* ANSI_41 System Parameters Message received */
  boolean mcrrpm_rx;
  /* MC-RR Parameters Message received */
  boolean unlm_rx;
  /* Universal Neighbor List Message */
  boolean eapm_rx;
  /* Extended Access Parameters Message */

  #ifdef FEATURE_AUTH
  boolean a41randm_rx;
  /* Ansi-41 RAND Message */
  #endif /* FEATURE_AUTH */
  #endif /* FEATURE_IS2000_REL_A_CC */

  byte config_msg_seq;
    /* Configuration Message sequence number which applies to the received
       System Parameter, Neighbor List, CDMA Channel List, Extended System
       Parameter and Global Service Redirection messages */
  byte acc_msg_seq;
    /* Access Parameter sequence number which applies to the received
       Access Parameter message */

} cdma_msg_rx_type;

#ifdef FEATURE_IS2000_REL_A_CC
typedef struct
{
  byte num_fccch;
  cai_fccch_rate_type fccch_rate;
  cai_code_rate_type  fccch_code_rate;
  byte                fccch_code_chan[CAI_MAX_NUM_FCCCH];
} cdma_fccch_info_type;

typedef struct
{
  byte                  num_bcch_bcast; /* Number of non-primary BCCH chans */
  struct 
  {
    byte                bcch_code_chan; /* walsh code index for the BCCH */
    cai_bcch_rate_type  brat;           /* BCCH data rate */
    cai_code_rate_type  bcch_code_rate; /* BCCH code rate */
  } bcch_info[CAI_MAX_NUM_BCCH_BCAST];

}cdma_bcast_bcch_info_type;
#endif /* FEATURE_IS2000_REL_A_CC */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enum used to indicate what message type to retrieve from BS Info array  */

typedef enum
{
  MC_SYNC_MSG = 0x10,
       /* a Sync Channel message type */
  MC_RX_MSG
    /* Message receipt structure */

} cdma_mc_msg_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   Types to store Neighbor Set and Active Set                            */

typedef struct
{
  word pilot_pn;  /* Neighbor pilot PNs */
  byte age;       /* AGE of neighbor */
  byte config;    /* Neighbor configuration */
  byte search_pri;     /* Pilot Channel search priority */
  boolean freq_incl;   /* Frequency included indicator */
  byte band;           /* Neighbor band class */
  word freq;           /* Neighbor frequency assignment */
  boolean access_entry_ho;    /* If Access Entry HO allowed */
  boolean access_ho_allowed;  /* If ACCESS HO and Access Probe allowed */
  byte  srch_win_nghbr;       /* Neighbor window for the pilot */
  boolean timing_incl;        /* If Timing included for the Neighbor */
  byte nghbr_tx_offset;       /* Neighbor TX offset */
  byte nghbr_tx_duration;     /* Neighbor TX Duration */
  byte nghbr_tx_period;       /* Neighbor TX PERIOD */
  boolean add_pilot_rec_incl;
  cai_pilot_rec_type nghbr_pilot_rec_type;
  caii_add_pilot_rec_type nghbr_pilot_rec;
  byte srch_offset_nghbr;

  #ifdef FEATURE_IS2000_REL_A
  boolean bcch_supported;   /* If BCCH is supported by this neighbor */
  #endif

} cdma_nghbr_set_type;

typedef struct
{
  byte pilot_inc;       /* Current pilot increment between neighbors */
  byte nghbr_max_age;   /* Maximum AGE for neighbors */
  word num_nghbr_pns;   /* Number of PNs in the Neighbor Set */
  cdma_nghbr_set_type nset[SRCH_NSET_MAX];   /* Neighbor set */

} cdma_nghbr_type;

typedef struct
{
  uint8   band_class;
  uint8   pch;
  uint16  cdma_channel;
  uint16  pn;

} mccdma_pch_id_type;

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/* Data Structure that contains the complete system parameters of a BS
   i.e SPM, ESPM, MC-RR and ANSI-41 sys paramaters, Most of the MC-RR and
   ANSI_41 parameters are stored in sp and esp data structure,  The rest
   are stored as separate parameters in this data structure */
typedef struct
{
  caii_sysparm_type sp;
    /* Most recently received System Parameter Message */
  caii_ext_sysparm_type esp;
  word ota_mcc;               /* Over The Air MCC */
  byte ota_imsi_11_12;        /* Over The Air IMSI_11_12 */
    /* Most recently received Extended System Parameters Message */

  #ifdef FEATURE_IS2000_REL_A_CC
  byte qpch_code_chan[CAI_MAX_NUM_QPCH]; /* code channel index of the QPCH for SR3 */
  cdma_fccch_info_type fccch_info; /* FCCCH infomation */
  cdma_bcast_bcch_info_type bcast_bcch_info; /* Broadcast BCCH channel info */

  #ifdef FEATURE_AUTH
  boolean sending_a41_rand; /* ANSI-41 Rand Message expected to be sent */
  #endif /* FEATURE_AUTH */
  #endif /* FEATURE_IS2000_REL_A_CC */

} mcc_complete_sys_parms_type;

#ifdef FEATURE_AUTH
/* On the Paging Channel, authentication values (auth and rand) are contained in
   Access Parameters Message.  On BCCH, auth is set by sending_rand in MC-RR, while
   rand is contained in Ansi-41 RAND.  Store in one structure for easy access. */
typedef struct
{
  byte  auth;
  dword rand;

} mcc_rand_parms_type;
#endif /* FEATURE_AUTH */

typedef struct
{
  caix_chan_type ovhd_chan;
  byte ovhd_walsh_ch;
    /* Broadcast Control channel or Paging channel mobile has been
       assigned to on this base station. The base station can have different
       parameters for different channels */

  #ifdef FEATURE_IS2000_REL_B
  cai_pch_rate_type prat;
    /* Current CDMA Paging channel data rate */
  cai_bcch_rate_type  brat;           /* BCCH data rate */
  cai_code_rate_type  bcch_code_rate; /* BCCH code rate */
  #endif

} mcc_ovhd_chan_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains base station specific information                   */

typedef struct
{
  boolean valid_pn;
    /* Indicator of whether pilot_pn is valid
       (i.e. whether entry has been written to or not) */
  word pilot_pn;
    /* pilot pn offset */
  boolean td_supported ;
    /* Indicates if this freq supports TD, so that we can access the correct
     BCCH info from the SYNC message */

  #ifdef FEATURE_IS2000_REL_B
  /* We need complete TD info about this if we have to do a direct
  TC to idle transition */
  byte td_mode;
    /* SR1 TD mode (OTD or STS) */
  byte td_power_level;
    /* SR1 TD transmit power level */
  #endif

  cdma_msg_rx_type rx;
    /* structure containing information about which messages have been
       received */
  caii_sync_type sy;
    /* Most recently received Sync Channel message */
  caii_accparm_type ap;
    /* Most recently received Access Parameter Message */
  cdma_nghbr_type nl;
    /* Most recently received Neighbor List Message */
  caii_glob_redirect_type gsd;
    /* Most recently received Global Service Redirection Message */
  byte nl_config_msg_seq;
    /* Config Seq num of most recently received Neighbor List Message */
  caii_chnlist_type cl;
    /* Most recently received Channel List Message */
  byte gnl_config_msg_seq;
    /* Config msg sequence of General Neighbor List Message */
  caii_analog_nghbr_type anl;
    /* Analog Neighbor List as received part of GNLM */
  boolean setting_search_win;
    /* flag to indicate if the GNLM is received and SRCH WIN needs be be
       defaulted: A case where GNLM arrives before the SPM */

  /* Extended Global Service Redirection Message */
  caii_ext_glb_redir_type egsd;
  /* Extended Channel List Message */
  caii_ext_chnlist_type  ecl;

  #ifdef FEATURE_IS2000_REL_A_CC
  caii_en_ac_parm_type eapm;
   /* Enhanced access parameters message */
  byte a41spm_config_msg_seq;
  /*  config_msg_seq of ANSI 41 system parameters message */
  byte mcrrpm_config_msg_seq;
  /*  config_msg_seq of MC-RR parameters message */
  byte unlm_config_msg_seq;
  /*  config_msg_seq of UNLM  message */
  #endif /* FEATURE_IS2000_REL_A_CC */

  mcc_complete_sys_parms_type csp;
    /* Complete sys parameters; contains SP, ESP, MC-RR and ANSI-41 data */

  #ifdef FEATURE_AUTH
  mcc_rand_parms_type rp;
  #endif /* FEATURE_AUTH */

  byte band_class;
    /* Band class of the base station */
  word cdma_ch;
    /* CDMA channel base station is using */
  boolean ovhd_chan_info_valid;
    /*Info about Channel on which OVHDs are received */
  mcc_ovhd_chan_info_type ovhd_chan_info;

  mccqpch_channel_type qpch_num;
    /* Quick paging channel number.  The number is generated when needed so that
       we don't need to determine if the right messages have arrived.  When this
       information is needed, i.e. entering sleeping, we are sure that all messages
       are current. */
  dword imsi_hash_key;

  byte hashed_band;     /* CDMA hash band as received in ECCLM */
  word hashed_cdmach;   /* CDMA hash frequency as received in the CLM or ECLM */

} cdma_bs_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type which contains fields of a matched order (unbundled)               */

typedef struct
{
  byte msg_type;            /* Message type */
  caii_pc_hdr_type hdr;     /* Paging Channel common header fields */
  byte order;               /* Order code */
  byte ordq;                /* Order qualification code */
  dword authbs;             /* For BS Challenge Confirmation Order only */
  byte roam_indi;           /* Enhanced Roaming Indication for CAI_REG_ACC */

  #ifdef FEATURE_IS2000_REL_A
  byte c_sig_encrypt_mode;    /* Signaling Encryption Mode */
  byte key_size;            /* Key Size */
  #endif

  #ifdef FEATURE_IS2000_REL_C
  boolean msg_int_info_incl;/* Message integrity info incl. */
  boolean change_keys;      /* Change keys indicator */
  boolean use_uak;          /* Use UIM Authentication Key indicator */
  #endif /* FEATURE_IS2000_REL_C */

  byte retry_type;          /* Retry type for CAI_RETRY_ORDER */
  byte retry_delay;         /* Retry delay for CAI_RETRY_ORDER */

  #ifdef FEATURE_IS2000_REL_D
#error code not present
  #endif /* FEATURE_IS2000_REL_D */

} mccdma_pc_ord_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Union for received non-duplicate message addressed to the mobile        */

/* ------------------------------------------------------------------
** Note - The msg_type and hdr fields of all structures in this union
** can be accessed through the caii_gen_pc_type.  This union and the
** supporting order and channel assignment types allow processing of
** the layer 2 fields of a directed paging channel message to be done
** without reguard to the message type.
** ------------------------------------------------------------------ */

typedef union
{
  caii_gen_pc_type            gen_pc;
  mccdma_pc_ord_type          pc_ord; /* Defined above */
  mccdma_chnasn_type          chnasn; /* Defined above */
  caii_pc_tmsi_asgn_type      pc_tmsi;
  caii_pc_burst_type          pc_burst;
  caii_pc_auth_ch_type        pc_auth;
  caii_pc_ssd_type            pc_ssd;
  caii_pc_feature_type        pc_feat;
  caii_serv_redirect_type     serv_redir;
  caii_pc_streq_type          pc_streq;
  caii_paca_type              paca;

  #ifdef FEATURE_IS2000_REL_A
  caii_sec_mode_cmd_type  sec_mode_cmd;
  #endif

  #ifdef FEATURE_IS2000_REL_C
  caii_auth_req_type          auth_req;
  #endif

} mccdma_matched_msg_type;

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type containing fields for layer 2 processing of paging channel msgs    */

typedef struct
{
  byte msg_seq;
  byte ack_seq;
  byte ack_type;
  byte valid_ack;
  boolean idle_msg_pending;
  mccdma_matched_msg_type pending_idle_msg;

} mcc_layer2_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type and queues for DTMF command procesing                              */

typedef struct
{
  q_link_type link;        /* Queue link */
  mc_name_type dtmf_cmd;   /* Type of DTMF command received from UI */
  union
  {
    word ms;      /* Number of ms key depressed (MC_STOP_CONT_DTMF) */
    byte digit;               /* Digit to send (MC_START_CONT_DTMF) */
    mc_burst_dtmf_type burst; /* Burst DTMF data (MC_BURST_DTMF_F)  */
  } parm;
} cdma_dtmf_type;

extern q_type mcc_dtmf_q;
  /* queue to place outstanding dtmf commands */
extern q_type mcc_dtmf_free_q;
  /* queue to place free buffers to be placed on the mcc_dtmf_free_q */

/* Default value for prev_pilot                                            */
#define NO_PREV_PILOT -1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Type for Pilot Report Processing                                        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Structure to hold pilot reporting overhead information */

typedef struct
{
  word msg_len;
    /* Length of Access Channel message excluding pilot report information */
  byte num_pilots_allowed;
    /* Number of pilots that can be reported in Access Channel Msg */
  int2 first_pilot;
    /* Active pilot of first access probe */
  int2 prev_pilot;
    /* Active pilot for the immediately previous access probe */
  byte num_pilots_attempted;
    /* Number of pilots that has been access attempted */
  boolean first_probe;
    /* Flag if first access probe */
  boolean apho_rebuild;
    /* Flag if ACCESS_HO_LIST need to build because of access probe handoff */
  cdma_nghbr_set_type attempt_list[CAI_N13M];
    /* List of pilots that have been access attempted */

} plt_rpt_ovhd_rec_type;

/* Structure to hold Access Handoff List */

typedef struct
{
  int2 pilot_pn;
    /* ---------------------------------------------------------------------
    ** Pilot PN is used as a key to perform the record matching of Access
    ** Handoff List against the Searcher Pilot Strength Measurement report
    ** during update
    ** ---------------------------------------------------------------------*/
  int2    pilot_pn_phase;   /* Pilot pn phase */
  byte    pilot_strength;   /* Pilot strength */
  boolean access_attempted; /* Whether access is attempted */

} aho_pilot_info_type ;

typedef struct
{
  byte                num_pilots; /* number of pilots in the list */
  aho_pilot_info_type pilot_list[CAI_N13M];

} aho_list_type;

/* Structure to hold Other Reported List */

typedef struct
{
  int2    pilot_pn;
    /* -----------------------------------------------------------------
    ** Pilot PN is used as a key to perform the record matching of Other
    ** Reported List against the SRCH NSet during swapping
    ** ----------------------------------------------------------------- */
  int2    pilot_pn_phase;   /* Pilot pn phase */
  byte    pilot_strength;   /* Pilot strength */
} othrpt_pilot_info_type ;

typedef struct
{
 byte                   num_pilots; /* number of pilots in the list */
 othrpt_pilot_info_type pilot_list[CAI_MAX_ADD_PILOTS];
} othrpt_list_type;

#ifdef FEATURE_UIM_RUIM
  /* Buffer used to retrieve the result of R-UIM operations */
extern byte uim_access_buffer[UIM_MAX_CHARS];
#endif /* FEATURE_UIM_RUIM */

extern cdma_bs_type *mcc_saved_current_bs_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                    MACROS                                               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to determine if overhead channel is Broadcast Control Channel or
   Paging Channel */
#ifdef FEATURE_IS2000_REL_A_CC
#define OVHD_CHAN_IS_BCCH \
   ( cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH )
#endif /* FEATURE_IS2000_REL_A_CC */

#define OVHD_CHAN_IS_PCH \
   ( cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC )

/* Macro to determine if additional overhead messages are required before
   a System Access can be attempted */

#define INVALID_SEQ 0xff
#define TEMP_NL_SEQ 0x7f
  /* CONFIG_MSG_SEQ is 6 bits, so anything over 0x3f is invalid */

#define CLR_ACC_MSG_SEQ() \
    (cur_bs_ptr->rx.acc_msg_seq = INVALID_SEQ)

#define AP_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.ap_rx && ( cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq ) )

/* GNLM, NLM, SPM, ESPM, CLM, ECLM, AMS41SPM, MCRRPM, UNLM, GSRDM and EGSRDM are 
   considered as received if the rx flag of the overhead message is equal to TRUE and
   one of the following condition is fullfilled:
   1) rx.config_msg_seq is not equal to INVALID_SEQ and the overhead message's 
      config_msg_seq value is equal to rx.config_msg_seq.

   OR

   2) rx.config_msg_seq is equal to INVALID_SEQ and the overhead message's 
      config_msg_seq value is not equal to INVALID_SEQ. 

      (Notice: We get into this condition after idle handoff. After idle handoff, 
      we force MS to stay awake to wait for a GPM, UPM or any overhead message
      with config_msg_seq by changing the value of rx.config_msg_seq to 
      INVALID_SEQ instead of forcing the MS to wait for a GPM or UPM in assigned
      slot that could takes a long time which hurts standby time. After MS idle on
      the new pilot, the value of rx.config_msg_seq will be updated when a GPM, UPM
      or any ovehead with config_msg_seq is received.
*/  
 
#define GNLM_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.gnl_rx && ( cur_bs_ptr->gnl_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->gnl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define GNLM_MSG_NOT_RECVD  ( !GNLM_MSG_IS_RECVD )

#define NLM_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.nl_rx && ( cur_bs_ptr->nl_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->nl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define SP_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.sp_rx && ( cur_bs_ptr->csp.sp.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->csp.sp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define SP_MSG_NOT_RECVD ( !SP_MSG_IS_RECVD )

#define ESP_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.esp_rx && (cur_bs_ptr->csp.esp.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define ESP_MSG_NOT_RECVD ( !ESP_MSG_IS_RECVD )

#define NGHBR_LIST_MSGS_NOT_RECVD ( !NLM_MSG_IS_RECVD || !GNLM_MSG_IS_RECVD )

#define CHAN_LIST_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.cl_rx && ( cur_bs_ptr->cl.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->cl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define GSR_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.gsd_rx && ( cur_bs_ptr->gsd.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->gsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define EXT_CHAN_LIST_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.ecl_rx && ( cur_bs_ptr->ecl.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->ecl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#define EGSR_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.egsd_rx && ( cur_bs_ptr->egsd.config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->egsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )

#ifdef FEATURE_IS2000_REL_A_CC
#define ANSI41SP_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.a41spm_rx && ( cur_bs_ptr->a41spm_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->a41spm_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define ANSI41SP_MSG_NOT_RECVD ( !ANSI41SP_MSG_IS_RECVD )

#define MCRRP_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.mcrrpm_rx && ( cur_bs_ptr->mcrrpm_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->mcrrpm_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define MCRRP_MSG_NOT_RECVD ( !MCRRP_MSG_IS_RECVD )

#define UNIV_NGHBR_LIST_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.unlm_rx && ( cur_bs_ptr->unlm_config_msg_seq != INVALID_SEQ ) && \
      ( ( cur_bs_ptr->unlm_config_msg_seq == cur_bs_ptr->rx.config_msg_seq ) || \
        ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ ) ) )
#define UNIV_NGHBR_LIST_MSG_NOT_RECVD ( !UNIV_NGHBR_LIST_MSG_IS_RECVD )

#define ENH_ACC_PARM_MSG_IS_RECVD \
    ( cur_bs_ptr->rx.eapm_rx && ( cur_bs_ptr->eapm.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq ) )
#endif /* FEATURE_IS2000_REL_A_CC */

/* Macro to determine if a PN offset is valid */
#define VALID_PN( pn )  \
  ( cdma_nghbrs.pilot_inc ? ((pn % cdma_nghbrs.pilot_inc ) == 0 ) : FALSE )

/* Macro to determine if a code channel is valid */
#define VALID_CODE_CHN( chn)  \
  ( (chn > 0) && (chn < 128 ) ) //Range extended for FEATURE_IS2000
/* Macro to determine if a QOF mask id is valid */
#define VALID_QOF_MASK_ID( id)  \
  ( id < 4 ) //Range extended for FEATURE_IS2000

/* Macro to determine next ACK_REQ_SEQ to be sent to the
   base station */
/*
#define NEXT_ACK( ack_req ) \
  ( ( ack_req + 1) % 3 )
*/

/* Macro to find main state from state variable      */

#define MAIN_STATE(state)  (state & 0xFF00 )

typedef enum
{
  SLOTTED_PAGE,                   /* Slotted Page Message */
  NON_SLOTTED_PAGE,               /* Page Message */
  GENERAL_PAGE                    /* General Page Message */

} mcc_page_enum_type;

typedef struct
{
  byte ack_type;                  /* Ack type */
  byte msg_seq;                   /* msg_seq of page addressed to mobile */
  boolean special_service;        /* whether or not the page included SO */
  word service_option;            /* Service Option (SO) included */
  dword tmsi_code;                /* TMSI code from general page record */

} mcc_page_info_type;

/* Type to specify retry info */
typedef struct
{
  cai_retry_order_type  retry_type; /* Origination, Resource Request or SCRM */
  boolean infinite_delay;       /* True if infinite delay */
  uint32  retry_delay_duration; /* Retry delay duration, in unit of ms */

} retry_delay_type;

typedef struct
{
  boolean                 return_if_fail; /* Return if fail flag */
  byte                    num_recs;       /* Number of redirection records */
  sd_redir_rec_s_type     *redir_rec_list;/* Redirection record list ptr */

} mcc_redir_info_type;

typedef enum
{
  MEAS_BACK_IGNORED, /* Ignore background system measurement request  */
  MEAS_BACK_ALLOWED, /* Allow background system measurement request   */
  MEAS_BACK_EXIT     /* Exit on background system measurement request */

} mcc_sys_meas_mode_type;

/* ----------------------------
** CDMA Channel monitoring mask
** ---------------------------- */
#define MCC_NONE         0x00
#define MCC_PCH          0x01

#ifdef FEATURE_IS2000_REL_A_CC
#define MCC_PRI_BCCH     0x02
#define MCC_FCCCH        0x04

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
#define MCC_NON_PRI_BCCH 0x08
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

/* ----------------
** Primary BCCH BCN
** ---------------- */
#define MCC_PRI_BCCH_BCN 1
#endif /* FEATURE_IS2000_REL_A_CC */

/* ---------------------------
** Channel monitoring priority
** --------------------------- */
typedef enum
{
  MCC_FIRST_PRIORITY,
  MCC_SECOND_PRIORITY,
  MCC_INVALID_PRIORITY

} mcc_channel_monitor_priority_type;

/*===========================================================================

                      DATA DECLARATIONS

===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*   External declarations for structures declared in mc_cdma.c            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DS       /* if Data Services are compiled in */
/* Regional definitions to support DS-originated voice calls               */
extern boolean mcc_dsvoice;   /* indicates Voice call originated by DS task */
#endif

/* All mobile station specific variables */
extern cdma_ms_type cdma;


/* Internal format of Status Response message to be transmitted
   on the Access Channel */
extern caii_ac_strsp_type cdma_strsp;

#ifdef FEATURE_CP_MEM_REDUCTION
#define MC_MAX_BASES 5
  /* Maximum number of base stations for which overhead information will
     be stored */
#else
#define MC_MAX_BASES 10
#endif /* FEATURE_CP_MEM_REDUCTION */

/* Base Station specific variables */
extern cdma_bs_type bs_info[MC_MAX_BASES];

/* Pointer to Current Base Station in Base Station Information array */
extern cdma_bs_type *cur_bs_ptr;

/* Pointer to Previous Base Station in Base Station Information array */
extern cdma_bs_type *last_bs_ptr;

/* Last state of CDMA state machine to process */
extern word cdma_last_state;

/* Structure containing exit information */
extern mcc_exit_type mcc_exit;

#define MCC_MAX_ACK_TYPES 2
#define MCC_MAX_MSG_SEQ 8

  #define MCC_MAX_ADDR_TYPE 8
  /* IMSI_S, ESN, IMSI, TMSI, BROADCAST. There are 3 unused fields, that  */
  /* are in the array.                                                    */
  /* Refer to sections 6.6.2.1.2 and 7.7.2.3.1 in IS-95B for more info    */

/* Structure containing Paging Channel or Forward Common Control Channel  */
/* duplicate msg sequence time stamps for non-page messages.              */
extern qword
  mcc_idle_frames[MCC_MAX_ACK_TYPES][MCC_MAX_MSG_SEQ][MCC_MAX_ADDR_TYPE];

/* Structure containing Paging Channel or Forward Common Control Channel  */
/* duplicate msg sequence time stamps for page records.                   */
extern qword
  mcc_idle_pg_frames[MCC_MAX_MSG_SEQ];


/* Timers */
extern rex_timer_type  mcc_rpt_timer;
extern rex_timer_type mcc_state_timer;
extern rex_timer_type  mcc_action_timer;

#ifdef FEATURE_IS95B_MDR
/* MDR SCAM message start timer */
extern rex_timer_type  mcc_scam_start_timer;
#endif /* FEATURE_IS95B_MDR */

#if defined(FEATURE_IS95B_MDR)
extern rex_timer_type  mcc_mdr_duration_timer;
#endif /* defined(FEATURE_IS95B_MDR) */

extern rex_timer_type  mcc_cmd_timer;
extern rex_timer_type  mcc_bs_insanity_timer;
extern rex_timer_type  mcc_so_ctl_timer;
extern rex_timer_type  mcc_srv_timer;
extern rex_timer_type  mcc_full_tmsi_timer;
extern rex_timer_type  mcc_tmsi_exp_timer;
extern rex_timer_type  mcc_slotted_timer;  /* Slotted Timer */

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

/* this timer is a callback timer. when a transceiver request
 * times out on waiting for resource, the callback function 
 * will be called. This timer is originally defined in mccdma.c
 */
extern rex_timer_type mcc_tr_request_timer;

extern rex_timer_type mcc_t50_timer;
extern rex_timer_type mcc_t51_timer;

#ifdef FEATURE_IS2000_REL_C_AKA
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA */

extern nv_cmd_type mcc_nv_buf;      /* Buffer for command to NV task */
extern nv_item_type mcc_nv_data;    /* Buffer for data from NV task */
extern txc_cmd_type mcc_tx_buf;     /* Buffer for command to Transmit task */
extern rxc_cmd_msg_type mcc_rx_buf; /* Buffer for command to Receive task */
extern rxtx_cmd_type mcc_rxtx_buf;  /* Buffer for command to Layer 2 task */
extern srch_cmd_type mcc_srch_buf;  /* Buffer for command to Search task */


/* Debug handoff parameters */
typedef struct
{
  byte t_add;
  byte t_drop;
  byte t_tdrop;
  byte t_comp;
  byte win_a;
  byte win_n;
  byte win_r;
  byte nghbr_max_age;

  byte soft_slope;
  int1 add_intercept;
  int1 drop_intercept;

} mcc_ho_cb_type;

/* Last entry reason to CDMA and Analog call processing  */
typedef struct
{
  mcc_entry_reason_type cdma;
    /* Last entry reason to CDMA call processing */

  #ifdef FEATURE_ACP
#error code not present
  #endif /* FEATURE_ACP */

} mcc_status_type;

extern mcc_ho_cb_type mcc_ho_debug;
extern mcc_ho_cb_type mcc_ho_real;

/* Internal format of (Enhanced) Access Channel Message                               */
extern caii_tx_msg_type mcc_tx_int_msg;

/* External format of (Enhanced) Access Channel Message                               */
extern cai_tx_msg_type mcc_tx_ext_msg;

/* Information needed for Paging/Access Channel Layer 2 processing         */
extern mcc_layer2_type mcc_layer2;

#ifdef FEATURE_EVRC_SO_MGMT
/* Structure containing Preferred Voice Service Option information         */
extern nv_pref_voice_so_type mcc_pref_voice_so;
#endif

/* Set values as bit positions to allow for possible treatment like a 
** bit mask.
*/
typedef enum
{
  NO_MATCH = 0,
  PAGE_MATCH = 1,
  BROADCAST_MATCH = 2

  #ifdef FEATURE_IS2000_REL_A_CC
  , ANNOUNCEMENT_MATCH = 4
  #endif /* FEATURE_IS2000_REL_A_CC */

} mccdma_page_match_result_type;

#ifdef FEATURE_BROADCAST_SMS
/* This variable should be replaced by mcc_is_bcsms_enabled() */
extern boolean mcc_bc_sms_enable;
#endif /* FEATURE_BROADCAST_SMS */

#ifndef FEATURE_MEM_REDUCE_PRL
/* buffer to hold the copy of the roaming list that is in NV */
extern nv_roaming_list_type mcc_roaming_list;
#endif /* !FEATURE_MEM_REDUCE_PRL */

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

/* The maximum that can be supported now is 
   MAX_BAND_GROUPS (2). Please increase the value of 
   MAX_BAND_GROUPS when adding more than 2 Band Groups in
   Band Class table
*/
extern mcc_band_group_type mcc_band_table[MAX_BAND_GROUPS];

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

/* Keys used to identify BS info */
typedef struct
{
  caix_chan_type  ovhd_chan;  /* Type of overhead channel used */
  word            pilot_pn;   /* Pilot PNs */
  word            cdma_ch;    /* Current CDMA Channel number */
  byte            band_class; /* Band class */
  byte            ovhd_walsh_ch; /* Current Broadcast Control or Paging Channel number */
} bs_info_id_type;

#ifdef FEATURE_IS2000_REL_A_CC
/* Buffer to hold segmented Universal Page Message */
extern caii_seg_univ_page_type mccdma_seg_univ_page;
#endif /* FEATURE_IS2000_REL_A_CC */

/* 
   mcc_burst_overflow_size contains the number of bytes the Burst message
   (PD or SDB) is exceeding its limit of successful transmission.
*/
extern uint16 mcc_burst_overflow_size;

#ifdef FEATURE_IS2000_REL_A_CC
/* While selecting the mode/rate to use on REACH, this variable keeps 
   track of the maximum number of bits that can be transmitten successfully
*/
extern word mcc_max_reach_size;
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCC_GET_SLOT_CYCLE_INDEX

DESCRIPTION
  Returns the slot cycle index preferred by the user.

DEPENDENCIES

RETURN VALUE
  Slot cycle index.

SIDE EFFECTS

===========================================================================*/

extern int8 mcc_get_slot_cycle_index(void);

#ifdef FEATURE_GPSONE_NEW_L2_ACK
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK */

/*===========================================================================

FUNCTION MCCIDL_ASSERT_SLEEP_OK

DESCRIPTION
  This function asserts the mccidl_sleep_ok signal.

  If two events occur which both trigger a sleep, the MCC task may go
  to sleep before the second trigger is removed from the queue.  Any
  sleep command which was generated before the "sleep_ok_time" is
  ignored to prevent this lingering sleep trigger from causing the
  mobile to go to sleep immediately after waking up.

DEPENDENCIES
  None.

RETURN VALUE
  true      sleep ok was asserted.
  false     sleep ok was not asserted.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mccidl_assert_sleep_ok
(
  qword rpt_time
    /* Time the sleep trigger was issued */
);

/*===========================================================================

FUNCTION CDMA_INIT

DESCRIPTION
  This procedure determines which substate of the initialization substate
  should be processed and vectors control to the appropriate procedure
  to process the substate.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_init( void );     /* Initialization state */

/*===========================================================================

FUNCTION CDMA_IDLE

DESCRIPTION
  This procedure determines which mode of idle operation is required and
  calls the appropriate procedure to handle the designated mode.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_idle( void );     /* Idle state */

/*===========================================================================

FUNCTION CDMA_SA

DESCRIPTION
  This function is the lowest level state machine for the System Acces
  state.  It determines which sub-state needs to be processed and calls
  the appropriate procedure to process the sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  The mobile will attempt to access the system and send a message to the
  base station during the System Access state.

===========================================================================*/

extern word cdma_sa( void );       /* System Access state */

/*===========================================================================

FUNCTION CDMA_TC

DESCRIPTION
  This function is the lowest level state machine for the Mobile Station
  Control on the Traffic Channel state. It determines which sub-state needs to
  be processed and calls the appropriate procedure to process the sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern word cdma_tc( void );       /* Traffic Channel state */

/*===========================================================================

FUNCTION MCC_GET_MSG

DESCRIPTION
  This procedure checks inside the base station information array for a
  message received for the currently active base station and returns a
  pointer to the requested message if a valid message is found.

DEPENDENCIES
  Requested Message must be one that is stored in the base station
  information array.

RETURN VALUE
  A pointer to the requested message if a valid message is found.  NULL if
  the message is not found.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

extern void *mcc_get_msg
(
  word msg_type
    /* message type to retrieve */
);

/*===========================================================================

FUNCTION MCC_GET_BS_INFO

DESCRIPTION
  This procedure checks inside the base station information array for an
  entry for the specified paging channel and returns a pointer to the
  information if the entry is found.  The time input parameter specifies
  when the information message will be used (thus can be used to check for
  expiration).  If NULL (a pointer) time is specified, the information will
  be returned without checking for expiration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the requested information is found.  FALSE otherwise.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

boolean mcc_get_bs_info
(
  bs_info_id_type *bs_id,
    /* specifies the broadcast control or paging channel to which the message
       belongs */
  qword info_use_time,
    /* the time the bs_info is used */
  cdma_bs_type **bs
    /* for returning the BS info */
);

/*===========================================================================

FUNCTION MCC_PUT_MSG

DESCRIPTION
  This procedure places a message for the currently active base station
  into the base station information array.

DEPENDENCIES
  Message to be placed in base station information array must be a
  valid type.

RETURN VALUE
  None.

SIDE EFFECTS
  This procedure will examine the message sequences of the messages
  being placed into the base station information array and adjust the
  message receipt flags and the message sequence numbers accordingly.

===========================================================================*/

extern void mcc_put_msg
(
  byte msg_type,      /* message type being placed in base station info */
  caii_rx_msg_type *msg_ptr          /* pointer to message to be placed */
);

/*===========================================================================

FUNCTION MCC_PUT_SEQ

DESCRIPTION
  This procedure places a message sequence number (overhead or access
  parameters) into the base station information array for the currently
  active base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Message receipt flags for the base station may change.

===========================================================================*/

extern void mcc_put_seq
(
  byte seq_num,                  /* sequence number to insert */
  boolean acc_seq                /* TRUE = acc_msg_seq, FALSE = ovhd_msg_seq */
);

/*===========================================================================
FUNCTION MCC_INIT_OVHD_INFO

DESCRIPTION
 This function initialises the Base station Info array to ensure that
 all the overhead messages are marked as invalid. This is done so that
 the Overhead Messages are processed properly when we re-enter CDMA.

 This function must be called each time CDMA is entered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void mcc_init_ovhd_info(void);

/*===========================================================================

FUNCTION MCC_ENTRY_INIT

DESCRIPTION
  This procedure initializes an entry in the base station information array
  with the parameters for the base station having the given pilot_pn and
  using the given cdma channel and paging channel.  If an entry already exists
  for the base station with the given pilot_pn and channel configuration, a
  new entry will not be created.  Since a base station may have different
  parameters for different channels, a separate entry will be kept for each
  different channel we use.  This procedure will set the current base station
  pointer to the correct entry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the base station information array is full, this procedure will
  replace the entry with the oldest time stamp with this entry.

===========================================================================*/

extern void mcc_entry_init
(
  bs_info_id_type *bs_info_id,
  boolean         td_supported
);

/*===========================================================================

FUNCTION MCC_PROCESS_REORDER

DESCRIPTION
  This function processes a reorder order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_process_reorder(void);

/*===========================================================================

FUNCTION PROCESS_INTERCEPT

DESCRIPTION
  This function processes an intercept order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void process_intercept(word curr_state);

/*===========================================================================

FUNCTION MCC_MS_ORIG

DESCRIPTION
  This function processes the Mobile Station Origination operation.
  During this operation the number dialed by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

extern void mcc_ms_orig
(
  mc_origination_type * orig_parms
    /* Pointer to message received from the handset */
);

/*===========================================================================

FUNCTION MCC_MS_SMS

DESCRIPTION
  This function processes the Mobile Station SMS operation.
  During this operation the SMS message data sent by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

extern void mcc_ms_sms
(
  mc_msg_type *cmd_ptr
    /* Pointer to message received from the handset */
);

/*===========================================================================

FUNCTION MCC_VALIDATE_SO_REQ

DESCRIPTION
  Determines if the service option request is acceptable

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets value of cdma.so_req and cdma.service_option_1

===========================================================================*/

extern void mcc_validate_so_req
(
  mcc_page_info_type *page_info
    /* Pointer to the body of the matched page */
);

/*===========================================================================

FUNCTION MCC_COMP_ADDR

DESCRIPTION
  This function takes a pointer to the header of a paging channel message
  and checks to see if it is addressed to the mobile.
  When IMSI addressing gets added this function will become much larger.
  This function is called from mccidl.c and mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address matches, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame
    /* Msg frame, in 20 ms */
);

/*===========================================================================

FUNCTION MCC_MATCH_MSG

DESCRIPTION
  This function checks all messages except page messages to see if the
  address matches and if the message is not a duplicate.  If both are
  true the message is coppied into  matched_message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the address of the message matches and the message is
  not a duplicate, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mcc_match_msg
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mccdma_matched_msg_type *mm_ptr,
    /* Pointer to struct to copy message into if match is found. */
  boolean store_chnasn_for_tci,
    /* Indicates if TC CAM and ECAM should be stored for TCI operation. */
  word curr_state
    /* State currently being processed */
);

/*===========================================================================

FUNCTION MCC_PARSE_PAGE

DESCRIPTION
  This function takes a pointer to a page message and searches through all
  the pages in it to see if any are addressed to the mobile.  If one is
  found, information from the page is coppied into a general structure
  to avoid having to parse the page twice. Since a mobile specific page match
  has highest priority, if a match is found, all other page records can be
  discarded, if not found, all page records (including broadcast pages if
  supported) need to be checked.  This function is called from mccidl.c and
  mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a page is found which is addressed to the mobile and is not a
  duplicate.  FALSE otherwise.

SIDE EFFECTS
  Stores the new config and access message sequences
  If broadcast paging is supported and broadcast pages are found, the
  BURST_TYPEs and BC_ADDRs fields are copied into mccdma_bc_page_rec[].

===========================================================================*/

extern mccdma_page_match_result_type mcc_parse_page
(
  word new_state,
    /* Next state to be processed */
  boolean pending_bcast,
    /* Indicates if it is a pending broadcast page */
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
);

/*===========================================================================
FUNCTION MCC_SRCH_CMD

DESCRIPTION
  This function sends a command to the Searcher task.  If the Searcher task
  responds with a status other than SRCH_DONE_S,  this function will call
  ERR_FATAL to log the fatal error.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns, the status returned by the Searcher
  task was SRCH_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_srch_cmd
(
  srch_cmd_type *cmd_ptr
    /* pointer to command to send to Searcher */
);

/*===========================================================================

FUNCTION MCC_RXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Receive subtask.  If the Receive
  responds with a status other than RXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns the status returned by RXC was RXC_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_rxc_cmd
(
  rxc_cmd_msg_type *cmd_ptr
    /* pointer to command to send to Receive task */
);

/*===========================================================================

FUNCTION MCC_TXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Transmit subtask.  If the Transmit
  task responds with a status other than TXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);

/*===========================================================================

FUNCTION MCC_RXTX_CMD

DESCRIPTION
  This function sends a command to the Layer 2 task.  If the Layer 2
  responds with a status other than RXTX_DONE_S, this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_rxtx_cmd
(
  rxtx_cmd_type *cmd_ptr
    /* pointer to command to send to Layer 2 task */
);

/*===========================================================================

FUNCTION MCC_NV_CMD
  This function sends a command to the NV task.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  Status returned by nv_cmd procedure.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

extern nv_stat_enum_type mcc_nv_cmd
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
);

/*===========================================================================

FUNCTION MCC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mcc_rpt_timer.

DEPENDENCIES
  Depends on previously having used the mcc_rpt_timer to time when to
  set the watchdog signal.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

extern dword mcc_wait
(
  dword mask
    /* Mask of signals to wait for */
);

/*===========================================================================

FUNCTION MCC_ENCODE_DTMF

DESCRIPTION
  This function converts a called-address digit received from UI
  (in ASCII format) to BCD format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

extern byte mcc_encode_dtmf
(
  byte ui_char
    /* ASCII digit received from the user interface */
);

/*===========================================================================

FUNCTION MCC_DECODE_DTMF

DESCRIPTION
  This function converts a BCD encoded called-address digit
  to ASCII format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

extern byte mcc_decode_dtmf
(
  byte bcd_digit
    /* BCD digit to be converted to ASCII equivalent */
);

/*===========================================================================

FUNCTION MCC_GET_MIN_INFO

DESCRIPTION
  This function loads the CDMA structure with the information for the
  orig_min.

DEPENDENCIES
  Depends on current nam being in cdma structure.

RETURN VALUE
  Indicator of whether all parameters were successfully read from NV.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_get_min_info ( void );

/* Stub out the vocoder structures/APIs for 8K targets */
#ifndef FEATURE_8K_STUFF_TO_BE_PURGED
#ifndef FEATURE_VOCODER_MANAGER
/*===========================================================================

FUNCTION MCC_VOC_SELFTEST

DESCRIPTION
  This procedure powers up and sends the selftest command to the vocoder.
  If the selftest fails the phone err_fatals, else the vocoder version is
  stored.

DEPENDENCIES
  None.

RETURN VALUE
  Result of tests in vocoder.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_voc_selftest( void );
#endif /* FEATURE_VOCODER_MANAGER */
#endif /* !FEATURE_8K_STUFF_TO_BE_PURGED */

/*===========================================================================

FUNCTION PERMUTE_ESN

DESCRIPTION
  This function permutes the ESN as described in IS-95 section 6.1.3.1.8.

DEPENDENCIES
  None.

RETURN VALUE
  Permuted esn.

SIDE EFFECTS
  None.

===========================================================================*/

extern dword permute_esn
(
  dword esn  /* ESN to permute */
);

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE_ECAM

DESCRIPTION
  This function detects whether a duplicate ECAM message has been received
  on the Paging Channel. This function does not record the time, because the
  main mcc_detect_duplicate function takes care of that. This function is 
  called only for detecting duplicate ECAMs so that we dont consider it as
  an ack for the previous transmission.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate ECAM was received on the Paging Channel.
  FALSE - The received ECAM was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate_ecam 
(
  /* Header of ECAM to process layer 2 fields of */
  caii_pc_hdr_type *hdr
);

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE

DESCRIPTION
  This function detects whether a duplicate message has been received
  on the Paging Channel.  If a duplicate message was NOT received this
  function records the time at which the message was received.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate message was received on the Paging Channel.
  FALSE - The received Paging Channel message was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate
(
  boolean page,
    /* Identify if page or non-page message */
  caii_pc_hdr_type *hdr
    /* Header of paging channel message to process layer 2 fields of */
);

/*===========================================================================

FUNCTION MCC_RESET_IDLE_FRAMES

DESCRIPTION
  This function resets the mcc_idle_frames and mcc_idle_pg_frames array
  to indicate that no messages received on the Paging Channel or Forward
  Common Control Channel will be duplicates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_reset_idle_frames( void );

/*===========================================================================

FUNCTION MCC_IDLE_HO_RX

DESCRIPTION
  This procedure clears messages received from the current base station and
  sends a request to the receive task to perform idle handoff processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mcc_rxtx_q will be emptied.

===========================================================================*/

extern void mcc_idle_ho_rx( void );

/*===========================================================================

FUNCTION MCC_PC_ASSIGN

DESCRIPTION
  This procedure does the required processing for a paging channel assignment
  (mode '001' or '101') from Idle or System Access state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_pc_assign
(
  word new_state,
    /* Next state to be processed */
  mccdma_chnasn_type *pc_assign_msg_ptr
    /* Message addressed to this mobile which specifies a Paging Channel
       assignment (ASSIGN_MODE equal to '001' or '101') */
);

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION MCC_AUTH_CMD

DESCRIPTION
  This function sends a command to the Authentication Task.  If the
  task responds with a status other than AUTH_RECEIVED_S,  this
  function will call ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_auth_cmd
(
  auth_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);

/*===========================================================================

FUNCTION MCC_RESYNC_AUTH

DESCRIPTION
  This function sends a command to re-synchronize the Authentication Task
  command queue by queueing a resync command and waiting for it to be
  processed.  If auth has other commands on its queue or is currently
  processing some other command, we will wait for it to finish everything
  and eventually get to our resync command and send the resync report.
  Any other reports it may send before that are discarded and returned to the
  free queue, so they will not be received later when they are not expected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mcc_resync_auth( void );

/*===========================================================================

FUNCTION MCC_QUEUE_AUTH_RPT

DESCRIPTION
  This function enqueues a specified report from the Authentication Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_queue_auth_rpt
(
  auth_rpt_type *rpt_ptr
    /* Pointer to authentication report */
);
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MCC_UIM_CMD

DESCRIPTION
  This function sends a command to the UIM Task.  It then waits for
  the initial command response from the UIM server.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Waits for command to be received and UIM task to START processing
  the command.

===========================================================================*/

extern void mcc_uim_cmd
(
  uim_cmd_type *cmd_ptr
    /* pointer to command to send to UIM task */
);

/*===========================================================================

FUNCTION MCC_QUEUE_UIM_RPT

DESCRIPTION
  This function enqueues a specified report from the UIM server Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_queue_uim_rpt
(
  uim_rpt_type *rpt_ptr
    /* Pointer to UIM server report */
);
#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_DH) && defined (FEATURE_DH_EXP))
/*===========================================================================

FUNCTION MCC_QUEUE_DH_RPT

DESCRIPTION
  This function enqueues a specified report from the DH Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_dh_rpt
(
  dh_rpt_type  *dh_rpt_buf_ptr
    /* Pointer to dh report buffer filled in by dh task */
);
#endif /* FEATURE_DH && FEATURE_DH_EXP */

/*===========================================================================

FUNCTION TC_SO_CHANGE

DESCRIPTION
  This function changes service options.  It does not do any checking to
  see if the new service option is different from the current service option.
  All error checking regarding the new service option should be done prior to
  calling this procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void  tc_so_change
(
  caii_srv_cfg_type  *cfg_ptr
  // config to get SO's from
);

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE

DESCRIPTION
  This function informs lower layers about the SCR change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes get queued up to take effect at the action time

===========================================================================*/

extern void tc_inform_scr_change( word new_so, byte action_time, boolean use_pending_cfg);

/*===========================================================================

FUNCTION MCC_INFO_BAND_CLASS

DESCRIPTION
  This function builds the band class information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void mcc_info_band_class(caii_band_rec_type * rec_ptr);

/*===========================================================================

FUNCTION MCC_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
   This function returns an integer value as follows:
    -1 - if the type of the requested information record is invalid or
         the specified qualification information is invalid.
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/

extern int mcc_info_rec
(
  boolean pc,
    /* TRUE  if status request received on paging chan;
       FALSE if on traffic chan */
  byte band,
    /* Band class qualifier */
  byte op_mode,
    /* Operating mode qualifier */
  byte req_typ,
    /* Type of information record requested */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz,
    /* Size of information record buffer, in bytes */
  byte *reject_reason
    /* Reason for rejecting the request */
);

/*===========================================================================

FUNCTION MCC_PROCESS_INFO_RECORDS

DESCRIPTION
  This function processes the Feature Notification, Alert With Information
  and Flash With Information messages.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2.  FALSE otherwise. (This information is
  needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mcc_process_info_records
(
  byte call_id,
    /* Call Id for CM */
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
);

/*===========================================================================

FUNCTION MCC_GET_STATE

DESCRIPTION
  Returns the current state of the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern byte mcc_get_state( void );

#if (defined(FEATURE_DH) && defined (FEATURE_DH_EXP))
/*===========================================================================

FUNCTION MCC_DH_CMD

DESCRIPTION
  This function sends a command to the DH Task.  If the
  task responds with a status other than AUTH_RECEIVED_S,  this
  function will call ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

extern void mcc_dh_cmd
(
  dh_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
);
#endif /* FEATURE_DH && FEATURE_DH_EXP*/

/*====================================================================

FUNCTION MCC_DEFAULT_IMSI_S1

DESCRIPTION
  This routine will compute default IMSI_S1 value as per IS-95B section 6.3.
  The algorithm is copied from uinv_default_imsi_s1.

DEPENDENCIES
  This function requires that the esn has been loaded into cdma.esn structure.

RETURN VALUE
  This function returns the encoded value of IMSI_S1 with the four least
  significant digits set to ESNp, converted directly from binary to decimal,
  modulo 10000. The other digits are set to zero.

SIDE EFFECTS

====================================================================*/
dword mcc_default_imsi_s1(void);

/* <EJECT> */

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S1

DESCRIPTION
  This function returns correct imsi_s1 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S1 if IMSI_M is programmed. Otherwise, return IMSI_T_S1 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/
dword mcc_get_auth_imsi_s1(void);

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S2

DESCRIPTION
  This function returns correct imsi_s2 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S2 if IMSI_M is programmed. Otherwise, return IMSI_T_S2 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/
word mcc_get_auth_imsi_s2(void);

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/*===========================================================================

FUNCTION MCC_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the updated search parameters to the searcher.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
  None.

SIDE EFFECTS
  The search parmameters in the searcher will be changed.

===========================================================================*/

void mcc_send_srch_parms( void );

/*===========================================================================

FUNCTION MCC_CM_MC_RPT

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_cm_mc_rpt
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
);

/*===========================================================================

FUNCTION mcc_check_alert_signal_info

DESCRIPTION
  This function checks whether the info record contains a signal info record
  or not and it also checks the whether the signal type is set to 1 or 2
  if the info record contains a signal info record

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2. or it does not contain a signal information record
  FALSE otherwise. (This information is needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_check_alert_signal_info
(
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
);

#if (defined(FEATURE_CDSMS) || defined (FEATURE_DBM))
/*===========================================================================

FUNCTION MCC_SEND_MO_DBM_STATUS

DESCRIPTION
  This function sends a status report to the CM task for a mobile originated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mo_dbm_status
(
  byte burst_type,
    /* Data Burst Message type, currently, it supports only SMS and PD */
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
);

/*===========================================================================

FUNCTION MCC_CHECK_MO_DBM_STATUS

DESCRIPTION
  This function checks if the MC is waiting for a layer2 ack for a specific
  data burst message (SMS or PD), and if so, sends out a status report to CM
  task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_check_mo_dbm_status
(
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
);

/*===========================================================================

FUNCTION MCC_SEND_MT_DBM_DATA

DESCRIPTION
  This function sends the data to the CM task for a mobile terminated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mt_dbm_data
(
  uint8 dbm_burst_type,
    /* The data burst type */
  uint8* data,
    /* Pointer to the paylaod */
  uint8 data_len
    /* The number of bytes in the payload */
);

#endif /* defined(FEATURE_CDSMS) || FEATURE_GPSONE */

/*===========================================================================

FUNCTION MCC_SEND_ORIG_FAILURE

DESCRIPTION
  This function sends a report to the CM task for an origination failure .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_orig_failure(cm_call_orig_fail_e_type err_code, byte call_id,
         cm_call_orig_fail_layer_e_type layer);

/*===========================================================================

FUNCTION mcc_set_layer2_ack

DESCRIPTION
  Set Layer2 ack info.

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

void mcc_set_layer2_ack
(
  byte ack_seq,
  byte ack_type,
  byte valid_ack
);

#ifdef FEATURE_BROADCAST_SMS
/*===========================================================================

FUNCTION MCC_BC_MATCH

DESCRIPTION
  This function parse the *pc_hdr to see if it is a BC message header that
  contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *pc_hdr contains enough
  useful bits (at least CAII_BC_HDR_LEN bits). And the current slot is the
  scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

uint16 mcc_bc_match(byte *bcast_hdr);
#endif /*  FEATURE_BROADCAST_SMS */

/* <EJECT> */
#ifdef FEATURE_IS2000_R_SCH
/*===========================================================================

FUNCTION MCC_SET_RSCH_DTX_SIG

DESCRIPTION
  This function sets the MCC_TXC_REV_DTX_SIG. It is called by TXC when it
  detects REV DTX.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_rsch_dtx_sig (void);
#endif

#ifdef FEATURE_FACTORY_TESTMODE
/*===========================================================================

FUNCTION CDMA_RF_INIT

DESCRIPTION
  This procedure initializes rf for the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_rf_init
(
  word channel,
    /* channel to acquire */
  byte band_class
    /*  band class to acquire in */
);

/*===========================================================================

FUNCTION CDMA_EXIT_INIT

DESCRIPTION
  This procedure initializes the Exit state for CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_exit_init( void );

/*===========================================================================

FUNCTION CDMA_CONTINUE

DESCRIPTION
  This function is called to reset the CDMA acquisition parameters when
  the call to function mcsys_cdma_continue() indicates that we are to
  continue operating in CDMA mode without passing control back to the
  system determination level first (i.e., we are currently operating on
  the preferred system).

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  RXTX and Searcher tasks will go to Start mode.  Receive and Transmit
  tasks will reset to CDMA initialization mode.

===========================================================================*/

word cdma_continue
(
  boolean voc_pwr_off,
    /* Indicator of whether vocoder power needs to be turned off */
  mcc_entry_type *mcc_entry_reason_ptr
    /* Contains entry parameters */
);

/*===========================================================================

FUNCTION CDMA_EXIT

DESCRIPTION
  This procedure is the exit state for the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/

void cdma_exit
(
  boolean voc_pwr_off
    /* indicator of whether vocoder power needs to be turned off */
);
#endif /* FEATURE_FACTORY_TESTMODE */

/*===========================================================================

FUNCTION MCC_UPDATE_BS_P_REV

DESCRIPTION
   Updates internal data and report events when BS _PREV is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_update_bs_p_rev
(
  uint8 bs_p_rev
);

/*===========================================================================

FUNCTION reg_retry_delay

DESCRIPTION
  This function binds a retry delay function to a function pointer. If an old
  retry delay function already exists, it overwrites it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*retry_delay_inform_func_type) (retry_delay_type retry_parm);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK retry_delay_inform_func_type POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

void reg_retry_delay
(
  retry_delay_inform_func_type ds_retry_delay_inform
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION reg_retry_delay */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION update_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration by calling DS's
  registered retry delay processing function.  It uses the current system
  time and given retry delay to compute the actual retry timer duration in
  unit of milliseconds.

  SCRM retry timer duration = (time left before the next 80ms boundary +
                               retry delay * 320) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void update_retry_delay
(
  cai_retry_order_type  retry_type, /* Origination, Resource Request or SCRM */
  uint8 retry_delay
);

/*===========================================================================

FUNCTION mcc_send_orig_fail_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration for an origination
  by sending comand CM_CALL_FAIL_RETRY_ORDER_F to CM. It uses the current
  system time and given retry delay to compute the actual retry timer
  duration in unit of milliseconds.

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 min)         least significant 7 bits of retry delay * 60000) ms

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 sec)         retry delay * 1000) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_send_orig_fail_retry_delay
(
  uint8  retry_type,
  uint8  retry_delay,
  uint16 so_req,
  uint8 call_id
);

/*===========================================================================

FUNCTION MC_GET_ENTRY

DESCRIPTION
  The function copies the last CDMA and Analog call processing entry reason
  to specified input structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.
===========================================================================*/

extern void mc_get_entry(mcc_status_type *);

/*===========================================================================

FUNCTION MCC_SYS_MEAS_CMD

DESCRIPTION
  The function processes system measurement request from system determination.
  It prepares the system measurement command and sends it to Search. If
  Search rejects the system measurement request, report to system
  determination of all pilots with minimum RXPWR / ECIO and determines
  whether continues on the current system based on the suggested SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if stay on current system. Otherwise, FALSE.


SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_sys_meas_cmd(void);

/*===========================================================================

FUNCTION MCC_SYS_MEAS_RPT

DESCRIPTION
  The function reports the system measurements based on search report to
  system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_sys_meas_rpt
(
  mccsrch_rpt_type *rpt_ptr /* Pointer to SRCH report */
);

/*===========================================================================

FUNCTION MCC_CONTINUE_ON_CURR_SYS

DESCRIPTION
  Based on passed in SS-Action, this function checks whether the MS can
  continue on its current system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SS-Action allows MS to continue on the current system.
  FALSE if SS-Action specifies change of mode or request dedicated
    system measurement.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_continue_on_curr_sys
(
  mcc_sys_meas_mode_type meas_mode, /* Type of measurment mode allowed */
  sd_ss_act_e_type       ss_action  /* SS-Action                       */
);

/*===========================================================================

FUNCTION MCC_INFORM_SD_CHAN_ASSIGN

DESCRIPTION
  This function informs system determination of the paging / traffic channel
  assignement.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the paging / traffic channel assignment is accepted by system
    determination.
  FALSE if the paging / traffic channel assignment is rejected by system
    determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_chan_assign
(
  mccdma_chnasn_type *chan_assign_msg_ptr,
    /* Message addressed to this mobile which specifies a Paging / Traffic
       Channel assignment */
  mcc_sys_meas_mode_type sys_meas_mode
    /* Mode of system measurement allowed */
);

/*===========================================================================

FUNCTION MCC_INFORM_SD_REDIRECTION

DESCRIPTION
  This function informs system determination of the redirection.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the redirection is accepted by system determination.
  FALSE if the redirection is rejected by system determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_redirection
(
  sd_ss_redir_e_type  redir_type, /* Type of redirection */
  mcc_redir_info_type *redir_info /* Redirection information */
);

/*===========================================================================

FUNCTION MCC_SET_CURR_NAM

DESCRIPTION
  This function saves the new NAM when the NAM is changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_set_curr_nam
(
  mc_nam_changed_type nam_changed /* Pointer to new NAM info */
);

#if defined(FEATURE_FACTORY_TESTMODE) || defined(FEATURE_PLT)
/*===========================================================================

FUNCTION MC_UPDATE_LAST_CDMA_ENTRY

DESCRIPTION
  This procedure updates the last CDMA entry reason based on the input
  parameter. It should be solely used by non MC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void mc_update_last_cdma_entry
(
  mcc_entry_reason_type entry_reason
);
#endif /* FEATURE_FACTORY_TESTMODE || FEATURE_PLT */

/*===========================================================================

FUNCTION  MCC_IS_OVHD_CFG_OKAY

DESCRIPTION
  This function checks if all ovhd messages' config message sequence are
  current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs' config message sequence are current , FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_ovhd_cfg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_ovhd_msg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_UPD_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages necessary for Update Overhead info
  substate are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_upd_ovhd_msg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_AHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages  necessary for Access HO are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_aho_ovhd_msg_okay( void );

/*===========================================================================

FUNCTION  MCC_IS_AEHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages  necessary for Access Entry HO
  are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_aeho_ovhd_msg_okay( void );

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION  MCC_IS_PCH_BCCH_INFO_AVAILABLE

DESCRIPTION
  This function checks if info abt ovhd channel (PCH/BCCH) is available
  for this BS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_is_pch_bcch_info_available
(
  bs_info_id_type *basic_bs_info,
  int *bs_index
);
#endif

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION  MCC_ARE_RAND_PARMS_OKAY

DESCRIPTION
  This function checks if rand parameters are current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are on the Paging Channel, since Rand Parameters always come
  in the Access Parameters Message on PCH.
  FALSE if the base station is sending ANSI-41 RAND Message and
  it is not current; TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_are_rand_parms_okay(void);
#endif /* FEATURE_AUTH */

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCC_EMPTY_SEG_UPM_BUFFER

DESCRIPTION
  This function empty the segmented Universal Page message buffer.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mcc_empty_seg_upm_buffer(void);

/*===========================================================================

FUNCTION MCC_MATCH_UPM_PARTIAL_ADDR

DESCRIPTION
  This function performs the Mobile Station partial address match operation
  for the Universal Page Message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if partial address match.
  FALSE partial address mismatch.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mcc_match_upm_partial_addr
(
  word len,
   /* Length of page block */
  byte *buf_ptr,
   /* Pointer to page block */
  word *offset,
    /* Pointer to offset from page_block to start next translation */
  caii_univ_page_interleaved_addr_type *addr_ptr
   /* Pointer to interleaved address structure */
);

/*===========================================================================

FUNCTION MCC_REASSEMBLE_SEG_UPM

DESCRIPTION
  This function reassembles Universal Page message from Universal
  Page segment messages.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if reassembly okay. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mcc_reassemble_seg_upm
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  caii_seg_univ_page_type *buf_ptr
    /* Pointer to buffer for storing reassembled segmented Universal Page
       message */
);
#endif /* FEATURE_IS2000_REL_A_CC */

/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS

DESCRIPTION
  This function clears the "rx" flag of all messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_clear_ovhd_msgs( void );

/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS_PREV_MISMATCH

DESCRIPTION
  This function marks all messages that depends on p_rev for processing.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_clear_ovhd_msgs_prev_mismatch( void );

/*===========================================================================

FUNCTION  mcc_expired_timer_callback_func

DESCRIPTION
  This is the function that will be executed (in MC context) upon
  the expiration of a MC timer that uses the Asynchronous Procedure
  Call (APC) feature of REX+. The function queues the timer ID (argument)
  of the expired timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_expired_timer_callback_func
( 
  unsigned long
);

/*===========================================================================

FUNCTION  mcc_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if the
  argument (timer ID) is present in it.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID on queue.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_timer_on_callback_q( unsigned long );

/*===========================================================================

FUNCTION  mcc_get_type_specific_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if the
  argument (timer type) is present in it. If present, it retrieves the timer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID is being returned in argument.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_get_type_specific_timer_on_callback_q
( 
  unsigned long timer_type, unsigned long * ret_timer_id
);

/*===========================================================================

FUNCTION  mcc_purge_callback_timer_q

DESCRIPTION
  This function purges the queue of expired timers and also clears the
  MCC_CALLBACK_TIMER_SIG

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_purge_timer_callback_q(void);

/*===========================================================================

FUNCTION  mcc_set_callback_timer

DESCRIPTION
  This function should be used to set timers that utilise the REX
  callback mechanism within MCC. It clears the timer, its entry in
  the callback queue, and sets the timer.

DEPENDENCIES
  None.

RETURN VALUE
  last countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

extern rex_timer_cnt_type mcc_set_callback_timer
(
    rex_timer_type       *p_timer,   /* pointer to timer to set */
    rex_timer_cnt_type    msecs,      /* value in milliseconds   */
    unsigned long         callback_timer_id
);

/*===========================================================================

FUNCTION  mcc_clr_callback_timer

DESCRIPTION
  This function should be used to clr timers that utilise the REX
  callback mechanism within MCC. It clears the timer and its entry in
  the callback queue.

DEPENDENCIES
  None.

RETURN VALUE
  countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

extern rex_timer_cnt_type mcc_clr_callback_timer
(
    rex_timer_type       *p_timer,   /* pointer to timer to set */
    unsigned long         callback_timer_id
);

/*===========================================================================

FUNCTION XLATE_DIALED_NUM

DESCRIPTION
  This function fills a cdma_dial_type buffer with the phone number
  dialed by the user translated into the digit code required by the
  base station. It will also fill in the number of digits dialed into
  the cdma_dial_type buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void xlate_dialed_num
(
  mc_origination_type * orig_parms,
    /* pointer to recieved message */
  cdma_dial_type *buf_ptr
    /* pointer to buffer to place translated phone number into */
);

/*===========================================================================

FUNCTION  MCC_UPDATE_ACC_MSG_SEQ

DESCRIPTION
  This function updates the stored access message sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mcc_update_acc_msg_seq
(
  byte acc_msg_seq /* Access message sequence */
);

/*===========================================================================

FUNCTION MCC_INCREMENT_PAG3_OR_FCCCH_3

DESCRIPTION
  This function increments count PAG_3 or FCCCH_3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void mcc_increment_pag_3_or_fccch_3(void);

/*===========================================================================

FUNCTION MCCIDL_GET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure gets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern int8 mccidl_get_slot_cycle_index_s(void);

/*===========================================================================

FUNCTION MCCIDL_SET_SLOT_CYCLE_INDEX_S

DESCRIPTION
  This procedure sets the slot cycle index.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mccidl_set_slot_cycle_index_s(int8 sci_s);

/*===========================================================================

FUNCTION MCC_GET_IDLE_PGSLOT

DESCRIPTION
  This procedure updates sleep_slot_mask with OR operation with input mask.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern word mcc_get_idle_pgslot(void);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DBM
/*===========================================================================

FUNCTION MCC_SDB_SUPPORTED

DESCRIPTION
  This function returns if the Short Data Burst is supported by the current
  Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mcc_sdb_supported(void);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mcc_sdb_supported */
#endif /* FEATURE_HTORPC_METACOMMENTS */
#endif /* FEATURE_DBM */

/*===========================================================================

FUNCTION MCCIDL_SET_SCI_MASK

DESCRIPTION
  This procedure sets position-in-slot from pgslot.

DEPENDENCIES
  None

RETURN VALUE
  Position-in-slot.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_set_sci_mask(word new_sci_mask);

/*===========================================================================

FUNCTION mcc_set_current_preferred_sci

DESCRIPTION
  This sets the current preferred SCI value. The preferred SCI in NV is 
  not updated
  
DEPENDENCIES
  None.

RETURN VALUE
  The preferred sci value MS is actually set to

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_set_current_preferred_sci(int8 sci_p);

/*===========================================================================

FUNCTION mcc_get_current_preferred_sci

DESCRIPTION
  Gets the current preferred SCI value. This may be different from the one
  that is stored in NV.
  
DEPENDENCIES
  None.

RETURN VALUE
  The current preferred sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_current_preferred_sci(void);

/*===========================================================================

FUNCTION mcc_set_last_sci_reg

DESCRIPTION
  This sets the last registered SCI value. The last registered SCI in NV is  
  not updated
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_last_sci_reg(int8 sci);

/*===========================================================================

FUNCTION mcc_get_last_sci_reg

DESCRIPTION
  Gets the last registered SCI value. This may be different from the one
  that is stored in NV.
  
DEPENDENCIES
  None.

RETURN VALUE
  The last registered sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_last_sci_reg(void);

/*===========================================================================

FUNCTION MCC_CONVERT_PREF_SCI_TO_OTA_SCI

DESCRIPTION
  
  This function converts the preferred SCI from internal format to OTA format 
  to be packed by parser.
  
  If the auto_msg_supported bit in ESP is set, it uses the following
  conversion mapping for Negative SCI.
                 
    Preferred SCI               OTA SCI
    -------------------------   -------
      0 - 1.28s (16 slots)       000
      1 - 2.56s (32 slots)       001
      2 - 5.12s (64 slots)       010
      3 - 10.24s (128 slots)     011
     -4 - 0.080s (1 slot)        100
     -3 - 0.160s (2 slots)       101
     -2 - 0.320s (4 slots)       110
     -1 - 0.640s (8 slots)       111
  

  Otherwise, it uses the following conversion mapping:

    Preferred SC  P_REV_IN_USE  Negative SCI supported  OTA SCI
    ------------- ------------  ----------------------  ---------------------
    > 0           N/A           N/A                     Same as preferred SCI
    < 0           < 11          N/A                     0
    < 0           >= 11         No                      0
    < 0           >= 11         Yes                     Same as preferred SCI
  
DEPENDENCIES
  None.

RETURN VALUE
  Preferred SCI value to be transmitted over the air

SIDE EFFECTS
  None.

===========================================================================*/
int8 mcc_convert_pref_sci_to_ota_sci(void);

/*===========================================================================

FUNCTION MCC_SET_OPERATIONAL_SCI

DESCRIPTION
  
  This function updates the operation SCI, slot_cycle_index_s
  from OTA MAX_SCI.
  
  If the auto_msg_supported bit in ESP is set, it uses negative SCIp,
  otherwise it used positive SCIp. The conversion mapping for Negative SCI
  is as follows

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   - if overheads are current, slot_cycle_index_s is set appropriately.
  FALSE  - if overheads are not current, slot_cycle_index_s cannot be set.

SIDE EFFECTS
  slot_cycle_index_s is updated.

===========================================================================*/

boolean mcc_set_operational_sci(void);

/*===========================================================================

FUNCTION MCC_CALCULATE_SCI_MASK

DESCRIPTION
  Returns (power(2,i) - 1), where i = solt cycle index

DEPENDENCIES
  None.

RETURN VALUE
  sci_mask
  
SIDE EFFECTS
  None
  
===========================================================================*/

word mcc_calculate_sci_mask(int8 sci);

/*===========================================================================

FUNCTION MCCIDL_MSG_HANDLER

DESCRIPTION
  This function processes a message received from the Receive task.  Based
  on the message type it will call the appropriate procedure to process the
  message.

DEPENDENCIES
  Input message is in internal format.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  If the header within the input caii_rx_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

word mccidl_msg_handler
(
  mccrxtx_msg_type *msg_ptr,
    /* CDMA message in internal format */
  word new_state
    /* Next state to be processed */
);

/*===========================================================================

FUNCTION MCCIDL_PROC_IGNORED_OVHD

DESCRIPTION
  This function processed rescan or (extended) global service redirection 
  that was ignored because overheads are not current at the time 
  SPM / ANS41SPM / GSRM / EGSRM was received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

word mccidl_proc_ignored_ovhd
(
  word curr_state /* Current state */
);

/*===========================================================================

FUNCTION MCCIDL_GET_NGHBR_INDEX

DESCRIPTION
  This procedure gets the index of a specified pilot in the neighbor list.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the index value of this pilot in the neighbor list.
  "-1" indicates that this pilot is not in the neighbor list.

SIDE EFFECTS
  None.

===========================================================================*/

int2 mccidl_get_nghbr_index
(
  word pilot_pn,
  word new_freq,
  byte new_band
);

/*===========================================================================

FUNCTION MCCIDL_HANDOFF

DESCRIPTION
  This procedure performs handoff processing when the Mobile is in
  Idle state and System Access state.  Idle Handoff, Access Entry HO,
  Access Probe Handoff and Access Handoff are processed with this
  function.

DEPENDENCIES
  Depends on the Initialization state already having been processed.

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_handoff
(
  word new_pilot,
    /* New pilot received in searcher report */
  word new_freq,
    /* New cdma frequency received in searcher report. */
  byte new_band_class,
    /* New band class received in searcher report */
  word new_state,
    /* Next state to be processed */
  int8 ecio_old_pilot,
  int8 ecio_new_pilot,
  cdma_ho_type ho_type
    /* Information for event reporting */
);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_ASET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_ASET_F command to update
  the active set information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_aset(void);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the Search task a SRCH_PARM_F command to update
  searh parameters.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
 None. 

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_parms(void);

/*===========================================================================

FUNCTION MCCIDL_SEND_SRCH_IDLE_NSET

DESCRIPTION
  This procedure sends the Search task a SRCH_IDLE_NSET_F command to update
  the neighbor set information.

DEPENDENCIES
  Depends on new neighbor list message being in bs_info.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_send_srch_idle_nset( void );

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION MCCIDL_GET_SCI_MASK

DESCRIPTION
  This procedure gets position-in-slot from pgslot.

DEPENDENCIES
  None

RETURN VALUE
  Position-in-slot.

SIDE EFFECTS
  None.

===========================================================================*/

word mccidl_get_sci_mask(void);
#endif /* FEATURE_IS2000_REL_A_CC */

#if defined(FEATURE_GPSONE_NEW_L2_ACK) || defined(FEATURE_SO_8026)
#error code not present
#endif /* FEATURE_GPSONE_NEW_L2_ACK || FEATURE_SO_8026 */

/*===========================================================================

FUNCTION MCCIDL_INIT_CHANNEL_MONITORING

DESCRIPTION
  This procedure initialize channel monitoring state variables.

DEPENDENCIES
  None.

RETURN VALUE
  Number of channel configurations in the priority list.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_init_channel_monitoring(void);

/*===========================================================================

FUNCTION MCCIDL_SUPERVISE_CHANNEL_MONITORING

DESCRIPTION
  This procedure updates the requirements of channel monitoring. It also 
  notifies the lower layers if the requirement changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccidl_supervise_channel_monitoring
(
  word new_state,
    /* Next state to be processed */
  mccsrch_wakeup_rpt_type *wakeup_rpt_ptr,
    /* Search wakeup report */
  boolean enh_bc_done
    /* Is channel switching due to enh broadcast SMS done */

);

/*===========================================================================

FUNCTION MCCIDL_CLEAR_BS_INSANITY_TIMER

DESCRIPTION
  This procedure clears the BS insanity timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_clear_bs_insanity_timer(void);

/*===========================================================================

FUNCTION MCCIDL_IS_ACCESS_FADE_TIMER_IN_USE

DESCRIPTION
  This procedure returns the value of mccidl_fast_raho.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_is_access_fade_timer_in_use(void);

/*===========================================================================

FUNCTION MCC_ADD_GROUP_IN_BAND_TABLE

DESCRIPTION
  Adds the specified group in the table. It also optionally adds all the
  band classes associated with it. If the bad group already exists, it will
  not add a duplicate group. Only band classes, if any associated with 
  this group are added.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_group_in_band_table
( 
  cdma_band_class_group_type gp,
  uint8 num_bands,
  uint8 * band_class
);

/*===========================================================================

FUNCTION MCC_ADD_BAND_CLASS_IN_GROUP
  Adds the specified band class in the specified group. It calls
  mcc_add_group_in_band_table to perform its task.
  
DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS_FROM_GROUP
  Removes the band class from the specified group

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class_from_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS
  Removes the first encountered band class entry in any group. It calls
  mcc_remove_band_class_from_group to perform its job.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class
(
  uint8 band_class
);


/*===========================================================================

FUNCTION MCC_BAND_CLASS_INIT

DESCRIPTION
  Initializes the band class table.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_band_class_init(void);

/*===========================================================================

FUNCTION MCC_GET_GROUP_FOR_BAND_CLASS

DESCRIPTION
  Given the band class, it returns the group it belongs to.

DEPENDENCIES
  None.

RETURN VALUE
  The group that the specified band class belongs to.
                                                           
SIDE EFFECTS
  None.

===========================================================================*/

cdma_band_class_group_type mcc_get_group_for_band_class(uint8 band_class);

/*===========================================================================

FUNCTION MCC_IS_CURRENT_BAND_CLASS_IN_GROUP

DESCRIPTION
  Determines if the current Band Class (cdma.band_class) belongs to the 
  specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise
                                                           
SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_current_band_class_in_group(cdma_band_class_group_type gp);

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_IN_GROUP  
  
DESCRIPTION
  Determines if the specified Band Class belongs to the specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
);

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_SUPPORTED

DESCRIPTION
  Determines if the specified band class is present in any group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the band class is supported, FALSE otherwise.
  
SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_supported(uint8 band_class);

/*===========================================================================

FUNCTION MCC_SET_SCM_FOR_BAND_CLASS

DESCRIPTION
  Sets the SCM bits depending on which group the supplied band class belongs

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SCM bits were successfully set, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_set_scm_for_band_class(uint8 band_class, byte * scm);

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

/*===========================================================================

FUNCTION MCCIDL_IS_FAST_RAHO

DESCRIPTION
  This procedure returns the value of mccidl_fast_raho.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccidl_is_fast_raho(void);

/*===========================================================================

FUNCTION MCCDMA_CLEAR_MC_QUEUES_SIGS_TIMERS

DESCRIPTION
  This fn will clear all of MC's timers, It clears all of MC's signals
  and purges RXTX, RXC & SRCH queues.

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  Any active MC signal is cleared & any otems on the queue are lost

===========================================================================*/

void mccdma_clear_mc_queues_sigs_timers( void );

#if (defined (FEATURE_HDR_HANDOFF) && defined (FEATURE_IS2000_REL_A))
#error code not present
#endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*===========================================================================
FUNCTION MCC_WAIT_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests an
  update of transceiver resource and wait until it obtains the resource or 
  request timeout or the lost of resource. 

  To be safe for the coding, a timer is applied while waiting for the searcher
  report SRCH_ACCESS_READY_R. For simplicity reason, the timer value is chosen
  to be the same as T33M which is the same as the one used in 
  mcc_request_transceiver_resource.

  When the timer goes out, it does not do anything here. Mobile will come into
  Access Failure when it assumes that the transceiver resource is available.

DEPENDENCIES
  None

RETURN VALUE
  Status of transceiver request.

SIDE EFFECTS
  None

===========================================================================*/

mcc_transceiver_request_status_type mcc_wait_transceiver_resource
(
  srch_access_perf_enum access_perf, 
  srch_access_type_enum access_type
);

/*===========================================================================
FUNCTION MCC_REQUEST_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests 
  transceiver resource before it comes into Access State from Idle state. 
  Upon receiving this command, Search will try to acquire the transeiver 
  resource with specified parameters, and send a SRCH_ACCESS_READY_R when 
  the resource is ready.

  Timer T33 (300ms) is applied to the waiting for the reply from the Searcher.

  This function returns immediately after starting the timer and sending
  the request to the Searcher.

  Function mccidl_proc_srch_rpt in mccidl.c will process the Search report 
  SRCH_ACCESS_READY_R. Function mccidl_online in mccidl.c will process the
  timer expiry of T33m. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_request_transceiver_resource
(
  srch_access_perf_enum     access_perf, 
  srch_access_type_enum     access_type,
  word                      sa_tc_state
);

/*===========================================================================
FUNCTION MCC_CELLULAR_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether cellular band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some cellular band class

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_cellular_band_is_supported(void);

/*===========================================================================
FUNCTION MCC_PCS_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether PCS band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some PCS band class

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_pcs_band_is_supported(void);

/*===========================================================================

FUNCTION MCC_SEND_CM_SRV_SYS_INFO

DESCRIPTION
  This function sends the CM task CM_SRV_SYS_INFO_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_srv_sys_info
(
  word rcv_fields,
  word sid,
  word nid,
  word reg_zone,
  byte pzid
);

/*===========================================================================

FUNCTION MCC_SETUP_REJECT_ORD

DESCRIPTION
  This function sets up the reject order to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mcc_setup_reject_ord
(
  byte ordq,
    /* reason for rejecting the message */
  byte rej_msg_type,
    /* the type of message being rejected */
  byte rej_order,
    /* if the rej_msg_type is an order, this is the type of order */
  byte rej_ordq
    /* if the rej_msg_type is an order, this is the ordq of the rej_order */
);

#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCC_SETUP_VPM_AND_AKEY

DESCRIPTION
  This function performs voice privacy setup and kicks off the process of
  calculating authentication keys.
  
DEPENDENCIES
  None

RETURN VALUE
  Voice privacy mode indicator. 

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_setup_vpm_and_akey
(
  boolean voice_privacy_desired, 
    /* Indicates whether VPM cannot supported for the call type. */
  byte bs_auth_mode             
    /* Auth mode of BS. */
);

/*===========================================================================

FUNCTION MCC_CHECK_IF_TRAF_CHAN_ASSGN_IS_OKAY

DESCRIPTION
  This function checks to see if a received traffic channel assignment is 
  okay.

DEPENDENCIES
  None

RETURN VALUE
  The reason for rejecting the ECAM.

SIDE EFFECTS
  None

===========================================================================*/

uint8 mcc_check_if_traf_chan_assgn_is_okay
(
  mccdma_chnasn_type *chnasn_ptr /* Pointer to matched (E)CAM message */
);

/*===========================================================================

FUNCTION MCC_PROC_TRAF_CHAN_ASSGN

DESCRIPTION
  This function performs traffic channel assignment processing. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_proc_traf_chan_assgn
(
  mccdma_chnasn_type *chnasn_ptr
    /* Pointer to (Extended) Channel Assignment Message */
);

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

/*===========================================================================

FUNCTION MCC_CDMA_CHANS_ARE_SUPPORTED

DESCRIPTION
  This function indentifies a list of channel frequencies that the mobile 
  supported in input_redir_rec and copies these valid channels along
  with other information in input_redir_rec into output_redir_rec.  
  It returns TRUE if at least one freq belongs to the specified band in 
  input_redir_rec.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean value indicating whether at least one freq belongs to the 
  specified band class in input_redir_rec.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_cdma_chans_are_supported 
(
  const caii_redirect_rec2_type  *input_redir_rec,
  sd_redir_rec_cdma_s_type *output_redir_rec
);

/*===========================================================================

FUNCTION MCC_SEND_TXC_EARLY_JUMP_CMD

DESCRIPTION
  This function sends request to Transmit task to Initialize encoder timing. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_txc_early_jump_cmd (void);

/* ==========================================================================

FUNCTION MCCSA_ACCESS_ATTEMPT_IN_PROGRESS

DESCRIPTION
  This function checks whether access attempt is in progress.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if access in progress. Otherwise, FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsa_access_attempt_in_progress(void);

/*===========================================================================

FUNCTION MCCSA_SET_PREV_PILOT

DESCRIPTION
  This function sets Previous pilot PN and its neighbor record in Access
  Attempt list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_set_prev_pilot_info ( cdma_bs_type *old_bs_ptr );

/*===========================================================================

FUNCTION MCCSA_UPDATE_ACCESS_ATTEMPT_LIST

DESCRIPTION
  This function updates Access Attempt list with info from neighbor list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_update_access_attempt_list(void);

/* ==========================================================================

FUNCTION MCCSA_UPDATE_SRCH_NEIGHS

DESCRIPTION
  This function updates the neighbor set which will be sent to Search task
  with info from Access Attempt list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_update_srch_neighs(void);

/*===========================================================================

FUNCTION MCCSA_IS_IN_ACCESS_HO_LIST

DESCRIPTION
  This function validates if the Mobile is allowed for Access Handoff and
  if it is in the ACCESS_HO_LIST

DEPENDENCIES
  Depends on the P_REV_IN_USE (atleast 4) and Access Handoff Feature allowed.

RETURN VALUE
  boolean (TRUE/FALSE)

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsa_is_in_access_ho_list(word pilot_pn);

/*===========================================================================

FUNCTION MCCSA_CALC_PSIST

DESCRIPTION
  This function calculates values to be used for the persistence test
  specified in IS-95 6.6.3.1.1.1 and 6.6.3.1.1.2.  The result is used
  in the TX task to compare against a pseudo-random number to determine
  if the mobile station will transmit during a given access slot.

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean value indicating if the access attempt should be
  aborted.

SIDE EFFECTS
  None

===========================================================================*/

access_result_type mccsa_calc_psist
(
  cai_tx_channel_id_type chan,
    /* one of {CAI_ACH, CAI_EACH} to indicate that the parm_ptr is a ap or eap */
  void *parm_ptr,
    /* Pointer to current Access Parameters Message */
  word curr_state
    /* Current System Access substate */
);

/*===========================================================================

FUNCTION MCCSA_TX_STOP

DESCRIPTION
  This function commands tx to stop transmitting accesses but keep the
  transmit clock running while an System Access Attempt is in progress.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsa_tx_stop( void );

#ifdef FEATURE_SO_8026
#error code not present
#endif /* FEATURE_SO_8026 */

#if (defined(FEATURE_HIGH_PSIST) && defined (FEATURE_PASSPORT_MODE))
/*==========================================================================

FUNCTION MCC_SEND_CM_PSIST_INFO

DESCRIPTION
  This function sends the CM task CM_PSIST_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_psist_info
(
 cm_psist_e_type psist
);
#endif /* FEATURE_HIGH_PSIST && FEATURE_PASSPORT_MODE */

/*===========================================================================

FUNCTION MCC_CHECK_IF_CSCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message to see if it can be
  supported by the MS.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_check_if_csch_srdm_is_okay
(
  caii_serv_redirect_type *serv_redir_ptr
);


/*===========================================================================

FUNCTION MCC_USE_PSEUDO_BS_INFO

DESCRIPTION
  This function saves the value of cur_bs_ptr to 
  mcc_pseudo_bs_info.old_cur_bs_ptr and changes the value of cur_bs_ptr to
  point to mcc_pseudo_bs_info.pseudo_bs.

  It also initializes mcc_pseudo_bs_info.pseudo_bs to make it as if MS has 
  SPM, ESPM and APM for the target 1x system for the handover.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_pseudo_bs_info
(
  byte band_class,
  word cdma_ch
);

/*===========================================================================

FUNCTION MCC_USE_1X_BS_INFO

DESCRIPTION
  This function set the value of cur_bs_ptr back to 
  mcc_pseudo_bs_info.old_cur_bs_ptr.

DEPENDENCIES
  mcc_go_to_pseudo_bs must be executed prior.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_1x_bs_info
(
  void
);

#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCCDMA_ENCRYPT_VOICE_FRAME

DESCRIPTION
  This function encrypts a voice frame when voice encryption is turned on
  over voice services
  

DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_encrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );


/*===========================================================================

FUNCTION MCCDMA_DECRYPT_VOICE_FRAME

DESCRIPTION
  This function decrypts a voice frame when voice decryption is turned on over
  voice services
  

DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_decrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              );


#if defined(FEATURE_IS2000_REL_C_AKA) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
#endif /* FEATURE_IS2000_REL_C_AKA || FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCC_CHANGE_IMSI_TYPE

DESCRIPTION
  This function updates the imsi type, if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the imsi was changed; FALSE otherwise.

SIDE EFFECTS
  The cdma imsi variables may change.

===========================================================================*/

boolean mcc_change_imsi_type( boolean imsi_t_supported );

/*===========================================================================

FUNCTION MCC_SET_ADDRESS
  This function sets the address fields for a message to be transmitted
  on the access channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_address
(
  caii_ac_hdr_type *hdr,
    /* Pointer to header of access channel message to be transmitted */
  word             msg_type /*lint -esym(715,msg_type*/
    /* message type */
);

#if defined(FEATURE_AUTH) && defined (FEATURE_AUTH_DIGITS)
/*===========================================================================

FUNCTION MCC_CALCULATE_AUTH_DATA

DESCRIPTION
  This function figures out what the auth_data should be for authenticating
  an origination or SMS.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed value of auth_data

SIDE EFFECTS
  None

===========================================================================*/

dword mcc_calculate_auth_data
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
);
#endif

/*===========================================================================

FUNCTION MCCIDL_RETRYING_SLEEP

DESCRIPTION
  This function returns whether the phone is in the process of retrying
  sleep via OOS phase 1.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we're retrying sleep in between reacq failures, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccidl_retrying_sleep(void);

/*===========================================================================

FUNCTION MCCIDL_UPDATE_SLEEP_SLOT_MASK

DESCRIPTION
  This procedure updates sleep_slot_mask with OR operation with input mask.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_update_sleep_slot_mask(byte mask);

#ifdef FEATURE_PASSPORT_MODE
/*===========================================================================

FUNCTION MCCNP_ITEM_IS_BLOCKED

DESCRIPTION
  Checks if the passed msg type is blocked 

DEPENDENCIES
  None

RETURN VALUE
  True    Passed msg type is blocked

  False   Passed msg type is not blocked

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_msg_is_blocked (cdma_msg_type msg_type);

/*===========================================================================

FUNCTION MCC_PASSPORT_MODE_IS_WORLD

DESCRIPTION
  Returns TRUE if the passport mode is world.

DEPENDENCIES
  None

RETURN VALUE
  True    Passport mode is world

  False   Passport mode is jcdma

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_passport_mode_is_jcdma ( void );

#endif /* FEATURE_PASSPORT_MODE */

/*===========================================================================

FUNCTION MCCDMA_GET_CUR_BS_INFO

DESCRIPTION
  This function make a copy of the following fields of cur_bs_ptr and pass
  the values to the calling function.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_get_cur_bs_info
(
  word* sid,              /* System Identification */
  word* nid,              /* Network Identification */
  word* base_id,          /* Base Station Identification */
  byte* base_class,       /* Base Station Class */
  word* pilot_pn,         /* Pilot PN sequence offset index */
  byte* packet_zone_id,   /* Packet Zone Identification */
  word* mcc,              /* Mobile Country Code */
  dword* base_lat,         /* Base Station Latitude */
  dword* base_long         /* Base Station Longitude */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccdma_get_cur_bs_info */
/*~ PARAM OUT sid POINTER */
/*~ PARAM OUT nid POINTER */
/*~ PARAM OUT base_id POINTER */
/*~ PARAM OUT base_class POINTER */
/*~ PARAM OUT pilot_pn POINTER */
/*~ PARAM OUT packet_zone_id POINTER */
/*~ PARAM OUT mcc POINTER */
/*~ PARAM OUT base_lat POINTER */
/*~ PARAM OUT base_long POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#endif /* MCCDMA_H */

