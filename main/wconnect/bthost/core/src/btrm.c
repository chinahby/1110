/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H    R A D I O    M A N A G E R    M O D U L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Radio Manager data and code.

Copyright (c) 2007-2010 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btrm.c#30 $
  $DateTime: 2010/11/26 00:07:42 $
  $Author: ssajjan $

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2010-11-26   ss  Fix for Missing conn comp event from SOC by Limiting 
                   Name request during Pin Request, and delete Link Key
                   when Transaction collision occurs to enable future 
                   bond request to succeed
  2010-05-06   us  Disbale Scans when intiating Remote Name request or
                   Create Connection for some SOC versions.
  2010-05-03   us  Disconnect ACL connection when busy timer expires
  2010-04-29   us  Not returned BT_EV_RM_RADION_DISABLED event  in certain scenarios.
                   Radio Disabled event will be sent before the radio has been disabled
  2010-04-29   us  RM event sent when the bt_cmd_rm_enforce_security_l2cap_ps 
                   fails after re-queing maximum number of times
  2010-04-29   us  Disabled default SSR emulation 
  2010-04-28   gbm Interface to cleanup pin_req_resp_app_id
                   Clear the flag disable_scan_pending when scans are enabled
  2009-10-02   ns  Resolved race condition when dedicated bonding is attempted
                   on an existing connnection and that connection is disconnected
                   before bonding is finished.
  2009-09-10   ns  Fixed a race condition when local device tries to
                   reconnect SCO after the remote side has disconnected it.
  2009-07-21   ns  Fixed a bug in enforce service security when 
                   local device has a 2.0 controller 
  2009-06-25   ns  IS_SSP_DOABLE and IS_SSR_DOABLE now return FALSE if
                   FEATURE_BT_2_1 is not defined 
  2009-06-11   ns  changes to fix compiler warnings when FEATURE_BT_2_1 is disabled
                   and bring in changes from low_pwer_config_ext to low_power_config
  2009-04-30   pg  Fixed lint error.
  2009-06-02   pg  Fixed compiler warning. 
  2009-05-04   pg  Added logic to propagate event notifications to upper layer
                   for radio power on/off and sync connection parameter on sync 
                   complete/change HCI events.
  2009-05-04   pg  Not sending BT_EV_RM_BOND_FAILED event when auth failure is
                   due to "key missing" error from the remote device.
  2009-02-23   gs  Removed condition that connection must be "settled" before 
                   HCI disconnect command can be sent to the controller.
  2009-04-02   pg  Send BT_EV_RM_BOND_FAILED event when RM enforced timeout 
                   for application to respond to a pin request expires.
  2009-02-26   pg  Added logic to provide notification of baseband
                   page START/STOP state changes. These event notifications
                   are derived from related HCI cmds/evts and only provide an 
                   approximate representation of the actual paging interval.
  2009-03-24   ag  - Take care of upgrading security level for 2.0 devices when
                     link is only authenticated.
                   - Fix the issue of service security pointer being reset before
                     re-pairing. Resetting the pointer caused service
                     requirements to be lost.
  2009-03-13   pg  Added logic to recover RM state if cmd status with error for
                   any HCI_Remote_Name_Req cmd is received. This is treated as
                   a name request failure on all pending name requests.
  2009-03-02   pg  Sending ACL connected event immdiately after HCI connection
                   complete.
  2009-03-02   pg  Not disconnecting ACL when link key is removed for the 
                   purpose of repairing.
  2009-02-26   hm  Protection against bad EIR data
  2009-02-18   hm  Created an API for Bond Cancel
  2009-01-26   pg  Send BT_EV_RM_BOND_FAILED event to cover all possible
                   combination of lisbon/lisbon lisbon/non-lisbon 
                   authentication scenarios.
  2009-02-09   vk  Checking Device Local version, before failing security for 
                   un-Encrypted link.
  2009-02-04   hm  Zero padded the passkey for user confirmation rqst and 
                   numberic comparison before sending it to the app.
  2009-01-30   vk  Initializing sec_level_min in case of enhancing PIN code 
                   level.
  2009-01-13   vk  Failing security if incoming connection is not encrypted
                   and should be if the security mode in use is SM4, and the
                   security setting is not NONE.
  2008-12-24   vk  No security inforcement on Legacy devices if security
                   requirement is none or low.  
  2008-12-22   hm  Error checking in returning EIR data
  2008-12-03   vk  Adding BT sleep voting support.
  2008-11-17   ag  Fixed comiler warnings
  2008-11-14   ag  Read remote features to determine 2.1 capability prior to any
                   other setup during connection complete processing
  2008-10-27   pg  Removed logic that was sending AFH channel map update events
                   to a non-existing bt app id.
  2008-10-27   pg  Fixed BT_EV_RM_RADIO_ACTIVITY event logic for SOC based BT.
  2008-10-27   gs  Fixed null bd address in device remove/all update event.
  2008-10-22   vk  Setting max sniff latency to 1.28 secs.
  2008-10-03   vk  Sending security complete event if max retries reached for 
                   RC and L2CAP security enforcement.
  2008-09-15   pg  Fixed multiple Compiler warnings.
  2008-09-09   NM  Apitize retrieval of link key from target for automation
  2008-09-03   gs  Modified device remove logic to clear all information in
                   both filesystem and RAM copy.
  2008-08-29   pg  Changed code to use clk_offset during ACL creation and
                   updated clk_offset on connection complete.
  2008-08-16   gs  Modified HCI command parameters for SCO/eSCO setup to latency
                   of 10ms and re-transmit effort of 1 (optimized for power)
  2008-08-21   pg  Added bt_cmd APIs to read remote feature and synchronous
                   connection parameters.
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse.
  2008-08-20   ag  - Make sure the existing connection is brought down when
                     we unbond from the remote connected device
                   - when a remote 2.1 device turns off encryption, disconnect
                     from it
  2008-08-18   vk  Setting Link Policy in case of Extended Low power config,
                   when App is requesting for Don't care option.
  2008-08-05   gs  Reset idle timeout after SCO connection is established.
                   Clear enh_enc_state flag if role switch is rejected by
                   the controller.  Fixed a few other issues in HCI disconnection
                   complete evt handling.
  2008-08-01   NM  Update security for legacy devices on link key notification
  2008-07-29   gs  Forcefully disconnect if ACL disconnect cmd fails after
                   max retry attempts.
                   Send RM_CONNECTION_FAILED_ACL event if RM_CONNECT_ACL
                   command fails after max-retry attempts.
  2008-07-28   sa  Resolved compiler warning because of unused variables.
  2008-07-16   gs  Updated bt_rm_get_conn_rm_handle() to fix race condition
                   in ACL disconnection.
  2008-07-01   rs  In case of non 2_1 devices once the Authentication happens
                   and if the Encr is not ON the security level is raised to
                   BT_RM_SL_1_LOW from NONE.
  2008-07-01   NM  Reject locally initiated sniff mode request if bonding is in
                   progress
  2008-06-27   vk  Handling sending of PIN code Request to App if its not
                   Prefetched in case of App initiating Bonding.
  2008-06-27   vk  Writing Link policy setting to controller once if disconnect
                   command is getting requeued.
  2008-06-10   vk  Checking if Role Switch is allowed before existing sniff.
  2008-05-21   ag  Added support for setting device security for JSR82 when 2.1
                   is enabled.
  2008-05-02   rs  Fixed Bonding issues with 2.0 and 2.1 devices in the enforce
                   security function mainly.
  2008-04-30   ag  Added code to mimic tester behavior; needed for 2.1
                   certification until BITE adds support for this
  2008-04-04   hm  Fixed klocwork errors
  2008-03-24   vk  Handling command complete event for HCI Read/write Defalut
                   Erroneous Data Reporting mode.
  2008-03-13   ag  Added support for MITM_NO_BONDING for GAP tests

 *
 *   #223         06 Mar 2008             VK
 * Doing Remote name request before sending PIN request to APP.
 *
 *   #222         05 Mar 2008             AG
 * Lisbon related changes:
 * - Once OOB data is read mark it as stale
 * - send service security results to app
 * - default authentication requirement for incoming dedicated bonding
 *   will now require MITM.
 * - added missing initialization for bonding_ss_ptr(CR 139736)
 *
 *   #221         29 Feb 2008             AG
 * Lisbon related changes:
 * - Changes from code review.
 * - Fixed SM2 connection with legacy device when security level is LOW
 * - Save link key even when authenticated link key expected, but not received
 * - Send io_cap_neg_reply with PAIRING_NOT_ALLOWED when app is in non-bondable
 *   state
 *
 *   #220         28 Feb 2008             VK
 * Do not expect Ecryption change events during Role Switch if Both Local
 * and Remote Devices support Pause Encryption Feature.
 *
 *   #219         21 Feb 2008             VK
 * Fixing Lint Errors.
 *
 *   #218         21 Feb 2008             VK
 * Setting supervision timeout to 5 sec for SOC targets.
 *
 *   #217         07 Feb 2008             GB
 * 2.1 - Read Local OOB Data Command Cmplt event is now sent to OEM
 * irrespective of Bondable state.
 *
 *   #216         07 Feb 2008             MH
 * Initiate SSR emulation if sniff initiator orientation is not set
 *
 *   #215         01 Feb 2008             AG
 * 2.1 - MITM requirement from initiator was getting clobbered in the
 * responder during re-bonding.
 *
 *   #214         28 Jan 2008             VK
 * Fixing Lint error.
 *
 *   #213         24 Jan 2008             VK
 * Adding missed case handling in earlier checkin for
 * BT_EV_HC_RD_REMOTE_VERSION_COMPLETE.
 *
 *   #212         17 Jan 2008             AG
 * Fix for disconnecting device when it is deleted from the device database.
 * 2.1 was breaking this in some legacy case.
 *
 *   #211         17 Jan 2008             AG
 * Featurize previous EIR fix.
 *
 *   #210         11 Jan 2008             HM
 * Not freeing EIR memory during a remove device
 *
 *   #209         11 Jan 2008             AG
 * Serialize HCI command read remote name request in case of 2.1
 *
 *   #208         07 Jan 2008             CO
 * Enhanced SSR implementation.
 *
 *   #207         17 Dec 2007             GS
 * Disable remote initiated sniff/park/hold and role-switch while processing
 * ACL disconnect command.
 *
 *   #206         14 Dec 2007             VK
 * Setting default supervision timeout to 5 sec for SOC targets.
 *
 *   #205         05 Dec 2007             AG
 * Added APIT support for 2.1.
 * Don't save debug link key if mode not enabled.
 *
 *   #204         05 Dec 2007             RB
 *  Fix warning - Initialize an uninitialized variable
 *
 *   #203         04 Dec 2007             MH
 * Minor correction in low power log message.
 *
 *   #202         27 Nov 2007             PN
 * Link status update event would now be sent when SCO connection state changes
 * or role switch policy changes.
 *
 *   #201         27 Oct 2007             PG
 * Added "manual control" logic to bt_rm_cmd_low_power_config cmd.
 *
 *   #200          05 Nov 2007            AG
 * Fixed previous changes for BT2.1
 *
 *   #199          05 Nov 2007            AG
 * Fixed RVCT21 compiler warning
 *
 *   #198          02 Nov 2007            PG
 * Removed unused RM cmds related to AFH implementation in embedded controller.
 * Fixed incorrect merge from an earlier checkin in bt_rm_background tick().
 *
 *   #197          01 Nov 2007            AG
 * Merged Lisbon(BT2.1) changes into the mainline
 *
 *   #196          01 Nov 2007            PG
 * Fixed acl data not getting sent due to "race condition" in entry to park
 * state.
 *
 *   #195          30 Oct 2007            WFD
 * Wrap calls to snd_aux_pcm_clk_*() functions with BT_SWDEV_NO_AUXPCM_CLK_CTRL
 *
 *   #194          19 Oct 2007            DSN\SSK
 * Handle comamnd complete events for Controller to Host flow control.
 *
 *   #193           09 Oct  2007           AP
 * Removed lint warning for scatternet enabled builds
 *
 *   #192           05 Oct  2007           PG
 * Changed sniff attempt parameter to 2 for optimizing current consumption.
 *
 *   #191           02 Oct  2007           BK
 * Added bt_rm_test_mode_enabled() to support check for test mode so
 * driver is not disabled if test mode is enabled.
 *
 *   #190           14 Sept 2007           PG
 * Corrected rssi timeout update logic in previous checkin
 *
 *   #189           12 Sept 2007           PG
 * Changed BT link RSSI update logic for SOC targets. HCI cmd to read
 * RSSI is sent periodically when link is active only provided app has
 * registered for RSSI updates.
 *
 *   #188           12 Sept 2007           PG
 * Changed must_be_settled param to bt_rm_get_conn_bd_addr() to false in
 * places where the connection is not required to be "settled".
 *
 *   #187           11 Sept 2007           BK
 * Implement QSoC Driver State state machine.
 *
 *   #186           04 Sept 2007           BH
 * Added support for QSoC vender specific encryption key length command.
 *
 *   #185           27 Aug 2007            PG
 * Changed inquiry result processing logic to handle num_response
 * parameter greater than 1.
 *
 *   #184           27 Aug 2007            PG
 * Fixed connect cmd getting ignored when it was received while a
 * disconnect cmd is already reque'd for the same device awaiting
 * unsniff to complete.
 *
 *   #183           23 Aug 2007            PG
 * Added BT link RSSI update support in link status for SOC targets.
 *
 *   #182          20 Aug 2007            PG
 * Seperated HCI generic vendor specific event handling from Qualcomm
 * proprietary implementation used with embedded controller.
 *
 *   #181          24 Aug 2007            VK
 * Changing Max Latency to 7 msec to accommodate one re-transmission.
 *
 *   #180          24 Aug 2007            VK
 * Fixing sending of invalid command param for "Accept Synchronous
 * Connection Request Command"
 *
 *   #179          24 Aug 2007            VK
 * Checking for remote features before issuing Sniff command to controller.
 *
 *   #178          17 Aug 2007            PG
 * Fixed HCI_Write_Link_Policy cmd getting sent with invalid HCI connection
 * handle.
 *
 *   #177          14 Aug 2007            PG
 * Fixed 2 disconnect ACL cmds getting qued during device remove.
 *
 *   #176          09 Aug 2007            PG
 * Disallowed role switch cmd when synchronous connection is present on the
 * same link.
 *
 *   #175          08 Aug 2007            PG
 * For QSOC, always accept role switch cmd from app when HOST_DIS_SCATTERNET
 * is not defined.
 *
 *   #174          07 Aug 2007            WFD
 * Add include of sndi.h to declare snd_aux_pcm_clk_enable() and _disable().
 * Cleaned up compile issues due to missing '}'.
 *
 *   #173          03 Aug 2007            PG
 * Fixed issues with Radio off cmd and HCI HW error recovery handling for SURF
 * builds with 7500/BC4 flash.
 *
 *   #172          30 Jul 2007            WFD
 * Request that AuxPCM clocks be enabled by AVS when a SCO connection occurs.
 * This will ensure that the clock is being sent from AMSS before the SoC begins
 * processing PCM data.
 *
 *   #171          20 Jul 2007            VK
 * Authenticating link before sending Set Connection Encryption command if
 * link is not already authenticated.
 *
 *   #170          13 Jul 2007            VK
 * Putting extra checks to find if Role switch has failed.
 *
 *   #169          12 Jul  2007           SSK
 * Disable Radio-Off/Radio-On on BC4 when FEATURE_BT_SOC1_BC4_DEBUG is defined
 * in order to obtain coredump info from BC4 SOC for debugging.
 *
 *   #168          11 Jul 2007            VK
 * Allowing all remote supported synchronous packet types in
 * "Set Synchronous Connection " and "Accept Synchronous Connection" commands.
 *
 *   #167          09 JUL 2007            PG
 * HCI Remote name req cancel and connection cancel support.
 *
 *   #166          10 Jul 2007            VK
 * Featurizing Peeking of Encryption key length only when
 * FEATURE_BT_EXTPF_SAP defined.
 *
 *   #165          03 Jul 2007            SSK
 * Fix HCI_RESET bug in HCI APIT Mode on BTS targets where the SOC does not power
 * back on after HCI RST cmd is sent.
 *
 *   #163-164       29 Jun 2007           PG
 * Disallowed inquiry cmd when HOST_DIS_SCATTERNET is defined.
 *
 *   #162          28 JUN 2007            VK
 * Doing Remote name request Req after connection complete.
 *
 *   #161          19 Jun 2007            PN
 * Fixed featurization problem for non-SoC targets.
 *
 *   #160          06 Jun 2007            DSN
 * Support vendor-specific read of Enc key Length from QSOC.
 *
 *   #159          07 Jun 2007            SSK
 * Implement Radio-Off & Radio-On to recover from SOC HW error event.
 *
 *   #158          04 JUN 2007            VK
 * Updating Device database with LMP feature and version for SOC targets.
 *
 *   #157          30 MAY 2007            PG
 * Featurized code to prevent scatternet from HCI host when the controller
 * is allowing scatternet.
 *
 *   #156          30 May 2007          VK
 * Requesting PIN from APP before Switching Off Encryption in case of re-pairing.
 *
 *   #155          18 May 2007            PR
 * Changed default max interval for low power modes
 *
 *   #154          17 MAY 2007            PG
 * Always set the BT_RM_LU_ROLE_B of the link status update flag when role
 * switch operation finishes.
 *
 *   #153          16 MAY 2007            VK
 * Fetching Remote LMP feature after Conn Complete event and Using the
 * same for deciding SCO packet type in Set Sync Connection command.
 *
 *   #151-152      16 MAY 2007            VK
 * Fixing Null pointer access in bt_cmd_rm_device_read_iter.
 *
 *   #150          05 MAY 2007            PG
 * Changes to remove compiler warnings with RVCT.
 *
 *    #149         05 May 2007            PG
 * Returned appropriate cmd status (instead of RM_BUSY) when role switch cmd
 * is not allowed due to existing piconet restrictions.
 *
 *    #148         04 May 2007            PG
 * Changed code not to modify scan window for SCO when QSOC is being used.
 * Changed mapping between bt_rm_cmd_config_application() parameters to
 * the parameters used for scanning.
 *
 *    #147         19 Apr 2007            PG
 * Featurized out code related to Qualcomm Vendor Specific commands for
 * SOC targets.
 *
 *    #146        16 April 2007          SSK
 * Log state of SOC1 Driver when hardware error event is received from SOC1.
 *
 *    #145        13 April 2007          DM
 * Change to not select 2/3-EVx packet types in Sync Conn Create and
 * Accept commands.
 *
 *    #144        12 April 2007          DM
 * Send an Inq Cmplt Event with Failed Status when Inquiry Cmd Gets rejected
 * by the Controller.
 *
 *    #143        13 April 2007          JN
 * Fixed radio on/off support for BTS4020. Fixed formatting.
 *
 *    #142        09 April 2007          PG
 * Added logic to act on the parameters received in
 * bt_rm_cmd_config_application api. This api lets an app to choose a
 * tradeoff between power and response time for iquiry and page scan.
 *
 *    #141        03 April 2007          VK
 * Allowing Re-Bonding.
 *
 *    #140        14 Mar  2007           JN
 * Fix indentation in bt_rm_ev_hc_qualcomm.
 *
 *    #139        14 Mar  2007           JN
 * Process radio activity event for SOC featurization. This in turn the fixes
 * the GUI update for device discoverable mode(defect #D4627). Fix was
 * provided by DS.
 *
 *    #138        14 March 2007          VK
 * Initializing security setting in test mode.
 *
 *    #137        09 March 2007          VK
 * Handling link key req event after recieving Link key notification
 * event but before Bonding completes.
 *
 *    #136        02 Mar  2007           JN
 * For HCI_RESET event, all call to SoC abstraction interface for SoC
 * specific processing.
 *
 *    #135        05 March 2007          VK
 * Removing Packet type management from RM for SOC targets.
 *
 *    #134        01 Mar 2007            PG
 * Enabled sending sniff req after idle timeout even when SCO is up if
 * FEATURE_BT_SOC is enabled.
 *
 *    #133        01 Mar 2007            DSN
 * Process event response for HCI Read Buffer Size command.
 *
 *    #132        23 Feb 2007            DM
 * Change packet type parameter in sync_conn creation to force only EVx packet
 * when ecso is attempted.
 *
 *    #131        22 Feb 2007            DM
 * Fixing Compiler warning.
 *
 *    #130        21 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #129        20 Feb 2007            DSN
 * Increasing the maximum number of events in RM layer for SOC solutions.
 *
 *    #128        09 Feb 2007           ACG
 * Fix compiler warning related to assignment statement bug.
 *
 *    #127        08 Feb 2007           DM
 * Changed max latency parameter on setup_sync_conn so as to allow ESCO
 * with EV3 and 1 retransmission or HVx with sco.
 *
 *    #126        31 Jan 2007           ACG
 * Rename function bt_rm_check_driver_idle_timeout to
 * bt_soc_etc_check_driver_idle_timeout.
 *
 *    #125        25 Jan 2007           VK
 *  RM starts re-pairing according to PIN length requirement in security mode 2.
 *
 *    #124        18 Jan 2007           VK
 * Enhancing Encryption in security Mode 3 if remote device disables the same
 * and in case of role switch command for a "encrypted ACL connection"
 * making connection unstable till all the three steps of Role switch completes.
 *
 *    #123        10 Jan 2007           ACG
 * Fix compile issues on 6k targets caused by 7500 related code check-ins.
 *
 *    #122        08 Jan 2007           ACG
 * Following features were added for the SOC solution:
 * - vote for SIO sleep/wakeup based on BT activity
 * - enable/disable driver
 * - radio on/off
 *
 *   #121         02 Jan 2007           VK
 * Removed by default sending "-ve link key reply" in SOC scatternet case.
 *
 *   #120         14 Dec 2006           DSN
 * Update radio activity mode for SOC solution.
 *
 *   #119         12 Dec 2006           SSK
 * RM\LM cleanup change that implements miscellaneous HCI commands on external
 * SOC.
 *
 *   #118         06 Dec 2006           VK
 * RM re-negotiate max slot in active mode only.
 *
 *   #117         22 Nov 2006           DM
 * Passing Timeout as number of slots to bt_hc_cmd_wr_auto_flush_to().
 * Handling of command complete for HCI Flush Command
 *
 *   #116         16 Nov 2006           RY
 * Initialize num_uuid128 to 0 prior to calls to bt_cmd_sd_search_service.
 *
 *   #115        16 Nov 2006           DM
 * Passing local_cid and frame_handle to bt_cmd_hc_tx_acl_data(),
 * Required for AV-Sync
 *
 *   #114        08 Oct 2006           VK
 * RM sends more appropriate error codes, when bonding fails.
 *
 *   #113        06 Nov  2006          PG
 * "disable scatternet and enable AMS" with SOC - take 2. Reverted changes
 * from #109. While in AMS mode, if switch to master has not succeeded,
 * incoming connection is now rejected.
 *
 *   #112        31 Oct  2006          VK
 * Fixing compilation conflict of bt_rm_atzrq_type enum.
 *
 *   #111       26 Oct 2006            VK
 * Added connection direction information in BT_EV_RM_AUTHORIZE_REQUEST event.
 *
 *   #110       20 Oct 2006            SSK
 * Featurize out Vendor Specific Qualcomm commands.
 *
 *   #109       17 Oct 2006            PG
 * Added framework to set different scan intervals when WLAN is detected to be
 * active. Workaround to "disable scatternet and enable AMS" with SOC.
 *
 *   #108       13 Sep 2006            MP
 * Changes are made to route the L2CAP ack packets properly to
 * corresponding registered Apps
 *
 *    #106       24 August 2006         VK
 * Removed err Message from bt_rm_hc_ev_qc_cur_conn_encryption.
 *
 *    #105       09 August 2006         VK
 * Link key is not deleted when authentication fails.
 *
 *    #104       26 Jul 2006            DM
 * Added bt_rm_ev_hc_tx_data_cfm
 * Changes to bt_rm_cmd_tx_data_acl to take in packet boundary flag
 *
 *   #103        27 Jun 2006           DSN
 * Added support for HostController SoC solution.
 *
 *  #102        15 Jun 2006            MH
 * Changed default Sniff max interval
 *
 *  #101       14 Jun 2006            JH
 * Added proper eSCO featurization.
 *
 *    #100       01 Jul 2006            JTL
 * Include CID in L2Cap Mode 2 security. Retry mode 2 security for L2Cap if
 * authorization is already in progress.
 *
 *    #99        15 May 2006            PN
 * When no BD address is given, Connection Role Switch Request setting makes
 * role switch possible on any future connection, during and after connection
 * setup.
 *
 *    #98        4  May 2006            DP
 * Clear authorization state after each Mode 2 security update cycle.  This
 * allows multiple authorizations based on L2CAP PSMs over the same L2CAP
 * connection.
 *
 *    #97       12 Apr 2006            JH
 * Always include btl2.h to eliminate compiler warning due to missing function
 *  prototype.
 *
 *    #96       12 Apr 2006            AP/PG
 * Fix for lingering L2CAP connections setup for L2PING
 *
 *    #95        5  Apr 2006            DP
 * Added support for Extended Service Security.  Added Remove All Devices
 * command.
 *
 *    #94        16 Mar 2006            JH
 * Properly handle eSCO links in bt_rm_ev_hc_connection_request().
 *
 *    #93        14 Mar 2006            JH
 * Modified the RM connect SCO command to support attempting an eSCO connection.
 *
 *    #92        2  Mar 2006            DP
 * Fix disconnect upon unbonding a connected device and when white
 * or black list updates want to disconnect a connected device.
 * Make service file names have 2 digits for the device index
 * on the end of them.  Fix merge error with previous checkin.
 *
 *    #91        27 Feb 2006            DP
 * Prevent re-pair for cases where PIN Reply results in negative
 * reply to HC.  Make 0 length PIN reply become negative reply.
 * Disallow bond command with 0 length PIN.
 *
 *    #90        30 Jan 2006            PN
 * Radio Activity event no longer gets sent as part of Disable Radio command.
 *
 *    #89        30 Jan 2006            PN
 * Added functions to register for, unregister for, and get radio activities.
 *
 *    #88        21 Dec 2005            DP
 * Changed HC QC Remote Auth or Pairing Failed event to use BD address
 * instead of HC handle to identify the connection.  Added HC Write Page
 * Timeout commands to list of types expected for HC Command Complete
 * events.
 *
 *    #87        14 Dec 2005            DSN
 * Added featurization for MSM7500 BT bringup.
 *
 *    #86        12 Dec 2005            DP
 * Automatic SDP query after bonding uses L2CAP UUID instead of RFCOMM
 * UUID so that services based on L2CAP are discovered and stored in
 * device database.
 *
 *    #85        23 Nov 2005            DP
 * Added RM commands for White List and Extended Service Security.
 * Black and White list functionality now operative.  Extended
 * Service Security functionality not complete yet.
 *
 *    #84        17 Nov 2005            JH
 * Featurized code which initiates/accepts eSCO connection under BT_SWDEV_ESCO.
 *
 *    #83        9  Nov 2005            DP
 * Page timeout no longer depends on remote page scan repetition
 * mode.  Inquiry length is no longer doubled without SCO.  A bond
 * failed event is now sent if HC disallowed the create connection
 * associated with a bond command.
 *
 *    #82        24 Oct 2005            JH
 * For BT 1.2, added code to process HCI Sync Conn Req Complete and Sync Conn
 *  Changed events; also make use of HCI Accept Sync Conn Req and Reject Sync
 *  Conn Req commands for both SCO and eSCO links.
 *
 *    #81        18 Oct 2005            JH
 * Added call to bt_rm_finish_service_security() to cleanup following failed
 *  pairing attempt when BT_EV_HC_QC_REM_AUTH_OR_PAIR_FAILED event is processed.
 *
 *    #80        13 Oct 2005            DP
 * Device Services Update commands now return status correctly.  Accept
 * SCO commands are now requeued when ACL packet types are not settled.
 * HC Write Page Timeout command status events are now expected.
 *
 *    #79        27 Sep 2005            DP
 * Added initial implementation for Black List.
 *
 *    #78        27 Sep 2005            DP
 * Double page/inquiry scan windows and page/inquiry timeouts
 * whenever a SCO link is currently present.
 *
 *    #77        21 Sep 2005            JTL
 * Always send out packet change messages, even if in sniff. Increase the retry
 * time to 30 seconds from 5.
 *
 *    #76        15 Sep 2005            DP
 * Fix bt_rm_find_conn_bd_addr() to check state before BD Addresses
 * to prevent dereferencing NULL pointer at initialization.
 *
 *    #75        14 Sep 2005            JH
 * Added support for eSCO; replaced bt_cmd_hc_add_sco_connection() with
 *  bt_cmd_hc_setup_sync_conn().
 *
 *    #74        14 Sep 2005            DP
 * Force packet type changes even on links in sniff when going
 * to 1 slot packets.
 *
 *    #73        22 Aug 2005            JH
 * Updated BT_RM_DEV_DB_VERSION to 7 due to the increase of BT_RM_MAX_DEVICES
 *  to 30.
 *
 *    #72        03 Aug 2005            GS
 * Mainlined mDSP reset handling functionality for UMTS targets.
 *
 *    #71        28 Jul 2005            DP
 * Adjust system monitoring timeouts to expire in order from lowest
 * to highest layers.
 *
 *    #70        28 Jul 2005            DP
 * Disconnect upon device unbond and remove.  Make sure security
 * information (e.g., link key) are removed from RAM as well in
 * both of these cases.
 *
 *    #69        27 Jun 2005
 * Added new BT 1.2 error codes in bt_rm_unbond_performed().
 *
 *    #68        24 Jun 2005            PN
 * Inquiry will not get started if device DB is full.
 *
 *    #67        22 Jun 2005            PN
 * Fixed problem of inadvertently sending Bond Failed event.  Fixed problem of
 * not sending event when bonding to a currently connected device.
 *
 *    #66        16 Jun 2005            DP
 * Added ACL packet type control for all connections in a piconet when
 * SCO is started and/or stopped.  Implemented RSSI delta link status
 * updates.
 *
 *    #65        01 Jun 2005            GS
 * Moved part of radio disable functionality to bt_hc_reset_driver().
 *
 *    #64        29 Apr 2005            PN
 * Added bt_rm_update_rssi().
 *
 *    #63        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro (improving upon previous checkin's
 * use of similar debug message macros).
 *
 *    #62        7  Apr 2005            DP
 * Disable park mode when master automatically.  Make max slots adjustment
 * occur prior to SCO link coming up.  Mode 2 security automatically begins
 * re-pair by unbonding upon failure to secure link first time. Make use
 * of new BDA debug message macros.
 *
 *    #61        06 Apr 2005            JH
 * Adding support for disabling BT radio, featurized using BT_SWDEV_RADIO_OFF.
 *
 *    #60        26 Mar 2005            JTL
 * Better parameter checking in commands.
 *
 *    #59        16 Mar 2005            DP
 * Added Service Security Result event.  Fixed Mode 2 security for client
 * connections.
 *
 *    #58        5  Mar 2005            JH
 * Added code for processing commands: bt_cmd_rm_config_afh,
 *  bt_cmd_rm_enable_afh, bt_cmd_rm_set_afh_ch_map and bt_cmd_rm_get_afh_ch_map.
 * Added code for processing events: BT_EV_RM_AFH_CH_MAP_SET and
 *  BT_EV_RM_AFH_CH_MAP
 *
 *    #57        7  Feb 2005            PN
 * QC extended command status now supplements some HCI failure command status.
 * ACL/SCO connection requests are now rejected when no apps are registed.
 *
 *    #56        7  Feb 2005            PN
 * Now allow only one-slot data packet types on all connections when SCO is
 * present.
 *
 *    #55         5 Jan 2005            JH
 * Added support for commands BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE and
 *  BT_CMD_RM_DEVICE_SERVICES_UPDATE.
 * Ensure dev_public.valid is true before writing to service file in
 *  bt_rm_write_service_file().
 * Added bt_rm_finish_service_update().
 *
 *    #54        17 Dec 2004            JH
 * Removed change made in #53; instead, if necessary, set update_status to
 * BT_RM_DUS_SERVICES_B to force service update the next time device is updated.
 *
 *    #53         8 Dec 2004            JH
 * Clear num_services in the device structure in bt_rm_cmd_device_remove().
 *
 *    #52        23 Nov 2004            JH
 * Fixed mispelt BT_AMS_DISABLED in bt_rm_check_scan_settings().
 *
 *    #51        17 Nov 2004            PN
 * Cleans up properly when Inquiry fails.
 *
 *    #50        26 Oct 2004            DP
 * Prevent automatic mode change to sniff when LMP is busy.
 *
 *    #49        24 Sep 2004            DP
 * Reworked AMS compilation controls.  Implemented disabling AMS.
 *
 *    #48        25 Aug 2004            DP
 * Filter class of device updates to ignore CODs of all zeroes.
 *
 *    #47        3  Aug 2004            PN
 * bt_cmd_rm_device_read_iter now correctly filters service class.
 *
 *    #46        30 Jul 2004            JH
 * Added support to enable sending ACL data in Sniff mode (SNIFF_LOCK state).
 * Added support for reading RSSI from DSP in bt_rm_copy_link_status.
 *
 *    #45        28 Jul 2004            DP
 * Backout device read iter changes.
 *
 *    #44        28 Jul 2004            DP
 * Make sniff attempt and timeout units correct.  Made device read iteration
 * consistent with device read with regard to the device valid field and
 * the return value of the function.
 *
 *    #43        22 Jun 2004            DP
 * Fixed service level security.
 *
 *    #42        2  Jun 2004            DP
 * Fixed default UUID/SCN pairing.
 *
 *    #41        1  Jun 2004            DP
 * Implemented Mode 2 security.  Stop Inquiry now sends a inquiry complete
 * event.  Fixed comments and improved a couple of routine names.  Also
 * improved debug information for inquiry results.
 *
 *    #40        17 May 2004            PN
 * Removed restrictions on targets with T_MSM6200 defined.
 * Increased BT_RM_NUM_EVENTS to 40.
 *
 *    #39        30 Mar 2004            DP
 * Improved API for role setting and role switch request allowing to
 * provide flexibility required for EMS support.
 *
 *    #38        18 Mar 2004            DP
 * Update device class in device database upon remote connection requests.
 *
 *    #37        6  Mar 2004            DP
 * Properly clear PIN request timeout.
 *
 *    #36        3  Mar 2004            DP
 * RM Bonded or Bond Failed event sent as response to PIN Reply
 * commands for all cases now.
 *
 *    #35        3  Feb 2004            DP
 * Fixed precision of inquiry scan interval conversion.
 *
 *    #32-34     30 Jan 2004            DP
 * Fix remote name event handling.
 *
 *    #31        29 Jan 2004            DP
 * Made device remove clear security information for device in RAM.
 * Added remote name request to connection setup sequence.  Class
 * of Device is now parsed into the device database fields for
 * inquiry responses.  Device update searches the device table with
 * appropriate parameters now.  Device read iter now can include
 * volatile devices.  Inquire command properly rounds the duration
 * parameter before forwarding to HC/BB.  Disconnecting ACL links
 * automatically disconnects associated SCO links without requiring
 * the APP ID to match.  Device database correctly detects when the
 * file version changes.
 *
 *    #30        14 Jan 2004            DP
 * Added support for limited discoverable scanning.
 *
 *    #29        12 Jan 2004            DP
 * Fixed command to set role switch allowed flag.
 *
 *    #28        19 Dec 2003            DP
 * Initialize link status registration table.
 *
 *    #27        19 Dec 2003            DP
 * Removed compiler warning and added this comment (2 revisions).
 *
 *    #25        19 Dec 2003            DP
 * Added RM command to allow role switch requests from remote devices.
 * Fixed naming convention on LMP policy definitions.
 *
 *    #24        12 Dec 2003            DP
 * Added busy timeout to ensure RM never remains busy indefinitely.
 * Fixed HC connection request event processing for reject case.
 * Fixed HC disconnect complete event processing when status not good.
 * Consolidated some event processing to facilitate busy timer expiration
 * processing.  Fixed Device Remove command for case when device currently
 * in a link. Send connection fail event if device table full for connect
 * ACL & SCO commands.  Automatically intiate SCO disconnect in disconnect
 * ACL command if SCO connected.
 *
 *    #23        26 Nov 2003            DP
 * Improved device read command and internal get device function.
 * Improved error/debug messages.  Get local info now null terminates
 * the device name.
 *
 *    #22        14 Nov 2003            DP
 * Page/Inquiry scan intervals adjusted when only doing one type
 * of scan to reduce power consumption.
 *
 *    #20        12 Nov 2003            DP
 * Fixed security handling, particularly after bonding.
 *
 *    #18        4  Nov 2003            DP
 * Added security APIs and link status reporting and role switch API.
 *
 *    #17        3  Oct 2003            DP
 * Converted RM timer to background timer.  Added SD Timeout response
 * event processing.
 *
 *    #15        26 Sep 2003            DP
 * Updated RM Device Manager API and added unbond and config application
 * commands.  Made service/device names character strings.
 *
 *    #14         06 Aug 2003           PN
 * Added support for Device DB.
 *
 *    #13         22 Jul 2003           RY
 * Updated bt_rm_refresh_device to handle the case when the bonded device
 * record is not found.
 *
 *    #12         16 May 2003           PN
 * Fix problem of referencing NULL pointer in bt_rm_get_conn_hc_handle().

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include "assert.h"
#include "dsm.h"
#include "nv.h"
#include "task.h"
#include "rex.h"
#include "bt.h"
#include "bti.h"
#include "btcomdef.h"
#ifdef BT_SWDEV_2_1_EIR
#include "memheap.h"
#endif /* BT_SWDEV_2_1_EIR */
#include "btsd.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "btevi.h"
#ifdef FEATURE_BT_SOC
#include "btsocetc.h"
#include "bthcdrv.h"
#include "btsoc.h"
#ifdef FEATURE_BT_QSOC
#include "btqsoc.h"
#endif /* FEATURE_BT_QSOC */
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */
#else
#include "btlm.h"
#include "btqddata.h"
#endif /* FEATURE_BT_SOC */

#include "btl2.h"
#include "btmsg.h"
#include "btrm.h"
#include "btsd.h"
#include "btutils.h"
#include "bthci.h"

#ifdef FEATURE_EFS
#include "fs.h"
#endif /* FEATURE_EFS */

#ifdef BT_TEST_PYLD_FAST_TX
#ifndef FEATURE_BT_SOC
#include "bthcetc.h"
#endif /* !FEATURE_BT_SOC */
#endif

#ifdef BT_SWDEV_RADIO_OFF
#ifndef FEATURE_BT_SOC
#include "btbb.h"
#include "bthchcbc.h"
#include "btqdspq.h"
#include "btse.h"
#endif /* !FEATURE_BT_SOC */
#include "bthc.h"
#include "bthcpe.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef FEATURE_BT_QSOC
#include "snd.h"
#endif /* FEATURE_BT_QSOC */

#ifdef FEATURE_BT_DCVS
#include "btdcvs.h"
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define BT_MSG_LAYER BT_MSG_RM

#undef BT_SWDEV_RM_UNTESTED

#define BT_RM_BUSY_TIMEOUT_MS      65000

#define BT_RM_BB_OPT_TIMEOUT_MS    30000

#ifdef BT_SWDEV_2_1_SSP
#define BT_RM_REQ_PASSKEY_TIMEOUT_MS 120000
#endif
#define BT_RM_REQ_TIMEOUT_MS       25000
#define BT_RM_MSS_ENC_TIMEOUT_MS   2500    /* 2.5 secs */
#ifdef BT_SWDEV_2_1_SSR
#define BT_RM_DEF_MAX_INTERVAL_MS  640
#define BT_RM_DEF_MAX_LATENCY      (BT_RM_DEF_MAX_INTERVAL * 2)
#else
#define BT_RM_DEF_MAX_INTERVAL_MS      1280
#endif /* BT_SWDEV_2_1_SSR*/
#define BT_RM_DEF_MAX_INC_INTERVAL_MS  2560

#define BT_RM_DEF_MIN_INTERVAL_MS  500
#define BT_RM_DEF_SNIFF_ATTEMPT    0x0002  /*  Approximately 1.25 ms.  */
#define BT_RM_DEF_SNIFF_TIMEOUT    0x0008  /*  Approximately 10 ms.  */
#define BT_RM_REMOTE_FEATURE_LEN   8

#define BT_RM_DEF_IDLE_TIMEOUT_MS (7 * 1000)
#define BT_RM_DEF_IDLE_MODE       BT_LM_SNIFF
#ifdef BT_SWDEV_2_1_SSR
#define BT_RM_DEF_SNIFF_RETRY_TO_MS (4 * 1000)
#define BT_RM_DEF_EMU_SNIFF_MODE_TO_MS (7 * 1000) /* Used for SSR emulation */
#define BT_RM_DEF_REMOTE_TIMEOUT  \
           (BT_RM_MS_TO_625USEC( BT_RM_DEF_IDLE_TIMEOUT_MS )) // TBD
#define BT_RM_DEF_LOCAL_TIMEOUT  \
           (BT_RM_MS_TO_625USEC( BT_RM_DEF_IDLE_TIMEOUT_MS )) // TBD
#define IS_SSR_DOABLE() \
           ((bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1) && \
            (conn_ptr->dev_ptr->dev_public.ssp_capable == TRUE))
#else
#define IS_SSR_DOABLE() (FALSE)
#endif /* BT_SWDEV_2_1_SSR*/ 

#ifdef BT_SWDEV_2_1_SSP
#define IS_SSP_DOABLE() \
            ((bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1) && \
            (conn_ptr->dev_ptr->dev_public.ssp_capable == TRUE))
#else
#define IS_SSP_DOABLE() (FALSE)
#endif /*BT_SWDEV_2_1_SSP*/

#define BT_RM_RSSI_UPD_TIMEOUT_MS  2500 /* 2.5 secs */

#define BT_RM_MS_TO_625USEC( ms ) ((ms * 8) / 5)
#define BT_RM_625USEC_TO_MS( six25usec ) ((six25usec * 5) / 8)

#define BT_RM_DEF_MAX_INTERVAL  \
           (BT_RM_MS_TO_625USEC( BT_RM_DEF_MAX_INTERVAL_MS ))

#define BT_RM_DEF_MAX_INCOMING_INTERVAL  \
           (BT_RM_MS_TO_625USEC( BT_RM_DEF_MAX_INC_INTERVAL_MS ))

#define BT_RM_DEF_MIN_INTERVAL  \
           (BT_RM_MS_TO_625USEC( BT_RM_DEF_MIN_INTERVAL_MS ))

#define BT_RM_DEF_MIN_INCOMING_INTERVAL  6

#define BT_RM_DEF_LINK_POLICY_M ( BT_LINK_POLICY_SNIFF_MODE_B |  \
                                  BT_LINK_POLICY_HOLD_MODE_B  )


#define BT_RM_INVALID_RSSI 127  /*  Valid is 0 to -127 dBm  */

#ifdef FEATURE_BT_1_2

#define BT_RM_SCO_BANDWIDTH         64000
#define BT_RM_SCO_LATENCY_DONT_CARE 0xFFFF

#endif

#ifdef BT_SWDEV_2_1_EIR
#define BT_RM_EIR_HEAP_SIZE   2048
#endif /* BT_SWDEV_2_1_EIR */

#ifdef BT_SWDEV_2_1_SSP
#define BT_RM_LMP_FEATURE_BIT_SSP      51
#define BT_RM_LMP_FEATURE_BIT_SSP_HOST 0
#define BT_RM_LMP_EXT_FEATURES_PAGE_1  1
#define BT_RM_MAX_REPAIR_ATTEMPTS      2
#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_2_1_EPR
#define BT_RM_LMP_FEATURE_BIT_PAUSE_ENCRYPTION      42
#endif /* BT_SWDEV_2_1_EPR */

#ifdef BT_SWDEV_2_1_EPR
#define BT_MAX_EK_AGE_LO      7*3600*2    /* 7 hrs */
#define BT_MAX_EK_AGE_HI      23*3600*2   /* 23 hrs */
#define BT_MAX_EK_AGE_STALE   BT_MAX_EK_AGE_HI + 2160   /* > 23.3 hrs */
#endif /* BT_SWDEV_2_1_EPR */

#define BT_RM_UPD_RA_AND_SETBIT( ra_bit_mask )                \
do                                                            \
{                                                             \
  bt_rm_data.current_radio_activity |= ra_bit_mask ;          \
  /* Not idle any more */                                     \
  bt_rm_data.current_radio_activity &= ~( BT_RA_RADIO_IDLE ); \
} while(0)

#define BT_RM_UPD_RA_AND_CLRBIT( ra_bit_mask )                \
do                                                            \
{                                                             \
  bt_rm_data.current_radio_activity &= ~( ra_bit_mask );      \
  /* If we are not inquiring/paging/scanning we are idle */   \
  if(( bt_rm_data.current_radio_activity &                    \
     ( BT_RA_INQUIRING | BT_RA_PAGING | BT_RA_INQ_SCANNING |  \
       BT_RA_PAGE_SCANNING )) == 0 )                          \
  {                                                           \
      bt_rm_data.current_radio_activity |= BT_RA_RADIO_IDLE;  \
  }                                                           \
} while(0)

#ifdef BT_SWDEV_2_1_EIR
/* protect against multiple calls to bt_cmd_rm functions */
LOCAL rex_crit_sect_type bt_rm_crit_sect;

/* Heap for dynamic memory allocation */
mem_heap_type   bt_rm_eir_heap;
LOCAL char      bt_rm_eir_heap_buf[ BT_RM_EIR_HEAP_SIZE ];
#endif /* BT_SWDEV_2_1_EIR */

#ifdef BT_SWDEV_2_1_SSR
#define BT_RM_SSR_NOT_ALLOWED      0
#define BT_RM_SSR_DONT_CARE        1
#endif /* BT_SWDEV_2_1_SSR */

/*  Information and queue for events destined for the RM layer.  */
LOCAL bt_event_q_info_type bt_rm_ev_q_info;
LOCAL q_type               bt_rm_ev_q;

LOCAL void bt_rm_cmd_disable_radio
(
  bt_cmd_msg_type*  rm_dr_ptr
);

LOCAL void bt_rm_cmd_unbond
(
  bt_cmd_msg_type*  rm_ub_ptr
);

/* Indicate the BT radio state when HW error event has been recieved */
LOCAL boolean bt_rm_hwerr_radio_off = FALSE;

bt_app_id_type bt_rm_app_id;

typedef enum
{
  BT_RM_CS_DISCONNECTED,
  BT_RM_CS_CONNECTING,
  BT_RM_CS_CONNECTED,
  BT_RM_CS_DISCONNECTING
} bt_rm_conn_state_type;

typedef enum
{
  BT_RM_RE_PAIR_PIN_NONE,
  BT_RM_RE_PAIR_PIN_WAITING,
  BT_RM_RE_PAIR_PIN_READY
} bt_rm_re_pair_pin_type;

#ifdef BT_SWDEV_2_1_SSR
typedef enum
{
  BT_RM_SSR_NOT_IN_PROGRESS,
  BT_RM_SSR_ENABLED
} bt_rm_ssr_state_type;
#endif /* BT_SWDEV_2_1_SSR */

#ifdef BT_SWDEV_2_1_SSP
typedef enum
{
  BT_RM_AS1_NUMERIC_COMPARISON,
  BT_RM_AS1_JUST_WORKS,
  BT_RM_AS1_PASSKEY_ENTRY,
  BT_RM_AS1_OOB
} bt_rm_auth_stage1_type;
#endif /* BT_SWDEV_2_1_SSP */

typedef struct
{
  uint16                        references;
  uint8                         index;
  bt_rm_dev_update_status_type  update_status;
  bt_device_type                dev_public;
  boolean                       inq_result_sent;
  bt_page_scan_mode_type        page_scan_mode;
  bt_page_scan_rep_mode_type    page_scan_rep_mode;
  bt_page_scan_per_mode_type    page_scan_per_mode;
  uint16                        clk_offset;
  boolean                       link_key_valid;
  bt_link_key_type              link_key;
  uint8                         link_key_pin_len_bytes;
  uint8                         num_services;
  bt_service_type               service[ BT_SD_MAX_NUM_OF_SRV_REC ];
#ifdef BT_SWDEV_2_1_EIR
  boolean                       eir_received;
  int8                          inq_resp_rssi;      /* range: -127 to +20 */
  boolean                       eir_tx_power_received;
  int8                          eir_tx_power_level; /* range: -127 to +127 */
  uint8*                        ptr_eir_data;
  uint8                         eir_uuid16_index;
  uint8                         eir_uuid32_index;
  uint8                         eir_uuid128_index;
  uint8                         eir_manuf_data_index;
#endif /* BT_SWDEV_2_1_EIR */
#ifdef BT_SWDEV_2_1_SSP
  boolean                       bonded_link_key;
  boolean                       authenticated_link_key;
  bt_rm_io_cap_type             io_cap;
  boolean                       oob_data_present;
  bt_sp_hash_type               sp_oob_hash_c;
  bt_sp_rand_type               sp_oob_rand_r;
#endif /* BT_SWDEV_2_1_SSP */
} bt_rm_device_type;

LOCAL bt_rm_device_type  bt_rm_device[ BT_RM_MAX_DEVICES ];
LOCAL uint32             bt_rm_dev_db_update_count;

#ifdef BT_SWDEV_2_1_EIR
/* For storing/aggragating the outgoing EIR */
typedef struct
{
  uint8                size_uuid[ BT_NUM_UUID_TYPES ];
  boolean              uuid_list_complete[ BT_NUM_UUID_TYPES ];
  bt_sd_uuid_type      uuid16[ BT_MAX_EIR_UUID16 ];
  bt_sd_uuid32_type    uuid32[ BT_MAX_EIR_UUID32 ];
  bt_sd_uuid128_type   uuid128[ BT_MAX_EIR_UUID128 ];
} bt_rm_uuid_list_type;

typedef struct
{
  /* Note: The local name and manuf data is stored in the EFS and hence in bt_efs_params*/
  boolean               is_name_complete;
  bt_rm_uuid_list_type  uuid_list;
  int8                  tx_power_level;
  boolean               tx_power_level_valid;
  int8                  tx_power_rfcal;
} bt_rm_device_eir_type;

LOCAL bt_rm_device_eir_type     bt_rm_device_eir;
#endif /* BT_SWDEV_2_1_EIR */

#ifdef BT_SWDEV_2_1_SSP
/* Local device OOB security information */
LOCAL bt_sp_hash_type  bt_rm_sp_hash;
LOCAL bt_sp_rand_type  bt_rm_sp_rand;
#ifdef FEATURE_BT_TEST_API_2_1_TESTER
#error code not present
#endif /* FEATURE_BT_TEST_API_2_1_TESTER */
#endif /* BT_SWDEV_2_1_SSP */

/*  Device read iterator state field definitions.  */
#define BT_RM_DRIS_RESET_SIG_M    0xFF000000
#define BT_RM_DRIS_RESET_SIG_V    0x5A000000
#define BT_RM_DRIS_UPDATE_COUNT_M 0x00FFFFFF

typedef struct
{
  bt_app_id_type      bt_app_id;
  bt_service_id_type  service_id;
  bt_security_type    security;
  boolean             authorize;
  boolean             authorize_first;
  uint8               min_pin_len_bytes;
  boolean             comb_link_key_only;
  uint8               min_enc_key_len_bytes;
#ifdef BT_SWDEV_2_1_SSP
  bt_sec_level_type   sec_level;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#endif /* BT_SWDEV_2_1_SSP */
} bt_rm_service_sec_type;

#define BT_RM_MAX_SERV_SECS 20
LOCAL bt_rm_service_sec_type  bt_rm_ss[ BT_RM_MAX_SERV_SECS ];

#ifdef BT_SWDEV_2_1_SSR
#define SNIFF_TO_4_LEGACY_DEV_MS  7000
#endif /* BT_SWDEV_2_1_SSR */

typedef struct
{
  bt_rm_handle_type       rm_handle;
  bt_rm_device_type*      dev_ptr;
  bt_app_id_type          bt_app_id_acl;
  bt_app_id_type          bt_app_id_sco;
  bt_rm_conn_state_type   state_acl;
  bt_rm_conn_state_type   state_sco;
  boolean                 pending_pkt_adjs;
  rex_timer_cnt_type      busy_timeout_ms_left;
  bt_rm_link_update_type  updates;
  bt_role_type            role;
  bt_role_type            role_request;
  uint16                  hc_handle_acl;
  uint16                  hc_handle_sco;
  bt_error_code_type      hc_error;
  uint8                   acl_resps_pending;
  rex_timer_cnt_type      acl_req_timeout_ms_left;
  uint8                   sco_resps_pending;
  rex_timer_cnt_type      sco_req_timeout_ms_left;
  uint8                   pin_req_resps_pending;
  bt_lmp_features_type    remote_lmp_features;
  boolean                 rd_remote_req_pending;
  boolean                 send_pin_req_pending;
  boolean                 remote_name_available;
  rex_timer_cnt_type      pin_req_timeout_ms_left;
  rex_timer_cnt_type      mss_enh_enc_timeout_ms_left;    // enc wait value
  bt_rm_enh_enc_type      enh_enc_state;
  bt_rm_re_pair_pin_type  re_pair_pin_state;
  bt_app_id_type          pin_req_resp_app_id;
  boolean                 pin_req_pairing_done;
  bt_link_mode_type       link_mode;
  bt_link_mode_type       link_mode_request;
  uint16                  max_interval;
  uint16                  cur_interval;
  bt_security_type        security;
  bt_security_type        security_min;
  bt_security_type        security_request;
  uint8                   security_min_pin_len_bytes;
  boolean                 security_comb_key_only;
  uint8                   security_min_enc_key_len_bytes;
  boolean                 authorized;
  boolean                 authorization_required;
  boolean                 authorize_first;
  bt_rm_atzrq_type        authorize_dir;
  bt_app_id_type          authorize_req_app_id;
  uint8                   sec_res_rfcomm_scn;
  uint16                  sec_res_l2cap_psm;
  uint16                  sec_res_l2cap_cid;
  bt_app_id_type          sec_res_enf_app_id;
  bt_app_id_type          sec_res_ss_app_id;
  bt_service_id_type      sec_res_ss_service_id;
  uint8                   pin_code_reply_len_bytes;
  boolean                 pin_code_reply_neg_sent;
  bt_hc_auth_enable_type  hc_auth_enable;
  boolean                 authenticated;
  bt_encrypt_mode_type    encrypt_mode;
  bt_encrypt_enable_type  encrypt_enable;
  uint8                   enc_key_len_bytes;
#ifdef FEATURE_BT_QSOC
  boolean                 vs_ekl_rd_in_progress;
#endif /* FEATURE_BT_QSOC */
  bt_app_id_type          bonding_app_id;
  bt_pin_code_type        bonding_pin;
  boolean                 bonding_key_rxd;
  bt_app_id_type          rname_app_id;
  uint8                   max_slots_allowed_acl;
  uint8                   max_slots_in_use_acl;
  uint16                  pkt_types_allowed_acl;
  uint16                  pkt_types_in_use_acl;
  uint16                  pkt_types_req_acl;
  uint16                  pkt_types_in_use_sco;
  uint16                  max_bytes_acl;
  uint16                  flush_timeout;
  uint16                  link_policy;
  uint16                  idle_timeout;
  rex_timer_cnt_type      idle_timeout_ms_left;
  bt_link_mode_type       idle_link_mode;
  uint16                  idle_max_interval; // not actually used?
  int8                    last_rssi;
  rex_timer_cnt_type      bb_opt_ms_left;
#ifdef FEATURE_BT_1_2
  boolean                 attempt_esco;
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_SSR
  uint16                  max_latency;
  uint16                  cur_latency;
  boolean                 stick_to_app_param;
  boolean                 sniff_initiator_oriented;
  boolean                 sniff_param_changed;
  rex_timer_cnt_type      sniff_retry_to_ms_left;
  boolean                 ssr_emulating;
  bt_rm_ssr_state_type    ssr_state;
  rex_timer_cnt_type      sniff_mode_to_ms_left;
#endif /* BT_SWDEV_2_1_SSR */
#ifdef BT_SWDEV_2_1_SSP
  boolean                 pairing_initiated;
  boolean                 dedicated_bonding;
  bt_rm_auth_req_type     local_auth_req;
  bt_rm_auth_req_type     remote_auth_req;
  boolean                 mitm_protection_reqd;
  bt_rm_auth_stage1_type  auth_stage1_proc;
  bt_rm_service_sec_type* bonding_ss_ptr;
  boolean                 rebond_req_pending;
  boolean                 authorized_rebond;
  uint8                   re_pair_attempts;
  bt_sec_level_type       sec_level;
  bt_sec_level_type       sec_level_min;
  bt_sec_level_type       sec_level_request;
  boolean                 update_link_key_request;
  boolean                 new_link_key;
  bt_lmp_features_type    remote_lmp_features_ext;
  boolean                 user_reply_neg;
#endif /* BT_SWDEV_2_1_SSP */
#ifdef BT_SWDEV_2_1_EPR
  uint32                  encryption_key_age;
  boolean                 enc_key_refresh_req_pending;
#endif /* BT_SWDEV_2_1_EPR */
#ifdef FEATURE_BT_SOC
  rex_timer_cnt_type      rssi_update_ms_left;
#endif /* FEATURE_BT_SOC */
  bt_sync_conn_param_type sync_conn_params;
  boolean                 bond_canceling;
  boolean                 ev_hc_conn_acl_rxd;
  boolean                 no_disc_on_del_link_key;
  rex_timer_cnt_type      bb_page_to_ms_left;
  boolean                 paging;
} bt_rm_conn_type;

LOCAL bt_rm_conn_type bt_rm_conn[ BT_MAX_CONNECTIONS ];

typedef struct
{
  bt_app_id_type           bt_app_id;
  boolean                  acl_registered;
  boolean                  sco_registered;
  boolean                  bondable;
  boolean                  radio_activity_registered;
  bt_rm_discoverable_type  discoverable;
  bt_rm_act_vs_pwr_type    disc_act_vs_pwr;
  boolean                  connectable;
  bt_rm_act_vs_pwr_type    conn_act_vs_pwr;
  boolean                  deliver_dev_updates;
  boolean                  auto_service_search_disabled;
} bt_rm_app_type;

#define BT_RM_MAX_APPS 10
LOCAL bt_rm_app_type  bt_rm_app[ BT_RM_MAX_APPS ];


typedef struct
{
  bt_app_id_type          bt_app_id;
  bt_rm_conn_type*        conn_ptr;
  bt_low_power_ctrl_type  low_power_ctrl;
  uint16                  max_interval;
  uint16                  idle_timeout;
#ifdef BT_SWDEV_2_1_SSR
  uint16                  max_latency;
  boolean                 stick_to_app_param;
  boolean                 sniff_initiator_oriented;
#endif /* BT_SWDEV_2_1_SSR */
} bt_rm_low_power_cfg_type;

#define BT_RM_MAX_LPCS 10
LOCAL bt_rm_low_power_cfg_type  bt_rm_lpc[ BT_RM_MAX_LPCS ];


typedef struct
{
  bt_app_id_type    bt_app_id;
  bt_rm_conn_type*  conn_ptr;
  boolean           security_changes;
  boolean           mode_changes;
  boolean           role_switches;
  boolean           new_acl_pkts;
  boolean           new_sco_pkts;
  uint8             rssi_delta;
  int8              rssi_last;
  boolean           sync_param_updates;
} bt_rm_link_status_reg_type;

#define BT_RM_MAX_LSRS 10
LOCAL bt_rm_link_status_reg_type  bt_rm_lsr[ BT_RM_MAX_LSRS ];


typedef struct
{
  uint8            rfcomm_scn;
  bt_sd_uuid_type  sdp_uuid;
  boolean          bd_addr_valid;
  bt_bd_addr_type  bd_addr;
} bt_rm_uuid_scn_pair_type;

#define BT_RM_MAX_UUID_SCN_PAIRS 15
LOCAL bt_rm_uuid_scn_pair_type  bt_rm_uuid_scn[ BT_RM_MAX_UUID_SCN_PAIRS ];


typedef struct
{
  bt_app_id_type       inq_bt_app_id;
  uint16               inq_max_resps;
  uint16               inq_responses;
  bt_security_type     security;
  uint8                min_enc_key_len_bytes;
  boolean              test_mode_enabled;
  boolean              telec_started;
  boolean              telec_transmit;
  boolean              telec_hopping;
  bt_role_type         accept_conn_role;
  bt_role_switch_type  create_conn_role_sw_req_ok;
#ifdef BT_SWDEV_RADIO_OFF
  bt_app_id_type       disable_radio_app_id;
  boolean              disable_radio;
  bt_app_id_type       disable_discoverability_app_id;
  boolean              disable_discoverability;
  bt_app_id_type       dev_black_list_app_id;
  uint16               dev_black_list_count;
  bt_bd_addr_type      dev_black_list[ BT_RM_MAX_DEV_BLACK_LIST_COUNT ];
  bt_app_id_type       dev_white_list_app_id;
  uint16               dev_white_list_count;
  bt_bd_addr_type      dev_white_list[ BT_RM_MAX_DEV_WHITE_LIST_COUNT ];
#endif
  bt_radio_activity_type  current_radio_activity;
#ifdef FEATURE_BT_SOC
  uint16               conf_page_scan_interval;
  uint16               conf_page_scan_window;
  uint16               conf_inq_scan_interval;
  uint16               conf_inq_scan_window;
#ifdef FEATURE_BT_1_2
  bt_inq_scan_mode_type  conf_inq_scan_type;
  bt_page_scan_mode_type conf_page_scan_type;
#endif /* FEATURE_BT_1_2 */
#endif /* FEATURE_BT_SOC */
#ifdef BT_SWDEV_2_1_SSP
  bt_sec_level_type    sec_level;
  boolean              sm4_debug_enable;
  boolean              oob_valid;
#endif /* BT_SWDEV_2_1_SSP */
  uint32               bt_module_st_mask;
  uint16               page_timeout;
} bt_rm_data_type;

LOCAL bt_rm_data_type  bt_rm_data;

#define BT_HCI_REASON( hc_rsn )  \
          ((bt_event_reason_type)(BT_EVR_HCI_START + hc_rsn))


#ifdef FEATURE_EFS
/*-------------------------------------------------------------------------*/
/*  Declarations for Device Data Base and Service Data Base files.         */
/*-------------------------------------------------------------------------*/

#define BT_RM_DEV_DB_NAME    "BT_RM_DEV_DB"
#define BT_RM_DEV_DB_VERSION 9

typedef struct
{
  uint8                       index;
  bt_device_type              dev_public;
  bt_page_scan_mode_type      page_scan_mode;
  bt_page_scan_rep_mode_type  page_scan_rep_mode;
  bt_page_scan_per_mode_type  page_scan_per_mode;
  uint16                      clk_offset;
  boolean                     link_key_valid;
  bt_link_key_type            link_key;
  uint8                       link_key_pin_len_bytes;
  uint8                       num_services;
#ifdef BT_SWDEV_2_1_SSP
  boolean                     bonded_link_key;
  boolean                     authenticated_link_key;
#endif /* BT_SWDEV_2_1_SSP */
#ifdef BT_SWDEV_2_1_EIR
  boolean                     eir_received;
#endif /* BT_SWDEV_2_1_EIR */
} bt_rm_device_efs_type;

/*  Structure below is to document the database layout.  */
/*  It is not used in code explicitly to save memory.    */
typedef struct
{
  uint8                  database_version;
  bt_security_type       security;
  bt_rm_device_efs_type  device[ BT_RM_MAX_DEVICES ];
#ifdef BT_SWDEV_2_1_SSP
  bt_sec_level_type      sec_level;
#endif /* BT_SWDEV_2_1_SSP */
} bt_rm_device_file_type;

/*  Service file names have 1 character added for index value.  */
#define BT_RM_SERV_DB_NAME_LEN 16
LOCAL char  service_file_name[ BT_RM_SERV_DB_NAME_LEN ] = "BT_RM_SERV_DBxx";

#define BT_RM_SERV_DB_VERSION 5
typedef struct
{
  uint8            database_version;
  uint8            index;
  bt_bd_addr_type  bd_addr;
  uint8            num_services;
  bt_service_type  service[ BT_SD_MAX_NUM_OF_SRV_REC ];
} bt_rm_service_efs_type;

#endif /* FEATURE_EFS */

/*  Extended Service Security test only.              */
/*  Allows special cases to be tested with emulator.  */
#undef BT_SWDEV_ESS_TEST

#ifdef BT_SWDEV_ESS_TEST
uint8                bt_rm_ess_test_klb = 0;
bt_hc_link_key_type  bt_rm_ess_test_lkt = BT_HC_LINK_KEY_COMB;
#endif /* BT_SWDEV_ESS_TEST */

/* Forward references. */
LOCAL void bt_rm_check_scan_settings( void );
LOCAL void bt_rm_link_status_updated( bt_rm_conn_type* conn_ptr );

#ifdef UPF
#define BT_MAX_EK_AGE_LO_UPF 60*2    /* 60 seconds */
boolean bt_shorten_epr_timer = FALSE;
#endif
/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_rm_find_conn_bd_addr

DESCRIPTION
  Finds a connection table entry with a specified BD address.

RETURN VALUE
  Pointer to connection table entry, if found; otherwise NULL.

===========================================================================*/
LOCAL bt_rm_conn_type* bt_rm_find_conn_bd_addr
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  uint16            i;
  bt_rm_conn_type*  conn_ptr = NULL;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
         (BT_BD_ADDRS_EQUAL( &bt_rm_conn[ i ].dev_ptr->dev_public.bd_addr,
                             bd_addr_ptr ) != FALSE) )
    {
      conn_ptr = &bt_rm_conn[ i ];
      break;
    }
  }

  return ( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_disconnect_acl

DESCRIPTION
  Disconnects a specified ACL connection.

===========================================================================*/
LOCAL void bt_rm_disconnect_acl
(
  bt_rm_conn_type*        conn_ptr,
  bt_rm_disc_reason_type  reason
)
{

  conn_ptr->state_acl = BT_RM_CS_DISCONNECTING;

  BT_MSG_API( "BT RM CMD TX: HC DiscA H %x R %x",
              conn_ptr->hc_handle_acl, reason, 0 );
  bt_cmd_hc_disconnect( conn_ptr->hc_handle_acl,
                        (uint8)(reason) );

}


/*===========================================================================

FUNCTION
  bt_rm_init_inquiry_data

DESCRIPTION
  Initializes data associated with inquiries.

===========================================================================*/
LOCAL void bt_rm_init_inquiry_data
(
  void
)
{

  uint16  i;

  bt_rm_data.inq_bt_app_id = BT_APP_ID_NULL;
  bt_rm_data.inq_max_resps = 0;
  bt_rm_data.inq_responses = 0;

  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    bt_rm_device[ i ].inq_result_sent = FALSE;
#ifdef BT_SWDEV_2_1_EIR
    bt_rm_device[ i ].eir_tx_power_received = FALSE;
#endif  /* BT_SWDEV_2_1_EIR */
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_application

DESCRIPTION
  Initializes a specified application table entry.

===========================================================================*/
LOCAL void bt_rm_init_application
(
  bt_rm_app_type*  app_ptr
)
{

  app_ptr->bt_app_id                    = BT_APP_ID_NULL;
  app_ptr->acl_registered               = FALSE;
  app_ptr->sco_registered               = FALSE;
  app_ptr->bondable                     = FALSE;
  app_ptr->discoverable                 = BT_RM_DISCOVERABLE_NOT;
  app_ptr->disc_act_vs_pwr              = BT_RM_AVP_AUTOMATIC;
  app_ptr->connectable                  = FALSE;
  app_ptr->conn_act_vs_pwr              = BT_RM_AVP_AUTOMATIC;
  app_ptr->deliver_dev_updates          = FALSE;
  app_ptr->auto_service_search_disabled = FALSE;

}


/*===========================================================================

FUNCTION
  bt_rm_init_unused_application

DESCRIPTION
  Initializes a specified application if it is currently unused.

===========================================================================*/
LOCAL void bt_rm_init_unused_application
(
  bt_rm_app_type*  app_ptr
)
{

  if ( (app_ptr->connectable                  == FALSE) &&
       (app_ptr->discoverable                 == FALSE) &&
       (app_ptr->acl_registered               == FALSE) &&
       (app_ptr->sco_registered               == FALSE) &&
       (app_ptr->bondable                     == FALSE) &&
       (app_ptr->deliver_dev_updates          == FALSE) &&
       (app_ptr->radio_activity_registered    == FALSE) &&
       (app_ptr->auto_service_search_disabled == FALSE) )
  {
    bt_rm_init_application( app_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_rm_init_low_power_config

DESCRIPTION
  Initializes a specified low power configuration table entry.

===========================================================================*/
LOCAL void bt_rm_init_low_power_config
(
  bt_rm_low_power_cfg_type*  lpc_ptr
)
{

  lpc_ptr->bt_app_id      = BT_APP_ID_NULL;
  lpc_ptr->conn_ptr       = NULL;
  lpc_ptr->low_power_ctrl = BT_LPC_DONT_CARE;
  lpc_ptr->max_interval   = 0;
  lpc_ptr->idle_timeout   = 0;

#ifdef BT_SWDEV_2_1_SSR
  lpc_ptr->max_latency              = BT_RM_SSR_NOT_ALLOWED;
  lpc_ptr->stick_to_app_param       = FALSE;
  lpc_ptr->sniff_initiator_oriented = TRUE;
#endif /* BT_SWDEV_2_1_SSR */

}


/*===========================================================================

FUNCTION
  bt_rm_init_link_status_reg

DESCRIPTION
  Initializes a specified link status registration table entry.

===========================================================================*/
LOCAL void bt_rm_init_link_status_reg
(
  bt_rm_link_status_reg_type*  lsr_ptr
)
{

  lsr_ptr->bt_app_id        = BT_APP_ID_NULL;
  lsr_ptr->conn_ptr         = NULL;
  lsr_ptr->security_changes = FALSE;
  lsr_ptr->mode_changes     = FALSE;
  lsr_ptr->role_switches    = FALSE;
  lsr_ptr->new_acl_pkts     = FALSE;
  lsr_ptr->new_sco_pkts     = FALSE;
  lsr_ptr->rssi_delta       = 0;
  lsr_ptr->rssi_last        = BT_RM_INVALID_RSSI;
  lsr_ptr->sync_param_updates = FALSE;

}


/*===========================================================================

FUNCTION
  bt_rm_init_uuid_scn_pair

DESCRIPTION
  Initializes a specified UUID SCN pair table entry.

===========================================================================*/
LOCAL void bt_rm_init_uuid_scn_pair
(
  bt_rm_uuid_scn_pair_type*  usp_ptr
)
{

  usp_ptr->rfcomm_scn    = BT_RC_SCN_NOT_SPECIFIED;
  usp_ptr->sdp_uuid      = BT_SD_INVALID_UUID;
  usp_ptr->bd_addr_valid = FALSE;

  memset( (void*)(usp_ptr->bd_addr.bd_addr_bytes), 0,
          sizeof( bt_bd_addr_type) );

}


/*===========================================================================

FUNCTION
  bt_rm_init_service_id

DESCRIPTION
  Initializes a specified service ID.

===========================================================================*/
LOCAL void bt_rm_init_service_id
(
  bt_service_id_type*  srv_id_ptr
)
{

  srv_id_ptr->service_id_method = BT_SIM_NONE;
  srv_id_ptr->sdp_uuid          = BT_SD_INVALID_UUID;
  srv_id_ptr->rfcomm_scn        = BT_RC_SCN_NOT_SPECIFIED;
  srv_id_ptr->l2cap_psm         = BT_L2_PSM_INVALID;

}


/*===========================================================================

FUNCTION
  bt_rm_init_service_security

DESCRIPTION
  Initializes a specified service security table entry.

===========================================================================*/
LOCAL void bt_rm_init_service_security
(
  bt_rm_service_sec_type*  ss_ptr
)
{

  ss_ptr->bt_app_id             = BT_APP_ID_NULL;
  ss_ptr->authorize             = FALSE;
  ss_ptr->authorize_first       = FALSE;
  ss_ptr->min_pin_len_bytes     = 0;
  ss_ptr->comb_link_key_only    = FALSE;
  ss_ptr->min_enc_key_len_bytes = 0;
#ifdef BT_SWDEV_2_1_SSP
  ss_ptr->sec_level             = BT_RM_SL_0_NONE;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
#else
  ss_ptr->security              = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */
  bt_rm_init_service_id( &ss_ptr->service_id );

}


/*===========================================================================

FUNCTION
  bt_rm_init_service

DESCRIPTION
  Initializes a specified service record.

===========================================================================*/
LOCAL void bt_rm_init_service
(
  bt_service_type*  serv_ptr
)
{

  serv_ptr->service_class         = BT_SERVICE_CLASS_UNKNOWN;
  serv_ptr->profile_version       = 0;
  serv_ptr->server_channel_number = BT_SPP_SCN_UNSPECIFIED;

}


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_reset_eir_data

DESCRIPTION
  Reset any EIR data received from the device

===========================================================================*/
void bt_rm_init_eir_data
(
  bt_rm_device_type*    dev_ptr
)
{
  uint8 i;

  /* Reset the flags */
  dev_ptr->eir_received                         = 0;

  dev_ptr->inq_resp_rssi                        = 0;
  dev_ptr->eir_tx_power_received                = FALSE;
  dev_ptr->eir_tx_power_level                   = 0;

  /* Free the memory */
  if( dev_ptr->ptr_eir_data != NULL )
  {
    mem_free( &bt_rm_eir_heap, dev_ptr->ptr_eir_data );
    dev_ptr->ptr_eir_data = NULL;
  }

  /* Reset the indices */
  dev_ptr->eir_manuf_data_index                 = BT_MAX_EIR_HCI_LEN;
  dev_ptr->eir_uuid128_index                    = BT_MAX_EIR_HCI_LEN;
  dev_ptr->eir_uuid16_index                     = BT_MAX_EIR_HCI_LEN;
  dev_ptr->eir_uuid32_index                     = BT_MAX_EIR_HCI_LEN;

  /* Check if we have received the full name of the device during the current inquiry */
  if( dev_ptr->inq_result_sent == FALSE ||
      !ISBITSET( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B ) )
  {
    /* We have not received the full name from the device during
       the current inquiry process  - we should reset eir_flag */
    dev_ptr->dev_public.eir_data.eir_flag         = 0;
  }

  /* Reset other eir data */
  dev_ptr->dev_public.eir_data.device_ranking   = BT_MAX_PATHLOSS;
  dev_ptr->dev_public.eir_data.manuf_data_size  = 0;
  for(i = 0; i < BT_NUM_UUID_TYPES; i++)
  {
    dev_ptr->dev_public.eir_data.num_uuids[ i ] = 0;
  }
}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_rm_init_device_security

DESCRIPTION
  Initializes security related information for a specified device.

===========================================================================*/
LOCAL void bt_rm_init_device_security
(
  bt_rm_device_type*  dev_ptr
)
{

#ifdef BT_SWDEV_2_1_SSP
  dev_ptr->dev_public.sec_level     = BT_RM_SL_0_NONE;
#else
  dev_ptr->dev_public.security      = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */
  dev_ptr->dev_public.bonded        = FALSE;
  dev_ptr->dev_public.link_key_type = BT_HC_LINK_KEY_NONE;
  dev_ptr->link_key_valid           = FALSE;
  dev_ptr->link_key_pin_len_bytes   = 0;

  memset( (void*)(&dev_ptr->link_key), 0,
          sizeof( bt_link_key_type ) );

#ifdef BT_SWDEV_2_1_SSP
  dev_ptr->dev_public.link_key_status = BT_RM_LKT_NONE;
  dev_ptr->bonded_link_key = FALSE;
  dev_ptr->authenticated_link_key = FALSE;
#endif /* BT_SWDEV_2_1_SSP */

}

/*===========================================================================

FUNCTION
  bt_rm_disconnect_device

DESCRIPTION
  If an ACL exists for the device, ques cmd to disconnect the ACL.

===========================================================================*/
LOCAL void bt_rm_disconnect_device
(
  bt_rm_device_type*  dev_ptr
)
{
  bt_rm_conn_type*  conn_ptr;

  if ( (conn_ptr = bt_rm_find_conn_bd_addr(
                     &dev_ptr->dev_public.bd_addr)) != NULL )
  {
    if ( ( conn_ptr->state_acl != BT_RM_CS_DISCONNECTING ) &&
         ( conn_ptr->no_disc_on_del_link_key == FALSE )
       )
    {
      bt_cmd_rm_disconnect_acl( conn_ptr->bt_app_id_acl,
                                conn_ptr->rm_handle,
                                BT_RMDR_USER_ENDED );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_device

DESCRIPTION
  Initializes a specified device table entry.

===========================================================================*/
LOCAL void bt_rm_init_device
(
  bt_rm_device_type*  dev_ptr
)
{

  uint8  i;

  dev_ptr->references                           = 0;
  dev_ptr->update_status                        = BT_RM_DUS_NONE;
  dev_ptr->dev_public.valid                     = FALSE;
  dev_ptr->dev_public.version.lmp_version       = 0;
  dev_ptr->dev_public.version.manufacturer_name = 0;
  dev_ptr->dev_public.version.lmp_subversion    = 0;
  dev_ptr->dev_public.value_1                   = 0;
  dev_ptr->dev_public.value_2                   = 0;
  dev_ptr->dev_public.value_3                   = 0;
  dev_ptr->dev_public.value_4                   = 0;
  dev_ptr->inq_result_sent                      = FALSE;
#ifdef FEATURE_BT_1_2
  dev_ptr->page_scan_mode                       = BT_PS_STANDARD;
#else
  dev_ptr->page_scan_mode                       = BT_PS_MANDATORY;
#endif
  dev_ptr->page_scan_rep_mode                   = BT_PSR_R1;
  dev_ptr->page_scan_per_mode                   = BT_PSP_P0;
  dev_ptr->clk_offset                           = 0;

#ifdef BT_SWDEV_2_1_EIR
  bt_rm_init_eir_data( dev_ptr );
#endif /* BT_SWDEV_2_1_EIR */
#ifdef BT_SWDEV_2_1_SSP
  dev_ptr->dev_public.ssp_capable = FALSE;
  dev_ptr->io_cap = BT_RM_IOC_NO_IO;
  dev_ptr->oob_data_present = FALSE;
#endif /* BT_SWDEV_2_1_SSP */

  dev_ptr->dev_public.service_class =
    BT_SERVICE_CLASS_UNKNOWN;
  dev_ptr->dev_public.major_dev_class =
    BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
  dev_ptr->dev_public.minor_dev_class =
    BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;

  memset( (void*)(dev_ptr->dev_public.bd_addr.bd_addr_bytes), 0,
          sizeof( bt_bd_addr_type) );

  memset( (void*)(&dev_ptr->dev_public.class_of_device), 0,
          sizeof( bt_cod_type) );

  memset( (void*)(&dev_ptr->dev_public.lmp_features), 0,
          sizeof( bt_lmp_features_type) );

  memset( (void*)dev_ptr->dev_public.name_str, 0,
          (BT_SD_MAX_DEVICE_NAME_LEN + 1) );

  memset( (void*)dev_ptr->dev_public.nick_name_str, 0,
          (BT_MAX_REMOTE_NICK_NAME_LEN + 1) );

  bt_rm_init_device_security( dev_ptr );

  /*  Initialize service information for device.  */
  dev_ptr->num_services = 0;

  for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
  {
    bt_rm_init_service( &dev_ptr->service[ i ] );
  }

#ifdef BT_SWDEV_2_1_SSP
  memset( (void*)(&dev_ptr->dev_public.lmp_features_ext), 0,
          sizeof( bt_lmp_features_type) );
#endif /* BT_SWDEV_2_1_SSP */
}


/*===========================================================================

FUNCTION
  bt_rm_init_devices

DESCRIPTION
  Initializes the entire device table.

===========================================================================*/
LOCAL void bt_rm_init_devices
(
  void
)
{

  uint8  i;

  bt_rm_dev_db_update_count = 0;

  /*  Initialize the RM device data.  */
  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    bt_rm_device[ i ].index = i;

    bt_rm_init_device( &bt_rm_device[ i ] );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_conn_idle_link_mode

DESCRIPTION
  Initializes the idle link mode portion of a specified connection.

===========================================================================*/
LOCAL void bt_rm_init_conn_idle_link_mode
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->idle_timeout         = BT_RM_DEF_IDLE_TIMEOUT_MS;
  conn_ptr->idle_timeout_ms_left = 0;
  conn_ptr->idle_link_mode       = BT_RM_DEF_IDLE_MODE;
  conn_ptr->idle_max_interval    = BT_RM_DEF_MAX_INTERVAL;

}


#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_rm_init_conn_4_manual_active

DESCRIPTION
  Initializes the lpc portion of a specified connection for manually set
  ACTIVE mode.

===========================================================================*/
LOCAL void bt_rm_init_conn_4_manual_active
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->idle_timeout           = 0;
  conn_ptr->idle_timeout_ms_left   = 0;
  conn_ptr->sniff_retry_to_ms_left = 0;
  conn_ptr->sniff_mode_to_ms_left  = 0;
  conn_ptr->ssr_emulating          = FALSE;
  conn_ptr->sniff_param_changed    = FALSE;
  conn_ptr->ssr_state              = BT_RM_SSR_NOT_IN_PROGRESS;
  conn_ptr->idle_link_mode         = BT_LM_ACTIVE;
  /* Update link policy to disable remote initiated sniff */
  conn_ptr->link_policy &= ~BT_LINK_POLICY_SNIFF_MODE_B;

}
#endif /* BT_SWDEV_2_1_SSR */


/*===========================================================================

FUNCTION
  bt_rm_total_active_sco_links

DESCRIPTION
  Returns the current number of active SCO links.

RETURN VALUE
  Total number of currently active SCO links.

===========================================================================*/
LOCAL uint16 bt_rm_total_active_sco_links
(
  void
)
{

  uint16  i;
  uint16  sco_links = 0;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
         (bt_rm_conn[ i ].state_sco != BT_RM_CS_DISCONNECTED) )
    {
      sco_links++;
    }
  }

  return ( sco_links );

}

/*===========================================================================

FUNCTION
  bt_rm_select_sco_packet_type

DESCRIPTION
  Returns the current number of active SCO links.

RETURN VALUE
  Total number of currently active SCO links.

===========================================================================*/
LOCAL uint16 bt_rm_select_sco_packet_type
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16  sync_pkt_types = 0;

#ifdef FEATURE_BT_1_2
        sync_pkt_types = BT_SYNC_DEF_PKT_TYPES;

#ifdef BT_SWDEV_ESCO
        if ( conn_ptr->attempt_esco == TRUE )
        {
             /* Check for EV3 */
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[3] &
                                               BT_LM_FEATURE_BYTE3_EV3_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_EV3_B;
            }

            /* Check for EV4 */
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[4] &
                                               BT_LM_FEATURE_BYTE4_EV4_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_EV4_B;
            }

            /* Check for EV5 */
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[4] &
                                               BT_LM_FEATURE_BYTE4_EV5_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_EV5_B;
            }

            /* Check for 2-EV3 */
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_2MBPS_ESCO_B )
            {
                sync_pkt_types &=  ( ~BT_SYNC_PT_2EV3_B );
            }

            /* Check for 2-EV5 */
            if( ( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_2MBPS_ESCO_B ) &&
                ( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_3SLOT_ESCO_B ) )
            {
                sync_pkt_types &=  ( ~BT_SYNC_PT_2EV5_B );
            }

            /* Check for 3-EV3 */
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_3MBPS_ESCO_B )
            {
                sync_pkt_types &=  ( ~BT_SYNC_PT_3EV3_B );
            }

            /* Check for 3-EV5 */
            if( ( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_3MBPS_ESCO_B ) &&
                ( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                                               BT_LM_FEATURE_BYTE5_EDR_3SLOT_ESCO_B ) )
            {
                sync_pkt_types &=  ( ~BT_SYNC_PT_3EV5_B );
            }
        }
#endif /* BT_SWDEV_ESCO */
        {
            if( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                   BT_LM_FEATURE_BYTE1_HV3_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_HV3_B;
            }

            if ( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                   BT_LM_FEATURE_BYTE1_HV2_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_HV2_B;
            }

            if ( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                   BT_LM_FEATURE_BYTE1_HV1_B )
            {
                sync_pkt_types |=  BT_SYNC_PT_HV1_B;
            }
        }
#else
        if( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                    BT_LM_FEATURE_BYTE1_HV3_B )
        {
            sync_pkt_types | = BT_PT_HV3_B;
        }

        if ( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                    BT_LM_FEATURE_BYTE1_HV2_B )
        {
            sync_pkt_types | = BT_PT_HV2_B;
        }

        if ( conn_ptr->remote_lmp_features.lmp_features_bytes[1] &
                                                    BT_LM_FEATURE_BYTE1_HV1_B )
        {
            sync_pkt_types | = BT_PT_HV1_B;
        }
#endif /* FEATURE_BT_1_2 */


  return ( sync_pkt_types );

}


/*===========================================================================

FUNCTION
  bt_rm_total_active_acl_links

DESCRIPTION
  Returns the current number of active acl links.

RETURN VALUE
  Total number of currently active acl links.

===========================================================================*/
LOCAL uint16 bt_rm_total_active_acl_links
(
  void
)
{

  uint16  i;
  uint16  acl_links = 0;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED)
    {
      acl_links++;
    }
  }

  return ( acl_links );

}

/*===========================================================================

FUNCTION
  bt_rm_check_for_slave_acl_link

DESCRIPTION
  Checks the connection table for slave links. Validates connection table
  for presence of scatternet and outputs error message.

RETURN VALUE
  FALSE if there is no slave ACL link, TRUE otherwise.

===========================================================================*/
LOCAL boolean bt_rm_check_for_slave_acl_link
(
  void
)
{

  uint16  i;
  uint16  slave_acls = 0;
  uint16  master_acls = 0;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED)
    {
      if (bt_rm_conn[ i ].role == BT_ROLE_SLAVE)
      {
        slave_acls++;
      }
      else
      {
        master_acls++;
      }
    }
  }

  BT_MSG_DEBUG( "BT RM: ACL roles S %x M %x", slave_acls, master_acls, 0 );

#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
  if ( (master_acls != 0) && (slave_acls != 0) )
  {
    BT_ERR( "BT RM: Mastery with Slavery", 0, 0, 0 );
  }

  if (slave_acls > 1)
  {
    BT_ERR( "BT RM: More than 1 ACL in Slave role", 0, 0, 0 );
  }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

  return ( slave_acls != 0 );

}

/*===========================================================================

FUNCTION
  bt_rm_get_acl_params_allowed

DESCRIPTION
  Determines the maximum slots and the ACL packet types allowed
  currently.

RETURN VALUE
  *max_slots_ptr  Maximum slots per ACL packet currently allowed.
  *pkt_types_ptr  Bit mask of BT_PT_*_B values for currently
                    allowed ACL packet types.

===========================================================================*/
LOCAL void bt_rm_get_acl_params_allowed
(
  uint8*   max_slots_ptr,
  uint16*  pkt_types_ptr
)
{

/*  BT_TBD:  This is only for testing.  */
static boolean enable_1_slot_only = FALSE;

  if ( (bt_rm_total_active_sco_links() > 0) ||
       (enable_1_slot_only != FALSE) )
  {
    *max_slots_ptr = 1;
    *pkt_types_ptr = BT_PT_ACL_MAX_SLOT_1_M;
  }
  else
  {
    *max_slots_ptr = 5;
    *pkt_types_ptr = BT_PT_ACL_MAX_SLOT_5_M;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_change_packet_types_acl

DESCRIPTION
  Changes ACL packet types in use on a specified connection to the
  ones specified.

===========================================================================*/
LOCAL void bt_rm_change_packet_types_acl
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->pkt_types_req_acl == 0 )
  {
    conn_ptr->pkt_types_req_acl = conn_ptr->pkt_types_allowed_acl;

    BT_MSG_HIGH( "BT RM: Change ACL Pkt Cur %x Prev %x H %x",
                conn_ptr->pkt_types_allowed_acl,
                conn_ptr->pkt_types_in_use_acl,
                conn_ptr->hc_handle_acl );
    BT_MSG_API( "BT RM CMD TX: HC Chg PT ACL H %x PT %x",
                conn_ptr->hc_handle_acl,
                conn_ptr->pkt_types_req_acl, 0 );
    bt_cmd_hc_change_pkt_type( conn_ptr->hc_handle_acl,
                               conn_ptr->pkt_types_req_acl );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Skip ACL PT Chg H/LM %04x PTR %x NPT %x",
                  ((conn_ptr->rm_handle << 8) | conn_ptr->link_mode),
                  conn_ptr->pkt_types_req_acl,
                  conn_ptr->pkt_types_allowed_acl );
  }

  conn_ptr->bb_opt_ms_left = BT_RM_BB_OPT_TIMEOUT_MS;

}


/*===========================================================================

FUNCTION
  bt_rm_check_conn_packet_types_acl

DESCRIPTION
  Takes appropriate action if the ACL packet types in use on a
  specified connection must be changed.

===========================================================================*/
LOCAL void bt_rm_check_conn_packet_types_acl
(
  bt_rm_conn_type*  conn_ptr
)
{

/*  BT_TBD:  This is only for testing.  */
static boolean enable_auto_changes = FALSE;

  if ( enable_auto_changes == FALSE )
  {
    /*  BT_TBD:  This is the normal code and all   */
    /*           that is actually needed in here.  */
    if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTED) &&
         (conn_ptr->pkt_types_allowed_acl !=
          conn_ptr->pkt_types_in_use_acl)            &&
         (conn_ptr->link_mode == BT_LM_ACTIVE)
       )
    {
      /*  New set of packet types to use on ACL link.  */
      bt_rm_change_packet_types_acl( conn_ptr );
    }
    else
    {
      conn_ptr->bb_opt_ms_left = 0;
    }
  }
  else
  {
    /*  BT_TBD:  This is only for testing.  */
    /*           Causes 1 to 5 slot packet  */
    /*           changes every 5 seconds.   */
    if ( conn_ptr->state_acl == BT_RM_CS_CONNECTED )
    {
      if ( conn_ptr->pkt_types_allowed_acl &
            (BT_PT_3_SLOT_M | BT_PT_5_SLOT_M) )
      {
        conn_ptr->max_slots_allowed_acl = 1;
        conn_ptr->pkt_types_allowed_acl = BT_PT_ACL_MAX_SLOT_1_M;
      }
      else
      {
        conn_ptr->max_slots_allowed_acl = 5;
        conn_ptr->pkt_types_allowed_acl = BT_PT_ACL_MAX_SLOT_5_M;
      }

      /*  New set of packet types to use on ACL link.  */
      bt_rm_change_packet_types_acl( conn_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_check_all_packet_types_acl

DESCRIPTION
  Takes appropriate action if ACL packet types in use must be changed
  on any current connection.

RETURN VALUE
  TRUE   If any ACL packet type changes were requested
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_check_all_packet_types_acl
(
  void
)
{

  boolean           pkt_change_reqs = FALSE;
#ifndef FEATURE_BT_SOC
  uint16            i;
  uint8             max_slots_allowed_acl;
  uint16            pkt_types_allowed_acl;
  bt_rm_conn_type*  conn_ptr;

  bt_rm_get_acl_params_allowed( &max_slots_allowed_acl,
                                &pkt_types_allowed_acl );

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    conn_ptr = &bt_rm_conn[ i ];

    if ( (conn_ptr->state_acl != BT_RM_CS_DISCONNECTED) &&
         (conn_ptr->max_slots_allowed_acl != max_slots_allowed_acl) )
    {
      conn_ptr->max_slots_allowed_acl = max_slots_allowed_acl;
      conn_ptr->pkt_types_allowed_acl = pkt_types_allowed_acl;

      /*  New set of packet types to use on ACL link.  */
      bt_rm_change_packet_types_acl( conn_ptr );

      pkt_change_reqs = TRUE;
    }
  }
#endif
  return ( pkt_change_reqs );

}


/*===========================================================================

FUNCTION
  bt_rm_init_connection_sco

DESCRIPTION
  Initializes the SCO portion of a specified connection.

===========================================================================*/
LOCAL void bt_rm_init_connection_sco
(
  bt_rm_conn_type*  conn_ptr
)
{
  boolean  send_link_status = FALSE;

  if ( conn_ptr->state_sco != BT_RM_CS_DISCONNECTED )
  {
    /* Should call bt_rm_link_status_updated() only after init logic below
    */
    send_link_status = TRUE;
  }
  conn_ptr->bt_app_id_sco            = BT_APP_ID_NULL;
  conn_ptr->state_sco                = BT_RM_CS_DISCONNECTED;
  conn_ptr->pending_pkt_adjs         = FALSE;
  conn_ptr->hc_handle_sco            = BT_HC_NULL_CONN_HANDLE;
  conn_ptr->sco_resps_pending        = 0;
  conn_ptr->sco_req_timeout_ms_left  = 0;
  conn_ptr->pkt_types_in_use_sco     = 0;

  memset( (void*)(&conn_ptr->sync_conn_params), 0,
          sizeof( bt_sync_conn_param_type ) );

#ifdef FEATURE_BT_1_2
  /* This value controls whether eSCO will be attempted when sync connnections
     is initiated by the local device. */
  conn_ptr->attempt_esco             = FALSE;
#endif /* FEATURE_BT_1_2 */

  if ( send_link_status == TRUE )
  {
    conn_ptr->updates  |= BT_RM_LU_SCO_PRESENCE_B;
    bt_rm_link_status_updated( conn_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_rm_init_conn_sec_results

DESCRIPTION
  Initializes the security results portion of a specified connection.

===========================================================================*/
LOCAL void bt_rm_init_conn_sec_results
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->sec_res_rfcomm_scn = BT_RC_SCN_NOT_SPECIFIED;
  conn_ptr->sec_res_l2cap_psm  = BT_L2_PSM_INVALID;
  conn_ptr->sec_res_l2cap_cid  = BT_L2_NULL_CID;
  conn_ptr->sec_res_enf_app_id = BT_APP_ID_NULL;
  conn_ptr->sec_res_ss_app_id  = BT_APP_ID_NULL;
  conn_ptr->authorize_dir      = BT_RM_ATZRQ_INVALID;
#ifdef BT_SWDEV_2_1_SSP
  conn_ptr->bonding_ss_ptr     = NULL;
#endif /* BT_SWDEV_2_1_SSP */

  bt_rm_init_service_id( &conn_ptr->sec_res_ss_service_id );

}


/*===========================================================================

FUNCTION
  bt_rm_update_lpc_disconnect

DESCRIPTION
  Removes entries from the low power configuration table referencing
  a specified connection.

===========================================================================*/
LOCAL void bt_rm_update_lpc_disconnect
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16  i;

  for ( i = 0; i < BT_RM_MAX_LPCS; i++ )
  {
    if ( bt_rm_lpc[ i ].conn_ptr == conn_ptr )
    {
      bt_rm_init_low_power_config( &bt_rm_lpc[ i ] );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_update_lsr_disconnect

DESCRIPTION
  Removes entries from the link status registration table referencing
  a specified connection.

===========================================================================*/
LOCAL void bt_rm_update_lsr_disconnect
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16  i;

  for ( i = 0; i < BT_RM_MAX_LSRS; i++ )
  {
    if ( bt_rm_lsr[ i ].conn_ptr == conn_ptr )
    {
      bt_rm_init_link_status_reg( &bt_rm_lsr[ i ] );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_conn_pin_request

DESCRIPTION
  Initializes the PIN request portion of a specified connection.

===========================================================================*/
LOCAL void bt_rm_init_conn_pin_request
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->pin_req_resps_pending   = 0;
  conn_ptr->pin_req_timeout_ms_left = 0;
  conn_ptr->pin_req_resp_app_id     = BT_APP_ID_NULL;
  conn_ptr->pin_req_pairing_done    = FALSE;

}


/*===========================================================================

FUNCTION
  bt_rm_init_connection

DESCRIPTION
  Initializes a specified connection completely (ACL & SCO).

===========================================================================*/
LOCAL void bt_rm_init_connection
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->bt_app_id_acl                  = BT_APP_ID_NULL;
  conn_ptr->state_acl                      = BT_RM_CS_DISCONNECTED;
  conn_ptr->state_sco                      = BT_RM_CS_DISCONNECTED;
  conn_ptr->busy_timeout_ms_left           = 0;
  conn_ptr->updates                        = BT_RM_LU_NONE;
  conn_ptr->role                           = BT_ROLE_NONE;
  conn_ptr->role_request                   = BT_ROLE_NONE;
  conn_ptr->hc_handle_acl                  = BT_HC_NULL_CONN_HANDLE;
  conn_ptr->hc_error                       = BT_BE_SUCCESS;
  conn_ptr->acl_resps_pending              = 0;
  conn_ptr->acl_req_timeout_ms_left        = 0;
  conn_ptr->link_mode                      = BT_LM_NONE;
  conn_ptr->link_mode_request              = BT_LM_NONE;
  conn_ptr->max_interval                   = BT_RM_DEF_MAX_INTERVAL;
  conn_ptr->cur_interval                   = 0;
#ifdef BT_SWDEV_2_1_SSR
  conn_ptr->max_latency                    = BT_RM_DEF_MAX_LATENCY;
  conn_ptr->cur_latency                    = 0;
  conn_ptr->stick_to_app_param             = FALSE;
  conn_ptr->sniff_initiator_oriented       = TRUE;
  conn_ptr->sniff_param_changed            = FALSE;
  conn_ptr->sniff_retry_to_ms_left         = 0;
  conn_ptr->ssr_emulating                  = FALSE;
  conn_ptr->ssr_state                      = BT_RM_SSR_NOT_IN_PROGRESS;
  conn_ptr->sniff_mode_to_ms_left          = 0;
#endif /* BT_SWDEV_2_1_SSR */
  conn_ptr->mss_enh_enc_timeout_ms_left    = 0;
  conn_ptr->enh_enc_state                  = BT_RM_ENH_ENC_OK;
  conn_ptr->re_pair_pin_state              = BT_RM_RE_PAIR_PIN_NONE;
#ifdef BT_SWDEV_2_1_SSP
  conn_ptr->sec_level                      = BT_RM_SL_0_NONE;
  conn_ptr->sec_level_min                  = BT_RM_SL_0_NONE;
  conn_ptr->sec_level_request              = BT_RM_SL_0_NONE;
#else
  conn_ptr->security                       = BT_SEC_NONE;
  conn_ptr->security_min                   = BT_SEC_NONE;
  conn_ptr->security_request               = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */
  conn_ptr->security_min_pin_len_bytes     = 0;
  conn_ptr->security_comb_key_only         = FALSE;
  conn_ptr->security_min_enc_key_len_bytes = 0;
  conn_ptr->authorized                     = FALSE;
  conn_ptr->authorization_required         = FALSE;
  conn_ptr->authorize_first                = FALSE;
  conn_ptr->authorize_req_app_id           = BT_APP_ID_NULL;
  conn_ptr->pin_code_reply_len_bytes       = 0;
  conn_ptr->pin_code_reply_neg_sent        = FALSE;
  conn_ptr->hc_auth_enable                 = BT_HC_AUTH_DISABLED;
  conn_ptr->authenticated                  = FALSE;
  conn_ptr->encrypt_mode                   = BT_EM_DISABLED;
  conn_ptr->encrypt_enable                 = BT_ENCRYPT_ENABLE_OFF;
  conn_ptr->enc_key_len_bytes              = 0;
#ifdef FEATURE_BT_QSOC
  conn_ptr->vs_ekl_rd_in_progress          = FALSE;
#endif /* FEATURE_BT_QSOC */
  conn_ptr->bonding_app_id                 = BT_APP_ID_NULL;
  conn_ptr->bonding_key_rxd                = FALSE;
  conn_ptr->bond_canceling                 = FALSE;
  conn_ptr->ev_hc_conn_acl_rxd             = FALSE;
  conn_ptr->rname_app_id                   = BT_APP_ID_NULL;
  conn_ptr->max_slots_allowed_acl          = BT_MAX_SLOTS_DEF;
  conn_ptr->max_slots_in_use_acl           = BT_MAX_SLOTS_DEF;
  conn_ptr->pkt_types_allowed_acl          = BT_PT_ACL_DEF_M;
  conn_ptr->pkt_types_in_use_acl           = BT_PT_ACL_DEF_M;
  conn_ptr->pkt_types_req_acl              = 0;
  conn_ptr->max_bytes_acl                  = BT_MTP_BB_INFO_LEN;
  conn_ptr->flush_timeout                  = BT_L2_DEFAULT_FLUSH_TIMEOUT;
  conn_ptr->link_policy                    = BT_RM_DEF_LINK_POLICY_M;
  conn_ptr->last_rssi                      = BT_RM_INVALID_RSSI;
  conn_ptr->bb_opt_ms_left                 = 0;
  conn_ptr->rd_remote_req_pending          = FALSE;
  conn_ptr->send_pin_req_pending           = FALSE;
  conn_ptr->remote_name_available          = FALSE;
#ifdef FEATURE_BT_SOC
  conn_ptr->rssi_update_ms_left            = 0;
#endif /* FEATURE_BT_SOC */
#ifdef BT_SWDEV_2_1_SSP
  conn_ptr->pairing_initiated              = FALSE;
  conn_ptr->dedicated_bonding              = FALSE;
  conn_ptr->local_auth_req                 = BT_RM_AR_NO_MITM_NO_BONDING;
  conn_ptr->remote_auth_req                = BT_RM_AR_NO_MITM_NO_BONDING;
  conn_ptr->mitm_protection_reqd           = FALSE;
  conn_ptr->re_pair_attempts               = 0;
  conn_ptr->auth_stage1_proc               = BT_RM_AS1_JUST_WORKS;
  conn_ptr->rebond_req_pending             = FALSE;
  conn_ptr->authorized_rebond              = FALSE;
  conn_ptr->update_link_key_request        = FALSE;
  conn_ptr->new_link_key                   = FALSE;
  conn_ptr->user_reply_neg                 = FALSE;
#endif /* BT_SWDEV_2_1_SSP */
#ifdef BT_SWDEV_2_1_EPR
  conn_ptr->enc_key_refresh_req_pending    = FALSE;
  conn_ptr->encryption_key_age             = 0;
#endif /* BT_SWDEV_2_1_EPR */
  conn_ptr->no_disc_on_del_link_key        = FALSE;
  conn_ptr->paging                         = FALSE;
  conn_ptr->bb_page_to_ms_left             = 0;

  memset( (void*)( &conn_ptr->remote_lmp_features.lmp_features_bytes),
                                                0, BT_RM_REMOTE_FEATURE_LEN );

#ifdef FEATURE_BT_2_1
  memset( (void*)( &conn_ptr->remote_lmp_features_ext.lmp_features_bytes),
                                                0, BT_RM_REMOTE_FEATURE_LEN );
#endif

  memset( (void*)(&conn_ptr->bonding_pin), 0,
          sizeof( bt_pin_code_type ) );

  if ( conn_ptr->dev_ptr != NULL )
  {
    conn_ptr->dev_ptr->references--;
    conn_ptr->dev_ptr = NULL;
  }

  bt_rm_init_connection_sco( conn_ptr );

  bt_rm_init_conn_idle_link_mode( conn_ptr );

  bt_rm_init_conn_pin_request( conn_ptr );

  bt_rm_update_lpc_disconnect( conn_ptr );

  bt_rm_update_lsr_disconnect( conn_ptr );

  bt_rm_init_conn_sec_results( conn_ptr );

}

#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_determine_bonded_key

DESCRIPTION
  Used during SSP when both devices Authentication requriments are known.
  It will use these requirements to determine if the link key will be
  a bonded key or not.

===========================================================================*/
LOCAL boolean bt_rm_determine_bonded_key
(
  bt_rm_conn_type*    conn_ptr
)
{
  boolean bonded_key = FALSE;

  if (( conn_ptr->remote_auth_req == BT_RM_AR_MITM_DEDICATED_BONDING    ) ||
      ( conn_ptr->remote_auth_req == BT_RM_AR_MITM_GENERAL_BONDING      ) ||
      ( conn_ptr->remote_auth_req == BT_RM_AR_NO_MITM_DEDICATED_BONDING ) ||
      ( conn_ptr->remote_auth_req == BT_RM_AR_NO_MITM_GENERAL_BONDING   ) ||
      ( conn_ptr->local_auth_req  == BT_RM_AR_MITM_DEDICATED_BONDING    ) ||
      ( conn_ptr->local_auth_req  == BT_RM_AR_MITM_GENERAL_BONDING      ) ||
      ( conn_ptr->local_auth_req  == BT_RM_AR_NO_MITM_DEDICATED_BONDING ) ||
      ( conn_ptr->local_auth_req  == BT_RM_AR_NO_MITM_GENERAL_BONDING   ))
  {
    bonded_key = TRUE;
  }
  BT_MSG_API( "BT RM: SSP H %x BondedLK %x",
              conn_ptr->hc_handle_acl, bonded_key, 0 );

  return bonded_key;
}

#endif /* BT_SWDEV_2_1_SSP */

/*===========================================================================

FUNCTION
  bt_rm_ev_send_radio_activity

DESCRIPTION
  Sends a RM Radio Activity event.
  Note : bt_radio_activity_type defines BT_RA_PAGING bit for "PAGING" state
  of the BT radio. VS event from controller would be required to report this
  state from the host. Current SOC based host implementations do not support
  reporting of "PAGING" state as no such VS event is available from the
  supported BT SOCs.

===========================================================================*/
LOCAL void bt_rm_ev_send_radio_activity
(
  void
)
{

  uint16          i;
  bt_ev_msg_type  ev;

  BT_MSG_HIGH( "BT RM: Radio Activity Updated RA %x",
              bt_rm_data.current_radio_activity, 0, 0 );

  ev.ev_hdr.ev_type                   = BT_EV_RM_RADIO_ACTIVITY;
  ev.ev_msg.ev_rm_ract.radio_activity = bt_rm_data.current_radio_activity;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].radio_activity_registered != FALSE) )
    {
      ev.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;
      BT_MSG_API( "BT RM EV TX: Radio Activity AID %x RA %x",
                  ev.ev_hdr.bt_app_id,
                  ev.ev_msg.ev_rm_ract.radio_activity, 0 );
      bt_ec_send_event( &ev );
    }
  }

}

/*===========================================================================

FUNCTION
  bt_rm_proc_driver_idle_to

DESCRIPTION
  Processes driver idle timer expiration.

===========================================================================*/
void bt_rm_proc_driver_idle_to
(
  void
)
{
  /* Idle time out has expired. Disable driver */
  bt_dc_disable_driver();

  /* Update radio activity and send event notification. */
  bt_rm_data.current_radio_activity &= ~( BT_RA_RADIO_POWERED_ON );
  bt_rm_ev_send_radio_activity();
}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_device_updated

DESCRIPTION
  Sends a RM Device Updated event.

===========================================================================*/
LOCAL void bt_rm_ev_send_device_updated
(
  bt_app_id_type                bt_app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  boolean                       success,
  bt_rm_dev_update_status_type  update_status
)
{

  bt_ev_msg_type  ev_rm_du;

  ev_rm_du.ev_hdr.ev_type                   = BT_EV_RM_DEVICE_UPDATED;
  ev_rm_du.ev_hdr.bt_app_id                 = bt_app_id;
  ev_rm_du.ev_msg.ev_rm_devud.bd_addr       = *bd_addr_ptr;
  ev_rm_du.ev_msg.ev_rm_devud.success       = success;
  ev_rm_du.ev_msg.ev_rm_devud.update_status = update_status;


  BT_MSG_HIGH( "BT RM: DEV UPD S %x US %x AID %x",
               ev_rm_du.ev_msg.ev_rm_devud.success,
               ev_rm_du.ev_msg.ev_rm_devud.update_status,
               ev_rm_du.ev_hdr.bt_app_id );

  BT_MSG_API( "BT RM EV TX: DevUpd S %x US %x AID %x",
               ev_rm_du.ev_msg.ev_rm_devud.success,
               ev_rm_du.ev_msg.ev_rm_devud.update_status,
               ev_rm_du.ev_hdr.bt_app_id );

  bt_ec_send_event( &ev_rm_du );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_notify_device_updated

DESCRIPTION
  Sends a RM Device Updated event to all appropriate applications.

===========================================================================*/
LOCAL void bt_rm_ev_notify_device_updated
(
  bt_app_id_type      bt_app_id,
  bt_bd_addr_type*    bd_addr,
  bt_rm_device_type*  dev_ptr,
  boolean             success
)
{

  uint16  i;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].deliver_dev_updates != FALSE) )
    {
      bt_rm_ev_send_device_updated(
        bt_rm_app[ i ].bt_app_id, bd_addr,
        success, dev_ptr->update_status );

      if ( bt_rm_app[ i ].bt_app_id == bt_app_id )
      {
        bt_app_id = BT_APP_ID_NULL;
      }
    }
  }

  if ( bt_app_id != BT_APP_ID_NULL )
  {
    bt_rm_ev_send_device_updated(
      bt_app_id, bd_addr,
      success, dev_ptr->update_status );
  }

  dev_ptr->update_status = BT_RM_DUS_NONE;

}


#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION
  bt_rm_set_service_file_name

DESCRIPTION
  Sets the service file name for a specified device table entry.

===========================================================================*/
LOCAL void bt_rm_set_service_file_name
(
  bt_rm_device_type*  dev_ptr
)
{

  /*  Make last 2 characters of service file name be    */
  /*  the ASCII characters for the 2 decimal digits of  */
  /*  the device index (with leading '0' as necessary)  */
  /*  and make sure the string is NULL terminated.      */

    service_file_name[ BT_RM_SERV_DB_NAME_LEN - 3 ] =
    '0' +  (dev_ptr->index/10);

    service_file_name[ BT_RM_SERV_DB_NAME_LEN - 2 ] =
    '0' +  (dev_ptr->index%10);

    service_file_name[ BT_RM_SERV_DB_NAME_LEN - 1 ] = '\0';

}
#endif /* FEATURE_EFS */


/*===========================================================================

FUNCTION
  bt_rm_write_service_file

DESCRIPTION
  Write a Service Data Base file to the flash file system.

INPUT
  dev_ptr  Pointer to device for which the service file is to be written.

OUTPUT
  success  TRUE if all is OK, otherwise FALSE

============================================================================*/
LOCAL boolean bt_rm_write_service_file
(
  bt_rm_device_type*  dev_ptr
)
{

  boolean                 success = FALSE;

#ifdef FEATURE_EFS

  boolean                 write_fault = FALSE;
  fs_open_xparms_type     fs_open_xparms;
  fs_rsp_msg_type         fs_rsp_msg;
  fs_handle_type          fs_handle;
  bt_rm_service_efs_type  efs_serv;
  uint8                   i;

  if ( dev_ptr->update_status & BT_RM_DUS_SERVICES_B )
  {
    bt_rm_set_service_file_name( dev_ptr );

    if ( (dev_ptr->num_services > 0) &&
         (dev_ptr->num_services <= BT_SD_MAX_NUM_OF_SRV_REC) )
    {
      /* only write services to file if valid */
      if (dev_ptr->dev_public.valid != FALSE)
      {
        BT_MSG_HIGH( "BT RM: Writing SERV file DI %x",
                     dev_ptr->index, 0, 0 );

        fs_open( service_file_name, FS_OA_READWRITE, NULL, NULL, &fs_rsp_msg );

        if ( fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S )
        {
          BT_MSG_HIGH( "BT RM: Creating SERV file DI %x",
                       dev_ptr->index, 0, 0 );

          fs_open_xparms.create.cleanup_option   = FS_OC_CLOSE;
          fs_open_xparms.create.buffering_option = FS_OB_ALLOW;
          fs_open_xparms.create.attribute_mask   = FS_FA_UNRESTRICTED;

          fs_open( service_file_name, FS_OA_CREATE, &fs_open_xparms,
                   NULL, &fs_rsp_msg );
        }

        if ( fs_rsp_msg.any.status == FS_OKAY_S )
        {
          fs_handle = fs_rsp_msg.open.handle;

          efs_serv.database_version = BT_RM_SERV_DB_VERSION;
          efs_serv.index            = dev_ptr->index;
          efs_serv.bd_addr          = dev_ptr->dev_public.bd_addr;
          efs_serv.num_services     = dev_ptr->num_services;

          for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
          {
            efs_serv.service[ i ] = dev_ptr->service[ i ];
          }

          fs_write( fs_handle, &efs_serv,
                    sizeof( bt_rm_service_efs_type ), NULL, &fs_rsp_msg );

          if ( (fs_rsp_msg.any.status != FS_OKAY_S) ||
               (fs_rsp_msg.write.count != sizeof( bt_rm_service_efs_type )) )
          {
            write_fault = TRUE;

            BT_ERR( "BT RM: Wr SERV S %x C %x DI %x",
                    fs_rsp_msg.any.status, fs_rsp_msg.write.count,
                    dev_ptr->index );
          }

          fs_close( fs_handle, NULL, &fs_rsp_msg );

          if ( fs_rsp_msg.any.status == FS_OKAY_S )
          {
            if ( write_fault == FALSE )
            {
              success = TRUE;
            }
          }
          else
          {
            BT_ERR( "BT RM: Wr SERV close S %x DI %x",
                    fs_rsp_msg.any.status, dev_ptr->index, 0 );
          }
        }
        else
        {
          BT_ERR( "BT RM: Wr SERV open S %x DI %x",
                  fs_rsp_msg.any.status, dev_ptr->index, 0 );
        }
      }
      else
      {
        success = TRUE;
      }
    }
    else if ( dev_ptr->num_services == 0 )
    {
      /*  Device has no services to save.  */

      BT_MSG_HIGH( "BT RM: Removing SERV file DI %x",
                   dev_ptr->index, 0, 0 );

      fs_remove( service_file_name, NULL, &fs_rsp_msg );

      if ( (fs_rsp_msg.any.status == FS_OKAY_S) ||
           (fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S) )
      {
        success = TRUE;
      }
      else
      {
        BT_ERR( "BT RM: Wr SERV remove S %x DI %x",
                fs_rsp_msg.any.status, dev_ptr->index, 0 );
      }
    }
    else
    {
      BT_ERR( "BT RM: Wr SERV Bad NS %x DI %x",
              dev_ptr->num_services, dev_ptr->index, 0 );
    }

    if ( success == FALSE )
    {
      dev_ptr->dev_public.valid = FALSE;
    }
  }
  else
  {
    success = TRUE;
  }

#else /*  No EFS  */

  dev_ptr->dev_public.valid = FALSE;

  BT_ERR( "BT RM: Wr SERV NO EFS! DI %x",
          dev_ptr->index, 0, 0 );

#endif /* FEATURE_EFS */

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_rm_write_device_file

DESCRIPTION
  Write the Device file to the flash file system.

OUTPUT
  success  TRUE if all is OK, otherwise FALSE

============================================================================*/
LOCAL boolean bt_rm_write_device_file
(
  void
)
{

  boolean                success = FALSE;
  uint8                  i;

#ifdef FEATURE_EFS

  boolean                write_fault = FALSE;
  fs_open_xparms_type    fs_open_xparms;
  fs_rsp_msg_type        fs_rsp_msg;
  fs_handle_type         fs_handle;
  uint8                  database_version;
  bt_rm_device_efs_type  efs_dev;

  BT_MSG_HIGH( "BT RM: Writing DEV file", 0, 0, 0 );

  fs_open( BT_RM_DEV_DB_NAME, FS_OA_READWRITE, NULL, NULL, &fs_rsp_msg );

  if ( fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S )
  {
    BT_MSG_HIGH( "BT RM: Creating DEV file", 0, 0, 0 );

    fs_open_xparms.create.cleanup_option   = FS_OC_CLOSE;
    fs_open_xparms.create.buffering_option = FS_OB_ALLOW;
    fs_open_xparms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    fs_open( BT_RM_DEV_DB_NAME, FS_OA_CREATE, &fs_open_xparms,
             NULL, &fs_rsp_msg );
  }

  if ( fs_rsp_msg.any.status == FS_OKAY_S )
  {
    fs_handle = fs_rsp_msg.open.handle;

    database_version = BT_RM_DEV_DB_VERSION;

    fs_write( fs_handle, &database_version,
              sizeof( uint8 ), NULL, &fs_rsp_msg );

    if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
         (fs_rsp_msg.write.count == sizeof( uint8 )) )
    {
#ifdef BT_SWDEV_2_1_SSP
      fs_write( fs_handle, &bt_rm_data.sec_level,
                sizeof( bt_sec_level_type ), NULL, &fs_rsp_msg );
#else
      fs_write( fs_handle, &bt_rm_data.security,
                sizeof( bt_security_type ), NULL, &fs_rsp_msg );
#endif /* BT_SWDEV_2_1_SSP */

      if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
           (fs_rsp_msg.write.count == sizeof( bt_security_type )) )
      {
        i = 0;
        while ( (i < BT_RM_MAX_DEVICES) && (write_fault == FALSE) )
        {
          if ( i != bt_rm_device[ i ].index )
          {
            BT_ERR( "BT RM: Wr DEV Bad DI %x %x",
                    bt_rm_device[ i ].index, i, 0 );

            bt_rm_device[ i ].index = i;
          }

          efs_dev.index              = i;
          efs_dev.dev_public         = bt_rm_device[ i ].dev_public;
          efs_dev.page_scan_mode     = bt_rm_device[ i ].page_scan_mode;
          efs_dev.page_scan_rep_mode = bt_rm_device[ i ].page_scan_rep_mode;
          efs_dev.page_scan_per_mode = bt_rm_device[ i ].page_scan_per_mode;
          efs_dev.clk_offset         = bt_rm_device[ i ].clk_offset;
          efs_dev.link_key_valid     = bt_rm_device[ i ].link_key_valid;
          efs_dev.link_key           = bt_rm_device[ i ].link_key;
          efs_dev.link_key_pin_len_bytes =
            bt_rm_device[ i ].link_key_pin_len_bytes;
          efs_dev.num_services       = bt_rm_device[ i ].num_services;
#ifdef BT_SWDEV_2_1_SSP
          efs_dev.bonded_link_key        = bt_rm_device[ i ].bonded_link_key;
          efs_dev.authenticated_link_key = bt_rm_device[ i ].authenticated_link_key;
          efs_dev.eir_received           = bt_rm_device[ i ].eir_received;
#endif /* BT_SWDEV_2_1_SSP */

          fs_write( fs_handle, &efs_dev,
                    sizeof( bt_rm_device_efs_type ), NULL, &fs_rsp_msg );

          if ( (fs_rsp_msg.any.status != FS_OKAY_S) ||
               (fs_rsp_msg.write.count != sizeof( bt_rm_device_efs_type )) )
          {
            write_fault = TRUE;

            BT_ERR( "BT RM: Wr DEV S %x C %x DI %x",
                    fs_rsp_msg.any.status, fs_rsp_msg.write.count, i );
          }
          if ( write_fault != FALSE )
          {
            bt_rm_device[ i ].dev_public.valid = FALSE;
          }
          i++;
        }
      }
      else
      {
        write_fault = TRUE;

        BT_ERR( "BT RM: Wr DEV DB Sec S %x C %x %x",
                fs_rsp_msg.any.status, fs_rsp_msg.write.count,
                sizeof( bt_security_type ) );
      }
    }
    else
    {
      write_fault = TRUE;

      for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
      {
        bt_rm_device[ i ].dev_public.valid = FALSE;
      }

      BT_ERR( "BT RM: Wr DEV DB Ver S %x C %x %x",
              fs_rsp_msg.any.status, fs_rsp_msg.write.count,
              sizeof( uint8 ) );
    }

    fs_close( fs_handle, NULL, &fs_rsp_msg );

    if ( fs_rsp_msg.any.status == FS_OKAY_S )
    {
      if ( write_fault == FALSE )
      {
        i = 0;
        success = TRUE;
        while ( (i < BT_RM_MAX_DEVICES) && (success != FALSE) )
        {
          success = bt_rm_write_service_file( &bt_rm_device[ i++ ] );
        }
      }
    }
    else
    {
      BT_ERR( "BT RM: Wr DEV file close S %x",
              fs_rsp_msg.any.status, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT RM: Wr DEV open S %x C %x",
            fs_rsp_msg.any.status, 0, 0 );
  }


#else /*  No EFS  */

  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    bt_rm_device[ i ].dev_public.valid = FALSE;
  }

  BT_ERR( "BT RM: Wr DEV NO EFS!", 0, 0, 0 );

#endif /* FEATURE_EFS */

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_rm_device_db_updated

DESCRIPTION
  Keeps track of updates to the Device and Services Data Base.

===========================================================================*/
LOCAL void bt_rm_device_db_updated
(
  void
)
{

  uint32  temp_update_count;

  temp_update_count          = bt_rm_dev_db_update_count;
  temp_update_count         += 1;
  temp_update_count         &= BT_RM_DRIS_UPDATE_COUNT_M;

  bt_rm_dev_db_update_count &= ~BT_RM_DRIS_UPDATE_COUNT_M;
  bt_rm_dev_db_update_count |= temp_update_count;

}


/*===========================================================================

FUNCTION
  bt_rm_device_updated

DESCRIPTION
  Keeps track of updates to the Device Data Base and writes the file
  to EFS.  Sends the RM Device Updated event as necessary.

OUTPUT
  success  TRUE if all is OK, otherwise FALSE

===========================================================================*/
LOCAL boolean bt_rm_device_updated
(
  bt_app_id_type      bt_app_id,
  bt_rm_device_type*  dev_ptr
)
{

  boolean  success;

  BT_MSG_DEBUG( "BT RM: DEV Updated DI %x S %x",
                dev_ptr->index, dev_ptr->update_status, 0 );

  bt_rm_device_db_updated();

  if ( dev_ptr->update_status != BT_RM_DUS_SERVICES_B )
  {
    success = bt_rm_write_device_file();
  }
  else
  {
    success = bt_rm_write_service_file( dev_ptr );
  }

  /*  Remove driver only bit from updated status.  */
  dev_ptr->update_status &= ~BT_RM_DUS_INTERNAL_B;

  if ( dev_ptr->update_status != BT_RM_DUS_NONE )
  {
    bt_rm_ev_notify_device_updated( bt_app_id, &dev_ptr->dev_public.bd_addr, 
                                    dev_ptr, success );
  }

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_rm_not_testing

DESCRIPTION
  Checks that the RM is currently not in a test state that prevents
  otherwise normal use of the RM (i.e., not in Test Mode or Telec
  test mode).

RETURN VALUES
  BT_CS_GN_SUCCESS if the RM is not in a testing state;
    otherwise BT_CS_RM_BUSY

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_not_testing
(
  void
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  if ( (bt_rm_data.telec_started != FALSE) ||
       (bt_rm_data.test_mode_enabled != FALSE) )
  {
    status = BT_CS_RM_BUSY;

    BT_ERR( "BT RM: Testing - RM busy.", 0, 0, 0 );
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_no_connections

DESCRIPTION
  Checks that the RM is not currently involved in any connections.

RETURN VALUES
  BT_CS_GN_SUCCESS if the RM is not involved in any connections;
    otherwise BT_CS_RM_BUSY

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_no_connections
(
  void
)
{

  uint16  i;

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED )
    {
      status = BT_CS_RM_BUSY;
      break;
    }
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_acl_link_active

DESCRIPTION
  Determines if the specified ACL link is currently active.  If not this
  routine will request the link be Unsniffed or Unparked as appropriate.

RETURN VALUES
  TRUE if link is currently active; FALSE otherwise

===========================================================================*/
LOCAL boolean bt_rm_acl_link_active
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean link_active = TRUE;

  /*  NOTE:  Any changes to the logic here may require  */
  /*         updates to bt_rm_check_busy_timeout().     */

  if ( conn_ptr->link_mode != BT_LM_ACTIVE )
  {
    link_active = FALSE;

    if ( conn_ptr->link_mode_request == BT_LM_NONE )
    {
      if ( conn_ptr->link_mode == BT_LM_SNIFF )
      {
        BT_MSG_API( "BT RM CMD TX: HC UnSniff H %x",
                    conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );

#ifdef BT_SWDEV_2_1_SSR
        if (conn_ptr->ssr_emulating == TRUE)
        {
          BT_MSG_API( "BT RM: SSR emulation terminated", 0, 0, 0 );

          conn_ptr->ssr_emulating = FALSE;
        }
#endif /* BT_SWDEV_2_1_SSR */
        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
      else if ( conn_ptr->link_mode == BT_LM_PARK )
      {
        BT_MSG_API( "BT RM CMD TX: HC UnPark H %x",
                    conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_exit_park_mode( conn_ptr->hc_handle_acl );

        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
    }
  }

  return ( link_active );

}

/*===========================================================================

FUNCTION
  bt_rm_acl_link_txrdy

DESCRIPTION
  Subject to configured idle_link_mode configuration, link mode transition
  to active will be requested if the link is not already active.

RETURN VALUES

===========================================================================*/
LOCAL void bt_rm_acl_link_txrdy
(
  bt_rm_conn_type*  conn_ptr
)
{
  if ( conn_ptr->link_mode != BT_LM_ACTIVE )
  {
    if ( conn_ptr->link_mode_request == BT_LM_NONE )
    {
      if ( ( conn_ptr->link_mode == BT_LM_SNIFF ) &&
           ( conn_ptr->idle_link_mode != BT_LM_SNIFF_LOCK ) )
      {
        BT_MSG_API( "BT RM CMD TX: HC UnSniff H %x",
                    conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );

#ifdef BT_SWDEV_2_1_SSR
        if (conn_ptr->ssr_emulating == TRUE)
        {
          BT_MSG_API( "BT RM: SSR emulation terminated", 0, 0, 0 );

          conn_ptr->ssr_emulating = FALSE;
        }
#endif /* BT_SWDEV_2_1_SSR */
        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
      else if ( conn_ptr->link_mode == BT_LM_PARK )
      {
        BT_MSG_API( "BT RM CMD TX: HC UnPark H %x",
                    conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_exit_park_mode( conn_ptr->hc_handle_acl );

        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
    }
  }

  return;
}

/*===========================================================================

FUNCTION
  bt_rm_connection_lmp_stable

DESCRIPTION
  Checks whether that a specified connection is "LMP stable", meaning that
  there are no outstanding PIN or Authorize Requests, bonding operations,
  Remote Name Requests, Role Switch, or Security related requests.

RETURN VALUES
  BT_CS_GN_SUCCESS if the connection is LMP stable;
    otherwise BT_CS_GN_RETRY_CMD_LATER

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_connection_lmp_stable
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  if ( (conn_ptr->bonding_app_id != BT_APP_ID_NULL) ||
       (conn_ptr->rname_app_id != BT_APP_ID_NULL) ||
       (conn_ptr->pin_req_resps_pending != 0) ||
       (conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL) ||
       (conn_ptr->authorize_req_app_id != BT_APP_ID_NULL) ||
       (conn_ptr->role_request != BT_ROLE_NONE) ||
#ifdef BT_SWDEV_2_1_SSP
       (conn_ptr->sec_level_request != BT_RM_SL_0_NONE) ||
#else
       (conn_ptr->security_request != BT_SEC_NONE) ||
#endif /* BT_SWDEV_2_1_SSP */
       (conn_ptr->pkt_types_req_acl != 0) ||
       (conn_ptr->enh_enc_state != BT_RM_ENH_ENC_OK) ||
       (conn_ptr->rd_remote_req_pending != FALSE)
#ifdef FEATURE_BT_QSOC
       || (conn_ptr->vs_ekl_rd_in_progress != FALSE)
#endif /* FEATURE_BT_QSOC */
       )
  {
    status = BT_CS_GN_RETRY_CMD_LATER;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_connection_stable

DESCRIPTION
  Checks that a specified connection is "stable", meaning that neither
  the ACL or SCO link is being brought up or torn down and that it is
  also "LMP stable" (see header of bt_rm_connection_lmp_stable() for
  description).

RETURN VALUES
  BT_CS_GN_SUCCESS if the connection is stable;
    otherwise BT_CS_GN_RETRY_CMD_LATER

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_connection_stable
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  /*  NOTE:  Any changes to the logic here may require  */
  /*         updates to bt_rm_check_busy_timeout().     */

  if ( (conn_ptr->state_acl != BT_RM_CS_CONNECTED) ||
       ((conn_ptr->state_sco != BT_RM_CS_DISCONNECTED) &&
        (conn_ptr->state_sco != BT_RM_CS_CONNECTED)) )
  {
    status = BT_CS_GN_RETRY_CMD_LATER;
  }
  else
  {
    status = bt_rm_connection_lmp_stable( conn_ptr );
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_connection_settled

DESCRIPTION
  Checks that a specified connection is "settled", which means it is
  currently "stable" (see header of bt_rm_connection_stable() for
  description) and furthermore, if the ACL link is currently in Sniff
  or Park this routine will request that it be Unsniffed or UnParked.

RETURN VALUES
  BT_CS_GN_SUCCESS if the connection is settled;
    otherwise BT_CS_GN_RETRY_CMD_LATER

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_connection_settled
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_cmd_status_type  status;

  /*  NOTE:  Any changes to the logic here may require  */
  /*         updates to bt_rm_check_busy_timeout().     */

  if ( (status = bt_rm_connection_stable( conn_ptr )) ==
          BT_CS_GN_SUCCESS )
  {
    if ( bt_rm_acl_link_active( conn_ptr ) == FALSE )
    {
      status = BT_CS_GN_RETRY_CMD_LATER;
    }
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_find_free_connection

DESCRIPTION
  Finds an unused connection table entry, if available.

RETURN VALUE
  Pointer to empty connection table entry, if available; otherwise NULL.

===========================================================================*/
LOCAL bt_rm_conn_type* bt_rm_find_free_connection
(
  void
)
{

  bt_rm_conn_type*  conn_ptr = NULL;
  uint16            i;
  uint16            acl_links = 0;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].state_acl == BT_RM_CS_DISCONNECTED )
    {
      if ( conn_ptr == NULL )
      {
        conn_ptr = &bt_rm_conn[ i ];
      }
    }
    else
    {
      acl_links++;
    }
  }
  if ( acl_links >= BT_MAX_ACL_CONNECTIONS )
  {
    conn_ptr = NULL;
  }

  return ( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_get_device

DESCRIPTION
  Finds a specified device in the device table.  If not present and
  required, a new entry in the table will be made for the device.

RETURN VALUE
  Pointer to device table entry
  NULL if no more room in table and entry was required or if the
    entry was not required and also not found in table

===========================================================================*/
LOCAL bt_rm_device_type* bt_rm_get_device
(
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           entry_required
)
{

  uint16              i;
  bt_rm_device_type*  dev_ptr = NULL;
  bt_rm_device_type*  alt_dev_ptr = NULL;

  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    if ( BT_BD_ADDRS_EQUAL(
           &bt_rm_device[ i ].dev_public.bd_addr,
           bd_addr_ptr ) != FALSE )
    {
      dev_ptr = &bt_rm_device[ i ];
      break;
    }
  }

  if ( (dev_ptr == NULL) && (entry_required != FALSE) )
  {
    for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
    {
      if ( (bt_rm_device[ i ].dev_public.valid == FALSE) &&
           (bt_rm_device[ i ].references == 0) )
      {
        if ( bt_rm_device[ i ].inq_result_sent == FALSE )
        {
          dev_ptr = &bt_rm_device[ i ];
          break;
        }
        else
        {
          alt_dev_ptr = &bt_rm_device[ i ];
        }
      }
    }

    if ( dev_ptr == NULL )
    {
      dev_ptr = alt_dev_ptr;
    }

    if ( dev_ptr != NULL )
    {
      bt_rm_init_device( dev_ptr );

      dev_ptr->dev_public.bd_addr = *bd_addr_ptr;
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Device table FULL!", 0, 0, 0 );
    }
  }

  return ( dev_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_get_conn_bd_addr

DESCRIPTION
  Finds the connection table entry corresponding to a specified BD
  address and when necessary it sets up a new connection table entry.
  A pointer to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if connection pointer is valid; otherwise an
    appropriate command status failure value is returned and the
    connection pointer is invalid.
  BT_CS_GN_RETRY_CMD_LATER if the connection that was found is
    in a condition that prevents another command from being
    processed right now.
  BT_CS_* as appropriate for other failures.
  Pointer to connection table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_conn_bd_addr
(
  boolean            must_exist,
  boolean            must_be_settled,
  bt_bd_addr_type*   bd_addr_ptr,
  bt_link_type       link_type,
  bt_rm_conn_type**  conn_ptr_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr = NULL;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    if ( (conn_ptr = bt_rm_find_conn_bd_addr( bd_addr_ptr )) == NULL )
    {
      if ( must_exist == FALSE )
      {
        if ( (conn_ptr = bt_rm_find_free_connection()) != NULL )
        {
          if ( (conn_ptr->dev_ptr =
                  bt_rm_get_device( bd_addr_ptr, TRUE )) != NULL )
          {
            conn_ptr->dev_ptr->references++;
          }
          else
          {
            status = BT_CS_RM_MAX_DEVICES;
          }
        }
        else
        {
          status = BT_CS_RM_MAX_CONNECTIONS;
        }
      }
      else
      {
        status = BT_CS_RM_NO_CONNECTION;
      }
    }
    else
    {
      if ( ( (link_type == BT_SCO_LINK)
#ifdef FEATURE_BT_1_2
             || (link_type == BT_ESCO_LINK)
#endif
           ) && (conn_ptr->state_acl != BT_RM_CS_CONNECTED) )
      {
        status = BT_CS_GN_RETRY_CMD_LATER;
      }
      else if ( must_be_settled != FALSE )
      {
        status = bt_rm_connection_settled( conn_ptr );
      }
    }
  }

  if ( status == BT_CS_GN_SUCCESS )
  {
    *conn_ptr_ptr = conn_ptr;
  }
  else
  {
    *conn_ptr_ptr = NULL;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_get_low_power_config

DESCRIPTION
  Finds the low power configuration table entry corresponding to a
  specified application ID and connection.  When necessary it sets up
  a new table entry.  A pointer to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if the low power configuration pointer is valid;
    otherwise an appropriate command status failure value is returned
    and the low power configuration pointer is invalid.
  BT_CS_RM_BUSY if updates to the RM low power configuration table would
    be inconsistent with what the RM is currently doing (e.g., test mode).
  Pointer to low power configuration table entry passed by reference is
    updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_low_power_config
(
  bt_app_id_type              bt_app_id,
  bt_bd_addr_type*            bd_addr_ptr,
  bt_rm_low_power_cfg_type**  lpc_ptr_ptr
)
{

  bt_cmd_status_type         status;
  bt_rm_low_power_cfg_type*  lpc_ptr = NULL;
  bt_rm_low_power_cfg_type*  free_lpc_ptr = NULL;
  bt_rm_conn_type*           conn_ptr;
  uint16                     i;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_LPCS; i++ )
    {
      if ( (bt_rm_lpc[ i ].bt_app_id == bt_app_id) &&
           (BT_BD_ADDRS_EQUAL(
              &bt_rm_lpc[ i ].conn_ptr->dev_ptr->dev_public.bd_addr,
              bd_addr_ptr ) != FALSE) )
      {
        lpc_ptr = &bt_rm_lpc[ i ];
      }
      else if ( (free_lpc_ptr == NULL) &&
                (bt_rm_lpc[ i ].bt_app_id == BT_APP_ID_NULL) )
      {
        free_lpc_ptr = &bt_rm_lpc[ i ];
      }
    }

    if ( lpc_ptr == NULL )
    {
      if ( free_lpc_ptr != NULL )
      {
        if ( (status =
                bt_rm_get_conn_bd_addr(
                  TRUE,   /*  Connection must exist.           */
                  FALSE,  /*  Connection need not be settled.  */
                  bd_addr_ptr, BT_ACL_LINK,
                  &conn_ptr )) == BT_CS_GN_SUCCESS )
        {
          lpc_ptr = free_lpc_ptr;

          lpc_ptr->bt_app_id = bt_app_id;
          lpc_ptr->conn_ptr  = conn_ptr;
        }
      }
      else
      {
        status = BT_CS_RM_MAX_APPLICATIONS;
      }
    }
  }

  *lpc_ptr_ptr = lpc_ptr;

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_get_application

DESCRIPTION
  Finds the application table entry corresponding to a specified ID.
  When necessary it sets up a new application table entry for the
  application.  A pointer to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if application pointer is valid; otherwise an
    appropriate command status failure value is returned and the
    application pointer is invalid.
  BT_CS_RM_BUSY if updates to the RM application table would be
    inconsistent with what the RM is currently doing (e.g., test mode).
  Pointer to application table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_application
(
  boolean           can_be_new_app,
  bt_app_id_type    bt_app_id,
  bt_rm_app_type**  app_ptr_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_app_type*     app_ptr = NULL;
  bt_rm_app_type*     free_app_ptr = NULL;
  uint16              i;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( bt_rm_app[ i ].bt_app_id == bt_app_id )
      {
        app_ptr = &bt_rm_app[ i ];
      }
      else if ( (free_app_ptr == NULL) &&
                (bt_rm_app[ i ].bt_app_id == BT_APP_ID_NULL) )
      {
        free_app_ptr = &bt_rm_app[ i ];
      }
    }

    if ( app_ptr == NULL )
    {
      if ( can_be_new_app != FALSE )
      {
        if ( free_app_ptr != NULL )
        {
          app_ptr = free_app_ptr;
          app_ptr->bt_app_id = bt_app_id;
        }
        else
        {
          status = BT_CS_RM_MAX_APPLICATIONS;
        }
      }
      else
      {
        status = BT_CS_GN_BAD_APP_ID;
      }
    }
  }

  *app_ptr_ptr = app_ptr;

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_get_link_status_reg

DESCRIPTION
  Finds the link status registration table entry corresponding to a
  specified application ID and connection.  When necessary it sets up
  a new table entry.  A pointer to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if the link status registration pointer is valid;
    otherwise an appropriate command status failure value is returned
    and the link status registration pointer is invalid.
  BT_CS_RM_BUSY if updates to the RM link status registration table would
    be inconsistent with what the RM is currently doing (e.g., test mode).
  Pointer to link status registration table entry passed by reference is
    updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_link_status_reg
(
  boolean                       can_be_new_lsr,
  bt_app_id_type                bt_app_id,
  bt_bd_addr_type*              bd_addr_ptr,
  bt_rm_link_status_reg_type**  lsr_ptr_ptr
)
{

  bt_cmd_status_type           status;
  bt_rm_link_status_reg_type*  lsr_ptr = NULL;
  bt_rm_link_status_reg_type*  free_lsr_ptr = NULL;
  bt_rm_conn_type*             conn_ptr;
  uint16                       i;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_LSRS; i++ )
    {
      if ( (bt_rm_lsr[ i ].bt_app_id == bt_app_id) &&
           (BT_BD_ADDRS_EQUAL(
              &bt_rm_lsr[ i ].conn_ptr->dev_ptr->dev_public.bd_addr,
              bd_addr_ptr ) != FALSE) )
      {
        lsr_ptr = &bt_rm_lsr[ i ];
      }
      else if ( (free_lsr_ptr == NULL) &&
                (bt_rm_lsr[ i ].bt_app_id == BT_APP_ID_NULL) )
      {
        free_lsr_ptr = &bt_rm_lsr[ i ];
      }
    }

    if ( lsr_ptr == NULL )
    {
      if ( can_be_new_lsr != FALSE )
      {
        if ( free_lsr_ptr != NULL )
        {
          if ( (status =
                  bt_rm_get_conn_bd_addr(
                    TRUE,   /*  Connection must exist.           */
                    FALSE,  /*  Connection need not be settled.  */
                    bd_addr_ptr, BT_ACL_LINK,
                    &conn_ptr )) == BT_CS_GN_SUCCESS )
          {
            lsr_ptr = free_lsr_ptr;

            lsr_ptr->bt_app_id = bt_app_id;
            lsr_ptr->conn_ptr  = conn_ptr;
          }
        }
        else
        {
          status = BT_CS_RM_MAX_APPLICATIONS;
        }
      }
      else
      {
        status = BT_CS_RM_NO_CONNECTION;
      }
    }
  }

  *lsr_ptr_ptr = lsr_ptr;

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_get_uuid_scn_pair

DESCRIPTION
  Finds the UUID SCN pair table entry corresponding to a specified
  RFCOMM SCN.  When necessary it sets up a new table entry.  A pointer
  to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if the UUID SCN pair pointer is valid;
    otherwise an appropriate command status failure value is returned
    and the UUID SCN pair pointer is invalid.
  BT_CS_RM_BUSY if updates to the RM UUID SCN pair table would
    be inconsistent with what the RM is currently doing (e.g., test mode).
  Pointer to UUID SCN pair table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_uuid_scn_pair
(
  boolean                     must_exist,
  uint8                       rfcomm_scn,
  bt_bd_addr_type*            bd_addr_ptr,
  bt_rm_uuid_scn_pair_type**  usp_ptr_ptr
)
{

  bt_cmd_status_type         status;
  bt_rm_uuid_scn_pair_type*  usp_ptr = NULL;
  bt_rm_uuid_scn_pair_type*  free_usp_ptr = NULL;
  uint16                     i;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_UUID_SCN_PAIRS; i++ )
    {
      if ( (bt_rm_uuid_scn[ i ].rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED) &&
           (bt_rm_uuid_scn[ i ].rfcomm_scn == rfcomm_scn) &&
           (((bd_addr_ptr == NULL) &&
                (bt_rm_uuid_scn[ i ].bd_addr_valid == FALSE)) ||
            ((bd_addr_ptr != NULL) &&
                (bt_rm_uuid_scn[ i ].bd_addr_valid != FALSE) &&
                   (BT_BD_ADDRS_EQUAL( bd_addr_ptr,
                                       &bt_rm_uuid_scn[ i ].bd_addr )))) )
      {
        usp_ptr = &bt_rm_uuid_scn[ i ];
        break;
      }
      else if ( (free_usp_ptr == NULL) &&
                (bt_rm_uuid_scn[ i ].rfcomm_scn == BT_RC_SCN_NOT_SPECIFIED) )
      {
        free_usp_ptr = &bt_rm_uuid_scn[ i ];
      }
    }

    if ( must_exist != FALSE )
    {
      if ( usp_ptr == NULL )
      {
        status = BT_CS_RM_BAD_UUID_SCN_PAIR;
      }
    }
    else
    {
      /*  Must be new.  */
      if ( usp_ptr == NULL )
      {
        if ( free_usp_ptr != NULL )
        {
          usp_ptr = free_usp_ptr;

          usp_ptr->rfcomm_scn = rfcomm_scn;
        }
        else
        {
          status = BT_CS_RM_MAX_UUID_SCN_PAIRS;
        }
      }
      else
      {
        usp_ptr = NULL;
        status  = BT_CS_RM_BAD_UUID_SCN_PAIR;
      }
    }
  }

  *usp_ptr_ptr = usp_ptr;

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_service_ids_equal

DESCRIPTION
  Compares 2 specified service identifiers.

RETURN VALUES
  TRUE if both service identifiers identify the same service
  FALSE otherwise

===========================================================================*/
LOCAL boolean bt_rm_service_ids_equal
(
  bt_service_id_type*  sid1_ptr,
  bt_service_id_type*  sid2_ptr
)
{

  boolean  service_ids_equal = FALSE;

  if ( sid1_ptr->service_id_method == sid2_ptr->service_id_method )
  {
    switch ( sid1_ptr->service_id_method )
    {
      case BT_SIM_SDP_UUID:
        {
          if ( (sid1_ptr->sdp_uuid != BT_SD_INVALID_UUID) &&
               (sid1_ptr->sdp_uuid == sid2_ptr->sdp_uuid) )
          {
            service_ids_equal = TRUE;
          }
          break;
        }

      case BT_SIM_RFCOMM_SCN:
        {
          if ( (sid1_ptr->rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED) &&
               (sid1_ptr->rfcomm_scn == sid2_ptr->rfcomm_scn) )
          {
            service_ids_equal = TRUE;
          }
          break;
        }

      case BT_SIM_L2CAP_PSM:
        {
          if ( (sid1_ptr->l2cap_psm != BT_L2_PSM_INVALID) &&
               (sid1_ptr->l2cap_psm == sid2_ptr->l2cap_psm) )
          {
            service_ids_equal = TRUE;
          }
          break;
        }

      case BT_SIM_DEFAULT_RFCOMM:
      case BT_SIM_DEFAULT_L2CAP:
        {
          service_ids_equal = TRUE;
          break;
        }

      default:
        {
          BT_ERR( "BT RM: Bad Service ID M %x",
                  sid1_ptr->service_id_method, 0, 0 );
          break;
        }
    }
  }

  return ( service_ids_equal );

}


/*===========================================================================

FUNCTION
  bt_rm_get_service_security

DESCRIPTION
  Finds the service security table entry corresponding to a
  specified application ID and service ID.  When necessary it sets up
  a new table entry.  A pointer to the entry is provided if available.

RETURN VALUES
  BT_CS_GN_SUCCESS if the service security pointer is valid;
    otherwise an appropriate command status failure value is returned
    and the service security pointer is invalid.
  BT_CS_RM_BUSY if updates to the RM service security table would
    be inconsistent with what the RM is currently doing (e.g., test mode).
  Pointer to service security table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_service_security
(
  boolean                   can_be_new_ss,
  bt_app_id_type            bt_app_id,
  bt_service_id_type*       service_id_ptr,
  bt_rm_service_sec_type**  ss_ptr_ptr
)
{

  bt_cmd_status_type       status;
  bt_rm_service_sec_type*  ss_ptr = NULL;
  bt_rm_service_sec_type*  free_ss_ptr = NULL;
  uint16                   i;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_SERV_SECS; i++ )
    {
      if ( bt_rm_service_ids_equal(
             service_id_ptr, &bt_rm_ss[ i ].service_id) != FALSE )
      {
        if ( (bt_app_id == BT_APP_ID_NULL) ||
             (bt_rm_ss[ i ].bt_app_id == bt_app_id) )
        {
          ss_ptr = &bt_rm_ss[ i ];
        }
        else
        {
          status = BT_CS_GN_BAD_APP_ID;
        }
        break;
      }
      else if ( (free_ss_ptr == NULL) &&
                (bt_rm_ss[ i ].bt_app_id == BT_APP_ID_NULL) )
      {
        free_ss_ptr = &bt_rm_ss[ i ];
      }
    }

    if ( status == BT_CS_GN_SUCCESS )
    {
      if ( ss_ptr == NULL )
      {
        if ( can_be_new_ss != FALSE )
        {
          if ( free_ss_ptr != NULL )
          {
            ss_ptr = free_ss_ptr;

            ss_ptr->bt_app_id  = bt_app_id;
            ss_ptr->service_id = *service_id_ptr;
          }
          else
          {
            status = BT_CS_RM_MAX_SERVICE_SECS;
          }
        }
        else
        {
          status = BT_CS_RM_BAD_SERVICE_SEC;
        }
      }
    }
  }

  *ss_ptr_ptr = ss_ptr;

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_get_acl_conn_hc_handle

DESCRIPTION
  Finds an ACL connection table entry with a specified HC connection
  handle.

RETURN VALUE
  Pointer to connection table entry, if found; otherwise NULL.

===========================================================================*/
LOCAL bt_rm_conn_type* bt_rm_get_acl_conn_hc_handle
(
  uint16  hc_handle
)
{

  uint16              i;
  bt_rm_conn_type*    conn_ptr = NULL;

  if ( bt_rm_not_testing() == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
    {
      if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
           (bt_rm_conn[ i ].hc_handle_acl == hc_handle) )
      {
        conn_ptr = &bt_rm_conn[ i ];
        break;
      }
    }
  }

  return ( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_get_conn_hc_handle

DESCRIPTION
  Finds a connection table entry with a specified HC connection handle.

RETURN VALUE
  Pointer to connection table entry, if found; otherwise NULL.

===========================================================================*/
LOCAL bt_rm_conn_type* bt_rm_get_conn_hc_handle
(
  uint16  hc_handle
)
{

  uint16              i;
  bt_rm_conn_type*    conn_ptr = NULL;

  if ( bt_rm_not_testing() == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
    {
      if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
           ((bt_rm_conn[ i ].hc_handle_acl == hc_handle) ||
            (bt_rm_conn[ i ].hc_handle_sco == hc_handle)) )
      {
        conn_ptr = &bt_rm_conn[ i ];
        break;
      }
    }
  }

  return ( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_get_conn_rm_handle

DESCRIPTION
  Finds the connection table entry corresponding to a specified RM
  handle.  A pointer to the entry is provided if found.

RETURN VALUE
  BT_CS_GN_SUCCESS if connection pointer is valid; otherwise an
    appropriate command status failure value is returned and the
    connection pointer is invalid.
  Pointer to connection table entry passed by reference is updated.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_get_conn_rm_handle
(
  boolean            must_be_settled,
  bt_rm_handle_type  rm_handle,
  bt_rm_conn_type**  conn_ptr_ptr
)
{

  bt_cmd_status_type  status   = BT_CS_GN_SUCCESS;
  bt_rm_conn_type*    conn_ptr = NULL;

  if ( (status = bt_rm_not_testing()) == BT_CS_GN_SUCCESS )
  {
    if ( rm_handle < BT_MAX_CONNECTIONS )
    {
      conn_ptr = &bt_rm_conn[ rm_handle ];

      if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
      {
        status   = BT_CS_RM_NO_CONNECTION;
        conn_ptr = NULL;
      }
      else if ( must_be_settled != FALSE )
      {
        if ( (status = bt_rm_connection_settled( conn_ptr )) !=
                BT_CS_GN_SUCCESS )
        {
          conn_ptr = NULL;
        }
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad H %x", rm_handle, 0, 0 );
      status = BT_CS_RM_BAD_HANDLE;
    }
  }

  *conn_ptr_ptr = conn_ptr;
  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_connected

DESCRIPTION
  Sends an RM Connected event for a specified connection and link type.

===========================================================================*/
LOCAL void bt_rm_ev_send_connected
(
  bt_rm_conn_type*  conn_ptr,
  bt_link_type      link_type
)
{

  bt_ev_msg_type  ev_rm_cn;

  /*  BT_TBD:  For compatibility with some headsets.       */
  /*           L2CAP connection requests go out too fast.  */
  rex_timed_wait( BT_WAIT_SIG, &bt_wait_timer, 5 );
  rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( link_type == BT_ACL_LINK )
  {
    ev_rm_cn.ev_hdr.ev_type             = BT_EV_RM_CONNECTED_ACL;
    ev_rm_cn.ev_hdr.bt_app_id           = conn_ptr->bt_app_id_acl;
    ev_rm_cn.ev_msg.ev_rm_conna.handle  = conn_ptr->rm_handle;
    ev_rm_cn.ev_msg.ev_rm_conna.bd_addr = conn_ptr->dev_ptr->dev_public.bd_addr;
  }
  else /*  SCO or eSCO  */
  {
    ev_rm_cn.ev_hdr.ev_type             = BT_EV_RM_CONNECTED_SCO;
    ev_rm_cn.ev_hdr.bt_app_id           = conn_ptr->bt_app_id_sco;
    ev_rm_cn.ev_msg.ev_rm_conns.handle  = conn_ptr->rm_handle;
    ev_rm_cn.ev_msg.ev_rm_conns.bd_addr = conn_ptr->dev_ptr->dev_public.bd_addr;

#ifdef FEATURE_BT_SOC
#ifndef FEATURE_BT_QSOC
    /* Double the scan window when SCO is present for SOC other than QSOC */
    /* TODO : Is this required for EV4 and EV5 packets ? */
    bt_rm_data.conf_page_scan_window = BT_PAGESCAN_WINDOW_SOC_SCO;
    bt_rm_data.conf_inq_scan_window = BT_INQUIRYSCAN_WINDOW_SOC_SCO;

    BT_MSG_API( "BT RM CMD TX: HC WrPScanAct I %x W %x",
                 bt_rm_data.conf_page_scan_interval,
                 bt_rm_data.conf_page_scan_window, 0 );
    bt_cmd_hc_wr_page_scan_act( bt_rm_data.conf_page_scan_interval,
                                bt_rm_data.conf_page_scan_window );

    BT_MSG_API( "BT RM CMD TX: HC WrIScanAct I %x W %x",
                 bt_rm_data.conf_inq_scan_interval,
                 bt_rm_data.conf_inq_scan_window, 0 );
    bt_cmd_hc_wr_inq_scan_act( bt_rm_data.conf_inq_scan_interval,
                               bt_rm_data.conf_inq_scan_window );
#else /* FEATURE_BT_QSOC */
/* snd_aux_pcm_clk_enable() API is not available on all targets. */
/* BT_SWDEV_NO_AUXPCM_CLK_CTRL should only be set on targets     */
/* that do not have this function.                               */
#ifndef BT_SWDEV_NO_AUXPCM_CLK_CTRL
    /* Request that the AVS enable the AuxPCM clock immediately */
    snd_aux_pcm_clk_enable(NULL);
    BT_MSG_HIGH( "BT RM: auxPcm clk enabled", 0, 0, 0);
#endif /* !BT_SWDEV_NO_AUXPCM_CLK_CTRL */
#endif /* !FEATURE_BT_QSOC */
#endif /* FEATURE_BT_SOC */

  }

  if ( ev_rm_cn.ev_hdr.bt_app_id != BT_APP_ID_NULL )
  {
    BT_MSG_HIGH( "BT RM: CONN T %x AID %x H %x",
                 link_type, ev_rm_cn.ev_hdr.bt_app_id,
                 conn_ptr->rm_handle );

    BT_MSG_API( "BT RM EV TX: Conn T %x AID %x H %x",
                link_type, ev_rm_cn.ev_hdr.bt_app_id,
                conn_ptr->rm_handle );
    bt_ec_send_event( &ev_rm_cn );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_copy_link_status

DESCRIPTION
  Provides the link status of a specified connection.

===========================================================================*/
LOCAL void bt_rm_copy_link_status
(
  bt_rm_conn_type*         conn_ptr,
  bt_rm_link_status_type*  link_status_ptr
)
{

  link_status_ptr->updates       = conn_ptr->updates;
  link_status_ptr->role          = conn_ptr->role;
#ifdef BT_SWDEV_2_1_SSP
  link_status_ptr->sec_level     = conn_ptr->sec_level;
#else
  link_status_ptr->security      = conn_ptr->security;
#endif /* BT_SWDEV_2_1_SSP */
  link_status_ptr->authorized    = conn_ptr->authorized;
  link_status_ptr->sco_present   =
     (conn_ptr->state_sco == BT_RM_CS_CONNECTED) ? TRUE : FALSE;
  link_status_ptr->role_sw_ok    =
    (conn_ptr->link_policy & BT_LINK_POLICY_ROLE_SWITCH_B) ? TRUE : FALSE;
  link_status_ptr->link_mode     = conn_ptr->link_mode;
  link_status_ptr->cur_interval_ms =
    BT_RM_625USEC_TO_MS( conn_ptr->cur_interval );
  link_status_ptr->pkt_types_acl = conn_ptr->pkt_types_in_use_acl;
  link_status_ptr->pkt_types_sco = conn_ptr->pkt_types_in_use_sco;
  link_status_ptr->error         = conn_ptr->hc_error;
  link_status_ptr->rssi          = conn_ptr->last_rssi;
  memcpy( (void*)(&link_status_ptr->sync_conn_params),
          (void*)(&conn_ptr->sync_conn_params),
          sizeof( bt_sync_conn_param_type ) );
}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_link_status

DESCRIPTION
  Sends an RM Link Status event for a specified connection.

===========================================================================*/
LOCAL void bt_rm_ev_send_link_status
(
  bt_app_id_type    bt_app_id,
  bt_rm_conn_type*  conn_ptr
)
{

  bt_ev_msg_type  ev_rm_ls;

  ev_rm_ls.ev_hdr.ev_type   = BT_EV_RM_LINK_STATUS;
  ev_rm_ls.ev_hdr.bt_app_id = bt_app_id;

  ev_rm_ls.ev_msg.ev_rm_lstat.link_status.bd_addr =
    conn_ptr->dev_ptr->dev_public.bd_addr;

  bt_rm_copy_link_status( conn_ptr,
                          &ev_rm_ls.ev_msg.ev_rm_lstat.link_status );

  BT_MSG_HIGH( "BT RM: LStat R %x M %x I %x",
               ev_rm_ls.ev_msg.ev_rm_lstat.link_status.role,
               ev_rm_ls.ev_msg.ev_rm_lstat.link_status.link_mode,
               ev_rm_ls.ev_msg.ev_rm_lstat.link_status.cur_interval_ms );

  BT_MSG_API( "BT RM EV TX: LStat AID %x M %x R %x",
              ev_rm_ls.ev_hdr.bt_app_id,
              ev_rm_ls.ev_msg.ev_rm_lstat.link_status.link_mode,
              ev_rm_ls.ev_msg.ev_rm_lstat.link_status.role );
  bt_ec_send_event( &ev_rm_ls );

}


/*===========================================================================

FUNCTION
  bt_rm_link_status_updated

DESCRIPTION
  Sends RM Link Status events as appropriate for a specified connection.

===========================================================================*/
LOCAL void bt_rm_link_status_updated
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean  send_link_status;
  uint16   i;
  uint8    cur_rssi_delta;

  if ( conn_ptr->updates != BT_RM_LU_NONE )
  {
    for ( i = 0; i < BT_RM_MAX_LSRS; i++ )
    {
      if ( (bt_rm_lsr[ i ].bt_app_id != BT_APP_ID_NULL) &&
           (bt_rm_lsr[ i ].conn_ptr == conn_ptr) )
      {
        send_link_status = FALSE;

        if ( conn_ptr->updates & BT_RM_LU_ERROR_VALID_B )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_ROLE_SW_OK_B) ||
             (conn_ptr->updates & BT_RM_LU_SCO_PRESENCE_B) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_SECURITY_B) &&
             (bt_rm_lsr[ i ].security_changes != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_LINK_MODE_B) &&
             (bt_rm_lsr[ i ].mode_changes != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_ROLE_B) &&
             (bt_rm_lsr[ i ].role_switches != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_ACL_PACKETS_B) &&
             (bt_rm_lsr[ i ].new_acl_pkts != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_SCO_PACKETS_B) &&
             (bt_rm_lsr[ i ].new_sco_pkts != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( (conn_ptr->updates & BT_RM_LU_RSSI_B) &&
             (bt_rm_lsr[ i ].rssi_delta != 0) )
        {
          /*  Send link status if RSSI has changed     */
          /*  by at least rssi_delta since last sent.  */

          /*  Calculate absolute value of change.  */
          if ( conn_ptr->last_rssi > bt_rm_lsr[ i ].rssi_last )
          {
            cur_rssi_delta = conn_ptr->last_rssi - bt_rm_lsr[ i ].rssi_last;

          }
          else
          {
            cur_rssi_delta = bt_rm_lsr[ i ].rssi_last - conn_ptr->last_rssi;
          }

          if ( cur_rssi_delta >= bt_rm_lsr[ i ].rssi_delta )
          {
            send_link_status = TRUE;
            bt_rm_lsr[ i ].rssi_last = conn_ptr->last_rssi;

#ifdef BT_DEBUG_RSSI
            BT_BDA( MSG_DEBUG, "BT RM: Output Link Status",
                    &conn_ptr->dev_ptr->dev_public.bd_addr );
            BT_MSG_DEBUG( "BT RM: Link Status RSSI %d",
                          conn_ptr->last_rssi, 0, 0 );
#endif
          }
        }

        if ( (conn_ptr->updates & BT_RM_LU_SYNC_PARAM_UPD_B ) &&
             (bt_rm_lsr[ i ].sync_param_updates != FALSE) )
        {
          send_link_status = TRUE;
        }

        if ( send_link_status != FALSE )
        {
          bt_rm_ev_send_link_status( bt_rm_lsr[ i ].bt_app_id, conn_ptr );
        }
      }
    }

    conn_ptr->hc_error = BT_BE_SUCCESS;
    conn_ptr->updates  = BT_RM_LU_NONE;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_update_rssi

DESCRIPTION
  Updates the RSSI of a specified connection.

===========================================================================*/
LOCAL void bt_rm_update_rssi
(
  bt_rm_conn_type*  conn_ptr,
  int8              rssi
)
{
  conn_ptr->last_rssi = rssi;

  conn_ptr->updates |= BT_RM_LU_RSSI_B;
  bt_rm_link_status_updated( conn_ptr );
}

/*===========================================================================

FUNCTION
  bt_rm_init_link

DESCRIPTION
  Initializes a specified link.

===========================================================================*/
LOCAL void bt_rm_init_link
(
  bt_rm_conn_type*  conn_ptr,
  bt_link_type      link_type
)
{

  if ( link_type == BT_ACL_LINK )
  {
    bt_rm_init_connection( conn_ptr );
  }
  else /*  SCO or eSCO  */
  {
    bt_rm_init_connection_sco( conn_ptr );

    /*  Allow 5 slot packets if possible.  */
    bt_rm_check_all_packet_types_acl();
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_completed_pin_request

DESCRIPTION
  Initializes PIN request information as appropriate for the
  specified connection.

===========================================================================*/
LOCAL void bt_rm_init_completed_pin_request
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( (conn_ptr->pin_req_resps_pending == 0) &&
       (conn_ptr->pin_req_pairing_done != FALSE) )
  {
    bt_rm_init_conn_pin_request( conn_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_reject_connection

DESCRIPTION
  Sends a HC Command to reject a connection.

===========================================================================*/
LOCAL void bt_rm_reject_connection
(
  bt_rm_conn_type*    conn_ptr,
  bt_error_code_type  hc_reason,
  bt_link_type        link_type
)
{

  BT_MSG_HIGH( "BT RM: Rej Conn R %x LT %x", hc_reason, link_type, 0 );
  BT_BDA( MSG_HIGH, "BT RM: Rej Conn",
          &conn_ptr->dev_ptr->dev_public.bd_addr );

  BT_MSG_API( "BT RM CMD TX: HC Rej Conn R %x LT %x",
              hc_reason, link_type, 0 );
  BT_BDA( MSG_API, "BT RM CMD TX: HC Rej Conn",
          &conn_ptr->dev_ptr->dev_public.bd_addr );

#ifdef FEATURE_BT_1_2
  if ( ( link_type == BT_SCO_LINK ) || ( link_type == BT_ESCO_LINK ) )
  {
    bt_cmd_hc_reject_sync_conn_req( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                    (uint8)(hc_reason) );
  }
  else
#endif /* FEATURE_BT_1_2 */
  {
    bt_cmd_hc_reject_connection( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                 (uint8)(hc_reason) );
  }

  bt_rm_init_link( conn_ptr, link_type );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_connection_failed

DESCRIPTION
  Sends an RM Connection Failed event for a specified connection
  and link type.

===========================================================================*/
LOCAL void bt_rm_ev_send_connection_failed
(
  bt_rm_conn_type*      conn_ptr,
  bt_link_type          link_type,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rm_cf;


  if ( link_type == BT_ACL_LINK )
  {
    ev_rm_cf.ev_hdr.ev_type             = BT_EV_RM_CONNECTION_FAILED_ACL;
    ev_rm_cf.ev_hdr.bt_app_id           = conn_ptr->bt_app_id_acl;
    ev_rm_cf.ev_msg.ev_rm_confa.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;
    ev_rm_cf.ev_msg.ev_rm_confa.reason  = reason;
  }
  else /*  SCO or eSCO  */
  {
    ev_rm_cf.ev_hdr.ev_type             = BT_EV_RM_CONNECTION_FAILED_SCO;
    ev_rm_cf.ev_hdr.bt_app_id           = conn_ptr->bt_app_id_sco;
    ev_rm_cf.ev_msg.ev_rm_confs.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;
    ev_rm_cf.ev_msg.ev_rm_confs.reason  = reason;
  }

  if ( ev_rm_cf.ev_hdr.bt_app_id != BT_APP_ID_NULL )
  {
    BT_MSG_HIGH( "BT RM: ConnF R %x LT %x", reason, link_type, 0 );
    BT_BDA( MSG_HIGH, "BT RM: ConnF",
            &conn_ptr->dev_ptr->dev_public.bd_addr );

    BT_MSG_API( "BT RM EV TX: ConnF R %x LT %x AID %x",
                reason, link_type, ev_rm_cf.ev_hdr.bt_app_id );
    BT_BDA( MSG_API, "BT RM EV TX: ConnF",
            &conn_ptr->dev_ptr->dev_public.bd_addr );
    bt_ec_send_event( &ev_rm_cf );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_conn_bd_addr_failed

DESCRIPTION
  Sends an RM Connection Failed event for a specified BD address
  and link type.

===========================================================================*/
LOCAL void bt_rm_ev_send_conn_bd_addr_failed
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_link_type          link_type,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rm_cf;

  if ( link_type == BT_ACL_LINK )
  {
    ev_rm_cf.ev_hdr.ev_type             = BT_EV_RM_CONNECTION_FAILED_ACL;
    ev_rm_cf.ev_hdr.bt_app_id           = bt_app_id;
    ev_rm_cf.ev_msg.ev_rm_confa.bd_addr = *bd_addr_ptr;
    ev_rm_cf.ev_msg.ev_rm_confa.reason  = reason;
  }
  else /*  SCO  */
  {
    ev_rm_cf.ev_hdr.ev_type             = BT_EV_RM_CONNECTION_FAILED_SCO;
    ev_rm_cf.ev_hdr.bt_app_id           = bt_app_id;
    ev_rm_cf.ev_msg.ev_rm_confs.bd_addr = *bd_addr_ptr;
    ev_rm_cf.ev_msg.ev_rm_confs.reason  = reason;
  }

  BT_MSG_HIGH( "BT RM: ConnF R %x T %x AID %x",
               reason, link_type, bt_app_id );
  BT_BDA( MSG_HIGH, "BT RM: ConnF", bd_addr_ptr );

  BT_MSG_API( "BT RM EV TX: ConnF R %x T %x AID %x",
              reason, link_type, ev_rm_cf.ev_hdr.bt_app_id );
  BT_BDA( MSG_API, "BT RM EV TX: ConnF", bd_addr_ptr );
  bt_ec_send_event( &ev_rm_cf );

}

/*===========================================================================

FUNCTION
  bt_rm_do_bb_page_started

DESCRIPTION
  This routine is called before sending a HCI cmd that causes baseband to 
  enter paging state.    
  
===========================================================================*/
LOCAL void bt_rm_do_bb_page_started
(
  bt_rm_conn_type*  conn_ptr
)
{
  boolean             page_to_timer_active = FALSE;
  uint16              i = 0;

  BT_MSG_DEBUG( "BT RM: Process page started RH %x TO %x RA %x ",
                 conn_ptr->rm_handle,
                 bt_rm_data.page_timeout,
                 bt_rm_data.current_radio_activity );

  conn_ptr->paging = TRUE;
  conn_ptr->bb_page_to_ms_left = 0;

  /* QSOC controller accepts multiple concurrent HCI_create_conn cmds.
   * Each connection creation is handled serially and controller will   
   * remain in paging state for each for up to the value specified in   
   * HCI_Page_Timeout. Controller's page timout processing is cloned    
   * in the host and at a time only one page timeout timer is activated.  
   * When bt_rm_do_bb_page_stopped() is called for any connection, it   
   * will start the time if another connection has "paging" bit set.    
   * All bets are off if cmd status for HCI_create_conn returns an      
   * error. This is a "known" RM limitation and ignored for the purpose 
   * of genergating page start/stop events.                            
  */

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( ( bt_rm_conn[ i ].paging == TRUE )&&
         ( bt_rm_conn[ i ].bb_page_to_ms_left != 0 )
       )
    {
      page_to_timer_active = TRUE;
      break;
    }
  }

  if ( bt_rm_data.current_radio_activity & BT_RA_PAGING )
  { /* BB was in paging state previously */
    if ( page_to_timer_active == FALSE )
    {
      /* Shouldn't be here .. log an error */
      BT_ERR( "BT RM: Inconsistent page state ", 0, 0, 0 );
    }
    else
    {
      /* Nothing to do for now. When page state is exited for the already
       * ongoing connection attempt, timer for this connection will be started
       * if required.
      */ 
      BT_MSG_DEBUG( "BT RM: Page timer already running RH %x",
                     i, 0, 0 );
    }
  }
  else 
  { 
    if ( page_to_timer_active == FALSE )
    {
      /* Start page timeout timer for this connection attempt */
      conn_ptr->bb_page_to_ms_left = (BT_RM_625USEC_TO_MS ( bt_rm_data.page_timeout )) 
                                      + BT_BACKGROUND_TIMER_MS;
      BT_MSG_DEBUG( "BT RM: Page timer started RH %x TOms %u",
                     conn_ptr->rm_handle, conn_ptr->bb_page_to_ms_left, 0 );
 
      /* New connection request will cause bb to start paging
       * Send RM radio activity event indicate bb has entered paging state 
      */
      bt_dc.page_active = TRUE;
      BT_RM_UPD_RA_AND_SETBIT( BT_RA_PAGING ); 


      if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
         (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
         (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
      {
	    if ( bt_dc.scan_active )
		{
		  /* Disable scans */
		  bt_dc.scan_active = FALSE;
		  bt_cmd_hc_wr_scan_enable( BT_HC_INQ_DIS_PAGE_DIS );
		  BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQ_SCANNING | BT_RA_PAGE_SCANNING );
		}
      }



      if( bt_rm_data.current_radio_activity & BT_RA_RADIO_POWERED_ON )
      {
        /* If radio is not powered on yet, HCI command is going to be
         * requed till radio is powered on in any case and at that point, a 
         * consolidated event will be sent.
        */
      bt_rm_ev_send_radio_activity();
    }
    }
    else
    {
      /* Shouldn't be here .. log an error */
      BT_ERR( "BT RM: Inconsistent page state", 0, 0, 0 );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_rm_do_bb_page_stopped

DESCRIPTION
  This routine is called under the following conditions :
    - Receipt of any HCI event that implies paging state exit at baseband.
        - HCI_Connection_Complete 
        - HCI_Remtote_Name_Complete 
      In addition, following events imply that baseband has already exited  
      paging state. Refer to "Volume 2, Part F, Message Sequence Charts" of
      the Bluetooth Specification.
        - HCI_Linkkey_Request
        - HCI_Pinkey_Request
        - HCI_Role_Changed_event
    - When RM maintained page timeout timer has expired for any connection 
      attempt. This can occur even when the connection has succeeded as 
      HCI connection complete can be delayed due to authorization/security 
      procedure that may be occuring at the remote HCI host interface.
  
===========================================================================*/
LOCAL void bt_rm_do_bb_page_stopped
(
  bt_rm_conn_type*  conn_ptr
)
{
  boolean             bb_is_paging = FALSE;
  uint16              i;

  BT_MSG_DEBUG( "BT RM: Process page stopped RH %x TOms %u RA %x ",
                 conn_ptr->rm_handle, conn_ptr->bb_page_to_ms_left, bt_rm_data.current_radio_activity );

  conn_ptr->paging = FALSE;
  conn_ptr->bb_page_to_ms_left = 0;

  /* Check if any connections are in paging state and start the timeout */
  /* timer on the first coonnection with paging state set */
  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].paging == TRUE )
    {
      /* Start page timeout timer for this connection attempt */
      bt_rm_conn[ i ].bb_page_to_ms_left = 
                      (BT_RM_625USEC_TO_MS ( bt_rm_data.page_timeout )) 
                      + BT_BACKGROUND_TIMER_MS;
      BT_MSG_DEBUG( "BT RM: Start timer on pending conn attempt RH %x TOms %u",
                     i, bt_rm_conn[ i ].bb_page_to_ms_left, 0 );
      bb_is_paging = TRUE;
      break;
    }
  }

  if ( bb_is_paging == FALSE ) 
  { /* All paging is timed out or completed */
    /* Send RM radio activity event to indicate bb has exited paging state */
    bt_dc.page_active = FALSE;
    BT_RM_UPD_RA_AND_CLRBIT( BT_RA_PAGING );
    bt_rm_ev_send_radio_activity();
	

    if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
       (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
       (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
    {
	   /* Re-evaluate required scans */
      bt_rm_check_scan_settings();
	}
  }
  else
  {
    /* Nothing to do for now. When page state is exited for all other already
     * ongoing connection attempts, event will be sent.
    */ 
    BT_MSG_DEBUG( "BT RM: Remaining in paging state",
                   0, 0, 0 );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_check_bb_page_state_timeout

DESCRIPTION
  Process timers that keep track of baseband page timeout. This is part of 
  the logic that generates approximate BB PAGE_START/STOP events based on 
  HCI commands and events.

===========================================================================*/
LOCAL void bt_rm_check_bb_page_state_timeout
(
  void
)
{
  uint16              i;

  /* Check for any connections with active page TO timers */
  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].bb_page_to_ms_left != 0 )
    {
      if ( bt_rm_conn[ i ].bb_page_to_ms_left <= BT_BACKGROUND_TIMER_MS )
      {
        /* Page TO timer has expired. Process "page stopped" on this 
         * connection. 
        */
        BT_MSG_DEBUG( "BT RM: Page TO expired RH %x", i, 0, 0 );
        bt_rm_do_bb_page_stopped( &bt_rm_conn[ i ] );
      }
      else
      {
        /* Update elapsed time */
        bt_rm_conn[ i ].bb_page_to_ms_left -= BT_BACKGROUND_TIMER_MS;
      }

      /* Break out of the for loop. Only one timer can be active.. */
      break;
    }
  }
}

/*===========================================================================

FUNCTION
  bt_rm_connection_failed

DESCRIPTION
  Handles the failure of a specified connection.

===========================================================================*/
LOCAL void bt_rm_connection_failed
(
  bt_rm_conn_type*      conn_ptr,
  bt_link_type          link_type,
  bt_event_reason_type  reason
)
{
  if( ( link_type == BT_ACL_LINK ) &&
      ( conn_ptr->paging == TRUE )
    )
  {
    /* Abnormal termination of page state .. cleanup */
    BT_MSG_HIGH( "BT RM: Connf .. cleanup page state RH %x",
                  conn_ptr->rm_handle, 0, 0 );
    bt_rm_do_bb_page_stopped( conn_ptr );
  }

  bt_rm_ev_send_connection_failed( conn_ptr, link_type, reason );

  bt_rm_init_link( conn_ptr, link_type );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_bonded

DESCRIPTION
  Sends a RM Bonded event for a specified connection.

===========================================================================*/
LOCAL void bt_rm_ev_send_bonded
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_ev_msg_type  ev_rm_b;

#ifdef BT_SWDEV_2_1_SSP
  uint16          i;

/* Don't send event in case of pairing */
  if (( conn_ptr->dedicated_bonding != TRUE ) &&
      ( conn_ptr->auth_stage1_proc == BT_RM_AS1_JUST_WORKS ) &&
      ( conn_ptr->dev_ptr->bonded_link_key != TRUE ))
  {
    return;
  }
#endif /* BT_SWDEV_2_1_SSP */

  ev_rm_b.ev_hdr.bt_app_id = BT_APP_ID_NULL;

  BT_MSG_HIGH( "BT RM: BONDED BAI %x PAI %x H %x",
               conn_ptr->bonding_app_id, conn_ptr->pin_req_resp_app_id,
               conn_ptr->rm_handle );
  BT_BDA( MSG_HIGH, "BT RM: BONDED",
          &conn_ptr->dev_ptr->dev_public.bd_addr );

  if ( conn_ptr->bonding_app_id != BT_APP_ID_NULL )
  {
    ev_rm_b.ev_hdr.bt_app_id = conn_ptr->bonding_app_id;
  }
  else if ( (conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL) &&
            (conn_ptr->pin_req_pairing_done == FALSE) )
  {
    ev_rm_b.ev_hdr.bt_app_id = conn_ptr->pin_req_resp_app_id;
  }

  /* Initilize Pin Request parameters */
  conn_ptr->pin_req_pairing_done = TRUE;
  bt_rm_init_completed_pin_request( conn_ptr );

  if ( ev_rm_b.ev_hdr.bt_app_id != BT_APP_ID_NULL )
  {
    ev_rm_b.ev_hdr.ev_type = BT_EV_RM_BONDED;

    ev_rm_b.ev_msg.ev_rm_bond.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;

#ifdef BT_SWDEV_2_1_SSP
    ev_rm_b.ev_msg.ev_rm_bond.lk_status_type =
      conn_ptr->dev_ptr->dev_public.link_key_status;

    BT_MSG_API( "BT RM EV TX: Bonded AID %x LKS %x",
                ev_rm_b.ev_hdr.bt_app_id,
                conn_ptr->dev_ptr->dev_public.link_key_status, 0 );
#else
    BT_MSG_API( "BT RM EV TX: Bonded AID %x",
                ev_rm_b.ev_hdr.bt_app_id, 0, 0 );

#endif /* BT_SWDEV_2_1_SSP */

    BT_BDA( MSG_API, "BT RM EV TX: Bonded",
            &ev_rm_b.ev_msg.ev_rm_bond.bd_addr );

    bt_ec_send_event( &ev_rm_b );
  }
#ifdef BT_SWDEV_2_1_SSP
  else
  {
    ev_rm_b.ev_hdr.ev_type = BT_EV_RM_BONDED;

    ev_rm_b.ev_msg.ev_rm_bond.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;

    ev_rm_b.ev_msg.ev_rm_bond.lk_status_type =
      conn_ptr->dev_ptr->dev_public.link_key_status;

    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
           (bt_rm_app[ i ].bondable != FALSE) )
      {
        ev_rm_b.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

        BT_MSG_API( "BT RM EV TX: Bonded AID %x LKS %x",
                    ev_rm_b.ev_hdr.bt_app_id,
                    conn_ptr->dev_ptr->dev_public.link_key_status, 0 );

        bt_ec_send_event( &ev_rm_b );
      }
    }
    BT_BDA( MSG_API, "BT RM EV TX: Bonded",
            &ev_rm_b.ev_msg.ev_rm_bond.bd_addr );
  }
#endif

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_bond_failed

DESCRIPTION
  Sends a RM Bond Failed event as specified.

===========================================================================*/
LOCAL void bt_rm_ev_send_bond_failed
(
  bt_app_id_type        bt_app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_rm_handle_type     rm_handle,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rm_bf;

  ev_rm_bf.ev_hdr.ev_type             = BT_EV_RM_BOND_FAILED;
  ev_rm_bf.ev_hdr.bt_app_id           = bt_app_id;
  ev_rm_bf.ev_msg.ev_rm_bondf.bd_addr = *bd_addr_ptr;
  ev_rm_bf.ev_msg.ev_rm_bondf.reason  = reason;

  BT_MSG_HIGH( "BT RM: BondF R %x H %x", reason, rm_handle, 0 );
  BT_BDA( MSG_HIGH, "BT RM: BondF", bd_addr_ptr );

  BT_MSG_API( "BT RM EV TX: BondF R %x AID %x",
              ev_rm_bf.ev_msg.ev_rm_bondf.reason,
              ev_rm_bf.ev_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM: BondF", bd_addr_ptr );
  bt_ec_send_event( &ev_rm_bf );

}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_bond_failed_all_bondable

DESCRIPTION
Send Bond fail event to all bondable apps  
===========================================================================*/
LOCAL void bt_rm_ev_send_bond_failed_all_bondable
(
  bt_rm_conn_type*      conn_ptr,
  bt_event_reason_type  reason
)
{
   uint16          i;
    /* Send Bond fail event to all bondable apps */

    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
      {
        BT_MSG_DEBUG( "BT RM: Sec cleanup: Sending bond failed BAI %x",
                      bt_rm_app[ i ].bt_app_id, 0, 0 );
        bt_rm_ev_send_bond_failed(
          bt_rm_app[ i ].bt_app_id,
          &conn_ptr->dev_ptr->dev_public.bd_addr,
          conn_ptr->rm_handle, 
          reason );
      }
    }
}

/*===========================================================================

FUNCTION
  bt_rm_update_device_name

DESCRIPTION
  Updates the name of a specified device as necessary.

===========================================================================*/
LOCAL void bt_rm_update_device_name
(
  bt_app_id_type      bt_app_id,
  bt_rm_device_type*  dev_ptr,
  char*               new_name_str_ptr
)
{

  uint8    i = 0;
  boolean  name_different = FALSE;

  while ( (i < BT_SD_MAX_DEVICE_NAME_LEN) &&
          (name_different == FALSE) )
  {
    if ( dev_ptr->dev_public.name_str[ i ] != new_name_str_ptr[ i ] )
    {
      name_different = TRUE;
    }

    if ( (dev_ptr->dev_public.name_str[ i ] == 0) ||
         (new_name_str_ptr[ i ] == 0) )
    {
      i = BT_SD_MAX_DEVICE_NAME_LEN;
    }
    else
    {
      i++;
    }
  }

  if ( name_different != FALSE )
  {
    memcpy( (void*)(dev_ptr->dev_public.name_str),
            (void*)(new_name_str_ptr),
            BT_SD_MAX_DEVICE_NAME_LEN );

    dev_ptr->dev_public.name_str[ BT_SD_MAX_DEVICE_NAME_LEN ] = 0;
#ifdef BT_SWDEV_2_1_EIR
    /* We have now received the full device name */
    SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B );
#endif /* BT_SWDEV_2_1_EIR */
    dev_ptr->update_status |= BT_RM_DUS_NAME_B;

    if ( dev_ptr->dev_public.valid != FALSE )
    {
      bt_rm_device_updated( bt_app_id, dev_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_deliver_remote_device_name

DESCRIPTION
  Provides the device name of a remote device to the RM.

===========================================================================*/
void bt_rm_deliver_remote_device_name
(
  bt_bd_addr_type*  bd_addr_ptr,
  char*             name_str
)
{

  bt_rm_device_type*  dev_ptr;

  BT_BDA( MSG_DEBUG, "BT RM RX: RemDevName", bd_addr_ptr );
  BT_MSG_DEBUG( "BT RM RX: RemDevName %c%c%c",
                name_str[ 0 ], name_str[ 1 ], name_str[ 2 ] );

  if ( (dev_ptr = bt_rm_get_device( bd_addr_ptr, TRUE )) != NULL )
  {
    bt_rm_update_device_name( bt_rm_app_id, dev_ptr, name_str );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: RemDevName ignored, table full.", 0, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_remote_name

DESCRIPTION
  Sends a RM Remote Name event for a specified connection.

===========================================================================*/
LOCAL void bt_rm_ev_send_remote_name
(
  bt_app_id_type      bt_app_id,
  bt_device_type*     dev_ptr,
  bt_cmd_status_type  status
)
{

  bt_ev_msg_type  ev_rm_rn;

  ev_rm_rn.ev_hdr.ev_type                   = BT_EV_RM_REMOTE_NAME;
  ev_rm_rn.ev_hdr.bt_app_id                 = bt_app_id;
  ev_rm_rn.ev_msg.ev_rm_rname.status        = status;
  ev_rm_rn.ev_msg.ev_rm_rname.bd_addr       = dev_ptr->bd_addr;

  if ( status == BT_CS_GN_SUCCESS )
  {
    memcpy( (void*)(ev_rm_rn.ev_msg.ev_rm_rname.name_str),
            (void*)(dev_ptr->name_str),
            BT_SD_MAX_DEVICE_NAME_LEN );
  }
  else
  {
    ev_rm_rn.ev_msg.ev_rm_rname.name_str[ 0 ] = 0;
  }

  BT_MSG_API( "BT RM EV TX: RName St %x C %x AID %x",
              ev_rm_rn.ev_msg.ev_rm_rname.status,
              ev_rm_rn.ev_msg.ev_rm_rname.name_str[ 0 ],
              ev_rm_rn.ev_hdr.bt_app_id );
  bt_ec_send_event( &ev_rm_rn );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_disconnected

DESCRIPTION
  Sends an RM Disconnected event for a specified connection and
  link type.

===========================================================================*/
LOCAL void bt_rm_ev_send_disconnected
(
  bt_rm_conn_type*      conn_ptr,
  bt_link_type          link_type,
  bt_event_reason_type  reason
)
{

  bt_ev_msg_type  ev_rm_d;

  if ( link_type == BT_ACL_LINK )
  {
    ev_rm_d.ev_hdr.ev_type            = BT_EV_RM_DISCONNECTED_ACL;
    ev_rm_d.ev_hdr.bt_app_id          = conn_ptr->bt_app_id_acl;
    ev_rm_d.ev_msg.ev_rm_disca.handle = conn_ptr->rm_handle;
    ev_rm_d.ev_msg.ev_rm_disca.reason = reason;
  }
  else /*  SCO  */
  {
    ev_rm_d.ev_hdr.ev_type            = BT_EV_RM_DISCONNECTED_SCO;
    ev_rm_d.ev_hdr.bt_app_id          = conn_ptr->bt_app_id_sco;
    ev_rm_d.ev_msg.ev_rm_discs.handle = conn_ptr->rm_handle;
    ev_rm_d.ev_msg.ev_rm_discs.reason = reason;

#ifdef FEATURE_BT_SOC
#ifndef FEATURE_BT_QSOC
    /* Restore scan window which was doubled when SCO is present */
    /* for SOC targets other than QSOC */
    bt_rm_data.conf_page_scan_window = BT_PAGESCAN_WINDOW_SOC;
    bt_rm_data.conf_inq_scan_window = BT_INQUIRYSCAN_WINDOW_SOC;

    BT_MSG_API( "BT RM CMD TX: HC WrPScanAct I %x W %x",
                 bt_rm_data.conf_page_scan_interval,
                 bt_rm_data.conf_page_scan_window, 0 );
    bt_cmd_hc_wr_page_scan_act( bt_rm_data.conf_page_scan_interval,
                                bt_rm_data.conf_page_scan_window );

    BT_MSG_API( "BT RM CMD TX: HC WrIScanAct I %x W %x",
                 bt_rm_data.conf_inq_scan_interval,
                 bt_rm_data.conf_inq_scan_window, 0 );
    bt_cmd_hc_wr_inq_scan_act( bt_rm_data.conf_inq_scan_interval,
                               bt_rm_data.conf_inq_scan_window );
#else /* FEATURE_BT_QSOC */
/* snd_aux_pcm_clk_disable() API is not available on all targets. */
/* BT_SWDEV_NO_AUXPCM_CLK_CTRL should only be set on targets     */
/* that do not have this function.                               */
#ifndef BT_SWDEV_NO_AUXPCM_CLK_CTRL
    /* tell AVS that BT is finished using AuxPCM */
    snd_aux_pcm_clk_disable(NULL);
    BT_MSG_HIGH( "BT RM: auxPcm clk disabled", 0, 0, 0);
#endif /* !BT_SWDEV_NO_AUXPCM_CLK_CTRL */
#endif /* !FEATURE_BT_QSOC */
#endif /* FEATURE_BT_SOC */

  }

  BT_MSG_HIGH( "BT RM: DISC R %x T %x H %x",
               reason, link_type, ev_rm_d.ev_msg.ev_rm_discs.handle );

  if ( ev_rm_d.ev_hdr.bt_app_id != BT_APP_ID_NULL )
  {
    BT_MSG_API( "BT RM EV TX: Disc R %x T %x AID %x",
                reason, link_type, ev_rm_d.ev_hdr.bt_app_id );
    bt_ec_send_event( &ev_rm_d );
  }

}


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_parse_eir

DESCRIPTION
  Parses the incoming EIR

===========================================================================*/
void bt_rm_parse_eir
(
  uint8*                eir_data,
  bt_rm_device_type*    dev_ptr
)
{
  uint16    index;
  uint8     eir_data_type_len;
  int       max_eir_data_len;
  uint8     eir_data_type;
  uint8     i;
  boolean   error = FALSE;

  BT_MSG_DEBUG( "BT RM: Parse EIR" ,0, 0, 0 );

  /* New EIR received from device - reset any old EIR data */
  bt_rm_init_eir_data( dev_ptr );

  /* Indicates that this is a BT2.1 compatible device */
  dev_ptr->eir_received = TRUE;
  dev_ptr->dev_public.ssp_capable = TRUE;
  dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;

  index = 0;

  /* Reset the EIR data flag as new EIR is being received for the device */
  dev_ptr->dev_public.eir_data.eir_flag = 0;

  while( index < BT_MAX_EIR_HCI_LEN )
  {
    max_eir_data_len = BT_MAX_EIR_HCI_LEN - 1 - index; 

    eir_data_type_len = eir_data[index++];
    eir_data_type     = eir_data[index++];

    if( eir_data_type_len > max_eir_data_len )
    {
      /* error condition */
      error = TRUE;
      BT_MSG_HIGH("BT RM: Bad EIR len :%x",eir_data_type_len,0,0);
      break;
    }

    /* If non significant part (zeroes) then skip */
    if( eir_data_type_len == 0)
    {
      BT_MSG_DEBUG("BT RM: Par EIR Skipping insignificant part i:%x", index,0,0);
      break;
    }

    /* To include only the data part */
    eir_data_type_len = eir_data_type_len - 1;

    switch( eir_data_type )
    {
      case BT_EIR_TYPE_FLAGS:
        BT_MSG_DEBUG( "BT RM: Par EIR Flags" ,0, 0, 0 );
        break;

      case BT_EIR_TYPE_UUID16_INCOMPLETE:
      case BT_EIR_TYPE_UUID16_COMPLETE:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
        if( eir_data_type == BT_EIR_TYPE_UUID16_COMPLETE )
        {
          SETBIT( dev_ptr->dev_public.eir_data.eir_flag,
                  BT_EIR_UUID16_LIST_CMPLT_B );
        }
        dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_16BIT_TYPE ] =
                                        eir_data_type_len/sizeof(bt_sd_uuid_type) ;
        dev_ptr->eir_uuid16_index = index;

        BT_MSG_DEBUG( "BT RM: Par EIR - UUID16: %x",
                       dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_16BIT_TYPE],
                       0, 0 );
        break;

      case BT_EIR_TYPE_UUID32_INCOMPLETE:
      case BT_EIR_TYPE_UUID32_COMPLETE:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
        if( eir_data_type == BT_EIR_TYPE_UUID32_COMPLETE )
        {
          SETBIT( dev_ptr->dev_public.eir_data.eir_flag,
                  BT_EIR_UUID32_LIST_CMPLT_B );
        }
        dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_32BIT_TYPE ] =
                                        eir_data_type_len/sizeof(bt_sd_uuid32_type) ;
        dev_ptr->eir_uuid32_index = index;
        BT_MSG_DEBUG( "BT RM: Par EIR - UUID32: %x",
                       dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_32BIT_TYPE],
                       0, 0 );
        break;

      case BT_EIR_TYPE_UUID128_INCOMPLETE:
      case BT_EIR_TYPE_UUID128_COMPLETE:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
        if( eir_data_type == BT_EIR_TYPE_UUID128_COMPLETE )
        {
          SETBIT( dev_ptr->dev_public.eir_data.eir_flag,
                  BT_EIR_UUID128_LIST_CMPLT_B );
        }
        dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_128BIT_TYPE ]
                                          = eir_data_type_len/sizeof(bt_sd_uuid128_type) ;
        dev_ptr->eir_uuid128_index = index;
        BT_MSG_DEBUG( "BT RM: Par EIR - UUID128: %x",
                       dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_128BIT_TYPE],
                       0, 0 );
        break;

      case BT_EIR_TYPE_LOCAL_NAME_SHORTENED:
      case BT_EIR_TYPE_LOCAL_NAME_COMPLETE:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );

        if( dev_ptr->inq_result_sent == TRUE &&
            ISBITSET( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B ) )
        {
          /* We have already received the name (using a name request) in this current inquiry
            dont bother with what is in the EIR */
          break;
        }

        if( eir_data_type == BT_EIR_TYPE_LOCAL_NAME_COMPLETE )
        {
          SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B );
        }
        if( eir_data_type_len <= BT_SD_MAX_DEVICE_NAME_LEN )
        {
          memcpy( (void *)dev_ptr->dev_public.name_str,
                  (void *)&eir_data[index],
                  eir_data_type_len );
          dev_ptr->dev_public.name_str[ eir_data_type_len ] = '\0';
        }
        else
        {
          /* Name longer than we can accept - just copy what we can*/
          memcpy( (void *)dev_ptr->dev_public.name_str,
                  (void *)&eir_data[index],
                  BT_SD_MAX_DEVICE_NAME_LEN-1 );
          dev_ptr->dev_public.name_str[ BT_SD_MAX_DEVICE_NAME_LEN ] = '\0';
          CLRBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B );
        }
        BT_MSG_DEBUG( "BT RM: Par EIR - Name: %x" ,eir_data_type_len, 0, 0 );
        break;

      case BT_EIR_TYPE_TX_POWER_LEVEL:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
        dev_ptr->eir_tx_power_received = TRUE;
        dev_ptr->eir_tx_power_level = eir_data[ index ];
        BT_MSG_DEBUG( "BT RM: Par EIR - TxPwr : %d" ,dev_ptr->eir_tx_power_level, 0, 0 );
        break;

      case BT_EIR_TYPE_MANUF_SPECIFIC_DATA:
        SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
        dev_ptr->dev_public.eir_data.manuf_data_size = eir_data_type_len;
        dev_ptr->eir_manuf_data_index = index;
        BT_MSG_DEBUG( "BT RM: Par EIR - MnfDt: %x" ,eir_data_type_len, 0, 0 );
        break;

      default:
        BT_MSG_DEBUG( "BT RM: Par EIR - UNKN " ,0, 0, 0 );
        break;
    }
    index = index + eir_data_type_len;
  }

  if( error == TRUE )
  {
    /* reset all data */
    dev_ptr->dev_public.eir_data.eir_flag = 0;
    dev_ptr->dev_public.eir_data.manuf_data_size  = 0;
    for(i = 0; i < BT_NUM_UUID_TYPES; i++)
    {
      dev_ptr->dev_public.eir_data.num_uuids[ i ] = 0;
    }
  }
  else 
  {
    if( index > 2 )
    { 
      /* Allocate memory */
      dev_ptr->ptr_eir_data = mem_malloc( &bt_rm_eir_heap, (size_t)(index - 2) );
      if( dev_ptr->ptr_eir_data != NULL )
      {
        memcpy( (void *)dev_ptr->ptr_eir_data, (void *)eir_data, (index - 2) );
      }
      else
      {
        /* We dont have enough memory */
        BT_MSG_DEBUG( "BT RM: Par EIR - No Memory" ,0, 0, 0 );

        /* Reset any received UUIDs or manuf data */
        // EIR_TODO: Check if eir_flag needs to cleared also (tx power or name could be received)
        dev_ptr->dev_public.eir_data.manuf_data_size  = 0;
        for(i = 0; i < BT_NUM_UUID_TYPES; i++)
        {
          dev_ptr->dev_public.eir_data.num_uuids[ i ] = 0;
        }
      }
    }
  }
  if ( dev_ptr->dev_public.valid != FALSE )
  {
    bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
  }
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_check_eir_uuid_list

DESCRIPTION
  Checks how much space there is for the UUID list in an outgoing EIR. If
  the space available is less the function returns the space available.

===========================================================================*/
void bt_rm_check_eir_uuid_list
(
  uint8     bytes_empty,
  uint8*    size,
  uint8     unit_size,
  boolean*  uuid_list_complete
)
{
  if( *size + BT_EIR_DATA_TYPE_HDR_LEN <= bytes_empty )
  {
    /* there is space for the complete list */
    return;
  }

  if( bytes_empty > BT_EIR_DATA_TYPE_HDR_LEN)
  {
    /* there is space for part of the list */
    *size = ( (bytes_empty - BT_EIR_DATA_TYPE_HDR_LEN)/unit_size ) * unit_size;
    *uuid_list_complete = FALSE;
  }
  else
  {
    /* there is no space */
    *size = 0;
    *uuid_list_complete = FALSE;
  }
  return;
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_append_eir_data

DESCRIPTION
  Fills in the EIR buffer

===========================================================================*/
uint8 bt_rm_append_eir_data
(
  uint8*  ptr_eir_packet,
  uint8   bytes_empty,
  uint8   data_type,
  uint8   data_size,
  uint8*  data_buffer
)
{
  uint8 index = 0;

  /* Compare length of eir entitity with bytes_empty
     if (length < bytes_to_fill)
        - fill eir_packet with eir entity
  */

  if( data_size != 0 &&
      (data_size + BT_EIR_DATA_TYPE_HDR_LEN) <= bytes_empty )
  {
    /* There is enough space*/
    ptr_eir_packet[index++] = data_size + 1;
    ptr_eir_packet[index++] = data_type;
    memcpy( (void *)&ptr_eir_packet[index],
            (void *)data_buffer,
            data_size );

    index += data_size;
    BT_MSG_DEBUG( "BT RM: Upd EIR - %x %u", data_type, data_size, 0 );
  }
  else
  {
    /* Not enough space - skip*/
    BT_MSG_DEBUG( "BT RM: Append EIR:  No data or No space - %x %u",
                   data_type, data_size, 0 );
  }

  return index;
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_update_eir

DESCRIPTION
  Updates the outgoing EIR and writes it to controller

===========================================================================*/
void bt_rm_update_eir
(
  void
)
{
  dsm_item_type*  dsm_ptr;
  uint8           eir_packet[ BT_MAX_EIR_HCI_LEN ];
  uint8           index;
  uint8           length_uuid;
  uint8           data_type;
  boolean         uuid_complete;
  uint8           fec_required;
  int8            tx_power;

  index = 0;

  BT_MSG_DEBUG( "BT RM: Update EIR" ,0, 0, 0 );

  /* Return if the SOC we are interfacing with does not support lisbon(2.1) */
  if( bt_soc_etc_data.lmp_version < BT_SOC_LMP_VERSION_2_1 )
  {
    BT_MSG_DEBUG( "BT RM: Update EIR SOC does not support feature" ,0, 0, 0 );
    return;
  }

#ifdef BT_SWDEV_RADIO_OFF
  /* Just return when the radio is off. The EIR will be updated when
     radio is switched on */
  if ( bt_dc.radio_state <= BT_RADIO_DISABLED )
  {
    BT_MSG_DEBUG( "BT RM: Update EIR - Radio off" ,0, 0, 0 );
    return;
  }
#endif

  /* Local Name */
  data_type = ( bt_rm_device_eir.is_name_complete ?
                BT_EIR_TYPE_LOCAL_NAME_COMPLETE : BT_EIR_TYPE_LOCAL_NAME_SHORTENED );
  index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_LEN - index),
                                  data_type, strlen( bt_efs_params.bt_short_name ),
                                  (uint8 *)bt_efs_params.bt_short_name );

  /* Manufacturer Specific Data */
  index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_HCI_LEN - index),
                                  BT_EIR_TYPE_MANUF_SPECIFIC_DATA,
                                  bt_efs_params.eir_manuf_data_size,
                                  (uint8 *)bt_efs_params.eir_manuf_data );

  /* 16-bit UUID */
  length_uuid = bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_16BIT_TYPE ];
  uuid_complete = bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_16BIT_TYPE ];
  bt_rm_check_eir_uuid_list( (BT_MAX_EIR_HCI_LEN - index), &length_uuid,
                              sizeof(bt_sd_uuid_type), &uuid_complete);
  data_type = ( uuid_complete ?
                BT_EIR_TYPE_UUID16_COMPLETE : BT_EIR_TYPE_UUID16_INCOMPLETE);
  index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_LEN - index),
                                  data_type, length_uuid,
                                  (uint8 *)bt_rm_device_eir.uuid_list.uuid16 );

   /* 32-bit UUIDs */
  length_uuid = bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_32BIT_TYPE ];
  uuid_complete = bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_32BIT_TYPE ];
  bt_rm_check_eir_uuid_list( (BT_MAX_EIR_HCI_LEN - index), &length_uuid,
                              sizeof(bt_sd_uuid32_type), &uuid_complete);
  data_type = ( uuid_complete ?
                BT_EIR_TYPE_UUID32_COMPLETE : BT_EIR_TYPE_UUID32_INCOMPLETE);
  index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_LEN - index),
                                  data_type, length_uuid,
                                  (uint8 *)bt_rm_device_eir.uuid_list.uuid32 );

  /* 128-bit UUIDs */
  length_uuid = bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_128BIT_TYPE ];
  uuid_complete = bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_128BIT_TYPE ];
  bt_rm_check_eir_uuid_list( (BT_MAX_EIR_HCI_LEN - index), &length_uuid,
                              sizeof(bt_sd_uuid128_type), &uuid_complete);
  data_type = ( uuid_complete ?
                BT_EIR_TYPE_UUID128_COMPLETE : BT_EIR_TYPE_UUID128_INCOMPLETE);
  index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_HCI_LEN - index),
                                  data_type, length_uuid,
                                  (uint8 *)bt_rm_device_eir.uuid_list.uuid128 );

  /* TX Power level */
  if( bt_rm_device_eir.tx_power_level_valid == TRUE )
  {
    tx_power = bt_rm_device_eir.tx_power_level - bt_rm_device_eir.tx_power_rfcal;
    index += bt_rm_append_eir_data( &eir_packet[index], (BT_MAX_EIR_LEN - index),
                                    BT_EIR_TYPE_TX_POWER_LEVEL,
                                    sizeof(bt_rm_device_eir.tx_power_level),
                                    (uint8 *)&tx_power );
  }

  BT_MSG_DEBUG( "BT RM: Upd EIR idx: %u", index, 0, 0 );

  /* Zero pad the rest of the packet
     EIR packets over HCI are always 240 bytes long */
  memset ( (void *)&eir_packet[index], 0, (BT_MAX_EIR_HCI_LEN - index) );

  /* Pack up eir_packet in a dsm and call the HCI write EIR function */
  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                 BT_MAX_EIR_HCI_LEN );

  if ( dsm_pushdown_tail( &dsm_ptr, (void*)(&eir_packet),
                          BT_MAX_EIR_HCI_LEN,
                          DSM_DS_SMALL_ITEM_POOL ) == BT_MAX_EIR_HCI_LEN )
  {
    fec_required = 1;
    bt_cmd_hc_wr_ext_inq_rsp(dsm_ptr, fec_required);
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Upd EIR DSM PD failed", 0, 0, 0 );
  }
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_update_eir_short_name

DESCRIPTION
  Updates the short name for the outgoing EIR

===========================================================================*/
void bt_rm_update_eir_short_name
(
  char*   short_name,
  boolean force_change
)
{
  if( (force_change == TRUE) ||
      (strlen( bt_efs_params.bt_short_name ) == 0) )
  {
    BT_STRCPY( (char *)bt_efs_params.bt_short_name,
               (char *)short_name,
               (BT_MAX_EIR_NAME_LEN + 1) );
  }

  if( !BT_STRCMP( bt_efs_params.bt_name, bt_efs_params.bt_short_name ) )
  {
    bt_rm_device_eir.is_name_complete = TRUE;
  }
  else
  {
    bt_rm_device_eir.is_name_complete = FALSE;
  }
}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_rm_connection_disconnected

DESCRIPTION
  Handles the disconnection of a specified connection.

===========================================================================*/
LOCAL void bt_rm_connection_disconnected
(
  bt_rm_conn_type*      conn_ptr,
  bt_link_type          link_type,
  bt_event_reason_type  reason
)
{

  bt_rm_ev_send_disconnected( conn_ptr, link_type, reason );

  bt_rm_init_link( conn_ptr, link_type );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_inquiry_complete

DESCRIPTION
  Sends a RM Inquiry Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_send_inquiry_complete
(
  bt_app_id_type  bt_app_id,
  uint8           num_responses
)
{

  bt_ev_msg_type  ev_rm_ic;

  ev_rm_ic.ev_hdr.ev_type                   = BT_EV_RM_INQUIRY_COMPLETE;
  ev_rm_ic.ev_hdr.bt_app_id                 = bt_app_id;
  ev_rm_ic.ev_msg.ev_rm_inqcp.num_responses = num_responses;

  BT_MSG_API( "BT RM EV TX: RM InqC AID %x N %x",
              ev_rm_ic.ev_hdr.bt_app_id,
              ev_rm_ic.ev_msg.ev_rm_inqcp.num_responses, 0 );

  bt_ec_send_event( &ev_rm_ic );

}


#ifdef FEATURE_BT_1_2
/*===========================================================================

FUNCTION
  bt_rm_ev_send_afh_ch_map_set

DESCRIPTION
  Sends a RM AFH Channel Map Set event.

===========================================================================*/
LOCAL void bt_rm_ev_send_afh_ch_map_set
(
  bt_app_id_type  app_id,
  uint8           status
)
{

  bt_ev_msg_type  ev_rm_acms;

  ev_rm_acms.ev_hdr.ev_type            = BT_EV_RM_AFH_CH_MAP_SET;
  ev_rm_acms.ev_hdr.bt_app_id          = app_id;
  ev_rm_acms.ev_msg.ev_rm_acms.success = ( status == BT_BE_SUCCESS );

  BT_MSG_API( "BT RM EV TX: RM AFH Ch Map Set AID %x Succ %x Stat %x",
              ev_rm_acms.ev_hdr.bt_app_id,
              ev_rm_acms.ev_msg.ev_rm_acms.success,
              status );

  bt_ec_send_event( &ev_rm_acms );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_afh_ch_map

DESCRIPTION
  Sends a RM AFH Channel Map event.

===========================================================================*/
LOCAL void bt_rm_ev_send_afh_ch_map
(
  bt_app_id_type                  app_id,
  bt_hc_rp_rd_afh_chan_map_type*  chan_map_ptr
)
{

  bt_ev_msg_type  ev_rm_acm;
  uint16          i;

  ev_rm_acm.ev_hdr.ev_type           = BT_EV_RM_AFH_CH_MAP;
  ev_rm_acm.ev_hdr.bt_app_id         = app_id;
  ev_rm_acm.ev_msg.ev_rm_acm.success = chan_map_ptr->status;

  for ( i = 0; i < 10; i++ )
  {
    ev_rm_acm.ev_msg.ev_rm_acm.ch_map.bytes[ i ] =
      chan_map_ptr->afh_chan_map[ i ];
  }

  BT_MSG_API( "BT RM EV TX: RM AFH Ch Map AID %x Succ %x",
              ev_rm_acm.ev_hdr.bt_app_id,
              ev_rm_acm.ev_msg.ev_rm_acm.success, 0 );

  bt_ec_send_event( &ev_rm_acm );

}
#endif /* FEATURE_BT_1_2 */


/*===========================================================================

FUNCTION
  bt_rm_disconnect_sco

DESCRIPTION
  Disconnects a specified SCO connection.

===========================================================================*/
LOCAL void bt_rm_disconnect_sco
(
  bt_rm_conn_type*        conn_ptr,
  bt_rm_disc_reason_type  reason
)
{

  conn_ptr->state_sco = BT_RM_CS_DISCONNECTING;

  BT_MSG_API( "BT RM CMD TX: HC DiscS H %x R %x",
              conn_ptr->hc_handle_sco, reason, 0 );
  bt_cmd_hc_disconnect( conn_ptr->hc_handle_sco, (uint8)(reason) );

}


/*===========================================================================

FUNCTION
  bt_rm_reset_idle_timeout

DESCRIPTION
  Resets the idle timeout for a specified connection.

===========================================================================*/
LOCAL void bt_rm_reset_idle_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  conn_ptr->idle_timeout_ms_left = conn_ptr->idle_timeout;
#ifdef BT_SWDEV_2_1_SSR
  /* idle_timeout takes precedence over sniff_retry_to */
  conn_ptr->sniff_retry_to_ms_left = 0;
#endif /* BT_SWDEV_2_1_SSR */

#ifndef FEATURE_BT_SOC
  {
    int8             rssi;

    rssi = bt_lm_get_rssi( &conn_ptr->dev_ptr->dev_public.bd_addr );
    if ( rssi <= 0 )  /*  Valid RSSI returned?  */
    {
      bt_rm_update_rssi( conn_ptr, rssi );
    }
  }
#endif /* !FEATURE_BT_SOC */

}


/*===========================================================================

FUNCTION
  bt_rm_update_device_lmp_info

DESCRIPTION
  Updates the LMP information of a specified device.

===========================================================================*/
LOCAL void bt_rm_update_device_lmp_info
(
  bt_rm_conn_type*  conn_ptr
)
{

#ifndef FEATURE_BT_SOC
  const bt_lm_attribute_type*  lm_attrib_ptr;
  bt_rm_device_type*           dev_ptr;
  uint8                        i;

  dev_ptr = conn_ptr->dev_ptr;

  if ( (lm_attrib_ptr =
          bt_lm_get_remote_attributes(
            &dev_ptr->dev_public.bd_addr )) != NULL )
  {

    for ( i = 0; i < sizeof( bt_lmp_features_type ); i++ )
    {
      if ( dev_ptr->dev_public.lmp_features.lmp_features_bytes[ i ] !=
             lm_attrib_ptr->lmp_features.lmp_features_bytes[ i ] )
      {
        dev_ptr->dev_public.lmp_features = lm_attrib_ptr->lmp_features;
        dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
        break;
      }
    }

    conn_ptr->remote_lmp_features = lm_attrib_ptr->lmp_features;

    if ( dev_ptr->dev_public.version.lmp_version !=
           lm_attrib_ptr->lmp_version )
    {
      dev_ptr->dev_public.version.lmp_version =
        lm_attrib_ptr->lmp_version;
      dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
    }

    if ( dev_ptr->dev_public.version.lmp_subversion !=
           lm_attrib_ptr->lmp_subversion )
    {
      dev_ptr->dev_public.version.lmp_subversion =
        lm_attrib_ptr->lmp_subversion;
      dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
    }

    if ( dev_ptr->dev_public.version.manufacturer_name !=
           lm_attrib_ptr->company_id )
    {
      dev_ptr->dev_public.version.manufacturer_name =
        lm_attrib_ptr->company_id;
      dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
    }
  }

  if ( (dev_ptr->dev_public.valid != FALSE) &&
       (dev_ptr->update_status != BT_RM_DUS_NONE) )
  {
    bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
  }

#endif /* FEATURE_BT_SOC */


}


/*===========================================================================

FUNCTION
  bt_rm_request_device_services

DESCRIPTION
  Requests Service information from a specified device.

RETURN VALUE
  TRUE if SDP Request is pending upon return, otherwise FALSE.

===========================================================================*/
LOCAL void bt_rm_request_device_services
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_sd_srch_pat_uuid_list_type     uuid_list;
  bt_sd_srch_pat_attr_id_list_type  attr_id_list;
  bt_cmd_status_type                cmd_status;

  uuid_list.num_uuid128 = 0;
  uuid_list.num_uuid    = 1;
  uuid_list.uuid[ 0 ]   = BT_SD_PROTOCOL_L2CAP;

  attr_id_list.num_attr_id           = 3;
  attr_id_list.attr_id[ 0 ].is_range = FALSE;
  attr_id_list.attr_id[ 0 ].value    =
    BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST;

  attr_id_list.attr_id[ 1 ].is_range = FALSE;
  attr_id_list.attr_id[ 1 ].value    =
    BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST;

  attr_id_list.attr_id[ 2 ].is_range = FALSE;
  attr_id_list.attr_id[ 2 ].value    =
    BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;

  BT_BDA( MSG_API, "BT RM CMD TX: SD Srch Srv",
          &conn_ptr->dev_ptr->dev_public.bd_addr );
  if ( (cmd_status = bt_cmd_sd_search_service(
          bt_rm_app_id,
          &conn_ptr->dev_ptr->dev_public.bd_addr,
          &uuid_list, &attr_id_list,
          BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT )) != BT_CS_GN_PENDING )
  {
    BT_ERR( "BT RM: Bad SD Srch Srv CS %x", cmd_status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad SD Srch Srv",
            &conn_ptr->dev_ptr->dev_public.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_finish_bond_attempt

DESCRIPTION
  If bonding was attempted on the specified connection this routine
  will send a RM Event indicating the outcome, namely either a Bonded
  or Bond Failed event.  It also clears the bond attempt information
  from the connection as this is the last step in that process.

===========================================================================*/
LOCAL void bt_rm_finish_bond_attempt
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_event_reason_type  reason;
  bt_rm_app_type*       app_ptr;
  bt_cmd_msg_type       cmd;

  /*  The condition check below must be mimiced exactly  */
  /*  within bt_rm_link_sec_re_pair_started().           */
  if ( conn_ptr->bonding_app_id != BT_APP_ID_NULL )
  {
    if ( (conn_ptr->bonding_key_rxd != FALSE) &&
         (conn_ptr->authenticated   != FALSE) &&
         (conn_ptr->bond_canceling != TRUE) )
    {
      bt_rm_ev_send_bonded( conn_ptr );

      if ( bt_rm_get_application(
             FALSE,  /*  Must be an existing application.  */
             conn_ptr->bonding_app_id,
             &app_ptr ) == BT_CS_GN_SUCCESS )
      {
        if (app_ptr->auto_service_search_disabled == FALSE)
        {
          bt_rm_request_device_services( conn_ptr );
        }
      }
      else
      {
        /* if app was not configured, request service info by default */
        bt_rm_request_device_services( conn_ptr );
      }
    }
    else
    {
      if( (conn_ptr->bond_canceling != FALSE) &&
          (conn_ptr->bonding_key_rxd != FALSE) )
      {
        /* We are bond canceling but link key has already been received,
           thus should unbond here */
        BT_MSG_DEBUG( "BT RM: Finish bond - unbond", 0, 0, 0 );

        cmd.cmd_hdr.cmd_type             = BT_CMD_RM_UNBOND;
        cmd.cmd_hdr.bt_app_id            = BT_APP_ID_NULL;
        cmd.cmd_msg.cmd_rm_unbnd.bd_addr =
          conn_ptr->dev_ptr->dev_public.bd_addr;

        bt_rm_cmd_unbond( &cmd );
        if ( cmd.cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT RM: Finish bond - Bad Unbond CS %x",
                  cmd.cmd_hdr.cmd_status, 0, 0 );
        }
      }
      conn_ptr->bond_canceling = FALSE;
      reason = BT_HCI_REASON(conn_ptr->hc_error);
      if(conn_ptr->bonding_key_rxd != FALSE)
      {
        conn_ptr->hc_error  = BT_BE_SUCCESS;
        conn_ptr->updates  &= ~BT_RM_LU_ERROR_VALID_B;
      }
      bt_rm_ev_send_bond_failed( conn_ptr->bonding_app_id,
                                 &conn_ptr->dev_ptr->dev_public.bd_addr,
                                 conn_ptr->rm_handle, reason );
      /* Initilize Pin Request parameters */
      conn_ptr->pin_req_pairing_done = TRUE;
      bt_rm_init_completed_pin_request( conn_ptr );
    }

    if ( conn_ptr->state_acl == BT_RM_CS_CONNECTING )
    {
      bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
    }
    else
    {
      bt_rm_reset_idle_timeout( conn_ptr );
    }

    conn_ptr->bonding_app_id  = BT_APP_ID_NULL;
    conn_ptr->bonding_key_rxd = FALSE;
#ifdef BT_SWDEV_2_1_SSP
    conn_ptr->dedicated_bonding = FALSE;
#endif /* BT_SWDEV_2_1_SSP */
  }

    memset( (void*)(&conn_ptr->bonding_pin), 0,
            sizeof( bt_pin_code_type ) );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_connection_req

DESCRIPTION
  Sends an RM Connection Request to a specified BT Application for
  a specified connection and link type.

===========================================================================*/
LOCAL void bt_rm_ev_send_connection_req
(
  bt_app_id_type    bt_app_id,
  bt_rm_conn_type*  conn_ptr,
  bt_link_type      link_type
)
{

  bt_ev_msg_type  ev_rm_cr;

  ev_rm_cr.ev_hdr.bt_app_id = bt_app_id;

  if ( link_type == BT_ACL_LINK )
  {
    ev_rm_cr.ev_hdr.ev_type = BT_EV_RM_CONNECT_REQ_ACL;

    ev_rm_cr.ev_msg.ev_rm_creqa.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;
    ev_rm_cr.ev_msg.ev_rm_creqa.class_of_device =
      conn_ptr->dev_ptr->dev_public.class_of_device;

    conn_ptr->acl_req_timeout_ms_left = BT_RM_REQ_TIMEOUT_MS;
  }
  else /*  SCO  */
  {
    ev_rm_cr.ev_hdr.ev_type = BT_EV_RM_CONNECT_REQ_SCO;

    ev_rm_cr.ev_msg.ev_rm_creqs.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;
    ev_rm_cr.ev_msg.ev_rm_creqs.class_of_device =
      conn_ptr->dev_ptr->dev_public.class_of_device;

    conn_ptr->sco_req_timeout_ms_left = BT_RM_REQ_TIMEOUT_MS;
  }

  BT_MSG_API( "BT RM EV TX: ConnR T %x AID %x",
              link_type, ev_rm_cr.ev_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM EV TX: ConnR",
          &conn_ptr->dev_ptr->dev_public.bd_addr );
  bt_ec_send_event( &ev_rm_cr );

}


/*===========================================================================

FUNCTION
  bt_rm_security_updated

DESCRIPTION
  Sets the value of the security field of a specified connection using
  its current states of authentication and encryption.

===========================================================================*/
LOCAL void bt_rm_security_updated
(
  bt_rm_conn_type*  conn_ptr
)
{

#ifdef BT_SWDEV_2_1_SSP
  bt_sec_level_type  cur_sec_level;

  if (( conn_ptr->authenticated != FALSE ) &&
      ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON ))
  {
    switch ( conn_ptr->dev_ptr->dev_public.link_key_status )
    {
      /* authenticated and bonded */
      case BT_RM_LKT_AUTH_GENERAL:
      case BT_RM_LKT_AUTH_DEDICATED:
      case BT_RM_LKT_AUTH_UNBONDED:
      case BT_RM_LKT_COMBINATION:
        cur_sec_level = BT_RM_SL_3_HIGH;
        break;

      /* bonded */
      case BT_RM_LKT_DEDICATED:
      case BT_RM_LKT_GENERAL:
        cur_sec_level = BT_RM_SL_2_MEDIUM;
        break;

      /* unauthenticated unbonded */
      case BT_RM_LKT_UNBONDED:
        cur_sec_level = BT_RM_SL_1_LOW;
        break;

      /* none */
      case BT_RM_LKT_NONE:
      case BT_RM_LKT_UNIT:
        cur_sec_level = BT_RM_SL_0_NONE;
        break;

      case BT_RM_LKT_DEBUG:
        cur_sec_level = BT_RM_SL_3_HIGH;
        break;

      default:
        cur_sec_level = BT_RM_SL_0_NONE;
        BT_ERR("BT RM: SSP Invalid LKS %x ",
               conn_ptr->dev_ptr->dev_public.link_key_status,0,0);
    }
  }
  else if ( ( conn_ptr->authenticated != FALSE ) &&
            (conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE) )
  {
    /*
     * Though the authentication is done the encr is not enabled till now
     * and the remote device is not ssp capable
     */
    cur_sec_level = BT_RM_SL_1_LOW;
  }
  else
  {
    cur_sec_level = BT_RM_SL_0_NONE;
  }

  BT_MSG_DEBUG( "BT RM: SSP SEC Update H %x EE %x A %x",
                conn_ptr->rm_handle, conn_ptr->encrypt_enable,
                conn_ptr->authenticated );
  BT_MSG_DEBUG( "BT RM: SSP SEC Update EM %x CurrSL %x NewSL %x",
                conn_ptr->encrypt_mode,
                conn_ptr->sec_level,
                cur_sec_level );

  if ( conn_ptr->sec_level < conn_ptr->sec_level_request )
  {
    BT_MSG_DEBUG( "BT RM: SSP SEC pending SL %x SLR %x RH %x",
                  conn_ptr->sec_level, conn_ptr->sec_level_request,
                  conn_ptr->rm_handle );
  }

  if ( conn_ptr->sec_level != cur_sec_level )
  {
    conn_ptr->sec_level  = cur_sec_level;
    conn_ptr->updates  |= BT_RM_LU_SECURITY_B;
  }
#else
  bt_security_type  cur_security;

  if ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON )
  {
    cur_security = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
  }
  else if ( conn_ptr->authenticated != FALSE )
  {
    cur_security = BT_SEC_AUTHENTICATE;
  }
  else
  {
    cur_security = BT_SEC_NONE;
  }

  BT_MSG_DEBUG( "BT RM: SEC Update H %x EE %x A %x",
                conn_ptr->rm_handle, conn_ptr->encrypt_enable,
                conn_ptr->authenticated );
  BT_MSG_DEBUG( "BT RM: SEC Update EM %x PS %x CS %x",
                conn_ptr->encrypt_mode, conn_ptr->security,
                cur_security );

  if ( conn_ptr->security < conn_ptr->security_request )
  {
    BT_MSG_DEBUG( "BT RM: SEC pending S %x SR %x RH %x",
                  conn_ptr->security, conn_ptr->security_request,
                  conn_ptr->rm_handle );
  }

  if ( conn_ptr->security != cur_security )
  {
    conn_ptr->security  = cur_security;
    conn_ptr->updates  |= BT_RM_LU_SECURITY_B;
  }
#endif /* BT_SWDEV_2_1_SSP */
}


/*===========================================================================

FUNCTION
  bt_rm_authorize_connection

DESCRIPTION
  Determines if the current authorization of a specified connection
  meets the minimum desired.  If current level of authorization is
  insufficient, the process of authorizing the link is started.

RETURN VALUES
  TRUE   If the current level of authorization meets the minimum desired
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_authorize_connection
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean         authorized_enough = TRUE;
  bt_ev_msg_type  ev_rm_ar;

  if ( (conn_ptr->authorized == FALSE) &&
       (conn_ptr->authorization_required != FALSE) )
  {
    /*  Authorization necessary now.  */
    authorized_enough = FALSE;

    ev_rm_ar.ev_hdr.ev_type   = BT_EV_RM_AUTHORIZE_REQUEST;
    ev_rm_ar.ev_hdr.bt_app_id = conn_ptr->authorize_req_app_id;

    ev_rm_ar.ev_msg.ev_rm_atzrq.service_id =
      conn_ptr->sec_res_ss_service_id;
    ev_rm_ar.ev_msg.ev_rm_atzrq.bd_addr =
      conn_ptr->dev_ptr->dev_public.bd_addr;
    if( (conn_ptr->authorize_dir == BT_RM_ATZRQ_INCOMING) ||
        (conn_ptr->authorize_dir == BT_RM_ATZRQ_OUTGOING) )
    {
        ev_rm_ar.ev_msg.ev_rm_atzrq.authorize_dir =
            conn_ptr->authorize_dir;
    }
    else
    {
        BT_ERR("BT RM: Authorize direction is not correct",0,0,0);
    }


    BT_MSG_API( "BT RM EV TX: AtzReq SCN %x AID %x Dir %d",
                ev_rm_ar.ev_msg.ev_rm_atzrq.service_id.rfcomm_scn,
                ev_rm_ar.ev_hdr.bt_app_id, conn_ptr->authorize_dir );
    BT_BDA( MSG_API, "BT RM EV TX: AtzReq",
            &ev_rm_ar.ev_msg.ev_rm_atzrq.bd_addr );
    bt_ec_send_event( &ev_rm_ar );
  }

  return ( authorized_enough );

}


/*===========================================================================

FUNCTION
  bt_rm_secure_connection

DESCRIPTION
  Determines if the current security level of a specified connection
  meets the minimum desired.  If the current level of security is
  insufficient, the process of upgrading the security of the link is
  started.

RETURN VALUES
  TRUE   If the current level of security meets the minimum desired
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_secure_connection
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean  secure_enough = TRUE;

#ifdef BT_SWDEV_2_1_SSP
  BT_MSG_HIGH( "BT RM: SSP Secure Connection CSL %x CSLM %x",
              conn_ptr->sec_level,
              conn_ptr->sec_level_min, 0 );

  if ( conn_ptr->sec_level < conn_ptr->sec_level_min )
  {
    secure_enough = FALSE;
    if ((conn_ptr->encrypt_enable!=TRUE) &&
        (conn_ptr->authenticated==TRUE) &&
        (!IS_SSP_DOABLE()))
    {
      /* Remote is not 2.1 capable and current link is only
         authenticated. */
      conn_ptr->enh_enc_state = BT_RM_ENH_ENC_STARTED;
      bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                                  BT_ENCRYPT_ENABLE_ON );
      conn_ptr->sec_level_request = conn_ptr->sec_level_min;
    }
    else
    {
      conn_ptr->enh_enc_state = BT_RM_ENH_ENC_AUTHENTICATING;
      BT_MSG_HIGH("BT RM CMD TX: HC SSP AuthReq",0,0,0);
      bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
      conn_ptr->pairing_initiated = TRUE;
      conn_ptr->sec_level_request = conn_ptr->sec_level_min;
    }
  }
#else
  if ( conn_ptr->security < conn_ptr->security_min )
  {
    secure_enough = FALSE;

    if ( ( conn_ptr->security_min == BT_SEC_AUTHENTICATE_AND_ENCRYPT ) &&
         ( conn_ptr->authenticated != FALSE ) )
    {
      bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                                  BT_ENCRYPT_ENABLE_ON );
    }
    else if ( ( conn_ptr->security_min == BT_SEC_AUTHENTICATE ) ||
              ( ( conn_ptr->security_min == BT_SEC_AUTHENTICATE_AND_ENCRYPT ) &&
                ( conn_ptr->authenticated == FALSE ) ) )
    {
      bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
    }

    conn_ptr->security_request = conn_ptr->security_min;
  }
#endif /* BT_SWDEV_2_1_SSP */

  return ( secure_enough );

}


/*===========================================================================

FUNCTION
  bt_rm_connection_secure_enough

DESCRIPTION
  Determines if the current security level of a specified connection
  meets the minimum desired.  If specified and the current level of
  security is insufficient, the process of upgrading the security of
  the link is started.

RETURN VALUES
  TRUE   If the current level of security meets the minimum desired
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_connection_secure_enough
(
  bt_rm_conn_type*  conn_ptr,
  boolean           req_sec_upgrade
)
{

  boolean  secure_enough = TRUE;

#ifdef BT_SWDEV_2_1_SSP
  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough Dev2.1 %x AZF %x",
               conn_ptr->dev_ptr->dev_public.ssp_capable,
               conn_ptr->authorize_first, 0 );

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough SL %x, SLMIN %x, AZ %x",
               conn_ptr->sec_level, conn_ptr->sec_level_min,
               conn_ptr->authorized);

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough AZR %x, LKPINL %x, SECMINPINL %x",
               conn_ptr->authorization_required,
               conn_ptr->dev_ptr->link_key_pin_len_bytes,
               conn_ptr->security_min_pin_len_bytes);

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough LTYPE %x, EKLEN %x, SECMINENCL %x",
               conn_ptr->dev_ptr->dev_public.link_key_type,
               conn_ptr->enc_key_len_bytes,
               conn_ptr->security_min_enc_key_len_bytes);


  if ( (( IS_SSP_DOABLE() ) &&
        ( conn_ptr->authorize_dir == BT_RM_ATZRQ_INCOMING )   &&
        ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_OFF ) &&
        ( conn_ptr->sec_level_min != BT_RM_SL_0_NONE ) )       ||
       ((conn_ptr->sec_level < conn_ptr->sec_level_min) &&
        ( ( IS_SSP_DOABLE() ) ||
          (conn_ptr->sec_level_min > BT_RM_SL_1_LOW ) ))  ||
       ((conn_ptr->authorized == FALSE) &&
        (conn_ptr->authorization_required != FALSE)) ||
       ((conn_ptr->sec_level_min != BT_RM_SL_0_NONE ) &&
        (conn_ptr->dev_ptr->link_key_pin_len_bytes <
           conn_ptr->security_min_pin_len_bytes)) ||
       ((conn_ptr->sec_level != BT_RM_SL_0_NONE) &&
        (((conn_ptr->dev_ptr->dev_public.link_key_type ==
             BT_HC_LINK_KEY_LOCAL) ||
           (conn_ptr->dev_ptr->dev_public.link_key_type ==
             BT_HC_LINK_KEY_REMOTE)) ||
         (conn_ptr->enc_key_len_bytes <
            conn_ptr->security_min_enc_key_len_bytes))) )
#else

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough SEC %x, SECMIN %x, AZ %x",
               conn_ptr->security, conn_ptr->security_min,
               conn_ptr->authorized);

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough AZR %x, LKPINL %x, SECMINPINL %x",
               conn_ptr->authorization_required,
               conn_ptr->dev_ptr->link_key_pin_len_bytes,
               conn_ptr->security_min_pin_len_bytes);

  BT_MSG_HIGH( "BT RM: SSP Connection Secure Enough LTYPE %x, EKLEN %x, SECMINENCL %x",
               conn_ptr->dev_ptr->dev_public.link_key_type,
               conn_ptr->enc_key_len_bytes,
               conn_ptr->security_min_enc_key_len_bytes);

  if ( (conn_ptr->security < conn_ptr->security_min) ||
       ((conn_ptr->authorized == FALSE) &&
        (conn_ptr->authorization_required != FALSE)) ||
       (((conn_ptr->security == BT_SEC_AUTHENTICATE) ||
         (conn_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT)) &&
        (conn_ptr->dev_ptr->link_key_pin_len_bytes <
           conn_ptr->security_min_pin_len_bytes)) ||
       ((conn_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT) &&
        (((conn_ptr->security_comb_key_only != FALSE) &&
          (conn_ptr->dev_ptr->dev_public.link_key_type !=
             BT_HC_LINK_KEY_COMB)) ||
         (conn_ptr->enc_key_len_bytes <
            conn_ptr->security_min_enc_key_len_bytes))) )
#endif /* BT_SWDEV_2_1_SSP */
  {
    secure_enough = FALSE;

    if ( req_sec_upgrade != FALSE )
    {
      if ( conn_ptr->authorize_first != FALSE )
      {
        if ( bt_rm_authorize_connection( conn_ptr ) != FALSE )
        {
          /*  Already authorized.  Must need securing.  */
          if ( bt_rm_secure_connection( conn_ptr ) != FALSE )
          {
            BT_ERR( "BT RM: Sec Conn inconsistency.", 0, 0, 0 );
          }
        }
      }
      else
      {
        if ( bt_rm_secure_connection( conn_ptr ) != FALSE )
        {
          /*  Already secure.  Must need authorization.  */
          if ( bt_rm_authorize_connection( conn_ptr ) != FALSE )
          {
            BT_ERR( "BT RM: Atz Conn inconsistency.", 0, 0, 0 );
          }
        }
      }
    }
  }

  BT_MSG_API( "BT RM: Connection Secure Enough Result %x",
              secure_enough, 0, 0 );

  return ( secure_enough );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_rfcomm_scn_sec_result

DESCRIPTION
  Sends a RM RFCOMM SCN Security Result event as specified.

===========================================================================*/
LOCAL void bt_rm_ev_send_rfcomm_scn_sec_result
(
  bt_app_id_type    bt_app_id,
  uint8             rfcomm_scn,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           secure_enough
)
{

  bt_ev_msg_type  ev_rm_rsr;

  ev_rm_rsr.ev_hdr.ev_type   = BT_EV_RM_RFCOMM_SCN_SEC_RESULT;
  ev_rm_rsr.ev_hdr.bt_app_id = bt_app_id;

  ev_rm_rsr.ev_msg.ev_rm_srscn.rfcomm_scn    = rfcomm_scn;
  ev_rm_rsr.ev_msg.ev_rm_srscn.bd_addr       = *bd_addr_ptr;
  ev_rm_rsr.ev_msg.ev_rm_srscn.secure_enough = secure_enough;

  BT_MSG_HIGH( "BT RM: SCN Sec Res %x SCN %x",
               secure_enough, rfcomm_scn, 0 );
  BT_BDA( MSG_HIGH, "BT RM: SCN Sec Res", bd_addr_ptr );


  BT_MSG_API( "BT RM EV TX: SCN Sec Res %x SCN %x",
               ev_rm_rsr.ev_msg.ev_rm_srscn.secure_enough,
               ev_rm_rsr.ev_msg.ev_rm_srscn.rfcomm_scn, 0 );
  BT_BDA( MSG_API, "BT RM EV TX: SCN Sec Res",
          &ev_rm_rsr.ev_msg.ev_rm_srscn.bd_addr );
  bt_ec_send_event( &ev_rm_rsr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_l2cap_psm_sec_result

DESCRIPTION
  Sends a RM L2CAP PSM Security Result event as specified.

===========================================================================*/
LOCAL void bt_rm_ev_send_l2cap_psm_sec_result
(
  bt_app_id_type    bt_app_id,
  uint16            l2cap_psm,
  uint16            l2cap_cid,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean           secure_enough
)
{

  bt_ev_msg_type  ev_rm_lsr;

  ev_rm_lsr.ev_hdr.ev_type   = BT_EV_RM_L2CAP_PSM_SEC_RESULT;
  ev_rm_lsr.ev_hdr.bt_app_id = bt_app_id;

  ev_rm_lsr.ev_msg.ev_rm_srpsm.l2cap_psm     = l2cap_psm;
  ev_rm_lsr.ev_msg.ev_rm_srpsm.l2cap_cid     = l2cap_cid;
  ev_rm_lsr.ev_msg.ev_rm_srpsm.bd_addr       = *bd_addr_ptr;
  ev_rm_lsr.ev_msg.ev_rm_srpsm.secure_enough = secure_enough;

  BT_MSG_HIGH( "BT RM: PSM Sec Res %x PSM %x CID %x",
               secure_enough, l2cap_psm, l2cap_cid );
  BT_BDA( MSG_HIGH, "BT RM: PSM Sec Res", bd_addr_ptr );

  BT_MSG_API( "BT RM EV TX: PSM Sec Res %x PSM %x CID %x",
              ev_rm_lsr.ev_msg.ev_rm_srpsm.secure_enough,
              ev_rm_lsr.ev_msg.ev_rm_srpsm.l2cap_psm,
              ev_rm_lsr.ev_msg.ev_rm_srpsm.l2cap_cid );
  BT_BDA( MSG_API, "BT RM EV TX: PSM Sec Res", bd_addr_ptr );
  bt_ec_send_event( &ev_rm_lsr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_serv_sec_result

DESCRIPTION
  Sends a RM Service Security Result event as specified.

===========================================================================*/
LOCAL void bt_rm_ev_send_serv_sec_result
(
  bt_rm_conn_type*  conn_ptr,
  boolean           secure_enough
)
{

  bt_ev_msg_type  ev_rm_ssr;

  ev_rm_ssr.ev_hdr.ev_type   = BT_EV_RM_SERVICE_SEC_RESULT;
  ev_rm_ssr.ev_hdr.bt_app_id = conn_ptr->sec_res_ss_app_id;

  ev_rm_ssr.ev_msg.ev_rm_ssres.service_id = conn_ptr->sec_res_ss_service_id;
  ev_rm_ssr.ev_msg.ev_rm_ssres.bd_addr =
    conn_ptr->dev_ptr->dev_public.bd_addr;

  ev_rm_ssr.ev_msg.ev_rm_ssres.secure_enough = secure_enough;

  ev_rm_ssr.ev_msg.ev_rm_ssres.authorize  = conn_ptr->authorization_required;
  ev_rm_ssr.ev_msg.ev_rm_ssres.authorized = conn_ptr->authorized;

#ifdef BT_SWDEV_2_1_SSP
  ev_rm_ssr.ev_msg.ev_rm_ssres.min_sec_level = conn_ptr->sec_level_min;
  ev_rm_ssr.ev_msg.ev_rm_ssres.sec_level     = conn_ptr->sec_level;
#else
  ev_rm_ssr.ev_msg.ev_rm_ssres.min_security = conn_ptr->security_min;
  ev_rm_ssr.ev_msg.ev_rm_ssres.security     = conn_ptr->security;
#endif /* BT_SWDEV_2_1_SSP */

  ev_rm_ssr.ev_msg.ev_rm_ssres.min_pin_len_bytes =
    conn_ptr->security_min_pin_len_bytes;

  if ( secure_enough != FALSE )
  {
    ev_rm_ssr.ev_msg.ev_rm_ssres.pin_len_bytes =
      conn_ptr->dev_ptr->link_key_pin_len_bytes;
  }
  else
  {
    BT_MSG_HIGH( "BT RM: Neg SSec Res MPLB %x PRLB %x CPLB %x",
                 conn_ptr->security_min_pin_len_bytes,
                 conn_ptr->pin_code_reply_len_bytes,
                 conn_ptr->dev_ptr->link_key_pin_len_bytes );

    ev_rm_ssr.ev_msg.ev_rm_ssres.pin_len_bytes =
      conn_ptr->pin_code_reply_len_bytes;
  }
  conn_ptr->pin_code_reply_len_bytes = 0;

  ev_rm_ssr.ev_msg.ev_rm_ssres.comb_link_key_only =
    conn_ptr->security_comb_key_only;
  ev_rm_ssr.ev_msg.ev_rm_ssres.link_key_type =
    conn_ptr->dev_ptr->dev_public.link_key_type;

  ev_rm_ssr.ev_msg.ev_rm_ssres.min_enc_key_len_bytes =
    conn_ptr->security_min_enc_key_len_bytes;
  ev_rm_ssr.ev_msg.ev_rm_ssres.enc_key_len_bytes =
    conn_ptr->enc_key_len_bytes;

#ifdef BT_SWDEV_2_1_SSP
  BT_MSG_HIGH( "BT RM: SSec Res %x AZR/AZD %04x MinSec/Sec %04x",
               secure_enough,
               (uint16)((conn_ptr->authorization_required << 8) |
                         conn_ptr->authorized),
               (uint16)((conn_ptr->sec_level_min << 8) |
                         conn_ptr->sec_level) );
#else
  BT_MSG_HIGH( "BT RM: SSP SSec Res %x AZR/AZD %04x MinSec/Sec %04x",
               secure_enough,
               (uint16)((conn_ptr->authorization_required << 8) |
                         conn_ptr->authorized),
               (uint16)((conn_ptr->security_min << 8) |
                         conn_ptr->security) );
#endif /* BT_SWDEV_2_1_SSP */
  BT_MSG_HIGH( "BT RM: SSec Res MPLB/PLB %04x CLKO/LKT %04x MEKL/EKL %04x",
               (uint16)((conn_ptr->security_min_pin_len_bytes << 8) |
                         conn_ptr->dev_ptr->link_key_pin_len_bytes),
               (uint16)((conn_ptr->security_comb_key_only << 8) |
                         conn_ptr->dev_ptr->dev_public.link_key_type),
               (uint16)((conn_ptr->security_min_enc_key_len_bytes << 8) |
                         conn_ptr->enc_key_len_bytes) );
  BT_BDA( MSG_HIGH, "BT RM: SSec Res",
                    &conn_ptr->dev_ptr->dev_public.bd_addr );

#ifdef BT_SWDEV_2_1_SSP
  BT_MSG_API( "BT RM EV TX: SSec Res %x AZR/AZD %04x MinSec/Sec %04x",
              ev_rm_ssr.ev_msg.ev_rm_ssres.secure_enough,
              (uint16)((ev_rm_ssr.ev_msg.ev_rm_ssres.authorize << 8) |
                        ev_rm_ssr.ev_msg.ev_rm_ssres.authorized),
              (uint16)((ev_rm_ssr.ev_msg.ev_rm_ssres.min_sec_level << 8) |
                        ev_rm_ssr.ev_msg.ev_rm_ssres.sec_level) );
#else
  BT_MSG_API( "BT RM EV TX: SSec Res %x AZR/AZD %04x MinSec/Sec %04x",
              ev_rm_ssr.ev_msg.ev_rm_ssres.secure_enough,
              (uint16)((ev_rm_ssr.ev_msg.ev_rm_ssres.authorize << 8) |
                        ev_rm_ssr.ev_msg.ev_rm_ssres.authorized),
              (uint16)((ev_rm_ssr.ev_msg.ev_rm_ssres.min_security << 8) |
                        ev_rm_ssr.ev_msg.ev_rm_ssres.security) );
#endif /* BT_SWDEV_2_1_SSP */
  BT_MSG_API( "BT RM EV TX: SSec Res MPLB %x PLB %x CLKO/LKT %04x",
              ev_rm_ssr.ev_msg.ev_rm_ssres.min_pin_len_bytes,
              ev_rm_ssr.ev_msg.ev_rm_ssres.pin_len_bytes,
              (uint16)((ev_rm_ssr.ev_msg.ev_rm_ssres.
                          comb_link_key_only << 8) |
                        ev_rm_ssr.ev_msg.ev_rm_ssres.link_key_type) );
  BT_MSG_API( "BT RM EV TX: SSec Res MEKL %x EKL %x AID %x",
              ev_rm_ssr.ev_msg.ev_rm_ssres.min_enc_key_len_bytes,
              ev_rm_ssr.ev_msg.ev_rm_ssres.enc_key_len_bytes,
              ev_rm_ssr.ev_hdr.bt_app_id );
  BT_BDA( MSG_API, "BT RM EV TX: SSec Res",
          &ev_rm_ssr.ev_msg.ev_rm_ssres.bd_addr );

  bt_ec_send_event( &ev_rm_ssr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_send_security_result

DESCRIPTION
  Sends the appropriate RM Security Result event for a specified
  connection.

===========================================================================*/
LOCAL void bt_rm_ev_send_security_result
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean  secure_enough;

  if ( (conn_ptr->sec_res_enf_app_id != BT_APP_ID_NULL) &&
       ((conn_ptr->sec_res_rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED) ||
        (conn_ptr->sec_res_l2cap_psm != BT_L2_PSM_INVALID)) )
  {
    secure_enough = bt_rm_connection_secure_enough( conn_ptr, FALSE );

    if ( conn_ptr->sec_res_rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED )
    {
      /*  Send RFCOMM SCN Security Result event.  */
      bt_rm_ev_send_rfcomm_scn_sec_result(
        conn_ptr->sec_res_enf_app_id,
        conn_ptr->sec_res_rfcomm_scn,
        &conn_ptr->dev_ptr->dev_public.bd_addr,
        secure_enough );
    }
    else
    {
      /*  Send L2CAP PSM Security Result event.  */
      bt_rm_ev_send_l2cap_psm_sec_result(
        conn_ptr->sec_res_enf_app_id,
        conn_ptr->sec_res_l2cap_psm,
        conn_ptr->sec_res_l2cap_cid,
        &conn_ptr->dev_ptr->dev_public.bd_addr,
        secure_enough );
    }

    if ( conn_ptr->sec_res_ss_app_id != BT_APP_ID_NULL )
    {
      /*  Send Service Security Result event.  */
      bt_rm_ev_send_serv_sec_result( conn_ptr, secure_enough );
    }

    conn_ptr->authorized             = FALSE;
    conn_ptr->authorization_required = FALSE;
    conn_ptr->authorize_first        = FALSE;
    conn_ptr->authorize_req_app_id   = BT_APP_ID_NULL;

    conn_ptr->security_min_pin_len_bytes     = 0;
    conn_ptr->security_comb_key_only         = FALSE;
    conn_ptr->security_min_enc_key_len_bytes = 0;
    conn_ptr->re_pair_pin_state              = BT_RM_RE_PAIR_PIN_NONE;
#ifdef BT_SWDEV_2_1_SSP
    conn_ptr->sec_level_min                  = BT_RM_SL_0_NONE;
#endif

    bt_rm_init_conn_sec_results( conn_ptr );
  }
  else
  {
    BT_ERR( "BT RM: Bad Sec Res AID %x SCN %x PSM %x",
            conn_ptr->sec_res_enf_app_id,
            conn_ptr->sec_res_rfcomm_scn,
            conn_ptr->sec_res_l2cap_psm );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_finish_service_security

DESCRIPTION
  Finishes service security as necessary on a specified connection.
  This is called after connection security enhancement results have
  been obtained (either Authentication and/or Encryption).  If they
  were not successful, security results are sent indicating this.
  If they were successful, authorization is initiated if necessary,
  or successful security results are sent.

===========================================================================*/
LOCAL void bt_rm_finish_service_security
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->sec_res_enf_app_id != BT_APP_ID_NULL )
  {
#ifdef BT_SWDEV_2_1_SSP
    /* TODO: Does this need to take into account if the remote device
     * is non SSP? */
    if ( ( conn_ptr->sec_level >= conn_ptr->sec_level_min ) &&
         (conn_ptr->dev_ptr->link_key_pin_len_bytes >=
                 conn_ptr->security_min_pin_len_bytes) )
#else
    if ( ( conn_ptr->security >= conn_ptr->security_min ) &&
         (conn_ptr->dev_ptr->link_key_pin_len_bytes >=
                 conn_ptr->security_min_pin_len_bytes) )
#endif /* BT_SWDEV_2_1_SSP */
    {
      /*  Connection security has been obtained.  */
      /*  Authorize connection as necessary.      */
      if ( bt_rm_authorize_connection( conn_ptr ) != FALSE )
      {
        /*  Authorization level OK.          */
        /*  Send Security Result event now.  */
        bt_rm_ev_send_security_result( conn_ptr );
      }
    }
    else
    {
      /*  Connection security not sufficient.  */
      /*  Send Security Result event now.      */
      bt_rm_ev_send_security_result( conn_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_update_conn_hc_error

DESCRIPTION
  Updates the HCI error field of a specified connection using a
  specified HCI status value.

===========================================================================*/
LOCAL void bt_rm_update_conn_hc_error
(
  bt_rm_conn_type*    conn_ptr,
  bt_error_code_type  hc_status
)
{

  if ( hc_status != BT_BE_SUCCESS )
  {
    if ( (conn_ptr->hc_error != BT_BE_SUCCESS) ||
         (conn_ptr->updates & BT_RM_LU_ERROR_VALID_B) )
    {
      BT_ERR( "BT RM: Lost HC Err %x LU %x",
              conn_ptr->hc_error, conn_ptr->updates, 0 );
      BT_BDA( ERR, "BT RM: Lost HC Err",
              &conn_ptr->dev_ptr->dev_public.bd_addr );
    }

    conn_ptr->hc_error  = (bt_error_code_type)BT_HCI_REASON( hc_status );
    conn_ptr->updates  |= BT_RM_LU_ERROR_VALID_B;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_finish_remote_name_request

DESCRIPTION
  Finishes a Remote Name Request.

===========================================================================*/
LOCAL void bt_rm_finish_remote_name_request
(
  bt_rm_conn_type*    conn_ptr,
  bt_cmd_status_type  status
)
{

  if( conn_ptr->rname_app_id != bt_rm_app_id )
  {
    bt_rm_ev_send_remote_name( conn_ptr->rname_app_id,
                               &conn_ptr->dev_ptr->dev_public,
                               status );

    if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTING)   &&
         (conn_ptr->state_sco == BT_RM_CS_DISCONNECTED) &&
         (conn_ptr->acl_resps_pending == 0)             &&
         (conn_ptr->pin_req_resps_pending == 0) )
    {
      /*  Connection entry exists only for this response.  */
      bt_rm_init_link( conn_ptr, BT_ACL_LINK );
    }
    else
    {
      bt_rm_reset_idle_timeout( conn_ptr );
    }
  }
  conn_ptr->rname_app_id = BT_APP_ID_NULL;

}


/*===========================================================================

FUNCTION
  bt_rm_check_park_link_policy

DESCRIPTION
  Updates the link policy of a specified connection based on the
  current role in the connection.  Park Mode is only enabled when
  the current role is slave.  Optionally an HCI Write Link Policy
  command is sent if the link policy value has changed.

===========================================================================*/
LOCAL void bt_rm_check_park_link_policy
(
  bt_rm_conn_type*  conn_ptr,
  boolean           update_hci
)
{

  uint16  prev_link_policy;

  prev_link_policy = conn_ptr->link_policy;

  if ( conn_ptr->role == BT_ROLE_SLAVE )
  {
    /*  Enable Park Mode when a slave.  */
    conn_ptr->link_policy |= BT_LINK_POLICY_PARK_MODE_B;
  }
  else
  {
    /*  Disable Park Mode when a master.  */
    conn_ptr->link_policy &= ~BT_LINK_POLICY_PARK_MODE_B;
  }

  if ( (update_hci != FALSE) &&
       (conn_ptr->link_policy != prev_link_policy) )

  {
    BT_MSG_API( "BT RM CMD TX: HC Wr LP %x H %x RH %x",
                conn_ptr->link_policy, conn_ptr->hc_handle_acl,
                conn_ptr->rm_handle );
    bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                              conn_ptr->link_policy );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_finish_role_switch

DESCRIPTION
  Finishes a role switch.

===========================================================================*/
LOCAL void bt_rm_finish_role_switch
(
  bt_rm_conn_type*    conn_ptr,
  bt_role_type        cur_role,
  bt_error_code_type  hc_status
)
{

  if ( conn_ptr->role_request != BT_ROLE_NONE )
  {
    bt_rm_update_conn_hc_error( conn_ptr, hc_status );

    conn_ptr->role_request = BT_ROLE_NONE;
  }

  conn_ptr->updates |= BT_RM_LU_ROLE_B;
  if ( conn_ptr->role != cur_role )
  {
    conn_ptr->role     = cur_role;

    if ( (conn_ptr->state_acl != BT_RM_CS_DISCONNECTED) &&
         (conn_ptr->state_acl != BT_RM_CS_CONNECTING)
       )
    {
      /* Update link policy/scan setting only if role change is being
         processed after connection complete.
      */
      bt_rm_check_park_link_policy( conn_ptr, TRUE );
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
    /* Role changed. Check if scanning needs enabling/disabling */
      bt_rm_check_scan_settings();
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
    }
  }

  bt_rm_link_status_updated( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_check_idle_link_mode

DESCRIPTION
  Sets the idle link mode of a specified connection using all applicable
  current low power configurations.

NOTE
  This function is deprecated and replaced with bt_rm_set_connection_lpc().

===========================================================================*/
LOCAL void bt_rm_check_idle_link_mode
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16                     i;
  bt_rm_low_power_cfg_type*  lpc_ptr;
  uint16                     best_idle_timeout = 0;
  bt_link_mode_type          best_idle_link_mode = BT_LM_NONE;
  uint16                     best_idle_max_interval = 0;
  bt_link_mode_type          cur_idle_link_mode;

  for ( i = 0; i < BT_RM_MAX_LPCS; i++ )
  {
    lpc_ptr = &bt_rm_lpc[ i ];
    if ( lpc_ptr->conn_ptr == conn_ptr )
    {
      if ( (lpc_ptr->idle_timeout != 0) &&
           ((lpc_ptr->low_power_ctrl == BT_LPC_SNIFF) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_PARK) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_HOLD) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_SNIFF_LOCK)) )
      {
        /*  Entry meant to configure idle low power mode.  */

        switch ( lpc_ptr->low_power_ctrl )
        {
          case BT_LPC_SNIFF_LOCK:
            /*
             * This option allows sending ACL data while in sniff.
             * Device will not be brought out of sniff when ACL data is to be
             * transmitted during sniff (using bt_rm_cmd_tx_data_acl).
             */
            {
              cur_idle_link_mode = BT_LM_SNIFF_LOCK;
              break;
            }
          case BT_LPC_SNIFF:
            {
              cur_idle_link_mode = BT_LM_SNIFF;
              break;
            }
          case BT_LPC_PARK:
            {
              cur_idle_link_mode = BT_LM_PARK;
              break;
            }
          default:
            {
              cur_idle_link_mode = BT_LM_HOLD;
              break;
            }
        }

        if ( best_idle_timeout == 0 )
        {
          /*  First one that applies;  must be best.  */

          best_idle_timeout      = lpc_ptr->idle_timeout;
          best_idle_link_mode    = cur_idle_link_mode;
          best_idle_max_interval = lpc_ptr->max_interval;
        }
        else
        {
          /*  Minimize idle timeout and max interval.  */
          if ( lpc_ptr->idle_timeout < best_idle_timeout )
          {
            best_idle_timeout = lpc_ptr->idle_timeout;
          }
          if ( lpc_ptr->max_interval < best_idle_max_interval )
          {
            best_idle_max_interval = lpc_ptr->max_interval;
          }
          if ( cur_idle_link_mode != best_idle_link_mode )
          {
            /* BT_TDB:  prioritize modes */
          }
        }
      }
    }
  }
  if ( best_idle_timeout != 0 )
  {
    conn_ptr->idle_timeout      = best_idle_timeout;
    conn_ptr->idle_link_mode    = best_idle_link_mode;
    conn_ptr->idle_max_interval = best_idle_max_interval;
  }
  else
  {
    /*  No configuration applies;  revert to default settings.  */
    bt_rm_init_conn_idle_link_mode( conn_ptr );
  }
  bt_rm_reset_idle_timeout( conn_ptr );

}


#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_rm_check_lpm_param

DESCRIPTION
  Checks the low power configuration of a specified connection which has been
  in sniff to see if it is compatible with the current one.

===========================================================================*/
LOCAL void bt_rm_check_lpm_param
(
  bt_rm_conn_type*  conn_ptr
)
{
  if ( IS_SSR_DOABLE() )
  {
    if ( conn_ptr->max_interval != conn_ptr->cur_interval )
    {
      conn_ptr->link_mode_request = BT_LM_ACTIVE;
    }
    else if ( (conn_ptr->ssr_state > BT_RM_SSR_NOT_IN_PROGRESS) &&
              (conn_ptr->max_latency != conn_ptr->cur_latency) )
    {
      /* if SSR is not in progress yet, this job is done
      ** in mode_change_event processing, when applicable.
      */
      BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                  conn_ptr->hc_handle_acl,
                  conn_ptr->max_latency, 0 );

      bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                 conn_ptr->max_latency,
                                 BT_RM_DEF_REMOTE_TIMEOUT,
                                 BT_RM_DEF_LOCAL_TIMEOUT );
    }
  }
  else  /* at least one end is a legacy device */
  {
    if ( conn_ptr->ssr_state != BT_RM_SSR_ENABLED )
    {
      if ( conn_ptr->max_interval != conn_ptr->cur_interval )
      {
        if (conn_ptr->ssr_emulating == TRUE)
        {
          BT_MSG_API( "BT RM: SSR emulation terminated", 0, 0, 0 );

          conn_ptr->ssr_emulating = FALSE;
        }
        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
    }
    else
    {
      if ( conn_ptr->max_latency != conn_ptr->cur_interval )
      {
        conn_ptr->link_mode_request = BT_LM_ACTIVE;
      }
    }
  }

  if ( conn_ptr->link_mode_request == BT_LM_ACTIVE )
  {
    /* set flag to Sniff when Unsniff is successful */
    conn_ptr->sniff_param_changed = TRUE;

    BT_MSG_API( "BT RM CMD TX: HC Unsniff H %x",
                conn_ptr->hc_handle_acl, 0, 0 );

    bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );
  }

}
#endif /* BT_SWDEV_2_1_SSR */


/*===========================================================================

FUNCTION
  bt_rm_set_connection_lpc

DESCRIPTION
  Sets the low power configuration of a specified connection using all
  applicable current low power configurations.

===========================================================================*/
LOCAL void bt_rm_set_connection_lpc
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16                     i;
  uint16                     prev_link_policy = 0;
  bt_rm_low_power_cfg_type*  lpc_ptr                  = NULL;
  uint16                     best_idle_timeout        = 0;
  uint16                     best_max_interval        = 0;
  bt_link_mode_type          best_idle_link_mode      = BT_LM_NONE;
  bt_link_mode_type          cur_idle_link_mode;
#ifdef BT_SWDEV_2_1_SSR
  uint16                     temp_latency             = conn_ptr->max_latency;
  uint16                     best_max_latency         = BT_RM_DEF_MAX_LATENCY;
  boolean                    max_latency_proposed     = FALSE;
  boolean                    stick_to_app_param       = FALSE;
  boolean                    sniff_initiator_oriented = FALSE;
#endif /* BT_SWDEV_2_1_SSR */

  for ( i = 0; i < BT_RM_MAX_LPCS; i++ )
  {
    lpc_ptr = &bt_rm_lpc[ i ];

    if ( lpc_ptr->conn_ptr == conn_ptr )
    {
      if ( (lpc_ptr->idle_timeout != 0) &&
           ((lpc_ptr->low_power_ctrl == BT_LPC_ACTIVE) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_SNIFF) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_PARK) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_HOLD) ||
            (lpc_ptr->low_power_ctrl == BT_LPC_SNIFF_LOCK)) )
      {
        /*  Entry meant to configure idle low power mode.  */

        prev_link_policy = conn_ptr->link_policy;

        switch ( lpc_ptr->low_power_ctrl )
        {
          case BT_LPC_ACTIVE:
            {
              cur_idle_link_mode = BT_LM_ACTIVE;
              break;
            }
          case BT_LPC_SNIFF_LOCK:
            /*
             * This option allows sending ACL data while in sniff.
             * Device will not be brought out of sniff when ACL data is to be
             * transmitted during sniff (using bt_rm_cmd_tx_data_acl).
             */
            {
              /* Re-enable remote initiated sniff */
              conn_ptr->link_policy |= BT_LINK_POLICY_SNIFF_MODE_B;
              cur_idle_link_mode     = BT_LM_SNIFF_LOCK;
              break;
            }
          case BT_LPC_DONT_CARE:
          case BT_LPC_SNIFF:
            {
              /* Re-enable remote initiated sniff */
              conn_ptr->link_policy |= BT_LINK_POLICY_SNIFF_MODE_B;
              cur_idle_link_mode     = BT_LM_SNIFF;
              break;
            }
          case BT_LPC_PARK:
            {
              cur_idle_link_mode = BT_LM_PARK;
              break;
            }
          default:
            {
              cur_idle_link_mode = BT_LM_HOLD;
              break;
            }
        }

        if ( best_idle_timeout == 0 )
        {
          /*  First one that applies;  must be best.  */

          best_idle_timeout        = lpc_ptr->idle_timeout;
          best_max_interval        = lpc_ptr->max_interval;
          best_idle_link_mode      = cur_idle_link_mode;
#ifdef BT_SWDEV_2_1_SSR
          stick_to_app_param       = lpc_ptr->stick_to_app_param;
          sniff_initiator_oriented = lpc_ptr->sniff_initiator_oriented;

          if ( lpc_ptr->max_latency != BT_RM_SSR_DONT_CARE )
          {
            /* App proposed a max_latency for RM to use */
            max_latency_proposed = TRUE;
            best_max_latency     = lpc_ptr->max_latency;
          }
          /* else, the profile accepts any SSR parameters to be used */
#endif /* BT_SWDEV_2_1_SSR */
        }
        else
        {
          /*  Resolving LPM parameters  */

          if ( lpc_ptr->idle_timeout < best_idle_timeout )
          {
            best_idle_timeout = lpc_ptr->idle_timeout;
          }

          if ( lpc_ptr->max_interval < best_max_interval )
          {
            best_max_interval = lpc_ptr->max_interval;
          }

          if ( cur_idle_link_mode != best_idle_link_mode )
          {
            /* BT_TDB:  prioritize modes */
          }

#ifdef BT_SWDEV_2_1_SSR
          if ( (lpc_ptr->max_latency != BT_RM_SSR_DONT_CARE) &&
               ((max_latency_proposed == FALSE) ||
                (lpc_ptr->max_latency < best_max_latency)) )
          {
            best_max_latency     = lpc_ptr->max_latency;
            max_latency_proposed = TRUE;
          }

          if ( lpc_ptr->stick_to_app_param == TRUE )
          {
            stick_to_app_param = TRUE;
          }

          if ( lpc_ptr->sniff_initiator_oriented == TRUE )
          {
            sniff_initiator_oriented = TRUE;
          }
#endif /* BT_SWDEV_2_1_SSR */
        }
      }
    }
  }

  if ( best_idle_timeout != 0 )
  {
    conn_ptr->idle_timeout             = best_idle_timeout;
    conn_ptr->max_interval             = best_max_interval;
    conn_ptr->idle_link_mode           = best_idle_link_mode;
#ifdef BT_SWDEV_2_1_SSR
    conn_ptr->max_latency              = best_max_latency;
    conn_ptr->stick_to_app_param       = stick_to_app_param;
    conn_ptr->sniff_initiator_oriented = sniff_initiator_oriented;
#endif /* BT_SWDEV_2_1_SSR */

    if ( conn_ptr->link_policy != prev_link_policy )
    {
      BT_MSG_API( "BT RM CMD TX: HC Wr LP %x H %x RH %x",
                  conn_ptr->link_policy, conn_ptr->hc_handle_acl,
                  conn_ptr->rm_handle );
      bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                                conn_ptr->link_policy );
    }
  }
  else
  {
    /*  No configuration applies;  revert to default settings.  */
    /* if we can enter here, max_interval & max_latency, etc. wouldn't
    ** have been changed. So no need to re-initialize them.
    */
    conn_ptr->link_policy = BT_RM_DEF_LINK_POLICY_M;
    bt_rm_init_conn_idle_link_mode( conn_ptr );
  }

#ifdef BT_SWDEV_2_1_SSR
  if ( conn_ptr->link_mode == BT_LM_SNIFF )
  {
    /* in Sniff */
    bt_rm_check_lpm_param(conn_ptr);
  }
  else if ( conn_ptr->link_mode_request == BT_LM_SNIFF )
  {
    /* Sniff in negotiation */
    conn_ptr->sniff_param_changed = TRUE;
  }
  else /* in Active Mode && no Sniff related activities ongoing */
  {
    /* if max_latency changed and local controller supports SSR,
    ** RM will update the controller of the new max_latency.
    */
    if ( (temp_latency != conn_ptr->max_latency) &&
         (bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1) )
    {
      BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                  conn_ptr->hc_handle_acl,
                  conn_ptr->max_latency, 0 );

      bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                 conn_ptr->max_latency,
                                 BT_RM_DEF_REMOTE_TIMEOUT,
                                 BT_RM_DEF_LOCAL_TIMEOUT );
    }

    bt_rm_reset_idle_timeout( conn_ptr );
  }
#else
  bt_rm_reset_idle_timeout( conn_ptr );
#endif /* BT_SWDEV_2_1_SSR */

}


/*===========================================================================

FUNCTION
  bt_rm_check_idle_timeout

DESCRIPTION
  Takes appropriate action if the specificied link idle timeout
  has expired.

===========================================================================*/
LOCAL void bt_rm_check_idle_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

#ifdef BT_SWDEV_2_1_SSR
  if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTED) &&
#ifndef FEATURE_BT_SOC
       (conn_ptr->state_sco == BT_RM_CS_DISCONNECTED) &&
#endif
       (conn_ptr->link_mode_request == BT_LM_NONE) &&
       (bt_rm_connection_lmp_stable( conn_ptr ) == BT_CS_GN_SUCCESS) )
  {
    uint16  cur_interval = conn_ptr->max_interval;

    if ( conn_ptr->idle_timeout_ms_left != 0 )
    {
      if ( conn_ptr->idle_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
      {
        conn_ptr->idle_timeout_ms_left = 0;

        if ( conn_ptr->link_mode == BT_LM_ACTIVE )
        {
          BT_MSG_API( "BT RM: idle timeout expired", 0, 0, 0 );

          conn_ptr->link_mode_request = BT_LM_SNIFF;
        }
        else if ( conn_ptr->ssr_emulating == FALSE )
        {
          /* in Sniff with BT 2.1 device */

          BT_MSG_API( "BT RM: SSR waiting timeout expired", 0, 0, 0 );

          BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                      conn_ptr->hc_handle_acl,
                      conn_ptr->max_latency, 0 );

          bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                     conn_ptr->max_latency,
                                     BT_RM_DEF_REMOTE_TIMEOUT,
                                     BT_RM_DEF_LOCAL_TIMEOUT );
        }
      }
      else
      {
        conn_ptr->idle_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
      }
    }
    else if ( conn_ptr->sniff_retry_to_ms_left != 0 )
    {
      /* can only reach here when in Active mode */

      if ( conn_ptr->sniff_retry_to_ms_left <= BT_BACKGROUND_TIMER_MS )
      {
        BT_MSG_API( "BT RM: Sniff retry timeout expired", 0, 0, 0 );

        if ( conn_ptr->ssr_emulating == TRUE )
        {
          /* SSR emu failed to be initiated just now,
          ** retry it now.
          */
          cur_interval = conn_ptr->max_latency;
        }

        conn_ptr->link_mode_request      = BT_LM_SNIFF;
        conn_ptr->sniff_retry_to_ms_left = 0;
      }
      else
      {
        conn_ptr->sniff_retry_to_ms_left -= BT_BACKGROUND_TIMER_MS;
      }
    }
    else if ( conn_ptr->ssr_emulating == TRUE )
    {
      /* SSR emulation ongoing */

      if ( conn_ptr->sniff_mode_to_ms_left != 0 )
      {
        if ( conn_ptr->sniff_mode_to_ms_left <= BT_BACKGROUND_TIMER_MS )
        {
          BT_MSG_API( "BT RM: SSR emu sniff mode timeout expired", 0, 0, 0 );

          conn_ptr->link_mode_request     = BT_LM_ACTIVE;
          conn_ptr->sniff_mode_to_ms_left = 0;

          BT_MSG_API( "BT RM CMD TX: HC Unsniff H %x",
                      conn_ptr->hc_handle_acl, 0, 0 );

          bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );
        }
        else
        {
          conn_ptr->sniff_mode_to_ms_left -= BT_BACKGROUND_TIMER_MS;
        }
      }
      else
      {
        /* How can we get here?? */
        BT_MSG_HIGH( "BT RM: SSR emu failed", 0, 0, 0 );
      }
    }

    switch ( conn_ptr->idle_link_mode )
    {
      case BT_LM_SNIFF:
      case BT_LM_SNIFF_LOCK:
        {
          if ( conn_ptr->link_mode_request == BT_LM_SNIFF )
          {
            if( !( conn_ptr->remote_lmp_features.lmp_features_bytes[0] &
                    BT_LM_FEATURE_BYTE0_SNIFF_B ) )
            {
              BT_MSG_API( "BT RM: Remote Device do not support sniff H %x",
                          conn_ptr->hc_handle_acl, 0, 0 );

              conn_ptr->link_mode_request = BT_LM_NONE;
            }
            else if (
              /* rm initiated bonding in progress */
              ( conn_ptr->bonding_app_id != BT_APP_ID_NULL) ||
              /* incoming dedicated bonding in progress */
              ( conn_ptr->dedicated_bonding == TRUE ) ||
              /* user cfm for bonding is pending from app */
              ( conn_ptr->pin_req_resps_pending != 0 ) ||
              /* rebond request cfm is pending from app */
              ( conn_ptr->rebond_req_pending == TRUE ) )
            {
              BT_MSG_API( "BT RM : BAI = 0x%x DB = 0x%x UCFMP = 0x%x",
                           conn_ptr->bonding_app_id, conn_ptr->dedicated_bonding,
                           conn_ptr->pin_req_resps_pending );
              BT_MSG_API( "BT RM : RBP = 0x%x",
                          conn_ptr->rebond_req_pending, 0, 0 );
              BT_MSG_API( "BT RM : Bonding in progress, reject sniff mode req",
                          0, 0, 0 );

              conn_ptr->link_mode_request = BT_LM_NONE;
            }
            else
            {
              BT_MSG_API( "BT RM CMD TX: HC Sniff H %x I %x",
                          conn_ptr->hc_handle_acl, cur_interval, 0 );

              bt_cmd_hc_sniff_mode(
                conn_ptr->hc_handle_acl,
                BT_RM_DEF_MIN_INTERVAL,
                cur_interval,
                BT_RM_DEF_SNIFF_ATTEMPT,
                BT_RM_DEF_SNIFF_TIMEOUT );
            }
          }
          break;
        }
      /* BT_TBD */
      case BT_LM_ACTIVE:
      case BT_LM_PARK:
      case BT_LM_HOLD:
      case BT_LM_NONE:
      default:
        {
          conn_ptr->link_mode_request = BT_LM_NONE;
          break;
        }
    }
  }
#else
  if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTED) &&
#ifndef FEATURE_BT_SOC
       (conn_ptr->state_sco == BT_RM_CS_DISCONNECTED) &&
#endif
       (conn_ptr->link_mode == BT_LM_ACTIVE) &&
       (conn_ptr->idle_timeout_ms_left != 0) &&
       (bt_rm_connection_lmp_stable( conn_ptr ) == BT_CS_GN_SUCCESS) )
  {
    if ( conn_ptr->idle_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      conn_ptr->idle_timeout_ms_left = 0;

      switch ( conn_ptr->idle_link_mode )
      {
        case BT_LM_SNIFF:
        case BT_LM_SNIFF_LOCK:
          {
            if ( conn_ptr->link_mode_request == BT_LM_SNIFF )
            {
              if( !( conn_ptr->remote_lmp_features.lmp_features_bytes[0] &
                        BT_LM_FEATURE_BYTE0_SNIFF_B ) )
              {
                BT_MSG_API( "BT RM: Remote Device do not support sniff H %x",
                            conn_ptr->hc_handle_acl, 0, 0 );

                conn_ptr->link_mode_request = BT_LM_NONE;
              }
              else if (
                /* rm initiated bonding in progress */
                ( conn_ptr->bonding_app_id != BT_APP_ID_NULL) ||
                /* pin code request response is pending from app */
                ( conn_ptr->pin_req_resps_pending != 0 ) )
              {
                BT_MSG_API( "BT RM : BAI = 0x%x PRSP = 0x%x",
                  conn_ptr->bonding_app_id, conn_ptr->pin_req_resps_pending, 0 );
                BT_MSG_API( "BT RM : Bonding in progress, reject sniff mode req",
                  0, 0, 0 );

                conn_ptr->link_mode_request = BT_LM_NONE;
              }
              else
              {
                BT_MSG_API( "BT RM CMD TX: HC Sniff H %x I %x",
                            conn_ptr->hc_handle_acl, conn_ptr->max_interval, 0 );

                bt_cmd_hc_sniff_mode(
                  conn_ptr->hc_handle_acl,
                  BT_RM_DEF_MIN_INTERVAL,
                  conn_ptr->max_interval,
                  BT_RM_DEF_SNIFF_ATTEMPT,
                  BT_RM_DEF_SNIFF_TIMEOUT );
              }
            }
            break;
          }
        /* BT_TBD */
        case BT_LM_ACTIVE:
        case BT_LM_PARK:
        case BT_LM_HOLD:
        case BT_LM_NONE:
        default:
          {
            conn_ptr->link_mode_request = BT_LM_NONE;
            break;
          }
      }
    }
    else
    {
      conn_ptr->idle_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }
#endif /* BT_SWDEV_2_1_SSR */

}


/*===========================================================================

FUNCTION
  bt_rm_check_acl_req_timeout

DESCRIPTION
  Takes appropriate action if the specificied link ACL request
  timeout has expired.

===========================================================================*/
LOCAL void bt_rm_check_acl_req_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->acl_req_timeout_ms_left != 0 )
  {
    if ( conn_ptr->acl_req_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      conn_ptr->acl_req_timeout_ms_left = 0;

      if ( conn_ptr->bt_app_id_acl == BT_APP_ID_NULL )
      {
        BT_ERR( "BT RM: ACL Req TO RH %x HHA %x",
                conn_ptr->rm_handle, conn_ptr->hc_handle_acl, 0 );

        bt_rm_reject_connection( conn_ptr,
                                 BT_BE_CONN_REJECTED_NO_RESOURCE,
                                 BT_ACL_LINK );
      }
      else
      {
        BT_ERR( "BT RM: Unexp ACL Req TO AID %x RH %x HHA %x",
                conn_ptr->bt_app_id_acl,
                conn_ptr->rm_handle, conn_ptr->hc_handle_acl );
      }
    }
    else
    {
      conn_ptr->acl_req_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_check_sco_req_timeout

DESCRIPTION
  Takes appropriate action if the specificied link SCO request
  timeout has expired.

===========================================================================*/
LOCAL void bt_rm_check_sco_req_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->sco_req_timeout_ms_left != 0 )
  {
    if ( conn_ptr->sco_req_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      conn_ptr->sco_req_timeout_ms_left = 0;

      if ( conn_ptr->bt_app_id_sco == BT_APP_ID_NULL )
      {
        BT_ERR( "BT RM: SCO Req TO RH %x HHS %x",
                conn_ptr->rm_handle, conn_ptr->hc_handle_sco, 0 );

        bt_rm_reject_connection( conn_ptr,
                                 BT_BE_CONN_REJECTED_NO_RESOURCE,
                                 BT_SCO_LINK );
      }
      else
      {
        BT_ERR( "BT RM: Unexp SCO Req TO AID %x RH %x HHS %x",
                conn_ptr->bt_app_id_sco,
                conn_ptr->rm_handle, conn_ptr->hc_handle_sco );
      }
    }
    else
    {
      conn_ptr->sco_req_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }

}

#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_rm_hc_get_rssi_if_reqd

DESCRIPTION
  If app has requested RSSI updates for this connection, sends HCI cmd to
  read RSSI from the controller.

===========================================================================*/
LOCAL void bt_rm_hc_get_rssi_if_reqd
(
  bt_rm_conn_type*  conn_ptr
)
{

  uint16  i;
  boolean rssi_update_reqd = FALSE;

  for ( i = 0; i < BT_RM_MAX_LSRS; i++ )
  {
    if ( ( bt_rm_lsr[ i ].conn_ptr == conn_ptr ) &&
         ( bt_rm_lsr[ i ].rssi_delta !=0 )
       )
    {
         rssi_update_reqd = TRUE;
         break;
    }
  }

  if ( rssi_update_reqd == TRUE )
  {
    bt_cmd_hc_rd_rssi( conn_ptr->hc_handle_acl );
    /* bt_rm_update_rssi called during processing of cmd cmplt for rd_rssi */
  }
}

/*===========================================================================

FUNCTION
  bt_rm_check_rssi_update_timeout

DESCRIPTION
  Takes appropriate action if rssi update timeout has expired.

===========================================================================*/
LOCAL void bt_rm_check_rssi_update_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( ( conn_ptr->state_acl == BT_RM_CS_CONNECTED ) &&
       ( conn_ptr->link_mode == BT_LM_ACTIVE)
     )
  {
    if (conn_ptr->rssi_update_ms_left != 0)
    {
      if ( conn_ptr->rssi_update_ms_left <= BT_BACKGROUND_TIMER_MS )
      {
        conn_ptr->rssi_update_ms_left = BT_RM_RSSI_UPD_TIMEOUT_MS;
        bt_rm_hc_get_rssi_if_reqd ( conn_ptr );
      }
      else
      {
        conn_ptr->rssi_update_ms_left -= BT_BACKGROUND_TIMER_MS;
      }
    }
  }
  else
  {
     /* clear rssi update timer */
     conn_ptr->rssi_update_ms_left = 0;
  }
}
#endif /* FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_rm_send_hc_pin_reply_negative

DESCRIPTION
  Sends a PIN Code Reply Negative command to HCI for the
  specified connection.

===========================================================================*/
LOCAL void bt_rm_send_hc_pin_reply_negative
(
  bt_rm_conn_type*  conn_ptr,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_bd_addr_type*  bda_ptr;

  if ( conn_ptr != NULL )
  {
    bda_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;
    conn_ptr->pin_code_reply_neg_sent  = TRUE;
  }
  else
  {
    bda_ptr = bd_addr_ptr;
  }

  if ( bda_ptr != NULL )
  {
    BT_BDA( MSG_API, "BT RM CMD TX: HC PIN Rep Neg", bda_ptr );
    bt_cmd_hc_pin_code_req_reply_neg( bda_ptr );
  }
  else
  {
    BT_ERR( "BT RM: Bad Send HC PIN Rep Neg - No BDA!", 0, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_finish_pin_req_attempt

DESCRIPTION
  Finishes the PIN request process for the specificied connection.

===========================================================================*/
LOCAL void bt_rm_finish_pin_req_attempt
(
  bt_rm_conn_type*  conn_ptr,
  boolean           pin_req_timeout
)
{

  if ( (conn_ptr->pin_req_resps_pending != 0) &&
       (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
       (conn_ptr->pin_req_pairing_done == FALSE) )
  {
#ifdef BT_SWDEV_2_1_SSP
   if ( conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE )
   {
     /* Legacy device */
     BT_MSG_HIGH( "BT RM CMD TX: HC SSP PinReplyNeg",0,0,0);
     if (conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_WAITING)
     {
         /* Timeout while pre-fetching pin for re-pairing ,
          * so just send the security result event
          */
         bt_rm_ev_send_security_result( conn_ptr );
     }
     else
     {
       bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
     }
   }
   else if (( conn_ptr->auth_stage1_proc == BT_RM_AS1_NUMERIC_COMPARISON ) ||
            ( conn_ptr->auth_stage1_proc == BT_RM_AS1_JUST_WORKS ))
   {
     BT_MSG_HIGH( "BT RM CMD TX: HC SSP UserConfNegReply",0,0,0);
     bt_cmd_hc_user_cfmtn_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
   }
   else
   {
     BT_MSG_HIGH( "BT RM CMD TX: HC SSP PKNegReply",0,0,0);
     bt_cmd_hc_user_passkey_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
   }
#else
      if (conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_WAITING)
      {
          /* Timeout while pre-fetching pin for re-pairing ,
           * so just send the security result event
           */
          bt_rm_ev_send_security_result( conn_ptr );
      }
      else
      {
        bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
      }
#endif /* BT_SWDEV_2_1_SSP */
  }
  else if ( pin_req_timeout != FALSE )
  {
#ifdef BT_SWDEV_2_1_SSP
    BT_MSG_DEBUG( "BT RM: SSP Unexp PINR/UCONFR/PKR TO PC %x AID %x D %x",
                  conn_ptr->pin_req_resps_pending,
                  conn_ptr->pin_req_resp_app_id,
                  conn_ptr->pin_req_pairing_done );
#else
    BT_MSG_DEBUG( "BT RM: Unexp PINR TO PC %x AID %x D %x",
                  conn_ptr->pin_req_resps_pending,
                  conn_ptr->pin_req_resp_app_id,
                  conn_ptr->pin_req_pairing_done );
#endif /* BT_SWDEV_2_1_SSP */
  }

  bt_rm_init_conn_pin_request( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_check_enh_enc_timeout

DESCRIPTION
  Takes appropriate action if the specificied link Enhance Encryption request
  timeout has expired.

===========================================================================*/
LOCAL void bt_rm_check_enh_enc_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{
  if( ( conn_ptr->mss_enh_enc_timeout_ms_left != 0 ) &&
      ( conn_ptr->pin_req_resps_pending == 0 ) )
  {
    if ( conn_ptr->mss_enh_enc_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      conn_ptr->mss_enh_enc_timeout_ms_left = 0;
      /* Enhance encryption as remote device disabled encription and
       * did not resume the same.*/
#ifdef BT_SWDEV_2_1_SSP
      if (( conn_ptr->enh_enc_state != BT_RM_ENH_ENC_OK) &&
          ( conn_ptr->sec_level_min  != BT_RM_SL_0_NONE ) )
#else
      if (( conn_ptr->enh_enc_state != BT_RM_ENH_ENC_OK) &&
          ( conn_ptr->security_min  == BT_SEC_AUTHENTICATE_AND_ENCRYPT ) )
#endif /* BT_SWDEV_2_1_SSP */
      {
        BT_MSG_DEBUG("BT RM: MSS Enc Enh timer expired starting Encryption H %d ECST %d",
                  conn_ptr->hc_handle_acl,
                  conn_ptr->enh_enc_state,
                  0);
        conn_ptr->enh_enc_state = BT_RM_ENH_ENC_STARTED;
        bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                                    BT_ENCRYPT_ENABLE_ON );
      }
      else
      {
          conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
      }
    }
    else
    {
      conn_ptr->mss_enh_enc_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_check_pin_req_timeout

DESCRIPTION
  Takes appropriate action if the specificied link PIN request
  timeout has expired.

===========================================================================*/
LOCAL void bt_rm_check_pin_req_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->pin_req_timeout_ms_left != 0 )
  {
    if ( conn_ptr->pin_req_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
    {
      conn_ptr->pin_req_timeout_ms_left = 0;

      /* Application has not responded with pin key during the timeout 
       * period. bt_rm_finish_pin_req_attempt() will cleanup and further
       * response are only sent if the local app initiated the bonding.
       * Send bond failed event so that app can update the UI appropiately
       * in case the pin request triggered by remote device.
      */
      if ( conn_ptr->bonding_app_id == BT_APP_ID_NULL ) 
      {
        bt_rm_ev_send_bond_failed_all_bondable(conn_ptr, BT_EVR_RM_BOND_NO_KEY);
      }
      bt_rm_finish_pin_req_attempt( conn_ptr, TRUE );
    }
    else
    {
      conn_ptr->pin_req_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_check_busy_timeout

DESCRIPTION
  Takes appropriate action if the specificied link busy timeout
  has expired.  The busy timeout makes sure links "settle" within
  a reasonable amount of time.  If not, the entry is cleared to
  prevent the RM from locking out all requests.

===========================================================================*/
LOCAL void bt_rm_check_busy_timeout
(
  bt_rm_conn_type*  conn_ptr
)
{

  boolean  link_busy = FALSE;

  /*  The logic to set link_busy below should conclude  */
  /*  the link is busy in the same way that             */
  /*  bt_rm_connection_settled() does, but here the     */
  /*  link state is not altered whatsoever.             */

  if ( bt_rm_connection_stable( conn_ptr ) != BT_CS_GN_SUCCESS )
  {
    link_busy = TRUE;
  }
  else if ( (conn_ptr->link_mode != BT_LM_ACTIVE) &&
            (conn_ptr->link_mode_request != BT_LM_NONE) )
  {
    link_busy = TRUE;
  }

  if ( link_busy != FALSE )
  {
    /*  Link is busy.  Monitor it.  */
    if ( conn_ptr->busy_timeout_ms_left != 0 )
    {
      if ( conn_ptr->busy_timeout_ms_left <= BT_BACKGROUND_TIMER_MS )
      {
        /*  Busy timeout has expired.  */

        conn_ptr->busy_timeout_ms_left = 0;

        BT_MSG_HIGH( "BT RM: BusyT Exp RH %x ASt %x SSt %x",
                     conn_ptr->rm_handle, conn_ptr->state_acl,
                     conn_ptr->state_sco );

        BT_MSG_HIGH( "BT RM: BusyT Exp BAID %x RNAID %x RR %x",
                     conn_ptr->bonding_app_id, conn_ptr->rname_app_id,
                     conn_ptr->role_request );

        BT_MSG_HIGH( "BT RM: BusyT Exp PR %x PRAID %x SR %x",
                     conn_ptr->pin_req_resps_pending,
                     conn_ptr->pin_req_resp_app_id,
                     conn_ptr->security_request );

        BT_MSG_HIGH( "BT RM: BusyT Exp LR %x PTRA %x",
                     conn_ptr->link_mode_request,
                     conn_ptr->pkt_types_req_acl, 0 );


        /*  Generate failure event and clean up as appropriate.  */

        if ( conn_ptr->bonding_app_id != BT_APP_ID_NULL )
        {
          bt_rm_update_conn_hc_error( conn_ptr, BT_BE_HW_FAILURE );
          bt_rm_finish_bond_attempt( conn_ptr );
        }
        else if ( conn_ptr->rname_app_id != BT_APP_ID_NULL )
        {
          bt_rm_finish_remote_name_request(
            conn_ptr, (bt_cmd_status_type)(BT_HCI_REASON( BT_BE_HW_FAILURE )) );
        }
        else if ( conn_ptr->state_acl == BT_RM_CS_CONNECTING )
        {
          bt_rm_connection_failed( conn_ptr, BT_ACL_LINK,
                                   (BT_HCI_REASON( BT_BE_HW_FAILURE )) );
        }
        else if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTING )
        {
          bt_rm_connection_disconnected(
            conn_ptr, BT_ACL_LINK, (BT_HCI_REASON( BT_BE_HW_FAILURE )) );
        }
        else if ( conn_ptr->state_sco == BT_RM_CS_CONNECTING )
        {
          bt_rm_connection_failed( conn_ptr, BT_SCO_LINK,
                                   (BT_HCI_REASON( BT_BE_HW_FAILURE )) );
        }
        else if ( conn_ptr->state_sco == BT_RM_CS_DISCONNECTING )
        {
          bt_rm_connection_disconnected(
            conn_ptr, BT_SCO_LINK, (BT_HCI_REASON( BT_BE_HW_FAILURE )) );
        }
        else if ( conn_ptr->role_request != BT_ROLE_NONE )
        {
          bt_rm_finish_role_switch( conn_ptr, conn_ptr->role,
                                    BT_BE_HW_FAILURE );
          if ( conn_ptr->enh_enc_state != BT_RM_ENH_ENC_OK )
          {
            conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
          }
        }
#ifdef BT_SWDEV_2_1_SSP
        else if ( conn_ptr->sec_level_request != BT_RM_SL_0_NONE )
        {
          conn_ptr->sec_level_request = BT_RM_SL_0_NONE;
#else
        else if ( conn_ptr->security_request != BT_SEC_NONE )
        {
          conn_ptr->security_request = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */

          bt_rm_update_conn_hc_error( conn_ptr, BT_BE_HW_FAILURE );
          bt_rm_link_status_updated( conn_ptr );
        }
        else if ( (conn_ptr->link_mode != BT_LM_ACTIVE) &&
                  (conn_ptr->link_mode_request != BT_LM_NONE) )
        {
          BT_ERR( "BT RM: ModeReq TO RH %x LM %x LMR %x",
                  conn_ptr->rm_handle, conn_ptr->link_mode,
                  conn_ptr->link_mode_request );

          conn_ptr->link_mode_request = BT_LM_NONE;
        }
        else if ( (conn_ptr->pin_req_resps_pending != 0) ||
                  (conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL) )
        {
          bt_rm_finish_pin_req_attempt( conn_ptr, FALSE );
        }
        else if ( conn_ptr->rd_remote_req_pending != FALSE )
        {
          conn_ptr->rd_remote_req_pending = FALSE;
        }

        if ( conn_ptr->pkt_types_req_acl != 0 )
        {
          BT_ERR( "BT RM: ChgPktAReq TO RH %x AA %x RA %x",
                  conn_ptr->rm_handle,
                  conn_ptr->pkt_types_in_use_acl,
                  conn_ptr->pkt_types_req_acl );

          conn_ptr->pkt_types_req_acl = 0;
        }
        if ( conn_ptr->state_acl == BT_RM_CS_CONNECTED )
        {
          bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
        }
      }
      else
      {
        conn_ptr->busy_timeout_ms_left -= BT_BACKGROUND_TIMER_MS;
      }
    }
    else
    {
      /*  Begin monitoring the busy link.  */
      conn_ptr->busy_timeout_ms_left = BT_RM_BUSY_TIMEOUT_MS;
    }
  }
  else
  {
    /*  Link is not busy.  No need to monitor it.  */
    conn_ptr->busy_timeout_ms_left = 0;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_optimize_bb_packets

DESCRIPTION
  Takes appropriate action to optimize the basband packets in use
  on a specified connection.

===========================================================================*/
LOCAL void bt_rm_optimize_bb_packets
(
  bt_rm_conn_type*  conn_ptr
)
{

  if ( conn_ptr->bb_opt_ms_left <= BT_BACKGROUND_TIMER_MS )
  {
    conn_ptr->bb_opt_ms_left = 0;

    bt_rm_check_conn_packet_types_acl( conn_ptr );
  }
  else
  {
    conn_ptr->bb_opt_ms_left -= BT_BACKGROUND_TIMER_MS;
  }

}


#ifdef BT_SWDEV_2_1_EPR
/*===========================================================================

FUNCTION
  bt_rm_check_conn_encryption_key

DESCRIPTION
  Checks the encryption key age and refreshes it periodically
  Disconnects if EK gets stale

===========================================================================*/
LOCAL void bt_rm_check_conn_encryption_key
(
  bt_rm_conn_type*  conn_ptr
)
{
  conn_ptr->encryption_key_age ++;
  if ( ! BT_LMP_FEATURE_SUPPORTED( BT_RM_LMP_FEATURE_BIT_PAUSE_ENCRYPTION,
                                   &conn_ptr->dev_ptr->dev_public.lmp_features) )
  {
    /* We dont refresh keys for legacy devices which dont support encryption pause */
    return;
  }

  if( conn_ptr->encryption_key_age >= BT_MAX_EK_AGE_STALE )
  {
    if (conn_ptr->state_acl != BT_RM_CS_DISCONNECTING &&
        conn_ptr->state_sco != BT_RM_CS_DISCONNECTING )
    {
      /* Stale EK (should have been refreshed before 23.3 hours) - disconnect the link */
      bt_cmd_rm_disconnect_acl( conn_ptr->bt_app_id_acl,
                                conn_ptr->rm_handle,
                                BT_RMDR_UNSPECIFIED_ERROR );
      BT_MSG_DEBUG( "BT RM: Stale EK dc %x Ag %x", conn_ptr->rm_handle,
                                                   conn_ptr->encryption_key_age, 0 );
    }
  }
#ifdef UPF
  else if(( conn_ptr->encryption_key_age >= BT_MAX_EK_AGE_LO &&
           conn_ptr->enc_key_refresh_req_pending == FALSE ) ||
          ( conn_ptr->encryption_key_age >= BT_MAX_EK_AGE_LO_UPF &&
            conn_ptr->enc_key_refresh_req_pending == FALSE  &&
            bt_shorten_epr_timer != FALSE ))
#else
  else if( conn_ptr->encryption_key_age >= BT_MAX_EK_AGE_LO &&
           conn_ptr->enc_key_refresh_req_pending == FALSE )
#endif
  {
    if( ( conn_ptr->link_mode == BT_LM_SNIFF &&
          conn_ptr->state_sco == BT_RM_CS_DISCONNECTED ) ||
          conn_ptr->encryption_key_age >= BT_MAX_EK_AGE_HI )
    {
      /* Refresh the EK when the link is idle.
         Also force the refresh if the EK is older than 23 hrs */
      bt_cmd_hc_refresh_encryption_key ( conn_ptr->hc_handle_acl );
      conn_ptr->enc_key_refresh_req_pending = TRUE;
      BT_MSG_DEBUG( "BT RM: Ref EK %x Ag %x", conn_ptr->rm_handle,
                                              conn_ptr->encryption_key_age, 0 );
    }
  }
}
#endif /* BT_SWDEV_2_1_EPR */


/*===========================================================================

FUNCTION
  bt_rm_background_tick

DESCRIPTION
  Performs background monitoring for RM.

===========================================================================*/
void bt_rm_background_tick
(
  void
)
{

  bt_rm_conn_type*  conn_ptr;
  uint16            i;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    conn_ptr = &bt_rm_conn[ i ];

    if ( conn_ptr->state_acl != BT_RM_CS_DISCONNECTED )
    {
      bt_rm_check_idle_timeout( conn_ptr );
      bt_rm_check_acl_req_timeout( conn_ptr );
      bt_rm_check_sco_req_timeout( conn_ptr );
      bt_rm_check_pin_req_timeout( conn_ptr );
      bt_rm_check_enh_enc_timeout (conn_ptr);
      bt_rm_check_busy_timeout( conn_ptr );
#ifndef FEATURE_BT_SOC
      bt_rm_optimize_bb_packets( conn_ptr );
#else
      bt_rm_check_rssi_update_timeout( conn_ptr );
#endif /* !FEATURE_BT_SOC */
#ifdef BT_SWDEV_2_1_EPR
      if ( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 &&
           conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON )
      {
        bt_rm_check_conn_encryption_key( conn_ptr );
      }
#endif /* BT_SWDEV_2_1_EPR */

    }
  }

  bt_rm_check_bb_page_state_timeout();

#ifdef FEATURE_BT_SOC
  bt_soc_etc_check_driver_idle_timeout();
#endif

}


/*===========================================================================

FUNCTION
  bt_rm_any_conn_lmactive_or_unstable

DESCRIPTION
   Returns TRUE if atleast one ACL is in link mode ACTIVE state or in
   "RM UNSTABLE" state.

===========================================================================*/
boolean bt_rm_any_conn_lmactive_or_unstable
(
  void
)
{
  uint16            i;
  boolean           retval = FALSE;
  bt_rm_conn_type*  conn_ptr;

  /* Look for ACTIVE or "RM UNSTABLE" connections */
  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    conn_ptr = &bt_rm_conn[ i ];
    if ( conn_ptr->state_acl != BT_RM_CS_DISCONNECTED )
    {
      /* TBD: Any other transitionary states need to be checked ? */
      if ( ( conn_ptr->link_mode == BT_LM_ACTIVE ) ||
           ( conn_ptr->link_mode_request != BT_LM_NONE ) ||
           ( bt_rm_connection_stable( conn_ptr ) != BT_CS_GN_SUCCESS )
         )
      {
#ifdef BT_BG_TIMER_DEBUG
        BT_MSG_DEBUG ( "BT RM: Link lmactive or unstable idx %x lm %x lmr %x",
                        i, conn_ptr->link_mode, conn_ptr->link_mode_request );
#endif
        retval = TRUE;
        break;
      }
    }
  }
  return retval;
}

/*===========================================================================

FUNCTION
  bt_rm_check_persistent_tick_required

DESCRIPTION
  Checks if current RM state requires background ticks to be running while
  the processor is in power collapse.

===========================================================================*/
boolean bt_rm_persistent_tick_required
(
  void
)
{
  /*
   * Persistent background tick is requested under the following situations:
   * 1. Page or or inquiry is in progress.
   * 2. a. One or more ACL links are up in Active mode ( i.e. not in LPM ).
   *    In this case persistent tick gurantees timing until all ACL links
   *    are in low power mode.
   *    b. There is atleast one connection that is "RM UNSTABLE" ( i.e there
   *    is an ongoing asynchronous transaction on the ACL ) that needs to be
   *    monitored for orderly completion.
  */

#ifdef BT_BG_TIMER_DEBUG
  BT_MSG_DEBUG ( "BT RM: Check for persistent tick: ds %x ",
                  bt_dc.driver_state, 0, 0 );
  BT_MSG_DEBUG ( "BT RM: Check for persistent tick sa %x ia %x pa %x",
                  bt_dc.scan_active, bt_dc.inq_active, bt_dc.page_active );
#endif
  if (bt_dc.driver_state == BT_DS_SOC_ACTIVE )
  {
    if ( bt_dc.inq_active ||
         bt_dc.page_active ||
         ( bt_rm_any_conn_lmactive_or_unstable() == TRUE ) ||
         ( bt_dc.radio_state == BT_RADIO_DISABLING )
        )
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION
  bt_rm_check_scan_settings

DESCRIPTION
  This routine updates the scan settings as necessary.

===========================================================================*/
LOCAL void bt_rm_check_scan_settings
(
  void
)
{

  uint16                   i;
  boolean                  page_scan = FALSE;
  uint16                   page_scan_interval;
  uint16                   page_scan_window;
  bt_rm_discoverable_type  discoverable_mode = BT_RM_DISCOVERABLE_NOT;
  uint8                    iac_count;
  dsm_item_type*           dsm_ptr;
  bt_lap_type              iac_lap;
  uint16                   inq_scan_interval;
  uint16                   inq_scan_window;
  bt_hc_scan_enable_type   scan_enable;
  bt_cod_type              cur_cod;
  bt_radio_activity_type   radio_activity_copy;

/*  BT_TBD:  This is only for testing.  */
static boolean double_scan_windows_with_sco = TRUE;

#ifndef FEATURE_BT_SOC
  double_scan_windows_with_sco = FALSE;
#endif

  if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
     (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
     (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
  {
    if ( bt_dc.page_active || bt_dc.inq_active )
    {
      /* bail out .. scans will be re-evaluated later */
      return;
    }
  }

#if defined ( BT_AMS_DISABLED )
  /*  Disable all scanning if we are in any connection.  */
  /*  This effectively disables multiple slave support.  */
  if ( bt_rm_no_connections() == BT_CS_GN_SUCCESS )
#elif defined ( FEATURE_BT_HOST_DIS_SCATTERNET )
  /* If there is an acl link in slave role, don't check for required scans. */
  /* This will effectively prevent scatternet provided link policy changes  */
  /* to allow role change is not permitted in AMS . When we are in AMS mode */
  /* incoming connections are rejected if the role is not master when       */
  /* connection complete event for ACL links is processed.                  */
  if ( bt_rm_check_for_slave_acl_link() == FALSE )
#endif /* BT_AMS_DISABLED */
  {
    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL )
      {
        if ( bt_rm_app[ i ].connectable != FALSE )
        {
          page_scan = TRUE;
        }
        if ( (bt_rm_app[ i ].discoverable > discoverable_mode)
#ifdef BT_SWDEV_RADIO_OFF
             && (bt_rm_data.disable_discoverability == FALSE)
#endif
           )
        {
          discoverable_mode = bt_rm_app[ i ].discoverable;
        }
      }
    }
  }

#ifndef FEATURE_BT_SOC
  cur_cod = bt_lm_data.attributes.class_of_device;
#else
  cur_cod = bt_class_of_device;
#endif /* FEATURE_BT_SOC */
  BT_SD_WR_COD_LIM_DISCV_OFF( cur_cod );

  if ( discoverable_mode != BT_RM_DISCOVERABLE_NOT )
  {
    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                   (2 * sizeof( bt_lap_type )) );

    iac_lap[ 0 ] = BT_GIAC_0;
    iac_lap[ 1 ] = BT_IAC_1;
    iac_lap[ 2 ] = BT_IAC_2;

    dsm_pushdown_tail( &dsm_ptr, (void*)(&iac_lap),
                       sizeof( bt_lap_type ),
                       DSM_DS_SMALL_ITEM_POOL );

    iac_count = 1;

    if ( discoverable_mode == BT_RM_DISCOVERABLE_LIMITED )
    {
      iac_lap[ 0 ] = BT_LIAC_0;
      iac_lap[ 1 ] = BT_IAC_1;
      iac_lap[ 2 ] = BT_IAC_2;

      dsm_pushdown_tail( &dsm_ptr, (void*)(&iac_lap),
                         sizeof( bt_lap_type ),
                         DSM_DS_SMALL_ITEM_POOL );

      iac_count++;

      BT_SD_WR_COD_LIM_DISCV_ON( cur_cod );
    }

    BT_MSG_API( "BT RM CMD TX: HC WrCurIACLAP C %x DM %x",
                iac_count, discoverable_mode, 0 );
    bt_cmd_hc_wr_cur_iac_lap( iac_count, dsm_ptr );
  }

#ifndef FEATURE_BT_SOC
  if ( (BT_SD_COD_2_LIM_DISCV( cur_cod )) !=
          (BT_SD_COD_2_LIM_DISCV( bt_lm_data.attributes.class_of_device)) )
  {
#else
  if ( (BT_SD_COD_2_LIM_DISCV( cur_cod )) !=
          (BT_SD_COD_2_LIM_DISCV( bt_class_of_device)) )
  {
#endif /* FEATURE_BT_SOC */
    BT_MSG_API( "BT RM CMD TX: HC Wr COD %02x %02x %02x",
                cur_cod.cod_bytes[ 2 ], cur_cod.cod_bytes[ 1 ],
                cur_cod.cod_bytes[ 0 ] );
    bt_cmd_hc_wr_class_of_device( cur_cod );
  }

  if ( page_scan == FALSE )
  {
    if ( discoverable_mode == BT_RM_DISCOVERABLE_NOT )
    {
      /*  Do not scan at all now.  Restore default inquiry and  */
      /*  page scan parameter settings and disable scanning.    */

      page_scan_interval = BT_DEFAULT_PAGESCAN_INTERVAL;
      page_scan_window   = BT_DEFAULT_PAGESCAN_WINDOW;

      inq_scan_interval  = BT_DEFAULT_INQUIRYSCAN_INTERVAL;
      inq_scan_window    = BT_DEFAULT_INQUIRYSCAN_WINDOW;

      scan_enable        = BT_HC_INQ_DIS_PAGE_DIS;

      bt_dc.disable_scan_pending = TRUE;
    }
    else
    {
      /*  Inquiry scan only now.  Restore default page scan   */
      /*  parameter settings and set inquiry scan parameters  */
      /*  for inquiry scan only.                              */

      page_scan_interval = BT_DEFAULT_PAGESCAN_INTERVAL;
      page_scan_window   = BT_DEFAULT_PAGESCAN_WINDOW;

      inq_scan_interval  = BT_DEFAULT_INQUIRYSCAN_ONLY_INTERVAL;

      if ( bt_rm_total_active_sco_links() == 0 )
      {
        inq_scan_window  = BT_DEFAULT_INQUIRYSCAN_WINDOW;
      }
      else
      {
        inq_scan_window  = BT_DEFAULT_INQUIRYSCAN_WINDOW_SCO;

        /*  BT_TBD:  This is only for testing.  */
        if ( double_scan_windows_with_sco == FALSE )
        {
          inq_scan_window = BT_DEFAULT_INQUIRYSCAN_WINDOW;
        }
      }

      scan_enable        = BT_HC_INQ_EN_PAGE_DIS;
      bt_dc.disable_scan_pending = FALSE;


      bt_dc.scan_active  = TRUE;
    }

    bt_l2_disconnect_acl_if_unused();

  }
  else
  {
    if ( discoverable_mode == BT_RM_DISCOVERABLE_NOT )
    {
      /*  Page scan only now.  Restore default inquiry scan  */
      /*  parameter settings and set page scan parameters    */
      /*  for page scan only.                                */

      page_scan_interval = BT_DEFAULT_PAGESCAN_ONLY_INTERVAL;

      if ( bt_rm_total_active_sco_links() == 0 )
      {
        page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW;
      }
      else
      {
        page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW_SCO;

        /*  BT_TBD:  This is only for testing.  */
        if ( double_scan_windows_with_sco == FALSE )
        {
          page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW;
        }
      }

      inq_scan_interval  = BT_DEFAULT_INQUIRYSCAN_INTERVAL;
      inq_scan_window    = BT_DEFAULT_INQUIRYSCAN_WINDOW;

      scan_enable        = BT_HC_INQ_DIS_PAGE_EN;
      bt_dc.disable_scan_pending = FALSE;

      bt_dc.scan_active  = TRUE;
    }
    else
    {
      /*  Inquiry and page scan now.  Restore default inquiry  */
      /*  and page scan parameter settings.                    */

      page_scan_interval = BT_DEFAULT_PAGESCAN_INTERVAL;

      inq_scan_interval  = BT_DEFAULT_INQUIRYSCAN_INTERVAL;

      if ( bt_rm_total_active_sco_links() == 0 )
      {
        page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW;
        inq_scan_window  = BT_DEFAULT_INQUIRYSCAN_WINDOW;
      }
      else
      {
        page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW_SCO;
        inq_scan_window  = BT_DEFAULT_INQUIRYSCAN_WINDOW_SCO;

        /*  BT_TBD:  This is only for testing.  */
        if ( double_scan_windows_with_sco == FALSE )
        {
          page_scan_window = BT_DEFAULT_PAGESCAN_WINDOW;
          inq_scan_window  = BT_DEFAULT_INQUIRYSCAN_WINDOW;
        }
      }

      scan_enable        = BT_HC_INQ_EN_PAGE_EN;
      bt_dc.disable_scan_pending = FALSE;

      bt_dc.scan_active  = TRUE;
    }
  }

#ifdef FEATURE_BT_SOC
#ifndef FEATURE_BT_QSOC
  /* Modify scan parameters if SOC other than QSOC is in use */
  if ( bt_rm_total_active_sco_links() == 0 )
  {
    bt_rm_data.conf_page_scan_window = BT_PAGESCAN_WINDOW_SOC;
    bt_rm_data.conf_inq_scan_window  = BT_INQUIRYSCAN_WINDOW_SOC;
  }
  else
  {
    bt_rm_data.conf_page_scan_window = BT_PAGESCAN_WINDOW_SOC_SCO;
    bt_rm_data.conf_inq_scan_window  = BT_INQUIRYSCAN_WINDOW_SOC_SCO;
  }
#endif /* !FEATURE_BT_QSOC */

  page_scan_interval = bt_rm_data.conf_page_scan_interval;
  page_scan_window = bt_rm_data.conf_page_scan_window;
  inq_scan_interval = bt_rm_data.conf_inq_scan_interval;
  inq_scan_window = bt_rm_data.conf_inq_scan_window;

  radio_activity_copy =   bt_rm_data.current_radio_activity;
  /* Reinit scan bits in current_radio_activity */
  /* Bits will be updated below based on scan_enable */
  BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQ_SCANNING | BT_RA_PAGE_SCANNING );

#endif /* FEATURE_BT_SOC */

  if ( scan_enable & BT_SCAN_PAGE_B )
  {
    BT_RM_UPD_RA_AND_SETBIT( BT_RA_PAGE_SCANNING );

#if ( (defined FEATURE_BT_SOC) && (defined FEATURE_BT_1_2) )
    BT_MSG_API( "BT RM CMD TX: HC WrPScanType %x",
                bt_rm_data.conf_page_scan_type, 0, 0 );
    bt_cmd_hc_wr_page_scan_type ( bt_rm_data.conf_page_scan_type );
#endif /* FEATURE_BT_SOC && FEATURE_BT_1_2 */

    BT_MSG_API( "BT RM CMD TX: HC WrPScanAct I %x W %x",
                page_scan_interval, page_scan_window, 0 );
    bt_cmd_hc_wr_page_scan_act( page_scan_interval, page_scan_window );
  }

  if ( scan_enable & BT_SCAN_INQ_B )
  {
    BT_RM_UPD_RA_AND_SETBIT( BT_RA_INQ_SCANNING );

#if ( (defined FEATURE_BT_SOC) && (defined FEATURE_BT_1_2) )
    BT_MSG_API( "BT RM CMD TX: HC WrIScanType %x",
                bt_rm_data.conf_inq_scan_type, 0, 0 );
    bt_cmd_hc_wr_inq_scan_type ( bt_rm_data.conf_inq_scan_type );
#endif /* FEATURE_BT_SOC && FEATURE_BT_1_2 */

    BT_MSG_API( "BT RM CMD TX: HC WrIScanAct I %x W %x",
                inq_scan_interval, inq_scan_window, 0 );
    bt_cmd_hc_wr_inq_scan_act( inq_scan_interval, inq_scan_window );
  }

  BT_MSG_API( "BT RM CMD TX: HC WrScanEn %x",
              scan_enable, 0, 0 );
  bt_cmd_hc_wr_scan_enable( scan_enable );

  if ( radio_activity_copy != bt_rm_data.current_radio_activity ) 
  {
    /* Radio activity has changed by above logic. Send event .. */
    if( bt_rm_data.current_radio_activity & BT_RA_RADIO_POWERED_ON )
    {
      /* If radio is not powered on yet, HCI command is going to be
       * requed till radio is powered on in any case and at that point, a 
       * consolidated event will be sent.
      */
  bt_rm_ev_send_radio_activity();
    }
  }
}

  //#ifndef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_init_hc_sec_settings_acl

DESCRIPTION
  Initializes the HC security parameters for a specified ACL connection.

===========================================================================*/
LOCAL void bt_rm_init_hc_sec_settings_acl
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_device_type*  dev_pub_ptr = &conn_ptr->dev_ptr->dev_public;

  if ( conn_ptr->bonding_app_id == BT_APP_ID_NULL )
  {
    /*  Use highest security setting between the one   */
    /*  stored for the specific remote device and the  */
    /*  default setting for the local device.          */
    if ( dev_pub_ptr->security > bt_rm_data.security )
    {
      conn_ptr->security_min = dev_pub_ptr->security;
    }
    else
    {
      conn_ptr->security_min = bt_rm_data.security;
    }
  }
  else
  {
    conn_ptr->security_min = BT_SEC_AUTHENTICATE;
  }

  if ( (conn_ptr->security_min == BT_SEC_AUTHENTICATE) ||
       (conn_ptr->security_min == BT_SEC_AUTHENTICATE_AND_ENCRYPT) )
  {
    conn_ptr->hc_auth_enable = BT_HC_AUTH_ALL_ENABLED;
  }
  else
  {
    conn_ptr->hc_auth_enable = BT_HC_AUTH_DISABLED;
  }

  if ( conn_ptr->security_min == BT_SEC_AUTHENTICATE_AND_ENCRYPT )
  {
#ifdef BT_SWDEV_2_1_SSP
    conn_ptr->encrypt_mode = BT_EM_ENABLED;
#else
    conn_ptr->encrypt_mode = BT_EM_POINT_TO_POINT;
#endif /* BT_SWDEV_2_1_SSP */
  }
  else
  {
    conn_ptr->encrypt_mode = BT_EM_DISABLED;
  }

  conn_ptr->authenticated  = FALSE;
  conn_ptr->encrypt_enable = BT_ENCRYPT_ENABLE_OFF;

  /*  Send HCI commands to initalize new ACL connection settings.  */
  BT_MSG_API( "BT RM CMD TX: HC Wr Ena A %x EM %x",
              conn_ptr->hc_auth_enable, conn_ptr->encrypt_mode, 0 );
  bt_cmd_hc_wr_auth_enable( conn_ptr->hc_auth_enable );
  bt_cmd_hc_wr_encrypt_mode( conn_ptr->encrypt_mode );

}
//#endif /* BT_SWDEV_2_1_SSP */

/*===========================================================================

FUNCTION
  bt_rm_set_conn_role_switch_req_policy

DESCRIPTION
  Sets the role switch request policy for the specified connection to
  the specified value.  This only updates the RM connection table
  link policy field.  It does NOT write the link policy to HCI.

===========================================================================*/
LOCAL void bt_rm_set_conn_role_switch_req_policy
(
  bt_rm_conn_type*  conn_ptr,
  boolean           role_switch_req_ok
)
{

  if ( role_switch_req_ok == FALSE )
  {
    conn_ptr->link_policy &= ~BT_LINK_POLICY_ROLE_SWITCH_B;
  }
  else
  {
    conn_ptr->link_policy |= BT_LINK_POLICY_ROLE_SWITCH_B;
  }

  conn_ptr->updates |= BT_RM_LU_ROLE_SW_OK_B;
  bt_rm_link_status_updated( conn_ptr );

}

/*===========================================================================

FUNCTION
  bt_rm_update_all_role_switch_req_policy

DESCRIPTION
  Updates the role switch request policy for all connections and writes the
  link policy to HCI.

===========================================================================*/
LOCAL void bt_rm_update_all_role_switch_req_policy
(
  bt_role_switch_type req_role_switch_policy
)
{
  uint16              i;
  bt_role_switch_type curr_role_switch_policy = BT_ROLE_SWITCH_NOT_ALLOWED;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].state_acl == BT_RM_CS_CONNECTED )
    {
      if ( bt_rm_conn[ i ].link_policy & BT_LINK_POLICY_ROLE_SWITCH_B )
      {
        curr_role_switch_policy = BT_ROLE_SWITCH_ALLOWED;
      }
      else
      {
        curr_role_switch_policy = BT_ROLE_SWITCH_NOT_ALLOWED;
      }

      if ( curr_role_switch_policy != req_role_switch_policy )
      {
        BT_MSG_HIGH( "BT RM: Updating role sw link policy",0,0,0 );
        bt_rm_set_conn_role_switch_req_policy( &bt_rm_conn[ i ],
                                               req_role_switch_policy );

        BT_MSG_API( "BT RM CMD TX: HC Wr LP H %x",
                    bt_rm_conn[ i ].link_policy,
                    bt_rm_conn[ i ].hc_handle_acl, 0 );
        bt_cmd_hc_wr_link_policy( bt_rm_conn[ i ].hc_handle_acl,
                                  bt_rm_conn[ i ].link_policy );
      }
    }
  }
}

/*===========================================================================

FUNCTION
  bt_rm_new_acl_connection_ok

DESCRIPTION
  Determines if a new ACL connection can be allowed.

RETURN VALUES
  BT_CS_GN_SUCCESS          If a new ACL connection can be created.
  BT_CS_GN_RETRY_CMD_LATER  If links are in transition such that the
                              determination cannot be made right now.
  BT_CS_RM_MAX_CONNECTIONS  If an ACL connection cannot be added.
  BT_CS_RM_CMD_NOT_ALLOWED  If FEATURE_BT_HOST_DIS_SCATTERNET defined
                            AND there is already a slave link.
===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_new_acl_connection_ok
(
  void
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  uint16              i;
  uint16              active_acl_connections = 0;
  uint16              sco_pkt_types_in_use = 0;
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
  uint16              slave_acls = 0;
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED )
    {
      active_acl_connections++;
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
      if (bt_rm_conn[ i ].role == BT_ROLE_SLAVE)
      {
        slave_acls++;
      }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
    }

    if ( bt_rm_conn[ i ].state_acl == BT_RM_CS_CONNECTED )
    {
      if ( (bt_rm_conn[ i ].state_sco != BT_RM_CS_DISCONNECTED) &&
           (bt_rm_conn[ i ].state_sco != BT_RM_CS_CONNECTED) )
      {
        /*  SCO connection in transition.  */
        status = BT_CS_GN_RETRY_CMD_LATER;
      }
      else if ( (bt_rm_conn[ i ].pkt_types_in_use_sco &
                  (BT_PT_HV1_B | BT_PT_HV2_B)) != 0 )
      {
        /*  SCO link using HV1 or HV2 packets.  */
        status = BT_CS_RM_MAX_CONNECTIONS;

        sco_pkt_types_in_use = bt_rm_conn[ i ].pkt_types_in_use_sco;
      }
    }
  }

  if ( active_acl_connections >= BT_MAX_DEVICES_CONNECTED )
  {
    status = BT_CS_RM_MAX_CONNECTIONS;
  }
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
  else if ( slave_acls != 0 )
  {
    if (status == BT_CS_GN_SUCCESS)
    {
      status = BT_CS_RM_CMD_NOT_ALLOWED;
    }
    BT_MSG_DEBUG( "BT RM: Existing Slv ACLs %x",slave_acls,0,0 );
  }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

  if ( (status != BT_CS_GN_SUCCESS) &&
       (status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    BT_MSG_DEBUG( "BT RM: New ACL disallowed S %x ACs %x SPT %x",
                  status, active_acl_connections, sco_pkt_types_in_use );
  }

  return ( status );

}

/*===========================================================================

FUNCTION
  bt_rm_wr_hc_page_to

DESCRIPTION
  Updates page timeout parameter to the controller.

===========================================================================*/
LOCAL void bt_rm_wr_hc_page_to
(
  void
)
{
  uint16  page_timeout;

  /*  Ensure timeout allows for minimum train repetitions.  */
  if ( bt_rm_total_active_sco_links() != 0 )
  {
    page_timeout = (2 * BT_DEFAULT_PAGE_TIMEOUT);
  }
  else
  {
    page_timeout = BT_DEFAULT_PAGE_TIMEOUT;
  }
  
  if ( bt_rm_data.page_timeout != page_timeout )
  { 
    /* Page timeout needs to be udpated to the controller.
     * Save the page timeout in RM and send HCI cmd to the controller. 
    */
    bt_rm_data.page_timeout = page_timeout;
    BT_MSG_API( "BT RM CMD TX: HC Wr Page TO %x",
                 page_timeout, 0, 0 );
    bt_cmd_hc_wr_page_to( page_timeout );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_create_connection_acl

DESCRIPTION
  Starts a specified new ACL connection.

===========================================================================*/
LOCAL void bt_rm_create_connection_acl
(
  bt_rm_conn_type*  conn_ptr
)
{

  bt_role_switch_type role_sw_req_ok = bt_rm_data.create_conn_role_sw_req_ok;

  /*  Update connection information.  */
  conn_ptr->state_acl  = BT_RM_CS_CONNECTING;

  conn_ptr->role       = BT_ROLE_MASTER;
  conn_ptr->updates   |= BT_RM_LU_ROLE_B;

  //#ifndef BT_SWDEV_2_1_SSP
  /*  Set link security parameters with HC.  */
  bt_rm_init_hc_sec_settings_acl( conn_ptr );
  //#endif /* BT_SWDEV_2_1_SSP */

#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
  if ( bt_rm_total_active_acl_links() > 1 )
  {
    /* Update link policy on existing links not to allow role SW */
    bt_rm_update_all_role_switch_req_policy( BT_ROLE_SWITCH_NOT_ALLOWED );
    BT_MSG_HIGH( "BT RM: Exisitng ACL. Disallow role sw for new",0,0,0 );
    role_sw_req_ok = BT_ROLE_SWITCH_NOT_ALLOWED;
  }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

  /*  Store role switch request policy for this link.  */
  bt_rm_set_conn_role_switch_req_policy( conn_ptr, role_sw_req_ok );

  /* Update page timeout */
  bt_rm_wr_hc_page_to();

  /*  Create the connection.  */

#ifdef FEATURE_BT_SOC
  /* Allow all packet types in case of SOC */
   conn_ptr->pkt_types_allowed_acl = BT_PT_ACL_MAX_SLOT_5_M;
   conn_ptr->max_slots_allowed_acl = 5;
#endif


  BT_MSG_API( "BT RM CMD TX: HC CrConnA PgScMd %x PTA %x RSROk %x",
              conn_ptr->dev_ptr->page_scan_mode,
              conn_ptr->pkt_types_allowed_acl,
              role_sw_req_ok );
  BT_BDA( MSG_API, "BT RM CMD TX: HC CrConnA",
          &conn_ptr->dev_ptr->dev_public.bd_addr );
  bt_cmd_hc_create_connection(
    &conn_ptr->dev_ptr->dev_public.bd_addr,
    (bt_bb_pkt_type)conn_ptr->pkt_types_allowed_acl,
    conn_ptr->dev_ptr->page_scan_rep_mode,
    conn_ptr->dev_ptr->page_scan_mode,
    conn_ptr->dev_ptr->clk_offset,
    role_sw_req_ok );

  /* Baseband will go into paging state shortly */
  bt_rm_do_bb_page_started( conn_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_update_bt_module_state

DESCRIPTION
  Processes the RM update BT module state cmd. Enumerations are defined for
  a predefined set of BT "modules". A module could be a Bluetooth profile,
  protocol entity or a management entity. At power up the state for
  all the modules is initialized to STATE_IDLE. This API can be
  called at any time after power up to update the state of identified
  module/s to STATE_IDLE or STATE_ACTIVE.
  Platform specific logic here will update the DCVS ( clock / Voltage scaling
  vote ) taking into consideration the current state of all
  the relevant modules.

===========================================================================*/
LOCAL void bt_rm_cmd_update_bt_module_state
(
  bt_cmd_msg_type*  rm_upd_bms_ptr
)
{
  BT_MSG_API( "BT RM CMD RX: Upd BT module State Module %x State %x",
              rm_upd_bms_ptr->cmd_msg.cmd_rm_upd_bms.module,
              rm_upd_bms_ptr->cmd_msg.cmd_rm_upd_bms.state, 0 );

  rm_upd_bms_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS ;
  if ( rm_upd_bms_ptr->cmd_msg.cmd_rm_upd_bms.state == BT_MODULE_ST_IDLE )
  {
    bt_rm_data.bt_module_st_mask &= (~rm_upd_bms_ptr->cmd_msg.cmd_rm_upd_bms.module);
  }
  else
  {
    /* BT_MODULE_STATE_ACTIVE */
    bt_rm_data.bt_module_st_mask |= rm_upd_bms_ptr->cmd_msg.cmd_rm_upd_bms.module;
  }

#ifdef FEATURE_BT_DCVS
  bt_vote_for_dcvs(bt_rm_data.bt_module_st_mask);
#endif

}

/*===========================================================================

FUNCTION
  bt_rm_cmd_device_update

DESCRIPTION
  Processes the RM Device Update command.

===========================================================================*/
LOCAL void bt_rm_cmd_device_update
(
  bt_cmd_msg_type*  rm_du_ptr
)
{

  bt_rm_device_type*  dev_ptr;
  bt_device_type*     new_dev_ptr;

  new_dev_ptr = &rm_du_ptr->cmd_msg.cmd_rm_dvupd.device;

  BT_MSG_API( "BT RM CMD RX: DevUpd UC %x AID %x",
              rm_du_ptr->cmd_msg.cmd_rm_dvupd.update_control,
              rm_du_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: DevUpd", &new_dev_ptr->bd_addr );

  rm_du_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( ((dev_ptr = bt_rm_get_device(
                     &new_dev_ptr->bd_addr, FALSE )) != NULL) &&
       (dev_ptr->dev_public.valid != FALSE) )
  {
    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_SECURITY_B) &&
#ifdef BT_SWDEV_2_1_SSP
         (new_dev_ptr->sec_level > BT_RM_SL_3_HIGH) )
#else
         (new_dev_ptr->security >= BT_SEC_MAX) )
#endif /* BT_SWDEV_2_1_SSP */
    {
      rm_du_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
    }
  }
  else
  {
    rm_du_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_DEVICE;
  }

  if ( rm_du_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    /*  Device update request is valid.  */

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
           update_control & BT_RM_DU_UNBOND_B) &&
         (dev_ptr->dev_public.bonded != FALSE) )
    {
      bt_rm_init_device_security( dev_ptr );
      /* Disconnect if the device is connected */
      bt_rm_disconnect_device ( dev_ptr );

      dev_ptr->update_status |= BT_RM_DUS_UNBONDED_B;
    }

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_SECURITY_B) &&
#ifdef BT_SWDEV_2_1_SSP
         (dev_ptr->dev_public.sec_level != new_dev_ptr->sec_level) )
    {
      dev_ptr->dev_public.sec_level  = new_dev_ptr->sec_level;
#else
         (dev_ptr->dev_public.security != new_dev_ptr->security) )
    {
      dev_ptr->dev_public.security  = new_dev_ptr->security;
#endif /* BT_SWDEV_2_1_SSP */
      dev_ptr->update_status       |= BT_RM_DUS_SECURITY_B;
    }

    if ( rm_du_ptr->cmd_msg.cmd_rm_dvupd.
           update_control & BT_RM_DU_NICK_NAME_B )
    {
      memcpy( (void *)dev_ptr->dev_public.nick_name_str,
              (void *)new_dev_ptr->nick_name_str,
              BT_MAX_REMOTE_NICK_NAME_LEN );

      dev_ptr->dev_public.nick_name_str[
        BT_MAX_REMOTE_NICK_NAME_LEN ]  = 0;
      dev_ptr->update_status          |= BT_RM_DUS_NICK_NAME_B;
    }

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_VALUE_1_B) &&
         (dev_ptr->dev_public.value_1 != new_dev_ptr->value_1) )
    {
      dev_ptr->dev_public.value_1  = new_dev_ptr->value_1;
      dev_ptr->update_status      |= BT_RM_DUS_VALUE_1_B;
    }

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_VALUE_2_B) &&
         (dev_ptr->dev_public.value_2 != new_dev_ptr->value_2) )
    {
      dev_ptr->dev_public.value_2  = new_dev_ptr->value_2;
      dev_ptr->update_status      |= BT_RM_DUS_VALUE_2_B;
    }

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_VALUE_3_B) &&
         (dev_ptr->dev_public.value_3 != new_dev_ptr->value_3) )
    {
      dev_ptr->dev_public.value_3  = new_dev_ptr->value_3;
      dev_ptr->update_status      |= BT_RM_DUS_VALUE_3_B;
    }

    if ( (rm_du_ptr->cmd_msg.cmd_rm_dvupd.
            update_control & BT_RM_DU_VALUE_4_B) &&
         (dev_ptr->dev_public.value_4 != new_dev_ptr->value_4) )
    {
      dev_ptr->dev_public.value_4  = new_dev_ptr->value_4;
      dev_ptr->update_status      |= BT_RM_DUS_VALUE_4_B;
    }

    if ( dev_ptr->update_status != BT_RM_DUS_NONE )
    {
      if ( bt_rm_device_updated(
             rm_du_ptr->cmd_hdr.bt_app_id, dev_ptr ) == FALSE )
      {
        rm_du_ptr->cmd_hdr.cmd_status = BT_CS_RM_EFS_ERR;

        BT_ERR( "BT RM: DevUpd EFS error! DI %x",
                dev_ptr->index, 0, 0 );
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_copy_device

DESCRIPTION
  Copies a specified device into a specified destination.

===========================================================================*/
LOCAL void bt_rm_copy_device
(
  bt_rm_device_type*  src_dev_ptr,
  bt_device_type*     dest_dev_ptr,
  uint8*              num_services_ptr,
  bt_service_type*    dest_services_ptr
)
{

  uint8  i;

  if ( src_dev_ptr != NULL )
  {
    *dest_dev_ptr = src_dev_ptr->dev_public;

    if ( num_services_ptr != NULL )
    {
      if ( *num_services_ptr > src_dev_ptr->num_services )
      {
        *num_services_ptr = src_dev_ptr->num_services;
      }

      if ( dest_services_ptr != NULL )
      {
        for ( i = 0; i < *num_services_ptr; i++ )
        {
          *dest_services_ptr++ = src_dev_ptr->service[ i ];
        }
      }
    }
  }
  else
  {
    dest_dev_ptr->valid = FALSE;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_read

DESCRIPTION
  Processes a RM Device Read command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_read
(
  bt_app_id_type    bt_app_id,
  bt_device_type*   dev_ptr,
  uint8*            num_services_ptr,
  bt_service_type*  dest_services_ptr
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  bt_rm_device_type*  src_dev_ptr;

  BT_MSG_API( "BT RM CMD RX: RM DevRd AID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM DevRd", &dev_ptr->bd_addr );

  if ( dev_ptr != NULL )
  {
    dev_ptr->valid = FALSE;

    TASKLOCK();

    if ( (src_dev_ptr =
            bt_rm_get_device( &dev_ptr->bd_addr, FALSE )) != NULL )
    {
      bt_rm_copy_device( src_dev_ptr, dev_ptr,
                         num_services_ptr, dest_services_ptr );
    }
    else
    {
      status = BT_CS_RM_NO_DEVICE;
    }

    TASKFREE();
  }
  else
  {
    status = BT_CS_RM_BAD_PARAM;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_unbond

DESCRIPTION
  Processes a RM Unbond command.

===========================================================================*/
LOCAL void bt_rm_cmd_unbond
(
  bt_cmd_msg_type*  rm_ub_ptr
)
{

  bt_cmd_msg_type  cmd;

  BT_MSG_API( "BT RM CMD RX: RM Unbond AID %x",
              rm_ub_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Unbond",
          &rm_ub_ptr->cmd_msg.cmd_rm_unbnd.bd_addr );

  cmd.cmd_msg.cmd_rm_dvupd.device.bd_addr =
    rm_ub_ptr->cmd_msg.cmd_rm_unbnd.bd_addr;

  if ( (rm_ub_ptr->cmd_hdr.cmd_status =
          bt_cmd_rm_device_read(
            bt_rm_app_id, &cmd.cmd_msg.cmd_rm_dvupd.device,
            NULL, NULL )) == BT_CS_GN_SUCCESS )
  {
    cmd.cmd_hdr.cmd_type                    = BT_CMD_RM_DEVICE_UPDATE;
    cmd.cmd_hdr.bt_app_id                   = rm_ub_ptr->cmd_hdr.bt_app_id;
    cmd.cmd_msg.cmd_rm_dvupd.update_control = BT_RM_DU_UNBOND_B;

    bt_rm_cmd_device_update( &cmd );

    rm_ub_ptr->cmd_hdr.cmd_status = cmd.cmd_hdr.cmd_status;
  }

}

#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_cmd_set_io_cap

DESCRIPTION
  Processes the RM Set IO Capability command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_io_cap
(
  bt_cmd_msg_type*  rm_sio_ptr
)
{
  // Default IO cap is set to DisplayYesNo in btdc.c
  // Override default here based on what the app sends
  BT_MSG_API( "BT RM CMD RX: RM SSP SetIOCap IOC %x",
              rm_sio_ptr->cmd_msg.cmd_rm_siocap.io_cap, 0, 0 );

  bt_dc.io_cap = rm_sio_ptr->cmd_msg.cmd_rm_siocap.io_cap;

  rm_sio_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_set_sm4

DESCRIPTION
  Processes the RM Set SM4 command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_sm4
(
  bt_cmd_msg_type*  rm_sssm4_ptr
)
{
  bt_rm_service_sec_type*  ss_ptr;

  BT_MSG_HIGH( "BT RM CMD RX: RM SSP SSM4 SIM %x UUID %x SCN %x",
              rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.service_id_method,
              rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.sdp_uuid,
              rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.rfcomm_scn );

  BT_MSG_API( "BT RM CMD RX: RM SSP SSM4 PSM %x AF/ATZ %04x AID %x",
              rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.l2cap_psm,
              (uint16)((rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.authorize_first << 8) |
                       rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.authorize),
              rm_sssm4_ptr->cmd_hdr.bt_app_id );

  BT_MSG_API( "BT RM CMD RX: RM SSP SSM4 SL %x",
              rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.sec_level, 0, 0 );

  if ( (rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.service_id_method !=
          BT_SIM_NONE) &&
       (rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id.service_id_method <
          BT_SIM_MAX) &&
#ifdef FEATURE_BT_TEST_API
#error code not present
#else
       (rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.sec_level <= BT_RM_SL_3_HIGH ) )
#endif /* FEATURE_BT_TEST_API */
  {
    if ( (rm_sssm4_ptr->cmd_hdr.cmd_status =
            bt_rm_get_service_security(
              TRUE,  /*  Can be new.  */
              rm_sssm4_ptr->cmd_hdr.bt_app_id,
              &rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.service_id,
              &ss_ptr )) == BT_CS_GN_SUCCESS )
    {
      {
        ss_ptr->sec_level =
          rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.sec_level;
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
        if ( ss_ptr->sec_level != BT_RM_SL_0_NONE )
        {
          bt_dc.default_sec_level = rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.sec_level;
        }
        ss_ptr->authorize =
          rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.authorize;
        ss_ptr->authorize_first =
          rm_sssm4_ptr->cmd_msg.cmd_rm_setsssm4.authorize_first;

        /*  Disable extended service security.  */
        ss_ptr->min_pin_len_bytes     = 0;
        ss_ptr->comb_link_key_only    = FALSE;
        ss_ptr->min_enc_key_len_bytes = 0;
      }
    }
  }
  else
  {
    rm_sssm4_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_wr_remote_oob_data

DESCRIPTION
  Processes the RM Wr Remote OOB Data command.

===========================================================================*/
LOCAL void bt_rm_cmd_wr_remote_oob_data
(
  bt_cmd_msg_type*  rm_oob_ptr
)
{
  bt_bd_addr_type    bd_addr;
  bt_rm_device_type* dev_ptr;
  uint8  index = 0;
  uint8* oob_data;
  uint8 oob_data_len, oob_data_type_len;
  uint8 oob_data_type;
  boolean oob_data_error = FALSE;

  /* OOB:  App uses this to write remote OOB data to RM */
  BT_MSG_API( "BT RM CMD RX: RM Wr Remote OOB", 0, 0, 0);

  oob_data = (uint8 *)&rm_oob_ptr->cmd_msg.cmd_rm_wro.oob_data;

  /* Extract length */
  oob_data_len = (uint16)oob_data[ index ];
  index += 2;

  if( oob_data_len < 8 )
  {
    BT_MSG_DEBUG( "BT RM CMD RX: RM Wr Rem OOB: Incorrect OOB format",
                  0, 0, 0 );
  }
  else
  {
    /* Extract BD Addr */
    memcpy ( (void *)&bd_addr,
             (void *)&oob_data[ index ],
             sizeof(bt_bd_addr_type) );
    index += sizeof(bt_bd_addr_type);

    if ( (dev_ptr = bt_rm_get_device( &bd_addr, TRUE )) != NULL )
    {
      while( index < oob_data_len )
      {
        oob_data_type_len = oob_data[index++];
        oob_data_type     = oob_data[index++];

        oob_data_type_len = oob_data_type_len - 1;

        switch( oob_data_type )
        {
          case BT_OOB_TYPE_COD:
            if( oob_data_type_len == sizeof(bt_cod_type) )
            {
              memcpy ( (void *)&dev_ptr->dev_public.class_of_device,
                       (void *)&oob_data[index],
                       sizeof(bt_cod_type) );
            }
            else
            {
              oob_data_error = TRUE;
            }
            break;

          case BT_OOB_TYPE_HASH:
            if( oob_data_type_len == sizeof(bt_sp_hash_type) )
            {
              memcpy ( (void *)dev_ptr->sp_oob_hash_c.sp_hash_c,
                       (void *)&oob_data[index],
                       sizeof(bt_sp_hash_type) );
            }
            else
            {
              oob_data_error = TRUE;
            }
            break;

          case BT_OOB_TYPE_RAND:
            if( oob_data_type_len == sizeof(bt_sp_rand_type) )
            {
              memcpy ( (void *)dev_ptr->sp_oob_rand_r.sp_rand_r,
                       (void *)&oob_data[index],
                       sizeof(bt_sp_rand_type) );
            }
            else
            {
              oob_data_error = TRUE;
            }
            break;

          default:
            BT_MSG_DEBUG( "BT RM CMD RX: RM Wr Rem OOB: UNKN OOB Type: %x",
                          oob_data_type, 0, 0 );
            break;
        }

        if( oob_data_error == TRUE )
        {
          /* The OOB data format is incorrect */
          BT_MSG_DEBUG( "BT RM CMD RX: RM Wr Rem OOB: Incorrect OOB format: %x",
                        oob_data_type, 0, 0 );
          break;
        }
        index += oob_data_type_len;
      }

      if( oob_data_error == FALSE )
      {
        dev_ptr->oob_data_present = TRUE;
      }
    }
    else
    {
      BT_MSG_DEBUG(" BT RM CMD RX: RM Wr Rem OOB: Device Not found ", 0, 0, 0);
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_user_confirmation_reply

DESCRIPTION
  Processes the RM User Confirmation  command.

===========================================================================*/
LOCAL void bt_rm_cmd_user_confirmation_reply
(
  bt_cmd_msg_type*  rm_uconfrep_ptr
)
{
  bt_rm_conn_type*  conn_ptr;

  BT_MSG_HIGH( "BT RM CMD RX: RM SSP UserConfRep %x AID %x",
              rm_uconfrep_ptr->cmd_msg.cmd_rm_uconfrep.answer,
              rm_uconfrep_ptr->cmd_hdr.bt_app_id, 0 );

  BT_BDA( MSG_API, "BT RM CMD RX: RM SSP UserConfRep",
          &rm_uconfrep_ptr->cmd_msg.cmd_rm_uconfrep.bd_addr );

  rm_uconfrep_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_uconfrep_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_uconfrep_ptr->cmd_msg.cmd_rm_uconfrep.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->pin_req_resps_pending > 0 )
    {
      if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
           (conn_ptr->pin_req_pairing_done == FALSE) )
      {
        conn_ptr->pin_req_resp_app_id = rm_uconfrep_ptr->cmd_hdr.bt_app_id;

        if ( rm_uconfrep_ptr->cmd_msg.cmd_rm_uconfrep.answer != FALSE )
        {
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP UserConfReply",0,0,0);
          bt_cmd_hc_user_cfmtn_rqst_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
        else
        {
          conn_ptr->user_reply_neg = TRUE;
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP UserConfNegReply",0,0,0);
          bt_cmd_hc_user_cfmtn_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
      }
      else
      {
        rm_uconfrep_ptr->cmd_hdr.cmd_status = BT_CS_RM_CONN_ACTIVE;
        BT_MSG_DEBUG( "BT RM: SSP Extra UCONF Rep AID %x %x D %x",
                      rm_uconfrep_ptr->cmd_hdr.bt_app_id,
                      conn_ptr->pin_req_resp_app_id,
                      conn_ptr->pin_req_pairing_done );
      }

      if ( --conn_ptr->pin_req_resps_pending == 0 )
      {
        conn_ptr->pin_req_timeout_ms_left = 0;

        if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
             (conn_ptr->pin_req_pairing_done == FALSE) )
        {
          conn_ptr->user_reply_neg = TRUE;
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP UserConfNegReply - no valid response from app",0,0,0);
          bt_cmd_hc_user_cfmtn_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );

          bt_rm_reset_idle_timeout( conn_ptr );
        }

        bt_rm_init_completed_pin_request( conn_ptr );
      }
    }
    else
    {
      rm_uconfrep_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNEXP_UCONF_REPLY;
      BT_MSG_HIGH( "BT RM: SSP Unexp UCONF Rep RH %x AID %x %x",
                    conn_ptr->rm_handle, rm_uconfrep_ptr->cmd_hdr.bt_app_id,
                    conn_ptr->pin_req_resp_app_id );
    }
  }
  else
  {
    BT_ERR( "BT RM: RM SSP UserConfRep Bad BD ADDR",0,0,0);
    rm_uconfrep_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_passkey_reply

DESCRIPTION
  Processes the RM Passkey Reply command.

===========================================================================*/
LOCAL void bt_rm_cmd_passkey_reply
(
  bt_cmd_msg_type*  rm_pkr_ptr
)
{
  bt_rm_conn_type*  conn_ptr;

  BT_MSG_HIGH( "BT RM CMD RX: RM SSP PKRep Val %x AID %x",
              rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.valid,
              rm_pkr_ptr->cmd_hdr.bt_app_id, 0 );

  BT_BDA( MSG_API, "BT RM CMD RX: RM SSP PKRep",
          &rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.bd_addr );

  rm_pkr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_pkr_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->pin_req_resps_pending > 0 )
    {
      if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
           (conn_ptr->pin_req_pairing_done == FALSE) )
      {
        conn_ptr->pin_req_resp_app_id = rm_pkr_ptr->cmd_hdr.bt_app_id;

        if ( rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.valid != FALSE )
        {
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP PKRep %d",
                       rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.passkey,0,0);
          bt_cmd_hc_user_passkey_rqst_reply( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                             rm_pkr_ptr->cmd_msg.cmd_rm_pkreply.passkey);
        }
        else
        {
          conn_ptr->user_reply_neg = TRUE;
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP PKNegRep",0,0,0);
          bt_cmd_hc_user_passkey_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
      }
      else
      {
        rm_pkr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CONN_ACTIVE;
        BT_MSG_DEBUG( "BT RM: SSP Extra PKRep AID %x %x D %x",
                      rm_pkr_ptr->cmd_hdr.bt_app_id,
                      conn_ptr->pin_req_resp_app_id,
                      conn_ptr->pin_req_pairing_done );
      }

      if ( --conn_ptr->pin_req_resps_pending == 0 )
      {
        conn_ptr->pin_req_timeout_ms_left = 0;

        if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
             (conn_ptr->pin_req_pairing_done == FALSE) )
        {
          conn_ptr->user_reply_neg = TRUE;
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP PKNegRep - no valid response from app",0,0,0);
          bt_cmd_hc_user_cfmtn_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );

          bt_rm_reset_idle_timeout( conn_ptr );
        }

        bt_rm_init_completed_pin_request( conn_ptr );
      }
    }
    else
    {
      rm_pkr_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNEXP_PASSKEY_REPLY;
      BT_MSG_HIGH( "BT RM: SSP Unexp PKRep RH %x AID %x %x",
                    conn_ptr->rm_handle, rm_pkr_ptr->cmd_hdr.bt_app_id,
                    conn_ptr->pin_req_resp_app_id );
    }
  }
  else
  {
    BT_ERR( "BT RM: RM SSP PKRep Bad BD ADDR",0,0,0);
    rm_pkr_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_keypress_notification

DESCRIPTION
  Processes the RM Keypress Notification command.

===========================================================================*/
LOCAL void bt_rm_cmd_keypress_notification
(
  bt_cmd_msg_type*  rm_kpn_ptr
)
{
  bt_rm_conn_type*  conn_ptr;

  BT_MSG_HIGH( "BT RM CMD RX: RM SSP KPNot AID %x Key %x",
               rm_kpn_ptr->cmd_hdr.bt_app_id,
               rm_kpn_ptr->cmd_msg.cmd_rm_kpn.key, 0 );

  BT_BDA( MSG_API, "BT RM CMD RX: RM SSP KPNot",
          &rm_kpn_ptr->cmd_msg.cmd_rm_kpn.bd_addr );

  rm_kpn_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_kpn_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_kpn_ptr->cmd_msg.cmd_rm_kpn.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->pin_req_resps_pending > 0 )
    {
          BT_MSG_HIGH( "BT RM CMD TX: HC SSP KPNot Key %d",
                       rm_kpn_ptr->cmd_msg.cmd_rm_kpn.key,0,0);
          bt_cmd_hc_send_keypress_notification( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                                rm_kpn_ptr->cmd_msg.cmd_rm_kpn.key );
          bt_rm_reset_idle_timeout( conn_ptr );
    }
    else
    {
      rm_kpn_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNEXP_KEYPRESS_NOTIFICATION;
      BT_MSG_HIGH( "BT RM: SSP Unexp PKRep RH %x AID %x %x",
                    conn_ptr->rm_handle, rm_kpn_ptr->cmd_hdr.bt_app_id,
                    conn_ptr->pin_req_resp_app_id );
    }
  }
  else
  {
    BT_ERR( "BT RM: RM SSP PKRep Bad BD ADDR",0,0,0);
    rm_kpn_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_rd_local_oob_data

DESCRIPTION
  Processes the RM Read Local OOB Data command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_rd_local_oob_data
(
  bt_app_id_type    bt_app_id,
  uint8*            oob_data_size,
  uint8*            oob_data
)
{
  uint8               index = 0;
  uint16               oob_opt_data_size;
  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  /* OOB: Read the OOB data from the controller */
  BT_MSG_API( "BT RM CMD RX: RD Local OOB data", 0, 0, 0);

  if ( bt_rm_data.oob_valid != FALSE )
  {
    TASKLOCK();

    if( *oob_data_size >= BT_LOCAL_OOB_DATA_SIZE )
    {
      /* Leave space for the length of the optional path */
      index += 2;

      /* BD Address */
      memcpy ( (void *)&oob_data[index],
               (void *)&bt_my_bd_addr,
               sizeof(bt_bd_addr_type) );

      index = index + sizeof(bt_bd_addr_type);

      /* Class of Device */
      oob_data[index++] = sizeof(bt_cod_type) + 1;
      oob_data[index++] = BT_OOB_TYPE_COD;
      memcpy ( (void *)&oob_data[index],
               (void *)&bt_class_of_device,
               sizeof(bt_cod_type) );

      index = index + sizeof(bt_cod_type);

      /* Hash */
      oob_data[index++] = sizeof(bt_sp_hash_type) + 1;
      oob_data[index++] = BT_OOB_TYPE_HASH;
      memcpy ( (void *)&oob_data[index],
               (void *)&bt_rm_sp_hash.sp_hash_c,
               sizeof(bt_sp_hash_type) );
      index = index + sizeof(bt_sp_hash_type);

      /* Random number */
      oob_data[index++] = sizeof(bt_sp_rand_type) + 1;
      oob_data[index++] = BT_OOB_TYPE_RAND;
      memcpy ( (void *)&oob_data[index],
               (void *)&bt_rm_sp_rand.sp_rand_r,
               sizeof(bt_sp_rand_type) );
      index = index + sizeof(bt_sp_rand_type);

      /* TODO: Do we want to put the local device name here? */

      /* Length of the optional part */
      oob_opt_data_size = index - ( sizeof(bt_bd_addr_type) + 2 );
      memcpy ( (void *)&oob_data[0],
               (void *)&oob_opt_data_size,
               sizeof(uint16) );

      *oob_data_size = index;
      /* Once read this data cannot be re-used. Mark as invalid. */
      bt_rm_data.oob_valid = FALSE;
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Rd local oob data: bad sz: %x",
                    *oob_data_size, 0, 0 );
      status = BT_CS_RM_BAD_PARAM;
    }
    TASKFREE();
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Stale OOB Data - Create Data Before Reading",
                  0, 0, 0 );
    status = BT_CS_RM_CMD_NOT_ALLOWED;
  }


  return (status);
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_create_local_oob_data

DESCRIPTION
  Processes the RM Read OOB Data command.

===========================================================================*/
LOCAL void bt_rm_cmd_create_local_oob_data
(
  bt_cmd_msg_type*  rm_roob_ptr
)
{
  /* Read the OOB data (hash, random value) from the controller */
  BT_MSG_API( "BT RM CMD RX: RD Local OOB data", 0, 0, 0);
  rm_roob_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  (void) bt_cmd_hc_rd_local_oob_data();
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_bond_ext

DESCRIPTION
  Processes the RM Bond Ext command.

===========================================================================*/
LOCAL void bt_rm_cmd_bond_ext
(
  bt_cmd_msg_type*  rm_bext_ptr
)
{
  bt_rm_conn_type*      conn_ptr;
  bt_event_reason_type  reason = BT_EVR_RM_NO_CONNECTION;


  BT_MSG_API( "BT RM CMD RX: RM Bond Ext AID %x MITM %x PINL %x",
                rm_bext_ptr->cmd_hdr.bt_app_id,
                rm_bext_ptr->cmd_msg.cmd_rm_bondext.mitm_protection_reqd,
                rm_bext_ptr->cmd_msg.cmd_rm_bondext.pin.length );

  BT_BDA( MSG_API, "BT RM CMD RX: RM SSP Bond Ext",
          &rm_bext_ptr->cmd_msg.cmd_rm_bondext.bd_addr );

  if ( (rm_bext_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            FALSE,  /*  Connection need not exist yet.  */
            TRUE,   /*  Connection must be settled.     */
            &rm_bext_ptr->cmd_msg.cmd_rm_bondext.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
    {
      if ( (rm_bext_ptr->cmd_hdr.cmd_status =
              bt_rm_new_acl_connection_ok()) == BT_CS_GN_SUCCESS )
      {
        conn_ptr->bonding_app_id  = rm_bext_ptr->cmd_hdr.bt_app_id;
        conn_ptr->bonding_pin     = rm_bext_ptr->cmd_msg.cmd_rm_bondext.pin;
        conn_ptr->bonding_key_rxd = FALSE;
        conn_ptr->mitm_protection_reqd = rm_bext_ptr->cmd_msg.cmd_rm_bondext.mitm_protection_reqd;
        conn_ptr->dedicated_bonding = TRUE;
        conn_ptr->pairing_initiated = TRUE;

        bt_rm_create_connection_acl( conn_ptr );
      }
      else
      {
        reason = BT_EVR_RM_NO_RESOURCES;
      }
    }
    else
    {
      /* The conn_ptr->state_acl has to be  BT_RM_CS_CONNECTED to reach here, 
      for CONNECTING and DISCONNECTING states the bt_rm_get_conn_bd_addr would
      have set the status to BT_CS_GN_RETRY_CMD_LATER */
      BT_MSG_API( "BT RM CMD RX: RM Bond Ext, state_acl = %x", conn_ptr->state_acl,0,0);
      BT_MSG_API( "BT RM CMD RX: RM Bond Ext, dedicated bonding on"
                  "existing ACL connection disconnect the ACL and requeue RM Bond Ext", 0,0,0);
      bt_rm_disconnect_acl(conn_ptr,BT_RMDR_USER_ENDED);
      rm_bext_ptr->cmd_hdr.cmd_status =  BT_CS_GN_RETRY_CMD_LATER;
                
     /* This block is commented out till we comprehensively resolve the
        L2CAP disconnect while bonding in progress issue. 
      conn_ptr->bonding_app_id  = rm_bext_ptr->cmd_hdr.bt_app_id;
      conn_ptr->bonding_pin     = rm_bext_ptr->cmd_msg.cmd_rm_bondext.pin;
      conn_ptr->bonding_key_rxd = FALSE;
      conn_ptr->mitm_protection_reqd = rm_bext_ptr->cmd_msg.cmd_rm_bondext.mitm_protection_reqd;

      BT_MSG_API( "BT RM CMD TX: HC SSP Auth Req H %x",
                  conn_ptr->hc_handle_acl, 0, 0 );
      bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
      conn_ptr->pairing_initiated = TRUE;
      conn_ptr->dedicated_bonding = TRUE;

      bt_rm_reset_idle_timeout( conn_ptr );
     */
    }
  }

  if ( (rm_bext_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_bext_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    bt_rm_ev_send_bond_failed( rm_bext_ptr->cmd_hdr.bt_app_id,
                               &rm_bext_ptr->cmd_msg.cmd_rm_bondext.bd_addr,
                               (bt_rm_handle_type)(-1), reason );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_determine_auth_stage1_proc

DESCRIPTION
  Determine authentication stage1 proc - numeric, passkey, just works

===========================================================================*/
LOCAL bt_rm_auth_stage1_type bt_rm_determine_auth_stage1_proc
(
  boolean           mitm_required,
  bt_rm_io_cap_type remote_io_cap,
  boolean           oob_data_present
)
{
    bt_rm_auth_stage1_type auth_stage1_proc= BT_RM_AS1_JUST_WORKS;

    BT_MSG_API( "BT RM: SSP Auth Stage1 Proc MITM %x RIOC %x LIOC %x",
                        mitm_required, remote_io_cap, bt_dc.io_cap );

    if( oob_data_present )
    {
      /* Just use OOB association model */
      BT_MSG_API( "BT RM: SSP Auth Stage1 Proc OOB %x", oob_data_present, 0, 0);
      auth_stage1_proc = BT_RM_AS1_OOB;
      return (auth_stage1_proc);
    }

    /* Determine pairing procedure - numeric, passkey, just works, OOB(TODO) */
    switch ( remote_io_cap )
    {
      case BT_RM_IOC_DISPLAY_ONLY:
        switch ( bt_dc.io_cap )
        {
          case BT_RM_IOC_DISPLAY_ONLY: /* fall through*/
          case BT_RM_IOC_DISPLAY_YES_NO: /* fall through */
            /* unauthenticated link key will be generated. No user
               interaction needed. */
          case BT_RM_IOC_NO_IO:
            auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            break;
          case BT_RM_IOC_KEYBOARD_ONLY:
            if( mitm_required == TRUE )
            {
              auth_stage1_proc = BT_RM_AS1_PASSKEY_ENTRY;
            }
            else
            {
              /* if neither side needs an authenticated link key.
                 no user interaction is needed. */
              auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            }
            break;
          default:
            BT_ERR( "BT RM: SSP Invalid Local IOCap %x",
                    bt_dc.io_cap, 0, 0 );
        }
        break;

      case BT_RM_IOC_DISPLAY_YES_NO:
        switch ( bt_dc.io_cap )
        {
          case BT_RM_IOC_DISPLAY_ONLY:
            auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            break;
          case BT_RM_IOC_DISPLAY_YES_NO:
            if( mitm_required == TRUE )
            {
              auth_stage1_proc = BT_RM_AS1_NUMERIC_COMPARISON;
            }
            else
            {
              /* if neither side needs an authenticated link key.
                 no user interaction is needed. */
              auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            }
            break;
          case BT_RM_IOC_KEYBOARD_ONLY:
            if( mitm_required == TRUE )
            {
              auth_stage1_proc = BT_RM_AS1_PASSKEY_ENTRY;
            }
            else
            {
              /* if neither side needs an authenticated link key.
                 no user interaction is needed. */
              auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            }
            break;
          case BT_RM_IOC_NO_IO:
            auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            break;
          default:
            BT_ERR( "BT RM: SSP Invalid Local IOCap %x",
                    bt_dc.io_cap, 0, 0 );
        }
        break;

      case BT_RM_IOC_KEYBOARD_ONLY:
        switch ( bt_dc.io_cap )
        {
          case BT_RM_IOC_DISPLAY_ONLY: /* fall through */
          case BT_RM_IOC_DISPLAY_YES_NO: /* fall through */
          case BT_RM_IOC_KEYBOARD_ONLY:
            if( mitm_required == TRUE )
            {
              auth_stage1_proc = BT_RM_AS1_PASSKEY_ENTRY;
            }
            else
            {
              /* if neither side needs an authenticated link key.
                 no user interaction is needed. */
              auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            }
            break;
          case BT_RM_IOC_NO_IO:
            auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
            break;
          default:
            BT_ERR( "BT RM: SSP Invalid Local IOCap %x",
                    bt_dc.io_cap, 0, 0 );
        }
        break;

      case BT_RM_IOC_NO_IO:
        auth_stage1_proc = BT_RM_AS1_JUST_WORKS;
        break;

      default:
        BT_ERR( "BT RM: SSP Invalid Remote IOCap %x",
                remote_io_cap,
                0, 0 );
    }
    BT_MSG_API( "BT RM: SSP Auth Stage1 Proc %x",
                auth_stage1_proc, 0, 0 );
    return(auth_stage1_proc);
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_authorize_rebond

DESCRIPTION
  Processes the RM Authorize Rebond command.

===========================================================================*/
LOCAL void bt_rm_cmd_authorize_rebond
(
  bt_cmd_msg_type*  rm_areb_ptr
)
{

  bt_rm_conn_type*      conn_ptr;
  bt_event_reason_type  reason = BT_EVR_RM_NO_CONNECTION;

  BT_MSG_API( "BT RM CMD RX: RM SSP ReBond AID %x",
              rm_areb_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM SSP ReBond",
          &rm_areb_ptr->cmd_msg.cmd_rm_areb.bd_addr );

  if ( (rm_areb_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            FALSE,  /*  Connection need not exist yet.  */
            TRUE,   /*  Connection must be settled.     */
            &rm_areb_ptr->cmd_msg.cmd_rm_areb.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->rebond_req_pending != TRUE )
    {
      reason = BT_EVR_RM_UNEXP_COMMAND;
      rm_areb_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNEXP_AUTHORIZE_REBOND;
      BT_ERR("BT RM CMD RX: RM SSP ReBond Unexpected!",0,0,0);
    }
    else
    {
      if ( conn_ptr->pairing_initiated == TRUE )
      {
        /* This happens when bonded link key already exists but re-pairing
           was started to upgrade the link key. The repairing resulted in
           authentication failure.
         */
        conn_ptr->rebond_req_pending = FALSE;
        if ( rm_areb_ptr->cmd_msg.cmd_rm_areb.bond_allowed != FALSE)
        {
          conn_ptr->authorized_rebond = TRUE;
          /* Start pairing */
        }
        else
        {
          /* Disconnect link - user denied re-pair */
          bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
        }
      }
      else
      {
        /* This happens when link key already exists, HCI_IO_capability_request
           event is received and BT_EV_RM_REBOND_REQUEST is sent to the app. */
        conn_ptr->rebond_req_pending = FALSE;
        if ( rm_areb_ptr->cmd_msg.cmd_rm_areb.bond_allowed != FALSE)
        {
          conn_ptr->authorized_rebond = TRUE;
          /* Send HCI_IO_capability_request{_negative}_reply */
          conn_ptr->bonding_key_rxd = FALSE;

          switch ( rm_areb_ptr->cmd_msg.cmd_rm_areb.bond_req )
          {
            case BT_RM_AB_DONT_CARE:
              conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_NO_BONDING;
              break;
            case BT_RM_AB_BOND:
              if ( (conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_DEDICATED) ||
                   (conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_AUTH_DEDICATED))
              {
                conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_DEDICATED_BONDING;
              }
              else
              {
                conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_GENERAL_BONDING;
              }
              break;
            case BT_RM_AB_MITM:
              conn_ptr->local_auth_req = BT_RM_AR_MITM_NO_BONDING;
              conn_ptr->mitm_protection_reqd = TRUE;
              break;
            case BT_RM_AB_MITM_BOND:
              if ( (conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_DEDICATED) ||
                   (conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_AUTH_DEDICATED))
              {
                conn_ptr->local_auth_req = BT_RM_AR_MITM_DEDICATED_BONDING;
              }
              else
              {
                conn_ptr->local_auth_req = BT_RM_AR_MITM_GENERAL_BONDING;
              }
              conn_ptr->mitm_protection_reqd = TRUE;
              break;
            default:
              reason = BT_EVR_RM_BAD_PARAM;
              rm_areb_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
              BT_ERR("BT RM: SSP ReBond Invalid Auth Req %x",
                     rm_areb_ptr->cmd_msg.cmd_rm_areb.bond_req, 0, 0);
          }
          if ( rm_areb_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
          {
            BT_MSG_API( "BT RM CMD TX: HC SSP IOCapRes H %x LAuthReq %x",
                            conn_ptr->hc_handle_acl, conn_ptr->local_auth_req, 0 );
            bt_cmd_hc_io_capability_response( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                              bt_dc.io_cap,
                                              conn_ptr->dev_ptr->oob_data_present,
                                              //FALSE,
                                              conn_ptr->local_auth_req );
            /* Determine if the link key will be bonded.
               We will save a link key if it was generated due to dedicated
               bonding or because MITM protection was required
            */
            conn_ptr->dev_ptr->bonded_link_key = bt_rm_determine_bonded_key( conn_ptr );
            conn_ptr->auth_stage1_proc = bt_rm_determine_auth_stage1_proc(conn_ptr->mitm_protection_reqd,
                                                                          conn_ptr->dev_ptr->io_cap,
                                                                          conn_ptr->dev_ptr->oob_data_present );
          }
        }
        else
        {
          /* App rejected rebond */
          BT_MSG_API( "BT RM CMD TX: HC SSP IOCapNegReply H %x ATZ Failure",
                      conn_ptr->hc_handle_acl, 0, 0 );
          bt_cmd_hc_io_capability_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                                  BT_BE_PAIRING_NOT_ALLOWED );
        }
      }
    }
  }

  if ( (rm_areb_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_areb_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    bt_rm_ev_send_bond_failed( rm_areb_ptr->cmd_hdr.bt_app_id,
                               &rm_areb_ptr->cmd_msg.cmd_rm_areb.bd_addr,
                               (bt_rm_handle_type)(-1), reason );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_set_sm4_debug_mode

DESCRIPTION
  Processes the RM Set SM4 Debug Mode command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_sm4_debug_mode
(
  bt_cmd_msg_type*  rm_sm4debug_ptr
)
{
  /* Enable/Disable generation of debug link key in controller */
  BT_MSG_API( "BT RM CMD RX: RM SSP SM4Debug M %x ",
              rm_sm4debug_ptr->cmd_msg.cmd_rm_sm4debug.enable_debug_mode,
              rm_sm4debug_ptr->cmd_hdr.bt_app_id, 0 );
  rm_sm4debug_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  bt_rm_data.sm4_debug_enable =
    rm_sm4debug_ptr->cmd_msg.cmd_rm_sm4debug.enable_debug_mode;
  bt_cmd_hc_wr_simple_pairing_debug_mode( bt_rm_data.sm4_debug_enable );
}

#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_rm_get_manuf_data

DESCRIPTION
 Processes an RM device get manuf data command

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_manuf_data
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  uint8*            manuf_data_size,
  uint8*            manuf_data_dest_ptr
)
{
  bt_rm_device_type*  src_dev_ptr;
  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RM CMD RX: RM Get Manuf Data %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM GetManufData", bd_addr_ptr );

  TASKLOCK();

  if( bd_addr_ptr == NULL )
  {
    /* Local Device Manuf data */
    if( *manuf_data_size >= bt_efs_params.eir_manuf_data_size )
    {
      *manuf_data_size = bt_efs_params.eir_manuf_data_size;
      memcpy( (void *)manuf_data_dest_ptr,
              (void *)bt_efs_params.eir_manuf_data,
              *manuf_data_size);
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Get mnf dt: lrg sz: %x", *manuf_data_size, 0, 0 );
      status = BT_CS_RM_BAD_PARAM;
    }
  }
  else if ( (src_dev_ptr = bt_rm_get_device( bd_addr_ptr, FALSE )) != NULL )
  {
    /* Remote device manuf data */
    if( ISBITSET( src_dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B ) &&
        ( *manuf_data_size >= src_dev_ptr->dev_public.eir_data.manuf_data_size ) )
    {
      *manuf_data_size = src_dev_ptr->dev_public.eir_data.manuf_data_size;
      memcpy( (void *)manuf_data_dest_ptr,
              (void *)&src_dev_ptr->ptr_eir_data[ src_dev_ptr->eir_manuf_data_index ],
              *manuf_data_size);
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Get mnf dt: lrg sz: %x", *manuf_data_size, 0, 0 );
      status = BT_CS_RM_BAD_PARAM;
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Get mnf dt: dev not fnd", 0, 0, 0 );
    status = BT_CS_RM_NO_DEVICE;
  }

  TASKFREE();

  return (status);
}



/*===========================================================================

FUNCTION
  bt_cmd_rm_get_uuid_list

DESCRIPTION
 Processes an RM device get uuid list command

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_uuid_list
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  bt_uuid_type      uuid_type,
  uint8             uuid_list_start_idx,
  uint8*            uuid_dest_size,
  uint16*           uuid_dest_ptr
)
{
  bt_rm_device_type*  src_dev_ptr;
  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  uint8 max_size;
  uint8 uuid_type_size = 2; /* Fixing lint Errors */

  BT_MSG_API( "BT RM CMD RX: RM Get UUID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM GetUUID", bd_addr_ptr );

  TASKLOCK();

  if ( (src_dev_ptr = bt_rm_get_device( bd_addr_ptr, FALSE )) != NULL )
  {
    if( ISBITSET( src_dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B ) &&
        ( uuid_list_start_idx < src_dev_ptr->dev_public.eir_data.num_uuids[ uuid_type ] ) )
    {
      /* UUID type size - can make a look up table for the size */
      if( uuid_type == BT_UUID_16BIT_TYPE)
      {
        uuid_type_size = sizeof(bt_sd_uuid_type);
      }
      else if( uuid_type == BT_UUID_32BIT_TYPE)
      {
        uuid_type_size = sizeof(bt_sd_uuid32_type);
      }
      else if( uuid_type == BT_UUID_128BIT_TYPE)
      {
        uuid_type_size = sizeof(bt_sd_uuid128_type);
      }

      /* Calculate max size */
      max_size = (( src_dev_ptr->dev_public.eir_data.num_uuids[ uuid_type ] - uuid_list_start_idx ) *
                    uuid_type_size );

      /* Check if app has asked for an odd size */
      if( *uuid_dest_size > max_size )
      {
        *uuid_dest_size = max_size;
      }
      else if( (*uuid_dest_size % uuid_type_size) != 0 )
      {
        *uuid_dest_size = ( (*uuid_dest_size)/uuid_type_size )  * uuid_type_size;
      }

      switch( uuid_type )
      {
        case BT_UUID_16BIT_TYPE:
          memcpy( (void *)uuid_dest_ptr,
                  (void *)&src_dev_ptr->ptr_eir_data[ src_dev_ptr->eir_uuid16_index + (uuid_list_start_idx * 2) ],
                  *uuid_dest_size );
          break;
        case BT_UUID_32BIT_TYPE:
          memcpy( (void *)uuid_dest_ptr,
                  (void *)&src_dev_ptr->ptr_eir_data[ src_dev_ptr->eir_uuid32_index + (uuid_list_start_idx * 4) ],
                  *uuid_dest_size );
          break;
        case BT_UUID_128BIT_TYPE:
          memcpy( (void *)uuid_dest_ptr,
                  (void *)&src_dev_ptr->ptr_eir_data[ src_dev_ptr->eir_uuid128_index + (uuid_list_start_idx * 16) ],
                  *uuid_dest_size );
          break;
        default:
          BT_MSG_DEBUG( "BT RM: Bad Param: UUID ty: %u,", 
                        uuid_type, 0, 0 );
          status = BT_CS_RM_BAD_PARAM;
          break;
      }
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Get UUID: idx: %x, ty: %u", uuid_list_start_idx, uuid_type, 0 );
      status = BT_CS_RM_BAD_PARAM;
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Get uuid: dev not fnd", 0, 0, 0 );
    status = BT_CS_RM_NO_DEVICE;
  }

  TASKFREE();

  return (status);
}
#endif /* BT_SWDEV_2_1_EIR */

/*===========================================================================

FUNCTION
  bt_rm_unbond_performed

DESCRIPTION
  Unbonds with a device as necessary based on the failure case.

OUTPUT
  TRUE   If unbonding of the device was performed
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_unbond_performed
(
  bt_rm_conn_type*    conn_ptr,
  bt_error_code_type  hc_status
)
{

  boolean          unbonded = FALSE;
  bt_cmd_msg_type  cmd;

  /*  If we are currently bonded with the remote device     */
  /*  and there is a positive indication of security        */
  /*  failure (not a link loss or other non-security        */
  /*  related problem) then unbond with the remote device.  */

  if ( (conn_ptr->dev_ptr->link_key_valid != FALSE) &&
       (conn_ptr->pin_code_reply_neg_sent == FALSE) )
  {
    switch ( hc_status )
    {
      case BT_BE_SUCCESS:
      {
        /*  Security was successfully enhanced - no need to re-pair.  */
        break;
      }

#ifdef BT_SWDEV_2_1_SSP
      case BT_BE_PAIRING_NOT_ALLOWED:
      case BT_BE_PAIRING_WITH_UNIT_KEY_NOT_ALLOWED:
      {
        if ( conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE )
        {
          /*  Remote side indicates it does not want to pair     */
          /*  with us... do not attempt a re-pair in this case.  */
          BT_BDA( MSG_DEBUG, "BT RM: SSP Remote no pairing %x",
                  &conn_ptr->dev_ptr->dev_public.bd_addr );
          BT_MSG_DEBUG( "BT RM: SSP Remote no pairing %x",
                        hc_status, 0, 0 );
          break;
        }
      }
      case BT_BE_AUTHENTICATION_FAILURE:
      {
        {
           /* Authentiction fails, it means either Pairing or
            * authentication or change of link key failed (SRES did not match)
            * Do not start repairing in this case.
            * Do not delete existing link key
            */
           BT_MSG_DEBUG( "BT RM: SSP not deleting link key",0, 0, 0 );
           break;
        }
      }
#else
      case BT_BE_PAIRING_NOT_ALLOWED:
      case BT_BE_PAIRING_WITH_UNIT_KEY_NOT_ALLOWED:
      {
        /*  Remote side indicates it does not want to pair     */
        /*  with us... do not attempt a re-pair in this case.  */
        BT_BDA( MSG_DEBUG, "BT RM: Remote no pairing %x",
                &conn_ptr->dev_ptr->dev_public.bd_addr );
        BT_MSG_DEBUG( "BT RM: Remote no pairing %x",
                      hc_status, 0, 0 );
        break;
      }
      case BT_BE_AUTHENTICATION_FAILURE:
      {
         /* Authentiction fails, it means either Pairing or
          * authentication or change of link key failed (SRES did not match)
          * Do not start repairing in this case.
          * Do not delete existing link key
          */
         BT_MSG_DEBUG( "BT RM: not deleting link key",0, 0, 0 );
         break;
      }
#endif /* BT_SWDEV_2_1_SSP */

      case BT_BE_KEY_MISSING:
      case BT_BE_CONN_REJECTED_SECURITY_FAILURE:
      case BT_BE_ENCRYPT_MODE_NOT_ACCEPTED:
      case BT_BE_UNIT_KEY_USED:
#ifdef FEATURE_BT_1_2
      case BT_BE_INSUFFICIENT_SECURITY:
#endif
      {
        /*  Failure appears to be a positive NAK to security.  */
        /*  Force re-pair by removing the stored link key and  */
        /*  initiating service level security again.           */

        /* ACL should not be disconnected after unbond in this */
        /* case.                                               */
        conn_ptr->no_disc_on_del_link_key = TRUE;

        BT_MSG_DEBUG( "BT RM: Forced Unbond St %x", hc_status, 0, 0 );
        BT_BDA( MSG_DEBUG, "BT RM: Forced Unbond",
                &conn_ptr->dev_ptr->dev_public.bd_addr );

        cmd.cmd_hdr.cmd_type             = BT_CMD_RM_UNBOND;
        cmd.cmd_hdr.bt_app_id            = BT_APP_ID_NULL;
        cmd.cmd_msg.cmd_rm_unbnd.bd_addr =
          conn_ptr->dev_ptr->dev_public.bd_addr;

        bt_rm_cmd_unbond( &cmd );

        conn_ptr->no_disc_on_del_link_key = FALSE;

        if ( cmd.cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
        {
          unbonded = TRUE;
        }
        else
        {
          BT_ERR( "BT RM: Bad Unbond CS %x",
                  cmd.cmd_hdr.cmd_status, 0, 0 );
        }

        break;
      }

      case BT_BE_UNKNOWN_HCI_CMD:
      case BT_BE_NO_CONNECTION:
      case BT_BE_HW_FAILURE:
      case BT_BE_PAGE_TIMEOUT:
      case BT_BE_MEMORY_FULL:
      case BT_BE_CONNECTION_TIMEOUT:
      case BT_BE_MAX_CONNECTIONS:
      case BT_BE_MAX_SCO_CONNECTIONS:
      case BT_BE_ACL_ALREADY_EXISTS:
      case BT_BE_CMD_DISALLOWED:
      case BT_BE_CONN_REJECTED_NO_RESOURCE:
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
      case BT_BE_CONN_REJECTED_PERSONAL_DEVICE:
#else /* BT 1.2 */
      case BT_BE_CONN_REJECTED_BD_ADDR:
#endif
      case BT_BE_HOST_TIMEOUT:
      case BT_BE_UNSUPPORTED_FEATURES_PARAM_VALUES:
      case BT_BE_INVALID_HCI_CMD_PARAMS:
      case BT_BE_USER_ENDED_CONNECTION:
      case BT_BE_LOW_RESOURCES:
      case BT_BE_POWER_GOING_OFF:
      case BT_BE_LOCAL_HOST_TERMINATED_CONNECTION:
      case BT_BE_REPEATED_ATTEMPTS:
      case BT_BE_UNKNOWN_LMP_PDU:
      case BT_BE_UNSUPPORTED_LMP_FEATURE:
      case BT_BE_SCO_OFFSET_REJECTED:
      case BT_BE_SCO_INTERVAL_REJECTED:
      case BT_BE_SCO_AIR_MODE_REJECTED:
      case BT_BE_INVALID_LMP_PARAMS:
      case BT_BE_UNSPECIFIED_ERROR:
      case BT_BE_UNSUPPORTED_LMP_PARAMS:
      case BT_BE_SWITCH_NOT_ALLOWED:
      case BT_BE_LMP_RESPONSE_TIMEOUT:
      case BT_BE_LMP_ERROR_TRANSACTION_COLLISION:
      case BT_BE_PDU_NOT_ALLOWED:
      case BT_BE_QOS_NOT_SUPPORTED:
      case BT_BE_INSTANT_PASSED:
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
      case BT_BE_INSUFFICIENT_RESOURCES:
#else /* BT 1.2 */
      case BT_BE_DIFFERENT_TRANSACTION_COLLISION:
      case BT_BE_QOS_UNACCEPTABLE_PARAMETER:
      case BT_BE_QOS_REJECTED:
      case BT_BE_CHANNEL_CLASSIFICATION_NOT_SUPPORTED:
      case BT_BE_PARAMETER_OUT_OF_MANDATORY_RANGE:
      case BT_BE_ROLE_SWITCH_PENDING:
      case BT_BE_RESERVED_SLOT_VIOLATION:
      case BT_BE_ROLE_SWITCH_FAILED:
#endif
      default:
      {
        /*  Failure is not a positive NAK to security...    */
        /*  may be related to link loss or other problems.  */
        /*  Do not start a re-pair in this case.            */
        break;
      }
    }
  }

  conn_ptr->pin_code_reply_neg_sent = FALSE;

  return ( unbonded );

}


/*===========================================================================

FUNCTION
  bt_rm_serv_sec_re_pair_started

DESCRIPTION
  Starts re-pairing with the remote device as necessary for service
  level security.

OUTPUT
  TRUE   If re-pairing to the remote device has been started
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_serv_sec_re_pair_started
(
  bt_rm_conn_type*    conn_ptr,
  bt_error_code_type  hc_status
)
{

  boolean  re_pair_started = FALSE;

  /*  If link security enhancement was triggered by service level   */
  /*  security and we have a link key stored for the device, the    */
  /*  enhancement would fail if the remote device no longer has a   */
  /*  valid link key for us.  Under these conditions we get either  */
  /*  get an HC Authentication Complete or Encryption Mode Change   */
  /*  event with a positive indication of failure (not a link       */
  /*  loss) and we unbond with the device and initiate service      */
  /*  level security again.  This new attempt ultimately results    */
  /*  in a PIN code request out of the RM as desired for a re-pair  */
  /*  to occur.                                                     */

  if ( bt_rm_unbond_performed( conn_ptr, hc_status ) != FALSE )
  {
    if ( bt_rm_connection_secure_enough( conn_ptr, TRUE ) == FALSE )
    {
      re_pair_started = TRUE;
    }
    else
    {
      BT_ERR( "BT RM: SSec Re-pair inconsistency.", 0, 0, 0 );
    }
  }

  BT_MSG_DEBUG( "BT RM: SSec Re-pair Y %x HC St %x",
                re_pair_started, hc_status, 0 );
  BT_BDA( MSG_DEBUG, "BT RM: SSec Re-pair",
          &conn_ptr->dev_ptr->dev_public.bd_addr );

  return ( re_pair_started );

}


/*===========================================================================

FUNCTION
  bt_rm_link_sec_re_pair_started

DESCRIPTION
  Starts re-pairing with the remote device as necessary for link
  level security.

OUTPUT
  TRUE   If re-pairing to the remote device has been started
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_link_sec_re_pair_started
(
  bt_rm_conn_type*    conn_ptr,
  bt_error_code_type  hc_status
)
{

  boolean  re_pair_started = FALSE;

  /*  If link security was triggered by link level security and    */
  /*  we have a link key stored for the device, the connection     */
  /*  would fail if the remote device no longer has a valid link   */
  /*  key for us.  Some devices detach under these conditions and  */
  /*  we get an HC Connection Complete with a positive indication  */
  /*  of failure (not a page failed/link loss) and we unbond with  */
  /*  the device and initiate the connection again.  This new      */
  /*  attempt ultimately results in a PIN code request out of the  */
  /*  RM as desired for a re-pair to occur.                        */

  /*  The condition checks here must mimic exactly the condition    */
  /*  checks just below the call to this routine within             */
  /*  bt_rm_ev_hc_conn_complete_acl() including those within        */
  /*  bt_rm_finish_bond_attempt() and bt_rm_pin_req_resp_failed().  */

  /*  The intent is to perform this unbonding only in cases where     */
  /*  the connection failure occurred because of link level security  */
  /*  failing with a currently bonded device.                         */

  if ( (conn_ptr->bonding_app_id != BT_APP_ID_NULL) ||
       ((conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL) &&
        (conn_ptr->pin_req_pairing_done == FALSE)) )
  {
    /*  Connection failure was not caused by link level  */
    /*  security with a currently bonded device.         */
    re_pair_started = FALSE;
  }
  else if ( bt_rm_unbond_performed( conn_ptr, hc_status ) != FALSE )
  {
    if ( bt_rm_new_acl_connection_ok() == BT_CS_GN_SUCCESS )
    {
      re_pair_started = TRUE;

      /*  Initiate connection again.  This time we will negatively  */
      /*  reply to the link key request forcing the PIN request as  */
      /*  desired for the re-pairing.                               */
      bt_rm_create_connection_acl( conn_ptr );
    }
  }

  BT_MSG_DEBUG( "BT RM: LSec Re-pair Y %x HC St %x",
                re_pair_started, hc_status, 0 );
  BT_BDA( MSG_DEBUG, "BT RM: LSec Re-pair",
          &conn_ptr->dev_ptr->dev_public.bd_addr );

  return ( re_pair_started );

}


/*===========================================================================

FUNCTION
  bt_rm_extract_ssr_service_class

DESCRIPTION
  Extracts the service class from the given service attribute.

===========================================================================*/
LOCAL bt_sd_uuid_type bt_rm_extract_ssr_service_class
(
  const bt_sd_srv_attr_rec_type*  attrib_ptr
)
{

  bt_sd_uuid_type  uuid = 0;

  if ( attrib_ptr->attr_value.uuid_list.num_uuid > 0 )
  {
    /*  The first service class on the list is what we want.  */
    uuid = attrib_ptr->attr_value.uuid_list.uuid[ 0 ];
    BT_MSG_DEBUG( "BT RM: SD SSR Service Class %x", uuid, 0, 0 );
  }

  return ( uuid );

}


/*===========================================================================

FUNCTION
  bt_rm_extract_ssr_psm

DESCRIPTION
  Extract the Protocol Service Multiplexer from the given service
  attribute.

===========================================================================*/
LOCAL uint16 bt_rm_extract_ssr_psm
(
  const bt_sd_srv_attr_rec_type*  attrib_ptr
)
{

  const bt_sd_proto_desc_list_type*  proto_desc_list_ptr;
  uint16                             psm = BT_L2_PSM_INVALID;
  uint8                              i;

  proto_desc_list_ptr = &(attrib_ptr->attr_value.proto_desc_list);

  BT_MSG_DEBUG( "BT RM: SD SSR PSM Num PD %x",
                proto_desc_list_ptr->num_proto_desc, 0, 0 );

  for ( i = 0; i < proto_desc_list_ptr->num_proto_desc; i++ )
  {
    if ( (proto_desc_list_ptr->proto_desc[ i ].uuid ==
            BT_SD_PROTOCOL_L2CAP) &&
         (proto_desc_list_ptr->proto_desc[ i ].num_param == 1) )
    {
      /*  PSM is always 16-bit wide.  */
      psm = (proto_desc_list_ptr->proto_desc[ i ].param[ 0 ].value & 0xFFFF);
      break;
    }
  }

  BT_MSG_DEBUG( "BT RM: SD SSR PSM %x I/NPD %x NP %x", psm,
                ((i << 8) | proto_desc_list_ptr->num_proto_desc),
                proto_desc_list_ptr->proto_desc[ i ].num_param );

  return ( psm );

}


/*===========================================================================

FUNCTION
  bt_rm_extract_ssr_server_channel

DESCRIPTION
  Extract the server channel number from the given service attribute.

===========================================================================*/
LOCAL uint8 bt_rm_extract_ssr_server_channel
(
  const bt_sd_srv_attr_rec_type*  attrib_ptr
)
{

  const bt_sd_proto_desc_list_type*  proto_desc_list_ptr;
  uint32                             mask = 0;
  uint8                              scn = 0;
  uint8                              i;

  proto_desc_list_ptr = &(attrib_ptr->attr_value.proto_desc_list);

  for ( i = 0; i < proto_desc_list_ptr->num_proto_desc; i++ )
  {
    if ( (proto_desc_list_ptr->proto_desc[ i ].uuid ==
            BT_SD_PROTOCOL_RFCOMM) &&
         (proto_desc_list_ptr->proto_desc[ i ].num_param == 1) )
    {
      switch( proto_desc_list_ptr->proto_desc[ i ].param[ 0 ].size )
      {
        case 1: /* 8-bit */
          mask = 0x000000FF;
          break;
        case 2: /* 16-bit */
          mask = 0x0000FFFF;
          break;
        default:
          mask = 0xFFFFFFFF;
          break;
      }
      scn = (proto_desc_list_ptr->proto_desc[ i ].
               param[ 0 ].value & mask);
      break;
    }
  }

  BT_MSG_DEBUG( "BT RM: SD SSR SCN %x I/NPD %04x S %x", scn,
                ((i << 8) | proto_desc_list_ptr->num_proto_desc),
                proto_desc_list_ptr->proto_desc[ i ].param[ 0 ].size );

  return ( scn );

}


/*===========================================================================

FUNCTION
  bt_rm_extract_ssr_profile_version

DESCRIPTION
  Extract the profile version from the given service attribute.

===========================================================================*/
LOCAL uint16 bt_rm_extract_ssr_profile_version
(
  const bt_sd_srv_attr_rec_type*  attrib_ptr
)
{

  const bt_sd_proto_desc_list_type*  proto_desc_list_ptr;
  uint32                             mask = 0;
  uint16                             version = 0;

  proto_desc_list_ptr = &(attrib_ptr->attr_value.proto_desc_list);

  if ( (proto_desc_list_ptr->num_proto_desc > 0) &&
       (proto_desc_list_ptr->proto_desc[ 0 ].num_param > 0) )
  {
    switch( proto_desc_list_ptr->proto_desc[ 0 ].param[ 0 ].size )
    {
      case 1: /* 8-bit */
        mask = 0x000000FF;
        break;
      case 2: /* 16-bit */
        mask = 0x0000FFFF;
        break;
      default:
        mask = 0xFFFFFFFF;
        break;
    }
    version = (proto_desc_list_ptr->proto_desc[ 0 ].param[0].value & mask);
  }

  BT_MSG_DEBUG( "BT RM: SD SSR ProfVer %x NPD %x NP/S %04x",
                version, proto_desc_list_ptr->num_proto_desc,
                ((proto_desc_list_ptr->proto_desc[ 0 ].num_param << 8) |
                    proto_desc_list_ptr->proto_desc[ 0 ].param[ 0 ].size) );

  return ( version );

}


/*===========================================================================

FUNCTION
  bt_rm_finish_service_update

DESCRIPTION
  Finish updating service information for the device.  The new service
  information should already been written to the services fields of the
  device structure.  The unused elements in the services fields are
  cleared as necessary.  The device and service files are updated
  accordingly.

===========================================================================*/
LOCAL void bt_rm_finish_service_update
(
  bt_app_id_type      bt_app_id,
  bt_rm_device_type*  dev_ptr,
  uint16              new_num_services
)
{
  uint16  i;

  if ( new_num_services != dev_ptr->num_services )
  {
    if ( new_num_services <= dev_ptr->num_services )
    {
      for ( i = new_num_services; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
      {
        bt_rm_init_service( &dev_ptr->service[ i ] );
      }
      dev_ptr->update_status |= BT_RM_DUS_SERVICES_B;
    }
    dev_ptr->num_services   = new_num_services;
    dev_ptr->update_status |= BT_RM_DUS_INTERNAL_B;
  }

  if ( (dev_ptr->dev_public.valid != FALSE) &&
       (dev_ptr->update_status != BT_RM_DUS_NONE) )
  {
    bt_rm_device_updated( bt_app_id, dev_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_sd_service_search_response

DESCRIPTION
  Processes a Service Discovery Service Search Response event.

===========================================================================*/
LOCAL void bt_rm_ev_sd_service_search_response
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type                  status;
  bt_rm_conn_type*                    conn_ptr;
  const bt_sd_srv_rec_type*           ssr_ptr;
  const bt_sd_srv_attr_rec_type*      attrib_ptr;
  uint8                               i;
  bt_ev_sd_service_search_resp_type*  ssr_ev_ptr;
  bt_service_type*                    service_ptr;
  uint8                               new_num_services;
  bt_sd_uuid_type                     new_service_class;
  uint16                              new_profile_version;
  uint8                               new_server_channel_number;

  ssr_ev_ptr = &ev_msg_ptr->ev_msg.ev_sd_service_search_resp;

  BT_MSG_API( "BT RM EV RX: SD Serv Srch Rsp C %x",
              ssr_ev_ptr->srv_rec_count, 0, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: SD Serv Srch Rsp",
          &ssr_ev_ptr->bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ssr_ev_ptr->bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    bt_rm_reset_idle_timeout( conn_ptr );

    new_num_services = 0;

    bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
                             BT_SD_ITER_ALL );

    ssr_ptr = bt_sd_srv_rec_iter.first();

    BT_BDA( MSG_API, "BT RM: SD SSR", &ssr_ev_ptr->bd_addr );
    BT_MSG_DEBUG( "BT RM: SD SSR Serv Rec Cnt %x",
                  ssr_ev_ptr->srv_rec_count, 0, 0 );

    while ( ssr_ptr != NULL )
    {
      BT_MSG_DEBUG( "BT RM: SD SSR Serv %x Attr Cnt %x",
                    new_num_services, ssr_ptr->num_srv_attr, 0 );

      if ( ssr_ptr->num_srv_attr > 0 )
      {
        if ( new_num_services < BT_SD_MAX_NUM_OF_SRV_REC )
        {
          service_ptr = &conn_ptr->dev_ptr->service[ new_num_services ];

          for ( i = 0; i < ssr_ptr->num_srv_attr; i++ )
          {
            attrib_ptr  = &ssr_ptr->srv_attr[ i ];

            BT_MSG_DEBUG( "BT RM: SD SSR Attr %x ID %x",
                          i, ssr_ptr->srv_attr[ i ].attr_id, 0 );

            switch ( ssr_ptr->srv_attr[ i ].attr_id )
            {
              case BT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST:
                {
                  if ( (new_service_class =
                          bt_rm_extract_ssr_service_class( attrib_ptr )) !=
                              service_ptr->service_class )
                  {
                    service_ptr->service_class        = new_service_class;
                    conn_ptr->dev_ptr->update_status |= BT_RM_DUS_SERVICES_B;
                  }
                  break;
                }
              case BT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST:
                {
                  (void)bt_rm_extract_ssr_psm( attrib_ptr );

                  if ( (new_server_channel_number =
                          bt_rm_extract_ssr_server_channel( attrib_ptr )) !=
                              service_ptr->server_channel_number )
                  {
                    service_ptr->server_channel_number =
                      new_server_channel_number;
                    conn_ptr->dev_ptr->update_status |= BT_RM_DUS_SERVICES_B;
                  }
                  break;
                }
              case BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST:
                {
                  if ( (new_profile_version =
                          bt_rm_extract_ssr_profile_version( attrib_ptr )) !=
                            service_ptr->profile_version )
                  {
                    service_ptr->profile_version      = new_profile_version;
                    conn_ptr->dev_ptr->update_status |= BT_RM_DUS_SERVICES_B;
                  }
                  break;
                }
            }
          }
          new_num_services++;
        }
        else
        {
          BT_MSG_DEBUG( "BT RM: SD SSR Serv Rec Ignored Max %x DI %x",
                        conn_ptr->dev_ptr->num_services,
                        conn_ptr->dev_ptr->index, 0 );
        }
      }
      ssr_ptr = bt_sd_srv_rec_iter.next();
    }

    bt_rm_finish_service_update( BT_APP_ID_NULL, conn_ptr->dev_ptr,
                                 new_num_services );
  }
  else
  {
    BT_ERR( "BT RM: Bad SD Serv Srch Rsp St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad SD Serv Srch Rsp", &ssr_ev_ptr->bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_sd_timeout_response

DESCRIPTION
  Processes a Service Discovery Timeout Response event.

===========================================================================*/
LOCAL void bt_rm_ev_sd_timeout_response
(
  bt_ev_sd_timeout_resp_type*  sdto_ev_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_BDA( MSG_API, "BT RM EV RX: SD TO Resp", &sdto_ev_ptr->bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &sdto_ev_ptr->bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) != BT_CS_GN_SUCCESS )
  {
    BT_ERR( "BT RM: Bad SD TO Resp St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad SD TO Resp", &sdto_ev_ptr->bd_addr );
  }

}


#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_ev_send_oob_local_data

DESCRIPTION
  Sends a RM OOB Local Data event.

===========================================================================*/
LOCAL void bt_rm_ev_send_oob_local_data
(
  void
)
{
  bt_ev_msg_type  ev_rm_olb;
  uint8           i;

  ev_rm_olb.ev_hdr.ev_type = BT_EV_RM_OOB_LOCAL_DATA;

  /* Fixed OOB Data Size for now. This will change if
     we include the local name or add anything else as
     part of the OOB */
  ev_rm_olb.ev_msg.ev_rm_oob_ld.size_oob_data = BT_LOCAL_OOB_DATA_SIZE;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL)
    {
      ev_rm_olb.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      BT_MSG_HIGH( "BT RM EV TX: OOB local data %x",
               ev_rm_olb.ev_hdr.bt_app_id, 0, 0 );

      bt_ec_send_event( &ev_rm_olb );
    }
  }
}
#endif /* BT_SWDEV_2_1_SSP */


/*===========================================================================

FUNCTION
  bt_rm_ev_sd_error_response

DESCRIPTION
  Processes a Service Discovery Error Response event.

===========================================================================*/
LOCAL void bt_rm_ev_sd_error_response
(
  bt_ev_sd_error_resp_type*  sde_ev_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM EV RX: SD Err Resp EC %x",
              sde_ev_ptr->error_code, 0, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: SD Err Resp", &sde_ev_ptr->bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &sde_ev_ptr->bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: No Conn SD Err Resp St %x", status, 0, 0 );
    BT_BDA( MSG_DEBUG, "BT RM: No Conn SD Err Resp",
            &sde_ev_ptr->bd_addr );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_auth_complete

DESCRIPTION
  Processes a HC Authentication Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_auth_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;
  uint16              hc_handle;
  boolean             re_pair_started = FALSE;

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_authc.status);
  hc_handle = ev_msg_ptr->ev_msg.ev_hc_authc.conn_hndl;

  BT_MSG_API( "BT RM EV RX: HC AuthC S %x H %x",
              hc_status, hc_handle, 0 );

  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
    /* If the authentication was not initiated via RM bond api, 
     * send bond failed, if auth has failed with any status other
     * than "key missing".
     */
    if ( ( hc_status != BT_BE_SUCCESS ) &&
         ( hc_status != BT_BE_KEY_MISSING ) &&
         ( conn_ptr->bonding_app_id == BT_APP_ID_NULL )
       ) 
    {
      BT_MSG_DEBUG( "BT RM: Sending bond failed ", 0 , 0, 0 );
      bt_rm_ev_send_bond_failed_all_bondable(conn_ptr, 
        (BT_HCI_REASON( hc_status )) );
    }

#ifdef BT_SWDEV_2_1_SSP
    //conn_ptr->pairing_initiated = FALSE;
    conn_ptr->local_auth_req    = BT_RM_AR_NO_MITM_NO_BONDING;
    conn_ptr->remote_auth_req   = BT_RM_AR_NO_MITM_NO_BONDING;
    conn_ptr->mitm_protection_reqd = FALSE;
    //conn_ptr->dedicated_bonding = FALSE;

    if ( conn_ptr->sec_level_request != BT_RM_SL_0_NONE )
    {
      conn_ptr->sec_level_request = BT_RM_SL_0_NONE;
#else
    if ( conn_ptr->security_request == BT_SEC_AUTHENTICATE )
    {
      conn_ptr->security_request = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_2_1_SSP
      if ((conn_ptr->user_reply_neg != FALSE) ||
          (re_pair_started =
              bt_rm_serv_sec_re_pair_started( conn_ptr,
                                              hc_status )) == FALSE )
      {
        conn_ptr->user_reply_neg = FALSE;
        bt_rm_update_conn_hc_error( conn_ptr, hc_status );
      }
#else
      if ((re_pair_started =
              bt_rm_serv_sec_re_pair_started( conn_ptr,
                                              hc_status )) == FALSE )
      {
        bt_rm_update_conn_hc_error( conn_ptr, hc_status );
      }
#endif /* BT_SWDEV_2_1_SSP */
    }

    if ( re_pair_started == FALSE )
    {
#ifdef BT_SWDEV_2_1_SSP
      conn_ptr->pairing_initiated = FALSE;
      //conn_ptr->dedicated_bonding = FALSE;
      conn_ptr->re_pair_attempts = 0;
      conn_ptr->rebond_req_pending = FALSE;
      conn_ptr->authorized_rebond  = FALSE;

      if ( hc_status != BT_BE_SUCCESS )
      {
        conn_ptr->authenticated = FALSE;
        conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
        BT_MSG_DEBUG( "BT RM: Auth failed S %x H %x",
                      hc_status, hc_handle, 0 );
        bt_rm_init_conn_pin_request( conn_ptr );
      }
      else
      {
        conn_ptr->authenticated = TRUE;
        if((conn_ptr->enh_enc_state == BT_RM_ENH_ENC_AUTHENTICATING) &&
           (conn_ptr->encrypt_enable!=TRUE))
        {
           /* RM is trying to do re-pairing while encryption was enabled on link
            * so now needs to enable encryption. */
           BT_MSG_DEBUG("Enabling encryption enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
           conn_ptr->enh_enc_state = BT_RM_ENH_ENC_STARTED;
           bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                             BT_ENCRYPT_ENABLE_ON );
        }
        else
        {
          /* This is needed when a link is already encrypted with an unauthenticated
           * link key and a service request needed an authenticated link key. In this
           * case, since the link is already encrypted, bt_cmd_hc_set_conn_encrypt is
           * not needed. Refresh encryption key here? */
          BT_MSG_DEBUG("Done enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
          conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
        }
      }
#else
      if ( hc_status != BT_BE_SUCCESS )
      {
        conn_ptr->authenticated = FALSE;
        BT_MSG_DEBUG( "BT RM: Auth failed S %x H %x",
                      hc_status, hc_handle, 0 );
      }
      else
      {
        conn_ptr->authenticated = TRUE;
      }

      if( ( conn_ptr->enh_enc_state == BT_RM_ENH_ENC_AUTHENTICATING ) ||
          ( ( conn_ptr->security_min == BT_SEC_AUTHENTICATE_AND_ENCRYPT ) &&
            ( hc_status == BT_BE_SUCCESS ) ) )
      {
         /* RM is trying to do re-pairing while encryption was enabled on link
          * so now needs to enable encryption. */
         BT_MSG_DEBUG("Enabling encryption enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
         conn_ptr->enh_enc_state = BT_RM_ENH_ENC_STARTED;
         bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                           BT_ENCRYPT_ENABLE_ON );
      }
      else
      {
          BT_MSG_DEBUG("Done enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
          conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
      }
#endif /* BT_SWDEV_2_1_SSP */

      bt_rm_finish_bond_attempt( conn_ptr );

      bt_rm_security_updated( conn_ptr );

      bt_rm_link_status_updated( conn_ptr );

#ifdef BT_SWDEV_2_1_SSP
      if ((conn_ptr->enh_enc_state == BT_RM_ENH_ENC_OK) ||
          (hc_status != BT_BE_SUCCESS))
      {
        /* Send security result event in two cases:
           1. authentication is complete and encryption is being enabled
           2. authentication fails and re-pairing is not initiated
        */
        bt_rm_finish_service_security( conn_ptr );
      }
    }
#else
      if(conn_ptr->enh_enc_state == BT_RM_ENH_ENC_OK)
      {
        bt_rm_finish_service_security( conn_ptr );
      }
    }
#endif /* BT_SWDEV_2_1_SSP */
#ifdef BT_SWDEV_2_1_SSP
    //TODO
    /*
    if ( hc_status != BT_BE_SUCCESS )
    {
        // Link key already exists. Check if rebonding is ok.
        if ( conn_ptr->dev_ptr->bonded_link_key == TRUE )
        {
          // Re-bond if bonded link key exists
          conn_ptr->authenticated = FALSE;
          BT_MSG_DEBUG( "BT RM: Auth failed - Send Rebond Req S %x H %x",
                        hc_status, hc_handle, 0 );
          conn_ptr->rebond_req_pending = TRUE;
          bt_rm_ev_send_rebond_request( &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
        else
        {
          // Automatically start pairing
          conn_ptr->authenticated = FALSE;
          BT_MSG_DEBUG( "BT RM: Auth failed - Start Repairing S %x H %x",
                        hc_status, hc_handle, 0 );
          bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
        }
    }
    */
#endif /* BT_SWDEV_2_1_SSP */
  }
  else
  {
    BT_ERR( "BT RM: Bad AuthC H %x S %x",
            hc_handle, hc_status, 0 );
  }

}


/*==========================================================================

FUNCTION
  bt_rm_ev_proc_hc_rd_buf_size

DESCRIPTION
  Processes the response to the HCI Read Buffer Size command

==========================================================================*/
void bt_rm_ev_proc_hc_rd_buf_size
(
  const bt_ev_msg_type*  ev_msg_ptr
)
{
  /* Convenience pointer */
  const bt_hc_rp_rd_buffer_size_type  *rd_buf_size_ev;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rd_buf_size_ev =
    &ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_buffer_size;

  if ( ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.rd_buffer_size.status ==
       BT_BE_SUCCESS )
  {
#ifdef FEATURE_BT_SOC
    bt_hc_drv_set_hci_buf_sizes( rd_buf_size_ev->hc_acl_data_pkt_len,
                                 rd_buf_size_ev->hc_sco_data_pkt_len,
                                 rd_buf_size_ev->hc_total_num_acl_data_pkts,
                                 rd_buf_size_ev->hc_total_num_sco_data_pkts );
#endif /* FEATURE_BT_SOC */

    BT_MSG_HIGH( "BT RM: Read Buf Siz resp St %x, ACL_len %x, Num_ACL %x",
                  rd_buf_size_ev->status,
                  rd_buf_size_ev->hc_acl_data_pkt_len,
                  rd_buf_size_ev->hc_total_num_acl_data_pkts );
  }
  else
  {
    /* Failure to get buffer sizes */
    BT_ERR( "BT RM: HCI Read Buffer size failed error: %lX",
             ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt.ret_params.status.status,
             0, 0 );
  }
} /* bt_rm_ev_proc_hc_rd_buf_size */


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_command_complete

DESCRIPTION
  Processes a HC Command Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_command_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  dsm_item_type*            dsm_ptr = NULL;
  bt_ev_hc_cmd_cmplt_type*  cmd_cmplt_ptr;

  cmd_cmplt_ptr = &ev_msg_ptr->ev_msg.ev_hc_cmd_cmplt;

  BT_MSG_API( "BT RM EV RX: HC Cmd Cmplt Op %2yyx",
              cmd_cmplt_ptr->cmd_opcode, 0, 0 );

  switch ( cmd_cmplt_ptr->cmd_opcode )
  {
    case BT_HCI_RD_LOCAL_NAME:
      {
        dsm_ptr = cmd_cmplt_ptr->ret_params.rd_local_name.dsm_ptr;
        break;
      }

    case BT_HCI_RD_CURRENT_IAC_LAP:
      {
        dsm_ptr = cmd_cmplt_ptr->ret_params.rd_curr_iac_lap.dsm_ptr;
        break;
      }

    case BT_HCI_INQUIRY_CANCEL:
      {
        if ( bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL )
        {
          bt_rm_ev_send_inquiry_complete(
            bt_rm_data.inq_bt_app_id,
            bt_rm_data.inq_responses );

          bt_rm_init_inquiry_data();

          bt_dc.inq_active = FALSE;

          /* Send RM radio activity event */
          BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQUIRING );
          bt_rm_ev_send_radio_activity();
		  
          if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
             (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
             (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
		  {
			 /* Re-evaluate required scans */
             bt_rm_check_scan_settings();
          }
        }
        else
        {
          BT_ERR( "BT RM EV RX: Unexp HC CmdC Inq Cancel", 0, 0, 0 );
        }
        break;
      }

    case BT_HCI_RD_BUFFER_SIZE:
      {
        bt_rm_ev_proc_hc_rd_buf_size( ev_msg_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_EIR
    case BT_HCI_RD_INQ_RSP_TX_PWR:
      BT_MSG_DEBUG( "BT RM EV RX: Rd Inq Rsp Tx pwr",
                    cmd_cmplt_ptr->cmd_opcode, 0, 0 );
      if( cmd_cmplt_ptr->ret_params.rd_inq_rsp_tx_pwr.status == 0 )
      {
        bt_rm_device_eir.tx_power_level = cmd_cmplt_ptr->ret_params.rd_inq_rsp_tx_pwr.tx_power_level;
        bt_rm_device_eir.tx_power_level_valid = TRUE;
      }
      /* Tx power is read from controller in bt_soc_etc_init (HCI reset)
         We update EIR here */
      bt_rm_update_eir();
      break;

    case BT_HCI_WR_EXT_INQ_RSP:
      BT_MSG_DEBUG( "BT RM EV RX: Wr Inq Rsp Cmpl",
                    cmd_cmplt_ptr->cmd_opcode, 0, 0 );
      break;
    case BT_HCI_WR_INQ_TX_PWR:
      BT_MSG_DEBUG( "BT RM EV RX: Wr Inq Tx pwr cmpl",
                    cmd_cmplt_ptr->cmd_opcode, 0, 0 );
      break;
    case BT_HCI_WR_INQUIRY_MODE:
      BT_MSG_DEBUG( "BT RM EV RX: Wr Inq mode cmpl",
                    cmd_cmplt_ptr->cmd_opcode, 0, 0 );
      break;
    case BT_CMD_HC_SET_EVENT_MASK:
      BT_MSG_DEBUG( "BT RM EV RX: Set evnt mask cmpl",
                    cmd_cmplt_ptr->cmd_opcode, 0, 0 );
      break;
#endif /* BT_SWDEV_2_1_EIR */

    case BT_HCI_HOST_BUFFER_SIZE:
      {
        break;
      }

    case BT_HCI_SET_HC_TO_HOST_FLOW_CONTROL:
      {
#ifdef FEATURE_BT_HCI_HOST_FCTL
        bt_hc_drv_hc2h_flow_support_status(
          cmd_cmplt_ptr->ret_params.status.status );
#endif /* FEATURE_BT_HCI_HOST_FCTL */
        break;
      }

#ifdef FEATURE_BT_SOC
    case BT_HCI_RD_LOCAL_SUPPORTED_FEAT:
      bt_soc_etc_proc_local_feat_ev(ev_msg_ptr);
      break;

    case BT_HCI_RD_LOCAL_VERSION_INFO:
      bt_soc_etc_proc_local_ver_ev(ev_msg_ptr);
      break;

    case BT_HCI_RD_RSSI:
      {
        bt_rm_conn_type*        conn_ptr;
        bt_hc_rp_rd_rssi_type*  rssi_cc_ptr =
                                &cmd_cmplt_ptr->ret_params.rd_rssi;

        conn_ptr = bt_rm_get_acl_conn_hc_handle( rssi_cc_ptr->conn_hndl );

        BT_MSG_DEBUG( "BT RM: RD RSSI CC S %x H %x R %x", rssi_cc_ptr->status,
                       rssi_cc_ptr->conn_hndl, rssi_cc_ptr->rssi );
        if (( rssi_cc_ptr->status == BT_BE_SUCCESS ) && ( conn_ptr != NULL ))
        {
          bt_rm_update_rssi( conn_ptr, rssi_cc_ptr->rssi );
        }
      }
      break;

#endif /* FEATURE_BT_SOC */

    case BT_HCI_WR_AUTO_FLUSH_TIMEOUT:
    case BT_HCI_WR_LINK_POLICY_SETTINGS:
#ifndef FEATURE_BT_SOC
    case BT_HCI_SET_INCOMING_SNIFF_PARAMS:
    case BT_HCI_SET_INCOMING_HOLD_PARAMS:
    case BT_HCI_SET_INCOMING_PARK_PARAMS:
#endif /* !FEATURE_BT_SOC */
    case BT_HCI_WR_AUTHENTICATION_ENABLE:
    case BT_HCI_WR_ENCRYPTION_MODE:
    case BT_HCI_LINK_KEY_RQST_REPLY:
    case BT_HCI_LINK_KEY_RQST_NEG_REPLY:
    case BT_HCI_PIN_CODE_RQST_REPLY:
    case BT_HCI_PIN_CODE_RQST_NEG_REPLY:
    case BT_HCI_WR_CURRENT_IAC_LAP:
    case BT_HCI_WR_PAGESCAN_ACTIVITY:
    case BT_HCI_WR_INQUIRYSCAN_ACTIVITY:
    case BT_HCI_WR_PAGE_TIMEOUT:
    case BT_HCI_WR_CLASS_OF_DEVICE:
    case BT_HCI_CHANGE_LOCAL_NAME:
    case BT_HCI_ENABLE_DUT_MODE:
    case BT_HCI_SET_EVENT_FILTER:
    case BT_HCI_WR_LINK_SUPERVISION_TIMEOUT:
    case BT_HCI_FLUSH:
#ifdef FEATURE_BT_1_2
    case BT_HCI_WR_INQUIRY_SCAN_TYPE:
    case BT_HCI_WR_PAGE_SCAN_TYPE:
#endif /* FEATURE_BT_1_2 */
#ifdef BT_SWDEV_2_1_SSP
    case BT_HCI_IO_CAPABILITY_RQST_REPLY:
    case BT_HCI_USER_CFMTN_RQST_REPLY:
    case BT_HCI_USER_CFMTN_RQST_NEG_REPLY:
    case BT_HCI_USER_PASSKEY_RQST_REPLY:
    case BT_HCI_USER_PASSKEY_RQST_NEG_REPLY:
    case BT_HCI_REMOTE_OOB_DATA_RQST_REPLY:
    case BT_HCI_REMOTE_OOB_DATA_RQST_NEG_REPLY:
    case BT_HCI_IO_CAPABILITY_RQST_NEG_REPLY:
    case BT_HCI_WR_SIMPLE_PAIRING_DEBUG_MODE:
    case BT_HCI_SEND_KEYPRESS_NOTIFICATION:
#endif /* BT_SWDEV_2_1_SSP */
#ifdef FEATURE_BT_2_1
    case BT_HCI_RD_DEFAULT_ERR_DATA_REPORTING:
    case BT_HCI_WR_DEFAULT_ERR_DATA_REPORTING:
#endif  /* FEATURE_BT_2_1 */
      {
        break;
      }

#ifdef BT_SWDEV_2_1_SSR
    case BT_HCI_SNIFF_SUBRATING:
      {
        BT_MSG_DEBUG( "BT RM EV RX: SSR cmd cmpl Op %x S %x",
                      cmd_cmplt_ptr->cmd_opcode,
                      cmd_cmplt_ptr->ret_params.conn_hndl.status, 0 );
        break;
      }
#endif /* BT_SWDEV_2_1_SSR */


#ifdef BT_SWDEV_2_1_SSP
    case BT_HCI_RD_LOCAL_OOB_DATA:
      /* Copy into local data structures */
      memcpy ( (void *)&bt_rm_sp_hash.sp_hash_c,
               (void *)&cmd_cmplt_ptr->ret_params.oob_data.sp_hash,
               sizeof(bt_sp_hash_type) );
      memcpy ( (void *)&bt_rm_sp_rand.sp_rand_r,
               (void *)&cmd_cmplt_ptr->ret_params.oob_data.sp_rand,
               sizeof(bt_sp_rand_type) );
      bt_rm_data.oob_valid = TRUE;
      bt_rm_ev_send_oob_local_data();
      break;
#endif /* BT_SWDEV_2_1_SSP */

    case BT_HCI_WR_SCAN_ENABLE:
      {
         if ( bt_dc.disable_scan_pending )
         {
           bt_dc.scan_active = FALSE;
           bt_dc.disable_scan_pending = FALSE;
         }
         break;
      }
#ifdef FEATURE_BT_SOC

#ifdef FEATURE_BT_QSOC
    case BT_HCI_QSOC_VS_CMD_GROUP:
      /* Call SoC interface handler to handle Vendor Specific events */
      bt_qsoc_process_vs_cmd_cmplt_event(ev_msg_ptr);
      break;
#endif /* FEATURE_BT_QSOC */

    case BT_HCI_RESET:
      {
        /* Only needed for SoC1 */
        if ( bt_soc_type == BT_SOC_TYPE_SOC1 )
        {
          if (( bt_dc.radio_state == BT_RADIO_DISABLING ) ||
              ( bt_dc.radio_state == BT_RADIO_DISABLED ))
          {
            BT_MSG_HIGH( "BT RM: Turning Radio off after HCI reset", 0, 0, 0 );

#ifndef FEATURE_BT_SOC1_FLASH
            /* cannot power cycle SOC1 flash part on SURF */
            bt_dc_disable_driver();
#endif /* !FEATURE_BT_SOC1_FLASH */

            bt_dc.radio_state = BT_RADIO_DISABLED;

            if ( bt_rm_hwerr_radio_off == TRUE )
            {
              bt_cmd_msg_type      radio_on_off_cmd;

              BT_MSG_HIGH( "BT RM: Attempting recovery from HW err. Radio on now", 0, 0, 0 );

              bt_rm_hwerr_radio_off = FALSE;
              bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;
              radio_on_off_cmd.cmd_hdr.bt_app_id = BT_APP_ID_NULL;
              radio_on_off_cmd.cmd_hdr.cmd_type  = BT_CMD_RM_DISABLE_RADIO;

              /* Turning radio-on after it was turned off due SOC HW error */
              radio_on_off_cmd.cmd_msg.cmd_rm_dsrad.disable = FALSE;
              bt_rm_cmd_disable_radio( &radio_on_off_cmd );
            }
          }
        }
        else
        {
#ifdef FEATURE_BT_QSOC

          if (( bt_dc.radio_state == BT_RADIO_DISABLING ) ||
              ( bt_dc.radio_state == BT_RADIO_DISABLED ))
          {
              bt_dc_disable_driver();
              bt_dc.radio_state = BT_RADIO_DISABLED;
            /*
             * If the HCI_RST Event is recieved in HCI APIT Test mode
             * due to a HCI_RST command sent from a Test host,then we
             * need to follow the radio-off with a radio-on command.
             */
              if ( bt_hc_pe_is_hci_rst_started() == TRUE && BT_QSOC_IS_READY() )
            {
              bt_cmd_msg_type      radio_on_off_cmd;

              radio_on_off_cmd.cmd_hdr.bt_app_id = BT_APP_ID_NULL;
              radio_on_off_cmd.cmd_hdr.cmd_type  = BT_CMD_RM_DISABLE_RADIO;
              radio_on_off_cmd.cmd_msg.cmd_rm_dsrad.disable = FALSE;

              /* Switch on the radio again in HCI Test Mode*/
              bt_rm_disable_radio( &radio_on_off_cmd );
              bt_hc_pe_hci_rst_done();
            }
          }
          else

#endif /* FEATURE_BT_QSOC */
          {
            /* Received during initialization, let SoC code handle it */
            if ( bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr != NULL )
            {
              bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr(ev_msg_ptr);
            }
            /* SOC is now powered on and initialized
             * Update radio activity and send event notification
             * Do not use BT_RM_UPD_RA_AND_CLRBIT macro as BT_RA_RADIO_IDLE 
             * is not changed here.
            */
            bt_rm_data.current_radio_activity |= BT_RA_RADIO_POWERED_ON;
            bt_rm_ev_send_radio_activity();
          }
        }
        break;
      }
#endif
#ifdef FEATURE_BT_1_2
    case BT_HCI_SET_AFH_HOST_CHAN_CLASS:
      {
        BT_MSG_DEBUG( "BT RM EV RX: HC CmdC Set AFH Ch Class S %x", 
                       cmd_cmplt_ptr->ret_params.status.status, 0, 0 );
        break;
      }

    case BT_HCI_READ_AFH_CHAN_MAP:
      {
        BT_ERR( "BT RM EV RX: Unexp HC CmdC Rd AFH Ch S %x",
                 cmd_cmplt_ptr->ret_params.status.status, 0, 0 );
        break;
      }

      /* Nothing to do here for cancel. If the Cancel succeeded, controller
       * still sends a connection complete event with error code
       * 0x2 = Unknown Connection Id. Error will be propagated to the app
       * in corresponding event.
      */
    case BT_HCI_CREATE_CONNECTION_CANCEL:
      {
        BT_MSG_DEBUG( "BT RM EV RX: Create Conn Cancel S %x ",
                       cmd_cmplt_ptr->ret_params.status.status, 0, 0 );
        BT_BDA( MSG_DEBUG, "BT RM: Create Conn Cancel",
                    &cmd_cmplt_ptr->ret_params.bd_addr.bd_addr );

        break;
      }

    case BT_HCI_REMOTE_NAME_RQST_CANCEL:
      {
        BT_MSG_DEBUG( "BT RM EV RX: Remote Name Req Cancel S %x ",
                       cmd_cmplt_ptr->ret_params.status.status, 0, 0 );
        BT_BDA( MSG_DEBUG, "BT RM: Remote Name Req Cancel",
                    &cmd_cmplt_ptr->ret_params.bd_addr.bd_addr );
        break;
      }

#ifndef FEATURE_BT_SOC
    case BT_HCI_CONTROL_AFH:
      {
        break;
      }
#endif /* !FEATURE_BT_SOC */
#endif /* FEATURE_BT_1_2 */

    default:
      {
        BT_ERR( "BT RM EV RX: Unexp HC CmdC OC %x",
                cmd_cmplt_ptr->cmd_opcode, 0, 0 );
        break;
      }
  }

  dsm_free_packet( &dsm_ptr );

}

/*===========================================================================

FUNCTION
  bt_rm_remote_name_complete_processing

DESCRIPTION
  This routine is called from remote name complete and cmd status event  
  handlers for remote name request. Name request response is propagated to 
  upper layers if required and operations that were on hold pending name 
  request will be triggered from here.

===========================================================================*/
LOCAL void bt_rm_remote_name_complete_processing
(
  bt_rm_conn_type*    conn_ptr,
  bt_cmd_status_type  status
)
{
  uint16              i;
  bt_ev_msg_type      ev_rm_pcr;

  bt_rm_finish_remote_name_request( conn_ptr, status );

#ifdef FEATURE_BT_SOC
  if ( ( conn_ptr->bonding_app_id != BT_APP_ID_NULL ) &&
       ( conn_ptr->send_pin_req_pending == FALSE ) )
  {
      bt_rm_finish_bond_attempt( conn_ptr );
  }
#endif

   /* Finish sending pin code request */
  if( conn_ptr->send_pin_req_pending == TRUE )
  {
    conn_ptr->pin_code_reply_len_bytes = 0;
    conn_ptr->pin_code_reply_neg_sent  = FALSE;

    ev_rm_pcr.ev_hdr.ev_type = BT_EV_RM_PIN_REQUEST;
    ev_rm_pcr.ev_msg.ev_rm_pinrq.bd_addr =
                        conn_ptr->dev_ptr->dev_public.bd_addr;

    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
           (bt_rm_app[ i ].bondable != FALSE) )
      {
        conn_ptr->pin_req_resps_pending++;

        ev_rm_pcr.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

        BT_MSG_API( "BT RM EV TX: PIN Req AID %x",
                    ev_rm_pcr.ev_hdr.bt_app_id, 0, 0 );
        BT_BDA( MSG_API, "BT RM EV TX: PIN Req",
                        &conn_ptr->dev_ptr->dev_public.bd_addr );
        bt_ec_send_event( &ev_rm_pcr );
      }
    }

    if ( conn_ptr->pin_req_resps_pending != 0 )
    {
      conn_ptr->pin_req_timeout_ms_left = BT_RM_REQ_TIMEOUT_MS;

      bt_rm_reset_idle_timeout( conn_ptr );
    }

    conn_ptr->send_pin_req_pending = FALSE;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_command_status

DESCRIPTION
  Processes a HC Command Status event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_command_status
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  uint8           hc_status;
  uint8           hc_num_cmds;
  uint16          hc_opcode;
  bt_ev_msg_type  bt_cmd_cmplt_msg;

  hc_status   = ev_msg_ptr->ev_msg.ev_hc_cmdst.status;
  hc_num_cmds = ev_msg_ptr->ev_msg.ev_hc_cmdst.num_hci_cmd_pkts;
  hc_opcode   = ev_msg_ptr->ev_msg.ev_hc_cmdst.cmd_opcode;

  BT_MSG_API( "BT RM EV RX: HC CmdSt %x OC %x N %x",
              hc_status, hc_opcode, hc_num_cmds );

  switch ( hc_opcode )
  {
    case BT_HCI_EXIT_SNIFF_MODE:
    case BT_HCI_EXIT_PARK_MODE:
    case BT_HCI_REJECT_CONNECTION_RQST:
    case BT_HCI_DISCONNECT:
    case BT_HCI_SET_CONNECTION_ENCRYPTION:
    case BT_HCI_AUTHENTICATION_REQUESTED:
    case BT_HCI_SNIFF_MODE:
    case BT_HCI_ACCEPT_CONNECTION_RQST:
    case BT_HCI_CREATE_CONNECTION:
    case BT_HCI_INQUIRY:
    case BT_HCI_REMOTE_NAME_RQST:
    case BT_HCI_ADD_SCO_CONNECTION:
    case BT_HCI_SWITCH_ROLE:
    case BT_HCI_CHANGE_CONNECTION_PKT_TYPE:
    case BT_HCI_WR_PAGE_TIMEOUT:
    case BT_HCI_RD_REMOTE_SUPPORTED_FEAT:
    case  BT_HCI_RD_REMOTE_VERSION_INFO:
#ifdef FEATURE_BT_1_2
    case BT_HCI_SETUP_SYNC_CONNECTION:
    case BT_HCI_ACCEPT_SYNC_CONN_REQ:
    case BT_HCI_REJECT_SYNC_CONN_REQ:
    case BT_HCI_CREATE_CONNECTION_CANCEL:
    case BT_HCI_REMOTE_NAME_RQST_CANCEL:
#endif
#ifdef BT_SWDEV_2_1_PB
    case BT_HCI_ENH_FLUSH:
#endif /* BT_SWDEV_2_1_PB */
#ifdef BT_SWDEV_2_1_EPR
    case BT_HCI_REFRESH_ENCRYPTION_KEY:
#endif /* BT_SWDEV_2_1_EPR */
      {
        break;
      }

    default:
      {
        BT_ERR( "BT RM: Unexp HC CmdSt %x OC %x N %x",
                hc_status, hc_opcode, hc_num_cmds );
        break;
      }
  }

  if ( hc_status != BT_BE_SUCCESS )
  {
    BT_MSG_HIGH( "BT RM: Not Succ HC CmdSt %x OC %x N %x",
                 hc_status, hc_opcode, hc_num_cmds );
    switch ( hc_opcode )
    {
      case BT_HCI_INQUIRY :
        {
          BT_MSG_API( "BT HC EV TX: HC Inquiry Cmplt", 0, 0, 0 );
          bt_cmd_cmplt_msg.ev_hdr.ev_type = BT_EV_HC_INQUIRY_COMPLETE;
          bt_cmd_cmplt_msg.ev_msg.ev_hc_inqcp.status = hc_status;
          bt_hc_send_event( &bt_cmd_cmplt_msg );
          break;
        }

      case BT_HCI_SWITCH_ROLE:
        {
          uint16 i;
          BT_ERR( "BT RM: Unexp RlSw CmdSt fail. Conn ptr not cleaned up!",
                   0, 0, 0 );

          /* Check to see if we need to clear enh_enc_state flag.  We are
             assuming that no more than one role switch operation is pending */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( (bt_rm_conn[ i ].role_request != BT_ROLE_NONE) &&
                 (bt_rm_conn[ i ].enh_enc_state == BT_RM_ENH_ENC_LOCAL) )
            {
              bt_rm_conn[ i ].enh_enc_state = BT_RM_ENH_ENC_OK;
            }
          }
          break;
        }

#ifdef BT_SWDEV_2_1_SSR
      case BT_HCI_SNIFF_MODE:
        {
          /* Do not know Sniff on which connection is unsuccessful.
          ** Iterate the connection table to set a flag for possible
          ** connections, in order to react correspondingly when
          ** mode_change_evt received.
          */
          uint16 i;

          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( (bt_rm_conn[ i ].link_mode_request == BT_LM_SNIFF) &&
                 (bt_rm_conn[ i ].ssr_emulating == TRUE) )
            {
              /* kick off a timeout to retry Sniff emulation */
              bt_rm_conn[ i ].sniff_retry_to_ms_left =
                BT_RM_DEF_SNIFF_RETRY_TO_MS;
            }
          }
          break;
        }
#endif /* BT_SWDEV_2_1_SSR */

        case BT_HCI_REMOTE_NAME_RQST:
        {
          uint16 i;
          bt_rm_conn_type*    conn_ptr = NULL;

          /* We have a problem as cmd_status only identifies the opcode. 
           * Multiple name requests can be in progress: app initiated 
           * (which are serialized by SD layer ) OR RM initiated (during 
           * connection establishment) on different ACLs. There is no
           * framework in place in RM to match the cmd status to the
           * conn_ptr on which the corresponding cmd is issued. Logic below  
           * will simulate name request failed on all pending name requests so
           * that the app receives a response to any initiated name request 
           * AND any operation in RM which are pending on the name request
           * will proceed further.          
          */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( bt_rm_conn[ i ].rname_app_id != BT_APP_ID_NULL )
            {
              conn_ptr = &bt_rm_conn[ i ];
              /* Cleanup .. */
              if ( conn_ptr->paging == TRUE )
              { 
                bt_rm_do_bb_page_stopped( conn_ptr );
              }

              BT_ERR( "BT RM: Triggering name request failure RH %x",
                       i, 0, 0 );
              BT_BDA( ERR, "BT RM: Rname fail ",
                            &conn_ptr->dev_ptr->dev_public.bd_addr );
              bt_rm_remote_name_complete_processing( conn_ptr, 
                                                     BT_CS_RM_NAME_REQ_FAILED );
            }
          }
          break;
        }

      default:
        {
          break;
        }
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_pin_req_resp_failed

DESCRIPTION
  Sends a Bond Failed event to the application that sent a PIN request
  reply on a specified link and clears the pin request information from
  the connection as this is the last step in that process.

===========================================================================*/
LOCAL void bt_rm_pin_req_resp_failed
(
  bt_rm_conn_type*      conn_ptr,
  bt_event_reason_type  reason
)
{

  /*  The condition check below must be mimiced exactly  */
  /*  within bt_rm_link_sec_re_pair_started().           */
  if ( (conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL) &&
       (conn_ptr->pin_req_pairing_done == FALSE) )
  {
    conn_ptr->pin_req_pairing_done = TRUE;

    bt_rm_ev_send_bond_failed(
      conn_ptr->pin_req_resp_app_id,
      &conn_ptr->dev_ptr->dev_public.bd_addr,
      conn_ptr->rm_handle, reason );

    bt_rm_init_completed_pin_request( conn_ptr );
  }
  else if ( conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL )
  {
    BT_MSG_DEBUG( "BT RM: Ignored PINRR failed H %x AID %x TO %x",
                  conn_ptr->rm_handle, conn_ptr->pin_req_resp_app_id,
                  conn_ptr->pin_req_timeout_ms_left );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_finish_pending_sec_ui

DESCRIPTION
  Sends a Bond Failed event to the applications to which security 
  related user input event has been sent previously. This function should be 
  called before doing any other security related cleanup that may re-init the
  conn_ptr fields. This should be called while processing :
  - HCI Connection Complete with HC error. That is, connection failed after
    controller had sent a connection request.
  - HCI Disconnection Complete. 

===========================================================================*/
LOCAL void bt_rm_finish_pending_sec_ui
(
  bt_rm_conn_type*      conn_ptr,
  bt_event_reason_type  reason
)
{
  /* Keeping this consistent with existing implementation which
   * allows for multiple apps to call set_bondable API. Existing 
   * implementation sends all of these apps appropriate authentication 
   * related user input event but only uses the first response 
   * in completion of the authentication process.
  */

#ifdef BT_SWDEV_2_1_SSP
    BT_MSG_DEBUG( "BT RM: Sec cleanup: Seurity debug RRP %x, AR %x, BAI %x",
                   conn_ptr->rebond_req_pending, conn_ptr->authorized_rebond , conn_ptr->bonding_app_id );
#endif

    BT_MSG_DEBUG( "BT RM: Sec cleanup: Seurity debug PRRA %x PRRP %x, reason %x",
                   conn_ptr->pin_req_resp_app_id, conn_ptr->pin_req_resps_pending, reason );
  if ( 
#ifdef BT_SWDEV_2_1_SSP
      ( conn_ptr->rebond_req_pending == TRUE ) || 
#endif
      (( conn_ptr->bonding_app_id == BT_APP_ID_NULL ) &&
       ( conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL ) &&
       ( conn_ptr->pin_req_resps_pending != 0 )
     )
     )
  {
    /* Locally initiated bonding is not in progress. But, security 
     * related user input event/s have been sent for which no response has 
     * been received so far. Send bond failed event to all the 
     * registered apps.
    */
     bt_rm_ev_send_bond_failed_all_bondable(conn_ptr, reason);                                          
  }
}


/*===========================================================================

FUNCTION
  bt_rm_conn_complete_processing

DESCRIPTION
  The main processing for HC Connection Complete event for ACL links.

===========================================================================*/
LOCAL void bt_rm_conn_complete_processing
(
  bt_rm_conn_type*  conn_ptr
)
{
  uint16              flush_to;

  BT_MSG_HIGH( "BT RM: Continue connection complete processing", 0, 0, 0 );

  if ( (IS_SSP_DOABLE() ) && 
       (conn_ptr->bonding_app_id != BT_APP_ID_NULL ))
  {
    BT_MSG_HIGH( "BT RM CMD TX: HC SSP Auth Req H %x",
                conn_ptr->hc_handle_acl, 0, 0 );
    bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else
  {
    if (conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE)
    {
      /* Take care of SM3 in case of legacy devices */
      if ( conn_ptr->encrypt_mode == BT_EM_DISABLED )
      {
        conn_ptr->encrypt_enable = BT_ENCRYPT_ENABLE_OFF;

        if ( conn_ptr->hc_auth_enable == BT_HC_AUTH_DISABLED )
        {
          conn_ptr->authenticated = FALSE;
        }
        else
        {
          conn_ptr->authenticated = TRUE;
        }
      }
      else
      {
        conn_ptr->encrypt_enable = BT_ENCRYPT_ENABLE_ON;
        conn_ptr->authenticated  = TRUE;
#if ( defined ( FEATURE_BT_QSOC ) && defined ( FEATURE_BT_EXTPF_SAP ) )
#error code not present
#endif /* FEATURE_BT_QSOC */
      }
    }

    bt_rm_security_updated( conn_ptr );

#ifdef FEATURE_BT_SOC
    if( ( conn_ptr->rname_app_id == BT_APP_ID_NULL ) &&
        ( conn_ptr->remote_name_available == FALSE ) )
    {

      /* Do remote name request for this link */
      BT_BDA( MSG_API, "BT RM CMD TX: HC RName Req",
                  &conn_ptr->dev_ptr->dev_public.bd_addr )
      conn_ptr->rname_app_id = bt_rm_app_id;
#ifdef FEATURE_BT_QSOC
      /* Serialize Read Remote Name Command if Encryption key
       * length peeking is in progress*/
      if(conn_ptr->vs_ekl_rd_in_progress == FALSE)
#endif /* FEATURE_BT_QSOC */
      {
        bt_cmd_hc_remote_name_req( conn_ptr->dev_ptr->dev_public.bd_addr,
                                   conn_ptr->dev_ptr->page_scan_rep_mode,
                                   conn_ptr->dev_ptr->page_scan_mode,
                                   conn_ptr->dev_ptr->clk_offset );
      }
    }
    if ( conn_ptr->rname_app_id == BT_APP_ID_NULL )
#endif
    {
      bt_rm_finish_bond_attempt( conn_ptr );
    }

    if (( conn_ptr->state_acl == BT_RM_CS_CONNECTING ) &&
        ( conn_ptr->bonding_app_id == BT_APP_ID_NULL ))
    {
      if ( bt_rm_connection_secure_enough( conn_ptr, FALSE ) != FALSE )
      {
        conn_ptr->state_acl  = BT_RM_CS_CONNECTED;

        conn_ptr->link_mode  = BT_LM_ACTIVE;
        conn_ptr->updates   |= BT_RM_LU_LINK_MODE_B;

        bt_rm_reset_idle_timeout( conn_ptr );

#ifdef FEATURE_BT_SOC
        /* Send HCI cmd to read clock offset */
        BT_MSG_API( "BT RM CMD TX: HC Rd Clk Offset H %x",
                     conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_rd_clk_offset ( conn_ptr->hc_handle_acl );

        bt_rm_hc_get_rssi_if_reqd ( conn_ptr );
        /* start the rssi update timer */
        conn_ptr->rssi_update_ms_left = BT_RM_RSSI_UPD_TIMEOUT_MS;
#endif /* FEATURE_BT_SOC */

        bt_rm_update_device_lmp_info( conn_ptr );
        if(conn_ptr->flush_timeout != BT_L2_DEFAULT_FLUSH_TIMEOUT)
        {
          flush_to = BT_RM_MS_TO_625USEC(conn_ptr->flush_timeout);
        }
        else
        {
          flush_to = 0x0;
        }
        bt_cmd_hc_wr_auto_flush_timeout( conn_ptr->hc_handle_acl,
                                         flush_to );

#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
        if ( bt_rm_total_active_acl_links() > 1 )
        {
          BT_MSG_HIGH( "BT RM: Exisitng ACL. Disallow role sw for new",0,0,0 );
          bt_rm_set_conn_role_switch_req_policy(
            conn_ptr, BT_ROLE_SWITCH_NOT_ALLOWED );
        }
        else
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
        {
          bt_rm_set_conn_role_switch_req_policy(
              conn_ptr, bt_rm_data.create_conn_role_sw_req_ok );
        }
        bt_rm_check_park_link_policy( conn_ptr, FALSE );

        BT_MSG_API( "BT RM CMD TX: HC Wrs H %x FTO %x LP %x",
                    conn_ptr->hc_handle_acl, conn_ptr->flush_timeout,
                    conn_ptr->link_policy );

        bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                                    conn_ptr->link_policy );

        BT_MSG_API( "BT RM CMD TX: HC Wrs LPM SmI %x PHmI %x MI %x",
                    BT_RM_DEF_MIN_INCOMING_INTERVAL,
                    BT_RM_DEF_MIN_INTERVAL,
                    conn_ptr->max_interval);
#ifndef FEATURE_BT_SOC
        bt_cmd_hc_set_incoming_sniff_params(
          conn_ptr->hc_handle_acl,
          BT_RM_DEF_MIN_INCOMING_INTERVAL,
          BT_RM_DEF_MAX_INCOMING_INTERVAL );

        bt_cmd_hc_set_incoming_hold_params(
          conn_ptr->hc_handle_acl,
          BT_RM_DEF_MIN_INTERVAL,
          BT_RM_DEF_MAX_INCOMING_INTERVAL );

        bt_cmd_hc_set_incoming_park_params(
          conn_ptr->hc_handle_acl,
          BT_RM_DEF_MIN_INTERVAL,
          BT_RM_DEF_MAX_INCOMING_INTERVAL );
#endif /* !FEATURE_BT_SOC */

        bt_rm_link_status_updated( conn_ptr );

        /*  Allow 5 slot packets if possible.  */
        bt_rm_check_all_packet_types_acl();

#ifdef BT_SWDEV_2_1_SSR
        if ( bt_soc_etc_data.lmp_version >= BT_SOC_LMP_VERSION_2_1 )
        {
          /* Set SSR parameters to local controller */
          BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                      conn_ptr->hc_handle_acl,
                      conn_ptr->max_latency, 0 );

          bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                     conn_ptr->max_latency,
                                     BT_RM_DEF_REMOTE_TIMEOUT,
                                     BT_RM_DEF_LOCAL_TIMEOUT );
        }
#endif /* BT_SWDEV_2_1_SSR */

#ifndef BT_SWDEV_2_1_SSP
#ifdef FEATURE_BT_SOC
        /* Do read remote supported feature request for this link */
        conn_ptr->rd_remote_req_pending = TRUE;
        bt_cmd_hc_rd_remote_feature( conn_ptr->hc_handle_acl );
        /* Do read remote supported version request for this link */
        bt_cmd_hc_rd_remote_version( conn_ptr->hc_handle_acl );
#endif /* FEATURE_BT_SOC */
#endif /* BT_SWDEV_2_1_SSP */
#ifdef FEATURE_BT_SOC
        /* Write Default Link supervision timeout if local device is Master */
        if ( conn_ptr->role == BT_ROLE_MASTER )
        {
            bt_cmd_hc_wr_link_supervision_timeout(conn_ptr->hc_handle_acl,
                                      BT_MIN_LINK_SUPERVISION_TIMEOUT);
        }
#endif /* FEATURE_BT_SOC */

      }
      else
      {
        /*  Link security was not at minimum desired.  */
        bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
      }
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_conn_complete_acl

DESCRIPTION
  Processes a HC Connection Complete event for ACL links.

===========================================================================*/
LOCAL void bt_rm_ev_hc_conn_complete_acl
(
  bt_ev_msg_type*   ev_msg_ptr,
  bt_rm_conn_type*  conn_ptr
)
{
  bt_error_code_type  hc_status;

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_concp.status);

  if ( hc_status == BT_BE_SUCCESS )
  {
    if ( conn_ptr->state_acl == BT_RM_CS_CONNECTING )
    {
      conn_ptr->ev_hc_conn_acl_rxd = TRUE;
#ifdef FEATURE_BT_ENABLE_SLEEP_VOTING
#error code not present
#endif

      conn_ptr->hc_handle_acl =
        ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl;

      conn_ptr->encrypt_mode =
        ev_msg_ptr->ev_msg.ev_hc_concp.encryption_mode;

#if ( defined (FEATURE_BT_HOST_DIS_SCATTERNET) ) || \
    ( defined (FEATURE_BT_SOC1) && !defined (FEATURE_BT_SOC1_CNTLR_EN_SCATTERNET) )
      /* There are configuration parameters (PSKEYS) in SOC1 allowing
         the controller to be configured to prevent scatternet.
         But, to allow AMS while disabling scatternet, additonal
         logic below is required in HOST.
         However, if FEATURE_BT_HOST_DIS_SCATTERNET is defined, host will
         assume that controller does not check for scatternet.
      */
      if ( ( bt_rm_total_active_acl_links() > 1 ) &&
           ( conn_ptr->role != BT_ROLE_MASTER ) )
      {
        BT_MSG_HIGH( "BT RM: Scatternet. CC before SW. Disc .. ",0,0,0 );
        bt_rm_disconnect_acl( conn_ptr, BT_RMDR_LOW_RESOURCES );
        return; /* bail out !! */
      }
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
      if ( conn_ptr->role == BT_ROLE_SLAVE )
      {
        /* Just completed connection has a slave role. Disable scans. */
        BT_MSG_API( "BT RM CMD TX: HC WrScanEn 0 - No scans in slv role", 0, 0, 0 );
        bt_cmd_hc_wr_scan_enable( BT_HC_INQ_DIS_PAGE_DIS );

        /* Send RM radio activity event */
        BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQ_SCANNING | BT_RA_PAGE_SCANNING );
        bt_rm_ev_send_radio_activity();
      }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
#endif /* FEATURE_BT_SOC1 || FEATURE_BT_HOST_DIS_SCATTERNET */

      /* Handle bt_cmd_rm_bond_cancel race condition - If HCI connected event was not 
         processed when bond_cancel was called we should disconnect ACL here */
      if ( conn_ptr->bond_canceling == TRUE )
      {
        BT_MSG_HIGH( "BT RM: bond canceling - disconnect acl",0,0,0 );
        bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
        return; 
      }
      bt_rm_ev_send_connected( conn_ptr, BT_ACL_LINK );

#ifdef BT_SWDEV_2_1_SSP
#ifdef FEATURE_BT_SOC
      /* Read remote features first to determine if device is 2.1 capable. 
         This is necessary to determine the right authentication procedure. */
      if (( conn_ptr->bonding_app_id != BT_APP_ID_NULL ) &&
          ( conn_ptr->bonding_key_rxd != FALSE ) &&
          ( conn_ptr->authenticated   != FALSE ))
      {
        /* This should happen only in case of dedicated bonding with
           2.0 device. Dedicated bonding with 2.1, service level
           connections with 2.1 and 2.0 should go through the remote
           features path in the "if" condition above. */
        bt_rm_conn_complete_processing(conn_ptr);
      }
      else
      {
        BT_MSG_HIGH( "BT RM: Read remote features to determine SSP capability H %x", 
                     conn_ptr->hc_handle_acl, 0, 0 );
        /* Do read remote supported feature request for this link */
        conn_ptr->rd_remote_req_pending = TRUE;
        bt_cmd_hc_rd_remote_feature( conn_ptr->hc_handle_acl );
        /* Do read remote supported version request for this link */
        bt_cmd_hc_rd_remote_version( conn_ptr->hc_handle_acl );
      }
#endif
#else
      bt_rm_conn_complete_processing(conn_ptr);
#endif /* BT_SWDEV_2_1_SSP */
    }
    else if ( (conn_ptr->state_acl != BT_RM_CS_CONNECTED) ||
              (conn_ptr->hc_handle_acl !=
                 ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl) )
    {
      BT_ERR( "BT RM: Bad ConnC ACL St %x HHA %x CH %x",
              conn_ptr->state_acl,
              conn_ptr->hc_handle_acl,
              ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl );
    }
  }
  else
  {
    if ( conn_ptr->state_acl != BT_RM_CS_CONNECTING )
    {
      BT_ERR( "BT RM: Bad ConnC ACL St %x S %x H %x",
              conn_ptr->state_acl, hc_status,
              conn_ptr->rm_handle );
    }
    
    if (hc_status == BT_BE_DIFFERENT_TRANSACTION_COLLISION)
    {
       bt_rm_init_device_security( conn_ptr->dev_ptr);
       conn_ptr->dev_ptr->update_status |= BT_RM_DUS_UNBONDED_B;
    }

    /*  Code in bt_rm_link_sec_re_pair_started() must mimic  */
    /*  exactly the condition checks below including those   */
    /*  within bt_rm_finish_bond_attempt() and               */
    /*  bt_rm_pin_req_resp_failed().                         */
    if ( bt_rm_link_sec_re_pair_started( conn_ptr, hc_status ) == FALSE )
    {
      conn_ptr->state_acl = BT_RM_CS_DISCONNECTING;

      bt_rm_update_conn_hc_error( conn_ptr, hc_status );

      bt_rm_finish_pending_sec_ui( conn_ptr,
                                    (BT_HCI_REASON( hc_status )) );

      bt_rm_finish_bond_attempt( conn_ptr );

      bt_rm_pin_req_resp_failed( conn_ptr,
                                 (BT_HCI_REASON( hc_status )) );

      bt_rm_connection_failed( conn_ptr, BT_ACL_LINK,
                               (BT_HCI_REASON( hc_status )) );
    }
    else
    {
      BT_MSG_HIGH( "BT RM: Forced Re-pair HC St %x",
                   hc_status, 0, 0 );
      BT_BDA( MSG_HIGH, "BT RM: Forced Re-pair",
              &conn_ptr->dev_ptr->dev_public.bd_addr );
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_conn_complete_sco

DESCRIPTION
  Processes a HC Connection Complete event for SCO links.

===========================================================================*/
LOCAL void bt_rm_ev_hc_conn_complete_sco
(
  bt_ev_msg_type*   ev_msg_ptr,
  bt_rm_conn_type*  conn_ptr
)
{

  bt_error_code_type  hc_status;

  hc_status = (bt_error_code_type)ev_msg_ptr->ev_msg.ev_hc_concp.status;

  if ( conn_ptr->state_sco == BT_RM_CS_CONNECTING )
  {
    if ( hc_status == BT_BE_SUCCESS )
    {
      /* reset idle timeout */
      bt_rm_reset_idle_timeout( conn_ptr );

      conn_ptr->hc_handle_sco =
        ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl;

      conn_ptr->state_sco = BT_RM_CS_CONNECTED;
      bt_rm_ev_send_connected( conn_ptr, BT_SCO_LINK );

      /* Should never get here .. BT1.2 and above spec allows the host the 
       * option to use "HCI_Accept_Conn_Req" cmd if the link type is SCO. But
       * BTHOST uses "HCI_Accept_Sync_Conn_Req" for both SCO and eSCO when
       * FEATURE_BT_1_2 is defined ( which is defined on all targets using BT SOC )
       * For completeness, update the hci handle/link type on sync_conn_params .. 
      */
      conn_ptr->sync_conn_params.hc_handle = 
                                    ev_msg_ptr->ev_msg.ev_hc_concp.conn_hndl;
      conn_ptr->sync_conn_params.link_type = BT_SCO_LINK;
      conn_ptr->hc_error = BT_BE_SUCCESS;
      conn_ptr->updates  |= BT_RM_LU_SCO_PRESENCE_B;
      bt_rm_link_status_updated( conn_ptr );
    }
    else
    {
      bt_rm_connection_failed( conn_ptr, BT_SCO_LINK,
                               (BT_HCI_REASON( hc_status )) );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad ConnC SCO St %x", conn_ptr->state_sco, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad ConnC SCO",
            &conn_ptr->dev_ptr->dev_public.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_connection_complete

DESCRIPTION
  Processes a HC Connection Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_connection_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM EV RX: HC ConnC S %x LT %x",
              ev_msg_ptr->ev_msg.ev_hc_concp.status,
              ev_msg_ptr->ev_msg.ev_hc_concp.link_type, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: HC ConnC",
          &ev_msg_ptr->ev_msg.ev_hc_concp.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_concp.bd_addr,
                   ev_msg_ptr->ev_msg.ev_hc_concp.link_type,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( ev_msg_ptr->ev_msg.ev_hc_concp.link_type == BT_ACL_LINK )
    {
      if ( conn_ptr->paging == TRUE )
      {
        bt_rm_do_bb_page_stopped( conn_ptr );
      }
      bt_rm_ev_hc_conn_complete_acl( ev_msg_ptr, conn_ptr );
    }
    else
    {
      bt_rm_ev_hc_conn_complete_sco( ev_msg_ptr, conn_ptr );
    }

#ifdef BT_AMS_DISABLED
    bt_rm_check_scan_settings();
#endif /* BT_AMS_DISABLED */

  }
#if 0
/* BT_TBD */
  else if ( status == BT_CS_GN_RETRY_CMD_LATER )
  {
  }
#endif
  else
  {
    BT_ERR( "BT RM: Bad ConnC St %x LT %x",
            status, ev_msg_ptr->ev_msg.ev_hc_concp.link_type, 0 );
    BT_BDA( ERR, "BT RM: Bad ConnC",
            &ev_msg_ptr->ev_msg.ev_hc_concp.bd_addr );
  }

}


#ifdef FEATURE_BT_1_2

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_sync_conn_complete

DESCRIPTION
  Processes a HC Synchronous Connection Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_sync_conn_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status = (bt_error_code_type)ev_msg_ptr->ev_msg.ev_hc_sccmp.status;
  bt_link_type        link_type = ev_msg_ptr->ev_msg.ev_hc_sccmp.link_type;

  BT_MSG_API( "BT RM EV RX: HC SynConCmp S %x LT %x H %x",
              hc_status, link_type, ev_msg_ptr->ev_msg.ev_hc_sccmp.conn_hndl );
  BT_BDA( MSG_API, "BT RM EV RX: HC SynConCmp",
          &ev_msg_ptr->ev_msg.ev_hc_sccmp.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_sccmp.bd_addr,
                   link_type,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_sco == BT_RM_CS_CONNECTING )
    {
      if ( hc_status == BT_BE_SUCCESS )
      {
        conn_ptr->hc_handle_sco = ev_msg_ptr->ev_msg.ev_hc_sccmp.conn_hndl;

        conn_ptr->state_sco = BT_RM_CS_CONNECTED;
        /* Save the sync conn params */
        conn_ptr->sync_conn_params.hc_handle = 
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.conn_hndl;
        conn_ptr->sync_conn_params.link_type =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.link_type;
        conn_ptr->sync_conn_params.tx_interval =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.tx_interval;
        conn_ptr->sync_conn_params.retx_window =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.retx_window;
        conn_ptr->sync_conn_params.rx_pkt_length =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.rx_pkt_length;
        conn_ptr->sync_conn_params.tx_pkt_length =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.tx_pkt_length;
        conn_ptr->sync_conn_params.air_mode =
                                   ev_msg_ptr->ev_msg.ev_hc_sccmp.air_mode;

        bt_rm_ev_send_connected( conn_ptr, link_type );

        conn_ptr->hc_error = BT_BE_SUCCESS;
        conn_ptr->updates  |= BT_RM_LU_SCO_PRESENCE_B;
        bt_rm_link_status_updated( conn_ptr );

#ifdef BT_AMS_DISABLED
        bt_rm_check_scan_settings();
#endif /* BT_AMS_DISABLED */
      }
      else
      {
        bt_rm_connection_failed( conn_ptr, link_type,
                                 (BT_HCI_REASON( hc_status )) );
#ifdef BT_AMS_DISABLED
        bt_rm_check_scan_settings();
#endif /* BT_AMS_DISABLED */
      }

      /* BT_TBD */
      /* need to make use of the tx_interval and retx_window in the packet
         selection scheme */
    }
    else
    {
      BT_MSG_HIGH( "BT RM: sync conn failed S %x St %x",
                  hc_status, conn_ptr->state_sco, 0 );
#ifdef BT_AMS_DISABLED
      bt_rm_check_scan_settings();
#endif /* BT_AMS_DISABLED */
    }
  }
#if 0
/* BT_TBD */
  else if ( status == BT_CS_GN_RETRY_CMD_LATER )
  {
  }
#endif
  else
  {
    BT_ERR( "BT RM: Bad SynConCmp St %x LT %x", status, link_type, 0 );
    BT_BDA( ERR, "BT RM: Bad SynConCmp",
            &ev_msg_ptr->ev_msg.ev_hc_sccmp.bd_addr );
  }
}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_sync_conn_change

DESCRIPTION
  Processes a HC Synchronous Connection Changed event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_sync_conn_change
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_rm_conn_type*    conn_ptr;
  uint16              hc_handle;

  hc_handle = ev_msg_ptr->ev_msg.ev_hc_scchg.conn_hndl;
  BT_MSG_API( "BT RM EV RX: HC SynConChg S %x H %x",
              ev_msg_ptr->ev_msg.ev_hc_scchg.status,
              hc_handle, 0 );

  /* HCI handle passed in the event is sync connection's handle. 
   * bt_rm_get_conn_hc_handle() looks up for both acl and sync hci handles.  
  */
  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    /* Only one Sync connection supported */
    /* sync_conn_params.hc_handle is already updated on sync conn complete */
    conn_ptr->sync_conn_params.tx_interval =
                               ev_msg_ptr->ev_msg.ev_hc_scchg.tx_interval;
    conn_ptr->sync_conn_params.retx_window =
                               ev_msg_ptr->ev_msg.ev_hc_scchg.retx_window;
    conn_ptr->sync_conn_params.rx_pkt_length =
                               ev_msg_ptr->ev_msg.ev_hc_scchg.rx_pkt_length;
    conn_ptr->sync_conn_params.tx_pkt_length =
                               ev_msg_ptr->ev_msg.ev_hc_scchg.tx_pkt_length;
    conn_ptr->hc_error = BT_BE_SUCCESS;
    conn_ptr->updates  |= BT_RM_LU_SYNC_PARAM_UPD_B;
    bt_rm_link_status_updated( conn_ptr );
  }
  /* BT_TBD */

  /* need to make use of the tx_interval and retx_window in the packet
     selection scheme */
}

#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_sniff_subrating

DESCRIPTION
  Processes a HC Sniff Subrating event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_sniff_subrating
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*    conn_ptr = NULL;
  bt_error_code_type  hc_status;
  uint16              hc_handle;
  uint16              cur_latency;

  hc_status   = (bt_error_code_type)
                     (ev_msg_ptr->ev_msg.ev_hc_ssr.status);
  hc_handle   = ev_msg_ptr->ev_msg.ev_hc_ssr.conn_hndl;
  cur_latency = ev_msg_ptr->ev_msg.ev_hc_ssr.max_tx_latency;

  BT_MSG_API( "BT RM EV RX: HC SSR H|S 0x%04x TxL %x RxL %x",
              ( (hc_handle << 8) | (uint16)hc_status ),
              cur_latency,
              ev_msg_ptr->ev_msg.ev_hc_ssr.max_rx_latency );

  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
    /* clear the timer for the Host to initiate SSR */
    conn_ptr->idle_timeout_ms_left = 0;

    if ( hc_status == BT_BE_SUCCESS )
    {
      conn_ptr->cur_latency = cur_latency;

      if ( conn_ptr->max_interval >= cur_latency )
      {
        BT_MSG_API( "BT RM: SSR not being used on the remote", 0, 0, 0 );

        conn_ptr->ssr_state = BT_RM_SSR_NOT_IN_PROGRESS;
      }
      else if ( conn_ptr->max_latency < cur_latency )
      {
        BT_MSG_API( "BT RM: SSR enabled with invalid latency", 0, 0, 0 );

        conn_ptr->ssr_state = BT_RM_SSR_ENABLED;

        BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                    conn_ptr->hc_handle_acl,
                    conn_ptr->max_latency, 0 );

        bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                   conn_ptr->max_latency,
                                   BT_RM_DEF_REMOTE_TIMEOUT,
                                   BT_RM_DEF_LOCAL_TIMEOUT );
      }
      else
      {
        /* max_interval < cur_latency <= max_latency */

        if ( conn_ptr->ssr_state != BT_RM_SSR_ENABLED )
        {
          BT_MSG_API( "BT RM: SSR enabled", 0, 0, 0 );

          conn_ptr->ssr_state = BT_RM_SSR_ENABLED;
        }
        else
        {
          BT_MSG_API( "BT RM: SSR re-negotiated", 0, 0, 0 );
        }
      }
    }
    else
    {
      BT_ERR( "BT RM: SSR failed S %x H %x", hc_status, hc_handle, 0 );

      if ( hc_status != BT_BE_LMP_ERROR_TRANSACTION_COLLISION )
      {
        conn_ptr->ssr_state = BT_RM_SSR_NOT_IN_PROGRESS;
      }

      bt_rm_update_conn_hc_error( conn_ptr, hc_status );
      bt_rm_link_status_updated( conn_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RM: invalid connection entry S %x H %x",
            hc_status, hc_handle, 0 );
  }

}
#endif /* BT_SWDEV_2_1_SSR */


#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_ev_send_keypress_notification

DESCRIPTION
  Sends a RM keypress notification event.

===========================================================================*/
LOCAL void bt_rm_ev_send_keypress_notification
(
  bt_rm_conn_type*    conn_ptr,
  bt_rm_keypress_notification_type key
)
{
  bt_ev_msg_type  ev_rm_kpn;
  uint16          i;

  ev_rm_kpn.ev_hdr.ev_type = BT_EV_RM_KEYPRESS_NOTIFICATION;
  ev_rm_kpn.ev_msg.ev_rm_keypress_notif.bd_addr =
    conn_ptr->dev_ptr->dev_public.bd_addr;
  ev_rm_kpn.ev_msg.ev_rm_keypress_notif.key = key;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
    {
      ev_rm_kpn.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      BT_MSG_HIGH( "BT RM EV TX: SSP KPNot AID %x",
               ev_rm_kpn.ev_hdr.bt_app_id, 0, 0 );

      BT_BDA( MSG_HIGH, "BT RM EV TX: SSP KPNot BDA",
                        &ev_rm_kpn.ev_msg.ev_rm_keypress_notif.bd_addr );

      bt_ec_send_event( &ev_rm_kpn );
    }
  }
  bt_rm_reset_idle_timeout( conn_ptr );
}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_passkey_notification

DESCRIPTION
  Sends a RM Passkey Notification Event.

===========================================================================*/
LOCAL void bt_rm_ev_send_passkey_notification
(
  bt_rm_conn_type*    conn_ptr,
  uint32              passkey
)
{
  bt_ev_msg_type  ev_rm_pkn;
  uint16          i;

  ev_rm_pkn.ev_hdr.ev_type = BT_EV_RM_PASSKEY_NOTIFICATION;
  ev_rm_pkn.ev_msg.ev_rm_passkey_notif.bd_addr =
    conn_ptr->dev_ptr->dev_public.bd_addr;

  (void) BT_STRFORMAT( ev_rm_pkn.ev_msg.ev_rm_passkey_notif.passkey,
                       sizeof(ev_rm_pkn.ev_msg.ev_rm_passkey_notif.passkey),
                       "%06lu", passkey );

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
    {
      ev_rm_pkn.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      BT_MSG_HIGH( "BT RM EV TX: SSP PasskeyNot AID %x",
               ev_rm_pkn.ev_hdr.bt_app_id, 0, 0 );

      BT_BDA( MSG_HIGH, "BT RM EV TX: SSP PasskeyNot BDA",
                        &ev_rm_pkn.ev_msg.ev_rm_passkey_notif.bd_addr );

      bt_ec_send_event( &ev_rm_pkn );
    }
  }
  bt_rm_reset_idle_timeout( conn_ptr );
}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_passkey_request

DESCRIPTION
  Sends a RM Passkey Request event to app.

===========================================================================*/
LOCAL void bt_rm_ev_send_passkey_request
(
  bt_rm_conn_type*    conn_ptr
)
{
  bt_ev_msg_type  ev_rm_pkr;
  uint16          i;

  ev_rm_pkr.ev_hdr.ev_type = BT_EV_RM_PASSKEY_REQUEST;
  ev_rm_pkr.ev_msg.ev_rm_passkey_req.bd_addr =
    conn_ptr->dev_ptr->dev_public.bd_addr;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
    {
      conn_ptr->pin_req_resps_pending++;

      ev_rm_pkr.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      BT_MSG_HIGH( "BT RM EV TX: SSP Passkey Req AID %x",
               ev_rm_pkr.ev_hdr.bt_app_id, 0, 0 );

      BT_BDA( MSG_HIGH, "BT RM EV TX: SSP Passkey Req BDA",
                        &ev_rm_pkr.ev_msg.ev_rm_passkey_req.bd_addr );

      bt_ec_send_event( &ev_rm_pkr );
    }
  }

  if ( conn_ptr->pin_req_resps_pending != 0 )
  {
    conn_ptr->pin_req_timeout_ms_left = BT_RM_REQ_PASSKEY_TIMEOUT_MS;
    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: SSP Passkey Req but not bondable H %x",
                  conn_ptr->rm_handle, 0, 0 );
    bt_cmd_hc_user_passkey_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_user_confirmation_request

DESCRIPTION
  Sends a RM User Confirmation Request event.

===========================================================================*/
LOCAL void bt_rm_ev_send_user_confirmation_request
(
  bt_rm_conn_type*    conn_ptr,
  char*               num
)
{
  bt_ev_msg_type  ev_rm_ucr;
  uint16          i;

  ev_rm_ucr.ev_hdr.ev_type           = BT_EV_RM_USER_CONFIRMATION_REQUEST;
  ev_rm_ucr.ev_msg.ev_rm_ucr.bd_addr = conn_ptr->dev_ptr->dev_public.bd_addr;
  if ( num != NULL )
  {
    BT_STRCPY( ev_rm_ucr.ev_msg.ev_rm_ucr.passkey, num,
               sizeof(ev_rm_ucr.ev_msg.ev_rm_ucr.passkey) );
  }
  else
  {
    memset( (void*)ev_rm_ucr.ev_msg.ev_rm_ucr.passkey, 0,
            sizeof(ev_rm_ucr.ev_msg.ev_rm_ucr.passkey) );
  }


  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
    {
      conn_ptr->pin_req_resps_pending++;

      ev_rm_ucr.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      if ( num != NULL )
      {
        BT_MSG_HIGH( "BT RM EV TX: SSP UserConfReq NC NUM %c%c%c",
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 0 ],
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 1 ],
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 2 ] );
        BT_MSG_HIGH( "BT RM EV TX: SSP UserConfReq NC NUM %c%c%c",
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 3 ],
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 4 ],
                     ev_rm_ucr.ev_msg.ev_rm_ucr.passkey[ 5 ] );
        BT_MSG_HIGH( "BT RM EV TX: SSP UserConfReq NC AID %x",
                     ev_rm_ucr.ev_hdr.bt_app_id, 0, 0 );
      }
      else
      {
        BT_MSG_HIGH( "BT RM EV TX: SSP UserConfReq JW NUM NULL AID %x",
                     ev_rm_ucr.ev_hdr.bt_app_id, 0, 0 );
      }


      BT_BDA( MSG_HIGH, "BT RM EV TX: SSP UserConfReq BDA",
                        &ev_rm_ucr.ev_msg.ev_rm_ucr.bd_addr );

      bt_ec_send_event( &ev_rm_ucr );

    }
  }
  if ( conn_ptr->pin_req_resps_pending != 0 )
  {
    conn_ptr->pin_req_timeout_ms_left = BT_RM_REQ_PASSKEY_TIMEOUT_MS;
    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: SSP UserConfReq but not bondable H %x",
                  conn_ptr->rm_handle, 0, 0 );
    bt_cmd_hc_user_cfmtn_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_send_rebond_request

DESCRIPTION
  Sends a RM Rebond Request event.

===========================================================================*/
LOCAL void bt_rm_ev_send_rebond_request
(
  bt_bd_addr_type*              bd_addr_ptr
)
{

  bt_ev_msg_type  ev_rm_rebreq;
  uint16          i;

  ev_rm_rebreq.ev_hdr.ev_type                   = BT_EV_RM_REBOND_REQUEST;
  ev_rm_rebreq.ev_msg.ev_rm_rebreq.bd_addr      = *bd_addr_ptr;

  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
         (bt_rm_app[ i ].bondable != FALSE) )
    {
      ev_rm_rebreq.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

      BT_MSG_HIGH( "BT RM EV TX: SSP Rebond Req AID %x",
               ev_rm_rebreq.ev_hdr.bt_app_id, 0, 0 );

      BT_BDA( MSG_HIGH, "BT RM EV TX: SSP Rebond Req BDA",
                        &ev_rm_rebreq.ev_msg.ev_rm_rebreq.bd_addr );

      bt_ec_send_event( &ev_rm_rebreq );
    }
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_io_cap_request

DESCRIPTION
  Processes a HC IO capability request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_io_cap_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  uint16              i;
  boolean             bondable_app=FALSE;

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP IOCapReq",
          &ev_msg_ptr->ev_msg.ev_hc_io_cap_rqst.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_io_cap_rqst.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    for ( i = 0; i < BT_RM_MAX_APPS; i++ )
    {
      if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
           (bt_rm_app[ i ].bondable != FALSE) )
      {
        bondable_app =  TRUE;
      }
    }
    if ( bondable_app != TRUE )
    {
      BT_MSG_API( "BT RM CMD TX: HC SSP IOCapNegReply H %x ATZ Failure - No bondable app",
                      conn_ptr->hc_handle_acl, 0, 0 );

      bt_cmd_hc_io_capability_rqst_neg_reply( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                              BT_BE_PAIRING_NOT_ALLOWED );
    }
    else
    {
      if ( conn_ptr->pairing_initiated != FALSE )
      {
        /* RM initiated pairing */
        if ( conn_ptr->dedicated_bonding != FALSE )
        {
          /* Dedicated bonding */
          if ( conn_ptr->mitm_protection_reqd != FALSE )
          {
            conn_ptr->local_auth_req = BT_RM_AR_MITM_DEDICATED_BONDING;
          }
          else
          {
            conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_DEDICATED_BONDING;
          }
          BT_MSG_API( "BT RM CMD TX: HC SSP IOCapRes H %x LAuthReq %x",
                          conn_ptr->hc_handle_acl, conn_ptr->local_auth_req, 0 );
          /* OOB: If we have received OOB from remote device the OOB_data_present
             parameter SHALL be set to TRUE  (Vol 2, E, 7.1.29) */
          bt_cmd_hc_io_capability_response( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                            bt_dc.io_cap,
                                            conn_ptr->dev_ptr->oob_data_present,
                                            conn_ptr->local_auth_req );
        }
        else
        {
          /* General bonding due to service request.                      */
          /* Use service's security level to determine auth requirements */
          if ( conn_ptr->bonding_ss_ptr != NULL )
          {
#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */
            {
              switch ( conn_ptr->bonding_ss_ptr->sec_level )
              {
                case BT_RM_SL_1_LOW:
                  conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_NO_BONDING;
                  conn_ptr->mitm_protection_reqd = FALSE;
                  break;
                case BT_RM_SL_2_MEDIUM:
                  conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_GENERAL_BONDING;
                  conn_ptr->mitm_protection_reqd = FALSE;
                  break;
                case BT_RM_SL_3_HIGH:
                  conn_ptr->local_auth_req = BT_RM_AR_MITM_GENERAL_BONDING;
                  conn_ptr->mitm_protection_reqd = TRUE;
                  break;
                default:
                  BT_ERR( "BT RM EV RX: HC SSP IOCapReq. Bad security requirement for service %x",
                          conn_ptr->bonding_ss_ptr->sec_level, 0, 0 );
              }
            }
          }
          else
          {
            /* Set default ss here for outgoing pairing request */
            conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_NO_BONDING;
            conn_ptr->mitm_protection_reqd = FALSE;
            BT_MSG_HIGH( "BT RM: No Service security ptr. Using default. ",0,0,0 );
          }
          BT_MSG_API( "BT RM CMD TX: HC SSP IOCapRes H %x LAuthReq %x",
                          conn_ptr->hc_handle_acl, conn_ptr->local_auth_req, 0 );
          bt_cmd_hc_io_capability_response ( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                             bt_dc.io_cap,
                                             conn_ptr->dev_ptr->oob_data_present,
                                             conn_ptr->local_auth_req );
        }
      }
      else
      {
        /* RM accepting pairing */
        if (( conn_ptr->dev_ptr->link_key_valid != FALSE ) &&
            ( conn_ptr->new_link_key == FALSE ))
        {
          /* Link key already exists. Check if rebonding is ok. */
          conn_ptr->rebond_req_pending = TRUE;
          BT_MSG_API( "BT RM EV TX: SSP RebondReq Link Key Exists",
                          conn_ptr->hc_handle_acl, conn_ptr->local_auth_req, 0 );
          bt_rm_ev_send_rebond_request( &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
        else
        {
          /* Set default ss here for incoming pairing request*/
          if ( conn_ptr->dedicated_bonding != TRUE )
          {
#ifdef FEATURE_BT_TEST_API_2_1_TESTER
#error code not present
#endif /* FEATURE_BT_TEST_API_2_1_TESTER */
            {
              conn_ptr->local_auth_req = BT_RM_AR_NO_MITM_NO_BONDING;
              BT_MSG_HIGH( "BT RM: No Service security ptr. Using default. ",0,0,0 );
#ifdef FEATURE_BT_TEST_API_2_1_TESTER
#error code not present
#endif /* FEATURE_BT_TEST_API_2_1_TESTER */
            }
          }
          else
          {
            conn_ptr->local_auth_req = BT_RM_AR_MITM_DEDICATED_BONDING;
            conn_ptr->mitm_protection_reqd = TRUE;
            BT_MSG_HIGH( "BT RM: MITM Reqd for incoming dedicated bonding",0,0,0 );
          }

          BT_MSG_API( "BT RM CMD TX: HC SSP IOCapRes H %x LAuthReq %x",
                          conn_ptr->hc_handle_acl, conn_ptr->local_auth_req, 0 );
          bt_cmd_hc_io_capability_response ( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                             bt_dc.io_cap,
                                             conn_ptr->dev_ptr->oob_data_present,
                                             conn_ptr->local_auth_req );
          /* Determine if the link key will be bonded.
             We will save a link key if it was generated due to dedicated
             bonding or because MITM protection was required
          */
          conn_ptr->dev_ptr->bonded_link_key = bt_rm_determine_bonded_key( conn_ptr );
          conn_ptr->auth_stage1_proc = bt_rm_determine_auth_stage1_proc( conn_ptr->mitm_protection_reqd,
                                                                         conn_ptr->dev_ptr->io_cap,
                                                                         conn_ptr->dev_ptr->oob_data_present );
        }
      }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad IOCapReq %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad IOCapReq",
            &ev_msg_ptr->ev_msg.ev_hc_io_cap_rqst.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_io_cap_response

DESCRIPTION
  Processes a HC IO capability response event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_io_cap_response
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  boolean             remote_initiated_dedicated_bonding=FALSE;

  BT_BDA( MSG_API, "BT RM EV RX: HC IOCapRes",
          &ev_msg_ptr->ev_msg.ev_hc_io_cap_res.bd_addr );

  BT_MSG_HIGH( "BT RM EV RX: HC IOCapRes RAuthReq %x IOC %x",
               ev_msg_ptr->ev_msg.ev_hc_io_cap_res.auth_req,
               ev_msg_ptr->ev_msg.ev_hc_io_cap_res.io_capability, 0 );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_io_cap_res.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    conn_ptr->remote_auth_req = ( bt_rm_auth_req_type )ev_msg_ptr->ev_msg.ev_hc_io_cap_res.auth_req;

    if (( conn_ptr->dedicated_bonding != TRUE ) &&
        (( conn_ptr->remote_auth_req == BT_RM_AR_MITM_DEDICATED_BONDING ) ||
         ( conn_ptr->remote_auth_req == BT_RM_AR_NO_MITM_DEDICATED_BONDING )))
    {
      conn_ptr->dedicated_bonding = TRUE;
      remote_initiated_dedicated_bonding = TRUE;
    }

    if ( conn_ptr->dedicated_bonding != FALSE )
    {
      /* Dedicated bonding */
      if ( remote_initiated_dedicated_bonding != FALSE )
      {
        conn_ptr->mitm_protection_reqd=
          ((conn_ptr->remote_auth_req==BT_RM_AR_MITM_DEDICATED_BONDING)||
           (bt_dc.default_sec_level==BT_RM_SL_3_HIGH)  );
      }
      else
      {
        conn_ptr->mitm_protection_reqd = conn_ptr->mitm_protection_reqd ||
          (conn_ptr->remote_auth_req==BT_RM_AR_MITM_NO_BONDING)       ||
          (conn_ptr->remote_auth_req==BT_RM_AR_MITM_DEDICATED_BONDING)||
          (conn_ptr->remote_auth_req==BT_RM_AR_MITM_GENERAL_BONDING);
      }
    }
    else
    {
      /* General bonding */
      if (( conn_ptr->bonding_ss_ptr != NULL ) &&
          ( conn_ptr->bonding_ss_ptr->sec_level == BT_RM_SL_3_HIGH ))
      {

        conn_ptr->mitm_protection_reqd = TRUE;
      }
      else
      {
        conn_ptr->mitm_protection_reqd=
          ((conn_ptr->remote_auth_req==BT_RM_AR_MITM_NO_BONDING)       ||
           (conn_ptr->remote_auth_req==BT_RM_AR_MITM_DEDICATED_BONDING)||
           (conn_ptr->remote_auth_req==BT_RM_AR_MITM_GENERAL_BONDING)  );
      }
    }

    if ( conn_ptr->pairing_initiated != FALSE )
    {
      conn_ptr->dev_ptr->bonded_link_key = bt_rm_determine_bonded_key( conn_ptr );
    }

    /* Determine pairing procedure - numeric, passkey, just works */
    conn_ptr->dev_ptr->io_cap = ( bt_rm_io_cap_type )ev_msg_ptr->ev_msg.ev_hc_io_cap_res.io_capability;
    conn_ptr->auth_stage1_proc = bt_rm_determine_auth_stage1_proc( conn_ptr->mitm_protection_reqd,
                                                                   conn_ptr->dev_ptr->io_cap,
                                                                   conn_ptr->dev_ptr->oob_data_present );

    if ( conn_ptr->dev_ptr->dev_public.valid != FALSE )
    {
      bt_rm_device_updated( BT_APP_ID_NULL, conn_ptr->dev_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad IOCapRes %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: SSP Bad IOCapRes",
            &ev_msg_ptr->ev_msg.ev_hc_io_cap_res.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_user_conf_request

DESCRIPTION
  Processes a HC user confirmation request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_user_conf_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  char                num_val_s[ BT_RM_SSP_PASSKEY_LEN ];

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP UserConfReq",
          &ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.bd_addr );

  BT_MSG_HIGH("BT RM EV RX: HC SSP UserConfReq NUM %lu",
              ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.num_val,0,0);

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    (void) BT_STRFORMAT( num_val_s, sizeof(num_val_s), "%06lu",
                         ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.num_val);
    switch ( conn_ptr->auth_stage1_proc )
    {
      case  BT_RM_AS1_NUMERIC_COMPARISON:
        if ( ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.num_val < 999999 )
        {
          #ifdef CUST_EDITION /*User has input pin code in bt_rm_cmd_bond_ext()*/ //ptusheng
          if ( conn_ptr->pairing_initiated != FALSE )
          {
            bt_cmd_hc_user_cfmtn_rqst_reply(&conn_ptr->dev_ptr->dev_public.bd_addr );
          }
          else
          #endif
          {
          BT_MSG_HIGH("BT RM EV TX: SSP UserConfReq to App for NC",
                      0,0,0);
          bt_rm_ev_send_user_confirmation_request(
            conn_ptr,
            num_val_s );
          }
        }
        else
        {
          BT_ERR( "BT RM: SSP UserConfReq Invalid Numeric val %lu",
                  ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.num_val, 0, 0 );
        }
        break;
      case BT_RM_AS1_JUST_WORKS:
        // ( conn_ptr->pairing_initiated != FALSE )||
        if ( (conn_ptr->dev_ptr->bonded_link_key == FALSE) || /* Non-bonded link key. Auto-accept */
             (conn_ptr->authorize_dir == BT_RM_ATZRQ_OUTGOING) || /* Already authorized - user initiated */
             (conn_ptr->authorized_rebond != FALSE) )  /* Already authorized - rebond response rxed */
        {
          BT_MSG_HIGH("BT RM CMD TX: HC SSP UserConfRep for JW - auto accept NBLK or Local Init",
                      num_val_s,0,0);
          bt_cmd_hc_user_cfmtn_rqst_reply(
              &conn_ptr->dev_ptr->dev_public.bd_addr );
        }
        else
        {
          BT_MSG_HIGH("BT RM EV TX: SSP UserConfReq to App for JW",
                      0,0,0);
          bt_rm_ev_send_user_confirmation_request(
              conn_ptr,
              NULL );
        }
        break;
      default:
        BT_ERR( "BT RM: SSP UserConfReq Unexpected! AUTH Stage1 Proc %x",
                conn_ptr->auth_stage1_proc, 0, 0 );
    }

  }
  else
  {
    BT_ERR( "BT RM: Bad IOCapRes %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: HC SSP UserConfReq Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_user_cfm_rqst.bd_addr );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_sp_complete

DESCRIPTION
  Processes a HC simple pairing event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_sp_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_sp_complt.status);
  BT_MSG_API( "BT RM EV RX: HC SSP SPComplete S %x ",
              hc_status, 0, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: HC SSP SPComplete",
          &ev_msg_ptr->ev_msg.ev_hc_sp_complt.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_sp_complt.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    conn_ptr->mitm_protection_reqd = FALSE;

    if (( hc_status != BT_BE_SUCCESS ) &&
        ( conn_ptr->pairing_initiated != TRUE ) &&
        ( conn_ptr->dev_ptr->bonded_link_key == TRUE ))
    {
      /* Authentication complete will not be received later. Cleanup now */
      conn_ptr->authenticated = FALSE;
      conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
      BT_MSG_DEBUG( "BT RM: Auth failed S %x", hc_status, 0, 0 );

      /* Here we need to send bond failed unconditionally */
      bt_rm_ev_send_bond_failed_all_bondable(conn_ptr, 
                                             (BT_HCI_REASON( hc_status )));
        
      /* During diconnection complete processing these flag are checked */
      /* Re-init the flags to prevent duplicate events being sent. */
      conn_ptr->rebond_req_pending = FALSE;
      conn_ptr->authorized_rebond = FALSE;

      bt_rm_init_conn_pin_request( conn_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad SSP SPComplete %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: SSP SPComplete Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_sp_complt.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_remote_host_supp_features

DESCRIPTION
  Processes a HC remote host supported features notification event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_remote_host_supp_features
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP RemoteHSFeatures",
          &ev_msg_ptr->ev_msg.ev_hc_rem_host_sf_ntfctn.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_rem_host_sf_ntfctn.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( BT_LMP_FEATURE_SUPPORTED( BT_RM_LMP_FEATURE_BIT_SSP_HOST,
                                   &ev_msg_ptr->ev_msg.ev_hc_rem_host_sf_ntfctn.host_supported_feat ) )
    {
      BT_MSG_DEBUG("BT RM: SSP supported by remote host",0,0,0);

      conn_ptr->dev_ptr->dev_public.ssp_capable = TRUE;
    }
    else
    {
      /* In case the remote has not set the feature bits */
      BT_MSG_DEBUG("BT RM: SSP NOT supported by host",0,0,0);
      conn_ptr->dev_ptr->dev_public.ssp_capable = FALSE;
    }
    conn_ptr->dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
    if ( conn_ptr->dev_ptr->dev_public.valid != FALSE )
    {
      bt_rm_device_updated( BT_APP_ID_NULL, conn_ptr->dev_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad SSP RemoteHSFeatures %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: SSP RemoteHSFeatures Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_rem_host_sf_ntfctn.bd_addr );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_encryption_key_refresh

DESCRIPTION
  Processes a HC encryption key refresh event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_encryption_key_refresh
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_rm_conn_type*    conn_ptr;
  uint16              hc_handle;
  bt_error_code_type  hc_status;

  hc_status = (bt_error_code_type)
                (ev_msg_ptr->ev_msg.ev_hc_enc_key_rc.status);
  hc_handle = ev_msg_ptr->ev_msg.ev_hc_enc_key_rc.conn_hndl;

#ifdef BT_SWDEV_2_1_EPR
  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    /* Enc Key Refresh is not pending now */
    conn_ptr->enc_key_refresh_req_pending = FALSE;

    if( hc_status == BT_BE_SUCCESS )
    {
      /* Reset enc key age counter */
      conn_ptr->encryption_key_age = 0;
    }
    else
    {
      /* Encryption key refresh failed */
      BT_MSG_HIGH("BT RM EV TX: EK Ref Failed %x", hc_status, 0, 0);
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad EK Ref %x S %x",
            hc_handle, hc_status, 0 );
  }
#endif /* BT_SWDEV_2_1_EPR */
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_user_pk_request

DESCRIPTION
  Processes a HC Passkey Request event and sends
  BT_EV_RM_PASSKEY_REQUEST event to app.

===========================================================================*/
LOCAL void bt_rm_ev_hc_user_pk_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP UserPKReq",
          &ev_msg_ptr->ev_msg.ev_hc_user_pk_rqst.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_user_pk_rqst.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    BT_MSG_HIGH("BT RM EV TX: SSP UserPKReq to App",0,0,0);

    bt_rm_ev_send_passkey_request( conn_ptr );

  }
  else
  {
    BT_ERR( "BT RM: Bad SSP UserPKReq %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: HC SSP UserPKReq Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_user_pk_rqst.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_oob_data_request

DESCRIPTION
  Processes a HC remote OOB data request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_oob_data_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  /* Send Remote OOB data to controller here */
  bt_rm_device_type*  dev_ptr;
  bt_bd_addr_type*    bd_addr;

  BT_BDA( MSG_API, "BT RM EV RX: HC OOB Data Req",
          &ev_msg_ptr->ev_msg.ev_hc_rem_oob_data_rqst.bd_addr );

  bd_addr = &ev_msg_ptr->ev_msg.ev_hc_rem_oob_data_rqst.bd_addr;

  if ( (dev_ptr = bt_rm_get_device( bd_addr, FALSE )) != NULL )
  {
    if ( dev_ptr->oob_data_present == TRUE )
    {
      /* TODO: Is this correct? */
      dev_ptr->oob_data_present = FALSE;
      BT_MSG_HIGH("BT RM CMD TX: OOB Data Req Rep",0,0,0);
      bt_cmd_hc_remote_oob_data_rqst_reply( bd_addr, &dev_ptr->sp_oob_hash_c,
                                            &dev_ptr->sp_oob_rand_r );
    }
    else
    {
      BT_MSG_HIGH("BT RM CMD TX: OOB Data Req Neg Rep",0,0,0);
      bt_cmd_hc_remote_oob_data_rqst_neg_reply( bd_addr );
    }
  }
  else
  {
    BT_MSG_HIGH("BT RM EV RX: Device not found in DB",0,0,0);
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_user_pk_notification

DESCRIPTION
  Processes a HC user passkey notification event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_user_pk_notification
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM EV RX: HC SSP UserPKNot PKey %d ",
              ev_msg_ptr->ev_msg.ev_hc_user_pk_ntfctn.passkey, 0, 0 );

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP UserPKNot",
          &ev_msg_ptr->ev_msg.ev_hc_user_pk_ntfctn.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_user_pk_ntfctn.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    BT_MSG_HIGH("BT RM EV TX: SSP UserPKNot to App",0,0,0);

    bt_rm_ev_send_passkey_notification( conn_ptr,
        ev_msg_ptr->ev_msg.ev_hc_user_pk_ntfctn.passkey );

  }
  else
  {
    BT_ERR( "BT RM: Bad SSP UserPKNot %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: HC SSP UserPKNot Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_user_pk_ntfctn.bd_addr );
  }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_kp_notification

DESCRIPTION
  Processes a HC keypress notification event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_kp_notification
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  // Make sure this is received after HCI_passkey_notification and before
  // HCI_user_passkey_request_{negative_}reply
  //
  // Send BT_EV_RM_KEYPRESS_NOTIFICATION event to app
  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM EV RX: HC SSP KPNot Key %x ",
              ev_msg_ptr->ev_msg.ev_hc_kp_ntfctn.notification_type, 0, 0 );

  BT_BDA( MSG_API, "BT RM EV RX: HC SSP KPNot",
          &ev_msg_ptr->ev_msg.ev_hc_kp_ntfctn.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_kp_ntfctn.bd_addr,
                   BT_ACL_LINK,
                   &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    BT_MSG_HIGH("BT RM EV TX: SSP KPNot to App",0,0,0);

    bt_rm_ev_send_keypress_notification( conn_ptr,
        ( bt_rm_keypress_notification_type )ev_msg_ptr->ev_msg.ev_hc_kp_ntfctn.notification_type );

  }
  else
  {
    BT_ERR( "BT RM: Bad SSP KPNot %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: HC SSP KPNot Bad BDA Param",
            &ev_msg_ptr->ev_msg.ev_hc_kp_ntfctn.bd_addr );
  }
}

#endif /* BT_SWDEV_2_1_SSP */
/*===========================================================================

FUNCTION
  bt_rm_update_acl_pkt_types_in_use

DESCRIPTION
  Updates the ACL packet types in use for a specified connection.

===========================================================================*/
LOCAL void bt_rm_update_acl_pkt_types_in_use
(
  bt_rm_conn_type*  conn_ptr,
  uint16            new_acl_pkt_types_in_use
)
{

  if ( conn_ptr->pkt_types_in_use_acl != new_acl_pkt_types_in_use )
  {
    BT_MSG_HIGH( "BT RM: TX ACL Pkt Types Cur %x Prev %x H %x",
                 new_acl_pkt_types_in_use,
                 conn_ptr->pkt_types_in_use_acl,
                 conn_ptr->rm_handle );

    conn_ptr->pkt_types_in_use_acl = new_acl_pkt_types_in_use;
    conn_ptr->updates |= BT_RM_LU_ACL_PACKETS_B;

    bt_rm_link_status_updated( conn_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_max_slots_changed

DESCRIPTION
  Processes a HC Max Slots Changed event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_max_slots_changed
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  uint16            hc_handle;
  uint8             max_slots;
  uint16            new_acl_pkt_types_in_use;

  hc_handle = ev_msg_ptr->ev_msg.ev_hc_maxsl.conn_hndl;
  max_slots = ev_msg_ptr->ev_msg.ev_hc_maxsl.lmp_max_slots;

  BT_MSG_API( "BT RM EV RX: HC MaxSlotCh %x H %x",
              max_slots, hc_handle,0 );

  if ( ((max_slots == 1) || (max_slots == 3) || (max_slots == 5)) &&
       ((conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL) )
  {
    if ( conn_ptr->hc_handle_acl == hc_handle )
    {
      if ( conn_ptr->max_slots_in_use_acl != max_slots )
      {
        BT_MSG_HIGH( "BT RM: ACL Max Slots Cur %x Prev %x H %x",
                     max_slots, conn_ptr->max_slots_in_use_acl,
                     conn_ptr->rm_handle );

        conn_ptr->max_slots_in_use_acl = max_slots;

#ifndef FEATURE_BT_SOC
        new_acl_pkt_types_in_use =
          bt_lm_acl_pkt_types_from_max_slots(
            conn_ptr->max_slots_in_use_acl );
#else
        new_acl_pkt_types_in_use = BT_PT_1_SLOT_M;
#endif /* FEATURE_BT_SOC */

        new_acl_pkt_types_in_use &= conn_ptr->pkt_types_allowed_acl;

        bt_rm_update_acl_pkt_types_in_use( conn_ptr,
                                           new_acl_pkt_types_in_use );
#ifndef FEATURE_BT_SOC
        bt_rm_check_conn_packet_types_acl( conn_ptr );
#endif
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad MaxSlotsCh H %x MS %x",
              hc_handle, max_slots, 0 );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad MaxSlotsCh H %x MS %x",
            hc_handle, max_slots, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_sco_hv3_fits_on_piconet

DESCRIPTION
  Checks if all of the packet types currently in use on the piconet allow
  for a SCO link with HV3 packets to be added.  To be OK all ACL links
  must only be using 1 slot ACL packets.

RETURN VALUE
  TRUE   If it is currently OK for an HV3 SCO link to be added
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_sco_hv3_fits_on_piconet
(
  void
)
{

  uint16   i;
  boolean  sco_hv3_ok = TRUE;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
         ((bt_rm_conn[ i ].pkt_types_req_acl != 0) ||
          ((bt_rm_conn[ i ].pkt_types_in_use_acl &
             (BT_PT_3_SLOT_M | BT_PT_5_SLOT_M)) != 0)) )
    {
      sco_hv3_ok = FALSE;
      break;
    }
  }

  return ( sco_hv3_ok );

}


/*===========================================================================

FUNCTION
  bt_rm_piconet_acl_pkt_types_settled

DESCRIPTION
  Checks if all of the ACL packet types currently in use on the piconet
  are settled in that there are no outstanding ACL packet type change
  requests.

RETURN VALUE
  TRUE   If ACL packet types on the piconet are settled
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_piconet_acl_pkt_types_settled
(
  void
)
{

  uint16   i;
  boolean  acl_pkt_types_settled = TRUE;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
         (bt_rm_conn[ i ].pkt_types_req_acl != 0) )
    {
      acl_pkt_types_settled = FALSE;
      break;
    }
  }

  return ( acl_pkt_types_settled );

}


/*===========================================================================

FUNCTION
  bt_rm_finish_sco_setup

DESCRIPTION
  Finishes any pending SCO setup that has been waiting for ACL packet
  type changes to complete on the piconet to allow a SCO link to fit.

===========================================================================*/
LOCAL void bt_rm_finish_sco_setup
(
  void
)
{

  uint16            i;
  bt_rm_conn_type*  conn_ptr;
  uint16            sync_pkt_types = 0;

  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    conn_ptr = &bt_rm_conn[ i ];

    if ( (conn_ptr->state_acl != BT_RM_CS_DISCONNECTED) &&
         (conn_ptr->state_sco == BT_RM_CS_CONNECTING) &&
         (conn_ptr->pending_pkt_adjs != FALSE) )
    {
      if ( bt_rm_sco_hv3_fits_on_piconet() != FALSE )
      {
        conn_ptr->pending_pkt_adjs = FALSE;

        if ( conn_ptr->sco_resps_pending != 0 )
        {
          BT_BDA( MSG_API, "BT RM CMD TX: HC Acpt ConnS",
                  &conn_ptr->dev_ptr->dev_public.bd_addr );

#ifdef FEATURE_BT_1_2
          bt_cmd_hc_accept_sync_conn_req(
            &conn_ptr->dev_ptr->dev_public.bd_addr,
            BT_LM_SCO_BANDWIDTH,
            BT_LM_SCO_BANDWIDTH, /* not applicable to SCO */
            BT_LM_ESCO_LATENCY_10_MS,
            BT_LM_VC_AC_CVSD,
            BT_LM_ESCO_RETX_OPTIMUM_POWER, /* not applicable to SCO */
            BT_SYNC_DEF_ALL_PKT_TYPES ); /* Allow all packet types when accepting
                                            SCO connection */
#else
          bt_cmd_hc_accept_connection(
            &conn_ptr->dev_ptr->dev_public.bd_addr, BT_ROLE_NONE );
#endif /* FEATURE_BT_1_2 */

          conn_ptr->sco_resps_pending       = 0;
          conn_ptr->sco_req_timeout_ms_left = 0;
        }
        else
        {
          sync_pkt_types = bt_rm_select_sco_packet_type ( conn_ptr );
#ifdef FEATURE_BT_1_2

          BT_MSG_API( "BT RM CMD TX: HC Add sync conn H %x PT %x AID %x",
                      conn_ptr->hc_handle_acl,
                      sync_pkt_types,
                      conn_ptr->bt_app_id_sco );

          bt_cmd_hc_setup_sync_conn(
            conn_ptr->hc_handle_acl,
            BT_LM_SCO_BANDWIDTH,
            BT_LM_SCO_BANDWIDTH,
            BT_LM_ESCO_LATENCY_10_MS,
            BT_LM_VC_AC_CVSD,
            BT_LM_ESCO_RETX_OPTIMUM_POWER,
            sync_pkt_types );
#else
            /* attempt_esco is ignored since eSCO is not supported */
          BT_MSG_API( "BT RM CMD TX: HC Add SCO H %x PT %x AID %x",
                      conn_ptr->hc_handle_acl,
                      sync_pkt_types,
                      conn_ptr->bt_app_id_sco );

          bt_cmd_hc_add_sco_connection(
            conn_ptr->hc_handle_acl,
            sync_pkt_types );
#endif /* FEATURE_BT_1_2 */
        }
      }
      break;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_pkt_type_changed

DESCRIPTION
  Processes a HC Connection Packet Type Changed event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_pkt_type_changed
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;
  uint16              hc_handle;
  uint16              pkt_type;

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_pktch.status);
  hc_handle = ev_msg_ptr->ev_msg.ev_hc_pktch.conn_hndl;
  pkt_type  = ev_msg_ptr->ev_msg.ev_hc_pktch.pkt_type;

  BT_MSG_API( "BT RM EV RX: HC PktCh %x S %x H %x",
              pkt_type, hc_status, hc_handle );

  if ( hc_status != BT_BE_SUCCESS )
  {
    BT_MSG_DEBUG( "BT RM: Bad PktCh S %x H %x PT %x",
                  hc_status, hc_handle, pkt_type );
  }

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( (conn_ptr->hc_handle_acl == hc_handle) &&
         (pkt_type & BT_PT_ACL_M) )
    {
      if ( conn_ptr->pkt_types_req_acl == 0 )
      {
        BT_ERR( "BT RM: Unexp PktCh H %x S %x PT %x",
                hc_handle, hc_status, pkt_type );
      }

      conn_ptr->pkt_types_req_acl = 0;

      bt_rm_update_acl_pkt_types_in_use( conn_ptr, pkt_type );

      bt_rm_finish_sco_setup();
    }
    else if ( (conn_ptr->hc_handle_sco == hc_handle) &&
              (pkt_type & BT_PT_SCO_M) )
    {
      if ( conn_ptr->pkt_types_in_use_sco != pkt_type )
      {
        conn_ptr->pkt_types_in_use_sco = pkt_type;
        conn_ptr->updates |= BT_RM_LU_SCO_PACKETS_B;

        bt_rm_link_status_updated( conn_ptr );

        BT_MSG_HIGH( "BT RM: PktCh SCO %x RH %x HH %x",
                     pkt_type, conn_ptr->rm_handle, hc_handle );

        /*  BT_TBD:  Optimize use of BB here.  */
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad PktCh H %x S %x PT %x",
              hc_handle, hc_status, pkt_type );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad PktCh H %x PT %x S %x",
            hc_handle, pkt_type, hc_status );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_accept_connection_acl

DESCRIPTION
  Determines if a specified ACL connection can be accepted on behalf
  of a specified BT APP ID and does so as necessary.

RETURN VALUE
  BT_CS_GN_SUCCESS if ACL link is accepted for BT APP ID.
  BT_CS_RM_CONN_ACTIVE if the ACL link cannot be accepted.
  BT_CS_GN_RETRY_CMD_LATER if the ACL link may be acceptable later.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_accept_connection_acl
(
  bt_rm_conn_type*  conn_ptr,
  bt_app_id_type    bt_app_id
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  bt_bd_addr_type*    bd_addr_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;

  if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTING )
  {
    status = BT_CS_GN_RETRY_CMD_LATER;
  }
  else
  {
    if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTING) &&
         ((conn_ptr->bt_app_id_acl == BT_APP_ID_NULL) ||
          (bt_app_id == BT_APP_ID_NULL)) )
    {
      if ( conn_ptr->bt_app_id_acl == BT_APP_ID_NULL )
      {
        conn_ptr->bt_app_id_acl = bt_app_id;
      }

      conn_ptr->role     = BT_ROLE_SLAVE;
      conn_ptr->updates |= BT_RM_LU_ROLE_B;

      //#ifndef BT_SWDEV_2_1_SSP
      bt_rm_init_hc_sec_settings_acl( conn_ptr );
      //#endif /* BT_SWDEV_2_1_SSP */

      BT_BDA( MSG_API, "BT RM CMD TX: HC Acpt ConnA", bd_addr_ptr );

      bt_cmd_hc_accept_connection( bd_addr_ptr,
                                     bt_rm_data.accept_conn_role );

      conn_ptr->acl_resps_pending       = 0;
      conn_ptr->acl_req_timeout_ms_left = 0;
    }

    if ( (bt_app_id != BT_APP_ID_NULL) &&
         (conn_ptr->bt_app_id_acl != bt_app_id) )
    {
      status = BT_CS_RM_CONN_ACTIVE;
    }
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_accept_connection_sco

DESCRIPTION
  Determines if a specified SCO connection can be accepted on behalf
  of a specified BT APP ID and does so as necessary.

RETURN VALUE
  BT_CS_GN_SUCCESS if SCO link is accepted for BT APP ID.
  BT_CS_RM_CONN_ACTIVE if the SCO link cannot be accepted.
  BT_CS_GN_RETRY_CMD_LATER if the SCO link may be acceptable later.

===========================================================================*/
LOCAL bt_cmd_status_type bt_rm_accept_connection_sco
(
  bt_rm_conn_type*  conn_ptr,
  bt_app_id_type    bt_app_id
)
{

  bt_cmd_status_type  status = BT_CS_GN_SUCCESS;
  bt_bd_addr_type*    bd_addr_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;

  if ( conn_ptr->state_sco == BT_RM_CS_DISCONNECTING )
  {
    status = BT_CS_GN_RETRY_CMD_LATER;
  }
  else
  {
    if ( (conn_ptr->state_sco == BT_RM_CS_CONNECTING) &&
         ((conn_ptr->bt_app_id_sco == BT_APP_ID_NULL) ||
          (bt_app_id == BT_APP_ID_NULL)) )
    {
      if ( conn_ptr->bt_app_id_sco == BT_APP_ID_NULL )
      {
        conn_ptr->bt_app_id_sco = bt_app_id;
      }

      /*  Only use 1 slot packets prior to SCO starting.  */
      if ( (conn_ptr->pending_pkt_adjs =
              bt_rm_check_all_packet_types_acl()) == FALSE )
      {
        BT_BDA( MSG_API, "BT RM CMD TX: HC Acpt ConnS", bd_addr_ptr );
#ifdef FEATURE_BT_1_2

        bt_cmd_hc_accept_sync_conn_req(
          bd_addr_ptr,
          BT_LM_SCO_BANDWIDTH,
          BT_LM_SCO_BANDWIDTH, /* not applicable to SCO */
          BT_LM_ESCO_LATENCY_10_MS,
          BT_LM_VC_AC_CVSD,
          BT_LM_ESCO_RETX_OPTIMUM_POWER, /* not applicable to SCO */
          BT_SYNC_DEF_ALL_PKT_TYPES ); /* Allow all packet types when accepting
                                           SCO connection */

#else
        bt_cmd_hc_accept_connection( bd_addr_ptr, BT_ROLE_NONE );
#endif

        conn_ptr->sco_resps_pending       = 0;
        conn_ptr->sco_req_timeout_ms_left = 0;
      }
    }

    if ( (bt_app_id != BT_APP_ID_NULL) &&
         (conn_ptr->bt_app_id_sco != bt_app_id) )
    {
      status = BT_CS_RM_CONN_ACTIVE;
    }
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_update_remote_class_of_device

DESCRIPTION
  Updates the Class of Device field of a remote device.  The device
  database in EFS is updated as necessary.

===========================================================================*/
LOCAL void bt_rm_update_remote_class_of_device
(
  bt_cod_type*        new_cod_ptr,
  bt_rm_device_type*  dev_ptr
)
{

  uint8    i;
  boolean  new_cod_different = FALSE;
  boolean  new_cod_nonzero   = FALSE;

  for ( i = 0; i < sizeof( bt_cod_type ); i++ )
  {
    if ( new_cod_ptr->cod_bytes[ i ] != 0 )
    {
      new_cod_nonzero = TRUE;
    }
    if ( dev_ptr->dev_public.class_of_device.cod_bytes[ i ] !=
           new_cod_ptr->cod_bytes[ i ] )
    {
      new_cod_different = TRUE;
    }
  }

  if ( (new_cod_nonzero != FALSE) && (new_cod_different != FALSE) )
  {
    dev_ptr->dev_public.class_of_device = *new_cod_ptr;

    bt_sd_parse_class_of_device(
      (bt_cod_type*)(&dev_ptr->dev_public.class_of_device),
      (uint16*)(&dev_ptr->dev_public.service_class),
      (uint16*)(&dev_ptr->dev_public.major_dev_class),
      (uint16*)(&dev_ptr->dev_public.minor_dev_class) );

    dev_ptr->update_status |= BT_RM_DUS_INTERNAL_B;
  }

  /*  Need to do update check below regardless of COD  */
  /*  update above due to prior updates that may have  */
  /*  occurred within bt_rm_ev_hc_inquiry_result().    */
  if ( (dev_ptr->dev_public.valid != FALSE) &&
       (dev_ptr->update_status != BT_RM_DUS_NONE) )
  {
    bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_connection_req_acl

DESCRIPTION
  Processes an HC ACL Connection Request.

RETURN VALUE
  BT_BE_SUCCESS if connection requests were successfully generated
    for applications;  otherwise the HC reason to be used to reject
    the connection immediately.

===========================================================================*/
LOCAL bt_error_code_type bt_rm_ev_hc_connection_req_acl
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_error_code_type  hc_reason;
  bt_rm_conn_type*    conn_ptr;
  uint16              i;

  if ( (status = bt_rm_get_conn_bd_addr(
                   FALSE,  /*  Connection need not exist yet.   */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
    {
      conn_ptr->state_acl = BT_RM_CS_CONNECTING;

      bt_rm_update_remote_class_of_device(
        &ev_msg_ptr->ev_msg.ev_hc_conrq.class_of_device,
        conn_ptr->dev_ptr );

      if ( conn_ptr->acl_resps_pending != 0 )
      {
        BT_ERR( "BT RM: ACL Resps Pending %x RH %x HHA %x",
                conn_ptr->acl_resps_pending,
                conn_ptr->rm_handle, conn_ptr->hc_handle_acl );

        conn_ptr->acl_resps_pending = 0;
      }

      status = BT_CS_RM_MAX_APPLICATIONS;
      for ( i = 0; i < BT_RM_MAX_APPS; i++ )
      {
        if ( bt_rm_app[ i ].acl_registered != FALSE )
        {
          conn_ptr->acl_resps_pending++;
          bt_rm_ev_send_connection_req( bt_rm_app[ i ].bt_app_id,
                                        conn_ptr, BT_ACL_LINK );
          status = BT_CS_GN_SUCCESS;
        }
      }

      if ( status != BT_CS_GN_SUCCESS )
      {
        bt_rm_init_link( conn_ptr, BT_ACL_LINK );
      }
    }
    else
    {
      status = bt_rm_accept_connection_acl( conn_ptr,
                                            BT_APP_ID_NULL );
    }
  }

  if ( status == BT_CS_GN_SUCCESS )
  {
    hc_reason = BT_BE_SUCCESS;
  }
#if 0
/* BT_TBD */
  else if ( status == BT_CS_GN_RETRY_CMD_LATER )
  {
    hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;
  }
#endif
  else
  {
    hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;
  }

  return ( hc_reason );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_connection_req_sco

DESCRIPTION
  Processes an HC SCO Connection Request.

RETURN VALUE
  BT_BE_SUCCESS if connection requests were successfully generated
    for applications;  otherwise the HC reason to be used to reject
    the connection immediately.

===========================================================================*/
LOCAL bt_error_code_type bt_rm_ev_hc_connection_req_sco
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_error_code_type  hc_reason;
  bt_rm_conn_type*    conn_ptr;
  uint16              i;

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr,
                   BT_SCO_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_sco == BT_RM_CS_DISCONNECTED )
    {
      conn_ptr->state_sco = BT_RM_CS_CONNECTING;

      bt_rm_update_remote_class_of_device(
        &ev_msg_ptr->ev_msg.ev_hc_conrq.class_of_device,
        conn_ptr->dev_ptr );

      if ( conn_ptr->sco_resps_pending != 0 )
      {
        BT_ERR( "BT RM: SCO Resps Pending %x RH %x HHS %x",
                conn_ptr->sco_resps_pending,
                conn_ptr->rm_handle, conn_ptr->hc_handle_sco );

        conn_ptr->sco_resps_pending = 0;
      }

      status = BT_CS_RM_MAX_APPLICATIONS;
      for ( i = 0; i < BT_RM_MAX_APPS; i++ )
      {
        if ( bt_rm_app[ i ].sco_registered != FALSE )
        {
          conn_ptr->sco_resps_pending++;
          bt_rm_ev_send_connection_req( bt_rm_app[ i ].bt_app_id,
                                        conn_ptr, BT_SCO_LINK );
          status = BT_CS_GN_SUCCESS;
        }
      }

      if ( status != BT_CS_GN_SUCCESS )
      {
        bt_rm_init_link( conn_ptr, BT_SCO_LINK );
      }
    }
    else
    {
      status = bt_rm_accept_connection_sco( conn_ptr,
                                            BT_APP_ID_NULL );
    }
  }

  if ( status == BT_CS_GN_SUCCESS )
  {
    hc_reason = BT_BE_SUCCESS;
  }
#if 0
/* BT_TBD */
  else if ( status == BT_CS_GN_RETRY_CMD_LATER )
  {
    hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;
  }
#endif
  else
  {
    hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;
  }

  return ( hc_reason );

}


#ifdef BT_SWDEV_RADIO_OFF
/*===========================================================================

FUNCTION
  bt_rm_device_in_black_list

DESCRIPTION
  Determines if a specified device is in the Device Black List.

RETURN VALUE
  TRUE   If device is in the Device Black List
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_device_in_black_list
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  uint16   i;
  boolean  device_in_black_list = FALSE;

  for ( i = 0; i < bt_rm_data.dev_black_list_count; i++ )
  {
    if ( BT_BD_ADDRS_EQUAL( &bt_rm_data.dev_black_list[ i ],
                              bd_addr_ptr ) != FALSE )
    {
      device_in_black_list = TRUE;

      BT_BDA( MSG_HIGH, "BT RM: Dev In Black List", bd_addr_ptr );

      break;
    }
  }

  return ( device_in_black_list );

}


/*===========================================================================

FUNCTION
  bt_rm_device_in_white_list

DESCRIPTION
  Determines if a specified device is in the Device White List.

RETURN VALUE
  TRUE   If device is in the Device White List
  FALSE  Otherwise

===========================================================================*/
LOCAL boolean bt_rm_device_in_white_list
(
  bt_bd_addr_type*  bd_addr_ptr
)
{

  uint16   i;
  boolean  device_in_white_list = FALSE;

  for ( i = 0; i < bt_rm_data.dev_white_list_count; i++ )
  {
    if ( BT_BD_ADDRS_EQUAL( &bt_rm_data.dev_white_list[ i ],
                              bd_addr_ptr ) != FALSE )
    {
      device_in_white_list = TRUE;
      break;
    }
  }

  if ( device_in_white_list == FALSE )
  {
    BT_BDA( MSG_HIGH, "BT RM: Dev NOT In White List", bd_addr_ptr );
  }

  return ( device_in_white_list );

}
#endif /* BT_SWDEV_RADIO_OFF */




#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_calculate_path_loss

DESCRIPTION
  Calculates the path loss

===========================================================================*/
void bt_rm_calculate_path_loss
(
  bt_rm_device_type*    dev_ptr,
  boolean               received_tx_power,
  boolean               received_rssi
)
{
  /* Default value is max */
  int16 dev_rank = BT_MAX_PATHLOSS;
  int16 tx_power = 0;

  if( received_tx_power == TRUE &&
      received_rssi     == TRUE )
  {
    /* Extended Inquiry Result event with TX power */
    tx_power = dev_ptr->eir_tx_power_level;
    dev_rank = dev_ptr->eir_tx_power_level - dev_ptr->inq_resp_rssi;
  }
  else if( received_tx_power == FALSE &&
           received_rssi == TRUE )
  {
    /* Extended Inquiry Result w/o TX power or Inq Result(RSSI) */
    tx_power = BT_TX_PWR_CLASS_2_DEVICE;
    if( dev_ptr->inq_resp_rssi > BT_TX_PWR_CLASS_2_DEVICE )
    {
      tx_power = BT_TX_PWR_CLASS_1_DEVICE;
    }
    dev_rank = tx_power - dev_ptr->inq_resp_rssi;
  }

  dev_ptr->dev_public.eir_data.device_ranking = (dev_rank < 0 ? 0: dev_rank);

  BT_MSG_DEBUG( "BT RM: Calc Pathloss, pwr: %x, loss: %x, I %x ",
                tx_power, dev_rank, dev_ptr->index );
}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_connection_request

DESCRIPTION
  Processes a HC Connection Request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_connection_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_link_type        link_type;
  bt_error_code_type  hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;

  link_type = ev_msg_ptr->ev_msg.ev_hc_conrq.link_type;

  BT_MSG_API( "BT RM EV RX: HC ConnR LT %x COD %x",
              ev_msg_ptr->ev_msg.ev_hc_conrq.link_type,
              ev_msg_ptr->ev_msg.ev_hc_conrq.
                class_of_device.cod_bytes[ 0 ], 0 );
  BT_BDA( MSG_API, "BT RM EV RX: HC ConnR",
          &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr );

#ifdef BT_SWDEV_RADIO_OFF
  if ( ((bt_rm_data.dev_white_list_count == 0) ||
        (bt_rm_device_in_white_list(
           &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr ) != FALSE)) &&
       (bt_rm_device_in_black_list(
          &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr ) == FALSE) )
  {
#endif /* BT_SWDEV_RADIO_OFF */
    if ( link_type == BT_ACL_LINK )
    {
      hc_reason = bt_rm_ev_hc_connection_req_acl( ev_msg_ptr );
    }
#ifdef FEATURE_BT_1_2
    else if ( ( link_type == BT_SCO_LINK ) || ( link_type == BT_ESCO_LINK ) )
#else
    else if ( link_type == BT_SCO_LINK )
#endif /* FEATURE_BT_1_2 */
    {
      hc_reason = bt_rm_ev_hc_connection_req_sco( ev_msg_ptr );
    }
    else
    {
      hc_reason = BT_BE_CONN_REJECTED_NO_RESOURCE;

      BT_ERR( "BT RM: Bad HC ConnR LT %x COD %x",
              link_type,
              ev_msg_ptr->ev_msg.ev_hc_conrq.
                class_of_device.cod_bytes[ 0 ], 0 );
      BT_BDA( ERR, "BT RM: Bad HC ConnR",
              &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr );
    }
#ifdef BT_SWDEV_RADIO_OFF
  }
  else
  {
    hc_reason = BT_BE_LOCAL_HOST_TERMINATED_CONNECTION;
  }
#endif /* BT_SWDEV_RADIO_OFF */

  if ( hc_reason != BT_BE_SUCCESS )
  {
    BT_MSG_API( "BT RM CMD TX: HC Rej Conn LT %x R %x",
                link_type, hc_reason, 0 );
    BT_BDA( MSG_API, "BT RM CMD TX: HC Rej Conn",
            &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr );

#ifdef FEATURE_BT_1_2
    if ( ( link_type == BT_SCO_LINK ) || ( link_type == BT_ESCO_LINK ) )
    {
      bt_cmd_hc_reject_sync_conn_req( &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr,
                                      (uint8)(hc_reason) );
    }
    else
#endif /* FEATURE_BT_1_2 */
    {
      bt_cmd_hc_reject_connection(
        &ev_msg_ptr->ev_msg.ev_hc_conrq.bd_addr, (uint8)(hc_reason) );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_disconnection_complete

DESCRIPTION
  Processes a HC Disconnection Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_disconnection_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;
  uint16              hc_handle;
  bt_error_code_type  hc_reason;
  bt_link_type        link_type;
#ifdef BT_SWDEV_2_1_SSP
  bt_cmd_msg_type     cmd;
#endif /* BT_SWDEV_2_1_SSP */
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
  bt_role_type        link_role = BT_ROLE_NONE;
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_disc.status);
  hc_handle = ev_msg_ptr->ev_msg.ev_hc_disc.conn_hndl;
  hc_reason = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_disc.reason);

  BT_MSG_API( "BT RM EV RX: HC DiscC S %x R %x H %x",
              hc_status, hc_reason, hc_handle );

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->hc_handle_acl == hc_handle )
    {
      link_type = BT_ACL_LINK;

#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
      link_role = conn_ptr->role;
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */

      bt_rm_update_conn_hc_error( conn_ptr,
                    (bt_error_code_type)(BT_HCI_REASON( hc_reason )) );

      bt_rm_finish_pending_sec_ui( conn_ptr,
                                    (BT_HCI_REASON( hc_status )) );

      bt_rm_finish_bond_attempt( conn_ptr );

      bt_rm_pin_req_resp_failed( conn_ptr,
                                 (BT_HCI_REASON( hc_reason )) );

      if ( conn_ptr->rname_app_id != BT_APP_ID_NULL)
      {
        bt_rm_finish_remote_name_request( conn_ptr, BT_CS_RM_NAME_REQ_FAILED );
      }

      switch ( conn_ptr->state_sco )
      {
      case BT_RM_CS_CONNECTING:
        bt_rm_ev_send_connection_failed( conn_ptr, BT_SCO_LINK, (BT_HCI_REASON( hc_reason )) );
        break;

      case BT_RM_CS_CONNECTED:
      case BT_RM_CS_DISCONNECTING:
        bt_rm_ev_send_disconnected( conn_ptr, BT_SCO_LINK, (BT_HCI_REASON( hc_reason )) );
        break;

      default:
        break;
      }

#ifdef BT_SWDEV_2_1_SSP

      /* If non-bonded link key, unbond first */
      if ((conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_AUTH_UNBONDED) ||
          (conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_UNBONDED) ||
          ((conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_DEBUG) &&
           (conn_ptr->dev_ptr->bonded_link_key != TRUE)))
      {
        BT_MSG_DEBUG( "BT RM: SSP Forced Unbond of !BLK/DBLK BLK %x DBLK %x",
                      conn_ptr->dev_ptr->bonded_link_key,
                      bt_rm_data.sm4_debug_enable, 0 );
        BT_BDA( MSG_DEBUG, "BT RM: SSP Forced Unbond of !BLK/DBLK",
                &conn_ptr->dev_ptr->dev_public.bd_addr );

        cmd.cmd_hdr.cmd_type             = BT_CMD_RM_UNBOND;
        cmd.cmd_hdr.bt_app_id            = BT_APP_ID_NULL;
        cmd.cmd_msg.cmd_rm_unbnd.bd_addr =
          conn_ptr->dev_ptr->dev_public.bd_addr;

        bt_rm_cmd_unbond( &cmd );

        if ( cmd.cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT RM: SSP Bad Unbond CS %x",
                  cmd.cmd_hdr.cmd_status, 0, 0 );
        }
        conn_ptr->pin_code_reply_neg_sent = FALSE;
      }
#endif /* BT_SWDEV_2_1_SSP */

    }
    else
    {
      link_type = BT_SCO_LINK;
    }

    bt_rm_ev_send_disconnected( conn_ptr, link_type,
                                (BT_HCI_REASON( hc_reason )) );

    if ( hc_status == BT_BE_SUCCESS )
    {
      bt_rm_init_link( conn_ptr, link_type );
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
      if ( link_type == BT_ACL_LINK )
      {
        if (link_role == BT_ROLE_SLAVE )
        {
          /* Link with slave role disconnected. Check if scan is needed */
          bt_rm_check_scan_settings();
        }

        if ( bt_rm_total_active_acl_links() == 1 )
        {
          /* One ACL link left. Revert role sw link policy */
          bt_rm_update_all_role_switch_req_policy(
                                    bt_rm_data.create_conn_role_sw_req_ok );
        }
      }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Bad HC DiscC S %x R %x H %x",
                    hc_status, hc_reason, hc_handle );
    }
#ifdef FEATURE_BT_ENABLE_SLEEP_VOTING
#error code not present
#endif

#ifdef BT_AMS_DISABLED
    bt_rm_check_scan_settings();
#endif /* BT_AMS_DISABLED */
  }
  else
  {
    BT_ERR( "BT RM: Bad HC DiscC H %x S %x R %x",
            hc_handle, hc_status, hc_reason );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_encryption_change

DESCRIPTION
  Processes a HC Encryption Change event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_encryption_change
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*        conn_ptr;
  bt_error_code_type      hc_status;
  uint16                  hc_handle;
  bt_encrypt_enable_type  encrypt_enable;
  boolean                 re_pair_started = FALSE;

  hc_status      = (bt_error_code_type)
                      (ev_msg_ptr->ev_msg.ev_hc_encch.status);
  hc_handle      = ev_msg_ptr->ev_msg.ev_hc_encch.conn_hndl;
  encrypt_enable = (bt_encrypt_enable_type)ev_msg_ptr->ev_msg.ev_hc_encch.encrypt_enable;

  BT_MSG_API( "BT RM EV RX: HC EncCh E %x S %x H %x",
              encrypt_enable, hc_status, hc_handle );

  if ( hc_status != BT_BE_SUCCESS )
  {
    BT_MSG_DEBUG( "BT RM: Encrypt failed S %x H %x E %x",
                  hc_status, hc_handle, encrypt_enable );
  }

  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
#ifdef BT_SWDEV_2_1_SSP
    if ( conn_ptr->sec_level_request != BT_RM_SL_0_NONE )
    {
      conn_ptr->sec_level_request = BT_RM_SL_0_NONE;
#else
    if ( conn_ptr->security_request == BT_SEC_AUTHENTICATE_AND_ENCRYPT )
    {
      conn_ptr->security_request = BT_SEC_NONE;
#endif /* BT_SWDEV_2_1_SSP */

      if ( (re_pair_started =
              bt_rm_serv_sec_re_pair_started( conn_ptr,
                                              hc_status )) == FALSE )
      {
        bt_rm_update_conn_hc_error( conn_ptr, hc_status );
      }
    }

    if ( re_pair_started == FALSE )
    {
      conn_ptr->encrypt_enable = encrypt_enable;
/*
      if( ( encrypt_enable == BT_ENCRYPT_ENABLE_OFF ) &&
          ( hc_status == BT_BE_SUCCESS ) )
      {
        if( conn_ptr->enh_enc_state == BT_RM_ENH_ENC_OK )
        {
#ifdef BT_SWDEV_2_1_SSP
            if (IS_SSP_DOABLE() == TRUE)
            {
              // Remote 2.1 device disabled encryption. This is
                // against the spec. Disconnect from rogue device. 
              BT_MSG_DEBUG("BT RM: SSP Remote 2.1 device turned off enc. Disconnecting.",
                           conn_ptr->enh_enc_state,0,0);
              bt_rm_disconnect_acl( conn_ptr, 
                                    ( bt_rm_disc_reason_type )BT_BE_CONN_REJECTED_SECURITY_FAILURE );
            }
            else
#endif // BT_SWDEV_2_1_SSP
            {
              BT_MSG_DEBUG("BT RM: Starting enh enc timer enc_state = %x",
                           conn_ptr->enh_enc_state,0,0);
              conn_ptr->mss_enh_enc_timeout_ms_left = BT_RM_MSS_ENC_TIMEOUT_MS;
              conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OFF;
            }
        }
        else if(conn_ptr->enh_enc_state == BT_RM_ENH_ENC_STARTED)
        {
            BT_MSG_DEBUG("BT RM: enh enc failed giving up enc_state = %x",
                                                conn_ptr->enh_enc_state,0,0);
            conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
        }
        else if(conn_ptr->enh_enc_state == BT_RM_ENH_ENC_DISABLING)
        {
            // Trying to do re-pairing start authentication which will triger re-pairing 
            BT_MSG_DEBUG("BT RM: Starting re-pairing enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
            conn_ptr->enh_enc_state = BT_RM_ENH_ENC_AUTHENTICATING;
            bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
#ifdef BT_SWDEV_2_1_SSP
            conn_ptr->pairing_initiated = TRUE;
#endif // BT_SWDEV_2_1_SSP 
        }
        else
        {
            BT_MSG_DEBUG("BT RM: No action required enc_state = %x",conn_ptr->enh_enc_state,0,0);
            conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
        }
      }
      else
*/	  	
      {
#ifdef BT_SWDEV_2_1_SSP
        conn_ptr->authenticated = TRUE;
#endif /* BT_SWDEV_2_1_SSP */
        conn_ptr->enh_enc_state = BT_RM_ENH_ENC_OK;
        conn_ptr->mss_enh_enc_timeout_ms_left = 0;

#if ( defined ( FEATURE_BT_QSOC ) && defined ( FEATURE_BT_EXTPF_SAP ) )
#error code not present
#endif /* FEATURE_BT_QSOC */
      }

      bt_rm_security_updated( conn_ptr );

      bt_rm_link_status_updated( conn_ptr );
      if(conn_ptr->enh_enc_state == BT_RM_ENH_ENC_OK)
      {
        bt_rm_finish_service_security( conn_ptr );
      }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad EncC H %x S %x E %x",
            hc_handle, hc_status, encrypt_enable );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_inquiry_complete

DESCRIPTION
  Processes a HC Inquiry Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_inquiry_complete
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  BT_MSG_API( "BT RM EV RX: HC InqC St %x",
              ev_msg_ptr->ev_msg.ev_hc_inqcp.status, 0, 0 );

  if ( bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL )
  {
    if ( ev_msg_ptr->ev_msg.ev_hc_inqcp.status != BT_BE_SUCCESS )
    {
      BT_ERR( "BT RM: Bad HC InqC St %x N %x IAID %x",
              ev_msg_ptr->ev_msg.ev_hc_inqcp.status,
              bt_rm_data.inq_responses,
              bt_rm_data.inq_bt_app_id );
    }

    bt_rm_ev_send_inquiry_complete(
      bt_rm_data.inq_bt_app_id,
      bt_rm_data.inq_responses );

    bt_rm_init_inquiry_data();

    bt_dc.inq_active = FALSE;

    /* Send RM radio activity event */
    BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQUIRING );
    bt_rm_ev_send_radio_activity();

    if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
       (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
       (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
	{
      /* Re-evaluate required scans */
      bt_rm_check_scan_settings();
    }
  }
  else
  {
    BT_ERR( "BT RM: Unexp HC InqC St %x",
            ev_msg_ptr->ev_msg.ev_hc_inqcp.status, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_inquiry_result

DESCRIPTION
  Processes a HC Inquiry Result event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_inquiry_result
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  dsm_item_type*              dsm_ptr;
  bt_rm_device_type*          dev_ptr;
  bt_bd_addr_type             bd_addr;
  bt_page_scan_rep_mode_type  page_scan_rep_mode;
  bt_page_scan_per_mode_type  page_scan_per_mode;
  bt_page_scan_mode_type      page_scan_mode;
  bt_cod_type                 class_of_device;
  uint16                      clk_offset;
  bt_ev_msg_type              ev_rm_ir;
  uint8                       num_inq_responses;
  uint8                       i;

  num_inq_responses = ev_msg_ptr->ev_msg.ev_hc_inqrs.num_responses;

  BT_MSG_API( "BT RM EV RX: HC InqR NR %x InqAID %x",
              num_inq_responses, bt_rm_data.inq_bt_app_id, 0 );

  dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_inqrs.dsm_ptr;

  if ( (bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL) &&
       (num_inq_responses != 0) &&
       (dsm_ptr != NULL) &&
       (dsm_length_packet( dsm_ptr ) >=
          (num_inq_responses *
           (sizeof( bt_bd_addr_type ) +
            sizeof( bt_page_scan_rep_mode_type ) +
            sizeof( bt_page_scan_per_mode_type ) +
            sizeof( bt_page_scan_mode_type ) +
            sizeof( bt_cod_type ) +
            sizeof( uint16 )))) &&
       ((bt_rm_data.inq_max_resps == 0) ||
        (bt_rm_data.inq_responses < bt_rm_data.inq_max_resps)) )
  {
     uint16 dsm_bdaddr_offset = 0;
     uint16 dsm_ps_rep_offset = dsm_bdaddr_offset +
                                num_inq_responses*sizeof( bt_bd_addr_type );
     /* reserved/unused field in BT 2.0 and above.*/
     uint16 dsm_ps_per_offset = dsm_ps_rep_offset +
                                num_inq_responses*sizeof( bt_page_scan_rep_mode_type );
     /* reserved/unused field in BT 2.0 and above.*/
     uint16 dsm_ps_mode_offset = dsm_ps_per_offset +
                                 num_inq_responses*sizeof( bt_page_scan_per_mode_type );
     uint16 dsm_cod_offset = dsm_ps_mode_offset +
                             num_inq_responses*sizeof( bt_page_scan_mode_type );
     uint16 dsm_clk_offset = dsm_cod_offset +
                             num_inq_responses*sizeof( bt_cod_type );

    /* Iterate num_inq_responses times through the dsm item
     * extracting one inquiry event in each iteration and sending
     * up if one has alredy not been sent for that device.
    */
    for ( i = 0; i < num_inq_responses; i++ )
    {
      dsm_extract( dsm_ptr, (dsm_bdaddr_offset+(i*sizeof( bt_bd_addr_type ))),
                   (void*) &bd_addr, sizeof( bt_bd_addr_type ) );

      BT_BDA( MSG_API, "BT RM EV RX: HC InqR", &bd_addr );

      dsm_extract( dsm_ptr,
                  (dsm_ps_rep_offset+(i*sizeof( bt_page_scan_rep_mode_type ))),
                  &page_scan_rep_mode, sizeof( bt_page_scan_rep_mode_type ) );

      dsm_extract( dsm_ptr,
                  (dsm_ps_per_offset+(i*sizeof( bt_page_scan_per_mode_type ))),
                  &page_scan_per_mode, sizeof( bt_page_scan_per_mode_type ) );

      dsm_extract( dsm_ptr,
                  (dsm_ps_mode_offset+(i*sizeof( bt_page_scan_mode_type ))),
                  &page_scan_mode, sizeof( bt_page_scan_mode_type ) );

      dsm_extract( dsm_ptr, (dsm_cod_offset+(i*sizeof( bt_cod_type ))),
                  (void*)&class_of_device, sizeof( bt_cod_type ) );

      dsm_extract( dsm_ptr, (dsm_clk_offset+(i*sizeof( uint16 ))),
                   &clk_offset, sizeof( uint16 ) );

      BT_MSG_DEBUG( "BT RM : HC InqR %x extracted. PSRM %x Clk %x ",
                    i, page_scan_rep_mode, clk_offset );

      if ( (dev_ptr = bt_rm_get_device( &bd_addr, TRUE )) != NULL )
      {
        if ( dev_ptr->inq_result_sent == FALSE )
        {
          if ( (dev_ptr->page_scan_rep_mode != page_scan_rep_mode) ||
               (dev_ptr->page_scan_per_mode != page_scan_per_mode) ||
               (dev_ptr->page_scan_mode     != page_scan_mode)     ||
               (dev_ptr->clk_offset         != (clk_offset | BT_CLK_OFFSET_VALID_V) ) )
          {
            dev_ptr->page_scan_rep_mode  = page_scan_rep_mode;
            dev_ptr->page_scan_per_mode  = page_scan_per_mode;
            dev_ptr->page_scan_mode      = page_scan_mode;
            dev_ptr->clk_offset          = clk_offset | BT_CLK_OFFSET_VALID_V;
            dev_ptr->update_status      |= BT_RM_DUS_INTERNAL_B;
          }

#ifdef BT_SWDEV_2_1_EIR
        /* Calculate path loss */
        bt_rm_init_eir_data( dev_ptr );
        bt_rm_calculate_path_loss( dev_ptr, FALSE, FALSE );
#endif /* BT_SWDEV_2_1_EIR */

          /*  Want this call last as it performs the EFS  */
          /*  write to the device database as necessary.  */
          bt_rm_update_remote_class_of_device( &class_of_device, dev_ptr );

          ev_rm_ir.ev_hdr.ev_type   = BT_EV_RM_INQUIRY_RESULT;
          ev_rm_ir.ev_hdr.bt_app_id = bt_rm_data.inq_bt_app_id;
          ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr =
            dev_ptr->dev_public.bd_addr;
          ev_rm_ir.ev_msg.ev_rm_inqrs.class_of_device =
            dev_ptr->dev_public.class_of_device;

          BT_MSG_API( "BT RM EV TX: RM InqR COD %02x %04x AID %x",
                      class_of_device.cod_bytes[ 2 ],
                      (uint16)((class_of_device.cod_bytes[ 1 ] << 8) |
                                class_of_device.cod_bytes[ 0 ]),
                      ev_rm_ir.ev_hdr.bt_app_id );
          BT_BDA( MSG_API, "BT RM EV TX: RM InqR",
                  &ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr );
          bt_ec_send_event( &ev_rm_ir );

          dev_ptr->inq_result_sent = TRUE;

          bt_rm_data.inq_responses++;
          if ((bt_rm_data.inq_max_resps != 0) &&
              (bt_rm_data.inq_responses >= bt_rm_data.inq_max_resps))
          {
            /* break out of the for loop */
            BT_MSG_HIGH( "BT RM: InqR cnt exceeded. InqR MR %x IR %x InqAID %x",
                         bt_rm_data.inq_max_resps, bt_rm_data.inq_responses,
                         bt_rm_data.inq_bt_app_id );
            break;
          }
        }
        else
        {
         /* Always update clk_offset field and set it to valid in ram copy */
         dev_ptr->clk_offset = clk_offset | BT_CLK_OFFSET_VALID_V;
          BT_BDA( MSG_DEBUG, "BT RM: Dup InqR", &bd_addr );
        }
      }
      else
      {
        BT_BDA( MSG_DEBUG, "BT RM: InqR Dropped", &bd_addr );
        /* Reached here because device table is full ..
         * Continue processing remaining inq response in this
         * event, even though they will be ignored either because
         * they are dups or because device table continues to be full.
        */
      }
    } /* end for loop */
  }
  else
  {
    BT_ERR( "BT RM: Bad HC InqR MR %x IR %x InqAID %x",
            bt_rm_data.inq_max_resps, bt_rm_data.inq_responses,
            bt_rm_data.inq_bt_app_id );
  }

  dsm_free_packet( &dsm_ptr );

}

#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_inquiry_result_rssi

DESCRIPTION
  Processes a HC Inquiry Result RSSI event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_inquiry_result_rssi
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  dsm_item_type*              dsm_ptr;
  bt_rm_device_type*          dev_ptr;
  bt_bd_addr_type             bd_addr;
  bt_page_scan_rep_mode_type  page_scan_rep_mode;
  bt_cod_type                 class_of_device;
  uint16                      clk_offset;
  uint8                       reserved;
  int8                        inq_resp_rssi;
  bt_ev_msg_type              ev_rm_ir;

  BT_MSG_API( "BT RM EV RX: HC InqR RSSI NR %x InqAID %x",
              ev_msg_ptr->ev_msg.ev_hc_inqrs.num_responses,
              bt_rm_data.inq_bt_app_id, 0 );

  dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_inqrs.dsm_ptr;

  if ( (bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL) &&
       (ev_msg_ptr->ev_msg.ev_hc_inqrs.num_responses == 1) &&
       (dsm_ptr != NULL) &&
       (dsm_length_packet( dsm_ptr ) >=
          (sizeof( bt_bd_addr_type ) +
           sizeof( bt_page_scan_rep_mode_type ) +
           sizeof( uint8 ) +
           sizeof( bt_cod_type ) +
           sizeof( uint16 ) +
           sizeof( int8 ) )) &&
       ((bt_rm_data.inq_max_resps == 0) ||
        (bt_rm_data.inq_responses < bt_rm_data.inq_max_resps)) )
  {
    /*   Inquiry Result structure       */
    /*   bt_bd_addr_type[]              */
    /*   bt_page_scan_rep_mode_type[]   */
    /*   uint8 reserved                 */
    /*   bt_cod_type[]                  */
    /*   uint16 clk_offset[]            */
    /*   uint8 rssi                     */
    dsm_pullup( &dsm_ptr, (void*) &bd_addr,
                sizeof( bt_bd_addr_type ) );

    BT_BDA( MSG_API, "BT RM EV RX: HC InqR Rssi", &bd_addr );

    dsm_pullup( &dsm_ptr, &page_scan_rep_mode,
                sizeof( bt_page_scan_rep_mode_type ) );
    dsm_pullup( &dsm_ptr, &reserved, sizeof( uint8 ) );
    dsm_pullup( &dsm_ptr, (void*)&class_of_device,
                sizeof( bt_cod_type ) );
    dsm_pullup( &dsm_ptr, &clk_offset, sizeof( uint16 ) );
    dsm_pullup( &dsm_ptr, &inq_resp_rssi, sizeof( int8 ) );

    if ( (dev_ptr = bt_rm_get_device( &bd_addr, TRUE )) != NULL )
    {
      if ( dev_ptr->inq_result_sent == FALSE )
      {
        if ( (dev_ptr->page_scan_rep_mode != page_scan_rep_mode) ||
             (dev_ptr->clk_offset         != (clk_offset | BT_CLK_OFFSET_VALID_V) ) )
        {
          dev_ptr->page_scan_rep_mode  = page_scan_rep_mode;
          dev_ptr->clk_offset          = clk_offset | BT_CLK_OFFSET_VALID_V;
          dev_ptr->update_status      |= BT_RM_DUS_INTERNAL_B;
        }

        /* Reset any EIR that may have been received from this device */
        bt_rm_init_eir_data( dev_ptr );

        /* Calculate path loss */
        dev_ptr->inq_resp_rssi = inq_resp_rssi;
        bt_rm_calculate_path_loss( dev_ptr, FALSE, TRUE );

        /*  Want this call last as it performs the EFS  */
        /*  write to the device database as necessary.  */
        bt_rm_update_remote_class_of_device( &class_of_device, dev_ptr );

        ev_rm_ir.ev_hdr.ev_type   = BT_EV_RM_INQUIRY_RESULT;
        ev_rm_ir.ev_hdr.bt_app_id = bt_rm_data.inq_bt_app_id;
        ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr =
          dev_ptr->dev_public.bd_addr;
        ev_rm_ir.ev_msg.ev_rm_inqrs.class_of_device =
          dev_ptr->dev_public.class_of_device;

        BT_MSG_API( "BT RM EV TX: RM InqR Rssi COD %02x %04x AID %x",
                    class_of_device.cod_bytes[ 2 ],
                    (uint16)((class_of_device.cod_bytes[ 1 ] << 8) |
                                class_of_device.cod_bytes[ 0 ]),
                    ev_rm_ir.ev_hdr.bt_app_id );
        BT_BDA( MSG_API, "BT RM EV TX: RM InqR Rssi",
                &ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr );
        bt_ec_send_event( &ev_rm_ir );

        dev_ptr->inq_result_sent = TRUE;

        bt_rm_data.inq_responses++;
      }
      else
      {
        /* Always update clk_offset field and set it to valid in ram copy */
        dev_ptr->clk_offset = clk_offset | BT_CLK_OFFSET_VALID_V;
        BT_BDA( MSG_DEBUG, "BT RM: Dup InqR Rssi", &bd_addr );

        /* We should use the highest received RSSI value */
        if( inq_resp_rssi > dev_ptr->inq_resp_rssi )
        {
          dev_ptr->inq_resp_rssi = inq_resp_rssi;
          bt_rm_calculate_path_loss( dev_ptr, FALSE, TRUE );
        }
      }
    }
    else
    {
      BT_BDA( MSG_DEBUG, "BT RM: InqR Rssi Dropped", &bd_addr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad HC InqR Rssi MR %x IR %x InqAID %x",
            bt_rm_data.inq_max_resps, bt_rm_data.inq_responses,
            bt_rm_data.inq_bt_app_id );
  }

  dsm_free_packet( &dsm_ptr );

}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_inquiry_result_extended

DESCRIPTION
  Processes a HC Extended Inquiry Result event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_inquiry_result_extended
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  dsm_item_type*              dsm_ptr;
  bt_rm_device_type*          dev_ptr;
  bt_bd_addr_type             bd_addr;
  bt_page_scan_rep_mode_type  page_scan_rep_mode;
  bt_cod_type                 class_of_device;
  uint16                      clk_offset;
  uint8                       reserved;
  int8                        inq_resp_rssi;
  uint8                       eir_data[ BT_MAX_EIR_HCI_LEN ];
           
  bt_ev_msg_type              ev_rm_ir;

  BT_MSG_API( "BT RM EV RX: HC InqR Ext NR %x InqAID %x",
              ev_msg_ptr->ev_msg.ev_hc_inqrs.num_responses,
              bt_rm_data.inq_bt_app_id, 0 );

  dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_inqrs.dsm_ptr;

  if ( (bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL) &&
       (ev_msg_ptr->ev_msg.ev_hc_inqrs.num_responses == 1) &&
       (dsm_ptr != NULL) &&
       (dsm_length_packet( dsm_ptr ) >=
          (sizeof( bt_bd_addr_type ) +
           sizeof( bt_page_scan_rep_mode_type ) +
           sizeof( uint8 ) +
           sizeof( bt_cod_type ) +
           sizeof( uint16 ) +
           sizeof( uint8 ) +
           BT_MAX_EIR_HCI_LEN )) &&
       ((bt_rm_data.inq_max_resps == 0) ||
        (bt_rm_data.inq_responses < bt_rm_data.inq_max_resps)) )
  {
    /*   Inquiry Result structure       */
    /*   bt_bd_addr_type[]              */
    /*   bt_page_scan_rep_mode_type[]   */
    /*   uint8 reserved                 */
    /*   bt_cod_type[]                  */
    /*   uint16 clk_offset[]            */
    /*   uint8 rssi                     */
    dsm_pullup( &dsm_ptr, (void*) &bd_addr,
                sizeof( bt_bd_addr_type ) );

    BT_BDA( MSG_API, "BT RM EV RX: HC InqRExt", &bd_addr );

    dsm_pullup( &dsm_ptr, &page_scan_rep_mode,
                sizeof( bt_page_scan_rep_mode_type ) );
    dsm_pullup( &dsm_ptr, &reserved, sizeof( uint8 ) );
    dsm_pullup( &dsm_ptr, (void*)&class_of_device,
                sizeof( bt_cod_type ) );
    dsm_pullup( &dsm_ptr, &clk_offset, sizeof( uint16 ) );
    dsm_pullup( &dsm_ptr, &inq_resp_rssi, sizeof( uint8 ) );

    dsm_pullup( &dsm_ptr, (void *)eir_data, BT_MAX_EIR_HCI_LEN );

    if ( (dev_ptr = bt_rm_get_device( &bd_addr, TRUE )) != NULL )
    {
      if ( dev_ptr->inq_result_sent == FALSE )
      {
        if ( (dev_ptr->page_scan_rep_mode != page_scan_rep_mode) ||
             (dev_ptr->clk_offset         != (clk_offset | BT_CLK_OFFSET_VALID_V) ) )
        {
          dev_ptr->page_scan_rep_mode  = page_scan_rep_mode;
          dev_ptr->clk_offset          = clk_offset | BT_CLK_OFFSET_VALID_V;
          dev_ptr->update_status      |= BT_RM_DUS_INTERNAL_B;
        }
        /* Extract contents of EIR */
        bt_rm_parse_eir( eir_data, dev_ptr );

        dev_ptr->inq_resp_rssi = inq_resp_rssi;

        /* Calculate path loss */
        bt_rm_calculate_path_loss( dev_ptr, dev_ptr->eir_tx_power_received, TRUE);

        /*  Want this call last as it performs the EFS  */
        /*  write to the device database as necessary.  */
        bt_rm_update_remote_class_of_device( &class_of_device, dev_ptr );

        ev_rm_ir.ev_hdr.ev_type   = BT_EV_RM_INQUIRY_RESULT;
        ev_rm_ir.ev_hdr.bt_app_id = bt_rm_data.inq_bt_app_id;
        ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr =
          dev_ptr->dev_public.bd_addr;
        ev_rm_ir.ev_msg.ev_rm_inqrs.class_of_device =
          dev_ptr->dev_public.class_of_device;

        BT_MSG_API( "BT RM EV TX: RM InqRExt COD %02x %04x AID %x",
                    class_of_device.cod_bytes[ 2 ],
                    (uint16)((class_of_device.cod_bytes[ 1 ] << 8) |
                                class_of_device.cod_bytes[ 0 ]),
                    ev_rm_ir.ev_hdr.bt_app_id );
        BT_BDA( MSG_API, "BT RM EV TX: RM InqRExt",
                &ev_rm_ir.ev_msg.ev_rm_inqrs.bd_addr );
        bt_ec_send_event( &ev_rm_ir );

        dev_ptr->inq_result_sent = TRUE;

        bt_rm_data.inq_responses++;
      }
      else
      {
        /* Always update clk_offset field and set it to valid in ram copy */
        dev_ptr->clk_offset = clk_offset | BT_CLK_OFFSET_VALID_V;
        BT_BDA( MSG_DEBUG, "BT RM: Dup InqRExt", &bd_addr );

        /* We should use the highest received RSSI value */
        if( inq_resp_rssi > dev_ptr->inq_resp_rssi )
        {
          dev_ptr->inq_resp_rssi = inq_resp_rssi;
          bt_rm_calculate_path_loss( dev_ptr, FALSE, TRUE );
        }

        /* If we get a duplicate event we process it only if the first one
           had the EIR bit set but no information */
        if ( !ISBITSET ( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B ) )
        {
          dev_ptr->inq_resp_rssi = inq_resp_rssi;
          bt_rm_parse_eir( eir_data, dev_ptr );
          bt_rm_calculate_path_loss( dev_ptr, dev_ptr->eir_tx_power_received, TRUE);
        }

      }
    }
    else
    {
      BT_BDA( MSG_DEBUG, "BT RM: InqRExt Dropped", &bd_addr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad HC InqRExt MR %x IR %x InqAID %x",
            bt_rm_data.inq_max_resps, bt_rm_data.inq_responses,
            bt_rm_data.inq_bt_app_id );
  }

  dsm_free_packet( &dsm_ptr );

}
#endif /* BT_SWDEV_2_1_EIR */



/*===========================================================================

FUNCTION
  bt_rm_ev_hc_link_key_request

DESCRIPTION
  Processes a HC Link Key Request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_link_key_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_bd_addr_type*    bd_addr_ptr;

  BT_BDA( MSG_API, "BT RM EV RX: HC LK Req",
          &ev_msg_ptr->ev_msg.ev_hc_keyrq.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_keyrq.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->paging == TRUE )
    { 
      bt_rm_do_bb_page_stopped( conn_ptr );
    }

    bd_addr_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;

#ifdef BT_SWDEV_2_1_SSP

    if ( ((conn_ptr->bonding_app_id == BT_APP_ID_NULL) ||
          (conn_ptr->bonding_key_rxd != FALSE))        &&
         (conn_ptr->dev_ptr->link_key_valid != FALSE) &&
         (conn_ptr->dev_ptr->link_key_pin_len_bytes >=
            conn_ptr->security_min_pin_len_bytes) )
    {
      if ( conn_ptr->update_link_key_request != FALSE )
      {
        BT_BDA( MSG_API, "BT RM CMD TX: HC SSP LK Rep Neg", bd_addr_ptr );
        BT_MSG_HIGH( "BT RM: SSP Upgrade LK UNAUTH->AUTH or NON-BONDED->BONDED",0,0,0);
        bt_cmd_hc_link_key_req_reply_neg( bd_addr_ptr );
      }
      else
      {
        BT_BDA( MSG_API, "BT RM CMD TX: HC SSP LK Rep Pos", bd_addr_ptr );
        bt_cmd_hc_link_key_req_reply( bd_addr_ptr,
                                      conn_ptr->dev_ptr->link_key );
      }
#else
    if ( ((conn_ptr->bonding_app_id == BT_APP_ID_NULL) ||
          (conn_ptr->bonding_key_rxd != FALSE))        &&
         (conn_ptr->dev_ptr->link_key_valid != FALSE) &&
         (conn_ptr->dev_ptr->link_key_pin_len_bytes >=
            conn_ptr->security_min_pin_len_bytes) &&
         ((conn_ptr->security_comb_key_only == FALSE) ||
          (conn_ptr->dev_ptr->dev_public.link_key_type ==
             BT_HC_LINK_KEY_COMB)) )
    {
      BT_BDA( MSG_API, "BT RM CMD TX: HC LK Rep Pos", bd_addr_ptr );
      bt_cmd_hc_link_key_req_reply( bd_addr_ptr,
                                    conn_ptr->dev_ptr->link_key );
#endif /* BT_SWDEV_2_1_SSP */
    }
    else
    {
      BT_BDA( MSG_API, "BT RM CMD TX: HC LK Rep Neg", bd_addr_ptr );
      bt_cmd_hc_link_key_req_reply_neg( bd_addr_ptr );

      bt_rm_reset_idle_timeout( conn_ptr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad LK Req St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad LK Req",
            &ev_msg_ptr->ev_msg.ev_hc_keyrq.bd_addr );

    BT_BDA( MSG_API, "BT RM CMD TX: HC LK Rep Neg",
            &ev_msg_ptr->ev_msg.ev_hc_keyrq.bd_addr );
    bt_cmd_hc_link_key_req_reply_neg(
      &ev_msg_ptr->ev_msg.ev_hc_keyrq.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_send_hc_pin_code_req_reply

DESCRIPTION
  Sends a HC PIN Code Request Reply command as specified.  If the PIN
  length is zero an HC PIN Code Request Reply Negative is sent instead.

===========================================================================*/
LOCAL void bt_rm_send_hc_pin_code_req_reply
(
  bt_rm_conn_type*  conn_ptr,
  uint8             pin_code_len,
  uint8*            pin_code_ptr
)
{

  if ( ( pin_code_len > 0 ) &&
       ( conn_ptr->re_pair_pin_state != BT_RM_RE_PAIR_PIN_WAITING ) )
  {
    conn_ptr->pin_code_reply_len_bytes = pin_code_len;
    conn_ptr->pin_code_reply_neg_sent  = FALSE;

    BT_BDA( MSG_API, "BT RM CMD TX: HC PIN Rep Pos",
            &conn_ptr->dev_ptr->dev_public.bd_addr );
    BT_MSG_API( "BT RM CMD TX: HC PIN Rep Pos L %x",
                pin_code_len, 0, 0 );
    bt_cmd_hc_pin_code_req_reply(
      &conn_ptr->dev_ptr->dev_public.bd_addr,
      pin_code_len, pin_code_ptr );

    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else if ( conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_WAITING )
  {
      if( ( pin_code_len > 0 ) &&
          ( pin_code_len <= BT_MAX_PIN_CODE_LEN ) )
      {
          /* Valid Pin go ahead with re-pairing */
          conn_ptr->re_pair_pin_state = BT_RM_RE_PAIR_PIN_READY;
          /* Store PIN */
          conn_ptr->bonding_pin.length  = pin_code_len;
          memcpy((uint8*)conn_ptr->bonding_pin.pin_code,pin_code_ptr,
                                            conn_ptr->bonding_pin.length);
          /* Start re-pairng after switching off Encryption */
          if( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON )
          {
              BT_MSG_DEBUG( "BT RM : Disabling encryption enh_enc_state %x",
                                                conn_ptr->enh_enc_state,0,0 );
              conn_ptr->enh_enc_state = BT_RM_ENH_ENC_DISABLING;
              bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                                BT_ENCRYPT_ENABLE_OFF );
          }
          else
          {
              BT_MSG_DEBUG( "BT RM: Starting re-pairing enh_enc_state %x",
                                                conn_ptr->enh_enc_state,0,0 );
              conn_ptr->enh_enc_state = BT_RM_ENH_ENC_AUTHENTICATING_ONLY;
              bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
          }
      }
      else
      {
          /* Invalid Pin go ahead and send security results */
          bt_rm_ev_send_security_result( conn_ptr );
      }
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: HC PIN Rep Pos L %x Made Neg",
                  pin_code_len, 0, 0 );
    bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_pin_code_request

DESCRIPTION
  Processes a HC PIN Code Request event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_pin_code_request
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_bd_addr_type*    bd_addr_ptr;
  bt_ev_msg_type      ev_rm_pcr;
  boolean             send_name_req = FALSE;
  uint16              i;

#ifdef BT_SWDEV_2_1_SSP
  BT_BDA( MSG_API, "BT RM EV RX: HC SSP PIN Req",
          &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->paging == TRUE )
    { 
      bt_rm_do_bb_page_stopped( conn_ptr );
    }

    bd_addr_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;

#ifdef FEATURE_BT_SOC1
#error code not present
#endif

    if ( (( conn_ptr->bonding_app_id != BT_APP_ID_NULL ) &&
         ( conn_ptr->bonding_pin.length > 0 )) ||
         ( conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_READY ) )
    {
      /*
       * The pre-fetching of the PIN can lead us to come here and we
       * need to handle that case as well
       */
      bt_rm_send_hc_pin_code_req_reply(
        conn_ptr, conn_ptr->bonding_pin.length,
        (uint8*)conn_ptr->bonding_pin.pin_code );
    }
    else
    {
      if ( (conn_ptr->pin_req_resps_pending == 0) &&
           (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) )
      {
        conn_ptr->pin_code_reply_len_bytes = 0;
        conn_ptr->pin_code_reply_neg_sent  = FALSE;

        ev_rm_pcr.ev_hdr.ev_type = BT_EV_RM_PIN_REQUEST;
        ev_rm_pcr.ev_msg.ev_rm_pinrq.bd_addr = *bd_addr_ptr;

        for ( i = 0; i < BT_RM_MAX_APPS; i++ )
        {
          if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
               (bt_rm_app[ i ].bondable != FALSE) )
          {
              if (strlen (conn_ptr->dev_ptr->dev_public.name_str) != 0)
              {
                conn_ptr->remote_name_available = TRUE; 
              }

              if(conn_ptr->remote_name_available == FALSE)
              {
                send_name_req = TRUE;
                break;
              }
              else
              {
                conn_ptr->pin_req_resps_pending++;
                ev_rm_pcr.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

                BT_MSG_API( "BT RM EV TX: PIN Req AID %x",
                            ev_rm_pcr.ev_hdr.bt_app_id, 0, 0 );
                BT_BDA( MSG_API, "BT RM EV TX: PIN Req", bd_addr_ptr );
                bt_ec_send_event( &ev_rm_pcr );
              }
          }
        }

        if ( send_name_req != FALSE )
        {
            /* Send Pin request after doing name request */
            conn_ptr->send_pin_req_pending = TRUE;
            conn_ptr->rname_app_id = bt_rm_app_id;
            bt_cmd_hc_remote_name_req( conn_ptr->dev_ptr->dev_public.bd_addr,
                                       conn_ptr->dev_ptr->page_scan_rep_mode,
                                       conn_ptr->dev_ptr->page_scan_mode,
                                       conn_ptr->dev_ptr->clk_offset );
        }
        else if ( conn_ptr->pin_req_resps_pending != 0 )
        {
          conn_ptr->pin_req_timeout_ms_left = BT_RM_REQ_TIMEOUT_MS;

          bt_rm_reset_idle_timeout( conn_ptr );
        }
        else
        {
          BT_MSG_DEBUG( "BT RM: PINR but not bondable H %x",
                        conn_ptr->rm_handle, 0, 0 );
          bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
        }
      }
      else
      {
        BT_MSG_DEBUG( "BT RM: Ignoring PINR PC %x AID %x TO %x",
                      conn_ptr->pin_req_resps_pending,
                      conn_ptr->pin_req_resp_app_id,
                      conn_ptr->pin_req_timeout_ms_left );
      }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad PIN Req St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad PIN Req",
            &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );

    bt_rm_send_hc_pin_reply_negative(
      NULL, &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );
  }
#else
  BT_BDA( MSG_API, "BT RM EV RX: HC PIN Req",
          &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->paging == TRUE )
    { 
      bt_rm_do_bb_page_stopped( conn_ptr );
    }

    bd_addr_ptr = &conn_ptr->dev_ptr->dev_public.bd_addr;

#if ( defined (FEATURE_BT_HOST_DIS_SCATTERNET) ) || \
    ( defined (FEATURE_BT_SOC1) && !defined (FEATURE_BT_SOC1_CNTLR_EN_SCATTERNET) )
    /* There are configuration parameters (PSKEYS) in SOC1 allowing
       the controller to be configured to prevent scatternet.
       But, to allow AMS while disabling scatternet, additonal
       logic below is required in HOST.
       However, if FEATURE_BT_HOST_DIS_SCATTERNET is defined, host will
       assume that controller does not check for scatternet.
    */
    if ( ( bt_rm_total_active_acl_links() > 1 ) &&
         ( conn_ptr->role != BT_ROLE_MASTER ) )
    {
      BT_MSG_HIGH( "BT RM: Scatternet. PR before SW, neg reply.",0,0,0 );
      bt_cmd_hc_pin_code_req_reply_neg( bd_addr_ptr );
      return; /* bail out !! */
    }
#endif
    if ( ( conn_ptr->bonding_app_id != BT_APP_ID_NULL ) ||
         ( conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_READY ) )
    {
      bt_rm_send_hc_pin_code_req_reply(
        conn_ptr, conn_ptr->bonding_pin.length,
        (uint8*)conn_ptr->bonding_pin.pin_code );
    }
    else
    {
      if ( (conn_ptr->pin_req_resps_pending == 0) &&
           (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) )
      {
        conn_ptr->pin_code_reply_len_bytes = 0;
        conn_ptr->pin_code_reply_neg_sent  = FALSE;

        ev_rm_pcr.ev_hdr.ev_type = BT_EV_RM_PIN_REQUEST;
        ev_rm_pcr.ev_msg.ev_rm_pinrq.bd_addr = *bd_addr_ptr;

        for ( i = 0; i < BT_RM_MAX_APPS; i++ )
        {
          if ( (bt_rm_app[ i ].bt_app_id != BT_APP_ID_NULL) &&
               (bt_rm_app[ i ].bondable != FALSE) )
          {
              if(conn_ptr->remote_name_available == FALSE)
              {
                send_name_req = TRUE;
                break;
              }
              else
              {
                conn_ptr->pin_req_resps_pending++;
                ev_rm_pcr.ev_hdr.bt_app_id = bt_rm_app[ i ].bt_app_id;

                BT_MSG_API( "BT RM EV TX: PIN Req AID %x",
                            ev_rm_pcr.ev_hdr.bt_app_id, 0, 0 );
                BT_BDA( MSG_API, "BT RM EV TX: PIN Req", bd_addr_ptr );
                bt_ec_send_event( &ev_rm_pcr );
              }
          }
        }

        if ( send_name_req != FALSE )
        {
            /* Send Pin request after doing name request */
            conn_ptr->send_pin_req_pending = TRUE;
            conn_ptr->rname_app_id = bt_rm_app_id;
            bt_cmd_hc_remote_name_req( conn_ptr->dev_ptr->dev_public.bd_addr,
                                       conn_ptr->dev_ptr->page_scan_rep_mode,
                                       conn_ptr->dev_ptr->page_scan_mode,
                                       conn_ptr->dev_ptr->clk_offset );
        }
        else if ( conn_ptr->pin_req_resps_pending != 0 )
        {
          conn_ptr->pin_req_timeout_ms_left = BT_RM_REQ_TIMEOUT_MS;

          bt_rm_reset_idle_timeout( conn_ptr );
        }
        else
        {
          BT_MSG_DEBUG( "BT RM: PINR but not bondable H %x",
                        conn_ptr->rm_handle, 0, 0 );
          bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
        }
      }
      else
      {
        BT_MSG_DEBUG( "BT RM: Ignoring PINR PC %x AID %x TO %x",
                      conn_ptr->pin_req_resps_pending,
                      conn_ptr->pin_req_resp_app_id,
                      conn_ptr->pin_req_timeout_ms_left );
      }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad PIN Req St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad PIN Req",
            &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );

    bt_rm_send_hc_pin_reply_negative(
      NULL, &ev_msg_ptr->ev_msg.ev_hc_pinrq.bd_addr );
  }
#endif /* BT_SWDEV_2_1_SSP */
}


/*===========================================================================

FUNCTION
  bt_rm_update_device_link_key_info

DESCRIPTION
  Updates the link key information a specified device as necessary.

===========================================================================*/
LOCAL void bt_rm_update_device_link_key_info
(
  bt_rm_conn_type*          conn_ptr,
  bt_ev_hc_key_notif_type*  lkn_ptr
)
{

  uint8               i;
  bt_rm_device_type*  dev_ptr = conn_ptr->dev_ptr;

  for ( i = 0; i < sizeof( bt_link_key_type ); i++ )
  {
    if ( dev_ptr->link_key.link_key_bytes[ i ] !=
           lkn_ptr->link_key.link_key_bytes[ i ] )
    {
      dev_ptr->link_key       = lkn_ptr->link_key;
      dev_ptr->update_status |= BT_RM_DUS_INTERNAL_B;
      break;
    }
  }

  if ( (dev_ptr->dev_public.link_key_type != lkn_ptr->key_type_key ) ||
       (dev_ptr->link_key_valid == FALSE) )
  {
    dev_ptr->dev_public.link_key_type  = lkn_ptr->key_type_key;
    dev_ptr->link_key_valid            = TRUE;
    dev_ptr->update_status            |= BT_RM_DUS_INTERNAL_B;

#ifdef BT_SWDEV_ESS_TEST
    if ( bt_rm_ess_test_lkt != BT_HC_LINK_KEY_COMB )
    {
        dev_ptr->dev_public.link_key_type = bt_rm_ess_test_lkt;
    }
#endif /* BT_SWDEV_ESS_TEST */

  }

  if ( (conn_ptr->pin_code_reply_len_bytes != 0) &&
       (dev_ptr->link_key_pin_len_bytes !=
          conn_ptr->pin_code_reply_len_bytes) )
  {
    dev_ptr->link_key_pin_len_bytes  = conn_ptr->pin_code_reply_len_bytes;
    dev_ptr->update_status          |= BT_RM_DUS_INTERNAL_B;
  }

#ifdef BT_SWDEV_2_1_SSP
  if ( (dev_ptr->dev_public.bonded == FALSE) ||
       (dev_ptr->dev_public.sec_level != conn_ptr->sec_level) )
  {
    dev_ptr->dev_public.bonded    = TRUE;
    dev_ptr->dev_public.sec_level = conn_ptr->sec_level; //TODO - is this right?
    dev_ptr->update_status       |= BT_RM_DUS_BONDED_B;
  }
#else
  if ( (dev_ptr->dev_public.bonded == FALSE) ||
       (dev_ptr->dev_public.security != BT_SEC_AUTHENTICATE_AND_ENCRYPT) )
  {
    dev_ptr->dev_public.bonded    = TRUE;
    dev_ptr->dev_public.security  = BT_SEC_AUTHENTICATE_AND_ENCRYPT;
    dev_ptr->update_status       |= BT_RM_DUS_BONDED_B;
  }
#endif /* BT_SWDEV_2_1_SSP */

  if ( dev_ptr->dev_public.valid == FALSE )
  {
    dev_ptr->dev_public.valid  = TRUE;
    dev_ptr->update_status    |= BT_RM_DUS_ADDED_B;
  }

  if ( dev_ptr->update_status != BT_RM_DUS_NONE )
  {
    bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_link_key_notification

DESCRIPTION
  Processes a HC Link Key Notification event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_link_key_notification
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
#ifdef BT_SWDEV_2_1_SSP
  boolean             security_failure=FALSE;
#endif /* BT_SWDEV_2_1_SSP */

  BT_BDA( MSG_API, "BT RM EV RX: HC LK Notif",
          &ev_msg_ptr->ev_msg.ev_hc_keynt.bd_addr );

  BT_MSG_HIGH( "BT RM: SSP Link Key Notif LKT %x",
               ev_msg_ptr->ev_msg.ev_hc_keynt.key_type_key, 0,0);

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_keynt.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {

#ifdef BT_SWDEV_2_1_SSP
    conn_ptr->authenticated = TRUE;
    conn_ptr->new_link_key = TRUE;
    conn_ptr->update_link_key_request = FALSE;
    switch ( ev_msg_ptr->ev_msg.ev_hc_keynt.key_type_key )
    {
      case BT_HC_LINK_KEY_DEBUG:
        conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_DEBUG;
        conn_ptr->dev_ptr->authenticated_link_key = TRUE;
        conn_ptr->dev_ptr->bonded_link_key = FALSE;
        if ( bt_rm_data.sm4_debug_enable != FALSE )
        {
          conn_ptr->dev_ptr->bonded_link_key = TRUE;
        }
        break;

      case BT_HC_LINK_KEY_UNAUTH:
        conn_ptr->dev_ptr->authenticated_link_key = FALSE;
        if ( conn_ptr->dev_ptr->bonded_link_key != TRUE )
        {
          conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_UNBONDED;
        }
        else
        {
          if (( conn_ptr->dedicated_bonding != TRUE ) &&
              ( conn_ptr->remote_auth_req != BT_RM_AR_NO_MITM_DEDICATED_BONDING ) &&
              ( conn_ptr->remote_auth_req != BT_RM_AR_MITM_DEDICATED_BONDING ))
          {
            conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_GENERAL;
          }
          else
          {
            conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_DEDICATED;
          }
        }
        break;

      case BT_HC_LINK_KEY_AUTH:
        conn_ptr->dev_ptr->authenticated_link_key = TRUE;
        if ( conn_ptr->dev_ptr->bonded_link_key != TRUE )
        {
          conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_AUTH_UNBONDED;
        }
        else
        {
          if (( conn_ptr->dedicated_bonding != TRUE ) &&
              ( conn_ptr->remote_auth_req != BT_RM_AR_NO_MITM_DEDICATED_BONDING ) &&
              ( conn_ptr->remote_auth_req != BT_RM_AR_MITM_DEDICATED_BONDING ))
          {
            conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_AUTH_GENERAL;
          }
          else
          {
            conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_AUTH_DEDICATED;
          }
        }
        break;

      case BT_HC_LINK_KEY_COMB:
        conn_ptr->dev_ptr->authenticated_link_key = TRUE;
        conn_ptr->dev_ptr->bonded_link_key = TRUE;
        conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_COMBINATION;
        break;

      case BT_HC_LINK_KEY_REMOTE:
      case BT_HC_LINK_KEY_LOCAL:
        conn_ptr->dev_ptr->authenticated_link_key = FALSE;
        conn_ptr->dev_ptr->bonded_link_key = FALSE;
        conn_ptr->dev_ptr->dev_public.link_key_status = BT_RM_LKT_UNIT;
        break;

      default:
        BT_ERR("BT RM: SSP Invalid LKType %x",
               ev_msg_ptr->ev_msg.ev_hc_keynt.key_type_key,0,0);
    }

    if (( conn_ptr->rebond_req_pending == TRUE ) || 
        ( conn_ptr->authorized_rebond == TRUE ))
    {
      BT_MSG_DEBUG( "BT RM: Reinit rebond flags RRP %x AR %x",
                     conn_ptr->rebond_req_pending,
                     conn_ptr->authorized_rebond, 0 );
      conn_ptr->rebond_req_pending = FALSE;
      conn_ptr->authorized_rebond = FALSE;
    }

    BT_MSG_HIGH( "BT RM: SSP Link Key Notif BLK %x AUTHLK %x LKS %x",
                 conn_ptr->dev_ptr->bonded_link_key,
                 conn_ptr->dev_ptr->authenticated_link_key,
                 conn_ptr->dev_ptr->dev_public.link_key_status );

    if (( conn_ptr->sec_level_min == BT_RM_SL_3_HIGH ) &&
        ( conn_ptr->dev_ptr->authenticated_link_key != TRUE ))
    {
      /* Insufficient security for service.                              */
      /* We requested an authenticated link key, but did not receive one.*/
      /* enforce_service_security will handle this later. Just printing  */
      /* a message right now.                                            */
      /* Note that even thought the link key is not the expected kind, it*/
      /* will be saved, since another service may be able to use it.     */
      BT_MSG_HIGH( "BT RM: Insufficient Security. Expected AUTHLK.",0,0,0);
    }
    else if ( conn_ptr->dev_ptr->dev_public.link_key_status == BT_RM_LKT_UNIT )
    {
      /* Unit keys not supported Disconnect */
      BT_MSG_HIGH( "BT RM: Pairing with unit key not allowed.",0,0,0);
      security_failure = TRUE;
      bt_rm_disconnect_acl( conn_ptr, 
                            ( bt_rm_disc_reason_type )BT_BE_PAIRING_WITH_UNIT_KEY_NOT_ALLOWED);
    }

    if ( security_failure != TRUE )
#endif /* BT_SWDEV_2_1_SSP */
    {
      bt_rm_update_device_link_key_info(
        conn_ptr, &ev_msg_ptr->ev_msg.ev_hc_keynt );

#ifdef BT_SWDEV_2_1_SSP
      if ( ( ( conn_ptr->pairing_initiated != TRUE ) &&
             ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON ) ) ||
             ( ( conn_ptr->pairing_initiated != TRUE ) &&
             ( conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE ) ) )
      {
        /* update security level on accepting side since no AuthC
           or EncCh events will be received.
         */
        bt_rm_security_updated( conn_ptr );
      }
#endif /* BT_SWDEV_2_1_SSP */

      if ( conn_ptr->bonding_app_id != BT_APP_ID_NULL )
      {
        conn_ptr->bonding_key_rxd = TRUE;

        bt_rm_reset_idle_timeout( conn_ptr );
      }
      else
      {
        bt_rm_ev_send_bonded( conn_ptr );

#ifdef BT_SWDEV_2_1_SSP
        conn_ptr->dedicated_bonding = FALSE;

        conn_ptr->bonding_app_id  = BT_APP_ID_NULL;
        memset( (void*)(&conn_ptr->bonding_pin), 0,
            sizeof( bt_pin_code_type ) );
#endif
      }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad LK Notif St %x", status, 0, 0 );
    BT_BDA( ERR, "BT RM: Bad LK Notif",
            &ev_msg_ptr->ev_msg.ev_hc_keynt.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_mode_change

DESCRIPTION
  Processes a HC Mode Change event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_mode_change
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*    conn_ptr = NULL;
  bt_error_code_type  hc_status;
  uint16              hc_handle;
  bt_link_mode_type   cur_link_mode;
  uint16              cur_interval;

  hc_status     = (bt_error_code_type)
                     (ev_msg_ptr->ev_msg.ev_hc_modec.status);
  hc_handle     = ev_msg_ptr->ev_msg.ev_hc_modec.conn_hndl;
  cur_link_mode = ev_msg_ptr->ev_msg.ev_hc_modec.current_mode;
  cur_interval  = ev_msg_ptr->ev_msg.ev_hc_modec.interval;

  BT_MSG_API( "BT RM EV RX: HC ModeCh 0x%x H|S 0x%04x I 0x%x",
              cur_link_mode,
              ( (hc_handle << 8) | (uint16)hc_status ),
              cur_interval );

  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->link_mode_request != BT_LM_NONE )
    {
      bt_rm_update_conn_hc_error( conn_ptr, hc_status );
    }

    if ( (conn_ptr->link_mode != cur_link_mode) ||
         (conn_ptr->cur_interval != cur_interval) )
    {
      conn_ptr->link_mode     = cur_link_mode;
      conn_ptr->cur_interval  = cur_interval;
      conn_ptr->updates      |= BT_RM_LU_LINK_MODE_B;
    }

    bt_rm_link_status_updated( conn_ptr );

    /* BT_TBD */
    switch ( cur_link_mode )
    {
      case BT_LM_ACTIVE:
        {

#ifdef FEATURE_BT_ENABLE_SLEEP_VOTING
#error code not present
#endif

#ifdef BT_SWDEV_2_1_SSR
          if ( conn_ptr->link_mode_request != BT_LM_SNIFF )
          {
            /* Unsniff successful */
            conn_ptr->ssr_state = BT_RM_SSR_NOT_IN_PROGRESS;

            BT_MSG_API( "BT RM: SSR emu %d Idle TO %dms LPC chg %d",
                        conn_ptr->ssr_emulating,
                        conn_ptr->idle_timeout_ms_left,
                        conn_ptr->sniff_param_changed );

            /* If unsniff was initiated by the local device, check:
            **  - if SSR emulation is ongoing. OR
            **  - if we shall try our own sniff param since we
            **    just rejected sniff param proposed by the remote. OR
            **  - if lpm_param have been changed and not compatible
            **    with current ones.
            */
            if ( (conn_ptr->link_mode_request != BT_LM_NONE) &&
                 ((conn_ptr->ssr_emulating == TRUE) ||
                  (conn_ptr->idle_timeout_ms_left != 0) ||
                  (conn_ptr->sniff_param_changed == TRUE)) )
            {
              /* if max_latency changed and local controller supports SSR,
              ** RM will update the controller of the new max_latency.
              */
              if ( (conn_ptr->cur_latency != conn_ptr->max_latency) &&
                   IS_SSR_DOABLE() )
              {
                BT_MSG_API( "BT RM CMD TX: HC SSR H %x L %x",
                            conn_ptr->hc_handle_acl,
                            conn_ptr->max_latency, 0 );

                bt_cmd_hc_sniff_subrating( conn_ptr->hc_handle_acl,
                                           conn_ptr->max_latency,
                                           BT_RM_DEF_REMOTE_TIMEOUT,
                                           BT_RM_DEF_LOCAL_TIMEOUT );
              }

              conn_ptr->link_mode_request    = BT_LM_SNIFF;
              conn_ptr->sniff_param_changed  = FALSE;
              conn_ptr->idle_timeout_ms_left = 0;
            }
            else
            {
              if (conn_ptr->ssr_emulating == TRUE)
              {
                BT_MSG_API( "BT RM: SSR emulation terminated", 0, 0, 0 );

                conn_ptr->ssr_emulating = FALSE;
              }

              if ( conn_ptr->idle_link_mode != BT_LM_ACTIVE )
              {
                bt_rm_reset_idle_timeout( conn_ptr );
              }
              conn_ptr->link_mode_request = BT_LM_NONE;
            }
          }
          else
          {
            /* Sniff failed */
            conn_ptr->link_mode_request = BT_LM_NONE;

            if ( conn_ptr->ssr_emulating == TRUE )
            {
              if ( conn_ptr->sniff_retry_to_ms_left == 0 )
              {
                /* SSR emulation failed, try regular Sniff */
                conn_ptr->ssr_emulating = FALSE;
              }
              else
              {
                /* time to retry the failed initiation of
                ** SSR emu just now.
                */
                conn_ptr->sniff_retry_to_ms_left = 0;
              }

              conn_ptr->link_mode_request   = BT_LM_SNIFF;
              conn_ptr->sniff_param_changed = FALSE;
            }
            else if ( conn_ptr->sniff_param_changed == TRUE )
            {
              /* sniff param changed, let's try the new one */
              conn_ptr->link_mode_request   = BT_LM_SNIFF;
              conn_ptr->sniff_param_changed = FALSE;
            }
          }

          if ( conn_ptr->link_mode_request == BT_LM_SNIFF )
          {
            cur_interval = conn_ptr->ssr_emulating ?
                             conn_ptr->max_latency : conn_ptr->max_interval;

            BT_MSG_API( "BT RM CMD TX: HC Sniff H %x I %x",
                        conn_ptr->hc_handle_acl, cur_interval, 0 );

            bt_cmd_hc_sniff_mode(
              conn_ptr->hc_handle_acl,
              BT_RM_DEF_MIN_INTERVAL,
              cur_interval,
              BT_RM_DEF_SNIFF_ATTEMPT,
              BT_RM_DEF_SNIFF_TIMEOUT );
          }
          else if ( (conn_ptr->idle_link_mode != BT_LM_ACTIVE) &&
                    (conn_ptr->idle_timeout_ms_left == 0) )
          {
            /* No idle_timeout ticking, kick off a timeout to retry Sniff */
            conn_ptr->sniff_retry_to_ms_left = BT_RM_DEF_SNIFF_RETRY_TO_MS;
          }
#else
          if ( (conn_ptr->link_mode_request != BT_LM_SNIFF) &&
               (conn_ptr->idle_link_mode != BT_LM_ACTIVE) )
          {
            bt_rm_reset_idle_timeout( conn_ptr );
          }
          conn_ptr->link_mode_request = BT_LM_NONE;
#endif /* BT_SWDEV_2_1_SSR */
#ifndef FEATURE_BT_SOC
          /* update connection packet types */
          bt_rm_check_conn_packet_types_acl(conn_ptr);
#else
          bt_rm_hc_get_rssi_if_reqd(conn_ptr);
          /* restart rssi update timer */
          conn_ptr->rssi_update_ms_left = BT_RM_RSSI_UPD_TIMEOUT_MS;
#endif /* !FEATURE_BT_SOC */
          break;
        }

      case BT_LM_SNIFF:
        {
#ifdef FEATURE_BT_SOC
          /* clear rssi update timer */
          conn_ptr->rssi_update_ms_left = 0;
#endif /* FEATURE_BT_SOC */
          if ( conn_ptr->link_mode_request == BT_LM_ACTIVE )
          {
            /*  Unsniff unsuccessful, try again.  */
            BT_MSG_API( "BT RM CMD TX: HC Unsniff H %x",
                        conn_ptr->hc_handle_acl, 0, 0 );

            bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );
          }
          else
          {
            conn_ptr->link_mode_request = BT_LM_NONE;

            /* Sniff successful */

            if ( conn_ptr->idle_link_mode == BT_LM_ACTIVE )
            {
              /* idle_link_mode was changed to ACTIVE when
              ** sniff was in negotiation.  Need to unsniff.
              */
              conn_ptr->link_mode_request = BT_LM_ACTIVE;
#ifdef BT_SWDEV_2_1_SSR
            }
            else if ( conn_ptr->ssr_emulating != TRUE )
            {
              if ( conn_ptr->sniff_param_changed == TRUE )
              {
                if ( conn_ptr->cur_interval != conn_ptr->max_interval )
                {
                  /* sniff param changed, let's try the new one */
                  conn_ptr->link_mode_request = BT_LM_ACTIVE;
                }
                else
                {
                  conn_ptr->sniff_param_changed = FALSE;
                }
              }
              else if ( (conn_ptr->cur_interval > conn_ptr->max_interval) &&
                        (conn_ptr->stick_to_app_param == TRUE) )
              {
                /* Apps don't like the cur_interval */
                conn_ptr->link_mode_request = BT_LM_ACTIVE;
              }
              else if ( conn_ptr->cur_interval < conn_ptr->max_latency )
              {
                /*
                In case HID, devices mostly do SSR emulation, so hid has option to disable SSR
                emulation by setting this flag.
                Disable SSR emulation if sniff_initiator_oriented flag is set.
               */
                if ( !IS_SSR_DOABLE() &&
                     conn_ptr->sniff_initiator_oriented != TRUE )
                {
                  /* start timer for SSR emulation */
                  BT_MSG_API( "BT RM: Launch SSR emulation", 0, 0, 0 );

                  conn_ptr->ssr_emulating         = TRUE;
                  conn_ptr->sniff_mode_to_ms_left =
                    BT_RM_DEF_EMU_SNIFF_MODE_TO_MS;
                }
                else
                {
                  if ( conn_ptr->sniff_initiator_oriented != TRUE )
                  {
                    /* kick off a timer to initiate SSR if the remote
                    ** doesn't do it, when there's no HID connection.
                    */
                    BT_MSG_API( "BT RM: SSR waiting TO started", 0, 0, 0 );

                    bt_rm_reset_idle_timeout( conn_ptr );
                  }
                }
              }
            }
            else /* conn_ptr->ssr_emulating == TRUE */
            {
              conn_ptr->ssr_emulating = FALSE;

              if ( (conn_ptr->sniff_param_changed == TRUE) ||
                   (conn_ptr->sniff_retry_to_ms_left != 0) )
              {
                if ( conn_ptr->cur_interval != conn_ptr->max_latency )
                {
                  conn_ptr->link_mode_request = BT_LM_ACTIVE;

                  if ( conn_ptr->sniff_retry_to_ms_left != 0 )
                  {
                    /* SSR emu failed just now, set the flag to retry */
                    conn_ptr->ssr_emulating = TRUE;
                  }
                }
                else
                {
                  conn_ptr->sniff_param_changed = FALSE;
                }

                conn_ptr->sniff_retry_to_ms_left = 0;
              }
              else if ( (conn_ptr->cur_interval <= conn_ptr->max_latency) ||
                        (conn_ptr->stick_to_app_param != TRUE) )
              {
                /* SSR emulation successful */
                conn_ptr->ssr_state = BT_RM_SSR_ENABLED;
              }
              else
              {
                conn_ptr->link_mode_request = BT_LM_ACTIVE;
              }
            }

            if ( conn_ptr->link_mode_request != BT_LM_ACTIVE )
            {
              /* clear timeouts in case it's due to an
              ** incoming Sniff request.
              */
              conn_ptr->idle_timeout_ms_left   = 0;
              conn_ptr->sniff_retry_to_ms_left = 0;
            }
            else
            {
#endif /* BT_SWDEV_2_1_SSR */
              BT_MSG_API( "BT RM CMD TX: HC Unsniff H %x",
                          conn_ptr->hc_handle_acl, 0, 0 );

              bt_cmd_hc_exit_sniff_mode( conn_ptr->hc_handle_acl );
            }
          }
#ifdef FEATURE_BT_ENABLE_SLEEP_VOTING
#error code not present
#endif
          break;
        }

      case BT_LM_PARK:
      case BT_LM_HOLD:
      default:
        {
          conn_ptr->link_mode_request    = BT_LM_NONE;
          conn_ptr->idle_timeout_ms_left = 0;
          break;
        }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad ModeCh H %x M %x I %x",
            hc_handle, cur_link_mode, cur_interval );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_role_change

DESCRIPTION
  Processes a HC Role Change event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_role_change
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;
  bt_role_type        cur_role;

  hc_status = (bt_error_code_type)
                     (ev_msg_ptr->ev_msg.ev_hc_role.status);
  cur_role  = ev_msg_ptr->ev_msg.ev_hc_role.new_role;

  BT_MSG_API( "BT RM EV RX: HC RoleCh %x S %x", cur_role, hc_status, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: HC RoleCh",
          &ev_msg_ptr->ev_msg.ev_hc_role.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_role.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->paging == TRUE )
    { 
      bt_rm_do_bb_page_stopped( conn_ptr );
    }

    if ( ( conn_ptr->role == cur_role ) && ( hc_status == BT_BE_SUCCESS ) )
    {
        hc_status = BT_BE_ROLE_SWITCH_FAILED;
        BT_ERR( "BT RM: Changed HCI Status CR %x OR %x S %x",
                                                cur_role, conn_ptr->role, hc_status );
    }

    if ( ( cur_role == BT_ROLE_MASTER ) && ( hc_status == BT_BE_SUCCESS ) &&
         ( conn_ptr->state_acl  == BT_RM_CS_CONNECTED ) )
    {
        /* Write Default Link supervision timeout if local device is Master */
        bt_cmd_hc_wr_link_supervision_timeout(conn_ptr->hc_handle_acl,
                                  BT_MIN_LINK_SUPERVISION_TIMEOUT);
    }

    bt_rm_finish_role_switch( conn_ptr, cur_role, hc_status );
  }
  else
  {
    BT_ERR( "BT RM: Bad RoleCh St %x R %x", status, cur_role, 0 );
    BT_BDA( ERR, "BT RM: Bad RoleCh",
            &ev_msg_ptr->ev_msg.ev_hc_role.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_num_cmplt_pkts

DESCRIPTION
  Processes a HC Number of Completed Packets event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_num_cmplt_pkts
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  dsm_item_type*  numpk_dsm_ptr;

  BT_MSG_API_PYLD( "BT RM EV RX: HC NumPk %x",
                   ev_msg_ptr->ev_msg.ev_hc_numpk.num_handles, 0, 0 );

  numpk_dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_numpk.dsm_ptr;

  dsm_free_packet( &numpk_dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_remote_name

DESCRIPTION
  Processes a HC Remote Name Request Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_remote_name
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_cmd_status_type  status;
  bt_rm_conn_type*    conn_ptr;
  bt_error_code_type  hc_status;
  dsm_item_type*      rname_dsm_ptr;
  uint16              name_length;
  char                new_name_str[ BT_SD_MAX_DEVICE_NAME_LEN + 1 ];

  hc_status = (bt_error_code_type)
                 (ev_msg_ptr->ev_msg.ev_hc_rname.status);

  rname_dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_rname.dsm_ptr;

  BT_MSG_API( "BT RM EV RX: HC RName St %x", hc_status, 0, 0 );
  BT_BDA( MSG_API, "BT RM EV RX: HC RName",
          &ev_msg_ptr->ev_msg.ev_hc_rname.bd_addr );

  if ( (status = bt_rm_get_conn_bd_addr(
                   TRUE,   /*  Connection must exist.           */
                   FALSE,  /*  Connection need not be settled.  */
                   &ev_msg_ptr->ev_msg.ev_hc_rname.bd_addr,
                   BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->paging == TRUE )
    { 
      bt_rm_do_bb_page_stopped( conn_ptr );
    }

    /* Processing of cmd status with failure could have reset rname_app_id
     * on all conn_ptr's with pending remote name request. So, update the 
     * name to device database first without checking for rname_app_id being
     * non-NULL (we have already identified the conn_ptr at this point ).  
    */
    if ( hc_status == BT_BE_SUCCESS )
    {
      status = BT_CS_GN_SUCCESS;

      /*  Copy the name up to maximum length.  */

      name_length = dsm_length_packet( rname_dsm_ptr );
      name_length = MIN( name_length, BT_SD_MAX_DEVICE_NAME_LEN );

      dsm_pullup( &rname_dsm_ptr, (void*)(new_name_str), name_length );

      new_name_str[ name_length ] = 0;
      conn_ptr->remote_name_available = TRUE;
      bt_rm_update_device_name(
        conn_ptr->rname_app_id, conn_ptr->dev_ptr, new_name_str );
#ifdef BT_SWDEV_2_1_EIR
      SETBIT( conn_ptr->dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_NAME_CMPLT_B );
#endif /* BT_SWDEV_2_1_EIR */
    }
    else
    {
      status = BT_CS_RM_NAME_REQ_FAILED;
    }

    if ( conn_ptr->rname_app_id != BT_APP_ID_NULL )
    {

      /* Notify upper layers and proceed with pending action if any */
      bt_rm_remote_name_complete_processing( conn_ptr, status );

    }
    else
    {
      BT_MSG_HIGH( "BT RM: Unexp RName RNAID %x St %x",
                    conn_ptr->rname_app_id, hc_status, 0 );
      BT_BDA( MSG_HIGH, "BT RM: Unexp RName",
              &ev_msg_ptr->ev_msg.ev_hc_rname.bd_addr );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad RName St %x HSt %x", status, hc_status, 0 );
    BT_BDA( ERR, "BT RM: Unexp RName",
            &ev_msg_ptr->ev_msg.ev_hc_rname.bd_addr );
  }

  dsm_free_packet( &rname_dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_remote_features

DESCRIPTION
  Processes a HC Read Remote Feature Request Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_remote_features
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

    bt_rm_conn_type*    conn_ptr;
    bt_rm_device_type*  dev_ptr;
    bt_error_code_type  hc_status;
    uint16              hc_handle;
    uint8               i;

    hc_status = (bt_error_code_type)
                   (ev_msg_ptr->ev_msg.ev_hc_rfeat.status);
    hc_handle = ev_msg_ptr->ev_msg.ev_hc_rfeat.conn_hndl;

    BT_MSG_API( "BT RM EV RX: HC Rd Remote Feature complete S %x H %x",
                hc_status, hc_handle, 0 );

    if ( hc_status != BT_BE_SUCCESS )
    {
      BT_ERR( "BT RM: Error Read Remote Feature complete H %x S %x",
              hc_handle, hc_status, 0 );
#ifndef BT_SWDEV_2_1_SSP
      if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
      {
        conn_ptr->rd_remote_req_pending = FALSE;
      }
      else
      {
        BT_ERR( "BT RM: Bad Read Remote Feature complete H %x S %x",
                hc_handle, hc_status, 0 );
      }
#endif /* BT_SWDEV_2_1_SSP */
    }
    else if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
    {

#ifndef BT_SWDEV_2_1_SSP
        conn_ptr->rd_remote_req_pending = FALSE;
#endif /* BT_SWDEV_2_1_SSP */
        conn_ptr->remote_lmp_features =
                                ev_msg_ptr->ev_msg.ev_hc_rfeat.lmp_features;
        dev_ptr = conn_ptr->dev_ptr;
        for ( i = 0; i < sizeof( bt_lmp_features_type ); i++ )
        {
          if ( dev_ptr->dev_public.lmp_features.lmp_features_bytes[ i ] !=
               ev_msg_ptr->ev_msg.ev_hc_rfeat.lmp_features.lmp_features_bytes[ i ] )
          {
            dev_ptr->dev_public.lmp_features =
                            ev_msg_ptr->ev_msg.ev_hc_rfeat.lmp_features;
            dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
            break;
          }
        }

#ifdef BT_SWDEV_2_1_SSP
        if ( BT_LMP_FEATURE_SUPPORTED( BT_RM_LMP_FEATURE_BIT_SSP,
                                       &conn_ptr->dev_ptr->dev_public.lmp_features) )
        {
          BT_MSG_DEBUG("BT RM: SSP supported by remote controller!",0,0,0);
          if ( conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE )
          {
            /* will be set based on host features during rname_req or
               if EIR was received
            */
            BT_MSG_DEBUG("BT RM: WARNING - ssp_capable was FALSE. This is expected on the server side",0,0,0);
            BT_MSG_DEBUG("BT RM: WARNING - ssp_capable was FALSE. HostSuppFeatures and/or EIR  not rxd",0,0,0);
          }
          conn_ptr->dev_ptr->dev_public.ssp_capable = TRUE;
          // Read extended features only if the remote supports 2.1
          bt_cmd_hc_rd_remote_feature_ext( conn_ptr->hc_handle_acl, BT_RM_LMP_EXT_FEATURES_PAGE_1 );
        }
        else
        {
          BT_MSG_DEBUG("BT RM: SSP not supported by remote!",0,0,0);
          conn_ptr->dev_ptr->dev_public.ssp_capable = FALSE;
          conn_ptr->rd_remote_req_pending = FALSE;
        }
        dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
#endif /* BT_SWDEV_2_1_SSP */

        if ( dev_ptr->dev_public.valid != FALSE )
        {
          bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
        }
        if (conn_ptr->dev_ptr->dev_public.ssp_capable != TRUE)
        {
          bt_rm_conn_complete_processing(conn_ptr);
        }
    }
    else
    {
      BT_ERR( "BT RM: Bad Read Remote Feature complete H %x S %x",
              hc_handle, hc_status, 0 );
    }
}

#ifdef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_remote_features_ext

DESCRIPTION
  Processes a HC Read Remote Feature Request Ext Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_remote_features_ext
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

    bt_rm_conn_type*    conn_ptr;
    bt_rm_device_type*  dev_ptr;
    bt_error_code_type  hc_status;
    uint16              hc_handle;
    uint8               i;
    boolean             ssp_controller;
    boolean             ssp_host;

    hc_status = (bt_error_code_type)
                   (ev_msg_ptr->ev_msg.ev_hc_rextf.status);
    hc_handle = ev_msg_ptr->ev_msg.ev_hc_rextf.conn_hndl;

    BT_MSG_API( "BT RM EV RX: HC Rd Remote Feature Ext complete S %x H %x",
                hc_status, hc_handle, 0 );

    if ( hc_status != BT_BE_SUCCESS )
    {
      BT_ERR( "BT RM: Error Read Remote Feature Ext complete H %x S %x",
              hc_handle, hc_status, 0 );

      if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
      {
        conn_ptr->rd_remote_req_pending = FALSE;
      }
      else
      {
        BT_ERR( "BT RM: Bad Read Remote Feature Ext complete H %x S %x",
                hc_handle, hc_status, 0 );
      }
    }
    else if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
    {

        conn_ptr->rd_remote_req_pending = FALSE;
        conn_ptr->remote_lmp_features_ext =
                                ev_msg_ptr->ev_msg.ev_hc_rextf.ext_lmp_features;
        dev_ptr = conn_ptr->dev_ptr;
        for ( i = 0; i < sizeof( bt_lmp_features_type ); i++ )
        {
          if ( dev_ptr->dev_public.lmp_features_ext.lmp_features_bytes[ i ] !=
               ev_msg_ptr->ev_msg.ev_hc_rextf.ext_lmp_features.lmp_features_bytes[ i ] )
          {
            dev_ptr->dev_public.lmp_features_ext =
                            ev_msg_ptr->ev_msg.ev_hc_rextf.ext_lmp_features;
            dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
            break;
          }
        }

        ssp_controller = BT_LMP_FEATURE_SUPPORTED( BT_RM_LMP_FEATURE_BIT_SSP,
                                       &conn_ptr->dev_ptr->dev_public.lmp_features);
        ssp_host = BT_LMP_FEATURE_SUPPORTED( BT_RM_LMP_FEATURE_BIT_SSP_HOST,
                                       &conn_ptr->dev_ptr->dev_public.lmp_features_ext );
        if (  ssp_controller && ssp_host )
        {
          BT_MSG_DEBUG("BT RM: SSP supported by remote host and controller",0,0,0);
          conn_ptr->dev_ptr->dev_public.ssp_capable = TRUE;
        }
        else
        {
          BT_MSG_DEBUG("BT RM: SSP not supported by remote",0,0,0);
          BT_MSG_DEBUG("BT RM: WARNING - ssp_capable was TRUE. Controller %x, Host %x.",
                       ssp_controller,ssp_host,0);

          conn_ptr->dev_ptr->dev_public.ssp_capable = FALSE;
        }
        dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;

        if ( dev_ptr->dev_public.valid != FALSE )
        {
          bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
        }
        bt_rm_conn_complete_processing(conn_ptr);
    }
    else
    {
      BT_ERR( "BT RM: Bad Read Remote Feature Ext complete H %x S %x",
              hc_handle, hc_status, 0 );
    }
}
#endif /* BT_SWDEV_2_1_SSP */

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_remote_version

DESCRIPTION
  Processes a HC Read Remote Version Complete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_remote_version
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

    bt_rm_conn_type*    conn_ptr;
    bt_rm_device_type*  dev_ptr;
    bt_error_code_type  hc_status;
    uint16              hc_handle;

    hc_status = (bt_error_code_type)
                   (ev_msg_ptr->ev_msg.ev_hc_rvers.status);
    hc_handle = ev_msg_ptr->ev_msg.ev_hc_rvers.conn_hndl;

    BT_MSG_API( "BT RM EV RX: HC Rd Remote version complete S %x H %x",
                hc_status, hc_handle, 0 );

    if ( hc_status != BT_BE_SUCCESS )
    {
        BT_ERR( "BT RM: Error Read Remote Version complete H %x S %x",
                hc_handle, hc_status, 0 );
    }
    else if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
    {

        dev_ptr = conn_ptr->dev_ptr;

        if ( dev_ptr->dev_public.version.lmp_version !=
               ev_msg_ptr->ev_msg.ev_hc_rvers.lmp_version )
        {
          dev_ptr->dev_public.version.lmp_version =
            ev_msg_ptr->ev_msg.ev_hc_rvers.lmp_version;
          dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
        }

        if ( dev_ptr->dev_public.version.lmp_subversion !=
               ev_msg_ptr->ev_msg.ev_hc_rvers.lmp_subversion )
        {
          dev_ptr->dev_public.version.lmp_subversion =
            ev_msg_ptr->ev_msg.ev_hc_rvers.lmp_subversion;
          dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
        }

        if ( dev_ptr->dev_public.version.manufacturer_name !=
               ev_msg_ptr->ev_msg.ev_hc_rvers.manufacturer_name )
        {
          dev_ptr->dev_public.version.manufacturer_name =
              ev_msg_ptr->ev_msg.ev_hc_rvers.manufacturer_name;
          dev_ptr->update_status |= BT_RM_DUS_LMP_INFO_B;
        }

        if ( (dev_ptr->dev_public.valid != FALSE) &&
             (dev_ptr->update_status != BT_RM_DUS_NONE) )
         {
           bt_rm_device_updated( BT_APP_ID_NULL, dev_ptr );
         }
    }
    else
    {
      BT_ERR( "BT RM: Bad Read Remote Version complete H %x S %x",
              hc_handle, hc_status, 0 );
    }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_clk_offset

DESCRIPTION
  Processes a HC Read Clk Offset compplete event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_clk_offset
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

    bt_rm_conn_type*    conn_ptr;
    bt_rm_device_type*  dev_ptr;
    bt_error_code_type  hc_status;
    uint16              hc_handle;

    hc_status = (bt_error_code_type)
                   (ev_msg_ptr->ev_msg.ev_hc_clock.status);
    hc_handle = ev_msg_ptr->ev_msg.ev_hc_clock.conn_hndl;

    BT_MSG_API( "BT RM EV RX: HC Rd Clk Offset complete S %x H %x",
                hc_status, hc_handle, 0 );

    if ( hc_status != BT_BE_SUCCESS )
    {
        BT_ERR( "BT RM: Error reading Clk Offset H %x S %x",
                hc_handle, hc_status, 0 );
    }
    else if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
    {
        dev_ptr = conn_ptr->dev_ptr;
        /* Just update the ram copy of db. No need to update the efs copy */
        dev_ptr->clk_offset =
           ev_msg_ptr->ev_msg.ev_hc_clock.clk_offset | BT_CLK_OFFSET_VALID_V;
    }
    else
    {
      BT_ERR( "BT RM: Bad Read Clk Offset complete H %x S %x",
              hc_handle, hc_status, 0 );
    }
}

/*===========================================================================

FUNCTION
  bt_rm_ev_hc_return_link_keys

DESCRIPTION
  Processes a HC Return Link Keys event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_return_link_keys
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  dsm_item_type*  retky_dsm_ptr;

  BT_MSG_API_PYLD( "BT RM EV RX: HC Link Keys %x",
                   ev_msg_ptr->ev_msg.ev_hc_retky.num_keys, 0, 0 );

  retky_dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_retky.dsm_ptr;

  dsm_free_packet( &retky_dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_rx_acl_data

DESCRIPTION
  Processes a HC Receive ACL Data event.

===========================================================================*/
void bt_rm_ev_hc_rx_acl_data
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  uint16            hc_handle;
  bt_ev_msg_type    ev_rm_rda;
  dsm_item_type*    dsm_ptr;

  hc_handle = ev_msg_ptr->ev_msg.ev_hc_rxda.conn_hndl;
  dsm_ptr   = ev_msg_ptr->ev_msg.ev_hc_rxda.dsm_ptr;

  BT_MSG_API_PYLD( "BT RM EV RX: HC RX ACLD H %x S %x", hc_handle,
                   ev_msg_ptr->ev_msg.ev_rm_rxda.l2cap_pkt_start, 0 );

  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
#ifdef BT_SWDEV_2_1_SSR
    if ( conn_ptr->link_mode == BT_LM_ACTIVE )
#endif /* BT_SWDEV_2_1_SSR */
    {
      bt_rm_reset_idle_timeout( conn_ptr );
    }

    ev_rm_rda.ev_hdr.bt_app_id = conn_ptr->bt_app_id_acl;
    ev_rm_rda.ev_hdr.ev_type   = BT_EV_RM_RX_DATA_ACL;

    ev_rm_rda.ev_msg.ev_rm_rxda.handle = conn_ptr->rm_handle;

    /* On the RX side, the pkt_bondary_flag will only be set to
     * BT_HC_PKT_BOUNDARY_START_FLUSHABLE for start packets. It will never be set to
     * BT_HC_PKT_BOUNDARY_START for a start packet.
     */
    if ( ev_msg_ptr->ev_msg.ev_hc_rxda.pkt_boundary_flag ==
           BT_HC_PKT_BOUNDARY_START_FLUSHABLE )
    {
      ev_rm_rda.ev_msg.ev_rm_rxda.l2cap_pkt_start = TRUE;
    }
    else
    {
      ev_rm_rda.ev_msg.ev_rm_rxda.l2cap_pkt_start = FALSE;
    }

    if ( ev_msg_ptr->ev_msg.ev_hc_rxda.broadcast_flag ==
           BT_HC_BROADCAST_NONE )
    {
      ev_rm_rda.ev_msg.ev_rm_rxda.broadcast = FALSE;
    }
    else
    {
      ev_rm_rda.ev_msg.ev_rm_rxda.broadcast = TRUE;
    }

    ev_rm_rda.ev_msg.ev_rm_rxda.dsm_ptr = dsm_ptr;

#ifdef BT_TEST_PYLD_FAST_RX
    bt_l2_process_rm_rx_acl_data( &ev_rm_rda );
#else
    bt_ec_send_event( &ev_rm_rda );
#endif

    dsm_ptr = NULL;  /*  Prevent freeing DSM.  */
  }
  else
  {
    BT_ERR( "BT RM: Bad RXDA H %x S %x", hc_handle,
            ev_msg_ptr->ev_msg.ev_rm_rxda.l2cap_pkt_start, 0 );
  }

  dsm_free_packet( &dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_ev_hc_tx_data_cfm

DESCRIPTION
  Processes a Tx Data Cfm Event.

===========================================================================*/
void bt_rm_ev_hc_tx_data_cfm
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  bt_rm_conn_type*  conn_ptr;
  uint16            hc_handle;
  bt_ev_msg_type    ev_rm_tx_data_cfm;

  hc_handle = ev_msg_ptr->ev_msg.ev_hc_tx_cfm.conn_hndl;
  BT_MSG_API_PYLD( "BT RM EV RX: HC TX Data Cfm H 0x%x", hc_handle, 0, 0);
  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
    ev_rm_tx_data_cfm.ev_hdr.bt_app_id = conn_ptr->bt_app_id_acl;
    ev_rm_tx_data_cfm.ev_hdr.ev_type   = BT_EV_RM_TX_DATA_CFM;

    ev_rm_tx_data_cfm.ev_msg.ev_rm_tx_cfm.handle = conn_ptr->rm_handle;
    ev_rm_tx_data_cfm.ev_msg.ev_rm_tx_cfm.local_cid =
                        ev_msg_ptr->ev_msg.ev_hc_tx_cfm.local_cid;
    ev_rm_tx_data_cfm.ev_msg.ev_rm_tx_cfm.up_handle_ptr =
                         ev_msg_ptr->ev_msg.ev_hc_tx_cfm.up_handle_ptr;
    bt_ec_send_event( &ev_rm_tx_data_cfm );
  }
  else
  {
    BT_ERR( "BT RM: NULL conn_ptr Bad TX CFM Handle = 0x%x", hc_handle, 0, 0 );
  }
}
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_rx_sco_data

DESCRIPTION
  Processes a HC Receive SCO Data event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_rx_sco_data
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  uint16            hc_handle;
  dsm_item_type*    dsm_ptr = NULL;

  hc_handle = ev_msg_ptr->ev_msg.ev_hc_rxds.conn_hndl;
  dsm_ptr   = ev_msg_ptr->ev_msg.ev_hc_rxds.dsm_ptr;

  BT_MSG_API_PYLD( "BT RM EV RX: HC RX SCOD H %x", hc_handle, 0, 0 );

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->hc_handle_sco == hc_handle )
    {
#ifdef BT_SWDEV_RM_UNTESTED
      bt_ev_msg_type    ev_rm_rds;
      ev_rm_rds.ev_hdr.bt_app_id = conn_ptr->bt_app_id_sco;
      ev_rm_rds.ev_hdr.ev_type   = BT_EV_RM_RX_DATA_SCO;

      ev_rm_rds.ev_msg.ev_rm_rxds.handle = conn_ptr->rm_handle;

      ev_rm_rds.ev_msg.ev_rm_rxds.dsm_ptr = dsm_ptr;

      bt_ec_send_event( &ev_rm_rds );

      dsm_ptr = NULL;  /*  Prevent freeing DSM.  */
#endif /* BT_SWDEV_RM_UNTESTED */
    }
    else
    {
      BT_ERR( "BT RM: Unexp RXDS HH %x HHS %x",
              hc_handle, conn_ptr->hc_handle_sco, 0 );
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad RXDS H %x", hc_handle, 0, 0 );
  }

  dsm_free_packet( &dsm_ptr );

}

#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_ext_cs_disconnect

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status for an HCI
  Disconnect command.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_ext_cs_disconnect
(
  uint16            hc_handle,
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             hc_status
)
{

  bt_rm_conn_type*  conn_ptr;

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->hc_handle_acl == hc_handle )
    {
      if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTING )
      {
        bt_rm_connection_disconnected(
          conn_ptr, BT_ACL_LINK, (BT_HCI_REASON( hc_status )) );
      }
      else
      {
        BT_ERR( "BT RM: Bad ExtCS Disc St %x",
                conn_ptr->state_acl, 0, 0 );
        BT_BDA( ERR, "BT RM: Bad ExtCS Disc", bd_addr_ptr );
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad ExtCS Disc H %x AH %x SH %x",
              hc_handle, conn_ptr->hc_handle_acl,
              conn_ptr->hc_handle_sco );
    }
  }
  else
  {
    BT_ERR( "BT RM: ExtCS Disc No Conn H %x", hc_handle, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_ext_cs_create_conn

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status for an HCI
  Create Connection command.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_ext_cs_create_conn
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             hc_status
)
{

  bt_rm_conn_type*    conn_ptr;

  if ( bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            bd_addr_ptr, BT_ACL_LINK,
            &conn_ptr ) == BT_CS_GN_SUCCESS )
  {
    if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTING) &&
         (conn_ptr->role == BT_ROLE_MASTER) )
    {
      if ( conn_ptr->bonding_app_id != BT_APP_ID_NULL )
      {
        conn_ptr->state_acl = BT_RM_CS_DISCONNECTING;

        bt_rm_update_conn_hc_error( conn_ptr, (bt_error_code_type)hc_status );

        bt_rm_finish_bond_attempt( conn_ptr );

        bt_rm_init_link( conn_ptr, BT_ACL_LINK );
      }
      else
      {
        bt_rm_connection_failed( conn_ptr, BT_ACL_LINK,
                                 (BT_HCI_REASON( hc_status )) );
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad ExtCS CC St %x R %x",
              conn_ptr->state_acl, conn_ptr->role, 0 );
      BT_BDA( ERR, "BT RM: Bad ExtCS CC",
              bd_addr_ptr );
    }
  }
  else
  {
    BT_BDA( ERR, "BT RM: ExtCS CC No Conn", bd_addr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_ext_cs_rem_name_req

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status for an HCI
  Remote Name Request command.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_ext_cs_rem_name_req
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             hc_status
)
{

  bt_rm_conn_type*    conn_ptr;

  if ( bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            bd_addr_ptr, BT_ACL_LINK,
            &conn_ptr ) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->rname_app_id != BT_APP_ID_NULL )
    {
      bt_rm_finish_remote_name_request(
        conn_ptr, (bt_cmd_status_type)(BT_HCI_REASON( hc_status )) );
    }
    else
    {
      BT_ERR( "BT RM: Bad ExtCS RNR RNAID %x",
              conn_ptr->rname_app_id, 0, 0 );
      BT_BDA( ERR, "BT RM: Bad ExtCS RNR", bd_addr_ptr );
    }
  }
  else
  {
    BT_BDA( ERR, "BT RM: ExtCS RNR No Conn", bd_addr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_ext_cs_add_sco

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status for an HCI
  Add SCO or Setup Sync Connection command.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_ext_cs_add_sco
(
  uint16            hc_handle,
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             hc_status
)
{

  bt_rm_conn_type*  conn_ptr;

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->hc_handle_sco == hc_handle )
    {
      if ( conn_ptr->state_sco == BT_RM_CS_CONNECTING )
      {
        bt_rm_connection_failed( conn_ptr, BT_SCO_LINK,
                                 (BT_HCI_REASON( hc_status )) );
      }
      else
      {
        BT_ERR( "BT RM: Bad ExtCS ASCO St %x",
                conn_ptr->state_sco, 0, 0 );
        BT_BDA( ERR, "BT RM: Bad ExtCS ASCO", bd_addr_ptr );
      }
    }
    else
    {
      BT_ERR( "BT RM: Bad ExtCS ASCO H %x SH %x AH %x",
              hc_handle, conn_ptr->hc_handle_sco,
              conn_ptr->hc_handle_acl );
    }
  }
  else
  {
    BT_ERR( "BT RM: ExtCS ASCO No Conn H %x", hc_handle, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_ext_cs_switch_role

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status for an HCI
  Switch Role command.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_ext_cs_switch_role
(
  bt_bd_addr_type*  bd_addr_ptr,
  uint8             hc_status
)
{

  bt_rm_conn_type*    conn_ptr;

  if ( bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            bd_addr_ptr, BT_ACL_LINK,
            &conn_ptr ) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->role_request != BT_ROLE_NONE )
    {
      bt_rm_finish_role_switch( conn_ptr, conn_ptr->role,
                      (bt_error_code_type)(BT_HCI_REASON( hc_status )) );
    }
    else
    {
      BT_ERR( "BT RM: Bad ExtCS SwRl RR %x",
              conn_ptr->role_request, 0, 0 );
      BT_BDA( ERR, "BT RM: Bad ExtCS SwRl", bd_addr_ptr );
    }
  }
  else
  {
    BT_BDA( ERR, "BT RM: ExtCS SwRl No Conn", bd_addr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_extended_cmd_status

DESCRIPTION
  Processes an HC QUALCOMM Extended Command Status event.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_extended_cmd_status
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_ev_hc_qc_extended_cmd_status_type*  ecs_ptr;
  uint8                                  hc_status;
  uint16                                 hc_opcode;
  bt_bd_addr_type*                       bd_addr_ptr;
  uint16                                 hc_handle;

  ecs_ptr = &ev_msg_ptr->ev_msg.ev_hc_qc.qc_event_params.extcs;

  hc_status   = ecs_ptr->norm_cs.status;
  hc_opcode   = ecs_ptr->norm_cs.cmd_opcode;
  bd_addr_ptr = &ecs_ptr->bd_addr;
  hc_handle   = ecs_ptr->conn_hndl;

  BT_MSG_API( "BT RM EV RX: HC QC ExtCS St %x OC %x H %x",
              hc_status, hc_opcode, hc_handle );
  BT_BDA( MSG_API, "BT RM EV RX: HC QC ExtCS", bd_addr_ptr );

  if ( hc_status != BT_BE_SUCCESS )
  {
    switch ( hc_opcode )
    {
      case BT_HCI_DISCONNECT:
        {
          bt_rm_hc_ev_qc_ext_cs_disconnect(
            hc_handle, bd_addr_ptr, hc_status );
          break;
        }

      case BT_HCI_CREATE_CONNECTION:
        {
          bt_rm_hc_ev_qc_ext_cs_create_conn(
            bd_addr_ptr, hc_status );
          break;
        }

      case BT_HCI_REMOTE_NAME_RQST:
        {
          bt_rm_hc_ev_qc_ext_cs_rem_name_req(
            bd_addr_ptr, hc_status );
          break;
        }

#ifdef FEATURE_BT_1_2
      case BT_HCI_SETUP_SYNC_CONNECTION:
#else
      case BT_HCI_ADD_SCO_CONNECTION:
#endif
        {
          bt_rm_hc_ev_qc_ext_cs_add_sco(
            hc_handle, bd_addr_ptr, hc_status );
          break;
        }

      case BT_HCI_SWITCH_ROLE:
        {
          bt_rm_hc_ev_qc_ext_cs_switch_role(
            bd_addr_ptr, hc_status );
          break;
        }

      default:
        {
          BT_ERR( "BT RM: Unexp HC QC ExtCS OC %x St %x",
                  hc_opcode, hc_status, 0 );
          break;
        }
    }
  }
  else
  {
    BT_ERR( "BT RM: Bad HC QC ExtCS CS %x OC %x H %x",
            hc_status, hc_opcode, hc_handle );
    BT_BDA( ERR, "BT RM: Bad HC QC ExtCS", bd_addr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_rem_auth_pair_failed

DESCRIPTION
  Processes an HC QUALCOMM Remote Authentication or Pairing Failed event.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_rem_auth_pair_failed
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_bd_addr_type*      bd_addr_ptr;
  bt_event_reason_type  reason;
  bt_rm_conn_type*      conn_ptr;

  bd_addr_ptr = &ev_msg_ptr->ev_msg.ev_hc_qc.qc_event_params.rmapf.bd_addr;

  BT_BDA( MSG_API, "BT RM EV RX: HC QC RAPF", bd_addr_ptr );
  BT_MSG_API( "BT RM EV RX: HC QC RAPF EC %x",
              ev_msg_ptr->ev_msg.ev_hc_qc.qc_event_params.rmapf.error,
              0, 0 );

  if ( (conn_ptr = bt_rm_find_conn_bd_addr( bd_addr_ptr )) != NULL )
  {
    reason = (BT_HCI_REASON( ev_msg_ptr->ev_msg.ev_hc_qc.
                               qc_event_params.rmapf.error ));

    bt_rm_pin_req_resp_failed( conn_ptr, reason );

    bt_rm_finish_service_security( conn_ptr );
  }
  else
  {
    BT_BDA( ERR, "BT RM: Bad HC QC PF BDA", bd_addr_ptr );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_radio_activity

DESCRIPTION
  Processes an HC QUALCOMM Radio Activity event.  An RM Radio Activity
  event is sent each applications that is registered for them.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_radio_activity
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  bt_rm_data.current_radio_activity =
    ev_msg_ptr->ev_msg.ev_hc_qc.qc_event_params.ract_chgd.ra;

  BT_MSG_API( "BT RM EV RX: HC QC Radio Activity %x",
              bt_rm_data.current_radio_activity, 0, 0 );

  if ( bt_dc.radio_state == BT_RADIO_DISABLED )
  {
    bt_rm_data.current_radio_activity = BT_RA_RADIO_FORCED_OFF;
  }

  bt_rm_ev_send_radio_activity();

}

#endif /* !FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_rm_hc_ev_qc_cur_conn_encryption

DESCRIPTION
  Processes an HC QUALCOMM Current Connection Encryption event.

===========================================================================*/
LOCAL void bt_rm_hc_ev_qc_cur_conn_encryption
(
  bt_ev_msg_type*  ev_msg_ptr
)
{


  bt_ev_hc_qc_cur_conn_encrypt_type*  cce_ptr;
  uint16                              hc_handle;
  uint8                               encrypt_enable;
  uint8                               cur_enc_key_len_bytes;
  bt_rm_conn_type*                    conn_ptr;

#ifndef FEATURE_BT_SOC
  cce_ptr = &ev_msg_ptr->ev_msg.ev_hc_qc.qc_event_params.ccenc;
#else
  cce_ptr = &ev_msg_ptr->ev_msg.ev_hc_ccenc;
#endif /* !FEATURE_BT_SOC */

  hc_handle             = cce_ptr->conn_hndl;
  encrypt_enable        = cce_ptr->encrypt_enable;
  cur_enc_key_len_bytes = cce_ptr->cur_enc_key_len_bytes;

  BT_MSG_API( "BT RM EV RX: HC QC Cur Conn Enc H %x E %x EKL %x",
              hc_handle, encrypt_enable, cur_enc_key_len_bytes );

  if ( (conn_ptr = bt_rm_get_conn_hc_handle( hc_handle )) != NULL )
  {
    if ( conn_ptr->hc_handle_acl == hc_handle )
    {
#if ( defined ( FEATURE_BT_QSOC ) && defined ( FEATURE_BT_EXTPF_SAP ) )
#error code not present
#endif /* FEATURE_BT_QSOC */
      conn_ptr->enc_key_len_bytes = cur_enc_key_len_bytes;

#ifdef BT_SWDEV_ESS_TEST
      if ( bt_rm_ess_test_klb != 0 )
      {
            conn_ptr->enc_key_len_bytes = bt_rm_ess_test_klb;
      }
#endif /* BT_SWDEV_ESS_TEST */

    }
    else
    {
      BT_ERR( "BT RM: Bad Cur Conn Enc H %x SH %x AH %x",
              hc_handle, conn_ptr->hc_handle_sco,
              conn_ptr->hc_handle_acl );
    }
  }
  else
  {
    BT_ERR( "BT RM: Cur Conn Enc No Conn H %x", hc_handle, 0, 0 );
  }

}

#ifndef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_qualcomm

DESCRIPTION
  Processes an HC QUALCOMM event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_qualcomm
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  BT_MSG_API( "BT RM EV RX: HC QC Type %x AID %x",
              ev_msg_ptr->ev_msg.ev_hc_qc.qc_event,
              ev_msg_ptr->ev_hdr.bt_app_id, 0 );

  switch ( ev_msg_ptr->ev_msg.ev_hc_qc.qc_event )
  {

    case BT_EV_HC_QC_RADIO_ACTIVITY_CHANGED:
      {
        bt_rm_hc_ev_qc_radio_activity( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_QC_CUR_CONN_ENCRYPT:
      {
        bt_rm_hc_ev_qc_cur_conn_encryption( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_QC_REM_AUTH_OR_PAIR_FAILED:
      {
        bt_rm_hc_ev_qc_rem_auth_pair_failed( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_QC_EXTENDED_CMD_STATUS:
      {
        bt_rm_hc_ev_qc_extended_cmd_status( ev_msg_ptr );
        break;
      }
    default:
      {
        BT_MSG_DEBUG( "BT RM: HC EV QC %x Ignored AID %x",
                      ev_msg_ptr->ev_msg.ev_hc_qc.qc_event,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }

  } /* switch */
}

#else
/*===========================================================================

FUNCTION
  bt_rm_ev_hc_vendor_specific

DESCRIPTION
  Processes an HC QUALCOMM event.

===========================================================================*/
LOCAL void bt_rm_ev_hc_vendor_specific
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  BT_MSG_API( "BT RM EV RX: HC VS Type AID %x",
              ev_msg_ptr->ev_hdr.bt_app_id, 0, 0 );

  /* Call SoC interface handler to handle Vendor Specific events */
  if( bt_soc_ifc.bt_soc_proc_vs_event_func_ptr != NULL )
  {
    bt_soc_ifc.bt_soc_proc_vs_event_func_ptr(ev_msg_ptr);
  }
  else
  {
    BT_MSG_DEBUG( "Vendor specific event handler NULL", 0,0,0 );
  }
}
#endif /* !FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_rm_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by the RM layer.

===========================================================================*/
LOCAL uint16 bt_rm_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_auth_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_chng_conn_key_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_cmd_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_cmd_status_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_conn_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_conn_pkt_type_chng_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_conn_request_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_data_overflow_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_disc_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_encrypt_change_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_flush_occurred_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_hw_error_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_inquiry_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_inquiry_result_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_key_notif_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_key_request_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_loopback_cmd_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_master_key_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_max_slots_change_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_mode_change_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_num_cmplt_pkts_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_pin_request_type ) );
#ifndef FEATURE_BT_1_2 /* BT 1.1 */
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_ps_mode_chng_type ) );
#endif
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_ps_rep_mode_chng_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_qos_setup_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_qos_violation_type ) );
#if !defined ( FEATURE_BT_SOC )
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_qualcomm_type ) );
#elif defined ( FEATURE_BT_SOC1 )
#error code not present
#elif defined ( FEATURE_BT_QSOC )
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_vs_var_type ) );
#endif
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_rd_clk_offset_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_rd_remote_feat_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_rd_remote_ver_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_remote_name_req_cmplt_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_ret_key_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_role_change_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_rx_acl_data_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_hc_rx_sco_data_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_service_search_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_timeout_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_error_resp_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}


/*===========================================================================

FUNCTION
  bt_rm_process_event

DESCRIPTION
  Handles all BT driver events coming to the RM layer.

===========================================================================*/
LOCAL void bt_rm_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
               BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT RM: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );
        }
        break;
      }

    case BT_EV_SD_SERVICE_SEARCH_RESP:
      {
        bt_rm_ev_sd_service_search_response( ev_msg_ptr );
        break;
      }

    case BT_EV_SD_TIMEOUT_RESP:
      {
        bt_rm_ev_sd_timeout_response(
          &ev_msg_ptr->ev_msg.ev_sd_timeout_resp );
        break;
      }

    case BT_EV_SD_ERROR_RESP:
      {
        bt_rm_ev_sd_error_response(
          &ev_msg_ptr->ev_msg.ev_sd_error_resp );
        break;
      }

    case BT_EV_HC_AUTH_COMPLETE:
      {
        bt_rm_ev_hc_auth_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_COMMAND_COMPLETE:
      {
        bt_rm_ev_hc_command_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_COMMAND_STATUS:
      {
        bt_rm_ev_hc_command_status( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_CONNECTION_COMPLETE:
      {
        bt_rm_ev_hc_connection_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_MAX_SLOTS_CHANGE:
      {
        bt_rm_ev_hc_max_slots_changed( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_CONN_PKT_TYPE_CHANGED:
      {
        bt_rm_ev_hc_pkt_type_changed( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_CONNECTION_REQUEST:
      {
        bt_rm_ev_hc_connection_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_DISCONNECTION_COMPLETE:
      {
        bt_rm_ev_hc_disconnection_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_ENCRYPTION_CHANGE:
      {
        bt_rm_ev_hc_encryption_change( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_INQUIRY_COMPLETE:
      {
        bt_rm_ev_hc_inquiry_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_INQUIRY_RESULT:
      {
        BT_MSG_DEBUG( "BT RM: HC EV Inquiry Result %x",
                      ev_msg_ptr->ev_hdr.ev_type,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        bt_rm_ev_hc_inquiry_result( ev_msg_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_EIR
    case BT_EV_HC_INQUIRY_RESULT_WITH_RSSI:
      {
        BT_MSG_DEBUG( "BT RM: HC EV Inquiry Result RSSI %x",
                      ev_msg_ptr->ev_hdr.ev_type,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        bt_rm_ev_hc_inquiry_result_rssi( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_INQUIRY_RESULT_EXTENDED:
      {
        BT_MSG_DEBUG( "BT RM: HC EV Inquiry Result Extended %x",
                      ev_msg_ptr->ev_hdr.ev_type,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        bt_rm_ev_hc_inquiry_result_extended( ev_msg_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_EIR */

    case BT_EV_HC_LINK_KEY_REQUEST:
      {
        bt_rm_ev_hc_link_key_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_PIN_CODE_REQUEST:
      {
        bt_rm_ev_hc_pin_code_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_LINK_KEY_NOTIFICATION:
      {
        bt_rm_ev_hc_link_key_notification( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_MODE_CHANGE:
      {
        bt_rm_ev_hc_mode_change( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_ROLE_CHANGE:
      {
        bt_rm_ev_hc_role_change( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_NUM_CMPLT_PKTS:
      {
        bt_rm_ev_hc_num_cmplt_pkts( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_REMOTE_NAME_REQ_COMPLETE:
      {
        bt_rm_ev_hc_remote_name( ev_msg_ptr );
        break;
      }

   case BT_EV_HC_RD_REMOTE_FEATURES_COMPLETE:
      {
          bt_rm_ev_hc_remote_features( ev_msg_ptr );
          break;
      }

    case BT_EV_HC_RETURN_LINK_KEYS:
      {
        bt_rm_ev_hc_return_link_keys( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_RX_ACL_DATA:
      {
        bt_rm_ev_hc_rx_acl_data( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_TX_DATA_CFM:
      {
        bt_rm_ev_hc_tx_data_cfm( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_RX_SCO_DATA:
      {
        bt_rm_ev_hc_rx_sco_data( ev_msg_ptr );
        break;
      }

#ifndef FEATURE_BT_SOC
    case BT_EV_HC_QUALCOMM:
      {
        bt_rm_ev_hc_qualcomm( ev_msg_ptr );
        break;
      }
#else
    case BT_EV_HC_VENDOR_SPECIFIC:
      {
        bt_rm_ev_hc_vendor_specific( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_CUR_CONN_ENCRYPT:
      {
        bt_rm_hc_ev_qc_cur_conn_encryption( ev_msg_ptr );
        break;
      }
#endif /* !FEATURE_BT_SOC */

#ifdef FEATURE_BT_1_2
    case BT_EV_HC_SYNC_CONN_COMPLETE:
      {
        bt_rm_ev_hc_sync_conn_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_SYNC_CONN_CHANGE:
      {
        bt_rm_ev_hc_sync_conn_change( ev_msg_ptr );
        break;
      }
#endif /* FEATURE_BT_1_2 */

#ifdef BT_SWDEV_2_1_SSR
    case BT_EV_HC_SNIFF_SUBRATING:
      {
        bt_rm_ev_hc_sniff_subrating( ev_msg_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSR */

#ifdef BT_SWDEV_2_1_SSP
    case BT_EV_HC_IO_CAPABILITY_REQUEST:
      {
        bt_rm_ev_hc_io_cap_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_IO_CAPABILITY_RESPONSE:
      {
        bt_rm_ev_hc_io_cap_response( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_USER_CONFIRMATION_REQUEST:
      {
        bt_rm_ev_hc_user_conf_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_SIMPLE_PAIRING_COMPLETE:
      {
        bt_rm_ev_hc_sp_complete( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_REMOTE_HOST_SUPPORTED_FEAT_NTFCTN:
      {
        bt_rm_ev_hc_remote_host_supp_features( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_ENCRYPTION_KEY_REFRESH_COMPLETE:
      {
        bt_rm_ev_hc_encryption_key_refresh( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_USER_PASSKEY_REQUEST:
      {
        bt_rm_ev_hc_user_pk_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_REMOTE_OOB_DATA_REQUEST:
      {
        bt_rm_ev_hc_oob_data_request( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_USER_PASSKEY_NOTIFICATION:
      {
        bt_rm_ev_hc_user_pk_notification( ev_msg_ptr );
        break;
      }

    case BT_EV_HC_KEYPRESS_NOTIFICATION:
      {
         bt_rm_ev_hc_kp_notification( ev_msg_ptr );
         break;
      }

   case BT_EV_HC_RD_REMOTE_EXT_FEATURES_COMPLETE:
      {
          bt_rm_ev_hc_remote_features_ext( ev_msg_ptr );
          break;
      }

#endif /* BT_SWDEV_2_1_SSP */

#ifdef FEATURE_BT_2_1
    case BT_EV_HC_LINK_SUPERVISION_TIMEOUT_CHANGED:
#endif /* FEATURE_BT_2_1 */
    case BT_EV_HC_CHANGE_CONN_KEY_COMPLETE:
    case BT_EV_HC_DATA_BUFFER_OVERFLOW:
    case BT_EV_HC_FLUSH_OCCURRED:
    case BT_EV_HC_LOOPBACK_COMMAND:
    case BT_EV_HC_MASTER_LINK_KEY_COMPLETE:
    case BT_EV_HC_PAGE_SCAN_MODE_CHANGE:
    case BT_EV_HC_PAGE_SCAN_REP_MODE_CHANGE:
    case BT_EV_HC_QOS_SETUP_COMPLETE:
    case BT_EV_HC_QOS_VIOLATION:
#ifdef BT_SWDEV_2_1_PB
    case BT_EV_HC_ENH_FLUSH_COMPLETE:
#endif /* BT_SWDEV_2_1_PB */
      {
        BT_MSG_DEBUG( "BT RM: HC EV %x Ignored AID %x",
                      ev_msg_ptr->ev_hdr.ev_type,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }
    case BT_EV_HC_RD_REMOTE_VERSION_COMPLETE:
      {
          bt_rm_ev_hc_remote_version ( ev_msg_ptr );
      }
      break;

    case BT_EV_HC_RD_CLK_OFFSET_COMPLETE:
      {
          bt_rm_ev_hc_clk_offset ( ev_msg_ptr );
      }
      break;

    case BT_EV_HC_HARDWARE_ERROR:
      {
#ifdef FEATURE_BT_SOC1
#error code not present
#endif /* FEATURE_BT_SOC1 */

#ifndef FEATURE_BT_SOC1_BC4_DEBUG
        {
#ifdef FEATURE_BT_SOC1_FLASH
#error code not present
#endif /* FEATURE_BT_SOC1_FLASH */
          bt_cmd_msg_type      radio_on_off_cmd;

         /* For all SOC targets, we handle HW error event by turning off the BT
          * Radio and turning it on again. At this point the SOC might have
          * become unresponsive. This procedure  results in the SOC being powered
          * off-on and all other required BT Re-initilization to take place
          * in order to get back into a good state */

          BT_ERR( "BT RM: HCI HW err. HWC %x, Doing Radio on/off to recover.",
                  ev_msg_ptr->ev_msg.ev_hc_hwerr.hardware_code, 0, 0 );

          /* The Radio-off procedure due to HW error evt has started*/
          bt_rm_hwerr_radio_off = TRUE;

          bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;
          radio_on_off_cmd.cmd_hdr.bt_app_id = BT_APP_ID_NULL;
          radio_on_off_cmd.cmd_hdr.cmd_type  = BT_CMD_RM_DISABLE_RADIO;

          /* Switch off the BT radio */
          bt_dc.radio_state = BT_RADIO_DISABLED;
          radio_on_off_cmd.cmd_msg.cmd_rm_dsrad.disable = TRUE;
          bt_rm_cmd_disable_radio( &radio_on_off_cmd );

#ifdef FEATURE_BT_SOC1_FLASH
#error code not present
#else
          /* Power off SOC and shutdown SOC driver*/
          bt_dc_disable_driver();

          /* Following the radio-off with a Radio-on command */
          radio_on_off_cmd.cmd_msg.cmd_rm_dsrad.disable = FALSE;
          bt_rm_cmd_disable_radio( &radio_on_off_cmd );
#endif /* !FEATURE_BT_SOC1_FLASH */
        }
#endif /* !FEATURE_BT_SOC1_BC4_DEBUG */
        break;
      }

    default:
      {
        BT_ERR( "BT RM: Bad RX EV Type %x AID %x",
                ev_msg_ptr->ev_hdr.ev_type,
                ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_process_ev_queue

DESCRIPTION
  Dequeues events on the BT RM event queue and processes them.

===========================================================================*/
void bt_rm_process_ev_queue
(
  void
)
{

  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_rm_ev_q );

  BT_VSTAT_MAX( q_count, RM_MAX_EV_Q_DEPTH );

  BT_MSG_API_PYLD( "RM ev Q - cur_cnt = %x, max_cnt = %x",
                   q_count,
                   bt_vital_stats[BT_VSAI_RM_MAX_EV_Q_DEPTH], 0);

  while ( (bt_ev_ptr = (bt_ev_msg_type *) q_get( &bt_rm_ev_q )) != NULL )
  {
    bt_rm_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_store_bt_event

DESCRIPTION
  Stores a specified BT event on the RM BT event queue.

===========================================================================*/
void bt_rm_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_store_bt_event( bt_ev_ptr, &bt_rm_ev_q_info );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_get_remote_feat_mask

DESCRIPTION
  Processes the RM get remote feat mask cmd. remote_feat_ptr will be updated
  with mask read from the remote device when the return value from this API is
  is BT_CS_GN_SUCCESS
===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_remote_feat_mask
(
  bt_app_id_type               bt_app_id,
  bt_bd_addr_type*             bd_addr_ptr,
  bt_lmp_features_type*        remote_feat_ptr /* out parameter */
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Get Sync Param AID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Get Sync Param", bd_addr_ptr );

  if ( ( remote_feat_ptr != NULL ) && ( bd_addr_ptr != NULL ) )
  {
    TASKLOCK();

    if ( (cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,  /*  Connection must exist.       */
              FALSE,  /*  Connection need not be settled.  */
              bd_addr_ptr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      *remote_feat_ptr = conn_ptr->remote_lmp_features;
    }
    else
    {
      BT_MSG_HIGH( "BT RM: Get Remote Feat: No ACL", 0, 0, 0 );
      cmd_status = BT_CS_RM_NO_CONNECTION;
    }

    TASKFREE();
  }
  else
  {
    BT_MSG_HIGH( "BT RM: Get Remote Feat: bad param AID %x bdap %x paramp %x"
                  ,bt_app_id, bd_addr_ptr, remote_feat_ptr );
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_get_sync_conn_params

DESCRIPTION
  Processes the RM get sync conn params cmd. sync_param_ptr will be updated
  with sync parametes (for the existing sync transport) when the return value
  from this API is BT_CS_GN_SUCCESS

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_sync_conn_params
(
  bt_app_id_type               bt_app_id,
  bt_bd_addr_type*             bd_addr_ptr,
  bt_sync_conn_param_type*     sync_param_ptr /* out parameter */
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Get Sync Param AID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Get Sync Param", bd_addr_ptr );

  if ( ( sync_param_ptr != NULL ) && ( bd_addr_ptr != NULL ) )
  {
    TASKLOCK();

    if ( (cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,  /*  Connection must exist.       */
              FALSE,  /*  Connection need not be settled.  */
              bd_addr_ptr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( conn_ptr->state_sco == BT_RM_CS_CONNECTED )
      {
        *sync_param_ptr = conn_ptr->sync_conn_params;
      }
      else
      {
        BT_MSG_HIGH( "BT RM: Get Sync Param: No sync conn", 0, 0, 0 );
        cmd_status = BT_CS_RM_NO_CONNECTION;
      }
    }
    else
    {
      BT_MSG_HIGH( "BT RM: Get Sync Param: No ACL", 0, 0, 0 );
      cmd_status = BT_CS_RM_NO_CONNECTION;
    }

    TASKFREE();
  }
  else
  {
    BT_MSG_HIGH( "BT RM: Get Sync Param: bad param AID %x bdap %x paramp %x"
                  ,bt_app_id, bd_addr_ptr, sync_param_ptr );
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_cmd_rm_get_local_info

DESCRIPTION
  Processes the RM Get Local Information command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_local_info
(
  bt_app_id_type         bt_app_id,
  bt_bd_addr_type*       bd_addr_ptr,
  bt_version_type*       version_ptr,
  bt_lmp_features_type*  lmp_features_ptr,
  bt_cod_type*           class_of_device_ptr,
  char*                  bt_name_str_ptr,
  bt_security_type*      security_ptr //TODO
)
{

  BT_MSG_API( "BT RM CMD RX: RM Get Loc Inf AID %x", bt_app_id, 0, 0 );

  TASKLOCK();

  if ( bd_addr_ptr != NULL )
  {
    *bd_addr_ptr = bt_my_bd_addr;
  }

#ifndef FEATURE_BT_SOC
  if ( version_ptr != NULL )
  {
    version_ptr->lmp_version       = BT_MY_LMP_VERSION;
    version_ptr->manufacturer_name = BT_LMP_COMP_ID_QCOM;
    version_ptr->lmp_subversion    = BT_LMP_SUB_VER_NUM_0;
  }

  if ( lmp_features_ptr != NULL )
  {
    *lmp_features_ptr = bt_lm_data.attributes.lmp_features;
  }
#else
    bt_soc_etc_get_info( version_ptr,
                         lmp_features_ptr );
#endif /* FEATURE_BT_SOC */

  if ( class_of_device_ptr != NULL )
  {
#ifndef FEATURE_BT_SOC
    *class_of_device_ptr = bt_lm_data.attributes.class_of_device;
#else
    *class_of_device_ptr = bt_class_of_device;
#endif /* FEATURE_BT_SOC */

  }

  if ( bt_name_str_ptr != NULL )
  {
    bt_efs_params.bt_name[ BT_MAX_NAME_LEN ] = 0;
    memcpy( (void *)(bt_name_str_ptr),
            (void *)(bt_efs_params.bt_name),
            BT_MAX_NAME_LEN );
  }

  if ( security_ptr != NULL ) //TODO
  {
    *security_ptr = bt_rm_data.security;
  }

  TASKFREE();

  return ( BT_CS_GN_SUCCESS );

}


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_cmd_rm_get_local_info_ext

DESCRIPTION
  Processes the RM Get Local Information Extended command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_local_info_ext
(
  bt_app_id_type         bt_app_id,
  bt_bd_addr_type*       bd_addr_ptr,
  bt_version_type*       version_ptr,
  bt_lmp_features_type*  lmp_features_ptr,
  bt_cod_type*           class_of_device_ptr,
  char*                  bt_name_str_ptr,
  bt_security_type*      security_ptr,
  char*                  bt_short_name_str_ptr
)
{

  BT_MSG_API( "BT RM CMD RX: RM Get Loc Inf Ext AID %x", bt_app_id, 0, 0 );

  /* Call the Get local info function */
  bt_cmd_rm_get_local_info( bt_app_id, bd_addr_ptr, version_ptr,
                            lmp_features_ptr, class_of_device_ptr,
                            bt_name_str_ptr, security_ptr );

  TASKLOCK();

  /* Short name */
  if( bt_short_name_str_ptr != NULL )
  {
    BT_STRCPY( (char *)bt_short_name_str_ptr,
               (char *)bt_efs_params.bt_short_name,
               (BT_MAX_EIR_NAME_LEN + 1) );
  }

  TASKFREE();

  return ( BT_CS_GN_SUCCESS );

}
#endif /* BT_SWDEV_2_1_EIR */


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_local_info

DESCRIPTION
  Processes the RM Set Local Information command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_local_info
(
  bt_cmd_msg_type*  rm_sli_ptr
)
{

  dsm_item_type*  dsm_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Set LI C %x N %x AID %x",
              rm_sli_ptr->cmd_msg.cmd_rm_setif.cod_valid,
              rm_sli_ptr->cmd_msg.cmd_rm_setif.bt_name_valid,
              rm_sli_ptr->cmd_hdr.bt_app_id );

  rm_sli_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( rm_sli_ptr->cmd_msg.cmd_rm_setif.cod_valid != FALSE )
  {
    BT_MSG_API( "BT RM CMD TX: HC Wr COD %02x %02x %02x",
      rm_sli_ptr->cmd_msg.cmd_rm_setif.class_of_device.cod_bytes[ 2 ],
      rm_sli_ptr->cmd_msg.cmd_rm_setif.class_of_device.cod_bytes[ 1 ],
      rm_sli_ptr->cmd_msg.cmd_rm_setif.class_of_device.cod_bytes[ 0 ] );
    bt_cmd_hc_wr_class_of_device(
      rm_sli_ptr->cmd_msg.cmd_rm_setif.class_of_device );
  }

  if ( rm_sli_ptr->cmd_msg.cmd_rm_setif.bt_name_valid != FALSE )
  {
#ifdef FEATURE_BT_SOC
    /* make sure the length stay in range.  Only saving BT_MAX_NAME_LEN
     * In case of QC baseband, save to efs is done below HCI.
    */
    memcpy( (void *)(bt_efs_params.bt_name),
            (void*)(rm_sli_ptr->cmd_msg.cmd_rm_setif.bt_name_str),
            BT_MAX_NAME_LEN );

    bt_efs_params.bt_name[ BT_MAX_NAME_LEN ] = 0;

    /* Save in EFS */
    bt_write_to_efs( FALSE );
#endif

    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB,
                                   BT_MAX_LOCAL_DEVICE_NAME_LEN );

    dsm_pushdown_tail(
      &dsm_ptr,
      (void*)(rm_sli_ptr->cmd_msg.cmd_rm_setif.bt_name_str),
      BT_MAX_LOCAL_DEVICE_NAME_LEN, DSM_DS_SMALL_ITEM_POOL );

    BT_MSG_API( "BT RM CMD TX: HC Chg Loc Name", 0, 0, 0 );
    bt_cmd_hc_change_local_name( dsm_ptr );
  }
}


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_cmd_set_local_info_ext

DESCRIPTION
  Processes the RM Set Local Information Ext command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_local_info_ext
(
  bt_cmd_msg_type*  rm_sli_ptr
)
{

  BT_MSG_API( "BT RM CMD RX: RM Set LI Ext C %x N %x AID %x",
              rm_sli_ptr->cmd_msg.cmd_rm_setif_ext.cod_valid,
              rm_sli_ptr->cmd_msg.cmd_rm_setif_ext.bt_name_valid,
              rm_sli_ptr->cmd_hdr.bt_app_id );

  rm_sli_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  bt_rm_cmd_set_local_info( rm_sli_ptr );

  /* BT Short name */
  if ( rm_sli_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_valid != FALSE )
  {
    bt_rm_update_eir_short_name( rm_sli_ptr->cmd_msg.cmd_rm_setif_ext.bt_short_name_str,
                                 TRUE );

    /* Save in EFS */
    bt_write_to_efs( FALSE );

    bt_rm_update_eir();

  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Set Loc Inf Ext No name", 0, 0, 0 );
  }

}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_cmd_wr_eir_data

DESCRIPTION
  Processes the RM Write EIR data command

===========================================================================*/
LOCAL void bt_rm_cmd_wr_eir_data
(
  bt_cmd_msg_type*  rm_sli_ptr
)
{
  BT_MSG_API( "BT RM CMD RX: RM Wr EIR Data : T %x Bytes %x",
               rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_type,
               rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size, 0);

  rm_sli_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch( rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_type )
  {
    case BT_EIR_TYPE_MANUF_SPECIFIC_DATA:
      if( rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size <=
                                           BT_MAX_EIR_MANUF_DATA_SIZE )
  {
    bt_efs_params.eir_manuf_data_size =
                    rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size;
    memcpy( (void *)bt_efs_params.eir_manuf_data,
                (void *)rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data,
                rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size );

    /* Update the EIR packet in the SoC */
    bt_rm_update_eir();

    /* Save in EFS */
    bt_write_to_efs( FALSE );
  }
  else
  {
        BT_MSG_DEBUG( "BT RM: CMD WR EIR LRG %x",
                      rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size, 0, 0 );
      }
      break;

    default:
      BT_MSG_DEBUG( "BT RM: CMD WR EIR UNKN %x",
                    rm_sli_ptr->cmd_msg.cmd_rm_wr_eir.eir_data_size, 0, 0 );
      break;
  }
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef BT_SWDEV_2_1_EIR
/*===========================================================================

FUNCTION
  bt_rm_cmd_wr_eir_uuid

DESCRIPTION
  Processes the RM Write UUID command

===========================================================================*/
LOCAL void bt_rm_cmd_wr_eir_uuid
(
  bt_cmd_msg_type*  rm_sli_ptr
)
{
  boolean error = TRUE;

  BT_MSG_API( "BT RM CMD RX: RM Wr EIR Data : %x Bytes, Type: %x",
               rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size,
               rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_type, 0);

  rm_sli_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch( rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_type )
  {
    case BT_UUID_16BIT_TYPE:
      if( rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size <=
                                  BT_MAX_EIR_UUID16 * sizeof(bt_sd_uuid_type) )
      {
        error = FALSE;
        bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_16BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_list_complete;
        bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_16BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size;
        memcpy( (void *)bt_rm_device_eir.uuid_list.uuid16,
                (void *)rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data,
                bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_16BIT_TYPE ] );
      }

      break;

    case BT_UUID_32BIT_TYPE:
      if( rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size <=
                                  BT_MAX_EIR_UUID32 * sizeof(bt_sd_uuid32_type) )
      {
        error = FALSE;
        bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_32BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_list_complete;
        bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_32BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size;
        memcpy( (void *)bt_rm_device_eir.uuid_list.uuid32,
                (void *)rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data,
                bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_32BIT_TYPE ] );
      }
      break;

    case BT_UUID_128BIT_TYPE:
      if( rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size <=
                                  BT_MAX_EIR_UUID128 * sizeof(bt_sd_uuid128_type) )
      {
        error = FALSE;
        bt_rm_device_eir.uuid_list.uuid_list_complete[ BT_UUID_128BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_list_complete;
        bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_128BIT_TYPE ] =
                        rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data_size;
        memcpy( (void *)bt_rm_device_eir.uuid_list.uuid128,
                (void *)rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_data,
                bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_128BIT_TYPE ] );
      }
      break;

    default:
      BT_MSG_DEBUG( "BT RM: CMD WR EIR UUID Invalid UUID type %x",
                    rm_sli_ptr->cmd_msg.cmd_rm_wr_uuid.uuid_type, 0, 0 );
      break;
  }

  if( error == FALSE )
  {
    /* Update the EIR packet in the SoC */
    bt_rm_update_eir();
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: CMD WR EIR UUID Large", 0, 0, 0 );
  }
}
#endif /* BT_SWDEV_2_1_EIR */


#ifdef FEATURE_BT_SOC
/*===========================================================================

FUNCTION
  bt_rm_update_scan_params

DESCRIPTION
  Update scan params based on response v/s pwr tradeoff requested by app.

===========================================================================*/

LOCAL void bt_rm_update_scan_params
(
  bt_rm_act_vs_pwr_type  page_scan_act_vs_pwr,
  bt_rm_act_vs_pwr_type  inq_scan_act_vs_pwr
)
{
  switch ( page_scan_act_vs_pwr )
  {
    case BT_RM_AVP_AUTOMATIC:
      /* soc default, standard */
      bt_rm_data.conf_page_scan_interval = BT_PAGESCAN_INTERVAL_HCI_DEF;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_page_scan_type = BT_PS_STANDARD;
#endif /* FEATURE_BT_1_2 */
      break;

    case BT_RM_AVP_SMALLEST_ACT_MOST_POWER:
      /* 640ms, std. Legacy values */
      bt_rm_data.conf_page_scan_interval = BT_PAGESCAN_INTERVAL_HCI_DEF/2;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_page_scan_type = BT_PS_STANDARD;
#endif /* FEATURE_BT_1_2 */
      break;

    case BT_RM_AVP_LARGER_ACT_LESS_POWER:
      /* 1.28s, interlaced */
      bt_rm_data.conf_page_scan_interval = BT_PAGESCAN_INTERVAL_HCI_DEF;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_page_scan_type = BT_PS_INTERLACED;
#endif /* FEATURE_BT_1_2 */
      break;

    case BT_RM_AVP_LARGEST_ACT_LEAST_POWER:
      /* 2.56s, interlaced */
      bt_rm_data.conf_page_scan_interval = 2*(BT_PAGESCAN_INTERVAL_HCI_DEF);
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_page_scan_type = BT_PS_INTERLACED;
#endif /* FEATURE_BT_1_2 */
      break;

    default:
    /* Nothing to do */
      BT_ERR( "BT RM: Invalid PS act_vs_pwr %x",
                page_scan_act_vs_pwr, 0, 0 );
      break;
  }

  switch ( inq_scan_act_vs_pwr )
  {
    case BT_RM_AVP_AUTOMATIC:
      /* soc default, standard */
      bt_rm_data.conf_inq_scan_interval = BT_INQUIRYSCAN_INTERVAL_HCI_DEF;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_inq_scan_type = BT_IS_STANDARD;
#endif /* FEATURE_BT_1_2 */
      break;

    case BT_RM_AVP_SMALLEST_ACT_MOST_POWER:
      /* 640ms, std. Legacy values */
      bt_rm_data.conf_inq_scan_interval = BT_INQUIRYSCAN_INTERVAL_HCI_DEF/4;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_inq_scan_type = BT_IS_STANDARD;
#endif /* FEATURE_BT_1_2 */
      break;

    case BT_RM_AVP_LARGER_ACT_LESS_POWER:
      /* 1.28s, interlaced */
      bt_rm_data.conf_inq_scan_interval = BT_INQUIRYSCAN_INTERVAL_HCI_DEF/2;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_inq_scan_type = BT_IS_INTERLACED;
 #endif /* FEATURE_BT_1_2 */
     break;

    case BT_RM_AVP_LARGEST_ACT_LEAST_POWER:
      /* 2.56s, interlaced */
      bt_rm_data.conf_inq_scan_interval = BT_INQUIRYSCAN_INTERVAL_HCI_DEF;
#ifdef FEATURE_BT_1_2
      bt_rm_data.conf_inq_scan_type = BT_IS_INTERLACED;
#endif /* FEATURE_BT_1_2 */
      break;

    default:
    /* Nothing to do */
      BT_ERR( "BT RM: Invalid IS act_vs_pwr %x",
                inq_scan_act_vs_pwr, 0, 0 );
      break;
  }

}
#endif /* FEATURE_BT_SOC */

/*===========================================================================

FUNCTION
  bt_rm_cmd_config_application

DESCRIPTION
  Processes the RM Configure Application command.

===========================================================================*/
LOCAL void bt_rm_cmd_config_application
(
  bt_cmd_msg_type*  rm_ca_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RM CfgApp R %x PSAVP %x AID %x",
              rm_ca_ptr->cmd_msg.cmd_rm_apcfg.remove_app_config,
              rm_ca_ptr->cmd_msg.cmd_rm_apcfg.page_scan_act_vs_pwr,
              rm_ca_ptr->cmd_hdr.bt_app_id );

  rm_ca_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_ca_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be new application.  */
            rm_ca_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( rm_ca_ptr->cmd_msg.cmd_rm_apcfg.remove_app_config == FALSE )
    {
      app_ptr->conn_act_vs_pwr =
        rm_ca_ptr->cmd_msg.cmd_rm_apcfg.page_scan_act_vs_pwr;

      app_ptr->disc_act_vs_pwr =
        rm_ca_ptr->cmd_msg.cmd_rm_apcfg.inq_scan_act_vs_pwr;

      app_ptr->deliver_dev_updates =
        rm_ca_ptr->cmd_msg.cmd_rm_apcfg.deliver_dev_updates;

      bt_rm_init_unused_application( app_ptr );

#ifdef FEATURE_BT_SOC
      /*
       * This API saves the configuration on a per app basis.
       * But with the current implementation, scan settings used will be based
       * on the most recent call to this api, irrespective of the app_id which
       * makes the call to the api. Currently, scan parameters are changed only
       * for soc based targets. App can disable/enable service registration to
       * ensure that the updates are applied immediately to the controller.
      */
      bt_rm_update_scan_params (
                       rm_ca_ptr->cmd_msg.cmd_rm_apcfg.page_scan_act_vs_pwr,
                       rm_ca_ptr->cmd_msg.cmd_rm_apcfg.inq_scan_act_vs_pwr );
#endif /* FEATURE_BT_SOC */

      /* bt_rm_check_scan_settings(); */
    }
    else
    {
      bt_rm_init_application( app_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_auto_service_search_disable

DESCRIPTION
  Processes a RM Automatic Service Search Disable command.

===========================================================================*/
LOCAL void bt_rm_cmd_auto_service_search_disable
(
  bt_cmd_msg_type*  rm_ssd_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Auto SS Dis %x AID %x",
              rm_ssd_ptr->cmd_msg.cmd_rm_ssdis.disable,
              rm_ssd_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_ssd_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be a new application.  */
            rm_ssd_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->auto_service_search_disabled =
      rm_ssd_ptr->cmd_msg.cmd_rm_ssdis.disable;

    bt_rm_init_unused_application( app_ptr );
  }

}

/*===========================================================================

FUNCTION
  bt_rm_cmd_register_link_status

DESCRIPTION
  Processes a RM Register Link Status command.

===========================================================================*/
LOCAL void bt_rm_cmd_register_link_status
(
  bt_cmd_msg_type*  rm_rls_ptr
)
{

  bt_rm_link_status_reg_type*  lsr_ptr = NULL;

  BT_MSG_API( "BT RM CMD RX: RM Reg LStat AID %x",
              rm_rls_ptr->cmd_hdr.bt_app_id, 0 , 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Reg LStat",
          &rm_rls_ptr->cmd_msg.cmd_rm_regls.bd_addr );

  if ( rm_rls_ptr->cmd_msg.cmd_rm_regls.rssi_delta < BT_RM_INVALID_RSSI )
  {
    if ( (rm_rls_ptr->cmd_hdr.cmd_status =
            bt_rm_get_link_status_reg(
              TRUE,  /*  Can be new.  */
              rm_rls_ptr->cmd_hdr.bt_app_id,
              &rm_rls_ptr->cmd_msg.cmd_rm_regls.bd_addr,
              &lsr_ptr )) == BT_CS_GN_SUCCESS )
    {
      lsr_ptr->security_changes =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.security_changes;
      lsr_ptr->mode_changes =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.mode_changes;
      lsr_ptr->role_switches =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.role_switches;
      lsr_ptr->new_acl_pkts =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.new_acl_pkts;
      lsr_ptr->new_sco_pkts =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.new_sco_pkts;
      lsr_ptr->rssi_delta =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.rssi_delta;
      lsr_ptr->rssi_last = BT_RM_INVALID_RSSI;
      lsr_ptr->sync_param_updates =
        rm_rls_ptr->cmd_msg.cmd_rm_regls.sync_param_updates;

      bt_rm_ev_send_link_status( lsr_ptr->bt_app_id, lsr_ptr->conn_ptr );
    }
  }
  else
  {
    rm_rls_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_unregister_link_status

DESCRIPTION
  Processes a RM Unregister Link Status command.

===========================================================================*/
LOCAL void bt_rm_cmd_unregister_link_status
(
  bt_cmd_msg_type*  rm_uls_ptr
)
{

  bt_rm_link_status_reg_type*  lsr_ptr = NULL;

  BT_MSG_API( "BT RM CMD RX: RM Unreg LS AID %x",
              rm_uls_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Unreg LS",
          &rm_uls_ptr->cmd_msg.cmd_rm_urgls.bd_addr );

  if ( (rm_uls_ptr->cmd_hdr.cmd_status =
          bt_rm_get_link_status_reg(
            FALSE,  /*  Must exist.  */
            rm_uls_ptr->cmd_hdr.bt_app_id,
            &rm_uls_ptr->cmd_msg.cmd_rm_urgls.bd_addr,
            &lsr_ptr )) == BT_CS_GN_SUCCESS )
  {
    bt_rm_init_link_status_reg( lsr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_get_link_status

DESCRIPTION
  Processes a RM Get Link Status command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_link_status
(
  bt_app_id_type           bt_app_id,
  bt_rm_link_status_type*  link_status_ptr
)
{

  bt_cmd_status_type  cmd_status;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Get LS AID %x", bt_app_id, 0, 0 );

  if ( link_status_ptr != NULL )
  {
    BT_BDA( MSG_API, "BT RM CMD RX: RM Get LS",
            &link_status_ptr->bd_addr );

    TASKLOCK();

    if ( (cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,   /*  Connection must exist.           */
              FALSE,  /*  Connection need not be settled.  */
              &link_status_ptr->bd_addr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      bt_rm_copy_link_status( conn_ptr, link_status_ptr );
    }

    TASKFREE();
  }
  else
  {
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_register_radio_activity

===========================================================================*/
LOCAL void bt_rm_cmd_register_radio_activity
(
  bt_cmd_msg_type*  rm_rra_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Reg RadioAct AID %x",
              rm_rra_ptr->cmd_hdr.bt_app_id, 0 , 0 );

  if ( (rm_rra_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be a new application.  */
            rm_rra_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->radio_activity_registered = TRUE;
  }

}

/*===========================================================================

FUNCTION
  bt_rm_cmd_unregister_radio_activity

===========================================================================*/
LOCAL void bt_rm_cmd_unregister_radio_activity
(
  bt_cmd_msg_type*  rm_ura_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Unreg RadioAct AID %x",
              rm_ura_ptr->cmd_hdr.bt_app_id, 0 , 0 );

  if ( (rm_ura_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            FALSE,  /*  Must be existing application.  */
            rm_ura_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->radio_activity_registered = FALSE;
    bt_rm_init_unused_application( app_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_get_radio_activity

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_radio_activity
(
  bt_app_id_type          bt_app_id,
  bt_radio_activity_type* radio_activity_ptr
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT RM CMD RX: RM Get RadioAct AID %x", bt_app_id, 0, 0 );

  if ( radio_activity_ptr != NULL )
  {
    if ( bt_dc.radio_state == BT_RADIO_DISABLED )
    {
      *radio_activity_ptr = BT_RA_RADIO_FORCED_OFF;
    }
    else
    {
      *radio_activity_ptr = bt_rm_data.current_radio_activity;
    }
  }
  else
  {
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return ( cmd_status );

}

/*===========================================================================

FUNCTION
  bt_rm_lp_ctrl_to_link_mode

DESCRIPTION
  Returns required idle link mode for the given lpc ctrl.

===========================================================================*/
LOCAL bt_link_mode_type bt_rm_lp_ctrl_to_link_mode
(
  bt_low_power_ctrl_type     low_power_ctrl
)
{
  bt_link_mode_type          idle_link_mode = BT_LM_SNIFF;
  switch ( low_power_ctrl )
  {
    case BT_LPC_ACTIVE:
      {
        idle_link_mode = BT_LM_ACTIVE;
        break;
      }
    case BT_LPC_PARK:
    {
      idle_link_mode = BT_LM_PARK;
      break;
    }
    case BT_LPC_HOLD:
    {
      idle_link_mode = BT_LM_HOLD;
      break;
    }
    case BT_LPC_SNIFF_LOCK:
      /*
       * This option allows sending ACL data while in sniff.
       * Device will not be brought out of sniff when ACL data is to be
       * transmitted during sniff (using bt_rm_cmd_tx_data_acl).
       */
      {
        idle_link_mode = BT_LM_SNIFF_LOCK;
        break;
      }
    case BT_LPC_DONT_CARE:
    case BT_LPC_SNIFF:
    default:
      {
        idle_link_mode = BT_LM_SNIFF;
        break;
      }
  }
  return ( idle_link_mode );
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_low_power_config

DESCRIPTION
  Processes a RM Low Power Configuration command.

===========================================================================*/
LOCAL void bt_rm_cmd_low_power_config
(
  bt_cmd_msg_type*  rm_lpc_ptr
)
{

  bt_rm_low_power_cfg_type*  lpc_ptr = NULL;
  bt_low_power_ctrl_type     low_power_ctrl;
  uint16                     max_interval;
  uint16                     idle_timeout;
  uint16                     prev_link_policy;
  bt_rm_conn_type*           conn_ptr = NULL;
  boolean                    sniff_now = FALSE;

  low_power_ctrl = rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.low_power_ctrl;
  max_interval = BT_RM_MS_TO_625USEC(
                   rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.max_interval_ms );
  idle_timeout = rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.idle_timeout_ms;

  BT_MSG_API( "BT RM CMD RX: RM LPCfg AID %x C %x I %x",
              rm_lpc_ptr->cmd_hdr.bt_app_id, low_power_ctrl,
              rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.max_interval_ms );

  rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( idle_timeout == 0 )
  {
    /* Manual control. Update idle link mode on the connection */
    if (  bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.bd_addr,
            BT_ACL_LINK,
            &conn_ptr ) == BT_CS_GN_SUCCESS )
    {
      if ( conn_ptr->rd_remote_req_pending != TRUE)
      {
        prev_link_policy = conn_ptr->link_policy;

        switch( low_power_ctrl )
        {
          case BT_LPC_DONT_CARE:
            /* deregister the associated entry in LPC table */
            if ( (rm_lpc_ptr->cmd_hdr.cmd_status =
                    bt_rm_get_low_power_config(
                      rm_lpc_ptr->cmd_hdr.bt_app_id,
                      &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.bd_addr,
                      &lpc_ptr )) == BT_CS_GN_SUCCESS )
            {
              bt_rm_init_low_power_config( lpc_ptr );
            }

            /* Revert to previously configured LPC(if any) or default.
            ** e.g. reconfigure idle_timeout.
            */
            bt_rm_set_connection_lpc( conn_ptr );
            break;

          case BT_LPC_ACTIVE:
            conn_ptr->idle_timeout = 0;
            conn_ptr->idle_timeout_ms_left = 0;
            conn_ptr->idle_link_mode = BT_LM_ACTIVE;

            /* Update link policy to disable remote initiated sniff */
            conn_ptr->link_policy &= ~BT_LINK_POLICY_SNIFF_MODE_B;
            /*  Exit LPM mode if in LPM mode */
            bt_rm_acl_link_active( conn_ptr );
            break;

          case BT_LPC_SNIFF:
          case BT_LPC_SNIFF_LOCK:
            /* Re-enable remote initiated sniff */
            conn_ptr->link_policy |= BT_LINK_POLICY_SNIFF_MODE_B;
            /* Update idle mode to the "manually" requested mode */
            conn_ptr->idle_link_mode = bt_rm_lp_ctrl_to_link_mode(low_power_ctrl);
            if (conn_ptr->link_mode == BT_LM_ACTIVE)
            {
              sniff_now = TRUE;
            }
            break;

          default:
            /* Unsupported ctrl for manual mode */
            rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
        }

        if ( rm_lpc_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
        {
          if ( conn_ptr->link_policy != prev_link_policy )
          {
            BT_MSG_API( "BT RM CMD TX: HC Wr LP %x H %x RH %x",
                        conn_ptr->link_policy, conn_ptr->hc_handle_acl,
                        conn_ptr->rm_handle );
            bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                                      conn_ptr->link_policy );
          }

          if ( sniff_now == TRUE )
          {
            conn_ptr->idle_timeout_ms_left = BT_BACKGROUND_TIMER_MS;
            bt_rm_check_idle_timeout( conn_ptr );
          }
        }
      }
      else
      {
        /* Remote feture read from controller in progress */
        /* Process cmd later */
        rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
    else
    {
      rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
    }
  }
  else /* idle_timeout != 0 */
  {
    /* Create/update LPC and update idle link mode on the connection */
    if ( low_power_ctrl < BT_LPC_LAST )
    {
      if ( max_interval >= BT_RM_DEF_MIN_INTERVAL )
      {
        if ( (rm_lpc_ptr->cmd_hdr.cmd_status =
                bt_rm_get_low_power_config(
                  rm_lpc_ptr->cmd_hdr.bt_app_id,
                  &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.bd_addr,
                  &lpc_ptr )) == BT_CS_GN_SUCCESS )
        {
          if ( low_power_ctrl == BT_LPC_DONT_CARE )
          {
            bt_rm_init_low_power_config( lpc_ptr );
          }
          else
          {
            lpc_ptr->low_power_ctrl = low_power_ctrl;
            lpc_ptr->idle_timeout   = idle_timeout;
            lpc_ptr->max_interval   = max_interval;
          }

          /* After updating the lpc table,
          ** now use the new lpc to update the conn table.
          */
          bt_rm_set_connection_lpc( lpc_ptr->conn_ptr );
        }
      }
      else
      {
        rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_INTERVAL;
      }
    }
    else
    {
      rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
    }
  }
}


#ifdef BT_SWDEV_2_1_SSR
/*===========================================================================

FUNCTION
  bt_rm_cmd_low_power_config_ext

DESCRIPTION
  Processes a RM Low Power Configuration Extension command.

===========================================================================*/
LOCAL void bt_rm_cmd_low_power_config_ext
(
  bt_cmd_msg_type*  rm_lpc_ptr
)
{

  bt_rm_low_power_cfg_type*  lpc_ptr = NULL;
  bt_low_power_ctrl_type     low_power_ctrl;
  uint16                     max_interval;
  uint16                     max_latency;
  uint16                     idle_timeout;
  uint16                     prev_link_policy;
  bt_rm_conn_type*           conn_ptr  = NULL;
  boolean                    sniff_now = FALSE;

  idle_timeout   = rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.idle_timeout_ms;
  low_power_ctrl = rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.low_power_ctrl;
  max_interval   = BT_RM_MS_TO_625USEC(
                     rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.max_interval_ms );
  max_latency    = BT_RM_MS_TO_625USEC(
                     rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.max_latency_ms );

  BT_MSG_API( "BT RM CMD RX: RM LPCfg Ext AID %x I %x L %x",
              rm_lpc_ptr->cmd_hdr.bt_app_id,
              max_interval,
              max_latency );

  rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( idle_timeout == 0 )
  {
    /* Manual control. Update idle link mode on the connection */
    if (  bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg.bd_addr,
            BT_ACL_LINK,
            &conn_ptr ) == BT_CS_GN_SUCCESS )
    {
      if ( conn_ptr->rd_remote_req_pending != TRUE)
      {
        prev_link_policy = conn_ptr->link_policy;

        switch( low_power_ctrl )
        {
          case BT_LPC_DONT_CARE:
            /* deregister the associated entry in LPC table */
            if ( (rm_lpc_ptr->cmd_hdr.cmd_status =
                    bt_rm_get_low_power_config(
                      rm_lpc_ptr->cmd_hdr.bt_app_id,
                      &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.bd_addr,
                      &lpc_ptr )) == BT_CS_GN_SUCCESS )
            {
              bt_rm_init_low_power_config( lpc_ptr );
            }

            /* Revert to previously configured LPC(if any) or default.
            ** e.g. reconfigure idle_timeout.
            */
            bt_rm_set_connection_lpc( conn_ptr );
            break;

          case BT_LPC_ACTIVE:
            bt_rm_init_conn_4_manual_active( conn_ptr );
            /*  Exit LPM mode if in LPM mode */
            bt_rm_acl_link_active( conn_ptr );
            break;

          case BT_LPC_SNIFF:
          case BT_LPC_SNIFF_LOCK:
            /* Re-enable remote initiated sniff */
            conn_ptr->link_policy |= BT_LINK_POLICY_SNIFF_MODE_B;
            /* Update idle mode to the "manually" requested mode */
            conn_ptr->idle_link_mode = bt_rm_lp_ctrl_to_link_mode(low_power_ctrl);
            if (conn_ptr->link_mode == BT_LM_ACTIVE)
            {
              sniff_now = TRUE;
            }
            break;

          default:
            /* Unsupported ctrl for manual mode */
            rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
        }

        if ( rm_lpc_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
        {
          if ( conn_ptr->link_policy != prev_link_policy )
          {
            BT_MSG_API( "BT RM CMD TX: HC Wr LP %x H %x RH %x",
                        conn_ptr->link_policy, conn_ptr->hc_handle_acl,
                        conn_ptr->rm_handle );
            bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                                      conn_ptr->link_policy );
          }

          if ( sniff_now == TRUE )
          {
            conn_ptr->idle_timeout_ms_left = BT_BACKGROUND_TIMER_MS;
            bt_rm_check_idle_timeout( conn_ptr );
          }
        }
      }
      else
      {
        /* Remote feture read from controller in progress */
        /* Process cmd later */
        rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
    else
    {
      rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
    }
  }
  else /* idle_timeout != 0 */
  {
    /* Create/update LPC and update idle link mode on the connection */
    if ( low_power_ctrl < BT_LPC_LAST )
    {
      if ( (max_interval >= BT_RM_DEF_MIN_INTERVAL) &&
           ((max_latency > max_interval) ||
            (max_latency <= BT_RM_SSR_DONT_CARE)) )
      {
        if ( (rm_lpc_ptr->cmd_hdr.cmd_status =
                bt_rm_get_low_power_config(
                  rm_lpc_ptr->cmd_hdr.bt_app_id,
                  &rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.bd_addr,
                  &lpc_ptr )) == BT_CS_GN_SUCCESS )
        {
          bt_rm_conn_type*  temp_conn_ptr = lpc_ptr->conn_ptr;

          if ( low_power_ctrl == BT_LPC_DONT_CARE )
          {
            bt_rm_init_low_power_config( lpc_ptr );
          }
          else
          {
            lpc_ptr->low_power_ctrl = low_power_ctrl;
            lpc_ptr->max_interval   = max_interval;
            lpc_ptr->idle_timeout   =
              rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.idle_timeout_ms;
            lpc_ptr->max_latency    = max_latency;
            lpc_ptr->stick_to_app_param       =
              rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.stick_to_app_param;
            lpc_ptr->sniff_initiator_oriented =
              rm_lpc_ptr->cmd_msg.cmd_rm_lpcfg_ext.sniff_initiator_oriented;
          }

          /* After updating the lpc table,
          ** now use the new lpc to update the conn table.
          */
          bt_rm_set_connection_lpc( temp_conn_ptr );
        }
      }
      else
      {
        rm_lpc_ptr->cmd_hdr.cmd_status = (max_interval < BT_RM_DEF_MIN_INTERVAL)?
          BT_CS_RM_LPC_BAD_INTERVAL : BT_CS_RM_LPC_BAD_LATENCY;
      }
    }
    else
    {
      rm_lpc_ptr->cmd_hdr.cmd_status = BT_CS_RM_LPC_BAD_CONFIG;
    }
  }

}
#endif /* BT_SWDEV_2_1_SSR */


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_connection_role

DESCRIPTION
  Processes a RM Set Connection Role command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_connection_role
(
  bt_cmd_msg_type*  rm_scr_ptr
)
{
  uint16                       i = 0;
  bt_rm_conn_type*             conn_ptr;
  bt_bd_addr_type*             bd_addr_ptr = NULL;


  if ( rm_scr_ptr->cmd_msg.cmd_rm_setrl.bd_addr_valid != FALSE )
  {
    bd_addr_ptr = &rm_scr_ptr->cmd_msg.cmd_rm_setrl.bd_addr;
  }

  BT_MSG_API( "BT RM CMD RX: RM Set C Role %x AID %x",
              rm_scr_ptr->cmd_msg.cmd_rm_setrl.role,
              rm_scr_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Set C Role", bd_addr_ptr );

  if ( (rm_scr_ptr->cmd_msg.cmd_rm_setrl.role == BT_ROLE_MASTER) ||
       (rm_scr_ptr->cmd_msg.cmd_rm_setrl.role == BT_ROLE_SLAVE) )
  {
    if ( bd_addr_ptr != NULL )
    {
      if ( (rm_scr_ptr->cmd_hdr.cmd_status =
              bt_rm_get_conn_bd_addr(
                TRUE,  /*  Connection must already yet.  */
                FALSE,  /*  Connection need not be settled.   */
                bd_addr_ptr,
                BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
      {
        if ( rm_scr_ptr->cmd_msg.cmd_rm_setrl.role != conn_ptr->role )
        {
          /*  For QSOC, always accept role switch cmd from app when
              HOST_DIS_SCATTERNET is not defined.
          */
#if ( defined (FEATURE_BT_HOST_DIS_SCATTERNET) ) || ( !defined (FEATURE_BT_QSOC) )
          if ( rm_scr_ptr->cmd_msg.cmd_rm_setrl.role == BT_ROLE_SLAVE )
          {
            uint16 i;
            for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
            {
              if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
                   (conn_ptr != &bt_rm_conn[ i ]) )
              {
                rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
                break;
              }
            }
          }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET || !FEATURE_BT_QSOC */

          if ( conn_ptr->state_sco != BT_RM_CS_DISCONNECTED )
          {
            rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
          }

          /* make sure there is no other pending switch requests */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( bt_rm_conn[ i ].role_request != BT_ROLE_NONE )
            {
              rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
              break;
            }
          }
        }
        else
        {
            rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
        }
      }


      if ( rm_scr_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
      {
         if( ( rm_scr_ptr->cmd_hdr.cmd_status =
               bt_rm_connection_settled( conn_ptr )) == BT_CS_GN_SUCCESS )
           {
             conn_ptr->role_request = rm_scr_ptr->cmd_msg.cmd_rm_setrl.role;
             /* set mss enc flags if encryption is "ON" for this connection */
             if(conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON)
             {
#ifdef FEATURE_BT_SOC
               bt_version_type              local_lmp_version;
               bt_lmp_features_type         local_lmp_features;

               bt_soc_etc_get_info( &local_lmp_version,&local_lmp_features );
               /* Do not expect Ecryption change events if Both Local and Remote
                  Devices support Pause Encryption Feature */
               if( !( ( conn_ptr->remote_lmp_features.lmp_features_bytes[5] &
                        BT_LM_FEATURE_BYTE5_PAUSE_ENCRYPTION ) &&
                       ( local_lmp_features.lmp_features_bytes[5] &
                        BT_LM_FEATURE_BYTE5_PAUSE_ENCRYPTION ) ) )
#endif /* FEATURE_BT_SOC */
                {
                     conn_ptr->enh_enc_state = BT_RM_ENH_ENC_LOCAL;
                }
            }
            bt_cmd_hc_switch_role( &conn_ptr->dev_ptr->dev_public.bd_addr,
                                    conn_ptr->role_request );
         }
      }
    }
    else
    {
      rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

      bt_rm_data.accept_conn_role = rm_scr_ptr->cmd_msg.cmd_rm_setrl.role;

      BT_MSG_DEBUG( "BT RM: Acpt Conn Role %x",
                    bt_rm_data.accept_conn_role, 0, 0 );
    }
  }
  else
  {
    rm_scr_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_get_connection_role

DESCRIPTION
  Processes the RM Get Connection Role command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_connection_role
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  bt_role_type*     role_ptr
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Get C Role AID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Get C Role", bd_addr_ptr );

  if ( role_ptr != NULL )
  {
    *role_ptr = BT_ROLE_NONE;

    TASKLOCK();

    if ( bd_addr_ptr != NULL )
    {
      if ( (cmd_status =
              bt_rm_get_conn_bd_addr(
                TRUE,  /*  Connection must exist.       */
                FALSE,  /*  Connection need not be settled.  */
                bd_addr_ptr,
                BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
      {
        *role_ptr = conn_ptr->role;
      }
    }
    else
    {
      *role_ptr = bt_rm_data.accept_conn_role;
    }

    TASKFREE();

    if ( cmd_status == BT_CS_GN_RETRY_CMD_LATER )
    {
      cmd_status = BT_CS_RM_CONN_ACTIVE;
    }
  }
  else
  {
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_conn_role_sw_req_ok

DESCRIPTION
  Processes a RM Set Connection Role Switch Request OK command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_conn_role_sw_req_ok
(
  bt_cmd_msg_type*  rm_scrsr_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  boolean           cur_rsr_ok = FALSE;
  boolean           req_rsr_ok = FALSE;
  bt_bd_addr_type*  bd_addr_ptr = NULL;

  if ( rm_scrsr_ptr->cmd_msg.cmd_rm_scrsr.bd_addr_valid != FALSE )
  {
    bd_addr_ptr = &rm_scrsr_ptr->cmd_msg.cmd_rm_scrsr.bd_addr;
  }

  BT_MSG_API( "BT RM CMD RX: RM Set C RlSwReq OK %x AID %x",
              rm_scrsr_ptr->cmd_msg.cmd_rm_scrsr.role_sw_req_ok,
              rm_scrsr_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Set C RlSwReq OK", bd_addr_ptr );

  if ( rm_scrsr_ptr->cmd_msg.cmd_rm_scrsr.role_sw_req_ok != FALSE )
  {
    req_rsr_ok = TRUE;
  }

  if ( bd_addr_ptr != NULL )
  {
    if ( (rm_scrsr_ptr->cmd_hdr.cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,  /*  Connection must exist.       */
              TRUE,  /*  Connection must be settled.  */
              bd_addr_ptr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( (conn_ptr->link_policy & BT_LINK_POLICY_ROLE_SWITCH_B) != 0 )
      {
        cur_rsr_ok = TRUE;
      }

      if ( req_rsr_ok != cur_rsr_ok )
      {
#ifdef FEATURE_BT_HOST_DIS_SCATTERNET
        if (( req_rsr_ok == TRUE ) && ( bt_rm_total_active_acl_links() > 1 ))
        {
          BT_MSG_DEBUG( "BT RM: Disallow Role SW Req OK to prevent scatternet",
                         0, 0, 0 );
          rm_scrsr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
        }
        else
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
        {
          bt_rm_set_conn_role_switch_req_policy( conn_ptr, req_rsr_ok );

          BT_MSG_API( "BT RM CMD TX: HC Wr LP %x RH %x H %x",
                      conn_ptr->link_policy,
                      conn_ptr->hc_handle_acl, conn_ptr->rm_handle );
          bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                                    conn_ptr->link_policy );
        }
      }
    }
  }
  else
  {
    rm_scrsr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

    if ( bt_rm_data.create_conn_role_sw_req_ok == BT_ROLE_SWITCH_ALLOWED )
    {
      cur_rsr_ok = TRUE;
    }

    if ( req_rsr_ok != cur_rsr_ok )
    {
      if ( req_rsr_ok == FALSE )
      {
        bt_rm_data.create_conn_role_sw_req_ok = BT_ROLE_SWITCH_NOT_ALLOWED;
      }
      else
      {
        bt_rm_data.create_conn_role_sw_req_ok = BT_ROLE_SWITCH_ALLOWED;
      }

      BT_MSG_DEBUG( "BT RM: Cr Conn Role SW Req OK %x",
                    bt_rm_data.create_conn_role_sw_req_ok, 0, 0 );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_get_conn_role_sw_req_ok

DESCRIPTION
  Processes the RM Get Connection Role Switch Request OK command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_get_conn_role_sw_req_ok
(
  bt_app_id_type    bt_app_id,
  bt_bd_addr_type*  bd_addr_ptr,
  boolean*          role_sw_req_ok_ptr
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  bt_rm_conn_type*    conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Get C Role SWR OK AID %x",
              bt_app_id, 0, 0 );

  if ( role_sw_req_ok_ptr != NULL )
  {
    *role_sw_req_ok_ptr = FALSE;

    TASKLOCK();

    if ( bd_addr_ptr != NULL )
    {
      if ( (cmd_status =
              bt_rm_get_conn_bd_addr(
                TRUE,  /*  Connection must exist.       */
                FALSE,  /*  Connection need not be settled.  */
                bd_addr_ptr,
                BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
      {
        if ( (conn_ptr->link_policy & BT_LINK_POLICY_ROLE_SWITCH_B) != 0 )
        {
          *role_sw_req_ok_ptr = TRUE;
        }
      }
    }
    else
    {
      *role_sw_req_ok_ptr = bt_rm_data.create_conn_role_sw_req_ok;
    }

    TASKFREE();

    if ( cmd_status == BT_CS_GN_RETRY_CMD_LATER )
    {
      cmd_status = BT_CS_RM_CONN_ACTIVE;
    }
  }
  else
  {
    cmd_status = BT_CS_RM_BAD_PARAM;
  }

  return( cmd_status );

}

#ifndef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_cmd_set_connection_security

DESCRIPTION
  Processes a RM Set Connection Security command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_connection_security
(
  bt_cmd_msg_type*  rm_scs_ptr
)
{

  bt_rm_conn_type*             conn_ptr;
  bt_rm_link_status_reg_type*  lsr_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Set Conn Sec %x AID %x",
              rm_scs_ptr->cmd_msg.cmd_rm_setcs.security,
              rm_scs_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Set Conn Sec",
          &rm_scs_ptr->cmd_msg.cmd_rm_setcs.bd_addr );

  if ( rm_scs_ptr->cmd_msg.cmd_rm_setcs.security < BT_SEC_MAX )
  {
    if ( (rm_scs_ptr->cmd_hdr.cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,  /*  Connection must exist.       */
              TRUE,  /*  Connection must be settled.  */
              &rm_scs_ptr->cmd_msg.cmd_rm_setcs.bd_addr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( conn_ptr->security_min <
             rm_scs_ptr->cmd_msg.cmd_rm_setcs.security )
      {
        conn_ptr->security_min = rm_scs_ptr->cmd_msg.cmd_rm_setcs.security;

        if ( bt_rm_connection_secure_enough( conn_ptr, TRUE ) != FALSE )
        {
          /*  Send link status if application is registered for it.  */
          if ( bt_rm_get_link_status_reg(
                 FALSE,  /*  Must exist.  */
                 rm_scs_ptr->cmd_hdr.bt_app_id,
                 &conn_ptr->dev_ptr->dev_public.bd_addr,
                 &lsr_ptr ) == BT_CS_GN_SUCCESS )
          {
            bt_rm_ev_send_link_status( rm_scs_ptr->cmd_hdr.bt_app_id,
                                       conn_ptr );
          }
        }
      }
    }
  }
  else
  {
    rm_scs_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}
#endif /* BT_SWDEV_2_1_SSP */

#ifndef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_cmd_bond

DESCRIPTION
  Processes a RM Bond command.

===========================================================================*/
LOCAL void bt_rm_cmd_bond
(
  bt_cmd_msg_type*  rm_b_ptr
)
{

  bt_rm_conn_type*      conn_ptr;
  bt_event_reason_type  reason = BT_EVR_RM_NO_CONNECTION;

  BT_MSG_API( "BT RM CMD RX: RM Bond PINL %x AID %x",
              rm_b_ptr->cmd_msg.cmd_rm_bond.pin.length,
              rm_b_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Bond",
          &rm_b_ptr->cmd_msg.cmd_rm_bond.bd_addr );

  if ( rm_b_ptr->cmd_msg.cmd_rm_bond.pin.length > 0 )
  {
    if ( (rm_b_ptr->cmd_hdr.cmd_status =
            bt_rm_get_conn_bd_addr(
              FALSE,  /*  Connection need not exist yet.  */
              TRUE,   /*  Connection must be settled.     */
              &rm_b_ptr->cmd_msg.cmd_rm_bond.bd_addr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
      {
        if ( (rm_b_ptr->cmd_hdr.cmd_status =
                bt_rm_new_acl_connection_ok()) == BT_CS_GN_SUCCESS )
        {
          conn_ptr->bonding_app_id  = rm_b_ptr->cmd_hdr.bt_app_id;
          conn_ptr->bonding_pin     = rm_b_ptr->cmd_msg.cmd_rm_bond.pin;
          conn_ptr->bonding_key_rxd = FALSE;

          bt_rm_create_connection_acl( conn_ptr );
        }
        else
        {
          reason = BT_EVR_RM_NO_RESOURCES;
        }
      }
      else
      {
        conn_ptr->bonding_app_id  = rm_b_ptr->cmd_hdr.bt_app_id;
        conn_ptr->bonding_pin     = rm_b_ptr->cmd_msg.cmd_rm_bond.pin;
        conn_ptr->bonding_key_rxd = FALSE;

        BT_MSG_API( "BT RM CMD TX: HC Auth Req H %x",
                    conn_ptr->hc_handle_acl, 0, 0 );
        bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );

        bt_rm_reset_idle_timeout( conn_ptr );
      }
    }
  }
  else
  {
    rm_b_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
    reason = BT_EVR_RM_BAD_PARAM;
  }

  if ( (rm_b_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_b_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    bt_rm_ev_send_bond_failed( rm_b_ptr->cmd_hdr.bt_app_id,
                               &rm_b_ptr->cmd_msg.cmd_rm_bond.bd_addr,
                               (bt_rm_handle_type)(-1), reason );
  }

}
#endif /* BT_SWDEV_2_1_SSP */


/*===========================================================================

FUNCTION
  bt_rm_cmd_bond_cancel

DESCRIPTION
  Processes a RM Bond Cancel command.

===========================================================================*/
LOCAL void bt_rm_cmd_bond_cancel
(
  bt_cmd_msg_type*  rm_bc_ptr
)
{

  bt_cmd_msg_type   cmd;
  bt_rm_conn_type*  conn_ptr;
  bt_bd_addr_type*  bd_addr_ptr = 
                      &rm_bc_ptr->cmd_msg.cmd_rm_bondc.bd_addr;

  rm_bc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_BDA( MSG_API, "BT RM CMD RX: RM BondC",
          &rm_bc_ptr->cmd_msg.cmd_rm_bondc.bd_addr );

  if ( (conn_ptr = bt_rm_find_conn_bd_addr( bd_addr_ptr ) ) != NULL )
  {
    /* Allow only the app which initiated bonding to cancel, also ensures that 
       bonding is actually going on */
    if( conn_ptr->bonding_app_id == rm_bc_ptr->cmd_hdr.bt_app_id )
    {
      if( conn_ptr->bond_canceling == FALSE )
      {
        conn_ptr->bond_canceling = TRUE;
        if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTING) &&
             (conn_ptr->ev_hc_conn_acl_rxd == FALSE) )
        {
          /* The original create connection will trigger the HCI connection complete 
             with a failed status, thus  bt_rm_ev_hc_conn_complete_acl() will get called, 
             which in turn calls bt_rm_finish_bond_attempt() and BT_EV_RM_BOND_FAILED event 
             is sent upto the app */
          bt_cmd_hc_create_connection_cancel ( bd_addr_ptr );
        }
        else if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTING) &&
                  (conn_ptr->ev_hc_conn_acl_rxd == TRUE) )
        {
          /* Will trigger a  call to bt_rm_ev_hc_disconnection_complete() which should 
             call bt_rm_finish_bond_attempt() and BT_EV_RM_BOND_FAILED should be sent 
             to the app */
          bt_rm_disconnect_acl( conn_ptr, BT_RMDR_USER_ENDED );
        }
        else
        {
          /* ACL is either disconnected/disconnecting (or) ACL is connected and 
             bonding request did not initiate an ACL connection. This case is handled 
             in bt_rm_finish_bond_attempt() */
        }

        /* Handle the case if link key has been received already but bonded event
           has not been sent to the app */
        if( conn_ptr->bonding_key_rxd == TRUE )
        {
          BT_MSG_DEBUG( "BT RM: BondC Unbond", 0, 0, 0 );

          cmd.cmd_hdr.cmd_type             = BT_CMD_RM_UNBOND;
          cmd.cmd_hdr.bt_app_id            = BT_APP_ID_NULL;
          cmd.cmd_msg.cmd_rm_unbnd.bd_addr =
            conn_ptr->dev_ptr->dev_public.bd_addr;

          bt_rm_cmd_unbond( &cmd );
          if ( cmd.cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
          {
            BT_ERR( "BT RM: BondC Bad Unbond CS %x",
                    cmd.cmd_hdr.cmd_status, 0, 0 );
          }
        }
      }
      else
      {
        BT_MSG_DEBUG( "BT RM: BondC in progress", 0, 0, 0 );
        rm_bc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
      }
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Bad App id", 0, 0, 0 );
      rm_bc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
    }
  }
  else 
  {
    BT_MSG_DEBUG( "BT RM: BD Addr not in Conn table", 0, 0, 0 );
    rm_bc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_bondable

DESCRIPTION
  Processes the RM Set Bondable command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_bondable
(
  bt_cmd_msg_type*  rm_sb_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: SetB %x AID %x",
              rm_sb_ptr->cmd_msg.cmd_rm_setbd.bondable,
              rm_sb_ptr->cmd_hdr.bt_app_id, 0 );

  if ( rm_sb_ptr->cmd_msg.cmd_rm_setbd.bondable != FALSE )
  {
    if ( (rm_sb_ptr->cmd_hdr.cmd_status =
            bt_rm_get_application(
              TRUE,  /*  Can be a new application.  */
              rm_sb_ptr->cmd_hdr.bt_app_id,
              &app_ptr )) == BT_CS_GN_SUCCESS )
    {
      app_ptr->bondable = TRUE;
    }
  }
  else
  {
    if ( (rm_sb_ptr->cmd_hdr.cmd_status =
            bt_rm_get_application(
              FALSE,  /*  Must be existing application.  */
              rm_sb_ptr->cmd_hdr.bt_app_id,
              &app_ptr )) == BT_CS_GN_SUCCESS )
    {
      app_ptr->bondable = FALSE;

      bt_rm_init_unused_application( app_ptr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_pin_reply

DESCRIPTION
  Processes a RM PIN Reply command.

===========================================================================*/
LOCAL void bt_rm_cmd_pin_reply
(
  bt_cmd_msg_type*  rm_pr_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM PIN Rep V %x L %x AID %x",
              rm_pr_ptr->cmd_msg.cmd_rm_pin.pin_valid,
              rm_pr_ptr->cmd_msg.cmd_rm_pin.pin.length,
              rm_pr_ptr->cmd_hdr.bt_app_id );
  BT_BDA( MSG_API, "BT RM CMD RX: RM PIN Rep",
          &rm_pr_ptr->cmd_msg.cmd_rm_pin.bd_addr );

  rm_pr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_pr_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_pr_ptr->cmd_msg.cmd_rm_pin.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->pin_req_resps_pending > 0 )
    {
      if ( rm_pr_ptr->cmd_msg.cmd_rm_pin.pin_valid != FALSE )
      {
        if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
             (conn_ptr->pin_req_pairing_done == FALSE) )
        {
          if ( rm_pr_ptr->cmd_msg.cmd_rm_pin.pin.length >=
                  conn_ptr->security_min_pin_len_bytes )
          {
            conn_ptr->pin_req_resp_app_id = rm_pr_ptr->cmd_hdr.bt_app_id;

            bt_rm_send_hc_pin_code_req_reply(
              conn_ptr, rm_pr_ptr->cmd_msg.cmd_rm_pin.pin.length,
              (uint8*)rm_pr_ptr->cmd_msg.cmd_rm_pin.pin.pin_code );
          }
          else
          {
            conn_ptr->pin_code_reply_len_bytes =
              rm_pr_ptr->cmd_msg.cmd_rm_pin.pin.length;

            rm_pr_ptr->cmd_hdr.cmd_status = BT_CS_RM_PIN_TOO_SHORT;
          }
        }
        else
        {
          rm_pr_ptr->cmd_hdr.cmd_status = BT_CS_RM_CONN_ACTIVE;
          BT_MSG_DEBUG( "BT RM: Extra PIN Rep AID %x %x D %x",
                        rm_pr_ptr->cmd_hdr.bt_app_id,
                        conn_ptr->pin_req_resp_app_id,
                        conn_ptr->pin_req_pairing_done );
        }
      }

      if ( --conn_ptr->pin_req_resps_pending == 0 )
      {
        conn_ptr->pin_req_timeout_ms_left = 0;

        if ( (conn_ptr->pin_req_resp_app_id == BT_APP_ID_NULL) &&
             (conn_ptr->pin_req_pairing_done == FALSE) )
        {
          if (conn_ptr->re_pair_pin_state == BT_RM_RE_PAIR_PIN_WAITING)
          {
              /* Invalid PIN while pre-fetching pin for re-pairing ,
               * so just send the security result event
               */
              bt_rm_ev_send_security_result( conn_ptr );
          }
          else
          {
          bt_rm_send_hc_pin_reply_negative( conn_ptr, NULL );
          }
          bt_rm_reset_idle_timeout( conn_ptr );
        }

        bt_rm_init_completed_pin_request( conn_ptr );
      }
    }
    else
    {
      rm_pr_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNEXP_PIN_REPLY;
      BT_MSG_DEBUG( "BT RM: Unexp PIN Rep RH %x AID %x %x",
                    conn_ptr->rm_handle, rm_pr_ptr->cmd_hdr.bt_app_id,
                    conn_ptr->pin_req_resp_app_id );
    }
  }
  else
  {
    BT_ERR( "BT RM: PIN Rep Bad BD ADDR",0,0,0);
    rm_pr_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_authorize_reply

DESCRIPTION
  Processes a RM Authorize Reply command.

===========================================================================*/
LOCAL void bt_rm_cmd_authorize_reply
(
  bt_cmd_msg_type*  rm_ar_ptr
)
{

  bt_rm_service_sec_type*  ss_ptr;
  bt_rm_conn_type*         conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM AtzR AZd %x SvIDM %x UUID %x",
              rm_ar_ptr->cmd_msg.cmd_rm_atzrp.authorized,
              rm_ar_ptr->cmd_msg.cmd_rm_atzrp.service_id.service_id_method,
              rm_ar_ptr->cmd_msg.cmd_rm_atzrp.service_id.sdp_uuid );
  BT_BDA( MSG_API, "BT RM CMD RX: RM AtzR",
          &rm_ar_ptr->cmd_msg.cmd_rm_atzrp.bd_addr );

  if ( (rm_ar_ptr->cmd_hdr.cmd_status =
          bt_rm_get_service_security(
            FALSE,  /*  Must exist.  */
            rm_ar_ptr->cmd_hdr.bt_app_id,
            &rm_ar_ptr->cmd_msg.cmd_rm_atzrp.service_id,
            &ss_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( (rm_ar_ptr->cmd_hdr.cmd_status =
            bt_rm_get_conn_bd_addr(
              TRUE,   /*  Connection must exist.           */
              FALSE,  /*  Connection need not be settled.  */
              &rm_ar_ptr->cmd_msg.cmd_rm_atzrp.bd_addr,
              BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( rm_ar_ptr->cmd_hdr.bt_app_id == conn_ptr->authorize_req_app_id )
      {
        conn_ptr->authorize_req_app_id = BT_APP_ID_NULL;

        if ( rm_ar_ptr->cmd_msg.cmd_rm_atzrp.authorized != FALSE )
        {
          conn_ptr->authorized = TRUE;

          bt_rm_link_status_updated( conn_ptr );

          /*  Enforce connection security if necessary.  */

          if ( bt_rm_secure_connection( conn_ptr ) )
          {
            /*  Connection security level OK.    */
            /*  Send Security Result event now.  */
            bt_rm_ev_send_security_result( conn_ptr );
          }
        }
        else
        {
          /*  Connection to service not authorized.  */
          /*  Send Security Result event now.        */
          bt_rm_ev_send_security_result( conn_ptr );
        }

        bt_rm_reset_idle_timeout( conn_ptr );
      }
      else
      {
        rm_ar_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_device_add

DESCRIPTION
  Processes the RM Device Add command.

===========================================================================*/
LOCAL void bt_rm_cmd_device_add
(
  bt_cmd_msg_type*  rm_da_ptr
)
{

  bt_rm_device_type*  dev_ptr;
  bt_device_type*     new_dev_ptr;

  new_dev_ptr = &rm_da_ptr->cmd_msg.cmd_rm_dvadd.device;

  BT_MSG_API( "BT RM CMD RX: DevAdd AID %x",
              rm_da_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: DevAdd", &new_dev_ptr->bd_addr );

  rm_da_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (dev_ptr = bt_rm_get_device(
                    &new_dev_ptr->bd_addr, TRUE )) != NULL )
  {
    if ( dev_ptr->dev_public.valid == FALSE )
    {
      if ( new_dev_ptr->valid != FALSE )
      {
        /*  Use application supplied device information.  */
        dev_ptr->dev_public = *new_dev_ptr;
      }

      dev_ptr->dev_public.valid  = TRUE;
      dev_ptr->update_status    |= BT_RM_DUS_ADDED_B;

      if ( bt_rm_device_updated(
             rm_da_ptr->cmd_hdr.bt_app_id, dev_ptr ) == FALSE )
      {
        rm_da_ptr->cmd_hdr.cmd_status = BT_CS_RM_EFS_ERR;

        BT_ERR( "BT RM: DevAdd EFS error! DI %x",
                dev_ptr->index, 0, 0 );
      }
    }
    else
    {
      rm_da_ptr->cmd_hdr.cmd_status = BT_CS_RM_DEVICE_EXISTS;
    }
  }
  else
  {
    rm_da_ptr->cmd_hdr.cmd_status = BT_CS_RM_MAX_DEVICES;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_device_services_update

DESCRIPTION
  Processes a RM device services update command.

===========================================================================*/
LOCAL void bt_rm_cmd_device_services_update
(
  bt_cmd_msg_type*  rm_dsu_ptr
)
{
  bt_rm_device_type* dev_ptr;
  uint16             new_num_services, i;

  BT_MSG_API( "BT RM CMD RX: RM DevServUpd NS %x AID %x",
              rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.num_services,
              rm_dsu_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM DevServUpd",
          &rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.bd_addr );

  if ( ( dev_ptr = bt_rm_get_device(
           &rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.bd_addr, FALSE ) ) != NULL )
  {
    new_num_services = rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.num_services;

    if ( new_num_services > BT_SD_MAX_NUM_OF_SRV_REC )
    {
      rm_dsu_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
    }
    else
    {
      rm_dsu_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

      for ( i = 0; i < new_num_services; i++ )
      {
        if ( memcmp( (void*)&dev_ptr->service[ i ],
                     (void*)&rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.services[ i ],
                     sizeof( bt_service_type ) ) != 0 )
        {
          dev_ptr->service[ i ] =
            rm_dsu_ptr->cmd_msg.cmd_rm_dsupd.services[ i ];
          dev_ptr->update_status |= BT_RM_DUS_SERVICES_B;
        }
      }

      bt_rm_finish_service_update( rm_dsu_ptr->cmd_hdr.bt_app_id,
                                   dev_ptr, new_num_services );
    }
  }
  else
  {
    rm_dsu_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_DEVICE;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_set_device_security

DESCRIPTION
  Processes a RM Set Device Security command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_device_security
(
  bt_cmd_msg_type*  rm_sds_ptr
)
{

  bt_cmd_msg_type   cmd;
  bt_bd_addr_type*  bd_addr_ptr = NULL;

  if ( rm_sds_ptr->cmd_msg.cmd_rm_setds.bd_addr_valid != FALSE )
  {
    bd_addr_ptr = &rm_sds_ptr->cmd_msg.cmd_rm_setds.bd_addr;
  }

  BT_MSG_API( "BT RM CMD RX: Set DevSec %x AID %x",
              rm_sds_ptr->cmd_msg.cmd_rm_setds.security,
              rm_sds_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: Set DevSec", bd_addr_ptr );

  if ( rm_sds_ptr->cmd_msg.cmd_rm_setds.security < BT_SEC_MAX )
  {
    rm_sds_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

    if ( bd_addr_ptr != NULL )
    {
      cmd.cmd_msg.cmd_rm_dvupd.device.bd_addr =
        rm_sds_ptr->cmd_msg.cmd_rm_setds.bd_addr;

      if ( (rm_sds_ptr->cmd_hdr.cmd_status =
              bt_cmd_rm_device_read(
                bt_rm_app_id, &cmd.cmd_msg.cmd_rm_dvupd.device,
                NULL, NULL )) == BT_CS_GN_SUCCESS )
      {
        if ( cmd.cmd_msg.cmd_rm_dvupd.device.security !=
               rm_sds_ptr->cmd_msg.cmd_rm_setds.security )
        {
          cmd.cmd_hdr.cmd_type  = BT_CMD_RM_DEVICE_UPDATE;
          cmd.cmd_hdr.bt_app_id = rm_sds_ptr->cmd_hdr.bt_app_id;
          cmd.cmd_msg.cmd_rm_dvupd.device.security =
            rm_sds_ptr->cmd_msg.cmd_rm_setds.security;
          cmd.cmd_msg.cmd_rm_dvupd.update_control = BT_RM_DU_SECURITY_B;

          bt_rm_cmd_device_update( &cmd );

          rm_sds_ptr->cmd_hdr.cmd_status = cmd.cmd_hdr.cmd_status;
        }
      }
    }
    else
    {
      if ( bt_rm_data.security !=
             rm_sds_ptr->cmd_msg.cmd_rm_setds.security )
      {
        bt_rm_data.security = rm_sds_ptr->cmd_msg.cmd_rm_setds.security;

        if ( bt_rm_write_device_file() == FALSE )
        {
          rm_sds_ptr->cmd_hdr.cmd_status = BT_CS_RM_EFS_ERR;
        }
      }
    }
  }
  else
  {
    rm_sds_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}

#ifndef BT_SWDEV_2_1_SSP
/*===========================================================================

FUNCTION
  bt_rm_cmd_set_service_security

DESCRIPTION
  Processes a RM Set Service Security command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_service_security
(
  bt_cmd_msg_type*  rm_sss_ptr
)
{

  bt_rm_service_sec_type*  ss_ptr;

  BT_MSG_API( "BT RM CMD RX: Set SSec SIM %x UUID %x SCN %x",
              rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.service_id_method,
              rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.sdp_uuid,
              rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.rfcomm_scn );

  BT_MSG_API( "BT RM CMD RX: Set SSec PSM %x AF/ATZ %04x AID %x",
    rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.l2cap_psm,
    (uint16)((rm_sss_ptr->cmd_msg.cmd_rm_setss.authorize_first << 8) |
                rm_sss_ptr->cmd_msg.cmd_rm_setss.authorize),
    rm_sss_ptr->cmd_hdr.bt_app_id );

  if ( (rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.service_id_method !=
          BT_SIM_NONE) &&
       (rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id.service_id_method <
          BT_SIM_MAX) &&
       (rm_sss_ptr->cmd_msg.cmd_rm_setss.security < BT_SEC_MAX) )
  {
    if ( (rm_sss_ptr->cmd_hdr.cmd_status =
            bt_rm_get_service_security(
              TRUE,  /*  Can be new.  */
              rm_sss_ptr->cmd_hdr.bt_app_id,
              &rm_sss_ptr->cmd_msg.cmd_rm_setss.service_id,
              &ss_ptr )) == BT_CS_GN_SUCCESS )
    {
      if ( (rm_sss_ptr->cmd_msg.cmd_rm_setss.security == BT_SEC_NONE) &&
           (rm_sss_ptr->cmd_msg.cmd_rm_setss.authorize == FALSE) )
      {
        bt_rm_init_service_security( ss_ptr );
      }
      else
      {
        ss_ptr->security =
          rm_sss_ptr->cmd_msg.cmd_rm_setss.security;
        ss_ptr->authorize =
          rm_sss_ptr->cmd_msg.cmd_rm_setss.authorize;
        ss_ptr->authorize_first =
          rm_sss_ptr->cmd_msg.cmd_rm_setss.authorize_first;

        /*  Disable extended service security.  */
        ss_ptr->min_pin_len_bytes     = 0;
        ss_ptr->comb_link_key_only    = FALSE;
        ss_ptr->min_enc_key_len_bytes = 0;
      }
    }
  }
  else
  {
    rm_sss_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}
#endif /* BT_SWDEV_2_1_SSP */

/*===========================================================================

FUNCTION
  bt_rm_cmd_set_service_sec_ext

DESCRIPTION
  Processes a RM Set Service Security Extended command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_service_sec_ext
(
  bt_cmd_msg_type*  rm_sse_ptr
)
{

  bt_rm_service_sec_type*  ss_ptr;

  BT_MSG_API( "BT RM CMD RX: Set SSecExt SIM %x UUID %x SCN %x",
              rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.service_id_method,
              rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.sdp_uuid,
              rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.rfcomm_scn );

  BT_MSG_API( "BT RM CMD RX: Set SSecExt PSM %x MPB %x CKO %x",
              rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.l2cap_psm,
              rm_sse_ptr->cmd_msg.cmd_rm_setse.min_pin_len_bytes,
              rm_sse_ptr->cmd_msg.cmd_rm_setse.comb_link_key_only );

  BT_MSG_API( "BT RM CMD RX: Set SSecExt MEKB %x AID %x",
              rm_sse_ptr->cmd_msg.cmd_rm_setse.min_enc_key_len_bytes,
              rm_sse_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.service_id_method !=
          BT_SIM_NONE) &&
       (rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id.service_id_method <
          BT_SIM_MAX) )
  {
    if ( (rm_sse_ptr->cmd_hdr.cmd_status =
            bt_rm_get_service_security(
              FALSE,  /*  Must exist.  */
              rm_sse_ptr->cmd_hdr.bt_app_id,
              &rm_sse_ptr->cmd_msg.cmd_rm_setse.service_id,
              &ss_ptr )) == BT_CS_GN_SUCCESS )
    {
#ifdef BT_SWDEV_2_1_SSP
      if (ss_ptr->sec_level != BT_RM_SL_0_NONE)
#else
      if ( (ss_ptr->security == BT_SEC_AUTHENTICATE) ||
           (ss_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT) )
#endif /* BT_SWDEV_2_1_SSP */
      {
        ss_ptr->min_pin_len_bytes =
          rm_sse_ptr->cmd_msg.cmd_rm_setse.min_pin_len_bytes;

#ifdef BT_SWDEV_2_1_SSP
        //TODO - take care of legacy device
        if ( ss_ptr->security != BT_RM_SL_0_NONE )
#else
        if ( ss_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT )
#endif /* BT_SWDEV_2_1_SSP */
        {
          ss_ptr->comb_link_key_only =
            rm_sse_ptr->cmd_msg.cmd_rm_setse.comb_link_key_only;
          ss_ptr->min_enc_key_len_bytes =
            rm_sse_ptr->cmd_msg.cmd_rm_setse.min_enc_key_len_bytes;
        }
      }
      else
      {
        rm_sse_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_SERVICE_SEC;
      }
    }
  }
  else
  {
    rm_sse_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_uuid_scn_paired

DESCRIPTION
  Processes a RM UUID SCN Paired command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_uuid_scn_paired
(
  bt_sd_uuid_type   sdp_uuid,
  uint8             rfcomm_scn,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_status_type         status = BT_CS_GN_SUCCESS;
  bt_rm_uuid_scn_pair_type*  usp_ptr = NULL;

  BT_MSG_API( "BT RM CMD RX: Paired UUID %x SCN %x",
              sdp_uuid, rfcomm_scn, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: Paired", bd_addr_ptr );

  if ( (sdp_uuid != BT_SD_INVALID_UUID) &&
       (rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED) )
  {
    /*  Avoid default UUIDs.  Changes here   */
    /*  affect code in bt_rm_powerup_init()  */
    /*  and bt_cmd_rm_uuid_scn_unpaired().   */
    if ( (sdp_uuid != BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY) &&
         (sdp_uuid != BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY) &&
         (sdp_uuid != BT_SD_SERVICE_CLASS_DIALUP_NETWORKING) )
    {
      if ( (status =
              bt_rm_get_uuid_scn_pair(
                FALSE,  /*  Must be new.  */
                rfcomm_scn, bd_addr_ptr, &usp_ptr )) == BT_CS_GN_SUCCESS )
      {
        usp_ptr->sdp_uuid = sdp_uuid;

        if ( bd_addr_ptr != NULL )
        {
          usp_ptr->bd_addr       = *bd_addr_ptr;
          usp_ptr->bd_addr_valid = TRUE;
        }

        BT_MSG_HIGH( "BT RM: Paired UUID %x SCN %x",
                     usp_ptr->sdp_uuid, usp_ptr->rfcomm_scn, 0 );
        BT_BDA( MSG_HIGH, "BT RM: Paired", bd_addr_ptr );
      }
    }
    else
    {
      BT_MSG_HIGH( "BT RM: Paired Default UUID %x SCN %x",
                   sdp_uuid, rfcomm_scn, 0 );
    }
  }
  else
  {
    status = BT_CS_RM_BAD_PARAM;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_cmd_rm_uuid_scn_unpaired

DESCRIPTION
  Processes a RM UUID SCN Unpaired command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_uuid_scn_unpaired
(
  bt_sd_uuid_type   sdp_uuid,
  uint8             rfcomm_scn,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_cmd_status_type         status = BT_CS_GN_SUCCESS;
  bt_rm_uuid_scn_pair_type*  usp_ptr = NULL;

  BT_MSG_API( "BT RM CMD RX: Unpaired UUID %x SCN %x",
              sdp_uuid, rfcomm_scn, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: Unpaired", bd_addr_ptr );

  if ( (sdp_uuid != BT_SD_INVALID_UUID) &&
       (rfcomm_scn != BT_RC_SCN_NOT_SPECIFIED) )
  {
    /*  Avoid default UUIDs.  Changes here   */
    /*  affect code in bt_rm_powerup_init()  */
    /*  and bt_cmd_rm_uuid_scn_paired().     */
    if ( (sdp_uuid != BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY) &&
         (sdp_uuid != BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY) &&
         (sdp_uuid != BT_SD_SERVICE_CLASS_DIALUP_NETWORKING) )
    {
      if ( (status =
              bt_rm_get_uuid_scn_pair(
                TRUE,  /*  Must exist.  */
                rfcomm_scn, bd_addr_ptr, &usp_ptr )) == BT_CS_GN_SUCCESS )
      {
        BT_MSG_HIGH( "BT RM: Unpaired UUID %x SCN %x",
                     usp_ptr->sdp_uuid, usp_ptr->rfcomm_scn, 0 );
        BT_BDA( MSG_HIGH, "BT RM: Unpaired", bd_addr_ptr );

        bt_rm_init_uuid_scn_pair( usp_ptr );
      }
    }
    else
    {
      BT_MSG_HIGH( "BT RM: Unpaired Default UUID %x SCN %x",
                   sdp_uuid, rfcomm_scn, 0 );
    }
  }
  else
  {
    status = BT_CS_RM_BAD_PARAM;
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_rm_enforce_service_security

DESCRIPTION
  Enforces a specified service security on a specified connection.

===========================================================================*/
LOCAL void bt_rm_enforce_service_security
(
  bt_rm_conn_type*         conn_ptr,
  bt_rm_service_sec_type*  ss_ptr,
  uint8                    sec_res_rfcomm_scn,
  uint16                   sec_res_l2cap_psm,
  uint16                   sec_res_l2cap_cid,
  bt_app_id_type           sec_res_enf_app_id
)
{

  boolean  secure_enough = FALSE;
  boolean  send_ssec_result = FALSE;
  bt_ev_msg_type  pin_req_ev;

#ifdef BT_SWDEV_2_1_SSP
  if ( conn_ptr->dev_ptr->link_key_valid != FALSE )
  {
    BT_MSG_HIGH( "BT RM: SSP Link Key Exists BLK %x AUTHLK %x LKS %x",
                 conn_ptr->dev_ptr->bonded_link_key,
                 conn_ptr->dev_ptr->authenticated_link_key,
                 conn_ptr->dev_ptr->dev_public.link_key_status );
  }

  if ( ss_ptr != NULL )
  {
    BT_MSG_API( "BT RM: SSP Enforce Service Security CSL %x CSLM %x SSL %x",
                conn_ptr->sec_level,
                conn_ptr->sec_level_min,
                ss_ptr->sec_level );

    if (( IS_SSP_DOABLE() )                      &&
        ( ss_ptr->sec_level != BT_RM_SL_0_NONE ) &&
        ( conn_ptr->authorize_dir == BT_RM_ATZRQ_INCOMING ) &&
        ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_OFF ))
    {
      /* Received connection request for a service which needs
         encryption. But encryption is not enabled. Fail security */
      conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

      conn_ptr->sec_level_min = ss_ptr->sec_level;
      secure_enough = FALSE;
      send_ssec_result = TRUE;
    }
    /*  Enforce Standard Service Security.  */
    else if (((conn_ptr->sec_level < ss_ptr->sec_level) ||
              ((conn_ptr->authorized == FALSE) &&
               (ss_ptr->authorize != FALSE))) )
    {
      if ( conn_ptr->sec_level < ss_ptr->sec_level )
      {
        if (( ss_ptr->sec_level == BT_RM_SL_1_LOW ) &&
            ( FALSE == IS_SSP_DOABLE() ))
           
        {
            /* from none to low for legacy (remote or local device)*/
            /* No security for legacy devices Till BT_RM_SL_1_LOW level */
            conn_ptr->dev_ptr->bonded_link_key = TRUE;
            conn_ptr->sec_level_min = ss_ptr->sec_level;
            conn_ptr->sec_level = ss_ptr->sec_level;

            conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
            conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
            conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
            conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

            secure_enough = TRUE;
        }
        else if (conn_ptr->sec_level == BT_RM_SL_0_NONE)
        {
          /*  Security of connection must be enhanced.  */
          conn_ptr->sec_level_min = ss_ptr->sec_level;

          if ( conn_ptr->security_min_pin_len_bytes != 0 )
          {
            BT_ERR( "BT RM: Enf SSec MPL inconsistency %x %x",
                    conn_ptr->security_min_pin_len_bytes,
                    ss_ptr->min_pin_len_bytes, 0 );
          }

          if ( conn_ptr->security_min_pin_len_bytes <
                 ss_ptr->min_pin_len_bytes )
          {
            conn_ptr->security_min_pin_len_bytes = ss_ptr->min_pin_len_bytes;
          }

          if ( bt_rm_data.min_enc_key_len_bytes <
                 ss_ptr->min_enc_key_len_bytes )
          {
            if ( conn_ptr->security_min_enc_key_len_bytes != 0 )
            {
              BT_ERR( "BT RM: Enf SSec MEKL inconsistency %x %x",
                      conn_ptr->security_min_enc_key_len_bytes,
                      ss_ptr->min_enc_key_len_bytes, 0 );
            }

            if ( conn_ptr->security_min_enc_key_len_bytes <
                   ss_ptr->min_enc_key_len_bytes )
            {
              conn_ptr->security_min_enc_key_len_bytes =
                ss_ptr->min_enc_key_len_bytes;

              bt_rm_data.min_enc_key_len_bytes = ss_ptr->min_enc_key_len_bytes;

              BT_MSG_API( "BT RM CMD TX: HC WrEncKeyLen Min %x Max %x",
                          bt_rm_data.min_enc_key_len_bytes,
                          BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES, 0 );
#ifndef FEATURE_BT_SOC
              bt_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                                        BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#else
              bt_soc_etc_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                                         BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#endif /* !FEATURE_BT_SOC */
            }
          }
          if (( conn_ptr->dev_ptr->link_key_valid != FALSE ) &&
              ( ss_ptr->sec_level == BT_RM_SL_3_HIGH ))
          {
            conn_ptr->dev_ptr->bonded_link_key = TRUE;
            /* Changing to HIGH */
            if (( IS_SSP_DOABLE() ) &&
                ( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_GENERAL ) &&
                ( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_DEDICATED ) &&
                ( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_UNBONDED ))
            {
              /* We have an unautheticated link key, but the
                 service requires am authenticated link key */
              conn_ptr->update_link_key_request = TRUE;
            }
          }
        }
        else if ( ss_ptr->sec_level == BT_RM_SL_3_HIGH )
        {
          /* Changing from LOW to HIGH or MEDIUM to HIGH */
          if (( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_GENERAL ) &&
              ( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_DEDICATED ) &&
              ( conn_ptr->dev_ptr->dev_public.link_key_status != BT_RM_LKT_AUTH_UNBONDED ))
          {
            conn_ptr->sec_level_min = ss_ptr->sec_level;
            if ((conn_ptr->encrypt_enable == TRUE) ||
                (IS_SSP_DOABLE()))
            {
              if ( conn_ptr->sec_level == BT_RM_SL_1_LOW )
              {
                conn_ptr->dev_ptr->bonded_link_key = TRUE;
                conn_ptr->sec_level = BT_RM_SL_2_MEDIUM;
              }
              /* We have an unautheticated link key, but the
                 service requires am authenticated link key */
              conn_ptr->update_link_key_request = TRUE;
            }
            secure_enough = FALSE;
          }
          else
          {
            /* Link key is already authenticated */
            conn_ptr->dev_ptr->bonded_link_key = TRUE;
            conn_ptr->sec_level_min = ss_ptr->sec_level;
            conn_ptr->sec_level = ss_ptr->sec_level;

            conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
            conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
            conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
            conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

            secure_enough = TRUE;
          }
        }
        else
        {
          /* Changing from LOW to MEDIUM */
          conn_ptr->sec_level_min = BT_RM_SL_2_MEDIUM;
          /* We have an non-bonded link key, but the
             service requires a bonded link key */
          conn_ptr->update_link_key_request = TRUE;
          secure_enough = FALSE;
        }
      }
      if ((conn_ptr->authorized == FALSE) &&
          (ss_ptr->authorize != FALSE))
      {
        /*  Connection must be Authorized.  */
        conn_ptr->authorization_required = TRUE;
        conn_ptr->authorize_first        = ss_ptr->authorize_first;
        conn_ptr->authorize_req_app_id   = ss_ptr->bt_app_id;
        secure_enough                    = FALSE;
      }

      if ( secure_enough != TRUE )
      {
        conn_ptr->sec_res_rfcomm_scn    = sec_res_rfcomm_scn;
        conn_ptr->sec_res_l2cap_psm     = sec_res_l2cap_psm;
        conn_ptr->sec_res_l2cap_cid     = sec_res_l2cap_cid;
        conn_ptr->sec_res_enf_app_id    = sec_res_enf_app_id;
        conn_ptr->sec_res_ss_app_id     = ss_ptr->bt_app_id;
        conn_ptr->sec_res_ss_service_id = ss_ptr->service_id;
        conn_ptr->bonding_ss_ptr        = ss_ptr;

        if ( (secure_enough =
                bt_rm_connection_secure_enough(
                  conn_ptr, TRUE )) == FALSE )
        {
          /*  Process to increase security has begun.  */
          /*  The end of that process will send the    */
          /*  service security result.                 */
          send_ssec_result = FALSE;
        }
        else
        {
          BT_ERR( "BT RM: Enf SSec inconsistency SE %x",
                  secure_enough, 0, 0 );
        }
      }
      else
      {
        send_ssec_result = TRUE;
      }
    }
    /*
     * Enforce Extended Service Security. for the only case when
     * sec upgrade is involving legacy device
     */
    else if ( (!(IS_SSP_DOABLE())) &&
              ((ss_ptr->sec_level != BT_RM_SL_0_NONE) &&
               (conn_ptr->dev_ptr->link_key_pin_len_bytes <
                  ss_ptr->min_pin_len_bytes)) ||
              ((ss_ptr->sec_level != BT_RM_SL_0_NONE) &&
               (conn_ptr->enc_key_len_bytes <
                   ss_ptr->min_enc_key_len_bytes)) )
    {
      /*  One of the following Extended Service Security requirements    */
      /*  has not been met:                                              */
      /*                                                                 */
      /*    1) The length of PIN used to authenticate or encrypt this    */
      /*         connection was less than the required minimum length.   */
      /*                                                                 */
      /*    2) The encryption key type required for this link is a       */
      /*         combination link key and that is not the link key       */
      /*         type currently in use.                                  */
      /*                                                                 */
      /*    3) The encryption key length in use less than the required   */
      /*         minimum length.                                         */
      /*                                                                 */
      /*  It is very difficult at this point to attempt to upgrade       */
      /*  the security on this connection to meet these extended         */
      /*  service security requirements.  Any such changes requires      */
      /*  data transfer to stop between the devices, stop encryption,    */
      /*  re-authenticate and/or encrypt and enforce the minimum         */
      /*  settings during the process, and upon success of this begin    */
      /*  data transfer between the devices again and then output the    */
      /*  successful Service Security Result Event.                      */
      /*                                                                 */
      /*  Recovery from the process above failing is problematic as it   */
      /*  is difficult to restore the previous link security...  link    */
      /*  keys may be lost, PINs may have to be re-entered, assumptions  */
      /*  about the remote device's reactions would have to be made and  */
      /*  it would have to cooperate pretty well to get through the      */
      /*  whole process... a highly unlikely scenario.                   */
      /*                                                                 */
      /*  Because of the above, we are implementing only case 1 as its   */
      /*  required for SAP profile certification                         */

      conn_ptr->sec_res_rfcomm_scn    = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm     = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid     = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id    = sec_res_enf_app_id;
      conn_ptr->sec_res_ss_app_id     = ss_ptr->bt_app_id;
      conn_ptr->sec_res_ss_service_id = ss_ptr->service_id;

      secure_enough    = FALSE;
      send_ssec_result = TRUE;

      if (conn_ptr->dev_ptr->link_key_pin_len_bytes <
          ss_ptr->min_pin_len_bytes)
      {
        send_ssec_result = FALSE;
        /* Case 1
         * PIN length used currentlly is not enough for this connection security
         * Need to do re-pairing for this connection. It has two cases.
         * 1.1 Connection encryption is on.
         *    Prefetch the PIN , then stop encryption , then start pairing
         *    process and resume the encryption.
         * 1.2 Connection encryption is off. - Not applicable to SSP/SM4
         *    This is a easy case just needs to start pairing.
         *
         */
        conn_ptr->security_min_pin_len_bytes = ss_ptr->min_pin_len_bytes;
        conn_ptr->sec_level_min = ss_ptr->sec_level;
        /* Case 1.1 */
        BT_MSG_DEBUG("Requesting PIN for re-pairing",0,0,0);
            conn_ptr->re_pair_pin_state = BT_RM_RE_PAIR_PIN_WAITING;
            pin_req_ev.ev_msg.ev_hc_pinrq.bd_addr =
                            conn_ptr->dev_ptr->dev_public.bd_addr;
            bt_rm_ev_hc_pin_code_request( &pin_req_ev);

        /* - pre-merge
        BT_MSG_DEBUG("Disabling encryption enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
        conn_ptr->enh_enc_state = BT_RM_ENH_ENC_DISABLING;
        bt_cmd_hc_set_conn_encrypt( conn_ptr->hc_handle_acl,
                                    BT_ENCRYPT_ENABLE_OFF );
        */
      }

      if ( conn_ptr->security_min_enc_key_len_bytes <
             ss_ptr->min_enc_key_len_bytes )
      {
        conn_ptr->security_min_enc_key_len_bytes =
          ss_ptr->min_enc_key_len_bytes;
      }
    }
    else
    {
      /*  The connection meets or exceeds the requirements  */
      /*  of both Standard and Extended Service Security.   */

      BT_MSG_HIGH("BT RM: SSP Conn meets service requirements.",0,0,0);
      conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

      secure_enough    = TRUE;
      send_ssec_result = TRUE;
    }
  }
  else
  {
    /*  No Service Security requirements for this connection.  */
    conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
    conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
    conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
    conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

    BT_MSG_HIGH("BT RM: SSP No security settings for service. Using default.",0,0,0);


    if (( IS_SSP_DOABLE() )                      &&
        ( conn_ptr->authorize_dir == BT_RM_ATZRQ_INCOMING )   &&
        ( conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_OFF ) &&
        ( conn_ptr->sec_res_l2cap_psm != BT_L2_PSM_INVALID )  && 
        ( conn_ptr->sec_res_l2cap_psm != BT_L2_PSM_SDP ) )
    {
      /* Received connection request for a service which needs
         encryption. But encryption is not enabled.
         Fail security; upper layer should disconnect */
      conn_ptr->sec_level_min = BT_RM_SL_1_LOW;
      send_ssec_result = TRUE;
      secure_enough = FALSE;
    }
    else if ( conn_ptr->sec_level == BT_RM_SL_0_NONE )
    {
      conn_ptr->sec_level_min = BT_RM_SL_1_LOW;
      conn_ptr->bonding_ss_ptr = ss_ptr;
      if ( (secure_enough =
              bt_rm_connection_secure_enough(
                conn_ptr, TRUE )) == FALSE )
      {
        /*  Process to increase security has begun.  */
        /*  The end of that process will send the    */
        /*  service security result.                 */
        send_ssec_result = FALSE;
      }
      else
      {
        send_ssec_result = TRUE;
      }
    }
    else
    {
      secure_enough    = TRUE;
      send_ssec_result = TRUE;
    }
  }

  if ( send_ssec_result != FALSE )
  {
    bt_rm_ev_send_security_result( conn_ptr );
  }
#else
  /* pre 2.1 code */
  if ( ss_ptr != NULL )
  {
    /*  Enforce Standard Service Security.  */
    if (((conn_ptr->security < ss_ptr->security) ||
        ((conn_ptr->authorized == FALSE) &&
         (ss_ptr->authorize != FALSE))) )
    {
      if ( conn_ptr->security_min < ss_ptr->security )
      {
        /*  Security of connection must be enhanced.  */
        conn_ptr->security_min = ss_ptr->security;

        if ( conn_ptr->security_min_pin_len_bytes != 0 )
        {
          BT_ERR( "BT RM: Enf SSec MPL inconsistency %x %x",
                  conn_ptr->security_min_pin_len_bytes,
                  ss_ptr->min_pin_len_bytes, 0 );
        }

        if ( conn_ptr->security_min_pin_len_bytes <
               ss_ptr->min_pin_len_bytes )
        {
          conn_ptr->security_min_pin_len_bytes = ss_ptr->min_pin_len_bytes;
        }

        if ( ss_ptr->comb_link_key_only != FALSE )
        {
          conn_ptr->security_comb_key_only = TRUE;
        }

        if ( bt_rm_data.min_enc_key_len_bytes <
               ss_ptr->min_enc_key_len_bytes )
        {
          if ( conn_ptr->security_min_enc_key_len_bytes != 0 )
          {
            BT_ERR( "BT RM: Enf SSec MEKL inconsistency %x %x",
                    conn_ptr->security_min_enc_key_len_bytes,
                    ss_ptr->min_enc_key_len_bytes, 0 );
          }

          if ( conn_ptr->security_min_enc_key_len_bytes <
                 ss_ptr->min_enc_key_len_bytes )
          {
            conn_ptr->security_min_enc_key_len_bytes =
              ss_ptr->min_enc_key_len_bytes;

            bt_rm_data.min_enc_key_len_bytes = ss_ptr->min_enc_key_len_bytes;

            BT_MSG_API( "BT RM CMD TX: HC WrEncKeyLen Min %x Max %x",
                        bt_rm_data.min_enc_key_len_bytes,
                        BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES, 0 );
#ifndef FEATURE_BT_SOC
            bt_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                                      BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#else
            bt_soc_etc_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                                       BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#endif /* !FEATURE_BT_SOC */
          }
        }
      }

      if ( ss_ptr->authorize != FALSE )
      {
        /*  Connection must be Authorized.  */
        conn_ptr->authorization_required = TRUE;
        conn_ptr->authorize_first        = ss_ptr->authorize_first;
        conn_ptr->authorize_req_app_id   = ss_ptr->bt_app_id;
      }

      conn_ptr->sec_res_rfcomm_scn    = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm     = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid     = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id    = sec_res_enf_app_id;
      conn_ptr->sec_res_ss_app_id     = ss_ptr->bt_app_id;
      conn_ptr->sec_res_ss_service_id = ss_ptr->service_id;

      if ( (secure_enough =
              bt_rm_connection_secure_enough(
                conn_ptr, TRUE )) == FALSE )
      {
        /*  Process to increase security has begun.  */
        /*  The end of that process will send the    */
        /*  service security result.                 */
        send_ssec_result = FALSE;
      }
      else
      {
        BT_ERR( "BT RM: Enf SSec inconsistency SE %x",
                secure_enough, 0, 0 );
      }
    }
    /*  Enforce Extended Service Security.  */
    else if ( (((ss_ptr->security == BT_SEC_AUTHENTICATE) ||
                (ss_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT)) &&
               (conn_ptr->dev_ptr->link_key_pin_len_bytes <
                  ss_ptr->min_pin_len_bytes)) ||
              ((ss_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT) &&
               (((ss_ptr->comb_link_key_only != FALSE) &&
                 (conn_ptr->dev_ptr->dev_public.link_key_type !=
                    BT_HC_LINK_KEY_COMB)) ||
                (conn_ptr->enc_key_len_bytes <
                   ss_ptr->min_enc_key_len_bytes))) )
    {
      /*  One of the following Extended Service Security requirements    */
      /*  has not been met:                                              */
      /*                                                                 */
      /*    1) The length of PIN used to authenticate or encrypt this    */
      /*         connection was less than the required minimum length.   */
      /*                                                                 */
      /*    2) The encryption key type required for this link is a       */
      /*         combination link key and that is not the link key       */
      /*         type currently in use.                                  */
      /*                                                                 */
      /*    3) The encryption key length in use less than the required   */
      /*         minimum length.                                         */
      /*                                                                 */
      /*  It is very difficult at this point to attempt to upgrade       */
      /*  the security on this connection to meet these extended         */
      /*  service security requirements.  Any such changes requires      */
      /*  data transfer to stop between the devices, stop encryption,    */
      /*  re-authenticate and/or encrypt and enforce the minimum         */
      /*  settings during the process, and upon success of this begin    */
      /*  data transfer between the devices again and then output the    */
      /*  successful Service Security Result Event.                      */
      /*                                                                 */
      /*  Recovery from the process above failing is problematic as it   */
      /*  is difficult to restore the previous link security...  link    */
      /*  keys may be lost, PINs may have to be re-entered, assumptions  */
      /*  about the remote device's reactions would have to be made and  */
      /*  it would have to cooperate pretty well to get through the      */
      /*  whole process... a highly unlikely scenario.                   */
      /*                                                                 */
      /*  Because of the above, we are implementing only case 1 as its   */
      /*  required for SAP profile certification                         */

      conn_ptr->sec_res_rfcomm_scn    = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm     = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid     = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id    = sec_res_enf_app_id;
      conn_ptr->sec_res_ss_app_id     = ss_ptr->bt_app_id;
      conn_ptr->sec_res_ss_service_id = ss_ptr->service_id;

      secure_enough    = FALSE;
      send_ssec_result = TRUE;

      if (((ss_ptr->security == BT_SEC_AUTHENTICATE) ||
                (ss_ptr->security == BT_SEC_AUTHENTICATE_AND_ENCRYPT)) &&
               (conn_ptr->dev_ptr->link_key_pin_len_bytes <
                  ss_ptr->min_pin_len_bytes))
      {
        send_ssec_result = FALSE;
        /* Case 1
         * PIN length used currentlly is not enough for this connection security
         * Need to do re-pairing for this connection. It has two cases.
        * 1.1 Connection encryption is on.
         *    Prefetch the PIN , then stop encryption , then start pairing
         *    process and resume the encryption.
        * 1.2 Connection encryption is off.
        *    This is a easy case just needs to start pairing.
        *
        */
        conn_ptr->security_min_pin_len_bytes = ss_ptr->min_pin_len_bytes;
        if(conn_ptr->encrypt_enable == BT_ENCRYPT_ENABLE_ON)
        {
            /* Case 1.1 */
            BT_MSG_DEBUG("Requesting PIN for re-pairing",0,0,0);
            conn_ptr->re_pair_pin_state = BT_RM_RE_PAIR_PIN_WAITING;
            pin_req_ev.ev_msg.ev_hc_pinrq.bd_addr =
                            conn_ptr->dev_ptr->dev_public.bd_addr;
            bt_rm_ev_hc_pin_code_request( &pin_req_ev);
        }
        else
        {
            /* Case 1.2 */
            BT_MSG_DEBUG("Starting re-pairing enh_enc_state %x",conn_ptr->enh_enc_state,0,0);
            conn_ptr->enh_enc_state = BT_RM_ENH_ENC_AUTHENTICATING_ONLY;
            bt_cmd_hc_authentication_requested( conn_ptr->hc_handle_acl );
        }
      }

      if ( ss_ptr->comb_link_key_only != FALSE )
      {
        conn_ptr->security_comb_key_only = TRUE;
      }

      if ( conn_ptr->security_min_enc_key_len_bytes <
             ss_ptr->min_enc_key_len_bytes )
      {
        conn_ptr->security_min_enc_key_len_bytes =
          ss_ptr->min_enc_key_len_bytes;
      }

    }
    else
    {
      /*  The connection meets or exceeds the requirements  */
      /*  of both Standard and Extended Service Security.   */

      conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
      conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
      conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
      conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

      secure_enough    = TRUE;
      send_ssec_result = TRUE;
    }
  }
  else
  {
    /*  No Service Security requirements for this connection.  */

    conn_ptr->sec_res_rfcomm_scn = sec_res_rfcomm_scn;
    conn_ptr->sec_res_l2cap_psm  = sec_res_l2cap_psm;
    conn_ptr->sec_res_l2cap_cid  = sec_res_l2cap_cid;
    conn_ptr->sec_res_enf_app_id = sec_res_enf_app_id;

    secure_enough    = TRUE;
    send_ssec_result = TRUE;
  }

  if ( send_ssec_result != FALSE )
  {
    bt_rm_ev_send_security_result( conn_ptr );
  }
#endif /* BT_SWDEV_2_1_SSP */
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_enforce_security_rfcomm_scn

DESCRIPTION
  Processes a RM Enforce Security RFCOMM SCN command.

===========================================================================*/
LOCAL void bt_rm_cmd_enforce_security_rfcomm_scn
(
  bt_cmd_msg_type*  rm_esr_ptr
)
{

  bt_rm_conn_type*           conn_ptr;
  bt_service_id_type         service_id;
  bt_rm_service_sec_type*    ss_ptr = NULL;
  bt_rm_uuid_scn_pair_type*  usp_ptr;
  bt_bd_addr_type*           bd_addr_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Enf Sec SCN %x LC %x",
              rm_esr_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn,
              rm_esr_ptr->cmd_msg.cmd_rm_esscn.local_client, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Enf Sec SCN",
          &rm_esr_ptr->cmd_msg.cmd_rm_esscn.bd_addr );

  if ( (rm_esr_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,  /*  Connection must exist.       */
            TRUE,  /*  Connection must be settled.  */
            &rm_esr_ptr->cmd_msg.cmd_rm_esscn.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    /*  Search for service security requirements.       */
    /*  In priority order of:                           */
    /*    1) If local server (i.e., not local client),  */
    /*         requests identified by this RFCOMM SCN   */
    /*    2) a) If local server, requests identified    */
    /*            by the SDP UUID paired with this      */
    /*            RFCOMM SCN without a BD address       */
    /*       b) If local client, requests identified    */
    /*            by the SDP UUID paired with this      */
    /*            RFCOMM SCN and the BD address passed  */
    /*    3) Default for all RFCOMM SCNs.               */

    rm_esr_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_SERVICE_SEC;

    if ( rm_esr_ptr->cmd_msg.cmd_rm_esscn.local_client == FALSE )
    {
      /*  Local server; case (1) above.  */

      service_id.service_id_method = BT_SIM_RFCOMM_SCN;
      service_id.rfcomm_scn = rm_esr_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn;

      rm_esr_ptr->cmd_hdr.cmd_status =
        bt_rm_get_service_security( FALSE,  /*  Must exist.  */
                                    BT_APP_ID_NULL,
                                    &service_id, &ss_ptr );
    }

    if ( rm_esr_ptr->cmd_hdr.cmd_status == BT_CS_RM_BAD_SERVICE_SEC )
    {
      /*  No RFCOMM SCN specified security applies.  */
      /*  Search for UUID paired with RFCOMM SCN /   */
      /*  BD address as applicable; case (2) above.  */

      if ( rm_esr_ptr->cmd_msg.cmd_rm_esscn.local_client == FALSE )
      {
        /*  Local server; do not use BD address; case (2a) above.  */
        bd_addr_ptr = NULL;
      }
      else
      {
        /*  Local client; use BD address; case (2b) above.  */
        bd_addr_ptr = &rm_esr_ptr->cmd_msg.cmd_rm_esscn.bd_addr;
      }

      if ( (rm_esr_ptr->cmd_hdr.cmd_status =
              bt_rm_get_uuid_scn_pair(
                TRUE,  /*  Must exist.  */
                rm_esr_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn,
                bd_addr_ptr, &usp_ptr )) == BT_CS_GN_SUCCESS )
      {
        /*  Found UUID SCN pair.  Search for    */
        /*  security specified by paired UUID.  */
        service_id.service_id_method = BT_SIM_SDP_UUID;
        service_id.sdp_uuid          = usp_ptr->sdp_uuid;

        if ( (rm_esr_ptr->cmd_hdr.cmd_status =
                bt_rm_get_service_security(
                  FALSE,  /*  Must exist.  */
                  BT_APP_ID_NULL,
                  &service_id,
                  &ss_ptr )) == BT_CS_RM_BAD_SERVICE_SEC )
        {
          /*  No UUID specified security found.  */
          rm_esr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
        }
      }
      else if ( rm_esr_ptr->cmd_hdr.cmd_status ==
                  BT_CS_RM_BAD_UUID_SCN_PAIR )
      {
        /*  No UUID SCN pair found.  */
        rm_esr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
      }

      if ( (rm_esr_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS) &&
           (ss_ptr == NULL) )
      {
        /*  Search for default for all RFCOMM SCNs.  */
        service_id.service_id_method = BT_SIM_DEFAULT_RFCOMM;

        if ( (rm_esr_ptr->cmd_hdr.cmd_status =
                bt_rm_get_service_security(
                  FALSE,  /*  Must exist.  */
                  BT_APP_ID_NULL,
                  &service_id,
                  &ss_ptr )) == BT_CS_RM_BAD_SERVICE_SEC )
        {
          rm_esr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
          ss_ptr = NULL;
        }
      }
    }

    if ( rm_esr_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
    {
      /* update enforcing of security is because of incoming RFCOMM
         connection or outgoing RFCOMM connection */
      if ( rm_esr_ptr->cmd_msg.cmd_rm_esscn.local_client == FALSE )
      {
          conn_ptr->authorize_dir = BT_RM_ATZRQ_INCOMING;
      }
      else
      {
          conn_ptr->authorize_dir = BT_RM_ATZRQ_OUTGOING;
      }

      bt_rm_enforce_service_security(
        conn_ptr, ss_ptr,
        rm_esr_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn,
        BT_L2_PSM_INVALID, BT_L2_NULL_CID,
        rm_esr_ptr->cmd_hdr.bt_app_id );
    }
  }

  if ( ( rm_esr_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER ) && 
       ( BT_CHECK_RETRY_EXPIRED( rm_esr_ptr->cmd_hdr.cmd_retries )  ) &&
       ( conn_ptr != NULL) )
  {
      /* Command retry have reached max limit send security 
         complete event with failure */
      BT_ERR( "BT RM CMD RX: RM Enf Sec max re-tries reached",0,0,0);
      /*  Send RFCOMM SCN Security Result event.  */
      bt_rm_ev_send_rfcomm_scn_sec_result( 
                                   rm_esr_ptr->cmd_hdr.bt_app_id,
                                   rm_esr_ptr->cmd_msg.cmd_rm_esscn.rfcomm_scn,
                                   &conn_ptr->dev_ptr->dev_public.bd_addr,
                                   FALSE );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_enforce_security_l2cap_psm

DESCRIPTION
  Processes a RM Enforce Security L2CAP PSM command.

===========================================================================*/
LOCAL void bt_rm_cmd_enforce_security_l2cap_psm
(
  bt_cmd_msg_type*  rm_esl_ptr
)
{

  bt_rm_conn_type*         conn_ptr;
  bt_service_id_type       service_id;
  bt_rm_service_sec_type*  ss_ptr = NULL;

  BT_MSG_API( "BT RM CMD RX: RM Enf Sec PSM %x CID %x",
              rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_psm,
              rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_cid, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM Enf Sec PSM",
          &rm_esl_ptr->cmd_msg.cmd_rm_espsm.bd_addr );

  if ( (rm_esl_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,  /*  Connection must exist.       */
            TRUE,  /*  Connection must be settled.  */
            &rm_esl_ptr->cmd_msg.cmd_rm_espsm.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    /*  Search for service security requirements.   */
    /*  In priority order of:                       */
    /*    1) Requests identified by this L2CAP PSM  */
    /*    2) Default for all L2CAP PSMs.            */

    service_id.service_id_method = BT_SIM_L2CAP_PSM;
    service_id.l2cap_psm = rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_psm;

    if ( (rm_esl_ptr->cmd_hdr.cmd_status =
            bt_rm_get_service_security(
              FALSE,  /*  Must exist.  */
              BT_APP_ID_NULL,
              &service_id,
              &ss_ptr )) == BT_CS_RM_BAD_SERVICE_SEC )
    {
      /*  No L2CAP PSM specified security found.  */
      /*  Search for default for all L2CAP PSMs.  */

      service_id.service_id_method = BT_SIM_DEFAULT_L2CAP;

      if ( (rm_esl_ptr->cmd_hdr.cmd_status =
              bt_rm_get_service_security(
                FALSE,  /*  Must exist.  */
                BT_APP_ID_NULL,
                &service_id,
                &ss_ptr )) == BT_CS_RM_BAD_SERVICE_SEC )
      {
        rm_esl_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
        ss_ptr = NULL;
      }
    }

    if ( rm_esl_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
    {
      if( conn_ptr->sec_res_l2cap_psm == BT_L2_PSM_INVALID )
      {
        /* enforcing of security is because of incoming L2CAP
             connection or outgoing L2CAP connection */
        if( (rm_esl_ptr->cmd_msg.cmd_rm_espsm.conn_dir == BT_RM_ATZRQ_INCOMING) ||
            (rm_esl_ptr->cmd_msg.cmd_rm_espsm.conn_dir == BT_RM_ATZRQ_OUTGOING) )
        {
            conn_ptr->authorize_dir = rm_esl_ptr->cmd_msg.cmd_rm_espsm.conn_dir;
        }
        else
        {
            BT_ERR("BT RM: Authorize direction is not correct",0,0,0);
        }
        bt_rm_enforce_service_security(
          conn_ptr, ss_ptr, BT_RC_SCN_NOT_SPECIFIED,
          rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_psm,
          rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_cid,
          rm_esl_ptr->cmd_hdr.bt_app_id );
      }
      else
      {
        /* Currently authorizing a link. Try again later */
        BT_MSG_DEBUG("BT RM Enf Sec: processing PSM %x CID %x; holding CID %x",
                     conn_ptr->sec_res_l2cap_psm,
                     conn_ptr->sec_res_l2cap_cid,
                     rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_cid );
        rm_esl_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
  }

  if ( ( rm_esl_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER ) && 
       ( BT_CHECK_RETRY_EXPIRED( rm_esl_ptr->cmd_hdr.cmd_retries )  ) )
  {
      /* Command retry have reached max limit send security 
         complete event with failure */
      BT_ERR( "BT RM CMD RX: RM Enf Sec PSM max re-tries reached",0,0,0);
      /*  Send L2CAP PSM Security Result event.  */
      bt_rm_ev_send_l2cap_psm_sec_result(
        rm_esl_ptr->cmd_hdr.bt_app_id,
        rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_psm,
        rm_esl_ptr->cmd_msg.cmd_rm_espsm.l2cap_cid,
        &rm_esl_ptr->cmd_msg.cmd_rm_espsm.bd_addr, 
        FALSE );
  }


}


/*===========================================================================

FUNCTION
  bt_cmd_rm_device_read_iter

DESCRIPTION
  Processes a RM Device Read Iter(atively) command.

===========================================================================*/
bt_cmd_status_type bt_cmd_rm_device_read_iter
(
  bt_app_id_type            bt_app_id,
  bt_rm_dev_iterator_type*  di_ptr,
  bt_device_type*           dest_dev_ptr,
  uint8*                    num_services_ptr,
  bt_service_type*          dest_services_ptr
)
{

  bt_cmd_status_type  status = BT_CS_RM_BAD_PARAM;
  bt_rm_device_type*  dev_ptr = NULL;
  uint8               i;
  bt_bd_addr_type     masked_bd_addr;
  boolean             inc_vol = FALSE;


  if( ( dest_dev_ptr != NULL ) && ( di_ptr != NULL ) )
  {
    BT_MSG_API( "BT RM CMD RX: RM DevRdI R %x I %x AID %x",
                  di_ptr->reset, di_ptr->index, bt_app_id );

    dest_dev_ptr->valid = FALSE;

    if ( (di_ptr->control & BT_RM_IC_INCLUDE_VOLATILE_B) != 0 )
    {
      inc_vol = TRUE;
    }

    di_ptr->control &= BT_RM_IC_VALUE_M;

    if ( (di_ptr != NULL ) &&
         (di_ptr->control < BT_RM_IC_MAX) &&
         ((di_ptr->reset != FALSE) ||
          ((di_ptr->state & BT_RM_DRIS_RESET_SIG_M) ==
              BT_RM_DRIS_RESET_SIG_V)) )
    {
      TASKLOCK();

      if ( di_ptr->reset != FALSE )
      {
        di_ptr->index              = 0;
        di_ptr->state              = BT_RM_DRIS_RESET_SIG_V;
        di_ptr->state             |= bt_rm_dev_db_update_count;
        di_ptr->device_db_updated  = FALSE;
        di_ptr->reset              = FALSE;
      }
      else
      {
        if ( di_ptr->index < BT_RM_MAX_DEVICES )
        {
          di_ptr->index++;
        }

        if ( (di_ptr->state & BT_RM_DRIS_UPDATE_COUNT_M) !=
                bt_rm_dev_db_update_count )
        {
          di_ptr->device_db_updated = TRUE;
        }
      }

      if ( di_ptr->control == BT_RM_IC_MATCH_BD_ADDR_MASK )
      {
        for ( i = 0; i < sizeof( bt_bd_addr_type ); i++ )
        {
          masked_bd_addr.bd_addr_bytes[ i ] =
            (di_ptr->bd_addr_value.bd_addr_bytes[ i ] &
               di_ptr->bd_addr_mask.bd_addr_bytes[ i ]);
        }
      }

      status = BT_CS_GN_SUCCESS;

      while ( (di_ptr->index < BT_RM_MAX_DEVICES) && (dev_ptr == NULL) )
      {
        if ( (inc_vol != FALSE) ||
             (bt_rm_device[ di_ptr->index ].dev_public.valid != FALSE) )
        {
          if ( di_ptr->control == BT_RM_IC_ALL )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_SERVICE_CLASS) &&
                    ((bt_rm_device[ di_ptr->index ].dev_public.
                        service_class & di_ptr->service_class) ==
                          di_ptr->service_class) )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_BONDED) &&
                    (bt_rm_device[ di_ptr->index ].dev_public.bonded ==
                       di_ptr->bonded) )

          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( di_ptr->control == BT_RM_IC_MATCH_BD_ADDR_MASK )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
            for ( i = 0; i < sizeof( bt_bd_addr_type ); i++ )
            {
              if ( masked_bd_addr.bd_addr_bytes[ i ] !=
                     (bt_rm_device[ di_ptr->index ].dev_public.
                        bd_addr.bd_addr_bytes[ i ] &
                          di_ptr->bd_addr_mask.bd_addr_bytes[ i ]) )
              {
                dev_ptr = NULL;
                break;
              }
            }
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_VALUE_1) &&
                    (bt_rm_device[ di_ptr->index ].dev_public.value_1 ==
                       di_ptr->value_1) )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_VALUE_2) &&
                    (bt_rm_device[ di_ptr->index ].dev_public.value_2 ==
                       di_ptr->value_2) )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_VALUE_3) &&
                    (bt_rm_device[ di_ptr->index ].dev_public.value_3 ==
                       di_ptr->value_3) )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
          else if ( (di_ptr->control == BT_RM_IC_MATCH_VALUE_4) &&
                    (bt_rm_device[ di_ptr->index ].dev_public.value_4 ==
                       di_ptr->value_4) )
          {
            dev_ptr = &bt_rm_device[ di_ptr->index ];
          }
        }
        if ( dev_ptr == NULL )
        {
          di_ptr->index++;
        }
      }

      if ( dev_ptr != NULL )
      {
        bt_rm_copy_device( dev_ptr, dest_dev_ptr,
                           num_services_ptr, dest_services_ptr );
      }

      TASKFREE();
    }
    else
    {
      status = BT_CS_RM_BAD_PARAM;
    }
  }
  else
  {
    BT_MSG_API( "BT RM CMD RX: RM DevRdI Bad Param AID %x",bt_app_id,0,0 );
    status = BT_CS_RM_BAD_PARAM;
  }

  return ( status );

}

/*===========================================================================

FUNCTION
  bt_rm_save_dev_info

DESCRIPTION
  Helper function for copying device data.

===========================================================================*/
LOCAL void bt_rm_copy_dev_info
(
  bt_rm_device_type* to_ptr,
  bt_rm_device_type* from_ptr
)
{
  to_ptr->dev_public.bd_addr         = from_ptr->dev_public.bd_addr;
  to_ptr->references                 = from_ptr->references;
  to_ptr->page_scan_mode             = from_ptr->page_scan_mode;
  to_ptr->page_scan_rep_mode         = from_ptr->page_scan_rep_mode;
  to_ptr->page_scan_per_mode         = from_ptr->page_scan_per_mode;
  to_ptr->clk_offset                 = from_ptr->clk_offset;
  to_ptr->dev_public.class_of_device = from_ptr->dev_public.class_of_device;
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_device_remove

DESCRIPTION
  Processes the RM Device Remove command.

===========================================================================*/
LOCAL void bt_rm_cmd_device_remove
(
  bt_cmd_msg_type*  rm_dr_ptr
)
{

  boolean             success;
  bt_rm_device_type*  dev_ptr;
  bt_rm_device_type   temp_device;

  bt_bd_addr_type     bd_addr;

  BT_MSG_API( "BT RM CMD RX: DevRmv AID %x",
              rm_dr_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: DevRmv",
          &rm_dr_ptr->cmd_msg.cmd_rm_dvrmv.bd_addr );

  rm_dr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( ((dev_ptr = bt_rm_get_device(
                     &rm_dr_ptr->cmd_msg.cmd_rm_dvrmv.bd_addr,
                     FALSE )) != NULL) &&
       (dev_ptr->dev_public.valid != FALSE) )
  {

    /* Disconnect if the device is connected */
    bt_rm_disconnect_device ( dev_ptr );

    /* Save device address info */
    bd_addr = dev_ptr->dev_public.bd_addr;

    /*  Remove all information from the device database in   */
    /*  both non-volatile memory and RAM copy except for     */
    /*  a few (only if there are any references)             */
    bt_rm_copy_dev_info ( &temp_device, dev_ptr );

    bt_rm_init_device( dev_ptr );

    if ( temp_device.references )
    {
      bt_rm_copy_dev_info ( dev_ptr, &temp_device );
    }

    dev_ptr->update_status = BT_RM_DUS_SERVICES_B;
    success = bt_rm_write_device_file();
    dev_ptr->update_status = BT_RM_DUS_NONE;

    if ( success != FALSE )
    {
      dev_ptr->update_status = BT_RM_DUS_REMOVED_B;

      bt_rm_ev_notify_device_updated( rm_dr_ptr->cmd_hdr.bt_app_id,
                                      &bd_addr, dev_ptr, success );
    }
    else
    {
      rm_dr_ptr->cmd_hdr.cmd_status = BT_CS_RM_EFS_ERR;
    }
  }
  else
  {
    rm_dr_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_DEVICE;
  }

}


#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION
  bt_rm_remove_dev_and_serv_files

DESCRIPTION
  Remove the Device and Service files from the flash file system.

============================================================================*/
LOCAL void bt_rm_remove_dev_and_serv_files
(
  void
)
{

  fs_rsp_msg_type  fs_rsp_msg;
  uint8            i;

  BT_MSG_HIGH( "BT RM: Removing DEV/SERV files", 0, 0, 0 );

  fs_remove( BT_RM_DEV_DB_NAME, NULL, &fs_rsp_msg );

  if ( (fs_rsp_msg.any.status != FS_OKAY_S) &&
       (fs_rsp_msg.any.status != FS_NONEXISTENT_FILE_S) )
  {
    BT_ERR( "BT RM: Remove DEV file S %x ",
            fs_rsp_msg.any.status, 0, 0 );
  }

  /*  Remove service files. */

  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    bt_rm_set_service_file_name( &bt_rm_device[ i ] );

    fs_remove( service_file_name, NULL, &fs_rsp_msg );

    if ( (fs_rsp_msg.any.status != FS_OKAY_S) &&
         (fs_rsp_msg.any.status != FS_NONEXISTENT_FILE_S) )
    {
      BT_ERR( "BT RM: Remove SERV file S %x DI %x",
              fs_rsp_msg.any.status, i, 0 );
    }
  }

}
#endif /* FEATURE_EFS */


/*===========================================================================

FUNCTION
  bt_rm_cmd_device_remove_all

DESCRIPTION
  Processes the RM Device Remove All command.

===========================================================================*/
LOCAL void bt_rm_cmd_device_remove_all
(
  bt_cmd_msg_type*  rm_dra_ptr
)
{

  uint16   i;
  boolean  success;
  bt_rm_device_type   temp_device;

  bt_bd_addr_type     bd_addr[ BT_RM_MAX_DEVICES ];

  BT_MSG_API( "BT RM CMD RX: DevRmvAll AID %x",
              rm_dra_ptr->cmd_hdr.bt_app_id, 0, 0 );

  rm_dra_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_DEVICE;

  for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
  {
    if ( bt_rm_device[ i ].dev_public.valid != FALSE )
    {
      rm_dra_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

      /* Disconnect if the device is connected */
      bt_rm_disconnect_device ( &bt_rm_device[ i ] );

      /*  Device currently exists in database file (i.e.,    */
      /*  it was added to database previously).  Remove it   */
      /*  from the filesystem as well as RAM copy, except    */
      /*  for a few parameters (if there are any references) */
      bt_rm_copy_dev_info ( &temp_device, &bt_rm_device[ i ] );

      bt_rm_init_device( &bt_rm_device[ i ] );

      if ( temp_device.references )
      {
        bt_rm_copy_dev_info ( &bt_rm_device[ i ], &temp_device );
      }

      bt_rm_device[ i ].update_status = BT_RM_DUS_REMOVED_B;

      /* Save bd addr info (needed while sending UPDATED evt) */
      bd_addr [ i ] = temp_device.dev_public.bd_addr;
    }
  }

  if ( rm_dra_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    bt_rm_remove_dev_and_serv_files();

    /*  Write empty device and service files.  */
    success = bt_rm_write_device_file();

    if ( success != FALSE )
    {
      for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
      {
        bt_rm_ev_notify_device_updated( rm_dra_ptr->cmd_hdr.bt_app_id,
                                        &bd_addr[ i ],
                                        &bt_rm_device[ i ], success );
      }
    }
    else
    {
      rm_dra_ptr->cmd_hdr.cmd_status = BT_CS_RM_EFS_ERR;
    }
  }
  else
  {
    rm_dra_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_DEVICE;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_test_mode_enable

DESCRIPTION
  Processes a RM Test Mode Enable command.

===========================================================================*/
LOCAL void bt_rm_cmd_test_mode_enable
(
  bt_cmd_msg_type*  rm_tme_ptr
)
{

  boolean  test_mode_enable = rm_tme_ptr->cmd_msg.cmd_rm_testm.enable;

  BT_MSG_API( "BT RM CMD RX: RM TestM En %x",
              test_mode_enable, 0, 0 );

  if ( (rm_tme_ptr->cmd_hdr.cmd_status = bt_rm_no_connections()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( bt_rm_data.telec_started == FALSE )
    {
      bt_rm_data.test_mode_enabled = test_mode_enable;

      if ( test_mode_enable != FALSE )
      {
        /*  Enable Test Mode.  */

        BT_MSG_API( "BT RM CMD TX: DC Set HCI Mode %x",
                    BT_HCIM_STANDARD_HCI_VOC_ON, 0, 0 );
        bt_cmd_dc_set_hci_mode( BT_HCIM_STANDARD_HCI_VOC_ON );

        BT_MSG_API( "BT RM CMD TX: HC Enable DUT", 0, 0, 0 );
        bt_cmd_hc_enable_dut_mode();

        BT_MSG_API( "BT RM CMD TX: Set EvFilt %x",
                    BT_AUTO_ACCEPT_ON_ROLE_SWITCH_DIS, 0, 0 );
        bt_cmd_hc_set_event_filter_conn_all_dev(
          BT_AUTO_ACCEPT_ON_ROLE_SWITCH_DIS ) ;

        BT_MSG_API( "BT RM CMD TX: HC WrScanEn I 1 P 1", 0, 0, 0 );
        bt_cmd_hc_wr_scan_enable( BT_HC_INQ_EN_PAGE_EN );

#ifdef BT_SWDEV_2_1_SSP
        //TODO
        BT_MSG_API("SSP - BT RM CMD TX: Authentication disable - Not implemented",0,0,0);
#else
        BT_MSG_API("BT RM CMD TX: HC WrAuthEn %x",BT_HC_AUTH_DISABLED,0,0);
        bt_cmd_hc_wr_auth_enable( BT_HC_AUTH_DISABLED );
#endif /* BT_SWDEV_2_1_SSP */
        BT_MSG_API("BT RM CMD TX: HC WrEncMode %x",BT_EM_DISABLED,0,0);
        bt_cmd_hc_wr_encrypt_mode( BT_EM_DISABLED );

      }
      else
      {
        /*  Disable Test Mode.  */

        BT_MSG_API( "BT RM CMD TX: Set EvFilt Clear All", 0, 0, 0 );
        bt_cmd_hc_set_event_filter_clear_all();

        BT_MSG_API( "BT RM CMD TX: DC Set HCI Mode %x",
                    BT_HCIM_OFF, 0, 0 );
        bt_cmd_dc_set_hci_mode( BT_HCIM_OFF );

        bt_rm_check_scan_settings();
      }
    }
    else
    {
      rm_tme_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_connectable

DESCRIPTION
  Processes the RM Set Connectable command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_connectable
(
  bt_cmd_msg_type*  rm_sc_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: Set Conn %x AID %x AVP %x",
              rm_sc_ptr->cmd_msg.cmd_rm_setc.connectable,
              rm_sc_ptr->cmd_hdr.bt_app_id,
              rm_sc_ptr->cmd_msg.cmd_rm_setc.act_vs_pwr );

  if ( (rm_sc_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be new application.  */
            rm_sc_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->connectable =
      rm_sc_ptr->cmd_msg.cmd_rm_setc.connectable;

    app_ptr->conn_act_vs_pwr =
      rm_sc_ptr->cmd_msg.cmd_rm_setc.act_vs_pwr;

    bt_rm_init_unused_application( app_ptr );

#ifdef BT_SWDEV_RADIO_OFF
    if ( bt_dc.radio_state == BT_RADIO_ENABLED )
#endif
    {
      bt_rm_check_scan_settings();
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_discoverable

DESCRIPTION
  Processes the RM Set Discoverable command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_discoverable
(
  bt_cmd_msg_type*  rm_sd_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: Set Discv %x AID %x AVP %x",
              rm_sd_ptr->cmd_msg.cmd_rm_setd.discoverable,
              rm_sd_ptr->cmd_hdr.bt_app_id,
              rm_sd_ptr->cmd_msg.cmd_rm_setd.act_vs_pwr );

  if ( (rm_sd_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be a new application.  */
            rm_sd_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->discoverable =
      rm_sd_ptr->cmd_msg.cmd_rm_setd.discoverable;

    app_ptr->disc_act_vs_pwr =
      rm_sd_ptr->cmd_msg.cmd_rm_setd.act_vs_pwr;

    bt_rm_init_unused_application( app_ptr );

#ifdef BT_SWDEV_RADIO_OFF
    if ( bt_dc.radio_state == BT_RADIO_ENABLED )
#endif
    {
      bt_rm_check_scan_settings();
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_inquire

DESCRIPTION
  Processes the RM Inquire command.

===========================================================================*/
LOCAL void bt_rm_cmd_inquire
(
  bt_cmd_msg_type*  rm_inq_ptr
)
{

  bt_lap_type  bt_lap;
  float        temp_inq_len;
  uint8        hc_inq_len;
  uint8        i;
  boolean      db_full = TRUE; /* assume DB is full */

  BT_MSG_API( "BT RM CMD RX: RM Inq AID %x MR %x AC %x",
              rm_inq_ptr->cmd_hdr.bt_app_id,
              rm_inq_ptr->cmd_msg.cmd_rm_inq.max_responses,
              rm_inq_ptr->cmd_msg.cmd_rm_inq.iac.iac_bytes[ 0 ] );

  for ( i=0; i<BT_RM_MAX_DEVICES; i++ )
  {
    if ( bt_rm_device[ i ].dev_public.valid == FALSE )
    {
      db_full = FALSE; /* at least one free entry available */
      break;
    }
  }
  if ( db_full != FALSE )
  {
    rm_inq_ptr->cmd_hdr.cmd_status = BT_CS_RM_MAX_DEVICES;
  }
  else if ( bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL )
  {
    rm_inq_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
  }
#if defined ( FEATURE_BT_HOST_DIS_SCATTERNET )
  /* If there is an acl link in slave role, reject inquiry cmd */
  else if ( bt_rm_check_for_slave_acl_link() != FALSE )
  {
    BT_MSG_HIGH( "BT RM: INQ disallowed, is slave in active conn", 0, 0, 0);
    rm_inq_ptr->cmd_hdr.cmd_status = BT_CS_RM_CMD_NOT_ALLOWED;
  }
#endif /* FEATURE_BT_HOST_DIS_SCATTERNET */
  else if ( (rm_inq_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
              BT_CS_GN_SUCCESS )
  {
    bt_rm_data.inq_bt_app_id = rm_inq_ptr->cmd_hdr.bt_app_id;
    bt_rm_data.inq_max_resps =
      rm_inq_ptr->cmd_msg.cmd_rm_inq.max_responses;

    /*  Convert IAC to LAP.  */
    bt_lap[ 0 ] = rm_inq_ptr->cmd_msg.cmd_rm_inq.iac.iac_bytes[ 0 ];
    bt_lap[ 1 ] = rm_inq_ptr->cmd_msg.cmd_rm_inq.iac.iac_bytes[ 1 ];
    bt_lap[ 2 ] = rm_inq_ptr->cmd_msg.cmd_rm_inq.iac.iac_bytes[ 2 ];

    /*  Convert RM inquiry duration to one used in baseband  */
    /*  ensuring the baseband duration is at least as long   */
    /*  as the RM one.  RM is in seconds.  HC is in units    */
    /*  of 1.28 seconds.                                     */

    temp_inq_len  = rm_inq_ptr->cmd_msg.cmd_rm_inq.duration_secs;
    temp_inq_len += 2.56;
    temp_inq_len /= 1.28;
    hc_inq_len    = (uint8)(temp_inq_len);

    BT_MSG_DEBUG( "BT RM: Inq RML %x HCL %x MR %x",
                  rm_inq_ptr->cmd_msg.cmd_rm_inq.duration_secs,
                  hc_inq_len,
                  rm_inq_ptr->cmd_msg.cmd_rm_inq.max_responses );

    BT_MSG_API( "BT RM CMD TX: HC Inq LAP %x L %x MR %x",
                bt_lap[ 0 ], hc_inq_len,
                rm_inq_ptr->cmd_msg.cmd_rm_inq.max_responses );
    bt_cmd_hc_inquiry(
      bt_lap, hc_inq_len,
      rm_inq_ptr->cmd_msg.cmd_rm_inq.max_responses );

    bt_dc.inq_active = TRUE;

    /* Send RM radio activity event */
    BT_RM_UPD_RA_AND_SETBIT( BT_RA_INQUIRING );


	  if((BT_QSOC_4025_A0 == bt_cmd_dc_get_soc_version()) || 
       (BT_QSOC_4025_B0 == bt_cmd_dc_get_soc_version()) ||
       (BT_QSOC_4025_B1 == bt_cmd_dc_get_soc_version()))
	  {
        if ( bt_dc.scan_active )
        {
          /* Disable scans */
          bt_dc.scan_active = FALSE;
          bt_cmd_hc_wr_scan_enable( BT_HC_INQ_DIS_PAGE_DIS );
          BT_RM_UPD_RA_AND_CLRBIT( BT_RA_INQ_SCANNING | BT_RA_PAGE_SCANNING );
        }
      }
	
    if( bt_rm_data.current_radio_activity & BT_RA_RADIO_POWERED_ON )
    {
      /* If radio is not powered on yet, HCI command is going to be
       * requed till radio is powered on in any case and at that point, a 
       * consolidated event will be sent.
      */
    bt_rm_ev_send_radio_activity();
  }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_stop_inquiry

DESCRIPTION
  Processes the RM Stop Inquiry command.

===========================================================================*/
LOCAL void bt_rm_cmd_stop_inquiry
(
  bt_cmd_msg_type*  rm_si_ptr
)
{

  BT_MSG_API( "BT RM CMD RX: RM Stop Inq AID %x",
              rm_si_ptr->cmd_hdr.bt_app_id, 0, 0 );

  rm_si_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_si_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( rm_si_ptr->cmd_hdr.bt_app_id == bt_rm_data.inq_bt_app_id )
    {
      BT_MSG_API( "BT RM CMD TX: HC Inq Cancel", 0, 0, 0 );
      bt_cmd_hc_inquiry_cancel();
    }
    else
    {
      rm_si_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_remote_name_req

DESCRIPTION
  Processes the RM Remote Name Request command.

===========================================================================*/
LOCAL void bt_rm_cmd_remote_name_req
(
  bt_cmd_msg_type*  rm_rnr_ptr
)
{

  bt_rm_conn_type*    conn_ptr;
  bt_rm_device_type*  dev_ptr;

  BT_BDA( MSG_API, "BT RM CMD RX: RM RName Req",
          &rm_rnr_ptr->cmd_msg.cmd_rm_rname.bd_addr );

  rm_rnr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_rnr_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            FALSE,  /*  Connection need not exist yet.  */
            TRUE,   /*  Connection must be settled.     */
            &rm_rnr_ptr->cmd_msg.cmd_rm_rname.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
    {
      conn_ptr->state_acl = BT_RM_CS_CONNECTING;
      /* Page timeout is left at the value configured during the last 
       * connection creation attempt. As the controller will start new page, 
       * update the timeout now 
      */
      bt_rm_wr_hc_page_to(); 

      bt_rm_do_bb_page_started( conn_ptr );
    }
    conn_ptr->rname_app_id = rm_rnr_ptr->cmd_hdr.bt_app_id;

    BT_BDA( MSG_API, "BT RM CMD TX: HC RName Req",
            &conn_ptr->dev_ptr->dev_public.bd_addr )
    bt_cmd_hc_remote_name_req( conn_ptr->dev_ptr->dev_public.bd_addr,
                               conn_ptr->dev_ptr->page_scan_rep_mode,
                               conn_ptr->dev_ptr->page_scan_mode,
                               conn_ptr->dev_ptr->clk_offset );

    bt_rm_reset_idle_timeout( conn_ptr );
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: Could not req name S %x",
                  rm_rnr_ptr->cmd_hdr.cmd_status, 0, 0 );
    BT_BDA( MSG_DEBUG, "BT RM: Could not req name",
            &rm_rnr_ptr->cmd_msg.cmd_rm_rname.bd_addr );

    if ( (dev_ptr =
            bt_rm_get_device(
              &rm_rnr_ptr->cmd_msg.cmd_rm_rname.bd_addr,
              FALSE )) != NULL )
    {
      rm_rnr_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

      bt_rm_ev_send_remote_name( rm_rnr_ptr->cmd_hdr.bt_app_id,
                                 &dev_ptr->dev_public,
                                 BT_CS_GN_SUCCESS );
    }
    else
    {
      BT_BDA( MSG_DEBUG, "BT RM: No dev name",
              &rm_rnr_ptr->cmd_msg.cmd_rm_rname.bd_addr );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_remote_name_req_cancel

DESCRIPTION
  Processes the RM Remote Name Request Cancel command.

===========================================================================*/
LOCAL void bt_rm_cmd_remote_name_req_cancel
(
  bt_cmd_msg_type*  rm_rnrc_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  bt_bd_addr_type*  bd_addr_ptr =
                      &rm_rnrc_ptr->cmd_msg.cmd_rm_rname_cancel.bd_addr;

  BT_BDA( MSG_API, "BT RM CMD RX: RM RName Req Cancel",
          bd_addr_ptr );

  rm_rnrc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (conn_ptr = bt_rm_find_conn_bd_addr( bd_addr_ptr ) ) != NULL )
  {
    if ( ( conn_ptr->state_acl == BT_RM_CS_CONNECTING ) &&
         ( conn_ptr->rname_app_id != BT_APP_ID_NULL ) )
    {
      /* Send the HCI Remote Name Req Cancel Cmd */
      bt_cmd_hc_remote_name_req_cancel ( bd_addr_ptr );
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Conn not in state to cancel CS %x CA %x",
                       conn_ptr->state_acl, conn_ptr->rname_app_id, 0 );
      rm_rnrc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: BD Addr not in Conn table", 0, 0, 0 );
    rm_rnrc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_connect_acl

DESCRIPTION
  Processes a RM Connect ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_connect_acl
(
  bt_cmd_msg_type*  rm_ca_ptr
)
{

  bt_rm_conn_type*     conn_ptr;
  bt_cmd_status_type*  status_ptr;
  bt_bd_addr_type*     bd_addr_ptr;
  bt_app_id_type       bt_app_id;

  status_ptr  = &rm_ca_ptr->cmd_hdr.cmd_status;
  bd_addr_ptr = &rm_ca_ptr->cmd_msg.cmd_rm_conna.bd_addr;
  bt_app_id   = rm_ca_ptr->cmd_hdr.bt_app_id;

  BT_MSG_API( "BT RM CMD RX: RM ConnA AID %x", bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM ConnA", bd_addr_ptr );

  if ( (*status_ptr =
           bt_rm_get_conn_bd_addr(
             FALSE,  /*  Connection need not exist yet.   */
             TRUE,  /*  Connection must be settled.  */
             bd_addr_ptr, BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->state_acl == BT_RM_CS_DISCONNECTED )
    {
      if ( (*status_ptr =
               bt_rm_new_acl_connection_ok()) == BT_CS_GN_SUCCESS )
      {
        conn_ptr->bt_app_id_acl = bt_app_id;
        bt_rm_create_connection_acl( conn_ptr );
      }
    }
    else
    {
      *status_ptr =
        bt_rm_accept_connection_acl( conn_ptr, bt_app_id );
    }
  }

  if ( ( (*status_ptr != BT_CS_GN_SUCCESS) &&
         (*status_ptr != BT_CS_GN_RETRY_CMD_LATER) ) ||
       ( (*status_ptr == BT_CS_GN_RETRY_CMD_LATER ) &&
         ( BT_CHECK_RETRY_EXPIRED( rm_ca_ptr->cmd_hdr.cmd_retries ) ) ) )
  {
    BT_MSG_HIGH( "BT RM: ConnFA No Res CS %x", *status_ptr, 0, 0 );
    BT_BDA( MSG_HIGH, "BT RM: ConnFA No Res", bd_addr_ptr );

    bt_rm_ev_send_conn_bd_addr_failed( bt_app_id, bd_addr_ptr,
                                       BT_ACL_LINK,
                                       BT_EVR_RM_NO_RESOURCES );
  }
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_connect_cancel

DESCRIPTION
  Processes a RM Connect Cancel command.

===========================================================================*/
LOCAL void bt_rm_cmd_connect_cancel
(
  bt_cmd_msg_type*  rm_cc_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  bt_bd_addr_type*  bd_addr_ptr =
                      &rm_cc_ptr->cmd_msg.cmd_rm_conn_cancel.bd_addr;

  BT_BDA( MSG_API, "BT RM CMD RX: RM Connect Cancel",
          bd_addr_ptr );

  rm_cc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (conn_ptr = bt_rm_find_conn_bd_addr( bd_addr_ptr ) ) != NULL )
  {
    if ( ( conn_ptr->state_acl == BT_RM_CS_CONNECTING ) &&
         ( conn_ptr->rname_app_id == BT_APP_ID_NULL ) )
    {
      /* Send the HCI Create Connection Cancel Cmd */
      bt_cmd_hc_create_connection_cancel ( bd_addr_ptr );
    }
    else
    {
      BT_MSG_DEBUG( "BT RM: Conn not in state to cancel CS %x CA %x",
                       conn_ptr->state_acl, conn_ptr->rname_app_id, 0 );
      rm_cc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }
  else
  {
    BT_MSG_DEBUG( "BT RM: BD Addr not in Conn table", 0, 0, 0 );
    rm_cc_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
  }
}

/*===========================================================================

FUNCTION
  bt_rm_cmd_register_acl

DESCRIPTION
  Processes a RM Register ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_register_acl
(
  bt_cmd_msg_type*  rm_ra_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RegA AID %x",
              rm_ra_ptr->cmd_hdr.bt_app_id, 0, 0 );

  if ( (rm_ra_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be new application.  */
            rm_ra_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->acl_registered = TRUE;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_accept_acl

DESCRIPTION
  Processes a RM Accept ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_accept_acl
(
  bt_cmd_msg_type*  rm_aa_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM AcptA AID %x",
              rm_aa_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM AcptA",
          &rm_aa_ptr->cmd_msg.cmd_rm_acpta.bd_addr );

  if ( (rm_aa_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_aa_ptr->cmd_msg.cmd_rm_acpta.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    rm_aa_ptr->cmd_hdr.cmd_status =
      bt_rm_accept_connection_acl( conn_ptr,
                                   rm_aa_ptr->cmd_hdr.bt_app_id );
  }

  if ( (rm_aa_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_aa_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    /* BT_TBD:  Send Connection Failed event? */
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_reject_acl

DESCRIPTION
  Processes a RM Reject ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_reject_acl
(
  bt_cmd_msg_type*  rm_ra_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM RejA R %x AID %x",
              rm_ra_ptr->cmd_msg.cmd_rm_reja.reason,
              rm_ra_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM RejA",
          &rm_ra_ptr->cmd_msg.cmd_rm_reja.bd_addr );

  rm_ra_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_ra_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_ra_ptr->cmd_msg.cmd_rm_reja.bd_addr,
            BT_ACL_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->acl_resps_pending > 0 )
    {
      if ( (--conn_ptr->acl_resps_pending == 0) &&
           (conn_ptr->bt_app_id_acl == BT_APP_ID_NULL) )
      {
        conn_ptr->acl_req_timeout_ms_left = 0;

        bt_rm_reject_connection(
          conn_ptr,
          (bt_error_code_type)rm_ra_ptr->cmd_msg.cmd_rm_reja.reason,
          BT_ACL_LINK );
      }
    }
  }


}


/*===========================================================================

FUNCTION
  bt_rm_cmd_unregister_acl

DESCRIPTION
  Processes a RM Unregister ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_unregister_acl
(
  bt_cmd_msg_type*  rm_ua_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: UnregA AID %x",
              rm_ua_ptr->cmd_hdr.bt_app_id, 0, 0 );

  if ( (rm_ua_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            FALSE,  /*  Must be existing application.  */
            rm_ua_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->acl_registered = FALSE;

    bt_rm_init_unused_application( app_ptr );
  }

}




/*===========================================================================

FUNCTION
  bt_rm_cmd_tx_data_acl

DESCRIPTION
  Processes a RM Transmit Data ACL command.

===========================================================================*/
void bt_rm_cmd_tx_data_acl
(
  bt_cmd_msg_type*  rm_tda_ptr
)
{

  bt_app_id_type                bt_app_id;
  bt_rm_handle_type             rm_handle;
  dsm_item_type*                dsm_ptr;
  bt_rm_conn_type*              conn_ptr;
  bt_hc_pkt_boundary_flag_type  pkt_boundary_flag;
  uint16                        local_cid; /* L2CAP local cid */
#ifndef FEATURE_BT_SOC
  uint16                        flush_to;
#endif

#if ((defined BT_TEST_PYLD_FAST_TX) && (!defined(FEATURE_BT_SOC)))
  bt_cmd_hc_tx_acl_data_type    hc_tda_cmd;
#endif

  bt_app_id       = rm_tda_ptr->cmd_hdr.bt_app_id;
  rm_handle       = rm_tda_ptr->cmd_msg.cmd_rm_txda.handle;
  dsm_ptr         = rm_tda_ptr->cmd_msg.cmd_rm_txda.dsm_ptr;
  pkt_boundary_flag = rm_tda_ptr->cmd_msg.cmd_rm_txda.l2cap_pkt_start;
  local_cid         = rm_tda_ptr->cmd_msg.cmd_rm_txda.local_cid;
  BT_MSG_PYLD( "BT RM CMD RX: RM TXDA H %x PB %x D %x",
               rm_handle, pkt_boundary_flag, dsm_ptr->data_ptr[ 0 ] );

  if ( (rm_tda_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_rm_handle(
            FALSE,  /*  Connection need not be settled.  */
            rm_handle, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->hc_handle_acl != BT_HC_NULL_CONN_HANDLE )
    {
      if ( bt_app_id == conn_ptr->bt_app_id_acl )
      {
        /*  Unsniff or Unpark if necessary.  */
        bt_rm_acl_link_txrdy( conn_ptr );

#ifdef BT_SWDEV_2_1_SSR
        if ( conn_ptr->link_mode == BT_LM_ACTIVE )
#endif /* BT_SWDEV_2_1_SSR */
        {
          bt_rm_reset_idle_timeout( conn_ptr );
        }

#ifndef FEATURE_BT_SOC
        if ( rm_tda_ptr->cmd_msg.cmd_rm_txda.flush_timeout !=
               conn_ptr->flush_timeout )
        {
          conn_ptr->flush_timeout =
            rm_tda_ptr->cmd_msg.cmd_rm_txda.flush_timeout;

          BT_MSG_API( "BT RM CMD TX: HC Wr FTO %x H %x",
                      conn_ptr->hc_handle_acl,
                      conn_ptr->flush_timeout, 0 );
          if(conn_ptr->flush_timeout != BT_L2_DEFAULT_FLUSH_TIMEOUT)
          {
              flush_to = BT_RM_MS_TO_625USEC(conn_ptr->flush_timeout);
          }
          else
          {
              flush_to = 0x0;
          }
          bt_cmd_hc_wr_auto_flush_timeout( conn_ptr->hc_handle_acl, flush_to);
        }
#endif /* FEATURE_BT_SOC */

#if ((defined BT_TEST_PYLD_FAST_TX) && (!defined(FEATURE_BT_SOC)))
        hc_tda_cmd.conn_hndl         = conn_ptr->hc_handle_acl;
        hc_tda_cmd.local_cid         = local_cid;
        hc_tda_cmd.pkt_boundary_flag = pkt_boundary_flag;
        hc_tda_cmd.broadcast_flag    = BT_HC_BROADCAST_NONE;
        hc_tda_cmd.dsm_ptr           = dsm_ptr;
        hc_tda_cmd.frame_handle      = rm_tda_ptr->cmd_msg.cmd_rm_txda.frame_handle;
        bt_hc_cmd_tx_acl_data( &hc_tda_cmd );
#else
        BT_MSG_PYLD( "BT RM CMD TX: HC TXDA H %x BF %x",
                     conn_ptr->hc_handle_acl, pkt_boundary_flag, 0 );
        bt_cmd_hc_tx_acl_data( conn_ptr->hc_handle_acl,
                               pkt_boundary_flag,
                               BT_HC_BROADCAST_NONE, dsm_ptr,
                               local_cid,
                               rm_tda_ptr->cmd_msg.cmd_rm_txda.frame_handle );
#endif

        dsm_ptr = NULL;  /*  Prevent freeing DSM.  */
      }
      else
      {
        rm_tda_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
      }
    }
    else
    {
      rm_tda_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_CONNECTION;
    }
  }

  dsm_free_packet( &dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_disconnect_acl

DESCRIPTION
  Processes a RM Disconnect ACL command.

===========================================================================*/
LOCAL void bt_rm_cmd_disconnect_acl
(
  bt_cmd_msg_type*  rm_da_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM DiscA H %x R %x AID %x",
               rm_da_ptr->cmd_msg.cmd_rm_disca.handle,
               rm_da_ptr->cmd_msg.cmd_rm_disca.reason,
               rm_da_ptr->cmd_hdr.bt_app_id );

  if ( (rm_da_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_rm_handle(
            FALSE,  /*  Connection must be settled.  */
            rm_da_ptr->cmd_msg.cmd_rm_disca.handle,
            &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    /* For disconnect command, connection need not be 'settled'. If SCO is present
       disconnect SCO first. Otherwise, send ACL disconnect command to controller 
       right-away */
    if ( rm_da_ptr->cmd_hdr.bt_app_id == conn_ptr->bt_app_id_acl )
    {
      if ( conn_ptr->state_sco == BT_RM_CS_DISCONNECTED )
      {
        bt_rm_disconnect_acl( conn_ptr,
                              rm_da_ptr->cmd_msg.cmd_rm_disca.reason );
      }
      else if ( conn_ptr->state_sco == BT_RM_CS_CONNECTED )
      {
        /* Update link policy to prevent link from going into lpm before 
           disconnecting */
    if ( conn_ptr->link_policy != 0 )
    {
        conn_ptr->link_policy = 0;
        BT_MSG_API( "BT RM CMD TX: HC Wr LP %x H %x RH %x",
                    conn_ptr->link_policy, conn_ptr->hc_handle_acl,
                    conn_ptr->rm_handle );
        bt_cmd_hc_wr_link_policy( conn_ptr->hc_handle_acl,
                              conn_ptr->link_policy );
    }

        bt_rm_acl_link_active( conn_ptr );
        bt_rm_disconnect_sco( conn_ptr,
                              rm_da_ptr->cmd_msg.cmd_rm_disca.reason );
      }
      else
      {
        rm_da_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;

        BT_MSG_API( "BT RM: Waiting for SCO discon before disc ACL", 0, 0, 0);
      }
    }
    else
    {
      rm_da_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }
  else
  {
    BT_ERR( "BT RM: ACL disconnect failed, app_id = %d, conn_ptr = 0x%x, status = %x",
            rm_da_ptr->cmd_hdr.bt_app_id, conn_ptr, rm_da_ptr->cmd_hdr.cmd_status );
  }
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_connect_sco

DESCRIPTION
  Processes a RM Connect SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_connect_sco
(
  bt_cmd_msg_type*  rm_cs_ptr
)
{

  bt_rm_conn_type*  conn_ptr;
  bt_bd_addr_type*  bd_addr_ptr;
  bt_app_id_type    bt_app_id;
  bt_ev_msg_type    ev_rm_cfs;
  boolean           attempt_esco;
  uint16            sync_pkt_types = 0 ;

  bd_addr_ptr  = &rm_cs_ptr->cmd_msg.cmd_rm_conns.bd_addr;
  bt_app_id    = rm_cs_ptr->cmd_hdr.bt_app_id;
  attempt_esco = rm_cs_ptr->cmd_msg.cmd_rm_conns.attempt_esco;

  BT_MSG_API( "BT RM CMD RX: RM ConnS AID %x eSCO %x",
              bt_app_id, attempt_esco, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM ConnS", bd_addr_ptr );

  if ( (rm_cs_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,  /*  Connection must exist.       */
            TRUE,  /*  Connection must be settled.  */
            bd_addr_ptr,
            BT_SCO_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( bt_rm_total_active_sco_links() < BT_MAX_SCO_CONNECTIONS )
    {
      if ( bt_rm_piconet_acl_pkt_types_settled() != FALSE )
      {
        if ( (conn_ptr->bt_app_id_sco == BT_APP_ID_NULL) &&
             (conn_ptr->state_sco == BT_RM_CS_DISCONNECTED) )
        {
          conn_ptr->bt_app_id_sco = bt_app_id;
          conn_ptr->state_sco     = BT_RM_CS_CONNECTING;
#ifdef FEATURE_BT_1_2
          conn_ptr->attempt_esco  = attempt_esco;
#endif /* FEATURE_BT_1_2 */

          /*  Only use 1 slot packets prior to SCO starting.  */
          if ( (conn_ptr->pending_pkt_adjs =
                  bt_rm_check_all_packet_types_acl()) == FALSE )
          {
            sync_pkt_types = bt_rm_select_sco_packet_type ( conn_ptr );
#ifdef FEATURE_BT_1_2

            BT_MSG_API( "BT RM CMD TX: HC Add sync conn H %x PT %x AID %x",
                        conn_ptr->hc_handle_acl,
                        sync_pkt_types,
                        conn_ptr->bt_app_id_sco );

            bt_cmd_hc_setup_sync_conn(
              conn_ptr->hc_handle_acl,
              BT_LM_SCO_BANDWIDTH,
              BT_LM_SCO_BANDWIDTH,
              BT_LM_ESCO_LATENCY_10_MS,
              BT_LM_VC_AC_CVSD,
              BT_LM_ESCO_RETX_OPTIMUM_POWER,
              sync_pkt_types );
#else

            /* attempt_esco is ignored since eSCO is not supported */
            BT_MSG_API( "BT RM CMD TX: HC Add SCO H %x PT %x AID %x",
                        conn_ptr->hc_handle_acl,
                        sync_pkt_types,
                        conn_ptr->bt_app_id_sco );

            bt_cmd_hc_add_sco_connection(
              conn_ptr->hc_handle_acl,
              sync_pkt_types );
#endif /* FEATURE_BT_1_2 */
          }
        }
        else
        {
          rm_cs_ptr->cmd_hdr.cmd_status = BT_CS_RM_CONN_ACTIVE;
        }
      }
      else
      {
        rm_cs_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
    }
    else
    {
      rm_cs_ptr->cmd_hdr.cmd_status = BT_CS_RM_MAX_CONNECTIONS;
    }
  }

  if ( (rm_cs_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_cs_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    ev_rm_cfs.ev_hdr.ev_type             = BT_EV_RM_CONNECTION_FAILED_SCO;
    ev_rm_cfs.ev_hdr.bt_app_id           = bt_app_id;
    ev_rm_cfs.ev_msg.ev_rm_confs.bd_addr = *bd_addr_ptr;
    ev_rm_cfs.ev_msg.ev_rm_confs.reason  = BT_EVR_RM_NO_RESOURCES;

    BT_MSG_HIGH( "BT RM: ConnFS No Res CS %x",
                 rm_cs_ptr->cmd_hdr.cmd_status, 0, 0 );
    BT_BDA( MSG_HIGH, "BT RM: ConnFS No Res", bd_addr_ptr );

    BT_MSG_API( "BT RM EV TX: ConnFS No Res AID %x", bt_app_id, 0, 0 );
    BT_BDA( MSG_API, "BT RM EV TX: ConnFS No Res", bd_addr_ptr );
    bt_ec_send_event( &ev_rm_cfs );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_register_sco

DESCRIPTION
  Processes a RM Register SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_register_sco
(
  bt_cmd_msg_type*  rm_rs_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: RegS AID %x",
              rm_rs_ptr->cmd_hdr.bt_app_id, 0, 0 );

  if ( (rm_rs_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            TRUE,  /*  Can be new application.  */
            rm_rs_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->sco_registered = TRUE;
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_accept_sco

DESCRIPTION
  Processes a RM Accept SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_accept_sco
(
  bt_cmd_msg_type*  rm_as_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM AcptS AID %x",
              rm_as_ptr->cmd_hdr.bt_app_id, 0, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM AcptS",
          &rm_as_ptr->cmd_msg.cmd_rm_acpts.bd_addr );

  rm_as_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (rm_as_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_as_ptr->cmd_msg.cmd_rm_acpts.bd_addr,
            BT_SCO_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( bt_rm_piconet_acl_pkt_types_settled() != FALSE )
    {
      rm_as_ptr->cmd_hdr.cmd_status =
        bt_rm_accept_connection_sco( conn_ptr,
                                     rm_as_ptr->cmd_hdr.bt_app_id );
    }
    else
    {
      rm_as_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
    }
  }

  if ( (rm_as_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS) &&
       (rm_as_ptr->cmd_hdr.cmd_status != BT_CS_GN_RETRY_CMD_LATER) )
  {
    /* BT_TBD:  Send Connection Failed event? */
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_reject_sco

DESCRIPTION
  Processes a RM Reject SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_reject_sco
(
  bt_cmd_msg_type*  rm_rs_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM RejS R %x AID %x",
              rm_rs_ptr->cmd_msg.cmd_rm_rejs.reason,
              rm_rs_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT RM CMD RX: RM RejS",
          &rm_rs_ptr->cmd_msg.cmd_rm_rejs.bd_addr );

  if ( (rm_rs_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_bd_addr(
            TRUE,   /*  Connection must exist.           */
            FALSE,  /*  Connection need not be settled.  */
            &rm_rs_ptr->cmd_msg.cmd_rm_rejs.bd_addr,
            BT_SCO_LINK, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( conn_ptr->sco_resps_pending > 0 )
    {
      if ( (--conn_ptr->sco_resps_pending == 0) &&
           (conn_ptr->bt_app_id_sco == BT_APP_ID_NULL) )
      {
        conn_ptr->sco_req_timeout_ms_left = 0;

        BT_BDA( MSG_API, "BT RM CMD TX: HC Rej ConnS",
                &conn_ptr->dev_ptr->dev_public.bd_addr );
        bt_rm_reject_connection
                 ( conn_ptr,
                   (bt_error_code_type)rm_rs_ptr->cmd_msg.cmd_rm_rejs.reason,
                                 BT_SCO_LINK
                 );
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_unregister_sco

DESCRIPTION
  Processes a RM Unregister SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_unregister_sco
(
  bt_cmd_msg_type*  rm_us_ptr
)
{

  bt_rm_app_type*  app_ptr;

  BT_MSG_API( "BT RM CMD RX: UnregS AID %x",
              rm_us_ptr->cmd_hdr.bt_app_id, 0, 0 );

  if ( (rm_us_ptr->cmd_hdr.cmd_status =
          bt_rm_get_application(
            FALSE,  /*  Must be existing application.  */
            rm_us_ptr->cmd_hdr.bt_app_id,
            &app_ptr )) == BT_CS_GN_SUCCESS )
  {
    app_ptr->sco_registered = FALSE;

    bt_rm_init_unused_application( app_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_tx_data_sco

DESCRIPTION
  Processes a RM Transmit Data SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_tx_data_sco
(
  bt_cmd_msg_type*  rm_tds_ptr
)
{

  bt_app_id_type              bt_app_id;
  bt_rm_handle_type           rm_handle;
  dsm_item_type*              dsm_ptr = NULL;
  bt_rm_conn_type*            conn_ptr;

#if ((defined BT_TEST_PYLD_FAST_TX) && (!defined(FEATURE_BT_SOC)))
  bt_cmd_hc_tx_sco_data_type  hc_tds_cmd;
#endif

  bt_app_id = rm_tds_ptr->cmd_hdr.bt_app_id;
  rm_handle = rm_tds_ptr->cmd_msg.cmd_rm_txds.handle;
  dsm_ptr   = rm_tds_ptr->cmd_msg.cmd_rm_txds.dsm_ptr;

  BT_MSG_PYLD( "BT RM CMD RX: RM TXDS H %x D %x",
               rm_handle, dsm_ptr->data_ptr[ 0 ], 0 );

  if ( (rm_tds_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_rm_handle(
            FALSE,  /*  Connection need not be settled.  */
            rm_handle, &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( (conn_ptr->state_acl == BT_RM_CS_CONNECTED) &&
         (conn_ptr->state_sco == BT_RM_CS_CONNECTED) )
    {
      if ( bt_app_id == conn_ptr->bt_app_id_sco )
      {
#if ((defined BT_TEST_PYLD_FAST_TX) && (!defined(FEATURE_BT_SOC)))
        hc_tds_cmd.conn_hndl = conn_ptr->hc_handle_sco;
        hc_tds_cmd.dsm_ptr   = dsm_ptr;
        bt_hc_cmd_tx_sco_data( &hc_tds_cmd );
#else
        BT_MSG_PYLD( "BT RM CMD TX: HC TXDS H %x D %x",
                     conn_ptr->hc_handle_sco,
                     dsm_ptr->data_ptr[ 0 ], 0 );
        bt_cmd_hc_tx_sco_data( conn_ptr->hc_handle_sco, dsm_ptr );
#endif

        dsm_ptr = NULL;  /*  Prevent freeing DSM.  */
      }
      else
      {
        rm_tds_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
      }
    }
    else
    {
      rm_tds_ptr->cmd_hdr.cmd_status = BT_CS_RM_NO_CONNECTION;
    }
  }

  dsm_free_packet( &dsm_ptr );

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_disconnect_sco

DESCRIPTION
  Processes a RM Disconnect SCO command.

===========================================================================*/
LOCAL void bt_rm_cmd_disconnect_sco
(
  bt_cmd_msg_type*  rm_ds_ptr
)
{

  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM DiscS H %x R %x AID %x",
              rm_ds_ptr->cmd_msg.cmd_rm_discs.handle,
              rm_ds_ptr->cmd_msg.cmd_rm_discs.reason,
              rm_ds_ptr->cmd_hdr.bt_app_id );

  if ( (rm_ds_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_rm_handle(
            TRUE,  /*  Connection must be settled.  */
            rm_ds_ptr->cmd_msg.cmd_rm_discs.handle,
            &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    if ( rm_ds_ptr->cmd_hdr.bt_app_id == conn_ptr->bt_app_id_sco )
    {
      bt_rm_disconnect_sco( conn_ptr,
                            rm_ds_ptr->cmd_msg.cmd_rm_discs.reason );
    }
    else
    {
      rm_ds_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
    }
  }

}

#ifdef BT_SWDEV_2_1_PB
LOCAL void bt_rm_cmd_set_auto_flush_to
(
  bt_cmd_msg_type*  rm_cmd_ptr
)
{
  bt_rm_conn_type*  conn_ptr;

  BT_MSG_API( "BT RM CMD RX: RM Set Auto Flush TO 0x%x H 0x%x",
               rm_cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.flush_to,
               rm_cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.rm_handle,
               0 );

  if ( (rm_cmd_ptr->cmd_hdr.cmd_status =
          bt_rm_get_conn_rm_handle(
            TRUE,  /*  Connection must be settled.  */
            rm_cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.rm_handle,
            &conn_ptr )) == BT_CS_GN_SUCCESS )
  {
    conn_ptr->flush_timeout =
               rm_cmd_ptr->cmd_msg.cmd_rm_set_auto_flush_to.flush_to;
    bt_cmd_hc_wr_auto_flush_timeout( conn_ptr->hc_handle_acl,
                                     BT_RM_MS_TO_625USEC(conn_ptr->flush_timeout) );
  }
  else
  {
    rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;
  }
}
#endif /* BT_SWDEV_2_1_PB */

#ifdef FEATURE_BT_TELEC
/*===========================================================================

FUNCTION
  bt_rm_cmd_start_telec_test

DESCRIPTION
  Processes a RM Start TELEC Test command.

===========================================================================*/
LOCAL void bt_rm_cmd_start_telec_test
(
  bt_cmd_msg_type*  rm_stt_ptr
)
{

  BT_MSG_API( "BT RM CMD RX: RM Telec Tx %x Hp %x AID %x",
              rm_stt_ptr->cmd_msg.cmd_rm_telec.transmit,
              rm_stt_ptr->cmd_msg.cmd_rm_telec.hopping,
              rm_stt_ptr->cmd_hdr.bt_app_id );

  if ( (rm_stt_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( (rm_stt_ptr->cmd_hdr.cmd_status = bt_rm_no_connections()) ==
            BT_CS_GN_SUCCESS )
    {
      bt_rm_data.telec_started = TRUE;

      bt_rm_data.telec_transmit =
        rm_stt_ptr->cmd_msg.cmd_rm_telec.transmit;
      bt_rm_data.telec_hopping  =
        rm_stt_ptr->cmd_msg.cmd_rm_telec.hopping;

      /*  Must use the BT only image.  */

      BT_MSG_API( "BT RM CMD TX: DC Set HCI Mode %x > %x",
                  BT_HCIM_OFF, BT_HCIM_STANDARD_HCI_ON, 0 );
      bt_cmd_dc_set_hci_mode( BT_HCIM_OFF );
      bt_cmd_dc_set_hci_mode( BT_HCIM_STANDARD_HCI_ON );

      if ( bt_rm_data.telec_transmit != FALSE )
      {
        /*  Continuous Transmit Test.  */

        BT_MSG_API( "BT RM CMD TX: HC Tx Cont %x",
                    bt_rm_data.telec_hopping, 0, 0 );
        bt_cmd_hc_tx_continuous( bt_rm_data.telec_hopping );
      }
      else
      {
        /*  Receive Test:  Place in continuous (R0) inquiry scan mode.  */

        BT_MSG_API( "BT RM CMD TX: HC Wr IScan Act R0", 0, 0, 0 );
        bt_cmd_hc_wr_inq_scan_act( 0x800, 0x800 );

        BT_MSG_API( "BT RM CMD TX: HC WrScanEn I 1 P 0", 0, 0, 0 );
        bt_cmd_hc_wr_scan_enable( BT_HC_INQ_EN_PAGE_DIS );
      }
    }
  }

}
#endif /* FEATURE_BT_TELEC */


#ifdef BT_SWDEV_RADIO_OFF
/*===========================================================================

FUNCTION
  bt_rm_radio_disabled

DESCRIPTION
  Performs post-processing after BT radio has been turned off.

===========================================================================*/
void bt_rm_radio_disabled
(
  void
)
{
  bt_ev_msg_type ev_rm_rd;

  ev_rm_rd.ev_hdr.ev_type   = BT_EV_RM_RADIO_DISABLED;
  ev_rm_rd.ev_hdr.bt_app_id = bt_rm_data.disable_radio_app_id;

#ifndef FEATURE_BT_SOC
  if ( bt_dc.radio_state == BT_RADIO_DISABLED )
#else
  if (( bt_dc.radio_state == BT_RADIO_DISABLED ) ||
      ( bt_dc.radio_state == BT_RADIO_DISABLING ))
#endif
  {
    ev_rm_rd.ev_msg.ev_rm_radio.disabled = TRUE;

    BT_MSG_HIGH( "BT RM: Radio Off", 0, 0, 0 );

    BT_MSG_API( "BT RM EV TX: Radio Off AID %x",
                bt_rm_data.disable_radio_app_id, 0, 0 );

    bt_ec_send_event( &ev_rm_rd );
  }
  else if ( bt_dc.radio_state == BT_RADIO_ENABLED )
  {
    ev_rm_rd.ev_msg.ev_rm_radio.disabled = FALSE;

    BT_MSG_HIGH( "BT RM: Radio On", 0, 0, 0 );

    BT_MSG_API( "BT RM EV TX: Radio On AID %x",
                bt_rm_data.disable_radio_app_id, 0, 0 );

    /*  Re-enable scanning as appropriate.  */
    bt_rm_check_scan_settings();

    bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;

    bt_ec_send_event( &ev_rm_rd );
  }
  else
  {
    BT_ERR( "BT RM: Radio on/off in unexpected state %x",
            bt_dc.radio_state, 0, 0 );
  }
  bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;
}


/*===========================================================================

FUNCTION
  bt_rm_end_connection

DESCRIPTION
  Sends an appropriate RM event for a specified connection such that
  the upper layers and applications are consistent with there being
  no connection existing any more at or below the RM.

===========================================================================*/
LOCAL void bt_rm_end_connection
(
  bt_rm_conn_type*       conn_ptr,
  bt_rm_conn_state_type  conn_state,
  bt_link_type           link_type,
  bt_event_reason_type   reason
)
{

  switch ( conn_state )
  {
    case BT_RM_CS_CONNECTING:
      {
        bt_rm_connection_failed( conn_ptr, link_type, reason );
        break;
      }
    case BT_RM_CS_CONNECTED:
    case BT_RM_CS_DISCONNECTING:
      {
        bt_rm_connection_disconnected( conn_ptr, link_type, reason );
        break;
      }

    case BT_RM_CS_DISCONNECTED:
    default:
      {
        /*  Nothing to do.  */
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_disable_radio

DESCRIPTION
  Processes a RM Disable Radio command.

===========================================================================*/
LOCAL void bt_rm_cmd_disable_radio
(
  bt_cmd_msg_type*  rm_dr_ptr
)
{

  boolean        new_disable_radio = FALSE;
  uint16         i;
  bt_ev_msg_type ev_rm_rd;

  BT_MSG_API( "BT RM CMD RX: Disable Radio %x AID %x",
              rm_dr_ptr->cmd_msg.cmd_rm_dsrad.disable,
              rm_dr_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_dr_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( (bt_rm_data.disable_radio_app_id == BT_APP_ID_NULL) ||
         (rm_dr_ptr->cmd_hdr.bt_app_id ==
            bt_rm_data.disable_radio_app_id) )
    {
      if ( rm_dr_ptr->cmd_msg.cmd_rm_dsrad.disable != FALSE )
      {
        new_disable_radio = TRUE;
      }
      bt_rm_data.disable_radio_app_id = rm_dr_ptr->cmd_hdr.bt_app_id;

      if ( bt_rm_data.disable_radio != new_disable_radio )
      {
        bt_rm_data.disable_radio = new_disable_radio;
        if ( bt_rm_data.disable_radio != FALSE )
        {
         /*  Send an appropriate RM event for each active  */
          /*  connection that it no longer exists.          */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            bt_rm_end_connection(
              &bt_rm_conn[ i ], bt_rm_conn[ i ].state_sco,
              BT_SCO_LINK, (BT_HCI_REASON( BT_BE_POWER_GOING_OFF )) );

            bt_rm_end_connection(
              &bt_rm_conn[ i ], bt_rm_conn[ i ].state_acl,
              BT_ACL_LINK, (BT_HCI_REASON( BT_BE_POWER_GOING_OFF )) );
          }

          /*  Send inquiry complete if inquiring.  */
          if ( bt_rm_data.inq_bt_app_id != BT_APP_ID_NULL )
          {
            bt_rm_ev_send_inquiry_complete(
              bt_rm_data.inq_bt_app_id,
              bt_rm_data.inq_responses );

            bt_rm_init_inquiry_data();
          }

          /*  Remove all queued HCI commands and events.  */
          bt_remove_queued_hc_commands();
          bt_hc_empty_ev_queue();

#ifndef FEATURE_BT_SOC
          /* Do HCI Reset */
          bt_hc_reset_driver();
#else
          bt_dc.radio_state = BT_RADIO_DISABLING;

          /* Send HCI RESET only if no HW Error event has been recieved from SOC.
           * Because the SOC will not be in a state to respond, if HW err evt has
           * been recieved.
           */
          if ( bt_rm_hwerr_radio_off == TRUE )
          {
            bt_hc_pe_init();
#ifndef FEATURE_BT_SOC1_FLASH
            /* The Radio-off procedure has ended*/
            /* For SOC1 SURF builds where SOC1 flash part is used */
            /* procedure will end after HCI reset cmd complete */
            bt_rm_hwerr_radio_off = FALSE;
#endif /* !FEATURE_BT_SOC1_FLASH */
          }
          else
          {
            bt_soc_etc_reset_driver();
          }
#endif /* FEATURE_BT_SOC */

          /* Generate appropriate event if already in disabled state */
#ifndef FEATURE_BT_SOC
          if ( bt_dc.radio_state == BT_RADIO_DISABLED )
#endif
          {
            bt_rm_radio_disabled();
          }
          bt_rm_data.current_radio_activity = BT_RA_RADIO_FORCED_OFF;
          bt_rm_ev_send_radio_activity();
        }
        else
        {
          bt_dc.radio_state = BT_RADIO_ENABLING;

#ifdef FEATURE_BT_SOC
          /* Enable driver (same for SOC1 ROM and QSOC) */
#ifndef FEATURE_BT_SOC1_FLASH
          bt_dc_enable_driver();
#endif /* !FEATURE_BT_SOC1_FLASH */

          bt_dc.radio_state = BT_RADIO_ENABLED;

          bt_rm_data.current_radio_activity = BT_RA_RADIO_IDLE;
          bt_rm_ev_send_radio_activity();

          /* notify all apps */
          bt_rm_radio_disabled();
#else /* FEATURE_BT_SOC */
          bt_dc_enable_driver();

#endif /* FEATURE_BT_SOC */
        }
      }
      else
      {
        BT_MSG_DEBUG( "BT RM: Ignored Disable Radio %x AID %x",
                      rm_dr_ptr->cmd_msg.cmd_rm_dsrad.disable,
                      rm_dr_ptr->cmd_hdr.bt_app_id, 0 );

        /*  Send Radio Disabled event anyway.  */
        ev_rm_rd.ev_hdr.ev_type   = BT_EV_RM_RADIO_DISABLED;
        ev_rm_rd.ev_hdr.bt_app_id = bt_rm_data.disable_radio_app_id;
        ev_rm_rd.ev_msg.ev_rm_radio.disabled = bt_rm_data.disable_radio;
		bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;

        bt_ec_send_event( &ev_rm_rd );
      }
    }
    else
    {
      rm_dr_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }
}


/*===========================================================================

FUNCTION
  bt_rm_cmd_disable_discoverability

DESCRIPTION
  Processes a RM Disable Discoverability command.

===========================================================================*/
LOCAL void bt_rm_cmd_disable_discoverability
(
  bt_cmd_msg_type*  rm_dd_ptr
)
{

  boolean  new_disable_discoverability = FALSE;

  BT_MSG_API( "BT RM CMD RX: Disable Dscvblty %x AID %x",
              rm_dd_ptr->cmd_msg.cmd_rm_dsdcv.disable,
              rm_dd_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_dd_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( (bt_rm_data.disable_discoverability_app_id == BT_APP_ID_NULL) ||
         (rm_dd_ptr->cmd_hdr.bt_app_id ==
            bt_rm_data.disable_discoverability_app_id) )
    {
      if ( rm_dd_ptr->cmd_msg.cmd_rm_dsdcv.disable != FALSE )
      {
        new_disable_discoverability = TRUE;
      }

      if ( bt_rm_data.disable_discoverability !=
             new_disable_discoverability )
      {
        bt_rm_data.disable_discoverability = new_disable_discoverability;

        if ( bt_rm_data.disable_discoverability != FALSE )
        {
          bt_rm_data.disable_discoverability_app_id =
            rm_dd_ptr->cmd_hdr.bt_app_id;

          BT_MSG_HIGH( "BT RM: Dscvblty DISABLED AID %x",
                       bt_rm_data.disable_discoverability_app_id, 0, 0 );
        }
        else
        {
          bt_rm_data.disable_discoverability_app_id = BT_APP_ID_NULL;

          BT_MSG_HIGH( "BT RM: Dscvblty Enabled AID %x",
                       rm_dd_ptr->cmd_hdr.bt_app_id, 0, 0 );
        }

        bt_rm_check_scan_settings();
      }
      else
      {
        BT_MSG_DEBUG( "BT RM: Ignored Disable Dscvblty %x AID %x",
                      rm_dd_ptr->cmd_msg.cmd_rm_dsrad.disable,
                      rm_dd_ptr->cmd_hdr.bt_app_id, 0 );
      }
    }
    else
    {
      rm_dd_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_device_black_list

DESCRIPTION
  Initializes the RM Device Black List.

===========================================================================*/
LOCAL void bt_rm_init_device_black_list
(
  void
)
{

  uint16  i;

  /*  Initialize the Device Black List.  */
  bt_rm_data.dev_black_list_app_id = BT_APP_ID_NULL;
  bt_rm_data.dev_black_list_count  = 0;

  for ( i = 0; i < BT_RM_MAX_DEV_BLACK_LIST_COUNT; i++ )
  {
    memset( (void*)(bt_rm_data.dev_black_list[ i ].bd_addr_bytes), 0,
            sizeof( bt_bd_addr_type) );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_device_black_list

DESCRIPTION
  Processes a RM Set Device Black List command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_device_black_list
(
  bt_cmd_msg_type*  rm_bl_ptr
)
{

  uint16  i;

  BT_MSG_API( "BT RM CMD RX: Set Dev Black List C %x AID %x",
              rm_bl_ptr->cmd_msg.cmd_rm_setbl.dev_black_list_count,
              rm_bl_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_bl_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( (bt_rm_data.dev_black_list_app_id == BT_APP_ID_NULL) ||
         (rm_bl_ptr->cmd_hdr.bt_app_id ==
            bt_rm_data.dev_black_list_app_id) )
    {
      if ( rm_bl_ptr->cmd_msg.cmd_rm_setbl.dev_black_list_count <=
             BT_RM_MAX_DEV_BLACK_LIST_COUNT )
      {
        if ( rm_bl_ptr->cmd_msg.cmd_rm_setbl.dev_black_list_count > 0 )
        {
          bt_rm_init_device_black_list();

          bt_rm_data.dev_black_list_app_id = rm_bl_ptr->cmd_hdr.bt_app_id;
          bt_rm_data.dev_black_list_count  =
            rm_bl_ptr->cmd_msg.cmd_rm_setbl.dev_black_list_count;

          for ( i = 0; i < bt_rm_data.dev_black_list_count; i++ )
          {
            bt_rm_data.dev_black_list[ i ] =
              rm_bl_ptr->cmd_msg.cmd_rm_setbl.dev_black_list[ i ];
          }

          /*  Disconnect from any device just black listed.  */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
                 (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTING) &&
                 (bt_rm_device_in_black_list(
                    &bt_rm_conn[ i ].dev_ptr->dev_public.bd_addr ) != FALSE) )
            {
              bt_cmd_rm_disconnect_acl( bt_rm_conn[ i ].bt_app_id_acl,
                                        bt_rm_conn[ i ].rm_handle,
                                        BT_RMDR_USER_ENDED );
            }
          }
        }
        else
        {
          BT_MSG_HIGH( "BT RM: No Device Black List AID %x",
                       rm_bl_ptr->cmd_hdr.bt_app_id, 0, 0 );

          bt_rm_init_device_black_list();
        }
      }
      else
      {
        rm_bl_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
      }
    }
    else
    {
      rm_bl_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_init_device_white_list

DESCRIPTION
  Initializes the RM Device White List.

===========================================================================*/
LOCAL void bt_rm_init_device_white_list
(
  void
)
{

  uint16  i;

  /*  Initialize the Device White List.  */
  bt_rm_data.dev_white_list_app_id = BT_APP_ID_NULL;
  bt_rm_data.dev_white_list_count  = 0;

  for ( i = 0; i < BT_RM_MAX_DEV_WHITE_LIST_COUNT; i++ )
  {
    memset( (void*)(bt_rm_data.dev_white_list[ i ].bd_addr_bytes), 0,
            sizeof( bt_bd_addr_type) );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_cmd_set_device_white_list

DESCRIPTION
  Processes a RM Set Device White List command.

===========================================================================*/
LOCAL void bt_rm_cmd_set_device_white_list
(
  bt_cmd_msg_type*  rm_wl_ptr
)
{

  uint16  i;

  BT_MSG_API( "BT RM CMD RX: Set Dev White List C %x AID %x",
              rm_wl_ptr->cmd_msg.cmd_rm_setwl.dev_white_list_count,
              rm_wl_ptr->cmd_hdr.bt_app_id, 0 );

  if ( (rm_wl_ptr->cmd_hdr.cmd_status = bt_rm_not_testing()) ==
          BT_CS_GN_SUCCESS )
  {
    if ( (bt_rm_data.dev_white_list_app_id == BT_APP_ID_NULL) ||
         (rm_wl_ptr->cmd_hdr.bt_app_id ==
            bt_rm_data.dev_white_list_app_id) )
    {
      if ( rm_wl_ptr->cmd_msg.cmd_rm_setwl.dev_white_list_count <=
             BT_RM_MAX_DEV_WHITE_LIST_COUNT )
      {
        if ( rm_wl_ptr->cmd_msg.cmd_rm_setwl.dev_white_list_count > 0 )
        {
          bt_rm_init_device_white_list();

          bt_rm_data.dev_white_list_app_id = rm_wl_ptr->cmd_hdr.bt_app_id;
          bt_rm_data.dev_white_list_count  =
            rm_wl_ptr->cmd_msg.cmd_rm_setwl.dev_white_list_count;

          for ( i = 0; i < bt_rm_data.dev_white_list_count; i++ )
          {
            bt_rm_data.dev_white_list[ i ] =
              rm_wl_ptr->cmd_msg.cmd_rm_setwl.dev_white_list[ i ];
          }

          /*  Disconnect from any device not in the current white list.  */
          for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
          {
            if ( (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED) &&
                 (bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTING) &&
                 (bt_rm_device_in_white_list(
                    &bt_rm_conn[ i ].dev_ptr->dev_public.bd_addr ) == FALSE) )
            {
              bt_cmd_rm_disconnect_acl( bt_rm_conn[ i ].bt_app_id_acl,
                                        bt_rm_conn[ i ].rm_handle,
                                        BT_RMDR_USER_ENDED );
            }
          }
        }
        else
        {
          BT_MSG_HIGH( "BT RM: No Device White List AID %x",
                       rm_wl_ptr->cmd_hdr.bt_app_id, 0, 0 );

          bt_rm_init_device_white_list();
        }
      }
      else
      {
        rm_wl_ptr->cmd_hdr.cmd_status = BT_CS_RM_BAD_PARAM;
      }
    }
    else
    {
      rm_wl_ptr->cmd_hdr.cmd_status = BT_CS_RM_BUSY;
    }
  }

}

/*===========================================================================

FUNCTION
  bt_rm_process_cmd_radio_off

DESCRIPTION
  Processes a Radio Manager command when the BNT radio is disabled.

===========================================================================*/
LOCAL void bt_rm_process_cmd_radio_off
(
  bt_cmd_msg_type*  rm_cmd_ptr
)
{
  switch ( rm_cmd_ptr->cmd_hdr.cmd_type )
  {
    /*
     * The following group of commands are disallowed when radio is disabled.
     */
    case BT_CMD_RM_SET_LOCAL_INFO:
#ifdef BT_SWDEV_2_1_EIR
    /* EIR_TODO: short name when radio is off ? */
    case BT_CMD_RM_SET_LOCAL_INFO_EXT:
#endif /* BT_SWDEV_2_1_EIR */
    case BT_CMD_RM_CONFIG_APPLICATION:
    case BT_CMD_RM_SET_CONN_ROLE:
    case BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK:
#ifndef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_SET_CONN_SECURITY:
#endif /* BT_SWDEV_2_1_SSP */
    case BT_CMD_RM_ENFORCE_SEC_RFCOMM_SCN:
    case BT_CMD_RM_ENFORCE_SEC_L2CAP_PSM:
    case BT_CMD_RM_PIN_REPLY:
    case BT_CMD_RM_AUTHORIZE_REPLY:
    case BT_CMD_RM_TEST_MODE_ENABLE:
    case BT_CMD_RM_INQUIRE:
    case BT_CMD_RM_STOP_INQUIRY:
    case BT_CMD_RM_REMOTE_NAME_REQ:
    case BT_CMD_RM_REMOTE_NAME_REQ_CANCEL:
    case BT_CMD_RM_CONNECT_CANCEL:
    case BT_CMD_RM_BOND_CANCEL:
    case BT_CMD_RM_ACCEPT_ACL:
    case BT_CMD_RM_REJECT_ACL:
    case BT_CMD_RM_TX_DATA_ACL:
    case BT_CMD_RM_DISCONNECT_ACL:
    case BT_CMD_RM_ACCEPT_SCO:
    case BT_CMD_RM_REJECT_SCO:
#ifdef BT_SWDEV_RM_UNTESTED
    case BT_CMD_RM_TX_DATA_SCO:
#endif
    case BT_CMD_RM_DISCONNECT_SCO:
#ifdef FEATURE_BT_TELEC
    case BT_CMD_RM_START_TELEC_TEST:
#endif
#ifdef BT_SWDEV_RADIO_OFF
    case BT_CMD_RM_DISABLE_DISCOVERABILITY:
#endif
#ifdef FEATURE_BT_1_2
    case BT_CMD_RM_ENABLE_AFH:
    case BT_CMD_RM_SET_AFH_CH_MAP:
    case BT_CMD_RM_GET_AFH_CH_MAP:
#endif
#ifdef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_USER_CONFIRMATION_REPLY:
    case BT_CMD_RM_PASSKEY_REPLY:
    case BT_CMD_RM_KEYPRESS_NOTIFICATION:
    case BT_CMD_RM_WR_REMOTE_OOB_DATA:
    case BT_CMD_RM_CREATE_LOCAL_OOB_DATA:
#endif /* BT_SWDEV_2_1_SSP */
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_RADIO_OFF;
        break;
      }
     case BT_CMD_RM_UPDATE_BT_MODULE_ST:
      {
        bt_rm_cmd_update_bt_module_state( rm_cmd_ptr );
        break;
      }

    /*
     * For the following 3 disallowed commands, the corresponding failed event
     * needs to be sent
     */
#ifndef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_BOND:
#else
    case BT_CMD_RM_BOND_EXT:
    case BT_CMD_RM_AUTHORIZE_REBOND:
#endif /* BT_SWDEV_2_1_SSP */
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_RADIO_OFF;

        bt_rm_ev_send_bond_failed( rm_cmd_ptr->cmd_hdr.bt_app_id,
                                   &rm_cmd_ptr->cmd_msg.cmd_rm_bond.bd_addr,
                                   (bt_rm_handle_type)(-1),
                                   BT_EVR_RM_RADIO_OFF );
        break;
      }

    case BT_CMD_RM_CONNECT_ACL:
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_RADIO_OFF;

        bt_rm_ev_send_conn_bd_addr_failed(
          rm_cmd_ptr->cmd_hdr.bt_app_id,
          &rm_cmd_ptr->cmd_msg.cmd_rm_conna.bd_addr,
          BT_ACL_LINK,
          BT_EVR_RM_RADIO_OFF );
        break;
      }

    case BT_CMD_RM_CONNECT_SCO:
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_RADIO_OFF;

        bt_rm_ev_send_conn_bd_addr_failed(
          rm_cmd_ptr->cmd_hdr.bt_app_id,
          &rm_cmd_ptr->cmd_msg.cmd_rm_conns.bd_addr,
          BT_SCO_LINK,
          BT_EVR_RM_RADIO_OFF );
        break;
      }

#ifdef BT_SWDEV_2_1_EIR
    /* The Wr UUID and Wr EIR data just update the RM data structures in
       radio-off mode. The updated EIR is written to the SoC when the radio
       turned on */
    case BT_CMD_RM_WR_EIR_DATA:
      {
        bt_rm_cmd_wr_eir_data( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_WR_EIR_UUID:
      {
        bt_rm_cmd_wr_eir_uuid( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_EIR */

    /*
     * The set connectable and set discoverable commands are allowed. However,
     * only the RM data structures are updated. Scanning is not enabled until
     * radio is re-enabled.
     */
    case BT_CMD_RM_SET_CONNECTABLE:
      {
        bt_rm_cmd_set_connectable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DISCOVERABLE:
      {
        bt_rm_cmd_set_discoverable( rm_cmd_ptr );
        break;
      }

    /*
     * The following commands are allowed even if radio is disabled.
     */
    case BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE:
      {
        bt_rm_cmd_auto_service_search_disable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REG_LINK_STATUS:
      {
        bt_rm_cmd_register_link_status( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREG_LINK_STATUS:
      {
        bt_rm_cmd_unregister_link_status( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REG_RADIO_ACTIVITY:
      {
        bt_rm_cmd_register_radio_activity( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREG_RADIO_ACTIVITY:
      {
        bt_rm_cmd_unregister_radio_activity( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_LOW_POWER_CONFIG:
      {
        bt_rm_cmd_low_power_config( rm_cmd_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_SSR
    case BT_CMD_RM_LOW_POWER_CONFIG_EXT:
      {
        bt_rm_cmd_low_power_config_ext( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSR */

    case BT_CMD_RM_SET_DEVICE_SECURITY:
      {
        bt_rm_cmd_set_device_security( rm_cmd_ptr );
        break;
      }

#ifndef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_SET_SERVICE_SECURITY:
      {
        bt_rm_cmd_set_service_security( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSP */

    case BT_CMD_RM_SET_SERVICE_SEC_EXT:
      {
        bt_rm_cmd_set_service_sec_ext( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNBOND:
      {
        bt_rm_cmd_unbond( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_BONDABLE:
      {
        bt_rm_cmd_set_bondable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_ADD:
      {
        bt_rm_cmd_device_add( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_UPDATE:
      {
        bt_rm_cmd_device_update( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_SERVICES_UPDATE:
      {
        bt_rm_cmd_device_services_update( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_REMOVE:
      {
        bt_rm_cmd_device_remove( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_REMOVE_ALL:
      {
        bt_rm_cmd_device_remove_all( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REGISTER_ACL:
      {
        bt_rm_cmd_register_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREGISTER_ACL:
      {
        bt_rm_cmd_unregister_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REGISTER_SCO:
      {
        bt_rm_cmd_register_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREGISTER_SCO:
      {
        bt_rm_cmd_unregister_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DISABLE_RADIO:
      {
        bt_rm_cmd_disable_radio( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DEVICE_BLACK_LIST:
      {
        bt_rm_cmd_set_device_black_list( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DEVICE_WHITE_LIST:
      {
        bt_rm_cmd_set_device_white_list( rm_cmd_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_SET_IO_CAP:
      {
        bt_rm_cmd_set_io_cap( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_SM4:
      {
        bt_rm_cmd_set_sm4( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSP */

    default:
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;
        BT_ERR( "BT RM: Bad RX CMD Type %x AID %x",
                 rm_cmd_ptr->cmd_hdr.cmd_type,
                 rm_cmd_ptr->cmd_hdr.bt_app_id, 0 );
        break;
      }
  }
}
#endif /* BT_SWDEV_RADIO_OFF */


/*===========================================================================

FUNCTION
  bt_rm_process_cmd_radio_on

DESCRIPTION
  Processes a Radio Manager command when the BT radio is enabled.

===========================================================================*/
LOCAL void bt_rm_process_cmd_radio_on
(
  bt_cmd_msg_type*  rm_cmd_ptr
)
{

  switch ( rm_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_RM_UPDATE_BT_MODULE_ST:
      {
        bt_rm_cmd_update_bt_module_state( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_LOCAL_INFO:
      {
        bt_rm_cmd_set_local_info( rm_cmd_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_EIR
    case BT_CMD_RM_SET_LOCAL_INFO_EXT:
      {
        bt_rm_cmd_set_local_info_ext( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_WR_EIR_DATA:
      {
        bt_rm_cmd_wr_eir_data( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_WR_EIR_UUID:
      {
        bt_rm_cmd_wr_eir_uuid( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_EIR */

    case BT_CMD_RM_CONFIG_APPLICATION:
      {
        bt_rm_cmd_config_application( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_AUTO_SERVICE_SEARCH_DISABLE:
      {
        bt_rm_cmd_auto_service_search_disable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REG_LINK_STATUS:
      {
        bt_rm_cmd_register_link_status( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREG_LINK_STATUS:
      {
        bt_rm_cmd_unregister_link_status( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REG_RADIO_ACTIVITY:
      {
        bt_rm_cmd_register_radio_activity( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREG_RADIO_ACTIVITY:
      {
        bt_rm_cmd_unregister_radio_activity( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_LOW_POWER_CONFIG:
      {
        bt_rm_cmd_low_power_config( rm_cmd_ptr );
        break;
      }

#ifdef BT_SWDEV_2_1_SSR
    case BT_CMD_RM_LOW_POWER_CONFIG_EXT:
      {
        bt_rm_cmd_low_power_config_ext( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSR */

    case BT_CMD_RM_SET_CONN_ROLE:
      {
        bt_rm_cmd_set_connection_role( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_CONN_ROLE_SW_REQ_OK:
      {
        bt_rm_cmd_set_conn_role_sw_req_ok( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DEVICE_SECURITY:
      {
        bt_rm_cmd_set_device_security( rm_cmd_ptr );
        break;
      }

#ifndef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_SET_CONN_SECURITY:
      {
        bt_rm_cmd_set_connection_security( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_SERVICE_SECURITY:
      {
        bt_rm_cmd_set_service_security( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSP */
    case BT_CMD_RM_SET_SERVICE_SEC_EXT:
      {
        bt_rm_cmd_set_service_sec_ext( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_ENFORCE_SEC_RFCOMM_SCN:
      {
        bt_rm_cmd_enforce_security_rfcomm_scn( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_ENFORCE_SEC_L2CAP_PSM:
      {
        bt_rm_cmd_enforce_security_l2cap_psm( rm_cmd_ptr );
        break;
      }
#ifndef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_BOND:
      {
        bt_rm_cmd_bond( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSP */
    case BT_CMD_RM_BOND_CANCEL:
      {
        bt_rm_cmd_bond_cancel( rm_cmd_ptr );
        break;
      }
    case BT_CMD_RM_UNBOND:
      {
        bt_rm_cmd_unbond( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_BONDABLE:
      {
        bt_rm_cmd_set_bondable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_PIN_REPLY:
      {
        bt_rm_cmd_pin_reply( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_AUTHORIZE_REPLY:
      {
        bt_rm_cmd_authorize_reply( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_ADD:
      {
        bt_rm_cmd_device_add( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_UPDATE:
      {
        bt_rm_cmd_device_update( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_SERVICES_UPDATE:
      {
        bt_rm_cmd_device_services_update( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_REMOVE:
      {
        bt_rm_cmd_device_remove( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DEVICE_REMOVE_ALL:
      {
        bt_rm_cmd_device_remove_all( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_TEST_MODE_ENABLE:
      {
        bt_rm_cmd_test_mode_enable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_CONNECTABLE:
      {
        bt_rm_cmd_set_connectable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DISCOVERABLE:
      {
        bt_rm_cmd_set_discoverable( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_INQUIRE:
      {
        bt_rm_cmd_inquire( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_STOP_INQUIRY:
      {
        bt_rm_cmd_stop_inquiry( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REMOTE_NAME_REQ:
      {
        bt_rm_cmd_remote_name_req( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REMOTE_NAME_REQ_CANCEL:
      {
        bt_rm_cmd_remote_name_req_cancel( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_CONNECT_ACL:
      {
        bt_rm_cmd_connect_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_CONNECT_CANCEL:
      {
        bt_rm_cmd_connect_cancel( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REGISTER_ACL:
      {
        bt_rm_cmd_register_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_ACCEPT_ACL:
      {
        bt_rm_cmd_accept_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REJECT_ACL:
      {
        bt_rm_cmd_reject_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREGISTER_ACL:
      {
        bt_rm_cmd_unregister_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_TX_DATA_ACL:
      {
        bt_rm_cmd_tx_data_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DISCONNECT_ACL:
      {
        bt_rm_cmd_disconnect_acl( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_CONNECT_SCO:
      {
        bt_rm_cmd_connect_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REGISTER_SCO:
      {
        bt_rm_cmd_register_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_ACCEPT_SCO:
      {
        bt_rm_cmd_accept_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_REJECT_SCO:
      {
        bt_rm_cmd_reject_sco( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_UNREGISTER_SCO:
      {
        bt_rm_cmd_unregister_sco( rm_cmd_ptr );
        break;
      }
#ifdef BT_SWDEV_2_1_PB
    case BT_CMD_RM_SET_AUTO_FLUSH_TO:
      {
        bt_rm_cmd_set_auto_flush_to ( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_PB */
#ifdef BT_SWDEV_RM_UNTESTED
    case BT_CMD_RM_TX_DATA_SCO:
      {
        bt_rm_cmd_tx_data_sco( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_RM_UNTESTED */

    case BT_CMD_RM_DISCONNECT_SCO:
      {
        bt_rm_cmd_disconnect_sco( rm_cmd_ptr );
        break;
      }

#ifdef FEATURE_BT_TELEC
    case BT_CMD_RM_START_TELEC_TEST:
      {
        bt_rm_cmd_start_telec_test( rm_cmd_ptr );
        break;
      }
#endif

#ifdef BT_SWDEV_RADIO_OFF
    case BT_CMD_RM_DISABLE_RADIO:
      {
        bt_rm_cmd_disable_radio( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_DISABLE_DISCOVERABILITY:
      {
        bt_rm_cmd_disable_discoverability( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DEVICE_BLACK_LIST:
      {
        bt_rm_cmd_set_device_black_list( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_DEVICE_WHITE_LIST:
      {
        bt_rm_cmd_set_device_white_list( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_RADIO_OFF */

#ifdef BT_SWDEV_2_1_SSP
    case BT_CMD_RM_SET_IO_CAP:
      {
        bt_rm_cmd_set_io_cap( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_SET_SM4:
      {
        bt_rm_cmd_set_sm4( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_WR_REMOTE_OOB_DATA:
      {
        bt_rm_cmd_wr_remote_oob_data( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_USER_CONFIRMATION_REPLY:
      {
        bt_rm_cmd_user_confirmation_reply( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_PASSKEY_REPLY:
      {
        bt_rm_cmd_passkey_reply( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_KEYPRESS_NOTIFICATION:
      {
        bt_rm_cmd_keypress_notification( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_CREATE_LOCAL_OOB_DATA:
      {
        bt_rm_cmd_create_local_oob_data( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_BOND_EXT:
      {
        bt_rm_cmd_bond_ext( rm_cmd_ptr );
        break;
      }

    case BT_CMD_RM_AUTHORIZE_REBOND:
        {
          bt_rm_cmd_authorize_rebond( rm_cmd_ptr );
          break;
        }

    case BT_CMD_RM_SET_SM4_DEBUG_MODE:
      {
        bt_rm_cmd_set_sm4_debug_mode( rm_cmd_ptr );
        break;
      }
#endif /* BT_SWDEV_2_1_SSP */

    default:
      {
        rm_cmd_ptr->cmd_hdr.cmd_status = BT_CS_RM_UNRECOGNIZED_CMD;
        BT_ERR( "BT RM: Bad RX CMD Type %x AID %x",
                 rm_cmd_ptr->cmd_hdr.cmd_type,
                 rm_cmd_ptr->cmd_hdr.bt_app_id, 0 );
        break;
      }
  }

}


/*===========================================================================

FUNCTION
  bt_rm_disable_radio

DESCRIPTION
  Processes a Radio Manager command.

===========================================================================*/
void bt_rm_disable_radio
(
  bt_cmd_msg_type*  rm_dr_ptr
)
{
  bt_rm_cmd_disable_radio( rm_dr_ptr );
}


/*===========================================================================

FUNCTION
  bt_rm_process_command

DESCRIPTION
  Processes a Radio Manager command.

===========================================================================*/
void bt_rm_process_command
(
  bt_cmd_msg_type*  rm_cmd_ptr
)
{

#ifdef BT_SWDEV_RADIO_OFF
  if ( bt_dc.radio_state <= BT_RADIO_DISABLED )
  {
    bt_rm_process_cmd_radio_off( rm_cmd_ptr );
  }
  else
#endif /* BT_SWDEV_RADIO_OFF */
  {
    bt_rm_process_cmd_radio_on( rm_cmd_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_rm_powerup_init

DESCRIPTION
  Initialize the Radio Manager upon powerup.

===========================================================================*/
void bt_rm_powerup_init
(
  void
)
{

  uint8  i;

  bt_rm_app_id = bt_cmd_ec_get_application_id();
  ASSERT_DEBUG( bt_rm_app_id != BT_APP_ID_NULL );
  BT_MSG_API( "BT RM TRX: EC Get AID %x", bt_rm_app_id, 0, 0 );
  BT_MSG_HIGH( "BT RM AID is %x", bt_rm_app_id, 0, 0 );

  /*  Initialize the RM event queue.  */
  q_init( &bt_rm_ev_q );
  bt_rm_ev_q_info.event_q_ptr      = &bt_rm_ev_q;
  bt_rm_ev_q_info.max_q_depth      = BT_RM_NUM_EVENTS;
  bt_rm_ev_q_info.event_q_bit_mask = BT_EVQ_RM_B;
  bt_rm_ev_q_info.max_event_bytes  = bt_rm_get_max_event_bytes();

#ifdef BT_SWDEV_2_1_SSP
  bt_rm_data.sec_level = BT_RM_SL_1_LOW;
#endif /* BT_SWDEV_2_1_SSP */
  bt_rm_data.security = BT_SEC_NONE;

  bt_rm_data.min_enc_key_len_bytes = BT_RM_DEF_MIN_ENC_KEY_LEN_BYTES;

  bt_rm_data.test_mode_enabled = FALSE;

  bt_rm_data.telec_started  = FALSE;
  bt_rm_data.telec_transmit = FALSE;
  bt_rm_data.telec_hopping  = FALSE;

  bt_rm_data.accept_conn_role           = BT_ROLE_MASTER;
  bt_rm_data.create_conn_role_sw_req_ok = BT_ROLE_SWITCH_NOT_ALLOWED;
  bt_rm_data.bt_module_st_mask = 0;
  bt_rm_data.page_timeout = 0;

  bt_rm_init_inquiry_data();

  /*  Initialize the RM application table.  */
  for ( i = 0; i < BT_RM_MAX_APPS; i++ )
  {
    bt_rm_init_application( &bt_rm_app[ i ] );
  }

  /*  Initialize the RM low power configuration table.  */
  for ( i = 0; i < BT_RM_MAX_LPCS; i++ )
  {
    bt_rm_init_low_power_config( &bt_rm_lpc[ i ] );
  }

  /*  Initialize the RM link status registration table.  */
  for ( i = 0; i < BT_RM_MAX_LSRS; i++ )
  {
    bt_rm_init_link_status_reg( &bt_rm_lsr[ i ] );
  }

  /*  Initialize the RM UUID SCN pair table.  */
  for ( i = 0; i < BT_RM_MAX_UUID_SCN_PAIRS; i++ )
  {
    bt_rm_init_uuid_scn_pair( &bt_rm_uuid_scn[ i ] );
  }

  /*  Add defaults.  Changes here affect   */
  /*  code in bt_cmd_rm_uuid_scn_paired()  */
  /*  and bt_cmd_rm_uuid_scn_unpaired().   */
#ifdef FEATURE_BT_AG /* For MSM7500 bringup - Revisit */
  bt_rm_uuid_scn[ 0 ].sdp_uuid =
    BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY;
  bt_rm_uuid_scn[ 0 ].rfcomm_scn = BT_RC_SCN_AG_HS;

  bt_rm_uuid_scn[ 1 ].sdp_uuid =
    BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY;
  bt_rm_uuid_scn[ 1 ].rfcomm_scn = BT_RC_SCN_AG_HF;
#endif /* FEATURE_BT_AG */ /* For MSM7500 bringup - Revisit */

  bt_rm_uuid_scn[ 2 ].sdp_uuid =
    BT_SD_SERVICE_CLASS_DIALUP_NETWORKING;
  bt_rm_uuid_scn[ 2 ].rfcomm_scn = BT_RC_SCN_NA;


  /*  Initialize the RM service security table.  */
  for ( i = 0; i < BT_RM_MAX_SERV_SECS; i++ )
  {
    bt_rm_init_service_security( &bt_rm_ss[ i ] );
  }

  /*  Initialize the RM connection table.    */
  /*  This must be done before initializing  */
  /*  the RM device table in RAM.            */
  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    bt_rm_conn[ i ].rm_handle = (bt_rm_handle_type)(i);
    bt_rm_conn[ i ].dev_ptr   = NULL;
    bt_rm_init_connection( &bt_rm_conn[ i ] );
  }

  /*  Initialize the RM device and service data in RAM.  */
  /*  This must be done after the connection table has   */
  /*  been initialized.                                  */
  bt_rm_init_devices();

#ifdef BT_SWDEV_2_1_EIR
  /* Initialize Critical section */
  rex_init_crit_sect( &bt_rm_crit_sect );

  /* Initialize the heap to store EIR responses */
  mem_init_heap( &bt_rm_eir_heap, bt_rm_eir_heap_buf, BT_RM_EIR_HEAP_SIZE, NULL );

  /* Outgoing EIR data structure */
  bt_rm_update_eir_short_name( bt_efs_params.bt_name, FALSE );

  bt_rm_device_eir.tx_power_rfcal        = 0;
  bt_rm_device_eir.tx_power_level_valid  = FALSE;

  bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_16BIT_TYPE ]   = 0;
  bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_32BIT_TYPE ]   = 0;
  bt_rm_device_eir.uuid_list.size_uuid[ BT_UUID_128BIT_TYPE ]  = 0;
#endif /* BT_SWDEV_2_1_EIR */

#ifdef BT_SWDEV_2_1_SSP
  bt_rm_data.oob_valid = FALSE;
#endif /* BT_SWDEV_2_1_SSP */

#ifdef BT_SWDEV_RADIO_OFF
  bt_rm_data.disable_radio_app_id = BT_APP_ID_NULL;
  bt_rm_data.disable_radio        = FALSE;

  bt_rm_data.disable_discoverability_app_id = BT_APP_ID_NULL;
  bt_rm_data.disable_discoverability        = FALSE;

  bt_rm_init_device_black_list();

  bt_rm_init_device_white_list();
#endif

#ifndef FEATURE_BT_SOC
  bt_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                            BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#else
  bt_soc_etc_cmd_hc_wr_enc_key_len( bt_rm_data.min_enc_key_len_bytes,
                             BT_RM_DEF_MAX_ENC_KEY_LEN_BYTES );
#endif /* !FEATURE_BT_SOC */

#ifdef FEATURE_BT_SOC
  bt_rm_data.conf_page_scan_interval = BT_PAGESCAN_INTERVAL_HCI_DEF;
  bt_rm_data.conf_page_scan_window = BT_PAGESCAN_WINDOW_SOC;
  bt_rm_data.conf_inq_scan_interval = BT_INQUIRYSCAN_INTERVAL_HCI_DEF;
  bt_rm_data.conf_inq_scan_window = BT_INQUIRYSCAN_WINDOW_SOC;
#ifdef FEATURE_BT_1_2
  bt_rm_data.conf_inq_scan_type = BT_IS_STANDARD;
  bt_rm_data.conf_page_scan_type = BT_PS_STANDARD;
#endif /* FEATURE_BT_1_2 */
#endif /* FEATURE_BT_SOC */

  bt_rm_data.current_radio_activity = BT_RA_RADIO_IDLE;
}

/*===========================================================================

FUNCTION
  bt_rm_any_conn_active

DESCRIPTION
  Checks for active connections. Returns TRUE if any ACL links are up.

===========================================================================*/
boolean bt_rm_any_conn_active
(
  void
)
{
  uint16            i;
  boolean           active_connections = FALSE;

  // Look for active connections
  for ( i = 0; i < BT_MAX_CONNECTIONS; i++ )
  {
    if ( bt_rm_conn[ i ].state_acl != BT_RM_CS_DISCONNECTED )
    {
      active_connections = TRUE;
      break;
    }
  }
  return active_connections;
}

/*===========================================================================

FUNCTION
  bt_rm_pending_acl_tx_lpm_check

DESCRIPTION
  This function is called when num_complete ack has not been received from
  the controller for a "timeout" period for acl data that has been sent to
  the controller. This function will esnure the link is in the require state
  for data tx based idle link mode configuration.

===========================================================================*/

void bt_rm_pending_acl_tx_lpm_check
(
  uint16  hc_handle
)
{
  bt_rm_conn_type*    conn_ptr;
  if ( (conn_ptr = bt_rm_get_acl_conn_hc_handle( hc_handle )) != NULL )
  {
    /*  Unsniff or Unpark if necessary.  */
    bt_rm_acl_link_txrdy( conn_ptr );
  }
  else
  {
    BT_ERR( "BT RM: Invalid HCI handle %x",hc_handle,0,0 );
  }
}

#ifdef FEATURE_EFS
/*===========================================================================

FUNCTION
  bt_rm_read_service_file

DESCRIPTION
  Reads the Service files in the flash file system for a specified device.
  If present, the data read is used to update the services information
  in RAM.

OUTPUT
  success  TRUE if the file was found and read without error,
             FALSE otherwise.

============================================================================*/
LOCAL boolean bt_rm_read_service_file
(
  bt_rm_device_type*  dev_ptr
)
{

  boolean                 success = FALSE;
  boolean                 read_fault = FALSE;
  boolean                 bda_matched;
  fs_rsp_msg_type         fs_rsp_msg;
  fs_handle_type          fs_handle = FS_NULL_HANDLE;
  bt_rm_service_efs_type  efs_serv;
  uint8                   i;


  /*  Note that all fs_* functions below will block with rex_wait().  */

  bt_rm_set_service_file_name( dev_ptr );

  fs_open( service_file_name, FS_OA_READWRITE,
           NULL, NULL, &fs_rsp_msg );

  if ( dev_ptr->num_services > 0 )
  {
    if ( fs_rsp_msg.any.status == FS_OKAY_S )
    {
      fs_handle = fs_rsp_msg.open.handle;

      /* Initializing to make klocwork happy */
      memset( &efs_serv, 0, sizeof(bt_rm_service_efs_type) );

      fs_read( fs_handle, &efs_serv,
               sizeof( bt_rm_service_efs_type ),
               NULL, &fs_rsp_msg );

      if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
           (fs_rsp_msg.read.count == sizeof( bt_rm_service_efs_type )) )
      {
        bda_matched = BT_BD_ADDRS_EQUAL( &efs_serv.bd_addr,
                                         &dev_ptr->dev_public.bd_addr );
        if ( (efs_serv.database_version == BT_RM_SERV_DB_VERSION) &&
             (efs_serv.index == dev_ptr->index) &&
             (efs_serv.num_services == dev_ptr->num_services) &&
             (bda_matched != FALSE) )
        {
          for ( i = 0; i < BT_SD_MAX_NUM_OF_SRV_REC; i++ )
          {
            dev_ptr->service[ i ] = efs_serv.service[ i ];
          }
        }
        else
        {
          read_fault = TRUE;

          BT_ERR( "BT RM: Rd SERV read V %x I %x %x",
                  efs_serv.database_version,
                  efs_serv.index, dev_ptr->index );

          BT_ERR( "BT RM: Rd SERV read NS %x %x E %x",
                  efs_serv.num_services, dev_ptr->num_services, bda_matched );
        }
      }
      else
      {
        read_fault = TRUE;

        BT_ERR( "BT RM: Rd SERV read S %x C %x DI %x",
                fs_rsp_msg.any.status, fs_rsp_msg.read.count,
                dev_ptr->index );
      }
    }
    else
    {
      read_fault = TRUE;

      BT_ERR( "BT RM: Rd SERV open S %x NS %x DI %x",
              fs_rsp_msg.any.status,
              dev_ptr->num_services, dev_ptr->index );
    }
  }
  else if ( fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S )
  {
    success = TRUE;
  }
  else
  {
    read_fault = TRUE;

    BT_ERR( "BT RM: Rd SERV open S %x NS %x DI %x",
            fs_rsp_msg.any.status,
            dev_ptr->num_services, dev_ptr->index );
  }

  if ( fs_handle != FS_NULL_HANDLE )
  {
    fs_close( fs_handle, 0, &fs_rsp_msg );

    if ( fs_rsp_msg.any.status == FS_OKAY_S )
    {
      if ( read_fault == FALSE )
      {
        success = TRUE;
      }
    }
    else
    {
      BT_ERR( "BT RM: Rd SERV close S %x",
              fs_rsp_msg.any.status, 0, 0 );
    }
  }

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_rm_read_dev_and_serv_files

DESCRIPTION
  Reads the Device and Service files in the flash file system.  If
  present, the data read is used to update the RM device/services
  information in RAM.

OUTPUT
  success  TRUE if the file was found and read without error,
             FALSE otherwise.

============================================================================*/
LOCAL boolean bt_rm_read_dev_and_serv_files
(
  void
)
{

  boolean                success = FALSE;
  boolean                read_fault = FALSE;
  fs_rsp_msg_type        fs_rsp_msg;
  fs_handle_type         fs_handle = FS_NULL_HANDLE;
  uint8                  database_version = BT_RM_SERV_DB_VERSION;
  bt_rm_device_efs_type  efs_dev;
  bt_rm_device_type*     dev_ptr;
  uint16                 i;


  /*  Note that all fs_* functions below will block with rex_wait().  */

  fs_open( BT_RM_DEV_DB_NAME, FS_OA_READONLY, NULL, NULL, &fs_rsp_msg );

  if ( fs_rsp_msg.any.status == FS_OKAY_S )
  {
    fs_handle = fs_rsp_msg.open.handle;

    fs_read( fs_handle, &database_version,
             sizeof( uint8 ), NULL, &fs_rsp_msg );

    if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
         (fs_rsp_msg.read.count == sizeof( database_version )) &&
         (database_version == BT_RM_DEV_DB_VERSION) )
    {
#ifdef BT_SWDEV_2_1_SSP
      fs_read( fs_handle, &bt_rm_data.sec_level,
               sizeof( bt_sec_level_type ), NULL, &fs_rsp_msg );
      if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
           (fs_rsp_msg.read.count == sizeof( bt_sec_level_type )) )
#else
      fs_read( fs_handle, &bt_rm_data.security,
               sizeof( bt_security_type ), NULL, &fs_rsp_msg );

      if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
           (fs_rsp_msg.read.count == sizeof( bt_security_type )) )
#endif /* BT_SWDEV_2_1_SSP */
      {
        /*  Read in device information from Device Data Base file.  */

        i = 0;
        while ( (i < BT_RM_MAX_DEVICES) && (read_fault == FALSE) )
        {
          dev_ptr = &bt_rm_device[ i ];

          if ( i != dev_ptr->index )
          {
            BT_ERR( "BT RM: Rd DEV Bad DI %x %x",
                    dev_ptr->index, i, 0 );

            dev_ptr->index = i;
          }

          /* Initializing to make klocwork happy */
          memset( &efs_dev, 0, sizeof(bt_rm_device_efs_type) );

          fs_read( fs_handle, &efs_dev,
                   sizeof( bt_rm_device_efs_type ), NULL, &fs_rsp_msg );

          if ( (fs_rsp_msg.any.status == FS_OKAY_S) &&
               (fs_rsp_msg.read.count == sizeof( bt_rm_device_efs_type )) )
          {
            if ( (efs_dev.num_services <= BT_SD_MAX_NUM_OF_SRV_REC) &&
                 (efs_dev.index == i) )
            {
              if ( efs_dev.dev_public.valid != FALSE )
              {
                dev_ptr->dev_public         = efs_dev.dev_public;
                dev_ptr->page_scan_mode     = efs_dev.page_scan_mode;
                dev_ptr->page_scan_rep_mode = efs_dev.page_scan_rep_mode;
                dev_ptr->page_scan_per_mode = efs_dev.page_scan_per_mode;
                /* Ignore any clk_offset in permanent store */
                /* TBD: clk_offset should be removed from the list of params in efs */
                dev_ptr->clk_offset         = 0;
                dev_ptr->link_key_valid     = efs_dev.link_key_valid;
                dev_ptr->link_key           = efs_dev.link_key;
                dev_ptr->link_key_pin_len_bytes =
                  efs_dev.link_key_pin_len_bytes;
                dev_ptr->num_services       = efs_dev.num_services;
#ifdef BT_SWDEV_2_1_SSP
                dev_ptr->bonded_link_key    = efs_dev.bonded_link_key;
                dev_ptr->authenticated_link_key = efs_dev.authenticated_link_key;
                dev_ptr->eir_received       = efs_dev.eir_received;
                if (dev_ptr->eir_received == TRUE )
                {
                  SETBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
                }
                else
                {
                  CLRBIT( dev_ptr->dev_public.eir_data.eir_flag, BT_EIR_DATA_RCVD_B );
                }
#endif /* BT_SWDEV_2_1_SSP*/
#ifdef BT_SWDEV_2_1_EIR
                /* We dont store the manuf data or the UUIDs in EFS so we clear
                   the counts when we read these records from EFS */
                dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_16BIT_TYPE ]  = 0;
                dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_32BIT_TYPE ]  = 0;
                dev_ptr->dev_public.eir_data.num_uuids[ BT_UUID_128BIT_TYPE ] = 0;
                dev_ptr->dev_public.eir_data.manuf_data_size                  = 0;
#endif /* BT_SWDEV_2_1_EIR */
              }
            }
            else
            {
              read_fault = TRUE;

              BT_ERR( "BT RM: Rd DEV read NS %x DI %x %x",
                      efs_dev.num_services, efs_dev.index, i );
            }
          }
          else
          {
            read_fault = TRUE;

            BT_ERR( "BT RM: Rd DEV read S %x C %x DI %x",
                    fs_rsp_msg.any.status, fs_rsp_msg.read.count, i );
          }
          i++;
        }
      }
      else
      {
        read_fault = TRUE;

#ifdef BT_SWDEV_2_1_SSP
        BT_ERR( "BT RM: SSP Rd DEV Sec S %x C %x %x",
                fs_rsp_msg.any.status, fs_rsp_msg.read.count,
                sizeof( bt_sec_level_type ) );
#else
        BT_ERR( "BT RM: Rd DEV Sec S %x C %x %x",
                fs_rsp_msg.any.status, fs_rsp_msg.read.count,
                sizeof( bt_security_type ) );
#endif /* BT_SWDEV_2_1_SSP */
      }
    }
    else
    {
      read_fault = TRUE;

      BT_ERR( "BT RM: Rd DEV DB Ver %x S %x C %x",
              database_version, fs_rsp_msg.any.status,
              fs_rsp_msg.read.count );
    }

    fs_close( fs_handle, NULL, &fs_rsp_msg );

    if ( (fs_rsp_msg.any.status == FS_OKAY_S) && (read_fault == FALSE) )
    {
      /*  Read service file for each device.  */

      i = 0;
      success = TRUE;
      while ( (i < BT_RM_MAX_DEVICES) && (success != FALSE) )
      {
        success = bt_rm_read_service_file( &bt_rm_device[ i ] );

        i++;
      }
    }
    else
    {
      BT_ERR( "BT RM: Rd DEV close S %x RF %x",
              fs_rsp_msg.any.status, read_fault, 0 );
    }
  }
  else if ( fs_rsp_msg.any.status != FS_NONEXISTENT_FILE_S )
  {
    BT_ERR( "BT RM: Rd DEV open S %x",
            fs_rsp_msg.any.status, 0, 0 );
  }

  return ( success );

}
#endif /* FEATURE_EFS */


/*===========================================================================

FUNCTION
  bt_rm_efs_init

DESCRIPTION
  Initialize device and service information in RAM using data stored
  in flash file system, if present, or it will initialize those
  files in EFS as necessary.

===========================================================================*/
void bt_rm_efs_init
(
  void
)
{

#ifdef FEATURE_EFS
  uint16  i;

  /*  Read the device and service information from EFS.  */
  if ( bt_rm_read_dev_and_serv_files() == FALSE )
  {
    /*  Either the files don't exist, they are the wrong version,  */
    /*  or they have been corrupted.  (Re)initialize the device    */
    /*  and service files now.                                     */

    bt_rm_remove_dev_and_serv_files();

    bt_rm_init_devices();

    /*  Mark all services updated so service files are updated.  */
    for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
    {
      bt_rm_device[ i ].update_status = BT_RM_DUS_SERVICES_B;
    }

    if ( bt_rm_write_device_file() == FALSE )
    {
      BT_ERR( "BT RM: Bad Init Device DB!", 0, 0, 0 );
    }

    /*  Clear services updated marks made above.  */
    for ( i = 0; i < BT_RM_MAX_DEVICES; i++ )
    {
      bt_rm_device[ i ].update_status = BT_RM_DUS_NONE;
    }

  }

#ifdef BT_SWDEV_2_1_EIR
  /* MUST be called after bt_efs_params_init from bt_task_start */
  bt_rm_update_eir_short_name( bt_efs_params.bt_name, FALSE );
#endif /* BT_SWDEV_2_1_EIR */



#endif /* FEATURE_EFS */

}


#ifdef BT_SWDEV_2_1_EIR
#ifdef BT_SWDEV_2_1_NV
/*===========================================================================

FUNCTION
  bt_rm_read_tx_rfcal_from_nv

DESCRIPTION
  Read NV_BT_TX_RFCAL_I from non-volatile memory.

===========================================================================*/
LOCAL void bt_rm_read_tx_rfcal_from_nv
(
  void
)
{
  nv_cmd_type   rd_tx_rfcal_nv_cmd;
  nv_item_type  nv_item;

  /* Read RF Tx chain path loss from NV RAM*/
  rd_tx_rfcal_nv_cmd.item       = NV_BT_TX_RFCAL_I;
  rd_tx_rfcal_nv_cmd.cmd        = NV_READ_F;
  rd_tx_rfcal_nv_cmd.data_ptr   = &nv_item;
  rd_tx_rfcal_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_tx_rfcal_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_tx_rfcal_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_tx_rfcal_nv_cmd);
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_tx_rfcal_nv_cmd.status == NV_DONE_S )
  {
    bt_rm_device_eir.tx_power_rfcal = nv_item.bt_tx_rfcal;
    BT_MSG_DEBUG( "BT: NV_BT_TX_RFCAL_I: %d", nv_item.bt_tx_rfcal, 0, 0 );
  }
  else
  {
    BT_MSG_DEBUG( "BT: Unable to read NV_BT_TX_RFCAL_I", 0, 0, 0 );
  }
}
#endif /* BT_SWDEV_2_1_NV */
#endif /* BT_SWDEV_2_1_EIR */


#ifdef FEATURE_BT_2_1
#ifdef BT_SWDEV_2_1_NV
/*===========================================================================

FUNCTION
  bt_rm_nv_init

DESCRIPTION

===========================================================================*/
void bt_rm_nv_init
(
  void
)
{
#ifdef BT_SWDEV_2_1_EIR
  bt_rm_read_tx_rfcal_from_nv();
#endif /* BT_SWDEV_2_1_EIR */
}
#endif /* BT_SWDEV_2_1_NV */
#endif /* FEATURE_BT_2_1 */


//Add By zzg 2011_02_25
boolean bt_save_efs_params(void)
{

  boolean              success = FALSE;

#ifdef FEATURE_EFS
  boolean              write_fault = FALSE;
  fs_open_xparms_type  fs_open_xparms;
  fs_rsp_msg_type      fs_rsp_msg;
  fs_handle_type       fs_handle;
  
  fs_open(BT_EFS_PARAMS_FILE_NAME, FS_OA_READWRITE, NULL, NULL, &fs_rsp_msg);

  if (fs_rsp_msg.any.status == FS_NONEXISTENT_FILE_S)
  {
    BT_MSG_HIGH("BT: Creating EFSparams file", 0, 0, 0);

    fs_open_xparms.create.cleanup_option   = FS_OC_CLOSE;
    fs_open_xparms.create.buffering_option = FS_OB_ALLOW;
    fs_open_xparms.create.attribute_mask   = FS_FA_UNRESTRICTED;

    fs_open(BT_EFS_PARAMS_FILE_NAME, FS_OA_CREATE, &fs_open_xparms, NULL, &fs_rsp_msg);
  }

  if (fs_rsp_msg.any.status == FS_OKAY_S)
  {
    fs_handle = fs_rsp_msg.open.handle;

    fs_write(fs_handle, &bt_efs_params, sizeof(bt_efs_params_type), NULL, &fs_rsp_msg);

    if ((fs_rsp_msg.any.status != FS_OKAY_S) ||
         (fs_rsp_msg.write.count != sizeof(bt_efs_params_type)))
    {
      write_fault = TRUE;

      BT_ERR("BT: EFSparams Wr S %x C %x %x",
              fs_rsp_msg.any.status, fs_rsp_msg.write.count,
              sizeof(bt_efs_params_type));
    }
    
    fs_close(fs_handle, NULL, &fs_rsp_msg);

    if (fs_rsp_msg.any.status == FS_OKAY_S)
    {
      if (write_fault == FALSE)
      {
        success = TRUE;
      }
    }
    else
    {
      BT_ERR("BT RM: Wr EFSparams close S %x", fs_rsp_msg.any.status, 0, 0);
    }
  }
  else
  {
    BT_ERR("BT: EFSparams open S %x", fs_rsp_msg.any.status, 0, 0);
  }

#else /*  No EFS  */
  
  BT_ERR( "BT: Wr EFSparams NO EFS!", 0, 0, 0 );

#endif /* FEATURE_EFS */

  return ( success );

}


/*===========================================================================
FUNCTION
  bt_rm_refresh_efs_param

DESCRIPTION
===========================================================================*/
void bt_rm_refresh_efs_param
(
  void
)
{
	MSG_FATAL("***zzg bt_rm_refresh_efs_param***", 0, 0, 0);

	bt_efs_params.version          = BT_EFS_PARAMS_FILE_VER;
	bt_efs_params.ag_ad_mic_gain   = BT_AG_DEFAULT_HS_VOLUME_LEVEL;
	bt_efs_params.ag_ad_spkr_gain  = BT_AG_DEFAULT_HS_VOLUME_LEVEL;
	bt_efs_params.ag_idle_mode     = BT_AGIM_DEFAULT_IDLE_MODE;
	bt_efs_params.ag_idle_timeout  = BT_AG_DEFAULT_IDLE_TIMEOUT;
	bt_efs_params.ag_pref_dev_type = BT_AD_HEADSET;

	bt_efs_params.low_power_mode   = BT_LPM_SNIFF;

	bt_efs_params.cert_mode        = FALSE;
	bt_efs_params.park_supported   = TRUE;
	bt_efs_params.pair_type        = BT_PAIR_COMB;
#ifdef BT_SWDEV_2_1_SSP
	bt_efs_params.encrypt_mode     = BT_EM_ENABLED;
#else
	bt_efs_params.encrypt_mode     = BT_EM_POINT_TO_POINT;
#endif /* BT_SWDEV_2_1_SSP */
	bt_efs_params.toggle_seqn_bit  = FALSE;
	bt_efs_params.mode_to_start_in = BT_HCIM_OFF;

	BT_STRCPY( bt_efs_params.bt_name, DEFAULT_BT_NAME, 
	         sizeof(bt_efs_params.bt_name) );

#ifdef BT_SWDEV_2_1_EIR
	/* Use the first 'n' characeters of the default name as short name */

    
	BT_STRCPY( (char *)bt_efs_params.bt_short_name,
	         bt_efs_params.bt_name, 
	         (BT_MAX_EIR_NAME_LEN + 1) );

	bt_efs_params.eir_manuf_data_size = 0;
#endif /* BT_SWDEV_2_1_EIR */

	bt_save_efs_params();

}
//Add End

/*===========================================================================

FUNCTION
  bt_rm_soc_reset_shutdown

DESCRIPTION
  Prepare the Radio Manager for a SOC shutdown.

===========================================================================*/
void bt_rm_soc_reset_shutdown
(
  void
)
{
  bt_rm_data.page_timeout = 0;
}

/*===========================================================================

FUNCTION
  bt_rm_qdsp_ready_init

DESCRIPTION
  Prepare the Radio Manager when a Bluetooth QDSP image is loaded.

===========================================================================*/
void bt_rm_qdsp_ready_init
(
  void
)
{

}


/*===========================================================================

FUNCTION
  bt_rm_qdsp_reset_shutdown

DESCRIPTION
  Prepare the Radio Manager for a Bluetooth QDSP image unloaded.

===========================================================================*/
void bt_rm_qdsp_reset_shutdown
(
  void
)
{

}

/*===========================================================================

FUNCTION
  bt_rm_test_mode_enabled

DESCRIPTION
  Returns the state of test mode.

RETURN VALUES
  TRUE if test mode is enabled, FALSE if not in test mode.

===========================================================================*/
boolean bt_rm_test_mode_enabled
(
  void
)
{

  return bt_rm_data.test_mode_enabled;

}


/*===========================================================================

FUNCTION
  bt_rm_auth_failed

DESCRIPTION
  In case when RM is unaware of Authentication failure, this function is called to cleanup
  the pin_req_resp_app_id after sending the appropriate event to application.

RETURN VALUES
  None

===========================================================================*/
void bt_rm_auth_failed (bt_bd_addr_type* bd_addr)
{
  bt_rm_conn_type*  conn_ptr;
  bt_cmd_status_type status;

  status = bt_rm_get_conn_bd_addr (FALSE, FALSE, bd_addr, BT_ACL_LINK, &conn_ptr);
  if ((status == BT_CS_GN_SUCCESS) && (conn_ptr->pin_req_resp_app_id != BT_APP_ID_NULL))
  {
    bt_rm_ev_send_bond_failed (conn_ptr->pin_req_resp_app_id,
                            bd_addr,
                            conn_ptr->rm_handle, BT_EVR_RM_SECURITY_NOT_MET );
    conn_ptr->pin_req_resp_app_id = BT_APP_ID_NULL;
  }

}

#ifdef FEATURE_BT_TEST_API
#error code not present
#endif /* FEATURE_BT_TEST_API */

#endif /* FEATURE_BT */
