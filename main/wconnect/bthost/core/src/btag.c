/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    A U D I O    G A T E W A Y

                  A P P L I C A T I O N    M O D U L E

GENERAL DESCRIPTION
  This module contains the Audio Gateway application for Bluetooth (per
  the headset and hands-free profiles).

Copyright (c) 2000-2010 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btag.c#13 $ 
  $DateTime: 2010/09/17 02:16:00 $
  $Author: singhr $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-09-17   rs  Ensuring that bt_ag_process_spp_event does not send out BT_EV_AG_CONNECTION_FAILED
                   on SIO open error
  2010-01-19   co  Maintain call related info across connections in CDMA.
  2009-12-08   co  Properly update AUDIO_DEV in bt_ag_cmd_connect().
  2009-11-20   pn  Volume received from remote gets saved in AVS.
  2009-10-15   co  Added AT+CLCC support in CDMA.
  2009-09-24   pn  Added flow control to support sending large amount of data.
  2009-09-02   pn  Echoes remote speaker gain change not done by user app.
  2009-08-25   co  Added Emergency call support in AT+CLCC processing.
  2009-07-31   gb  Added support for FEATURE_IBT_DYNAMIC. 
  2009-04-14   pn  Watermark queues get initialized by dsm_queue_init().
  2009-03-05   co  The AG now generates RSSI updates internally.
  2009-01-13   rs  Moved the emptying of external I/O watermark queue functions 
                   to OEM.
  2008-10-31   co  Added multi-mode support to AT+CHUP processing.
  2008-10-30   co  AT+CHUP now ends the active call while waiting call exists.
  2008-10-06   co  Replaced deprecated functions.
  2008-08-14   rb  Added handling to keep the background timer tick during
                   power collapse
  2008-07-28   sa  Resolved compiler warning because of unused variables.
  2008-05-27   rb  Fix compiler warnings
  2008-04-09   co  Deprecated usage of CM_CALL_EVENT_EXIT_TC.
  2008-03-27   pn  Pacified Klocwork.
  2008-03-18   co  Merged data definitions from btagi.h. Shall stop using
                   btagi.h from now on.
                   Not to send data if AG's TX buffer high WM occurred.
 
 *
 *   #162         25 Feb 2008           CO
 * Fixed that AT+CHUP not processed well in conference calls on 1X.
 *
 *   #161         05 Dec 2007           CO
 * Upgraded AG's state mahcine by:
 *  - Adding BT_AGS_OPEN corresponding to SPP_ST_OPEN.
 *  - Replacing BT_AGS_OPENED with BT_AGS_CONNECTED.
 *
 *   #160         27 Nov 2007           PN
 * Sends DISCONNECTED event when incoming connection goes down due to
 * disabling.
 *
 *   #159         05 Sep 2007           CO
 * AG now processes AT+CLCC for UMTS only.
 * +CLCC now indicates held calls in a conference call as "multiparty calls".
 *
 *   #158         17 Aug 2007           CO
 * AG now accepts AT commands with redundant parameters.
 *
 *   #157         02 Aug 2007           CO
 * Cleared timer when accepting SCO requests from a remote.
 *
 *   #156         24 Jul 2007           CO
 * For UMTS, removed support of AT+CHLD=4 from +CHLD response.
 *
 *   #155         29 Jun 2007           CO
 * For UMTS, AG now retrieves the held call with AT+CHLD=1 in a single
 * call scenario.
 *
 *   #154         22 Jun 2007           CO
 * AG now no longer reconnects automatically.  In "Low Power" idle mode,
 * outgoing connections will always be attempted pre-defined MAX times.
 *
 *   #153         08 Jun 2007           CO
 * Featurized the previous change for UMTS only.
 *
 *   #152         08 Jun 2007           CO
 * Fixed that AG sends a redundant callheld indicator for AT+CHLD=2.
 *
 *   #151         15 May 2007           SSK
 * Fix compiler warnings associated with mixed usage of enum types. 
 *
 *   #150         11 May 2007           CO
 * Improved AT command parameter parsing and "ATD...;" processing.
 *
 *   #149         03 Apr 2007           CO
 * Fixed that AG sets idle timer when either SCO is being setup or
 * has been established.
 *
 *   #148         21 Mar 2007           CO
 * Fixed critical lint errors.
 *
 *   #147         13 Mar 2007           CO
 * Added support to HSP Errata 2112 feature "in-band ringing and RING
 * are mutually exclusive".
 *
 *   #146         09 Mar 2007           CO
 * Improved AT command parsing and processing.
 * Made code cleanup.
 *
 *   #145         27 Feb 2007           CO
 * Featurized AT+CLCC processing for HFP 1.5 only.
 * For UMTS: fixed bugs in AT+CLCC related handlings.
 *
 *   #144         16 Feb 2007           PN
 * Made use of safe string functions. Fixed critical lint errors.
 *
 *   #143         10 Jan 2007           CO
 * Fixed a compile error for 1X targets in previous check-in.
 * Optimized code in bt_ag_cpe_process_list_call_info().
 *
 *   #142         04 Jan 2007           CO
 * Added a module to store call info.  Relying on this module:
 *  - Added AT+CLCC command processing, instead of fowarding it to Externl
 *    I/O.
 *  - Fixed issue that AT+CHLD=1x/2x processing not working properly for
 *    UMTS.
 *
 *   #141         18 Dec 2006           CO
 * Improved error checking in bt_ag_cancel_connect().
 * Fixed issue that AG sent BT_EV_AG_CONNECTION_FAILED right away when
 * bt_ag_cancel_connect() was invoked.
 * 
 *   #140         13 Dec 2006           CO
 * Made code cleanup in bt_ag_process_cm_call_event() and
 * bt_ag_process_cm_call_event_sups().
 * Improved error checking for AT+CHLD processing (mostly for UMTS).
 * Fixed issue that bt_ag_process_cm_call_info_list() did not correctly
 * update call info indicators when invoked in the middle of some
 * call scenarios.
 * For UMTS:
 *  - Fixed issue that AG may send multiple call hold states to remote
 *    device during AT+CHLD=1/1x processing.
 *  - Fixed issue that AT+CHUP processing only hangs up one call when
 *    there exist two held calls.
 * 
 *   #139         13 Dec 2006           PN
 * Appropriate number type is now sent with +CCWA as well as +CLIP.
 * Phone number in CM events now gets NULL terminated as soon as they arrive.
 *
 *   #138         16 Nov 2006           RY
 * Initialize num_uuid128 to 0 prior to calls to bt_cmd_sd_search_service. 
 *
 *   #137         06 Nov 2006           CO
 * Fixed AG not responding to AT+BRSF=0 when SDP has been received.
 * 
 *   #136         30 Oct 2006           PN
 * Made sure phone number sent via +CLIP does not contain extraneous char.
 * Modified debug message logging to show actual data sent via +CIND.
 *
 *   #135         23 Oct 2006           JTL
 * Disconnect command now cancels a connect operation in progress.
 *
 *   #134         19 Oct 2006           CO
 * Fixed compilation errors introduced in last change.
 *
 *   #133         19 Oct 2006           CO
 * Fixed potential buffer overflows by using strncpy() instead of strcpy().
 * For UMTS targets:
 * - Fixed issues that fail to hang up outgoing calls with AT+CHUP and
 *   AT+CHLD=1/1x commands.
 * - Improved AT+CHLD=1/2 processing to cover the case with a held call and
 *   a waiting call.
 * - Improved AT+CHUP processing to hang up multi-party calls.
 * - Fixed flaws in AT+CHLD=1x/2x command processing.
 *
 *   #132         6 Oct 2006           PN
 * Valid number type is now sent with +CLIP for HFP 1.5.
 *
 *   #131         03 Oct 2006           PN
 * Feature bitmap received from SDP query no longer replaces that received in
 * AT+BRSF. SDP query is performed right after ACL up only for HFP 1.5 feature.
 * SCO request from HF is now rejected if SLC is not yet established.  All
 * state transition messages are now logged at HIGH level. General 
 * featurization simplification and code clean-up performed.
 *
 *   #130         25 Sep 2006           PN
 * AG now queries CM for SS and Phone info when HFP connection comes up.
 *
 *   #129         20 Sep 2006           CO
 * Modified AT+CHUP command processing in TWC scenarios according to
 * section 4.31.2 in HFP 1.5 spec.
 * Fixed issues on UMTS targets that AG cannot correctly process AT+CHLD=1x/2x
 * commands with x value larger than 9.
 *
 *   #128         18 Sep 2006           PN
 * Fixed comparison problem causing +BSIR to be sent unintentionally.
 *
 *   #127         15 Sep 2006           VK
 * Modified Registration of AG with CM. AG is registered with CM at AG enabling 
 * and de-registered at AG disabling.
 *
 *   #126         14 Sep 2006           CO
 * Fixed HFP1.0/1.5 inter-op issues, and the duplicate "call_setup"
 * indicator added in version #121 is removed.  Corresponding change was also
 * made in btagi.h.
 *
 *   #125         7 Sep 2006            PN
 * Modified support for inband ringing. Removed support for AT+BTRH. Improved
 * handling of AT+VTS in UMTS.  Modified handling of AT+CHLD=1 when there's 
 * only one call. Added handling for CM_CALL_EVENT_EXIT_TC.
 *
 *   #124         31 Aug 2006           JTL
 * Don't TX data if the SIO stream is closed. This is especially important
 * for external IO.
 *
 *   #123         30 Aug 2006           MP
 *  Fixed the buffer overflow problem in bt_ag_cmd_send_current_rh()
 *  properly
 *
 *   #122         16 Aug 2006           JH
 * Fixed compilation error.
 *
 *   #121         27 Jul 2006           CO
 * "call_setup" indicator is added as another form of the "callsetup" 
 * indicator.  Some HFP1.0/1.5 inter-op issues are handled by the amendment.
 * AG now accepts AT+CHLD = 1 sent by devices not setting the Multi-Party bit.
 *
 *   #120         20 Jul 2006           MP
 * Fixed potential buffer overflow problem in bt_ag_cmd_send_current_rh().
 *
 *   #119         7 Jul 2006            PN
 * AT+BLDN is now accepted in multi-party call scenario.  The unchanged call 
 * status is now sent when 2nd MO call becomes active.
 *
 *   #118        21 Jun 2006            PN
 * Fixed potential buffer overflow problem in bt_ag_cpe_send_response().
 *
 *   #117       13 Jun 2006            PN
 * Incoming call_setup indicator is now re-sent to car-kit if active call ended
 * by remote party before waiting call is answered/rejected.
 *
 *   #116        9 Jun 2006            PN
 * Placed the attributes in service search pattern in ascending order.
 *
 *   #115        9 Jun 2006            PN
 * Multiparty feature checks are now made against both AG and HF.
 *
 *   #114        6 Jun 2006            PN
 * Fixed AG timer management problem when scheduling ring alert.  Fixed problem
 * of UMTS waiting call not getting tracked when Call Waiting notification is 
 * not activated. Added missing reason code for AUDIO_DISCONNECTED event.
 *
 *   #113        1 Jun 2006            PN
 * Fixed problem with invalid +CHLD response being sent to HF when phone has
 * no service.  Added support for AT+CHLD=0 in 1X.
 *
 *   #112       16 May 2006            PN
 * Delays sending DEV_PICKUP event when waiting 1x call becomes new incoming
 * call to make sure app does not receive the event too early.  Also delays 
 * updating call setup state until call is answered.
 *
 *   #111       12 May 2006            JH
 * Removed unused variable to eliminate compiler warning.
 *
 *   #110        8 May 2006            PN
 * CONNECTION_FAILED event is no longer sent if connection initiated by remote
 * device gets rejected either by the app or by AG. Cleaned up Active mode.
 *
 *   #109        2 May 2006            PN
 * Memory Dialing and Redialing commands are handled differently for HFP 1.5.
 * Roaming status no longer equate to service availability.
 *
 *   #108       25 Apr 2006            PN
 * Fixed problem with Memory Dialing.
 *
 *   #107       11 Apr 2006            PN
 * "QC" removed from service name. CONNECTED event is sent if already connected
 * when receiving Connect command. Car-kit is now informed when held call ends.
 * No longer misinforms car-kit when a party in multi-party call hangs up.
 * Correctly processes CM_SS_EVENT_SRV_CHANGED. Audio Connect command is no
 * longer rejected during SLC bringup.
 *
 *   #106       11 Apr 2006            PN
 * CONNECT FAILED event is no longer sent for stream that is closing. Background
 * monitoring no longer sends failure events repeatedly. 
 *
 *   #105       14 Mar 2006            PN
 * Added support for attempting eSCO connection when HFP 1.5 is supported by
 * both local and remote devices; This involves requesting HFP version number
 * in servcie search and calling updated RM connect SCO API. 
 *
 *   #104        9 Mar 2006            JH
 * Eliminated compiler warning in 1x builds.
 *
 *   #103        3 Mar 2006            PN
 * AT+CHUP now only hangs up active call(s) in the multiparty call scenarios;
 * AT+CHLD=? is now responded with an empty list if HF does not support 
 * multiparty calls;  +CHLD and +BRSF now send matching info for HFP1.5;
 * Incoming call ID is now saved to handle the case multiple INCOM events 
 * from CM for same call. (CR 87759); Volume gains will only be sent to HF
 * if different; Certification test cases for multiparty calls in UMTS fixed;
 * Properly handles CM_INBAND_EVENT_REV_START_CONT_DTMF.
 *
 *   #102        16 Jan 2006            PN
 * Now comparing cm_client_id against CM_CLIENT_TYPE_BLUETOOTH instead of
 * CM_CLIENT_ID_ANONYMOUS; +CLIP is no longer sent without accompanying RING.
 *
 *   #101        9  Jan 2006            PN
 * Featurized old CM compatible code with FEATURE_BT_WITH_OLD_CM; Removed
 * references of bt_cmd_sd_register_services().
 *
 *   #100        05 Dec 2005            DSN
 * Fixed RVCT compile error. 
 *
 *    #99        6  Dec 2005            PN
 * Fixed more problems in UMTS 3-way call scenarios in which active call ends;
 * Fixed problem of Audio Connection command being dropped if currently bringing
 * up Audio connection; Fixed problem of accepting SCO request from HF before
 * SLC is established; Added code to deregister from CM when connection brought
 * up by remote device goes down; Changed the way CM_SS_EVENT_SRV_CHANGED is
 * processed.
 *
 *    #98        30 Nov 2005            PN
 * State variables properly updated when active UMTS call ends and waiting call
 * remains.  AT+CHLD=1x and AT+CHLD=2x are no longer rejected in HFP 1.5.
 *
 *    #97        21 Nov 2005            PN
 * Changed Roaming values to be HFP 1.5 specs compliant. Fixed problem of not
 * deregistering with CM. Further improved multi-party call control for 1X.
 *
 *    #96        9  Nov 2005            DP
 * AG now uses new SD service registration command so as to not
 * be connectable solely for the SD service registration.
 *
 *    #95        09 Nov 2005            PN
 * Improved handling of AT+CHLD=1 and AT+CHLD=2 for 1X.
 *
 *    #94        27 Oct 2005            PN
 * Replaced BT_SWDEV_HFP_1_5 with FEATURE_BT_HFP_1_5.
 *
 *    #93        24 Oct 2005            PN
 * Added code to save call ID when processing CM call event INCOM.
 *
 *    #92        24 Oct 2005            PN
 * Added code to save call ID when processing CM call events ORIG, IS_WAITING,
 * and RETRIEVED.
 *
 *    #91        19 Oct 2005            PN
 * Fixed problem of declaring SLC too soon when connected to HFP 0.96 device.
 *
 *    #90        17 Oct 2005            PN
 * HFP 1.5 indicators are only supported with BT_SWDEV_HFP_1_5 enabled.  Call
 * id is now tracked to keep call state accurate.  Compile warning for Audio 
 * Debug removed.
 *
 *    #89        12 Oct 2005            PN
 * AG no longer tries to close a stream more than once.
 *
 *    #88        30 Sep 2005            PN
 * SD registration is now done regardless of server/client role.
 *
 *    #87        21 Sep 2005            PN
 * Removed definition of BT_SWDEV_HFP_1_5.
 *
 *    #86        16 Sep 2005            PN
 * Default ring period is now set to 2 seconds (=8*0.25). Info is retrieved for 
 * each active voice call to get its associated phone number. HFP 1.5 indicators
 * now retain their values between connections.
 *
 *    #85        14 Sep 2005            PN
 * Enabled BT_SWDEV_HFP_1_5. Added code to clear timer after SLC comes up.
 *
 *    #84        31 Aug 2005            PN
 * Added support for backward compatibility to MSM6500 R4.1 P6.  Added support
 * for enhanced call control in UMTS.  Only send new battery charge ind and RSSI
 * if SLC is up. Connected call ID is now used in calls to cm_mm_call_cmd_sups().
 *
 *    #83        25 Aug 2005            PN
 * Mainlined processing of AT+BTRH.  Added processing of AT+CMEE.  Added HFP1.5
 * features bitmap.
 *
 *    #82        18 Aug 2005            PN
 * Feature bitmap sent via +BRSF now indicates capability of rejecting call.
 * RINGs now also get sent even when audio is connected.
 *
 *    #81        3  Aug 2005            PN
 * Fixed problem due to BREW External I/O watermark callback being called with 
 * TASKLOCK.
 *
 *    #80        1  Aug 2005            PN
 * Added support for 'callheld' indicator. Mainlined battery charge, RSSI, and 
 * roaming indicators.  Changed CM call info query method.
 *
 *    #79        20 Jun 2005            PN
 * AG now sends new call and call setup status to car-kit regardless of audio 
 * connection status.
 *
 *    #78        7  Jun 2005            PN
 * Correctly determines whether to reset timer after sending response to HS/HF.
 * Audio Connect command is rejected if no remote BD address is available.
 *
 *    #77        1  Jun 2005            PN
 * Improved background monitoring.  Correctly parses Serving System Info event 
 * from CM.  When disconnected from car-kit while operating in Passive mode, 
 * SLC down event now gets sent to app.
 *
 *    #76        26 May 2005            PN
 * Call number from CM_CALL_EVENT_CALLER_ID is now unconditionally saved.
 *
 *    #75        13 May 2005            PN
 * No longer sends RING to car-kit once call has been answered.
 *
 *    #74        6  May 2005            DP
 * Ignore characters prior to AT at start of AT command messages.
 *
 *    #73        29 Apr 2005            PN
 * Audio Connect command will now be rejected if SLC is not complete.  AG now
 * remembers if it was enabled with NULL BD address after initiating connection
 * to remote device.
 *
 *    #72        12 Apr 2005            PN
 * Unused code removed.  Different CM events are handled for CDMA multi-party 
 * calls. Comands and events are now processed based on serving system, not on 
 * defined features.
 *
 *    #71        12 Apr 2005            GS
 * Don't cancel AG timer when BT QDSP image is unloaded.
 *
 *    #70         8 Apr 2005            DP
 * Make use of BT_BDA debug message macro.
 *
 *    #69        28 Mar 2005            PN
 * AG now properly handles page fail situation when idle mode is ACTIVE.
 *
 *    #68        23 Mar 2005            PN
 * No longer sends call/call setup status right after SLC comes up. No longer
 * accepts ring commands from app if HS/HF's BD address is not available. No
 * longer expects CHLD test command from version 0.96 HFP car-kits.
 *
 *    #67        18 Mar 2005            PN
 * AG now waits at least 1 second between rings sent to HS/HF.  Improved multi-
 * party call handling.
 *
 *    #66        16 Mar 2005            DP
 * Increased don't exceed counts on watermarks required by improved
 * Credit Based Flow Control on RFCOMM.
 *
 *    #65        2  Mar 2005            PN
 * Fixed problem processing CM call events.
 *
 *    #64        24 Feb 2005            PN
 * Phone number in +CCWA is now in double quotes.  MSG_API added for CM inband
 * commands.  AT+CHLD=0 is no longer rejected.  For MO calls, +CIEV: 3,3 is now 
 * sent only after SCO link is up.  Call events for CS Data calls now get
 * processed by AG.
 *
 *    #63        7  Feb 2005            PN
 * App will no longer receive Connection Failed event if connection is initiated
 * by remote device.  Put back code that clears idle timer at SCO connection up.
 *
 *    #62        27 Jan 2005            PN
 * Added support for audio debug.
 *
 *    #61        13 Dec 2004            PN
 * AG now updates call and call setup states only for voice calls.
 *
 *    #60        3  Dec 2004            PN
 * Added double quotes around phone number accompanying +CLIP.  In Active mode,
 * AG no longer reconnects to remote device if app initiates disconnection.
 * CONNECT_FAILED event is now sent when connect command fails. No longer cleans
 * up in bt_ag_qdsp_reset_shutdown(). Fixed race condition when timer expires 
 * coincides with receiving ring_audio_device command.
 *
 *    #59        30 Nov 2004            PN
 * AUDIO_CONNECTED event generation is delayed when audio is already connected.
 * When SLC comes up, call/call setup status are sent to car-kit if necessary.
 * AG now rejects ATA when there's no incoming call present.  Limited service
 * is now considered no service.
 *
 *    #58        22 Nov 2004            PN
 * Depending on GSM/WCDMA or CDMA, different CM events are used as to determine
 * whether remote party has been reached in MO calls. Removed from AG feature 
 * bitmap the BT_AG_F_CALL_REJECT_ABLE bit.
 *
 *    #57        17 Nov 2004            PN
 * Dialed string length is now checked.  DISABLED and ENABLED events are now
 * sent after RM commands are done. No longer send extraneous SLC_CHANGED event.
 * CALL_CONNECT event from CM is now used as signal that remote party is reached.
 *
 *    #56        23 Sep 2004            PN
 * Fixed problem of timer being reset inadvertently.
 *
 *    #55        20 Sep 2004            PN
 * Resets timer before performing all pending commands.  Only sends +CLIP 
 * after every RING.  Ignores subsequent AUDIO_CONNECT command if already 
 * in process of bringing up audio link.
 *
 *    #54        26 Aug 2004            PN
 * Fixed problem of partial processing of AUDIO_CONNECT, AUDIO_DISCONNECT, and 
 * RING_AUDIO_DEVICE commands.  Fixed problem of NULL pointer access in 
 * bt_ag_cpe_send_response().  Reset timer after SLC comes up.  Now expects
 * SPP_DISCONNECTED when closing a connected client stream.
 *
 *    #53        26 Aug 2004            PN
 * Fixed BT_AG_HF_EXTENSION featurization problem.
 *
 *    #52        25 Aug 2004            DP
 * Added battery charge, receive signal strength, and roam indicators to
 * AG.  Removed spaces from VGS and VGM responses.  Fixed volume gain
 * commands and a debug message.
 *
 *    #51        28 Jul 2004            DP
 * Really don't convert characters to upper case now.
 *
 *    #50        13 Jul 2004            DP
 * No longer convert characters received from audio device to upper
 * case.  Implement flow control across external I/O and AG transmit
 * watermarks.
 *
 *    #49        8  Jul 2004            PN
 * Fixed format for +CHLD response.  Added DM message when sending SLC_CHANGE 
 * event.  Changed DM message in bt_ag_cpe_process_dial().  Made use of 
 * CPE_PARAM_0.
 *
 *    #48        2  Jul 2004            DP
 * Make features supported bit consistent, allow CMER always, ignore
 * all spaces in parameter list, send connection failed event.
 *
 *    #47        1  Jun 2004            DP
 * Implemented AG External I/O feature.  Removed predeclarations at top
 * of file and cleaned up headers and comments resulting in heavy code
 * rearrangement.
 *
 *    #46        5  May 2004            PN
 * Fixed problem which caused AG to mistakenly disconnect link to HS/HF when 
 * commanded to stop sending RING signals to HS/HF.
 *
 *    #45        12 Apr 2004            PN
 * - Added support for AT+CPBR=?
 * - Added utility function bt_ag_pap2cme_err()
 *
 *    #44        8  Apr 2004            PN
 * - AG will now wait until all opened SIO streams are closed before opening 
 *   new ones.
 * - Fixed potential problem of client SIO stream not getting closed in the
 *   process of disabling AG.
 * - Added background monitoring for AG.
 *
 *    #43        26 Mar 2004            PN
 * Fixed problem associated with Disconnect command.
 *
 *    #42        25 Mar 2004            PN
 * - removed critical lint errors
 * - AT command parser now correctly handles different flavors of 
 *   AT+CHLD, AT+CPBS, and AT+BTRH
 * - added support for AT+CPBF and AT+CPBW (BT_AG_HF_EXTENSION)
 * - fixed BT_AG_HF_EXTENSION bugs
 *
 *    #41        16 Mar 2004            PN
 * - SDP services are now registered only for server streams
 * - SDP services are now unregistered when the stream(s) are closed
 * - event reasons are now propagated from lower layer (SR314023)
 * - AG now correctly resumes idle state after a connection goes down
 *
 *    #40        3  Mar 2004            PN
 * Fixed problem that prevented connection with HS after previous connections
 * with HF and vice versa when AG was enabled with no BD address.
 *
 *    #39        20 Feb 2004            PN
 * - Introduced BT_AGS_CLOSING to fix bug in bt_cmd_ag_disconnect().
 * - Fixed a few Lint errors.
 *
 *    #38        17 Feb 2004            PN
 * Fixed problem of failure to disable AG when disable command came as 
 * max connection attempts reached.
 *
 *    #37        6  Feb 2004            PN
 * - further improved AG disabling process
 * - removed unused flag bt_ag.ringing
 * - AG will now reset timer if idle timeout value changes
 * - AG will now accept spaces in AT command parameters
 * - AG will no longer reject abnormal dialed strings
 * - AG will now log correct info when processing AT+BRSF
 * - AG will now accept AT+CIND? any time
 *
 *    #36        5  Feb 2004            PN
 * - added code to enable/disable VR capability in AG
 * - removed bug that caused ACL link to go down after a call ended when a 
 *   certain condition was met
 * - AG now sends call waiting response to HF even if no caller ID is available
 *
 *    #35        29 Jan 2004            PN
 * AG no longer intercepts call hangup command from car-kit.
 *
 *    #34        28 Jan 2004            PN
 * AG now properly handles idle timeout of 0 seconds.
 *
 *    #33        27 Jan 2004            PN
 * - removed redundant check in bt_ag_process_call_hangup_cmd()
 * - when disabling AG, all appropriate events get sent back to apps (instead 
 *   of only AG_DISABLED event)
 *
 *    #32        22 Jan 2004            PN
 * - utilized disconnect reason to decide whether to force server mode
 * - made sure SLC and an audio link were up before generating DTMF tone
 * - set a time limit for SLC establishment
 *
 *    #31        20 Jan 2004            PN
 * - Un-featurized FEATURE_BT_HF
 *
 *    #30        20 Jan 2004            PN
 * - AG will now register service with SD rather than letting SIO do this
 *
 *    #29        12 Jan 2004            PN
 * - added support for VR activation by AG
 * - modified bt_ag_send_response() to print more descriptive debug messages
 * - CM registration is now done when SPP connection to HF device happens
 * - CM deregistration is now also done whenever HF stream is closed
 *
 *    #28        18 Dec 2003            PN
 * - replaced ASSERTs with BT_ERR()s
 * - fixed bug that caused AG to open a client stream without closing server 
 *   stream(s)
 * - AUDIO_DISCONNECTED event will now be sent to the app that requested audio
 *   connection instead of the UI app
 * - DEV_HANGUP event will now be sent regardless of call state
 *
 *    #27        5  Dec 2003            PN
 * Changed bt_ag_check_rx_wm_data() to process events before data.
 *
 *    #26        3  Dec 2003            PN
 * - made sure all open streams get closed before opening another.
 * - fixed the problem with AG_CONNECTED event being sent erroneously when 
 *   server stream got disconnected.
 *
 *    #25        14 Nov 2003            PN
 * Added code to handle commands BT_CMD_AG_CONNECT and BT_CMD_AG_DISCONNECT.
 *
 *    #24        14 Nov 2003            DP
 * Provide audio device in connected event.
 *
 *    #23        4  Nov 2003            PN
 * - Added back the code that was inadvertently removed in revision #22.
 * - Made some of the debug messages more descriptive.
 *
 *    #22        2  Oct 2003            RY
 * Added the support to accept connection from any audio device.
 *
 *    #21        26 Sep 2003            DP
 * Updated AG connect/disconnect and VR capable APIs.
 *
 *    #20        23 Sep 2003            RY
 * Updated to use CM_CLIENT_TYPE_BLUETOOTH.
 *
 *    #19         26 Aug 2003           PN
 * Made use of input params max_rings and ring_period of 
 * bt_cmd_ag_ring_audio_device().
 *
 *    #18        25 Aug 2003            RY
 * Updated bt_ag_process_vr_activation_cmd to correctly handle VR deactivation.
 *
 *    #17         02 Jul 2003           PN
 * - Fixed bug in parsing AT command parameters
 * - Fixed bug in building event messages for DEV_DIAL and MEM_DIAL events.
 * - Fixed bug in building debug message in bt_ag_process_cm_call_event().
 * - Removed phone info request from bt_ag_cm_reg().
 *
 *    #16         20 Jun 2003           PN
 * Replaced CM_CLIENT_TYPE_FLOATING3 with CM_CLIENT_TYPE_FLOATING4.
 *
 *    #15         12 Jun 2003           PN
 * - Added code to handle ATD>n;
 * - Added code to send BT_EV_AG_DEV_SEND_DTMF.
 * - Send BT_EV_AG_SLC_CHANGED to reflect change in SLC status.
 * - Added bt_ag_cmd_set_inband_ring().
 * - Added support for BT_AG_HF_EXTENSION.
 * - General code clean up.
 *
 *    #14         12 May 2003           PN
 * - Modified AG to ignore AT+CHUP when audio link to HF is down during a call.
 * - Removed old edit history (dated Sept 2002 and earlier) because AG was 
 *   completely rewritten in Oct 2002.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_AG
#include <stdio.h>
#include <string.h>
#include "rex.h"
#include "cm.h"
#include "snd.h"

#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btag.h"
#include "btsd.h"
#include "btutils.h"

#ifdef BT_SWDEV_QXDM_LOG_PER
#include "btlogi.h"
#include "log.h"
#endif

#include "task.h"
#include "err.h"
#include "btmsg.h"

#define BT_MSG_LAYER  BT_MSG_AG   /* necessary for btmsg.h */

#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */

LOCAL boolean bt_ag_cm_reg(void);
LOCAL void    bt_ag_cancel_connect(void);

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                        Data Definitions                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define BT_AG_SERVICE_NAME    "Voice Gateway"
#define BT_AG_HS_PROFILE_VERSION_NUMBER   0x0100
#define HFP_VER_1_0                       0x0101
#define HFP_VER_1_5                       0x0105
#define HFP_VER_1_2                       0x0102 /* pre adoption version */
#ifdef FEATURE_BT_HFP_1_5
#define BT_AG_HF_PROFILE_VERSION_NUMBER   HFP_VER_1_5
#else
#define BT_AG_HF_PROFILE_VERSION_NUMBER   HFP_VER_1_0
#endif /* FEATURE_BT_HFP_1_5 */

#define BT_AG_DEFAULT_RING_PERIOD              8
#define BT_AG_MIN_HS_VOLUME_LEVEL              0
#define BT_AG_MAX_HS_VOLUME_LEVEL             15
#define BT_AG_MAX_NUM_BT_EVENTS               10  /* queue depth */
#define BT_AG_MAX_CONNECTION_ATTEMPTS          5
#define BT_AG_SLC_SETUP_TO            1000*60*60  /* SLC should be set up 
                                                     within this time */

#define BT_AG_RESP_PREFIX                 "\r\n"
#define BT_AG_RESP_POSTFIX                "\r\n"
#define BT_AG_RESP_MAX_LEN                    64
#define BT_AG_RESP_NAME_MAX_LEN               12

#define BT_AG_INVALID_SCO_HANDLE            (-1)

#define BT_HF_F_ECNR_ON               0x00000001
#define BT_HF_F_MULTIPARTY_CALL       0x00000002
#define BT_HF_F_CLI_PRESENTATION      0x00000004
#define BT_HF_F_VR_ACTIVATION         0x00000008
#define BT_HF_F_REMOTE_VOL_CTRL       0x00000010
#define BT_HF_F_ENH_CALL_STATUS       0x00000020
#define BT_HF_F_ENH_CALL_CONTROL      0x00000040
#ifdef FEATURE_BT_HFP_1_5
/* HFP 1.5 extended feature bitmap for HF */
#define BT_HF_HFP_1_5_EXT_FEATURES  ( BT_HF_F_ENH_CALL_STATUS | \
                                      BT_HF_F_ENH_CALL_CONTROL )
#endif /* FEATURE_BT_HFP_1_5 */


#define BT_AG_F_MULTIPARTY_CALL       0x00000001
#define BT_AG_F_ECNR_FUNC             0x00000002
#define BT_AG_F_VR_ON                 0x00000004
#define BT_AG_F_IN_BAND_RING_TONE     0x00000008
#define BT_AG_F_VOICE_TAG             0x00000010
#define BT_AG_F_CALL_REJECT_ABLE      0x00000020
#define BT_AG_F_ENH_CALL_STATUS       0x00000040
#define BT_AG_F_ENH_CALL_CONTROL      0x00000080
#define BT_AG_F_EXTENDED_ERR_CODE     0x00000100

#define BT_AG_SS_TIMEOUT_MS 35500  /*  AG Steady State timer.  */

#define MULTIPARTY_SUPPORTED() (                          \
  ((HF_DATA.features_bitmap & BT_HF_F_MULTIPARTY_CALL) == \
   BT_HF_F_MULTIPARTY_CALL) &&                            \
  ((bt_ag.features_bitmap & BT_AG_F_MULTIPARTY_CALL) ==   \
   BT_AG_F_MULTIPARTY_CALL) )

#define IS_VOICE_CALL( t ) ( (t == CM_CALL_TYPE_VOICE)     || \
                             (t == CM_CALL_TYPE_CS_DATA)   || \
                             (t == CM_CALL_TYPE_EMERGENCY) || \
                             (t == CM_CALL_TYPE_STD_OTASP) || \
                             (t == CM_CALL_TYPE_NON_STD_OTASP) )

#define BT_AG_IN_CDMA ( (HF_DATA.ss_info.sys_mode == SYS_SYS_MODE_CDMA)  || \
                        (HF_DATA.ss_info.sys_mode == SYS_SYS_MODE_HDR)   || \
                        (HF_DATA.ss_info.sys_mode == SYS_SYS_MODE_AMPS) )
#define BT_AG_IN_GW   ( (HF_DATA.ss_info.sys_mode == SYS_SYS_MODE_GSM)   || \
                        (HF_DATA.ss_info.sys_mode == SYS_SYS_MODE_WCDMA) )

#define IS_DIGIT( x ) ( ((x-'0') >= 0) && ((x-'0') <= 9) )
#define TO_UPPER( x ) (( x >= 'a' && x <= 'z' ) ? ( x - 0x20 ) : x )

#define STREAM_CLOSING_B   0x80
/* if a certain stream is being closed */
#define IS_STREAM_CLOSING( stream_id ) (    \
  ((stream_id) != SIO_NO_STREAM_ID) &&      \
  ISBITSET (stream_id, STREAM_CLOSING_B) )


/* flag bits used in communication between BRSF/CIND=? processing
** and SDP response processing (Success, Error/Timeout).
*/
#define SDP_RESP_B          0x0001
#define BRSF_B              0x0002
#define INBAND_B            0x0004
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/* Wait this amount of time, then assume waiting call has ended.
   This is added because there are no signals/events in 1X to alert AG when
   remote party hangs up the waiting call. */
#define BT_AG_1X_CW_TIMEOUT (1000 * 60) /* 1 minute should be safe enough*/
#define CDMA_CW_B           0x0008
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */

#define BT_AG_SEND_DATA_B      0x0100


#ifdef FEATURE_BT_HFP_1_5
#define BT_AG_NUM_TYPE_UNKNOWN       (0x80 | CM_TON_UNKNOWN       | CM_NPI_ISDN)
#define BT_AG_NUM_TYPE_INTERNATIONAL (0x80 | CM_TON_INTERNATIONAL | CM_NPI_ISDN)
#define BT_AG_NUM_TYPE_NATIONAL      (0x80 | CM_TON_NATIONAL      | CM_NPI_ISDN)
#endif


/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for AG.                      */
/*-------------------------------------------------------------------------*/


/* event related */
typedef enum
{
  BT_AG_SPP_EV,
  BT_AG_CM_EV_CALL,
  BT_AG_CM_EV_CALL_INFO_LIST,
  BT_AG_CM_EV_PH,
  BT_AG_CM_EV_SS,
  BT_AG_CM_EV_INBAND
} bt_ag_ev_enum_type;

typedef struct
{
  q_link_type             link;
  bt_ag_ev_enum_type      ev_type;
} bt_ag_ev_hdr_type;

/* Handsfree specific data */
#ifdef BT_AG_HF_EXTENSION
typedef struct
{
  char                pb_name[5];
  bt_pap_device_type  ph_type;
  boolean             supported;
} bt_ag_pb_name_entry_type;
#endif /* BT_AG_HF_EXTENSION */

typedef enum
{
  CME_ERR_NO_CONN               = 1,
  CME_ERR_OP_NOT_ALLOWED        = 3,
  CME_ERR_OP_NOT_SUPPORTED,
  CME_ERR_PH_SIM_PIN_REQUIRED,
  CME_ERR_SIM_NOT_INSERTED      = 10,
  CME_ERR_SIM_PIN_REQUIRED,
  CME_ERR_SIM_PUK_REQUIRED,
  CME_ERR_SIM_FAILURE,
  CME_ERR_SIM_BUSY,
  CME_ERR_WRONG_PASSWORD        = 16,
  CME_ERR_SIM_PIN2_REQUIRED,
  CME_ERR_SIM_PUK2_REQUIRED,
  CME_ERR_MEMORY_FULL           = 20,
  CME_ERR_INVALID_INDEX,
  CME_ERR_MEMORY_FAILURE        = 23,
  CME_ERR_TEXT_TOO_LONG,
  CME_ERR_BAD_CHAR_IN_TEXT_STR,
  CME_ERR_DIAL_STRING_TOO_LONG,
  CME_ERR_BAD_CHAR_IN_DIAL_STR,
  CME_ERR_NO_NETWORK_SERVICE    = 30,
  CME_ERR_EMERGENCY_CALLS_ONLY  = 32
} bt_ag_cme_err_type;

typedef enum
{
  BT_AG_HCS_BRSF,
  BT_AG_HCS_CIND_TEST,
  BT_AG_HCS_CIND_READ,
  BT_AG_HCS_CMER,
  BT_AG_HCS_CHLD_TEST,
  BT_AG_HCS_DONE
} bt_ag_hf_conn_state_type;

typedef enum
{
  BT_AG_CALL_SETUP_NONE,
  BT_AG_CALL_SETUP_IN,
  BT_AG_CALL_SETUP_OUT,
  BT_AG_CALL_SETUP_REMOTE_PARTY_ALERTED
} bt_ag_call_setup_state_type;

typedef enum
{
  BT_AG_CALL_HELD_NONE       = 0,
  BT_AG_CALL_HELD_ACTIVE     = 1, /* a call placed on hold, 
                                     or active call swapped with held call */
  BT_AG_CALL_HELD_TERMINATED = 2  /* active call terminated 
                                     while held call remains */
} bt_ag_call_held_state_type;

typedef enum
{
  BT_AG_INDICATOR_NONE,
  BT_AG_INDICATOR_SERVICE,
  BT_AG_INDICATOR_CALL,
  BT_AG_INDICATOR_CALL_SETUP,
  BT_AG_INDICATOR_SIGNAL,
  BT_AG_INDICATOR_BATTCHG,
  BT_AG_INDICATOR_ROAM,
  BT_AG_INDICATOR_CALL_HELD
} bt_ag_indicator_type;

typedef enum
{
  BT_AIER_NO_IER, /* no indicator event reporting   */
  BT_AIER_KEEP,   /* keep indicator event in AG     */
  BT_AIER_FWD     /* forward indicator event to HF  */
} bt_ag_ind_ev_report_type;

typedef enum
{
  BT_AG_CH_RELEASE_ALL,
  BT_AG_CH_RELEASE_ACTIVE,
  BT_AG_CH_HOLD_ACTIVE,
  BT_AG_CH_ADD_NEW,
  BT_AG_CH_TRANSFER
} bt_ag_call_hold_type;

typedef enum
{
  BT_AG_ROAM_0 = 0,
  BT_AG_ROAM_1,
  BT_AG_ROAM_2
} bt_ag_roam_type;

typedef struct
{
  uint32                      features_bitmap;
  uint16                      profile_version;
  bt_ag_hf_conn_state_type    connection_state;
  bt_ag_call_setup_state_type call_setup_state;
  bt_ag_call_held_state_type  call_held_state;
  bt_ag_ind_ev_report_type    ier_mode;
  cm_client_id_type           cm_client_id;
  cm_call_state_info_list_s_type call_info_list;
  boolean                     service_avail;
  boolean                     call_active;
  boolean                     vr_enabled;
  boolean                     notify_call_waiting;
  boolean                     notify_cli;
  boolean                     nrec_off;
  boolean                     dtmf_started;
  boolean                     ext_err_enabled;
  cm_num_s_type               phone_num;
  cm_call_id_type             call_id;
  cm_call_id_type             incoming_call_id;
  cm_mm_ss_info_s_type        ss_info;
  uint8                       num_calls;
  uint8                       rssi;
  uint8                       battchg;
  uint8                       roam;
  boolean                     supported_rh[ BT_AG_RH_MAX ];
#ifdef BT_AG_HF_EXTENSION
  uint8                       selected_pb_idx;  /* idx into bt_ag_ph_name_table */
  uint16                      pb_total_recs;    /* selected pb capacity */
  uint16                      entries_2_read;   /* valid when AT+CPBR rxd */
#endif /* BT_AG_HF_EXTENSION */
} bt_ag_hf_data_type;

typedef struct
{
  bt_ag_ev_hdr_type       hdr;
  cm_call_event_e_type    call_event;
  cm_mm_call_info_s_type  call_info;
} bt_ag_cm_call_ev_type;

typedef struct
{
  bt_ag_ev_hdr_type       hdr;
  cm_ph_event_e_type      ph_event;
  cm_ph_info_s_type       ph_info;
} bt_ag_cm_ph_ev_type;

typedef struct
{
  bt_ag_ev_hdr_type       hdr;
  cm_ss_event_e_type      ss_event;
  cm_mm_ss_info_s_type    ss_info;
} bt_ag_cm_ss_ev_type;

typedef struct
{
  bt_ag_ev_hdr_type       hdr;
  cm_inband_event_e_type  inband_event;
  cm_inband_info_s_type   inband_info;
} bt_ag_cm_inband_ev_type;

#ifdef FEATURE_BT_HFP_1_5
/* AT+CLCC related */
#define BT_AG_CLCC_NOT_MPTY             0
#define BT_AG_CLCC_MPTY                 1
#define BT_AG_CLCC_UNKNOWN              129
#define BT_AG_CLCC_INTERNATIONAL        145

typedef enum
{
  BT_AG_CLCC_DIRECTION_MO               = 0,
  BT_AG_CLCC_DIRECTION_MT               = 1
} bt_ag_clcc_direction_type;

typedef enum
{
  BT_AG_CLCC_STATE_ACTIVE               = 0,
  BT_AG_CLCC_STATE_HELD                 = 1,
  BT_AG_CLCC_STATE_DIALING              = 2,
  BT_AG_CLCC_STATE_ALERTING             = 3,
  BT_AG_CLCC_STATE_INCOMING             = 4,
  BT_AG_CLCC_STATE_WAITING              = 5
} bt_ag_clcc_state_type;

typedef enum
{
  BT_AG_CLCC_MODE_VOICE                 = 0,
  BT_AG_CLCC_MODE_DATA                  = 1,
  BT_AG_CLCC_MODE_FAX                   = 2
} bt_ag_clcc_mode_type;
#endif /* FEATURE_BT_HFP_1_5 */
/* End of Handsfree specific data */

typedef struct
{
  bt_ag_ev_hdr_type         hdr;
  bt_spp_status_type        spp_status;
} bt_ag_spp_status_ev_type;

typedef union
{
  bt_ag_ev_hdr_type         hdr;
  bt_ag_spp_status_ev_type  spp_ev;
  bt_ag_cm_call_ev_type     call_ev;
  bt_ag_cm_ph_ev_type       ph_ev;
  bt_ag_cm_ss_ev_type       ss_ev;
  bt_ag_cm_inband_ev_type   inband_ev;
} bt_ag_ev_type;

typedef struct
{
  bt_bd_addr_type             bd_addr;
  bt_bd_addr_type*            bd_addr_ptr; /* used to specify if a BD_ADDR is
                                            * available or not; only set via
                                            * cmd_enable; otherwise, NULL */
  bt_audio_device_type        device_type; /* headset or handsfree */
  bt_sd_uuid_type             service_uuid;
  bt_rc_scn_enum_type         server_channel;
  uint8                       mic_volume;
  uint8                       spkr_volume;

  bt_ag_hf_data_type          hf_data;

} bt_ag_ad_data_type;


typedef enum
{
  BT_AGS_DISABLED,            /* no SIO conn.             */
  BT_AGS_CLOSED,              /* SIO connection closed    */
  BT_AGS_OPENING,             /* opening SIO port         */
  BT_AGS_OPEN,                /* SIO port ready for       */
                              /* incoming connetions      */
  BT_AGS_CONNECTED,           /* SIO port connected       */
  BT_AGS_AUDIO_CONNECTING,    /* trying to make SCO conn. */
  BT_AGS_AUDIO_DISCONNECTING, /* trying to close SCO conn.*/
  BT_AGS_AUDIO_CONNECTED,     /* SCO connection up        */
  BT_AGS_CLOSING,             /* SIO connection closing   */
  BT_AGS_MAX
} bt_ag_state_type;

typedef struct
{
  bt_ag_state_type            state;
  bt_app_id_type              app_id_ag;          /* AG's app ID     */
  bt_app_id_type              app_id_user;        /* user's app ID   */
  bt_app_id_type              app_id_ring_device; /* app ringing audio dev */
  bt_app_id_type              app_id_add_audio;   /* app adding audio */

  bt_bd_addr_type             enabled_bd_addr;    /* BD addr in Enable cmd */
  bt_audio_device_type        enabled_device_type;
  bt_sd_uuid_type             enabled_service_class;

  rex_timer_cnt_type          ss_timer_ms_left;   /*  Steady State timer.  */
  
  bt_event_q_info_type        ev_q_info;
  sio_stream_id_type          sio_stream_id;
  sio_stream_id_type          sio_stream_id_hs;
  sio_stream_id_type          sio_stream_id_hf;
  dsm_item_type**             cur_dsm_ptr_ptr;
  dsm_item_type*              tx_dsm_ptr;
  dsm_watermark_type          tx_wm;
  dsm_watermark_type          rx_wm;
  q_type                      ev_q;
  q_type                      to_sio_q;
  q_type                      from_sio_q;

  bt_ag_ev_type               cb_ev_pool[ BT_AG_MAX_NUM_BT_EVENTS ];
  q_type                      cb_ev_free_q;
  q_type                      cb_ev_q;
  bt_event_reason_type        event_reason;

  bt_sd_srv_rec_type*         srv_rec_ptr;
  bt_sd_uuid_type             service_class;    /* the UUID to use */
  uint16                      profile_version_number;

  bt_ag_idle_mode_type        idle_mode;
  uint8                       conn_attempts;
  uint32                      idle_timeout;     /* in ms */
  uint16                      ring_count;
  uint16                      ring_period;

  int32                       sco_handle;
  boolean                     force_server_mode;
  boolean                     is_sio_server;
  boolean                     going_idle;
  boolean                     is_idle;
  boolean                     adding_audio;
  boolean                     ringing;
  boolean                     set_mic_gain;
  boolean                     set_spkr_gain;
  boolean                     hs_svc_registered;
  boolean                     hf_svc_registered;
  boolean                     disabling;
  boolean                     send_audio_conn_ev;
  boolean                     tx_wm_enabled; /* tx high wm not reached */

  bt_ag_ad_data_type          audio_device; /* data for HS/HF */

  uint16                      flags;
  uint32                      features_bitmap;
} bt_ag_data_type;


typedef enum
{
  BT_AG_RESP_LIST,
  BT_AG_RESP_INFO
} bt_ag_resp_sub_type;


typedef enum
{
  BT_AG_RESP_OK = 0,        /* ACK to command execution                     */
  BT_AG_RESP_ERROR,         /* error indication code                        */
  BT_AG_RESP_RING,          /* incoming call indication                     */
  BT_AG_RESP_VGM,           /* microphone volume gain report                */
  BT_AG_RESP_VGS,           /* speaker volume gain report                   */
  BT_AG_RESP_BINP,          /* BT input response                            */
  BT_AG_RESP_BRSF,          /* supported features in AG                     */
  BT_AG_RESP_BSIR,          /* in-band ring tone in AG                      */
  BT_AG_RESP_BVRA,          /* voice recognition function in AG             */
  BT_AG_RESP_CCWA,          /* call waiting notification                    */
  BT_AG_RESP_CHLD,          /* call hold and multiparty services            */
  BT_AG_RESP_CIEV,          /* indicator event                              */
  BT_AG_RESP_CIND,          /* indicator status/order mapping               */
#ifdef FEATURE_BT_HFP_1_5
  BT_AG_RESP_CLCC,          /* info of a single call                        */
#endif /* FEATURE_BT_HFP_1_5 */
  BT_AG_RESP_CLIP,          /* calling line identification notification     */
  BT_AG_RESP_BTRH,          /* list of supported/current r&h states         */
  BT_AG_RESP_CME_ERROR,
#ifdef BT_AG_HF_EXTENSION
  BT_AG_RESP_PAP_CPBS,      /* list of/info on supported phonebooks         */
  BT_AG_RESP_PAP_CPBR,      /* phonebook entry info                         */
  BT_AG_RESP_PAP_CPBF,      /* find phonebook entry                         */
  BT_AG_RESP_PAP_CNUM,      /* subscriber number info                       */
#endif /* BT_AG_HF_EXTENSION */
  BT_AG_RESP_MAX
} bt_ag_response_type;

LOCAL const char rsps_from_ag [ BT_AG_RESP_MAX ][ BT_AG_RESP_NAME_MAX_LEN ] =
{
  { "OK"      },
  { "ERROR"   },
  { "RING"    },
  { "+VGM="   }, /* params=<gain> (gain=0-15)                   */
  { "+VGS="   }, /* params=<gain> (gain=0-15)                   */
  { "+BINP: " }, /* param=<phone number string (32 digits max)> */
  { "+BRSF: " }, /* param=<AG supported features bitmap>        */
  { "+BSIR: " }, /* param=0 or 1                                */
  { "+BVRA: " }, /* param=0 or 1                                */
  { "+CCWA: " }, /* params=<phone number>,0,1                   */
  { "+CHLD: " }, /* param=(0,1,2,3,4)                           */
  { "+CIEV: " }, /* params=<ind>,<val>                          */
  { "+CIND: " }, /* params depending on list or info flavor     */
#ifdef FEATURE_BT_HFP_1_5
  { "+CLCC: " }, /* no param                                    */
#endif /* FEATURE_BT_HFP_1_5 */
  { "+CLIP: " }, /* params=<phone number>,<type>                */
  { "+BTRH: " }, /* param depending on list or info flavor      */
  { "+CME ERROR:" }
#ifdef BT_AG_HF_EXTENSION
  ,
  { "+CPBS: " }, /* param depending on list or info flavor      */
  { "+CPBR: " }, /* param=idx,num,type,text                     */
  { "+CPBF: " }, /* param=idx,num,type,text                     */
  { "+CNUM: " }  /* param=num,type,service                      */
#endif /* BT_AG_HF_EXTENSION */
};

LOCAL const char cind_mapping_params[] =
  "(\"service\",(0,1)),(\"call\",(0,1)),(\"callsetup\",(0-3))";

#ifdef FEATURE_BT_HFP_1_5
LOCAL const char cind_mapping_1_5_params[] =
  ",(\"signal\",(0-5)),(\"battchg\",(0-5))"
#ifdef BT_AG_HF_EXTENSION
  ",(\"roam\",(0-2))"
#else
  ",(\"roam\",(0,1))"
#endif
  ",(\"callheld\",(0-2))";
#endif /* FEATURE_BT_HFP_1_5 */

/*  AT command parsing engine constants.  */
#define BT_AG_CMD_TERM_CHAR       '\r'
#define BT_AG_CMD_DELIMITER       ';'
#define BT_AG_CMD_PARAM_SEPARATOR ','
#define BT_AG_CPE_MAX_CMD_LEN     128
#define BT_AG_CPE_MAX_CMD_PARAMS  5

#define BT_AG_CMD_MIN 0
typedef enum
{
  BT_AG_CMD_CKPD = BT_AG_CMD_MIN,  /*  HS button pressed                    */
  BT_AG_CMD_VGM,            /*  microphone volume gain                      */
  BT_AG_CMD_VGS,            /*  speaker volume gain                         */
  BT_AG_CMD_CALL_ANSWER,    /*  answer call                                 */
  BT_AG_CMD_DIAL,           /*  dial given number                           */
  BT_AG_CMD_BINP,           /*  request data input                          */
#ifndef FEATURE_BT_HFP_1_5
  BT_AG_CMD_BLDN,           /*  call last number dialed                     */
#endif
  BT_AG_CMD_BRSF,           /*  notifies AG of supported features in HF     */
  BT_AG_CMD_BVRA,           /*  toggle voice recognition function in AG     */
  BT_AG_CMD_CCWA,           /*  toggle call waiting notification function   */
  BT_AG_CMD_CHLD,           /*  call hold & multiparty                      */
  BT_AG_CMD_CHUP,           /*  hang up call                                */
  BT_AG_CMD_CIND_READ,      /*  get indicators status                       */
  BT_AG_CMD_CIND_TEST,      /*  get mapping of indicators in AG             */
#ifdef FEATURE_BT_HFP_1_5
  BT_AG_CMD_CLCC,           /*  read call info                              */
#endif /* FEATURE_BT_HFP_1_5 */
  BT_AG_CMD_CLIP,           /*  toggle calling line ID notification func.   */
  BT_AG_CMD_CMER,           /*  activate indicator event reporting func.    */
  BT_AG_CMD_NREC,           /*  disable EC & NR functions in AG             */
  BT_AG_CMD_VTS,            /*  transmit DTMF tone                          */
#ifdef BT_AG_HF_EXTENSION                                                  
  BT_AG_CMD_BTRH_READ,      /*  query current respond and hold state of AG  */
  BT_AG_CMD_BTRH,           /*  respond and hold state in AG                */
  BT_AG_CMD_PAP_CPBS_READ,  /*  get phonebook info                          */
  BT_AG_CMD_PAP_CPBS,       /*  phonebook source                            */
  BT_AG_CMD_PAP_CPBR,       /*  read phonebook entry                        */
  BT_AG_CMD_PAP_CPBF,       /*  find phonebook entries matching text        */
  BT_AG_CMD_PAP_CPBW,       /*  write phonebook entry                       */
  BT_AG_CMD_PAP_CNUM,       /*  read subscriber number info                 */
#endif /* BT_AG_HF_EXTENSION */

  BT_AG_CMD_MAX
} bt_ag_cmd_type;

typedef struct
{
  char                cmd_text[ 11 ];
  uint8               param_cnt; /* amount of necessary param of a command */
  bt_ag_int_cmd_type  int_cmd_b;
} bt_ag_cpe_int_cmd_type;

LOCAL const bt_ag_cpe_int_cmd_type bt_ag_cpe_cmd[ BT_AG_CMD_MAX ] =
{
  { "AT+CKPD=",    1, BT_AG_IC_CKPD_B  },  /*  HS button pressed           */
  { "AT+VGM=",     1, BT_AG_IC_VGM_B   },  /*  microphone volume gain      */
  { "AT+VGS=",     1, BT_AG_IC_VGS_B   },  /*  speaker volume gain         */
  { "ATA\r",       0, BT_AG_IC_A_B     },  /*  call answer                 */
  { "ATD",         1, BT_AG_IC_D_B     },  /*  dial num or mem location    */
  { "AT+BINP=1\r", 0, BT_AG_IC_BINP1_B },  /*  request data input from AG  */
#ifndef FEATURE_BT_HFP_1_5
  { "AT+BLDN\r",   0, BT_AG_IC_BLDN_B  },  /*  call last number dialed     */
#endif
  { "AT+BRSF=",    1, BT_AG_IC_NONE_M  },  /*  supported features in HF    */
  { "AT+BVRA=",    1, BT_AG_IC_BVRA_B  },  /*  toggle VR function in AG    */
  { "AT+CCWA=",    1, BT_AG_IC_CCWA_B  },  /*  toggle call waiting notif.  */
  { "AT+CHLD=",    1, BT_AG_IC_NONE_M  },  /*  call hold & multiparty      */
  { "AT+CHUP\r",   0, BT_AG_IC_CHUP_B  },  /*  hang up call                */
  { "AT+CIND?\r",  0, BT_AG_IC_NONE_M  },  /*  get indicators status       */
  { "AT+CIND=?\r", 0, BT_AG_IC_NONE_M  },  /*  get indicator mapping       */
#ifdef FEATURE_BT_HFP_1_5
  { "AT+CLCC\r",   0, BT_AG_IC_NONE_M  },  /*  get call info               */
#endif /* FEATURE_BT_HFP_1_5 */
  { "AT+CLIP=",    1, BT_AG_IC_CLIP_B  },  /*  toggle calling ID notif.    */
  { "AT+CMER=",    4, BT_AG_IC_NONE_M  },  /*  activate indicator events   */
  { "AT+NREC=",    1, BT_AG_IC_NREC_B  },  /*  disable EC & NR             */
  { "AT+VTS=",     1, BT_AG_IC_VTS_B   }   /*  transmit DTMF tone          */
#ifdef BT_AG_HF_EXTENSION
  ,
  { "AT+BTRH?\r",  0, BT_AG_IC_BTRH_B  },  /* get respond and hold state   */
  { "AT+BTRH=",    1, BT_AG_IC_BTRH_B  },  /* respond and hold state       */
  { "AT+CPBS?\r",  0, BT_AG_IC_CPBS_B  },  /* get phonebook info           */
  { "AT+CPBS=",    1, BT_AG_IC_CPBS_B  },  /* phonebook source             */
  { "AT+CPBR=",    2, BT_AG_IC_CPBR_B  },  /* read phonebook entry         */
  { "AT+CPBF=",    1, BT_AG_IC_CPBF_B  },  /* find phonebook entries       */
  { "AT+CPBW=",    1, BT_AG_IC_CPBW_B  },  /* write phone book entry       */
  { "AT+CNUM\r",   0, BT_AG_IC_CNUM_B  }   /* read subscriber number info  */
#endif /* BT_AG_HF_EXTENSION */
};

/* structure used to map call indexes to call ids */
typedef struct
{
  uint8                   num_call_ids;
  cm_mm_call_info_s_type  call_info[ CM_CALL_ID_MAX ];  
} call_info_table_type;
LOCAL call_info_table_type call_info_table;

LOCAL bt_ag_data_type bt_ag;
LOCAL rex_timer_type  bt_ag_timer;
LOCAL bt_bd_addr_type null_bd_addr = { 0, 0, 0, 0, 0, 0 };
LOCAL char bt_ag_qxdm_msg[ 200 ];

#define AUDIO_DEV bt_ag.audio_device
#define HF_DATA   bt_ag.audio_device.hf_data


#ifdef BT_AG_HF_EXTENSION
bt_ag_pb_name_entry_type bt_ag_ph_name_table[] =
{
  { "\"ME\"", BT_PAP_DEV_ME, FALSE },  /* EFS phonebook list             */
  { "\"RC\"", BT_PAP_DEV_RC, FALSE },  /* received calls list            */
  { "\"DC\"", BT_PAP_DEV_DC, FALSE },  /* dialed calls list              */
  { "\"MC\"", BT_PAP_DEV_MC, FALSE },  /* missed calls list              */
  { "\"MT\"", BT_PAP_DEV_MT, FALSE },  /* EFS + SIM phonebook list       */
  { "\"SM\"", BT_PAP_DEV_SM, FALSE },  /* SIM phonebook list             */
  { "\"FD\"", BT_PAP_DEV_FD, FALSE },  /* SIM fixed dialing number list  */
  { "\"LD\"", BT_PAP_DEV_LD, FALSE },  /* SIM last dialing number list   */
};

#endif /* BT_AG_HF_EXTENSION */


/*  BT AG command parsing engine data.  */

typedef enum
{
  BT_AG_ACPS_GET_CMD_IDX,
  BT_AG_ACPS_EXTRACT_PARAMS,
  BT_AG_ACPS_UNRECOGNIZED_CMD,
  BT_AG_ACPS_RX_OVERFLOW
} bt_ag_cmd_states_type;

typedef struct
{
  bt_ag_cmd_states_type state;
  bt_ag_cmd_type        cmd_idx;
  uint16                cmd_char_idx;
  
  char                  rx_buf[ BT_AG_CPE_MAX_CMD_LEN ];
  uint16                rx_count;
  uint16                rx_param_idx[ BT_AG_CPE_MAX_CMD_PARAMS ];
  uint16                rx_param_len[ BT_AG_CPE_MAX_CMD_PARAMS ];
  uint8                 rx_param_count;

  boolean               enable_external_io;
  boolean               respond_pending;
  dsm_item_type*        tx_dsm_ptr;
  dsm_watermark_type*   rx_wm_ptr;
  dsm_watermark_type*   tx_wm_ptr;
  wm_cb_type            orig_tx_wm_ne_fptr;
  bt_ag_int_cmd_type    int_cmd_off_mask;
#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
  wm_cb_type            orig_rx_wm_ne_fptr;
  boolean               call_orig_rx_wm_ne_func;
#endif
} bt_ag_cmd_parsing_engine_type;

bt_ag_cmd_parsing_engine_type  bt_agcpe;

#define CPE_PARAM_0 (bt_agcpe.rx_param_idx[ 0 ])
#define CPE_PARAM_1 (bt_agcpe.rx_param_idx[ 1 ])
#define CPE_PARAM_2 (bt_agcpe.rx_param_idx[ 2 ])
#define CPE_PARAM_3 (bt_agcpe.rx_param_idx[ 3 ])

#define CPE_PARAM_LEN_0 (bt_agcpe.rx_param_len[ 0 ])
#define CPE_PARAM_LEN_1 (bt_agcpe.rx_param_len[ 1 ])
#define CPE_PARAM_LEN_2 (bt_agcpe.rx_param_len[ 2 ])
#define CPE_PARAM_LEN_3 (bt_agcpe.rx_param_len[ 3 ])


/*  Tests basic AG External I/O capability via QXDM (QC only).  */
#undef BT_SWDEV_AG_EXTAPP_TEST

#ifdef BT_SWDEV_AG_EXTAPP_TEST
dsm_watermark_type  extapp_from_hf_wm;  /*  For data RXd from HF.  */
q_type              extapp_from_hf_q;

dsm_watermark_type  extapp_to_hf_wm;    /*  For data to TX to HF.  */
q_type              extapp_to_hf_q;

#define ERROR_RESP_LEN 9
uint8  error_resp[ ERROR_RESP_LEN ] =
               { '\r', '\n', 'E', 'R', 'R', 'O', 'R', '\r', '\n' };


/*===========================================================================
  Call back for Non-Empty RX watermark.
===========================================================================*/
void extapp_rx_hf_data_cb( void )
{

  uint8           rx_byte;
  dsm_item_type*  dsm_ptr;
  dsm_item_type*  tx_dsm_ptr;

  BT_MSG_HIGH( "BT AG: Got Ext I/O Data.", 0, 0, 0 );

  /*  Remove all data from receive watermark and parse it.  */
  while ( (dsm_ptr = dsm_dequeue( &extapp_from_hf_wm )) != NULL )
  {
    while ( dsm_pullup( &dsm_ptr, &rx_byte, sizeof( uint8 ) )
              == sizeof( uint8 ) )
    {
      BT_MSG_HIGH( "BT AG: Ext I/O RX %x (%c)", rx_byte, rx_byte, 0 );

      if ( rx_byte == '\r' )
      {
        tx_dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, OK_RESP_LEN );

        if ( dsm_pushdown_tail( &tx_dsm_ptr,
                                (void*)(error_resp),
                                ERROR_RESP_LEN,
                                DSM_DS_SMALL_ITEM_POOL ) == ERROR_RESP_LEN )
        {
          dsm_enqueue( &extapp_to_hf_wm, &tx_dsm_ptr );

          BT_MSG_HIGH( "BT AG: Ext I/O TX Error Resp", 0, 0, 0 );
        }
        else
        {
          BT_ERR( "BT AG: Ext I/O Bad TX", 0, 0, 0 );
        }
      }
    }
  }

}
#endif /* BT_SWDEV_AG_EXTAPP_TEST */


/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/* the call_info_table lookup algorithm here inside this
 * featurization is based on the fact that there are at
 * most two calls can co-exist in CDMA (is this TRUE?).
 */

/*===========================================================================

FUNCTION
  bt_ag_call_info_table_update_waiting_call

DESCRIPTION
  update the CDMA waiting call entry in the call_info_table

===========================================================================*/
LOCAL void bt_ag_call_info_table_update_waiting_call
(
  boolean  call_is_ended  /* TRUE if waiting call ended */
)
{

  /* call_info[ 1 ] holds the waiting call info */

  if ( call_is_ended )
  {
    call_info_table.call_info[ 1 ].call_id = CM_CALL_ID_INVALID;
    call_info_table.num_call_ids--;
  }
  else
  {
    /* waiting call answered */
    call_info_table.call_info[ 1 ].call_state = CM_CALL_STATE_CONV;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_call_info_table_update_callheld

DESCRIPTION
  update the CDMA call IDs in the call_info_table when callheld set to
  BT_AG_CALL_HELD_ACTIVE.

===========================================================================*/
LOCAL void bt_ag_call_info_table_update_callheld
(
  const cm_mm_call_info_s_type*  call_info_ptr  /* active call info */
)
{

  if ( call_info_table.call_info[ 0 ].call_id !=
         call_info_ptr->call_id )
  {
    /* active-held call swap */

    call_info_table.call_info[ 0 ].call_id =
      call_info_ptr->call_id;
    call_info_table.call_info[ 1 ].call_id =
      ++call_info_table.call_info[ 1 ].call_id % CM_CALL_ID_MAX;
  }
  else
  {
    /* waiting call answered.
     * call_info[ 1 ] holds the waiting call info.
     */

    call_info_table.call_info[ 1 ].call_id =
      call_info_ptr->call_id;
    call_info_table.call_info[ 0 ].call_id =
      ++call_info_table.call_info[ 0 ].call_id % CM_CALL_ID_MAX;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_call_info_table_update_2nd_ended

DESCRIPTION
  clear the 2nd CDMA call entry in the call_info_table

===========================================================================*/
LOCAL void bt_ag_call_info_table_update_2nd_ended
(
  void
)
{

  /* the 2nd call (MO) was ended, revise the call id of the
   * 1st call back to the real one.
   */
  call_info_table.call_info[ 0 ].call_id =
    call_info_table.call_info[ 1 ].call_id;
  call_info_table.call_info[ 1 ].call_id = CM_CALL_ID_INVALID;
  call_info_table.num_call_ids--;

}
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */


/*===========================================================================

FUNCTION
  bt_ag_call_info_table_cleanup

DESCRIPTION
  Initializes the call_info_table

===========================================================================*/
LOCAL void bt_ag_call_info_table_cleanup
(
  void
)
{

  uint8 seq_num = 0;
  
  while ( seq_num < CM_CALL_ID_MAX )
  {
    call_info_table.call_info[seq_num].call_id = CM_CALL_ID_INVALID;
    seq_num++;
  }

  call_info_table.num_call_ids = 0;

}


/*===========================================================================

FUNCTION
  bt_ag_update_call_info_table

DESCRIPTION
  Updates call_info_table.
  The call_info_table must have been initialized by
  bt_ag_call_info_table_cleanup.

===========================================================================*/
LOCAL void bt_ag_update_call_info_table
(
  bt_ag_cm_call_ev_type*  ev_ptr
)
{

  uint8 seq_num   = 0;
  uint8 l_seq_num = CM_CALL_ID_MAX; /* lowest available seq_num */

  BT_MSG_DEBUG( "BT AG: Rcvd call info, ev=%x id=%x st=%d held=%d",
                ev_ptr->call_event,
                ev_ptr->call_info.call_id,
                ev_ptr->call_info.call_state );

  if ( (ev_ptr->call_event == CM_CALL_EVENT_INFO) ||
       (ev_ptr->call_event == CM_CALL_EVENT_ORIG) ||
       (ev_ptr->call_event == CM_CALL_EVENT_INCOM) )
  {
    /* new call, add an entry for it */

    /* if the call_info table is not full */
    if ( call_info_table.num_call_ids < CM_CALL_ID_MAX )
    {
      /* checks if call info has been stored */
      while ( seq_num < CM_CALL_ID_MAX )
      {
        /* store the lowest available seq_num */
        if ( (l_seq_num == CM_CALL_ID_MAX) &&
             (call_info_table.call_info[seq_num].call_id ==
                CM_CALL_ID_INVALID) )
        {
          l_seq_num = seq_num;
        }

        if ( call_info_table.call_info[seq_num].call_id !=
               ev_ptr->call_info.call_id )
        {
          seq_num++;
        }
        else
        {
          BT_MSG_SIG ( "BT AG: Call info has been stored! id=%x",
                         ev_ptr->call_info.call_id, 0, 0 );
          break;
        }
      }

      if ( (seq_num == CM_CALL_ID_MAX) &&
           (l_seq_num < CM_CALL_ID_MAX) ) /* Added to fix lint errors */
      {
        /* Copy passed data to local variable */
        memcpy( (void*)&call_info_table.call_info[l_seq_num],
                (void*)&ev_ptr->call_info,
                sizeof( cm_mm_call_info_s_type ) );

        call_info_table.num_call_ids++;

        BT_MSG_HIGH( "BT AG: Updated call info, idx|id=0x%04x st=%d held=%d",
                     ((uint16)((l_seq_num + 1) << 8) | 
                      call_info_table.call_info[l_seq_num].call_id),                
                     call_info_table.call_info[l_seq_num].call_state,
                     HF_DATA.call_held_state );
      }
    }
    else
    {
      MSG_ERROR( "BT AG: Call info table is full! cNo=%d",
                 call_info_table.num_call_ids, 0, 0 );
    }
  }
  else
  {
    /* call info entry should exist already */

    /* find the corresponding entry for the call */
    while ( (seq_num < CM_CALL_ID_MAX) &&
            (call_info_table.call_info[seq_num].call_id !=
             ev_ptr->call_info.call_id) )
    {
      seq_num++;
    }

    if ( seq_num < CM_CALL_ID_MAX )
    {
      /* may be able to optimize the code below to get rid
       * of the duplicated check of the call_event.
       */
      if ( ev_ptr->call_event == CM_CALL_EVENT_END )
      {
        call_info_table.call_info[seq_num].call_id = CM_CALL_ID_INVALID;
        call_info_table.num_call_ids--;
      }
      else if ( ev_ptr->call_event == CM_CALL_EVENT_CONNECT )
      {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        if ( call_info_table.call_info[seq_num].call_state ==
               CM_CALL_STATE_CONV )
        {
          /* this is the 2nd call (MO).
           * find another entry and copy passed data
           * to local variables.
           */
          call_info_table.call_info[seq_num].call_id =
            ++call_info_table.call_info[seq_num].call_id % CM_CALL_ID_MAX;

          seq_num = ( seq_num + 1 ) % CM_CALL_ID_MAX;

          memcpy( (void*)&call_info_table.call_info[seq_num],
                  (void*)&ev_ptr->call_info,
                  sizeof( cm_mm_call_info_s_type ) );

          call_info_table.num_call_ids++;

          BT_MSG_HIGH( "BT AG: Updated call info, idx|id=0x%04x st=%d held=%d",
                       ((uint16)((l_seq_num + 1) << 8) | 
                        call_info_table.call_info[l_seq_num].call_id),                
                       call_info_table.call_info[l_seq_num].call_state,
                       HF_DATA.call_held_state );
        }
        else
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
        {
          call_info_table.call_info[seq_num].call_state =
            CM_CALL_STATE_CONV;
        }
      }
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      else if ( ev_ptr->call_event == CM_CALL_EVENT_CALLER_ID )
      {
        /* get the caller ID for the CDMA call */
        call_info_table.call_info[seq_num].num = ev_ptr->call_info.num;
      }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
      /* must be (ev_ptr->call_event == CM_CALL_EVENT_PROGRESS_INFO_IND) */
      else if ( call_info_table.call_info[seq_num].call_state ==
                CM_CALL_STATE_ORIG )
      {
        call_info_table.call_info[seq_num].call_state =
          CM_CALL_STATE_CC_IN_PROGRESS;
      }

      BT_MSG_HIGH( "BT AG: Updated call info, idx|id=0x%04x st=%d held=%d",
                   ((uint16)((seq_num + 1) << 8) | 
                    call_info_table.call_info[seq_num].call_id),                
                   call_info_table.call_info[seq_num].call_state,
                   HF_DATA.call_held_state );
    }
    else
    {
      MSG_ERROR ("BT AG: Call id %x NOT found!", ev_ptr->call_info.call_id, 0, 0);
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ok_to_send

DESCRIPTION
  Determines if it's okay to send data

===========================================================================*/
LOCAL boolean bt_ag_ok_to_send
(
  void
)
{
  if ( (bt_ag.state > BT_AGS_OPEN) && (bt_ag.state < BT_AGS_CLOSING) &&
       (bt_ag.tx_wm_enabled == TRUE) )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION
  bt_ag_initialize_external_cmd_io

DESCRIPTION
  Initializes external AG command input/output.

===========================================================================*/
LOCAL void bt_ag_initialize_external_cmd_io
(
  void
)
{

  bt_agcpe.enable_external_io   = FALSE;
  bt_agcpe.rx_wm_ptr            = NULL;
  bt_agcpe.tx_wm_ptr            = NULL;
  bt_agcpe.tx_dsm_ptr           = NULL;
  bt_agcpe.int_cmd_off_mask     = BT_AG_IC_NONE_M;

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
  bt_agcpe.call_orig_rx_wm_ne_func = FALSE;
#endif

}


/*===========================================================================

FUNCTION
  bt_ag_disable_external_cmd_io

DESCRIPTION
  Disables external AG command input/output.

===========================================================================*/
LOCAL void bt_ag_disable_external_cmd_io
(
  void
)
{

  BT_MSG_DEBUG( "BT AG: ATCMD Disable external parsing.", 0, 0, 0 );

  bt_agcpe.enable_external_io = FALSE;
  bt_agcpe.respond_pending    = FALSE;
  bt_agcpe.int_cmd_off_mask   = BT_AG_IC_NONE_M;

  bt_ag.cur_dsm_ptr_ptr       = &bt_ag.tx_dsm_ptr;
  dsm_free_packet( &bt_agcpe.tx_dsm_ptr );

  bt_agcpe.rx_wm_ptr = NULL;
  bt_agcpe.tx_wm_ptr = NULL;

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_set_command_idx

DESCRIPTION
  Set the command parsing engine command index to the specified
  value or the next one above the value that is valid.  If the end
  of the command list is reached the engine state is updated to
  reflect that.

===========================================================================*/
LOCAL void bt_ag_cpe_set_command_idx
(
  bt_ag_cmd_type  new_cmd_idx
)
{

  bt_agcpe.cmd_idx = new_cmd_idx;

  /*  Increment index to next command to be internally     */
  /*  recognized for processing.  If external I/O is       */
  /*  enabled then commands may be individually turned     */
  /*  off with the bit mask in bt_agcpe.int_cmd_off_mask.  */

  while ( (bt_agcpe.cmd_idx < BT_AG_CMD_MAX) &&
          (bt_agcpe.enable_external_io != FALSE) && 
          ((bt_agcpe.int_cmd_off_mask &
              bt_ag_cpe_cmd[ bt_agcpe.cmd_idx ].int_cmd_b) != 0) )
  {
    bt_agcpe.cmd_idx++;
  }

  bt_agcpe.cmd_char_idx = 0;
  
  if ( bt_agcpe.cmd_idx == BT_AG_CMD_MAX )
  {
    bt_agcpe.state = BT_AG_ACPS_UNRECOGNIZED_CMD;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_init_parameter

DESCRIPTION
  Initializes the specified parameter in the AG command parsing engine.

===========================================================================*/
LOCAL void bt_ag_cpe_init_parameter
(
  uint8  param_idx
)
{
  
  bt_agcpe.rx_param_idx[ param_idx ] = bt_agcpe.rx_count;
  bt_agcpe.rx_param_len[ param_idx ] = 0;

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_reset_engine

DESCRIPTION
  Resets the AG command parsing engine.

===========================================================================*/
LOCAL void bt_ag_cpe_reset_engine
(
  void
)
{

  uint8  i;

  bt_agcpe.state          = BT_AG_ACPS_GET_CMD_IDX;
  bt_agcpe.rx_count       = 0;
  bt_agcpe.rx_param_count = 0;

  for ( i = 0; i < BT_AG_CPE_MAX_CMD_PARAMS; i++ )
  {
    bt_ag_cpe_init_parameter( i );
  }

  bt_ag_cpe_set_command_idx( (bt_ag_cmd_type)BT_AG_CMD_MIN );

}

/*===========================================================================

FUNCTION
  bt_ag_send_volume_to_snd_task

DESCRIPTION
  Updates AVS with new volume setting.

===========================================================================*/
LOCAL void bt_ag_send_volume_to_snd_task( uint8 volume )
{
#if defined(FEATURE_BT_DEVICE_VOLUME_CONTROL)
#error code not present
#endif /* FEATURE_BT_DEVICE_VOLUME_CONTROL */
}

/*===========================================================================

FUNCTION
  bt_ag_cleanup

DESCRIPTION
  Resets AG state to disable.

===========================================================================*/
LOCAL void bt_ag_cleanup
(
  void
)
{

  /*  Initialize core data.  */
  bt_ag.state                   = BT_AGS_DISABLED;
  bt_ag.sio_stream_id           = SIO_NO_STREAM_ID;
  bt_ag.sio_stream_id_hs        = SIO_NO_STREAM_ID;
  bt_ag.sio_stream_id_hf        = SIO_NO_STREAM_ID;
  bt_ag.app_id_user             = BT_APP_ID_NULL;
  bt_ag.app_id_ring_device      = BT_APP_ID_NULL;
  bt_ag.is_sio_server           = TRUE;
  bt_ag.force_server_mode       = FALSE;
  bt_ag.going_idle              = FALSE;
  bt_ag.is_idle                 = TRUE;
  bt_ag.set_mic_gain            = FALSE;
  bt_ag.set_spkr_gain           = FALSE;
  bt_ag.hs_svc_registered       = FALSE;
  bt_ag.hf_svc_registered       = FALSE;
  bt_ag.send_audio_conn_ev      = FALSE;
  bt_ag.srv_rec_ptr             = NULL;
  bt_ag.service_class           = BT_SD_INVALID_UUID;
  bt_ag.profile_version_number  = 0;
  bt_ag.conn_attempts           = 0;
  bt_ag.ring_count              = 0;
  bt_ag.flags                   = 0;

  AUDIO_DEV.device_type         = BT_AD_HEADSET;
  AUDIO_DEV.spkr_volume         = BT_AG_DEFAULT_HS_VOLUME_LEVEL;
  AUDIO_DEV.mic_volume          = BT_AG_DEFAULT_HS_VOLUME_LEVEL;

  memset( (byte*)(&AUDIO_DEV.bd_addr), 0, sizeof( bt_bd_addr_type ) );
  AUDIO_DEV.bd_addr_ptr         = NULL;

  bt_ag.features_bitmap         = BT_AG_F_MULTIPARTY_CALL 
                                | BT_AG_F_IN_BAND_RING_TONE
                                | BT_AG_F_ECNR_FUNC
#ifdef FEATURE_PHONE_VR
                                | BT_AG_F_VR_ON
#endif /* FEATURE_PHONE_VR */
                                ;

  /*  Reset AG command parsing engine.  */
  bt_ag_cpe_reset_engine();

}


/*===========================================================================

FUNCTION
  bt_ag_send_event

DESCRIPTION
  Builds and sends an event up to UI.

===========================================================================*/
LOCAL void bt_ag_send_event
(
  bt_event_type  ev_type
)
{

  bt_bd_addr_type*  bd_addr_ptr = &AUDIO_DEV.bd_addr;
  boolean           keep_going  = TRUE;
  bt_ev_msg_type    event_msg;

  event_msg.ev_hdr.bt_app_id = bt_ag.app_id_user;

  switch ( ev_type )
  {
    case BT_EV_AG_ENABLED:
      BT_MSG_API( "BT AG EV TX: AG Enabled Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_enabled.bd_addr = *bd_addr_ptr;
      break;

    case BT_EV_AG_DISABLED:
      BT_MSG_API( "BT AG EV TX: AG Disabled Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_disabled.bd_addr      = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_disabled.event_reason = BT_EVR_AG_DISABLE_RXD;
      break;

    case BT_EV_AG_BUTTON_PRESSED:
      BT_MSG_API( "BT AG EV TX: HS Button Pressed Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_button_pressed.bd_addr = *bd_addr_ptr;
      break;

    case BT_EV_AG_SLC_CHANGED:
      BT_MSG_API( "BT AG EV TX: SLC Changed Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_slc_changed.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_slc_changed.slc_up =
        (HF_DATA.connection_state == BT_AG_HCS_DONE) ? TRUE : FALSE;
      break;

    case BT_EV_AG_DEV_PICKUP:
      BT_MSG_API( "BT AG EV TX: HF Pick Up Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_pickup.bd_addr = *bd_addr_ptr;
      break;

    case BT_EV_AG_DEV_HANGUP:
      BT_MSG_API( "BT AG EV TX: HF Hang Up Event", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_hangup.bd_addr = *bd_addr_ptr;
      break;

    case BT_EV_AG_DEV_DIAL:
      BT_MSG_API( "BT AG EV TX: HF Dialed", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_dial.bd_addr   = *bd_addr_ptr;
      BT_STRCPY( (char*) event_msg.ev_msg.ev_ag_dev_dial.phone_num,
                 &bt_agcpe.rx_buf[ CPE_PARAM_0 ],
                 sizeof(event_msg.ev_msg.ev_ag_dev_dial.phone_num) );
      break;

    case BT_EV_AG_DEV_REDIAL:
      BT_MSG_API( "BT AG EV TX: HF Redialed", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_redial.bd_addr = *bd_addr_ptr;
      break;

    case BT_EV_AG_DEV_MEM_DIAL:
      BT_MSG_API( "BT AG EV TX: HF Memory Dialed", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_mem_dial.bd_addr = *bd_addr_ptr;
      BT_STRCPY( (char*) event_msg.ev_msg.ev_ag_dev_mem_dial.mem_entry,
                 &bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ],
                 sizeof(event_msg.ev_msg.ev_ag_dev_mem_dial.mem_entry) );
      break;

    case BT_EV_AG_DEV_SEND_DTMF:
      BT_MSG_API( "BT AG EV TX: HF Send DTMF", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_dev_send_dtmf.bd_addr   = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_dev_send_dtmf.dtmf_char = 
        bt_agcpe.rx_buf[ CPE_PARAM_0 ];
      break;

    case BT_EV_AG_VR_TOGGLE:
      BT_MSG_API( "BT AG EV TX: HF Toggled VR", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_vr_state.bd_addr     = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_vr_state.vr_enabled  = HF_DATA.vr_enabled;
      break;

    case BT_EV_AG_RING_FAILED:
      BT_MSG_API( "BT AG EV TX: Ring Failed Event", 0, 0, 0 );
      event_msg.ev_hdr.bt_app_id                  = bt_ag.app_id_ring_device;
      event_msg.ev_msg.ev_ag_ring_failed.bd_addr  = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_ring_failed.event_reason = BT_EVR_AG_CONN_FAILED;
      break;

    case BT_EV_AG_AUDIO_CONNECTED:
      MSG_HIGH( "BT AG: Audio Connected", 0, 0, 0 );
      BT_MSG_API( "BT AG EV TX: Audio Connected Event", 0, 0, 0 );
      if ( bt_ag.app_id_add_audio != BT_APP_ID_NULL )
      {
        event_msg.ev_hdr.bt_app_id = bt_ag.app_id_add_audio;
      }
      event_msg.ev_msg.ev_ag_audio_connected.bd_addr = *bd_addr_ptr;
#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */
      break;

    case BT_EV_AG_AUDIO_CONNECT_FAILED:
      BT_MSG_API( "BT AG EV TX: Audio Connect Failed Event", 0, 0, 0 );
      if ( bt_ag.app_id_add_audio != BT_APP_ID_NULL )
      {
        event_msg.ev_hdr.bt_app_id = bt_ag.app_id_add_audio;
      }
      event_msg.ev_msg.ev_ag_audio_connect_failed.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_audio_connect_failed.event_reason =
        bt_ag.event_reason;
      break;

    case BT_EV_AG_AUDIO_DISCONNECTED:
      MSG_HIGH( "BT AG: Audio Disconnected", 0, 0, 0 );
      BT_MSG_API( "BT AG EV TX: Audio Disconnected Event", 0, 0, 0 );
      if ( bt_ag.app_id_add_audio != BT_APP_ID_NULL )
      {
        event_msg.ev_hdr.bt_app_id = bt_ag.app_id_add_audio;
      }
      event_msg.ev_msg.ev_ag_audio_disconnected.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_audio_disconnected.event_reason =
        bt_ag.event_reason;
#ifdef BT_AUDIO_DEBUG
#error code not present
#endif /* BT_AUDIO_DEBUG */
      break;

    case BT_EV_AG_SPKR_GAIN_REPORT:
      BT_MSG_API( "BT AG EV TX: Spkr Gain - %d",
                  AUDIO_DEV.spkr_volume, 0, 0 );
      event_msg.ev_msg.ev_ag_ad_spkr_gain_report.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_ad_spkr_gain_report.ad_speaker_gain =
        AUDIO_DEV.spkr_volume;
      break;

    case BT_EV_AG_MIC_GAIN_REPORT:
      BT_MSG_API( "BT AG EV TX: Mic Gain - %d",
                  AUDIO_DEV.mic_volume, 0, 0 );
      event_msg.ev_msg.ev_ag_ad_mic_gain_report.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_ad_mic_gain_report.ad_microphone_gain =
        AUDIO_DEV.mic_volume;
      break;

    case BT_EV_AG_CONNECTED:
      event_msg.ev_msg.ev_ag_connected.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_connected.audio_device =
        AUDIO_DEV.device_type;
      MSG_HIGH( "BT AG: RC Connected", 0, 0, 0 );
      BT_MSG_API( "BT AG EV TX: RC Connected AD %x",
        event_msg.ev_msg.ev_ag_connected.audio_device, 0, 0 );
      break;

    case BT_EV_AG_CONNECTION_FAILED:
      BT_MSG_API( "BT AG EV TX: RC Connection Failed", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_connection_failed.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_connection_failed.event_reason = 
        bt_ag.event_reason;
      break;

    case BT_EV_AG_DISCONNECTED:
      MSG_HIGH( "BT AG: RC Disconnected", 0, 0, 0 );
      BT_MSG_API( "BT AG EV TX: RC Disconnected", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_disconnected.bd_addr = *bd_addr_ptr;
      event_msg.ev_msg.ev_ag_disconnected.event_reason = bt_ag.event_reason;
      break;

    default:
      keep_going = FALSE; /* invalid event type */
      break;
  }

  if ( keep_going )
  {
    BT_BDA( MSG_API, "BT AG EV TX:", bd_addr_ptr );
    event_msg.ev_hdr.ev_type = ev_type;
    bt_ec_send_event( &event_msg );  /* send it up */
  }
  else
  {
    BT_ERR( "BT AG: Event NOT SENT T %x", ev_type, 0, 0 );
    BT_BDA( ERR, "BT AG: Event NOT SENT", bd_addr_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_ag_disable

DESCRIPTION
  Reverses the enabling process.

===========================================================================*/
LOCAL void bt_ag_disable
(
  void
)
{
  
  bt_ag.disabling = FALSE;

  /*  Unegister with RM for SCO connection requests.  */
  BT_MSG_API( "BT AG CMD TX: RM unreg SCO", 0, 0, 0 );
  bt_cmd_rm_unregister_sco( bt_ag.app_id_ag );

}


/*===========================================================================

FUNCTION
  bt_ag_sco_cleanup

DESCRIPTION
  Resets AG state to enabled with no audio.

===========================================================================*/
LOCAL void bt_ag_sco_cleanup
(
  void
)
{

  BT_MSG_DEBUG( "BT AG: SCO cleanup; st=%d hndl=%d",
                bt_ag.state, bt_ag.sco_handle, 0 );

  if ( bt_ag.sco_handle != BT_AG_INVALID_SCO_HANDLE )
  {
    BT_MSG_API( "BT AG CMD TX: RM Disconnect SCO", 0, 0, 0 );
    bt_cmd_rm_disconnect_sco( bt_ag.app_id_ag,
                              bt_ag.sco_handle,
                              BT_RMDR_USER_ENDED );
    bt_ag.sco_handle      = BT_AG_INVALID_SCO_HANDLE;
    bt_ag.state           = BT_AGS_AUDIO_DISCONNECTING;
  }
  else
  {
    bt_ag.state           = BT_AGS_CONNECTED;
  }

  bt_ag.app_id_add_audio  = BT_APP_ID_NULL;

}


/*===========================================================================

FUNCTION
  bt_ag_open_sio

DESCRIPTION
  Opens SIO connection.

===========================================================================*/
LOCAL sio_stream_id_type bt_ag_open_sio
(
  sio_open_type*  so_ptr
)
{
  
  sio_stream_id_type  sio_stream_id;

  /*  Open new SIO stream.  */
  sio_stream_id = sio_open( so_ptr );
  if ( sio_stream_id != SIO_NO_STREAM_ID )
  {
    BT_MSG_DEBUG( "BT AG Info: SIO open sid=%d, srvr=%d",
                  sio_stream_id, so_ptr->bt_open_ptr->client_app?0:1, 0 );

    /*  Prevent going idle while trying to connect to AD.  */
    rex_clr_timer( &bt_ag_timer );
  }
  else  /* failed to open SIO connection */
  {
    BT_ERR( "BT AG ERR: sio_open() failed", 0, 0, 0 );
  }

  return ( sio_stream_id );

}


/*===========================================================================

FUNCTION
  bt_ag_close_hs_stream

DESCRIPTION
  Closes HS stream if it exists.

===========================================================================*/
LOCAL void bt_ag_close_hs_stream
(
  void
)
{
  
  if ( !ISBITSET (bt_ag.sio_stream_id_hs, STREAM_CLOSING_B) )
  {
    BT_MSG_DEBUG( "BT AG: closing HS SIO sid=%d",
                  bt_ag.sio_stream_id_hs, 0, 0 );

    sio_close( bt_ag.sio_stream_id_hs, NULL );
    SETBIT ( bt_ag.sio_stream_id_hs, STREAM_CLOSING_B );

    if ( bt_ag.hs_svc_registered != FALSE )
    {
      bt_cmd_sd_unregister_service(
        bt_ag.app_id_ag,
        BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY );
      bt_ag.hs_svc_registered = FALSE;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cm_dereg

DESCRIPTION
  Deregister with Call Manager.

===========================================================================*/
LOCAL void bt_ag_cm_dereg
(
  void
)
{
  
  if ( HF_DATA.cm_client_id == CM_CLIENT_TYPE_BLUETOOTH )
  {
    BT_MSG_DEBUG( "BT AG Info: deactivate & release cm client", 0, 0, 0 );
    cm_client_deact( HF_DATA.cm_client_id );
    cm_client_release( HF_DATA.cm_client_id );
    HF_DATA.cm_client_id = CM_CLIENT_ID_ANONYMOUS;
    HF_DATA.num_calls    = 0;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_hf_cleanup

DESCRIPTION
  Initialize HF related data.

===========================================================================*/
LOCAL void bt_ag_hf_cleanup
(
  void
)
{

  uint8    i;
  boolean  send_slc_event = FALSE;

  if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
  {
    send_slc_event = TRUE;
  }

  HF_DATA.features_bitmap     = 0;
  HF_DATA.connection_state    = BT_AG_HCS_BRSF;
  HF_DATA.notify_call_waiting = FALSE;
  HF_DATA.notify_cli          = FALSE;
  HF_DATA.nrec_off            = FALSE;
  HF_DATA.vr_enabled          = FALSE;
  HF_DATA.ier_mode            = BT_AIER_NO_IER;
  HF_DATA.dtmf_started        = FALSE;
  HF_DATA.profile_version     = HFP_VER_1_0; /* assume car-kit supports ver 1.1*/

  /* HFP 1.5 data */
  HF_DATA.ext_err_enabled     = FALSE;

  for ( i=0; i<BT_AG_RH_MAX; i++)
  {
    HF_DATA.supported_rh[i] = FALSE;
  }

#ifdef BT_AG_HF_EXTENSION
  HF_DATA.selected_pb_idx     = ARR_SIZE( bt_ag_ph_name_table );
  HF_DATA.pb_total_recs  = 0;
  HF_DATA.entries_2_read = 0;
#endif /* BT_AG_HF_EXTENSION */

  if ( (bt_ag.state == BT_AGS_DISABLED) ||
       !BT_AG_IN_CDMA )
  {
    HF_DATA.service_avail       = FALSE;
    HF_DATA.call_active         = FALSE;
    HF_DATA.call_setup_state    = BT_AG_CALL_SETUP_NONE;
    HF_DATA.call_held_state     = BT_AG_CALL_HELD_NONE;
    HF_DATA.call_id             = CM_CALL_ID_INVALID;
    HF_DATA.incoming_call_id    = CM_CALL_ID_INVALID;

    memset( &HF_DATA.phone_num, 0, sizeof( cm_num_s_type ) );
    memset( &HF_DATA.ss_info, 0, sizeof( cm_mm_ss_info_s_type ) );
  }
  /* ELSE, need to keep all call related info across connections
   * to support Enhanced Call Status feature in CDMA.
   *
   * TODO: we should be able to do the same for all serving
   * systems, and hence be able to simplify our code in quite
   * a few places.
   */

  if ( send_slc_event )
  {
    MSG_HIGH( "BT AG: SLC down", 0, 0, 0 );
    bt_ag_send_event( BT_EV_AG_SLC_CHANGED );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_close_hf_stream

DESCRIPTION
  Closes HF stream if it exists.

===========================================================================*/
LOCAL void bt_ag_close_hf_stream
(
  void
)
{
  
  if ( !ISBITSET (bt_ag.sio_stream_id_hf, STREAM_CLOSING_B) )
  {
    BT_MSG_DEBUG( "BT AG: closing HF SIO sid=%d",
                  bt_ag.sio_stream_id_hf, 0, 0 );

    sio_close( bt_ag.sio_stream_id_hf, NULL );
    SETBIT ( bt_ag.sio_stream_id_hf, STREAM_CLOSING_B );

    if ( bt_ag.hf_svc_registered != FALSE )
    {
      bt_cmd_sd_unregister_service(
        bt_ag.app_id_ag,
        BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY );
      bt_ag.hf_svc_registered = FALSE;
    }

    bt_ag_hf_cleanup();
  }

}


/*===========================================================================

FUNCTION
  bt_ag_close_sio

DESCRIPTION
  Closes existing SIO connection(s).

===========================================================================*/
LOCAL void bt_ag_close_sio
(
  void
)
{
  
  dsm_empty_queue( &bt_ag.tx_wm );
  dsm_empty_queue( &bt_ag.rx_wm );
  dsm_free_packet( &bt_ag.tx_dsm_ptr );

  bt_ag_close_hs_stream();
  bt_ag_close_hf_stream();

  bt_ag.sio_stream_id = SIO_NO_STREAM_ID;

}


/*===========================================================================

FUNCTION
  bt_ag_update_sio_conn_status

DESCRIPTION
  This callback function is called by SIO when SPP connection status changed.

===========================================================================*/
LOCAL void bt_ag_update_sio_conn_status
(
  bt_spp_status_type*  spp_status_ptr
)
{

  bt_ag_ev_type* ev_ptr;

  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    ev_ptr->hdr.ev_type       = BT_AG_SPP_EV;
    ev_ptr->spp_ev.spp_status = *spp_status_ptr;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG ERR: dropping SIO conn status ev", 0, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_init_sio_open_params

DESCRIPTION
  Initializes the values in the specified SIO structures.

===========================================================================*/
LOCAL void bt_ag_init_sio_open_params
(
  sio_open_type*     so_ptr,
  bt_spp_open_type*  bso_ptr
)
{

  /*  Fill out generic SIO information.  */
  so_ptr->port_id        = SIO_PORT_BT_SPP;
  so_ptr->rx_flow        = SIO_FCTL_OFF;
  so_ptr->tx_flow        = SIO_FCTL_OFF;
  so_ptr->stream_mode    = SIO_GENERIC_MODE;
  so_ptr->tx_queue       = &bt_ag.tx_wm;
  so_ptr->rx_queue       = &bt_ag.rx_wm;
  so_ptr->tail_char_used = FALSE;
  so_ptr->rx_func_ptr    = NULL;

  /*  Fill out Bluetooth specific information.  */
  so_ptr->bt_open_ptr           = bso_ptr;
  bso_ptr->status_change_fptr   = bt_ag_update_sio_conn_status;
  bso_ptr->config_change_fptr   = NULL;
  bso_ptr->modem_status_fptr    = NULL;
  bso_ptr->line_error_fptr      = NULL;
  bso_ptr->max_frame_size       = BT_SPP_MFS_AUTOMATIC;
  bso_ptr->service_version      = 0;
  bso_ptr->service_name_str_ptr = NULL;

}

/*===========================================================================

FUNCTION
  bt_ag_sio_opened

DESCRIPTION
  Saves info for the SIO stream that just got opened.

===========================================================================*/
LOCAL void bt_ag_sio_opened
(
  bt_sd_uuid_type    uuid, 
  sio_stream_id_type sio_stream_id
)
{

  switch( uuid )
  {
    case BT_SD_SERVICE_CLASS_HEADSET:
    case BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY:
      bt_ag.sio_stream_id_hs = sio_stream_id;
      if ( bt_ag.hs_svc_registered == FALSE )
      {
        bt_cmd_sd_register_serv_ext (
          bt_ag.app_id_ag,
          BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY,
          BT_AG_HS_PROFILE_VERSION_NUMBER,
          BT_RC_SCN_AG_HS,
          BT_L2_PSM_INVALID,
          FALSE,  /*  Do not page scan for this SD registration.  */
          BT_AG_SERVICE_NAME );
        bt_ag.hs_svc_registered = TRUE;
      }
      break;

    case BT_SD_SERVICE_CLASS_HANDSFREE:
    case BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY:
      bt_ag.sio_stream_id_hf = sio_stream_id;
      if ( bt_ag.hf_svc_registered == FALSE )
      {
        bt_cmd_sd_register_serv_ext (
          bt_ag.app_id_ag,
          BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY,
          BT_AG_HF_PROFILE_VERSION_NUMBER,
          BT_RC_SCN_AG_HF,
          BT_L2_PSM_INVALID,
          FALSE,  /*  Do not page scan for this SD registration.  */
          BT_AG_SERVICE_NAME );
        bt_ag.hf_svc_registered = TRUE;
      }
      break;

    default:
      BT_ERR( "BT AG: SIO opened, Bad UUID %x", uuid, 0, 0 );
      break;
  }

}

/*===========================================================================

FUNCTION
  bt_ag_open_sio_server

DESCRIPTION
  Opens SIO connection in server mode.

===========================================================================*/
LOCAL sio_stream_id_type bt_ag_open_sio_server
(
  bt_sd_uuid_type  uuid
)
{

  sio_open_type        so;
  bt_spp_open_type     bso;
  sio_stream_id_type   sio_stream_id;
  bt_rc_scn_enum_type  scn;

  switch ( uuid )
  {
    case BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY:
      scn = BT_RC_SCN_AG_HS;
      break;
    case BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY:
      scn = BT_RC_SCN_AG_HF;
      break;
    default:
      BT_ERR( "BT AG: Open SIO Server Bad UUID %x", uuid, 0, 0 );
      return SIO_NO_STREAM_ID;
  }

  bt_ag_init_sio_open_params( &so, &bso );

  /*  Set server specific parameters.  */
  bso.client_app        = FALSE;
  bso.service_uuid      = BT_SD_INVALID_UUID;
  bso.bd_addr_ptr       = NULL;
  bso.rc_server_channel = (bt_spp_scn_enum_type) scn;

  sio_stream_id = bt_ag_open_sio( &so );

  /*  Set the non_empty_func_ptr back to NULL because BT SIO will save  */
  /*  AG's callback and replace it with its own, which would create a   */
  /*  problem if AG does a SIO open again with the same watermark       */
  /*  (server mode only), which will now have the SIO's callback, not   */
  /*  AG's.  Ideally, we'd like to use a separate set of watermarks     */
  /*  for each SIO open, but this is too complicated for AG.            */
  so.tx_queue->non_empty_func_ptr = NULL;

  /*  Save stream ID.  */
  if ( sio_stream_id != SIO_NO_STREAM_ID )
  {
    bt_ag_sio_opened( uuid, sio_stream_id );
  }

  return ( sio_stream_id );

}


/*===========================================================================

FUNCTION
  bt_ag_open_sio_client

DESCRIPTION
  Opens SIO connection in client mode.

===========================================================================*/
LOCAL sio_stream_id_type bt_ag_open_sio_client
(
  bt_bd_addr_type*  bd_addr_ptr,
  bt_sd_uuid_type   uuid
)
{

  sio_open_type       so;
  bt_spp_open_type    bso;
  sio_stream_id_type  sio_stream_id;

  bt_ag.state = BT_AGS_OPENING;

  bt_ag_init_sio_open_params( &so, &bso );

  /* Set client specific parameters */
  bso.client_app            = TRUE;
  bso.service_uuid          = uuid;
  bso.bd_addr_ptr           = bd_addr_ptr;
  bso.rc_server_channel     = BT_SPP_SCN_UNSPECIFIED;

  sio_stream_id = bt_ag_open_sio( &so );

  /* Save stream ID */
  if ( sio_stream_id != SIO_NO_STREAM_ID )
  {
    bt_ag_sio_opened( uuid, sio_stream_id );
  }

  return ( sio_stream_id );

}


/*===========================================================================

FUNCTION
  bt_ag_go_idle

DESCRIPTION
  Enters idle mode.

===========================================================================*/
LOCAL void bt_ag_go_idle
(
  void
)
{
  
  boolean  success = FALSE;

  BT_MSG_DEBUG( "BT AG: AG going idle im=%x it=%x fsm=%x", 
                bt_ag.idle_mode, bt_ag.idle_timeout, bt_ag.force_server_mode );

  bt_ag.state   = BT_AGS_OPENING;
  bt_ag.is_idle = TRUE;

  if ( (bt_ag.force_server_mode == TRUE) || 
       (bt_ag.idle_mode == BT_AGIM_AUTOMATIC) ||
       (BT_BD_ADDRS_EQUAL( &AUDIO_DEV.bd_addr, &null_bd_addr ) == TRUE) )
  {
    /*  Server.  */
    bt_ag.is_sio_server = TRUE;

    if ( AUDIO_DEV.bd_addr_ptr == NULL )
    {
      /*  Accept connection from all remote audio devices.   */
      /*  Open both the Headset and Hands-Free SIO streams.  */
      if ( (bt_ag_open_sio_server(
              BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY ) != 
                SIO_NO_STREAM_ID) &&
           (bt_ag_open_sio_server(
              BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY ) !=
                SIO_NO_STREAM_ID) )
      {
        success = TRUE;
      }
    }
    else
    {
      if ( bt_ag_open_sio_server(
             bt_ag.service_class ) != SIO_NO_STREAM_ID )
      {
        success = TRUE;
      }
    }
  }
  else if ( (bt_ag.idle_mode == BT_AGIM_LOW_POWER) &&
            (BT_BD_ADDRS_EQUAL( &AUDIO_DEV.bd_addr, &null_bd_addr ) != TRUE) )
  {
    /*  Client.  */
    bt_ag.is_sio_server = FALSE;

    if ( bt_ag_open_sio_client(
           &AUDIO_DEV.bd_addr,
           AUDIO_DEV.service_uuid ) != SIO_NO_STREAM_ID )
    {
      success = TRUE;
    }
  }
  else
  {
    BT_ERR( "BT AG: Bad server/client FSM %x IM %x BDAP %x",
            bt_ag.force_server_mode, bt_ag.idle_mode,
            AUDIO_DEV.bd_addr_ptr  );
  }

  if ( success == FALSE )
  {
    bt_ag_disable();
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_config

DESCRIPTION
  Configure the various modes in AG.  UI can configure AG at anytime.

===========================================================================*/
LOCAL void bt_ag_cmd_config
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{

  bt_ag_idle_mode_type current_idle_mode    = bt_ag.idle_mode;
  uint32               current_idle_timeout = bt_ag.idle_timeout;

  BT_MSG_API( "BT AG CMD RX: AG Config IM %x ITO %x AID %x",
              ag_cmd_ptr->cmd_msg.cmd_ag_config.idle_mode,
              ag_cmd_ptr->cmd_msg.cmd_ag_config.idle_timeout,
              ag_cmd_ptr->cmd_hdr.bt_app_id );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /*  Save new configuration.  */
  bt_ag.idle_mode    = ag_cmd_ptr->cmd_msg.cmd_ag_config.idle_mode;
  bt_ag.idle_timeout = ag_cmd_ptr->cmd_msg.cmd_ag_config.idle_timeout * 1000;

  BT_MSG_DEBUG( "BT AG: AG Config IM %x ITO %x AID %x",
                bt_ag.idle_mode, bt_ag.idle_timeout, 
                ag_cmd_ptr->cmd_hdr.bt_app_id );

  /*  Actions required only if idle mode changed while idle.  */
  if ( (current_idle_mode != bt_ag.idle_mode) &&  /*  Idle mode changed?  */
       (bt_ag.state >= BT_AGS_OPENING) &&         /*  Enabled?            */
       (bt_ag.is_idle != FALSE) )                 /*  Idle?               */
  {
    bt_ag.going_idle = TRUE;
    bt_ag_close_sio();
  }
  else if ( current_idle_timeout != bt_ag.idle_timeout )
  {
    if ( (bt_ag.idle_timeout > 0) && (bt_ag.state == BT_AGS_CONNECTED) )
    {
      rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );  /*  Reset timer.  */
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_check_and_save_params

DESCRIPTION
  Validates and saves the parameters passed in via either the Enable or
  Connect command.

===========================================================================*/
LOCAL boolean bt_ag_check_and_save_params
(
  bt_app_id_type        app_id,
  bt_bd_addr_type*      bd_addr_ptr,
  bt_audio_device_type  dev_type,
  boolean               enabling
)
{

  boolean params_valid = TRUE;

  /*  Check if BD Addr is specified or not.  */
  if ( BT_BD_ADDRS_EQUAL( bd_addr_ptr, &null_bd_addr ) == TRUE )
  {
    if ( enabling != FALSE )
    {
      /*  BD Addr not specified - server.  */
      AUDIO_DEV.bd_addr_ptr = NULL;
    }
    else
    {
      /*  A BD address is required for Connect command.  */
      params_valid = FALSE;
    }
  }
  else
  {
    /*  BD Addr specified - server or client depending on idle mode.  */
    
    switch ( dev_type )
    {
      case BT_AD_HEADSET:
        bt_ag.service_class = BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY;
        bt_ag.profile_version_number = BT_AG_HS_PROFILE_VERSION_NUMBER;
        AUDIO_DEV.service_uuid = BT_SD_SERVICE_CLASS_HEADSET;
        AUDIO_DEV.server_channel = BT_RC_SCN_AG_HS;
        break;
      case BT_AD_HANDSFREE:
        bt_ag.service_class = BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY;
        bt_ag.profile_version_number = BT_AG_HF_PROFILE_VERSION_NUMBER;
        AUDIO_DEV.service_uuid = BT_SD_SERVICE_CLASS_HANDSFREE;
        AUDIO_DEV.server_channel = BT_RC_SCN_AG_HF;
        break;
      default:
        BT_ERR( "BT AG: Bad audio device %x", dev_type, 0, 0 );
        params_valid = FALSE;
        break;
    }

    if ( params_valid &&
         (enabling != FALSE) )
    {
      AUDIO_DEV.bd_addr_ptr = &AUDIO_DEV.bd_addr;

      bt_ag.enabled_bd_addr       = *bd_addr_ptr;
      bt_ag.enabled_device_type   = dev_type;
      bt_ag.enabled_service_class = bt_ag.service_class;
    }
  }

  if ( params_valid &&
       ((enabling != FALSE) ||
        (bt_ag.state == BT_AGS_CLOSED)) )  // Connect cmd can be processed now
  {
    /*  Save parameters.  */
    bt_ag.app_id_user     = app_id;
    AUDIO_DEV.bd_addr     = *bd_addr_ptr;
    AUDIO_DEV.device_type = dev_type;
  }

  return ( params_valid );

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_enable

DESCRIPTION
  Enables/starts the Audio Gateway application with specified bd_addr.
  Enters idle mode.

===========================================================================*/
LOCAL void bt_ag_cmd_enable
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  bt_cmd_ag_enable_type*  enable_cmd_ptr =
                            &ag_cmd_ptr->cmd_msg.cmd_ag_enable;

  BT_MSG_API( "BT AG CMD RX: Enable Dev %x AID %x",
              enable_cmd_ptr->audio_device,
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT AG EV RX: Enable ", &enable_cmd_ptr->bd_addr );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag.state == BT_AGS_DISABLED )
  {
    /*  Check parameters.  */
    if ( bt_ag_check_and_save_params( ag_cmd_ptr->cmd_hdr.bt_app_id,
                                      &enable_cmd_ptr->bd_addr,
                                      enable_cmd_ptr->audio_device,
                                      TRUE ) != FALSE )
    {
      bt_ag_call_info_table_cleanup();
      bt_ag_go_idle();
      if ( (bt_ag.sio_stream_id_hs != SIO_NO_STREAM_ID) ||
           (bt_ag.sio_stream_id_hf != SIO_NO_STREAM_ID) )
      {
        /*  Register with RM for SCO connection requests.  */
        BT_MSG_API( "BT AG CMD TX: RM reg SCO", 0, 0, 0 );
        bt_cmd_rm_register_sco( bt_ag.app_id_ag );
      }
      else
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_SIO_STREAM;
      }
    }
    else
    {
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
    }
  }
  else
  {
    BT_ERR( "BT AG: Already Enabled St %x", bt_ag.state, 0, 0 );
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_ALREADY_ENABLED;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_connect

DESCRIPTION
  Initiates a connection to remote device.

===========================================================================*/
LOCAL void bt_ag_cmd_connect
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{

  bt_cmd_ag_connect_type*  conn_cmd_ptr =
                             &ag_cmd_ptr->cmd_msg.cmd_ag_connect;

  BT_MSG_API( "BT AG CMD RX: Connect Dev %x St %x AID %x",
              conn_cmd_ptr->audio_device,
              bt_ag.state, ag_cmd_ptr->cmd_hdr.bt_app_id );
  BT_BDA( MSG_API, "BT AG CMD RX: Connect", &conn_cmd_ptr->bd_addr );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( bt_ag_check_and_save_params( ag_cmd_ptr->cmd_hdr.bt_app_id,
                                    &conn_cmd_ptr->bd_addr,
                                    conn_cmd_ptr->audio_device,
                                    FALSE ) != FALSE )
  {
    switch ( bt_ag.state )
    {
      case BT_AGS_DISABLED:
        BT_ERR( "BT AG: AG Conn not enabled St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
        break;

      case BT_AGS_CLOSED:
        bt_ag.is_idle       = FALSE;
        bt_ag.is_sio_server = FALSE;
        bt_ag.conn_attempts = 0;
            
        if ( bt_ag_open_sio_client(
               &AUDIO_DEV.bd_addr,
               AUDIO_DEV.service_uuid ) == SIO_NO_STREAM_ID )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_SIO_STREAM;
        }
        break;

      case BT_AGS_OPENING:
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
        break;
      
      case BT_AGS_OPEN:
        bt_ag.is_sio_server            = FALSE;
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
        bt_ag_close_sio();
        break;

      case BT_AGS_CONNECTED:
      case BT_AGS_AUDIO_CONNECTING:
      case BT_AGS_AUDIO_DISCONNECTING:
      case BT_AGS_AUDIO_CONNECTED:
        BT_BDA( MSG_DEBUG, "BT AG: already connected", &AUDIO_DEV.bd_addr );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_ALREADY_CONNECTED;
        bt_ag_send_event( BT_EV_AG_CONNECTED );
        break;

      default:
        BT_ERR( "BT AG: AG Conn Bad state %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
        break;
    }
  }
  else
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_uint32toa

DESCRIPTION
  Converts a decimal number to a character string.  Caller must make sure
  char_string is large enough to hold the result.

===========================================================================*/
LOCAL uint8 bt_ag_uint32toa
(
  uint32  num,          /*  The number to be converted.  */
  char*   char_string   /*  The destination string.      */
)
{

  uint8  i = 0, start, end, cur_char;

  /*  Generate digits in reverse order.  */
  do
  {
    char_string[ i++ ] = (num % 10) + '0';
  } while ( (num /= 10) > 0 );

  /*  Reverse the digits.  */
  for ( start = 0, end = (i - 1); start < end; start++, end-- )
  {
    cur_char             = char_string[ start ];
    char_string[ start ] = char_string[ end ];
    char_string[ end ]   = cur_char;
  }

  return ( i );

}


/*===========================================================================

FUNCTION
  bt_ag_atouint32

DESCRIPTION
  Converts a string of numbers to an unsigned decimal number.

===========================================================================*/
LOCAL uint32 bt_ag_atouint32
(
  char*  char_string  /*  The string to be converted.  */
)
{

  uint32  num = 0;
  while ( *char_string != NULL )  /*  End of string?  */
  {
    num *= 10;
    num += (*char_string - '0');
    char_string++;  /*  Move on to next digit in string.  */
  }

  return ( num );

}


/*===========================================================================

FUNCTION
  bt_ag_param2int32

DESCRIPTION
  Converts the AT command parameter string started at 'idx' in
  bt_agcpe.rx_buf to a 32-bit integer.  Returns -1 if the string
  contains non-digit character.

===========================================================================*/
LOCAL int32 bt_ag_param2int32
( 
  uint16  idx
)
{

  int32  num = 0;

  while ( bt_agcpe.rx_buf[ idx ] != NULL )  /*  End of string?  */
  {
    if ( IS_DIGIT( bt_agcpe.rx_buf[ idx ] ) )
    {
      num *= 10;
      num += (bt_agcpe.rx_buf[ idx ] - '0');

      /*  Move on to next digit in string.  */
      idx++;
    }
    else
    {
      num = -1;
      break;
    }
  }

  return ( num );

}


/*===========================================================================

FUNCTION
  bt_ag_build_chld_resp_str

DESCRIPTION
  Builds a string as response to AT+CHLD=? sent by HF.  Depending on current
  cellular system (if any), more or fewer call hold operations can be done.

===========================================================================*/
LOCAL void bt_ag_build_chld_resp_str
(
  char*  resp_str,  /* Pointer to the string being built */
  uint16 resp_str_size
)
{
  enum {
    SYS_NEITHER,
    SYS_1X,
    SYS_GW
  } curr_sys;

  if ( ! MULTIPARTY_SUPPORTED() )
  {
    // multiparty call feature is not supported, yet HF is issuing
    // AT+CHLD=? which is a violation of HFP specs; GSM 07.07 states
    // that the error response to send is +CME ERROR: <some err code>, 
    // which is not covered by HFP specs;  so the only thing we can do is
    // respond a list of single item 0 to indicate we can reject a call
    // (some HFs chokes at an empty list)
    curr_sys = SYS_NEITHER; /* indicates sending empty list */
  }
  else if ( BT_AG_IN_CDMA )
  {
    curr_sys = SYS_1X;
  }
  else if ( BT_AG_IN_GW )
  {
    curr_sys = SYS_GW;
  }
  else /* hasn't camped onto any cellular system */
  {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) && \
    (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#error code not present
#elif (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#else
    curr_sys = SYS_1X;
#endif
  }

  switch ( curr_sys )
  {
    case SYS_GW:
      /* removed '4' from the string until we have proper facility
      ** to verify our support on AT+CHLD=4.
      */
      BT_STRCPY( resp_str, "(0,1,2,3)", resp_str_size );
#ifdef FEATURE_BT_HFP_1_5
      /* AG response depends on HF's HFP version */
      if ( HF_DATA.profile_version >= HFP_VER_1_2 )
      {
        /* removed '4' from the string until we have proper facility
        ** to verify our support on AT+CHLD=4.
        */
        BT_STRCPY( resp_str, "(0,1,2,3,1x,2x)", resp_str_size );
      }
#endif /* FEATURE_BT_HFP_1_5 */
      break;
    case SYS_1X:
      BT_STRCPY( resp_str, "(1,2)", resp_str_size );
      break;
    case SYS_NEITHER:
    default:
      BT_STRCPY( resp_str, "(0)", resp_str_size );
      break;
  }
}

/*===========================================================================

FUNCTIONS
  bt_ag_external_io_final_response

DESCRIPTION
  Returns TRUE if given DSM item holds the final response.

===========================================================================*/
static boolean bt_ag_external_io_final_response( dsm_item_type* dsm_ptr )
{
  boolean is_final = FALSE;
  char buf[10] = "";
  uint16 haystack_len = dsm_length_packet(dsm_ptr);
  uint16 needle_len = MIN(haystack_len, sizeof(buf));
  if ((haystack_len > 0) &&
      (dsm_extract(dsm_ptr, haystack_len-needle_len, buf, sizeof(buf)) > 0))
  {
    if ( (strstr(buf, "OK") != NULL) || (strstr(buf, "ERROR") != NULL) )
    {
      BT_MSG_DEBUG( "BT AG: final external response", 0, 0, 0 );
      is_final = TRUE;
    }
  }
  return is_final;
}

/*===========================================================================

FUNCTION
  bt_ag_cpe_dequeuing_external_io_tx_wm

DESCRIPTION
  Dequeues external I/O TX WM if there's no pending data on TX DSM chain.

===========================================================================*/
LOCAL void bt_ag_cpe_dequeuing_external_io_tx_wm
(
  void
)
{
  dsm_item_type* dsm_ptr;
  uint32 len = dsm_length_packet( bt_agcpe.tx_dsm_ptr );

  BT_MSG_DEBUG( "dequeuing_external_io_tx_wm - have=%d", len, 0, 0 );
  if ( (len == 0) && ((dsm_ptr = dsm_dequeue( bt_agcpe.tx_wm_ptr )) != NULL) )
  {
    if (bt_ag_external_io_final_response( dsm_ptr ) != FALSE)
    {
      bt_agcpe.respond_pending = FALSE; // expect no more from upper layer
    }
    dsm_append( &bt_agcpe.tx_dsm_ptr, &dsm_ptr );
  }
}

/*===========================================================================

FUNCTION
  bt_ag_send_data

DESCRIPTION
  Transmits data with flow control restrictions as necessary.  
  Data from external I/O gets sent first, then internal data.

===========================================================================*/
LOCAL void bt_ag_send_data( void )
{
  uint32 len;
  uint32 room;
  dsm_item_type* dsm_ptr;

  while ( bt_ag_ok_to_send() && 
          (dsm_length_packet( *bt_ag.cur_dsm_ptr_ptr ) > 0) )
  {
    room = bt_ag.tx_wm.dont_exceed_cnt - dsm_queue_cnt( &bt_ag.tx_wm );
    len = dsm_dup_packet( &dsm_ptr, *bt_ag.cur_dsm_ptr_ptr, 0, room );
    dsm_pullup( bt_ag.cur_dsm_ptr_ptr, NULL, len );
    BT_MSG_DEBUG( "send_data: len=%d room=%d", len, room, 0 );
    if ( bt_ag.cur_dsm_ptr_ptr == &bt_agcpe.tx_dsm_ptr )
    {
      BT_MSG_DEBUG( "BT AG TX: ext IO data", 0, 0, 0 );
      bt_ag_cpe_dequeuing_external_io_tx_wm();
      if ( (len < room) && (bt_agcpe.respond_pending == FALSE) )
      {
        bt_ag.cur_dsm_ptr_ptr = &bt_ag.tx_dsm_ptr;
      }
    }
    else
    {
      BT_MSG_DEBUG( "BT AG TX: internal IO data", 0, 0, 0 );
      if ( (len < room) && (dsm_length_packet( bt_agcpe.tx_dsm_ptr ) > 0) )
      {
        bt_ag.cur_dsm_ptr_ptr = &bt_agcpe.tx_dsm_ptr;
      }
    }
    sio_transmit( bt_ag.sio_stream_id, dsm_ptr );
  }
}

/*===========================================================================

FUNCTION
  bt_ag_cpe_send_response

DESCRIPTION
  Builds and sends a response to HS/HF.
  AG typically sends responses like OK and ERROR.
  AG can send unsolicited result codes like RING, +VGS, +VGM, etc.
  All AG responses to HS are of the format \r\n<response>\r\n since the
  verbose state is 1 for AG to HS/HF result codes.

===========================================================================*/
LOCAL void bt_ag_cpe_send_response
(
  bt_ag_response_type  resp,    /*  The response to send to HS.  */
  uint32               generic  /*  Additional info.             */
)
{

  char            param_str[ MAX(BT_AG_RESP_MAX_LEN,CM_MAX_NUMBER_CHARS) + 10 ];
  dsm_item_type*  dsm_ptr    = NULL;
  uint32          len        = 0;
  uint32          features_bitmap;
  boolean         keep_going = FALSE;
  uint32          to_value;
  uint8           num_type = 0;

  if ( (bt_ag.state > BT_AGS_OPEN) && (bt_ag.state < BT_AGS_CLOSING) )
  {
    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_RFCOMM, 1 );
    if ( dsm_ptr != NULL )
    {
      keep_going = TRUE;
    }
  }

  if ( keep_going )
  {
    /*  Start with prefix.  */
    len = strlen( BT_AG_RESP_PREFIX );
    if ( dsm_pushdown_tail( &dsm_ptr, BT_AG_RESP_PREFIX,
                            len, DSM_DS_SMALL_ITEM_POOL ) == len )
    {
      /* make sure the right error response is sent */
      if ( (resp == BT_AG_RESP_CME_ERROR) &&
           (HF_DATA.ext_err_enabled == FALSE) )
      {
        resp = BT_AG_RESP_ERROR;
      }

      /*  Append the response.  */
      len = strlen( rsps_from_ag[ resp ] );
      if ( dsm_pushdown_tail( &dsm_ptr, rsps_from_ag[ resp ],
                              len, DSM_DS_SMALL_ITEM_POOL ) == len )
      {
        keep_going = TRUE;
      }
    }
  }

  if ( keep_going )
  {
    /*  Append parameters as necessary.  */
    switch ( resp )
    {
      case BT_AG_RESP_OK:     /*  ACK to command execution.  */
        BT_MSG_DEBUG( "BT AG TX: OK", 0, 0, 0 );
        /*  No parameters.  */
        break;

      case BT_AG_RESP_ERROR:  /*  Error indication code.  */
        BT_MSG_DEBUG( "BT AG TX: ERROR", 0, 0, 0 );
        /*  No parameters.  */
        break;

      case BT_AG_RESP_RING:   /*  Incoming call indication.  */
        BT_MSG_DEBUG( "BT AG TX: RING", 0, 0, 0 );
        /*  No parameters.  */
        break;

      case BT_AG_RESP_VGM:   /*   Microphone volume gain report.  */
        BT_MSG_DEBUG( "BT AG TX: +VGM: %d", AUDIO_DEV.mic_volume, 0, 0 );
        len        = bt_ag_uint32toa( AUDIO_DEV.mic_volume, param_str );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;

      case BT_AG_RESP_VGS:   /*  Speaker volume gain report.  */
        BT_MSG_DEBUG( "BT AG TX: +VGS: %d", AUDIO_DEV.spkr_volume, 0, 0 );
        len        = bt_ag_uint32toa( AUDIO_DEV.spkr_volume, param_str );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;

      case BT_AG_RESP_BINP:  /*  BT input response.  */
        break;

      case BT_AG_RESP_BRSF:  /*  Supported features in AG.  */
        features_bitmap = bt_ag.features_bitmap | BT_AG_F_CALL_REJECT_ABLE;
#ifdef FEATURE_BT_HFP_1_5
        /* AG response depends on HF's HFP version */
        if ( HF_DATA.profile_version >= HFP_VER_1_2 )
        {
          features_bitmap |= BT_AG_F_ENH_CALL_STATUS
                          |  BT_AG_F_EXTENDED_ERR_CODE
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
                          ;
        }                                                  
#endif /* FEATURE_BT_HFP_1_5 */
        len        = bt_ag_uint32toa( features_bitmap, param_str );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        BT_MSG_DEBUG( "BT AG TX: +BRSF: %d (0x%04x)", 
                      features_bitmap, features_bitmap, 0 );
        break;

      case BT_AG_RESP_BSIR:  /*  In-band ring tone in AG.  */
        param_str[0] = ((boolean)generic == TRUE) ? '1' : '0';
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, 1,
                                          DSM_DS_SMALL_ITEM_POOL ) == 1 );
        BT_MSG_DEBUG( "BT AG TX: +BSIR: %c", param_str[0], 0, 0 );
        break;

      case BT_AG_RESP_BVRA:  /*  VR function in AG.  */
        param_str[0] = (HF_DATA.vr_enabled ? '1' : '0');
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, 1,
                                          DSM_DS_SMALL_ITEM_POOL ) == 1 );
        BT_MSG_DEBUG( "BT AG TX: +BVRA: %c", param_str[0], 0, 0 );
        break;

      case BT_AG_RESP_CCWA:  /*  Call waiting notification.  */
#ifdef FEATURE_BT_HFP_1_5
        num_type = (HF_DATA.phone_num.buf[ 0 ] == '+') ?
                   BT_AG_NUM_TYPE_INTERNATIONAL : BT_AG_NUM_TYPE_UNKNOWN;
#endif
        /*  Build the string "phone num,type,1"  */
        len        = snprintf( param_str, sizeof( param_str ), "\"%s\",%d,1", 
                               HF_DATA.phone_num.buf, num_type );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        snprintf( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ) - 1,
                  "BT AG TX: %s %s", rsps_from_ag[ resp ], param_str );
        bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = NULL;
        BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
        break;

      case BT_AG_RESP_CHLD:  /*  Call hold and multiparty services.  */
      {
        bt_ag_build_chld_resp_str( param_str, sizeof(param_str) );
        len = strlen( param_str );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        BT_STRFORMAT( bt_ag_qxdm_msg, sizeof(bt_ag_qxdm_msg), 
                      "BT AG TX: +CHLD: %s", param_str );
        BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
        break;
      }
      case BT_AG_RESP_CIEV:  /*  Indicator event.  */
        /*  Build the string "<ind>,<val>"  */
        bt_ag_uint32toa( generic, &param_str[0] );
        param_str[1] = ',';
        switch ( (bt_ag_indicator_type)generic )
        {
          case BT_AG_INDICATOR_NONE:
            keep_going = FALSE;
            break;
          case BT_AG_INDICATOR_SERVICE:
            param_str[2] = HF_DATA.service_avail ? '1' : '0';
            break;
          case BT_AG_INDICATOR_CALL:
            param_str[2] = HF_DATA.call_active ? '1' : '0';
            break;
          case BT_AG_INDICATOR_CALL_SETUP:
            bt_ag_uint32toa( HF_DATA.call_setup_state, &param_str[2] );
            break;
          case BT_AG_INDICATOR_SIGNAL:
            bt_ag_uint32toa( (uint32)(HF_DATA.rssi), &param_str[2] );
            break;
          case BT_AG_INDICATOR_BATTCHG:
            bt_ag_uint32toa( (uint32)(HF_DATA.battchg), &param_str[2] );
            break;
          case BT_AG_INDICATOR_ROAM:
            bt_ag_uint32toa( (uint32)(HF_DATA.roam), &param_str[2] );
            break;
          case BT_AG_INDICATOR_CALL_HELD:
            bt_ag_uint32toa( HF_DATA.call_held_state, &param_str[2] );
            break;
          default:
            BT_ERR( "BT AG: Send CIEV bad type %x", generic, 0, 0 );
            keep_going = FALSE;
            break;
        }
        if ( keep_going )
        {
          BT_MSG_DEBUG( "BT AG: Send +CIEV: %c,%c",
                        param_str[0], param_str[2], 0 );
          keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, 3,
                                            DSM_DS_SMALL_ITEM_POOL ) == 3 );
        }
        break;

      case BT_AG_RESP_CIND:
        BT_MSG_DEBUG( "BT AG TX: +CIND: ", 0, 0, 0 );
        if ( (bt_ag_resp_sub_type) generic == BT_AG_RESP_INFO )
        {
          /* Build the string "<service ind>,<call ind> *
           * ,<callsetup ind>"                          */
          len = BT_STRFORMAT( param_str, sizeof(param_str), "%d,%d,%d",
                              HF_DATA.service_avail,
                              HF_DATA.call_active,
                              HF_DATA.call_setup_state );
#ifdef FEATURE_BT_HFP_1_5
          /* AG response depends on HF's HFP version */
          if ( HF_DATA.profile_version >= HFP_VER_1_2 )
          {
            /* Add the string "<RSSI>,<Battchg ind>,<Roam ind>,<callheld ind>" */
            len += BT_STRFORMAT( &param_str[len], sizeof(param_str)-len,
                                 ",%d,%d,%d,%d",
                                 HF_DATA.rssi,
                                 HF_DATA.battchg,
                                 HF_DATA.roam,
                                 HF_DATA.call_held_state );
          }                  
#endif /* FEATURE_BT_HFP_1_5 */
          keep_going = ( dsm_pushdown_tail(
                           &dsm_ptr, param_str, len,
                           DSM_DS_SMALL_ITEM_POOL ) == len );

          BT_STRCPY( bt_ag_qxdm_msg, param_str, sizeof(bt_ag_qxdm_msg) );
          BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
        }
        else /*  Must be BT_AG_RESP_LIST.  */
        {
          len = strlen( cind_mapping_params );
          keep_going = ( dsm_pushdown_tail(
                           &dsm_ptr, cind_mapping_params, len,
                           DSM_DS_SMALL_ITEM_POOL ) == len );

          BT_MSG_DEBUG( cind_mapping_params, 0, 0, 0 );

#ifdef FEATURE_BT_HFP_1_5
          /* AG response depends on HF's HFP version */
          if ( HF_DATA.profile_version >= HFP_VER_1_2 )
          {
            len = strlen( cind_mapping_1_5_params );
            keep_going = ( dsm_pushdown_tail(
                           &dsm_ptr, cind_mapping_1_5_params, len,
                           DSM_DS_SMALL_ITEM_POOL ) == len );

            BT_MSG_DEBUG( cind_mapping_1_5_params, 0, 0, 0 );
          }
#endif /* FEATURE_BT_HFP_1_5 */                           
        }
        break;

#ifdef FEATURE_BT_HFP_1_5
      case BT_AG_RESP_CLCC:  /*  List info on a single call.  */
        /*  Build the string  */
        len = strlen( (const char*) generic );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, (void*)generic, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;
#endif /* FEATURE_BT_HFP_1_5 */

      case BT_AG_RESP_CLIP:  /*  Calling line identification notification.  */
#ifdef FEATURE_BT_HFP_1_5
        num_type = (HF_DATA.phone_num.buf[ 0 ] == '+') ?
                   BT_AG_NUM_TYPE_INTERNATIONAL : BT_AG_NUM_TYPE_UNKNOWN;
#endif
        /*  Build the string "<num>,type".  */
        len        = snprintf( param_str, sizeof( param_str ), "\"%s\",%d", 
                               HF_DATA.phone_num.buf, num_type );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );

        snprintf( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ) - 1,
                  "BT AG TX: %s %s", rsps_from_ag[ resp ], param_str );
        bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = NULL;
        BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
        break;

      case BT_AG_RESP_CME_ERROR:
        len        = bt_ag_uint32toa( generic, param_str );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, param_str, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;

#ifdef BT_AG_HF_EXTENSION
      case BT_AG_RESP_BTRH:
        len = strlen( (char*) generic );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, (void*)generic, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;

      case BT_AG_RESP_PAP_CPBS:
      case BT_AG_RESP_PAP_CNUM:
        len = strlen( (char*) generic );
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, (void*)generic, len,
                                          DSM_DS_SMALL_ITEM_POOL ) == len );
        break;

      case BT_AG_RESP_PAP_CPBR:
      case BT_AG_RESP_PAP_CPBF:
      {
        bt_cmd_ag_send_pb_read_result_type* spr_cmd_ptr = 
          (bt_cmd_ag_send_pb_read_result_type*) generic;
        char tmp_str[ sizeof(bt_pap_record_type) ];

        std_strlprintf( tmp_str, sizeof(tmp_str),
                        "%d,%s,%d,", 
                        spr_cmd_ptr->record.index, 
                        spr_cmd_ptr->record.number,
                        spr_cmd_ptr->record.type );
        len = strlen( tmp_str );

        memcpy( (void*)&tmp_str[ len ], (void*)spr_cmd_ptr->record.text,
                bt_agcpe.rx_param_len[ 3 ] );
        tmp_str[ len+bt_agcpe.rx_param_len[ 3 ] ]   = '\0';
        tmp_str[ len+bt_agcpe.rx_param_len[ 3 ]+1 ] = '\0';

        len += (bt_agcpe.rx_param_len[ 3 ] + 2);
        keep_going = ( dsm_pushdown_tail( &dsm_ptr, (void*)tmp_str, len,
                                          DSM_DS_LARGE_ITEM_POOL ) == len );
        break;
      }
#endif /* BT_AG_HF_EXTENSION */

      default:
        BT_ERR( "BT AG: unexpected response %x", resp, 0, 0 );
        keep_going = FALSE;
        break;
    }
  }

  if ( keep_going )
  {
    /*  Append postfix.  */
    len        = strlen( BT_AG_RESP_POSTFIX );
    keep_going = ( dsm_pushdown_tail( &dsm_ptr, BT_AG_RESP_POSTFIX, len,
                                      DSM_DS_SMALL_ITEM_POOL ) == len );
  }

  if ( keep_going )
  {
    BT_MSG_SIG( "BT AG TX: response %x SID %x", resp,
                bt_ag.sio_stream_id, 0 );

    dsm_append( &bt_ag.tx_dsm_ptr, &dsm_ptr );
    bt_ag_send_data();

    /*  Set timer to go idle now.  */
    if ( bt_ag.state < BT_AGS_AUDIO_CONNECTED )
    {
      if ( (bt_ag.ring_count == 0) ||
           ((resp == BT_AG_RESP_RING) && (bt_ag.ring_count == 1)) )
      {
        if ( (AUDIO_DEV.device_type == BT_AD_HANDSFREE) &&
             (HF_DATA.connection_state < BT_AG_HCS_DONE) )
        {
          to_value = BT_AG_SLC_SETUP_TO;
        }
        else
        {
          to_value = bt_ag.idle_timeout;
        }
        if ( to_value > 0 )
        {
          BT_MSG_DEBUG( "BT AG: Send Resp Set TO %x", to_value, 0, 0 );
          rex_set_timer( &bt_ag_timer, to_value );
        }
      }
    }
  }
  else if ( dsm_ptr != NULL )
  {
    BT_ERR( "BT AG: Send Resp problem", 0, 0, 0 );
    dsm_free_packet( &dsm_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ring_audio_device

DESCRIPTION
  Sends RING indication to HS/HF.  If CLI notification is enabled, sends
  +CLIP response to HF.

===========================================================================*/
LOCAL void bt_ag_ring_audio_device
(
  void
)
{
  
  if ( (HF_DATA.connection_state == BT_AG_HCS_DONE) ||
       ((AUDIO_DEV.device_type == BT_AD_HEADSET) &&
        (bt_ag.state != BT_AGS_AUDIO_CONNECTED)) )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_RING, 0 );

    if ( --bt_ag.ring_count > 0 )
    {
      /*  Set timer to wait before sending out another RING.  */
      BT_MSG_DEBUG( "BT AG: set timer to send RING", 0, 0, 0 );
      rex_set_timer( &bt_ag_timer, (bt_ag.ring_period * 250) );
    }

    if ( HF_DATA.notify_cli && (HF_DATA.phone_num.len > 0) )
    {
      /*  Send subscriber number info to HF.  */
      bt_ag_cpe_send_response( BT_AG_RESP_CLIP, 0 );
    }
    else
    {
      BT_MSG_DEBUG( "BT AG: CLIP not sent, cli=%d pnl=%d", 
                    HF_DATA.notify_cli, HF_DATA.phone_num.len, 0 );
    }
  }

}

/*===========================================================================

FUNCTION
  bt_ag_save_ring_cmd_data

DESCRIPTION
  Saves RING_AUDIO_DEVICE command's data to be used later.

===========================================================================*/
LOCAL void bt_ag_save_ring_cmd_data( bt_cmd_msg_type* ag_cmd_ptr )
{
  bt_ag.app_id_ring_device = ag_cmd_ptr->cmd_hdr.bt_app_id;
  bt_ag.ring_count  = ag_cmd_ptr->cmd_msg.cmd_ag_ring_audio_device.max_rings;
  bt_ag.ring_period = ( bt_ag.ring_count > 1 ) ? 
    ag_cmd_ptr->cmd_msg.cmd_ag_ring_audio_device.ring_period : 1;
  if ( (bt_ag.ring_count > 1) && 
       (bt_ag.ring_period < BT_AG_DEFAULT_RING_PERIOD) )
  {
    BT_MSG_DEBUG( "BT AG: changing ring_period from %d to %d",
                  ag_cmd_ptr->cmd_msg.cmd_ag_ring_audio_device.ring_period, 
                  BT_AG_DEFAULT_RING_PERIOD, 0 );
    bt_ag.ring_period = BT_AG_DEFAULT_RING_PERIOD;
  }
}

/*===========================================================================

FUNCTION
  bt_ag_cmd_ring_audio_device

DESCRIPTION
  Creates a connection if none exists, then sends RING result code to
  the audio device to indicate an incoming call.  Ignores multiple ring
  commands.

===========================================================================*/
LOCAL void bt_ag_cmd_ring_audio_device
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  bt_cmd_ag_ring_audio_device_type  *cmd_ptr = 
    &ag_cmd_ptr->cmd_msg.cmd_ag_ring_audio_device;

  BT_MSG_API( "BT AG CMD RX: Ring AD MR %x RP %x AID %x",
              cmd_ptr->max_rings, cmd_ptr->ring_period,
              ag_cmd_ptr->cmd_hdr.bt_app_id );

  BT_MSG_DEBUG( "BT AG: Ring AD Times %x Period %x St %x",
                cmd_ptr->max_rings, cmd_ptr->ring_period, bt_ag.state );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  /*  No pending ring command?  */
  if ( (bt_ag.ring_count == 0) && 
       (cmd_ptr->max_rings > 0) &&
       (HF_DATA.call_active == FALSE) )
  {
    switch ( bt_ag.state )
    {
      case BT_AGS_DISABLED:
        BT_ERR( "BT AG: Can't ring dev St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
        break;

      case BT_AGS_CLOSED:
        if ( AUDIO_DEV.bd_addr_ptr == NULL )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
        }
        else
        {
          bt_ag.is_idle       = FALSE;
          bt_ag.is_sio_server = FALSE;

          if ( bt_ag_open_sio_client(
                 AUDIO_DEV.bd_addr_ptr,
                 AUDIO_DEV.service_uuid ) == SIO_NO_STREAM_ID )
          {
            ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_SIO_STREAM;
          }
          else
          {
            bt_ag_save_ring_cmd_data( ag_cmd_ptr );
          }
        }
        break;

      case BT_AGS_OPENING:
        if ( AUDIO_DEV.bd_addr_ptr == NULL )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
        }
        else
        {
          if ( bt_ag.is_sio_server )
          {
            ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
          }
          else
          {
            /*  Else, already trying to connect to remote device.  */
            bt_ag_save_ring_cmd_data( ag_cmd_ptr );
          }
        }
        break;

      case BT_AGS_OPEN:
        if ( AUDIO_DEV.bd_addr_ptr == NULL )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
        }
        else
        {
          bt_ag.is_idle       = FALSE;
          bt_ag.is_sio_server = FALSE;
          bt_ag_close_sio();
          bt_ag_save_ring_cmd_data( ag_cmd_ptr );
        }
        break;

      case BT_AGS_CONNECTED:
      case BT_AGS_AUDIO_CONNECTING:
      case BT_AGS_AUDIO_DISCONNECTING:
      case BT_AGS_AUDIO_CONNECTED:
        /*  RFCOMM link to remote device is up.  Send RING now.  */
        bt_ag_save_ring_cmd_data( ag_cmd_ptr );
        bt_ag_ring_audio_device();
        break;

      default:
        BT_ERR( "BT AG: Ring HS in bad state - %d",
                bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
        break;
    }
  }
  else if ( cmd_ptr->max_rings == 0 ) /* stop ringing? */
  {
    if ( bt_ag.ring_count > 0 )
    {
      bt_ag.ring_count = 0; /* make sure I won't send RING again */
      rex_clr_sigs( &bt_tcb, BT_AG_TIMER_SIG );
      rex_clr_timer( &bt_ag_timer );
    }
    if ( (bt_ag.idle_timeout > 0) && (bt_ag.state == BT_AGS_CONNECTED) )
    {
      rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );
      BT_MSG_DEBUG( "BT AG: set timer as ringing stops %d", 
                    bt_ag.idle_timeout, 0, 0 );
    }
  }
}


/*===========================================================================

FUNCTION
  bt_ag_add_audio

===========================================================================*/
LOCAL void bt_ag_add_audio( void )
{

  boolean attempt_esco = FALSE;

#ifdef FEATURE_BT_HFP_1_5
  if ( HF_DATA.profile_version >= HFP_VER_1_2 )
  {
    attempt_esco = TRUE;
  }
#endif
  BT_MSG_API( "BT AG CMD TX: RM Connect SCO", 0, 0, 0 );
  bt_cmd_rm_connect_sco( bt_ag.app_id_ag, &AUDIO_DEV.bd_addr, 
                         attempt_esco );
  bt_ag.state        = BT_AGS_AUDIO_CONNECTING;
  bt_ag.adding_audio = FALSE;
  rex_clr_timer( &bt_ag_timer );

}

/*===========================================================================

FUNCTION
  bt_ag_cmd_audio_connect

DESCRIPTION
  Establishes an SCO connection with the HS for audio transmission.
  This command can be called for both incoming and outgoing calls.

===========================================================================*/
LOCAL void bt_ag_cmd_audio_connect
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Aud Conn St %x AID %x",
              bt_ag.state, ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( bt_ag.adding_audio != FALSE )
  {
    return;  /* already is trying to bring up audio */
  }

  /*  Save app ID for use later to send connection result back.  */
  bt_ag.app_id_add_audio = ag_cmd_ptr->cmd_hdr.bt_app_id;

  switch ( bt_ag.state )
  {
    case BT_AGS_DISABLED:
      BT_ERR( "BT AG: Aud Conn Bad St %x AID %x",
              bt_ag.state, ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
      break;

    case BT_AGS_CLOSED:
      if ( AUDIO_DEV.bd_addr_ptr == NULL )
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
      }
      else
      {
        bt_ag.is_idle       = FALSE;
        bt_ag.is_sio_server = FALSE;

        if ( bt_ag_open_sio_client(
               AUDIO_DEV.bd_addr_ptr,
               AUDIO_DEV.service_uuid ) == SIO_NO_STREAM_ID )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_SIO_STREAM;
        }
        else
        {
          bt_ag.adding_audio = TRUE;
        }
      }
      break;

    case BT_AGS_OPENING:
      if ( AUDIO_DEV.bd_addr_ptr == NULL )
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
      }
      else
      {
        if ( bt_ag.is_sio_server )
        {
          ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
        }
        else
        {
          /*  Else, already trying to connect to remote device.  */
          bt_ag.adding_audio = TRUE;
        }
      }
      break;

    case BT_AGS_OPEN:
      if ( AUDIO_DEV.bd_addr_ptr == NULL )
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NO_BD_ADDR;
      }
      else
      {
        bt_ag.is_idle       = FALSE;
        bt_ag.is_sio_server = FALSE;
        bt_ag.adding_audio  = TRUE;
        bt_ag_close_sio();        
      }
      break;

    case BT_AGS_CONNECTED:
      if ( (AUDIO_DEV.device_type == BT_AD_HEADSET) ||
           (HF_DATA.connection_state == BT_AG_HCS_DONE) )
      {
        /*  RFCOMM link is up, I'll add audio now.  */
        bt_ag_add_audio();
      }
      else
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
      break;

    case BT_AGS_AUDIO_CONNECTED:
      BT_MSG_DEBUG( "BT AG: SCO already up", 0, 0, 0 );
      bt_ag.send_audio_conn_ev = TRUE;
      rex_set_timer( &bt_ag_timer, 5 ); // wait 5 ms before telling AVS
      break;

    case BT_AGS_AUDIO_CONNECTING:
    case BT_AGS_AUDIO_DISCONNECTING:
      /*  Currently waiting for SCO link to settle.  */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    default:
      BT_ERR( "BT AG: Aud Conn Bad St %x", bt_ag.state, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_audio_disconnect

DESCRIPTION
  Closes a connection to the headset.

===========================================================================*/
LOCAL void bt_ag_cmd_audio_disconnect
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Audio Disc St %x AID %x",
              bt_ag.state, ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag.state == BT_AGS_AUDIO_CONNECTED )
  {
    bt_ag_sco_cleanup();
  }
  else if ( (bt_ag.state == BT_AGS_AUDIO_CONNECTING) ||
            (bt_ag.state == BT_AGS_AUDIO_DISCONNECTING) )
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
  }
  else
  {
    bt_ag.event_reason = BT_EVR_GN_SUCCESS;
    bt_ag_send_event( BT_EV_AG_AUDIO_DISCONNECTED );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_ad_spkr_gain

DESCRIPTION
  Sets the speaker volume for the audio device.  For Headset devices,
  speaker gain can be set only if there's an audio link present.  For
  Hands-free devices, audio connection is not a necessary pre-condition.

===========================================================================*/
LOCAL void bt_ag_cmd_set_ad_spkr_gain
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  uint8 vgs = ag_cmd_ptr->cmd_msg.cmd_ag_spkr.ad_speaker_gain;

  BT_MSG_API( "BT AG CMD RX: Set Spkr Gain VGS %x AID %x",
              vgs, ag_cmd_ptr->cmd_hdr.bt_app_id,  0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( vgs > BT_AG_MAX_HS_VOLUME_LEVEL )
  {
    BT_ERR( "BT AG: Set Spkr Gain Bad VGS %x", vgs, 0, 0 );
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_VOLUME_OUT_OF_RANGE;
  }
  else
  {
    switch ( bt_ag.state )
    {
      case BT_AGS_DISABLED:
        BT_ERR( "BT AG: Set Spkr Gain Bad St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
        break;

      case BT_AGS_OPENING:
      case BT_AGS_OPEN:
        bt_ag.set_spkr_gain   = TRUE;  /*  Remember to set speaker gain.  */
        AUDIO_DEV.spkr_volume = vgs;
        break;

      case BT_AGS_CONNECTED:
      case BT_AGS_AUDIO_CONNECTING:
      case BT_AGS_AUDIO_CONNECTED:
      case BT_AGS_AUDIO_DISCONNECTING:
        if ( (AUDIO_DEV.device_type == BT_AD_HEADSET) ||
             (HF_DATA.connection_state == BT_AG_HCS_DONE) )
        {
          if ( AUDIO_DEV.spkr_volume != vgs )
          {
            AUDIO_DEV.spkr_volume = vgs;
            bt_ag_cpe_send_response( BT_AG_RESP_VGS, 0 );

            if (ag_cmd_ptr->cmd_hdr.bt_app_id != bt_ag.app_id_user)
            {
               /* echo the change */
               bt_ag_send_event( BT_EV_AG_SPKR_GAIN_REPORT );
            }
          }
        }
        break;

      default:
        BT_ERR( "BT AG: Set Spkr Gain Bad St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
        break;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_ad_mic_gain

DESCRIPTION
  Sets the microphone volume for the audio device.

===========================================================================*/
LOCAL void bt_ag_cmd_set_ad_mic_gain
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  uint8 vgm = ag_cmd_ptr->cmd_msg.cmd_ag_mic.ad_microphone_gain;

  BT_MSG_API( "BT AG CMD RX: Set Mic Gain VGM %x AID %x",
              vgm, ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );
  
  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( vgm > BT_AG_MAX_HS_VOLUME_LEVEL )
  {
    BT_ERR( "BT AG: Set Mic Gain Bad VGM %x", vgm, 0, 0 );
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_VOLUME_OUT_OF_RANGE;
  }
  else
  {
    switch ( bt_ag.state )
    {
      case BT_AGS_DISABLED:
        BT_ERR( "BT AG: Set Mic Gain Bad St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
        break;

      case BT_AGS_OPENING:
      case BT_AGS_OPEN:
        bt_ag.set_mic_gain   = TRUE;  /*  Remember to set mic gain.  */
        AUDIO_DEV.mic_volume = vgm;
        break;

      case BT_AGS_CONNECTED:
      case BT_AGS_AUDIO_CONNECTING:
      case BT_AGS_AUDIO_CONNECTED:
      case BT_AGS_AUDIO_DISCONNECTING:
        if ( (AUDIO_DEV.device_type == BT_AD_HEADSET) ||
             (HF_DATA.connection_state == BT_AG_HCS_DONE) )
        {
          if ( AUDIO_DEV.mic_volume != vgm )
          {
            AUDIO_DEV.mic_volume = vgm;
            bt_ag_cpe_send_response( BT_AG_RESP_VGM, 0 );
          }
        }
        break;

      default:
        BT_ERR( "BT AG: Set Mic Gain Bad St %x", bt_ag.state, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
        break;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_disconnect

DESCRIPTION
  Brings down the RFCOMM connection.

===========================================================================*/
LOCAL void bt_ag_cmd_disconnect
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Disconnect AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  switch ( bt_ag.state )
  {
    case BT_AGS_DISABLED:
      BT_MSG_DEBUG( "BT AG: AG disabled", 0, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
      break;

    case BT_AGS_CLOSED:
    case BT_AGS_OPEN:
      BT_MSG_DEBUG( "BT AG: AG not connected", 0, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_CONNECTED;
      break;

    case BT_AGS_OPENING:
      if ( bt_ag.is_sio_server )
      {
        BT_MSG_DEBUG( "BT AG: AG not connected", 0, 0, 0 );
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_CONNECTED;
      }
      else
      {
        BT_MSG_DEBUG( "BT AG: Canceling connect", 0, 0, 0 );
        bt_ag_cancel_connect();
      }
      break;

    case BT_AGS_AUDIO_CONNECTED:
      bt_ag_sco_cleanup();   /*  Close the SCO connection.  */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_AUDIO_DISCONNECTING:
      /*  Currently waiting for SCO disconnection  */
      /*  event from RM.  Nothing to do here.      */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_AUDIO_CONNECTING:
      /*  Currently waiting for audio connection  */
      /*  to come up.  Can't disconnect yet.      */
      /*  event from RM.  Nothing to do here.      */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_CLOSING:
      /*  Waiting to close.  */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_CONNECTED:
      sio_ioctl( bt_ag.sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
      bt_ag.state = BT_AGS_CLOSING;
      break;

    default:
      BT_ERR( "BT AG: AG Disc Bad St %x", bt_ag.state, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_disable

DESCRIPTION
  Disables/stops the AG. This involves RC disconnection.

===========================================================================*/
LOCAL void bt_ag_cmd_disable
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Disable St %x Dis %x AID %x",
              bt_ag.state, bt_ag.disabling,
              ag_cmd_ptr->cmd_hdr.bt_app_id );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch ( bt_ag.state )
  {
    case BT_AGS_DISABLED:
      BT_MSG_DEBUG( "BT AG: Already disabled", 0, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_NOT_ENABLED;
      break;

    case BT_AGS_AUDIO_CONNECTED:
      bt_ag_sco_cleanup();  /*  Close the SCO connection.  */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_AUDIO_DISCONNECTING:
      /*  Currently waiting for SCO disconnection  */
      /*  event from RM.  Nothing to do here.      */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_AUDIO_CONNECTING:
      /*  Currently waiting for audio connection  */
      /*  to come up.  Can't disable yet.         */
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_CONNECTED:
      bt_ag.disabling = TRUE;
      sio_ioctl( bt_ag.sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
      break;

    case BT_AGS_OPENING:
      if ( bt_ag.is_sio_server )
      {
        /*  Currently waiting for server stream  */
        /*  to be opened.  Can't disable yet.    */
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      }
      else
      {
        bt_ag.disabling = TRUE;
        bt_ag_close_sio();
      }
      break;

    case BT_AGS_OPEN:
      bt_ag.disabling = TRUE;
      bt_ag_close_sio();
      break;

    case BT_AGS_CLOSING:
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_RETRY_CMD_LATER;
      break;

    case BT_AGS_CLOSED:
      bt_ag_disable();
      break;

    default:
      BT_ERR( "BT AG: Disable Bad St %x", bt_ag.state, 0, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INTERNAL_ERROR;
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_vr_capable

DESCRIPTION
  Enables/disables VR capability in AG.

===========================================================================*/
LOCAL void bt_ag_cmd_set_vr_capable
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Set VR Capable %x AID %x",
              ag_cmd_ptr->cmd_msg.cmd_ag_set_vr.capable,
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( ag_cmd_ptr->cmd_msg.cmd_ag_set_vr.capable !=
       ((bt_ag.features_bitmap & BT_AG_F_VR_ON) == BT_AG_F_VR_ON) )
  {
    if ( ag_cmd_ptr->cmd_msg.cmd_ag_set_vr.capable != FALSE)
    {
#ifdef FEATURE_PHONE_VR
      bt_ag.features_bitmap |= BT_AG_F_VR_ON;
#endif
    }
    else
    {
      bt_ag.features_bitmap &= ~BT_AG_F_VR_ON;
    }

    if ( bt_ag.hf_svc_registered != FALSE )
    {
      /*  bt_cmd_sd_update_service_record() doesn't do enough.  */
      bt_cmd_sd_unregister_service(
        bt_ag.app_id_ag,
        BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY );
      bt_cmd_sd_register_serv_ext (
        bt_ag.app_id_ag,
        BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY,
        BT_AG_HF_PROFILE_VERSION_NUMBER,
        BT_RC_SCN_AG_HF,
        BT_L2_PSM_INVALID,
        FALSE,  /*  Do not page scan for this SD registration.  */
        BT_AG_SERVICE_NAME );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_update_vr_state

DESCRIPTION
  Updates VR state kept by AG.  Informs HF of change if SLC is up.

===========================================================================*/
LOCAL void bt_ag_cmd_update_vr_state
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  BT_MSG_API( "BT AG CMD RX: Update VR State %x AID %x",
              ag_cmd_ptr->cmd_msg.cmd_ag_update_vr_state.vr_enabled,
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( (AUDIO_DEV.device_type == BT_AD_HANDSFREE) &&
       (HF_DATA.vr_enabled != 
          ag_cmd_ptr->cmd_msg.cmd_ag_update_vr_state.vr_enabled) )
  {
    HF_DATA.vr_enabled = 
      ag_cmd_ptr->cmd_msg.cmd_ag_update_vr_state.vr_enabled;
    bt_ag_cpe_send_response( BT_AG_RESP_BVRA, 0 );
  }

}


/*===========================================================================

FUNCTIONS
  bt_ag_cpe_external_io_tx_wm_ne_cb

DESCRIPTION
  This is linked into the TX "Gone Non-Empty" call back function
  for the external I/O transmit watermark to facilitate the movement
  of transmit data.

===========================================================================*/
LOCAL void bt_ag_cpe_external_io_tx_wm_ne_cb
(
  void
)
{

  BT_MSG_DEBUG( "external_io_tx_wm_ne_cb", 0, 0, 0 );

  if ( (bt_agcpe.respond_pending != FALSE) &&
       (bt_ag.state > BT_AGS_OPEN) && (bt_ag.state < BT_AGS_CLOSING) )
  {
    bt_ag_cpe_dequeuing_external_io_tx_wm();

    bt_ag.flags |= BT_AG_SEND_DATA_B;
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: CPE ExtIO Ignored TXD C %x",
            bt_agcpe.tx_wm_ptr->current_cnt, 0, 0 );

    dsm_empty_queue( bt_agcpe.tx_wm_ptr );
  }

  /*  Call the application's callback as necessary.  */
  if ( bt_agcpe.orig_tx_wm_ne_fptr != NULL )
  {
    (bt_agcpe.orig_tx_wm_ne_fptr)();
  }

}

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
/*===========================================================================

FUNCTIONS
  bt_ag_cpe_external_io_rx_wm_ne_cb

DESCRIPTION
  This is linked into the RX "Gone Non-Empty" call back function
  for the external I/O receive watermark.  This is done to avoid problem of
  entering critical section (in BREW) while tasks are locked.

===========================================================================*/
LOCAL void bt_ag_cpe_external_io_rx_wm_ne_cb
(
  void
)
{

  /*  Call the application's callback as necessary.  */
  if ( bt_agcpe.orig_rx_wm_ne_fptr != NULL )
  {
    bt_agcpe.call_orig_rx_wm_ne_func = TRUE;
    (void) rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }

}
#endif

/*===========================================================================

FUNCTION
  bt_ag_cmd_external_io_config

DESCRIPTION
  This function configures the external AG command I/O.

===========================================================================*/
LOCAL void bt_ag_cmd_external_io_config
(
  bt_cmd_msg_type*  extio_ptr
)
{

  BT_MSG_API( "BT AG CMD RX: ExtIO Cfg En %x Ctrl %x AID %x",
              extio_ptr->cmd_msg.cmd_ag_extio.enable_external_io,
              extio_ptr->cmd_msg.cmd_ag_extio.int_cmd_off_mask,
              extio_ptr->cmd_hdr.bt_app_id );

  
  if ( (extio_ptr->cmd_msg.cmd_ag_extio.enable_external_io == FALSE) ||
       ((extio_ptr->cmd_msg.cmd_ag_extio.rx_wm_ptr != NULL) &&
        (extio_ptr->cmd_msg.cmd_ag_extio.tx_wm_ptr != NULL)) )
  {
    extio_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
    
    /*  Clear any previous settings.  */
    bt_ag_disable_external_cmd_io();

    if ( extio_ptr->cmd_msg.cmd_ag_extio.enable_external_io != FALSE )
    {
      bt_agcpe.rx_wm_ptr = extio_ptr->cmd_msg.cmd_ag_extio.rx_wm_ptr;
      bt_agcpe.tx_wm_ptr = extio_ptr->cmd_msg.cmd_ag_extio.tx_wm_ptr;
      
      INTLOCK();
      bt_agcpe.orig_tx_wm_ne_fptr =
        extio_ptr->cmd_msg.cmd_ag_extio.tx_wm_ptr->non_empty_func_ptr;
      bt_agcpe.tx_wm_ptr->non_empty_func_ptr =
        bt_ag_cpe_external_io_tx_wm_ne_cb;
#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
      bt_agcpe.orig_rx_wm_ne_fptr =
        extio_ptr->cmd_msg.cmd_ag_extio.rx_wm_ptr->non_empty_func_ptr;
      bt_agcpe.rx_wm_ptr->non_empty_func_ptr =
        bt_ag_cpe_external_io_rx_wm_ne_cb;
#endif
      INTFREE();
      
      bt_agcpe.int_cmd_off_mask =
        extio_ptr->cmd_msg.cmd_ag_extio.int_cmd_off_mask;
      
      bt_agcpe.enable_external_io = TRUE;
    }
  }
  else
  {
    extio_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_inband_ring

DESCRIPTION
  Enables/disables inband ringing in AG.  Informs HF of change if
  SLC is up.

===========================================================================*/
LOCAL void bt_ag_cmd_set_inband_ring
( 
  bt_cmd_msg_type*  ag_cmd_ptr 
)
{
  
  boolean already_enabled = ISBITSET( bt_ag.flags, INBAND_B );

  BT_MSG_API( "BT AG CMD RX: Set Inband Ring AID %x new %x cur %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id,
              ag_cmd_ptr->cmd_msg.cmd_ag_set_inband_ring.inband_enabled,
              already_enabled );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( ag_cmd_ptr->cmd_msg.cmd_ag_set_inband_ring.inband_enabled !=
         already_enabled )
  {
    if ( already_enabled )
    {
      CLRBIT( bt_ag.flags, INBAND_B );
    }
    else
    {
      SETBIT( bt_ag.flags, INBAND_B );
    }

    if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_BSIR,
                               (uint32)(!already_enabled) );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_battchg_ind

DESCRIPTION
  Sets the Battery Charge Indication value and if different than the last
  value set then the information is sent to the car-kit if currently
  connected.

===========================================================================*/
LOCAL void bt_ag_cmd_set_battchg_ind
( 
  bt_cmd_msg_type*  ag_sbc_ptr
)
{
  
  uint8  new_battchg = (uint8)(ag_sbc_ptr->cmd_msg.cmd_ag_setbc.battchg);

  BT_MSG_API( "BT AG CMD RX: SetBC %x Cur %x AID %x",
              new_battchg, HF_DATA.battchg,
              ag_sbc_ptr->cmd_hdr.bt_app_id );

  ag_sbc_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( new_battchg <= (uint8)(BT_AG_BATTCHG_LEVEL_5) )
  {
    if ( HF_DATA.battchg != new_battchg )
    {
      HF_DATA.battchg = new_battchg;
#ifdef FEATURE_BT_HFP_1_5
      /* Indicator change shall be sent only if headset supports HFP 1.5 */
      if ( HF_DATA.profile_version >= HFP_VER_1_2 )
      {
        if ( (HF_DATA.connection_state == BT_AG_HCS_DONE) &&
             (HF_DATA.ier_mode == BT_AIER_FWD) )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CIEV,
                                   BT_AG_INDICATOR_BATTCHG );
        }
      }
#endif /* FEATURE_BT_HFP_1_5 */
    }
  }
  else
  {
    ag_sbc_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_get_battchg_ind

DESCRIPTION
  Gets the current AG Battery Charge Indication value.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_get_battchg_ind
( 
  bt_app_id_type             bt_app_id,
  bt_ag_battchg_level_type*  battchg_ptr
)
{
  
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  
  if ( battchg_ptr != NULL )
  {
    /*  No need for Task lock as it is one byte of data.  */
    *battchg_ptr = (bt_ag_battchg_level_type)(HF_DATA.battchg);
  }
  else
  {
    cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

  BT_MSG_API( "BT AG CMD RX: GetBC %x AID %x",
              *battchg_ptr, bt_app_id, 0 );

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_set_rssi

DESCRIPTION
  Sets the Receive Signal Strength Indication value and if different than
  the last value set then the information is sent to the car-kit if
  currently connected.

===========================================================================*/
LOCAL void bt_ag_cmd_set_rssi
( 
  bt_cmd_msg_type*  ag_srs_ptr
)
{
  
  uint8 new_rssi = (uint8)(ag_srs_ptr->cmd_msg.cmd_ag_setrs.rssi);

  BT_MSG_API( "BT AG CMD RX: SetRSSI %x Cur %x AID %x",
              new_rssi, HF_DATA.rssi,
              ag_srs_ptr->cmd_hdr.bt_app_id );

  ag_srs_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( new_rssi <= (uint8)(BT_AG_RSSI_LEVEL_5) )
  {
    if ( HF_DATA.rssi != new_rssi )
    {
      HF_DATA.rssi = new_rssi;

#ifdef FEATURE_BT_HFP_1_5
      /* Indicator change shall be sent only if headset supports HFP 1.5 */
      if ( HF_DATA.profile_version >= HFP_VER_1_2 )
      {
        if ( (HF_DATA.connection_state == BT_AG_HCS_DONE) &&
             (HF_DATA.ier_mode == BT_AIER_FWD) )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CIEV,
                                   BT_AG_INDICATOR_SIGNAL );
        }
      }
#endif /* FEATURE_BT_HFP_1_5 */
    }
  }
  else
  {
    ag_srs_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

}


/*===========================================================================

FUNCTION
  bt_cmd_ag_get_rssi

DESCRIPTION
  Gets the current AG Receive Signal Strength Indication value.

===========================================================================*/
bt_cmd_status_type bt_cmd_ag_get_rssi
( 
  bt_app_id_type          bt_app_id,
  bt_ag_rssi_level_type*  rssi_ptr
)
{
  
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;
  
  if ( rssi_ptr != NULL )
  {
    /*  No need for Task lock as it is one byte of data.  */
    *rssi_ptr = (bt_ag_rssi_level_type)(HF_DATA.rssi);
  }
  else
  {
    cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }

  BT_MSG_API( "BT AG CMD RX: GetRSSI %x AID %x",
              *rssi_ptr, bt_app_id, 0 );

  return ( cmd_status );

}


#ifdef BT_AG_HF_EXTENSION
/*===========================================================================

FUNCTION
  bt_ag_cmd_send_supported_rh

DESCRIPTION
  Sends a list of supported respond and hold states.

===========================================================================*/
LOCAL void bt_ag_cmd_send_supported_rh
( 
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  char   tmp_str[ BT_AG_RESP_MAX_LEN ];
  int    len = 0;
  uint8  i;
  bt_cmd_ag_send_supported_rh_type*  ssr_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_supported_rh;

  BT_MSG_API( "BT AG CMD RX: Send Supported R&H AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  BT_STRCPY( tmp_str, "(", sizeof(tmp_str) );
  len = strlen( tmp_str );

  for ( i=0; i<BT_AG_RH_MAX; i++ )
  {
    HF_DATA.supported_rh[ i ] = ssr_cmd_ptr->supported_rh[ i ];
    if ( HF_DATA.supported_rh[ i ] == TRUE )
    {
      std_strlprintf( &tmp_str[ len ], sizeof(tmp_str) - len, "%d,", i );
    }
  }

  if ( tmp_str[ len-1 ] == ',' )  /*  Anything on list?  */
  {
    tmp_str[ len-1 ] = ')';
    bt_ag_cpe_send_response( BT_AG_RESP_BTRH, (uint32)tmp_str );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_current_rh

DESCRIPTION
  Sends the current respond and hold state, if any.

===========================================================================*/
LOCAL void bt_ag_cmd_send_current_rh
( 
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  char  tmp_str[3];
  bt_cmd_ag_send_current_rh_type*  scr_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_current_rh;

  BT_MSG_API( "BT AG CMD RX: Send Supported R&H AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( scr_cmd_ptr->is_in_rh )
  {
    if ( (scr_cmd_ptr->current_rh < BT_AG_RH_MAX) && 
         (HF_DATA.supported_rh[ scr_cmd_ptr->current_rh ] == TRUE) )
    {
      std_strlprintf( tmp_str, sizeof( trm_str),
                      "%d", scr_cmd_ptr->current_rh );
      bt_ag_cpe_send_response( BT_AG_RESP_BTRH, (uint32)tmp_str );
    }
    else
    {
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
    }
  }
  if ( ag_cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_pap2cme_err

DESCRIPTION
  Converts PAP result code to CME error code.

===========================================================================*/
LOCAL bt_ag_cme_err_type bt_ag_pap2cme_err
(
  bt_pap_return_type  pap_err
)
{
  
  bt_ag_cme_err_type cme_err;
  
  switch ( pap_err )
  {
    case BT_PAP_ERROR_NO_SIM:
      cme_err = CME_ERR_SIM_NOT_INSERTED;
      break;

    case BT_PAP_ERROR_INDEX_INVALID:
      cme_err = CME_ERR_INVALID_INDEX;
      break;

    case BT_PAP_ERROR_NOT_READY:
      cme_err = CME_ERR_OP_NOT_ALLOWED;
      break;

    case BT_PAP_ERROR_NO_MEMORY:
      cme_err = CME_ERR_MEMORY_FULL;
      break;

    default:
      cme_err = CME_ERR_OP_NOT_ALLOWED;
      break;
  }

  return ( cme_err );

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_list

DESCRIPTION
  Sends a list of supported phonebooks to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_list
( 
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  uint8    i, j;
  char     tmp_str[ BT_AG_RESP_MAX_LEN ];
  uint16   len = 0;
  boolean  found = FALSE;
  bt_cmd_ag_send_pb_list_type*  spl_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_list;

  BT_MSG_API( "BT AG CMD RX: Send Phonebook List AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( spl_cmd_ptr->num_supported_phonebooks > 0 )
  {
    BT_STRCPY( tmp_str, "(", sizeof(tmp_str) );
    len = 1;
  }

  for ( i=0; i<spl_cmd_ptr->num_supported_phonebooks; i++)
  {
    for ( j=0; j<ARR_SIZE(bt_ag_ph_name_table) && !found; j++)
    {
      if ( bt_ag_ph_name_table[j].ph_type == 
           spl_cmd_ptr->supported_phonebooks[i] )
      {
        bt_ag_ph_name_table[j].supported = TRUE;
        len = std_strlprintf( &tmp_str[len], sizeof(tmp_str) - len,
                              "%s,", bt_ag_ph_name_table[j].pb_name );
        found = TRUE;
      }
    }
    if ( !found )
    {
      BT_ERR( "BT AG: unexpected ph ID %d", 
              spl_cmd_ptr->supported_phonebooks[i], 0, 0 );
    }
    else
    {
      found = FALSE;  /* reset for next pb ID */
    }
  }

  if ( (len > 0) && (tmp_str[ len-1 ] == ',') )  /* anything on list? */
  {
    tmp_str[ len-1 ] = ')';
    bt_ag_cpe_send_response( BT_AG_RESP_PAP_CPBS, (uint32)tmp_str );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_select_result

DESCRIPTION
  Sends result of phonebook selection to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_select_result
( 
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  bt_cmd_ag_send_pb_select_result_type*  sps_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_select_result;

  BT_MSG_API( "BT AG CMD RX: Send PB Sel Res R %x ID %x AID %x ",
              sps_cmd_ptr->result, sps_cmd_ptr->pb_id,
              ag_cmd_ptr->cmd_hdr.bt_app_id );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type != 
         sps_cmd_ptr->pb_id )
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }
  else if ( sps_cmd_ptr->result == BT_PAP_SUCCESS )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR, 
                             bt_ag_pap2cme_err( sps_cmd_ptr->result ) );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_info

DESCRIPTION
  Sends info on the selected phonebook to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_info
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{

  bt_cmd_ag_send_pb_info_type*  spi_cmd_ptr = 
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_info;

  BT_MSG_API( "BT AG CMD RX: Send Phonebook Info AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type != 
         spi_cmd_ptr->pb_id )
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }
  else
  {
    char tmp_str[ BT_AG_RESP_MAX_LEN ];

    HF_DATA.pb_total_recs = spi_cmd_ptr->total_recs;  /*  Save it.  */
    std_strlprintf( tmp_str, sizeof(tmp_str), "%s,%d,%d", 
                    bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].pb_name,
                    spi_cmd_ptr->used_recs,
                    spi_cmd_ptr->total_recs );
    bt_ag_cpe_send_response( BT_AG_RESP_PAP_CPBS, (uint32)(tmp_str) );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_entries_info

DESCRIPTION
  Sends entries info on the selected phonebook to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_entries_info
( 
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  char  tmp_str[ BT_AG_RESP_MAX_LEN ];
  bt_cmd_ag_send_pb_entries_info_type *spei_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_entries_info;

  BT_MSG_API( "BT AG CMD RX: Send PB Entries Info R %x AID %x",
              spei_cmd_ptr->result, ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );
  
  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  if ( spei_cmd_ptr->result == BT_PAP_SUCCESS )
  {
    std_strlprintf( tmp_str, sizeof(tmp_str),
                    "(%d,%d),%d,%d", 
                    spei_cmd_ptr->start_idx,
                    spei_cmd_ptr->end_idx,
                    spei_cmd_ptr->max_num_len,
                    spei_cmd_ptr->max_text_len );
    bt_ag_cpe_send_response( BT_AG_RESP_PAP_CPBR, (uint32)(tmp_str) );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR, 
                             bt_ag_pap2cme_err( spei_cmd_ptr->result ) );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_read_result

DESCRIPTION
  Sends result of the phonebook entry read on the selected phonebook to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_read_result
( 
  bt_cmd_msg_type* ag_cmd_ptr
)
{
  
  bt_cmd_ag_send_pb_read_result_type *spr_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_read_result;

  BT_MSG_API( "BT AG CMD RX: Send PB Read Result AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type != 
         spr_cmd_ptr->pb_id )
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }
  else
  {
    if ( spr_cmd_ptr->event == BT_EV_AG_PAP_CPBR )
    {
      if ( HF_DATA.entries_2_read == 0 )
      {
        ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_CMD_STATE;
        BT_ERR( "BT AG: Bad BT_CMD_AG_SEND_PB_READ_RESULT", 0, 0, 0 );
      }
      else
      {
        HF_DATA.entries_2_read--;  /*  Another entry read.  */

        if ( spr_cmd_ptr->result == BT_PAP_SUCCESS )
        {
          /*  This entry is not empty, send result.  */
          bt_ag_cpe_send_response( BT_AG_RESP_PAP_CPBR,
                                   (uint32)(spr_cmd_ptr) );
        }
        else if ( spr_cmd_ptr->result == BT_PAP_ERROR_NOT_READY )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                                   CME_ERR_OP_NOT_ALLOWED );
        }

        if ( HF_DATA.entries_2_read == 0 )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
        }
      }
    }
    else /*  Must be BT_EV_AG_PAP_CPBF  */
    {
      if ( spr_cmd_ptr->result == BT_PAP_SUCCESS )
      {
        /*  Matching entry found, send result.  */
        bt_ag_cpe_send_response( BT_AG_RESP_PAP_CPBF,
                                 (uint32)(spr_cmd_ptr) );
      }
      else if ( spr_cmd_ptr->result == BT_PAP_ERROR_NOT_READY )
      {
        bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                                 CME_ERR_OP_NOT_ALLOWED );
      }
      else /*  No (more) matching entry found. */
      {
        bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_pb_write_result

DESCRIPTION
  Sends result of the phonebook edit on the selected phonebook to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_pb_write_result
( 
  bt_cmd_msg_type* ag_cmd_ptr
)
{
  
  bt_cmd_ag_send_pb_write_result_type *spw_cmd_ptr =
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_pb_write_result;

  BT_MSG_API( "BT AG CMD RX: Send PB Write Result R %x ID %x AID %x",
              spw_cmd_ptr->result, spw_cmd_ptr->pb_id,
              ag_cmd_ptr->cmd_hdr.bt_app_id );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
  
  if ( bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type != 
         spw_cmd_ptr->pb_id )
  {
    ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_AG_INVALID_PARAMETERS;
  }
  else if ( spw_cmd_ptr->result == BT_PAP_SUCCESS )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                             bt_ag_pap2cme_err( spw_cmd_ptr->result ) );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cmd_send_subscriber_number_info

DESCRIPTION
  Sends result of the phonebook entry read on the selected phonebook
  to HF.

===========================================================================*/
LOCAL void bt_ag_cmd_send_subscriber_number_info
( 
  bt_cmd_msg_type* ag_cmd_ptr
)
{
  
  uint8  i = 0;
  char   tmp_str[ sizeof(bt_pap_subsc_num_info_type) ];
  bt_cmd_ag_send_subsc_num_info_type*  sni_cmd_ptr = 
    &ag_cmd_ptr->cmd_msg.cmd_ag_send_subsc_num_info;

  BT_MSG_API( "BT AG CMD RX: Send Subsc Num Info AID %x",
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

  ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  while ( (i<3) && (sni_cmd_ptr->sni[i].number[0] != 0) )
  {
    std_strlprintf( tmp_str, sizeof(tmp_str), ",%s,%d,,%d",
                    sni_cmd_ptr->sni[i].number,
                    sni_cmd_ptr->sni[i].type,
                    sni_cmd_ptr->sni[i].service );
    bt_ag_cpe_send_response( BT_AG_RESP_PAP_CNUM, (uint32)(tmp_str) );
    i++;
  }
  bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );

}

#endif /* BT_AG_HF_EXTENSION */


/*===========================================================================

FUNCTION
  bt_ag_perform_pending_cmds

DESCRIPTION
  Performs commands that may be pending.

===========================================================================*/
LOCAL void bt_ag_perform_pending_cmds
(
  void
)
{
  
  /*  Need to update inband setting? */
  if ( (HF_DATA.connection_state == BT_AG_HCS_DONE) && 
       (ISBITSET( bt_ag.flags, INBAND_B ) != 
        ((bt_ag.features_bitmap & BT_AG_F_IN_BAND_RING_TONE) == 
         BT_AG_F_IN_BAND_RING_TONE)) )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_BSIR, FALSE );
  }

  /*  Waiting to set mic gain?  */
  if ( bt_ag.set_mic_gain != FALSE )
  {
    bt_ag.set_mic_gain = FALSE;
    bt_ag_cpe_send_response( BT_AG_RESP_VGM, 0 );
  }

  /*  Waiting to set speaker gain?  */
  if ( bt_ag.set_spkr_gain != FALSE )
  {
    bt_ag.set_spkr_gain = FALSE;
    bt_ag_cpe_send_response( BT_AG_RESP_VGS, 0 );
  }

  /*  Waiting to ring audio device?  */
  if ( (bt_ag.ring_count > 0) &&
       ((AUDIO_DEV.device_type == BT_AD_HEADSET) ||
        (HF_DATA.call_active == FALSE)) )
  {
    bt_ag_ring_audio_device();
  }
  else
  {
    bt_ag.ring_count = 0;
  }

  /*  Waiting to bring up audio link?  */
  if ( bt_ag.adding_audio != FALSE )
  {
    bt_ag_add_audio();
  }

}


/*===========================================================================

FUNCTION
  bt_ag_slc_done

DESCRIPTION
  Final steps of SLC establishment.

===========================================================================*/
LOCAL void bt_ag_slc_done( void )
{
  BT_MSG_HIGH( "BT AG: SLC established!", 0, 0, 0 );
  HF_DATA.connection_state = BT_AG_HCS_DONE;
  bt_ag_send_event( BT_EV_AG_SLC_CHANGED );
  if ( bt_ag.idle_timeout > 0 )
  {
    rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );  /* Reset timer */
  }
  else
  {
    rex_clr_timer( &bt_ag_timer );
  }
  bt_ag_perform_pending_cmds();
}


/*===========================================================================

FUNCTION
  bt_ag_establish_service_level_connection

DESCRIPTION
  Creates service level connection required by the HFP.

===========================================================================*/
LOCAL void bt_ag_establish_service_level_connection
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: Establish SLC St %x HS St %x",
                bt_ag.state, HF_DATA.connection_state, 0 );

  switch ( HF_DATA.connection_state )
  {
    case BT_AG_HCS_BRSF:
      HF_DATA.connection_state = BT_AG_HCS_CIND_TEST;
      bt_ag_cpe_send_response( BT_AG_RESP_BRSF, 0 );
      bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
      break;

    case BT_AG_HCS_CIND_TEST:
      HF_DATA.connection_state = BT_AG_HCS_CIND_READ;
      bt_ag_cpe_send_response( BT_AG_RESP_CIND, BT_AG_RESP_LIST );
      bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
      break;

    case BT_AG_HCS_CIND_READ:
      HF_DATA.connection_state = BT_AG_HCS_CMER;
      bt_ag_cpe_send_response( BT_AG_RESP_CIND, BT_AG_RESP_INFO );
      bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
      break;

    case BT_AG_HCS_CMER:
      if ( MULTIPARTY_SUPPORTED() )
      {
        HF_DATA.connection_state = BT_AG_HCS_CHLD_TEST;
      }
      else
      {
        bt_ag_slc_done();
      }
      break;

    case BT_AG_HCS_CHLD_TEST:
      bt_ag_cpe_send_response( BT_AG_RESP_CHLD, 0 );
      bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
      bt_ag_slc_done();
      break;

    case BT_AG_HCS_DONE:
      bt_ag_perform_pending_cmds();
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cm_call_cmd_cb

DESCRIPTION
  Callback function for all cm_mm_call_cmd_*().

===========================================================================*/
LOCAL void bt_ag_cm_call_cmd_cb
(
  void*                   data_block_ptr,
  cm_call_cmd_e_type      cmd,
  cm_call_cmd_err_e_type  cmd_err
)
{

  BT_MSG_DEBUG( "BT AG: CM Call Cmd %x Err %x", cmd, cmd_err, 0 );

  if ( cmd_err == CM_CALL_CMD_ERR_NOERR )
  {
    switch ( cmd )
    {
      case CM_CALL_CMD_ORIG:
      case CM_CALL_CMD_ANSWER:
      case CM_CALL_CMD_END:
      case CM_CALL_CMD_SUPS:
#ifndef FEATURE_BT_WITH_OLD_CM
      case CM_CALL_CMD_INFO_LIST_GET:
#else
      case CM_CALL_CMD_INFO_GET:
#endif
        break;

      default:
        BT_ERR( "BT AG: CM Call Cmd %x", cmd, 0, 0 );
        break;
    }
  }
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */

}


/*===========================================================================
FUNCTION
  bt_ag_end_current_call

DESCRIPTION
  Ends the current (active or alerting) call.

===========================================================================*/
LOCAL void bt_ag_end_current_call( void )
{
  cm_end_params_s_type end_params;
  end_params.call_id = HF_DATA.call_id;
  end_params.info_type = CM_CALL_MODE_INFO_NONE;
  
  if ( BT_AG_IN_CDMA )
  {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    end_params.end_params.cdma_end.end_reason_included = TRUE;
    end_params.end_params.cdma_end.end_reason = CM_CALL_END_REL_NORMAL;
    end_params.info_type = CM_CALL_MODE_INFO_CDMA;
#endif
  }
  else if ( BT_AG_IN_GW )
  {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif
  }
  else
  {
    MSG_ERROR( "BT AG: current call not in any network!", 0, 0, 0 );
  }

  if ( end_params.info_type != CM_CALL_MODE_INFO_NONE )
  {
    BT_MSG_DEBUG( "BT AG: ending current call, ID=%d", HF_DATA.call_id, 0, 0 );
    cm_mm_call_cmd_end( bt_ag_cm_call_cmd_cb, NULL, 
                        HF_DATA.cm_client_id, 1, &end_params );
  }
}


/*===========================================================================

FUNCTION
  bt_ag_cpe_handle_unrecognized_cmd

DESCRIPTION
  Handles the unrecognized command by either forwarding it to the app that
  registered for external IO, or sending back ERROR if external IO is disabled.

===========================================================================*/
LOCAL boolean bt_ag_cpe_handle_unrecognized_cmd( void )
{

  boolean forwarded = FALSE;
  uint8 i;
  uint32 dsm_pkt_len;
  dsm_item_type* dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, 
                                                bt_agcpe.rx_count );
  if ( (bt_agcpe.enable_external_io == FALSE) ||
       (bt_agcpe.respond_pending != FALSE) )
  {
    /*  Unrecognized command - send error response.  */
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );

    BT_ERR( "BT AG: CPE dropping cmd L %x", bt_agcpe.rx_count, 0, 0 );

    for ( i = 0; i < bt_agcpe.rx_count; i++ )
    {
      BT_ERR( "BT AG: CPE Ignore cmd %X (%c)",
              bt_agcpe.rx_buf[ i ],
              bt_agcpe.rx_buf[ i ], 0 );
    }
  }
  else if ( dsm_pushdown( &dsm_ptr, (void*)(bt_agcpe.rx_buf), bt_agcpe.rx_count, 
                          DSM_DS_SMALL_ITEM_POOL ) != bt_agcpe.rx_count )
  {
    dsm_pkt_len = dsm_length_packet( dsm_ptr );
    BT_ERR( "BT AG: CPE Bad pushdown %x %x",
            bt_agcpe.rx_count, dsm_pkt_len, 0 );

    dsm_free_packet( &dsm_ptr );
  }
  else
  {
    dsm_enqueue( bt_agcpe.rx_wm_ptr, &dsm_ptr );
    bt_agcpe.respond_pending = TRUE;
    forwarded = TRUE;
  }
  return forwarded;

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_button_pressed

DESCRIPTION
  Called to process the AT+CKPD=<val> command.
  Only parameter expected is "200".

===========================================================================*/
LOCAL void bt_ag_cpe_process_button_pressed
(
  void
)
{
  
  int32 val = bt_ag_param2int32( CPE_PARAM_0 );
  
  BT_MSG_SIG( "BT AG RX: AT+CKPD=%c%c%c", 
              bt_agcpe.rx_buf[ CPE_PARAM_0 ],
              bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ],
              bt_agcpe.rx_buf[ CPE_PARAM_0 + 2 ] );

  if ( (AUDIO_DEV.device_type == BT_AD_HEADSET) && (val == 200) )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
    bt_ag_send_event( BT_EV_AG_BUTTON_PRESSED );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
  }

}

/*===========================================================================

FUNCTION
  bt_ag_cpe_process_volume_gain

DESCRIPTION
  Called to process the AT+VGM/AT+VGS=<gain> commands.

===========================================================================*/
LOCAL void bt_ag_cpe_process_volume_gain
(
  void
)
{
  int32                gain = bt_ag_param2int32( CPE_PARAM_0 );
  bt_ag_response_type  rsp  = BT_AG_RESP_ERROR;

  /*  Good parameter?  */
  if ( (gain >= BT_AG_MIN_HS_VOLUME_LEVEL) && 
       (gain <= BT_AG_MAX_HS_VOLUME_LEVEL) )
  {
    rsp = BT_AG_RESP_OK;

    /*  Only save the volume gain if not waiting  */
    /*  to set audio device's volume.             */
    if ( bt_agcpe.cmd_idx == BT_AG_CMD_VGM )
    {
      BT_MSG_SIG( "BT AG RX: AG AT+VGM=%d", gain, 0, 0 );
      if ( bt_ag.set_mic_gain == FALSE )
      {
        AUDIO_DEV.mic_volume = (uint8) gain;
        bt_ag_send_event( BT_EV_AG_MIC_GAIN_REPORT );
      }
    }
    else if ( bt_agcpe.cmd_idx == BT_AG_CMD_VGS )
    {
      BT_MSG_SIG( "BT AG RX: AT+VGS=%d", gain, 0, 0 );
      if (AUDIO_DEV.spkr_volume != (uint8) gain)
      {
        AUDIO_DEV.spkr_volume = (uint8) gain;
        bt_ag_send_volume_to_snd_task( AUDIO_DEV.spkr_volume );
        bt_ag_send_event( BT_EV_AG_SPKR_GAIN_REPORT );

        /* make sure to drop the pending vol change from app */
        bt_ag.set_spkr_gain = FALSE;
      }
    }
    else
    {
      BT_ERR( "BT AG ERR: CPE Bad cmd_idx %x", bt_agcpe.cmd_idx, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT AG ERR: Invalid AT+VGS/VGM Gain %d cmd_idx %x",
            gain, bt_agcpe.cmd_idx, 0 );
  }

  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_call_answer

DESCRIPTION
  Called to process the ATA command.

===========================================================================*/
LOCAL void bt_ag_cpe_process_call_answer
(
  void
)
{
  
  BT_MSG_SIG( "BT AG RX: ATA", 0, 0, 0 );
  if ( HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN )
  {
    bt_ag_send_event( BT_EV_AG_DEV_PICKUP );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_dial

DESCRIPTION
  Called to process the ATDdd..dd; and ATD>nnn; commands.

===========================================================================*/
LOCAL void bt_ag_cpe_process_dial
(
  void
)
{

  char*                num_str = &bt_agcpe.rx_buf[ CPE_PARAM_0 ];
  bt_ag_response_type  rsp     = BT_AG_RESP_OK;
  boolean send_rsp             = TRUE;

  BT_STRFORMAT( bt_ag_qxdm_msg, sizeof(bt_ag_qxdm_msg),
                "BT AG RX: ATD%s", num_str );
  bt_ag_qxdm_msg[ sizeof(bt_ag_qxdm_msg) - 1 ] = '\0';
  BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );

  /* if ';' at the end of ATD command has been received */
  /* check if ">= 2" to avoid critical lint error on boundary check */
  if ( (bt_agcpe.rx_count >= 2) &&
       (bt_agcpe.rx_buf[ bt_agcpe.rx_count - 2 ] == BT_AG_CMD_DELIMITER) )
  {
    bt_agcpe.rx_count--;
    /* replace ';' with '\0' */
    bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

    if ( num_str[0] == '>' )
    {
#ifdef FEATURE_BT_HFP_1_5
      bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = BT_AG_CMD_TERM_CHAR;
      bt_ag_cpe_handle_unrecognized_cmd(); /* let app deal with it */
      send_rsp = FALSE;
#else
      int32 mem_entry = bt_ag_param2int32( CPE_PARAM_0 + 1 );

      if ( (CPE_PARAM_LEN_0 > 1) && (mem_entry >= 0) )
      {
        bt_ag_send_event( BT_EV_AG_DEV_MEM_DIAL );
      }
      else
      {
        rsp = BT_AG_RESP_ERROR;
      }
#endif
    }
    else
    {
      uint8  num_len = (uint8) strlen( num_str );

      if ( (MULTIPARTY_SUPPORTED() || (HF_DATA.call_active == FALSE)) &&
           (num_len > 0) &&
           (num_len < CM_MAX_NUMBER_CHARS) )
      {
        bt_ag_send_event( BT_EV_AG_DEV_DIAL );
      }
      else  /*  Multiparty call not supported; can't dial during a call.  */
      {
        rsp = BT_AG_RESP_ERROR;
      }
    }
  }
  else
  {
    BT_ERR( "BT AG ERR: ATD command has no \";\" char", 0, 0, 0 );
    rsp = BT_AG_RESP_ERROR;
  }

  if ( send_rsp != FALSE )
  {
    bt_ag_cpe_send_response( rsp, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_redial

DESCRIPTION
  Called to process the AT+BLDN command.

===========================================================================*/
LOCAL void bt_ag_cpe_process_redial
(
  void
)
{
  
  bt_ag_response_type  rsp = BT_AG_RESP_ERROR;

  BT_MSG_SIG( "BT AG RX: AT+BLDN", 0, 0, 0 );

  if ( MULTIPARTY_SUPPORTED() || (HF_DATA.call_active == FALSE) )
  {
    rsp = BT_AG_RESP_OK;
    bt_ag_send_event( BT_EV_AG_DEV_REDIAL );
  }
  bt_ag_cpe_send_response( rsp, 0 );

}

/*===========================================================================

FUNCTION
  bt_ag_cpe_process_supported_features

DESCRIPTION
  Called to process the AT+BRSF=<features bitmap> command.
  <features bitmap> is a 32-bit unsigned integer.

  For now, assume the bitmap comes as character string representing a decimal
  value, i.e. 17 (bit4 and bit0 set) - TBD

  In response to this AT command, AT sends back to HF +BRSF response.
  
===========================================================================*/
LOCAL void bt_ag_cpe_process_supported_features
(
  void
)
{
  
  int32    bitmap       = bt_ag_param2int32( CPE_PARAM_0 );
  boolean  wait_for_SDP = FALSE;

  BT_MSG_SIG( "BT AG RX: AT+BRSF=%d (0x%04x)", bitmap, bitmap, 0 );

#ifdef FEATURE_BT_HFP_1_5
  /* If it's not a consecutive AT+BRSF received after another AT+BRSF */
  if ( !ISBITSET( bt_ag.flags, BRSF_B ) )
#endif /* FEATURE_BT_HFP_1_5 */
  {
    if ( HF_DATA.connection_state != BT_AG_HCS_BRSF )
    {
      if ( HF_DATA.features_bitmap == (uint32)bitmap )
      {
        if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_BRSF, 0 );
          bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
        }
        else
        {
          BT_ERR( "BT AG ERR: SLC conn st (%d) != BRSF",
                  HF_DATA.connection_state, 0, 0 );
          bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
        }
      }
      else
      {
        /* different bitmap value received, current SLC shall be
        ** considered invalid.  Thus, AG initiates SLC release.
        */
        BT_ERR( "BT AG ERR: Diff HF Feature Bitmap Rcvd - curr 0x%x rcvd 0x%x",
                HF_DATA.features_bitmap, bitmap, 0 );
        BT_MSG_HIGH( "BT AG: Releasing SLC...", 0, 0, 0 );
        bt_cmd_ag_disconnect( bt_ag.app_id_ag );
        /* after SLC has been torn down, HF may initiate SLC by
        ** sending AT+BRSF again with the new feature bitmap.
        */
      }
    }
    else if ( bitmap >= 0 )
    {
#ifdef FEATURE_BT_HFP_1_5
      /* SDP response not yet received? */
      if ( !ISBITSET( bt_ag.flags, SDP_RESP_B ) )
      {
        /* If Error/Timeout response to bt_ag_query_hf_sdp_info() 
        ** has been received.
        */
        if ( HF_DATA.profile_version == HFP_VER_1_5 )
        {
          HF_DATA.profile_version = HFP_VER_1_0; /* reset version number */
        }
        else
        {
          SETBIT( bt_ag.flags, BRSF_B );
        }
        /* If HF supports extended features defined in HFP 1.5
        ** it must support HFP 1.5.
        */
        if ( bitmap & BT_HF_HFP_1_5_EXT_FEATURES )
        {
          HF_DATA.profile_version = HFP_VER_1_5;
        }
        /* If no Error/Timeout response received */
        else if ( ISBITSET( bt_ag.flags, BRSF_B ) )
        {
          wait_for_SDP = TRUE;
        }
      }
      else
      {
        CLRBIT( bt_ag.flags, SDP_RESP_B );
        /* Some HFs may have nothing in their SDP responses.
        ** AG shall take advantage of the bitmap received.
        */
        if ( bitmap & BT_HF_HFP_1_5_EXT_FEATURES )
        {
          HF_DATA.profile_version = HFP_VER_1_5;
        }
      }
#endif /* FEATURE_BT_HFP_1_5 */    
      HF_DATA.features_bitmap = (uint32) bitmap;
      if ( wait_for_SDP == FALSE )
      {
        bt_ag_establish_service_level_connection();
      }
    }
    else
    {
      bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_ind_event_report_act

DESCRIPTION
  Called to process the AT+CMER=<mode>,0,0,<ind> command.
  <mode> is expected to be either 0 or 3.
  <ind> is expected to be either 0 or 1.

===========================================================================*/
LOCAL void bt_ag_cpe_process_ind_event_report_act
(
  void
)
{
  
  bt_ag_response_type  rsp  = BT_AG_RESP_ERROR;
  int32                mode = bt_ag_param2int32( CPE_PARAM_0 );
  int32                ind  = bt_ag_param2int32( CPE_PARAM_3 );

  BT_MSG_SIG( "BT AG RX: AT+CMER=%d,0,0,%d", mode, ind, 0 );

  if ( ind == 0 )
  {
    HF_DATA.ier_mode = BT_AIER_NO_IER;
    rsp              = BT_AG_RESP_OK;
  }
  else if ( ind == 1 )
  {
    if ( mode == 0 )
    {
      HF_DATA.ier_mode = BT_AIER_KEEP;
      rsp              = BT_AG_RESP_OK;
    }
    else if ( mode == 3 )
    {
      HF_DATA.ier_mode = BT_AIER_FWD;
      rsp              = BT_AG_RESP_OK;
    }
  }

  if ( rsp == BT_AG_RESP_OK )
  {
    if ( HF_DATA.connection_state == BT_AG_HCS_CMER )
    {
      bt_ag_establish_service_level_connection();
    }
    else if ( HF_DATA.connection_state != BT_AG_HCS_DONE )
    {
      BT_ERR( "BT AG ERR: SLC conn st (%d) != CMER",
              HF_DATA.connection_state, 0, 0 );

      rsp = BT_AG_RESP_ERROR;
    }
  }
  else
  {
    BT_ERR( "BT AG: Bad CMER Parameters", 0, 0, 0 );
  }

  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_disable_nr_ec

DESCRIPTION
  Called to process AT+NREC=<nrec> command received from HF.
  <nrec> is expected to be 0; service level connection must already be
  established.

===========================================================================*/
LOCAL void bt_ag_cpe_process_disable_nr_ec
(
  void
)
{
  
  bt_ag_response_type  rsp  = BT_AG_RESP_ERROR;
  int32                nrec = bt_ag_param2int32( CPE_PARAM_0 );

  BT_MSG_SIG( "BT AG RX: AT+NREC=%d", nrec, 0, 0 );

  if ( nrec == 0 )
  {
    HF_DATA.nrec_off = TRUE;
    rsp              = BT_AG_RESP_OK;
  }
  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cm_inband_cmd_cb

DESCRIPTION
  Callback function for cm_mm_inband_cmd_*().

===========================================================================*/
LOCAL void bt_ag_cm_inband_cmd_cb
(
  void*                     data_ptr,  /*  Not used.  */
  cm_inband_cmd_e_type      cmd,
  cm_inband_cmd_err_e_type  cmd_err
)
{

  if ( cmd_err != CM_INBAND_CMD_ERR_NOERR )
  {
    BT_ERR( "BT AG: CM Inband Cmd Err cmd=%x err=%x", cmd, cmd_err, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_dtmf_generation_cmd

DESCRIPTION
  Called to process AT+VTS=<code> command received from HF.
  <code> is expected to be a single character in the set 0-9,#,*,A-D

===========================================================================*/
LOCAL void bt_ag_cpe_process_generate_dtmf
(
  void
)
{
  
  char*                code_ptr = &bt_agcpe.rx_buf[ CPE_PARAM_0 ];
  bt_ag_response_type  rsp      = BT_AG_RESP_ERROR;

  BT_MSG_SIG( "BT AG RX: AT+VTS=%c", *code_ptr, 0, 0 );

  if ( (HF_DATA.call_active) &&
       (bt_ag.state == BT_AGS_AUDIO_CONNECTED) &&
       (strlen( code_ptr ) == 1) )  /*  DTMF code is a single character.  */
  {
    /*  Verify DTMF code.  */
    if ( IS_DIGIT( *code_ptr ) ||
         ((*code_ptr >= 'A') && (*code_ptr <= 'D')) ||
         ((*code_ptr == '#') || (*code_ptr == '*')) )
    {
      bt_ag_send_event( BT_EV_AG_DEV_SEND_DTMF );

      if ( BT_AG_IN_CDMA )
      {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        BT_MSG_API( "BT AG CMD TX: CM Inband Burst DTMF", 0, 0, 0 );
        if ( cm_mm_inband_cmd_burst_dtmf( bt_ag_cm_inband_cmd_cb, NULL,
                                          HF_DATA.cm_client_id, HF_DATA.call_id,
                                          CAI_DTMF_ON_150, CAI_DTMF_OFF_100,
                                          1, (uint8*)code_ptr ) == TRUE )
        {
          rsp = BT_AG_RESP_OK;
        }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
      }
      else if ( BT_AG_IN_GW )
      {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM*/
      }
      else
      {
        BT_ERR( "BT AG: SS=%x", HF_DATA.ss_info.sys_mode, 0, 0 );
      }
    }
  }

  if ( HF_DATA.dtmf_started == FALSE )
  {
    bt_ag_cpe_send_response( rsp, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_call_waiting_act

DESCRIPTION
  Called to process AT+CCWA=<val> command received from HF.
  <val> is expected to be 0 or 1.

===========================================================================*/
LOCAL void bt_ag_cpe_process_call_waiting_act
(
  void
)
{
  
  bt_ag_response_type  rsp = BT_AG_RESP_ERROR;
  int32                val = bt_ag_param2int32( CPE_PARAM_0 );

  BT_MSG_SIG( "BT AG RX: AT+CCWA=%d", val, 0, 0 );

  if ( (val == 0) || (val == 1) )
  {
    HF_DATA.notify_call_waiting = (val == 0) ? FALSE : TRUE;
    rsp = BT_AG_RESP_OK;
  }
  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_call_hangup

DESCRIPTION
  Called to process AT+CHUP command received from HF.  This could be for
  hanging up an existing call, or for rejecting an incoming call.

===========================================================================*/
LOCAL void bt_ag_cpe_process_call_hangup
(
  void
)
{

  bt_ag_response_type         rsp = BT_AG_RESP_OK;
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif

  BT_MSG_SIG( "BT AG RX: AT+CHUP calls=%d cs=%d held=%d", HF_DATA.num_calls, 
              HF_DATA.call_setup_state, HF_DATA.call_held_state );

  if ( HF_DATA.num_calls > 1 )
  {
    if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_TERMINATED )
    {
      /* shall do nothing to held calls */
      rsp = BT_AG_RESP_ERROR;
    }
    else
    {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */
      {
        /* end the active call */
        bt_ag_end_current_call();

        /* if in CDMA, waiting/held call may be rung back
         * after active call ends
         */
      }
    }
  }
  else if ( HF_DATA.num_calls == 1 )
  {
    /* we end held call in single call
     * scenarios somehow, as some headsets do
     * send CHUP for this purpose.
     */
    bt_ag_send_event( BT_EV_AG_DEV_HANGUP );  /*  User hung up.  */    
  }
  else
  {
    rsp = BT_AG_RESP_ERROR;
  }

  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_call_hold_test

DESCRIPTION
  Called to process AT+CHLD=? command received from HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_call_hold_test
(
  void
)
{

  BT_MSG_SIG( "BT AG RX: AT+CHLD=?", 0, 0, 0 );
  BT_MSG_DEBUG( "BT AG: sys=%d HFfeat=0x%x", HF_DATA.ss_info.sys_mode,
                HF_DATA.features_bitmap, 0 );
  if ( HF_DATA.connection_state == BT_AG_HCS_CHLD_TEST )
  {
    bt_ag_establish_service_level_connection();
  }
  else if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CHLD, 0 );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    BT_ERR( "BT AG ERR: SLC conn st (%d) != CHLD_TEST",
            HF_DATA.connection_state, 0, 0 );
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_call_hold

DESCRIPTION
  Called to process AT+CHLD=<val> command received from HF.
  <val> is expected to be 0-4.

===========================================================================*/
LOCAL void bt_ag_cpe_process_call_hold
(
  void
)
{

  bt_ag_response_type         rsp = BT_AG_RESP_CME_ERROR;
  uint32                      err = CME_ERR_NO_NETWORK_SERVICE;
  int32                       val = bt_ag_param2int32( CPE_PARAM_0 );
  cm_call_sups_params_s_type  sups_params;
  cm_call_sups_type_e_type    sups_type = CM_CALL_SUPS_TYPE_NONE;


  BT_MSG_SIG( "BT AG RX: AT+CHLD=%d", val, 0, 0 );
  /* AG now accepts AT+CHLD = 1 sent by devices not setting the *
   * Multi-Party bit                                            */
  if ( (HF_DATA.num_calls > 0) &&
       (MULTIPARTY_SUPPORTED() || (val == 1)) )
  {
    BT_MSG_DEBUG( "BT AG: call_active=%d held_st=%d setup=%d", 
                  HF_DATA.call_active, HF_DATA.call_held_state, 
                  HF_DATA.call_setup_state );

    /*  Call supplementary service.  */
    memset( &sups_params, 0, sizeof(cm_call_sups_params_s_type));

    if ( BT_AG_IN_CDMA )
    {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      if ( (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) &&
           ISBITSET( bt_ag.flags, CDMA_CW_B ) )
      {
        rex_clr_timer( &bt_ag_timer );
      }

      sups_type                        = CM_CALL_SUPS_TYPE_SIMPLE_FLASH;
      sups_params.call_id              = HF_DATA.call_id;
      sups_params.sups_number.len      = 0;
      sups_params.sups_number.buf[ 0 ] = 0;

      switch ( (bt_ag_call_hold_type) val )
      {
        case BT_AG_CH_RELEASE_ALL:
        {
          sups_type = CM_CALL_SUPS_TYPE_NONE;

          if ( (HF_DATA.call_held_state == BT_AG_CALL_HELD_NONE) &&
               (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) )
          {
            rsp = BT_AG_RESP_OK;

            if ( ISBITSET( bt_ag.flags, CDMA_CW_B ) )
            {
              /* to ignore the waiting call.
               *
               * Note: if the waiting call is ignored on the
               * phone UI directly instead of sending this AT
               * command, we'll update the call info table after
               * BT_AG_1X_CW_TIMEOUT.
               */

              rex_set_timer( &bt_ag_timer, 10 ); /* 10 ms */

              /* TODO: the way how waiting call is handled can be
               * fine tuned. e.g we might not need to set the timer
               * and handle it in bt_ag_timer_expired().
               */
            }
            else /* else, single call case */
            {
              bt_ag_send_event( BT_EV_AG_DEV_HANGUP );
            }
          }
          break;
        }
        case BT_AG_CH_RELEASE_ACTIVE:
        {
          if ( HF_DATA.num_calls == 1 )
          {
            bt_ag_send_event( BT_EV_AG_DEV_HANGUP ); /* end the only call */
            sups_type = CM_CALL_SUPS_TYPE_NONE;
            rsp = BT_AG_RESP_OK;
          }
          else if ( ISBITSET( bt_ag.flags, CDMA_CW_B ) ||
                    (HF_DATA.call_held_state == BT_AG_CALL_HELD_ACTIVE) )
          {
            /* hang up active call first */
            bt_ag_end_current_call();

            /* waiting call will be answered after active call ends */
            sups_type = CM_CALL_SUPS_TYPE_NONE;
            rsp = BT_AG_RESP_OK;
          }
          else
          {
            sups_type = CM_CALL_SUPS_TYPE_NONE;
          }
          break;
        }
        case BT_AG_CH_HOLD_ACTIVE:
        {
          if ( HF_DATA.call_held_state != BT_AG_CALL_HELD_NONE )
          {
            /* toggle between active and held calls */
            sups_params.sups_number = HF_DATA.phone_num;
          }
          break;
        }
        case BT_AG_CH_ADD_NEW:
        {
          if ( (HF_DATA.call_held_state != BT_AG_CALL_HELD_ACTIVE) ||
               (HF_DATA.call_setup_state != BT_AG_CALL_SETUP_OUT) )
          {
            /* no 3rd party MO call to add */
            sups_type = CM_CALL_SUPS_TYPE_NONE;
          }
          break;
        }
        default:
        {
          BT_ERR( "BT AG: CHLD Bad/Unsupported Hold Action %d", val, 0, 0 );
          sups_type = CM_CALL_SUPS_TYPE_NONE;
          break;
        }
      }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
    }
    else if ( BT_AG_IN_GW )
    {
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM*/
    }

    if ( sups_type != CM_CALL_SUPS_TYPE_NONE )
    {
      BT_MSG_DEBUG( "BT AG: CM supsT=%d id=%x", 
                    sups_type, sups_params.call_id, 0 );
      cm_mm_call_cmd_sups( bt_ag_cm_call_cmd_cb, NULL,
                           HF_DATA.cm_client_id,
                           sups_type, &sups_params );

      rsp = BT_AG_RESP_OK;
    }
  }

  bt_ag_cpe_send_response( rsp, err );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_read_indicators

DESCRIPTION
  Called to process AT+CIND? command received from HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_read_indicators
(
  void
)
{
  
  BT_MSG_SIG( "BT AG RX: AT+CIND?", 0, 0, 0 );
  
  if ( (HF_DATA.connection_state != BT_AG_HCS_CIND_READ) &&
       (HF_DATA.connection_state != BT_AG_HCS_DONE) )
  {
    BT_ERR( "BT AG ERR: SLC conn st (%d) != CIND_READ",
            HF_DATA.connection_state, 0, 0 );
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
  }
  else if ( HF_DATA.connection_state != BT_AG_HCS_CIND_READ )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CIND, BT_AG_RESP_INFO );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    bt_ag_establish_service_level_connection();
  }

}


/*===========================================================================

FUNCTION
  bt_ag_query_hf_sdp_info

DESCRIPTION
  Sends command to SD to search for supported features bitmap and profile
  desciptor list from HF.

===========================================================================*/
LOCAL void bt_ag_query_hf_sdp_info
(
  void
)
{
  
  bt_sd_srch_pat_uuid_list_type     uuid_list;
  bt_sd_srch_pat_attr_id_list_type  attr_id_list;
  uint8                             idx = 0;

  uuid_list.num_uuid128 = 0;
  uuid_list.num_uuid = 1;
  uuid_list.uuid[0]  = BT_SD_SERVICE_CLASS_HANDSFREE;

#ifdef FEATURE_BT_HFP_1_5
  attr_id_list.num_attr_id             = idx+1;
  attr_id_list.attr_id[ idx ].is_range = FALSE;
  attr_id_list.attr_id[ idx ].value    = 
    BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST;
  idx++;
#endif

  attr_id_list.num_attr_id             = idx+1;
  attr_id_list.attr_id[ idx ].is_range = FALSE;
  attr_id_list.attr_id[ idx ].value    = BT_SD_ATTR_ID_SUPPORTED_FEATURES;
  idx++;

  BT_MSG_API( "BT AG CMD TX: SD Search Service", 0, 0, 0 );
  bt_cmd_sd_search_service( bt_ag.app_id_ag,
                            &bt_ag.audio_device.bd_addr,
                            &uuid_list, &attr_id_list,
                            BT_SD_DEFAULT_MAX_ATTR_BYTE_COUNT );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_get_ind_mapping

DESCRIPTION
  Called to process AT+CIND=? command received from HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_get_ind_mapping
(
  void
)
{
  
  BT_MSG_SIG( "BT AG RX: AT+CIND=?", 0, 0, 0 );

  if ( HF_DATA.connection_state == BT_AG_HCS_BRSF )
  {
    /*  Could be a v0.96 HF which doesn't send AT+BRSF cmd.  */
    HF_DATA.connection_state = BT_AG_HCS_CIND_TEST;

#ifdef FEATURE_BT_HFP_1_5
    if ( ISBITSET( bt_ag.flags, SDP_RESP_B ) )
    {
      CLRBIT( bt_ag.flags, SDP_RESP_B );
    }
    /* If failed to get HF service info by last query */
    else if ( HF_DATA.profile_version == HFP_VER_1_5 )
#endif /* FEATURE_BT_HFP_1_5 */
    {
      HF_DATA.profile_version = HFP_VER_1_0;
      bt_ag_query_hf_sdp_info();      
    }
  }

  if ( HF_DATA.connection_state == BT_AG_HCS_CIND_TEST )
  {
    bt_ag_establish_service_level_connection();    
  }
  else if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CIND, BT_AG_RESP_LIST );
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
  else
  {
    BT_ERR( "BT AG ERR: SLC conn st (%d) != CIND_TEST",
            HF_DATA.connection_state, 0, 0 );
    bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
  }

}


#ifdef FEATURE_BT_HFP_1_5
/*===========================================================================

FUNCTION
  bt_ag_cpe_process_list_call_info

DESCRIPTION
  Called to process AT+CLCC command received from HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_list_call_info
(
  void
)
{

  bt_ag_response_type         rsp   = BT_AG_RESP_ERROR;
  uint8                       index = 0;
  uint8                       len   = 0;
  bt_ag_clcc_direction_type   call_direction;
  bt_ag_clcc_state_type       call_state;
  bt_ag_clcc_mode_type        call_mode;
  boolean                     call_multiparty;
  uint8                       call_num_type;
  char                        tmp_str[ BT_AG_RESP_MAX_LEN ];
  uint8                       call_id_no = call_info_table.num_call_ids;

  BT_MSG_SIG( "BT AG RX: AT+CLCC - calls=%d",
              call_info_table.num_call_ids, 0, 0 );

  /* each iteration processes information for one call */
  for ( ; (call_id_no > 0) &&
          (index < ARR_SIZE( call_info_table.call_info )); index++ )
  {
    if ( call_info_table.call_info[index].call_id != CM_CALL_ID_INVALID )
    {
      call_id_no--;

      /* obtain call direction */
      /* outgoing : incoming */
      call_direction = (call_info_table.call_info[index].direction ==
                          CM_CALL_DIRECTION_MO) ?
                       BT_AG_CLCC_DIRECTION_MO : BT_AG_CLCC_DIRECTION_MT;

      /* obtain call state */
      switch ( call_info_table.call_info[index].call_state )
      {
        case CM_CALL_STATE_CONV:
        {
          if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_NONE )
          {
            call_state = BT_AG_CLCC_STATE_ACTIVE; /* active */
          }
          else if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_TERMINATED )
          {
            call_state = BT_AG_CLCC_STATE_HELD; /* held */
          }
          else
          {
            /* active : held */
            call_state = (call_info_table.call_info[index].call_id ==
                            HF_DATA.call_id) ? 
                         BT_AG_CLCC_STATE_ACTIVE : BT_AG_CLCC_STATE_HELD;
          }
          break;
        }
        case CM_CALL_STATE_ORIG:
        {
          call_state = BT_AG_CLCC_STATE_DIALING; /* dialing */
          break;
        }
        case CM_CALL_STATE_CC_IN_PROGRESS:
        {
          call_state = BT_AG_CLCC_STATE_ALERTING; /* alerting */      
          break;
        }
        case CM_CALL_STATE_INCOM:
        {
          /* incoming : waiting */
          call_state = (call_info_table.call_info[index].call_id ==
                          HF_DATA.call_id) ? 
                       BT_AG_CLCC_STATE_INCOMING : BT_AG_CLCC_STATE_WAITING;
          break;
        }
        default:
        {
          MSG_ERROR( "BT AG: +CLCC - invalid call state %d id=%x",
                     call_info_table.call_info[index].call_state,
                     call_info_table.call_info[index].call_id, 0 );
          continue; // skip this call
        }
      }

      /* obtain the call mode */
      switch ( call_info_table.call_info[index].call_type )
      {
        case CM_CALL_TYPE_VOICE:
        case CM_CALL_TYPE_EMERGENCY:
        {
          call_mode = BT_AG_CLCC_MODE_VOICE; /* voice */
          break;
        }
        case CM_CALL_TYPE_PS_DATA:
        {
          call_mode = BT_AG_CLCC_MODE_DATA; /* data */
          break;
        }
        case CM_CALL_TYPE_CS_DATA:
        {
          call_mode = BT_AG_CLCC_MODE_FAX; /* fax */
          break;
        }
        default:
        {
          MSG_ERROR( "BT AG: +CLCC - invalid call mode %d id=%x",
                     call_info_table.call_info[index].call_type,
                     call_info_table.call_info[index].call_id, 0 );
          continue; // skip this call
        }
      }
      
      /* obtain multi-party info (conference or not) */
      /* multiparty : not multiparty */
      call_multiparty = 
        ( (call_info_table.num_call_ids > 1) &&
          (HF_DATA.call_held_state != BT_AG_CALL_HELD_ACTIVE) &&
          (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_NONE) ) ?
        BT_AG_CLCC_MPTY : BT_AG_CLCC_NOT_MPTY;

      call_info_table.call_info[index].num.
        buf[call_info_table.call_info[index].num.len] = '\0';

      /* obtain the number type */
      /* INTERNATIONAL : UNKNOWN */
      call_num_type = (call_info_table.call_info[index].num.buf[0] == '+') ?
                      BT_AG_CLCC_INTERNATIONAL : BT_AG_CLCC_UNKNOWN;

      /* format the response packet */
      snprintf( tmp_str, sizeof( tmp_str ) - 1,
                "%d,%d,%d,%d,%d,\"%s\",%d",
                index + 1, call_direction, call_state,
                call_mode, call_multiparty,
                (char*) call_info_table.call_info[index].num.buf,
                call_num_type );
      tmp_str[sizeof( tmp_str ) - 1] = '\0';

      /* debug msg for QXDM */
      len += BT_STRFORMAT( bt_ag_qxdm_msg + len,
                           sizeof( bt_ag_qxdm_msg ) - 1 - len,
                           "+CLCC: %s;", (const char*) tmp_str );
            
      bt_ag_cpe_send_response( BT_AG_RESP_CLCC, (uint32)(tmp_str) );
    }
  }

  if ( call_info_table.num_call_ids > 0 )
  {
    bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
    BT_MSG_DEBUG( "BT AG TX:", 0, 0, 0 );
    BT_MSG_DEBUG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
  }

  rsp = BT_AG_RESP_OK;

  bt_ag_cpe_send_response( rsp, 0 );

}
#endif /* FEATURE_BT_HFP_1_5 */


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_caller_id_act

DESCRIPTION
  Called to process AT+CLIP=<val> command received from HF.
  <val> is expected to be 0 or 1.

===========================================================================*/
LOCAL void bt_ag_cpe_process_caller_id_act
(
  void
)
{
  
  bt_ag_response_type  rsp = BT_AG_RESP_ERROR;
  int32                val = bt_ag_param2int32( CPE_PARAM_0 );

  BT_MSG_SIG( "BT AG RX: AT+CLIP=%d", val, 0, 0 );

  if ( (val == 0) || (val == 1) )
  {
    HF_DATA.notify_cli = (val == 0) ? FALSE : TRUE;
    rsp = BT_AG_RESP_OK;
  }
  bt_ag_cpe_send_response( rsp, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_vr_activation

DESCRIPTION
  Called to process AT+BVRA=<val> command received from HF.
  <val> is expected to be 0 or 1.
  If AG does not support VR, it doesn't support BVRA command.

===========================================================================*/
LOCAL void bt_ag_cpe_process_vr_activation
(
  void
)
{
  
  bt_ag_response_type  rsp = BT_AG_RESP_ERROR;
  int32                val = bt_ag_param2int32( CPE_PARAM_0 );
  boolean              enabling_vr = (val == 0) ? FALSE : TRUE;

  BT_MSG_SIG( "BT AG RX: AT+BVRA=%d", val, 0, 0 );

  if ( ((bt_ag.features_bitmap & BT_AG_F_VR_ON) == BT_AG_F_VR_ON) &&
       ((val == 0) || (val == 1)) )
  {
    if ( HF_DATA.vr_enabled != enabling_vr )
    {
      HF_DATA.vr_enabled = enabling_vr;

      bt_ag_send_event( BT_EV_AG_VR_TOGGLE );
    }
    rsp = BT_AG_RESP_OK;
  }
  
  bt_ag_cpe_send_response( rsp, 0 );

}


#ifdef BT_AG_HF_EXTENSION
/*===========================================================================

FUNCTION
  bt_ag_cpe_process_btrh

DESCRIPTION
  Called to process AT+BTRH command received from HF.  The input parameter
  indicates which flavor of this command was received.

===========================================================================*/
LOCAL void bt_ag_cpe_process_btrh
(
  bt_ag_cmd_sub_type  sub_type
)
{

  boolean         keep_going = TRUE;
  bt_ev_msg_type  event_msg;
  int32           rh_state = 0;

  if ( sub_type == BT_AG_CMD_SET )
  {
    rh_state = bt_ag_param2int32( CPE_PARAM_0 );
    if ( (rh_state < 0) || (rh_state >= BT_AG_RH_MAX) ||
         (HF_DATA.supported_rh[ rh_state ] == FALSE) )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_OP_NOT_SUPPORTED );
      keep_going = FALSE;
    }
  }

  if ( keep_going )
  {
    BT_MSG_API( "BT AG EV TX: BTRH subT=%x rhSt=%d", sub_type, rh_state, 0 );
    event_msg.ev_hdr.ev_type   = BT_EV_AG_BTRH_RCVD;
    event_msg.ev_hdr.bt_app_id = bt_ag.app_id_user;
    event_msg.ev_msg.ev_ag_btrh.sub_type = sub_type;
    event_msg.ev_msg.ev_ag_btrh.rh_state = rh_state;
    bt_ec_send_event( &event_msg );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_send_pap_event

DESCRIPTION
  Builds and sends an PAP specific event up to BT app (i.e. UI).

===========================================================================*/
LOCAL void bt_ag_send_pap_event
(
  bt_event_type  ev_type,
  uint32         generic   /*  Additional info.  */
)
{
  
  bt_ev_msg_type  event_msg;
  boolean         success = TRUE;

  switch ( ev_type )
  {
    case BT_EV_AG_PAP_CPBS:
    {
      BT_MSG_API( "BT AG EV TX: CPBS (%d)", generic, 0, 0 );
      event_msg.ev_msg.ev_ag_pap_cpbs.sub_type = 
        (bt_pap_cmd_sub_type)(generic);

      if ( (bt_pap_cmd_sub_type)(generic) != BT_PAP_CMD_TEST )
      {
        event_msg.ev_msg.ev_ag_pap_cpbs.selected_pb = 
          bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type;
      }
      break;
    }

    case BT_EV_AG_PAP_CPBR:
    {
      BT_MSG_API( "BT AG EV TX: CPBR Idx %x", generic, 0, 0 );
      event_msg.ev_msg.ev_ag_pap_cpbr.mem_entry = (uint16)(generic);
      event_msg.ev_msg.ev_ag_pap_cpbr.pb = 
        bt_ag_ph_name_table[ HF_DATA.selected_pb_idx ].ph_type;
      break;
    }

    case BT_EV_AG_PAP_CPBF:
    {
      BT_MSG_API( "BT AG EV TX: CPBF", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_pap_cpbf =
        *((bt_ev_ag_pap_cpbf_type*)(generic));
      break;
    }

    case BT_EV_AG_PAP_CPBW:
    {
      BT_MSG_API( "BT AG EV TX: CPBW", 0, 0, 0 );
      event_msg.ev_msg.ev_ag_pap_cpbw =
        *((bt_ev_ag_pap_cpbw_type*)(generic));
      break;
    }

    case BT_EV_AG_PAP_CNUM:
      BT_MSG_API( "BT AG EV TX: CNUM", 0, 0, 0 );
      /*  No additional info.  */
      break;

    default:
      BT_ERR( "BT AG: Bad PAP Event Type %x", ev_type, 0, 0 );
      success = FALSE;
      break;
  }

  if ( success != FALSE )
  {
    event_msg.ev_hdr.bt_app_id = bt_ag.app_id_user;
    event_msg.ev_hdr.ev_type   = ev_type;

    bt_ec_send_event( &event_msg );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_cpbs

DESCRIPTION
  Called to process AT+CPBS command received from HF.  The input parameter
  indicates which flavor of this command was received.

===========================================================================*/
LOCAL void bt_ag_cpe_process_cpbs
(
  bt_pap_cmd_sub_type  sub_type
)
{

  char*    ph_name = &bt_agcpe.rx_buf[ CPE_PARAM_0 ];
  boolean  found = FALSE;
  uint8    i;
  boolean  keep_going = TRUE;

  if ( sub_type == BT_PAP_CMD_SET )
  {
    for ( i=0; i<ARR_SIZE(bt_ag_ph_name_table) && !found; i++ )
    {
      if ( (strcmp( ph_name, bt_ag_ph_name_table[i].pb_name ) == 0) &&
           (bt_ag_ph_name_table[i].supported != FALSE) )
      {
        HF_DATA.selected_pb_idx = i;
        found = TRUE;
      }
    }
    if ( !found )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_OP_NOT_SUPPORTED );
      keep_going = FALSE;
    }
  }
  else if ( sub_type == BT_PAP_CMD_READ )
  {
    if ( HF_DATA.selected_pb_idx == ARR_SIZE(bt_ag_ph_name_table) )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_OP_NOT_ALLOWED );
      keep_going = FALSE;
    }
  }

  if ( keep_going )
  {
    bt_ag_send_pap_event( BT_EV_AG_PAP_CPBS, sub_type );
  }
  
}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_cpbr

DESCRIPTION
  Called to process AT+CPBR=idx1[,idx2] command received from HF.
  The absence of idx2 means only entry at idx1 is to be read.

===========================================================================*/
LOCAL void bt_ag_cpe_process_cpbr
(
  bt_pap_cmd_sub_type  sub_type
)
{
  
  if ( sub_type == BT_PAP_CMD_SET )
  {
    boolean  one_entry  = (bt_agcpe.rx_param_count == 1) ? TRUE : FALSE;
    int32    start_idx  = bt_ag_param2int32( CPE_PARAM_0 );
    int32    end_idx    =  one_entry ? -1 : bt_ag_param2int32( CPE_PARAM_1 );
  
    if ( (start_idx < 0) || (start_idx >= HF_DATA.pb_total_recs) ||
         (!one_entry && ((end_idx < 0) || (end_idx >=
                                             HF_DATA.pb_total_recs))) )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR, CME_ERR_INVALID_INDEX );
    }
    else
    {
      int32 i;
      HF_DATA.entries_2_read = 0;
  
      if ( !one_entry && (end_idx < start_idx) )
      {
        i         = end_idx;
        end_idx   = start_idx;
        start_idx = i;
        HF_DATA.entries_2_read = end_idx - start_idx + 1;
      }
  
      for ( i=start_idx; i <= end_idx; i++ )
      {
        bt_ag_send_pap_event( BT_EV_AG_PAP_CPBR, i );
        HF_DATA.entries_2_read++;
      }
    }
  }
  else /*  Must be BT_PAP_CMD_TEST.  */
  {
    bt_ag_send_pap_event( BT_EV_AG_PAP_CPBR, -1 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_cpbf

DESCRIPTION
  Called to process AT+CPBF=<findtext> command received from HF.
  <findtext> is in UCS2 (Unicode?) format.

===========================================================================*/
LOCAL void bt_ag_cpe_process_cpbf
(
  void
)
{

  bt_ev_ag_pap_cpbf_type  ev;

  if ( (CPE_PARAM_LEN_0 & 1) == 0 )  /*  Divisible by 2?  */
  {
    if ( CPE_PARAM_LEN_0 > BT_PAP_MAX_TEXT_LENGTH )
    {
      CPE_PARAM_LEN_0 = BT_PAP_MAX_TEXT_LENGTH;
    }
    memcpy( (void*)ev.find_text,
            (void*)&bt_agcpe.rx_buf[ CPE_PARAM_0 ], 
            CPE_PARAM_LEN_0 );
    ev.find_text[ (CPE_PARAM_LEN_0/sizeof(uint16)) ] = 0;

    bt_ag_send_pap_event( BT_EV_AG_PAP_CPBF, (uint32)&ev );
  }
  else
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                             CME_ERR_BAD_CHAR_IN_TEXT_STR );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_cpbw

DESCRIPTION
  Called to process AT+CPBW=[<idx>][,<num>[,<type>[,<text>]]] command 
  received from HF.  <text> is in UCS2 (Unicode?) format.

===========================================================================*/
LOCAL void bt_ag_cpe_process_cpbw
(
  void
)
{
  
  bt_ev_ag_pap_cpbw_type ev;
  boolean  keep_going = TRUE;
  uint16*  text_str = (uint16*)&bt_agcpe.rx_buf[ CPE_PARAM_3 ];

  ev.erase_entry = (bt_agcpe.rx_param_count == 1);
  ev.mem_entry = -1;

  if ( CPE_PARAM_LEN_0 != 0 )
  {
    ev.mem_entry = bt_ag_param2int32( CPE_PARAM_0 );
    if ( ev.mem_entry >= HF_DATA.pb_total_recs )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_INVALID_INDEX );
      keep_going = FALSE;
    }
  }

  if ( (keep_going != FALSE) && (ev.erase_entry != FALSE) && 
       (ev.mem_entry == -1) )
  {
    bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                             CME_ERR_INVALID_INDEX );
    keep_going = FALSE;
  }

  if ( (keep_going != FALSE) && (bt_agcpe.rx_param_count > 1) )
  {
    if ( strlen( &bt_agcpe.rx_buf[ CPE_PARAM_1 ] ) > 
         BT_PAP_MAX_NUM_LENGTH )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_DIAL_STRING_TOO_LONG );
      keep_going = FALSE;
    }
    else if ( (CPE_PARAM_3 & 1) != 0 )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_BAD_CHAR_IN_TEXT_STR );
    }
    else if ( (CPE_PARAM_3/sizeof(uint16)) > BT_PAP_MAX_TEXT_LENGTH )
    {
      bt_ag_cpe_send_response( BT_AG_RESP_CME_ERROR,
                               CME_ERR_TEXT_TOO_LONG );
      keep_going = FALSE;
    }
  }

  if ( keep_going != FALSE )
  {
    if ( bt_agcpe.rx_param_count > 1 )
    {
      BT_STRCPY( (char*)ev.phone_num, 
                 &bt_agcpe.rx_buf[ CPE_PARAM_1 ],
                 sizeof( ev.phone_num ) );
      ev.type = bt_agcpe.rx_buf[ CPE_PARAM_2 ];
      memcpy ( (void*)ev.text, (void*)text_str, CPE_PARAM_LEN_3 );
      ev.text[ (CPE_PARAM_LEN_3/sizeof(uint16)) ] = 0;
    }
    bt_ag_send_pap_event( BT_EV_AG_PAP_CPBW, (uint32)&ev );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_cnum_cmd

DESCRIPTION
  Called to process AT+CNUM command received from HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_cnum
(
  void
)
{
  
  bt_ag_send_pap_event( BT_EV_AG_PAP_CNUM, 0 );

}
#endif /* BT_AG_HF_EXTENSION */


/*===========================================================================

FUNCTION
  bt_ag_cpe_process_command

DESCRIPTION
  Called to process the command received from HS/HF.

===========================================================================*/
LOCAL void bt_ag_cpe_process_command
(
  void
)
{

  if ( AUDIO_DEV.device_type == BT_AD_HANDSFREE )  /* Hands-Free device */
  {
    if ( HF_DATA.connection_state != BT_AG_HCS_DONE ) /* SLC is not up yet */
    {
      switch ( bt_agcpe.cmd_idx )
      {
        case BT_AG_CMD_BRSF:      /*  HF supported features notification.  */
          bt_ag_cpe_process_supported_features();
          break;

        case BT_AG_CMD_CIND_TEST:    /*  Get mapping of indicators in AG.  */
          bt_ag_cpe_process_get_ind_mapping();
          break;

        case BT_AG_CMD_CIND_READ:    /*  Get indicators status.  */
          bt_ag_cpe_process_read_indicators();
          break;

        case BT_AG_CMD_CMER:      /*  Activate indicator event reporting.  */
          bt_ag_cpe_process_ind_event_report_act();
          break;

        case BT_AG_CMD_CHLD:         /*  Call hold & multiparty.  */
          if ( (bt_agcpe.rx_buf[ CPE_PARAM_0 ] == '?') &&
               (bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ] == NULL) )
          {
            /*  Get the supported multiparty call services.  */
            bt_ag_cpe_process_call_hold_test();
          }
          else
          {
            BT_MSG_SIG( "BT AG RX: AT+CHLD=%d",
                        bt_ag_param2int32( CPE_PARAM_0 ), 0, 0 );
            /*  can't handle call hold & multiparty now  */
            BT_ERR( "BT AG ERR: SLC Not Established!", 0, 0, 0 );
            bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
          }
          break;

        default:
          /* Replace '\r' with '\0' */
          bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

          BT_STRFORMAT( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ),
                        "BT AG RX: %s", (const char*) bt_agcpe.rx_buf );
          bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
          BT_MSG_SIG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
          BT_ERR( "BT AG ERR: SLC Not Established!",
                  bt_agcpe.cmd_idx, 0, 0 );

          bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
          break;
      }
    }
    else /* SLC is established already */
    {
      switch ( bt_agcpe.cmd_idx )
      {
        case BT_AG_CMD_BRSF:      /*  HF supported features notification.  */
          bt_ag_cpe_process_supported_features();
          break;
        
        case BT_AG_CMD_CIND_TEST:    /*  Get mapping of indicators in AG.  */
          bt_ag_cpe_process_get_ind_mapping();
          break;

        case BT_AG_CMD_CIND_READ:    /*  Get indicators status.  */
          bt_ag_cpe_process_read_indicators();
          break;

        case BT_AG_CMD_CMER:      /*  Activate indicator event reporting.  */
          bt_ag_cpe_process_ind_event_report_act();
          break;

        case BT_AG_CMD_CHLD:         /*  Call hold & multiparty.  */
          if ( (bt_agcpe.rx_buf[ CPE_PARAM_0 ] == '?') &&
               (bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ] == NULL) )
          {
            /*  Get the supported multiparty call services.  */
            bt_ag_cpe_process_call_hold_test();
          }
          else
          {
            /*  Handle call hold & multiparty.  */
            bt_ag_cpe_process_call_hold();
          }
          break;

        case BT_AG_CMD_VGM:          /*  Microphone volume gain.  */
        case BT_AG_CMD_VGS:          /*  Speaker volume gain.     */
          bt_ag_cpe_process_volume_gain();
          break;
    
        case BT_AG_CMD_CALL_ANSWER:  /*  Answer call.  */
          bt_ag_cpe_process_call_answer();
          break;
    
        case BT_AG_CMD_DIAL:         /*  Dial number (direct or memory).  */
          bt_ag_cpe_process_dial();
          break;
    
        case BT_AG_CMD_BINP:         /*  Request data input.  */
          BT_MSG_SIG( "BT AG RX: AT+BINP - not supported yet", 0, 0, 0 );
          bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
          break;

#ifndef FEATURE_BT_HFP_1_5
        case BT_AG_CMD_BLDN:         /*  Call last number dialed.  */
          bt_ag_cpe_process_redial();
          break;
#endif
     
        case BT_AG_CMD_BVRA:         /*  Toggle VR function in AG.  */
          bt_ag_cpe_process_vr_activation();
          break;
    
        case BT_AG_CMD_CCWA:         /*  Toggle call waiting notification.  */
          bt_ag_cpe_process_call_waiting_act();
          break;
        
        case BT_AG_CMD_CHUP:         /*  Hang up call.  */
          bt_ag_cpe_process_call_hangup();
          break;

#ifdef FEATURE_BT_HFP_1_5
        case BT_AG_CMD_CLCC:         /*  List current calls  */
          bt_ag_cpe_process_list_call_info();
          break;
#endif /* FEATURE_BT_HFP_1_5 */

        case BT_AG_CMD_CLIP:         /*  Toggle calling line ID notif.  */
          bt_ag_cpe_process_caller_id_act();
          break;
        
        case BT_AG_CMD_NREC:         /*  Disable EC & NR functions in AG.  */
          bt_ag_cpe_process_disable_nr_ec();
          break;
    
        case BT_AG_CMD_VTS:          /*  Transmit DTMF tone.  */
          bt_ag_cpe_process_generate_dtmf();
          break;
    
#ifdef BT_AG_HF_EXTENSION
        case BT_AG_CMD_BTRH_READ:
          bt_ag_cpe_process_btrh( BT_AG_CMD_READ );
          break;
    
        case BT_AG_CMD_BTRH:
          if ( (bt_agcpe.rx_buf[ CPE_PARAM_0 ] == '?') &&
               (bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ] == NULL) )
          {
            bt_ag_cpe_process_btrh( BT_AG_CMD_TEST );
          }
          else
          {
            bt_ag_cpe_process_btrh( BT_AG_CMD_SET );
          }
          break;
    
        case BT_AG_CMD_PAP_CPBS_READ:
          bt_ag_cpe_process_cpbs( BT_PAP_CMD_READ );
          break;
    
        case BT_AG_CMD_PAP_CPBS:
          if ( (bt_agcpe.rx_buf[ CPE_PARAM_0 ] == '?') &&
               (bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ] == NULL) )
          {
            bt_ag_cpe_process_cpbs( BT_PAP_CMD_TEST );
          }
          else
          {
            bt_ag_cpe_process_cpbs( BT_PAP_CMD_SET );
          }
          break;
    
        case BT_AG_CMD_PAP_CPBR:
          if ( (bt_agcpe.rx_buf[ CPE_PARAM_0 ] == '?') &&
               (bt_agcpe.rx_buf[ CPE_PARAM_0 + 1 ] == NULL) )
          {
            bt_ag_cpe_process_cpbr( BT_PAP_CMD_TEST );
          }
          else
          {
            bt_ag_cpe_process_cpbr( BT_PAP_CMD_SET );
          }
          break;

        case BT_AG_CMD_PAP_CPBF:
          bt_ag_cpe_process_cpbf();
          break;
    
        case BT_AG_CMD_PAP_CPBW:
          bt_ag_cpe_process_cpbw();
          break;

        case BT_AG_CMD_PAP_CNUM:
          bt_ag_cpe_process_cnum();
          break;
#endif /* BT_AG_HF_EXTENSION */
    
        default:
          /* Replace '\r' with '\0' */
          bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

          BT_STRFORMAT( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ),
                        "BT AG RX: %s", (const char*) bt_agcpe.rx_buf );
          bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
          BT_MSG_SIG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
          BT_ERR( "BT AG ERR: Bad cmd_idx %x",
                  bt_agcpe.cmd_idx, 0, 0 );

          bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
          break;
      }
    }
  }
  else if ( AUDIO_DEV.device_type == BT_AD_HEADSET )  /* Headset device */
  {
    switch ( bt_agcpe.cmd_idx )
    {
      case BT_AG_CMD_CKPD:         /*  HS button pressed.  */
        bt_ag_cpe_process_button_pressed();
        break;
  
      case BT_AG_CMD_VGM:          /*  Microphone volume gain.  */
      case BT_AG_CMD_VGS:          /*  Speaker volume gain.     */
        bt_ag_cpe_process_volume_gain();
        break;

      default:
        /* Replace '\r' with '\0' */
        bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

        BT_STRFORMAT( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ),
                      "BT AG RX: %s", (const char*) bt_agcpe.rx_buf );
        bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
        BT_MSG_SIG( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );
        BT_ERR( "BT AG ERR: Bad cmd_idx %x",
                bt_agcpe.cmd_idx, 0, 0 );

        bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
        break;
    }
  }
  
  bt_ag_cpe_reset_engine();

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_append_rx_byte

DESCRIPTION
  Adds specified byte to end of parsing engine receive buffer while
  checking for overflow.  If overflow occurs the parsing engine is
  reset.

RETURN VALUE
  TRUE if all OK.
  FALSE if error occurred.
  
===========================================================================*/
LOCAL boolean bt_ag_cpe_append_rx_byte
(
  byte  rx_byte
)
{

  boolean  success = TRUE;

  if ( bt_agcpe.state != BT_AG_ACPS_RX_OVERFLOW )
  {
    if ( bt_agcpe.rx_count < BT_AG_CPE_MAX_CMD_LEN )
    {
      bt_agcpe.rx_buf[ bt_agcpe.rx_count++ ] = rx_byte;
    }
    else
    {
      success        = FALSE;
      bt_agcpe.state = BT_AG_ACPS_RX_OVERFLOW;

      BT_ERR( "BT AG: AT cmd RX overflow %x", bt_agcpe.rx_count, 0, 0 );
      
      bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
      
      if ( rx_byte == BT_AG_CMD_TERM_CHAR )
      {
        bt_ag_cpe_reset_engine();
      }
    }
  }

  return ( success );

}


/*===========================================================================

FUNCTION
  bt_ag_cpe_command_recognized

DESCRIPTION
  Updates the command parsing engine command index by comparing the
  defined command texts to the string in the parsing engine receive
  buffer.  The parsing engine command index is updated and in some
  cases the engine state as well.  A command is considered recognized
  when the complete text has been received.

RETURN VALUE
  TRUE if command recognized.
  FALSE if command not recognized.
  
===========================================================================*/
LOCAL boolean bt_ag_cpe_command_recognized
(
  void
)
{

  boolean  cmd_found = FALSE;
  uint16   i, j;
  char     cur_rx_char_upper_case;

  i = bt_agcpe.cmd_idx;       /*  Command index.            */
  j = bt_agcpe.cmd_char_idx;  /*  Receive character index.  */

  while ( (cmd_found == FALSE) &&
          (bt_agcpe.state != BT_AG_ACPS_UNRECOGNIZED_CMD) )
  {
    cur_rx_char_upper_case = TO_UPPER( (bt_agcpe.rx_buf[ j ]) );

    if ( bt_ag_cpe_cmd[ i ].cmd_text[ j ] == '\0' )
    {
      /*  Characters received match entire command text.  */
      bt_agcpe.cmd_idx = (bt_ag_cmd_type)i;
      cmd_found        = TRUE;
    }
    else if ( j == bt_agcpe.rx_count )
    {
      /*  Have compared all received characters.  */
      /*  Exit without a match (yet).             */
      bt_agcpe.cmd_char_idx = j;
      break;
    }
    else if ( bt_ag_cpe_cmd[ i ].cmd_text[ j ] ==
                cur_rx_char_upper_case )
    {
      /*  Received character matches command text  */
      /*  character.  Move to next character.      */
      j++;
    }
    else
    {
      if ( bt_agcpe.rx_count > 2 )
      {
        /*  Character mis-match between command text    */
        /*  in table and received character.  Move      */
        /*  to next command in table.  Start comparing  */
        /*  at first character again.                   */

        bt_ag_cpe_set_command_idx( ++bt_agcpe.cmd_idx );

        i = bt_agcpe.cmd_idx;
        j = 0;
      }
      else
      {
        /*  First 1 or 2 characters received were   */
        /*  not "AT" as expected for all commands.  */
        /*  Throw out all characters received so    */
        /*  far and start the command matching      */
        /*  process all over beginning with the     */
        /*  next character received.                */
        
        BT_MSG_HIGH( "BT AG: Discard Rcvd Invalid Info", 0, 0, 0 );
        bt_ag_cpe_reset_engine();
        break;
      }
    }
  }

  return ( cmd_found );

}

/*===========================================================================

FUNCTION
  bt_ag_check_ext_err_cmd

DESCRIPTION
  If the unrecognized AT command (to be forwarded to app) is AT+CMEE, AG 
  needs to process it as well.

===========================================================================*/
LOCAL void bt_ag_check_ext_err_cmd( void )
{

  char tmp_buf[3]; /* expect only "=1" */
  char* buf_ptr = &bt_agcpe.rx_buf[ 7 ];
  char* tmp_buf_ptr = tmp_buf;

  if ( memcmp( bt_agcpe.rx_buf, "AT+CMEE", 7 ) == 0 )
  {
    while ( (*buf_ptr != BT_AG_CMD_TERM_CHAR) &&
            (tmp_buf_ptr < &tmp_buf[ 2 ]) )
    {
      if ( *buf_ptr != ' ' )
      {
        *tmp_buf_ptr++ = *buf_ptr;
      }
      buf_ptr++;
    }
    if ( memcmp( tmp_buf, "=1", 2 ) == 0 )
    {
      HF_DATA.ext_err_enabled = TRUE;
      BT_MSG_DEBUG( "BT AG: Extended Error enabled", 0, 0, 0 );
    }
  }

}

/*===========================================================================

FUNCTION
  bt_ag_cpe_process_rx_byte

DESCRIPTION
  Called to process the next byte received from HS/HF.  This is the
  entry point to the AG command parsing engine.  Recognized commands
  are processed and responses are sent as appropriate.  Unrecognized
  commands may be forwarded via external I/O configuration.

===========================================================================*/
LOCAL void bt_ag_cpe_process_rx_byte
(
  byte rx_byte  /*  Byte received from HS/HF.  */
)
{
  switch ( bt_agcpe.state )
  {
    case BT_AG_ACPS_GET_CMD_IDX:
      if ( bt_ag_cpe_append_rx_byte( rx_byte ) != FALSE )
      {
        if ( (bt_ag_cpe_command_recognized() != FALSE) &&
             (bt_agcpe.cmd_idx < ARR_SIZE(bt_ag_cpe_cmd)) ) /* Klocwork */
        {
          if ( bt_ag_cpe_cmd[ bt_agcpe.cmd_idx ].param_cnt > 0 )
          {
            bt_agcpe.state = BT_AG_ACPS_EXTRACT_PARAMS;

            bt_ag_cpe_init_parameter( bt_agcpe.rx_param_count );
          }
          else
          {
            bt_ag_cpe_process_command();
          }
        }
        else if ( bt_agcpe.state == BT_AG_ACPS_UNRECOGNIZED_CMD )
        {
          /*  If end of unrecognized command received.  */
          /*  Forward to application and/or discard     */
          /*  as appropriate.                           */
          if ( bt_agcpe.rx_buf[bt_agcpe.rx_count - 1] == BT_AG_CMD_TERM_CHAR )
          {
            if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
            {
              if ( bt_ag_cpe_handle_unrecognized_cmd() != FALSE )
              {
                bt_ag_check_ext_err_cmd();
              }
            }
            else
            {
              /* Replace '\r' with '\0' */
              bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

              BT_STRFORMAT( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ),
                            "BT AG RX: %s - Unrecognized AT CMD Rcvd",
                            (const char*) bt_agcpe.rx_buf );
              bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
              BT_MSG_HIGH( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );

              bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
            }

            bt_ag_cpe_reset_engine();
          }
        }
      }
      break;

    case BT_AG_ACPS_EXTRACT_PARAMS:
      /*  Ignore spaces in parameter list.  */
      if ( rx_byte != ' ' )
      {
        if ( bt_ag_cpe_append_rx_byte( rx_byte ) != FALSE )
        {
          if ( (rx_byte != BT_AG_CMD_TERM_CHAR) &&
               (rx_byte != BT_AG_CMD_PARAM_SEPARATOR) )
          {
            if ( bt_agcpe.rx_param_count < BT_AG_CPE_MAX_CMD_PARAMS )
            {
              /*  Parameter not complete yet.  */
              bt_agcpe.rx_param_len[ bt_agcpe.rx_param_count ]++;
            }
          }
          else
          {
            /*  Parameter complete.  Overwrite    */
            /*  terminating character with NULL.  */
            bt_agcpe.rx_buf[ (bt_agcpe.rx_count - 1) ] = NULL;
            bt_agcpe.rx_param_count++;

            if ( rx_byte != BT_AG_CMD_TERM_CHAR )
            {
              if ( bt_agcpe.rx_param_count < BT_AG_CPE_MAX_CMD_PARAMS )
              {
                /*  Prepare for next parameter.  */
                bt_ag_cpe_init_parameter( bt_agcpe.rx_param_count );
              }
            }
            else if ( bt_agcpe.cmd_idx < ARR_SIZE(bt_ag_cpe_cmd) ) /* Klocwork */
            {
              if ( bt_agcpe.rx_param_count !=
                     bt_ag_cpe_cmd[ bt_agcpe.cmd_idx ].param_cnt )
              {
                BT_ERR( "BT AG ERR: CPE param cnt mismatch CI %x E %x C %x",
                              bt_agcpe.cmd_idx,
                              bt_ag_cpe_cmd[ bt_agcpe.cmd_idx ].param_cnt,
                              bt_agcpe.rx_param_count );
              }

              if ( bt_agcpe.rx_param_count <
                     bt_ag_cpe_cmd[ bt_agcpe.cmd_idx ].param_cnt )
              {
                bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
                bt_ag_cpe_reset_engine();
              }
              else
              {
                /*  End of command received.  Process it.  */
                bt_ag_cpe_process_command();
              }
            }
          }
        }
      }
      break;

    case BT_AG_ACPS_UNRECOGNIZED_CMD:
      if ( (bt_ag_cpe_append_rx_byte( rx_byte ) != FALSE) &&
           (rx_byte == BT_AG_CMD_TERM_CHAR) )
      {
        if ( HF_DATA.connection_state == BT_AG_HCS_DONE )
        {
          /*  End of unrecognized command received.  */
          /*  Forward to application and/or discard  */
          /*  as appropriate.                        */
          if ( bt_ag_cpe_handle_unrecognized_cmd() != FALSE )
          {
            bt_ag_check_ext_err_cmd( );
          }
        }
        else
        {
          /* Replace '\r' with '\0' */
          bt_agcpe.rx_buf[ bt_agcpe.rx_count - 1 ] = '\0';

          BT_STRFORMAT( bt_ag_qxdm_msg, sizeof( bt_ag_qxdm_msg ),
                        "BT AG RX: %s - Unrecognized AT CMD Rcvd",
                        (const char*) bt_agcpe.rx_buf );
          bt_ag_qxdm_msg[ sizeof( bt_ag_qxdm_msg ) - 1 ] = '\0';
          BT_MSG_HIGH( (const char*) bt_ag_qxdm_msg, 0, 0, 0 );

          bt_ag_cpe_send_response( BT_AG_RESP_ERROR, 0 );
        }

        bt_ag_cpe_reset_engine();
      }
      break;

    case BT_AG_ACPS_RX_OVERFLOW:
      if ( rx_byte == BT_AG_CMD_TERM_CHAR )
      {
        bt_ag_cpe_reset_engine();
      }
      break;

    default:
      BT_ERR( "BT AG: CPE bad state %x", bt_agcpe.state, 0, 0 );
      bt_ag_cpe_reset_engine();
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by AG.

===========================================================================*/
LOCAL uint16 bt_ag_get_max_event_bytes
(
  void
)
{
  
  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connect_req_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connected_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_connection_failed_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rm_disconnected_sco_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_service_search_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_error_resp_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return ( max_eb );

}


/*===========================================================================

FUNCTION
  bt_ag_ev_rm_conn_failed_sco

DESCRIPTION
  Processes the BT_EV_RM_CONNECTION_FAILED_SCO event.

===========================================================================*/
LOCAL void bt_ag_ev_rm_conn_failed_sco
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: RM SCO ConnF St %x", bt_ag.state, 0, 0 );

  switch ( bt_ag.state )
  {
    case BT_AGS_AUDIO_CONNECTING:
    case BT_AGS_AUDIO_CONNECTED:
      bt_ag_send_event( BT_EV_AG_AUDIO_CONNECT_FAILED );
      bt_ag_sco_cleanup();
      if ( bt_ag.idle_timeout > 0)
      {
        BT_MSG_DEBUG( "BT AG: RM SCO CONNF TO %x",
                      bt_ag.idle_timeout, 0, 0 );
        rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );
      }
      break;

    default:
      BT_ERR( "BT AG: RM ConnF SCO AG St %x", bt_ag.state, 0, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ev_cmd_done

DESCRIPTION
  Processes the BT_EV_GN_CMD_DONE event.

===========================================================================*/
LOCAL void bt_ag_ev_cmd_done
(
  bt_ev_gn_cmd_done_type*  ev_cmd_done_ptr
)
{
  
  switch( ev_cmd_done_ptr->cmd_status )
  {
    case BT_CS_GN_SUCCESS:
      switch ( ev_cmd_done_ptr->cmd_type )
      {
        case BT_CMD_RM_REGISTER_SCO:
          // last step of enabling is done; inform app
          BT_MSG_HIGH( "BT AG: AG is enabled.", 0, 0, 0 );
          /* register with CM, when enabling AG */
          if ( bt_ag_cm_reg() == FALSE )  /*  Failed to register with CM?  */
          {
            BT_ERR("BT AG:  Failed to register with CM",0,0,0);
            bt_ag_cm_dereg();
          }
          bt_ag_send_volume_to_snd_task( AUDIO_DEV.spkr_volume );
          bt_ag_send_event( BT_EV_AG_ENABLED );
          break;

        case BT_CMD_RM_UNREGISTER_SCO:
          // last step of disabling is done; inform app
          BT_MSG_HIGH( "BT AG: AG is disabled", 0, 0, 0 );
          bt_ag_send_event( BT_EV_AG_DISABLED );
          /* de-register with CM, when disabling AG */
          bt_ag_cm_dereg();
          bt_ag_cleanup();
          rex_clr_timer( &bt_ag_timer );  /*  Deactivate idle timer.  */
          break;
      }
      break;

    case BT_CS_RM_NO_CONNECTION:
      if ( ev_cmd_done_ptr->cmd_type == BT_CMD_RM_CONNECT_SCO )
      {
        BT_MSG_HIGH( "BT AG EV RX: RM Conn SCO cmd Failed", 0, 0, 0 );
        bt_ag.event_reason = BT_EVR_RM_NO_CONNECTION;
        bt_ag_ev_rm_conn_failed_sco();
      }
      break;

    case BT_CS_RM_UNRECOGNIZED_CMD:
    case BT_CS_RM_MAX_CONNECTIONS:
    case BT_CS_RM_MAX_DEVICES:
    case BT_CS_RM_MAX_APPLICATIONS:
    case BT_CS_RM_BAD_HANDLE:
    case BT_CS_RM_BUSY:
    case BT_CS_RM_CONN_ACTIVE:
    case BT_CS_RM_NAME_REQ_FAILED:
    default:
      BT_ERR( "BT AG: Cmd Done T %x Bad Stat %x",
              ev_cmd_done_ptr->cmd_type,
              ev_cmd_done_ptr->cmd_status, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_snd_cb

DESCRIPTION
  Callback function for snd_**_ctl() functions.

===========================================================================*/
LOCAL void bt_ag_snd_cb
(
  const void*      client_data_ptr,
  snd_status_type  status
)
{

  BT_MSG_DEBUG( "BT AG: Snd CB EC/NR %x Status %x",
                *((uint8*)client_data_ptr), status, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_ev_rm_disconnected_sco

DESCRIPTION
  Processes the BT_EV_RM_DISCONNECTED_SCO event.

===========================================================================*/
LOCAL void bt_ag_ev_rm_disconnected_sco
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: RM Disc'd SCO St %x Dis %x", 
                bt_ag.state, bt_ag.disabling, 0 );

  switch ( bt_ag.state )
  {
    case BT_AGS_AUDIO_CONNECTED:
      bt_ag.sco_handle = BT_AG_INVALID_SCO_HANDLE;
      /*  Falls through.  */

    case BT_AGS_AUDIO_DISCONNECTING:
      BT_MSG_HIGH( "BT AG: SCO Disconnected", 0, 0, 0 );
      bt_ag_send_event( BT_EV_AG_AUDIO_DISCONNECTED );
      bt_ag.state = BT_AGS_CONNECTED;

      if ( HF_DATA.nrec_off )
      {
        uint8 ecnr = 1;
        BT_MSG_API( "BT AG CMD TX: SND Echo Cancelling on", 0, 0, 0 );
        snd_ec_ctl( SND_EC_NS_NORMAL_OPERATION, bt_ag_snd_cb, (void*)&ecnr );

        ecnr = 2;
        BT_MSG_API( "BT AG CMD TX: SND Noise Suppression on", 0, 0, 0 );
        snd_ns_ctl( SND_EC_NS_NORMAL_OPERATION, bt_ag_snd_cb, (void*)&ecnr );
      }

      if ( bt_ag.idle_timeout > 0 )
      {
        BT_MSG_DEBUG( "BT AG: set timer in rm_disc_sco", 0, 0, 0 );
        rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );
      }
      break;

     case BT_AGS_OPENING:
     case BT_AGS_OPEN:
      /*  This is the case where RFCOMM link goes down while AG is in   */
      /*  audio connected state.  This event comes after AG closed SIO  */
      /*  stream and opened another in server mode.  Audio link has     */
      /*  been disconnected.                                            */
      bt_ag_send_event( BT_EV_AG_AUDIO_DISCONNECTED );
      break;

    case BT_AGS_DISABLED:
      BT_MSG_DEBUG( "BT AG: Clr timer in rm_disc_sco", 0, 0, 0 );
      rex_clr_timer( &bt_ag_timer );
      break;

    default:
      BT_ERR( "BT AG: RM Disc'd AG St %x", bt_ag.state, 0, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ev_sd_service_search_resp

DESCRIPTION
  Processes the BT_EV_SD_SERVICE_SEARCH_RESP event.

===========================================================================*/
LOCAL void bt_ag_ev_sd_service_search_resp
(
  bt_ev_sd_service_search_resp_type*  ev_ptr
)
{

  bt_sd_srv_rec_type*  srv_rec_ptr = NULL;
  uint8                i;
  boolean              features_found = FALSE;
#ifdef FEATURE_BT_HFP_1_5
  boolean              profile_version_found = FALSE;
  boolean              continue_slc          = FALSE;

  /* if AT+BRSF has been processed and
  ** AG is waiting for SLC to be setup.
  */
  if ( ISBITSET( bt_ag.flags, BRSF_B ) &&
       (HF_DATA.profile_version < HFP_VER_1_5) )
  {
    continue_slc = TRUE;
  }
#endif /* FEATURE_BT_HFP_1_5 */

  if ( BT_BD_ADDRS_EQUAL( &ev_ptr->bd_addr, &AUDIO_DEV.bd_addr) )
  {
    BT_MSG_DEBUG( "BT AG: recCnt %x", ev_ptr->srv_rec_count, 0, 0 );

    bt_sd_srv_rec_iter.init( BT_SD_DB_TYPE_SRV_SRCH_RESULT_DB,
                             BT_SD_ITER_ALL );
    for ( srv_rec_ptr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.first();
          srv_rec_ptr != NULL;
          srv_rec_ptr = (bt_sd_srv_rec_type*) bt_sd_srv_rec_iter.next()
         )
    {
      if ( srv_rec_ptr->num_srv_attr > 0 )
      {
        for ( i=0; 
              (i<srv_rec_ptr->num_srv_attr)
#ifdef FEATURE_BT_HFP_1_5
              && (!features_found || !profile_version_found);
#else
              && !features_found;
#endif /* FEATURE_BT_HFP_1_5 */
              i++ )
        {
          switch ( srv_rec_ptr->srv_attr[i].attr_id )
          {
            case BT_SD_ATTR_ID_SUPPORTED_FEATURES:
            {
#ifdef FEATURE_BT_HFP_1_5
              /* don't overwrite the bitmap received via BRSF */
              if ( !ISBITSET( bt_ag.flags, BRSF_B ) )
#endif /* FEATURE_BT_HFP_1_5 */
              {
                HF_DATA.features_bitmap = 
                  (uint32) (srv_rec_ptr->srv_attr[i].
                            attr_value.primitive_value & 0xFFFF);
                BT_MSG_DEBUG( "BT AG: HF supported features=0x%x",
                              HF_DATA.features_bitmap, 0, 0 );
              }
              
              features_found = TRUE;
              break;
            }
#ifdef FEATURE_BT_HFP_1_5
            case BT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST:
            {
              HF_DATA.profile_version = 
                bt_sd_extract_profile_version( &srv_rec_ptr->srv_attr[ i ] );
              profile_version_found = TRUE;

              BT_MSG_DEBUG( "BT AG: HFP version=0x%x", 
                            HF_DATA.profile_version, 0, 0 );
              break;
            }
#endif /* FEATURE_BT_HFP_1_5 */
            default:
              break;
          }
        }
      }
    }

#ifdef FEATURE_BT_HFP_1_5
    if ( !ISBITSET( bt_ag.flags, BRSF_B ) )
    {
      /* If AT+CIND=? command not processed yet */
      if ( HF_DATA.connection_state == BT_AG_HCS_BRSF )
      {
        SETBIT( bt_ag.flags, SDP_RESP_B );
      }
    }
    else
    {
      CLRBIT( bt_ag.flags, BRSF_B );
      /* If AT+BRSF=<features bitmap> command has been processed,
      ** and AG is waiting for SDP_resp to setup SLC.
      */
      if ( continue_slc )
      {
        bt_ag_establish_service_level_connection();
      }
    }
#endif /* FEATURE_BT_HFP_1_5 */
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ev_rm_connected_sco

DESCRIPTION
  Processes the BT_EV_RM_CONNECTED_SCO event.

===========================================================================*/
LOCAL void bt_ag_ev_rm_connected_sco
(
  bt_ev_rm_connected_sco_type*  ev_ptr
)
{
  
  BT_MSG_API( "BT AG EV RX: RM Conn SCO St %x", bt_ag.state, 0, 0 );

  if( bt_ag.state == BT_AGS_AUDIO_CONNECTING )
  {
    if ( BT_BD_ADDRS_EQUAL( &AUDIO_DEV.bd_addr, &ev_ptr->bd_addr ) )
    {
      BT_MSG_HIGH( "BT AG: AG SCO Connected", 0, 0, 0 );
      bt_ag.state       = BT_AGS_AUDIO_CONNECTED;
      bt_ag.sco_handle  = ev_ptr->handle;
      bt_ag_send_event( BT_EV_AG_AUDIO_CONNECTED );

      /*  Prevent going idle now that SCO link is up.  */
      rex_clr_timer( &bt_ag_timer );

      bt_ag.ring_count = 0;
      bt_ag.is_idle    = FALSE;

      if ( HF_DATA.nrec_off )
      {
        uint8 ecnr = 1;
        BT_MSG_API( "BT AG CMD TX: SND Echo Cancelling off", 0, 0, 0 );
        snd_ec_ctl( SND_EC_NS_FORCE_DISABLED, bt_ag_snd_cb, (void*)&ecnr );

        ecnr = 2;
        BT_MSG_API( "BT AG CMD TX: SND Noise Suppression off", 0, 0, 0 );
        snd_ns_ctl( SND_EC_NS_FORCE_DISABLED, bt_ag_snd_cb, (void*)&ecnr );
      }
    }
    else /*  SCO connection not from target HS.  */
    {
      BT_ERR( "BT AG: SCO Conn'd to wrong HS", 0, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT AG: RM Conn'd SCO EV AG St %x", bt_ag.state, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_event

DESCRIPTION
  Handles all BT driver events coming to the AG layer.

===========================================================================*/
LOCAL void bt_ag_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{
  
  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_API( "BT AG EV RX: Cmd Done Type %x Stat %x",
              ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
              ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status, 0 );
      bt_ag_ev_cmd_done( &ev_msg_ptr->ev_msg.ev_gn_cmd_done );
      break;

    case BT_EV_RM_CONNECT_REQ_SCO:
      BT_MSG_API( "BT AG EV RX: RM Conn Req SCO", 0, 0, 0 );
      if ( (AUDIO_DEV.device_type == BT_AD_HANDSFREE) &&
           BT_BD_ADDRS_EQUAL( &ev_msg_ptr->ev_msg.ev_rm_creqs.bd_addr,
                              &AUDIO_DEV.bd_addr ) &&
           (HF_DATA.connection_state == BT_AG_HCS_DONE) )
      {
        BT_MSG_API( "BT AG CMD TX: RM Accept SCO", 0, 0, 0 );
        bt_cmd_rm_accept_sco( bt_ag.app_id_ag, &AUDIO_DEV.bd_addr );
        bt_ag.state = BT_AGS_AUDIO_CONNECTING;
        rex_clr_timer( &bt_ag_timer );
      }
      else
      {
        BT_ERR( "BT AG: SCO req unexp dev=%d SLCst=%d", AUDIO_DEV.device_type,
                HF_DATA.connection_state, 0 );
        BT_MSG_API( "BT AG CMD TX: RM Reject SCO", 0, 0, 0 );
        bt_cmd_rm_reject_sco( bt_ag.app_id_ag,
                              &AUDIO_DEV.bd_addr,
                              BT_RMRJ_NO_RESOURCE );
      }
      break;

    case BT_EV_RM_CONNECTED_SCO:
      bt_ag_ev_rm_connected_sco( &ev_msg_ptr->ev_msg.ev_rm_conns );
      break;

    case BT_EV_RM_CONNECTION_FAILED_SCO:
      BT_MSG_API( "BT AG EV RX: RM ConnF St %x R %x",
                  bt_ag.state, ev_msg_ptr->ev_msg.ev_rm_confs.reason, 0 );
      bt_ag.event_reason = ev_msg_ptr->ev_msg.ev_rm_confs.reason;
      bt_ag_ev_rm_conn_failed_sco();
      break;

    case BT_EV_RM_DISCONNECTED_SCO:
      BT_MSG_API( "BT AG EV RX: RM Disc SCO R %x",
                  ev_msg_ptr->ev_msg.ev_rm_discs.reason, 0, 0 );
      bt_ag.event_reason = ev_msg_ptr->ev_msg.ev_rm_discs.reason;
      bt_ag_ev_rm_disconnected_sco();
      break;

    case BT_EV_SD_SERVICE_SEARCH_RESP:
      BT_MSG_API( "BT AG EV RX: SD Serv Srch Rsp", 0, 0, 0 );
      bt_ag_ev_sd_service_search_resp(
        &ev_msg_ptr->ev_msg.ev_sd_service_search_resp );
      break;

    case BT_EV_SD_ERROR_RESP:
    case BT_EV_SD_TIMEOUT_RESP:
      BT_MSG_API( "BT AG EV RX: SD Srch Error/Timeout Rsp ev=%x err=%x",
                  ev_msg_ptr->ev_hdr.ev_type,
                  ev_msg_ptr->ev_msg.ev_sd_error_resp.error_code, 0 );

#ifdef FEATURE_BT_HFP_1_5
      /* If no AT+BRSF=<features bitmap> received yet */
      if ( !ISBITSET( bt_ag.flags, BRSF_B ) )
      {
        /* If AT+CIND=? command not processed yet */
        if ( HF_DATA.connection_state == BT_AG_HCS_BRSF )
        {
          /* Semaphore that tells BT_EV_SD_ERROR_RESP
          ** or BT_EV_SD_TIMEOUT_RESP received
          */
          HF_DATA.profile_version = HFP_VER_1_5;
        }
      }
      else
      {
        CLRBIT( bt_ag.flags, BRSF_B );
        if ( HF_DATA.profile_version < HFP_VER_1_5 )
        {
          bt_ag_establish_service_level_connection();
        }
      }
#endif /* FEATURE_BT_HFP_1_5 */
      break;

    default:
      BT_ERR( "BT AG: Bad RX EV Type %x AID %x",
              ev_msg_ptr->ev_hdr.ev_type,
              ev_msg_ptr->ev_hdr.bt_app_id, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_store_bt_event

DESCRIPTION
  This callback function stores a specified BT event on the
  AG application BT event queue.

===========================================================================*/
LOCAL void bt_ag_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{
  
  bt_store_bt_event( bt_ev_ptr, &bt_ag.ev_q_info );

}


/*===========================================================================

FUNCTION
  bt_ag_every_rx_data_cb

DESCRIPTION
  This callback function is called by SIO every time rx data is enqueued
  onto the AG RX watermark.

===========================================================================*/
LOCAL void bt_ag_every_rx_data_cb
(
  void
)
{
  
  (void) rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );

}


/*===========================================================================

FUNCTION
  bt_ag_clear_pending_cmds

DESCRIPTION
  Checks for pending commands and send fail events.

===========================================================================*/
LOCAL void bt_ag_clear_pending_cmds
(
  void
)
{
  if ( bt_ag.ring_count > 0 )
  {
    bt_ag.ring_count = 0;  /*  Don't try to ring device again.  */
    bt_ag_send_event( BT_EV_AG_RING_FAILED );
  }
  if ( bt_ag.adding_audio != FALSE )
  {
    bt_ag.adding_audio = FALSE;
    bt_ag.event_reason = (bt_event_reason_type)BT_BE_NO_CONNECTION;
    bt_ag_send_event( BT_EV_AG_AUDIO_CONNECT_FAILED );
  }
}


/*===========================================================================

FUNCTION
  bt_ag_call_ev_cb

DESCRIPTION
  Callback function for cm_mm_client_call_reg().
  Updates indicator status for call and call_setup.

===========================================================================*/
LOCAL void bt_ag_call_ev_cb
(
  cm_call_event_e_type           call_event,
  const cm_mm_call_info_s_type*  call_info_ptr
)
{

  bt_ag_ev_type* ev_ptr;

  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    ev_ptr->hdr.ev_type        = BT_AG_CM_EV_CALL;
    ev_ptr->call_ev.call_event = call_event;
    ev_ptr->call_ev.call_info  = *call_info_ptr;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: Dropping CM Call Ev %x", call_event, 0, 0 );
  }

}

#ifndef FEATURE_BT_WITH_OLD_CM
/*===========================================================================

FUNCTION
  bt_ag_call_info_list_cb

DESCRIPTION
  Callback function invoked as result of cm_mm_call_cmd_get_call_info_list().

===========================================================================*/
LOCAL void bt_ag_call_info_list_cb
(
  const cm_call_state_info_list_s_type *list_ptr
)
{

  bt_ag_ev_type* ev_ptr;

  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    /* Copy passed data to local variable */
    memcpy( (void*)&HF_DATA.call_info_list,
            list_ptr,
            sizeof(cm_call_state_info_list_s_type) );

    ev_ptr->hdr.ev_type = BT_AG_CM_EV_CALL_INFO_LIST;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: Dropping CM Call list info", 0, 0, 0 );
  }

}
#endif /* ! FEATURE_BT_WITH_OLD_CM */


/*===========================================================================

FUNCTION
  bt_ag_ph_ev_cb

DESCRIPTION
  Callback function for cm_client_ph_reg().

===========================================================================*/
LOCAL void bt_ag_ph_ev_cb
(
  cm_ph_event_e_type        ph_event,
  const cm_ph_info_s_type*  ph_info_ptr
)
{
  
  bt_ag_ev_type*  ev_ptr;

  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    ev_ptr->hdr.ev_type    = BT_AG_CM_EV_PH;
    ev_ptr->ph_ev.ph_event = ph_event;
    ev_ptr->ph_ev.ph_info  = *ph_info_ptr;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: Dropping CM Phone Ev %x", ph_event, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_ss_ev_cb

DESCRIPTION
  Callback function for cm_mm_client_ss_reg().

===========================================================================*/
LOCAL void bt_ag_ss_ev_cb
(
  cm_ss_event_e_type           ss_event,
  const cm_mm_ss_info_s_type*  ss_info_ptr
)
{

  bt_ag_ev_type* ev_ptr;

  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    ev_ptr->hdr.ev_type    = BT_AG_CM_EV_SS;
    ev_ptr->ss_ev.ss_event = ss_event;
    ev_ptr->ss_ev.ss_info  = *ss_info_ptr;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: Dropping CM SS Ev %x", ss_event, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_inband_ev_cb

DESCRIPTION
  Callback function for cm_mm_client_inband_reg().

===========================================================================*/
LOCAL void bt_ag_inband_ev_cb
(
  cm_inband_event_e_type        inband_event,
  const cm_inband_info_s_type*  inband_info_ptr
)
{
  bt_ag_ev_type* ev_ptr;
  if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
  {
    ev_ptr->hdr.ev_type            = BT_AG_CM_EV_INBAND;
    ev_ptr->inband_ev.inband_event = inband_event;
    ev_ptr->inband_ev.inband_info  = *inband_info_ptr;
    q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
    rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
  }
  else
  {
    BT_ERR( "BT AG: Dropping CM Inband Ev", inband_event, 0, 0 );
  }
}


/*===========================================================================

FUNCTION
  bt_ag_cm_ss_cmd_cb

DESCRIPTION
  Callback function for cm_mm_ss_cmd_*().

===========================================================================*/
LOCAL void bt_ag_cm_ss_cmd_cb
(
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err
)
{

  BT_MSG_DEBUG( "BT AG: CM SS Cmd %x Err %x", cmd, cmd_err, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cm_ph_cmd_cb

DESCRIPTION
  Callback function for cm_ph_cmd_*().

===========================================================================*/
LOCAL void bt_ag_cm_ph_cmd_cb
(
  void                  *data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
)
{

  BT_MSG_DEBUG( "BT AG: CM PH Cmd %x Err %x", cmd, cmd_err, 0 );

}


/*===========================================================================

FUNCTION
  bt_ag_cm_reg

DESCRIPTION
  Register with Call Manager.

===========================================================================*/
LOCAL boolean bt_ag_cm_reg
(
  void
)
{
  
  cm_client_status_e_type  cm_client_status = CM_CLIENT_OK;

  if ( HF_DATA.cm_client_id == CM_CLIENT_TYPE_BLUETOOTH )
  {
    return ( TRUE );  /*  Already registered.  */
  }

  /*  Initialize Call Manager client object.  */
  cm_client_status = cm_client_init( CM_CLIENT_TYPE_BLUETOOTH,
                                     &HF_DATA.cm_client_id );
  BT_MSG_DEBUG( "BT AG: CM Client ID %x", HF_DATA.cm_client_id, 0, 0 );

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Register with CM for call events.  */
    cm_client_status = cm_mm_client_call_reg( HF_DATA.cm_client_id,
                                              bt_ag_call_ev_cb,
                                              CM_CLIENT_EVENT_REG,
                                              CM_CALL_EVENT_ORIG,
                                              CM_CALL_EVENT_INFO,
                                              NULL );
    BT_MSG_DEBUG( "BT AG: Client call_reg Status %x",
                  cm_client_status, 0, 0 );
  }

#ifndef FEATURE_BT_WITH_OLD_CM
  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Register for call info list query results.  */
    cm_client_status = cm_mm_client_call_info_list_reg( HF_DATA.cm_client_id,
                                                        bt_ag_call_info_list_cb,
                                                        CM_CLIENT_EVENT_REG, 
                                                        NULL );
    BT_MSG_DEBUG( "BT AG: Client call_info_list_reg Status %x",
                  cm_client_status, 0, 0 );
  }
#endif

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Register with CM for phone events.  */
    cm_client_status = cm_client_ph_reg( HF_DATA.cm_client_id,
                                         bt_ag_ph_ev_cb,
                                         CM_CLIENT_EVENT_REG,
                                         CM_PH_EVENT_INFO,
                                         CM_PH_EVENT_INFO,
                                         NULL );
    BT_MSG_DEBUG( "BT AG: ph_reg Status %x", cm_client_status, 0, 0 );
  }

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Register with CM for serving-system events.  */
    cm_client_status = cm_mm_client_ss_reg( HF_DATA.cm_client_id,
                                            bt_ag_ss_ev_cb,
                                            CM_CLIENT_EVENT_REG,
                                            CM_SS_EVENT_SRV_CHANGED,
                                            CM_SS_EVENT_INFO,
                                            NULL );
    BT_MSG_DEBUG( "BT AG: ss_reg Status %x", cm_client_status, 0, 0 );
  }

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Register with CM for inband events.  */
    cm_client_status =
      cm_mm_client_inband_reg( HF_DATA.cm_client_id,
                               bt_ag_inband_ev_cb,
                               CM_CLIENT_EVENT_REG,
                               CM_INBAND_EVENT_REV_BURST_DTMF,
                               CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF,
                               NULL );
    BT_MSG_DEBUG( "BT AG: inband_reg Status %x", cm_client_status, 0, 0 );
  }

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Activate my client object.  */
    cm_client_status = cm_client_act( HF_DATA.cm_client_id );
    BT_MSG_DEBUG( "BT AG: client_act Status %x", cm_client_status, 0, 0 );
  }

  if ( cm_client_status == CM_CLIENT_OK )
  {
    /*  Request serving-system information.  */
    cm_ss_cmd_get_ss_info( bt_ag_cm_ss_cmd_cb, NULL, HF_DATA.cm_client_id );
    cm_ph_cmd_get_ph_info( bt_ag_cm_ph_cmd_cb, NULL, HF_DATA.cm_client_id );
  }
  else
  {
    BT_ERR( "BT AG: CM reg failed Status %x", cm_client_status, 0, 0 );
  }

  return ( cm_client_status == CM_CLIENT_OK ) ? TRUE : FALSE;

}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_connect_failed

DESCRIPTION
  Processes the SPP connect failed event received from SIO indicating
  RFCOMM link couldn't be brought up.

===========================================================================*/
LOCAL void bt_ag_process_spp_connect_failed
(
  sio_stream_id_type  stream_id
)
{
  
  switch ( bt_ag.state )
  {
    case BT_AGS_OPENING:  /*  Trying to connect to audio device.  */
    case BT_AGS_OPEN:
      if ( bt_ag.disabling )
      {
        bt_ag_clear_pending_cmds();
        bt_ag_close_sio();
      }
      else if ( (stream_id == bt_ag.sio_stream_id_hs) ||
                (stream_id == bt_ag.sio_stream_id_hf) )
      {
        bt_ag.going_idle = TRUE;

        /* trying to send RING or bring up audio link? */
        if ( (bt_ag.ring_count > 0) || (bt_ag.adding_audio != FALSE) )
        {
          bt_ag_clear_pending_cmds();
        }
        else if ( bt_ag.is_sio_server != FALSE )
        {
          if ( bt_ag.event_reason != BT_EVR_AG_CONN_FAILED )
          {
            /* server stream at OPENING state is ready for action */
            bt_ag.going_idle = FALSE;

            if ( bt_ag.event_reason != BT_EVR_RM_SECURITY_NOT_MET )
            {
              bt_ag_send_event( BT_EV_AG_CONNECTION_FAILED );
            }
          }
        }
        else if ( bt_ag.idle_mode == BT_AGIM_LOW_POWER )
        {
          if ( ++bt_ag.conn_attempts >= BT_AG_MAX_CONNECTION_ATTEMPTS )
          {
            BT_MSG_HIGH( "BT AG: %x connect attempts failed",
                         bt_ag.conn_attempts, 0, 0 );
            bt_ag.conn_attempts = 0;
            bt_ag.force_server_mode = TRUE;
          }

          if ( bt_ag.event_reason != BT_EVR_AG_CONN_FAILED )
          {
            bt_ag_send_event( BT_EV_AG_CONNECTION_FAILED );
          }
          bt_ag_clear_pending_cmds();
        }
        else if ( bt_ag.event_reason != BT_EVR_AG_CONN_FAILED )
        {
          /* must be result of Connect command */
          bt_ag_send_event( BT_EV_AG_CONNECTION_FAILED );
        }

        if ( bt_ag.going_idle == TRUE )
        {
          bt_ag_close_sio();
        }
      }
      else /* doesn't match either stream ids */
      {
        BT_MSG_DEBUG( "BT AG: sidHS=0x%x sidHF=0x%x sid=0x%x",
                      bt_ag.sio_stream_id_hs, bt_ag.sio_stream_id_hf, 
                      stream_id);
        if ( ((stream_id | STREAM_CLOSING_B) == bt_ag.sio_stream_id_hs) ||
             ((stream_id | STREAM_CLOSING_B) == bt_ag.sio_stream_id_hf) )
        {
          /* lower layer reports error on the stream being closed;
          ** must be result of bt_ag_cancel_connect()
          */
          bt_ag_send_event( BT_EV_AG_CONNECTION_FAILED );
        }
        else
        {
          /* not even one of my streams; how did I get here? */
          BT_ERR( "BT AG: sid 0x%x not expected", bt_ag.state, 0, 0 );
        }
      }
      break;

    default:
      BT_ERR( "BT AG: SIO connect failed AG St %x", bt_ag.state, 0, 0 );
      break;
  }
}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_connected

DESCRIPTION
  Processes the SPP connected event received from SIO indicating RFCOMM link
  is up.

===========================================================================*/
LOCAL void bt_ag_process_spp_connected
(
  bt_spp_status_type*  ss_ptr
)
{
  
  boolean  keep_going = TRUE;

  bt_ag.sio_stream_id = ss_ptr->stream_id;

  if ( (bt_ag.disabling != FALSE) || (bt_ag.state == BT_AGS_CLOSING) )
  {
    sio_ioctl( bt_ag.sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
    return;
  }

  if ( AUDIO_DEV.bd_addr_ptr == NULL )  /*  Server, accept all BD_ADDR.  */
  {
    AUDIO_DEV.bd_addr = ss_ptr->bd_addr;
    AUDIO_DEV.server_channel =
      (bt_rc_scn_enum_type)(ss_ptr->rc_server_channel);
    bt_ag.service_class = ss_ptr->service_uuid;

    if ( ss_ptr->stream_id == bt_ag.sio_stream_id_hs )
    {
      AUDIO_DEV.device_type = BT_AD_HEADSET;
      bt_ag_close_hf_stream();
    }
    else if ( ss_ptr->stream_id == bt_ag.sio_stream_id_hf )
    {
      AUDIO_DEV.device_type = BT_AD_HANDSFREE;
      bt_ag_close_hs_stream();
    }
    else
    {
      BT_ERR( "BT AG: Bad SPP connected SID %x", ss_ptr->stream_id, 0, 0 );
      keep_going = FALSE;
    }
  }
  else if ( BT_BD_ADDRS_EQUAL( &ss_ptr->bd_addr,
                               &AUDIO_DEV.bd_addr ) == FALSE )
  {
    /*  Connected to wrong audio device.  */
    /*  Treat as if connection failed.    */
    BT_MSG_HIGH( "BT AG: SPP connected to wrong AD", 0, 0, 0 );
    BT_BDA( MSG_DEBUG, "BT AG: expected", &AUDIO_DEV.bd_addr );
    BT_BDA( MSG_DEBUG, "BT AG: connected", &ss_ptr->bd_addr );
    bt_ag.event_reason = BT_EVR_AG_CONN_FAILED; /* signal not to send ev */
    bt_ag_process_spp_connect_failed( ss_ptr->stream_id );
    keep_going = FALSE;
  }

  if ( keep_going &&
       ((bt_ag.state == BT_AGS_OPENING) || /*  Trying to connect to HS or    */
        (bt_ag.state == BT_AGS_OPEN)) )    /*  waiting for connect from HS?  */
  {
    bt_ag.state = BT_AGS_CONNECTED;
    bt_ag_send_event( BT_EV_AG_CONNECTED );

    switch ( AUDIO_DEV.device_type )
    {
      case BT_AD_HEADSET:
        if ( bt_ag.idle_timeout > 0 )
        {
          BT_MSG_DEBUG( "BT AG: process_spp_conn timer set %x",
                        bt_ag.idle_timeout, 0, 0 );
          rex_set_timer( &bt_ag_timer, bt_ag.idle_timeout );
        }
        bt_ag_perform_pending_cmds();
        break;

      case BT_AD_HANDSFREE:
        if ( !BT_AG_IN_CDMA &&
             (HF_DATA.cm_client_id == CM_CLIENT_TYPE_BLUETOOTH) )
        {
          /*  Request serving-system information.  */
          cm_ss_cmd_get_ss_info( bt_ag_cm_ss_cmd_cb, NULL, 
                                 HF_DATA.cm_client_id );
          cm_ph_cmd_get_ph_info( bt_ag_cm_ph_cmd_cb, NULL, 
                                 HF_DATA.cm_client_id );
        }
#ifdef FEATURE_BT_HFP_1_5
        bt_ag_query_hf_sdp_info();
#endif
        /*  Expecting a BRSF command from HF.  */
        HF_DATA.connection_state = BT_AG_HCS_BRSF;
        BT_MSG_DEBUG( "BT AG: process_spp_conn timer set %x",
                      BT_AG_SLC_SETUP_TO, 0, 0 );
        rex_set_timer( &bt_ag_timer, BT_AG_SLC_SETUP_TO );
        break;

      default:
        BT_ERR( "BT AG: SIO connected Bad AD %x",
                AUDIO_DEV.device_type, 0, 0 );
        break;
    }
  }
  else if ( keep_going )
  {
    BT_ERR( "BT AG: SIO connected AG St %x", bt_ag.state, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_disconnected

DESCRIPTION
  Processes the SPP disconnected event received from SIO indicating RFCOMM
  link is down.  Only client streams reach SPP disconnected state.

===========================================================================*/
LOCAL void bt_ag_process_spp_disconnected
(
  bt_event_reason_type  reason
)
{
  
  BT_MSG_API( "BT AG EV RX: SPP disc AG St %x Dis %x", 
              bt_ag.state, bt_ag.disabling, 0 );

  switch ( bt_ag.state )
  {
    case BT_AGS_DISABLED:
      /*  A result of bt_ag_cmd_disable().       */
      /*  Nothing to do; just ignore the event.  */
      break;

    case BT_AGS_AUDIO_DISCONNECTING:
    case BT_AGS_AUDIO_CONNECTED:
      /*  RFCOMM link suddenly went down. */
      bt_ag_sco_cleanup();
      /*  Falls through.  */

    case BT_AGS_AUDIO_CONNECTING:
    case BT_AGS_CONNECTED:
    case BT_AGS_CLOSING:
      bt_ag_send_event( BT_EV_AG_DISCONNECTED );
      if ( bt_ag.going_idle != FALSE )
      {
        /* do nothing now; wait for next SPP status update */
        break;
      }
      if ( bt_ag.disabling == FALSE )
      {
        if ( BT_BD_ADDRS_EQUAL( &bt_ag.enabled_bd_addr,
                                &null_bd_addr ) == FALSE )
        {
          AUDIO_DEV.bd_addr     = bt_ag.enabled_bd_addr;
          AUDIO_DEV.device_type = bt_ag.enabled_device_type;
          bt_ag.service_class   = bt_ag.enabled_service_class;
        }
        else
        {
          AUDIO_DEV.bd_addr_ptr = NULL;
        }

        /* AG shall not initiate paging after disconnections */
        bt_ag.force_server_mode = TRUE;
        bt_ag.going_idle        = TRUE;
      }
      bt_ag_close_sio();
      break;

    case BT_AGS_OPENING:
      /* connecting attempt was cancelled before connection is up */
      bt_ag_send_event( BT_EV_AG_CONNECTION_FAILED );
      /* BT_SPP_ST_CLOSED will be coming up */
      break;

    default:
      BT_ERR( "BT AG: SIO disc'd AG St %x", bt_ag.state, 0, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_open

DESCRIPTION
  Processes the SPP open event received from SIO.  Only server streams reach
  SPP open state.

===========================================================================*/
LOCAL void bt_ag_process_spp_open
(
  bt_event_reason_type  reason
)
{

  BT_MSG_API( "BT AG EV RX: SPP open AG St %x Dis %x", 
              bt_ag.state, bt_ag.disabling, 0 );

  if ( bt_ag.disabling != FALSE )
  {
    bt_ag_close_sio();
    bt_ag_send_event( BT_EV_AG_DISCONNECTED );
  }
  else if ( bt_ag.state >= BT_AGS_CONNECTED )
  {
    if ( bt_ag.state == BT_AGS_AUDIO_CONNECTED )
    {
      /*  RFCOMM link lost;  bring down SCO link if necessary.  */
      bt_ag_sco_cleanup();
    }

    if ( AUDIO_DEV.device_type == BT_AD_HANDSFREE )
    {
      bt_ag_hf_cleanup();
    }

    bt_ag.state   = BT_AGS_OPEN;
    bt_ag.is_idle = TRUE;
    bt_ag_send_event( BT_EV_AG_DISCONNECTED );
    rex_clr_timer( &bt_ag_timer );

    if ( AUDIO_DEV.bd_addr_ptr == NULL )
    {
      /*  Open the other server stream.  */
      if ( bt_ag.sio_stream_id_hf == SIO_NO_STREAM_ID )
      {
        if ( bt_ag_open_sio_server( 
               BT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY ) == 
                 SIO_NO_STREAM_ID )
        {
          BT_ERR( "BT AG: Failed to open HF stream", 0, 0, 0 );
        }
      }
      else
      {
        if ( bt_ag_open_sio_server(
               BT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY ) == 
                 SIO_NO_STREAM_ID )
        {
          BT_ERR( "BT AG: Failed to open HS stream", 0, 0, 0 );
        }
      }
    }
  }
  else
  {
    bt_ag.state = BT_AGS_OPEN;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_closed

DESCRIPTION
  Processes the SPP closed event received from SIO.

===========================================================================*/
LOCAL void bt_ag_process_spp_closed
(
  sio_stream_id_type  stream_id
)
{
  
  if ( bt_ag.sio_stream_id_hs == (stream_id | STREAM_CLOSING_B) )
  {
    bt_ag.sio_stream_id_hs = SIO_NO_STREAM_ID;
  }
  else if ( bt_ag.sio_stream_id_hf == (stream_id | STREAM_CLOSING_B) )
  {
    bt_ag.sio_stream_id_hf = SIO_NO_STREAM_ID;
  }
  else
  {
    BT_ERR( "BT AG: Bad SPP Closed SID %x", stream_id, 0, 0 );
    return;
  }

  if ( (bt_ag.sio_stream_id_hs == SIO_NO_STREAM_ID) &&
       (bt_ag.sio_stream_id_hf == SIO_NO_STREAM_ID) )
  {
    if ( bt_ag.disabling != FALSE )
    {
      bt_ag_disable();
    }
    else if ( bt_ag.going_idle != FALSE )
    {
      bt_ag.going_idle = FALSE;
      bt_ag_go_idle();
      bt_ag.force_server_mode = FALSE;  /*  Done with this flag.  */
    }
    else if ( (bt_ag.ring_count > 0) || /* trying to ring remote device? */
              (bt_ag.adding_audio != FALSE) ) /* or to bring up audio? */
    {
      /* connect to remote device*/
      if ( bt_ag_open_sio_client( AUDIO_DEV.bd_addr_ptr,
                                  AUDIO_DEV.service_uuid ) == SIO_NO_STREAM_ID )
      {
        bt_ag_clear_pending_cmds();
      }
    }
    else
    {
      bt_ag.state = BT_AGS_CLOSED;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_spp_event

DESCRIPTION
  Process SPP status event.

===========================================================================*/
LOCAL void bt_ag_process_spp_event
(
  bt_ag_spp_status_ev_type*  ev_ptr
)
{
  
  BT_MSG_API( "BT AG EV RX: SPP State %x AG St %x",
              ev_ptr->spp_status.spp_state, bt_ag.state, 0 );

  bt_ag.event_reason = ev_ptr->spp_status.spp_reason;

  switch ( ev_ptr->spp_status.spp_state )
  {
    case BT_SPP_ST_CONNECTED:
      BT_MSG_DEBUG( "BT AG: SPP Connected SID %x", 
                    ev_ptr->spp_status.stream_id, 0, 0 );
      bt_ag_process_spp_connected( &ev_ptr->spp_status );
      break;

    case BT_SPP_ST_OPEN_ERROR:
      BT_MSG_DEBUG( "BT AG: SPP Open Error SID %x R %x", 
                    ev_ptr->spp_status.stream_id, 
                    ev_ptr->spp_status.spp_reason, 0 );
      bt_ag_process_spp_connect_failed( ev_ptr->spp_status.stream_id );
      break;

    case BT_SPP_ST_CLOSED:
      BT_MSG_DEBUG( "BT AG: SPP Closed SID %x", 
                    ev_ptr->spp_status.stream_id, 0, 0 );
      bt_ag_process_spp_closed( ev_ptr->spp_status.stream_id );
      break;

    case BT_SPP_ST_DISCONNECTED:
      BT_MSG_DEBUG( "BT AG: SPP Disconnected SID %x R %x", 
                    ev_ptr->spp_status.stream_id, 
                    ev_ptr->spp_status.spp_reason, 0 );
      bt_ag_process_spp_disconnected( ev_ptr->spp_status.spp_reason );
      break;

    case BT_SPP_ST_OPEN:
      BT_MSG_DEBUG( "BT AG: SPP Open SID %x R %x", 
                    ev_ptr->spp_status.stream_id, 
                    ev_ptr->spp_status.spp_reason, 0 );
      bt_ag_process_spp_open( ev_ptr->spp_status.spp_reason );
      break;

    default:
      BT_MSG_DEBUG( "BT AG: Ignored SPP State %x",
                    ev_ptr->spp_status.spp_state, 0, 0 );
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_cancel_connect

DESCRIPTION
  Cancel an establishing outbound connection.

===========================================================================*/
LOCAL void bt_ag_cancel_connect
(
  void
)
{
  BT_MSG_HIGH( "BT AG: AG sio_stream_id_hf %x sio_stream_id_hs %x",
               bt_ag.sio_stream_id_hf, bt_ag.sio_stream_id_hs, 0 );

       /* to avoid that both are NULL stream id */  
  if ( (bt_ag.sio_stream_id_hf != bt_ag.sio_stream_id_hs) &&
       !IS_STREAM_CLOSING ( bt_ag.sio_stream_id_hf ) &&
       !IS_STREAM_CLOSING ( bt_ag.sio_stream_id_hs ) )
  {
    if ( bt_ag.is_sio_server == FALSE)
    {
      /* signal indicating not to send BT_EV_AG_CONNECTION_FAILED */
      bt_ag.event_reason = BT_EVR_AG_CONN_FAILED;
      /* I'm SIO client trying to connect to remote device. */
      /* doesn't matter which SID to pass in */
      bt_ag_process_spp_connect_failed( bt_ag.sio_stream_id_hf );
      /* After this, the STREAM_CLOSING_B of one or both of the
         stream IDs would have been set to indicate it's waiting
         for the CLOSED status.
      */
    }
    else
    {
      BT_ERR( "BT AG: Canceling opening server stream forbidden! AG St %x",
              bt_ag.state, 0, 0 );
    }
  }
}


/*===========================================================================

FUNCTION
  bt_ag_process_cm_call_event_sups

DESCRIPTION
  Processes CM call_sups event.
  - UMTS: force sending call hold state for active-held call swap.
  - 1X  : may need to update call status info here,
          due to network limitations.

===========================================================================*/
LOCAL bt_ag_call_held_state_type bt_ag_process_cm_call_event_sups
(
  cm_mm_call_info_s_type*  call_info_ptr
)
{

  bt_ag_call_held_state_type   cur_call_held_state = HF_DATA.call_held_state;

  BT_MSG_DEBUG( "BT AG: Sups - call inf, id=%x st=%d subT=%d", 
                call_info_ptr->call_id, call_info_ptr->call_state,
                call_info_ptr->mode_info.info.gw_cs_call.sups_type );
  BT_MSG_DEBUG( "BT AG: actCid=%x setup=%d held=%d", HF_DATA.call_id, 
                HF_DATA.call_setup_state, HF_DATA.call_held_state );

  if ( BT_AG_IN_CDMA )
  {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    BT_MSG_DEBUG( "BT AG: wait=%d ", 
                  ISBITSET( bt_ag.flags, CDMA_CW_B ), 0, 0 );

    if ( (call_info_ptr->call_id == CM_CALL_ID_INVALID) &&
         (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) &&
         ISBITSET( bt_ag.flags, CDMA_CW_B ) )
    {
      /* timer expire because waiting call has not been answered */
      HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
      CLRBIT( bt_ag.flags, CDMA_CW_B );
      bt_ag_call_info_table_update_waiting_call( TRUE );
      HF_DATA.num_calls--;
    }
    else if ( ISBITSET( bt_ag.flags, CDMA_CW_B ) &&
              (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) )
    {
      /* active call put on hold and waiting call answered */
      rex_clr_timer( &bt_ag_timer );
      CLRBIT( bt_ag.flags, CDMA_CW_B );
      HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
      HF_DATA.call_held_state  = BT_AG_CALL_HELD_ACTIVE;
      bt_ag_call_info_table_update_waiting_call( FALSE );
      bt_ag_call_info_table_update_callheld( call_info_ptr );
    }
    else if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_ACTIVE )
    {
      if ( HF_DATA.call_setup_state == BT_AG_CALL_SETUP_OUT )
      {
        /* this Flash merges the 2 calls */
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
        HF_DATA.call_held_state  = BT_AG_CALL_HELD_NONE;
      }
      else
      {
        /* active/held swap, force callheld update */
        cur_call_held_state = BT_AG_CALL_HELD_NONE;
        bt_ag_call_info_table_update_callheld( call_info_ptr );
      }
    }
    else if ( memcmp( call_info_ptr->num.buf,
                      HF_DATA.phone_num.buf, HF_DATA.phone_num.len ) != 0 )
    {
      if ( HF_DATA.num_calls == 1 )
      {
        /* must be the case of 3rd party call being an MO call */
        HF_DATA.call_held_state  = BT_AG_CALL_HELD_ACTIVE;
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_OUT;
        HF_DATA.num_calls++;
        /* no info on whether 3rd party will be reached... */
      }
      else /* there were two calls */
      {
        /* the 2nd call was ended in a conference call.
         * is this a generic network behavior?
         */
        bt_ag_call_info_table_update_2nd_ended();
        HF_DATA.num_calls--;
      }
    }
    else
    {
      /* single call swap */
      if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_NONE )
      {
        HF_DATA.call_held_state = BT_AG_CALL_HELD_TERMINATED;
      }
      else
      {
        HF_DATA.call_held_state = BT_AG_CALL_HELD_NONE;
      }
    }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
  }
  else if ( BT_AG_IN_GW )
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif  /* FEATURE_WCDMA || FEATURE_GSM */
  }
  return cur_call_held_state;

}


/*===========================================================================

FUNCTION
  bt_ag_process_cm_call_event_end

===========================================================================*/
LOCAL void bt_ag_process_cm_call_event_end
(
  bt_ag_cm_call_ev_type*  ev_ptr
)
{
  BT_MSG_DEBUG( "BT AG: call ended; reason=%d", 
                ev_ptr->call_info.end_status, 0, 0 );

  bt_ag_update_call_info_table( ev_ptr );

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  if ( BT_AG_IN_CDMA )
  {
    HF_DATA.num_calls = 0;

    bt_ag_call_info_table_cleanup();

    if ( ISBITSET( bt_ag.flags, CDMA_CW_B ) &&
         (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) )
    {
      rex_clr_timer( &bt_ag_timer );
      CLRBIT( bt_ag.flags, CDMA_CW_B );
    }
  }
  else
#endif /* EATURE_CDMA_800 || FEATURE_CDMA_1900 */
  if ( HF_DATA.num_calls > 0 )
  {
    HF_DATA.num_calls--;
  }

  if ( HF_DATA.num_calls == 0 )
  {
    BT_MSG_DEBUG( "BT AG: last call ended", 0, 0, 0 );

    HF_DATA.call_id          = CM_CALL_ID_INVALID;
    HF_DATA.incoming_call_id = CM_CALL_ID_INVALID;
    HF_DATA.call_active      = FALSE;
    HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
    HF_DATA.call_held_state  = BT_AG_CALL_HELD_NONE;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */
  }
  else /* HF_DATA.num_calls > 0 */
  {
    if ( HF_DATA.call_held_state == BT_AG_CALL_HELD_ACTIVE )
    {
      if ( HF_DATA.call_id != ev_ptr->call_info.call_id )
      {
        BT_MSG_DEBUG( "BT AG: held call ended", 0, 0, 0 );  
        HF_DATA.call_held_state = BT_AG_CALL_HELD_NONE;
   
        if ( HF_DATA.call_setup_state > BT_AG_CALL_SETUP_IN )
        {
          HF_DATA.call_active = FALSE;
        }
#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */     
      }
      else
      {
        BT_MSG_DEBUG( "BT AG: active call ended cS=%d",
                      HF_DATA.call_setup_state, 0, 0 );
        HF_DATA.call_held_state  = BT_AG_CALL_HELD_TERMINATED;
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
      }
    }
    /* else, BT_AG_CALL_HELD_NONE or BT_AG_CALL_HELD_TERMINATED */
    /* if it was a waiting + active/held case */
    else if ( HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN )
    {
      if ( HF_DATA.call_id != ev_ptr->call_info.call_id )
      {
        BT_MSG_DEBUG( "BT AG: waiting call ended", 0, 0, 0 );
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
      }
      else
      {
        BT_MSG_DEBUG( "BT AG: active or held call ended cH=%d",
                      HF_DATA.call_held_state, 0, 0 );
        HF_DATA.call_active     = FALSE;
        HF_DATA.call_id         = HF_DATA.incoming_call_id;
        HF_DATA.call_held_state = BT_AG_CALL_HELD_NONE;

        // we can't pass PTS test if we run the following statement!
        /* force call_setup update; some HFs need this to
        ** know the waiting call is still alerting.
        
        cur_call_setup_state = BT_AG_CALL_SETUP_NONE;
        */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */
      }
    }
    /* else, it was a conference call or held + held case */
    else
    {
      BT_MSG_DEBUG( "BT AG: active or held call ended cH=%d",
                    HF_DATA.call_held_state, 0, 0 );
    }

#ifndef FEATURE_BT_WITH_OLD_CM
    /* in order to update HF_DATA.call_id */
    if ( cm_mm_call_cmd_get_call_info_list( bt_ag_cm_call_cmd_cb, NULL, 
                                            HF_DATA.cm_client_id ) == FALSE )
    {
      BT_ERR( "BT AG: failed to get info for calls", 0, 0, 0 );
    }
#endif /* ! FEATURE_BT_WITH_OLD_CM */
  }
}


/*===========================================================================

FUNCTION
  bt_ag_process_cm_call_event

DESCRIPTION
  Processes CM call event.

===========================================================================*/
LOCAL void bt_ag_process_cm_call_event
(
  bt_ag_cm_call_ev_type*  ev_ptr
)
{

  bt_ag_call_setup_state_type  cur_call_setup_state =
                                 HF_DATA.call_setup_state;
  bt_ag_call_held_state_type   cur_call_held_state  = HF_DATA.call_held_state;
  boolean                      cur_call_active      = HF_DATA.call_active;
  boolean                      mo_call_connected    = FALSE;

  if ( (ev_ptr->call_info.call_type != CM_CALL_TYPE_VOICE) &&
       (ev_ptr->call_info.call_type != CM_CALL_TYPE_CS_DATA) &&
       (ev_ptr->call_info.call_type != CM_CALL_TYPE_EMERGENCY) &&
       (ev_ptr->call_info.call_type != CM_CALL_TYPE_STD_OTASP) &&
       (ev_ptr->call_info.call_type != CM_CALL_TYPE_NON_STD_OTASP) &&
       (ev_ptr->call_event != CM_CALL_EVENT_MNG_CALLS_CONF)
     )
  {
    return; /* only interested in voice call */
  }

  BT_MSG_API( "BT AG EV RX: CM Call EV|T=0x%04x ID=%x numLen=%d", 
              ((uint16)(ev_ptr->call_event << 8) | ev_ptr->call_info.call_type),
              ev_ptr->call_info.call_id, ev_ptr->call_info.num.len );
  BT_MSG_DEBUG( "BT AG: num calls=%d", HF_DATA.num_calls, 0, 0 );

  /* CM call event usually comes with a phone number, 
     make sure it's NULL-terminated before using it. */
  ev_ptr->call_info.num.buf[ ev_ptr->call_info.num.len ] = NULL;

  switch ( ev_ptr->call_event )
  {
    case CM_CALL_EVENT_INFO:
    {
      bt_ag_update_call_info_table( ev_ptr );

      switch ( ev_ptr->call_info.call_state )
      {
        case CM_CALL_STATE_CONV:
#ifdef FEATURE_BT_WITH_OLD_CM
          HF_DATA.call_active      = TRUE;
          HF_DATA.call_id          = ev_ptr->call_info.call_id;
          HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
          HF_DATA.num_calls++;
#endif
          break;

        case CM_CALL_STATE_ORIG:
          HF_DATA.phone_num        = ev_ptr->call_info.num;
#ifdef FEATURE_BT_WITH_OLD_CM
          HF_DATA.call_setup_state = BT_AG_CALL_SETUP_OUT;
          HF_DATA.num_calls++;
          if ( HF_DATA.num_calls > 1 )
          {
            HF_DATA.call_held_state = BT_AG_CALL_HELD_ACTIVE;
          }
#endif
          break;

        case CM_CALL_STATE_INCOM:
          HF_DATA.phone_num        = ev_ptr->call_info.num;
#ifdef FEATURE_BT_WITH_OLD_CM
          HF_DATA.call_id          = ev_ptr->call_info.call_id;
          HF_DATA.call_setup_state = BT_AG_CALL_SETUP_IN;
          HF_DATA.num_calls++;
#endif
          break;

        default:
          BT_MSG_DEBUG( "BT AG: Ignoring call state %x", 
                        ev_ptr->call_info.call_state, 0, 0 );
          break;
      }
      break;
    }

    case CM_CALL_EVENT_ORIG:    /*  Phone originated a call.  */
      bt_ag_update_call_info_table( ev_ptr );
      HF_DATA.phone_num        = ev_ptr->call_info.num;
      HF_DATA.call_setup_state = BT_AG_CALL_SETUP_OUT;
      if ( HF_DATA.call_id != ev_ptr->call_info.call_id )
      {
        HF_DATA.call_id = ev_ptr->call_info.call_id;
        HF_DATA.num_calls++;
      }
      if ( HF_DATA.num_calls > 1 )
      {
        HF_DATA.call_held_state = BT_AG_CALL_HELD_ACTIVE;
      }
      break;

    case CM_CALL_EVENT_ANSWER:  /*  Incoming call was answered.  */
      /* call indicators are updated in following
      ** CM_CALL_EVENT_CONNECT procssing.
      */
      break;

    case CM_CALL_EVENT_END:     /*  Originated/incoming call ended.  */
    {
      bt_ag_process_cm_call_event_end( ev_ptr );
      break;
    }
    case CM_CALL_EVENT_INCOM:   /*  Phone received an incoming call.  */
    {
      bt_ag_update_call_info_table( ev_ptr );
      HF_DATA.phone_num        = ev_ptr->call_info.num;
      HF_DATA.call_setup_state = BT_AG_CALL_SETUP_IN;
      if ( (HF_DATA.incoming_call_id != ev_ptr->call_info.call_id) && 
           (HF_DATA.call_id != ev_ptr->call_info.call_id) )
      {
        HF_DATA.incoming_call_id = ev_ptr->call_info.call_id;
        HF_DATA.num_calls++;
      }

      if ( (HF_DATA.call_active != FALSE) &&
           (HF_DATA.notify_call_waiting != FALSE) )
      {
        BT_MSG_DEBUG( "BT AG: A GW call is waiting", 0, 0, 0 );
        bt_ag_cpe_send_response( BT_AG_RESP_CCWA, 0 );
      }
      else if ( HF_DATA.call_active == FALSE )
      {
        /* only update call ID if not a waiting call */
        HF_DATA.call_id = ev_ptr->call_info.call_id;
      }
      break;
    }
    case CM_CALL_EVENT_CONNECT: /*  Originated/incoming call connected.  */
    {
      bt_ag_update_call_info_table( ev_ptr );

      if ( (ev_ptr->call_info.mode_info.info_type == CM_CALL_MODE_INFO_CDMA) &&
           (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_OUT) )
      {
        mo_call_connected = TRUE;
      }

      if ( HF_DATA.num_calls > 1 )
      {
        HF_DATA.call_held_state = BT_AG_CALL_HELD_ACTIVE;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */

        /* force update of call_active for MO call */
        if ( HF_DATA.call_setup_state ==
               BT_AG_CALL_SETUP_REMOTE_PARTY_ALERTED )
        {
          cur_call_active = FALSE;
        }
      }

      HF_DATA.incoming_call_id = CM_CALL_ID_INVALID;
      HF_DATA.call_id          = ev_ptr->call_info.call_id;
      HF_DATA.call_active      = TRUE;
      HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
      break;
    }
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    case CM_CALL_EVENT_CALLER_ID: /* CDMA only */
    {
      HF_DATA.phone_num = ev_ptr->call_info.num;

      if ( HF_DATA.call_active != FALSE )
      {
        /* this is call waiting notification */

        BT_MSG_DEBUG( "BT AG: A CDMA call is waiting", 0, 0, 0 );

        /* we fake the call info here for the waiting call
         * in order to keep it in the call info table.
         */
        ev_ptr->call_event           = CM_CALL_EVENT_INCOM;
        ev_ptr->call_info.call_id    =
          ++ev_ptr->call_info.call_id % CM_CALL_ID_MAX;
        ev_ptr->call_info.call_state = CM_CALL_STATE_INCOM;
        HF_DATA.num_calls++;
        bt_ag_update_call_info_table( ev_ptr );
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_IN;
        SETBIT( bt_ag.flags, CDMA_CW_B );

        if ( HF_DATA.notify_call_waiting != FALSE )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CCWA, 0 );
          rex_set_timer( &bt_ag_timer, BT_AG_1X_CW_TIMEOUT );
        }
      }
      else /* else, it's the only single call case */
      {
        /* CDMA MT caller ID becomes available once this event received */
        bt_ag_update_call_info_table( ev_ptr );
      }
      break;
    }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
    case CM_CALL_EVENT_SUPS:
    {
      cur_call_held_state = 
        bt_ag_process_cm_call_event_sups( &ev_ptr->call_info );

      if ( BT_AG_IN_CDMA &&
           (HF_DATA.num_calls > 1) )
      {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
        if ( (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_NONE) &&
             (HF_DATA.call_held_state  == BT_AG_CALL_HELD_NONE) )
        {
          /* not to send callsetup */
          cur_call_setup_state = BT_AG_CALL_SETUP_NONE;
        }
        else if ( HF_DATA.call_setup_state == BT_AG_CALL_SETUP_OUT )
        {
          /* an MO call was made to form a 3-way calling scenario */

          /* we fake the call info here for the MO call
           * in order to keep it in the call info table.
           */
          mo_call_connected            = TRUE;
          ev_ptr->call_event           = CM_CALL_EVENT_CONNECT;
          ev_ptr->call_info.call_state = CM_CALL_STATE_CONV;

          bt_ag_update_call_info_table( ev_ptr );
        }
#endif /* FEATURE_CDMA_800 || FEATURE_CDMA_1900 */
      }
      break;
    }
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif  /* FEATURE_WCDMA || FEATURE_GSM */

    default:
      /*  Ignore other events.  */
      break;
  }

  /*  If Indicator Status Update function is enabled,  */
  /*  forward to HF new indicator event status.        */
  if ( (HF_DATA.ier_mode == BT_AIER_FWD) && 
       (bt_ag.state >= BT_AGS_CONNECTED) && (bt_ag.state != BT_AGS_CLOSING) )
  {
    /* CDMA call connected? */
    if ( mo_call_connected != FALSE )
    {
      if ( HF_DATA.num_calls == 1 )
      {
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_REMOTE_PARTY_ALERTED;
        bt_ag_cpe_send_response( BT_AG_RESP_CIEV, BT_AG_INDICATOR_CALL_SETUP );
        bt_ag_cpe_send_response( BT_AG_RESP_CIEV, BT_AG_INDICATOR_CALL );
        HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
        bt_ag_cpe_send_response( BT_AG_RESP_CIEV, BT_AG_INDICATOR_CALL_SETUP );
      }
      else
      {
        /* hide the 2nd MO call info from the remote,
         * but callheld needs to be sent - strange, but
         * required by some customer.
         */
        if ( HF_DATA.features_bitmap & BT_HF_F_ENH_CALL_STATUS )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CIEV,
                                   BT_AG_INDICATOR_CALL_HELD );
        }
      }
    }
    else
    {
      /* change in call state? */
      if ( cur_call_active != HF_DATA.call_active )
      {
        bt_ag_cpe_send_response( BT_AG_RESP_CIEV, 
                                 BT_AG_INDICATOR_CALL );
      }

      /* change in call setup state? */
      if ( cur_call_setup_state != HF_DATA.call_setup_state )
      {
        bt_ag_cpe_send_response( BT_AG_RESP_CIEV, 
                                 BT_AG_INDICATOR_CALL_SETUP );
      }

#ifdef FEATURE_BT_HFP_1_5
      /* Call_held_state indicator change shall be sent only if:
      ** - headset supports Enhanced Call Status feature
      ** - change in call held state
      ** - no CHLD_B and CHLD_1_AC_HD_B set (no AT+CHLD=1/1x being processed)
      */
      if ( (HF_DATA.features_bitmap & BT_HF_F_ENH_CALL_STATUS) &&
           (cur_call_held_state != HF_DATA.call_held_state) )
      {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif  /* FEATURE_WCDMA || FEATURE_GSM */
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CIEV, 
                                   BT_AG_INDICATOR_CALL_HELD );
        }
      }
#endif /* FEATURE_BT_HFP_1_5 */
    }
  }

}


#ifndef FEATURE_BT_WITH_OLD_CM
/*===========================================================================

FUNCTION
  bt_ag_process_cm_call_info_list

DESCRIPTION
  Processes CM call infomation list.

===========================================================================*/
LOCAL void bt_ag_process_cm_call_info_list( void )
{
  uint8 i;
  uint8 num_active_calls                = 0;
  uint8 num_orig_calls                  = 0;
  uint8 num_incom_calls                 = 0;
  cm_call_id_type held_call_id          = CM_CALL_ID_INVALID;
  cm_call_state_info_list_s_type* l_ptr = &HF_DATA.call_info_list;

  HF_DATA.num_calls        = 0;
  HF_DATA.call_id          = CM_CALL_ID_INVALID;
  HF_DATA.incoming_call_id = CM_CALL_ID_INVALID;

  for ( i=0; i<l_ptr->number_of_active_calls; i++ )
  {
    /* only interested in voice calls */
    if ( IS_VOICE_CALL( l_ptr->info[ i ].call_type ) &&
         (l_ptr->info[ i ].call_state > CM_CALL_STATE_IDLE) )
    {
      HF_DATA.num_calls++;
      switch ( l_ptr->info[ i ].call_state )
      {
        case CM_CALL_STATE_CONV:
          num_active_calls++;
          /* if it's a held call */
          if ( l_ptr->info[ i ].call_subst.conv == 
               CM_CALL_CONV_CALL_ON_HOLD )
          {
            held_call_id = l_ptr->info[ i ].call_id;
          }
          else if ( HF_DATA.call_id == CM_CALL_ID_INVALID )
          {
            /* no MO call info acquired yet */
            HF_DATA.call_id = l_ptr->info[ i ].call_id;
          }
          break;
        case CM_CALL_STATE_ORIG:
          num_orig_calls++;
          /* MO call has the highest priority to seize HF_DATA.call_id */
          HF_DATA.call_id = l_ptr->info[ i ].call_id;
          break;
        case CM_CALL_STATE_INCOM:
          num_incom_calls++;
          HF_DATA.incoming_call_id = l_ptr->info[ i ].call_id;          
          break;
      }
      BT_MSG_API( "BT AG CMD TX: Get Call Info id=%d st=%d", 
                  l_ptr->info[ i ].call_id, l_ptr->info[ i ].call_state, 0 );
      cm_mm_call_cmd_get_call_info( NULL, NULL,
                                    HF_DATA.cm_client_id,
                                    l_ptr->info[ i ].call_id );
    }
  }

  BT_MSG_DEBUG( "BT AG: call info list - a=%d o=%d i=%d", 
                num_active_calls, num_orig_calls, num_incom_calls );

  /* Update call state */
  if ( num_active_calls > 0 )
  {
    HF_DATA.call_active = TRUE;
  }

  /* Update call setup state */
  if ( num_orig_calls > 0 )
  {
    HF_DATA.call_setup_state = BT_AG_CALL_SETUP_OUT;
  }
  else if ( num_incom_calls > 0 )
  {
    HF_DATA.call_setup_state = BT_AG_CALL_SETUP_IN;
  }
  else
  {
    HF_DATA.call_setup_state = BT_AG_CALL_SETUP_NONE;
  }

  /* Update current call id & call hold state */
  if ( HF_DATA.call_id == CM_CALL_ID_INVALID )
  {
    if ( held_call_id != CM_CALL_ID_INVALID )
    {
      /* held (and incoming) call only */
      HF_DATA.call_id         = held_call_id;
      HF_DATA.call_held_state = BT_AG_CALL_HELD_TERMINATED;
    }
    else if ( HF_DATA.incoming_call_id != CM_CALL_ID_INVALID )
    {
      /* there's only an incoming call */
      HF_DATA.call_id = HF_DATA.incoming_call_id;
    }
  }
  else if ( held_call_id != CM_CALL_ID_INVALID )
  {
    /* it's an MO/active + held case */
    HF_DATA.call_held_state = BT_AG_CALL_HELD_ACTIVE;
  }
  
  BT_MSG_HIGH( "BT AG: call info - cS=%d cH=%d id=%x", 
               HF_DATA.call_setup_state,
               HF_DATA.call_held_state,
               HF_DATA.call_id );

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA) || FEATURE_GSM */
}
#endif /* ! FEATURE_BT_WITH_OLD_CM */


/*===========================================================================

FUNCTION
  bt_ag_process_cm_ph_event

DESCRIPTION
  Processes CM phone event.

===========================================================================*/
LOCAL void bt_ag_process_cm_ph_event
(
  bt_ag_cm_ph_ev_type*  ev_ptr
)
{

  BT_MSG_API( "BT AG EV RX: Phone Event %x", ev_ptr->ph_event, 0, 0 );

  if ( ev_ptr->ph_event == CM_PH_EVENT_INFO )
  {
    if ( ev_ptr->ph_info.is_in_use )
    {
      BT_MSG_DEBUG( "BT AG: Getting call info", 0, 0, 0 );

#ifndef FEATURE_BT_WITH_OLD_CM
      if ( cm_mm_call_cmd_get_call_info_list( bt_ag_cm_call_cmd_cb, NULL, 
                                              HF_DATA.cm_client_id ) == FALSE )
      {
        BT_ERR( "BT AG: failed to get info for calls", 0, 0, 0 );
      }
#else
      {
        cm_call_id_type i;
        for ( i=0; i < CM_CALL_ID_MAX; i++ )
        {
          cm_mm_call_cmd_get_call_info( NULL, NULL,
                                        HF_DATA.cm_client_id,
                                        (cm_call_id_type) i );
        }
      }
#endif /* FEATURE_BT_WITH_OLD_CM */
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_cm_ss_event

DESCRIPTION
  Processes CM serving-system event.

===========================================================================*/
LOCAL void bt_ag_process_cm_ss_event
(
  bt_ag_cm_ss_ev_type*  ev_ptr
)
{
  cm_mm_ss_info_s_type*  ssip              = &ev_ptr->ss_info;
  boolean                cur_service_avail = HF_DATA.service_avail;
#ifdef FEATURE_BT_HFP_1_5
  uint8                  cur_roam          = HF_DATA.roam;
  bt_cmd_msg_type        rssi_msg;
#endif /* FEATURE_BT_HFP_1_5 */

  BT_MSG_API( "BT AG EV RX: SS Event %x", ev_ptr->ss_event, 0, 0 );
  
  switch ( ev_ptr->ss_event )
  {
    case CM_SS_EVENT_INFO:
      /*  This event only sent after request for SS info.  */
      AUDIO_DEV.hf_data.ss_info = *ssip;
      if ( (ssip->srv_status == SYS_SRV_STATUS_SRV)  &&
           (ssip->sys_mode   != SYS_SYS_MODE_NO_SRV) &&
           (ssip->srv_domain != SYS_SRV_DOMAIN_NO_SRV) )
      {
        HF_DATA.service_avail = TRUE;
      }
      else
      {
        HF_DATA.service_avail = FALSE;
      }
      break;

#ifdef FEATURE_BT_HFP_1_5
    case CM_SS_EVENT_RSSI:
      if ( ssip->rssi < 60 )
      { 
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_5;
      }
      else if ( ssip->rssi < 75 )
      {
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_4;
      }
      else if ( ssip->rssi < 90 )
      {
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_3;
      }
      else if ( ssip->rssi < 105 )
      {
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_2;
      }
      else if ( ssip->rssi < 120 )
      {
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_1;
      }
      else
      {
        rssi_msg.cmd_msg.cmd_ag_setrs.rssi = BT_AG_RSSI_LEVEL_0;
      }

      BT_MSG_API( "BT AG: Internally Set RSSI %x",
                  rssi_msg.cmd_msg.cmd_ag_setrs.rssi, 0, 0 );

      rssi_msg.cmd_hdr.bt_app_id = 0; /* arbitrary app_id */

      bt_ag_cmd_set_rssi( &rssi_msg );
      break;
#endif /* FEATURE_BT_HFP_1_5 */

    case CM_SS_EVENT_SRV_CHANGED:
      BT_MSG_DEBUG( "BT AG: SS Event change fields %x",
                    ssip->changed_fields, 0, 0 );

      if ( ssip->changed_fields & CM_SS_SRV_IND_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Srv Status %x", ssip->srv_status, 0, 0 );
        HF_DATA.service_avail = (ssip->srv_status == SYS_SRV_STATUS_SRV);
      }

      if ( ssip->changed_fields & CM_SS_SRV_STATUS_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Srv Status %x", ssip->srv_status, 0, 0 );
        HF_DATA.ss_info.srv_status = ssip->srv_status;
        HF_DATA.service_avail = (ssip->srv_status == SYS_SRV_STATUS_SRV);
      }

      if ( ssip->changed_fields & CM_SS_SYS_MODE_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Sys mode %x", ssip->sys_mode, 0, 0 );
        HF_DATA.ss_info.sys_mode = ssip->sys_mode;
      }

      if ( ssip->changed_fields & CM_SS_SRV_DOMAIN_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Srv domain %x", ssip->srv_domain, 0, 0 );
        HF_DATA.ss_info.srv_domain = ssip->srv_domain;
      }

      if ( ssip->changed_fields & CM_SS_SRV_CAPABILITY_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Srv Cap %x", ssip->srv_capability, 0, 0 );
        HF_DATA.ss_info.srv_capability = ssip->srv_capability;
      }

      if ( ssip->changed_fields & CM_SS_ROAM_STATUS_MASK )
      {
        BT_MSG_DEBUG( "BT AG: Roam status %x", ssip->roam_status, 0, 0 );
        HF_DATA.ss_info.roam_status = ssip->roam_status;
        if ( ssip->roam_status == SYS_ROAM_STATUS_OFF )
        {
          HF_DATA.roam = BT_AG_ROAM_0;
        }
#ifdef BT_AG_HF_EXTENSION
        else if ( ssip->roam_status != SYS_ROAM_STATUS_ON )
        {
          HF_DATA.roam = BT_AG_ROAM_2;
        }
#endif
        else
        {
          HF_DATA.roam = BT_AG_ROAM_1;
        }
      }

      /*  If Indicator Status Update function is enabled,  */
      /*  forward to HF new indicator event status.        */
      if ( (HF_DATA.ier_mode == BT_AIER_FWD) &&
           (bt_ag.state >= BT_AGS_CONNECTED) && 
           (bt_ag.state != BT_AGS_CLOSING) )
      {
        if ( cur_service_avail != HF_DATA.service_avail )
        {
          bt_ag_cpe_send_response( BT_AG_RESP_CIEV,
                                   BT_AG_INDICATOR_SERVICE );
        }
#ifdef FEATURE_BT_HFP_1_5
        /* Indicator change shall be sent only if headset supports HFP 1.5 */
        if ( HF_DATA.profile_version >= HFP_VER_1_2 )
        {
          if ( cur_roam != HF_DATA.roam )
          {
            bt_ag_cpe_send_response( BT_AG_RESP_CIEV,
                                     BT_AG_INDICATOR_ROAM );
          }
        }
#endif /* FEATURE_BT_HFP_1_5 */
      }
      break;

    default:
      /*  Ignore all other SS events. */
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_cm_inband_event

DESCRIPTION
  Processes CM inband event.

===========================================================================*/
LOCAL void bt_ag_process_cm_inband_event
(
  bt_ag_cm_inband_ev_type*  ev_ptr
)
{

  BT_MSG_API( "BT AG EV RX: Inband event %x", ev_ptr->inband_event, 0, 0 );

  switch ( ev_ptr->inband_event )
  {
    case CM_INBAND_EVENT_REV_BURST_DTMF:
      /*  Send a burst DTMF.  */
      BT_MSG_DEBUG( "BT AG: Inband Burst DTMF C %x D %c",
                    ev_ptr->inband_info.cnt,
                    ev_ptr->inband_info.digits[0], 0 );
      /*  BT_TBD - what else to do?  */
      break;

    case CM_INBAND_EVENT_REV_START_CONT_DTMF:
      if ( HF_DATA.dtmf_started != FALSE )
      {
        /*  Start a continuous DTMF tone.  */
        BT_MSG_DEBUG( "BT AG: Cont DTMF set timer %x", 95, 0, 0 );
        rex_set_timer( &bt_ag_timer, 95 );
      }
      break;

    case CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF:
      /*  WCDMA/GSM only: confirm whether a  */
      /*  continuous DTMF tone is played.    */
      /*  Ignored.                           */
      break;

    case CM_INBAND_EVENT_REV_STOP_CONT_DTMF:
      /*  Stop a continuous DTMF tone.  */
      break;

    case CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF:
      /*  WCDMA/GSM only: confirm a continuous  */
      /*  DTMF tone is stopped.                 */
      /*  Ignored.                              */
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_current_supported_features

DESCRIPTION
  Returns AG's features bitmap (not to be confused with HF's).

===========================================================================*/
uint32 bt_ag_current_supported_features
(
  void
)
{
  
  return ( bt_ag.features_bitmap );
}


/*===========================================================================

FUNCTION
  bt_ag_tx_low_wm_cb

DESCRIPTION
  This callback function is called when an AG TX low watermark occurs.

===========================================================================*/
LOCAL void bt_ag_tx_low_wm_cb
(
  void
)
{

  BT_MSG_DEBUG( "BT AG: AG TX lo WM - enabled=%d", bt_ag.tx_wm_enabled, 0, 0 );
  bt_ag.tx_wm_enabled = TRUE;
  bt_ag_send_data();

}


/*===========================================================================

FUNCTION
  bt_ag_tx_high_wm_cb

DESCRIPTION
  This callback function is called when an AG TX high watermark occurs.

===========================================================================*/
LOCAL void bt_ag_tx_high_wm_cb
(
  void
)
{

  BT_MSG_DEBUG( "BT AG: AG TX hi WM - enabled=%d", bt_ag.tx_wm_enabled, 0, 0 );
  bt_ag.tx_wm_enabled = FALSE;
  
}


/*===========================================================================

FUNCTION
  bt_ag_powerup_init

DESCRIPTION
  Initializes the Audio Gateway Module upon powerup.

===========================================================================*/
void bt_ag_powerup_init
(
  void
)
{
  
  uint8  i;
  uint32 dne;

  BT_MSG_HIGH( "BT AG: Powerup init", 0, 0, 0 );

  /*  Initialize event queue.  */
  (void) q_init( &bt_ag.ev_q );
  bt_ag.ev_q_info.event_q_ptr      = &bt_ag.ev_q;
  bt_ag.ev_q_info.max_q_depth      = BT_AG_MAX_NUM_BT_EVENTS;
  bt_ag.ev_q_info.event_q_bit_mask = BT_EVQ_AG_B;
  bt_ag.ev_q_info.max_event_bytes  = bt_ag_get_max_event_bytes();

  /*  Set up TX watermark.  */
  dne = 1000;
  dsm_queue_init( &bt_ag.tx_wm, dne, &bt_ag.to_sio_q );
  bt_ag.tx_wm.lo_watermark        = dne / 10;
  bt_ag.tx_wm.hi_watermark        = bt_ag.tx_wm.lo_watermark * 9;
  bt_ag.tx_wm.lowater_func_ptr    = bt_ag_tx_low_wm_cb;
  bt_ag.tx_wm.hiwater_func_ptr    = bt_ag_tx_high_wm_cb;
  bt_ag.tx_wm_enabled             = TRUE;
  bt_ag.tx_dsm_ptr                = NULL;
  bt_ag.cur_dsm_ptr_ptr           = &bt_ag.tx_dsm_ptr;

  /*  Set up RX watermark.  */
  dne = 500;
  dsm_queue_init( &bt_ag.rx_wm, dne, &bt_ag.from_sio_q );
  bt_ag.rx_wm.lo_watermark        = dne / 10;
  bt_ag.rx_wm.hi_watermark        = bt_ag.rx_wm.lo_watermark * 9;
  bt_ag.rx_wm.non_empty_func_ptr  = bt_ag_every_rx_data_cb;

  /*  Initialize AG state variables.  */
  bt_ag.idle_mode        = BT_AGIM_DEFAULT_IDLE_MODE;
  bt_ag.idle_timeout     = BT_AG_DEFAULT_IDLE_TIMEOUT * 1000;

  bt_ag.ss_timer_ms_left = 0;

  bt_ag.sco_handle       = BT_AG_INVALID_SCO_HANDLE;
  bt_ag_sco_cleanup();
  bt_ag_hf_cleanup();
  bt_ag_call_info_table_cleanup();

  q_init( &bt_ag.cb_ev_q );
  q_init( &bt_ag.cb_ev_free_q );
  for ( i = 0; i < BT_AG_MAX_NUM_BT_EVENTS; i++ )
  {
    q_put( &bt_ag.cb_ev_free_q, q_link( &bt_ag.cb_ev_pool[ i ],
                                        &bt_ag.cb_ev_pool[ i ].hdr.link ) );
  }

  bt_ag_cleanup();

  /* must go after bt_ag_cleanup() due to dependency on AG's state */
  bt_ag_hf_cleanup();
  
  bt_ag_initialize_external_cmd_io();

  bt_ag.app_id_ag =
    bt_cmd_ec_get_app_id_and_register( bt_ag_store_bt_event );

  /*  Define idle timer.  */
  timer_def( &bt_ag_timer, &bt_always_on_timer_group, &bt_tcb, 
             BT_AG_TIMER_SIG, NULL, 0 );

#ifdef BT_SWDEV_AG_EXTAPP_TEST
  /*  Initialize for data received from HF.  */
  dsm_queue_init( &extapp_from_hf_wm, 1000, &extapp_from_hf_q );
  extapp_from_hf_wm.non_empty_func_ptr  = extapp_rx_hf_data_cb;

  /*  Initialize for data transmitted to HF.  */
  dsm_queue_init( &extapp_to_hf_wm, 1000, &extapp_from_hf_q );

  if ( bt_cmd_ag_external_io_config(
         bt_ag.app_id_ag, TRUE, &extapp_from_hf_wm,
         &extapp_to_hf_wm, BT_AG_IC_CKPD_B ) != BT_CS_GN_PENDING )
  {
    BT_ERR( "BT AG: Bad Ext I/O Setup.", 0, 0, 0 );
  }
#endif /* BT_SWDEV_AG_EXTAPP_TEST */

}


/*===========================================================================

FUNCTION
  bt_ag_qdsp_ready_init

DESCRIPTION
  Initialize the Audio Gateway Module when the Bluetooth QDSP image is loaded

===========================================================================*/
void bt_ag_qdsp_ready_init
(
  void
)
{

  /*  Nothing to do.  */

}


/*===========================================================================

FUNCTION
  bt_ag_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the Audio Gateway module when the Bluetooth QDSP image is
  unloaded

===========================================================================*/
void bt_ag_qdsp_reset_shutdown
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: bt_ag_qdsp_reset_shutdown()", 0, 0, 0 );

  /*  
  rex_clr_timer( &bt_ag_timer );
  rex_clr_sigs( &bt_tcb, BT_AG_TIMER_SIG );
  */

}

/*===========================================================================

FUNCTION
  bt_ag_soc_reset_shutdown

DESCRIPTION
  De-initialize the Audio Gateway module when the Bluetooth SOC is shutdown

===========================================================================*/
void bt_ag_soc_reset_shutdown
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: bt_ag_soc_reset_shutdown()", 0, 0, 0 );
 
  rex_clr_timer( &bt_ag_timer );
  rex_clr_sigs( &bt_tcb, BT_AG_TIMER_SIG );

}


/*===========================================================================

FUNCTION
  bt_ag_timer_expired

DESCRIPTION
  Called everytime AG timer expired.

===========================================================================*/
void bt_ag_timer_expired
(
  void
)
{
  
  BT_MSG_DEBUG( "BT AG: Timer expired St %x Rings %x",
                bt_ag.state, bt_ag.ring_count, 0 );

  if ( HF_DATA.dtmf_started )
  {
    HF_DATA.dtmf_started = FALSE;
    cm_mm_inband_cmd_stop_cont_dtmf( bt_ag_cm_inband_cmd_cb, NULL,
                                     HF_DATA.cm_client_id,
                                     HF_DATA.call_id,
                                     0 ); /*  Duration not used.  */
    bt_ag_cpe_send_response( BT_AG_RESP_OK, 0 );
  }
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  else if ( ISBITSET( bt_ag.flags, CDMA_CW_B ) && 
            (HF_DATA.call_setup_state == BT_AG_CALL_SETUP_IN) )
  {
    /* assume wating call has ended */
    bt_ag_ev_type* ev_ptr;
    BT_MSG_DEBUG( "BT AG: faking CM_CALL_EVENT_SUPS", 0, 0, 0 );
    if ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_free_q )) != NULL )
    {
      ev_ptr->hdr.ev_type                 = BT_AG_CM_EV_CALL;
      ev_ptr->call_ev.call_event          = CM_CALL_EVENT_SUPS;
      ev_ptr->call_ev.call_info.call_type = CM_CALL_TYPE_VOICE;
      ev_ptr->call_ev.call_info.call_id   = CM_CALL_ID_INVALID;
      ev_ptr->call_ev.call_info.num       = HF_DATA.phone_num;
      q_put( &bt_ag.cb_ev_q, &ev_ptr->hdr.link );
      rex_set_sigs( &bt_tcb, BT_AG_RX_DATA_SIG );
    }
    else
    {
      BT_ERR( "BT AG: cb_ev_free_q empty", 0, 0, 0 );
    }
  }
#endif
  else if ( bt_ag.send_audio_conn_ev )
  {
    bt_ag.send_audio_conn_ev = FALSE;
    bt_ag_send_event( BT_EV_AG_AUDIO_CONNECTED );
  }
  else if ( (bt_ag.state == BT_AGS_CONNECTED) &&
            (AUDIO_DEV.device_type == BT_AD_HANDSFREE) && 
            (HF_DATA.connection_state < BT_AG_HCS_DONE) )
  {
    BT_ERR( "BT AG: SLC set up timeout Conn St %x",
            HF_DATA.connection_state, 0, 0 );
    bt_ag.state = BT_AGS_CLOSING; // force server mode 
    sio_ioctl( bt_ag.sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
  }
  else if ( bt_ag.ring_count > 0 )
  {
    bt_ag_ring_audio_device();
  }
  // could end up here after ringing stops in which case the
  // timer would have gotten reset to idle timeout value
  else if ( (rex_get_timer( &bt_ag_timer ) == 0) && (bt_ag.idle_timeout > 0) )
  {
    switch ( bt_ag.state )
    {
      case BT_AGS_AUDIO_DISCONNECTING:
      case BT_AGS_AUDIO_CONNECTED:
      case BT_AGS_CONNECTED:
        /*  Bring down the existing RFCOMM link and go idle  */
        /*  when SPP disconnected event occurs.              */
        if ( bt_ag.sio_stream_id == SIO_NO_STREAM_ID )
        {
          BT_ERR( "BT AG: No SIO Stream", 0, 0, 0 );
        }
        else if ( (bt_ag.idle_mode == BT_AGIM_AUTOMATIC) &&
                  (bt_ag.idle_timeout > 0) )
        {
          sio_ioctl( bt_ag.sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
        }
        else
        {
          bt_ag.is_idle = TRUE;
        }
        break;

      default:
        BT_ERR( "BT AG: Timer expired Bad AG St %x", bt_ag.state, 0, 0 );
        break;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ag_background_tick

DESCRIPTION
  Performs background monitoring for AG.

===========================================================================*/
void bt_ag_background_tick
(
  void
)
{

  if ( bt_ag.state != BT_AGS_DISABLED )
  {
    if ( (bt_ag.disabling == FALSE) &&
         ((bt_ag.state == BT_AGS_OPEN) ||
          (bt_ag.state == BT_AGS_CONNECTED) ||
          (bt_ag.state == BT_AGS_AUDIO_CONNECTED) || 
          (bt_ag.state == BT_AGS_CLOSED)) )
    {
      /*  Steady state reached - disable timer.  */
      bt_ag.ss_timer_ms_left = 0;
    }
    else
    {
      if ( bt_ag.ss_timer_ms_left == 0 )
      {
        bt_ag.ss_timer_ms_left = BT_AG_SS_TIMEOUT_MS;
      }
      else if ( bt_ag.ss_timer_ms_left > BT_BACKGROUND_TIMER_MS )
      {
        bt_ag.ss_timer_ms_left -= BT_BACKGROUND_TIMER_MS;
      }
      else
      {
        /*  AG steady state monitor has expired.  */
        BT_MSG_HIGH( "BT AG: SST expired AG St %x Dis %x",
                     bt_ag.state, bt_ag.disabling, 0 );

        bt_ag.ss_timer_ms_left = 0;

        switch ( bt_ag.state )
        {
          case BT_AGS_OPENING:
          {
            bt_ag_cancel_connect();
            break;
          }
          case BT_AGS_AUDIO_CONNECTING:
          {
            /* waiting for SCO connected/connect failed event */
            bt_ag_ev_rm_conn_failed_sco();
            break;
          }
          case BT_AGS_AUDIO_DISCONNECTING:
          {
            /* waiting for SCO disconnected event */
            bt_ag_ev_rm_disconnected_sco();
            break;
          }
          case BT_AGS_CLOSING:
          {
            if ( bt_ag.is_sio_server == FALSE )
            {
              bt_ag_close_sio();
            }
            else
            {
              bt_ag_process_spp_open( BT_EVR_L2_LINK_LOST );
            }
            break;
          }
        }
      }
    }
  }

} 


/*===========================================================================

FUNCTION
  bt_ag_process_command

DESCRIPTION
  Processes all commands intended for AG

===========================================================================*/
void bt_ag_process_command
(
  bt_cmd_msg_type*  ag_cmd_ptr
)
{
  
  switch( ag_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_AG_CONFIG:
      bt_ag_cmd_config( ag_cmd_ptr );
      break;

    case BT_CMD_AG_ENABLE:
      bt_ag_cmd_enable( ag_cmd_ptr );
      break;

    case BT_CMD_AG_CONNECT:
      bt_ag_cmd_connect( ag_cmd_ptr );
      break;

    case BT_CMD_AG_RING_AUDIO_DEVICE:
      bt_ag_cmd_ring_audio_device( ag_cmd_ptr );
      break;

    case BT_CMD_AG_AUDIO_CONNECT:
      bt_ag_cmd_audio_connect( ag_cmd_ptr );
      break;

    case BT_CMD_AG_AUDIO_DISCONNECT:
      bt_ag_cmd_audio_disconnect( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SET_AD_SPKR_GAIN:
      bt_ag_cmd_set_ad_spkr_gain( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SET_AD_MIC_GAIN:
      bt_ag_cmd_set_ad_mic_gain( ag_cmd_ptr );
      break;

    case BT_CMD_AG_DISCONNECT:
      bt_ag_cmd_disconnect( ag_cmd_ptr );
      break;

    case BT_CMD_AG_DISABLE:
      bt_ag_cmd_disable( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SET_VR_CAPABLE:
      bt_ag_cmd_set_vr_capable( ag_cmd_ptr );
      break;

    case BT_CMD_AG_UPDATE_VR_STATE:
      bt_ag_cmd_update_vr_state( ag_cmd_ptr );
      break;

    case BT_CMD_AG_EXT_IO_CONFIG:
      bt_ag_cmd_external_io_config( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SET_INBAND_RING:
      bt_ag_cmd_set_inband_ring( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SET_BATTCHG_IND:
      bt_ag_cmd_set_battchg_ind( ag_cmd_ptr );
      break;
  
    case BT_CMD_AG_SET_RSSI:
      bt_ag_cmd_set_rssi( ag_cmd_ptr );
      break;

#ifdef BT_AG_HF_EXTENSION
    case BT_CMD_AG_SEND_SUPPORTED_RH:
      bt_ag_cmd_send_supported_rh( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_CURRENT_RH:
      bt_ag_cmd_send_current_rh( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_LIST:
      bt_ag_cmd_send_pb_list( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_SELECT_RESULT:
      bt_ag_cmd_send_pb_select_result( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_INFO:
      bt_ag_cmd_send_pb_info( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_ENTRIES_INFO:
      bt_ag_cmd_send_pb_entries_info( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_READ_RESULT:
      bt_ag_cmd_send_pb_read_result( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_PB_WRITE_RESULT:
      bt_ag_cmd_send_pb_write_result( ag_cmd_ptr );
      break;

    case BT_CMD_AG_SEND_SUBS_NUM_INFO:
      bt_ag_cmd_send_subscriber_number_info( ag_cmd_ptr );
      break;
#endif /* BT_AG_HF_EXTENSION */

    default:
      BT_ERR( "BT AG: Bad Cmd Type %x AID %x",
              ag_cmd_ptr->cmd_hdr.cmd_type,
              ag_cmd_ptr->cmd_hdr.bt_app_id, 0 );
      ag_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
      break;
  }

}


/*===========================================================================

FUNCTION
  bt_ag_process_ev_queue

DESCRIPTION
  Dequeues events on the AG event queue and processes them.

===========================================================================*/
void bt_ag_process_ev_queue
(
  void
)
{
  
  bt_ev_msg_type*  bt_ev_ptr;
  int              q_count = q_cnt( &bt_ag.ev_q );

  BT_VSTAT_MAX( q_count, AG_MAX_EV_Q_DEPTH );

  while( ( bt_ev_ptr = (bt_ev_msg_type*) q_get( &bt_ag.ev_q ) ) != NULL )
  {
    bt_ag_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}


/*===========================================================================

FUNCTION
  bt_ag_check_rx_wm_data

DESCRIPTION
  Processes data on the AG RX watermark.

===========================================================================*/
void bt_ag_check_rx_wm_data
(
  void
)
{
  
  dsm_item_type*  dsm_ptr;
  uint8           data_byte;
  bt_ag_ev_type*  ev_ptr;

#if defined ( FEATURE_IBT )  ||  defined ( FEATURE_IBT_DYNAMIC )
  if ( bt_agcpe.call_orig_rx_wm_ne_func != FALSE )
  {
    bt_agcpe.call_orig_rx_wm_ne_func = FALSE;
    if ( bt_agcpe.orig_rx_wm_ne_fptr != NULL )
    {
      (bt_agcpe.orig_rx_wm_ne_fptr)();
    }
  }
#endif

  if ( bt_ag.flags & BT_AG_SEND_DATA_B )
  {
     bt_ag.flags &= ~BT_AG_SEND_DATA_B;
     if ( dsm_length_packet( bt_ag.tx_dsm_ptr ) == 0 )
     {
       bt_ag.cur_dsm_ptr_ptr = &bt_agcpe.tx_dsm_ptr;
     }
     bt_ag_send_data();
  }

  while ( (ev_ptr = (bt_ag_ev_type*) q_get( &bt_ag.cb_ev_q )) != NULL )
  {
    switch ( ev_ptr->hdr.ev_type )
    {
      case BT_AG_SPP_EV:
        bt_ag_process_spp_event( &ev_ptr->spp_ev );
        break;
      case BT_AG_CM_EV_CALL:
        bt_ag_process_cm_call_event( &ev_ptr->call_ev );
        break;
#ifndef FEATURE_BT_WITH_OLD_CM
      case BT_AG_CM_EV_CALL_INFO_LIST:
        bt_ag_process_cm_call_info_list();
        break;
#endif
      case BT_AG_CM_EV_PH:
        bt_ag_process_cm_ph_event( &ev_ptr->ph_ev );
        break;
      case BT_AG_CM_EV_SS:
        bt_ag_process_cm_ss_event( &ev_ptr->ss_ev );
        break;
      case BT_AG_CM_EV_INBAND:
        bt_ag_process_cm_inband_event( &ev_ptr->inband_ev );
        break;
    }
    q_put( &bt_ag.cb_ev_free_q, &ev_ptr->hdr.link );
  }

  while ( (dsm_ptr = dsm_dequeue( &bt_ag.rx_wm )) != NULL )
  {
    while ( dsm_pullup( &dsm_ptr, &data_byte, sizeof( uint8 ) ) ==
              sizeof( uint8 ) )
    {
      if ( bt_ag.sio_stream_id != SIO_NO_STREAM_ID )
      {
        bt_ag_cpe_process_rx_byte( data_byte );
      }
      else
      {
        BT_MSG_DEBUG( "BT AG: dropping left over data %c", data_byte, 0, 0 );
      }
    }

    dsm_free_packet( &dsm_ptr );  /*  Just to be safe.  */
  }

}


#ifdef BT_SWDEV_QXDM_LOG_PER
/*===========================================================================

FUNCTION
  bt_ag_snapshot_logging_function

DESCRIPTION
  Log all the AG related statistics.

===========================================================================*/
void bt_ag_snapshot_logging_function
(
  void
)
{
  
  log_bt_ag_type*  log_ptr;

  log_ptr = ( log_bt_ag_type *)log_alloc(log_bt_ag, sizeof(log_bt_ag_type));

  if( log_ptr != NULL )
  {
    log_ptr->ag_state                         = bt_ag.state;
    log_ptr->ag_app_id                        = bt_ag.app_id_ag;
    *((bt_bd_addr_type *)log_ptr->ag_bd_addr) = AUDIO_DEV.bd_addr;
    log_ptr->ag_my_server_channel             = bt_ag.is_sio_server;
    log_ptr->ag_remote_server_channel         = bt_ag.idle_mode;
    log_ptr->ag_rfcomm_pid                    = bt_ag.sio_stream_id;
    log_ptr->ag_sco_handle                    = bt_ag.sco_handle;

    log_commit(log_ptr);
  }

}
#endif /* BT_SWDEV_QXDM_LOG_PER */

#endif /* FEATURE_BT_AG */
#endif /* FEATURE_BT */
