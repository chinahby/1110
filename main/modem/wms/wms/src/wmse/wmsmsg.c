/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    W M S   M O D U L E
                    -- Message Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the configuration group functions which process the client
  commands of the configuration group.

  Copyright (c) 1998 - 2008
  by QUALCOMM INCORPORATED.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsmsg.c#11 $ $DateTime: 2010/02/23 18:05:14 $ $Author: rajang $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/09   HN      Correct retry timer value in wms_msg_mc_mo_status_proc()
04/27/09   HN      Set proper routing for CCAT when SMSPP service is inactive
11/13/08   HKM     In HCPD if current mcc is invalid pass home mcc to CM
09/26/08   HKM     Added HCPD support for incoming messages    
08/06/08   YC      Disabled MO msg retry if DC can't be established
06/24/08   PMD     Removed Tabs from code
06/23/08   HN      Fixed Lint & Klocwork warnings
06/18/08   PMD     Populate the Message in Send Event while send from Memory
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/19/08   PMD     Removed wms_msg_is_ok_to_send_gw_sms and check for not 
                   sending Message in Limited Service
04/22/08   PMD     Modified wms_cm_gw_send_rl_*** prototypes
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
04/02/08   PMD     Corrected Length Checks
03/31/08   PMD     Added Buffer Overflow checks
02/29/08   PMD     Fixed logic in wms_msg_mo_sms_fdn_control_func()
02/27/08   PMD     Updated with comments for Enhanced Domain Selection 
02/26/08   PMD     Updated wmscm_gw changes for Enhanced Domain Selection
02/22/08   PMD     Fixed Compile Warning with CM_API_PLUS_DIALING on GSM Builds.
02/15/08   PMD     Fixed bug in Enhanced Domain Selection with no CM_NAS
02/14/08   PMD     Added support for common type wms_cm_conn_id_type
02/11/08   PMD     Unified Logic for Domain Selection Enhancements
02/07/08   PMD     Fixed Error Codes in wms_msg_do_write_sts_report()
01/30/08   PMD/PY  Created cm/nas interfacing layer in wmscm_gw.h and wmscm_gw.c
01/14/08   PMD     Added support for MO SMS FDN Control 
11/20/07   PMD     Fixed signed, unsigned operations for mo_retry_interval
11/14/07   PMD     Fixed Lint Warnings
11/06/07   NP      A permanent SIP failure should stop the retry timer in WMS 
10/29/07   PMD     Fixed bug in wms_msg_gstk_evt_proc()
10/09/07   PY      Refactor the TL ack handler.
09/07/07   PY      fix wms_msg_choose_bearer() to chose active bearer.
08/15/07   PY      split wmsmsg.h to form wmsmsgi.h a private header.
08/07/07   PY      Added a couple of abstractions to CM/NAS interfacing code.
08/06/07   PMD     Plus code dialing enhancements
07/30/07   PMD     Fixed Potential Compile Warning
07/23/07   PMD     Added support for plus code dialing
07/25/07   PMD     Removed Error Message for Ack Report
07/23/07   PMD     Fixed Compile Warning due to typecast of ack_client_id
07/20/07   PMD     Added support for correct Ack_Report Event
07/11/07   PY      removed wms_bc_s_ptr dependency on wmsmsg.c
07/06/07   PMD     Domain setting changes based on discussion
06/20/07   PMD     Include wmstsi.h
06/06/07   PMD     Added more RP-Errors for swtiching domain to CS_ONLY.
05/02/07   HQ      Added FEATURE_WMS_NO_CM_MSG2; mainline some IWMSC fields.
04/20/07   HQ      Updates for interfacing with WMS Components.
04/19/07   YZ      Fixed MO_ACK timer constant.
04/12/07   HQ      Fixed Lint errors.
04/06/07   HQ      Added more logic for Domain Selection Optimization.
04/03/07   HQ      Added support for App Processor Power Collapse.
04/02/07   HQ      Disable the check of RP_OA present flag for now.
03/28/07   PMD     Fix for not delivering Consumed Messages to other clients
                   if route is WMS_ROUTE_TRANSFER_AND_ACK
03/27/07   HQ      Revert some changes for domain selection.
03/21/07   HQ      Check for RP_OA/RP_DA being present or not.
03/16/07   HQ      Added support for LPM.
03/16/07   PMD     Fixed lint warning due to indentation
03/10/07   HQ      Added support for GW Domain Selection Enhancement.
                   fixed compiler warning regarding cmd_err not in use.
03/07/07   HQ      Fixed compiler warning regarding cm_s_ptr.
03/06/07   HQ      Added support for IWMSC.
03/02/07   HQ      Switch to 1x if SMS over EVDO fails.
03/01/07   HQ      Cleaned up SS info for GW.
02/28/07   HQ      Added support of FEATURE_SMS_OVER_IP;
                   Display F3 msg with proper chosen_bearer;
                   Handle new SIP errors.
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
02/09/07   PMD     Added Support for NO_RESPONSE_FROM_NETWORK
01/27/06   PMD     Added check for No / Limited Service for GW MO SMS
01/23/07   HQ      Added mapping of new status codes.
01/17/07   HQ      Updated copyright info to include 2007.
01/15/07   HQ      Fixed incorrect status mapping for WMS_MISSING_PARM_S.
01/15/07   PMD     Defaulting Domain Preference to CS Only in case of failure
01/04/07   PMD     Fixed bug in WMS_MSG_EVENT_WRITE_STS_REPORT Event Payload
12/04/06   HQ      Clear mc_msg.is_valid if layer2 timer expires.
11/30/06   NP      WMS to release CDMA DC / traffic call when done with traffic
                   channel use
11/29/06   PMD     Fixed Null Pointer Dereference in wms_msg_cdma_map_status()
                   Fixed Null Pointer Dereference in wms_msg_get_retry_period()
11/21/06   HQ      Clean up lint errors; changed msg levels to HIGH.
11/13/06   HQ      Check ESN in incoming messages for JCDMA2.
11/07/06   PMD     Fixed intorduced bug due to bad fix
11/07/06   PMD     Fixed SC Address Fillup incase of MO SMS Control
11/06/06   PMD     Eliminated FEATURE_SMS_LTK from Code
11/01/06   PMD     Fixed Incorrect Client ID in WMS_MSG_EVENT_ACK_REPORT Event
                   Only Allow Privileged / Consuming Clients to Ack the MT SMS
                   Changed wms_msg_cdma_deliver() function to return client_id
                   Fiexed bug in wms_msg_map_mt_record();Added Debug Statements
10/25/06   PMD     Fixed High Lint Warnings due to access out-of-bounds pointer 
10/24/06   PMD     Fixed High Lint Warning
10/17/06   PMD     Fixed Lint Warnings by adding checks and renaming variables
10/05/06   PMD     Fixed Compile Warnings
10/05/06   PMD     Read Service Option from NV (dc_s_ptr->default_so_from_nv)
09/28/06   NP      Fill in SC Address if none supplied for msg storage also.
09/28/06   PMD     Send Memory Capacity Exceeded Error on SIM Write Failure 
                   if Phone Memory is not available
09/15/06   PMD     Changed default Service Option to SO_6
08/26/06   PMD     Added support for EVENT_MO_SMS_RETRY_ATTEMPT
                   Added Function Headers
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
08/14/06   PMD     Replaced / Mainlined FEATURE_CDSMS_UDH_WAP in code.
08/12/06   PMD     Added Checks for msg_s_ptr == NULL in get/set_retry_period.
07/31/06   PMD     Removed unused feature - FEATURE_UASMS_SHIM
07/31/06   PMD     Removed unused feature - PERL_UTF
07/31/06   PMD     Removed unused feature - SMS_TREX
06/26/06   PMD     Removed Commented Code; Fixed bug in write_cdma_sms()
06/14/06   PMD     Fix for SMS Retry with no Previous Report
06/06/06   PMD     Added Checks for Message Tag while writing message
06/01/06   PMD     Featurized RUIM Cache under FEATURE_CDSMS_CACHE
05/19/06   PMD     Fixed compile warning
05/04/06   HQ      Added auto-dc send feature;
                   Fxied the MO Ack timeout issue during retry.
05/03/06   PMD     Dont Auto-Store Type 0 Messages in WMS
05/03/06   PMD     Added support for SMS CCAT Feature
                   Modified wms_ts_decode_CDMA_bd prototype
04/25/06   HQ      Fixed lint errors.
04/24/06   PMD     Added fix for RPC - check_voicemail_contents
04/20/06   PMD     Eliminated MO Ack Timer, Pending Ack Timer Logic for CDSMS
                   Updated MO SMS and ACK Retry Logic and Record Type.
                   Mainlined FEATURE_SMS_RETRY.
                   Added Function Headers.
04/11/06   HQ      Fixes for cleaning up Lint Errors; updated comment headers.
04/07/06   HQ      Added wms_msg_cdma_mt_busy() and wms_msg_cdma_mo_busy().
03/30/06   PMD/HQ  Discard SCPT messages if SCPT is not enabled.
02/03/06   HKM     Removed the done queue pointing to pending queue logic
                   so that command buffer is not out of resources
02/03/06   PMD     Removed Null Pointer check for cl_msg_ptr in wms_msg_send_gw()
02/02/06   PMD     Added check for length before packing SMS TPDU
01/31/06   PMD     Update Delete Message Event with Correct Tag
01/27/06   PMD     Return FALSE if FEATURE_CDSMS_DUP_DETECTION is not defined.
01/25/06   VK      Updated Copyright Info
01/23/06   VK      More Lint fixes
01/20/06   PMD     Include wmsts.h to resolve Compile Warnings
12/22/05   VK      Removed ERR_FATALs
12/21/05   VK      Lint fixes
12/20/05   PMD     Cleaned up FEATURE_SIRIUS from Code Base.
12/20/05   PMD     Added Support for CS Only and PS Only GW Domain Preference.
12/12/05   VK      Checks for NULL Ptrs
11/18/05   VK      Cleaned up Lint warnings. Removed some redudant code. 
11/04/05   PMD     Modified WAP Push Handling under F_SIRIUS
10/27/05   PMD     No Special Handling for MT WAP Push SMS under F_SIRIUS
10/27/05   PMD     Fixed Compile Warning
10/26/05   PMD     Fixed Storing of MT SMS Message.
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/20/05   VK      Removed Lint Suppressions
10/19/05   PMD     Pick up SC Address from first SIM Template rather than last
10/19/05   VK      Added new feature FEATURE_CDSMS_DUP_DETECTION
                   Added new feature for awi - FEATURE_ACP
                   Added new feature FEATURE_CDSMS_CACHE
10/19/05   PMD     MT SMS Storage enhancements under FEATURE_SIRIUS
                   Updated Memory Full Handling in wms_msg_do_write for GW_NV
10/18/05   PMD     Added Support to store to SIM Memory if Phone Memory is Full
                   for incoming GW SMS Message under FEATURE_SIRIUS
                   Fixed CPHS and ALS Logic while storing Incoming Message.
10/18/05   VK      SMS_RETRY Featurization fixes
10/17/05   PMD     Updated Copyright Information.
10/17/05   PMD     Support for New Retry Logic in WMS
10/17/05   PMD     Added Support for wms_msg_get_retry_period() API
10/17/05   PMD     Used msg_s_ptr->mo_retry_interval in place of Constant
10/14/05   PMD     Changed wms_msg_check_tpdu_type prototype to be a pointer.
09/27/05   PMD     Added support for decoding PDU before storing GW Message.
09/20/05   PMD     Added GCF Flag Check before setting Retry Period.
                   Read GCF Flag before Retrying Message.
09/06/05   PMD     Added code for rex_set_timer( WMS Clear Timer)
09/06/05   PMD     Added wms_msg_get_retry_period API
09/01/05   PMD     Added Check if Retry is Enabled before clearing record.
08/23/05   PMD     Added mem_store while replacing existing Message.
                   Fixed Compile Warning.
08/08/05   Rex     Fixed lint warning in 1x code
08/05/05   Rex     Code cleanup
08/04/05   PMD     Fixed certain Lint Warnings in code.
07/27/05   PMD     Added special handling for SM_TYPE_0 Messages.
07/22/05   PMD     Check for valid sim slot for sending message from Memory
                   if sim sms is not complete.
07/18/05   PMD     Implemented Fallback to CS Domain if PS SMS Fails.
07/17/05   PMD     Fixed bracket problem for wms_clear_timer.
07/08/05   Rex     Fixed problem in clear CDMA MO record
06/22/05   PMD     Fixed bracket problem in Clear GW MO Record.
06/17/05   PMD     Added FEATURE_GWSMS_IGNORE_SIM_MEMORY Flag
06/17/05   PMD     Fixed Merge Issue with Status Report Write
06/10/05   VK      Fixed compile errors
06/07/05   Rex     Fixed voicemail checking
06/05/05   PMD     Locate MO SMS Message for Status Report only for SIM Memory store
06/03/05   Rex     Fixed clearing all mo record for timer expiration
05/14/05   PMD     Added RAM Storage Support for Status Reports
05/05/05   Rex     Added function wms_msg_offline
03/31/05   Rex     Added TP-PI for delivery report messages
03/07/05   PMD     Fixed Memory Leak in wms_msg_gw_cb_data_download_proc
03/07/05   PMD     Fix for UMTS BMC Message Length Check
02/14/05   Rex     Length checking for memcpy
02/04/05   PMD     Added dual slot support for SMS PP Download.
01/28/05   PMD     Modify Routes for 1x WAP Push Messages to Transfer and Ack.
01/24/05   PMD     Check for SC Address Length Check before Sending Message.
01/18/05   VK      Fixed compile problems.
01/17/05   Rex     Code cleanup for WMS decoupling from NAS module and
                   using CM, featurize as FEATURE_APIONE
01/17/05   VK      Fixed Compiler warnings
01/10/05   Rex     Fixes for CPHS feturization name
01/07/05   PMD     Added feature FEATURE_SMS_LTK
01/06/05   Rex     Fixed clear msg record in wms_msg_cdma_ack_proc
12/14/04   PMD     Modify Routes for WAP Push Messages to Transfer and Ack.
12/01/04   PMD     Dont Display Debug Message for Failure Cause if WMS_RPT_OK
11/16/04   PMD     Changed Prototype for wms_msg_SMMA_conf_proc
                   Cleared MO Record for SMMA Confirmation.
11/03/04   PMD     Added Debug Messages
10/06/04   PMD     Added Debug Statements for MT SMS Messages
10/04/04   PMD     Added support for CB Data Download
09/24/04   Rex     Featurization fix
09/21/04   Rex     Added support for CPHS
08/04/04   PMD     Set cmd_err = NONE if NV Template is good.
08/03/04   Rex     Added gw template mask check for sc address presence
08/02/04   PMD     Added Support to insert SC Address from Template if client
                   provides no SC Address.
08/02/04   PMD     Fixed Voicemail Handling in wms_msg_sm_data_ind_proc()
                   Removed wms_check_udh_voicemail() function.
07/30/04   HQ      Removed ERR_FATALs.
07/29/04   HQ      Fixed wrong display of MSG_ERRORs.
07/15/04   PMD     Changed Msg Level for Unable to allocate Log Rsp Packet.
07/15/04   PMD     Added support for Duplicate Detection Indication from CB
06/30/04   PMD     Added Debug Messages to print Cause Values for Failures
06/28/03   VK      Added cmd_err
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
05/29/04   Rex     Added function wms_msg_gw_cb_send_log
04/29/04   HQ      Changed WMS_TL_UNEXPECTED_PARM_VALUE_S to
                   WMS_TL_UNRECOGNIZED_PARM_VALUE_S for invalid parm values.
04/13/04   HQ      In wms_msg_map_mo_record(), check both in_use and smi;
                   don't access MN buffer after sending it to MN since MN
                   has higher priority than WMS and MN might clear the buffer.
04/12/04   HQ      In wms_msg_map_mt_record(), match against tid instead of smi.
04/06/04   PMD     Added Checks for Command TPDU in check_tpdu_type()
                   Fix for failed MO SMS Control Message.
03/29/04   Rex     Check for start full service cdma/gw ready prior to building
                   the submit message
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
                   Fixed Compile Warning.
03/15/04   AP      Fixed attempt to free memory that is occupied by a staticly
                   declared structure.
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
02/19/04   HQ      Added support for FEATURE_GWSMS_DEPERSO.
02/17/04   PMD     Added check for not Sending Message if decoding Fails
02/04/04   PMD     Cleaned up Lint Warnings in code
02/04/04   ak      Added Perl UTF flag
02/04/04   AP      Clear sigs when processing retries and exiting retry.
01/29/04   PMD     Initialized Message Mode in msg_event_info
01/23/04   AP      Should not retry in wms_msg_cdma_mo_ack_timer_proc().
01/20/04   HQ      Changed ruim_data's size to NV_SMS_DATA_SIZ to avoid data
                   overflow when a message is read from NV.
01/16/04   PMD     Included "task.c"
01/16/04   AP/PMD  Added GW and CDMA Retry
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
12/15/03   HQ      Moved #include tmc.h.
12/04/03   HQ/AP   Fixed GSM SMS PP download address conversion problem.
12/02/03   HQ      Resolved some Lint errors.
11/14/03   Rex     Removed check_tpdu type for status report and command
11/04/03   AP      Adjustments for FEATURE_SMS_TRANSPARENT_MO:
                   check for ruim_max_slots before storing to CARD,
                   flag to get next msg ids when building submit messages,
                   use client_id_type for API's.
                   Check nv_cmd_err if nv_write fails.
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
                   Added message_mode in status_info when reading templates.
10/28/03   HQ      Added rp_cause in gw_ack_info_s_type.
10/23/03   HQ      Use wms_rp_cause_e_type for cause_value in ack_info.
10/15/03   HQ      Fixed compiler warnings.
09/07/03   PMD     Added Support for msg_event String Display
09/03/03   HQ      Skip duplicate detection for non-WMS_ROUTE_STORE_AND_NOTIFY
                   messages.
08/28/03   AP      Added support to generate new msg event ACK_REPORT.
08/21/03   HQ      Skip msg_id generation for TELESERVICE_WAP.
08/20/03   PMD     Assigned correct value to the Client_ID field in Status
                   Info Event
08/08/03   HQ      Fixed CDMA status mapping for
                   WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S.
08/04/03   PMD     Changes to suppress / remove Lint Errors
08/01/03   HQ      Fixed compile warnings by removing unused gsdi_rsp.
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
07/21/03   PMD     Fixed Index of Voice Mail Message while writing in Replace
                   Mode
07/10/03   Rex     Added SMS diag for EVENT_MT_SMS_NOTIFY and
                   EVENT_MO_SMS_STATUS
07/01/03   PMD     Fixed Compilation Error in wms_check_udh_voicemail
                   Added New function for checking Pending MO/MT records
                   Changed handling in wms_msg_gstk_evt_cb
06/24/03   SUN     Fixed voice mail header
06/20/03   HQ      Check if message_mode==GW for dynamic alpha ID;
                   set alpha_id.data to NULL when it is freed.
06/19/03   HQ      Moved gw_template_alpha_id under FEATURE_GWSMS.
06/18/03   PMD     Added new variable for storing the GW template Alpha id
                   Added correct Alpha_id to msg_send_event in
                   write_template_proc
06/11/03   PMD     Added a check for cmd_err = none before sending a message
06/08/03   PMD     Initialization for dynamic Alpha ID in GW template
06/05/03   PMD     Added Support for Alpha Tag in Status Info and Submit Report
06/05/03   SUN     Remember the client id for messages that are sent from memory
06/02/03   AP      Don't reset mt and mo record timers when hadling pending
                   messages.  Made rtre control more specific for
                   NV_RTRE_CONTROL_USE_RUIM.
05/28/03   AP      Added client_id to mt_message_info struct for both GW and
                   CDMA after allowing the clients to parse the MT message;
                   get mn cmd buf when acking and tpdu type is NONE.
05/28/03   HQ      Set message mode to GW when a GW submit report is received.
05/22/03   HQ      Make scpt_data and scpt_result to be pointers.
05/09/03   SUN     Set Message mode to GW when a GW message is received
05/06/03   HQ      Use uint32 counter instead of uint8 for dup detect loop.
05/02/03   HQ/AP   Correctly set mem_store to NONE for voice mail REPLACE;
                   use the correct array boundary for dup cache;
                   set tag to SENT for MO msg not requesting TL Ack;
                   replace WMS_DUMMY_CLIENT_ID with WMS_CLIENT_TYPE_INTERNAL;
                   fixed wrong command error codes being used.
04/30/03   AP      Changed feature-ization for client_ts to not be dependant on
                   FEATURE_GSTK.
04/23/03   AP/HQ   Fixed compile issue with client_ts.
04/17/03   AP      Added support for FEATURE_UIM_TOOLKIT_UTK && FEATURE_GSTK.
04/11/03   SUN     Send Message list and Message Status Event to client when
                   Status Report is deleted.
04/03/03   AP      Pass correct length to unpack ascii when checking for BREW
                   prefix in cdma_deliver() to not overflow buf.
03/27/03   AP      Updated msg_event_info if client's parsing callback does
                   not want to share the message in cdma_deliver.
03/26/03   HQ      Skip dup detection for GSM1x SMS messages.
03/17/03   HQ      Check whether SMS PP download is supported before sending
                   PP download command;
                   changed  GSTK address data format.
03/14/03   SUN     While deleting, skip the empty messages
03/14/03   SUN     Filled the Status Report Index Corectly
02/26/03   HQ      Added support for MO SMS Control.
02/21/03   SUN     Changed function name for Status Report
02/14/03   AP      Added support for JCDMA and client parsing.
02/11/03   HQ      Changed PP download function prototype;
                   allocate heap memory for TPDU.
02/10/03   SUN     Cache the message reference number,
                   Delete the status report when the message is deleted
02/07/03   HQ      Fixed compile problem for SMS PP Download.
02/07/03   SUN     Added Support for Status Reports and Commands
                   Cached Message list Info for GW
01/30/03   SUN     Set sim_data to FFs before writing to SIM.
01/24/03   AP      Added additional check of shared flag and routing; if the
                   routing is store and notify then WMS will ack the message.
01/24/03   AP      Added support for delivering the submit report with client id
                   and properly delivering a CDMA MT msg to the clients.
01/15/03   HQ      Added support for FEATURE_CDSMS_IS637B_BROADCAST_SCPT.
01/06/03   HQ      Added support for FEATURE_GWSMS_PP_DOWNLOAD.
12/17/02   AP      Fixed handling of AWI ack when there is no error;
                   Replace usage of decode_CDMA_bd with decode_bearer_data to
                   correctly decode amps messages.
12/09/02   AP      Feature-ized support for wmsdiag.
12/05/02   AP      Fixed parameter check in wms_msg_cdma_check_duplicate().
12/04/02   HQ      Removed compiler warnings.
11/21/02   HQ      Added event reporting to Diag:
                   EVENT_MT_SMS_NOTIFY and EVENT_MO_SMS_STATUS.
11/18/02   HQ      Feature-ized wms_msg_cdma_has_wap_udh() under
                   FEATURE_CDSMS_UDH_WAP.
11/14/02   HQ      Properly feature-ized msg info cache and dup detection to
                   resolve compile errors and warnings for GW targets.
11/13/02   AP      Added support for: Simultaneous MO messages not
                   requesting a tl_ack. Updating duplicate detection cache.
                   Fixed memcpy of correct data before updating msg list cache,
                   mainlined message info cache support.  Update cmd_ptr for
                   message send.
11/06/02   SUN/HQ  Allocate MN buffer in wms_msg_do_client_ack().
11/05/02   HQ      Check CDMA teleservice msg type for sending MO msg;
                   don't change msg id for Cancellation and User Ack;
                   check GW TPDU type for sending MO msg;
                   don't change msg_ref if reject_dup is ON in Submit TPDU.
11/01/02   HQ      Added msg routing to WAP/MMS client in CDMA mode.
10/30/02   SUN     If NV is full, it should always send MEMORY_FULL_ERR
10/30/02   SUN     Saving Routing Information for Deliver Msgs
10/23/02   HQ      Added support for Brew SMS prefix.
10/18/02   SUN     Fixed the TPDU type for SUBMIT Reports
10/06/02   HQ      Added support of Brew oriented message event.
10/06/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE and
                   FEATURE_GWSMS_MSG_INFO_CACHE.
10/04/02   AP      Fixed handling of voice mail message for CDMA.
08/13/02   AP      Feature-ized parts of wms_msg_cdma_deliver() to check if
                   FEATURE_UASMS_SHIM is not defined, to avoid route change.
08/06/02   AP      Added proper routing and mem_store in the case of a UTK
                   UPDATE RECORD.
08/05/02   HQ      Fixed GW MN buffer overflow problem.
08/03/02   HQ      Added wms_msg_event_notify_single_client();
                   added single client routing for UTK and GSM1x messages.
08/01/02   SUN     Fixed Compile errors without FEATURE_GWSMS_UDH_EMS Defined
08/02/02   AP      Feature-ized GW specific code in wms_msg_total_memory_full.
07/31/02   SUN     Set MEM_FULL flag when SIM and NV are full. SMMA is sent if
                   a slot is avaiilable in either SIM or NV
07/31/02   SUN     Changed RP Cause for MT messages> 160 chars to
                   WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION
06/10/02   SUN     Fixed Delete All in NV. Added MSG_ERRORs, Added function to
                   replace voicemail.
07/10/02   HQ      Set the right tag when writing messages to RUIM;
                   store RUIM format in NV for CDMA SMS parameters.
06/30/02   AP      Modify teleservice to be WMS_TELESERVICE_BROADCAST when
                   tl_message_type is WMS_TL_TYPE_BROADCAST.
06/25/02   HQ      Fixed a compile problem.
06/22/02   HQ      Added RUIM memory processing (read, write, send, templates).
06/22/02   HQ      Added command wms_msg_delete_all() and
                   wms_msg_delete_template_all().
06/15/02   HQ      Added support of FEATURE_BROADCAST_SMS.
06/10/02   HQ      Added processing of WMS_MSG_EVENT_MT_MESSAGE_ERROR.
06/10/02   SUN     Wait only for SIM Initialisation before sending SMMA
06/06/02   SUN     Cleared in_use flag for SMMA
06/03/02   HQ      Change smi in wms_msg_map_mo_record to be a word.
06/03/02   SUN     SMMA should be sent only after sim initialization is complete
                   and after location update.
05/16/02   SUN     Fixed Template Read/Write error
05/14/02   SUN     Fixed SMMA Issue
05/09/02   SUN     Fixed TPDU Length issue.
05/07/02   HQ      Added lots of message handling logic under FEATURE_CDSMS.
05/06/02   SUN     Wrong error msg was being sent if udl > 140 octets
04/23/02   SUN     Filled raw data before sending message from memory
                   Added format type while sending message and receving report
04/12/02   SUN     Fixed Voicemail insert problem
                   Added more error checking while sending the message
                   from the memory store
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
03/07/02   SUN     Added error checking in submit report function
02/22/02   SUN     Message can be written with WMS_TAG_MO_SENT,
                   updated index number after write in NV
02/13/02   SUN     Added nv replace and delete commands
02/08/02   SUN     Added error checking for tag/tpdu type and MTI.
                   Allowed writing MO_SENT_ST_STORED messages to other mem
                   stores.
02/04/02   SUN     Added RP_CAUSE
01/25/02   HQ      Removed CFG_EVENT_WRITE_ERROR.
01/22/02   SUN     Added index checking during msg read.
01/17/02   SUN     Fixed invalid index issues
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/19/01   HQ      Updated interface between WMS and MN.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsmsgi.h"
#include "wmstsi.h"
#include "wmspriv.h"
#include "wmscl.h"
#include "wmscfg.h"
#include "wmsdc.h"
#include "wmsdbg.h"
#include "msg.h"
#include "err.h"
#include "string.h"
#include "wmsnv.h"
#include "wmscm.h"
#include "wmsts.h"
#include "bit.h"
#include "event.h"
#include "tmc.h"
#include "task.h"

#include "cm.h"
#include "cmph.h"

#ifdef FEATURE_CDSMS
#include "wmsruim.h"
#include "wmstscdma.h"
#include "mccdma.h"
#ifdef FEATURE_CDSMS_BROADCAST
#include "wmsbc.h"
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#ifndef CUST_EDITION
#include "pbmlib.h"
#else
#include "gstk_exp.h"
#endif
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#if defined(FEATURE_GWSMS_PP_DOWNLOAD) || ((defined(FEATURE_UIM_TOOLKIT_UTK) || \
    defined(FEATURE_CCAT)))//Gemsea Modify for SMS-PP && defined(FEATURE_GSTK))
#include "gstki.h"
#endif /* F_GWSMS_PP_DOWNLOAD || ((F_UIM_TOOLKIT_UTK||F_CCAT) && F_GSTK) */

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */

#ifdef CM_API_PLUS_DIALING
#include "cmutil.h"
#endif /* CM_API_PLUS_DIALING */

/*===========================================================================
========================== MACROS   =========================================
===========================================================================*/
#define WMS_RELATIVE_VALIDITY_IMMEDIATE          246
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
#include "OEMWMS_CacheInfo.h"
#endif
#endif
/*===========================================================================
======================== LOCAL DATA =========================================
===========================================================================*/

static wms_msg_s_type  *msg_s_ptr;
static wms_cfg_s_type  *cfg_s_ptr;
#ifdef FEATURE_CDSMS
static wms_dc_s_type   *dc_s_ptr;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


static wms_msg_event_info_s_type         msg_event_info;

#if (defined(FEATURE_CDSMS)||defined(FEATURE_GWSMS_DEPERSO))
static wms_ack_info_s_type               ack_info;
#endif

#ifdef FEATURE_CDSMS
static uint8                             ruim_data[NV_SMS_DATA_SIZ];
static wms_msg_ruim_sms_s_type           ruim_sms;
static wms_tl_message_type               cdma_tl;
static wms_cdma_message_s_type           cdma_cl;
static wms_raw_ts_data_s_type            cdma_raw_bd;
//static uint8                           nv_buf[WMS_MAX_LEN];
#if (defined(FEATURE_UIM_TOOLKIT_UTK)||defined(FEATURE_CCAT))//Gemsea Modify for SMS-PP && defined(FEATURE_GSTK)
/* UTK static variables */
static wms_OTA_message_type              cdma_OTA;
static wms_address_s_type                pp_download_address;
static wms_subaddress_s_type             pp_download_subaddress;
static wms_teleservice_e_type            pp_download_teleservice;
static wms_message_number_type           pp_download_msg_id;
static wms_client_message_s_type         pp_download_user_ack_cl_msg;
#endif /* FEATURE_UIM_TOOLKIT_UTK && FEATURE_GSTK */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_DEPERSO
#error code not present
#endif /* FEATURE_GWSMS_DEPERSO */

#if defined(FEATURE_GWSMS) || defined(FEATURE_UIM_TOOLKIT_UTK) \
    || defined(FEATURE_CCAT)
static wms_client_ts_data_s_type         client_ts;
#endif

#if defined(FEATURE_GWSMS_PP_DOWNLOAD) || ((defined(FEATURE_GSTK) || \
    defined(FEATURE_CCAT)) || defined(FEATURE_UIM_TOOLKIT_UTK))//Gemsea Modify for SMS-PP && defined(FEATURE_UIM_TOOLKIT_UTK))
/* Toolkit static variables */
static gstk_address_type                 gstk_address;
static gstk_sms_tpdu_type                gstk_sms_tpdu;
#endif /* FEATURE_GWSMS_PP_DOWNLOAD || ((F_GSTK||F_CCAT) && F_UIM_TOOLKIT_UTK) */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#if defined(FEATURE_GWSMS) && defined(FEATURE_GSTK) && \
    defined(FEATURE_GWSMS_MO_CONTROL)
#error code not present
#endif /* F_GWSMS && F_GSTK && F_GWSMS_MO_CONTROL */

/* Timer for clearing the mo record periodically every 30 sec */
rex_timer_type  wms_retry_timer;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
#ifdef CM_API_PLUS_DIALING
static boolean wms_msg_convert_plus_address
(  
  wms_address_s_type      *wms_addr
);

static boolean wms_msg_convert_address_to_plus
(  
  wms_address_s_type      *wms_addr
);

#endif /* CM_API_PLUS_DIALING */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
#ifdef CUST_EDITION
static byte g_mochannel = WMS_MO_ONLY_TRAFFIC;
#endif
/*===========================================================================
======================== FUNCTIONS ==========================================
===========================================================================*/
#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/


static wms_cmd_err_e_type wms_msg_do_client_ack
(
  wms_cmd_type          * cmd_ptr
);

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

wms_cmd_err_e_type wms_msg_do_write
(
  wms_write_mode_e_type         write_mode, /* IN */
  wms_client_message_s_type     * msg_ptr   /* IN-OUT */
);

/*
*/
wms_msg_s_type* wms_msg_s_ptr
(
  void
)
{
  static wms_msg_s_type  msg_s;

  return &msg_s;

} /* wms_msg_s_ptr() */

#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION wms_msg_send_gstk_evt

DESCRIPTION
  Used to post GSTK Event to WMS Command Queue for Processing.

DEPENDENCIES
  none

RETURN VALUE
  wms_status_e_type

SIDE EFFECTS
  None.
===========================================================================*/
wms_status_e_type wms_msg_send_gstk_evt
(
  gstk_cmd_from_card_type  * gstk_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
  wms_cmd_type        *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL)
  {
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_GSTK_EVT;
    cmd_ptr->hdr.client_id   = WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.msg_gstk_evt.gstk_evt = * gstk_ptr;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
} /* wms_msg_send_gstk_evt() */
#endif /* FEATURE_GSTK */

#if defined(FEATURE_GWSMS_PP_DOWNLOAD) || ((defined(FEATURE_UIM_TOOLKIT_UTK) || \
    defined(FEATURE_CCAT))) //Gemsea Modify for SMS-PP && defined(FEATURE_GSTK))
/*
*/
void wms_msg_gstk_evt_cb
(
  gstk_cmd_from_card_type *    gstk_ptr
)
{
  wms_message_mode_e_type      msg_mode;

  if (gstk_ptr == NULL)
  {
    MSG_ERROR("gstk_ptr null pointer in wms_msg_gstk_evt_cb!",0,0,0);
  }
  else
  {
    switch( gstk_ptr->hdr_cmd.command_id )
    {
      case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
        msg_mode = (wms_message_mode_e_type)gstk_ptr->hdr_cmd.user_data;

        if(msg_mode == WMS_MESSAGE_MODE_GW)
        {
  #ifdef FEATURE_GWSMS_PP_DOWNLOAD
#error code not present
  #endif /* FEATURE_GWSMS_PP_DOWNLOAD */
        }
        else if(msg_mode == WMS_MESSAGE_MODE_CDMA)
        {
#if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT)) //Gemsea Modify for SMS-PP && defined(FEATURE_GSTK)
          client_ts.format      = WMS_FORMAT_CDMA;
          client_ts.u.cdma.mask = WMS_MASK_BD_MSG_ID;

          /* Set the message_id info */
          client_ts.u.cdma.message_id.type        = WMS_BD_TYPE_USER_ACK;
          client_ts.u.cdma.message_id.id_number   = pp_download_msg_id;
          client_ts.u.cdma.message_id.udh_present = FALSE;

          if( gstk_ptr->cmd.sms_pp_download_rsp.general_resp ==
              GSTK_ENVELOPE_CMD_SUCCESS )
          {
            MSG_FATAL("PP Download via Envelope was successful",0,0,0);
            /* No response codes required */
          }
          else
          {
            client_ts.u.cdma.mask |= WMS_MASK_BD_USER_RESP;
            if( gstk_ptr->cmd.sms_pp_download_rsp.general_resp ==
                GSTK_ENVELOPE_CMD_FAIL )
            {
              client_ts.u.cdma.user_response = WMS_RSP_DATA_DOWNLOAD_ERROR;
            }
            else /* BUSY */
            {
              client_ts.u.cdma.user_response = WMS_RSP_APPLICATION_TOOLKIT_BUSY;
            }

            if(gstk_ptr->cmd.sms_pp_download_rsp.response_data_present)
            {
              client_ts.u.cdma.mask |= WMS_MASK_BD_USER_DATA;
              client_ts.u.cdma.user_data.data_len =
                MIN (gstk_ptr->cmd.sms_pp_download_rsp.ack.length, WMS_CDMA_USER_DATA_MAX);
              client_ts.u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
              memcpy(client_ts.u.cdma.user_data.data,
                     gstk_ptr->cmd.sms_pp_download_rsp.ack.ack,
                     client_ts.u.cdma.user_data.data_len);
            }
          }
          pp_download_user_ack_cl_msg.msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
          pp_download_user_ack_cl_msg.msg_hdr.index     =
            (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
          pp_download_user_ack_cl_msg.msg_hdr.tag       = WMS_TAG_MO_NOT_SENT;
          pp_download_user_ack_cl_msg.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;

          /* Fill in cached info from envelope origination */
          if(wms_ts_encode(&client_ts, &pp_download_user_ack_cl_msg.u.cdma_message.raw_ts) == WMS_OK_S)
          {
            pp_download_user_ack_cl_msg.u.cdma_message.teleservice =
              pp_download_teleservice;
            pp_download_user_ack_cl_msg.u.cdma_message.address =
              pp_download_address;
            pp_download_user_ack_cl_msg.u.cdma_message.subaddress =
              pp_download_subaddress;
            pp_download_user_ack_cl_msg.u.cdma_message.is_tl_ack_requested = FALSE;
            pp_download_user_ack_cl_msg.u.cdma_message.is_service_present  = FALSE;
            pp_download_user_ack_cl_msg.u.cdma_message.is_mo               = TRUE;
            pp_download_user_ack_cl_msg.u.cdma_message.service = WMS_SRV_UNKOWN;

            (void)wms_msg_send( WMS_CLIENT_TYPE_INTERNAL,
                                NULL,
                                NULL,
                                WMS_SEND_MODE_CLIENT_MESSAGE,
                                &pp_download_user_ack_cl_msg );
            MSG_HIGH("UTK ENVELOPE USER ACK SENT OK",0,0,0);
          }
          else
          {
            MSG_ERROR("CONVERSION TO RAW DATA FAILED",0,0,0);
            MSG_ERROR("PROBLEM SENDING UTK USER ACK",0,0,0);
          }
#endif /* (FEATURE_UIM_TOOLKIT_UTK||FEATURE_CCAT) && FEATURE_GSTK */
        }
        break;

#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */

      case GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP:
        MSG_HIGH("Recieved CB Data Download Indication", 0, 0, 0);
        break;

      default:
        /* other commands/responses are ignored */
        break;

    } /* switch */
  }

  /* done */
  return;
} /* wms_msg_gstk_evt_cb() */

#endif /* FEATURE_GWSMS_PP_DOWNLOAD || ((F_UIM_TOOLKIT_UTK||F_CCAT) && F_GSTK) */


/*===========================================================================
FUNCTION wms_msg_filter_events

DESCRIPTION
  Decide whether this event can be sent to the client or not based on
  power management settings.

DEPENDENCIES
  none

RETURN VALUE
  TRUE:  OK to send event to client
  FALSE: Not OK to send event to client

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_filter_events
(
  wms_client_id_type       client_id,
  wms_client_s_type        * client_ptr,
  wms_msg_event_e_type     event
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return TRUE;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_msg_filter_events() */


/*
** TRUE:  shared; can continue with processing this event
** FALSE: not-shared; stop processing this event
*/
boolean wms_msg_event_notify
(
  wms_msg_event_e_type        event,
  wms_msg_event_info_s_type   *msg_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint32                      i;
  boolean                     shared = TRUE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (msg_event_info_ptr == NULL)
  {
    MSG_ERROR("msg_event_info_ptr null pointer!",0,0,0);
    return FALSE;
  }


#ifdef FEATURE_WMS_DEBUG_STRING
  MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event, wms_dbg_get_msg_event_string(event));
#else
  MSG_HIGH("wms_msg_event_notify: %d ", event, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->msg_event_cb )
    {
      if( TRUE == wms_msg_filter_events( (wms_client_id_type) i, client_ptr, event ))
      {
        client_ptr->msg_event_cb( event, msg_event_info_ptr, &shared );
      }

      if(shared == FALSE)
      {
        /* Message is consumed by the client, do not share this message with
        ** any other clients.
        */
        break;
      }
    }
  }

  /* done */
  return shared;

} /* wms_msg_event_notify() */

wms_client_type_e_type wms_msg_parse_msg
(
  const wms_client_message_s_type   *cl_msg_ptr
)
{
  wms_client_s_type           *client_ptr;
  uint32                      i;
  wms_client_type_e_type      client_id = WMS_CLIENT_TYPE_MAX;
  boolean                     shared = TRUE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_msg_parse_msg() called", 0,0,0);

  /* Notify clients
  */
  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    client_ptr = wms_client_ptr( (wms_client_id_type) i );
    if( client_ptr != NULL &&
        client_ptr->activated == TRUE &&
        client_ptr->parse_msg_cb )
    {
      client_ptr->parse_msg_cb( cl_msg_ptr, &shared );

      if(shared == FALSE)
      {
        /* Message is consumed by the client, do not share this message with
        ** any other clients.
        */
        client_id = (wms_client_type_e_type)i;
        break;
      }
    }
  }

  /* done */
  return client_id;
} /* wms_msg_parse_msg */


/*
*/
void wms_msg_event_notify_single_client
(
  wms_client_type_e_type      client,
  wms_msg_event_e_type        event,
  wms_msg_event_info_s_type   *msg_event_info_ptr
)
{
  wms_client_s_type           *client_ptr;

#ifdef FEATURE_WMS_DEBUG_STRING
    MSG_SPRINTF_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Notify: %d %s", event, wms_dbg_get_msg_event_string(event));
#else
    MSG_HIGH("wms_msg_event_notify: %d ", event, 0, 0);
#endif /* FEATURE_WMS_DEBUG_STRING */

  /* Notify the client
  */
  client_ptr = wms_client_ptr( (wms_client_id_type) client );
  if( client_ptr != NULL &&
      client_ptr->activated == TRUE &&
      client_ptr->msg_event_cb )
  {
    if( TRUE == wms_msg_filter_events( (wms_client_id_type) client, client_ptr, event ))
    {
      client_ptr->msg_event_cb( event, msg_event_info_ptr, NULL );
    }
  }

  /* done */
  return;

} /* wms_msg_event_notify_single_client() */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */




#ifdef FEATURE_CDSMS
/* Check for duplicate messages.
** Return value of TRUE means a duplicate was found, else FALSE.
*/
boolean wms_msg_cdma_check_dups
(
  const wms_tl_message_type     *new_tl_msg
)
{
#ifdef FEATURE_CDSMS_DUP_DETECTION


  boolean                           dup_found = TRUE;
  uint32                            i;

 if (new_tl_msg->cl_bd.message_id.udh_present == 1)
 	return FALSE;
  MSG_HIGH("Checking for duplicate messages!",0,0,0);


  if (new_tl_msg == NULL)
  {
    MSG_ERROR("new_tl_msg null pointer!",0,0,0);
    return FALSE;
  }


  if( cdma_tl.teleservice == WMS_TELESERVICE_WAP
#if defined(FEATURE_QMA) // Gemsea
    || cdma_tl.teleservice == WMS_TELESERVICE_QMA_WPUSH
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
    || cdma_tl.teleservice == WMS_TELESERVICE_WPUSH
#endif 
#ifdef FEATURE_GSM1x
#error code not present
#endif /* FEATURE_GSM1x */
    )
  {
    /* No dup detection to be done for the above teleservices
    */
    return FALSE;
  }


  /* Chech each message in the info cache for a duplicate */
  for(i=0; i<WMS_MAX_CDMA_MESSAGES_IN_MEM; i++)
  {
    /* Initialize dup_found for every check */
    dup_found = FALSE;

    /* Only compare to valid messages in the dup detect list */
    if((cfg_s_ptr->dup_detect[i].tl_mask == 0) &&
       (cfg_s_ptr->dup_detect[i].cl_mask == 0))
    {
      /* Go on to the next index in cache */
      continue;
    }

    /* Check matching msg id and address parameters against the message
    ** from the dup detection cache (mandatory fields).
    */
    if(new_tl_msg->cl_bd.mask & WMS_MASK_BD_MSG_ID)
    {
      if(new_tl_msg->cl_bd.message_id.id_number ==
         cfg_s_ptr->dup_detect[i].msg_id.id_number)
      {
        /* Check address (mandatory field) */
        if((new_tl_msg->address.digit_mode  ==
            cfg_s_ptr->dup_detect[i].address.digit_mode) &&
           (new_tl_msg->address.number_mode ==
            cfg_s_ptr->dup_detect[i].address.number_mode) &&
           (new_tl_msg->address.number_of_digits ==
            cfg_s_ptr->dup_detect[i].address.number_of_digits))
        {
          if(new_tl_msg->address.number_of_digits == 0)
          {
            /* might be an address for broadcast where the number of digits will
            ** be 0, might be a match.
            */
            dup_found = TRUE;
          }
          else
          {
            int k;
            for(k=0; k<new_tl_msg->address.number_of_digits; k++)
            {
              if(new_tl_msg->address.digits[k] ==
                cfg_s_ptr->dup_detect[i].address.digits[k])
              {
                /* Possible match */
                dup_found = TRUE;
              }
              else
              {
                /* Mismatch in address digits */
                dup_found = FALSE;
                break; /* get out of this loop early */
              }
            }
          }
        }
        else
        {
          /* Address does not match, therefore this is not a duplicate */
          dup_found = FALSE;
        }
      }
      else /* Msg ID does not match with this msg from cache */
      {
        dup_found = FALSE;
      }

      /* Keep checking we may have a duplicate (optional parameters) */
      if(dup_found)
      {
        /* Possible duplicate, check mc timestamp
        ** These instances are considered duplicates:
        ** (1) Msg ids and address are the same and timestamp is the same.
        ** (2) Msg ids and address are the same and both messages do not have
        **     a mc timestamp.
        */
        if((new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME) &&
           (cfg_s_ptr->dup_detect[i].cl_mask & WMS_MASK_BD_MC_TIME) &&
           (new_tl_msg->cl_bd.mc_time.year ==
              cfg_s_ptr->dup_detect[i].mc_time.year)   &&
           (new_tl_msg->cl_bd.mc_time.month ==
              cfg_s_ptr->dup_detect[i].mc_time.month)  &&
           (new_tl_msg->cl_bd.mc_time.day ==
              cfg_s_ptr->dup_detect[i].mc_time.day)    &&
           (new_tl_msg->cl_bd.mc_time.hour ==
              cfg_s_ptr->dup_detect[i].mc_time.hour)   &&
           (new_tl_msg->cl_bd.mc_time.minute ==
              cfg_s_ptr->dup_detect[i].mc_time.minute) &&
           (new_tl_msg->cl_bd.mc_time.second ==
              cfg_s_ptr->dup_detect[i].mc_time.second))
        {
          /* Timestamp & msg_id match */
          dup_found = TRUE;
        }
        else if(((new_tl_msg->cl_bd.mask & WMS_MASK_BD_MC_TIME) == 0) &&
                ((cfg_s_ptr->dup_detect[i].cl_mask & WMS_MASK_BD_MC_TIME) == 0))
        {
          /* No timestamp & msg_id match */
          dup_found = TRUE;
        }
        else
        {
          /* MC timestamp mis-match, this is not a duplicate. */
          dup_found = FALSE;
        }
      }

      /* Keep checking we may have a duplicate (optional parameters) */
      if(dup_found)
      {
        /* Possible duplicate, check subaddress only if it is available */
        if(new_tl_msg->mask & WMS_MASK_TL_SUBADDRESS)
        {
          /* Subaddress is available in the new message, check if the cached
          ** message has a subaddress to compare
          */
          if(cfg_s_ptr->dup_detect[i].tl_mask & WMS_MASK_TL_SUBADDRESS)
          {
            /* Compare all subparameters of the subaddress */
            if((new_tl_msg->subaddress.number_of_digits ==
                  cfg_s_ptr->dup_detect[i].sub_address.number_of_digits) &&
               (new_tl_msg->subaddress.type ==
                  cfg_s_ptr->dup_detect[i].sub_address.type) &&
               (new_tl_msg->subaddress.odd ==
                  cfg_s_ptr->dup_detect[i].sub_address.odd))
            {
              int n;

              /* Compare the digits */
              for(n=0; n<new_tl_msg->subaddress.number_of_digits; n++)
              {
                if(new_tl_msg->subaddress.digits[n] ==
                    cfg_s_ptr->dup_detect[i].sub_address.digits[n])
                {
                  /* Possible match */
                  dup_found = TRUE;
                }
                else
                {
                  dup_found = FALSE;
                  /* get out of this loop */
                  break;
                }
              }
            }
            else
            {
              /* Sub-address mismatch */
              dup_found = FALSE;
            }
          }
          else
          {
            /* New msg has sub-address, but msg from cache does not. */
            dup_found = FALSE;
          }
        }
        else if(cfg_s_ptr->dup_detect[i].tl_mask & WMS_MASK_TL_SUBADDRESS)
        {
          /* The new message does not have a sub-address, but our message info
          ** in cache does, therefore this is not a duplicate.
          */
          dup_found = FALSE;
        }
        else
        {
          /* Both messages do not have a sub-address parameter to check.
          ** Keep dup_found as TRUE;
          */
        }
      }

      /* Final Check with dup found.  If we found a duplicate do an early
      ** return.  Otherwise, keep checking.
      */
      if(dup_found)
      {
        MSG_HIGH("Duplicate Message found!", 0, 0, 0);
        return dup_found;
      }
    }
    else /* No Msg ID field, this is an invalid message. */
    {
      MSG_ERROR("Invalid Message in duplicate detect", 0, 0, 0);
      return FALSE;
    }
  }

  /* If we got this far, a duplicate was not found. */
  return dup_found;
#endif 
} /* wms_msg_cdma_check_dups() */

/*=========================================================================
FUNCTION
  wms_msg_cdma_next_seq_num

DESCRIPTION
  This function returns the next SMS TL sequence number to be used for
  message origination, which increments by 1 each time, starting from 0.
  It goes back to 0 again when the max sequence number is reached.

DEPENDENCIES
  None

RETURN VALUE
  The next sequence number to be used.

SIDE EFFECTS
  Increment the internal sequence number.

=========================================================================*/
uint8  wms_msg_cdma_next_seq_num
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_s_ptr->tl_seq_num = (msg_s_ptr->tl_seq_num+1) % WMS_TL_SEQ_NUM_MAX;

  return msg_s_ptr->tl_seq_num;

} /* end of wms_msg_cdma_next_seq_num() */

/*=========================================================================
FUNCTION
  wms_msg_cdma_next_non_tl_seq_num

DESCRIPTION
  This function returns the next SMS NON TL sequence number to be used for
  message origination, which increments by 1 each time, starting from 0.
  It goes back to 0 again when the max sequence number is reached.

DEPENDENCIES
  None

RETURN VALUE
  The next sequence number to be used for message not requesting a tl ack.

SIDE EFFECTS
  Increment the internal sequence number.

=========================================================================*/
uint8  wms_msg_cdma_next_non_tl_seq_num
(
  void
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_s_ptr->non_tl_seq_num = (msg_s_ptr->non_tl_seq_num+1)%WMS_TL_SEQ_NUM_MAX;

  /* The bit wise OR will signify this is a message with no tl_ack requested */
  return msg_s_ptr->non_tl_seq_num | WMS_DUMMY_SEQ_NUM;

} /* end of wms_msg_cdma_next_non_tl_seq_num() */


/*===========================================================================
FUNCTION   wms_msg_cdma_map_mo_rec

DESCRIPTION
  Search the MO record queue for the record that is active and has a matching
  sequence number.

DEPENDENCIES
  None

RETURNS
  NULL - no match found and no free record found
  Non-NULL and state == IDLE: no match found but a free record found
  Non-NULL and state != IDLE: a match found

SIDE EFFECTS
  None
===========================================================================*/
wms_msg_cdma_message_record_type *wms_msg_cdma_map_mo_rec
(
  uint8  seq_num
)
{
  uint8                       i;
  wms_msg_cdma_message_record_type  * rec_ptr = NULL;

  for( i=0; i<WMS_MAX_MO_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mo_rec[i].state == WMS_MSG_STATE_IDLE )
    {
      /* found a free record */
      rec_ptr = & msg_s_ptr->cdma_mo_rec[i];

      /* Continue the search to see if we can find
      ** a matching sequence number that is in use. If not,
      ** we will return the pointer to the free record if
      ** we found in previously.
      */
    }
    else if( msg_s_ptr->cdma_mo_rec[i].seq_num == seq_num )
    {
      /* found the matching record */
      rec_ptr = & msg_s_ptr->cdma_mo_rec[i];
      break;
    }
  }

  /* done */
  return rec_ptr;

} /* wms_msg_cdma_map_mo_rec() */


/*
*/
/*===========================================================================
FUNCTION   wms_msg_cdma_map_mt_rec

DESCRIPTION
  Search the MT record queue for the record that is active and has a matching
  sequence number.

DEPENDENCIES
  None

RETURNS
  NULL - no match found and no free record found
  Non-NULL and state == IDLE: no match found but a free record found
  Non-NULL and state != IDLE: a match found

SIDE EFFECTS
  None
===========================================================================*/
wms_msg_cdma_message_record_type * wms_msg_cdma_map_mt_rec
(
  uint8 seq_num
)
{
  uint8                       i;
  wms_msg_cdma_message_record_type  * rec_ptr = NULL;

  for( i=0; i<WMS_MAX_MT_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mt_rec[i].state == WMS_MSG_STATE_IDLE )
    {
      /* found a free record */
      rec_ptr = & msg_s_ptr->cdma_mt_rec[i];
    }
    else if( msg_s_ptr->cdma_mt_rec[i].seq_num == seq_num )
    {
      /* found the matching record */
      rec_ptr = & msg_s_ptr->cdma_mt_rec[i];
      break;
    }
  }

  /* done */
  return rec_ptr;

} /* end of wms_map_mt_rec() */



/*===========================================================================
FUNCTION   wms_msg_cdma_clear_msg_rec

DESCRIPTION
  Clear a message record by changing its state to IDLE and clearing the timer.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Reset the record.
===========================================================================*/
void wms_msg_cdma_clear_msg_rec
(
  wms_msg_cdma_message_record_type * rec_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_s_ptr->mc_msg.is_valid = FALSE; 
  
  if( rec_ptr != NULL )
  {
    MSG_HIGH("clr msg rec with seq_num = %d (tid 0x%lx)",
             rec_ptr->seq_num, rec_ptr->user_data, 0);
    rec_ptr->state           = WMS_MSG_STATE_IDLE;
    rec_ptr->seq_num         = WMS_DUMMY_SEQ_NUM;
#ifdef CUST_EDITION
    clk_dereg(&rec_ptr->clk_timer);
#else
    (void) rex_clr_timer( & rec_ptr->timer );
#endif //#ifdef CUST_EDITION

    rec_ptr->retry_time_left = 0;
    rec_ptr->retry_wait_time = 0;
    if (rec_ptr->ota)
    {
      mem_free(&tmc_heap, (void*)rec_ptr->ota);
    }
    rec_ptr->ota             = NULL;
    rec_ptr->large_msg = FALSE;

    // If call is active,
    // AND If auto disconnect is not turned on,
    // AND If call was initiated by us
    // AND MO/MT queues are free
    if( dc_s_ptr->call_active && 
        !dc_s_ptr->auto_disc_active &&
        dc_s_ptr->wms_initiated_call &&
        !wms_msg_cdma_mo_busy () && 
        !wms_msg_cdma_mt_busy () )
    {
      MSG_HIGH ("WMS initiated traffic call, no more MO/MT recs, tear down traffic channel!", 0,0,0);
      // dc_s_ptr->wms_initiated_call will be reset after the call is actually disconnected
      if( WMS_OK_S != wms_dc_disconnect ( WMS_CLIENT_TYPE_INTERNAL,
                               NULL,
                               NULL ) )
      {
        // Ideally, this should not fail.  However, if it did, 
        // it would be re-tried the next time we are clearing the
        // records, and more importantly, in real networks, the
        // network itself would force and tear down the traffic
        // channel itself ...so no extra processing needed.
        MSG_ERROR ("wms_dc_disconnect failed!!", 0,0,0);
      }
    }
  }
  else
  {
    MSG_ERROR("Null rec_ptr!", 0,0,0);
  }

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

  /* done */
  return;

} /* end of wms_msg_cdma_clear_msg_rec() */



/*===========================================================================
FUNCTION wms_msg_cdma_ok_to_send_sms

DESCRIPTION
  Check if MC is ready to send an SMS message.

DEPENDENCIES
  None

RETURNS
  True: ready; False: not ready.

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_msg_cdma_ok_to_send_sms
(
  void
)
{

  return ( msg_s_ptr->mc_msg.is_valid == FALSE );

} /* end of wms_msg_cdma_ok_to_send_sms() */

/*===========================================================================
FUNCTION   wms_msg_choose_bearer

DESCRIPTION
  Choose which bearer to use for sending messages

DEPENDENCIES
  None

RETURNS
  Bearer to use

SIDE EFFECTS
  Applicable for 1x Only. Donot use with GW.

===========================================================================*/
wms_bearer_e_type wms_msg_choose_bearer
(
  void
)
{
  uint32                  i;
  wms_bearer_e_type       chosen_bearer    = WMS_BEARER_MAX;
  wms_bearer_e_type       preferred_bearer = WMS_BEARER_MAX;

  /* Find a preferred bearer and a potentially chosen bearer
  */
  for( i=0; i < WMS_BEARER_MAX; i++ )
  {
    if( WMS_BEARER_USAGE_PREFERRED == cfg_s_ptr->bearer_prefs[i] )
    {
      /* Record the Preferred bearer */
      preferred_bearer = (wms_bearer_e_type) i;
      MSG_HIGH("Found Preferred bearer: %d", preferred_bearer, 0,0);
    }

    if( WMS_BEARER_MAX == chosen_bearer &&
        WMS_BEARER_USAGE_ALLOWED == cfg_s_ptr->bearer_prefs[i] &&
        TRUE == cfg_s_ptr->active_bearers[i] )
    {
      /* Record the first found usable bearer */
      chosen_bearer = (wms_bearer_e_type) i;
      MSG_HIGH("Found first usable bearer: %d", chosen_bearer, 0,0);
    }
  } /* for loop */


  if( WMS_BEARER_MAX != preferred_bearer &&
      TRUE == cfg_s_ptr->active_bearers[preferred_bearer] )
  {
    /* If a bearer is preferred and it is active, use it */
    chosen_bearer = preferred_bearer;
    MSG_HIGH("Using the active Preferred bearer: %d", chosen_bearer,0,0);
  }
  else if( WMS_BEARER_MAX > cfg_s_ptr->last_used_bearer &&
           TRUE == cfg_s_ptr->active_bearers[cfg_s_ptr->last_used_bearer] )
  {
    /* If no preferred bearer is chosen, check if last used bearer is active */
    chosen_bearer = cfg_s_ptr->last_used_bearer;
    MSG_HIGH("Using last used and active bearer: %d", chosen_bearer, 0,0);
  }
  else if( WMS_BEARER_MAX == chosen_bearer )
  {
    /* Pick a default if nothing is available */
    if( WMS_BEARER_MAX != preferred_bearer )
    {
      chosen_bearer = preferred_bearer;
      MSG_HIGH("No usable bearer found; using the Preferred bearer: %d",chosen_bearer,0,0);
    }
    else
    {
#ifdef FEATURE_CDSMS
      chosen_bearer = WMS_BEARER_CDMA_1X;
#else
      chosen_bearer = WMS_BEARER_GW_CS;
#endif
    }
  }
  /* Otherwise, the first active and allowed bearer is the chosen bearer */

  MSG_ERROR("Chosen bearer = %d", chosen_bearer,0,0);

  cfg_s_ptr->last_used_bearer = chosen_bearer;

  return chosen_bearer;

} /* wms_msg_choose_bearer() */


/*===========================================================================
FUNCTION   wms_msg_cdma_send_sms_OTA

DESCRIPTION
  Send a raw SMS message to the MC task and update the local MC message info.

DEPENDENCIES
  None

RETURNS
  TRUE:   success
  FALSE:  failure

SIDE EFFECTS
  Update the local MC msg info buffer and reset the DTC idle timer.

===========================================================================*/
wms_status_e_type wms_msg_cdma_send_sms_OTA
(
  boolean                            is_ack,
  wms_msg_cdma_message_record_type   * rec_ptr,
  const wms_OTA_message_type         * OTA_ptr,
  const wms_address_s_type           * address_ptr
)
{
  wms_status_e_type  st = WMS_OK_S;

  wms_bearer_e_type  bearer;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Sending SMS OTA: len=%d, fmt=%d",
           OTA_ptr->data_len, OTA_ptr->format, 0 );

  if(rec_ptr == NULL || OTA_ptr == NULL || address_ptr == NULL)
  {
    MSG_HIGH("Null pointer: wms_msg_send_CDMA_OTA",0 ,0,0);
    st = WMS_NULL_PTR_S;
    return st;
  }

  if( ! wms_msg_cdma_ok_to_send_sms() )
  {
    MSG_MED("Send OTA: MC is not ready",0,0,0);
    st = WMS_NETWORK_NOT_READY_S;
  }
  else
  {
    bearer = wms_msg_choose_bearer();

    if( WMS_BEARER_CDMA_1X == bearer )
    {
      /* reset the idle timer if auto disconnect is enabled
      */
      if( dc_s_ptr->auto_disc_active == TRUE )
      {
        (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
      }
    }

    msg_s_ptr->mc_msg.is_valid = TRUE;
    msg_s_ptr->mc_msg.is_ack  = is_ack;
    msg_s_ptr->mc_msg.rec_ptr = rec_ptr;
    msg_s_ptr->mc_msg.bearer   = bearer;

#ifdef FEATURE_IWMSC
#error code not present
#else /* FEATURE_IWMSC */

#ifdef FEATURE_WMS_NO_CM_MSG2
    /* Old way to send SMS to CM/CP:
    */
    /* global cdma_wms_msg is declared in mccdma.h/.c */
    cdma_wms_msg.ota.format     = OTA_ptr->format;
    cdma_wms_msg.ota.data_len   = OTA_ptr->data_len;
    memcpy( (char *)cdma_wms_msg.ota.data,
            (char*) OTA_ptr->data,
            OTA_ptr->data_len );
    cdma_wms_msg.address.digit_mode = address_ptr->digit_mode;
    cdma_wms_msg.address.number_mode = address_ptr->number_mode;
    cdma_wms_msg.address.number_type = address_ptr->number_type;
    cdma_wms_msg.address.number_plan = address_ptr->number_plan;
    cdma_wms_msg.address.number_of_digits = address_ptr->number_of_digits;
    memcpy( cdma_wms_msg.address.digits,
            address_ptr->digits,
            address_ptr->number_of_digits );
	MSG_FATAL("wms_cm_sms_send_cdma_msg::::::::::::::111",0,0,0);
    wms_cm_sms_send_cdma_msg( (uint8*) & cdma_wms_msg );

#else /* FEATURE_WMS_NO_CM_MSG2 */

    /* New way to send SMS to CM:
    */
    wms_cm_sms_send_cdma_msg2( address_ptr, OTA_ptr, bearer );

#endif /* FEATURE_WMS_NO_CM_MSG2 */

#endif /* FEATURE_IWMSC */
  }

  /* done */
  return st;

} /* end of wms_msg_cdma_send_sms_OTA() */


/*===========================================================================
FUNCTION   wms_msg_cdma_map_status

DESCRIPTION
  Map the status code to the code that is to be transferred in the encoded
  message.

DEPENDENCIES
  None

RETURNS
  error class and TL status.

SIDE EFFECTS
  None
===========================================================================*/
void wms_msg_cdma_map_status
(
  wms_status_e_type       st, /* IN */
  wms_error_class_e_type  * error_class_ptr, /* OUT */
  wms_cdma_tl_status_e_type    * tl_status_ptr   /* OUT */
)
{

  if(error_class_ptr == NULL || tl_status_ptr == NULL )
  {
    MSG_HIGH("Null pointer: wms_msg_send_CDMA_OTA",0 ,0,0);
    return;
  }

  switch( st )
  {
    case WMS_OK_S:
      *error_class_ptr = WMS_ERROR_NONE;
      break;

    case WMS_OUT_OF_RESOURCES_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;
      break;

    case WMS_INVALID_TRANSACTION_ID_S:
    case WMS_INVALID_FORMAT_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_OTHER_TERMINAL_PROBLEM_S;
      break;

    case WMS_INVALID_PARM_SIZE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_UNEXPECTED_PARM_SIZE_S;
      break;

    case WMS_INVALID_USER_DATA_SIZE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_USER_DATA_SIZE_ERROR_S;
      break;

    case WMS_INVALID_PARM_VALUE_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_UNRECOGNIZED_PARM_VALUE_S;
      break;

    case WMS_MISSING_PARM_S:
      *error_class_ptr = WMS_ERROR_PERM;
      *tl_status_ptr = WMS_TL_MISSING_MANDATORY_PARM_S;
      break;

    case WMS_NETWORK_NOT_READY_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_NETWORK_FAILURE_S;
      break;

    case WMS_TERMINAL_BLOCKED_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_SMS_TERMINATION_DENIED_S;
      break;

    case WMS_TERMINAL_BUSY_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_DESTINATION_BUSY_S;
      break;
#ifdef CUST_EDITION
#ifdef FEATURE_CARRIER_TAIWAN_APBW    
    case WMS_UNSUPPORTED_S:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_INVALID_TELESERVICE_ID_S;
      break;
#endif      
#endif // #ifdef CUST_EDITION
    case WMS_GENERAL_ERROR_S:
    default:
      *error_class_ptr = WMS_ERROR_TEMP;
      *tl_status_ptr = WMS_TL_OTHER_GENERAL_PROBLEMS_S;
      break;
  }

  /* done */
  return;

} /* end of wms_msg_cdma_map_status() */


/*
*/
wms_status_e_type wms_msg_map_cmd_err_to_status
(
  wms_cmd_err_e_type   cmd_err
)
{
  wms_status_e_type  st;
  switch( cmd_err )
  {
    case WMS_CMD_ERR_NONE:
      st = WMS_OK_S;
      break;

    case WMS_CMD_ERR_NO_RESOURCE:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case WMS_CMD_ERR_MSG_MEMORY_FULL:
      st = WMS_OUT_OF_RESOURCES_S;
      break;

    case WMS_CMD_ERR_MSG_TRANSACTION_ID:
      st = WMS_INVALID_TRANSACTION_ID_S;
      break;

    case WMS_CMD_ERR_MSG_TPDU_TYPE:
      st = WMS_INVALID_TPDU_TYPE_S;
      break;

    case WMS_CMD_ERR_MSG_FORMAT:
      st = WMS_INVALID_FORMAT_S;
      break;

    case WMS_CMD_ERR_CLIENT_ID:

    case WMS_CMD_ERR_CFG_NV_WRITE:
    case WMS_CMD_ERR_CFG_NV_READ:
    case WMS_CMD_ERR_CFG_ROUTE:
    case WMS_CMD_ERR_CFG_MEM_STORE:
    case WMS_CMD_ERR_CFG_MSG_CLASS:

    case WMS_CMD_ERR_MSG_MEMORY_STORE:
    case WMS_CMD_ERR_MSG_INDEX:
    case WMS_CMD_ERR_MSG_EMPTY_MESSAGE:
    case WMS_CMD_ERR_MSG_TAG:
    case WMS_CMD_ERR_MSG_ENCODE:
    case WMS_CMD_ERR_MSG_DECODE:
    case WMS_CMD_ERR_MSG_SEND_MODE:
    case WMS_CMD_ERR_MSG_WRITE_MODE:

    case WMS_CMD_ERR_MSG_SIM_WRITE:
    case WMS_CMD_ERR_MSG_SIM_READ:
    case WMS_CMD_ERR_MSG_SIM_DELETE:
    case WMS_CMD_ERR_MSG_RUIM_WRITE:
    case WMS_CMD_ERR_MSG_RUIM_READ:
    case WMS_CMD_ERR_MSG_RUIM_DELETE:
    case WMS_CMD_ERR_MSG_NV_WRITE:
    case WMS_CMD_ERR_MSG_NV_READ:
    case WMS_CMD_ERR_MSG_NV_DELETE:

    case WMS_CMD_ERR_BC_BAD_PREF:
    case WMS_CMD_ERR_BC_CANNOT_ACTIVATE:
    case WMS_CMD_ERR_BC_CANNOT_ACTIVATE_ALL:
    case WMS_CMD_ERR_BC_NV_WRITE:
    case WMS_CMD_ERR_BC_BAD_SRV_ID:
    case WMS_CMD_ERR_BC_TABLE_FULL:
    case WMS_CMD_ERR_BC_DUPLICATE_SRV:

    case WMS_CMD_ERR_DC_BAD_STATE:
    case WMS_CMD_ERR_DC_BAD_CALL_ID:
    default:
      st = WMS_GENERAL_ERROR_S;
      break;
  }

  return st;

} /* wms_msg_map_cmd_err_to_status() */

/*===========================================================================
FUNCTION   wms_msg_get_next_unused_seq_num

DESCRIPTION
  Search the next unused sequence number starting with specified number.
  Note that the sequence number returned by wms_msg_cdma_next_seq_num()
  might be used already in the case when sending a large number of 
  messages that causes the sequence number to wrap around.

DEPENDENCIES
  None

RETURNS
  wms_status_e_type

SIDE EFFECTS
  None
===========================================================================*/
static wms_status_e_type wms_msg_get_next_unused_seq_num
(
  uint8  start_seq_num, 
  uint8* unused_seq_num_ptr
)
{

  /* find the next available seq_num */
  uint8               seq_num = start_seq_num;
  boolean             record_found = FALSE;
  wms_status_e_type   st = WMS_OK_S;
  wms_msg_cdma_message_record_type  *rec_ptr = NULL;
  
  if(NULL == unused_seq_num_ptr)
  {
    return WMS_NULL_PTR_S;
  }
  
  while( 1 )
  {
    rec_ptr = wms_msg_cdma_map_mo_rec( seq_num );
    if( rec_ptr == NULL )
    {
      MSG_HIGH("No free MO record available!",0,0,0);
      break;
    }
    else if( rec_ptr->state == WMS_MSG_STATE_IDLE )
    {
      /* found a free MO record */
      MSG_HIGH("MO seq=%d", seq_num, 0, 0 );
      *unused_seq_num_ptr = seq_num;
      record_found  = TRUE;
      break;
    }
    else
    {
      /* same seq_num in use   */
      /* increment the seq_num */
      seq_num = (seq_num + 1) % WMS_MAX_MO_MESSAGES;

      if( seq_num == start_seq_num )
      {
        /* We are back to the starting point */
        /* No more record available!         */
        MSG_HIGH("All seq_num in use!", 0,0,0);
        break;
      }
    } /* if */

  } /* while */
  

  if( record_found == FALSE )
  {
    st = WMS_OUT_OF_RESOURCES_S;
  }

  return st;
}

/*===========================================================================
FUNCTION   wms_msg_cdma_ack_msg_i

DESCRIPTION
  Send an ack message corresponding to the MT record.

DEPENDENCIES
  None

RETURNS
  Status of the ack.

SIDE EFFECTS
  Update the MT record's state.

===========================================================================*/
wms_status_e_type  wms_msg_cdma_ack_msg_i
(
  wms_msg_cdma_message_record_type       * rec_ptr,
  wms_error_class_e_type                   error_class,
  wms_cdma_tl_status_e_type                tl_status
)
{
  byte                           seq_num;
  wms_status_e_type              st = WMS_OK_S;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( rec_ptr == NULL )
  {
    MSG_ERROR("Null rec_ptr!",0,0,0);
    return WMS_NULL_PTR_S;  /* SHORT-RETURN */
  }

  MSG_HIGH("cdma_ack_msg_i: seq_n=%d,tid=0x%lx",
           rec_ptr->seq_num,rec_ptr->user_data,0);

  seq_num = rec_ptr->seq_num;

  if( rec_ptr->state == WMS_MSG_STATE_IDLE )
  {
    MSG_HIGH( "ack msg: seq %d not active!", seq_num, 0, 0 );
    st = WMS_INVALID_TRANSACTION_ID_S;
  }
  else
  {
    /* fill in TL msg structure
    */
    cdma_tl.tl_message_type          = WMS_TL_TYPE_ACK;
    cdma_tl.mask                     = WMS_MASK_TL_CAUSE_CODES;
    cdma_tl.cause_code.reply_seq_num = seq_num;
    cdma_tl.cause_code.error_class   = error_class;
    cdma_tl.cause_code.tl_status     = tl_status;

    // address should always be present
    cdma_tl.mask |= WMS_MASK_TL_ADDRESS;
    cdma_tl.address = rec_ptr->address;

    if( rec_ptr->subaddress.number_of_digits != 0 )
    {
      cdma_tl.mask |= WMS_MASK_TL_SUBADDRESS;
      cdma_tl.subaddress = rec_ptr->subaddress;
    }

    /* encode msg without BD data
    */
    cdma_tl.is_mo = TRUE;
    st = wms_ts_encode_CDMA_tl( & cdma_tl,
                                NULL,  /* No BD */
                                & ruim_sms.ota );

    if( st == WMS_OK_S )
    {
      msg_s_ptr->mc_msg.tid         = (wms_transaction_id_type) rec_ptr->user_data;
      msg_s_ptr->mc_msg.error_class = error_class;
      msg_s_ptr->mc_msg.tl_status   = tl_status;

      st = wms_msg_cdma_send_sms_OTA( TRUE,   /* is a TL ack */
                                      rec_ptr,
                                      & ruim_sms.ota,
                                      & cdma_tl.address );
    }

  } /* if state==IDLE - else */

  /* done */
  return st;

} /* end of wms_msg_cdma_ack_msg_i() */

/*===========================================================================
FUNCTION wms_msg_cdma_send_MO

DESCRIPTION
   It stores MO SMS into CDMA REC and sends MO SMS request to lower layer

RETURN VALUE
   wms_cmd_err_e_type

===========================================================================*/
wms_cmd_err_e_type wms_msg_cdma_send_MO
(
  boolean                       send_status_to_clients,
  wms_cmd_type                  *cmd_ptr,
  wms_send_mode_e_type          send_mode,
  const wms_client_msg_hdr_s_type *msg_hdr_ptr,
  const wms_tl_message_type           *tl_ptr,
  const wms_OTA_message_type          *ota_ptr
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;
  wms_status_e_type                 st;
  wms_msg_cdma_message_record_type  *rec_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("wms_msg_cdma_send_MO()",0,0,0);

  MSG_HIGH("snd msg: user_data=0x%lx", (long) cmd_ptr->hdr.user_data, 0, 0 );

  if(tl_ptr == NULL || ota_ptr == NULL)
  {
    MSG_HIGH("Null pointer: wms_msg_send_CDMA_MO",0 ,0,0);
    return WMS_CMD_ERR_NULL_PTR;
  }

  /* --------------------------------------------------------------- */
  /* ----------------- acquire a free msg record ------------------- */
  /* --------------------------------------------------------------- */

  if( !(tl_ptr->mask & WMS_MASK_TL_BEARER_REPLY_OPTION) )
  {
    /* ---------------- TL ack is NOT requested ------------- */
    MSG_HIGH("snd msg: No ACK requested", 0, 0, 0);
  }
  else
  {
    /* ---------------- TL ack is requested ------------- */
    MSG_HIGH("MO msg tl_ack requested", 0, 0, 0 );
  }

  rec_ptr = wms_msg_cdma_map_mo_rec( tl_ptr->bearer_reply_seq_num );

  if ( NULL == rec_ptr || rec_ptr->state != WMS_MSG_STATE_IDLE )
  {
    cmd_err = WMS_CMD_ERR_NO_RESOURCE;
  }
  /* --------------------------------------------------------------- */
  /* ----------------- Now we are ready to send the message -------- */
  /* --------------------------------------------------------------- */

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* ---------------fill in the rest of the record -------------- */
    if (rec_ptr)
    {
      rec_ptr->seq_num = tl_ptr->bearer_reply_seq_num;
      rec_ptr->send_status_to_clients = send_status_to_clients;
      rec_ptr->user_data    = cmd_ptr->hdr.user_data;
      rec_ptr->msg_hdr      = *msg_hdr_ptr;
      rec_ptr->send_mode    = send_mode;
      rec_ptr->client_id    = (wms_client_type_e_type) cmd_ptr->hdr.client_id;

      rec_ptr->address      = tl_ptr->address;
      rec_ptr->subaddress   = tl_ptr->subaddress;

      /* Initialize the Prev Report to Timer Expired Status */
      rec_ptr->prev_report.user_data = rec_ptr->user_data;
      rec_ptr->prev_report.client_id = rec_ptr->client_id;
      rec_ptr->prev_report.report_status = WMS_RPT_MO_RETRY_PERIOD_EXPIRED;
      rec_ptr->prev_report.alpha_id.len = 0;
      rec_ptr->prev_report.alpha_id.data = NULL;
      rec_ptr->prev_report.message_mode = WMS_MESSAGE_MODE_CDMA;
      
      rec_ptr->ota = (wms_OTA_message_type*)mem_malloc(&tmc_heap, sizeof(wms_OTA_message_type));
      if (rec_ptr->ota != NULL)
      {
        memcpy ((void*)rec_ptr->ota, (void*)ota_ptr, sizeof(wms_OTA_message_type));

        /* Set the retry_time_left */
        rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
        rec_ptr->retry_wait_time = 0;
        rec_ptr->large_msg = FALSE;
        
#ifdef CUST_EDITION	 
#ifndef FEATURE_OEMOMH 
        if (g_mochannel == WMS_MO_ONLY_TRAFFIC)
        {
            rec_ptr->large_msg = TRUE;
        }
        else
        {
            rec_ptr->large_msg = FALSE;
        }        
        
        /* Set the Global Retry Timer */
        //wms_set_retry_timer();
        clk_dereg(&rec_ptr->clk_timer);
        clk_reg2(&rec_ptr->clk_timer, 
                 wms_mo_timer,
                 CLK_MS_PER_SEC,
                 0,
                 FALSE,
                 rec_ptr);
        
        /* Request to set up DC if call is not already active */
        if ((!dc_s_ptr->call_active) && 
            (!dc_s_ptr->bInTC) && 
            (g_mochannel == WMS_MO_ONLY_TRAFFIC))
        {
           MSG_HIGH("WMS initiating DC... %d", dc_s_ptr->default_so_from_nv,0,0);
#ifdef FEATURE_VERSION_ESIA
           st = wms_dc_connect(WMS_CLIENT_TYPE_INTERNAL,
                    NULL,
                    NULL,
                    WMS_DC_SO_6/*dc_s_ptr->default_so_from_nv*/);
#else
           st = wms_dc_connect(WMS_CLIENT_TYPE_INTERNAL,
                    NULL,
                    NULL,
                    dc_s_ptr->default_so_from_nv);
#endif
           if (st == WMS_OK_S)
           {
               dc_s_ptr->wms_initiated_call = TRUE;
               rec_ptr->retry_time_left = WMS_MAX_RETRY_PERIOD;
               
               // 注意: 业务信道一旦建立函数 wms_msg_auto_dc_send 会被调用, 故这里给最大等待时间
               rec_ptr->retry_wait_time = WMS_MAX_RETRY_PERIOD;
               rec_ptr->state = WMS_MSG_STATE_QUEUED;
           }
           else
           {
               /* Assign the new error code */
               cmd_ptr->cmd.mc_mo_status.status = st;

               /* No retry for this msg any more */
               rec_ptr->retry_time_left = -1;
               
               cmd_err = WMS_CMD_ERR_BUSY;
           }
        }
        else
        {
#endif // #ifndef FEATURE_OEMOMH
#endif /*CUST_EDITION*/
            if (msg_s_ptr->mo_retry_period > 0)
            { 
#ifndef CUST_EDITION
                /* Set the Global Retry Timer */
                wms_set_retry_timer();
#else
                clk_dereg(&rec_ptr->clk_timer);
                clk_reg2(&rec_ptr->clk_timer, 
                         wms_mo_timer,
                         CLK_MS_PER_SEC,
                         0,
                         FALSE,
                         rec_ptr);
#endif
            }

            /* ------------- Now send the msg out -------------- */

            st = wms_msg_cdma_send_sms_OTA( FALSE,  /* not a TL Ack msg */
                                            rec_ptr,
                                            ota_ptr,
                                            & tl_ptr->address );

            if( st == WMS_OK_S )
            {
              MSG_HIGH("MO msg State: L2 in Progress",0,0,0);
              rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
            }
            else if( st == WMS_NETWORK_NOT_READY_S )
            {
              /* reset the interval timer if there is enough time left from the
              ** retry period.
              */
              if((rec_ptr->retry_time_left - (sint31)msg_s_ptr->mo_retry_interval) > 0)
              {
                /* Set the retry_wait_time */
                rec_ptr->retry_wait_time = msg_s_ptr->mo_retry_interval;
                rec_ptr->state = WMS_MSG_STATE_QUEUED;
              }
              else
              {
                /* Declare Failure */
                cmd_err = WMS_CMD_ERR_BUSY;
              }
            }
            else
            {
              /* Declare Failure */
              cmd_err = WMS_CMD_ERR_NULL_PTR;
            }
#ifdef CUST_EDITION
#ifndef FEATURE_OEMOMH
        }
#endif
#endif
      }
      else
      {
        MSG_ERROR("rec_ptr->ota Malloc Failed!",0,0,0);
        cmd_err = WMS_CMD_ERR_NO_RESOURCE;
      }
    }
  }
  return cmd_err;

} /* wms_msg_cdma_send_MO() */

/*===========================================================================
FUNCTION wms_msg_cdma_send_mo_channel

DESCRIPTION
   It is wrapper around wms_msg_cdma_send_MO fuction to decide either send 
   CDMA MO SMS over Access Channel or Traffic Channel
   
RETURN VALUE
   wms_cmd_err_e_type

===========================================================================*/
wms_cmd_err_e_type wms_msg_cdma_send_mo_channel
(
  boolean                          send_status_to_clients,
  wms_cmd_type                     *cmd_ptr,
  wms_send_mode_e_type             send_mode,
  const wms_client_msg_hdr_s_type  *msg_hdr_ptr,
  const wms_tl_message_type        *tl_ptr,
  const wms_OTA_message_type       *ota_ptr
)
{
   wms_cmd_err_e_type cmd_err = WMS_CMD_ERR_NONE;

   if( (FALSE == dc_s_ptr->is_access_channel_enabled) && 
       (FALSE == dc_s_ptr->is_traffic_channel_enabled) )
   {
      /* This should never happen */
      MSG_HIGH("Access and Traffic both 0s never happen", 0, 0, 0);
      return WMS_CMD_ERR_UNSUPPORTED;
   }
   else if ((TRUE == dc_s_ptr->is_access_channel_enabled) && 
            (FALSE == dc_s_ptr->is_traffic_channel_enabled)) 
   {
      MSG_HIGH("Sending on Access Channel Only", 0, 0, 0);

      cmd_err = wms_msg_cdma_send_MO(
                        send_status_to_clients,
                        cmd_ptr,
                        send_mode,
                        msg_hdr_ptr,
                        tl_ptr,
                        ota_ptr );

      return cmd_err;
   }
   else if ((FALSE == dc_s_ptr->is_access_channel_enabled) && 
            (TRUE == dc_s_ptr->is_traffic_channel_enabled)) 
   {     
     wms_status_e_type                 st;
     wms_msg_cdma_message_record_type  *rec_ptr;

     /* Send of Traffic Channel */
     /* Save the msg first */
     MSG_HIGH("Sending on Traffic Channel Only", 0, 0, 0);
     MSG_HIGH("Save Msg first and Open Traffic Channel", 0, 0, 0);
      
     if(tl_ptr == NULL || ota_ptr == NULL)
     {
       MSG_HIGH("Null pointer: wms_msg_send_CDMA_MO",0 ,0,0);
       return WMS_CMD_ERR_NULL_PTR;
     }
          
     /* --------------------------------------------------------------- */
     /* ----------------- acquire a free msg record ------------------- */
     /* --------------------------------------------------------------- */
   
     if( !(tl_ptr->mask & WMS_MASK_TL_BEARER_REPLY_OPTION) )
     {
       /* ---------------- TL ack is NOT requested ------------- */
       MSG_HIGH("snd msg: No ACK requested", 0, 0, 0);
     }
     else
     {
       /* ---------------- TL ack is requested ------------- */
       MSG_HIGH("MO msg tl_ack requested", 0, 0, 0 );
     }
   
     rec_ptr = wms_msg_cdma_map_mo_rec( tl_ptr->bearer_reply_seq_num );
   
     if ( NULL == rec_ptr || rec_ptr->state != WMS_MSG_STATE_IDLE )
     { 
        return WMS_CMD_ERR_NO_RESOURCE;
     } 
   
     /* Save the message in REC */
     rec_ptr->seq_num = tl_ptr->bearer_reply_seq_num;
     rec_ptr->send_status_to_clients = send_status_to_clients;
     rec_ptr->user_data    = cmd_ptr->hdr.user_data;
     rec_ptr->msg_hdr      = *msg_hdr_ptr;
     rec_ptr->send_mode    = send_mode;
     rec_ptr->client_id    = (wms_client_type_e_type) cmd_ptr->hdr.client_id;        
     rec_ptr->address      = tl_ptr->address;
     rec_ptr->subaddress   = tl_ptr->subaddress;
     rec_ptr->large_msg    = TRUE;
     rec_ptr->state        = WMS_MSG_STATE_QUEUED;
         
     rec_ptr->ota = (wms_OTA_message_type*)mem_malloc(&tmc_heap, sizeof(wms_OTA_message_type));

     if (rec_ptr->ota != NULL)
     {
        memcpy ((void*)rec_ptr->ota, (void*)ota_ptr, sizeof(wms_OTA_message_type));
     }
     else
     {
       MSG_ERROR("rec_ptr->ota Malloc Failed!",0,0,0);        
       return WMS_CMD_ERR_NO_RESOURCE;
     }

     MSG_HIGH("Msg Saved in REC, SEQ Num %d  ", rec_ptr->seq_num,0,0);

     /* Request to set up DC if call is not already active */
     if ( !dc_s_ptr->call_active )
     {
       MSG_HIGH("WMS initiating DC... ", 0,0,0);

       st = wms_dc_connect( WMS_CLIENT_TYPE_INTERNAL,
                NULL,
                NULL,
                dc_s_ptr->default_so_from_nv );
      
       if(WMS_OK_S == st)
       {
           dc_s_ptr->wms_initiated_call = TRUE;
           /* Fall through in order to start the retries for large msg */
           rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
           /* Wait till RETRY Period to hear back from CM */
           rec_ptr->retry_wait_time = 0;

           if (msg_s_ptr->mo_retry_period > 0)
           { 
              /* Set the Global Retry Timer */
#ifndef CUST_EDITION
              wms_set_retry_timer();
#else
              clk_dereg(&rec_ptr->clk_timer);
              clk_reg2(&rec_ptr->clk_timer, 
                       wms_mo_timer,
                       CLK_MS_PER_SEC,
                       0,
                       FALSE,
                       rec_ptr);
#endif

           }
       }
       else
       {
          /* Clear the MO msg record */
          wms_msg_cdma_clear_msg_rec( rec_ptr );
          MSG_HIGH("DC origination failed", 0,0,0);
          return WMS_CMD_ERR_NO_RESOURCE;
       }
     }
     else
     { 
       MSG_HIGH("Traffic Channel is Open already ", 0,0,0);
       rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
       rec_ptr->retry_wait_time = 1;

       if (msg_s_ptr->mo_retry_period > 0)
       { 
          /* Set the Global Retry Timer */
#ifndef CUST_EDITION
          wms_set_retry_timer();
#else
          clk_dereg(&rec_ptr->clk_timer);
          clk_reg2(&rec_ptr->clk_timer, 
                   wms_mo_timer,
                   CLK_MS_PER_SEC,
                   0,
                   FALSE,
                   rec_ptr);
#endif
       }
       return cmd_err;
     }
   }
   else if ((TRUE == dc_s_ptr->is_access_channel_enabled) && 
            (TRUE == dc_s_ptr->is_traffic_channel_enabled)) 
   {
     /* Send on Access First and if Fail try on Trafffic Channel */
     cmd_err = wms_msg_cdma_send_MO(
                        send_status_to_clients,
                        cmd_ptr,
                        send_mode,
                        msg_hdr_ptr,
                        tl_ptr,
                        ota_ptr );     
   }

   return cmd_err;

} /* wms_msg_cdma_send_mo_channel*/



#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*
*/
wms_cmd_err_e_type wms_msg_send_from_memory_store
(
  wms_cmd_type    *cmd_ptr,
  uint32          rec_index
)
{
  uint32                            i = cmd_ptr->cmd.msg_send.message.msg_hdr.index;
#ifdef FEATURE_CDSMS
  wms_message_number_type           message_number = 0;
#endif /* FEATURE_CDSMS */
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->cmd.msg_send.message.msg_hdr.mem_store )
  {

#ifdef FEATURE_CDSMS
#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if( i >= cfg_s_ptr->ruim_max_slots )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if( ! WMS_IS_MO( (wms_message_tag_e_type) cfg_s_ptr->ruim_tags[i] ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }
      else if( wms_ruim_read_sms( i, ruim_data ) != TRUE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if( wms_ts_cdma_unpack_ruim_sms( ruim_data, & ruim_sms ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( wms_ts_decode_CDMA_OTA( & ruim_sms.ota, & cdma_tl ) != WMS_OK_S )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
      }
      else
      {
        /* Don't change msg id if it is CANCELLATION or USER_ACK:
        */
        if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
#ifdef CUST_EDITION			
#if defined(FEATURE_QMA)
            cdma_tl.teleservice != WMS_TELESERVICE_QMA_WPUSH &&
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
            cdma_tl.teleservice != WMS_TELESERVICE_WPUSH &&
#endif 
#endif /*CUST_EDITION*/
            cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
        {
          /* use the next message number
          */
          message_number = wms_cfg_get_ruim_next_message_number();
          cdma_tl.cl_bd.message_id.id_number = message_number;
        }

        /* use the next sequence number
        */
        if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
        {
          cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_seq_num();
        }
        else
        {
          cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
        }

        if( WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num, 
                                                        &cdma_tl.bearer_reply_seq_num) )
        {
           cmd_err = WMS_CMD_ERR_NO_RESOURCE;
        }
        
#ifdef CM_API_PLUS_DIALING
        if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
        {
          MSG_ERROR("wms_msg_convert_plus_address Failed!",0,0,0);
        }
#endif /* CM_API_PLUS_DIALING */

        /* Encode the message with new info
        */
        cdma_tl.is_mo = TRUE;
        if( wms_ts_encode_CDMA_OTA( & cdma_tl, & ruim_sms.ota )
              != WMS_OK_S )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        /* Update the contents of cmd_ptr w/new message_id */
        else if(wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                          &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S)
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else
        {
          /* Change the tag to NOT_SENT since we are going to send/resend it
          */
          ruim_sms.status          = WMS_TAG_MO_NOT_SENT;
          if( wms_ts_cdma_pack_ruim_sms( & ruim_sms, ruim_data ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
        }
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_ruim_write_sms( i, ruim_data, FALSE ) != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
        }
        else
        {
#ifdef CUST_EDITION			
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
          wms_cacheinfolist_updatenodetag(WMS_MEMORY_STORE_RUIM, i, ruim_sms.status);
#endif  
#endif /*CUST_EDITION*/        
          cmd_err = wms_msg_cdma_send_mo_channel(
          TRUE,
                            cmd_ptr,
                            cmd_ptr->cmd.msg_send.send_mode,
                            & cmd_ptr->cmd.msg_send.message.msg_hdr,
                            & cdma_tl,
                            & ruim_sms.ota );
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = 0;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
      break; //WMS_MEMORY_STORE_RUIM
#endif /* FEATURE_CDSMS_RUIM */

    case WMS_MEMORY_STORE_NV_CDMA:
      if( i >= cfg_s_ptr->nv_cdma_max_slots )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if( ! WMS_IS_MO( cfg_s_ptr->nv_cdma_tags[i]) )
      {
        cmd_err = WMS_CMD_ERR_MSG_TAG;
      }
      else if( wms_nv_read_cdma_sms( i, ruim_data ) != TRUE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else if( ruim_data[0] != (uint8) WMS_FORMAT_CDMA )
      {
        cmd_err = WMS_CMD_ERR_MSG_FORMAT;
      }
      else
      {
        /* Layout of SMS message in NV. */
        /* 0: format                    */
        /* 1: tag/status                */
        /* 2: len                       */
        /* 3+: data                     */
        ruim_sms.ota.format      = WMS_FORMAT_CDMA;
        ruim_sms.status          = (uint8) WMS_TAG_MO_NOT_SENT;
        ruim_sms.ota.data_len    = MIN ((uint32)ruim_data[2], WMS_MAX_LEN);
        memcpy( (void*)ruim_sms.ota.data, ruim_data+3, ruim_sms.ota.data_len );
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_ts_decode_CDMA_OTA( & ruim_sms.ota, & cdma_tl ) != WMS_OK_S )
        {
          cmd_err = WMS_CMD_ERR_MSG_DECODE;
        }
        else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
        {
          cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
        }
        else
        {
          /* Don't change msg id if it is CANCELLATION or USER_ACK:
          */
          if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
#ifdef CUST_EDITION			  
#if defined(FEATURE_QMA)
              cdma_tl.teleservice != WMS_TELESERVICE_QMA_WPUSH &&
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
              cdma_tl.teleservice != WMS_TELESERVICE_WPUSH &&
#endif 
#endif /*CUST_EDITION*/
              cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
          {
            /* use the next message number
            */
            message_number = wms_cfg_get_ruim_next_message_number();
            cdma_tl.cl_bd.message_id.id_number = message_number;
          }

          /* use the next sequence number if TL Ack is requested
          */
          if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
          {
            cdma_tl.bearer_reply_seq_num = (uint8) wms_msg_cdma_next_seq_num();
          }
          else
          {
            cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
          }

          if (WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num, 
                                                          &cdma_tl.bearer_reply_seq_num))
          {
             cmd_err = WMS_CMD_ERR_NO_RESOURCE;
          }
                    
#ifdef CM_API_PLUS_DIALING
          if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
          {
            MSG_ERROR("wms_msg_convert_plus_address Failed!",0,0,0);
          }
#endif /* CM_API_PLUS_DIALING */

          /* Encode the message with new info
          */
          cdma_tl.is_mo = TRUE;
          if( wms_ts_encode_CDMA_OTA( & cdma_tl, & ruim_sms.ota )
                != WMS_OK_S )
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          /* Update the contents of cmd_ptr w/new message_id */
          else if(wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                            &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S)
          {
            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
          }
          else
          {
            memcpy( ruim_data+3,
                    (void*)ruim_sms.ota.data,
                    ruim_sms.ota.data_len );
          }
        }
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_nv_write_cdma_sms( i, ruim_data ) != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
        else
        {
#ifdef CUST_EDITION			
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
          wms_cacheinfolist_updatenodetag(WMS_MEMORY_STORE_NV_CDMA, i, ruim_sms.status);
#endif          
#endif /*CUST_EDITION*/
          cmd_err = wms_msg_cdma_send_mo_channel(
          TRUE,
                            cmd_ptr,
                            cmd_ptr->cmd.msg_send.send_mode,
                            & cmd_ptr->cmd.msg_send.message.msg_hdr,
                            & cdma_tl,
                            & ruim_sms.ota );
        }
      }

      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );

      // Populate and Dispatch Send Event Callback
      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = 0;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
      break; //WMS_MEMORY_STORE_NV_CDMA
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d",
                 cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store, 0, 0 );
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
      
      // Populate and Dispatch Command Callback
      wms_client_cmd_status( cmd_ptr, cmd_err );
      break;
  } // switch

  return cmd_err;

} /* wms_msg_send_from_memory_store() */



#ifdef FEATURE_CDSMS
/*
*/
wms_cmd_err_e_type wms_msg_cdma_send_client_message
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;
  wms_cdma_message_s_type   * cl_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* convert client msg to internal TL format
  */
  cl_ptr = & cmd_ptr->cmd.msg_send.message.u.cdma_message;
  wms_ts_convert_cl2tl( cl_ptr, & cdma_tl );

  if( wms_ts_decode_CDMA_bd( & cl_ptr->raw_ts, FALSE, FALSE, & cdma_tl.cl_bd ) != WMS_OK_S )
  {
    cmd_err = WMS_CMD_ERR_MSG_DECODE;
  }
  else if( ! WMS_IS_MO_CDMA_MSG_TYPE( cdma_tl.cl_bd.message_id.type ) )
  {
    cmd_err = WMS_CMD_ERR_MSG_TPDU_TYPE;
  }
  else
  {
    /* Don't change msg id if it is CANCELLATION or USER_ACK:
    */
    if( cdma_tl.cl_bd.message_id.type == WMS_BD_TYPE_SUBMIT &&
#ifdef CUST_EDITION		
        cmd_ptr->hdr.client_id != WMS_CLIENT_TYPE_WMS_APP &&  
#if defined(FEATURE_QMA)
        cdma_tl.teleservice != WMS_TELESERVICE_QMA_WPUSH &&
#elif defined(FEATURE_CARRIER_CHINA_TELCOM)
        cdma_tl.teleservice != WMS_TELESERVICE_WPUSH &&
#endif 
#endif /*CUST_EDITION*/
        cdma_tl.teleservice           != WMS_TELESERVICE_WAP )
    {
      /* use the next message number
      */
      cdma_tl.cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();
    }

    /* use the next sequence number if TL Ack is requested
    */
    if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
    {
      cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_seq_num();
    }
    else
    {
      cdma_tl.bearer_reply_seq_num = wms_msg_cdma_next_non_tl_seq_num();
    }

    if( WMS_OK_S != wms_msg_get_next_unused_seq_num(cdma_tl.bearer_reply_seq_num, 
                                                    &cdma_tl.bearer_reply_seq_num) )
    {
       cmd_err = WMS_CMD_ERR_NO_RESOURCE;
    }

#ifdef CM_API_PLUS_DIALING
    if (FALSE == wms_msg_convert_plus_address(&cdma_tl.address))
    {
       MSG_ERROR("wms_msg_convert_plus_address Failed!",0,0,0);
    }
#endif /* CM_API_PLUS_DIALING */

    /* Encode the message with new info
    */
    cdma_tl.is_mo = TRUE;
    if( wms_ts_encode_CDMA_OTA( & cdma_tl,  & ruim_sms.ota ) != WMS_OK_S )
    {
      cmd_err = WMS_CMD_ERR_MSG_ENCODE;
    }
    /* Update the contents of cmd_ptr w/new message_id */
    else if(wms_ts_encode_bearer_data(&cdma_tl.cl_bd,
                                      &cmd_ptr->cmd.msg_send.message.u.cdma_message.raw_ts) != WMS_OK_S)
    {
      cmd_err = WMS_CMD_ERR_MSG_ENCODE;
    }
    else
    {
      cmd_err = wms_msg_cdma_send_mo_channel(
            TRUE,
                        cmd_ptr,
                        cmd_ptr->cmd.msg_send.send_mode,
                        & cmd_ptr->cmd.msg_send.message.msg_hdr,
                        & cdma_tl,
                        & ruim_sms.ota );
    }
  }

  return cmd_err;

} /* wms_msg_cdma_send_client_message() */
#endif /* FEATURE_CDSMS */

/*
*/
wms_cmd_err_e_type wms_msg_send_client_message
(
  wms_cmd_type    *cmd_ptr,
  uint32          rec_index
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


#ifdef FEATURE_CDSMS
  if( cmd_ptr->cmd.msg_send.message.msg_hdr.message_mode
                   == WMS_MESSAGE_MODE_CDMA )
  {
    cmd_err = wms_msg_cdma_send_client_message( cmd_ptr );
    return cmd_err;
  }
#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  return cmd_err;

} /* wms_msg_send_client_message() */




#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/* This function checks if the Client has the appropriate privileges to Ack the message
   Rules:
   client_id ==  WMS_CLIENT_TYPE_INTERNAL  --> Go Ahead and Ack it
   mt_record_client_id ==  WMS_CLIENT_TYPE_MAX
     If (Primary Client Present) && (client_id = Primary_Client) --> Ack it
     else Ack it.
   client_id ==  mt_record_client_id  --> Ack it
*/
boolean wms_msg_check_valid_client_ack
(
  wms_client_id_type        client_id,
  wms_client_id_type        mt_record_client_id
)
{
  /* Internal Client */
  if (client_id == WMS_CLIENT_TYPE_INTERNAL)
  {
    return TRUE;
  }

  if (mt_record_client_id != WMS_CLIENT_TYPE_MAX)
  {
    /* Exclusive Message */
    if (mt_record_client_id == client_id)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    /* Common Message */
    if (cfg_s_ptr->primary_client != WMS_CLIENT_TYPE_MAX)
    {
      /* Primary Client Initialized */
      /* Only primary Client can ACK it */
      if (cfg_s_ptr->primary_client == client_id)
      {
        return TRUE;
      }
      else
      {
        return FALSE;
      }
    }
    else
    {
      /* Primary Client Not Initialized...Anyone can ACK it */
      return TRUE;
    }
  }
}

/*
*/
void wms_msg_send_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( TRUE == cfg_s_ptr->lpm )
  {
    cmd_err = WMS_CMD_ERR_MSG_LPM;
  }
  else
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  if( cmd_ptr->cmd.msg_send.send_mode == WMS_SEND_MODE_MEMORY_STORE )
  {
    cmd_err = wms_msg_send_from_memory_store( cmd_ptr, i );
  }
  else if( cmd_ptr->cmd.msg_send.send_mode == WMS_SEND_MODE_CLIENT_MESSAGE )
  {
    cmd_err = wms_msg_send_client_message( cmd_ptr, i );
    wms_client_cmd_status( cmd_ptr, cmd_err );
    if( cmd_err == WMS_CMD_ERR_NONE )
    {
#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */
      {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
        msg_event_info.status_info.send_mode  = cmd_ptr->cmd.msg_send.send_mode;
        msg_event_info.status_info.message    = cmd_ptr->cmd.msg_send.message;
        msg_event_info.status_info.alpha_id.len = 0;
        msg_event_info.status_info.alpha_id.data = 0;
        (void)wms_msg_event_notify( WMS_MSG_EVENT_SEND, &msg_event_info );
      }
    }
  }
  else
  {
    cmd_err = WMS_CMD_ERR_MSG_SEND_MODE;
    wms_client_cmd_status( cmd_ptr, cmd_err );
  }

  /* done */
  return;

} /* wms_msg_send_proc() */



#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION   wms_msg_awi_ack

DESCRIPTION
  Send an AWI SMS ack message.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void wms_msg_awi_ack
(
  boolean                   final_ack,
  boolean                   bf_bit,
  wms_error_class_e_type    error_class,
  wms_cdma_tl_status_e_type      tl_status
)
{
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
} /* wms_msg_awi_ack() */



wms_cmd_err_e_type wms_msg_cdma_ack_proc
(
  wms_cmd_type      *cmd_ptr
)
{
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;
  boolean               record_found;
  uint32                i;
  wms_ack_info_s_type   * ack_ptr = & cmd_ptr->cmd.msg_ack.ack_info;
  wms_msg_cdma_message_record_type  * rec_ptr;
  wms_status_e_type     st = WMS_OK_S;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CDMA Ack processing
  */

  /* search the mt_q for the matching tid
  */
  record_found = FALSE;

  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    rec_ptr = & msg_s_ptr->cdma_mt_rec[i];

    if( rec_ptr->state     != WMS_MSG_STATE_IDLE  &&
        rec_ptr->user_data == (void*) (ack_ptr->transaction_id) )
    {
      record_found = TRUE;

      /* add the user data to the mt rec ptr */
      rec_ptr->mt_rec_user_data = cmd_ptr->hdr.user_data;
      break;
    }
  }

  if( record_found == FALSE )
  {
    MSG_HIGH( "Invalid TID 0x%lx!", ack_ptr->transaction_id, 0, 0 );
    cmd_err = WMS_CMD_ERR_MSG_TRANSACTION_ID;
  }
  else if (!wms_msg_check_valid_client_ack( cmd_ptr->hdr.client_id, rec_ptr->client_id ))
  {
    MSG_HIGH("Unprivileged Client %d Acking the Message", cmd_ptr->hdr.client_id, 0, 0);
    cmd_err = WMS_CMD_ERR_CLIENT_ID;
  }
  else if( rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK )
  {
    MSG_HIGH( "ack msg: found tid=%d, seq_num=%d",
              ack_ptr->transaction_id, rec_ptr->seq_num, 0 );

    /* clear the mt ack timer
    */
#ifdef CUST_EDITION
    clk_dereg(&rec_ptr->clk_timer);
#else
    (void) rex_clr_timer( & rec_ptr->timer );    
#endif //#ifdef CUST_EDITION
    
    /* Copy Ack Data to Record */
    rec_ptr->error_class     = ack_ptr->u.cdma.error_class;
    rec_ptr->tl_status       = ack_ptr->u.cdma.tl_status;

    /* Set the retry_time_left  - Irrespective of Retry Period = 0 */
    rec_ptr->retry_time_left = WMS_MAX_ACK_RETRY_PERIOD;
    rec_ptr->retry_wait_time = 0;

    /* Assign Client ID of Acking Client */
    rec_ptr->client_id       = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    
    /* Set the Global Retry Timer */
#ifdef CUST_EDITION
   clk_reg2(&rec_ptr->clk_timer, 
             wms_mt_timer,
             CLK_MS_PER_SEC,
             0,
             FALSE,
             rec_ptr);
#else
    wms_set_retry_timer();
#endif

    st = wms_msg_cdma_ack_msg_i( rec_ptr,
                                 ack_ptr->u.cdma.error_class,
                                 ack_ptr->u.cdma.tl_status );

    if( st == WMS_OK_S )
    {
      MSG_HIGH("ack msg: L2 timer set",0,0,0);
      rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
    }
    else if( st == WMS_NETWORK_NOT_READY_S )
    {
      rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
      rec_ptr->state           = WMS_MSG_STATE_QUEUED;
    }
    else /* other error */
    {
      MSG_HIGH("ack msg error",0,0,0);
      wms_msg_cdma_clear_msg_rec( rec_ptr );
    } /* if st - else */
  }
  else
  {
    ERR("Unexpected msg state %d", rec_ptr->state, 0, 0 );
    cmd_err = WMS_CMD_ERR_BUSY;
  }

  /* done */
  return cmd_err;

} /* wms_msg_cdma_ack_proc() */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================
FUNCTION wms_msg_total_memory_full

DESCRIPTION
  Function to check if all the Memory Stores are full. 

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_msg_total_memory_full
(
  void
)
{
#ifdef FEATURE_GWSMS
#error code not present
#else 
  return TRUE;
#endif /* FEATURE_GWSMS */
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* #ifdef FEATURE_GWSMS */

static wms_cmd_err_e_type wms_msg_do_client_ack
(
  wms_cmd_type          * cmd_ptr
)
{
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;
  wms_ack_info_s_type       * ack_ptr = & cmd_ptr->cmd.msg_ack.ack_info;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_CDSMS

  if( ack_ptr->message_mode == WMS_MESSAGE_MODE_CDMA )
  {
    cmd_err = wms_msg_cdma_ack_proc( cmd_ptr );
    return cmd_err;
  }

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  return cmd_err;

} /* wms_msg_do_client_ack() */


/*
*/
void wms_msg_ack_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type    cmd_err;

  if( TRUE == cfg_s_ptr->lpm )
  {
    cmd_err = WMS_CMD_ERR_MSG_LPM;
  }
  else
  {
    cmd_err = wms_msg_do_client_ack( cmd_ptr );
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  /* If the client is INTERNAL, don't send any events to clients */
  if( cmd_err == WMS_CMD_ERR_NONE &&
      cmd_ptr->hdr.client_id != (uint8) WMS_CLIENT_TYPE_INTERNAL )
  {
    msg_event_info.ack_info = cmd_ptr->cmd.msg_ack.ack_info;

    (void)wms_msg_event_notify( WMS_MSG_EVENT_ACK, &msg_event_info );
  }

  // done
  return;

} /* wms_msg_ack_proc() */


/*
*/
void wms_msg_read_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;
  wms_memory_store_e_type     mem_store;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  i = cmd_ptr->cmd.msg_read.index;


  mem_store = cmd_ptr->cmd.msg_read.mem_store;

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if (i >= cfg_s_ptr->ruim_max_slots )
       {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR("Invalid message index %d",
                 cmd_ptr->cmd.msg_read.index, 0, 0 );
      }
      else if(cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR("Empty  message %d",
                 cmd_ptr->cmd.msg_read.index, 0, 0 );
      }
      else if( wms_ruim_read_sms( i, ruim_data ) != TRUE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if( wms_ts_cdma_unpack_ruim_sms( ruim_data , & ruim_sms ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else if( wms_ts_decode_CDMA_tl( & ruim_sms.ota, & cdma_tl, & cdma_raw_bd )
               != WMS_OK_S )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      else
      {
        wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );
        cdma_cl.raw_ts = cdma_raw_bd;

        msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
        msg_event_info.status_info.message.msg_hdr.index     = i;
        msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
        msg_event_info.status_info.message.msg_hdr.tag       = (wms_message_tag_e_type)ruim_sms.status;
        msg_event_info.status_info.message.u.cdma_message    = cdma_cl;
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */


    case WMS_MEMORY_STORE_NV_CDMA:
      if  (i >= cfg_s_ptr->nv_cdma_max_slots)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR("Invalid message index %d",cmd_ptr->cmd.msg_read.index,0,0 );
        break;
      }
      if(cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR("Empty  message %d", cmd_ptr->cmd.msg_read.index, 0, 0 );
        break;
      }

      if( wms_nv_read_cdma_sms( i, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        /* Layout of SMS message in NV. */
        /* 0: format                    */
        /* 1: tag/status                */
        /* 2: len                       */
        /* 3+: data                     */
        ruim_sms.ota.format   = (wms_format_e_type) ruim_data[0];
        ruim_sms.status       = ruim_data[1];
        ruim_sms.ota.data_len = ruim_data[2];
        memcpy( (void*)ruim_sms.ota.data, ruim_data+3, ruim_sms.ota.data_len );

        if( wms_ts_decode_CDMA_tl( & ruim_sms.ota, & cdma_tl, & cdma_raw_bd )
            == WMS_OK_S )
        {
          wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );
          cdma_cl.raw_ts = cdma_raw_bd;
#ifdef CUST_EDITION
          {
             int nPos = 2+ruim_sms.ota.data_len;
             
             cdma_cl.concat_8.msg_ref = ruim_data[nPos+1];
             cdma_cl.concat_8.total_sm = ruim_data[nPos+2];
             cdma_cl.concat_8.seq_num = ruim_data[nPos+3];
          }
#endif
          msg_event_info.status_info.message.msg_hdr.message_mode =
                                                        WMS_MESSAGE_MODE_CDMA;
          msg_event_info.status_info.message.msg_hdr.index      = i;
          msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
#ifdef CUST_EDITION
          msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) (ruim_sms.status);
		  if(msg_event_info.status_info.message.msg_hdr.tag == WMS_TAG_MO_DRAFT)
		  {
		  	msg_event_info.status_info.message.msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
		  }
#else
          msg_event_info.status_info.message.msg_hdr.tag = (wms_message_tag_e_type) (ruim_sms.status & 0x7);
#endif
          msg_event_info.status_info.message.u.cdma_message     = cdma_cl;
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_DECODE;
        }
      }
      break;

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d",
                 cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store, 0, 0 );
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_READ, &msg_event_info );
  }

  // done
  return;

} /* wms_msg_read_proc() */

/*=========================================================================
FUNCTION
  wms_msg_is_mo_retry_allowed

DESCRIPTION
  Function to check if Retry is Allowed or not based on the Error.

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_is_mo_retry_allowed
(
  wms_message_mode_e_type        submit_mode,
  const wms_submit_report_info_s_type  *report_info_ptr
)
{
  boolean do_retry = FALSE;

  switch(report_info_ptr->report_status)
  {
    case WMS_RPT_LL_ERROR:
    case WMS_RPT_GENERAL_ERROR:
    case WMS_RPT_OUT_OF_RESOURCES:
    case WMS_RPT_NETWORK_NOT_READY:
    case WMS_RPT_PHONE_NOT_READY:
    case WMS_RPT_NO_ACK:
    case WMS_RPT_NOT_ALLOWED_IN_AMPS:
    case WMS_RPT_ACCESS_BLOCK:
    case WMS_RPT_SMR_TR1M_TIMEOUT:
    case WMS_RPT_SMR_TR2M_TIMEOUT:
    case WMS_RPT_SMR_NO_RESOURCES:
    case WMS_RPT_ACCESS_TOO_LARGE: /* Set up SMS call */
    case WMS_RPT_SIP_TEMP_ERROR:
    case WMS_RPT_RP_ERROR:
    case WMS_RPT_CP_ERROR:
    case WMS_RPT_NO_RESPONSE_FROM_NETWORK:
      do_retry = TRUE;
      MSG_HIGH("Retry Allowed: %d",report_info_ptr->report_status,0,0);
      break;
    case WMS_RPT_MO_CONTROL_DISALLOW:
    case WMS_RPT_MO_CONTROL_ERROR:
    case WMS_RPT_CDMA_TL_ERROR:
    case WMS_RPT_DC_TOO_LARGE:
    case WMS_RPT_CANNOT_SEND_BROADCAST:
    default:
      do_retry = FALSE;
      MSG_HIGH("Retry disallowed: %d",report_info_ptr->report_status,0,0);
      break;
  }

  return do_retry;

} /* wms_msg_is_mo_retry_allowed() */

wms_cmd_err_e_type wms_msg_do_write
(
  wms_write_mode_e_type         write_mode, /* IN */
  wms_client_message_s_type     * msg_ptr   /* IN-OUT */
)
{
  uint32                              i =0;
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(msg_ptr == NULL)
  {
    cmd_err = WMS_CMD_ERR_NULL_PTR;
    MSG_HIGH("wms_msg_do_write: NULL ptr",0,0,0);
    return cmd_err;
  }

  /* Check for Valid Message Tag */
  if( msg_ptr->msg_hdr.tag != WMS_TAG_MT_READ &&
      msg_ptr->msg_hdr.tag != WMS_TAG_MT_NOT_READ &&
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_NOT_SENT &&
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT &&
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_RECEIVED &&
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_STORED &&
#ifdef CUST_EDITION
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_DRAFT &&
      msg_ptr->msg_hdr.tag != WMS_TAG_PHRASE &&
      msg_ptr->msg_hdr.tag != WMS_TAG_RESERVE &&
      msg_ptr->msg_hdr.tag != WMS_TAG_RSVFAILED &&
#endif
      msg_ptr->msg_hdr.tag != WMS_TAG_MO_SENT_ST_STORED
    )
  {
    MSG_ERROR("Invalid Tag %d in wms_msg_do_write", msg_ptr->msg_hdr.tag, 0, 0);
    return WMS_CMD_ERR_MSG_TAG;
  }
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE_USELIST)  && defined(FEATURE_AUTOREPLACE)
  if (cfg_s_ptr->client_memory_full && 
      cfg_s_ptr->autoreplace &&
      write_mode != WMS_WRITE_MODE_REPLACE &&
      msg_ptr->msg_hdr.tag == WMS_TAG_MT_NOT_READ)
  {
    extern void wms_get_oldestMTmsgIdx(wms_memory_store_e_type *mem_store, wms_message_index_type *index);
    
    write_mode = WMS_WRITE_MODE_REPLACE;
    wms_get_oldestMTmsgIdx(&(msg_ptr->msg_hdr.mem_store), &(msg_ptr->msg_hdr.index));
  }
#endif 
#endif //#ifdef CUST_EDITION
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


  switch( msg_ptr->msg_hdr.mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
#ifdef CUST_EDITION		
      if ((msg_ptr->msg_hdr.tag == WMS_TAG_MO_DRAFT) || 
          (msg_ptr->msg_hdr.tag == WMS_TAG_PHRASE) ||
          (msg_ptr->msg_hdr.tag == WMS_TAG_RESERVE) ||
          (msg_ptr->msg_hdr.tag == WMS_TAG_RSVFAILED))
      {// 此类消息不允许写入卡
        MSG_FATAL("Invalid Tag %d in wms_msg_do_write", msg_ptr->msg_hdr.tag, 0, 0);
        return WMS_CMD_ERR_MSG_TAG;
      }
#endif /*CUST_EDITION*/
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        for( i=0; i<cfg_s_ptr->ruim_max_slots; i++ )
        {
          if( cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE )
          {
            /* found a free slot */
            break;
          }
        }

        if( i == cfg_s_ptr->ruim_max_slots )
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          wms_cfg_do_memory_full( WMS_MEMORY_STORE_RUIM );
        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        if( i >= cfg_s_ptr->ruim_max_slots )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* prepare for the message to be written to NV
        */
        wms_ts_convert_cl2tl( & msg_ptr->u.cdma_message,
                              & cdma_tl );

        if( wms_ts_encode_CDMA_tl( & cdma_tl,
                                   & msg_ptr->u.cdma_message.raw_ts,
                                   & ruim_sms.ota ) == WMS_OK_S )
        {
          //add by yangdecai
          MSG_FATAL("::::::::add by yangdecai ::%d",msg_ptr->u.cdma_message.concat_8.total_sm,0,0);
		  MSG_FATAL("cfg_s_ptr->ruim_max_slots=%d,i=%d",cfg_s_ptr->ruim_max_slots,i,0);
          if((cfg_s_ptr->ruim_max_slots-i)<msg_ptr->u.cdma_message.concat_8.total_sm)
          {
          		cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          		wms_cfg_do_memory_full( WMS_MEMORY_STORE_RUIM );
          }
          //add by yangdecai  end
          if(cmd_err == WMS_CMD_ERR_NONE)
	      {
	          ruim_sms.status = (uint8) msg_ptr->msg_hdr.tag;
	          if( wms_ts_cdma_pack_ruim_sms( & ruim_sms, ruim_data ) == FALSE )
	          {
	            cmd_err = WMS_CMD_ERR_MSG_ENCODE;
	          }
#if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT))
	          /* Check if this is a prl update, if so another read must be invoked
	          ** to get the correct user data from the card.
	          */
	          else if(wms_ts_decode(&msg_ptr->u.cdma_message.raw_ts,
	                                &client_ts) != WMS_OK_S)
	          {
	            cmd_err = WMS_CMD_ERR_MSG_DECODE;
	          }
	          else
	          {
	            if((client_ts.u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE) &&
	               (client_ts.u.cdma.display_mode == WMS_DISPLAY_MODE_RESERVED) &&
	               (client_ts.u.cdma.download_mode == WMS_DOWNLOAD_MODE_UPDATE_PRL)&&
	               cmd_err == WMS_CMD_ERR_NONE)
	            {
	              /* This message contains a PRL Update, set the appropriate flag
	              ** when doing the write.
	              */
	              MSG_FATAL("WMS_DOWNLOAD_MODE_UPDATE_PRL %d",i,0,0);
	              if(!(wms_ruim_write_sms(i, ruim_data, TRUE)))
	              {
	                cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
	              }
#ifdef CUST_EDITION				  
	              // 亚太宽频粉红色卡更新成功后提示字符串不完整
#ifdef FEATURE_CARRIER_TAIWAN_APBW
	              else
	              {
	                extern boolean ChkUpdate(uint8 *ruimdata);
	                
	                if (ChkUpdate(ruim_data))
	                {
	                   (void)wms_ruim_write_sms(i, ruim_data, FALSE);
	                }
	              }
#endif
#endif /*CUST_EDITION*/
	            }
	            else
	            {
	              /* not a PRL update, do a regular write */
	              if(!(wms_ruim_write_sms(i, ruim_data, FALSE)))
	              {
	                cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
	              }
	            }
	          }

	          if(cmd_err == WMS_CMD_ERR_NONE)
#else
	          else if( wms_ruim_write_sms( i, ruim_data, FALSE ) != TRUE )
	          {
	            cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
	          }
	          else
#endif /* defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT) */
	          {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
	              /* Add new Message Information Cache */
	              wms_cfg_update_msg_info_cache(msg_ptr->msg_hdr.tag,
	                                  WMS_MEMORY_STORE_RUIM,
	                                  i,
	                                  ruim_data,
	                                  (uint8)cfg_s_ptr->ruim_sms_rec_len);
#endif            
#endif // #ifdef CUST_EDITION
	            /* RUIM write was successful, update message list
	            */
	            cfg_s_ptr->ruim_tags[i] = (wms_message_tag_e_type)ruim_sms.status;
	            msg_ptr->msg_hdr.index  = i;

	            /* Update the duplicate detection info cache if this is a MT msg. */
		  if( wms_ts_decode_CDMA_bd( & msg_ptr->u.cdma_message.raw_ts, FALSE, FALSE,
                             & cdma_tl.cl_bd) != WMS_OK_S )
			{
			return FALSE;
			}
	            if(WMS_IS_MT(cfg_s_ptr->ruim_tags[i]))
	            {
	              if(wms_ts_decode_bearer_data(&msg_ptr->u.cdma_message.raw_ts,
	                                           &cdma_tl.cl_bd) != WMS_OK_S)
	              {
	                MSG_ERROR("Decode CDMA_bd failed", 0, 0, 0);
	                cmd_err = WMS_CMD_ERR_MSG_DECODE;
	              }
	              /* Check if this message is already in the cache */
	              else if(wms_msg_cdma_check_dups(&cdma_tl) == FALSE)
	              {
	                /* Update the duplicate detection cache */
	                wms_cfg_update_dup_info_cache(msg_ptr->msg_hdr.mem_store,
	                                              i,
	                                              &cdma_tl);
	              }
	            }
	          }
          }
		  else
		  {
		  	  break;
		  }
        }
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */


    case WMS_MEMORY_STORE_NV_CDMA:
	{
	  int nMin, nMax;
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
#ifdef CUST_EDITION		  

        switch (msg_ptr->msg_hdr.tag)
        {
            // 收件箱
            case WMS_TAG_MT_NOT_READ:
            case WMS_TAG_MT_READ:
                if ((msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_VMN_95) ||
                    (msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_MWI))
                {// 语音短信
                    nMin = VOICE_INDEX;
                    nMax = VOICE_INDEX;
                }
                else
                {// 一般短信
                    nMin = IN_START;
                    nMax = IN_END;
                }
                break;
                
            // 发件箱
            case WMS_TAG_MO_SENT:
            case WMS_TAG_MO_NOT_SENT:
                nMin = OUT_START;
                nMax = OUT_END;
                break;
                
            // 常用语
            case WMS_TAG_PHRASE:
                nMin = PHRASE_START+MAX_OEMTEMPLATES;
                nMax = PHRASE_END;
                break;
                
            // 草稿
            case WMS_TAG_MO_DRAFT:
                nMin = DRAFT_START;
                nMax = DRAFT_END;
                break;
                
            // 预约
            case WMS_TAG_RESERVE:
            case WMS_TAG_RSVFAILED:
                nMin = RESERVE_START;
                nMax = RESERVE_END;
                break;
            
            default:
                MSG_ERROR("Invalid Tag %d in wms_msg_do_write", msg_ptr->msg_hdr.tag, 0, 0);
                return WMS_CMD_ERR_MSG_TAG;
        }
	
        for (i=nMin; i<=nMax; i++)
#else
        for( i=0; i<cfg_s_ptr->nv_cdma_max_slots; i++ )
#endif /*CUST_EDITION*/	
        {
          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            /* found a free slot */
            break;
          }
        }
#ifdef CUST_EDITION
        if (i > nMax)
#else
        if( i == cfg_s_ptr->nv_cdma_max_slots )
#endif //#ifdef CUST_EDITION
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          wms_cfg_do_memory_full( WMS_MEMORY_STORE_NV_CDMA );

        }
      } /* if INSERT */
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        i = msg_ptr->msg_hdr.index;
        if( i >= cfg_s_ptr->nv_cdma_max_slots )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        /* prepare for the message to be written to NV
        */
        wms_ts_convert_cl2tl( & msg_ptr->u.cdma_message,
                              & cdma_tl );

        if( wms_ts_encode_CDMA_tl( & cdma_tl,
                                   & msg_ptr->u.cdma_message.raw_ts,
                                   & ruim_sms.ota ) == WMS_OK_S )
        {
          /* Layout of SMS message in NV. */
          /* 0: format                    */
          /* 1: tag/status                */
          /* 2: len                       */
          /* 3+: data                     */
		  //add by yangdecai
		  MSG_FATAL("::::::::add by yangdecai ::%d",msg_ptr->u.cdma_message.concat_8.total_sm,0,0);
		  MSG_FATAL("::::::::::::::::::nMax=%d,i=%d",nMax,i,0);
		  if((nMax-i)<msg_ptr->u.cdma_message.concat_8.total_sm)
		  	{
		  		 MSG_FATAL("NV_CDMA_MAX_SLOTS is full.......................",0,0,0);
		  		 cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          		 wms_cfg_do_memory_full( WMS_MEMORY_STORE_NV_CDMA );
		  	}
		  if(cmd_err == WMS_CMD_ERR_NONE)
		  {
	          ruim_data[0] = (uint8) msg_ptr->u.cdma_message.raw_ts.format;
	          ruim_data[1] = (uint8)msg_ptr->msg_hdr.tag;
	          ruim_data[2] = (uint8)ruim_sms.ota.data_len;
	          memcpy( ruim_data+3, (void*)ruim_sms.ota.data, ruim_sms.ota.data_len );
#ifdef CUST_EDITION
	          {
	             int nPos = 2+ruim_sms.ota.data_len;
	             ruim_data[nPos+1] = msg_ptr->u.cdma_message.concat_8.msg_ref;
	             ruim_data[nPos+2] = msg_ptr->u.cdma_message.concat_8.total_sm;
	             ruim_data[nPos+3] = msg_ptr->u.cdma_message.concat_8.seq_num;
	          }
#endif
	          if(write_mode == WMS_WRITE_MODE_INSERT && cmd_err == WMS_CMD_ERR_NONE)
	          {
	            if( wms_nv_write_cdma_sms( i, ruim_data ) == FALSE )
	            {
	              cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
	            }
	            else
	            {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
	              /* Update Message Information Cache */
	              wms_cfg_update_msg_info_cache(msg_ptr->msg_hdr.tag,
	                                  WMS_MEMORY_STORE_NV_CDMA,
	                                  i,
	                                  ruim_data,
	                                  WMS_MAX_LEN);
#endif                                  
#endif
	              /* NV write was successful, update message list
	              */
	              cfg_s_ptr->nv_cdma_tags[i]  = msg_ptr->msg_hdr.tag;
	              msg_ptr->msg_hdr.index      = i;
  if( wms_ts_decode_CDMA_bd( & msg_ptr->u.cdma_message.raw_ts, FALSE, FALSE,
                             & cdma_tl.cl_bd) != WMS_OK_S )
  {
    return FALSE;
  }
	              /* Update the duplicate detection info cache if this is a MT msg. */
	              if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
	              {
	                if(wms_ts_decode_bearer_data(&msg_ptr->u.cdma_message.raw_ts,
	                                             &cdma_tl.cl_bd) != WMS_OK_S)
	                {
	                  MSG_ERROR("Decode CDMA_bd failed", 0, 0, 0);
	                  cmd_err = WMS_CMD_ERR_MSG_DECODE;
	                }
	                /* Check if this message is already in the cache */
	                else if(wms_msg_cdma_check_dups(&cdma_tl) == FALSE)
	                {
	                  /* Update the duplicate detection cache */
	                  wms_cfg_update_dup_info_cache(msg_ptr->msg_hdr.mem_store,
	                                                i,
	                                                &cdma_tl);
	                }
	              }
	            }
	          }
	          else if (write_mode == WMS_WRITE_MODE_REPLACE)
	          {
	            if( wms_nv_replace_cdma_sms( i, ruim_data ) == FALSE )
	            {
	               cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
	            }
	            else
	            {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
	              /* Add new Message Information Cache */
	              wms_cfg_update_msg_info_cache(msg_ptr->msg_hdr.tag,
	                                  WMS_MEMORY_STORE_NV_CDMA,
	                                  i,
	                                  ruim_data,
	                                  WMS_MAX_LEN);
#endif              
#endif // #ifdef CUST_EDITION
	              /* NV write was successful, update message list
	              */
	              cfg_s_ptr->nv_cdma_tags[i]  = msg_ptr->msg_hdr.tag;
	              msg_ptr->msg_hdr.index      = i;
	            }
	        }
        }
	    else 	
		{
			break;
		}
        }
        else
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }

        wms_cfg_cdma_update_voice_mail_index( (wms_format_e_type) ruim_data[0],   /* format */
                                              (wms_message_tag_e_type) ruim_data[1],   /* tag */
                                              ruim_data[2],   /* len */
                                              ruim_data+3,    /* data */
                                              i );
      }
	  }
      break;

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d",
                 msg_ptr->msg_hdr.mem_store, 0, 0 );
      break;
  } /* switch mem_store */

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    /* send out message list event */
    wms_cfg_do_message_list( msg_ptr->msg_hdr.mem_store, WMS_TAG_NONE );
    if( write_mode == WMS_WRITE_MODE_INSERT )
    {
      wms_cfg_do_memory_status( msg_ptr->msg_hdr.mem_store, WMS_TAG_NONE );
    }
  }

  /* done
  */
  return cmd_err;

} /* wms_msg_do_write() */


/*
*/
void wms_msg_write_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type                cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MT_READ &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MT_NOT_READ &&
#ifdef CUST_EDITION
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_RESERVE &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_RSVFAILED &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_DRAFT &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_PHRASE &&
#endif
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_NOT_SENT &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_RECEIVED &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_NOT_STORED &&
      cmd_ptr->cmd.msg_write.message.msg_hdr.tag != WMS_TAG_MO_SENT_ST_STORED
    )
  {
    cmd_err = WMS_CMD_ERR_MSG_TAG;
    wms_client_cmd_status( cmd_ptr, cmd_err );
    MSG_FATAL("Invalid Tag %d in wms_msg_write_proc", cmd_ptr->cmd.msg_write.message.msg_hdr.tag, 0, 0);
    return;
  }

  cmd_err = wms_msg_do_write( cmd_ptr->cmd.msg_write.write_mode,
                                & cmd_ptr->cmd.msg_write.message );

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.write_mode = cmd_ptr->cmd.msg_write.write_mode;
    msg_event_info.status_info.message    = cmd_ptr->cmd.msg_write.message;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_WRITE, &msg_event_info );
  }

  /* done */
  return;

} /* wms_msg_write_proc() */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*
*/
void wms_msg_delete_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i = cmd_ptr->cmd.msg_delete.index;
  wms_memory_store_e_type     mem_store = cmd_ptr->cmd.msg_delete.mem_store;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  switch( mem_store )
  {

#ifdef FEATURE_CDSMS


#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if (i >=cfg_s_ptr->ruim_max_slots )
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR("Invalid message index %d",
                 cmd_ptr->cmd.msg_delete.index, 0, 0 );
      }
      else if(cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR("Empty  message %d",
                 cmd_ptr->cmd.msg_delete.index, 0, 0 );
      }
      else if( wms_ruim_delete_sms( i ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
      }
      else
      {
        /* Update duplicate detection list */
        if(WMS_IS_MT(cfg_s_ptr->ruim_tags[i]))
        {
          /* Update the duplicate detection info cache. Mark the location as
          ** empty by simply setting the mask to 0.
          */
          wms_cfg_delete_dup_info_cache(mem_store, i);
        }

        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = cfg_s_ptr->ruim_tags[i];
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
        {
            uint8 data = cfg_s_ptr->ruim_tags[i];
            
            /* Delet old Message Information Cache */
            wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                      WMS_MEMORY_STORE_RUIM,
                      i,
                      &data,
                      1);
        }
#endif
#endif
        /* RUIM delete was successful, update message list
        */
        cfg_s_ptr->ruim_tags[i] = WMS_TAG_NONE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS_RUIM */

    case WMS_MEMORY_STORE_NV_CDMA:
      /* Clear out the record
      */
      if  (i >= cfg_s_ptr->nv_cdma_max_slots)
      {
        cmd_err =  WMS_CMD_ERR_MSG_INDEX;
        MSG_ERROR("Invalid message index %d",
                    cmd_ptr->cmd.msg_delete.index, 0, 0 );
        break;
      }
      if(cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE)
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR("Empty  message %d",
                    cmd_ptr->cmd.msg_delete.index, 0, 0 );
        break;
      }

          /* Layout of SMS message in NV. */
          /* 0: format                    */
          /* 1: tag/status                */
          /* 2: len                       */
          /* 3+: data                     */

      ruim_data[1] =  (uint8)WMS_TAG_NONE;

      if( wms_nv_delete_cdma_sms( i, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
      }
      else
      {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
        uint8 data = cfg_s_ptr->nv_cdma_tags[i];
        
        /* Delet old Message Information Cache */
        wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                  WMS_MEMORY_STORE_NV_CDMA,
                  i,
                  &data,
                  1);
#endif
#endif
        /* Update duplicate detection list */
        if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
        {
          /* Update the duplicate detection info cache. Mark the location as
          ** empty by simply setting the mask to 0.
          */
          wms_cfg_delete_dup_info_cache(mem_store, i);
        }

        /* Update Delete Message Event with Correct Tag */
        msg_event_info.status_info.message.msg_hdr.tag = cfg_s_ptr->nv_cdma_tags[i];

        /* NV write was successful, update message list
        */
        cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
        if( cfg_s_ptr->cdma_voice_mail_index == i )
        {
#ifndef CUST_EDITION
          cfg_s_ptr->cdma_voice_mail_index   = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
#endif
          cfg_s_ptr->cdma_voice_mail_active  = FALSE;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d", mem_store, 0, 0 );
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type)cmd_ptr->hdr.client_id;

    msg_event_info.status_info.message.msg_hdr.index = i;
    msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_message_list( mem_store, WMS_TAG_NONE );
    wms_cfg_do_memory_status( mem_store, WMS_TAG_NONE );

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  /* done */
  return;

} /* wms_msg_delete_proc() */

#ifdef CUST_EDITION	
void wms_msg_delete_box_proc(wms_cmd_type    *cmd_ptr)
{
    wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
    uint32                      i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    switch (cmd_ptr->cmd.msg_delete_box.box_deltype)
    {
#ifdef FEATURE_CDSMS
        case WMS_BOXES_DEL_ALL:
        case WMS_INBOX_DEL_ALL:
#ifdef FEATURE_CDSMS_RUIM
        case WMS_INBOX_DEL_RUIM:
            for (i=0; i < cfg_s_ptr->ruim_max_slots; i++)
            {
                if (WMS_IS_MT(cfg_s_ptr->ruim_tags[i]))
                {
                    if (wms_ruim_delete_sms(i) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
                    }
                    else
                    {
                        /* Update duplicate detection list */
                        wms_cfg_delete_dup_info_cache(WMS_MEMORY_STORE_RUIM, i);
                        
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        {
                            uint8 data = cfg_s_ptr->ruim_tags[i];
                            
                            /* Delet old Message Information Cache */
                            wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                      WMS_MEMORY_STORE_RUIM,
                                      i,
                                      &data,
                                      1);
                        }
#endif
                        
                        /* RUIM delete was successful, update message list */
                        cfg_s_ptr->ruim_tags[i] = WMS_TAG_NONE;
                    }
                }
                else
                {
                    continue; /* skip slots */
                }
            }
            
            if (cmd_err == WMS_CMD_ERR_NONE)
            {
                // send out CFG message list event
                wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
            }
            if (cmd_ptr->cmd.msg_delete_box.box_deltype == WMS_INBOX_DEL_RUIM)
            {
                msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                break;
            }
#endif /* FEATURE_CDSMS_RUIM */
        case WMS_INBOX_DEL_PHONE:
            for (i=IN_START; i <= IN_END; i++)
            {
                if (WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
                {
                    ruim_data[1] =  (uint8)WMS_TAG_NONE;
                    
                    if (wms_nv_delete_cdma_sms(i, ruim_data) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
                    }
                    else
                    {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        uint8 data = cfg_s_ptr->nv_cdma_tags[i];
                        
                        /* Delet old Message Information Cache */
                        wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                  WMS_MEMORY_STORE_NV_CDMA,
                                  i,
                                  &data,
                                  1);
#endif

                        /* Update duplicate detection list */
                        wms_cfg_delete_dup_info_cache(WMS_MEMORY_STORE_NV_CDMA, i);
                        
                        /* NV write was successful, update message list */
                        cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
                        
                    }
                }
                else
                {
                    continue; /* skip slots */
                }
            }
            
            if (cmd_ptr->cmd.msg_delete_box.box_deltype != WMS_BOXES_DEL_ALL)
            {
                msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                break;
            }
                
        case WMS_OUTBOX_DEL_ALL:
            for (i=OUT_START; i <= OUT_END; i++)
            {
                if ((cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_MO_SENT) ||
                    (cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_MO_NOT_SENT))
                {
                    ruim_data[1] =  (uint8)WMS_TAG_NONE;
                    
                    if (wms_nv_delete_cdma_sms(i, ruim_data) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
                    }
                    else
                    {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        uint8 data = cfg_s_ptr->nv_cdma_tags[i];
                        
                        /* Delet old Message Information Cache */
                        wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                  WMS_MEMORY_STORE_NV_CDMA,
                                  i,
                                  &data,
                                  1);
#endif

                        /* NV write was successful, update message list */
                        cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
                    }
                }
                else
                {
                    continue; /* skip slots */
                }
            }
            
            if (cmd_ptr->cmd.msg_delete_box.box_deltype != WMS_BOXES_DEL_ALL)
            {
                // 删除卡上 MO 消息
#ifdef FEATURE_CDSMS_RUIM
                for (i=0; i < cfg_s_ptr->ruim_max_slots; i++)
                {
                    if ((cfg_s_ptr->ruim_tags[i] == WMS_TAG_MO_SENT) ||
                        (cfg_s_ptr->ruim_tags[i] == WMS_TAG_MO_NOT_SENT))
                    {
                        if (wms_ruim_delete_sms(i) == FALSE)
                        {
                            cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
                        }
                        else
                        {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                            {
                                uint8 data = cfg_s_ptr->ruim_tags[i];
                                
                                /* Delet old Message Information Cache */
                                wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                          WMS_MEMORY_STORE_RUIM,
                                          i,
                                          &data,
                                          1);
                            }
#endif
                            
                            /* RUIM delete was successful, update message list */
                            cfg_s_ptr->ruim_tags[i] = WMS_TAG_NONE;
                        }
                    }
                    else
                    {
                        continue; /* skip slots */
                    }
                }
                
                if (cmd_err == WMS_CMD_ERR_NONE)
                {
                    // send out CFG message list event
                    wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                    wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                }
#endif /* FEATURE_CDSMS_RUIM */
                
                msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                break;
            }
            
        case WMS_DRAFT_DEL_ALL:
            for (i=DRAFT_START; i <= DRAFT_END; i++)
            {
                if (cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_MO_DRAFT)
                {
                    ruim_data[1] =  (uint8)WMS_TAG_NONE;
                    
                    if (wms_nv_delete_cdma_sms(i, ruim_data) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
                    }
                    else
                    {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        /* Delet old Message Information Cache */
                        {
                            uint8 data = cfg_s_ptr->nv_cdma_tags[i];
                            
                            /* Delet old Message Information Cache */
                            wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                      WMS_MEMORY_STORE_NV_CDMA,
                                      i,
                                      &data,
                                      1);
                        }
#endif

                        /* NV write was successful, update message list */
                        cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
                    }
                }
                else
                {
                    continue; /* skip slots */
                }
            }
            
            msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
            break;
            
        case WMS_RESERVE_DEL_ALL:
        case WMS_RSVFAILED_DEL_ALL:
        case WMS_RSVANDRSVFAILED_DEL_ALL:
            {
                wms_message_tag_e_type delTag;
                
                if (cmd_ptr->cmd.msg_delete_box.box_deltype == WMS_RESERVE_DEL_ALL)
                {
                    delTag = WMS_TAG_RESERVE;
                }
                else
                {
                    delTag = WMS_TAG_RSVFAILED;
                }
                
                for (i=RESERVE_START; i <= RESERVE_END; i++)
                {
                    if (cfg_s_ptr->nv_cdma_tags[i] == delTag || 
                        cmd_ptr->cmd.msg_delete_box.box_deltype == WMS_RSVANDRSVFAILED_DEL_ALL)
                    {
                        ruim_data[1] =  (uint8)WMS_TAG_NONE;
                        
                        if (wms_nv_delete_cdma_sms(i, ruim_data) == FALSE)
                        {
                            cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
                        }
                        else
                        {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                            /* Delet old Message Information Cache */
                            {
                                uint8 data = cfg_s_ptr->nv_cdma_tags[i];
                                
                                /* Delet old Message Information Cache */
                                wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                          WMS_MEMORY_STORE_NV_CDMA,
                                          i,
                                          &data,
                                          1);
                            }
#endif

                            /* NV write was successful, update message list */
                            cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
                        }
                    }
                    else
                    {
                        continue; /* skip slots */
                    }
                }
            }
            
            msg_event_info.status_info.message.msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
            break;

#endif /* FEATURE_CDSMS */

        default:
            cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
            MSG_ERROR("Invalid box del type %d", cmd_ptr->cmd.msg_delete_box.box_deltype, 0, 0 );
            break;
    } /* switch mem_store */

    wms_client_cmd_status( cmd_ptr, cmd_err );
    
    if (cmd_err == WMS_CMD_ERR_NONE)
    {
        msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
        msg_event_info.status_info.client_id  =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
        (void)wms_msg_event_notify(WMS_CMD_MSG_DELETE_BOX, &msg_event_info);
        
        if (cmd_ptr->cmd.msg_delete_box.box_deltype != WMS_INBOX_DEL_RUIM)
        {
            // send out CFG message list event
            wms_cfg_do_message_list(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);
            wms_cfg_do_memory_status(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);
        }
    }
    
    /* done */
    return;
} /* wms_msg_delete_box_proc() */

void wms_msg_copy_proc(wms_cmd_type    *cmd_ptr)
{
#if defined(FEATURE_CDSMS) && defined(FEATURE_CDSMS_RUIM)
    wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
    uint32                      src,dest,i;
    wms_memory_store_e_type     src_store;
    wms_memory_store_e_type     dest_store;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    src = cmd_ptr->cmd.msg_copy.copypram.index_src;
    src_store = cmd_ptr->cmd.msg_copy.copypram.memStore_src;
    dest = cmd_ptr->cmd.msg_copy.copypram.index_dest;
    dest_store = cmd_ptr->cmd.msg_copy.copypram.memStore_dest;

    switch (src_store)
    {
        case WMS_MEMORY_STORE_RUIM:
            if (src >= cfg_s_ptr->ruim_max_slots )
            {
                cmd_err =  WMS_CMD_ERR_MSG_INDEX;
                MSG_ERROR("Invalid message index %d", src, 0, 0);
            }
            else if (cfg_s_ptr->ruim_tags[src] == WMS_TAG_NONE)
            {
                cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
                MSG_ERROR("Empty  message %d",  src, 0, 0);
            }
            else if (src_store == dest_store)
            {
                cmd_err =  WMS_CMD_ERR_MSG_MEMORY_STORE;
                MSG_ERROR("Source store is same as target store",  0, 0, 0);
            }
            else if (wms_ruim_read_sms(src, &ruim_data[1]) != TRUE)
            {
                cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
            }
            else
            {
                ruim_data[0] = WMS_FORMAT_CDMA;
                
                if (dest == WMS_DUMMY_MESSAGE_INDEX)
                {// 插入
                    for (i=IN_START; i<=IN_END; i++)
                    {
                        if (cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE)
                        {
                            /* found a free slot */
                            dest = i;
                            break;
                        }
                    }
                    
                    if (dest == WMS_DUMMY_MESSAGE_INDEX)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
                    }
                    else if (wms_nv_write_cdma_sms(dest, ruim_data) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
                    }
                    else
                    {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        /* Update Message Information Cache */
                        wms_cfg_update_msg_info_cache(ruim_data[1],
                                          WMS_MEMORY_STORE_NV_CDMA,
                                          dest,
                                          ruim_data,
                                          WMS_MAX_LEN);
#endif 

                        /* NV write was successful, update message list
                        */
                        cfg_s_ptr->nv_cdma_tags[dest]  = ruim_data[1];
                        
                        // send out CFG message list event
                        wms_cfg_do_message_list(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);
                        wms_cfg_do_memory_status(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);

                        if (cmd_ptr->cmd.msg_copy.copypram.delete_src)
                        {// 移动消息
                            if (wms_ruim_delete_sms(src) == FALSE)
                            {
                                cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
                            }
                            else
                            {
                                /* Update duplicate detection list */
                                wms_cfg_delete_dup_info_cache(WMS_MEMORY_STORE_RUIM, src);
                                
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                                {
                                    uint8 data = cfg_s_ptr->ruim_tags[src];
                                    
                                    /* Delet old Message Information Cache */
                                    wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                              WMS_MEMORY_STORE_RUIM,
                                              src,
                                              &data,
                                              1);
                                }
#endif
                                
                                /* RUIM delete was successful, update message list */
                                cfg_s_ptr->ruim_tags[src] = WMS_TAG_NONE;
                                
                                // send out CFG message list event
                                wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                                wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                            }
                        }
                    }
                }
                else
                {// 替换
                    // TODO
                }
            }
            break;

        case WMS_MEMORY_STORE_NV_CDMA:
            if (src >= cfg_s_ptr->nv_cdma_max_slots)
            {
                cmd_err =  WMS_CMD_ERR_MSG_INDEX;
                MSG_ERROR("Invalid message index %d",src,0,0);
                break;
            }
            if (cfg_s_ptr->nv_cdma_tags[src] == WMS_TAG_NONE)
            {
                cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
                MSG_ERROR("Empty  message %d", src, 0, 0 );
                break;
            }
            
            if (wms_nv_read_cdma_sms(src, ruim_data) == FALSE)
            {
                cmd_err = WMS_CMD_ERR_MSG_NV_READ;
            }
            else
            {
                /* Layout of SMS message in NV. */
                /* 0: format                    */
                /* 1: tag/status                */
                /* 2: len                       */
                /* 3+: data                     */
                if (dest == WMS_DUMMY_MESSAGE_INDEX)
                {// 插入
                    for (i=0; i<cfg_s_ptr->ruim_max_slots; i++)
                    {
                        if (cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE)
                        {
                            /* found a free slot */
                            dest = i;
                            break;
                        }
                    }
                    
                    if (dest == WMS_DUMMY_MESSAGE_INDEX)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
                    }
                    else if (wms_ruim_write_sms(dest, &ruim_data[1], FALSE) == FALSE)
                    {
                        cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
                    }
                    else
                    {
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                        /* Update Message Information Cache */
                        wms_cfg_update_msg_info_cache(ruim_data[1],
                                          WMS_MEMORY_STORE_RUIM,
                                          dest,
                                          &ruim_data[1],
                                          (uint8)cfg_s_ptr->ruim_sms_rec_len);
#endif 

                        /* RUIM write was successful, update message list */
                        cfg_s_ptr->ruim_tags[dest] = (wms_message_tag_e_type)ruim_data[1];
                        
                        // send out CFG message list event
                        wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                        wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
                        
                        if (cmd_ptr->cmd.msg_copy.copypram.delete_src)
                        {// 移动消息
                            ruim_data[1] = WMS_TAG_NONE;
                            
                            if (wms_nv_delete_cdma_sms(src, ruim_data) == FALSE)
                            {
                                cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
                            }
                            else
                            {
                                /* Update duplicate detection list */
                                wms_cfg_delete_dup_info_cache(WMS_MEMORY_STORE_NV_CDMA, src);
                                
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                                /* Delet Message Information Cache */
                                {
                                    uint8 data = cfg_s_ptr->nv_cdma_tags[src];
                                    
                                    /* Delet old Message Information Cache */
                                    wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                              WMS_MEMORY_STORE_NV_CDMA,
                                              src,
                                              &data,
                                              1);
                                }
#endif 
                                
                                /* NV write was successful, update message list */
                                cfg_s_ptr->nv_cdma_tags[src] = WMS_TAG_NONE;
                                
                                // send out CFG message list event
                                wms_cfg_do_message_list(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);
                                wms_cfg_do_memory_status(WMS_MEMORY_STORE_NV_CDMA, WMS_TAG_NONE);
                            }
                        }
                    }
                }
                else
                {// 替换
                    // TODO
                }
            }
            break;

        default:
            cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
            MSG_ERROR("Invalid memory store %d", cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store, 0, 0 );
            break;
    } /* switch mem_store */

    wms_client_cmd_status(cmd_ptr, cmd_err);
    
    return;
#endif /* defined(FEATURE_CDSMS) && defined(FEATURE_CDSMS_RUIM) */
} /* wms_msg_copy_proc() */

#if defined(FEATURE_UIM_TOOLKIT)
void wms_msg_refresh_proc(wms_cmd_type    *cmd_ptr)
{
#if defined(FEATURE_CDSMS) && defined(FEATURE_CDSMS_RUIM)
    wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    cfg_s_ptr->refresh_in_progress = TRUE;
    cfg_s_ptr->cdma_init_complete = FALSE;
    
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
    {
        // 清除 Cache
        int i;
        MSG_FATAL("wms_msg_refresh_proc 0x%x",db_getuiminitmask(),0,0);
        if (db_getuiminitmask() & INITUIMSMSMASK)
        {
            return;
        }
        for (i=0; i < cfg_s_ptr->ruim_max_slots; i++)
        {
            if (cfg_s_ptr->ruim_tags[i] != WMS_TAG_NONE)
            {
                ruim_data[0] = cfg_s_ptr->ruim_tags[i];
                ruim_data[1] = 0;
                
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
                /* Delet old Message Information Cache */
                wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                      WMS_MEMORY_STORE_RUIM,
                                      i,
                                      ruim_data,
                                      (uint8)cfg_s_ptr->ruim_sms_rec_len);
#endif  
              
                /* Update duplicate detection list */
                wms_cfg_delete_dup_info_cache(WMS_MEMORY_STORE_RUIM, i);
                
                /* RUIM delete was successful, update message list */
                cfg_s_ptr->ruim_tags[i] = WMS_TAG_NONE;
            }
            else
            {
                continue; /* skip slots */
            }
            
            wms_cfg_do_message_list(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
            wms_cfg_do_memory_status(WMS_MEMORY_STORE_RUIM, WMS_TAG_NONE);
        }
    }
    db_setuiminitmask(INITUIMSMSMASK);
    //wms_ruim_init_stepbystep();
#ifdef FEATURE_OMH_SMS
    wms_ruim_OMH_init();
#endif
#else        
    wms_ruim_init();
#endif        

    wms_client_cmd_status(cmd_ptr, cmd_err);
#endif /* defined(FEATURE_CDSMS) && defined(FEATURE_CDSMS_RUIM) */
} /* wms_msg_refresh_proc() */
#endif
#endif /*CUST_EDITION*/
/*
*/
void wms_msg_delete_all_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                      i;
  wms_memory_store_e_type     mem_store = cmd_ptr->cmd.msg_delete_all.mem_store;
  wms_message_tag_e_type      tag = cmd_ptr->cmd.msg_delete_all.tag;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      for( i=0; i < cfg_s_ptr->ruim_max_slots; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == cfg_s_ptr->ruim_tags[i] ) )
        {
          if( cfg_s_ptr->ruim_tags[i] == WMS_TAG_NONE )
          {
            continue; /* skip empty slots */
          }
          else if( wms_ruim_delete_sms( i ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
          }
          else
          {
            /* Update duplicate detection list */
            if(WMS_IS_MT(cfg_s_ptr->ruim_tags[i]))
            {
              /* Update the duplicate detection info cache. Mark the location as
              ** empty by simply setting the mask to 0.
              */
              wms_cfg_delete_dup_info_cache(mem_store, i);
            }
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
            {
                uint8 data = cfg_s_ptr->ruim_tags[i];
                
                /* Delet old Message Information Cache */
                wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                          WMS_MEMORY_STORE_RUIM,
                          i,
                          &data,
                          1);
            }
#endif
#endif //#ifdef CUST_EDITION
            /* RUIM delete was successful, update message list
            */
            cfg_s_ptr->ruim_tags[i] = WMS_TAG_NONE;
          }
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS_RUIM */

    case WMS_MEMORY_STORE_NV_CDMA:
      for( i=0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
      {
        if( tag == WMS_TAG_NONE ||
            ( tag == cfg_s_ptr->nv_cdma_tags[i] ) )
        {
          ruim_data[1] =  (uint8)WMS_TAG_NONE;

          if( cfg_s_ptr->nv_cdma_tags[i] == WMS_TAG_NONE )
          {
            continue; /* skip empty slots */
          }
          else if( wms_nv_delete_cdma_sms( i, ruim_data ) == FALSE )
          {
            cmd_err = WMS_CMD_ERR_MSG_NV_DELETE;
          }
          else
          {
            /* Update duplicate detection list */
            if(WMS_IS_MT(cfg_s_ptr->nv_cdma_tags[i]))
            {
              /* Update the duplicate detection info cache. Mark the location as
              ** empty by simply setting the mask to 0.
              */
              wms_cfg_delete_dup_info_cache(mem_store, i);
            }

            /* NV write was successful, update message list
            */
            cfg_s_ptr->nv_cdma_tags[i] = WMS_TAG_NONE;
            if( cfg_s_ptr->cdma_voice_mail_index == i )
            {
              cfg_s_ptr->cdma_voice_mail_index=(wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
              cfg_s_ptr->cdma_voice_mail_active  = FALSE;
            }
          }
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d", mem_store, 0, 0 );
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag     = WMS_TAG_NONE;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_ALL, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_message_list( mem_store, WMS_TAG_NONE );
    wms_cfg_do_memory_status( mem_store, WMS_TAG_NONE );

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }

  /* done */
  return;

} /* wms_msg_delete_all_proc() */


wms_cmd_err_e_type wms_msg_do_modify_tag
(
  wms_memory_store_e_type     mem_store,
  wms_message_index_type      index,
  wms_message_tag_e_type      tag
)
{
  wms_cmd_err_e_type    cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if( wms_ruim_read_sms( index, ruim_data ) != TRUE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else
      {
        ruim_data[0] = (uint8)tag;
        if( wms_ruim_write_sms( index, ruim_data, FALSE ) != TRUE )
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
        }
        else
        {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
          wms_cacheinfolist_updatenodetag(WMS_MEMORY_STORE_RUIM, index, tag);
#endif          
#endif // #ifdef CUST_EDITION
          /* RUIM write was successful, update message list
          */
          cfg_s_ptr->ruim_tags[index] = tag;
        }
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */

    case WMS_MEMORY_STORE_NV_CDMA:
      if( wms_nv_read_cdma_sms( index, ruim_data ) == FALSE)
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else
      {
        ruim_data[1] = (uint8)tag;
        if( wms_nv_replace_cdma_sms( index, ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
        else
        {
#ifdef CUST_EDITION
#if defined(FEATURE_CDSMS_CACHE) || defined(FEATURE_CDSMS_CACHE_USELIST)
          wms_cacheinfolist_updatenodetag(WMS_MEMORY_STORE_NV_CDMA, index, tag);
#endif          
#endif
          /* SIM write was successful, update message list
          */
          cfg_s_ptr->nv_cdma_tags[index] = tag;
        }
      }
      break;

#endif /* FEATURE_CDSMS */




#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d", mem_store,0,0);
      break;
  }

  /* send out message list event */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_cfg_do_message_list( mem_store, WMS_TAG_NONE );
//    wms_cfg_do_memory_status( mem_store, WMS_TAG_NONE );
  }


  /* done */
  return cmd_err;

} /* wms_msg_do_modify_tag() */



/*
*/
void wms_msg_modify_tag_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                        i = cmd_ptr->cmd.msg_modify_tag.index;
  wms_memory_store_e_type     mem_store = cmd_ptr->cmd.msg_modify_tag.mem_store;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd_err = wms_msg_do_modify_tag( mem_store,
                                     i,
                                     cmd_ptr->cmd.msg_modify_tag.tag );

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index = i;
    msg_event_info.status_info.message.msg_hdr.mem_store = mem_store;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_MODIFY_TAG, &msg_event_info );
  }

  /* done */
  return;

} /* wms_msg_modify_tag_proc() */


/*
*/
void wms_msg_read_template_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  uint32                        i;
  wms_memory_store_e_type     mem_store;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  wms_cdma_template_s_type      * cdma_template_ptr
                     = & msg_event_info.status_info.message.u.cdma_template;
#endif /* FEATURE_CDSMS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  i             = cmd_ptr->cmd.msg_read_template.index;
  mem_store     = cmd_ptr->cmd.msg_read_template.mem_store;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
       msg_event_info.status_info.message.msg_hdr.message_mode =
        WMS_MESSAGE_MODE_CDMA;
      if( i >= cfg_s_ptr->ruim_max_templates )
      {
        cmd_err = WMS_CMD_ERR_MSG_INDEX;
      }
      else if( wms_ruim_read_smsp( i, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_READ;
      }
      else if( wms_ts_cdma_decode_smsp( ruim_data,
                                        (uint8) cfg_s_ptr->ruim_smsp_rec_len,
                                        cdma_template_ptr ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */


    case WMS_MEMORY_STORE_NV_CDMA:
       msg_event_info.status_info.message.msg_hdr.message_mode =
        WMS_MESSAGE_MODE_CDMA;
      //TBD: support multiple templates in the future
      if( wms_nv_read_cdma_params( 0, ruim_data ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_NV_READ;
      }
      else if( wms_ts_cdma_decode_smsp( ruim_data,
                                        255,
                                        cdma_template_ptr ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_DECODE;
      }
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      MSG_ERROR("Invalid memory store %d",
                 cmd_ptr->cmd.msg_write.message.msg_hdr.mem_store, 0, 0 );
      break;
  } /* switch mem_store */


  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data          = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id          =  (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = i;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_READ_TEMPLATE, &msg_event_info );
  }

  // done
  return;

} /* wms_msg_read_template_proc() */



/*
*/
void wms_msg_write_template_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_cmd_err_e_type          cmd_err = WMS_CMD_ERR_NONE;
  wms_memory_store_e_type     mem_store;
  uint32                      i;
  wms_write_mode_e_type       write_mode;
  
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  wms_cdma_template_s_type      * cdma_template_ptr
                   = & cmd_ptr->cmd.msg_write_template.message.u.cdma_template;
#endif /* FEATURE_CDSMS */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  i             = cmd_ptr->cmd.msg_write_template.message.msg_hdr.index;
  mem_store     = cmd_ptr->cmd.msg_write_template.message.msg_hdr.mem_store;
  write_mode    = cmd_ptr->cmd.msg_write_template.write_mode;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if( write_mode == WMS_WRITE_MODE_INSERT )
      {
        for( i=0; i<cfg_s_ptr->ruim_max_templates; i++ )
        {
          if( cfg_s_ptr->ruim_templates[i] == FALSE )
          {
            break;
          }
        }

        if( i == cfg_s_ptr->ruim_max_templates )
        {
          cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
          wms_cfg_do_memory_full(WMS_MEMORY_STORE_RUIM);

        }
      }
      else if( write_mode == WMS_WRITE_MODE_REPLACE )
      {
        if( i >= cfg_s_ptr->ruim_max_templates )
        {
          cmd_err = WMS_CMD_ERR_MSG_INDEX;
        }
      }
      else
      {
        cmd_err = WMS_CMD_ERR_MSG_WRITE_MODE;
      }

      if( cmd_err == WMS_CMD_ERR_NONE )
      {
        if( wms_ts_cdma_encode_smsp( cdma_template_ptr,
                                     (uint8) cfg_s_ptr->ruim_smsp_rec_len,
                                     ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else if( wms_ruim_write_smsp( i, ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_WRITE;
        }
        else if( cfg_s_ptr->ruim_templates[i] == FALSE )
        {
          cfg_s_ptr->ruim_templates[i] = TRUE;
          wms_cfg_do_template_list( WMS_MEMORY_STORE_RUIM );
        }
#ifdef CUST_EDITION
#ifdef  FEATURE_CDSMS_CACHE_USELIST
        if (cmd_err == WMS_CMD_ERR_NONE)
        {
          wms_cfg_update_msg_info_cache(WMS_TAG_MO_TEMPLATE,
                                       WMS_MEMORY_STORE_RUIM,
                                       i,
                                       ruim_data,
                                       (uint8)cfg_s_ptr->ruim_smsp_rec_len);
        }
#endif
#endif //#ifdef CUST_EDITION
      }
      break;
#endif /* FEATURE_CDSMS_RUIM */

    case WMS_MEMORY_STORE_NV_CDMA:
      //TBD:  support multiple templates in the future
      {
        i = 0;
        if( wms_ts_cdma_encode_smsp( cdma_template_ptr,
                                     255,
                                     ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_ENCODE;
        }
        else if( wms_nv_write_cdma_params( i, ruim_data ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
        }
      }
      break;

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


    default:
      cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data  = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id  = (wms_client_type_e_type) cmd_ptr->hdr.client_id;
    msg_event_info.status_info.write_mode = write_mode;
    msg_event_info.status_info.message    = cmd_ptr->cmd.msg_write.message;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    (void)wms_msg_event_notify( WMS_MSG_EVENT_WRITE_TEMPLATE, &msg_event_info );
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  /* done */
  return;

} /* wms_msg_write_template_proc() */


/*
*/
void wms_msg_delete_template_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_memory_store_e_type   mem_store;
  wms_message_index_type    i;
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mem_store   = cmd_ptr->cmd.msg_delete_template.mem_store;
  i           = cmd_ptr->cmd.msg_delete_template.index;

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      if( cfg_s_ptr->ruim_templates[i] == FALSE )
      {
        cmd_err =  WMS_CMD_ERR_MSG_EMPTY_MESSAGE;
        MSG_ERROR("Empty  message %d",
                 cmd_ptr->cmd.msg_delete_template.index, 0, 0 );
      }
      else if( wms_ruim_delete_smsp( i ) == FALSE )
      {
        cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
      }
      else
      {
#ifdef CUST_EDITION
#ifdef  FEATURE_CDSMS_CACHE_USELIST
        uint8 data=WMS_TAG_MO_TEMPLATE;
         
        wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                      WMS_MEMORY_STORE_RUIM,
                                      i,
                                      &data,
                                      1);
#endif                                      
#endif // #ifdef CUST_EDITION
        cfg_s_ptr->ruim_templates[i] = FALSE;
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS_RUIM */

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data          = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id          = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = i;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_TEMPLATE, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_template_list( mem_store );
  }

  /* done */
  return;

} /* wms_msg_delete_template_proc() */



/*
*/
void wms_msg_delete_template_all_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_memory_store_e_type   mem_store;
  wms_message_index_type    i = 0;
  wms_cmd_err_e_type        cmd_err = WMS_CMD_ERR_NONE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  mem_store   = cmd_ptr->cmd.msg_delete_template.mem_store;

  MSG_HIGH("Deleting all templates: mem_store=%d", mem_store,0,0);
  i = i; /* to remove compiler warning */

  switch( mem_store )
  {

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM
    case WMS_MEMORY_STORE_RUIM:
      for( i=0; i < cfg_s_ptr->ruim_max_templates; i++ )
      {
        if( cfg_s_ptr->ruim_templates[i] == FALSE )
        {
          continue;
        }

        if( wms_ruim_delete_smsp( i ) == FALSE )
        {
          cmd_err = WMS_CMD_ERR_MSG_RUIM_DELETE;
        }
        else
        {
#ifdef CUST_EDITION
#ifdef  FEATURE_CDSMS_CACHE_USELIST
          uint8 data = WMS_TAG_MO_TEMPLATE;
          
          wms_cfg_update_msg_info_cache(WMS_TAG_NONE,
                                        WMS_MEMORY_STORE_RUIM,
                                        i,
                                        &data,
                                        1);
#endif                                        
#endif // #ifdef CUST_EDITION
          cfg_s_ptr->ruim_templates[i] = FALSE;
        }
      }
      msg_event_info.status_info.message.msg_hdr.message_mode =
                                                         WMS_MESSAGE_MODE_CDMA;
      break;
#endif /* FEATURE_CDSMS_RUIM */

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

    default:
      cmd_err = WMS_CMD_ERR_MSG_MEMORY_STORE;
      break;
  }

  wms_client_cmd_status( cmd_ptr, cmd_err );

  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    msg_event_info.status_info.user_data          = cmd_ptr->hdr.user_data;
    msg_event_info.status_info.client_id          = (wms_client_type_e_type)cmd_ptr->hdr.client_id;
    msg_event_info.status_info.message.msg_hdr.index      = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    msg_event_info.status_info.message.msg_hdr.mem_store  = mem_store;
    msg_event_info.status_info.message.msg_hdr.tag        = WMS_TAG_MO_TEMPLATE;
    (void)wms_msg_event_notify( WMS_MSG_EVENT_DELETE_TEMPLATE_ALL, &msg_event_info );

    // send out CFG message list event
    wms_cfg_do_template_list( mem_store );
  }

  /* done */
  return;

} /* wms_msg_delete_template_all_proc() */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


#ifdef FEATURE_GSTK
/*
*/
void wms_msg_gstk_evt_proc
(
  wms_cmd_type    *cmd_ptr
)
{
#ifdef FEATURE_GWSMS_MO_CONTROL
#error code not present
#endif /* FEATURE_GWSMS_MO_CONTROL */

  /* Done */
  return;

} /* wms_msg_mo_control_rsp_proc() */

#endif /* FEATURE_GSTK */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS

wms_report_status_e_type wms_msg_cdma_map_report_status
(
  wms_status_e_type  st
)
{
  MSG_FATAL("WMS_RPT_OK st = %d",st,0,0);
  switch( st )
  {
    case WMS_OK_S:
      return WMS_RPT_OK;

    case WMS_OUT_OF_RESOURCES_S:
      return WMS_RPT_OUT_OF_RESOURCES;


    case WMS_NETWORK_NOT_READY_S:
      return WMS_RPT_NETWORK_NOT_READY;

    case WMS_PHONE_NOT_READY_S:
      return WMS_RPT_PHONE_NOT_READY;

    case WMS_NOT_ALLOWED_IN_AMPS_S:
      return WMS_RPT_NOT_ALLOWED_IN_AMPS;

    case WMS_NETWORK_FAILURE_S:
      return WMS_RPT_LL_ERROR;

    case WMS_ACCESS_TOO_LARGE_S:
      return WMS_RPT_ACCESS_TOO_LARGE;

    case WMS_DTC_TOO_LARGE_S:
      return WMS_RPT_DC_TOO_LARGE;

    case WMS_ACCESS_BLOCK_S:
      return WMS_RPT_ACCESS_BLOCK;

    case WMS_SIP_PERM_ERROR_S:
      return WMS_RPT_SIP_PERM_ERROR;

    case WMS_SIP_TEMP_ERROR_S:
      return WMS_RPT_SIP_TEMP_ERROR;

    case WMS_INVALID_TPDU_TYPE_S:
    case WMS_INVALID_VALIDITY_FORMAT_S:
    case WMS_MT_MSG_FAILED_S: /* internal use */
    case WMS_INVALID_TRANSACTION_ID_S:
    case WMS_INVALID_FORMAT_S:
    case WMS_GENERAL_ERROR_S:
    case WMS_INVALID_PARM_SIZE_S:
    case WMS_INVALID_USER_DATA_SIZE_S:
    case WMS_INVALID_PARM_VALUE_S:
    case WMS_MISSING_PARM_S:
    default:
      return WMS_RPT_GENERAL_ERROR;
  }

} /* wms_msg_cdma_map_report_status() */

/*=========================================================================
FUNCTION
  wms_msg_auto_dc_send

DESCRIPTION
  Automatically send large messages when DC is connected.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_auto_dc_send
(
  void
)
{
  uint32                i;
  wms_status_e_type     st;

  for(i=0; i< WMS_MAX_MO_MESSAGES; i++)
  {
    if (msg_s_ptr->cdma_mo_rec[i].state == WMS_MSG_STATE_QUEUED &&
        msg_s_ptr->cdma_mo_rec[i].large_msg == TRUE )
    {
      /* Send the Large Message over DC */
      st = wms_msg_cdma_send_sms_OTA( FALSE,   /* not a TL ack */
                                      &msg_s_ptr->cdma_mo_rec[i],
                                      msg_s_ptr->cdma_mo_rec[i].ota,
                                      &msg_s_ptr->cdma_mo_rec[i].address );

      // Send MO SMS Retry Attempt Event
      wms_msg_evt_mo_sms_retry_attempt( (uint32)msg_s_ptr->cdma_mo_rec[i].user_data,
                                        i,
                                        msg_s_ptr->cdma_mo_rec[i].prev_report.report_status,
                                        msg_s_ptr->cdma_mo_rec[i].prev_report.cause_info.tl_status,
                                        msg_s_ptr->cdma_mo_rec[i].retry_time_left );

      if( st == WMS_OK_S )
      {
        msg_s_ptr->cdma_mo_rec[i].state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
      }
      else if( st == WMS_NETWORK_NOT_READY_S )
      {
        if( msg_s_ptr->mo_retry_period > 0 )
        {
          /* Retry is enabled; Get ready for retry */
          msg_s_ptr->cdma_mo_rec[i].state = WMS_MSG_STATE_QUEUED;

          /* Reset retry interval and start retry timer */
          msg_s_ptr->cdma_mo_rec[i].retry_wait_time
                                        = msg_s_ptr->mo_retry_interval;
#ifndef CUST_EDITION
          wms_set_retry_timer();
#else
          clk_dereg(&msg_s_ptr->cdma_mo_rec[i].clk_timer);
          clk_reg2(&msg_s_ptr->cdma_mo_rec[i].clk_timer, 
                   wms_mo_timer,
                   CLK_MS_PER_SEC,
                   0,
                   FALSE,
                   &msg_s_ptr->cdma_mo_rec[i]);
#endif
        }
        else
        {
          /* Retry is not enabled; send submit report to clients now */
          if( msg_s_ptr->cdma_mo_rec[i].send_status_to_clients == TRUE )
          {
            msg_event_info.submit_report_info =
                                    msg_s_ptr->cdma_mo_rec[i].prev_report;
            msg_event_info.submit_report_info.report_status
                                          = WMS_RPT_NETWORK_NOT_READY;
            (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                            &msg_event_info);
          }

          /* Clear the MO record */
          wms_msg_cdma_clear_msg_rec(&msg_s_ptr->cdma_mo_rec[i]);
        }
      }

      break; /* Only send the first seen large message */

    } /* if found large msg */

  } /* for loop */
        
  return;

} /* wms_msg_auto_dc_send() */


/*=========================================================================
FUNCTION
  wms_msg_stop_dc_resend

DESCRIPTION
  Stop resending large messages over DC when it cannot be established.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_stop_dc_resend
(
  void
)
{
  uint32 i;

  for (i = 0; i < WMS_MAX_MO_MESSAGES; i++)
  {
    if (msg_s_ptr->cdma_mo_rec[i].state == WMS_MSG_STATE_QUEUED &&
        msg_s_ptr->cdma_mo_rec[i].large_msg == TRUE )
    {
      
       /* Send submit report to clients now */
       if( msg_s_ptr->cdma_mo_rec[i].send_status_to_clients == TRUE )
       {
         msg_event_info.submit_report_info =
                                 msg_s_ptr->cdma_mo_rec[i].prev_report;
         msg_event_info.submit_report_info.report_status
                                       = WMS_RPT_NETWORK_NOT_READY;
         (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                         &msg_event_info);
       }

       /* Clear the MO record so it will not be resend anymore */
       wms_msg_cdma_clear_msg_rec(&msg_s_ptr->cdma_mo_rec[i]);
        

    } /* if found large msg */

  } /* for loop */
        
  return;

} /* wms_msg_stop_dc_resend() */

/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status_proc

DESCRIPTION
  This function processed the MO SMS Status WMS Command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_mc_mo_status_proc
(
  wms_cmd_type    *cmd_ptr
)
{
  wms_msg_cdma_message_record_type  * rec_ptr = NULL;
  wms_status_e_type                 st;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_MSG_MC_MO_STATUS_E", 0, 0, 0);

#ifdef FEATURE_SMS_OVER_IP
  if( WMS_SIP_PERM_ERROR_S == cmd_ptr->cmd.mc_mo_status.status ) 
  {
    /* Make sure we won't send SMS over SIP again due to a permanent error
    */
    cfg_s_ptr->active_bearers[cmd_ptr->cmd.mc_mo_status.bearer] = FALSE;
  }
#endif /* FEATURE_SMS_OVER_IP */

  rec_ptr = msg_s_ptr->mc_msg.rec_ptr;

  if( msg_s_ptr->mc_msg.is_valid == FALSE )
  {
    /* there is no msg being sent; ignore this event
    */
    MSG_HIGH("no msg was being sent", 0, 0, 0);
  }
  else if( msg_s_ptr->mc_msg.is_ack == TRUE )
  {

/* --------------------------------------------------------- */
/* -------------------- got ack msg status ----------------- */
/* --------------------------------------------------------- */

    MSG_HIGH("got ack st=0x%lx", cmd_ptr->cmd.mc_mo_status.status, 0, 0 );
    MSG_HIGH("seq=%d", rec_ptr->seq_num, 0, 0);

    /* an ack msg was sent; result is available */
    if( rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
    {
      /* MT timer not expired */
      if( cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S )
      {
        /* ack was successfully sent; clear the MT record */
        if(rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL)
        {
          msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
          msg_event_info.ack_report_info.client_id    = rec_ptr->client_id;
          msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
          msg_event_info.ack_report_info.transaction_id =
            (wms_transaction_id_type)rec_ptr->user_data;
          msg_event_info.ack_report_info.is_success = TRUE;
          (void) wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
        }
        wms_msg_cdma_clear_msg_rec( rec_ptr );
      }
      else
      {
        rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
        rec_ptr->state           = WMS_MSG_STATE_QUEUED;
        
        rec_ptr->error_class  = msg_s_ptr->mc_msg.error_class;
        rec_ptr->tl_status    = msg_s_ptr->mc_msg.tl_status;
      }
    }
    else if( rec_ptr->state == WMS_MSG_STATE_IDLE )
    {
      /* the timer expired; ignore this event */
      MSG_HIGH("MT msg(%d) rec is idle", rec_ptr->seq_num, 0, 0);
    }
    else
    {
      ERR("Unexpected msg state %d", rec_ptr->state, 0,0);
      if(rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL)
      {
        msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
        msg_event_info.ack_report_info.client_id = rec_ptr->client_id;
        msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
        msg_event_info.ack_report_info.transaction_id =
          (wms_transaction_id_type)rec_ptr->user_data;
        msg_event_info.ack_report_info.is_success = FALSE;
        (void) wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }
      wms_msg_cdma_clear_msg_rec( rec_ptr );
    }
  }
  else  /* a teleservice M.O. msg was sent */
  {

/* -------------------------------------------------------- */
/* -------------------- got MO msg status ----------------- */
/* -------------------------------------------------------- */

    MSG_HIGH("got MO st=0x%lx", cmd_ptr->cmd.mc_mo_status.status, 0, 0 );
    MSG_HIGH("seq=%d", rec_ptr->seq_num, 0, 0);
    MSG_HIGH("tid=0x%lx", rec_ptr->user_data, 0, 0);

    if( rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
    {
      if( cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S &&
          !(rec_ptr->seq_num & WMS_DUMMY_SEQ_NUM) )
      {
        /* Msg requesting TL ack was sent successfully */
        /* Set up MO ack timer and wait for TL ack from base station */
        MSG_HIGH("MO msg State; Wait for Ack", 0,0,0);
        rec_ptr->state = WMS_MSG_STATE_WAIT_FOR_ACK;
        
        /* Increase Retry Period by WMS_TIMER_MO_ACK */
        rec_ptr->retry_time_left = WMS_TIMER_MO_ACK/1000;
      }
      else
      {
        if( cmd_ptr->cmd.mc_mo_status.status == WMS_ACCESS_TOO_LARGE_S )
        {
           if (TRUE == dc_s_ptr->is_traffic_channel_enabled) 
           {                      
          rec_ptr->large_msg = TRUE;

          MSG_HIGH("Got WMS_ACCESS_TOO_LARGE MO status.", 0,0,0);

          /* Request to set up DC if call is not already active */
#ifndef CUST_EDITION
          if ( !dc_s_ptr->call_active )
#else
          if ((!dc_s_ptr->call_active) && !dc_s_ptr->bInTC)
#endif
          {
              MSG_HIGH("WMS initiating DC... ", 0,0,0);
#ifdef FEATURE_VERSION_ESIA
              st = wms_dc_connect( WMS_CLIENT_TYPE_INTERNAL,
                      NULL,
                      NULL,
                      /*dc_s_ptr->default_so_from_nv*/WMS_DC_SO_6  );
#else
              st = wms_dc_connect( WMS_CLIENT_TYPE_INTERNAL,
                      NULL,
                      NULL,
                      dc_s_ptr->default_so_from_nv );
#endif
              if( st == WMS_OK_S )
              {
                  dc_s_ptr->wms_initiated_call = TRUE;
                     /* Fall back in order to start the retries for large msg */
                  rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
                     /* Wait till RETRY Period to hear back from CM */
 #ifdef CUST_EDITION	                 
                  clk_dereg(&rec_ptr->clk_timer);
                  clk_reg2(&rec_ptr->clk_timer, 
                           wms_mo_timer,
                           CLK_MS_PER_SEC,
                           0,
                           FALSE,
                           rec_ptr);
                  
                  // 注意: 业务信道一旦建立函数 wms_msg_auto_dc_send 会被调用, 故这里给最大等待时间
                  rec_ptr->retry_wait_time = WMS_MAX_RETRY_PERIOD;
                  rec_ptr->state = WMS_MSG_STATE_QUEUED;
#else

                     rec_ptr->retry_wait_time = 0;
                     rec_ptr->state = WMS_MSG_STATE_QUEUED;
#endif /*CUST_EDITION*/	
              }
              else
              {
                  /* Assign the new error code */
                  cmd_ptr->cmd.mc_mo_status.status = st;

                  /* No retry for this msg any more */
                  rec_ptr->retry_time_left = -1;
              }
          }
          else
          {
              rec_ptr->retry_time_left = msg_s_ptr->mo_retry_period;
                 rec_ptr->retry_wait_time = 1;
             }
           }
           else
           {
               /* Phone is not cofigured to send of Traffic channel
                  Send Error code
               */
               cmd_ptr->cmd.mc_mo_status.status = WMS_UNSUPPORTED_S;

               /* No retry for this msg any more */
               rec_ptr->retry_time_left = -1;

               MSG_HIGH("Traffic Channel is not configured",0,0,0);
          }
        }

        /* Prepare the event info in case we need to notify clients
        */
        msg_event_info.submit_report_info.report_status =
          wms_msg_cdma_map_report_status( cmd_ptr->cmd.mc_mo_status.status );
        msg_event_info.submit_report_info.user_data   = rec_ptr->user_data;
        msg_event_info.submit_report_info.client_id    = rec_ptr->client_id;
        msg_event_info.submit_report_info.alpha_id.len = 0;
        msg_event_info.submit_report_info.alpha_id.data = 0;
        msg_event_info.submit_report_info.message_mode= WMS_MESSAGE_MODE_CDMA;

        /* Handle other error cases
        */
        if( (cmd_ptr->cmd.mc_mo_status.status != WMS_OK_S ) && 
            (cmd_ptr->cmd.mc_mo_status.status != WMS_ACCESS_TOO_LARGE_S ) )
        {
          MSG_HIGH("MO SMS L2 error: %d", cmd_ptr->cmd.mc_mo_status.status,0,0);

          if((msg_s_ptr->mo_retry_period > 0 || rec_ptr->large_msg == TRUE) &&
             (wms_msg_is_mo_retry_allowed(WMS_MESSAGE_MODE_CDMA,&msg_event_info.submit_report_info)) &&
             ((rec_ptr->retry_time_left - (sint31)msg_s_ptr->mo_retry_interval) > 0))
          {
            /* Set the retry_wait_time */
            rec_ptr->retry_wait_time = msg_s_ptr->mo_retry_interval;

            /* queue the message */
            rec_ptr->prev_report = msg_event_info.submit_report_info;
            rec_ptr->state       = WMS_MSG_STATE_QUEUED;
          }
          else if( rec_ptr->send_status_to_clients == TRUE )
          {
            (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT,
                                        & msg_event_info );

            /* Clear the MO msg record
            */
            wms_msg_cdma_clear_msg_rec( rec_ptr );
#ifdef FEATURE_WMS_DIAG
            /* Send event to diag interface
            */
            wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );
#endif  /*FEATURE_WMS_DIAG*/

          }
          else
          {
            /* Clear the MO msg record
            */
            wms_msg_cdma_clear_msg_rec( rec_ptr );

#ifdef FEATURE_WMS_DIAG
            /* Send event to diag interface
            */
            wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );
#endif  /*FEATURE_WMS_DIAG*/
          }
        }
        else
        {
          /* MO msg without TL ack requested was successful.
             Send OK status to client now.
          */
          MSG_HIGH( "MO Msg (No TL Ack) OK.", 0,0,0);

          /* Update the tag from NOT_SENT to SENT
          */
          if( rec_ptr->send_mode == WMS_SEND_MODE_MEMORY_STORE )
          {
            (void) wms_msg_do_modify_tag( rec_ptr->msg_hdr.mem_store,
                                          rec_ptr->msg_hdr.index,
                                          WMS_TAG_MO_SENT );
          }
        }

        if( rec_ptr->send_status_to_clients  == TRUE
            && cmd_ptr->cmd.mc_mo_status.status == WMS_OK_S )
        {
          /* Send MO status event to clients
          */
          msg_event_info.submit_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
          
          (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT,
                                      & msg_event_info );

          /* Clear the MO msg record
          */
          wms_msg_cdma_clear_msg_rec( rec_ptr );

#ifdef FEATURE_WMS_DIAG
          /* Send event to diag interface
          */
          wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );
#endif  /*FEATURE_WMS_DIAG*/
        }
      }
    }
    else if( rec_ptr->state == WMS_MSG_STATE_IDLE )
    {
      /*
      ** record was invalidated because timer has expired or
      ** TL Ack arrived before layer2 Ack.
      */
      MSG_HIGH("MO msg rec %d not valid", rec_ptr->seq_num, 0,0);
    }
    else
    {
      ERR("Unexpected msg state %d", rec_ptr->state, 0,0);
      wms_msg_cdma_clear_msg_rec( rec_ptr );
    }

  } /* if is_ack==TRUE - else */

  /* layer2 processing done */
  msg_s_ptr->mc_msg.is_valid = FALSE;

  /* done */
  return;

} /* wms_msg_mc_mo_status_proc() */


/*
*/
void wms_msg_cdma_mt_mwi_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  wms_cmd_err_e_type   cmd_err = WMS_CMD_ERR_NONE;
  wms_client_message_s_type  *msg_ptr = & msg_event_info.mt_message_info.message;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_MSG_MC_MWI_E", 0, 0, 0);

  /* reset the idle timer if auto disconnect is enabled
  */
  if( dc_s_ptr->auto_disc_active == TRUE )
  {
    (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }

  /* Store message and notify clients
  ** NOTE: MWI is always stored in NV.
  */
  msg_event_info.mt_message_info.transaction_id
                                          = (uint8)WMS_DUMMY_TRANSACTION_ID;
  msg_event_info.mt_message_info.route = WMS_ROUTE_STORE_AND_NOTIFY;

  msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_ptr->msg_hdr.mem_store    = WMS_MEMORY_STORE_NV_CDMA;
  msg_ptr->msg_hdr.tag          = WMS_TAG_MT_NOT_READ;

  msg_ptr->u.cdma_message.address.number_of_digits  = 0;
  msg_ptr->u.cdma_message.subaddress.number_of_digits = 0;
  msg_ptr->u.cdma_message.is_mo                     = FALSE;
  msg_ptr->u.cdma_message.is_service_present        = FALSE;
  msg_ptr->u.cdma_message.is_tl_ack_requested       = FALSE;
  msg_ptr->u.cdma_message.teleservice               = WMS_TELESERVICE_MWI;

  /* only one byte in raw bd is valid which carries the msg count */
  msg_ptr->u.cdma_message.raw_ts.format     = WMS_FORMAT_MWI;
  msg_ptr->u.cdma_message.raw_ts.len        = 1;
  msg_ptr->u.cdma_message.raw_ts.data[0]    = cmd_ptr->cmd.mwi.msg_count;


  if( cfg_s_ptr->cdma_voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX )
  {
    /* replace a previous voice mail notification in NV
    */
    msg_ptr->msg_hdr.index = cfg_s_ptr->cdma_voice_mail_index;
    cmd_err = wms_msg_do_write( WMS_WRITE_MODE_REPLACE, msg_ptr );
    if( cmd_err != WMS_CMD_ERR_NONE )
    {
      /* msg cannot be stored
      */
      msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
      msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    }
  }
  else
  {
    /* There was voice mail stored before. Insert this one.
    */
    cmd_err = wms_msg_do_write( WMS_WRITE_MODE_INSERT, msg_ptr );
    if( cmd_err != WMS_CMD_ERR_NONE )
    {
      /* msg cannot be stored
      */
      msg_event_info.mt_message_info.route = WMS_ROUTE_TRANSFER_AND_ACK;
      msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
      msg_ptr->msg_hdr.index     = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
    }
  }

  /* Send msg to clients
  */
  (void)wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE, & msg_event_info );

  /* done */
  return;

} /* wms_msg_cdma_mt_mwi_proc() */


#ifdef FEATURE_SMS_UDH
/*===========================================================================
FUNCTION   wms_msg_cdma_has_wap_udh

DESCRIPTION
  Check if a given Message has a WAP Push Header

DEPENDENCIES
  None

RETURNS
  TRUE:   WAP UDH Present
  FALSE:  WAP UDH Absent

SIDE EFFECTS
  None

===========================================================================*/
boolean wms_msg_cdma_has_wap_udh
(
  wms_client_bd_s_type  * cl_ptr
)
{
  uint32  i;

  if( cl_ptr->message_id.udh_present == TRUE &&
      cl_ptr->user_data.num_headers != 0 )
  {
    for( i=0; i < cl_ptr->user_data.num_headers; i++ )
    {
      if( cl_ptr->user_data.headers[i].header_id == WMS_UDH_PORT_8 ||
          cl_ptr->user_data.headers[i].header_id == WMS_UDH_PORT_16 )
      {
        /* found a WAP header */
        return TRUE;
      }
    }
  }

  /* No WAP header found */
  return FALSE;

} /* wms_msg_cdma_has_wap_udh() */

#endif /* FEATURE_SMS_UDH */

#ifdef FEATURE_CDSMS_BROADCAST
static wms_status_e_type wms_msg_process_mt_SCPT
(
 wms_tl_message_type       * tl_ptr
)
{
  wms_status_e_type           st = WMS_OK_S;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ! (tl_ptr->mask & WMS_MASK_TL_BEARER_DATA) ||
      ! (tl_ptr->cl_bd.mask & WMS_MASK_BD_SCPT_DATA) ||
      tl_ptr->cl_bd.scpt_data_ptr->num_entries == 0 )
  {
    /* SCPT data parameter not present */
    st = WMS_MISSING_PARM_S;
  }
  else
  {
    /* Send a new command to WMS to process SCPT data */
    st = wms_bc_scpt_data( tl_ptr );
  }

  return st;

} /* wms_msg_process_mt_SCPT() */
#endif /* FEATURE_CDSMS_BROADCAST */

#if (defined(FEATURE_UIM_TOOLKIT_UTK) || defined(FEATURE_CCAT)) //Gemsea Modify for SMS-PP && defined(FEATURE_GSTK)
wms_status_e_type wms_msg_utk_pp_download_proc
(
  wms_cdma_message_s_type *cdma_msg,
  wms_tl_message_type     *tl_msg
)
{
  wms_status_e_type       st = WMS_OK_S;
  gstk_status_enum_type   gstk_status;


  st = wms_ts_encode_CDMA_tl( &cdma_tl,
                              &cdma_msg->raw_ts,
                              &cdma_OTA );

  /* GSTK does not need to look at the address, so 0 the length */
  gstk_address.length = 0;

  /* Copy the ota data into the tpdu of the envelope. */
  gstk_sms_tpdu.length = cdma_OTA.data_len;
  gstk_sms_tpdu.tpdu   = mem_malloc(&tmc_heap, cdma_OTA.data_len);
  if(gstk_sms_tpdu.tpdu == NULL)
  {
    /* early return */
    return WMS_GENERAL_ERROR_S;
  }
  memcpy(gstk_sms_tpdu.tpdu, (uint8*)cdma_OTA.data, cdma_OTA.data_len);

  /* Send ENVELOPE command to the card
  */
  gstk_status = gstk_send_envelope_sms_pp_dl_command (
                              (wms_message_mode_e_type)WMS_MESSAGE_MODE_CDMA,
                              TRUE,
                              & gstk_address,
                              & gstk_sms_tpdu,
                              wms_msg_gstk_evt_cb );

  mem_free(&tmc_heap, gstk_sms_tpdu.tpdu);

  if(gstk_status != GSTK_SUCCESS)
  {
    /* Sending down the PP download was unsuccessful */
    st = WMS_GENERAL_ERROR_S;
  }

  return st;
}
#endif /* (FEATURE_UIM_TOOLKIT_UTK||FEATURE_CCAT) && FEATURE_GSTK */


#ifdef FEATURE_JCDMA_2
/*===========================================================================
FUNCTION   wms_msg_cdma_check_esn

DESCRIPTION
  Check whether the received ESN parameter matches the ESN of the phone.

DEPENDENCIES
  None

RETURNS
  WMS_OK_S: Matches (including case of ESN not being present in message)
  WMS_ESN_MISMATCH_S:  Does not match
  Other errors (e.g. no memory or null pointer)

SIDE EFFECTS
  None

===========================================================================*/
wms_status_e_type wms_msg_cdma_check_esn
(
  const wms_raw_ts_data_s_type      * raw_bd_ptr,
  wms_client_bd_s_type              * cl_bd_ptr
)
{
  wms_status_e_type  st = WMS_OK_S;
  uint8  parm_id, parm_len;
  uint8  pos = 0;

  cl_bd_ptr->other.other_data = NULL;

  if( NULL == raw_bd_ptr || NULL == cl_bd_ptr )
  {
    MSG_ERROR("Null pointer",0,0,0);
    st = WMS_NULL_PTR_S;
  }
  else if( cl_bd_ptr->mask & WMS_MASK_BD_OTHER &&
           cl_bd_ptr->other.desired_other_len > 0 )
  {
    cl_bd_ptr->other.input_other_len =
            MIN( cl_bd_ptr->other.desired_other_len, WMS_MAX_LEN );
    cl_bd_ptr->other.other_data =
            mem_malloc( &tmc_heap, cl_bd_ptr->other.input_other_len );

    if( NULL == cl_bd_ptr->other.other_data )
    {
      MSG_ERROR("No memory available", 0,0,0);
      st = WMS_OUT_OF_RESOURCES_S;
    }
    else if( (st=wms_ts_decode_CDMA_bd( raw_bd_ptr,
                                        FALSE, /* is_unpacked?  */
                                        TRUE,  /* decode_other? */
                                        cl_bd_ptr)) != WMS_OK_S )
    {
      MSG_ERROR("Decoding other parameters failed",0,0,0);
      st = WMS_INVALID_FORMAT_S;
    }
    else if( cl_bd_ptr->mask & WMS_MASK_BD_OTHER &&
             cl_bd_ptr->other.desired_other_len > 0 )
    {
      while( pos < cl_bd_ptr->other.input_other_len )
      {
        parm_id  = cl_bd_ptr->other.other_data[pos++];
        parm_len = cl_bd_ptr->other.other_data[pos++];

        if( WMS_BD_ESN == parm_id && parm_len >= 4 )
        {
          /* Found the ESN parameter and do the comparison */
          uint32 received_esn = (cl_bd_ptr->other.other_data[pos] << 24) +
                                (cl_bd_ptr->other.other_data[pos+1] << 16) +
                                (cl_bd_ptr->other.other_data[pos+2] << 8) +
                                (cl_bd_ptr->other.other_data[pos+3]);
          MSG_HIGH( "Received ESN=0x%lx; phone ESN=0x%lx",
                    received_esn,
                    cfg_s_ptr->phone_esn,
                    0);
          if( received_esn != cfg_s_ptr->phone_esn )
          {
            MSG_ERROR("ESN mismatch! Msg ignored.",0,0,0);
            st = WMS_ESN_MISMATCH_S;
            break;
          }
        }
        pos += parm_len;
      } /* while */
    } /* if */
  } /* if */

  if( NULL != cl_bd_ptr->other.other_data)
  {
    mem_free( &tmc_heap, cl_bd_ptr->other.other_data );
    cl_bd_ptr->other.input_other_len = 0;
    cl_bd_ptr->other.other_data = NULL;
  }

  return st;

} /* wms_msg_cdma_check_esn() */
#endif /* FEATURE_JCDMA_2 */

/*=========================================================================
FUNCTION
wms_msg_check_flash_sms_message

DESCRIPTION
  Check if a CDMA message is a Flash SMS Message.
  This is a Synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Is Flash SMS Message
  FALSE: Is Flash SMS Message

SIDE EFFECTS
  None

=========================================================================*/
static boolean wms_msg_check_flash_sms_message
(
  const wms_tl_message_type *cdma_tl_ptr
)
{
  boolean ret_value = FALSE;

  MSG_MED("Function wms_msg_check_flash_sms_message entered", 0, 0, 0);

  if (NULL == cdma_tl_ptr)
  {
    MSG_ERROR("Null Parameter Passed in wms_msg_check_flash_sms_message", 0, 0, 0);
    return ret_value;
  }

  if (cdma_tl_ptr->mask & WMS_MASK_TL_BEARER_DATA) 
  {
    if((cdma_tl_ptr->cl_bd.mask & WMS_MASK_BD_VALID_REL)
       && (WMS_RELATIVE_VALIDITY_IMMEDIATE == 
           wms_ts_encode_relative_time(&cdma_tl_ptr->cl_bd.validity_relative)))
    {
      ret_value = TRUE;
    }
    else if ( (cdma_tl_ptr->cl_bd.mask & WMS_MASK_BD_DISPLAY_MODE)
              && (WMS_DISPLAY_MODE_IMMEDIATE == cdma_tl_ptr->cl_bd.display_mode) )
    {
      ret_value = TRUE;
    }
  }

  MSG_MED("Function wms_msg_check_flash_sms_message result = %d", ret_value, 0, 0);
  return ret_value;

}


/* Deliver or store CDMA or Analog messages.
** It does not handle the TL ack of the MT msg
*/
wms_status_e_type wms_msg_cdma_deliver
(
  wms_transaction_id_type    tid,
  wms_cdma_message_s_type  * cl_ptr,
  wms_routing_s_type       * routing_ptr,
  wms_client_type_e_type   * client_ptr
)
{
  wms_status_e_type          st = WMS_OK_S;
  wms_cmd_err_e_type         cmd_err = WMS_CMD_ERR_NONE;
  wms_write_mode_e_type      write_mode = WMS_WRITE_MODE_INSERT;
  wms_client_message_s_type  *msg_ptr = & msg_event_info.mt_message_info.message;

  boolean                    deliver_to_all_clients = TRUE;
  wms_client_type_e_type     client        = WMS_CLIENT_TYPE_MAX;
  wms_client_type_e_type     parsed_client = WMS_CLIENT_TYPE_MAX;
  boolean                    shared = TRUE;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  // For testing purpose, return WMS_TERMINAL_BLOCKED_S here.

  MSG_HIGH("Delivering CDMA/AMPS messages",0,0,0);

  /* Unpack all fields, including BD subparameters
  */
  wms_ts_convert_cl2tl( cl_ptr, & cdma_tl );

  st = wms_ts_decode_bearer_data( & cl_ptr->raw_ts, & cdma_tl.cl_bd );

  if( st != WMS_OK_S )
  {
    return st;   /* RETURN */
  }
#ifndef CUST_EDITION
#if defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
  if ((cdma_tl.teleservice != WMS_TELESERVICE_VMN_95) &&
      (cdma_tl.teleservice != WMS_TELESERVICE_IS91_VOICE_MAIL) &&
      (cdma_tl.teleservice != WMS_TELESERVICE_MWI))
  {
     MSG_HIGH("msg has been discarded!", 0,0,0);
     routing_ptr->route = WMS_ROUTE_DISCARD;
     return WMS_OK_S;
  }
#endif  
#endif
  /* ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  ** Before processing according to the routing, change the routing for special
  ** cases first.
  ** ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  */

#ifdef FEATURE_CDSMS_BROADCAST
  if( cdma_tl.teleservice == WMS_TELESERVICE_BROADCAST )
  {
    /* All broadcast messages go to the clients directly;
    ** enhancements later will save the messages in the broadcast SMS memories.
    */
    routing_ptr->route     = WMS_ROUTE_TRANSFER_AND_ACK;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif /* FEATURE_CDSMS_BROADCAST */


  if( cdma_tl.teleservice == WMS_TELESERVICE_SCPT )
#ifdef FEATURE_CDSMS_BROADCAST
  {
    routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
    st = wms_msg_process_mt_SCPT( & cdma_tl );
    return st;
  }
#else /* FEATURE_CDSMS_BROADCAST */
  {
    routing_ptr->route     = WMS_ROUTE_DISCARD;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif /* FEATURE_CDSMS_BROADCAST */
#ifdef CUST_EDITION
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
  if (cdma_tl.teleservice == WMS_TELESERVICE_ISRAEL_SILENT)
  {
    routing_ptr->route     = WMS_ROUTE_DISCARD;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif
#endif //#ifdef CUST_EDITION
#ifdef CUST_EDITION//def FEATURE_CCAT
  MSG_FATAL("cdma_tl.teleservice %d %d %d",cdma_tl.teleservice,gstk_is_sms_pp_supported(),nv_rtre_control());
  if( cdma_tl.teleservice == WMS_TELESERVICE_CATPT)
  {
#ifdef CUST_EDITION//def FEATURE_CCAT
    /* Check if service is allocated and activated */
    if(gstk_is_sms_pp_supported() && (nv_rtre_control() == NV_RTRE_CONTROL_USE_RUIM))
    {
      /* Cache some specific data for use when acking the status of the
      ** pp download (user ack).
      */
      pp_download_address       = cl_ptr->address;
      pp_download_subaddress    = cl_ptr->subaddress;
      pp_download_teleservice   = cl_ptr->teleservice;
      pp_download_msg_id        = cdma_tl.cl_bd.message_id.id_number;

      st = wms_msg_utk_pp_download_proc(cl_ptr, &cdma_tl);

      if(st == WMS_OK_S)
      {
        /* Set the route, such that this message is acked properly */
        routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
      }

      /* return early no more processing by cdma_deliver() is needed */
      return st;
    }
    else
    {
      /* Either svc is not activated or allocated.
      ** Treat this as a normal message.
      */
      routing_ptr->route       = WMS_ROUTE_STORE_AND_NOTIFY;
      routing_ptr->mem_store   = WMS_MEMORY_STORE_RUIM;
    }
#endif /* FEATURE_GSTK */
  }
#endif /* FEATURE_CCAT */

#ifdef FEATURE_UIM_TOOLKIT_UTK
  /* The routing for UTK messages should be changed to TRANSFER_ONLY,
  ** so that the application needs to ack it later.
  */
  MSG_FATAL("cdma_tl.cl_bd.mask 0x%x 0x%x 0x%x",cdma_tl.cl_bd.mask,cdma_tl.cl_bd.display_mode,cdma_tl.cl_bd.download_mode);
  if( ( cdma_tl.cl_bd.mask & WMS_MASK_BD_DISPLAY_MODE ) &&
      ( cdma_tl.cl_bd.display_mode == WMS_DISPLAY_MODE_RESERVED ) )
  {
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
    /* Make sure that the current rtre control is not NV.
    */
    if( (cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_PP_VAS ||
         cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_PP_PRL ||
         cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_UPDATE_PRL) &&
        (nv_rtre_control() != NV_RTRE_CONTROL_USE_RUIM) )
    {
      /* This PRL update can not be sent down to the card, return bad status
      ** and the returning function will ack the message appropriately.
      */
      return WMS_GENERAL_ERROR_S;
    }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    if( cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_PP_VAS ||
        cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_PP_PRL )
    {
#ifndef CUST_EDITION//ndef FEATURE_GSTK
      /* The UI client will send down the envelope command to the card */
      routing_ptr->route     = WMS_ROUTE_TRANSFER_ONLY;
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;

      deliver_to_all_clients = FALSE;
      client                 = WMS_CLIENT_TYPE_UI;
#else /* uses GSTK module */
      /* Check if service is allocated and activated */
      if(gstk_is_sms_pp_supported())
      {
        /* Cache some specific data for use when acking the status of the
        ** pp download (user ack).
        */
        pp_download_address       = cl_ptr->address;
        pp_download_subaddress    = cl_ptr->subaddress;
        pp_download_teleservice   = cl_ptr->teleservice;
        pp_download_msg_id        = cdma_tl.cl_bd.message_id.id_number;

        st = wms_msg_utk_pp_download_proc(cl_ptr, &cdma_tl);

        if(st == WMS_OK_S)
        {
          /* Set the route, such that this message is acked properly */
          routing_ptr->route = WMS_ROUTE_INTERNAL_PROC;
        }

        /* return early no more processing by cdma_deliver() is needed */
        return st;
      }
      else
      {
        /* Either svc is not activated or allocated.
        ** Treat this as a normal message.
        */
        routing_ptr->route       = WMS_ROUTE_STORE_AND_NOTIFY;
        routing_ptr->mem_store   = WMS_MEMORY_STORE_RUIM;
      }
#endif /* FEATURE_GSTK */
    }
    /* Treat UPDATE RECORDS as regular messages, but explicitly set the
    ** route and mem_store.
    */
    else if( cdma_tl.cl_bd.download_mode == WMS_DOWNLOAD_MODE_UPDATE_PRL )
    {
      routing_ptr->route         = WMS_ROUTE_STORE_AND_NOTIFY;
      routing_ptr->mem_store     = WMS_MEMORY_STORE_RUIM;
    }
  }
  else
#endif /* FEATURE_UIM_TOOLKIT_UTK */

#ifdef FEATURE_GSM1x
#error code not present
#endif /* FEATURE_GSM1x */

  /* Always store voice mails to NV
  */
#ifndef CUST_EDITION
  if( cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS )
#else
  if ((cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS) &&
      ((cdma_tl.teleservice == WMS_TELESERVICE_VMN_95) ||
       (cdma_tl.teleservice == WMS_TELESERVICE_IS91_VOICE_MAIL) ||
       (cdma_tl.teleservice == WMS_TELESERVICE_MWI)))
#endif //#ifndef CUST_EDITION
  {
    tid                    = (uint32)WMS_DUMMY_TRANSACTION_ID;
    routing_ptr->route     = WMS_ROUTE_STORE_AND_NOTIFY;
    routing_ptr->mem_store = WMS_MEMORY_STORE_NV_CDMA;
  }

  /* Fill in msg header
  */
  msg_event_info.mt_message_info.route          = routing_ptr->route;
  msg_event_info.mt_message_info.transaction_id = tid;

  msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_ptr->msg_hdr.mem_store    = routing_ptr->mem_store;
  msg_ptr->msg_hdr.tag          = WMS_TAG_MT_NOT_READ;
  msg_ptr->u.cdma_message       = * cl_ptr;

  /* voice mail handling
  */
#ifndef CUST_EDITION
  if( cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS )
#else
  if ((cdma_tl.cl_bd.mask & WMS_MASK_BD_NUM_OF_MSGS) &&
      ((cdma_tl.teleservice == WMS_TELESERVICE_VMN_95) ||
       (cdma_tl.teleservice == WMS_TELESERVICE_IS91_VOICE_MAIL) ||
       (cdma_tl.teleservice == WMS_TELESERVICE_MWI)))
#endif //#ifndef CUST_EDITION
  {
    MSG_HIGH("Got voice mail",0,0,0);
    if( cfg_s_ptr->cdma_voice_mail_index != (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX )
    {
      /* replace a previous voice mail notification in NV
      */
      msg_ptr->msg_hdr.index      = cfg_s_ptr->cdma_voice_mail_index;
      write_mode                  = WMS_WRITE_MODE_REPLACE;
      MSG_HIGH("replacing old voice mail",0,0,0);
    }
  }

  /* ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  ** Processing message according to the final routing value.
  ** ------------------------------------------------------------------------
  ** ------------------------------------------------------------------------
  */

  /* Check if this message is to be delivered to a specific client.
  */
  parsed_client = wms_msg_parse_msg(msg_ptr);
  if( parsed_client != WMS_CLIENT_TYPE_MAX )
  {
    /* This message is not to be shared */
    deliver_to_all_clients  = FALSE;
    msg_event_info.mt_message_info.route = routing_ptr->route =
      WMS_ROUTE_TRANSFER_ONLY;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store =
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
    client = parsed_client;
  }
#ifdef CUST_EDITION	  
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
  if (msg_ptr->u.cdma_message.teleservice == WMS_TELESERVICE_CHINAUNICOMREG)
  {
    MSG_HIGH("China Unicom Register ack Message Detected! Routes Changed", 0, 0, 0);
    msg_event_info.mt_message_info.route = routing_ptr->route =
      WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store =
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }
#endif  
#endif /*CUST_EDITION*/
  /* Handling for WAP Push Messages */
  if (wms_cfg_check_wap_push_message(msg_ptr))
  { 
#ifdef FEATURE_CARRIER_TAIWAN_APBW    
    return WMS_UNSUPPORTED_S;
#else    
    MSG_HIGH("WAP Push Message Detected! Routes Changed", 0, 0, 0);
    msg_event_info.mt_message_info.route = routing_ptr->route =
      WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store =
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
#endif
  }

  if (wms_msg_check_flash_sms_message(&cdma_tl))
  {
    MSG_HIGH("Flash SMS Message Detected! Routes Changed to No Store", 0, 0, 0);
    msg_event_info.mt_message_info.route = routing_ptr->route =
      WMS_ROUTE_TRANSFER_AND_ACK;
    msg_event_info.mt_message_info.message.msg_hdr.mem_store =
      routing_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  }

#ifdef FEATURE_JCDMA_2
  /* Check ESN and discard the message if ESN does not match */
  if( TRUE == deliver_to_all_clients )
  {
    st = wms_msg_cdma_check_esn( & cl_ptr->raw_ts, & cdma_tl.cl_bd );
    if( st != WMS_OK_S )
    {
      /* If error is WMS_ESN_MISMATCH_S, message is not delivered and no ack
      ** will be sent; in case of any other errors, an ack with error code
      ** will be sent to the network by the calling function.
      */
      return st;
    }
  }
#endif /* FEATURE_JCDMA_2 */


  /* set the client id to the mt_message_info */
  msg_event_info.mt_message_info.client_id = client;

  MSG_HIGH("Routing=%d, Mem=%d", routing_ptr->route, routing_ptr->mem_store,0);


  switch( routing_ptr->route )
  {
    case WMS_ROUTE_DISCARD:
      /* Do NOT transfer the message to the clients.
      ** The Ack will be sent after this function is called.
      */
      MSG_HIGH("Routing is DISCARD",0,0,0);
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_TRANSFER_ONLY:
      /* Transfer the message to clients.
      ** The Ack will NOT be sent after this function is called.
      */
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_TRANSFER_AND_ACK:
      /* Transfer the message to clients.
      ** The Ack will be sent after this function is called.
      */
      cmd_err = WMS_CMD_ERR_NONE;
      break;

    case WMS_ROUTE_STORE_AND_NOTIFY:
      /* Check for duplicate messages before any more processing is done to this
      ** message.  Do not notify clients if this is a duplicate, simply return
      ** WMS_OK_S and change the routing to DISCARD so the msg will be acked.
      */
        if( wms_ts_decode_CDMA_bd( & msg_ptr->u.cdma_message.raw_ts, FALSE, FALSE,
                             & cdma_tl.cl_bd) != WMS_OK_S )
	{
		return FALSE;
	}
      if( wms_msg_cdma_check_dups( & cdma_tl ) == TRUE )
      {
        MSG_HIGH("Dup msg not stored", 0,0,0);
        routing_ptr->route = WMS_ROUTE_DISCARD;
        return WMS_OK_S;
      }
 #ifdef CUST_EDITION	     
      if (cfg_s_ptr->client_memory_full &&
#ifdef FEATURE_AUTOREPLACE
          (cfg_s_ptr->autoreplace == FALSE) &&
#endif          
          (write_mode != WMS_WRITE_MODE_REPLACE))
      {
        MSG_HIGH("Storage full!", 0,0,0);
        return WMS_OUT_OF_RESOURCES_S;
      }
      
#if defined(FEATURE_CARRIER_ISRAEL_PELEPHONE)
      // 以色列版本在此将消息由可视顺序转为逻辑顺序
      if ((cdma_tl.teleservice != WMS_TELESERVICE_VMN_95) &&
          (cdma_tl.teleservice != WMS_TELESERVICE_IS91_VOICE_MAIL) &&
          (cdma_tl.teleservice != WMS_TELESERVICE_MWI))
      {
         extern void wmsutil_decodemessagefromview2logic(wms_client_message_s_type *pclt_msg);

         wmsutil_decodemessagefromview2logic(msg_ptr);
      }
#endif
#endif /*CUST_EDITION*/
      /* Store the message and notify the clients.
      ** The Ack will be sent after this function is called.
      */
      cmd_err = wms_msg_do_write( write_mode, msg_ptr );
      MSG_FATAL("Writing msg result: %d %d", cmd_err,cdma_tl.teleservice,0);
#ifdef FEATURE_OEMOMH
      if( cdma_tl.teleservice == WMS_TELESERVICE_CATPT)
      {
        routing_ptr->route = WMS_ROUTE_DISCARD; // 不发送通知
      }
#endif
      break;

    default:
      ERR("Bad route: %d", routing_ptr->route, 0,0);
      cmd_err = WMS_CMD_ERR_CFG_ROUTE;
      break;
  }


#ifdef FEATURE_WMS_DIAG
  /* Report events to Diag
  */
  if( cmd_err == WMS_CMD_ERR_NONE )
  {
    wms_evt_mt_sms_notify_s_type   evt;
    evt.mem_type      =  (uint8)msg_ptr->msg_hdr.mem_store;
    evt.msg_index     = msg_ptr->msg_hdr.index;

    event_report_payload( EVENT_MT_SMS_NOTIFY, sizeof(evt), (void*) & evt );
  }
#endif /* FEATURE_WMS_DIAG */

  /* Send msg event to clients
  */
  if( cmd_err == WMS_CMD_ERR_NONE && routing_ptr->route != WMS_ROUTE_DISCARD )
  {
    /* Send msg to clients.
    */
    if( deliver_to_all_clients == TRUE )
    {
      shared = wms_msg_event_notify( WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                     & msg_event_info );

      /* Check if WMS is required to ack this message. */
      if( (shared == FALSE) &&
          (routing_ptr->route != WMS_ROUTE_STORE_AND_NOTIFY) )
      {
        /* The client will ack this message, change the route accordingly */
        routing_ptr->route = WMS_ROUTE_TRANSFER_ONLY;
      }
    }
    else
    {
      wms_msg_event_notify_single_client( client,
                                          WMS_MSG_EVENT_RECEIVED_MESSAGE,
                                          & msg_event_info );
    }
    MSG_HIGH("Msg delivered to clients",0,0,0);
  }

  if (client_ptr)
  {
    *client_ptr = client;
  }

  /* done */
  st = wms_msg_map_cmd_err_to_status( cmd_err );

  return st;

} /* wms_msg_cdma_deliver() */

/*===========================================================================
FUNCTION wms_msg_mt_CDMA_tl_ack_proc

DESCRIPTION
  Handle the TL ack from network.
  rec_ptr : is the message recored for which the TL Ack arrived.
  cause_code_ptr : is the cause code response from the network.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_msg_mt_CDMA_tl_ack_proc(
   wms_msg_cdma_message_record_type * rec_ptr,
   wms_cause_code_type              * cause_code_ptr
)
{
   if( rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK ||
       rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
   {
      /*
            ** If TL Ack arrives before layer2 ack, assume layer2 status is OK,
            ** and it will be ignored when it comes later.
            */
      MSG_HIGH("Got Ack (tid 0x%lx)", rec_ptr->user_data, 0, 0);

      if( rec_ptr->send_status_to_clients == TRUE )
      {
         /* deliver status to client:
              */
         MSG_FATAL("WMS_RPT_OK error_class = %d",cause_code_ptr->error_class,0,0);     
         msg_event_info.submit_report_info.client_id = rec_ptr->client_id;
         msg_event_info.submit_report_info.user_data = rec_ptr->user_data;
         msg_event_info.submit_report_info.report_status =
            (cause_code_ptr->error_class == WMS_ERROR_NONE) ?
            WMS_RPT_OK : WMS_RPT_CDMA_TL_ERROR;
         msg_event_info.submit_report_info.cause_info.error_class =
            cause_code_ptr->error_class;
         msg_event_info.submit_report_info.cause_info.tl_status =
            cause_code_ptr->tl_status;
         msg_event_info.submit_report_info.alpha_id.len = 0;
         msg_event_info.submit_report_info.alpha_id.data = 0;

         msg_event_info.submit_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
         
         (void)wms_msg_event_notify( WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info );
      }

      #ifdef FEATURE_WMS_DIAG
      wms_msg_evt_mo_sms_status( & msg_event_info, & rec_ptr->msg_hdr );
      #endif /*FEATURE_WMS_DIAG*/


      /* invalidate the mo_record:
            */
      wms_msg_cdma_clear_msg_rec( rec_ptr );

      /* Update message status if it is in a memory store and got ack:
            */
      if( rec_ptr->send_mode == WMS_SEND_MODE_MEMORY_STORE &&
          msg_event_info.submit_report_info.report_status == WMS_RPT_OK )
      {
         (void) wms_msg_do_modify_tag( rec_ptr->msg_hdr.mem_store,
                                       rec_ptr->msg_hdr.index,
                                       WMS_TAG_MO_SENT );
      }
   }
   else
   {
      MSG_HIGH("Ignore unexpected TL Ack", 0, 0,0 );
   }
}

/*
*/
void wms_msg_mt_CDMA_proc
(
  wms_cmd_mc_mt_msg_type  * mt_msg_cmd_ptr,
  wms_routing_s_type      * routing_ptr
)
{
  wms_transaction_id_type      tid;
  byte                         seq_num = 0;
  wms_status_e_type            st;
  wms_msg_cdma_message_record_type    * rec_ptr = NULL;
  boolean                      no_delivery = FALSE;

  wms_OTA_message_type    * OTA_ptr = & mt_msg_cmd_ptr->ota;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get next transaction id
  */
  tid = wms_msg_get_next_tid();


  /* extract transport layer fields
  */
  st = wms_ts_decode_CDMA_tl
           (
             OTA_ptr,              /* IN  */
             & cdma_tl,          /* OUT */
             & cdma_cl.raw_ts    /* OUT */
           );

  MSG_LOW("OTA len=%d", OTA_ptr->data_len, 0, 0 );
  MSG_LOW("bd_len=%d", cdma_cl.raw_ts.len, 0, 0 );

  
  // if UASMS_MISSING_MANDATORY_PARM_S, decoding was successful
  // for each parameter, but address was missing in received msg.
  if( st == WMS_MISSING_PARM_S &&
      cdma_tl.tl_message_type == WMS_TL_TYPE_POINT_TO_POINT &&
      cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION )
  {
    st = WMS_OK_S;        // as if received a good message
    no_delivery = TRUE;   // will send back ack immediately without
                          // devliering msg to client
  }

  if( st != WMS_OK_S )
  {
    /* error msg already reported in decoding functions */
  }
  else if( cdma_tl.tl_message_type == WMS_TL_TYPE_ACK )
  {

   /* ----------------------------------------------------- */
   /* -------------- Process Ack to MO msg ---------------- */
   /* ----------------------------------------------------- */
   
       seq_num = cdma_tl.cause_code.reply_seq_num;
   
       /* Only cause codes field should be present in an ack
       */
       if( cdma_tl.mask != WMS_MASK_TL_CAUSE_CODES )
       {
         MSG_HIGH( "TL Ack's mask %d not expected %d",
                   cdma_tl.mask, WMS_MASK_TL_CAUSE_CODES, 0 );
         st = WMS_MISSING_PARM_S;
       }
       else if( (rec_ptr=wms_msg_cdma_map_mo_rec(seq_num)) == NULL ||
                rec_ptr->state == WMS_MSG_STATE_IDLE )
       {
         /* no matching msg pending */
         MSG_HIGH("No matching mo rec %d", seq_num, 0, 0 );
         st = WMS_INVALID_PARM_VALUE_S;
       }
       else 
       {
          wms_msg_mt_CDMA_tl_ack_proc(rec_ptr, &cdma_tl.cause_code);
       }

  } /* -------- End of processing received SMS TL Ack. -------- */

  else
  {

/* -------------------------------------------------------- */
/* ------ process received MT PtoP or Broadcast msg: ------ */
/* -------------------------------------------------------- */

    wms_ts_convert_tl2cl( & cdma_tl, & cdma_cl );


    if( cdma_tl.tl_message_type == WMS_TL_TYPE_BROADCAST )
    {
      /* remove the TL Ack mask if any
      */
      cdma_tl.mask &=  ~ WMS_MASK_TL_BEARER_REPLY_OPTION;

      /* Map the teleservice to the API's unique broadcast teleservice
      */
      cdma_tl.teleservice = WMS_TELESERVICE_BROADCAST;

#ifdef FEATURE_CDSMS_BROADCAST
      if( FALSE == wms_bc_enable_check() )
      {
        /* drop the message since BC is disabled
        */
        no_delivery = TRUE;
        MSG_HIGH("Dropping BC SMS msg.",0,0,0);
      }
#else /* FEATURE_CDSMS_BROADCAST */
      no_delivery = TRUE;
      MSG_HIGH("Dropping BC SMS msg.",0,0,0);
#endif /* FEATURE_CDSMS_BROADCAST */
    }

#ifdef CM_API_PLUS_DIALING
    if (FALSE == wms_msg_convert_address_to_plus(&cdma_tl.address))
    {
        MSG_ERROR("wms_msg_convert_address_to_plus Failed!",0,0,0);
    }
#endif /* CM_API_PLUS_DIALING */

    if( cdma_tl.mask & WMS_MASK_TL_BEARER_REPLY_OPTION
        && TRUE == mt_msg_cmd_ptr->ack_pending )
    {

/* ---------------------------------------------- */
/* ----------- process reply option ------------- */
/* ---------------------------------------------- */

      seq_num = cdma_tl.bearer_reply_seq_num;
      rec_ptr = wms_msg_cdma_map_mt_rec( seq_num );

      MSG_HIGH("MT msg: TL ack (%d) required!", seq_num, 0, 0 );

      if( rec_ptr == NULL )
      {
        MSG_HIGH("No more MT rec available!",0,0,0);
        st = WMS_OUT_OF_RESOURCES_S;
      }
      else if( rec_ptr->state != WMS_MSG_STATE_IDLE )
      {
        /* the same seq num is in use. ignore this msg
        */
        MSG_HIGH( "MT msg: seq %d in use!", seq_num, 0, 0 );
        st = WMS_INVALID_PARM_VALUE_S;
      }
      else  /* found a free record */
      {
#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

        /* deliver msg to client depending on the routing.
        */
        if( no_delivery == FALSE )
        {
          st = wms_msg_cdma_deliver( tid, & cdma_cl, routing_ptr, &rec_ptr->client_id );
        }

#ifdef FEATURE_JCDMA_2
        if( WMS_ESN_MISMATCH_S == st )
        {
          /* Msg is ignored; no ack will be sent */
          MSG_ERROR("No ack will be sent due to ESN mismatch!",0,0,0);
          return;
        }
#endif /* FEATURE_JCDMA_2 */

        /* fill in mt record
        */
        rec_ptr->state     = WMS_MSG_STATE_WAIT_FOR_ACK;
        rec_ptr->user_data = (void*) tid;
        rec_ptr->seq_num   = seq_num;

        if( cdma_tl.mask & WMS_MASK_TL_ADDRESS )
        {
          rec_ptr->address    = cdma_tl.address;
        }
        else
        {
          rec_ptr->address.number_of_digits = 0;
        }

        if( cdma_tl.mask & WMS_MASK_TL_SUBADDRESS )
        {
          rec_ptr->subaddress = cdma_tl.subaddress;
        }
        else
        {
          rec_ptr->subaddress.number_of_digits = 0;
        }

        /* init the corresponding timer in the record queue
        */
#ifndef CUST_EDITION
        (void) rex_set_timer(
                         & rec_ptr->timer,
                         WMS_TIMER_MT_ACK
                       );
#else
        clk_dereg(&rec_ptr->clk_timer);
        clk_reg2(&rec_ptr->clk_timer, 
                 wms_mt_timer,
                 CLK_MS_PER_SEC,
                 0,
                 FALSE,
                 rec_ptr);
#endif // #ifndef CUST_EDITION

        MSG_HIGH( "MT: ack timer set.", 0, 0, 0);
        MSG_HIGH( "MT: tid=%d, seq=%d", tid, seq_num, 0 );

        /* If msg was good, and routing is Transfer Only,
        ** set up timer, but don't send ack yet.
        ** The client is supposed to ack the msg.
        */
        if( no_delivery == FALSE &&
            st == WMS_OK_S &&
            routing_ptr->route == WMS_ROUTE_TRANSFER_ONLY )
        {
          /* No ack is sent immediately */
        }
        else
        {
          // send TL ack for good or bad MT msg immediately when needed.
          ack_info.message_mode       = WMS_MESSAGE_MODE_CDMA;
          ack_info.transaction_id     = tid;
          wms_msg_cdma_map_status( st,
                                   & ack_info.u.cdma.error_class,
                                   & ack_info.u.cdma.tl_status );

          (void) wms_msg_ack( (uint8) WMS_CLIENT_TYPE_INTERNAL,
                              NULL, /* cmd_cb */
                              NULL, /* user_data */
                              & ack_info );
        }
      }
    } /* ----- done with reply option processing ----- */
    else
    {
/* ---------------------------------------------- */
/* ----------- No SMS Ack requested ------------- */
/* ---------------------------------------------- */

      /* deliver msg to client
      */
      if( no_delivery == FALSE )
      {
        MSG_HIGH( "MT: No ACK requested", 0, 0, 0 );

        (void) wms_msg_cdma_deliver( (uint32)WMS_DUMMY_TRANSACTION_ID,
                                     & cdma_cl,
                                     routing_ptr,
                                     NULL );
      }
    } /* if */

  } /* ---- done with CDMA PtoP or Broadcast msg processing ---- */

} /* wms_msg_mt_CDMA_proc() */


/*
*/
void wms_msg_mt_IS91EP_proc
(
  wms_OTA_message_type      * OTA_ptr,
  wms_routing_s_type        * routing_ptr
)
{
  byte                          num_digits;  /* total raw bd len */

  word   i, j;    /* Index for looping through char/digits */
  word   msg_pos; /* Bit position in SMS message relative to start of data */

  word   chari_size; /* Bit size of each chari digit/address */

  /* Character/Digit in each Extended Protocol 28-bit word */
  /* (12-bit parity field excluded in each EP word)        */
  byte   chr_per_word;

  byte   num_of_ep_words; /* Num of Extended Protocol 28-bit words */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize current bit position to message length field, */
  /* char/digit size, characters in an EP word and the number */
  /* of EP words in the SMS message.                          */
  msg_pos      = 15;
  chari_size   = 6;
  chr_per_word = 4;


  /* All EP teleservices are mobile-terminated PTP only
  */
  if( OTA_ptr->format == WMS_FORMAT_ANALOG_CLI )
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_PAGE;

    chari_size   = 4;
    chr_per_word = 6;
  }
  else if( OTA_ptr->format == WMS_FORMAT_ANALOG_VOICE_MAIL )
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_VOICE_MAIL;
  }
  else
  {
    cdma_cl.teleservice = WMS_TELESERVICE_IS91_SHORT_MESSAGE;
  }

  cdma_cl.address.number_of_digits    = 0;
  cdma_cl.subaddress.number_of_digits = 0;
  cdma_cl.is_tl_ack_requested         = FALSE;

  cdma_cl.raw_ts.format = OTA_ptr->format;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* EP Word 1 - Extended Protocol Header    */
  /* Unpack number of EP Message words, not  */
  /* including header word and set the       */
  /* number of fields for user data          */
  num_of_ep_words = b_unpackb( (byte*)OTA_ptr->data,
                               msg_pos,
                               WMS_EP_MSG_LEN_BITS );

  num_digits = 0;

  msg_pos += WMS_EP_MSG_LEN_BITS;  /* skip msg len */

  msg_pos += WMS_EP_MST_BITS;      /* skip message type */


  /* Loop - EP Words 2-N and unpack each Message word */
  for( i=0; i < num_of_ep_words; i++ )
  {
    /* Skip over end of EP data word padding (4 bits) and    */
    /* header data (4 bits) at the beginning of EP data word */
    msg_pos += 8;


    /* Loop - unpack each character to user data or callback number
    */
    for( j=0; j < chr_per_word; j++ )
    {
      cdma_cl.raw_ts.data[ num_digits ] =
           b_unpackb( (byte*)OTA_ptr->data, msg_pos, chari_size );

      msg_pos += chari_size;
      num_digits ++;

    } /* for each chari */

  } /* for each word */


  /* Set bd len
  */
  cdma_cl.raw_ts.len = num_digits;


  /* deliver msg to client
  */
  (void) wms_msg_cdma_deliver( (uint32)WMS_DUMMY_TRANSACTION_ID,
                               & cdma_cl,
                               routing_ptr,
                               NULL );

  /* done */
  return;

} /* wms_msg_mt_IS91EP_proc() */


/*
*/
void wms_msg_mt_AWISMS_proc
(
  wms_OTA_message_type      * OTA_ptr,
  wms_routing_s_type        * routing_ptr
)
{
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

} /* wms_msg_mt_AWISMS_proc() */



/*
*/
void wms_msg_cdma_mt_msg_proc
(
  wms_cmd_type  *cmd_ptr
)
{
  wms_routing_s_type         routing;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH("WMS_CMD_MSG_MT_MSG_E", 0, 0, 0);

  /* reset the idle timer if auto disconnect is enabled
  */
  if( dc_s_ptr->auto_disc_active == TRUE )
  {
    (void) rex_set_timer( & dc_s_ptr->idle_timer, dc_s_ptr->idle_timeout );
  }

  /* Check message routing for CDMA messages.
  ** Note: Analog messages are always stored in NV.
  */
  switch( cmd_ptr->cmd.mt_msg.ota.format )
  {
    case WMS_FORMAT_CDMA:
      routing = cfg_s_ptr->routes.pp_routes[WMS_MESSAGE_CLASS_CDMA];
      wms_msg_mt_CDMA_proc( & cmd_ptr->cmd.mt_msg,
                            & routing );
      break;

    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
      routing.route     = WMS_ROUTE_STORE_AND_NOTIFY;
      routing.mem_store = WMS_MEMORY_STORE_NV_CDMA;
      wms_msg_mt_IS91EP_proc( & cmd_ptr->cmd.mt_msg.ota, & routing );
      break;

    case WMS_FORMAT_ANALOG_AWISMS:
      routing.route     = WMS_ROUTE_STORE_AND_NOTIFY;
      routing.mem_store = WMS_MEMORY_STORE_NV_CDMA;
      wms_msg_mt_AWISMS_proc( & cmd_ptr->cmd.mt_msg.ota, & routing );
      break;

    default:
      ERR("Invalid format: %d", cmd_ptr->cmd.mt_msg.ota.format, 0,0);
      break;
  }


  /* done */
  return;

} /* wms_msg_cdma_mt_msg_proc() */


#ifdef FEATURE_CDSMS_BROADCAST
/*
*/
void wms_msg_send_scpt_result
(
  wms_cmd_type          * cmd_ptr,
  wms_tl_message_type   * tl_ptr
)
{
  wms_status_e_type                 st = WMS_OK_S;
  //wms_msg_cdma_message_record_type  * rec_ptr = NULL;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* use the next message number
  */
  tl_ptr->cl_bd.message_id.id_number = wms_cfg_get_ruim_next_message_number();

#ifdef CM_API_PLUS_DIALING
  if (FALSE == wms_msg_convert_plus_address(&tl_ptr->address))
  {
    MSG_ERROR("wms_msg_convert_plus_address Failed!",0,0,0);
  }
#endif /* CM_API_PLUS_DIALING */

  /* Encode the client bearer data
  */
  st = wms_ts_encode_CDMA_bd( & tl_ptr->cl_bd, & cdma_raw_bd );

  if( st == WMS_OK_S )
  {
    /* ----------------- TL ack is always requested   ------------------- */
    /* Use the same TL fields as in the incoming message, except for
    ** for the bearer reply option.
    */
    tl_ptr->mask |= WMS_MASK_TL_BEARER_REPLY_OPTION;

    /* use the next sequence number
    */
    tl_ptr->bearer_reply_seq_num = (uint8)wms_msg_cdma_next_seq_num();
    st = wms_msg_get_next_unused_seq_num(tl_ptr->bearer_reply_seq_num, 
                                         &tl_ptr->bearer_reply_seq_num);

    if( WMS_OK_S == st )
    {
    st = wms_ts_encode_CDMA_tl( tl_ptr,
                                & cdma_raw_bd,
                                & ruim_sms.ota );
  }
  }

  if( st == WMS_OK_S )
  {
    wms_cmd_err_e_type cmd_err = wms_msg_cdma_send_mo_channel(
          FALSE,
                            cmd_ptr,
                            cmd_ptr->cmd.msg_send.send_mode,
                            & cmd_ptr->cmd.msg_send.message.msg_hdr,
                            tl_ptr,
                            & ruim_sms.ota );

    /* if no resource, drop the MO SCPT result message */
    if( cmd_err != WMS_CMD_ERR_NONE )
    {
      MSG_HIGH("MO SCPT result failed: cmd_err %d", cmd_err, 0,0);
    }
  }

  return;

} /* wms_msg_send_scpt_result() */
#endif /* FEATURE_CDSMS_BROADCAST */

#endif /* FEATURE_CDSMS */


/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_status

DESCRIPTION
  Event Notification function for MO SMS Status to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_status
(
  wms_msg_event_info_s_type  * msg_evt_ptr,
  wms_client_msg_hdr_s_type  * msg_hdr_ptr
)
{
#ifdef FEATURE_WMS_DIAG
  wms_evt_mo_sms_status_s_type  evt;

  evt.transaction_id = (uint32) msg_evt_ptr->submit_report_info.user_data;
  evt.mem_type       = (uint8)msg_hdr_ptr->mem_store;
  evt.msg_index      = msg_hdr_ptr->index;
  evt.report_status  = (uint8)msg_evt_ptr->submit_report_info.report_status;

  if( msg_evt_ptr->submit_report_info.message_mode == WMS_MESSAGE_MODE_CDMA )
  {
    evt.cause_code     = (uint8)msg_evt_ptr->submit_report_info.cause_info.tl_status;
  }
  else
  {
    evt.cause_code     = (uint8) msg_evt_ptr->submit_report_info.cause_info.cause_value;
  }

  event_report_payload( EVENT_MO_SMS_STATUS, sizeof(evt), (void*) & evt );
#endif /* FEATURE_WMS_DIAG */

  return;

} /* wms_msg_evt_mo_sms_status() */

/*=========================================================================
FUNCTION
  wms_msg_evt_mo_sms_retry_attempt

DESCRIPTION
  Event Notification function for MO SMS Retry Attempt to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_evt_mo_sms_retry_attempt
(
  uint32 transaction_id,
  uint32 record_id,
  uint8  prev_report_status,
  uint8  prev_cause_code,
  uint32 retry_time_left
)
{
#ifdef FEATURE_WMS_DIAG
  wms_evt_mo_sms_retry_attempt_s_type  evt;

  evt.transaction_id     = transaction_id;
  evt.record_id          = record_id;
  evt.prev_report_status = prev_report_status;
  evt.prev_cause_code    = prev_cause_code;
  evt.retry_time_left    = retry_time_left;

  event_report_payload( EVENT_MO_SMS_RETRY_ATTEMPT, sizeof(evt), (void*) & evt );
#endif /* FEATURE_WMS_DIAG */

  return;

} /* wms_msg_evt_mo_sms_retry_attempt() */

/*
*/
void wms_msg_process_cmd
(
  wms_cmd_type  *cmd_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( cmd_ptr->hdr.cmd_id )
  {
    case WMS_CMD_MSG_SEND:
      wms_msg_send_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_ACK:
      wms_msg_ack_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_READ:
      wms_msg_read_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_WRITE:
      wms_msg_write_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE:
      wms_msg_delete_proc( cmd_ptr );
      break;
#ifdef CUST_EDITION
    case WMS_CMD_MSG_DELETE_BOX:
      wms_msg_delete_box_proc( cmd_ptr );
      break;
    
    case WMS_CMD_MSG_COPY:
      wms_msg_copy_proc( cmd_ptr );
      break;
    
#if defined(FEATURE_UIM_TOOLKIT)
    case WMS_CMD_REFRESH_IND:
      wms_msg_refresh_proc(cmd_ptr);
      break;
#endif      
#endif // #ifdef CUST_EDITION
    case WMS_CMD_MSG_DELETE_ALL:
      wms_msg_delete_all_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_READ_TEMPLATE:
      wms_msg_read_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_WRITE_TEMPLATE:
      wms_msg_write_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE_TEMPLATE:
      wms_msg_delete_template_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_DELETE_TEMPLATE_ALL:
      wms_msg_delete_template_all_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MODIFY_TAG:
      wms_msg_modify_tag_proc( cmd_ptr );
      break;

#ifdef FEATURE_CDSMS
    case WMS_CMD_MSG_MC_MO_STATUS_E:
      wms_msg_mc_mo_status_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MC_MT_MSG_E:
      wms_msg_cdma_mt_msg_proc( cmd_ptr );
      break;

    case WMS_CMD_MSG_MC_MWI_E:
      wms_msg_cdma_mt_mwi_proc( cmd_ptr );
      break;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GSTK
    case WMS_CMD_MSG_GSTK_EVT:
      wms_msg_gstk_evt_proc( cmd_ptr );
      break;
#endif /* FEATURE_GSTK */

    default:
      MSG_ERROR("Invalid cmd: %d", cmd_ptr->hdr.cmd_id, 0,0);
      break;
  }

  /* done */
  return;

} /* wms_msg_process_cmd() */



/*
*/
wms_transaction_id_type wms_msg_get_next_tid
(
  void
)
{
  static  wms_transaction_id_type     tid = 0;

  return tid++;

} /* end of wms_msg_get_next_tid() */




#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */




/*
*/
void wms_msg_init
(
  void
)
{
  uint32    i;

  msg_s_ptr = wms_msg_s_ptr();
  cfg_s_ptr = wms_cfg_s_ptr();
#ifdef FEATURE_CDSMS
  dc_s_ptr  = wms_dc_s_ptr();
#endif /* FEATURE_CDSMS */
  
#ifndef CUST_EDITION
  rex_def_timer(&wms_retry_timer,
                &wms_tcb,
                 WMS_MO_RETRY_TIMER_SIG);
#endif

#ifdef FEATURE_CDSMS
  msg_s_ptr->tid             = 0;
  msg_s_ptr->tl_seq_num      = WMS_TL_SEQ_NUM_MAX - 1;
  msg_s_ptr->mc_msg.is_valid = FALSE;
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  /* -------------------- Init message record queues -------------------- */
  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    msg_s_ptr->cdma_mt_rec[ i ].state   = WMS_MSG_STATE_IDLE;
    msg_s_ptr->cdma_mt_rec[ i ].seq_num = WMS_DUMMY_SEQ_NUM;
#ifndef CUST_EDITION
    rex_def_timer(
                   & msg_s_ptr->cdma_mt_rec[ i ].timer,
                   & wms_tcb,
                   WMS_MT_ACK_TIMER_SIG
                 );
#else
    clk_def(&msg_s_ptr->cdma_mt_rec[i].clk_timer);
#endif //#ifdef CUST_EDITION
  }

  for( i=0; i < WMS_MAX_MO_MESSAGES; i++ )
  {
    msg_s_ptr->cdma_mo_rec[ i ].state   = WMS_MSG_STATE_IDLE;
    msg_s_ptr->cdma_mo_rec[ i ].seq_num = WMS_DUMMY_SEQ_NUM;
    msg_s_ptr->cdma_mo_rec[i].ota = NULL;
    msg_s_ptr->cdma_mo_rec[i].large_msg = FALSE;
#ifdef CUST_EDITION
    clk_def(&msg_s_ptr->cdma_mo_rec[i].clk_timer);
#endif
  }

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  /* ---------------- Init the command queues and buffers ---------------- */
  
  /* invalidate CDMA sms msg global data
  */
  msg_s_ptr->mc_msg.is_valid = FALSE;

  msg_s_ptr->tl_seq_num = WMS_TL_SEQ_NUM_MAX-1;  /* next value will be 0 */
  msg_s_ptr->non_tl_seq_num = WMS_TL_SEQ_NUM_MAX-1;

#ifdef FEATURE_CDSMS_BROADCAST
  /* Allocate buffers for SCPT data and result pointers:
  */
  cdma_tl.cl_bd.scpt_data_ptr
                  = mem_malloc( & tmc_heap, sizeof(wms_bc_scpt_data_s_type) );
  cdma_tl.cl_bd.scpt_result_ptr
                  = mem_malloc( & tmc_heap, sizeof(wms_bc_scpt_result_s_type) );

  if(cdma_tl.cl_bd.scpt_data_ptr == NULL || cdma_tl.cl_bd.scpt_result_ptr == NULL)
  {
    MSG_ERROR("Null ptr - cdma_tl.cl_bd.scpt_data_ptr & cdma_tl.cl_bd.scpt_result_ptr",0,0,0);
    return;
  }
#endif /* FEATURE_CDSMS_BROADCAST */

  /* done with all init's
  */
  MSG_HIGH( "wms_msg_init(): CDMA done", 0, 0, 0 );

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_APPS_POWER_COLLAPSE */


  /* done */
  return;

} /* wms_msg_init() */




#ifdef FEATURE_CDSMS
/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_ack_timer_proc

DESCRIPTION
  This function processed the MT SMS Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
#ifndef CUST_EDITION
void wms_msg_cdma_mt_ack_timer_proc
(
  void
)
{
  uint32                            i;
  wms_status_e_type                 st = WMS_OK_S;
  wms_msg_cdma_message_record_type  * rec_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH( "got WMS_MT_ACK_TIMER_SIG", 0, 0, 0 );

  (void) rex_clr_sigs( rex_self(), WMS_MT_ACK_TIMER_SIG );

  /* process each record whose timer expired
  */
  for( i = 0; i < WMS_MAX_MT_MESSAGES; i ++ )
  {
    rec_ptr = & msg_s_ptr->cdma_mt_rec[i];

    if( rec_ptr->state                    != WMS_MSG_STATE_IDLE &&
        rex_get_timer( & rec_ptr->timer ) == 0 )
    {
      MSG_HIGH("found MT tid=0x%lx, seq=%d",
              rec_ptr->user_data, rec_ptr->seq_num, 0 );

      if( rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK )
      {
        MSG_HIGH( "Client didn't ack msg",0,0,0);

        st = wms_msg_cdma_ack_msg_i( rec_ptr, WMS_ERROR_TEMP, WMS_TL_NO_ACK_S );

        /* don't care about layer2 ack status;
           don't report ack status to client
        */
        if( st == WMS_OK_S )
        {
          msg_s_ptr->mc_msg.is_valid = FALSE;
        }
        /* else, no retry or queuing is done */
      }
      else if( rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS )
      {
        /* this timer sig is from the layer2 ack timer */
        /* sending ack msg failed; report error to user */
        MSG_HIGH( "MT ack layer2 timeout",0,0,0);

        msg_s_ptr->mc_msg.is_valid = FALSE;
      }
      else if( rec_ptr->state == WMS_MSG_STATE_QUEUED )
      {
        MSG_HIGH( "MT ack queued too long",0,0,0);
      }
      else
      {
        rec_ptr->state = WMS_MSG_STATE_IDLE;
        ERR("Unexpected msg state %d", rec_ptr->state, 0,0 );
      }

      if(rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL &&
         (rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS ||
          rec_ptr->state == WMS_MSG_STATE_QUEUED))
      {
        msg_event_info.ack_report_info.client_id = rec_ptr->client_id;
        msg_event_info.ack_report_info.transaction_id =
          (wms_transaction_id_type)rec_ptr->user_data;
        msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
        msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
        msg_event_info.ack_report_info.is_success = FALSE;
        (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
      }

      wms_msg_cdma_clear_msg_rec( rec_ptr );

    } /* if timer expired */

  } /* for */

  /* done */
  return;

} /* wms_msg_cdma_mt_ack_timer_proc() */
#endif

/*=========================================================================
FUNCTION
  wms_msg_awi_ack_timer_proc

DESCRIPTION
  This function processes the AWI Ack Timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sends a Negative Ack to the Network

=========================================================================*/
void wms_msg_awi_ack_timer_proc
(
  void
)
{
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
} /* wms_msg_awi_ack_timer_proc() */



/*=========================================================================
FUNCTION
  wms_msg_mc_mo_status

DESCRIPTION
  This function processed the MO SMS Status recieved from MC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Posts a command to WMS Command Queue

=========================================================================*/
void wms_msg_mc_mo_status
(
  wms_error_class_e_type     error_class,
  wms_status_e_type          status,
  wms_bearer_e_type          bearer
)
{
  wms_cmd_type    * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( msg_s_ptr->mc_msg.is_valid == FALSE )
  {
    return;  /* no msg being sent; no need to report */
  }

  /* got a layer2 status */
  /* report to client */
  /* send a command to sms task */

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL )
  {
    /* Set up command */
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MO_STATUS_E;
    cmd_ptr->hdr.client_id   =  (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.mc_mo_status.error_class   = error_class;
    cmd_ptr->cmd.mc_mo_status.status     = status;
    cmd_ptr->cmd.mc_mo_status.bearer     = bearer;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    ERR("Out of cmd buf",0,0,0);
    /* else, command buffer allocation failed. Nothing to do since
       error was reported/logged already; the command will be ignored.
    */
  }

  /* done */
  return;

} /* end of wms_msg_mc_mo_status() */


/* <EJECT> */
/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_MWI

DESCRIPTION
  This function sends a command to the SMS task for processing the received
  message waiting indication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the SMS command queue.

=========================================================================*/
void wms_msg_cdma_mt_MWI
(
  byte  msg_count
)
{
  wms_cmd_type    * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("wms_msg_cdma_mt_MWI() called", 0,0,0);

  cmd_ptr = wms_get_cmd_buf();

  if( cmd_ptr != NULL )
  {
    /* Set up command */
    cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MWI_E;
    cmd_ptr->hdr.client_id   =  (uint8)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.cmd_cb      = NULL;
    cmd_ptr->hdr.user_data   = NULL;

    cmd_ptr->cmd.mwi.msg_count   = msg_count;

    wms_put_cmd( cmd_ptr );
  }
  /* else, command buffer allocation failed. Nothing to do since
     error was reported/logged already; the command will be ignored.  */

  /* done */
  return;

} /* end of wms_msg_cdma_mt_MWI() */


/* <EJECT> */
/*===========================================================================
FUNCTION   wms_msg_awi_store_info

DESCRIPTION
  Parse and store a segment of AWI SMS message into the AWI info buffer.

DEPENDENCIES
  None

RETURNS
  Status of the storing.

SIDE EFFECTS
  Update AWI info buffer.
===========================================================================*/
wms_error_class_e_type wms_msg_awi_store_info
(
  byte      awi_cnt,     /* Current AWI SMS info data byte count */

  /* Pointer to received (mobile-terminated) analog IS-95 AWI data
     to be translated (word 1 not included) */
  byte      * awi_ptr,
  wms_cdma_tl_status_e_type  * tl_status_ptr /* OUT */
)
{
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  return WMS_ERROR_NONE;
} /* end of wms_msg_awi_store_info() */



/* <EJECT> */
/*===========================================================================
FUNCTION   wms_msg_awi_assemble_mt

DESCRIPTION
  Assemble a segment of AWI SMS message into the AWI info buffer.

DEPENDENCIES
  None

RETURNS
  Status of the assembling.

SIDE EFFECTS
  Update AWI info buffer.

===========================================================================*/
wms_error_class_e_type wms_msg_awi_assemble_mt
(
  byte           sms_len,
  byte           * sms_ptr,
  boolean        * msg_complete_ptr,   /* OUT: if the entire msg is received */
  wms_cdma_tl_status_e_type  * tl_status_ptr /* OUT */
)
{
  wms_error_class_e_type     err = WMS_ERROR_NONE;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

  /* done */
  return err;

} /* end of wms_msg_awi_assemble_mt() */



/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_msg

DESCRIPTION
  This function receives an SMS message and sends a command to the SMS task
  for processing.

  For Analog AWI SMS messages, segments are assembled into a complete
  message before being sent to the SMS task and an ack order is returned
  for intermediate segments.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Add a command to the command queue. AWI SMS info is updated for AWI SMS
  segments.

=========================================================================*/
void wms_msg_cdma_mt_msg
(
  wms_format_e_type      format,

  /* length of sms message in bytes */
  word                   sms_len,

  /* Pointer to received (mobile-terminated) SMS message */
  byte                   * sms_ptr,
  wms_bearer_e_type      bearer,
  boolean                   ack_pending,
  wms_transaction_id_type   mt_iwmsc_tid
)
{
  wms_cmd_type            * cmd_ptr;
  wms_error_class_e_type  error_class  = WMS_ERROR_NONE;
  wms_cdma_tl_status_e_type    tl_status = WMS_TL_ADDRESS_VACANT_S;
  boolean                 msg_complete = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("wms_msg_cdma_mt_msg() called", 0,0,0);
#ifdef CUST_EDITION
  if (!cfg_s_ptr->cdma_init_complete)
  {
    MSG_ERROR("WMS not inited!", 0,0,0);
    return;
  }
#endif

  if( format == WMS_FORMAT_MWI )
  {
    wms_msg_cdma_mt_MWI( * sms_ptr );  // data is only one byte, i.e. msg_count
    return; /* SHORT-RETURN */
  }


  if( sms_len > WMS_MAX_LEN )
  {
    MSG_HIGH( "MT msg: msg len %d is larger than max %d",
              sms_len, WMS_MAX_LEN, 0 );
    error_class = WMS_ERROR_PERM;
    tl_status   = WMS_TL_UNRECOGNIZED_PARM_VALUE_S;
  }


  if( format >= WMS_FORMAT_MAX )
  {
    ERR( "MT msg: msg format %d is invalid!", format, 0, 0 );
  }
  else if( format == WMS_FORMAT_ANALOG_AWISMS )
  {
    /*----------------------------------------------------------------------
      For AWISMS msgs, L2 ack + L3 ack will be available if msg is not
      complete or there is an error. Ack will not be available immediately
      if msg is complete and has been sent to the SMS task.
    -----------------------------------------------------------------------*/

    /* assemble the segments
    */
    if( error_class == WMS_ERROR_NONE )
    {
      error_class = wms_msg_awi_assemble_mt( (byte)sms_len,
                                             sms_ptr,
                                             & msg_complete,
                                             & tl_status );
    }

    /* if msg is not complete or if there is an error, return status in
       order confirmation within the MC task context
    */
    if( error_class != WMS_ERROR_NONE )
    {
      /* msg failed */
      MSG_HIGH("AWI ack: error_class=%d, tl_status=%d", error_class,
               tl_status,0);
      wms_msg_awi_ack( TRUE, FALSE, error_class, tl_status );
    }
    else if( msg_complete == FALSE )
    {
      /* msg Ok, but waiting for more segments */
      MSG_HIGH("AWI Ack: no errors",0,0,0);
      wms_msg_awi_ack( FALSE, FALSE, WMS_ERROR_NONE, tl_status );
    }
    /* else: no ack is sent immediately;
       msg will be delivered to the UI and the UI needs to ack it.
    */
  }
  /* else send msg to sms task for cdma and is91ep msgs */


  /* send the cmd; in case of AWISMS, the msg should be complete
  */
  if( error_class == WMS_ERROR_NONE
      &&
      ( format        != WMS_FORMAT_ANALOG_AWISMS ||
        msg_complete  == TRUE
      )
    )
  {
    cmd_ptr = wms_get_cmd_buf();

    if( cmd_ptr == NULL )
    {
      /* Command buffer allocation failed.
      */
      error_class = WMS_ERROR_TEMP;
      tl_status   = WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S;

      /* send tl ack in MC task context
      */
      if( format == WMS_FORMAT_CDMA )
      {
        /* no action taken; the base station will resend the msg */
      }
      else if( format == WMS_FORMAT_ANALOG_AWISMS )
      {
        /* msg failed */
        MSG_HIGH("AWI ack: error_class=%d, tl_status=%d", error_class,
                 tl_status,0);
        wms_msg_awi_ack( TRUE, FALSE, error_class, tl_status );
      }
      /* else there is no tl ack available for reporting errors for
         other formats
      */
    }
    else
    {
      /* Set up command
      */
      cmd_ptr->hdr.cmd_id      = WMS_CMD_MSG_MC_MT_MSG_E;
      cmd_ptr->hdr.client_id   = (wms_client_id_type) WMS_CLIENT_TYPE_INTERNAL;
      cmd_ptr->hdr.cmd_cb      = NULL;
      cmd_ptr->hdr.user_data   = NULL;

      cmd_ptr->cmd.mt_msg.ota.format    = format;
      cmd_ptr->cmd.mt_msg.bearer        = bearer;

      if( format == WMS_FORMAT_ANALOG_AWISMS )
      {
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
      }
      else
      {
        /* CDMA or IS91EP */
        cmd_ptr->cmd.mt_msg.ota.data_len = sms_len;
        memcpy( (byte*) cmd_ptr->cmd.mt_msg.ota.data,
                sms_ptr,
                MIN(sms_len,WMS_MAX_LEN) );
        cmd_ptr->cmd.mt_msg.ack_pending  =  ack_pending;
        cmd_ptr->cmd.mt_msg.mt_iwmsc_tid =  mt_iwmsc_tid;
      }

      /* Send it to WMS task command queue
      */
      wms_put_cmd( cmd_ptr );
    }

  } /* if */


  /* done */
  return;

} /* wms_msg_cdma_mt_msg() */

/*=========================================================================
FUNCTION
  wms_msg_cdma_mt_busy

DESCRIPTION
  Check whether CDMA MT records are in use or not.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mt_busy
(
  void
)
{
  uint32  i;
  boolean busy = FALSE;

  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mt_rec[i].state != WMS_MSG_STATE_IDLE )
    {
      // This Record in Use
      busy = TRUE;
      break;
    }
  }

  return busy;

} /* wms_msg_cdma_mt_busy() */


/*=========================================================================
FUNCTION
  wms_msg_cdma_mo_busy

DESCRIPTION
  Check whether CDMA MO records are in use or not.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE: busy
  FALSE: not busy

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_msg_cdma_mo_busy
(
  void
)
{
  uint32  i;
  boolean busy = FALSE;
#ifdef CUST_EDITION
  if (cfg_s_ptr->multisend)
  {
    return TRUE;
  }
#endif
  for( i=0; i < WMS_MAX_MO_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mo_rec[i].state != WMS_MSG_STATE_IDLE )
    {
      // This Record in Use
      busy = TRUE;
      break;
    }
  }

  return busy;

} /* wms_msg_cdma_mo_busy() */


#endif /* FEATURE_CDSMS */



/*=========================================================================
FUNCTION
  wms_msg_get_cache_info

DESCRIPTION
  Allow the client to retrieve the message cache info for a message stored in
  a memory.

  NOTE: This is a synchronous function call.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  None.

=========================================================================*/
wms_status_e_type wms_msg_get_cache_info
(
  wms_memory_store_e_type         mem_store,
  wms_message_index_type          index,
  wms_message_tag_e_type          tag,
  uint8                           * cache_ptr
)
{
  wms_status_e_type st = WMS_OK_S;

/* TBD: critical section setup
*/
  if( tag == WMS_TAG_MO_TEMPLATE )
  {
    switch( mem_store )
    {
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif/*FEATURE_GWSMS_CACHE*/

#if (defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE))
      case WMS_MEMORY_STORE_RUIM:
        memcpy( cache_ptr,
                cfg_s_ptr->ruim_template_info_cache[index],
                WMS_MSG_INFO_CACHE_SIZE );
        break;
#endif /* defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE) */

      default:
        st = WMS_GENERAL_ERROR_S;
        break;
    }
  }
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif /* FEATURE_GWSMS_CACHE */
  else
  {
    switch( mem_store )
    {
#ifdef FEATURE_GWSMS_CACHE
#error code not present
#endif /*FEATURE_GWSMS_CACHE*/

#if (defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE))
      case WMS_MEMORY_STORE_RUIM:
        memcpy( cache_ptr,
                cfg_s_ptr->ruim_sms_info_cache[index],
                WMS_MSG_INFO_CACHE_SIZE );
        break;
#endif /* defined(FEATURE_CDSMS_RUIM) && defined(FEATURE_CDSMS_CACHE) */

#ifdef FEATURE_CDSMS
      case WMS_MEMORY_STORE_NV_CDMA:
#ifdef FEATURE_CDSMS_CACHE
        memcpy( cache_ptr,
                cfg_s_ptr->nv_cdma_info_cache[index],
                WMS_MSG_INFO_CACHE_SIZE );
#endif /* FEATURE_CDSMS_CACHE */
        break;
#endif /* FEATURE_CDSMS */

      default:
        st = WMS_GENERAL_ERROR_S;
        break;
    }
  }

  return st;

} /* wms_msg_get_cache_info() */

/*=========================================================================
FUNCTION
  wms_msg_check_in_use_records

DESCRIPTION
  Checks the MO / MT records for any in use records

DEPENDENCIES
  None

RETURN VALUE
  TRUE: There are in use records
  FALSE: No in use records

SIDE EFFECTS
  None.

=========================================================================*/
boolean wms_msg_check_in_use_records
(
  void
)
{
  /* Initialize to Indicate No records in Use */
  boolean ret_val = FALSE;
  int i;

#ifdef FEATURE_CDSMS
  for (i=0; i < WMS_MAX_MO_MESSAGES; i++)
  {
    if (msg_s_ptr->cdma_mo_rec[i].state != WMS_MSG_STATE_IDLE)
    {
      /* This Record in Use */
      ret_val = TRUE;
    }
  }

  if (!ret_val)
  {
    for (i=0; i < WMS_MAX_MT_MESSAGES; i++)
    {
      if (msg_s_ptr->cdma_mt_rec[i].state != WMS_MSG_STATE_IDLE)
      {
        /* This Record in Use */
        ret_val = TRUE;
      }
    }
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  return ret_val;
}

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */
#ifdef CUST_EDITION	

/*=========================================================================
FUNCTION
  wms_msg_set_auto_cdma_params

DESCRIPTION
  Sets particular CDMA parameters based on cdma_set_params_ptr, to be used for
  MO messaging on the CDMA network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Internal data set for MO processing.

=========================================================================*/
wms_status_e_type wms_msg_set_auto_cdma_params
(
  wms_client_id_type              client_id,
  wms_cdma_template_s_type        *cdma_set_params_ptr  /* IN */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */

}

/*=========================================================================
FUNCTION
  wms_msg_set_auto_gw_params

DESCRIPTION
  Sets particular GW parameters based on gw_set_params_ptr, to be used for
  MO messaging on the GW network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Internal data set for MO processing.

=========================================================================*/
wms_status_e_type wms_msg_set_auto_gw_params
(
  wms_client_id_type              client_id,
  wms_gw_template_s_type          *gw_set_params_ptr    /* IN */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */
}

/*=========================================================================
FUNCTION
  wms_msg_get_auto_cdma_params

DESCRIPTION
  Gets particular CDMA parameters, to be used for MO messaging on the CDMA
  network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Modifies cdma_get_params_ptr directly.

=========================================================================*/
wms_status_e_type wms_msg_get_auto_cdma_params
(
  wms_client_id_type              client_id,
  wms_cdma_template_s_type        *cdma_get_params_ptr  /* OUT */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */
}

/*=========================================================================
FUNCTION
  wms_msg_get_auto_gw_params

DESCRIPTION
  Gets particular GW parameters, to be used for MO messaging on the GW
  network while the format is AUTOMATIC.

DEPENDENCIES
  None

RETURN VALUE
  status codes

SIDE EFFECTS
  Modifies gw_get_params_ptr directly.

=========================================================================*/
wms_status_e_type wms_msg_get_auto_gw_params
(
  wms_client_id_type              client_id,
  wms_gw_template_s_type          *gw_get_params_ptr    /* OUT */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */
}
#endif /*CUST_EDITION*/
/*=========================================================================
FUNCTION
  wms_msg_set_retry_period

DESCRIPTION
  Allow a client to specify the period of allowing retries of sms messages.
  NOTE: if period = 0 seconds, retry will not be attempted.

DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_set_retry_period
(
  wms_client_id_type              client_id,
  uint32                          period     /* in seconds */
)
{
  wms_status_e_type st = WMS_OK_S;

  wms_client_s_type *client_ptr;

  client_ptr = wms_client_ptr(client_id);

  if(client_ptr == NULL || client_ptr->activated == FALSE)
  {
    st = WMS_GENERAL_ERROR_S;
  }
  else if (period > WMS_MAX_RETRY_PERIOD)
  {
    st = WMS_INVALID_PARM_VALUE_S;
  }
  else if (NULL == msg_s_ptr)
  {
     MSG_ERROR("WMS Not Ready - msg_s_ptr is NULL",0,0,0);
     st = WMS_PHONE_NOT_READY_S;
  }
  else
  {
    /* Don't Set Retry Period if GCF Flag is Set */
    boolean gcf_flag = FALSE;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    
    if (gcf_flag == TRUE)
    {
      msg_s_ptr->mo_retry_period = 0;
      MSG_HIGH("Retry Period not set due to GCF Flag Enabled", 0, 0, 0);
    }
    else
    {
      nv_item_type                 nvi; 
      msg_s_ptr->mo_retry_period = period;
      nvi.sms_mo_retry_period    = (uint8)period;

      if( NV_DONE_S != wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) )
      {
        st = WMS_GENERAL_ERROR_S;
      }
      else
      {
        MSG_HIGH("Set Retry Period = %d seconds by Client %d", period, client_id, 0);
      }
    }
  }

  return st;

} /* wms_msg_set_retry_period() */

/*=========================================================================
FUNCTION
  wms_msg_get_retry_period

DESCRIPTION
  Allow a client to retrieve the period of allowing retries of sms messages.
  
DEPENDENCIES
  None

RETURN VALUE
  WMS_OK_S
  WMS_OUT_OF_RESOURCES_S

SIDE EFFECTS
  A command is sent to WMS task.

=========================================================================*/
wms_status_e_type wms_msg_get_retry_period
(
  wms_client_id_type              client_id,
  uint32                          * period     /* in seconds */
)
{
  wms_status_e_type st = WMS_OK_S;

  wms_client_s_type *client_ptr;

  client_ptr = wms_client_ptr(client_id);

  if (period == NULL)
  {
    MSG_HIGH("wms_msg_get_retry_period: NULL ptr",0,0,0);
    MSG_ERROR("period null pointer!",0,0,0);
    st = WMS_NULL_PTR_S;
  }
  else if(client_ptr == NULL || client_ptr->activated == FALSE)
  {
    st = WMS_GENERAL_ERROR_S;
  }
  else if (NULL == msg_s_ptr)
  {
     MSG_ERROR("WMS Not Ready - msg_s_ptr is NULL",0,0,0);
     st = WMS_PHONE_NOT_READY_S;
  }
  else 
  {
    *period = msg_s_ptr->mo_retry_period;
  }

  return st;
}

#ifndef CUST_EDITION
/*=========================================================================
FUNCTION
  wms_set_retry_timer

DESCRIPTION
  This function Sets the Global Retry Timer to tick every second.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_set_retry_timer
(
  void
)
{
  if (rex_get_timer(&wms_retry_timer) == 0)
  {
    /* Set Timer for 1 second */
    (void) rex_set_timer(&wms_retry_timer, 1000);
  }
}

/*=========================================================================
FUNCTION
  wms_msg_process_retry_sig

DESCRIPTION
  This function processed the Retry Signal

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_msg_process_retry_sig
(
  void
)
{
  int i;
  boolean set_timer = FALSE;

  MSG_HIGH( "got WMS_MO_RETRY_TIMER_SIG", 0, 0, 0 );
  (void) rex_clr_sigs( rex_self(), WMS_MO_RETRY_TIMER_SIG );

#ifdef FEATURE_CDSMS
  /* Go through all the pending CDMA MT Records 
  ** and decrement retry_time_left and retry_wait_time
  ** Also check retry_wait_time becoming 0 to send message
  ** and retry_time_left becoming 0 to clear message
  */
  for(i=0; i< WMS_MAX_MT_MESSAGES; i++)
  {
    if (msg_s_ptr->cdma_mt_rec[i].state != WMS_MSG_STATE_IDLE)
    {
      /* Decrement Timers */
      if (msg_s_ptr->cdma_mt_rec[i].retry_time_left > 0)
      {
        if ( msg_s_ptr->cdma_mt_rec[i].state == WMS_MSG_STATE_QUEUED
             && msg_s_ptr->cdma_mt_rec[i].retry_wait_time > 0 )
        {
          msg_s_ptr->cdma_mt_rec[i].retry_wait_time -= 1;
          
          if (msg_s_ptr->cdma_mt_rec[i].retry_wait_time == 0)
          {
            wms_status_e_type     st;

            /* Send the Ack Again */
            st = wms_msg_cdma_ack_msg_i( &msg_s_ptr->cdma_mt_rec[i],
                                         msg_s_ptr->cdma_mt_rec[i].error_class,
                                         msg_s_ptr->cdma_mt_rec[i].tl_status );
    
            if( st == WMS_OK_S )
            {
              MSG_HIGH("ack msg: L2 timer set",0,0,0);
              msg_s_ptr->cdma_mt_rec[i].state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
            }
            else if( st == WMS_NETWORK_NOT_READY_S )
            {
              msg_s_ptr->cdma_mt_rec[i].retry_wait_time = 1; /* Retry after 1 second */
              msg_s_ptr->cdma_mt_rec[i].state           = WMS_MSG_STATE_QUEUED;
            }
            else /* other error */
            {
              MSG_HIGH("ack msg error",0,0,0);
                    
              if(msg_s_ptr->cdma_mt_rec[i].client_id != WMS_CLIENT_TYPE_INTERNAL)
              {
                msg_event_info.ack_report_info.client_id = msg_s_ptr->cdma_mt_rec[i].client_id;
                msg_event_info.ack_report_info.transaction_id =
                  (wms_transaction_id_type)msg_s_ptr->cdma_mt_rec[i].user_data;
                msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
                msg_event_info.ack_report_info.user_data = msg_s_ptr->cdma_mt_rec[i].mt_rec_user_data;
                msg_event_info.ack_report_info.is_success = FALSE;

                (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
              }
            
              wms_msg_cdma_clear_msg_rec( &msg_s_ptr->cdma_mt_rec[i] );
            } /* if st - else */
          }
        }

        msg_s_ptr->cdma_mt_rec[i].retry_time_left -= 1;

        /* Check if retry_wait_time expires */
        if (msg_s_ptr->cdma_mt_rec[i].retry_time_left == 0)
        {
          if(msg_s_ptr->cdma_mt_rec[i].client_id != WMS_CLIENT_TYPE_INTERNAL)
          {
            msg_event_info.ack_report_info.client_id = msg_s_ptr->cdma_mt_rec[i].client_id;
            msg_event_info.ack_report_info.transaction_id =
              (wms_transaction_id_type)msg_s_ptr->cdma_mt_rec[i].user_data;
            msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
            msg_event_info.ack_report_info.user_data = msg_s_ptr->cdma_mt_rec[i].mt_rec_user_data;
            msg_event_info.ack_report_info.is_success = FALSE;

            (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
          }

          wms_msg_cdma_clear_msg_rec(&msg_s_ptr->cdma_mt_rec[i]);
        }
      }
      /* Set timer in use */
      set_timer = TRUE;
    }
  }

  /* Go through all the pending CDMA MO Records 
  ** and decrement retry_time_left and retry_wait_time
  ** Also check retry_wait_time becoming 0 to send message
  ** and retry_time_left becoming 0 to clear message
  */
  for(i=0; i< WMS_MAX_MO_MESSAGES; i++)
  {
    if (msg_s_ptr->cdma_mo_rec[i].state != WMS_MSG_STATE_IDLE)
    {
      /* Decrement Timers */
      if (msg_s_ptr->cdma_mo_rec[i].retry_time_left > 0)
      {
        if ( msg_s_ptr->cdma_mo_rec[i].state == WMS_MSG_STATE_QUEUED
             && msg_s_ptr->cdma_mo_rec[i].retry_wait_time > 0 )
        {
          msg_s_ptr->cdma_mo_rec[i].retry_wait_time -= 1;
          
          if (msg_s_ptr->cdma_mo_rec[i].retry_wait_time == 0)
          {
            wms_status_e_type     st;

            /* Send the Message Again */
            st = wms_msg_cdma_send_sms_OTA( FALSE,   /* not a TL ack */
                                            &msg_s_ptr->cdma_mo_rec[i],
                                             msg_s_ptr->cdma_mo_rec[i].ota,
                                            &msg_s_ptr->cdma_mo_rec[i].address );

            // Send MO SMS Retry Attempt Event
            wms_msg_evt_mo_sms_retry_attempt( (uint32)msg_s_ptr->cdma_mo_rec[i].user_data,
                                              i,
                                              msg_s_ptr->cdma_mo_rec[i].prev_report.report_status,
                                              msg_s_ptr->cdma_mo_rec[i].prev_report.cause_info.tl_status,
                                              msg_s_ptr->cdma_mo_rec[i].retry_time_left );
     
            if( st == WMS_OK_S )
            {
              /* update the msg state of the rec_ptr
              */
              msg_s_ptr->cdma_mo_rec[i].state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
            }
            else if( st == WMS_NETWORK_NOT_READY_S )
            {
              /* reset the interval timer if there is enough time left from the
              ** retry period.
              */
              if((msg_s_ptr->cdma_mo_rec[i].retry_time_left - (sint31)msg_s_ptr->mo_retry_interval) > 0)
              {
                /* Set the retry_wait_time */
                msg_s_ptr->cdma_mo_rec[i].retry_wait_time = msg_s_ptr->mo_retry_interval;
                msg_s_ptr->cdma_mo_rec[i].state = WMS_MSG_STATE_QUEUED;
              }
              else
              {
                msg_event_info.submit_report_info = msg_s_ptr->cdma_mo_rec[i].prev_report;

                if( msg_s_ptr->cdma_mo_rec[i].send_status_to_clients == TRUE )
                {
                  (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                            &msg_event_info);
                }

                wms_msg_cdma_clear_msg_rec(&msg_s_ptr->cdma_mo_rec[i]);
              }
            }
          }
        }
        
        msg_s_ptr->cdma_mo_rec[i].retry_time_left -= 1;
        /* Check if retry_wait_time expires */
        if (msg_s_ptr->cdma_mo_rec[i].retry_time_left == 0)
        {
          msg_event_info.submit_report_info = msg_s_ptr->cdma_mo_rec[i].prev_report;

          if( msg_s_ptr->cdma_mo_rec[i].state == WMS_MSG_STATE_WAIT_FOR_ACK )
          {
            msg_event_info.submit_report_info.report_status = WMS_RPT_NO_ACK;
          }
          
          if( msg_s_ptr->cdma_mo_rec[i].send_status_to_clients == TRUE )
          {
            (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT,
                                       &msg_event_info);
          }

          /* layer2 processing done */
          msg_s_ptr->mc_msg.is_valid = FALSE;

          wms_msg_cdma_clear_msg_rec(&msg_s_ptr->cdma_mo_rec[i]);
        }
      }
      /* Set timer in use */
      set_timer = TRUE;
    }
  }
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  if (set_timer == TRUE)
  {
    wms_set_retry_timer();
  }
}
#endif

void wms_mt_timer(int4 ms_interval, void *user_data_ptr)
{
    boolean set_timer = FALSE;
    wms_msg_cdma_message_record_type  *rec_ptr;
    wms_status_e_type     st;
    
    rec_ptr = (wms_msg_cdma_message_record_type *)user_data_ptr;
    
    clk_dereg(&rec_ptr->clk_timer);
    if (rec_ptr->state == WMS_MSG_STATE_IDLE)
    {
        return;
    }
    
    rec_ptr->retry_time_left--;
    
    /* Decrement Timers */
    if (rec_ptr->retry_time_left > 0)
    {
        switch (rec_ptr->state)
        {// For MT msg: IDLE->WAIT_FOR_ACK->QUEUED->LAYER2_IN_PROGRESS->IDLE
            case WMS_MSG_STATE_LAYER2_IN_PROGRESS:
                MSG_HIGH( "Client didn't ack msg",0,0,0);
        
                st = wms_msg_cdma_ack_msg_i(rec_ptr, WMS_ERROR_TEMP, WMS_TL_NO_ACK_S);
        
                /* don't care about layer2 ack status;
                   don't report ack status to client
                */
                if(st == WMS_OK_S)
                {
                  msg_s_ptr->mc_msg.is_valid = FALSE;
                }
                break;
                
            case WMS_MSG_STATE_WAIT_FOR_ACK:
                MSG_HIGH( "Client didn't ack msg",0,0,0);
                
                st = wms_msg_cdma_ack_msg_i(rec_ptr, rec_ptr->error_class, rec_ptr->tl_status);
                
                if (st == WMS_OK_S)
                {
                    MSG_HIGH("ack msg: L2 timer set",0,0,0);
                    rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
                    set_timer = TRUE;
                }
                else if (st == WMS_NETWORK_NOT_READY_S)
                {
                    rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
                    rec_ptr->state           = WMS_MSG_STATE_QUEUED;
                    set_timer = TRUE;
                }
                else
                {
                    MSG_HIGH("ack msg error",0,0,0);
                }
                break;
                
            default:// WMS_MSG_STATE_QUEUED
                if (rec_ptr->retry_wait_time > 0)
                {
                    rec_ptr->retry_wait_time -= 1;
                    
                    if (rec_ptr->retry_wait_time == 0)
                    {
                        wms_status_e_type     st;
                        
                        /* Send the Ack Again */
                        st = wms_msg_cdma_ack_msg_i(rec_ptr, rec_ptr->error_class, rec_ptr->tl_status);
                        
                        if (st == WMS_OK_S)
                        {
                            MSG_HIGH("ack msg: L2 timer set",0,0,0);
                            rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
                            set_timer = TRUE;
                        }
                        else if (st == WMS_NETWORK_NOT_READY_S)
                        {
                            rec_ptr->retry_wait_time = 1; /* Retry after 1 second */
                            rec_ptr->state           = WMS_MSG_STATE_QUEUED;
                            set_timer = TRUE;
                        }
                        else
                        {
                            MSG_HIGH("ack msg error",0,0,0);
                        }
                    }
                    else
                    {
                        set_timer = TRUE;
                    }
                }
                break;
        }
    }
    
    if (set_timer == TRUE)
    {
        clk_reg2(&rec_ptr->clk_timer, 
                 wms_mt_timer,
                 CLK_MS_PER_SEC,
                 0,
                 FALSE,
                 rec_ptr);
    }
    else
    {
        if (rec_ptr->client_id != WMS_CLIENT_TYPE_INTERNAL)
        {
            msg_event_info.ack_report_info.client_id = rec_ptr->client_id;
            msg_event_info.ack_report_info.transaction_id =
                    (wms_transaction_id_type)rec_ptr->user_data;
            msg_event_info.ack_report_info.message_mode = WMS_MESSAGE_MODE_CDMA;
            msg_event_info.ack_report_info.user_data = rec_ptr->mt_rec_user_data;
            msg_event_info.ack_report_info.is_success = FALSE;
            
            (void)wms_msg_event_notify(WMS_MSG_EVENT_ACK_REPORT, &msg_event_info);
        }
        
        wms_msg_cdma_clear_msg_rec(rec_ptr);
    }
}

void wms_mo_timer(int4 ms_interval, void *user_data_ptr)
{
    boolean set_timer = FALSE;
    wms_msg_cdma_message_record_type  *rec_ptr;
    
    rec_ptr = (wms_msg_cdma_message_record_type *)user_data_ptr;
    
    clk_dereg(&rec_ptr->clk_timer);
    if (rec_ptr->state == WMS_MSG_STATE_IDLE)
    {
        return;
    }
    
    rec_ptr->retry_time_left--;
    
    /* Decrement Timers */
    if (rec_ptr->retry_time_left > 0)
    {
        if (rec_ptr->state == WMS_MSG_STATE_QUEUED
            && rec_ptr->retry_wait_time > 0)
        {
            rec_ptr->retry_wait_time -= 1;
            
            if (rec_ptr->retry_wait_time == 0)
            {
                wms_status_e_type     st;
                
                /* Send the Message Again */
                st = wms_msg_cdma_send_sms_OTA(FALSE,   /* not a TL ack */
                                               rec_ptr,
                                               rec_ptr->ota,
                                               &rec_ptr->address);
                
                // Send MO SMS Retry Attempt Event
                wms_msg_evt_mo_sms_retry_attempt((uint32)rec_ptr->user_data,
                            (rec_ptr-msg_s_ptr->cdma_mo_rec)/sizeof(wms_msg_cdma_message_record_type *),
                            rec_ptr->prev_report.report_status,
                            rec_ptr->prev_report.cause_info.tl_status,
                            rec_ptr->retry_time_left);
                
                if (st == WMS_OK_S)
                {
                    /* update the msg state of the rec_ptr
                    */
                    rec_ptr->state = WMS_MSG_STATE_LAYER2_IN_PROGRESS;
                    set_timer = TRUE;
                }
                else if (st == WMS_NETWORK_NOT_READY_S)
                {
                    /* reset the interval timer if there is enough time left from the
                    ** retry period.
                    */
                    if ((rec_ptr->retry_time_left - msg_s_ptr->mo_retry_interval) > 0)
                    {
                        /* Set the retry_wait_time */
                        rec_ptr->retry_wait_time = msg_s_ptr->mo_retry_interval;
                        rec_ptr->state = WMS_MSG_STATE_QUEUED;
                        set_timer = TRUE;
                    }
                }
            }
            else
            {
                set_timer = TRUE;
            }
        }
        else if(rec_ptr->state == WMS_MSG_STATE_LAYER2_IN_PROGRESS
            && rec_ptr->retry_wait_time > 0
            && !dc_s_ptr->call_active
            && !dc_s_ptr->bInTC
            && !rec_ptr->large_msg)
        {
            set_timer = TRUE;
            rec_ptr->retry_wait_time -= 1;
            
            if (rec_ptr->retry_wait_time == 0)
            {// 改走业务信道
                wms_status_e_type     st;
                
                MSG_HIGH("WMS initiating DC... ", 0,0,0);
    
                st = wms_dc_connect(WMS_CLIENT_TYPE_INTERNAL,
                        NULL,
                        NULL,
                        dc_s_ptr->default_so_from_nv);
                        
                if (st == WMS_OK_S)
                {
                    dc_s_ptr->wms_initiated_call = TRUE;
                    rec_ptr->retry_time_left = WMS_MAX_RETRY_PERIOD;
                   
                    // 注意: 业务信道一旦建立函数 wms_msg_auto_dc_send 会被调用, 故这里给最大等待时间
                    rec_ptr->retry_wait_time = WMS_MAX_RETRY_PERIOD;
                    rec_ptr->state = WMS_MSG_STATE_QUEUED;
                    
                    rec_ptr->large_msg = TRUE;
                }
            }
        }
        else
        {
            set_timer = TRUE;
        }
    }
    else
    {
        if (rec_ptr->state == WMS_MSG_STATE_WAIT_FOR_ACK)
        {
            rec_ptr->prev_report.report_status = WMS_RPT_NO_ACK;
        }
    }
    
    if (set_timer == TRUE)
    {
        clk_reg2(&rec_ptr->clk_timer, 
                 wms_mo_timer,
                 CLK_MS_PER_SEC,
                 0,
                 FALSE,
                 rec_ptr);
    }
    else
    {
        msg_event_info.submit_report_info = rec_ptr->prev_report;
        
        if (rec_ptr->send_status_to_clients == TRUE)
        {
            (void)wms_msg_event_notify(WMS_MSG_EVENT_SUBMIT_REPORT, &msg_event_info);
        }
        
        /* layer2 processing done */
        msg_s_ptr->mc_msg.is_valid = FALSE;
        
        wms_msg_cdma_clear_msg_rec(rec_ptr);
    }    
}


void wms_msg_offline
(
  void
)
{
  uint32    i;

  // Simple function doing cleanup
  // Currently only called when phone goes into LPM
  // If you are changing this, check for any side effects
  // it might cause to LPM when phone wakes up and goes online!

  msg_s_ptr = wms_msg_s_ptr();
#ifndef CUST_EDITION
  (void) rex_clr_timer(&wms_retry_timer);
#endif
#ifdef FEATURE_CDSMS
  for( i=0; i < WMS_MAX_MT_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mt_rec[ i ].state != WMS_MSG_STATE_IDLE )
    {
      wms_msg_cdma_clear_msg_rec( &msg_s_ptr->cdma_mt_rec[ i ] );
    }
  }

  for( i=0; i < WMS_MAX_MO_MESSAGES; i++ )
  {
    if( msg_s_ptr->cdma_mo_rec[ i ].state != WMS_MSG_STATE_IDLE )
    {
      wms_msg_cdma_clear_msg_rec( &msg_s_ptr->cdma_mo_rec[ i ] );
    }
  }
#endif

#ifdef FEATURE_GWSMS
#error code not present
#endif

} /* wms_msg_offline() */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
#ifdef CM_API_PLUS_DIALING
/*===========================================================================

FUNCTION wms_msg_wms_address_to_cm_num

DESCRIPTION
  This local function converts the WMS Address to CM Number

DEPENDENCIES
  None.

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean wms_msg_wms_address_to_cm_num
(  
  wms_address_s_type      *wms_addr,  /* INPUT */ 
  cm_num_s_type           *cm_num     /* OUTPUT */
)
{
  if (NULL == wms_addr || NULL == cm_num)
  {
    return FALSE;
  }

  /* Perform conversion */

  /* Direct Conversion due to Same Enum Values */
  cm_num->digit_mode  = (cm_digit_mode_e_type)wms_addr->digit_mode;
  cm_num->number_type = wms_addr->number_type;
  cm_num->number_plan = wms_addr->number_plan;

  /* Copy Digits over */
  cm_num->len = MIN(wms_addr->number_of_digits, CM_MAX_NUMBER_CHARS);
  cm_num->len = MIN(cm_num->len, WMS_ADDRESS_MAX);
  memcpy(cm_num->buf, wms_addr->digits, cm_num->len);

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_wms_address_from_cm_num

DESCRIPTION
  This local function converts the WMS Address from CM Number

DEPENDENCIES
  None.

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean wms_msg_wms_address_from_cm_num
(  
  cm_num_s_type          *cm_num,    /* INPUT */ 
  wms_address_s_type     *wms_addr   /* OUTPUT */
)
{
  if (NULL == wms_addr || NULL == cm_num)
  {
    return FALSE;
  }

  /* Perform conversion */

  /* Direct Conversion due to Same Enum Values */
  wms_addr->digit_mode  = (wms_digit_mode_e_type)cm_num->digit_mode;
  wms_addr->number_type = (wms_number_type_e_type)cm_num->number_type;
  wms_addr->number_plan = (wms_number_plan_e_type)cm_num->number_plan;
  
  /* Copy Digits over */
  wms_addr->number_of_digits = MIN(cm_num->len, WMS_ADDRESS_MAX);
  memcpy(wms_addr->digits, cm_num->buf, wms_addr->number_of_digits);

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_convert_plus_address

DESCRIPTION
  This local function converts the plus to the appropriate 
  code by calling CM utilities

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  Input Number is modified on success

===========================================================================*/
static boolean wms_msg_convert_plus_address
(  
  wms_address_s_type      *wms_addr  
)
{
  cmutil_num_conv_req_e_type  num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;

  cm_num_details_for_conversion_s_type num_details;
  cm_num_s_type                        converted_num;

  cmutil_conver_result_e_type          conver_result;

  /* Input Parameter Validation */
  if (NULL == wms_addr)
  {
    return FALSE;
  }

  memset (&num_details, 0, sizeof(num_details));
  memset (&converted_num, 0, sizeof(converted_num));

  /* Init num conversion params */
  cm_util_init_num_conversion_params ( &num_conv_req,
                                       &num_details );

  /* Operation to be performed on num */
  num_conv_req = CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS;

  /* Fill the number details before calling conversion */
  num_details.hs_based_plus_dial_setting
     = cfg_s_ptr->hs_based_plus_dial_setting;
  if (CM_INVALID_MOBILE_COUNTRY_CODE == cfg_s_ptr->current_mobile_country_code)
  {
      num_details.current_mobile_country_code
      = cfg_s_ptr->home_mobile_country_code;
  }
  else 
  {
      num_details.current_mobile_country_code 
       = cfg_s_ptr->current_mobile_country_code;
  }
  num_details.home_mobile_country_code
     = cfg_s_ptr->home_mobile_country_code;

  /* Assuming Voice Call for outgoing SMS Messages */
  num_details.call_type = CM_CALL_TYPE_SMS;

  // Convert from WMS Address to CM Number
  if (FALSE == wms_msg_wms_address_to_cm_num(wms_addr, &num_details.num))
  {
    MSG_ERROR (" wms_msg_wms_address_to_cm_num Failed!", 0, 0, 0 );
    return FALSE;
  }

  /* Perform Conversion */
  conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                     &num_details,
                                                     &converted_num );

  if (CMUTIL_CONVER_RESULT_NUM_MODIFIED == conver_result)
  {
    MSG_HIGH("CM num_conversion - Number Changed",0,0,0);

    /* Convert back to WMS Address */
    if (FALSE == wms_msg_wms_address_from_cm_num(&converted_num, wms_addr))
    {
      MSG_ERROR (" wms_msg_wms_address_from_cm_num Failed!", 0, 0, 0 );
      return FALSE;
    }
  }
  else if (CMUTIL_CONVER_RESULT_NUM_UNCHANGED == conver_result)
  {
    MSG_HIGH("CM num_conversion - Number Unchanged",0,0,0);
  }
  else
  {
    MSG_ERROR (" CM num_conversion Failed Status = %d", conver_result, 0, 0 );
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION wms_msg_convert_address_to_plus

DESCRIPTION
  This local function converts the code to the appropriate 
  code by calling CM utilities

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  Input Number is modified on success

===========================================================================*/
static boolean wms_msg_convert_address_to_plus
(  
  wms_address_s_type      *wms_addr  
)
{
  cmutil_num_conv_req_e_type  num_conv_req = CMUTIL_NUM_CONV_REQ_NONE;

  cm_num_details_for_conversion_s_type num_details;
  cm_num_s_type                        converted_num;

  cmutil_conver_result_e_type          conver_result;

  /* Input Parameter Validation */
  if (NULL == wms_addr)
  {
    return FALSE;
  }

  memset (&num_details, 0, sizeof(num_details));
  memset (&converted_num, 0, sizeof(converted_num));

  /* Init num conversion params */
  cm_util_init_num_conversion_params ( &num_conv_req,
                                       &num_details );

  /* Operation to be performed on num */
  num_conv_req = CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS;

  /* Fill the number details before calling conversion */
  num_details.hs_based_plus_dial_setting
     = cfg_s_ptr->hs_based_plus_dial_setting;
  num_details.current_mobile_country_code
     = cfg_s_ptr->current_mobile_country_code;
  num_details.home_mobile_country_code
     = cfg_s_ptr->home_mobile_country_code;

  /* Assuming Voice Call for incoming SMS Messages */
  num_details.call_type = CM_CALL_TYPE_SMS;

  // Convert from WMS Address to CM Number
  if (FALSE == wms_msg_wms_address_to_cm_num(wms_addr, &num_details.num))
  {
    MSG_ERROR (" wms_msg_wms_address_to_cm_num Failed!", 0, 0, 0 );
    return FALSE;
  }

  /* Perform Conversion */
  conver_result  = cm_util_perform_num_conversion (  num_conv_req,
                                                     &num_details,
                                                     &converted_num );

  if (CMUTIL_CONVER_RESULT_NUM_MODIFIED == conver_result)
  {
    MSG_HIGH("CM num_conversion - Number Changed",0,0,0);

    /* Convert back to WMS Address */
    if (FALSE == wms_msg_wms_address_from_cm_num(&converted_num, wms_addr))
    {
      MSG_ERROR (" wms_msg_wms_address_from_cm_num Failed!", 0, 0, 0 );
      return FALSE;
    }
  }
  else if (CMUTIL_CONVER_RESULT_NUM_UNCHANGED == conver_result)
  {
    MSG_HIGH("CM num_conversion - Number Unchanged",0,0,0);
  }
  else
  {
    MSG_ERROR (" CM num_conversion Failed Status = %d", conver_result, 0, 0 );
    return FALSE;
  }

  return TRUE;
}

#endif /* CM_API_PLUS_DIALING */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

void wms_msg_setmochannel(byte usechl)
{
#ifndef FEATURE_OEMOMH 
    g_mochannel = usechl;
#endif
}

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmsmsg.c */
